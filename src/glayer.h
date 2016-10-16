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

#endif /* GLAYER_H_ */
