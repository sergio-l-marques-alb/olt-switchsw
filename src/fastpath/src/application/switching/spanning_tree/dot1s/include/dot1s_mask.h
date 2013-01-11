/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_mask.h
*
* @purpose   dot1s mask data structures and defines
*
* @component dot1s
*
* @comments 
*
* @create    07/01/2001
*
* @author    wjacobs
*
* @end
*             
**********************************************************************/
#ifndef DOT1S_MASK_H
#define DOT1S_MASK_H

/*-----------------------
 * Interface Mask Macros 
 *-----------------------
 */

/* Number of bytes in mask */
#define DOT1S_ACQ_INDICES   (((L7_LAST_COMPONENT_ID) - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct {
        L7_uchar8 value[DOT1S_ACQ_INDICES];
} DOT1S_ACQUIRED_MASK_t;

/* Mask of components which have "acquired" an interface */
/*typedef DOT1S_ACQUIRED_MASK_t AcquiredMask;  */

/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define DOT1S_ACQ_NONZEROMASK(mask, result){                          \
    L7_uint32 _i_;                                                    \
    DOT1S_ACQUIRED_MASK_t *_p_;                                       \
                                                                      \
    _p_ = (DOT1S_ACQUIRED_MASK_t *)&(mask);                           \
    for(_i_ = 0; _i_ < DOT1S_ACQ_INDICES; _i_++)                      \
        if(_p_ -> value[_i_]){                                        \
            result = L7_TRUE;                                         \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = L7_FALSE;                                        \
}

/* Least significant bit/rightmost bit is lowest interface # */

/* SETMASKBIT turns on bit index # k in mask j. */
#define DOT1S_ACQ_SETMASKBIT(j, k)                                    \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))   

/* CLRMASKBIT turns off bit index # k in mask j. */
#define DOT1S_ACQ_CLRMASKBIT(j, k)                                    \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))      

/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define DOT1S_ACQ_ISMASKBITSET(j, k)                               \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )  

/*****************************************************************************/
/*********************DOT1S VLAN MASK MACROS & DEFINES*********************/
/*****************************************************************************/



/* Number of bytes in mask */
#define DOT1S_VLAN_INDICES   (((L7_DOT1Q_MAX_VLAN_ID + 2) - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct {
        L7_uchar8 value[DOT1S_VLAN_INDICES];
} DOT1S_VLAN_MASK_t;

/*
 * NONZEROMASK returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define DOT1S_VLAN_NONZEROMASK(mask, result){                         \
    L7_uint32 _i_;                                                    \
    DOT1S_VLAN_MASK_t *_p_;                                           \
                                                                      \
    _p_ = (NIM_MASK_t *)&mask;                                      \
    for(_i_ = 0; _i_ < DOT1S_VLAN_INDICES; _i_++)                     \
        if(_p_ -> value[_i_]){                                        \
            result = TRUE;                                            \
            break;                                                    \
        }                                                             \
        else                                                          \
            result = FALSE;                                           \
}



/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */

/* SETMASKBIT turns on bit index # k in mask j. */
#define DOT1S_VLAN_SETMASKBIT(j, k)                                    \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))   



/* CLRMASKBIT turns off bit index # k in mask j. */
#define DOT1S_VLAN_CLRMASKBIT(j, k)                                    \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))      
            


/* MASKEQ sets mask j equal to mask k. */
#define DOT1S_VLAN_MASKEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                (j).value[x] = (k).value[x]; \
        } \
}



/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define DOT1S_VLAN_MASKOREQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                (j).value[x] |= (k).value[x]; \
        } \
}



/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define DOT1S_VLAN_MASKEXOREQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                j.value[x] ^= k.value[x]; \
        } \
}



/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define DOT1S_VLAN_MASKANDEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                j.value[x] &= k.value[x]; \
        } \
}

/* MASKINV inverts the bits in mask j. */
#define DOT1S_VLAN_MASKINV(j) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                j.value[x] = ~(j.value[x]); \
        } \
}

/* MASKANDEQINV turns on the bits in mask j that are on in both mask j and the
   bitwise-inverse of mask k. */
#define DOT1S_VLAN_MASKANDEQINV(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < DOT1S_VLAN_INDICES; x++) { \
                j.value[x] &= ~(k.value[x]); \
        } \
}


/* FHMASKBIT finds the index of the most-significant bit turned-on in
   mask j and returns that index in k. */
#define DOT1S_VLAN_FHMASKBIT(j, k) { \
        L7_uint32 x; \
 \
        for (x = (DOT1S_VLAN_INDICES - 1); x >= 0; x--) { \
                if ( j.value[x] ) \
                        break; \
        }; \
        if (x >= 0) { \
/* This is for i960
                asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(j.value[x])); */ \
/* This is not */ \
                L7_uint32 i; \
                for (i = 7; i >= 0; i--) { \
                    if ( j.value[x] & (1 << i)) { \
                       k = i; \
                       break; \
                    } \
                } \
/* End non-i960 */ \
        } else { \
                k = 0xff; \
        }; \
        k = (k + x*(DOT1S_VLAN_INDICES*sizeof(L7_uint32))); \
}



/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define DOT1S_VLAN_ISMASKBITSET(j, k)                               \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )  


#endif /* DOT1S_MASK_H */
