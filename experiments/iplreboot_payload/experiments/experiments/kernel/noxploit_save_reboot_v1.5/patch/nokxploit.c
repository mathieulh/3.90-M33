#include <pspsdk.h>
#include <pspkernel.h>

//#define DEBUG

typedef struct 
{ 
	u32 signature; 
	int version; 
	int offsets[8]; 
} PBPHeader;

#define PBP_SIGNATURE 0x50425000

struct SceKernelLoadExecVSHParam {
/* Size of structure in bytes */
    SceSize     size; 
/* Size of the arguments string */
    SceSize     args;
/* Pointer to the arguments strings */
    void * argp;
/* "game", "updater" or "vsh" ("game" when loading homebrews) */
    const char * key;
/* unknown, it seems to be some kind of flag. the firmware set it to 
   0x00000400. it looks like is related with the next fields of the 
   structure, it's better to set it to 0 if we don't know how to use 
   those fields */
    u32 unk1;
/* unknown, the firmware always set it to 0x09CF344C, which seems to 
   be a pointer */
    void *unk2;
/* unknown. the firmware sets it to 0 */
    u32 unk3;
/* unknown. the firmware sets it to 0 */
    u32 unk4;
/* unknown. the firmware sets it to 0 */
    u32 unk5
};

char buffer[16384];

void copyBootMem(void)
{
	int i;
	unsigned int *mem  = 0x88c00000;
					   //0x883e0000
	unsigned int *dest = 0x883f0000;
	for(i=0; i<0x2000; i++)
		*(dest++) = *(mem++);
}

int hooked_LoadExecForKernel_28D0D249(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	// Save registers to restore them later
	// The compiler should do it with sN registers, but since we
	// are going to exit the function in a "different way", we cannot
	// rely on the compiler
	u32 reg_a0, reg_a1, reg_a2, reg_a3, 
	reg_s0, reg_s1, reg_s2, reg_s3,
	reg_s4, reg_s5, reg_s6, reg_s7, reg_ra;
	
	SceUID pbp;
	PBPHeader header;
	
	asm("sw $ra, 0(%0)\n" : : "r"(&reg_ra));
	asm("sw $a0, 0(%0)\n" : : "r"(&reg_a0));
	asm("sw $a1, 0(%0)\n" : : "r"(&reg_a1));
	asm("sw $a2, 0(%0)\n" : : "r"(&reg_a2));
	asm("sw $a3, 0(%0)\n" : : "r"(&reg_a3));
	asm("sw $s0, 0(%0)\n" : : "r"(&reg_s0));
	asm("sw $s1, 0(%0)\n" : : "r"(&reg_s1));
	asm("sw $s2, 0(%0)\n" : : "r"(&reg_s2));
	asm("sw $s3, 0(%0)\n" : : "r"(&reg_s3));
	asm("sw $s4, 0(%0)\n" : : "r"(&reg_s4));
	asm("sw $s5, 0(%0)\n" : : "r"(&reg_s5));
	asm("sw $s6, 0(%0)\n" : : "r"(&reg_s6));
	asm("sw $s7, 0(%0)\n" : : "r"(&reg_s7));

#ifdef DEBUG
	SceUID fd;

	fd = sceIoOpen("ms0:/debugging.bin", PSP_O_WRONLY|PSP_O_CREAT, 0777);
	
	if (file)
		sceIoWrite(fd, file, strlen(file));
	
	if (param)
	{
		if (param->argp)
			sceIoWrite(fd, param->argp, param->args);

		if (param->key)
			sceIoWrite(fd, param->key, strlen(param->key));

		sceIoWrite(fd, &param->unk1, 4);
		sceIoWrite(fd, &param->unk2, 4);
		sceIoWrite(fd, &param->unk3, 4);
		sceIoWrite(fd, &param->unk4, 4);
		sceIoWrite(fd, &param->unk5, 4);
	}

	sceIoClose(fd);
#endif

	pbp = sceIoOpen(file, PSP_O_RDONLY, 0777);
	
	if (pbp >= 0)
	{
		sceIoRead(pbp, &header, sizeof(PBPHeader));

		if (header.signature == PBP_SIGNATURE)
		{
			SceUID elf;
			int size, read;

			strcpy(file+strlen(file)-3, "ELF");	
			elf = sceIoOpen(file, PSP_O_WRONLY|PSP_O_CREAT, 0777);
			
			sceIoLseek32(pbp, header.offsets[6], PSP_SEEK_SET);
			
			size = header.offsets[7]-header.offsets[6];
			read = 0;

			while (1)
			{
				read = sceIoRead(pbp, buffer, 16384);

				if (read > 0)
					sceIoWrite(elf, buffer, read);

				if (read < 16384)
					break;
			}

			sceIoClose(elf);
		}

		sceIoClose(pbp);
	}

	// Restore regs
	asm("lw $s7, 0(%0)\n" : : "r"(&reg_s7));
	asm("lw $s6, 0(%0)\n" : : "r"(&reg_s6));
	asm("lw $s5, 0(%0)\n" : : "r"(&reg_s5));
	asm("lw $s4, 0(%0)\n" : : "r"(&reg_s4));
	asm("lw $s3, 0(%0)\n" : : "r"(&reg_s3));
	asm("lw $s2, 0(%0)\n" : : "r"(&reg_s2));
	asm("lw $s1, 0(%0)\n" : : "r"(&reg_s1));
	asm("lw $s0, 0(%0)\n" : : "r"(&reg_s0));
	asm("lw $a3, 0(%0)\n" : : "r"(&reg_a3));
	asm("lw $a2, 0(%0)\n" : : "r"(&reg_a2));
	asm("lw $a1, 0(%0)\n" : : "r"(&reg_a1));
	asm("lw $a0, 0(%0)\n" : : "r"(&file));
	asm("lw $ra, 0(%0)\n" : : "r"(&reg_ra));

	// Two first original instructions of LoadExecForKernel_28D0D249
	asm("addiu $sp, $sp, 0xfff0\n");
	asm("addu  $a2, $a1, $zero\n");

// 214:   27bdfff0        addiu   sp,sp,-16
// 218:   00a03021        move    a2,a1
	
	// Go to LoadExecForKernel_28D0D249 + 2 instructions
	asm("li $v0, 0x880BC27C\n");
	asm("jr $v0\n");
}