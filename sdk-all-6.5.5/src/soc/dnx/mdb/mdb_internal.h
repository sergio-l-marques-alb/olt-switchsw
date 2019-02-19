/*! \file mdb.h
 *
 * Contains all of the internal MDB defines and globals.
 *
 */
/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

#ifndef MDB_H_INTERNAL_INCLUDED
/* { */
#  define MDB_H_INTERNAL_INCLUDED

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal_defines.h>
#include <soc/dnx/dbal_structures.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>

/*
 * The MDB consists of 16 Macro-A and 8 Macro-B
 * Each Macro consists of 2 buckets, 4 clusters per bucket
 * Macro-A cluster is 16k rows, Macro-B cluster is 8k rows
 */
#define MDB_NOF_MACRO_A                         16
#define MDB_NOF_MACRO_A_ROWS_CLUSTER           (16*1024)
#define MDB_NOF_MACRO_A_CLUSTERS                8
#define MDB_NOF_MACRO_A_CLUSTER_IN_BUCKET       4

#define MDB_NOF_MACRO_B                         8
#define MDB_NOF_MACRO_B_ROWS_CLUSTER           (8*1024)
#define MDB_NOF_MACRO_B_CLUSTERS                8
#define MDB_NOF_MACRO_B_CLUSTER_IN_BUCKET       4

#define MDB_NOF_MACRO_ROW_BITS                  120

/*#define MDB_DEBUG_PRINTS                        0*/

typedef enum
{
  MDB_DB_EM,
  MDB_DB_DIRECT,
  MDB_DB_TCAM,
  MDB_DB_KAPS,

  MDB_NOF_DB_TYPES
} mdb_db_type_e;

typedef enum
{
  MDB_PHYSICAL_TABLE_ISEM_1,
  MDB_PHYSICAL_TABLE_INLIF_1,
  MDB_PHYSICAL_TABLE_IVSI,
  MDB_PHYSICAL_TABLE_ISEM_2,
  MDB_PHYSICAL_TABLE_INLIF_2,
  MDB_PHYSICAL_TABLE_ISEM_3,
  MDB_PHYSICAL_TABLE_INLIF_3,
  MDB_PHYSICAL_TABLE_LEM,
  MDB_PHYSICAL_TABLE_ADS_1,
  MDB_PHYSICAL_TABLE_ADS_2,
  MDB_PHYSICAL_TABLE_KAPS_1,
  MDB_PHYSICAL_TABLE_KAPS_2,
  MDB_PHYSICAL_TABLE_IOEM_0,
  MDB_PHYSICAL_TABLE_IOEM_1,
  MDB_PHYSICAL_TABLE_MAP,
  MDB_PHYSICAL_TABLE_FEC_1,
  MDB_PHYSICAL_TABLE_FEC_2,
  MDB_PHYSICAL_TABLE_FEC_3,
  MDB_PHYSICAL_TABLE_MC_ID,
  MDB_PHYSICAL_TABLE_GLEM_0,
  MDB_PHYSICAL_TABLE_GLEM_1,
  MDB_PHYSICAL_TABLE_LL_1,
  MDB_PHYSICAL_TABLE_LL_2,
  MDB_PHYSICAL_TABLE_LL_3,
  MDB_PHYSICAL_TABLE_LL_4,
  MDB_PHYSICAL_TABLE_EEDB_1,
  MDB_PHYSICAL_TABLE_EEDB_2,
  MDB_PHYSICAL_TABLE_EEDB_3,
  MDB_PHYSICAL_TABLE_EEDB_4,
  MDB_PHYSICAL_TABLE_EOEM_0,
  MDB_PHYSICAL_TABLE_EOEM_1,
  MDB_PHYSICAL_TABLE_ESEM,
  MDB_PHYSICAL_TABLE_EVSI,
  MDB_PHYSICAL_TABLE_EXEM_1,
  MDB_PHYSICAL_TABLE_EXEM_2,
  MDB_PHYSICAL_TABLE_EXEM_3,
  MDB_PHYSICAL_TABLE_EXEM_4,
  MDB_PHYSICAL_TABLE_RMEP,

  MDB_NOF_PHYSICAL_TABLES
} mdb_physical_tables_e;

typedef struct
{
  /*
   * mdb_db_type_e db_type;
   */

  /*
   * Number of type A clusters associated with this DB
   */
  uint8 macro_a_clusters_num;
  /*
   * Addressing is in clusters from 0 to MDB_NOF_MACRO_A * MDB_NOF_MACRO_A_CLUSTERS
   * This identifies a single cluster based on its index counting from the first cluster in its MACRO
   */
  uint8 macro_a_clusters_addr[MDB_NOF_MACRO_A];

  /*
   * Number of type B clusters associated with this DB
   */
  uint8 macro_b_clusters_num;
  /*
   * Addressing is in clusters from 0 to MDB_NOF_MACRO_B * MDB_NOF_MACRO_B_CLUSTERS
   * This identifies a single cluster based on its index counting from the first cluster in its MACRO
   */
  uint8 macro_b_clusters_addr[MDB_NOF_MACRO_B];

} mdb_db_info_t;

extern mdb_db_info_t mdb_db_infos[MDB_NOF_PHYSICAL_TABLES];
/* Mapping between DBAL physical tables and MDB physical tables */
extern mdb_physical_tables_e mdb_direct_dbal_to_mdb[DBAL_NOF_PHYSICAL_TABLES];
/* Mapping between DBAL EM tables and MDB EM registers */
extern soc_mem_t mdb_em_dbal_to_mdb[DBAL_NOF_PHYSICAL_TABLES];

/* Calculate the number of entries per row  */
shr_error_e mdb_direct_table_calc_entries_per_row(
  int unit,
  dbal_physical_table_def_t * dbal_physical_table,
  uint32 * entries_per_row);

/* returns whether or not the the payload is full or partially masked  */
shr_error_e mdb_direct_is_payload_masked(
  int unit,
  dbal_physical_entry_t * entry,
  int *is_masked);

/* } */
#endif /* !MDB_H_INTERNAL_INCLUDED */
