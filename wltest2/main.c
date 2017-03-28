#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psploadexec_kernel.h>

#include <systemctrl.h>
#include "main.h"


PSP_MODULE_INFO("dfdfd", 0x1006, 1, 0);

int Ioctl(u32 a0, u32 a1, u32 a2, u32 a3, u32 t0, u32 t1, int (* f)(u32, u32, u32, u32, u32, u32))
{
	/*Kprintf("a0 = 0x%08X\n", a0);
	Kprintf("a1 = 0x%08X\n", a1);
	Kprintf("a2 = 0x%08X\n", a2);
	Kprintf("a3 = 0x%08X\n", a3);
	Kprintf("t0 = 0x%08X\n", t0);
	Kprintf("t1 = 0x%08X\n", t1);

	Kprintf("function address: 0x%08X\n", (u32)f);*/

	SceModule2 *mod = sceKernelFindModuleByAddress((u32)f);

	if (mod)
	{
		Kprintf("Module name: %s\n", mod->modname);
		Kprintf("Module text_addr: 0x%08X\n", mod->text_addr);
	}

	return f(a0, a1, a2, a3, t0, t1);
}

u8 cmd[512] __attribute__((aligned(64)));
u8 rnk[512] __attribute__((aligned(64)));

u8 *g_nk;

int (* SendCommandHardware)(u32 a0, u8 *a1, u32 a2);
int (* RecvCommandHardware)(u32 a0, u8 *a1, u32 a2);
int (* SendCommand)(u8 *unk, u8 *cmd_st, int size);
int (* RecvCmd)(u32 a0, u32 a1, u32 a2);



int (* SendCommand2)(u8 *nk, int code, int size, u8 *cmd);
int (* RecvCommand2)(u8 *nk, int code, int size, u8 *cmd); 

int (* Kll)(u8 *, u32, u8 *);

u8 *lnk;


void PatchIf(u32 text_addr)
{
	u8 *cmd_st = cmd;
	int size = 16;
	/*memset(cmd_st, 0, size);
		
	cmd_st[1] = size;
		
	cmd_st[3] = 0x4D;
	cmd_st[7] = 34;

	SceModule2 *mod = sceKernelFindModuleByName("sceWlan_Driver");

	//_sw(0x00001021, mod->text_addr+0x189D8);
	//_sw(0x00001021, mod->text_addr+0x189C8);

	ClearCaches();

	//SendCommand(g_nk,cmd_st, size);
	//RecvCmd(g_nk, cmd_st, size);*/

	printf("ifhandle\n");

	if (lnk)
	{
		KllPatched(lnk, 0x503, cmd);

		int i;

		printf("ifhandle2\n");

		for (i = 0; i < size; i++)
			printf("+%02X", cmd_st[i]);

		printf("\n");
	}
}


int KllPatched(u8 *nk, u32 y, u8 *cmd_st)
{
	int i;
	u32 addr;

	if (y == 0x503)
	{
		memcpy(cmd, cmd_st, sizeof(cmd));
		lnk = rnk;
		memcpy(lnk, nk, sizeof(rnk));
	}

	//printf("x, y, z = 0x%08X, 0x%08X, 0x%08X\n", nk, y, cmd_st);

	addr = (u32)cmd_st;

	addr = (addr >> 28);

	if (addr == 8)
	{	
		for (i = 0; i < 0x10; i++)
		{
			//printf("K%02X\n", cmd_st[i]);
		}
	}

	// 0600-4800-0A00-0000-0000-01FF

	
	int res = Kll(nk, y, cmd_st);

	if (addr == 8)
	{
		for (i = 0; i < 0x10; i++)
		{
			//printf("L%02X\n", cmd_st[i]);
		}
	}

	return res;
}

int S3(u32 a, u32 b, u32 c, int (* f)(u32, u32, u32))
{
	//printf("S3 = 0x%08X\n", f);	
	//Kprintf("a, b, c = 0x%08X, 0x%08X, 0x%08X\n", a, b, c);

	if (f)
		return f(a, b, c);

	return -1;
}

void convert(void *ss, void *dd, int size)
{
	u32 *s = ss;
	u32 *d = dd;
	int i;

	int r = (size & 3);

	if (r)
		size += (4-r);
	
	for (i = 0; i < size /4; i++)
	{
		d[i] = __builtin_allegrex_wsbw(s[i]);
	}
}

int done4D, done16;

int ProcessDetSend(int code, u8 *cmd_st)
{
	convert(cmd_st, cmd, cmd_st[1]);

	// 1200-1800-1100-0000-004F6101227C-0000-7100-0300-00-000000
	// 1280-1600-0100-0000-7100-0000-01C0-0600-0104-8284-0000

	if (code == 0x12)
	{
		printf("*** ASSOCIATE %s ***\n", "COMMAND");

		printf("Peer MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", cmd[8], cmd[9], cmd[10], cmd[11], cmd[12], cmd[13]);
		printf("CapInfo: 0x%04X\n", *(u16 *)&cmd[14]);
		printf("ListenInterval: 0x%04X\n", *(u16 *)&cmd[16]);
		printf("Beacon period: 0x%04X\n", *(u16 *)&cmd[18]);
		printf("DTIM period: 0x%02X\n", *(u8 *)&cmd[20]);

		//*(u16 *)&cmd[16] |= 0x2000;		

		convert(cmd, cmd_st, cmd_st[1]);

		//return 1;
		
	}

	else if (code == 0x16)
	{
		convert(cmd_st, cmd, cmd_st[1]);

		printf("*** SNMP MIB COMMAND%s\n", "***");		
		printf("Action %s\n", (*(u16 *)&cmd[8]) ? "Set" : "Get");
		printf("OID = 0x%04X\n", *(u16 *)&cmd[10]);

		/*if (*(u16 *)&cmd[10] == 6)
		{
			memset(cmd+8, 0, 20);

			if (done16 == 0)
			{
			
				*(u16 *)&cmd[10] = 1;
				// 82-84-0B-16-0C-12-18-24
				*(u16 *)&cmd[12] = 8;

				cmd[14] = 0x82;
				cmd[15] = 0x84;
				cmd[16] = 0x0B;
				cmd[17] = 0x16;
				cmd[18] = 0x0C;
				cmd[19] = 0x12;
				cmd[20] = 0x18;
				cmd[21] = 0x48;

				convert(cmd, cmd_st, cmd_st[1]);
			}

			else if (done16 == 1)
			{
				*(u16 *)&cmd[10] = 1;
				*(u16 *)&cmd[8] = 0;
				convert(cmd, cmd_st, cmd_st[1]);
			}
			
			done16++;;
		}*/
		
	}

	else if (code == 0x1F && !done4D)
	{
		/*convert(cmd_st, cmd, cmd_st[1]);
		memset(cmd+8, 0, 8);
		done4D = 1;

		*(u16 *)&cmd[0] = 0x10a;
		cmd[2] = 16;

		convert(cmd, cmd_st, cmd[2]);*/
	}

	else if (code == 0x03)
	{
		convert(cmd_st, cmd, cmd_st[1]);
		cmd[16] = 0xDA;
		cmd[17] = 0xDA;
		cmd[18] = 0xDA;
		cmd[19] = 0xDA;
		cmd[20] = 0xDA;
		cmd[21] = 0xDA;
		convert(cmd, cmd_st, cmd_st[1]);
	}

	else if (code == 0x5A)
	{
		printf("*** GPSI BUS CONFIG %s\n", "***");
	}

	else if (code == 0x7A)
	{
		convert(cmd_st, cmd, cmd_st[1]);

		*(u16 *)&cmd[10] = 2;
		*(u16 *)&cmd[12] = 10;
		*(u16 *)&cmd[14] = 4;

		convert(cmd, cmd_st, cmd_st[1]);
	}

	else if (code == 0x7D)
	{
		convert(cmd_st, cmd, cmd_st[1]);

		*(u16 *)&cmd[10] = 0;
		
		convert(cmd, cmd_st, cmd_st[1]);
	}
	
	return 0;
}

int ProcessDetRecv(int code, u8 *cmd_st)
{
	int x = cmd_st[6];

	if (x == 1)
	{
		printf("Command %s\n", "failed");
	}
	else if (x == 2)
	{
		printf("Command is not %s\n", "supported");
	}
	else if (x == 3)
	{
		printf("Command %s\n", "pending");
	}
	else if (x == 4)
	{
		printf("Previosus command %s\n", "busy");
	}
	else if (x == 5)
	{
		printf("Command partial %s\n", "data");
	}
	
	return 0;
}

int SendCommandPatched(u8 *unk, u8 *cmd_st, int size)
{
	//g_nk = unk;
	
	if (1/*cmd_st[2] == 0 && cmd_st[3] != 1*/)
	{	
		printf("Send Command 0x%02X, size = %d\n", cmd_st[3], size);
		
		int i;

		if (!ProcessDetSend(cmd_st[3], cmd_st))
		{
			printf("Dumping command... \n");
			
			for (i = 0; i < size; i += 4)
			{
				printf("%02X", cmd_st[i+3]);
				printf("%02X", cmd_st[i+2]);
				printf("%02X", cmd_st[i+1]);
				printf("%02X", cmd_st[i+0]);
			}
		}
	

		printf("\n");

		if (cmd_st[3] == 0xDA) 
		{
			/*cmd_st[1] = 0x1A;
			size = 0x1A;

			Kprintf("Changing command.\n");

			u8 *p = cmd_st+12;

			for (i = 0; i < 14; i++)
			p[i] = 0xDA;*/

			//SendCommand(unk, cmd_st, size);

			//RecvCmdPatched(unk, cmd_st, size);

			/*cmd_st[3] = 0x0028;
			size = cmd_st[1] = 12;*/

			//int i;
			//u8 *p = cmd_st+8;	
			//cmd_st = cmd;
			u8 j = cmd_st[7];

			size = 16;
			memset(cmd_st, 0, size-8);
		
			cmd_st[1] = size;
			cmd_st[7] = j;
		
			cmd_st[3] = 0x4D;

			cmd_st[11] = 1;
			cmd_st[9] = 1;

			/*cmd_st[8] = 0xDA;
			cmd_st[9] = 0xDA;
			cmd_st[10] =  0;
			cmd_st[11]= 1;
			cmd_st[12] = 0xDA;
			cmd_st[13] = 0xDA;
			cmd_st[14] = 0xDA;
			cmd_st[15] = 0xDA;*/

			//int res = SendCommand(unk, cmd_st, size);

			//RecvCmdPatched(unk, cmd_st, size);
			//cmd_st[11] |= 0x80;
			//p[12] = -1;

			//return res;
		

		}
	}	

	else 
		;//printf("Othercmd");
		
	return SendCommand(unk, cmd_st, size);
}

#define LoadHardware(o) \
	intr = sceKernelCpuSuspendIntr(); \
	addr = hal[o/4]; \
	x = *addr; \
	sceKernelCpuResumeIntrWithSync(intr); \
	x

int SendCommand3_1Patched(u32 a0, u32 a1, u8 *a2, u32 a3)
{
	int intr;
	u32 **hal, *addr, x;

	hal = NULL;
	addr = 0;

	int d = LoadHardware(0x18);


	return 0;
}

int SendCommandHardwarePatched(u32 a0, u8 *a1, u32 a2)
{
	//g_nk = (void *)a0;
	
	//printf("a0 = 0x%08X, a1 = 0x%08X, a2=0x%08X\n", a0, a1, a2);

	int i;

	for (i = 0; i < 4; i++)
		;//printf("%02X", a1[i]);

	//printf("\n");

	return SendCommandHardware(a0, a1, a2);
}

int RecvCommandHardwarePatched(u32 a0, u8 *a1, u32 a2)
{
	int res = RecvCommandHardware(a0, a1, a2);
	
	//printf("b0 = 0x%08X, b1 = 0x%08X, b2=0x%08X\n", a0, a1, a2);

	int i;

	for (i = 0; i < 4; i++)
		//;printf("%02X", a1[i]);

	//printf("\n");

	return res;
}


int RecvCmdPatched(u8 *a0, u8 *a1, u32 a2)
{
	int i;
	
	
	
	int res = RecvCmd(a0, a1, a2);
	
	//Kprintf("a0 = 0x%08X, a1 = 0x%08X, a2 = 0x%08X\n", a0, a1, a2);

	a2 = a1[2];
	
	printf("Receive command 0x%02X, size = %d\n", a1[0], a2);

	if (!ProcessDetRecv(a1[0],a1))
	{
		printf("Dumping response...\n");
	
		for (i = 0; i < a2; i++)
		{
			printf("%02X", a1[i]);
		}

		printf("\n");
	}

	/*if (a1[0] == 0x22)
	{
		u8 *p = a1+12;

		int i;

		for (i = 0; i < 14; i++)
			p[i] = 1;
	}*/
	
	
	return res;
}

void PWlan(u32 text_addr)
{
	MAKE_CALL(text_addr+0xD4AC, S3);
	_sw(0x00403821, text_addr+0xD484);
	MAKE_CALL(text_addr+0x129C, KllPatched);
	MAKE_CALL(text_addr+0x12C0, KllPatched);
	MAKE_CALL(text_addr+0x130C, KllPatched);
	MAKE_CALL(text_addr+0x1340, KllPatched);
	MAKE_CALL(text_addr+0x1360, KllPatched);
	MAKE_CALL(text_addr+0x139C, KllPatched);
	MAKE_CALL(text_addr+0x13D0, KllPatched);
	MAKE_CALL(text_addr+0x1404, KllPatched);
	MAKE_CALL(text_addr+0x1438, KllPatched);
	MAKE_CALL(text_addr+0x147C, KllPatched);
	MAKE_CALL(text_addr+0x1B90, KllPatched);
	MAKE_CALL(text_addr+0x1BA0, KllPatched);
	MAKE_CALL(text_addr+0x1E38, KllPatched);
	MAKE_CALL(text_addr+0x1F40, KllPatched);
	MAKE_CALL(text_addr+0x2380, KllPatched);
	MAKE_CALL(text_addr+0x2504, KllPatched);
	MAKE_CALL(text_addr+0x2858, KllPatched);
	MAKE_CALL(text_addr+0x2F70, KllPatched);
	MAKE_CALL(text_addr+0x2F90, KllPatched);
	MAKE_CALL(text_addr+0x3040, KllPatched);
	MAKE_CALL(text_addr+0x3c4C, KllPatched);
	MAKE_CALL(text_addr+0x406C, KllPatched);
	MAKE_CALL(text_addr+0x4744, KllPatched);
	MAKE_CALL(text_addr+0x4788, KllPatched);
	MAKE_CALL(text_addr+0x47D0, KllPatched);
	MAKE_CALL(text_addr+0x49B0, KllPatched);
	MAKE_CALL(text_addr+0x49C4, KllPatched);
	MAKE_CALL(text_addr+0x4d80, KllPatched);
	MAKE_CALL(text_addr+0x4de8, KllPatched);
	MAKE_CALL(text_addr+0x4eb0, KllPatched);
	MAKE_CALL(text_addr+0x508C, KllPatched);
	MAKE_CALL(text_addr+0x5124, KllPatched);
	MAKE_CALL(text_addr+0x51Ec, KllPatched);
	MAKE_CALL(text_addr+0x5254, KllPatched);
	MAKE_CALL(text_addr+0x52b8, KllPatched);
	MAKE_CALL(text_addr+0x533c, KllPatched);
	MAKE_CALL(text_addr+0x546c, KllPatched);
	MAKE_CALL(text_addr+0x5540, KllPatched);
	MAKE_CALL(text_addr+0x55a4, KllPatched);
	MAKE_CALL(text_addr+0x5720, KllPatched);
	MAKE_CALL(text_addr+0x575c, KllPatched);
	MAKE_CALL(text_addr+0x5840, KllPatched);
	MAKE_CALL(text_addr+0x58Ec, KllPatched);
	MAKE_CALL(text_addr+0x5A24, KllPatched);
	MAKE_CALL(text_addr+0x5A5c, KllPatched);
	MAKE_CALL(text_addr+0x5AB4, KllPatched);
	MAKE_CALL(text_addr+0x5b04, KllPatched);
	MAKE_CALL(text_addr+0x5b54, KllPatched);
	MAKE_CALL(text_addr+0x5bac, KllPatched);
	MAKE_CALL(text_addr+0x5c64, KllPatched);
	MAKE_CALL(text_addr+0x5cc4, KllPatched);
	MAKE_CALL(text_addr+0x5f9c, KllPatched);
	MAKE_CALL(text_addr+0x604c, KllPatched);
	MAKE_CALL(text_addr+0x62cc, KllPatched);
	MAKE_CALL(text_addr+0x637c, KllPatched);
	MAKE_CALL(text_addr+0x6400, KllPatched);
	MAKE_CALL(text_addr+0x64e0, KllPatched);
	MAKE_CALL(text_addr+0x64fc, KllPatched);
	MAKE_CALL(text_addr+0x65cc, KllPatched);
	MAKE_CALL(text_addr+0x6660, KllPatched);
	MAKE_CALL(text_addr+0x6698, KllPatched);
	MAKE_CALL(text_addr+0x6764, KllPatched);
	MAKE_CALL(text_addr+0x67e0, KllPatched);
	MAKE_CALL(text_addr+0x680c, KllPatched);
	MAKE_CALL(text_addr+0x6a48, KllPatched);
	MAKE_CALL(text_addr+0x6a58, KllPatched);
	MAKE_CALL(text_addr+0x6aac, KllPatched);
	MAKE_CALL(text_addr+0x6aec, KllPatched);
	MAKE_CALL(text_addr+0x6b08, KllPatched);
	MAKE_CALL(text_addr+0x6bb0, KllPatched);
	MAKE_CALL(text_addr+0x6bd0, KllPatched);
	MAKE_CALL(text_addr+0x6d24, KllPatched);
	MAKE_CALL(text_addr+0x6d4c, KllPatched);
	MAKE_CALL(text_addr+0x6eec, KllPatched);
	MAKE_CALL(text_addr+0x6f38, KllPatched);
	MAKE_CALL(text_addr+0x6f84, KllPatched);
	MAKE_CALL(text_addr+0x6fbc, KllPatched);
	MAKE_CALL(text_addr+0x6fe4, KllPatched);
	MAKE_CALL(text_addr+0x700c, KllPatched);
	MAKE_CALL(text_addr+0x70b8, KllPatched);
	MAKE_CALL(text_addr+0x7178, KllPatched);
	MAKE_CALL(text_addr+0x7be8, KllPatched);
	MAKE_CALL(text_addr+0x7f6c, KllPatched);
	MAKE_CALL(text_addr+0x7fe8, KllPatched);
	MAKE_CALL(text_addr+0x8004, KllPatched);
	MAKE_CALL(text_addr+0x8014, KllPatched);
	MAKE_CALL(text_addr+0x8028, KllPatched);
	MAKE_CALL(text_addr+0x80BC, KllPatched);
	MAKE_CALL(text_addr+0x8170, KllPatched);
	MAKE_CALL(text_addr+0x81e4, KllPatched);
	MAKE_CALL(text_addr+0x8454, KllPatched);
	MAKE_CALL(text_addr+0x84c8, KllPatched);
	MAKE_CALL(text_addr+0x853c, KllPatched);
	MAKE_CALL(text_addr+0x864c, KllPatched);
	MAKE_CALL(text_addr+0x86c0, KllPatched);
	MAKE_CALL(text_addr+0x876c, KllPatched);
	MAKE_CALL(text_addr+0x877c, KllPatched);
	Kll = (void *)(text_addr+0xD41C);
	//_sw(0x24040007, text_addr+0xD928);
	//_sw(0x2403004A, text_addr+0xD92C);
	MAKE_CALL(text_addr+0xDB54, SendCommandPatched);
	MAKE_CALL(text_addr+0xDEB0, SendCommandPatched);
	MAKE_CALL(text_addr+0xE0C8, SendCommandPatched);
	MAKE_CALL(text_addr+0xE250, SendCommandPatched);
	MAKE_CALL(text_addr+0xE848, SendCommandPatched);
	MAKE_CALL(text_addr+0xEA30, SendCommandPatched);
	MAKE_CALL(text_addr+0xEE5C, SendCommandPatched);
	MAKE_CALL(text_addr+0xF268, SendCommandPatched);
	MAKE_CALL(text_addr+0xF5BC, SendCommandPatched);
	MAKE_CALL(text_addr+0xF814, SendCommandPatched);
	MAKE_CALL(text_addr+0xFBCC, SendCommandPatched);
	MAKE_CALL(text_addr+0xFCB8, SendCommandPatched);
	MAKE_CALL(text_addr+0xFDB4, SendCommandPatched);
	MAKE_CALL(text_addr+0xFF2C, SendCommandPatched);
	MAKE_CALL(text_addr+0x10088, SendCommandPatched);
	MAKE_CALL(text_addr+0x101A8, SendCommandPatched);
	MAKE_CALL(text_addr+0x102CC, SendCommandPatched);
	MAKE_CALL(text_addr+0x103E8, SendCommandPatched);
	MAKE_CALL(text_addr+0x106A8, SendCommandPatched);
	MAKE_CALL(text_addr+0x10780, SendCommandPatched);
	MAKE_CALL(text_addr+0x108A0, SendCommandPatched);
	MAKE_CALL(text_addr+0x109F4, SendCommandPatched);
	MAKE_CALL(text_addr+0x10b48, SendCommandPatched);
	MAKE_CALL(text_addr+0x10cb4, SendCommandPatched);
	MAKE_CALL(text_addr+0x10e50, SendCommandPatched);
	MAKE_CALL(text_addr+0x10f68, SendCommandPatched);
	MAKE_CALL(text_addr+0x11064, SendCommandPatched);
	MAKE_CALL(text_addr+0x11138, SendCommandPatched);
	MAKE_CALL(text_addr+0x1121c, SendCommandPatched);
	MAKE_CALL(text_addr+0x112dc, SendCommandPatched);
	MAKE_CALL(text_addr+0x113d8, SendCommandPatched);
	MAKE_CALL(text_addr+0x114d4, SendCommandPatched);
	MAKE_CALL(text_addr+0x115cC, SendCommandPatched);
	MAKE_CALL(text_addr+0x116c4, SendCommandPatched);
	MAKE_CALL(text_addr+0x11868, SendCommandPatched);
	MAKE_CALL(text_addr+0x1198c, SendCommandPatched);
	MAKE_CALL(text_addr+0x11b08, SendCommandPatched);
	MAKE_CALL(text_addr+0x11c4c, SendCommandPatched);
	MAKE_CALL(text_addr+0x11d88, SendCommandPatched);
	MAKE_CALL(text_addr+0x11e90, SendCommandPatched);
	MAKE_CALL(text_addr+0x11fac, SendCommandPatched);
	MAKE_CALL(text_addr+0x16bcc, SendCommandPatched);
	MAKE_CALL(text_addr+0x16db8, SendCommandPatched);
	SendCommand = (void *)(text_addr+0xF8D0);
	/*SendCommand = (void *)(text_addr+0xCD24);
	MAKE_CALL(text_addr+0xf9a0, SendCommandPatched);
	MAKE_CALL(text_addr+0x13dd0, SendCommandPatched);
	MAKE_CALL(text_addr+0x16c54, SendCommandPatched);*/

	RecvCmd = (void *)(text_addr+0xF9E4);
	MAKE_CALL(text_addr+0xd62c, RecvCmdPatched);

	//MAKE_CALL(text_addr+0xD3A8, SendCommand3_1Patched);

	MAKE_CALL(text_addr+0xD3FC, SendCommandHardwarePatched);
	SendCommandHardware = (void *)(text_addr+0xAAC4);
	
	MAKE_CALL(text_addr+0xD308, RecvCommandHardwarePatched);
	RecvCommandHardware = (void *)(text_addr+0xA56C);

	SendCommand2 = (void *)(text_addr+0xCD24);
	RecvCommand2 = (void *)(text_addr+0xC734);
}

STMOD_HANDLER previous;

int OnModuleStart(SceModule2 *mod)
{
	char *modname = mod->modname;
	u32 text_addr = mod->text_addr;

	if (strcmp(modname, "sceNetInterface_Service") == 0)
	{
		//PatchIf(text_addr);
	}

	else if (strcmp(modname, "sceWlan_Driver") == 0)
	{
		PWlan(text_addr);
	}

	if (previous)
		return previous(mod);

	return 0;
}

int module_start(SceSize args, void *argp)
{
	SceModule2 *mod = sceKernelFindModuleByName("sceWlan_Driver");
	if (mod)
		PWlan(mod->text_addr);
	
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	
	return 0;
}

int module_stop()
{
	return 0;
}




