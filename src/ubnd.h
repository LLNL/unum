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

#ifndef UBND_H_
#define UBND_H_

#include "ulayer.h" /* ubnd_s */
#include "glayer.h" /* end_t */

#define MAX_NBITS 185 /* up to env 4,6 */

typedef struct {
	long long int ubitsmoved;
	long long int ubndsmoved;
	long long int ops; /* operations */
	long long int opc2[3][3]; /* operand class for 2 operand operations */
	long long int opc3[3][3][3]; /* operand class for 3 operand operations */
#if defined(NBITS_HISTO)
	long long int nbits[MAX_NBITS+1]; /* histogram of nbits in operands */
#endif
} ustats_t;

extern ustats_t stats;

#if defined(__cplusplus)
extern "C" {
#endif

int ltuQ(const ubnd_s *u, const ubnd_s *v);
int gtuQ(const ubnd_s *u, const ubnd_s *v);
int nequQ(const ubnd_s *u, const ubnd_s *v);
int nnequQ(const ubnd_s *u, const ubnd_s *v);
int sameuQ(const ubnd_s *u, const ubnd_s *v);
int cmpuQ(const ubnd_s *u, end_t ue, const ubnd_s *v, end_t ve);

int spanszerouQ(const ubnd_s *u);
void intersectuQ(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);

void plusu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
void minusu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
void timesu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
void divideu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);

void powu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
void squareu(ubnd_s *a, const ubnd_s *u);
void sqrtu(ubnd_s *a, const ubnd_s *u);
void negateu(ubnd_s *a, const ubnd_s *u);
void absu(ubnd_s *a, const ubnd_s *u);
void expu(ubnd_s *a, const ubnd_s *u);
void logu(ubnd_s *a, const ubnd_s *u);
void cosu(ubnd_s *a, const ubnd_s *u);
void sinu(ubnd_s *a, const ubnd_s *u);
void tanu(ubnd_s *a, const ubnd_s *u);
void cotu(ubnd_s *a, const ubnd_s *u);

void minu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
void maxu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);

int cliplu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);
int cliphu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v);

#if defined (__cplusplus)
}
#endif

#endif /* UBND_H_ */
