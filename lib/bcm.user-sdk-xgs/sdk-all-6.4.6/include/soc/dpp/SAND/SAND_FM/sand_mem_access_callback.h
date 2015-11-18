/* $Id: sand_mem_access_callback.h,v 1.3 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/


#ifndef __SOC_SAND_MEM_ACCESS_CALLBACK_H_INCLUDED__
/* { */
#define __SOC_SAND_MEM_ACCESS_CALLBACK_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

/* $Id: sand_mem_access_callback.h,v 1.3 Broadcom SDK $
 * SOC_SAND_MEM_READ_CALLBACK_STRUCT is a structure used by
 * the API method soc_sand_mem_read_callback();
 * This structure is passed from the API method to the
 * callback method holding the specific read request information
 */
typedef struct
{
  /*
   * the unit to read from
   */
  int      unit;
  /*
   * the address of the user supplied buffer, to write the
   * information read from the chip into.
   */
  uint32     *result_ptr;
  /*
   * This buffer is for the internal use of the callback mechanism
   * we copy the last result of the callback, and call the user
   * supplied callback, only if there is a change in that result
   * The reason it's a (huge) array and a specially allocated buffer
   * for that specific buffer (result_ptr) size, is that we wouldn't
   * be able to free it, when the callback item gets kicked out of
   * the system.
   */
  uint32     copy_of_last_result_ptr[SOC_SAND_CALLBACK_BUF_SIZE>>2];
  /*
   * only after copying a value into this buffer it's contect becomes valid
   */
  uint32      copy_of_result_is_valid;
  /*
   * address, within the chip, to read from
   */
  uint32     offset;
  /*
   * the size (in bytes) of the block of information to read from
   * the chip (the read block will be [offset, offset+size]
   */
  uint32      size;
  /*
   * whether this is a direct or indirect access to the chip.
   * offset should be interpreted as a direct or indirext address
   */
  uint32      indirect;
  /*
   * the transaction_id of the requested action, necessary for
   * killing the job later
   */
  uint32     soc_sand_tcm_callback_id;
  /*
   * user supplied parameter, that is passed back, to help the
   * user to distinguish between the different callbacks.
   */
  uint32     user_callback_id;
  /*
   * user supplied parameter. The user callback method to call
   * if the data read from the chip changed.
   */
  SOC_SAND_USER_CALLBACK_FUNC  user_callback_proc;
} SOC_SAND_MEM_READ_CALLBACK_STRUCT;


/*****************************************************
*NAME
*  soc_sand_mem_read_callback
*TYPE:
*  PROC
*DATE:
*  23/OCT/2002
*FUNCTION:
*  This procedure physically reads internal chip registers
*  as per specified offset.
*  This procedure may, depending on input, result in
*  activation of specified callback when target registers
*  contents has changed.
*CALLING SEQUENCE:
*  soc_sand_mem_read_callback(buffer, size)
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32      *buffer -
*      This is actually a SOC_SAND_MEM_READ_CALLBACK_STRUCT
*      pointer. see above for the definition of this structure.
*    uint32      size    -
*       sizeof(SOC_SAND_MEM_READ_CALLBACK_STRUCT)
*  SOC_SAND_INDIRECT:
*    FE200_TRIGGER_TIMEOUT .
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FE200_NO_ERR some error occured
*  SOC_SAND_INDIRECT:
*    the chip is read, information is loaded to the user
*    supplied buffer, and the user supplied function is called.
*REMARKS:
*  static (local, private) method, unavailable to other modules.
*  This is a private callback used by the API service
*  fe200_mem_read()
*  if the API call requested for a polling job, this is the
*  callback being registered for that job. It does the actual
*  reading from the chip, and calls the user supplied method
*  if the read buffer contain different content, then what
*  was already at the user supplied buffer.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_mem_read_callback(
    uint32 *buffer,
    uint32 size
  );


#ifdef  __cplusplus
}
#endif

/* } __SOC_SAND_MEM_ACCESS_CALLBACK_H_INCLUDED__*/
#endif
