#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsuspend.h>
#include <psppower.h>
#include <psprtc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

#include <kubridge.h>


PSP_MODULE_INFO("Kicker", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define printf    pspDebugScreenPrintf

/*int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	if (seek > 0)
	{
		if (sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
		{
			sceIoClose(fd);
			return -1;
		}
	}

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
}*/

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int LoadStartModule(char *file)
{
	SceUID mod = kuKernelLoadModule(file, 0, NULL);
	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

u8 header[0x100];
u8 act[0x1038];
u8 table[4*1024*1024];
u8 buf1[1*1024*1024];
u8 buf2[1*1024*1024];

char str1[50], str2[50];

char *formattime(int seconds, char *str)
{
	int hours;
	int minutes;
	
	minutes = seconds / 60;
	seconds = seconds % 60;
	
	hours = minutes / 60;
	minutes = minutes % 60;
	
	sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds);
	return str;
}

int main(void)
{
    int res, table_size;

	pspDebugScreenInit();

	pspDebugScreenSetTextColor(0x000000FF);
	printf("NP Decryptor by CipherUpdate & kono.\n\n");
	pspDebugScreenSetTextColor(0x00FFFFFF);

	if (LoadStartModule("flash0:/kd/npdrm.prx") < 0)
	{
		printf("Error loading module npdrm");
		sceKernelDelayThread(4000000);
		sceKernelExitGame();
	}

	if (LoadStartModule("npeg.prx") < 0)
	{
		printf("Error loading module npeg");
		sceKernelDelayThread(4000000);
		sceKernelExitGame();
	}

	printf("Modules loaded.\n");

	res = NpegOpen("NP.PBP", header, act, table, &table_size);
	printf("NpegOpen = %d\n", res);

	if (res >= 0)
	{
		int i, block_size, nblocks, iso_size;
		
		WriteFile("header.bin", header, 0x100);
		WriteFile("lookup_table.bin", table, table_size);

		block_size = *(u32 *)&header[0x0C] * 0x800;
		nblocks = table_size / 32;

		iso_size = *(u32 *)&header[0x64] - *(u32 *)&header[0x54] + 1;
		iso_size *= 0x800;

		printf("Dumped header and lookup_table.\n\n");
		printf("ISO will be %d MB. Do you want to write it? (X = yes, O = exit)\n", iso_size / 1048576);

		while (1)
		{
			SceCtrlData pad;

			sceCtrlReadBufferPositive(&pad, 1);

			if (pad.Buttons & PSP_CTRL_CROSS)
				break;

			else if (pad.Buttons & PSP_CTRL_CIRCLE)
				sceKernelExitGame();

			sceKernelDelayThread(8000);
		}

		int x, y;
		u32 start;
		u64 tick;

		x = pspDebugScreenGetX();
		y = pspDebugScreenGetY();

		char name[64];

		sprintf(name, "ms0:/ISO/%s.iso", (char *)header+0x70);

		SceUID fd = sceIoOpen(name, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		if (fd < 0)
		{
			printf("Error creating %s - 0x%08X\n", name, fd);
			goto EXIT;
		}

		sceRtcGetCurrentTick(&tick);
		start = (u32)tick;
		
		for (i = 0; i < nblocks; i++)
		{
			u32 dif;
			u32 written;
			u32 remwrite;
			int rate;
			int estimated;
			
			res = NpegReadBlock(buf1, buf2, i);
			if (res <= 0)
			{
				printf("Error %d reading block %d\n", res, i);
				sceIoClose(fd);
				sceIoRemove("dump.iso");
				goto EXIT;
			}

			sceIoWrite(fd, buf2, res);

			if ((i % 0x10) == 0)
			{			
				pspDebugScreenSetXY(x, y);				
				sceRtcGetCurrentTick(&tick);

				dif = (u32)tick - start;
				dif /= 1000000;
				written = ((block_size * i) / 1024);
				remwrite = ((nblocks - i) * block_size) / 1024;

				rate = (dif == 0) ? 0 : written/dif;
				estimated = (rate == 0) ? 0 : remwrite / rate;
				
				printf("Dumping... %3d%% (%d KB/s, elapsed: %s, remaining: %s)\n", (100 * i) / nblocks,  rate, formattime(dif, str1), formattime(estimated, str2));
			}

			if (i == (nblocks-1))
			{
				pspDebugScreenSetXY(x, y);
			}

			scePowerTick(0);
		}

		sceIoClose(fd);
		NpegClose();	

		printf("\n\n");
	}
	else
	{
		printf("Error!\n");
	}

EXIT:

	printf("Terminated. Press a key to exit.\n");

	int keys = PSP_CTRL_SELECT | PSP_CTRL_START | PSP_CTRL_UP | PSP_CTRL_LEFT | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE | PSP_CTRL_CROSS | PSP_CTRL_SQUARE | PSP_CTRL_CIRCLE;
	
	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & keys)
			break;

		sceKernelDelayThread(12000);
	}

	sceKernelExitGame();

    return 0;
}

