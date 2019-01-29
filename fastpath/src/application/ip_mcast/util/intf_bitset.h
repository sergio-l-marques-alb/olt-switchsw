/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   intf_bitset.h
*
* @purpose    route interface bit set 
*
* @component  
*
* @comments   none
*
* @create     01/27/2006
*
* @author     gkiran/dsatyanarayana
*
   NOTES:
   This is not a library it is a wrapper around the 
   bitset library in mcast_bitset.h. Use this specifically
   for router interface bit mask only.
* @end
**********************************************************************/
#ifndef _INTF_BITSET_H_
#define _INTF_BITSET_H_
#include "mcast_bitset.h"

#define MAX_INTERFACES (L7_RTR_MAX_RTR_INTERFACES + 1)
#define MCAST_MAX_INTERFACES    MAX_INTERFACES
/* Interface bitset structure */
typedef struct _interface_bitset_t {
   L7_uchar8  bits[MCAST_BITX_NUM_BITS(MCAST_MAX_INTERFACES)];
} interface_bitset_t;


#define BITX_SET(p, n)  MCAST_BITX_SET((p)->bits, (n))

/* To reset the bit*/
#define BITX_RESET(p, n) MCAST_BITX_RESET((p)->bits, (n))

#define BITX_TEST(p, n) MCAST_BITX_TEST((p)->bits, (n))

#define BITX_OR(p, q, r) MCAST_BITX_OR((p)->bits, (q)->bits, (r)->bits)

#define BITX_XOR(p, q, r) MCAST_BITX_XOR((p)->bits, (q)->bits, (r)->bits)

#define BITX_AND(p, q, r) MCAST_BITX_AND((p)->bits, (q)->bits, (r)->bits)

#define BITX_MASK_AND(p, q, r)                     \
{                                                  \
  interface_bitset_t n;                            \
  MCAST_BITX_NEG((q)->bits, (&n)->bits);           \
  MCAST_BITX_AND((p)->bits, (&n)->bits, (r)->bits);\
}

#define BITX_NEG(p, r) MCAST_BITX_NEG((p)->bits, (r)->bits)

#define BITX_COPY(p, r) MCAST_BITX_COPY((p)->bits, (r)->bits)

#define BITX_DEBUG_PRINT(p) MCAST_BITX_DEBUG_PRINT((p)->bits)

#define BITX_IS_EMPTY(p, result) MCAST_BITX_IS_EMPTY((p)->bits, (result))

#define BITX_RESET_ALL(p) MCAST_BITX_RESET_ALL((p)->bits)

#define BITX_COMPARE(p, q)  MCAST_BITX_COMPARE((p)->bits, (q)->bits)

#define BITX_SUBTRACT(p, q, r) MCAST_BITX_SUBTRACT((p)->bits, (q)->bits, (r)->bits)

#define BITX_ADD(p, q, r) MCAST_BITX_ADD((p)->bits, (q)->bits, (r)->bits)

#endif /* _INTF_BITSET_H_ */
