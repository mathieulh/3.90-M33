/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * host_driver.c - PSPLINK HostFS IO driver
 *
 * Copyright (c) 2006 James Forshaw <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.ps2dev.org/psp/trunk/psplink/usbhostfs/host_driver.c $
 * $Id: host_driver.c 1954 2006-06-28 17:54:29Z tyranid $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "usbhostfs.h"

static int io_init(PspIoDrvArg *arg)
{
	/* Nothing to do */
	return 0;
}

static int io_exit(PspIoDrvArg *arg)
{
	/* Nothing to do */
	return 0;
}

static int io_open(PspIoDrvFileArg *arg, char *file, int mode, SceMode mask)
{
	int ret = -1;
	struct HostFsOpenCmd cmd;
	struct HostFsOpenResp resp;

	if(file == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_OPEN;
	cmd.cmd.extralen = strlen(file)+1;
	cmd.mode = mode;
	cmd.mask = mask;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), file, strlen(file)+1, NULL, 0))
		{
			/* Set the resultant fid into the arg structure */
			if(resp.res >= 0)
			{
				arg->arg = (void *) (resp.res);
				ret = 0;
			}
			else
			{
				ret = resp.res;
			}

			DEBUG_PRINTF("Returned fid %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending open command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_close(PspIoDrvFileArg *arg)
{
	int ret = -1;
	struct HostFsCloseCmd cmd;
	struct HostFsCloseResp resp;

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_CLOSE;
	cmd.cmd.extralen = 0;
	cmd.fid = (int) (arg->arg);

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, NULL, 0))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending close command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

int usb_read_data(int fd, void *data, int len)
{
	struct HostFsReadCmd cmd;
	struct HostFsReadResp resp;
	int blocks;
	int residual;
	int ret = 0;

	if(len < 0)
	{
		MODPRINTF("Invalid read length (%d)\n", len);
		return -1;
	}

	if(len == 0)
	{
		return 0;
	}

	if(data == NULL)
	{
		MODPRINTF("NULL data pointer\n");
		return -1;
	}

	blocks = len / HOSTFS_MAX_BLOCK;
	residual = len % HOSTFS_MAX_BLOCK;

	while(blocks > 0)
	{
		memset(&cmd, 0, sizeof(cmd));
		memset(&resp, 0, sizeof(resp));
		cmd.cmd.magic = HOSTFS_MAGIC;
		cmd.cmd.command = HOSTFS_CMD_READ;
		cmd.cmd.extralen = 0;
		cmd.len = HOSTFS_MAX_BLOCK;
		cmd.fid = fd;

		if(usb_connected())
		{
			if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, data, HOSTFS_MAX_BLOCK))
			{
				DEBUG_PRINTF("Read: Returned result %d\n", resp.res);
				if(resp.res > 0)
				{
					data += resp.res;
					ret += resp.res;
				}
				else
				{
					/* If we had an error straight away */
					if((resp.res < 0) && (ret == 0))
					{
						ret = resp.res;
					}

					/* Otherwise just return how much we managed to read */
					break;
				}
			}
			else
			{
				MODPRINTF("Error in sending read command\n");
				ret = -1;
				break;
			}
		}
		else
		{
			MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
			ret = -1;
			break;
		}

		blocks--;
	}

	do
	{
		if((blocks == 0) && (residual > 0))
		{
			memset(&cmd, 0, sizeof(cmd));
			memset(&resp, 0, sizeof(resp));
			cmd.cmd.magic = HOSTFS_MAGIC;
			cmd.cmd.command = HOSTFS_CMD_READ;
			cmd.cmd.extralen = 0;
			cmd.len = residual;
			cmd.fid = fd;

			if(usb_connected())
			{
				if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, data, residual))
				{
					DEBUG_PRINTF("Read: Returned result %d\n", resp.res);
					if(resp.res > 0)
					{
						data += resp.res;
						ret += resp.res;
					}
					else
					{
						/* If we had an error straight away */
						if((resp.res < 0) && (ret == 0))
						{
							ret = resp.res;
						}

						/* Otherwise just return how much we managed to read */
						break;
					}
				}
				else
				{
					MODPRINTF("Error in sending read command\n");
					ret = -1;
					break;
				}
			}
			else
			{
				MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
				ret = -1;
				break;
			}
		}
	}
	while(0);

	return ret;
}

static int io_read(PspIoDrvFileArg *arg, char *data, int len)
{
	DEBUG_PRINTF("read: arg %p, data %p, len %d\n", arg, data, len);

	return usb_read_data((int) arg->arg, data, len);
}

int usb_write_data(int fd, const void *data, int len)
{
	struct HostFsWriteCmd cmd;
	struct HostFsWriteResp resp;
	int blocks;
	int residual;
	int ret = 0;

	if(len < 0)
	{
		MODPRINTF("Invalid write length (%d)\n", len);
		return -1;
	}

	if(len == 0)
	{
		return 0;
	}

	if(data == NULL)
	{
		MODPRINTF("NULL data pointer\n");
		return -1;
	}


	blocks = len / HOSTFS_MAX_BLOCK;
	residual = len % HOSTFS_MAX_BLOCK;

	while(blocks > 0)
	{
		memset(&cmd, 0, sizeof(cmd));
		memset(&resp, 0, sizeof(resp));
		cmd.cmd.magic = HOSTFS_MAGIC;
		cmd.cmd.command = HOSTFS_CMD_WRITE;
		cmd.cmd.extralen = HOSTFS_MAX_BLOCK;
		cmd.fid = fd;

		if(usb_connected())
		{
			if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), data, HOSTFS_MAX_BLOCK, NULL, 0))
			{
				DEBUG_PRINTF("Write: Returned result %d\n", resp.res);
				if(resp.res > 0)
				{
					data += resp.res;
					ret += resp.res;
				}
				else
				{
					/* If we had an error straight away */
					if((resp.res < 0) && (ret == 0))
					{
						ret = resp.res;
					}

					/* Otherwise just return how much we managed to write */

					break;
				}
			}
			else
			{
				MODPRINTF("Error in sending write command\n");
				ret = -1;
				break;
			}
		}
		else
		{
			MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
			ret = -1;
			break;
		}

		blocks--;
	}

	do
	{
		if((blocks == 0) && (residual > 0))
		{
			memset(&cmd, 0, sizeof(cmd));
			memset(&resp, 0, sizeof(resp));
			cmd.cmd.magic = HOSTFS_MAGIC;
			cmd.cmd.command = HOSTFS_CMD_WRITE;
			cmd.cmd.extralen = residual;
			cmd.fid = fd;

			if(usb_connected())
			{
				if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), data, residual, NULL, 0))
				{
					DEBUG_PRINTF("Write: Returned result %d\n", resp.res);
					if(resp.res > 0)
					{
						ret += resp.res;
					}
					else
					{
						/* If we had an error straight away */
						if((resp.res < 0) && (ret == 0))
						{
							ret = resp.res;
						}

						/* Otherwise just return how much we managed to write */
						break;
					}
				}
				else
				{
					MODPRINTF("Error in sending write command\n");
					ret = -1;
					break;
				}
			}
			else
			{
				MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
				ret = -1;
				break;
			}
		}
	}
	while(0);

	return ret;
}

static int io_write(PspIoDrvFileArg *arg, const char *data, int len)
{
	DEBUG_PRINTF("write: arg %p, data %p, len %d\n", arg, data, len);

	return usb_write_data((int) arg->arg, data, len);
}

static SceOff io_lseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	SceOff ret = -1;
	struct HostFsLseekCmd cmd;
	struct HostFsLseekResp resp;

	DEBUG_PRINTF("lseek: ofs %d, whence %d\n", (int) ofs, whence);

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_LSEEK;
	cmd.fid = (int) (arg->arg);
	cmd.ofs = ofs;
	cmd.whence = whence;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, NULL, 0))
		{
			if(resp.res >= 0)
			{
				ret = resp.ofs;
			}

			DEBUG_PRINTF("Lseek returned res %d\n", ret);
		}
		else
		{
			MODPRINTF("Error in sending lseek command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_ioctl(PspIoDrvFileArg *arg, unsigned int cmdno, void *indata, int inlen, void *outdata, int outlen)
{
	/* Do nothing atm */
	int ret = -1;
	struct HostFsIoctlCmd cmd;
	struct HostFsIoctlResp resp;

	/* Ensure our lengths are zeroed */
	if(indata == NULL)
	{
		inlen = 0;
	}

	if(outdata == NULL)
	{
		outlen = 0;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_IOCTL;
	cmd.cmd.extralen = inlen;
	cmd.cmdno = cmdno;
	cmd.fid = (int) (arg->arg);
	cmd.outlen = outlen;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), indata, inlen, outdata, outlen))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending ioctl command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_remove(PspIoDrvFileArg *arg, const char *name)
{
	int ret = -1;
	struct HostFsRemoveCmd cmd;
	struct HostFsRemoveResp resp;

	if(name == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_REMOVE;
	cmd.cmd.extralen = strlen(name)+1;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), name, strlen(name)+1, NULL, 0))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending remove command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_mkdir(PspIoDrvFileArg *arg, const char *name, SceMode mode)
{
	int ret = -1;
	struct HostFsMkdirCmd cmd;
	struct HostFsMkdirResp resp;

	if(name == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_MKDIR;
	cmd.cmd.extralen = strlen(name)+1;
	cmd.mode = mode;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), name, strlen(name)+1, NULL, 0))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending mkdir command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_rmdir(PspIoDrvFileArg *arg, const char *name)
{
	int ret = -1;
	struct HostFsRmdirCmd cmd;
	struct HostFsRmdirResp resp;

	if(name == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_RMDIR;
	cmd.cmd.extralen = strlen(name)+1;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), name, strlen(name)+1, NULL, 0))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending rmdir command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_dopen(PspIoDrvFileArg *arg, const char *dir)
{
	int ret = -1;
	struct HostFsDopenCmd cmd;
	struct HostFsDopenResp resp;

	if(dir == NULL)
	{
		MODPRINTF("Invalid dir name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_DOPEN;
	cmd.cmd.extralen = strlen(dir)+1;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), dir, strlen(dir)+1, NULL, 0))
		{
			/* Set the resultant did into the arg structure */
			if(resp.res >= 0)
			{
				arg->arg = (void *) (resp.res);
				ret = 0;
			}
			else
			{
				ret = resp.res;
			}

			DEBUG_PRINTF("Returned did %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending dopen command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_dclose(PspIoDrvFileArg *arg)
{
	int ret = -1;
	struct HostFsDcloseCmd cmd;
	struct HostFsDcloseResp resp;

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_DCLOSE;
	cmd.cmd.extralen = 0;
	cmd.did = (int) (arg->arg);

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, NULL, 0))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned result %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending dclose command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_dread(PspIoDrvFileArg *arg, SceIoDirent *dir)
{
	int ret = -1;
	struct HostFsDreadCmd cmd;
	struct HostFsDreadResp resp;

	if(dir == NULL)
	{
		MODPRINTF("Invalid dir pointer (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_DREAD;
	cmd.cmd.extralen = 0;
	cmd.did = (int) (arg->arg);

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, dir, sizeof(SceIoDirent)))
		{
			DEBUG_PRINTF("Dread: Returned result %d\n", resp.res);
			ret = resp.res;
		}
		else
		{
			MODPRINTF("Error in sending read command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_getstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	int ret = -1;
	struct HostFsGetstatCmd cmd;
	struct HostFsGetstatResp resp;

	if(file == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	if(stat == NULL)
	{
		MODPRINTF("Invalid stat pointer\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_GETSTAT;
	cmd.cmd.extralen = strlen(file)+1;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), file, strlen(file)+1, stat, sizeof(SceIoStat)))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending getstat command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_chstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat, int bits)
{
	int ret = -1;
	struct HostFsChstatCmd cmd;
	struct HostFsChstatResp resp;

	if(file == NULL)
	{
		MODPRINTF("Invalid file name (NULL)\n");
		return -1;
	}

	if(stat == NULL)
	{
		MODPRINTF("Invalid stat pointer (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_CHSTAT;
	cmd.cmd.extralen = strlen(file)+1;
	cmd.bits = bits;
	cmd.mode = stat->st_mode;
	cmd.size = stat->st_size;
	cmd.atime.year = stat->st_atime.year;
	cmd.atime.month = stat->st_atime.month;
	cmd.atime.day = stat->st_atime.day;
	cmd.atime.hour = stat->st_atime.hour;
	cmd.atime.minute = stat->st_mtime.minute;
	cmd.atime.second = stat->st_mtime.second;
	cmd.mtime.year = stat->st_mtime.year;
	cmd.mtime.month = stat->st_mtime.month;
	cmd.mtime.day = stat->st_mtime.day;
	cmd.mtime.hour = stat->st_mtime.hour;
	cmd.mtime.minute = stat->st_mtime.minute;
	cmd.mtime.second = stat->st_mtime.second;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), file, strlen(file)+1, NULL, 0))
		{
			/* Set the resultant did into the arg structure */
			ret = resp.res;

			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending chstat command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_rename(PspIoDrvFileArg *arg, const char *oldname, const char *newname)
{
	char buf[HOSTFS_RENAME_BUFSIZE];
	int ret = -1;
	struct HostFsRenameCmd cmd;
	struct HostFsRenameResp resp;
	int size;

	if((oldname == NULL) || (newname == NULL))
	{
		MODPRINTF("Invalid file names for rename command\n");
		return -1;
	}

	size = strlen(oldname) + strlen(newname) + 2;
	if(size > sizeof(buf))
	{
		MODPRINTF("Not enough buffer space for rename command, size %d, bufsize %d\n", size, sizeof(buf));
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_RENAME;
	cmd.cmd.extralen = size;
	cmd.fsnum = arg->fs_num;

	/* Fill buffer */
	strcpy(buf, oldname);
	strcpy(buf+strlen(oldname)+1, newname);

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), buf, size, NULL, 0))
		{
			/* Set the resultant fid into the arg structure */
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending rename command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_chdir(PspIoDrvFileArg *arg, const char *dir)
{
	int ret = -1;
	struct HostFsChdirCmd cmd;
	struct HostFsChdirResp resp;

	if(dir == NULL)
	{
		MODPRINTF("Invalid dir name (NULL)\n");
		return -1;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_CHDIR;
	cmd.cmd.extralen = strlen(dir)+1;
	cmd.fsnum = arg->fs_num;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), dir, strlen(dir)+1, NULL, 0))
		{
			ret = resp.res;

			DEBUG_PRINTF("Returned %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending chdir command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_mount(PspIoDrvFileArg *arg)
{
	/* Do nothing atm */
	return -1;
}

static int io_umount(PspIoDrvFileArg *arg)
{
	/* Do nothing atm */
	return -1;
}

static int io_devctl(PspIoDrvFileArg *arg, const char *name, unsigned int cmdno, void *indata, int inlen, void *outdata, int outlen)
{
	int ret = -1;
	struct HostFsDevctlCmd cmd;
	struct HostFsDevctlResp resp;

	if(name == NULL)
	{
		MODPRINTF("Invalid name (NULL)\n");
		return -1;
	}

	/* Handle the get info devctl */
	if(cmdno == DEVCTL_GET_INFO)
	{
		void **p = (void **) indata;
		if((p) && (*p))
		{
			outdata = *p;
			outlen = sizeof(struct DevctlGetInfo);
			indata = NULL;
			inlen = 0;
		}
		else
		{
			return -1;
		}
	}

	/* Ensure our lengths are zeroed */
	if(indata == NULL)
	{
		inlen = 0;
	}

	if(outdata == NULL)
	{
		outlen = 0;
	}

	memset(&cmd, 0, sizeof(cmd));
	memset(&resp, 0, sizeof(resp));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_DEVCTL;
	cmd.cmd.extralen = inlen;
	cmd.cmdno = cmdno;
	cmd.fsnum = arg->fs_num;
	cmd.outlen = outlen;

	if(usb_connected())
	{
		if(command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), indata, inlen, outdata, outlen))
		{
			ret = resp.res;
			DEBUG_PRINTF("Returned res %d\n", resp.res);
		}
		else
		{
			MODPRINTF("Error in sending devctl command\n");
		}
	}
	else
	{
		MODPRINTF("%s: Error PC side not connected\n", __FUNCTION__);
	}

	return ret;
}

static int io_unknown(PspIoDrvFileArg *arg)
{
	/* Do nothing atm */
	MODPRINTF("Unknown command called\n");
	return -1;
}

static PspIoDrvFuncs host_funcs = 
{
	io_init,
	io_exit,
	io_open,
	io_close,
	io_read,
	io_write,
	io_lseek,
	io_ioctl,
	io_remove,
	io_mkdir,
	io_rmdir,
	io_dopen,
	io_dclose,
	io_dread,
	io_getstat,
	io_chstat,
	io_rename,
	io_chdir,
	io_mount,
	io_umount,
	io_devctl,
	io_unknown,
};

static PspIoDrv host_driver = 
{
	"host", 0x10, 0x800, "HOST", &host_funcs
};

int hostfs_init(void)
{
	int ret;

	(void) sceIoDelDrv("host"); /* Ignore error */
	ret = sceIoAddDrv(&host_driver);
	if(ret < 0)
	{
		return ret;
	}

	return 0;
}

void hostfs_term(void)
{
	(void) sceIoDelDrv("host");
}
