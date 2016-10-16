#
# $Id: Makefile $
#
# Description: Makefile for test program
#
# $Log: Makefile $
#

#GMP = $(HOME)/src/gmp-6.1.0
SRC = ../src
TARG = tbasic

#DEFS += $(if $(findstring Windows_NT,$(OS)),-DTIMEOFDAY,-DGETTIME)

MODULES = conv gbnd glayer gmp_aux hlayer support ubnd uenv ulayer unum

OBJECTS = $(addsuffix .o,$(TARG) $(MODULES))
HEADERS = $(addsuffix .h,$(MODULES)) mpx.h gmp_macro.h
SOURCES = $(addsuffix .c,$(TARG) $(MODULES)) $(HEADERS)

VPATH = $(subst ' ',:,$(SRC))

OPT = -O3
CPPFLAGS = $(DEFS)
ifdef GMP
  CPPFLAGS += -I$(GMP)
endif
CPPFLAGS += $(patsubst %,-I%,$(SRC))
CFLAGS = $(OPT) -Wall

ifdef GMP
  LDFLAGS = -L$(GMP)/.libs
endif
LDLIBS = -lgmp

.PHONY: all
all: $(TARG)

.PHONY: check
check: $(TARG)
	./$(TARG) -v

.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(TARG)$(EXE)

.PHONY: vars
vars:
	@echo TARG: $(TARG)$(EXE)
	@echo OBJECTS: $(OBJECTS)
	@echo HEADERS: $(HEADERS)
	@echo SOURCES: $(SOURCES)
	@echo DEFS: $(DEFS)

$(TARG): $(OBJECTS)

$(OBJECTS): $(MAKEFILE_LIST) # rebuild if MAKEFILE changes
# establish module specific dependencies
$(TARG).o: $(HEADERS)
#module.o: module.h
conv.o: conv.h support.h gbnd.h uenv.h glayer.h ulayer.h mpx.h gmp_aux.h
gbnd.o: gbnd.h glayer.h
glayer.o: glayer.h uenv.h mpx.h gmp_aux.h
gmp_aux.o: gmp_aux.h
hlayer.o: hlayer.h conv.h support.h uenv.h glayer.h ulayer.h mpx.h gmp_aux.h
support.o: support.h uenv.h glayer.h ulayer.h mpx.h gmp_aux.h
ubnd.o: ubnd.h conv.h support.h gbnd.h uenv.h glayer.h ulayer.h mpx.h gmp_aux.h
uenv.o: uenv.h mpx.h
ulayer.o: ulayer.h uenv.h mpx.h gmp_aux.h
unum.o: unum.h ubnd.h uenv.h ulayer.h mpx.h gmp_aux.h
