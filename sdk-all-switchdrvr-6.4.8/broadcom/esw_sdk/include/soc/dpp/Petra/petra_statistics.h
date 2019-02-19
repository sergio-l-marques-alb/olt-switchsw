/* $Id: soc_petra_statistics.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PETRA_STATISTICS_INCLUDED__
/* { */
#define __SOC_PETRA_STATISTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_api_statistics.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define  SOC_PETRA_STAT_PARAMS_HAS_THIS_FLAVOR(params, print_level) \
  (SOC_SAND_GET_BITS_RANGE(params, 5,5) && (print_level == SOC_PETRA_STAT_PRINT_LEVEL_ERROR_ONLY) ) || \
  ( (!(SOC_SAND_GET_BITS_RANGE(params, 4,3) == 0)) && (print_level == SOC_PETRA_STAT_PRINT_LEVEL_PACKET_WALKTHROUGH) ) || \
  ( (!(SOC_SAND_GET_BITS_RANGE(params, 5,3) == 0)) && (print_level == SOC_PETRA_STAT_PRINT_LEVEL_ALL) )

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  /*
  * 64 bit counter
  * Note:
  *Cleared* after having been reported.
  */
  SOC_SAND_64CNT     counter ;
  /*
  * Last physical Device-Counter.
  * To be used for:
  * 1. Debugging.
  * 2. Operational: When the register is NOT auto-clear.
  */
  SOC_SAND_U64  last_device_counter;

} SOC_PETRA_COUNTER;
/* $Id: soc_petra_statistics.h,v 1.5 Broadcom SDK $
 * End Packet counters private information.}
 */

typedef struct
{
  /*
   * Device id of the chip to read from
   */
  int         unit;
  /*
   * User supplied buffer to write to data from the
   * chip into it.
   */
  SOC_SAND_64CNT*          result_counter;
  /*
   * The 'pkt_cnt_type' specify which counter the user
   * wishes to count.
   */
  SOC_PETRA_STAT_COUNTER_TYPE  pkt_cnt_type;
  /*
   * the transaction_id of the requested action, necessary for
   * killing the job later
   */
  uint32         soc_sand_tcm_callback_id;
  /*
   * user supplied parameter, that is passed back, to help the
   * user to distinguish between the different callbacks.
   */
  uint32         user_callback_id;
  /*
   * user supplied parameter. The user callback method to call
   * if the data read from the chip changed.
   */
  SOC_SAND_USER_CALLBACK_FUNC  user_callback_func;
} SOC_PETRA_STAT_PKT_CNT_CALLBACK;

typedef struct
{
  SOC_PETRA_STAT_COUNTER_TYPE type;
  /*
   * Short name of counter
   */
  const char      *short_name;
  /*
   * Long name of counter (with explanation)
   */
  const char      *long_name;
  /*
   * Device offset of the counter
   */
  SOC_PETRA_REG_FIELD *field;

  uint32 indirect_addr;
 /*
  * bit wise field :
  *  - 0:0 -indirect 1- direct.
  *  - 1-2: Number of words 0-3
  *  - 3-4: level from 0-3
  */
  uint32  params;


} SOC_PETRA_COUNTER_INFO;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 * Packet counters private information.
 * This information is get updated by,
 * and get read by 'soc_petra_collect_pkt_counters()'.
 * the counters that will be used by the direct call to
 * soc_petra_counter_get
 * the counters get zeroed after every direct call
 */
extern
  SOC_PETRA_COUNTER
    soc_petra_direct_counters[SOC_SAND_MAX_DEVICE][SOC_PETRA_NOF_COUNTER_TYPES];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_stat_is_relevant_counter(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE    counter_id,
    SOC_SAND_OUT uint8                 *is_cnt_relevant
  );


/*********************************************************************
* NAME:
*   soc_petra_stat_ifp_cnt_select_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Incoming FAP Ports for which the number of
*   words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*   SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info -
*     The Incoming FAP Ports for which the counters are
*     enabled.
* REMARKS:
*   The counters are read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_ifp_cnt_select_get_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Incoming FAP Ports for which the number of
*   words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*   SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
* REMARKS:
*   The counters are read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_ifp_cnt_select_set_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Incoming FAP Ports for which the number of
*   words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*   SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info -
*     The Incoming FAP Ports for which the counters are
*     enabled.
* REMARKS:
*   The counters are read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IFP_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_ifp_cnt_select_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Incoming FAP Ports for which the number of
*   words is counted (SOC_PETRA_IDR_RECEIVED_PKT_PORT_0_CNT -
*   SOC_PETRA_IDR_RECEIVED_PKT_PORT_3_CNT).
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_PETRA_STAT_IFP_SELECT_INFO *info -
*     The Incoming FAP Ports for which the counters are
*     enabled.
* REMARKS:
*   The counters are read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_ifp_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IFP_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_voq_cnt_select_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VOQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info -
*     The Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VOQ programmable counter. A programmable
*   counter for Virtual Statistics Queue can be set/read
*   using soc_petra_stat_voq_cnt_select_set
*   /soc_petra_itm_voq_counter_read APIs
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_voq_cnt_select_get_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VOQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VOQ programmable counter. A programmable
*   counter for Virtual Statistics Queue can be set/read
*   using soc_petra_stat_voq_cnt_select_set
*   /soc_petra_itm_voq_counter_read APIs
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_voq_cnt_select_set_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VOQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info -
*     The Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VOQ programmable counter. A programmable
*   counter for Virtual Statistics Queue can be set/read
*   using soc_petra_stat_voq_cnt_select_set
*   /soc_petra_itm_voq_counter_read APIs
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_voq_cnt_select_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VOQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_PETRA_STAT_VOQ_SELECT_INFO *info -
*     The Ingress Queue (VOQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VOQ programmable counter. A programmable
*   counter for Virtual Statistics Queue can be set/read
*   using soc_petra_stat_voq_cnt_select_set
*   /soc_petra_itm_voq_counter_read APIs
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_voq_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VOQ_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_vsq_cnt_select_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VSQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info -
*     The Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VSQ programmable counter. A programmable
*   counter for VOQ can be set using
*   soc_petra_stat_voq_cnt_select_set API
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_vsq_cnt_select_get_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VSQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VSQ programmable counter. A programmable
*   counter for VOQ can be set using
*   soc_petra_stat_voq_cnt_select_set API
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_get_verify(
    SOC_SAND_IN  int                  unit
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_vsq_cnt_select_set_verify
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VSQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info -
*     The Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VSQ programmable counter. A programmable
*   counter for VOQ can be set using
*   soc_petra_stat_voq_cnt_select_set API
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_stat_vsq_cnt_select_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Selects the Ingress Queue (VSQ) for which the number of
*   enqueued/dequeued packets is counted (not including
*   discarded packets). Optionally an enable-mask can be
*   supplied to count groups of queues.
* INPUT:
*   SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT SOC_PETRA_STAT_VSQ_SELECT_INFO *info -
*     The Ingress Queue (VSQ) for which the number of
*     enqueued/dequeued packets is counted and an optional
*     enable-mask.
* REMARKS:
*   1. The counter is read/printed by the regular counter
*   read/print APIs of the soc_petra_stat module. 2. This API
*   sets the VSQ programmable counter. A programmable
*   counter for VOQ can be set using
*   soc_petra_stat_voq_cnt_select_set API
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_stat_vsq_cnt_select_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_STAT_VSQ_SELECT_INFO *info
  );

uint32
  soc_petra_stat_pkt_counters_collect_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE  counter_type
  );

uint32
  soc_petra_stat_counter_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_COUNTER_TYPE  counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                *result_ptr
  );

uint32
  soc_petra_stat_all_counters_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_STAT_PRINT_LEVEL              print_level,
    SOC_SAND_IN  uint32                            sampling_period,
    SOC_SAND_OUT SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS   *all_statistic_counters
  );

/*****************************************************
*NAME
*  soc_petra_get_pkt_cnt_callback
*TYPE:
*  PROC
*DATE:
*  23/OCT/2002
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32          *buffer -
*      This is actually a SOC_PETRA_PKT_CNT_CALLBACK_STRUCT
*      pointer. see above for the definition of this structure.
*    uint32          size    -
*       sizeof(SOC_PETRA_PKT_CNT_CALLBACK_STRUCT)
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32     -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not SOC_PETRA_NO_ERR some error occurred
*  SOC_SAND_INDIRECT:
*    the chip is read, information is loaded to the user
*    supplied buffer, and the user supplied function is called.
*REMARKS:
*  This is a private callback used by the API service
*  soc_petra_get_pkt_XXXXX_cnt()
*  if the API call requested for a polling job, this is the
*  callback being registered for that job. It does the actual
*  reading from the chip, and calls the user supplied method
*  if the read buffer contain different content, then what
*  was already at the user supplied buffer.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_stat_pkt_cnt_callback_get(
    SOC_SAND_INOUT  uint32     *buffer,
    SOC_SAND_OUT    uint32     size
  );

/*****************************************************
 * See details in soc_petra_statistics.c
 *****************************************************/
uint32
  soc_petra_stat_statistics_module_initialize(
    SOC_SAND_IN int    unit
  );

/*****************************************************
 * See details in soc_petra_statistics.c
 *****************************************************/
uint32
  soc_petra_stat_statistics_device_initialize(
    SOC_SAND_IN  int    unit
  );

/*****************************************************
 * See details in soc_petra_statistics.c
 *****************************************************/
uint32
  soc_petra_stat_statistics_device_finalize(
    SOC_SAND_IN  int    unit
  );

/*****************************************************
 * See details in soc_petra_statistics.c
 *****************************************************/
uint32
  soc_petra_stat_statistics_deferred_counter_clear(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN SOC_PETRA_STAT_COUNTER_TYPE counter_type
  );

uint32
  soc_petra_stat_counter_info_get(
    SOC_SAND_OUT SOC_PETRA_COUNTER_INFO  *counters[SOC_PETRA_NOF_COUNTER_TYPES]
  );

/**********************************************
 *    Test Functions: {
 **********************************************/
#if SOC_PETRA_DEBUG

/*
 * Example for user soc_petra_get_pkt_XX callback function
 */

SOC_SAND_RET
  soc_petra_stat_pkt_user_callback_get(
    SOC_SAND_IN    uint32       user_callback_id,
    SOC_SAND_INOUT uint32       *buff,
    SOC_SAND_OUT   uint32       **new_buff,
    SOC_SAND_IN    uint32       buff_size,
    SOC_SAND_IN    uint32       service_ex,
    SOC_SAND_IN    int       unit,
    SOC_SAND_IN    uint32       driver_callback_id,
    SOC_SAND_IN    uint32       meaningless_01,
    SOC_SAND_IN    uint32       meaningless_02
  );

SOC_PETRA_COUNTER_INFO* soc_petra_stat_counter_info_ptr_get(void);

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_STATISTICS_INCLUDED__*/
#endif
