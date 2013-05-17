/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2004-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/util/util_keying.c
 * @purpose   Private functions to implement Keying for Advanced Functions
 * @component keying
 * @comments  none
 * @create    02/17/2004
 * @author    Suhel Goel
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "strlib_base_common.h"
#include "string.h"
#include "stdio.h"
#include "comm_mask.h"
#include "util_keying.h"

/*********************************************************************
*
* @purpose Shift left all bits in the mask by the value of component id
*
* @param ComponentMask* KeyingMask      @b{(input)}  Structure to keying feature
* @param L7_uint32      currCompId      @b{(input)}  Current Component Id
*
* @returns
*
* @notes
*
* @end
*
*********************************************************************/
void featureKeyingMaskFillLeftMostBits(ComponentMask * keyingMask, L7_uint32 currCompId)
{

  L7_uint32 whole_bytes_shifted = 0;
  L7_uint32 bits_in_byte_shifted = 0;
  L7_int32 x = 0;

  /* Bit index 1 occupies the zeroeth bit of the mask when setting the mask,
     so gracefully allow for a value of k= 0 */

  whole_bytes_shifted   = 0;
  bits_in_byte_shifted  = 0;

  if (currCompId > 0)
  {
    whole_bytes_shifted = (currCompId-1)/(8*sizeof(L7_uchar8));
    bits_in_byte_shifted = (currCompId-1) % (8*sizeof(L7_uchar8));
    bits_in_byte_shifted++;
  }

  /* Shift bits in upper indices */
  for (x = (COMPONENT_INDICES - 1); x > whole_bytes_shifted; x--)
  {
    /* Shift the higher index */
    keyingMask->value[x] = 0xFF;
  }

  /* now fill and shift the bits in the whole byte belonging to bit k */
  keyingMask->value[whole_bytes_shifted] = 0xFF;
  keyingMask->value[whole_bytes_shifted] = (keyingMask->value[whole_bytes_shifted] << bits_in_byte_shifted);
  /* Lastly, zero the indices below the whole byte belonging to bit k */
  for (x = (whole_bytes_shifted - 1); x >= 0; x--)
  {
    keyingMask->value[x] = 0;
  }
}

/*********************************************************************
*
* @purpose Find the next component id
*
* @param ComponentMask* KeyingMask      @b{(input)}  Structure to keying feature
* @param L7_uint32*     nextCompId      @b{(output)} Next Component Id
*
* @returns
*
* @notes
*
* @end
*
*********************************************************************/
void featureKeyingMaskFLMaskBit(ComponentMask * keyingMask, L7_uint32 * nextCompId)
{

  L7_uint32 counter = 0;
  L7_uint32 counter_bits = 0;
  L7_uint32 next_comp_id = 0;

  for (counter = 0; counter <= (COMPONENT_INDICES - 1);  counter++)
  {
    if ( keyingMask->value[counter] )
    {
      break;
    }
  }

  if (counter <= (COMPONENT_INDICES-1))
  {
    for (counter_bits=0; counter_bits <= 7; counter_bits++)
    {
      if ( keyingMask->value[counter] & (1 << counter_bits))
      {
        next_comp_id = counter_bits;
        break;
      }
    }
  }
  /* Calculate the bit offset, remembering that each index is a byte */
  /* Ensure a 1-based value is returned */
  next_comp_id = (next_comp_id + (counter*8) + 1 );
  *nextCompId = next_comp_id;

}
