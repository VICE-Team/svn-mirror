#include <stdio.h>

/*
 *	Like strchr, except has a length limit.
 */
char *
strnchr(char *s, int c, int n)
{
	while (n--)
		if (*s == c) return s; else ++s;
	return NULL;
}

