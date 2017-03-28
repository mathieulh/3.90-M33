#include <pspsdk.h>

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

int (* Reboot)(void *a0, void *a1, void *a2, void *a3) = (void *)0x88c00000;
int (* DcacheClear150)(void) = (void *)0x88c02c64;
int (* IcacheClear150)(void) = (void *)0x88c02c90;

typedef struct
{
	char path[64];
	u32  addr;
	u32  size;
} RebootFile;

RebootFile *reb_files = (void *)0x89000000;
int cur_file;
int cur_seek;

void ClearCaches150()
{
	DcacheClear150();
	IcacheClear150();
}

int my_strcmp(char *s1, char *s2)
{
	unsigned char uc1, uc2;
    while (*s1 != '\0' && *s1 == *s2) 
	{
        s1++;
        s2++;
    }
    /* Compare the characters as unsigned char and
       return the difference.  */
    uc1 = (*(unsigned char *) s1);
    uc2 = (*(unsigned char *) s2);
    return ((uc1 < uc2) ? -1 : (uc1 > uc2));
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

int sceBootLfatOpenPatched(char *file)
{	
	int i;

	for (i = 0; i < 80; i++)
	{
		if (my_strcmp(reb_files[i].path, file) == 0)
		{
			cur_file = i;
			cur_seek = 0;
			return 0;
		}
	}
	
	return 0x80010002;	
}

int sceBootLfatReadPatched(void *buf, int size)
{
	int remaining = reb_files[cur_file].size - cur_seek;
	
	if (remaining < size)
		size = remaining;

	if (size > 0)
	{
		my_memcpy((char *)buf, (char *)(reb_files[cur_file].addr+cur_seek), size);
		cur_seek += size;
	}

	return size;
}

int sceBootLfatClosePatched()
{
	return 0;
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
	int i;
	u32 load_addr = 0;
	
	my_memcpy((void *)0x883f0000, (void *)0xbfc00200, 0x200);
	
	// Patch read funcs
	MAKE_CALL(0x88c00084, sceBootLfatOpenPatched);
	MAKE_CALL(0x88c000b4, sceBootLfatReadPatched);
	MAKE_CALL(0x88c000e0, sceBootLfatClosePatched);

	// Ignore fat mount
	_sw(0x00001021, 0x88c00074);

	// Increase load address
	for (i = 0; i < 80; i++)
	{
		u32 new_addr = reb_files[i].addr + reb_files[i].size;

		if (new_addr > load_addr)
			load_addr = new_addr;
	}

	load_addr = (load_addr + 0x3F) & ~0x3F;
	_sw(load_addr, 0x88c16988);
	
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
