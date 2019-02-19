/* $Id: petra_ingress_scheduler.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_ingress_scheduler.c
*
* MODULE PREFIX:  soc_petra_ingress_scheduler
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

#include <soc/dpp/Petra/petra_ingress_scheduler.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_ingress_scheduler.h>
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_CONVERSION_TEST_REGRESSION_STEP 9999
#define SOC_PETRA_CONVERSION_TEST_MAX_RATE_DIFF 250000


#define SOC_PA_ING_SCH_CLOS_NOF_SHAPERS SOC_PETRA_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_ING_SCH_CLOS_NOF_SHAPERS \
    (SOC_PETRA_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS + SOC_PETRA_ING_SCH_CLOS_NOF_HP_SHAPERS)
#else
#define SOC_PB_ING_SCH_CLOS_NOF_SHAPERS SOC_PA_ING_SCH_CLOS_NOF_SHAPERS
#endif

#define SOC_PETRA_ING_SCH_CLOS_NOF_SHAPERS \
  SOC_PETRA_CST_VALUE_DISTINCT(ING_SCH_CLOS_NOF_SHAPERS, uint32)

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
* NAME:
*     soc_petra_ingress_scheduler_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_petra_ingress_scheduler_regs_init(
    SOC_SAND_IN  int                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_REGS_INIT);


 SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_INIT);

  res = soc_petra_ingress_scheduler_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_init()",0,0);
}

/*********************************************************************
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  SOC_SAND_MAGIC_NUM_VERIFY(mesh_info);
  SOC_SAND_MAGIC_NUM_VERIFY(exact_mesh_info);

  for (index = 0;index < SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS;index++)
  {
    if (mesh_info->contexts[index].weight > SOC_PETRA_ING_SCH_MAX_WEIGHT_VALUE)
    {
      /*ERROR - weight out of range*/
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR, 10, exit);
    }
    if (mesh_info->contexts[index].shaper.max_burst > SOC_PETRA_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
      /*ERROR - max_credit out of range*/
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 20, exit);
    }
    if (mesh_info->contexts[index].id > SOC_PETRA_ING_SCH_MAX_ID_VALUE)
    {
      /*ERROR - id out of range*/
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_MESH_ID_OUT_OF_RANGE_ERR, 30, exit);
    }
  }
  if (mesh_info->total_rate_shaper.max_burst > SOC_PETRA_ING_SCH_MAX_MAX_CREDIT_VALUE)
  {
    /*ERROR - max_credit out of range*/
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (mesh_info->nof_entries > SOC_PETRA_ING_SCH_MAX_NOF_ENTRIES)
  {
    /*ERROR - nof_entries out of range*/
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_MESH_NOF_ENTRIES_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_verify()",0,0);
}

/*********************************************************************
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  uint32
    res,
    exact_total_max_rate,
    exact_max_rate;
  uint32
    index;
  SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO
    current_context;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    wfq_weights[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_max_crdts[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_delays[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_cals[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  res = soc_petra_ingress_scheduler_mesh_reg_flds_db_get(
          unit,
          wfq_weights,
          shaper_max_crdts,
          shaper_delays,
          shaper_cals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

  sal_memcpy(
    exact_mesh_info,
    mesh_info,
    sizeof(SOC_PETRA_ING_SCH_MESH_INFO)
  );

  for (index=0;index<(mesh_info->nof_entries);index++)
  {
    current_context = mesh_info->contexts[index];

    /* write WfqWeight field */
    SOC_PETRA_IMPLICIT_FLD_SET(wfq_weights[current_context.id], current_context.weight, 20, exit);

    /*write Shaper fields*/
    res = soc_petra_ingress_scheduler_shaper_values_set(
            unit,
            &(current_context.shaper),
            &(shaper_max_crdts[current_context.id]),
            &(shaper_delays[current_context.id]),
            &(shaper_cals[current_context.id]),
            &exact_max_rate
          );

    SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);

    exact_mesh_info->contexts[index].shaper.max_rate = exact_max_rate;

  }

  /*write Total Shaper fields*/

  res = soc_petra_ingress_scheduler_shaper_values_set(
          unit,
          &mesh_info->total_rate_shaper,
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_max_credit_reg.shaper8_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_delay_reg.shaper8_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_cal_reg.shaper8_cal),
          &exact_total_max_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,40,exit);

  exact_mesh_info->total_rate_shaper.max_rate = exact_total_max_rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_set_unsafe()",0,0);
}

/*********************************************************************
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info
  )
{
  uint32
    res,
    index;
  SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO
    current_context;
  SOC_PETRA_ING_SCH_SHAPER
    current_shaper;
  SOC_PETRA_REG_FIELD
    wfq_weights[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_max_crdts[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_delays[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shaper_cals[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS];
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);

  regs = soc_petra_regs();

  res = soc_petra_ingress_scheduler_mesh_reg_flds_db_get(
          unit,
          wfq_weights,
          shaper_max_crdts,
          shaper_delays,
          shaper_cals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

  for (index=0;index<SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS;index++)
  {
    current_context.id = index;

    /*write WfqWeight field*/
    SOC_PETRA_IMPLICIT_FLD_GET(wfq_weights[index],current_context.weight , 20, exit);

    /*write Shaper fields*/
    res = soc_petra_ingress_scheduler_shaper_values_get(
            unit,
            &(shaper_max_crdts[index]),
            &(shaper_delays[index]),
            &(shaper_cals[index]),
            &(current_context.shaper)
          );

    SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);

    sal_memcpy(
      &(mesh_info->contexts[index]),
      &(current_context),
      sizeof(SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO)
    );
  }

  /* write Total Shaper fields */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_max_credit_reg.shaper8_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_delay_reg.shaper8_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper8_cal_reg.shaper8_cal),
          &current_shaper
        );

  SOC_SAND_CHECK_FUNC_RESULT(res,40,exit);

  sal_memcpy(
    &(mesh_info->total_rate_shaper),
    &(current_shaper),
    sizeof(SOC_PETRA_ING_SCH_SHAPER)
  );
  mesh_info->nof_entries = SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure reads the values of the the shaper (max_burst, max_rate)
*     from the suitable registers fields (ShaperMaxCredit, ShaperDelay, ShaperCal).
*     The max_rate value of the Shaper structure is retrieved from the
*     ShaperDelay & ShaperCal fields, using an additional function that
*     converts the suitable values to max_rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_ingress_scheduler_shaper_values_get(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *max_credit_fld,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *delay_fld,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *cal_fld,
    SOC_SAND_OUT  SOC_PETRA_ING_SCH_SHAPER      *shaper
  )
{

  uint32
    res;
  uint32
    shaper_delay_2_clocks,
    shaper_cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_SHAPER_VALUES_GET);

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  /* Get Shaper Values { */

  /* read ShaperMaxCredit field */
  SOC_PETRA_IMPLICIT_FLD_GET(*max_credit_fld, shaper->max_burst, 10, exit);

  /* write ShaperDelay field */
  SOC_PETRA_IMPLICIT_FLD_GET(*delay_fld, shaper_delay_2_clocks, 20, exit);

  /* write ShaperCal field */
  SOC_PETRA_IMPLICIT_FLD_GET(*cal_fld, shaper_cal, 30,exit);

  /* convert delay and cal to max_rate */
  res = soc_petra_ingress_scheduler_delay_cal_to_max_rate_form(
          unit,
          (shaper_delay_2_clocks*2),
          shaper_cal,
          &(shaper->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,4,exit);

  /* Get Shaper Values } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_shaper_values_get()",0,0);
}

/*********************************************************************
*     This procedure converts the ingress scheduler shaper register field
*     values:
*     1. ShaperDelay: Time interval to add the credit.
*        in two clocks cycles resolution.
*     2. ShaperCal: Credit to add, in bytes resolution.
*
*     to the max_rate value of the shaper structure in the API (in kbps)
*
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_ingress_scheduler_delay_cal_to_max_rate_form(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          shaper_delay,
    SOC_SAND_IN  uint32          shaper_cal,
    SOC_SAND_OUT uint32          *max_rate
  )
{
  uint32
    res,
    device_ticks_per_sec,
    exact_rate_in_kbits_per_sec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_DELAY_CAL_TO_MAX_RATE_FORM);

  SOC_SAND_CHECK_NULL_INPUT(max_rate);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  if (shaper_cal == 0)
  {
    exact_rate_in_kbits_per_sec = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            shaper_delay,
            shaper_cal,
            device_ticks_per_sec,
            &exact_rate_in_kbits_per_sec
            );
    if (res == SOC_SAND_OVERFLOW_ERR)
    {
      /*
       *	Unshaped
       */
      exact_rate_in_kbits_per_sec = SOC_PETRA_ING_SCH_MAX_MAX_CREDIT_VALUE;
    }
  }

  *max_rate = exact_rate_in_kbits_per_sec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_delay_cal_to_max_rate_form()",0,0);
}

/*********************************************************************
*     This procedure is used for the Mesh topology. This procedure initializes
*     arrays of shaper_max_crdts, shaper_delays and shaper_cals with the
*     appropriate fields. This is done fo0r easier implementation of the set and
*     get functions.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_reg_flds_db_get(
    SOC_SAND_IN  int       unit,
    SOC_PETRA_REG_FIELD          wfq_weights[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_max_crdts[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_delays[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_cals[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS]
    )
{

  uint32
    index,
    wfq_index,
    shaper_index;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_REG_FLDS_DB_GET);

  regs = soc_petra_regs();

  /* Set Mesh Fields { */

  for (index = 0 ; index < SOC_PETRA_NOF_WFQ_REGS ; ++index)
  {
    wfq_index = index * (SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS / SOC_PETRA_NOF_WFQ_REGS);
    wfq_weights[wfq_index + 0] = SOC_PETRA_REG_DB_ACC(regs->ipt.wfq_weight_reg[index].wfq0_weight);
    wfq_weights[wfq_index + 1] = SOC_PETRA_REG_DB_ACC(regs->ipt.wfq_weight_reg[index].wfq1_weight);
    wfq_weights[wfq_index + 2] = SOC_PETRA_REG_DB_ACC(regs->ipt.wfq_weight_reg[index].wfq2_weight);
    wfq_weights[wfq_index + 3] = SOC_PETRA_REG_DB_ACC(regs->ipt.wfq_weight_reg[index].wfq3_weight);
  }
  for (index = 0 ; index < SOC_PETRA_NOF_SHAPER_MAX_CREDIT_REGS ; ++index)
  {
    shaper_index = index * (SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS / SOC_PETRA_NOF_SHAPER_MAX_CREDIT_REGS);
    shaper_max_crdts[shaper_index + 0] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_max_credit_reg[index].shaper0_max_credit);
    shaper_max_crdts[shaper_index + 1] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_max_credit_reg[index].shaper1_max_credit);
  }
  for (index = 0 ; index < SOC_PETRA_NOF_SHAPER_DELAY_REGS ; ++index)
  {
    shaper_index = index * (SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS / SOC_PETRA_NOF_SHAPER_DELAY_REGS);
    shaper_delays[shaper_index + 0] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_delay_reg[index].shaper0_delay);
    shaper_delays[shaper_index + 1] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_delay_reg[index].shaper1_delay);
  }
  for (index = 0 ; index < SOC_PETRA_NOF_SHAPER_CAL_REGS ; ++index)
  {
    shaper_index = index * (SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS / SOC_PETRA_NOF_SHAPER_CAL_REGS);
    shaper_cals[shaper_index + 0] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_cal_reg[index].shaper0_cal);
    shaper_cals[shaper_index + 1] = SOC_PETRA_REG_DB_ACC(regs->ipt.shaper_cal_reg[index].shaper1_cal);
  }

  /* } Set Mesh Fields */

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_reg_flds_db_get()",0,0);
}

/*********************************************************************
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  )
{
  uint32
    index,
    weights[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    shapers_max_burst[SOC_PETRA_ING_SCH_CLOS_NOF_GLOBAL_SHAPERS +
                                  SOC_PETRA_ING_SCH_CLOS_NOF_HP_SHAPERS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  SOC_SAND_MAGIC_NUM_VERIFY(clos_info);
  SOC_SAND_MAGIC_NUM_VERIFY(exact_clos_info);

  weights[0] = clos_info->weights.fabric_lp.weight1;
  weights[1] = clos_info->weights.fabric_lp.weight2;
  weights[2] = clos_info->weights.fabric_hp.weight1;
  weights[3] = clos_info->weights.fabric_hp.weight2;
  weights[4] = clos_info->weights.global_lp.weight1;
  weights[5] = clos_info->weights.global_lp.weight2;
  weights[6] = clos_info->weights.global_hp.weight1;
  weights[7] = clos_info->weights.global_hp.weight2;

  shapers_max_burst[0] = clos_info->shapers.local.max_burst;
  shapers_max_burst[1] = clos_info->shapers.fabric.max_burst;
#ifdef LINK_PB_LIBRARIES
  shapers_max_burst[2] = clos_info->shapers.hp.local.max_burst;
  shapers_max_burst[3] = clos_info->shapers.hp.fabric_unicast.max_burst;
  shapers_max_burst[4] = clos_info->shapers.hp.fabric_multicast.max_burst;
#endif

  for (index = 0 ;index < SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS ; ++index)
  {
    if (weights[index] > SOC_PETRA_ING_SCH_MAX_WEIGHT_VALUE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR, 10, exit);
    }
  }
  for (index = 0 ;index < SOC_PETRA_ING_SCH_CLOS_NOF_SHAPERS; ++index)
  {
    if (shapers_max_burst[index] > SOC_PETRA_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_verify()",0,0);
}

/*********************************************************************
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  sal_memcpy(
    exact_clos_info,
    clos_info,
    sizeof(SOC_PETRA_ING_SCH_CLOS_INFO)
    );
  /* Set Weights */
  res = soc_petra_ingress_scheduler_clos_weights_set(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  /* Set Global Shapers */
  res = soc_petra_ingress_scheduler_clos_global_shapers_set(
          unit,
          clos_info,
          exact_clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* Set HP Shapers */
    res = soc_pb_ingress_scheduler_clos_hp_shapers_set(
            unit,
            clos_info,
            exact_clos_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res,3,exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure writes all the weights in the clos_info structure
*     to the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
   soc_petra_ingress_scheduler_clos_weights_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO*  clos_info
    )
{

  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_WEIGHTS_SET);

  regs = soc_petra_regs();

  /* Set Weights { */
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[0].wfq0_weight, clos_info->weights.global_hp.weight1, 2 , exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[0].wfq1_weight, clos_info->weights.global_hp.weight2, 4 , exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[0].wfq2_weight, clos_info->weights.fabric_hp.weight1, 6 , exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[0].wfq3_weight, clos_info->weights.fabric_hp.weight2, 8 , exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[1].wfq0_weight, clos_info->weights.fabric_lp.weight1, 10, exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[1].wfq1_weight, clos_info->weights.fabric_lp.weight2, 12, exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[1].wfq2_weight, clos_info->weights.global_lp.weight1, 14, exit);
  SOC_PETRA_FLD_SET(regs->ipt.wfq_weight_reg[1].wfq3_weight, clos_info->weights.global_lp.weight2, 16, exit);
  /* } Set Weights */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_weights_set()",0,0);
}

/*********************************************************************
*     This procedure writes the values of the global shapers (fabric and local)
*     in the clos_info structure to the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_global_shapers_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
     SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
   )
{

  uint32
    res,
    exact_local_max_rate,
    exact_fab_max_rate;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_SET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  regs = soc_petra_regs();

  /* Set Global Shapers { */

  /* Set Global Local Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_set(
          unit,
          &(clos_info->shapers.local),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[0].shaper0_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[0].shaper0_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[0].shaper0_cal),
          &exact_local_max_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
  exact_clos_info->shapers.local.max_rate = exact_local_max_rate;

  /* Set Global Fabric Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_set(
          unit,
          &(clos_info->shapers.fabric),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[0].shaper1_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[0].shaper1_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[0].shaper1_cal),
          &exact_fab_max_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);
  exact_clos_info->shapers.fabric.max_rate = exact_fab_max_rate;

  /* } Set Global Shapers */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_global_shapers_set()",0,0);
}

/*********************************************************************
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  soc_petra_PETRA_ING_SCH_CLOS_INFO_clear(clos_info);

  /* Get Weights */
  res = soc_petra_ingress_scheduler_clos_weights_get(
          unit,
          clos_info
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Get Global Shapers */
  res = soc_petra_ingress_scheduler_clos_global_shapers_get(
          unit,
          clos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* Get HP Shapers */
    res = soc_pb_ingress_scheduler_clos_hp_shapers_get(
            unit,
            clos_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res,30,exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure reads the values of the global shapers (fabric and local)
*     to the clos_info structure from the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_weights_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
    )
{

  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_WEIGHTS_GET);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[0].wfq0_weight, clos_info->weights.global_hp.weight1,  0, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[0].wfq1_weight, clos_info->weights.global_hp.weight2,  2, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[0].wfq2_weight, clos_info->weights.fabric_hp.weight1,  4, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[0].wfq3_weight, clos_info->weights.fabric_hp.weight2,  6, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[1].wfq0_weight, clos_info->weights.fabric_lp.weight1,  8, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[1].wfq1_weight, clos_info->weights.fabric_lp.weight2, 12, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[1].wfq2_weight, clos_info->weights.global_lp.weight1, 14, exit);
  SOC_PETRA_FLD_GET(regs->ipt.wfq_weight_reg[1].wfq3_weight, clos_info->weights.global_lp.weight2, 16, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_weights_get()",0,0);
}

/*********************************************************************
*     This procedure reads the values of the global shapers (fabric and local)
*     to the clos_info structure from the suitable registers fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_global_shapers_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_GET);

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  regs = soc_petra_regs();

  /* Get Global Shapers { */

  /* Get Global Local Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[0].shaper0_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[0].shaper0_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[0].shaper0_cal),
          &(clos_info->shapers.local)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);

  /* Get Global Fabric Shaper */
  res = soc_petra_ingress_scheduler_shaper_values_get(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_max_credit_reg[0].shaper1_max_credit),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_delay_reg[0].shaper1_delay),
          SOC_PETRA_REG_DB_ACC_REF(regs->ipt.shaper_cal_reg[0].shaper1_cal),
          &(clos_info->shapers.fabric)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  /* } Get Global Shapers */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_global_shapers_get()",0,0);
}

/*********************************************************************
*     This procedure writes the values a given shapers (max_burst, max_rate)
*     in the clos_info structure to the suitable registers fields
*     (ShaperMaxCredit, ShaperDelay, ShaperCal). The ShaperDelay & ShaperCal
*     fields are retrieved using an additional function that converts max_rate
*     to the suitable values.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_shaper_values_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_SHAPER      *shaper,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *max_credit_fld,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *delay_fld,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *cal_fld,
    SOC_SAND_OUT uint32                  *exact_max_rate
  )
{

  uint32
    res;

  uint32
    shaper_delay_2_clocks,
    shaper_cal,
    exact;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_SHAPER_VALUES_SET);

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  /* Set Shaper { */

  /*write ShaperMaxCredit field*/
  SOC_PETRA_IMPLICIT_FLD_SET(*max_credit_fld, shaper->max_burst, 1,exit);

  /*convert max_rate to delay and cal*/
   res = soc_petra_ingress_scheduler_rate_to_delay_cal_form(
            unit,
            shaper->max_rate,
            &shaper_delay_2_clocks,
            &shaper_cal,
            &exact
         );
   SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  /*write ShaperDelay field*/
  SOC_PETRA_IMPLICIT_FLD_SET(*delay_fld, shaper_delay_2_clocks, 3,exit);

  /*write ShaperCal field*/

  SOC_PETRA_IMPLICIT_FLD_SET(*cal_fld, shaper_cal, 4,exit);

  *exact_max_rate = exact;

  /* } Set Shaper */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_shaper_values_set()",0,0);
}

/*********************************************************************
*     This procedure converts the ingress scheduler shaper max_rate
*     given in kbps to the values of the register fields-
*     1. ShaperDelay: Time interval to add the credit.
*        in two clocks cycles resolution.
*     2. ShaperCal: Credit to add, in bytes resolution.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_rate_to_delay_cal_form(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32             max_rate,
    SOC_SAND_OUT uint32*            shaper_delay_2_clocks,
    SOC_SAND_OUT uint32*            shaper_cal,
    SOC_SAND_OUT uint32*            exact_max_rate
  )
{
  uint32
    res,
    device_ticks_per_sec,
    delay_value,
    cal_value,
    exact_cal_value_long,
    exact_rate_in_kbits_per_sec;
  uint8
    cal_and_delay_in_range = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_RATE_TO_DELAY_CAL_FORM);

  SOC_SAND_CHECK_NULL_INPUT(shaper_delay_2_clocks);
  SOC_SAND_CHECK_NULL_INPUT(shaper_cal);

  SOC_SAND_CHECK_NULL_INPUT(exact_max_rate);

  cal_value = SOC_PETRA_ING_SCH_FIRST_CAL_VAL;

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  if (0 == max_rate)
  {
    *shaper_cal = SOC_PETRA_ING_SCH_MIN_CAL_VAL;
    *shaper_delay_2_clocks = SOC_PETRA_ING_SCH_MAX_DELAY_VAL;
    *exact_max_rate = 0;
    goto exit;
  }

  while ((cal_value > 1) && \
    (cal_value < SOC_PETRA_ING_SCH_MAX_CAL_VAL) && \
    !(cal_and_delay_in_range)
  )
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            max_rate,
            cal_value,
            device_ticks_per_sec,
            &delay_value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    if (delay_value > (SOC_PETRA_ING_SCH_MAX_DELAY_VAL*2))
    {
      cal_value /= 2;
    }
    else if (delay_value < (SOC_PETRA_ING_SCH_MIN_DELAY_VAL*2))
    {
      cal_value *= 2;
      if (cal_value > SOC_PETRA_ING_SCH_MAX_CAL_VAL)
      {
        cal_value = SOC_PETRA_ING_SCH_MAX_CAL_VAL;
      }
    }
    else
    {
      cal_and_delay_in_range = TRUE;
    }
  }

  res = soc_sand_kbits_per_sec_to_clocks(
          max_rate,
          cal_value,
          device_ticks_per_sec,
          &delay_value
         );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

  if(delay_value > (SOC_PETRA_ING_SCH_MAX_DELAY_VAL*2))
  {
    delay_value = (SOC_PETRA_ING_SCH_MAX_DELAY_VAL*2);
  }

  /*
   * the delay value has to be an Even Number
   * (for delay in 2 clocks resolution).
   */
  delay_value = (((delay_value + 1) / 2)*2);

  res = soc_sand_clocks_to_kbits_per_sec(
          delay_value,
          cal_value,
          device_ticks_per_sec,
          &exact_rate_in_kbits_per_sec
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  soc_petra_ingress_scheduler_exact_cal_value(
          cal_value,
          max_rate,
          exact_rate_in_kbits_per_sec,
          &exact_cal_value_long
        );

  if (exact_cal_value_long > SOC_PETRA_ING_SCH_MAX_CAL_VAL)
  {
    exact_cal_value_long = SOC_PETRA_ING_SCH_MAX_CAL_VAL;
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ING_SCH_EXACT_CAL_LARGER_THAN_MAXIMUM_VALUE_ERR, 40, exit);
  }

  res = soc_sand_clocks_to_kbits_per_sec(
          delay_value,
          exact_cal_value_long,
          device_ticks_per_sec,
          &exact_rate_in_kbits_per_sec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  *shaper_delay_2_clocks = delay_value / 2;
  *shaper_cal = exact_cal_value_long;
  *exact_max_rate = exact_rate_in_kbits_per_sec;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_rate_to_delay_cal_form()",0,0);
}

/*********************************************************************
*     This procedure returns a more exact cal value.
*     in order to get an exact cal value
*     calculate:
*        cal_value * max_rate / exact_rate_in_kbits_per_sec;
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_exact_cal_value(
    SOC_SAND_IN uint32  cal_value,
    SOC_SAND_IN uint32  max_rate,
    SOC_SAND_IN uint32  exact_rate_in_kbits_per_sec,
    SOC_SAND_OUT uint32 *exact_cal_value_long
  )
{

  SOC_SAND_U64
    tmp_result,
    exact_cal_value_u64,
    round_helper;

  soc_sand_u64_clear(&tmp_result);

  soc_sand_u64_clear(&exact_cal_value_u64);

  soc_sand_u64_multiply_longs(
    cal_value,
    max_rate,
    &tmp_result
  );

  soc_sand_u64_devide_u64_long(
    &tmp_result,
    exact_rate_in_kbits_per_sec,
    &exact_cal_value_u64
  );

  soc_sand_u64_to_long(
    &exact_cal_value_u64,
    exact_cal_value_long
  );

  soc_sand_u64_multiply_longs(
    *exact_cal_value_long,
    exact_rate_in_kbits_per_sec,
    &round_helper
  );

  if(soc_sand_u64_is_bigger(&tmp_result,&round_helper))
  {
    *exact_cal_value_long = *exact_cal_value_long + 1;
  }

  if(*exact_cal_value_long == 0)
  {
    *exact_cal_value_long = 1;
  }

  return 0;
}

/*********************************************************************
*     This procedure perform a test that compares the rate values to
*     the exact rate values that are received after the
*     conversion function.
*     The procedure tests 2 main criteria:
*     1. That the exact error percentage from the rate does not exceed limit.
*     2. That the exact is always larger than rate.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint8
  soc_petra_ingress_scheduler_conversion_test(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  )
{

  uint32
    ret,
    index,
    rate = SOC_PETRA_ING_SCH_MAX_RATE_MIN,
    tmp_delay,
    tmp_cal,
    exact,
    err_percent,
    max_err_percent=0;

  uint8
    pass = TRUE;

   for (index = SOC_PETRA_ING_SCH_MAX_RATE_MIN; index < SOC_PETRA_ING_SCH_MAX_RATE_MAX; index = (index + 1))
   {
     if(rate > 1000 && is_regression)
     {
       index += SOC_PETRA_CONVERSION_TEST_REGRESSION_STEP;
     }
     rate = index;

     ret = soc_petra_ingress_scheduler_rate_to_delay_cal_form(
       0,
       rate,
       &tmp_delay,
       &tmp_cal,
       &exact
       );

     if(soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
     {
       if (!silent)
       {
         soc_sand_os_printf(
           "soc_petra_ingress_scheduler_conversion_test:"
           "soc_petra_ingress_scheduler_rate_to_delay_cal_form FAIL (100)"
           "\n\r"
         );
       }
       pass = FALSE;
       goto exit;
     }

     if(exact < rate)
     {
       if (!silent)
       {
         soc_sand_os_printf(
           "soc_petra_ingress_scheduler_conversion_test: FAIL (200)"
           "exact rate value is smaller than rate"
           " \n\r"
         );
       }
       pass = FALSE;
       goto exit;
     }

     if((exact - rate) > SOC_PETRA_CONVERSION_TEST_MAX_RATE_DIFF)
     {
       if (!silent)
       {
         soc_sand_os_printf(
           "soc_petra_ingress_scheduler_conversion_test: FAIL (300)"
           "difference between exact_rate and rate is OUT OF LIMIT"
           "\n\r"
         );
       }
       pass = FALSE;
       goto exit;
     }

     err_percent = (((exact - rate)*10000)/rate);
     if (err_percent > 10)
     {
       if (!silent)
       {
         soc_sand_os_printf(
           "soc_petra_ingress_scheduler_conversion_test: FAIL (400)"
           "error percentage is OUT OF LIMIT"
           "\n\r"
          );
       }
        pass = FALSE;
        goto exit;
     }
     if(max_err_percent < err_percent)
     {
       max_err_percent = err_percent;
     }
   }
   if (!silent)
   {
     soc_sand_os_printf(
       "soc_petra_ingress_scheduler_conversion_test:"
       "\n\r"
       "max_err_percent =  %u . %02u \n\r",
       max_err_percent/100,
       max_err_percent%100
     );
   }

exit:
   return pass;
}

/*}*/

#include <soc/dpp/SAND/Utils/sand_footer.h>
