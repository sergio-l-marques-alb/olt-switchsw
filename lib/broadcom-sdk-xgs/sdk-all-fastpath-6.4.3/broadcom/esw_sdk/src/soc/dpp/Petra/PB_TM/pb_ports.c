/* $Id: pb_ports.c,v 1.28 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_parser.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>

#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_tdm.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_action_cmd.h>

#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_api_cnt.h>

#include <soc/dpp/Petra/petra_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PORT_NDX_MAX                                        (SOC_PETRA_NOF_LOCAL_PORTS-1)
#define SOC_PB_PORTS_FIRST_HEADER_SIZE_MAX                         (63)
#define SOC_PB_PORT_PP_PORT_INFO_FC_TYPE_MAX                       (SOC_PB_PORTS_NOF_FC_TYPES-1)
#define SOC_PB_PORTS_PROCESSOR_ID_MAX                              (SOC_PB_CNT_NOF_PROCESSOR_IDS-1)
#define SOC_PB_PORTS_CNT_ID_MAX                                    (8*1024-1)
/* $Id: pb_ports.c,v 1.28 Broadcom SDK $
*  This is just to verify we don't get stuck in an infinite loop in case of error condition
*/
#define SOC_PB_PORTS_IF_ITERATIONS_MAX (SOC_PB_NOF_MAC_LANES * SOC_PETRA_NOF_IF_CHANNELS_MAX + 10)

#define SOC_PB_PORTS_FEM_ACTION_ID_DFLT                            (0)
#define SOC_PB_PORTS_FEM_ACTION_ID_SRC_PORT_EXT                    (1)
#define SOC_PB_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED        (1)

#define SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR                       (0)
#define SOC_PB_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED          (1)
#define SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_EXT                      (1)
#define SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_INJECTED                 (2)

#define SOC_PB_PORT_ITMH_SIZE_IN_BYTES                             (4)

#define SOC_PB_PORTS_FEM_BIT_SELECT_MSB_DFLT                       (3)
#define SOC_PB_PORTS_FEM_BIT_SELECT_MSB_PP_PORT_PROFILE_INJECTED   (31)


/*
 *	EGQ TXi thresholds
 */
/* XAUI/RXAUI (SPAUI/RSPAUI) */
#define SOC_PB_EGQ_TXI_TH_XAUI_LIKE     10
#define SOC_PB_EGQ_TXI_TH_ID_XAUI_LIKE  0

/* GMII */
#define SOC_PB_EGQ_TXI_TH_GMII          5
#define SOC_PB_EGQ_TXI_TH_ID_GMII       1

/* Interlaken */
#define SOC_PB_EGQ_TXI_TH_ILKN          65
#define SOC_PB_EGQ_TXI_TH_ID_ILKN       3

/* CPU */
#define SOC_PB_EGQ_TXI_TH_CPU           4
#define SOC_PB_EGQ_TXI_TH_ID_CPU        4

/* OLP */
#define SOC_PB_EGQ_TXI_TH_OLP           4
#define SOC_PB_EGQ_TXI_TH_ID_OLP        5

/* Recycling */
#define SOC_PB_EGQ_TXI_TH_RCY           29
#define SOC_PB_EGQ_TXI_TH_ID_RCY        6

/* General */
#define SOC_PB_EGQ_TXI_TH_GEN           5

#define SOC_PB_EGQ_TXI_SEL_REG_FLD_SIZE_BITS    3

#define SOC_PB_PP_OUT_MIRR_VID_DONT_CARE        7
#define SOC_PB_PP_OUT_MIRR_VID_SHIFT            6

#define SOC_PB_PORT_PP_PORT_SET_IHP_LL_MIRROR_PROFILE_DEFAULT_VID 7
#define SOC_PB_PORT_PP_PORT_SET_IHP_LL_MIRROR_PROFILE_INVALID (0)

#define SOC_PB_PORT_LAG_LB_RNG_SIZE  (256)

#define SOC_PB_PORTS_PORT_NDX_MAX                                  (79)

/*     Maximal number of channels in an Interlaken interface.  */
#define  SOC_PB_NOF_ILKN_IF_CHANNELS_MAX 80

/* Dedicated ports for ILKN interfaces */
#define SOC_PETRA_NIF_ID_ILKN_0_DEDICATED_PORT 1
#define SOC_PETRA_NIF_ID_ILKN_1_DEDICATED_PORT 2

#define SOC_PB_PORTS_FORWARDING_TDM_RAW_OFFSET_BASE                 (((SOC_SAND_GET_BITS_RANGE(SOC_PB_PARSER_PFC_RAW, 5, 4)) << 8) + (1 << 7))

/* 
 * Internal indication of an unspecified (unknown) channel  
 */
#define SOC_PB_PORTS_CH_UNKNOWN 0xffff

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

uint32
  soc_pb_ports_init(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fem_bit_ndx,
    fem_map_ndx,
    fem_pgm_dflt;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA
    pp_context_fem_bit_select_table;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA
    pp_context_fem_map_index_table;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA
    pp_context_fem_field_select_map;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
    src_system_port_fem_bit_select_table;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
    src_system_port_fem_map_index_table;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
    src_system_port_fem_field_select_map;
  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
    pp_port_fem_bit_select_table;
  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
    pp_port_fem_map_index_table;
  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
    pp_port_fem_field_select_map;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_INIT);

  regs = soc_petra_regs();

 /*
  * Set ID of olp port
  */
  SOC_PB_FLD_SET(regs->ire.olp_fap_port_configuration_reg.olp_fap_port, SOC_PETRA_OLP_PORT_ID, 1, exit);

 /*
  * Set ID of CPU port (For synchronize interface)
  */
  SOC_PB_FLD_SET(regs->ire.fap_port_configuration_reg.reg_fap_port, SOC_PETRA_FRST_CPU_PORT_ID, 2, exit);


  /*
   *	Set the PP context / System Port / PP Port FEMs for the default FEM (direct extraction)
   */
  fem_pgm_dflt = SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR;

  pp_context_fem_bit_select_table.pp_context_fem_bit_select = SOC_PB_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &pp_context_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = SOC_PB_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  pp_port_fem_bit_select_table.pp_port_fem_bit_select = SOC_PB_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &pp_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  pp_context_fem_map_index_table.pp_context_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_DFLT;
  pp_context_fem_map_index_table.pp_context_fem_map_data = 0;
  src_system_port_fem_map_index_table.src_system_port_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_DFLT;
  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  pp_port_fem_map_index_table.pp_port_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_DFLT;
  pp_port_fem_map_index_table.pp_port_fem_map_data = 0;
  for (fem_map_ndx = 0; fem_map_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    /*
     *	For the FEM-Program 0, the entry offset is the 4b value index
     */
    res = soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe(
            unit,
            SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &pp_context_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
    res = soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    res = soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &pp_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
  }

  /*
   *	Set the direct extraction
   */
  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_PB_PORTS_FEM_PP_CONTEXT_SIZE; ++fem_bit_ndx)
  {
    pp_context_fem_field_select_map.pp_context_fem_field_select[fem_bit_ndx] = fem_bit_ndx;
  }
  res = soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe(
          unit,
          SOC_PB_PORTS_FEM_ACTION_ID_DFLT,
          &pp_context_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE; ++fem_bit_ndx)
  {
    src_system_port_fem_field_select_map.src_system_port_fem_field_select[fem_bit_ndx] = fem_bit_ndx;
  }
  res = soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          SOC_PB_PORTS_FEM_ACTION_ID_DFLT,
          &src_system_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_PB_PORTS_FEM_PP_PORT_SIZE; ++fem_bit_ndx)
  {
    pp_port_fem_field_select_map.pp_port_fem_field_select[fem_bit_ndx] = fem_bit_ndx;
  }
  res = soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          SOC_PB_PORTS_FEM_ACTION_ID_DFLT,
          &pp_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  /*
   *	FEM for the Source-System-Port extension
   */
  fem_pgm_dflt = SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_EXT;

  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = 31;
  res = soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  for (fem_map_ndx = 0; fem_map_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    if (fem_map_ndx & 0x2)
    {
      src_system_port_fem_map_index_table.src_system_port_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_SRC_PORT_EXT;
    }
    else
    {
      src_system_port_fem_map_index_table.src_system_port_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_DFLT;
    }

    res = soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_EXT,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
  }

  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE; ++fem_bit_ndx)
  {
    src_system_port_fem_field_select_map.src_system_port_fem_field_select[fem_bit_ndx] = 16 + fem_bit_ndx;
  }
  res = soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          SOC_PB_PORTS_FEM_ACTION_ID_SRC_PORT_EXT,
          &src_system_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);

  /*
   * FEM for the Injected PP-Context: PFQ0 = 0,
   * PP-Context depends on PTCH[23:22]
   */
  fem_pgm_dflt = SOC_PB_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED;

  pp_context_fem_bit_select_table.pp_context_fem_bit_select = SOC_PB_PORTS_FEM_BIT_SELECT_MSB_PP_PORT_PROFILE_INJECTED;
  res = soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &pp_context_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  pp_context_fem_map_index_table.pp_context_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED;
  for (fem_map_ndx = 0; fem_map_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    if (((fem_map_ndx >> 2) == 0) || ((fem_map_ndx >> 2) == 1))
    {
      /* PTCH - ITMH */
      pp_context_fem_map_index_table.pp_context_fem_map_data = SOC_PB_PARSER_PP_CONTEXT_ITMH;
    }
    else if ((fem_map_ndx >> 2) == 2)
    {
      /* PTCH - Eth no In-LIF */
      pp_context_fem_map_index_table.pp_context_fem_map_data = SOC_PB_PARSER_PP_CONTEXT_ETH;
    }
    else
    {
      /* PTCH - Eth with In-LIF */
      pp_context_fem_map_index_table.pp_context_fem_map_data = SOC_PB_PARSER_PP_CONTEXT_INJECTED_EXT_KEY;
    }
    /*
     *	For the FEM-Program 1, the entry offset is the 4b value index
     */
    res = soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            fem_map_ndx,
            &pp_context_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);
  }

  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_PB_PORTS_FEM_PP_CONTEXT_SIZE; ++fem_bit_ndx)
  {
    /* PP-Context = FEM-Map-Data, PFQ-0 = PTCH[21:19] */
    if (fem_bit_ndx < SOC_PB_PP_PORT_PP_CONTEXT_QUALIFIER_LSB)
    {
      pp_context_fem_field_select_map.pp_context_fem_field_select[fem_bit_ndx] = 0; /* PFQ 0*/
    }
    else
    {
      pp_context_fem_field_select_map.pp_context_fem_field_select[fem_bit_ndx] = fem_bit_ndx - SOC_PB_PP_PORT_PP_CONTEXT_QUALIFIER_LSB; /* PP-Context */
      SOC_SAND_SET_BIT(pp_context_fem_field_select_map.pp_context_fem_field_select[fem_bit_ndx], 0x1, SOC_PB_PMF_FEM_MAP_DATA_ENCODED_BIT);
    }
  }
  res = soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe(
          unit,
          SOC_PB_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED,
          &pp_context_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);

  /* Set the Profile 2 to go to action 1: Source system port = PTCH[29:16] */
  fem_pgm_dflt = SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_INJECTED;

  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = 31;
  res = soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  src_system_port_fem_map_index_table.src_system_port_fem_map_index = SOC_PB_PORTS_FEM_ACTION_ID_SRC_PORT_EXT;
  for (fem_map_ndx = 0; fem_map_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    res = soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
  }

  /*
   *	EGQ TXi thresholds
   */
  SOC_PB_FLD_SET(regs->egq.nrdy_th0_2_reg.nrdy_th0, SOC_PB_EGQ_TXI_TH_XAUI_LIKE, 100, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th0_2_reg.nrdy_th1, SOC_PB_EGQ_TXI_TH_GMII, 102, exit);
  /* Currently same as prvious */
  SOC_PB_FLD_SET(regs->egq.nrdy_th0_2_reg.nrdy_th2, SOC_PB_EGQ_TXI_TH_GMII, 104, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th3_5_reg.nrdy_th3, SOC_PB_EGQ_TXI_TH_ILKN, 106, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th3_5_reg.nrdy_th4, SOC_PB_EGQ_TXI_TH_CPU, 108, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th3_5_reg.nrdy_th5, SOC_PB_EGQ_TXI_TH_OLP, 110, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th6_7_reg.nrdy_th6, SOC_PB_EGQ_TXI_TH_RCY, 112, exit);
  SOC_PB_FLD_SET(regs->egq.nrdy_th6_7_reg.nrdy_th7, SOC_PB_EGQ_TXI_TH_GEN, 114, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ports_init()", 0, 0);
}

uint32 
  soc_pb_ports_expected_chan_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_NIF_TYPE      nif_type,
    SOC_SAND_IN  uint8           is_channelized_interface,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID  nif_id,
    SOC_SAND_IN  uint32           if_id_internal,
    SOC_SAND_OUT uint32           *channel,
    SOC_SAND_OUT uint32           *channel_step
  )
{
  uint32
    ch_id = SOC_PB_PORTS_CH_UNKNOWN,
    ch_step = 1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PORTS_EXPECTED_CHAN_GET);

  switch(nif_type)
  {
  case SOC_PB_NIF_TYPE_RXAUI:
  case SOC_PB_NIF_TYPE_XAUI:
  case SOC_PB_NIF_TYPE_ILKN:
    if (is_channelized_interface)
    {
      ch_id = SOC_PB_PORTS_CH_UNKNOWN;
    }
    else
    {
      ch_id = 0;
    }
    break;
  case SOC_PB_NIF_TYPE_QSGMII:
  case SOC_PB_NIF_TYPE_SGMII:
    if(if_id_internal == SOC_PB_NIF_INVALID_VAL_INTERN)
    {
      ch_id = SOC_PB_PORTS_CH_UNKNOWN;
    }
    else
    {
      /* QSGMII are using "ch_id" 0,16,32,48, SGMII are using 0,32 */
      ch_id = (SOC_PETRA_NIF_SGMII_LANE(if_id_internal) * (SOC_PETRA_NOF_IF_CHANNELS_MAX / SOC_PETRA_MAX_NIFS_PER_MAL));
    }
    ch_step = (nif_type == SOC_PB_NIF_TYPE_QSGMII)?16:32;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit);
    break;
  }

  *channel = ch_id;
  *channel_step = ch_step;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_expected_chan_get()",0,0);
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
  soc_pb_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;
  uint32
    sys_fap_id_self;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_port2sys_data;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    ips_lookup_data;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

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
     *  in the EGQ and IHP
     */
    /* EGQ */
    res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
            unit,
            mapped_fap_port_id,
            &egq_ppct_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    egq_ppct_data.dst_system_port_id = sys_phys_port_ndx;

    res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
            unit,
            mapped_fap_port_id,
            &egq_ppct_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* IHP */
    res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
            unit,
            mapped_fap_port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    ihp_port2sys_data.system_port_value = sys_phys_port_ndx;
    ihp_port2sys_data.system_port_value_to_use = 0x1;

    res = soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
            unit,
            mapped_fap_port_id,
            &ihp_port2sys_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* store in SW DB */
    res = soc_pb_sw_db_local_to_sys_port_set(
            unit,
            mapped_fap_port_id,
            sys_phys_port_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sys_phys_to_local_port_map_set_unsafe()",0,0);
}


uint32
  soc_pb_local_to_sys_phys_port_map_itself_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

  res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
          unit,
          fap_local_port_ndx,
          &egq_ppct_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *sys_phys_port_id = egq_ppct_data.dst_system_port_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_local_to_sys_phys_port_map_itself_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure the Port profile for ports of type TM and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_pp_port_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    ihp_ll_mirror_profile_tbl_idx,
    res = SOC_SAND_OK;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info_previous;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH
    pp_port_strength;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ihp_ll_mirror_profile_tbl;
  SOC_PB_PMF_PGM_MGMT_NDX
    pgm_ndx;
  SOC_PB_PMF_PGM_MGMT_PARAMS
    *pgm_params = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_port_pp_port_get_unsafe(
          unit,
          pp_port_ndx,
          &pp_port_info_previous
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	1. Check if there is already a relevant PMF Program, otherwise
   *     check if there is enough place for a new one
   */
  SOC_PETRA_ALLOC(pgm_params, SOC_PB_PMF_PGM_MGMT_PARAMS, 1);
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(pgm_params);
  SOC_PETRA_COPY(&(pgm_params->pp_port), info, SOC_PB_PORT_PP_PORT_INFO, 1);

  /*
   *  Insert a new element: PP-Port
   */
  SOC_PB_PMF_PGM_MGMT_NDX_clear(&pgm_ndx);
  pgm_ndx.pp_port_ndx = pp_port_ndx;

  res = soc_pb_pmf_pgm_mgmt_insert_unsafe(
          unit,
          SOC_PB_PMF_PGM_MGMT_SOURCE_PP_PORT,
          &pgm_ndx,
          pgm_params,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  if (*success != SOC_SAND_SUCCESS)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   *	Do not parse the first header if exists
   */
  res = soc_pb_parser_nof_bytes_to_remove_set(
          unit,
          pp_port_ndx,
          info->first_header_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * 2. Save / Restore the previous strength if the current / previous
   *    header type is Raw
   */
  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (
      (info->header_type == SOC_PETRA_PORT_HEADER_TYPE_RAW)
     )
  {
    /*
     *	Get the trap strength configuration
     */
    pp_port_strength.trap_code = pinfo_llr_tbl.default_cpu_trap_code;
    pp_port_strength.strength[0] = pinfo_llr_tbl.default_action_profile_fwd;
    pp_port_strength.strength[1] = pinfo_llr_tbl.default_action_profile_snp;

    /*
     *	Save it
     */
    res = soc_pb_sw_db_lbg_profile_strength_stat_save(
            unit,
            pp_port_ndx,
            &pp_port_strength
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    pinfo_llr_tbl.default_action_profile_fwd = 0x7;
    pinfo_llr_tbl.default_action_profile_snp = 0x3;
  }
  else if (
        (info->header_type != SOC_PETRA_PORT_HEADER_TYPE_RAW)
        && (pp_port_info_previous.header_type == SOC_PETRA_PORT_HEADER_TYPE_RAW)
        )
  {
    /*
     *	Restore the previous configuration to the actual PP-Port
     */
    res = soc_pb_sw_db_lbg_profile_strength_stat_load(
            unit,
            pp_port_ndx,
            &pp_port_strength
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    pinfo_llr_tbl.default_cpu_trap_code = pp_port_strength.trap_code;
    pinfo_llr_tbl.default_action_profile_fwd = pp_port_strength.strength[0];
    pinfo_llr_tbl.default_action_profile_snp = pp_port_strength.strength[1];
  }

  if ((info->header_type != SOC_PETRA_PORT_HEADER_TYPE_RAW))
  {
    if (info->is_snoop_enabled == TRUE)
    {
      pinfo_llr_tbl.default_action_profile_snp = 0x1;
    }
    else /* The default port snoop value is more important */
    {
      if ((info->header_type != SOC_PETRA_PORT_HEADER_TYPE_ETH)){
          pinfo_llr_tbl.default_action_profile_snp = 0x2;
      }
      else{
          pinfo_llr_tbl.default_action_profile_snp = 0x0;
      }
    }
    pinfo_llr_tbl.default_cpu_trap_code = 0;
  }

  res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  /*
   *  Inbound mirroring: Relevant Only for non Ethernet header.
   *  Set per VLAN * PP Port, VLAN don't care for TM, mirror action command index as attribute of PP Port in TM API
   *  For Ethernet header, use API soc_pb_pp_llp_mirror_port_dflt_set.
   */

  if ((info->header_type != SOC_PETRA_PORT_HEADER_TYPE_ETH))
  {
    /* Untagged */
    ihp_ll_mirror_profile_tbl_idx = SOC_SAND_SET_BITS_RANGE(pp_port_ndx,5,0);
    ihp_ll_mirror_profile_tbl_idx |= SOC_SAND_SET_BITS_RANGE(SOC_PB_PORT_PP_PORT_SET_IHP_LL_MIRROR_PROFILE_DEFAULT_VID,8,6);
    ihp_ll_mirror_profile_tbl.ll_mirror_profile = info->mirror_profile;

    res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
      unit,
      ihp_ll_mirror_profile_tbl_idx,
      &ihp_ll_mirror_profile_tbl
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }
  
  soc_pb_sw_db_pp_port_header_type_set(
    unit,
    pp_port_ndx,
    info->header_type
   );


  *success = SOC_SAND_SUCCESS;

exit:
  SOC_PETRA_FREE(pgm_params);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_set_unsafe()", pp_port_ndx, 0);
}

uint32
  soc_pb_port_pp_port_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, SOC_PB_PP_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PORT_PP_PORT_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_set_verify()", pp_port_ndx, 0);
}

/*********************************************************************
*     Get the Port profile settings.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_pp_port_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO        *info
  )
{
  uint32
    ihp_ll_mirror_profile_tbl_idx,
    pmf_pgm,
    res = SOC_SAND_OK;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
    ihp_ll_mirror_profile_tbl;
  uint32
    pfg_ndx;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PORT_PP_PORT_INFO_clear(info);

  /*
   * 1. Get the PMF-related PP Port attributes
   * All the possible Header types must be looked up to find the right PP info
   */
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  profile_ndx.pmf_pgm_mgmt_ndx.pp_port_ndx = pp_port_ndx;

  header_type = soc_pb_sw_db_pp_port_header_type_get(
                  unit,
                  pp_port_ndx
                );
  switch (header_type)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_TM:
  case SOC_PETRA_PORT_HEADER_TYPE_PROG:
  case SOC_PETRA_PORT_HEADER_TYPE_INJECTED:
    pfg_ndx = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
    profile_ndx.pmf_pgm_ndx = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(TRUE, TRUE);
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM:
    pfg_ndx = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING;
    break;
  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
  default:
    pfg_ndx = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;
    break;
  }
  profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx = pfg_ndx;
  res = soc_pb_profile_get(
          unit,
          SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
          &profile_ndx,
          &profile_params,
          &pmf_pgm
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_COPY(info, &(profile_params.pmf_pgm_mgmt_params.pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
  /* Override the header type */
  info->header_type = header_type;

  /*
   *  2. Get the Snoop and Mirror attributes
   */
  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if ((pinfo_llr_tbl.default_action_profile_snp == 0x1) && (pinfo_llr_tbl.default_cpu_trap_code == 0))
  {
    info->is_snoop_enabled = TRUE;
  }
  else
  {
    info->is_snoop_enabled = FALSE;
  }

  ihp_ll_mirror_profile_tbl_idx = SOC_SAND_SET_BITS_RANGE(pp_port_ndx,5,0);
  ihp_ll_mirror_profile_tbl_idx |= SOC_SAND_SET_BITS_RANGE(SOC_PB_PORT_PP_PORT_SET_IHP_LL_MIRROR_PROFILE_DEFAULT_VID,8,6);
  res = soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
          unit,
          ihp_ll_mirror_profile_tbl_idx,
          &ihp_ll_mirror_profile_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  info->mirror_profile = ihp_ll_mirror_profile_tbl.ll_mirror_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_get_unsafe()", pp_port_ndx, 0);
}

uint32
  soc_pb_port_pp_port_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, SOC_PB_PP_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_get_verify()", pp_port_ndx, 0);
}

/*********************************************************************
*     Map the Port to its Port profile for ports of type TM
 *     and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION           direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    tbl_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_SET_UNSAFE);

  if (
      (direction_ndx == SOC_PETRA_PORT_DIRECTION_INCOMING)
      || (direction_ndx == SOC_PETRA_PORT_DIRECTION_BOTH)
     )
  {
    res = soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(
            unit,
            port_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    tbl_data.pp_port_offset1 = pp_port;
    tbl_data.pp_port_offset2 = 2; /* No influence in general (except ad) */
    tbl_data.pp_port_profile = SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR;
    tbl_data.pp_port_use_offset_directly = 0x1;

    res = soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(
            unit,
            port_ndx,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (
      (direction_ndx == SOC_PETRA_PORT_DIRECTION_OUTGOING)
      || (direction_ndx == SOC_PETRA_PORT_DIRECTION_BOTH)
     )
  {
    res = soc_pb_egq_pct_tbl_get_unsafe(unit, port_ndx, &pct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_egq_ppct_tbl_get_unsafe(unit, port_ndx, &egq_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

    pct_tbl.out_pp_port = pp_port;
    egq_ppct_tbl.out_pp_port = pp_port;

    res = soc_pb_egq_pct_tbl_set_unsafe(unit, port_ndx, &pct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_pb_pp_egq_ppct_tbl_set_unsafe(unit, port_ndx, &egq_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_port_to_pp_port_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION           direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_outgoing,
    header_type_incoming;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_PORT_DIRECTION_NDX_MAX, SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port, SOC_PB_PP_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 12, exit);

  /*
   *	Verify the header type is identical at the ingress
   */
  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &header_type_incoming,
          &header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


  SOC_PB_PORT_PP_PORT_INFO_clear(&pp_port_info);
  res = soc_pb_port_pp_port_get_unsafe(
          unit,
          pp_port,
          &pp_port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Verify the Header types are identical except the injected ports (PP-Port header returns then TM) */
  if (header_type_incoming != SOC_PETRA_PORT_HEADER_TYPE_INJECTED)
  {
    if (
        (pp_port_info.header_type != header_type_incoming)
         && ((header_type_incoming != SOC_PETRA_PORT_HEADER_TYPE_RAW) && (pp_port_info.header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW))
        )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_PROFILE_IS_HEADER_TYPE_INCONSISTENT_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_port_to_pp_port_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Map the Port to its Port profile for ports of type TM
 *     and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    tbl_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_GET_UNSAFE);

  res = soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(
          unit,
          port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  *pp_port_in = tbl_data.pp_port_offset1;

  /*
   *	Outgoing
   */
  res = soc_pb_egq_pct_tbl_get_unsafe(unit, port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  *pp_port_out = pct_tbl.out_pp_port;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_get_unsafe()", port_ndx, 0);
}


uint32
  soc_pb_ports_set_in_src_sys_port_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                                 tm_port_id,
    SOC_SAND_IN  uint32                                sys_port
  )
{
  uint32
    res;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    sys_port_config_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_SET_IN_SRC_SYS_PORT_SET_UNSAFE);

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
          unit,
          tm_port_id,
          &sys_port_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  sys_port_config_tbl_data.system_port_value = sys_port;

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
          unit,
          tm_port_id,
          &sys_port_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_set_in_src_sys_port_set_unsafe()",tm_port_id,sys_port);

}


uint32
  soc_pb_ports_set_in_src_sys_port_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                                 tm_port_id,
    SOC_SAND_OUT  uint32                               *sys_port
  )
{
  uint32
    res;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    sys_port_config_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_SET_IN_SRC_SYS_PORT_GET_UNSAFE);

  *sys_port = 0;

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
          unit,
          tm_port_id,
          &sys_port_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *sys_port = sys_port_config_tbl_data.system_port_value;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_set_in_src_sys_port_set_unsafe()",tm_port_id,*sys_port);

}

uint32
  soc_pb_ports_lag_members_ranges_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  uint32                                 lag_size,
    SOC_SAND_IN  uint32                                 sys_port,
    SOC_SAND_IN  uint32                                 remove
  )
{
  uint32
    res;
  uint32
    fap_id,
    port_id = 0,
    lag_info_idx,
    sys_port_curr,
    member_id,
    sys_fap_id_self;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_pp_ppct_data;
  uint32
    lag_rng_size=0,
    remain=0,
    curr_rng=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (lag_size)
  {
    lag_rng_size = SOC_PB_PORT_LAG_LB_RNG_SIZE/lag_size;
    remain = SOC_PB_PORT_LAG_LB_RNG_SIZE - lag_rng_size *lag_size;
    for (lag_info_idx = 0; lag_info_idx < lag_size; lag_info_idx++)
    {
      member_id = (lag_size -lag_info_idx)-1;
      res = soc_pb_irr_lag_mapping_tbl_get_unsafe(
              unit,
              lag_ndx,
              member_id,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      sys_port_curr = irr_glag_mapping_tbl_data.destination;

      res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
              unit,
              sys_port_curr,
              &fap_id,
              &port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (fap_id == sys_fap_id_self)
      {
        res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        /* update PPCT */
        egq_pp_ppct_data.lb_key_min = curr_rng;
        egq_pp_ppct_data.lb_key_max = curr_rng + (lag_rng_size-1);
        /* this is for remain to divide it between ports */
        if (remain)
        {
          egq_pp_ppct_data.lb_key_max += 1;
          --remain;
        }
        curr_rng = egq_pp_ppct_data.lb_key_max + 1;
        res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
      else/* save room for none-local ports */
      {
        curr_rng += (lag_rng_size);
        if (remain)
        {
          curr_rng += 1;
          --remain;
        }
      }
    }
  }
  
  if (remove)
  {
    res = soc_petra_sys_phys_to_local_port_map_get_unsafe(
            unit,
            sys_port,
            &fap_id,
            &port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (fap_id == sys_fap_id_self)
    {
      /* remove configuration of removed port */
       res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        /* update PPCT */
        egq_pp_ppct_data.lb_key_min = 1;
        egq_pp_ppct_data.lb_key_max = 0;
        res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_members_ranges_set_unsafe()",lag_ndx,0);
}

/*********************************************************************
*     Add a system port as a member in LAG.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_lag_member_add_unsafe(
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
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_pp_ppct_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  uint8
    in_enable,
    out_enable,
    in_use;
  uint32
    res;
  SOC_PETRA_PORTS_LAG_INFO
    in_lag,
    out_lag;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_MEMBER_ADD_UNSAFE);

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

  res = soc_pb_ports_lag_get_unsafe(
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
    SOC_PB_DO_NOTHING_AND_EXIT;
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
    irr_glag_mapping_tbl_data.destination = lag_member->sys_port;
    /*
     *  This mapping affects the LAG hushing.
     *  All LAG must be set sequential here,
     *  the member-id is just a running index,
     *  not meaningful otherwise
     */
    res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
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
      res = soc_pb_egq_pct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_pct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_pp_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = soc_petra_ports_logical_sys_id_build(
              TRUE,
              lag_ndx,
              lag_member->member_id,
              dummy_sys_phys_id,
              &sys_logic_port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      egq_pp_ppct_data.dst_system_port_id = sys_logic_port_id;
      egq_pct_data.dest_port = sys_logic_port_id;

      res = soc_pb_egq_pct_tbl_set_unsafe(
              unit,
              port_id,
              &egq_pct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
              unit,
              port_id,
              &egq_pp_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
   /*
    * update size
    */
    res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


    irr_glag_to_lag_range_tbl_data.range = out_lag.nof_entries; /* hw will add 1*/

    res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
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

    res = soc_pb_ports_lag_members_ranges_set_unsafe(
            unit,
            lag_ndx,
            irr_glag_to_lag_range_tbl_data.range+1,
            lag_member->sys_port,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

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

    res = soc_pb_ports_set_in_src_sys_port_set_unsafe(
            unit,
            port_id,
            sys_logic_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_member_add_unsafe()",lag_ndx,0);
}

/*********************************************************************
*     Remove Physical System port to be  member of a a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_lag_sys_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER           *lag_member
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
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_pp_ppct_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  uint32
    removed_member_index=0,
    move_member_index,
    org_sys_port;
  SOC_PETRA_PORTS_LAG_INFO
    in_lag,
    out_lag;
  uint8
    found=FALSE,
    in_use,
    in_enable,
    out_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_SYS_PORT_REMOVE_UNSAFE);

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
    if (fap_id == sys_fap_id_self)
    {
      /* restore mapping, use SW DB to get the origin phy-port*/
      res = soc_pb_sw_db_local_to_sys_port_get(
              unit,
              port_id,
              &org_sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      res = soc_pb_ports_set_in_src_sys_port_set_unsafe(
              unit,
              port_id,
              org_sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_pb_egq_pct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_pct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              port_id,
              &egq_pp_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      egq_pp_ppct_data.dst_system_port_id = org_sys_port;
      egq_pct_data.dest_port = org_sys_port;

      res = soc_pb_egq_pct_tbl_set_unsafe(
              unit,
              port_id,
              &egq_pct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

      res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
              unit,
              port_id,
              &egq_pp_ppct_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    }
  }

  if (out_enable == TRUE)
  {
    res = soc_pb_ports_lag_get_unsafe(
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
      SOC_PB_DO_NOTHING_AND_EXIT;
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
      res = soc_pb_irr_lag_mapping_tbl_get_unsafe(
              unit,
              lag_ndx,
              move_member_index,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
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

    res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /* if was last member */
    if (out_lag.nof_entries <= 1)/*cannot be zero as removed member was found */
    {
      in_use = FALSE;
      irr_glag_to_lag_range_tbl_data.range = 0;
    }
    else /* >= 2, still members in LAG*/
    {
      in_use = TRUE;
      irr_glag_to_lag_range_tbl_data.range = out_lag.nof_entries - 2; /* removed & hw add 1 */
    }

    res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
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
    irr_glag_mapping_tbl_data.destination = SOC_PETRA_SYS_PHYS_PORT_INVALID;
    res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
            unit,
            lag_ndx,
            move_member_index,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = soc_pb_ports_lag_members_ranges_set_unsafe(
            unit,
            lag_ndx,
            (in_use)?irr_glag_to_lag_range_tbl_data.range+1:0,
            sys_port,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_sys_port_remove_unsafe()",lag_ndx,sys_port);
}


/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_lag_set_unsafe(
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
    lag_rng_size=0,
    remain=0,
    curr_rng=0,
    sys_fap_id_self,
    org_sys_port,
    port_id;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_pp_ppct_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_PORTS_LAG_INFO
    in_prev_lag_info,
    out_prev_lag_info;
  uint8
    in_enable,
    out_enable,
    in_use;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_SET_UNSAFE);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &sys_fap_id_self
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_pb_ports_lag_get_unsafe(
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
        /* restore mapping, use SW DB to get the origin phy-port*/
        res = soc_pb_sw_db_local_to_sys_port_get(
                unit,
                port_id,
                &org_sys_port
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        res = soc_pb_ports_set_in_src_sys_port_set_unsafe(
                unit,
                port_id,
                org_sys_port
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
    res = soc_pb_irr_lag_mapping_tbl_get_unsafe(
            unit,
            lag_ndx,
            0,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    irr_glag_mapping_tbl_data.destination = SOC_PETRA_SYS_PHYS_PORT_INVALID;
    res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
            unit,
            lag_ndx,
            0,
            &irr_glag_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    irr_glag_to_lag_range_tbl_data.range = 0;
    res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
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
        res = soc_pb_egq_pct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

        res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /* restore mapping, use SW DB to get the origin phy-port*/
        res = soc_pb_sw_db_local_to_sys_port_get(
                unit,
                port_id,
                &org_sys_port
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        egq_pp_ppct_data.dst_system_port_id = org_sys_port;
        egq_pct_data.dest_port = org_sys_port;

        res = soc_pb_egq_pct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

        res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      }
    }
  }

  if (info->nof_entries != 0)
  {
    lag_rng_size = SOC_PB_PORT_LAG_LB_RNG_SIZE/info->nof_entries;
    remain = SOC_PB_PORT_LAG_LB_RNG_SIZE - lag_rng_size *info->nof_entries;
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
      irr_glag_mapping_tbl_data.destination = info->lag_member_sys_ports[lag_info_idx].sys_port;
      /*
       *  This mapping affects the LAG hushing.
       *  All LAG must be set sequential here,
       *  the member-id is just a running index,
       *  not meaningful otherwise
       */
      res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
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
        res = soc_pb_egq_pct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = soc_petra_ports_logical_sys_id_build(
                TRUE,
                lag_ndx,
                info->lag_member_sys_ports[lag_info_idx].member_id,
                dummy_sys_phys_id,
                &sys_logic_port_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        egq_pp_ppct_data.dst_system_port_id = sys_logic_port_id;
        egq_pct_data.dest_port = sys_logic_port_id;
        egq_pp_ppct_data.lb_key_min = curr_rng;
        egq_pp_ppct_data.lb_key_max = curr_rng + (lag_rng_size-1);
        /* this is for remain to divide it between ports */
        if (remain)
        {
          egq_pp_ppct_data.lb_key_max += 1;
          --remain;
        }
        curr_rng = egq_pp_ppct_data.lb_key_max + 1;

        res = soc_pb_egq_pct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
                unit,
                port_id,
                &egq_pp_ppct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
      else
      {
        /* save room for other devices */
        curr_rng +=  (lag_rng_size);
        /* this is for remain to divide it between ports */
        if (remain)
        {
          curr_rng += 1;
          --remain;
        }
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

      res = soc_pb_ports_set_in_src_sys_port_set_unsafe(
              unit,
              port_id,
              sys_logic_port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  }

  /*
   * Set lag-size table entry {
   */

  if (out_enable)
  {
     res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
              unit,
              lag_ndx,
              &irr_glag_to_lag_range_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    if (info->nof_entries >= 1)
    {
      irr_glag_to_lag_range_tbl_data.range = info->nof_entries - 1;

      res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_lag_get_unsafe(
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
    cur_sys_port_id = 0xffffffff, /* Assigning all 1s, in case we accidently access it without assigning real value */
    nof_members;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  uint8
    is_lag,
    in_use;
  uint32
    lag_id,
    lag_member_id,
    sys_phys_port_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_GET_UNSAFE);

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
    res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    nof_members =
      irr_glag_to_lag_range_tbl_data.range + 1;

    for (lag_info_idx = 0; lag_info_idx < nof_members; lag_info_idx++)
    {
      res = soc_pb_irr_lag_mapping_tbl_get_unsafe(
              unit,
              lag_ndx,
              lag_info_idx,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      sys_port_curr = irr_glag_mapping_tbl_data.destination;
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
        res = soc_pb_egq_pct_tbl_get_unsafe(
                unit,
                port_id,
                &egq_pct_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        res = soc_petra_ports_logical_sys_id_parse(
                egq_pct_data.dest_port,
                &is_lag,
                &lag_id,
                &(lag_member_id),
                &sys_phys_port_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        out_info->lag_member_sys_ports[lag_info_idx].member_id = lag_member_id;
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

    res = soc_pb_ports_set_in_src_sys_port_get_unsafe(
            unit,
            port_id,
            &cur_sys_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /*
     *  Get in-LAG configuration.
     *  'sys_phys_port_id' is not parsed here,
     *  since the field is represented as a LAG.
     *  So we access the EGQ for sys-port value
     */
    res = soc_petra_ports_logical_sys_id_parse(
            cur_sys_port_id,
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

       res = soc_pb_sw_db_local_to_sys_port_get(
              unit,
              port_id,
              &sys_port_curr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_get_unsafe()",0,0);
}

uint32
  soc_pb_ports_lag_sys_port_info_get_unsafe(
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
    sys_logic_port_id = 0,
    port_id;
  uint8
    is_lag;
  uint32
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_PORTS_LAG_INFO
    in_prev_lag_info,
    out_prev_lag_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_SYS_PORT_INFO_GET_UNSAFE);

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

    res = soc_pb_ports_set_in_src_sys_port_get_unsafe(
            unit,
            port_id,
            &sys_logic_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_ports_logical_sys_id_parse(
            sys_logic_port_id,
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
    res = soc_pb_ports_lag_get_unsafe(
            unit,
            lag_id,
            &in_prev_lag_info,
            &out_prev_lag_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    for (lag_member_id = 0; lag_member_id < out_prev_lag_info.nof_entries; ++lag_member_id)
    {
      res = soc_pb_irr_lag_mapping_tbl_get_unsafe(
              unit,
              lag_id,
              lag_member_id,
              &irr_glag_mapping_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      if (irr_glag_mapping_tbl_data.destination == sys_port)
      {
        ++(port_lag_info->out_lags[lag_id]);/* count how many times this port exist in this lag*/
        ++(port_lag_info->nof_of_out_lags);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_sys_port_info_get_unsafe()",0,0);
}


uint32
  soc_pb_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32
    res;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_ORDER_PRESERVE_SET_UNSAFE);

   /* IRR */

   /* Set, order - preserving { */
    res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    irr_glag_to_lag_range_tbl_data.mode =
      SOC_SAND_BOOL2NUM(is_order_preserving);

    res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_glag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    /*Set order - preserving } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_order_preserve_set_unsafe()",0,0);
}

/*********************************************************************
*     Per-Lag information
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32
    res;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_LAG_ORDER_PRESERVE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

  res = soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
          unit,
          lag_ndx,
          &irr_glag_to_lag_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *is_order_preserving =
    SOC_SAND_NUM2BOOL(irr_glag_to_lag_range_tbl_data.mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_lag_order_preserve_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_header_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    port_type_val = 0,
    res;
  SOC_PB_EGR_PROG_TM_PORT_PROFILE
    eg_tm_profile;
  uint8
    in_enable,
    out_enable,
    skip_config = FALSE;
  uint32
    fatp_nof_ports,
    port_curr,
    port_max;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    egq_ppct_data;
  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA
    tm_port_pp_context;
  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN
    otmh_extensions_en;
  SOC_PETRA_PORT_HEADER_TYPE
    previous_headers[2];
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_port2sys_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_HEADER_TYPE_SET_UNSAFE);

  soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_clear(&otmh_extensions_en);

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  fatp_nof_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);
  port_max = SOC_PETRA_MAX_PRT_IF_FAT_PIPE(port_ndx, fatp_nof_ports);

  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);

  if (in_enable)
  {
    skip_config = FALSE;
    switch(header_type) {
    case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    case SOC_PETRA_PORT_HEADER_TYPE_INJECTED: /* No use of the port_type */
      port_type_val = SOC_PB_PARSER_PP_CONTEXT_ETH;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
    case SOC_PETRA_PORT_HEADER_TYPE_RAW:
      port_type_val = SOC_PB_PARSER_PP_CONTEXT_RAW;
      break;

    case SOC_PETRA_PORT_HEADER_TYPE_TM:
      port_type_val = SOC_PB_PARSER_PP_CONTEXT_ITMH;
      break;

    case SOC_PETRA_PORT_HEADER_TYPE_PROG:
      port_type_val = SOC_PB_PARSER_PP_CONTEXT_PROG;
      break;

    case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
    case SOC_PETRA_PORT_HEADER_TYPE_TDM:
      port_type_val = SOC_PB_PARSER_PP_CONTEXT_FTMH;
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
        /*
         *	Get the previous header type (for raw, tdm)
         */
        res = soc_pb_port_header_type_get_unsafe(
                unit,
                port_curr,
                &(previous_headers[SOC_PETRA_PORT_DIRECTION_INCOMING]),
                &(previous_headers[SOC_PETRA_PORT_DIRECTION_OUTGOING])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /* IHP-Port Info */
        res = soc_pb_pp_ihp_tm_port_pp_context_config_tbl_get_unsafe(
                unit,
                port_curr,
                &tm_port_pp_context
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        /*
         *  The PP context is bits 5:3 of offset1, 2:0 is Packet-Format-Qualifier0
         *  (equal 0)
         */
        tm_port_pp_context.pp_context_offset1 = port_type_val << SOC_PB_PP_PORT_PP_CONTEXT_QUALIFIER_LSB;
        tm_port_pp_context.pp_context_offset2 = 0; /* 16 first bits, e.g. PTCH[31:16] */
        if (header_type != SOC_PETRA_PORT_HEADER_TYPE_INJECTED)
        {
          tm_port_pp_context.pp_context_profile = SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR;
        }
        else /* SOC_PETRA_PORT_HEADER_TYPE_INJECTED */
        {
          tm_port_pp_context.pp_context_profile = SOC_PB_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED;
        }
        tm_port_pp_context.pp_context_value_to_use = 0x1; /* Explicit Value */
        res = soc_pb_pp_ihp_tm_port_pp_context_config_tbl_set_unsafe(
                unit,
                port_curr,
                &tm_port_pp_context
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /* For Injected, extract also the System-Port */
        if (header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED)
        {
          res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
                  unit,
                  port_ndx,
                  &ihp_port2sys_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
          ihp_port2sys_data.system_port_profile = SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_INJECTED;
          ihp_port2sys_data.system_port_offset2 = 2; /* For PTCH[15:0]*/
          res = soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
                  unit,
                  port_ndx,
                  &ihp_port2sys_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
        }

        /* For TDM */
        if (header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM || header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
        {
          /*
           * In order to establish port to be as TDM, it is necessary to
           * create push queues (of TDM type) and to change the
           * destination id packets mapping to "push queues"
           * See UM for more details.
           */
          res = soc_pb_tdm_ifp_set_unsafe(
                  unit,
                  port_curr,
                  TRUE
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);
        }
        else if (
          (header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM && header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
          && (previous_headers[SOC_PETRA_PORT_DIRECTION_INCOMING] == SOC_PETRA_PORT_HEADER_TYPE_TDM
          || previous_headers[SOC_PETRA_PORT_DIRECTION_INCOMING] == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
          )
        {
          /* Disable TDM configuration */
          res = soc_pb_tdm_ifp_set_unsafe(
                  unit,
                  port_curr,
                  FALSE
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);
        }
         
      }
    }
  }

  if (out_enable)
  {
    skip_config = FALSE;
    switch(header_type) {
    case SOC_PETRA_PORT_HEADER_TYPE_ETH:
      port_type_val = 0x3;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_ETH;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_RAW:
      port_type_val = 0x1;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_RAW;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_TM:
      port_type_val = 0x2;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_PROG:
      skip_config = TRUE;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_RAW;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_CPU:
      port_type_val = 0x0;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
      port_type_val = 0x0;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
    case SOC_PETRA_PORT_HEADER_TYPE_TDM:
      port_type_val = 0x0;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1;
      break;
    case SOC_PETRA_PORT_HEADER_TYPE_INJECTED:
      port_type_val = 0x0;
      eg_tm_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_INJECTED;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 130, exit);
    }

    if (!skip_config)
    {
      for (port_curr = port_ndx; port_curr <= port_max; port_curr++)
      {
        /*
         *	Get the previous header type (for tdm)
         */
        res = soc_pb_port_header_type_get_unsafe(
                unit,
                port_curr,
                &(previous_headers[SOC_PETRA_PORT_DIRECTION_INCOMING]),
                &(previous_headers[SOC_PETRA_PORT_DIRECTION_OUTGOING])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 135, exit);

        /* EGQ-PCT */
        res = soc_pb_egq_pct_tbl_get_unsafe(
                unit,
                port_curr,
                &(egq_pct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

        egq_pct_data.port_type = port_type_val;

        res = soc_pb_egq_pct_tbl_set_unsafe(
                unit,
                port_curr,
                &(egq_pct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

        /* EGQ-PPCT */
        res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
                unit,
                port_curr,
                &(egq_ppct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

        egq_ppct_data.port_type = port_type_val;

        res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
                unit,
                port_curr,
                &(egq_ppct_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
       /*
        * fix Egress TM editing profile
        */
        if (header_type == SOC_PETRA_PORT_HEADER_TYPE_TM)
        {
          otmh_extensions_en.outlif_ext_en = SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER;
          res = soc_pb_ports_otmh_extension_set_unsafe(
                  unit,
                  port_curr,
                  &(otmh_extensions_en)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
        }
        else
        {
          if (header_type != SOC_PETRA_PORT_HEADER_TYPE_PROG)
          {
            SOC_PB_PROFILE_NDX_clear(&profile_ndx);
            SOC_PB_PROFILE_PARAMS_clear(&profile_params);
            profile_params.eg_profile = eg_tm_profile;
            profile_ndx.port_ndx = port_curr;
            res = soc_pb_profile_add(
                    unit,
                    SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
                    &profile_ndx,
                    &profile_params,
                    FALSE,
                    &success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

            if (success != SOC_SAND_SUCCESS)
            {
              SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_EG_TM_PROFILE_FULL_ERR, 20, exit);
            }
          }
        }
        /* For TDM */
        if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET)
        {
          /* Packet mode */
          if (header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM || header_type == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
          {
            /* Configure OFP to be TDM */
            res = soc_pb_tdm_ofp_set_unsafe(
                    unit,
                    port_curr,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
          }
          else if (
            (header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM && header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
            && (previous_headers[SOC_PETRA_PORT_DIRECTION_OUTGOING] == SOC_PETRA_PORT_HEADER_TYPE_TDM
            || previous_headers[SOC_PETRA_PORT_DIRECTION_OUTGOING] == SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW)
            )
          {
            /* Disable Configuration OFP TDM */
            res = soc_pb_tdm_ofp_set_unsafe(
                    unit,
                    port_curr,
                    FALSE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 195, exit);
          }
        }
        else /* TDM MODE */
        {
          /* TDM cell mode */
          /* Disable Configuration OFP TDM if port is CPU */
          if (header_type == SOC_PETRA_PORT_HEADER_TYPE_CPU)
          {
            res = soc_pb_tdm_ofp_set_unsafe(
                    unit,
                    port_curr,
                    FALSE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
          }
          else if (
            (header_type != SOC_PETRA_PORT_HEADER_TYPE_CPU)
            && (previous_headers[SOC_PETRA_PORT_DIRECTION_OUTGOING] == SOC_PETRA_PORT_HEADER_TYPE_CPU)
            )
          {
            res = soc_pb_tdm_ofp_set_unsafe(
                    unit,
                    port_curr,
                    TRUE
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 205, exit);
          }
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_header_type_set_unsafe()",0,0);
}

uint32
  soc_pb_port_parse_header_type_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_IN uint32 port_pp_context,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE * header_type_in
  )
{
  uint32
     res;
  uint8
    is_tdm;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PARSE_HEADER_TYPE_UNSAFE);

  switch(port_pp_context) {
  case SOC_PB_PARSER_PP_CONTEXT_RAW:
    *header_type_in = SOC_PETRA_PORT_HEADER_TYPE_RAW;
    break;
  case SOC_PB_PARSER_PP_CONTEXT_ITMH:
    *header_type_in = SOC_PETRA_PORT_HEADER_TYPE_TM;
    break;
  case SOC_PB_PARSER_PP_CONTEXT_ETH:
    *header_type_in = SOC_PETRA_PORT_HEADER_TYPE_ETH;
    break;
  case SOC_PB_PARSER_PP_CONTEXT_PROG:
    *header_type_in = SOC_PETRA_PORT_HEADER_TYPE_PROG;
    break;
  case SOC_PB_PARSER_PP_CONTEXT_FTMH:
    res = soc_pb_tdm_ifp_get_unsafe(
      unit,
      port_ndx,
      &is_tdm
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
    *header_type_in = (is_tdm == TRUE) ? SOC_PETRA_PORT_HEADER_TYPE_TDM : SOC_PETRA_PORT_HEADER_TYPE_STACKING;
    break;
  default:
    *header_type_in = SOC_PETRA_PORT_HEADER_TYPE_NONE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_parse_header_type_unsafe()",0,0);
}


uint32
  soc_pb_port_header_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    internal_profile,
    port_type_val = 0,
    res;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_NONE,
    header_type_out = SOC_PETRA_PORT_HEADER_TYPE_NONE;
  SOC_PB_EGR_PROG_TM_PORT_PROFILE
    port_profile;
  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA
    tm_port_pp_context;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_HEADER_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(header_type_incoming);
  SOC_SAND_CHECK_NULL_INPUT(header_type_outgoing);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* IHP-Port Info */
  res = soc_pb_pp_ihp_tm_port_pp_context_config_tbl_get_unsafe(
    unit,
    port_ndx,
    &tm_port_pp_context
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_type_val = tm_port_pp_context.pp_context_offset1 >> SOC_PB_PP_PORT_PP_CONTEXT_QUALIFIER_LSB;

  res = soc_pb_port_parse_header_type_unsafe(
          unit,
          port_ndx,
          port_type_val,
          &header_type_in
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  /* Special case for Injected */
  if (tm_port_pp_context.pp_context_profile == SOC_PB_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED)
  {
    header_type_in = SOC_PETRA_PORT_HEADER_TYPE_INJECTED;
  }

  /* EGQ-PCT */
  res = soc_pb_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &(egq_pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  port_type_val = egq_pct_data.port_type;

  switch(port_type_val) {
  case 0x0:
    res = soc_pb_egr_prog_editor_profile_get(
            unit,
            port_ndx,
            &port_profile,
            &internal_profile
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    switch(port_profile)
    {
    case SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU:
      header_type_out = SOC_PETRA_PORT_HEADER_TYPE_CPU;
      break;

    case SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1:
      header_type_out = SOC_PETRA_PORT_HEADER_TYPE_STACKING;
      break;

    case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1:
      header_type_out = SOC_PETRA_PORT_HEADER_TYPE_TDM;
      break;

    case SOC_PB_EGR_PROG_TM_PORT_PROFILE_INJECTED:
      header_type_out = SOC_PETRA_PORT_HEADER_TYPE_INJECTED;
      break;

    default:
      header_type_out = SOC_PETRA_PORT_HEADER_TYPE_NONE;
    }
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_header_type_get_unsafe()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pb_port_ingr_map_write_val
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
  soc_pb_port_ingr_map_write_val(
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
  SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;
  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA
    nif2ctxt_data;
  SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA
    rcy_ctxt_map_data;
  SOC_PB_NIF_TYPE
    nif_type;
  uint8
    is_channelized_interface = FALSE;
  uint32
    if_id_internal;
  uint32
    reg_idx,
    fld_idx,
    mal_id,
    ch_id = map_info->channel_id,
    ch_step;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_INGR_MAP_WRITE_VAL);

  regs = soc_petra_regs();

  /* mapping value - FAP index or unmapped */
  port_mapping_val = is_mapped ? port_ndx : SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION;
  
  if (SOC_PB_NIF_IS_PB_ID(map_info->if_id))
  {
    if_id_internal = soc_pb_nif2intern_id(map_info->if_id);

    nif_type = soc_pb_nif_id2type(map_info->if_id);

    res = soc_pb_nif_is_channelized(
            unit,
            map_info->if_id,
            &is_channelized_interface
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(is_channelized_interface)
    {
      ch_id = map_info->channel_id;
    }
    else
    {
      res = soc_pb_ports_expected_chan_get(
              unit,
              nif_type,
              is_channelized_interface,
              map_info->if_id,
              if_id_internal,
              &ch_id,
              &ch_step
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if(ch_id == SOC_PB_PORTS_CH_UNKNOWN)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_NON_CH_IF_ERR, 35, exit);
      }
    }

    /*
    *  Map port to interface (MAL).
    */
    mal_id = SOC_PB_NIF2MAL_GLBL_ID(if_id_internal);

    res = soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
            unit,
            mal_id,
            &(nif2ctxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    reg_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
    if(!is_channelized_interface)
    {
      to_map_val = 0x0; /* Must be unset if mapped to non-channelized interface */
    }
    SOC_SAND_SET_BIT(nif2ctxt_data.contexts_bit_mapping[reg_idx], to_map_val, fld_idx);

    res = soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
            unit,
            mal_id,
            &(nif2ctxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     *  Map port to channel (in-MAL offset for SGMII)
     */

    nif_ctxt_map_data.fap_port = port_mapping_val;
    offset = mal_id * SOC_PETRA_NOF_IF_CHANNELS_MAX + ch_id;

    res = soc_pb_ire_nif_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(nif_ctxt_map_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else if(SOC_PETRA_IS_CPU_IF_ID(map_info->if_id))
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
  else if (SOC_PETRA_IS_OLP_IF_ID(map_info->if_id))
  {
    SOC_PETRA_FLD_SET(regs->ire.olp_fap_port_configuration_reg.olp_fap_port, port_mapping_val, 80, exit);
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(map_info->if_id))
  {
    /* mapping value - FAP index or unmapped */
    rcy_ctxt_map_data.fap_port = port_mapping_val;
    offset = ch_id;

    res = soc_pb_ire_rcy_ctxt_map_tbl_set_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_ingr_map_write_val()",port_ndx,map_info->if_id);
}

/*********************************************************************
* NAME:
*     soc_pb_port2if_ingr_map_new_unmap_old
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
  soc_pb_port2if_ingr_map_new_unmap_old(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT2IF_INGR_MAP_NEW_UNMAP_OLD);

  /*
  * Find if the FAP with the given index is already mapped,
  * if it is - mark it unmap it.
  * Note: in a valid state, this loop will only be entered once,
  * since otherwise currently more then one interface is mapped
  * to a single FAP port.
  */
  do
  {
    res = soc_pb_port_to_interface_map_get_unsafe(
            unit,
            port_ndx,
            &curr_info,
            &out_curr_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

/*
 * COVERITY
 *
 * curr_info.channel_id and curr_info.if_id are initialized inside soc_pb_port_to_interface_map_get_unsafe
 */
/* coverity[uninit_use] */
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
        res = soc_pb_port_ingr_map_write_val(
                unit,
                port_ndx,
                FALSE,
                &curr_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }

    if (iter_count++ >= SOC_PB_PORTS_IF_ITERATIONS_MAX)
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
    res = soc_pb_port_ingr_map_write_val(
            unit,
            port_ndx,
            TRUE,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port2if_ingr_map_new_unmap_old()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pb_port_egr_map_write_val
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
  soc_pb_port_egr_map_write_val(
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
    txi_sel_val,
    res;
  uint32
    thresh_sel_fld_val[2];
  uint32
    ch_id = map_info->channel_id,
    mal_id,
    sch_nif_offset,
    reg_outer_idx = 0,
    reg_inner_idx = 0,
    fld_idx = 0;
  uint32
    if_id_internal,
    fqp_txi_context_id;
  uint8
    is_gmii;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_data;
  SOC_PETRA_TBL_FIELD
    thresh_sel_fld;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_EGR_MAP_WRITE_VAL);

  regs = soc_petra_regs();
  SOC_PETRA_CLEAR(thresh_sel_fld_val, uint32, 2);

  if (SOC_PB_NIF_IS_PB_ID(map_info->if_id))
  {
    if_id_internal = soc_pb_nif2intern_id(map_info->if_id);
    mal_id = SOC_PB_NIF2MAL_GLBL_ID(if_id_internal);
    fqp_txi_context_id = if_id_internal;

    nif_type = soc_pb_nif_id2type(map_info->if_id);
    if (nif_type == SOC_PB_NIF_TYPE_NONE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 3, exit);
    }

    is_gmii = SOC_PB_NIF_IS_TYPE_GMII(nif_type);

    switch(nif_type)
    {
    case SOC_PB_NIF_TYPE_XAUI:
    case SOC_PB_NIF_TYPE_RXAUI:
      ch_id = map_info->channel_id;
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_XAUI_LIKE;
      break;
    case SOC_PB_NIF_TYPE_ILKN:
      ch_id = map_info->channel_id;
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_ILKN;
      break;
    case SOC_PB_NIF_TYPE_QSGMII:
    case SOC_PB_NIF_TYPE_SGMII:
      ch_id = 0;
      sch_nif_offset = soc_petra_sch_if2sched_offset(if_id_internal);
      if (!soc_petra_is_channelized_interface_id(if_id_internal))
      {
        SOC_PB_FLD_SET(regs->sch.port_nif_reg[sch_nif_offset].nifxx_port_id, port_ndx, 25, exit);
      }
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_GMII;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit);
      break;
    }

    /*
     *	When un-mapping, unmap both channelized and non-channelized DB,
     *  since a port could be previously mapped to a different interface-type
     */
    if ((!is_gmii) || !(is_mapped))
    {
      /* set SOC_PETRA_IF_CH_NIF { */

      /* Outer index - MAL index of the channelized NIF */
      reg_outer_idx = mal_id;
      /* Each NIF is configured via 3 registers */
      reg_inner_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
      fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

      SOC_PB_REG_GET(regs->egq.mapping_configuration_mal_in_spaui_reg[reg_outer_idx][reg_inner_idx], reg_val, 35, exit);

      to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
      SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

      SOC_PB_REG_SET(regs->egq.mapping_configuration_mal_in_spaui_reg[reg_outer_idx][reg_inner_idx], reg_val, 40, exit);
      /* set SOC_PETRA_IF_CH_NIF } */
    }

    if (is_gmii || !(is_mapped))
    {
      /* set SOC_PETRA_IF_NONCH_NIF { */
      
      /* Outer index - MAL index of the channelized NIF */
      reg_inner_idx = mal_id;
      fld_idx = SOC_PETRA_FLD_IDX_GET(if_id_internal, SOC_PB_MAX_NIFS_PER_MAL);

      SOC_PB_REG_GET(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx], reg_val, 50, exit);

      to_map_val = SOC_SAND_BOOL2NUM(is_mapped);

      SOC_PB_FLD_TO_REG(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx].valid[fld_idx], to_map_val, reg_val, 60, exit);

      fld_val = port_ndx;

      SOC_PB_FLD_TO_REG(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx].ofp_index[fld_idx], fld_val, reg_val, 70, exit);

      SOC_PB_REG_SET(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx], reg_val, 80, exit);
      /* set SOC_PETRA_IF_NONCH_NIF } */
    }
  }
  else if(SOC_PETRA_IS_ECI_IF_ID(map_info->if_id))
  {
    /* CPU, OLP */
    SOC_PB_REG_GET(regs->egq.mapping_configuration_for_cpu_ports_reg, reg_val, 90, exit);

    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);

    if (port_ndx == SOC_PETRA_FRST_CPU_PORT_ID)
    {
      /* CPU */
      fld_idx = 0;
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_CPU;
      fqp_txi_context_id = SOC_PB_NIF_NOF_NIFS + 0;
    } else if (SOC_SAND_IS_VAL_IN_RANGE(port_ndx, SOC_PETRA_SCND_CPU_PORT_ID, SOC_PETRA_LAST_CPU_PORT_ID))
    {
      /* CPU */
      fld_idx = port_ndx - SOC_PETRA_SCND_CPU_PORT_ID + 1;
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_CPU;
      fqp_txi_context_id = SOC_PB_NIF_NOF_NIFS + 0;
    } else if (SOC_PETRA_IS_OLP_FAP_PORT_ID(port_ndx))
    {
      /* OLP */
      fld_idx = 7;

      SOC_PB_FLD_SET(regs->sch.olp_config_reg.olpport_id, port_ndx, 95, exit);
      txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_OLP;
      fqp_txi_context_id = SOC_PB_NIF_NOF_NIFS + 1;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_INVALID_ECI_PORT_IDX_ERR, 100, exit);
    }

    SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

    SOC_PB_REG_SET(regs->egq.mapping_configuration_for_cpu_ports_reg, reg_val, 110, exit);
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(map_info->if_id))
  {
    /* RCY */
    reg_inner_idx = port_ndx / SOC_SAND_REG_SIZE_BITS;
    fld_idx = port_ndx % SOC_SAND_REG_SIZE_BITS;

    SOC_PB_REG_GET(regs->egq.mapping_configuration_for_recycling_ports_reg[reg_inner_idx], reg_val, 120, exit);

    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
    SOC_SAND_SET_BIT(reg_val, to_map_val, fld_idx);

    SOC_PB_REG_SET(regs->egq.mapping_configuration_for_recycling_ports_reg[reg_inner_idx], reg_val, 130, exit);
    txi_sel_val = SOC_PB_EGQ_TXI_TH_ID_RCY;
    fqp_txi_context_id = SOC_PB_NIF_NOF_NIFS + 2;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 140, exit);
  }

  /* Configure the channel number. When non-channelized, ch_id=0 */
  res = soc_pb_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &(pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  pct_data.port_ch_num = ch_id;

  res = soc_pb_egq_pct_tbl_set_unsafe(
          unit,
          port_ndx,
          &(pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  /*
   *	TXi threshold select
   */
  reg_inner_idx = (SOC_PB_EGQ_TXI_SEL_REG_FLD_SIZE_BITS * fqp_txi_context_id) / SOC_SAND_REG_SIZE_BITS;
  thresh_sel_fld.lsb = (uint8)((SOC_PB_EGQ_TXI_SEL_REG_FLD_SIZE_BITS * fqp_txi_context_id) % SOC_SAND_REG_SIZE_BITS);
  thresh_sel_fld.msb = (uint8)(thresh_sel_fld.lsb + SOC_PB_EGQ_TXI_SEL_REG_FLD_SIZE_BITS - 1);
  
  SOC_PB_REG_GET(regs->egq.nrdy_th_sel_reg[reg_inner_idx], thresh_sel_fld_val[0], 170, exit);
  if (reg_inner_idx < (SOC_PB_EGQ_NRDY_TH_SEL_REG_MULT_NOF_REGS-1))
  {
    SOC_PB_REG_GET(regs->egq.nrdy_th_sel_reg[reg_inner_idx+1], thresh_sel_fld_val[1], 172, exit);
  }

  res = soc_petra_field_in_place_set(
          &(txi_sel_val),
          &(thresh_sel_fld),
          thresh_sel_fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  SOC_PB_REG_SET(regs->egq.nrdy_th_sel_reg[reg_inner_idx], thresh_sel_fld_val[0], 170, exit);
  if (reg_inner_idx < (SOC_PB_EGQ_NRDY_TH_SEL_REG_MULT_NOF_REGS-1))
  {
    SOC_PB_REG_SET(regs->egq.nrdy_th_sel_reg[reg_inner_idx+1], thresh_sel_fld_val[1], 172, exit);
  }
 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_egr_map_write_val()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pb_port2if_egr_map_new_unmap_old
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
  soc_pb_port2if_egr_map_new_unmap_old(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK,
    pqp_reg_val,
    fqp_reg_val,
    ilkn_cfg;
  uint8
    is_already_mapped = FALSE,
    ilkn_full_pkt = FALSE;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    curr_info,
    in_curr_info;
  uint32
    iter_count = 0,
    reg_idx,
    bit_offset,
    ilkn_id;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT2IF_EGR_MAP_NEW_UNMAP_OLD);

  regs = soc_petra_regs();

  reg_idx    = port_ndx / 32;
  bit_offset = port_ndx % 32;

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
    res = soc_pb_port_to_interface_map_get_unsafe(
            unit,
            port_ndx,
            &in_curr_info,
            &curr_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
/*
 * COVERITY
 *
 * curr_info.channel_id and curr_info.if_id are initialized inside soc_pb_port_to_interface_map_get_unsafe
 */
/* coverity[uninit_use] */
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
        res = soc_pb_port_egr_map_write_val(
                unit,
                port_ndx,
                FALSE,
                &curr_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
    }

    if (iter_count++ >= SOC_PB_PORTS_IF_ITERATIONS_MAX)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR, 70, exit);
    }

  } while((!is_already_mapped) && (curr_info.if_id != SOC_PETRA_IF_ID_NONE));

  if ((!is_already_mapped) && (info->if_id != SOC_PETRA_IF_ID_NONE))
  {
    SOC_PB_REG_GET(regs->egq.pqp_spr_per_port_dis_reg[reg_idx], pqp_reg_val, 80, exit);
    SOC_PB_REG_GET(regs->egq.fqp_spr_per_port_dis_reg[reg_idx], fqp_reg_val, 90, exit);
    pqp_reg_val &= SOC_SAND_RBIT(bit_offset);
    fqp_reg_val &= SOC_SAND_RBIT(bit_offset);
    if (SOC_PB_NIF_IS_TYPE_ID(ILKN, info->if_id))
    {
      ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, info->if_id);
      SOC_PB_FLD_GET(regs->egq.nif_interlaken_mode_reg.cfg_ilaken[ilkn_id], ilkn_cfg, 100, exit);
      ilkn_full_pkt = SOC_SAND_NUM2BOOL(ilkn_cfg);
    }
    if (ilkn_full_pkt)
    {
      fqp_reg_val |= SOC_SAND_BIT(bit_offset);
    }
    else
    {
      pqp_reg_val |= SOC_SAND_BIT(bit_offset);
    }
    SOC_PB_REG_SET(regs->egq.pqp_spr_per_port_dis_reg[reg_idx], pqp_reg_val, 110, exit);
    SOC_PB_REG_SET(regs->egq.fqp_spr_per_port_dis_reg[reg_idx], fqp_reg_val, 120, exit);

    /*
     * Write the FAP index to the HW,
     * this is the actual mapping.
     */
    res = soc_pb_port_egr_map_write_val(
            unit,
            port_ndx,
            TRUE,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port2if_egr_map_new_unmap_old()",0,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res,
    ilkn_a_enabled = 0,
    ilkn_b_enabled = 0,
    fld_val = 0;
  uint8
    in_enable,
    out_enable;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_INTERFACE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

    regs = soc_petra_regs();


  /* ILKN: Verify the following when mapping ports 1,2:
    If ILKN-A is enabled, FAP Port 1 can only be mapped to ILKN-A
    If ILKN-B is enabled, FAP Port 2 can only be mapped to ILKN-B */
  SOC_PB_REG_GET(regs->nbi.enable_interlaken_reg, fld_val, 61, exit);
  SOC_PB_FLD_GET(regs->nbi.enable_interlaken_reg.enable_ilkn[0], ilkn_a_enabled, 62, exit);
  SOC_PB_FLD_GET(regs->nbi.enable_interlaken_reg.enable_ilkn[1], ilkn_b_enabled, 63, exit);

  /* In case the whole register is not initialized */
  /* Overwrite the "Enabled" indication to FALSE   */
  ilkn_a_enabled = (fld_val == 0xFFFFFFFF) ? FALSE : ilkn_a_enabled;
  ilkn_b_enabled = (fld_val == 0xFFFFFFFF) ? FALSE : ilkn_b_enabled;

  if(port_ndx == SOC_PETRA_NIF_ID_ILKN_0_DEDICATED_PORT)
  {
    if(ilkn_a_enabled)
    {
      /* ILKN A IS ENABLED */
      /* Port 1 must be mapped to ILKN-A */
      if(info->if_id != SOC_PETRA_NIF_ID_ILKN_0)
      {
          /*SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_ILLEGAL_ILKN_PORT_MAPPING,70,exit);*/
      }
    }
  }
  if(port_ndx == SOC_PETRA_NIF_ID_ILKN_1_DEDICATED_PORT)
  {
    if(ilkn_b_enabled)
    {
      /* ILKN B IS ENABLED */
      /* Port 2 must be mapped to ILKN-B */
      if(info->if_id != SOC_PETRA_NIF_ID_ILKN_1)
      {
        /*SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_ILLEGAL_ILKN_PORT_MAPPING,80,exit);*/
      }
    }
  }

  in_enable = SOC_PETRA_PORT_IS_INCOMING(direction_ndx);
  out_enable = SOC_PETRA_PORT_IS_OUTGOING(direction_ndx);

  if (in_enable)
  {
    /*
     *  Map Incoming FAP Ports to Interfaces at the Ingress
     */

    res = soc_pb_port2if_ingr_map_new_unmap_old(
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
    res = soc_pb_port2if_egr_map_new_unmap_old(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_to_interface_map_set_unsafe()",port_ndx,0);
}

/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_INTERFACE_MAP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_PORT_NOF_DIRECTIONS-1,
    SOC_PETRA_PORT_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  if (SOC_PB_NIF_IS_PB_ID(info->if_id))
  {
    res = soc_pb_nif_id_verify(info->if_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_petra_interface_id_verify(unit, info->if_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_ports_fap_and_nif_type_match_verify(
            info->if_id,
            port_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, info->if_id))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->channel_id, SOC_PB_NOF_ILKN_IF_CHANNELS_MAX-1,
      SOC_PETRA_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 47, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->channel_id, SOC_PETRA_NOF_IF_CHANNELS_MAX-1,
      SOC_PETRA_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 50, exit
    );
  }

  /* Verify channel_id is 0 for non-channelized interface IDs */
  if ((info->channel_id != 0) &&
      ((((direction_ndx == SOC_PETRA_PORT_DIRECTION_INCOMING || direction_ndx == SOC_PETRA_PORT_DIRECTION_BOTH) && SOC_PETRA_IS_CPU_IF_ID(info->if_id)) ||
       (SOC_PETRA_IS_OLP_IF_ID(info->if_id)) ||
       (SOC_PB_NIF_IS_TYPE_ID(SGMII, info->if_id)) ||
       (SOC_PB_NIF_IS_TYPE_ID(QSGMII, info->if_id)))))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NON_ZERO_CHANNEL_FOR_UNCHANNELIZED_IF_TYPE_ERR, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_to_interface_map_verify()",port_ndx,0);
}



/*********************************************************************
*     Maps the specified FAP Port to interface and channel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_get_unsafe(
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
  SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;
  SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA
    rcy_ctxt_map_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_data;
  SOC_PB_NIF_TYPE
    nif_type;
  uint8
    ilkn_extended = FALSE,
    skip_mal = FALSE,
    found = FALSE,
    is_mapped = FALSE,
    is_channelized_interface;
  uint32  
    fld_idx,
    mal_id = 0,
    ch_id,
    ch_step,
    ch_start, 
    ch_end,
    nof_chs,
    reg_outer_idx = 0,
    reg_inner_idx = 0,
    if_id_internal;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    info_in,
    info_out;
  SOC_PETRA_INTERFACE_ID
    nif_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_INTERFACE_MAP_GET_UNSAFE);

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
  for (mal_id = 0; (mal_id < SOC_PB_NOF_MAC_LANES) && (!found); mal_id++)
  {
    nof_chs = SOC_PETRA_NOF_IF_CHANNELS_MAX; ilkn_extended = FALSE;
    res = soc_pb_nif_mal_type_get(
            unit,
            mal_id,
            &nif_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    if(nif_type == SOC_PB_NIF_TYPE_NONE)
    {
      continue;
    }

    if (nif_type == SOC_PB_NIF_TYPE_ILKN)
    {
      if(SOC_PB_NIF_IS_ILKN_MAL(mal_id))
      {
        SOC_PB_FLD_GET(regs->ire.fap_port_configuration_reg.use_all_channels, fld_val, 40, exit);
        if (SOC_SAND_GET_BIT(fld_val, mal_id == 0 ? 0 : 2))
        {
          nof_chs = SOC_PB_NOF_ILKN_IF_CHANNELS_MAX; ilkn_extended = TRUE; skip_mal = TRUE;
        }
      }    
    }

    if((!ilkn_extended) && (skip_mal))
    {
      skip_mal = FALSE;
      continue;
    }

    if_id_internal = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
    nif_id = soc_pb_nif_intern2nif_id(nif_type, if_id_internal);
    if(SOC_PB_NIF_IS_TYPE_GMII(nif_type))
    {
      if_id_internal = SOC_PB_NIF_INVALID_VAL_INTERN;
      is_channelized_interface = FALSE;
    }
    else
    {
      res = soc_pb_nif_is_channelized(
              unit,
              nif_id,
              &is_channelized_interface
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }

    res = soc_pb_ports_expected_chan_get(
            unit,              
            nif_type,
            is_channelized_interface,
            nif_id,
            if_id_internal,
            &ch_id,
            &ch_step
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

    if(ch_id == SOC_PB_PORTS_CH_UNKNOWN)
    {
      ch_start = 0; 
      ch_end = nof_chs-1;
    }
    else
    {
      ch_start = ch_end = ch_id;
    }

    for (ch_id = ch_start; (ch_id <= ch_end) && (!found); ch_id+= ch_step)
    {
      offset = mal_id * SOC_PETRA_NOF_IF_CHANNELS_MAX + ch_id;
      res = soc_pb_ire_nif_ctxt_map_tbl_get_unsafe(
              unit,
              offset,
              &(nif_ctxt_map_data)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      port_mapping_val = nif_ctxt_map_data.fap_port;
      if (port_mapping_val == port_ndx)
      {
        found = TRUE;

        switch(nif_type)
        {
        case SOC_PB_NIF_TYPE_XAUI:
        case SOC_PB_NIF_TYPE_RXAUI:
        case SOC_PB_NIF_TYPE_ILKN:
          info_in.channel_id = ch_id;
          if_id_internal = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
          break;
        case SOC_PB_NIF_TYPE_QSGMII:
        case SOC_PB_NIF_TYPE_SGMII:
          info_in.channel_id = 0;
          if_id_internal = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id) + (ch_id / (SOC_PETRA_NOF_IF_CHANNELS_MAX / SOC_PETRA_MAX_NIFS_PER_MAL ));
          break;
        default:
          info_in.channel_id = 0;
          if_id_internal = SOC_PB_NIF_INVALID_VAL_INTERN;
          break;
        }

        info_in.if_id = soc_pb_nif_intern2nif_id(nif_type, if_id_internal);
      }
    }
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
          res = soc_pb_ire_rcy_ctxt_map_tbl_get_unsafe(
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
  for (mal_id = 0; (mal_id < SOC_PB_NOF_MAC_LANES) && (!found); mal_id++)
  {
    /* Outer index - MAL index of the channelized NIF */
    reg_outer_idx = mal_id;
    /* Each NIF is configured via 3 registers */
    reg_inner_idx = SOC_PETRA_REG_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(port_ndx, SOC_SAND_REG_SIZE_BITS);

    SOC_PB_REG_GET(regs->egq.mapping_configuration_mal_in_spaui_reg[reg_outer_idx][reg_inner_idx], reg_val, 40, exit);
    to_map_val = SOC_SAND_GET_BIT(reg_val, fld_idx);
    is_mapped = SOC_SAND_NUM2BOOL(to_map_val);
    if (is_mapped)
    {
      found = TRUE;
      if_id_internal = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);

      res = soc_pb_nif_mal_type_get(
              unit,
              mal_id,
              &nif_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      info_out.if_id = soc_pb_nif_intern2nif_id(nif_type, if_id_internal);
      /* Real channel id will be checked later */
      info_out.channel_id = 0;
    }
  }


  if (found == FALSE)
  {
    /*
     *  Check non-channelized NIF mapping
     */
    for (mal_id = 0; (mal_id < SOC_PB_NOF_MAC_LANES) && (!found); mal_id++)
    {
      for (fld_idx = 0; (fld_idx < SOC_PB_MAX_NIFS_PER_MAL) && (!found); fld_idx++)
      {
        reg_inner_idx = mal_id;

        SOC_PB_REG_GET(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx], reg_val, 50, exit);

        SOC_PB_FLD_FROM_REG(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx].valid[fld_idx], to_map_val, reg_val, 60, exit);

        is_mapped = SOC_SAND_NUM2BOOL(to_map_val);

        if (is_mapped == TRUE)
        {
          fld_val = 0;
          SOC_PB_FLD_FROM_REG(regs->egq.mapping_configuration_mal_in_sgmii_reg[reg_inner_idx].ofp_index[fld_idx], fld_val, reg_val, 70, exit);

          if (fld_val == port_ndx)
          {
            found = TRUE;

            if_id_internal = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id) + fld_idx;
            res = soc_pb_nif_mal_type_get(
                    unit,
                    mal_id,
                    &nif_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

            info_out.if_id = soc_pb_nif_intern2nif_id(nif_type, if_id_internal);
            info_out.channel_id = 0;
          };
        }
      }
    } /* Non-channelized interfaces loop */
  } /* Check non-channelized interfaces */

  if (!found)
  {
    /* CPU, OLP */
    SOC_PB_REG_GET(regs->egq.mapping_configuration_for_cpu_ports_reg, reg_val, 90, exit);
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
    }
    else if (SOC_SAND_IS_VAL_IN_RANGE(port_ndx, SOC_PETRA_SCND_CPU_PORT_ID, SOC_PETRA_LAST_CPU_PORT_ID))
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

      SOC_PB_REG_GET(regs->egq.mapping_configuration_for_recycling_ports_reg[reg_inner_idx], reg_val, 120, exit);

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
    res = soc_pb_egq_pct_tbl_get_unsafe(
            unit,
            port_ndx,
            &(pct_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    info_out.channel_id = pct_data.port_ch_num;
  }

  if (!found)
  {
    info_out.if_id = SOC_PETRA_IF_ID_NONE;
    info_out.channel_id = 0;
  }

  SOC_PETRA_COPY(info_incoming, &info_in, SOC_PETRA_PORT2IF_MAPPING_INFO, 1);
  SOC_PETRA_COPY(info_outgoing, &info_out, SOC_PETRA_PORT2IF_MAPPING_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_to_interface_map_get_unsafe()",port_ndx,0);
}

/*********************************************************************
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res,
    field_val;
  SOC_PB_PP_REGS
    *regs;
  SOC_PB_REGS
    *tm_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_FTMH_EXTENSION_SET_UNSAFE);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();


  field_val = (uint32)ext_option;

  SOC_PB_PP_FLD_SET(regs->eci.system_headers_configurations0_reg.ftmh_ext, field_val, 10, exit);

	/* No update of the egress editor profile since it is supposed to be called only at init */
	
  /*
   * PB-B0 bug fix to disable by default by writing FMTH-Ext[1]
   */
  if (SOC_PB_REV_ABOVE_A1)
  {
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->egq.egq_chicken_bits.eep_resolution_chicken_bit, (field_val >> 1), 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_ftmh_extension_set_unsafe()",0,0);
}

uint32
  soc_pb_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res,
    field_val;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_FTMH_EXTENSION_GET_UNSAFE);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->eci.system_headers_configurations0_reg.ftmh_ext, field_val, 10, exit);

  *ext_option = (SOC_PETRA_PORTS_FTMH_EXT_OUTLIF)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_ftmh_extension_get_unsafe()",0,0);
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
  soc_pb_ports_otmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_outgoing,
    header_type_incoming;
  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF
    ftmh_ext;
  uint32
    tm_port_profile;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_OTMH_EXTENSION_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Verify the Port is TM
   */
  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &header_type_incoming,
          &header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (header_type_outgoing != SOC_PETRA_PORT_HEADER_TYPE_TM)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_TM_PORT_ERR, 30, exit);
  }

  res = soc_petra_ports_ftmh_extension_get_unsafe(
          unit,
          &ftmh_ext
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if ((ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER)&&
    (info->outlif_ext_en != SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR, 60, exit);
  }
  else
  {
    if (
      (ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC)&&
      !((info->outlif_ext_en == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC)||
      (info->outlif_ext_en == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER))
      )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR, 70, exit);
    }
  }

  tm_port_profile =
    SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD(info->src_ext_en, info->dest_ext_en, info->outlif_ext_en);
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  profile_params.eg_profile = tm_port_profile;
  profile_ndx.port_ndx = port_ndx;
  res = soc_pb_profile_add(
          unit,
          SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
          &profile_ndx,
          &profile_params,
          FALSE,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  if (success != SOC_SAND_SUCCESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_EG_TM_PROFILE_FULL_ERR, 120, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_otmh_extension_set_unsafe()",0,0);
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
  soc_pb_ports_otmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    internal_profile,
    res;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_outgoing,
    header_type_incoming;
  SOC_PB_EGR_PROG_TM_PORT_PROFILE
    port_profile;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_OTMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Verify the Port is TM
   */
  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &header_type_incoming,
          &header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (header_type_outgoing != SOC_PETRA_PORT_HEADER_TYPE_TM)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_TM_PORT_ERR, 20, exit);
  }

  res = soc_pb_egr_prog_editor_profile_get(
          unit,
          port_ndx,
          &port_profile,
          &internal_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->outlif_ext_en = (port_profile  - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV)
    % SOC_PETRA_PORTS_FTMH_NOF_EXT_OUTLIFS;
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_GET(port_profile, info->src_ext_en);
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_DEST_GET(port_profile, info->dest_ext_en);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_otmh_extension_get_unsafe()",0,0);
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
  soc_pb_port_egr_hdr_credit_discount_select_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE    cr_discnt_type
  )
{
  uint32
    res;
  SOC_PB_EGQ_PCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE);

  res = soc_pb_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  data.cr_adjust_type = cr_discnt_type;

  res = soc_pb_egq_pct_tbl_set_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_egr_hdr_credit_discount_select_set_unsafe()",0,0);
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
  soc_pb_port_egr_hdr_credit_discount_select_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;
  SOC_PB_EGQ_PCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

  res = soc_petra_fap_port_id_verify(unit, port_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_egq_pct_tbl_get_unsafe(
          unit,
          port_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *cr_discnt_type = data.cr_adjust_type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_port_egr_hdr_credit_discount_select_get_unsafe()",0,0);
}
/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_itmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  )
{
  uint32
    res;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_port2sys_data;
  uint32
    pp_port_in,
    pp_port_out,
    sce_port_ext_start_in_bytes;
  SOC_PETRA_PORT_HEADER_TYPE
    header_type_outgoing,
    header_type_incoming;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_ITMH_EXTENSION_SET_UNSAFE);

  /*
   *	Verify it is a TM Port
   */
  res = soc_petra_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &header_type_incoming,
          &header_type_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  if (header_type_incoming != SOC_PETRA_PORT_HEADER_TYPE_TM)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_TM_PORT_ERR, 5, exit);
  }

  /*
   *	Get-Modify-Set to the data table
   */

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port2sys_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Computation of the beginning of the source port extension
   *  according to the previous possible extensions
   *  Get the PP Port of this TM Port
   */
  res = soc_pb_port_to_pp_port_map_get_unsafe(
          unit,
          port_ndx,
          &pp_port_in,
          &pp_port_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_port_pp_port_get_unsafe(
          unit,
          pp_port_in,
          &pp_port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   * Assumption that the Statistic-Tag is at the beginning of the packet
   * The Ingress Shaping + Src-System-Port cannot be supported: the location
   * of the Source-System-Port is not set per Packet
   */
  sce_port_ext_start_in_bytes = pp_port_info.first_header_size + SOC_PB_PORT_ITMH_SIZE_IN_BYTES;

  ihp_port2sys_data.system_port_offset2 = sce_port_ext_start_in_bytes;
  if (ext_en == TRUE)
  {
    ihp_port2sys_data.system_port_profile = SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_EXT;
  }
  else
  {
    ihp_port2sys_data.system_port_profile = SOC_PB_PORTS_FEM_PROFILE_DIRECT_EXTR;
  }

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihp_port2sys_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_itmh_extension_set_unsafe()",0,0);
}

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_itmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT uint8                 *ext_en
  )
{
  uint32
    res;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_port2sys_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_ITMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ext_en);

  res = soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihp_port2sys_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ihp_port2sys_data.system_port_profile == SOC_PB_PORTS_FEM_PROFILE_SRC_PORT_EXT)
  {
    *ext_en = TRUE;
  }
  else
  {
    *ext_en = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_itmh_extension_get_unsafe()",0,0);
}

STATIC
  uint32
    soc_pb_port_pp_port_to_trap_ndx_map_get_unsafe(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         pp_port_ndx,
      SOC_SAND_OUT uint32         *trap_ndx
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TM_PROFILE_TO_TRAP_NDX_MAP_GET_UNSAFE);

  /*
   *	Use the user-defined traps + unknown DA
   */
  if (pp_port_ndx <= 61)
  {
    if (pp_port_ndx <= 5)
    {
      *trap_ndx = 0x2A + pp_port_ndx;
    }
    else if (pp_port_ndx <= 7)
    {
      *trap_ndx = 0x5D + (pp_port_ndx - 6);
    }
    else if (pp_port_ndx <= 10)
    {
      *trap_ndx = 0x9C +  (pp_port_ndx - 8);
    }
    else if (pp_port_ndx <= 12)
    {
      *trap_ndx = 0xAD + (pp_port_ndx - 11);
    }
    else if (pp_port_ndx <= 27)
    {
      *trap_ndx = 0xB1 + (pp_port_ndx - 13);
    }
    else if (pp_port_ndx <= 35)
    {
      *trap_ndx = 0xC8 + (pp_port_ndx - 28);
    }
    else if (pp_port_ndx <= 51)
    {
      *trap_ndx = 0xD0 + (pp_port_ndx - 36);
    }
    else /* if (pp_port_ndx <= 61) */
    {
      *trap_ndx = 0xF0 + (pp_port_ndx - 52);
    }
  }
  else if (pp_port_ndx <= 63)
  {
    *trap_ndx = 0x98 + (pp_port_ndx - 62);
  }
  else /* Unknown */
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORT_TRAP_CODE_NOT_SUPPORTED_ERR, 10, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_to_trap_ndx_map_get_unsafe()", pp_port_ndx, 0);
}


uint32
  soc_pb_port_forwarding_header_configuration_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pp_port_ndx,
    SOC_SAND_OUT uint32                            *trap_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *fwd_act_profile_tbl_data,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA     *snoop_tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_CONFIGURATION_GET_UNSAFE);

  /*
   *	1. Apply static TM Port Profile to Trap index mapping:
   *     Use user-defined traps
   */
  res = soc_pb_port_pp_port_to_trap_ndx_map_get_unsafe(
          unit,
          pp_port_ndx,
          trap_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
          unit,
          *trap_ndx,
          fwd_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_pb_pp_ihb_snoop_action_tbl_get_unsafe(
          unit,
          *trap_ndx,
          snoop_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_configuration_get_unsafe()", pp_port_ndx, 0);
}

uint32
  soc_pb_port_forwarding_header_configuration_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pp_port_ndx,
    SOC_SAND_IN  uint32                             trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *fwd_act_profile_tbl_data,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA     *snoop_tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_CONFIGURATION_SET_UNSAFE);

  res = soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
          unit,
          trap_ndx,
          fwd_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
          unit,
          trap_ndx,
          snoop_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  pinfo_llr_tbl.default_cpu_trap_code = trap_ndx;

  res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_configuration_set_unsafe()", pp_port_ndx, 0);
}

STATIC
  uint32
    soc_pb_port_forwarding_header_dest_compute(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  SOC_PETRA_DEST_INFO      *destination,
      SOC_SAND_IN  uint8              is_itmh_format, /* 18b destination encoding */
      SOC_SAND_OUT uint32             *dest_fin
    )
{
  uint32
    dest = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_PORTS_ITMH
    itmh;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(destination);
  SOC_SAND_CHECK_NULL_INPUT(dest_fin);

  *dest_fin = 0;

  /*
   *	1. Build the ITMH destination encoding
   */
  soc_petra_PETRA_PORTS_ITMH_clear(&itmh);
  itmh.base.destination.type = destination->type;
  itmh.base.destination.id = destination->id;
  itmh.extension.src_port.type = 0;

  if (itmh.base.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
  {
    soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q);

    res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
            unit,
            &base_q
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    if (base_q.queue_id_add_not_decrement)
    {
      SOC_SAND_ERR_IF_BELOW_MIN(
        itmh.base.destination.id, base_q.base_queue_id,
        SOC_PETRA_Q_ID_WITH_BASE_Q_MISMATCH_ERR, 16, exit
      );

      itmh.base.destination.id -= base_q.base_queue_id;
    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        itmh.base.destination.id, base_q.base_queue_id,
        SOC_PETRA_Q_ID_WITH_BASE_Q_MISMATCH_ERR, 17, exit
        );

      itmh.base.destination.id += base_q.base_queue_id;
    }
  }

  /*
   *	Encode in 17b encoding
   */
  if (!is_itmh_format) {
      switch(itmh.base.destination.type)
      {
      case SOC_PETRA_DEST_TYPE_QUEUE:
        dest = itmh.base.destination.id | ( 3 << 15); 
        break;
      case SOC_PETRA_DEST_TYPE_MULTICAST:
        dest = itmh.base.destination.id | ( 1 << 14);
        break;
      case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
        soc_petra_ports_logical_sys_id_build(FALSE, 0, 0, itmh.base.destination.id, &dest);
        break;
      case SOC_PETRA_DEST_TYPE_LAG:
        soc_petra_ports_logical_sys_id_build(TRUE, itmh.base.destination.id, 0, 0, &dest);
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 18, exit);
        break;
      }
  }
  else {
      switch(itmh.base.destination.type)
      {
      case SOC_PETRA_DEST_TYPE_QUEUE:
        dest = itmh.base.destination.id | ( 3 << 16); 
        break;
      case SOC_PETRA_DEST_TYPE_MULTICAST:
        dest = itmh.base.destination.id | ( 2 << 16);
        break;
      case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
        soc_petra_ports_logical_sys_id_build(FALSE, 0, 0, itmh.base.destination.id, &dest);
        break;
      case SOC_PETRA_DEST_TYPE_LAG:
        soc_petra_ports_logical_sys_id_build(TRUE, itmh.base.destination.id, 0, 0, &dest);
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 18, exit);
        break;
      }
  }

  *dest_fin = dest;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_dest_compute()", 0, 0);
}




STATIC
  uint32
    soc_pb_port_forwarding_header_set_for_raw(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       pp_port_ndx,
      SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
    )
{
  uint32
    dest,
    trap_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
    snoop_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	1. Apply static TM Port Profile to Trap index mapping:
   *     Use user-defined traps
   */
  res = soc_pb_port_forwarding_header_configuration_get_unsafe(
          unit,
          pp_port_ndx,
          &trap_ndx,
          &fwd_act_profile_tbl_data,
          &snoop_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  /*
   *	2. Build the ITMH destination encoding
   */
  res = soc_pb_port_forwarding_header_dest_compute(
          unit,
          &(info->destination),
          FALSE, /* 17b encoding */
          &dest
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  fwd_act_profile_tbl_data.fwd_act_destination = dest;
  fwd_act_profile_tbl_data.fwd_act_destination_valid = 0x1;
  fwd_act_profile_tbl_data.fwd_act_traffic_class = info->tr_cls;
  fwd_act_profile_tbl_data.fwd_act_traffic_class_valid = 0x1;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence = info->dp;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_valid = 0x1;
  fwd_act_profile_tbl_data.fwd_act_counter_pointer = info->counter.id;
  fwd_act_profile_tbl_data.fwd_act_counter_pointer_selector = info->counter.processor_id;

  /*
   *	3. Disable the other operations
   */
  fwd_act_profile_tbl_data.fwd_act_trap = 0x0;
  fwd_act_profile_tbl_data.fwd_act_control = 0x0;
  fwd_act_profile_tbl_data.fwd_act_fwd_offset_index_valid = 0x0;
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi = 0x0;
  fwd_act_profile_tbl_data.fwd_act_destination_add_vsi_shift = 0x0;
  fwd_act_profile_tbl_data.fwd_act_meter_pointer = 0x0;
  fwd_act_profile_tbl_data.fwd_act_meter_pointer_selector = 0x0;
  fwd_act_profile_tbl_data.fwd_act_drop_precedence_meter_command_valid = 0x0;
  fwd_act_profile_tbl_data.fwd_act_ethernet_policer_pointer_valid = 0x0;
  fwd_act_profile_tbl_data.fwd_act_learn_disable = 0x1;

  snoop_tbl_data.snoop_action = info->snoop_cmd_ndx;

  /*
   *	4. Write to the Hardware
   */
  res = soc_pb_port_forwarding_header_configuration_set_unsafe(
          unit,
          pp_port_ndx,
          trap_ndx,
          &fwd_act_profile_tbl_data,
          &snoop_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_set_for_raw()", pp_port_ndx, 0);
}



/*********************************************************************
*      Define the Forwarding header parameters for Raw ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    dest,
    res = SOC_SAND_OK;
  SOC_PB_PMF_DIRECT_TBL_DATA
    dt_data;
  uint32
    tm_port_ndx,
    entry_offset_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* 
   * Different processing between Raw and TDM_Raw 
   */
  if (pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS)
  {
    res = soc_pb_port_forwarding_header_set_for_raw(
            unit,
            pp_port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } 
  else
  {
    /* 
     * Compute the destination 
     */
    res = soc_pb_port_forwarding_header_dest_compute(
            unit,
            &(info->destination),
            TRUE, /* ITMH 18b encoding */
            &dest
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    

    /* 
     * Insert the entry to direct table 
     */
    SOC_PB_PMF_DIRECT_TBL_DATA_clear(&dt_data);
    dt_data.val = dest;
    tm_port_ndx = SOC_PB_PORT_FORWARDING_HEADER_PORT_NDX_FOR_TDM_RAW_GET(pp_port_ndx);
    entry_offset_ndx = SOC_PB_PORTS_FORWARDING_TDM_RAW_OFFSET_BASE + tm_port_ndx;
    res = soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
            unit,
            entry_offset_ndx,
            &dt_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_set_unsafe()", pp_port_ndx, 0);
}

STATIC
  uint32
    soc_pb_port_forwarding_header_verify(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       pp_port_ndx,
      SOC_SAND_IN  uint8                       is_set
    )
{
  uint32
    tm_port_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, SOC_PB_PP_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

    /*
     *	Verify the TM Port Profile is for Raw ports
     */
    SOC_PB_PORT_PP_PORT_INFO_clear(&pp_port_info);
    res = soc_pb_port_pp_port_get_unsafe(
            unit,
            pp_port_ndx,
            &pp_port_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if ((
		  (pp_port_info.header_type != SOC_PETRA_PORT_HEADER_TYPE_RAW) 
		  && (pp_port_info.header_type != SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW))
		&& (is_set))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_PP_PORT_FOR_RAW_PORTS_ERR, 40, exit);
    }
  }
  else
  {
    tm_port_ndx = SOC_PB_PORT_FORWARDING_HEADER_PORT_NDX_FOR_TDM_RAW_GET(pp_port_ndx);
    SOC_SAND_ERR_IF_ABOVE_MAX(tm_port_ndx, SOC_PB_PORTS_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 36, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_get_verify()", pp_port_ndx, 0);
}



uint32
  soc_pb_port_forwarding_header_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PORTS_FORWARDING_HEADER_INFO, info, 20, exit);

  res = soc_pb_port_forwarding_header_verify(
          unit,
          pp_port_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_set_verify()", pp_port_ndx, 0);
}

uint32
  soc_pb_port_forwarding_header_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_GET_VERIFY);

  res = soc_pb_port_forwarding_header_verify(
          unit,
          pp_port_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_get_verify()", pp_port_ndx, 0);
}

/*********************************************************************
*     Define the Forwarding header parameters for Raw ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    dest,
    trap_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
    fwd_act_profile_tbl_data;
  SOC_PETRA_PORTS_ITMH
    itmh;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
    snoop_tbl_data;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q;
  SOC_PB_PMF_DIRECT_TBL_DATA
    dt_data;
  uint32
    tm_port_ndx,
    entry_offset_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(info);

  /* 
   * Different processing between Raw and TDM_Raw 
   */
  if (pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS)
  {
    /*
     *	1. Apply static TM Port Profile to Trap index mapping:
     *     Use user-defined traps
     */
    res = soc_pb_port_forwarding_header_configuration_get_unsafe(
            unit,
            pp_port_ndx,
            &trap_ndx,
            &fwd_act_profile_tbl_data,
            &snoop_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     *	2. Get the ITMH destination encoding
     */
    soc_petra_PETRA_PORTS_ITMH_clear(&itmh);
    dest = fwd_act_profile_tbl_data.fwd_act_destination;

    info->tr_cls = fwd_act_profile_tbl_data.fwd_act_traffic_class;
    info->dp = fwd_act_profile_tbl_data.fwd_act_drop_precedence;

    info->counter.id = fwd_act_profile_tbl_data.fwd_act_counter_pointer;
    info->counter.processor_id = fwd_act_profile_tbl_data.fwd_act_counter_pointer_selector;

    info->snoop_cmd_ndx = snoop_tbl_data.snoop_action;
  } 
  else /* TDM_RAW */
  {
    /* 
     * Get the entry from the direct table 
     */
    SOC_PB_PMF_DIRECT_TBL_DATA_clear(&dt_data);
    tm_port_ndx = SOC_PB_PORT_FORWARDING_HEADER_PORT_NDX_FOR_TDM_RAW_GET(pp_port_ndx);
    entry_offset_ndx = SOC_PB_PORTS_FORWARDING_TDM_RAW_OFFSET_BASE + tm_port_ndx;
    res = soc_pb_pmf_db_direct_tbl_entry_get_unsafe(
            unit,
            entry_offset_ndx,
            &dt_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    dest = dt_data.val;
  }

  /* 
   * Decode  the destination 
   */
  if (SOC_SAND_GET_BITS_RANGE(dest, 16, 15) == 0x3)
  {
    /* Flow */
    itmh.base.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
    itmh.base.destination.id = SOC_SAND_GET_BITS_RANGE(dest, 14, 0);
  }
  else if (SOC_SAND_GET_BITS_RANGE(dest, 16, 14) == 0x1)
  {
    /* Multicast */
    itmh.base.destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
    itmh.base.destination.id = SOC_SAND_GET_BITS_RANGE(dest, 13, 0);
  }
  else if (SOC_SAND_GET_BITS_RANGE(dest, 16, 12) == 0x1)
  {
    /* LAG */
    itmh.base.destination.type = SOC_PETRA_DEST_TYPE_LAG;
    itmh.base.destination.id = SOC_SAND_GET_BITS_RANGE(dest, 7, 0);
  }
  else if (SOC_SAND_GET_BITS_RANGE(dest, 16, 12) == 0x0)
  {
    /* Port */
    itmh.base.destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
    itmh.base.destination.id = SOC_SAND_GET_BITS_RANGE(dest, 11, 0);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 18, exit);
  }

  if (itmh.base.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
  {
    soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q);

    res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
            unit,
            &base_q
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    if (base_q.queue_id_add_not_decrement)
    {
      itmh.base.destination.id += base_q.base_queue_id;
    }
    else
    {
      itmh.base.destination.id -= base_q.base_queue_id;
    }
  }

  info->destination.type = itmh.base.destination.type;
  info->destination.id = itmh.base.destination.id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_get_unsafe()", pp_port_ndx, 0);
}


uint32
  SOC_PB_PORT_COUNTER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->processor_id, SOC_PB_PORTS_PROCESSOR_ID_MAX, SOC_PB_PORTS_PROCESSOR_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, SOC_PB_PORTS_CNT_ID_MAX, SOC_PB_PORTS_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PORT_COUNTER_INFO_verify()",0,0);
}

uint32
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_dest_info_verify(
          &(info->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  SOC_SAND_ERR_IF_ABOVE_MAX(info->tr_cls, SOC_PETRA_TR_CLS_MAX, SOC_PB_PORTS_TR_CLS_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_PETRA_MAX_DROP_PRECEDENCE, SOC_PB_PORTS_DP_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->snoop_cmd_ndx, SOC_PETRA_MAX_SNOOP_COMMAND_INDEX, SOC_PB_PORTS_SNOOP_CMD_NDX_OUT_OF_RANGE_ERR, 13, exit);
  if (info->counter.processor_id != SOC_PB_CNT_NOF_PROCESSOR_IDS)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_PORT_COUNTER_INFO, &(info->counter), 14, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PORTS_FORWARDING_HEADER_INFO_verify()",0,0);
}

uint32
  SOC_PB_PORT_PP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO *info
  )
{
  uint8
    is_header_tm_related;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_header_size, SOC_PB_PORTS_FIRST_HEADER_SIZE_MAX, SOC_PB_PORTS_FIRST_HEADER_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fc_type, SOC_PB_PORT_PP_PORT_INFO_FC_TYPE_MAX, SOC_PB_PORTS_FC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, SOC_PETRA_PORT_NOF_HEADER_TYPES-1, SOC_PB_PORTS_HEADER_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mirror_profile, SOC_PB_ACTION_NDX_MAX, SOC_PB_ACTION_NDX_OUT_OF_RANGE_ERR, 12, exit);

  /*
   * TM Header type only attributes
   */
  is_header_tm_related = ((info->header_type == SOC_PETRA_PORT_HEADER_TYPE_TM) || (info->header_type == SOC_PETRA_PORT_HEADER_TYPE_INJECTED))? TRUE:FALSE;
  if (
      ((!is_header_tm_related) && (info->is_tm_ing_shaping_enabled == TRUE))
      || ((!is_header_tm_related) && (info->is_tm_pph_present_enabled == TRUE))
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_TM_PORT_IS_PPH_PRESENT_ERR, 20, exit);
  }
  if (
      ((info->header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH) && (info->mirror_profile != SOC_PB_PORT_PP_PORT_SET_IHP_LL_MIRROR_PROFILE_INVALID))
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_ETH_PORT_MIRROR_PROFILE_ERR, 30, exit);
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PORT_PP_PORT_INFO_verify()",0,0);
}

uint32
  soc_pb_ports_mirror_outbound_dflt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  uint8                 is_enable
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    internal_vid_ndx = SOC_PB_PP_OUT_MIRR_VID_DONT_CARE;
  SOC_PETRA_REGS
    *regs = NULL;
  uint8
    is_port_vlan_exist;
  uint32
    fld_idx,
    start_bit,
    start_reg_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_MIRROR_OUTBOUND_DFLT_SET_UNSAFE);

  regs = soc_petra_regs();
  
  is_port_vlan_exist = (is_enable == TRUE) ? 0x1:0x0;

  start_bit = (SOC_PB_PP_OUT_MIRR_VID_DONT_CARE << SOC_PB_PP_OUT_MIRR_VID_SHIFT) | pp_port_ndx;
  start_reg_idx  = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
  fld_idx        = SOC_PETRA_FLD_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);

  SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[start_reg_idx].mirror_enable , fld_val, 10, exit);
  SOC_SAND_SET_BIT(fld_val, is_enable, fld_idx);
  SOC_PB_FLD_SET(regs->epni.mirror_enable_reg[start_reg_idx].mirror_enable , fld_val, 92, exit);

  /* Enable/Disable mirror for all port x internal_vids that are not specific configured. */
  for (internal_vid_ndx = 0; internal_vid_ndx < SOC_PB_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES; internal_vid_ndx++)
  {
    res = soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_get(
            unit,
            pp_port_ndx,
            internal_vid_ndx,
            &is_port_vlan_exist
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    if (is_port_vlan_exist == FALSE)
    {
      start_bit = (internal_vid_ndx << SOC_PB_PP_OUT_MIRR_VID_SHIFT) | pp_port_ndx;
      start_reg_idx  = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
      fld_idx        = SOC_PETRA_FLD_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);

      SOC_PB_FLD_GET(regs->epni.mirror_enable_reg[start_reg_idx].mirror_enable , fld_val, 90, exit);
      SOC_SAND_SET_BIT(fld_val, is_enable, fld_idx);
      SOC_PB_FLD_SET(regs->epni.mirror_enable_reg[start_reg_idx].mirror_enable , fld_val, 92, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_mirror_outbound_dflt_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_mirror_outbound_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    reg_buff[2],
    reg_val,
    fld_val,
    is_mirror_enabled_val = SOC_SAND_BOOL2NUM(info->enable);
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_data;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    ifp_mapping,
    out_mapping;
  uint32
    ch_id,
    start_reg_idx,
    end_reg_idx,
    start_bit,
    end_bit,
    pp_port_in_idx,
    pp_port_out_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_MIRROR_OUTBOUND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  reg_buff[0] = 0;
  reg_buff[1] = 0;

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
    res = soc_petra_port_to_interface_map_get_unsafe(
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

    SOC_PETRA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_buff[0] ,30 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PETRA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_buff[1] ,40 , exit);
    }

    fld_val = (uint32)ch_id;
    fld.lsb = (uint8)start_bit;
    fld.msb = (uint8)end_bit;

    SOC_PB_IMPLICIT_FLD_TO_REG(fld, fld_val, reg_buff[0], 50, exit);

    /* EPNI */
    SOC_PETRA_REG_SET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_buff[0] ,60 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PETRA_REG_SET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_buff[1] ,70 , exit);
    }
  }

  /* EPNI - Egress mirrroring enable/disable */
  res = soc_pb_port_to_pp_port_map_get_unsafe(
          unit,
          ofp_ndx,
          &pp_port_in_idx,
          &pp_port_out_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  if (info->skip_port_deafult_enable == FALSE) 
  {    
    res = soc_pb_ports_mirror_outbound_dflt_set_unsafe(
            unit,
            pp_port_out_idx,
            info->enable
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

  /* EGQ - PCT */
  res = soc_pb_egq_pct_tbl_get_unsafe(
          unit,
          ofp_ndx,
          &(egq_pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  egq_pct_data.outbound_mirr = is_mirror_enabled_val;

  res = soc_pb_egq_pct_tbl_set_unsafe(
          unit,
          ofp_ndx,
          &(egq_pct_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /* IDR */
  start_bit = info->ifp_id;
  start_reg_idx = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
  SOC_PB_REG_GET(regs->idr.context_outbound_mirror_reg[start_reg_idx], reg_val, 120, exit);
  SOC_SAND_SET_BIT(reg_val, is_mirror_enabled_val, start_bit);
  SOC_PB_REG_SET(regs->idr.context_outbound_mirror_reg[start_reg_idx], reg_val, 122, exit);

  /* IRR */
  start_bit = info->ifp_id;
  start_reg_idx = SOC_PETRA_REG_IDX_GET(start_bit, SOC_SAND_REG_SIZE_BITS);
  SOC_PB_REG_GET(regs->irr.port_is_outbound_mirror_reg[start_reg_idx], reg_val, 130, exit);
  SOC_SAND_SET_BIT(reg_val, is_mirror_enabled_val, start_bit);
  SOC_PB_REG_SET(regs->irr.port_is_outbound_mirror_reg[start_reg_idx], reg_val, 132, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_mirror_outbound_set_unsafe()", ofp_ndx, 0);
}

/*********************************************************************
*     Get outbound mirroring configuration for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_mirror_outbound_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  uint32
    res,
    reg_val[2],
    fld_val;
  SOC_PB_EGQ_PCT_TBL_DATA
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORTS_MIRROR_OUTBOUND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_clear(info);

  reg_val[0] = 0;
  reg_val[1] = 0;

  /* EGQ - PCT */
  res = soc_pb_egq_pct_tbl_get_unsafe(
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

    SOC_PETRA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[start_reg_idx],reg_val[0] ,60 , exit);
    if (end_reg_idx != start_reg_idx)
    {
      SOC_PETRA_REG_GET(regs->epni.map_ofp_to_mirr_channel_reg[end_reg_idx],reg_val[1] ,70 , exit);
    }

    fld.lsb = (uint8)start_bit;
    fld.msb = (uint8)end_bit;

    SOC_PB_IMPLICIT_FLD_FROM_REG(fld, fld_val, (reg_val[0]), 80, exit);

    ch_id = (uint32)fld_val;

    /*
     *  Find out the IFP mapped to this RCY channel
     */
    for (port_idx = 0; port_idx < SOC_PETRA_NOF_FAP_PORTS; port_idx++)
    {
      res = soc_pb_port_to_interface_map_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ports_mirror_outbound_get_unsafe()",0,0);
}

/*********************************************************************
*     Allocate and return a free recycle interface channel and a free reassembly
*     context, by allocating an ITM port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
soc_pb_ports_reassembly_context_and_recycle_channel_alloc_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_OUT uint32 *context, 
    SOC_SAND_OUT uint32 *channel)
{
  int rv, port;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(context);

  rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_allocate_tm_port_in_range_and_recycle_channel,(unit, SOC_PETRA_NOF_FAP_PORTS, &port, channel));
  if (rv == SOC_E_NONE && port >=0) {
      *context = port; /* assume 1-1 mapping of ports to reassembly contexts */
  } else {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OUTBND_MIRR_IFP_NOT_MAPPED_TO_RCY_IF_ERR, 10, exit); 
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ports_reassembly_context_and_recycle_channel_alloc_unsafe()", 0, 0);
}

/*********************************************************************
*     release a used reassembly context, by releasing its ITM port, also release 
*     its reassembly context.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
soc_pb_ports_reassembly_context_and_recycle_channel_free_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 context)
{
  int rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_free_tm_port_and_recycle_channel,(unit, context));
  if (rv != SOC_E_NONE) {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PORTS_NOT_TM_PORT_ERR, 10, exit); 
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ports_reassembly_context_and_mirror_channel_free_unsafe()", 0, 0);
}


#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


