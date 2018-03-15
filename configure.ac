# Copyright (C) 2006-2014 Free Software Foundation, Inc.
# This configure.ac script is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

# Boilerplate.
# AC_PREREQ([2.69])
AC_INIT([unum], [1.0.2], [scott@llnl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/conv.c])
AC_CONFIG_HEADERS([config.h])

# Options.
# USE_MPN

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

# Checks for libraries.
AC_CHECK_LIB([gmp], [__gmpf_init],,
 [AC_MSG_ERROR(libgmp not found or uses a different ABI (including static vs shared).)])

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h gmp-impl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strchr strpbrk strstr])

# Output.
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 tests/Makefile
])
AC_OUTPUT
