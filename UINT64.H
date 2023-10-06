#ifndef UINT64_H
#define UINT64_H

#if defined(__TURBOC__) && defined(__MSDOS__)
typedef unsigned long uint32;
typedef unsigned int  uint16;
typedef long int32;
typedef int  int16;
typedef int Bool;
static int clz(uint32 x) {
	if (x == 0) return 32;
	int n = 0;
	if (x <= 0x0000FFFFul) { n += 16; x <<= 16; }
	if (x <= 0x00FFFFFFul) { n += 8;  x <<= 8; }
	if (x <= 0x0FFFFFFFul) { n += 4;  x <<= 4; }
	if (x <= 0x3FFFFFFFul) { n += 2;  x <<= 2; }
	if (x <= 0x7FFFFFFFul) ++n;
	return n;
}
#else
typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef int   int32;
typedef short int16;
typedef bool Bool;
static_assert(sizeof(uint32) == 4 && sizeof(int32) == 4, "Incorrectly sized typedef for 32-bit integers!");
static_assert(sizeof(uint16) == 2 && sizeof(int16) == 2, "Incorrectly sized typedef for 16-bit integers!");
#ifdef _MSC_VER
#include <intrin.h>
#define clz __lzcnt
#else
#define clz __builtin_clz
#endif
#endif

#define HI16(x) ((x)>>16)
#define LO16(x) ((uint16)(x))
#define MAKE32(hi, lo) (((uint32)(hi) << 16) | (uint32)(lo))

// Multiword integer division, according to Knuth's algorithm D.
static void divmnu(uint16 *q, uint16 *r, const uint16 *u, const uint16 *v, int m, int n)
{
	uint32 quot, rem, k = 0;
	int i;
	if (n == 1) // Special case if divisor < 64K
	{
		for(int j = m-1; j >= 0; --j)
		{
			uint32 div = MAKE32(k, u[j]);
			q[j] = LO16(div / v[0]);
			k = div - q[j]*v[0];
		}
		r[0] = (uint16)k;
		return;
	}
	// Normalize u and v to un and vn.
	uint32 s = clz(v[n-1]) - 16; // 0 <= s <= 15.
	uint16 vn[8], un[10]; // Input n and m must be <= 4.
	un[m] = u[m-1] >> (16 - s);
	for(i = n-1; i > 0; --i) vn[i] = (v[i] << s) | (v[i-1] >> (16-s));
	for(i = m-1; i > 0; --i) un[i] = (u[i] << s) | (u[i-1] >> (16-s));
	vn[0] = v[0] << s;
	un[0] = u[0] << s;
	for(int j = m-n; j >= 0; --j)
	{
		uint32 div = MAKE32(un[j+n], un[j+n-1]);
		quot = div / vn[n-1]; // Calculate estimated quotient
		rem = div - quot*vn[n-1];
again:
		if (HI16(quot) || quot*vn[n-2] > MAKE32(rem, un[j+n-2]))
		{
			--quot;
			rem += vn[n-1];
			if (!HI16(rem)) goto again;
		}
		k = 0;
		int32 t;
		for(int i = 0; i < n; ++i)
		{
			uint32 p = quot * vn[i];
			t = un[i+j] - k - LO16(p);
			un[i+j] = LO16(t);
			k = HI16(p) - HI16(t);
		}
		t = un[j+n] - k;
		un[j+n] = LO16(t);
		q[j] = LO16(quot);
		if (t < 0)
		{
			--q[j];
			k = 0;
			for(int i = 0; i < n; ++i)
			{
				t = k + un[i+j] + vn[i];
				un[i+j] = LO16(t);
				k = HI16(t);
			}
			un[j+n] += LO16(k);
		}
	}
	// Unnormalize and pass back remainder.
	for(i = 0; i < n; ++i) r[i] = (un[i] >> s) | (un[i+1] << (16-s));
}

class uint64
{
public:
	uint32 lo, hi;
	uint64() {}
	uint64(uint32 hi, uint32 lo):hi( hi), lo(lo) {}
	uint64(          int lo):hi(-(lo<0)), lo(lo) {}
	uint64( unsigned int lo):hi(      0), lo(lo) {}
	uint64(         long lo):hi(-(lo<0)), lo(lo) {}
	uint64(unsigned long lo):hi(      0), lo(lo) {}

	uint64 operator+(      uint32  rhs) const { uint32 Lo = lo +  rhs;    return    uint64(         hi + (Lo < lo), Lo); }
	uint64 operator+(const uint64 &rhs) const { uint32 Lo = lo +  rhs.lo; return    uint64(rhs.hi + hi + (Lo < lo), Lo); }
	uint64 operator-(      uint32  rhs) const { uint32 Lo =  1 + ~rhs;    return *this + uint64(0xFFFFFFFFul + !Lo, Lo); }
	uint64 operator-(const uint64 &rhs) const { uint32 Lo =  1 + ~rhs.lo; return *this + uint64(     ~rhs.hi + !Lo, Lo); }

	uint64 operator *(int16 rhs) const { return *this * (uint16)rhs; }
	uint64 operator *(int32 rhs) const { return *this * (uint32)rhs; }
	uint64 operator *(uint16 rhs) const
	{
		const uint16 *u = (const uint16*)this;
		uint32 tmp0 = (uint32)u[0]*rhs;
		uint32 tmp1 = (uint32)u[1]*rhs + HI16(tmp0);
		uint32 tmp2 = (uint32)u[2]*rhs + HI16(tmp1);
		uint32 tmp3 = (uint32)u[3]*rhs + HI16(tmp2);
		return uint64(MAKE32(     tmp3,  LO16(tmp2)),
									MAKE32(LO16(tmp1), LO16(tmp0)));
	}

	uint64 operator *(uint32 rhs) const
	{
		const uint16 *u = (const uint16*)this;
		const uint16 *v = (const uint16*)&rhs;
		uint32 tmp0 = (uint32)u[0]*v[0];
		uint32 tmp1 = (uint32)u[1]*v[0] + HI16(tmp0);
		uint32 tmp2 = (uint32)u[2]*v[0] + HI16(tmp1);
		uint32 tmp3 = (uint32)u[0]*v[1] + LO16(tmp1);
		uint32 tmp4 = (uint32)u[1]*v[1] + LO16(tmp2) + HI16(tmp3);
		uint32 tmp5 = (uint32)u[2]*v[1] + (uint32)u[3]*v[0] + HI16(tmp2);
		return uint64(MAKE32(tmp5, 0) + tmp4, MAKE32(tmp3, LO16(tmp0)));
	}

	uint64 operator *(const uint64 &rhs) const
	{
		const uint16 *u = (const uint16*)this;
		const uint16 *v = (const uint16*)&rhs;
		uint32 tmp0 = (uint32)u[0]*v[0];
		uint32 tmp1 = (uint32)u[1]*v[0] + HI16(tmp0);
		uint32 tmp2 = (uint32)u[2]*v[0] + HI16(tmp1);
		uint32 tmp3 = (uint32)u[3]*v[0] + HI16(tmp2);
		uint32 tmp4 = (uint32)u[0]*v[1] + LO16(tmp1);
		uint32 tmp5 = (uint32)u[1]*v[1] + LO16(tmp2) + HI16(tmp4);
		uint32 tmp6 = (uint32)u[0]*v[2] + LO16(tmp5);
		uint32 tmp7 = (uint32)u[1]*v[2] + (uint32)u[0]*v[3] + (uint32)u[2]*v[1] + LO16(tmp3) + HI16(tmp5);
		return uint64(MAKE32(tmp7, 0) + tmp6, MAKE32(tmp4, LO16(tmp0)));
	}

	int size_uint16s() const // Counts how many digits this number has in "base 64K".
	{
		if (hi >> 16) return 4;
		if (hi)       return 3;
		if (lo >> 16) return 2;
		return 1; // 0 is deliberately counted to have one digit.
	}

	void div_rem(const uint64 &rhs, uint64 *quot, uint64 *rem) const
	{
		int m = size_uint16s();
		int n = rhs.size_uint16s();
		quot->lo = quot->hi = rem->lo = rem->hi = 0;
		divmnu((uint16*)quot, (uint16*)rem, (uint16*)&lo, (uint16*)&rhs, m>n?m:n, n);
	}

	uint64 operator /(const uint64 &rhs) const
	{
		if (rhs.hi > hi || (rhs.hi == hi && rhs.lo > lo)) return uint64(0, 0);
		if (!(rhs.hi|hi)) return uint64(0, lo / rhs.lo);
		uint64 quot, rem;
		div_rem(rhs, &quot, &rem);
		return quot;
	}

	uint64 operator %(const uint64 &rhs) const
	{
		uint64 quot, rem;
		div_rem(rhs, &quot, &rem);
		return rem;
	}

	uint64 &operator++()    { if (!++lo) ++hi; return *this; }
	uint64 &operator--()    { if (!lo--) --hi; return *this; }
	uint64  operator++(int) { uint64 old = *this; if (!++lo) ++hi; return old; }
	uint64  operator--(int) { uint64 old = *this; if (!lo--) --hi; return old; }

	uint64 &operator+=(uint32 rhs) { *this = *this + rhs; return *this; }
	uint64 &operator-=(uint32 rhs) { *this = *this - rhs; return *this; }
	uint64 &operator*=(uint32 rhs) { *this = *this * rhs; return *this; }
	uint64 &operator/=(uint32 rhs) { *this = *this / rhs; return *this; }
	uint64 &operator%=(uint32 rhs) { *this = *this % rhs; return *this; }
	uint64 &operator&=(uint32 rhs) { *this = *this & rhs; return *this; }
	uint64 &operator|=(uint32 rhs) { *this = *this | rhs; return *this; }
	uint64 &operator^=(uint32 rhs) { *this = *this ^ rhs; return *this; }
	uint64 &operator<<=(uint32 rhs) { *this = *this << rhs; return *this; }
	uint64 &operator>>=(uint32 rhs) { *this = *this >> rhs; return *this; }
	uint64 &operator+=(const uint64 &rhs) { *this = *this + rhs; return *this; }
	uint64 &operator-=(const uint64 &rhs) { *this = *this - rhs; return *this; }
	uint64 &operator*=(const uint64 &rhs) { *this = *this * rhs; return *this; }
	uint64 &operator/=(const uint64 &rhs) { *this = *this / rhs; return *this; }
	uint64 &operator%=(const uint64 &rhs) { *this = *this % rhs; return *this; }
	uint64 &operator&=(const uint64 &rhs) { *this = *this & rhs; return *this; }
	uint64 &operator|=(const uint64 &rhs) { *this = *this | rhs; return *this; }
	uint64 &operator^=(const uint64 &rhs) { *this = *this ^ rhs; return *this; }

	Bool operator > (uint32 rhs) const { return  hi || lo >  rhs; }
	Bool operator >=(uint32 rhs) const { return  hi || lo >= rhs; }
	Bool operator < (uint32 rhs) const { return !hi && lo <  rhs; }
	Bool operator <=(uint32 rhs) const { return !hi && lo <= rhs; }
	Bool operator ==(uint32 rhs) const { return !hi && lo == rhs; }
	Bool operator !=(uint32 rhs) const { return  hi || lo != rhs; }
	Bool operator > (const uint64 &rhs) const { return  hi >  rhs.hi || (hi == rhs.hi && lo >  rhs.lo); }
	Bool operator >=(const uint64 &rhs) const { return  hi >  rhs.hi || (hi == rhs.hi && lo >= rhs.lo); }
	Bool operator < (const uint64 &rhs) const { return  hi <  rhs.hi || (hi == rhs.hi && lo <  rhs.lo); }
	Bool operator <=(const uint64 &rhs) const { return  hi <  rhs.hi || (hi == rhs.hi && lo <= rhs.lo); }
	Bool operator ==(const uint64 &rhs) const { return (lo == rhs.lo) & (hi == rhs.hi); }
	Bool operator !=(const uint64 &rhs) const { return (lo != rhs.lo) | (hi != rhs.hi); }

	Bool operator &&(uint32 rhs) const { return (lo|hi) && (rhs); }
	Bool operator ||(uint32 rhs) const { return  lo|hi  |   rhs; }
	Bool operator &&(const uint64 &rhs) const { return (lo|hi) && (rhs.lo|rhs.hi); }
	Bool operator ||(const uint64 &rhs) const { return  lo|hi  |   rhs.lo|rhs.hi; }

	uint64 operator &(uint32 rhs) const { return uint64( 0, lo&rhs); }
	uint64 operator |(uint32 rhs) const { return uint64(hi, lo|rhs); }
	uint64 operator ^(uint32 rhs) const { return uint64(hi, lo^rhs); }
	uint64 operator &(const uint64 &rhs) const { return uint64(rhs.hi&hi, lo&rhs.lo); }
	uint64 operator |(const uint64 &rhs) const { return uint64(rhs.hi|hi, lo|rhs.lo); }
	uint64 operator ^(const uint64 &rhs) const { return uint64(rhs.hi^hi, lo^rhs.lo); }

	uint64 operator <<(uint32 rhs) const
	{
		if (rhs >= 64) return uint64(0);
		if (rhs >= 32) return uint64(lo<<(rhs-32), 0);
		if (rhs == 0) return *this;
		return uint64((hi<<rhs) | (lo>>(32-rhs)),   lo<<rhs);
	}
	uint64 operator >>(uint32 rhs) const
	{
		if (rhs >= 64) return uint64(0);
		if (rhs >= 32) return uint64(0, hi>>(rhs-32));
		if (rhs == 0) return *this;
		return uint64( hi>>rhs,   (hi<<(32-rhs)) | (lo>>rhs));
	}

	uint64 operator -() const { return uint64(~hi, ~lo) + 1; }
	uint64 operator ~() const { return uint64(~hi, ~lo); }
	uint64 operator !() const { return !(hi|lo); }

	char *to_str() const
	{
		// Largest 64-bit number is 9,223,372,036,854,775,807, i.e.
		// 19 digits (+1 for empty character)
		// This function is not MT-safe, but if you don't have 64-bit integers
		// you might not have threads either.
		static char str[128];
		static int pos = 0;
		char *s = str+pos;
		int i = 0;

		const uint64 div(1000000000ul);
		uint64 tmp = *this;
		while(tmp != 0)
		{
			uint64 quotient, remainder;
			tmp.div_rem(div, &quotient, &remainder);
			tmp = quotient;
			for(int j = 0; remainder.lo != 0 || (quotient != 0 && j < 9); ++j)
			{
				s[i++] = '0' + (remainder.lo % 10);
				remainder.lo /= 10;
			}
		}
		if (i == 0) s[i++] = '0';

		// reverse string
		for(int j = 0; j < i/2; ++j)
		{
			char tmp = s[j];
			s[j] = s[i-j-1];
			s[i-j-1] = tmp;
		}
		s[i++] = 0;
		pos = (i+20 > sizeof(str)) ? 0 : i;
		return s;
	}

	static uint64 from_str(const char *str)
	{
		uint64 u(0);
		while(*str >= '0' && *str <= '9')
			u = u * uint16(10) + (*str++ - '0');
		return u;
	}
};
#endif
