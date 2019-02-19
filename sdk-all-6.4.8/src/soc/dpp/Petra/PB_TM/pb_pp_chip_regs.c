/* $Id: pb_pp_chip_regs.c,v 1.9 Broadcom SDK $
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
* FILENAME:       soc_pb_pp_chip_regs.c
*
* MODULE PREFIX:  chip_regs
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
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

#define  SOC_PB_PP_DB_REG_SET(reg, base_value, step_value) \
  reg.addr.base = (base_value) * sizeof(uint32); \
  reg.addr.step = (step_value) * sizeof(uint32)

#define  SOC_PB_PP_DB_REG_FLD_SET(fld_value, base_value, step_value, msb_value, lsb_value) \
  soc_pb_pp_reg_fld_set(fld_value, base_value, step_value, msb_value, lsb_value)

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

static SOC_PB_PP_REGS
  Soc_pb_pp_regs;
static uint8
  Soc_pb_pp_regs_initialized = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC void
  soc_pb_pp_reg_fld_set(
    SOC_SAND_OUT SOC_PETRA_REG_FIELD *field,
    SOC_SAND_IN  uint32         base,
    SOC_SAND_IN  uint16         step,
    SOC_SAND_IN  uint8          msb,
    SOC_SAND_IN  uint8          lsb
  )
{
   field->addr.base = sizeof(uint32) * (base);
   field->addr.step = sizeof(uint32) * (step);
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

/* Block registers initialization: ECI  */
STATIC void
  soc_pb_pp_regs_init_ECI(void)
{

  Soc_pb_pp_regs.eci.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_ECI;
  Soc_pb_pp_regs.eci.addr.base = sizeof(uint32) * 0x0000;
  Soc_pb_pp_regs.eci.addr.step = sizeof(uint32) * 0x0000;

  /* TPID-Configuration-Register-0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[0], 0x0013, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[0].tpid0), 0x0013, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[0].tpid1), 0x0013, 0x0000, 31, 16);

  /* TPID-Configuration-Register-1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[1], 0x0014, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[1].tpid0), 0x0014, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_configuration_register_reg[1].tpid1), 0x0014, 0x0000, 31, 16);

  /* SA-Lookup-Type */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.sa_lookup_type_reg, 0x0015, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.sa_lookup_type_reg.sa_lookup_type), 0x0015, 0x0000, 1, 0);

  /* System-Headers-Configurations0 Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.system_headers_configurations0_reg, 0x0016, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.system_headers_configurations0_reg.ftmh_ext), 0x0016, 0x0000, 1, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.system_headers_configurations0_reg.ftmh_lb_key_ext_enable), 0x0016, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.system_headers_configurations0_reg.ftmh_lb_key_ext_mode), 0x0016, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.system_headers_configurations0_reg.add_pph_eep_ext), 0x0016, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.system_headers_configurations0_reg.pph_petra_a_compatible), 0x0016, 0x0000, 5, 5);

  /* TPID-Configuration-Register-2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.tpid_configuration_register_2_reg, 0x003a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_configuration_register_2_reg.itag_tpid), 0x003a, 0x0000, 15, 0);

  /* TPID-Profile-Configuration-Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.tpid_profile_configuration_reg_reg, 0x003f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index0), 0x003f, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_profile_configuration_reg_reg.tpid_profile_outer_index1), 0x003f, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index0), 0x003f, 0x0000, 23, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.tpid_profile_configuration_reg_reg.tpid_profile_inner_index1), 0x003f, 0x0000, 31, 24);

  /* Ethernet-Type-Trill-Configuration-Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.ethernet_type_trill_configuration_reg_reg, 0x0087, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.ethernet_type_trill_configuration_reg_reg.ethertype_trill), 0x0087, 0x0000, 15, 0);

  /* Ethernet-Type-Mpls-Configuration-Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.ethernet_type_mpls_configuration_reg_reg, 0x0088, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.ethernet_type_mpls_configuration_reg_reg.ethertype_mpls0), 0x0088, 0x0000, 15, 0);

  /* My-TRILL-Nickname */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.my_trill_nickname_reg, 0x0089, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.my_trill_nickname_reg.my_trill_nickname), 0x0089, 0x0000, 15, 0);

  /* MAC-in-MAC-VSI */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.eci.mac_in_mac_vsi_reg, 0x008a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.eci.mac_in_mac_vsi_reg.mac_in_mac_vsi), 0x008a, 0x0000, 13, 0);
}

/* Block registers initialization: OLP  */
STATIC void
  soc_pb_pp_regs_init_OLP(void)
{
  uint8
    learning_offset,
    shadow_offset,
    loopback_offset,
    entry_ndx,
    reg_ndx;

  Soc_pb_pp_regs.olp.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_OLP;
  Soc_pb_pp_regs.olp.addr.base = sizeof(uint32) * 0x4400;
  Soc_pb_pp_regs.olp.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.interrupt_reg, 0x4400, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_reg.unexp_sop_err), 0x4400, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_reg.unexp_eop_err), 0x4400, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_reg.unexp_valid_bytes_err), 0x4400, 0x0000, 8, 8);

  /* Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.interrupt_mask_reg, 0x4410, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_mask_reg.unexp_sop_err_mask), 0x4410, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_mask_reg.unexp_eop_err_mask), 0x4410, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.interrupt_mask_reg.unexp_valid_bytes_err_mask), 0x4410, 0x0000, 8, 8);

  /* General Configuration0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.general_configuration0_reg, 0x4454, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.general_configuration0_reg.dsp_eth_type), 0x4454, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.general_configuration0_reg.dis_dsp_eth_type), 0x4454, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.general_configuration0_reg.bytes_to_skip), 0x4454, 0x0000, 25, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.general_configuration0_reg.da_fwd_type), 0x4454, 0x0000, 30, 28);

  /* General Configuration1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.general_configuration1_reg, 0x4502, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.general_configuration1_reg.source_device), 0x4502, 0x0000, 10, 0);

  /* Dsp Event Table */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    loopback_offset = reg_ndx % SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS;
    learning_offset = (loopback_offset + 1) % SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS;
    shadow_offset = (loopback_offset + 2) % SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS;

    for (entry_ndx = 0; entry_ndx < SOC_PB_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG; ++entry_ndx)
    {
      SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_event_table_reg[reg_ndx], (0x4505 + reg_ndx),0x0000);
   
      SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_event_table_reg[reg_ndx].dsp_loopback[entry_ndx]),
        (0x4505 + reg_ndx),
        0x0000,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + loopback_offset,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + loopback_offset
        );

      SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_event_table_reg[reg_ndx].dsp_fifo_learning[entry_ndx]),
        (0x4505 + reg_ndx),
        0x0000,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + learning_offset,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + learning_offset
      );

      SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_event_table_reg[reg_ndx].dsp_fifo_shadow[entry_ndx]),
        (0x4505 + reg_ndx),
        0x0000,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + shadow_offset,
        (SOC_PB_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS * entry_ndx) + shadow_offset
      );
    }
  }

  /* Dsp Event Table */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_event_table_raw_reg[reg_ndx], (0x4505 + reg_ndx), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_event_table_raw_reg[reg_ndx].dsp_fifo), (0x4505 + reg_ndx), 0x0000, 31, 0);
  }

  /* Dsp Engine Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0], 0x450b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0].max_cmd_delay), 0x450b, 0x0000, 10, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0].dsp_generation_en), 0x450b, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0].max_dsp_cmd), 0x450b, 0x0000, 14, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0].min_dsp), 0x450b, 0x0000, 22, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[0].dsp_header_size), 0x450b, 0x0000, 28, 24);

  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1], 0x4514, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1].max_cmd_delay), 0x4514, 0x0000, 10, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1].dsp_generation_en), 0x4514, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1].max_dsp_cmd), 0x4514, 0x0000, 14, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1].min_dsp), 0x4514, 0x0000, 22, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_engine_configuration_reg[1].dsp_header_size), 0x4514, 0x0000, 28, 24);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][0], 0x450c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][0].dsp_header1), 0x450c, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][1], 0x450d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][1].dsp_header1), 0x450d, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][2], 0x450e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][2].dsp_header1), 0x450e, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][3], 0x450f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][3].dsp_header1), 0x450f, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][4], 0x4510, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][4].dsp_header1), 0x4510, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][5], 0x4511, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][5].dsp_header1), 0x4511, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][6], 0x4512, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][6].dsp_header1), 0x4512, 0x0000, 31, 0);

  /* Dsp Header1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[0][7], 0x4513, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[0][7].dsp_header1), 0x4513, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][0], 0x4515, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][0].dsp_header1), 0x4515, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][1], 0x4516, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][1].dsp_header1), 0x4516, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][2], 0x4517, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][2].dsp_header1), 0x4517, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][3], 0x4518, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][3].dsp_header1), 0x4518, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][4], 0x4519, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][4].dsp_header1), 0x4519, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][5], 0x451a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][5].dsp_header1), 0x451a, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][6], 0x451b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][6].dsp_header1), 0x451b, 0x0000, 31, 0);

  /* Dsp Header2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.olp.dsp_header_reg[1][7], 0x451c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.olp.dsp_header_reg[1][7].dsp_header1), 0x451c, 0x0000, 31, 0);
}

/* Block registers initialization: IHP  */
STATIC void
  soc_pb_pp_regs_init_IHP(void)
{
  uint8
    reg_ndx,
    fld_ndx;

  Soc_pb_pp_regs.ihp.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_IHP;
  Soc_pb_pp_regs.ihp.addr.base = sizeof(uint32) * 0x6000;
  Soc_pb_pp_regs.ihp.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.interrupt_reg, 0x6000, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_reg.isem_interrupt), 0x6000, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_reg.large_em_interrupt), 0x6000, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_reg.prsr_interrupt), 0x6000, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_reg.vtt_illegal_range), 0x6000, 0x0000, 3, 3);

  /* Isem Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_interrupt_reg, 0x6001, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_error_cam_table_full), 0x6001, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_error_table_coherency), 0x6001, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_error_delete_unknown_key), 0x6001, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_error_reached_max_entry_limit), 0x6001, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_warning_inserted_existing), 0x6001, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_management_unit_failure_valid), 0x6001, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_reg.isem_management_completed), 0x6001, 0x0000, 6, 6);

  /* Large Em Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_interrupt_reg, 0x6002, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_cam_table_full), 0x6002, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_table_coherency), 0x6002, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_delete_unknown_key), 0x6002, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_reached_max_entry_limit), 0x6002, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_warning_inserted_existing), 0x6002, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_learn_request_over_static), 0x6002, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_warning_learn_over_existing), 0x6002, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_change_fail_non_exist), 0x6002, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_error_change_request_over_static), 0x6002, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_warning_change_non_exist_from_other), 0x6002, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_warning_change_non_exist_from_self), 0x6002, 0x0000, 10, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_management_unit_failure_valid), 0x6002, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_mngmnt_req_fid_exceed_limit), 0x6002, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_mngmnt_req_fid_exceed_limit_static_allowed), 0x6002, 0x0000, 13, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_mngmnt_req_system_vsi_not_found), 0x6002, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_event_ready), 0x6002, 0x0000, 15, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_event_fifo_event_drop), 0x6002, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_event_fifo_high_threshold_reached), 0x6002, 0x0000, 17, 17);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_reply_ready), 0x6002, 0x0000, 18, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_reply_fifo_reply_drop), 0x6002, 0x0000, 19, 19);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_lookup_burst_fifo_drop), 0x6002, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_age_reached_end_index), 0x6002, 0x0000, 21, 21);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_flu_reached_end_index), 0x6002, 0x0000, 22, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_amsg_drop), 0x6002, 0x0000, 23, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_fmsg_drop), 0x6002, 0x0000, 24, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_lookup_request_contention), 0x6002, 0x0000, 25, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_src_or_ll_lookup_on_wrong_cycle), 0x6002, 0x0000, 26, 26);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_reg.large_em_fcnt_counter_overflow), 0x6002, 0x0000, 27, 27);

  /* Prsr Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.prsr_interrupt_reg, 0x6003, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.prsr_interrupt_reg.max_header_stack_exceeded), 0x6003, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.prsr_interrupt_reg.illegal_address_encountered), 0x6003, 0x0000, 1, 1);

  /* Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.interrupt_mask_reg, 0x6010, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_mask_reg.isem_interrupt_mask), 0x6010, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_mask_reg.large_em_interrupt_mask), 0x6010, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_mask_reg.prsr_interrupt_mask), 0x6010, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.interrupt_mask_reg.vtt_illegal_range_mask), 0x6010, 0x0000, 3, 3);

  /* Isem Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg, 0x6011, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_error_cam_table_full_mask), 0x6011, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_error_table_coherency_mask), 0x6011, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_error_delete_unknown_key_mask), 0x6011, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_error_reached_max_entry_limit_mask), 0x6011, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_warning_inserted_existing_mask), 0x6011, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_management_unit_failure_valid_mask), 0x6011, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_interrupt_mask_reg.isem_management_completed_mask), 0x6011, 0x0000, 6, 6);

  /* Large Em Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg, 0x6012, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_cam_table_full_mask), 0x6012, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_table_coherency_mask), 0x6012, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_delete_unknown_key_mask), 0x6012, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_reached_max_entry_limit_mask), 0x6012, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_warning_inserted_existing_mask), 0x6012, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_learn_request_over_static_mask), 0x6012, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_warning_learn_over_existing_mask), 0x6012, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_change_fail_non_exist_mask), 0x6012, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_error_change_request_over_static_mask), 0x6012, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_warning_change_non_exist_from_other_mask), 0x6012, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_warning_change_non_exist_from_self_mask), 0x6012, 0x0000, 10, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_management_unit_failure_valid_mask), 0x6012, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_mngmnt_req_fid_exceed_limit_mask), 0x6012, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_mngmnt_req_fid_exceed_limit_static_allowed_mask), 0x6012, 0x0000, 13, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_mngmnt_req_system_vsi_not_found_mask), 0x6012, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_event_ready_mask), 0x6012, 0x0000, 15, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_event_fifo_event_drop_mask), 0x6012, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_event_fifo_high_threshold_reached_mask), 0x6012, 0x0000, 17, 17);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_reply_ready_mask), 0x6012, 0x0000, 18, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_reply_fifo_reply_drop_mask), 0x6012, 0x0000, 19, 19);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_lookup_burst_fifo_drop_mask), 0x6012, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_age_reached_end_index_mask), 0x6012, 0x0000, 21, 21);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_flu_reached_end_index_mask), 0x6012, 0x0000, 22, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_amsg_drop_mask), 0x6012, 0x0000, 23, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_fmsg_drop_mask), 0x6012, 0x0000, 24, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_lookup_request_contention_mask), 0x6012, 0x0000, 25, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_src_or_ll_lookup_on_wrong_cycle_mask), 0x6012, 0x0000, 26, 26);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_interrupt_mask_reg.large_em_fcnt_counter_overflow_mask), 0x6012, 0x0000, 27, 27);

  /* Prsr Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.prsr_interrupt_mask_reg, 0x6013, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.prsr_interrupt_mask_reg.max_header_stack_exceeded_mask), 0x6013, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.prsr_interrupt_mask_reg.illegal_address_encountered_mask), 0x6013, 0x0000, 1, 1);

  /* Soc_petra C bugfix        */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg, 0x6050, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg.llr_authentication_chicken_bit), 0x6050, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg.mact_aging_high_res_chicken_bit), 0x6050, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg.soc_petra_a_compatible), 0x6050, 0x0000, 6, 6);

  /* Soc_petra C bugfix        */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg_2, 0x6052, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.soc_petra_c_bugfix_reg_2.disable_llc_fix), 0x6052, 0x0000, 0, 0);

  /* Ll Mirror Vid01 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.ll_mirror_vid01_reg, 0x60d0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid01_reg.ll_mirror_vid0), 0x60d0, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid01_reg.ll_mirror_vid1), 0x60d0, 0x0000, 23, 12);

  /* Ll Mirror Vid23 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.ll_mirror_vid23_reg, 0x60d1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid23_reg.ll_mirror_vid2), 0x60d1, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid23_reg.ll_mirror_vid3), 0x60d1, 0x0000, 23, 12);

  /* Ll Mirror Vid45 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.ll_mirror_vid45_reg, 0x60d2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid45_reg.ll_mirror_vid4), 0x60d2, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ll_mirror_vid45_reg.ll_mirror_vid5), 0x60d2, 0x0000, 23, 12);

  /* Eth Mc Bmac Addr Prefix */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.eth_mc_bmac_addr_prefix_reg, 0x60d3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.eth_mc_bmac_addr_prefix_reg.eth_mc_bmac_addr_prefix), 0x60d3, 0x0000, 23, 0);

  /* My BMac Uc Msb Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_msb_config_reg[0], 0x60d4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_msb_config_reg[0].my_bmac_uc_msb), 0x60d4, 0x0000, 31, 0);

  /* My BMac Uc Msb Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_msb_config_reg[1], 0x60d5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_msb_config_reg[1].my_bmac_uc_msb), 0x60d5, 0x0000, 7, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[0], 0x60d6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[0].my_bmac_uc_lsb_bitmap), 0x60d6, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[1], 0x60d7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[1].my_bmac_uc_lsb_bitmap), 0x60d7, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[2], 0x60d8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[2].my_bmac_uc_lsb_bitmap), 0x60d8, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[3], 0x60d9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[3].my_bmac_uc_lsb_bitmap), 0x60d9, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[4], 0x60da, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[4].my_bmac_uc_lsb_bitmap), 0x60da, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[5], 0x60db, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[5].my_bmac_uc_lsb_bitmap), 0x60db, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[6], 0x60dc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[6].my_bmac_uc_lsb_bitmap), 0x60dc, 0x0000, 31, 0);

  /* My BMac Uc Lsb Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[7], 0x60dd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_bmac_uc_lsb_bitmap_reg[7].my_bmac_uc_lsb_bitmap), 0x60dd, 0x0000, 31, 0);

  /* Link Layer Lookup Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.link_layer_lookup_cfg_reg, 0x60de, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.link_layer_lookup_cfg_reg.sa_based_vid_prefix), 0x60de, 0x0000, 14, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.link_layer_lookup_cfg_reg.pbp_mact_prefix), 0x60de, 0x0000, 17, 15);

  /* Action Profile Pbp Sa Drop Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg, 0x60df, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop0_fwd), 0x60df, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop0_snp), 0x60df, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop1_fwd), 0x60df, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop1_snp), 0x60df, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop2_fwd), 0x60df, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop2_snp), 0x60df, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop3_fwd), 0x60df, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_pbp_sa_drop_map_reg.action_profile_pbp_sa_drop3_snp), 0x60df, 0x0000, 19, 18);

  /* Pbp Te Bvid Range */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.pbp_te_bvid_range_reg, 0x60e0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_low), 0x60e0, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_te_bvid_range_reg.pbp_te_bvid_range_high), 0x60e0, 0x0000, 23, 12);

  /* Pbp Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg, 0x60e1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_te_transplant_fwd), 0x60e1, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_te_transplant_snp), 0x60e1, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_te_unknown_tunnel_fwd), 0x60e1, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_te_unknown_tunnel_snp), 0x60e1, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_transplant_fwd), 0x60e1, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_transplant_snp), 0x60e1, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_learn_snoop_fwd), 0x60e1, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pbp_action_profiles_reg.action_profile_pbp_learn_snoop_snp), 0x60e1, 0x0000, 19, 18);

  /* Initial Vlan Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg, 0x60e2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_sa_authentication_failed_fwd), 0x60e2, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_sa_authentication_failed_snp), 0x60e2, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_port_not_permitted_fwd), 0x60e2, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_port_not_permitted_snp), 0x60e2, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_unexpected_vid_fwd), 0x60e2, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.initial_vlan_action_profiles_reg.action_profile_unexpected_vid_snp), 0x60e2, 0x0000, 14, 13);

  /* Filtering Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg, 0x60e3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_sa_multicast_fwd), 0x60e3, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_sa_multicast_snp), 0x60e3, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_sa_equals_da_fwd), 0x60e3, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_sa_equals_da_snp), 0x60e3, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_8021x_fwd), 0x60e3, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.action_profile_8021x_snp), 0x60e3, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.filtering_action_profiles_reg.protocol_8021x), 0x60e3, 0x0000, 18, 15);

  /* Action Profile Acceptable Frame Types */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.action_profile_acceptable_frame_types_reg, 0x60e4, 0x0000);
  for(fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_NOF_FLDS; fld_ndx++)
  {
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_fwd[fld_ndx]), 0x60e4, 0x0000, (uint8)(5*fld_ndx+2), (uint8)(5*fld_ndx));
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.action_profile_acceptable_frame_types_reg.action_profile_acceptable_frame_type_snp[fld_ndx]), 0x60e4, 0x0000, (uint8)(5*fld_ndx+4), (uint8)(5*fld_ndx+3));
  }

  /* Pcp Decoding Table */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_PCP_DECODING_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.pcp_decoding_table_reg[reg_ndx], (0x60e5 + (reg_ndx * 1)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pcp_decoding_table_reg[reg_ndx].pcp_decoding), (0x60a4 + (reg_ndx * 1)), 0x0000, 31, 0);
  }

  /* Traffic Class L4 Range[0..2] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.traffic_class_l4_range_reg[reg_ndx], (0x60e9 + (reg_ndx * 1)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.traffic_class_l4_range_reg[reg_ndx].tc_l4_range_min), (0x60e9 + (reg_ndx * 1)), 0x0000, 15, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.traffic_class_l4_range_reg[reg_ndx].tc_l4_range_max), (0x60e9 + (reg_ndx * 1)), 0x0000, 31, 16);
  }

  /* Traffic Class L4 Range Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.traffic_class_l4_range_cfg_reg, 0x60ec, 0x0000);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_TRAFFIC_CLASS_L4_RANGE_CFG_REG_NOF_FLDS; ++fld_ndx)
  {
    SOC_PB_PP_DB_REG_FLD_SET(
      &(Soc_pb_pp_regs.ihp.traffic_class_l4_range_cfg_reg.tc_l4_range_outside[fld_ndx]),
      0x60ec,
      0x0000,
      fld_ndx,
      fld_ndx);
  }

  /* L4 Port Ranges 2 Tc Table */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_L4_PORT_RANGES_2_TC_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.l4_port_ranges_2_tc_table_reg[reg_ndx], (0x60ed + (reg_ndx * 1)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.l4_port_ranges_2_tc_table_reg[reg_ndx].l4_port_ranges_2_tc), (0x60ed + (reg_ndx * 1)), 0x0000, 31, 0);
  }

  /* Incoming Up Map[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.incoming_up_map_reg[reg_ndx], (0x60f1 + (reg_ndx * 1)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.incoming_up_map_reg[reg_ndx].incoming_user_priority), (0x60f1 + (reg_ndx * 1)), 0x0000, 23, 0);
  }

  /* Traffic Class To User Priority */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.traffic_class_to_user_priority_reg, 0x60f5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.traffic_class_to_user_priority_reg.tc_2_up), 0x60f5, 0x0000, 23, 0);

  /* De To Dp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.de_to_dp_map_reg, 0x60f6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.de_to_dp_map_reg.dp_when_de_is_0), 0x60f6, 0x0000, 1, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.de_to_dp_map_reg.dp_when_de_is_1), 0x60f6, 0x0000, 3, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.de_to_dp_map_reg.default_drop_precedence), 0x60f6, 0x0000, 5, 4);

  /* Drop Precedence Map Pcp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.drop_precedence_map_pcp_reg[0], 0x60f7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.drop_precedence_map_pcp_reg[0].pcp_2_dp), 0x60f7, 0x0000, 31, 0);

  /* Drop Precedence Map Pcp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.drop_precedence_map_pcp_reg[1], 0x60f8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.drop_precedence_map_pcp_reg[1].pcp_2_dp), 0x60f8, 0x0000, 31, 0);

  /* Mac Layer Trap Arp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg, 0x60f9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg.action_profile_my_arp_fwd), 0x60f9, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg.action_profile_my_arp_snp), 0x60f9, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg.action_profile_arp_fwd), 0x60f9, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg.action_profile_arp_snp), 0x60f9, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_arp_reg.arp_trap_ignore_da), 0x60f9, 0x0000, 10, 10);

  /* My Arp Ip1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_arp_ip1_reg, 0x60fa, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_arp_ip1_reg.my_arp_ip1), 0x60fa, 0x0000, 31, 0);

  /* My Arp Ip2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_arp_ip2_reg, 0x60fb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_arp_ip2_reg.my_arp_ip2), 0x60fb, 0x0000, 31, 0);

  /* Mac Layer Trap Igmp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg, 0x60fc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_membership_query_fwd), 0x60fc, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_membership_query_snp), 0x60fc, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_report_leave_msg_fwd), 0x60fc, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_report_leave_msg_snp), 0x60fc, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_undefined_fwd), 0x60fc, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_igmp_reg.action_profile_igmp_undefined_snp), 0x60fc, 0x0000, 14, 13);

  /* Mac Layer Trap Icmp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg, 0x60fd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_mc_listener_query_fwd), 0x60fd, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_mc_listener_query_snp), 0x60fd, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_report_done_msg_fwd), 0x60fd, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_report_done_msg_snp), 0x60fd, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_undefined_fwd), 0x60fd, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_icmp_reg.action_profile_icmpv6_mld_undefined_snp), 0x60fd, 0x0000, 14, 13);

  /* Mac Layer Trap Dhcp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg, 0x60fe, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_server_fwd), 0x60fe, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_server_snp), 0x60fe, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_client_fwd), 0x60fe, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcp_client_snp), 0x60fe, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_server_fwd), 0x60fe, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_server_snp), 0x60fe, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_client_fwd), 0x60fe, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mac_layer_trap_dhcp_reg.action_profile_dhcpv6_client_snp), 0x60fe, 0x0000, 19, 18);

  /* General Trap[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.general_trap_reg_0[reg_ndx], (0x60ff + (reg_ndx * 5)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_0[reg_ndx].general_trap_da), (0x60ff + (reg_ndx * 5)), 0x0000, 31, 0);
  }

  /* General Trap[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.general_trap_reg_1[reg_ndx], (0x6100 + (reg_ndx * 5)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_1[reg_ndx].general_trap_da), (0x6100 + (reg_ndx * 5)), 0x0000, 15, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_1[reg_ndx].general_trap_da_bits), (0x6100 + (reg_ndx * 5)), 0x0000, 21, 16);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_1[reg_ndx].general_trap_ethernet_type), (0x6100 + (reg_ndx * 5)), 0x0000, 25, 22);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_1[reg_ndx].general_trap_sub_type), (0x6100 + (reg_ndx * 5)), 0x0000, 31, 26);
  }

  /* General Trap[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.general_trap_reg_2[reg_ndx], (0x6101 + (reg_ndx * 5)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_2[reg_ndx].general_trap_sub_type), (0x6101 + (reg_ndx * 5)), 0x0000, 1, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_2[reg_ndx].general_trap_sub_type_mask), (0x6101 + (reg_ndx * 5)), 0x0000, 9, 2);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_2[reg_ndx].general_trap_ip_protocol), (0x6101 + (reg_ndx * 5)), 0x0000, 13, 10);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_2[reg_ndx].general_trap_l4_port), (0x6101 + (reg_ndx * 5)), 0x0000, 31, 14);
  }

  /* General Trap[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.general_trap_reg_3[reg_ndx], (0x6102 + (reg_ndx * 5)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_3[reg_ndx].general_trap_l4_port), (0x6102 + (reg_ndx * 5)), 0x0000, 13, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_3[reg_ndx].general_trap_l4_port_mask), (0x6102 + (reg_ndx * 5)), 0x0000, 31, 14);
  }

  /* General Trap[0..3] */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx], (0x6103 + (reg_ndx * 5)), 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx].general_trap_l4_port_mask), (0x6103 + (reg_ndx * 5)), 0x0000, 13, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx].general_trap_enable_bmp), (0x6103 + (reg_ndx * 5)), 0x0000, 18, 14);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx].general_trap_inverse_bmp), (0x6103 + (reg_ndx * 5)), 0x0000, 23, 19);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx].general_trap_action_profile_fwd), (0x6103 + (reg_ndx * 5)), 0x0000, 26, 24);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.general_trap_reg_4[reg_ndx].general_trap_action_profile_snp), (0x6103 + (reg_ndx * 5)), 0x0000, 28, 27);
  }

  /* Dbg Llr Trap0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.dbg_llr_trap0_reg, 0x6113, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.dbg_llr_trap0_reg.dbg_llr_trap0), 0x6113, 0x0000, 31, 0);

  /* Dbg Llr Trap1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.dbg_llr_trap1_reg, 0x6114, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.dbg_llr_trap1_reg.dbg_llr_trap1), 0x6114, 0x0000, 9, 0);

  /* Large Em Flu Machine Hit Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_flu_machine_hit_counter_reg, 0x6240, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_hit_counter_reg.large_em_flu_machine_hit_counter), 0x6240, 0x0000, 16, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_hit_counter_reg.large_em_flu_machine_hit_counter_overflow), 0x6240, 0x0000, 17, 17);

  /* Large Em Lookup Arbiter Link Layer Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_link_layer_lookup_counter_reg, 0x6242, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_link_layer_lookup_counter_reg.large_em_lookup_arbiter_link_layer_lookup_counter), 0x6242, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_link_layer_lookup_counter_reg.large_em_lookup_arbiter_link_layer_lookup_counter_overflow), 0x6242, 0x0000, 31, 31);

  /* Large Em Lookup Arbiter Source Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_source_lookup_counter_reg, 0x6243, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_source_lookup_counter_reg.large_em_lookup_arbiter_source_lookup_counter), 0x6243, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_source_lookup_counter_reg.large_em_lookup_arbiter_source_lookup_counter_overflow), 0x6243, 0x0000, 31, 31);

  /* Large Em Lookup Arbiter Destination Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_destination_lookup_counter_reg, 0x6244, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_destination_lookup_counter_reg.large_em_lookup_arbiter_destination_lookup_counter), 0x6244, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_destination_lookup_counter_reg.large_em_lookup_arbiter_destination_lookup_counter_overflow), 0x6244, 0x0000, 31, 31);

  /* Large Em Lookup Arbiter Learn Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_learn_lookup_counter_reg, 0x6245, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_learn_lookup_counter_reg.large_em_lookup_arbiter_learn_lookup_counter), 0x6245, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_arbiter_learn_lookup_counter_reg.large_em_lookup_arbiter_learn_lookup_counter_overflow), 0x6245, 0x0000, 31, 31);

  /* Large Em Learn Filter Properties */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg, 0x6246, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_drop_duplicate), 0x6246, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_entry_ttl), 0x6246, 0x0000, 9, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_res), 0x6246, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_watermark), 0x6246, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_drop_duplicate_counter), 0x6246, 0x0000, 30, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_filter_properties_reg.large_em_learn_filter_drop_duplicate_counter_overflow), 0x6246, 0x0000, 31, 31);

  /* Large Em Ingress Learn Type */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_ingress_learn_type_reg, 0x6247, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_ingress_learn_type_reg.large_em_ingress_learn_type), 0x6247, 0x0000, 0, 0);

  /* Large Em Lookup Filter Properties */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg, 0x6248, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_drop_duplicate), 0x6248, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_entry_ttl), 0x6248, 0x0000, 9, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_res), 0x6248, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_watermark), 0x6248, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_drop_duplicate_counter), 0x6248, 0x0000, 30, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_filter_properties_reg.large_em_lookup_filter_drop_duplicate_counter_overflow), 0x6248, 0x0000, 31, 31);

  /* Large Em Lookup Burst Fifo Properties */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_burst_fifo_properties_reg, 0x6249, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_burst_fifo_properties_reg.large_em_lookup_burst_fifo_watermark), 0x6249, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_burst_fifo_properties_reg.large_em_lookup_burst_fifo_drop_counter), 0x6249, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_burst_fifo_properties_reg.large_em_lookup_burst_fifo_drop_counter_overflow), 0x6249, 0x0000, 16, 16);

  /* Large Em Learn Lookup General Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg, 0x624a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_mact), 0x624a, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_elk), 0x624a, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_elk_bits63_to58), 0x624a, 0x0000, 7, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_elk_bits127_to122), 0x624a, 0x0000, 13, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_access_vsi_db_on_service_mac), 0x624a, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_access_vsi_db_on_backbone_mac), 0x624a, 0x0000, 15, 15);

  /* Large Em Learn Lookup Ingress Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_learn_lookup_ingress_lookup_counter_reg, 0x624b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_ingress_lookup_counter_reg.large_em_learn_lookup_ingress_lookup_counter), 0x624b, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_ingress_lookup_counter_reg.large_em_learn_lookup_ingress_lookup_counter_overflow), 0x624b, 0x0000, 31, 31);

  /* Large Em Learn Lookup Egress Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_learn_lookup_egress_lookup_counter_reg, 0x624c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_egress_lookup_counter_reg.large_em_learn_lookup_egress_lookup_counter), 0x624c, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_lookup_egress_lookup_counter_reg.large_em_learn_lookup_egress_lookup_counter_overflow), 0x624c, 0x0000, 31, 31);

  /* Large Em Lookup Lookup Mode */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_lookup_lookup_mode_reg, 0x624d,0x0000);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES; ++fld_ndx)
  {
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_dynamic[fld_ndx]),
      0x6177, 0x0000,
      1 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx),
      0 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_static[fld_ndx]),
      0x6177, 0x0000,
      3 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx),
      2 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_lookup_lookup_mode_reg.large_em_lookup_allowed_events_learn[fld_ndx]),
      0x6177, 0x0000,
      4 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx),
      4 + (SOC_PB_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS * fld_ndx));
  }

  /* Large Em Learn Not Needed Drop Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_learn_not_needed_drop_counter_reg, 0x624e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_not_needed_drop_counter_reg.large_em_learn_not_needed_drop_counter), 0x624e, 0x0000, 14, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_learn_not_needed_drop_counter_reg.large_em_learn_not_needed_drop_counter_overflow), 0x624e, 0x0000, 15, 15);

  /* Large Em Mngmnt Req Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_configuration_reg, 0x624f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_configuration_reg.large_em_mngmnt_req_request_fifo_high_threshold), 0x624f, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_configuration_reg.large_em_mngmnt_req_allow_static_exceed), 0x624f, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_configuration_reg.large_em_mngmnt_req_acknowledge_only_failure), 0x624f, 0x0000, 12, 12);

  /* Large Em Mngmnt Req Counters */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg, 0x6250, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_reply_drop_reply_not_needed_counter), 0x6250, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_reply_drop_reply_not_needed_counter_overflow), 0x6250, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_fid_exceed_limit_counter), 0x6250, 0x0000, 14, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_fid_exceed_limit_counter_overflow), 0x6250, 0x0000, 15, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_fid_exceed_limit_static_allowed_counter), 0x6250, 0x0000, 22, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_fid_exceed_limit_static_allowed_counter_overflow), 0x6250, 0x0000, 23, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_system_vsi_not_found_counter), 0x6250, 0x0000, 30, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_counters_reg.large_em_mngmnt_req_system_vsi_not_found_counter_overflow), 0x6250, 0x0000, 31, 31);

  /* Large Em Mngmnt Req Request Fifo Entry Count */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_fifo_entry_count_reg, 0x6251, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_fifo_entry_count_reg.large_em_mngmnt_req_request_fifo_entry_count), 0x6251, 0x0000, 7, 0);

  /* Large Em Mngmnt Req Request Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_counter_reg, 0x6252, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_counter_reg.large_em_mngmnt_req_request_counter), 0x6252, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_counter_reg.large_em_mngmnt_req_request_counter_overflow), 0x6252, 0x0000, 31, 31);

  /* Large Em Mngmnt Req Request Fifo Watermark */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_fifo_watermark_reg, 0x6253, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_request_fifo_watermark_reg.large_em_mngmnt_req_request_fifo_watermark), 0x6253, 0x0000, 7, 0);

  /* Large Em Mngmnt Req Exceed Limit Fid */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_exceed_limit_fid_reg, 0x6254, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_exceed_limit_fid_reg.large_em_mngmnt_req_exceed_limit_fid), 0x6254, 0x0000, 13, 0);

  /* Large Em Mngmnt Req Exceed Limit Static Allowed Fid */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_exceed_limit_static_allowed_fid_reg, 0x6255, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_exceed_limit_static_allowed_fid_reg.large_em_mngmnt_req_exceed_limit_static_allowed_fid), 0x6255, 0x0000, 13, 0);

  /* Large Em Mngmnt Req System Vsi Not Found System Vsi */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_system_vsi_not_found_system_vsi_reg, 0x6256, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_mngmnt_req_system_vsi_not_found_system_vsi_reg.large_em_mngmnt_req_system_vsi_not_found_system_vsi), 0x6256, 0x0000, 15, 0);

  /* Large Em Cpu Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_0, 0x6257, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_0.large_em_req_mff_is_key), 0x6257, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_0.large_em_req_mff_mac), 0x6257, 0x0000, 31, 1);

  /* Large Em Cpu Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_1, 0x6258, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_1.large_em_req_mff_mac), 0x6258, 0x0000, 16, 0);
  
  /* Large Em Cpu Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2, 0x6259, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_mff_key_db_profile), 0x6259, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_part_of_lag), 0x6259, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_command), 0x6259, 0x0000, 4, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_stamp), 0x6259, 0x0000, 12, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_payload_destination), 0x6259, 0x0000, 28, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_payload_asd), 0x6183, 0x0000, 31, 29);

  /* Large Em Cpu Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3, 0x625a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_payload_asd), 0x625a, 0x0000, 20, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_payload_is_dynamic), 0x625a, 0x0000, 21, 21);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_age_payload_age_status), 0x625a, 0x0000, 23, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_age_payload), 0x625a, 0x0000, 24, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_success), 0x625a, 0x0000, 25, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_reason), 0x625a, 0x0000, 27, 26);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_resreved), 0x625a, 0x0000, 29, 28);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_self), 0x625a, 0x0000, 30, 30);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_qualifier), 0x625a, 0x0000, 31, 31);

  /* Large Em Cpu Request Trigger */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cpu_request_trigger_reg, 0x625b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cpu_request_trigger_reg.large_em_cpu_request_trigger), 0x625b, 0x0000, 0, 0);

  /* Large Em Olp Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_0, 0x625c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_0.large_em_olp_request_request), 0x625c, 0x0000, 31, 0);

  /* Large Em Olp Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_1, 0x625d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_1.large_em_olp_request_request), 0x625d, 0x0000, 31, 0);

  /* Large Em Olp Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_2, 0x625e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_2.large_em_olp_request_request), 0x625e, 0x0000, 31, 0);

  /* Large Em Olp Request Request */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_3, 0x625f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_olp_request_request_reg_3.large_em_olp_request_request), 0x625f, 0x0000, 31, 0);

  /* Large Em Olp Request Trigger */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_olp_request_trigger_reg, 0x6260, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_olp_request_trigger_reg.large_em_olp_request_trigger), 0x6260, 0x0000, 0, 0);

  /* Large Em Age Machine Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_machine_configuration_reg, 0x6261, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_configuration_reg.large_em_age_aging_enable), 0x6261, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_configuration_reg.large_em_age_machine_pause), 0x6261, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_configuration_reg.large_em_age_machine_access_shaper), 0x6261, 0x0000, 9, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_configuration_reg.large_em_age_stamp), 0x6261, 0x0000, 17, 10);

  /* Large Em Age Machine Current Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_machine_current_index_reg, 0x6262, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_current_index_reg.large_em_age_machine_current_index), 0x6262, 0x0000, 16, 0);

  /* Large Em Age Machine Status */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_machine_status_reg, 0x6263, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_status_reg.large_em_age_machine_active), 0x6263, 0x0000, 0, 0);

  /* Large Em Age Machine Meta Cycle */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_machine_meta_cycle_reg, 0x6264, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_machine_meta_cycle_reg.large_em_age_machine_meta_cycle), 0x6264, 0x0000, 31, 0);

  /* Large Em Age Aging Mode */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_aging_mode_reg, 0x6267,0x0000);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES; ++fld_ndx)
  {
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_aging_mode_reg.large_em_age_aging_cfg_ptr[fld_ndx]),
      0x6267, 0x0000,
      1 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 0 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_aging_mode_reg.large_em_age_own_system_physical_ports[fld_ndx]),
      0x6267, 0x0000,
      2 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 2 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_aging_mode_reg.large_em_age_own_system_lag_ports[fld_ndx]),
      0x6267, 0x0000,
      3 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx), 3 + (SOC_PB_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS * fld_ndx));
  }

  /* Large Em Age Aging Resolution */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_age_aging_resolution_reg, 0x6268, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_age_aging_resolution_reg.large_em_age_aging_resolution), 0x6268, 0x0000, 0, 0);

  /* Large Em Flu Machine Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_flu_machine_configuration_reg, 0x626a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_configuration_reg.large_em_flu_machine_pause), 0x626a, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_configuration_reg.large_em_flu_machine_access_shaper), 0x626a, 0x0000, 8, 1);

  /* Large Em Flu Machine Current Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_flu_machine_current_index_reg, 0x626b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_current_index_reg.large_em_flu_machine_current_index), 0x626b, 0x0000, 16, 0);

  /* Large Em Flu Machine Status */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_flu_machine_status_reg, 0x626c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_status_reg.large_em_flu_machine_active), 0x626c, 0x0000, 0, 0);

  /* Large Em Flu Machine End Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_flu_machine_end_index_reg, 0x626d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_flu_machine_end_index_reg.large_em_flu_machine_end_index), 0x626d, 0x0000, 16, 0);

  /* Large Em Fid Db Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_fid_db_configuration_reg, 0x626e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_fid_db_configuration_reg.large_em_fid_db_check_fid_limit), 0x626e, 0x0000, 0, 0);

  /* Large Em Cpu Event */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_0, 0x626f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_0.large_em_req_mff_is_key), 0x626f, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_0.large_em_req_mff_mac), 0x626f, 0x0000, 31, 1);

  /* Large Em Cpu Event */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_1, 0x6270, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_1.large_em_req_mff_mac), 0x6270, 0x0000, 16, 0);

  /* Large Em Cpu Event */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2, 0x6271, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_mff_key_db_profile), 0x6271, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_part_of_lag), 0x6271, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_command), 0x6271, 0x0000, 4, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_stamp), 0x6271, 0x0000, 12, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_payload_destination), 0x6271, 0x0000, 28, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_2.large_em_req_payload_asd), 0x6171, 0x0000, 31, 29);

  /* Large Em Cpu Event */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3, 0x6272, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_payload_asd), 0x6272, 0x0000, 20, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_payload_is_dynamic), 0x6272, 0x0000, 21, 21);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_age_payload_age_status), 0x6272, 0x0000, 23, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_age_payload), 0x6272, 0x0000, 24, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_success), 0x6272, 0x0000, 25, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_reason), 0x6272, 0x0000, 27, 26);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_resreved), 0x6272, 0x0000, 29, 28);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_self), 0x6272, 0x0000, 30, 30);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_reg.reg_3.large_em_req_qualifier), 0x6272, 0x0000, 31, 31);

  /* Large Em Event Fifo Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_configuration_reg, 0x6273, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_full_threshold), 0x6273, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_access_fid_db), 0x6273, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_configuration_reg.large_em_event_fifo_access_vsi_db), 0x6273, 0x0000, 8, 8);

  /* Large Em Event Fifo High Threshold */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_high_threshold_reg, 0x6274, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_high_threshold_reg.large_em_event_fifo_high_threshold), 0x6274, 0x0000, 6, 0);

  /* Large Em Event Fifo Event Drop Counters */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg, 0x6275, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_mrq_event_drop_counter), 0x6275, 0x0000, 8, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_mrq_event_drop_counter_overflow), 0x6275, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_age_event_drop_counter), 0x6275, 0x0000, 18, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_age_event_drop_counter_overflow), 0x6275, 0x0000, 19, 19);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_lrf_event_drop_counter), 0x6275, 0x0000, 28, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_drop_counters_reg.large_em_event_fifo_lrf_event_drop_counter_overflow), 0x6275, 0x0000, 29, 29);

  /* Large Em Event Fifo Event Fifo Entry Count */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_fifo_entry_count_reg, 0x6276, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_fifo_entry_count_reg.large_em_event_fifo_event_fifo_entry_count), 0x6276, 0x0000, 7, 0);

  /* Large Em Event Fifo Watermark */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_watermark_reg, 0x6277, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_watermark_reg.large_em_event_fifo_watermark), 0x6277, 0x0000, 7, 0);

  /* Large Em Event Fifo Event Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_counter_reg, 0x6278, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_counter_reg.large_em_event_fifo_event_counter), 0x6278, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_event_fifo_event_counter_reg.large_em_event_fifo_event_counter_overflow), 0x6278, 0x0000, 31, 31);

  /* Mact Cpu Reply */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_0, 0x6279, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_0.large_em_req_mff_is_key), 0x6279, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_0.large_em_req_mff_mac), 0x6279, 0x0000, 31, 1);

  /* Mact Cpu Reply */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_1, 0x627a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_1.large_em_req_mff_mac), 0x627a, 0x0000, 16, 0);

  /* Mact Cpu Reply */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2, 0x627b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_mff_key_db_profile), 0x627b, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_part_of_lag), 0x627b, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_command), 0x627b, 0x0000, 4, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_stamp), 0x627b, 0x0000, 12, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_payload_destination), 0x627b, 0x0000, 28, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_2.large_em_req_payload_asd), 0x627b, 0x0000, 31, 29);

  /* Mact Cpu Reply */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3, 0x627c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_payload_asd), 0x627c, 0x0000, 20, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_payload_is_dynamic), 0x627c, 0x0000, 21, 21);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_age_payload_age_status), 0x627c, 0x0000, 23, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_age_payload), 0x627c, 0x0000, 24, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_success), 0x627c, 0x0000, 25, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_reason), 0x627c, 0x0000, 27, 26);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_resreved), 0x627c, 0x0000, 29, 28);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_self), 0x627c, 0x0000, 30, 30);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_reg.reg_3.large_em_req_qualifier), 0x627c, 0x0000, 31, 31);

  /* Large Em Reply Fifo Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_fifo_configuration_reg, 0x627d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_configuration_reg.large_em_reply_fifo_full_threshold), 0x627d, 0x0000, 6, 0);

  /* Large Em Reply Fifo Reply Drop Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_drop_counter_reg, 0x627e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_drop_counter_reg.large_em_reply_fifo_reply_drop_counter), 0x627e, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_drop_counter_reg.large_em_reply_fifo_reply_drop_counter_overflow), 0x627e, 0x0000, 8, 8);

  /* Large Em Reply Fifo Reply Fifo Entry Count */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_fifo_entry_count_reg, 0x627f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_fifo_entry_count_reg.large_em_reply_fifo_reply_fifo_entry_count), 0x627f, 0x0000, 7, 0);

  /* Large Em Reply Fifo Watermark */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_fifo_watermark_reg, 0x6280, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_watermark_reg.large_em_reply_fifo_watermark), 0x6280, 0x0000, 7, 0);

  /* Large Em Reply Fifo Reply Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_counter_reg, 0x6281, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_counter_reg.large_em_reply_fifo_reply_counter), 0x6281, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reply_fifo_reply_counter_reg.large_em_reply_fifo_reply_counter_overflow), 0x6281, 0x0000, 31, 31);

  /* Large Em Error Learn Request Over Static Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_learn_request_over_static_counter_reg, 0x6282, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_learn_request_over_static_counter_reg.large_em_error_learn_request_over_static_counter), 0x6282, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_learn_request_over_static_counter_reg.large_em_error_learn_request_over_static_counter_overflow), 0x6282, 0x0000, 8, 8);

  /* Large Em Warning Learn Over Existing Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_warning_learn_over_existing_counter_reg, 0x6283, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_learn_over_existing_counter_reg.large_em_warning_learn_over_existing_counter), 0x6283, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_learn_over_existing_counter_reg.large_em_warning_learn_over_existing_counter_overflow), 0x6283, 0x0000, 8, 8);

  /* Large Em Error Change Fail Non Exist Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_change_fail_non_exist_counter_reg, 0x6284, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_change_fail_non_exist_counter_reg.large_em_error_change_fail_non_exist_counter), 0x6284, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_change_fail_non_exist_counter_reg.large_em_error_change_fail_non_exist_counter_overflow), 0x6284, 0x0000, 8, 8);

  /* Large Em Error Change Request Over Static Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_change_request_over_static_counter_reg, 0x6285, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_change_request_over_static_counter_reg.large_em_error_change_request_over_static_counter), 0x6285, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_change_request_over_static_counter_reg.large_em_error_change_request_over_static_counter_overflow), 0x6285, 0x0000, 8, 8);

  /* Large Em Warning Change Non Exist From Other Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_other_counter_reg, 0x6286, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_other_counter_reg.large_em_warning_change_non_exist_from_other_counter), 0x6286, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_other_counter_reg.large_em_warning_change_non_exist_from_other_counter_overflow), 0x6286, 0x0000, 8, 8);

  /* Large Em Warning Change Non Exist From Self Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_self_counter_reg, 0x6287, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_self_counter_reg.large_em_warning_change_non_exist_from_self_counter), 0x6287, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_change_non_exist_from_self_counter_reg.large_em_warning_change_non_exist_from_self_counter_overflow), 0x6287, 0x0000, 8, 8);

  /* Ihp Enablers */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.ihp_enablers_reg, 0x6080, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.enable_data_path), 0x6080, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.enable_mact_bubbles), 0x6080, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.enable_tcam_bubbles), 0x6080, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.enable_indirect_bubbles), 0x6080, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.force_bubbles), 0x6080, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.max_header_stack), 0x6080, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.ihp_enablers_reg.accept_one_packet), 0x6080, 0x0000, 28, 28);

  /* Sync Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.sync_counter_reg, 0x6081, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sync_counter_reg.sync_counter), 0x6081, 0x0000, 9, 0);

  /* Undef Program Data */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.undef_program_data_reg, 0x6082, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.undef_program_data_reg.undef_program_data), 0x6082, 0x0000, 9, 0);

  /* Pp Context Custom Macro Select */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.pp_context_custom_macro_select_reg, 0x6083, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.pp_context_custom_macro_select_reg.pp_ctxt_cstm_mcro_sel), 0x6083, 0x0000, 7, 0);

  /* Mpls Label Range[0..2]Low */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[0], 0x6084, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[0].mpls_label_range_low), 0x6084, 0x0000, 19, 0);
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[1], 0x6085, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[1].mpls_label_range_low), 0x6085, 0x0000, 19, 0);
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[2], 0x6086, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_low_reg[2].mpls_label_range_low), 0x6086, 0x0000, 19, 0);

  /* Mpls Label Range[0..2]High */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[0], 0x6087, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[0].mpls_label_range_high), 0x6087, 0x0000, 19, 0);
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[1], 0x6088, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[1].mpls_label_range_high), 0x6088, 0x0000, 19, 0);
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[2], 0x6089, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_high_reg[2].mpls_label_range_high), 0x6089, 0x0000, 19, 0);

  /* Gre Eth Type Ipv4 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.gre_eth_type_ipv4_reg, 0x608a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.gre_eth_type_ipv4_reg.gre_eth_type_ipv4), 0x608a, 0x0000, 15, 0);

  /* Gre Eth Type Ipv6 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.gre_eth_type_ipv6_reg, 0x608b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.gre_eth_type_ipv6_reg.gre_eth_type_ipv6), 0x608b, 0x0000, 15, 0);

  /* Gre Eth Type Mpls */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.gre_eth_type_mpls_reg, 0x608c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.gre_eth_type_mpls_reg.gre_eth_type_mpls), 0x608c, 0x0000, 15, 0);

  /* Gre Eth Type Custom */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.gre_eth_type_custom_reg, 0x608d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.gre_eth_type_custom_reg.gre_eth_type_custom), 0x608d, 0x0000, 15, 0);

  /* Program Illegel Address */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.program_illegel_address_reg, 0x608e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.program_illegel_address_reg.program_address), 0x608e, 0x0000, 10, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.program_illegel_address_reg.program_mem_index), 0x608e, 0x0000, 12, 12);

  /* Header Stack Exceed Program Address */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.header_stack_exceed_program_address_reg, 0x608f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.header_stack_exceed_program_address_reg.header_stack_exceed_program_address), 0x608f, 0x0000, 10, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_0, 0x6090, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_0.last_rcvd_hdr), 0x6090, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_1, 0x6091, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_1.last_rcvd_hdr), 0x6091, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_2, 0x6092, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_2.last_rcvd_hdr), 0x6092, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_3, 0x6093, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_3.last_rcvd_hdr), 0x6093, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_4, 0x6094, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_4.last_rcvd_hdr), 0x6094, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_5, 0x6095, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_5.last_rcvd_hdr), 0x6095, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_6, 0x6096, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_6.last_rcvd_hdr), 0x6096, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_7, 0x6097, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_7.last_rcvd_hdr), 0x6097, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_8, 0x6098, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_8.last_rcvd_hdr), 0x6098, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_9, 0x6099, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_9.last_rcvd_hdr), 0x6099, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_10, 0x609a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_10.last_rcvd_hdr), 0x609a, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_11, 0x609b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_11.last_rcvd_hdr), 0x609b, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_12, 0x609c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_12.last_rcvd_hdr), 0x609c, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_13, 0x609d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_13.last_rcvd_hdr), 0x609d, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_14, 0x609e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_14.last_rcvd_hdr), 0x609e, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_15, 0x609f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_15.last_rcvd_hdr), 0x609f, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_16, 0x60a0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_16.last_rcvd_hdr), 0x60a0, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_17, 0x60a1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_17.last_rcvd_hdr), 0x60a1, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_18, 0x60a2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_18.last_rcvd_hdr), 0x60a2, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_19, 0x60a3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_19.last_rcvd_hdr), 0x60a3, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_20, 0x60a4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_20.last_rcvd_hdr), 0x60a4, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_21, 0x60a5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_21.last_rcvd_hdr), 0x60a5, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_22, 0x60a6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_22.last_rcvd_hdr), 0x60a6, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_23, 0x60a7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_23.last_rcvd_hdr), 0x60a7, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_24, 0x60a8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_24.last_rcvd_hdr), 0x60a8, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_25, 0x60a9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_25.last_rcvd_hdr), 0x60a9, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_26, 0x60aa, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_26.last_rcvd_hdr), 0x60aa, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_27, 0x60ab, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_27.last_rcvd_hdr), 0x60ab, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_28, 0x60ac, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_28.last_rcvd_hdr), 0x60ac, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_29, 0x60ad, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_29.last_rcvd_hdr), 0x60ad, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_30, 0x60ae, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_30.last_rcvd_hdr), 0x60ae, 0x0000, 31, 0);

  /* Last Received Header Reg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_header_reg_31, 0x60af, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_header_reg_31.last_rcvd_hdr), 0x60af, 0x0000, 31, 0);

  /* Last Generated Values */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_generated_values_reg, 0x60b0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_generated_values_reg.last_pp_port), 0x60b0, 0x0000, 5, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_generated_values_reg.last_pp_context), 0x60b0, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_generated_values_reg.last_qual0), 0x60b0, 0x0000, 14, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_generated_values_reg.last_src_sys_port), 0x60b0, 0x0000, 28, 16);

  /* Last Received Port */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.last_received_port_reg, 0x60b1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_port_reg.last_rcvd_tm_port), 0x60b1, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_port_reg.last_rcvd_packet_size), 0x60b1, 0x0000, 14, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.last_received_port_reg.last_rcvd_valid), 0x60b1, 0x0000, 16, 16);

  /* Rcvd Packet Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.rcvd_packet_counter_reg, 0x60b2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.rcvd_packet_counter_reg.rcvd_packet_counter), 0x60b2, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.rcvd_packet_counter_reg.rcvd_packet_counter_overflow), 0x60b2, 0x0000, 31, 31);

  /* Isem Lookup Arbiter Counters */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_lookup_arbiter_counters_reg, 0x62a0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_lookup_arbiter_counters_reg.isem_lookup_arbiter_counter), 0x62a0, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_lookup_arbiter_counters_reg.isem_lookup_arbiter_counter_overflow), 0x62a0, 0x0000, 31, 31);

  /* Vsd Shared Bfid */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vsd_shared_bfid_reg, 0x6060, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vsd_shared_bfid_reg.vsd_shared_bfid), 0x6060, 0x0000, 11, 0);

  /* Vtt General Configs */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_general_configs_reg, 0x6180, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.vrid_my_mac_mode), 0x6180, 0x0000, 1, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.always_do2nd_lookup), 0x6180, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.trill_vsi_from_outer_eth), 0x6180, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.cfm_pp_context), 0x6180, 0x0000, 6, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.my_bmac_parser_pmf_profile), 0x6180, 0x0000, 11, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.ipv4_over_mpls_parser_pmf_profile), 0x6180, 0x0000, 15, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.ipv6_over_mpls_parser_pmf_profile), 0x6180, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.eth_over_pwe_parser_pmf_profile), 0x6180, 0x0000, 23, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.inner_eth_encapsulation), 0x6180, 0x0000, 25, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.vtt_forwarding_strength), 0x6180, 0x0000, 30, 28);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_general_configs_reg.mac_in_mac), 0x6180, 0x0000, 31, 31);

  /* Vsi Values0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vsi_values0_reg, 0x6181, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vsi_values0_reg.ac_p2p_to_ac_vsi), 0x6181, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vsi_values0_reg.ac_p2p_to_pwe_vsi), 0x6181, 0x0000, 29, 16);

  /* Vsi Values1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vsi_values1_reg, 0x6182, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vsi_values1_reg.label_pwe_p2p_vsi), 0x6182, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vsi_values1_reg.trill_default_vsi), 0x6182, 0x0000, 29, 16);

  /* Service Type Values */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.service_type_values_reg, 0x6183, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.service_type_values_reg.service_type_label_pwe_p2p), 0x6183, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.service_type_values_reg.service_type_label_pwe_mp), 0x6183, 0x0000, 6, 4);

  /* Asd Ac Prefix */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.asd_ac_prefix_reg, 0x6184, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.asd_ac_prefix_reg.asd_ac_prefix), 0x6184, 0x0000, 9, 0);

  /* My Mac Msb Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_mac_msb_config_reg[0], 0x6185, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_mac_msb_config_reg[0].my_mac_msb), 0x6185, 0x0000, 31, 0);

  /* My Mac Msb Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.my_mac_msb_config_reg[1], 0x6186, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.my_mac_msb_config_reg[1].my_mac_msb), 0x6186, 0x0000, 15, 0);

  /* All Rbridges Mac Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.all_rbridges_mac_config_reg[0], 0x6187, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.all_rbridges_mac_config_reg[0].all_rbridges_mac), 0x6187, 0x0000, 31, 0);

  /* All Rbridges Mac Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.all_rbridges_mac_config_reg[1], 0x6188, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.all_rbridges_mac_config_reg[1].all_rbridges_mac), 0x6188, 0x0000, 15, 0);

  /* Vrid My Mac Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vrid_my_mac_config_reg_0, 0x6189, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vrid_my_mac_config_reg_0.vrid_my_mac), 0x6189, 0x0000, 31, 0);

  /* Vrid My Mac Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vrid_my_mac_config_reg_1, 0x618a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vrid_my_mac_config_reg_1.vrid_my_mac), 0x618a, 0x0000, 15, 0);

  /* Vtt Trap Strengths0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg, 0x618b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.port_not_vlan_member_fwd), 0x618b, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.port_not_vlan_member_snp), 0x618b, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.my_bmac_and_learn_null_fwd), 0x618b, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.my_bmac_and_learn_null_snp), 0x618b, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.my_bmac_unknown_isid_fwd), 0x618b, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.my_bmac_unknown_isid_snp), 0x618b, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.header_size_err_fwd), 0x618b, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths0_reg.header_size_err_snp), 0x618b, 0x0000, 29, 28);

  /* Vtt Trap Strengths1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg, 0x618c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.illegal_pfc_fwd), 0x618c, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.illegal_pfc_snp), 0x618c, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.cfm_accelarated_ingress_fwd), 0x618c, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.cfm_accelarated_ingress_snp), 0x618c, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.stp_state_block_fwd), 0x618c, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.stp_state_block_snp), 0x618c, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.stp_state_learn_fwd), 0x618c, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths1_reg.stp_state_learn_snp), 0x618c, 0x0000, 29, 28);

  /* Vtt Trap Strengths2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg, 0x618d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.ip_comp_mc_invalid_ip_fwd), 0x618d, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.ip_comp_mc_invalid_ip_snp), 0x618d, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.my_mac_and_ip_disable_fwd), 0x618d, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.my_mac_and_ip_disable_snp), 0x618d, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.trill_version_fwd), 0x618d, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.trill_version_snp), 0x618d, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.trill_invalid_ttl_fwd), 0x618d, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths2_reg.trill_invalid_ttl_snp), 0x618d, 0x0000, 29, 28);

  /* Vtt Trap Strengths3 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg, 0x618e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_chbh_fwd), 0x618e, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_chbh_snp), 0x618e, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_no_reverse_fec_fwd), 0x618e, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_no_reverse_fec_snp), 0x618e, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_cite_fwd), 0x618e, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_cite_snp), 0x618e, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_illegel_inner_mc_fwd), 0x618e, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths3_reg.trill_illegel_inner_mc_snp), 0x618e, 0x0000, 29, 28);

  /* Vtt Trap Strengths4 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg, 0x618f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_mpls_disable_fwd), 0x618f, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_mpls_disable_snp), 0x618f, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_arp_fwd), 0x618f, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_arp_snp), 0x618f, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_unknown_l3_fwd), 0x618f, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.my_mac_and_unknown_l3_snp), 0x618f, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.mpls_no_resources_fwd), 0x618f, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths4_reg.mpls_no_resources_snp), 0x618f, 0x0000, 29, 28);

  /* Vtt Trap Strengths5 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg, 0x6190, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_invalid_label_in_range_fwd), 0x6190, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_invalid_label_in_range_snp), 0x6190, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_fwd), 0x6190, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_snp), 0x6190, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_label14_fwd), 0x6190, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_pwe_no_bos_label14_snp), 0x6190, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_ttl0_fwd), 0x6190, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths5_reg.mpls_ttl0_snp), 0x6190, 0x0000, 29, 28);

  /* Vtt Trap Strengths6 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg, 0x6191, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_control_word_trap_fwd), 0x6191, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_control_word_trap_snp), 0x6191, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_control_word_drop_fwd), 0x6191, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_control_word_drop_snp), 0x6191, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_lsp_bos_fwd), 0x6191, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_lsp_bos_snp), 0x6191, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_vrf_no_bos_fwd), 0x6191, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths6_reg.mpls_vrf_no_bos_snp), 0x6191, 0x0000, 29, 28);

  /* Vtt Trap Strengths7 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg, 0x6192, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value0_fwd), 0x6192, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value0_snp), 0x6192, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value1_fwd), 0x6192, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value1_snp), 0x6192, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value2_fwd), 0x6192, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value2_snp), 0x6192, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value3_fwd), 0x6192, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths7_reg.mpls_label_value3_snp), 0x6192, 0x0000, 29, 28);

  /* Vtt Trap Strengths8 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg, 0x6193, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.mpls_invalid_label_in_sem_fwd), 0x6193, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.mpls_invalid_label_in_sem_snp), 0x6193, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_version_error_fwd), 0x6193, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_version_error_snp), 0x6193, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_checksum_error_fwd), 0x6193, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_checksum_error_snp), 0x6193, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_header_length_error_fwd), 0x6193, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths8_reg.ipv4_header_length_error_snp), 0x6193, 0x0000, 29, 28);

  /* Vtt Trap Strengths9 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg, 0x6194, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_total_length_error_fwd), 0x6194, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_total_length_error_snp), 0x6194, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_ttl0_fwd), 0x6194, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_ttl0_snp), 0x6194, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_has_options_fwd), 0x6194, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_has_options_snp), 0x6194, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_ttl1_fwd), 0x6194, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths9_reg.ipv4_ttl1_snp), 0x6194, 0x0000, 29, 28);

  /* Vtt Trap Strengths10 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg, 0x6195, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_sip_equal_dip_fwd), 0x6195, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_sip_equal_dip_snp), 0x6195, 0x0000, 5, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_dip_zero_fwd), 0x6195, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_dip_zero_snp), 0x6195, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_sip_is_mc_fwd), 0x6195, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_sip_is_mc_snp), 0x6195, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_fragmented_fwd), 0x6195, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths10_reg.ipv4_fragmented_snp), 0x6195, 0x0000, 29, 28);

  /* Vtt Trap Strengths11 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_strengths11_reg, 0x6196, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths11_reg.header_size_err_over_mpls_fwd), 0x6196, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_strengths11_reg.header_size_err_over_mpls_snp), 0x6196, 0x0000, 5, 4);

  /* Mpls Label Value Over Pwe */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_value_over_pwe_reg, 0x6197, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_over_pwe_reg.mpls_label_value_over_pwe), 0x6197, 0x0000, 3, 0);

  /* Sem Opcode Usage */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg, 0x6198, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_l3[0]), 0x6198, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_l3[1]), 0x6198, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_l3[2]), 0x6198, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_l3[3]), 0x6198, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[0]), 0x6198, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[1]), 0x6198, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[2]), 0x6198, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.sem_opcode_usage_reg.sem_opcode_use_tc_dp[3]), 0x6198, 0x0000, 7, 7);

  /* Cos Profile Usage */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.cos_profile_usage_reg, 0x6199, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.cos_profile_usage_reg.cos_profile_use_l3), 0x6199, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.cos_profile_usage_reg.cos_profile_use_l2), 0x6199, 0x0000, 31, 16);

  /* Mpls Label Value Bos Action Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_value_bos_action_index_reg, 0x619a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_bos_action_index_reg.mpls_label_value_bos_action_index), 0x619a, 0x0000, 31, 0);

  /* Mpls Label Value No Bos Action Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_value_no_bos_action_index_reg, 0x619b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_no_bos_action_index_reg.mpls_label_value_no_bos_action_index), 0x619b, 0x0000, 31, 0);

  /* Mpls Range Configs[0..2] */
  for (reg_ndx = 0; reg_ndx < 3; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx], 0x619c + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx].range_in_rif), 0x619c + reg_ndx, 0x0000, 11, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx].range_in_rif_valid), 0x619c + reg_ndx, 0x0000, 12, 12);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx].range_cos_profile), 0x619c + reg_ndx, 0x0000, 19, 16);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx].range_is_simple), 0x619c + reg_ndx, 0x0000, 20, 20);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_range_configs_reg[reg_ndx].range_is_pipe), 0x619c + reg_ndx, 0x0000, 24, 24);
  }


  /* Mpls Range Configs[0..2] */
  for (reg_ndx = 0; reg_ndx < 3; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_range_base_reg[reg_ndx], 0x619f + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_range_base_reg[reg_ndx].mpls_label_range_base), 0x619f + reg_ndx, 0x0000, 19, 0);
  }

  /* Mpls Label Value Configs[0..15] */
  for (reg_ndx = 0; reg_ndx < 16; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_value_configs_reg[reg_ndx], 0x61a2 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_configs_reg[reg_ndx].mpls_value_in_rif), 0x61a2 + reg_ndx, 0x0000, 11, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_configs_reg[reg_ndx].mpls_value_in_rif_valid), 0x61a2 + reg_ndx, 0x0000, 12, 12);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_configs_reg[reg_ndx].mpls_value_cos_profile), 0x61a2 + reg_ndx, 0x0000, 19, 16);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_value_configs_reg[reg_ndx].mpls_value_model_is_pipe), 0x61a2 + reg_ndx, 0x0000, 20, 20);
  }

  /* Mpls Label Has Ip */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mpls_label_has_ip_reg, 0x61b2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mpls_label_has_ip_reg.mpls_label_has_ip), 0x61b2, 0x0000, 15, 0);

  /* Mef L2cp Transparent Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[0], 0x61b3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[0].mef_l2cp_transparent_bitmap), 0x61b3, 0x0000, 31, 0);

  /* Mef L2cp Transparent Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[1], 0x61b4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[1].mef_l2cp_transparent_bitmap), 0x61b4, 0x0000, 31, 0);

  /* Mef L2cp Transparent Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[2], 0x61b5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[2].mef_l2cp_transparent_bitmap), 0x61b5, 0x0000, 31, 0);

  /* Mef L2cp Transparent Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[3], 0x61b6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.mef_l2cp_transparent_bitmap_reg[3].mef_l2cp_transparent_bitmap), 0x61b6, 0x0000, 31, 0);

  /* Clear Mpls Label Encountered Bit */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.clear_mpls_label_encountered_bit_reg, 0x61b7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.clear_mpls_label_encountered_bit_reg.mpls_label_encountered_index), 0x61b7, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.clear_mpls_label_encountered_bit_reg.mpls_label_encountered_index_is_range), 0x61b7, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.clear_mpls_label_encountered_bit_reg.clear_mpls_label_encountered_bit_trigger), 0x61b7, 0x0000, 20, 20);

  /* Vtt Program Encountered */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_program_encountered_reg, 0x61b8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_program_encountered_reg.vtt_program_first_lookup), 0x61b8, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_program_encountered_reg.vtt_program_second_lookup), 0x61b8, 0x0000, 31, 16);

  /* Vtt Illegal Range Label */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_illegal_range_label_reg, 0x61b9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_illegal_range_label_reg.vtt_illegal_range_label), 0x61b9, 0x0000, 19, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_illegal_range_label_reg.vtt_illegal_range_label_range_index), 0x61b9, 0x0000, 21, 20);

  /* Vtt Trap Encountered1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_encountered1_reg, 0x61ba, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_encountered1_reg.vtt_traps_encountered1), 0x61ba, 0x0000, 31, 0);

  /* Vtt Trap Encountered2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.vtt_trap_encountered2_reg, 0x61bb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.vtt_trap_encountered2_reg.vtt_traps_encountered2), 0x61bb, 0x0000, 12, 0);

  /* Large Em Reset Status Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_reset_status_reg, 0x6200, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_reset_status_reg.large_em_keyt_reset_done), 0x6200, 0x0000, 0, 0);

  /* Large Em Management Unit Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_management_unit_configuration_reg, 0x6206, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_configuration_reg.large_em_mngmnt_unit_enable), 0x6206, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_configuration_reg.large_em_mngmnt_unit_active), 0x6206, 0x0000, 4, 4);

  /* Large Em Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_0, 0x6209, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_0.large_em_mngmnt_unit_failure_valid), 0x6209, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_0.large_em_mngmnt_unit_failure_reason), 0x6209, 0x0000, 11, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_0.large_em_mngmnt_unit_failure_key), 0x6209, 0x0000, 31, 12);

  /* Large Em Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_1, 0x620a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_1.large_em_mngmnt_unit_failure_key), 0x620a, 0x0000, 31, 0);

  /* Large Em Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_2, 0x620b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_management_unit_failure_reg_2.large_em_mngmnt_unit_failure_key), 0x620b, 0x0000, 10, 0);

  /* Large Em Diagnostics */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_reg, 0x620c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_reg.large_em_diagnostics_lookup), 0x620c, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_reg.large_em_diagnostics_read), 0x620c, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_reg.large_em_diagnostics_read_age), 0x620c, 0x0000, 2, 2);

  /* Large Em Diagnostics Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_index_reg, 0x620d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_index_reg.large_em_diagnostics_index), 0x620d, 0x0000, 16, 0);

  /* Large Em Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_key_reg[0], 0x620e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_key_reg[0].large_em_diagnostics_key), 0x620e, 0x0000, 31, 0);

  /* Large Em Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_key_reg[1], 0x620f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_key_reg[1].large_em_diagnostics_key), 0x620f, 0x0000, 30, 0);

  /* Large Em Diagnostics Lookup Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_0, 0x6210, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_found), 0x6210, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_dest), 0x6210, 0x0000, 16, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_asd), 0x6210, 0x0000, 31, 17);

  /* Large Em Diagnostics Lookup Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_1, 0x6211, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_payload_asd), 0x6211, 0x0000, 8, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_payload_is_dyn), 0x6211, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_age_stat), 0x6211, 0x0000, 12, 10);

  /* Large Em Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_read_result_reg_0, 0x6212, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_read_result_reg_0.large_em_entry_valid), 0x6212, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_read_result_reg_0.large_em_entry_key), 0x6212, 0x0000, 31, 1);

  /* Large Em Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_diagnostics_read_result_reg_1, 0x6213, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_diagnostics_read_result_reg_1.large_em_entry_key), 0x6213, 0x0000, 31, 0);

  /* Large Em EMCDefrag Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_emcdefrag_configuration_reg, 0x6214, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_emcdefrag_configuration_reg.large_em_defrag_enable), 0x6214, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_emcdefrag_configuration_reg.large_em_defrag_mode), 0x6214, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_emcdefrag_configuration_reg.large_em_defrag_period), 0x6214, 0x0000, 31, 8);

  /* Large Em Requests Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_requests_counter_reg, 0x6215, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_requests_counter_reg.large_em_requests_counter), 0x6215, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_requests_counter_reg.large_em_requests_counter_overflow), 0x6215, 0x0000, 31, 31);

  /* Large Em Error Delete Unknown Key Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_delete_unknown_key_counter_reg, 0x6217, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_delete_unknown_key_counter_reg.large_em_error_delete_unknown_key_counter), 0x6217, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_delete_unknown_key_counter_reg.large_em_error_delete_unknown_key_counter_overflow), 0x6217, 0x0000, 7, 7);

  /* Large Em Error Reached Max Entry Limit Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_reached_max_entry_limit_counter_reg, 0x6218, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_reached_max_entry_limit_counter_reg.large_em_error_reached_max_entry_limit_counter), 0x6218, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_reached_max_entry_limit_counter_reg.large_em_error_reached_max_entry_limit_counter_overflow), 0x6218, 0x0000, 7, 7);

  /* Large Em Error Cam Table Full Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_error_cam_table_full_counter_reg, 0x6219, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_cam_table_full_counter_reg.large_em_error_cam_table_full_counter), 0x6219, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_error_cam_table_full_counter_reg.large_em_error_cam_table_full_counter_overflow), 0x6219, 0x0000, 7, 7);

  /* Large Em Warning Inserted Existing Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_warning_inserted_existing_counter_reg, 0x621a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_inserted_existing_counter_reg.large_em_warning_inserted_existing_counter), 0x621a, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_warning_inserted_existing_counter_reg.large_em_warning_inserted_existing_counter_overflow), 0x621a, 0x0000, 7, 7);

  /* Large Em Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_entries_counter_reg, 0x621b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_entries_counter_reg.large_em_entries_counter), 0x621b, 0x0000, 16, 0);

  /* Large Em Cam Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.large_em_cam_entries_counter_reg, 0x621c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.large_em_cam_entries_counter_reg.large_em_cam_entries_counter), 0x621c, 0x0000, 5, 0);

  /* Isem Reset Status Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_reset_status_reg, 0x61c0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_reset_status_reg.isem_keyt_reset_done), 0x61c0, 0x0000, 0, 0);

  /* Isem Management Unit Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_management_unit_configuration_reg, 0x61c6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_configuration_reg.isem_mngmnt_unit_enable), 0x61c6, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_configuration_reg.isem_mngmnt_unit_active), 0x61c6, 0x0000, 4, 4);

  /* Isem Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_0, 0x61c9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_0.isem_mngmnt_unit_failure_valid), 0x61c9, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_0.isem_mngmnt_unit_failure_reason), 0x61c9, 0x0000, 11, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_0.isem_mngmnt_unit_failure_key), 0x61c9, 0x0000, 31, 12);

  /* Isem Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_1, 0x61ca, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_management_unit_failure_reg_1.isem_mngmnt_unit_failure_key), 0x61ca, 0x0000, 13, 0);

  /* Isem Diagnostics */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_reg, 0x61cb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_reg.isem_diagnostics_lookup), 0x61cb, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_reg.isem_diagnostics_read), 0x61cb, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_reg.isem_diagnostics_read_age), 0x61cb, 0x0000, 2, 2);

  /* Isem Diagnostics Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_index_reg, 0x61cc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_index_reg.isem_diagnostics_index), 0x61cc, 0x0000, 15, 0);

  /* Isem Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_key_reg[0], 0x61cd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_key_reg[0].isem_diagnostics_key), 0x61cd, 0x0000, 31, 0);

  /* Isem Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_key_reg[1], 0x61ce, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_key_reg[1].isem_diagnostics_key), 0x61ce, 0x0000, 1, 0);

  /* Isem Diagnostics Lookup Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_lookup_result_reg, 0x61cf, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_lookup_result_reg.isem_entry_found), 0x61cf, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_lookup_result_reg.isem_entry_payload), 0x61cf, 0x0000, 16, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_lookup_result_reg.isem_entry_age_stat), 0x61cf, 0x0000, 20, 17);

  /* Isem Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_read_result_reg_0, 0x61d0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_read_result_reg_0.isem_entry_valid), 0x61d0, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_read_result_reg_0.isem_entry_key), 0x61d0, 0x0000, 31, 1);

  /* Isem Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_diagnostics_read_result_reg_1, 0x61d1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_diagnostics_read_result_reg_1.isem_entry_key), 0x61d1, 0x0000, 2, 0);

  /* Isem EMCDefrag Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_emcdefrag_configuration_reg, 0x61d2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_emcdefrag_configuration_reg.isem_defrag_enable), 0x61d2, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_emcdefrag_configuration_reg.isem_defrag_mode), 0x61d2, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_emcdefrag_configuration_reg.isem_defrag_period), 0x61d2, 0x0000, 31, 8);

  /* Isem Requests Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_requests_counter_reg, 0x61d3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_requests_counter_reg.isem_requests_counter), 0x61d3, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_requests_counter_reg.isem_requests_counter_overflow), 0x61d3, 0x0000, 31, 31);

  /* Isem Error Delete Unknown Key Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_error_delete_unknown_key_counter_reg, 0x61d5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_delete_unknown_key_counter_reg.isem_error_delete_unknown_key_counter), 0x61d5, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_delete_unknown_key_counter_reg.isem_error_delete_unknown_key_counter_overflow), 0x61d5, 0x0000, 7, 7);

  /* Isem Error Reached Max Entry Limit Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_error_reached_max_entry_limit_counter_reg, 0x61d6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_reached_max_entry_limit_counter_reg.isem_error_reached_max_entry_limit_counter), 0x61d6, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_reached_max_entry_limit_counter_reg.isem_error_reached_max_entry_limit_counter_overflow), 0x61d6, 0x0000, 7, 7);

  /* Isem Error Cam Table Full Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_error_cam_table_full_counter_reg, 0x61d7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_cam_table_full_counter_reg.isem_error_cam_table_full_counter), 0x61d7, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_error_cam_table_full_counter_reg.isem_error_cam_table_full_counter_overflow), 0x61d7, 0x0000, 7, 7);

  /* Isem Warning Inserted Existing Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_warning_inserted_existing_counter_reg, 0x61d8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_warning_inserted_existing_counter_reg.isem_warning_inserted_existing_counter), 0x61d8, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_warning_inserted_existing_counter_reg.isem_warning_inserted_existing_counter_overflow), 0x61d8, 0x0000, 7, 7);

  /* Isem Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_entries_counter_reg, 0x61d9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_entries_counter_reg.isem_entries_counter), 0x61d9, 0x0000, 15, 0);

  /* Isem Cam Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihp.isem_cam_entries_counter_reg, 0x61da, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihp.isem_cam_entries_counter_reg.isem_cam_entries_counter), 0x61da, 0x0000, 5, 0);
}

/* Block registers initialization: IHB  */
STATIC void
  soc_pb_pp_regs_init_IHB(void)
{
  uint8
    reg_ndx,
    fld_ndx2,
    fld_ndx;


  Soc_pb_pp_regs.ihb.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_IHB;
  Soc_pb_pp_regs.ihb.addr.base = sizeof(uint32) * 0x6400;
  Soc_pb_pp_regs.ihb.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.interrupt_reg, 0x6400, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.bank_a_request_collision_int), 0x6400, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.bank_b_request_collision_int), 0x6400, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.bank_c_request_collision_int), 0x6400, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.bank_d_request_collision_int), 0x6400, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.lem_rpf_not_fec_ptr_int), 0x6400, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.nif_physical_err), 0x6400, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.seq_rx_biger_seq_exp_and_smaller_seq_tx), 0x6400, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.seq_rx_smaller_seq_exp_or_bigger_eq_seq_tx), 0x6400, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.flp_fifo_full), 0x6400, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.flp_lookup_timeout), 0x6400, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.nif_rx_fifo_ovf), 0x6400, 0x0000, 10, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.invalid_address_int), 0x6400, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.lb_vector_index_illegal_calc_int), 0x6400, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_reg.fec_entry_accessed_int), 0x6400, 0x0000, 13, 13);

  /* Interrupt Mask Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.interrupt_mask_reg, 0x6410, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.bank_a_request_collision_int_mask), 0x6410, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.bank_b_request_collision_int_mask), 0x6410, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.bank_c_request_collision_int_mask), 0x6410, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.bank_d_request_collision_int_mask), 0x6410, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.lem_rpf_not_fec_ptr_int_mask), 0x6410, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.nif_physical_err_mask), 0x6410, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.seq_rx_biger_seq_exp_and_smaller_seq_tx_mask), 0x6410, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.seq_rx_smaller_seq_exp_or_bigger_eq_seq_tx_mask), 0x6410, 0x0000, 7, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.flp_fifo_full_mask), 0x6410, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.flp_lookup_timeout_mask), 0x6410, 0x0000, 9, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.nif_rx_fifo_ovf_mask), 0x6410, 0x0000, 10, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.invalid_address_int_mask), 0x6410, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.lb_vector_index_illegal_calc_int_mask), 0x6410, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.fec_entry_accessed_int_mask), 0x6410, 0x0000, 13, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.illegal_bytes_to_remove_value_mask), 0x6410, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.data_center_app), 0x6410, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.interrupt_mask_reg.enable_out_lif_1), 0x6410, 0x0000, 1, 1);

  /* Sequence Tx */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dc_app_reg, 0x6452, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dc_app_reg.dc_app), 0x6452, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dc_app_reg.enable_outlif_1), 0x6452, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dc_app_reg_b0, 0x656c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dc_app_reg_b0.dc_app), 0x656c, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dc_app_reg_b0.enable_outlif_1), 0x656c, 0x0000, 0, 0);

   /* Sequence Tx */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.sequence_tx_reg, 0x6610, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.sequence_tx_reg.sequence_tx), 0x6610, 0x0000, 15, 0);

  /* Sequence Expected */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.sequence_expected_reg, 0x6611, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.sequence_expected_reg.sequence_expected), 0x6611, 0x0000, 15, 0);

  /* Last Error Sequence Rx */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.last_error_sequence_rx_reg, 0x6612, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.last_error_sequence_rx_reg.last_error_sequence_rx), 0x6612, 0x0000, 15, 0);

  /* Good Replies Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.good_replies_counter_reg, 0x6613, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.good_replies_counter_reg.good_replies_counter), 0x6613, 0x0000, 31, 0);

  /* Bad Replies Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.bad_replies_counter_reg, 0x6614, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.bad_replies_counter_reg.bad_replies_counter), 0x6614, 0x0000, 31, 0);

  /* Lookups Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookups_counter_reg, 0x6615, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookups_counter_reg.lookups_counter), 0x6615, 0x0000, 31, 0);

  /* Learns Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.learns_counter_reg, 0x6616, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.learns_counter_reg.learns_counter), 0x6616, 0x0000, 31, 0);

  /* Nif Tx Init Credits */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.nif_tx_init_credits_reg, 0x6617, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.nif_tx_init_credits_reg.nif_tx_init_credits), 0x6617, 0x0000, 4, 0);

  /* Spr Dly Fraction */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.spr_dly_fraction_reg, 0x6618, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.spr_dly_fraction_reg.spr_dly_fraction), 0x6618, 0x0000, 7, 0);

  /* Spr Dly */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.spr_dly_reg, 0x6619, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.spr_dly_reg.spr_dly), 0x6619, 0x0000, 4, 0);

  /* Tx Channel Num */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.tx_channel_num_reg, 0x661a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tx_channel_num_reg.tx_channel_num), 0x661a, 0x0000, 7, 0);

  /* Lookup Control */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookup_control_reg, 0x661b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_control_reg.enable_elk_lookup), 0x661b, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_control_reg.enable_flp_reply), 0x661b, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_control_reg.cpu_lookup_trigger), 0x661b, 0x0000, 2, 2);

  /* Cpu Lookup Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.cpu_lookup_key_reg_0, 0x661c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.cpu_lookup_key_reg_0.cpu_lookup_key), 0x661c, 0x0000, 31, 0);

  /* Cpu Lookup Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.cpu_lookup_key_reg_1, 0x661d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.cpu_lookup_key_reg_1.cpu_lookup_key), 0x661d, 0x0000, 31, 0);

  /* Lookup Reply */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookup_reply_reg, 0x661e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_reply_reg.lookup_reply_valid), 0x661e, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_reply_reg.lookup_reply_error), 0x661e, 0x0000, 1, 1);

  /* Lookup Reply Data */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookup_reply_data_reg_0, 0x661f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_reply_data_reg_0.lookup_reply_data), 0x661f, 0x0000, 31, 0);

  /* Lookup Reply Data */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookup_reply_data_reg_1, 0x6620, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_reply_data_reg_1.lookup_reply_data), 0x6620, 0x0000, 31, 0);

  /* Lookup Reply Seq Num */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lookup_reply_seq_num_reg, 0x6621, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lookup_reply_seq_num_reg.lookup_reply_seq_num), 0x6621, 0x0000, 15, 0);

  /* Nif Physical Err Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.nif_physical_err_counter_reg, 0x6622, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.nif_physical_err_counter_reg.nif_physical_err_counter), 0x6622, 0x0000, 31, 0);

  /* Soc_petra C */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_reg, 0x6623, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_mode), 0x6623, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_lkp_record_size), 0x6623, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_lkp_select), 0x6623, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_lrn_record_size), 0x6623, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_nop_record_size), 0x6623, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_burst_size), 0x6623, 0x0000, 9, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_lkp_packing), 0x6623, 0x0000, 10, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_reg.soc_petra_c_nif_credits), 0x6623, 0x0000, 11, 11);

  /* Soc_petra C */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_IHB_CPU_LOOKUP_KEY_MSB_REG_REG_MULT_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.cpu_lookup_key_msb_reg[reg_ndx], 0x6624 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.cpu_lookup_key_msb_reg[reg_ndx].cpu_lookup_key_msb), 0x6624 + reg_ndx, 0x0000, 31, 0);
  }

  /* Ecmp Lb Key Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ecmp_lb_key_cfg_reg, 0x6561, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_seed), 0x6561, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_cfg_reg.ecmp_lb_hash_index), 0x6561, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_use_in_port), 0x6561, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_shift), 0x6561, 0x0000, 24, 21);

  /* Trap If Accessed[0..1] */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.trap_if_accessed_reg[0], 0x6562, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trap_if_accessed_reg[0].trap_if_accessed), 0x6562, 0x0000, 31, 0);

  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.trap_if_accessed_reg[1], 0x6563, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trap_if_accessed_reg[0].trap_if_accessed), 0x6563, 0x0000, 31, 0);

  /* Icmp Redirect Enable */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.icmp_redirect_enable_reg_0, 0x6564, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_redirect_enable_reg_0.icmp_redirect_enable), 0x6564, 0x0000, 31, 0);

  /* Icmp Redirect Enable */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.icmp_redirect_enable_reg_1, 0x6565, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_redirect_enable_reg_1.icmp_redirect_enable), 0x6565, 0x0000, 31, 0);

  /* Action Profile Uc Rpf */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_uc_rpf_reg, 0x6566, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_uc_rpf_reg.action_profile_uc_strict_rpf_fail_fwd), 0x6566, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_uc_rpf_reg.action_profile_uc_strict_rpf_fail_snp), 0x6566, 0x0000, 4, 3);

  /* Action Profile Mc Rpf */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg, 0x6567, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_explicit_rpf_fail_fwd), 0x6567, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_explicit_rpf_fail_snp), 0x6567, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_as_is_rpf_fail_fwd), 0x6567, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_as_is_rpf_fail_snp), 0x6567, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_rpf_fail_fwd), 0x6567, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_rpf_fail_snp), 0x6567, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_ecmp_fwd), 0x6567, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_mc_rpf_reg.action_profile_mc_use_sip_ecmp_snp), 0x6567, 0x0000, 19, 18);

  /* Action Profile General */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_general_reg, 0x6568, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_facility_invalid_fwd), 0x6568, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_facility_invalid_snp), 0x6568, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_fec_entry_accessed_fwd), 0x6568, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_fec_entry_accessed_snp), 0x6568, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_icmp_redirect_fwd), 0x6568, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_general_reg.action_profile_icmp_redirect_snp), 0x6568, 0x0000, 14, 13);

  /* Dbg Last Resolved Trap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg, 0x6569, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code), 0x6569, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_snoop_code), 0x6569, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_cpu_trap_code_strength), 0x6569, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_snoop_code_strength), 0x6569, 0x0000, 20, 19);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_resolved_trap_reg.dbg_last_resolved_valid), 0x6569, 0x0000, 21, 21);

  /* Dbg Last Trap Change Destination */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_last_trap_change_destination_reg, 0x656a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination), 0x656a, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_last_trap_change_destination_reg.dbg_last_trap_change_destination_valid), 0x656a, 0x0000, 8, 8);

  /* Dbg Fer Trap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_fer_trap_reg, 0x656b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_fer_trap_reg.dbg_fer_trap), 0x656b, 0x0000, 7, 0);

  /* Soc_petrac Shaper Enable */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_shaper_enable_reg, 0x6470, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_shaper_enable_reg.soc_petra_c_shaper_enable), 0x6470, 0x0000, 0, 0);

  /* Action Profile Sa Drop Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg, 0x64a0,0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[0]), 0x64a0, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[0]), 0x64a0, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[1]), 0x64a0, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[1]), 0x64a0, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[2]), 0x64a0, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[2]), 0x64a0, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_fwd[3]), 0x64a0, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_drop_map_reg.action_profile_sa_drop_snp[3]), 0x64a0, 0x0000, 19, 18);

  /* Action Profile Sa Not Found Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg, 0x64a1,0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[0]), 0x64a1, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[0]), 0x64a1, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[1]), 0x64a1, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[1]), 0x64a1, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[2]), 0x64a1, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[2]), 0x64a1, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_fwd[3]), 0x64a1, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_sa_not_found_map_reg.action_profile_sa_not_found_snp[3]), 0x64a1, 0x0000, 19, 18);

  /* P2p Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.p2p_cfg_reg, 0x64a2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.p2p_cfg_reg.action_profile_p2p_misconfiguration_fwd), 0x64a2, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.p2p_cfg_reg.action_profile_p2p_misconfiguration_snp), 0x64a2, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.p2p_cfg_reg.transparent_p2p_asd_learn_prefix), 0x64a2, 0x0000, 14, 5);

  /* Flp General Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.flp_general_cfg_reg, 0x64a4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.forwarding_strength), 0x64a4, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.enable_forwarding_lookup), 0x64a4, 0x0000, 10, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.tcam_access_cycle), 0x64a4, 0x0000, 11, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.elk_enable), 0x64a4, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.egw_full_threshold), 0x64a4, 0x0000, 21, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.action_profile_elk_error_fwd), 0x64a4, 0x0000, 24, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_general_cfg_reg.action_profile_elk_error_snp), 0x64a4, 0x0000, 26, 25);

  /* Action Profile Same Interface */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_same_interface_reg, 0x64a5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_same_interface_reg.action_profile_same_interface_fwd), 0x64a5, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_same_interface_reg.action_profile_same_interface_snp), 0x64a5, 0x0000, 4, 3);

  /* Action Profile Da Not Found Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg, 0x64a6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[0]), 0x64a6, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[0]), 0x64a6, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[1]), 0x64a6, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[1]), 0x64a6, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[2]), 0x64a6, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[2]), 0x64a6, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_fwd[3]), 0x64a6, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.action_profile_da_not_found_map_reg.action_profile_da_not_found_snp[3]), 0x64a6, 0x0000, 19, 18);

  /* All Esadi Rbridges */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.all_esadi_rbridges_reg[0], 0x64a7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.all_esadi_rbridges_reg[0].all_esadi_rbridges), 0x64a7, 0x0000, 31, 0);

  /* All Esadi Rbridges */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.all_esadi_rbridges_reg[1], 0x64a8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.all_esadi_rbridges_reg[1].all_esadi_rbridges), 0x64a8, 0x0000, 15, 0);

  /* Trill Unknown */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.trill_unknown_reg, 0x64a9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trill_unknown_reg.action_profile_trill_unknown_uc_fwd), 0x64a9, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trill_unknown_reg.action_profile_trill_unknown_uc_snp), 0x64a9, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trill_unknown_reg.action_profile_trill_unknown_mc_fwd), 0x64a9, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.trill_unknown_reg.action_profile_trill_unknown_mc_snp), 0x64a9, 0x0000, 9, 8);

  /* Ipv4 Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv4_cfg_reg, 0x64aa, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_cfg_reg.rpf_allow_default_value), 0x64aa, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_cfg_reg.compatible_mc_bridge_fallback), 0x64aa, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_cfg_reg.action_profile_uc_loose_rpf_fwd), 0x64aa, 0x0000, 4, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_cfg_reg.action_profile_uc_loose_rpf_snp), 0x64aa, 0x0000, 6, 5);

  /* Ipv6 Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv6_cfg_reg, 0x64ab, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_cfg_reg.action_profile_default_ucv6_fwd), 0x64ab, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_cfg_reg.action_profile_default_ucv6_snp), 0x64ab, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_cfg_reg.action_profile_default_mcv6_fwd), 0x64ab, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_cfg_reg.action_profile_default_mcv6_snp), 0x64ab, 0x0000, 9, 8);

  /* Lsr Elsp Range Min */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lsr_elsp_range_min_reg, 0x64ac, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_elsp_range_min_reg.elsp_range_min), 0x64ac, 0x0000, 19, 0);

  /* Lsr Elsp Range Max */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lsr_elsp_range_max_reg, 0x64ad, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_elsp_range_max_reg.elsp_range_max), 0x64ad, 0x0000, 19, 0);

  /* Mpls Exp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mpls_exp_map_reg, 0x64ae, 0x0000);
  for(fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_MPLS_EXP_REG_NOF_FLDS; ++fld_ndx)
  {
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mpls_exp_map_reg.mpls_exp[fld_ndx]), 0x64ae, 0x0000, 2 + fld_ndx * 3, 0 + fld_ndx * 3);
  }

  /* VSI and System-VSI for Transparent P2P Pwe service */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.transparent_p2p_pwe_vsi_reg, 0x64af, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.transparent_p2p_pwe_vsi_reg.transparent_p2p_pwe_vsi), 0x64af, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.transparent_p2p_pwe_vsi_reg.transparent_p2p_pwe_system_vsi), 0x64af, 0x0000, 29, 14);

  /* VSI and System-VSI for Transparent P2P AC service */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.transparent_p2p_pwe_vsi_reg, 0x64a3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.transparent_p2p_ac_vsi_reg.transparent_p2p_ac_vsi), 0x64a3, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.transparent_p2p_ac_vsi_reg.transparent_p2p_ac_system_vsi), 0x64a3, 0x0000, 29, 14);

  /* Lsr Action Profile */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lsr_action_profile_reg, 0x64b0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_p2p_no_bos_fwd), 0x64b0, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_p2p_no_bos_snp), 0x64b0, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_control_word_trap_fwd), 0x64b0, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_control_word_trap_snp), 0x64b0, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_control_word_drop_fwd), 0x64b0, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_control_word_drop_snp), 0x64b0, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_unknown_label_fwd), 0x64b0, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_unknown_label_snp), 0x64b0, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_p2p_mplsx4_fwd), 0x64b0, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.action_profile_mpls_p2p_mplsx4_snp), 0x64b0, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lsr_action_profile_reg.parser_pmf_profile_eth_over_pwe), 0x64b0, 0x0000, 28, 25);

  /* Learning Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.learning_cfg_reg, 0x64b1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.learning_cfg_reg.learn_enable_map), 0x64b1, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.learning_cfg_reg.ingress_learning), 0x64b1, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.learning_cfg_reg.opportunistic_learning), 0x64b1, 0x0000, 17, 17);

  /* Mef L2cp Peer Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[0], 0x64b2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[0].mef_l2cp_peer_bitmap), 0x64b2, 0x0000, 31, 0);

  /* Mef L2cp Peer Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[1], 0x64b3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[1].mef_l2cp_peer_bitmap), 0x64b3, 0x0000, 31, 0);

  /* Mef L2cp Peer Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[2], 0x64b4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[2].mef_l2cp_peer_bitmap), 0x64b4, 0x0000, 31, 0);

  /* Mef L2cp Peer Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[3], 0x64b5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_peer_bitmap_reg[3].mef_l2cp_peer_bitmap), 0x64b5, 0x0000, 31, 0);

  /* Mef L2cp Drop Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[0], 0x64b6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[0].mef_l2cp_drop_bitmap), 0x64b6, 0x0000, 31, 0);

  /* Mef L2cp Drop Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[1], 0x64b7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[1].mef_l2cp_drop_bitmap), 0x64b7, 0x0000, 31, 0);

  /* Mef L2cp Drop Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[2], 0x64b8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[2].mef_l2cp_drop_bitmap), 0x64b8, 0x0000, 31, 0);

  /* Mef L2cp Drop Bitmap */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[3], 0x64b9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mef_l2cp_drop_bitmap_reg[3].mef_l2cp_drop_bitmap), 0x64b9, 0x0000, 31, 0);

  /* Ethernet Action Profiles0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ethernet_action_profiles0_reg, 0x64ba, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_peer_fwd), 0x64ba, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_peer_snp), 0x64ba, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_drop_fwd), 0x64ba, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles0_reg.action_profile_eth_mef_l2cp_drop_snp), 0x64ba, 0x0000, 9, 8);

  /* Ethernet Action Profiles1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg, 0x64bb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_membership_query_fwd), 0x64bb, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_membership_query_snp), 0x64bb, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_report_leave_msg_fwd), 0x64bb, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_report_leave_msg_snp), 0x64bb, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_undefined_fwd), 0x64bb, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.igmp_undefined_snp), 0x64bb, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_mc_listener_query_fwd), 0x64bb, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_mc_listener_query_snp), 0x64bb, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_report_done_fwd), 0x64bb, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_report_done_snp), 0x64bb, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_undefined_fwd), 0x64bb, 0x0000, 27, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ethernet_action_profiles1_reg.icmpv6_mld_undefined_snp), 0x64bb, 0x0000, 29, 28);

  /* Ipv4 Action Profiles0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg, 0x64bc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_version_error_fwd), 0x64bc, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_version_error_snp), 0x64bc, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_checksum_version_error_fwd), 0x64bc, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_checksum_version_error_snp), 0x64bc, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_header_length_error_fwd), 0x64bc, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_header_length_error_snp), 0x64bc, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_total_length_error_fwd), 0x64bc, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_total_length_error_snp), 0x64bc, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_ttl0_fwd), 0x64bc, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_ttl0_snp), 0x64bc, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_has_options_fwd), 0x64bc, 0x0000, 27, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles0_reg.action_profile_ipv4_has_options_snp), 0x64bc, 0x0000, 29, 28);

  /* Ipv4 Action Profiles1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg, 0x64bd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_ttl1_fwd), 0x64bd, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_ttl1_snp), 0x64bd, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_equal_dip_fwd), 0x64bd, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_equal_dip_snp), 0x64bd, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_dip_zero_fwd), 0x64bd, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_dip_zero_snp), 0x64bd, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_is_mc_fwd), 0x64bd, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv4_action_profiles1_reg.action_profile_ipv4_sip_is_mc_snp), 0x64bd, 0x0000, 19, 18);

  /* Ipv6 Action Profiles0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg, 0x64be, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_version_error_fwd), 0x64be, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_version_error_snp), 0x64be, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count0_fwd), 0x64be, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count0_snp), 0x64be, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count1_fwd), 0x64be, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_hop_count1_snp), 0x64be, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_unspecified_destination_fwd), 0x64be, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_unspecified_destination_snp), 0x64be, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_loopback_address_fwd), 0x64be, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_loopback_address_snp), 0x64be, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_multicast_source_fwd), 0x64be, 0x0000, 27, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles0_reg.action_profile_ipv6_multicast_source_snp), 0x64be, 0x0000, 29, 28);

  /* Ipv6 Action Profiles1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg, 0x64bf, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_next_header_null_fwd), 0x64bf, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_next_header_null_snp), 0x64bf, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_unspecified_source_fwd), 0x64bf, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_unspecified_source_snp), 0x64bf, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_destination_fwd), 0x64bf, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_destination_snp), 0x64bf, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_destination_fwd), 0x64bf, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_destination_snp), 0x64bf, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_source_fwd), 0x64bf, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_link_local_source_snp), 0x64bf, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_source_fwd), 0x64bf, 0x0000, 27, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles1_reg.action_profile_ipv6_site_local_source_snp), 0x64bf, 0x0000, 29, 28);

  /* Ipv6 Action Profiles2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg, 0x64c0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_compatible_destination_fwd), 0x64c0, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_compatible_destination_snp), 0x64c0, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_mapped_destination_fwd), 0x64c0, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_ipv4_mapped_destination_snp), 0x64c0, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_multicast_destination_fwd), 0x64c0, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ipv6_action_profiles2_reg.action_profile_ipv6_multicast_destination_snp), 0x64c0, 0x0000, 14, 13);

  /* Mpls Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.mpls_action_profiles_reg, 0x64c1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mpls_action_profiles_reg.action_profile_mpls_ttl0_fwd), 0x64c1, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mpls_action_profiles_reg.action_profile_mpls_ttl0_snp), 0x64c1, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mpls_action_profiles_reg.action_profile_mpls_ttl1_fwd), 0x64c1, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.mpls_action_profiles_reg.action_profile_mpls_ttl1_snp), 0x64c1, 0x0000, 9, 8);

  /* Tcp Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg, 0x64c2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_sn_flags_zero_fwd), 0x64c2, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_sn_flags_zero_snp), 0x64c2, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_sn_zero_flags_set_fwd), 0x64c2, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_sn_zero_flags_set_snp), 0x64c2, 0x0000, 9, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_syn_fin_fwd), 0x64c2, 0x0000, 12, 10);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_syn_fin_snp), 0x64c2, 0x0000, 14, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_equal_ports_fwd), 0x64c2, 0x0000, 17, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_equal_ports_snp), 0x64c2, 0x0000, 19, 18);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_incomplete_header_fwd), 0x64c2, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_incomplete_header_snp), 0x64c2, 0x0000, 24, 23);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_offset_lt8_fwd), 0x64c2, 0x0000, 27, 25);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcp_action_profiles_reg.action_profile_tcp_fragment_offset_lt8_snp), 0x64c2, 0x0000, 29, 28);

  /* Udp Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.udp_action_profiles_reg, 0x64c3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.udp_action_profiles_reg.action_profile_udp_equal_ports_fwd), 0x64c3, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.udp_action_profiles_reg.action_profile_udp_equal_ports_snp), 0x64c3, 0x0000, 4, 3);

  /* Icmp Action Profiles */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.icmp_action_profiles_reg, 0x64c4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_action_profiles_reg.action_profile_icmp_data_gt_576_fwd), 0x64c4, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_action_profiles_reg.action_profile_icmp_data_gt_576_snp), 0x64c4, 0x0000, 4, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_action_profiles_reg.action_profile_icmp_fragmented_fwd), 0x64c4, 0x0000, 7, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.icmp_action_profiles_reg.action_profile_icmp_fragmented_snp), 0x64c4, 0x0000, 9, 8);

  /* Dbg Program Selection Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_program_selection_map_reg, 0x64c5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_program_selection_map_reg.dbg_program_selection_map), 0x64c5, 0x0000, 15, 0);

  /* Dbg Flp Trap0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_flp_trap0_reg, 0x64c6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_trap0_reg.dbg_flp_trap0), 0x64c6, 0x0000, 31, 0);

  /* Dbg Flp Trap1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_flp_trap1_reg, 0x64c7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_trap1_reg.dbg_flp_trap1), 0x64c7, 0x0000, 31, 0);

  /* Dbg Flp Trap2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_flp_trap2_reg, 0x64c8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_trap2_reg.dbg_flp_trap2), 0x64c8, 0x0000, 12, 0);

  /* Dbg Flp Fifo */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_flp_fifo_reg, 0x64c9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_fifo_reg.flp_fifo_entry_count), 0x64c9, 0x0000, 8, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_fifo_reg.flp_fifo_last_read_address), 0x64c9, 0x0000, 20, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_flp_fifo_reg.flp_fifo_last_write_address), 0x64c9, 0x0000, 28, 21);

  /* Soc_petra C Chicken bit */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bit_reg, 0x656d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bit_reg.chicken_bit_mc_use_sip_ecmp), 0x656d, 0x0000, 0, 0);

  /* Soc_petra C Cfg */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_cfg_reg, 0x64ca, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_cfg_reg.extended_vrf), 0x64ca, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_cfg_reg.elk_key), 0x64ca, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_cfg_reg.l2_keys_use_ac), 0x64ca, 0x0000, 2, 2);

  /* Soc_petra C ACL Key select */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_acl_key_select_reg, 0x64cb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_acl_key_select_reg.soc_petra_c_acl_key_select), 0x64cb, 0x0000, 1, 0);

  /* Soc_petra C ELK DB ID */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_elk_db_id_reg, 0x64cc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_elk_db_id_reg.soc_petra_c_elk_db_id), 0x64cc, 0x0000, 15, 0);

  /* Soc_petra C UC VRF Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_uc_vrf_config_reg, 0x64cd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_uc_vrf_config_reg.destination), 0x64cd, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_uc_vrf_config_reg.l3_vpn_dflt_routing), 0x64cd, 0x0000, 16, 16);

  /* Soc_petra C MC VRF Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_mc_vrf_config_reg, 0x64ce, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_mc_vrf_config_reg.destination), 0x64ce, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_dflt_mc_vrf_config_reg.l3_vpn_dflt_routing), 0x64ce, 0x0000, 16, 16);

  /* Soc_petra C Chicken Bits */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bits_reg, 0x64cf, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bits_reg.chicken_bit_mldigmp_encapsulation), 0x64cf, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bits_reg.chicken_bit_checksum_err), 0x64cf, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bits_reg.chicken_bit_l2_cp), 0x64cf, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.soc_petra_c_chicken_bits_reg.chicken_bit_zero_elk_reply_if_not_found), 0x64cf, 0x0000, 3, 3);

  /* Non Ethernet Meter Pointer */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.non_ethernet_meter_pointer_reg, 0x6590, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.non_ethernet_meter_pointer_reg.non_ethernet_meter_pointer), 0x6590, 0x0000, 8, 0);

  /* Lpm Query Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg, 0x65b0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg.l2_depth), 0x65b0, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg.l3_depth), 0x65b0, 0x0000, 5, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg.l4_depth), 0x65b0, 0x0000, 8, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg.l5_depth), 0x65b0, 0x0000, 12, 9);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_configuration_reg.vrf_depth), 0x65b0, 0x0000, 16, 13);

  /* Query Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.query_key_reg_0, 0x65b1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_key_reg_0.query_key), 0x65b1, 0x0000, 31, 0);

  /* Query Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.query_key_reg_1, 0x65b2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_key_reg_1.query_key), 0x65b2, 0x0000, 7, 0);

  /* Query Payload */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.query_payload_reg, 0x65b3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_payload_reg.query_trigger), 0x65b3, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_payload_reg.query_payload), 0x65b3, 0x0000, 14, 1);

  /* Lpm Query Cnt */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.lpm_query_cnt_reg, 0x65b4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_cnt_reg.lpm_query_cnt), 0x65b4, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.lpm_query_cnt_reg.lpm_query_cnt_overflow), 0x65b4, 0x0000, 31, 31);

  /* PMFGeneral Configuration1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg, 0x6501, 0x0000);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_REGS_TCAM_NOF_BANKS; ++fld_ndx)
  {
    for (fld_ndx2 = 0; fld_ndx2 < SOC_PB_PP_REGS_TCAM_NOF_CYCLES; ++fld_ndx2)
    {
      SOC_PB_PP_DB_REG_FLD_SET(
        &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.enable_pass_bank_access[fld_ndx][fld_ndx2]),
        0x6501,
        0x0000,
        fld_ndx + (fld_ndx2 * SOC_PB_PP_REGS_TCAM_NOF_BANKS),
        fld_ndx + (fld_ndx2 * SOC_PB_PP_REGS_TCAM_NOF_BANKS)
      );
    }
  }
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.use_default_key_a), 0x6501, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.use_default_key_b), 0x6501, 0x0000, 13, 13);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.fixed_keys_use_ac), 0x6501, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.fixed_keys_use_tcp_control), 0x6501, 0x0000, 15, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.pmf_strength), 0x6501, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.force_program), 0x6501, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmfgeneral_configuration1_reg.force_program_num), 0x6501, 0x0000, 28, 24);

  /* PMF CPU Trap Code Profile */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_PMF_CPU_TRAP_CODE_PRO_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.pmf_cpu_trap_code_profile_reg[reg_ndx], 0x6502 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmf_cpu_trap_code_profile_reg[reg_ndx].pmf_cpu_trap_code_profile), 0x6502 + reg_ndx, 0x0000, 31, 0);
  }

  /* PMF Default TCAM Action */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.pmf_default_tcam_action_reg, 0x650a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmf_default_tcam_action_reg.pmf_default_tcam_action), 0x650a, 0x0000, 31, 0);

  /* Packet Header Size Range[0..2] */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_IHP_PMF_PKT_HDR_SZ_RNG_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.packet_header_size_range_reg[reg_ndx], 0x650b + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.packet_header_size_range_reg[reg_ndx].pkt_hdr_size_range_low), 0x650b + reg_ndx, 0x0000, 6, 0);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.packet_header_size_range_reg[reg_ndx].pkt_hdr_size_range_high), 0x650b + reg_ndx, 0x0000, 14, 8);
  }

  /* ECMP LB Key Select */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg, 0x6511, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[0]), 0x6511, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[1]), 0x6511, 0x0000, 6, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[2]), 0x6511, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[3]), 0x6511, 0x0000, 14, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[4]), 0x6511, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[5]), 0x6511, 0x0000, 22, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[6]), 0x6511, 0x0000, 26, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[7]), 0x6511, 0x0000, 30, 28);

  /* Dbg Eth Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_0, 0x6524, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_0.dbg_eth_key), 0x6524, 0x0000, 31, 0);

  /* Dbg Eth Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_1, 0x6525, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_1.dbg_eth_key), 0x6525, 0x0000, 31, 0);

  /* Dbg Eth Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_2, 0x6526, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_2.dbg_eth_key), 0x6526, 0x0000, 31, 0);

  /* Dbg Eth Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_3, 0x6527, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_3.dbg_eth_key), 0x6527, 0x0000, 31, 0);

  /* Dbg Eth Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_4, 0x6528, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_eth_key_reg_4.dbg_eth_key), 0x6528, 0x0000, 15, 0);

  /* Dbg IPv4 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_0, 0x6529, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_0.dbg_ipv4_key), 0x6529, 0x0000, 31, 0);

  /* Dbg IPv4 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_1, 0x652a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_1.dbg_ipv4_key), 0x652a, 0x0000, 31, 0);

  /* Dbg IPv4 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_2, 0x652b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_2.dbg_ipv4_key), 0x652b, 0x0000, 31, 0);

  /* Dbg IPv4 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_3, 0x652c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_3.dbg_ipv4_key), 0x652c, 0x0000, 31, 0);

  /* Dbg IPv4 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_4, 0x652d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv4_key_reg_4.dbg_ipv4_key), 0x652d, 0x0000, 15, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_0, 0x652e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_0.dbg_ipv6_key), 0x652e, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_1, 0x652f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_1.dbg_ipv6_key), 0x652f, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_2, 0x6530, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_2.dbg_ipv6_key), 0x6530, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_3, 0x6531, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_3.dbg_ipv6_key), 0x6531, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_4, 0x6532, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_4.dbg_ipv6_key), 0x6532, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_5, 0x6533, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_5.dbg_ipv6_key), 0x6533, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_6, 0x6534, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_6.dbg_ipv6_key), 0x6534, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_7, 0x6535, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_7.dbg_ipv6_key), 0x6535, 0x0000, 31, 0);

  /* Dbg IPv6 Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_8, 0x6536, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_ipv6_key_reg_8.dbg_ipv6_key), 0x6536, 0x0000, 31, 0);

  /* Dbg FEM[0..7]Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_femkey_reg, 0x653a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_femkey_reg.dbg_femkey), 0x653a, 0x0000, 31, 0);

  /* Dbg FEM[0..7] */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.dbg_fem_reg, 0x653b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_fem_reg.dbg_femaction_valid), 0x653b, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_fem_reg.dbg_femaction_type), 0x653b, 0x0000, 7, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_fem_reg.dbg_femaction), 0x653b, 0x0000, 24, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.dbg_fem_reg.dbg_femprogram), 0x653b, 0x0000, 29, 28);

  /* Tcam Controller */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.tcam_controller_reg, 0x65e0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_controller_reg.tcam_ctrl_trigger), 0x65e0, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_controller_reg.tcam_ctrl_bank), 0x65e0, 0x0000, 2, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_controller_reg.tcam_ctrl_src), 0x65e0, 0x0000, 11, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_controller_reg.tcam_ctrl_dst), 0x65e0, 0x0000, 20, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_controller_reg.tcam_ctrl_range), 0x65e0, 0x0000, 29, 21);

  /* Pmf Query Cnt */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.pmf_query_cnt_reg, 0x65e1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmf_query_cnt_reg.pmf_query_cnt), 0x65e1, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.pmf_query_cnt_reg.pmf_query_cnt_overflow), 0x65e1, 0x0000, 31, 31);

  /* Flp Query Cnt */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.flp_query_cnt_reg, 0x65e2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_query_cnt_reg.flp_query_cnt), 0x65e2, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.flp_query_cnt_reg.flp_query_cnt_overflow), 0x65e2, 0x0000, 31, 31);

  /* Egq Query Cnt */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.egq_query_cnt_reg, 0x65e3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.egq_query_cnt_reg.egq_query_cnt), 0x65e3, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.egq_query_cnt_reg.egq_query_cnt_overflow), 0x65e3, 0x0000, 31, 31);

  /* Query Cnt Enable */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.query_cnt_enable_reg, 0x65e4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_cnt_enable_reg.pmf_query_cnt_enable), 0x65e4, 0x0000, 3, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_cnt_enable_reg.flp_query_cnt_enable), 0x65e4, 0x0000, 7, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.query_cnt_enable_reg.egq_query_cnt_enable), 0x65e4, 0x0000, 11, 8);

  /* Tcam Bist Wr */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg, 0x65e6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.tmode), 0x65e6, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_en), 0x65e6, 0x0000, 4, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_mode), 0x65e6, 0x0000, 6, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_resume), 0x65e6, 0x0000, 10, 7);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_tck), 0x65e6, 0x0000, 14, 11);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_se), 0x65e6, 0x0000, 18, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_wr_reg.smbist_si), 0x65e6, 0x0000, 22, 19);

  /* Tcam Bist Re */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.ihb.tcam_bist_re_reg, 0x65e7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_re_reg.smbist_on), 0x65e7, 0x0000, 3, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_re_reg.smbist_done), 0x65e7, 0x0000, 7, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_re_reg.smbist_fail), 0x65e7, 0x0000, 11, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.ihb.tcam_bist_re_reg.smbist_so), 0x65e7, 0x0000, 15, 12);
}

/* Block registers initialization: EGQ  */
STATIC void
  soc_pb_pp_regs_init_EGQ(void)
{
  uint8
    reg_ndx,
    fld_ndx;

  Soc_pb_pp_regs.egq.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_EGQ;
  Soc_pb_pp_regs.egq.addr.base = sizeof(uint32) * 0x5800;
  Soc_pb_pp_regs.egq.addr.step = sizeof(uint32) * 0x0000;

  /* Interrupt Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.interrupt_reg, 0x5800, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.interrupt_reg.cud_isnt_out_vsi_int), 0x5800, 0x0000, 13, 13);

  /* Bypass System Vsi Em */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.bypass_system_vsi_em_reg, 0x5a50, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.bypass_system_vsi_em_reg.bypass_system_vsi_em), 0x5a50, 0x0000, 0, 0);

  /* Svem Lookup Arbiter Egress Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_lookup_arbiter_egress_lookup_counter_reg, 0x5a51, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_lookup_arbiter_egress_lookup_counter_reg.svem_lookup_arbiter_egress_lookup_counter), 0x5a51, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_lookup_arbiter_egress_lookup_counter_reg.svem_lookup_arbiter_egress_lookup_counter_overflow), 0x5a51, 0x0000, 31, 31);

  /* Svem Lookup Arbiter Large Em Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_lookup_arbiter_large_em_lookup_counter_reg, 0x5a52, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_lookup_arbiter_large_em_lookup_counter_reg.svem_lookup_arbiter_large_em_lookup_counter), 0x5a52, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_lookup_arbiter_large_em_lookup_counter_reg.svem_lookup_arbiter_large_em_lookup_counter_overflow), 0x5a52, 0x0000, 31, 31);

  /* Svem Last Lookup Egress */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_last_lookup_egress_reg, 0x5a53, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_egress_reg.svem_last_lookup_egress_key), 0x5a53, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_egress_reg.svem_last_lookup_egress_result), 0x5a53, 0x0000, 29, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_egress_reg.svem_last_lookup_egress_result_found), 0x5a53, 0x0000, 30, 30);

  /* Svem Last Lookup Large Em */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_last_lookup_large_em_reg, 0x5a54, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_large_em_reg.svem_last_lookup_large_em_key), 0x5a54, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_large_em_reg.svem_last_lookup_large_em_result), 0x5a54, 0x0000, 29, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_last_lookup_large_em_reg.svem_last_lookup_large_em_result_found), 0x5a54, 0x0000, 30, 30);

  /* Svem Reset Status Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_reset_status_reg, 0x5a5a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_reset_status_reg.svem_keyt_reset_done), 0x5a5a, 0x0000, 0, 0);

  /* Svem Management Unit Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_management_unit_configuration_reg, 0x5a60, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_management_unit_configuration_reg.svem_mngmnt_unit_enable), 0x5a60, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_management_unit_configuration_reg.svem_mngmnt_unit_active), 0x5a60, 0x0000, 4, 4);

  /* Svem Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_management_unit_failure_reg, 0x5a63, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_management_unit_failure_reg.svem_mngmnt_unit_failure_valid), 0x5a63, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_management_unit_failure_reg.svem_mngmnt_unit_failure_reason), 0x5a63, 0x0000, 11, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_management_unit_failure_reg.svem_mngmnt_unit_failure_key), 0x5a63, 0x0000, 27, 12);

  /* Svem Diagnostics */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_diagnostics_reg, 0x5a64, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_reg.svem_diagnostics_lookup), 0x5a64, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_reg.svem_diagnostics_read), 0x5a64, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_reg.svem_diagnostics_read_age), 0x5a64, 0x0000, 2, 2);

  /* Svem Diagnostics Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_diagnostics_index_reg, 0x5a65, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_index_reg.svem_diagnostics_index), 0x5a65, 0x0000, 13, 0);

  /* Svem Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_diagnostics_key_reg, 0x5a66, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_key_reg.svem_diagnostics_key), 0x5a66, 0x0000, 15, 0);

  /* Svem Diagnostics Lookup Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_diagnostics_lookup_result_reg, 0x5a67, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_lookup_result_reg.svem_entry_found), 0x5a67, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_lookup_result_reg.svem_entry_payload), 0x5a67, 0x0000, 12, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_lookup_result_reg.svem_entry_age_stat), 0x5a67, 0x0000, 16, 13);

  /* Svem Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_diagnostics_read_result_reg, 0x5a68, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_read_result_reg.svem_entry_valid), 0x5a68, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_diagnostics_read_result_reg.svem_entry_key), 0x5a68, 0x0000, 16, 1);

  /* Svem EMCDefrag Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_emcdefrag_configuration_reg, 0x5a69, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_emcdefrag_configuration_reg.svem_defrag_enable), 0x5a69, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_emcdefrag_configuration_reg.svem_defrag_mode), 0x5a69, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_emcdefrag_configuration_reg.svem_defrag_period), 0x5a69, 0x0000, 31, 8);

  /* Svem Requests Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_requests_counter_reg, 0x5a6a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_requests_counter_reg.svem_requests_counter), 0x5a6a, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_requests_counter_reg.svem_requests_counter_overflow), 0x5a6a, 0x0000, 31, 31);

  /* Svem Error Delete Unknown Key Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_error_delete_unknown_key_counter_reg, 0x5a6c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_delete_unknown_key_counter_reg.svem_error_delete_unknown_key_counter), 0x5a6c, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_delete_unknown_key_counter_reg.svem_error_delete_unknown_key_counter_overflow), 0x5a6c, 0x0000, 7, 7);

  /* Svem Error Reached Max Entry Limit Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_error_reached_max_entry_limit_counter_reg, 0x5a6d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_reached_max_entry_limit_counter_reg.svem_error_reached_max_entry_limit_counter), 0x5a6d, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_reached_max_entry_limit_counter_reg.svem_error_reached_max_entry_limit_counter_overflow), 0x5a6d, 0x0000, 7, 7);

  /* Svem Error Cam Table Full Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_error_cam_table_full_counter_reg, 0x5a6e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_cam_table_full_counter_reg.svem_error_cam_table_full_counter), 0x5a6e, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_error_cam_table_full_counter_reg.svem_error_cam_table_full_counter_overflow), 0x5a6e, 0x0000, 7, 7);

  /* Svem Warning Inserted Existing Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_warning_inserted_existing_counter_reg, 0x5a6f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_warning_inserted_existing_counter_reg.svem_warning_inserted_existing_counter), 0x5a6f, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_warning_inserted_existing_counter_reg.svem_warning_inserted_existing_counter_overflow), 0x5a6f, 0x0000, 7, 7);

  /* Svem Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_entries_counter_reg, 0x5a70, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_entries_counter_reg.svem_entries_counter), 0x5a70, 0x0000, 13, 0);

  /* Svem Cam Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.svem_cam_entries_counter_reg, 0x5a71, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.svem_cam_entries_counter_reg.svem_cam_entries_counter), 0x5a71, 0x0000, 5, 0);

  /* EHPGeneral Settings */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg, 0x588d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.disable_pkt_filter), 0x588d, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.disable_same_port_filter), 0x588d, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.tm_raw_port_learn_disable), 0x588d, 0x0000, 2, 2);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.cpu_port_learn_disable), 0x588d, 0x0000, 3, 3);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.custom_learn), 0x588d, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.cfg_dot1_ad), 0x588d, 0x0000, 5, 5);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.ad_acl_arp_en), 0x588d, 0x0000, 6, 6);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.error_code_mvr), 0x588d, 0x0000, 10, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.auxiliary_data_table_mode), 0x588d, 0x0000, 13, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.ip_mc_translate_vsi), 0x588d, 0x0000, 14, 14);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.nwk_offset_add), 0x588d, 0x0000, 15, 15);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.split_horizon_filter), 0x588d, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.disable_bounce_back_en), 0x588d, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ehpgeneral_settings_reg.bounce_back_threshold), 0x588d, 0x0000, 27, 21);

  /* Deny Packet Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.deny_packet_counter_reg, 0x588e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.deny_packet_counter_reg.deny_pkt_cnt), 0x588e, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.deny_packet_counter_reg.deny_pkt_cnt_ovf), 0x588e, 0x0000, 31, 31);

  /* Eep Tunnel Range */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.eep_tunnel_range_reg, 0x589a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.eep_tunnel_range_reg.eep_tunnel_range_bottom), 0x589a, 0x0000, 13, 0);

  /* Outlif Error Code */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.outlif_error_code_reg_0, 0x589b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.outlif_error_code_reg_0.outlif_error_code), 0x589b, 0x0000, 31, 0);

  /* Outlif Error Code */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.outlif_error_code_reg_1, 0x589c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.outlif_error_code_reg_1.outlif_error_code), 0x589c, 0x0000, 31, 0);

  /* Outlif Error Code */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.outlif_error_code_reg_2, 0x589d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.outlif_error_code_reg_2.outlif_error_code), 0x589d, 0x0000, 31, 0);

  /* Trill Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.trill_config_reg, 0x589e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.trill_config_reg.tpid_profile_trill), 0x589e, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.trill_config_reg.trill_default_vid), 0x589e, 0x0000, 15, 4);

  /* Ethernet Types Reg0 */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_REGS; ++reg_ndx)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.ethernet_types_reg[reg_ndx], 0x589f + reg_ndx, 0x0000);
    for (fld_ndx = 0; fld_ndx < SOC_PB_PP_EGQ_ETHERNET_TYPES_NOF_FLDS; ++fld_ndx)
    {
      SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ethernet_types_reg[reg_ndx].eth_type[fld_ndx]), 0x589f + reg_ndx, 0x0000, 15 + (16 * fld_ndx), 0 + (16 * fld_ndx));
    }
  }

  /* Action Profile1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.action_profile1_reg, 0x58a4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_no_vsi_translation), 0x58a4, 0x0000, 7, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_dss_stacking), 0x58a4, 0x0000, 11, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_lag_multicast), 0x58a4, 0x0000, 15, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_exclude_src), 0x58a4, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_vlan_membership), 0x58a4, 0x0000, 23, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_unacceptable_frame_type), 0x58a4, 0x0000, 27, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile1_reg.action_profile_illegal_eep), 0x58a4, 0x0000, 31, 28);

  /* Action Profile2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.action_profile2_reg, 0x58a5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_src_equal_dest), 0x58a5, 0x0000, 3, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_unknown_da), 0x58a5, 0x0000, 7, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_split_horizon), 0x58a5, 0x0000, 11, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_private_vlan), 0x58a5, 0x0000, 15, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_ttl_scope), 0x58a5, 0x0000, 19, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_mtu_violation), 0x58a5, 0x0000, 23, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_trill_ttl_zero), 0x58a5, 0x0000, 27, 24);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile2_reg.action_profile_trill_same_interface), 0x58a5, 0x0000, 31, 28);

  /* Action Profile3 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.action_profile3_reg, 0x58a6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.action_profile3_reg.action_profile_bounce_back), 0x58a6, 0x0000, 3, 0);

  /* Acceptable Frame Type Table */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[0], 0x58a7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[0].acceptable_frame_type_table), 0x58a7, 0x0000, 31, 0);

  /* Acceptable Frame Type Table */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[1], 0x58a8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[1].acceptable_frame_type_table), 0x58a8, 0x0000, 31, 0);

  /* Acceptable Frame Type Table */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[2], 0x58a9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[2].acceptable_frame_type_table), 0x58a9, 0x0000, 31, 0);

  /* Acceptable Frame Type Table */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[3], 0x58aa, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acceptable_frame_type_reg[3].acceptable_frame_type_table), 0x58aa, 0x0000, 31, 0);

  /* Private Vlan Filter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.private_vlan_filter_reg, 0x58ab, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.private_vlan_filter_reg.private_vlan_filter), 0x58ab, 0x0000, 15, 0);

  /* Ttl Scope */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.ttl_scope_reg_0, 0x58ac, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ttl_scope_reg_0.ttl_scope), 0x58ac, 0x0000, 31, 0);

  /* Ttl Scope */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.ttl_scope_reg_1, 0x58ad, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.ttl_scope_reg_1.ttl_scope), 0x58ad, 0x0000, 31, 0);

  /* Erpp Debug Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.erpp_debug_configuration_reg, 0x58b6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.erpp_debug_configuration_reg.debug_select), 0x58b6, 0x0000, 2, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.erpp_debug_configuration_reg.erpp_error_code), 0x58b6, 0x0000, 6, 4);

  /* ACL General Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acl_general_configuration_reg, 0x58b7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.enable_action_profile), 0x58b7, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.enable_out_tm_port), 0x58b7, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.enable_cud_action), 0x58b7, 0x0000, 8, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.enable_tc_action), 0x58b7, 0x0000, 12, 12);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.enable_dp_action), 0x58b7, 0x0000, 16, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.acl_dp[0]), 0x58b7, 0x0000, 21, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.acl_dp[1]), 0x58b7, 0x0000, 23, 22);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.acl_general_configuration_reg.high_priority_acl), 0x58b7, 0x0000, 24, 24);

  /* ACL TCAM Access Enabler */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.acl_tcam_access_enabler_reg, 0x58b8, 0x0000);
  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_REGS_TCAM_NOF_BANKS; ++fld_ndx)
  {
    SOC_PB_PP_DB_REG_FLD_SET(
      &(Soc_pb_pp_regs.egq.acl_tcam_access_enabler_reg.enable_tcam_bank_access[fld_ndx]), \
      0x58b8, 0x0000,
      fld_ndx,
      fld_ndx
     );
  }

  /* L4 Protocol Code Reg0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.l4_protocol_code_reg[0], 0x58b9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[0].l4_protocol_code[0]), 0x58b9, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[0].l4_protocol_code[1]), 0x58b9, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[0].l4_protocol_code[2]), 0x58b9, 0x0000, 23, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[0].l4_protocol_code[3]), 0x58b9, 0x0000, 31, 24);

  /* L4 Protocol Code Reg1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.l4_protocol_code_reg[1], 0x58ba, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[1].l4_protocol_code[0]), 0x58ba, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[1].l4_protocol_code[1]), 0x58ba, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[1].l4_protocol_code[2]), 0x58ba, 0x0000, 23, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[1].l4_protocol_code[3]), 0x58ba, 0x0000, 31, 24);

  /* L4 Protocol Code Reg2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.l4_protocol_code_reg[2], 0x58bb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[2].l4_protocol_code[0]), 0x58bb, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[2].l4_protocol_code[1]), 0x58bb, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[2].l4_protocol_code[2]), 0x58bb, 0x0000, 23, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[2].l4_protocol_code[3]), 0x58bb, 0x0000, 31, 24);

  /* L4 Protocol Code Reg3 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.egq.l4_protocol_code_reg[3], 0x58bc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[3].l4_protocol_code[0]), 0x58bc, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[3].l4_protocol_code[1]), 0x58bc, 0x0000, 15, 8);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.egq.l4_protocol_code_reg[3].l4_protocol_code[2]), 0x58bc, 0x0000, 23, 16);
}

/* Block registers initialization: EPNI  */
STATIC void
  soc_pb_pp_regs_init_EPNI(void)
{
  uint8
    reg_ndx;

  Soc_pb_pp_regs.epni.nof_instances = SOC_PB_PP_BLK_NOF_INSTANCES_EPNI;
  Soc_pb_pp_regs.epni.addr.base = sizeof(uint32) * 0x3a00;
  Soc_pb_pp_regs.epni.addr.step = sizeof(uint32) * 0x0000;

  /* Esem Reset Status Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_reset_status_reg, 0x3b0a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_reset_status_reg.esem_keyt_reset_done), 0x3b0a, 0x0000, 0, 0);

  /* Esem Management Unit Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_management_unit_configuration_reg, 0x3b10, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_configuration_reg.esem_mngmnt_unit_enable), 0x3b10, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_configuration_reg.esem_mngmnt_unit_active), 0x3b10, 0x0000, 4, 4);

  /* Esem Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_0, 0x3b13, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_valid), 0x3b13, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_reason), 0x3b13, 0x0000, 11, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_0.esem_mngmnt_unit_failure_key), 0x3b13, 0x0000, 31, 12);

  /* Esem Management Unit Failure */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_1, 0x3b14, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_management_unit_failure_reg_1.esem_mngmnt_unit_failure_key), 0x3b14, 0x0000, 0, 0);

  /* Esem Diagnostics */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_diagnostics_reg, 0x3b15, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_reg.esem_diagnostics_lookup), 0x3b15, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_reg.esem_diagnostics_read), 0x3b15, 0x0000, 1, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_reg.esem_diagnostics_read_age), 0x3b15, 0x0000, 2, 2);

  /* Esem Diagnostics Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_diagnostics_index_reg, 0x3b16, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_index_reg.esem_diagnostics_index), 0x3b16, 0x0000, 13, 0);

  /* Esem Diagnostics Key */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_diagnostics_key_reg, 0x3b17, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_key_reg.esem_diagnostics_key), 0x3b17, 0x0000, 20, 0);

  /* Esem Diagnostics Lookup Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_diagnostics_lookup_result_reg, 0x3b18, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_lookup_result_reg.esem_entry_found), 0x3b18, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_lookup_result_reg.esem_entry_payload), 0x3b18, 0x0000, 15, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_lookup_result_reg.esem_entry_age_stat), 0x3b18, 0x0000, 19, 16);

  /* Esem Diagnostics Read Result */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_diagnostics_read_result_reg, 0x3b19, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_read_result_reg.esem_entry_valid), 0x3b19, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_diagnostics_read_result_reg.esem_entry_key), 0x3b19, 0x0000, 21, 1);

  /* Esem EMCDefrag Configuration Register */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_emcdefrag_configuration_reg, 0x3b1a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_emcdefrag_configuration_reg.esem_defrag_enable), 0x3b1a, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_emcdefrag_configuration_reg.esem_defrag_mode), 0x3b1a, 0x0000, 4, 4);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_emcdefrag_configuration_reg.esem_defrag_period), 0x3b1a, 0x0000, 31, 8);

  /* Esem Requests Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_requests_counter_reg, 0x3b1b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_requests_counter_reg.esem_requests_counter), 0x3b1b, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_requests_counter_reg.esem_requests_counter_overflow), 0x3b1b, 0x0000, 31, 31);

  /* Esem Error Delete Unknown Key Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_error_delete_unknown_key_counter_reg, 0x3b1d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_delete_unknown_key_counter_reg.esem_error_delete_unknown_key_counter), 0x3b1d, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_delete_unknown_key_counter_reg.esem_error_delete_unknown_key_counter_overflow), 0x3b1d, 0x0000, 7, 7);

  /* Esem Error Reached Max Entry Limit Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_error_reached_max_entry_limit_counter_reg, 0x3b1e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_reached_max_entry_limit_counter_reg.esem_error_reached_max_entry_limit_counter), 0x3b1e, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_reached_max_entry_limit_counter_reg.esem_error_reached_max_entry_limit_counter_overflow), 0x3b1e, 0x0000, 7, 7);

  /* Esem Error Cam Table Full Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_error_cam_table_full_counter_reg, 0x3b1f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_cam_table_full_counter_reg.esem_error_cam_table_full_counter), 0x3b1f, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_error_cam_table_full_counter_reg.esem_error_cam_table_full_counter_overflow), 0x3b1f, 0x0000, 7, 7);

  /* Esem Warning Inserted Existing Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_warning_inserted_existing_counter_reg, 0x3b20, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_warning_inserted_existing_counter_reg.esem_warning_inserted_existing_counter), 0x3b20, 0x0000, 6, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_warning_inserted_existing_counter_reg.esem_warning_inserted_existing_counter_overflow), 0x3b20, 0x0000, 7, 7);

  /* Esem Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_entries_counter_reg, 0x3b21, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_entries_counter_reg.esem_entries_counter), 0x3b21, 0x0000, 13, 0);

  /* Esem Cam Entries Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_cam_entries_counter_reg, 0x3b22, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_cam_entries_counter_reg.esem_cam_entries_counter), 0x3b22, 0x0000, 5, 0);

  /* Esem Lookup Arbiter Lookup Counter */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_lookup_arbiter_lookup_counter_reg, 0x3b00, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_lookup_arbiter_lookup_counter_reg.esem_lookup_arbiter_lookup_counter), 0x3b00, 0x0000, 30, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_lookup_arbiter_lookup_counter_reg.esem_lookup_arbiter_lookup_counter_overflow), 0x3b00, 0x0000, 31, 31);

  /* Esem Last Lookup */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_last_lookup_reg_0, 0x3b01, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_last_lookup_reg_0.esem_last_lookup_key), 0x3b01, 0x0000, 20, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_last_lookup_reg_0.esem_last_lookup_result), 0x3b01, 0x0000, 31, 21);

  /* Esem Last Lookup */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_last_lookup_reg_1, 0x3b02, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_last_lookup_reg_1.esem_last_lookup_result), 0x3b02, 0x0000, 3, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_last_lookup_reg_1.esem_last_lookup_result_found), 0x3b02, 0x0000, 4, 4);

  /* Esem Int Reg Mask */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_int_reg, 0x3a02, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_int_reg.esem_management_completed), 0x3a02, 0x0000, 6, 6);

  /* Esem Int Reg Mask */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.esem_int_reg_mask_reg, 0x3a12, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.esem_int_reg_mask_reg.esem_int_reg_mask), 0x3a12, 0x0000, 6, 0);

  /* General Pp Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.general_pp_config_reg, 0x3a79, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.general_pp_config_reg.mvr_enable), 0x3a79, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.general_pp_config_reg.p2p_vsi), 0x3a79, 0x0000, 14, 1);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.general_pp_config_reg.tpid_profile_trill), 0x3a79, 0x0000, 18, 16);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.general_pp_config_reg.nwk_offset_add), 0x3a79, 0x0000, 20, 20);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.general_pp_config_reg.auxiliary_data_table_mode), 0x3a79, 0x0000, 25, 24);

  /* Eep Limit */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.eep_limit_reg, 0x3a7a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.eep_limit_reg.eep_link_layer_limit), 0x3a7a, 0x0000, 13, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.eep_limit_reg.eep_ip_tunnel_limit), 0x3a7a, 0x0000, 29, 16);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[0], 0x3a7b, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[0].ipv4_exp_to_tos_map), 0x3a7b, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[1], 0x3a7c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[1].ipv4_exp_to_tos_map), 0x3a7c, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[2], 0x3a7d, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[2].ipv4_exp_to_tos_map), 0x3a7d, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[3], 0x3a7e, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[3].ipv4_exp_to_tos_map), 0x3a7e, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[4], 0x3a7f, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[4].ipv4_exp_to_tos_map), 0x3a7f, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[5], 0x3a80, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[5].ipv4_exp_to_tos_map), 0x3a80, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[6], 0x3a81, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[6].ipv4_exp_to_tos_map), 0x3a81, 0x0000, 31, 0);

  /* Ipv4 Exp To Tos Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[7], 0x3a82, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_exp_to_tos_map_reg[7].ipv4_exp_to_tos_map), 0x3a82, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[0], 0x3a83, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[0].ipv6_exp_to_tc_map), 0x3a83, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[1], 0x3a84, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[1].ipv6_exp_to_tc_map), 0x3a84, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[2], 0x3a85, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[2].ipv6_exp_to_tc_map), 0x3a85, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[3], 0x3a86, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[3].ipv6_exp_to_tc_map), 0x3a86, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[4], 0x3a87, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[4].ipv6_exp_to_tc_map), 0x3a87, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[5], 0x3a88, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[5].ipv6_exp_to_tc_map), 0x3a88, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[6], 0x3a89, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[6].ipv6_exp_to_tc_map), 0x3a89, 0x0000, 31, 0);

  /* Ipv6 Exp To Tc Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[7], 0x3a8a, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv6_exp_to_tc_map_reg[7].ipv6_exp_to_tc_map), 0x3a8a, 0x0000, 31, 0);

  /* Trill Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.trill_config_reg, 0x3a8c, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.trill_config_reg.trill_nickname), 0x3a8c, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.trill_config_reg.trill_op_cnt), 0x3a8c, 0x0000, 21, 16);

  /* REG DEF CHANGE: Changed to array of regs */
  for (reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    /* Mpls Profile */
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mpls_profile_reg[reg_ndx], 0x3a8d + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mpls_profile_reg[reg_ndx].mpls_profile), 0x3a8d + reg_ndx, 0x0000, 31, 0);
  }

  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS; reg_ndx++)
  {
    /* Mpls Exp Map Table */
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mpls_exp_map_table_reg[reg_ndx], 0x3a91 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mpls_exp_map_table_reg[reg_ndx].mpls_exp_map_table), 0x3a91 + reg_ndx, 0x0000, 31, 0);
  }

  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_IP_DSCP_TO_EXP_MAP_REG_MULT_NOF_REGS; reg_ndx++)
  {
    /* Ip Dscp To Exp Map */
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ip_dscp_to_exp_map_reg[reg_ndx], 0x3a93 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ip_dscp_to_exp_map_reg[reg_ndx].ip_dscp_to_exp_map), 0x3a93 + reg_ndx, 0x0000, 31, 0);
  }

  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_IPV4_TOS_REG_MULT_NOF_REGS; reg_ndx++)
  {
    /* Ipv4 Tos */
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_tos_reg[reg_ndx], 0x3a99 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_tos_reg[reg_ndx].ipv4_tos), 0x3a99 + reg_ndx, 0x0000, 31, 0);
  }

  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_IPV4_TTL_REG_MULT_NOF_REGS; reg_ndx++)
  {
    /* Ipv4 Ttl */
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_ttl_reg[reg_ndx], 0x3a9d + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_ttl_reg[reg_ndx].ipv4_ttl), 0x3a9d + reg_ndx, 0x0000, 31, 0);
  }

  /* Ipv4 Sip */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_IPV4_SIP_REG_MULT_NOF_REGS; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ipv4_sip_reg[reg_ndx], 0x3aa1 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ipv4_sip_reg[reg_ndx].ipv4_sip), 0x3aa1 + reg_ndx, 0x0000, 31, 0);
  }

  /* Sa Msb */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sa_msb_reg_0, 0x3ab1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sa_msb_reg_0.sa_msb), 0x3ab1, 0x0000, 31, 0);

  /* Sa Msb */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sa_msb_reg_1, 0x3ab2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sa_msb_reg_1.sa_msb), 0x3ab2, 0x0000, 9, 0);

  /* Bsa */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.bsa_reg_0, 0x3ab3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.bsa_reg_0.bsa), 0x3ab3, 0x0000, 31, 0);

  /* Bsa */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.bsa_reg_1, 0x3ab4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.bsa_reg_1.bsa), 0x3ab4, 0x0000, 15, 0);

  /* Itag Tc Dp Map */
  for(reg_ndx = 0; reg_ndx < SOC_PB_PP_EPNI_ITAG_TC_DP_MAP_REG_MULT_NOF_REGS; reg_ndx++)
  {
    SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.itag_tc_dp_map_regs[reg_ndx], 0x3ab5 + reg_ndx, 0x0000);
    SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.itag_tc_dp_map_regs[reg_ndx].itag_tc_dp_map), 0x3ab5 + reg_ndx, 0x0000, 31, 0);
  }
  
  /* Itag Tc Dp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_1, 0x3ab6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_1.itag_tc_dp_map), 0x3ab6, 0x0000, 31, 0);

  /* Itag Tc Dp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_2, 0x3ab7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_2.itag_tc_dp_map), 0x3ab7, 0x0000, 31, 0);

  /* Itag Tc Dp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_3, 0x3ab8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.itag_tc_dp_map_reg_3.itag_tc_dp_map), 0x3ab8, 0x0000, 31, 0);

  /* Tpid Profile Mac In Mac */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.tpid_profile_mac_in_mac_reg, 0x3ab9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.tpid_profile_mac_in_mac_reg.tpid_profile_mac_in_mac), 0x3ab9, 0x0000, 2, 0);

  /* Tpid Profile System */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.tpid_profile_system_reg, 0x3aba, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.tpid_profile_system_reg.tpid_profile_system), 0x3aba, 0x0000, 2, 0);

  /* Acceptable Frame Type */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.acceptable_frame_type_reg[0], 0x3abb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.acceptable_frame_type_reg[0].acceptable_frame_type), 0x3abb, 0x0000, 31, 0);

  /* Acceptable Frame Type */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.acceptable_frame_type_reg[1], 0x3abc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.acceptable_frame_type_reg[1].acceptable_frame_type), 0x3abc, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_0, 0x3abd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_0.sem_opcode_offset), 0x3abd, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_1, 0x3abe, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_1.sem_opcode_offset), 0x3abe, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_2, 0x3abf, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_2.sem_opcode_offset), 0x3abf, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_3, 0x3ac0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_3.sem_opcode_offset), 0x3ac0, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_4, 0x3ac1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_4.sem_opcode_offset), 0x3ac1, 0x0000, 31, 0);

  /* Sem Opcode Offset */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_5, 0x3ac2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.sem_opcode_offset_reg_5.sem_opcode_offset), 0x3ac2, 0x0000, 31, 0);

  /* Default Sem Index */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.default_sem_index_reg, 0x3ac3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.default_sem_index_reg.default_sem_index), 0x3ac3, 0x0000, 13, 0);

  /* Mirror Vid Reg0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mirror_vid_reg0_reg, 0x3ac4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg0_reg.mirror_vid0), 0x3ac4, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg0_reg.mirror_vid1), 0x3ac4, 0x0000, 23, 12);

  /* Mirror Vid Reg1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mirror_vid_reg1_reg, 0x3ac5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg1_reg.mirror_vid2), 0x3ac5, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg1_reg.mirror_vid3), 0x3ac5, 0x0000, 23, 12);

  /* Mirror Vid Reg2 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mirror_vid_reg2_reg, 0x3ac6, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg2_reg.mirror_vid4), 0x3ac6, 0x0000, 11, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg2_reg.mirror_vid5), 0x3ac6, 0x0000, 23, 12);

  /* Mirror Vid Reg3 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mirror_vid_reg3_reg, 0x3ac7, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mirror_vid_reg3_reg.mirror_vid6), 0x3ac7, 0x0000, 11, 0);

  /* Counter Base Reg0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_base_reg0_reg, 0x3ac8, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_base_reg0_reg.counter_base_tm), 0x3ac8, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_base_reg0_reg.counter_base_vsi), 0x3ac8, 0x0000, 31, 16);

  /* Counter Base Reg1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_base_reg1_reg, 0x3ac9, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_base_reg1_reg.counter_base_outlif), 0x3ac9, 0x0000, 15, 0);

  /* Counter Range Low Reg0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_range_low_reg0_reg, 0x3aca, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_low_reg0_reg.counter_range_low_tm), 0x3aca, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_low_reg0_reg.counter_range_low_vsi), 0x3aca, 0x0000, 31, 16);

  /* Counter Range Low Reg1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_range_low_reg1_reg, 0x3acb, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_low_reg1_reg.counter_range_low_outlif), 0x3acb, 0x0000, 15, 0);

  /* Counter Range High Reg0 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_range_high_reg0_reg, 0x3acc, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_high_reg0_reg.counter_range_high_tm), 0x3acc, 0x0000, 15, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_high_reg0_reg.counter_range_high_vsi), 0x3acc, 0x0000, 31, 16);

  /* Counter Range High Reg1 */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_range_high_reg1_reg, 0x3acd, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_range_high_reg1_reg.counter_range_high_outlif), 0x3acd, 0x0000, 15, 0);

  /* Counter Dp Map */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.counter_dp_map_reg, 0x3ace, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.counter_dp_map_reg.counter_dp_map), 0x3ace, 0x0000, 7, 0);

  /* Ttl Decrement Enable */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.ttl_decrement_enable_reg, 0x3ae0, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ttl_decrement_enable_reg.ttl_decrement_enable), 0x3ae0, 0x0000, 0, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.ttl_decrement_enable_reg.ttl_tunnel_disable), 0x3ae0, 0x0000, 1, 1);

  /* Custom Config */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.custom_config_reg, 0x3ae1, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.custom_config_reg.ip_protocol_custom), 0x3ae1, 0x0000, 7, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.custom_config_reg.ethernet_type_custom), 0x3ae1, 0x0000, 23, 8);

  /* Mpls Control Word */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.mpls_control_word_reg, 0x3ae2, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.mpls_control_word_reg.mpls_control_word), 0x3ae2, 0x0000, 31, 0);

  /* Pcp Dei Profile Use Tc Dp */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.pcp_dei_profile_use_tc_dp_reg, 0x3ae3, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.pcp_dei_profile_use_tc_dp_reg.pcp_dei_profile_use_tc_dp), 0x3ae3, 0x0000, 15, 0);

  /* Count Out Of Range */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.count_out_of_range_reg, 0x3ae4, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.count_out_of_range_reg.count_out_of_range), 0x3ae4, 0x0000, 3, 0);

  /* Etpp Debug Configuration */
  SOC_PB_PP_DB_REG_SET(Soc_pb_pp_regs.epni.etpp_debug_configuration_reg, 0x3ae5, 0x0000);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.etpp_debug_configuration_reg.debug_select), 0x3ae5, 0x0000, 3, 0);
  SOC_PB_PP_DB_REG_FLD_SET( &(Soc_pb_pp_regs.epni.etpp_debug_configuration_reg.debug_wr), 0x3ae5, 0x0000, 4, 4);
}

uint32
  soc_pb_pp_regs_get(
    SOC_SAND_OUT SOC_PB_PP_REGS  **soc_pb_pp_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_GET);

  if (Soc_pb_pp_regs_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_REGS_NOT_INITIALIZED, 20, exit);
  }

  *soc_pb_pp_regs = &Soc_pb_pp_regs;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_regs_get()", 0, 0);
}

uint32
  soc_pb_pp_regs_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGS_INIT);

  if (Soc_pb_pp_regs_initialized == TRUE)
  {
    goto exit;
  }

  soc_pb_pp_regs_init_ECI();
  soc_pb_pp_regs_init_OLP();
  soc_pb_pp_regs_init_IHP();
  soc_pb_pp_regs_init_IHB();
  soc_pb_pp_regs_init_EGQ();
  soc_pb_pp_regs_init_EPNI();

  Soc_pb_pp_regs_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_regs_init", 0, 0);
}

SOC_PB_PP_REGS*
  soc_pb_pp_regs()
{
  SOC_PB_PP_REGS*
    regs = NULL;

  soc_pb_pp_regs_get(
    &(regs)
  );

  return regs;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
