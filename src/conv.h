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

#ifndef CONV_H_
#define CONV_H_

#include "ulayer.h"
#include "glayer.h"

#if defined(__cplusplus)
extern "C" {
#endif

int midpoint(mpf_s *a, const gbnd_s *g);
void roundu(unum_s *un);

/*-------- gbnd conversion --------*/
gbnd_s *si2g(gbnd_s *g, signed long si);
gbnd_s *ui2g(gbnd_s *g, unsigned long ui);
gbnd_s *d2g(gbnd_s *g, double d);
gbnd_s *f2g(gbnd_s *g, const mpf_s *f);
//gbnd_s *s2g(gbnd_s *g, const char *s, int base);
//gbnd_s *x2g(gbnd_s *g, FILE *stream, int base);
signed long g2si(const gbnd_s *g);
unsigned long g2ui(const gbnd_s *g);
double g2d(const gbnd_s *g);
mpf_s *g2f(mpf_s *f, const gbnd_s *g);
//char *g2s(char *s, const gbnd_s *g, int base); /* size_t digits, e vs f? */
//void g2x(FILE *stream, const gbnd_s *g, int base); /* size_t digits, e vs f? */

/*-------- unum conversion --------*/
unum_s *si2un(unum_s *un, signed long si);
unum_s *ui2un(unum_s *un, unsigned long ui);
unum_s *d2un(unum_s *un, double d);
signed long un2si(const unum_s *un);
unsigned long un2ui(const unum_s *un);
double un2d(const unum_s *un);

/*-------- ubnd conversion --------*/
ubnd_s *si2ub(ubnd_s *ub, signed long si);
ubnd_s *ui2ub(ubnd_s *ub, unsigned long ui);
ubnd_s *d2ub(ubnd_s *ub, double d);
signed long ub2si(const ubnd_s *ub);
unsigned long ub2ui(const ubnd_s *ub);
double ub2d(const ubnd_s *ub);

/*-------- other conversion --------*/
void u2f(mpf_s *f, const unum_s *u);
void f2u(unum_s *u, const mpf_s *f);

void unum2g(gbnd_s *a, const unum_s *u);
void ubnd2g(gbnd_s *a, const ubnd_s *ub);
void u2g(gbnd_s *a, const ubnd_s *ub);
void g2u(ubnd_s *a, const gbnd_s *g);
void g2ur(ubnd_s *a, const gbnd_s *g);

void unify(ubnd_s *a, const ubnd_s *ub);
void smartunify(ubnd_s *a, const ubnd_s *ub, const mpf_s *ratio);
void guessu(unum_s *a, const ubnd_s *ub);

#if defined (__cplusplus)
}
#endif

#endif /* CONV_H_ */
