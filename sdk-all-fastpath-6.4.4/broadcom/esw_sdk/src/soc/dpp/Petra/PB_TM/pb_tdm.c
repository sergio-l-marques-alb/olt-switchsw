/* $Id: soc_pb_tdm.c,v 1.5 Broadcom SDK $
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

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tdm.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_fabric.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_TDM_PORT_NDX_MAX                                    (79)
#define SOC_PB_TDM_CELL_SIZE_MIN                                   (65)
#define SOC_PB_TDM_CELL_SIZE_MAX                                   (128)
#define SOC_PB_TDM_IS_TDM_MAX                                      (2)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_MAX                         (31)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_MAX                     (1*1024-1)
#define SOC_PB_TDM_FTMH_OPT_MC_MC_ID_MAX                           (16*1024-1)
#define SOC_PB_TDM_FTMH_STANDARD_UC_SYS_PHY_PORT_MAX               (4*1024-1)
#define SOC_PB_TDM_FTMH_STANDARD_MC_MC_ID_MAX                      (16*1024-1)
#define SOC_PB_TDM_FTMH_STANDARD_MC_USER_DEF_MAX                   (8192*1024 - 1)
#define SOC_PB_TDM_FTMH_INFO_ACTION_ING_MAX                        (SOC_PB_TDM_NOF_ING_ACTIONS-1)
#define SOC_PB_TDM_FTMH_INFO_ACTION_EG_MAX                         (SOC_PB_TDM_NOF_EG_ACTIONS-1)
#define SOC_PB_TDM_FRAGMENT_NUM                                    (0x180)
#define SOC_PB_TDM_FTMH_OPT_TYPE_UC                                (0)
#define SOC_PB_TDM_FTMH_OPT_TYPE_MC                                (1)

#define SOC_PB_TDM_FTMH_OPT_MODE_VAL_FLD                           (0x2)
#define SOC_PB_TDM_FTMH_STA_MODE_VAL_FLD                           (0x1)
#define SOC_PB_TDM_FTMH_UNCHANGED_MODE_VAL_FLD                     (0x0)
#define SOC_PB_TDM_FTMH_EXTERNAL_MODE_VAL_FLD                      (0x3)
#define SOC_PB_TDM_VERSION_ID                                      (0x2)
#define SOC_PB_TDM_MC_ID_ROUTE_NDX_MAX                             (15)
/* $Id: soc_pb_tdm.c,v 1.5 Broadcom SDK $
 * Define of fields inside the header
 */
#define SOC_PB_TDM_FTMH_OPT_TYPE_START_BIT                         (63)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_LSB                         (58)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_MSB                         (62)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS                    (SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_MSB - SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_LSB + 1)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB                     (48)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_MSB                     (57)
#define SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS                (SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_MSB - SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB + 1)

#define SOC_PB_TDM_FTMH_OPT_MC_MC_ID_LSB                           (48)
#define SOC_PB_TDM_FTMH_OPT_MC_MC_ID_MSB                           (61)
#define SOC_PB_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS                      (SOC_PB_TDM_FTMH_OPT_MC_MC_ID_MSB - SOC_PB_TDM_FTMH_OPT_MC_MC_ID_LSB + 1)


#define SOC_PB_TDM_FTMH_STA_TYPE_START_BIT                         (16)
#define SOC_PB_TDM_FTMH_STA_VERSION_LSB                            (62)
#define SOC_PB_TDM_FTMH_STA_VERSION_MSB                            (63)
#define SOC_PB_TDM_FTMH_STA_VERSION_NOF_BITS                       (SOC_PB_TDM_FTMH_STA_VERSION_MSB - SOC_PB_TDM_FTMH_STA_VERSION_LSB + 1)

#define SOC_PB_TDM_FTMH_STA_TYPE_UC                                (0)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB                  (0)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_MSB                  (15)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS             (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_MSB - SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT        (0)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB                  (19)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_MSB                  (23)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS             (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_MSB - SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT        (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB                  (32)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_MSB                  (42)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS             (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_MSB - SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT        (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT + SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)
/* nof bits part 1-2 */
#define SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2      (SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS + SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)

#define SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB                    (24)
#define SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_MSB                    (31)
#define SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS               (SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_MSB - SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB + 1)

#define SOC_PB_TDM_FTMH_STA_UC_FAP_ID_LSB                         (48)
#define SOC_PB_TDM_FTMH_STA_UC_FAP_ID_MSB                         (58)
#define SOC_PB_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS                    (SOC_PB_TDM_FTMH_STA_UC_FAP_ID_MSB - SOC_PB_TDM_FTMH_STA_UC_FAP_ID_LSB + 1)


#define SOC_PB_TDM_FTMH_STA_TYPE_MC                                (1)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB                  (14)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_MSB                  (15)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS             (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_MSB - SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT        (0)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB                  (19)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_MSB                  (20)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS             (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_MSB - SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT        (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB                  (21)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_MSB                  (23)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS             (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_MSB - SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT        (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT + SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB                  (32)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_MSB                  (47)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS             (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_MSB - SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT        (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT + SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)
/* nof bits parts 1-2 */
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2      (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS + SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)
/* nof bits parts 1-3 */
#define SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3      (SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2 + SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)

#define SOC_PB_TDM_FTMH_STA_MC_MC_ID_LSB                           (0)
#define SOC_PB_TDM_FTMH_STA_MC_MC_ID_MSB                           (13)
#define SOC_PB_TDM_FTMH_STA_MC_MC_ID_NOF_BITS                      (SOC_PB_TDM_FTMH_STA_MC_MC_ID_MSB - SOC_PB_TDM_FTMH_STA_MC_MC_ID_LSB + 1)
#define SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_LSB                    (24)
#define SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_MSB                    (31)
#define SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_NOF_BITS               (SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_MSB - SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_LSB + 1)






/* } */
/*************
 * MACROS    *
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
*     soc_pb_tdm_init
* FUNCTION:
*     Initialization of the TDM configuration depends on the tdm mode.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_tdm_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res;
  uint32
    i;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
  SOC_PETRA_FABRIC_CONNECT_MODE
    fabric_mode;
  uint8
    is_local;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_INIT);

  regs = soc_petra_regs();

  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);
  
  /* General configuration - tdm mode */
  fld_val = (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET)?0x0:0x1;
  SOC_PB_FLD_SET(regs->eci.general_controls_reg.tdm_mode, fld_val, 10, exit);

  /* TDM general configuration */
  
  if (!(tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET))
  {
    /* Enable tdm cell mode */
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.egress_tdm_mode, fld_val, 20, exit);

    /* Enable 2 bytes ftmh only in optimize ftmh mode */
    fld_val = (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)?0x1:0x0;
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.tdm_2bytes_ftmh, fld_val, 30, exit);
    SOC_PB_FLD_SET(regs->egq.tdm_general_configuration_reg.tdm_2bytes_ftmh, fld_val, 33, exit);
    SOC_PB_FLD_SET(regs->epni.tdm_epe_configuration_reg.tdm_2bytes_ftmh, fld_val, 35, exit);

    /* enable EGQ shaper, only in optimized FTMH mode.  */
    SOC_PB_FLD_SET(regs->egq.tdm_general_configuration_reg.tdm_2bytes_shaper_size_en, fld_val, 37, exit);
    
  }
  else
  {
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.tdm_pkt_mode_en, fld_val, 43, exit);

    fld_val = 0x0;
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.egress_tdm_mode, fld_val, 45, exit);
  }

  /* Fragment number */

  if (!soc_petra_sw_db_is_fap20_in_system_get(unit))
  {
    /* Enable EGQ frag-num */
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->egq.tdm_frag_num_config1_reg.tdm_fdr_frag_num1_en, fld_val, 50, exit);

    fld_val = SOC_PB_TDM_FRAGMENT_NUM;
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.tdm_frg_num, fld_val, 55, exit);

    SOC_PB_FLD_SET(regs->egq.tdm_frag_num_config1_reg.tdm_fdr_frag_num1_num, fld_val, 60, exit);
  }

  
  /* TDM packet size limit range 65-128 */
  fld_val = SOC_PB_TDM_CELL_SIZE_MIN;
  SOC_PB_FLD_SET(regs->ire.tdm_size_reg.tdm_min_size, fld_val, 63,exit);
  
  fld_val = SOC_PB_TDM_CELL_SIZE_MAX;
  SOC_PB_FLD_SET(regs->ire.tdm_size_reg.tdm_max_size, fld_val, 65,exit);
    
  /* FDT TDM configuration */
  /* Enable cell size different than 128B */
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->fdt.tdm_and_link_load_configuration_reg.tdm_dif_size_en, fld_val, 70,exit);

  /* Send TDM packets from IPT only in packet mode */
  fld_val = (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET) ? 0x1:0x0;
  SOC_PB_FLD_SET(regs->fdt.tdm_and_link_load_configuration_reg.ipt_tdm_dif_size_en, fld_val, 75,exit);
  
  /* TDM cells with no CRC */
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->egq.tdm_general_configuration_reg.tdm_strip_fabric_crc_en, fld_val, 80, exit);
  
  if (!(tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET))
  {
    /* On init, assumes all ports are TDM ports */
    
    /* CRC removal */
    /* In TDM cell mode, TDM packets with no CRC */
    for (i = 0; i < SOC_PB_EPNI_TDM_EN_CRC_PER_PORT_REG_MULT_NOF_REGS; i++)
    {
      SOC_PB_REG_SET(regs->epni.tdm_en_crc_per_port_reg[i], 0x0, 85, exit);
    }

    /* In TMD cell mode, Map all ofp to TDM ports */
    for (i = 0; i < SOC_PB_EGQ_OFP2TDM_MAP_REG_MULT_NOF_REGS; i++)
    {
      SOC_PB_REG_SET(regs->egq.ofp2tdm_map_reg[i], 0xffffffff, 90, exit);
    }
    for (i = 0; i < SOC_PB_EPNI_OFP2TDM_MAP_REG_MULT_NOF_REGS; i++)
    {
      SOC_PB_REG_SET(regs->epni.ofp2tdm_map_reg[i], 0xffffffff, 95, exit);
    }
  }
  
  if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET)
  {
    /* IPT recognize TDM packets */

    /* IRE FTMH version for TDM packet to identify the packets as TDM flows. */
    fld_val = SOC_PB_TDM_VERSION_ID;
    SOC_PB_FLD_SET(regs->ire.dynamic_configuration_reg.ftmh_version,fld_val, 120, exit);
    
    /* IPT TDM enable */
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->ipt.ipt_enables_reg.tdm_en, fld_val, 125, exit);
    
    /* Strip crc for TDM packets */
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->ipt.ipt_enables_reg.tdm_strip_fabric_crc_en, fld_val, 130,exit);

    /* Enable push queue for TDM packets */
    fld_val = SOC_PB_TDM_PUSH_QUEUE_TYPE;
    SOC_PB_FLD_SET(regs->ips.push_queue_types_config_reg.push_queue_type, fld_val, 135, exit);
    
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->ips.push_queue_types_config_reg.push_queue_type_en, fld_val, 140, exit);
  }

  /*
   * In NON Fabric mode, enable traffic tdm local only.
   * Note that fabric module must be initialize before TDM module
   */
  if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
  {
    res = soc_petra_fabric_connect_mode_get(
      unit,
      &fabric_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    is_local =
      (fabric_mode == SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP)? TRUE:FALSE;

    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.force_all_local, is_local, 160, exit);
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_tdm_init()",0,0);
}

uint32
  soc_pb_tdm_ing_ftmh_fill_header(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PB_TDM_FTMH                 *ftmh,
    SOC_SAND_IN     SOC_PB_TDM_FTMH_INFO_MODE       ftmh_mode,
    SOC_SAND_OUT    SOC_PB_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    tmp = 0,
    res = SOC_SAND_OK;
  uint32
    mapped_fap_port_id = 0,
    mapped_fap_id = 0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_ING_FTMH_FILL_HEADER);

  SOC_SAND_CHECK_NULL_INPUT(ftmh);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  switch(ftmh_mode)
  {
    case SOC_PB_TDM_FTMH_INFO_MODE_OPT_UC:
      /* Optimized & UC */
      tmp = SOC_PB_TDM_FTMH_OPT_TYPE_UC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      tmp = ftmh->opt_uc.dest_if;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_LSB,
        SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      tmp = ftmh->opt_uc.dest_fap_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB,
        SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      break;
    case SOC_PB_TDM_FTMH_INFO_MODE_OPT_MC:
      /* Optimized & MC */
      tmp = SOC_PB_TDM_FTMH_OPT_TYPE_MC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tmp = ftmh->opt_mc.mc_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_OPT_MC_MC_ID_LSB,
        SOC_PB_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
      break;
    case SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_UC:
      /* Standard & UC */
      tmp = SOC_PB_TDM_FTMH_STA_TYPE_UC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      tmp = 0;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_VERSION_LSB,
        SOC_PB_TDM_FTMH_STA_VERSION_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

      res = soc_petra_sys_phys_to_local_port_map_get(unit, ftmh->standard_uc.sys_phy_port, &mapped_fap_id, &mapped_fap_port_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

      tmp = mapped_fap_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_UC_FAP_ID_LSB,
        SOC_PB_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

      tmp = mapped_fap_port_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB,
        SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /* User defined */
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
      
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB,
        SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      break;
    case SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_MC:
      /* Standard & MC */
      tmp = SOC_PB_TDM_FTMH_STA_TYPE_MC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

      tmp = 0;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_VERSION_LSB,
        SOC_PB_TDM_FTMH_STA_VERSION_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      tmp = ftmh->standard_mc.mc_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_MC_ID_LSB,
        SOC_PB_TDM_FTMH_STA_MC_MC_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

      tmp = 0xff;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_LSB,
        SOC_PB_TDM_FTMH_STA_MC_INTERNAL_USE_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);

      /* User defined */
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 78, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB,
        SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      break;
    default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ing_ftmh_fill_header()", 0, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ing_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_ING_ACTION           action_ing,
    SOC_SAND_IN  SOC_PB_TDM_FTMH                 *ftmh,
    SOC_SAND_IN  uint8                   is_mc
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IRE_TDM_CONFIG_TBL_DATA
    soc_pb_ire_tdm_config_tbl_data;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
  SOC_PETRA_PORT_HEADER_TYPE
    incoming_header_type,
    outgoing_header_type;
  SOC_PB_TDM_FTMH_INFO_MODE
    ftmh_mode = SOC_PB_TDM_FTMH_INFO_MODE_OPT_UC;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_ING_FTMH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ftmh);

  res = SOC_SAND_OK; sal_memset(
    &soc_pb_ire_tdm_config_tbl_data,
    0,
    sizeof(SOC_PB_IRE_TDM_CONFIG_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = soc_pb_ire_tdm_config_tbl_get_unsafe(
    unit,
    port_ndx,
    &soc_pb_ire_tdm_config_tbl_data
    );

  /* CPU port, no limited packet size */
  res = soc_pb_port_header_type_get_unsafe(
          unit,
          port_ndx,
          &incoming_header_type,
          &outgoing_header_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  
  if (incoming_header_type == SOC_PETRA_PORT_HEADER_TYPE_CPU)
  {
    soc_pb_ire_tdm_config_tbl_data.cpu = 0x1;
  }

  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);

  switch(action_ing)
  {
    case SOC_PB_TDM_ING_ACTION_ADD:
      if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
      {
        soc_pb_ire_tdm_config_tbl_data.mode = SOC_PB_TDM_FTMH_OPT_MODE_VAL_FLD;
        ftmh_mode = (is_mc == FALSE) ? SOC_PB_TDM_FTMH_INFO_MODE_OPT_UC:SOC_PB_TDM_FTMH_INFO_MODE_OPT_MC;
      }
      else
      {
        soc_pb_ire_tdm_config_tbl_data.mode = SOC_PB_TDM_FTMH_STA_MODE_VAL_FLD;
        ftmh_mode = (is_mc == FALSE) ? SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_UC:SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_MC;
      }
      break;
    case SOC_PB_TDM_ING_ACTION_CUSTOMER_EMBED:
      /* Same as ADD, but different mode */
      soc_pb_ire_tdm_config_tbl_data.mode = SOC_PB_TDM_FTMH_EXTERNAL_MODE_VAL_FLD;
      if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
      {
        ftmh_mode = (is_mc == FALSE) ? SOC_PB_TDM_FTMH_INFO_MODE_OPT_UC:SOC_PB_TDM_FTMH_INFO_MODE_OPT_MC;
      }
      else
      {
        ftmh_mode = (is_mc == FALSE) ? SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_UC:SOC_PB_TDM_FTMH_INFO_MODE_STANDARD_MC;
      }
      break;
    case SOC_PB_TDM_ING_ACTION_NO_CHANGE:
      soc_pb_ire_tdm_config_tbl_data.mode = SOC_PB_TDM_FTMH_UNCHANGED_MODE_VAL_FLD;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_ACTION_ING_OUT_OF_RANGE_ERR, 15, exit);
  }

  if (action_ing != SOC_PB_TDM_ING_ACTION_NO_CHANGE)
  {
    res = soc_pb_tdm_ing_ftmh_fill_header(
      unit,
      ftmh,
      ftmh_mode,
      &soc_pb_ire_tdm_config_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_pb_ire_tdm_config_tbl_set_unsafe(
          unit,
          port_ndx,
          &soc_pb_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ing_ftmh_set_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_eg_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_EG_ACTION            action_eg
  )
{
  uint32
    port_profile = 0,
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_EG_FTMH_SET_UNSAFE);
  
  switch(action_eg)
  {
    case SOC_PB_TDM_EG_ACTION_REMOVE:
      port_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1;
      break;
    case SOC_PB_TDM_EG_ACTION_CUSTOMER_EXTRACT:
      port_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM2;
      break;
    case SOC_PB_TDM_EG_ACTION_NO_CHANGE:
      port_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU;
      break;
    default:
      break;
  }
  
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  profile_params.eg_profile = port_profile;
  profile_ndx.port_ndx = port_ndx;
  res = soc_pb_profile_add(
          unit,
          SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
          &profile_ndx,
          &profile_params,
          FALSE,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (success != SOC_SAND_SUCCESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PORTS_EG_TM_PROFILE_FULL_ERR, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_eg_ftmh_set_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress and egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  res = soc_pb_tdm_ing_ftmh_set_unsafe(
          unit,
          port_ndx,
          info->action_ing,
          &info->ftmh,
          info->is_mc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_tdm_eg_ftmh_set_unsafe(
          unit,
          port_ndx,
          info->action_eg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ftmh_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_SET_VERIFY);

  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  /* PMM only in standard mode */
  if (info->action_ing == SOC_PB_TDM_ING_ACTION_CUSTOMER_EMBED
    &&  tdm_mode != SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_CUSTOMER_EMBED_IN_OPTIMIZED_MODE_ERR, 7, exit);
  }
  SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_FTMH_INFO, info, 20, exit);
  /* API relevant only in tdm traffic mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(tdm_mode,SOC_PETRA_MGMT_TDM_MODE_TDM_OPT, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);

  /* Verify struct SOC_PB_TDM_FTMH */
  SOC_SAND_CHECK_NULL_INPUT(&(info->ftmh));

  switch (tdm_mode)
  {
  case SOC_PETRA_MGMT_TDM_MODE_TDM_OPT:
    if (!info->is_mc)
    {
      SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_FTMH_OPT_UC, &(info->ftmh.opt_uc), 10, exit);
    }
    else
    {
      SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_FTMH_OPT_MC, &(info->ftmh.opt_mc), 11, exit);
    }
  	break;
  case SOC_PETRA_MGMT_TDM_MODE_TDM_STA:
    if (!info->is_mc)
    {
      SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_FTMH_STANDARD_UC, &(info->ftmh.standard_uc), 12, exit);
    }
    else
    {
      SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_FTMH_STANDARD_MC, &(info->ftmh.standard_mc), 13, exit);
    }
    break;
  default:
    break;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ftmh_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  /* API relevant only in tdm traffic mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(soc_petra_sw_db_tdm_mode_get(unit),SOC_PETRA_MGMT_TDM_MODE_TDM_OPT, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ing_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_ING_ACTION           *action_ing,
    SOC_SAND_OUT SOC_PB_TDM_FTMH                 *ftmh,
    SOC_SAND_OUT uint8                   *is_mc
  )
{
  uint32
    tmp = 0,
    res = SOC_SAND_OK;
  SOC_PB_IRE_TDM_CONFIG_TBL_DATA
    soc_pb_ire_tdm_config_tbl_data;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
  uint32
    mapped_fap_id = 0,
    mapped_fap_port_id = 0,
    tmp_sys = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_ING_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_ing);
  SOC_SAND_CHECK_NULL_INPUT(ftmh);
  SOC_SAND_CHECK_NULL_INPUT(is_mc);

  res = SOC_SAND_OK; sal_memset(
          &soc_pb_ire_tdm_config_tbl_data,
          0,
          sizeof(SOC_PB_IRE_TDM_CONFIG_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = soc_pb_ire_tdm_config_tbl_get_unsafe(
          unit,
          port_ndx,
          &soc_pb_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch (soc_pb_ire_tdm_config_tbl_data.mode)
  {
    case SOC_PB_TDM_FTMH_EXTERNAL_MODE_VAL_FLD:
      *action_ing = SOC_PB_TDM_ING_ACTION_CUSTOMER_EMBED;
      break;
    case SOC_PB_TDM_FTMH_UNCHANGED_MODE_VAL_FLD:
      *action_ing = SOC_PB_TDM_ING_ACTION_NO_CHANGE;
      break;
    case SOC_PB_TDM_FTMH_OPT_MODE_VAL_FLD:
    case SOC_PB_TDM_FTMH_STA_MODE_VAL_FLD:
      *action_ing = SOC_PB_TDM_ING_ACTION_ADD;
      break;
  }
  
  if (*action_ing != SOC_PB_TDM_ING_ACTION_NO_CHANGE)
  {
    tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);

    if (soc_pb_ire_tdm_config_tbl_data.mode == SOC_PB_TDM_FTMH_OPT_MODE_VAL_FLD)
    {
      /* Verify tdm mode is right */
      if (tdm_mode != SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 23, exit);
      }
     
      res = soc_sand_bitstream_get_any_field(
        &(soc_pb_ire_tdm_config_tbl_data.header[0]),
        SOC_PB_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tmp)
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      if (tmp == SOC_PB_TDM_FTMH_OPT_TYPE_UC)
      {
        /* Optimized & UC */
        *is_mc = FALSE;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB,
          SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS,
          &(ftmh->opt_uc.dest_fap_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_LSB,
          SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS,
          &(ftmh->opt_uc.dest_if)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
      }
      else
      {
        /* Optimized & MC */
        *is_mc = TRUE;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_OPT_MC_MC_ID_LSB,
          SOC_PB_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS,
          &(ftmh->opt_mc.mc_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
      }
      
    }
    else
    {
      /* Verify tdm mode is right */
      if (tdm_mode != SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 33, exit);
      }

      res = soc_sand_bitstream_get_any_field(
        &soc_pb_ire_tdm_config_tbl_data.header[0],
        SOC_PB_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tmp)
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

      if (tmp == SOC_PB_TDM_FTMH_STA_TYPE_UC)
      {
        /* Standard & UC */
        *is_mc = FALSE;

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_UC_FAP_ID_LSB,
          SOC_PB_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS,
          &tmp
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

        mapped_fap_id = tmp;

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB,
          SOC_PB_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS,
          &tmp
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
        mapped_fap_port_id = tmp;

        tmp_sys = 0;
        res = soc_petra_local_to_sys_phys_port_map_get(unit, mapped_fap_id, mapped_fap_port_id, &tmp_sys);
        SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

        ftmh->standard_uc.sys_phy_port = tmp_sys;
        /* User defined */
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
          &(ftmh->standard_uc.user_def)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
        
        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
          &(ftmh->standard_uc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2,
          SOC_PB_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
          &(ftmh->standard_uc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
      }
      else
      {
        /* Standard & MC */
        *is_mc = TRUE;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_MC_MC_ID_LSB,
          SOC_PB_TDM_FTMH_STA_MC_MC_ID_NOF_BITS,
          &(ftmh->standard_mc.mc_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

        /* User defined */
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
          &(ftmh->standard_mc.user_def)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &soc_pb_ire_tdm_config_tbl_data.header[0],
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3,
          SOC_PB_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
      }

    }
    
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ing_ftmh_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_eg_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_EG_ACTION            *action_eg
  )
{
  uint32
    internal_profile,
    res = SOC_SAND_OK;
  SOC_PB_EGR_PROG_TM_PORT_PROFILE
    port_profile;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_EG_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_eg);

  res = soc_pb_egr_prog_editor_profile_get(
          unit,
          port_ndx,
          &port_profile,
          &internal_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  switch(port_profile)
  {
  case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1:
    *action_eg = SOC_PB_TDM_EG_ACTION_REMOVE;
    break;
  case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM2:
    *action_eg = SOC_PB_TDM_EG_ACTION_CUSTOMER_EXTRACT;
    break;
  case SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU:
    *action_eg = SOC_PB_TDM_EG_ACTION_NO_CHANGE;
    break;
  default:
    *action_eg = SOC_PB_TDM_NOF_EG_ACTIONS;
    break;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ing_ftmh_get_unsafe()", port_ndx, 0);
}
/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress and egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_TDM_FTMH_INFO_clear(info);

  res = soc_pb_tdm_ing_ftmh_get_unsafe(
          unit,
          port_ndx,
          &info->action_ing,
          &info->ftmh,
          &info->is_mc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_tdm_eg_ftmh_get_unsafe(
          unit,
          port_ndx,
          &info->action_eg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Optimized FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_SET_UNSAFE);
  
  regs = soc_petra_regs();

  /* Set optimized TDM packets */
  fld_val = cell_size;
  SOC_PB_FLD_SET(regs->egq.tdm_ehp_configuration_reg.tdm_2bytes_pkt_size, fld_val, 10, exit);
  SOC_PB_FLD_SET(regs->epni.tdm_epe_configuration_reg.tdm_2bytes_pkt_size, fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_set_unsafe()", 0, 0);
}

uint32
  soc_pb_tdm_opt_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cell_size, SOC_PB_TDM_CELL_SIZE_MIN, SOC_PB_TDM_CELL_SIZE_MAX, SOC_PB_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  
  /* This API is relevent only in optimized mode. */
  if (soc_petra_sw_db_tdm_mode_get(unit) != SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_set_verify()", 0, 0);
}

uint32
  soc_pb_tdm_opt_size_get_verify(
    SOC_SAND_IN  int                   unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_GET_VERIFY);
  
  /* This API is relevent only in optimized mode. */
  if (soc_petra_sw_db_tdm_mode_get(unit) != SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_get_verify()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Optimized FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cell_size);
  
  regs = soc_petra_regs();
  
  SOC_PB_FLD_GET(regs->egq.tdm_ehp_configuration_reg.tdm_2bytes_pkt_size, fld_val, 10, exit);
  
  *cell_size = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Standard FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  regs = soc_petra_regs();

  fld_val = size_range->start;
  SOC_PB_FLD_SET(regs->ire.tdm_size_reg.tdm_min_size, fld_val, 20,exit);

  fld_val = size_range->end;
  SOC_PB_FLD_SET(regs->ire.tdm_size_reg.tdm_max_size, fld_val, 30,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_set_unsafe()", 0, 0);
}

uint32
  soc_pb_tdm_stand_size_range_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(size_range->end, SOC_PB_TDM_CELL_SIZE_MAX, SOC_PB_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(size_range->start, SOC_PB_TDM_CELL_SIZE_MIN, SOC_PB_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 15, exit);

  /* This API is relevant only in standard mode. */
  if (soc_petra_sw_db_tdm_mode_get(unit) != SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_set_verify()", 0, 0);
}

uint32
  soc_pb_tdm_stand_size_range_get_verify(
    SOC_SAND_IN  int                   unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_GET_VERIFY);
  
  /* This API is relevant only in standard mode. */
  if (soc_petra_sw_db_tdm_mode_get(unit) != SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_get_verify()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Standard FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);
  
  regs = soc_petra_regs();

  
  SOC_PB_FLD_GET(regs->ire.tdm_size_reg.tdm_min_size, fld_val, 20,exit);
  size_range->start = fld_val;

  SOC_PB_FLD_GET(regs->ire.tdm_size_reg.tdm_max_size, fld_val, 30,exit);
  size_range->end = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the OFP ports configured as TDM destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ofp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_val;
  uint32
    indx,
    inner_port,
    i;
  uint8
    is_exists_tdm_port = FALSE;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
  SOC_PETRA_REGS
    *regs;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OFP_SET_UNSAFE);
  
  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);
  regs = soc_petra_regs();

  indx = port_ndx / SOC_SAND_REG_SIZE_BITS;
  inner_port = port_ndx - SOC_SAND_REG_SIZE_BITS*indx;
  
  SOC_PB_FLD_GET(regs->egq.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val,10,exit)

  tmp = is_tdm;
  res = soc_sand_bitstream_set_any_field(
    &tmp,
    inner_port,
    1,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  SOC_PB_FLD_SET(regs->egq.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val,15,exit);
  
  SOC_PB_FLD_GET(regs->epni.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val,20,exit);
  tmp = is_tdm;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          inner_port,
          1,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

  SOC_PB_FLD_SET(regs->epni.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val,25,exit);

  /* Fabric CRC Disable if TDM enable */
  SOC_PB_FLD_GET(regs->epni.tdm_en_crc_per_port_reg[indx].tdm_en_crc_per_port,fld_val, 30,exit);
  tmp = SOC_SAND_NUM2BOOL_INVERSE(is_tdm);
  res = soc_sand_bitstream_set_any_field(
    &tmp,
    inner_port,
    1,
    &fld_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  SOC_PB_FLD_SET(regs->epni.tdm_en_crc_per_port_reg[indx].tdm_en_crc_per_port,fld_val,35,exit);

  /* In packet mode, Change TDM enable in EGQ if exists port type tdm */
  if ((tdm_mode == SOC_PETRA_MGMT_TDM_MODE_PACKET))
  {
  
    is_exists_tdm_port = is_tdm;
    if (is_exists_tdm_port == FALSE)
    {
      /* Run over all ports and check if port is in tdm type. if not, disable EGQ TDM mode. */
      for (i = 0; i < SOC_PB_EGQ_OFP2TDM_MAP_REG_MULT_NOF_REGS; i++)
      {
        fld_val = 0;
        SOC_PB_FLD_GET(regs->egq.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val,15,exit);
        if (fld_val != 0)
        {
          is_exists_tdm_port = TRUE;
        }
      }
    }
    SOC_PB_FLD_SET(regs->eci.tdm_configuration_reg.egress_tdm_mode,is_exists_tdm_port,40,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ofp_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ofp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OFP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_tdm, SOC_PB_TDM_IS_TDM_MAX, SOC_PB_TDM_IS_TDM_OUT_OF_RANGE_ERR, 20, exit);
  
  /* API relevant only in packet mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(soc_petra_sw_db_tdm_mode_get(unit),SOC_PETRA_MGMT_TDM_MODE_PACKET, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ofp_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ofp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OFP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* API relevant only in packet mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(soc_petra_sw_db_tdm_mode_get(unit),SOC_PETRA_MGMT_TDM_MODE_PACKET, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ofp_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the IFP ports configured as TDM destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ofp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_val;
  uint32
    inner_port,
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OFP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_tdm);
  
  regs = soc_petra_regs();

  indx = port_ndx / SOC_SAND_REG_SIZE_BITS;
  inner_port = port_ndx - SOC_SAND_REG_SIZE_BITS*indx;
  SOC_PB_FLD_GET(regs->egq.ofp2tdm_map_reg[indx].ofp2tdm_map,fld_val, 15,exit);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
    &fld_val,
    inner_port,
    1,
    &tmp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_tdm = (tmp == 1) ? TRUE:FALSE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ifp_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Set the IFP ports configured as TDM destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ifp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_val;
  uint32
    indx,
    inner_port;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_IFP_SET_UNSAFE);
  
  regs = soc_petra_regs();

  indx = port_ndx / SOC_SAND_REG_SIZE_BITS;
  inner_port = port_ndx - SOC_SAND_REG_SIZE_BITS*indx;
  
  /* IFP is TDM if Soc_petra Stamps ftmh version other than 0 */
  SOC_PB_REG_GET(regs->ire.set_ftmh_version_reg[indx],fld_val,10,exit);

  tmp = is_tdm;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          inner_port,
          1,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  SOC_PB_REG_SET(regs->ire.set_ftmh_version_reg[indx],fld_val,15,exit);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ifp_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ifp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_IFP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_tdm, SOC_PB_TDM_IS_TDM_MAX, SOC_PB_TDM_IS_TDM_OUT_OF_RANGE_ERR, 20, exit);
  
  /* API relevant only in packet mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(soc_petra_sw_db_tdm_mode_get(unit),SOC_PETRA_MGMT_TDM_MODE_PACKET, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ifp_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_tdm_ifp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_IFP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_TDM_PORT_NDX_MAX, SOC_PB_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* API relevant only in packet mode. */
  SOC_SAND_ERR_IF_BELOW_MIN(soc_petra_sw_db_tdm_mode_get(unit),SOC_PETRA_MGMT_TDM_MODE_PACKET, SOC_PB_TDM_INVALID_TDM_MODE_ERR, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ifp_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the OFP ports configured as TDM destination.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ifp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp,
    fld_val;
  uint32
    inner_port,
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_IFP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_tdm);
  
  regs = soc_petra_regs();

  indx = port_ndx / SOC_SAND_REG_SIZE_BITS;
  inner_port = port_ndx - SOC_SAND_REG_SIZE_BITS*indx;
  
  SOC_PB_REG_GET(regs->ire.set_ftmh_version_reg[indx],fld_val, 15,exit);
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          &fld_val,
          inner_port,
          1,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_tdm = (tmp == 1) ? TRUE:FALSE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ifp_get_unsafe()", port_ndx, 0);
}
/*********************************************************************
*     Set the TDM multicast static route configuration. Up to
 *     16 routes can be defined. For a TDM Multicast cell, its
 *     selected Route is done according to the 4 MSB of its
 *     Multicast-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  )
{
  uint32
    start_bit,
    link_bitmap[2] = {0},
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_info);

  /*
   * Get the entry table
   */
  entry_offset = ((SOC_PETRA_AQFM_MAX_NOF_FAPS - (SOC_PB_TDM_MC_ID_ROUTE_NDX_MAX + 1) + mc_id_route_ndx) / 2) * 2;
  res = soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Modify the good part of the line
   */
  link_bitmap[0] = route_info->link_bitmap.arr[0];
  link_bitmap[1] = route_info->link_bitmap.arr[1];
  start_bit = (mc_id_route_ndx % 2 == 1)? SOC_DPP_DEFS_GET(unit, nof_fabric_links) : 0;
  res = soc_sand_bitstream_set_any_field(
          link_bitmap,
          start_bit,
          SOC_DPP_DEFS_GET(unit, nof_fabric_links),
          tbl_data.unicast_distribution_memory_for_data_cells
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   * Set it back
   */
  res = soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_set_unsafe()", mc_id_route_ndx, 0);
}

uint32
  soc_pb_tdm_mc_static_route_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id_route_ndx, SOC_PB_TDM_MC_ID_ROUTE_NDX_MAX, SOC_PB_TDM_MC_ID_ROUTE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TDM_MC_STATIC_ROUTE_INFO, route_info, 20, exit);

  /*
   * Available only for B0 and above
   */
  if (SOC_PB_REV_A1_OR_BELOW == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DEVICE_REVISION_REV_A1_OR_BELOW_ERR, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_set_verify()", mc_id_route_ndx, 0);
}

uint32
  soc_pb_tdm_mc_static_route_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id_route_ndx, SOC_PB_TDM_MC_ID_ROUTE_NDX_MAX, SOC_PB_TDM_MC_ID_ROUTE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /*
   * Available only for B0 and above
   */
  if (SOC_PB_REV_A1_OR_BELOW == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DEVICE_REVISION_REV_A1_OR_BELOW_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_get_verify()", mc_id_route_ndx, 0);
}

/*********************************************************************
*     Set the TDM multicast static route configuration. Up to
 *     16 routes can be defined. For a TDM Multicast cell, its
 *     selected Route is done according to the 4 MSB of its
 *     Multicast-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  )
{
  uint32
    start_bit,
    link_bitmap[2] = {0},
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(route_info);

  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_clear(route_info);

  /*
   * Get the entry table
   */
  entry_offset = ((SOC_PETRA_AQFM_MAX_NOF_FAPS - (SOC_PB_TDM_MC_ID_ROUTE_NDX_MAX + 1) + mc_id_route_ndx) / 2) * 2;
  res = soc_petra_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Modify the good part of the line
   */
  start_bit = (mc_id_route_ndx % 2 == 1)? SOC_DPP_DEFS_GET(unit, nof_fabric_links) : 0;
  res = soc_sand_bitstream_get_any_field(
          tbl_data.unicast_distribution_memory_for_data_cells,
          start_bit,
          SOC_DPP_DEFS_GET(unit, nof_fabric_links),
          link_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  route_info->link_bitmap.arr[0] = link_bitmap[0];
  route_info->link_bitmap.arr[1] = link_bitmap[1];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_get_unsafe()", mc_id_route_ndx, 0);
}

uint32
  SOC_PB_TDM_FTMH_OPT_UC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_if, SOC_PB_TDM_FTMH_OPT_UC_DEST_IF_MAX, SOC_PB_TDM_DEST_IF_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_fap_id, SOC_PB_TDM_FTMH_OPT_UC_DEST_FAP_ID_MAX, SOC_PB_TDM_DEST_FAP_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_FTMH_OPT_UC_verify()",0,0);
}

uint32
  SOC_PB_TDM_FTMH_OPT_MC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mc_id, SOC_PB_TDM_FTMH_OPT_MC_MC_ID_MAX, SOC_PB_TDM_MC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_FTMH_OPT_MC_verify()",0,0);
}

uint32
  SOC_PB_TDM_FTMH_STANDARD_UC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sys_phy_port, SOC_PB_TDM_FTMH_STANDARD_UC_SYS_PHY_PORT_MAX, SOC_PB_TDM_SYS_PHY_PORT_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_FTMH_STANDARD_UC_verify()",0,0);
}

uint32
  SOC_PB_TDM_FTMH_STANDARD_MC_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mc_id, SOC_PB_TDM_FTMH_STANDARD_MC_MC_ID_MAX, SOC_PB_TDM_MC_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->user_def, SOC_PB_TDM_FTMH_STANDARD_MC_USER_DEF_MAX, SOC_PB_TDM_MC_USER_DEF_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_FTMH_STANDARD_MC_verify()",0,0);
}

uint32
  SOC_PB_TDM_FTMH_INFO_verify(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_ing, SOC_PB_TDM_FTMH_INFO_ACTION_ING_MAX, SOC_PB_TDM_ACTION_ING_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_eg, SOC_PB_TDM_FTMH_INFO_ACTION_EG_MAX, SOC_PB_TDM_ACTION_EG_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_FTMH_INFO_verify()",0,0);
}
uint32
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->link_bitmap.arr[1] >> (SOC_PETRA_FBR_NOF_LINKS - 32))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TDM_LINK_BITMAP_OUT_OF_RANGE_ERR, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TDM_MC_STATIC_ROUTE_INFO_verify()",0,0);
}
#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

