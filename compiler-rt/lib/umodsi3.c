//===-- umodsi3.c - Implement __umodsi3 -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements __umodsi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a % b

su_int
__umodsi3(su_int a, su_int b)
{
    return a - (a / b) * b;
}
