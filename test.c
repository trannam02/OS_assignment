#include <stdio.h>

#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#define BITS_PER_BYTE           8
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define BIT(nr)                 (1U << (nr))
#define BIT_ULL(nr)             (1ULL << (nr))
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)

#define BITS_TO_LONGS(nr)       DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
	(((~0U) << (l)) & (~0U >> (BITS_PER_LONG  - (h) - 1)))

#define NBITS2(n) ((n&2)?1:0)
#define NBITS4(n) ((n&(0xC))?(2+NBITS2(n>>2)):(NBITS2(n)))
#define NBITS8(n) ((n&0xF0)?(4+NBITS4(n>>4)):(NBITS4(n)))
#define NBITS16(n) ((n&0xFF00)?(8+NBITS8(n>>8)):(NBITS8(n)))
#define NBITS32(n) ((n&0xFFFF0000)?(16+NBITS16(n>>16)):(NBITS16(n)))
#define NBITS(n) (n==0?0:NBITS32(n))

#define EXTRACT_NBITS(nr, h, l) ((nr&GENMASK(h,l)) >> l)
/* CPU Bus definition */
#define PAGING_CPU_BUS_WIDTH 22 /* 22bit bus - MAX SPACE 4MB */
#define PAGING_PAGESZ  256      /* 256B or 8-bits PAGE NUMBER */
#define PAGING_MEMRAMSZ BIT(10) /* 1MB */
#define PAGING_PAGE_ALIGNSZ(sz) (DIV_ROUND_UP(sz,PAGING_PAGESZ)*PAGING_PAGESZ)

#define PAGING_MEMSWPSZ BIT(14) /* 16MB */
#define PAGING_SWPFPN_OFFSET 5  
#define PAGING_MAX_PGN  (DIV_ROUND_UP(BIT(PAGING_CPU_BUS_WIDTH),PAGING_PAGESZ))

#define PAGING_SBRK_INIT_SZ PAGING_PAGESZ
/* PTE BIT */
#define PAGING_PTE_PRESENT_MASK BIT(31) 
#define PAGING_PTE_SWAPPED_MASK BIT(30)
#define PAGING_PTE_RESERVE_MASK BIT(29)
#define PAGING_PTE_DIRTY_MASK BIT(28)
#define PAGING_PTE_EMPTY01_MASK BIT(14)
#define PAGING_PTE_EMPTY02_MASK BIT(13)

/* PTE BIT PRESENT */
#define PAGING_PTE_SET_PRESENT(pte) (pte=pte|PAGING_PTE_PRESENT_MASK)
#define PAGING_PTE_PAGE_PRESENT(pte) (pte&PAGING_PTE_PRESENT_MASK)

/* USRNUM */
#define PAGING_PTE_USRNUM_LOBIT 15
#define PAGING_PTE_USRNUM_HIBIT 27
/* FPN */
#define PAGING_PTE_FPN_LOBIT 0
#define PAGING_PTE_FPN_HIBIT 12
/* SWPTYP */
#define PAGING_PTE_SWPTYP_LOBIT 0
#define PAGING_PTE_SWPTYP_HIBIT 4
/* SWPOFF */
#define PAGING_PTE_SWPOFF_LOBIT 5
#define PAGING_PTE_SWPOFF_HIBIT 25

/* PTE masks */
#define PAGING_PTE_USRNUM_MASK GENMASK(PAGING_PTE_USRNUM_HIBIT,PAGING_PTE_USRNUM_LOBIT)
#define PAGING_PTE_FPN_MASK    GENMASK(PAGING_PTE_FPN_HIBIT,PAGING_PTE_FPN_LOBIT)
#define PAGING_PTE_SWPTYP_MASK GENMASK(PAGING_PTE_SWPTYP_HIBIT,PAGING_PTE_SWPTYP_LOBIT)
#define PAGING_PTE_SWPOFF_MASK GENMASK(PAGING_PTE_SWPOFF_HIBIT,PAGING_PTE_SWPOFF_LOBIT)

/* Extract PTE */
#define PAGING_PTE_OFFST(pte) GETVAL(pte,PAGING_OFFST_MASK,PAGING_ADDR_OFFST_LOBIT)
#define PAGING_PTE_PGN(pte)   GETVAL(pte,PAGING_PGN_MASK,PAGING_ADDR_PGN_LOBIT)
#define PAGING_PTE_FPN(pte)   GETVAL(pte,PAGING_PTE_FPN_MASK,PAGING_PTE_FPN_LOBIT)
#define PAGING_PTE_SWP(pte)   GETVAL(pte,PAGING_PTE_SWPOFF_MASK,PAGING_SWPFPN_OFFSET)

/* OFFSET */
#define PAGING_ADDR_OFFST_LOBIT 0
#define PAGING_ADDR_OFFST_HIBIT (NBITS(PAGING_PAGESZ) - 1)

/* PAGE Num */
#define PAGING_ADDR_PGN_LOBIT NBITS(PAGING_PAGESZ)
#define PAGING_ADDR_PGN_HIBIT (PAGING_CPU_BUS_WIDTH - 1)

/* Frame PHY Num */
#define PAGING_ADDR_FPN_LOBIT NBITS(PAGING_PAGESZ)
#define PAGING_ADDR_FPN_HIBIT (NBITS(PAGING_MEMRAMSZ) - 1)

/* SWAPFPN */
#define PAGING_SWP_LOBIT NBITS(PAGING_PAGESZ)
#define PAGING_SWP_HIBIT (NBITS(PAGING_MEMSWPSZ) - 1)

/* Value operators */
#define SETBIT(v,mask) (v=v|mask)
#define CLRBIT(v,mask) (v=v&~mask)

#define SETVAL(v,value,mask,offst) (v=(v&~mask)|((value<<offst)&mask))
#define GETVAL(v,mask,offst) ((v&mask)>>offst)

/* Other masks */
#define PAGING_OFFST_MASK  GENMASK(PAGING_ADDR_OFFST_HIBIT,PAGING_ADDR_OFFST_LOBIT)
#define PAGING_PGN_MASK  GENMASK(PAGING_ADDR_PGN_HIBIT,PAGING_ADDR_PGN_LOBIT)
#define PAGING_FPN_MASK  GENMASK(PAGING_ADDR_FPN_HIBIT,PAGING_ADDR_FPN_LOBIT)
#define PAGING_SWP_MASK  GENMASK(PAGING_SWP_HIBIT,PAGING_SWP_LOBIT)

/* Extract OFFSET */
//#define PAGING_OFFST(x)  ((x&PAGING_OFFST_MASK) >> PAGING_ADDR_OFFST_LOBIT)
#define PAGING_OFFST(x)  GETVAL(x,PAGING_OFFST_MASK,PAGING_ADDR_OFFST_LOBIT)
/* Extract Page Number*/
#define PAGING_PGN(x)  GETVAL(x,PAGING_PGN_MASK,PAGING_ADDR_PGN_LOBIT)
/* Extract FramePHY Number*/
#define PAGING_FPN(x)  GETVAL(x,PAGING_FPN_MASK,PAGING_ADDR_FPN_LOBIT)
/* Extract SWAPFPN */
#define PAGING_PGN(x)  GETVAL(x,PAGING_PGN_MASK,PAGING_ADDR_PGN_LOBIT)
/* Extract SWAPTYPE */
#define PAGING_FPN(x)  GETVAL(x,PAGING_FPN_MASK,PAGING_ADDR_FPN_LOBIT)

/* Memory range operator */
/* TODO implement the INCLUDE checking mechanism - currently dummy op only */
#define INCLUDE(x1,x2,y1,y2) (0)
/* TODO implement the OVERLAP checking mechanism - currently dummy op only */
#define OVERLAP(x1,x2,y1,y2) (1)

int main(){
    return 0;
};
