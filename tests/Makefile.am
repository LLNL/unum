# Copyright (C) 2006-2014 Free Software Foundation, Inc.
# This Makefile.am is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

check_PROGRAMS = tbasic tulayer tunumxx
tunumxx_SOURCES = tunumxx.cpp
EXTRA_DIST = tdev.mak tdevxx.mak tdev.c
AM_CPPFLAGS = -I$(top_srcdir)/src
LDADD = $(top_builddir)/src/libunum.a
TESTS = $(check_PROGRAMS)
