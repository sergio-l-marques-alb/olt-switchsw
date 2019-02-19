/* $Id: soc_sand_trigger.c,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

/*****************************************************
 * See details in soc_sand_trigger.h
 *****************************************************/
/* $Id: soc_sand_trigger.c,v 1.2 Broadcom SDK $
 *  Number of busy-wait retries on trigger verify
 */
#define SOC_SAND_TRIGGER_VERIFY_0_NOF_BW_RETRY  50
/*
 *  Number of timed delay retries on trigger verify
 */
#define SOC_SAND_TRIGGER_VERIFY_0_NOF_TD_RETRY  7
#define SOC_SAND_TRIGGER_VERIFY_0_NOF_RETRY  \
  (SOC_SAND_TRIGGER_VERIFY_0_NOF_BW_RETRY + SOC_SAND_TRIGGER_VERIFY_0_NOF_TD_RETRY)
/*
 *  Before last iteration on polling trigger-down on indirect access
 */
#define SOC_SAND_TRIGGER_VERIFY_BEFORE_LAST_TRY_DELAY_MS 10
/*
 *  Last iteration on polling trigger-down on indirect access
 */
#define SOC_SAND_TRIGGER_VERIFY_LAST_TRY_DELAY_MS 1000


SOC_SAND_RET
  soc_sand_trigger_verify_0(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, /* in nano seconds */
    SOC_SAND_IN uint32  trigger_bit
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err;

  uint32
      *device_base_address ;

  device_base_address = soc_sand_get_chip_descriptor_base_addr(unit) ;
  err = 0 ;
  ex  = SOC_SAND_OK ;


  ex = soc_sand_trigger_verify_0_by_base(
           offset,
           timeout,
           trigger_bit,
           device_base_address
         );
    if (SOC_SAND_OK != ex)
    {
      err = 1 ;
      goto exit ;
    }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TRIGGER_VERIFY_0,
        "General error in soc_sand_trigger_verify_0()",err,0,0,0,0,0) ;
  return ex ;
}

/*****************************************************
 * See details in soc_sand_trigger.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_trigger_assert_1(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, /* in nano seconds */
    SOC_SAND_IN uint32  trigger_bit
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err ;
  uint32
    *device_base_address ;

  device_base_address = soc_sand_get_chip_descriptor_base_addr(unit) ;
  err = 0 ;
  ex = SOC_SAND_OK ;

  ex = soc_sand_trigger_assert_1_by_base(
         offset,
         timeout,
         trigger_bit,
         device_base_address
       );

  if (SOC_SAND_OK != ex)
  {
    err = 1 ;
    goto exit ;
  }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TRIGGER_ASSERT_1,
        "General error in soc_sand_trigger_assert_1()",err,0,0,0,0,0) ;
  return ex ;
}


/*****************************************************
 * See details in soc_sand_trigger.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_trigger_verify_0_by_base(
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, /* in nano seconds */
    SOC_SAND_IN uint32  trigger_bit,
    SOC_SAND_IN uint32  *device_base_address
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    reg = 0;
  uint32
    i ;
  uint32
    err,
    trig_mask,
    timeout_i;

  err = 0 ;
  ex  = SOC_SAND_OK ;

  reg = 0 ;
  trig_mask = SOC_SAND_BIT(trigger_bit);
  /*
   * this is timeout heuristics. Since the timeout is in nano seconds,
   * there's no much point in slicing it to many iterations,
   * but only 1 iteration, seemed not very fair, so we make it 4 times,
   * hard coded, right now, and in the future a modification may be in order.
   */
  timeout_i = timeout>>2;
  for (i = 0 ; i < SOC_SAND_TRIGGER_VERIFY_0_NOF_RETRY ; ++i)
  {
    ex = soc_sand_physical_read_from_chip(
           &reg,
           device_base_address,
           offset,
           sizeof(reg)
         ) ;
    if (SOC_SAND_OK != ex)
    {
      err = 1 ;
      goto exit ;
    }
    if ( (reg & trig_mask) != trig_mask )
    {
      /*
       * Trigger is down.
       */
      goto exit ;
    }
    if (i >= SOC_SAND_TRIGGER_VERIFY_0_NOF_BW_RETRY) /* start Time Delay */ 
    {
      if (i <= (SOC_SAND_TRIGGER_VERIFY_0_NOF_RETRY - 4)) /* */
      {
          soc_sand_os_nano_sleep(timeout_i, NULL) ;
          timeout_i *= 4;
      } 
      else if(i == (SOC_SAND_TRIGGER_VERIFY_0_NOF_RETRY - 3)) /* before last one wait SOC_SAND_TRIGGER_VERIFY_BEFORE_LAST_TRY_DELAY_MS milisec */
      {
          soc_sand_os_task_delay_milisec(SOC_SAND_TRIGGER_VERIFY_BEFORE_LAST_TRY_DELAY_MS);
      } 
      else if(i == (SOC_SAND_TRIGGER_VERIFY_0_NOF_RETRY - 2)) /* last one wait SOC_SAND_TRIGGER_VERIFY_LAST_TRY_DELAY_MS milisec */
      {
          soc_sand_os_task_delay_milisec(SOC_SAND_TRIGGER_VERIFY_LAST_TRY_DELAY_MS);
      }
    }
  }
  ex = SOC_SAND_TRG_TIMEOUT ; /* timeout */
  err = 2;
  goto exit ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TRIGGER_VERIFY_0,
        "General error in soc_sand_trigger_verify_0_by_base()",err,0,0,0,0,0) ;
  return ex ;
}

/*****************************************************
 * See details in soc_sand_trigger.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_trigger_assert_1_by_base(
    SOC_SAND_IN     uint32  offset,
    SOC_SAND_IN     uint32  timeout, /* in nano seconds */
    SOC_SAND_IN     uint32  trigger_bit,
    SOC_SAND_INOUT  uint32  *device_base_address
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    val ;
  uint32
    err ;
  /*
   */
  err = 0 ;
  ex = SOC_SAND_OK ;
  /*
   * make sure the CPU ended all the writing,
   * before triggering the indirect.
   */
  SOC_SAND_SYNC_IOS ;
  /*
   * Write the trigger bit.
   * Most of the times the trigger is: 0x00000001.
   */
  val = SOC_SAND_BIT(trigger_bit) ;

  /*
   * read-modify-write the trigger.
   */
  ex = soc_sand_read_modify_write(
         device_base_address,
         offset,
         trigger_bit,
         val,
         1
       ) ;
  if (SOC_SAND_OK != ex)
  {
    err = 1 ;
    goto exit ;
  }
  /*
   * make sure the CPU finished triggering,
   * before testing it to zero.
   */
  SOC_SAND_SYNC_IOS ;
  if(timeout > 0)
  {
    /*
     * Asked to wait for trigger to come down.
     */
    ex = soc_sand_trigger_verify_0_by_base(offset,timeout, trigger_bit,device_base_address) ;
    if (SOC_SAND_OK != ex)
    {
      err = 2 ;
      goto exit ;
    }
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TRIGGER_ASSERT_1,
        "General error in soc_sand_trigger_assert_1_by_base()",err,0,0,0,0,0) ;
  return ex ;
}
