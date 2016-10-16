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

#ifndef HLAYER_H_
#define HLAYER_H_

#include <stdio.h> /* FILE* */
#include <stdlib.h> /* abort */

#include "ulayer.h"
#include "glayer.h"

#if defined(DEBUG)

#define AOP0(str,res,tpr) do { \
	printf("%s: %s, %s, line %d.\n", str,__FILE__,__func__,__LINE__); \
	print_##tpr(res); putchar('\n'); \
	abort(); } while (0)

#define AOP1(str,res,tpr,oper,op1,tp1) do { \
	printf("%s: %s, %s, line %d.\n", str,__FILE__,__func__,__LINE__); \
	print_##tpr(res); printf(" = %s ", #oper); \
	print_##tp1(op1); putchar('\n'); \
	abort(); } while (0)

#define AOP2(str,res,tpr,op1,tp1,oper,op2,tp2) do { \
	printf("%s: %s, %s, line %d.\n", str,__FILE__,__func__,__LINE__); \
	print_##tpr(res); printf(" = "); \
	print_##tp1(op1); printf(" %s ", #oper); \
	print_##tp2(op2); putchar('\n'); \
	abort(); } while (0)

#else /* !DEBUG */
#define AOP0(str,res,tpr)
#define AOP1(str,res,tpr,oper,op1,tp1)
#define AOP2(str,res,tpr,op1,tp1,oper,op2,tp2)
#endif /* DEBUG */

#if defined(__cplusplus)
extern "C" {
#endif

/* NOTE: Need the spectrum of scanf & printf, vprintf, fprintf, vfprintf... */
/* Put these in unum.c? */

/* Single number */

void scan_un(unum_s *un);
void scan_ub(ubnd_s *ub);
void scan_gb(gbnd_s *gb);

void fscan_un(FILE *fp, unum_s *un);
void fscan_ub(FILE *fp, ubnd_s *ub);
void fscan_gb(FILE *fp, gbnd_s *gb);

void sscan_un(const char *str, unum_s *un);
void sscan_ub(const char *str, ubnd_s *ub);
void sscan_gb(const char *str, gbnd_s *gb);

/* TODO: specify precision (digits right of decimal)
   and conversion format (e, f, or g).
   Use global vars, function, or args? */

void print_un(const unum_s *un);
void print_ub(const ubnd_s *ub);
void print_gb(const gbnd_s *gb);

void fprint_un(FILE *fp, const unum_s *un);
void fprint_ub(FILE *fp, const ubnd_s *ub);
void fprint_gb(FILE *fp, const gbnd_s *gb);

void sprint_un(char *str, const unum_s *un);
void sprint_ub(char *str, const ubnd_s *ub);
void sprint_gb(char *str, const gbnd_s *gb);

void uview_un(const unum_s *un);
void uview_ub(const ubnd_s *ub);
void uview_gb(const gbnd_s *gb);

void view_uenv(void);

void print_stats(void);

#if defined (__cplusplus)
}
#endif

#endif /* HLAYER_H_ */
