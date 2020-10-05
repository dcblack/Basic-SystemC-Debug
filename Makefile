#!make -f

SRCS:=behavior.cpp observer.cpp stimulus.cpp top.cpp main.cpp
ifdef HELP
ARGS:=-help
else
 ifdef DEBUG
  ifeq "${DEBUG}" "splitter"
   ARGS:=-debug=splitter -trace
  else
   ifeq "${DEBUG}" "all"
    ARGS:=-debugall -trace
   else
    ARGS:=-debug -trace
   endif
  endif
 else
 ifdef TRACE
  ARGS:=-debug -trace
 endif
 endif
endif

RULES:=$(firstword $(wildcard $(addsuffix /Makefile.defs,${SCC_APPS}/make ../../.. ../.. .. .)))
$(if ${RULES},$(info INFO: Including $(realpath ${RULES})),$(error Could not find Makefile.defs))
include ${RULES}
