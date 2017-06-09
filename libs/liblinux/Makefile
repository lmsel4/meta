# Targets
TARGETS := liblinux.a

# Header files/directories this library provides
HDRFILES := $(SOURCE_DIR)/liblinux_include

# Source files required to build the target
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/kernel/printk/*.c))
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c))

include $(SOURCE_DIR)/common.mk
include $(SEL4_COMMON)/common.mk
