NAME = ext-random
SRCS = ext_random.c
RANDOM_CASES ?= 1000000

include $(AM_HOME)/Makefile.app

CFLAGS += -march=rv64gcv -mabi=lp64d
CFLAGS += -fno-tree-vectorize -fno-tree-slp-vectorize
CFLAGS += -DRANDOM_CASES=$(RANDOM_CASES)
