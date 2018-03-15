/*
 * Copyright (c) 2016, Lawrence Livermore National Security, LLC. 
 * Produced at the Lawrence Livermore National Laboratory. Written by
 * G. Scott Lloyd, lloyd23@llnl.gov. LLNL-CODE-704762. All rights reserved.
 * 
 * This file is part of Unum. For details, see
 * http://github.com/LLNL/unum
 * 
 * Please also read COPYING – Our Notice and GNU Lesser General Public 
 * License. 
 * 
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License (as published by the 
 * Free Software Foundation) version 2.1 dated February 1999. 
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and 
 * conditions of the GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

#ifndef UENV_H_
#define UENV_H_

#include "mpx.h"

#define MAX_ESIZESIZE 4U
#define MAX_FSIZESIZE 11U
#define MAX_ESIZE (1U << MAX_ESIZESIZE)
#define MAX_FSIZE (1U << MAX_FSIZESIZE)
#define MAX_UBITS (2U + MAX_ESIZE + MAX_FSIZE + MAX_ESIZESIZE + MAX_FSIZESIZE)

/* Number of bits available in MPX variable (unum) */
#define UBITS maxubits

/* Number of limbs available in MPX variable (unum) */
#define NLIMBS ulimbs
#define MAX_ULIMBS MPX_BITS2LIMBS(MAX_UBITS)

/* +1 for hidden bit, +1 for extra precision in calc. */
#define FSIZE2PREC(n) ((n)+2+128)

/* Precision in bits of MPF variable */
#define PBITS pbits
#define MAX_PBITS FSIZE2PREC(MAX_FSIZE)

/* Precision in limbs of MPF variable */
#define PLIMBS plimbs
#define MAX_PLIMBS MPF_BITS2LIMBS(MAX_PBITS)

#if (1 + MAX_ESIZESIZE + MAX_FSIZESIZE) > GMP_NUMB_BITS
#error "utag size exceeds GMP limb size"
#endif

extern int esizesize;
extern int fsizesize;
extern int esizemax;
extern int fsizemax;
extern int utagsize;
extern int maxubits;

extern mp_size_t ulimbs;
extern mp_size_t plimbs;
extern mp_bitcnt_t pbits;

extern mpx_t one;
extern mpx_t ubitmask;
extern mpx_t fsizemask;
extern mpx_t esizemask;
extern mpx_t efsizemask;
extern mpx_t utagmask;
extern mpx_t ulpu;
extern mpx_t smallsubnormalu;
extern mpx_t smallnormalu;
extern mpx_t signbigu;
extern mpx_t posinfu;
extern mpx_t maxrealu;
extern mpx_t minrealu;
extern mpx_t neginfu;
extern mpx_t negbigu;
extern mpx_t qNaNu;
extern mpx_t sNaNu;
extern mpx_t negopeninfu;
extern mpx_t posopeninfu;
extern mpx_t negopenzerou;

extern mpf_t maxreal;
extern mpf_t smallsubnormal;

#if defined(__cplusplus)
extern "C" {
#endif

void init_uenv(void);
void clear_uenv(void);
void set_uenv(int e, int f);

#if defined (__cplusplus)
}
#endif

/* TODO: add uenv_s *arg to functions

typedef struct uenv_s;

void init_uenv(uenv_s *uenv, int e, int f);
void set_uenv(uenv_s *uenv);
void view_uenv(uenv_s *uenv);

*/

#endif /* UENV_H_ */
