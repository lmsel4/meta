#
# Copyright 2015, NICTA
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(NICTA_BSD)
#

# Targets
TARGETS := $(notdir $(SOURCE_DIR)).bin

# Set custom entry point as we are a rootserver and will not be
# started in a standard way
ENTRY_POINT := _sel4_start

# Source files required to build the target
CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c))
CFILES   += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/plat/${PLAT}/*.c))
CFILES   += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/arch/${ARCH}/*.c))

ASMFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/arch/${ARCH}/*.S))
ASMFILES += $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/plat/${PLAT}/*.S))

# Libraries required to build the target
LIBS = c sel4 sel4muslcsys \
       sel4simple sel4vka sel4allocman sel4vspace \
       sel4platsupport \
       utils sel4simple-default sel4utils sel4debug \
       linux

# extra cflags 
CFLAGS += -Wall -Wextra -g

CFLAGS += -D__KERNEL__ -D__LITTLE_ENDIAN -std=gnu89
CFLAGS += -DCONFIG_X86_32=y -DCONFIG_TREE_RCU=y
CFLAGS += -DCONFIG_MCORE2=y

__BASE := $(objtree)/stage/x86/pc99/include/liblinux_include
CFLAGS += -I$(__BASE)/x86/uapi
CFLAGS += -I$(__BASE)/x86
CFLAGS += -I$(__BASE)
CFLAGS += -I$(__BASE)/uapi


include $(SEL4_COMMON)/common.mk

