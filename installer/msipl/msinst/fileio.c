
/*
	PSP MS boot block installer
*/

#include <windows.h>
#include <ddk/ntddstor.h>
#include <ddk/ntdddisk.h>

#define PHY_DRIVE_STRING "\\\\.\\PHYSICALDRIVE"

static char path[256];

int read_direct(int drv,int sector,int size,void *buf)
{
	HANDLE hndl;
	DWORD read_size;
	int result = -1;
	sprintf(path,"%s%d",PHY_DRIVE_STRING,drv);

//printf("OPEN '%s'\n",path);
if(drv==0) 
{
	printf("Drive 0 failsafe.\n");
	return -1; // fail safe
}

	hndl = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if(hndl==INVALID_HANDLE_VALUE) 
	{
		printf("Cannot open device %s\n", path);
		return -1;
	}

//printf("SEEK 0x%08X\n",sector*0x200);
	if( SetFilePointer(hndl,sector*0x200,NULL,FILE_BEGIN) == sector*0x200)
	{
//printf("RD\n");
		if(ReadFile(hndl,buf,size*0x200,&read_size,NULL) == TRUE)
			result = 0;
		else
			printf("Error in ReadFile.\n");
	}
	else
	{
		printf("Error setting filepointer.\n");
	}
	CloseHandle(hndl);
	return result;
}

int write_direct(int drv,int sector,int size,void *buf)
{
	HANDLE hndl;
	DWORD read_size;
	int result = -1;

	sprintf(path,"%s%d",PHY_DRIVE_STRING,drv);
//printf("OPEN '%s'\n",path);
if(drv==0) 
{
	printf("Drive 0 failsfe.\n");
	return -1;    // fail safe
}

	hndl = CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if(hndl==INVALID_HANDLE_VALUE) 
	{
		printf("Cannot open device %s\n", path);
		return -1;
	}

//printf("SEEK 0x%08X\n",sector*0x200);
	if( SetFilePointer(hndl,sector*0x200,NULL,FILE_BEGIN) == sector*0x200)
	{
//printf("WR %d\n",size*0x200);
		if(WriteFile(hndl,buf,size*0x200,&read_size,NULL) == TRUE)
			result = 0;
		else
			printf("Error in WriteFile.\n");
	}
	else
	{
		printf("Error setting filepointer.\n");
	}
	CloseHandle(hndl);
	return result;
}
