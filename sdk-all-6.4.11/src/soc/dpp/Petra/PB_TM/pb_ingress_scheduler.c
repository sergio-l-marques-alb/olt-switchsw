/* $Id: pb_ingress_scheduler.c,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_pb_ingress_scheduler.c
*
* MODULE PREFIX:  soc_pb_ingress_scheduler
*
* FILE DESCRIPTION:   in the H file.
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

#include <soc/dpp/Petra/PB_TM/pb_ingress_scheduler.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_ingress_scheduler.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>

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
*     This procedure writes the values of the high priority shapers
*     (fabric multicast, fabric unicast and local) in the clos_info
*     structure to the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ingress_scheduler_clos_hp_shapers_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
    )
{

  uint32
    res,
    exact_local_max_rate,
    exact_fabm_max_rate,
    exact_fabu_max_rate;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  regs = soc_petra_regs();

  /* Set HP Shapers { */

  /* Set HP Local Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_set(
    unit,
    &(clos_info->shapers.hp.local),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[1].shaper0_max_credit),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[1].shaper0_delay),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[1].shaper0_cal),
    &exact_local_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  exact_clos_info->shapers.hp.local.max_rate = exact_local_max_rate;

  /* Set HP Unicast Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_set(
    unit,
    &(clos_info->shapers.hp.fabric_unicast),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[1].shaper1_max_credit),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[1].shaper1_delay),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[1].shaper1_cal),
    &exact_fabu_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  exact_clos_info->shapers.hp.fabric_unicast.max_rate = exact_fabu_max_rate;

  /* Set HP Multicast Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_set(
    unit,
    &(clos_info->shapers.hp.fabric_multicast),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[2].shaper0_max_credit),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[2].shaper0_delay),
    SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[2].shaper0_cal),
    &exact_fabm_max_rate
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);
  exact_clos_info->shapers.hp.fabric_multicast.max_rate = exact_fabm_max_rate;

  /* } Set HP Shapers */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ingress_scheduler_clos_hp_shapers_set()",0,0);
}


/*********************************************************************
*     This procedure reads the values of the high priority shapers
*     (fabric multicast, fabric unicast and local) to the clos_info
*     structure from the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ingress_scheduler_clos_hp_shapers_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  regs = soc_petra_regs();

  /* Get HP Shapers { */

  /* Get HP Local Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[1].shaper0_max_credit),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[1].shaper0_delay),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[1].shaper0_cal),
          &(clos_info->shapers.hp.local)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);

  /* Get HP Unicast Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[1].shaper1_max_credit),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[1].shaper1_delay),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[1].shaper1_cal),
          &(clos_info->shapers.hp.fabric_unicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  /* Get HP Multicast Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[2].shaper0_max_credit),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[2].shaper0_delay),
          SOC_PB_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[2].shaper0_cal),
          &(clos_info->shapers.hp.fabric_multicast)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);

  /* } Get HP Shapers */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ingress_scheduler_clos_hp_shapers_get()",0,0);
}


/*}*/

#include <soc/dpp/SAND/Utils/sand_footer.h>
