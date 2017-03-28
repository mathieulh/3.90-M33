#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadexec_kernel.h>

#include <stdio.h>
#include <string.h>


PSP_MODULE_INFO("M33IdCanager_Old_Driver", 0x5006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

static u32 FindProc(const char* szMod, const char* szLib, u32 nid)
{
    struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;

	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
	{
		//***printf("Cannot find module %s\n", szMod);
		return 0;
	}
	
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
	//***printf("entTab %p - entLen %d\n", entTab, entLen);
	while(i < entLen)
    {
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

        if(entry->libname && !strcmp(entry->libname, szLib))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(entry->stubcount > 0)
			{
				for(count = 0; count < entry->stubcount; count++)
				{
					if (vars[count] == nid)
						return vars[count+total];					
				}
			}
		}

		i += (entry->len * 4);
	}

	//***printf("Funtion not found.\n");
	return 0;
}

int ReadFile(char *file, void *buf, int size, int mode)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, mode);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int WriteFile(char *file, void *buf, int size, int mode)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, mode);

	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

/*unsigned char g_keys[16] = 
{
	0x4D, 0x42, 0x5C, 0xCE, 0xA9, 0x3A, 0xF2, 0xC2, 0x0C, 0xFB, 0x1F, 0xF1, 0x20, 0x0B, 0xBE, 0x22
};*/


int SysMemForKernel_419DB8F4(void *);
int sceKernelExitVSHKernel(struct SceKernelLoadExecVSHParam *);

void RebootVSHWithError(u32 error)
{
	struct SceKernelLoadExecVSHParam param;	
	u32 vshmain_args[0x20/4];

	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sceKernelExitVSHKernel(&param);
}

#define RETURN(x) RebootVSHWithError(x); return x
#define RETURN_DELETE_FPL(x) sceKernelDeleteFpl(fpl); return x

int GetLicense(char *filename, u8 *buf)
{
	u32 *header = (u32 *)buf;
	SceUID fd;

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		return fd;
	}

	int res = sceIoRead(fd, header, 0x28);
	if (res != 0x28)
		return -1;

	if (header[0] != 0x50425000)
	{
		return -1;
	}

	res = sceIoLseek(fd, header[0x20/4]+0x560, PSP_SEEK_SET);
	if (res < 0)
		return res;

	res = sceIoRead(fd, buf, 0x30);
	sceIoClose(fd);
	return res;
}

int ReadLicense(char *file, void *buf)
{
	SceIoStat stat; /* sp */
	
	int res = sceIoGetstat(file, &stat);

	if (res < 0)
		return res;

	if (stat.st_size != 152)
	{
		return 0x80010016;
	}

	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, 152);

	sceIoClose(fd);

	if (read < 0)
		return read;

	if (read < 152)
		return 0x80010016;

	return 0;
}

int ReadFlash2Act(void *buf)
{
	SceIoStat stat; /* sp */
	
	int res = sceIoGetstat("flash2:/act.dat", &stat); // Devhook proably fails here

	if (res  < 0)
	{
		if (res == 0x80010002) /* file doesn't exist */ 
			return res;
		else
			return 0x80010327;
	}

	if (stat.st_size != 4152) /* 0x1038 */
	{
		return 0x80010017;
	}

	SceUID fd = sceIoOpen("flash2:/act.dat", PSP_O_RDONLY, 0);

	if (fd < 0)
		return 0x80010328;

	int read = sceIoRead(fd, buf, 4152);

	sceIoClose(fd);

	if (read < 0)
		return read;

	if (read < 4152)
		return 0x80010329;

	return read;
}

int GetOriginalKeys(char *filename, void *keys, char *keyfile)
{
	int res;
	u8 *data = NULL;
	char license_path[64];
	int ( *scePopsMan_driver_92E9E38D_2)(void *);
	int ( * scePspNpDrm_driver_5667B7B9_2)(void *, void *, void*);
	
	SceUID fpl = sceKernelCreateFpl("EcsIDCanager", PSP_MEMORY_PARTITION_KERNEL, 0, 0x1370, 1, NULL);

	if (fpl < 0)
	{
		return 0xCA000002;
	}

	if (sceKernelAllocateFpl(fpl, (void *)&data, NULL) < 0)
	{
		RETURN_DELETE_FPL(0xCA000003);
	}

	memset(data, 0, 0x1360);

	if (GetLicense(filename, data+0x1100) < 0)
	{
		RETURN_DELETE_FPL(0xCA000004);
	}

	strcpy(license_path, "ms0:/PSP/LICENSE/");
	strcat(license_path, (char *)data+0x1100);
	strcat(license_path, ".rif");

	res = ReadLicense(license_path, data+0x1038);
	if (res < 0)
	{
		RETURN_DELETE_FPL(0xCA000005);
	}

	res = ReadFlash2Act(data);
	if (res < 0)
	{
		RETURN_DELETE_FPL(0xCA000006);
	}

	scePspNpDrm_driver_5667B7B9_2 = (void *)FindProc("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x5667B7B9);

	res = scePspNpDrm_driver_5667B7B9_2(data+0x1350, data, data+0x1038);
	if (res < 0)
	{
		RETURN_DELETE_FPL(0xCA000007); 
	}

	WriteFile(keyfile, data+0x1350, 0x10, 0777);
	
	memcpy(keys, data+0x1350, 0x10);
	
	scePopsMan_driver_92E9E38D_2 = (void *)FindProc("scePops_Manager", "scePopsMan_driver", 0x92E9E38D);

	res = scePopsMan_driver_92E9E38D_2(data+0x1350);

	memset(data, 0, 0x1370);
	sceKernelDeleteFpl(fpl);

	return 0;
}

int SetKeys(char *filename, void *thekeys)
{		
	int res;
	char path[64];
	char *p;	
	int ( *scePopsMan_driver_92E9E38D_2)(void *);
		
	strcpy(path, filename);
	p = strrchr(path, '/');

	if (!p)
	{
		RETURN(0xCA000000);
	}

	strcpy(p+1, "KEYS.BIN");

	res = ReadFile(path, thekeys, 0x10, 0);

	if (res != 0x10)
	{
		SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0);

		if (fd >= 0)
		{
			u32 header[0x28/4];

			sceIoRead(fd, header, 0x28);
			sceIoLseek(fd, header[0x20/4], PSP_SEEK_SET);
			sceIoRead(fd, header, 4);
			sceIoClose(fd);

			if (header[0] == 0x464C457F) /* ELF */
			{
				memset(thekeys, 0xDA, 0x10);
				goto SET_KEYS;
			}
		}
		
		res = GetOriginalKeys(filename, thekeys, path);

		if (res < 0)
		{
			RETURN(res);
		}
		else
			return 0;
	}

SET_KEYS:

	scePopsMan_driver_92E9E38D_2 = (void *)FindProc("scePops_Manager", "scePopsMan_driver", 0x92E9E38D);

	res = scePopsMan_driver_92E9E38D_2(thekeys);

	if (res < 0)
	{
		RETURN(0xCA000001);
	}

	return 0;
}

int module_start(SceSize args, void *argp) __attribute__((alias("_start")));

int _start(SceSize args, void *argp)
{
	SysMemForKernel_419DB8F4(SetKeys);
	
	return 0;
}




