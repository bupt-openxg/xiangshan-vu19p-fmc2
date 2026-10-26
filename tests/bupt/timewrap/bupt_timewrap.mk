# ReConstruct: link-time whole-main cycle counter for BUPT FFT/MIMO builds.
BUPT_TIME_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
BUPT_TIME_SRC := $(BUPT_TIME_DIR)measure_main.c
BUPT_TIME_OBJ := $(DST_DIR)/bupt_timewrap.o

LDFLAGS    += --wrap=main
LINK_FILES += $(BUPT_TIME_OBJ)
image: $(BUPT_TIME_OBJ)

$(BUPT_TIME_OBJ): $(BUPT_TIME_SRC)
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) -std=gnu11 $(CFLAGS) -c -o $@ $(realpath $<)
