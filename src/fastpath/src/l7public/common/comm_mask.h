/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename comm_mask.h
*
* @purpose The purpose of this file is to have a central location for
*          the common interface mask macros and relevant #defines
*          to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 5/16/2002
*
* @author rjindal
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/


#ifndef INCLUDE_COMM_MASK
#define INCLUDE_COMM_MASK


#include "l7_common.h"


/*--------------------------------------*/
/*  Common Mask Macros & Defines        */
/*  Generic Mask Macros & Defines        */
/*--------------------------------------*/

/*
 * Note that these macros are for one-based masks, not zero-based.
 */

typedef L7_uchar8 L7_mask_values_t;

/* Number of entries per mask unit */
#define L7_MASK_UNIT                    (sizeof(L7_mask_values_t) * 8)

/*
 *
 * Number of elements of L7_mask_values_t in mask.
 *
 * Declare a mask using this macro where _size is the number of bits to be
 * used, starting with bit 1.
 *
 * E.g., for a mask for interfaces:
 *
 * L7_mask_values_t myMask[L7_MASK_LEN(L7_MAX_INTERFACE_COUNT)];
 *
 */
#define L7_MASK_LEN(_size)              ((((_size) - 1) / L7_MASK_UNIT) + 1)

/*
 * NONZEROMASK returns 1 if any bit in word mask of NUM length
 * is turned-on.  The result, 1 or 0 is stored in result.
 */
#define L7_NONZEROMASK(_mask, _result, _size)             \
  do                                                      \
  {                                                       \
    L7_uint32 _i_;                                        \
    L7_mask_values_t *_p_ = (L7_mask_values_t *)&(_mask); \
                                                          \
    (_result) = 0;                                        \
    for (_i_ = 0; _i_ < L7_MASK_LEN(_size); _i_++)        \
    {                                                     \
      if (_p_[_i_] != 0)                                  \
      {                                                   \
        (_result) = 1;                                    \
        break;                                            \
      }                                                   \
    }                                                     \
  } while (0)

/* Least significant bit is lowest interface # */

/* SETMASKBIT turns on bit index # k in mask j. */
#define L7_SETMASKBIT(j, k)                                          \
  ((j)[((k) - 1) / L7_MASK_UNIT] |= 1 << (((k) - 1) % L7_MASK_UNIT))

/* CLRMASKBIT turns off bit index # k in mask j. */
#define L7_CLRMASKBIT(j, k)                                          \
  ((j)[((k) - 1) / L7_MASK_UNIT] &= ~(1 << (((k)-1) % L7_MASK_UNIT)))

/* MASKEQ sets mask j equal to mask k. */
#define L7_MASKEQ(j, k, _size)                                       \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] = (k)[x];                                               \
    }                                                                \
  } while (0)

/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define L7_MASKOREQ(j, k, _size)                                     \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] |= (k)[x];                                              \
    }                                                                \
  } while (0)

/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define L7_MASKEXOREQ(j, k, _size)                                   \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] ^= (k)[x];                                              \
    }                                                                \
  } while (0)

/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define L7_MASKANDEQ(j, k, _size)                                    \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] &= (k)[x];                                              \
    }                                                                \
  } while (0)

/* MASKINV inverts the bits in mask j. */
#define L7_MASKINV(j, _size)                                         \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] = ~((j)[x]);                                            \
    }                                                                \
  } while (0)

/* MASKANDEQINV turns on the bits in mask j that are on in both mask j
   and the bitwise-inverse of mask k. */
#define L7_MASKANDEQINV(j, k, _size)                                 \
  do                                                                 \
  {                                                                  \
    L7_uint32 x;                                                     \
                                                                     \
    for (x = 0; x < L7_MASK_LEN(_size); x++)                         \
    {                                                                \
      (j)[x] &= ~((k)[x]);                                           \
    }                                                                \
  } while (0)

/* FHMASKBIT finds the index of the most-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define L7_FHMASKBIT(j, k, _size)                                    \
  do                                                                 \
  {                                                                  \
    L7_int32 x;                                                      \
                                                                     \
    for (x = (L7_MASK_LEN(_size) - 1); x >= 0; x--)                  \
    {                                                                \
      if ( (j)[x] )                                                  \
      {                                                              \
        break;                                                       \
      }                                                              \
    }                                                                \
                                                                     \
    k = 0;                                                           \
    if (x >= 0)                                                      \
    {                                                                \
      L7_int32 i;                                                    \
      for (i = L7_MASK_UNIT - 1; i >= 0; i--)                        \
      {                                                              \
        if ( (j)[x] & (1 << i))                                      \
        {                                                            \
          k = i + 1 + (x * L7_MASK_UNIT);                            \
          break;                                                     \
        }                                                            \
      }                                                              \
    }                                                                \
  } while (0)

/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define L7_ISMASKBITSET(j, k)                                        \
  ((j)[(((k) - 1) / L7_MASK_UNIT)] & ( 1 << ((k-1) % L7_MASK_UNIT)))

/* MASKREV reverses the bits in within each byte of mask j. */
#define L7_MASKREV(j, _size)                     \
  do                                             \
  {                                              \
    L7_uint32 x,y;                               \
    L7_uchar8 b=0;                               \
                                                 \
    for (x = 0; x < L7_MASK_LEN(_size); x++)     \
    {                                            \
      b = 0;                                     \
      for (y=0; y<8; y++)                        \
      {                                          \
        if ((j).value[x] & (0x80 >> y))          \
        {                                        \
          b |= 0x1 << y;                         \
        }                                        \
      }                                          \
      (j).value[x] = b;                          \
    }                                            \
  } while (0)

/*--------------------------------------*/
/*  Interface Mask Macros & Defines     */
/*--------------------------------------*/

/* Number of entries per mask byte */
#define L7_INTF_MASK_UNIT               (sizeof(L7_uchar8) * 8)

/* Number of bytes in mask */
#define L7_INTF_INDICES   ((L7_MAX_INTERFACE_COUNT - 1) / L7_INTF_MASK_UNIT + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8   value[L7_INTF_INDICES];
} L7_INTF_MASK_t;


/*
 * NONZEROMASK returns 1 if any bit in word mask of NUM length
 * is turned-on.  The result, 1 or 0 is stored in result.
 */
#define L7_INTF_NONZEROMASK(mask, result){                           \
    L7_uint32 _i_;                                                   \
    L7_INTF_MASK_t *_p_;                                             \
                                                                     \
    _p_ = (L7_INTF_MASK_t *)&mask;                                   \
    for(_i_ = 0; _i_ < L7_INTF_INDICES; _i_++)                       \
        if(_p_->value[_i_] != 0){                                    \
            result = 1;                                              \
            break;                                                   \
        }                                                            \
        else                                                         \
            result = 0;                                              \
}


/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */

/* SETMASKBIT turns on bit index # k in mask j. */
#define L7_INTF_SETMASKBIT(j, k)                                     \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))
                         

/* CLRMASKBIT turns off bit index # k in mask j. */
#define L7_INTF_CLRMASKBIT(j, k)                                     \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                 \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))
                        

/* MASKEQ sets mask j equal to mask k. */
#define L7_INTF_MASKEQ(j, k) {                                       \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                (j).value[x] = (k).value[x];                         \
        }                                                            \
}


/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define L7_INTF_MASKOREQ(j, k) {                                     \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                (j).value[x] |= (k).value[x];                        \
        }                                                            \
}


/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define L7_INTF_MASKEXOREQ(j, k) {                                   \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                j.value[x] ^= k.value[x];                            \
        }                                                            \
}


/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define L7_INTF_MASKANDEQ(j, k) {                                    \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                (j).value[x] &= (k).value[x];                            \
        }                                                            \
}


/* MASKINV inverts the bits in mask j. */
#define L7_INTF_MASKINV(j) {                                         \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                j.value[x] = ~(j.value[x]);                          \
        }                                                            \
}


/* MASKANDEQINV turns on the bits in mask j that are on in both mask j
   and the bitwise-inverse of mask k. */
#define L7_INTF_MASKANDEQINV(j, k) {                                 \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                (j).value[x] &= ~((k).value[x]);                         \
        }                                                            \
}


/* FHMASKBIT finds the index of the most-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define L7_INTF_FHMASKBIT(j, k) {                                    \
        L7_int32 x;                                                  \
                                                                     \
        for (x = (L7_INTF_INDICES - 1); x >= 0; x--) {               \
                if ( j.value[x] )                                    \
                        break;                                       \
        };                                                           \
        k = 0;                                                       \
        if (x >= 0) {                                                \
/* This is for i960
      asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(j.value[x])); */ \
/* This is not */                                                    \
                L7_int32 i;                                          \
                for (i = 7; i >= 0; i--) {                           \
                    if ( j.value[x] & (1 << i)) {                    \
                       k = i + 1 + (x * (8 * sizeof(L7_uchar8)));    \
                       break;                                        \
                    }                                                \
                }                                                    \
/* End non-i960 */                                                   \
        };                                                           \
}

/* FLMASKBIT finds the index of the least-significant bit turned-on in
   mask j and returns that index in k.  Since this is a 1-based
   mask, 0 is returned for "no bits set". */
#define L7_INTF_FLMASKBIT(j, k) {                                    \
        L7_int32 x;                                                  \
                                                                     \
        for (x = 0; x <= (L7_INTF_INDICES - 1); x++) {               \
                if ( j.value[x] )                                    \
                        break;                                       \
        };                                                           \
        k = 0;                                                       \
        if (x < L7_INTF_INDICES) {                                   \
/* This is for i960
      asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(j.value[x])); */ \
/* This is not */                                                    \
                L7_int32 i;                                          \
                for (i = 0; i <= 7; i++) {                           \
                    if ( j.value[x] & (1 << i)) {                    \
                       k = i + 1 + (x * (8 * sizeof(L7_uchar8)));    \
                       break;                                        \
                    }                                                \
                }                                                    \
/* End non-i960 */                                                   \
        };                                                           \
}


/*  returns 0 if the byte k is not set in mask j */
#define L7_INTF_ISMASKBYTESET(j, k)                                   \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))])

/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define L7_INTF_ISMASKBITSET(j, k)                                   \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                    \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )
                         

/* MASKREV reverses the bits in within each byte of mask j. */
#define L7_INTF_MASKREV(j) {                                         \
        L7_uint32 x,y;                                               \
        L7_uchar8 b=0;                                               \
                                                                     \
        for (x = 0; x < L7_INTF_INDICES; x++) {                      \
                b = 0;                                               \
                for (y=0; y<8; y++)                                  \
                {                                                    \
                  if (j.value[x] & (0x80 >> y))                      \
                    b |= 0x1 << y;                                   \
                }                                                    \
                j.value[x] = b;                                      \
        }                                                            \
}



/* Macro definitions for VLAN MASK operations */
/*
 * NONZEROMASK returns 1 if any bit in word mask of NUM length
 * is turned-on.  The result, 1 or 0 is stored in result.
 */
#define L7_VLAN_NONZEROMASK(mask, result){                           \
    L7_uint32 _i_;                                                   \
    L7_VLAN_MASK_t *_p_;                                             \
                                                                     \
    _p_ = (L7_VLAN_MASK_t *)&mask;                                   \
    for(_i_ = 0; _i_ < L7_VLAN_INDICES; _i_++)                       \
        if(_p_->value[_i_] != 0){                                    \
            result = 1;                                              \
            break;                                                   \
        }                                                            \
        else                                                         \
            result = 0;                                              \
}


/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */

/* SETMASKBIT turns on bit index # k in mask j. */
#define L7_VLAN_SETMASKBIT(j, k)                                     \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))


/* CLRMASKBIT turns off bit index # k in mask j. */
#define L7_VLAN_CLRMASKBIT(j, k)                                     \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                 \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))
#define L7_VLAN_CLRMASKBIT_POINTER(j, k)                                     \
           ((j)->value[((k-1)/(8*sizeof(L7_uchar8)))]                 \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))

/* MASKEQ sets mask j equal to mask k. */
#define L7_VLAN_MASKEQ(j, k) {                                       \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                (j).value[x] = (k).value[x];                         \
        }                                                            \
}


/* MASKOREQ or's on the bits in mask j that are on in either mask j or k. */
#define L7_VLAN_MASKOREQ(j, k) {                                     \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                (j).value[x] |= (k).value[x];                        \
        }                                                            \
}


/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define L7_VLAN_MASKEXOREQ(j, k) {                                   \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                j.value[x] ^= k.value[x];                            \
        }                                                            \
}


/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define L7_VLAN_MASKANDEQ(j, k) {                                    \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                j.value[x] &= k.value[x];                            \
        }                                                            \
}


/* MASKINV inverts the bits in mask j. */
#define L7_VLAN_MASKINV(j) {                                         \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                j.value[x] = ~(j.value[x]);                          \
        }                                                            \
}


/* MASKANDEQINV turns on the bits in mask j that are on in both mask j
   and the bitwise-inverse of mask k. */
#define L7_VLAN_MASKANDEQINV(j, k) {                                 \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                j.value[x] &= ~(k.value[x]);                         \
        }                                                            \
}
#define L7_VLAN_MASKANDEQINV_POINTER(j, k) {                                 \
        L7_uint32 x;                                                 \
                                                                     \
        for (x = 0; x < L7_VLAN_INDICES; x++) {                      \
                j->value[x] &= ~(k.value[x]);                         \
        }                                                            \
}



/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define L7_VLAN_ISMASKBITSET(j, k)                                   \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]                    \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )

#define L7_VLAN_ISMASKBITSET_POINTER(j, k)                                   \
        ((j)->value[((k-1)/(8*sizeof(L7_uchar8)))]                    \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )


#endif
