#include <pspsdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *memcpy(void *m1, const void *m2, size_t size)
{
	int i;
	u8 *p1 = (u8 *)m1;
	u8 *p2 = (u8 *)m2;

	for (i = 0; i < size; i++)
	{
		p1[i] = p2[i];
	}

	return m1;
}

int strcasecmp(const char *s1, const char *s2)
{
	int i;
	char ch1, ch2;

	for (i = 0; ; i++)
	{
		ch1 = s1[i];
		ch2 = s2[i];

		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 0x20;

		if (ch2 >= 'a' && ch2 <= 'z')
			ch2 -= 0x20;

		if (ch1 != ch2)
		{
			return ch2 - ch1;
		}

		if (ch1 == 0)
			break;
	}

	return 0;
}

