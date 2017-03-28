/*
 * Our little libc :) 
*/


int strlen(char *str)
{
	int n = 0;

	while (*str++)
		n++;

	return n;
}

// char *strcpy(char *str1, char *str2) -> oops, built-in function,
// Less coding :)