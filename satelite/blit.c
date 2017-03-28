/*
	PSP VSH 24bpp text bliter
*/

#include <pspuser.h>
#include <pspdisplay.h>

#include <psppaf.h>

#define ALPHA_BLEND 1

extern unsigned char msx[];

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int pwidth, pheight, bufferwidth, pixelformat;
static unsigned int* vram32;

static u32 fcolor = 0x00ffffff;
static u32 bcolor = 0xff000000;

#if ALPHA_BLEND
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static u32 adjust_alpha(u32 col)
{
	u32 alpha = col>>24;
	u8 mul;
	u32 c1,c2;

	if(alpha==0)    return col;
	if(alpha==0xff) return col;

	c1 = col & 0x00ff00ff;
	c2 = col & 0x0000ff00;
	mul = (u8)(255-alpha);
	c1 = ((c1*mul)>>8)&0x00ff00ff;
	c2 = ((c2*mul)>>8)&0x0000ff00;
	return (alpha<<24)|c1|c2;
}
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//int blit_setup(int sx,int sy,const char *msg,int fg_col,int bg_col)
int blit_setup(void)
{
	int unk;
	sceDisplayGetMode(&unk, &pwidth, &pheight);
	sceDisplayGetFrameBuf((void*)&vram32, &bufferwidth, &pixelformat, &unk);
	if( (bufferwidth==0) || (pixelformat!=3)) return -1;

	fcolor = 0x00ffffff;
	bcolor = 0xff000000;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// blit text
/////////////////////////////////////////////////////////////////////////////
void blit_set_color(int fg_col,int bg_col)
{
	fcolor = fg_col;
	bcolor = bg_col;
}

/////////////////////////////////////////////////////////////////////////////
// blit text
/////////////////////////////////////////////////////////////////////////////
int blit_string(int sx,int sy,const char *msg)
{
	int x,y,p;
	int offset;
	char code;
	unsigned char font;
	u32 fg_col,bg_col;
	int charsize = 1;

	if (pwidth == 720) // tv out
		charsize++;

	sx *= charsize;
	sy *= charsize;

	u32 col,c1,c2;
	u32 alpha;

	fg_col = adjust_alpha(fcolor);
	bg_col = adjust_alpha(bcolor);	

//Kprintf("MODE %d WIDTH %d\n",pixelformat,bufferwidth);
	if( (bufferwidth==0) || (pixelformat!=3)) return -1;

	for(x = 0; msg[x]; x++)
	{
		code = msg[x] & 0x7f; // 7bit ANK
		for(y = 0; y < (8*charsize); y++)
		{
			if (pheight == 503) // composite or component-interlace
			{			
				if (((sy+y) & 1))
				{
					offset = (((sy+y) / 2)+262) * bufferwidth + sx+x*(8*charsize);
				}
				else
				{
					offset = ((sy+y) / 2) * bufferwidth + sx+x*(8*charsize);
				}
			}
			else
			{
				offset = (sy+y) * bufferwidth + sx+x*(8*charsize);
			}

			if (charsize == 2)			
				font = y>=15 ? 0x00 : msx[code*8 + y/2];
			else
				font = y>=7 ? 0x00 : msx[code*8 + y];

			for(p = 0; p < 8; p++)
			{
				col = (font & 0x80) ? fg_col : bg_col;
				alpha = col>>24;
				if(alpha==0 || pheight == 503) 
				{
					vram32[offset] = col;
					
					if (charsize == 2)
						vram32[offset+1] = col;
				}
				else if(alpha!=0xff)
				{
					c2 = vram32[offset];
					c1 = c2 & 0x00ff00ff;
					c2 = c2 & 0x0000ff00;
					c1 = ((c1*alpha)>>8)&0x00ff00ff;
					c2 = ((c2*alpha)>>8)&0x0000ff00;
					vram32[offset] = (col&0xffffff) + c1 + c2;

					if (charsize == 2)
						vram32[offset+1] = vram32[offset];
				}

				font <<= 1;
				offset += charsize;
			}
		}
	}
	return x;
}

int blit_string_ctr(int sy,const char *msg)
{
	int sx = 480/2-paf_strlen(msg)*(8/2);
	return blit_string(sx,sy,msg);
}
