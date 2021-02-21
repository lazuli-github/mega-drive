#include <stdio.h>
#include <stdint.h>

#include "m68k/cpu.h"
#include "m68k/utils.h"

int
main(void)
{
	m68kinit();
	printf("%d\n", fstbitindex(0b10000));

	return 0;
}
