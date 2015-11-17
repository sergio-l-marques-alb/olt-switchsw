#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_multicast_fabric.c,v 1.20 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/port_sw_db.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */



#define ARAD_MULT_NOF_INGRESS_SHAPINGS             (2)
#define ARAD_MULT_TC_MAPPING_FABRIC_MULT_NO_IS     (16)
#define ARAD_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS   (17)

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
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_VERIFY);

  res = arad_traffic_class_verify(tr_cls_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* ARAD_MULT_FABRIC_CLS_MIN may be changed and be grater than 0 */
  /* coverity[unsigned_compare : FALSE] */
  if ((new_mult_cls < ARAD_MULT_FABRIC_CLS_MIN)||
    (new_mult_cls > ARAD_MULT_FABRIC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_verify()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    entry_offset,
    res;
  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;
  int32
    is_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE);

  for (is_ndx = 0; is_ndx < ARAD_MULT_NOF_INGRESS_SHAPINGS; ++is_ndx)
  {
    if (is_ndx == 0)
    {
      entry_offset = ARAD_MULT_TC_MAPPING_FABRIC_MULT_NO_IS;
    }
    else
    {
      entry_offset = ARAD_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS;
    }

    res = arad_irr_traffic_class_mapping_tbl_get_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    tbl_data.traffic_class_mapping[tr_cls_ndx] = new_mult_cls;

    res = arad_irr_traffic_class_mapping_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe()",0,0);
}
/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_TR_CLS           tr_cls_ndx,
    SOC_SAND_OUT ARAD_MULT_FABRIC_CLS  *new_mult_cls
  )
{
  uint32
    entry_offset,
    res;
  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);

  entry_offset = ARAD_MULT_TC_MAPPING_FABRIC_MULT_NO_IS;

  res = arad_irr_traffic_class_mapping_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *new_mult_cls = tbl_data.traffic_class_mapping[tr_cls_ndx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe()",0,0);
}
/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_base_queue_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_VERIFY);

  if (!arad_is_queue_valid(unit, queue_id))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_NUMBER_OF_QUEUE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_verify()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_base_queue_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_FABRIC_MC_BASE_QUEUEr, REG_PORT_ANY, 0, FABRIC_MC_BASE_QUEUEf,  queue_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_base_queue_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_BASE_QUEUE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_id);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRR_FABRIC_MC_BASE_QUEUEr, REG_PORT_ANY, 0, FABRIC_MC_BASE_QUEUEf, queue_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_base_queue_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the Fabric Multicast credit generator configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_credit_source_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_INFO    *info
  )
{
  uint8
    sch_in = FALSE,
    sch_ou = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  sch_in = TRUE;
  sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->best_effort.be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->guaranteed.gr_sch_port.multicast_class_valid);

  sch_ou = TRUE;
  sch_ou = sch_ou && (info->credits_via_sch);

  if (
    ((sch_in) && (!sch_ou)) ||
    ((!sch_in) && (sch_ou))
    )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_CONF_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_verify()",0,0);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
STATIC
  uint32
    arad_mult_fabric_credit_source_port_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_MULTICAST_CLASS           multicast_class_ndx,
      SOC_SAND_IN  ARAD_MULT_FABRIC_PORT_INFO     *info,
      SOC_SAND_OUT ARAD_MULT_FABRIC_PORT_INFO     *exact_info
    )
{
  uint32
    res = SOC_SAND_OK,
    mcast_class_port_id = info->mcast_class_port_id,
    multicast_class_valid = info->multicast_class_valid == TRUE ? 1 : 0,
    buffer = 0,
    base_port_tc;
  ARAD_OFP_RATES_INTERFACE_SHPR_INFO
    shaper;
  ARAD_OFP_RATE_INFO
    ofp;
  ARAD_INTERFACE_ID
    if_id;
  soc_reg_t
      mcast_reg;
  soc_field_t
      mcast_class_port_id_fld[ARAD_NOF_MULTICAST_CLASSES] 
        = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf},
      multicast_class_valid_fld[ARAD_NOF_MULTICAST_CLASSES] 
        = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};
  int core=0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_PORT_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  arad_ARAD_OFP_RATE_INFO_clear(&ofp);
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_clear(&shaper);

  switch (multicast_class_ndx) {
    case ARAD_MULTICAST_CLASS_0:
    case ARAD_MULTICAST_CLASS_1:
      mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
      break;
    case ARAD_MULTICAST_CLASS_2:
    case ARAD_MULTICAST_CLASS_3:
      mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR, 30, exit); 
      break;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, mcast_reg, REG_PORT_ANY,  0, &buffer));

  if(multicast_class_valid) {
      res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, mcast_class_port_id,  &base_port_tc);  
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      ARAD_FLD_TO_REG(mcast_reg, mcast_class_port_id_fld[multicast_class_ndx], base_port_tc, (buffer), 20, exit);
      exact_info->mcast_class_port_id = info->mcast_class_port_id;
  }

  ARAD_FLD_TO_REG(mcast_reg, multicast_class_valid_fld[multicast_class_ndx], multicast_class_valid, (buffer), 30, exit);
  exact_info->multicast_class_valid = info->multicast_class_valid;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_set(unit, mcast_reg, REG_PORT_ANY,  0,  buffer));

  if (multicast_class_valid)
  {
    /*
     *  Disable the OFP rate of the scheduler port to force one source of credits
     *  for this Fabric multicast scheduler port
     */

    /* Verify OFP is connected to scheduler, */
    /* i.e. it is connected to valid interface */
    res = arad_port_to_interface_map_get(
                unit,
                core,
                mcast_class_port_id,
                &if_id,
                NULL
              );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

    if (if_id != ARAD_IF_ID_NONE)
    {
      /* OFP is connected to scheduler */
      res = arad_ofp_rates_sch_single_port_rate_sw_set(unit, core, mcast_class_port_id, 0);
      if (res != SOC_E_NONE) {
          SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR,55,exit);
      }
      res = arad_ofp_rates_sch_single_port_rate_hw_set(unit, core, mcast_class_port_id); 
      if (res != SOC_E_NONE) {
          SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR,65,exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_port_set()",0,0);
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
STATIC
  uint32
    arad_mult_fabric_credit_source_port_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  ARAD_MULTICAST_CLASS         multicast_class_ndx,
      SOC_SAND_OUT ARAD_MULT_FABRIC_PORT_INFO   *info
    )
{
  uint32
    res,
    multicast_class_valid = 0,
      dsp_pp,
    buffer = 0,
    base_port_tc;
  soc_reg_t
      mcast_reg;
  uint32 
      nof_prio;
  soc_pbmp_t 
      ports_bm;
  int
      core;
  soc_port_t
      port;
  soc_field_t
      mcast_class_port_id_fld[ARAD_NOF_MULTICAST_CLASSES] 
        = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf},
      multicast_class_valid_fld[ARAD_NOF_MULTICAST_CLASSES] 
        = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_PORT_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (multicast_class_ndx) {
    case ARAD_MULTICAST_CLASS_0:
    case ARAD_MULTICAST_CLASS_1:
      mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
      break;
    case ARAD_MULTICAST_CLASS_2:
    case ARAD_MULTICAST_CLASS_3:
      mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR, 30, exit); 
      break;
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, mcast_reg, REG_PORT_ANY,  0, &buffer));

  ARAD_FLD_FROM_REG(mcast_reg, mcast_class_port_id_fld[multicast_class_ndx], (base_port_tc), (buffer), 20, exit);

  /* Look for match base q pair */
  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

  SOC_PBMP_ITER(ports_bm, port)
  {
      res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &core);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

      res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, dsp_pp, &nof_prio);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

      if (base_port_tc == nof_prio) {
          info->mcast_class_port_id = dsp_pp;
      }
  } 


  ARAD_FLD_FROM_REG(mcast_reg, multicast_class_valid_fld[multicast_class_ndx], (multicast_class_valid), (buffer), 30, exit);
  info->multicast_class_valid = multicast_class_valid == 1 ? TRUE : FALSE;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_port_get()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_shaper_set(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  soc_reg_t                     reg_desc,
      SOC_SAND_IN  ARAD_MULT_FABRIC_SHAPER_INFO      *info,
      SOC_SAND_OUT ARAD_MULT_FABRIC_SHAPER_INFO      *exact_info
    )
{
  uint32
    res;
  uint32
    rate = 0,
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_SHAPER_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = soc_reg32_get(                                   
          unit,                                              
          reg_desc,                                  
          (REG_PORT_ANY),
          0,                               
          &(buffer)                                            
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          &(info->max_burst),
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SHIFT,
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(buffer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_intern_rate2clock(
          unit,
          info->rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_sand_bitstream_set_any_field(
          &(rate),
          ARAD_MULT_FABRIC_SHAPER_RATE_SHIFT,
          ARAD_MULT_FABRIC_SHAPER_RATE_SIZE,
          &(buffer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(exact_info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_reg32_set(unit, (reg_desc), REG_PORT_ANY, 0, buffer);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  exact_info->max_burst = info->max_burst;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_shaper_set()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_shaper_get(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  soc_reg_t                     reg_desc,
      SOC_SAND_OUT ARAD_MULT_FABRIC_SHAPER_INFO      *info
    )
{
  uint32
    res;
  uint32
    rate = 0,
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_SHAPER_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_reg32_get(                                   					
          unit,                                              
          reg_desc,                                  							
          (REG_PORT_ANY),																										
          0,                                									
          &(buffer)                                            
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &(buffer),
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SHIFT,
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(info->max_burst)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_get_any_field(
          &(buffer),
          ARAD_MULT_FABRIC_SHAPER_RATE_SHIFT,
          ARAD_MULT_FABRIC_SHAPER_RATE_SIZE,
          &(rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_shaper_get()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_be_wfq_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_MULT_FABRIC_BE_INFO       *info,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO       *exact_info
    )
{
  uint32
    res,
    wfq_enable = info->wfq_enable == TRUE ? 0x1 : 0x0,
    be_sch_port[ARAD_NOF_MULTICAST_CLASSES - 1],
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  be_sch_port[ARAD_MULTICAST_CLASS_0] = info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight;
  be_sch_port[ARAD_MULTICAST_CLASS_1] = info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight;
  be_sch_port[ARAD_MULTICAST_CLASS_2] = info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight;

  /*
   * SP vs WFQ info
   * if SP do not write WFQ. Otherwise write.
   */

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IPS_BFMC_CLASS_CONFIGSr(unit, &(buffer)));

  ARAD_FLD_TO_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_WFQ_ENf, wfq_enable, (buffer), 20, exit);

  exact_info->wfq_enable = info->wfq_enable;

  if (wfq_enable)
  {
    ARAD_FLD_TO_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_2_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_0]), (buffer), 50, exit);
    exact_info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight = info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight;

    ARAD_FLD_TO_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_3_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_1]), (buffer), 40, exit);
    exact_info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight = info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight;

    ARAD_FLD_TO_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_4_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_2]), (buffer), 30, exit);
    exact_info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight = info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_IPS_BFMC_CLASS_CONFIGSr(unit,  buffer));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_be_wfq_set()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_be_wfq_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO       *info
    )
{
  uint32
    res,
    wfq_enable = 0,
    be_sch_port[ARAD_NOF_MULTICAST_CLASSES - 1],
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  be_sch_port[ARAD_MULTICAST_CLASS_0] = 0;
  be_sch_port[ARAD_MULTICAST_CLASS_1] = 0;
  be_sch_port[ARAD_MULTICAST_CLASS_2] = 0;

  /*
   * SP vs WFQ info
   * if SP do not write WFQ. Otherwise write.
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_IPS_BFMC_CLASS_CONFIGSr(unit, &(buffer)));

  ARAD_FLD_FROM_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_WFQ_ENf, (wfq_enable), (buffer), 30, exit);

  info->wfq_enable = (wfq_enable==0x1 ? TRUE : FALSE);

  if (info->wfq_enable)
  {
    ARAD_FLD_FROM_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_2_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_0]), (buffer), 40, exit);

    ARAD_FLD_FROM_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_3_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_1]), (buffer), 50, exit);

    ARAD_FLD_FROM_REG(IPS_BFMC_CLASS_CONFIGSr, BFMC_CLASS_4_Wf, (be_sch_port[ARAD_MULTICAST_CLASS_2]), (buffer), 60, exit);
  }

  info->be_sch_port[ARAD_MULTICAST_CLASS_0].weight = be_sch_port[ARAD_MULTICAST_CLASS_0];
  info->be_sch_port[ARAD_MULTICAST_CLASS_1].weight = be_sch_port[ARAD_MULTICAST_CLASS_1];
  info->be_sch_port[ARAD_MULTICAST_CLASS_2].weight = be_sch_port[ARAD_MULTICAST_CLASS_2];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_be_wfq_get()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_be_set(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  ARAD_MULT_FABRIC_BE_INFO    *info,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO    *exact_info
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /*
   * SP and WFQ info
   */
  res = arad_mult_fabric_credit_source_be_wfq_set(
          unit,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set shaper info
   */
  res = arad_mult_fabric_credit_source_shaper_set(
          unit,
          IPS_BFMC_SHAPER_CONFIGSr,
          &(info->be_shaper),
          &(exact_info->be_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Set port info
   */
  res = arad_mult_fabric_credit_source_port_set(
          unit,
          ARAD_MULTICAST_CLASS_3,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port),
          &(exact_info->be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_mult_fabric_credit_source_port_set(
          unit,
          ARAD_MULTICAST_CLASS_2,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port),
          &(exact_info->be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_mult_fabric_credit_source_port_set(
          unit,
          ARAD_MULTICAST_CLASS_1,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port),
          &(exact_info->be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_be_set()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_be_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO *info
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Get SP and WFQ info
   */
  res = arad_mult_fabric_credit_source_be_wfq_get(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Get shaper info
   */
  res = arad_mult_fabric_credit_source_shaper_get(
          unit,
          IPS_BFMC_SHAPER_CONFIGSr,
          &(info->be_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Get port info- inversion in the scheduler for the order
   * of importance of the multicast classes MC3 < MC2 < MC1 (best effort)
   */
  res = arad_mult_fabric_credit_source_port_get(
          unit,
          ARAD_MULTICAST_CLASS_3,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_0].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_mult_fabric_credit_source_port_get(
          unit,
          ARAD_MULTICAST_CLASS_2,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_1].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_mult_fabric_credit_source_port_get(
          unit,
          ARAD_MULTICAST_CLASS_1,
          &(info->be_sch_port[ARAD_MULTICAST_CLASS_2].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_be_get()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_gu_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_MULT_FABRIC_GR_INFO *info,
      SOC_SAND_OUT ARAD_MULT_FABRIC_GR_INFO *exact_info
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_GU_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /*
   * Set shaper info
   */
  res = arad_mult_fabric_credit_source_shaper_set(
          unit,
          IPS_GFMC_SHAPER_CONFIGSr,
          &(info->gr_shaper),
          &(exact_info->gr_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set port info - inversion in the scheduler for the order of importance of the
   * multicast classes: the guaranteed multicast class is 0
   */
  res = arad_mult_fabric_credit_source_port_set(
          unit,
          ARAD_MULTICAST_CLASS_0,
          &(info->gr_sch_port),
          &(exact_info->gr_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_gu_set()",0,0);
}

STATIC
  uint32
    arad_mult_fabric_credit_source_gu_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT ARAD_MULT_FABRIC_GR_INFO *info
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_GU_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Set shaper info
   */
  res = arad_mult_fabric_credit_source_shaper_get(
          unit,
          IPS_GFMC_SHAPER_CONFIGSr,
          &(info->gr_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set port info- inversion in the scheduler for the order of importance of the
   * multicast classes: the guaranteed multicast class is 0
   */
  res = arad_mult_fabric_credit_source_port_get(
          unit,
          ARAD_MULTICAST_CLASS_0,
          &(info->gr_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_gu_get()",0,0);
}



/*********************************************************************
*     Set the Fabric Multicast credit generator configuration
*     for the Default Fabric Multicast Queue configuration.
*     The fabric multicast queues are 0 - 3, and the credits
*     comes either directly to these queues or according to a
*     scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_credit_source_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_INFO    *info,
    SOC_SAND_OUT ARAD_MULT_FABRIC_INFO    *exact_info
  )
{
  uint32
    res;
  uint32
    buffer,
    rate = 0;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  if (SOC_IS_JERICHO(unit)) { SOC_EXIT; } 

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, 0, FMC_CREDITS_FROM_SCHf,  info->credits_via_sch));
    exact_info->credits_via_sch = info->credits_via_sch;

  res = arad_intern_rate2clock(
          unit,
          info->max_rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_FMC_SCHEDULER_CONFIGSr, REG_PORT_ANY, 0, FMC_MAX_CR_RATEf,  rate));

  buffer = 0;
  res = soc_sand_bitstream_set_any_field(
          &(info->max_burst),
          0,
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(buffer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  75,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_FMC_SCHEDULER_CONFIGSr, REG_PORT_ANY, 0, FMC_MAX_BURSTf,  buffer));

  res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
         &(exact_info->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

  res = arad_mult_fabric_credit_source_be_set(
          unit,
          &(info->best_effort),
          &(exact_info->best_effort)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_mult_fabric_credit_source_gu_set(
          unit,
          &(info->guaranteed),
          &(exact_info->guaranteed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_set_unsafe()",0,0);
}

/*********************************************************************
*     Set the Fabric Multicast credit generator configuration
*     for the Default Fabric Multicast Queue configuration.
*     The fabric multicast queues are 0 - 3, and the credits
*     comes either directly to these queues or according to a
*     scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_credit_source_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_INFO    *info
  )
{
  uint32
    res;
  uint32
    buffer,
    credits_via_sch = 0,
    rate = 0;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_CREDIT_SOURCE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_mult_fabric_credit_source_gu_get(
          unit,
          &(info->guaranteed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_mult_fabric_credit_source_be_get(
          unit,
          &(info->best_effort)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_FMC_SCHEDULER_CONFIGSr, REG_PORT_ANY, 0, FMC_MAX_CR_RATEf, &(rate)));

  res = arad_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(info->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  buffer = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_FMC_SCHEDULER_CONFIGSr, REG_PORT_ANY, 0, FMC_MAX_BURSTf, &(buffer)));
  res = soc_sand_bitstream_get_any_field(
          &(buffer),
          0,
          ARAD_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(info->max_burst)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, SOC_CORE_ALL, 0, FMC_CREDITS_FROM_SCHf, &credits_via_sch));
  info->credits_via_sch = credits_via_sch  == 1 ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_credit_source_get_unsafe()",0,0);
}
/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_enhanced_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    res,
    reg_val = 0;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_ENHANCED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBAL_17r(unit, &reg_val));
  ARAD_FLD_TO_REG(ECI_GLOBAL_17r, FMC_QNUM_LOWf, queue_range->start, reg_val, 20, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_GLOBAL_17r(unit,  reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBAL_18r(unit, &reg_val));
  ARAD_FLD_TO_REG(ECI_GLOBAL_18r, FMC_QNUM_HIGHf, queue_range->end, reg_val, 50, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_GLOBAL_18r(unit,  reg_val));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_enhanced_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_enhanced_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_ENHANCED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_range->start, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
   );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_range->end, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 20, exit
   );

  if (queue_range->start > queue_range->end)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_MULTI_FABRIC_QUEUE_ORDER_ERR, 30, exit);
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_enhanced_set_verify()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_mult_fabric_enhanced_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    res,
    reg_val = 0;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MULT_FABRIC_ENHANCED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBAL_17r(unit, &reg_val));
  ARAD_FLD_FROM_REG(ECI_GLOBAL_17r, FMC_QNUM_LOWf, queue_range->start, reg_val, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBAL_18r(unit, &reg_val));
  ARAD_FLD_FROM_REG(ECI_GLOBAL_18r, FMC_QNUM_HIGHf, queue_range->end, reg_val, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_enhanced_get_unsafe()",0,0);
}

uint32
  arad_mult_fabric_flow_control_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(fc_map);
  SOC_SAND_MAGIC_NUM_VERIFY(fc_map);

  if(fc_map->gfmc_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->gfmc_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 1, exit);
  }

  if(fc_map->bfmc0_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc0_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 2, exit);
  }

  if(fc_map->bfmc1_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc1_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 3, exit);
  }

  if(fc_map->bfmc2_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      SOC_SAND_ERR_IF_ABOVE_MAX(fc_map->bfmc2_lb_fc_map, 0xf, ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 4, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_verify()", 0, 0);
}

  
uint32
  arad_mult_fabric_flow_control_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{
  uint32 fld_value, res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(fc_map->gfmc_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->gfmc_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc0_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc0_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc1_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc1_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf,  fld_value));
  }

  if(fc_map->bfmc2_lb_fc_map != ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP) {
      fld_value = fc_map->bfmc2_lb_fc_map & 0xf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf,  fld_value));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_unsafe()",0,0);
}

uint32
  arad_mult_fabric_flow_control_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(fc_map);
  SOC_SAND_MAGIC_NUM_VERIFY(fc_map);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_set_verify()", 0, 0);
}

uint32
  arad_mult_fabric_flow_control_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP     *fc_map
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf, &fc_map->gfmc_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf, &fc_map->bfmc0_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf, &fc_map->bfmc1_lb_fc_map));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf, &fc_map->bfmc2_lb_fc_map));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mult_fabric_flow_control_get_unsafe()",0,0);
}

/*
* Function:
*      arad_multicast_table_size_get
* Purpose:
*      Get MC table size
* Parameters:
*      unit           - (IN)  Unit number.
* Returns:
*      SOC_E_xxx
*/
soc_error_t 
arad_multicast_table_size_get(int unit, uint32* mc_table_size)
{
    SOCDNX_INIT_FUNC_DEFS;

    *mc_table_size = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;

    SOCDNX_FUNC_RETURN;    
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */
