/* $Id: pb_chip_tbls.c,v 1.11 Broadcom SDK $
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
* FILENAME:       soc_pb_chip_tbls.c
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
 
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
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

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
static SOC_PB_TBLS    Soc_pb_tbls;
static uint8  Soc_pb_tbls_initialized = FALSE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* soc_pb_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT SOC_PB_TBLS  **soc_pb_tbls - pointer to pb
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_pb_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_tbls_get(
    SOC_SAND_OUT SOC_PB_TBLS  **soc_pb_tbls
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TBLS_GET);

  if (Soc_pb_tbls_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TBLS_NOT_INITIALIZED, 10, exit);
  }

  *soc_pb_tbls = &Soc_pb_tbls;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_is_tbl_db_initialized()", 0, 0);
}

/*****************************************************
*NAME
* soc_pb_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
SOC_PB_TBLS*
  soc_pb_tbls(void)
{

  SOC_PB_TBLS*
    tbls = NULL;

  soc_pb_tbls_get(
    &tbls
  );

  return tbls;
}

/*****************************************************
*NAME
* soc_pb_tbl_fld_set
*TYPE:
*  PROC
*FUNCTION:
*  Sets a pb table field
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_PETRA_TBL_FIELD *field - pointer to pb table field structure
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
  soc_pb_tbl_fld_set(
    SOC_PETRA_TBL_FIELD *field,
    uint16       msb,
    uint16       lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}



/* Block tables initialization: OLP 	 */
STATIC void
  soc_pb_tbls_init_OLP(void)
{

  /* Pge Mem */
  Soc_pb_tbls.olp.pge_mem_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.olp.pge_mem_tbl.addr.size = 0x0020;
  Soc_pb_tbls.olp.pge_mem_tbl.addr.width_bits = 32;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.olp.pge_mem_tbl.pge_mem), 31, 0);
}

/* Block tables initialization: IRE 	 */
STATIC void
  soc_pb_tbls_init_IRE(void)
{

  /* Nif Ctxt Map */
  Soc_pb_tbls.ire.nif_ctxt_map_tbl.addr.base = 0x00004000;
  Soc_pb_tbls.ire.nif_ctxt_map_tbl.addr.size = 0x0400;
  Soc_pb_tbls.ire.nif_ctxt_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.nif_ctxt_map_tbl.fap_port), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.nif_ctxt_map_tbl.parity), 8, 8);

  /* Nif Port To Ctxt Bit Map */
  Soc_pb_tbls.ire.nif_port_to_ctxt_bit_map_tbl.addr.base = 0x00005000;
  Soc_pb_tbls.ire.nif_port_to_ctxt_bit_map_tbl.addr.size = 0x0010;
  Soc_pb_tbls.ire.nif_port_to_ctxt_bit_map_tbl.addr.width_bits = 81;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.nif_port_to_ctxt_bit_map_tbl.contexts_bit_mapping), 79, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.nif_port_to_ctxt_bit_map_tbl.parity), 80, 80);

  /* Rcy Ctxt Map */
  Soc_pb_tbls.ire.rcy_ctxt_map_tbl.addr.base = 0x00006000;
  Soc_pb_tbls.ire.rcy_ctxt_map_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ire.rcy_ctxt_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.rcy_ctxt_map_tbl.fap_port), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.rcy_ctxt_map_tbl.parity), 8, 8);

  /* Tdm Config */
  Soc_pb_tbls.ire.tdm_config_tbl.addr.base = 0x00007000;
  Soc_pb_tbls.ire.tdm_config_tbl.addr.size = 0x0050;
  Soc_pb_tbls.ire.tdm_config_tbl.addr.width_bits = 73;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.tdm_config_tbl.mode), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.tdm_config_tbl.cpu), 4, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.tdm_config_tbl.header), 71, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ire.tdm_config_tbl.parity), 72, 72);
}

/* Block tables initialization: IDR 	 */
STATIC void
  soc_pb_tbls_init_IDR(void)
{
  uint32
    tbl_indx;

  /* Context Mru */
  Soc_pb_tbls.idr.context_mru_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.idr.context_mru_tbl.addr.size = 0x0050;
  Soc_pb_tbls.idr.context_mru_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.context_mru_tbl.org_size), 13, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.context_mru_tbl.size), 29, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.context_mru_tbl.parity), 32, 32);

  /* Complete PC */
  Soc_pb_tbls.idr.complete_pc_tbl.addr.base = 0x00150000;
  Soc_pb_tbls.idr.complete_pc_tbl.addr.size = 0x0200;
  Soc_pb_tbls.idr.complete_pc_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.complete_pc_tbl.pcb_pointer), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.complete_pc_tbl.count), 16, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.complete_pc_tbl.ecc), 24, 20);

  /* Pcb link tbl */
  
  Soc_pb_tbls.idr.pcb_link_tbl.addr.base = 0x00100000;
  Soc_pb_tbls.idr.pcb_link_tbl.addr.size = 0x0200;
  Soc_pb_tbls.idr.pcb_link_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.pcb_link_tbl.pcb_pointer), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.pcb_link_tbl.ecc), 15, 12);


  /* Ethernet Meter Profiles */
  Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.addr.base = 0x001a0000;
  Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.addr.size = 0x0020;
  Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.addr.width_bits = 41;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.rate_mantissa), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.rate_exp), 15, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.burst_mantissa), 21, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.burst_exp), 28, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.packet_mode), 32, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_profiles_tbl.parity), 40, 40);

  /* Ethernet Meter Status */
  Soc_pb_tbls.idr.ethernet_meter_status_tbl.addr.base = 0x001b0000;
  Soc_pb_tbls.idr.ethernet_meter_status_tbl.addr.size = 0x0154;
  Soc_pb_tbls.idr.ethernet_meter_status_tbl.addr.width_bits = 73;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_status_tbl.enable), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_status_tbl.time_stamp), 14, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_status_tbl.level), 44, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_status_tbl.reserved), 68, 48);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_status_tbl.parity), 72, 72);

  /* Ethernet Meter Config */
  Soc_pb_tbls.idr.ethernet_meter_config_tbl.addr.base = 0x001c0000;
  Soc_pb_tbls.idr.ethernet_meter_config_tbl.addr.size = 0x0154;
  Soc_pb_tbls.idr.ethernet_meter_config_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_config_tbl.profile), 4, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_config_tbl.global_meter_ptr), 10, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.ethernet_meter_config_tbl.parity), 12, 12);

  /* Global Meter Profiles */
  Soc_pb_tbls.idr.global_meter_profiles_tbl.addr.base = 0x001d0000;
  Soc_pb_tbls.idr.global_meter_profiles_tbl.addr.size = 0x0008;
  Soc_pb_tbls.idr.global_meter_profiles_tbl.addr.width_bits = 37;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.rate_mantissa), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.rate_exp), 15, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.burst_mantissa), 21, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.burst_exp), 28, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.packet_mode), 32, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_profiles_tbl.enable), 36, 36);

  /* Global Meter Status */
  Soc_pb_tbls.idr.global_meter_status_tbl.addr.base = 0x001e0000;
  Soc_pb_tbls.idr.global_meter_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.idr.global_meter_status_tbl.addr.width_bits = 41;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_status_tbl.time_stamp), 10, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.global_meter_status_tbl.level), 40, 12);

  for(tbl_indx = 0; tbl_indx < 6; ++tbl_indx)
  {
    /* dbuff_pointer_cache Offset Table */
    Soc_pb_tbls.idr.dbuff_pointer_cache[tbl_indx].addr.base = 0x00180000 + (tbl_indx * 0x100);
    Soc_pb_tbls.idr.dbuff_pointer_cache[tbl_indx].addr.size = 0x000a;
    Soc_pb_tbls.idr.dbuff_pointer_cache[tbl_indx].addr.width_bits = 239;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.idr.dbuff_pointer_cache[tbl_indx].field), 238, 0);
  }

}

/* Block tables initialization: IRR */
STATIC void
  soc_pb_tbls_init_IRR(void)
{
  uint8
    tc_ndx;
  uint8
    fld_idx;

  /* Is Ingress Replication DB */
  Soc_pb_tbls.irr.is_ingress_replication_db_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.irr.is_ingress_replication_db_tbl.addr.size = 0x0800;
  Soc_pb_tbls.irr.is_ingress_replication_db_tbl.addr.width_bits = 9;
  for (fld_idx = 0 ; fld_idx < SOC_PB_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL ; fld_idx++)
  {
    /* In PA, fld_idx X matches field 8-x. In PB, fld_idx X matches field X */
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.is_ingress_replication_db_tbl.is_ingress_replication[fld_idx]),
      fld_idx,
      fld_idx);
  }

  /* Ingress Replication Multicast DB */
  Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.addr.size = 0x8000;
  Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.addr.width_bits = 84;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.base_queue1), 14, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.is_queue_number1), 15, 15);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.base_queue2), 30, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.is_queue_number2), 31, 31);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.out_lif1), 47, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.out_lif2), 63, 48);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.egress_data), 68, 64);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.ingress_replication_multicast_db_tbl.link_ptr), 86, 72);

  /* Egress Replication Multicast DB */
  Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.addr.size = 0x8000;
  Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.addr.width_bits = 84;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_lif3), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_port3), 22, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_lif2), 38, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_port2), 45, 39);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_lif1), 61, 46);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.out_port1), 68, 62);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.egress_replication_multicast_db_tbl.link_ptr), 86, 72);

  /* MCDB-Egress-Format-B */
  Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.addr.size = 0x8000;
  Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.addr.width_bits = 84;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.multicast_bitmap_ptr), 22, 11);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.out_lif2), 38, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.out_port), 45, 39);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.out_lif1), 61, 46);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.format_select), 68, 62);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_b_tbl.link_ptr), 86, 72);

  /* MCDB-Egress-Format-C */
  Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.addr.size = 0x8000;
  Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.addr.width_bits = 84;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.multicast_bitmap_ptr2), 17, 6);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.out_lif2), 33, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.multicast_bitmap_ptr1), 45, 34);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.out_lif1), 61, 46);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.format_select), 68, 62);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcdb_egress_format_c_tbl.link_ptr), 86, 72);

  /* Snoop Mirror Table0 */
  Soc_pb_tbls.irr.snoop_mirror_table0_tbl.addr.base = 0x00030000;
  Soc_pb_tbls.irr.snoop_mirror_table0_tbl.addr.size = 0x0020;
  Soc_pb_tbls.irr.snoop_mirror_table0_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.destination), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.tc), 18, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.tc_ow), 20, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.dp), 25, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.dp_ow), 28, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table0_tbl.parity), 32, 32);

  /* Snoop Mirror Table1 */
  Soc_pb_tbls.irr.snoop_mirror_table1_tbl.addr.base = 0x00040000;
  Soc_pb_tbls.irr.snoop_mirror_table1_tbl.addr.size = 0x0020;
  Soc_pb_tbls.irr.snoop_mirror_table1_tbl.addr.width_bits = 81;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.meter_ptr0), 12, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.meter_ptr0_ow), 16, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.meter_ptr1), 32, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.meter_ptr1_ow), 36, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.counter_ptr0), 51, 40);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.counter_ptr0_ow), 52, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.counter_ptr1), 67, 56);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.counter_ptr1_ow), 68, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.dp_cmd), 73, 72);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.dp_cmd_ow), 76, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.snoop_mirror_table1_tbl.parity), 80, 80);

  /* Free Pcb Memory */
  Soc_pb_tbls.irr.free_pcb_memory_tbl.addr.base = 0x00060000;
  Soc_pb_tbls.irr.free_pcb_memory_tbl.addr.size = 0x2000;
  Soc_pb_tbls.irr.free_pcb_memory_tbl.addr.width_bits = 18;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.free_pcb_memory_tbl.pcp), 12, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.free_pcb_memory_tbl.ecc), 17, 13);

  /* Pcb Link Table */
  Soc_pb_tbls.irr.pcb_link_table_tbl.addr.base = 0x00070000;
  Soc_pb_tbls.irr.pcb_link_table_tbl.addr.size = 0x2000;
  Soc_pb_tbls.irr.pcb_link_table_tbl.addr.width_bits = 18;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.pcb_link_table_tbl.pcp), 12, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.pcb_link_table_tbl.ecc), 17, 13);

  /* Is Free Pcb Memory */
  Soc_pb_tbls.irr.is_free_pcb_memory_tbl.addr.base = 0x00090000;
  Soc_pb_tbls.irr.is_free_pcb_memory_tbl.addr.size = 0x0400;
  Soc_pb_tbls.irr.is_free_pcb_memory_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.is_free_pcb_memory_tbl.is_pcp), 9, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.is_free_pcb_memory_tbl.ecc), 13, 10);

  /* Is Pcb Link Table */
  Soc_pb_tbls.irr.is_pcb_link_table_tbl.addr.base = 0x000a0000;
  Soc_pb_tbls.irr.is_pcb_link_table_tbl.addr.size = 0x0400;
  Soc_pb_tbls.irr.is_pcb_link_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.is_pcb_link_table_tbl.is_pcp), 9, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.is_pcb_link_table_tbl.ecc), 13, 10);

  /* Rpf Memory */
  Soc_pb_tbls.irr.rpf_memory_tbl.addr.base = 0x00110000;
  Soc_pb_tbls.irr.rpf_memory_tbl.addr.size = 0x1000;
  Soc_pb_tbls.irr.rpf_memory_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.rpf_memory_tbl.pointer), 11, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.rpf_memory_tbl.ecc), 16, 12);

  /* Mcr Memory */
  Soc_pb_tbls.irr.mcr_memory_tbl.addr.base = 0x00180000;
  Soc_pb_tbls.irr.mcr_memory_tbl.addr.size = 0x0300;
  Soc_pb_tbls.irr.mcr_memory_tbl.addr.width_bits = 83;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcr_memory_tbl.descriptor), 72, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.mcr_memory_tbl.ecc), 82, 76);

  /* Isf Memory */
  Soc_pb_tbls.irr.isf_memory_tbl.addr.base = 0x001b0000;
  Soc_pb_tbls.irr.isf_memory_tbl.addr.size = 0x0080;
  Soc_pb_tbls.irr.isf_memory_tbl.addr.width_bits = 58;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.isf_memory_tbl.descriptor), 51, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.isf_memory_tbl.ecc), 57, 52);

  /* Destination Table */
  Soc_pb_tbls.irr.destination_table_tbl.addr.base = 0x001c0000;
  Soc_pb_tbls.irr.destination_table_tbl.addr.size = 0x1000;
  Soc_pb_tbls.irr.destination_table_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.destination_table_tbl.queue_number), 14, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.destination_table_tbl.queue_valid), 16, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.destination_table_tbl.tc_profile), 20, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.destination_table_tbl.parity), 24, 24);

  /* Lag To Lag Range */
  Soc_pb_tbls.irr.lag_to_lag_range_tbl.addr.base = 0x00200000;
  Soc_pb_tbls.irr.lag_to_lag_range_tbl.addr.size = 0x0100;
  Soc_pb_tbls.irr.lag_to_lag_range_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_to_lag_range_tbl.range), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_to_lag_range_tbl.mode), 4, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_to_lag_range_tbl.parity), 8, 8);

  /* Lag Mapping */
  Soc_pb_tbls.irr.lag_mapping_tbl.addr.base = 0x00220000;
  Soc_pb_tbls.irr.lag_mapping_tbl.addr.size = 0x1000;
  Soc_pb_tbls.irr.lag_mapping_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_mapping_tbl.destination), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_mapping_tbl.parity), 16, 16);

  /* Lag Next Member */
  Soc_pb_tbls.irr.lag_next_member_tbl.addr.base = 0x00230000;
  Soc_pb_tbls.irr.lag_next_member_tbl.addr.size = 0x0100;
  Soc_pb_tbls.irr.lag_next_member_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_next_member_tbl.offset), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.lag_next_member_tbl.parity), 4, 4);

  /* Rsq Fifo tbl */
  Soc_pb_tbls.irr.rsq_fifo_tbl.addr.base = 0x00100000;
  Soc_pb_tbls.irr.rsq_fifo_tbl.addr.size = 0x01000;
  Soc_pb_tbls.irr.rsq_fifo_tbl.addr.width_bits = 107;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.rsq_fifo_tbl.descriptor), 84, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.rsq_fifo_tbl.next_ptr), 99 , 88);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.rsq_fifo_tbl.ecc), 106 , 100);

  /* Smooth Division */
  Soc_pb_tbls.irr.smooth_division_tbl.addr.base = 0x00240000;
  Soc_pb_tbls.irr.smooth_division_tbl.addr.size = 0x1000;
  Soc_pb_tbls.irr.smooth_division_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.smooth_division_tbl.member), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.smooth_division_tbl.parity), 4, 4);

  /* Traffic Class Mapping */
  Soc_pb_tbls.irr.traffic_class_mapping_tbl.addr.base = 0x00250000;
  Soc_pb_tbls.irr.traffic_class_mapping_tbl.addr.size = 0x0008;
  Soc_pb_tbls.irr.traffic_class_mapping_tbl.addr.width_bits = 24;
  for (tc_ndx = 0; tc_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++tc_ndx)
  {
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.irr.traffic_class_mapping_tbl.traffic_class_mapping[tc_ndx]), 3*tc_ndx+2, 3*tc_ndx);
  }
}

/* Block tables initialization: IHP */
STATIC void
  soc_pb_tbls_init_IHP(void)
{

  /* Port Mine Table Lag Port */
  Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.addr.base = 0x01310000;
  Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.lag_port_mine0), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.lag_port_mine1), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.lag_port_mine2), 2, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.port_mine_table_lag_port_tbl.lag_port_mine3), 3, 3);

  /* Tm Port Pp Port Config */
  Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.addr.size = 0x0050;
  Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.addr.width_bits = 21;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.pp_port_offset1), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.pp_port_offset2), 13, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.pp_port_profile), 17, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_pp_port_config_tbl.pp_port_use_offset_directly), 20, 20);

  /* Tm Port Sys Port Config */
  Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.addr.size = 0x0050;
  Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.addr.width_bits = 38;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.system_port_offset1), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.system_port_offset2), 13, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.system_port_value), 28, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.system_port_profile), 33, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.tm_port_sys_port_config_tbl.system_port_value_to_use), 37, 36);

  /* Pp Port Values */
  Soc_pb_tbls.ihp.pp_port_values_tbl.addr.base = 0x00040000;
  Soc_pb_tbls.ihp.pp_port_values_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihp.pp_port_values_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihp.pp_port_values_tbl.pp_port_system_port_value), 12, 0);


  /* SOC_PB_IHP_EM_PLDT_H0 */

  Soc_pb_tbls.ihp.isem_keyt_h0_tbl.addr.base = 0x00500000;
  Soc_pb_tbls.ihp.isem_keyt_h0_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_keyt_h0_tbl.addr.width_bits = 21;


  /* SOC_PB_IHP_EM_PLDT_H1 */

  Soc_pb_tbls.ihp.isem_keyt_h1_tbl.addr.base = 0x00510000;
  Soc_pb_tbls.ihp.isem_keyt_h1_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_keyt_h1_tbl.addr.width_bits = 21;


  /* SOC_PB_IHP_EM_PLDT_AUX */

  Soc_pb_tbls.ihp.isem_keyt_aux_tbl.addr.base = 0x00520000;
  Soc_pb_tbls.ihp.isem_keyt_aux_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_keyt_aux_tbl.addr.width_bits = 21;


  /* SOC_PB_IHP_ISEM_PLDT_H0_TBL */

  Soc_pb_tbls.ihp.isem_pldt_h0_tbl.addr.base = 0x00530000;
  Soc_pb_tbls.ihp.isem_pldt_h0_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_pldt_h0_tbl.addr.width_bits = 16;

  /*  SOC_PB_IHP_ISEM_PLDT_H1_TBL */

  Soc_pb_tbls.ihp.isem_pldt_h1_tbl.addr.base = 0x00540000;
  Soc_pb_tbls.ihp.isem_pldt_h1_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_pldt_h1_tbl.addr.width_bits = 16;

  /* SOC_PB_IHP_ISEM_PLDT_AUX_TBL */

  Soc_pb_tbls.ihp.isem_pldt_aux_tbl.addr.base = 0x00550000;
  Soc_pb_tbls.ihp.isem_pldt_aux_tbl.addr.size = 0x04000;
  Soc_pb_tbls.ihp.isem_pldt_aux_tbl.addr.width_bits = 16;

  /* SOC_PB_IHP_ISEM_MGMT_MEM_A_TBL */

  Soc_pb_tbls.ihp.isem_mgmt_mem_a_tbl.addr.base = 0x00560000;
  Soc_pb_tbls.ihp.isem_mgmt_mem_a_tbl.addr.size = 0x020;
  Soc_pb_tbls.ihp.isem_mgmt_mem_a_tbl.addr.width_bits = 50;

  /* SOC_PB_IHP_ISEM_MGMT_MEM_B_TBL */

  Soc_pb_tbls.ihp.isem_mgmt_mem_b_tbl.addr.base = 0x00570000;
  Soc_pb_tbls.ihp.isem_mgmt_mem_b_tbl.addr.size = 0x020;
  Soc_pb_tbls.ihp.isem_mgmt_mem_b_tbl.addr.width_bits = 50;


  /* SOC_PB_IHB_EM_KEYT_H0_TBL */

  Soc_pb_tbls.ihp.isem_em_keyt_h0_tbl.addr.base = 0x00600000;
  Soc_pb_tbls.ihp.isem_em_keyt_h0_tbl.addr.size = 0x08000;
  Soc_pb_tbls.ihp.isem_em_keyt_h0_tbl.addr.width_bits = 49;

  /*SOC_PB_IHP_EM_KEYT_H1_TBL*/

  Soc_pb_tbls.ihp.isem_em_keyt_h1_tbl.addr.base = 0x00610000;
  Soc_pb_tbls.ihp.isem_em_keyt_h1_tbl.addr.size = 0x08000;
  Soc_pb_tbls.ihp.isem_em_keyt_h1_tbl.addr.width_bits = 49;

  /*SOC_PB_IHP_EM_KEYT_AUX_TBL*/

  Soc_pb_tbls.ihp.isem_em_keyt_aux_tbl.addr.base = 0x00620000;
  Soc_pb_tbls.ihp.isem_em_keyt_aux_tbl.addr.size = 0x08000;
  Soc_pb_tbls.ihp.isem_em_keyt_aux_tbl.addr.width_bits = 49;

  /*SOC_PB_IHP_EM_PLDT_H0*/

  Soc_pb_tbls.ihp.em_pldt_h0_tbl.addr.base = 0x00630000;
  Soc_pb_tbls.ihp.em_pldt_h0_tbl.addr.size = 0x08000;
  Soc_pb_tbls.ihp.em_pldt_h0_tbl.addr.width_bits = 41;
}

/* Block tables initialization: QDR 	 */
STATIC void
  soc_pb_tbls_init_QDR(void)
{

  /* Qdr Mem */
  Soc_pb_tbls.qdr.qdr_mem.addr.base = 0x10000000;
  Soc_pb_tbls.qdr.qdr_mem.addr.size = 0x400000;
  Soc_pb_tbls.qdr.qdr_mem.addr.width_bits = 32;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.qdr.qdr_mem.data), 31, 0);

  /* Qdr Dll Mem */
  Soc_pb_tbls.qdr.qdr_dll_mem.addr.base = 0x20000000;
  Soc_pb_tbls.qdr.qdr_dll_mem.addr.size = 0x0080;
  Soc_pb_tbls.qdr.qdr_dll_mem.addr.width_bits = 28;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.qdr.qdr_dll_mem.qdr_dll_mem), 27, 0);
}

/* Block tables initialization: MMU */
STATIC void
  soc_pb_tbls_init_MMU(void)
{

  /* DRAM Address space */
  Soc_pb_tbls.mmu.dram_address_space_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.mmu.dram_address_space_tbl.addr.size = 0x4000000;
  Soc_pb_tbls.mmu.dram_address_space_tbl.addr.width_bits = 512;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.dram_address_space_tbl.data), 511, 0);

  /* IDF */
  Soc_pb_tbls.mmu.idf_tbl.addr.base = 0x04000000;
  Soc_pb_tbls.mmu.idf_tbl.addr.size = 0x0800;
  Soc_pb_tbls.mmu.idf_tbl.addr.width_bits = 28;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.idf_tbl.data), 27, 0);

  /* FDF */
  Soc_pb_tbls.mmu.fdf_tbl.addr.base = 0x08000000;
  Soc_pb_tbls.mmu.fdf_tbl.addr.size = 0x0040;
  Soc_pb_tbls.mmu.fdf_tbl.addr.width_bits = 6;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.fdf_tbl.data), 5, 0);

  /* Waf Halfa Waddr */
  Soc_pb_tbls.mmu.waf_halfa_waddr_tbl.addr.base = 0x0c000000;
  Soc_pb_tbls.mmu.waf_halfa_waddr_tbl.addr.size = 0x0030;
  Soc_pb_tbls.mmu.waf_halfa_waddr_tbl.addr.width_bits = 6;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waf_halfa_waddr_tbl.waddr_half_a), 5, 0);

  /* Waf Halfb Waddr */
  Soc_pb_tbls.mmu.waf_halfb_waddr_tbl.addr.base = 0x10000000;
  Soc_pb_tbls.mmu.waf_halfb_waddr_tbl.addr.size = 0x0030;
  Soc_pb_tbls.mmu.waf_halfb_waddr_tbl.addr.width_bits = 6;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waf_halfb_waddr_tbl.waddr_half_b), 5, 0);

  /* Wafa Halfa Raddr Status */
  Soc_pb_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.base = 0x14000000;
  Soc_pb_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafa_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafa_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafa_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Wafb Halfa Raddr Status */
  Soc_pb_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.base = 0x18000000;
  Soc_pb_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafb_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafb_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafb_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Wafc Halfa Raddr Status */
  Soc_pb_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.base = 0x1c000000;
  Soc_pb_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafc_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafc_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafc_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Wafd Halfa Raddr Status */
  Soc_pb_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.base = 0x20000000;
  Soc_pb_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafd_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafd_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafd_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Wafe Halfa Raddr Status */
  Soc_pb_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.base = 0x24000000;
  Soc_pb_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafe_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafe_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafe_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Waff Halfa Raddr Status */
  Soc_pb_tbls.mmu.waff_halfa_raddr_status_tbl.addr.base = 0x28000000;
  Soc_pb_tbls.mmu.waff_halfa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.waff_halfa_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waff_halfa_raddr_status_tbl.raddr_half_a), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waff_halfa_raddr_status_tbl.status_half_a), 12, 6);

  /* Wafa Halfb Raddr Status */
  Soc_pb_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.base = 0x2c000000;
  Soc_pb_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafa_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafa_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafa_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Wafb Halfb Raddr Status */
  Soc_pb_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.base = 0x30000000;
  Soc_pb_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafb_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafb_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafb_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Wafc Halfb Raddr Status */
  Soc_pb_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.base = 0x34000000;
  Soc_pb_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafc_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafc_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafc_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Wafd Halfb Raddr Status */
  Soc_pb_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.base = 0x38000000;
  Soc_pb_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafd_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafd_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafd_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Wafe Halfb Raddr Status */
  Soc_pb_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.base = 0x3c000000;
  Soc_pb_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.wafe_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafe_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.wafe_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Waff Halfb Raddr Status */
  Soc_pb_tbls.mmu.waff_halfb_raddr_status_tbl.addr.base = 0x40000000;
  Soc_pb_tbls.mmu.waff_halfb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.waff_halfb_raddr_status_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waff_halfb_raddr_status_tbl.raddr_half_b), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.waff_halfb_raddr_status_tbl.status_half_b), 12, 6);

  /* Raf Waddr */
  Soc_pb_tbls.mmu.raf_waddr_tbl.addr.base = 0x44000000;
  Soc_pb_tbls.mmu.raf_waddr_tbl.addr.size = 0x0030;
  Soc_pb_tbls.mmu.raf_waddr_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.raf_waddr_tbl.waddr), 6, 0);

  /* Rafa Raddr Status */
  Soc_pb_tbls.mmu.rafa_raddr_status_tbl.addr.base = 0x48000000;
  Soc_pb_tbls.mmu.rafa_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rafa_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafa_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafa_raddr_status_tbl.status), 14, 7);

  /* Rafb Raddr Status */
  Soc_pb_tbls.mmu.rafb_raddr_status_tbl.addr.base = 0x4c000000;
  Soc_pb_tbls.mmu.rafb_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rafb_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafb_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafb_raddr_status_tbl.status), 14, 7);

  /* Rafc Raddr Status */
  Soc_pb_tbls.mmu.rafc_raddr_status_tbl.addr.base = 0x50000000;
  Soc_pb_tbls.mmu.rafc_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rafc_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafc_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafc_raddr_status_tbl.status), 14, 7);

  /* Rafd Raddr Status */
  Soc_pb_tbls.mmu.rafd_raddr_status_tbl.addr.base = 0x54000000;
  Soc_pb_tbls.mmu.rafd_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rafd_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafd_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafd_raddr_status_tbl.status), 14, 7);

  /* Rafe Raddr Status */
  Soc_pb_tbls.mmu.rafe_raddr_status_tbl.addr.base = 0x58000000;
  Soc_pb_tbls.mmu.rafe_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rafe_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafe_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rafe_raddr_status_tbl.status), 14, 7);

  /* Raff Raddr Status */
  Soc_pb_tbls.mmu.raff_raddr_status_tbl.addr.base = 0x5c000000;
  Soc_pb_tbls.mmu.raff_raddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.raff_raddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.raff_raddr_status_tbl.raddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.raff_raddr_status_tbl.status), 14, 7);

  /* Rdfa Waddr Status */
  Soc_pb_tbls.mmu.rdfa_waddr_status_tbl.addr.base = 0x60000000;
  Soc_pb_tbls.mmu.rdfa_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdfa_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfa_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfa_waddr_status_tbl.status), 14, 7);

  /* Rdfb Waddr Status */
  Soc_pb_tbls.mmu.rdfb_waddr_status_tbl.addr.base = 0x64000000;
  Soc_pb_tbls.mmu.rdfb_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdfb_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfb_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfb_waddr_status_tbl.status), 14, 7);

  /* Rdfc Waddr Status */
  Soc_pb_tbls.mmu.rdfc_waddr_status_tbl.addr.base = 0x68000000;
  Soc_pb_tbls.mmu.rdfc_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdfc_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfc_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfc_waddr_status_tbl.status), 14, 7);

  /* Rdfd Waddr Status */
  Soc_pb_tbls.mmu.rdfd_waddr_status_tbl.addr.base = 0x6c000000;
  Soc_pb_tbls.mmu.rdfd_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdfd_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfd_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfd_waddr_status_tbl.status), 14, 7);

  /* Rdfe Waddr Status */
  Soc_pb_tbls.mmu.rdfe_waddr_status_tbl.addr.base = 0x70000000;
  Soc_pb_tbls.mmu.rdfe_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdfe_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfe_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdfe_waddr_status_tbl.status), 14, 7);

  /* Rdff Waddr Status */
  Soc_pb_tbls.mmu.rdff_waddr_status_tbl.addr.base = 0x74000000;
  Soc_pb_tbls.mmu.rdff_waddr_status_tbl.addr.size = 0x0008;
  Soc_pb_tbls.mmu.rdff_waddr_status_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdff_waddr_status_tbl.waddr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdff_waddr_status_tbl.status), 14, 7);

  /* Rdf Raddr */
  Soc_pb_tbls.mmu.rdf_raddr_tbl.addr.base = 0x78000000;
  Soc_pb_tbls.mmu.rdf_raddr_tbl.addr.size = 0x0030;
  Soc_pb_tbls.mmu.rdf_raddr_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.mmu.rdf_raddr_tbl.raddr), 6, 0);
}

/* Block tables initialization: IQM */
STATIC void
  soc_pb_tbls_init_IQM(void)
{
  uint32
    fld_idx;

  /* BDB Link List */
  Soc_pb_tbls.iqm.bdb_link_list_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.iqm.bdb_link_list_tbl.addr.size = 0x10000;
  Soc_pb_tbls.iqm.bdb_link_list_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.bdb_link_list_tbl.bdb_link_list), 15, 0);

  /* Packet Queue Descriptor (Dynamic ) */
  Soc_pb_tbls.iqm.dynamic_tbl.addr.base = 0x00100000;
  Soc_pb_tbls.iqm.dynamic_tbl.addr.size = 0x8000;
  Soc_pb_tbls.iqm.dynamic_tbl.addr.width_bits = 59;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.dynamic_tbl.pq_head_ptr), 21, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.dynamic_tbl.que_not_empty), 22, 22);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.dynamic_tbl.pq_inst_que_size), 46, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.dynamic_tbl.pq_avrg_szie), 58, 47);

  /* Packet Queue Descriptor (Static) */
  Soc_pb_tbls.iqm.static_tbl.addr.base = 0x00200000;
  Soc_pb_tbls.iqm.static_tbl.addr.size = 0x8000;
  Soc_pb_tbls.iqm.static_tbl.addr.width_bits = 20;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.static_tbl.credit_class), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.static_tbl.rate_class), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.static_tbl.connection_class), 14, 10);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.static_tbl.traffic_class), 17, 15);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.static_tbl.que_signature), 19, 18);

  /* Packet Queue Tail Pointer */
  Soc_pb_tbls.iqm.packet_queue_tail_pointer_tbl.addr.base = 0x00300000;
  Soc_pb_tbls.iqm.packet_queue_tail_pointer_tbl.addr.size = 0x8000;
  Soc_pb_tbls.iqm.packet_queue_tail_pointer_tbl.addr.width_bits = 22;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_tail_pointer_tbl.tail_ptr), 21, 0);

  /* Packet Queue Red Weight table */
  Soc_pb_tbls.iqm.packet_queue_red_weight_table_tbl.addr.base = 0x00400000;
  Soc_pb_tbls.iqm.packet_queue_red_weight_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.packet_queue_red_weight_table_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_weight_table_tbl.pq_weight), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_weight_table_tbl.avrg_en), 6, 6);

  /* Credit Discount table */
  Soc_pb_tbls.iqm.credit_discount_table_tbl.addr.base = 0x00500000;
  Soc_pb_tbls.iqm.credit_discount_table_tbl.addr.size = 0x0010;
  Soc_pb_tbls.iqm.credit_discount_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.credit_discount_table_tbl.crdt_disc_val), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.credit_discount_table_tbl.discnt_sign), 7, 7);

  /* Full User Count Memory */
  Soc_pb_tbls.iqm.full_user_count_memory_tbl.addr.base = 0x00600000;
  Soc_pb_tbls.iqm.full_user_count_memory_tbl.addr.size = 0x8000;
  Soc_pb_tbls.iqm.full_user_count_memory_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.full_user_count_memory_tbl.flus_cnt), 11, 0);

  /* Mini-Multicast User Count Memory */
  Soc_pb_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.base = 0x00700000;
  Soc_pb_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.size = 0x0800;
  Soc_pb_tbls.iqm.mini_multicast_user_count_memory_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.mini_multicast_user_count_memory_tbl.mn_us_cnt), 63, 0);

  /* Packet Queue Red parameters table */
  Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.base = 0x00800000;
  Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.addr.width_bits = 80;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_mnt), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_exp), 10, 6);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_wred_en), 11, 11);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c2), 43, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c3), 47, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_c1), 52, 48);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_avrg_max_th), 64, 53);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_avrg_min_th), 76, 65);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.pq_wred_pckt_sz_ignr), 77, 77);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_queue_red_parameters_table_tbl.addmit_logic), 79, 78);

  /* Packet Descriptor Fifos Memory */
  Soc_pb_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.base = 0x00900000;
  Soc_pb_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.packet_descriptor_fifos_memory_tbl.addr.width_bits = 30;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.packet_descriptor_fifos_memory_tbl.tx_pd), 29, 0);

  /* Tx Descriptor Fifos Memory */
  Soc_pb_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.base = 0x00a00000;
  Soc_pb_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.tx_descriptor_fifos_memory_tbl.addr.width_bits = 85;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.tx_descriptor_fifos_memory_tbl.tx_dscr), 84, 0);

  /* VSQ Descriptor Rate Class - group A */
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base = 0x01100000;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size = 0x0004;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_a_tbl.vsq_rc_a), 3, 0);

  /* VSQ Descriptor Rate Class - group B */
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base = 0x01200000;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size = 0x0020;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_b_tbl.vsq_rc_b), 3, 0);

  /* VSQ Descriptor Rate Class - group C */
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base = 0x01300000;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_c_tbl.vsq_rc_c), 3, 0);

  /* VSQ Descriptor Rate Class - group D */
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base = 0x01400000;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_descriptor_rate_class_group_d_tbl.vsq_rc_d), 3, 0);

  /* VSQ Qsize memory - group A */
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.base = 0x01500000;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.size = 0x0004;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_a_tbl.addr.width_bits = 50;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_a_tbl.vsq_size_wrds), 27, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_a_tbl.vsq_size_bds), 49, 28);

  /* VSQ Qsize memory - group B */
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.base = 0x01600000;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.size = 0x0020;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_b_tbl.addr.width_bits = 50;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_b_tbl.vsq_size_wrds), 27, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_b_tbl.vsq_size_bds), 49, 28);

  /* VSQ Qsize memory - group C */
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.base = 0x01700000;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_c_tbl.addr.width_bits = 50;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_c_tbl.vsq_size_wrds), 27, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_c_tbl.vsq_size_bds), 49, 28);

  /* VSQ Qsize memory - group D */
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.base = 0x01800000;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.vsq_qsize_memory_group_d_tbl.addr.width_bits = 50;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_d_tbl.vsq_size_wrds), 27, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_qsize_memory_group_d_tbl.vsq_size_bds), 49, 28);

  /* VSQ Average Qsize memory - group A */
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.base = 0x01900000;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.size = 0x0004;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_a_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group B */
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.base = 0x01a00000;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.size = 0x0020;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_b_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group C */
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.base = 0x01b00000;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_c_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Average Qsize memory - group D */
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.base = 0x01c00000;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_average_qsize_memory_group_d_tbl.vsq_avrg_size), 11, 0);

  /* VSQ Flow-Control Parameters table - group A */
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.base = 0x01d00000;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.size = 0x0010;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.width_bits = 46;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].wred_en), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].avrg_size_en), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].red_weight_q), 7, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_words_mnt), 12, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_words_exp), 17, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_words_mnt), 22, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_words_exp), 27, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_bd_mnt), 31, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].set_threshold_bd_exp), 36, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_bd_mnt), 40, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[0].clear_threshold_bd_exp), 45, 41);


  /* VSQ Flow-Control Parameters table - group B */
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.base = 0x01e00000;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.size = 0x0010;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].addr.width_bits = 46;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].wred_en), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].avrg_size_en), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].red_weight_q), 7, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_words_mnt), 12, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_words_exp), 17, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_words_mnt), 22, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_words_exp), 27, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_bd_mnt), 31, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].set_threshold_bd_exp), 36, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_bd_mnt), 40, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[1].clear_threshold_bd_exp), 45, 41);

  /* VSQ Flow-Control Parameters table - group C */
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.base = 0x01f00000;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.size = 0x0010;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].addr.width_bits = 46;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].wred_en), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].avrg_size_en), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].red_weight_q), 7, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_words_mnt), 12, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_words_exp), 17, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_words_mnt), 22, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_words_exp), 27, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_bd_mnt), 31, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].set_threshold_bd_exp), 36, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_bd_mnt), 40, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[2].clear_threshold_bd_exp), 45, 41);

  /* VSQ Flow-Control Parameters table - group D */
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.base = 0x02000000;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.size = 0x0010;
  Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].addr.width_bits = 46;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].wred_en), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].avrg_size_en), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].red_weight_q), 7, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_words_mnt), 12, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_words_exp), 17, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_words_mnt), 22, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_words_exp), 27, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_bd_mnt), 31, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].set_threshold_bd_exp), 36, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_bd_mnt), 40, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_flow_control_parameters_table_group_tbl[3].clear_threshold_bd_exp), 45, 41);

  /* VSQ Queue Parameters table - group A */
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.base = 0x02100000;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].addr.width_bits = 89;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c2), 31, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c3), 35, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].c1), 40, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].max_avrg_th), 52, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].min_avrg_th), 64, 53);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_szie_bds_mnt), 72, 66);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_szie_bds_exp), 76, 73);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_size_words_mnt), 83, 77);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[0].vq_max_size_words_exp), 88, 84);


  /* VSQ Queue Parameters table - group B */
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.base = 0x02200000;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].addr.width_bits = 89;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c2), 31, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c3), 35, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].c1), 40, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].max_avrg_th), 52, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].min_avrg_th), 64, 53);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_szie_bds_mnt), 72, 66);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_szie_bds_exp), 76, 73);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_size_words_mnt), 83, 77);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[1].vq_max_size_words_exp), 88, 84);

  /* VSQ Queue Parameters table - group C */
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.base = 0x02300000;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].addr.width_bits = 89;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c2), 31, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c3), 35, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].c1), 40, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].max_avrg_th), 52, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].min_avrg_th), 64, 53);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_szie_bds_mnt), 72, 66);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_szie_bds_exp), 76, 73);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_size_words_mnt), 83, 77);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[2].vq_max_size_words_exp), 88, 84);

  /* VSQ Queue Parameters table - group D */
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.base = 0x02400000;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.size = 0x0040;
  Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].addr.width_bits = 89;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c2), 31, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c3), 35, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].c1), 40, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].max_avrg_th), 52, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].min_avrg_th), 64, 53);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_wred_pckt_sz_ignr), 65, 65);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_szie_bds_mnt), 72, 66);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_szie_bds_exp), 76, 73);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_size_words_mnt), 83, 77);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.vsq_queue_parameters_table_group_tbl[3].vq_max_size_words_exp), 88, 84);

  /* System Red parameters table */
  Soc_pb_tbls.iqm.system_red_parameters_table_tbl.addr.base = 0x02500000;
  Soc_pb_tbls.iqm.system_red_parameters_table_tbl.addr.size = 0x0100;
  Soc_pb_tbls.iqm.system_red_parameters_table_tbl.addr.width_bits = 21;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.adm_th), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.prob_th), 7, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.drp_th), 11, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.drp_prob_indx1), 15, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.drp_prob_indx2), 19, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_parameters_table_tbl.sys_red_en), 20, 20);

  /* System Red drop probability values */
  Soc_pb_tbls.iqm.system_red_drop_probability_values_tbl.addr.base = 0x02600000;
  Soc_pb_tbls.iqm.system_red_drop_probability_values_tbl.addr.size = 0x0010;
  Soc_pb_tbls.iqm.system_red_drop_probability_values_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_drop_probability_values_tbl.drp_prob), 15, 0);

  /* Source Qsize range thresholds (System Red) */
  Soc_pb_tbls.iqm.system_red_tbl.addr.base = 0x02700000;
  Soc_pb_tbls.iqm.system_red_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.system_red_tbl.addr.width_bits = 180;
  for (fld_idx = 0; fld_idx < SOC_PETRA_SYS_RED_NOF_Q_RNGS; ++fld_idx)
  {
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.system_red_tbl.qsz_rng_th[fld_idx]),
      (uint8)(12*(fld_idx+1)-1),
      (uint8)(12*(fld_idx))
      );

  /* Congestion Point (CNM) descriptor - Static */
  Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.addr.base = 0x02800000;
  Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.addr.size = 0x1000;
  Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.cp_enable), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.cp_class), 3, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_descriptor_static_tbl.cp_id), 15, 4);

  /* Congestion Point (CNM) descriptor - Dynamic */
  Soc_pb_tbls.iqm.cnm_descriptor_dynamic_tbl.addr.base = 0x02900000;
  Soc_pb_tbls.iqm.cnm_descriptor_dynamic_tbl.addr.size = 0x1000;
  Soc_pb_tbls.iqm.cnm_descriptor_dynamic_tbl.addr.width_bits = 32;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_descriptor_dynamic_tbl.cp_enqued1), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_descriptor_dynamic_tbl.cp_qsize_old), 31, 16);

  /* Congestion Point (CNM) parameters table */
  Soc_pb_tbls.iqm.cnm_parameters_table_tbl.addr.base = 0x02a00000;
  Soc_pb_tbls.iqm.cnm_parameters_table_tbl.addr.size = 0x0008;
  Soc_pb_tbls.iqm.cnm_parameters_table_tbl.addr.width_bits = 140;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_qeq), 11, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_w), 16, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_fb_max_val), 30, 17  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_quant_div), 34, 31  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[0]), 47, 35  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[1]), 60, 48  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[2]), 73, 61  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[3]), 86, 74  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[4]), 99, 87  );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[5]), 112, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[6]), 125, 113);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_sample_base[7]), 138, 126);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnm_parameters_table_tbl.cp_fixed_sample_base), 139, 139);

  /* Meter-processor result resolve table - Static */
  Soc_pb_tbls.iqm.meter_processor_result_resolve_table_static_tbl.addr.base = 0x03000000;
  Soc_pb_tbls.iqm.meter_processor_result_resolve_table_static_tbl.addr.size = 0x0040;
  Soc_pb_tbls.iqm.meter_processor_result_resolve_table_static_tbl.addr.width_bits = 3;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.meter_processor_result_resolve_table_static_tbl.iqm_dp), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.meter_processor_result_resolve_table_static_tbl.etm_de), 2, 2);

  /* PRFSEL 0 */
  Soc_pb_tbls.iqm.prfsel_tbl[0].addr.base = 0x03100000;
  Soc_pb_tbls.iqm.prfsel_tbl[0].addr.size = 0x2000;
  Soc_pb_tbls.iqm.prfsel_tbl[0].addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfsel_tbl[0].profile), 8, 0);

  /* PRFSEL 1 */
  Soc_pb_tbls.iqm.prfsel_tbl[1].addr.base = 0x03200000;
  Soc_pb_tbls.iqm.prfsel_tbl[1].addr.size = 0x2000;
  Soc_pb_tbls.iqm.prfsel_tbl[1].addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfsel_tbl[1].profile), 8, 0);

  /* PRFCFG 0 */
  Soc_pb_tbls.iqm.prfcfg_tbl[0].addr.base = 0x03300000;
  Soc_pb_tbls.iqm.prfcfg_tbl[0].addr.size = 0x0200;
  Soc_pb_tbls.iqm.prfcfg_tbl[0].addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].color_aware), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].coupling_flag), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].fairness_mode), 2, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].cir_mantissa_64), 8, 3);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].cir_reverse_exponent), 13, 9);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].reset_cir), 14, 14);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].cbs_mantissa_64), 20, 15);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].cbs_exponent), 24, 21);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].eir_mantissa_64), 30, 25);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].eir_reverse_exponent), 35, 31);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].reset_eir), 36, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].ebs_mantissa_64), 42, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[0].ebs_exponent), 46, 43);

  /* PRFCFG 1 */
  Soc_pb_tbls.iqm.prfcfg_tbl[1].addr.base = 0x03400000;
  Soc_pb_tbls.iqm.prfcfg_tbl[1].addr.size = 0x0200;
  Soc_pb_tbls.iqm.prfcfg_tbl[1].addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].color_aware), 0, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].coupling_flag), 1, 1);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].fairness_mode), 2, 2);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].cir_mantissa_64), 8, 3);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].cir_reverse_exponent), 13, 9);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].reset_cir), 14, 14);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].cbs_mantissa_64), 20, 15);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].cbs_exponent), 24, 21);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].eir_mantissa_64), 30, 25);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].eir_reverse_exponent), 35, 31);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].reset_eir), 36, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].ebs_mantissa_64), 42, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.prfcfg_tbl[1].ebs_exponent), 46, 43);

  /* Normal Dynamica */
  Soc_pb_tbls.iqm.normal_dynamica_tbl.addr.base = 0x03700000;
  Soc_pb_tbls.iqm.normal_dynamica_tbl.addr.size = 0x2000;
  Soc_pb_tbls.iqm.normal_dynamica_tbl.addr.width_bits = 62;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamica_tbl.time_stamp), 19, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamica_tbl.cbl), 40, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamica_tbl.ebl), 61, 41);

  /* Normal Dynamicb */
  Soc_pb_tbls.iqm.normal_dynamicb_tbl.addr.base = 0x03800000;
  Soc_pb_tbls.iqm.normal_dynamicb_tbl.addr.size = 0x2000;
  Soc_pb_tbls.iqm.normal_dynamicb_tbl.addr.width_bits = 62;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamicb_tbl.time_stamp), 19, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamicb_tbl.cbl), 40, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.normal_dynamicb_tbl.ebl), 61, 41);

  /* High Dynamica */
  Soc_pb_tbls.iqm.high_dynamica_tbl.addr.base = 0x03900000;
  Soc_pb_tbls.iqm.high_dynamica_tbl.addr.size = 0x0010;
  Soc_pb_tbls.iqm.high_dynamica_tbl.addr.width_bits = 52;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamica_tbl.time_stamp), 7, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamica_tbl.cbl), 29, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamica_tbl.ebl), 51, 30);

  /* High Dynamicb */
  Soc_pb_tbls.iqm.high_dynamicb_tbl.addr.base = 0x03a00000;
  Soc_pb_tbls.iqm.high_dynamicb_tbl.addr.size = 0x0010;
  Soc_pb_tbls.iqm.high_dynamicb_tbl.addr.width_bits = 52;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamicb_tbl.time_stamp), 7, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamicb_tbl.cbl), 29, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.high_dynamicb_tbl.ebl), 51, 30);

  /* Cnts Mema */
  Soc_pb_tbls.iqm.cnts_mem_tbl[0].addr.base = 0x03b00000;
  Soc_pb_tbls.iqm.cnts_mem_tbl[0].addr.size = 0x2000;
  Soc_pb_tbls.iqm.cnts_mem_tbl[0].addr.width_bits = 57;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnts_mem_tbl[0].packets_counter), 24, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnts_mem_tbl[0].octets_counter), 56, 25);

  /* Cnts Memb */
  Soc_pb_tbls.iqm.cnts_mem_tbl[1].addr.base = 0x03c00000;
  Soc_pb_tbls.iqm.cnts_mem_tbl[1].addr.size = 0x2000;
  Soc_pb_tbls.iqm.cnts_mem_tbl[1].addr.width_bits = 57;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnts_mem_tbl[1].packets_counter), 24, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.cnts_mem_tbl[1].octets_counter), 56, 25);

  /* Ovth Mema */
  Soc_pb_tbls.iqm.ovth_mema_tbl.addr.base = 0x03d00000;
  Soc_pb_tbls.iqm.ovth_mema_tbl.addr.size = 0x0080;
  Soc_pb_tbls.iqm.ovth_mema_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.ovth_mema_tbl.ovth_counter_bits), 63, 0);

  /* Ovth Memb */
  Soc_pb_tbls.iqm.ovth_memb_tbl.addr.base = 0x03e00000;
  Soc_pb_tbls.iqm.ovth_memb_tbl.addr.size = 0x0080;
  Soc_pb_tbls.iqm.ovth_memb_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.iqm.ovth_memb_tbl.ovth_counter_bits), 63, 0);
  }
}

/* Block tables initialization: IPS 	 */
STATIC void
  soc_pb_tbls_init_IPS(void)
{

  /* System Physical Port Lookup Table */
  Soc_pb_tbls.ips.system_physical_port_lookup_table_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.ips.system_physical_port_lookup_table_tbl.addr.size = 0x2000;
  Soc_pb_tbls.ips.system_physical_port_lookup_table_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.system_physical_port_lookup_table_tbl.sys_phy_port), 11, 0);

  /* Destination Device And Port Lookup Table */
  Soc_pb_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.base = 0x00008000;
  Soc_pb_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.size = 0x1000;
  Soc_pb_tbls.ips.destination_device_and_port_lookup_table_tbl.addr.width_bits = 19;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.destination_device_and_port_lookup_table_tbl.dest_port), 7, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.destination_device_and_port_lookup_table_tbl.dest_dev), 18, 8);

  /* Flow Id Lookup Table */
  Soc_pb_tbls.ips.flow_id_lookup_table_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.ips.flow_id_lookup_table_tbl.addr.size = 0x2000;
  Soc_pb_tbls.ips.flow_id_lookup_table_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.flow_id_lookup_table_tbl.base_flow), 13, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.flow_id_lookup_table_tbl.sub_flow_mode), 14, 14);

  /* Queue Type Lookup Table */
  Soc_pb_tbls.ips.queue_type_lookup_table_tbl.addr.base = 0x00018000;
  Soc_pb_tbls.ips.queue_type_lookup_table_tbl.addr.size = 0x8000;
  Soc_pb_tbls.ips.queue_type_lookup_table_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_type_lookup_table_tbl.queue_type_lookup_table), 3, 0);

  /* Queue Priority Map Select */
  Soc_pb_tbls.ips.queue_priority_map_select_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.ips.queue_priority_map_select_tbl.addr.size = 0x0200;
  Soc_pb_tbls.ips.queue_priority_map_select_tbl.addr.width_bits = 2;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_priority_map_select_tbl.queue_priority_map_select), 1, 0);

  /* Queue Priority Maps Table */
  Soc_pb_tbls.ips.queue_priority_maps_table_tbl.addr.base = 0x00028000;
  Soc_pb_tbls.ips.queue_priority_maps_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ips.queue_priority_maps_table_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_priority_maps_table_tbl.queue_priority_maps_table), 63, 0);

  /* Queue Size-Based Thresholds Table */
  Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.addr.base = 0x00030000;
  Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.addr.size = 0x0010;
  Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.addr.width_bits = 36;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.off_to_slow_msg_th), 7, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.off_to_norm_msg_th), 15, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.slow_to_norm_msg_th), 23, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.norm_to_slow_msg_th), 31, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_based_thresholds_table_tbl.fsm_th_mul), 35, 32);

  /* Credit Balance Based Thresholds Table */
  Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.base = 0x00038000;
  Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.size = 0x0010;
  Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.addr.width_bits = 32;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.backoff_enter_qcr_bal_th), 7, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.backoff_exit_qcr_bal_th), 15, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.backlog_enter_qcr_bal_th), 23, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_balance_based_thresholds_table_tbl.backlog_exit_qcr_bal_th), 31, 24);

  /* Empty Queue Credit Balance Table */
  Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.addr.base = 0x00040000;
  Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.addr.size = 0x0010;
  Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.empty_qsatisfied_cr_bal), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.max_empty_qcr_bal), 31, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.empty_queue_credit_balance_table_tbl.exceed_max_empty_qcr_bal), 32, 32);

  /* Credit Watchdog Thresholds Table */
  Soc_pb_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.base = 0x00048000;
  Soc_pb_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.size = 0x0010;
  Soc_pb_tbls.ips.credit_watchdog_thresholds_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_watchdog_thresholds_table_tbl.wd_status_msg_gen_period), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.credit_watchdog_thresholds_table_tbl.wd_delete_qth), 7, 4);

  /* Queue Descriptor Table */
  Soc_pb_tbls.ips.queue_descriptor_table_tbl.addr.base = 0x00050000;
  Soc_pb_tbls.ips.queue_descriptor_table_tbl.addr.size = 0x8000;
  Soc_pb_tbls.ips.queue_descriptor_table_tbl.addr.width_bits = 28;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.cr_bal), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.crs), 17, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.one_pkt_deq), 18, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.wd_last_cr_time), 23, 19);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.in_dqcq), 24, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.wd_delete), 25, 25);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_descriptor_table_tbl.fsmrq_ctrl), 27, 26);

  /* Queue Size Table */
  Soc_pb_tbls.ips.queue_size_table_tbl.addr.base = 0x00058000;
  Soc_pb_tbls.ips.queue_size_table_tbl.addr.size = 0x8000;
  Soc_pb_tbls.ips.queue_size_table_tbl.addr.width_bits = 11;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_table_tbl.exponent), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_table_tbl.mantissa), 6, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.queue_size_table_tbl.qsize_4b), 10, 7);

  /* System Red Max Queue Size Table */
  Soc_pb_tbls.ips.system_red_max_queue_size_table_tbl.addr.base = 0x00078000;
  Soc_pb_tbls.ips.system_red_max_queue_size_table_tbl.addr.size = 0x1000;
  Soc_pb_tbls.ips.system_red_max_queue_size_table_tbl.addr.width_bits = 6;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.system_red_max_queue_size_table_tbl.maxqsz), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.system_red_max_queue_size_table_tbl.maxqsz_age), 5, 4);
  
  /* FMS message memory Table */
  Soc_pb_tbls.ips.fms_msg_mem_table_tbl.addr.base = 0x00080000;
  Soc_pb_tbls.ips.fms_msg_mem_table_tbl.addr.size = 0x80;
  Soc_pb_tbls.ips.fms_msg_mem_table_tbl.addr.width_bits = 59;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.flow_id), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.dest_pipe), 26, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.flow_status), 28, 27);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.dest_port), 36, 29);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.queue_size_4b), 40, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.queue_number), 55, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.orig_fs), 56, 56);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.disable_timer), 57, 57);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_msg_mem_table_tbl.valid), 58, 58);
  
  /* FMS timestamp table  */
  Soc_pb_tbls.ips.fms_timestamp_table_tbl.addr.base = 0x00088000;
  Soc_pb_tbls.ips.fms_timestamp_table_tbl.addr.size = 0x100;
  Soc_pb_tbls.ips.fms_timestamp_table_tbl.addr.width_bits = 13;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_timestamp_table_tbl.time_stamp), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.fms_timestamp_table_tbl.bin_idx), 12, 6);

  /* enq_block_qnum_table_tbl */
  Soc_pb_tbls.ips.enq_block_qnum_table_tbl.addr.base = 0x00090000;
  Soc_pb_tbls.ips.enq_block_qnum_table_tbl.addr.size = 0x020;
  Soc_pb_tbls.ips.enq_block_qnum_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.enq_block_qnum_table_tbl.qnum), 14, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.enq_block_qnum_table_tbl.valid), 15, 15);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ips.enq_block_qnum_table_tbl.visited), 16, 16);

}
  
/* Block tables initialization: IPT 	 */
STATIC void
  soc_pb_tbls_init_IPT(void)
{

  /* BDQ */
  Soc_pb_tbls.ipt.bdq_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.ipt.bdq_tbl.addr.size = 0x1000;
  Soc_pb_tbls.ipt.bdq_tbl.addr.width_bits = 26;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.bdq_tbl.bdq), 25, 0);

  /* PCQ */
  Soc_pb_tbls.ipt.pcq_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.ipt.pcq_tbl.addr.size = 0x1000;
  Soc_pb_tbls.ipt.pcq_tbl.addr.width_bits = 63;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.pcq_tbl.pcq), 62, 0);

  /* Sop MMU */
  Soc_pb_tbls.ipt.sop_mmu_tbl.addr.base = 0x00030000;
  Soc_pb_tbls.ipt.sop_mmu_tbl.addr.size = 0x0600;
  Soc_pb_tbls.ipt.sop_mmu_tbl.addr.width_bits = 84;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.sop_mmu_tbl.sop_mmu), 83, 0);

  /* Mop MMU */
  Soc_pb_tbls.ipt.mop_mmu_tbl.addr.base = 0x00040000;
  Soc_pb_tbls.ipt.mop_mmu_tbl.addr.size = 0x0600;
  Soc_pb_tbls.ipt.mop_mmu_tbl.addr.width_bits = 31;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.mop_mmu_tbl.mop_mmu), 30, 0);

  /* FDTCTL */
  Soc_pb_tbls.ipt.fdtctl_tbl.addr.base = 0x00050000;
  Soc_pb_tbls.ipt.fdtctl_tbl.addr.size = 0x0200;
  Soc_pb_tbls.ipt.fdtctl_tbl.addr.width_bits = 30;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdtctl_tbl.fdtctl), 29, 0);

  /* FDTDATA */
  Soc_pb_tbls.ipt.fdtdata_tbl.addr.base = 0x00060000;
  Soc_pb_tbls.ipt.fdtdata_tbl.addr.size = 0x0200;
  Soc_pb_tbls.ipt.fdtdata_tbl.addr.width_bits = 512;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdtdata_tbl.fdtdata), 511, 0);

  /* EGQCTL */
  Soc_pb_tbls.ipt.egqctl_tbl.addr.base = 0x00070000;
  Soc_pb_tbls.ipt.egqctl_tbl.addr.size = 0x0020;
  Soc_pb_tbls.ipt.egqctl_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.egqctl_tbl.egqctl), 6, 0);

  /* EGQDATA */
  Soc_pb_tbls.ipt.egqdata_tbl.addr.base = 0x00080000;
  Soc_pb_tbls.ipt.egqdata_tbl.addr.size = 0x0020;
  Soc_pb_tbls.ipt.egqdata_tbl.addr.width_bits = 512;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.egqdata_tbl.egqdata), 511, 0);

  /* Select Source Counter (Petar-B: Cfg Byte Cnt) */
  Soc_pb_tbls.ipt.select_source_sum_tbl.addr.base = 0x000a0000;
  Soc_pb_tbls.ipt.select_source_sum_tbl.addr.size = 0x0001;
  Soc_pb_tbls.ipt.select_source_sum_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.select_source_sum_tbl.select_source_sum), 46, 0);

  /* Egq Txq Wr Addr */
  Soc_pb_tbls.ipt.egq_txq_wr_addr_tbl.addr.base = 0x000b0000;
  Soc_pb_tbls.ipt.egq_txq_wr_addr_tbl.addr.size = 0x0001;
  Soc_pb_tbls.ipt.egq_txq_wr_addr_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.egq_txq_wr_addr_tbl.egq_txq_wr_addr), 4, 0);

  /* Egq Txq Rd Addr */
  Soc_pb_tbls.ipt.egq_txq_rd_addr_tbl.addr.base = 0x000c0000;
  Soc_pb_tbls.ipt.egq_txq_rd_addr_tbl.addr.size = 0x0001;
  Soc_pb_tbls.ipt.egq_txq_rd_addr_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.egq_txq_rd_addr_tbl.egq_txq_rd_addr), 4, 0);

  /* Fdt Txq Wr Addr */
  Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.addr.base = 0x000d0000;
  Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.addr.size = 0x0001;
  Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.addr.width_bits = 63;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq0_wr_addr), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq1_wr_addr), 17, 9);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq2_wr_addr), 26, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq3_wr_addr), 35, 27);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq4_wr_addr), 44, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq5_wr_addr), 53, 45);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_wr_addr_tbl.dtq6_wr_addr), 62, 54);

  /* Fdt Txq Rd Addr */
  Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.addr.base = 0x000e0000;
  Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.addr.size = 0x0001;
  Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.addr.width_bits = 63;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq0_rd_addr), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq1_rd_addr), 17, 9);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq2_rd_addr), 26, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq3_rd_addr), 35, 27);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq4_rd_addr), 44, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq5_rd_addr), 53, 45);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.fdt_txq_rd_addr_tbl.dtq6_rd_addr), 62, 54);

  /* Gci Backoff Mask */
  Soc_pb_tbls.ipt.gci_backoff_mask_tbl.addr.base = 0x000f0000;
  Soc_pb_tbls.ipt.gci_backoff_mask_tbl.addr.size = 0x0003;
  Soc_pb_tbls.ipt.gci_backoff_mask_tbl.addr.width_bits = 48;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.gci_backoff_mask_tbl.mask0), 11, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.gci_backoff_mask_tbl.mask1), 23, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.gci_backoff_mask_tbl.mask2), 35, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ipt.gci_backoff_mask_tbl.mask3), 47, 36);
}

/* Block tables initialization: DPI 	 */
STATIC void
  soc_pb_tbls_init_DPI(void)
{

  /* DLL RAM */
  Soc_pb_tbls.dpi.dll_ram_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.dpi.dll_ram_tbl.addr.size = 0x0080;
  Soc_pb_tbls.dpi.dll_ram_tbl.addr.width_bits = 56;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.dpi.dll_ram_tbl.dll_ram), 55, 0);
}

/* Block tables initialization: FDT 	 */
STATIC void
  soc_pb_tbls_init_FDT(void)
{

  /* Ipt Contro L Fifo */
  Soc_pb_tbls.fdt.ipt_contro_l_fifo_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.fdt.ipt_contro_l_fifo_tbl.addr.size = 0x0001;
  Soc_pb_tbls.fdt.ipt_contro_l_fifo_tbl.addr.width_bits = 61;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.fdt.ipt_contro_l_fifo_tbl.ipt_contro_l_fifo), 60, 0);
}


/* Block tables initialization: NBI 	 */
STATIC void
  soc_pb_tbls_init_NBI(void)
{
  uint32
    indx;

  for(indx = 0 ; indx < 16; ++indx){
    Soc_pb_tbls.nbi.mlf_rx_tbl[indx].addr.base = 0x00090000 + (indx * 0x100);
    Soc_pb_tbls.nbi.mlf_rx_tbl[indx].addr.size = 0x0080;
    Soc_pb_tbls.nbi.mlf_rx_tbl[indx].addr.width_bits = 21;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.nbi.mlf_rx_tbl[indx].field), 20, 0);
  }

  for(indx = 0 ; indx < 16; ++indx){
    if (indx == 3)
    {
      Soc_pb_tbls.nbi.mlf_tx_tbl[indx].addr.base = 0x00010300;
    }
    else
    {
      Soc_pb_tbls.nbi.mlf_tx_tbl[indx].addr.base = 0x00091000 + (indx * 0x100);
    }
    Soc_pb_tbls.nbi.mlf_tx_tbl[indx].addr.size = 0x0080;
    Soc_pb_tbls.nbi.mlf_tx_tbl[indx].addr.width_bits = 21;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.nbi.mlf_tx_tbl[indx].field), 20, 0);
  }

    Soc_pb_tbls.nbi.mlf_rx_eob.addr.base = 0x00092000;
    Soc_pb_tbls.nbi.mlf_rx_eob.addr.size = 0x0010;
    Soc_pb_tbls.nbi.mlf_rx_eob.addr.width_bits = 128;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.nbi.mlf_rx_eob.field), 127, 0);

    Soc_pb_tbls.nbi.mlf_tx_eob.addr.base = 0x00093000;
    Soc_pb_tbls.nbi.mlf_tx_eob.addr.size = 0x0010;
    Soc_pb_tbls.nbi.mlf_tx_eob.addr.width_bits = 32;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.nbi.mlf_tx_eob.field), 31, 0);



}

/* Block tables initialization: RTP */
STATIC void
  soc_pb_tbls_init_RTP(void)
{

  /* Unicast Distribution Memory for data cells */
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.size = 0x0800;
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.addr.width_bits = 72;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.rtp.unicast_distribution_memory_for_data_cells_tbl.unicast_distribution_memory_for_data_cells), 71, 0);

  /* Unicast Distribution Memory for control cells */
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base = 0x00001000;
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.size = 0x0800;
  Soc_pb_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.addr.width_bits = 72;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.rtp.unicast_distribution_memory_for_control_cells_tbl.unicast_distribution_memory_for_control_cells), 71, 0);
}

/* Block tables initialization: EGQ */
STATIC void
  soc_pb_tbls_init_EGQ(void)
{
  /* Egress Shaper Nif Mal0 Credit Configuration (Nif Mal0 Scm) */
  /* Same for MAL 0 - 15                                        */
  Soc_pb_tbls.egq.nif_scm_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.egq.nif_scm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.nif_scm_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.nif_scm_tbl.port_cr_to_add), 17, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.nif_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper Recycling Ports Credit Configuration (Rcy Scm) */
  Soc_pb_tbls.egq.rcy_scm_tbl.addr.base = 0x00110000;
  Soc_pb_tbls.egq.rcy_scm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.rcy_scm_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rcy_scm_tbl.port_cr_to_add), 17, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rcy_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper CPUPorts Credit Configuration(Cpu Scm) */
  Soc_pb_tbls.egq.cpu_scm_tbl.addr.base = 0x00120000;
  Soc_pb_tbls.egq.cpu_scm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.cpu_scm_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cpu_scm_tbl.port_cr_to_add), 17, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cpu_scm_tbl.ofp_index), 24, 18);

  /* Egress Shaper Calendar Selector (CCM) */
  Soc_pb_tbls.egq.ccm_tbl.addr.base = 0x00130000;
  Soc_pb_tbls.egq.ccm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.ccm_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ccm_tbl.interface_select), 4, 0);

  /* Per Port Max Credit Memory (PMC) */
  Soc_pb_tbls.egq.pmc_tbl.addr.base = 0x00140000;
  Soc_pb_tbls.egq.pmc_tbl.addr.size = 0x00a0;
  Soc_pb_tbls.egq.pmc_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pmc_tbl.port_max_credit), 15, 0);

  /* Egress Shaper Per Port Credit Balance Memory (CBM) */
  Soc_pb_tbls.egq.cbm_tbl.addr.base = 0x00150000;
  Soc_pb_tbls.egq.cbm_tbl.addr.size = 0x0050;
  Soc_pb_tbls.egq.cbm_tbl.addr.width_bits = 25;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cbm_tbl.cbm), 24, 0);

  /* Free Buffers Allocation Bitmap Memory (FBM) */
  Soc_pb_tbls.egq.fbm_tbl.addr.base = 0x00200000;
  Soc_pb_tbls.egq.fbm_tbl.addr.size = 0x0080;
  Soc_pb_tbls.egq.fbm_tbl.addr.width_bits = 32;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fbm_tbl.free_buffer_memory), 31, 0);

  /* Free Descriptors Allocation Bitmap Memory (FDM) */
  Soc_pb_tbls.egq.fdm_tbl.addr.base = 0x00210000;
  Soc_pb_tbls.egq.fdm_tbl.addr.size = 0x0080;
  Soc_pb_tbls.egq.fdm_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fdm_tbl.free_descriptor_memory), 63, 0);

  /* QM */
  Soc_pb_tbls.egq.qm_tbl.addr.base = 0x00220000;
  Soc_pb_tbls.egq.qm_tbl.addr.size = 0x0280;
  Soc_pb_tbls.egq.qm_tbl.addr.width_bits = 60;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.read_pointer), 12, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.packet_size256to_eop), 21, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.counter_decreament), 33, 22);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.copy_data), 49, 34);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.last_seg_size), 51, 50);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.before_last_seg_size), 54, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.pqp_qnum), 56, 55);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qm_tbl.pqp_oc768_qnum), 58, 57);

  /* QSM */
  Soc_pb_tbls.egq.qsm_tbl.addr.base = 0x00230000;
  Soc_pb_tbls.egq.qsm_tbl.addr.size = 0x0200;
  Soc_pb_tbls.egq.qsm_tbl.addr.width_bits = 15;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.qsm_tbl.qsm), 14, 0);

  /* DCM */
  Soc_pb_tbls.egq.dcm_tbl.addr.base = 0x00240000;
  Soc_pb_tbls.egq.dcm_tbl.addr.size = 0x0200;
  Soc_pb_tbls.egq.dcm_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.dcm_tbl.dcm), 11, 0);

  /* Ofp Dequeue Wfq Configuration Memory (DWM) */
  Soc_pb_tbls.egq.dwm_tbl.addr.base = 0x00250000;
  Soc_pb_tbls.egq.dwm_tbl.addr.size = 0x0050;
  Soc_pb_tbls.egq.dwm_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.dwm_tbl.mc_or_mc_low_queue_weight), 15, 8); /* Wrong documentation until Soc_petra-B B0 */
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.dwm_tbl.uc_or_uc_low_queue_weight), 7, 0);

  /* Ofp Dequeue Wfq Configuration Memory (DWM_NEW) */
  Soc_pb_tbls.egq.dwm_new_tbl.addr.base = 0x00260000;
  Soc_pb_tbls.egq.dwm_new_tbl.addr.size = 0x0050;
  Soc_pb_tbls.egq.dwm_new_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.dwm_new_tbl.mc_high_queue_weight), 15, 8); /* Wrong documentation until Soc_petra-B B0 */
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.dwm_new_tbl.uc_high_queue_weight), 7, 0);

  /* RRDM */
  Soc_pb_tbls.egq.rrdm_tbl.addr.base = 0x00270000;
  Soc_pb_tbls.egq.rrdm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.rrdm_tbl.addr.width_bits = 40;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.crcremainder), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.reas_state), 17, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.eopfrag_num), 26, 18);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.nxt_frag_number), 35, 27);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.stored_seg_size), 37, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rrdm_tbl.fix129), 38, 38);

  /* RPDM */
  Soc_pb_tbls.egq.rpdm_tbl.addr.base = 0x00280000;
  Soc_pb_tbls.egq.rpdm_tbl.addr.size = 0x0100;
  Soc_pb_tbls.egq.rpdm_tbl.addr.width_bits = 35;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rpdm_tbl.packet_start_buffer_pointer), 11, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rpdm_tbl.packet_buffer_write_pointer), 24, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rpdm_tbl.reas_state), 26, 25);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rpdm_tbl.packet_frag_cnt), 34, 27);

  /* Port Configuration Table (PCT) */
  Soc_pb_tbls.egq.pct_tbl.addr.base = 0x00290000;
  Soc_pb_tbls.egq.pct_tbl.addr.size = 0x0050;
  Soc_pb_tbls.egq.pct_tbl.addr.width_bits = 51;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.prog_editor_value), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.out_pp_port), 21, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.outbound_mirr), 22, 22   );
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.dest_port), 35, 23);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.port_profile), 39, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.port_ch_num), 47, 40);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.port_type), 49, 48);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pct_tbl.cr_adjust_type), 50, 50);

  /* Vlan Table Configuration Memory(Vlan Table) */
  Soc_pb_tbls.egq.vlan_table_tbl.addr.base = 0x002a0000;
  Soc_pb_tbls.egq.vlan_table_tbl.addr.size = 0x1000;
  Soc_pb_tbls.egq.vlan_table_tbl.addr.width_bits = 80;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.vlan_table_tbl.vlan_membership), 79, 0);

  /* Tc Dp Map */
  Soc_pb_tbls.egq.tc_dp_map_tbl.addr.base = 0x002d0000;
  Soc_pb_tbls.egq.tc_dp_map_tbl.addr.size = 0x0200;
  Soc_pb_tbls.egq.tc_dp_map_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tc_dp_map_tbl.tc), 2, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tc_dp_map_tbl.dp), 4, 3);

  /* Cfc Flow Control */
  Soc_pb_tbls.egq.cfc_flow_control_tbl.addr.base = 0x00300000;
  Soc_pb_tbls.egq.cfc_flow_control_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.cfc_flow_control_tbl.addr.width_bits = 80;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cfc_flow_control_tbl.cfc_flow_control), 79, 0);

  /* Nifa Flow Control */
  Soc_pb_tbls.egq.nifa_flow_control_tbl.addr.base = 0x00310000;
  Soc_pb_tbls.egq.nifa_flow_control_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.nifa_flow_control_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.nifa_flow_control_tbl.nifa_flow_control), 15, 0);

  /* Nifb Flow Control */
  Soc_pb_tbls.egq.nifb_flow_control_tbl.addr.base = 0x00320000;
  Soc_pb_tbls.egq.nifb_flow_control_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.nifb_flow_control_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.nifb_flow_control_tbl.nifb_flow_control), 15, 0);

  /* Cpu Last Header */
  Soc_pb_tbls.egq.cpu_last_header_tbl.addr.base = 0x00330000;
  Soc_pb_tbls.egq.cpu_last_header_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.cpu_last_header_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cpu_last_header_tbl.cpu_last_header), 63, 0);

  /* Ipt Last Header */
  Soc_pb_tbls.egq.ipt_last_header_tbl.addr.base = 0x00340000;
  Soc_pb_tbls.egq.ipt_last_header_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ipt_last_header_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ipt_last_header_tbl.ipt_last_header), 63, 0);

  /* Fdr Last Header */
  Soc_pb_tbls.egq.fdr_last_header_tbl.addr.base = 0x00350000;
  Soc_pb_tbls.egq.fdr_last_header_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.fdr_last_header_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fdr_last_header_tbl.fdr_last_header), 63, 0);

  /* Cpu Packet Counter */
  Soc_pb_tbls.egq.cpu_packet_counter_tbl.addr.base = 0x00360000;
  Soc_pb_tbls.egq.cpu_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.cpu_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.cpu_packet_counter_tbl.cpu_packet_counter), 32, 0);

  /* Ipt Packet Counter */
  Soc_pb_tbls.egq.ipt_packet_counter_tbl.addr.base = 0x00370000;
  Soc_pb_tbls.egq.ipt_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ipt_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ipt_packet_counter_tbl.ipt_packet_counter), 32, 0);

  /* Fdr Packet Counter */
  Soc_pb_tbls.egq.fdr_packet_counter_tbl.addr.base = 0x00380000;
  Soc_pb_tbls.egq.fdr_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.fdr_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fdr_packet_counter_tbl.fdr_packet_counter), 32, 0);

  /* Rqp Packet Counter */
  Soc_pb_tbls.egq.rqp_packet_counter_tbl.addr.base = 0x00390000;
  Soc_pb_tbls.egq.rqp_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.rqp_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rqp_packet_counter_tbl.rqp_packet_counter), 32, 0);

  /* Rqp Discard Packet Counter */
  Soc_pb_tbls.egq.rqp_discard_packet_counter_tbl.addr.base = 0x003a0000;
  Soc_pb_tbls.egq.rqp_discard_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.rqp_discard_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.rqp_discard_packet_counter_tbl.rqp_discard_packet_counter), 32, 0);

  /* Ehp Unicast Packet Counter */
  Soc_pb_tbls.egq.ehp_unicast_packet_counter_tbl.addr.base = 0x003b0000;
  Soc_pb_tbls.egq.ehp_unicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ehp_unicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ehp_unicast_packet_counter_tbl.ehp_unicast_packet_counter), 32, 0);

  /* Ehp Multicast High Packet Counter */
  Soc_pb_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.base = 0x003c0000;
  Soc_pb_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ehp_multicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ehp_multicast_high_packet_counter_tbl.ehp_multicast_high_packet_counter), 32, 0);

  /* Ehp Multicast Low Packet Counter */
  Soc_pb_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.base = 0x003d0000;
  Soc_pb_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ehp_multicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ehp_multicast_low_packet_counter_tbl.ehp_multicast_low_packet_counter), 32, 0);

  /* Ehp Discard Packet Counter */
  Soc_pb_tbls.egq.ehp_discard_packet_counter_tbl.addr.base = 0x003e0000;
  Soc_pb_tbls.egq.ehp_discard_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.ehp_discard_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.ehp_discard_packet_counter_tbl.ehp_discard_packet_counter), 32, 0);

  /* Pqp Unicast High Packet Counter */
  Soc_pb_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.base = 0x003f0000;
  Soc_pb_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_unicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_unicast_high_packet_counter_tbl.pqp_unicast_high_packet_counter), 32, 0);

  /* Pqp Unicast Low Packet Counter */
  Soc_pb_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.base = 0x00400000;
  Soc_pb_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_unicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_unicast_low_packet_counter_tbl.pqp_unicast_low_packet_counter), 32, 0);

  /* Pqp Multicast High Packet Counter */
  Soc_pb_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.base = 0x00410000;
  Soc_pb_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_multicast_high_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_multicast_high_packet_counter_tbl.pqp_multicast_high_packet_counter), 32, 0);

  /* Pqp Multicast Low Packet Counter */
  Soc_pb_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.base = 0x00420000;
  Soc_pb_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_multicast_low_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_multicast_low_packet_counter_tbl.pqp_multicast_low_packet_counter), 32, 0);

  /* Pqp Unicast High Bytes Counter */
  Soc_pb_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.base = 0x00430000;
  Soc_pb_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_unicast_high_bytes_counter_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_unicast_high_bytes_counter_tbl.pqp_unicast_high_bytes_counter), 46, 0);

  /* Pqp Unicast Low Bytes Counter */
  Soc_pb_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.base = 0x00440000;
  Soc_pb_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_unicast_low_bytes_counter_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_unicast_low_bytes_counter_tbl.pqp_unicast_low_bytes_counter), 46, 0);

  /* Pqp Multicast High Bytes Counter */
  Soc_pb_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.base = 0x00450000;
  Soc_pb_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_multicast_high_bytes_counter_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_multicast_high_bytes_counter_tbl.pqp_multicast_high_bytes_counter), 46, 0);

  /* Pqp Multicast Low Bytes Counter */
  Soc_pb_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.base = 0x00460000;
  Soc_pb_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_multicast_low_bytes_counter_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_multicast_low_bytes_counter_tbl.pqp_multicast_low_bytes_counter), 46, 0);

  /* Pqp Discard Unicast Packet Counter */
  Soc_pb_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.base = 0x00470000;
  Soc_pb_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_discard_unicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_discard_unicast_packet_counter_tbl.pqp_discard_unicast_packet_counter), 32, 0);

  /* Pqp Discard Multicast Packet Counter */
  Soc_pb_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.base = 0x00480000;
  Soc_pb_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.pqp_discard_multicast_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.pqp_discard_multicast_packet_counter_tbl.pqp_discard_multicast_packet_counter), 32, 0);

  /* Fqp Packet Counter */
  Soc_pb_tbls.egq.fqp_packet_counter_tbl.addr.base = 0x00490000;
  Soc_pb_tbls.egq.fqp_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.fqp_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fqp_packet_counter_tbl.fqp_packet_counter), 32, 0);

  /* Fqp Nif Port Mux */
  Soc_pb_tbls.egq.fqp_nif_port_mux_tbl.addr.base = 0x004a0000;
  Soc_pb_tbls.egq.fqp_nif_port_mux_tbl.addr.size = 0x0080;
  Soc_pb_tbls.egq.fqp_nif_port_mux_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.fqp_nif_port_mux_tbl.fqp_nif_port_mux), 6, 0);

  /* Key Profile Map Index */
  Soc_pb_tbls.egq.key_profile_map_index_tbl.addr.base = 0x00650000;
  Soc_pb_tbls.egq.key_profile_map_index_tbl.addr.size = 0x0040;
  Soc_pb_tbls.egq.key_profile_map_index_tbl.addr.width_bits = 3;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.key_profile_map_index_tbl.key_profile_map_index), 2, 0);

  /* TCAM Key Resolution Profile */
  Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.addr.base = 0x00660000;
  Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.addr.size = 0x0008;
  Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.key_select), 2, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.key_and_value), 7, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.key_or_value), 11, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.tcam_key_resolution_profile_tbl.tcam_profile), 15, 12);

  /* Parser Last Sys Record */
  Soc_pb_tbls.egq.parser_last_sys_record_tbl.addr.base = 0x00680000;
  Soc_pb_tbls.egq.parser_last_sys_record_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.parser_last_sys_record_tbl.addr.width_bits = 227;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.parser_last_sys_record_tbl.parser_last_sys_record), 226, 0);

  /* Parser Last Nwk Record1 */
  Soc_pb_tbls.egq.parser_last_nwk_record1_tbl.addr.base = 0x00690000;
  Soc_pb_tbls.egq.parser_last_nwk_record1_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.parser_last_nwk_record1_tbl.addr.width_bits = 256;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.parser_last_nwk_record1_tbl.parser_last_nwk_record1), 255, 0);

  /* Parser Last Nwk Record2 */
  Soc_pb_tbls.egq.parser_last_nwk_record2_tbl.addr.base = 0x006a0000;
  Soc_pb_tbls.egq.parser_last_nwk_record2_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.parser_last_nwk_record2_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.parser_last_nwk_record2_tbl.parser_last_nwk_record2), 46, 0);

  /* Erpp Debug */
  Soc_pb_tbls.egq.erpp_debug_tbl.addr.base = 0x006b0000;
  Soc_pb_tbls.egq.erpp_debug_tbl.addr.size = 0x0001;
  Soc_pb_tbls.egq.erpp_debug_tbl.addr.width_bits = 231;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.egq.erpp_debug_tbl.erpp_debug), 230, 0);

  /* SOC_PB_EGQ_SVEM_PLDT_H0 */

  Soc_pb_tbls.egq.svem_keyt_h0_tbl.addr.base = 0x00520000;
  Soc_pb_tbls.egq.svem_keyt_h0_tbl.addr.size = 0x01000;
  Soc_pb_tbls.egq.svem_keyt_h0_tbl.addr.width_bits = 12;


  /* SOC_PB_EGQ_SVEM_PLDT_H1 */

  Soc_pb_tbls.egq.svem_keyt_h1_tbl.addr.base = 0x00530000;
  Soc_pb_tbls.egq.svem_keyt_h1_tbl.addr.size = 0x01000;
  Soc_pb_tbls.egq.svem_keyt_h1_tbl.addr.width_bits = 12;


  /* SOC_PB_EGQ_SVEM_PLDT_AUX */

  Soc_pb_tbls.egq.svem_keyt_aux_tbl.addr.base = 0x00540000;
  Soc_pb_tbls.egq.svem_keyt_aux_tbl.addr.size = 0x00020;
  Soc_pb_tbls.egq.svem_keyt_aux_tbl.addr.width_bits = 12;


  /* SOC_PB_EGQ_SVEM_PLDT_H0_TBL */

  Soc_pb_tbls.egq.svem_pldt_h0_tbl.addr.base = 0x00550000;
  Soc_pb_tbls.egq.svem_pldt_h0_tbl.addr.size = 0x01000;
  Soc_pb_tbls.egq.svem_pldt_h0_tbl.addr.width_bits = 12;

  /*  SOC_PB_EGQ_SVEM_PLDT_H1_TBL */

  Soc_pb_tbls.egq.svem_pldt_h1_tbl.addr.base = 0x00560000;
  Soc_pb_tbls.egq.svem_pldt_h1_tbl.addr.size = 0x01000;
  Soc_pb_tbls.egq.svem_pldt_h1_tbl.addr.width_bits = 12;

  /* SOC_PB_EGQ_SVEM_PLDT_AUX_TBL */

  Soc_pb_tbls.egq.svem_pldt_aux_tbl.addr.base = 0x00570000;
  Soc_pb_tbls.egq.svem_pldt_aux_tbl.addr.size = 0x0020;
  Soc_pb_tbls.egq.svem_pldt_aux_tbl.addr.width_bits = 12;

  /* SOC_PB_EGQ_SVEM_MGMT_MEM_A_TBL */

  Soc_pb_tbls.egq.svem_mgmt_mem_a_tbl.addr.base = 0x00580000;
  Soc_pb_tbls.egq.svem_mgmt_mem_a_tbl.addr.size = 0x020;
  Soc_pb_tbls.egq.svem_mgmt_mem_a_tbl.addr.width_bits = 28;

  /* SOC_PB_EGQ_SVEM_MGMT_MEM_B_TBL */

  Soc_pb_tbls.egq.svem_mgmt_mem_b_tbl.addr.base = 0x00590000;
  Soc_pb_tbls.egq.svem_mgmt_mem_b_tbl.addr.size = 0x020;
  Soc_pb_tbls.egq.svem_mgmt_mem_b_tbl.addr.width_bits = 28;


}

/* Block tables initialization: IHB  */
STATIC void
  soc_pb_tbls_init_IHB(void)
{
  uint32
    tbl_indx;

  /* Pinfo Lbp */
  Soc_pb_tbls.ihb.pinfo_lbp_tbl.addr.base = 0x00d00000;
  Soc_pb_tbls.ihb.pinfo_lbp_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.pinfo_lbp_tbl.addr.width_bits = 6;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.pinfo_lbp_tbl.counter_compension), 5, 0);

  /* Pinfo PMF */
  Soc_pb_tbls.ihb.pinfo_pmf_tbl.addr.base = 0x00300000;
  Soc_pb_tbls.ihb.pinfo_pmf_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.pinfo_pmf_tbl.addr.width_bits = 3;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.pinfo_pmf_tbl.port_pmf_profile), 2, 0);

  /* Packet Format Code Profile */
  Soc_pb_tbls.ihb.packet_format_code_profile_tbl.addr.base = 0x00310000;
  Soc_pb_tbls.ihb.packet_format_code_profile_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.packet_format_code_profile_tbl.addr.width_bits = 3;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.packet_format_code_profile_tbl.packet_format_code_profile), 2, 0);

  /* Src Dest Port For L3 ACL Key */
  Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.addr.base = 0x00410000;
  Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.addr.size = 0x0018;
  Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.addr.width_bits = 64;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.source_port_min), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.source_port_max), 31, 16);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.destination_port_min), 47, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.src_dest_port_for_l3_acl_key_tbl.destination_port_max), 63, 48);

  /* Direct 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00440000;
  Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 10;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.direct_key_select), 2, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.direct_db_and_value), 5, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_1st_pass_key_profile_resolved_data_tbl.direct_db_or_value), 9, 8);

  /* Direct 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00450000;
  Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 10;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.direct_key_select), 2, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.direct_db_and_value), 5, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.direct_2nd_pass_key_profile_resolved_data_tbl.direct_db_or_value), 9, 8);

  /* FEM0 4b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00470000;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM0 4b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.base = 0x00480000;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM0 4b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.base = 0x00490000;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM0 4b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.addr.base = 0x004a0000;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.addr.width_bits = 34;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_1st_pass_map_table_tbl.field_select_map3), 33, 28);

  /* FEM1 4b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x004b0000;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM1 4b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.base = 0x004c0000;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM1 4b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.base = 0x004d0000;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM1 4b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.addr.base = 0x004e0000;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.addr.width_bits = 34;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_1st_pass_map_table_tbl.field_select_map3), 33, 28);

  /* FEM2 14b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x004f0000;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM2 14b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.base = 0x00500000;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM2 14b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.base = 0x00510000;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM2 14b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.addr.base = 0x00520000;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM2 14b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_offset_table_tbl.addr.base = 0x00530000;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_1st_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_1st_pass_offset_table_tbl.offset), 13, 0);

  /* FEM3 14b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00540000;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM3 14b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.base = 0x00550000;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM3 14b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.base = 0x00560000;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM3 14b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.addr.base = 0x00570000;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM3 14b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_offset_table_tbl.addr.base = 0x00580000;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_1st_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_1st_pass_offset_table_tbl.offset), 13, 0);

  /* FEM4 14b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00590000;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM4 14b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.base = 0x005a0000;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM4 14b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.base = 0x005b0000;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM4 14b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.addr.base = 0x005c0000;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM4 14b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_offset_table_tbl.addr.base = 0x005d0000;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_1st_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_1st_pass_offset_table_tbl.offset), 13, 0);

  /* FEM5 17b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x005e0000;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM5 17b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.base = 0x005f0000;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM5 17b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.base = 0x00600000;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM5 17b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.addr.base = 0x00610000;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM5 17b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_offset_table_tbl.addr.base = 0x00620000;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_1st_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_1st_pass_offset_table_tbl.offset), 16, 0);

  /* FEM6 17b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00630000;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM6 17b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.base = 0x00640000;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM6 17b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.base = 0x00650000;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM6 17b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.addr.base = 0x00660000;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM6 17b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_offset_table_tbl.addr.base = 0x00670000;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_1st_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_1st_pass_offset_table_tbl.offset), 16, 0);

  /* FEM7 17b 1st Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.base = 0x00680000;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM7 17b 1st Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.base = 0x00690000;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM7 17b 1st Pass Map Index Table */
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.base = 0x006a0000;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM7 17b 1st Pass Map Table */
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.addr.base = 0x006b0000;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM7 17b 1st Pass Offset Table */
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_offset_table_tbl.addr.base = 0x006c0000;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_1st_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_1st_pass_offset_table_tbl.offset), 16, 0);

  /* FEM0 4b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x006d0000;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM0 4b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.base = 0x006e0000;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM0 4b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.base = 0x006f0000;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM0 4b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.addr.base = 0x00700000;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.addr.width_bits = 34;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem0_4b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);

  /* FEM1 4b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00710000;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM1 4b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.base = 0x00720000;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM1 4b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.base = 0x00730000;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM1 4b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.addr.base = 0x00740000;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.addr.width_bits = 34;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem1_4b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);

  /* FEM2 14b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00750000;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM2 14b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.base = 0x00760000;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM2 14b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.base = 0x00770000;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM2 14b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.addr.base = 0x00780000;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM2 14b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.base = 0x00790000;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem2_14b_2nd_pass_offset_table_tbl.offset), 13, 0);

  /* FEM3 14b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x007a0000;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM3 14b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.base = 0x007b0000;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM3 14b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.base = 0x007c0000;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM3 14b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.addr.base = 0x007d0000;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM3 14b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.base = 0x007e0000;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem3_14b_2nd_pass_offset_table_tbl.offset), 13, 0);

  /* FEM4 14b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x007f0000;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM4 14b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.base = 0x00800000;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM4 14b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.base = 0x00810000;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM4 14b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.addr.base = 0x00820000;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.addr.width_bits = 118;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_map_table_tbl.offset_index), 117, 116);

  /* FEM4 14b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.base = 0x00830000;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.width_bits = 14;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem4_14b_2nd_pass_offset_table_tbl.offset), 13, 0);

  /* FEM5 17b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00840000;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM5 17b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.base = 0x00850000;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM5 17b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.base = 0x00860000;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM5 17b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.addr.base = 0x00870000;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM5 17b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.base = 0x00880000;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem5_17b_2nd_pass_offset_table_tbl.offset), 16, 0);

  /* FEM6 17b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x00890000;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM6 17b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.base = 0x008a0000;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM6 17b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.base = 0x008b0000;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM6 17b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.addr.base = 0x008c0000;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM6 17b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.base = 0x008d0000;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem6_17b_2nd_pass_offset_table_tbl.offset), 16, 0);

  /* FEM7 17b 2nd Pass Key Profile Resolved Data */
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base = 0x008e0000;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size = 0x0008;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.program), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.key_select), 7, 4);

  /* FEM7 17b 2nd Pass Program Resolved Data */
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.base = 0x008f0000;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits = 5;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.bit_select), 4, 0);

  /* FEM7 17b 2nd Pass Map Index Table */
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.base = 0x00900000;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.size = 0x0040;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_index_table_tbl.map_index), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_index_table_tbl.map_data), 7, 4);

  /* FEM7 17b 2nd Pass Map Table */
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.addr.base = 0x00910000;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.addr.width_bits = 142;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.action_type), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map0), 9, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map1), 17, 12);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map2), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map3), 33, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map4), 41, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map5), 49, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map6), 57, 52);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map7), 65, 60);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map8), 73, 68);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map9), 81, 76);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map10), 89, 84);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map11), 97, 92);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map12), 105, 100);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map13), 113, 108);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map14), 121, 116);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map15), 129, 124);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.field_select_map16), 137, 132);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_map_table_tbl.offset_index), 141, 140);

  /* FEM7 17b 2nd Pass Offset Table */
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.base = 0x00920000;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.size = 0x0004;
  Soc_pb_tbls.ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.width_bits = 17;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.fem7_17b_2nd_pass_offset_table_tbl.offset), 16, 0);

  for(tbl_indx = 0; tbl_indx < 10; ++tbl_indx)
  {
    /* elk_fifo_data Offset Table */
    Soc_pb_tbls.ihb.elk_fifo_data[tbl_indx].addr.base = 0x00150000 + (tbl_indx * 0x100);
    Soc_pb_tbls.ihb.elk_fifo_data[tbl_indx].addr.size = 0x00c0;
    Soc_pb_tbls.ihb.elk_fifo_data[tbl_indx].addr.width_bits = 1024;
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.ihb.elk_fifo_data[tbl_indx].field), 1023, 0);

  }
 

}

/* Block tables initialization: EPNI 	 */
STATIC void
  soc_pb_tbls_init_EPNI(void)
{
  uint16
    fld_idx,
    tbl_idx;

  /* Epe Packet Counter */
  Soc_pb_tbls.epni.epe_packet_counter_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.epni.epe_packet_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.epni.epe_packet_counter_tbl.addr.width_bits = 33;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.epe_packet_counter_tbl.epe_packet_counter), 32, 0);

  /* Epe Bytes Counter */
  Soc_pb_tbls.epni.epe_bytes_counter_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.epni.epe_bytes_counter_tbl.addr.size = 0x0001;
  Soc_pb_tbls.epni.epe_bytes_counter_tbl.addr.width_bits = 47;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.epe_bytes_counter_tbl.epe_bytes_counter), 46, 0);

  /* Copy Engine0 Program */
  for (tbl_idx = 0; tbl_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS; ++tbl_idx)
  {
    for (fld_idx = 0; fld_idx < SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS; ++fld_idx)
    {
      Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].addr.base = 0x000d0000 + (0x10000 *tbl_idx);
      Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].addr.size = 0x0010;
      Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].addr.width_bits = 40;
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_valid[fld_idx]), 0+(fld_idx*20), 0+(fld_idx*20));
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_source_select[fld_idx]), 1+(fld_idx*20), 1+(fld_idx*20));
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_header_offset_select[fld_idx]), 4+(fld_idx*20), 2+(fld_idx*20));
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_niblle_field_offset[fld_idx]), 12+(fld_idx*20), 5+(fld_idx*20));
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_bit_count[fld_idx]), 17+(fld_idx*20), 13+(fld_idx*20));
      soc_pb_tbl_fld_set( &(Soc_pb_tbls.epni.copy_engine_program_tbl[tbl_idx].inst_lfem_program[fld_idx]), 19+(fld_idx*20), 18+(fld_idx*20));
    }
  }

  /* Lfem Field Select Map */
  for (tbl_idx = 0; tbl_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS; ++tbl_idx)
  {
    Soc_pb_tbls.epni.lfem_field_select_map_tbl[tbl_idx].addr.base = 0x00100000 + (0x10000 * tbl_idx);
    Soc_pb_tbls.epni.lfem_field_select_map_tbl[tbl_idx].addr.size = 0x0003;
    Soc_pb_tbls.epni.lfem_field_select_map_tbl[tbl_idx].addr.width_bits = 96;
    
    for (fld_idx = 0; fld_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++fld_idx)
    {
      soc_pb_tbl_fld_set(
        &(Soc_pb_tbls.epni.lfem_field_select_map_tbl[tbl_idx].lfem_field_select_map[fld_idx]),
        ((fld_idx + 1) * 6)-1,
        (fld_idx * 6));
    }
  }
  /* SOC_PB_EPNI_EM_PLDT_H0 */

  Soc_pb_tbls.epni.esem_keyt_h0_tbl.addr.base = 0x00200000;
  Soc_pb_tbls.epni.esem_keyt_h0_tbl.addr.size = 0x01000;
  Soc_pb_tbls.epni.esem_keyt_h0_tbl.addr.width_bits = 10;


  /* SOC_PB_EPNI_EM_PLDT_H1 */

  Soc_pb_tbls.epni.esem_keyt_h1_tbl.addr.base = 0x00210000;
  Soc_pb_tbls.epni.esem_keyt_h1_tbl.addr.size = 0x01000;
  Soc_pb_tbls.epni.esem_keyt_h1_tbl.addr.width_bits = 10;


  /* SOC_PB_EPNI_EM_PLDT_AUX */

  Soc_pb_tbls.epni.esem_keyt_aux_tbl.addr.base = 0x00220000;
  Soc_pb_tbls.epni.esem_keyt_aux_tbl.addr.size = 0x0020;
  Soc_pb_tbls.epni.esem_keyt_aux_tbl.addr.width_bits = 22;


  /* SOC_PB_EPNI_ISEM_PLDT_H0_TBL */

  Soc_pb_tbls.epni.esem_pldt_h0_tbl.addr.base = 0x00230000;
  Soc_pb_tbls.epni.esem_pldt_h0_tbl.addr.size = 0x01000;
  Soc_pb_tbls.epni.esem_pldt_h0_tbl.addr.width_bits = 15;

  /*  SOC_PB_EPNI_ISEM_PLDT_H1_TBL */

  Soc_pb_tbls.epni.esem_pldt_h1_tbl.addr.base = 0x00240000;
  Soc_pb_tbls.epni.esem_pldt_h1_tbl.addr.size = 0x01000;
  Soc_pb_tbls.epni.esem_pldt_h1_tbl.addr.width_bits = 15;

  /* SOC_PB_EPNI_ISEM_PLDT_AUX_TBL */

  Soc_pb_tbls.epni.esem_pldt_aux_tbl.addr.base = 0x00250000;
  Soc_pb_tbls.epni.esem_pldt_aux_tbl.addr.size = 0x0020;
  Soc_pb_tbls.epni.esem_pldt_aux_tbl.addr.width_bits = 15;

  /* SOC_PB_EPNI_ISEM_MGMT_MEM_A_TBL */

  Soc_pb_tbls.epni.esem_mgmt_mem_a_tbl.addr.base = 0x00260000;
  Soc_pb_tbls.epni.esem_mgmt_mem_a_tbl.addr.size = 0x020;
  Soc_pb_tbls.epni.esem_mgmt_mem_a_tbl.addr.width_bits = 36;

  /* SOC_PB_EPNI_ISEM_MGMT_MEM_B_TBL */

  Soc_pb_tbls.epni.esem_mgmt_mem_b_tbl.addr.base = 0x00270000;
  Soc_pb_tbls.epni.esem_mgmt_mem_b_tbl.addr.size = 0x020;
  Soc_pb_tbls.epni.esem_mgmt_mem_b_tbl.addr.width_bits = 36;
}

/* Block tables initialization: CFC */
STATIC void
  soc_pb_tbls_init_CFC(void)
{

  /* RCL2 OFP */
  Soc_pb_tbls.cfc.rcl2_ofp_tbl.addr.base = 0x00000000;
  Soc_pb_tbls.cfc.rcl2_ofp_tbl.addr.size = 0x0020;
  Soc_pb_tbls.cfc.rcl2_ofp_tbl.addr.width_bits = 11;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.rcl2_ofp_tbl.ofp_num), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.rcl2_ofp_tbl.sch_hp_valid), 7, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.rcl2_ofp_tbl.sch_lp_valid), 8, 8);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.rcl2_ofp_tbl.egq_hp_valid), 9, 9);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.rcl2_ofp_tbl.egq_lp_valid), 10, 10);

  /* NIFCLSB2 OFP */
  Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.addr.base = 0x00010000;
  Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.addr.size = 0x0020;
  Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.addr.width_bits = 44;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.ofp_num[0]), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.ofp_num[1]), 13, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.ofp_num[2]), 20, 14);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.ofp_num[3]), 27, 21);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_hp_valid[0]), 28, 28);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_hp_valid[1]), 29, 29);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_hp_valid[2]), 30, 30);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_hp_valid[3]), 31, 31);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_lp_valid[0]), 32, 32);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_lp_valid[1]), 33, 33);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_lp_valid[2]), 34, 34);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.sch_ofp_lp_valid[3]), 35, 35);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_hp_valid[0]), 36, 36);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_hp_valid[1]), 37, 37);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_hp_valid[2]), 38, 38);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_hp_valid[3]), 39, 39);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_lp_valid[0]), 40, 40);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_lp_valid[1]), 41, 41);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_lp_valid[2]), 42, 42);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.nifclsb2_ofp_tbl.egq_ofp_lp_valid[3]), 43, 43);

  /* CALRXA */
  Soc_pb_tbls.cfc.calrxa_tbl.addr.base = 0x00020000;
  Soc_pb_tbls.cfc.calrxa_tbl.addr.size = 0x0200;
  Soc_pb_tbls.cfc.calrxa_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.calrxa_tbl.fc_index), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.calrxa_tbl.fc_dest_sel), 8, 7);

  /* CALRXB */
  Soc_pb_tbls.cfc.calrxb_tbl.addr.base = 0x00030000;
  Soc_pb_tbls.cfc.calrxb_tbl.addr.size = 0x0200;
  Soc_pb_tbls.cfc.calrxb_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.calrxb_tbl.fc_index), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.calrxb_tbl.fc_dest_sel), 8, 7);

  /* Oob0 Sch Map */
  Soc_pb_tbls.cfc.oob0_sch_map_tbl.addr.base = 0x00040000;
  Soc_pb_tbls.cfc.oob0_sch_map_tbl.addr.size = 0x0080;
  Soc_pb_tbls.cfc.oob0_sch_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob0_sch_map_tbl.ofp_hr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob0_sch_map_tbl.lp_ofp_valid), 7, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob0_sch_map_tbl.hp_ofp_valid), 8, 8);

  /* Oob1 Sch Map */
  Soc_pb_tbls.cfc.oob1_sch_map_tbl.addr.base = 0x00050000;
  Soc_pb_tbls.cfc.oob1_sch_map_tbl.addr.size = 0x0080;
  Soc_pb_tbls.cfc.oob1_sch_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob1_sch_map_tbl.ofp_hr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob1_sch_map_tbl.lp_ofp_valid), 7, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.oob1_sch_map_tbl.hp_ofp_valid), 8, 8);

  /* CALTX */
  Soc_pb_tbls.cfc.caltx_tbl.addr.base = 0x00060000;
  Soc_pb_tbls.cfc.caltx_tbl.addr.size = 0x0200;
  Soc_pb_tbls.cfc.caltx_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.caltx_tbl.fc_index), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.caltx_tbl.fc_source_sel), 11, 9);

  /* Ilkn0 CALRX */
  Soc_pb_tbls.cfc.ilkn0_calrx_tbl.addr.base = 0x00070000;
  Soc_pb_tbls.cfc.ilkn0_calrx_tbl.addr.size = 0x0100;
  Soc_pb_tbls.cfc.ilkn0_calrx_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_calrx_tbl.fc_index), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_calrx_tbl.fc_dest_sel), 8, 7);

  /* Ilkn1 CALRX */
  Soc_pb_tbls.cfc.ilkn1_calrx_tbl.addr.base = 0x00080000;
  Soc_pb_tbls.cfc.ilkn1_calrx_tbl.addr.size = 0x0100;
  Soc_pb_tbls.cfc.ilkn1_calrx_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_calrx_tbl.fc_index), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_calrx_tbl.fc_dest_sel), 8, 7);

  /* Ilkn0 Sch Map */
  Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.addr.base = 0x00090000;
  Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.addr.size = 0x0080;
  Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.ofp_hr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.lp_ofp_valid), 7, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_sch_map_tbl.hp_ofp_valid), 8, 8);

  /* Ilkn1 Sch Map */
  Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.addr.base = 0x000a0000;
  Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.addr.size = 0x0080;
  Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.addr.width_bits = 9;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.ofp_hr), 6, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.lp_ofp_valid), 7, 7);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_sch_map_tbl.hp_ofp_valid), 8, 8);

  /* ILKN0 CALTX */
  Soc_pb_tbls.cfc.ilkn0_caltx_tbl.addr.base = 0x000b0000;
  Soc_pb_tbls.cfc.ilkn0_caltx_tbl.addr.size = 0x0100;
  Soc_pb_tbls.cfc.ilkn0_caltx_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_caltx_tbl.fc_index), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn0_caltx_tbl.fc_source_sel), 11, 9);

  /* ILKN1 CALTX */
  Soc_pb_tbls.cfc.ilkn1_caltx_tbl.addr.base = 0x000c0000;
  Soc_pb_tbls.cfc.ilkn1_caltx_tbl.addr.size = 0x0100;
  Soc_pb_tbls.cfc.ilkn1_caltx_tbl.addr.width_bits = 12;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_caltx_tbl.fc_index), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.cfc.ilkn1_caltx_tbl.fc_source_sel), 11, 9);
}

/* Block tables initialization: SCH */
STATIC void
  soc_pb_tbls_init_SCH(void)
{
  uint32
    fld_idx = 0;

  /* Scheduler Credit Generation Calendar (CAL) */
  Soc_pb_tbls.sch.cal_tbl.addr.base = 0x40000000;
  Soc_pb_tbls.sch.cal_tbl.addr.size = 0xfc00;
  Soc_pb_tbls.sch.cal_tbl.addr.width_bits = 7;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.cal_tbl.hrsel), 6, 0);

  /* Device Rate Memory (DRM) */
  Soc_pb_tbls.sch.drm_tbl.addr.base = 0x40010000;
  Soc_pb_tbls.sch.drm_tbl.addr.size = 0x0128;
  Soc_pb_tbls.sch.drm_tbl.addr.width_bits = 18;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.drm_tbl.device_rate), 17, 0);

  /* Dual Shaper Memory (DSM) */
  Soc_pb_tbls.sch.dsm_tbl.addr.base = 0x40020000;
  Soc_pb_tbls.sch.dsm_tbl.addr.size = 0x0200;
  Soc_pb_tbls.sch.dsm_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.dsm_tbl.dual_shaper_ena), 15, 0);

  /* Flow Descriptor Memory Static (FDMS) */
  Soc_pb_tbls.sch.fdms_tbl.addr.base = 0x40030000;
  Soc_pb_tbls.sch.fdms_tbl.addr.size = 0xe000;
  Soc_pb_tbls.sch.fdms_tbl.addr.width_bits = 23;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fdms_tbl.sch_number), 13, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fdms_tbl.cos), 21, 14);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fdms_tbl.hrsel_dual), 22, 22);

  /* Shaper Descriptor Memory Static (SHDS) */
  Soc_pb_tbls.sch.shds_tbl.addr.base = 0x40040000;
  Soc_pb_tbls.sch.shds_tbl.addr.size = 0x7000;
  Soc_pb_tbls.sch.shds_tbl.addr.width_bits = 42;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.peak_rate_man_even), 5, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.peak_rate_exp_even), 9, 6);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.max_burst_even), 18, 10);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.slow_rate2_sel_even), 19, 19);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.peak_rate_man_odd), 25, 20);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.peak_rate_exp_odd), 29, 26);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.max_burst_odd), 38, 30);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.slow_rate2_sel_odd), 39, 39);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.max_burst_update_even), 40, 40);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shds_tbl.max_burst_update_odd), 41, 41);

  /* Scheduler Enable Memory (SEM) */
  Soc_pb_tbls.sch.sem_tbl.addr.base = 0x40050000;
  Soc_pb_tbls.sch.sem_tbl.addr.size = 0x0800;
  Soc_pb_tbls.sch.sem_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sem_tbl.sch_enable), 7, 0);

  /* Flow Sub-Flow (FSF) */
  Soc_pb_tbls.sch.fsf_tbl.addr.base = 0x40060000;
  Soc_pb_tbls.sch.fsf_tbl.addr.size = 0x0700;
  Soc_pb_tbls.sch.fsf_tbl.addr.width_bits = 16;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fsf_tbl.sfenable), 15, 0);

  /* Flow Group Memory (FGM) */
  Soc_pb_tbls.sch.fgm_tbl.addr.base = 0x40070000;
  Soc_pb_tbls.sch.fgm_tbl.addr.size = 0x0800;
  Soc_pb_tbls.sch.fgm_tbl.addr.width_bits = 16;
  for (fld_idx = 0; fld_idx < SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
    soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fgm_tbl.flow_group[fld_idx]), (uint8)(2*fld_idx+1), (uint8)(2*fld_idx));
  }

  /* HR-Scheduler-Configuration (SHC) */
  Soc_pb_tbls.sch.shc_tbl.addr.base = 0x40080000;
  Soc_pb_tbls.sch.shc_tbl.addr.size = 0x0100;
  Soc_pb_tbls.sch.shc_tbl.addr.width_bits = 4;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shc_tbl.hrmode), 1, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.shc_tbl.hrmask_type), 3, 2);

  /* CL-Schedulers Configuration (SCC) */
  Soc_pb_tbls.sch.scc_tbl.addr.base = 0x40090000;
  Soc_pb_tbls.sch.scc_tbl.addr.size = 0x2000;
  Soc_pb_tbls.sch.scc_tbl.addr.width_bits = 8;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.scc_tbl.clsch_type), 7, 0);

  /* CL-Schedulers Type (SCT) */
  Soc_pb_tbls.sch.sct_tbl.addr.base = 0x400a0000;
  Soc_pb_tbls.sch.sct_tbl.addr.size = 0x0100;
  Soc_pb_tbls.sch.sct_tbl.addr.width_bits = 48;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.clconfig), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.af0_inv_weight), 13, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.af1_inv_weight), 23, 14);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.af2_inv_weight), 33, 24);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.af3_inv_weight), 43, 34);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.wfqmode), 45, 44);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.enh_clen), 46, 46);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.sct_tbl.enh_clsphigh), 47, 47);

  /* Flow to Queue Mapping (FQM) */
  Soc_pb_tbls.sch.fqm_tbl.addr.base = 0x400b0000;
  Soc_pb_tbls.sch.fqm_tbl.addr.size = 0x3800;
  Soc_pb_tbls.sch.fqm_tbl.addr.width_bits = 18;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fqm_tbl.base_queue_num), 12, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fqm_tbl.sub_flow_mode), 13, 13);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.fqm_tbl.flow_slow_enable), 17, 14);

  /* Flow to FIP Mapping (FFM) */
  Soc_pb_tbls.sch.ffm_tbl.addr.base = 0x400c0000;
  Soc_pb_tbls.sch.ffm_tbl.addr.size = 0x1c00;
  Soc_pb_tbls.sch.ffm_tbl.addr.width_bits = 11;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.ffm_tbl.device_number), 10, 0);

  /* Token Memory Controller (TMC) */
  Soc_pb_tbls.sch.soc_tmctbl.addr.base = 0x40100000;
  Soc_pb_tbls.sch.soc_tmctbl.addr.size = 0xe000;
  Soc_pb_tbls.sch.soc_tmctbl.addr.width_bits = 10;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.soc_tmctbl.token_count), 8, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.soc_tmctbl.slow_status), 9, 9);

  /* Port Queue Size (PQS) */
  Soc_pb_tbls.sch.pqs_tbl.addr.base = 0x401d0000;
  Soc_pb_tbls.sch.pqs_tbl.addr.size = 0x0051;
  Soc_pb_tbls.sch.pqs_tbl.addr.width_bits = 21;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.pqs_tbl.max_qsz), 3, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.pqs_tbl.flow_id), 19, 4);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.pqs_tbl.aging_bit), 20, 20);

  /* Scheduler Init */
  Soc_pb_tbls.sch.scheduler_init_tbl.addr.base = 0x41000000;
  Soc_pb_tbls.sch.scheduler_init_tbl.addr.size = 0x0001;
  Soc_pb_tbls.sch.scheduler_init_tbl.addr.width_bits = 1;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.scheduler_init_tbl.schinit), 0, 0);

  /* Force Status Message */
  Soc_pb_tbls.sch.force_status_message_tbl.addr.base = 0x42000000;
  Soc_pb_tbls.sch.force_status_message_tbl.addr.size = 0x0001;
  Soc_pb_tbls.sch.force_status_message_tbl.addr.width_bits = 20;
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.force_status_message_tbl.message_flow_id), 15, 0);
  soc_pb_tbl_fld_set( &(Soc_pb_tbls.sch.force_status_message_tbl.message_type), 19, 16);
}
/*****************************************************
*NAME
*  soc_pb_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Pb tables database.
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
  soc_pb_tbls_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TBLS_INIT);

  if (Soc_pb_tbls_initialized == TRUE)
  {
    goto exit;
  }

  soc_pb_tbls_init_OLP();
  soc_pb_tbls_init_IRE();
  soc_pb_tbls_init_IDR();
  soc_pb_tbls_init_IRR();
  soc_pb_tbls_init_IHP();
  soc_pb_tbls_init_QDR();
  soc_pb_tbls_init_MMU();
  soc_pb_tbls_init_IQM();
  soc_pb_tbls_init_IPS();
  soc_pb_tbls_init_IPT();
  soc_pb_tbls_init_DPI();
  soc_pb_tbls_init_FDT();
  soc_pb_tbls_init_NBI();
  soc_pb_tbls_init_RTP();
  soc_pb_tbls_init_EGQ();
  soc_pb_tbls_init_IHB();
  soc_pb_tbls_init_EPNI();
  soc_pb_tbls_init_CFC();
  soc_pb_tbls_init_SCH();

  Soc_pb_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tbls_init",0,0);
}
#include <soc/dpp/SAND/Utils/sand_footer.h>

