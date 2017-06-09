# Targets
TARGETS := liblinux.a

# Header files/directories this library provides
HDRFILES := $(SOURCE_DIR)/liblinux_include

# Source files required to build the target
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/kernel/printk/*.c))
CFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c))

CFLAGS += -Wall -Wextra -g -Werror -Wno-unused-parameter
CFLAGS += -D__KERNEL__ -D__LITTLE_ENDIAN -std=gnu89
CFLAGS += -DCONFIG_X86_32 -DCONFIG_HZ=1000 -DCONFIG_TREE_RCU=y
CFLAGS += -DHZ=1000
CFLAGS += -include linux/kconfig.h -include align.h
CFLAGS += -include seL4/types.h -include linux/types.h
CFLAGS += -Wno-incompatible-pointer-types -Wno-shift-count-overflow
CFLAGS += -Wno-implicit-function-declaration

__BASE := $(objtree)/stage/x86/pc99/include/liblinux_include
CFLAGS += -I$(__BASE)
CFLAGS += -I$(__BASE)/x86
CFLAGS += -I$(__BASE)/x86/uapi
CFLAGS += -I$(__BASE)/uapi
CFLAGS += -include seL4/types.h


include $(SEL4_COMMON)/common.mk
