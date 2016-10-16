/*
$Id: tdev.c $

Description: Test file

$Log: $
*/

#include <stdio.h> /* printf, putchar */
#include <stdlib.h> /* exit, atoi, malloc, realloc, free, rand */
#include <string.h> /* strrchr, strlen, strcmp, memcpy */
#include <ctype.h> /* isdigit */
#include <errno.h> /* errno */
//#define NDEBUG 1
//#include <assert.h> /* assert */

#include "unum.h"

#define PROG "tdev"
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

#if 1
	unum_set_env(3, 4);
	{
		unum *u1;
		unum *u2;
		unum *ur;
		int ess, fss;

		unum_get_env(&ess, &fss);
		printf("\n# test unum, env:%d,%d #\n", ess, fss);
		UNUM_ALLOC(u1);
		UNUM_ALLOC(u2);
		UNUM_ALLOC(ur);

		/* NOTE: example calls to unum functions */
		unum_set_si(u1, 2);
		unum_set_si(u2, 2);
		unum_add(ur, u1, u2);
		printf("2 + 2 = "); unum_print(ur); putchar('\n');

		UNUM_FREE(ur);
		UNUM_FREE(u2);
		UNUM_FREE(u1);
	}
#endif

	return(EXIT_SUCCESS); /* or EXIT_FAILURE */
}
