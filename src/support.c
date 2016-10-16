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

#include <stdlib.h> /* labs */

#include "support.h"
#include "uenv.h"


void utag(utag_s *ut, const unum_s *u)
{
	unsigned long u0 = mpx_get_ui(u);

	ut->fsize = (u0 & mpx_get_ui(fsizemask)) + 1;
	ut->esize = ((u0 & mpx_get_ui(esizemask)) >> fsizesize) + 1;
	ut->ubit = (u0 & mpx_get_ui(ubitmask)) >> (esizesize + fsizesize);
}

void signmask(unum_s *a, const unum_s *u)
{
	utag_s ut;

	utag(&ut, u);
	mpx_lshift(a, one, ut.esize + ut.fsize + utagsize);
}

/* Biggest unum possible with identical utag contents. */

void bigu(unum_s *a, const unum_s *u)
{
	utag_s ut;

	utag(&ut, u);
	mpx_lshift(a, one, ut.esize + ut.fsize + utagsize);
	mpx_sub(a, a, ulpu);
	if (ut.esize == esizemax && ut.fsize == fsizemax) {
		mpx_sub(a, a, ulpu);
	}
	mpx_add_ui(a, a, ((ut.esize-1) << fsizesize) | (ut.fsize-1));
}

/* Find the scale factor, with exceptions. */

long scale(const mpf_s *f)
{
	long exp;

	if (mpf_sgn(f) == 0) return 0;
	/* floor(log2(abs(f))) */
	mpf_get_d_2exp(&exp, f);
	return(exp-1);
}

/* Find best number of scale bits, accounting for subnormals. */

int ne(const mpf_s *f)
{
	long sf, tmp;
	int nb;

	if (mpf_sgn(f) == 0 || (sf = scale(f)) == 1) return 1;
	/* ceil(log2(abs(sf-1)+1))+1 */
	tmp = labs(sf-1)+1;
	/* ceil(log2(x)): 5->3, 4->2, 3->2, 2->1, 1->0 */
	for (nb = 0, tmp--; tmp; tmp >>= 1, nb++) ;
	return(nb+1);
}

/* utag independent */

int inexQ(const unum_s *u)
{
	return mpx_tstbit(u, utagsize - 1);
}

int exQ(const unum_s *u)
{
	return !mpx_tstbit(u, utagsize-1);
}

int infuQ(const unum_s *u)
{
	if (mpx_cmp(u, posinfu) == 0) return 1;
	if (mpx_cmp(u, neginfu) == 0) return -1;
	return 0;
}

int nanuQ(const unum_s *u)
{
	if (mpx_cmp(u, qNaNu) == 0) return 1;
	if (mpx_cmp(u, sNaNu) == 0) return -1;
	return 0;
}

/* Add a zero bit to the fraction length of an exact unum, if possible. */
/* The ubit is ignored and not set. */

void promotef(unum_s *a, const unum_s *u)
{
	utag_s ut;
	MPX_VAR(tmp);

	utag(&ut, u);
	if (ut.fsize < fsizemax) {
		/* float mask */
		mpx_lshift(tmp, one, 1 + ut.esize + ut.fsize);
		mpx_sub_ui(tmp, tmp, 1);
		mpx_lshift(tmp, tmp, utagsize);
		/* float <<= 1 */
		mpx_and(tmp, u, tmp);
		mpx_lshift(tmp, tmp, 1);
		/* include utag with fsize+1 */
		mpx_add_ui(a, tmp, ((ut.esize-1) << fsizesize) | ut.fsize);
	} else {
		mpx_set(a, u);
	}
}

/* Increase the length of the exponent field of an exact unum, if possible. */
/* The ubit is ignored and not set. */

void promotee(unum_s *a, const unum_s *u)
{
	int s, nsigbits;
	long e;
	utag_s ut;
	MPX_VAR(tmp);

	utag(&ut, u);

	/* If already maximum exponent size, do nothing.
	   This also handles NaN and infinity values. */
	if (ut.esize == esizemax) {
		mpx_set(a, u);
		return;
	}

	/* exponent mask */
	mpx_set_ui(tmp, (1UL << ut.esize) - 1);
	mpx_lshift(tmp, tmp, ut.fsize + utagsize);

	/* exponent */
	mpx_and(tmp, u, tmp);
	mpx_rshift(tmp, tmp, ut.fsize + utagsize);
	e = mpx_get_ui(tmp);

	/* fraction mask */
	mpx_lshift(tmp, one, ut.fsize);
	mpx_sub_ui(tmp, tmp, 1);
	mpx_lshift(tmp, tmp, utagsize);

	/* fraction */
	mpx_and(tmp, u, tmp);
	mpx_rshift(tmp, tmp, utagsize);

	/* Take care of u=0 case, ignoring the sign bit.
	   It's simply the new utag. */
	if (e == 0 && mpx_zero_p(tmp)) {
		/* set utag with esize+1 */
		mpx_set_ui(a, (ut.esize << fsizesize) | (ut.fsize-1));
		return;
	}

	/* sign */
	s = mpx_tstbit(u, ut.esize + ut.fsize + utagsize);

	/* If normal (nonzero exponent), slide sign bit left,
	   add 2^es-1 to e, increment esize. */
	if (e > 0) {
		mpx_set_ui(a, s);
		mpx_lshift(a, a, ut.esize+1);
		mpx_add_ui(a, a, e + (1UL << (ut.esize-1)));
		mpx_lshift(a, a, ut.fsize);
		mpx_ior(a, a, tmp);
		mpx_lshift(a, a, utagsize);
		mpx_add_ui(a, a, (ut.esize << fsizesize) | (ut.fsize-1));
		return;
	}

	/* floor(log2(f))+1 */
	nsigbits = mpx_sizeinbase(tmp, 2);
	/* TODO: good place for an assert(nsigbits < ?) */

	/* If room to shift, stay subnormal. */
	if (ut.fsize - nsigbits >= 1UL << (ut.esize-1)) {
		mpx_lshift(a, tmp, (1UL << (ut.esize-1)) + utagsize);
		mpx_add_ui(a, a, (ut.esize << fsizesize) | (ut.fsize-1));
		if (s) mpx_setbit(a, ut.esize+1 + ut.fsize + utagsize);
		return;
	}

	/* Subnormal becomes normal. Trickiest case. */
	/* The fraction slides left such that the
	   leftmost 1 becomes the hidden bit. */

	/* Proto difference: the prototype subtracts a residual from a higher
	   exponent, while this implementation adds the fraction without the
	   hidden bit to a lower exponent. */
	mpx_set_ui(a, s);
	mpx_lshift(a, a, ut.esize+1);
	mpx_add_ui(a, a, (1UL << (ut.esize-1)) - ut.fsize + nsigbits); /* exponent */
	mpx_lshift(a, a, ut.fsize);
	mpx_clrbit(tmp, nsigbits-1); /* hidden bit */
	mpx_lshift(tmp, tmp, ut.fsize-nsigbits+1);
	mpx_ior(a, a, tmp);
	mpx_lshift(a, a, utagsize);
	mpx_add_ui(a, a, (ut.esize << fsizesize) | (ut.fsize-1));
}

/* Promote a pair of exact unums to the same esize and fsize. */

void promote(unum_s *a, unum_s *b, const unum_s *u, const unum_s *v)
{
	utag_s ut, vt;

	utag(&ut, u);
	utag(&vt, v);
	mpx_set(a, u);
	mpx_set(b, v);
	while (ut.esize < vt.esize) {promotee(a, a); ut.esize++;}
	while (vt.esize < ut.esize) {promotee(b, b); vt.esize++;}
	while (ut.fsize < vt.fsize) {promotef(a, a); ut.fsize++;}
	while (vt.fsize < ut.fsize) {promotef(b, b); vt.fsize++;}
}

/* Demote the fraction of a unum if possible, even if it makes it inexact. */
/* The ubit is maintained or introduced */

void demotef(unum_s *a, const unum_s *u)
{
	utag_s ut;
	MPX_VAR(tmp);

	utag(&ut, u);
	/* Cannot make the fraction any smaller. */
	if (ut.fsize == 1 || infuQ(u) || nanuQ(u)) {
		mpx_set(a, u);
	} else {
		/* shift fraction right one bit. */
		/* float mask */
		mpx_lshift(tmp, one, 1 + ut.esize + ut.fsize);
		mpx_sub_ui(tmp, tmp, 1);
		mpx_lshift(tmp, tmp, utagsize);
		/* float >>= 1 */
		mpx_and(tmp, u, tmp);
		mpx_rshift(a, tmp, 1);
		/* include utag with fsize-1 */
		mpx_set_ui(tmp, (ut.ubit << (utagsize-1)) | ((ut.esize-1) << fsizesize) | (ut.fsize-2));
		mpx_ior(a, a, tmp);
	}
}

/* Demote exponent length of a unum, even if it makes it inexact. */
/* The ubit is maintained or introduced */

void demotee(unum_s *a, const unum_s *u)
{
	int sign, spos, left2;
	long expo;
	utag_s ut;
	MPX_VAR(tmp);
	MPX_VAR(frac);
	mpf_t f, fi;

	utag(&ut, u);
	/* Cannot make the exponent any smaller. */
	if (ut.esize == 1 || infuQ(u) || nanuQ(u)) {
		mpx_set(a, u);
		return;
	}

	/* exponent mask */
	mpx_set_ui(tmp, (1UL << ut.esize) - 1);
	mpx_lshift(tmp, tmp, ut.fsize + utagsize);

	/* exponent */
	mpx_and(tmp, u, tmp);
	mpx_rshift(tmp, tmp, ut.fsize + utagsize);
	expo = mpx_get_ui(tmp);

	/* sign */
	spos = ut.esize + ut.fsize + utagsize;
	sign = mpx_tstbit(u, spos);

	/* fraction mask */
	mpx_lshift(tmp, one, ut.fsize);
	mpx_sub_ui(tmp, tmp, 1);
	mpx_lshift(tmp, tmp, utagsize);

	/* fraction */
	mpx_and(tmp, u, tmp);
	mpx_rshift(frac, tmp, utagsize);

	mpf_init2(f, ut.fsize);
	mpf_init2(fi, ut.fsize);

	/* Subnormal, so decreasing expo size means
	   shifting fraction right by 2^2^es-2 bits. */
	if (expo == 0) {
		mpf_set_x(f, frac);
		mpf_div_2exp(f, f, 1UL << (ut.esize-2));
		mpf_trunc(fi, f);
		mpx_set_f(a, fi);
		mpx_lshift(a, a, utagsize);
		mpx_add_ui(a, a, (ut.ubit << (utagsize-1)) | ((ut.esize-2) << fsizesize) | (ut.fsize-1));
		if (sign) mpx_setbit(a, spos-1);
		if (mpf_cmp(fi, f) != 0) mpx_setbit(a, utagsize-1);
		mpf_clear(fi);
		mpf_clear(f);
		return;
	}

	left2 = (expo & (3 << (ut.esize-2))) >> (ut.esize-2);

	/* If the left two exponent bits are 00
	   (but it's normal, since we fell through the previous test),
	   result switches to subnormal. The exponent after the first
	   two bits joins the fraction like a fixed-point number,
	   before shifting the fraction to the right. The
	   new exponent is zero, of course. */
	if (left2 == 0) {
		mpx_setbit(frac, ut.fsize); /* set hidden bit */
		mpf_set_x(f, frac);
		mpf_div_2exp(f, f, (1UL << (ut.esize-2))-expo+1);
		mpf_trunc(fi, f);
		mpx_set_f(a, fi);
		mpx_lshift(a, a, utagsize);
		mpx_add_ui(a, a, (ut.ubit << (utagsize-1)) | ((ut.esize-2) << fsizesize) | (ut.fsize-1));
		if (sign) mpx_setbit(a, spos-1);
		if (mpf_cmp(fi, f) != 0) mpx_setbit(a, utagsize-1);
		mpf_clear(fi);
		mpf_clear(f);
		return;
	}

	/* If the left two exponent bits are 01 or 10,
	   squeeze out the second bit; if that leaves a subnormal exponent,
	   shift the hidden bit and fraction bits right. */
	if (left2 <= 2) {
		int e = (expo & ((1 << (ut.esize-2))-1)) | ((left2 >> 1) << (ut.esize-2));
		if (e == 0) mpx_setbit(frac, ut.fsize); /* set hidden bit */
		mpf_set_x(f, frac);
		if (e == 0) mpf_div_2exp(f, f, 1);
		mpx_set_ui(a, e);
		mpx_lshift(a, a, ut.fsize);
		mpf_trunc(fi, f);
		mpx_set_f(tmp, fi);
		mpx_ior(a, a, tmp);
		mpx_lshift(a, a, utagsize);
		mpx_add_ui(a, a, (ut.ubit << (utagsize-1)) | ((ut.esize-2) << fsizesize) | (ut.fsize-1));
		if (sign) mpx_setbit(a, spos-1);
		if (mpf_cmp(fi, f) != 0) mpx_setbit(a, utagsize-1);
		mpf_clear(fi);
		mpf_clear(f);
		return;
	}

	/* If the first two exponent bits are 11,
	   always get an unbounded unum, all 1s for fraction. */
	mpx_lshift(a, one, ut.esize + ut.fsize);
	mpx_sub_ui(a, a, 1);
	mpx_lshift(a, a, utagsize-1);
	mpx_add_ui(a, a, ((ut.esize-2) << fsizesize) | (ut.fsize-1));
	if (sign) mpx_setbit(a, spos-1);

	mpf_clear(fi);
	mpf_clear(f);
}
