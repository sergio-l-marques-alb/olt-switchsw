/* $Id: petra_a_chip_tbls.c,v 1.10 Broadcom SDK $
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
* FILENAME:       soc_petra_a_chip_tbls.c
*
* MODULE PREFIX:  soc_petra_tblsO
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

#include <soc/dpp/Petra/petra_a_chip_tbls.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_framework.h>
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
static SOC_PA_TBLS  Soc_petra_a_tbls;
static uint8    Soc_petra_a_tbls_initialized = FALSE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* soc_petra_tbls_get
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT  SOC_PA_TBLS  **soc_petra_tbls - pointer to soc_petra
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_petra_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pa_tbls_get(
    SOC_SAND_OUT  SOC_PA_TBLS  **soc_pa_tbls
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TBLS_GET);

  if (Soc_petra_a_tbls_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBLS_NOT_INITIALIZED, 10, exit);
  }

  *soc_pa_tbls = &Soc_petra_a_tbls;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_tbls_get()", 0, 0);
}

/*****************************************************
*NAME
* soc_pa_tbl_fld_set
*TYPE:
*  PROC
*DATE:
*  27/07/2007
*FUNCTION:
*  Sets a soc_petra table field
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_PETRA_TBL_FIELD *field - pointer to soc_petra table field structure
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
  soc_pa_tbl_fld_set(
    SOC_PETRA_TBL_FIELD *field,
    uint16       msb,
    uint16       lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

SOC_PA_TBLS*
  soc_pa_tbls(void)
{
  SOC_PA_TBLS*
    tbls = NULL;

  soc_pa_tbls_get(
    &tbls
  );

  return tbls;
}

/* Block tables initialization: OLP */
STATIC void
  soc_pa_tbls_init_OLP(void)
{

  /* Pge Mem */
  Soc_petra_a_tbls.olp.pge_mem_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.olp.pge_mem_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.olp.pge_mem_tbl.addr.width_bits = 32;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.olp.pge_mem_tbl.pge_mem), 31, 0);
}

/* Block tables initialization: IRE */
STATIC void
  soc_pa_tbls_init_IRE(void)
{

  /* Nif Ctxt Map */
  Soc_petra_a_tbls.ire.nif_ctxt_map_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.ire.nif_ctxt_map_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ire.nif_ctxt_map_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.nif_ctxt_map_tbl.fap_port), 6, 0);

  /* Nif Port2ctxt Bit Map */
  Soc_petra_a_tbls.ire.nif_port2ctxt_bit_map_tbl.addr.base = 0x00020000;
  Soc_petra_a_tbls.ire.nif_port2ctxt_bit_map_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.ire.nif_port2ctxt_bit_map_tbl.addr.width_bits = 80;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.nif_port2ctxt_bit_map_tbl.contexts_bit_mapping), 79, 0);

  /* Rcy Ctxt Map */
  Soc_petra_a_tbls.ire.rcy_ctxt_map_tbl.addr.base = 0x00030000;
  Soc_petra_a_tbls.ire.rcy_ctxt_map_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ire.rcy_ctxt_map_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.rcy_ctxt_map_tbl.fap_port), 6, 0);

  /* cpu packet counters */
  Soc_petra_a_tbls.ire.cpu_packet_counter_tbl.addr.base = 0x00040000;
  Soc_petra_a_tbls.ire.cpu_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ire.cpu_packet_counter_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.cpu_packet_counter_tbl.counter), 63, 0);

  /* olp packet counters */
  Soc_petra_a_tbls.ire.olp_packet_counter_tbl.addr.base = 0x00050000;
  Soc_petra_a_tbls.ire.olp_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ire.olp_packet_counter_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.olp_packet_counter_tbl.counter), 63, 0);

  /* nifa packet counters */
  Soc_petra_a_tbls.ire.nifa_packet_counter_tbl.addr.base = 0x00060000;
  Soc_petra_a_tbls.ire.nifa_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ire.nifa_packet_counter_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.nifa_packet_counter_tbl.counter), 63, 0);

  /* nifb packet counters */
  Soc_petra_a_tbls.ire.nifb_packet_counter_tbl.addr.base = 0x00070000;
  Soc_petra_a_tbls.ire.nifb_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ire.nifb_packet_counter_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.nifb_packet_counter_tbl.counter), 63, 0);

  /* rcy packet counters */
  Soc_petra_a_tbls.ire.rcy_packet_counter_tbl.addr.base = 0x00080000;
  Soc_petra_a_tbls.ire.rcy_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ire.rcy_packet_counter_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ire.rcy_packet_counter_tbl.counter), 63, 0);
}

/* Block tables initialization: IDR */
STATIC void
  soc_pa_tbls_init_IDR(void)
{

  /* Complete PC */
  Soc_petra_a_tbls.idr.complete_pc_tbl.addr.base = 0x00100000;
  Soc_petra_a_tbls.idr.complete_pc_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.idr.complete_pc_tbl.addr.width_bits = 18;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.idr.complete_pc_tbl.pcb_pointer), 8, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.idr.complete_pc_tbl.count), 12, 9);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.idr.complete_pc_tbl.ecc), 17, 13);
}

/* Block tables initialization: IRR */
STATIC void
  soc_pa_tbls_init_IRR(void)
{
  uint8
    fld_idx;

  /* Is Ingress Replication DB */
  Soc_petra_a_tbls.irr.is_ingress_replication_db_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.irr.is_ingress_replication_db_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.irr.is_ingress_replication_db_tbl.addr.width_bits = 4;
  for (fld_idx = 0 ; fld_idx < SOC_PA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL ; fld_idx++)
  {
    soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.is_ingress_replication_db_tbl.is_ingress_replication[fld_idx]), fld_idx, fld_idx);
  }

  /* Ingress Replication Multicast DB */
  Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.addr.base = 0x00020000;
  Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.addr.width_bits = 84;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.base_queue1), 14, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.is_queue_number1), 15, 15);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.base_queue2), 30, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.is_queue_number2), 31, 31);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.out_lif1), 47, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.out_lif2), 63, 48);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.egress_data), 68, 64);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.ingress_replication_multicast_db_tbl.link_ptr), 83, 69);

  /* Egress Replication Multicast DB */
  Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.addr.base = 0x00020000;
  Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.addr.width_bits = 84;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_lif3), 15, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_port3), 22, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_lif2), 38, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_port2), 45, 39);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_lif1), 61, 46);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.out_port1), 68, 62);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.egress_replication_multicast_db_tbl.link_ptr), 83, 69);

  /* Mirror Table */
  Soc_petra_a_tbls.irr.mirror_table_tbl.addr.base = 0x00030000;
  Soc_petra_a_tbls.irr.mirror_table_tbl.addr.size = 0x0060;
  Soc_petra_a_tbls.irr.mirror_table_tbl.addr.width_bits = 24;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.mirror_destination), 14, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.is_multicast), 14, 14);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.is_queue_number), 15, 15);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.is_outbound_mirror), 16, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.mirror_traffic_class), 19, 17);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.traffic_class_over_write), 20, 20);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.mirror_drop_precedence), 22, 21);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.mirror_table_tbl.drop_precedence_over_write), 23, 23);

  /* Snoop Table */
  Soc_petra_a_tbls.irr.snoop_table_tbl.addr.base = 0x00060000;
  Soc_petra_a_tbls.irr.snoop_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.irr.snoop_table_tbl.addr.width_bits = 24;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.snoop_destination), 14, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.is_multicast), 14, 14);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.is_queue_number), 15, 15);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.snoop_traffic_class), 19, 17);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.traffic_class_over_write), 20, 20);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.snoop_drop_precedence), 22, 21);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.snoop_table_tbl.drop_precedence_over_write), 23, 23);

  /* Glag To Lag Range */
  Soc_petra_a_tbls.irr.glag_to_lag_range_tbl.addr.base = 0x000f0000;
  Soc_petra_a_tbls.irr.glag_to_lag_range_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.irr.glag_to_lag_range_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.glag_to_lag_range_tbl.glag_to_lag_range), 3, 0);

  /* Smooth Division */
  Soc_petra_a_tbls.irr.smooth_division_tbl.addr.base = 0x00100000;
  Soc_petra_a_tbls.irr.smooth_division_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.irr.smooth_division_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.smooth_division_tbl.smooth_division), 3, 0);

  /* Glag Mapping */
  Soc_petra_a_tbls.irr.glag_mapping_tbl.addr.base = 0x00200000;
  Soc_petra_a_tbls.irr.glag_mapping_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.irr.glag_mapping_tbl.addr.width_bits = 17;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.glag_mapping_tbl.glag_mapping), 16, 0);

  /* Destination Table */
  Soc_petra_a_tbls.irr.destination_table_tbl.addr.base = 0x00300000;
  Soc_petra_a_tbls.irr.destination_table_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.irr.destination_table_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.destination_table_tbl.queue_number), 14, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.destination_table_tbl.queue_valid), 15, 15);

  /* Glag Next Member */
  Soc_petra_a_tbls.irr.glag_next_member_tbl.addr.base = 0x00400000;
  Soc_petra_a_tbls.irr.glag_next_member_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.irr.glag_next_member_tbl.addr.width_bits = 5;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.glag_next_member_tbl.offset), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.glag_next_member_tbl.rr_lb_mode), 4, 4);

  /* Rlag Next Member */
  Soc_petra_a_tbls.irr.rlag_next_member_tbl.addr.base = 0x00500000;
  Soc_petra_a_tbls.irr.rlag_next_member_tbl.addr.size = 0x0400;
  Soc_petra_a_tbls.irr.rlag_next_member_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.rlag_next_member_tbl.two_lsb), 1, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.irr.rlag_next_member_tbl.two_msb), 3, 2);
}

/* Block tables initialization: IHP */
STATIC void
  soc_pa_tbls_init_IHP(void)
{
  uint32
    fld_idx = 0;

  /* Port Info */
  Soc_petra_a_tbls.ihp.port_info_tbl.addr.base = 0x000e0000;
  Soc_petra_a_tbls.ihp.port_info_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.ihp.port_info_tbl.addr.width_bits = 39;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.port_type), 2, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.bytes_to_skip), 7, 3);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.custom_command_select), 8, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.statistics_profile_extension), 10, 9);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.port_type_extension), 22, 22);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.statistics_profile), 27, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.use_lag_member), 28, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.has_mirror), 29, 29);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.mirror_is_mc), 30, 30);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.bytes_to_remove), 35, 31);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.header_remove), 36, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.append_ftmh), 37, 37);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_info_tbl.append_prog_header), 38, 38);

  /* Port To System Port Id */
  Soc_petra_a_tbls.ihp.port_to_system_port_id_tbl.addr.base = 0x00120000;
  Soc_petra_a_tbls.ihp.port_to_system_port_id_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.ihp.port_to_system_port_id_tbl.addr.width_bits = 13;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.port_to_system_port_id_tbl.port_to_system_port_id), 12, 0);

  /* Static Header */
  Soc_petra_a_tbls.ihp.static_header_tbl.addr.base = 0x00130000;
  Soc_petra_a_tbls.ihp.static_header_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.ihp.static_header_tbl.addr.width_bits = 81;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.static_header_tbl.shaping), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.static_header_tbl.shaping_itmh), 32, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.static_header_tbl.itmh), 80, 33);

  /* System Port My Port Table */
  Soc_petra_a_tbls.ihp.system_port_my_port_table_tbl.addr.base = 0x00140000;
  Soc_petra_a_tbls.ihp.system_port_my_port_table_tbl.addr.size = 0x0400;
  Soc_petra_a_tbls.ihp.system_port_my_port_table_tbl.addr.width_bits = 4;
  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS; fld_idx++)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.system_port_my_port_table_tbl.system_port_my_port_table[fld_idx]),
      (uint8)(fld_idx),
      (uint8)(fld_idx)
    );
  }

  /* Ptc Commands1 */
  Soc_petra_a_tbls.ihp.ptc_commands1_tbl.addr.base = 0x00500000;
  Soc_petra_a_tbls.ihp.ptc_commands1_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ihp.ptc_commands1_tbl.addr.width_bits = 40;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.ptc_commands1_tbl.ptc_commands), 39, 0);

  /* Ptc Commands2 */
  Soc_petra_a_tbls.ihp.ptc_commands2_tbl.addr.base = 0x00510000;
  Soc_petra_a_tbls.ihp.ptc_commands2_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ihp.ptc_commands2_tbl.addr.width_bits = 40;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.ptc_commands2_tbl.ptc_commands), 39, 0);

  /* Ptc Key Program Lut */
  Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.addr.base = 0x00520000;
  Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.addr.width_bits = 72;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.ptc_key_program_ptr), 5, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.ptc_key_program_var), 13, 6);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.ptc_key_program_lut_tbl.ptc_cos_profile), 17, 14);

  /* Key Program */
  Soc_petra_a_tbls.ihp.key_program_tbl.addr.base = 0x00530000;
  Soc_petra_a_tbls.ihp.key_program_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program_tbl.addr.width_bits = 26;
  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_KEY_PROG_NOF_INSTR; fld_idx++)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program_tbl.select[fld_idx]),
      (uint8)(1 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(0 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program_tbl.length[fld_idx]),
      (uint8)(4 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(2 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program_tbl.nibble_shift[fld_idx]),
      (uint8)(5 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(5 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program_tbl.byte_shift[fld_idx]),
      (uint8)(10 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(6 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program_tbl.offset_select[fld_idx]),
      (uint8)(12 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(11 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
  }

  /* Key Program0 */
  Soc_petra_a_tbls.ihp.key_program0_tbl.addr.base = 0x00530000;
  Soc_petra_a_tbls.ihp.key_program0_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program0_tbl.addr.width_bits = 26;
  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_KEY_PROG_NOF_INSTR; fld_idx++)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program0_tbl.select[fld_idx]),
      (uint8)(1 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(0 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program0_tbl.length[fld_idx]),
      (uint8)(4 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(2 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program0_tbl.nibble_shift[fld_idx]),
      (uint8)(5 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(5 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program0_tbl.byte_shift[fld_idx]),
      (uint8)(10 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(6 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.key_program0_tbl.offset_select[fld_idx]),
      (uint8)(12 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx)),
      (uint8)(11 + (SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE*fld_idx))
    );
  }

  /* Key Program1 */
  Soc_petra_a_tbls.ihp.key_program1_tbl.addr.base = 0x00540000;
  Soc_petra_a_tbls.ihp.key_program1_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program1_tbl.addr.width_bits = 26;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program1_tbl.key_program1), 25, 0);

  /* Key Program2 */
  Soc_petra_a_tbls.ihp.key_program2_tbl.addr.base = 0x00550000;
  Soc_petra_a_tbls.ihp.key_program2_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program2_tbl.addr.width_bits = 29;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program2_tbl.instruction4), 12, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program2_tbl.instruction3), 25, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program2_tbl.exact_match_enable), 26, 26);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program2_tbl.exact_match_mask_index), 28, 27);

  /* Key Program3 */
  Soc_petra_a_tbls.ihp.key_program3_tbl.addr.base = 0x00560000;
  Soc_petra_a_tbls.ihp.key_program3_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program3_tbl.addr.width_bits = 26;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program3_tbl.key_program3), 25, 0);

  /* Key Program4 */
  Soc_petra_a_tbls.ihp.key_program4_tbl.addr.base = 0x00570000;
  Soc_petra_a_tbls.ihp.key_program4_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.ihp.key_program4_tbl.addr.width_bits = 45;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.instruction9), 12, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.instruction10), 25, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.tcam_match_enable), 26, 26);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.tcam_match_select), 27, 27);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.tcam_key_and_value), 31, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.tcam_key_or_value), 35, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.bytes_to_remove_hdr_sel), 37, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.bytes_to_remove_hdr_size), 42, 38);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.key_program4_tbl.cos_hdr_var_mask_select), 44, 43);

  /* Programmable Cos */
  for (fld_idx = 0; fld_idx < SOC_PETRA_IHP_PRGR_COS_TBL_FLDS; fld_idx++)
  {
    Soc_petra_a_tbls.ihp.programmable_cos_tbl.addr.base = 0x00580000;
    Soc_petra_a_tbls.ihp.programmable_cos_tbl.addr.size = 0x0100;
    Soc_petra_a_tbls.ihp.programmable_cos_tbl.addr.width_bits = 80;
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.programmable_cos_tbl.drop_precedence[fld_idx]),
      (uint8)(5*fld_idx+1),
      (uint8)(5*fld_idx)
    );
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.ihp.programmable_cos_tbl.traffic_class[fld_idx]),
      (uint8)(5*fld_idx + 4),
      (uint8)(5*fld_idx + 2)
    );
  }

  /* Programmable Cos1 */
  Soc_petra_a_tbls.ihp.programmable_cos1_tbl.addr.base = 0x00620000;
  Soc_petra_a_tbls.ihp.programmable_cos1_tbl.addr.size = 0x0007;
  Soc_petra_a_tbls.ihp.programmable_cos1_tbl.addr.width_bits = 80;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ihp.programmable_cos1_tbl.programmable_cos1), 79, 0);
}


/* Block tables initialization: IQM */
STATIC void
  soc_pa_tbls_init_QDR(void)
{
  /* BDB Link List */
  Soc_petra_a_tbls.qdr.qdr_mem.addr.base = 0x10000000;
  Soc_petra_a_tbls.qdr.qdr_mem.addr.size = 0x400000;
  Soc_petra_a_tbls.qdr.qdr_mem.addr.width_bits = 18;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.qdr.qdr_mem.data), 17, 0);

  Soc_petra_a_tbls.qdr.qdr_dll_mem.addr.base       = 0x20000000;
  Soc_petra_a_tbls.qdr.qdr_dll_mem.addr.size       = 0x80;
  Soc_petra_a_tbls.qdr.qdr_dll_mem.addr.width_bits = 28;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.qdr.qdr_dll_mem.qdr_dll_mem), 27, 0);
}

/* Block tables initialization: IQM */
STATIC void
  soc_pa_tbls_init_IQM(void)
{
  uint32
    fld_idx;

  /* BDB Link List */
  Soc_petra_a_tbls.iqm.bdb_link_list_tbl.addr.base = 0x00000000;
  Soc_petra_a_tbls.iqm.bdb_link_list_tbl.addr.size = 0x10000;
  Soc_petra_a_tbls.iqm.bdb_link_list_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.bdb_link_list_tbl.bdb_link_list), 15, 0);

  /* Packet Queue Descriptor (Dynamic ) */
  Soc_petra_a_tbls.iqm.dynamic_tbl.addr.base = 0x00100000;
  Soc_petra_a_tbls.iqm.dynamic_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.iqm.dynamic_tbl.addr.width_bits = 59;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.dynamic_tbl.pq_head_ptr), 21, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.dynamic_tbl.que_not_empty), 22, 22);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.dynamic_tbl.pq_inst_que_size), 46, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.dynamic_tbl.pq_avrg_szie), 58, 47);

  /* Packet Queue Descriptor (Static) */
  Soc_petra_a_tbls.iqm.static_tbl.addr.base = 0x00200000;
  Soc_petra_a_tbls.iqm.static_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.iqm.static_tbl.addr.width_bits = 20;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.static_tbl.credit_class), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.static_tbl.rate_class), 9, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.static_tbl.connection_class), 14, 10);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.static_tbl.traffic_class), 17, 15);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.static_tbl.que_signature), 19, 18);

  /* Packet Queue Tail Pointer */
  Soc_petra_a_tbls.iqm.packet_queue_tail_pointer_tbl.addr.base = 0x00300000;
  Soc_petra_a_tbls.iqm.packet_queue_tail_pointer_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.iqm.packet_queue_tail_pointer_tbl.addr.width_bits = 22;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_tail_pointer_tbl.tail_ptr), 21, 0);

  /* Packet Queue Red Weight table */
  Soc_petra_a_tbls.iqm.packet_queue_red_weight_table_tbl.addr.base = 0x00400000;
  Soc_petra_a_tbls.iqm.packet_queue_red_weight_table_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.packet_queue_red_weight_table_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_weight_table_tbl.pq_weight), 5, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_weight_table_tbl.avrg_en), 6, 6);

  /* Credit Discount table */
  Soc_petra_a_tbls.iqm.credit_discount_table_tbl.addr.base = 0x00500000;
  Soc_petra_a_tbls.iqm.credit_discount_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.credit_discount_table_tbl.addr.width_bits = 8;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.credit_discount_table_tbl.crdt_disc_val), 6, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.credit_discount_table_tbl.discnt_sign), 7, 7);

  /* Full User Count Memory */
  Soc_petra_a_tbls.iqm.full_user_count_memory_tbl.addr.base = 0x00600000;
  Soc_petra_a_tbls.iqm.full_user_count_memory_tbl.addr.size = 0x2000;
  Soc_petra_a_tbls.iqm.full_user_count_memory_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.full_user_count_memory_tbl.flus_cnt), 11, 0);

  /* Mini-Multicast User Count Memory */
  Soc_petra_a_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.base = 0x00700000;
  Soc_petra_a_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.size = 0x0800;
  Soc_petra_a_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.mini_multicast_user_count_memory_tbl.mn_us_cnt), 63, 0);

  /* Packet Queue Red parameters table */
  Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.base = 0x00800000;
  Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.width_bits = 80;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_mnt), 5, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_exp), 10, 6);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_wred_en), 11, 11);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c2), 43, 12);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c3), 47, 44);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c1), 52, 48);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_avrg_max_th), 64, 53);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_avrg_min_th), 76, 65);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_wred_pckt_sz_ignr), 77, 77);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_queue_red_parameters_table_tbl.addmit_logic), 79, 78);

  /* Packet Descriptor Fifos Memory */
  Soc_petra_a_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.base = 0x00900000;
  Soc_petra_a_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.width_bits = 30;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.packet_descriptor_fifos_memory_tbl.tx_pd), 29, 0);

  /* Tx Descriptor Fifos Memory */
  Soc_petra_a_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.base = 0x00a00000;
  Soc_petra_a_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.width_bits = 85;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.tx_descriptor_fifos_memory_tbl.tx_dscr), 84, 0);

  /* VSQ Descriptor Rate Class - group A */
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base = 0x01100000;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size = 0x0004;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.vsq_rc_a), 3, 0);

  /* VSQ Descriptor Rate Class - group B */
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base = 0x01200000;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.vsq_rc_b), 3, 0);

  /* VSQ Descriptor Rate Class - group C */
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base = 0x01300000;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.vsq_rc_c), 3, 0);

  /* VSQ Descriptor Rate Class - group D */
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base = 0x01400000;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.vsq_rc_d), 3, 0);

  /* VSQ Qsize memory - group A */
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.base = 0x01500000;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.size = 0x0004;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_a_tbl.vsq_size_wrds), 23, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_a_tbl.vsq_size_bds), 45, 24);

  /* VSQ Qsize memory - group B */
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.base = 0x01600000;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_b_tbl.vsq_size_wrds), 23, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_b_tbl.vsq_size_bds), 45, 24);

  /* VSQ Qsize memory - group C */
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.base = 0x01700000;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_c_tbl.vsq_size_wrds), 23, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_c_tbl.vsq_size_bds), 45, 24);

  /* VSQ Qsize memory - group D */
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.base = 0x01800000;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_d_tbl.vsq_size_wrds), 23, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_qsize_memory_group_d_tbl.vsq_size_bds), 45, 24);

  /* VSQ Average Qsize memory - group A */
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.base = 0x01900000;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.size = 0x0004;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group B */
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.base = 0x01a00000;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group C */
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.base = 0x01b00000;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group D */
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.base = 0x01c00000;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Flow-Control Parameters table - group A */
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.base = 0x01d00000;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].wred_en), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].avrg_size_en), 1, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].red_weight_q), 7, 2);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_words_mnt), 12, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_words_exp), 17, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_words_mnt), 22, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_words_exp), 27, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_bd_mnt), 31, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_bd_exp), 36, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_bd_mnt), 40, 37);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_bd_exp), 45, 41);

  /* VSQ Flow-Control Parameters table - group B */
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.base = 0x01e00000;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].wred_en), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].avrg_size_en), 1, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].red_weight_q), 7, 2);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_words_mnt), 12, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_words_exp), 17, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_words_mnt), 22, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_words_exp), 27, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_bd_mnt), 31, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_bd_exp), 36, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_bd_mnt), 40, 37);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_bd_exp), 45, 41);

  /* VSQ Flow-Control Parameters table - group C */
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.base = 0x01f00000;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].wred_en), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].avrg_size_en), 1, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].red_weight_q), 7, 2);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_words_mnt), 12, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_words_exp), 17, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_words_mnt), 22, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_words_exp), 27, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_bd_mnt), 31, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_bd_exp), 36, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_bd_mnt), 40, 37);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_bd_exp), 45, 41);

  /* VSQ Flow-Control Parameters table - group D */
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.base = 0x02000000;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].wred_en), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].avrg_size_en), 1, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].red_weight_q), 7, 2);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_words_mnt), 12, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_words_exp), 17, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_words_mnt), 22, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_words_exp), 27, 23);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_bd_mnt), 31, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_bd_exp), 36, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_bd_mnt), 40, 37);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_bd_exp), 45, 41);

  /* VSQ Queue Parameters table - group A */
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.base = 0x02100000;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.width_bits = 89;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c2), 31, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c3), 35, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c1), 40, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].max_avrg_th), 52, 41);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].min_avrg_th), 64, 53);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_szie_bds_mnt), 72, 66);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_szie_bds_exp), 76, 73);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_size_words_mnt), 83, 77);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_size_words_exp), 88, 84);

  /* VSQ Queue Parameters table - group B */
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.base = 0x02200000;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.width_bits = 89;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c2), 31, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c3), 35, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c1), 40, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].max_avrg_th), 52, 41);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].min_avrg_th), 64, 53);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_szie_bds_mnt), 72, 66);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_szie_bds_exp), 76, 73);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_size_words_mnt), 83, 77);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_size_words_exp), 88, 84);

  /* VSQ Queue Parameters table - group C */
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.base = 0x02300000;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.width_bits = 89;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c2), 31, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c3), 35, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c1), 40, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].max_avrg_th), 52, 41);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].min_avrg_th), 64, 53);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_szie_bds_mnt), 72, 66);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_szie_bds_exp), 76, 73);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_size_words_mnt), 83, 77);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_size_words_exp), 88, 84);

  /* VSQ Queue Parameters table - group D */
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.base = 0x02400000;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.width_bits = 89;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c2), 31, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c3), 35, 32);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c1), 40, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].max_avrg_th), 52, 41);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].min_avrg_th), 64, 53);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_szie_bds_mnt), 72, 66);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_szie_bds_exp), 76, 73);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_size_words_mnt), 83, 77);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_size_words_exp), 88, 84);

  /* System Red parameters table */
  Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.addr.base = 0x02500000;
  Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.addr.width_bits = 21;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.adm_th), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.prob_th), 7, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.drp_th), 11, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.drp_prob_indx1), 15, 12);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.drp_prob_indx2), 19, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_parameters_table_tbl.sys_red_en), 20, 20);

  /* System Red drop probability values */
  Soc_petra_a_tbls.iqm.system_red_drop_probability_values_tbl.addr.base = 0x02600000;
  Soc_petra_a_tbls.iqm.system_red_drop_probability_values_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.iqm.system_red_drop_probability_values_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.iqm.system_red_drop_probability_values_tbl.drp_prob), 15, 0);

  /* Source Qsize range thresholds (System Red) */
  Soc_petra_a_tbls.iqm.system_red_tbl.addr.base = 0x02700000;
  Soc_petra_a_tbls.iqm.system_red_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.iqm.system_red_tbl.addr.width_bits = 180;
  for (fld_idx = 0; fld_idx < SOC_PETRA_SYS_RED_NOF_Q_RNGS; ++fld_idx)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.iqm.system_red_tbl.qsz_rng_th[fld_idx]),
      (uint8)(12*(fld_idx+1)-1),
      (uint8)(12*(fld_idx))
    );
  }

}

/* Block tables initialization: IPS */
STATIC void
  soc_pa_tbls_init_IPS(void)
{

  /* System Physical Port Lookup Table */
  Soc_petra_a_tbls.ips.system_physical_port_lookup_table_tbl.addr.base = 0x00000000;
  Soc_petra_a_tbls.ips.system_physical_port_lookup_table_tbl.addr.size = 0x2000;
  Soc_petra_a_tbls.ips.system_physical_port_lookup_table_tbl.addr.width_bits = 12;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.system_physical_port_lookup_table_tbl.sys_phy_port), 11, 0);

  /* Destination Device And Port Lookup Table */
  Soc_petra_a_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.base = 0x00008000;
  Soc_petra_a_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.width_bits = 19;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.destination_device_and_port_lookup_table_tbl.dest_port), 7, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.destination_device_and_port_lookup_table_tbl.dest_dev), 18, 8);

  /* Flow Id Lookup Table */
  Soc_petra_a_tbls.ips.flow_id_lookup_table_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.ips.flow_id_lookup_table_tbl.addr.size = 0x2000;
  Soc_petra_a_tbls.ips.flow_id_lookup_table_tbl.addr.width_bits = 15;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.flow_id_lookup_table_tbl.base_flow), 13, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.flow_id_lookup_table_tbl.sub_flow_mode), 14, 14);

  /* Queue Type Lookup Table */
  Soc_petra_a_tbls.ips.queue_type_lookup_table_tbl.addr.base = 0x00018000;
  Soc_petra_a_tbls.ips.queue_type_lookup_table_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.ips.queue_type_lookup_table_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_type_lookup_table_tbl.queue_type_lookup_table), 3, 0);

  /* Queue Priority Map Select */
  Soc_petra_a_tbls.ips.queue_priority_map_select_tbl.addr.base = 0x00020000;
  Soc_petra_a_tbls.ips.queue_priority_map_select_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ips.queue_priority_map_select_tbl.addr.width_bits = 2;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_priority_map_select_tbl.queue_priority_map_select), 1, 0);

  /* Queue Priority Maps Table */
  Soc_petra_a_tbls.ips.queue_priority_maps_table_tbl.addr.base = 0x00028000;
  Soc_petra_a_tbls.ips.queue_priority_maps_table_tbl.addr.size = 0x0004;
  Soc_petra_a_tbls.ips.queue_priority_maps_table_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_priority_maps_table_tbl.queue_priority_maps_table), 63, 0);

  /* Queue Size-Based Thresholds Table */
  Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.addr.base = 0x00030000;
  Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.addr.width_bits = 36;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.off_to_slow_msg_th), 7, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.off_to_norm_msg_th), 15, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.slow_to_norm_msg_th), 23, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.norm_to_slow_msg_th), 31, 24);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_based_thresholds_table_tbl.fsm_th_mul), 35, 32);

  /* Credit Balance Based Thresholds Table */
  Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.base = 0x00038000;
  Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.width_bits = 32;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.backoff_enter_qcr_bal_th), 7, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.backoff_exit_qcr_bal_th), 15, 8);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.backlog_enter_qcr_bal_th), 23, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_balance_based_thresholds_table_tbl.backlog_exit_qcr_bal_th), 31, 24);

  /* Empty Queue Credit Balance Table */
  Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.addr.base = 0x00040000;
  Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.empty_qsatisfied_cr_bal), 15, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.max_empty_qcr_bal), 31, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.empty_queue_credit_balance_table_tbl.exceed_max_empty_qcr_bal), 32, 32);

  /* Credit Watchdog Thresholds Table */
  Soc_petra_a_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.base = 0x00048000;
  Soc_petra_a_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.width_bits = 8;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_watchdog_thresholds_table_tbl.wd_status_msg_gen_period), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.credit_watchdog_thresholds_table_tbl.wd_delete_qth), 7, 4);

  /* Queue Descriptor Table */
  Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.addr.base = 0x00050000;
  Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.addr.width_bits = 28;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.cr_bal), 15, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.crs), 17, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.one_pkt_deq), 18, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.wd_last_cr_time), 23, 19);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.in_dqcq), 24, 24);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.wd_delete), 25, 25);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_descriptor_table_tbl.fsmrq_ctrl), 27, 26);

  /* Queue Size Table */
  Soc_petra_a_tbls.ips.queue_size_table_tbl.addr.base = 0x00058000;
  Soc_petra_a_tbls.ips.queue_size_table_tbl.addr.size = 0x8000;
  Soc_petra_a_tbls.ips.queue_size_table_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_table_tbl.exponent), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_table_tbl.mantissa), 6, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.queue_size_table_tbl.qsize_4b), 10, 7);

  /* System Red Max Queue Size Table */
  Soc_petra_a_tbls.ips.system_red_max_queue_size_table_tbl.addr.base = 0x00078000;
  Soc_petra_a_tbls.ips.system_red_max_queue_size_table_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.ips.system_red_max_queue_size_table_tbl.addr.width_bits = 6;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.system_red_max_queue_size_table_tbl.maxqsz), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ips.system_red_max_queue_size_table_tbl.maxqsz_age), 5, 4);
}

/* Block tables initialization: IPT */
STATIC void
  soc_pa_tbls_init_IPT(void)
{
  /* BDQ */
  Soc_petra_a_tbls.ipt.bdq_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.ipt.bdq_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.ipt.bdq_tbl.addr.width_bits = 26;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.bdq_tbl.bdq), 25, 0);

  /* PCQ */
  Soc_petra_a_tbls.ipt.pcq_tbl.addr.base = 0x00020000;
  Soc_petra_a_tbls.ipt.pcq_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.ipt.pcq_tbl.addr.width_bits = 62;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.pcq_tbl.pcq), 61, 0);

  /* Sop MMU */
  Soc_petra_a_tbls.ipt.sop_mmu_tbl.addr.base = 0x00030000;
  Soc_petra_a_tbls.ipt.sop_mmu_tbl.addr.size = 0x0600;
  Soc_petra_a_tbls.ipt.sop_mmu_tbl.addr.width_bits = 62;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.sop_mmu_tbl.sop_mmu), 61, 0);

  /* Mop MMU */
  Soc_petra_a_tbls.ipt.mop_mmu_tbl.addr.base = 0x00040000;
  Soc_petra_a_tbls.ipt.mop_mmu_tbl.addr.size = 0x0600;
  Soc_petra_a_tbls.ipt.mop_mmu_tbl.addr.width_bits = 30;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.mop_mmu_tbl.mop_mmu), 29, 0);

  /* FDTCTL */
  Soc_petra_a_tbls.ipt.fdtctl_tbl.addr.base = 0x00050000;
  Soc_petra_a_tbls.ipt.fdtctl_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ipt.fdtctl_tbl.addr.width_bits = 29;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.fdtctl_tbl.fdtctl), 28, 0);

  /* FDTDATA */
  Soc_petra_a_tbls.ipt.fdtdata_tbl.addr.base = 0x00060000;
  Soc_petra_a_tbls.ipt.fdtdata_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.ipt.fdtdata_tbl.addr.width_bits = 512;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.fdtdata_tbl.fdtdata), 511, 0);

  /* EGQCTL */
  Soc_petra_a_tbls.ipt.egqctl_tbl.addr.base = 0x00070000;
  Soc_petra_a_tbls.ipt.egqctl_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.ipt.egqctl_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.egqctl_tbl.egqctl), 6, 0);

  /* EGQDATA */
  Soc_petra_a_tbls.ipt.egqdata_tbl.addr.base = 0x00080000;
  Soc_petra_a_tbls.ipt.egqdata_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.ipt.egqdata_tbl.addr.width_bits = 512;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.egqdata_tbl.egqdata), 511, 0);

  Soc_petra_a_tbls.ipt.select_source_sum_tbl.addr.base = 0x000a0000;
  Soc_petra_a_tbls.ipt.select_source_sum_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.ipt.select_source_sum_tbl.addr.width_bits = 47;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.ipt.select_source_sum_tbl.select_source_sum), 46, 0);
}

/* Block tables initialization: DPI */
STATIC void
  soc_pa_tbls_init_DPI(void)
{

  /* DLL_RAM */
  Soc_petra_a_tbls.dpi.dll_ram_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.dpi.dll_ram_tbl.addr.size = 0x0080;
  Soc_petra_a_tbls.dpi.dll_ram_tbl.addr.width_bits = 56;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.dpi.dll_ram_tbl.dll_ram), 55, 0);
}

/* Block tables initialization: RTP */
STATIC void
  soc_pa_tbls_init_RTP(void)
{

  /* Unicast Distribution Memory for data cells */
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base = 0x00000000;
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.size = 0x0800;
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.width_bits = 72;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.unicast_distribution_memory_for_data_cells), 71, 0);

  /* Unicast Distribution Memory for control cells */
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base = 0x00001000;
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.size = 0x0800;
  Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.width_bits = 72;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.unicast_distribution_memory_for_control_cells), 71, 0);
}

/* Block tables initialization: EGQ */
STATIC void
  soc_pa_tbls_init_EGQ(void)
{
  /* Egress Shaper Credit Configuration (Nif Ch Scm) */
  Soc_petra_a_tbls.egq.nif_scm_tbl.addr.base = 0x00010000;
  Soc_petra_a_tbls.egq.nif_scm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.nif_scm_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nif_scm_tbl.port_cr_to_add), 17, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nif_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper Nifa And Nifb Non Channeleized Ports Credit Configuration (Nifab Nch Scm) */
  Soc_petra_a_tbls.egq.nifab_nch_scm_tbl.addr.base = 0x00090000;
  Soc_petra_a_tbls.egq.nifab_nch_scm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.nifab_nch_scm_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nifab_nch_scm_tbl.port_cr_to_add), 17, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nifab_nch_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper Recycling Ports Credit Configuration (Rcy Scm) */
  Soc_petra_a_tbls.egq.rcy_scm_tbl.addr.base = 0x000a0000;
  Soc_petra_a_tbls.egq.rcy_scm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.rcy_scm_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rcy_scm_tbl.port_cr_to_add), 17, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rcy_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper CPUPorts Credit Configuration(Cpu Scm) */
  Soc_petra_a_tbls.egq.cpu_scm_tbl.addr.base = 0x000b0000;
  Soc_petra_a_tbls.egq.cpu_scm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.cpu_scm_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cpu_scm_tbl.port_cr_to_add), 17, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cpu_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper Calendar Selector (CCM) */
  Soc_petra_a_tbls.egq.ccm_tbl.addr.base = 0x000c0000;
  Soc_petra_a_tbls.egq.ccm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.ccm_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ccm_tbl.interface_select), 3, 0);

  /* Per Port Max Credit Memory (PMC) */
  Soc_petra_a_tbls.egq.pmc_tbl.addr.base = 0x000d0000;
  Soc_petra_a_tbls.egq.pmc_tbl.addr.size = 0x00a0;
  Soc_petra_a_tbls.egq.pmc_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pmc_tbl.port_max_credit), 15, 0);

  /* Egress Shaper Per Port Credit Balance Memory (CBM) */
  Soc_petra_a_tbls.egq.cbm_tbl.addr.base = 0x000e0000;
  Soc_petra_a_tbls.egq.cbm_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.egq.cbm_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cbm_tbl.cbm), 24, 0);

  /* Free Buffesr Allocation Bitmap Memory (FBM) */
  Soc_petra_a_tbls.egq.fbm_tbl.addr.base = 0x000f0000;
  Soc_petra_a_tbls.egq.fbm_tbl.addr.size = 0x0080;
  Soc_petra_a_tbls.egq.fbm_tbl.addr.width_bits = 32;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.fbm_tbl.free_buffer_memory), 31, 0);

  /* Free Descriptors Allocation Bitmap Memory (FDM) */
  Soc_petra_a_tbls.egq.fdm_tbl.addr.base = 0x00100000;
  Soc_petra_a_tbls.egq.fdm_tbl.addr.size = 0x0080;
  Soc_petra_a_tbls.egq.fdm_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.fdm_tbl.free_descriptor_memory), 63, 0);

  /* Ofp Dequeue Wfq Configuration Memory (DWM) */
  Soc_petra_a_tbls.egq.dwm_tbl.addr.base = 0x00110000;
  Soc_petra_a_tbls.egq.dwm_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.egq.dwm_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.dwm_tbl.mc_or_mc_low_queue_weight), 15, 8); /* Wrong documentation */
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.dwm_tbl.uc_or_uc_low_queue_weight), 7, 0);

  /* RRDM */
  Soc_petra_a_tbls.egq.rrdm_tbl.addr.base = 0x00120000;
  Soc_petra_a_tbls.egq.rrdm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.rrdm_tbl.addr.width_bits = 46;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.crcremainder), 15, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.reas_state), 17, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.eopfrag_num), 26, 18);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.nxt_frag_number), 35, 27);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.stored_seg_size), 37, 36);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rrdm_tbl.fix129), 38, 38);

  /* RPDM */
  Soc_petra_a_tbls.egq.rpdm_tbl.addr.base = 0x00130000;
  Soc_petra_a_tbls.egq.rpdm_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.egq.rpdm_tbl.addr.width_bits = 41;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rpdm_tbl.packet_start_buffer_pointer), 11, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rpdm_tbl.packet_buffer_write_pointer), 24, 12);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rpdm_tbl.reas_state), 26, 25);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rpdm_tbl.packet_frag_cnt), 34, 27);

  /* Port Configuration Table (PCT) */
  Soc_petra_a_tbls.egq.pct_tbl.addr.base = 0x00140000;
  Soc_petra_a_tbls.egq.pct_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.egq.pct_tbl.addr.width_bits = 72;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.outbound_mirr), 0, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.is_mirroring), 1, 1);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.otmh_lif_ext_ena), 3, 2);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.otmh_dest_ext_ena), 4, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.otmh_src_ext_ena), 5, 5);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.port_type), 49, 48);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.port_channel_number), 55, 50);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.destination_port_extension_id), 68, 56);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pct_tbl.header_compensation_type), 69, 69);

  /* Per Port Configuration Table(PPCT) */
  Soc_petra_a_tbls.egq.ppct_tbl.addr.base = 0x00160000;
  Soc_petra_a_tbls.egq.ppct_tbl.addr.size = 0x0050;
  Soc_petra_a_tbls.egq.ppct_tbl.addr.width_bits = 82;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.multicast_lag_load_balancing_enable), 3, 3);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.sys_port_id), 15, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.glag_member_port_id), 19, 16);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.glag_port_id), 27, 20);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.port_is_glag_member), 28, 28);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ppct_tbl.port_type), 81, 80);


  /* Vlan Table Configuration Memory(Vlan Table) */
  Soc_petra_a_tbls.egq.vlan_table_tbl.addr.base = 0x00150000;
  Soc_petra_a_tbls.egq.vlan_table_tbl.addr.size = 0x1000;
  Soc_petra_a_tbls.egq.vlan_table_tbl.addr.width_bits = 80;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.vlan_table_tbl.vlan_membership), 79, 0);


  /* CfcFlowControl */
  Soc_petra_a_tbls.egq.cfc_flow_control_tbl.addr.base = 0x00180000;
  Soc_petra_a_tbls.egq.cfc_flow_control_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.cfc_flow_control_tbl.addr.width_bits = 80;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cfc_flow_control_tbl.cfc_flow_control), 79, 0);

  /* NifaFlowControl */
  Soc_petra_a_tbls.egq.nifa_flow_control_tbl.addr.base = 0x00190000;
  Soc_petra_a_tbls.egq.nifa_flow_control_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.nifa_flow_control_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nifa_flow_control_tbl.nifa_flow_control), 15, 0);

  /* NifbFlowControl */
  Soc_petra_a_tbls.egq.nifb_flow_control_tbl.addr.base = 0x001a0000;
  Soc_petra_a_tbls.egq.nifb_flow_control_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.nifb_flow_control_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.nifb_flow_control_tbl.nifb_flow_control), 15, 0);

  /* CpuLastHeader */
  Soc_petra_a_tbls.egq.cpu_last_header_tbl.addr.base = 0x001b0000;
  Soc_petra_a_tbls.egq.cpu_last_header_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.cpu_last_header_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cpu_last_header_tbl.cpu_last_header), 63, 0);

  /* IptLastHeader */
  Soc_petra_a_tbls.egq.ipt_last_header_tbl.addr.base = 0x001c0000;
  Soc_petra_a_tbls.egq.ipt_last_header_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ipt_last_header_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ipt_last_header_tbl.ipt_last_header), 63, 0);

  /* FdrLastHeader */
  Soc_petra_a_tbls.egq.fdr_last_header_tbl.addr.base = 0x001d0000;
  Soc_petra_a_tbls.egq.fdr_last_header_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.fdr_last_header_tbl.addr.width_bits = 64;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.fdr_last_header_tbl.fdr_last_header), 63, 0);

  /* CpuPacketCounter */
  Soc_petra_a_tbls.egq.cpu_packet_counter_tbl.addr.base = 0x001e0000;
  Soc_petra_a_tbls.egq.cpu_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.cpu_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.cpu_packet_counter_tbl.cpu_packet_counter), 32, 0);

  /* IptPacketCounter */
  Soc_petra_a_tbls.egq.ipt_packet_counter_tbl.addr.base = 0x001f0000;
  Soc_petra_a_tbls.egq.ipt_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ipt_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ipt_packet_counter_tbl.ipt_packet_counter), 32, 0);

  /* FdrPacketCounter */
  Soc_petra_a_tbls.egq.fdr_packet_counter_tbl.addr.base = 0x00200000;
  Soc_petra_a_tbls.egq.fdr_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.fdr_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.fdr_packet_counter_tbl.fdr_packet_counter), 32, 0);

  /* RqpPacketCounter */
  Soc_petra_a_tbls.egq.rqp_packet_counter_tbl.addr.base = 0x00210000;
  Soc_petra_a_tbls.egq.rqp_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.rqp_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rqp_packet_counter_tbl.rqp_packet_counter), 32, 0);

  /* RqpDiscardPacketCounter */
  Soc_petra_a_tbls.egq.rqp_discard_packet_counter_tbl.addr.base = 0x00220000;
  Soc_petra_a_tbls.egq.rqp_discard_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.rqp_discard_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.rqp_discard_packet_counter_tbl.rqp_discard_packet_counter), 32, 0);

  /* EhpUnicastPacketCounter */
  Soc_petra_a_tbls.egq.ehp_unicast_packet_counter_tbl.addr.base = 0x00230000;
  Soc_petra_a_tbls.egq.ehp_unicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ehp_unicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ehp_unicast_packet_counter_tbl.ehp_unicast_packet_counter), 32, 0);

  /* EhpMulticastHighPacketCounter */
  Soc_petra_a_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.base = 0x00240000;
  Soc_petra_a_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ehp_multicast_high_packet_counter_tbl.ehp_multicast_high_packet_counter), 32, 0);

  /* EhpMulticastLowPacketCounter */
  Soc_petra_a_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.base = 0x00250000;
  Soc_petra_a_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ehp_multicast_low_packet_counter_tbl.ehp_multicast_low_packet_counter), 32, 0);

  /* EhpDiscardPacketCounter */
  Soc_petra_a_tbls.egq.ehp_discard_packet_counter_tbl.addr.base = 0x00260000;
  Soc_petra_a_tbls.egq.ehp_discard_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.ehp_discard_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.ehp_discard_packet_counter_tbl.ehp_discard_packet_counter), 32, 0);

  /* PqpUnicastHighPacketCounter */
  Soc_petra_a_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.base = 0x00270000;
  Soc_petra_a_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_unicast_high_packet_counter_tbl.pqp_unicast_high_packet_counter), 32, 0);

  /* PqpUnicastLowPacketCounter */
  Soc_petra_a_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.base = 0x00280000;
  Soc_petra_a_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_unicast_low_packet_counter_tbl.pqp_unicast_low_packet_counter), 32, 0);

  /* PqpMulticastHighPacketCounter */
  Soc_petra_a_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.base = 0x00290000;
  Soc_petra_a_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_multicast_high_packet_counter_tbl.pqp_multicast_high_packet_counter), 32, 0);

  /* PqpMulticastLowPacketCounter */
  Soc_petra_a_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.base = 0x002a0000;
  Soc_petra_a_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_multicast_low_packet_counter_tbl.pqp_multicast_low_packet_counter), 32, 0);

  /* PqpUnicastHighBytesCounter */
  Soc_petra_a_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.base = 0x002b0000;
  Soc_petra_a_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.width_bits = 47;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_unicast_high_bytes_counter_tbl.pqp_unicast_high_bytes_counter), 46, 0);

  /* PqpUnicastLowBytesCounter */
  Soc_petra_a_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.base = 0x002c0000;
  Soc_petra_a_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.width_bits = 47;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_unicast_low_bytes_counter_tbl.pqp_unicast_low_bytes_counter), 46, 0);

  /* PqpMulticastHighBytesCounter */
  Soc_petra_a_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.base = 0x002d0000;
  Soc_petra_a_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.width_bits = 47;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_multicast_high_bytes_counter_tbl.pqp_multicast_high_bytes_counter), 46, 0);

  /* PqpMulticastLowBytesCounter */
  Soc_petra_a_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.base = 0x002e0000;
  Soc_petra_a_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.width_bits = 47;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_multicast_low_bytes_counter_tbl.pqp_multicast_low_bytes_counter), 46, 0);

  /* PqpDiscardUnicastPacketCounter */
  Soc_petra_a_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.base = 0x002f0000;
  Soc_petra_a_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_discard_unicast_packet_counter_tbl.pqp_discard_unicast_packet_counter), 32, 0);

  /* PqpDiscardMulticastPacketCounter */
  Soc_petra_a_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.base = 0x00300000;
  Soc_petra_a_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.pqp_discard_multicast_packet_counter_tbl.pqp_discard_multicast_packet_counter), 32, 0);

  /* FqpPacketCounter */
  Soc_petra_a_tbls.egq.fqp_packet_counter_tbl.addr.base = 0x00310000;
  Soc_petra_a_tbls.egq.fqp_packet_counter_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.egq.fqp_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.egq.fqp_packet_counter_tbl.fqp_packet_counter), 32, 0);
}



/* Block tables initialization: EPNI */
STATIC void
  soc_pa_tbls_init_EPNI(void)
{
  /* EPE2PNI packet counter */
  Soc_petra_a_tbls.epni.epe_packet_counter_tbl.addr.base = 0x00040000;
  Soc_petra_a_tbls.epni.epe_packet_counter_tbl.addr.size = 0x1;
  Soc_petra_a_tbls.epni.epe_packet_counter_tbl.addr.width_bits = 33;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.epni.epe_packet_counter_tbl.epe_packet_counter), 32, 0);

  /* EPE2PNI bytes counter */
  Soc_petra_a_tbls.epni.epe_bytes_counter_tbl.addr.base = 0x00050000;
  Soc_petra_a_tbls.epni.epe_bytes_counter_tbl.addr.size = 0x1;
  Soc_petra_a_tbls.epni.epe_bytes_counter_tbl.addr.width_bits = 49;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.epni.epe_bytes_counter_tbl.epe_bytes_counter), 46, 0);

}
/* Block tables initialization: CFC */
STATIC void
  soc_pa_tbls_init_CFC(void)
{
  uint32
    fld_idx = 0;

  /* Recycle to Out Going Fap Port Mapping */
  Soc_petra_a_tbls.cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.base = 0x00000000;
  Soc_petra_a_tbls.cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.size = 0x0020;
  Soc_petra_a_tbls.cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.recycle_to_out_going_fap_port_mapping_tbl.egq_ofp_num), 6, 0);

  /* NIF A Class Based to OFP mapping */
  Soc_petra_a_tbls.cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.base = 0x00100000;
  Soc_petra_a_tbls.cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.width_bits = 14;
  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.cfc.nif_a_class_based_to_ofp_mapping_tbl.egq_ofp_num[fld_idx]),
      (uint8)(6 + ((6-0+1)*fld_idx)),
      (uint8)(0 + ((6-0+1)*fld_idx))
    );
  }

  /* NIF B Class Based to OFP mapping */
  Soc_petra_a_tbls.cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.base = 0x00200000;
  Soc_petra_a_tbls.cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.size = 0x0010;
  Soc_petra_a_tbls.cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.width_bits = 14;
  for (fld_idx = 0; fld_idx < SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS; fld_idx++)
  {
    soc_pa_tbl_fld_set(
      &(Soc_petra_a_tbls.cfc.nif_b_class_based_to_ofp_mapping_tbl.egq_ofp_num[fld_idx]),
      (uint8)(6 + ((6-0+1)*fld_idx)),
      (uint8)(0 + ((6-0+1)*fld_idx))
    );
  }
  /* Out Of Band (A) Scheduler's based flow-control to OFP mapping */
  Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base = 0x00400000;
  Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size = 0x0080;
  Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.ofp_hr), 6, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.lp_ofp_valid), 7, 7);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.hp_ofp_valid), 8, 8);

  /* Out Of Band (B) Scheduler's based flow-control to OFP mapping */
  Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.base = 0x00500000;
  Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size = 0x0080;
  Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.ofp_hr), 6, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.lp_ofp_valid), 7, 7);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.hp_ofp_valid), 8, 8);

  /* Out Of Band Rx A  calendar mapping */
  Soc_petra_a_tbls.cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.base = 0x00600000;
  Soc_petra_a_tbls.cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_rx_a_calendar_mapping_tbl.fc_index), 6, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_rx_a_calendar_mapping_tbl.fc_dest_sel), 8, 7);

  /* Out Of Band Rx B calendar mapping */
  Soc_petra_a_tbls.cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.base = 0x00700000;
  Soc_petra_a_tbls.cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_rx_b_calendar_mapping_tbl.fc_index), 6, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_rx_b_calendar_mapping_tbl.fc_dest_sel), 8, 7);

  /* Out Of Band Tx  calendar mapping */
  Soc_petra_a_tbls.cfc.out_of_band_tx_calendar_mapping_tbl.addr.base = 0x00800000;
  Soc_petra_a_tbls.cfc.out_of_band_tx_calendar_mapping_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.cfc.out_of_band_tx_calendar_mapping_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_tx_calendar_mapping_tbl.fc_index), 8, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.cfc.out_of_band_tx_calendar_mapping_tbl.fc_source_sel), 10, 9);
}

/* Block tables initialization: SCH */
STATIC void
  soc_pa_tbls_init_SCH(void)
{
  uint32
    fld_idx = 0;

  /* Scheduler Credit Generation Calendar (CAL) */
  Soc_petra_a_tbls.sch.cal_tbl.addr.base = 0x40000000;
  Soc_petra_a_tbls.sch.cal_tbl.addr.size = 0xf400;
  Soc_petra_a_tbls.sch.cal_tbl.addr.width_bits = 7;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.cal_tbl.hrsel), 6, 0);

  /* Device Rate Memory (DRM) */
  Soc_petra_a_tbls.sch.drm_tbl.addr.base = 0x40010000;
  Soc_petra_a_tbls.sch.drm_tbl.addr.size = 0x0128;
  Soc_petra_a_tbls.sch.drm_tbl.addr.width_bits = 18;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.drm_tbl.device_rate), 17, 0);

  /* Dual Shaper Memory (DSM) */
  Soc_petra_a_tbls.sch.dsm_tbl.addr.base = 0x40020000;
  Soc_petra_a_tbls.sch.dsm_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.sch.dsm_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.dsm_tbl.dual_shaper_ena), 15, 0);

  /* Flow Descriptor Memory Static (FDMS) */
  Soc_petra_a_tbls.sch.fdms_tbl.addr.base = 0x40030000;
  Soc_petra_a_tbls.sch.fdms_tbl.addr.size = 0xe000;
  Soc_petra_a_tbls.sch.fdms_tbl.addr.width_bits = 23;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fdms_tbl.sch_number), 13, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fdms_tbl.cos), 21, 14);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fdms_tbl.hrsel_dual), 22, 22);

  /* Shaper Descriptor Memory Static (SHDS) */
  Soc_petra_a_tbls.sch.shds_tbl.addr.base = 0x40040000;
  Soc_petra_a_tbls.sch.shds_tbl.addr.size = 0x7000;
  Soc_petra_a_tbls.sch.shds_tbl.addr.width_bits = 42;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.peak_rate_man_even), 5, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.peak_rate_exp_even), 9, 6);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.max_burst_even), 18, 10);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.slow_rate2_sel_even), 19, 19);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.peak_rate_man_odd), 25, 20);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.peak_rate_exp_odd), 29, 26);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.max_burst_odd), 38, 30);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.slow_rate2_sel_odd), 39, 39);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.max_burst_update_even), 40, 40);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shds_tbl.max_burst_update_odd), 41, 41);

  /* Scheduler Enable Memory (SEM) */
  Soc_petra_a_tbls.sch.sem_tbl.addr.base = 0x40050000;
  Soc_petra_a_tbls.sch.sem_tbl.addr.size = 0x0800;
  Soc_petra_a_tbls.sch.sem_tbl.addr.width_bits = 8;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sem_tbl.sch_enable), 7, 0);

  /* Flow Sub-Flow (FSF) */
  Soc_petra_a_tbls.sch.fsf_tbl.addr.base = 0x40060000;
  Soc_petra_a_tbls.sch.fsf_tbl.addr.size = 0x0700;
  Soc_petra_a_tbls.sch.fsf_tbl.addr.width_bits = 16;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fsf_tbl.sfenable), 15, 0);

  /* Flow Group Memory (FGM) */
  Soc_petra_a_tbls.sch.fgm_tbl.addr.base = 0x40070000;
  Soc_petra_a_tbls.sch.fgm_tbl.addr.size = 0x0800;
  Soc_petra_a_tbls.sch.fgm_tbl.addr.width_bits = 16;
  for (fld_idx = 0; fld_idx < SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
    soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fgm_tbl.flow_group[fld_idx]), (uint8)(2*fld_idx+1), (uint8)(2*fld_idx));
  }

  /* HR-Scheduler-Configuration (SHC) */
  Soc_petra_a_tbls.sch.shc_tbl.addr.base = 0x40080000;
  Soc_petra_a_tbls.sch.shc_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.sch.shc_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shc_tbl.hrmode), 1, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.shc_tbl.hrmask_type), 3, 2);

  /* CL-Schedulers Configuration (SCC) */
  Soc_petra_a_tbls.sch.scc_tbl.addr.base = 0x40090000;
  Soc_petra_a_tbls.sch.scc_tbl.addr.size = 0x2000;
  Soc_petra_a_tbls.sch.scc_tbl.addr.width_bits = 8;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.scc_tbl.clsch_type), 7, 0);

  /* CL-Schedulers Type (SCT) */
  Soc_petra_a_tbls.sch.sct_tbl.addr.base = 0x400a0000;
  Soc_petra_a_tbls.sch.sct_tbl.addr.size = 0x0100;
  Soc_petra_a_tbls.sch.sct_tbl.addr.width_bits = 48;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.clconfig), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.af0_inv_weight), 13, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.af1_inv_weight), 23, 14);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.af2_inv_weight), 33, 24);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.af3_inv_weight), 43, 34);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.wfqmode), 45, 44);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.enh_clen), 46, 46);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.sct_tbl.enh_clsphigh), 47, 47);

  /* Flow to Queue Mapping (FQM) */
  Soc_petra_a_tbls.sch.fqm_tbl.addr.base = 0x400b0000;
  Soc_petra_a_tbls.sch.fqm_tbl.addr.size = 0x3800;
  Soc_petra_a_tbls.sch.fqm_tbl.addr.width_bits = 18;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fqm_tbl.base_queue_num), 12, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fqm_tbl.sub_flow_mode), 13, 13);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.fqm_tbl.flow_slow_enable), 17, 14);

  /* Flow to FIP Mapping (FFM) */
  Soc_petra_a_tbls.sch.ffm_tbl.addr.base = 0x400c0000;
  Soc_petra_a_tbls.sch.ffm_tbl.addr.size = 0x1c00;
  Soc_petra_a_tbls.sch.ffm_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.ffm_tbl.device_number), 10, 0);

  /* Token Memory Controller (TMC) */
  Soc_petra_a_tbls.sch.soc_tmctbl.addr.base = 0x40100000;
  Soc_petra_a_tbls.sch.soc_tmctbl.addr.size = 0xe000;
  Soc_petra_a_tbls.sch.soc_tmctbl.addr.width_bits = 10;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.soc_tmctbl.token_count), 8, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.soc_tmctbl.slow_status), 9, 9);

  /* Port Queue Size (PQS) */
  Soc_petra_a_tbls.sch.pqs_tbl.addr.base = 0x401d0000;
  Soc_petra_a_tbls.sch.pqs_tbl.addr.size = 0x0051;
  Soc_petra_a_tbls.sch.pqs_tbl.addr.width_bits = 21;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.pqs_tbl.max_qsz), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.pqs_tbl.flow_id), 19, 4);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.pqs_tbl.aging_bit), 20, 20);

  /* Scheduler Init */
  Soc_petra_a_tbls.sch.scheduler_init_tbl.addr.base = 0x41000000;
  Soc_petra_a_tbls.sch.scheduler_init_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.sch.scheduler_init_tbl.addr.width_bits = 2;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.scheduler_init_tbl.schinit), 0, 0);

  /* Force Status Message */
  Soc_petra_a_tbls.sch.force_status_message_tbl.addr.base = 0x42000000;
  Soc_petra_a_tbls.sch.force_status_message_tbl.addr.size = 0x0001;
  Soc_petra_a_tbls.sch.force_status_message_tbl.addr.width_bits = 25;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.force_status_message_tbl.message_flow_id), 15, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.sch.force_status_message_tbl.message_type), 19, 16);
}

/* Block tables initialization: MMU */
STATIC void
  soc_pa_tbls_init_MMU(void)
{

  /* DRAM Address space */
  Soc_petra_a_tbls.mmu.dram_address_space_tbl.addr.base = 0x00000000;
  Soc_petra_a_tbls.mmu.dram_address_space_tbl.addr.size = 0x4000000;
  Soc_petra_a_tbls.mmu.dram_address_space_tbl.addr.width_bits = 512;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.dram_address_space_tbl.data), 511, 0);

  /* IDF */
  Soc_petra_a_tbls.mmu.idf_tbl.addr.base = 0x50000000;
  Soc_petra_a_tbls.mmu.idf_tbl.addr.size = 0x0200;
  Soc_petra_a_tbls.mmu.idf_tbl.addr.width_bits = 28;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.idf_tbl.data), 27, 0);

  /* FDF */
  Soc_petra_a_tbls.mmu.fdf_tbl.addr.base = 0x51000000;
  Soc_petra_a_tbls.mmu.fdf_tbl.addr.size = 0x0040;
  Soc_petra_a_tbls.mmu.fdf_tbl.addr.width_bits = 6;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.fdf_tbl.data), 5, 0);

  /* RdfaWaddrStatus */
  Soc_petra_a_tbls.mmu.rdfa_waddr_status_tbl.addr.base = 0x52000000;
  Soc_petra_a_tbls.mmu.rdfa_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdfa_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfa_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfa_waddr_status_tbl.status), 10, 5);

  /* RdfbWaddrStatus */
  Soc_petra_a_tbls.mmu.rdfb_waddr_status_tbl.addr.base = 0x53000000;
  Soc_petra_a_tbls.mmu.rdfb_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdfb_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfb_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfb_waddr_status_tbl.status), 10, 5);

  /* RdfcWaddrStatus */
  Soc_petra_a_tbls.mmu.rdfc_waddr_status_tbl.addr.base = 0x54000000;
  Soc_petra_a_tbls.mmu.rdfc_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdfc_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfc_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfc_waddr_status_tbl.status), 10, 5);

  /* RdfdWaddrStatus */
  Soc_petra_a_tbls.mmu.rdfd_waddr_status_tbl.addr.base = 0x55000000;
  Soc_petra_a_tbls.mmu.rdfd_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdfd_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfd_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfd_waddr_status_tbl.status), 10, 5);

  /* RdfeWaddrStatus */
  Soc_petra_a_tbls.mmu.rdfe_waddr_status_tbl.addr.base = 0x56000000;
  Soc_petra_a_tbls.mmu.rdfe_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdfe_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfe_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdfe_waddr_status_tbl.status), 10, 5);

  /* RdffWaddrStatus */
  Soc_petra_a_tbls.mmu.rdff_waddr_status_tbl.addr.base = 0x57000000;
  Soc_petra_a_tbls.mmu.rdff_waddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rdff_waddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdff_waddr_status_tbl.waddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdff_waddr_status_tbl.status), 10, 5);

  /* RdfRaddr */
  Soc_petra_a_tbls.mmu.rdf_raddr_tbl.addr.base = 0x58000000;
  Soc_petra_a_tbls.mmu.rdf_raddr_tbl.addr.size = 0x0030;
  Soc_petra_a_tbls.mmu.rdf_raddr_tbl.addr.width_bits = 5;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rdf_raddr_tbl.raddr), 4, 0);

  /* WafHalfaWaddr */
  Soc_petra_a_tbls.mmu.waf_halfa_waddr_tbl.addr.base = 0x60000000;
  Soc_petra_a_tbls.mmu.waf_halfa_waddr_tbl.addr.size = 0x0030;
  Soc_petra_a_tbls.mmu.waf_halfa_waddr_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waf_halfa_waddr_tbl.waddr_half_a), 3, 0);

  /* WafHalfbWaddr */
  Soc_petra_a_tbls.mmu.waf_halfb_waddr_tbl.addr.base = 0x61000000;
  Soc_petra_a_tbls.mmu.waf_halfb_waddr_tbl.addr.size = 0x0030;
  Soc_petra_a_tbls.mmu.waf_halfb_waddr_tbl.addr.width_bits = 4;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waf_halfb_waddr_tbl.waddr_half_b), 3, 0);

  /* WafaHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.base = 0x62000000;
  Soc_petra_a_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafa_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafa_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WafbHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.base = 0x63000000;
  Soc_petra_a_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafb_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafb_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WafcHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.base = 0x64000000;
  Soc_petra_a_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafc_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafc_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WafdHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.base = 0x65000000;
  Soc_petra_a_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafd_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafd_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WafeHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.base = 0x66000000;
  Soc_petra_a_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafe_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafe_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WaffHalfaRaddrStatus */
  Soc_petra_a_tbls.mmu.waff_halfa_raddr_status_tbl.addr.base = 0x67000000;
  Soc_petra_a_tbls.mmu.waff_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.waff_halfa_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waff_halfa_raddr_status_tbl.raddr_half_a), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waff_halfa_raddr_status_tbl.status_half_a), 8, 4);

  /* WafaHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.base = 0x68000000;
  Soc_petra_a_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafa_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafa_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* WafbHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.base = 0x69000000;
  Soc_petra_a_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafb_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafb_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* WafcHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.base = 0x6a000000;
  Soc_petra_a_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafc_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafc_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* WafdHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.base = 0x6b000000;
  Soc_petra_a_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafd_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafd_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* WafeHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.base = 0x6c000000;
  Soc_petra_a_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafe_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.wafe_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* WaffHalfbRaddrStatus */
  Soc_petra_a_tbls.mmu.waff_halfb_raddr_status_tbl.addr.base = 0x6d000000;
  Soc_petra_a_tbls.mmu.waff_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.waff_halfb_raddr_status_tbl.addr.width_bits = 9;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waff_halfb_raddr_status_tbl.raddr_half_b), 3, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.waff_halfb_raddr_status_tbl.status_half_b), 8, 4);

  /* RafWaddr */
  Soc_petra_a_tbls.mmu.raf_waddr_tbl.addr.base = 0x70000000;
  Soc_petra_a_tbls.mmu.raf_waddr_tbl.addr.size = 0x0030;
  Soc_petra_a_tbls.mmu.raf_waddr_tbl.addr.width_bits = 5;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.raf_waddr_tbl.waddr), 4, 0);

  /* RafaRaddrStatus */
  Soc_petra_a_tbls.mmu.rafa_raddr_status_tbl.addr.base = 0x71000000;
  Soc_petra_a_tbls.mmu.rafa_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rafa_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafa_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafa_raddr_status_tbl.status), 10, 5);

  /* RafbRaddrStatus */
  Soc_petra_a_tbls.mmu.rafb_raddr_status_tbl.addr.base = 0x72000000;
  Soc_petra_a_tbls.mmu.rafb_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rafb_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafb_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafb_raddr_status_tbl.status), 10, 5);

  /* RafcRaddrStatus */
  Soc_petra_a_tbls.mmu.rafc_raddr_status_tbl.addr.base = 0x73000000;
  Soc_petra_a_tbls.mmu.rafc_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rafc_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafc_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafc_raddr_status_tbl.status), 10, 5);

  /* RafdRaddrStatus */
  Soc_petra_a_tbls.mmu.rafd_raddr_status_tbl.addr.base = 0x74000000;
  Soc_petra_a_tbls.mmu.rafd_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rafd_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafd_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafd_raddr_status_tbl.status), 10, 5);

  /* RafeRaddrStatus */
  Soc_petra_a_tbls.mmu.rafe_raddr_status_tbl.addr.base = 0x75000000;
  Soc_petra_a_tbls.mmu.rafe_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.rafe_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafe_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.rafe_raddr_status_tbl.status), 10, 5);

  /* RaffRaddrStatus */
  Soc_petra_a_tbls.mmu.raff_raddr_status_tbl.addr.base = 0x76000000;
  Soc_petra_a_tbls.mmu.raff_raddr_status_tbl.addr.size = 0x0008;
  Soc_petra_a_tbls.mmu.raff_raddr_status_tbl.addr.width_bits = 11;
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.raff_raddr_status_tbl.raddr), 4, 0);
  soc_pa_tbl_fld_set( &(Soc_petra_a_tbls.mmu.raff_raddr_status_tbl.status), 10, 5);
}

/*****************************************************
*NAME
*  soc_petra_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Soc_petra tables database.
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
  soc_pa_tbls_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TBLS_INIT);

  if (Soc_petra_a_tbls_initialized == TRUE)
  {
    goto exit;
  }

  soc_pa_tbls_init_OLP();
  soc_pa_tbls_init_IRE();
  soc_pa_tbls_init_IDR();
  soc_pa_tbls_init_IRR();
  soc_pa_tbls_init_IHP();
  soc_pa_tbls_init_IQM();
  soc_pa_tbls_init_QDR();
  soc_pa_tbls_init_IPS();
  soc_pa_tbls_init_IPT();
  soc_pa_tbls_init_DPI();
  soc_pa_tbls_init_RTP();
  soc_pa_tbls_init_EGQ();
  soc_pa_tbls_init_EPNI();
  soc_pa_tbls_init_CFC();
  soc_pa_tbls_init_SCH();
  soc_pa_tbls_init_MMU();

  Soc_petra_a_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_tbls_init",0,0);
}



/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

