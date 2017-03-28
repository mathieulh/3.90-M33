#include <pspsdk.h>
#include <pspkernel.h>


PSP_MODULE_INFO("pspUmdEmu_Driver", 0x1007, 1, 0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

int u = 0;

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int w = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return w;
}

int MyDevctl(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int res = sceIoDevctl(dev, cmd, indata, inlen, outdata, outlen);
	
	if (u == 0 && cmd == 0x1E28035)
	{
		WriteFile("ms0:/umd_devctl_in.bin", indata, inlen);
		WriteFile("ms0:/umd_devctl_out.bin", outdata, outlen);
		WriteFile("ms0:/umd_devctl_res.bin", &res, 4);
		WriteFile("ms0:/umd_devctl_out_ex.bin", *(u32 *)outdata, 0x800);
		u = 1;
	}

	return res;
}

int (* sceUmdManRegisterInsertEjectUMDCallBack)(int id, void *callback, void *arg);

int (* F1)(void *a0, void *a1, void *a2, void *a3, void *t0);
int (* F2)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1);

int m1, m2;

int MyF2(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1)
{
	if (!m2)
		WriteFile("ms0:/ff2_param0.bin", a0, 0x100);
	
	int res = F2(a0, a1, a2, a3, t0, t1);

	if (!m2)
	{
		WriteFile("ms0:/ff2_res.bin", &res, 4);
		WriteFile("ms0:/ff2_param4_retres.bin", t1, 4);
		m2 = 1;
	}

	return res;
}

int MyF1(void *a0, void *a1, void *a2, void *a3, void *t0)
{
	if (!m1)
		WriteFile("ms0:/ff1_param0.bin", a0, 0x100);
		
	int res = F1(a0, a1, a2, a3, t0);

	if (!m1)
	{
		WriteFile("ms0:/ff1_res.bin", &res, 4);
		WriteFile("ms0:/ff1_param4_retres.bin", t0, 0x100);
		m1 = 1;
	}

	return res;
}

int x1;

int (* X1)(int);

int MyX1(int a)
{
	if (!x1)
		WriteFile("ms0:/x1_a.bin", &a, 4);

	int res = X1(a);
	if (!x1)
	{
		WriteFile("ms0:/x1_res.bin", &res, 4);
		WriteFile("ms0:/x1_res_buf.bin", (void *)res, 0x1000);
		x1 = 1;
	}

	return res;
}

int (* oldcallback)(int x, void *, void *);

int mycallback(int x, void *d, int u)
{
	WriteFile("ms0:/callback_d.bin", d, 0x100);
	WriteFile("ms0:/callback_e.bin", &u, 4);
	
	int res = oldcallback(x, d, u);

	WriteFile("ms0:/callback_d_aft.bin", d, 0x100);
	return res;
}

int r = 0;

int MyIE(int id, void *callback, void *arg)
{
	u32 *mod =  (u32 *)sceKernelFindModuleByName("sceIsofs_driver");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x1D80, MyF2);
	F2 = (void *)(text_addr+0x1080);

	MAKE_CALL(text_addr+0x3A24, MyF1);
	F1 = (void *)(text_addr+0x19A0);

	MAKE_CALL(text_addr+0x3854, MyX1);
	X1 = (void *)(text_addr+0x250);

	if (!r)
	{
		WriteFile("ms0:/isofs_textaddr.bin", &text_addr, 4);
		WriteFile("ms0:/callback_arg.bin", arg, 0x100);
		r = 1;
	}

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	oldcallback = callback;
	
	int res = sceUmdManRegisterInsertEjectUMDCallBack(id, mycallback, arg);
	WriteFile("ms0:/register_res.bin", &res, 4);

	return res;
}

int (* sceUmdMan_driver_60933ECD)();

int (* T1)(void *addr);

int MyT1(void *addr)
{
	WriteFile("ms0:/t1_buf_bef.bin", addr, 0x100);	
	int res = T1(addr);
	WriteFile("ms0:/t1_buf_aft.bin", addr, 0x100);

	return res;
}

int (* isofs_mount)(PspIoDrvFileArg *arg,const char *asgn_name,const char *dev_name,int wr_mode, void *unk, int usize);

u8 data[0x100];
int gres=0xDADADADA;

int IoMount(PspIoDrvFileArg *arg,const char *asgn_name,const char *dev_name,int wr_mode, void *unk, int usize)
{
	WriteFile("ms0:/mount_arg.bin", arg, 0x20);
	WriteFile("ms0:/mount_assgn.bin", asgn_name, 0x20);
	WriteFile("ms0:/mount_devname.bin", dev_name, 0x20);
	WriteFile("ms0:/mount_wrmode.bin", &wr_mode, 4);
	WriteFile("ms0:/mount_unk.bin", unk, 0x20);
	WriteFile("ms0:/mount_usize.bin", &usize, 4);

	
	WriteFile("ms0:/init_after.bin", data, 0x100);
	WriteFile("ms0:/6093_res.bin", &gres, 4);


	return isofs_mount(arg, asgn_name, dev_name, wr_mode, unk, usize);
}

int (*isofs_init)(void *);

u32 heap;

int IoInit(void *buf)
{
	int res = isofs_init(buf);

	u32 mem = _lw(heap);	
	memcpy(data, (void *)mem, 0x100);

	return res;
}

int UPatched()
{
	u32 *mod =  (u32 *)sceKernelFindModuleByName("sceIsofs_driver");
	u32 text_addr = *(mod+27);

	MAKE_CALL(text_addr+0x28B8, MyT1);
	T1 = (void *)(text_addr+0x4CF0);

	_sw((u32)IoMount, text_addr+0x6960);
	isofs_mount = (void *)(text_addr+0x2718);

	_sw((u32)IoInit, text_addr+0x6918);
	isofs_init = (void *)(text_addr+0x250C);

	heap = text_addr+0x6B00;

	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
	
	gres = sceUmdMan_driver_60933ECD();	
	return gres;
}

int module_start(SceSize args, void *argp)
{
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceIOFileManager");
	u32 text_addr = *(mod+27);

	_sw((u32)MyDevctl, text_addr+0x5FE4);	

	mod = (u32 *)sceKernelFindModuleByName("sceUmdMan_driver");
	text_addr = *(mod+27);

	_sw((u32)MyIE, text_addr+0x105A8);
	sceUmdManRegisterInsertEjectUMDCallBack = (void *)(text_addr+0xA2E4);

	_sw((u32)UPatched,	text_addr+0x10528);
	sceUmdMan_driver_60933ECD = (void *)(text_addr+0xCCAC);
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	return 0;
}

