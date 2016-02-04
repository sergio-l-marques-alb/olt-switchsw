/* $Id: petra_scheduler_elements.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_scheduler_elements.h
*
* MODULE PREFIX:  soc_petra_scheduler_elements
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


#ifndef __SOC_PETRA_SCHEDULER_ELEMENTS_H_INCLUDED__
/* { */
#define __SOC_PETRA_SCHEDULER_ELEMENTS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
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

#define SOC_PETRA_SCH_SE_IS_CL(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, SOC_PETRA_CL_SE_ID_MIN,SOC_PETRA_CL_SE_ID_MAX)
#define SOC_PETRA_SCH_SE_IS_HR(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, SOC_PETRA_HR_SE_ID_MIN,SOC_PETRA_HR_SE_ID_MAX)
#define SOC_PETRA_SCH_SE_IS_FQ(se_id) SOC_SAND_IS_VAL_IN_RANGE(se_id, SOC_PETRA_FQ_SE_ID_MIN,SOC_PETRA_FQ_SE_ID_MAX)

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
*   soc_petra_sch_se_state_get
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Get scheduler element state (enabled/disabled) from the device
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_SE_ID          se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_OUT    uint8                   *is_se_enabled -
*     TRUE if the scheduling element is enabled
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_state_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT    uint8                *is_se_enabled
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_state_set
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Set scheduler element state (enabled/disabled) to the device
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_OUT    uint8                   is_se_enabled -
*     TRUE if the scheduling element is enabled.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    It is assumed that se_ndx is verified by the calling function.
*****************************************************/
uint32
  soc_petra_sch_se_state_set(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     uint8                is_se_enabled
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_dual_shaper_get
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_OUT    uint8                   *is_dual_shaper -
*     True if the aggregate is configured as dual shaper
*     (according to CIR/EIR configuration).
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    It is assumed that se_ndx is verified by the calling function.
*****************************************************/
uint32
  soc_petra_sch_se_dual_shaper_get(
    SOC_SAND_IN     int                unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_TYPE        se_type,
    SOC_SAND_OUT    uint8                *is_dual_shaper
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_dual_shaper_set
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_IN    uint8                   is_dual_shaper -
*     True if the aggregate is configured as dual shaper
*     (according to CIR/EIR configuration).
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_dual_shaper_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          se_ndx,
    SOC_SAND_IN     uint8                   is_dual_shaper
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_config_get
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_OUT  SOC_PETRA_SCH_SE_INFO  *se -
*     Scheduling element info.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    It is assumed that se_ndx is verified by the calling function.
*****************************************************/
uint32
  soc_petra_sch_se_config_get(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx,
    SOC_SAND_IN   SOC_PETRA_SCH_SE_TYPE  se_type,
    SOC_SAND_OUT  SOC_PETRA_SCH_SE_INFO  *se
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_config_set
* TYPE:
*   PROC
* DATE:
*   13/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN   SOC_PETRA_SCH_SE_INFO  *se -
*     Scheduling element info.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Dual shaper configuration (part of the se configuration)
*   is only set when the spouse aggregate is enabled -
*   after enabling the second aggregate.
*   When the spouse is not enabled, dual shaper configuration  will be unset.
*****************************************************/
uint32
  soc_petra_sch_se_config_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO     *se,
    SOC_SAND_IN  uint32              nof_subflows
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_verify_unsafe
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Verify scheduling element info.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO      *se -
*     scheduling element info.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO      *se
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_get_unsafe
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx -
*     Scheduling element index. Range: 0 - 16K-1.
*   SOC_SAND_OUT  SOC_PETRA_SCH_SE_INFO  *se -
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_get_unsafe(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   SOC_PETRA_SCH_SE_ID    se_ndx,
    SOC_SAND_OUT  SOC_PETRA_SCH_SE_INFO  *se
  );

/*****************************************************
* NAME
*   soc_petra_sch_se_set_unsafe
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Clear from the code.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_SE_INFO      *se -
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_set_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO      *se,
    SOC_SAND_IN  uint32              nof_subflows
  );

/* CL { */


/*****************************************************
* NAME
*   soc_petra_sch_se_id_and_type_match_verify
* TYPE:
*   PROC
* DATE:
*   24/10/2007
* FUNCTION:
*  Verify the id is in the range
*   that matches se_type id-s range
* INPUT:
*   SOC_PETRA_SCH_SE_ID                   se_id -
*   SOC_PETRA_SCH_SE_TYPE                 se_type -
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_se_id_and_type_match_verify(
    SOC_PETRA_SCH_SE_ID                   se_id,
    SOC_PETRA_SCH_SE_TYPE                 se_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_verify
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets a single class type in the table. The driver writes
*     to the following tables: CL-Schedulers Type (SCT)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx -
*     The index of the class type to configure (0-255).
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type -
*     A Scheduler class type.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets a single class type in the table. The driver writes
*     to the following tables: CL-Schedulers Type (SCT)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type -
*     A Scheduler class type.
*     The class index is part of the class_type definition
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *exact_class_type -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *exact_class_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_class_type_params_set_unsafe

* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx -
*     The index of the class type to configure (0-255).
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type -
*     A Scheduler class type.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_table_verify
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_class_type_params_table_set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct -
*     A Scheduler class type table.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_table_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_table_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the scheduler class type table as a whole.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct -
*     A Scheduler class type table.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *exact_sct -
*     Loaded with the actual parameters given difference due
*     to rounding.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_table_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *exact_sct
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_class_type_params_table_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_class_type_params_table_set_unsafe
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct -
*     A Scheduler class type table.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_class_type_params_table_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_CL_CLASS_TABLE *sct
  );

/* CL } */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_ELEMENTS_H_INCLUDED__*/
#endif
