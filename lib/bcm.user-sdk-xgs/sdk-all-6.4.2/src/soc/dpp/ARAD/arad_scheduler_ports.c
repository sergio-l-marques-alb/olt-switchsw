#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_scheduler_ports.c,v 1.25 Broadcom SDK $
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
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <soc/mem.h>


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_scheduler_elements.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/dpp/port_sw_db.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_SCH_TCG_NDX_DEFAULT                    (0)
#define ARAD_SCH_SINGLE_MEMBER_TCG_START            (4)
#define ARAD_SCH_SINGLE_MEMBER_TCG_END              (7)
#define ARAD_SCH_TCG_WEIGHT_MIN                     (0)
#define ARAD_SCH_TCG_WEIGHT_MAX                     (1023)
/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx) \
   (ARAD_BASE_PORT_TC2PS(base_port_tc)*ARAD_NOF_TCGS_IN_PS + tcg_ndx)
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


/*****************************************************
* NAME
*    arad_sch_hr_lowest_hp_class_select_get
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Get selected hp class out of the available configurations
*   (access device, SHC)
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN  ARAD_SCH_PORT_ID  port_ndx -
*     Port index. Range: 0 - 63.
*   SOC_SAND_IN  uint32           hp_class_conf_idx -
*     Selects which of the 4 available high priority class configurations
*     will be applied to the specified port. Range 0-3.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
STATIC uint32
  arad_sch_hr_lowest_hp_class_select_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID                  port_ndx,
    SOC_SAND_OUT ARAD_SCH_PORT_LOWEST_HP_HR_CLASS  *hp_class_conf_idx
  )
{
  uint32
    offset,
    res;
  ARAD_SCH_SHC_TBL_DATA
    shc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_LOWEST_HP_CLASS_SELECT_GET);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Read indirect from SHC table
   */

  offset  = port_ndx;

  res = arad_sch_shc_tbl_get_unsafe(
          unit,
          offset,
          &shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *hp_class_conf_idx = shc_tbl_data.hrmask_type;

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_lowest_hp_class_select_get", 0, 0);
}

/*****************************************************
* NAME
*   arad_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Assign HR scheduling element to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  ARAD_SCH_PORT_ID  port_ndx -
*     The index of the port to set.
*     Range: 0 - 79
*   SOC_SAND_IN  uint8           is_port_hr -
*     If TRUE, the HR will be assigned to the port.
*     Otherwise - unasigned.
*     HR that is not assigned to port can be used as
*     HR scheduler.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  arad_sch_hr_to_port_assign_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID    port_ndx,
    SOC_SAND_IN  uint8           is_port_hr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    reg_idx,
    fld_val = 0,
    is_port_hr_val;
  uint32
    base_port_tc,
    nof_priorities,
    priority_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_SET); 

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_ndx, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_ndx, 0, &nof_priorities);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

  /* Not even set */
  if (base_port_tc == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 20, exit)
  }
  reg_idx = ARAD_REG_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_ENABLE__PORTENm(unit, MEM_BLOCK_ANY, reg_idx, &fld_val));
  
  is_port_hr_val = SOC_SAND_BOOL2NUM(is_port_hr);
  start_idx = ARAD_FLD_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
   
  /* Set HR enable for all realted q-pairs */
  for (priority_i = 0; priority_i < nof_priorities; priority_i++)
  {    
    SOC_SAND_SET_BIT(fld_val, is_port_hr_val, start_idx+priority_i);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_ENABLE__PORTENm(unit, MEM_BLOCK_ANY, reg_idx, &fld_val));


exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_to_port_assign_set", 0, 0);
}

/*****************************************************
* NAME
*   arad_sch_hr_tcg_map_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Assign HR scheduling element to TCG specific group.
* INPUT:
*   SOC_SAND_IN  ARAD_SCH_SE_ID     se_id -
*     The index of the scheduler index to set. Must be an HR Port-Priority.
*     Range: 0 - 32K-1
*   SOC_SAND_IN  ARAD_TCG_NDX       tcg_ndx -
*     TCG (Traffic class groups) that the HR is mapped to.
*     Range: 0 - 7.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  arad_sch_hr_tcg_map_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_SCH_SE_ID      se_id,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    table_ndx,
    data,
    fld_val = 0;
  uint32
    hr_ndx,
    dummy_tc,
    port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_SET); 

  res = arad_sch_se_id_verify_unsafe(
          unit,
          se_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_HR_ID_ERR, 20, exit);
  }

  /* Validate HR given is enabled HR Port Priority and not just a reserved one */
  res = arad_sch_se2port_tc_id_get_unsafe(
          unit,
          se_id,
          &port_id,
          &dummy_tc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (port_id == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 35, exit);
  }

  /* HR index: out of 256 */
  hr_ndx = se_id - ARAD_HR_SE_ID_MIN;

  table_ndx = ARAD_REG_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  start_idx = ARAD_FLD_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, table_ndx, &data));
  fld_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,TC_PG_MAPf);

  res = soc_sand_bitstream_set_any_field(&tcg_ndx,start_idx * ARAD_NOF_TCG_IN_BITS,ARAD_NOF_TCG_IN_BITS,&fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_set(unit,&data,TC_PG_MAPf,fld_val);  
  /* Set tcg index */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, table_ndx, &data));    

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_tcg_map_set", 0, 0);
}

/*****************************************************
* NAME
*   arad_sch_hr_tcg_map_get
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Retreive HR scheduling element to TCG specific group.
* INPUT:
*   SOC_SAND_IN  ARAD_SCH_SE_ID     se_id -
*     The index of the scheduler index to set. Must be an HR Port-Priority.
*     Range: 0 - 32K-1
*   SOC_SAND_IN  ARAD_TCG_NDX       tcg_ndx -
*     TCG (Traffic class groups) that the HR is mapped to.
*     Range: 0 - 7.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  arad_sch_hr_tcg_map_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_SCH_SE_ID      se_id,
    SOC_SAND_OUT ARAD_TCG_NDX       *tcg_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    start_idx,
    table_ndx,
    data,
    fld_val = 0;
  uint32
    hr_ndx,
    dummy_tc,
    port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

  res = arad_sch_se_id_verify_unsafe(
          unit,
          se_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_HR_ID_ERR, 20, exit);
  }

  /* Validate HR given is enabled HR Port Priority and not just a reserved one */
  res = arad_sch_se2port_tc_id_get_unsafe(
          unit,
          se_id,
          &port_id,
          &dummy_tc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (port_id == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 35, exit);
  }

  /* HR index: out of 256 */
  hr_ndx = se_id - ARAD_HR_SE_ID_MIN;

  table_ndx = ARAD_REG_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  start_idx = ARAD_FLD_IDX_GET(hr_ndx, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, table_ndx, &data));
  fld_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,TC_PG_MAPf);

  res = soc_sand_bitstream_get_any_field(&fld_val,start_idx * ARAD_NOF_TCG_IN_BITS,ARAD_NOF_TCG_IN_BITS,tcg_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
 
exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_tcg_map_get", 0, 0);
}

/*****************************************************
* NAME
*   arad_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Check if an HR scheduling element is assigned to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  ARAD_SCH_PORT_ID  port_ndx -
*     The index of the port to set.
*     Range: 0 - 79
*   SOC_SAND_OUT  uint8           is_port_hr -
*     If TRUE, the HR is assigned to the port.
*     Otherwise - unasigned.
*     HR that is not assigned to port can be used as
*     HR scheduler.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  arad_sch_hr_to_port_assign_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID    port_ndx,
    SOC_SAND_OUT uint8           *is_port_hr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_idx,
    reg_idx,
    fld_val = 0,
    is_port_hr_val;
  uint32
    base_port_tc;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_HR_TO_PORT_ASSIGN_GET);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_ndx, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  reg_idx = ARAD_REG_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);
  fld_idx = ARAD_FLD_IDX_GET(base_port_tc, ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, READ_SCH_PORT_ENABLE__PORTENm(unit, MEM_BLOCK_ANY, reg_idx, &fld_val));

  /* Assuming first is enable, all HRs related are enabled */
  is_port_hr_val = SOC_SAND_GET_BIT(fld_val, fld_idx);
  *is_port_hr = SOC_SAND_NUM2BOOL(is_port_hr_val);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("arad_sch_hr_to_port_assign_get", 0, 0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_sch_port_sched_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO      *port_info
  )
{
  uint32
    res;
  uint32
    priority_i,
    nof_priorities,
    tcg_i;
  uint8
    is_one_member;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_ndx, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  /* No support in ARAD */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->lowest_hp_class, SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST, SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST,
      ARAD_SCH_HP_CLASS_OUT_OF_RANGE_ERR, 30, exit
    );

  SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(
      port_info->hr_mode, SOC_TMC_SCH_SE_HR_MODE_LAST,
      ARAD_SCH_HR_MODE_INVALID_ERR, 35, exit
    );


  for (priority_i = 0; priority_i < nof_priorities; priority_i++)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
        port_info->hr_modes[priority_i], SOC_TMC_SCH_HR_MODE_NONE, SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ,
        ARAD_SCH_HR_MODE_INVALID_ERR, 35, exit
      );    
  }

  if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    /* TCG enable */
    for (priority_i = 0; priority_i < nof_priorities; priority_i++) 
    {
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
          port_info->tcg_ndx[priority_i], ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );
    }

    /* Check each single member TCG that only one priority is mapped to { */
    for (tcg_i = ARAD_SCH_SINGLE_MEMBER_TCG_START; tcg_i <= ARAD_SCH_SINGLE_MEMBER_TCG_END; tcg_i++)
    {
      is_one_member = FALSE;
      for (priority_i = 0; priority_i < nof_priorities; priority_i++) 
      {
        if (port_info->tcg_ndx[priority_i] == tcg_i)
        {
          if (is_one_member)
          {
            /* More than one member set to this tcg */
            SOC_SAND_SET_ERROR_CODE(ARAD_TCG_SINGLE_MEMBER_ERR, 100+tcg_i, exit);
          }
          else
          {
            is_one_member = TRUE;
          }
        }     
      }
    }
    /* Check each single member TCG that only one priority is mapped to } */
  }
  else
  {
    /* Verify all tcgs are mapped to default */
    for (priority_i = 0; priority_i < nof_priorities; priority_i++) {
      SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(
        port_info->tcg_ndx[priority_i], ARAD_SCH_TCG_NDX_DEFAULT,
        ARAD_TCG_NOT_SUPPORTED_ERR, 110, exit
      );
    }
  }

  if (port_info->enable == FALSE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      port_info->max_expected_rate, ARAD_IF_MAX_RATE_MBPS,
      ARAD_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 140, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->max_expected_rate, 1, ARAD_IF_MAX_RATE_MBPS,
      ARAD_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 150, exit
    );
  }

  if (port_info->group == ARAD_SCH_GROUP_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_GROUP_ERR, 160, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_verify()",0,0);
}

/*********************************************************************
* NAME:
*     arad_sch_port_tcg_weight_set/get _unsafe
* TYPE:
*   PROC
* DATE:
*  
* FUNCTION:
*     Sets, for a specified TCG within a certain Port
*     its excess rate. Excess traffic is scheduled between other TCGs
*     according to a weighted fair queueing or strict priority policy. 
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  ARAD_SCH_PORT_ID          port_id -
*     Port id, 0 - 255. Set invalid in case of invalid attributes.
*  SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx -
*     TCG index. 0-7.
*  SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT      *tcg_weight -
*     TCG weight information.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function must only be called for eight priorities port.
*********************************************************************/
uint32
  arad_sch_port_tcg_weight_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID          port_id,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT       *tcg_weight
  )
{
  uint32
    res,
    data,
    is_tcg_weight_val,
    field_val;
  uint32
    base_port_tc,
    ps;
  soc_reg_above_64_val_t
    data_above_64;
  soc_field_t
    field_name;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_SET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);


  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  ps = ARAD_BASE_PORT_TC2PS(base_port_tc);

  /* Set TCG weight valid { */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, ps, &data));
  field_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,PG_WFQ_VALIDf);
 
  is_tcg_weight_val = tcg_weight->tcg_weight_valid ? 1:0; 
  SOC_SAND_SET_BIT(field_val, is_tcg_weight_val, tcg_ndx);
  
  soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_set(unit,&data,PG_WFQ_VALIDf,field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, ps, &data));    
  /* Set TCG weight valid } */

  /* Set TCG weight { */
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_PORT_SCHEDULER_WEIGHTS__PSWm(unit, MEM_BLOCK_ANY, ps, data_above_64));
    switch (tcg_ndx)
    {
    case 0:
      field_name = WFQ_PG_0_WEIGHTf;
      break;
    case 1:
      field_name = WFQ_PG_1_WEIGHTf;
      break;
    case 2:
      field_name = WFQ_PG_2_WEIGHTf;
      break;
    case 3:
      field_name = WFQ_PG_3_WEIGHTf;
      break;
    case 4:
      field_name = WFQ_PG_4_WEIGHTf;
      break;
    case 5:
      field_name = WFQ_PG_5_WEIGHTf;
      break;
    case 6:
      field_name = WFQ_PG_6_WEIGHTf;
      break;
    case 7:
      field_name = WFQ_PG_7_WEIGHTf;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = tcg_weight->tcg_weight;

    soc_SCH_PORT_SCHEDULER_WEIGHTS__PSWm_field32_set(unit,data_above_64,field_name,field_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_SCH_PORT_SCHEDULER_WEIGHTS__PSWm(unit, MEM_BLOCK_ANY, ps, data_above_64));    
  }
  /* Set TCG weight } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_set_unsafe()",port_id,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID          port_id,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT       *tcg_weight
  )
{
  uint32
    res,
    data,
    is_tcg_weight_val,
    field_val;
  uint32
    base_port_tc,
    ps;
  soc_reg_above_64_val_t
    data_above_64;
  soc_field_t
    field_name;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_GET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  ps = ARAD_BASE_PORT_TC2PS(base_port_tc);

  /* Get TCG weight valid { */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, MEM_BLOCK_ANY, ps, &data));
  field_val = soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_get(unit,&data,PG_WFQ_VALIDf);
 
  is_tcg_weight_val = SOC_SAND_GET_BIT(field_val, tcg_ndx);
  tcg_weight->tcg_weight_valid = SOC_SAND_NUM2BOOL(is_tcg_weight_val);
  /* Get TCG weight valid } */

  /* Get TCG weight { */
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_PORT_SCHEDULER_WEIGHTS__PSWm(unit, MEM_BLOCK_ANY, ps, data_above_64));
    switch (tcg_ndx)
    {
    case 0:
      field_name = WFQ_PG_0_WEIGHTf;
      break;
    case 1:
      field_name = WFQ_PG_1_WEIGHTf;
      break;
    case 2:
      field_name = WFQ_PG_2_WEIGHTf;
      break;
    case 3:
      field_name = WFQ_PG_3_WEIGHTf;
      break;
    case 4:
      field_name = WFQ_PG_4_WEIGHTf;
      break;
    case 5:
      field_name = WFQ_PG_5_WEIGHTf;
      break;
    case 6:
      field_name = WFQ_PG_6_WEIGHTf;
      break;
    case 7:
      field_name = WFQ_PG_7_WEIGHTf;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = soc_SCH_PORT_SCHEDULER_WEIGHTS__PSWm_field32_get(unit,data_above_64,field_name);
    tcg_weight->tcg_weight = field_val;
  }
  /* Get TCG weight } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_get_unsafe()",port_id,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_set_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID          port_id,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT       *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_port_tc,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_SET_VERIFY_UNSAFE);
  
  /* Verify port */
  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  if (base_port_tc == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 17, exit)
  }

  /* API functionality only when port is with 8 priorities. */
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_id, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);

  if (!(nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 20, exit);
  }

  /* Verify TCG */
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );

  /* Verify TCG weight */
  if (tcg_weight->tcg_weight_valid)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
            tcg_weight->tcg_weight, ARAD_SCH_TCG_WEIGHT_MIN, ARAD_SCH_TCG_WEIGHT_MAX, 
            ARAD_SCH_TCG_WEIGHT_OUT_OF_RANGE_ERR, 35, exit
          );
  }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_set_verify_unsafe()",port_id,tcg_ndx);
}

uint32
  arad_sch_port_tcg_weight_get_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID          port_id,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT  ARAD_SCH_TCG_WEIGHT       *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_port_tc,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_TCG_WEIGHT_GET_VERIFY_UNSAFE);
  
  /* Verify port */
  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  if (base_port_tc == ARAD_SCH_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 17, exit)
  }

  /* API functionality only when port is with 8 priorities. */

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_id, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);

  if (!(nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 20, exit);
  }

  /* Verify TCG */
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tcg_weight_get_verify_unsafe()",port_id,tcg_ndx);
}

/*********************************************************************
*     Sets the scheduler-port state (enable/disable), and its
*     HR mode of operation (single or dual). The driver writes
*     to the following tables: Scheduler Enable Memory (SEM),
*     HR-Scheduler-Configuration (SHC), Flow Group Memory
*     (FGM)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_sch_port_sched_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  ARAD_SCH_SE_INFO
    se;
  uint32 
    nof_priorities,
    priority_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_ndx, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);
      
  /*
   *  Assign scheduler group {
   */
  if (port_info->enable == FALSE)
  {
    res = arad_sch_hr_to_port_assign_set(
            unit,
            port_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

    /* Go over all HRs (per priority) to disable them */    
    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(
              unit,
              port_ndx,
              priority_i,
              &(se.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      res = arad_sch_se_state_set(
            unit,
            se.id,
            FALSE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        /* By default, Map SE to TCG 0, in case of 8 priorities */
        res = arad_sch_hr_tcg_map_set(
                unit,
                se.id,
                ARAD_SCH_TCG_NDX_DEFAULT
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }

    }
  }
  else
  {
    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(
              unit,
              port_ndx,
              priority_i,
              &(se.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      se.state = (port_info->enable == TRUE)?ARAD_SCH_SE_STATE_ENABLE:ARAD_SCH_SE_STATE_DISABLE;
      se.is_dual = FALSE;
      se.type = ARAD_SCH_SE_TYPE_HR;
      se.type_info.hr.mode = port_info->hr_modes[priority_i];

      /*
       * The port HR scheduler element group is set here
       * based on port info, not on se info.
       */
      se.group = ARAD_SCH_GROUP_NONE;

      res = arad_sch_se_set_unsafe(
              unit,
              &se,
              1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /* Map SE to TCG, only in case of 8 priorities */
      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        res = arad_sch_hr_tcg_map_set(
                unit,
                se.id,
                port_info->tcg_ndx[priority_i]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }
    }

    arad_ARAD_SCH_PORT_HP_CLASS_INFO_clear(&hp_class_info);

  /*
   *  Assign scheduler group }
   */

    res = arad_sch_hr_to_port_assign_set(
            unit,
            port_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_set_unsafe()",0,0);
}



/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_sch_port_sched_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID        port_ndx,
    SOC_SAND_OUT ARAD_SCH_PORT_INFO      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_SCH_SE_INFO
    se;
  ARAD_SCH_PORT_LOWEST_HP_HR_CLASS
    hp_class_select_idx = 0;
  ARAD_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  uint8
    is_port_hr;
  uint32
    nof_priorities,
    priority_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_SCHED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = arad_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_sch_hr_to_port_assign_get(
          unit,
          port_ndx,
          &is_port_hr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  port_info->enable = is_port_hr;

  if (is_port_hr)
  {
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_ndx, 0, &nof_priorities);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
      res = arad_sch_port_tc2se_id(
              unit,
              port_ndx,
              priority_i,
              &(se.id)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /*
       * Read scheduler properties from the device
       */
      res = arad_sch_se_get_unsafe(
              unit,
              se.id,
              &se
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      if ((se.state == ARAD_SCH_SE_STATE_DISABLE) && (is_port_hr == TRUE))
      {
        port_info->enable = FALSE;
      }
      
      if (se.type != ARAD_SCH_SE_TYPE_HR)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_SCH_SE_PORT_SE_TYPE_NOT_HR_ERR, 30, exit);
      }

      port_info->hr_modes[priority_i] = se.type_info.hr.mode;
      
      if (nof_priorities == ARAD_TCG_NOF_PRIORITIES_SUPPORT)
      {
        res = arad_sch_hr_tcg_map_get(
                unit,
                se.id,
                &(port_info->tcg_ndx[priority_i])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
      }
    }
  } 

  res = arad_sch_hr_lowest_hp_class_select_get(
          unit,
          port_ndx,
          &hp_class_select_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  arad_ARAD_SCH_PORT_HP_CLASS_INFO_clear(&hp_class_info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_sched_get_unsafe()",0,0);
}


uint32
  arad_sch_se2port_tc_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_SE_ID    se_id,
    SOC_SAND_OUT ARAD_SCH_PORT_ID  *port_id,
    SOC_SAND_OUT uint32            *tc 
  )
{
  uint32
    port_ndx,
    base_port_tc,
    port_tc_to_check,
    nof_priorities,
    res,
    tm_port;
  int
    core;
  soc_pbmp_t
    ports_bm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(
       se_id, ARAD_HR_SE_ID_MIN, ARAD_HR_SE_ID_MIN + ARAD_SCH_MAX_PORT_ID))
  {
    *port_id = ARAD_SCH_PORT_ID_INVALID;
  }
  else
  {
    *port_id = ARAD_SCH_PORT_ID_INVALID;
    port_tc_to_check = se_id - ARAD_HR_SE_ID_MIN;

    /* Find match range of base_port_tc <= port_tc_to_check(HR) < base_port_tc + nof_priorities */
    res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_PBMP_ITER(ports_bm, port_ndx)
    {
        res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &core);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 13, exit);

        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core, &base_port_tc);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

        res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, tm_port, core, &nof_priorities); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        if (SOC_SAND_IS_VAL_IN_RANGE(port_tc_to_check,base_port_tc,base_port_tc+nof_priorities-1))
        {
            /* Match */
            *tc = port_tc_to_check - base_port_tc;
            *port_id = tm_port;
            break;
        }
    }    
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_se2port_tc_id_get_unsafe()",se_id,0);
}


uint32
  arad_sch_port_tc2se_id_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID        port_id,
    SOC_SAND_IN  uint32               tc,
    SOC_SAND_OUT ARAD_SCH_SE_ID          *se_id
  )
{
  uint32
    base_port_tc,
    nof_priorities,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, port_id, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (port_id == ARAD_SCH_PORT_ID_INVALID || 
      port_id > ARAD_SCH_MAX_PORT_ID ||
      base_port_tc == ARAD_SCH_PORT_ID_INVALID ||
      tc >= nof_priorities)
  {
    *se_id = ARAD_SCH_SE_ID_INVALID;
  }
  else
  {
    *se_id = base_port_tc + tc + ARAD_HR_SE_ID_MIN;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_tc2se_id_get_unsafe()",port_id,tc);
}

/*
 * Convert a rate given in Kbits/sec units to quanta
 * The conversion is done according to:
 *                       
 *  Rate [Kbits/Sec] =   Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec] * quanta [1/clocks] 
 *  
 *                                          1
 *  Where quanta = -------------------------------------------------------------
 *                        interval_between_credits_in_clocks [clocks]
 */
STATIC
SOC_SAND_RET
  arad_sch_port_rate_kbits_per_sec_to_qaunta(
    SOC_SAND_IN       uint32    rate,     /* in Kbits/sec */
    SOC_SAND_IN       uint32   credit,   /* in Bytes */
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   quanta  /* in device clocks */
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    calc2,
    calc;

  if (NULL == quanta)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == rate)
  {
    /* Divide by zero */
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }
  /* credit * SOC_SAND_NOF_BITS_IN_CHAR = convert to bits */
  /* calc = credit [kbits] * ticks_per_sec / 1024 [clocks/sec] */
  calc = ((credit * SOC_SAND_NOF_BITS_IN_CHAR)*(SOC_SAND_DIV_ROUND(ticks_per_sec,1000)));
  /* calc2 = rate / calc [1/clocks] */
  calc2 = SOC_SAND_DIV_ROUND(rate,calc); 
  if(calc2 != 0){
      *quanta = calc2;
  }
  else{
      *quanta = 1;
  }
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}

/* 
*   Convert quanta given in
*   device clocks to rate in Kbits/sec units.
*   The conversion is done according to:
*  Rate [Kbits/Sec] =   Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec] * quanta 
*  
*                                          1
*  Where quanta = -------------------------------------------------------------
*                        interval_between_credits_in_clocks [clocks]
*/
SOC_SAND_RET
  arad_sch_port_qunta_to_rate_kbits_per_sec(
    SOC_SAND_IN       uint32    quanta, /* in device clocks */
    SOC_SAND_IN       uint32   credit,   /* in Bytes */
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      /* in Kbits/sec */
  )
{
  SOC_SAND_RET
    ex ;
  SOC_SAND_U64
    calc2;
  uint32
    calc,
    tmp;

  if (NULL == rate)
  {
    ex = SOC_SAND_NULL_POINTER_ERR ;
    goto exit ;
  }
  if (0 == quanta)
  {
    /* Divide by zero */
    ex = SOC_SAND_DIV_BY_ZERO_ERR ;
    goto exit ;
  }

  calc = ((credit * SOC_SAND_NOF_BITS_IN_CHAR))*(SOC_SAND_DIV_ROUND(ticks_per_sec,1000));                        
  soc_sand_u64_multiply_longs(calc, quanta, &calc2);
  if (soc_sand_u64_to_long(&calc2, &tmp))
  {
    /* Overflow */
    ex = SOC_SAND_OVERFLOW_ERR ;
    goto exit ;
  }
  *rate = tmp;
  ex = SOC_SAND_OK ;
exit:
  return ex ;
}
 

/*********************************************************************
*     Sets, for a specified port_priority 
*     its maximal credit rate. This API is
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_sch_port_priority_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     ARAD_SCH_PORT_ID        port_id,
    SOC_SAND_IN     uint32               priority_ndx,
    SOC_SAND_IN     ARAD_SCH_PORT_PRIORITY_RATE_INFO *info,
    SOC_SAND_OUT    ARAD_SCH_PORT_PRIORITY_RATE_INFO *exact_info
  )
{
  uint32
    res,
    quanta,
    device_ticks_per_sec,
    nof_ticks,
    rate_internal,
    exact_rate_internal,
    quanta_nof_bits,
    credit_worth,
    assigned_val,
    credit,
    cal_length,
    access_period,
    field_val,    
    data,
    tbl_data;
  uint32
    base_port_tc,
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(exact_info);

  rate_internal = info->rate;

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  quanta_nof_bits = soc_mem_field_length(unit, SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm, QUANTA_TO_ADDf);
  /*
   * Get credit size and device frequency for the following calculations
   */
  res = arad_mgmt_credit_worth_get(
            unit,
            &credit_worth
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  /* 1. calculate credit: based on: (credit_worth / assigned credit worth) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, REBOUNDED_CREDIT_WORTHf, &assigned_val));
  credit = SOC_SAND_DIV_ROUND_UP(credit_worth,assigned_val);

  /* 2. nof_ticks = device_ticks / (calendar length * access period) */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, REG_PORT_ANY,&data));
  /* cal length is +1 from register value */
  cal_length = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, data, PIR_SHAPERS_CAL_LENGTHf) + 1;
  field_val = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, data, PIR_SHAPERS_CAL_ACCESS_PERIODf);
  access_period = (field_val >> 4);

  nof_ticks = SOC_SAND_DIV_ROUND_UP(device_ticks_per_sec,(cal_length*access_period));

  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    quanta = 0;
  }
  else
  {    
    /* 3. calculate quanta */
    res = arad_sch_port_rate_kbits_per_sec_to_qaunta(
            rate_internal,
            credit,
            nof_ticks,
            &quanta
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
  }

  offset = base_port_tc + priority_ndx;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));
  soc_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm_field32_set(unit,&tbl_data,QUANTA_TO_ADDf,quanta);
  soc_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm_field32_set(unit,&tbl_data,MAX_BURSTf,info->max_burst);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));


  if (0 == quanta)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            quanta,
            credit,
            nof_ticks,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  exact_info->rate = exact_rate_internal;
  exact_info->max_burst = info->max_burst;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_priority_shaper_rate_set_unsafe()",0,0);
}

uint32
  arad_sch_port_priority_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     ARAD_SCH_PORT_ID        port_id,
    SOC_SAND_IN     uint32               priority_ndx,    
    SOC_SAND_OUT    ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  )
{
  uint32
    res,
    offset,
    quanta,
    nof_ticks,
    device_ticks_per_sec,
    rate_internal,
    credit_worth,
    assigned_val,
    credit,
    cal_length,
    access_period,
    field_val,
    data,
    tbl_data;
  uint32
    base_port_tc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(info);  

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = arad_mgmt_credit_worth_get(
          unit,
          &credit_worth
       );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, REBOUNDED_CREDIT_WORTHf, &assigned_val));
  credit = SOC_SAND_DIV_ROUND_UP(credit_worth,assigned_val);

  /* nof_ticks = device_ticks / (calendar length * access period) */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, REG_PORT_ANY,&data));
  /* cal length is +1 from register value */
  cal_length = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, data, PIR_SHAPERS_CAL_LENGTHf) + 1;
  field_val = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, data, PIR_SHAPERS_CAL_ACCESS_PERIODf);
  access_period = (field_val >> 4);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  nof_ticks = SOC_SAND_DIV_ROUND_UP(device_ticks_per_sec,(cal_length*access_period));
  
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  offset = base_port_tc + priority_ndx;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));
  quanta = soc_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm_field32_get(unit,&tbl_data,QUANTA_TO_ADDf);
  info->max_burst = soc_SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm_field32_get(unit,&tbl_data,MAX_BURSTf);

  if (0 == quanta)
  {
    rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            quanta,
            credit,
            nof_ticks,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  info->rate = rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_port_priority_shaper_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified tcg 
*     its maximal credit rate. This API is
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_sch_tcg_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     ARAD_SCH_PORT_ID        port_id,
    SOC_SAND_IN     ARAD_TCG_NDX            tcg_ndx,
    SOC_SAND_IN     ARAD_SCH_TCG_RATE_INFO *info,
    SOC_SAND_OUT    ARAD_SCH_TCG_RATE_INFO *exact_info
  )
{
  uint32
    res,
    quanta,
    device_ticks_per_sec,
    nof_ticks,
    rate_internal,
    exact_rate_internal,
    quanta_nof_bits,
    credit_worth,   
    assigned_val,
    credit,
    cal_length,
    access_period,
    field_val,
    data, 
    tbl_data;
  uint32
    base_port_tc,
    offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  arad_ARAD_SCH_TCG_RATE_INFO_clear(exact_info);

  rate_internal = info->rate;

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  quanta_nof_bits = soc_mem_field_length(unit, SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm, QUANTA_TO_ADDf);
  /*
   * Get credit size and device frequency for the following calculations
   */
  res = arad_mgmt_credit_worth_get(
            unit,
            &credit_worth
          );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  /* 1. calculate credit: based on: (credit_worth / assigned credit worth) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, REBOUNDED_CREDIT_WORTHf, &assigned_val));
  credit = SOC_SAND_DIV_ROUND_UP(credit_worth,assigned_val);

  /* 2. nof_ticks = device_ticks / (calendar length * access period) */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, REG_PORT_ANY,&data));
  /* cal length is +1 from register value */
  cal_length = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, data, CIR_SHAPERS_CAL_LENGTHf) + 1;
  field_val = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, data, CIR_SHAPERS_CAL_ACCESS_PERIODf);
  access_period = (field_val >> 4);

  nof_ticks = SOC_SAND_DIV_ROUND_UP(device_ticks_per_sec,(cal_length*access_period));

  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    quanta = 0;
  }
  else
  {    
    /* 3. calculate quanta */
    res = arad_sch_port_rate_kbits_per_sec_to_qaunta(
            rate_internal,
            credit,
            nof_ticks,
            &quanta
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
  }

  offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));
  soc_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm_field32_set(unit,&tbl_data,QUANTA_TO_ADDf,quanta);
  soc_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm_field32_set(unit,&tbl_data,MAX_BURSTf,info->max_burst);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));


  if (0 == quanta)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            quanta,
            credit,
            nof_ticks,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  exact_info->rate = exact_rate_internal;
  exact_info->max_burst = info->max_burst;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_tcg_shaper_rate_set_unsafe()",port_id,tcg_ndx);
}

uint32
  arad_sch_tcg_shaper_rate_get_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     ARAD_SCH_PORT_ID        port_id,
    SOC_SAND_IN     ARAD_TCG_NDX            tcg_ndx,    
    SOC_SAND_OUT    ARAD_SCH_TCG_RATE_INFO *info
  )
{
  uint32
    res,
    offset,
    quanta,
    nof_ticks,
    device_ticks_per_sec,
    rate_internal,
    credit_worth,
    assigned_val,
    credit,
    cal_length,
    access_period,
    field_val,
    data,
    tbl_data;
  uint32
    base_port_tc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  arad_ARAD_SCH_TCG_RATE_INFO_clear(info);  

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = arad_mgmt_credit_worth_get(
          unit,
          &credit_worth
       );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, SOC_CORE_ALL, 0, REBOUNDED_CREDIT_WORTHf, &assigned_val));
  credit = SOC_SAND_DIV_ROUND_UP(credit_worth,assigned_val);

  /* nof_ticks = device_ticks / (calendar length * access period) */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, REG_PORT_ANY,&data));
  /* cal length is +1 from register value */
  cal_length = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, data, CIR_SHAPERS_CAL_LENGTHf) + 1;
  field_val = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, data, CIR_SHAPERS_CAL_ACCESS_PERIODf);
  access_period = (field_val >> 4);

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  nof_ticks = SOC_SAND_DIV_ROUND_UP(device_ticks_per_sec,(cal_length*access_period));

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, port_id, 0, &base_port_tc);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm(unit,MEM_BLOCK_ANY,offset,&tbl_data));
  quanta = soc_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm_field32_get(unit,&tbl_data,QUANTA_TO_ADDf);
  info->max_burst = soc_SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm_field32_get(unit,&tbl_data,MAX_BURSTf);

  if (0 == quanta)
  {
    rate_internal = 0;
  }
  else
  {
    res = arad_sch_port_qunta_to_rate_kbits_per_sec(
            quanta,
            credit,
            nof_ticks,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  info->rate = rate_internal;  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sch_tcg_shaper_rate_get_unsafe()",port_id,tcg_ndx);
}

void
  arad_ARAD_SCH_PORT_PRIORITY_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_PORT_PRIORITY_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_SCH_PORT_PRIORITY_RATE_INFO));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_SCH_TCG_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_TCG_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_SCH_TCG_RATE_INFO));
    
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

