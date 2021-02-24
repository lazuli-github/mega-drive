#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "../utils/utils.h"

uint8_t memspace[] = {};
struct cartmem cartmem = {};
struct ram ram = {};

static void
memerror(const char *file, const char *func, int line, const char *msg)
{
	fprintf(stderr, "*** MEMORY ERROR ***\n");
	fprintf(stderr, "\t%s\n", msg);
#ifndef NDEBUG
	fprintf(stderr, "\tfile: %s\n", file);
	fprintf(stderr, "\tfunction: %s\n", func);
	fprintf(stderr, "\tline: %d: \n", line);
#endif
	exit(1);
}

void
writemem(uint32_t addr, uint32_t data, int size)
{
	if (addr + size > sizeof(memspace) - 1)
		MEMERROR("Out of bounds memory access");
	switch (size) {
	case 1:
		((uint8_t *) memspace)[addr] = (uint8_t) data;
		break;
	case 2:
		((uint16_t *) memspace)[addr] = (uint16_t) data;
		break;
	case 4:
		((uint32_t *) memspace)[addr] = (uint32_t) data;
		break;
	default:
		UNEXPECTEDERROR();
	}
}

uint32_t
readmem(uint32_t addr, int size)
{
	switch (size) {
	case 1:
		return ((uint8_t *) memspace)[addr];
	case 2:
		return ((uint16_t *) memspace)[addr];
	case 4:
		return ((uint32_t *) memspace)[addr];
	default:
		UNEXPECTEDERROR();
	}
}

void
meminit(void)
{
	/* XXX: watch out for off-by-one bugs. The cartridge memory starts at
	 * 0x000000 and ends at 0x3fffff, so its size is NOT 0x3fffff. Because
	 * of the first address 0x000000, its size is actually 0x3fffff + 1. */
	cartmem.mem = &memspace[0x000000];
	cartmem.size = 0x3fffff + 1;
	ram.mem = &memspace[0xff0000];
	ram.size = 0xffff + 1;
}