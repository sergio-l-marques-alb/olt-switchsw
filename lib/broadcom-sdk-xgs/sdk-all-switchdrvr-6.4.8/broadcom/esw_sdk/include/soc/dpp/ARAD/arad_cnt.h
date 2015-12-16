/* $Id: arad_cnt.h,v 1.8 Broadcom SDK $
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

#ifndef __ARAD_CNT_INCLUDED__
/* { */
#define __ARAD_CNT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_api_cnt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_CNT_NOF_MODE_EG_TYPES   SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD

#define ARAD_COUNTER_NDX_MAX(unit, proc_id) \
                                ((proc_id < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) ? \
                                 (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) - 1) : \
                                 (SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor) - 1))
#define ARAD_CNT_USE_DMA                                           (1) /* If set to 0, change also _FIFO_DEPTH_MAX to 64 */
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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_cnt_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
 *   arad_cnt_dma_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the DMA for Counter Processor
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_dma_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
    SOC_SAND_IN  int                   cache_length
  );
uint32
  arad_cnt_dma_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   dma_ndx,
    SOC_SAND_IN  int                   cache_length
  );
uint32
  arad_cnt_dma_unset_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                         dma_ndx
  );
uint32
  arad_cnt_dma_unset_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         dma_ndx
  );
/*********************************************************************
* NAME:
 *   arad_cnt_counters_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Counter Processor: its counting mode and
 *   its counting source.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 *   SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info -
 *     Counter info of the counter processor
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_counters_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info
  );

uint32
  arad_cnt_counters_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info
  );

uint32
  arad_cnt_counters_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_cnt_counters_set_unsafe" API.
 *     Refer to "arad_cnt_counters_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_cnt_counters_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO        *info
  );

/*********************************************************************
* NAME:
 *   arad_cnt_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the current status of the counter processor.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 *   SOC_SAND_OUT ARAD_CNT_STATUS               *proc_status -
 *     Counter processor status info
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_status_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_STATUS               *proc_status
  );

uint32
  arad_cnt_status_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  );

/*********************************************************************
* NAME:
 *   arad_cnt_algorithmic_read_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read the counter values saved in a cache according to
 *   the polling algorithm (algorithmic method).
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 *   SOC_SAND_OUT ARAD_CNT_RESULT_ARR           *result_arr -
 *     Counter values from the first 'nof_counters' counters in
 *     the cache which contains the counters chosen by the
 *     polling algorithm.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_algorithmic_read_unsafe(
    SOC_SAND_IN  int                                          unit,
    SOC_SAND_IN  int                                          dma_ndx,
    SOC_SAND_IN  SOC_TMC_CNT_COUNTERS_INFO                   *counter_info_array,
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR                         *result_arr
  );

uint32
  arad_cnt_algorithmic_read_verify(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  int                                        dma_ndx
  );

/*********************************************************************
* NAME:
 *   arad_cnt_direct_read_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read the counter value according to the counter.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx -
 *     Counter processor index
 *   SOC_SAND_IN  uint32                    counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 *   SOC_SAND_OUT ARAD_CNT_RESULT               *read_rslt -
 *     Counter processor read result (in particular the counter
 *     values).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_direct_read_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO         *counter_info,
    SOC_SAND_IN  uint32                         counter_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT                *read_rslt
  );

uint32
  arad_cnt_direct_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                    counter_ndx
  );

/*********************************************************************
* NAME:
 *   arad_cnt_q2cnt_id_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Convert a queue index to the counter index.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   queue_ndx -
 *     Queue index. Range: 0 - 32K-1.
 *   SOC_SAND_OUT uint32                   *counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 * REMARKS:
 *   The result depends directly on the parameters set with
 *   the function arad_cnt_src_type_set. Relevant only if the
 *   counting source is the VOQs.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_q2cnt_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx,
    SOC_SAND_OUT uint32                   *counter_ndx
  );

uint32
  arad_cnt_q2cnt_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx
  );

/*********************************************************************
* NAME:
 *   arad_cnt_cnt2q_id_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Convert a counter index to the respective queue index.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   counter_ndx -
 *     Counter index. Range: 0 - 8K-1.
 *   SOC_SAND_OUT uint32                   *queue_ndx -
 *     Queue index. Range: 0 - 32K-1.
 * REMARKS:
 *   The result depends directly on the parameters set with
 *   the function arad_cnt_src_type_set. Relevant only if the
 *   counting source is the VOQs.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_cnt2q_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   counter_ndx,
    SOC_SAND_OUT uint32                   *queue_ndx
  );

uint32
  arad_cnt_cnt2q_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   counter_ndx
  );


/*********************************************************************
* NAME:
 *   arad_cnt_meter_hdr_compensation_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the header compensation factor for the counter
 *   and meter modules to enable a meter and counter
 *   processing of this packet according to its original
 *   packet size.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   port_ndx -
 *     Incoming PP port id. Range: 0 - 63.
 *   SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx -
 *     ingress/egress.
 *   SOC_SAND_IN  int32                    hdr_compensation -
 *     Header compensation value to remove from the packet
 *     size. Units: Bytes. Range: -15 - 15.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_cnt_meter_hdr_compensation_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  int32                    hdr_compensation
  );

uint32
  arad_cnt_meter_hdr_compensation_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int32                    hdr_compensation
  );

uint32
  arad_cnt_meter_hdr_compensation_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction

  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_cnt_meter_hdr_compensation_set_unsafe" API.
 *     Refer to "arad_cnt_meter_hdr_compensation_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_cnt_meter_hdr_compensation_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_OUT int32                    *hdr_compensation
  );
uint32 
    arad_cnt_lif_counting_set(
       SOC_SAND_IN int                  unit,
       SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
       SOC_SAND_IN int                  command_id,
       SOC_SAND_IN uint32               lif_counting_mask,
       SOC_SAND_IN int                  lif_stack_to_count
       );
uint32 
   arad_cnt_lif_counting_get(
      SOC_SAND_IN int                  unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
      SOC_SAND_IN int                  command_id,
      SOC_SAND_IN uint32               lif_counting_mask,
      SOC_SAND_OUT int*                 lif_stack_to_count
   );
uint32 
   arad_cnt_lif_counting_range_set(
      SOC_SAND_IN int                  unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
      SOC_SAND_IN int                  range_id,
      SOC_SAND_IN SOC_SAND_U32_RANGE*  range
   );
uint32 
   arad_cnt_lif_counting_range_get(
      SOC_SAND_IN int                  unit,
      SOC_SAND_IN SOC_TMC_CNT_SRC_TYPE source,
      SOC_SAND_IN int                  range_id,
      SOC_SAND_OUT SOC_SAND_U32_RANGE* range
      );
uint32 
   arad_cnt_base_val_set(
      SOC_SAND_IN int                       unit,
      SOC_SAND_IN SOC_TMC_CNT_PROCESSOR_ID  processor_ndx,
      SOC_SAND_IN SOC_TMC_CNT_COUNTERS_INFO *info
   );
uint32 
   arad_cnt_epni_regs_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
      SOC_SAND_IN  int                    src_core,
      SOC_SAND_IN ARAD_CNT_MODE_EG_TYPES  eg_mode_type,
      SOC_SAND_IN int                     command_id
      );
#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_CNT_MODE_EG_verify(
    SOC_SAND_IN  ARAD_CNT_MODE_EG *info
  );

uint32
  ARAD_CNT_COUNTERS_INFO_verify(
     SOC_SAND_IN  int                    unit,
     SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info
  );

uint32
  ARAD_CNT_OVERFLOW_verify(
    SOC_SAND_IN  ARAD_CNT_OVERFLOW *info
  );

uint32
  ARAD_CNT_STATUS_verify(
    SOC_SAND_IN  ARAD_CNT_STATUS *info
  );

uint32
  ARAD_CNT_RESULT_verify(
    SOC_SAND_IN  ARAD_CNT_RESULT *info
  );

uint32
  ARAD_CNT_RESULT_ARR_verify(
     SOC_SAND_IN  int                  unit,
     SOC_SAND_IN  ARAD_CNT_RESULT_ARR *info
  );

#endif /* ARAD_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_CNT_INCLUDED__*/
#endif


