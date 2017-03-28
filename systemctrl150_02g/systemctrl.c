#include <pspsdk.h>
#include <pspkernel.h>

#include <string.h>

#include <systemctrl.h>

int sctrlHENGetVersion()
{
	return 0x00000700;
}

int sctrlSEGetVersion()
{
	return 0x00001011;
}

int sctrlSEGetOtherDevkit()
{
	return 0x03070110;
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	int k1 = pspSdkSetK1(0);
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");
	u32 text_addr = *(mod+27);

	u32 *(* GetDevice)(char *) = (void *)(text_addr+0x16D4);
	u32 *u;

	u = GetDevice(drvname);

	if (!u)
	{
		pspSdkSetK1(k1);
		return NULL;
	}

	pspSdkSetK1(k1);
	return (PspIoDrv *)u[1];
}

u32 sctrlHENFindFunction(const char* szMod, const char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule2 *pMod;
	void *entTab;
	int entLen;

	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
	{
		//Kprintf("Cannot find module %s\n", szMod);
		return 0;
	}
	
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
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

	return 0;
}

