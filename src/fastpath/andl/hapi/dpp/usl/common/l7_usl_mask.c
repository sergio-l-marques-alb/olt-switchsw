/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_mask.c
*
* @purpose    Mask utilites
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "string.h"
#include "l7_usl_mask.h"


#define L7_MASK_IDENTIFIER (L7_uint32)0x43375417
typedef struct
{
  L7_uint32 maskIdentifier;
  L7_uint32 size;
  L7_uint32 numElements;
  L7_uint32 count;
  L7_uchar8 *bitmap;

} l7_mask_t;

/*********************************************************************
* @purpose  Create a mask of the size indicated
*
* @param    numElements  @{(input)}  The number of elements in the mask
* @param    handle       @{(output)} The handle for all future accesses to the mask
*
* @returns  L7_SUCCESS if the mask is created
* @returns  L7_ERROR   if the mask is not created
*
* @end
*********************************************************************/
L7_RC_t l7_mask_create(L7_uint32 numElements,l7_mask_handle_t *handle)
{
  L7_uint32 maskSize = 0;
  l7_mask_t *mask;
  L7_uchar8 *maskBytes;
  L7_RC_t rc = L7_ERROR;

  do
  {
    /* not going to create a mask of size 0 */
    if (numElements == 0) break; /* goto while 0 ) */

    /* mask size is the number of octets necessary to hold the bitmap */
    maskSize = ( numElements / 8 ) + 1;

    mask = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(l7_mask_t) + maskSize);

    if (mask == L7_NULLPTR) break; /* goto while 0 ) */

    memset((void *)mask,0,sizeof(l7_mask_t) + maskSize);

    maskBytes = (L7_uchar8 *)mask;
    mask->numElements = numElements;
    mask->size        = maskSize;
    mask->maskIdentifier = L7_MASK_IDENTIFIER;
    mask->count       = 0;
    mask->bitmap      = (L7_uchar8 *)(maskBytes + sizeof(l7_mask_t));

    /* 
     * return the pointer to the mask as a handle, all future requests on this
     * mask must provide the handle
     */
    *handle = (l7_mask_handle_t)mask;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Free the memory associated with the mask
*
* @param    handle    @{(inout)}    The mask to free
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     The handle is modified to an invalid handle
*
* @end
*********************************************************************/
L7_RC_t l7_mask_delete(l7_mask_handle_t *handle)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)*handle;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    osapiFree(L7_DRIVER_COMPONENT_ID, mask);

    *handle = 0;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;

}

/*********************************************************************
* @purpose  Copies the content of one mask to another
*
* @param    src    @{(input)}  The source mask
* @param    dst    @{(input)}  The destination mask
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     The destination mask must be a valid mask of the same size
* @end
*********************************************************************/
L7_RC_t l7_mask_copy(l7_mask_handle_t src, l7_mask_handle_t dst)
{
  L7_RC_t rc = L7_ERROR;
  l7_mask_t *dstMask, *srcMask;

  do
  {
    dstMask = (l7_mask_t *)dst;
    srcMask = (l7_mask_t *)src;

    if ((dstMask == L7_NULLPTR) || (srcMask == L7_NULLPTR)) break;

    if (dstMask->maskIdentifier != L7_MASK_IDENTIFIER) break;

    if (srcMask->maskIdentifier != L7_MASK_IDENTIFIER) break;

    if ((srcMask->numElements != dstMask->numElements) &&
        (srcMask->size != dstMask->size)) break;

    memcpy((void*)dstMask->bitmap,(void *)srcMask->bitmap, srcMask->size);

    dstMask->count = srcMask->count;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Zero out all the elements of a mask
*
* @param    handle    @{(input)}    The mask to clear
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_clear(l7_mask_handle_t handle)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    memset((void *)mask->bitmap,0,mask->size);

    mask->count = 0;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;

}

/*********************************************************************
* @purpose  Test a mask to see if it is zero
*
* @param    handle    @{(input)}    The mask to operate on
* @param    isZero    @{(output)}   The result (L7_TRUE if mask is zero)
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_is_zero(l7_mask_handle_t handle, L7_BOOL *isZero)
{
  L7_BOOL   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    if (isZero == L7_NULL) break;

    *isZero = (mask->count == 0) ? L7_TRUE : L7_FALSE;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Test to see if two masks are equal
*
* @param    handle    @{(input)}    The mask to operate on
* @param    handle2   @{(input)}    The mask to operate on
* @param    isEqual   @{(output)}   The result (L7_TRUE if equal)
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_is_equal(l7_mask_handle_t handle, l7_mask_handle_t handle2,L7_BOOL *isEqual) 
{
  L7_BOOL   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  l7_mask_t *mask2 = (l7_mask_t*)handle2;
  L7_uint32 i;

  do
  {
    if (mask == L7_NULLPTR)  break; /* goto while 0 ) */
    if (mask2 == L7_NULLPTR) break; /* goto while 0 ) */

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */
    if (mask2->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */
    if (mask2->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    if (mask->size != mask2->size) break; /* goto while 0 ) */

    if (isEqual == L7_NULLPTR) break; /* goto while 0 ) */

    *isEqual = L7_TRUE;

    for (i = 0; i < mask->size; i++ )
    {
      if (mask->bitmap[i] != mask2->bitmap[i])
      {
        *isEqual = L7_FALSE;
        break;
      }
    }

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Invert the contents of the mask
*
* @param    handle    @{(input)}  The mask to operate on
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_invert(l7_mask_handle_t handle)
{
  L7_RC_t rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 i;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    for (i = 0; i < mask->size; i++ ) 
      mask->bitmap[i] = ~mask->bitmap[i];

    mask->count = mask->numElements - mask->count;
    
    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Sets a bit in the mask
*
* @param    handle    @{(input)}  The mask to operate on
* @param    bit       @{(input)}  The bit to be set
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     The bit is 1 based
* @end
*********************************************************************/
L7_RC_t l7_mask_bit_set(l7_mask_handle_t handle,L7_uint32 bit)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 bytePos, bitPos;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if ((bit > mask->numElements) || (bit == 0)) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */


    bytePos = (bit - 1) / 8;
    bitPos  = (bit - 1) % 8;

    mask->bitmap[bytePos] |= (1 << bitPos);

    mask->count++;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;

}

/*********************************************************************
* @purpose  Clears a bit in the mask
*
* @param    handle    @{(input)}  The mask to operate on
* @param    bit       @{(input)}  The bit to be clear
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     The bit is 1 based
* @end
*********************************************************************/
L7_RC_t l7_mask_bit_clear(l7_mask_handle_t handle,L7_uint32 bit)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 bytePos, bitPos;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if ((bit > mask->numElements) || (bit == 0)) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    bytePos = (bit - 1) / 8;
    bitPos  = (bit - 1) % 8;

    mask->bitmap[bytePos] &= ~(1 << bitPos);

    mask->count--;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}


/*********************************************************************
* @purpose  Test whether a bit is set
*
* @param    handle    @{(input)}    The mask to operate on
* @param    bit       @{(input)}    The bit to be tested
* @param    status    @{(output)}   The result of the test
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_is_bit_set(l7_mask_handle_t handle, L7_uint32 bit, L7_BOOL *isSet)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 bytePos, bitPos;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if ((bit > mask->numElements) || (bit == 0)) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    bytePos = (bit - 1) / 8;
    bitPos  = (bit - 1) % 8;

    if (mask->bitmap[bytePos] & (1 << bitPos))
      *isSet = L7_TRUE;
    else
      *isSet = L7_FALSE;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Finds the next element in the mask
*
* @param    handle    @{(input)}  The mask to operate on
* @param    bit       @{(input)}  The starting point 
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     bit should actually be one less than the first element you want to 
*           check.  Bit should be zero in order to check the first element of mask
* @end
*********************************************************************/
L7_RC_t l7_mask_next_get(l7_mask_handle_t handle,L7_uint32 *pBit)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 bytePos, bitPos;
  L7_uint32 myBit;
  L7_BOOL   foundNext = L7_FALSE;

  do
  {
    if (pBit == L7_NULLPTR) break;

    /* increment the bit to the first element to be checked */
    myBit = *pBit;
    myBit++; 

    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (myBit > mask->numElements)
    {
      *pBit = USL_MASK_NOT_FOUND;
      break; /* goto while 0 ) */
    }

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    bytePos = (myBit - 1) / 8;
    bitPos  = (myBit - 1) % 8;

    /* see if anything is in the present byte */
    while ( bitPos < 8 )
    {
      if (mask->bitmap[bytePos] & (1 << bitPos))
      {
        foundNext = L7_TRUE;
        break;
      }
      bitPos++;
    }

    if (foundNext == L7_FALSE)
    {
      bytePos++;

      for (;bytePos < mask->size; bytePos++)
      {
        if (mask->bitmap[bytePos])
        {
          bitPos = 0;

          /* see if anything is in the present byte */
          while ( bitPos < 8 )
          {
            if (mask->bitmap[bytePos] & (1 << bitPos))
            {
              foundNext = L7_TRUE;
              break;
            }
            bitPos++;
          }
        }
        if (foundNext == L7_TRUE) break;
      }
    }

    if (foundNext == L7_TRUE)
    {
      myBit = (bytePos * 8 ) + (bitPos);
      myBit++;
      *pBit = myBit;
    }
    else
      *pBit = USL_MASK_NOT_FOUND;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;

}



/*********************************************************************
* @purpose  Finds the first free element in the mask
*
* @param    handle    @{(input)}  The mask to operate on
* @param    bit       @{(input)}  The starting point 
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @note     bit should actually be one less than the first element you want to 
*           check.  Bit should be zero in order to check the first element of mask
* @end
*********************************************************************/
L7_RC_t l7_mask_first_free_get(l7_mask_handle_t handle,L7_uint32 *pBit)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 bytePos, bitPos;
  L7_uint32 myBit;
  L7_BOOL   foundFirst = L7_FALSE;

  do
  {
    if (pBit == L7_NULLPTR) break;

    /* increment the bit to the first element to be checked */
    myBit = *pBit;
    myBit++; 

    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (myBit > mask->numElements)
    {
      *pBit = USL_MASK_NOT_FOUND;
      break; /* goto while 0 ) */
    }

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    bytePos = (myBit - 1) / 8;
    bitPos  = (myBit - 1) % 8;

    /* see if anything is in the present byte */
    while ( bitPos < 8 )
    {
      if ( 0 == (mask->bitmap[bytePos] & (1 << bitPos)) )
      {
        foundFirst = L7_TRUE;
        break;
      }
      bitPos++;
    }

    if (foundFirst == L7_FALSE)
    {
      bytePos++;

      for (;bytePos < mask->size; bytePos++)
      {
        if (mask->bitmap[bytePos] != 0xff)
        {
          bitPos = 0;

          /* see if anything is in the present byte */
          while ( bitPos < 8 )
          {
            if ( 0== (mask->bitmap[bytePos] & (1 << bitPos)))
            {
              foundFirst = L7_TRUE;
              break;
            }
            bitPos++;
          }
        }
        if (foundFirst == L7_TRUE) break;
      }
    }

    if (foundFirst == L7_TRUE)
    {
      myBit = (bytePos * 8 ) + (bitPos);
      myBit++;
      *pBit = myBit;
    }
    else
      *pBit = USL_MASK_NOT_FOUND;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;

}


/*********************************************************************
* @purpose  Print a mask for debugging
*
* @param    handle    @{(input)}    The mask to print
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
L7_RC_t l7_mask_print(l7_mask_handle_t handle)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 i;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    sysapiPrintf("bit %d - (hex) ",mask->size * 8);
    for (i=1; i <= mask->size; i++)
    {
      sysapiPrintf("%0.2x ",mask->bitmap[mask->size - i]);

      if ((i % 32) == 0) sysapiPrintf(" bit %d\n",(mask->size - i) * 8);
    }
    sysapiPrintf(" bit 1 \n");

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Write a mask to a buffer
*
* @param    handle    @{(input)}    The mask to print
* @param    buf       @{(output)}   Buffer to written to
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    The buffer should be able to hold l7_mask_sizeof() bytes
*
* @end
*********************************************************************/
L7_RC_t l7_mask_buffer_write(l7_mask_handle_t handle,L7_uchar8 *buf)
{
  L7_RC_t   rc = L7_ERROR;
  l7_mask_t *mask = (l7_mask_t*)handle;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */

    if (buf == L7_NULLPTR) break;

    memcpy(buf,mask,sizeof(l7_mask_t) + mask->size);

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Get the sizeof (in bytes) a MASK
*
* @param    handle    @{(input)}    The mask to print
*
* @returns  The size of the mask in bytes, or zero
*
* @end
*********************************************************************/
L7_uint32 l7_mask_sizeOf(l7_mask_handle_t handle)
{
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 size = 0;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    if (mask->bitmap == L7_NULLPTR) break; /* goto while 0 ) */
    
    size = sizeof(l7_mask_t) + mask->size;

  } while ( 0 );

  return size;
}

/*********************************************************************
* @purpose  Read a mask from a buffer
*
* @param    handle    @{(input)}    The mask to print
* @param    buf       @{(output)}   Buffer to written to
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    The mask is part of the buffer, the buffer can't be freed until the mask
*           is no longer needed.  The user may use l7_mask_copy if the mask needs
*           to be persistent
*
* @end
*********************************************************************/
L7_RC_t l7_mask_buffer_read(L7_uchar8 *buf,l7_mask_handle_t *handle)
{
  l7_mask_t *tmpMask;
  L7_RC_t   rc = L7_ERROR;

  do
  {
    if (buf == L7_NULLPTR) break;

    tmpMask = (l7_mask_t*)buf;

    if (tmpMask->maskIdentifier != L7_MASK_IDENTIFIER) break;

    tmpMask->bitmap = buf + sizeof(l7_mask_t);

    *handle = (l7_mask_handle_t)tmpMask;

    rc = L7_SUCCESS;

  } while ( 0 );

  return rc;
}

/*********************************************************************
* @purpose  Get the number of set elements in the mask
*
* @param    handle    @{(input)}    The mask 
*
* @returns  the number of elements in the mask
*
* @end
*********************************************************************/
L7_uint32 l7_mask_number_set_get(l7_mask_handle_t handle)
{
  l7_mask_t *mask = (l7_mask_t*)handle;
  L7_uint32 count = 0;

  do
  {
    if (mask == L7_NULLPTR) break;

    if (mask->maskIdentifier != L7_MASK_IDENTIFIER) break; /* goto while 0 ) */

    count = mask->count;

  } while ( 0 );

  return count;
}
