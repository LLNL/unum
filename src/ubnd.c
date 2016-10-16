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

#include "uenv.h"
#include "ubnd.h"
#include "gbnd.h"
#include "support.h"
#include "conv.h"

#if defined(ROUND)
#define G2U(ub,gb) g2ur(ub,gb)
#else

#if defined(DEBUG_EXT)

#include <stdio.h> /* printf */
#include <stdlib.h> /* abort */
#include "hlayer.h" /* print_ub */

#define G2U(ub,gb) do { \
	gbnd_s tgb; \
	gbnd_init(&tgb); \
	g2u(ub,gb); \
	u2g(&tgb,ub); \
	if (tgb.l.inf || tgb.r.inf \
		) { \
		printf("G2U: %s, %s, line %d.\n", __FILE__,__func__,__LINE__); \
		printf("extreme: "); print_ub(ub); putchar('\n'); \
		abort(); \
	} \
	gbnd_clear(&tgb); \
} while (0)
#if 0
	mpf_t tfw; \
	mpf_init2(tfw, UPREC); \
	mpf_sub(tfw, tgb.r.f, tgb.l.f); \
	mpf_mul_2exp(tfw, tfw, 15); \
		|| mpf_cmp(tfw, maxreal) > 0 \
		|| (tgb.l.open && mpf_sgn(tgb.l.f) == 0) || (tgb.r.open && mpf_sgn(tgb.r.f) == 0) \
	mpf_clear(tfw);
#endif

#else
#define G2U(ub,gb) g2u(ub,gb)
#endif /* DEBUG_EXT */

#endif /* ROUND */

ustats_t stats;

#if defined(STATS)

#define INTERVAL(ub) ((ub->p) ? 2 : inexQ(ub->l) ? 1 : 0)

static int nbits(const ubnd_s *u)
{
	int total = 1;
	utag_s ut;

	utag(&ut, u->l);
	total += 1 + ut.esize + ut.fsize + utagsize;
	if (u->p) {
		utag(&ut, u->r);
		total += 1 + ut.esize + ut.fsize + utagsize;
	}
#if defined(NBITS_HISTO)
	if (total <= MAX_NBITS) stats.nbits[total]++;
#endif
	return total;
}

void tally2(const ubnd_s *a, const ubnd_s *u)
{
	stats.ubitsmoved += nbits(a) + nbits(u);
	stats.ubndsmoved += 2;
	stats.ops++;
	stats.opc2[INTERVAL(a)][INTERVAL(u)]++;
}

void tally3(const ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	stats.ubitsmoved += nbits(a) + nbits(u) + nbits(v);
	stats.ubndsmoved += 3;
	stats.ops++;
	stats.opc3[INTERVAL(a)][INTERVAL(u)][INTERVAL(v)]++;
}

#define TALLY2(a,u) tally2(a,u);
#define TALLY3(a,u,v) tally3(a,u,v);

#else
#define TALLY2(a,u)
#define TALLY3(a,u,v)
#endif

/* Test if ubound u is strictly less than ubound v. */

int ltuQ(const ubnd_s *u, const ubnd_s *v)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = ltgQ(&g, &h);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Test if ubound u is strictly greater than ubound v. */

int gtuQ(const ubnd_s *u, const ubnd_s *v)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = gtgQ(&g, &h);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Test if ubound u is nowhere equal to ubound v. */

int nequQ(const ubnd_s *u, const ubnd_s *v)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = neqgQ(&g, &h);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Test if ubound u is not nowhere (somewhere) equal to ubound v. */

int nnequQ(const ubnd_s *u, const ubnd_s *v)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = nneqgQ(&g, &h);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Test if ubound u value is identical to ubound v value. */

int sameuQ(const ubnd_s *u, const ubnd_s *v)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = samegQ(&g, &h);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Compare end points from ubounds u and v. */

int cmpuQ(const ubnd_s *u, end_t ue, const ubnd_s *v, end_t ve)
{
	int res;
	gbnd_s g, h;

	gbnd_init(&g);
	gbnd_init(&h);

	u2g(&g, u);
	u2g(&h, v);
	res = cmpgQ(&g, ue, &h, ve);

	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

/* Test if a ubound spans zero. */

int spanszerouQ(const ubnd_s *u)
{
	int res;
	gbnd_s g;

	gbnd_init(&g);

	u2g(&g, u);
	res = spanszerogQ(&g);

	gbnd_clear(&g);
	return res;
}

/* Addition in the u-layer. */
/* With tallying of bits and numbers moved. */

void plusu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	plusg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

/* Subtraction in the u-layer. */
/* With tallying of bits and numbers moved. */

void minusu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	minusg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

/* Multiplication in the u-layer. */
/* With tallying of bits and numbers moved. */

void timesu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	timesg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

/* Division in the u-layer. */
/* With tallying of bits and numbers moved. */

void divideu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	divideg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

/* Square in the u-layer. */
/* With tallying of bits and numbers moved. */

void squareu(ubnd_s *a, const ubnd_s *u)
{
	gbnd_s g, x;

	gbnd_init(&g);
	gbnd_init(&x);

	u2g(&g, u);
	squareg(&x, &g);
	G2U(a, &x);
	TALLY2(a,u)

	gbnd_clear(&x);
	gbnd_clear(&g);
}

/* Square root in the u-layer. */
/* With tallying of bits and numbers moved. */

void sqrtu(ubnd_s *a, const ubnd_s *u)
{
	gbnd_s g, x;

	gbnd_init(&g);
	gbnd_init(&x);

	u2g(&g, u);
	sqrtg(&x, &g);
	G2U(a, &x);
	TALLY2(a,u)

	gbnd_clear(&x);
	gbnd_clear(&g);
}

/* Negate a ubound. */
/* TODO: why no tallying in prototype? */

void negateu(ubnd_s *a, const ubnd_s *u)
{
	MPX_VAR(tmpl);
	MPX_VAR(tmpr);

	a->p = u->p;
	if (!mpx_zero_p(u->l)) {
		signmask(tmpl, u->l);
		mpx_xor(tmpl, u->l, tmpl);
	} else mpx_set_ui(tmpl, 0);
	if (u->p && !mpx_zero_p(u->r)) {
		signmask(tmpr, u->r);
		mpx_xor(tmpr, u->r, tmpr);
	} else mpx_set_ui(tmpr, 0);
	mpx_set(a->l, (u->p) ? tmpr : tmpl);
	if (u->p) mpx_set(a->r, tmpl);
}

/* Absolute value in the u-layer. */
/* With tallying of bits and numbers moved. */

void absu(ubnd_s *a, const ubnd_s *u)
{
	gbnd_s g, x;

	gbnd_init(&g);
	gbnd_init(&x);

	u2g(&g, u);
	absg(&x, &g);
	G2U(a, &x);
	TALLY2(a,u)

	gbnd_clear(&x);
	gbnd_clear(&g);
}

void minu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	ming(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

void maxu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	maxg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
}

int cliplu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;
	int res;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	res = cliplg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}

int cliphu(ubnd_s *a, const ubnd_s *u, const ubnd_s *v)
{
	gbnd_s g, h, x;
	int res;

	gbnd_init(&g);
	gbnd_init(&h);
	gbnd_init(&x);

	u2g(&g, u);
	u2g(&h, v);
	res = cliphg(&x, &g, &h);
	G2U(a, &x);
	TALLY3(a,u,v)

	gbnd_clear(&x);
	gbnd_clear(&h);
	gbnd_clear(&g);
	return res;
}
