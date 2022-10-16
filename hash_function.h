#ifndef __HASH_FUNCTION_H
#define __HASH_FUNCTION_H

/*
 *Routines to test the hash are included if SELF_TEST is defined.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
typedef  unsigned long  int  ub4;  
typedef  unsigned       char ub1;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

static ub4 hash(register ub1 *k, register ub4 length, register ub4 level);

/*
 *mix -- mix 4 32-bit values reversibly.
 */

#define mix(a,b,c,d) \
{ \
  a += d; d += a; a ^= (a>>7);  \
  b += a; a += b; b ^= (b<<13);  \
  c += b; b += c; c ^= (c>>17); \
  d += c; c += d; d ^= (d<<9); \
  a += d; d += a; a ^= (a>>3);  \
  b += a; a += b; b ^= (b<<7);  \
  c += b; b += c; c ^= (c>>15); \
  d += c; c += d; d ^= (d<<11); \
}

static inline ub4 hash(register ub1 *k, register ub4 length, register ub4 level)
{
   register ub4 a,b,c,d,len;
   len = length;
   a = b = c = 0x9e3779b9;
   d = level;               
    while (len >= 16)
   {
      a += (k[0] +((ub4)k[1]<<8) +((ub4)k[2]<<16) +((ub4)k[3]<<24));
      b += (k[4] +((ub4)k[5]<<8) +((ub4)k[6]<<16) +((ub4)k[7]<<24));
      c += (k[8] +((ub4)k[9]<<8) +((ub4)k[10]<<16)+((ub4)k[11]<<24));
      d += (k[12]+((ub4)k[13]<<8)+((ub4)k[14]<<16)+((ub4)k[15]<<24));
      mix(a,b,c,d);
      k += 16; len -= 16;
   }
   d += length;
   switch(len)            
   {
   case 15: d+=((ub4)k[14]<<24);
   case 14: d+=((ub4)k[13]<<16);
   case 13: d+=((ub4)k[12]<<8);
   case 12: c+=((ub4)k[11]<<24);
   case 11: c+=((ub4)k[10]<<16);
   case 10: c+=((ub4)k[9]<<8);
   case 9 : c+=k[8];
   case 8 : b+=((ub4)k[7]<<24);
   case 7 : b+=((ub4)k[6]<<16);
   case 6 : b+=((ub4)k[5]<<8);
   case 5 : b+=k[4];
   case 4 : a+=((ub4)k[3]<<24);
   case 3 : a+=((ub4)k[2]<<16);
   case 2 : a+=((ub4)k[1]<<8);
   case 1 : a+=k[0];
  }
   mix(a,b,c,d);
   /*--report the result-- */
   return d;
}
#endif

