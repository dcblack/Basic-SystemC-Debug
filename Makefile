#!make -f
#

SRCS := behavior.cpp \
        observer.cpp \
        stimulus.cpp \
        top.cpp \
        main.cpp

define DOCUMENTATION

Description
-----------

This makefile specially equiped to allow specification
of run-time argument by defining four variables on the
command-line: HELP, DEBUG, TRACE, and INJECT.

EXAMPLES
--------
make HELP=1
MAKE DEBUG=all
MAKE DEBUG=observer TRACE=1
MAKE DEBUG=observer TRACE=1 INJECT=33

endef
   
ifndef ARGS
 ifdef HELP
    ARGS := -help
 else
 #--------------------
 ifdef DEBUG
  ifeq "${DEBUG}" "all"
    ARGS := -debugall -trace
  else
    ARGS := -debug=${DEBUG}
  endif
 else
 endif
 #--------------------
 ifdef TRACE
    ARGS += -trace
 endif
 #--------------------
 ifdef INJECT
    ARGS += -inject=${INJECT}
 endif
endif
#--------------------
# Default if nothing was defined
ifndef HELP
 ifndef DEBUG
  ifndef TRACE
   ifndef INJECT
    ARGS := -trace
   endif
  endif
 endif
endif
endif
$(info ARGS=${ARGS})

RULES:=$(firstword $(wildcard $(addsuffix /Makefile.defs,${SCC_APPS}/make ../../.. ../.. .. .)))
$(if ${RULES},$(info INFO: Including $(realpath ${RULES})),$(error Could not find Makefile.defs))
include ${RULES}
