#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "types.h"

void ErrorExit(char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);
	exit(-1);
}

int main(int argc, char *argv[])
{
	FILE *i, *o;
	u8 ch;
	
	if (argc != 3)
	{
		ErrorExit("Usage %s input.bin output.txt\n", argv[0]);
	}

	i = fopen(argv[1], "rb");
	if (!i)
	{
		ErrorExit("Cannot open %s.\n", argv[1]);
	}

	o = fopen(argv[2], "w");
	if (!o)
	{
		fclose(i);
		ErrorExit("Cannot create %s.\n", argv[2]);
	}

	while (fread(&ch, 1, 1, i) == 1)
	{
		fprintf(o, ".byte\t0x%02X\n", ch);
	}

	fclose(i);
	fclose(o);

	printf("Done.\n");
	
	return 0;
}

