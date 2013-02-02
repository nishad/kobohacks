/*
 * my_neon.h
 * little arm neon helper
 *
 * Copyright (c) 2009-2010 Jan Seiffert
 *
 * This file is part of g2cd.
 *
 * g2cd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * g2cd is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with g2cd.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id: $
 */

#ifndef MY_NEON_H
# define MY_NEON_H

# if defined(__ARM_NEON__)
#  include <arm_neon.h>
#  define SOVUCQ sizeof(uint8x16_t)
#  define SOVUC sizeof(uint8x8_t)

static inline uint8x16_t neon_simple_alignq(uint8x16_t a, uint8x16_t b, unsigned amount)
{
	switch(amount % SOVUCQ)
	{
	case  0: return a;
	case  1: return vextq_u8(a, b,  1);
	case  2: return vextq_u8(a, b,  2);
	case  3: return vextq_u8(a, b,  3);
	case  4: return vextq_u8(a, b,  4);
	case  5: return vextq_u8(a, b,  5);
	case  6: return vextq_u8(a, b,  6);
	case  7: return vextq_u8(a, b,  7);
	case  8: return vextq_u8(a, b,  8);
	case  9: return vextq_u8(a, b,  9);
	case 10: return vextq_u8(a, b, 10);
	case 11: return vextq_u8(a, b, 11);
	case 12: return vextq_u8(a, b, 12);
	case 13: return vextq_u8(a, b, 13);
	case 14: return vextq_u8(a, b, 14);
	case 15: return vextq_u8(a, b, 15);
	}
	return b;
}

static inline uint8x8_t neon_simple_align(uint8x8_t a, uint8x8_t b, unsigned amount)
{
	switch(amount % SOVUC)
	{
	case  0: return a;
	case  1: return vext_u8(a, b,  1);
	case  2: return vext_u8(a, b,  2);
	case  3: return vext_u8(a, b,  3);
	case  4: return vext_u8(a, b,  4);
	case  5: return vext_u8(a, b,  5);
	case  6: return vext_u8(a, b,  6);
	case  7: return vext_u8(a, b,  7);
	}
	return b;
}
# endif

# define ACMP_MSK 0x000F0000
# define ACMP_SHR 16
# define ACMP_SHL 12
# define ACMP_NRB 4

static inline unsigned long alu_ucmp_gte_sel(unsigned long a, unsigned long b, unsigned long sel_a, unsigned long sel_b)
{
	unsigned long t, sel;

	/* a >= b ? sel_a : sel_b */
	asm(
		/*          Rn >= Rm -> 1 */
		"usub8	%0, %2, %3\n\t"
		/*   1 -> Rn  Rm <- 0 */
		"sel	%1, %4, %5"
		: /* %0 */ "=&r" (t),
		  /* %1 */ "=&r" (sel)
		: /* %2 */ "r" (a),
		  /* %3 */ "r" (b),
		  /* %4 */ "r" (sel_a),
		  /* %5 */ "r" (sel_b)
	);
	return sel;
}

# define alu_ucmp_eqz_msk(a) alu_ucmp_gte_msk(0, a)
static inline unsigned long alu_ucmp_gte_msk(unsigned long a, unsigned long b)
{
	unsigned long res;

	/* a >= b ? 1 : 0 */
	asm(
		/*          Rn >= Rm -> 1 */
		"usub8	%0, %1, %2\n\t"
		"mrs	%0, CPSR" /* we want the APSR, but gas doesn't know about it */
		: /* %0 */ "=r" (res)
		: /* %1 */ "r" (a),
		  /* %2 */ "r" (b)
	);
	/* bit 16 to 19 */
	return res;
}

# define alu_ucmp16_eqz_msk(a) alu_ucmp16_gte_msk(0, a)
static inline unsigned long alu_ucmp16_gte_msk(unsigned long a, unsigned long b)
{
	unsigned long res;

	/* a >= b ? 1 : 0 */
	asm(
		/*          Rn >= Rm -> 1 */
		"usub16	%0, %1, %2\n\t"
		"mrs	%0, CPSR" /* we want the APSR, but gas doesn't know about it */
		: /* %0 */ "=r" (res)
		: /* %1 */ "r" (a),
		  /* %2 */ "r" (b)
	);
	/* bit 16 to 19 */
	return res;
}

# if (__GNUC__ > 4) || (__GNUC__ == 4)
#  define ctlz(a) __builtin_clz(a)
#  define cttz(a) __builtin_ctz(a)
# else
static inline unsigned ctlz(size_t a)
{
	unsigned r;
	asm("clz	%0, %1" : "=r" (r) : "r" (a));
	return r;
}

static inline unsigned cttz(size_t a)
{
	size_t ;
	a = (size_t)(-((ssize_t)a)) & a;
	return 31 - ctlz(a);
}
# endif

# define arm_nul_byte_index_b(x) ((HOST_IS_BIGENDIAN) ? ctlz((x)) : cttz((x)))
# define arm_nul_byte_index_e(x) ((HOST_IS_BIGENDIAN) ? ctlz((x) << ACMP_SHL) : cttz((x) >> ACMP_SHR))
# define arm_nul_byte_index_e_last(x) ((HOST_IS_BIGENDIAN) ? (SOSTM1-cttz((x) >> ACMP_SHR)) : (SOSTM1-ctlz((x) << ACMP_SHL)))
#endif
