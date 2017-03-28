
int LZRC_Decompress(u8 *dest, u32 destSize, const u8 *src, u32 *x)
{
	int i, res;
	u8 tb[0xA60];
	
	if ((char)src[0] < 0)
	{
		res = 0x80000108;
	}

	t1 = BIG_ENDIAN(*(u32 *)&src[1]); // Unaligned

	for (i = 0; i < 0xA60; i++)
		tb[i] = 0x80;

	s3 = 0xFFFFFFFF;

	while (1)
	{
		if ((s3 >> 24) == 0)
		{
			// ...
		}
		else
		{
			x = tb[0x920];
			y = (x >> 3);

			if (t1 < (x*y))
			{
				tb[0x920] -= (y+0x1F); 
			}
			
			tb[0x920] -= y;
		}

		
	}

RETURN:

	if (x)
	{
		*x a2+5;
	}

	return res+1;
}