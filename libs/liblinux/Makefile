# Targets
TARGETS := liblinux.a

# Header files/directories this library provides
HDRFILES := $(wildcard $(SOURCE_DIR)/include/*) \
            $(wildcard ${SOURCE_DIR}/include/${ARCH}/*) \
            $(wildcard ${SOURCE_DIR}/arch_include/${ARCH}/*)

# Source files required to build the target
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/kernel/printk/*.c))

CFLAGS += -Wall -Wextra -g -Werror -Wno-unused-parameter
CFLAGS += -D__EXPORTED_HEADERS__ -D__KERNEL__ -D__LITTLE_ENDIAN

include $(SEL4_COMMON)/common.mk
