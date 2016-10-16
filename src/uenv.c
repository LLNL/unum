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

#include <stdio.h> /* fprintf */
#include <stdlib.h> /* exit */

#include "uenv.h"

int esizesize;
int fsizesize;
int esizemax;
int fsizemax;
int utagsize;
int maxubits;

mp_size_t ulimbs;
mp_size_t plimbs;
mp_bitcnt_t uprec;

mpx_t one;
mpx_t ubitmask;
mpx_t fsizemask;
mpx_t esizemask;
mpx_t efsizemask;
mpx_t utagmask;
mpx_t ulpu;
mpx_t smallsubnormalu;
mpx_t smallnormalu;
mpx_t signbigu;
mpx_t posinfu;
mpx_t maxrealu;
mpx_t minrealu;
mpx_t neginfu;
mpx_t negbigu;
mpx_t qNaNu;
mpx_t sNaNu;
mpx_t negopeninfu;
mpx_t posopeninfu;
mpx_t negopenzerou;

mpf_t maxreal;
mpf_t smallsubnormal;

static int initialized;


void init_uenv(void)
{
	mpx_init2(one, MAX_UBITS);
	mpx_init2(ubitmask, MAX_UBITS);
	mpx_init2(fsizemask, MAX_UBITS);
	mpx_init2(esizemask, MAX_UBITS);
	mpx_init2(efsizemask, MAX_UBITS);
	mpx_init2(utagmask, MAX_UBITS);
	mpx_init2(ulpu, MAX_UBITS);
	mpx_init2(smallsubnormalu, MAX_UBITS);
	mpx_init2(smallnormalu, MAX_UBITS);
	mpx_init2(signbigu, MAX_UBITS);
	mpx_init2(posinfu, MAX_UBITS);
	mpx_init2(maxrealu, MAX_UBITS);
	mpx_init2(minrealu, MAX_UBITS);
	mpx_init2(neginfu, MAX_UBITS);
	mpx_init2(negbigu, MAX_UBITS);
	mpx_init2(qNaNu, MAX_UBITS);
	mpx_init2(sNaNu, MAX_UBITS);
	mpx_init2(negopeninfu, MAX_UBITS);
	mpx_init2(posopeninfu, MAX_UBITS);
	mpx_init2(negopenzerou, MAX_UBITS);

	mpf_init2(maxreal, MAX_UPREC);
	mpf_init2(smallsubnormal, MAX_UPREC);

	initialized = 1;
}

void clear_uenv(void)
{
	mpx_clear(one);
	mpx_clear(ubitmask);
	mpx_clear(fsizemask);
	mpx_clear(esizemask);
	mpx_clear(efsizemask);
	mpx_clear(utagmask);
	mpx_clear(ulpu);
	mpx_clear(smallsubnormalu);
	mpx_clear(smallnormalu);
	mpx_clear(signbigu);
	mpx_clear(posinfu);
	mpx_clear(maxrealu);
	mpx_clear(minrealu);
	mpx_clear(neginfu);
	mpx_clear(negbigu);
	mpx_clear(qNaNu);
	mpx_clear(sNaNu);
	mpx_clear(negopeninfu);
	mpx_clear(posopeninfu);
	mpx_clear(negopenzerou);

	mpf_clear(maxreal);
	mpf_clear(smallsubnormal);

	initialized = 0;
}

/* Set the environment variables based on esizesize and fsizesize.
   Here, maximum esizesize is MAX_ESIZESIZE
   and maximum fsizesize is MAX_FSIZESIZE. */

void set_uenv(int e, int f)
{
	if (!initialized) init_uenv();
	if (e < 0 || e > MAX_ESIZESIZE || f < 0 || f > MAX_FSIZESIZE) {
		fprintf(stderr, " -- error: exponent size size or fraction size size out of range\n");
		fprintf(stderr, " -- max exponent size size: %u\n", MAX_ESIZESIZE);
		fprintf(stderr, " -- max fraction size size: %u\n", MAX_FSIZESIZE);
		exit(EXIT_FAILURE);
	}

	/* integer type */
	esizesize = e;
	fsizesize = f;
	esizemax = 1 << e;
	fsizemax = 1 << f;
	utagsize = 1 + e + f;
	maxubits = 1 + esizemax + fsizemax + utagsize;
	ulimbs = MPX_BITS2LIMBS(maxubits);
	uprec = FSIZE2PREC(fsizemax);
	plimbs = MPF_BITS2LIMBS(uprec);

	/* unum type */
	mpx_set_ui(one, 1);
	mpx_lshift(ubitmask, one, utagsize-1);
	mpx_lshift(fsizemask, one, f);
		mpx_sub_ui(fsizemask, fsizemask, 1);
	mpx_sub_ui(esizemask, ubitmask, 1);
		mpx_sub(esizemask, esizemask, fsizemask);
	mpx_ior(efsizemask, esizemask, fsizemask);
	mpx_ior(utagmask, ubitmask, efsizemask);
	mpx_lshift(ulpu, one, utagsize);
	mpx_add(smallsubnormalu, efsizemask, ulpu);
	/* Proto difference: smallnormalu is in reduced form here. */
	mpx_lshift(smallnormalu, one, utagsize+1);
		mpx_ior(smallnormalu, smallnormalu, esizemask);
	mpx_lshift(signbigu, one, maxubits-1);
	mpx_sub_ui(posinfu, signbigu, 1);
		mpx_sub(posinfu, posinfu, ubitmask);
	mpx_sub(maxrealu, posinfu, ulpu);
	mpx_add(minrealu, maxrealu, signbigu);
	mpx_add(neginfu, posinfu, signbigu);
	mpx_sub(negbigu, neginfu, ulpu);
	mpx_add(qNaNu, posinfu, ubitmask);
	mpx_add(sNaNu, neginfu, ubitmask);
	if (utagsize == 1) {
		mpx_set_ui(negopeninfu, 0xD);
	} else {
		mpx_set_ui(negopeninfu, 0xF);
		mpx_lshift(negopeninfu, negopeninfu, utagsize-1);
	}
	if (utagsize == 1) {
		mpx_set_ui(posopeninfu, 0x5);
	} else {
		mpx_set_ui(posopeninfu, 0x7);
		mpx_lshift(posopeninfu, posopeninfu, utagsize-1);
	}
	mpx_set_ui(negopenzerou, 0x9);
		mpx_lshift(negopenzerou, negopenzerou, utagsize-1);

	/* float type */
	mpf_set_ui(maxreal, 1);
		mpf_mul_2exp(maxreal, maxreal, fsizemax);
		mpf_sub_ui(maxreal, maxreal, 1);
		mpf_div_2exp(maxreal, maxreal, fsizemax-1);
		mpf_mul_2exp(maxreal, maxreal, 1UL << (esizemax-1));
	mpf_set_ui(smallsubnormal, 1);
		mpf_div_2exp(smallsubnormal, smallsubnormal, (1UL << (esizemax-1))+fsizemax-2);
}
