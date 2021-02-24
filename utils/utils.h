//
// Created by lazuli on 2/18/21.
//

#ifndef M68K_UTILS_INCLUDE_GUARD
#define M68K_UTILS_INCLUDE_GUARD

#define UNEXPECTEDERROR() \
	unexpectederror(__FILE__, __func__, __LINE__);

void unexpectederror(const char* file, const char* func, int line);
int fstbitindex(uint32_t bit);
uint32_t changebit(uint32_t num, uint32_t bit, int pos);

#endif /* M68K_UTILS_INCLUDE_GUARD */