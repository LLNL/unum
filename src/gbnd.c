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

#include "gbnd.h"
#include "hlayer.h"


/* scratchpad */

/* Test if interval g is strictly less than interval h. */

int ltgQ(const gbnd_s *g, const gbnd_s *h)
{
	return !(g->nan || h->nan) && cmp_gn(&g->r, RE, &h->l, LE) < 0;
}

/* Test if interval g is strictly greater than interval h. */

int gtgQ(const gbnd_s *g, const gbnd_s *h)
{
	return !(g->nan || h->nan) && cmp_gn(&g->l, LE, &h->r, RE) > 0;
}

/* Test if interval g is nowhere equal to interval h. */

int neqgQ(const gbnd_s *g, const gbnd_s *h)
{
	return !(g->nan || h->nan) && (ltgQ(g, h) || gtgQ(g, h));
}

/* Test if interval g is not nowhere (somewhere) equal to interval h. */

int nneqgQ(const gbnd_s *g, const gbnd_s *h)
{
	return !(g->nan || h->nan) && !(ltgQ(g, h) || gtgQ(g, h));
}

/* Test if interval g is identical to interval h. */

int samegQ(const gbnd_s *g, const gbnd_s *h)
{
	return (g->nan && h->nan) ||
		(
			g->nan == h->nan &&
			mpf_cmp(g->l.f, h->l.f) == 0 &&
			g->l.inf == h->l.inf &&
			g->l.open == h->l.open &&
			mpf_cmp(g->r.f, h->r.f) == 0 &&
			g->r.inf == h->r.inf &&
			g->r.open == h->r.open
		);
}

/* Compare end points from intervals g and h. */

int cmpgQ(const gbnd_s *g, end_t ge, const gbnd_s *h, end_t he)
{
	if (g->nan || h->nan) return 0; /* FIXME: what about NaNs? */
	else return cmp_gn(ge==LE ? &g->l : &g->r, ge, he==LE ? &h->l : &h->r, he);
}

/* Test if interval g spans zero. */

int spanszerogQ(const gbnd_s *g)
{
	return ((!mpf_sgn(g->l.f) && !g->l.open) || (!mpf_sgn(g->r.f) && !g->r.open)) ||
	        (mpf_sgn(g->l.f) < 0 && mpf_sgn(g->r.f) > 0);
}

/* Add two general intervals. */

void plusg(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	/* If any value is NaN, the result is also NaN. */
	if (x->nan || y->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,plusg,y,gb);
		return;
	}
	a->nan = 0;

	/* Compute left endpoint: */
	/* Cases involving exact infinity or -infinity: */
	if (x->l.inf && mpf_sgn(x->l.f) < 0 && !x->l.open) {
		if (y->l.inf && mpf_sgn(y->l.f) > 0 && !y->l.open) {a->nan = 1;}
		else {mpf_set_si(a->l.f, -1); a->l.inf = 1; a->l.open = 0;}
	} else if (y->l.inf && mpf_sgn(y->l.f) < 0 && !y->l.open) {
		if (x->l.inf && mpf_sgn(x->l.f) > 0 && !x->l.open) {a->nan = 1;}
		else {mpf_set_si(a->l.f, -1); a->l.inf = 1; a->l.open = 0;}
	} else if (
		(x->l.inf && mpf_sgn(x->l.f) > 0 && !x->l.open) ||
		(y->l.inf && mpf_sgn(y->l.f) > 0 && !y->l.open)
		) {
		mpf_set_si(a->l.f, 1); a->l.inf = 1; a->l.open = 0;
	} else if (x->l.inf && mpf_sgn(x->l.f) < 0) {
		if (y->l.inf && mpf_sgn(y->l.f) > 0 && !y->l.open)
			{mpf_set_si(a->l.f, 1); a->l.inf = 1; a->l.open = 0;}
		else
			{mpf_set_si(a->l.f, -1); a->l.inf = 1; a->l.open = 1;}
	} else if (y->l.inf && mpf_sgn(y->l.f) < 0) {
		if (x->l.inf && mpf_sgn(x->l.f) > 0 && !x->l.open)
			{mpf_set_si(a->l.f, 1); a->l.inf = 1; a->l.open = 0;}
		else
			{mpf_set_si(a->l.f, -1); a->l.inf = 1; a->l.open = 1;}
	} else {
		/* What's left is the arithmetic case, done with extended precision. */
		mpf_add(a->l.f, x->l.f, y->l.f);
		a->l.inf = 0;
		a->l.open = x->l.open || y->l.open;
		/* NOTE: this range check is not in the Mathematica notebook 6-24-2015 */
#if 0
		{
			mpf_t tmpf;
			mpf_init2(tmpf, PBITS);
			mpf_neg(tmpf, maxreal);
			if (mpf_cmp(a->l.f, tmpf) < 0) {
				mpf_set_si(a->l.f, -1); a->l.inf = 1; a->l.open = 1;
			} else if (mpf_cmp(a->l.f, maxreal) > 0) {
				mpf_set(a->l.f, maxreal); a->l.inf = 0; a->l.open = 1;
			}
			mpf_clear(tmpf);
		}
#endif
	}

	/* Compute right endpoint, using similar logic: */
	/* Cases involving exact infinity or -infinity: */
	if (x->r.inf && mpf_sgn(x->r.f) < 0 && !x->r.open) {
		if (y->r.inf && mpf_sgn(y->r.f) > 0 && !y->r.open) {a->nan = 1;}
		else {mpf_set_si(a->r.f, -1); a->r.inf = 1; a->r.open = 0;}
	} else if (y->r.inf && mpf_sgn(y->r.f) < 0 && !y->r.open) {
		if (x->r.inf && mpf_sgn(x->r.f) > 0 && !x->r.open) {a->nan = 1;}
		else {mpf_set_si(a->r.f, -1); a->r.inf = 1; a->r.open = 0;}
	} else if (
		(x->r.inf && mpf_sgn(x->r.f) > 0 && !x->r.open) ||
		(y->r.inf && mpf_sgn(y->r.f) > 0 && !y->r.open)
		) {
		mpf_set_si(a->r.f, 1); a->r.inf = 1; a->r.open = 0;
	} else if (x->r.inf && mpf_sgn(x->r.f) > 0) {
		if (y->r.inf && mpf_sgn(y->r.f) < 0 && !y->r.open)
			{mpf_set_si(a->r.f, -1); a->r.inf = 1; a->r.open = 0;}
		else
			{mpf_set_si(a->r.f, 1); a->r.inf = 1; a->r.open = 1;}
	} else if (y->r.inf && mpf_sgn(y->r.f) > 0) {
		if (x->r.inf && mpf_sgn(x->r.f) < 0 && !x->r.open)
			{mpf_set_si(a->r.f, -1); a->r.inf = 1; a->r.open = 0;}
		else
			{mpf_set_si(a->r.f, 1); a->r.inf = 1; a->r.open = 1;}
	} else {
		/* What's left is the arithmetic case, done with extended precision. */
		mpf_add(a->r.f, x->r.f, y->r.f);
		a->r.inf = 0;
		a->r.open = x->r.open || y->r.open;
		/* TODO: this range check is not in the Mathematica notebook 6-24-2015 */
#if 0
		{
			mpf_t tmpf;
			mpf_init2(tmpf, PBITS);
			mpf_neg(tmpf, maxreal);
			if (mpf_cmp(a->r.f, tmpf) < 0) {
				mpf_set(a->r.f, tmpf); a->r.inf = 0; a->r.open = 1;
			} else if (mpf_cmp(a->r.f, maxreal) > 0) {
				mpf_set_si(a->r.f, 1); a->r.inf = 1; a->r.open = 1;
			}
			mpf_clear(tmpf);
		}
#endif
	}

	if (a->nan) {
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,plusg,y,gb);
	}
}

/* Subtraction in the g-layer. */

void minusg(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	gbnd_s gb;

	gbnd_init(&gb);
	negateg(&gb, y);
	plusg(a, x, &gb);
	gbnd_clear(&gb);
}

/* The "left" multiplication table for general intervals. */

static int timesposleft(gnum_s *a, const gnum_s *x, const gnum_s *y)
{
	int nan = 0;

	if (!mpf_sgn(x->f) && !x->open) {
		if (y->inf && !y->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;}
	} else if (!mpf_sgn(y->f) && !y->open) {
		if (x->inf && !x->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;}
	} else if (!mpf_sgn(x->f) && x->open) {
		if (y->inf && !y->open) {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
		else {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
	} else if (!mpf_sgn(y->f) && y->open) {
		if (x->inf && !x->open) {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
		else {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
	} else if ((x->inf && !x->open) || (y->inf && !y->open)) {
		mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;
	} else {
		mpf_mul(a->f, x->f, y->f);
		a->inf = 0;
		a->open = x->open || y->open;
	}
	return nan;
}

/* The "right" multiplication table for general intervals. */

static int timesposright(gnum_s *a, const gnum_s *x, const gnum_s *y)
{
	int nan = 0;

	if (x->inf && !x->open) {
		if (!mpf_sgn(y->f) && !y->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
	} else if (y->inf && !y->open) {
		if (!mpf_sgn(x->f) && !x->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
	} else if (x->inf && x->open) {
		if (!mpf_sgn(y->f) && !y->open) {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 1;}
	} else if (y->inf && y->open) {
		if (!mpf_sgn(x->f) && !x->open) {mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 1;}
	} else if ((!mpf_sgn(x->f) && !x->open) || (!mpf_sgn(y->f) && !y->open)) {
		mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;
	} else {
		mpf_mul(a->f, x->f, y->f);
		a->inf = 0;
		a->open = x->open || y->open;
	}
	return nan;
}

/* Multiplication in the g-layer. */

void timesg(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	gnum_s lcan, rcan;
	gnum_s agn, xgn, ygn;

	/* If any value is NaN, the result is also NaN. */
	if (x->nan || y->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,timesg,y,gb);
		return;
	}
	a->nan = 0;

	gnum_init(&lcan);
	gnum_init(&rcan);
	gnum_init(&agn);
	gnum_init(&xgn);
	gnum_init(&ygn);

	/* Lower left corner is in upper right quadrant, facing uphill: */
	if (mpf_sgn(x->l.f) >= 0 && mpf_sgn(y->l.f) >= 0) {
		a->nan |= timesposleft(&lcan, &x->l, &y->l);
	} else {mpf_set_si(lcan.f, 1); lcan.inf = 1; lcan.open = 0;}
	/* Upper right corner is in lower left quadrant, facing uphill: */
	if ((mpf_sgn(x->r.f) < 0 || ((mpf_sgn(x->r.f) == 0) && x->r.open)) &&
	    (mpf_sgn(y->r.f) < 0 || ((mpf_sgn(y->r.f) == 0) && y->r.open))) {
		neg_gn(&xgn, &x->r);
		neg_gn(&ygn, &y->r);
		a->nan |= timesposleft(&agn, &xgn, &ygn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}
	/* Upper left corner is in upper left quadrant, facing uphill: */
	if ((mpf_sgn(x->l.f) < 0 || ((mpf_sgn(x->l.f) == 0) && !x->l.open)) &&
	    (mpf_sgn(y->r.f) > 0 || ((mpf_sgn(y->r.f) == 0) && !y->r.open))) {
		neg_gn(&xgn, &x->l);
		a->nan |= timesposright(&agn, &xgn, &y->r);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}
	/* Lower right corner is in lower right quadrant, facing uphill: */
	if ((mpf_sgn(x->r.f) > 0 || ((mpf_sgn(x->r.f) == 0) && !x->r.open)) &&
	    (mpf_sgn(y->l.f) < 0 || ((mpf_sgn(y->l.f) == 0) && !y->l.open))) {
		neg_gn(&ygn, &y->l);
		a->nan |= timesposright(&agn, &x->r, &ygn);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}

	/* Upper right corner is in upper right quadrant, facing downhill: */
	if ((mpf_sgn(x->r.f) > 0 || ((mpf_sgn(x->r.f) == 0) && !x->r.open)) &&
	    (mpf_sgn(y->r.f) > 0 || ((mpf_sgn(y->r.f) == 0) && !y->r.open))) {
		a->nan |= timesposright(&rcan, &x->r, &y->r);
	} else {mpf_set_si(rcan.f, -1); rcan.inf = 1; rcan.open = 0;}
	/* Lower left corner is in lower left quadrant, facing downhill: */
	if ((mpf_sgn(x->l.f) < 0 || ((mpf_sgn(x->l.f) == 0) && !x->l.open)) &&
	    (mpf_sgn(y->l.f) < 0 || ((mpf_sgn(y->l.f) == 0) && !y->l.open))) {
		neg_gn(&xgn, &x->l);
		neg_gn(&ygn, &y->l);
		a->nan |= timesposright(&agn, &xgn, &ygn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}
	/* Lower right corner is in upper left quadrant, facing downhill: */
	if ((mpf_sgn(x->r.f) < 0 || ((mpf_sgn(x->r.f) == 0) && x->r.open)) &&
	    (mpf_sgn(y->l.f) >= 0)) {
		neg_gn(&xgn, &x->r);
		a->nan |= timesposleft(&agn, &xgn, &y->l);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}
	/* Upper left corner is in lower right quadrant, facing downhill: */
	if ((mpf_sgn(x->l.f) >= 0) &&
	    (mpf_sgn(y->r.f) < 0 || ((mpf_sgn(y->r.f) == 0) && y->r.open))) {
		neg_gn(&ygn, &y->r);
		a->nan |= timesposleft(&agn, &x->l, &ygn);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}

	if (a->nan) {
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,timesg,y,gb);
	}	else {
		mpf_set(a->l.f, lcan.f);
		a->l.inf = lcan.inf;
		a->l.open = lcan.open;
		mpf_set(a->r.f, rcan.f);
		a->r.inf = rcan.inf;
		a->r.open = rcan.open;
	}
	gnum_clear(&ygn);
	gnum_clear(&xgn);
	gnum_clear(&agn);
	gnum_clear(&rcan);
	gnum_clear(&lcan);
}

/* The "left" division table for general intervals. */

static int divideposleft(gnum_s *a, const gnum_s *x, const gnum_s *y)
{
	int nan = 0;

	if (!mpf_sgn(y->f) && !y->open) {
		nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;
	} else if (x->inf && !x->open) {
		if (y->inf && !y->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
	} else if ((!mpf_sgn(x->f) && !x->open) || (y->inf && !y->open)) {
		mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;
	} else if ((!mpf_sgn(x->f) && x->open) || (y->inf && y->open)) {
		mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;
	} else {
		mpf_div(a->f, x->f, y->f);
		a->inf = 0;
		a->open = x->open || y->open;
	}
	return nan;
}

/* The "right" division table for general intervals. */

static int divideposright(gnum_s *a, const gnum_s *x, const gnum_s *y)
{
	int nan = 0;

	if (!mpf_sgn(y->f) && !y->open) {
		nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;
	} else if (x->inf && !x->open) {
		if (y->inf && !y->open) {nan = 1; mpf_set_ui(a->f, 0); a->inf = 0; a->open = 1;}
		else {mpf_set_ui(a->f, 1); a->inf = 1; a->open = 0;}
	} else if ((!mpf_sgn(x->f) && !x->open) || (y->inf && !y->open)) {
		mpf_set_ui(a->f, 0); a->inf = 0; a->open = 0;
	} else if ((x->inf && x->open) || (!mpf_sgn(y->f) && y->open)) {
		mpf_set_ui(a->f, 1); a->inf = 1; a->open = 1;
	} else {
		mpf_div(a->f, x->f, y->f);
		a->inf = 0;
		a->open = x->open || y->open;
	}
	return nan;
}

/* Division in the g-layer. */

void divideg(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	gnum_s lcan, rcan;
	gnum_s agn, xgn, ygn;

	/* If any value is NaN, or denominator contains 0, the result is also NaN. */
	if (x->nan || y->nan ||
	    ((mpf_sgn(y->l.f) < 0 || ((mpf_sgn(y->l.f) == 0) && !y->l.open)) &&
	     (mpf_sgn(y->r.f) > 0 || ((mpf_sgn(y->r.f) == 0) && !y->r.open)))) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,divideg,y,gb);
		return;
	}
	a->nan = 0;

	gnum_init(&lcan);
	gnum_init(&rcan);
	gnum_init(&agn);
	gnum_init(&xgn);
	gnum_init(&ygn);

	/* FIXME: should not need a test for (y == 0(closed)), follows prototype */
	/* Upper left corner is in upper right quadrant, facing uphill: */
	if (mpf_sgn(x->l.f) >= 0 &&
	    (mpf_sgn(y->r.f) > 0 || ((mpf_sgn(y->r.f) == 0) && !y->r.open))) {
		a->nan |= divideposleft(&lcan, &x->l, &y->r);
	} else {mpf_set_si(lcan.f, 1); lcan.inf = 1; lcan.open = 0;}
	/* Lower right corner is in lower left quadrant, facing uphill: */
	if ((mpf_sgn(x->r.f) < 0 || ((mpf_sgn(x->r.f) == 0) &&  x->r.open)) &&
	    (mpf_sgn(y->l.f) < 0 || ((mpf_sgn(y->l.f) == 0) && !y->l.open))) {
		neg_gn(&xgn, &x->r);
		neg_gn(&ygn, &y->l);
		a->nan |= divideposleft(&agn, &xgn, &ygn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}
	/* Lower left corner is in upper left quadrant, facing uphill: */
	if ((mpf_sgn(x->l.f) < 0 || ((mpf_sgn(x->l.f) == 0) && !x->l.open)) &&
	    mpf_sgn(y->l.f) >= 0) {
		neg_gn(&xgn, &x->l);
		a->nan |= divideposright(&agn, &xgn, &y->l);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}
	/* Upper right corner is in lower right quadrant, facing uphill: */
	if ((mpf_sgn(x->r.f) > 0 || ((mpf_sgn(x->r.f) == 0) && !x->r.open)) &&
	    (mpf_sgn(y->r.f) < 0 || ((mpf_sgn(y->r.f) == 0) &&  y->r.open))) {
		neg_gn(&ygn, &y->r);
		a->nan |= divideposright(&agn, &x->r, &ygn);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, LE, &lcan, LE) < 0) {
			mpf_set(lcan.f, agn.f);
			lcan.inf = agn.inf;
			lcan.open = agn.open;
		}
	}

	/* Lower right corner is in upper right quadrant, facing downhill: */
	if ((mpf_sgn(x->r.f) > 0 || ((mpf_sgn(x->r.f) == 0) && !x->r.open)) &&
	    mpf_sgn(y->l.f) >= 0) {
		a->nan |= divideposright(&rcan, &x->r, &y->l);
	} else {mpf_set_si(rcan.f, -1); rcan.inf = 1; rcan.open = 0;}
	/* Upper left corner is in lower left quadrant, facing downhill: */
	if ((mpf_sgn(x->l.f) < 0 || ((mpf_sgn(x->l.f) == 0) && !x->l.open)) &&
	    (mpf_sgn(y->r.f) < 0 || ((mpf_sgn(y->r.f) == 0) &&  y->r.open))) {
		neg_gn(&xgn, &x->l);
		neg_gn(&ygn, &y->r);
		a->nan |= divideposright(&agn, &xgn, &ygn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}
	/* Upper right corner is in upper left quadrant, facing downhill: */
	if ((mpf_sgn(x->r.f) < 0 || ((mpf_sgn(x->r.f) == 0) &&  x->r.open)) &&
	    (mpf_sgn(y->r.f) > 0 || ((mpf_sgn(y->r.f) == 0) && !y->r.open))) {
		neg_gn(&xgn, &x->r);
		a->nan |= divideposleft(&agn, &xgn, &y->r);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}
	/* Lower left corner is in lower right quadrant, facing downhill: */
	if (mpf_sgn(x->l.f) >= 0 &&
	    (mpf_sgn(y->l.f) < 0 || ((mpf_sgn(y->l.f) == 0) && !y->l.open))) {
		neg_gn(&ygn, &y->l);
		a->nan |= divideposleft(&agn, &x->l, &ygn);
		neg_gn(&agn, &agn);
		if (!a->nan && cmp_gn(&agn, RE, &rcan, RE) > 0) {
			mpf_set(rcan.f, agn.f);
			rcan.inf = agn.inf;
			rcan.open = agn.open;
		}
	}

	if (a->nan) {
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,divideg,y,gb);
	}	else {
		mpf_set(a->l.f, lcan.f);
		a->l.inf = lcan.inf;
		a->l.open = lcan.open;
		mpf_set(a->r.f, rcan.f);
		a->r.inf = rcan.inf;
		a->r.open = rcan.open;
	}
	gnum_clear(&ygn);
	gnum_clear(&xgn);
	gnum_clear(&agn);
	gnum_clear(&rcan);
	gnum_clear(&lcan);
}

/* Square in the g-layer. */

void squareg(gbnd_s *a, const gbnd_s *g)
{
	gnum_s t1, t2;
	gnum_s *aL, *aR;

	if (g->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP1("NaN",a,gb,squareg,g,gb);
		return;
	}
	a->nan = 0;

	gnum_init(&t1);
	gnum_init(&t2);

	mpf_pow_ui(t1.f, g->l.f, 2);
	t1.inf = g->l.inf;
	t1.open = g->l.open;

	mpf_pow_ui(t2.f, g->r.f, 2);
	t2.inf = g->r.inf;
	t2.open = g->r.open;

	if (cmp_gn(&t1, RE, &t2, RE) > 0) {
		aL = &t2; aR = &t1;
	} else {
		aL = &t1; aR = &t2;
	}

	/* See if 0 is in the range */
	if (spanszerogQ(g)) {
		mpf_set_si(a->l.f, 0);
		a->l.inf = 0;
		a->l.open = 0;
	} else {
		mpf_set(a->l.f, aL->f);
		a->l.inf = aL->inf;
		a->l.open = aL->open;
	}

	mpf_set(a->r.f, aR->f);
	a->r.inf = aR->inf;
	a->r.open = aR->open;

	gnum_clear(&t2);
	gnum_clear(&t1);
}

/* Square root in the g-layer. */

void sqrtg(gbnd_s *a, const gbnd_s *g)
{
	if (g->nan || mpf_sgn(g->l.f) < 0) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP1("NaN",a,gb,sqrtg,g,gb);
		return;
	}
	a->nan = 0;

	mpf_sqrt(a->l.f, g->l.f);
	mpf_sqrt(a->r.f, g->r.f);
	a->l.inf = g->l.inf;
	a->r.inf = g->r.inf;
	a->l.open = g->l.open;
	a->r.open = g->r.open;
}

/* Negate a general interval. */

void negateg(gbnd_s *a, const gbnd_s *g)
{
	gnum_s tmp;
	const gnum_s *gl, *gr;

	/* Handle NaN input */
	if (g->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP1("NaN",a,gb,negateg,g,gb);
		return;
	}
	a->nan = 0;

	/* Use temporary if input and output are the same */
	if (a == g) {
		gnum_init(&tmp);
		mpf_set(tmp.f, g->l.f);
		tmp.inf  = g->l.inf;
		tmp.open = g->l.open;
		gl = &tmp;
	} else {
		gl = &g->l;
	}
	gr = &g->r;

	/* Negate and reverse */
	mpf_neg(a->l.f, gr->f);
	a->l.inf  = gr->inf;
	a->l.open = gr->open;

	mpf_neg(a->r.f, gl->f);
	a->r.inf  = gl->inf;
	a->r.open = gl->open;

	if (a == g) gnum_clear(&tmp);
}

/* Absolute value in the g-layer. */

void absg(gbnd_s *a, const gbnd_s *g)
{
	gbnd_s tmp;

	if (g->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP1("NaN",a,gb,absg,g,gb);
		return;
	}
	a->nan = 0;

	gbnd_init(&tmp);
	mpf_abs(tmp.l.f, g->l.f);
	tmp.l.inf = g->l.inf;
	tmp.l.open = g->l.open;
	mpf_abs(tmp.r.f, g->r.f);
	tmp.r.inf = g->r.inf;
	tmp.r.open = g->r.open;
	if (mpf_sgn(g->r.f) <= 0) {
		mpf_set(a->l.f, tmp.r.f);
		a->l.inf = tmp.r.inf;
		a->l.open = tmp.r.open;
		mpf_set(a->r.f, tmp.l.f);
		a->r.inf = tmp.l.inf;
		a->r.open = tmp.l.open;
	} else if (mpf_sgn(g->l.f) <= 0) {
		mpf_set_si(a->l.f, 0);
		a->l.inf = 0;
		a->l.open = 0;
		if (mpf_cmp(tmp.l.f, tmp.r.f) < 0) {
			mpf_set(a->r.f, tmp.r.f);
			a->r.inf = tmp.r.inf;
			a->r.open = tmp.r.open;
		} else if (mpf_cmp(tmp.l.f, tmp.r.f) > 0) {
			mpf_set(a->r.f, tmp.l.f);
			a->r.inf = tmp.l.inf;
			a->r.open = tmp.l.open;
		} else {
			mpf_set(a->r.f, tmp.r.f);
			a->r.inf = tmp.r.inf;
			a->r.open = tmp.l.open & tmp.r.open;
		}
	} else {
		mpf_set(a->l.f, tmp.l.f);
		a->l.inf = tmp.l.inf;
		a->l.open = tmp.l.open;
		mpf_set(a->r.f, tmp.r.f);
		a->r.inf = tmp.r.inf;
		a->r.open = tmp.r.open;
	}

	gbnd_clear(&tmp);
}

void ming(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	/* If any value is NaN, the result is also NaN. */
	if (x->nan || y->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,ming,y,gb);
		return;
	}
	a->nan = 0;

	if (cmp_gn(&x->l, LE, &y->l, LE) < 0) {
		mpf_set(a->l.f, x->l.f);
		a->l.inf = x->l.inf;
		a->l.open = x->l.open;
	} else {
		mpf_set(a->l.f, y->l.f);
		a->l.inf = y->l.inf;
		a->l.open = y->l.open;
	}
	if (cmp_gn(&x->r, RE, &y->r, RE) < 0) {
		mpf_set(a->r.f, x->r.f);
		a->r.inf = x->r.inf;
		a->r.open = x->r.open;
	} else {
		mpf_set(a->r.f, y->r.f);
		a->r.inf = y->r.inf;
		a->r.open = y->r.open;
	}
}

void maxg(gbnd_s *a, const gbnd_s *x, const gbnd_s *y)
{
	/* If any value is NaN, the result is also NaN. */
	if (x->nan || y->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,x,gb,maxg,y,gb);
		return;
	}
	a->nan = 0;

	if (cmp_gn(&x->l, LE, &y->l, LE) > 0) {
		mpf_set(a->l.f, x->l.f);
		a->l.inf = x->l.inf;
		a->l.open = x->l.open;
	} else {
		mpf_set(a->l.f, y->l.f);
		a->l.inf = y->l.inf;
		a->l.open = y->l.open;
	}
	if (cmp_gn(&x->r, RE, &y->r, RE) > 0) {
		mpf_set(a->r.f, x->r.f);
		a->r.inf = x->r.inf;
		a->r.open = x->r.open;
	} else {
		mpf_set(a->r.f, y->r.f);
		a->r.inf = y->r.inf;
		a->r.open = y->r.open;
	}
}

int cliplg(gbnd_s *a, const gbnd_s *g, const gbnd_s *h)
{
	int res = 0;

	/* If any value is NaN, the result is also NaN. */
	if (g->nan || h->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,g,gb,cliplg,h,gb);
		return 0;
	}
	a->nan = 0;

	if (cmp_gn(&g->l, LE, &h->l, LE) < 0) {
		mpf_set(a->l.f, h->l.f);
		a->l.inf = h->l.inf;
		a->l.open = h->l.open;
		res = 1;
	} else {
		mpf_set(a->l.f, g->l.f);
		a->l.inf = g->l.inf;
		a->l.open = g->l.open;
	}
	if (cmp_gn(&g->r, RE, &h->r, RE) < 0) {
		mpf_set(a->r.f, h->r.f);
		a->r.inf = h->r.inf;
		a->r.open = h->r.open;
		res = 1;
	} else {
		mpf_set(a->r.f, g->r.f);
		a->r.inf = g->r.inf;
		a->r.open = g->r.open;
	}
	return res;
}

int cliphg(gbnd_s *a, const gbnd_s *g, const gbnd_s *h)
{
	int res = 0;

	/* If any value is NaN, the result is also NaN. */
	if (g->nan || h->nan) {
		a->nan = 1;
		mpf_set_si(a->l.f, 0); mpf_set_si(a->r.f, 0);
		a->l.inf = a->r.inf = 0;
		a->l.open = a->r.open = 1;
		AOP2("NaN",a,gb,g,gb,cliphg,h,gb);
		return 0;
	}
	a->nan = 0;

	if (cmp_gn(&g->l, LE, &h->l, LE) > 0) {
		mpf_set(a->l.f, h->l.f);
		a->l.inf = h->l.inf;
		a->l.open = h->l.open;
		res = 1;
	} else {
		mpf_set(a->l.f, g->l.f);
		a->l.inf = g->l.inf;
		a->l.open = g->l.open;
	}
	if (cmp_gn(&g->r, RE, &h->r, RE) > 0) {
		mpf_set(a->r.f, h->r.f);
		a->r.inf = h->r.inf;
		a->r.open = h->r.open;
		res = 1;
	} else {
		mpf_set(a->r.f, g->r.f);
		a->r.inf = g->r.inf;
		a->r.open = g->r.open;
	}
	return res;
}
