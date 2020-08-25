/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim_mask.h
*
* @purpose Utilities for managing various mask/set data structures.
*
* @component Routing Logical Interface Manager
*
* @comments Assumes bit numbering is zero-based and from least to
*           most significant bit.
*
* @create 02/21/2005
*
* @author eberge
* @end
*
**********************************************************************/

#ifndef INCLUDE_RLIM_MASK_H
#define INCLUDE_RLIM_MASK_H

/*
 * Mask array element type
 */
#define RLIM_MASK_ELEM_TYPE L7_uchar8

/*
 * Number of bits in an array element
 */
#define RLIM_MASK_ELEM_BITS (8*sizeof(RLIM_MASK_ELEM_TYPE))

/*
 * Return the "invalid" bit value for a given mask type
 */
#define RLIM_MASK_INVALID_BIT(mask) \
        (sizeof((mask).value)*RLIM_MASK_ELEM_BITS)

/*
 * NONZERO returns true if any bit in word mask of NUM length
 * is turned-on.  The result, TRUE or FALSE is stored in
 * result.
 */
#define RLIM_MASK_NONZERO(mask, result) {                             \
        L7_uint32 _i_;                                                \
        RLIM_MASK_ELEM_TYPE *_p_;                                     \
                                                                      \
        _p_ = &(mask).value[0];                                       \
        for(_i_ = 0; _i_ < sizeof((mask).value); _i_++) {             \
            if (_p_[_i_]) {                                           \
                result = L7_TRUE;                                     \
                break;                                                \
            }                                                         \
            else {                                                    \
                result = L7_FALSE;                                    \
            }                                                         \
        }                                                             \
}

/* SETBIT turns on bit index # k in mask */
#define RLIM_MASK_SETBIT(mask, k)                                     \
        ((mask).value[((k)/RLIM_MASK_ELEM_BITS)]                      \
              |= 1 << ((k) % RLIM_MASK_ELEM_BITS))

/* CLRBIT turns off bit index # k in mask */
#define RLIM_MASK_CLRBIT(mask, k)                                     \
        ((mask).value[((k)/RLIM_MASK_ELEM_BITS)]                      \
              &= ~(1 << ((k) % RLIM_MASK_ELEM_BITS)))

/* ISBITSET returns 0 if the interface k is not set in mask */
#define RLIM_MASK_ISBITSET(mask, k)                                   \
        ((mask).value[((k)/RLIM_MASK_ELEM_BITS)]                      \
              & (1 << ((k) % RLIM_MASK_ELEM_BITS)))

/* FHBIT finds the index of the most-significant bit turned-on in
   mask and returns that index in k. */
#define RLIM_MASK_FHBIT(mask, k) {                                    \
        L7_int32 _x_;                                                 \
                                                                      \
        for (_x_ = sizeof((mask).value)-1; _x_ >= 0; _x_--) {         \
          if ( (mask).value[_x_] )                                    \
            break;                                                    \
        }                                                             \
        if (_x_ >= 0) {                                               \
/* This is for i960
          asm volatile ("scanbit %1,%0" : "=d"(k) : "d"(mask.value[_x_])); */ \
/* This is not */                                                     \
          L7_uint32 _i_;                                              \
          for (_i_ = RLIM_MASK_ELEM_BITS-1; _i_ >= 0; _i_--) {        \
            if ( (mask).value[_x_] & (1 << _i_)) {                    \
              (k) = _i_;                                              \
              break;                                                  \
            }                                                         \
          }                                                           \
/* End non-i960 */                                                    \
          (k) = ((k) + (_x_ * RLIM_MASK_ELEM_BITS));                  \
        } else {                                                      \
          (k) = RLIM_MASK_INVALID_BIT(mask);                          \
        }                                                             \
}

/* FLBIT finds the index of the least-significant bit turned-on in
   mask and returns that index in k. */
#define RLIM_MASK_FLBIT(mask, k) {                                    \
        L7_int32 _x_;                                                 \
                                                                      \
        for (_x_ = 0; _x_ < sizeof((mask).value); _x_++) {            \
          if ( (mask).value[_x_] )                                    \
            break;                                                    \
        }                                                             \
        if (_x_ < sizeof((mask).value)) {                             \
          L7_uint32 _i_;                                              \
          for (_i_ = 0; _i_ < RLIM_MASK_ELEM_BITS; _i_++) {           \
            if ( (mask).value[_x_] & (1 << _i_)) {                    \
              (k) = _i_;                                              \
              break;                                                  \
            }                                                         \
          }                                                           \
          (k) = ((k) + (_x_ * RLIM_MASK_ELEM_BITS));                  \
        } else {                                                      \
          (k) = RLIM_MASK_INVALID_BIT(mask);                          \
        }                                                             \
}


#endif /* !defined(INCLUDE_RLIM_MASK_H) */
