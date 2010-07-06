/* -*- mode: C; tab-width: 2; indent-tabs-mode: nil; fill-column: 79; coding: iso-latin-1-unix -*- */

#ifndef HPCC_H
#define HPCC_H 1

/* HPL includes:
stdio.h
stdlib.h
string.h
stdarg.h
vararg.h (if necessary)
mpi.h
*/

#define MPIFFT_TIMING_COUNT 8

/* Define 64-bit types and corresponding format strings for printf() and constants */
#ifdef LONG_IS_64BITS
typedef unsigned long u64Int;
typedef long s64Int;
#define FSTR64 "%ld"
#define FSTRU64 "%lu"
#define ZERO64B 0L
#else
typedef unsigned long long u64Int;
typedef long long s64Int;
#define FSTR64 "%lld"
#define FSTRU64 "%llu"
#define ZERO64B 0LL
#endif

#ifdef LONG_IS_64BITS
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#else
#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#endif


#endif
