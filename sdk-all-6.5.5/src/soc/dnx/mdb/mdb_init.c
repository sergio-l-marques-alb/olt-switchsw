/*! \file mdb_init.c
 *
 * Contains all of the MDB initialization sequences.
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

#include <soc/dnx/mdb.h>
#include "mdb_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

mdb_db_info_t mdb_db_infos[MDB_NOF_PHYSICAL_TABLES];

/*
 *  mdb_default_connection_array:
 *  Physical DB	A0	A1	A2	A3	A4	A5	A6	A7	A8	A9	A10	A11	A12	A13	A14	A15	B0	B1	B2	B3	B4	B5	B6	B7
 *  ISEM_1	    1	1	1	1																	1			1
 *  INLIF_1	    																1	1
 *  IVSI	    						1	1
 *  ISEM_2	    								1	1	1	1							1	1
 *  INLIF_2	    												1	1
 *  ISEM_3	    																1	1	1	1
 *  INLIF_3	    														1	1
 *  LEM	1	    1	1	1	1	1	1	1	1	1				1					1	1
 *  ADS_1	    										1
 *  ADS_2	    											1
 *  KAPS_1	    	1		1	1	1			1	1	1	1	1	1	1	1					1			1
 *  KAPS_2	    1		1				1	1			1	1	1	1	1	1					1	1	1	1
 *  IOEM_0	    							1	1														1	1
 *  IOEM_1	    								1		1										1	1
 *  MAP		    							1	1
 *  FEC_1	    1	1
 *  FEC_2	    		1	1
 *  FEC_3	    				1	1
 *  MC_ID	    																				1	1	1	1
 *  GLEM_0	    									1	1	1	1
 *  GLEM_1	    				1	1	1	1
 *  LL_1	    																		1
 *  LL_2	    																			1
 *  LL_3	    																		1
 *  LL_4	    																			1
 *  EEDB_1	    1	1
 *  EEDB_2	    		1	1
 *  EEDB_3	    				1	1
 *  EEDB_4	    						1	1
 *  EOEM_0	    														1	1			1	1
 *  EOEM_1	    														1	1			1	1
 *  ESEM	    									1	1	1	1
 *  EVSI	    										1	1
 *  EXEM_1	    								1	1							1	1
 *  EXEM_2	    																1	1				1	1
 *  EXEM_3	    																1	1				1	1
 *  EXEM_4	    																1	1				1	1
 *  RMEP	    																		1	1	1			1
 *
 *
 * An array that holds (MDB_NOF_MACRO_A + MDB_NOF_MACRO_B) entries for each physical DB.
 * If a specific MACRO is valid for the DB, the corresponding array cell will describe how many clusters are allocated to it.
 * The DBs appearance in the array must be aligned to physical_tables_e.
 * The clusters within each macro are allocated sequentially on a first-come first-serve basis.
 */
static uint8 mdb_default_connection_array[MDB_NOF_PHYSICAL_TABLES * (MDB_NOF_MACRO_A + MDB_NOF_MACRO_B)] = {
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1
};

shr_error_e
mdb_init(
  int unit)
{
  int db_index, macro_index, cluster_counter;

  /*
   * The counters are used to allocate the clusters in each macro to specific DBs
   */
  uint8 macro_a_counter[MDB_NOF_MACRO_A];
  uint8 macro_b_counter[MDB_NOF_MACRO_B];

  SHR_FUNC_INIT_VARS(unit);

  sal_memset(macro_a_counter, 0x0, MDB_NOF_MACRO_A * sizeof(macro_a_counter[0]));
  sal_memset(macro_b_counter, 0x0, MDB_NOF_MACRO_B * sizeof(macro_b_counter[0]));

  for (db_index = 0; db_index < MDB_NOF_PHYSICAL_TABLES; db_index++)
  {
    for (macro_index = 0; macro_index < (MDB_NOF_MACRO_A + MDB_NOF_MACRO_B); macro_index++)
    {
      /*
       * the counter value is the amount of associated clusters in the macro
       */
      for (cluster_counter = 0;
           cluster_counter <
           mdb_default_connection_array[(db_index * (MDB_NOF_MACRO_A + MDB_NOF_MACRO_B)) + macro_index];
           cluster_counter++)
      {
        /*
         * Update the appropriate address in the DB array, MACRO B array is right after MACRO A
         */
        if (macro_index < MDB_NOF_MACRO_A)
        {
          /*
           * Update Macro A addresses
           */
          mdb_db_infos[db_index].macro_a_clusters_addr[mdb_db_infos[db_index].macro_a_clusters_num] =
            (macro_index * MDB_NOF_MACRO_A_CLUSTERS) + macro_a_counter[macro_index];
          macro_a_counter[macro_index]++;
          mdb_db_infos[db_index].macro_a_clusters_num++;
        }
        else
        {
          /*
           * Update Macro B addresses
           */
          mdb_db_infos[db_index].macro_b_clusters_addr[mdb_db_infos[db_index].macro_b_clusters_num] =
            ((macro_index - MDB_NOF_MACRO_A) * MDB_NOF_MACRO_B_CLUSTERS) +
            macro_b_counter[(macro_index - MDB_NOF_MACRO_A)];
          macro_b_counter[(macro_index - MDB_NOF_MACRO_A)]++;
          mdb_db_infos[db_index].macro_b_clusters_num++;
        }
      }
    }
  }

  /*
   * Initialize the mapping between DBAL physical tables and MDB physical tables
   */
  for (db_index = 0; db_index < DBAL_NOF_PHYSICAL_TABLES; db_index++)
  {
    switch (db_index)
    {
      case DBAL_PHYSICAL_TABLE_TCAM:
        mdb_direct_dbal_to_mdb[db_index] = MDB_NOF_PHYSICAL_TABLES;
        break;

      case DBAL_PHYSICAL_TABLE_LPM:
        mdb_direct_dbal_to_mdb[db_index] = MDB_NOF_PHYSICAL_TABLES;
        break;

      case DBAL_PHYSICAL_TABLE_ISEM_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_ISEM_1;
        break;

      case DBAL_PHYSICAL_TABLE_INLIF_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_INLIF_1;
        break;

      case DBAL_PHYSICAL_TABLE_IVSI:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_IVSI;
        break;

      case DBAL_PHYSICAL_TABLE_ISEM_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_ISEM_2;
        break;

      case DBAL_PHYSICAL_TABLE_INLIF_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_INLIF_2;
        break;

      case DBAL_PHYSICAL_TABLE_ISEM_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_ISEM_3;
        break;

      case DBAL_PHYSICAL_TABLE_INLIF_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_INLIF_3;
        break;

      case DBAL_PHYSICAL_TABLE_LEM:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_LEM;
        break;

      case DBAL_PHYSICAL_TABLE_IOEM_0:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_IOEM_0;
        break;

      case DBAL_PHYSICAL_TABLE_IOEM_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_IOEM_1;
        break;

      case DBAL_PHYSICAL_TABLE_MAP:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_MAP;
        break;

      case DBAL_PHYSICAL_TABLE_FEC_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_FEC_1;
        break;

      case DBAL_PHYSICAL_TABLE_FEC_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_FEC_2;
        break;

      case DBAL_PHYSICAL_TABLE_FEC_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_FEC_3;
        break;

      case DBAL_PHYSICAL_TABLE_MC_ID:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_MC_ID;
        break;

      case DBAL_PHYSICAL_TABLE_GLEM_0:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_GLEM_0;
        break;

      case DBAL_PHYSICAL_TABLE_GLEM_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_GLEM_1;
        break;

      case DBAL_PHYSICAL_TABLE_LL_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_LL_1;
        break;

      case DBAL_PHYSICAL_TABLE_LL_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_LL_2;
        break;

      case DBAL_PHYSICAL_TABLE_LL_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_LL_3;
        break;

      case DBAL_PHYSICAL_TABLE_LL_4:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_LL_4;
        break;

      case DBAL_PHYSICAL_TABLE_EEDB_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EEDB_1;
        break;

      case DBAL_PHYSICAL_TABLE_EEDB_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EEDB_2;
        break;

      case DBAL_PHYSICAL_TABLE_EEDB_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EEDB_3;
        break;

      case DBAL_PHYSICAL_TABLE_EEDB_4:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EEDB_4;
        break;

      case DBAL_PHYSICAL_TABLE_EOEM_0:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EOEM_0;
        break;

      case DBAL_PHYSICAL_TABLE_EOEM_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EOEM_1;
        break;

      case DBAL_PHYSICAL_TABLE_ESEM:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_ESEM;
        break;

      case DBAL_PHYSICAL_TABLE_EVSI:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EVSI;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_1:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EXEM_1;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_2:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EXEM_2;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_3:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EXEM_3;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_4:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_EXEM_4;
        break;

      case DBAL_PHYSICAL_TABLE_RMEP:
        mdb_direct_dbal_to_mdb[db_index] = MDB_PHYSICAL_TABLE_RMEP;
        break;

      default:
        mdb_direct_dbal_to_mdb[db_index] = MDB_NOF_PHYSICAL_TABLES;
    }
  }

  /*
   * Initialize the mapping between DBAL EM tables and MDB EM registers
   */
  for (db_index = 0; db_index < DBAL_NOF_PHYSICAL_TABLES; db_index++)
  {
    switch (db_index)
    {
      case DBAL_PHYSICAL_TABLE_ISEM_1:
        mdb_em_dbal_to_mdb[db_index] = MDB_ISEM_1m;
        break;

      case DBAL_PHYSICAL_TABLE_ISEM_2:
        mdb_em_dbal_to_mdb[db_index] = MDB_ISEM_2m;
        break;

      case DBAL_PHYSICAL_TABLE_ISEM_3:
        mdb_em_dbal_to_mdb[db_index] = MDB_ISEM_3m;
        break;

      case DBAL_PHYSICAL_TABLE_LEM:
        mdb_em_dbal_to_mdb[db_index] = MDB_LEMm;
        break;

      case DBAL_PHYSICAL_TABLE_IOEM_0:
        mdb_em_dbal_to_mdb[db_index] = MDB_IOEM_0m;
        break;

      case DBAL_PHYSICAL_TABLE_IOEM_1:
        mdb_em_dbal_to_mdb[db_index] = MDB_IOEM_1m;
        break;

      case DBAL_PHYSICAL_TABLE_GLEM_0:
        mdb_em_dbal_to_mdb[db_index] = MDB_GLEM_0m;
        break;

      case DBAL_PHYSICAL_TABLE_GLEM_1:
        mdb_em_dbal_to_mdb[db_index] = MDB_GLEM_1m;
        break;

      case DBAL_PHYSICAL_TABLE_EOEM_0:
        mdb_em_dbal_to_mdb[db_index] = MDB_EOEM_0m;
        break;

      case DBAL_PHYSICAL_TABLE_EOEM_1:
        mdb_em_dbal_to_mdb[db_index] = MDB_EOEM_1m;
        break;

      case DBAL_PHYSICAL_TABLE_ESEM:
        mdb_em_dbal_to_mdb[db_index] = MDB_ESEMm;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_1:
        mdb_em_dbal_to_mdb[db_index] = MDB_EXEM_1m;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_2:
        mdb_em_dbal_to_mdb[db_index] = MDB_EXEM_2m;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_3:
        mdb_em_dbal_to_mdb[db_index] = MDB_EXEM_3m;
        break;

      case DBAL_PHYSICAL_TABLE_EXEM_4:
        mdb_em_dbal_to_mdb[db_index] = MDB_EXEM_4m;
        break;

      case DBAL_PHYSICAL_TABLE_RMEP:
        mdb_em_dbal_to_mdb[db_index] = MDB_RMEPm;
        break;

      default:
        mdb_em_dbal_to_mdb[db_index] = INVALIDm;
    }
  }

  SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_capacity(
  int unit,
  dbal_physical_table_def_t * dbal_physical_table,
  int *capacity)
{
  mdb_physical_tables_e physical_table = mdb_direct_dbal_to_mdb[dbal_physical_table->physical_db_type];
  uint32 entries_per_row;
  SHR_FUNC_INIT_VARS(unit);

  if (physical_table >= MDB_NOF_PHYSICAL_TABLES)
  {
    
    /*
     * SHR_IF_ERR_EXIT(SHR_E_PARAM);
     */
    *capacity = 100000;
  }
  else
  {
    if (dbal_physical_table->max_entry_size == 0)
    {
      *capacity = 0;
    }
    else
    {
      SHR_IF_ERR_EXIT(mdb_direct_table_calc_entries_per_row(unit, dbal_physical_table, &entries_per_row));

      *capacity = ((mdb_db_infos[physical_table].macro_a_clusters_num * MDB_NOF_MACRO_A_ROWS_CLUSTER) +
                   (mdb_db_infos[physical_table].macro_b_clusters_num * MDB_NOF_MACRO_B_ROWS_CLUSTER)) *
        entries_per_row;
    }
  }

exit:
  SHR_FUNC_EXIT;
}
