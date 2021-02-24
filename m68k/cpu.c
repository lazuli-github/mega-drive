#include <assert.h>

#include "cpu.h"
#include "../memory/memory.h"
#include "../utils/utils.h"

static struct m68kregs m68kregs = {};
static int INITIALIZED = 0;

m68kregvalue
m68kgetreg(m68kreg reg)
{
	switch (reg) {
	case M68KREG_PC:
		return m68kregs.progctr;
	case M68KREG_SR:
		return m68kregs.sttsreg;
	case M68KREG_USP:
		return m68kregs.usrsp;
	case M68KREG_SSP:
		return m68kregs.supsp;

	case M68KREG_D0:
		return m68kregs.dataregs[0];
	case M68KREG_D1:
		return m68kregs.dataregs[1];
	case M68KREG_D2:
		return m68kregs.dataregs[2];
	case M68KREG_D3:
		return m68kregs.dataregs[3];
	case M68KREG_D4:
		return m68kregs.dataregs[4];
	case M68KREG_D5:
		return m68kregs.dataregs[5];
	case M68KREG_D6:
		return m68kregs.dataregs[6];
	case M68KREG_D7:
		return m68kregs.dataregs[7];

	case M68KREG_A0:
		return *(m68kregs.addrregsp[0]);
	case M68KREG_A1:
		return *(m68kregs.addrregsp[1]);
	case M68KREG_A2:
		return *(m68kregs.addrregsp[2]);
	case M68KREG_A3:
		return *(m68kregs.addrregsp[3]);
	case M68KREG_A4:
		return *(m68kregs.addrregsp[4]);
	case M68KREG_A5:
		return *(m68kregs.addrregsp[5]);
	case M68KREG_A6:
		return *(m68kregs.addrregsp[6]);
	case M68KREG_A7:
		return *(m68kregs.addrregsp[7]);

	default:
		UNEXPECTEDERROR();
	}
}

void
m68ksetreg(m68kreg reg, m68kregvalue value)
{
	switch (reg) {
	case M68KREG_PC:
		m68kregs.progctr = value;
		break;
	case M68KREG_SR:
		m68kregs.sttsreg = value;
		break;
	case M68KREG_USP:
		m68kregs.usrsp = value;
		break;
	case M68KREG_SSP:
		m68kregs.supsp = value;
		break;

	case M68KREG_D0:
		m68kregs.dataregs[0] = value;
		break;
	case M68KREG_D1:
		m68kregs.dataregs[1] = value;
		break;
	case M68KREG_D2:
		m68kregs.dataregs[2] = value;
		break;
	case M68KREG_D3:
		m68kregs.dataregs[3] = value;
		break;
	case M68KREG_D4:
		m68kregs.dataregs[4] = value;
		break;
	case M68KREG_D5:
		m68kregs.dataregs[5] = value;
		break;
	case M68KREG_D6:
		m68kregs.dataregs[6] = value;
		break;
	case M68KREG_D7:
		m68kregs.dataregs[7] = value;
		break;

	case M68KREG_A0:
		*(m68kregs.addrregsp[0]) = value;
		break;
	case M68KREG_A1:
		*(m68kregs.addrregsp[1]) = value;
		break;
	case M68KREG_A2:
		*(m68kregs.addrregsp[2]) = value;
		break;
	case M68KREG_A3:
		*(m68kregs.addrregsp[3]) = value;
		break;
	case M68KREG_A4:
		*(m68kregs.addrregsp[4]) = value;
		break;
	case M68KREG_A5:
		*(m68kregs.addrregsp[5]) = value;
		break;
	case M68KREG_A6:
		*(m68kregs.addrregsp[6]) = value;
		break;
	case M68KREG_A7:
		*(m68kregs.addrregsp[7]) = value;
		break;

	default:
		UNEXPECTEDERROR();
	}
}

/*
 * Each status register bit has a mask assigned to it in the m68ksttsmask.
 * This mask is also used to refer to each bit.
 */
m68ksttsbit
m68kgetsttsbit(m68ksttsmask bitname)
{
	uint16_t sr = m68kgetreg(M68KREG_SR);
	int pos = fstbitindex(bitname);

	/* Bits in the status register higher than the extend bit can't be accessed in user mode. */
	if (INITIALIZED && m68kgetreg(M68KREG_SR) | M68KSTTS_SUPSTATE && bitname > M68KSTTS_EXTEND)
		return 0; /* Or should I throw some kind of error? */
	return (sr >> pos) & 0x1;
}

void
m68ksetsttsbit(m68ksttsmask bitname, m68ksttsbit value)
{
	uint16_t sr = m68kgetreg(M68KREG_SR), newsr;
	int pos = fstbitindex(bitname);

	/* Bits in the status register higher than the extend bit can't be accessed in user mode. */
	if (INITIALIZED && m68kgetreg(M68KREG_SR) | M68KSTTS_SUPSTATE && pos > M68KSTTS_EXTEND)
		return;
	newsr = changebit(sr, value, pos);
	m68ksetreg(M68KREG_SR, newsr);
}

/*
 * https://en.wikibooks.org/wiki/68000_Assembly#Addressing_Modes
 * TODO: Make this function shorter and write tests.
 */
void
m68kins_move(m68kreg dst, m68kaddrmode dstmode, m68kreg src, m68kaddrmode srcmode, m68ksize size)
{
	int isdstind = 0, issrcind = 0;
	m68kregvalue dstvalue, srcvalue, result = 0;
	uint32_t dstaddr, srcaddr;

	switch (dstmode) {
	case M68KADDRMODE_DIMM:
		dstvalue = m68kgetreg(dst);
		break;
	case M68KADDRMODE_AIMM:
		dstvalue = m68kgetreg(dst);
		break;
	case M68KADDRMODE_IND:
		isdstind = 1;
		if (dst >= M68KREG_A0 && dst <= M68KREG_A7 && size != M68KLWORD)
			UNEXPECTEDERROR();
		dstaddr = m68kgetreg(dst);
		break;
	default:
		UNEXPECTEDERROR();
	}
	switch (srcmode) {
	case M68KADDRMODE_DIMM:
		srcvalue = m68kgetreg(src);
		break;
	case M68KADDRMODE_AIMM:
		srcvalue = m68kgetreg(src);
		break;
	case M68KADDRMODE_IND:
		issrcind = 1;
		if (dst >= M68KREG_A0 && dst <= M68KREG_A7 && size != M68KLWORD)
			UNEXPECTEDERROR();
		srcaddr = m68kgetreg(dst);
		srcvalue = readmem(srcaddr, size);
		break;
	default:
		UNEXPECTEDERROR();
	}

	/* Moving directly from memory to memory is not possible. */
	if (isdstind && issrcind)
		UNEXPECTEDERROR();

	if (isdstind) {
		writemem(dstaddr, srcvalue, size);
	} else {
		switch (size) {
		case M68KBYTE:
			result = (dstvalue & 0xffffff00) | (srcvalue & 0x000000ff);
			break;
		case M68KWORD:
			result = (dstvalue & 0xffff0000) | (srcvalue & 0x0000ffff);
			break;
		case M68KLWORD:
			result = (dstvalue & 0x00000000) | (srcvalue & 0xffffffff);
			break;
		default:
			UNEXPECTEDERROR();
		}

		if (result & 0x80)
			m68ksetsttsbit(M68KSTTS_NEGATIVE, 1);
		else
			m68ksetsttsbit(M68KSTTS_NEGATIVE, 0);
		if (!result)
			m68ksetsttsbit(M68KSTTS_ZERO, 1);
		else
			m68ksetsttsbit(M68KSTTS_ZERO, 1);
		m68ksetsttsbit(M68KSTTS_OVERFLOW, 0);
		m68ksetsttsbit(M68KSTTS_CARRY, 0);

		m68ksetreg(dst, result);
	}
}

/*
 * Initialize the CPU by setting up the pointer of address registers
 * and entering supervisor mode.
 */
void
m68kinit(void)
{
	int i;

	for (i = 0; i < 7; i++)
		m68kregs.addrregsp[i] = &m68kregs.addrregs[i];
	m68kregs.addrregsp[7] = (uint32_t *) &m68kregs.supsp;
	m68ksetsttsbit(M68KSTTS_SUPSTATE, 1);
	/* Ensures that the CPU is in supervisor mode. */
	assert(m68kgetreg(M68KREG_SR) >= 0b0010000000000000);

	INITIALIZED = 1;
}
