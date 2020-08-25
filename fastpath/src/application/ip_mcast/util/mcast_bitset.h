/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mcast_bitset.h
*
* @purpose    Generic BIT manipulations
*
* @component  Multicast
*
* @comments   Following are the tips to use this bit manipulation library
*                 1) Define maximum number of bits
*                       #define MAX_NUM_NBRS 256   
* 
*                 2) Define your data type something like below. 
*                       typedef struct intf_bitset_s
*                       {
*                          L7_uchar8 bits[MCAST_BITX_NUM_BITS(MAX_NUM_NBRS)];
*                       } nbr_bitset_t;
*
*                      Note : The data type should be L7_uchar8 & make use of 
*                             MCAST_BITX_NUM_BITS to compute the array size.
*
*                 3) Invoke the macros as in the following code snippet
*                      {
*                          nbr_bitset_t a1, b1, c1, *k;
*                          L7_BOOL      rc;
*
*                          MCAST_BITX_IS_EMPTY (c1.bits, rc);
*                          MCAST_BITX_OR (a1.bits, b1.bits, c1.bits);
*                          MCAST_BITX_SET (k->bits, 5);
*                      }
*
*                      Note: Please ensure that array name ("bits" in our case)
*                            is always passed to the MACROs
*
* @create     05/05/2006
*
* @author     dsatyanarayana
*
* @end
**********************************************************************/
#ifndef _MCAST_BITSET_H_
#define _MCAST_BITSET_H_

#define MCAST_BITX_NBITS (sizeof (L7_uchar8) * 8)	/* No of bits per array entry */

/*********************************************************************
* @purpose  Compute the number of bytes required to represent the bits
*
* @param    n      @b{(input)}  Total number of bits.
*
* @returns  Number ]of bytes (L7_uchar8 type) required to represent the bits
*
* @comments Refer to the comments section in the File Header for usage.
*       
* @end
*********************************************************************/
#define MCAST_BITX_NUM_BITS(n) (((n)+MCAST_BITX_NBITS-1)/MCAST_BITX_NBITS)

/*********************************************************************
* @purpose  Set a specific bit in a bitmap
*
* @param    p      @b{(inout)}  Bitmap to operate on.
* @param    n      @b{(input)}  Bit Index.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
*       
* @end
*********************************************************************/
#define MCAST_BITX_SET(p, n) ((p)[(n)/MCAST_BITX_NBITS] |=       \
                            (1 << ((n) % MCAST_BITX_NBITS)))

/*********************************************************************
* @purpose  Clear/Reset a specific bit in a bitmap
*
* @param    p      @b{(inout)}  Bitmap to operate on.
* @param    n      @b{(input)}  Bit Index.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
*       
* @end
*********************************************************************/
#define MCAST_BITX_RESET(p, n) ((p)[(n)/MCAST_BITX_NBITS] &=     \
                            ~(1 << ((n) % MCAST_BITX_NBITS)))



/******************************************************************************
* @purpose  Reset all bits.
*
* @param    p      @b{(input)}  Source bitmap.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
*
* @end
******************************************************************************/
#define MCAST_BITX_RESET_ALL(p)                                  \
  do                                                             \
  {                                                              \
    memset(p, 0, sizeof(p));                                     \
  } while (0)

/*********************************************************************
* @purpose  Check if a specific bit in a bitmap is set
*
* @param    p      @b{(inout)}  Bitmap to check.
* @param    n      @b{(input)}  Bit Index.
*
* @returns  0      Bit 'n' is not set.
* @returns  non-0  Bit 'n' is set.
*
* @comments Refer to the comments section in the File Header for usage.
*       
* @end
*********************************************************************/
#define MCAST_BITX_TEST(p, n) ((p)[(n)/MCAST_BITX_NBITS] &       \
                            (1 << ((n) % MCAST_BITX_NBITS)))

/*********************************************************************
* @purpose  Test if a bitmap is Empty
*
* @param    p      @b{(input)}  Bitmap to test.
* @param    result @b{(output)} L7_TRUE if Empty, L7_FALSE otherwise.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
*       
* @end
*********************************************************************/
#define MCAST_BITX_IS_EMPTY(p, result)                            \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    (result) = L7_TRUE;                                           \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      if((p)[x] != 0)                                             \
      {                                                           \
        (result) = L7_FALSE;                                      \
        break;                                                    \
      }                                                           \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Logically OR bitmaps.
*
* @param    p      @b{(input)}  First bitmap in the OR operation.
* @param    q      @b{(input)}  Second bitmap in the OR operation.
* @param    r      @b{(output)} Resultant bitmap after the OR operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p, q & r are all of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_OR(p, q, r)                                    \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = (p)[x] | (q)[x];                                   \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Logically XOR bitmaps.
*
* @param    p      @b{(input)}  First bitmap in the XOR operation.
* @param    q      @b{(input)}  Second bitmap in the XOR operation.
* @param    r      @b{(output)} Resultant bitmap after the XOR operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p, q & r are all of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_XOR(p, q, r)                                   \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = (p)[x] ^ (q)[x];                                   \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Logically AND bitmaps.
*
* @param    p      @b{(input)}  First bitmap in the AND operation.
* @param    q      @b{(input)}  Second bitmap in the AND operation.
* @param    r      @b{(output)} Resultant bitmap after the AND operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p, q & r are all of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_AND(p, q, r)                                   \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = (p)[x] & (q)[x];                                   \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Logically Negate a bitmap.
*
* @param    p      @b{(input)}  Bitmap to operate on.
* @param    r      @b{(output)} Resultant bitmap after the Negate operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p & r are both of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_NEG(p, r)                                      \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = ~(p)[x];                                           \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  ADD two bitmaps.
*
* @param    p      @b{(input)}  First bitmap to the ADD operation.
* @param    q      @b{(input)}  Second bitmap to the ADD operation.
* @param    r      @b{(output)} Resultant bitmap after the ADD operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments The ADD operation simply collects all the bits in both the given
*           bitmaps. In this respect it is identical to the OR operation.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p, q & r are all of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_ADD   MCAST_BITX_OR

/*********************************************************************
* @purpose  SUBTRACT one bitmap from another.
*
* @param    p      @b{(input)}  Bitmap to SUBTRACT from.
* @param    q      @b{(input)}  Bitmap to be SUBTRACTED.
* @param    r      @b{(output)} Resultant bitmap after the SUBTRACT operation.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments The SUBTRACT operation simply removes bits from the first bitmap
*           that are also in the second bitmap. In this respect it is identical
*           to "p & ~q" operation.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p, q & r are all of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_SUBTRACT(p, q, r)                              \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = (p)[x] & ~(q)[x];                                  \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Copy a bitmap.
*
* @param    p      @b{(input)}  Source bitmap.
* @param    r      @b{(output)} Destination bitmap.
*
* @returns  None
*
* @comments Refer to the comments section in the File Header for usage.
* @comments It is the caller's responsibility to ensure that the bitmaps
*           p & r are both of the same size.
*
* @end
*********************************************************************/
#define MCAST_BITX_COPY(p, r)                                     \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
      (r)[x] = (p)[x];                                            \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Print a bitmap.
*
* @param    p      @b{(input)}  Source bitmap.
* @param    r      @b{(output)} Destination bitmap.
*
* @returns  None
*
* @comments Use of this macro is recommended only for debug purposes.
*
* @end
*********************************************************************/
#define MCAST_BITX_DEBUG_PRINT(p)                                 \
  do                                                              \
  {                                                               \
    L7_uint32 x;                                                  \
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n(p) = ");             \
    for (x = 0; x < sizeof(p); x++)                               \
    {                                                             \
     SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "%x ", (p)[x]);         \
    }                                                             \
  } while (0)

/*********************************************************************
* @purpose  Compare the bitset
*
* @param    p      @b{(input)}  Source bitmap.
* @param    r      @b{(output)} Destination bitmap.
*
* @returns  None
*
* @comments 
*
* @end
*********************************************************************/
#define MCAST_BITX_COMPARE(p,q)  memcmp(p,q,sizeof(p)) 

#endif /* _MCAST_BITSET_H_ */

