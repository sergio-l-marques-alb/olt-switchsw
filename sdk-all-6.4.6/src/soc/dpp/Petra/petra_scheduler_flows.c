/* $Id: petra_scheduler_flows.c,v 1.9 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_scheduler_flows.h>
#include <soc/dpp/Petra/petra_scheduler_flow_converts.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>

#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/drv.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
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

/*********************************************************************
 *     Sets the Independent-Per-Flow Weight configuration mode
 *     (proportional or inverse-proportional). The mode affects
 *     all subsequent configurations of the flow weight in
 *     Independent-Per-Flow mode, as configured by flow_set and
 *     aggregate_set APIs.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET_UNSAFE);
    
  SOC_DPP_CONFIG(unit)->tm.ipf_config_mode = mode;
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_flow_ipf_config_mode_set_unsafe()", 0, 0);
}

uint32
  soc_petra_sch_flow_ipf_config_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mode, SOC_PETRA_SCH_NOF_FLOW_IPF_CONFIG_MODES, SOC_PETRA_END2END_SCHEDULER_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_flow_ipf_config_mode_set_verify()", 0, 0);
}

/*********************************************************************
 *     Sets the Independent-Per-Flow Weight configuration mode
 *     (proportional or inverse-proportional). The mode affects
 *     all subsequent configurations of the flow weight in
 *     Independent-Per-Flow mode, as configured by flow_set and
 *     aggregate_set APIs.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE *mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode);
    
  *mode = SOC_DPP_CONFIG(unit)->tm.ipf_config_mode;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_flow_ipf_config_mode_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_nof_subflows_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          base_flow_id,
    SOC_SAND_OUT    uint32                   *nof_subflows
  )
{
  uint32
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_FSF_TBL_DATA
    fsf_tbl_data;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_NOF_SUBFLOWS_GET);

  /*
   * Check input parameter
   */
  SOC_SAND_CHECK_NULL_INPUT(nof_subflows);

  res = soc_petra_sch_per1k_info_get_unsafe(
          unit, 0,
          SOC_PETRA_SCH_FLOW_TO_1K_ID(base_flow_id),
          &global_per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  offset  = SOC_PETRA_REG_IDX_GET(base_flow_id, SOC_SAND_REG_SIZE_BITS);

  idx = SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW(base_flow_id, global_per1k_info.is_odd_even);
  idx = SOC_PETRA_FLD_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
  idx = idx / 2 + idx % 2;

  res = soc_petra_sch_fsf_tbl_get_unsafe(
          unit,
          offset,
          &fsf_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *nof_subflows = SOC_SAND_GET_BIT(fsf_tbl_data.sfenable,idx)? 2 : 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_nof_subflows_get()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_nof_subflows_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID           base_flow_id,
    SOC_SAND_IN     uint32                    nof_subflows,
    SOC_SAND_IN     uint8                   is_odd_even
  )
{
  uint32
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_FSF_TBL_DATA
    fsf_tbl_data;
  uint32
    bit_val_to_set = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_NOF_SUBFLOWS_SET);

  offset  = SOC_PETRA_REG_IDX_GET(base_flow_id, SOC_SAND_REG_SIZE_BITS);

  idx = SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW(base_flow_id, is_odd_even);
  idx = SOC_PETRA_FLD_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
  idx = (idx/2) + (idx%2);

  res = soc_petra_sch_fsf_tbl_get_unsafe(
          unit,
          offset,
          &fsf_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bit_val_to_set = (nof_subflows > 1) ? 0x1 : 0x0;

  if (SOC_SAND_GET_BIT(fsf_tbl_data.sfenable,idx) != bit_val_to_set)
  {
     SOC_SAND_SET_BIT(fsf_tbl_data.sfenable,bit_val_to_set,idx);
     res = soc_petra_sch_fsf_tbl_set_unsafe(
             unit,
             offset,
             &fsf_tbl_data
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_nof_subflows_set()",0,0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_slow_enable_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID           flow_ndx,
    SOC_SAND_IN     uint8                   is_slow_enabled
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    value,
    offset = 0,
    idx = 0;
  SOC_PETRA_SCH_FQM_TBL_DATA
    fqm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_SLOW_ENABLE_SET);

  value = SOC_SAND_BOOL2NUM(is_slow_enabled);

  offset  = SOC_PETRA_SCH_FLOW_TO_QRTT_ID(flow_ndx);
  idx = SOC_PETRA_SCH_FLOW_ID_IN_QRTT(flow_ndx);

  res = soc_petra_sch_fqm_tbl_get_unsafe(
          unit,
          offset,
          &fqm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          &value,
          idx,
          1,
          &(fqm_tbl_data.flow_slow_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_sch_fqm_tbl_set_unsafe(
          unit,
          offset,
          &fqm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_slow_enable_set()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_slow_enable_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          flow_ndx,
    SOC_SAND_OUT    uint8                   *is_slow_enabled
  )
{
  uint32
    offset = 0,
    idx = 0,
    res = SOC_SAND_OK,
    value = 0;
  SOC_PETRA_SCH_FQM_TBL_DATA
    fqm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_SLOW_ENABLE_GET);

  /*
   * Check input parameter
   */
  SOC_SAND_CHECK_NULL_INPUT(is_slow_enabled);

  offset  = SOC_PETRA_SCH_FLOW_TO_QRTT_ID(flow_ndx);
  idx = SOC_PETRA_SCH_FLOW_ID_IN_QRTT(flow_ndx);

  res = soc_petra_sch_fqm_tbl_get_unsafe(
          unit,
          offset,
          &fqm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &(fqm_tbl_data.flow_slow_enable),
          idx,
          1,
          &value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  *is_slow_enabled = (value == 0x1) ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_se_state_get()",0,0);
}

/*****************************************************
* NAME
*    soc_petra_sch_flow_subflow_get
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Get a single subflow of a flow.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_INOUT  SOC_PETRA_SCH_SUBFLOW      *subflow -
*     The subflow info
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
STATIC uint32
  soc_petra_sch_flow_subflow_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_INOUT  SOC_PETRA_SCH_SUBFLOW      *subflow
  )
{
  uint32
    offset = 0,
    res;
  SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  SOC_PETRA_SCH_FDMS_TBL_DATA
    fdms_tbl_data;
  SOC_PETRA_SCH_SHDS_TBL_DATA
    shds_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_SUBFLOW_GET);
  SOC_SAND_CHECK_NULL_INPUT(subflow);

  /*
   * Read indirect from FDMS table
   */

  offset  = subflow->id;

  res = soc_petra_sch_fdms_tbl_get_unsafe(
          unit,
          offset,
          &fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (fdms_tbl_data.cos)
  {
    internal_sub_flow.cos = fdms_tbl_data.cos;
    internal_sub_flow.hr_sel_dual = fdms_tbl_data.hrsel_dual;
    internal_sub_flow.sch_number = fdms_tbl_data.sch_number;

    /*
     * Read indirect from SHDS table
     */

    offset  = subflow->id/2;

    res = soc_petra_sch_shds_tbl_get_unsafe(
            unit,
            offset,
            &shds_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (subflow->id%2 == 0)
    {
      internal_sub_flow.max_burst = shds_tbl_data.max_burst_even;
      internal_sub_flow.max_burst_update = shds_tbl_data.max_burst_update_even;
      internal_sub_flow.peak_rate_man = shds_tbl_data.peak_rate_man_even;
      internal_sub_flow.peak_rate_exp = shds_tbl_data.peak_rate_exp_even;
      internal_sub_flow.slow_rate_index = shds_tbl_data.slow_rate2_sel_even;
    }
    else
    {
      internal_sub_flow.max_burst = shds_tbl_data.max_burst_odd;
      internal_sub_flow.max_burst_update = shds_tbl_data.max_burst_update_odd;
      internal_sub_flow.peak_rate_man = shds_tbl_data.peak_rate_man_odd;
      internal_sub_flow.peak_rate_exp = shds_tbl_data.peak_rate_exp_odd;
      internal_sub_flow.slow_rate_index = shds_tbl_data.slow_rate2_sel_odd;
    }

    /*
     * Convert to user representation.
     */
    res = soc_petra_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert(
            unit,
            &internal_sub_flow,
            subflow
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    subflow->is_valid = TRUE;
  }
  else
  {
    subflow->is_valid = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_subflow_get()",0,0);
}


/*****************************************************
* NAME
*    soc_petra_sch_flow_subflow_set
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     uint32                 slow_rate_index -
*   SOC_SAND_INOUT  SOC_PETRA_SCH_SUBFLOW      *subflow -
*     The subflow info
*   SOC_SAND_IN     SOC_PETRA_SCH_SUBFLOW        *prev_subflow -
*     previous subflow info
*   SOC_SAND_OUT    SOC_PETRA_SCH_SUBFLOW        *exact_subflow -
*     The exact subflow info written to the device.
*     May vary from the supplied due to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
STATIC uint32
  soc_petra_sch_flow_subflow_set(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     uint32                slow_rate_index,
    SOC_SAND_IN     SOC_PETRA_SCH_SUBFLOW        *subflow,
    SOC_SAND_OUT    SOC_PETRA_SCH_SUBFLOW        *exact_subflow
  )
{
  uint32
    offset = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  SOC_PETRA_SCH_FDMS_TBL_DATA
    fdms_tbl_data;
  SOC_PETRA_SCH_SHDS_TBL_DATA
    shds_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_SUBFLOW_SET);

  /*
   * Check input parameter
   */
  SOC_SAND_CHECK_NULL_INPUT(subflow);
  SOC_SAND_CHECK_NULL_INPUT(exact_subflow);

  SOC_PETRA_COPY(exact_subflow, subflow, SOC_PETRA_SCH_SUBFLOW, 1);

  if (subflow->is_valid == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_INVALID_ERR, 5, exit);
  }

  res = soc_petra_sch_flow_id_verify_unsafe(
          unit,
          subflow->id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Convert to internal representation.
   */
  res = soc_petra_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert(
          unit,
          subflow,
          &internal_sub_flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Slow rate index
   */
  switch(slow_rate_index) {
    case SOC_PETRA_SCH_SLOW_RATE_NDX_1:
      internal_sub_flow.slow_rate_index = 0x0;
      break;
    case SOC_PETRA_SCH_SLOW_RATE_NDX_2:
      internal_sub_flow.slow_rate_index = 0x1;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SLOW_RATE_INDEX_INVALID_ERR, 25, exit);
  }

  /*
   * Get current value - we only update part of the fields (odd/even)
   */
  offset  = subflow->id / 2;
  res = soc_petra_sch_shds_tbl_get_unsafe(
          unit,
          offset,
          &shds_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (subflow->id % 2 == 0)
  {
    if (
        (shds_tbl_data.max_burst_even >= internal_sub_flow.max_burst) &&
        (internal_sub_flow.max_burst != 0x0)
       )
    {
      internal_sub_flow.max_burst_update = 0;
    }
    else
    {
      internal_sub_flow.max_burst_update = 1;
    }
    shds_tbl_data.max_burst_even = internal_sub_flow.max_burst;
    shds_tbl_data.max_burst_update_even = internal_sub_flow.max_burst_update;
    shds_tbl_data.peak_rate_man_even = internal_sub_flow.peak_rate_man;
    shds_tbl_data.peak_rate_exp_even = internal_sub_flow.peak_rate_exp;
    shds_tbl_data.slow_rate2_sel_even = internal_sub_flow.slow_rate_index;
  }
  else
  {
    if (
        (shds_tbl_data.max_burst_odd >= internal_sub_flow.max_burst) &&
        (internal_sub_flow.max_burst != 0x0)
       )
    {
      internal_sub_flow.max_burst_update = 0;
    }
    else
    {
      internal_sub_flow.max_burst_update = 1;
    }
    shds_tbl_data.max_burst_odd = internal_sub_flow.max_burst;
    shds_tbl_data.max_burst_update_odd = internal_sub_flow.max_burst_update;
    shds_tbl_data.peak_rate_man_odd = internal_sub_flow.peak_rate_man;
    /* internal_sub_flow.peak_rate_exp is initialized by soc_petra_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert */
    /* coverity[uninit_use_in_call:FALSE] */
    shds_tbl_data.peak_rate_exp_odd = internal_sub_flow.peak_rate_exp;
    shds_tbl_data.slow_rate2_sel_odd = internal_sub_flow.slow_rate_index;
  }

  /*
   * Write indirect to SHDS table
   */
  res = soc_petra_sch_shds_tbl_set_unsafe(
          unit,
          offset,
          &shds_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   * Write indirect from FDMS table
   */
  offset  = subflow->id;

  fdms_tbl_data.cos = internal_sub_flow.cos;
  fdms_tbl_data.hrsel_dual = internal_sub_flow.hr_sel_dual;
  fdms_tbl_data.sch_number = internal_sub_flow.sch_number;
  res = soc_petra_sch_fdms_tbl_set_unsafe(
          unit,
          offset,
          &fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   * Convert back to user representation.
   */
  exact_subflow->id = subflow->id;

  res = soc_petra_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert(
          unit,
          &internal_sub_flow,
          exact_subflow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_subflow_set()",0,0);
}


/*****************************************************
* NAME
*    soc_petra_sch_sub_flow_spouse_id
* TYPE:
*   PROC
* DATE:
*   24/10/2007
* FUNCTION:
*  in composite mode, return the id of the other subflow
* INPUT:
*   SOC_PETRA_SCH_FLOW_ID flow_id - flow index.
*   uint8 is_odd_even -
*     odd-even configuration:
*     TRUE is 0-1 configuration, FALSE is 0-2 configuration.
* RETURNS:
*   The id of the other subflow.
* REMARKS:
*    None.
*****************************************************/
STATIC SOC_PETRA_SCH_FLOW_ID
  soc_petra_sch_sub_flow_spouse_id(
    SOC_SAND_IN SOC_PETRA_SCH_FLOW_ID flow_id,
    SOC_SAND_IN uint8 is_odd_even
  )
{
  SOC_PETRA_SCH_FLOW_ID
    spouse_fid = 0;

  if (is_odd_even)
  {
    spouse_fid =
      (flow_id==SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW_0_1(flow_id))?flow_id+1:flow_id-1;
  }
  else
  {
    spouse_fid =
      (flow_id==SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW_0_2(flow_id))?flow_id+2:flow_id-2;
  }

  return spouse_fid;
}

/*****************************************************
* NAME
*    soc_petra_sch_is_sub_flow_second
* TYPE:
*   PROC
* DATE:
*   24/10/2007
* FUNCTION:
*   In composite mode,
*   check if the id is the second (not independent) subflow.
* INPUT:
*   SOC_PETRA_SCH_FLOW_ID flow_id -
*   uint8 is_odd_even -
*     odd-even configuration:
*     TRUE is 0-1 configuration, FALSE is 0-2 configuration.
* RETURNS:
*   TRUE if this is second (not independent) subflow.
* REMARKS:
*    None.
*****************************************************/
STATIC uint8
  soc_petra_sch_is_sub_flow_second(
    SOC_PETRA_SCH_FLOW_ID flow_id,
    uint8 is_odd_even
  )
{
  uint8
    is_second = FALSE;

  is_second =
      (flow_id==SOC_PETRA_SCH_SUB_FLOW_BASE_FLOW(flow_id, is_odd_even))?FALSE:TRUE;

  return is_second;
}

uint32
  soc_petra_sch_is_hr_subflow_valid(
    SOC_SAND_IN     SOC_PETRA_SCH_SUB_FLOW_HR*  hr_properties,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_INFO*      se
    )
{
  SOC_PETRA_SCH_SE_HR_MODE
    hr_sched_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IS_HR_SUBFLOW_VALID);

  if (se->type != SOC_PETRA_SCH_SE_TYPE_HR)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_TYPE_MISMATCH_ERR, 10, exit);
  }
  /*
   * Check if sp_class matches the se mode
   */
  hr_sched_mode = se->type_info.hr.mode;

  switch (hr_properties->sp_class)
  {
  /*
   * EF1-EF3 are the highest SP level in all HR modes
   */
  case SOC_PETRA_SCH_FLOW_HR_CLASS_EF1:
  case SOC_PETRA_SCH_FLOW_HR_CLASS_EF2:
  case SOC_PETRA_SCH_FLOW_HR_CLASS_EF3:
    /* Nothing to check */
    break;

  /* SINGLE WFQ MODE */
  case SOC_PETRA_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ:
  case SOC_PETRA_SCH_FLOW_HR_SINGLE_CLASS_BE1:
    if (hr_sched_mode != SOC_PETRA_SCH_HR_MODE_SINGLE_WFQ)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 20, exit);
    }
    break;

  /* DUAL WFQ MODE */
  case SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ:
  case SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ:
  case SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_BE2:
    if (hr_sched_mode != SOC_PETRA_SCH_HR_MODE_DUAL_WFQ)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 30, exit);
    }
    break;

  /* ENHANCED PRIORITY MODE */
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF1:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF2:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF3:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF4:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF5:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_AF6:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ:
  case SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_BE2:
    if (hr_sched_mode != SOC_PETRA_SCH_HR_MODE_ENHANCED_PRIO_WFQ)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 40, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_CLASS_OUT_OF_RANGE_ERR, 50, exit);
  }
/* $Id: petra_scheduler_flows.c,v 1.9 Broadcom SDK $
 * if the class is WFQ, check that the weight is within range (1-4096)
 */
  if ((SOC_PETRA_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ   == hr_properties->sp_class) ||
      (SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ     == hr_properties->sp_class) ||
      (SOC_PETRA_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ     == hr_properties->sp_class) ||
      (SOC_PETRA_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ == hr_properties->sp_class))
  {
    if ((hr_properties->weight < SOC_PETRA_SCH_FLOW_HR_MIN_WEIGHT) ||
        (hr_properties->weight > SOC_PETRA_SCH_FLOW_HR_MAX_WEIGHT))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_WEIGHT_OUT_OF_RANGE_ERR, 60, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_is_hr_subflow_valid",0,0);
}

uint32
  soc_petra_sch_is_cl_subflow_valid(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SUB_FLOW_CL*             cl_properties,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_INFO*                 se
 )
{
  uint32
      res;
  SOC_PETRA_SCH_SE_CL_CLASS_INFO
    class_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IS_CL_SUBFLOW_VALID);

  if (se->type != SOC_PETRA_SCH_SE_TYPE_CL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 10, exit);
  }

  res = soc_petra_sch_class_type_params_get_unsafe(
          unit,
          se->type_info.cl.id,
          &class_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Check if class matches the se mode
   */
  switch (cl_properties->sp_class)
  {
    /*
     * SP1: The se operates in one of the following configurations:
     *      1. Mode 1 (the WFQ mode is don't care)
     *      2. Mode 2 and discrete WFQ mode
     *      3. Mode 4 (the WFQ mode is don't care)
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1:
    if (!((SOC_PETRA_SCH_CL_MODE_1 == class_type.mode) ||
          ((SOC_PETRA_SCH_CL_MODE_2 == class_type.mode) &&
           SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode))      ||
          (SOC_PETRA_SCH_CL_MODE_4 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 30, exit);
    }
    break;

    /*
     * SP2: The se operates in one of the following configurations:
     *      1. Mode 1 (the WFQ mode is don't care)
     *      2. Mode 2 and discrete WFQ mode
     *      3. Mode 3 (the WFQ mode is don't care)
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2:
    if (!((SOC_PETRA_SCH_CL_MODE_1 == class_type.mode)        ||
          ((SOC_PETRA_SCH_CL_MODE_2 == class_type.mode) &&
           SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode))             ||
          (SOC_PETRA_SCH_CL_MODE_3 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 40, exit);
    }
    break;

    /*
     * SP3,SP4: The se operates in Mode 1 (the WFQ mode is don't care)
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP4:
    if (!(SOC_PETRA_SCH_CL_MODE_1 == class_type.mode))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 50, exit);
    }
    break;

    /*
     * SP1_WFQ: The se operates in one of the following configurations:
     *          1. Mode 3 and non-discrete WFQ mode
     *          2. Mode 5 and non-discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ:
    if (!(SOC_PETRA_SCH_IS_INDEPENDENT_WFQ_MODE(class_type.weight_mode)  &&
          ((SOC_PETRA_SCH_CL_MODE_3 == class_type.mode) ||
           (SOC_PETRA_SCH_CL_MODE_5 == class_type.mode)))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 60, exit);
    }
    break;

    /*
     * SP1_WFQ1, SP1_WFQ2, SP1_WFQ3: The se operates in one of
     *          the following configurations:
     *          1. Mode 3 and discrete WFQ mode
     *          2. Mode 5 and discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3:
    if (!(SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode)  &&
          ((SOC_PETRA_SCH_CL_MODE_3 == class_type.mode) ||
           (SOC_PETRA_SCH_CL_MODE_5 == class_type.mode)))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 70, exit);
    }
    break;

    /*
     * SP1_WFQ4: The se operates in Mode 5 and discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4:
    if (!(SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode) &&
          (SOC_PETRA_SCH_CL_MODE_5 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 80, exit);
    }
    break;

    /*
     * SP2_WFQ: The se operates in Mode 4 and non-discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ:
    if (!(SOC_PETRA_SCH_IS_INDEPENDENT_WFQ_MODE(class_type.weight_mode) &&
          (SOC_PETRA_SCH_CL_MODE_4 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 90, exit);
    }
    break;

    /*
     * SP2_WFQ1, SP2_WFQ2, SP2_WFQ3: The se operates in
     *           mode 4 and discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3:
    if (!(SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode) &&
          (SOC_PETRA_SCH_CL_MODE_4 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 100, exit);
    }
    break;

    /*
     * SP3_WFQ1, SP3_WFQ2: The se operates in
     *           mode 2 and discrete WFQ mode
     */
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1:
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2:
    if (!(SOC_PETRA_SCH_IS_DISCRETE_WFQ_MODE(class_type.weight_mode) &&
          (SOC_PETRA_SCH_CL_MODE_2 == class_type.mode))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR, 110, exit);
    }
    break;
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED:
    if (class_type.enhanced_mode != SOC_PETRA_CL_ENHANCED_MODE_ENABLED_HP)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_ENHANCED_SP_MODE_MISMATCH_ERR, 120, exit);
    }
    break;
  case SOC_PETRA_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED:
    if (class_type.enhanced_mode != SOC_PETRA_CL_ENHANCED_MODE_ENABLED_LP)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_ENHANCED_SP_MODE_MISMATCH_ERR, 130, exit);
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_CLASS_OUT_OF_RANGE_ERR, 140, exit);
  }
  /*
   * check that independent weight is within range
   */
  if (SOC_PETRA_SCH_IS_WFQ_CLASS_VAL(cl_properties->sp_class) &&
      SOC_PETRA_SCH_IS_INDEPENDENT_WFQ_MODE(class_type.weight_mode)
     )
  {
    if ((cl_properties->weight < SOC_PETRA_SCH_SUB_FLOW_CL_MIN_WEIGHT) ||
        (((SOC_PETRA_SCH_CL_MODE_3 == class_type.mode)  ||
          (SOC_PETRA_SCH_CL_MODE_4 == class_type.mode)) &&
         (cl_properties->weight > SOC_PETRA_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4)) ||
        ((SOC_PETRA_SCH_CL_MODE_5 == class_type.mode)  &&
         (cl_properties->weight > SOC_PETRA_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_WEIGHT_OUT_OF_RANGE_ERR, 150, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_is_cl_subflow_valid",0,0);
}


uint32
  soc_petra_sch_is_subflow_valid(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     uint32           sub_flow_i,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID   flow_id,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW     *flow,
    SOC_SAND_IN     uint8           is_odd_even
    )
{
  uint32
    res;
  const SOC_PETRA_SCH_SUBFLOW*
    sub_flow;
  SOC_PETRA_SCH_SE_INFO
    se;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IS_SUBFLOW_VALID);

  sub_flow = (const SOC_PETRA_SCH_SUBFLOW*)&flow->sub_flow[sub_flow_i];
  /*
   * check:
   *   a. The subflow id is legal for this flow
   *   b. The credit source is a valid se
   *   c. The subflow type is legal
   *   d. The type-specific properties are valid
   */
  res = soc_petra_sch_flow_id_verify_unsafe(
            unit,
            sub_flow->id
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if ( !SOC_PETRA_SCH_IS_SUB_FLOW_OF_FLOW(flow_id, sub_flow->id,is_odd_even) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_ID_MISMATCH_WITH_FLOW_ID_ERR, 20, exit);
  }

  /*
   * Check that the credit source se element is valid and enabled
   */
  se.id = sub_flow->credit_source.id;

  res = soc_petra_sch_se_id_verify_unsafe(
            unit,
            se.id
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_sch_se_id_and_type_match_verify(
          se.id,
          sub_flow->credit_source.se_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_sch_se_get_unsafe(
          unit,
          se.id,
          &se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (SOC_PETRA_SCH_SE_STATE_ENABLE != se.state)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_ATTACHED_TO_DISABLED_SCHEDULER_ERR, 6, exit);
  }

  switch(sub_flow->credit_source.se_type)
  {
  case SOC_PETRA_SCH_SE_TYPE_HR:
    res = soc_petra_sch_is_hr_subflow_valid(
            &sub_flow->credit_source.se_info.hr, &se
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    break;

  case SOC_PETRA_SCH_SE_TYPE_CL:
    res = soc_petra_sch_is_cl_subflow_valid(
            unit,
            &sub_flow->credit_source.se_info.cl,
            &se
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;

  case SOC_PETRA_SCH_SE_TYPE_FQ:
    /* Nothing to check for FQ subflows */
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SUB_FLOW_SE_TYPE_OUT_OF_RANGE_ERR, 90, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_is_subflow_valid()",0,0);
}

uint32
  soc_petra_sch_subflows_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow
  )
{
  uint32
    res;
  uint32
    sub_flow_i = 0;
  uint8
    is_flow_odd_even = FALSE,
    flow_gap_found = FALSE;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    per1k_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SUBFLOWS_VERIFY_UNSAFE);

  if (!soc_petra_is_flow_valid(unit, flow_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FLOW_ID_OUT_OF_RANGE_ERR, 2, exit)
  }

  /*
   * 1. At least one subflow should be valid
   * 2. No gaps are allowed in the subflows array
   * 3. All credit sources of the subflows should be associated with the
   *    same flow group.
   * 4. For each valid subflow check:
   *    a. The subflow id is legal
   *    b. The credit source is a valid se
   *    c. The subflow type is legal
   *    d, The type-specific properties are valid
   */

  res = soc_petra_sch_per1k_info_get_unsafe(
          unit, 0,
          SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_ndx),
          &per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  is_flow_odd_even = per1k_info.is_odd_even;

  for(sub_flow_i = 0; sub_flow_i < SOC_PETRA_SCH_NOF_SUB_FLOWS; sub_flow_i++)
  {
    if(flow->sub_flow[sub_flow_i].is_valid == TRUE)
    {
      if (flow_gap_found)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_GAP_IN_SUB_FLOW_ERR, 15, exit);
      }
      res = soc_petra_sch_is_subflow_valid(
              unit,
              sub_flow_i,
              flow_ndx,
              flow,
              is_flow_odd_even
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      flow_gap_found = TRUE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_subflows_verify()",0,0);
}

/*********************************************************************
*     Input verification for soc_petra_sch_flow_verify_unsafe
*********************************************************************/
uint32
  soc_petra_sch_flow_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow
  )
{
  uint32
    res;
  SOC_PETRA_SCH_SE_ID
    se_id = SOC_PETRA_SCH_SE_ID_INVALID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_VERIFY_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(flow);

  SOC_SAND_MAGIC_NUM_VERIFY(flow);

  if (!soc_petra_is_flow_valid(unit, flow_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FLOW_ID_OUT_OF_RANGE_ERR, 2, exit)
  }

  /* verify flow id */
  res = soc_petra_sch_flow_id_verify_unsafe(
            unit,
            flow_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  switch(flow->flow_type)
  {
  case SOC_PETRA_FLOW_SIMPLE:
    res = soc_petra_sch_flow_id_verify_unsafe(
            unit,
            flow_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
  case SOC_PETRA_FLOW_AGGREGATE:
    /* verify scheduling element id */
    se_id = soc_petra_sch_flow2se_id(flow_ndx);

    if (se_id == SOC_PETRA_SCH_SE_ID_INVALID)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_SE_ID_ERR, 20, exit);
    }

    res = soc_petra_sch_se_id_verify_unsafe(
            unit,
            se_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_TYPE_UNDEFINED_ERR, 40, exit)
    break;
  }

  res = soc_petra_sch_subflows_verify_unsafe(
          unit,
          flow_ndx,
          flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_verify_unsafe()",0,0);
}

/*********************************************************************
*     Sets a se flow, from a scheduling element (or
*     elements) another element . The driver writes to the
*     following tables: Scheduler Enable Memory (SEM), Shaper
*     Descriptor Memory Static(SHDS) Flow Sub-Flow (FSF) Flow
*     Descriptor Memory Static (FDMS)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow
  )
{
  uint32
    nof_subflows = 0,
    nof_subflows_curr,
    res = SOC_SAND_OK;
  uint32
    slow_index[SOC_PETRA_SCH_NOF_SUB_FLOWS],
    sub_flow_i,
    spouse_subf_id;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    per1k_info;
  SOC_PETRA_SCH_SE_ID
    se_id = SOC_PETRA_SCH_SE_ID_INVALID;
  uint8
    is_slow_enabled,
    is_flow_odd_even = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(flow);
  SOC_SAND_CHECK_NULL_INPUT(exact_flow);

  SOC_PETRA_COPY(exact_flow, flow, SOC_PETRA_SCH_FLOW, 1);

  res = soc_petra_sch_per1k_info_get_unsafe(
          unit, 0,
          SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_ndx),
          &per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  is_flow_odd_even = per1k_info.is_odd_even;

  for (sub_flow_i = 0; sub_flow_i < SOC_PETRA_SCH_NOF_SUB_FLOWS; sub_flow_i++)
  {
    if (flow->sub_flow[sub_flow_i].is_valid)
    {
      ++nof_subflows;
    }
  }
  /*
   * Verify that flow_ndx is an independent flow, and not a subflow.
   */
  if ((nof_subflows > 1) && SOC_PETRA_SCH_COMPOSITE_IS_SECOND_SUBFLOW(flow_ndx,is_flow_odd_even))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_ID_IS_SECOND_SUB_FLOW_ERR, 20, exit);
  }

  /*
   *	Delete the second sub-flow if needed
   */
  if (flow->sub_flow[SOC_PETRA_SCH_NOF_SUB_FLOWS-1].is_valid == FALSE)
  {
    /*
     *  Check if previously this was an active sub-flow.
     *  If so - delete it.
     */
    /*
     *  Get flow <=> sub flows mapping of the base flow id.
     *  It is flow_ndx itself if it is base-flow, or the flow-d
     *  of the spouse flow.
     */
    res = soc_petra_sch_flow_nof_subflows_get(
            unit,
            flow_ndx,
            &nof_subflows_curr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    if (nof_subflows_curr > 1)
    {
      /*
       *  This is currently a composite flow
       */

      spouse_subf_id = soc_petra_sch_sub_flow_spouse_id(
                         flow_ndx,
                         is_flow_odd_even
                       );

       res = soc_petra_sch_flow_delete_unsafe(
                unit,
                spouse_subf_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
    }
  }

  res = soc_petra_sch_flow_nof_subflows_set(
          unit,
          flow_ndx,
          nof_subflows,
          is_flow_odd_even
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (flow->sub_flow[0].is_valid == FALSE)
  {
    /*
     *	Delete the flow and exit
     */
    res = soc_petra_sch_flow_delete_unsafe(
              unit,
              flow_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

      SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   * Simple flows that use IDs that might also be used as aggregate,
   * should be disabled as elementary schedulers.
   */
  for (sub_flow_i = 0; sub_flow_i < nof_subflows; sub_flow_i++)
  {
    se_id = soc_petra_sch_flow2se_id(flow->sub_flow[sub_flow_i].id);
    if((SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(se_id)) &&
       (flow->flow_type == SOC_PETRA_FLOW_SIMPLE))
    {
      res = soc_petra_sch_se_state_set(
              unit,
              se_id,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
  }

  /*
   * When the second sub-flow feature is enabled,
   *   each sub-flow might be set to use different slow value.
   */
  is_slow_enabled = flow->is_slow_enabled;

  if(flow->flow_type == SOC_PETRA_FLOW_SIMPLE)
  {
    exact_flow->is_slow_enabled = (is_slow_enabled)?TRUE:FALSE;
  }
  else
  {
    exact_flow->is_slow_enabled = FALSE;
  }

  /*
   * Write sub-flow/s properties (credit source, QoS params, shaper)
   * to FDMS & SHDS
   */
  for (sub_flow_i = 0; sub_flow_i < nof_subflows; sub_flow_i++)
  {
    if (flow->is_slow_enabled == FALSE)
    {
      slow_index[sub_flow_i] = SOC_PETRA_SCH_SLOW_RATE_NDX_1;
    }
    else
    {
      slow_index[sub_flow_i] = flow->sub_flow[sub_flow_i].slow_rate_ndx;
    }

    res = soc_petra_sch_flow_subflow_set(
            unit,
            slow_index[sub_flow_i],
            &flow->sub_flow[sub_flow_i],
            &exact_flow->sub_flow[sub_flow_i]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  res = soc_petra_sch_flow_slow_enable_set(
          unit,
          flow_ndx,
          SOC_SAND_BOOL2NUM(is_slow_enabled)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_set_unsafe()",flow_ndx,0);
}



/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow
  )
{
  uint32
    nof_subflows,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    per1k_info;
  uint8
    is_invalid,
    is_odd_even = FALSE,
    is_aggregate = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(flow);

  /* verify flow id */
  res = soc_petra_sch_flow_id_verify_unsafe(
            unit,
            flow_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sch_flow_is_deleted_get_unsafe(
          unit,
          flow_ndx,
          &is_invalid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  flow->sub_flow[0].is_valid = (is_invalid)?FALSE:TRUE;

  if(!flow->sub_flow[0].is_valid)
  {
    soc_petra_PETRA_SCH_FLOW_clear(flow);
  }

  res = soc_petra_sch_per1k_info_get(
      unit,0,
      SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_ndx),
      &per1k_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  is_odd_even = per1k_info.is_odd_even;

  /*
   *  Get flow <=> sub flows mapping of the base flow id.
   *  It is flow_ndx itself if it is base-flow, or the flow-d
   *  of the spouse flow.
   */
  res = soc_petra_sch_flow_nof_subflows_get(
          unit,
          flow_ndx,
          &nof_subflows
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (nof_subflows > 1)
  {
    /*
     *  This is a composite flow
     */
    if (soc_petra_sch_is_sub_flow_second(flow_ndx, is_odd_even))
    {
      flow->flow_type = SOC_PETRA_FLOW_NONE;
      goto exit;
    }
  }

  /*
   * If the flow ID is within the aggregate range, check if it is indeed an
   * aggregate flow or a regular one
   */
  if (soc_petra_sch_is_flow_id_se_id(flow_ndx))
  {
    res = soc_petra_sch_se_state_get(
            unit,
            soc_petra_sch_flow2se_id(flow_ndx),
            &is_aggregate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    flow->flow_type  = is_aggregate?SOC_PETRA_FLOW_AGGREGATE:SOC_PETRA_FLOW_SIMPLE;
  }
  else
  {
    flow->flow_type  = SOC_PETRA_FLOW_SIMPLE;
  }

  /*
   * Read sub-flow/s properties (credit source, QoS params, shaper)
   * from FDMS & SHDS
   */

  if (nof_subflows == SOC_PETRA_SCH_NOF_SUB_FLOWS)
  {
    /* composite flow */
    flow->sub_flow[0].id = flow_ndx;
    flow->sub_flow[1].id = soc_petra_sch_sub_flow_spouse_id(
                             flow_ndx,
                             is_odd_even
                           );

    res = soc_petra_sch_flow_is_deleted_get_unsafe(
            unit,
            flow->sub_flow[1].id,
            &is_invalid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    flow->sub_flow[1].is_valid = SOC_SAND_NUM2BOOL_INVERSE(is_invalid);

    if(flow->sub_flow[0].is_valid)
    {
      res = soc_petra_sch_flow_subflow_get(
            unit,
              &flow->sub_flow[0]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_petra_sch_flow_subflow_get(
              unit,
              &flow->sub_flow[1]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
  else
  {
    /* single (not composite) flow */
    flow->sub_flow[0].id = flow_ndx;

    if(flow->sub_flow[0].is_valid)
    {
      res = soc_petra_sch_flow_subflow_get(
              unit,
              &flow->sub_flow[0]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    soc_petra_PETRA_SCH_SUBFLOW_clear(
            &flow->sub_flow[1]
          );
  }

  if (!is_aggregate)
  {
    soc_petra_sch_flow_slow_enable_get(
      unit,
      flow_ndx,
      &flow->is_slow_enabled
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  else
  {
    flow->is_slow_enabled = FALSE;
  }

  if(flow->sub_flow[0].is_valid)
  {
    /*
     * Check if the flow is valid
     */
    res = soc_petra_sch_flow_verify_unsafe(
            unit,
            flow_ndx,
            flow
          );
    if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
    {
      flow->flow_type = SOC_PETRA_FLOW_NONE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_get_unsafe()",0,0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_status_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_STATUS_VERIFY);

  if (!soc_petra_is_flow_valid(unit, flow_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FLOW_ID_OUT_OF_RANGE_ERR, 2, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_status_verify()",0,0);
}

/*********************************************************************
*     Set flow state to off/on. The state of the flow will be
*     updated, unless was configured otherwise. Note: useful
*     for virtual flows, for which the flow state must be
*     explicitly set
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_status_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state
  )
{
  uint32
    offset,
    res;
  SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_DATA
    fsm_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_STATUS_SET_UNSAFE);

  if ((state != SOC_PETRA_SCH_FLOW_OFF) &&
      (state != SOC_PETRA_SCH_FLOW_ON))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_STATUS_NOT_ON_OFF_ERR, 10, exit);
  }

  res = soc_petra_sch_flow_id_verify_unsafe(
          unit,
          flow_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Write to the FSM table.
   * This is a one-entry table, so the offset is always 0
   */
  offset = 0;
  fsm_data.message_flow_id = flow_ndx;
  fsm_data.message_type = state;

  res = soc_petra_sch_force_status_message_tbl_set_unsafe(
          unit,
          offset,
          &fsm_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_status_set_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

