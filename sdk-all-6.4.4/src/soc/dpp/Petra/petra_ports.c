/* $Id: petra_ports.c,v 1.11 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_nif.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/Petra/petra_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_ports.c,v 1.11 Broadcom SDK $
 *  This is just to verify we don't get stuck in an infinite loop in case of error condition
 */
#define SOC_PETRA_PORTS_IF_ITERATIONS_MAX (SOC_PETRA_NOF_MAC_LANES * SOC_PETRA_NOF_IF_CHANNELS_MAX + 10)


/*
 * by implementation Each LAG has at least one member.
 * undefined LAG (lag with no members) should not be used (pointed to)
 * by SW: LAG may has zero members.
 *        SW fills the lonely LAG member with this system port,
 *        which forward such packets to system port 4095
 */

/* 
 * Internal indication of an unspecified (unknown) channel  
 */
#define SOC_PA_PORTS_CH_UNKNOWN 0xffff

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

/* Declaration, see definition below */
STATIC uint32
  soc_pa_port_to_interface_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  );

uint32
  soc_petra_ports_fap_and_nif_type_match_verify(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID if_id,
    SOC_SAND_IN SOC_PETRA_FAP_PORT_ID  fap_port_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FAP_AND_NIF_TYPE_MATCH_VERIFY);

  if (SOC_PETRA_IS_CPU_IF_ID(if_id))
  {
    if (!(SOC_PETRA_IS_CPU_FAP_PORT_ID(fap_port_id)))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 10, exit);
    }
  }
  else if (SOC_PETRA_IS_OLP_IF_ID(if_id))
  {
    if (!(SOC_PETRA_IS_OLP_FAP_PORT_ID(fap_port_id)))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 20, exit);
    }
  }
  else if(!SOC_PETRA_IS_NIF_ID(if_id) && !SOC_PETRA_IS_RCY_IF_ID(if_id) && !SOC_PETRA_IS_NONE_IF_ID(if_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_fap_and_nif_type_match_verify()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_ports_regs_init
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
  soc_petra_ports_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_REGS_INIT);

  regs = soc_petra_regs();

  /*
   *	CAUTION: the following register has in practice a different
   *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
   */
  SOC_PETRA_FLD_SET(regs->ire.cpu_fap_port_configuration_reg.cpu_fap_port, SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION, 10, exit);
  SOC_PETRA_FLD_SET(regs->ire.olp_fap_port_configuration_reg.olp_fap_port, SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_ports_init
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
  soc_petra_ports_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_INIT);

  res = soc_petra_ports_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_init()",0,0);
}

uint32 
  soc_pa_ports_expected_chan_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_NIF_TYPE      nif_type,
    SOC_SAND_IN  uint8           is_channelized_interface,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID  nif_id,
    SOC_SAND_OUT uint32           *channel
  )
{
  uint32
    ch_id = SOC_PA_PORTS_CH_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_EXPECTED_CHAN_GET);

  switch(nif_type)
  {
  case SOC_PETRA_NIF_TYPE_XAUI:
  case SOC_PETRA_NIF_TYPE_SPAUI:
    if (!SOC_PETRA_IS_MAL_NIF_ID(nif_id))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_NIF_ID_NOT_FIRST_IN_MAL_ERR, 20, exit);
    }
    if (is_channelized_interface)
    {
      ch_id = SOC_PA_PORTS_CH_UNKNOWN;
    }
    else
    {
      ch_id = 0;
    }
    break;
  case SOC_PETRA_NIF_TYPE_SGMII:
    ch_id = (SOC_PETRA_NIF_SGMII_LANE(nif_id)* (SOC_PETRA_NOF_IF_CHANNELS_MAX / SOC_PETRA_MAX_NIFS_PER_MAL));
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit);
    break;
  }

  *channel = ch_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_expected_chan_get()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pa_port_ingr_map_write_val
* TYPE:
*   PROC
* FUNCTION:
*     Write the specified mapping value to a device.
*     Maps/Unmaps Incoming FAP Port to ingress Interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32            port_ndx -
*     FAP Port index. Range: 0-79.
*  SOC_SAND_IN  uint8            is_mapped -
*     If TRUE - map. If FALSE - unmap.
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
*     Interface and channel to map.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_pa_port_ingr_map_write_val(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  uint8            is_mapped,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
  )
{
  uint32
    offset = 0,
    port_mapping_val,
    to_map_val,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;
  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA
    nif2xtxt_data;
  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA
    rcy_ctxt_map_data;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized_interface = FALSE;
  SOC_PETRA_INTERFACE_ID
    if_id = map_info->if_id;
  uint32
    reg_idx,
    fld_idx,
    mal_id,
    ch_id = map_info->channel_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_INGRESS_MAP);

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(if_id);

  /* mapping value - FAP index or unmapped */
  port_mapping_val = is_mapped?port_ndx:SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION;

  if (SOC_PETRA_IS_NIF_ID(if_id))
  {
    res = soc_petra_nif_type_get_unsafe(
            unit,
            SOC_PETRA_NIF2MAL_NDX(if_id),
            &nif_type,
            &is_channelized_interface
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(is_channelized_interface)
    {
      ch_id = map_info->channel_id;
    }
    else
    {
      res = soc_pa_ports_expected_chan_get(
              unit,              
              nif_type,
              is_channelized_interface,
              if_id,
              &ch_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if(ch_id == SOC_PA_PORTS_CH_UNKNOWN)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_NON_CH_IF_ERR, 35, exit);
      }
    }
    
    /*
     *  Map port to interface (MAL).
     */
    res = soc_petra_ire_nif_port2ctxt_bit_map_tbl_get_unsafe(
            unit,
            mal_id,
            &(nif2xtxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    reg_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
    if(!is_channelized_interface)
    {
      to_map_val = 0x0; /* Must be unset if mapped to non-channelized interface */
    }
    SOC_SAND_SET_BIT(nif2xtxt_data.contexts_bit_mapping[reg_idx], to_map_val, fld_idx);

    res = soc_petra_ire_nif_port2ctxt_bit_map_tbl_set_unsafe(
            unit,
            mal_id,
            &(nif2xtxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     *  Map port to channel (in-MAL offset for SGMII)
     */

    nif_ctxt_map_data.fap_port = port_mapping_val;
    offset = mal_id * SOC_PETRA_NOF_IF_CHANNELS_MAX + ch_id;

    res = soc_petra_ire_nif_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(nif_ctxt_map_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else if(SOC_PETRA_IS_CPU_IF_ID(if_id))
  {
    if (
        (port_mapping_val == SOC_PETRA_FRST_CPU_PORT_ID) ||
        (port_mapping_val == SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION)
       )
    {
      /*
       *	CAUTION: the following register has in practice a different
       *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
       */
     SOC_PETRA_FLD_SET(regs->ire.cpu_fap_port_configuration_reg.cpu_fap_port, port_mapping_val, 70, exit);
    }
  }
  else if (SOC_PETRA_IS_OLP_IF_ID(if_id))
  {
    SOC_PETRA_FLD_SET(regs->ire.olp_fap_port_configuration_reg.olp_fap_port, port_mapping_val, 80, exit);
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(if_id))
  {
    /* mapping value - FAP index or unmapped */
    rcy_ctxt_map_data.fap_port = port_mapping_val;
    offset = ch_id;

    res = soc_petra_ire_rcy_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(rcy_ctxt_map_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_ingr_map_write_val()",port_ndx,map_info->if_id);
}

/*********************************************************************
* NAME:
*     soc_pa_port2if_ingr_map_new_unmap_old
* TYPE:
*   PROC
* FUNCTION:
*     Maps Incoming FAP Port to ingress Interface
*     Check if the specified FAP Port is already mapped.
*     If currently mapped to a different interface - unmap the
*     old mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32            port_ndx -
*     FAP Port index. Range: 0-79.
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
*     Interface and channel to map.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_pa_port2if_ingr_map_new_unmap_old(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;
  uint8
    is_already_mapped = FALSE;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    curr_info,
    out_curr_info;
  uint32
    iter_count = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT2IF_INGR_MAP_NEW_UNMAP_OLD);

  /*
     * Find if the FAP with the given index is already mapped,
     * if it is - mark it unmap it.
     * Note: in a valid state, this loop will only be entered once,
     * since otherwise currently more then one interface is mapped
     * to a single FAP port.
     */
    do
    {
      res = soc_pa_port_to_interface_map_get_unsafe(
              unit,
              port_ndx,
              &curr_info,
              &out_curr_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if ((curr_info.if_id == info->if_id) &&
          (curr_info.channel_id == info->channel_id))
      {
        /*
         * If the FAP port is already mapped to the interface,
         * leave the configuration as is.
         */
        is_already_mapped = TRUE;
      }
      else
      {
        if (curr_info.if_id != SOC_PETRA_IF_ID_NONE)
        {
          /*
           * The FAP port is currently mapped -
           * remove the previous mapping.
           */
          res = soc_pa_port_ingr_map_write_val(
                  unit,
                  port_ndx,
                  FALSE,
                  &curr_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
      }

      if (iter_count++ >= SOC_PETRA_PORTS_IF_ITERATIONS_MAX)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR, 25, exit);
      }
    } while((!is_already_mapped) && (curr_info.if_id != SOC_PETRA_IF_ID_NONE));

    if ((!is_already_mapped) && (info->if_id != SOC_PETRA_IF_ID_NONE))
    {
      /*
       * Write the FAP index to the HW,
       * this is the actual mapping.
       */
      res = soc_pa_port_ingr_map_write_val(
              unit,
              port_ndx,
              TRUE,
              info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port2if_ingr_map_new_unmap_old()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pa_port_egr_map_write_val
* TYPE:
*   PROC
* FUNCTION:
*     Write the specified mapping value to a device.
*     Maps/Unmaps Outgoing FAP Port to egress Interface
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32            port_ndx -
*     FAP Port index. Range: 0-79.
*  SOC_SAND_IN  uint8            is_mapped -
*     If TRUE - map. If FALSE - unmap.
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
*     Interface and channel to map.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_pa_port_egr_map_write_val(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  uint8            is_mapped,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    to_map_val = 0,
    res;
  uint32
    ch_id = map_info->channel_id,
    nonch_if_id,
    mal_id,
    sch_nif_offset,
    reg_outer_idx = 0,
    reg_inner_idx = 0,
    fld_idx = 0;
  SOC_PETRA_INTERFACE_ID
    if_id = map_info->if_id;
  uint8
    is_channelized_id = FALSE,
    is_channelized_interface;
  SOC_PETRA_NIF_TYPE
    nif_type;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    pct_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_EGR_MAP_WRITE_VAL);

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(if_id);

  if (SOC_PETRA_IS_NIF_ID(if_id))
  {
    /* NIF */
    res = soc_petra_nif_type_get_unsafe(
            unit,
            SOC_PETRA_NIF2MAL_NDX(if_id),
            &nif_type,
            &is_channelized_interface
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    switch(nif_type)
    {
    case SOC_PETRA_NIF_TYPE_XAUI:
    case SOC_PETRA_NIF_TYPE_SPAUI:
      if (!SOC_PETRA_IS_MAL_NIF_ID(if_id))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_NIF_ID_NOT_FIRST_IN_MAL_ERR, 20, exit);
      }
      ch_id = map_info->channel_id;
      break;
    case SOC_PETRA_NIF_TYPE_SGMII:
      ch_id = 0;
      sch_nif_offset = soc_petra_sch_if2sched_offset(map_info->if_id);
      if (!soc_petra_is_channelized_interface_id(map_info->if_id))
      {
        SOC_PETRA_FLD_SET(regs->sch.port_nif_reg[sch_nif_offset].nifxx_port_id, port_ndx, 25, exit);
      }
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit);
      break;
    }

    is_channelized_id = SOC_PETRA_IS_MAL_NIF_ID(if_id);
    if (is_channelized_id)
    {
      /* set SOC_PETRA_IF_CH_NIF { */

      /* Outer index - MAL index of the channelized NIF */
      reg_outer_idx = mal_id;
      /* Each NIF is configured via 3 registers */
      reg_inner_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
      fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

      SOC_PA_REG_GET(regs->egq.map_conf_chnif_port_reg[reg_outer_idx][reg_inner_idx], reg_val, 35, exit);

      to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
      SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

      SOC_PA_REG_SET(regs->egq.map_conf_chnif_port_reg[reg_outer_idx][reg_inner_idx], reg_val, 40, exit);
      /* set SOC_PETRA_IF_CH_NIF } */
    }
    else
    {
      /* set SOC_PETRA_IF_NONCH_NIF { */
      /*
       * non-channelized id-s are in range 0 - 23.
       * It does not include indexes 0, 4, 8... (channelized indexes).
       */
      nonch_if_id = if_id - (SOC_PETRA_NIF2MAL_NDX(if_id) + 1);
      reg_inner_idx = SOC_PETRA_REG_IDX_GET(nonch_if_id, SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_FLDS);
      fld_idx = SOC_PETRA_FLD_IDX_GET(nonch_if_id, SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_FLDS);

      SOC_PA_REG_GET(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx], reg_val, 50, exit);

      to_map_val = SOC_SAND_BOOL2NUM(is_mapped);

      SOC_PA_FLD_TO_REG(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx].valid[fld_idx], to_map_val, reg_val, 60, exit);

      fld_val = port_ndx;

      SOC_PA_FLD_TO_REG(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx].ofp_index[fld_idx], fld_val, reg_val, 70, exit);

      SOC_PA_REG_SET(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx], reg_val, 80, exit);
      /* set SOC_PETRA_IF_NONCH_NIF } */
    }
  }
  else if(SOC_PETRA_IS_ECI_IF_ID(if_id))
  {
    /* CPU, OLP */
    SOC_PA_REG_GET(regs->egq.map_conf_cpu_ports_reg, reg_val, 90, exit);

    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);

    if (port_ndx == SOC_PETRA_FRST_CPU_PORT_ID)
    {
      /* CPU */
      fld_idx = 0;
    } else if (SOC_SAND_IS_VAL_IN_RANGE(port_ndx, SOC_PETRA_SCND_CPU_PORT_ID, SOC_PETRA_LAST_CPU_PORT_ID))
    {
      /* CPU */
      fld_idx = port_ndx - SOC_PETRA_SCND_CPU_PORT_ID + 1;
    } else if (SOC_PETRA_IS_OLP_FAP_PORT_ID(port_ndx))
    {
      /* OLP */
      fld_idx = 7;

      SOC_PA_FLD_SET(regs->sch.olp_config_reg.olpport_id, port_ndx, 95, exit);
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_INVALID_ECI_PORT_IDX_ERR, 100, exit);
    }

    SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

    SOC_PA_REG_SET(regs->egq.map_conf_cpu_ports_reg, reg_val, 110, exit);
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(if_id))
  {
    /* RCY */
    reg_inner_idx = port_ndx / SOC_SAND_REG_SIZE_BITS;
    fld_idx = port_ndx % SOC_SAND_REG_SIZE_BITS;

    SOC_PA_REG_GET(regs->egq.map_conf_rcy_ports_reg[reg_inner_idx], reg_val, 120, exit);

    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
    SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

    SOC_PA_REG_SET(regs->egq.map_conf_rcy_ports_reg[reg_inner_idx], reg_val, 130, exit);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 140, exit);
  }

  is_channelized_interface = soc_petra_is_channelized_interface_id(if_id);
  if (is_channelized_id || is_channelized_interface)
  {
    /* Configure the channel number */
    res = soc_petra_egq_pct_tbl_get_unsafe(
            unit,
            port_ndx,
            &(pct_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    pct_data.port_channel_number = ch_id;

    res = soc_petra_egq_pct_tbl_set_unsafe(
            unit,
            port_ndx,
            &(pct_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_egr_map_write_val()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pa_port2if_egr_map_new_unmap_old
* TYPE:
*   PROC
* FUNCTION:
*     Maps Outgoing FAP Port to egress Interface
*     Check if the specified FAP Port is already mapped.
*     If currently mapped to a different interface - unmap the
*     old mapping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32            port_ndx -
*     FAP Port index. Range: 0-79.
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO   *map_info
*     Interface and channel to map.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_pa_port2if_egr_map_new_unmap_old(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;
  uint8
    is_already_mapped = FALSE;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    curr_info,
    in_curr_info;
  uint32
    iter_count = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT2IF_EGR_MAP_NEW_UNMAP_OLD);

  /*
   * Find if the FAP with the given index is already mapped,
   * if it is - mark it unmap it.
   * Note: in a valid state, this loop will only be entered once,
   * since otherwise currently more then one interface is mapped
   * to a single FAP port.
   */
  is_already_mapped = FALSE;
  do
  {
    res = soc_pa_port_to_interface_map_get_unsafe(
            unit,
            port_ndx,
            &in_curr_info,
            &curr_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((curr_info.if_id == info->if_id) &&
        (curr_info.channel_id == info->channel_id))
    {
      /*
       * If the FAP port is already mapped to the interface,
       * leave the configuration as is.
       */
      is_already_mapped = TRUE;
    }
    else
    {
      if (curr_info.if_id != SOC_PETRA_IF_ID_NONE)
      {
        /*
         * The FAP port is currently mapped -
         * remove the previous mapping.
         */
        res = soc_pa_port_egr_map_write_val(
                unit,
                port_ndx,
                FALSE,
                &curr_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }

    if (iter_count++ >= SOC_PETRA_PORTS_IF_ITERATIONS_MAX)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR, 25, exit);
    }

  } while((!is_already_mapped) && (curr_info.if_id != SOC_PETRA_IF_ID_NONE));

  if ((!is_already_mapped) && (info->if_id != SOC_PETRA_IF_ID_NONE))
  {
    /*
     * Write the FAP index to the HW,
     * this is the actual mapping.
     */
    res = soc_pa_port_egr_map_write_val(
              unit,
              port_ndx,
              TRUE,
              info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port2if_egr_map_new_unmap_old()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_ports_logical_sys_id_build
* TYPE:
*   PROC
* FUNCTION:
*     Build Logical System Port used internally to indicate
*     either lag index and lag member index, or System Physical Port.
*     The Logical System Port structure is defined as following:
*     +------------------------------------------------------------------------+
*     | Field              Size | Bit/s | Meaning                               |
*     +------------------------------------------------------------------------+
*     | IS_LAG              1   |  12   | Indicates that the port is a LAG port |
*     +------------------------------------------------------------------------+
*     | If (IS_LAG)            |       |                                       |
*     +------------------------------------------------------------------------+
*     | LAG_PORT_MEMBER_ID  4   |  11:8 | Identify a member physical port       |
*     |                        |       | (a LAG Port unique value)             |
*     +------------------------------------------------------------------------+
*     | LAG_PORT_ID          8   |  7:0   | The system-level LAG ID               |
*     +------------------------------------------------------------------------+
*     | Else                   |       |                                       |
*     +------------------------------------------------------------------------+
*     | PHYSICAL_PORT_ID    12 | 11:0   | System-level unique physical port ID  |
*     +------------------------------------------------------------------------+
*     | End if                 |       |                                       |
*     +------------------------------------------------------------------------+
*
* INPUT:
*   uint8 is_lag_not_phys -
*     If TRUE - Is a LAG member.
*     If FALSE - Is a System Physical Port
*   uint32 lag_id - LAG index.
*   uint32 lag_member_id - LAG member index.
*   uint32 sys_phys_port_id - System Physical Port index.
*   uint32* sys_logic_port_id - the constructed System Logical Port index.
* REMARKS:
*     Based on is_lag_not_phys value, only one of the following is
*     relevant (the other one is ignored):
*     1. <lag_id, lag_member_id>
*     2. sys_phys_port_id
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_logical_sys_id_build(
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  )
{
  uint32
    constructed_val = 0;
  SOC_PETRA_REG_FIELD
    is_lag_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LOGICAL_SYS_ID_BUILD);

  /*
   *  Initialize SYS_PORT fields
   */
  is_lag_fld.lsb = 12;
  is_lag_fld.msb = 12;

  lag_member_id_fld.lsb = 8;
  lag_member_id_fld.msb = 11;

  lag_id_fld.lsb = 0;
  lag_id_fld.msb = 7;

  phys_port_id_fld.lsb = 0;
  phys_port_id_fld.msb = 11;

  if (is_lag_not_phys)
  {
    constructed_val |= SOC_PETRA_FLD_IN_PLACE(0x1, is_lag_fld);
    constructed_val |= SOC_PETRA_FLD_IN_PLACE (lag_id, lag_id_fld);
    constructed_val |= SOC_PETRA_FLD_IN_PLACE (lag_member_id, lag_member_id_fld);
  }
  else
  {
    constructed_val |= SOC_PETRA_FLD_IN_PLACE(0x0, is_lag_fld);
    constructed_val |= SOC_PETRA_FLD_IN_PLACE (sys_phys_port_id, phys_port_id_fld);
  }

  *sys_logic_port_id = constructed_val;

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_logical_sys_id_build()",0,0);
}

/* Definition due to MBCM and a change in Arad */
uint32
  soc_petra_ports_logical_sys_id_build_with_device(
	  SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  )
{
  return soc_petra_ports_logical_sys_id_build(
            is_lag_not_phys,
            lag_id,
            lag_member_id,
            sys_phys_port_id,
            sys_logic_port_id
          );
}


/*********************************************************************
* NAME:
*     soc_petra_ports_logical_sys_id_parse
* TYPE:
*   PROC
* FUNCTION:
*     Parse Logical System Port used internally to indicate
*     either lag index and lag member index, or System Physical Port.
*     The Logical System Port structure is defined as following:
*     +------------------------------------------------------------------------+
*     | Field              Size | Bit/s | Meaning                               |
*     +------------------------------------------------------------------------+
*     | IS_LAG              1   |  12   | Indicates that the port is a LAG port |
*     +------------------------------------------------------------------------+
*     | If (IS_LAG)            |       |                                       |
*     +------------------------------------------------------------------------+
*     | LAG_PORT_MEMBER_ID  4   |  11:8 | Identify a member physical port       |
*     |                        |       | (a LAG Port unique value)             |
*     +------------------------------------------------------------------------+
*     | LAG_PORT_ID          8   |  7:0   | The system-level LAG ID               |
*     +------------------------------------------------------------------------+
*     | Else                   |       |                                       |
*     +------------------------------------------------------------------------+
*     | PHYSICAL_PORT_ID    12 | 11:0   | System-level unique physical port ID  |
*     +------------------------------------------------------------------------+
*     | End if                 |       |                                       |
*     +------------------------------------------------------------------------+
*
* INPUT:
*   uint32* sys_logic_port_id - the parsed System Logical Port index.
*   uint8 *is_lag_not_phys -
*     If TRUE - Is a LAG member.
*     If FALSE - Is a System Physical Port
*   uint32 *lag_id - LAG index.
*   uint32 *lag_member_id - LAG member index.
*   uint32 *sys_phys_port_id - System Physical Port index.
* REMARKS:
*     Based on is_lag_not_phys value, only one of the following is
*     relevant (the other one is ignored):
*     1. <lag_id, lag_member_id>
*     2. sys_phys_port_id
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_logical_sys_id_parse(
    SOC_SAND_IN  uint32  sys_logic_port_id,
    SOC_SAND_OUT uint8 *is_lag_not_phys,
    SOC_SAND_OUT uint32  *lag_id,
    SOC_SAND_OUT uint32  *lag_member_id,
    SOC_SAND_OUT uint32  *sys_phys_port_id
  )
{
  uint32
    parsed_val = sys_logic_port_id;
  SOC_PETRA_REG_FIELD
    is_lag_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LOGICAL_SYS_ID_PARSE);

  /*
   *  Initialize SYS_PORT fields
   */
  is_lag_fld.lsb = 12;
  is_lag_fld.msb = 12;

  lag_member_id_fld.lsb = 8;
  lag_member_id_fld.msb = 11;

  lag_id_fld.lsb = 0;
  lag_id_fld.msb = 7;

  phys_port_id_fld.lsb = 0;
  phys_port_id_fld.msb = 11;


  *is_lag_not_phys = SOC_SAND_NUM2BOOL(SOC_PETRA_FLD_FROM_PLACE(parsed_val, is_lag_fld));
  if (*is_lag_not_phys)
  {
    *lag_id        = SOC_PETRA_FLD_FROM_PLACE (parsed_val, lag_id_fld);
    *lag_member_id = SOC_PETRA_FLD_FROM_PLACE (parsed_val, lag_member_id_fld);
  }
  else
  {
    *sys_phys_port_id = SOC_PETRA_FLD_FROM_PLACE (parsed_val, phys_port_id_fld);
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_logical_sys_id_parse()",0,0);
}

/*********************************************************************
*     Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*     pair. The mapping is unique - single System Physical
*     Port is mapped to a single local port per specified
*     device. This configuration effects: 1. Resolving
*     destination FAP Id and OFP Id 2. Per-port pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;
  uint32
    reg_idx,
    fld_idx,
    sys_fap_id_self;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA
    ihp_my_port_data;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    ips_lookup_data;
  uint8
    is_self_port;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (mapped_fap_id == sys_fap_id_self)
  {
    /*
     *  System port is mapped to local device.
     *  Update local to system physical port mapping
     *  in the EGQ and the IHP
     */

    /* EGQ */
    res = soc_petra_egq_ppct_tbl_get_unsafe(
            unit,
            mapped_fap_port_id,
            &egq_ppct_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    egq_ppct_data.sys_port_id = sys_phys_port_ndx;

    res = soc_petra_egq_ppct_tbl_set_unsafe(
            unit,
            mapped_fap_port_id,
            &egq_ppct_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* IHP */
    ihp_port2sys_data.port_to_system_port_id = sys_phys_port_ndx;

    res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
            unit,
            mapped_fap_port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    is_self_port = TRUE;
  }
  else
  {
    is_self_port = FALSE;
  }

  /* IHP */
  reg_idx = SOC_PETRA_REG_IDX_GET(sys_phys_port_ndx, SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(sys_phys_port_ndx, SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS);

  res = soc_petra_ihp_system_port_my_port_table_tbl_get_unsafe(
          unit,
          reg_idx,
          &ihp_my_port_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  ihp_my_port_data.system_port_my_port_table[fld_idx] = SOC_SAND_BOOL2NUM(is_self_port);

  res = soc_petra_ihp_system_port_my_port_table_tbl_set_unsafe(
          unit,
          reg_idx,
          &ihp_my_port_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  /* IPS */
  ips_lookup_data.dest_dev = mapped_fap_id;
  ips_lookup_data.dest_port = mapped_fap_port_id;

  res = soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
          unit,
          sys_phys_port_ndx,
          &ips_lookup_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_sys_phys_to_local_port_map_set_unsafe()",0,0);
}


uint32
  soc_petra_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(sys_phys_to_local_port_map_set_unsafe,(unit, sys_phys_port_ndx, mapped_fap_id, mapped_fap_port_id));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_phys_to_local_port_map_set_unsafe()",0,0);
}



/*********************************************************************
*     Map System Virtual FAP Port to a <fap_id, port_id>
*     pair.
*     Enable mapping of several Virtual TM ports to the same
*     Local port.
*     Help the user to distribute the rate of a single physical
*     port to different purposes.
*     This configuration effects:
*     Resolving destination FAP Id and OFP Id
*********************************************************************/
uint32
  soc_petra_sys_virtual_port_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    ips_lookup_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);
  /* IPS */
  ips_lookup_data.dest_dev = mapped_fap_id;
  ips_lookup_data.dest_port = mapped_fap_port_id;

  res = soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
          unit,
          sys_phys_port_ndx,
          &ips_lookup_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_virtual_port_to_local_port_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*     pair. The mapping is unique - single System Physical
*     Port is mapped to a single local port per specified
*     device. This configuration effects: 1. Resolving
*     destination FAP Id and OFP Id 2. Per-port pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_phys_port_ndx, SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID,
    SOC_PETRA_SYSTEM_PHYSICAL_PORT_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mapped_fap_id, SOC_PETRA_MAX_DEVICE_ID,
    SOC_PETRA_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  res = soc_petra_fap_port_id_verify(unit, mapped_fap_port_id, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_phys_to_local_port_map_verify()",0,0);
}

/*********************************************************************
*     Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*     pair. The mapping is unique - single System Physical
*     Port is mapped to a single local port per specified
*     device. This configuration effects: 1. Resolving
*     destination FAP Id and OFP Id 2. Per-port pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  )
{
  uint32
    res;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    ips_lookup_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_id);
  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_port_id);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_phys_port_ndx, SOC_PETRA_NOF_SYS_PHYS_PORTS-1,
    SOC_PETRA_SYSTEM_PHYSICAL_PORT_OUT_OF_RANGE_ERR, 10, exit
  );

  /* IPS */

  res = soc_petra_ips_destination_device_and_port_lookup_table_tbl_get_unsafe(
          unit,
          sys_phys_port_ndx,
          &ips_lookup_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *mapped_fap_id  = (uint32)ips_lookup_data.dest_dev;
  *mapped_fap_port_id = (uint32)ips_lookup_data.dest_port;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sys_phys_to_local_port_map_get_unsafe()",0,0);
}

/*********************************************************************
*     Get a System Physical FAP Port mapped to a FAP port in
*     the local device. The mapping is unique - single System
*     Physical Port is mapped to a single local port per
*     specified device. This configuration effects: 1.
*     Resolving destination FAP Id and OFP Id 2. Per-port
*     pruning
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_local_to_sys_phys_port_map_itself_get_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint32                 fap_local_port_ndx,
      SOC_SAND_OUT uint32                 *sys_phys_port_id
    )
{
  uint32
    res;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

  res = soc_petra_egq_ppct_tbl_get_unsafe(
          unit,
          fap_local_port_ndx,
          &egq_ppct_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *sys_phys_port_id = egq_ppct_data.sys_port_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_local_to_sys_phys_port_map_itself_get_unsafe()",0,0);
}


uint32
  soc_petra_local_to_sys_phys_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;
  uint32
    sys_fap_id_self,
    curr_sys_port_id = 0,
    curr_fap_id,
    curr_port_id;
  uint8
    found = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

  res = soc_petra_fap_port_id_verify(unit, fap_local_port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fap_ndx, SOC_PETRA_MAX_DEVICE_ID,
    SOC_PETRA_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
        unit,
        &sys_fap_id_self
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* EGQ */

  if (fap_ndx == sys_fap_id_self)
  {
    SOC_PETRA_DIFF_DEVICE_CALL(local_to_sys_phys_port_map_itself_get_unsafe,(unit, fap_local_port_ndx, sys_phys_port_id));
  }
  else
  {
    do {

      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              curr_sys_port_id,
              &curr_fap_id,
              &curr_port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if ((curr_fap_id == fap_ndx) && (curr_port_id == fap_local_port_ndx))
      {
        found = TRUE;
      }
      else
      {
        curr_sys_port_id++;
      }
    } while((curr_sys_port_id <= SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID) && (!found));

    if (found)
    {
      *sys_phys_port_id = curr_sys_port_id;
    }
    else
    {
      *sys_phys_port_id = SOC_PETRA_SYS_PHYS_PORT_INVALID;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_local_to_sys_phys_port_map_get_unsafe()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;
  uint8
    in_enable,
    out_enable;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_TO_INTERFACE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  if (in_enable)
  {
    /*
     *  Map Incoming FAP Ports to Interfaces at the Ingress
     */

    res = soc_pa_port2if_ingr_map_new_unmap_old(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }

  if (out_enable)
  {
    /*
     *  Map Outgoing FAP Ports to Interfaces at the Egress
     */
    res = soc_pa_port2if_egr_map_new_unmap_old(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_to_interface_map_set_unsafe()",0,0);
}

uint32
  soc_petra_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_TO_INTERFACE_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(port_to_interface_map_set_unsafe,(unit, port_ndx, direction_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_to_interface_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_pa_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_TO_INTERFACE_MAP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  res = soc_petra_interface_id_verify(unit, info->if_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->channel_id, SOC_PETRA_NOF_IF_CHANNELS_MAX-1,
    SOC_PETRA_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 30, exit
  );

  res = soc_petra_ports_fap_and_nif_type_match_verify(
            info->if_id,
            port_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_to_interface_map_verify()",0,0);
}

uint32
  soc_petra_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_TO_INTERFACE_MAP_VERIFY);

  SOC_PETRA_DIFF_DEVICE_CALL(port_to_interface_map_verify,(unit, port_ndx, direction_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_to_interface_map_verify()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_pa_port_to_interface_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  )
{
  uint32
    offset = 0,
    port_mapping_val,
    to_map_val,
    reg_val,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;
  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA
    rcy_ctxt_map_data;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    pct_data;
  SOC_PETRA_NIF_TYPE
    nif_type;
  SOC_PETRA_INTERFACE_ID
    nif_id,
    nif_id_first;
  uint8
    is_channelized_id = FALSE,
    is_channelized_interface = FALSE,
    found = FALSE,
    is_mapped = FALSE;
  uint32
    fld_idx,
    mal_id = 0,
    ch_start, 
    ch_end,
    ch_id,
    reg_outer_idx = 0,
    reg_inner_idx = 0,
    nonch_if_id,
    max_nonch_if_id;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    info_in,
    info_out;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_TO_INTERFACE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_incoming);
  SOC_SAND_CHECK_NULL_INPUT(info_outgoing);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_in);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_out);

  /* direction = incoming */
  found = FALSE;
  /*
   *  Check NIF mapping
   */
  for (mal_id = 0; (mal_id < SOC_PETRA_NOF_MAC_LANES) && (!found); mal_id++)
  {
    res = soc_petra_nif_type_get_unsafe(
            unit,
            mal_id,
            &nif_type,
            &is_channelized_interface
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    if((nif_type == SOC_PETRA_NIF_TYPE_NONE) || (nif_type == SOC_PETRA_NIF_NOF_TYPES))
    {
      continue;
    }

    nif_id_first = SOC_PETRA_MAL2NIF_NDX(mal_id);
   
    /* Loop over all related interface id's */
    for(nif_id = nif_id_first; nif_id < nif_id_first + SOC_PETRA_MAX_NIFS_PER_MAL; nif_id++)
    {
      /* For  XAUI/SPUI only the first interface id is relevant */
      if((nif_type != SOC_PETRA_NIF_TYPE_SGMII) && (nif_id != nif_id_first))
      {
        continue;
      }

      /* Get the expected channel according to NIF TYPE & NIF ID */
      res = soc_pa_ports_expected_chan_get(
              unit,              
              nif_type,
              is_channelized_interface,
              nif_id,
              &ch_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

      if(ch_id == SOC_PA_PORTS_CH_UNKNOWN)
      {
        ch_start = 0; 
        ch_end = SOC_PETRA_NOF_IF_CHANNELS_MAX-1;
      }
      else
      {
        ch_start = ch_end = ch_id;
      }

      for (ch_id = ch_start; (ch_id <= ch_end) && (!found); ch_id++)
      {
        offset = mal_id * SOC_PETRA_NOF_IF_CHANNELS_MAX + ch_id;
        res = soc_petra_ire_nif_ctxt_map_tbl_get_unsafe(
                unit,
                offset,
                &(nif_ctxt_map_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        port_mapping_val = nif_ctxt_map_data.fap_port;
        if (port_mapping_val == port_ndx)
        {
          found = TRUE;
      
          switch(nif_type)
          {
          case SOC_PETRA_NIF_TYPE_XAUI:
          case SOC_PETRA_NIF_TYPE_SPAUI:
            info_in.channel_id = ch_id;
            info_in.if_id = nif_id;
            break;
          case SOC_PETRA_NIF_TYPE_SGMII:
            info_in.channel_id = 0;
            info_in.if_id = nif_id;
            break;
          default:
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit);
            break;
          }
        }
      }/* for Channel loop */
    }/* for NIF loop */
  } /* for MAL loop */

  if (!found)
  {
    /*
     *	CAUTION: the following register has in practice a different
     *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
     */
    /* CPU {*/
    SOC_PETRA_FLD_GET(regs->ire.cpu_fap_port_configuration_reg.cpu_fap_port, port_mapping_val, 70, exit);
    if (port_mapping_val == port_ndx)
    {
      found = TRUE;
      info_in.if_id = SOC_PETRA_IF_ID_CPU;
      info_in.channel_id = 0;
    }
    else
    {
      /* OLP {*/
      SOC_PETRA_FLD_GET(regs->ire.olp_fap_port_configuration_reg.olp_fap_port, port_mapping_val, 80, exit);
      if (port_mapping_val == port_ndx)
      {
        found = TRUE;
        info_in.if_id = SOC_PETRA_IF_ID_OLP;
        info_in.channel_id = 0;
      }
      else
      {
        /* RCY {*/
        for (ch_id = 0; (ch_id < SOC_PETRA_NOF_IF_CHANNELS_MAX) && (!found); ch_id++)
        {
          offset = ch_id;
          res = soc_petra_ire_rcy_ctxt_map_tbl_get_unsafe(
                 unit,
                 offset,
                 &(rcy_ctxt_map_data)
               );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          port_mapping_val = rcy_ctxt_map_data.fap_port;
          if (port_mapping_val == port_ndx)
          {
            found = TRUE;
            info_in.if_id = SOC_PETRA_IF_ID_RCY;
            info_in.channel_id = ch_id;
          }
        }
      } /* RCY }*/
    }/* OLP }*/
  }/* CPU }*/

  if (!found)
  {
    info_in.if_id = SOC_PETRA_IF_ID_NONE;
    info_in.channel_id = 0;
  }

  /* Outgoing */
  found = FALSE;

  /*
   *  Check NIF mapping, channelized id-s
   */
  for (mal_id = 0; (mal_id < SOC_PETRA_NOF_MAC_LANES) && (!found); mal_id++)
  {
    /* Outer index - MAL index of the channelized NIF */
    reg_outer_idx = mal_id;
    /* Each NIF is configured via 3 registers */
    reg_inner_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

    SOC_PA_REG_GET(regs->egq.map_conf_chnif_port_reg[reg_outer_idx][reg_inner_idx], reg_val, 40, exit);
    to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
    is_mapped = SOC_SAND_NUM2BOOL(to_map_val);
    if (is_mapped)
    {
      found = TRUE;
      info_out.if_id = SOC_PETRA_MAL2NIF_NDX(mal_id);
      /* Real channel id will be checked later */
      info_out.channel_id = 0;
      is_channelized_id = TRUE;
    }
  }

  if (found == FALSE)
  {
    /*
     *  Check non-channelized NIF mapping
     */
    max_nonch_if_id = SOC_PETRA_IF_NOF_NIFS - (SOC_PETRA_NIF2MAL_NDX(SOC_PETRA_IF_NOF_NIFS) + 1);
    for (nonch_if_id = 0; (nonch_if_id <= max_nonch_if_id) && (!found); nonch_if_id++)
    {
      reg_inner_idx = SOC_PETRA_REG_IDX_GET(nonch_if_id, SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_FLDS);
      fld_idx = SOC_PETRA_FLD_IDX_GET(nonch_if_id, SOC_PETRA_NOF_MAP_CONF_NONCH_NIF_FLDS);

      SOC_PA_REG_GET(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx], reg_val, 50, exit);

      SOC_PA_FLD_FROM_REG(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx].valid[fld_idx], to_map_val, reg_val, 60, exit);

      is_mapped = SOC_SAND_NUM2BOOL(to_map_val);

      if (is_mapped == TRUE)
      {
        fld_val = 0;
        SOC_PA_FLD_FROM_REG(regs->egq.map_conf_nif_non_ch_ports_reg[reg_inner_idx].ofp_index[fld_idx], fld_val, reg_val, 70, exit);

        if (fld_val == port_ndx)
        {
          found = TRUE;
          /*
           * non-channelized id-s are in range 0 - 23.
           * It does not include indexes 0, 4, 8... (channelized indexes).
           */
          info_out.if_id = nonch_if_id + (nonch_if_id/(SOC_PETRA_MAX_NIFS_PER_MAL-1)) + 1;
          info_out.channel_id = 0;
        };
      }
    } /* Non-channelized interfaces loop */
  } /* Check non-channelized interfaces */

  if (!found)
  {
    /* CPU, OLP */
    SOC_PA_REG_GET(regs->egq.map_conf_cpu_ports_reg, reg_val, 90, exit);
    if (port_ndx == SOC_PETRA_FRST_CPU_PORT_ID)
    {
      /* CPU */
      fld_idx = 0;
      to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
      is_mapped = SOC_SAND_NUM2BOOL(to_map_val);
      if (is_mapped == TRUE)
      {
        found = TRUE;
        info_out.if_id = SOC_PETRA_IF_ID_CPU;
        /* Real channel id will be checked later */
        info_out.channel_id = 0;
      }
    } else if (SOC_SAND_IS_VAL_IN_RANGE(port_ndx, SOC_PETRA_SCND_CPU_PORT_ID, SOC_PETRA_LAST_CPU_PORT_ID))
    {
      /* CPU */
      fld_idx = port_ndx - SOC_PETRA_SCND_CPU_PORT_ID + 1;
      to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
      is_mapped = SOC_SAND_NUM2BOOL(to_map_val);
      if (is_mapped == TRUE)
      {
        found = TRUE;
        info_out.if_id = SOC_PETRA_IF_ID_CPU;
        /* Real channel id will be checked later */
        info_out.channel_id = 0;
      }
    } else if (port_ndx == SOC_PETRA_OLP_PORT_ID)
    {
      /* OLP */
      fld_idx = 7;
      to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
      is_mapped = SOC_SAND_NUM2BOOL(to_map_val);
      if (is_mapped == TRUE)
      {
        found = TRUE;
        info_out.if_id = SOC_PETRA_IF_ID_OLP;
        info_out.channel_id = 0;
      }
    }

    if (!found)
    {
      /* RCY */
      reg_inner_idx = port_ndx / SOC_SAND_REG_SIZE_BITS;
      fld_idx = port_ndx % SOC_SAND_REG_SIZE_BITS;

      SOC_PA_REG_GET(regs->egq.map_conf_rcy_ports_reg[reg_inner_idx], reg_val, 120, exit);

      to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
      is_mapped = SOC_SAND_BOOL2NUM(to_map_val);
      if (is_mapped == TRUE)
      {
        found = TRUE;
        info_out.if_id = SOC_PETRA_IF_ID_RCY;
        /* Real channel id will be checked later */
        info_out.channel_id = 0;
      }
    }
  }

  if (found == TRUE)
  {
    /*
     *  Check channel index for channelized interfaces
     */
    is_channelized_interface = soc_petra_is_channelized_interface_id(info_out.if_id);
    if (is_channelized_id || is_channelized_interface)
    {
      /* Check the channel number */
      res = soc_petra_egq_pct_tbl_get_unsafe(
              unit,
              port_ndx,
              &(pct_data)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
      info_out.channel_id = pct_data.port_channel_number;
    }
  }

  if (!found)
  {
    info_out.if_id = SOC_PETRA_IF_ID_NONE;
    info_out.channel_id = 0;
  }


  SOC_PETRA_COPY(info_incoming, &info_in, SOC_PETRA_PORT2IF_MAPPING_INFO, 1);
  SOC_PETRA_COPY(info_outgoing, &info_out, SOC_PETRA_PORT2IF_MAPPING_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_to_interface_map_get_unsafe()",mal_id,0);
}

uint32
  soc_petra_port_to_interface_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_TO_INTERFACE_MAP_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(port_to_interface_map_get_unsafe,(unit, port_ndx, info_incoming, info_outgoing));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_to_interface_map_get_unsafe()",0,0);
}



uint32
  soc_pa_ports_lag_sys_port_info_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    sys_port,
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info
  )
{
  uint32
    res;
  uint32
    fap_id,
    sys_fap_id_self,
    port_id;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  uint8
    is_lag;
  uint32
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_PORTS_LAG_INFO
    in_prev_lag_info,
    out_prev_lag_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_INFO_GET_UNSAFE);

  soc_petra_PETRA_PORT_LAG_SYS_PORT_INFO_clear(port_lag_info);

  res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
          unit,
          sys_port,
          &fap_id,
          &port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
        unit,
        &sys_fap_id_self
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (fap_id  == sys_fap_id_self)
  {
    res = soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe(
            unit,
            port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_petra_ports_logical_sys_id_parse(
            ihp_port2sys_data.port_to_system_port_id,
            &is_lag,
            &lag_id,
            &lag_member_id,
            &sys_phys_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    port_lag_info->in_member = is_lag;
    if (is_lag)
    {
      port_lag_info->in_lag = lag_id;
    }
  }

  for (lag_id = 0; lag_id < SOC_PETRA_NOF_LAG_GROUPS; ++lag_id)
  {
    res = soc_petra_ports_lag_get_unsafe(
            unit,
            lag_id,
            &in_prev_lag_info,
            &out_prev_lag_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    for (lag_member_id = 0; lag_member_id < out_prev_lag_info.nof_entries; ++lag_member_id)
    {
      res = soc_petra_irr_glag_mapping_tbl_get_unsafe(
              unit,
              lag_id,
              lag_member_id,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (irr_glag_mapping_tbl_data.glag_mapping == sys_port)
      {
        ++(port_lag_info->out_lags[lag_id]);/* count how many times this port exist in this lag*/
        ++(port_lag_info->nof_of_out_lags);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_lag_sys_port_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Add Physical System port to be  member of a a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_sys_port_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  uint32                 sys_port
  )
{
  uint32
    res;
  SOC_PETRA_PORTS_LAG_INFO
    in_lag,
    out_lag;
  SOC_PETRA_PORTS_LAG_INFO
    *lag_to_set = NULL;
  uint32
    port_ndx;
  uint8
    in_enable,
    out_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_ADD_UNSAFE);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  soc_petra_PETRA_PORTS_LAG_INFO_clear(&in_lag);
  soc_petra_PETRA_PORTS_LAG_INFO_clear(&out_lag);

  res = soc_petra_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          &in_lag,
          &out_lag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (in_enable == TRUE)
  {
    lag_to_set = &in_lag;

    SOC_SAND_ERR_IF_ABOVE_MAX(
      lag_to_set->nof_entries, SOC_PETRA_PORTS_LAG_IN_MEMBERS_MAX-1,
      SOC_PETRA_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 20, exit
    );

    lag_to_set->lag_member_sys_ports[lag_to_set->nof_entries].sys_port = sys_port;
    lag_to_set->nof_entries++;

    for (port_ndx = 0; port_ndx < lag_to_set->nof_entries; port_ndx++)
    {
      lag_to_set->lag_member_sys_ports[port_ndx].member_id = port_ndx % SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX;
    }

    res = soc_petra_ports_lag_set_unsafe(
            unit,
            SOC_PETRA_PORT_DIRECTION_INCOMING,
            lag_ndx,
            lag_to_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (out_enable == TRUE)
  {
    lag_to_set = &out_lag;

    SOC_SAND_ERR_IF_ABOVE_MAX(
      lag_to_set->nof_entries, SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX-1,
      SOC_PETRA_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 35, exit
    );

    lag_to_set->lag_member_sys_ports[lag_to_set->nof_entries].sys_port = sys_port;
    lag_to_set->nof_entries++;

    for (port_ndx = 0; port_ndx < lag_to_set->nof_entries; port_ndx++)
    {
      lag_to_set->lag_member_sys_ports[port_ndx].member_id = port_ndx % SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX;
    }

    res = soc_petra_ports_lag_set_unsafe(
            unit,
            SOC_PETRA_PORT_DIRECTION_OUTGOING,
            lag_ndx,
            lag_to_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_add_unsafe()",lag_ndx,sys_port);
}

/*********************************************************************
*     Add a system port as a member in LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    sys_logic_port_id,
    dummy_sys_phys_id = 0;
  uint32
    fap_id,
    sys_fap_id_self,
    port_id;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  uint8
    in_enable,
    out_enable,
    in_use;
  uint32
    res;
  SOC_PETRA_PORTS_LAG_INFO
    in_lag,
    out_lag;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_MEMBER_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lag_member);
  SOC_SAND_CHECK_NULL_INPUT(success);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  /*
   *  Verify input
   */
  soc_petra_PETRA_PORTS_LAG_INFO_clear(&in_lag);
  in_lag.nof_entries = 1;
  in_lag.lag_member_sys_ports[0].sys_port = lag_member->sys_port;
  in_lag.lag_member_sys_ports[0].member_id = lag_member->member_id;

  res = soc_petra_ports_lag_verify(
          unit,
          direction_ndx,
          lag_ndx,
          &in_lag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  soc_petra_PETRA_PORTS_LAG_INFO_clear(&in_lag);
  soc_petra_PETRA_PORTS_LAG_INFO_clear(&out_lag);

  res = soc_petra_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          &in_lag,
          &out_lag
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (
      (in_enable && in_lag.nof_entries   >= SOC_PETRA_PORTS_LAG_IN_MEMBERS_MAX) ||
      (out_enable && out_lag.nof_entries >= SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX)
     )
  {
    /*
     *  Reached maximal number of members at least in one direction
     */
    *success = FALSE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /* get local Fap-id*/
  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* get port's Fap-id and local port*/
  res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
          unit,
          lag_member->sys_port,
          &fap_id,
          &port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (out_enable)
  {
    /* IRR */
    irr_glag_mapping_tbl_data.glag_mapping = lag_member->sys_port;
    /*
     *  This mapping affects the LAG hushing.
     *  All LAG must be set sequential here,
     *  the member-id is just a running index,
     *  not meaningful otherwise
     */
    res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
            unit,
            lag_ndx,
            out_lag.nof_entries,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   /*
    * EGQ for pruning
    */
    if (fap_id == sys_fap_id_self)
    {
      res = soc_petra_egq_ppct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      egq_ppct_data.glag_port_id = lag_ndx;
      egq_ppct_data.glag_member_port_id =
        lag_member->member_id; /* Doesn't really matter */
      egq_ppct_data.port_is_glag_member = SOC_SAND_BOOL2NUM(TRUE);

      res = soc_petra_egq_ppct_tbl_set_unsafe(
              unit,
              port_id,
              &egq_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
   /*
    * update size
    */
    irr_glag_to_lag_range_tbl_data.glag_to_lag_range = out_lag.nof_entries; /* hw will add 1*/

    res = soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    in_use = TRUE; /* sure entry in use after addition */

    res = soc_petra_sw_db_lag_in_use_set(
            unit,
            lag_ndx,
            in_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  if (in_enable && (fap_id == sys_fap_id_self))
  {
    /*
     *  Note:
     *  1. The member index here is significant only when the packet is sent to CPU and used by
     *     a higher protocol. It is not significant for LAG-based pruning
     *  2. System physical id is irrelevant - dummy_sys_phys_id is passed.
     */
    res = soc_petra_ports_logical_sys_id_build(
            TRUE,
            lag_ndx,
            lag_member->member_id,
            dummy_sys_phys_id,
            &sys_logic_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    ihp_port2sys_data.port_to_system_port_id = sys_logic_port_id;

    res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
            unit,
            port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_member_add_unsafe()",lag_ndx,0);
}

/*********************************************************************
*     Remove Physical System port to be  member of a a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_sys_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member
  )
{
  uint32
    res;
  uint32
    lag_info_idx = 0,
    fap_id,
    sys_fap_id_self,
    port_id,
    sys_port = lag_member->sys_port;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  uint32
    removed_member_index=0,
    move_member_index;
  SOC_PETRA_PORTS_LAG_INFO
    in_lag,
    out_lag;
  uint8
    found=FALSE,
    in_use,
    in_enable,
    out_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_REMOVE_UNSAFE);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  soc_petra_PETRA_PORTS_LAG_INFO_clear(&in_lag);
  soc_petra_PETRA_PORTS_LAG_INFO_clear(&out_lag);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
        unit,
        sys_port,
        &fap_id,
        &port_id
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (in_enable == TRUE)
  {
     /* restore mapping, use egress table to get the origin phy-port*/
    if (fap_id == sys_fap_id_self)
    {
      res = soc_petra_egq_ppct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      ihp_port2sys_data.port_to_system_port_id = egq_ppct_data.sys_port_id;

      res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
              unit,
              port_id,
              &ihp_port2sys_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  if (out_enable == TRUE)
  {
    res = soc_petra_ports_lag_get_unsafe(
            unit,
            lag_ndx,
            &in_lag,
            &out_lag
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    /* find which member this system port, if any */
    for (lag_info_idx = 0; lag_info_idx < out_lag.nof_entries; lag_info_idx++)
    {
      if (out_lag.lag_member_sys_ports[lag_info_idx].sys_port == sys_port)
      {
        removed_member_index = lag_info_idx;
        found = TRUE;
        break;
      }
    }
    /* if not found then there is nothing to do*/
    if (!found)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
   /*
    * initially set move_member_index to point to remove member.
    * to be set to invalid later.
    */
    move_member_index = removed_member_index;
    /* if not last then replace it with last */
    if (removed_member_index != out_lag.nof_entries-1)
    {
      move_member_index = out_lag.nof_entries-1;
      res = soc_petra_irr_glag_mapping_tbl_get_unsafe(
              unit,
              lag_ndx,
              move_member_index,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
              unit,
              lag_ndx,
              removed_member_index,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
   /*
    * update size
    */

    /* if was last member */
    if (out_lag.nof_entries <= 1)/*cannot be zero as removed member was found */
    {
      in_use = FALSE;
      irr_glag_to_lag_range_tbl_data.glag_to_lag_range = 0;
    }
    else /* >= 2, still members in LAG*/
    {
      in_use = TRUE;
      irr_glag_to_lag_range_tbl_data.glag_to_lag_range = out_lag.nof_entries - 2; /* removed & hw add 1 */
    }

    res = soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = soc_petra_sw_db_lag_in_use_set(
            unit,
            lag_ndx,
            in_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    /* empty invalid entry */
    irr_glag_mapping_tbl_data.glag_mapping = SOC_PETRA_SYS_PHYS_PORT_INVALID;
    res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
            unit,
            lag_ndx,
            move_member_index,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);


  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_remove_unsafe()",lag_ndx,sys_port);
}


/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  )
{
  uint32
    sys_logic_port_id,
    dummy_sys_phys_id = 0,
    res;
  uint32
    lag_info_idx = 0,
    fap_id,
    prev_lag_indx,
    sys_fap_id_self,
    port_id;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  SOC_PETRA_PORTS_LAG_INFO
    in_prev_lag_info,
    out_prev_lag_info;
  uint8
    in_enable,
    out_enable,
    in_use;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SET_UNSAFE);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          &in_prev_lag_info,
          &out_prev_lag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

 /*
  * in order to restore port to system port mapping first
  */
  if (in_enable)
  {
    for (prev_lag_indx = 0; prev_lag_indx < in_prev_lag_info.nof_entries; ++prev_lag_indx)
    {
      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              in_prev_lag_info.lag_member_sys_ports[prev_lag_indx].sys_port,
              &fap_id,
              &port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (fap_id == sys_fap_id_self)
      {
        res = soc_petra_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ihp_port2sys_data.port_to_system_port_id = egq_ppct_data.sys_port_id;

        res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
                unit,
                port_id,
                &ihp_port2sys_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
    }
  }

  if (out_enable)
  {
    /*
     *  Reset Outgoing configuration - set LAG to have
     *  a single invalid member
     */

    irr_glag_mapping_tbl_data.glag_mapping = SOC_PETRA_SYS_PHYS_PORT_INVALID;
    res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
            unit,
            lag_ndx,
            0,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    irr_glag_to_lag_range_tbl_data.glag_to_lag_range = 0;
    res = soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

   /*
    * reset pruning in EGQ.
    */
    for (prev_lag_indx = 0; prev_lag_indx < out_prev_lag_info.nof_entries; ++prev_lag_indx)
    {
      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              out_prev_lag_info.lag_member_sys_ports[prev_lag_indx].sys_port,
              &fap_id,
              &port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      if (fap_id == sys_fap_id_self)
      {
        res = soc_petra_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        egq_ppct_data.port_is_glag_member = SOC_SAND_BOOL2NUM(FALSE);

        res = soc_petra_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
  }

  for (lag_info_idx = 0; lag_info_idx < info->nof_entries; lag_info_idx++)
  {
    res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
            unit,
            info->lag_member_sys_ports[lag_info_idx].sys_port,
            &fap_id,
            &port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (out_enable)
    {
      /* IRR */
      irr_glag_mapping_tbl_data.glag_mapping = info->lag_member_sys_ports[lag_info_idx].sys_port;
      /*
       *  This mapping affects the LAG hushing.
       *  All LAG must be set sequential here,
       *  the member-id is just a running index,
       *  not meaningful otherwise
       */
      res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
              unit,
              lag_ndx,
              lag_info_idx,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

     /*
      * EGQ for pruning
      */
      if (fap_id == sys_fap_id_self)
      {
        res = soc_petra_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

        egq_ppct_data.glag_port_id = lag_ndx;
        egq_ppct_data.glag_member_port_id =
          info->lag_member_sys_ports[lag_info_idx].member_id; /* Doesn't really matter */
        egq_ppct_data.port_is_glag_member = SOC_SAND_BOOL2NUM(TRUE);

        res = soc_petra_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      }
    }

    if (in_enable && (fap_id == sys_fap_id_self))
    {
      /*
       *  Note:
       *  1. The member index here is significant only when the packet is sent to CPU and used by
       *     a higher protocol. It is not significant for LAG-based pruning
       *  2. System physical id is irrelevant - dummy_sys_phys_id is passed.
       */
      res = soc_petra_ports_logical_sys_id_build(
              TRUE,
              lag_ndx,
              info->lag_member_sys_ports[lag_info_idx].member_id,
              dummy_sys_phys_id,
              &sys_logic_port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      ihp_port2sys_data.port_to_system_port_id = sys_logic_port_id;

      res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
              unit,
              port_id,
              &ihp_port2sys_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }
  }

  /*
   * Set lag-size table entry {
   */

  if (out_enable)
  {
    if (info->nof_entries >= 1)
    {
      irr_glag_to_lag_range_tbl_data.glag_to_lag_range = info->nof_entries - 1;

      res = soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
              unit,
              lag_ndx,
              &irr_glag_to_lag_range_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
      in_use = TRUE;
    }
    else
    {
      in_use = FALSE;
    }

    res = soc_petra_sw_db_lag_in_use_set(
            unit,
            lag_ndx,
            in_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_set_unsafe()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_order_preserve_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, SOC_PETRA_MAX_LAG_GROUP_ID,
    SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_order_preserve_verify()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32
    res;
  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA
    irr_glag_next_member_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_SET_UNSAFE);

   /* IRR */

   /* Set, order - preserving { */
    res = soc_petra_irr_glag_next_member_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_next_member_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    irr_glag_next_member_tbl_data.rr_lb_mode =
      SOC_SAND_BOOL2NUM(is_order_preserving);

    res = soc_petra_irr_glag_next_member_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_glag_next_member_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    /*Set order - preserving } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_lag_order_preserve_set_unsafe()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32
    res;
  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA
    irr_glag_next_member_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_ORDER_PRESERVE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

   res = soc_petra_irr_glag_next_member_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_next_member_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *is_order_preserving =
    SOC_SAND_NUM2BOOL(irr_glag_next_member_tbl_data.rr_lb_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_lag_order_preserve_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_add_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  uint32                 sys_port
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_SYS_PORT_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, SOC_PETRA_MAX_LAG_GROUP_ID,
    SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_add_verify()",0,0);
}


/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_lag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  )
{
  uint8
    in_enable,
    out_enable;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);


  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, SOC_PETRA_MAX_LAG_GROUP_ID,
    SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  if (out_enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nof_entries, SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX,
      SOC_PETRA_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 30, exit
    );
  }

  if (in_enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nof_entries, SOC_PETRA_PORTS_LAG_IN_MEMBERS_MAX,
      SOC_PETRA_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 35, exit
    );
  }

  for (idx = 0; idx < info->nof_entries; idx++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->lag_member_sys_ports[idx].member_id, SOC_PETRA_PORTS_LAG_MEMBER_ID_MAX,
      SOC_PETRA_PORT_LAG_MEMBER_ID_OUT_OF_RANGE_ERR, 36, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_verify()",lag_ndx,0);
}

void
  soc_petra_ports_lag_mem_id_mark_invalid(
    SOC_SAND_INOUT SOC_PETRA_PORTS_LAG_INFO      *info
  )
{
  uint32
    ind;

  if (info != NULL)
  {
    for (ind=0; ind<SOC_PETRA_PORTS_LAG_MEMBERS_MAX; ++ind)
    {
      info->lag_member_sys_ports[ind].member_id = SOC_PETRA_FAP_PORT_ID_INVALID;
    }
  }
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_lag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *in_info,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *out_info
  )
{
  uint32
    res;
  uint32
    fap_id,
    port_id = 0,
    lag_info_idx,
    in_nof_entries = 0,
    sys_port_curr,
    port_id_curr,
    sys_fap_id_self,
    nof_members;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port2sys_data;
  uint8
    is_lag,
    in_use;
  uint32
    lag_id,
    lag_member_id,
    sys_phys_port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_LAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_info);
  SOC_SAND_CHECK_NULL_INPUT(out_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, SOC_PETRA_NOF_LAG_GROUPS-1,
    SOC_PETRA_PORT_LAG_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  /*
   *	We first mark all member-ids as invalid, since we only want to update the ones we
   *  can read.
   *  For example, the outgoing LAG member-ids for ports that are not on the self-FAP
   *  are irrelevant, and cannot be read from the local FAP.
   *  If these id-s are significant for the application, the application has to
   *  set and maintain the member-ids.
   */
  soc_petra_PETRA_PORTS_LAG_INFO_clear(in_info);
  soc_petra_ports_lag_mem_id_mark_invalid(in_info);
  soc_petra_PETRA_PORTS_LAG_INFO_clear(out_info);
  soc_petra_ports_lag_mem_id_mark_invalid(out_info);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   * Out-LAG
   */
  res = soc_petra_sw_db_lag_in_use_get(
          unit,
          lag_ndx,
          &in_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);

  if (in_use)
  {
    res = soc_petra_irr_glag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    nof_members =
      irr_glag_to_lag_range_tbl_data.glag_to_lag_range + 1;

    for (lag_info_idx = 0; lag_info_idx < nof_members; lag_info_idx++)
    {
      res = soc_petra_irr_glag_mapping_tbl_get_unsafe(
              unit,
              lag_ndx,
              lag_info_idx,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      sys_port_curr = irr_glag_mapping_tbl_data.glag_mapping;
      out_info->lag_member_sys_ports[lag_info_idx].sys_port = sys_port_curr;

      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              sys_port_curr,
              &fap_id,
              &port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (fap_id == sys_fap_id_self)
      {
        res = soc_petra_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        out_info->lag_member_sys_ports[lag_info_idx].member_id = egq_ppct_data.glag_member_port_id;
      }
    }
  }
  else
  {
    nof_members = 0;
  }

  out_info->nof_entries = nof_members;

  /*
   * In-LAG
   */
  for (port_id = 0; port_id < SOC_PETRA_NOF_FAP_PORTS; port_id++)
  {
    res = soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe(
            unit,
            port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /*
     *  Get in-LAG configuration.
     *  'sys_phys_port_id' is not parsed here,
     *  since the field is represented as a LAG.
     *  So we access the EGQ for sys-port value
     */
    res = soc_petra_ports_logical_sys_id_parse(
            ihp_port2sys_data.port_to_system_port_id,
            &is_lag,
            &lag_id,
            &lag_member_id,
            &sys_phys_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if ((is_lag) && (lag_id == lag_ndx))
    {
      /*
       *  LAG member found
       */
      res = soc_petra_egq_ppct_tbl_get_unsafe(
              unit,
              port_id,
              &(egq_ppct_data)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      sys_port_curr = egq_ppct_data.sys_port_id;
      in_info->lag_member_sys_ports[in_nof_entries].sys_port = sys_port_curr;

      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              sys_port_curr,
              &fap_id,
              &port_id_curr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (fap_id == sys_fap_id_self)
      {
        in_info->lag_member_sys_ports[in_nof_entries].member_id = lag_member_id;
      }
      in_nof_entries++;
    }
  } /* Loop through all members*/
  in_info->nof_entries = in_nof_entries;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_get_unsafe()",0,0);
}

/********************************************************************* 
*  Get a local port and answer wether this port is a lag member. 
*  if is_in_lag = TRUE  get the lag ID to lag_id  
*********************************************************************/ 
uint32  
  soc_petra_ports_is_port_lag_member_unsafe( 
    SOC_SAND_IN  int          unit, 
    SOC_SAND_IN  uint32          port_id, 
    SOC_SAND_OUT uint8           *is_in_lag, 
    SOC_SAND_OUT uint32          *lag_id) 
{         
         
  
  uint32  
    res, 
    lag_member_id,
    system_port_value,
    sys_phys_port_id;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    IHP_port_to_system_port_id_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
  
  SOC_SAND_ERR_IF_ABOVE_MAX(port_id, SOC_PETRA_MAX_FAP_PORT_ID, SOC_PETRA_FAP_PORT_ID_INVALID_ERR, 10, exit); 

  if (soc_petra_sw_db_ver_get(unit) == SOC_PETRA_DEV_VER_B)
  {  
    res = soc_pb_ports_set_in_src_sys_port_get_unsafe(unit, port_id, &system_port_value);
  }
  else
  {
    res = soc_petra_ihp_port_to_system_port_id_tbl_get_unsafe(unit, port_id, &IHP_port_to_system_port_id_tbl_data);
    system_port_value = IHP_port_to_system_port_id_tbl_data.port_to_system_port_id;
  }
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);   
  
  /* check if port is in lag  */ 
  res = soc_petra_ports_logical_sys_id_parse( 
                   system_port_value, 
                   is_in_lag, 
                   lag_id, 
                   &(lag_member_id), 
                   &sys_phys_port_id 
                   ); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ports_is_port_lag_member_unsafe()",unit,port_id); 
}

uint32
  soc_petra_ports_lag_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_set_unsafe,(unit, direction_ndx, lag_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_set_unsafe()",0,0);
}


uint32
  soc_petra_ports_lag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_get_unsafe,(unit, lag_ndx, info_incoming, info_outgoing));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_get_unsafe()",0,0);
}


uint32
  soc_petra_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_order_preserve_set_unsafe,(unit, lag_ndx, is_order_preserving));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_order_preserve_set_unsafe()",0,0);
}

uint32
  soc_petra_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_order_preserve_get_unsafe,(unit, lag_ndx, is_order_preserving));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_order_preserve_get_unsafe()",0,0);
}


uint32
  soc_petra_ports_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_member_add_unsafe,(unit, direction_ndx, lag_ndx, lag_member, success));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_member_add_unsafe()",0,0);
}

uint32
  soc_petra_ports_lag_sys_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member    
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_sys_port_remove_unsafe,(unit, direction_ndx, lag_ndx, lag_member));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_remove_unsafe()",0,0);
}

uint32
  soc_petra_ports_lag_sys_port_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sys_port,
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_lag_sys_port_info_get_unsafe,(unit, sys_port, port_lag_info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_lag_sys_port_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_port_header_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    port_type_val = 0,
    res;
  uint8
    in_enable,
    out_enable,
    itmh_remove = TRUE,
    skip_config = FALSE;
  uint32
    fatp_nof_ports,
    port_curr,
    port_max;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_HEADER_TYPE_SET_UNSAFE);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  fatp_nof_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);
  port_max = SOC_PETRA_MAX_PRT_IF_FAT_PIPE(port_ndx, fatp_nof_ports);

  if (in_enable)
  {
    skip_config = FALSE;
    switch(header_type) {
    case SOC_PETRA_PORT_HEADER_TYPE_ETH:
      itmh_remove = FALSE;
      port_type_val = 0x3;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_RAW:
      itmh_remove = FALSE;
      port_type_val = 0x1;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_TM:
      itmh_remove = TRUE;
      port_type_val = 0x2;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_PROG:
      itmh_remove = FALSE;
      port_type_val = 0x4;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_CPU:
      skip_config = TRUE;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }

    if (!skip_config)
    {
      for (port_curr = port_ndx; port_curr <= port_max; port_curr++)
      {
        /* IHP-Port Info */
        res = soc_petra_ihp_port_info_tbl_get_unsafe(
                unit,
                port_curr,
                &(ihp_port_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ihp_port_data.port_type = port_type_val;
        ihp_port_data.header_remove = SOC_SAND_BOOL2NUM(itmh_remove);

        res = soc_petra_ihp_port_info_tbl_set_unsafe(
                unit,
                port_curr,
                &(ihp_port_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
    }
  }

  if (out_enable)
  {
    skip_config = FALSE;
    switch(header_type) {
    case SOC_PETRA_PORT_HEADER_TYPE_ETH:
      port_type_val = 0x3;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_RAW:
      port_type_val = 0x1;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_TM:
      port_type_val = 0x2;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_PROG:
      skip_config = TRUE;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_CPU:
      port_type_val = 0x0;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }

    if (!skip_config)
    {
      for (port_curr = port_ndx; port_curr <= port_max; port_curr++)
      {
        /* EGQ-PCT */
        res = soc_petra_egq_pct_tbl_get_unsafe(
                unit,
                port_curr,
                &(egq_pct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        egq_pct_data.port_type = port_type_val;

        res = soc_petra_egq_pct_tbl_set_unsafe(
                unit,
                port_curr,
                &(egq_pct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        /* EGQ-PPCT */
        res = soc_petra_egq_ppct_tbl_get_unsafe(
                unit,
                port_curr,
                &(egq_ppct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        egq_ppct_data.port_type = port_type_val;

        res = soc_petra_egq_ppct_tbl_set_unsafe(
                unit,
                port_curr,
                &(egq_ppct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_header_type_set_unsafe()",0,0);
}


uint32
  soc_petra_port_header_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_HEADER_TYPE_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(port_header_type_set_unsafe,(unit, port_ndx, direction_ndx, header_type));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_header_type_set_unsafe()",0,0);
}


/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_port_header_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_HEADER_TYPE_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    header_type, SOC_PETRA_PORT_NOF_HEADER_TYPES-1,
    SOC_PETRA_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_header_type_verify()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_port_header_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    port_type_val = 0,
    res;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_data;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_NONE,
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_HEADER_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(header_type_incoming);
  SOC_SAND_CHECK_NULL_INPUT(header_type_outgoing);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* IHP-Port Info */
  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          port_ndx,
          &(ihp_port_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_type_val = ihp_port_data.port_type;

  switch(port_type_val) {
  case 0x1:
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_RAW;
    break;
  case 0x2:
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_TM;
    break;
  case 0x3:
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_ETH;
    break;
  case 0x4:
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_PROG;
    break;
  default:
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_NONE;
  }


  /* EGQ-PCT */
  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &(egq_pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_type_val = egq_pct_data.port_type;

  switch(port_type_val) {
  case 0x0:
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_CPU;
    break;
  case 0x1:
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_RAW;
    break;
  case 0x2:
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_TM;
    break;
  case 0x3:
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_ETH;
    break;
  default:
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_NONE;
  }


  *header_type_incoming = header_type_in;
  *header_type_outgoing = header_type_out;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_header_type_get_unsafe()",0,0);
}

uint32
  soc_petra_port_header_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_HEADER_TYPE_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(port_header_type_get_unsafe,(unit, port_ndx, header_type_incoming, header_type_outgoing));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_header_type_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure inbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    dest_sys_port;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA
    irr_mirror_data;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_INBOUND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          ifp_ndx,
          &ihp_port_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihp_port_data.has_mirror = info->enable;

  /*Set mirror info {*/
  res = soc_petra_irr_mirror_table_tbl_get_unsafe(
          unit,
          ifp_ndx,
          &irr_mirror_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Drop Precedence */
  irr_mirror_data.drop_precedence_over_write = SOC_SAND_BOOL2NUM(info->dp_override.enable);
  irr_mirror_data.mirror_drop_precedence = info->dp_override.override_val;

  /* Traffic Class */
  irr_mirror_data.traffic_class_over_write = SOC_SAND_BOOL2NUM(info->tc_override.enable);
  irr_mirror_data.mirror_traffic_class = info->tc_override.override_val;

  /* Is Multicast */
  ihp_port_data.mirror_is_mc =
    SOC_SAND_BOOL2NUM(info->destination.type == SOC_PETRA_DEST_TYPE_MULTICAST);

  irr_mirror_data.is_multicast = ihp_port_data.mirror_is_mc;

  /* Is Queue */
  irr_mirror_data.is_queue_number =
    SOC_SAND_NUM2BOOL(info->destination.type == SOC_PETRA_DEST_TYPE_QUEUE);

  /* Destination Index */
  switch(info->destination.type) {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    irr_mirror_data.mirror_destination = info->destination.id;
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    irr_mirror_data.mirror_destination =
      (SOC_PETRA_MULTICAST_DEST_INDICATION | info->destination.id);
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    res = soc_petra_ports_logical_sys_id_build(
            FALSE,
            0,
            0,
            info->destination.id,
            &dest_sys_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    irr_mirror_data.mirror_destination = dest_sys_port;
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    res = soc_petra_ports_logical_sys_id_build(
            TRUE,
            info->destination.id,
            0,
            0,
            &dest_sys_port
          );
     SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

     irr_mirror_data.mirror_destination = dest_sys_port;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DEST_TYPE_OUT_OF_RANGE_ERR, 35, exit);
  }
  /*Set mirror info }*/

  /*
   * If the destination is multicast, we must first update the IHP
   * (allocating multicast buffers) and only after the IRR
   * (setting the destination to multicast).
   * Otherwise, since it can be changed from multicast to unicast,
   * we must first update the IRR and only after the IHP, since
   * the multicast buffers are needed until the port destination is changed
   * to unicast).
   */
  if (info->destination.type == SOC_PETRA_DEST_TYPE_MULTICAST)
  {
    res = soc_petra_ihp_port_info_tbl_set_unsafe(
            unit,
            ifp_ndx,
            &ihp_port_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_irr_mirror_table_tbl_set_unsafe(
            unit,
            ifp_ndx,
            &irr_mirror_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else
  {
    res = soc_petra_irr_mirror_table_tbl_set_unsafe(
            unit,
            ifp_ndx,
            &irr_mirror_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_petra_ihp_port_info_tbl_set_unsafe(
            unit,
            ifp_ndx,
            &ihp_port_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_inbound_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure inbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_INBOUND_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, ifp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->dp_override.override_val, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_PORTS_MIRROR_DROP_PRCDNC_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->tc_override.override_val, SOC_PETRA_TR_CLS_MAX,
    SOC_PETRA_PORTS_MIRROR_TR_CLS_OUT_OF_RANGE_ERR, 30, exit
  );

  res = soc_petra_dest_info_verify(&(info->destination));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_inbound_verify()",0,0);
}

/*********************************************************************
*     Configure inbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA
    irr_mirror_data;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_data;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_INBOUND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fap_port_id_verify(unit, ifp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          ifp_ndx,
          &ihp_port_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->enable = SOC_SAND_NUM2BOOL(ihp_port_data.has_mirror);

  res = soc_petra_irr_mirror_table_tbl_get_unsafe(
          unit,
          ifp_ndx,
          &irr_mirror_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Drop Precedence */
  info->dp_override.enable = SOC_SAND_NUM2BOOL(irr_mirror_data.drop_precedence_over_write);
  info->dp_override.override_val = irr_mirror_data.mirror_drop_precedence;

  /* Traffic Class */
  info->tc_override.enable = SOC_SAND_NUM2BOOL(irr_mirror_data.traffic_class_over_write);
  info->tc_override.override_val = irr_mirror_data.mirror_traffic_class;

  /* Is Multicast */
  if(SOC_SAND_BOOL2NUM(ihp_port_data.mirror_is_mc) == TRUE)
  {
    info->destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
    info->destination.id = irr_mirror_data.mirror_destination & ~SOC_PETRA_MULTICAST_DEST_INDICATION;
  }
  else if(SOC_SAND_BOOL2NUM(irr_mirror_data.is_queue_number) == TRUE)
  {
    info->destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
    info->destination.id = irr_mirror_data.mirror_destination;
  }
  else
  {
    res = soc_petra_ports_logical_sys_id_parse(
                irr_mirror_data.mirror_destination,
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

       if (is_lag_not_phys)
       {
         info->destination.type = SOC_PETRA_DEST_TYPE_LAG;
         info->destination.id = lag_id;
       }
       else /* type == sys_phy_port*/
       {
         info->destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
         info->destination.id = sys_phys_port_id & ~SOC_PETRA_MULTICAST_DEST_INDICATION;
       }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_inbound_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_mirror_outbound_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    reg_val[2],
    fld_val;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA
    irr_mirror_data;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    ifp_mapping,
    out_mapping;
  uint32
    ch_id,
    start_reg_idx,
    end_reg_idx,
    start_bit,
    end_bit;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_MIRROR_OUTBOUND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  reg_val[0] = 0;
  reg_val[1] = 0;

  if (info->enable)
  {
    /*
     * EPNI - Map Ofp To Mirr Channel
     * The steps are as following:
     * 1. Verify the Incoming Port to which the OFP will be mirrored is mapped
     *    To the recycling interface.
     * 2. Get the channel number of the incoming port. The Outgoing port must
     *    be mapped to the same channel.
     * 3. Map the Outgoing port to the Recycling interface, same channel as the IFP
     * 4. Enable/disable (as requested) the mirroring configuration both
     *    at the ingress and at the egress.
     *  Note: Apart from being outbound mirrored, the Outgoing FAP port can also
     *    be mapped to any other interface (e.g. Network) at the egress.
     */

    /*
     *  Set OFP to mirror channel mapping register.
     *  Requires special treatment, since a single field may reside on
     *  two adjacent register.
     */
    res = soc_pa_port_to_interface_map_get_unsafe(
            unit,
            info->ifp_id,
            &ifp_mapping,
            &out_mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!SOC_PETRA_IS_RCY_IF_ID(ifp_mapping.if_id))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OUTBND_MIRR_IFP_NOT_MAPPED_TO_RCY_IF_ERR, 20, exit);
    }
    ch_id = ifp_mapping.channel_id;

    /*
     *  Get start and end bits in range 0 ..479
     */
    start_bit = ofp_ndx * SOC_PETRA_MAP_OFP_TO_MIRR_CH_FLD_SIZE;
    end_bit   = ((ofp_ndx + 1) * SOC_PETRA_MAP_OFP_TO_MIRR_CH_FLD_SIZE) - 1;

    start_reg_idx = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
    end_reg_idx   = SOC_PETRA_REG_IDX_GET(end_bit, SOC_SAND_REG_SIZE_BITS);

    /*
     *  Get start and end bits in range 0 ..63
     */
    start_bit = start_bit - start_reg_idx * SOC_SAND_REG_SIZE_BITS;
    end_bit = end_bit - start_reg_idx * SOC_SAND_REG_SIZE_BITS;

    SOC_PA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_val[0] ,30 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_val[1] ,40 , exit);
    }

    fld_val = (uint32)ch_id;
    fld.lsb = (uint8)start_bit;
    fld.msb = (uint8)end_bit;

    SOC_PA_IMPLICIT_FLD_TO_REG(fld, fld_val, reg_val[0], 50, exit);

    SOC_PA_REG_SET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_val[0] ,60 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PA_REG_SET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_val[1] ,70 , exit);
    }
  }

  /* IRR - Mirror Table */
  res = soc_petra_irr_mirror_table_tbl_get_unsafe(
          unit,
          info->ifp_id,
          &irr_mirror_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  irr_mirror_data.is_outbound_mirror = SOC_SAND_BOOL2NUM(info->enable);

  res = soc_petra_irr_mirror_table_tbl_set_unsafe(
          unit,
          info->ifp_id,
          &irr_mirror_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /* EGQ - PCT */
  res = soc_petra_egq_pct_tbl_get_unsafe(
    unit,
    ofp_ndx,
    &(egq_pct_data)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  egq_pct_data.outbound_mirr = SOC_SAND_BOOL2NUM(info->enable);

  res = soc_petra_egq_pct_tbl_set_unsafe(
    unit,
    ofp_ndx,
    &(egq_pct_data)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_mirror_outbound_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_mirror_outbound_set_unsafe,(unit, ofp_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_outbound_set_unsafe()",ofp_ndx,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_fap_port_id_verify(unit, info->ifp_id, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_outbound_verify()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ports_mirror_outbound_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    reg_val[2],
    fld_val;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    ifp_mapping,
    out_mapping;
  uint32
    port_idx,
    ch_id,
    start_reg_idx,
    end_reg_idx,
    start_bit,
    end_bit;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_clear(info);

  reg_val[0] = 0;
  reg_val[1] = 0;

  /* EGQ - PCT */
  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          ofp_ndx,
          &(egq_pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->enable = SOC_SAND_BOOL2NUM(egq_pct_data.outbound_mirr);

  if (info->enable)
  {
    /*
     *  Get start and end bits in range 0 ..479
     */
    start_bit = ofp_ndx * SOC_PETRA_MAP_OFP_TO_MIRR_CH_FLD_SIZE;
    end_bit   = ((ofp_ndx + 1) * SOC_PETRA_MAP_OFP_TO_MIRR_CH_FLD_SIZE) - 1;

    start_reg_idx = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
    end_reg_idx   = SOC_PETRA_REG_IDX_GET(end_bit, SOC_SAND_REG_SIZE_BITS);

    /*
     *  Get start and end bits in range 0 ..63
     */
    start_bit = start_bit - start_reg_idx * SOC_SAND_REG_SIZE_BITS;
    end_bit = end_bit - start_reg_idx * SOC_SAND_REG_SIZE_BITS;

    SOC_PA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_val[0] ,60 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_val[1] ,70 , exit);
    }

    fld.lsb = (uint8)start_bit;
    fld.msb = (uint8)end_bit;

    SOC_PA_IMPLICIT_FLD_FROM_REG(fld, fld_val, (reg_val[0]), 80, exit);

    ch_id = (uint32)fld_val;

    /*
     *  Find out the IFP mapped to this RCY channel
     */
    for (port_idx = 0; port_idx < SOC_PETRA_NOF_FAP_PORTS; port_idx++)
    {
      res = soc_pa_port_to_interface_map_get_unsafe(
              unit,
              port_idx,
              &ifp_mapping,
              &out_mapping
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (SOC_PETRA_IS_RCY_IF_ID(ifp_mapping.if_id) && (ch_id == ifp_mapping.channel_id))
      {
        info->ifp_id = port_idx;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_mirror_outbound_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_MIRROR_OUTBOUND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_mirror_outbound_get_unsafe,(unit, ofp_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_mirror_outbound_get_unsafe()",ofp_ndx,0);
}

/*********************************************************************
*     Configure the snooping function.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_snoop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO     *info
  )
{
  uint32
    res,
    fld_val,
    fld_val_mc,
    dest_sys_port;
  SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA
    irr_snoop_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SNOOP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  if (snoop_cmd_ndx != 0)
  {
    SOC_PA_FLD_GET(regs->ihp.snoop_config_reg.snoop_disable,fld_val,2,exit);
    fld_val |= SOC_SAND_BIT(snoop_cmd_ndx-1);
    SOC_PA_FLD_SET(regs->ihp.snoop_config_reg.snoop_disable,fld_val,3,exit);

    if(info->enable == FALSE)
    {
      goto exit;
    }

    fld_val = info->size_bytes;
    SOC_PA_FLD_SET(regs->irr.snoop_num_of_bytes_configuration_reg.snoop_num_of_bytes[snoop_cmd_ndx-1], fld_val, 10, exit);
  }

  res = soc_petra_irr_snoop_table_tbl_get_unsafe(
    unit,
    snoop_cmd_ndx,
    &irr_snoop_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Drop Precedence */
  irr_snoop_data.drop_precedence_over_write = SOC_SAND_BOOL2NUM(info->dp_override.enable);
  irr_snoop_data.snoop_drop_precedence = info->dp_override.override_val;

  /* Traffic Class */
  irr_snoop_data.traffic_class_over_write = SOC_SAND_BOOL2NUM(info->tc_override.enable);
  irr_snoop_data.snoop_traffic_class = info->tc_override.override_val;

  /* Is Multicast */
  irr_snoop_data.is_multicast =
    SOC_SAND_BOOL2NUM(info->destination.type == SOC_PETRA_DEST_TYPE_MULTICAST);

  /* Is Queue */
  irr_snoop_data.is_queue_number =
    SOC_SAND_BOOL2NUM(info->destination.type == SOC_PETRA_DEST_TYPE_QUEUE);

  /* Destination Index */
  switch(info->destination.type) {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    irr_snoop_data.snoop_destination = info->destination.id;
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    irr_snoop_data.snoop_destination = (SOC_PETRA_MULTICAST_DEST_INDICATION | info->destination.id);
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    res = soc_petra_ports_logical_sys_id_build(
            FALSE,
            0,
            0,
            info->destination.id,
            &dest_sys_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    irr_snoop_data.snoop_destination = dest_sys_port;
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    res = soc_petra_ports_logical_sys_id_build(
            TRUE,
            info->destination.id,
            0,
            0,
            &dest_sys_port
          );
     SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

     irr_snoop_data.snoop_destination = dest_sys_port;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DEST_TYPE_OUT_OF_RANGE_ERR, 35, exit);
  }

  res = soc_petra_irr_snoop_table_tbl_set_unsafe(
    unit,
    snoop_cmd_ndx,
    &irr_snoop_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (snoop_cmd_ndx != 0)
  {
    SOC_PA_FLD_GET(regs->ihp.snoop_config_reg.snoop_is_mc,fld_val_mc,60,exit);
    if(info->destination.type == SOC_PETRA_DEST_TYPE_MULTICAST)
    {
      fld_val_mc |= SOC_SAND_BIT(snoop_cmd_ndx-1);
    }
    else
    {
      fld_val_mc &= ~SOC_SAND_BIT(snoop_cmd_ndx-1);
    }
    SOC_PA_FLD_SET(regs->ihp.snoop_config_reg.snoop_is_mc,fld_val_mc,61,exit);

    if(info->enable)
    {
      SOC_PA_FLD_GET(regs->ihp.snoop_config_reg.snoop_disable,fld_val,62,exit);
      fld_val &= ~SOC_SAND_BIT(snoop_cmd_ndx-1);
      SOC_PA_FLD_SET(regs->ihp.snoop_config_reg.snoop_disable,fld_val,63,exit);
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_snoop_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure the snooping function.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_snoop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO     *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SNOOP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    snoop_cmd_ndx, SOC_PETRA_MAX_SNOOP_COMMAND_INDEX,
    SOC_PETRA_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->dp_override.override_val, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_PORTS_MIRROR_DROP_PRCDNC_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->tc_override.override_val, SOC_PETRA_TR_CLS_MAX,
    SOC_PETRA_PORTS_MIRROR_TR_CLS_OUT_OF_RANGE_ERR, 30, exit
  );

  res = soc_petra_dest_info_verify(&(info->destination));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_snoop_verify()",0,0);
}

/*********************************************************************
*     Configure the snooping function.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_snoop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO     *info
  )
{
  uint32
    res,
    fld_val,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA
    irr_snoop_data;
  uint8
    is_lag_not_phys;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SNOOP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    snoop_cmd_ndx, SOC_PETRA_MAX_SNOOP_COMMAND_INDEX,
    SOC_PETRA_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  if (snoop_cmd_ndx != 0)
  {
    SOC_PA_FLD_GET(regs->irr.snoop_num_of_bytes_configuration_reg.snoop_num_of_bytes[snoop_cmd_ndx-1],fld_val, 10, exit);
    info->size_bytes = fld_val;
  }
  else
  {
    info->size_bytes = 0;
  }

  info->enable = FALSE;
  SOC_PA_FLD_GET(regs->ihp.snoop_config_reg.snoop_disable,fld_val,62,exit);

  if(!SOC_SAND_GET_BIT(fld_val,snoop_cmd_ndx-1))
  {
    info->enable = TRUE;
  }

  res = soc_petra_irr_snoop_table_tbl_get_unsafe(
    unit,
    snoop_cmd_ndx,
    &irr_snoop_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Drop Precedence */
  info->dp_override.enable = SOC_SAND_NUM2BOOL(irr_snoop_data.drop_precedence_over_write);
  info->dp_override.override_val = irr_snoop_data.snoop_drop_precedence;

  /* Traffic Class */
  info->tc_override.enable = SOC_SAND_BOOL2NUM(irr_snoop_data.traffic_class_over_write);
  info->tc_override.override_val = irr_snoop_data.snoop_traffic_class;

  if (irr_snoop_data.is_queue_number == TRUE)
  {
    info->destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
    info->destination.id = irr_snoop_data.snoop_destination;
  }
  else
  {
    if (irr_snoop_data.is_multicast == TRUE)
    {
      info->destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
      info->destination.id = irr_snoop_data.snoop_destination & ~SOC_PETRA_MULTICAST_DEST_INDICATION;
    }
    else
    {
      res = soc_petra_ports_logical_sys_id_parse(
                irr_snoop_data.snoop_destination,
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

       if (is_lag_not_phys)
       {
         info->destination.type = SOC_PETRA_DEST_TYPE_LAG;
         info->destination.id = lag_id;
       }
       else /* type == sys_phy_port*/
       {
         info->destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
         info->destination.id = sys_phys_port_id;
       }
     }
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_snoop_get_unsafe()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_ports_itmh_extension_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
      SOC_SAND_IN  uint8                 ext_en
    )
{
  uint32
    res;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_info_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_ITMH_EXTENSION_SET_UNSAFE);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihp_port_info_tbl_data.port_type_extension =
    SOC_SAND_BOOL2NUM(ext_en);

  res = soc_petra_ihp_port_info_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_itmh_extension_set_unsafe()",0,0);
}

uint32
  soc_petra_ports_itmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_ITMH_EXTENSION_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_itmh_extension_set_unsafe,(unit, port_ndx, ext_en));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_itmh_extension_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_itmh_extension_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_ITMH_EXTENSION_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10 , exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_itmh_extension_verify()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_ports_itmh_extension_get_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
      SOC_SAND_OUT uint8                 *ext_en
    )
{
  uint32
    res;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_info_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_ITMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ext_en);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *ext_en = SOC_SAND_NUM2BOOL(ihp_port_info_tbl_data.port_type_extension);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_itmh_extension_get_unsafe()",0,0);
}


uint32
  soc_petra_ports_itmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT uint8                 *ext_en
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_ITMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ext_en);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_itmh_extension_get_unsafe,(unit, port_ndx, ext_en));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_itmh_extension_get_unsafe()",0,0);
}

/*********************************************************************
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO      *info
  )
{
  uint32
    res;
  SOC_PETRA_PORTS_ITMH
    itmh_info;
  SOC_PETRA_HPU_ITMH_HDR
    sh_itmh;
  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA
    ihp_static_header_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SHAPING_HEADER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_static_header_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihp_static_header_tbl_data.shaping = SOC_SAND_BOOL2NUM(info->enable);

  itmh_info.base.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
  itmh_info.base.destination.id = info->queue_id;

  itmh_info.base.dp = 0;
  itmh_info.base.exclude_src = FALSE;
  itmh_info.base.out_mirror_dis = FALSE;
  itmh_info.base.pp_header_present = FALSE;
  itmh_info.base.snoop_cmd_ndx = 0;
  itmh_info.base.tr_cls = 0;

  res = soc_petra_hpu_itmh_build(
          &itmh_info,
          &sh_itmh
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_set_field(
          &(ihp_static_header_tbl_data.shaping_itmh),
          0,
          32,
          sh_itmh.base
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ihp_static_header_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_shaping_header_set_unsafe()",0,0);
}

/*********************************************************************
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SHAPING_HEADER_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10 , exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->queue_id, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 60, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_shaping_header_verify()",0,0);
}

/*********************************************************************
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO      *info
  )
{
  uint32
    res;
  SOC_PETRA_PORTS_ITMH
    itmh_info;
  SOC_PETRA_HPU_ITMH_HDR
    sh_itmh;
  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA
    ihp_static_header_tbl_data;
  uint32
    shaping_itmh_lcl[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_SHAPING_HEADER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_PORTS_ISP_INFO_clear(info);

  res = soc_petra_ihp_static_header_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *shaping_itmh_lcl = ihp_static_header_tbl_data.shaping_itmh;
  res = soc_sand_bitstream_get_field(
          shaping_itmh_lcl,
          0,
          32,
          &(sh_itmh.base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

/*
 * COVERITY
 *
 * Irrelevant for ingress shaping (only Frwrd. Header is used)
 */
/* coverity[uninit_use_in_call] */
  res = soc_petra_hpu_itmh_parse(
          &sh_itmh,
          &itmh_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (itmh_info.base.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
  {
    info->enable = SOC_SAND_BOOL2NUM(ihp_static_header_tbl_data.shaping);
    info->queue_id = itmh_info.base.destination.id;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_shaping_header_get_unsafe()",0,0);
}

/*********************************************************************
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info
  )
{
  uint32
    res;
  SOC_PETRA_PORTS_ITMH
    info_intern;
  SOC_PETRA_HPU_ITMH_HDR
    itmh;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q;
  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA
    ihp_static_header_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FORWARDING_HEADER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_COPY(&info_intern, info, SOC_PETRA_PORTS_ITMH, 1);

  if (info_intern.base.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
  {
    soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q);

    res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
            unit,
            &base_q
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (base_q.queue_id_add_not_decrement)
    {
      SOC_SAND_ERR_IF_BELOW_MIN(
        info_intern.base.destination.id, base_q.base_queue_id,
        SOC_PETRA_Q_ID_WITH_BASE_Q_MISMATCH_ERR, 6, exit
      );

      info_intern.base.destination.id -= base_q.base_queue_id;
    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info_intern.base.destination.id, base_q.base_queue_id,
        SOC_PETRA_Q_ID_WITH_BASE_Q_MISMATCH_ERR, 7, exit
      );

      info_intern.base.destination.id += base_q.base_queue_id;
    }
  }

  res = soc_petra_hpu_itmh_build(
          &info_intern,
          &itmh
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_ihp_static_header_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*Bits 15:0 are the extension info*/
  res = soc_sand_bitstream_set_field(
          ihp_static_header_tbl_data.itmh,
          0,
          16,
          itmh.extention_src_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*Bits 16:47 are the extension info*/
  res = soc_sand_bitstream_set_field(
          ihp_static_header_tbl_data.itmh,
          16,
          32,
          itmh.base
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_ihp_static_header_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_forwarding_header_set_unsafe()",0,0);
}

/*********************************************************************
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH            *info
  )
{
  uint32
    res;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type,
    header_type_outgoing;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FORWARDING_HEADER_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10 , exit);

  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &header_type,
          &header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_hpu_itmh_build_verify(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_forwarding_header_verify()",0,0);
}

/*********************************************************************
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH            *info
  )
{
  uint32
    res;
  SOC_PETRA_PORTS_ITMH
    info_intern;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q;
  SOC_PETRA_HPU_ITMH_HDR
    itmh;
  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA
    ihp_static_header_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FORWARDING_HEADER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_PORTS_ITMH_clear(&info_intern);
  soc_petra_PETRA_PORTS_ITMH_clear(info);

  res = soc_petra_ihp_static_header_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_static_header_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*Bits 15:0 are the extension info*/
  res = soc_sand_bitstream_get_field(
          ihp_static_header_tbl_data.itmh,
          0,
          16,
          &(itmh.extention_src_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*Bits 16:47 are the extension info*/
  res = soc_sand_bitstream_get_field(
          ihp_static_header_tbl_data.itmh,
          16,
          32,
          &(itmh.base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_hpu_itmh_parse(
          &itmh,
          &info_intern
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (info_intern.base.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
  {
    soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q);

    res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
            unit,
            &base_q
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (base_q.queue_id_add_not_decrement)
    {
      info_intern.base.destination.id += base_q.base_queue_id;
    }
    else
    {
      info_intern.base.destination.id -= base_q.base_queue_id;
    }
  }

  SOC_PETRA_COPY(info, &info_intern, SOC_PETRA_PORTS_ITMH, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_forwarding_header_get_unsafe()",0,0);
}

/*********************************************************************
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_stag_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS   *info
  )
{
  uint32
    res,
    stag_pro_val = 0;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_info_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_STAG_FIELDS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  stag_pro_val = SOC_SAND_BOOL2NUM(info->cid)|
                 SOC_SAND_SET_BITS_RANGE(SOC_SAND_BOOL2NUM(info->ifp),1,1)|
                 SOC_SAND_SET_BITS_RANGE(SOC_SAND_BOOL2NUM(info->tr_cls),2,2)|
                 SOC_SAND_SET_BITS_RANGE(SOC_SAND_BOOL2NUM(info->dp),3,3)|
                 SOC_SAND_SET_BITS_RANGE(SOC_SAND_BOOL2NUM(info->data_type),4,4);

  ihp_port_info_tbl_data.statistics_profile = stag_pro_val;

  res = soc_petra_ihp_port_info_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_stag_set_unsafe()",0,0);
}

/*********************************************************************
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_stag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS   *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_STAG_FIELDS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10 , exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_stag_verify()",0,0);
}

/*********************************************************************
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_stag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS   *info
  )
{
  uint32
    res,
    stag_pro_val = 0;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_info_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_STAG_FIELDS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ihp_port_info_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port_info_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  stag_pro_val = ihp_port_info_tbl_data.statistics_profile;

  info->cid = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(stag_pro_val, 0, 0));
  info->ifp = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(stag_pro_val, 1, 1));
  info->tr_cls = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(stag_pro_val, 2, 2));
  info->dp = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(stag_pro_val, 3, 3));
  info->data_type = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(stag_pro_val, 4, 4));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_stag_get_unsafe()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_ftmh_extension_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FTMH_EXTENSION_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
     ext_option, SOC_PETRA_PORTS_FTMH_NOF_EXT_OUTLIFS,
      SOC_PETRA_FTMH_EXTENSION_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_ftmh_extension_verify()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_pa_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res,
    field_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_FTMH_EXTENSION_SET_UNSAFE);

  regs = soc_petra_regs();

  field_val = (uint32)ext_option;

  SOC_PA_FLD_SET(regs->eci.general_controls_reg.ftmh_ext_mode, field_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_ftmh_extension_set_unsafe()",0,0);
}

uint32
  soc_petra_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FTMH_EXTENSION_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_ftmh_extension_set_unsafe,(unit, ext_option));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_ftmh_extension_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_pa_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res,
    field_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_FTMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ext_option);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->eci.general_controls_reg.ftmh_ext_mode, field_val, 10, exit);
 *ext_option = (SOC_PETRA_PORTS_FTMH_EXT_OUTLIF)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_ftmh_extension_get_unsafe()",0,0);
}

uint32
  soc_petra_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_FTMH_EXTENSION_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_ftmh_extension_get_unsafe,(unit, ext_option));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_ftmh_extension_get_unsafe()",0,0);
}

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_ports_otmh_extension_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
      SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
    )
{
  uint32
    res;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_tbl_data;
  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF
    ftmh_ext;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_OTMH_EXTENSION_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &egq_pct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  egq_pct_tbl_data.otmh_src_ext_ena = SOC_SAND_BOOL2NUM(info->src_ext_en);
  egq_pct_tbl_data.otmh_dest_ext_ena = SOC_SAND_BOOL2NUM(info->dest_ext_en);

  res = soc_petra_ports_ftmh_extension_get_unsafe(
          unit,
          &ftmh_ext
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if ((ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER)&&
    (info->outlif_ext_en != SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR, 30, exit);
  }
  else
  {
    if (
      (ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC)&&
      !((info->outlif_ext_en == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC)||
      (info->outlif_ext_en == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER))
      )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR, 40, exit);
    }
  }


  egq_pct_tbl_data.otmh_lif_ext_ena = (uint32)info->outlif_ext_en;

  res = soc_petra_egq_pct_tbl_set_unsafe(
          unit,
          port_ndx,
          &egq_pct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_otmh_extension_set_unsafe()",0,0);
}
uint32
  soc_petra_ports_otmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_OTMH_EXTENSION_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_otmh_extension_set_unsafe,(unit, port_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_otmh_extension_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ports_otmh_extension_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_OTMH_EXTENSION_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10 , exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_otmh_extension_verify()",0,0);
}

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_ports_otmh_extension_get_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
      SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
    )
{
  uint32
    res;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORTS_OTMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &egq_pct_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->src_ext_en = SOC_SAND_NUM2BOOL(egq_pct_tbl_data.otmh_src_ext_ena);
  info->dest_ext_en = SOC_SAND_NUM2BOOL(egq_pct_tbl_data.otmh_dest_ext_ena);
  info->outlif_ext_en = egq_pct_tbl_data.otmh_lif_ext_ena;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ports_otmh_extension_get_unsafe()",0,0);
}

uint32
  soc_petra_ports_otmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_OTMH_EXTENSION_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ports_otmh_extension_get_unsafe,(unit, port_ndx, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ports_otmh_extension_get_unsafe()",0,0);
}

uint32
  soc_pa_if2mal_id(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID if_id,
    SOC_SAND_OUT uint32          *mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_OFP_MAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mal_ndx);

  *mal_ndx = SOC_PETRA_IF2MAL_NDX(if_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_if2mal_id()",0,0);
}

#ifdef LINK_PB_LIBRARIES
uint32
  soc_pb_if2mal_id(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID if_id,
    SOC_SAND_OUT uint32          *mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_OFP_MAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(mal_ndx);

  *mal_ndx = SOC_PB_IF2MAL_NDX(if_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error soc_pb_if2mal_id soc_pa_if2mal_id()",0,0);
}
#endif

uint32
  soc_petra_if2mal_id(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID if_id,
    SOC_SAND_OUT uint32          *mal_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_OFP_MAL_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(if2mal_id,(unit, if_id, mal_ndx));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_if2mal_id()",if_id,0);
}


/*
 *  Get the MAL index or equivalent of the MAL
 *  to which the given port is mapped at the egress
 */
uint32
  soc_petra_port_ofp_mal_get_unsafe(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID ofp_ndx,
    SOC_SAND_OUT uint32         *mal_ndx
  )
{
  uint32
    res;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    mapping,
    in_mapping;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_OFP_MAL_GET_UNSAFE);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&mapping);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&in_mapping);

  SOC_SAND_CHECK_NULL_INPUT(mal_ndx);

  /*
   *  Get the MAL index of the requested port
   */
  res = soc_petra_port_to_interface_map_get_unsafe(
          unit,
          ofp_ndx,
          &in_mapping,
          &mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_if2mal_id(
          unit,
          mapping.if_id,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_ofp_mal_get_unsafe()",ofp_ndx,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_REG_FIELD
    *uc_fld = NULL;
  SOC_PETRA_REG_FIELD
    *mc_fld = NULL;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(port_hdr_type_ndx)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_raw);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_raw);
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_cpu);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_cpu );
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_eth);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_eth);
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_TM:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_tm);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_tm);
    break;
  default:
    break;
  }

  value = 0;
  value |= SOC_SAND_SET_FLD_IN_PLACE((uint32)soc_sand_abs(info->uc_credit_discount), 0, SOC_SAND_BITS_MASK(6, 0));
  value |= SOC_SAND_SET_FLD_IN_PLACE(info->uc_credit_discount < 0 ? 1 : 0, 7, SOC_SAND_BITS_MASK(7, 7));
  SOC_PETRA_IMPLICIT_FLD_SET(*uc_fld, value, 10, exit);

  value = 0;
  value |= SOC_SAND_SET_FLD_IN_PLACE((uint32)soc_sand_abs(info->mc_credit_discount), 0, SOC_SAND_BITS_MASK(6, 0));
  value |= SOC_SAND_SET_FLD_IN_PLACE(info->mc_credit_discount < 0 ? 1 : 0, 7, SOC_SAND_BITS_MASK(7, 7));
  SOC_PETRA_IMPLICIT_FLD_SET(*mc_fld, value, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_type_set_unsafe()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_hdr_type_ndx, SOC_PETRA_PORT_NOF_HEADER_TYPES-1,
    SOC_PETRA_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type_ndx, SOC_PETRA_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_type_verify()",0,0);
}

/*********************************************************************
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_REG_FIELD
    *uc_fld = NULL;
  SOC_PETRA_REG_FIELD
    *mc_fld = NULL;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_TYPE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_hdr_type_ndx, SOC_PETRA_PORT_NOF_HEADER_TYPES-1,
    SOC_PETRA_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type_ndx, SOC_PETRA_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

  switch(port_hdr_type_ndx)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_raw);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_raw);
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_cpu);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_cpu );
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_eth);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_eth);
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_TM:
    uc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.unicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_uc_tm);
    mc_fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.multicast_egress_packet_header_compensation_reg[cr_discnt_type_ndx].hdr_adjust_mc_tm);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    break;
  }

  SOC_PETRA_IMPLICIT_FLD_GET(*uc_fld, value, 10, exit);
  info->uc_credit_discount = SOC_SAND_GET_FLD_FROM_PLACE(value,  0, SOC_SAND_BITS_MASK(6, 0));
  info->uc_credit_discount *= SOC_SAND_GET_FLD_FROM_PLACE(value, 7, SOC_SAND_BITS_MASK(7, 7)) ? -1 : 1;

  SOC_PETRA_IMPLICIT_FLD_GET(*mc_fld, value, 20, exit);
  info->mc_credit_discount = 0;
  info->mc_credit_discount = SOC_SAND_GET_FLD_FROM_PLACE(value,  0, SOC_SAND_BITS_MASK(6, 0));
  info->mc_credit_discount *= SOC_SAND_GET_FLD_FROM_PLACE(value, 7, SOC_SAND_BITS_MASK(7, 7)) ? -1 : 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_type_get_unsafe()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_port_egr_hdr_credit_discount_select_set_unsafe(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  uint32                              port_ndx,
      SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE    cr_discnt_type
    )
{
  uint32
    res;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE);

  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  data.header_compensation_type = cr_discnt_type;

  res = soc_petra_egq_pct_tbl_set_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_egr_hdr_credit_discount_select_set_unsafe()",0,0);
}


uint32
  soc_petra_port_egr_hdr_credit_discount_select_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE    cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(port_egr_hdr_credit_discount_select_set_unsafe,(unit, port_ndx, cr_discnt_type));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_select_set_unsafe()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type, SOC_PETRA_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_select_verify()",0,0);
}

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_port_egr_hdr_credit_discount_select_get_unsafe(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  uint32                              port_ndx,
      SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
    )
{
  uint32
    res;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *cr_discnt_type = data.header_compensation_type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_port_egr_hdr_credit_discount_select_get_unsafe()",0,0);
}


uint32
  soc_petra_port_egr_hdr_credit_discount_select_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

  SOC_PETRA_DIFF_DEVICE_CALL(port_egr_hdr_credit_discount_select_get_unsafe,(unit, port_ndx, cr_discnt_type));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_port_egr_hdr_credit_discount_select_get_unsafe()",0,0);
}


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

