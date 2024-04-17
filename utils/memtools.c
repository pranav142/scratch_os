#include "memtools.h"

void *memcpy(void *dest, const void *src, uint32_t n) {
  char *d = (char *)dest;
  const char *s = (const char *)src;
  while (n--) {
    *d++ = *s++;
  }
  return dest;
}

void *memset(void *ptr, int value, uint32_t n) {
  unsigned char *p = (unsigned char *)ptr;
  while (n--) {
    *p++ = (unsigned char)value;
  }
  return ptr;
}

int memcmp(const void *s1, const void *s2, uint32_t n) {
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  for (uint32_t i = 0; i < n; i++)
    if (p1[i] != p2[i])
      return 1;

  return 0;
}
