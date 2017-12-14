/*
$Id: tcomparison.c $

Description: Test file

$Log: $
*/

#include <stdio.h> /* printf, putchar */
#include <stdlib.h> /* exit, atoi, malloc, realloc, free, rand */
#include <string.h> /* strchr, strlen, strcmp, memcpy */
#include <ctype.h> /* isdigit */
#include <errno.h> /* errno */
//#define NDEBUG 1
//#include <assert.h> /* assert */

#include "unum.h"
#include "gbnd.h"

#define PROG "tbasic"
#ifndef VERSION
#define VERSION "1.0"
#endif

#define DEFAULT_INT 1
#define DEFAULT_STR "ABC"

#define BFLAG 0x01

#define VFLAG 0x1000

int flags; /* argument flags */
int iarg = DEFAULT_INT; /* int argument */
char *sarg = DEFAULT_STR; /* string argument */


int num_same(const char *a, const char *ae, const char *b, const char *be, size_t length)
{
	/* mantissa */
	while (a < ae && b < be && length) {
		if (*a != *b) return 0;
		if (isdigit(*a)) length--;
		a++; b++;
	}
	/* exponent */
	if ((a = strpbrk(a, "Ee")) != NULL && a < ae) {
		if ((b = strpbrk(b, "Ee")) != NULL && b < be) {
			a++; b++;
			while (a < ae && b < be) {
				if (*a != *b) return 0;
				a++; b++;
			}
		} else return 0;
	}
	return 1;
}

int ustr_same(const char *a, const char *b, size_t length)
{
	char *aL = strpbrk(a, "([");
	char *aC = strchr(a, ',');
	char *aR = strpbrk(a, ")]");
	char *bL = strpbrk(b, "([");
	char *bC = strchr(b, ',');
	char *bR = strpbrk(b, ")]");
	if (aL != NULL && aC != NULL && aR != NULL) { /* bound */
		if (bL != NULL && bC != NULL && bR != NULL) {
			if (*aL == *bL && *aC == *bC && *aR == *bR) {
				return
					num_same(aL+1, aC, bL+1, bC, length) &&
					num_same(aC+1, aR, bC+1, bR, length);
			}
		}
	} else { /* exact */
		return
			num_same(a, strchr(a, '\0'), b, strchr(a, '\0'), length);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	int tfail = 0;

/*----------------------*/
/*-------- ubnd --------*/
/*----------------------*/

#if 1
	unum_set_env(3, 4);
	{
		signed long si1, si2;
		unsigned long ui1, ui2;
		double d1, d2;
		char str1[64], str2[64];
		UBND_VAR(ub);
		int ess, fss;
		int ok, fail = 0;

		unum_get_env(&ess, &fss);
		printf("\n# test ubnd conversion, env:%d,%d #\n", ess, fss);

#define UBND_STR(val,chk) \
		strcpy(str1, val); ubnd_set_str(ub, str1); ubnd_get_str(str2, ub); \
		fail |= !(ok = strcmp(str2, chk) == 0); \
		printf("%s str %s : %s\n", ok ? "OK  " : "FAIL", str1, str2)

		UBND_STR("(1,Inf)","(1,Inf)");
        UBND_STR("(-Inf,-1)","(-Inf,-1)");
		tfail |= fail;
	}
#endif

#if 1
    unum_set_env(3, 4);
    {
        UBND_VAR(ub1);
        UBND_VAR(ub2);
        UBND_VAR(ubr);
        UBND_VAR(ubc);
        int ess, fss;
        int ir, ok, fail = 0;

        unum_get_env(&ess, &fss);
        printf("\n# test ubnd operations, env:%d,%d #\n", ess, fss);

#define UBND_ROP(op1,oper,op2,chk) \
		ubnd_set_str(ub1, op1); \
		ubnd_set_str(ub2, op2); \
		fail |= !(ok = ((ir = ubnd_##oper(ub1, ub2)) && chk) || (!ir && !chk)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		ubnd_print(ub1); printf(" %s ", #oper); ubnd_print(ub2); \
		printf(" : %s\n", ir ? "true" : "false")
        UBND_ROP("(1,Inf)",gt,"(2,Inf)",0);
        UBND_ROP("(2,Inf)",lt,"(1,Inf)",0);
        UBND_ROP("(-Inf,1)",gt,"(-Inf,-1)",0);
        UBND_ROP("(-Inf,-1)",lt,"(-Inf,1)",0);
        tfail |= fail;
    }
#endif
	unum_clear_env();
	printf("\ntest %s\n", tfail ? "FAIL" : "OK");
	return(tfail ? EXIT_FAILURE : EXIT_SUCCESS);
}
