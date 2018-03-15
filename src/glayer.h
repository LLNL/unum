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

#ifndef GLAYER_H_
#define GLAYER_H_

#include "gmp.h"

/* pg 66
Definition: The #g-layer# is the scratchpad where results are computed 
such that they are always correct to the smallest representable 
uncertainty when they are returned to the u-layer. 
*/

#if 0
/* example gnum_s fields, see pg 65 */
typedef struct {
	unsigned long long frac;
	unsigned int expo;
	unsigned int fneg : 1;
	unsigned int eneg : 1;
	unsigned int open : 1;
	unsigned int inf : 1;
} gnum_s;
#endif

/* Implement the values "frac & expo" with GMP mpf_t. */
/* The booleans: fneg & eneg are discernible from the mpf_t structure. */

typedef struct {
	mpf_t f;
	unsigned int open : 1;
	unsigned int inf : 1;
} gnum_s;
typedef gnum_s gnum_t[1];

/* pg 65
Definition: A #gbound# is the data structure used for temporary 
calculations at higher precision than in the unum environment; that is, 
the scratchpad. 
*/
typedef struct {
	gnum_s l;
	gnum_s r;
	unsigned int nan : 1;
} gbnd_s;
typedef gbnd_s gbnd_t[1];

typedef enum {RE=0, LE=2} end_t;

#if defined(__cplusplus)
extern "C" {
#endif

void gnum_init(gnum_s *gn);
void gnum_clear(gnum_s *gn);

void gbnd_init(gbnd_s *gb);
void gbnd_clear(gbnd_s *gb);

#if defined (__cplusplus)
}
#endif

	/*
	Comparison of interval end points

	Key:
	C = closed
	O = open
	L = left interval end
	R = right interval end
	- = less than
	0 = equal to
	+ = greater than

	C,L C,L : [n  [n  : 0
	C,L C,R : [n   n] : 0
	C,R C,L :  n] [n  : 0
	C,R C,R :  n]  n] : 0

	C,L O,L : [n  (n  : -
	C,L O,R : [n   n) : +
	C,R O,L :  n] (n  : -
	C,R O,R :  n]  n) : +

	O,L C,L : (n  [n  : +
	O,L C,R : (n   n] : +
	O,R C,L :  n) [n  : -
	O,R C,R :  n)  n] : -

	O,L O,L : (n  (n  : 0
	O,L O,R : (n   n) : +
	O,R O,L :  n) (n  : -
	O,R O,R :  n)  n) : 0

	   0 1 2
	   R C L
	0R 0 - -
	1C + 0 -
	2L + + 0
	*/

static inline int cmp_gn(const gnum_s *x, end_t xe, const gnum_s *y, end_t ye)
{
	int res;

	if ( x->inf && !y->inf) return  mpf_sgn(x->f);
	if (!x->inf &&  y->inf) return -mpf_sgn(y->f);
	if ((res = mpf_cmp(x->f, y->f)) != 0) return res;
	return ((x->open)?xe:1) - ((y->open)?ye:1);
}

/* Negates numerical part of an endpoint. */

static inline void neg_gn(gnum_s *a, const gnum_s *x)
{
	mpf_neg(a->f, x->f);
	a->inf  = x->inf;
	a->open = x->open;
}

#endif /* GLAYER_H_ */
