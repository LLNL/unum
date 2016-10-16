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

#include <limits.h> /* LONG_MIN, LONG_MAX, ULONG_MAX */
#include <float.h> /* DBL_MAX */
#include <math.h> /* isnan, isinf, NAN, INFINITY */

#include "conv.h"
#include "uenv.h"
#include "support.h"
#include "gbnd.h"
#include "hlayer.h"

#if defined(ROUND)
#define F2U(un,f) do {f2u(un,f); roundu(un);} while (0)
#else
#define F2U(un,f) f2u(un,f)
#endif


/* Returns +1 or -1 for infinity or the midpoint of interval g in a */

int midpoint(mpf_s *a, const gbnd_s *g)
{
	if (!(g->l.inf || g->r.inf)) { /* Both ends finite */
		if (mpf_cmp(g->l.f, g->r.f)) {
			mpf_add(a, g->l.f, g->r.f); /* Calc midpoint of [x, y] */
			mpf_div_2exp(a, a, 1);
			return 0;
		}
		mpf_set(a, g->l.f); /* Exact x */
		return 0;
	}
	if (g->l.inf && g->r.inf) { /* Both ends infinite */
		if (mpf_cmp(g->l.f, g->r.f)) {
			mpf_set_ui(a, 0); /* Zero midpoint of [-Inf,+Inf] */
			return 0;
		}
		return mpf_sgn(g->l.f); /* Exact Inf */
	}
	/* One end infinite */
	if (g->l.inf) {
		return mpf_sgn(g->l.f); /* -Inf to x */
	}
	return mpf_sgn(g->r.f); /* x to +Inf */
}

void roundu(unum_s *un)
{
	/* If exact, we're done. */
	if (exQ(un)) return;
	/* TODO: implement other rounding modes */
	/* else round to nearest even. */
	if (mpx_tstbit(un, utagsize)) mpx_add(un, un, ubitmask);
	else mpx_sub(un, un, ubitmask);
	/* Compress away the last zero in the fraction, since it is even. */
	demotef(un, un);
}

/*-------- gbnd conversion --------*/

gbnd_s *si2g(gbnd_s *g, signed long si)
{
	g->nan = 0;
	mpf_set_si(g->l.f, si); g->l.inf = 0; g->l.open = 0;
	mpf_set_si(g->r.f, si); g->r.inf = 0; g->r.open = 0;
	return g;
}

gbnd_s *ui2g(gbnd_s *g, unsigned long ui)
{
	g->nan = 0;
	mpf_set_ui(g->l.f, ui); g->l.inf = 0; g->l.open = 0;
	mpf_set_ui(g->r.f, ui); g->r.inf = 0; g->r.open = 0;
	return g;
}

gbnd_s *d2g(gbnd_s *g, double d)
{
	if (isnan(d)) {
		g->nan = 1;
		mpf_set_si(g->l.f, 0); g->l.inf = 0; g->l.open = 1;
		mpf_set_si(g->r.f, 0); g->r.inf = 0; g->r.open = 1;
		return g;
	}
	g->nan = 0;
	if (isinf(d)) {
		int inf = (d < 0.0) ? -1 : 1;
		mpf_set_si(g->l.f, inf); g->l.inf = 1; g->l.open = 0;
		mpf_set_si(g->r.f, inf); g->r.inf = 1; g->r.open = 0;
		return g;
	}
	mpf_set_d(g->l.f, d); g->l.inf = 0; g->l.open = 0;
	mpf_set_d(g->r.f, d); g->r.inf = 0; g->r.open = 0;
	return g;
}

gbnd_s *f2g(gbnd_s *g, const mpf_s *f)
{
	g->nan = 0;
	mpf_set(g->l.f, f); g->l.inf = 0; g->l.open = 0;
	mpf_set(g->r.f, f); g->r.inf = 0; g->r.open = 0;
	return g;
}

#if 0
gbnd_s *s2g(gbnd_s *g, const char *s, int base)
{
	/* TODO: parse string for NaN, Inf, (), []? */
	g->nan = 0;
	mpf_set_str(g->l.f, s, base); g->l.inf = 0; g->l.open = 0;
	mpf_set_str(g->r.f, s, base); g->r.inf = 0; g->r.open = 0;
	return g;
}
#endif

signed long g2si(const gbnd_s *g)
{
	int inf;
	signed long rop;
	mpf_t tmpf;

	if (g->nan) return 0;
	mpf_init2(tmpf, UPREC);
	inf = midpoint(tmpf, g);
	/* TODO: round? */
	if (inf < 0 || mpf_cmp_si(tmpf, LONG_MIN) < 0) rop = LONG_MIN;
	else if (inf > 0 || mpf_cmp_si(tmpf, LONG_MAX) > 0) rop = LONG_MAX;
	else rop = mpf_get_si(tmpf);
	mpf_clear(tmpf);
	return(rop);
}

unsigned long g2ui(const gbnd_s *g)
{
	int inf;
	unsigned long rop;
	mpf_t tmpf;

	if (g->nan) return 0;
	mpf_init2(tmpf, UPREC);
	inf = midpoint(tmpf, g);
	/* TODO: round? */
	if (inf < 0 || mpf_sgn(tmpf) < 0) rop = 0;
	else if (inf > 0 || mpf_cmp_ui(tmpf, ULONG_MAX) > 0) rop = ULONG_MAX;
	else rop = mpf_get_ui(tmpf);
	mpf_clear(tmpf);
	return(rop);
}

double g2d(const gbnd_s *g)
{
	int inf;
	double rop;
	mpf_t tmpf;

	if (g->nan) return NAN;
	mpf_init2(tmpf, UPREC);
	inf = midpoint(tmpf, g);
	/* TODO: round? */
	if (inf < 0) rop = -INFINITY;
	else if (inf > 0) rop = INFINITY;
	else rop = mpf_get_d(tmpf);
	mpf_clear(tmpf);
	return(rop);
}

mpf_s *g2f(mpf_s *f, const gbnd_s *g)
{
	int inf;
	mpf_t negmaxreal;

	if (g->nan) {mpf_set_ui(f, 0); return f;}
	mpf_init2(negmaxreal, UPREC);
	inf = midpoint(f, g);
	/* TODO: round? */
	mpf_neg(negmaxreal, maxreal);
	if (inf < 0 || mpf_cmp(f, negmaxreal) < 0) {
		mpf_set(f, negmaxreal);
	} else if (inf > 0 || mpf_cmp(f, maxreal) > 0) {
		mpf_set(f, maxreal);
	}
	mpf_clear(negmaxreal);
	return(f);
}

/*-------- unum conversion --------*/

unum_s *si2un(unum_s *un, signed long si)
{
	mpf_t tmpf;

	mpf_init2(tmpf, sizeof(si)*8);
	mpf_set_si(tmpf, si);
	F2U(un, tmpf);
	mpf_clear(tmpf);
	return un;
}

unum_s *ui2un(unum_s *un, unsigned long ui)
{
	mpf_t tmpf;

	mpf_init2(tmpf, sizeof(ui)*8);
	mpf_set_ui(tmpf, ui);
	F2U(un, tmpf);
	mpf_clear(tmpf);
	return un;
}

unum_s *d2un(unum_s *un, double d)
{
	mpf_t tmpf;

	if (isnan(d)) {
		mpx_set(un, qNaNu); /* TODO: handle sNaN? */
		return un;
	}
	if (isinf(d)) {
		mpx_set(un, (d < 0.0) ? neginfu : posinfu);
		return un;
	}
	mpf_init2(tmpf, 53);
	mpf_set_d(tmpf, d);
	F2U(un, tmpf);
	mpf_clear(tmpf);
	return un;
}

signed long un2si(const unum_s *un)
{
	signed long rop;
	gbnd_t gb;

	gbnd_init(gb);
	unum2g(gb, un);
	rop = g2si(gb);
	gbnd_clear(gb);
	return(rop);
}

unsigned long un2ui(const unum_s *un)
{
	unsigned long rop;
	gbnd_t gb;

	gbnd_init(gb);
	unum2g(gb, un);
	rop = g2ui(gb);
	gbnd_clear(gb);
	return(rop);
}

double un2d(const unum_s *un)
{
	double rop;
	gbnd_t gb;

	gbnd_init(gb);
	unum2g(gb, un);
	rop = g2d(gb);
	gbnd_clear(gb);
	return(rop);
}

/*-------- ubnd conversion --------*/

ubnd_s *si2ub(ubnd_s *ub, signed long si)
{
	mpf_t tmpf;

	mpf_init2(tmpf, sizeof(si)*8);
	mpf_set_si(tmpf, si);
	F2U(ub->l, tmpf); ub->p = 0;
	mpf_clear(tmpf);
	return ub;
}

ubnd_s *ui2ub(ubnd_s *ub, unsigned long ui)
{
	mpf_t tmpf;

	mpf_init2(tmpf, sizeof(ui)*8);
	mpf_set_ui(tmpf, ui);
	F2U(ub->l, tmpf); ub->p = 0;
	mpf_clear(tmpf);
	return ub;
}

ubnd_s *d2ub(ubnd_s *ub, double d)
{
	mpf_t tmpf;

	if (isnan(d)) {
		ub->p = 0;
		mpx_set(ub->l, qNaNu); /* TODO: handle sNaN? */
		return ub;
	}
	if (isinf(d)) {
		ub->p = 0;
		mpx_set(ub->l, (d < 0.0) ? neginfu : posinfu);
		return ub;
	}
	mpf_init2(tmpf, 53);
	mpf_set_d(tmpf, d);
	F2U(ub->l, tmpf); ub->p = 0;
	mpf_clear(tmpf);
	return ub;
}

signed long ub2si(const ubnd_s *ub)
{
	signed long rop;
	gbnd_t gb;

	gbnd_init(gb);
	u2g(gb, ub);
	rop = g2si(gb);
	gbnd_clear(gb);
	return(rop);
}

unsigned long ub2ui(const ubnd_s *ub)
{
	unsigned long rop;
	gbnd_t gb;

	gbnd_init(gb);
	u2g(gb, ub);
	rop = g2ui(gb);
	gbnd_clear(gb);
	return(rop);
}

double ub2d(const ubnd_s *ub)
{
	double rop;
	gbnd_t gb;

	gbnd_init(gb);
	u2g(gb, ub);
	rop = g2d(gb);
	gbnd_clear(gb);
	return(rop);
}

/*-------- other conversion --------*/

/* Convert an exact unum to its [internal] float value. */
/* Ignores ubit */
/* Proto difference: does not handle infinity. */

void u2f(mpf_s *f, const unum_s *u)
{
	long bias, expo, expovalue;
	int sign;
	utag_s ut;
	MPX_VAR(tmp);

	/* ubit, exponent size, fraction size */
	utag(&ut, u);

	/* exponent bias */
	bias = (1UL << (ut.esize-1)) - 1;

	/* exponent mask */
	mpx_set_ui(tmp, (1UL << ut.esize) - 1);
	mpx_lshift(tmp, tmp, ut.fsize + utagsize);

	/* exponent */
	mpx_and(tmp, u, tmp);
	mpx_rshift(tmp, tmp, ut.fsize + utagsize);
	expo = mpx_get_ui(tmp);

	expovalue = expo - bias + ((expo) ? 0 : 1);

	/* sign */
	sign = mpx_tstbit(u, ut.esize + ut.fsize + utagsize);

	/* fraction mask */
	mpx_lshift(tmp, one, ut.fsize);
	mpx_sub_ui(tmp, tmp, 1);
	mpx_lshift(tmp, tmp, utagsize);

	/* fraction */
	mpx_and(tmp, u, tmp);
	mpx_rshift(tmp, tmp, utagsize);

	mpf_set_x(f, tmp);
	mpf_div_2exp(f, f, ut.fsize);
	if (expo) mpf_add_ui(f, f, 1);
	if (expovalue >= 0) mpf_mul_2exp(f, f, expovalue);
	else mpf_div_2exp(f, f, -expovalue);

	if (sign) mpf_neg(f, f);
}

/* Conversion of an [internal] floatable real to a unum. */
/* Most of the complexity stems from seeking the shortest possible bit string. */
/* Proto difference: does not handle exceptional nonnumeric values. */
/* See x2u in prototype */

void f2u(unum_s *u, const mpf_s *f)
{
	int fs;
	long sf;
	mpf_t absf;
	mpf_t tmp1;
	mpf_t tmp2;

	/* Zero is a special case. The smallest unum for it is just 0: */
	if (mpf_sgn(f) == 0) {
		mpx_set_ui(u, 0);
		return;
	}
	mpf_init2(absf, mpf_get_prec(f));
	mpf_abs(absf, f);
	/* Magnitudes too large to represent: */
	if (mpf_cmp(absf, maxreal) > 0) {
		mpx_ior(u, maxrealu, ubitmask);
		if (mpf_sgn(f) < 0) mpx_ior(u, u, signbigu);
		mpf_clear(absf);
		return;
	}
	/* Magnitudes too small to represent become "inexact zero"
	   with the maximum exponent and fraction field sizes: */
	if (mpf_cmp(absf, smallsubnormal) < 0) {
		mpx_set(u, utagmask);
		if (mpf_sgn(f) < 0) mpx_ior(u, u, signbigu);
		mpf_clear(absf);
		return;
	}
	/* For subnormal numbers, divide by the ULP value to get the
	   fractional part. The While loop strips off trailing bits. */
	mpf_init2(tmp1, UPREC);
	mpf_init2(tmp2, UPREC);
	u2f(tmp1, smallnormalu);
	if (mpf_cmp(absf, tmp1) < 0) {
		unsigned long efbits = mpx_get_ui(efsizemask);
		unsigned long spos = maxubits-1;
		int ubit;
		mpf_div(tmp1, absf, smallsubnormal);
		mpf_trunc(tmp2, tmp1);
		mpx_set_f(u, tmp2); /* fractional part */
		ubit = mpf_cmp(tmp1, tmp2) != 0;
		if (!ubit) {
			while (!mpx_tstbit(u, 0)) {
				mpx_rshift(u, u, 1); /* strip trailing zero bits */
				efbits--;
				spos--;
			}
		}
		mpx_lshift(u, u, utagsize);
		mpx_add_ui(u, u, efbits);
		if (ubit) mpx_ior(u, u, ubitmask);
		if (mpf_sgn(f) < 0) mpx_setbit(u, spos);
		mpf_clear(tmp2);
		mpf_clear(tmp1);
		mpf_clear(absf);
		return;
	}
	/* All remaining cases are in the normalized range. */
	fs = 0;
	if ((sf = scale(f)) >= 0) mpf_div_2exp(tmp1, absf, sf);
	else mpf_mul_2exp(tmp1, absf, -sf);
	/* If a number is more concise as a subnormal, make it one. */
	if (mpf_cmp_ui(tmp1, 1) == 0 && sf <= 0) {
		int es; /* exponent size */
		int pc; /* population count */
		long tmp;
		for (es = pc = 0, tmp = 1-sf; tmp; tmp >>= 1, es++) if (tmp & 1) pc++;
		if (pc == 1) {
			mpx_set_ui(u, es-1);
			mpx_lshift(u, u, fsizesize);
			mpx_ior(u, u, ulpu);
			if (mpf_sgn(f) < 0) mpx_setbit(u, es+1+utagsize);
			mpf_clear(tmp2);
			mpf_clear(tmp1);
			mpf_clear(absf);
			return;
		}
	}
	mpf_trunc(tmp2, tmp1);
	while (mpf_cmp(tmp1, tmp2) != 0 && fs < fsizemax) { /* TODO: optimize */
		fs++;
		mpf_mul_2exp(tmp1, tmp1, 1);
		mpf_trunc(tmp2, tmp1);
	}
	if (mpf_cmp(tmp1, tmp2) == 0) {
		int nef = ne(f);
		MPX_VAR(tmpu);
		/* The value is representable exactly. Fill in fields from right to left: */
		/* Size of fraction field, fits in the rightmost fsizesize bits. */
		mpx_set_ui(u, fs - ((fs > 0) ? 1 : 0));
		/* Size of exponent field minus 1, fits in the esizesize bits. */
		mpx_set_ui(tmpu, ne(f) - 1);
		mpx_lshift(tmpu, tmpu, fsizesize);
		mpx_ior(u, u, tmpu);
		/* Significant bits after hidden bit, fits left of the unum tag bits. */
		if (fs > 0) {
			/* follows prototype, TODO: make more efficient */
			long s1 = scale(tmp1);
			mpf_set_ui(tmp1, 1);
			mpf_mul_2exp(tmp1, tmp1, s1); /* 2^scale(tmp1) */
			mpf_sub(tmp2, tmp2, tmp1);
			mpx_set_f(tmpu, tmp2);
			mpx_lshift(tmpu, tmpu, utagsize);
			mpx_ior(u, u, tmpu);
		}
		/* Value of exponent bits, adjusted for bias. */
		mpx_set_ui(tmpu, sf + (1 << (nef-1)) - 1);
		mpx_lshift(tmpu, tmpu, utagsize + fs + ((fs == 0) ? 1 : 0));
		mpx_ior(u, u, tmpu);
		/* If negative, add the sign bit. */
		if (mpf_sgn(f) < 0) {
			mpx_set_ui(tmpu, 1);
			mpx_lshift(tmpu, tmpu, utagsize + fs + ((fs == 0) ? 1 : 0) + nef);
			mpx_ior(u, u, tmpu);
		}
	} else {
		long s1, sff;
		int nef, ne1, ne2;
		MPX_VAR(tmpu);
		/* Inexact. Use all available fraction bits. */
		if ((sff = sf-fsizemax) >= 0) mpf_div_2exp(tmp1, absf, sff);
		else mpf_mul_2exp(tmp1, absf, -sff);
		mpf_ceil(tmp1, tmp1);
		if (sff >= 0) mpf_mul_2exp(tmp1, tmp1, sff);
		else mpf_div_2exp(tmp1, tmp1, -sff);
		nef = ne(f);
		ne1 = ne(tmp1);
		ne2 = (nef > ne1) ? nef : ne1;
		/* All bits on for the fraction size, since we're using the maximum. */
		mpx_set(u, fsizemask);
		/* Store the exponent size minus 1 in the exponent size field. */
		mpx_set_ui(tmpu, ne2 - 1);
		mpx_lshift(tmpu, tmpu, fsizesize);
		mpx_ior(u, u, tmpu);
		/* Make it inexact. */
		mpx_ior(u, u, ubitmask);
		/* Fraction bits are the ones to the left of the binary
		   point after removing hidden bit and scaling. */
		if ((s1 = scale(tmp1)) >= 0) mpf_div_2exp(tmp2, tmp1, s1);
		else mpf_mul_2exp(tmp2, tmp1, -s1);
		mpf_sub_ui(tmp2, tmp2, 1); /* remove hidden bit */
		mpf_mul_2exp(tmp2, tmp2, fsizemax);
		mpf_trunc(tmp2, tmp2);
		mpx_set_f(tmpu, tmp2);
		mpx_lshift(tmpu, tmpu, utagsize);
		mpx_ior(u, u, tmpu);
		/* Exponent value goes in the exponent field. */
		mpx_set_ui(tmpu, s1 + (1 << (ne2-1)) - 1);
		mpx_lshift(tmpu, tmpu, utagsize + fsizemax);
		mpx_ior(u, u, tmpu);
		/* Back off by one ULP. */
		/* If frac is zero, this will borrow from exponent. */
		mpx_sub(u, u, ulpu);
		/* If negative, set the sign bit in the unum. */
		if (mpf_sgn(f) < 0) {
			mpx_set_ui(tmpu, 1);
			mpx_lshift(tmpu, tmpu, utagsize + fsizemax + ne2);
			mpx_ior(u, u, tmpu);
		}
	}
	mpf_clear(tmp2);
	mpf_clear(tmp1);
	mpf_clear(absf);
	return;
}

/* Conversion of a unum to a general interval. */
/* Proto difference: the test for infinity is outside of u2f() */

void unum2g(gbnd_s *a, const unum_s *u)
{
	mp_bitcnt_t signpos;
	MPX_VAR(tmp);

//printf("unum2g: "); uview_un(u);
	if (nanuQ(u)) { /* NaN */
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		//AOP1("NaN",a,gb,unum2g,u,un); // causes recursion on print
//printf(" NaN: "); print_gb(a); putchar('\n');
	} else {
		int inf = infuQ(u);
		if (inf) { /* infinite */
			a->nan = 0;
			mpf_set_si(a->l.f, inf); mpf_set_si(a->r.f, inf);
			a->l.inf = a->r.inf = 1;
			a->l.open = a->r.open = 0;
//printf(" Inf: "); print_gb(a); putchar('\n');
			return;
		}
		if (exQ(u)) { /* exact */
			a->nan = 0;
			u2f(a->l.f, u); mpf_set(a->r.f, a->l.f);
			a->l.inf = a->r.inf = 0;
			a->l.open = a->r.open = 0;
//printf(" exact: "); print_gb(a); putchar('\n');
			return;
		}
		/* open */
		a->nan = 0;
		a->l.open = a->r.open = 1;
		bigu(tmp, u);
		mpx_ior(tmp, tmp, ubitmask);
		if (mpx_cmp(u, tmp) == 0) { /* (bigu, Inf) */
			u2f(a->l.f, u); mpf_set_si(a->r.f, 1);
			a->l.inf = 0; a->r.inf = 1;
//printf(" (bigu, Inf): "); print_gb(a); putchar('\n');
			return;
		}
		{
			utag_s ut;
			utag(&ut, u);
			signpos = ut.esize + ut.fsize + utagsize;
		}
		mpx_setbit(tmp, signpos);
		if (mpx_cmp(u, tmp) == 0) { /* (-Inf, -bigu) */
			mpf_set_si(a->l.f, -1); u2f(a->r.f, u);
			a->l.inf = 1; a->r.inf = 0;
//printf(" (-Inf, -bigu): "); print_gb(a); putchar('\n');
			return;
		}
		mpx_add(tmp, u, ulpu);
		a->l.inf = a->r.inf = 0;
		/* If negative, left endpoint is farther from zero. */
		if (mpx_tstbit(u, signpos)) {
			u2f(a->l.f, tmp); u2f(a->r.f, u); /* (-(x+ulp), -x) */
//printf(" (-(x+ulp), -x): "); print_gb(a); putchar('\n');
		} else {
			u2f(a->l.f, u); u2f(a->r.f, tmp); /* (x, x+ulp) */
//printf(" (x, x+ulp): "); print_gb(a); putchar('\n');
		}
	}
}

/* Conversion of a ubound to a general interval. */

void ubnd2g(gbnd_s *a, const ubnd_s *ub)
{
	gbnd_s gL, gR;

	if (nanuQ(ub->l) || nanuQ(ub->r)) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		//AOP1("NaN",a,gb,ubnd2g,ub,ub);
		return;
	}

	gbnd_init(&gL);
	gbnd_init(&gR);
	a->nan = 0;

	unum2g(&gL, ub->l);
	mpf_set(a->l.f, gL.l.f);
	a->l.inf  = gL.l.inf;
	a->l.open = gL.l.open;

	unum2g(&gR, ub->r);
	mpf_set(a->r.f, gR.r.f);
	a->r.inf  = gR.r.inf;
	a->r.open = gR.r.open;

	gbnd_clear(&gL);
	gbnd_clear(&gR);
}

/* Conversion of a unum or ubound to a general interval. */

void u2g(gbnd_s *a, const ubnd_s *ub)
{
//printf(" u2g: "); uview_ub(ub); putchar('\n');}
	if (!ub->p) unum2g(a, ub->l);
	else ubnd2g(a, ub);
//printf("u2g(a): "); print_gb(a); putchar('\n');
}

/* Seek a single-ULP enclosure for a ubound >= zero. */
/* unifypos is only called by unify which screens for NaN & Inf */

void unifypos(ubnd_s *a, const ubnd_s *ub)
{
	utag_s ut;
	unum_s *u;
	unum_s *v;
	MPX_VAR(uu);
	MPX_VAR(uv);
	MPX_VAR(uw);
	MPX_VAR(ux);
	gbnd_s gu, gv, gn, gb;

// printf(" unifypos:"); print_ub(ub); putchar('\n');
	u = ub->l;
	v = (ub->p) ? ub->r : ub->l;

#if 0
	/* Needed for book version of unify. */
	/* Recursion breaker. Avoid call to g2u below. */
	if (!ub->p || mpx_cmp(ub->l, ub->r) == 0) {
		a->p = 0;
		mpx_set(a->l, ub->l);
		mpx_set(a->r, ub->l);
		return;
	}
#endif

	gbnd_init(&gu);
	gbnd_init(&gv);

// printf(" trivial:"); putchar('\n');
	/* Trivial case where endpoints express the same value. */
	/* TODO: make more efficient */
	unum2g(&gu, u);
	unum2g(&gv, v);
	if (samegQ(&gu, &gv)) {
		/* NOTE: call to g2u has recursion potential */
		g2u(a, &gu);
		gbnd_clear(&gv);
		gbnd_clear(&gu);
		return;
	}

// printf(" low:"); putchar('\n');
	/* Cannot unify if the interval includes exact 0, 1, 2, or 3. */
	gbnd_init(&gn);
	gbnd_init(&gb);
	u2g(&gb, ub);
	if (nneqgQ(&gb,ui2g(&gn,0)) || nneqgQ(&gb,ui2g(&gn,1)) ||
		nneqgQ(&gb,ui2g(&gn,2)) || nneqgQ(&gb,ui2g(&gn,3))) {
		a->p = ub->p;
		mpx_set(a->l, ub->l);
		mpx_set(a->r, ub->r);
		gbnd_clear(&gb);
		gbnd_clear(&gn);
		gbnd_clear(&gv);
		gbnd_clear(&gu);
		return;
	}
	gbnd_clear(&gb);

	/* Refine the endpoints for the tightest possible unification. */
	if (gu.l.inf) {
		if (mpf_sgn(gu.l.f) > 0) mpx_set(uu, posinfu);
		else mpx_set(uu, neginfu);
	} else f2u(uu, gu.l.f);
	if (gv.r.inf) {
		if (mpf_sgn(gv.r.f) > 0) mpx_set(uv, posinfu);
		else mpx_set(uv, neginfu);
	} else f2u(uv, gv.r.f);
// printf(" promote:"); uview_un(uu); putchar(' '); uview_un(uv); putchar('\n');
	/* NOTE: if exact, expand open endpoints to include original interval */
	promote(uu, uw, uu, efsizemask);
	if (inexQ(u)) mpx_add(uu, uu, ubitmask); else mpx_sub(uu, uu, ubitmask);
	promote(uv, uw, uv, efsizemask);
	if (inexQ(v)) mpx_sub(uv, uv, ubitmask); else mpx_add(uv, uv, ubitmask);
	/* Check for one ulp difference between promoted u and v. */
	if (mpx_cmp(uu, uv) == 0) {
		a->p = 0;
		mpx_set(a->l, uu);
		mpx_set(a->r, uu);
		gbnd_clear(&gn);
		gbnd_clear(&gv);
		gbnd_clear(&gu);
		return;
	}

// printf(" oinf:"); uview_un(uu); putchar(' '); uview_un(uv); putchar('\n');
	unum2g(&gv, uv);
	/* If upper bound is open infinity and lower bound > maxreal,
	   special handling is needed. */
	if (gv.r.inf && mpf_sgn(gv.r.f) > 0 && gv.r.open) {
		unum2g(&gu, uu);
		unum2g(&gn, maxrealu);
		if (ltgQ(&gn, &gu)) {
			a->p = 0;
			mpx_set(a->l, maxrealu);
			mpx_ior(a->l, a->l, ubitmask);
			mpx_set(a->l, a->r);
		} else {
			/* Demote the left bound until the upper bound is open infinity. */
			for (;;) {
				unum2g(&gu, uu);
				if (gu.r.inf && mpf_sgn(gu.r.f) > 0) break;
				utag(&ut, uu);
				if (ut.esize > 1) demotee(uu, uu); else demotef(uu, uu); 
			}
			a->p = 0;
			mpx_set(a->l, uu);
			mpx_set(a->r, uu);
		}
		gbnd_clear(&gn);
		gbnd_clear(&gv);
		gbnd_clear(&gu);
		return;
	}

// printf(" demotee:"); uview_un(uu); putchar(' '); uview_un(uv); putchar('\n');
	/* While demoting exponents is possible and still
	   leaves unums within the ubound, demote both exponents. */
	for (;;) {
		if (mpx_cmp(uu, uv) == 0) break;
		utag(&ut, uu);
		if (ut.esize <= 1) break; /* assume uu & uv esize are same */
		demotee(uw, uu);
		unum2g(&gu, uw);
		demotee(ux, uv);
		unum2g(&gv, ux);
		if (mpf_cmp(gu.l.f, gv.l.f) >= 0 ||
			mpf_cmp(gu.r.f, gv.r.f) >= 0 ||
			(gv.r.inf && mpf_sgn(gv.r.f) > 0)) break;
		mpx_set(uu, uw);
		mpx_set(uv, ux);
	}
// printf(" demotef:"); uview_un(uu); putchar(' '); uview_un(uv); putchar('\n');
	for (;;) {
		if (mpx_cmp(uu, uv) == 0) break;
		utag(&ut, uu);
		if (ut.fsize <= 1) break; /* assume uu & uv fsize are same */
		/* fraction mask */
		mpx_lshift(uw, one, ut.fsize);
		mpx_sub_ui(uw, uw, 1);
		mpx_lshift(uw, uw, utagsize);
		mpx_set(ux, uw);
		/* fraction */
		mpx_and(uw, uu, uw);
		mpx_and(ux, uv, ux);
		if (mpx_cmp(uw, ux) == 0) break;
		demotef(uu, uu);
		demotef(uv, uv);
	}

// printf(" ozero:"); uview_un(uu); putchar(' '); uview_un(uv); putchar('\n');
	/* If u is inexact zero and v < 1, a little special handling is needed. */
	unum2g(&gv, uv);
	/* float mask */
	utag(&ut, uu);
	mpx_lshift(uw, one, 1 + ut.esize + ut.fsize); /* TODO: leave out sign bit? */
	mpx_sub_ui(uw, uw, 1);
	mpx_lshift(uw, uw, utagsize);
	/* float bits */
	mpx_and(uw, uu, uw);
	if (mpx_cmp(uu, uv) != 0 &&
		mpx_zero_p(uw) &&
		mpx_tstbit(uu, utagsize-1) &&
		ltgQ(&gv, ui2g(&gn,1)))
	{
		int n;
		long exp;
		double frac;
		mpf_t tmpf;
		if (exQ(uv)) mpx_add(ux, uv, ubitmask);
		else mpx_set(ux, uv);
		unum2g(&gv, ux);
		/* n = floor(log2(1-log2(f))) */
		frac = mpf_get_d_2exp(&exp, gv.r.f);
		if (frac == 0.5) exp--; /* ceil(log2(x)) */
#if defined(DEBUG)
		if (exp > 1)
			abort();
#endif
		exp = 1-exp;
		/* floor(log2(x)): 5->2, 4->2, 3->1, 2->1, 1->0 */
		for (n = 0; exp; exp >>= 1, n++) ; n--;
		if (esizemax < n) n = esizemax;
		a->p = 0;
		mpf_init2(tmpf, 1); /* only needs one bit of precision */
		mpf_set_ui(tmpf, 1);
		mpf_div_2exp(tmpf, tmpf, (1UL << n)-1);
		f2u(a->l, tmpf);
		mpx_sub(a->l, a->l, ubitmask);
		mpx_set(a->r, a->l);
		mpf_clear(tmpf);
		gbnd_clear(&gn);
		gbnd_clear(&gv);
		gbnd_clear(&gu);
		return;
	}

	if (mpx_cmp(uu, uv) == 0) {
		a->p = 0;
		mpx_set(a->l, uu);
		mpx_set(a->r, uu);
	} else {
		a->p = ub->p;
		mpx_set(a->l, ub->l);
		mpx_set(a->r, ub->r);
	}
// printf(" finish: "); print_ub(a); putchar('\n');

	gbnd_clear(&gn);
	gbnd_clear(&gv);
	gbnd_clear(&gu);
}

/* Seek a single-ULP enclosure for a ubound. */
/* NOTE: Mathematica version differs from book. */

/* Mathematica version of unify */
void unify(ubnd_s *a, const ubnd_s *ub)
{
	MPX_VAR(u1);
	MPX_VAR(u2);
	ubnd_s uba = {1, u1, u2};
	MPX_VAR(u3);
	MPX_VAR(u4);
	ubnd_s ubb = {1, u3, u4};
	unum_s *u;
	unum_s *v;
	gbnd_s gu, gv, zero;

	u = ub->l;
	v = (ub->p) ? ub->r : ub->l;
// printf(" unify: "); print_ub(ub); fflush(stdout);
	if (nanuQ(u) || nanuQ(v)) {
		a->p = 0;
		mpx_set(a->l, qNaNu);
		mpx_set(a->r, qNaNu);
		AOP1("NaN",a,ub,unify,ub,ub);
// printf(" NaN "); fflush(stdout);
		return;
	}
	if (mpx_cmp(u, posinfu) == 0 && mpx_cmp(v, posinfu) == 0) {
		a->p = 0;
		mpx_set(a->l, posinfu);
		mpx_set(a->r, posinfu);
// printf(" +Inf "); fflush(stdout);
		return;
	}
	if (mpx_cmp(u, neginfu) == 0 && mpx_cmp(v, neginfu) == 0) {
		a->p = 0;
		mpx_set(a->l, neginfu);
		mpx_set(a->r, neginfu);
// printf(" -Inf "); fflush(stdout);
		return;
	}
	gbnd_init(&gu);
	gbnd_init(&gv);
	gbnd_init(&zero);
	unum2g(&gu, u);
	unum2g(&gv, v);
	ui2g(&zero, 0);
	/* Skip trivial cases that cannot be unified. */
	/* NOTE: when called by g2u(), any NaN cases will be handled beforehand. */
	if (infuQ(u) || infuQ(v) || (ltgQ(&gu,&zero) && !ltgQ(&gv,&zero))) {
		a->p = ub->p;
		mpx_set(a->l, ub->l);
		mpx_set(a->r, ub->r);
		gbnd_clear(&gu);
		gbnd_clear(&gv);
		gbnd_clear(&zero);
// printf(" +-Inf_Intersect0 "); fflush(stdout);
		return;
	}
	if (ltgQ(&gu,&zero) && ltgQ(&gv,&zero)) {
		/* TODO: add a neg(unum_s *, const unum_s *) function to support.c? */
		/* ubnd.h not included here */
		void negateu(ubnd_s *a, const ubnd_s *u);
		negateu(&uba, ub);
		unifypos(&ubb, &uba);
		negateu(a, &ubb);
		gbnd_clear(&gu);
		gbnd_clear(&gv);
		gbnd_clear(&zero);
// printf(" NegUnify "); fflush(stdout);
		return;
	}
	/* If the bounds represent the same value, use the smaller bit string. */
	if (samegQ(&gu, &gv)) {
		unum_s *tu = (mpx_cmp(u,v) < 0) ? u : v;
		a->p = 0;
		mpx_set(a->l, tu);
		mpx_set(a->r, tu);
		gbnd_clear(&gu);
		gbnd_clear(&gv);
		gbnd_clear(&zero);
// printf(" Same "); fflush(stdout);
		return;
	}
	unifypos(a, ub);

// printf(" PosUnify "); print_ub(a); fflush(stdout);
	gbnd_clear(&gu);
	gbnd_clear(&gv);
	gbnd_clear(&zero);
}

#if 0
/* Book version of unify */
void unify(ubnd_s *a, const ubnd_s *ub)
{
	MPX_VAR(u1);
	MPX_VAR(u2);
	ubnd_s uba = {1, u1, u2};
	MPX_VAR(u3);
	MPX_VAR(u4);
	ubnd_s ubb = {1, u3, u4};
	gbnd_s g;

	gbnd_init(&g);
	u2g(&g, ub);
	/* Exception case */
	if (g.nan) {
		a->p = 0;
		mpx_set(a->l, qNaNu);
		mpx_set(a->r, qNaNu);
		gbnd_clear(&g);
		AOP1("NaN",a,ub,unify,ub,ub);
		return;
	}
	/* No need to check for intervals spanning 0; unifypos does it. */
	if (mpf_cmp_si(g.l.f, 0) >= 0) {
		unifypos(a, ub);
	} else {
		/* Negate both operands, call unifypos, and negate them again. */
		negateui(&uba, ub);
		unifypos(&ubb, &uba);
		negateui(a, &ubb);
	}
	gbnd_clear(&g);
}
#endif

/* Find the left half of a ubound (numerical value and open-closed bit). */

void ubleft(unum_s *u, const gnum_s *gn)
{
	mpf_t tmpf;

	if (gn->inf && mpf_sgn(gn->f) < 0) {
		if (gn->open) mpx_set(u, negopeninfu);
		else mpx_set(u, neginfu);
		return;
	}
	mpf_init2(tmpf, UPREC);
	/* TODO: reduce f2u calls. g2u() does one before calling ubleft(). */
	f2u(u, gn->f);
	u2f(tmpf, u);
	/* TODO: A less expensive test that doesn't require f2u -> u2f is needed. */
	if (mpf_cmp(tmpf, gn->f) == 0) {
		if (gn->open) {
			if (mpf_sgn(gn->f) < 0) mpx_sub(u, u, ulpu);
			mpx_ior(u, u, ubitmask);
		}
		mpf_clear(tmpf);
		return;
	}
	if (gn->open) mpx_ior(u, u, ubitmask);
	mpf_clear(tmpf);
}

/* Find the right half of a ubound (numerical value and open-closed bit).
   Not exactly the reverse of ubleft, because of "negative zero". */

void ubright(unum_s *u, const gnum_s *gn)
{
	mpf_t tmpf;

	if (gn->inf && mpf_sgn(gn->f) > 0) {
		if (gn->open) mpx_set(u, posopeninfu);
		else mpx_set(u, posinfu);
		return;
	}
	if (gn->open && mpf_sgn(gn->f) == 0) {
		mpx_set(u, negopenzerou);
		return;
	}
	mpf_init2(tmpf, UPREC);
	/* TODO: reduce f2u calls. g2u() does one before calling ubright(). */
	f2u(u, gn->f);
	u2f(tmpf, u);
	/* TODO: A less expensive test that doesn't require f2u -> u2f is needed. */
	if (mpf_cmp(tmpf, gn->f) == 0) {
		if (gn->open) {
			if (mpf_sgn(gn->f) >= 0) mpx_sub(u, u, ulpu);
			mpx_ior(u, u, ubitmask);
		}
		mpf_clear(tmpf);
		return;
	}
	if (gn->open) mpx_ior(u, u, ubitmask);
	mpf_clear(tmpf);
}

/* Convert a general interval to the closest possible ubound. */

void g2u(ubnd_s *a, const gbnd_s *g)
{
// printf(" g2u: "); print_gb(g);
	/* Get rid of the NaN cases first. */
	if (   g->nan ||
		(  g->l.inf  && !g->r.inf   && mpf_sgn(g->l.f) > 0) ||
		(! g->l.inf  &&  g->r.inf   && mpf_sgn(g->r.f) < 0) ||
		(!(g->l.inf   ^  g->r.inf)  && mpf_cmp(g->l.f,g->r.f) > 0) ||
#if defined(XOR_PATCH)
		/* FIXME: detect when g-layer truncation occurs */
		/* Proto difference: use xor instead of ior. */
		/* Instead of throwing a NaN when g-layer truncation occurs,
		   this allows a close but incorrect number to pass through. */
		( (g->l.open   ^  g->r.open) && mpf_cmp(g->l.f,g->r.f) == 0)
#else
		( (g->l.open  ||  g->r.open) && mpf_cmp(g->l.f,g->r.f) == 0)
#endif
		)
	{
		a->p = 0;
		mpx_set(a->l, qNaNu);
		mpx_set(a->r, a->l);
		AOP1("NaN",a,ub,g2u,g,gb);
// printf(" NaN: "); print_ub(a); putchar('\n');
		return;
	}
	/* Handle Inf cases */
	a->p = 1;
	if (g->l.inf) {
		if (mpf_sgn(g->l.f) > 0) mpx_set(a->l, posinfu);
		else mpx_set(a->l, neginfu);
	} else f2u(a->l, g->l.f);
	if (g->r.inf) {
		if (mpf_sgn(g->r.f) > 0) mpx_set(a->r, posinfu);
		else mpx_set(a->r, neginfu);
	} else f2u(a->r, g->r.f);
// printf(" g2u(a): "); print_ub(a); putchar(' '); uview_ub(a);
	/* If both unums are identical, and both open or both closed,
	   we have a single unum bound. */
	if (!(g->l.open ^ g->r.open) && mpx_cmp(a->l,a->r) == 0)
	{
		/*
		The open state of a gnum could change if the environment changes. f2u() 
		does not set the ubit explicitly, only implicitly based on the fraction 
		bits available. This assumes that the environment doesn't change between 
		the setting of the open flag and when it is read. Otherwise the open 
		state of the unum returned here could be different from the open flag. 
		*/
		a->p = 0;
// printf(" [x] or (x,x+ulp)?: "); print_un(a->l); putchar('\n');
		return;
	}
	/* See if general interval is expressible as a single unum without loss. */
	{
		MPX_VAR(u1);
		MPX_VAR(u2);
		ubnd_s ub = {1, u1, u2};
		gbnd_s gA, gB;

		gbnd_init(&gA);
		gbnd_init(&gB);

		/* TODO: A less expensive test that doesn't require f2u -> u2f (again) is needed. */
		ubleft(ub.l, &g->l);
		ubright(ub.r, &g->r);
		unify(a, &ub);
		u2g(&gA, a);
		u2g(&gB, &ub);
		if (gA.nan != gB.nan ||
			mpf_cmp(gA.l.f, gB.l.f) != 0 ||
			gA.l.inf != gB.l.inf ||
			gA.l.open != gB.l.open ||
			mpf_cmp(gA.r.f, gB.r.f) != 0 ||
			gA.r.inf != gB.r.inf ||
			gA.r.open != gB.r.open)
		{
			a->p = 1;
			mpx_set(a->l, ub.l);
			mpx_set(a->r, ub.r);
// printf(" (no-unify)");
		}
// printf(" [(x,y)]: "); print_ub(a); putchar('\n');

		gbnd_clear(&gA);
		gbnd_clear(&gB);
	}
}

/* Convert a general interval to an exact ubound with rounding. */

void g2ur(ubnd_s *a, const gbnd_s *g)
{
	int inf;
	mpf_t tmpf;

	a->p = 0;
	/* NaN case */
	if (   g->nan ||
		(  g->l.inf  && !g->r.inf   && mpf_sgn(g->l.f) > 0) ||
		(! g->l.inf  &&  g->r.inf   && mpf_sgn(g->r.f) < 0) ||
		(!(g->l.inf   ^  g->r.inf)  && mpf_cmp(g->l.f,g->r.f) > 0) ||
#if defined(XOR_PATCH)
		/* FIXME: detect when g-layer truncation occurs */
		/* Proto difference: use xor instead of ior. */
		/* Instead of throwing a NaN when g-layer truncation occurs,
		   this allows a close but incorrect number to pass through. */
		( (g->l.open   ^  g->r.open) && mpf_cmp(g->l.f,g->r.f) == 0)
#else
		( (g->l.open  ||  g->r.open) && mpf_cmp(g->l.f,g->r.f) == 0)
#endif
		)
	{
		mpx_set(a->l, qNaNu);
		mpx_set(a->r, a->l);
		AOP1("NaN",a,ub,g2ur,g,gb);
		return;
	}
	/* Average the endpoint values and convert to a unum. */
	mpf_init2(tmpf, UPREC);
	inf = midpoint(tmpf, g);
	if (inf > 0) mpx_set(a->l, posinfu);
	else if (inf < 0) mpx_set(a->l, neginfu);
	else f2u(a->l, tmpf);
	mpf_clear(tmpf);
	roundu(a->l);
}

/* Turn a unum into a guess, for imitation of float behavior. */

void guessu(unum_s *a, const ubnd_s *ub)
{
	int inf;
	gbnd_s gb;
	mpf_t tmpf;

	gbnd_init(&gb);
	u2g(&gb, ub);
	/* NaN case */
	if (gb.nan) {
		mpx_set(a, qNaNu);
		gbnd_clear(&gb);
		AOP1("NaN",a,un,guessu,ub,ub);
		return;
	}
	/* Average the endpoint values and convert to a unum. */
	mpf_init2(tmpf, UPREC);
	inf = midpoint(tmpf, &gb);
	if (inf > 0) mpx_set(a, posinfu);
	else if (inf < 0) mpx_set(a, neginfu);
	else f2u(a, tmpf);
	mpf_clear(tmpf);
	gbnd_clear(&gb);
	roundu(a);
}
