/*
$Id: tbasic.c $

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
	int nok = 0;
	char *s;
	int tfail = 0;

	while (--argc > 0 && (*++argv)[0] == '-')
		for (s = argv[0]+1; *s; s++)
			switch (*s) {
			case 'b':
				flags |= BFLAG;
				break;
			case 'i':
				if (isdigit((int)s[1])) iarg = atoi(s+1);
				else nok = 1;
				s += strlen(s+1);
				break;
			case 's':
				sarg = s+1;
				s += strlen(s+1);
				break;
			case 'v':
				flags |= VFLAG;
				break;
			default:
				nok = 1;
				fprintf(stderr, " -- not an option: %c\n", *s);
				break;
			}

	if (flags & VFLAG) fprintf(stderr, "%s %s\n", PROG, VERSION);
	if (nok /*|| argc < 1*/ || (argc > 0 && *argv[0] == '?')) {
		fprintf(stderr, "Usage: %s -bv -i<int> -s<str> <in_file> [<out_file>]\n", PROG);
		fprintf(stderr, "  -b  boolean argument\n");
		fprintf(stderr, "  -i  integer argument, default: %d\n", DEFAULT_INT);
		fprintf(stderr, "  -s  string argument, default: %s\n", DEFAULT_STR);
		fprintf(stderr, "  -v  version\n");
		exit(EXIT_FAILURE);
	}

#if 0
	{
		FILE *fin, *fout;

		if ((fin = fopen(argv[0], "r")) == NULL) {
			fprintf(stderr, " -- can't open file: %s\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		if (argc < 2) {
			fout = stdout;
		} else if ((fout = fopen(argv[1], "w")) == NULL) {
			fprintf(stderr, " -- can't open file: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}

		/* do something */

		fclose(fin);
		fclose(fout);
	}
#endif

/*----------------------*/
/*-------- unum --------*/
/*----------------------*/

#if 1
	unum_set_env(3, 4);
	{
		signed long si1, si2;
		unsigned long ui1, ui2;
		double d1, d2;
		char str1[64], str2[64];
		UNUM_VAR(un);
		int ess, fss;
		int ok, fail = 0;

		unum_get_env(&ess, &fss);
		printf("\n# test unum conversions, env:%d,%d #\n", ess, fss);

#define UNUM_SI(val,chk) \
		si1 = val; unum_set_si(un, si1); si2 = unum_get_si(un); \
		fail |= !(ok = si2 == chk); \
		printf("%s si %ld : %ld\n", ok ? "OK  " : "FAIL", si1, si2)
		UNUM_SI(-31, -31);

#define UNUM_UI(val,chk) \
		ui1 = val; unum_set_ui(un, ui1); ui2 = unum_get_ui(un); \
		fail |= !(ok = ui2 == chk); \
		printf("%s ui %lu : %lu\n", ok ? "OK  " : "FAIL", ui1, ui2)
		UNUM_UI(31, 31);

#define UNUM_D(val,chk) \
		d1 = val; unum_set_d(un, d1); d2 = unum_get_d(un); \
		fail |= !(ok = d2 == chk); \
		printf("%s d %e : %e\n", ok ? "OK  " : "FAIL", d1, d2)
		UNUM_D(6.0e5, 6.0e5);

#define UNUM_STR(val,chk) \
		strcpy(str1, val); unum_set_str(un, str1); unum_get_str(str2, un); \
		fail |= !(ok = strcmp(str2, chk) == 0); \
		printf("%s str %s : %s\n", ok ? "OK  " : "FAIL", str1, str2)
		UNUM_STR("(-0.25,-0.125)", "(-0.25,-0.125)");
		UNUM_STR("(6,8)", "(6,8)");
		UNUM_STR("(23.1,23.9)", "(23,24)");
		UNUM_STR("(-Inf,-4]", "(-Inf,-3.999969482421875)");
		UNUM_STR("(-Inf,-3]", "NaN");
		UNUM_STR("(25,32.5)", "NaN");
		/* TODO: check if "(25,32.5)" will unify */

		tfail |= fail;
	}
#endif

#if 1
	unum_set_env(3, 4);
	{
		UNUM_VAR(u1);
		UNUM_VAR(u2);
		UNUM_VAR(ur);
		UNUM_VAR(uc);
		int ess, fss;
		int ir, ok, fail = 0;

		unum_get_env(&ess, &fss);
		printf("\n# test unum operations, env:%d,%d #\n", ess, fss);

#define UNUM_ROP(op1,oper,op2,chk) \
		unum_set_str(u1, op1); \
		unum_set_str(u2, op2); \
		fail |= !(ok = ((ir = unum_##oper(u1, u2)) && chk) || (!ir && !chk)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		unum_print(u1); printf(" %s ", #oper); unum_print(u2); \
		printf(" : %s\n", ir ? "true" : "false")
		UNUM_ROP("(6,8)", lt, "(8,10)", 1);
		UNUM_ROP("(1,2)", lt, "2", 1);
		UNUM_ROP("(24,25)", lt, "(24,32)", 0);
		UNUM_ROP("-2", lt, "-1", 1);
		UNUM_ROP("(4,5)", gt, "(3,4)", 1);
		UNUM_ROP("Inf", gt, "Inf", 0);
		UNUM_ROP("(3,4)", neq, "(4,5)", 1);
		UNUM_ROP("(4,5)", neq, "(4,6)", 0);
		UNUM_ROP("4", seq, "(4,4.0625)", 0);
		UNUM_ROP("(4,5)", seq, "(4,6)", 1);
		UNUM_ROP("NaN", same, "NaN", 1);
		UNUM_ROP("Inf", same, "Inf", 1);
		UNUM_ROP("(1,2)", same, "(1,2)", 1);
		UNUM_ROP("(4,5)", same, "(4,6)", 0);

#define UNUM_AOP(op1,oper,op2,chk) \
		unum_set_str(u1, op1); \
		unum_set_str(u2, op2); \
		unum_##oper(ur, u1, u2); \
		unum_set_str(uc, chk); \
		fail |= !(ok = unum_same(ur, uc)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		unum_print(u1); printf(" %s ", #oper); unum_print(u2); \
		printf(" = "); unum_print(ur); putchar('\n')
		UNUM_AOP("2.0", add, "2.0", "4");
		UNUM_AOP("3.2768e4", sub, "125e-3", "(32767.75,32768)");
		UNUM_AOP("(6.25,6.5)", mul, "(4,4.125)", "(25,26.8125)");
		UNUM_AOP("(4,4.0625)", mul, "(-5,-4)", "(-20.3125,-16)");
		UNUM_AOP("(24,25)", div, "(5,6)", "(4,5)");

#define UNUM_OP(oper,op1,chk) \
		unum_set_str(u1, op1); \
		unum_##oper(ur, u1); \
		unum_set_str(uc, chk); \
		fail |= !(ok = unum_same(ur, uc)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		printf("%s ", #oper); unum_print(u1); \
		printf(" = "); unum_print(ur); putchar('\n')
		UNUM_OP(sq, "(4,4.125)", "(16,17.015625)");
		UNUM_OP(sq, "3", "9");
		UNUM_OP(sqrt, "2", "1.4142135623730950488");
		UNUM_OP(neg, "11", "-11");
		UNUM_OP(neg, "(8,9)", "(-9,-8)");
		UNUM_OP(abs, "-Inf", "Inf");
		UNUM_OP(abs, "(-9,-8)", "(8,9)");

		tfail |= fail;
	}
#endif

#if 1
	unum_set_env(3, 4);
	{
		UNUM_VAR(u1);
		UNUM_VAR(u2);
		UNUM_VAR(urA);
		UNUM_VAR(urB);
		int ess, fss;
		int ok, fail = 0;
		char str[128];
		char *opA1 =  "3.14159265358979323846264338327950288419716939937510582";
		char *opA2 =  "1.41421356237309504880168872420969807856967187537694807";
		char *chkA = "(4.5556640625,"
		              "4.555908203125)";
		char *chkB =  "4.5557861328125";

		unum_get_env(&ess, &fss);
		printf("\n# test unum miscellanea, env:%d,%d #\n", ess, fss);

		unum_set_str(u1, opA1);
		unum_set_str(u2, opA2);
		unum_add(urA, u1, u2);
		unum_get_str(str, urA);
		fail |= !(ok = ustr_same(str, chkA, 5));
		printf("%s PI add sqrt(2)\n", ok ? "OK  " : "FAIL");
		unum_guess(urB, urA);
		unum_get_str(str, urB);
		fail |= !(ok = ustr_same(str, chkB, 5));
		printf("%s guess (PI add sqrt(2))\n", ok ? "OK  " : "FAIL");

#define UNUM_GUESS(op1,chk) \
		unum_set_str(u1, op1); \
		unum_guess(urA, u1); \
		unum_get_str(str, urA); \
		fail |= !(ok = ustr_same(str, chk, 5)); \
		printf("%s guess ", ok ? "OK  " : "FAIL"); unum_print(u1); \
		printf(" = "); unum_print(urA); putchar('\n')
		UNUM_GUESS("10", "10");
		UNUM_GUESS("(-10,-8)", "-9");
		UNUM_GUESS("(3,Inf)", "Inf");
		UNUM_GUESS("(-1,0)", "-0.5");
		/* TODO: subnormal check */

		tfail |= fail;
	}
#endif

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
		printf("\n# test ubnd conversions, env:%d,%d #\n", ess, fss);

#define UBND_SI(val,chk) \
		si1 = val; ubnd_set_si(ub, si1); si2 = ubnd_get_si(ub); \
		fail |= !(ok = si2 == chk); \
		printf("%s si %ld : %ld\n", ok ? "OK  " : "FAIL", si1, si2)
		UBND_SI(-31, -31);

#define UBND_UI(val,chk) \
		ui1 = val; ubnd_set_ui(ub, ui1); ui2 = ubnd_get_ui(ub); \
		fail |= !(ok = ui2 == chk); \
		printf("%s ui %lu : %lu\n", ok ? "OK  " : "FAIL", ui1, ui2)
		UBND_UI(31, 31);

#define UBND_D(val,chk) \
		d1 = val; ubnd_set_d(ub, d1); d2 = ubnd_get_d(ub); \
		fail |= !(ok = d2 == chk); \
		printf("%s d %e : %e\n", ok ? "OK  " : "FAIL", d1, d2)
		UBND_D(6.0e5, 6.0e5);

#define UBND_STR(val,chk) \
		strcpy(str1, val); ubnd_set_str(ub, str1); ubnd_get_str(str2, ub); \
		fail |= !(ok = strcmp(str2, chk) == 0); \
		printf("%s str %s : %s\n", ok ? "OK  " : "FAIL", str1, str2)
		UBND_STR("(-0.25,-0.125)", "(-0.25,-0.125)");
		UBND_STR("(6,8)", "(6,8)");
		UBND_STR("(23.1,23.9)", "(23.099853515625,23.900146484375)");
		UBND_STR("(-Inf,-4]", "(-Inf,-4]");
		UBND_STR("(-Inf,-3]", "(-Inf,-3]");
		UBND_STR("(1,Inf)", "(1,Inf)");

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
		UBND_ROP("[1,3]", lt, "[3,100]", 0);
		UBND_ROP("[1,3]", lt, "(3,100]", 1);
		UBND_ROP("-2", lt, "-1", 1);
		UBND_ROP("(2,Inf)", lt, "(1,Inf)", 0);
		UBND_ROP("(-Inf,-1)", lt, "(-Inf,1)", 0);
		UBND_ROP("Inf", gt, "Inf", 0);
		UBND_ROP("(1,Inf)", gt, "(2,Inf)", 0);
		UBND_ROP("(-Inf,1)", gt, "(-Inf,-1)", 0);
		UBND_ROP("[1,2]", neq, "[3,4]", 1);
		UBND_ROP("[1,3]", neq, "[2,4]", 0);
		UBND_ROP("4", seq, "(4,4.0625)", 0);
		UBND_ROP("[-1,3]", seq, "[2,4]", 1);
		UBND_ROP("NaN", same, "NaN", 1);
		UBND_ROP("Inf", same, "Inf", 1);
		UBND_ROP("(1,2)", same, "(1,2)", 1);
		UBND_ROP("(4,5)", same, "(4,6)", 0);

#define UBND_AOP(op1,oper,op2,chk) \
		ubnd_set_str(ub1, op1); \
		ubnd_set_str(ub2, op2); \
		ubnd_##oper(ubr, ub1, ub2); \
		ubnd_set_str(ubc, chk); \
		fail |= !(ok = ubnd_same(ubr, ubc)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		ubnd_print(ub1); printf(" %s ", #oper); ubnd_print(ub2); \
		printf(" = "); ubnd_print(ubr); putchar('\n')
		UBND_AOP("[2.0,6.5)", add, "(2.0,3.5]", "(4,10)");
		UBND_AOP("3.2768e4", sub, "25e-2", "32767.75");
		UBND_AOP("[-1,2)", mul, "(-0.5,1]", "[-1,2)");
		UBND_AOP("[-1,2)", mul, "[-1,2)", "(-2,4)");
		UBND_AOP("3", mul, "5", "15");
		UBND_AOP("(6.25,6.5)", mul, "(4,4.125)", "(25,26.8125)");
		UBND_AOP("(4,4.0625)", mul, "(-5,-4)", "(-20.3125,-16)");
		UBND_AOP("[3,4)", div, "[2,3)", "(1,2)");
		UBND_AOP("[5,13)", div, "Inf", "0");
		UBND_AOP("[3,7]", div, "[0,1)", "NaN");
		UBND_AOP("21", div, "7", "3");
		UBND_AOP("(25,26.8125)", div, "(4,4.125)", "(6.060606,6.703125)");
		UBND_AOP("2", div, "(0,2]", "[1,Inf)");

#define UBND_OP(oper,op1,chk) \
		ubnd_set_str(ub1, op1); \
		ubnd_##oper(ubr, ub1); \
		ubnd_set_str(ubc, chk); \
		fail |= !(ok = ubnd_same(ubr, ubc)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		printf("%s ", #oper); ubnd_print(ub1); \
		printf(" = "); ubnd_print(ubr); putchar('\n')
		UBND_OP(sq, "(0.5,0.515625)", "(0.25,0.265869140625)");
		UBND_OP(sq, "(-Inf,1.25]", "[0,Inf)");
		UBND_OP(sq, "[-1,2)", "[0,4)");
		UBND_OP(sq, "(-2,2]", "[0,4]");
		UBND_OP(sq, "(-4,4)", "[0,16)");
		UBND_OP(sqrt, "2", "1.4142135623730950488");
		UBND_OP(sqrt, "(1,1.5625]", "(1,1.25]");
		UBND_OP(neg, "11", "-11");
		UBND_OP(neg, "[-51.5,17)", "(-17,51.5]");
		UBND_OP(abs, "-Inf", "Inf");
		UBND_OP(abs, "(-9,-8)", "(8,9)");
		UBND_OP(abs, "[-205,121)", "[0,205]");

		tfail |= fail;
	}
#endif

#if 1
	unum_set_env(2, 7);
	{
		UNUM_VAR(ur);
		UBND_VAR(ub1);
		UBND_VAR(ub2);
		UBND_VAR(ubr);
		int ess, fss;
		int ok, fail = 0;
		char str[256];
		char *opA1 =  "3.14159265358979323846264338327950288419716939937510582";
		char *opA2 =  "1.41421356237309504880168872420969807856967187537694807";
		char *chkA = "(4.5558062159628882872643321074892009627584049658624,"
		              "4.5558062159628882872643321074892009627819148528789)";
		char *chkB =  "4.5558062159628882872643321074892009627701599093706";

		unum_get_env(&ess, &fss);
		printf("\n# test ubnd miscellanea, env:%d,%d #\n", ess, fss);

		ubnd_set_str(ub1, opA1);
		ubnd_set_str(ub2, opA2);
		ubnd_add(ubr, ub1, ub2);
		ubnd_get_str(str, ubr);
		fail |= !(ok = ustr_same(str, chkA, 39));
		printf("%s PI add sqrt(2)\n", ok ? "OK  " : "FAIL");
		ubnd_guess(ur, ubr);
		unum_get_str(str, ur);
		fail |= !(ok = ustr_same(str, chkB, 39));
		printf("%s guess (PI add sqrt(2))\n", ok ? "OK  " : "FAIL");

#define UBND_GUESS(op1,chk) \
		ubnd_set_str(ub1, op1); \
		ubnd_guess(ur, ub1); \
		unum_get_str(str, ur); \
		fail |= !(ok = ustr_same(str, chk, 39)); \
		printf("%s guess ", ok ? "OK  " : "FAIL"); ubnd_print(ub1); \
		printf(" = "); unum_print(ur); putchar('\n')
		UBND_GUESS("[-Inf,Inf)", "0");
		UBND_GUESS("[-5,0]", "-2.5");
		UBND_GUESS("(8,Inf]", "Inf");
		UBND_GUESS("(-Inf,-3]", "-Inf");
		/* TODO: subnormal check */

		tfail |= fail;
	}
#endif

	unum_clear_env();
	printf("\ntest %s\n", tfail ? "FAIL" : "OK");
	return(tfail ? EXIT_FAILURE : EXIT_SUCCESS);
}
