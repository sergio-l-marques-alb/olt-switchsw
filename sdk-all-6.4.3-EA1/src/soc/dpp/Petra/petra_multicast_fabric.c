/* $Id: petra_multicast_fabric.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_multicast_fabric.c
*
* MODULE PREFIX:  soc_petra_mult_fabric
*
* FILE DESCRIPTION: refer to H file.
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

#include <soc/dpp/Petra/petra_multicast_fabric.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_ports.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>


#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_multicast_fabric.h>
#endif
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
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_VERIFY);

  res = soc_petra_traffic_class_verify(tr_cls_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* SOC_PETRA_MULT_FABRIC_CLS_MIN may be changed and be grater than 0 */
  /* coverity[unsigned_compare : FALSE] */
  if ((new_mult_cls < SOC_PETRA_MULT_FABRIC_CLS_MIN)||
    (new_mult_cls > SOC_PETRA_MULT_FABRIC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_verify()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PA_FLD_SET(regs->irr.egress_or_fabric_mc_tc_class_mapping_reg.egr_or_fabric_mc_tc_mapping[tr_cls_ndx], new_mult_cls, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe()",0,0);
}

uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe,(unit, tr_cls_ndx, new_mult_cls));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
      SOC_SAND_IN  int              unit,
      SOC_SAND_IN  SOC_PETRA_TR_CLS           tr_cls_ndx,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS  *new_mult_cls
    )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->irr.egress_or_fabric_mc_tc_class_mapping_reg.egr_or_fabric_mc_tc_mapping[tr_cls_ndx], fld_val, 10, exit);
  *new_mult_cls = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe()",0,0);
}

uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS           tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS  *new_mult_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);

  SOC_PETRA_DIFF_DEVICE_CALL(mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe,(unit, tr_cls_ndx, new_mult_cls));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_BASE_QUEUE_VERIFY);

  if (!soc_petra_is_queue_valid(unit, queue_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_FABRIC_ILLEGAL_NUMBER_OF_QUEUE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_base_queue_verify()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_BASE_QUEUE_SET_UNSAFE);

  SOC_PETRA_FLD_SET(regs->irr.egress_or_fabric_multicast_base_queue_reg.egress_or_fabric_multicast_base_queue_register, queue_id, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_base_queue_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_BASE_QUEUE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_id);

  SOC_PETRA_FLD_GET(regs->irr.egress_or_fabric_multicast_base_queue_reg.egress_or_fabric_multicast_base_queue_register, *queue_id, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_base_queue_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the Fabric Multicast credit generator configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info
  )
{
  uint8
    sch_in = FALSE,
    sch_ou = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  sch_in = TRUE;
  sch_in = sch_in && (info->best_effort.be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->best_effort.be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->best_effort.be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].be_sch_port.multicast_class_valid);
  sch_in = sch_in && (info->guaranteed.gr_sch_port.multicast_class_valid);

  sch_ou = TRUE;
  sch_ou = sch_ou && (info->credits_via_sch);

  if (
    ((sch_in) && (!sch_ou)) ||
    ((!sch_in) && (sch_ou))
    )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_FABRIC_ILLEGAL_CONF_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_verify()",0,0);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
STATIC
  uint32
    soc_petra_mult_fabric_credit_source_port_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PETRA_MULTICAST_CLASS           multicast_class_ndx,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_PORT_INFO     *info,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_PORT_INFO     *exact_info
    )
{
  uint32
    res,
    mcast_class_port_id = info->mcast_class_port_id,
    multicast_class_valid = info->multicast_class_valid == TRUE ? 1 : 0,
    buffer = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper;
  SOC_PETRA_OFP_RATE_INFO
    ofp;
  uint32
    mal_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_PORT_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  soc_petra_PETRA_OFP_RATE_INFO_clear(&ofp);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper);

  SOC_PETRA_REG_GET(regs->sch.sch_fabric_multicast_port_configuration_reg, buffer, 10, exit);

  SOC_PETRA_FLD_TO_REG(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[multicast_class_ndx].port_id, mcast_class_port_id, (buffer), 20, exit);
  exact_info->mcast_class_port_id = info->mcast_class_port_id;

  SOC_PETRA_FLD_TO_REG(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[multicast_class_ndx].enable, multicast_class_valid, (buffer), 30, exit);
  exact_info->multicast_class_valid = info->multicast_class_valid;

  SOC_PETRA_REG_SET(regs->sch.sch_fabric_multicast_port_configuration_reg, buffer, 40, exit);

  if (multicast_class_valid)
  {
    /*
     *  Disable the OFP rate of the scheduler port to force one source of credits
     *  for this Fabric multicast scheduler port
     */

    /* Verify OFP is connected to scheduler, */
    /* i.e. it is connected to valid mal     */
    res = soc_petra_port_ofp_mal_get_unsafe(
                unit,
                mcast_class_port_id,
                &mal_i
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    if (mal_i != SOC_PETRA_IF_ID_NONE)
    {
      /* OFP is connected to scheduler */
      res = soc_petra_ofp_rates_single_port_get_unsafe(
            unit,
            mcast_class_port_id,
            &shaper,
            &ofp
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      ofp.sch_rate = 0;

      res = soc_petra_ofp_rates_single_port_set_unsafe(
            unit,
            mcast_class_port_id,
            &shaper,
            &ofp
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_port_set()",0,0);
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
STATIC
  uint32
    soc_petra_mult_fabric_credit_source_port_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PETRA_MULTICAST_CLASS         multicast_class_ndx,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_PORT_INFO   *info
    )
{
  uint32
    res;
  uint32
    multicast_class_valid = 0,
    mcast_class_port_id = 0,
    buffer = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_PORT_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_REG_GET(regs->sch.sch_fabric_multicast_port_configuration_reg, buffer, 10, exit);

  SOC_PETRA_FLD_FROM_REG(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[multicast_class_ndx].port_id, (mcast_class_port_id), (buffer), 20, exit);
  info->mcast_class_port_id = mcast_class_port_id;

  SOC_PETRA_FLD_FROM_REG(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[multicast_class_ndx].enable, (multicast_class_valid), (buffer), 30, exit);
  info->multicast_class_valid = multicast_class_valid == 1 ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_port_get()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_shaper_set(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PETRA_REG_ADDR                     *dst_reg,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_SHAPER_INFO      *info,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_SHAPER_INFO      *exact_info
    )
{
  uint32
    res;
  uint32
    rate = 0,
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SHAPER_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = soc_petra_read_reg_unsafe(
          unit,
          dst_reg,
          SOC_PETRA_DEFAULT_INSTANCE,
          &buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          &(info->max_burst),
          SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SHIFT,
          SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(buffer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_intern_rate2clock(
          unit,
          info->rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_sand_bitstream_set_any_field(
          &(rate),
          SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SHIFT,
          SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SIZE,
          &(buffer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(exact_info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_write_reg_unsafe(
          unit,
          dst_reg,
          SOC_PETRA_DEFAULT_INSTANCE,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exact_info->max_burst = info->max_burst;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_shaper_set()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_shaper_get(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  SOC_PETRA_REG_ADDR                     *dst_reg,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_SHAPER_INFO      *info
    )
{
  uint32
    res;
  uint32
    rate = 0,
    buffer = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SHAPER_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_read_reg_unsafe(
          unit,
          dst_reg,
          SOC_PETRA_DEFAULT_INSTANCE,
          &buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &(buffer),
          SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SHIFT,
          SOC_PETRA_MULT_FABRIC_SHAPER_MAX_BURST_SIZE,
          &(info->max_burst)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_get_any_field(
          &(buffer),
          SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SHIFT,
          SOC_PETRA_MULT_FABRIC_SHAPER_RATE_SIZE,
          &(rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_shaper_get()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_be_wfq_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_BE_INFO       *info,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO       *exact_info
    )
{
  uint32
    res;
  uint32
    wfq_enable = info->wfq_enable == TRUE ? 0x1 : 0x0,
    be_sch_port[SOC_PETRA_NOF_MULTICAST_CLASSES - 1],
    buffer = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  be_sch_port[SOC_PETRA_MULTICAST_CLASS_0] = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].weight;
  be_sch_port[SOC_PETRA_MULTICAST_CLASS_1] = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].weight;
  be_sch_port[SOC_PETRA_MULTICAST_CLASS_2] = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].weight;

  /*
   * SP vs WFQ info
   * if SP do not write WFQ. Otherwise write.
   */

  SOC_PETRA_REG_GET(regs->ips.bfmc_class_configs_reg, (buffer), 10, exit);

  SOC_PETRA_FLD_TO_REG(regs->ips.bfmc_class_configs_reg.bfmc_wfq_en, wfq_enable, (buffer), 20, exit);

  exact_info->wfq_enable = info->wfq_enable;

  if (wfq_enable)
  {
    SOC_PETRA_FLD_TO_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_0], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_0]), (buffer), 50, exit);
    exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].weight = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].weight;

    SOC_PETRA_FLD_TO_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_1], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_1]), (buffer), 40, exit);
    exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].weight = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].weight;

    SOC_PETRA_FLD_TO_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_2], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_2]), (buffer), 30, exit);
    exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].weight = info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].weight;
  }

  SOC_PETRA_REG_SET(regs->ips.bfmc_class_configs_reg, buffer, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_be_wfq_set()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_be_wfq_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO       *info
    )
{
  uint32
    res;
  uint32
    wfq_enable = 0,
    be_sch_port[SOC_PETRA_NOF_MULTICAST_CLASSES - 1],
    buffer = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  be_sch_port[SOC_PETRA_MULTICAST_CLASS_0] = 0;
  be_sch_port[SOC_PETRA_MULTICAST_CLASS_1] = 0;
  be_sch_port[SOC_PETRA_MULTICAST_CLASS_2] = 0;

  /*
   * SP vs WFQ info
   * if SP do not write WFQ. Otherwise write.
   */
  SOC_PETRA_REG_GET(regs->ips.bfmc_class_configs_reg, (buffer), 20, exit);

  SOC_PETRA_FLD_FROM_REG(regs->ips.bfmc_class_configs_reg.bfmc_wfq_en, (wfq_enable), (buffer), 30, exit);

  info->wfq_enable = (wfq_enable==0x1 ? TRUE : FALSE);

  if (info->wfq_enable)
  {
    SOC_PETRA_FLD_FROM_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_0], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_0]), (buffer), 40, exit);

    SOC_PETRA_FLD_FROM_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_1], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_1]), (buffer), 50, exit);

    SOC_PETRA_FLD_FROM_REG(regs->ips.bfmc_class_configs_reg.bfmc_class_w[SOC_PETRA_MULTICAST_CLASS_2], (be_sch_port[SOC_PETRA_MULTICAST_CLASS_2]), (buffer), 60, exit);
  }

  info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].weight = be_sch_port[SOC_PETRA_MULTICAST_CLASS_0];
  info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].weight = be_sch_port[SOC_PETRA_MULTICAST_CLASS_1];
  info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].weight = be_sch_port[SOC_PETRA_MULTICAST_CLASS_2];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_be_wfq_get()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_be_set(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_BE_INFO    *info,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO    *exact_info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /*
   * SP and WFQ info
   */
  res = soc_petra_mult_fabric_credit_source_be_wfq_set(
          unit,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set shaper info
   */
  res = soc_petra_mult_fabric_credit_source_shaper_set(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ips.bfmc_shaper_configs_reg.addr),
          &(info->be_shaper),
          &(exact_info->be_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Set port info
   */
  res = soc_petra_mult_fabric_credit_source_port_set(
          unit,
          SOC_PETRA_MULTICAST_CLASS_3,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].be_sch_port),
          &(exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_mult_fabric_credit_source_port_set(
          unit,
          SOC_PETRA_MULTICAST_CLASS_2,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].be_sch_port),
          &(exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_mult_fabric_credit_source_port_set(
          unit,
          SOC_PETRA_MULTICAST_CLASS_1,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].be_sch_port),
          &(exact_info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_be_set()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_be_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO *info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_BE_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Get SP and WFQ info
   */
  res = soc_petra_mult_fabric_credit_source_be_wfq_get(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Get shaper info
   */
  res = soc_petra_mult_fabric_credit_source_shaper_get(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ips.bfmc_shaper_configs_reg.addr),
          &(info->be_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Get port info- inversion in the scheduler for the order
   * of importance of the multicast classes MC3 < MC2 < MC1 (best effort)
   */
  res = soc_petra_mult_fabric_credit_source_port_get(
          unit,
          SOC_PETRA_MULTICAST_CLASS_3,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_0].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_mult_fabric_credit_source_port_get(
          unit,
          SOC_PETRA_MULTICAST_CLASS_2,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_1].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_mult_fabric_credit_source_port_get(
          unit,
          SOC_PETRA_MULTICAST_CLASS_1,
          &(info->be_sch_port[SOC_PETRA_MULTICAST_CLASS_2].be_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_be_get()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_gu_set(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_GR_INFO *info,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_GR_INFO *exact_info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GU_SET);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /*
   * Set shaper info
   */
  res = soc_petra_mult_fabric_credit_source_shaper_set(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ips.gfmc_shaper_configs_reg.addr),
          &(info->gr_shaper),
          &(exact_info->gr_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set port info - inversion in the scheduler for the order of importance of the
   * multicast classes: the guaranteed multicast class is 0
   */
  res = soc_petra_mult_fabric_credit_source_port_set(
          unit,
          SOC_PETRA_MULTICAST_CLASS_0,
          &(info->gr_sch_port),
          &(exact_info->gr_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_gu_set()",0,0);
}

STATIC
  uint32
    soc_petra_mult_fabric_credit_source_gu_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_GR_INFO *info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GU_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Set shaper info
   */
  res = soc_petra_mult_fabric_credit_source_shaper_get(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ips.gfmc_shaper_configs_reg.addr),
          &(info->gr_shaper)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set port info- inversion in the scheduler for the order of importance of the
   * multicast classes: the guaranteed multicast class is 0
   */
  res = soc_petra_mult_fabric_credit_source_port_get(
          unit,
          SOC_PETRA_MULTICAST_CLASS_0,
          &(info->gr_sch_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_gu_get()",0,0);
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
  soc_petra_mult_fabric_credit_source_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *exact_info
  )
{
  uint32
    res;
  uint32
    rate = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  SOC_PETRA_FLD_SET(regs->ips.ips_general_configurations_reg.fmc_credits_from_sch, info->credits_via_sch, 50, exit);
    exact_info->credits_via_sch = info->credits_via_sch;

  res = soc_petra_intern_rate2clock(
          unit,
          info->max_rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  SOC_PETRA_FLD_SET(regs->ips.fmc_scheduler_configs_reg.fmc_max_cr_rate, rate, 70, exit);

  res = soc_petra_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
         &(exact_info->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

  res = soc_petra_mult_fabric_credit_source_be_set(
          unit,
          &(info->best_effort),
          &(exact_info->best_effort)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_mult_fabric_credit_source_gu_set(
          unit,
          &(info->guaranteed),
          &(exact_info->guaranteed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_set_unsafe()",0,0);
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
  soc_petra_mult_fabric_credit_source_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *info
  )
{
  uint32
    res;
  uint32
    credits_via_sch = 0,
    rate = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_mult_fabric_credit_source_gu_get(
          unit,
          &(info->guaranteed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mult_fabric_credit_source_be_get(
          unit,
          &(info->best_effort)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_FLD_GET(regs->ips.fmc_scheduler_configs_reg.fmc_max_cr_rate, (rate), 30, exit);

  res = soc_petra_intern_clock2rate(
          unit,
          rate,
          FALSE, /* is_for_ips FALSE: for FMC */
          &(info->max_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  SOC_PETRA_FLD_GET(regs->ips.ips_general_configurations_reg.fmc_credits_from_sch, credits_via_sch, 40, exit);
  info->credits_via_sch = credits_via_sch  == 1 ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    reg_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ENHANCED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->eci.fabric_multicast_queue_boundaries_reg, reg_val, 10, exit);

  SOC_PETRA_FLD_TO_REG(regs->eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_high,
                      queue_range->end, reg_val, 20, exit);

  SOC_PETRA_FLD_TO_REG(regs->eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_low,
                      queue_range->start, reg_val, 30, exit);

  SOC_PETRA_REG_SET(regs->eci.fabric_multicast_queue_boundaries_reg, reg_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_enhanced_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ENHANCED_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_range->start, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
   );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_range->end, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 20, exit
   );

  if (queue_range->start > queue_range->end)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULTI_FABRIC_QUEUE_ORDER_ERR, 30, exit);
 }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_enhanced_set_verify()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    reg_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ENHANCED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->eci.fabric_multicast_queue_boundaries_reg, reg_val, 10, exit);

  SOC_PETRA_FLD_FROM_REG(regs->eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_high,
    queue_range->end, reg_val, 20, exit);

  SOC_PETRA_FLD_FROM_REG(regs->eci.fabric_multicast_queue_boundaries_reg.fmc_qnum_low,
    queue_range->start, reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_enhanced_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                      tbl_refresh_enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(links);
  SOC_SAND_MAGIC_NUM_VERIFY(links);

  if (links->bitmap[1] > SOC_SAND_BITS_MASK((SOC_DPP_DEFS_GET(unit, nof_fabric_links) - 32) - 1, 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_FABRIC_ILLEGAL_NOF_LINKS, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_active_links_verify()",0,0);
}

/*********************************************************************
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  )
{
  uint32
    fld_val,
    res,
    indx;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(links);

  for (indx = 0;
       indx < SOC_PETRA_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS;
       indx++)
  {
    SOC_PETRA_REG_SET(regs->rtp.multicast_link_up_reg[indx], links->bitmap[indx], 10, exit);
  }

  fld_val = SOC_SAND_BOOL2NUM(tbl_refresh_enable);
  SOC_PETRA_FLD_SET(regs->rtp.multicast_distribution_configuration_reg.enable_mclupdates, fld_val, 10, exit);

  fld_val = SOC_SAND_BOOL2NUM(!tbl_refresh_enable);
  SOC_PETRA_FLD_SET(regs->rtp.multicast_distribution_configuration_reg.bypass_update, fld_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_active_links_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  )
{
  uint32
    res,
    indx;
  uint32
    local_tbl_refresh_enable = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(links);
  SOC_SAND_CHECK_NULL_INPUT(tbl_refresh_enable);

  for (indx = 0;
       indx < SOC_PETRA_MULT_FABRIC_NOF_UINT32S_FOR_ACTIVE_MC_LINKS;
       indx++)
  {
    SOC_PETRA_REG_GET(regs->rtp.multicast_link_up_reg[indx], links->bitmap[indx], 10, exit);
  }

  SOC_PETRA_FLD_GET(regs->rtp.multicast_distribution_configuration_reg.enable_mclupdates, local_tbl_refresh_enable, 20, exit);

  *tbl_refresh_enable = SOC_SAND_NUM2BOOL(local_tbl_refresh_enable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_active_links_get_unsafe()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
