DFT_IMPL ?= oai
FFT_POINTS ?= 16
FFT_REPEAT ?= 1
FFT_WARMUP_REPEAT ?= 2
FFT_RUN_IDFT ?= 0
FFT_VALIDATE ?= 1
FFT_ROI_HPM_ENABLE ?= 1
FFT_ROI_HPM_PROFILE ?= canonical
FFT_MAINLINE ?= 1

ifneq ($(origin FFT_KERNEL_SOURCE),undefined)
$(error FFT_KERNEL_SOURCE is not configurable in the frozen export)
endif
ifneq ($(origin FFT_KERNEL_FLAGS),undefined)
$(error FFT_KERNEL_FLAGS is not configurable in the frozen export)
endif
ifneq ($(FFT_MAINLINE),1)
$(error This export contains only maintained-mainline kernels; FFT_MAINLINE must remain 1)
endif
FFT_CFG_TAG := n$(FFT_POINTS)-r$(FFT_REPEAT)-idft$(FFT_RUN_IDFT)
FFT_SCRATCH_CLEAN_TARGETS := \
	$(CURDIR)/__pycache__/ \
	$(CURDIR)/all_asm.s \
	$(CURDIR)/elf_with_vset.asm \
	$(CURDIR)/elf_without_vset.asm \
	$(CURDIR)/rvv_vset.asm \
	$(CURDIR)/simulator_err*.txt \
	$(CURDIR)/*.log
APP_CLEAN_TARGETS := $(CURDIR)/build/ $(FFT_SCRATCH_CLEAN_TARGETS)

ifeq ($(FFT_VALIDATE),1)
FFT_MODE_TAG := validate
FFT_MODE_CFLAGS := -DFFT_VALIDATE=1
else ifeq ($(FFT_VALIDATE),0)
FFT_MODE_TAG := perf
FFT_MODE_CFLAGS := -DFFT_VALIDATE=0
else
$(error Unsupported FFT_VALIDATE='$(FFT_VALIDATE)'; use '1' or '0')
endif

ifeq ($(FFT_ROI_HPM_ENABLE),1)
FFT_HPM_ENABLE_CFLAGS := -DFFT_ROI_HPM_ENABLE=1
FFT_HPM_ENABLE_TAG :=
else ifeq ($(FFT_ROI_HPM_ENABLE),0)
FFT_HPM_ENABLE_CFLAGS := -DFFT_ROI_HPM_ENABLE=0
FFT_HPM_ENABLE_TAG := -hpmoff
else
$(error Unsupported FFT_ROI_HPM_ENABLE='$(FFT_ROI_HPM_ENABLE)'; use '0' or '1')
endif

ifeq ($(FFT_ROI_HPM_PROFILE),canonical)
FFT_HPM_CFLAGS := -DFFT_ROI_HPM_PROFILE=0
FFT_HPM_TAG :=
else ifeq ($(FFT_ROI_HPM_PROFILE),diagnostic)
FFT_HPM_CFLAGS := -DFFT_ROI_HPM_PROFILE=1
FFT_HPM_TAG := -hpmdiag
else ifeq ($(FFT_ROI_HPM_PROFILE),backend_diag)
FFT_HPM_CFLAGS := -DFFT_ROI_HPM_PROFILE=2
FFT_HPM_TAG := -hpmbackdiag
else
$(error Unsupported FFT_ROI_HPM_PROFILE='$(FFT_ROI_HPM_PROFILE)'; use 'canonical', 'diagnostic', or 'backend_diag')
endif

FFT_MAINLINE_TAG := -mainline

FFT_WARMUP_TAG := -w$(FFT_WARMUP_REPEAT)
FFT_BUILD_TAG := $(DFT_IMPL)-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)$(FFT_WARMUP_TAG)

ifneq ($(wildcard $(CURDIR)/kernels/mainline.mk),)
include $(CURDIR)/kernels/mainline.mk
else
$(error Missing frozen kernel map: $(CURDIR)/kernels/mainline.mk)
endif

ifneq ($(DFT_IMPL),oai)
ifeq ($(strip $(FFT_KERNEL_SOURCE)),)
$(error No frozen kernel is mapped for DFT_IMPL=$(DFT_IMPL), FFT_POINTS=$(FFT_POINTS))
endif
endif

ifeq ($(DFT_IMPL),dft_rvv)
FFT_ARCH_CFLAGS := -march=rv64gcv -mabi=lp64d
else ifeq ($(DFT_IMPL),dft_ext)
FFT_ARCH_CFLAGS := -march=rv64gcv -mabi=lp64d
else
FFT_ARCH_CFLAGS := -march=rv64gc -mabi=lp64d
endif

ifeq ($(DFT_IMPL),oai)
NAME ?= hello-oai-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)
FFT_APP_SRCS := ./oai_dfts.c
FFT_APP_CFLAGS := -DFFT_OAI_DFT16_MINIMAL=1 -fno-tree-vectorize -fno-tree-slp-vectorize
else ifeq ($(DFT_IMPL),dft_scalar)
FFT_BUILD_TAG := $(DFT_IMPL)-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)$(FFT_WARMUP_TAG)
NAME ?= hello-scalar-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)
ifeq ($(strip $(FFT_KERNEL_SOURCE)),)
FFT_APP_SRCS := ./dft_scalar.c
else
FFT_APP_SRCS := $(FFT_KERNEL_SOURCE)
endif
ifeq ($(FFT_VALIDATE),1)
FFT_APP_SRCS += ./oai_dfts.c
FFT_APP_CFLAGS := -DFFT_OAI_DFT16_MINIMAL=1 -DFFT_LIBRARY_ONLY=1 -fno-tree-vectorize -fno-tree-slp-vectorize
else
FFT_APP_CFLAGS := -fno-tree-vectorize -fno-tree-slp-vectorize
endif
else ifeq ($(DFT_IMPL),dft_rvv)
ifeq ($(strip $(FFT_KERNEL_SOURCE)),)
FFT_APP_SRCS := ./dft_rvv.c
else
FFT_APP_SRCS := $(FFT_KERNEL_SOURCE)
endif
NAME ?= hello-rvv-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)
FFT_APP_CFLAGS :=
FFT_APP_CFLAGS += -fno-tree-vectorize -fno-tree-slp-vectorize
ifeq ($(FFT_VALIDATE),1)
FFT_APP_SRCS += ./oai_dfts.c
FFT_APP_CFLAGS += -DFFT_OAI_DFT16_MINIMAL=1 -DFFT_LIBRARY_ONLY=1
endif
else ifeq ($(DFT_IMPL),dft_ext)
ifeq ($(strip $(FFT_KERNEL_SOURCE)),)
FFT_APP_SRCS := ./dft_ext.c
else
FFT_APP_SRCS := $(FFT_KERNEL_SOURCE)
endif
NAME ?= hello-ext-$(FFT_MODE_TAG)-$(FFT_CFG_TAG)
FFT_APP_CFLAGS :=
FFT_APP_CFLAGS += -fno-tree-vectorize -fno-tree-slp-vectorize
ifeq ($(FFT_VALIDATE),1)
FFT_APP_SRCS += ./oai_dfts.c
FFT_APP_CFLAGS += -DFFT_OAI_DFT16_MINIMAL=1 -DFFT_LIBRARY_ONLY=1
endif
else
$(error Unsupported DFT_IMPL='$(DFT_IMPL)'; use 'oai', 'dft_scalar', 'dft_rvv', or 'dft_ext')
endif

# Keep object files variant-specific so switching build flags does not reuse
# stale objects built with incompatible preprocessor flags.
FFT_BUILD_TAG := $(FFT_BUILD_TAG)$(FFT_HPM_ENABLE_TAG)$(FFT_HPM_TAG)$(FFT_MAINLINE_TAG)
DST_DIR := $(CURDIR)/build/$(ARCH)-$(FFT_BUILD_TAG)/

SRCS = $(FFT_APP_SRCS)
include $(AM_HOME)/Makefile.app

CFLAGS   += $(FFT_ARCH_CFLAGS)
CFLAGS   += -DFFT_MAIN_POINTS=$(FFT_POINTS) -DFFT_MAIN_REPEAT=$(FFT_REPEAT) -DFFT_MAIN_WARMUP_REPEAT=$(FFT_WARMUP_REPEAT) -DFFT_MAIN_RUN_IDFT=$(FFT_RUN_IDFT)
CFLAGS   += -DFFT_MAINLINE_ENABLE=1
CFLAGS   += $(FFT_MODE_CFLAGS)
CFLAGS   += $(FFT_HPM_ENABLE_CFLAGS)
CFLAGS   += $(FFT_HPM_CFLAGS)
CFLAGS   += $(FFT_APP_CFLAGS)
# Point-specific exports retain static helpers for other supported sizes.
CFLAGS   += -Wno-error=unused-function -Wno-error=unused-variable
CFLAGS   += $(FFT_KERNEL_FLAGS)

# ReConstruct: keep the OAI validation reference scalar on this FPGA port.
# With -march=rv64gcv SIMDe maps the SSE dft16 to RVV and the core hangs after
# a few repeated calls; the scalar reference is bit-identical for this kernel.
# Object path deliberately matches OBJS ($(DST_DIR)/./<name>.o).
$(DST_DIR)/./oai_dfts.o: CFLAGS := $(filter-out -march=rv64gc -march=rv64gcv,$(CFLAGS)) -march=rv64gc

tidy:
	rm -rf $(FFT_SCRATCH_CLEAN_TARGETS)

.PHONY: tidy

FFT_FROZEN_IMPLS := dft_scalar dft_rvv dft_ext
FFT_FROZEN_POINTS := 16 64 128 256 512 1024 2048 4096

.PHONY: xvbbp-build-all
xvbbp-build-all:
	@set -eu; \
	for impl in $(FFT_FROZEN_IMPLS); do \
	  for points in $(FFT_FROZEN_POINTS); do \
	    for idft in 0 1; do \
	      $(MAKE) DFT_IMPL=$$impl FFT_POINTS=$$points FFT_RUN_IDFT=$$idft FFT_VALIDATE=1; \
	    done; \
	  done; \
	done
