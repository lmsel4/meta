#ifndef __SEL4_ALIGN_H
#define __SEL4_ALIGN_H

#ifndef __ALIGN_MASK
#define __ALIGN_MASK(x, mask)  (((x)+(mask))&~(mask))
#endif

#if !defined(HZ) || HZ == 0
#define HZ CONFIG_HZ
#endif

#if TICK_NSEC == 0
#undef TICK_NSEC
#define TICK_NSEC 10
#endif

#endif // __SEL4_ALIGN_H
