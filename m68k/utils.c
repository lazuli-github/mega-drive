#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void
unexpectederror(const char* file, const char* func, int line)
{
	fprintf(stderr, "*** UNEXPECTED ERROR ***\n");
#ifdef NDEBUG
	fprintf(stderr, "\tAn unexpected error that could not be handled happened\n");
#else
	fprintf(stderr, "\tfile: %s\n", file);
	fprintf(stderr, "\tfunction: %s\n", func);
	fprintf(stderr, "\tline: %d: \n", line);
#endif
	exit(1);
}


/*
 * Returns the first bit set in a uint32_t number, counting from the least
 * significant bit to the most significant bit.
 * If there are no bits set, a negative number is returned.
 *
 * The algorithm works by using the de Bruijn sequence B(2, 5), which is
 * 0x077CB531. This sequence has the special property of generating unique
 * 5 bit permutations in the 5 most significant bits when multiplied by a
 * power of two, ie., binary numbers that have a single bit set. So we can
 * uniquely identify any 32 bits power of two using just 5 bits (2^5 = 32).
 *
 * However, not all numbers are powers of two, but two's complement has the
 * useful property of preserving only the first bit that was set in its
 * unsigned representation. So when multiplying the unsigned integer 27
 * by -1, it's going to result in a signed integer and preserve only the first
 * bit that was set in 27. By using the AND operation, we can extract this
 * single bit into a new integer with that single bit set and then multiply
 * it by the de Bruijn sequence 0x077CB531 to get a unique 5 bits permutation.
 *
 * The lookup table can then constructed by checking the results of the
 * multiplication for all possible 32 bits powers, where each element
 * represents the index of the bit and the table elements are indexed by
 * the result of the multiplication.
 *
 * This was the fastest way of calculating the first bit index without
 * using non portable solutions such as the ffsl POSIX function.
 */
int
fstbitindex(uint32_t bit)
{
	const uint32_t seq = 0x077CB531UL;
	int r;
	static const int table[32] = {
		 0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
		31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9,
	};

	r = table[((bit & -bit) * seq) >> 27];
	r |= (bit == 0) * (-1);
	if (bit == 0)
		assert(r < 0);
	return r;
}

/*
 * Changes the bit of num at the index pos.
 * bit must be 0 or 1 and pos must be less than 32,
 * this way nor the bit variable nor the 1 literal
 * will overflow.
 */
uint32_t
changebit(uint32_t num, uint32_t bit, int pos)
{
	uint32_t newnum = (num & ~((uint32_t) 1 << pos)) | (bit << pos);

	assert(bit == 0 || bit == 1);
	assert(pos < 32 && pos >= 0);

	return newnum;
}
