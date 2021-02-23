#ifndef MEMORY_INCLUDE_GUARD
#define MEMORY_INCLUDE_GUARD

#include <stdint.h>

#define MEMERROR(msg) \
	memerror(__FILE__, __func__, __LINE__, msg);

/* This memory space is going to map all the Mega Drive memory as seen here:
 * https://en.wikibooks.org/wiki/Genesis_Programming */
extern uint8_t memspace[0xffffff];

struct cartmem {
	uint8_t *mem;
	uint32_t size;
};

struct ram {
	uint8_t *mem;
	uint32_t size;
};

void writemem(uint32_t addr, uint32_t data, int size);
uint32_t readmem(uint32_t addr, int size);
void meminit(void);

#endif /* MEMORY_INCLUDE_GUARD */
