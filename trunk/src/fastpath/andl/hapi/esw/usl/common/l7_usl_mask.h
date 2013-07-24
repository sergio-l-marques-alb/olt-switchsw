/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_mask.h
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

#ifndef _L7_USL_MASK_H
#define _L7_USL_MASK_H

#include "l7_common.h"

#define USL_MASK_NOT_FOUND (L7_uint32)(~0)

/* 
 * Type used to pass masks in the API
 */
typedef L7_uint32 l7_mask_handle_t;

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
extern L7_RC_t l7_mask_create(L7_uint32 numElements,l7_mask_handle_t *handle);

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
extern L7_RC_t l7_mask_delete(l7_mask_handle_t *handle);

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
extern L7_RC_t l7_mask_copy(l7_mask_handle_t src, l7_mask_handle_t dst);

/*********************************************************************
* @purpose  Zero out all the elements of a mask
*
* @param    handle    @{(input)}    The mask to clear
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
extern L7_RC_t l7_mask_clear(l7_mask_handle_t handle);

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
extern L7_RC_t l7_mask_is_zero(l7_mask_handle_t handle, L7_BOOL *isZero);

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
extern L7_RC_t l7_mask_is_equal(l7_mask_handle_t handle, l7_mask_handle_t handle2,L7_BOOL *isEqual); 

/*********************************************************************
* @purpose  Invert the contents of the mask
*
* @param    handle    @{(input)}  The mask to operate on
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
extern L7_RC_t l7_mask_invert(l7_mask_handle_t handle);

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
extern L7_RC_t l7_mask_bit_set(l7_mask_handle_t handle,L7_uint32 bit);

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
extern L7_RC_t l7_mask_bit_clear(l7_mask_handle_t handle,L7_uint32 bit);

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
extern L7_RC_t l7_mask_is_bit_set(l7_mask_handle_t handle, L7_uint32 bit, L7_BOOL *isSet);

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
extern L7_RC_t l7_mask_next_get(l7_mask_handle_t handle,L7_uint32 *pBit);

/*********************************************************************
* @purpose  Print a mask for debugging
*
* @param    handle    @{(input)}    The mask to print
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @end
*********************************************************************/
extern L7_RC_t l7_mask_print(l7_mask_handle_t handle);

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
extern L7_RC_t l7_mask_buffer_read(L7_uchar8 *buf,l7_mask_handle_t *handle);

/*********************************************************************
* @purpose  Get the sizeof (in bytes) a MASK
*
* @param    handle    @{(input)}    The mask to print
*
* @returns  The size of the mask in bytes, or zero
*
* @end
*********************************************************************/
extern L7_uint32 l7_mask_sizeOf(l7_mask_handle_t handle);

/*********************************************************************
* @purpose  Write a mask to a buffer
*
* @param    handle    @{(input)}    The mask to write out
* @param    buf       @{(output)}   Buffer to written to
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    The buffer should be able to hold l7_mask_sizeof() bytes
*
* @end
*********************************************************************/
extern L7_RC_t l7_mask_buffer_write(l7_mask_handle_t handle,L7_uchar8 *buf);


/*********************************************************************
* @purpose  Get the number of set elements in the mask
*
* @param    handle    @{(input)}    The mask
*
* @returns  the number of elements in the mask
*
* @end
**********************************************************************/
extern L7_uint32 l7_mask_number_set_get(l7_mask_handle_t handle);

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
L7_RC_t l7_mask_first_free_get(l7_mask_handle_t handle,L7_uint32 *pBit);
#endif
