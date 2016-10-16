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

#include <stddef.h> /* NULL */
#include <stdio.h> /* printf */
#include <string.h> /* strpbrk */
#include <ctype.h> /* isspace */

#include "gmp.h" /* gmp_printf, gmp_scanf */
#include "hlayer.h"
#include "uenv.h"
#include "support.h"
#include "conv.h"
#include "ubnd.h"

#if defined(ROUND)
#define G2U(ub,gb) g2ur(ub,gb)
#else
#define G2U(ub,gb) g2u(ub,gb)
#endif

#define INF "Inf"
#define NAN "NaN"
#define FMT_INP "%Fg"
/* FIXME: output precision is limited for now */
//#define FMT_OUT "%.24Fg"
#define FMT_OUT "%.Fg"

/* * * * * * * * * * scan * * * * * * * * * */

void scan_un(unum_s *un)
{
	fscan_un(stdin, un);
}

void scan_ub(ubnd_s *ub)
{
	fscan_ub(stdin, ub);
}

void scan_gb(gbnd_s *gb)
{
	fscan_gb(stdin, gb);
}

void fscan_un(FILE *fp, unum_s *un)
{
	MPX_VAR(u2);
	ubnd_s ub = {0, un, u2};

	fscan_ub(fp, &ub);
	if (ub.p) { /* g2u() doesn't unify if there is any drop in precision. */
		unify(&ub, &ub);
		if (ub.p) {
			mpx_set(un, qNaNu);
			AOP0("can't unify",un,un);
		}
	}
}

void fscan_ub(FILE *fp, ubnd_s *ub)
{
	gbnd_s gb;

	gbnd_init(&gb);
	fscan_gb(fp, &gb);
	G2U(ub, &gb);
	gbnd_clear(&gb);
}

static int fscan_gn(FILE *fp, gnum_s *gn)
{
	int res = 0;
	int neg = 0;
	int c;

	while (isspace(c = getc(fp)));
	if (c == '+' || c == '-') {
		neg = c == '-';
		c = getc(fp);
	}
	if (toupper(c) == 'I') {
		if (toupper(c = getc(fp)) == 'N') {
			if (toupper(c = getc(fp)) == 'F') {
				res = 1;
				mpf_set_si(gn->f, 1);
				gn->inf = 1;
			} else ungetc(c, fp);
		} else ungetc(c, fp);
	} else {
		ungetc(c, fp);
		res = gmp_fscanf(fp, FMT_INP, gn->f);
		gn->inf = 0;
	}
	if (res == 1 && neg) mpf_neg(gn->f, gn->f);
	return(res);
}

void fscan_gb(FILE *fp, gbnd_s *gb)
{
	int resL = 0;
	int resR = 0;
	int c;

	while (isspace(c = getc(fp)));
	if (c == '(' || c == '[') { /* bound */
		gb->l.open = c == '(';
		resL = fscan_gn(fp, &gb->l);
		while (isspace(c = getc(fp)));
		if (c == ',') {
			resR = fscan_gn(fp, &gb->r);
			while (isspace(c = getc(fp)));
			if (c == ')' || c == ']') {
				gb->r.open = c == ')';
			} else {
				ungetc(c, fp);
				resR = 0;
			}
		} else ungetc(c, fp);
	} else { /* exact */
		ungetc(c, fp);
		resL = resR = fscan_gn(fp, &gb->l);
		mpf_set(gb->r.f, gb->l.f);
		gb->r.inf = gb->l.inf;
		gb->l.open = gb->r.open = 0;
	}
	if (resL != 1 || resR != 1) { /* NaN */
		gb->nan = 1;
		mpf_set_si(gb->l.f, 0); mpf_set_si(gb->r.f, 0);
		gb->l.inf = gb->r.inf = 0;
		gb->l.open = gb->r.open = 1;
		AOP0("conversion error",gb,gb);
	}
}

void sscan_un(const char *str, unum_s *un)
{
	MPX_VAR(u2);
	ubnd_s ub = {0, un, u2};

	sscan_ub(str, &ub);
	if (ub.p) { /* g2u() doesn't unify if there is any drop in precision. */
		unify(&ub, &ub);
		if (ub.p) {
			mpx_set(un, qNaNu);
			AOP0("can't unify",un,un);
		}
	}
}

void sscan_ub(const char *str, ubnd_s *ub)
{
	gbnd_s gb;

	gbnd_init(&gb);
	sscan_gb(str, &gb);
	G2U(ub, &gb);
	gbnd_clear(&gb);
}

static int sscan_gn(const char *str, gnum_s *gn)
{
	char *cp, *bC;
	int res;

	if ((cp = strstr(str, INF)) != NULL &&
	    ((bC = strchr(str, ',')) == NULL || bC > cp)) {
		res = 1;
		mpf_set_si(gn->f, (cp > str && cp[-1] == '-') ? -1 : 1);
		gn->inf = 1;
	} else {
		res = gmp_sscanf(str, FMT_INP, gn->f);
		gn->inf = 0;
	}
	return res;
}

void sscan_gb(const char *str, gbnd_s *gb)
{
	char *bL, *bC, *bR;

	if (strstr(str, NAN) != NULL) { /* NaN */
		gb->nan = 1;
		mpf_set_si(gb->l.f, 0); mpf_set_si(gb->r.f, 0);
		gb->l.inf = gb->r.inf = 0;
		gb->l.open = gb->r.open = 1;
		AOP0("read NaN",gb,gb);
		return;
	}
	bL = strpbrk(str, "([");
	bC = strchr(str, ',');
	bR = strpbrk(str, ")]");
	if (bL != NULL && bC != NULL && bR != NULL) { /* bound */
		int resL = sscan_gn(bL+1, &gb->l);
		int resR = sscan_gn(bC+1, &gb->r);
		gb->nan = resL != 1 || resR != 1;
		gb->l.open = *bL == '(';
		gb->r.open = *bR == ')';
	} else { /* exact */
		int res = sscan_gn(str, &gb->l);
		mpf_set(gb->r.f, gb->l.f);
		gb->r.inf = gb->l.inf;
		gb->l.open = gb->r.open = 0;
		gb->nan = res != 1;
	}
	if (gb->nan) {
		AOP0("conversion error",gb,gb);
	}
}

/* * * * * * * * * * print * * * * * * * * * */

void print_un(const unum_s *un)
{
	fprint_un(stdout, un);
}

void print_ub(const ubnd_s *ub)
{
	fprint_ub(stdout, ub);
}

void print_gb(const gbnd_s *gb)
{
	fprint_gb(stdout, gb);
}

void fprint_un(FILE *fp, const unum_s *un)
{
	gbnd_s gb;

	gbnd_init(&gb);
	unum2g(&gb, un);
	fprint_gb(fp, &gb);
	gbnd_clear(&gb);
}

void fprint_ub(FILE *fp, const ubnd_s *ub)
{
	gbnd_s gb;

	gbnd_init(&gb);
	u2g(&gb, ub); 
	fprint_gb(fp, &gb);
	gbnd_clear(&gb);
}

/* View a float as a decimal, using as many digits as needed to be exact. */
/* autoN in the prototype */

static void fprint_gn(FILE *fp, const gnum_s *gn)
{
	if (gn->inf) { /* infinite */
		if (mpf_sgn(gn->f) < 0) fputc('-', fp);
		fputs(INF, fp);
	} else {
		gmp_fprintf(fp, FMT_OUT, gn->f);
	}
}

void fprint_gb(FILE *fp, const gbnd_s *gb)
{
	if (gb->nan) { /* NaN */
		fputs(NAN, fp);
	} else if (gb->l.open || gb->r.open || mpf_cmp(gb->l.f,gb->r.f) != 0) { /* bound */
		fputc((gb->l.open) ? '(' : '[', fp);
		fprint_gn(fp, &gb->l);
		fputc(',', fp);
		fprint_gn(fp, &gb->r);
		fputc((gb->r.open) ? ')' : ']', fp);
	} else { /* exact */
		fprint_gn(fp, &gb->l);
	}
}

void sprint_un(char *str, const unum_s *un)
{
	gbnd_s gb;

	gbnd_init(&gb);
	unum2g(&gb, un);
	sprint_gb(str, &gb);
	gbnd_clear(&gb);
}

void sprint_ub(char *str, const ubnd_s *ub)
{
	gbnd_s gb;

	gbnd_init(&gb);
	u2g(&gb, ub);
	sprint_gb(str, &gb);
	gbnd_clear(&gb);
}

#define sputc(c,str) (*(str)++ = (c))
#define sputs(s,str) do {strcpy(str,s); (str) += strlen(s);} while (0)

static void sprint_gn(char *str, const gnum_s *gn)
{
	if (gn->inf) { /* infinite */
		if (mpf_sgn(gn->f) < 0) sputc('-', str);
		sputs(INF, str);
		*str = '\0';
	} else {
		gmp_sprintf(str, FMT_OUT, gn->f);
	}
}

void sprint_gb(char *str, const gbnd_s *gb)
{
	if (gb->nan) { /* NaN */
		sputs(NAN, str);
	} else if (gb->l.open || gb->r.open || mpf_cmp(gb->l.f,gb->r.f) != 0) { /* bound */
		sputc((gb->l.open) ? '(' : '[', str);
		sprint_gn(str, &gb->l); str += strlen(str);
		sputc(',', str);
		sprint_gn(str, &gb->r); str += strlen(str);
		sputc((gb->r.open) ? ')' : ']', str);
		*str = '\0';
	} else { /* exact */
		sprint_gn(str, &gb->l); str += strlen(str);
	}
}

/* * * * * * * * * * view * * * * * * * * * */

#define ANSI_RESET         "\x1b[0m"
#define ANSI_BOLD          "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"

// #define COLOR_BRIGHTBLUE "\x1b[38;2;63;127;255m"
// #define COLOR_SANEGREEN "\x1b[38;2;0;191;0m"

void uview_un(const unum_s *u)
{
	unsigned long pos;
	unsigned long fcnt;
	utag_s ut;

	utag(&ut, u);
	pos = 1 + ut.esize + ut.fsize + utagsize;
	/* sign */
	printf(ANSI_BOLD);
	printf(ANSI_COLOR_RED);
	putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	putchar(' ');
	/* exponent */
	printf(ANSI_COLOR_CYAN);
	for (fcnt = ut.esize; fcnt; --fcnt) {
		putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	}
	putchar(' ');
	/* fraction */
	printf(ANSI_COLOR_YELLOW);
	for (fcnt = ut.fsize; fcnt; --fcnt) {
		putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	}
	putchar(' ');
	printf(ANSI_RESET);
	/* ubit */
	printf(ANSI_COLOR_MAGENTA);
	putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	putchar(' ');
	/* esizesize */
	printf(ANSI_COLOR_CYAN);
	for (fcnt = esizesize; fcnt; --fcnt) {
		putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	}
	putchar(' ');
	/* fsizesize */
	printf(ANSI_COLOR_YELLOW);
	for (fcnt = fsizesize; fcnt; --fcnt) {
		putchar(mpx_tstbit(u, --pos) ? '1' : '0');
	}
	printf(ANSI_RESET);
}

void uview_ub(const ubnd_s *ub)
{
	if (!ub->p) { /* single unum */
		uview_un(ub->l);
	} else { /* pair of unums */
		printf("%c", '|');
		uview_un(ub->l);
		putchar(',');
		uview_un(ub->r);
		printf("%c", '|');
	}
}

void uview_gb(const gbnd_s *gb)
{
	MPX_VAR(u1);
	MPX_VAR(u2);
	ubnd_s ub = {1, u1, u2};
	g2u(&ub, gb);
	uview_ub(&ub);
}

void view_uenv(void)
{
	if (MPX_PTR(one) == NULL) return;

#define SHOWI(i) printf("%-15s: %d\n", #i, i);
#define SHOWL(i) printf("%-15s: %ld\n", #i, i);
	printf("\n# int sizes #\n");
	SHOWI(esizesize);
	SHOWI(fsizesize);
	SHOWI(esizemax);
	SHOWI(fsizemax);
	SHOWI(utagsize);
	SHOWI(maxubits);
	SHOWL(ulimbs); /* long int */

#define SHOWM(m) gmp_printf("%-15s: 0x%Nx\n", #m, MPX_PTR(m), ulimbs);
	printf("\n# unum masks #\n");
	SHOWM(one);
	SHOWM(fsizemask);
	SHOWM(esizemask);
	SHOWM(ubitmask);
	SHOWM(efsizemask);
	SHOWM(utagmask);
	SHOWM(ulpu);
	SHOWM(signbigu);

#define SHOWU(u) printf("%-15s: ", #u); uview_un(u); printf(": "); print_un(u); putchar('\n');
	printf("\n# unum extremes #\n");
	SHOWU(smallsubnormalu);
	SHOWU(smallnormalu);
	SHOWU(posinfu);
	SHOWU(maxrealu);
	SHOWU(minrealu);
	SHOWU(neginfu);
	SHOWU(negbigu);
	SHOWU(qNaNu);
	SHOWU(sNaNu);
	SHOWU(negopeninfu);
	SHOWU(posopeninfu);
	SHOWU(negopenzerou);

#define SHOWF(f) gmp_printf("%-15s: %Fe\n", #f, f);
	printf("\n# mp float extremes #\n");
	SHOWF(maxreal);
	SHOWF(smallsubnormal);
}

void print_stats(void)
{
	printf("ubits moved  : %lld\n", stats.ubitsmoved);
	printf("ubnds moved  : %lld\n", stats.ubndsmoved);
	if (stats.ubndsmoved) {
		printf("avg bits/num : %f\n", (double)stats.ubitsmoved/stats.ubndsmoved);
	}
	printf("max unum bits: %d\n", UBITS);
	printf("max ubnd bits: %d\n", 2*UBITS+1);
	{
		int a, u, v;
		char *pcl[] = {"exac", "inex", "pair"};
		for (a = 0; a < 3; a++) {
			long long int subtotal = 0;
			for (u = 0; u < 3; u++) {
				subtotal += stats.opc2[a][u];
				if (stats.opc2[a][u])
					printf("%s = oper(%s)  : %4.1f%% %lld\n",
						pcl[a], pcl[u],
						(double)stats.opc2[a][u]/stats.ops*100,
						stats.opc2[a][u]);
			}
			for (u = 0; u < 3; u++) {
				for (v = 0; v < 3; v++) {
					subtotal += stats.opc3[a][u][v];
					if (stats.opc3[a][u][v])
						printf("%s = %s o %s : %4.1f%% %lld\n",
							pcl[a], pcl[u], pcl[v],
							(double)stats.opc3[a][u][v]/stats.ops*100,
							stats.opc3[a][u][v]);
				}
			}
			printf("%s sub total---- : %4.1f%% %lld\n", pcl[a],
				(double)subtotal/stats.ops*100, subtotal);
		}
		printf("total ops--------- :       %lld\n", stats.ops);
	}
#if defined(NBITS_HISTO)
	{
		int i;
		int ubbits = 2*UBITS+1;
		int end = (ubbits < MAX_NBITS) ? ubbits : MAX_NBITS;
		if (ubbits > MAX_NBITS) printf(" -- warning: not showing bins beyond %d\n", end);
		printf("nbits histogram:\n");
		for (i = 4; i <= end; i++) {
			printf("%3d %lld\n", i, stats.nbits[i]);
		}
	}
#endif
}
