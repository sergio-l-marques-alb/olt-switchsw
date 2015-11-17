/* $Id: sand_callback_handles.h,v 1.4 Broadcom SDK $
* $Copyright: Copyright 2012 Broadcom Corporation.
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
* $Copyright: Copyright 2012 Broadcom Corporation.
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
#ifndef SOC_SAND_CALLBACK_HANDLES_H
#define SOC_SAND_CALLBACK_HANDLES_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_delta_list.h>

/* $Id: sand_callback_handles.h,v 1.4 Broadcom SDK $
 * Handles utils, for registering and unregistering all device services
 * (both polling and interrupts).
 * {
 */
typedef struct
{
  /*
   * valid word - a mark telling us this is actually an FE_CALLBACK_HANDLE
   */
  uint32 valid_word;
  /*
   * flag indicating whether this callback weas registerd for
   * an interrupt (0) or for polling (1).
   */
  uint32  int_or_poll;
  /*
   * The device id of the service. Very useful when unregistering a whole device
   */
  int  unit;
  /*
   * The procedure id of the service. useful when services shouldn't register twice
   */
  uint32  proc_id;
  /*
   * the handle returned by the callback engine delta queue.
   */
  uint32 soc_sand_polling_handle;
  /*
   * upto 2 interrupt handles.
   */
  uint32 soc_sand_interrupt_handle;
} SOC_SAND_CALLBACK_HANDLE;
/*
 */
#define SOC_SAND_CALLBACK_HANDLE_VALID_WORD 0xBCBCBCBC


extern SOC_SAND_DELTA_LIST
    *Soc_sand_handles_list  ;


SOC_SAND_RET
  soc_sand_handles_init_handles(
    void
  );
/*
 */
SOC_SAND_RET
  soc_sand_handles_shut_down_handles(
    void
  );
/*
 */
SOC_SAND_RET
  soc_sand_handles_register_handle(
    uint32  int_or_poll,
    int  unit,
    uint32  proc_id,
    uint32 soc_sand_polling_handle,
    uint32 soc_sand_interrupt_handle,
    uint32 *public_handle
  );
/*
 */
SOC_SAND_RET
  soc_sand_handles_unregister_handle(
    uint32 fe_service_handle
  );
/*
 */
SOC_SAND_RET
  soc_sand_handles_unregister_all_device_handles(
    int unit
  );
/*
 */
SOC_SAND_CALLBACK_HANDLE
  *soc_sand_handles_search_next_handle(
    int        unit,
    uint32        proc_id,
    SOC_SAND_CALLBACK_HANDLE  *current
);
/*
 */
SOC_SAND_RET
  soc_sand_handles_is_handle_exist(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  proc_id,
    SOC_SAND_OUT uint32* callback_exist
  );


SOC_SAND_RET
  soc_sand_handles_delta_list_take_mutex(
    void
  );
SOC_SAND_RET
  soc_sand_handles_delta_list_give_mutex(
    void
  );
sal_thread_t
  soc_sand_handles_delta_list_get_owner(
    void
  );

/*
 * }
 * End of device serivecs handle utils
 */
#ifdef  __cplusplus
}
#endif
#endif
