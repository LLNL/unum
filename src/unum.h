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

#ifndef UNUM_H_
#define UNUM_H_

#include <stdlib.h>

#define UNUM_ALLOC(un) (un = malloc(unum_sz))
#define UNUM_FREE(un) free(un)
#define UNUM_VAR(un) char un[unum_sz]

#define UBND_ALLOC(ub) (ub = malloc(ubnd_sz))
#define UBND_FREE(ub) free(ub)
#define UBND_VAR(ub) char ub[ubnd_sz]

/* pg 4
Definition: A #unum# is a bit string of variable length that has six 
sub-fields: Sign bit, exponent, fraction, uncertainty bit, exponent 
size, and fraction size. 
*/
typedef void unum;

/* pg 62
Definition: A #ubound# is a single unum or a pair of unums that 
represent a mathematical interval of the real line. Closed endpoints are 
represented by exact unums, and open endpoints are represented by 
inexact unums. 
*/
/* If the first byte is one, then a pair of unums follow */
typedef void ubnd;

extern size_t unum_sz;
extern size_t ubnd_sz;

#if defined(__cplusplus)
extern "C" {
#endif

void unum_init_env(void);
void unum_clear_env(void);
void unum_set_env(int e, int f);
void unum_get_env(int *e, int *f);

/*----------------------*/
/*-------- unum --------*/
/*----------------------*/

void unum_set(unum *rop, const unum *op);
void unum_set_si(unum *rop, signed long op);
void unum_set_ui(unum *rop, unsigned long op);
void unum_set_d(unum *rop, double op);
void unum_set_str(unum *rop, const char *str);
int unum_scan(unum *rop);

signed long unum_get_si(const unum *op);
unsigned long unum_get_ui(const unum *op);
double unum_get_d(const unum *op);
char *unum_get_str(char *str, const unum *op);
int unum_print(const unum *op);
void unum_view(const unum *op);

int unum_nbytes(const unum *op);

int unum_cmp(const unum *op1, const unum *op2);
int unum_lt(const unum *op1, const unum *op2);   /* less than */
int unum_gt(const unum *op1, const unum *op2);   /* greater than */
int unum_neq(const unum *op1, const unum *op2);  /* nowhere equal, disjoint */
int unum_seq(const unum *op1, const unum *op2);  /* somewhere equal, overlap */
int unum_same(const unum *op1, const unum *op2); /* identical */

void unum_add(unum *rop, const unum *op1, const unum *op2);
void unum_sub(unum *rop, const unum *op1, const unum *op2);
void unum_mul(unum *rop, const unum *op1, const unum *op2);
void unum_div(unum *rop, const unum *op1, const unum *op2);

void unum_pow(unum *rop, const unum *op1, const unum *op2);
void unum_sq(unum *rop, const unum *op);
void unum_sqrt(unum *rop, const unum *op);
void unum_neg(unum *rop, const unum *op);
void unum_abs(unum *rop, const unum *op);
void unum_exp(unum *rop, const unum *op);
void unum_log(unum *rop, const unum *op);
void unum_cos(unum *rop, const unum *op);
void unum_sin(unum *rop, const unum *op);
void unum_tan(unum *rop, const unum *op);
void unum_cot(unum *rop, const unum *op);

void unum_guess(unum *rop, const unum *op);

/*----------------------*/
/*-------- ubnd --------*/
/*----------------------*/

void ubnd_set(ubnd *rop, const ubnd *op);
void ubnd_set_si(ubnd *rop, signed long op);
void ubnd_set_ui(ubnd *rop, unsigned long op);
void ubnd_set_d(ubnd *rop, double op);
void ubnd_set_str(ubnd *rop, const char *str);
int ubnd_scan(ubnd *rop);

signed long ubnd_get_si(const ubnd *op);
unsigned long ubnd_get_ui(const ubnd *op);
double ubnd_get_d(const ubnd *op);
char *ubnd_get_str(char *str, const ubnd *op);
int ubnd_print(const ubnd *op);
void ubnd_view(const ubnd *op);

int ubnd_nbytes(const ubnd *op);

int ubnd_cmp(const ubnd *op1, const ubnd *op2);
int ubnd_lt(const ubnd *op1, const ubnd *op2);   /* less than */
int ubnd_gt(const ubnd *op1, const ubnd *op2);   /* greater than */
int ubnd_neq(const ubnd *op1, const ubnd *op2);  /* nowhere equal, disjoint */
int ubnd_seq(const ubnd *op1, const ubnd *op2);  /* somewhere equal, overlap */
int ubnd_same(const ubnd *op1, const ubnd *op2); /* identical */

void ubnd_add(ubnd *rop, const ubnd *op1, const ubnd *op2);
void ubnd_sub(ubnd *rop, const ubnd *op1, const ubnd *op2);
void ubnd_mul(ubnd *rop, const ubnd *op1, const ubnd *op2);
void ubnd_div(ubnd *rop, const ubnd *op1, const ubnd *op2);

void ubnd_pow(ubnd *rop, const ubnd *op1, const ubnd *op2);
void ubnd_sq(ubnd *rop, const ubnd *op);
void ubnd_sqrt(ubnd *rop, const ubnd *op);
void ubnd_neg(ubnd *rop, const ubnd *op);
void ubnd_abs(ubnd *rop, const ubnd *op);
void ubnd_exp(ubnd *rop, const ubnd *op);
void ubnd_log(ubnd *rop, const ubnd *op);
void ubnd_cos(ubnd *rop, const ubnd *op);
void ubnd_sin(ubnd *rop, const ubnd *op);
void ubnd_tan(ubnd *rop, const ubnd *op);
void ubnd_cot(ubnd *rop, const ubnd *op);

void ubnd_guess(unum *rop, const ubnd *op);

int ubnd_spans_zero(const ubnd *op);
void ubnd_intersect(ubnd *rop, const ubnd *op1, const ubnd *op2);

#if defined (__cplusplus)
}
#endif

#endif /* UNUM_H_ */
