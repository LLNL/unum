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

#include "unum.h"
#include "uenv.h"
#include "ubnd.h"
#include "conv.h"
#include "support.h"
#include "hlayer.h"

/* use unum_ prefix on mlayer API (memory or machine layer) */

size_t unum_sz;
size_t ubnd_sz;

/* Independent of the current environment */

int numbits(int e, int f, const unum *op)
{
	int esize, fsize;
	unsigned char *ptr = (unsigned char *)op;
	unsigned int efbytes = (e + f + 7) / 8;
	unsigned long tmp = 0;

	while (efbytes) tmp = (tmp << 8) | ptr[--efbytes];
	fsize = (tmp & ((1U << f)-1)) + 1;
	esize = ((tmp & ((1U << e)-1) << f) >> f) + 1;
	return(2 + esize + fsize + e + f);
}

/* handles byte aligned unums */
/* TODO: use offset parameter for bit-packed unums */

unsigned int unum_load(unum_s *rop, const unum *op)
{
	unsigned int nbits = numbits(esizesize, fsizesize, op);
	unsigned int nbytes = (nbits + 7) / 8;

	mpx_import_b(rop, op, nbytes);
	return(nbytes);
}

unsigned int unum_store(unum *rop, const unum_s *op)
{
	unsigned int nbits = numbits(esizesize, fsizesize, MPX_PTR(op));
	unsigned int nbytes = (nbits + 7) / 8;

	mpx_export_b(rop, nbytes, op);
	return(nbytes);
}

unsigned int ubnd_load(ubnd_s *rop, const ubnd *op)
{
	unsigned int nbytes = 1, n;
	unsigned char *ptr = (unsigned char *)op;

	rop->p = *ptr++;
	n = unum_load(rop->l, ptr);
	nbytes += n;
	if (rop->p) {ptr += n; n = unum_load(rop->r, ptr); nbytes += n;}
	return(nbytes);
}

unsigned int ubnd_store(ubnd *rop, const ubnd_s *op)
{
	unsigned int nbytes = 1, n;
	unsigned char *ptr = (unsigned char *)rop;

	*ptr++ = op->p;
	n = unum_store(ptr, op->l);
	nbytes += n;
	if (op->p) {ptr += n; n = unum_store(ptr, op->r); nbytes += n;}
	return(nbytes);
}

void unum_init_env(void)
{
	init_uenv();
}

void unum_clear_env(void)
{
	clear_uenv();
}

void unum_set_env(int e, int f)
{
	set_uenv(e, f);
	/* TODO: calculate size from (maxubits + 7) / 8 */
	unum_sz = sizeof(mp_limb_t)*NLIMBS;
	ubnd_sz = sizeof(mp_limb_t)*NLIMBS*2+1;
}

void unum_get_env(int *e, int *f)
{
	*e = esizesize;
	*f = fsizesize;
}

/*----------------------*/
/*-------- unum --------*/
/*----------------------*/

void unum_set(unum *rop, const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	unum_store(rop, un);
}

void unum_set_si(unum *rop, signed long op)
{
	UN_VAR(un);

	unum_store(rop, si2un(un, op));
}

void unum_set_ui(unum *rop, unsigned long op)
{
	UN_VAR(un);

	unum_store(rop, ui2un(un, op));
}

void unum_set_d(unum *rop, double op)
{
	UN_VAR(un);

	unum_store(rop, d2un(un, op));
}

void unum_set_str(unum *rop, const char *str)
{
	UN_VAR(un);

	sscan_un(str, un);
	unum_store(rop, un);
}

int unum_scan(unum *rop)
{
	UN_VAR(un);

	scan_un(un);
	unum_store(rop, un);
	return(1);
}

signed long unum_get_si(const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	return un2si(un);
}

unsigned long unum_get_ui(const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	return un2ui(un);
}

double unum_get_d(const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	return un2d(un);
}

char *unum_get_str(char *str, const unum *op)
{
	UN_VAR(un);

	/* TODO: allocate if str == NULL, estimate length? */
	unum_load(un, op);
	sprint_un(str, un);
	return(str);
}

int unum_print(const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	print_un(un);
	return(1);
}

void unum_view(const unum *op)
{
	UN_VAR(un);

	unum_load(un, op);
	uview_un(un);
}

int unum_nbytes(const unum *op)
{
	return (numbits(esizesize, fsizesize, op)+7)/8;
}

/*-------- Relational Operations --------*/

int unum_lt(const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};

	unum_load(un1, op1);
	unum_load(un2, op2);
	return ltuQ(&ub1, &ub2);
}

int unum_gt(const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};

	unum_load(un1, op1);
	unum_load(un2, op2);
	return gtuQ(&ub1, &ub2);
}

int unum_neq(const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};

	unum_load(un1, op1);
	unum_load(un2, op2);
	return nequQ(&ub1, &ub2);
}

int unum_seq(const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};

	unum_load(un1, op1);
	unum_load(un2, op2);
	return nnequQ(&ub1, &ub2);
}

int unum_same(const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};

	unum_load(un1, op1);
	unum_load(un2, op2);
	return sameuQ(&ub1, &ub2);
}

/*-------- Arithmetic Operations --------*/

void unum_add(unum *rop, const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	UN_VAR(unrA); UN_VAR(unrB);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};
	ubnd_s ubr = {0, unrA, unrB};

	unum_load(un1, op1);
	unum_load(un2, op2);
	plusu(&ubr, &ub1, &ub2);
	if (ubr.p) {
		unify(&ubr, &ubr);
		if (ubr.p) mpx_set(unrA, qNaNu);
	}
	unum_store(rop, unrA);
}

void unum_sub(unum *rop, const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	UN_VAR(unrA); UN_VAR(unrB);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};
	ubnd_s ubr = {0, unrA, unrB};

	unum_load(un1, op1);
	unum_load(un2, op2);
	minusu(&ubr, &ub1, &ub2);
	if (ubr.p) {
		unify(&ubr, &ubr);
		if (ubr.p) mpx_set(unrA, qNaNu);
	}
	unum_store(rop, unrA);
}

void unum_mul(unum *rop, const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	UN_VAR(unrA); UN_VAR(unrB);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};
	ubnd_s ubr = {0, unrA, unrB};

	unum_load(un1, op1);
	unum_load(un2, op2);
	timesu(&ubr, &ub1, &ub2);
	if (ubr.p) {
		unify(&ubr, &ubr);
		if (ubr.p) mpx_set(unrA, qNaNu);
	}
	unum_store(rop, unrA);
}

void unum_div(unum *rop, const unum *op1, const unum *op2)
{
	UN_VAR(un1);
	UN_VAR(un2);
	UN_VAR(unrA); UN_VAR(unrB);
	ubnd_s ub1 = {0, un1, un1};
	ubnd_s ub2 = {0, un2, un2};
	ubnd_s ubr = {0, unrA, unrB};

	unum_load(un1, op1);
	unum_load(un2, op2);
	divideu(&ubr, &ub1, &ub2);
	if (ubr.p) {
		unify(&ubr, &ubr);
		if (ubr.p) mpx_set(unrA, qNaNu);
	}
	unum_store(rop, unrA);
}

void unum_sq(unum *rop, const unum *op)
{
	UN_VAR(unA); UN_VAR(unB);
	ubnd_s ub = {0, unA, unB};

	unum_load(unA, op);
	squareu(&ub, &ub);
	if (ub.p) {
		unify(&ub, &ub);
		if (ub.p) mpx_set(unA, qNaNu);
	}
	unum_store(rop, unA);
}

void unum_sqrt(unum *rop, const unum *op)
{
	UN_VAR(unA); UN_VAR(unB);
	ubnd_s ub = {0, unA, unB};

	unum_load(unA, op);
	sqrtu(&ub, &ub);
	if (ub.p) {
		unify(&ub, &ub);
		if (ub.p) mpx_set(unA, qNaNu);
	}
	unum_store(rop, unA);
}

void unum_neg(unum *rop, const unum *op)
{
	UN_VAR(unA); UN_VAR(unB);
	ubnd_s ub = {0, unA, unB};

	unum_load(unA, op);
	negateu(&ub, &ub);
	/* assert(ub.p == 0); */
	unum_store(rop, unA);
}

void unum_abs(unum *rop, const unum *op)
{
	UN_VAR(unA); UN_VAR(unB);
	ubnd_s ub = {0, unA, unB};

	unum_load(unA, op);
	absu(&ub, &ub);
	/* assert(ub.p == 0); */
	unum_store(rop, unA);
}

void unum_guess(unum *rop, const unum *op)
{
	UN_VAR(un1);
	UN_VAR(unr);
	ubnd_s ub1 = {0, un1, un1};

	unum_load(un1, op);
	guessu(unr, &ub1);
	unum_store(rop, unr);
}

/*----------------------*/
/*-------- ubnd --------*/
/*----------------------*/

void ubnd_set(ubnd *rop, const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	ubnd_store(rop, ub);
}

void ubnd_set_si(ubnd *rop, signed long op)
{
	UB_VAR(ub);

	ubnd_store(rop, si2ub(ub, op));
}

void ubnd_set_ui(ubnd *rop, unsigned long op)
{
	UB_VAR(ub);

	ubnd_store(rop, ui2ub(ub, op));
}

void ubnd_set_d(ubnd *rop, double op)
{
	UB_VAR(ub);

	ubnd_store(rop, d2ub(ub, op));
}

void ubnd_set_str(ubnd *rop, const char *str)
{
	UB_VAR(ub);

	sscan_ub(str, ub);
	ubnd_store(rop, ub);
}

int ubnd_scan(ubnd *rop)
{
	UB_VAR(ub);

	scan_ub(ub);
	ubnd_store(rop, ub);
	return(1);
}

signed long ubnd_get_si(const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	return ub2si(ub);
}

unsigned long ubnd_get_ui(const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	return ub2ui(ub);
}

double ubnd_get_d(const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	return ub2d(ub);
}

char *ubnd_get_str(char *str, const ubnd *op)
{
	UB_VAR(ub);

	/* TODO: allocate if str == NULL, estimate length? */
	ubnd_load(ub, op);
	sprint_ub(str, ub);
	return(str);
}

int ubnd_print(const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	print_ub(ub);
	return(1);
}

void ubnd_view(const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	uview_ub(ub);
}

int ubnd_nbytes(const ubnd *op)
{
	int nbytes = 1;

	nbytes += (numbits(esizesize, fsizesize, op+1)+7)/8;
	nbytes += (numbits(esizesize, fsizesize, op+nbytes)+7)/8;
	return nbytes;
}

/*-------- Relational Operations --------*/

int ubnd_lt(const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	return ltuQ(ub1, ub2);
}

int ubnd_gt(const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	return gtuQ(ub1, ub2);
}

int ubnd_neq(const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	return nequQ(ub1, ub2);
}

int ubnd_seq(const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	return nnequQ(ub1, ub2);
}

int ubnd_same(const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	return sameuQ(ub1, ub2);
}

/*-------- Arithmetic Operations --------*/

void ubnd_add(ubnd *rop, const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);
	UB_VAR(ubr);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	plusu(ubr, ub1, ub2);
	ubnd_store(rop, ubr);
}

void ubnd_sub(ubnd *rop, const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);
	UB_VAR(ubr);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	minusu(ubr, ub1, ub2);
	ubnd_store(rop, ubr);
}

void ubnd_mul(ubnd *rop, const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);
	UB_VAR(ubr);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	timesu(ubr, ub1, ub2);
	ubnd_store(rop, ubr);
}

void ubnd_div(ubnd *rop, const ubnd *op1, const ubnd *op2)
{
	UB_VAR(ub1);
	UB_VAR(ub2);
	UB_VAR(ubr);

	ubnd_load(ub1, op1);
	ubnd_load(ub2, op2);
	divideu(ubr, ub1, ub2);
	ubnd_store(rop, ubr);
}

void ubnd_sq(ubnd *rop, const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	squareu(ub, ub);
	ubnd_store(rop, ub);
}

void ubnd_sqrt(ubnd *rop, const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	sqrtu(ub, ub);
	ubnd_store(rop, ub);
}

void ubnd_neg(ubnd *rop, const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	negateu(ub, ub);
	ubnd_store(rop, ub);
}

void ubnd_abs(ubnd *rop, const ubnd *op)
{
	UB_VAR(ub);

	ubnd_load(ub, op);
	absu(ub, ub);
	ubnd_store(rop, ub);
}

void ubnd_guess(unum *rop, const ubnd *op)
{
	UB_VAR(ub1);
	UN_VAR(unr);

	ubnd_load(ub1, op);
	guessu(unr, ub1);
	unum_store(rop, unr);
}
