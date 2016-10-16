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

#ifndef ULAYER_H_
#define ULAYER_H_

#include "mpx.h"

/* pg 63
Definition: The #u-layer# is the level of computer arithmetic where all 
the operands are unums (and data structures made from unums, like 
ubounds). 
*/

/* pg 4
Definition: A #unum# is a bit string of variable length that has six 
sub-fields: Sign bit, exponent, fraction, uncertainty bit, exponent 
size, and fraction size. 
*/
typedef mpx_s unum_s;
typedef mpx_t unum_t;

/* pg 62
Definition: A #ubound# is a single unum or a pair of unums that 
represent a mathematical interval of the real line. Closed endpoints are 
represented by exact unums, and open endpoints are represented by 
inexact unums. 
*/
typedef struct {
	unsigned char p;
	unum_s *l;
	unum_s *r;
} ubnd_s;
typedef ubnd_s ubnd_t[1];

#define UN_VAR(un) \
  MPX_VAR(un)

#define UB_VAR(ub) \
  MPX_VAR(_##ub##A); \
  MPX_VAR(_##ub##B); \
  ubnd_t ub = {{0, _##ub##A, _##ub##B}}

#if defined(__cplusplus)
extern "C" {
#endif

void unum_init(unum_s *un);
void unum_clear(unum_s *un);
void unum_copy(unum_s *dst, const unum_s *src);

void ubnd_init(ubnd_s *ub);
void ubnd_clear(ubnd_s *ub);
void ubnd_copy(ubnd_s *dst, const ubnd_s *src);

#if defined (__cplusplus)
}
#endif

#endif /* ULAYER_H_ */
