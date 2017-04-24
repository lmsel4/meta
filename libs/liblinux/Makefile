# Targets
TARGETS := liblinux.a

# Header files/directories this library provides
HDRFILES := $(SOURCE_DIR)/liblinux_include

# Source files required to build the target
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/kernel/printk/*.c))

CFLAGS += -Wall -Wextra -g -Werror -Wno-unused-parameter
CFLAGS += -D__KERNEL__ -D__LITTLE_ENDIAN -std=gnu89
CFLAGS += -DCONFIG_X86_32

__BASE := $(STAGE_ROOT)/x86/pc99/include/liblinux_include
CFLAGS += -I$(__BASE)/x86
CFLAGS += -I$(__BASE)/x86/uapi
CFLAGS += -I$(__BASE)/uapi
CFLAGS += -I$(__BASE)

include $(SEL4_COMMON)/common.mk
