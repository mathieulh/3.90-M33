/*
	PSP MS boot block installer
*/

#include <windows.h>
#include <winioctl.h>

#include <stdio.h>
#include "fileio.h"

unsigned char code_buf[1024*1024];
unsigned char sector_buf[0x200];

#define MAX_MEDIA 10

static char path[256];

int ValidateAndGetPhisycalDrive(char letter)
{
	VOLUME_DISK_EXTENTS extents;
	HANDLE hndl;
	DWORD  bytesReturned;
	int result;
	
	sprintf(path, "%c:\\", letter);

	if (GetDriveType(path) != DRIVE_REMOVABLE)
	{
		printf("Drive %c is invalid or not removable\n", letter);
		return -1;
	}

	sprintf(path, "\\\\.\\%c:", letter);
	hndl = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if(hndl == INVALID_HANDLE_VALUE) 
	{
		printf("Cannot open logical drive.\n");
		return -1;
	}

	result = DeviceIoControl(hndl, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &extents, sizeof(extents), &bytesReturned, NULL);
	if (GetLastError() == ERROR_MORE_DATA || extents.NumberOfDiskExtents != 1)
	{
		printf("Logical drive has more than a physical drive. Cannot handle it.\n");
		result = -1;
	}
	else if (result == 0)
	{
		printf("Error 0x%08X in DeviceIoControl.\n", result);
		result = -1;
	}
	else
	{
		result = extents.Extents[0].DiskNumber;
	}

	CloseHandle(hndl);
	return result;
}


int main(int argc,char **argv)
{
	int abs_sec,ttl_sec;
	int signature;
	int type;
	int Drv;
	int file_size;
	int file_block;
	int free_block;
	FILE *fp;
	char *code_fname;
	int i;
	int ans;

	printf("PSP MS IPL Installer\n");

	if(argc != 3 || strlen(argv[1]) != 1)
	{
		printf("usage: %s driveletter ipl_block_file\n", argv[0]);
		return -1;
	}

	// load IPL data
	code_fname = argv[2];
	printf("Load IPL code %s\n",code_fname);
	fp = fopen(code_fname,"rb");
	if(fp==NULL)
	{
		printf("Can't open %s\n",code_fname);
		return -1;
	}
	file_size = fread(code_buf,1,sizeof(code_buf),fp);
	fclose(fp);

	if(file_size<0)
	{
		printf("read error\n");
		return -1;
	}
	file_block = (file_size+0xfff)/0x1000;
	printf("%d bytes(%d block) readed\n",file_size,file_block);
	/*if(file_block >1)
	{
		printf("Too large code size\n");
		return -1;
	}*/

	Drv = ValidateAndGetPhisycalDrive(argv[1][0]);
	if(Drv<0) return -1;

	printf("\nTarget DRIVE is %d\n",Drv);


	// read PARTBLOCK
	printf("Check partation Sector\n");
	if( read_direct(Drv,0x00,1,sector_buf) < 0)
	{
		printf("Can't read partation sector\n");
		return -1;
	}

	abs_sec   = sector_buf[0x1c6]|(sector_buf[0x1c7]<<8)|(sector_buf[0x1c8]<<16)|(sector_buf[0x1c9]<<24);
	ttl_sec   = sector_buf[0x1ca]|(sector_buf[0x1cb]<<8)|(sector_buf[0x1cc]<<16)|(sector_buf[0x1cd]<<24);
	signature = sector_buf[0x1fe]|(sector_buf[0x1ff]<<8);
	type      = sector_buf[0x1c2];

#if 1
	printf("boot status        0x%02X\n",sector_buf[0x1be]);
	printf("start head         0x%02X\n",sector_buf[0x1bf]);
	printf("start sec/cyl    0x%04X\n",sector_buf[0x1c0]|(sector_buf[0x1c1]<<8));
	printf("partation type     0x%02X\n",type);
	printf("last head          0x%02X\n",sector_buf[0x1c3]);
	printf("last sec/cyl     0x%04X\n",sector_buf[0x1c4]|(sector_buf[0x1c5]<<8));
	printf("abs sector   0x%08X\n",abs_sec);
	printf("ttl sector   0x%08X\n",ttl_sec);
	printf("signature        0x%04X\n",signature);
#endif
	if(signature!=0xaa55)
	{
		printf("Bad Signature\n");
		return -1;
	}

	// check BPB
	printf("Check BPB Sector\n");
	if( read_direct(Drv,abs_sec,1,sector_buf) < 0)
	{
		printf("Can't read BPB sector\n");
		return -1;
	}
	signature = sector_buf[0x1fe]|(sector_buf[0x1ff]<<8);
	printf("signature        %04X\n",signature);
	if(signature!=0xaa55)
	{
		printf("Bad Signature\n");
		return -1;
	}

	// check free space
	printf("Check free reserved sector:");
	free_block = (abs_sec-0x10)/8;
	if( free_block < file_block)
	{
		printf("to small reserved sectors\n");
		return -1;
	}
	printf("OK\n");

	printf("Write ABS Sector 0x%X to 0x%X\n",0x10,0x10 + (file_block*8)-1);
	printf("Are You Sure ?[Y]");
	ans = getchar() | 0x20;
	printf("\n");
	if(ans!='y')
	{
		printf("Canceled\n");
		return -1;
	}

	printf("Write MS BOOT CODE\n");
	// write data
	if( write_direct(Drv,0x10,file_block*8,code_buf) < 0)
	{
		printf("Can't write code\n");
		return -1;
	}

#if 0
	for(i=0;i<0x1ff;i++)
	{
		printf("%02X ",sector_buf[i]);
	}
#endif

	return 0;
}
