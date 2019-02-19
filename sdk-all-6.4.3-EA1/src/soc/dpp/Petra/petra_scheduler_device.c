/* $Id: petra_scheduler_device.c,v 1.7 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_scheduler_device.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_scheduler_device.c,v 1.7 Broadcom SDK $
 *  Internal Rate resolution - Kilo-bits per second.
 */
#define SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION 64

/*
 *  Scheduler index definitions.
 *  Indexes 0-3, 16-19 represent MAL 0-3, 4-7 accordingly.
 *  This corresponds to NIF indexes 0, 4, 8,... 28.
 *  Those indexes are also referred as "channelized indexes".
 *  This does not necessarily mean that the interface is channelized
 *  (for example it can be XAUI non-channelized interface),
 *  but a channelized (SPAUI) interface can be one of these indexes.
 *  These indexes are refered to in scheduler registers as 'ChNif-registers'
 *
 *  Indexes 4-15, 20-31 represent the rest of network indexes
 *  in range 0..31. These can be SGMII interface indexes (non-channelized).
 *  These indexes are refered to in scheduler registers as 'OnePortNif-registers'
 */


#define SOC_PETRA_SCH_1PORTNIF_A_MIN  4
#define SOC_PETRA_SCH_1PORTNIF_A_MAX  15
#define SOC_PETRA_SCH_1PORTNIF_B_MIN  20
#define SOC_PETRA_SCH_1PORTNIF_B_MAX  31

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_scheduler_device.h>
#endif

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PETRA_SCH_IS_1PORTNIF_ID(id)                                                \
  SOC_SAND_NUM2BOOL(                                                                    \
    (SOC_SAND_IS_VAL_IN_RANGE(id, SOC_PETRA_SCH_1PORTNIF_A_MIN, SOC_PETRA_SCH_1PORTNIF_A_MAX)) || \
    (SOC_SAND_IS_VAL_IN_RANGE(id, SOC_PETRA_SCH_1PORTNIF_B_MIN, SOC_PETRA_SCH_1PORTNIF_B_MAX))    \
  )

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
*     Converts from interface index as represented by
*     SOC_PETRA_INTERFACE_ID to the offset for the relevant
*     scheduler register (channelized or 1-port.
*********************************************************************/
uint32
  soc_petra_sch_if2sched_offset(
    uint32 nif_id
  )
{
  uint32
    sched_nif_id = 0;

  if (soc_petra_is_channelized_interface_id(nif_id))
  {
    /* PA: 0-7, corresponds to order in sch registers (NIF[0-3],[16-19])*/
    /* PB: 0-15, corresponds to order in sch registers (NIF[0-3],[16-19],[32-35],[48-51])*/
    sched_nif_id = nif_id / SOC_PETRA_MAX_NIFS_PER_MAL;
  }
  else
  {
    /* PA: 0-23, corresponds to order in sch registers (NIF[4-15], [20-31])*/
    /* PB: 0-47, corresponds to order in sch registers (NIF[4-15], [20-31],[36-47], [52-63])*/
    sched_nif_id = nif_id - 1 - (nif_id  / SOC_PETRA_MAX_NIFS_PER_MAL);
  }

  return sched_nif_id;
}


uint32
  soc_petra_sch_if_shaper_rate_verify(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID  if_ndx,
    SOC_SAND_IN     uint32            rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_SHAPER_RATE_VERIFY);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rate, SOC_PETRA_IF_MAX_RATE_KBPS,
    SOC_PETRA_OFP_RATES_SCH_RATE_OUT_OF_RANGE_ERR , 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_if_shaper_rate_verify()",0,0);
}

/*********************************************************************
*     Gets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_if_shaper_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID  if_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
  uint32
    res,
    rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *max_cr_rate_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;
  uint8
    fast_traversal = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_SHAPER_RATE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(rate);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  mal_idx = SOC_PETRA_NIF2MAL_NDX(if_ndx);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      sch_nif_offset = soc_petra_sch_if2sched_offset(if_ndx);
      if (soc_petra_is_channelized_interface_id(if_ndx))
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_idx].ch_nifxx_max_cr_rate);
      }
      else
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_max_cr_rate);
        fast_traversal = TRUE;
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rate1_reg.cpumax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate1_reg.rcymax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  case SOC_PETRA_IF_TYPE_OLP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*max_cr_rate_fld, credit_rate, 3, exit);

  if (0 == credit_rate)
  {
    rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

    if (fast_traversal)
    {
      rate_internal = rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
    }
  }

  *rate = rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_if_shaper_rate_get_unsafe()",0,0);
}

uint32
  soc_petra_sch_if_shaper_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID  if_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_SHAPER_RATE_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_if_shaper_rate_get_unsafe,(unit, if_ndx, rate));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_if_shaper_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interfaces - see REMARKS
*     section below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_if_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID      if_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res,
    credit_rate,
    device_ticks_per_sec,
    rate_internal,
    exact_rate_internal,
    credit_worth;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *max_cr_rate_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;
  uint8
    fast_traversal = FALSE;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_SHAPER_RATE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  mal_idx = SOC_PETRA_NIF2MAL_NDX(if_ndx);
  rate_internal = rate;

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      if (soc_petra_is_channelized_interface_id(if_ndx))
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_idx].ch_nifxx_max_cr_rate);
      }
      else
      {
        sch_nif_offset = soc_petra_sch_if2sched_offset(if_ndx);
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_max_cr_rate);
        fast_traversal = TRUE;
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rate1_reg.cpumax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate1_reg.rcymax_cr_rate);
      break;
   case SOC_PETRA_IF_TYPE_ERP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  case SOC_PETRA_IF_TYPE_OLP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  if (fast_traversal)
  {
    rate_internal = SOC_SAND_DIV_ROUND_UP(rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);


  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    credit_rate = 0;
  }
  else
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            rate_internal,
            credit_worth,
            device_ticks_per_sec,
            &credit_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_PETRA_FLD_MAX(*max_cr_rate_fld));

    if (fast_traversal)
    {
      SOC_SAND_LIMIT_FROM_BELOW(credit_rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
    }
  }

  SOC_PETRA_IMPLICIT_FLD_SET(*max_cr_rate_fld, credit_rate, 4, exit);

  if (0 == credit_rate)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (fast_traversal)
    {
      exact_rate_internal = exact_rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
    }
  }
  *exact_rate = exact_rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_if_shaper_rate_set_unsafe()",if_ndx,0);
}

uint32
  soc_petra_sch_if_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID      if_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_SHAPER_RATE_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_if_shaper_rate_set_unsafe,(unit, if_ndx, rate, exact_rate));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_if_shaper_rate_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  )
{
  uint32
    weight_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_WEIGHT_CONF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(if_weights);

  SOC_SAND_MAGIC_NUM_VERIFY(if_weights);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    if_weights->nof_enties, SOC_PETRA_SCH_NOF_IF_WEIGHTS,
    SOC_PETRA_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  for (weight_idx = 0; weight_idx < SOC_PETRA_SCH_NOF_IF_WEIGHTS; weight_idx++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      if_weights->weight[weight_idx].id, SOC_PETRA_SCH_NOF_IF_WEIGHTS,
      SOC_PETRA_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR, 20, exit
    );

    /* SOC_PETRA_SCH_DEVICE_IF_WEIGHT_MIN may be changed and be grater than 0*/
    /* coverity[unsigned_compare : FALSE] */
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      if_weights->weight[weight_idx].val, SOC_PETRA_SCH_DEVICE_IF_WEIGHT_MIN, SOC_PETRA_SCH_DEVICE_IF_WEIGHT_MAX,
      SOC_PETRA_SCH_DEVICE_IF_WEIGHT_OUT_OF_RANGE_ERR, 30, exit
    );
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_if_weight_conf_verify()",0,0);
}

/*********************************************************************
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  )
{
  uint32
    res,
    weight_reg_idx;
  uint32
    entry_idx,
    weight_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_WEIGHT_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(if_weights);
  regs = soc_petra_regs();

  for (entry_idx = 0; entry_idx < if_weights->nof_enties; entry_idx++)
  {
    weight_idx = if_weights->weight[entry_idx].id;

    /* Weight value write { */
    weight_reg_idx = weight_idx/2;

    if ((weight_idx%2) == 0)
    {
      SOC_PETRA_FLD_SET(regs->sch.dvs_weight_config_reg[weight_reg_idx].wfqweight_xx, if_weights->weight[entry_idx].val, 30, exit);
    }
    else
    {
      SOC_PETRA_FLD_SET(regs->sch.dvs_weight_config_reg[weight_reg_idx].wfqweight_xx_plus_1, if_weights->weight[entry_idx].val, 40, exit);
    }
    /* Weight value write } */

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_if_weight_conf_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets the device interfaces scheduler
*     weight configuration. Up to 8 weight configuration can
*     be pre-configured. Each scheduler interface will be
*     configured to use one of these pre-configured weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_if_weight_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_IF_WEIGHTS      *if_weights
  )
{
  uint32
    res,
    weight_reg_idx;
  uint32
    weight_idx,
    entry_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_IF_WEIGHT_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(if_weights);
  regs = soc_petra_regs();

  if_weights->nof_enties = SOC_PETRA_SCH_NOF_IF_WEIGHTS;

  for (entry_idx = 0; entry_idx < if_weights->nof_enties; entry_idx++)
  {
    weight_idx = entry_idx;
    
    SOC_SAND_ERR_IF_ABOVE_MAX(
      weight_idx, SOC_PETRA_SCH_NOF_IF_WEIGHTS,
      SOC_PETRA_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR, 20, exit
    );

    if_weights->weight[entry_idx].id = entry_idx;
    /* Weight value write { */
    weight_reg_idx = weight_idx/2;

    if ((weight_idx%2) == 0)
    {
      SOC_PETRA_FLD_GET(regs->sch.dvs_weight_config_reg[weight_reg_idx].wfqweight_xx, (if_weights->weight[weight_idx].val), 30, exit);
    }
    else
    {
      SOC_PETRA_FLD_GET(regs->sch.dvs_weight_config_reg[weight_reg_idx].wfqweight_xx_plus_1, (if_weights->weight[weight_idx].val), 40, exit);
    }
    /* Weight value write } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_if_weight_conf_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_device_if_weight_idx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_IF_WEIGHT_IDX_VERIFY);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PETRA_SCH_IS_1PORTNIF_ID(if_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INTERFACE_IS_SINGLE_PORT_ERR, 20, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
      weight_index, SOC_PETRA_SCH_NOF_IF_WEIGHTS,
      SOC_PETRA_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR, 30, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_device_if_weight_idx_verify()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  )
{
  uint32
    weight_idx_result,
    res;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *weight_idx_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(weight_index);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      mal_idx = soc_petra_sch_if2sched_offset(if_ndx);
      if (soc_petra_is_channelized_interface_id(if_ndx))
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_idx].ch_nifxx_weight);
      }
      else
      {
        sch_nif_offset = soc_petra_sch_if2sched_offset(if_ndx);
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_weight);
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cpuweight);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.rcyweight);
      break;
  case SOC_PETRA_IF_TYPE_OLP:
    weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpweight);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpweight);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /* Weight index read{ */
  SOC_PETRA_IMPLICIT_FLD_GET(*weight_idx_fld, (weight_idx_result), 20, exit);
  /* Weight index read} */

  *weight_index = weight_idx_result;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_sch_device_if_weight_idx_get_unsafe()",0,0);
}

uint32
  soc_petra_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_device_if_weight_idx_get_unsafe,(unit, if_ndx, weight_index));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_device_if_weight_idx_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  )
{
  uint32
    res;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *weight_idx_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      sch_nif_offset = soc_petra_sch_if2sched_offset(if_ndx);
      mal_idx = SOC_PETRA_NIF2MAL_NDX(if_ndx);
      if (soc_petra_is_channelized_interface_id(if_ndx))
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_idx].ch_nifxx_weight);
      }
      else
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_weight);
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cpuweight);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.rcyweight);
      break;
  case SOC_PETRA_IF_TYPE_OLP:
    weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpweight);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpweight);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /* Weight index write{ */
  SOC_PETRA_IMPLICIT_FLD_SET(*weight_idx_fld, weight_index, 20, exit);
  /* Weight index write} */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_device_if_weight_set_unsafe()",0,0);
}

uint32
  soc_petra_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_device_if_weight_idx_set_unsafe,(unit, if_ndx, weight_index));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_mal_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate,
    SOC_SAND_OUT uint32                  *exact_rate
  )
{
  uint32
    interval_in_clock_64_th,
    calc,
    offset,
  credit_worth,
    res;
  SOC_PETRA_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  /*
   *  Calculate interval between credits (in Clocks/64) according to:
   *
   *
   *                       Credit [bits] * Num_of_Mega_clocks_64th_in_sec [(M * clocks)/(64 * sec)]
   *  Rate [Mbits/Sec] =   -----------------------------------------------------------------------
   *                          interval_between_credits_in_clock_64th [clocks/64]
   */

  if (0 == rate)
  {
    interval_in_clock_64_th = 0;
  }
  else
  {
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (soc_petra_chip_mega_ticks_per_sec_get(unit) * SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, rate);
    interval_in_clock_64_th = calc;
  }

  SOC_SAND_LIMIT_FROM_ABOVE(interval_in_clock_64_th, SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_MAX);

  if (interval_in_clock_64_th != 0)
  {
    SOC_SAND_LIMIT_FROM_BELOW(interval_in_clock_64_th, SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_MIN);
  }

  offset = (nof_active_links_ndx * SOC_PETRA_SCH_NOF_RCI_LEVELS) + rci_level_ndx;
  drm_tbl_data.device_rate = interval_in_clock_64_th;

  /*
   * Write indirect to DRM table, single entry
   */
  res = soc_petra_sch_drm_tbl_set_unsafe(
          unit,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Fill exact_dev_rate with the actual written params
   */

  if (0 == interval_in_clock_64_th)
  {
    *exact_rate = 0;
  }
  else
  {
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (soc_petra_chip_mega_ticks_per_sec_get(unit) * SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, interval_in_clock_64_th);
    *exact_rate = calc;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_device_rate_entry_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_IN  uint32                  rate
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_RATE_ENTRY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nof_active_links_ndx, SOC_PETRA_SCH_NOF_LINKS,
    SOC_PETRA_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,10,exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rci_level_ndx, SOC_PETRA_SCH_NOF_RCI_LEVELS-1,
    SOC_PETRA_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,20,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_device_rate_entry_verify()",0,0);
}

/*********************************************************************
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables: Device Rate Memory (DRM)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_device_rate_entry_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rci_level_ndx,
    SOC_SAND_IN  uint32                 nof_active_links_ndx,
    SOC_SAND_OUT uint32                  *rate
  )
{
  uint32
    interval_in_clock_64_th,
    calc,
    offset,
  credit_worth,
    res;
  SOC_PETRA_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rate);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nof_active_links_ndx, SOC_PETRA_SCH_NOF_LINKS,
    SOC_PETRA_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,10,exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rci_level_ndx, SOC_PETRA_SCH_NOF_RCI_LEVELS-1,
    SOC_PETRA_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,20,exit
  );

  /*
   * Read indirect from DRM table
   */

  offset = (nof_active_links_ndx * SOC_PETRA_SCH_NOF_RCI_LEVELS) + rci_level_ndx;

  res = soc_petra_sch_drm_tbl_get_unsafe(
          unit,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  interval_in_clock_64_th = drm_tbl_data.device_rate;
  /*
   *  Calculate Device Credit Rate Generation (in Mbits/secs) according to:
   *
   *
   *                       Credit [bits] * Num_of_Mega_clocks_64th_in_sec [(M * clocks)/(64 * sec)]
   *  Rate [Mbits/Sec] =   -----------------------------------------------------------------------
   *                          interval_between_credits_in_clock_64th [clocks/64]
   */
  if (0 == interval_in_clock_64_th)
  {
    *rate = 0;
  }
  else
  {
    res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (soc_petra_chip_mega_ticks_per_sec_get(unit) * SOC_PETRA_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, interval_in_clock_64_th);
    *rate = calc;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_device_rate_entry_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device MAC LANE or equivalent, (NIF-Ports,
*     recycling & CPU) its actual credit rate (sum of ports). This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
*     Note: for SGMII - configures only the first interface in MAL
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_mal_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     uint32            mal_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
   uint32
    res,
    rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *sum_of_ports_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_SCH_MAL_RATE_GET_UNSAFE);

  regs = soc_petra_regs();

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_NIF:
    sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_ndx].ch_nifxx_sum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_CPU:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rates0_reg.cpusum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_RCY:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate0_reg.rcysum_of_ports);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*sum_of_ports_fld, credit_rate, 3, exit);

  if (0 == credit_rate)
  {
    rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);
  }

  *rate = rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_sch_mal_rate_get_unsafe()",0,0);
}

uint32
  soc_petra_sch_mal_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     uint32            mal_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_MAL_RATE_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_mal_rate_get_unsafe,(unit, mal_ndx, rate));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_mal_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device MAC LANE or equivalent, (NIF-Ports,
*     recycling & CPU) its actual credit rate (sum of ports). This API is
*     only valid for Channelized interfaces - see REMARKS
*     section below.
*     Note: for SGMII - configures only the first interface in MAL
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sch_mal_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     uint32                mal_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res,
    rate_internal,
    exact_rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *sum_of_ports_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_SCH_MAL_RATE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  regs = soc_petra_regs();

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  rate_internal = SOC_SAND_DIV_ROUND_UP(rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_NIF:
    sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_ndx].ch_nifxx_sum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_CPU:
    /*
     * Although OLP is non channelized interface it is treated as such
     * because it shares the same calendar with the CPU.
     */
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rates0_reg.cpusum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_RCY:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate0_reg.rcysum_of_ports);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    credit_rate = 0;
  }
  else
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            rate_internal,
            credit_worth,
            device_ticks_per_sec,
            &credit_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_PETRA_FLD_MAX(*sum_of_ports_fld));
    SOC_SAND_LIMIT_FROM_BELOW(credit_rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
  }

  SOC_PETRA_IMPLICIT_FLD_SET(*sum_of_ports_fld, credit_rate, 4, exit);

  if (0 == credit_rate)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  *exact_rate = exact_rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_mal_rate_set_unsafe()",0,0);
}
    
uint32
  soc_petra_sch_mal_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     uint32                mal_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_MAL_RATE_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sch_mal_rate_set_unsafe,(unit, mal_ndx, rate, exact_rate));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_mal_rate_set_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
