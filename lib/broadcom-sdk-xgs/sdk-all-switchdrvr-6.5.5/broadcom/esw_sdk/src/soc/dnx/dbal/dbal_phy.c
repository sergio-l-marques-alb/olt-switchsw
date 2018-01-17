/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX
#include <shared/bsl.h>
#include "dbal.h"
#include <soc/dnx/mdb.h>


static dbal_physical_mngr_info_t physical_table_mngr = 

{
    {
        {{0}},
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE TCAM",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_TCAM,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ NULL,
            /*.entry_get               =*/ NULL,
            /*.entry_delete            =*/ NULL,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LPM",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LPM,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ NULL,
            /*.entry_get               =*/ NULL,
            /*.entry_delete            =*/ NULL,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE ISEM 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_ISEM_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE INLIF 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_INLIF_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE IVSI",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_IVSI,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE ISEM 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_ISEM_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE INLIF 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_INLIF_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE ISEM 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_ISEM_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE INLIF 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_INLIF_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LEM",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LEM,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE IOEM 0",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_IOEM_0,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE IOEM 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_IOEM_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE MAP",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_MAP,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE FEC 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_FEC_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE FEC 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_FEC_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE FEC 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_FEC_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE MC ID",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_MC_ID,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE GLEM 0",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_GLEM_0,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE GLEM 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_GLEM_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LL 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LL_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LL 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LL_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LL 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LL_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE LL 4",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_LL_4,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EEDB 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EEDB_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EEDB 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EEDB_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EEDB 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EEDB_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EEDB 4",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EEDB_4,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EOEM 0",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EOEM_0,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EOEM 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EOEM_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE ESEM",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_ESEM,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EVSI",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EVSI,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_direct_table_entry_add,
            /*.entry_get               =*/ mdb_direct_table_entry_get,
            /*.entry_delete            =*/ mdb_direct_table_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EXEM 1",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EXEM_1,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EXEM 2",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EXEM_2,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EXEM 3",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EXEM_3,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE EXEM 4",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_EXEM_4,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        },
        {
            /**************************************** Database information ******************************************/
            /*.physical_name           =*/ "MDB TABLE RMEP",
            /*.physical_db_type        =*/ DBAL_PHYSICAL_TABLE_RMEP,
            /*.max_entry_size        =*/ 0,
            /*.max_capacity            =*/ 0,
            /*.nof_entries             =*/ 0,
            /**************************************** Database operations ********************************************/
            /*.entry_set               =*/ mdb_em_entry_add,
            /*.entry_get               =*/ mdb_em_entry_get,
            /*.entry_delete            =*/ mdb_em_entry_delete,
            /*.table_clear             =*/ NULL,
            /*.table_default_values_set=*/ NULL,
            /*.table_init              =*/ NULL
            /*********************************************************************************************************/
        }
    }
};

/* general EM access functions*/

shr_error_e dbal_phy_entry_print(int unit, dbal_physical_entry_t* entry)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("key received: ")));

    for (i = 0; i < (entry->key_size/32)-1; i++) {
        LOG_CLI((BSL_META("%x:"),entry->key[i]));
    }
    LOG_CLI((BSL_META("%x "),entry->key[i]));

    LOG_CLI((BSL_META("payload received: ")));

    for (i = 0; i < (entry->payload_size/32)-1; i++) {
        LOG_CLI((BSL_META("%x:"),entry->payload[i]));
    }
    LOG_CLI((BSL_META("%x\n"),entry->payload[i]));

    SHR_FUNC_EXIT;
}

shr_error_e dbal_em_entry_add(int unit, dbal_physical_tables_e physical_table, dbal_physical_entry_t *entry)
{

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI(("\ndbal_em_entry_add:: missing implementation "));
    SHR_IF_ERR_EXIT(dbal_phy_entry_print(unit,entry));

exit:
    SHR_FUNC_EXIT;   
}

shr_error_e dbal_tcam_entry_add(int unit, dbal_physical_tables_e physical_table, dbal_physical_entry_t *entry)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LOG_MODULE,"\n missing implementation %s%s%s%s\n", EMPTY,EMPTY,EMPTY,EMPTY);
    SHR_IF_ERR_EXIT(dbal_phy_entry_print(unit,entry));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dbal_lpm_entry_add(int unit, dbal_physical_tables_e physical_table, dbal_physical_entry_t *entry)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LOG_MODULE, "\n missing implementation %s%s%s%s\n", EMPTY,EMPTY,EMPTY,EMPTY);
    SHR_IF_ERR_EXIT(dbal_phy_entry_print(unit,entry));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dbal_physical_table_init(int unit)
{
    int i;
    dbal_physical_table_def_t* PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init(unit));

    for (i = 0; i < DBAL_NOF_PHYSICAL_TABLES; i++) {

        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, i, &PhysicalTable));

        if(physical_table_mngr.physical_tables[i].physical_db_type != i){                
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_init %d\n", i);
        }

        SHR_IF_ERR_EXIT(dbal_max_entry_size_get(unit, i, &physical_table_mngr.physical_tables[i].max_entry_size));

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, PhysicalTable, &physical_table_mngr.physical_tables[i].max_capacity));

        if (physical_table_mngr.physical_tables[i].table_init) {
            physical_table_mngr.physical_tables[i].table_init(unit);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_physical_table_get(int unit, dbal_physical_tables_e physical_table_id, dbal_physical_table_def_t** physical_table)
{
    SHR_FUNC_INIT_VARS(unit);

    if (physical_table_id > DBAL_NOF_PHYSICAL_TABLES) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR dbal_physical_table_get %d\n", physical_table_id);
    }
    *physical_table = &(physical_table_mngr.physical_tables[physical_table_id]);
    
exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_phy_table_entry_get(int unit, dbal_physical_tables_e physical_db_id, uint32 app_id, dbal_physical_entry_t* phy_entry)
{
    dbal_physical_table_def_t* PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    if(PhysicalTable->entry_get == NULL){
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR missing implementation for %s\n", PhysicalTable->physical_name);
    }

    SHR_IF_ERR_EXIT(PhysicalTable->entry_get(unit, physical_db_id, app_id, phy_entry));
    
exit:
    SHR_FUNC_EXIT;
}

shr_error_e dbal_phy_table_entry_delete(
   int unit, dbal_physical_tables_e physical_db_id, uint32 app_id, dbal_physical_entry_t * phy_entry)
{
    dbal_physical_table_def_t* PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    if(PhysicalTable->entry_delete == NULL){
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n",
                                  PhysicalTable->physical_name);
    }

    SHR_IF_ERR_EXIT(PhysicalTable->entry_delete(unit, physical_db_id, app_id, phy_entry));
    PhysicalTable->nof_entries--;
    
exit:
    SHR_FUNC_EXIT;
}


shr_error_e dbal_phy_table_entry_add(int unit, dbal_physical_tables_e physical_db_id, uint32 app_id, dbal_physical_entry_t* phy_entry)
{
    dbal_physical_table_def_t* PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    if (PhysicalTable->nof_entries + 1 > PhysicalTable->max_capacity) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "physical not found %d %d\n", PhysicalTable->nof_entries, PhysicalTable->max_capacity);
    }

    if(PhysicalTable->entry_add == NULL){
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "missing implementation %s\n", PhysicalTable->physical_name);
    }

    SHR_IF_ERR_EXIT(PhysicalTable->entry_add(unit, physical_db_id, app_id, phy_entry));
    PhysicalTable->nof_entries++;

    
exit:
    SHR_FUNC_EXIT;
}


/* Physical DB operations */

shr_error_e dbal_physical_db_info_print(int unit, dbal_physical_tables_e physical_db_id)
{

    dbal_physical_table_def_t* PhysicalTable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, physical_db_id, &PhysicalTable));

    LOG_CLI((BSL_META("Physical Table %-20s\n"),PhysicalTable->physical_name));
    LOG_CLI((BSL_META("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")));    
    LOG_CLI((BSL_META("\tMax capacity: %d, Entries added: %d\n"),PhysicalTable->max_capacity, PhysicalTable->nof_entries));

exit:
    SHR_FUNC_EXIT;
}
