/*
$Id: tulayer.c $

Description: Test file

$Log: $
*/

#include <stdio.h> /* printf, putchar */
#include <stdlib.h> /* exit, atoi, malloc, realloc, free, rand */
#include <string.h> /* strrchr, strlen, strcmp, memcpy */
#include <ctype.h> /* isdigit */
#include <errno.h> /* errno */
#include <math.h> /* NAN, INFINITY */
//#define NDEBUG 1
//#include <assert.h> /* assert */

#include "uenv.h"
#include "conv.h"
#include "support.h"
#include "ubnd.h"
#include "hlayer.h"

#define PROG "tulayer"
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

	set_uenv(3, 2);
	view_uenv();

	// set_uenv(3, 4);
	// view_uenv();

	// set_uenv(4, 11);
	// view_uenv();

	putchar('\n');

#if 0
	set_uenv(3, 2);
	{
		int es;
		mpf_t fp, tiny;
		mpx_t ut, un;

		mpf_init2(fp, MAX_FSIZE);
		mpf_init2(tiny, MAX_FSIZE);
		mpx_init2(ut, MAX_UBITS);
		mpx_init2(un, MAX_UBITS);
		mpf_div_2exp(tiny, smallsubnormal, 1);
		gmp_printf("tiny           : %Fe\n", tiny);

#define TEST1(u) printf("%-15s: ", #u); u2f(fp,u); f2u(un,fp); uview_un(un); gmp_printf(": %Fe\n", fp);
		printf("\n# test u2f f2u, environment env:%d,%d #\n", esizesize, fsizesize);
		TEST1(smallsubnormalu);
		TEST1(smallnormalu);
		TEST1(maxrealu);
		TEST1(minrealu);
		TEST1(negbigu);
		TEST1(negopeninfu);
		TEST1(posopeninfu);
		TEST1(negopenzerou);

#define TEST2(u) printf("%-15s: ", #u); u2f(fp,u); mpf_add(fp, fp, tiny); f2u(un,fp); uview_un(un); gmp_printf(": %Fe\n", fp);
		printf("\n# test u2f f2u, environment + half smallsubnormal env:%d,%d #\n", esizesize, fsizesize);
		TEST2(smallsubnormalu);
		TEST2(smallnormalu);
		TEST2(maxrealu);
		TEST2(minrealu);
		TEST2(negbigu);
		TEST2(negopeninfu);
		TEST2(posopeninfu);
		TEST2(negopenzerou);

#define TEST3(u) printf("%-15s: ", #u); u2f(fp,u); mpf_sub(fp, fp, tiny); f2u(un,fp); uview_un(un); gmp_printf(": %Fe\n", fp);
		printf("\n# test u2f f2u, environment - half smallsubnormal env:%d,%d #\n", esizesize, fsizesize);
		TEST3(smallsubnormalu);
		TEST3(smallnormalu);
		TEST3(maxrealu);
		TEST3(minrealu);
		TEST3(negbigu);
		TEST3(negopeninfu);
		TEST3(posopeninfu);
		TEST3(negopenzerou);

		printf("\n# test u2f f2u subnormal, iterate esize env:%d,%d #\n", esizesize, fsizesize);
		for (es = 0; es < esizemax; es++) {
			mpx_set_ui(ut, es);
			mpx_lshift(ut, ut, fsizesize);
			mpx_ior(ut, ut, ulpu);
			//printf("pre            : ", es); uview_un(ut); putchar('\n');
			printf("%-15d: ", es); u2f(fp,ut); f2u(un,fp); uview_un(un); gmp_printf(": %Fe\n", fp);
		}

		mpf_clear(fp);
		mpx_clear(ut);
		mpx_clear(un);
	}
#endif

#if 1
	set_uenv(2, 2);
	{
		gbnd_s gb;
		MPX_VAR(u1);
		MPX_VAR(u2);
		ubnd_s ub = {0, u1, u2};

		gbnd_init(&gb);

#define TEST1(u) \
		printf("%-15s: ", #u); ub.p = 0; mpx_set(ub.l,u); \
		u2g(&gb,&ub); g2u(&ub,&gb); \
		uview_ub(&ub); printf(": "); print_ub(&ub); putchar('\n')
		printf("\n# test u2g g2u, env:%d,%d #\n", esizesize, fsizesize);
		TEST1(smallsubnormalu);
		TEST1(smallnormalu);
		TEST1(posinfu);
		TEST1(maxrealu);
		TEST1(minrealu);
		TEST1(neginfu);
		TEST1(negbigu);
		TEST1(qNaNu);
		TEST1(sNaNu);
		TEST1(negopeninfu);
		TEST1(posopeninfu);
		TEST1(negopenzerou);

		gbnd_clear(&gb);
	}
#endif

#if 0
	set_uenv(3, 2);
	{
		int i;
		MPX_VAR(tmp);

		printf("\n# test mpx_sizeinbase, env:%d,%d #\n", esizesize, fsizesize);
		for (i = 1; i <= 16; i++) {
			mpx_set_ui(tmp, i);
			mpx_lshift(tmp, tmp, maxubits-5);
			printf("i:%d bits:%u\n", i, mpx_sizeinbase(tmp, 2));
		}
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		MPX_VAR(u1);
		MPX_VAR(u2);
		ubnd_s ub = {0, u1, u2};

		printf("\n# test unify, env:%d,%d #\n", esizesize, fsizesize);
		/* NOTE: env{3,4} prototype does unify (23.1,23.9)
		   -> (23.099853515625, 23.900146484375)
		   -> (0x77199AF,0x77E66AF) =
		   s:0 e:111 f:0111 u:1 es:010 fs:0011
		   (23,24)
		*/
		/* NOTE: env{3,4} prototype does unify (-Inf,-4] =
		   s:1 e:1 f:1111111111111111 u:1 es:000 fs:1111
		   (-Inf,-3.999969482421875)
		*/
		/* NOTE: env{3,4} prototype does not unify (-Inf,-3] =
		    s:1 e:1 f:1 u:1 es:000 fs:000,
		    s:1 e:1 f:1 u:0 es:000 fs:000
		    (-Inf,-3]
		*/

#define UNIFY_UB(op) \
		sscan_ub(op, &ub); \
		printf("unify:%s\n",op); unify(&ub, &ub); \
		printf("out:"); uview_ub(&ub); putchar('\n'); \
		printf("out:"); print_ub(&ub); putchar('\n')
		UNIFY_UB("(23.1,23.9)");
		UNIFY_UB("(-Inf,-4]");
		UNIFY_UB("(-Inf,-3]");
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		MPX_VAR(u1);
		MPX_VAR(u2);
		ubnd_s ub = {0, u1, u2};

		printf("\n# test sscan, env:%d,%d #\n", esizesize, fsizesize);

#define SSCAN_UN(op) \
		printf("sscan_un:%s\n",op); sscan_un(op, u1); \
		printf("out:"); uview_un(u1); putchar('\n'); \
		printf("out:"); print_un(u1); putchar('\n')
		SSCAN_UN("(23.1,23.9)");
		SSCAN_UN("(-Inf,-4]");
		/*SSCAN_UN("(-Inf,-3]");*/ /* can't unify */
		SSCAN_UN("0.1");

		printf("-----\n");

#define SSCAN_UB(op) \
		printf("sscan_ub:%s\n",op); sscan_ub(op, &ub); \
		printf("out:"); uview_ub(&ub); putchar('\n'); \
		printf("out:"); print_ub(&ub); putchar('\n')
		SSCAN_UB("(23.1,23.9)");
		SSCAN_UB("(-Inf,-4]");
		SSCAN_UB("(-Inf,-3]");
		SSCAN_UB("0.1");
	}
#endif

#if 0
	set_uenv(3, 4);
	{
		MPX_VAR(u1);
		MPX_VAR(u2);
		ubnd_s ub = {0, u1, u2};
		int c;
		char str[256];

		printf("\n# test scan and print, env:%d,%d #\n", esizesize, fsizesize);
		printf("bnd:"); scan_ub(&ub);
		putchar('|');
		while ((c = getc(stdin)) != '\0' && c != '\n' && c != EOF) putchar(c);
		putchar('|'); putchar('\n');
		print_ub(&ub); putchar('\n');
		sprint_ub(str, &ub);
		printf("%s\n", str);
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		UB_VAR(ub);

		printf("\n# test d2ub, env:%d,%d #\n", esizesize, fsizesize);
#define D2UB(op) \
		print_ub(d2ub(ub,op)); printf(" = d2ub %g\n",op);
		D2UB(1.0);
		D2UB(6.02214e23);
		D2UB(M_PI);
		D2UB(NAN);
		D2UB(+INFINITY);
		D2UB(-INFINITY);
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		int i;
		UB_VAR(ub1);
		UB_VAR(ub2);

		printf("\n# test plusu small increment, env:%d,%d #\n", esizesize, fsizesize);

		sscan_ub("(6.42786833891686830433e-18,6.4279212784760716981e-18)", ub1);
		printf("arg1:"); print_ub(ub1); putchar('\n');
		printf("arg1:"); uview_ub(ub1); putchar('\n');
		sscan_ub("965.6484375", ub2);
		printf("arg2:"); print_ub(ub2); putchar('\n');
		printf("arg2:"); uview_ub(ub2); putchar('\n');
		for (i = 0; i < 3; i++) {
			plusu(ub2, ub1, ub2);
			printf("res :"); print_ub(ub2); putchar('\n');
			printf("res :"); uview_ub(ub2); putchar('\n');
		}

		printf("-----\n");

		sscan_ub("4", ub1);
		printf("arg1:"); print_ub(ub1); putchar('\n');
		printf("arg1:"); uview_ub(ub1); putchar('\n');
		mpx_ior(ub2->l, smallsubnormalu, ubitmask); ub2->p = 0;
		printf("arg2:"); print_ub(ub2); putchar('\n');
		printf("arg2:"); uview_ub(ub2); putchar('\n');
		for (i = 0; i < 3; i++) {
			plusu(ub1, ub1, ub2);
			printf("res :"); print_ub(ub1); putchar('\n');
			printf("res :"); uview_ub(ub1); putchar('\n');
		}

		printf("-----\n");

		sscan_ub("4", ub1);
		printf("arg1:"); print_ub(ub1); putchar('\n');
		printf("arg1:"); uview_ub(ub1); putchar('\n');
		unum_copy(ub2->l, smallnormalu); ub2->p = 0;
		printf("arg2:"); print_ub(ub2); putchar('\n');
		printf("arg2:"); uview_ub(ub2); putchar('\n');
		for (i = 0; i < 3; i++) {
			plusu(ub1, ub1, ub2);
			printf("res :"); print_ub(ub1); putchar('\n');
			printf("res :"); uview_ub(ub1); putchar('\n');
		}
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		UN_VAR(un1);
		UN_VAR(un2);

		printf("\n# test promotee, env:%d,%d #\n", esizesize, fsizesize);
		si2un(un1, 1);
		printf("in : "); print_un(un1); printf(" : "); uview_un(un1); putchar('\n');
		promotee(un2, un2);
		printf("out: "); print_un(un2); printf(" : "); uview_un(un2); putchar('\n');
	}
#endif

#if 1
	set_uenv(3, 4);
	{
		UB_VAR(ub1);
		UB_VAR(ub2);
		UB_VAR(ubr);
		UB_VAR(ubc);
		int ir, ok, fail = 0;

		printf("\n# test cmp & clip, env:%d,%d #\n", esizesize, fsizesize);

#define UB_ROPE(op1,e1,oper,op2,e2,chk) \
		sscan_ub(op1, ub1); \
		sscan_ub(op2, ub2); \
		fail |= !(ok = ((ir = oper##uQ(ub1,e1, ub2,e2)) == chk)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		print_ub(ub1); printf("%s %s ", #e1, #oper); print_ub(ub2); printf("%s", #e2); \
		printf(" : %d\n", ir)
		UB_ROPE("[1,3]",LE, cmp, "[3,100]",LE, -1);
		UB_ROPE("[1,3]",RE, cmp, "[3,100]",LE, 0);
		UB_ROPE("100",RE, cmp, "(3,100)",RE,  1);

#define UB_AOP(op1,oper,op2,chk) \
		sscan_ub(op1, ub1); \
		sscan_ub(op2, ub2); \
		oper##u(ubr, ub1, ub2); \
		sscan_ub(chk, ubc); \
		fail |= !(ok = sameuQ(ubr, ubc)); \
		printf("%s ", ok ? "OK  " : "FAIL"); \
		print_ub(ub1); printf(" %s ", #oper); print_ub(ub2); \
		printf(" = "); print_ub(ubr); putchar('\n')
		UB_AOP("[4,6]", clipl, "(5,5.5]", "(5,6]");
		UB_AOP("[4,6]", clipl, "(5,7)", "(5,7)");
		UB_AOP("[4,8]", cliph, "(5,7)", "[4,7)");
		UB_AOP("[4,8]", cliph, "(3,7)", "(3,7)");

		tfail |= fail;
	}
#endif

	clear_uenv();
	printf("\ntest %s\n", tfail ? "FAIL" : "OK");
	return(tfail ? EXIT_FAILURE : EXIT_SUCCESS);
}
