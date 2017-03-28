#include <pspsdk.h>

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

int (* Reboot)(void *a0, void *a1, void *a2, void *a3) = (void *)0x88c00000;
int (* DcacheClear150)(void) = (void *)0x88c02c64;
int (* IcacheClear150)(void) = (void *)0x88c02c90;
int (* sceBootLfatOpen)(char *file) = (void *)0x88c07618;

void ClearCaches150()
{
	DcacheClear150();
	IcacheClear150();
}

int my_memcmp(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

void my_memcpy(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		m1[i] = m2[i];			
	}
}

char g_file[64];

int sceBootLfatOpenPatched(char *file)
{	
	// Copy to other buffer to avoid changing the string
	my_memcpy(g_file, file, 64);
	
	if (my_memcmp(g_file, "/kd", 3) == 0)
	{
		g_file[2] = 'm';
	}
	else if (my_memcmp(g_file, "/vsh/module", 11) == 0)
	{
		g_file[5] = 'p';
	}

	return sceBootLfatOpen(g_file);	
}

int PatchLoadCore150(void *a0, void *a1, int (* module_start)(void *, void *))
{
	/* No Plain Module Check Patch */
	_sw(0x340D0001, 0x880152e0);
	ClearCaches150();

	return module_start(a0, a1);
}

int entry(void *a0, void *a1, void *a2, void *a3)
{	
	my_memcpy((void *)0x883f0000, (void *)0xbfc00200, 0x200);
	
	// Patch call to sceBootLfatOpen
	MAKE_CALL(0x88c00084, sceBootLfatOpenPatched);
	
	/* Patch the call to LoadCore module_start */
	// 88c00fec: mov $v0, $s2 -> mov $a2, $s2 (module_start)
	// 88c00ff0: mov $a0, $s5
	// 88c00ff4: mov $a1, $sp
	// 88c00ff8: jr  $v0 -> jal PatchLoadCore150
	// 88c00ffc: mov $sp, $s6
	_sw(0x02403021, 0x88c00fec);
	MAKE_CALL(0x88c00ff8, PatchLoadCore150);	
	
	ClearCaches150();
	return Reboot(a0, a1, a2, a3);	
}

int main() { return 0; }
