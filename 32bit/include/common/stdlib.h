#ifndef STDLIB_H
#define STDLIB_H

void memcpy(void* dest, void* src, int bytes);
void bzero(void* dest, int bytes);
char* itoa(int i);
int strcmp(const char *s1, const char *s2);

#endif
