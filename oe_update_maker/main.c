#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspcrypt.h>
#include <psppower.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <zlib.h>

#include "dxar.h"
#include "recovery.h"
#include "pspbtcnf.h"
#include "systemctrl.h"
#include "vshctrl.h"
#include "uart4.h"
#include "idcanager.h"
#include "reboot150.h"
#include "popcorn.h"

PSP_MODULE_INFO("oe_update_maker", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define printf pspDebugScreenPrintf

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

char *subset150[] =
{	
	"flash0:/kd/ata.prx",
	"flash0:/kd/audio.prx",
	"flash0:/kd/audiocodec.prx",
	"flash0:/kd/blkdev.prx",
	"flash0:/kd/chkreg.prx",
	"flash0:/kd/clockgen.prx",
	"flash0:/kd/codec.prx",
	"flash0:/kd/ctrl.prx",
	"flash0:/kd/display.prx",
	"flash0:/kd/dmacman.prx",
	"flash0:/kd/dmacplus.prx",
	"flash0:/kd/emc_ddr.prx",
	"flash0:/kd/emc_sm.prx",
	"flash0:/kd/exceptionman.prx",
	"flash0:/kd/fatmsmod.prx",
	"flash0:/kd/ge.prx",
	"flash0:/kd/gpio.prx",
	"flash0:/kd/hpremote.prx",
	"flash0:/kd/i2c.prx",
	"flash0:/kd/idstorage.prx",
	"flash0:/kd/ifhandle.prx",
	"flash0:/kd/impose.prx",
	"flash0:/kd/init.prx",
	"flash0:/kd/interruptman.prx",
	"flash0:/kd/iofilemgr.prx",
	"flash0:/kd/isofs.prx",
	"flash0:/kd/lcdc.prx",
	"flash0:/kd/led.prx",
	"flash0:/kd/lfatfs.prx",
	"flash0:/kd/lflash_fatfmt.prx",
	"flash0:/kd/libatrac3plus.prx",
	"flash0:/kd/libhttp.prx",
	"flash0:/kd/libparse_http.prx",
	"flash0:/kd/libparse_uri.prx",
	"flash0:/kd/libupdown.prx",
	"flash0:/kd/loadcore.prx",
	"flash0:/kd/loadexec.prx",
	"flash0:/kd/me_for_vsh.prx",
	"flash0:/kd/me_wrapper.prx",
	"flash0:/kd/mebooter.prx",
	"flash0:/kd/mediaman.prx",
	"flash0:/kd/mediasync.prx",
	"flash0:/kd/memab.prx",
	"flash0:/kd/memlmd.prx",
	"flash0:/kd/mesg_led.prx",
	"flash0:/kd/mgr.prx",
	"flash0:/kd/modulemgr.prx",
	"flash0:/kd/mpeg_vsh.prx",
	"flash0:/kd/mpegbase.prx",
	"flash0:/kd/msaudio.prx",
	"flash0:/kd/mscm.prx",
	"flash0:/kd/msstor.prx",
	"flash0:/kd/openpsid.prx",
	"flash0:/kd/peq.prx",
	"flash0:/kd/power.prx",
	"flash0:/kd/pspbtcnf.txt",
	"flash0:/kd/pspbtcnf_game.txt",
	"flash0:/kd/pspbtcnf_updater.txt",
	"flash0:/kd/pspcnf_tbl.txt",
	"flash0:/kd/pspnet.prx",
	"flash0:/kd/pspnet_adhoc.prx",
	"flash0:/kd/pspnet_adhoc_auth.prx",
	"flash0:/kd/pspnet_adhoc_download.prx",
	"flash0:/kd/pspnet_adhoc_matching.prx",
	"flash0:/kd/pspnet_adhocctl.prx",
	"flash0:/kd/pspnet_ap_dialog_dummy.prx",
	"flash0:/kd/pspnet_apctl.prx",
	"flash0:/kd/pspnet_inet.prx",
	"flash0:/kd/pspnet_resolver.prx",
	"flash0:/kd/pwm.prx",
	"flash0:/kd/registry.prx",
	"flash0:/kd/resource/impose.rsc",
	"flash0:/kd/rtc.prx",
	"flash0:/kd/semawm.prx",
	"flash0:/kd/sircs.prx",
	"flash0:/kd/stdio.prx",
	"flash0:/kd/sysclib.prx",
	"flash0:/kd/syscon.prx",
	"flash0:/kd/sysmem.prx",
	"flash0:/kd/sysreg.prx",
	"flash0:/kd/systimer.prx",
	"flash0:/kd/threadman.prx",
	"flash0:/kd/uart4.prx",
	"flash0:/kd/umd9660.prx",
	"flash0:/kd/umdman.prx",
	"flash0:/kd/usb.prx",
	"flash0:/kd/usbstor.prx",
	"flash0:/kd/usbstorboot.prx",
	"flash0:/kd/usbstormgr.prx",
	"flash0:/kd/usbstorms.prx",
	"flash0:/kd/usersystemlib.prx",
	"flash0:/kd/utility.prx",
	"flash0:/kd/utils.prx",
	"flash0:/kd/vaudio.prx",
	"flash0:/kd/vaudio_game.prx",
	"flash0:/kd/videocodec.prx",
	"flash0:/kd/vshbridge.prx",
	"flash0:/kd/wlan.prx",
	"flash0:/vsh/module/chnnlsv.prx",
	"flash0:/vsh/module/common_gui.prx",
	"flash0:/vsh/module/common_util.prx",
	"flash0:/vsh/module/dialogmain.prx",
	"flash0:/vsh/module/heaparea1.prx",
	"flash0:/vsh/module/heaparea2.prx",
	"flash0:/vsh/module/netconf_plugin.prx",
	"flash0:/vsh/module/netplay_client_plugin.prx",
	"flash0:/vsh/module/netplay_server_utility.prx",
	"flash0:/vsh/module/osk_plugin.prx",
	//"flash0:/vsh/module/paf.prx",
	"flash0:/vsh/module/pafmini.prx",
	"flash0:/vsh/module/savedata_auto_dialog.prx",
	"flash0:/vsh/module/savedata_plugin.prx",
	"flash0:/vsh/module/savedata_utility.prx",
	//"flash0:/vsh/module/vshmain.prx"	
};

char *no351[14] =
{
	"flash0:/vsh/nodule/lftv_main_plugin.prx",
	"flash0:/vsh/nodule/lftv_middleware.prx",
	"flash0:/vsh/nodule/lftv_plugin.prx",
	"flash0:/vsh/resource/lftv_main_plugin.rco",
	"flash0:/vsh/resource/lftv_rmc_univer3in1.rco",
	"flash0:/vsh/resource/lftv_rmc_univer3in1_jp.rco",
	"flash0:/vsh/resource/lftv_rmc_univerpanel.rco",
	"flash0:/vsh/resource/lftv_rmc_univerpanel_jp.rco",
	"flash0:/vsh/resource/lftv_rmc_univertuner.rco",
	"flash0:/vsh/resource/lftv_rmc_univertuner_jp.rco",
	"flash0:/vsh/resource/lftv_tuner_jp_jp.rco",
	"flash0:/vsh/resource/lftv_tuner_us_en.rco",
	"flash0:/font/kr0.pgf"
};

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}


u32 FindProc(const char* szMod, const char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;

	pMod = sceKernelFindModuleByName(szMod);

	if (!pMod)
	{
		printf("Cannot find module %s\n", szMod);
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

	printf("Funtion not found.\n");
	return 0;
}


#define OVERHEAD    0x150 /* size of encryption block overhead */
#define SIZE_A      0x110 /* size of uncompressed file entry = 272 bytes */

// for 1.50 and later, they mangled the plaintext parts of the header
static void Demangle(const u8* pIn, u8* pOut)
{
    u8 buffer[20+0x130];
    memcpy(buffer+20, pIn, 0x130);
    u32* pl = (u32*)buffer; // first 20 bytes
    pl[0] = 5;
    pl[1] = pl[2] = 0;
    pl[3] = 0x55;
    pl[4] = 0x130;

    semaphore_4C537C72(buffer, 20+0x130, buffer, 20+0x130, PSP_KIRK_SCRAMBLE);
    memcpy(pOut, buffer, 0x130);
}

int DecodeBlock(const u8* pIn, int cbIn, u8* pOut)
{
    // pOut also used as temporary buffer for mangled input
    // assert((((u32)pOut) & 0x3F) == 0); // must be aligned

    memcpy(pOut, pIn, cbIn + 0x10); // copy a little more for $10 page alignment

    int ret;
    int cbOut;
    
    // new style - scrambled PSAR (works with 1.5x and 2.00)
    Demangle(pIn+0x20, pOut+0x20); // demangle the inside $130 bytes
    if (*(u32*)&pOut[0xd0] != 0x0E000000)
    {
		printf("Demangle failed\n");
        printf(" PSAR format not supported\n");
        // SaveFile("ms0:/err1.bin", pIn, cbIn);
        // SaveFile("ms0:/err2.bin", pOut, cbIn);
        return -1;
	}
    
	ret = sceMesgd_driver_102DC8AF(pOut, cbIn, &cbOut);
    if (ret != 0)
		;//***printf("Sys_DecodeE returned $%x %d\n", ret, cbOut);
    
    if (ret != 0)
        return -1; // error

    return cbOut;
}

static unsigned long const g_key0[] =
{
  0x7b21f3be, 0x299c5e1d, 0x1c9c5e71, 0x96cb4645, 0x3c9b1be0, 0xeb85de3d, 
  0x4a7f2022, 0xc2206eaa, 0xd50b3265, 0x55770567, 0x3c080840, 0x981d55f2, 
  0x5fd8f6f3, 0xee8eb0c5, 0x944d8152, 0xf8278651, 0x2705bafa, 0x8420e533, 
  0x27154ae9, 0x4819aa32, 0x59a3aa40, 0x2cb3cf65, 0xf274466d, 0x3a655605, 
  0x21b0f88f, 0xc5b18d26, 0x64c19051, 0xd669c94e, 0xe87035f2, 0x9d3a5909, 
  0x6f4e7102, 0xdca946ce, 0x8416881b, 0xbab097a5, 0x249125c6, 0xb34c0872, 
};
static unsigned long const g_key2[] =
{
  0xccfda932, 0x51c06f76, 0x046dcccf, 0x49e1821e, 0x7d3b024c, 0x9dda5865, 
  0xcc8c9825, 0xd1e97db5, 0x6874d8cb, 0x3471c987, 0x72edb3fc, 0x81c8365d, 
  0xe161e33a, 0xfc92db59, 0x2009b1ec, 0xb1a94ce4, 0x2f03696b, 0x87e236d8, 
  0x3b2b8ce9, 0x0305e784, 0xf9710883, 0xb039db39, 0x893bea37, 0xe74d6805, 
  0x2a5c38bd, 0xb08dc813, 0x15b32375, 0x46be4525, 0x0103fd90, 0xa90e87a2, 
  0x52aba66a, 0x85bf7b80, 0x45e8ce63, 0x4dd716d3, 0xf5e30d2d, 0xaf3ae456, 
};
static unsigned long const g_key3[] =
{
  0xa6c8f5ca, 0x6d67c080, 0x924f4d3a, 0x047ca06a, 0x08640297, 0x4fd4a758, 
  0xbd685a87, 0x9b2701c2, 0x83b62a35, 0x726b533c, 0xe522fa0c, 0xc24b06b4, 
  0x459d1cac, 0xa8c5417b, 0x4fea62a2, 0x0615d742, 0x30628d09, 0xc44fab14, 
  0x69ff715e, 0xd2d8837d, 0xbeed0b8b, 0x1e6e57ae, 0x61e8c402, 0xbe367a06, 
  0x543f2b5e, 0xdb3ec058, 0xbe852075, 0x1e7e4dcc, 0x1564ea55, 0xec7825b4, 
  0xc0538cad, 0x70f72c7f, 0x49e8c3d0, 0xeda97ec5, 0xf492b0a4, 0xe05eb02a, 
};
static unsigned long const g_key44[] =
{
  0xef80e005, 0x3a54689f, 0x43c99ccd, 0x1b7727be, 0x5cb80038, 0xdd2efe62, 
  0xf369f92c, 0x160f94c5, 0x29560019, 0xbf3c10c5, 0xf2ce5566, 0xcea2c626, 
  0xb601816f, 0x64e7481e, 0x0c34debd, 0x98f29cb0, 0x3fc504d7, 0xc8fb39f0, 
  0x0221b3d8, 0x63f936a2, 0x9a3a4800, 0x6ecc32e3, 0x8e120cfd, 0xb0361623, 
  0xaee1e689, 0x745502eb, 0xe4a6c61c, 0x74f23eb4, 0xd7fa5813, 0xb01916eb, 
  0x12328457, 0xd2bc97d2, 0x646425d8, 0x328380a5, 0x43da8ab1, 0x4b122ac9, 
};
static unsigned long const g_key20[] =
{
  0x33b50800, 0xf32f5fcd, 0x3c14881f, 0x6e8a2a95, 0x29feefd5, 0x1394eae3, 
  0xbd6bd443, 0x0821c083, 0xfab379d3, 0xe613e165, 0xf5a754d3, 0x108b2952, 
  0x0a4b1e15, 0x61eadeba, 0x557565df, 0x3b465301, 0xae54ecc3, 0x61423309, 
  0x70c9ff19, 0x5b0ae5ec, 0x989df126, 0x9d987a5f, 0x55bc750e, 0xc66eba27, 
  0x2de988e8, 0xf76600da, 0x0382dccb, 0x5569f5f2, 0x8e431262, 0x288fe3d3, 
  0x656f2187, 0x37d12e9c, 0x2f539eb4, 0xa492998e, 0xed3958f7, 0x39e96523, 
};
static unsigned long const g_key3A[] =
{
  0x67877069, 0x3abd5617, 0xc23ab1dc, 0xab57507d, 0x066a7f40, 0x24def9b9, 
  0x06f759e4, 0xdcf524b1, 0x13793e5e, 0x0359022d, 0xaae7e1a2, 0x76b9b2fa, 
  0x9a160340, 0x87822fba, 0x19e28fbb, 0x9e338a02, 0xd8007e9a, 0xea317af1, 
  0x630671de, 0x0b67ca7c, 0x865192af, 0xea3c3526, 0x2b448c8e, 0x8b599254, 
  0x4602e9cb, 0x4de16cda, 0xe164d5bb, 0x07ecd88e, 0x99ffe5f8, 0x768800c1, 
  0x53b091ed, 0x84047434, 0xb426dbbc, 0x36f948bb, 0x46142158, 0x749bb492, 
};

typedef struct
{
    u32 tag; // 4 byte value at offset 0xD0 in the PRX file
    u8* key; // "step1_result" use for XOR step
    u8 code;
    u8 codeExtra;
} TAG_INFO;

static const TAG_INFO g_tagInfo[] =
{
    // 1.x PRXs
    { 0x00000000, (u8*)g_key0, 0x42 },
    { 0x02000000, (u8*)g_key2, 0x45 },
    { 0x03000000, (u8*)g_key3, 0x46 },

    // 2.0 PRXs
    { 0x4467415d, (u8*)g_key44, 0x59, 0x59 },
    { 0x207bbf2f, (u8*)g_key20, 0x5A, 0x5A },
    { 0x3ace4dce, (u8*)g_key3A, 0x5B, 0x5B },
};

static TAG_INFO const* GetTagInfo(u32 tagFind)
{
    int iTag;
    for (iTag = 0; iTag < sizeof(g_tagInfo)/sizeof(TAG_INFO); iTag++)
        if (g_tagInfo[iTag].tag == tagFind)
            return &g_tagInfo[iTag];
    return NULL; // not found
}


static void ExtraV2Mangle(u8* buffer1, u8 codeExtra)
{
    static u8 g_dataTmp[20+0xA0] __attribute__((aligned(0x40)));
    u8* buffer2 = g_dataTmp; // aligned

    memcpy(buffer2+20, buffer1, 0xA0);
    u32* pl2 = (u32*)buffer2;
    pl2[0] = 5;
    pl2[1] = pl2[2] = 0;
    pl2[3] = codeExtra;
    pl2[4] = 0xA0;

    int ret = semaphore_4C537C72(buffer2, 20+0xA0, buffer2, 20+0xA0, 7);
    if (ret != 0);
        //printf("extra de-mangle returns %d\n", ret);
    // copy result back
    memcpy(buffer1, buffer2, 0xA0);
}

int DecryptPRX1(const u8* pbIn, u8* pbOut, int cbTotal, u32 tag)
{
    TAG_INFO const* pti = GetTagInfo(tag);
    if (pti == NULL)
        return -1;

    // build conversion into pbOut
    memcpy(pbOut, pbIn, cbTotal);
    memset(pbOut, 0, 0x150);
    memset(pbOut, 0x55, 0x40); // first $40 bytes ignored

    // step3 demangle in place
    u32* pl = (u32*)(pbOut+0x2C);
    pl[0] = 5; // number of ulongs in the header
    pl[1] = pl[2] = 0;
    pl[3] = pti->code; // initial seed for PRX
    pl[4] = 0x70;   // size

    // redo part of the SIG check (step2)
    u8 buffer1[0x150];
    memcpy(buffer1+0x00, pbIn+0xD0, 0x80);
    memcpy(buffer1+0x80, pbIn+0x80, 0x50);
    memcpy(buffer1+0xD0, pbIn+0x00, 0x80);
    if (pti->codeExtra != 0)
        ExtraV2Mangle(buffer1+0x10, pti->codeExtra);
    memcpy(pbOut+0x40 /* 0x2C+20 */, buffer1+0x40, 0x40);
    
    int ret;
    int iXOR;
    for (iXOR = 0; iXOR < 0x70; iXOR++)
        pbOut[0x40+iXOR] = pbOut[0x40+iXOR] ^ pti->key[0x14+iXOR];

    ret = semaphore_4C537C72(pbOut+0x2C, 20+0x70, pbOut+0x2C, 20+0x70, 7);
    if (ret != 0)
    {
        //printf("mangle#7 returned $%x\n", ret);
        return -1;
    }

    for (iXOR = 0x6F; iXOR >= 0; iXOR--)
        pbOut[0x40+iXOR] = pbOut[0x2C+iXOR] ^ pti->key[0x20+iXOR];

    memset(pbOut+0x80, 0, 0x30); // $40 bytes kept, clean up
    pbOut[0xA0] = 1;
    // copy unscrambled parts from header
    memcpy(pbOut+0xB0, pbIn+0xB0, 0x20); // file size + lots of zeros
    memcpy(pbOut+0xD0, pbIn+0x00, 0x80); // ~PSP header

    // step4: do the actual decryption of code block
    //  point 0x40 bytes into the buffer to key info
    ret = semaphore_4C537C72(pbOut, cbTotal, pbOut+0x40, cbTotal-0x40, 0x1);
    if (ret != 0)
    {
        //printf("mangle#1 returned $%x\n", ret);
        return -1;
    }

    // return cbTotal - 0x150; // rounded up size
	return *(u32*)&pbIn[0xB0]; // size of actual data (fix thanks to Vampire)
}

/***2.60-2.80 PRX Decryption routines and sigcheck generation (Dark_AleX) ***/

/* kernel modules 2.80 */
u8 keys280_0[0x10] =
{
	0xCA, 0xFB, 0xBF, 0xC7, 0x50, 0xEA, 0xB4, 0x40,
	0x8E, 0x44, 0x5C, 0x63, 0x53, 0xCE, 0x80, 0xB1
};

/* user modules 2.80 */
u8 keys280_1[0x10] =
{
	0x40, 0x9B, 0xC6, 0x9B, 0xA9, 0xFB, 0x84, 0x7F,
	0x72, 0x21, 0xD2, 0x36, 0x96, 0x55, 0x09, 0x74
};

/* vshmain executable 2.80 */
u8 keys280_2[0x10] =
{
	0x03, 0xA7, 0xCC, 0x4A, 0x5B, 0x91, 0xC2, 0x07,
	0xFF, 0xFC, 0x26, 0x25, 0x1E, 0x42, 0x4B, 0xB5
};

/* kernel modules 2.60-2.71 */
u8 keys260_0[0x10] =
{
	0xC3, 0x24, 0x89, 0xD3, 0x80, 0x87, 0xB2, 0x4E,
	0x4C, 0xD7, 0x49, 0xE4, 0x9D, 0x1D, 0x34, 0xD1

};

/* user modules 2.60-2.71 */
u8 keys260_1[0x10] =
{
	0xF3, 0xAC, 0x6E, 0x7C, 0x04, 0x0A, 0x23, 0xE7,
	0x0D, 0x33, 0xD8, 0x24, 0x73, 0x39, 0x2B, 0x4A
};

/* vshmain 2.60-2.71 */
u8 keys260_2[0x10] =
{
	0x72, 0xB4, 0x39, 0xFF, 0x34, 0x9B, 0xAE, 0x82,
	0x30, 0x34, 0x4A, 0x1D, 0xA2, 0xD8, 0xB4, 0x3C
};

/* kernel modules 3.00 */
u8 keys300_0[0x10] =
{
	0x9F, 0x67, 0x1A, 0x7A, 0x22, 0xF3, 0x59, 0x0B, 
    0xAA, 0x6D, 0xA4, 0xC6, 0x8B, 0xD0, 0x03, 0x77

};

/* user modules 3.00 */
u8 keys300_1[0x10] =
{
	0x15, 0x07, 0x63, 0x26, 0xDB, 0xE2, 0x69, 0x34, 
    0x56, 0x08, 0x2A, 0x93, 0x4E, 0x4B, 0x8A, 0xB2

};

/* vshmain 3.00 */
u8 keys300_2[0x10] =
{
	0x56, 0x3B, 0x69, 0xF7, 0x29, 0x88, 0x2F, 0x4C, 
    0xDB, 0xD5, 0xDE, 0x80, 0xC6, 0x5C, 0xC8, 0x73

};

u8 keys303_0[0x10] =
{
	0x7b, 0xa1, 0xe2, 0x5a, 0x91, 0xb9, 0xd3, 0x13,
	0x77, 0x65, 0x4a, 0xb7, 0xc2, 0x8a, 0x10, 0xaf
};

/* kernel modules 3.10 */
u8 keys310_0[0x10] = 
{
	0xa2, 0x41, 0xe8, 0x39, 0x66, 0x5b, 0xfa, 0xbb,
	0x1b, 0x2d, 0x6e, 0x0e, 0x33, 0xe5, 0xd7, 0x3f
};

/* user modules 3.10 */
u8 keys310_1[0x10] = 
{
	0xA4, 0x60, 0x8F, 0xAB, 0xAB, 0xDE, 0xA5, 0x65, 
	0x5D, 0x43, 0x3A, 0xD1, 0x5E, 0xC3, 0xFF, 0xEA
};

/* vshmain 3.10 */
u8 keys310_2[0x10] = 
{
	0xE7, 0x5C, 0x85, 0x7A, 0x59, 0xB4, 0xE3, 0x1D, 
	0xD0, 0x9E, 0xCE, 0xC2, 0xD6, 0xD4, 0xBD, 0x2B
};

/* reboot.bin 3.10 */
u8 keys310_3[0x10] = 
{
    0x2E, 0x00, 0xF6, 0xF7, 0x52, 0xCF, 0x95, 0x5A,
    0xA1, 0x26, 0xB4, 0x84, 0x9B, 0x58, 0x76, 0x2F
};

/* kernel modules 3.30 */ 
u8 keys330_0[0x10] = 
{ 
	0x3B, 0x9B, 0x1A, 0x56, 0x21, 0x80, 0x14, 0xED,
	0x8E, 0x8B, 0x08, 0x42, 0xFA, 0x2C, 0xDC, 0x3A
};

/* user modules 3.30 */ 
u8 keys330_1[0x10] = 
{ 
    0xE8, 0xBE, 0x2F, 0x06, 0xB1, 0x05, 0x2A, 0xB9, 
    0x18, 0x18, 0x03, 0xE3, 0xEB, 0x64, 0x7D, 0x26 
}; 

/* vshmain 3.30 */ 
u8 keys330_2[0x10] = 
{ 
    0xAB, 0x82, 0x25, 0xD7, 0x43, 0x6F, 0x6C, 0xC1, 
    0x95, 0xC5, 0xF7, 0xF0, 0x63, 0x73, 0x3F, 0xE7 
}; 

/* reboot.bin 3.30 */ 
u8 keys330_3[0x10] = 
{ 
    0xA8, 0xB1, 0x47, 0x77, 0xDC, 0x49, 0x6A, 0x6F, 
    0x38, 0x4C, 0x4D, 0x96, 0xBD, 0x49, 0xEC, 0x9B 
}; 

/* stdio.prx 3.30 */
u8 keys330_4[0x10] =
{
	0xEC, 0x3B, 0xD2, 0xC0, 0xFA, 0xC1, 0xEE, 0xB9,
	0x9A, 0xBC, 0xFF, 0xA3, 0x89, 0xF2, 0x60, 0x1F
};

typedef struct
{
    u32 tag; // 4 byte value at offset 0xD0 in the PRX file
    u8  *key; // 16 bytes keys
    u8 code; // code for scramble 
} TAG_INFO2;

static TAG_INFO2 g_tagInfo2[] =
{
	{ 0x4C940BF0, keys330_0, 0x43 }, 
	{ 0x457B0AF0, keys330_1, 0x5B }, 
	{ 0x38020AF0, keys330_2, 0x5A }, 
	{ 0x4C940AF0, keys330_3, 0x62 }, 
	{ 0x4C940CF0, keys330_4, 0x43 }, 
	{ 0xcfef09f0, keys310_0, 0x62 },
	{ 0x457b08f0, keys310_1, 0x5B },
	{ 0x380208F0, keys310_2, 0x5A },
	{ 0xcfef08f0, keys310_3, 0x62 },
	{ 0xCFEF07F0, keys303_0, 0x62 },
	{ 0xCFEF06F0, keys300_0, 0x62 },
	{ 0x457B06F0, keys300_1, 0x5B },
	{ 0x380206F0, keys300_2, 0x5A },
	{ 0xCFEF05F0, keys280_0, 0x62 },
	{ 0x457B05F0, keys280_1, 0x5B },
	{ 0x380205F0, keys280_2, 0x5A },
	{ 0x16D59E03, keys260_0, 0x62 },
	{ 0x76202403, keys260_1, 0x5B },
	{ 0x0F037303, keys260_2, 0x5A }
};


static TAG_INFO2 *GetTagInfo2(u32 tagFind)
{
    int iTag;

    for (iTag = 0; iTag < sizeof(g_tagInfo2) / sizeof(TAG_INFO2); iTag++)
	{
        if (g_tagInfo2[iTag].tag == tagFind)
		{
            return &g_tagInfo2[iTag];
		}
	}
    
	return NULL; // not found
}

int Scramble(u32 *buf, u32 size, u32 code)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = code;
	buf[4] = size;

	if (semaphore_4C537C72(buf, size+0x14, buf, size+0x14, 7) < 0)
	{
		return -1;
	}

	return 0;
}

int DecryptPRX2(u8 *inbuf, u8 *outbuf, u32 size, u32 tag)
{
	TAG_INFO2 const* pti = GetTagInfo2(tag);

	if (!pti)
	{
		//printf("Unknown 2.80 tag.\n");
		return -1;
	}

	int retsize = *(int *)&inbuf[0xB0];
	u8	tmp1[0x150], tmp2[0x90+0x14], tmp3[0x60+0x14];

	memset(tmp1, 0, 0x150);
	memset(tmp2, 0, 0x90+0x14);
	memset(tmp3, 0, 0x60+0x14);

	memcpy(outbuf, inbuf, size);

	if (*((u32 *)outbuf) != 0x5053507E) // "~PSP"
	{
		//printf("Error: unknown signature.\n");
		return -1;
	}

	if (size < 0x160) 
	{
		//printf("Buffer not big enough.\n");
		return -1;
	}

	if (((u32)outbuf & 0x3F)) 
	{
		//printf("Buffer not aligned to 64 bytes.\n");
		return -1;
	}

	if ((size - 0x150) < retsize)
	{
		//printf("No enough data.\n");
		return -1;
	}

	memcpy(tmp1, outbuf, 0x150);

	int i, j;
	u8 *p = tmp2+0x14;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			p[(i << 4) + j] = pti->key[j]; 			
		}

		p[(i << 4)] = i;
	}	

	if (Scramble((u32 *)tmp2, 0x90, pti->code) < 0)
	{
		//printf("Error in Scramble #1.\n");
		return -1;
	}

	memcpy(outbuf, tmp1+0xD0, 0x5C);
	memcpy(outbuf+0x5C, tmp1+0x140, 0x10);
	memcpy(outbuf+0x6C, tmp1+0x12C, 0x14);
	memcpy(outbuf+0x80, tmp1+0x080, 0x30);
	memcpy(outbuf+0xB0, tmp1+0x0C0, 0x10);
	memcpy(outbuf+0xC0, tmp1+0x0B0, 0x10);
	memcpy(outbuf+0xD0, tmp1+0x000, 0x80);

	memcpy(tmp3+0x14, outbuf+0x5C, 0x60);	

	if (Scramble((u32 *)tmp3, 0x60, pti->code) < 0)
	{
		//printf("Error in Scramble #2.\n");
		return -1;
	}

	memcpy(outbuf+0x5C, tmp3, 0x60);
	memcpy(tmp3, outbuf+0x6C, 0x14);
	memcpy(outbuf+0x70, outbuf+0x5C, 0x10);
	memset(outbuf+0x18, 0, 0x58);
	memcpy(outbuf+0x04, outbuf, 0x04);

	*((u32 *)outbuf) = 0x014C;
	memcpy(outbuf+0x08, tmp2, 0x10);	

	/* sha-1 */
	if (semaphore_4C537C72(outbuf, 3000000, outbuf, 3000000, 0x0B) < 0)
	{
		//printf("Error in semaphore2  #1.\n");
		return -1;
	}	

	if (memcmp(outbuf, tmp3, 0x14) != 0)
	{
		//printf("SHA-1 incorrect.\n");
		return -1;
	}

	int iXOR;

	for (iXOR = 0; iXOR < 0x40; iXOR++)
	{
		tmp3[iXOR+0x14] = outbuf[iXOR+0x80] ^ tmp2[iXOR+0x10];
	}

	if (Scramble((u32 *)tmp3, 0x40, pti->code) != 0)
	{
		//printf("Error in Scramble #2.\n");
		return -1;
	}
	
	for (iXOR = 0x3F; iXOR >= 0; iXOR--)
	{
		outbuf[iXOR+0x40] = tmp3[iXOR] ^ tmp2[iXOR+0x50]; // uns 8
	}

	memset(outbuf+0x80, 0, 0x30);
	*(u32 *)&outbuf[0xA0] = 1;

	memcpy(outbuf+0xB0, outbuf+0xC0, 0x10);
	memset(outbuf+0xC0, 0, 0x10);

	// The real decryption
	if (semaphore_4C537C72(outbuf, size, outbuf+0x40, size-0x40, 0x1) < 0)
	{
		//printf("Error in semaphore2  #2.\n");
		return -1;
	}

	if (retsize < 0x150)
	{
		// Fill with 0
		memset(outbuf+retsize, 0, 0x150-retsize);		
	}

	return retsize;

}

u32 (* UtilsForKernel_7DD07271) (void* destP, u32 cb, const void* scrP, u32* retP);

int DecryptDecompress(u8 *dataIn, int cbFile, u8 *dataOut, int decompress)
{
	int size = DecryptPRX1(dataIn, dataOut, cbFile, *(u32*)&dataIn[0xD0]);

	if (size <= 0)
	{
		size = DecryptPRX2(dataIn, dataOut, cbFile, *(u32*)&dataIn[0xD0]);
	}

	if (size <= 0)
	{
		if (dataIn[0x150] == 0x1F && dataIn[0x151] == 0x8B)
		{
			size = cbFile-0x150;
			memcpy(dataOut, dataIn+0x150, size);
		}
		else
		{
			return size;
		}
	}

	if (!decompress)
		return size;

	if (dataOut[0] == 0x1F && dataOut[1] == 0x8B)
	{
		memcpy(dataIn, dataOut, size);
		size =  sceKernelGzipDecompress(dataOut, 2000000, dataIn, NULL);
	}

	else if (memcmp(dataOut, "2RLZ", 4) == 0)
	{
		memcpy(dataIn, dataOut, size);
		size = UtilsForKernel_7DD07271(dataOut, 2000000, dataIn+4, NULL);
	}

	return size;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		return -1;
	}

	int written = sceIoWrite(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return written;
}

int ReadFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);

	if (fd < 0)
	{
		return -1;
	}

	int read = sceIoRead(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return read;
}

int DecryptModule(u8 *buf, int size, u8 *buf2, u8 *header)
{
	memcpy(header, buf, 0x150);

	int nsize = DecryptDecompress(buf, size, buf2, 1);
	if (nsize <= 0)
	{
		ErrorExit(6000, "Error in decryption.\n");
	}

	return nsize;
}

int PackModule(u8 *buf, int size, u8 *buf2, u8 *header, u32 oe_tag)
{
	gzFile f = gzopen("ms0:/test.bin", "wb");
	if (f == Z_NULL)
	{
		ErrorExit(6000, "File open error.\n");
	}

	if (gzwrite(f, buf, size) != size)
	{
		ErrorExit(6000, "File write error.\n");
	}

	gzclose(f);	

	int csize = ReadFile("ms0:/test.bin", buf2+0x150, 2000000);
	if (csize <= 0)
	{
		ErrorExit(6000, "File read error.\n");
	}

	sceIoRemove("ms0:/test.bin");

	*(u32 *)&header[0x2C] = csize+0x150;
	*(u32 *)&header[0xB0] = csize;
	*(u32 *)&header[0x130] = oe_tag;	
	memcpy(buf2, header, 0x150);	

	return (csize+0x150);
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int PatchPower(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	_sw(0, text_addr+0xB50);

	_sw(0, text_addr+0x1DA4);
	_sw(0, text_addr+0x2508);
	_sw(0, text_addr+0x2510);
	_sw(0, text_addr+0x2518);
	_sw(0, text_addr+0x2520);
	_sw(0, text_addr+0x2528);
	_sw(0, text_addr+0x2530);
	_sw(0, text_addr+0x2538);
	_sw(0, text_addr+0x2540);	
	_sw(0, text_addr+0x25E4);
	_sw(0, text_addr+0x25F8);
	_sw(0, text_addr+0x2600);
	_sw(0, text_addr+0x2608);
	_sw(0, text_addr+0x2610);
	_sw(0, text_addr+0x2618);
	_sw(0, text_addr+0x2620);
	_sw(0, text_addr+0x2628);

	ClearCaches();

	return PackModule(buf2, nsize, buf, header, 0xF5D02F46);
}

int PatchWlan(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	_sw(0, text_addr+0x2530);
	ClearCaches();

	return PackModule(buf2, nsize, buf, header, 0xF5D02F46);
}

int PatchLowIo(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xC0;

	_sh(0xac60, text_addr+0x8836);
	ClearCaches();

	return PackModule(buf2, nsize, buf, header, 0xF5D02F46);
}

int PatchLoadExec(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	_sw(0x3C0188fc, text_addr+0x199C);

	// Allow LoadExecVSH in whatever user level
	_sw(0x1000000b, text_addr+0x1004);
	_sw(0, text_addr+0x1044);

	// Allow ExitVSHVSH in whatever user level
	_sw(0x10000008, text_addr+0x07A8);
	_sw(0, text_addr+0x07DC);
	
	ClearCaches();
	return PackModule(buf2, nsize, buf, header, 0xF5D02F46);
}

int PatchMsFat(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	_sw(0, text_addr+0x22D8);
	
	ClearCaches();
	return PackModule(buf2, nsize, buf, header, 0xF5D02F46);
}

int PatchVshMain(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xC0;

	// Allow old sfo's.
	_sw(0, (u32)(text_addr+0xE728));
	_sw(0, (u32)(text_addr+0xE730)); 
	
	ClearCaches();
	return PackModule(buf2, nsize, buf, header, 0x2DA8CEA7);
}

int PatchMsVideoPlugin(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	// Patch resolution limit to 130560 pixels (480x272)
	_sh(0xfe00, text_addr+0x32568);
	_sh(0xfe00, text_addr+0x325D4);
	_sh(0xfe00, text_addr+0x345B4);
	_sh(0xfe00, text_addr+0x34628);
	_sh(0xfe00, text_addr+0x346E8);
	_sh(0xfe00, text_addr+0x3A694);
	_sh(0xfe00, text_addr+0x3A7A8);
	_sh(0xfe00, text_addr+0x488E0);
	
	// Patch bitrate limit 768+2 (increase to 16384+2)
	_sh(0x4003, text_addr+0x34550);
	_sh(0x4003, text_addr+0x345DC);

	// Patch bitrate limit 4000 (increase to 16384+2)
	_sh(0x4003, text_addr+0x34684);
	_sh(0x4003, text_addr+0x39904);
	
	ClearCaches();
	return PackModule(buf2, nsize, buf, header, 0x2DA8CEA7);
}

int PatchGamePlugin(u8 *buf, int size, u8 *buf2, u8 *header)
{
	int nsize = DecryptModule(buf, size, buf2, header);
	u32 text_addr = (u32)buf2 + 0xA0;

	_sw(0x1000fff6, text_addr+0xFD84);
	_sw(0x24040000, text_addr+0xFD88);
	
	ClearCaches();
	return PackModule(buf2, nsize, buf, header, 0x2DA8CEA7);
}

int PatchModule(char *file, u8 *buf, int size, u8 *buf2)
{
	u8 header[0x150];
	int retsize = size;
	
	if (strcmp(file, "flash0:/kn/power.prx") == 0)
	{
		retsize = PatchPower(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/kn/wlan.prx") == 0)
	{
		retsize = PatchWlan(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/kn/lowio.prx") == 0)
	{
		retsize = PatchLowIo(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/kn/loadexec.prx") == 0)
	{
		retsize = PatchLoadExec(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/kn/fatmsmod.prx") == 0)
	{
		retsize = PatchMsFat(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/vsh/nodule/vshmain.prx") == 0)
	{
		retsize = PatchVshMain(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/vsh/nodule/msvideo_main_plugin.prx") == 0)
	{
		retsize = PatchMsVideoPlugin(buf, size, buf2, header);
	}
	else if (strcmp(file, "flash0:/vsh/nodule/game_plugin.prx") == 0)
	{
		retsize = PatchGamePlugin(buf, size, buf2, header);
	}
	
	return retsize;
}

int extract_psar(u8 *dataPSAR, int cbFile, u8 *dataOut, u8 *dataOut2, int is150)
{
    if (memcmp(dataPSAR, "PSAR", 4) != 0)
    {
        ErrorExit(6000, "ERROR: Not a PSAR file.\n");
        return -1;
    }
	
	SceUID psar;

	if (is150)
	{
		psar = sceIoOpen("ms0:/mydec.psar", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		sceIoWrite(psar, dataPSAR, 0x10);
	}

    int i;
	int cbOut;
	int sz;
	char name[64];

    // at the start of the PSAR file,
    //   there are one or two special version data chunks
    // printf("Special PSAR records:\n");
    cbOut = DecodeBlock(&dataPSAR[0x10], OVERHEAD+SIZE_A, dataOut);
    if (cbOut <= 0)
    {
        ErrorExit(6000, "ERROR: Failed to decode(1)\n");
        return -1;
    }
    if (cbOut != SIZE_A)
    {
        ErrorExit(6000, "ERROR: Start PSAR chunk has invalid size (%d)\n", cbOut);
        return -1;
    }

	if (is150)
	{
		sceIoWrite(psar, dataOut, cbOut);
	}
   
    int iBase = 0x10+OVERHEAD+SIZE_A; // after first entry
            // iBase points to the next block to decode (0x10 aligned)

	// second block
	cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+100, dataOut);
	if (cbOut <= 0)
	{
		//printf("Performing V2.70 test\n"); // version 2.7 is bigger
		cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+144, dataOut);
		if (cbOut <= 0)
		{
			printf("Failed to decode(2)\n");
			return -1;
		}
	}

	if (is150)
		sceIoWrite(psar, dataOut, cbOut);
       
	int cbChunk = (cbOut + 15) & 0xFFFFFFF0;
	iBase += OVERHEAD+cbChunk;    

    // smart enumerate
    //printf("Writing files...\n");
    
    while (iBase < cbFile-OVERHEAD)
    {
        scePowerTick(0);
		
		cbOut = DecodeBlock(&dataPSAR[iBase], OVERHEAD+SIZE_A, dataOut);
        if (cbOut <= 0)
        {
            ErrorExit(6000, "ERROR: Abort return cb=%d [@$%x]\n", cbOut, iBase);
            //break;
        }
        if (cbOut != SIZE_A)
        {
            ErrorExit(6000, "ERROR: Bad size for filename %d\n", cbOut);
            //break;
        }

		if (is150)
		{
			sceIoWrite(psar, dataOut, cbOut);
		}

        strcpy(name, (const char*)&dataOut[4]);
        //printf("'%s' ", name);		

        u32* pl = (u32*)&dataOut[0x100];
		int sigcheck = (dataOut[0x10F] == 2);
		sigcheck = 0;
        
		// pl[0] is 0
		// pl[1] is the PSAR chunk size (including OVERHEAD)
		// pl[2] is true file size (TypeA=272=SIZE_A, TypeB=size when expanded)
		// pl[3] is flags or version?
        if (pl[0] != 0)
        {
            ErrorExit(6000, "ERROR: Abort pl[0] = $%x\n", pl[0]);
            //break;
        }

        iBase += OVERHEAD + SIZE_A;
        u32 cbDataChunk = pl[1]; // size of next data chunk (including OVERHEAD)
        u32 cbExpanded = pl[2]; // size of file when expanded

        if (cbExpanded > 0)
        {
            cbOut = DecodeBlock(&dataPSAR[iBase], cbDataChunk, dataOut);
            if (cbOut > 10 && dataOut[0] == 0x78 && dataOut[1] == 0x9C)
            {
                // standard Deflate header

				if (is150)
				{
					sceIoWrite(psar, dataOut, cbOut);
				}

                const u8* pbIn = &dataOut[2]; // after header
                u32 pbEnd;
                int ret = sceKernelDeflateDecompress(dataOut2, cbExpanded, pbIn, &pbEnd);
                if (ret == cbExpanded)
                {
                    if (is150)
					{
						for (i = 0; i < 112; i++)
						{
							if (strcmp(name, subset150[i]) == 0)
							{
								if (strcmp(name, "flash0:/kd/pspbtcnf_game.txt") == 0)
								{
									cbExpanded = sizeof(pspbtcnf_game150);
									memcpy(dataOut2, pspbtcnf_game150, cbExpanded);
								}

								else if (strcmp(name, "flash0:/kd/pspbtcnf_updater.txt") == 0)
								{
									cbExpanded = sizeof(pspbtcnf_updater150);
									memcpy(dataOut2, pspbtcnf_updater150, cbExpanded);
								}

								else if (strcmp(name, "flash0:/kd/uart4.prx") == 0)
								{
									strcpy(name, "flash0:/kd/uart4r.prx");
								}
								else if (strcmp(name, "flash0:/kd/emc_sm.prx") == 0)
								{
									strcpy(name, "flash0:/kd/emc_smr.prx");
								}
								else if (strcmp(name, "flash0:/kd/emc_ddr.prx") == 0)
								{
									strcpy(name, "flash0:/kd/emc_ddrr.prx");
								}
								else if (strcmp(name, "flash0:/kd/ge.prx") == 0)
								{
									strcpy(name, "flash0:/kd/emc_sm.prx");
								}
								else if (strcmp(name, "flash0:/kd/idstorage.prx") == 0)
								{
									strcpy(name, "flash0:/kd/idstorager.prx");
								}
								else if (strcmp(name, "flash0:/kd/syscon.prx") == 0)
								{
									strcpy(name, "flash0:/kd/emc_ddr.prx");
								}
								else if (strcmp(name, "flash0:/kd/rtc.prx") == 0)
								{
									strcpy(name, "flash0:/kd/rtcr.prx");
								}
								else if (strcmp(name, "flash0:/kd/display.prx") == 0)
								{
									strcpy(name, "flash0:/kd/ge.prx");
								}
								else if (strcmp(name, "flash0:/kd/ctrl.prx") == 0)
								{
									strcpy(name, "flash0:/kd/idstorage.prx");
								}
								else if (strcmp(name, "flash0:/kd/loadexec.prx") == 0)
								{
									strcpy(name, "flash0:/kd/syscon.prx");
								}
								
								if (dxarAddFile(name, dataOut2, cbExpanded, 0, 0, dataOut, 2000000) < 0)
								{
									ErrorExit(6000, "Error (dxar) %s.\n", name);
								}	

								break;
							}
						}
						
						//if (strcmp(name, "flash0:/kd/loadexec.prx") == 0)
						if (strcmp(name, "flash0:/kd/syscon.prx") == 0)
						{
							sz = DecryptDecompress(dataOut2, cbExpanded, dataOut, 1);
								
							if (sz < 0)
								ErrorExit(6000, "Cannot decrypt loadexec.\n");

							WriteFile("ms0:/loadexec.prx", dataOut, sz);
						}
					}

					else 
					{
						if (strncmp(name, "flash0:/kd", 10) == 0)
							name[9] = 'n';

						else if (strncmp(name, "flash0:/vsh/module", 18) == 0)
							name[12] = 'n';

						int addit = 1;

						for (i = 0; i < 13; i++)
						{
							if (strcmp(name, no351[i]) == 0)
							{
								addit = 0;
								break;
							}
						}

						if (addit)
						{

							if (strcmp(name, "flash0:/kn/pspbtcnf.bin") == 0)
							{
								sigcheck = 0;
								cbExpanded = sizeof(pspbtcnf_bin);
								memcpy(dataOut2, pspbtcnf_bin, cbExpanded);
							}

							if (strncmp(name, "ipl", 3) != 0)
							{
								cbExpanded = PatchModule(name, dataOut2, cbExpanded, dataOut);
								
								if (dxarAddFile(name, dataOut2, cbExpanded, 0, sigcheck, dataOut, 2000000) < 0)
								{
									ErrorExit(6000, "Error (dxar) %s.\n", name);
								}

								if (strcmp(name, "flash0:/font/ltn9.pgf") == 0)
								{
									if (dxarAddFile("flash0:/font/kr0.pgf", dataOut2, cbExpanded, 0, sigcheck, dataOut, 2000000) < 0)
									{
										ErrorExit(6000, "Error (dxar) %s.\n", name);
									}
								}
							}

							if (strcmp(name, "flash0:/kn/loadexec.prx") == 0)
							{
								sz = DecryptDecompress(dataOut2, cbExpanded, dataOut, 1);
									
								if (sz < 0)
									ErrorExit(6000, "Cannot decrypt loadexec.\n");

								WriteFile("ms0:/loadexec.prx", dataOut, sz);
							}

							else if (strcmp(name, "flash0:/kn/sysmem.prx") == 0)
							{
								sz = DecryptDecompress(dataOut2, cbExpanded, dataOut, 1);
									
								if (sz < 0)
									ErrorExit(6000, "Cannot decrypt sysmem.prx.\n");

								//pspSdkInstallNoPlainModuleCheckPatch();
								memcpy((void *)0x883e0000, dataOut+0xC0, sz-0xC0);

								// static relocate
								MAKE_JUMP(0x883F07B4, 0x883f070c);
								MAKE_JUMP(0x883F0800, 0x883f070c);
								MAKE_JUMP(0x883F0A24, 0x883f0a44);
								MAKE_JUMP(0x883F0C24, 0x883f070c);
								MAKE_JUMP(0x883F0C40, 0x883f0bc0);
								MAKE_JUMP(0x883F0C50, 0x883f0be8);
								MAKE_JUMP(0x883F0C68, 0x883f0bac);
								MAKE_JUMP(0x883F0C80, 0x883f0b70);
								MAKE_JUMP(0x883F0C9C, 0x883f0b84);
								MAKE_JUMP(0x883F0CB8, 0x883f0b48);
								MAKE_JUMP(0x883F0CD4, 0x883f0aac);
								MAKE_JUMP(0x883F0D08, 0x883f0b14);
								MAKE_JUMP(0x883F0D40, 0x883f0adc);
								MAKE_JUMP(0x883F0D74, 0x883f0f14);
								MAKE_JUMP(0x883F0DC0, 0x883f088c);
								MAKE_JUMP(0x883F0DDC, 0x883f09d8);
								MAKE_JUMP(0x883F0DF8, 0x883f0f14);
								MAKE_JUMP(0x883F0E14, 0x883f09c8);
								MAKE_JUMP(0x883F0E30, 0x883f099c);
								MAKE_JUMP(0x883F0E7C, 0x883f098c);
								MAKE_JUMP(0x883F0E98, 0x883f08c0);
								MAKE_JUMP(0x883F0ECC, 0x883f0928);
								MAKE_JUMP(0x883F0F04, 0x883f08f0);
								
								sceKernelDcacheWritebackAll();
								sceKernelIcacheClearAll();

								//WriteFile("ms0:/decompress.bin", 0x883F06B0, 0x4000);

								UtilsForKernel_7DD07271 = (void *)0x883F06B0;
							}
						}
					}
				}
                    
                else
                {
                    ErrorExit(6000, "ERROR: deflate error $%x\n", ret);
                    //nErr++;
                }
            }
            else
            {
                ErrorExit(6000, "decode data ERROR cbOut=%d\n", cbOut);
                //nErr++;
            }			
        }
        else if (cbExpanded == 0)
        {
            
        }
        else
        {
            ErrorExit(6000, "cbExpanded bogus (%d) -- abort\n", cbExpanded);
            //break;
        }
        
		//printf("\n");
           
        iBase += cbDataChunk;   // skip over data chunk	
    }
	
	sz = ReadFile("ms0:/loadexec.prx", dataOut, 2000000);
	sceIoRemove("ms0:/loadexec.prx");

	for (i = 0; i < sz-4; i++)
	{
		if (memcmp(dataOut+i, "~PSP", 4) == 0)
		{
			break; 
		}
	}

	if (is150)
	{
		sz = DecryptDecompress(dataOut+i, *(u32 *)&dataOut[i+0x2C], dataOut2, 0);
	}
	else
	{
		sz = DecryptDecompress(dataOut+i, *(u32 *)&dataOut[i+0x2C], dataOut2, 1);
	}

	if (sz <= 0)
	{
		ErrorExit(6000, "Cannot decrypt reboot.bin.\n");
	}

	if (is150)
	{
		for (i = 0; i < sizeof(reboot150); i++)
		{
			if (memcmp(reboot150+i, "reboot.bin", 10) == 0)
			{
				memcpy(reboot150+i+0x10, dataOut2, sz);

				if (dxarAddFile("flash0:/kn/reboot150.prx", reboot150, sizeof(reboot150), 0, 0, dataOut2, 2000000) < 0)
				{
					ErrorExit(6000, "Error (dxar).\n");
				}

				break;
			}
		}

		if (dxarAddFile("flash0:/kd/uart4.prx", uart4, sizeof(uart4), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}

		if (dxarAddFile("flash0:/kd/recovery.prx", recovery, sizeof(recovery), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}
	}

	else
	{
		int compsize = deflateCompress(dataOut2, sz, dataOut, 2000000);

		if (compsize <= 0)
		{
			ErrorExit(6000, "Error compressing reboot.bin.\n");
		}

		WriteFile("ms0:/reboot351.def", dataOut, compsize);

		for (i = 0; i < sizeof(systemctrl150); i++)
		{
			if (memcmp(systemctrl150+i, "reboot.bin", 10) == 0)
			{
				memcpy(systemctrl150+i+0x10, dataOut, compsize);

				if (dxarAddFile("flash0:/kd/rtc.prx", systemctrl150, sizeof(systemctrl150), 0, 0, dataOut2, 2000000) < 0)
				{
					ErrorExit(6000, "Error (dxar).\n");
				}

				break;
			}
		}

		if (dxarAddFile("flash0:/kn/idcanager.prx", idcanager, sizeof(idcanager), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}

		if (dxarAddFile("flash0:/kn/popcorn.prx", popcorn, sizeof(popcorn), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}

		if (dxarAddFile("flash0:/kn/systemctrl.prx", systemctrl351, sizeof(systemctrl351), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}

		if (dxarAddFile("flash0:/kn/vshctrl.prx", vshctrl, sizeof(vshctrl), 0, 0, dataOut2, 2000000) < 0)
		{
			ErrorExit(6000, "Error (dxar).\n");
		}
	}

	if (is150)
	{
		sceIoClose(psar);
	}

    return 0;
}

u8 *dataPSAR, *dataOut, *dataOut2;

u8 sha1_150[20] =
{
	0x1A, 0x4C, 0x91, 0xE5, 0x2F, 0x67, 0x9B, 0x8B, 
	0x8B, 0x29, 0xD1, 0xA2, 0x6A, 0xF8, 0xC5, 0xCA, 
	0xA6, 0x04,	0xD3, 0x30
};

u8 sha1_351[20] = 
{
	0x12, 0x51, 0xF4, 0x43, 0xDC, 0x9D, 0xAC, 0xFF, 
	0xC8, 0x43, 0x9F, 0x6F, 0x53, 0xB5, 0x80, 0xBE, 
	0x91, 0x08, 0x60, 0x5C
};

u8 sha1_dxar[20] = 
{
	0x6F, 0x76, 0xBB, 0x67, 0xB3, 0x1E, 0xFF, 0x6F, 
	0xEC, 0xB7, 0x2D, 0x9D, 0x4A, 0xF4, 0x43, 0xC7, 
	0x93, 0xE1, 0xE5, 0x21
};

u8 sha1[20];

// 96B4E3ED430A591B077624FC326622AC
// 9956936EADF91D578D4087499B7211B9429E99F7

SceKernelUtilsSha1Context ctx;

#define PSAR_SIZE 16327456

int main(int argc, char *argv[])
{
	SceUID fd;
	int size, read, i;
	
	pspDebugScreenInit();

	printf("3.51 OE-A DXAR Update maker by Dark_AleX.\n");
	printf("Uses code of psardumper (PspPet) for psar extraction.\n");

	dataPSAR = (u8 *)memalign(0x40, PSAR_SIZE+256);

	if (!dataPSAR)
		ErrorExit(6000, "Cannot allocate memory (1).\n");

	dataOut = (u8 *)memalign(0x40, 2000000);

	if (!dataOut)
		ErrorExit(6000, "Cannot allocate memory (2).\n");

	dataOut2 = (u8 *)memalign(0x40, 2000000);

	if (!dataOut2)
		ErrorExit(6000, "Cannot allocate memory (3).\n");

	if (dxarInit("DATA.DXAR") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarInitSection("DIR") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/data") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/data/cert") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/dic") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/font") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/kd") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/kd/resource") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/vsh") < 0)
		ErrorExit(5000, "Error (dxar).\n");	

	if (dxarAddDirectory("flash0:/vsh/etc") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/vsh/module") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/vsh/nodule") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/vsh/resource") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/kn") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarAddDirectory("flash0:/kn/resource") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarEndSection(dataOut, 2000000) < 0)
		ErrorExit(5000, "Error (dxar).\n");

	printf("Opening and checking 150.PBP...\n");

	fd = sceIoOpen("150.PBP", PSP_O_RDONLY, 0777);
	if (fd < 0)
	{
		ErrorExit(6000, "Cannot open file.\n");
	}

	size = sceIoLseek(fd, 0, SEEK_END);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	sceIoRead(fd, dataOut, 200);

	if (memcmp(dataOut, "\0PBP", 4) != 0)
	{
		sceIoClose(fd);
		ErrorExit(6000, "Invalid PBP file.\n");
	}
	
	size = size -  *(u32 *)&dataOut[0x24];

	sceIoLseek(fd, *(u32 *)&dataOut[0x24], PSP_SEEK_SET);

	if (size != 10149440)
	{
		ErrorExit(6000, "Invalid 1.50 update file.\n");
		sceIoClose(fd);
	}

	if (sceIoRead(fd, dataPSAR, 10149440) != 10149440)
	{
		ErrorExit(6000, "Invalid 1.50 update file (2).\n");
		sceIoClose(fd);
	}

	sceIoClose(fd);

	sceKernelUtilsSha1Digest(dataPSAR, 10149440, sha1);

	if (memcmp(sha1, sha1_150, 20) != 0)
	{
		ErrorExit(6000, "Invalid 1.50 update file (3).\n");		
	}

	if (dxarInitSection("1.50") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	printf("Extracting and packing 1.50 firmware subset...\n");

	extract_psar(dataPSAR, 10149440, dataOut, dataOut2, 1);

	if (dxarEndSection(dataOut, 2000000) < 0)
		ErrorExit(5000, "Error (dxar).\n");

	printf("Opening and checking 351.PBP...\n");

	fd = sceIoOpen("351.PBP", PSP_O_RDONLY, 0777);
	if (fd < 0)
	{
		ErrorExit(6000, "Cannot open file.\n");
	}

	size = sceIoLseek(fd, 0, SEEK_END);
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	sceIoRead(fd, dataOut, 200);

	if (memcmp(dataOut, "\0PBP", 4) != 0)
	{
		sceIoClose(fd);
		ErrorExit(6000, "Invalid PBP file.\n");
	}
	
	size = size -  *(u32 *)&dataOut[0x24];

	sceIoLseek(fd, *(u32 *)&dataOut[0x24], PSP_SEEK_SET);

	if (size != PSAR_SIZE)
	{
		ErrorExit(6000, "Invalid 3.51 update file.\n");
		sceIoClose(fd);
	}

	if (sceIoRead(fd, dataPSAR, PSAR_SIZE) != PSAR_SIZE)
	{
		ErrorExit(6000, "Invalid 3.51 update file (2).\n");
		sceIoClose(fd);
	}

	sceIoClose(fd);

	sceKernelUtilsSha1Digest(dataPSAR, PSAR_SIZE, sha1);

	if (memcmp(sha1, sha1_351, 20) != 0)
	{
		ErrorExit(6000, "Invalid 3.51 update file (3).\n");		
	}

	if (dxarInitSection("3.51") < 0)
		ErrorExit(5000, "Error (dxar).\n");

	printf("Extracting and packing 3.51 firmware...\n");	

	extract_psar(dataPSAR, PSAR_SIZE, dataOut, dataOut2, 0);

	if (dxarEndSection(dataOut, 2000000) < 0)
		ErrorExit(5000, "Error (dxar).\n");

	if (dxarEnd(dataOut, 2000000) < 0)
		ErrorExit(5000, "Error (dxar).\n");

	printf("Doing static SHA-1 verification...\n");

	fd = sceIoOpen("DATA.DXAR", PSP_O_RDONLY, 0777);

	if (fd < 0)
	{
		ErrorExit(5000, "Cannot open DATA.DXAR for reading.\n");
	}

	sceKernelUtilsSha1BlockInit(&ctx);

	while ((read = sceIoRead(fd, dataOut, 2000000)) > 0)
	{
		sceKernelUtilsSha1BlockUpdate(&ctx, dataOut, read);
	}

	sceIoClose(fd);
	sceKernelUtilsSha1BlockResult(&ctx, sha1);

	printf("SHA1: ");
	for (i = 0; i < 20; i++)
		printf("%02X", sha1[i]);

	if (memcmp(sha1, sha1_dxar, 20) != 0)
	{
		pspDebugScreenSetTextColor(0x000000FF);
		ErrorExit(10000, "\nERROR: Invalid SHA-1. Try again.\nAuto-exiting in 10 seconds.\n");
	}

	pspDebugScreenSetTextColor(0x0000FF00);

	ErrorExit(5000, "\nCorrect value.\nThe dxar creation was succesfull.\nAuto exiting in 5 seconds.\n");
	
	return 0;
}
