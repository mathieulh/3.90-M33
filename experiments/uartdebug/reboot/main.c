#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <string.h>

PSP_MODULE_INFO("rebooth", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

u32 FindProc(const char* szMod, const char* szLib, u32 nid)
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

int myfinalize()
{
	//Kprintf("Entering my finalize...!!\n");
	pspDebugSioPuts("hola!!!");
	u8 *src = (void *)0xbfc00040;
	u8 *dst = (void *)0x883e0000;
	int i;

	for (i = 0; i < 0x1000; i++)
	{
		//Kprintf("%02X\n", src[i]);
		pspDebugSioPutchar(src[i]);
		//dst[i] = 0xFF;
	}

	while (1);

	return 0;
}

void cache()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int main_thread(SceSize args, void *argp)
{
	pspDebugSioInit();
	pspDebugSioSetBaud(115200);
	pspDebugSioPuts("Test.\n");
	sceKernelDelayThread(2*1000*1000);
	Kprintf("Rebooting in 10 seconds...\n");

	u32 *mod = (u32 *)sceKernelFindModuleByName("scePower_Service");

	u32 text_addr = *(mod+27);

	//_sw(NOP, text_addr+0x1FA0);
	//MAKE_CALL(text_addr+0x1EF0, myfinalize);
	//*MAKE_CALL(text_addr+0x17f4, myfinalize);
	//*MAKE_CALL(text_addr+0x18b4, myfinalize);
	//*MAKE_CALL(text_addr+0x190c, myfinalize);
	//*MAKE_CALL(text_addr+0x1928, myfinalize);
	//*MAKE_CALL(text_addr+0x1940, myfinalize);
	//*MAKE_CALL(text_addr+0x19ac, myfinalize);
	//BMAKE_CALL(text_addr+0x1a5c, myfinalize);
	//*MAKE_CALL(text_addr+0x1ae8, myfinalize);
	//*MAKE_CALL(text_addr+0x1b4c, myfinalize);
	//BMAKE_CALL(text_addr+0x1b64, myfinalize);
	//*MAKE_CALL(text_addr+0x1b78, myfinalize);
	//BNTMAKE_CALL(text_addr+0x1b9c, myfinalize);
	_sw(NOP, text_addr+0x2500); /* Patch the disable of uart */
	//*MAKE_CALL(text_addr+0x1bac, myfinalize);
	//*MAKE_CALL(text_addr+0x1cd0, myfinalize);
	//NTMAKE_CALL(text_addr+0x1e58, myfinalize);
	//MAKE_CALL(text_addr+0x2254, myfinalize);
	//_sw(0xDADADADA, text_addr+0x2254);
	//_sw(0xEEEEEEEE, text_addr+0x2264);
	//_sw(0x040404DA, text_addr+0x210C);
	//MAKE_CALL(text_addr+0x2020, myfinalize);
	//BMAKE_CALL(text_addr+0x1be8, myfinalize);
	//_sw(0xA1222222, text_addr+0x1e58);
	//_sw(0xac000000, text_addr+0x1ce4);
	//MAKE_CALL(text_addr+0x1ce4, myfinalize);
	_sw(0, text_addr+0x1BAC);
	_sw(0, text_addr+0x1BE8);
	//MAKE_CALL(text_addr+0x1Cd0, myfinalize);
	MAKE_CALL(text_addr+0x3B50, myfinalize);
	_sw(0, text_addr+0x1Ce4);
	cache();

	sceKernelDelayThread(10*1000*1000);
	Kprintf("Rebooting......\n");

	int (* reboot)(void *) = (void *)FindProc("scePower_Service", "scePower_driver", 0x0442D852);

	reboot(0);

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID th;

	th = sceKernelCreateThread("main_thread", main_thread, 0x20, 0x10000, 0, NULL);

	if (th >= 0)
		sceKernelStartThread(th, args, argp);
	
	return 0;
}

