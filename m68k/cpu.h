#ifndef M68K_CPU_INCLUDE_GUARD
#define M68K_CPU_INCLUDE_GUARD

#include <stdint.h>

/*
 * The register a[7] is used as a hardware stack pointer
 * during stacking for subroutine calls and exception handling.
 * In the user programming model, a[7] refers to the user stack
 * pointer (USP). In supervisor programming model register, a[7]
 * refers to the supervisor stack pointer (SSP). This is implemented
 * by setting addregsp[7] to usrsp or supsp.
 *
 * The lower 8 bits of the Status Register is the only portion
 * of it available in the user mode. This is an illustration of
 * the Status Register (0 means that the bit is not used):
 * ----------------------------------	exit(1);--------------------------------
 * T1 | T0 | S | M | 0 | l2 | l1 | l0 | 0 | 0 | 0 | X | N | Z | V | C
 * ------------------------------------------------------------------
 * |_______| |   |     |______________|             |   |   |   |   |_ CARRY
 *     |     |   |             |                    |   |   |   |_____ OVERFLOW
 *   TRACE   |   |         INTERRUPT                |   |   |_________ ZERO
 *   ENABLE  |   |       PRIORITY MASK              |   |_____________ NEGATIVE
 *           |   |                                  |_________________ EXTEND
 * SUPERVISOR/   |
 * USER STATE    |
 *               |
 * MASTER/INTERRUPT
 * STATE
 *             ______________________________________
 *             | T1 | T0 | TRACE MODE               |
 *             --------------------------------------
 *             | 0  | 0  | NO TRACE                 |
 *             | 1  | 0  | TRACE ON ANY INSTRUCTION |
 *             | 0  | 1  | TRACE ON CHANGE OF FLOW  |
 *             | 1  | 1  | UNDEFINED                |
 *             --------------------------------------
 * The M bit is not needed for the 68000.
 *
 * A data register that is either a source or a destination operand
 * only uses or changes the appropriate lower 8 or 16 bits (in byte or
 * word operations, respectively). The remaining high-order portion
 * does not change and goes unused.
 */
struct m68kregs {
	uint32_t  progctr;
	uint16_t  sttsreg;
	uint16_t  usrsp;
	uint16_t  supsp;
	uint32_t  dataregs[8];
	uint32_t  addrregs[8];
	uint32_t* addrregsp[8];
};

typedef uint8_t m68ksttsbit;
typedef uint32_t m68kregvalue;

typedef enum m68ksttsmask {
	M68KSTTS_CARRY    = 0x1,
	M68KSTTS_OVERFLOW = 0x2,
	M68KSTTS_ZERO     = 0x4,
	M68KSTTS_NEGATIVE = 0x8,
	M68KSTTS_EXTEND   = 0x10,
	/* 0              = 0x20, */
	/* 0              = 0x40, */
	/* 0              = 0x80, */
	M68KSTTS_L0       = 0x100,
	M68KSTTS_L1       = 0x200,
	M68KSTTS_L2       = 0x400,
	/* 0              = 0x800, */
	M68KSTTS_MASSTATE = 0x1000,
	M68KSTTS_SUPSTATE = 0x2000,
	M68KSTTS_T0       = 0x4000,
	M68KSTTS_T1       = 0x8000,
} m68ksttsmask;

/*
 * There are two levels of privilege. User mode gives access to
 * everything except privileged instructions such as interrupt
 * level controls. Supervisor privilege gives access to everything.
 */
typedef enum M68KPRIVLVL {
	M68KUSR = 0,
	M68KSUP = 1,
};

typedef enum m68kregnames {
	M68KREG_PC, /* progctr */
	M68KREG_SR, /* sttsreg */

	M68KREG_USP, /* usrsp */
	M68KREG_SSP, /* supsp */

	/* dataregs[0 - 7] */
	M68KREG_D0,
	M68KREG_D1,
	M68KREG_D2,
	M68KREG_D3,
	M68KREG_D4,
	M68KREG_D5,
	M68KREG_D6,
	M68KREG_D7,

	/* *addrregsp[0 - 7] */
	M68KREG_A0,
	M68KREG_A1,
	M68KREG_A2,
	M68KREG_A3,
	M68KREG_A4,
	M68KREG_A5,
	M68KREG_A6,
	M68KREG_A7,
} m68kreg;

typedef enum m68kaddrmode {
	M68KADDRMODE_DIMM,
	M68KADDRMODE_AIMM,
	M68KADDRMODE_IND,
	/* TODO: add more addressing modes */
} m68kaddrmode;

typedef enum m68ksize {
	M68KBYTE  = 1, /* Byte  */
	M68KWORD  = 2, /* Bytes */
	M68KLWORD = 4, /* Bytes */
} m68ksize;

void m68kinit(void);
m68ksttsbit m68kgetsttsbit(m68ksttsmask bitname);

#endif /* M68K_CPU_INCLUDE_GUARD */