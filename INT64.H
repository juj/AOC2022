#ifndef INT64_H
#define INT64_H

#include <assert.h>
#include "../uint64.h"

class int64
{
public:
	unsigned long lo;
	long hi;
	int64(long hi, unsigned long lo) :hi(hi), lo(lo) {}
	int64(int val) :hi(0), lo(val) {}
	int64(unsigned int val) :hi(0), lo(val) {}
	int64(unsigned long val) :hi(0), lo(val) {}
	int64(long val):hi(val<0?-1:0), lo(val) {}

	int64 operator *(long val) const
	{
		int sign = 0;
		if (val < 0)
		{
			sign = -1;
			val = -val;
		}
		assert(hi >= 0); // TODO
		assert(val <= 32767); // TODO
		unsigned long Lo1 = (lo >> 16) * val;
		unsigned long Hi = hi * val + (Lo1>>16);
		unsigned long LoA = (lo & 0xFFFF) * val;
		unsigned long LoB = ((Lo1 & 0xFFFF) << 16);
		unsigned long Lo = LoA + LoB;
		if (Lo < LoA) ++Hi;
		int64 num(Hi, LoA+LoB);
		if (sign)
		{
			num.hi = ~num.hi;
			num.lo = ~num.lo;
			if (num.lo == 0xFFFFFFFFul)
			{
				num.lo = 0;
				++num.hi;
			}
			else
				++num.lo;
		}
		return num;
	}

	char *to_str() const
	{

		return ((uint64*)this)->to_str();

		static char str[256];
		char *s = str;
		int i = 0;

		unsigned long Hi = hi;
		unsigned long Lo = lo;
		if (hi < 0)
		{
			*s++ = '-';
			Hi = ~Hi;
			Lo = ~Lo;
			if (Lo == 0xFFFFFFFFul)
			{
				Lo = 0;
				++Hi;
			}
			else
				++Lo;
		}

		unsigned long remainder;
		unsigned long quotient = 0;//div64_by32(Hi, Lo, 1000000000ul, &remainder);
		assert(remainder != 0xFFFFFFFFu);
		while(remainder)
		{
			s[i++] = '0' + (remainder % 10);
			remainder /= 10;
		}
		while(quotient)
		{
			s[i++] = '0' + (quotient % 10);
			quotient /= 10;
		}

		// reverse string
		for(int j = 0; j < i/2; ++j)
		{
			int tmp = s[j];
			s[j] = s[i-j-1];
			s[i-j-1] = tmp;
		}
		s[i] = 0;
		return str;
	}
};

#endif
