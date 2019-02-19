/* $Id: petra_scheduler_ports.c,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_scheduler_ports.c
*
* MODULE PREFIX:  soc_petra_scheduler_ports
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_scheduler_ports.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
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

STATIC SOC_PETRA_SCH_GROUP
  soc_petra_sch_least_utilized_group_get(
    SOC_SAND_IN  int             unit
  )
{
  SOC_PETRA_SCH_GROUP
    least_utilized_grp = SOC_PETRA_SCH_GROUP_LAST;
  uint32
    rate_a,
    rate_b,
    rate_c,
    min_rate;

  rate_a = soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
              unit,
              SOC_PETRA_SCH_GROUP_A
           );

  rate_b = soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
              unit,
              SOC_PETRA_SCH_GROUP_B
           );

  rate_c = soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
              unit,
              SOC_PETRA_SCH_GROUP_C
           );

  min_rate = SOC_SAND_MIN(rate_a, rate_b);
  min_rate = SOC_SAND_MIN(min_rate, rate_c);

  if (min_rate == rate_a)
  {
    least_utilized_grp = SOC_PETRA_SCH_GROUP_A;
  }
  else if (min_rate == rate_b)
  {
    least_utilized_grp = SOC_PETRA_SCH_GROUP_B;
  }
  else
  {
    least_utilized_grp = SOC_PETRA_SCH_GROUP_C;
  }

  return least_utilized_grp;

}

STATIC uint32
  soc_petra_sch_port_group_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID    port_ndx,
    SOC_SAND_IN  uint32             max_expected_rate,
    SOC_SAND_IN  SOC_PETRA_SCH_GROUP      group
  )
{
  uint32
    res = 0,
    port_expected_rate_curr,
    grp_rate_curr,
    grp_rate_new;
  SOC_PETRA_SCH_GROUP
    grp_curr;
  SOC_PETRA_SCH_SE_ID
    se_id;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_GROUP_TO_PORT_ASSIGN);

  se_id = soc_petra_sch_port2se_id(port_ndx);
  SOC_PETRA_SCH_VERIFY_SE_ID(se_id, 10, exit);

  /* update port's max expected rate { */
  port_expected_rate_curr = soc_petra_sw_db_sch_max_expected_port_rate_get(
                unit,
                port_ndx
              );

  soc_petra_sw_db_sch_max_expected_port_rate_set(
    unit,
    port_ndx,
    max_expected_rate
  );
  /* } */

  /* get current port parameters { */
  res = soc_petra_sch_se_group_get(
          unit,
          se_id,
          &grp_curr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  grp_rate_curr = soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
                           unit,
                           grp_curr
                         );
  if (grp_rate_curr < port_expected_rate_curr)
  {
    /*
     *  accumulated rate can't be less then the rate of every port
     *  that belongs to it
     */
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_GRP_AND_PORT_RATE_MISMATCH_ERR, 30, exit);
  }
  /* } */

  /* update accumulated rate database of current port group */
  soc_petra_sw_db_sch_accumulated_grp_port_rate_set(
    unit,
    grp_curr,
    grp_rate_curr - port_expected_rate_curr
  );

  if (max_expected_rate != 0)
  {
    /* Update the FGM/STL and the new port accumulated rate in SW database */
    res = soc_petra_sch_se_group_set(
            unit,
            se_id,
            group
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    grp_rate_new = soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
                             unit,
                             group
                           );

    soc_petra_sw_db_sch_accumulated_grp_port_rate_set(
      unit,
      group,
      grp_rate_new + max_expected_rate
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_group_set()",0,0);
}

/*****************************************************
* NAME
*    soc_petra_sch_hr_lowest_hp_class_select_set
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Set hp class as selected out of the available configurations
*   (access device, SHC)
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
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
  soc_petra_sch_hr_lowest_hp_class_select_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx,
    SOC_SAND_IN  uint32           hp_class_conf_idx
  )
{
  uint32
    offset,
    res;
  SOC_PETRA_SCH_SHC_TBL_DATA
    shc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_HR_LOWEST_HP_CLASS_SELECT_SET);

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    hp_class_conf_idx, SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS-1,
    SOC_PETRA_SCH_HP_CLASS_IDX_OUT_OF_RANGE_ERR, 20, exit
  );

  /*
   * Write indirect to SHC table
   */

  offset  = port_ndx;

  res = soc_petra_sch_shc_tbl_get_unsafe(
          unit,
          offset,
          &shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (shc_tbl_data.hrmask_type != hp_class_conf_idx)
  {
    shc_tbl_data.hrmask_type = hp_class_conf_idx;

    res = soc_petra_sch_shc_tbl_set_unsafe(
          unit,
          offset,
          &shc_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_hr_lowest_hp_class_select_set", 0, 0);
}

/*****************************************************
* NAME
*    soc_petra_sch_hr_lowest_hp_class_select_get
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
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
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
  soc_petra_sch_hr_lowest_hp_class_select_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID                  port_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS  *hp_class_conf_idx
  )
{
  uint32
    offset,
    res;
  SOC_PETRA_SCH_SHC_TBL_DATA
    shc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_HR_LOWEST_HP_CLASS_SELECT_GET);

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Read indirect from SHC table
   */

  offset  = port_ndx;

  res = soc_petra_sch_shc_tbl_get_unsafe(
          unit,
          offset,
          &shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *hp_class_conf_idx = shc_tbl_data.hrmask_type;

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_hr_lowest_hp_class_select_get", 0, 0);
}

/*****************************************************
* NAME
*   soc_petra_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Assign HR scheduling element to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
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
  soc_petra_sch_hr_to_port_assign_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx,
    SOC_SAND_IN  uint8           is_port_hr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_idx,
    reg_idx,
    fld_val = 0,
    is_port_hr_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_HR_TO_PORT_ASSIGN_SET);

  regs = soc_petra_regs();

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  reg_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

  is_port_hr_val = SOC_SAND_BOOL2NUM(is_port_hr);

  SOC_PETRA_FLD_GET(regs->sch.hrport_en_reg[reg_idx].hrport_en, fld_val, 30, exit);

  SOC_SAND_SET_BIT(fld_val, is_port_hr_val, fld_idx);

  SOC_PETRA_FLD_SET(regs->sch.hrport_en_reg[reg_idx].hrport_en, fld_val, 30, exit);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_hr_to_port_assign_set", 0, 0);
}

/*****************************************************
* NAME
*   soc_petra_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Check if an HR scheduling element is assigned to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
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
  soc_petra_sch_hr_to_port_assign_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx,
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
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_HR_TO_PORT_ASSIGN_GET);

  regs = soc_petra_regs();

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  reg_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

  SOC_PETRA_FLD_GET(regs->sch.hrport_en_reg[reg_idx].hrport_en, fld_val, 30, exit);
  is_port_hr_val = SOC_SAND_GET_BIT(fld_val, fld_idx);
  *is_port_hr = SOC_SAND_NUM2BOOL(is_port_hr_val);

exit:
   SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_hr_to_port_assign_get", 0, 0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_port_sched_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_SCHED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->lowest_hp_class, SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1, SOC_PETRA_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED,
      SOC_PETRA_SCH_HP_CLASS_OUT_OF_RANGE_ERR, 30, exit
    );

  if (port_info->enable == FALSE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      port_info->max_expected_rate, SOC_PETRA_IF_MAX_RATE_MBPS,
      SOC_PETRA_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 40, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      port_info->max_expected_rate, 1, SOC_PETRA_IF_MAX_RATE_MBPS,
      SOC_PETRA_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR, 50, exit
    );
  }

  if (port_info->group == SOC_PETRA_SCH_GROUP_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_PORT_GROUP_ERR, 60, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_sched_verify()",0,0);
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
  soc_petra_sch_port_sched_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info
  )
{
  uint32
    curr_max_expected_rate,
    res = SOC_SAND_OK;
  uint32
    hp_class_conf_idx = 0,
    hp_class_conf_match_idx = 0;
  SOC_PETRA_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  SOC_PETRA_SCH_SE_INFO
    se;
  SOC_PETRA_SCH_GROUP
    group;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_SCHED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  se.id = soc_petra_sch_port2se_id(port_ndx);

  /*
   *  Assign scheduler group {
   */
  if (port_info->enable == FALSE)
  {
    /* close port */
    res = soc_petra_sch_port_group_set(
              unit,
              port_ndx,
              0,
              SOC_PETRA_SCH_GROUP_AUTO
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    res = soc_petra_sch_hr_to_port_assign_set(
            unit,
            port_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

    res = soc_petra_sch_se_state_set(
            unit,
            se.id,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  }
  else
  {
    se.state = (port_info->enable == TRUE)?SOC_PETRA_SCH_SE_STATE_ENABLE:SOC_PETRA_SCH_SE_STATE_DISABLE;
    se.is_dual = FALSE;
    se.type = SOC_PETRA_SCH_SE_TYPE_HR;
    se.type_info.hr.mode = port_info->hr_mode;

    /*
     * The port HR scheduler element group is set here
     * based on port info, not on se info.
     */
    se.group = SOC_PETRA_SCH_GROUP_NONE;

    res = soc_petra_sch_se_set_unsafe(
            unit,
            &se,
            1
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_sch_port_hp_class_conf_get_unsafe(
            unit,
            &hp_class_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    hp_class_conf_match_idx = SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS;
    for (hp_class_conf_idx = 0; hp_class_conf_idx < SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS; hp_class_conf_idx++)
    {
      if(hp_class_info.lowest_hp_class[hp_class_conf_idx] == port_info->lowest_hp_class)
      {
        hp_class_conf_match_idx = hp_class_conf_idx;
      }
    }

    if (hp_class_conf_match_idx == SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS)
    {
      /* no matching index was found */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_HP_CLASS_NOT_AVAILABLE_ERR,60, exit);
    }

    res = soc_petra_sch_hr_lowest_hp_class_select_set(
            unit,
            port_ndx,
            hp_class_conf_match_idx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (port_info->group == SOC_PETRA_SCH_GROUP_AUTO)
    {
      group = soc_petra_sch_least_utilized_group_get(unit);
    }
    else
    {
      group = port_info->group;
    }

    curr_max_expected_rate = soc_petra_sw_db_sch_max_expected_port_rate_get(
                             unit,
                             port_ndx
                           );
    if ((curr_max_expected_rate == 0))
    {
      /*
       *  This is an "open port" operation,
       *  or a group explicitly assigned by the user -
       *  assign the scheduler group in the FGM
       */
      res = soc_petra_sch_port_group_set(
              unit,
              port_ndx,
              port_info->max_expected_rate,
              group
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

  /*
   *  Assign scheduler group }
   */

    res = soc_petra_sch_hr_to_port_assign_set(
            unit,
            port_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_sched_set_unsafe()",0,0);
}



/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_port_sched_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO      *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_SE_INFO
    se;
  SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS
    hp_class_select_idx = 0;
  SOC_PETRA_SCH_PORT_HP_CLASS_INFO
    hp_class_info;
  uint8
    is_port_hr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_SCHED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_petra_sch_port_id_verify_unsafe(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  se.id = soc_petra_sch_port2se_id(port_ndx);

  /*
   * Read scheduler properties from the device
   */
  res = soc_petra_sch_se_get_unsafe(
          unit,
          se.id,
          &se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_sch_hr_to_port_assign_get(
          unit,
          port_ndx,
          &is_port_hr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if ((se.state == SOC_PETRA_SCH_SE_STATE_ENABLE) && (is_port_hr == TRUE))
  {
    port_info->enable = TRUE;
  }
  else
  {
    port_info->enable = FALSE;
  }

  if (se.type != SOC_PETRA_SCH_SE_TYPE_HR)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_SE_PORT_SE_TYPE_NOT_HR_ERR, 30, exit);
  }

  port_info->hr_mode = se.type_info.hr.mode;

  res = soc_petra_sch_hr_lowest_hp_class_select_get(
          unit,
          port_ndx,
          &hp_class_select_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_sch_port_hp_class_conf_get_unsafe(
            unit,
            &hp_class_info
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    hp_class_select_idx, SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS-1,
    SOC_PETRA_SCH_HP_CLASS_NOT_AVAILABLE_ERR, 60, exit
  );

  port_info->lowest_hp_class = hp_class_info.lowest_hp_class[hp_class_select_idx];

  port_info->max_expected_rate = soc_petra_sw_db_sch_max_expected_port_rate_get(
                                    unit,
                                    port_ndx
                                  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_sched_get_unsafe()",0,0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  )
{
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_HP_CLASS_CONF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(hp_class_info);

  for (idx = 0; idx < SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS; idx++)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      hp_class_info->lowest_hp_class[idx], SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1, SOC_PETRA_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED,
      SOC_PETRA_SCH_HP_CLASS_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_hp_class_conf_verify()",0,0);
}


/*********************************************************************
*   Sets the group of available configurations for high priority
*   hr class settings.
*   Out of 5 possible configurations, 4 are available at any time.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  )
{
  uint32
    curr_class_internal,
    res;
  const uint32
    hp_class_internal[SOC_PETRA_SCH_LOW_FC_NOF_VALID_CONFS] =
    {0x1, 0x3, 0x7, 0x7fff, 0x1fff};
  uint32
    hp_class_conf_idx = 0;
  SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS
    curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_NONE;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *curr_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_HP_CLASS_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hp_class_info);

  regs = soc_petra_regs();

  for (hp_class_conf_idx = 0; hp_class_conf_idx < SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS; hp_class_conf_idx++)
  {
    curr_class = hp_class_info->lowest_hp_class[hp_class_conf_idx];
    curr_class_internal = hp_class_internal[curr_class-1];

    switch(hp_class_conf_idx)
    {
    case 0:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask0);
      break;
    case 1:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask1);
        break;
    case 2:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_2_3_reg.hrfcmask2);
        break;
    case 3:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_2_3_reg.hrfcmask3);
        break;
    /* must default. Otherwise - compilation error */
    /* coverity[dead_error_begin:FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_HP_CLASS_IDX_OUT_OF_RANGE_ERR, 10, exit);
        break;
    }

    SOC_PETRA_IMPLICIT_FLD_SET(*curr_fld, curr_class_internal, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_hp_class_conf_set_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  )
{
  uint32
    curr_class_internal,
    res;
  uint32
    hp_class_conf_idx = 0;
  SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS
    curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_NONE;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *curr_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PORT_HP_CLASS_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hp_class_info);

  regs = soc_petra_regs();

  for (hp_class_conf_idx = 0; hp_class_conf_idx < SOC_PETRA_SCH_LOW_FC_NOF_AVAIL_CONFS; hp_class_conf_idx++)
  {
    switch(hp_class_conf_idx)
    {
    case 0:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask0);
      break;
    case 1:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_0_1_reg.hrfcmask1);
        break;
    case 2:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_2_3_reg.hrfcmask2);
        break;
    case 3:
      curr_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.hr_flow_control_mask_2_3_reg.hrfcmask3);
        break;
    /* must default. Otherwise - compilation error */
    /* coverity[dead_error_begin:FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_HP_CLASS_IDX_OUT_OF_RANGE_ERR, 10, exit);
        break;
    }

    SOC_PETRA_IMPLICIT_FLD_GET(*curr_fld, curr_class_internal, 10, exit);

    switch(curr_class_internal)
    {
    case 0x1:
      curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;
      break;
    case 0x3:
      curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF2;
      break;
    case 0x7:
      curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF3;
        break;
    case 0x7fff:
      curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ;
        break;
    case 0x1fff:
      curr_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_HP_CLASS_VAL_INVALID_ERR, 20, exit);
        break;
    }

    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      curr_class, SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1, SOC_PETRA_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED,
      SOC_PETRA_SCH_HP_CLASS_OUT_OF_RANGE_ERR, 30, exit
    );

    hp_class_info->lowest_hp_class[hp_class_conf_idx] = curr_class;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_port_hp_class_conf_get_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
