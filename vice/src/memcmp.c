#include <string.h>

int
memcmp (s1, s2, n)
     const void *s1;
     const void *s2;
     size_t n;
{
  const char *p1 = (const char *) s1;
  const char *p2 = (const char *) s2;

  while (n-- > 0)
    {
      if (*p1 < *p2)
	return -1;
      else if (*p1 > *p2)
	return +1;
      p1++, p2++;
    }

  return 0;
}
