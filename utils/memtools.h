#pragma once

#include <stdint.h>

void *memset(void *ptr, int value, uint32_t n);
void *memcpy(void *dest, const void *src, uint32_t n);
int memcmp(const void *s1, const void *s2, uint32_t n);
