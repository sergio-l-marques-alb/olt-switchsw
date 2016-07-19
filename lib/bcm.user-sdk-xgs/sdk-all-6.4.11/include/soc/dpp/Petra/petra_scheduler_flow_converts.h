/* $Id: petra_scheduler_flow_converts.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_scheduler_flow_converts.h
*
* MODULE PREFIX:  soc_petra_scheduler_flow_converts
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


#ifndef __SOC_PETRA_SCHEDULER_FLOW_CONVERTS_H_INCLUDED__
/* { */
#define __SOC_PETRA_SCHEDULER_FLOW_CONVERTS_H_INCLUDED__

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

typedef struct
{
  uint32                                sch_number;
  uint32                                cos;
  uint32                                hr_sel_dual;
  uint32                                peak_rate_man;
  uint32                                peak_rate_exp;
  uint32                                max_burst;
  uint32                                max_burst_update;
  uint32                                slow_rate_index;
} SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC;

uint32
  soc_petra_sch_INTERNAL_SUB_FLOW_to_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    SOC_PETRA_SCH_SUBFLOW           *sub_flow
    );

uint32
  soc_petra_sch_SUB_FLOW_to_INTERNAL_SUB_FLOW_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SUBFLOW             *sub_flow,
    SOC_SAND_OUT    SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow
    );

uint32
  soc_petra_sch_from_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_OUT    SOC_PETRA_SCH_SUBFLOW           *sub_flow
    );

uint32
  soc_petra_sch_to_internal_subflow_shaper_convert(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SUBFLOW           *sub_flow,
    SOC_SAND_OUT    SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC  *internal_sub_flow,
    SOC_SAND_IN     uint32                     round_up
    );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_FLOW_CONVERTS_H_INCLUDED__*/
#endif
