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

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include "ulayer.h"

/* serves as a companion to ulayer */

typedef struct {
	unsigned int fsize;
	unsigned int esize;
	unsigned int ubit;
} utag_s;
typedef utag_s utag_t[1];

#if defined(__cplusplus)
extern "C" {
#endif

void utag(utag_s *ut, const unum_s *u);

void signmask(unum_s *a, const unum_s *u);
void bigu(unum_s *a, const unum_s *u);

long scale(const mpf_s *f);
int ne(const mpf_s *f);

int inexQ(const unum_s *u);
int exQ(const unum_s *u);
int infuQ(const unum_s *u);
int nanuQ(const unum_s *u);

void promotef(unum_s *a, const unum_s *u);
void promotee(unum_s *a, const unum_s *u);
void promote(unum_s *a, unum_s *b, const unum_s *u, const unum_s *v);
void demotef(unum_s *a, const unum_s *u);
void demotee(unum_s *a, const unum_s *u);

#if defined (__cplusplus)
}
#endif

#endif /* SUPPORT_H_ */
