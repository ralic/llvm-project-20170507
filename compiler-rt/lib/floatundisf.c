//===-- floatundisf.c - Implement __floatundisf ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements __floatundisf for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"
#include <float.h>

// Returns: convert a to a float, rounding toward even.

// Assumption: float is a IEEE 32 bit floating point type 
//             du_int is a 64 bit integral type

// seee eeee emmm mmmm mmmm mmmm mmmm mmmm

float
__floatundisf(du_int a)
{
    if (a == 0)
        return 0.0F;
    const unsigned N = sizeof(du_int) * CHAR_BIT;
    int sd = N - __builtin_clzll(a);  // number of significant digits
    int e = sd - 1;             // exponent
    if (sd > FLT_MANT_DIG)
    {
        //  start:  0000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQxxxxxxxxxxxxxxxxxx
        //  finish: 000000000000000000000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQR
        //                                                12345678901234567890123456
        //  1 = msb 1 bit
        //  P = bit FLT_MANT_DIG-1 bits to the right of 1
        //  Q = bit FLT_MANT_DIG bits to the right of 1
        //  R = "or" of all bits to the right of Q
        switch (sd)
        {
        case FLT_MANT_DIG + 1:
            a <<= 1;
            break;
        case FLT_MANT_DIG + 2:
            break;
        default:
            a = (a >> (sd - (FLT_MANT_DIG+2))) |
                ((a & ((du_int)(-1) >> ((N + FLT_MANT_DIG+2) - sd))) != 0);
        };
        // finish:
        a |= (a & 4) != 0;  // Or P into R
        ++a;  // round - this step may add a significant bit
        a >>= 2;  // dump Q and R
        // a is now rounded to FLT_MANT_DIG or FLT_MANT_DIG+1 bits
        if (a & ((du_int)1 << FLT_MANT_DIG))
        {
            a >>= 1;
            ++e;
        }
        // a is now rounded to FLT_MANT_DIG bits
    }
    else
    {
        a <<= (FLT_MANT_DIG - sd);
        // a is now rounded to FLT_MANT_DIG bits
    }
    float_bits fb;
    fb.u = ((e + 127) << 23)       |  // exponent
           ((su_int)a & 0x007FFFFF);  // mantissa
    return fb.f;
}
