/* $Id: pb_pp_chip_tbls.c,v 1.8 Broadcom SDK $
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
* FILENAME:       soc_pb_pp_chip_tbls.c
*
* MODULE PREFIX:  chip_tbls
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
 
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
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

#define SOC_PB_PP_KEY_PGM_INSTR_SKIP            (0x20)

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PB_PP_TBLS
  Soc_pb_pp_tbls;
static uint8
  Soc_pb_pp_tbls_initialized = FALSE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* soc_pb_pp_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT SOC_PB_PP_TBLS  **soc_pb_pp_tbls - pointer to soc_pb_pp_
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_pp_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_pb_pp_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_pp_tbls_get(
    SOC_SAND_OUT SOC_PB_PP_TBLS  **soc_pb_pp_tbls
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TBLS_GET);

  if (Soc_pb_pp_tbls_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBLS_NOT_INITIALIZED, 10, exit);
  }

  *soc_pb_pp_tbls = &Soc_pb_pp_tbls;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_is_tbl_db_initialized()", 0, 0);
}

/*****************************************************
*NAME
* soc_pb_pp_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *soc_pb_pp_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
SOC_PB_PP_TBLS*
  soc_pb_pp_tbls(void)
{

  SOC_PB_PP_TBLS*
    tbls = NULL;

  soc_pb_pp_tbls_get(
    &tbls
  );

  return tbls;
}

/*****************************************************
*NAME
* soc_pb_pp_tbl_fld_set
*TYPE:
*  PROC
*FUNCTION:
*  Sets a soc_pb_pp_ table field
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_PETRA_TBL_FIELD *field - pointer to soc_pb_pp_ table field structure
*    uint8  msb -  field most significant bit
*    uint8  lsb -  field least significant bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_pb_pp_tbl_fld_set(
    SOC_PETRA_TBL_FIELD *field,
    uint16      msb,
    uint16      lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

/* Block tables initialization: IHP */
STATIC void
  soc_pb_pp_tbls_init_IHP(void)
{

  /* Port Mine Table Physical Port */
  Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.addr.base = 0x01300000;
  Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.physical_port_mine0), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.physical_port_mine1), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.physical_port_mine2), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_mine_table_physical_port_tbl.physical_port_mine3), 3, 3);
  /* Pinfo Llr */
  Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.addr.base = 0x00200000;
  Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.addr.width_bits = 66;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.vlan_classification_profile), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.pcp_profile), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.ignore_incoming_pcp), 5, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.tc_dp_tos_enable), 6, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.tc_dp_tos_index), 7, 7);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.tc_l4_protocol_enable), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.tc_subnet_enable), 9, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.tc_port_protocol_enable), 10, 10);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.default_traffic_class), 13, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.ignore_incoming_up), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.incoming_up_map_profile), 16, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.use_dei), 17, 17);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.drop_precedence_profile), 19, 18);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.sa_lookup_enable), 20, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.action_profile_sa_drop_index), 22, 21);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.enable_sa_authentication), 23, 23);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.ignore_incoming_vid), 24, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.vid_ipv4_subnet_enable), 25, 25);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.vid_port_protocol_enable), 26, 26);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.protocol_profile), 29, 27);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.default_initial_vid), 41, 30);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.non_authorized_mode_8021x), 42, 42);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.reserved_mc_trap_profile), 44, 43);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.enable_arp_trap), 45, 45);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.enable_igmp_trap), 46, 46);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.enable_mld_trap), 47, 47);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.enable_dhcp_trap), 48, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.general_trap_enable), 52, 49);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.default_cpu_trap_code), 60, 53);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.default_action_profile_fwd), 63, 61);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pinfo_llr_tbl.default_action_profile_snp), 65, 64);

  /* Llr Llvp */
  Soc_pb_pp_tbls.ihp.llr_llvp_tbl.addr.base = 0x00210000;
  Soc_pb_pp_tbls.ihp.llr_llvp_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihp.llr_llvp_tbl.addr.width_bits = 7;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.incoming_vid_exist), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.incoming_tag_exist), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.incoming_stag_exist), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.acceptable_frame_type_action), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.outer_comp_index), 5, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llr_llvp_tbl.inner_comp_index), 6, 6);

  /* Ll Mirror Profile */
  Soc_pb_pp_tbls.ihp.ll_mirror_profile_tbl.addr.base = 0x00220000;
  Soc_pb_pp_tbls.ihp.ll_mirror_profile_tbl.addr.size = 0x0200;
  Soc_pb_pp_tbls.ihp.ll_mirror_profile_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.ll_mirror_profile_tbl.ll_mirror_profile), 3, 0);

  /* Subnet Classify */
  Soc_pb_pp_tbls.ihp.subnet_classify_tbl.addr.base = 0x00230000;
  Soc_pb_pp_tbls.ihp.subnet_classify_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihp.subnet_classify_tbl.addr.width_bits = 54;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet), 31, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet_mask), 36, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet_vid), 48, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet_tc), 51, 49);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet_tc_valid), 52, 52);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.subnet_classify_tbl.ipv4_subnet_valid), 53, 53);

  /* Port Protocol */
  Soc_pb_pp_tbls.ihp.port_protocol_tbl.addr.base = 0x00240000;
  Soc_pb_pp_tbls.ihp.port_protocol_tbl.addr.size = 0x0080;
  Soc_pb_pp_tbls.ihp.port_protocol_tbl.addr.width_bits = 17;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_protocol_tbl.vid), 11, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_protocol_tbl.vid_valid), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_protocol_tbl.traffic_class), 15, 13);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.port_protocol_tbl.traffic_class_valid), 16, 16);

  /* Tos 2 Cos */
  Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.addr.base = 0x00250000;
  Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.addr.width_bits = 6;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.valid), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.tc), 3, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tos_2_cos_tbl.dp), 5, 4);

  /* Reserved Mc */
  Soc_pb_pp_tbls.ihp.reserved_mc_tbl.addr.base = 0x00260000;
  Soc_pb_pp_tbls.ihp.reserved_mc_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihp.reserved_mc_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.reserved_mc_tbl.fwd), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.reserved_mc_tbl.snp), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.reserved_mc_tbl.cpu_trap_code), 7, 5);

  /* Large Em Pldt Format1 */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_tbl.format_specific_data), 40, 16);

  /* Large Em Pldt Format1 Asd Tp2p */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.out_lif), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.out_lif_valid), 32, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.identifier), 37, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_tp2p_tbl.p2p_service), 40, 40);

  /* Large Em Pldt Format1 Asd Pbp */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.learn_destination), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.isid_domain), 37, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.learn_type), 38, 38);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.sa_drop), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_pbp_tbl.is_dynamic), 40, 40);

  /* Large Em Pldt Format1 Asd Ilm */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.mpls_label), 35, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.mpls_command), 39, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ilm_tbl.p2p_service), 40, 40);

  /* Large Em Pldt Format1 Asd Sp2p */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.out_lif), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.out_lif_valid), 32, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.tpid_profile), 38, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.has_cw), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_sp2p_tbl.p2p_service), 40, 40);

  /* Large Em Pldt Format1 Asd Ethernet */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.asd), 39, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format1_asd_ethernet_tbl.is_dynamic), 40, 40);

  /* Large Em Pldt Format2 */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_tbl.permitted_system_port), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_tbl.format_specific_data), 40, 16);

  /* Large Em Pldt Format2 Trill */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.permitted_system_port), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.adjacent), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.drop_if_vid_differ), 36, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.permit_all_ports), 37, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.override_vid_in_tagged), 38, 38);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.use_vid_in_untagged), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_trill_tbl.is_dynamic), 40, 40);

  /* Large Em Pldt Format2 Auth */
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.permitted_system_port), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.vid), 27, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.accept_untagged), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.drop_if_vid_differ), 36, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.permit_all_ports), 37, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.override_vid_in_tagged), 38, 38);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.use_vid_in_untagged), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_pldt_format2_auth_tbl.is_dynamic), 40, 40);

  /* Flush Db */
  Soc_pb_pp_tbls.ihp.flush_db_tbl.addr.base = 0x00760000;
  Soc_pb_pp_tbls.ihp.flush_db_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihp.flush_db_tbl.addr.width_bits = 198;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.compare_valid), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.compare_key_16_msbs_data), 16, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.compare_key_16_msbs_mask), 32, 17);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.compare_payload_data), 73, 33);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.compare_payload_mask), 114, 74);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.action_drop), 115, 115);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.action_transplant_payload_data), 156, 116);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.flush_db_tbl.action_transplant_payload_mask), 197, 157);

  /* Large Em Fid Counter Db */
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_db_tbl.addr.base = 0x00780000;
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_db_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_db_tbl.addr.width_bits = 19;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_db_tbl.entry_count), 14, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_db_tbl.profile_pointer), 18, 16);

  /* Large Em Fid Counter Profile Db */
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.addr.base = 0x00790000;
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.addr.width_bits = 18;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.limit), 14, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.interrupt_en), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.message_en), 16, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_fid_counter_profile_db_tbl.is_link_layer_fid), 17, 17);

  /* Large Em Aging Configuration Table */
  Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.addr.base = 0x007a0000;
  Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.addr.size = 0x0020;
  Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.addr.width_bits = 6;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.delete_entry[FALSE]), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.create_aged_out_event[FALSE]), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.create_refresh_event[FALSE]), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.delete_entry[TRUE]), 3, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.create_aged_out_event[TRUE]), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.large_em_aging_configuration_table_tbl.create_refresh_event[TRUE]), 5, 5);

  /* Tm Port Pp Context Config */
  Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.addr.base = 0x00020000;
  Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.addr.size = 0x0050;
  Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.addr.width_bits = 22;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.pp_context_offset1), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.pp_context_offset2), 13, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.pp_context_profile), 17, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tm_port_pp_context_config_tbl.pp_context_value_to_use), 21, 20);

  /* Pp Port Info */
  Soc_pb_pp_tbls.ihp.pp_port_info_tbl.addr.base = 0x00030000;
  Soc_pb_pp_tbls.ihp.pp_port_info_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.pp_port_info_tbl.addr.width_bits = 10;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_info_tbl.tpid_profile), 9, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_info_tbl.outer_header_start), 5, 0);

  /* Pp Port Values */
  Soc_pb_pp_tbls.ihp.pp_port_values_tbl.addr.base = 0x00040000;
  Soc_pb_pp_tbls.ihp.pp_port_values_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.pp_port_values_tbl.addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_values_tbl.pp_port_packet_format_qualifier0_value), 18, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_values_tbl.pp_port_pp_context_value), 22, 20);

  /* Pp Port Fem Bit Select Table */
  Soc_pb_pp_tbls.ihp.pp_port_fem_bit_select_table_tbl.addr.base = 0x00050000;
  Soc_pb_pp_tbls.ihp.pp_port_fem_bit_select_table_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.pp_port_fem_bit_select_table_tbl.addr.width_bits = 5;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_bit_select_table_tbl.pp_port_fem_bit_select), 4, 0);

  /* Pp Port Fem Map Index Table */
  Soc_pb_pp_tbls.ihp.pp_port_fem_map_index_table_tbl.addr.base = 0x00060000;
  Soc_pb_pp_tbls.ihp.pp_port_fem_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.pp_port_fem_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_map_index_table_tbl.pp_port_fem_map_index), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_map_index_table_tbl.pp_port_fem_map_data), 7, 4);

  /* Pp Port Fem Field Select Map */
  Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.addr.base = 0x00070000;
  Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.addr.width_bits = 46;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[0]), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[1]), 13, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[2]), 21, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[3]), 29, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[4]), 37, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_port_fem_field_select_map_tbl.pp_port_fem_field_select[5]), 45, 40);

  /* Src System Port Fem Bit Select Table */
  Soc_pb_pp_tbls.ihp.src_system_port_fem_bit_select_table_tbl.addr.base = 0x00080000;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_bit_select_table_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_bit_select_table_tbl.addr.width_bits = 5;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_bit_select_table_tbl.src_system_port_fem_bit_select), 4, 0);

  /* Src System Port Fem Map Index Table */
  Soc_pb_pp_tbls.ihp.src_system_port_fem_map_index_table_tbl.addr.base = 0x00090000;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_map_index_table_tbl.src_system_port_fem_map_index), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_map_index_table_tbl.src_system_port_fem_map_data), 7, 4);

  /* Src System Port Fem Field Select Map */
  Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.addr.base = 0x000a0000;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.addr.width_bits = 102;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[0]), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[1]), 13, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[2]), 21, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[3]), 29, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[4]), 37, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[5]), 45, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[6]), 53, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[7]), 61, 56);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[8]), 69, 64);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[9]), 77, 72);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[10]), 85, 80);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[11]), 93, 88);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.src_system_port_fem_field_select_map_tbl.src_system_port_fem_field_select[12]), 101, 96);

  /* Pp Context Fem Bit Select Table */
  Soc_pb_pp_tbls.ihp.pp_context_fem_bit_select_table_tbl.addr.base = 0x000b0000;
  Soc_pb_pp_tbls.ihp.pp_context_fem_bit_select_table_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.pp_context_fem_bit_select_table_tbl.addr.width_bits = 5;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_bit_select_table_tbl.pp_context_fem_bit_select), 4, 0);

  /* Pp Context Fem Map Index Table */
  Soc_pb_pp_tbls.ihp.pp_context_fem_map_index_table_tbl.addr.base = 0x000c0000;
  Soc_pb_pp_tbls.ihp.pp_context_fem_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.pp_context_fem_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_map_index_table_tbl.pp_context_fem_map_index), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_map_index_table_tbl.pp_context_fem_map_data), 7, 4);

  /* Pp Context Fem Field Select Map */
  Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.addr.base = 0x000d0000;
  Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.addr.size = 0x0004;
  Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.addr.width_bits = 46;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[0]), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[1]), 13, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[2]), 21, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[3]), 29, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[4]), 37, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pp_context_fem_field_select_map_tbl.pp_context_fem_field_select[5]), 45, 40);

  /* Parser Program1 */
  Soc_pb_pp_tbls.ihp.parser_program1_tbl.addr.base = 0x000e0000;
  Soc_pb_pp_tbls.ihp.parser_program1_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.ihp.parser_program1_tbl.addr.width_bits = 11;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_program1_tbl.next_addr_base), 6, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_program1_tbl.macro_sel), 10, 8);

  /* Parser Program2 */
  Soc_pb_pp_tbls.ihp.parser_program2_tbl.addr.base = 0x000f0000;
  Soc_pb_pp_tbls.ihp.parser_program2_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.ihp.parser_program2_tbl.addr.width_bits = 11;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_program2_tbl.next_addr_base), 6, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_program2_tbl.macro_sel), 10, 8);

  /* Packet Format Table */
  Soc_pb_pp_tbls.ihp.packet_format_table_tbl.addr.base = 0x00100000;
  Soc_pb_pp_tbls.ihp.packet_format_table_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.ihp.packet_format_table_tbl.addr.width_bits = 14;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.packet_format_table_tbl.packet_format_code), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.packet_format_table_tbl.parser_pmf_profile), 11, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.packet_format_table_tbl.key_program_profile), 13, 12);



  /* Parser Custom Macro Parameters */
  Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.addr.base = 0x00110000;
  Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.addr.width_bits = 65;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_word_select), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_mask_left), 8, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_mask_right), 16, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_condition_select), 24, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_comparator_mode), 29, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_comparator_mask), 46, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_shift_sel), 48, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_shift_a), 56, 52);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_parameters_tbl.cstm_shift_b), 64, 60);

  /* Parser Eth Protocols */
  Soc_pb_pp_tbls.ihp.parser_eth_protocols_tbl.addr.base = 0x00120000;
  Soc_pb_pp_tbls.ihp.parser_eth_protocols_tbl.addr.size = 0x0007;
  Soc_pb_pp_tbls.ihp.parser_eth_protocols_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_eth_protocols_tbl.eth_type_protocol), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_eth_protocols_tbl.eth_sap_protocol), 31, 16);

  /* Parser Ip Protocols */
  Soc_pb_pp_tbls.ihp.parser_ip_protocols_tbl.addr.base = 0x00130000;
  Soc_pb_pp_tbls.ihp.parser_ip_protocols_tbl.addr.size = 0x0007;
  Soc_pb_pp_tbls.ihp.parser_ip_protocols_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_ip_protocols_tbl.ip_protocol), 7, 0);

  /* Parser Custom Macro Protocols */
  Soc_pb_pp_tbls.ihp.parser_custom_macro_protocols_tbl.addr.base = 0x00140000;
  Soc_pb_pp_tbls.ihp.parser_custom_macro_protocols_tbl.addr.size = 0x000f;
  Soc_pb_pp_tbls.ihp.parser_custom_macro_protocols_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.parser_custom_macro_protocols_tbl.cstm_protocol), 31, 0);

  /* Isem Management Request */
  Soc_pb_pp_tbls.ihp.isem_management_request_tbl.addr.base = 0x00900000;
  Soc_pb_pp_tbls.ihp.isem_management_request_tbl.addr.size = 0x0001;
  Soc_pb_pp_tbls.ihp.isem_management_request_tbl.addr.width_bits = 65;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_type), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_stamp), 10, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_key), 44, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_payload), 60, 45);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_age_status), 62, 61);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_self), 63, 63);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.isem_management_request_tbl.isem_refreshed_by_dsp), 64, 64);

  /* Vsi Isid */
  Soc_pb_pp_tbls.ihp.vsi_isid_tbl.addr.base = 0x01100000;
  Soc_pb_pp_tbls.ihp.vsi_isid_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.vsi_isid_tbl.addr.width_bits = 24;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_isid_tbl.vsi_or_isid), 23, 0);

  /* Vsi My Mac */
  Soc_pb_pp_tbls.ihp.vsi_my_mac_tbl.addr.base = 0x01110000;
  Soc_pb_pp_tbls.ihp.vsi_my_mac_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.vsi_my_mac_tbl.addr.width_bits = 7;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_my_mac_tbl.my_mac_lsb), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_my_mac_tbl.enable_my_mac), 6, 6);

  /* Vsi Topology Id */
  Soc_pb_pp_tbls.ihp.vsi_topology_id_tbl.addr.base = 0x01120000;
  Soc_pb_pp_tbls.ihp.vsi_topology_id_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.vsi_topology_id_tbl.addr.width_bits = 6;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_topology_id_tbl.vsi_topology_id), 5, 0);

  /* Vsi Fid Class */
  Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.addr.base = 0x01130000;
  Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.addr.width_bits = 12;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.fid_class[0]), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.fid_class[1]), 5, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.fid_class[2]), 8, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_fid_class_tbl.fid_class[3]), 11, 9);

  /* Vsi Da Not Found */
  Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.addr.base = 0x01140000;
  Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.profile_index[0]), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.profile_index[1]), 3, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.profile_index[2]), 5, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_da_not_found_tbl.profile_index[3]), 7, 6);

  /* Bvd Topology Id */
  Soc_pb_pp_tbls.ihp.bvd_topology_id_tbl.addr.base = 0x01150000;
  Soc_pb_pp_tbls.ihp.bvd_topology_id_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.bvd_topology_id_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_topology_id_tbl.bvd_topology_id), 3, 0);

  /* Bvd Fid Class */
  Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.addr.base = 0x01160000;
  Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.fid_class[0]), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.fid_class[1]), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.fid_class[2]), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_fid_class_tbl.fid_class[3]), 3, 3);

  /* Bvd Da Not Found */
  Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.addr.base = 0x01170000;
  Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.profile_index[0]), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.profile_index[1]), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.profile_index[2]), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.bvd_da_not_found_tbl.profile_index[3]), 3, 3);

  /* Fid Class 2 Fid */
  Soc_pb_pp_tbls.ihp.fid_class_2_fid_tbl.addr.base = 0x01180000;
  Soc_pb_pp_tbls.ihp.fid_class_2_fid_tbl.addr.size = 0x0007;
  Soc_pb_pp_tbls.ihp.fid_class_2_fid_tbl.addr.width_bits = 14;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.fid_class_2_fid_tbl.fid), 13, 0);

  /* Vlan Range Compression Table */
  Soc_pb_pp_tbls.ihp.vlan_range_compression_table_tbl.addr.base = 0x00300000;
  Soc_pb_pp_tbls.ihp.vlan_range_compression_table_tbl.addr.size = 0x0020;
  Soc_pb_pp_tbls.ihp.vlan_range_compression_table_tbl.addr.width_bits = 24;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vlan_range_compression_table_tbl.vlan_range_lower_limit), 11, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vlan_range_compression_table_tbl.vlan_range_upper_limit), 23, 12);

  /* Vtt In Pp Port Vlan Config */
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_vlan_config_tbl.addr.base = 0x00310000;
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_vlan_config_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_vlan_config_tbl.addr.width_bits = 64;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_vlan_config_tbl.range_valid0), 31, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_vlan_config_tbl.range_valid1), 63, 32);

  /* Designated Vlan Table */
  Soc_pb_pp_tbls.ihp.designated_vlan_table_tbl.addr.base = 0x00320000;
  Soc_pb_pp_tbls.ihp.designated_vlan_table_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihp.designated_vlan_table_tbl.addr.width_bits = 12;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.designated_vlan_table_tbl.designated_vlan), 11, 0);

  /* Vsi Port Membership */
  Soc_pb_pp_tbls.ihp.vsi_port_membership_tbl.addr.base = 0x00330000;
  Soc_pb_pp_tbls.ihp.vsi_port_membership_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.vsi_port_membership_tbl.addr.width_bits = 64;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vsi_port_membership_tbl.vlan_port_member_line), 63, 0);

  /* Vtt In Pp Port Config */
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.addr.base = 0x00340000;
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.addr.width_bits = 41;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.port_is_pbp), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.vlan_domain), 9, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.small_em_key_profile), 14, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.designated_vlan_index), 18, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.default_sem_base), 33, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.default_sem_opcode), 37, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_in_pp_port_config_tbl.learn_ac), 40, 40);

  /* Sem Result Table */
  Soc_pb_pp_tbls.ihp.sem_result_table_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_tbl.sem_result_table), 61, 0);

  /* Sem Result Table Ac P2p To Ac */
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.out_lif), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.vlan_edit_vid), 27, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.vlan_edit_pcp_dei_profile), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.vlan_edit_profile), 34, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.mef_l2_cp_profile), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.cfm_max_level), 38, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.cfm_trap_valid), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_ac_tbl.type), 61, 60);

  /* Sem Result Table Ac P2p To Pwe */
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.out_lif_or_vc_label), 23, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.vlan_edit_pcp_dei_profile), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.vlan_edit_profile), 34, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.mef_l2_cp_profile), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.cfm_max_level), 38, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.cfm_trap_valid), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pwe_tbl.type), 61, 60);

  /* Sem Result Table Ac P2p To Pbb */
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.isid), 23, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.vlan_edit_pcp_dei_profile), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.vlan_edit_profile), 34, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.mef_l2_cp_profile), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.cfm_max_level), 38, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.cfm_trap_valid), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_p2p_to_pbb_tbl.type), 61, 60);

  /* Sem Result Table Ac Mp */
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.tt_learn_enable), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.orientation_is_hub), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.vlan_edit_vid), 27, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.vlan_edit_pcp_dei_profile), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.vlan_edit_profile), 34, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.mef_l2_cp_profile), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.cfm_max_level), 38, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.cfm_trap_valid), 39, 39);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.learn_destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ac_mp_tbl.type), 61, 60);

  /* Sem Result Table Isid P2p */
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.isidor_out_lifor_vc_label), 37, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.tpid_profile_index), 39, 38);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.service_type_lsb), 60, 60);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_p2p_tbl.type), 61, 61);

  /* Sem Result Table Isid Mp */
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.destination_valid), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.orientation_is_hub), 16, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.tt_learn_enable), 17, 17);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.tpid_profile), 39, 38);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.cos_profile), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.service_type_lsb), 60, 60);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_isid_mp_tbl.type), 61, 61);

  /* Sem Result Table Trill */
  Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.addr.width_bits = 60;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.tt_learn_enable), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.tpid_profile), 17, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.service_type), 43, 41);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_trill_tbl.learn_destination), 59, 44);

  /* Sem Result Table Ip Tt */
  Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.addr.width_bits = 44;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.in_rif), 11, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.in_rif_valid), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.cos_profile), 40, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_ip_tt_tbl.service_type), 43, 41);

  /* Sem Result Table Label Pwe P2p */
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.model_is_pipe), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.destination_valid), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.tpid_profile), 17, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.has_cw), 18, 18);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.valid), 19, 19);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.isidor_out_lifor_vc_label), 43, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_p2p_tbl.type), 61, 60);

  /* Sem Result Table Label Pwe Mp */
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.vsi), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.orientation_is_hub), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.tt_learn_enable), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.tpid_profile), 17, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.has_cw), 18, 18);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.valid), 19, 19);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.learn_asd), 43, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.learn_destination), 59, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_pwe_mp_tbl.type), 61, 60);

  /* Sem Result Table Label Vrl */
  Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.in_rif), 11, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.in_rif_valid), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.model_is_pipe), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.valid), 19, 19);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.cos_profile), 40, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.service_type), 43, 41);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_vrl_tbl.type), 61, 60);

  /* Sem Result Table Label Lsp */
  Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.in_rif), 11, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.in_rif_valid), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.model_is_pipe), 14, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.valid), 19, 19);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.cos_profile), 40, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.service_type), 43, 41);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_table_label_lsp_tbl.type), 61, 60);

  /* Sem Opcode Ip Offsets */
  Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.addr.base = 0x00360000;
  Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.addr.width_bits = 21;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv6_sem_offset), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv6_add_offset_to_base), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv6_opcode_valid), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv4_sem_offset), 14, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv4_add_offset_to_base), 16, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_ip_offsets_tbl.ipv4_opcode_valid), 20, 20);

  /* Sem Opcode Tc Dp Offsets */
  Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.addr.base = 0x00370000;
  Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.addr.size = 0x0080;
  Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.addr.width_bits = 9;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.tc_dp_sem_offset), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.tc_dp_add_offset_to_base), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_tc_dp_offsets_tbl.tc_dp_opcode_valid), 8, 8);

  /* Sem Opcode Pcp Dei Offsets */
  Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.addr.base = 0x00380000;
  Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.addr.width_bits = 9;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.pcp_dei_sem_offset), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.pcp_dei_add_offset_to_base), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_opcode_pcp_dei_offsets_tbl.pcp_dei_opcode_valid), 8, 8);

  /* Stp Table */
  Soc_pb_pp_tbls.ihp.stp_table_tbl.addr.base = 0x00390000;
  Soc_pb_pp_tbls.ihp.stp_table_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihp.stp_table_tbl.addr.width_bits = 128;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.stp_table_tbl.per_port_stp_state), 127, 0);

  /* Vrid My Mac Map */
  Soc_pb_pp_tbls.ihp.vrid_my_mac_map_tbl.addr.base = 0x003a0000;
  Soc_pb_pp_tbls.ihp.vrid_my_mac_map_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihp.vrid_my_mac_map_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vrid_my_mac_map_tbl.vrid_my_mac_map), 31, 0);

  /* Mpls Label Range Encountered */
  Soc_pb_pp_tbls.ihp.mpls_label_range_encountered_tbl.addr.base = 0x003b0000;
  Soc_pb_pp_tbls.ihp.mpls_label_range_encountered_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.mpls_label_range_encountered_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.mpls_label_range_encountered_tbl.mpls_label_range_encountered), 15, 0);

  /* Mpls Tunnel Termination Valid */
  Soc_pb_pp_tbls.ihp.mpls_tunnel_termination_valid_tbl.addr.base = 0x003c0000;
  Soc_pb_pp_tbls.ihp.mpls_tunnel_termination_valid_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.mpls_tunnel_termination_valid_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.mpls_tunnel_termination_valid_tbl.mpls_tunnel_termination_valid), 15, 0);

  /* Ip Over Mpls Exp Mapping */
  Soc_pb_pp_tbls.ihp.ip_over_mpls_exp_mapping_tbl.addr.base = 0x003d0000;
  Soc_pb_pp_tbls.ihp.ip_over_mpls_exp_mapping_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihp.ip_over_mpls_exp_mapping_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.ip_over_mpls_exp_mapping_tbl.ip_over_mpls_exp_mapping), 7, 0);

  /* Vtt Llvp */
  Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.addr.base = 0x003e0000;
  Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.addr.width_bits = 13;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.incoming_tag_structure), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.ivec), 9, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt_llvp_tbl.pcp_dei_profile), 12, 12);
  

  /* Llvp Prog Sel */
  Soc_pb_pp_tbls.ihp.llvp_prog_sel_tbl.addr.base = 0x003f0000;
  Soc_pb_pp_tbls.ihp.llvp_prog_sel_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.llvp_prog_sel_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.llvp_prog_sel_tbl.llvp_prog_sel), 3, 0);

  /* Vtt1st Key Construction0 */
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.addr.base = 0x00400000;
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.addr.width_bits = 36;
  /*soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.inst_valid), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.inst_source_select), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.inst_header_offset_select), 4, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.inst_niblle_field_offset), 12, 5);*/
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.key_inst0), 17, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction0_tbl.key_inst1), 35, 18);

  /* Vtt1st Key Construction1 */
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.addr.base = 0x00410000;
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.addr.width_bits = 54;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.key_inst2), 17, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.key_inst3), 37, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.and_mask), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.or_mask), 47, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt1st_key_construction1_tbl.sem_lookup_enable), 48, 48);

  /* Vtt2nd Key Construction */
  Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.addr.base = 0x00420000;
  Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.addr.width_bits = 48;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.key_inst0), 17, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.key_inst1), 37, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.and_mask), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vtt2nd_key_construction_tbl.or_mask), 47, 44);

  /* Ingress Vlan Edit Command Table */
  Soc_pb_pp_tbls.ihp.ingress_vlan_edit_command_table_tbl.addr.base = 0x00430000;
  Soc_pb_pp_tbls.ihp.ingress_vlan_edit_command_table_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihp.ingress_vlan_edit_command_table_tbl.addr.width_bits = 6;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.ingress_vlan_edit_command_table_tbl.ivec), 5, 0);

  /* Vlan Edit Pcp Dei Map */
  Soc_pb_pp_tbls.ihp.vlan_edit_pcp_dei_map_tbl.addr.base = 0x00440000;
  Soc_pb_pp_tbls.ihp.vlan_edit_pcp_dei_map_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.vlan_edit_pcp_dei_map_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.vlan_edit_pcp_dei_map_tbl.pcp_dei), 3, 0);

  /* Pbb Cfm Table */
  Soc_pb_pp_tbls.ihp.pbb_cfm_table_tbl.addr.base = 0x00450000;
  Soc_pb_pp_tbls.ihp.pbb_cfm_table_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.pbb_cfm_table_tbl.addr.width_bits = 7;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pbb_cfm_table_tbl.pbb_cfm_trap_valid), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.pbb_cfm_table_tbl.pbb_cfm_max_level), 6, 4);

  /* Sem Result Accessed */
  Soc_pb_pp_tbls.ihp.sem_result_accessed_tbl.addr.base = 0x00460000;
  Soc_pb_pp_tbls.ihp.sem_result_accessed_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihp.sem_result_accessed_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.sem_result_accessed_tbl.sem_result_accessed), 15, 0);

  /* In Rif Config Table */
  Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.addr.base = 0x00470000;
  Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.vrf), 7, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.uc_rpf_enable), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.enable_routing_mc), 9, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.enable_routing_uc), 10, 10);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.enable_routing_mpls), 11, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.in_rif_config_table_tbl.in_rif_cos_profile), 15, 12);

  /* Tc Dp Map Table */
  Soc_pb_pp_tbls.ihp.tc_dp_map_table_tbl.addr.base = 0x00480000;
  Soc_pb_pp_tbls.ihp.tc_dp_map_table_tbl.addr.size = 0x3c00;
  Soc_pb_pp_tbls.ihp.tc_dp_map_table_tbl.addr.width_bits = 7;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tc_dp_map_table_tbl.drop_precedence), 5, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihp.tc_dp_map_table_tbl.traffic_class), 2, 0);
}

/* Block tables initialization: IHB  */
STATIC void
  soc_pb_pp_tbls_init_IHB(void)
{
  uint32
    tbl_ndx;

  /* Pinfo Fer */
  Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.addr.base = 0x00b00000;
  Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.addr.width_bits = 21;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.ecmp_lb_key_count), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.lag_lb_key_start), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.lag_lb_key_count), 9, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.lb_profile), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.lb_bos_search), 16, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_fer_tbl.lb_include_bos_hdr), 20, 20);

  /* Ecmp Group Size */
  Soc_pb_pp_tbls.ihb.ecmp_group_size_tbl.addr.base = 0x00b10000;
  Soc_pb_pp_tbls.ihb.ecmp_group_size_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihb.ecmp_group_size_tbl.addr.width_bits = 9;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.ecmp_group_size_tbl.ecmp_group_size), 8, 0);

  /* Lb Pfc Profile */
  Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.addr.base = 0x00b20000;
  Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.addr.size = 0x0080;
  Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.addr.width_bits = 20;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.lb_vector_index1), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.lb_vector_index2), 7, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.lb_vector_index3), 11, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.lb_vector_index4), 15, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_pfc_profile_tbl.lb_vector_index5), 19, 16);

  /* Lb Vector Program Map */
  Soc_pb_pp_tbls.ihb.lb_vector_program_map_tbl.addr.base = 0x00b30000;
  Soc_pb_pp_tbls.ihb.lb_vector_program_map_tbl.addr.size = 0x000f;
  Soc_pb_pp_tbls.ihb.lb_vector_program_map_tbl.addr.width_bits = 49;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_vector_program_map_tbl.chunk_bitmap), 47, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lb_vector_program_map_tbl.chunk_size), 48, 48);

  /* Fec Super Entry */
  Soc_pb_pp_tbls.ihb.fec_super_entry_tbl.addr.base = 0x00b40000;
  Soc_pb_pp_tbls.ihb.fec_super_entry_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_super_entry_tbl.addr.width_bits = 15;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_super_entry_tbl.ecmp_group_size_index), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_super_entry_tbl.protection_pointer), 14, 4);

  /* Fec Entry[0..1] */
  Soc_pb_pp_tbls.ihb.fec_entry_tbl.addr.base = 0x00b50000;
  Soc_pb_pp_tbls.ihb.fec_entry_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_entry_tbl.addr.width_bits = 45;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_tbl.fec_entry), 44, 0);

  /* Fec Entry General */
  Soc_pb_pp_tbls.ihb.fec_entry_general_tbl.addr.base = 0x00b50000;
  Soc_pb_pp_tbls.ihb.fec_entry_general_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_entry_general_tbl.addr.width_bits = 45;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_general_tbl.destination), 15, 0);

  /* Fec Entry Eth Or Trill */
  Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.addr.base = 0x00b50000;
  Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.addr.width_bits = 45;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.out_lif_or_dist_tree_nick), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_eth_or_trill_tbl.is_out_lif), 32, 32);

  /* Fec Entry Ip */
  Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.addr.base = 0x00b50000;
  Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.addr.width_bits = 45;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.out_lif_lsb), 28, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.uc_rpf_mode), 29, 29);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.mc_rpf_mode), 31, 30);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.rif), 43, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_ip_tbl.out_lif_type), 44, 44);

  /* Fec Entry Default */
  Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.addr.base = 0x00b50000;
  Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.addr.width_bits = 45;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.out_lif), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_default_tbl.out_vsi_lsb), 43, 32);

  /* Fec Entry Accessed[0..1] */
  Soc_pb_pp_tbls.ihb.fec_entry_accessed_tbl.addr.base = 0x00b70000;
  Soc_pb_pp_tbls.ihb.fec_entry_accessed_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihb.fec_entry_accessed_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fec_entry_accessed_tbl.fec_entry_accessed), 7, 0);

  /* Path Select */
  Soc_pb_pp_tbls.ihb.path_select_tbl.addr.base = 0x00b90000;
  Soc_pb_pp_tbls.ihb.path_select_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.path_select_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select0), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select1), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select2), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select3), 3, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select4), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select5), 5, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select6), 6, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.path_select_tbl.path_select7), 7, 7);

  /* Destination Status */
  Soc_pb_pp_tbls.ihb.destination_status_tbl.addr.base = 0x00ba0000;
  Soc_pb_pp_tbls.ihb.destination_status_tbl.addr.size = 0x0200;
  Soc_pb_pp_tbls.ihb.destination_status_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.destination_status_tbl.destination_valid), 7, 0);

  /* Fwd Act Profile */
  Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.addr.base = 0x00bb0000;
  Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.addr.width_bits = 125;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_trap), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_control), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_destination), 24, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_destination_valid), 28, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_destination_add_vsi), 32, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_destination_add_vsi_shift), 37, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_traffic_class), 42, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_traffic_class_valid), 44, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_drop_precedence), 49, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_drop_precedence_valid), 52, 52);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_meter_pointer), 68, 56);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_meter_pointer_selector), 72, 72);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_counter_pointer), 87, 76);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_counter_pointer_selector), 88, 88);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_drop_precedence_meter_command), 93, 92);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_drop_precedence_meter_command_valid), 96, 96);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_fwd_offset_index), 102, 100);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_fwd_offset_index_valid), 104, 104);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_ethernet_policer_pointer), 116, 108);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_ethernet_policer_pointer_valid), 120, 120);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.fwd_act_profile_tbl.fwd_act_learn_disable), 124, 124);

  /* Snoop Action */
  Soc_pb_pp_tbls.ihb.snoop_action_tbl.addr.base = 0x00bc0000;
  Soc_pb_pp_tbls.ihb.snoop_action_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.snoop_action_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.snoop_action_tbl.snoop_action), 3, 0);

  /* Pinfo Flp */
  Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.addr.base = 0x00100000;
  Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.addr.width_bits = 30;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.program_translation_profile), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.sa_lookup_enable), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.action_profile_sa_drop_index), 9, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.action_profile_sa_not_found_index), 13, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.action_profile_da_not_found_index), 17, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.transparent_p2p_service_enable), 20, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.enable_unicast_same_interface_filter), 24, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_flp_tbl.learn_enable), 28, 28);
  

  /* Program Translation Map */
  Soc_pb_pp_tbls.ihb.program_translation_map_tbl.addr.base = 0x00110000;
  Soc_pb_pp_tbls.ihb.program_translation_map_tbl.addr.size = 0x000f;
  Soc_pb_pp_tbls.ihb.program_translation_map_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_translation_map_tbl.program00), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_translation_map_tbl.program01), 7, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_translation_map_tbl.program10), 11, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_translation_map_tbl.program11), 15, 12);

  /* Flp Key Program Map */
  Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.addr.base = 0x00120000;
  Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.addr.width_bits = 157;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_a_instruction0), 17, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_a_instruction1), 35, 18);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_b_instruction0), 53, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_b_instruction1), 71, 54);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_b_instruction2), 89, 72);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.key_b_instruction3), 107, 90);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.elk_lkp_valid), 108, 108);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_1st_lkp_valid), 109, 109);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_1st_lkp_key_select), 110, 110);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_1st_lkp_key_type), 111, 111);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_1st_lkp_and_value), 115, 112);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_1st_lkp_or_value), 119, 116);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_2nd_lkp_valid), 120, 120);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_2nd_lkp_key_select), 121, 121);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_2nd_lkp_and_value), 125, 122);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lem_2nd_lkp_or_value), 129, 126);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_1st_lkp_valid), 130, 130);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_1st_lkp_and_value), 132, 131);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_1st_lkp_or_value), 134, 133);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_2nd_lkp_valid), 135, 135);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_2nd_lkp_and_value), 137, 136);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.lpm_2nd_lkp_or_value), 139, 138);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.tcam_key_size), 140, 140);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.tcam_bank_valid), 144, 141);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.tcam_and_value), 148, 145);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.tcam_or_value), 152, 149);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.flp_key_program_map_tbl.data_processing_profile), 156, 153);

  /* Unknown Da Action Profiles */
  Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.addr.base = 0x00130000;
  Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.addr.size = 0x0018;
  Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.fwd), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.snp), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.unknown_da_action_profiles_tbl.cpu_trap_code_lsb), 7, 5);

  /* Vrf Config */
  Soc_pb_pp_tbls.ihb.vrf_config_tbl.addr.base = 0x00140000;
  Soc_pb_pp_tbls.ihb.vrf_config_tbl.addr.size = 0x0200;
  Soc_pb_pp_tbls.ihb.vrf_config_tbl.addr.width_bits = 17;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.vrf_config_tbl.destination), 15, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.vrf_config_tbl.l3vpn_default_routing), 16, 16);

  /* Header Profile */
  Soc_pb_pp_tbls.ihb.header_profile_tbl.addr.base = 0x00d10000;
  Soc_pb_pp_tbls.ihb.header_profile_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihb.header_profile_tbl.addr.width_bits = 21;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_st_vsq_ptr_tc_mode), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_learn_disable), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_build_pph), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_build_ftmh), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_ftmh_pph_present), 16, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.header_profile_tbl.header_profile_always_add_pph_learn_ext), 20, 20);
  

  /* Snp Act Profile */
  Soc_pb_pp_tbls.ihb.snp_act_profile_tbl.addr.base = 0x00d20000;
  Soc_pb_pp_tbls.ihb.snp_act_profile_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihb.snp_act_profile_tbl.addr.width_bits = 10;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.snp_act_profile_tbl.snp_act_snp_sampling_probability), 9, 0);

  /* Mrr Act Profile */
  Soc_pb_pp_tbls.ihb.mrr_act_profile_tbl.addr.base = 0x00d30000;
  Soc_pb_pp_tbls.ihb.mrr_act_profile_tbl.addr.size = 0x0010;
  Soc_pb_pp_tbls.ihb.mrr_act_profile_tbl.addr.width_bits = 10;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.mrr_act_profile_tbl.mrr_act_mrr_sampling_probability), 9, 0);

  /* Lpm1 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[0].addr.base = 0x00e00000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[0].addr.size = 0x1000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[0].addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[0].lpm), 22, 0);

  /* Lpm2 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[1].addr.base = 0x00e10000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[1].addr.size = 0x2000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[1].addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[1].lpm), 22, 0);

  /* Lpm3 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[2].addr.base = 0x00e20000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[2].addr.size = 0x3000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[2].addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[2].lpm), 22, 0);

  /* Lpm4 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[3].addr.base = 0x00e30000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[3].addr.size = 0x3000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[3].addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[3].lpm), 22, 0);

  /* Lpm5 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[4].addr.base = 0x00e40000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[4].addr.size = 0x3000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[4].addr.width_bits = 23;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[4].lpm), 22, 0);

  /* Lpm6 */
  Soc_pb_pp_tbls.ihb.lpm_tbl[5].addr.base = 0x00e50000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[5].addr.size = 0x3000;
  Soc_pb_pp_tbls.ihb.lpm_tbl[5].addr.width_bits = 14;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.lpm_tbl[5].lpm), 13, 0);

  /* Pinfo PMF Key Gen Var */
  Soc_pb_pp_tbls.ihb.pinfo_pmf_key_gen_var_tbl.addr.base = 0x00320000;
  Soc_pb_pp_tbls.ihb.pinfo_pmf_key_gen_var_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.ihb.pinfo_pmf_key_gen_var_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.pinfo_pmf_key_gen_var_tbl.pinfo_pmf_key_gen_var), 31, 0);

  /* Program Selection Map0 */
  Soc_pb_pp_tbls.ihb.program_selection_map0_tbl.addr.base = 0x00330000;
  Soc_pb_pp_tbls.ihb.program_selection_map0_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map0_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map0_tbl.program_selection_map0), 30, 0);

  /* Program Selection Map1 */
  Soc_pb_pp_tbls.ihb.program_selection_map1_tbl.addr.base = 0x00340000;
  Soc_pb_pp_tbls.ihb.program_selection_map1_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map1_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map1_tbl.program_selection_map1), 30, 0);

  /* Program Selection Map2 */
  Soc_pb_pp_tbls.ihb.program_selection_map2_tbl.addr.base = 0x00350000;
  Soc_pb_pp_tbls.ihb.program_selection_map2_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map2_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map2_tbl.program_selection_map2), 30, 0);

  /* Program Selection Map3 */
  Soc_pb_pp_tbls.ihb.program_selection_map3_tbl.addr.base = 0x00360000;
  Soc_pb_pp_tbls.ihb.program_selection_map3_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map3_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map3_tbl.program_selection_map3), 30, 0);

  /* Program Selection Map4 */
  Soc_pb_pp_tbls.ihb.program_selection_map4_tbl.addr.base = 0x00370000;
  Soc_pb_pp_tbls.ihb.program_selection_map4_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map4_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map4_tbl.program_selection_map4), 30, 0);

  /* Program Selection Map5 */
  Soc_pb_pp_tbls.ihb.program_selection_map5_tbl.addr.base = 0x00380000;
  Soc_pb_pp_tbls.ihb.program_selection_map5_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.ihb.program_selection_map5_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map5_tbl.program_selection_map5), 30, 0);

  /* Program Selection Map6 */
  Soc_pb_pp_tbls.ihb.program_selection_map6_tbl.addr.base = 0x00390000;
  Soc_pb_pp_tbls.ihb.program_selection_map6_tbl.addr.size = 0x0080;
  Soc_pb_pp_tbls.ihb.program_selection_map6_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map6_tbl.program_selection_map6), 30, 0);

  /* Program Selection Map7 */
  Soc_pb_pp_tbls.ihb.program_selection_map7_tbl.addr.base = 0x003a0000;
  Soc_pb_pp_tbls.ihb.program_selection_map7_tbl.addr.size = 0x0080;
  Soc_pb_pp_tbls.ihb.program_selection_map7_tbl.addr.width_bits = 31;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_selection_map7_tbl.program_selection_map7), 30, 0);

  /* Program Vars Table */
  Soc_pb_pp_tbls.ihb.program_vars_table_tbl.addr.base = 0x003b0000;
  Soc_pb_pp_tbls.ihb.program_vars_table_tbl.addr.size = 0x0020;
  Soc_pb_pp_tbls.ihb.program_vars_table_tbl.addr.width_bits = 36;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.first_pass_key_profile_index), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.second_pass_key_profile_index), 6, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.tag_selection_profile_index), 10, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.bytes_to_remove_header), 13, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.bytes_to_remove_offset), 20, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.system_header_profile), 26, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.program_vars_table_tbl.copy_program_variable), 35, 28);

  for (tbl_ndx = 0; tbl_ndx < SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS; ++tbl_ndx)
  {
    /* Key A Program Instruction Table[0..3] */
    Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].addr.base = 0x003c0000 + (tbl_ndx * SOC_PB_PP_KEY_PGM_INSTR_SKIP);
    Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].addr.size = 0x0020;
    Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].addr.width_bits = 53;
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst0_valid), 0, 0);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst0_source_select), 4, 4);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst0_header_offset_select), 10, 8);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst0_niblle_field_offset), 19, 12);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst0_bit_count), 24, 20);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst1_valid), 28, 28);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst1_source_select), 32, 32);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst1_header_offset_select), 38, 36);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst1_niblle_field_offset), 47, 40);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_a_program_instruction_table_tbl[tbl_ndx].inst1_bit_count), 52, 48);

    /* Key B Program Instruction Table[0..3] */
    Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].addr.base = 0x00400000 + (tbl_ndx * SOC_PB_PP_KEY_PGM_INSTR_SKIP);
    Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].addr.size = 0x0020;
    Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].addr.width_bits = 53;
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst0_valid), 0, 0);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst0_source_select), 4, 4);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst0_header_offset_select), 10, 8);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst0_niblle_field_offset), 19, 12);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst0_bit_count), 24, 20);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst1_valid), 28, 28);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst1_source_select), 32, 32);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst1_header_offset_select), 38, 36);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst1_niblle_field_offset), 47, 40);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.key_b_program_instruction_table_tbl[tbl_ndx].inst1_bit_count), 52, 48);
  }

  /* TCAM 1st Pass Key Profile Resolved Data */
  Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00420000;
  Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_a_key_select), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_a_db_id_and_value), 7, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_a_db_id_or_value), 11, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_b_key_select), 15, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_b_db_id_and_value), 19, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_b_db_id_or_value), 23, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_c_key_select), 27, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_c_db_id_and_value), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_c_db_id_or_value), 35, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_d_key_select), 39, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_d_db_id_and_value), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.bank_d_db_id_or_value), 47, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.tcam_pd1_members), 51, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.tcam_pd2_members), 55, 52);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.tcam_sel3_member), 57, 56);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_1st_pass_key_profile_resolved_data_tbl.tcam_sel4_member), 61, 60);

  /* TCAM 2nd Pass Key Profile Resolved Data */
  Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00430000;
  Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 62;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_a_key_select), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_a_db_id_and_value), 7, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_a_db_id_or_value), 11, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_b_key_select), 15, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_b_db_id_and_value), 19, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_b_db_id_or_value), 23, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_c_key_select), 27, 24);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_c_db_id_and_value), 31, 28);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_c_db_id_or_value), 35, 32);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_d_key_select), 39, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_d_db_id_and_value), 43, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.bank_d_db_id_or_value), 47, 44);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.tcam_pd1_members), 51, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.tcam_pd2_members), 55, 52);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.tcam_sel3_member), 57, 56);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.tcam_sel4_member), 61, 60);

  /* Direct Action Table */
  Soc_pb_pp_tbls.ihb.direct_action_table_tbl.addr.base = 0x00460000;
  Soc_pb_pp_tbls.ihb.direct_action_table_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.ihb.direct_action_table_tbl.addr.width_bits = 20;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.direct_action_table_tbl.direct_action_table), 19, 0);

  /* Tcam Bank */
  Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.addr.base = 0x00f00000;
  Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.addr.width_bits = 302;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.vbe), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.mskdatae), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.wr), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.rd), 3, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.cmp), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.flush), 5, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.di), 149, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.vbi), 150, 150);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.cfg), 152, 151);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.hqsel), 156, 153);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.mdi), 300, 157);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_in_tbl.reserved), 301, 301);

   /* Tcam Bank */
  Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.addr.base = 0x00f00000;
  Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.addr.width_bits = 302;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.vbo), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.dataout), 144, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.hadr), 155, 145);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.hit), 156, 156);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.mhit), 157, 157);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_bank_out_tbl.mdo), 301, 158);
  
  /* Tcam Action[0..3] */
  for (tbl_ndx = 0; tbl_ndx < SOC_PB_TCAM_NOF_BANKS; ++tbl_ndx)
  {
    Soc_pb_pp_tbls.ihb.tcam_action_tbl[tbl_ndx].addr.base = 0x00f10000 + (tbl_ndx * 0x0200);
    Soc_pb_pp_tbls.ihb.tcam_action_tbl[tbl_ndx].addr.size = 0x0200;
    Soc_pb_pp_tbls.ihb.tcam_action_tbl[tbl_ndx].addr.width_bits = 64;
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_action_tbl[tbl_ndx].low), 31, 0);
    soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.ihb.tcam_action_tbl[tbl_ndx].high), 63, 32);
  }
}

/* Block tables initialization: EGQ */
STATIC void
  soc_pb_pp_tbls_init_EGQ(void)
{
  /* Per Port Configuration Table(PPCT) */
  Soc_pb_pp_tbls.egq.ppct_tbl.addr.base = 0x002b0000;
  Soc_pb_pp_tbls.egq.ppct_tbl.addr.size = 0x0050;
  Soc_pb_pp_tbls.egq.ppct_tbl.addr.width_bits = 74;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.out_pp_port), 5, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.mtu), 19, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.dst_system_port_id), 32, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.lb_key_max), 40, 33);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.lb_key_min), 48, 41);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.is_stacking_port), 49, 49);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.peer_tm_domain_id), 53, 50);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.port_type), 55, 54);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.cnm_intrcpt_fc_vec_index_5_0), 61, 56);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.cnm_intrcpt_fc_vec_index_12_6), 68, 62);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.cnm_intrcpt_fc_vec_index_13_13), 69, 69);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.cnm_intrcpt_en), 70, 70);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ppct_tbl.ad_count_out_port_flag), 73, 73);

  /* packet processing Per Port Configuration Table(PP PPCT) */
  Soc_pb_pp_tbls.egq.pp_ppct_tbl.addr.base = 0x002c0000;
  Soc_pb_pp_tbls.egq.pp_ppct_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.egq.pp_ppct_tbl.addr.width_bits = 22;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.pvlan_port_type), 1, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.orientation_is_hub), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.eei_type), 3, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.unknown_bc_da_action_filter), 4, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.unknown_mc_da_action_filter), 5, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.unknown_uc_da_action_filter), 6, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.enable_src_equal_dst_filter), 7, 7);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.acceptable_frame_type_profile), 9, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.egress_vsi_filtering_enable), 10, 10);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.disable_filtering), 11, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.acl_profile), 13, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.acl_data), 19, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.disable_learning), 20, 20);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.pp_ppct_tbl.port_is_pbp), 21, 21);

  /* Ingress Vlan Edit Command Map */
  Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.addr.base = 0x004b0000;
  Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.addr.width_bits = 17;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.tpid_profile_link), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.edit_command_outer_vid_source), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.edit_command_outer_pcp_dei_source), 6, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.edit_command_inner_vid_source), 8, 7);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.edit_command_inner_pcp_dei_source), 10, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.edit_command_bytes_to_remove), 12, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.outer_tpid_ndx), 14, 13);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ingress_vlan_edit_command_map_tbl.inner_tpid_ndx), 16, 15);

  /* Vsi Membership Memory(Vsi Membership) */
  Soc_pb_pp_tbls.egq.vsi_membership_tbl.addr.base = 0x00600000;
  Soc_pb_pp_tbls.egq.vsi_membership_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.egq.vsi_membership_tbl.addr.width_bits = 64;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.vsi_membership_tbl.vsi_membership), 63, 0);

  /* Ttl Scope Memory(Ttl Scope) */
  Soc_pb_pp_tbls.egq.ttl_scope_tbl.addr.base = 0x00610000;
  Soc_pb_pp_tbls.egq.ttl_scope_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.egq.ttl_scope_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.ttl_scope_tbl.ttl_scope), 3, 0);

  /* Auxiliary Data Memory(Aux Table) */
  Soc_pb_pp_tbls.egq.aux_table_tbl.addr.base = 0x00620000;
  Soc_pb_pp_tbls.egq.aux_table_tbl.addr.size = 0x0800;
  Soc_pb_pp_tbls.egq.aux_table_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.aux_table_tbl.auxtable), 7, 0);

  /* Eep Orientation Memory(Eep Orientation) */
  Soc_pb_pp_tbls.egq.eep_orientation_tbl.addr.base = 0x00630000;
  Soc_pb_pp_tbls.egq.eep_orientation_tbl.addr.size = 0x0600;
  Soc_pb_pp_tbls.egq.eep_orientation_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.eep_orientation_tbl.eep_orientation), 7, 0);

  /* Cfm Trap */
  Soc_pb_pp_tbls.egq.cfm_trap_tbl.addr.base = 0x00640000;
  Soc_pb_pp_tbls.egq.cfm_trap_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.egq.cfm_trap_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.cfm_trap_tbl.cfm_max_level), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.cfm_trap_tbl.cfm_trap_valid), 3, 3);

  /* TBL DEF CHANGE: Replaced single 33b field with actual fields */

  /* Action Profile Table */
  Soc_pb_pp_tbls.egq.action_profile_table_tbl.addr.base = 0x00670000;
  Soc_pb_pp_tbls.egq.action_profile_table_tbl.addr.size = 0x0008;
  Soc_pb_pp_tbls.egq.action_profile_table_tbl.addr.width_bits = 33;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.otm_valid), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.otm), 7, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.discard), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.tc), 11, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.tc_valid), 12, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.dp), 14, 13);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.dp_valid), 15, 15);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.cud), 31, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.egq.action_profile_table_tbl.cud_valid), 32, 32);
}

/* Block tables initialization: EPNI 	 */
STATIC void
  soc_pb_pp_tbls_init_EPNI(void)
{
  /* TBL DEF CHANGE: use bank1/2 as a single logical table */

  /* Egress Encapsulation Entry - MPLS Tunnel Format */
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.addr.base = SOC_PB_PP_CHIP_TBLS_LOGICAL_TBL_BASE_ADDR;
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.addr.size = 0x3000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.addr.width_bits = 72;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.next_eep), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.next_vsi_lsb), 25, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.mpls2_label), 45, 26);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.mpls2_command), 47, 46);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.has_cw), 26, 26);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.tpid_profile), 28, 27);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.mpls1_label), 67, 48);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_mpls_tunnel_format_tbl.mpls1_command), 71, 68);

  /* Egress Encapsulation Entry - IP Tunnel Format */
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.addr.base = SOC_PB_PP_CHIP_TBLS_LOGICAL_TBL_BASE_ADDR;
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.addr.size = 0x3000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.addr.width_bits = 72;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.next_eep), 13, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.next_vsi_lsb), 25, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.gre_enable), 26, 26   );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.ipv4_tos_index), 30, 27);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.ipv4_ttl_index), 32, 31);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.ipv4_src_index), 36, 33);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_encapsulation_entry_ip_tunnel_format_tbl.ipv4_dst), 68, 37);

  /* Encapsulation-Memory Link Layer Entry Format */
  Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.addr.base = 0x00030000;
  Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.addr.width_bits = 72;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.vid), 11, 0 );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.vid_valid), 12, 12   );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.encapsulation_memory_link_layer_entry_format_tbl.dest_mac), 60, 13);

  /* egress encapsulation bank1 */
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank1_tbl.addr.base = 0x00030000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank1_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank1_tbl.addr.width_bits = 72;

  /* egress encapsulation bank2 */
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank2_tbl.addr.base = 0x00040000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank2_tbl.addr.size = 0x2000;
  Soc_pb_pp_tbls.epni.egress_encapsulation_bank2_tbl.addr.width_bits = 72;

  /* Tx Tag Table */
  Soc_pb_pp_tbls.epni.tx_tag_table_tbl.addr.base = 0x00050000;
  Soc_pb_pp_tbls.epni.tx_tag_table_tbl.addr.size = 0x1000;
  Soc_pb_pp_tbls.epni.tx_tag_table_tbl.addr.width_bits = 64;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.tx_tag_table_tbl.entry), 63, 0);

  /* Spanning Tree Protocol State Memory (STP) */
  Soc_pb_pp_tbls.epni.stp_tbl.addr.base = 0x00060000;
  Soc_pb_pp_tbls.epni.stp_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.epni.stp_tbl.addr.width_bits = 64;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.stp_tbl.egress_stp_state), 63, 0);

  /* Small Em Result Memory */
  Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.addr.base = 0x00070000;
  Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.addr.size = 0x4000;
  Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.pcp_dei_profile), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.vid[1]), 15, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.vid[0]), 27, 16);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.small_em_result_memory_tbl.vlan_edit_profile), 31, 28);

  /* Pcp Dei Table */
  Soc_pb_pp_tbls.epni.pcp_dei_table_tbl.addr.base = 0x00080000;
  Soc_pb_pp_tbls.epni.pcp_dei_table_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.epni.pcp_dei_table_tbl.addr.width_bits = 4;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pcp_dei_table_tbl.pcp_enc_table), 3, 0);

  /* PP PCT */
  Soc_pb_pp_tbls.epni.pp_pct_tbl.addr.base = 0x00090000;
  Soc_pb_pp_tbls.epni.pp_pct_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.epni.pp_pct_tbl.addr.width_bits = 50;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.acceptable_frame_type_profile), 1, 0  );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.port_is_pbp), 2, 2    );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.egress_stp_filter_enable), 3, 3    );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.egress_vsi_filter_enable), 4, 4    );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.disable_filter), 5, 5    );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.default_sem_result_0), 6, 6    );
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.default_sem_result_14_1), 20, 7);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.exp_map_profile), 22, 21);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.c_tpid_index), 24, 23);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.s_tpid_index), 26, 25);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.tag_profile), 28, 27);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.vlan_domain), 34, 29);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.eei_type), 35, 35);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.cep_c_vlan_edit), 36, 36);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.llvp_profile), 39, 37);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.mpls_ethertype_select), 40, 40);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.count_mode), 42, 41);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.count_enable), 43, 43);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.pp_pct_tbl.counter_compensation), 49, 44);

  /* Llvp Table */
  Soc_pb_pp_tbls.epni.llvp_table_tbl.addr.base = 0x000a0000;
  Soc_pb_pp_tbls.epni.llvp_table_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.epni.llvp_table_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.llvp_table_tbl.llvp_c_tag_offset), 0, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.llvp_table_tbl.llvp_packet_has_c_tag), 1, 1);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.llvp_table_tbl.llvp_packet_has_up), 2, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.llvp_table_tbl.llvp_packet_has_pcp_dei), 3, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.llvp_table_tbl.llvp_incoming_tag_format), 7, 4);

  /* Egress Edit Cmd */
  Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.addr.base = 0x000b0000;
  Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.addr.size = 0x0100;
  Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.addr.width_bits = 16;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.outer_tpid_index), 15, 14);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.inner_tpid_index), 13, 12);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.outer_vid_source), 11, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.inner_vid_source), 8, 6);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.outer_pcp_dei_source), 5, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.inner_pcp_dei_source), 3, 2);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.egress_edit_cmd_tbl.tags_to_remove), 1, 0);

  /* Program Vars */
  Soc_pb_pp_tbls.epni.program_vars_tbl.addr.base = 0x000c0000;
  Soc_pb_pp_tbls.epni.program_vars_tbl.addr.size = 0x0c00;
  Soc_pb_pp_tbls.epni.program_vars_tbl.addr.width_bits = 15;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.program_vars_tbl.program_index), 3, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.program_vars_tbl.new_header_size), 7, 4);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.program_vars_tbl.add_network_header), 8, 8);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.program_vars_tbl.remove_network_header), 9, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.program_vars_tbl.system_header_size), 14, 10);

  /* Ingress Vlan Edit Command Map */
  Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.addr.base = 0x00130000;
  Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.addr.size = 0x0040;
  Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.addr.width_bits = 17;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.tpid_profile_link), 2, 0);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.edit_command_outer_vid_source), 4, 3);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.edit_command_outer_pcp_dei_source), 6, 5);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.edit_command_inner_vid_source), 8, 7);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.edit_command_inner_pcp_dei_source), 10, 9);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.edit_command_bytes_to_remove), 12, 11);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.outer_tpid_ndx), 14, 13);
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ingress_vlan_edit_command_map_tbl.inner_tpid_ndx), 16, 15);

  /* Ip Exp Map */
  Soc_pb_pp_tbls.epni.ip_exp_map_tbl.addr.base = 0x00140000;
  Soc_pb_pp_tbls.epni.ip_exp_map_tbl.addr.size = 0x0400;
  Soc_pb_pp_tbls.epni.ip_exp_map_tbl.addr.width_bits = 8;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.ip_exp_map_tbl.ip_exp_map), 7, 0);

  /* Etpp Debug */
  Soc_pb_pp_tbls.epni.etpp_debug_tbl.addr.base = 0x00150000;
  Soc_pb_pp_tbls.epni.etpp_debug_tbl.addr.size = 0x0018;
  Soc_pb_pp_tbls.epni.etpp_debug_tbl.addr.width_bits = 32;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.etpp_debug_tbl.etpp_debug), 31, 0);

  /* Esem management request */
  Soc_pb_pp_tbls.epni.esem_mgmt_request_tbl.addr.base = 0x00280000;
  Soc_pb_pp_tbls.epni.esem_mgmt_request_tbl.addr.size = 0x001;
  Soc_pb_pp_tbls.epni.esem_mgmt_request_tbl.addr.width_bits = 51;
  soc_pb_pp_tbl_fld_set( &(Soc_pb_pp_tbls.epni.esem_mgmt_request_tbl.esem_request), 59, 0);

}
/*****************************************************
*NAME
*  soc_pb_pp_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Soc_pb_pp_pp tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_pp_tbls_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TBLS_INIT);

  if (Soc_pb_pp_tbls_initialized == TRUE)
  {
    goto exit;
  }

  soc_pb_pp_tbls_init_IHP();
  soc_pb_pp_tbls_init_IHB();
  soc_pb_pp_tbls_init_EGQ();
  soc_pb_pp_tbls_init_EPNI();

  Soc_pb_pp_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_tbls_init",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

