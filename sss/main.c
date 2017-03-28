#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
#include <ctype.h>
#include <elf.h>

static char *file_start;

#pragma pack(1)

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


#define swap32(x) ({\
	unsigned _x = (x); \
	(((_x)>>24)&0xff)\
	|\
	(((_x)>>8)&0xff00)\
	|\
	(((_x)<<8)&0xff0000)\
	|\
	(((_x)<<24)&0xff000000);\
})

#define swap16(x) ({unsigned _x = (x); (((_x)>>8)&0xff) | (((_x)<<8)&0xff00); })

#if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)

#define le32_to_cpu(x) swap32(x)
#define le16_to_cpu(x) swap16(x)
#define be32_to_cpu(x) (x)
#define be16_to_cpu(x) (x)

#else

#define le32_to_cpu(x) (x)
#define le16_to_cpu(x) (x)
#define be32_to_cpu(x) swap32(x)
#define be16_to_cpu(x) swap16(x)

#endif

#define cpu_to_le32(x) le32_to_cpu(x)
#define cpu_to_le16(x) le16_to_cpu(x)
#define cpu_to_be32(x) be32_to_cpu(x)
#define cpu_to_be16(x) be16_to_cpu(x)

//////////////////

void exit(int reason)
{
	printf("reason = %d\n", reason);
}

static void hexdump(char *prefix, char *data, int size){
  if(size > 640)
  	size = 640;
  while(size){
    int amt = (size>16) ? 16 : size;
    char samp[20];
    char hex[80];
    samp[amt]='\0';
    hex[0] = '\0';
    memcpy(samp,data,amt);
    int i = 0;
    char *hp = hex;
    while(i<amt){
      snprintf(hp,42,"%02x ",(unsigned char)samp[i]);
      hp += 3;
      if((i&3)==3){
        hp[0]=' ';
        hp[1]='\0';
        hp++;
      }
      if(!isprint(samp[i]))
        samp[i]='.';
      i++;
    }
    fprintf(stderr,"%s%-52.52s<%s>\n",prefix,hex,samp);
    data += amt;
    size -= amt;
  }
}

/////////////////////////////////////

static unsigned crctable[256];

#define POLYNOMIAL 0x04C11DB7

static void init_crc(void){
	unsigned dividend = 0;
	do{
		unsigned remainder = dividend << 24;
		unsigned bit = 0;
		do{
			if(remainder & 0x80000000){
				remainder <<= 1;
				remainder ^= POLYNOMIAL;
			}else{
				remainder <<= 1;
			}
		}while(++bit<8);
		crctable[dividend++] = remainder;
	}while(dividend<256);
}

// for Marvell, pass 0 as the initial remainder
static unsigned do_crc(unsigned remainder, const unsigned char *p, unsigned n){
	unsigned i = 0;
	while(i < n){
		unsigned char data = *p ^ (remainder >> 24);
		remainder = crctable[data] ^ (remainder << 8);
		p++;
		i++;
	}
	return remainder;
}

///////////////////////////////////////

#define need(size,msg)({        \
	if(mapsize<(int)(size)){   \
		fprintf(stderr,"not enough bytes, have %d of %d for %s\n",(int)mapsize,(int)(size),msg);     \
		return;     \
	}                    \
})

////////////////////////////////////

static char elfdata[4096+128*1024];

static unsigned x0sz;
static unsigned xcsz;
static unsigned shsz;
static char shstrtab[4096];

static int newstr(const char *const s){
	int len = strlen(s)+1;
	memcpy(shstrtab+shsz,s,len);
	shsz += len;
	return shsz-len;
}

struct chunk{
	char name[10];
	char w; // write
	char x; // execute
	unsigned memaddr;
	unsigned memsize;
	unsigned fileaddr;
	unsigned *filesize;
};

struct chunk chunks[] = {
{"",         0,0,0x00000000,0x00000,0x00000,NULL}, // broken GNU crap expects this
{".000",     0,1,0x00000000,0x10000,0x01000,&x0sz},// 1st chunk of code
{".040",     1,0,0x04000000,0x02000,0x11000,NULL}, // boot code sets SP to 0x04002000
{".800",     1,0,0x80000000,0x10000,0x11000,NULL}, // IO memory, probably including USB
{".900",     1,0,0x90000000,0x0c000,0x11000,NULL}, // IO memory
{".c00",     0,1,0xc0000000,0x10000,0x11000,&xcsz},// 2nd chunk of code
{".bss",     1,0,0xc0010000,0x18000,0x21000,NULL}, // 96 KiB RAM
{".fff",     0,1,0xffff0000,0x0fff0,0x21000,NULL}, // boot code ends up in here somewhere
{".shstrtab",0,0,0x00000000,0x00000,0x00e00,&shsz},// must be last for ARRAYSIZE(e.shdr)-1
};

#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

static void mkelf(void){
	struct{
		Elf32_Ehdr ehdr;
		Elf32_Phdr phdr[ARRAYSIZE(chunks)];
		Elf32_Shdr shdr[ARRAYSIZE(chunks)];
		char shstrtab[ARRAYSIZE(chunks)*8];
	}e;

	if(sizeof e > 4096){
		fprintf(stderr, "shit, %d bytes\n", (int)(sizeof e));
		exit(45);
	}

	memset(&e, '\0', sizeof e);

	chunks[ARRAYSIZE(e.shdr)-1].fileaddr = sizeof e - sizeof e.shstrtab;

	int j = 0;
	int i;
	for(i=0;i<(int)ARRAYSIZE(chunks);i++){
		unsigned u;

		if(chunks[i].name[0] && chunks[i].memsize && chunks[i].filesize){
			e.phdr[j].p_type = cpu_to_le32(PT_LOAD);
			e.phdr[j].p_offset = cpu_to_le32(chunks[i].fileaddr); // where in the file
			e.phdr[j].p_vaddr = cpu_to_le32(chunks[i].memaddr);
			e.phdr[j].p_paddr = cpu_to_le32(chunks[i].memaddr);
			e.phdr[j].p_filesz = cpu_to_le32(chunks[i].filesize ? *chunks[i].filesize : 0);
			e.phdr[j].p_memsz = cpu_to_le32(chunks[i].memsize);
			u = 0;
			if(chunks[i].w)
				u |= PF_R|PF_W;
			if(chunks[i].x)
				u |= PF_R|PF_X;
			e.phdr[j].p_flags = cpu_to_le32(u);
			e.phdr[j].p_align = cpu_to_le32(0);
			j++;
		}

		u = newstr(chunks[i].name);
		e.shdr[i].sh_name = cpu_to_le32(u);
		u = SHT_PROGBITS;
		if(!chunks[i].name[0])
			u = SHT_NULL;
		else if(!chunks[i].memsize)
			u = SHT_STRTAB;
		else if(!chunks[i].filesize)
			u = SHT_NOBITS;
		e.shdr[i].sh_type = cpu_to_le32(u);
		u = 0;
		if(chunks[i].w)
			u |= SHF_ALLOC|SHF_WRITE;
		if(chunks[i].x)
			u |= SHF_ALLOC|SHF_EXECINSTR;
		e.shdr[i].sh_flags = cpu_to_le32(u);
		e.shdr[i].sh_addr = cpu_to_le32(chunks[i].memaddr);
		e.shdr[i].sh_offset = cpu_to_le32(chunks[i].fileaddr);
		e.shdr[i].sh_size = cpu_to_le32(chunks[i].filesize ? *chunks[i].filesize : chunks[i].memsize);
		e.shdr[i].sh_link = cpu_to_le32(0);
		e.shdr[i].sh_info = cpu_to_le32(0);
		e.shdr[i].sh_addralign = cpu_to_le32(0);
		e.shdr[i].sh_entsize = cpu_to_le32(0);
	}

	memcpy(&e.ehdr,"\177ELF", 4);
	e.ehdr.e_ident[4] = ELFCLASS32;
	e.ehdr.e_ident[5] = ELFDATA2LSB;
	e.ehdr.e_ident[6] = EV_CURRENT;
	e.ehdr.e_ident[7] = ELFOSABI_STANDALONE;
	e.ehdr.e_ident[8] = 0;
	e.ehdr.e_ident[9] = 0;
	e.ehdr.e_ident[10] = 0;
	e.ehdr.e_ident[11] = 0;
	e.ehdr.e_ident[12] = 0;
	e.ehdr.e_ident[13] = 0;
	e.ehdr.e_ident[14] = 0;
	e.ehdr.e_ident[15] = 0;
	e.ehdr.e_type = cpu_to_le16(ET_EXEC);
	e.ehdr.e_machine = cpu_to_le16(EM_ARM);
	e.ehdr.e_version = cpu_to_le32(EV_CURRENT);
	e.ehdr.e_entry = cpu_to_le32(0);  // entry point
	e.ehdr.e_phoff = cpu_to_le32(sizeof e.ehdr); // offset to program header table
	e.ehdr.e_shoff = cpu_to_le32(sizeof e.ehdr + sizeof e.phdr); // offset to section header table
	e.ehdr.e_flags = cpu_to_le32(0);
	e.ehdr.e_ehsize = cpu_to_le16(sizeof e.ehdr);
	e.ehdr.e_phentsize = cpu_to_le16(sizeof e.phdr[0]);
	e.ehdr.e_phnum = cpu_to_le16(j);
	e.ehdr.e_shentsize = cpu_to_le16(sizeof e.shdr[0]);
	e.ehdr.e_shnum = cpu_to_le16(ARRAYSIZE(e.shdr));
	e.ehdr.e_shstrndx = cpu_to_le16(ARRAYSIZE(e.shdr)-1);

	memcpy(e.shstrtab, shstrtab, sizeof e.shstrtab);
	memcpy(elfdata, &e, sizeof e);
}

////////////////////////////////////

/** fwheader */
struct fwheader {
	u32 dnldcmd;    // 1, except 4 for the 0-sized block at the end
	u32 baseaddr;   // within 64 k following 0 or 0xc0000000
	u32 datalength; // The driver accepts 0 to 600. We see 512, plus short blocks at the end of a segment/section.
	u32 CRC;
};
// for sending to the chip, an extra u32 seq num goes here (before data)

#define FW_HAS_DATA_TO_RECV		0x00000001
#define FW_HAS_LAST_BLOCK		0x00000004

///////////////////////////////////////

static void spew(char *map, ssize_t mapsize){
	int xcmax = 0;
	int x0max = 0;
	for(;;){
		if(!mapsize){
			fprintf(stderr,"ran out at file offset %zd 0x%08zx\n",map-file_start,map-file_start);
			exit(19);
		}
		if(mapsize<0){
			fprintf(stderr,"INTERNAL ERROR NEGATIVE REMAINDER %zd 0x%08zx\n",map-file_start,map-file_start);
			exit(7);
		}

		struct fwheader fwheader;
		unsigned tmp;
		char *saved = map;

		need(sizeof fwheader, "fwheader");
		memcpy(&fwheader,map,sizeof fwheader);
		map += sizeof fwheader;
		mapsize -= sizeof fwheader;
		if((unsigned)mapsize>=le32_to_cpu(fwheader.datalength)){
			memcpy(&tmp,map+le32_to_cpu(fwheader.datalength)-4,4);
		}
		fprintf(stderr,
			"%08x  cmd:%08x addr:%08x len:%08x crc:%08x end:%08x\n",
			(unsigned)(saved-file_start),
			le32_to_cpu(fwheader.dnldcmd),
			le32_to_cpu(fwheader.baseaddr),
			le32_to_cpu(fwheader.datalength),
			le32_to_cpu(fwheader.CRC),
			le32_to_cpu(tmp)
		);
		if(do_crc(0,(unsigned char*)&fwheader,16)){
			fprintf(stderr,"bad header CRC\n");
			exit(91);
		}
		if(le32_to_cpu(fwheader.dnldcmd)==FW_HAS_LAST_BLOCK && fwheader.datalength==0){
			xcsz = xcmax;
			x0sz = x0max;
			return;
		}
		if(le32_to_cpu(fwheader.datalength) > 600){
			fprintf(stderr,"oh crap\n");
			hexdump("x ",saved,mapsize+sizeof fwheader);
		}
		need(le32_to_cpu(fwheader.datalength), "data");
		int len = le32_to_cpu(fwheader.datalength) - 4;
		int addr = le32_to_cpu(fwheader.baseaddr) & 0x0000ffff;
		int past = addr + len;
		if(past>0x10000)
			exit(89);
		if(do_crc(0,(unsigned char*)map,len+4)){
			
			fprintf(stderr,"bad body CRC 0x%08X\n", cpu_to_be32(*(unsigned int *)&map[508]));
			
			memset(map+508, 0, 4);
			fprintf(stderr, "correct CRC 0x%08X\n", do_crc(0, (unsigned char *)map, len));
			exit(90);
		}
		switch(le32_to_cpu(fwheader.baseaddr) & 0xfff00000u){
		case 0xc0000000:
			{
				if(past>xcmax)
					xcmax=past;
				memcpy(elfdata+4096+64*1024+addr,map,len);
			}
			break;
		case 0x00000000:
			{
				if(past>x0max)
					x0max=past;
				memcpy(elfdata+4096+addr,map,len);
			}
			break;
		default:
			fprintf(stderr,"le32_to_cpu(fwheader.baseaddr) is 0x%08x\n",le32_to_cpu(fwheader.baseaddr));
			exit(9);
		}
//		hexdump("data ",map,le32_to_cpu(fwheader.datalength));
		map += le32_to_cpu(fwheader.datalength);
		mapsize -= le32_to_cpu(fwheader.datalength);

//				hexdump("junk ",map,mapsize);
	}
}

static void mapit(char *name, char *out){
  int fd = open(name, O_RDONLY);
  if(fd<3){
    perror("open");
    exit(88);
  }
  struct stat sbuf;
  fstat(fd,&sbuf);
  char *map;
  map = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  file_start = map;

  if(!map || map==(char*)-1){
  	fprintf(stderr,"%d is a bad size or mapping failed\n",(int)sbuf.st_size);
  	exit(3);
  }
  int mapsize = sbuf.st_size;
  fprintf(stderr,"mapped    %6d bytes\n",mapsize);
  spew(map,mapsize);
  fprintf(stderr, "Create elf\n");
  mkelf();
  FILE *f = fopen(out, "wb");
  if (!f)
	  fprintf(stderr, "Cannot create output\n");
  else
  {
	fprintf(stderr, "size = %d\n", sizeof elfdata);
	  
	  fwrite(elfdata,sizeof elfdata,1,f);
	fclose(f);
  }
}

int main(int argc, char *argv[]){
  init_crc();
//  fprintf(stderr,"filename %s\n",argv[1]);
  mapit(argv[1], argv[2]);
  return 0;
}

