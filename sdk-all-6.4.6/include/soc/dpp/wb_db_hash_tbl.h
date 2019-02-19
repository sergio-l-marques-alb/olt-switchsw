/*
* $Id: wb_db_hash_tbl.h,v 1.5 Broadcom SDK $
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
*
* 
*/ 
#ifndef _SOC_WB_DB_HASH_TBL_H_
#define _SOC_WB_DB_HASH_TBL_H_

#ifdef BCM_WARM_BOOT_SUPPORT   

#include <shared/hash_tbl.h>

/* fill saved_data_ptr with the data from orig_data (orig_data may be a complex data structure) */
typedef int (*dpp_hash_tbl_save_fn)(void *orig_data, uint8 *saved_data_ptr);

/* return pointer to the data to be saved to the hashed table based of the data saved in wb*/
typedef int (*dpp_hash_tbl_load_fn)(uint8 *wb_data_ptr, void **data_ptr);

typedef struct dpp_wb_hash_tbl_data_s {
    int                      is_initialized;
    int                      max_nof_values; /* same as in dpp_wb_engine */
    int                      max_data_size;
    int                      nof_used_values;
    int                      key_size;
    int                      wb_var_idx;     /*dpp_wb_engine var indx*/
    uint8                   *data;
    uint8                   *keys;
    shr_htb_hash_table_t     ht;             /*pointer to the actual hash table*/
    dpp_hash_tbl_save_fn     save_fn;
    dpp_hash_tbl_load_fn     load_fn;
} dpp_wb_hash_tbl_data_t;

typedef enum
{
    SOC_DPP_WB_HASH_TBLS_OAM_MA_NAME,
	SOC_DPP_WB_HASH_TBLS_OAM_MA_TO_MEP,
	SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_INFO,
	/*SOC_DPP_WB_HASH_TBLS_OAM_BFD_MEP_TO_RMEP,*/
	SOC_DPP_WB_HASH_TBLS_OAM_BFD_RMEP_INFO,
    SOC_DPP_WB_HASH_TBLS_OAM_MEP_TO_AIS_ID,
    SOC_DPP_WB_HASH_TBLS_NUM

} SOC_DPP_WB_HASH_TBLS;


extern int current_hash_tbl_idx;
extern int current_saved_idx;
extern int nof_active_htb;

int
dpp_hash_tbl_wb_init(int unit, int hash_tbl_idx, int wb_var_idx, shr_htb_hash_table_t ht, int key_size, int max_nof_values, int max_data_size, dpp_hash_tbl_save_fn save_fn, dpp_hash_tbl_load_fn load_fn);

int
dpp_hash_tbl_wb_destroy(int unit,int hash_tbl_idx);

int
dpp_fill_wb_arrays_from_hash_tbl(int unit, int hash_tbl_idx);

int
dpp_extract_hash_tbl_from_wb_arrays(int unit, int hash_tbl_idx);

#endif /*BCM_WARM_BOOT_SUPPORT*/

#endif /*_SOC_WB_DB_HASH_TBL_H_*/

