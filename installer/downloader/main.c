#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <psputility.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <psppower.h>
#include <pspgum.h>
#include <psprtc.h>

#include <systemctrl.h>

#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>

PSP_MODULE_INFO("TheDownloader", 0, 1, 0);

PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define printf  pspDebugScreenPrintf

static unsigned int __attribute__((aligned(16))) list[262144];

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2)

#define PROGRAM "ms0:/PSP/GAME/UPDATE/PUPD.PBP"

static int BUF_SIZE;

u8 *memaddr;
int mem_position = 0;
SceUID fd;

int Flush()
{
	if (sceIoWrite(fd, memaddr, mem_position) != mem_position)
		return -1;

	mem_position = 0;
	return 0;
}

int WriteDelayed(void *buf, int size)
{
	if (size + mem_position >= BUF_SIZE)
	{
		if (Flush() < 0)
			return -1;
	}

	memcpy(memaddr+mem_position, buf, size);
	mem_position += size;

	return size;
}

void uSetupGu()
{
	sceGuInit();
	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

char read_buffer[100*1024] __attribute__((aligned(64)));

int uFetchFile(char* updater_url, char* local_url, char *useragent)
{
	int template, connection, request, ret, status, dataend;
	u32 bytes_written, percentage, contentsize;
	u64 tick;	
	u32 start, dif, rate;
	SceUID part;
	int mem;

	if (sceKernelMaxFreeMemSize() >= (30*1024*1024))
	{
		sctrlHENSetMemory(24, 28);
		BUF_SIZE = (28*1024*1024);
	}
	else
	{
		BUF_SIZE = (8*1024*1024);
	}

	part = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_Low, BUF_SIZE+256, NULL);
	if (part < 0)
	{
		printf("Error 0x%08X allocating memory.\n", part);
		return -1;
	}

	memaddr = sceKernelGetBlockHeadAddr(part);
	//printf("Address = 0x%08X\n", (unsigned int)memaddr);

	ret = sceHttpInit(20000);
	if (ret < 0) return 0x12340000;
  
	if(useragent != 0)
	{
		template = sceHttpCreateTemplate(useragent, 1, 1);
	} 
	else 
	{
		template = sceHttpCreateTemplate("M33Update-agent/0.0.1 libhttp/1.0.0", 1, 1);
	}
  
	if (template < 0) 
		return 0x12340001;

	ret = sceHttpSetResolveTimeOut(template, 3000000);
	if (ret < 0) 
		return 0x12340002;

	ret = sceHttpSetRecvTimeOut(template, 60000000);
	if (ret < 0) 
		return 0x12340003;

	ret = sceHttpSetSendTimeOut(template, 60000000);
	if (ret < 0) 
		return 0x12340004;

	connection = sceHttpCreateConnectionWithURL(template, updater_url, 0);
	if (connection < 0) 
		return 0x12340005;

	request = sceHttpCreateRequestWithURL(connection, 0, updater_url, 0);
	if (request < 0) return 0x12340006;

	ret = sceHttpSendRequest(request, 0, 0);
	if (ret < 0) return 0x12340007;

	ret = sceHttpGetStatusCode(request, &status);
	if (ret < 0) return 0x12340008;
	if (status != 200) return 0x12340009;

	ret = sceHttpGetContentLength(request, &contentsize);
	if (ret < 0) return 0x1234000A;

	printf("File size: %d bytes (%d MB)\n", contentsize, contentsize / 1048576);
	dataend = 0;
	bytes_written = 0;

	int x = pspDebugScreenGetX();
	int y = pspDebugScreenGetY();
	u32 oldpercentage = -1;
	percentage = 0;

	fd = sceIoOpen(local_url, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		printf("Error creating file.\n");
		return fd;
	}
	
	sceIoLseek(fd, contentsize, PSP_SEEK_SET);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	sceRtcGetCurrentTick(&tick);
	start = (u32)tick;
	
	while(dataend == 0)
	{
		ret = sceHttpReadData(request, read_buffer, sizeof(read_buffer));
		if (ret < 0)
		{
			sceIoWrite(fd,local_url,4);
			sceIoClose(fd);
			printf("Error 0x%08X in sceHttpReadData.\n", ret);
			return 0x1234000B;
		}
	
		if (ret == 0) 
			dataend = 1;
	
		if (ret > 0)
		{
			bytes_written += ret;
			percentage = bytes_written/contentsize;
			if (WriteDelayed(read_buffer, ret) != ret)
			{
				printf("Memory stick write error.\n");
				break;
			}

			percentage = (u32)((u32)100*(u32)bytes_written)/(u32)contentsize;

			if (percentage != oldpercentage)
			{      
				sceRtcGetCurrentTick(&tick);

				dif = (u32)tick - start;
				dif /= 1000000;
				rate = (dif == 0) ? 0 : (bytes_written/1024)/dif;
				
				pspDebugScreenSetXY(x, y);	
				printf("Downloading... %3u%% (%d KB/s)\n", percentage, rate);
				oldpercentage = percentage;
			}	
	  
			scePowerTick(0);    
		}
	}
	
	pspDebugScreenSetXY(x, y);  
	printf("Downloading... 100%%\n");

	if (Flush() < 0)
	{
		printf("Memory Stick write error.\n");
		return -1;
	}

	sceIoClose(fd);
	printf("File saved (size = %d bytes).\n", bytes_written);
	
	return 0;
}

int uConnectApDialog()
{
	int done = 0;
	pspUtilityNetconfData data;
	u32 language;
	u32 button;

	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &button);


	memset(&data, 0, sizeof(data));
	data.base.size = sizeof(data);
	data.base.language = language;
	data.base.buttonSwap = button;
	data.base.graphicsThread = 17;
	data.base.unknown = 19;
	data.base.fontThread = 18;
	data.base.soundThread = 16;
	data.action = PSP_NETCONF_ACTION_CONNECTAP;

	sceUtilityNetconfInitStart(&data);

	while(!done){
		//Clear the screen...
		sceGuStart(GU_DIRECT, list);
		sceGuClearColor(0xFF000000);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		sceGuFinish();
		sceGuSync(0,0);
		
		switch(sceUtilityNetconfGetStatus()){
			case PSP_UTILITY_DIALOG_NONE:
				break;
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityNetconfUpdate(1);
				break;
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityNetconfShutdownStart();
				done = 1;
				break;
			default:
				break;
		}
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

	return 1;
}

int uLoadNetModules()
{
	int result = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	if (result < 0)
	{
		printf("uLoadNetModules: Error 0x%08X loading pspnet.prx.\n", result);
		return result;
	}

	result = sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
	if (result < 0)
	{
		printf("uLoadNetModules: Error 0x%08X loading pspnet_inet.prx.\n", result);
		return result;
	}

	result = sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI);
	if (result < 0)
	{
		printf("uLoadNetModules: Error 0x%08X loading parseuri.\n", result);
		return result;
	}

	result = sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP);
	if (result < 0)
	{
		printf("uLoadNetModules: Error 0x%08X loading parsehttp.\n", result);
		return result;
	}

	result = sceUtilityLoadNetModule(PSP_NET_MODULE_HTTP);
	if (result < 0)
	{
		printf("uLoadNetModules: Error 0x%08X loading libhttp.prx.\n", result);
		return result;
	}

	return 0;
}

int NetInit()
{
	int

	ret = sceNetInit(0x20000, 42, 0, 42, 0);
	if (ret < 0) {
		//printf("sceNetInit() failed. ret = 0x%x\n", ret);
		return ret;
	}

	ret = sceNetInetInit();
	if (ret < 0) {
		//printf("sceNetInetInit() failed. ret = 0x%x\n", ret);
		//net_term();
		return ret;
	}

	ret = sceNetResolverInit();
	if (ret < 0) {
		//printf("sceNetResolverInit() failed. ret = 0x%x\n", ret);
		//net_term();
		return ret;
	}

	return sceNetApctlInit(0x5400, 48);
}


int main(){
	int ret;

	int UPDATER_DOWNLOAD = 1;
	

	//CHECK FOR UPDATER on Memory Stick. If it is there, set UPDATER_DOWNLOAD to 0!
	//
	//
	//
	//

	
	if (UPDATER_DOWNLOAD == 1){
		
		uLoadNetModules();	
		NetInit();
		
		uSetupGu();		

		//Clear the screen...
		sceGuStart(GU_DIRECT, list);
		sceGuClearColor(0xFF0000cc);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		sceGuFinish();
		sceGuSync(0,0);
	
		uConnectApDialog();
	}

	sceKernelDelayThread(400000);
	pspDebugScreenInit();

    
	pspDebugScreenClear();
	
	//DON'T PRINT anything to screen before here - if UPDATER_DOWNLOAD is enabled.
	//
	//
	//
	
	if (UPDATER_DOWNLOAD == 1){
		ret = uFetchFile("http://updates.lan.st/3.90/EBOOT.PBP", "ms0:/PSP/GAME/UPDATE/390.PBP", 0);
	  	if (ret != 0)
		{
	          pspDebugScreenPrintf("uFetchFile: Error 0x%08X\n", ret);
			  sceKernelDelayThread(5*1000*1000);
			  sceKernelExitGame();
			  sceKernelSleepThread();
		} else 
		{
		  //pspDebugScreenPrintf("uFetchFile: File successfully downloaded");
		}
	}

	printf("\nReturning to M33 updater...\n");
	sceDisplaySetHoldMode(1);
		
	struct SceKernelLoadExecVSHParam param;

	memset(&param, 0, sizeof(param));			
	param.size = sizeof(param);
	param.args = strlen(PROGRAM)+1;
	param.argp = PROGRAM;
	param.key = "updater";

	sctrlKernelLoadExecVSHMs1(PROGRAM, &param);
	
      	
	//DO INSTALL SHIT, such as extracting the PRXs from the EBOOT in which you are - as it is running in USERMODE
	// You can also download the updater modules from dark-alex.org and load them :P Up to you.
	// The network will still be initialized at this point. If you need more memory, you'll have to 
	// unload the network modules and kill the netconf utility ^^
	//
	
	sceKernelExitGame();
        return 0;
}
