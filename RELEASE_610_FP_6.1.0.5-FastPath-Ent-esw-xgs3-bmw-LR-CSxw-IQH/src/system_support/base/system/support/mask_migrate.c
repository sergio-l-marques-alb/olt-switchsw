/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename mask_migrate.c
*
* @purpose Mask Migration Helper routines
*
* @component NIM
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <string.h>
#include "defaultconfig.h"

static void *maskUtilSema = L7_NULLPTR;

extern L7_RC_t maskShiftRightBits (L7_uchar8 * j, L7_int32 k, L7_int32 m);
extern L7_RC_t maskShiftLeftBits (L7_uchar8 * j, L7_int32 k, L7_int32 m);
extern L7_RC_t maskOrBits (L7_uchar8 * j, L7_uchar8 * k, L7_int32 m);
extern L7_RC_t maskAndBits (L7_uchar8 * j, L7_uchar8 * k, L7_int32 m);
extern L7_RC_t maskClearBit (L7_uchar8 * j, L7_int32 k, L7_int32 m);
extern void maskDebugBitMaskAllocate (L7_int32 m);

 /* NOTE:  Before making this a utility, check Endianness and direction of the
    highest mask bits.    Check with Eric Lund as the direction of 
    left and right here is somewhat suspect.  It could simply be that we
    storing our mask data in a counter-intuitive manner.  Note that SNMP
    needs to flip our bits around to deal with data  */
#define L7_MASK_LIMIT   0xFFFF

#define L7_MASK_INDICES   ((L7_MASK_LIMIT - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Mask storage */
typedef struct
{
  L7_uchar8 value[L7_MASK_INDICES];
} L7_MASK_t;

static L7_MASK_t mask_src;
static L7_MASK_t mask_dest;

L7_RC_t maskUtilInit (void)
{
  maskUtilSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Shift bits in mask j right by k bits
*
* @param    *j              pointer to bit mask
* @param    k               bit index
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskShiftRightBits (L7_uchar8 * j, L7_int32 k, L7_int32 m)
{

  L7_int32 whole_bytes_shifted;
  L7_int32 bits_in_byte_shifted;
  L7_int32 x, y;
  L7_uchar8 _temp_;
  L7_uchar8 working_byte;
  L7_int32 mask_indices;

  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  if (k > m)
    return L7_FAILURE;

  /* Protect data */

  (void) osapiSemaTake (maskUtilSema, L7_WAIT_FOREVER);

  /* Initialize this actual mask's mask info */
  mask_indices = ((m - 1) / (sizeof (L7_uchar8) * 8) + 1);

  memset ((void *) &mask_src, 0, sizeof (L7_MASK_t));
  memset ((void *) &mask_dest, 0, sizeof (L7_MASK_t));

  /* Initialize the working copy of the mask */
  for (x = 0; x < mask_indices; x++)
  {
    mask_src.value[x] = ((L7_MASK_t *) j)->value[x];
  }

  /* Bit index 1 occupies the zeroeth bit of the mask when setting the mask, 
     so gracefully allow for a value of k= 0 */

  whole_bytes_shifted = 0;
  bits_in_byte_shifted = 0;


  if (k > 0)
  {

    whole_bytes_shifted = (k - 1) / (8 * sizeof (L7_uchar8));
    bits_in_byte_shifted = ((k - 1) % (8 * sizeof (L7_uchar8)));

    /* The above yields a zero-based result.  To accomodate shift, add 1 */
    bits_in_byte_shifted++;
  }



  /* Shift all of the whole bytes down to the appropriate byte entry towards the end */
  y = 0;
  for (x = (whole_bytes_shifted); x < mask_indices; x++)
  {

    /* Shift the higher index */
    mask_dest.value[x] = mask_src.value[y];
    y++;
  };


  /* Now, do the necessary bit shifting within the bytes that were shifted to the end */

  /* Shift bits in upper indices t */
  for (x = (mask_indices - 1); x >= (whole_bytes_shifted); x--)
  {
    /* Save the bits in the lower index which must be shifted  */
    /*   into the higher index. */
    _temp_ = (mask_dest.value[x - 1] >> (8 - bits_in_byte_shifted));

    /* Shift the higher index */
    working_byte = mask_dest.value[x];
    mask_dest.value[x] = (working_byte << bits_in_byte_shifted);

    /*  OR in the bytes from the lower index such that they are now in
       the same bye */
    mask_dest.value[x] |= _temp_;
  };

  /* now shift the bits in the partial byte above the first whole byte in the shift */

  /*
     working_byte =  mask_dest.value[whole_bytes_shifted];
     mask_dest.value[whole_bytes_shifted] = (working_byte >> bits_in_byte_shifted); 
   */
  /* Note that the bits in the lower indices are effectively zeroed by the memset
     of mask_dest, so nothing needs to be done for them */



  /* Update the ultimate destination mask j */
  /* We did not touch mask j in this cycle of the existence of this routine.
     Optimization can be made when this routine is moved to the mask utility. */


  for (x = 0; x < mask_indices; x++)
  {
    ((L7_MASK_t *) j)->value[x] = mask_dest.value[x];
  }

  /* Return sema */

  (void) osapiSemaGive (maskUtilSema);

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Shift bits in mask j left by k bits
*
* @param    *j              pointer to bit mask
* @param    k               bit index
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskShiftLeftBits (L7_uchar8 * j, L7_int32 k, L7_int32 m)
{

  L7_int32 whole_bytes_shifted;
  L7_int32 bits_in_byte_shifted;
  L7_int32 x, y;
  L7_uchar8 _temp_;
  L7_uchar8 working_byte;
  L7_int32 mask_indices;



  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  if (k > m)
    return L7_FAILURE;

  /* Protect data */

  (void) osapiSemaTake (maskUtilSema, L7_WAIT_FOREVER);

  /* Initialize this actual mask's mask info */
  mask_indices = ((m - 1) / (sizeof (L7_uchar8) * 8) + 1);

  memset ((void *) &mask_src, 0, sizeof (L7_MASK_t));
  memset ((void *) &mask_dest, 0, sizeof (L7_MASK_t));

  /* Initialize the working copy of the mask */
  for (x = 0; x < mask_indices; x++)
  {
    mask_src.value[x] = ((L7_MASK_t *) j)->value[x];
  }

  /* Bit index 1 occupies the zeroeth bit of the mask when setting the mask, 
     so gracefully allow for a value of k= 0 */

  whole_bytes_shifted = 0;
  bits_in_byte_shifted = 0;


  if (k > 0)
  {

    whole_bytes_shifted = (k - 1) / (8 * sizeof (L7_uchar8));
    bits_in_byte_shifted = ((k - 1) % (8 * sizeof (L7_uchar8)));

    /* The above yields a zero-based result.  To accomodate shift, add 1 */
    bits_in_byte_shifted++;
  }


  /* Step 1 */
  /* Copy all of the whole bytes down to the appropriate byte entry towards the end */

  y = whole_bytes_shifted;
  if (bits_in_byte_shifted == 8)
  {
    /* Ensure that shifts which align on a byte boundary are not missed */
    y++;
  }
  for (x = 0; x < mask_indices; x++)
  {

    if (y == mask_indices)
      break;

    /* Shift the higher index */
    mask_dest.value[x] = mask_src.value[y];
    y++;
  };



  /* Step 2 */
  /* Now, do the necessary bit shifting within the bytes that were shifted to the end */

  /* Shift bits in upper indices  */

  y = mask_indices - whole_bytes_shifted;
  for (x = 0; x < y; x++)
  {
    /* Save the bits in the lower index which must be shifted  */
    /*   into the higher index. */
    _temp_ = (mask_dest.value[x] >> (bits_in_byte_shifted));
    /* Shift the higher index */
    working_byte = mask_dest.value[x + 1];
    mask_dest.value[x] = (working_byte << (8 - bits_in_byte_shifted));

    /*  OR in the bytes from the lower index such that they are now in
       the same bye */
    mask_dest.value[x] |= _temp_;
  };


  /* Step 3 */
  /* now shift the bits in the partial byte below the last whole byte in the shift */


  y = mask_indices - whole_bytes_shifted;
  working_byte = mask_dest.value[y];
  mask_dest.value[y] = (working_byte << bits_in_byte_shifted);

  /* Note that the bits in the upper indices are effectively zeroed by the memset
     of mask_dest, so nothing needs to be done for them */



  /* Update the ultimate destination mask j */
  /* We did not touch mask j in this cycle of the existence of this routine.
     Optimization can be made when this routine is moved to the mask utility. */


  for (x = (0); x < mask_indices; x++)
  {
    ((L7_MASK_t *) j)->value[x] = mask_dest.value[x];
  }

  /* Return sema */

  (void) osapiSemaGive (maskUtilSema);

  return L7_SUCCESS;

}



/*********************************************************************
* @purpose  Or bits in masks j and k bits
*
* @param    *j              pointer to bit mask
* @param    *k              pointer to bit mask
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskOrBits (L7_uchar8 * j, L7_uchar8 * k, L7_int32 m)
{

  L7_int32 x;
  L7_int32 mask_indices;


  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  /* Protect data */

  (void) osapiSemaTake (maskUtilSema, L7_WAIT_FOREVER);

  /* Initialize this actual mask's mask info */
  mask_indices = ((m - 1) / (sizeof (L7_uchar8) * 8) + 1);

  for (x = 0; x < mask_indices; x++)
  {
    ((L7_MASK_t *) j)->value[x] |= ((L7_MASK_t *) k)->value[x];
  }

  /* Return sema */

  (void) osapiSemaGive (maskUtilSema);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Or bits in masks j and k bits
*
* @param    *j              pointer to bit mask
* @param    *k              pointer to bit mask
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskAndBits (L7_uchar8 * j, L7_uchar8 * k, L7_int32 m)
{

  L7_int32 x;
  L7_int32 mask_indices;


  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  /* Protect data */

  (void) osapiSemaTake (maskUtilSema, L7_WAIT_FOREVER);

  /* Initialize this actual mask's mask info */
  mask_indices = ((m - 1) / (sizeof (L7_uchar8) * 8) + 1);

  for (x = 0; x < mask_indices; x++)
  {
    ((L7_MASK_t *) j)->value[x] &= ((L7_MASK_t *) k)->value[x];
  }

  /* Return sema */

  (void) osapiSemaGive (maskUtilSema);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Clear bit in mask j 
*
* @param    *j              pointer to bit mask
* @param    k               bit index
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskClearBit (L7_uchar8 * j, L7_int32 k, L7_int32 m)
{

  L7_uint32 bytePos, bitPos;



  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  if (k > m)
    return L7_FAILURE;


  /* Protect data */

  (void) osapiSemaTake (maskUtilSema, L7_WAIT_FOREVER);

  bytePos = (k - 1) / 8;
  bitPos = (k - 1) % 8;


  ((L7_MASK_t *) j)->value[bytePos] &= ~(1 << bitPos);



  /* Return sema */

  (void) osapiSemaGive (maskUtilSema);

  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  Set bit in mask j 
*
* @param    *j              pointer to bit mask
* @param    k               bit index
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       The size of the bit mask (m) must be less than L7_MASK_LIMIT
* @notes       Note that by passing in a size of mask, we allow manipulation
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
L7_RC_t maskSetBit (L7_uchar8 * j, L7_int32 k, L7_int32 m)
{

  L7_uint32 bytePos, bitPos;

  /* Ensure that the size of the bit mask can be handled */
  if (m > L7_MASK_LIMIT)
    return L7_FAILURE;

  if (k > m)
    return L7_FAILURE;

  bytePos = (k - 1) / 8;
  bitPos = (k - 1) % 8;


  ((L7_MASK_t *) j)->value[bytePos] |= (1 << bitPos);

  return L7_SUCCESS;
}


/*************************************************************************/
/*************************************************************************/
/********************   START OF MASK DEBUGGING DATA *********************/
/*************************************************************************/
/*************************************************************************/

/* 
  Test tips for mask utility routines:
  
  1. Test bit manipulation both on a byte boundary and not on a byte boundary.
  
  2. Test an uneven offset
      
*/

/* 
  Sample test 1 for validity of mask utility routines:
  
  For shift left bit mask:
  
   - Given mask size of 76 bits, and instruction to shift left by 28 bytes
     
     
  Sample test mask:
  
  Mask at beginning:                         76th byte
                                            V
  Byte  0   1   2   3   4   5   6   7   8   9   A   B   C...
       ab  cd  ef  e1  e2  e3  aa  bb  cc  dd  ee  ff   F1
  
  Mask at end:
      
  Byte  0   1   2   3   4   5   6   7   8   9   10  11  ... 
       1e  2e  3a  ab  bc  d0  00  00  XX  XX  XX  XX
  
 */


/* 
  Sample test 2 for validity of mask utility routines:
  
  For shift left bit mask:
  
   - shift left by 24 bytes
      
       
     
     
  Sample test mask:
  
  Mask at beginning:
      
  Byte  0   1   2   3   4   5   6   7   8   9   A   B   
       aa  cd  ef  e1  e2  e3  aa  bb  cc  dd  ee  ff   
  
  Mask at end:
      
  Byte  0   1  2   3   4   5   6   7   8   9   A  B  ... 
       e1  e2  e3  aa  bb  cc  dd  ee  ff  00 00  00
 */

/* 
  Sample test 3 for validity of mask utility routines:
  
  Test bit mask routines for an odd numbered mask, e.g. 33 or 37 bit mask
  
 */




/*********************************************************************
* @purpose  Allocate memory to test a mask
*
* @param    m               total number of bits in mask
*
* @returns  void
*
* @notes       This routine is to provide memory for devshell testing of bit mask.
* @notes       of bits in the middle of a mask
*                                                   
* @end
*********************************************************************/
void maskDebugBitMaskAllocate (L7_int32 m)
{
  void *pBitMask;


  pBitMask = osapiMalloc (L7_SIM_COMPONENT_ID, ((m - 1) / (sizeof (L7_uchar8) * 8) + 1));

  sysapiPrintf ("Allocated memory for a bit mask of size %d bits:  Located at %x\n", m, pBitMask);

  return;
}
