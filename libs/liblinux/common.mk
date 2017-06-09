# base
CFLAGS += -D__KERNEL__ -std=gnu89
CFLAGS += -Wno-implicit-function-declaration -Wno-incompatible-pointer-types
CFLAGS += -Wno-pointer-sign -Wno-shift-count-overflow

# CONFIG_*
CFLAGS += -DCONFIG_X86_32=y -DCONFIG_TREE_RCU=y
CFLAGS += -DCONFIG_MCORE2=y -DCONFIG_HZ=1000
CFLAGS += -DCONFIG_X86_L1_CACHE_SHIFT=6

# TODO find correct include path defining these
CFLAGS += -DBITS_PER_LONG=32 -D__LITTLE_ENDIAN 
CFLAGS += -DHZ=1000 -DMSEC_PER_SEC=1000

# some always needed files, TODO too much magic 
CFLAGS += -include seL4/types.h
CFLAGS += -include linux/kconfig.h -include align.h -include linux/types.h
CFLAGS += -include asm/msr-index.h
CFLAGS += -include $(srctree)/include/generated/autoconf.h

# magic to add the correct Linux header
__BASE := $(objtree)/stage/x86/pc99/include/liblinux_include
CFLAGS += -I$(__BASE)
CFLAGS += -I$(__BASE)/x86
CFLAGS += -I$(__BASE)/x86/uapi
CFLAGS += -I$(__BASE)/uapi
