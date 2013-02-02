/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-2011 Mark Adler
 * Copyright (C) 2010-2011 Jan Seiffert
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include <stdio.h>
#include <unistd.h>

#define GCC_ATTR_UNUSED_PARAM __attribute__((__unused__))
#define GCC_ATTR_USED_VAR __attribute__((__used__))

/*
 * This is not the original adler32.c from the zlib distribution,
 * but a heavily modified version. If you are looking for the
 * original, please go to zlib.net.
 */

#define BASE 65521UL    /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* use NO_DIVIDE if your processor does not do division in hardware --
   try it both ways to see which is faster */
#ifdef NO_DIVIDE
/* note that this assumes BASE is 65521, where 65536 % 65521 == 15
   (thank you to John Reiser for pointing this out) */
#  define CHOP(a) \
    do { \
        unsigned long tmp = a >> 16; \
        a &= 0xffffUL; \
        a += (tmp << 4) - tmp; \
    } while (0)
#  define MOD28(a) \
    do { \
        CHOP(a); \
        if (a >= BASE) a -= BASE; \
    } while (0)
#  define MOD(a) \
    do { \
        CHOP(a); \
        MOD28(a); \
    } while (0)
#  define MOD63(a) \
    do { /* this assumes a is not negative */ \
        z_off64_t tmp = a >> 32; \
        a &= 0xffffffffL; \
        a += (tmp << 8) - (tmp << 5) + tmp; \
        tmp = a >> 16; \
        a &= 0xffffL; \
        a += (tmp << 4) - tmp; \
        tmp = a >> 16; \
        a &= 0xffffL; \
        a += (tmp << 4) - tmp; \
        if (a >= BASE) a -= BASE; \
    } while (0)
#else
#  define CHOP(a) a %= BASE
#  define MOD(a) a %= BASE
#  define MOD28(a) a %= BASE
#  define MOD63(a) a %= BASE
#endif


#ifndef MIN_WORK
# define MIN_WORK 16
#endif

/* ========================================================================= */
static noinline uint32_t adler32_1(uint32_t adler, const uint8_t *buf, unsigned len GCC_ATTR_UNUSED_PARAM)
{
	uint32_t sum2;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	adler += buf[0];
	if(adler >= BASE)
		adler -= BASE;
	sum2 += adler;
	if(sum2 >= BASE)
		sum2 -= BASE;
	return adler | (sum2 << 16);
}

/* ========================================================================= */
static noinline uint32_t adler32_common(uint32_t adler, const uint8_t *buf, unsigned len)
{
	uint32_t sum2;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	while(len--) {
		adler += *buf++;
		sum2 += adler;
	}
	if(adler >= BASE)
		adler -= BASE;
	/* only added so many BASE's */
	MOD28(sum2);
	return adler | (sum2 << 16);
}

#ifndef HAVE_ADLER32_VEC
#  if (defined(__LP64__) || ((SIZE_MAX-0) >> 31) >= 2) && !defined(NO_ADLER32_VEC)

/* On 64 Bit archs, we can do pseudo SIMD with a nice win.
 * This is esp. important for old Alphas, they do not have byte
 * access.
 * This needs some register but x86_64 is fine (>= 9 for the mainloop
 * req.). If your 64 Bit arch is more limited, throw it away...
 */
#  ifndef UINT64_C
#   if defined(_MSC_VER) || defined(__BORLANDC__)
#    define UINT64_C(c)    (c ## ui64)
#   else
#    define UINT64_C(c)    (c ## ULL)
#   endif
#  endif

#  undef VNMAX
#  define VNMAX (2*NMAX+((9*NMAX)/10))

static noinline uint32_t adler32_vec(uint32_t adler, const uint8_t *buf, unsigned len)
{
	uint32_t s1, s2;
	unsigned k;

	/* split Adler-32 into component sums */
	s1 = adler & 0xffff;
	s2 = (adler >> 16) & 0xffff;

	/* align input data */
	k    = ALIGN_DIFF(buf, sizeof(uint64_t));
	len -= k;
	if(k) do {
		s1 += *buf++;
		s2 += s1;
	} while(--k);

	k = len > VNMAX ? VNMAX : len;
	len -= k;
	if(likely(k >= 2 * sizeof(uint64_t))) do
	{
		uint32_t vs1, vs2;
		uint32_t vs1s;

		/* add s1 to s2 for rounds to come */
		s2 += s1 * ROUND_TO(k, sizeof(uint64_t));
		vs1s = vs1 = vs2 = 0;
		do
		{
			uint64_t vs1l = 0, vs1h = 0, vs1l_s = 0, vs1h_s = 0;
			uint64_t a, b, c, d, e, f, g, h;
			unsigned j;

			j = k > 23 * sizeof(uint64_t) ? 23 : k/sizeof(uint64_t);
			k -= j * sizeof(uint64_t);
			/* add s1 to s1 round sum for rounds to come */
			vs1s += j * vs1;
			do
			{
				uint64_t in8 = *(const uint64_t *)buf;
				buf += sizeof(uint64_t);
				/* add this s1 to s1 round sum */
				vs1l_s += vs1l;
				vs1h_s += vs1h;
				/* add up input data to s1 */
				vs1l +=  in8 & UINT64_C(0x00ff00ff00ff00ff);
				vs1h += (in8 & UINT64_C(0xff00ff00ff00ff00)) >> 8;
			} while(--j);

			/* split s1 */
			if(HOST_IS_BIGENDIAN)
			{
				a = (vs1h >> 48) & 0x0000ffff;
				b = (vs1l >> 48) & 0x0000ffff;
				c = (vs1h >> 32) & 0x0000ffff;
				d = (vs1l >> 32) & 0x0000ffff;
				e = (vs1h >> 16) & 0x0000ffff;
				f = (vs1l >> 16) & 0x0000ffff;
				g = (vs1h      ) & 0x0000ffff;
				h = (vs1l      ) & 0x0000ffff;
			}
			else
			{
				a = (vs1l      ) & 0x0000ffff;
				b = (vs1h      ) & 0x0000ffff;
				c = (vs1l >> 16) & 0x0000ffff;
				d = (vs1h >> 16) & 0x0000ffff;
				e = (vs1l >> 32) & 0x0000ffff;
				f = (vs1h >> 32) & 0x0000ffff;
				g = (vs1l >> 48) & 0x0000ffff;
				h = (vs1h >> 48) & 0x0000ffff;
			}

			/* add s1 & s2 horiz. */
			vs2 += 8*a + 7*b + 6*c + 5*d + 4*e + 3*f + 2*g + 1*h;
			vs1 += a + b + c + d + e + f + g + h;

			/* split and add up s1 round sum */
			vs1l_s = ((vs1l_s      ) & UINT64_C(0x0000ffff0000ffff)) +
			         ((vs1l_s >> 16) & UINT64_C(0x0000ffff0000ffff));
			vs1h_s = ((vs1h_s      ) & UINT64_C(0x0000ffff0000ffff)) +
			         ((vs1h_s >> 16) & UINT64_C(0x0000ffff0000ffff));
			vs1l_s += vs1h_s;
			vs1s += ((vs1l_s      ) & UINT64_C(0x00000000ffffffff)) +
			        ((vs1l_s >> 32) & UINT64_C(0x00000000ffffffff));
		} while(k >= sizeof(uint64_t));
		CHOP(vs1s);
		s2 += vs1s * 8 + vs2;
		CHOP(s2);
		s1 += vs1;
		CHOP(s1);
		len += k;
		k = len > VNMAX ? VNMAX : len;
		len -= k;
	} while(k >= sizeof(uint64_t));

	/* handle trailer */
	if(k) do {
		s1 += *buf++;
		s2 += s1;
	} while (--k);
	MOD28(s1);
	MOD28(s2);

	/* return recombined sums */
	return (s2 << 16) | s1;
}
# else
static noinline uint32_t adler32_vec(uint32_t adler, const uint8_t *buf, unsigned len)
{
	uint32_t sum2;
	unsigned n;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;

	/* do length NMAX blocks -- requires just one modulo operation */
	while(len >= NMAX)
	{
		len -= NMAX;
		n = NMAX / 16;	/* NMAX is divisible by 16 */
		do {
			DO16(buf);	/* 16 sums unrolled */
			buf += 16;
		} while(--n);
		MOD(adler);
		MOD(sum2);
	}

	/* do remaining bytes (less than NMAX, still just one modulo) */
	if(len)
	{	/* avoid modulos if none remaining */
		while(len >= 16) {
			len -= 16;
			DO16(buf);
			buf += 16;
		}
		while (len--) {
			adler += *buf++;
			sum2 += adler;
		}
		MOD(adler);
		MOD(sum2);
	}

	/* return recombined sums */
	return adler | (sum2 << 16);
}
# endif
#endif

/* ========================================================================= */
#if MIN_WORK - 16 > 0
#  ifndef NO_ADLER32_GE16
static noinline uint32_t adler32_ge16(uint32_t adler, const uint8_t *buf, unsigned len)
{
	uint32_t sum2;
	unsigned n;

	/* split Adler-32 into component sums */
	sum2 = (adler >> 16) & 0xffff;
	adler &= 0xffff;
	n = len / 16;
	len %= 16;

	do {
		DO16(buf); /* 16 sums unrolled */
		buf += 16;
	} while(--n);

	/* handle trailer */
	while(len--) {
		adler += *buf++;
		sum2 += adler;
	}

	MOD28(adler);
	MOD28(sum2);

	/* return recombined sums */
	return adler | (sum2 << 16);
}
#  endif
#  define COMMON_WORK 16
#else
#  define COMMON_WORK MIN_WORK
#endif

/* ========================================================================= */
uint32_t adler32(uint32_t adler, const uint8_t *buf, unsigned len)
{
	/* in case user likes doing a byte at a time, keep it fast */
	if(1 == len)
		return adler32_1(adler, buf, len); /* should create a "fast" tailcall */

	/* initial Adler-32 value (deferred check for len == 1 speed) */
	if(buf == NULL)
		return 1L;

	/* in case short lengths are provided, keep it somewhat fast */
	if(unlikely(len < COMMON_WORK))
		return adler32_common(adler, buf, len); /* should create a "fast" tailcall */
#if MIN_WORK - 16 > 0
	if(unlikely(len < MIN_WORK))
		return adler32_ge16(adler, buf, len); /* should create a "fast" tailcall */
#endif

	return adler32_vec(adler, buf, len);
}

static char const rcsid_a32g[] GCC_ATTR_USED_VAR = "$Id: $";
/* EOF */
