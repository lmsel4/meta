#ifndef __LINUX_GFP_H
#define __LINUX_GFP_H

/*
 * In case of changes, please don't forget to update
 * include/trace/events/mmflags.h and tools/perf/builtin-kmem.c
 */

/* Plain integer GFP bitmasks. Do not use this directly. */
#define ___GFP_DMA              0x01u
#define ___GFP_HIGHMEM          0x02u
#define ___GFP_DMA32            0x04u
#define ___GFP_MOVABLE          0x08u
#define ___GFP_RECLAIMABLE      0x10u
#define ___GFP_HIGH             0x20u
#define ___GFP_IO               0x40u
#define ___GFP_FS               0x80u
#define ___GFP_COLD             0x100u
#define ___GFP_NOWARN           0x200u
#define ___GFP_REPEAT           0x400u
#define ___GFP_NOFAIL           0x800u
#define ___GFP_NORETRY          0x1000u
#define ___GFP_MEMALLOC         0x2000u
#define ___GFP_COMP             0x4000u
#define ___GFP_ZERO             0x8000u
#define ___GFP_NOMEMALLOC       0x10000u
#define ___GFP_HARDWALL         0x20000u
#define ___GFP_THISNODE         0x40000u
#define ___GFP_ATOMIC           0x80000u
#define ___GFP_ACCOUNT          0x100000u
#define ___GFP_NOTRACK          0x200000u
#define ___GFP_DIRECT_RECLAIM   0x400000u
#define ___GFP_OTHER_NODE       0x800000u
#define ___GFP_WRITE            0x1000000u
#define ___GFP_KSWAPD_RECLAIM   0x2000000u
/* If the above are modified, __GFP_BITS_SHIFT may need updating */

/*
 * Physical address zone modifiers (see linux/mmzone.h - low four bits)
 *
 * Do not put any conditional on these. If necessary modify the definitions
 * without the underscores and use them consistently. The definitions here may
 * be used in bit comparisons.
 */
#define __GFP_DMA       ((__force gfp_t)___GFP_DMA)
#define __GFP_HIGHMEM   ((__force gfp_t)___GFP_HIGHMEM)
#define __GFP_DMA32     ((__force gfp_t)___GFP_DMA32)
#define __GFP_MOVABLE   ((__force gfp_t)___GFP_MOVABLE)  /* ZONE_MOVABLE allowed */
#define GFP_ZONEMASK    (__GFP_DMA|__GFP_HIGHMEM|__GFP_DMA32|__GFP_MOVABLE)

/*
 * Page mobility and placement hints
 *
 * These flags provide hints about how mobile the page is. Pages with similar
 * mobility are placed within the same pageblocks to minimise problems due
 * to external fragmentation.
 *
 * __GFP_MOVABLE (also a zone modifier) indicates that the page can be
 *   moved by page migration during memory compaction or can be reclaimed.
 *
 * __GFP_RECLAIMABLE is used for slab allocations that specify
 *   SLAB_RECLAIM_ACCOUNT and whose pages can be freed via shrinkers.
 *
 * __GFP_WRITE indicates the caller intends to dirty the page. Where possible,
 *   these pages will be spread between local zones to avoid all the dirty
 *   pages being in one zone (fair zone allocation policy).
 *
 * __GFP_HARDWALL enforces the cpuset memory allocation policy.
 *
 * __GFP_THISNODE forces the allocation to be satisified from the requested
 *   node with no fallbacks or placement policy enforcements.
 *
 * __GFP_ACCOUNT causes the allocation to be accounted to kmemcg.
 */
#define __GFP_RECLAIMABLE ((__force gfp_t)___GFP_RECLAIMABLE)
#define __GFP_WRITE     ((__force gfp_t)___GFP_WRITE)
#define __GFP_HARDWALL   ((__force gfp_t)___GFP_HARDWALL)
#define __GFP_THISNODE  ((__force gfp_t)___GFP_THISNODE)
#define __GFP_ACCOUNT   ((__force gfp_t)___GFP_ACCOUNT)

/*
 * Watermark modifiers -- controls access to emergency reserves
 *
 * __GFP_HIGH indicates that the caller is high-priority and that granting
 *   the request is necessary before the system can make forward progress.
 *   For example, creating an IO context to clean pages.
 *
 * __GFP_ATOMIC indicates that the caller cannot reclaim or sleep and is
 *   high priority. Users are typically interrupt handlers. This may be
 *   used in conjunction with __GFP_HIGH
 *
 * __GFP_MEMALLOC allows access to all memory. This should only be used when
 *   the caller guarantees the allocation will allow more memory to be freed
 *   very shortly e.g. process exiting or swapping. Users either should
 *   be the MM or co-ordinating closely with the VM (e.g. swap over NFS).
 *
 * __GFP_NOMEMALLOC is used to explicitly forbid access to emergency reserves.
 *   This takes precedence over the __GFP_MEMALLOC flag if both are set.
 */
#define __GFP_ATOMIC    ((__force gfp_t)___GFP_ATOMIC)
#define __GFP_HIGH      ((__force gfp_t)___GFP_HIGH)
#define __GFP_MEMALLOC  ((__force gfp_t)___GFP_MEMALLOC)
#define __GFP_NOMEMALLOC ((__force gfp_t)___GFP_NOMEMALLOC)

/*
 * Reclaim modifiers
 *
 * __GFP_IO can start physical IO.
 *
 * __GFP_FS can call down to the low-level FS. Clearing the flag avoids the
 *   allocator recursing into the filesystem which might already be holding
 *   locks.
 *
 * __GFP_DIRECT_RECLAIM indicates that the caller may enter direct reclaim.
 *   This flag can be cleared to avoid unnecessary delays when a fallback
 *   option is available.
 *
 * __GFP_KSWAPD_RECLAIM indicates that the caller wants to wake kswapd when
 *   the low watermark is reached and have it reclaim pages until the high
 *   watermark is reached. A caller may wish to clear this flag when fallback
 *   options are available and the reclaim is likely to disrupt the system. The
 *   canonical example is THP allocation where a fallback is cheap but
 *   reclaim/compaction may cause indirect stalls.
 *
 * __GFP_RECLAIM is shorthand to allow/forbid both direct and kswapd reclaim.
 *
 * __GFP_REPEAT: Try hard to allocate the memory, but the allocation attempt
 *   _might_ fail.  This depends upon the particular VM implementation.
 *
 * __GFP_NOFAIL: The VM implementation _must_ retry infinitely: the caller
 *   cannot handle allocation failures. New users should be evaluated carefully
 *   (and the flag should be used only when there is no reasonable failure
 *   policy) but it is definitely preferable to use the flag rather than
 *   opencode endless loop around allocator.
 *
 * __GFP_NORETRY: The VM implementation must not retry indefinitely and will
 *   return NULL when direct reclaim and memory compaction have failed to allow
 *   the allocation to succeed.  The OOM killer is not called with the current
 *   implementation.
 */
#define __GFP_IO        ((__force gfp_t)___GFP_IO)
#define __GFP_FS        ((__force gfp_t)___GFP_FS)
#define __GFP_DIRECT_RECLAIM    ((__force gfp_t)___GFP_DIRECT_RECLAIM) /* Caller can reclaim */
#define __GFP_KSWAPD_RECLAIM    ((__force gfp_t)___GFP_KSWAPD_RECLAIM) /* kswapd can wake */
#define __GFP_RECLAIM ((__force gfp_t)(___GFP_DIRECT_RECLAIM|___GFP_KSWAPD_RECLAIM))
#define __GFP_REPEAT    ((__force gfp_t)___GFP_REPEAT)
#define __GFP_NOFAIL    ((__force gfp_t)___GFP_NOFAIL)
#define __GFP_NORETRY   ((__force gfp_t)___GFP_NORETRY)

/*
 * Action modifiers
 *
 * __GFP_COLD indicates that the caller does not expect to be used in the near
 *   future. Where possible, a cache-cold page will be returned.
 *
 * __GFP_NOWARN suppresses allocation failure reports.
 *
 * __GFP_COMP address compound page metadata.
 *
 * __GFP_ZERO returns a zeroed page on success.
 *
 * __GFP_NOTRACK avoids tracking with kmemcheck.
 *
 * __GFP_NOTRACK_FALSE_POSITIVE is an alias of __GFP_NOTRACK. It's a means of
 *   distinguishing in the source between false positives and allocations that
 *   cannot be supported (e.g. page tables).
 *
 * __GFP_OTHER_NODE is for allocations that are on a remote node but that
 *   should not be accounted for as a remote allocation in vmstat. A
 *   typical user would be khugepaged collapsing a huge page on a remote
 *   node.
 */
#define __GFP_COLD      ((__force gfp_t)___GFP_COLD)
#define __GFP_NOWARN    ((__force gfp_t)___GFP_NOWARN)
#define __GFP_COMP      ((__force gfp_t)___GFP_COMP)
#define __GFP_ZERO      ((__force gfp_t)___GFP_ZERO)
#define __GFP_NOTRACK   ((__force gfp_t)___GFP_NOTRACK)
#define __GFP_NOTRACK_FALSE_POSITIVE (__GFP_NOTRACK)
#define __GFP_OTHER_NODE ((__force gfp_t)___GFP_OTHER_NODE)

/* Room for N __GFP_FOO bits */
#define __GFP_BITS_SHIFT 26
#define __GFP_BITS_MASK ((__force gfp_t)((1 << __GFP_BITS_SHIFT) - 1))

/*
 * Useful GFP flag combinations that are commonly used. It is recommended
 * that subsystems start with one of these combinations and then set/clear
 * __GFP_FOO flags as necessary.
 *
 * GFP_ATOMIC users can not sleep and need the allocation to succeed. A lower
 *   watermark is applied to allow access to "atomic reserves"
 *
 * GFP_KERNEL is typical for kernel-internal allocations. The caller requires
 *   ZONE_NORMAL or a lower zone for direct access but can direct reclaim.
 *
 * GFP_KERNEL_ACCOUNT is the same as GFP_KERNEL, except the allocation is
 *   accounted to kmemcg.
 *
 * GFP_NOWAIT is for kernel allocations that should not stall for direct
 *   reclaim, start physical IO or use any filesystem callback.
 *
 * GFP_NOIO will use direct reclaim to discard clean pages or slab pages
 *   that do not require the starting of any physical IO.
 *
 * GFP_NOFS will use direct reclaim but will not use any filesystem interfaces.
 *
 * GFP_USER is for userspace allocations that also need to be directly
 *   accessibly by the kernel or hardware. It is typically used by hardware
 *   for buffers that are mapped to userspace (e.g. graphics) that hardware
 *   still must DMA to. cpuset limits are enforced for these allocations.
 *
 * GFP_DMA exists for historical reasons and should be avoided where possible.
 *   The flags indicates that the caller requires that the lowest zone be
 *   used (ZONE_DMA or 16M on x86-64). Ideally, this would be removed but
 *   it would require careful auditing as some users really require it and
 *   others use the flag to avoid lowmem reserves in ZONE_DMA and treat the
 *   lowest zone as a type of emergency reserve.
 *
 * GFP_DMA32 is similar to GFP_DMA except that the caller requires a 32-bit
 *   address.
 *
 * GFP_HIGHUSER is for userspace allocations that may be mapped to userspace,
 *   do not need to be directly accessible by the kernel but that cannot
 *   move once in use. An example may be a hardware allocation that maps
 *   data directly into userspace but has no addressing limitations.
 *
 * GFP_HIGHUSER_MOVABLE is for userspace allocations that the kernel does not
 *   need direct access to but can use kmap() when access is required. They
 *   are expected to be movable via page reclaim or page migration. Typically,
 *   pages on the LRU would also be allocated with GFP_HIGHUSER_MOVABLE.
 *
 * GFP_TRANSHUGE and GFP_TRANSHUGE_LIGHT are used for THP allocations. They are
 *   compound allocations that will generally fail quickly if memory is not
 *   available and will not wake kswapd/kcompactd on failure. The _LIGHT
 *   version does not attempt reclaim/compaction at all and is by default used
 *   in page fault path, while the non-light is used by khugepaged.
 */
#define GFP_ATOMIC      (__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#define GFP_KERNEL      (__GFP_RECLAIM | __GFP_IO | __GFP_FS)
#define GFP_KERNEL_ACCOUNT (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT      (__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO        (__GFP_RECLAIM)
#define GFP_NOFS        (__GFP_RECLAIM | __GFP_IO)
#define GFP_TEMPORARY   (__GFP_RECLAIM | __GFP_IO | __GFP_FS |  \
                         __GFP_RECLAIMABLE)
#define GFP_USER        (__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_DMA         __GFP_DMA
#define GFP_DMA32       __GFP_DMA32
#define GFP_HIGHUSER    (GFP_USER | __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE    (GFP_HIGHUSER | __GFP_MOVABLE)
#define GFP_TRANSHUGE_LIGHT     ((GFP_HIGHUSER_MOVABLE | __GFP_COMP |   \
                                  __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM)
#define GFP_TRANSHUGE   (GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)

/* Convert GFP flags to their corresponding migrate type */
#define GFP_MOVABLE_MASK (__GFP_RECLAIMABLE|__GFP_MOVABLE)
#define GFP_MOVABLE_SHIFT 3


#endif
