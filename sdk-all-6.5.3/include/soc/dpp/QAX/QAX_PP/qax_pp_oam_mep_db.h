/*
 * $Id: $
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
 * File: qax_pp_oam_mep_db.h
 */

#ifndef __QAX_PP_OAM_MEP_DB_INCLUDED__
#define __QAX_PP_OAM_MEP_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, blk, index, data) \
            soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                                blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)
#define SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_DM_STAT_ONE_WAYm(unit, blk, index, data) \
            soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                               blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)
#define SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, blk, index, data) \
            soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                                blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)
#define SOC_QAX_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, blk, index, data) \
            soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index),\
                               blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data)

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

/*
 * Find where to add or delete entries to/from the MEP-DB for loss/delay
 * add/update/delete.
 * The pointers in lm_dm_info should already be set to valid populated
 * entry buffers.
 * The action_type must be initialized to one of:
 *   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_ADD_UPDATE
 *   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE
 *
 * Returns the scanning result (lm/dm/lm_stat/last entries) and what
 * entries need allocation or freeing.
 */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/*
 * Add/Remove/Update LM/DM entries in the MEP-DB.
 * This assumes that soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find was
 * already called and that the required entries were allocated.
 */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_set(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/*
 * Remove LM/DM entries in the MEP-DB. This assumes that
 * soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find was already called.
 */
soc_error_t soc_qax_pp_oam_oamp_lm_dm_delete(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Find where to add entries to the MEP-DB (May return REMOVE action in case of update) */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Find what to remove */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Scan the MEP-DB starting with the supplied MEP and find
   what has already been added to it and where */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_mep_scan(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Add allocation request to expand/create the LM/DM entry chain */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Add entry to the delete and deallocate request list of lm_dm_info */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info,
   uint32 entry
   );

/* Set the pointer to the LM/DM chain in the MEP entry */
soc_error_t
soc_qax_pp_oam_mep_db_lm_dm_ptr_set(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info,
   uint32 first_entry
   );

/* Clear the 'LAST' bit from the former last entry after adding a new
   entry to the chain */
soc_error_t
soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info, /* To resolve the entry type */
   uint32 entry,
   uint8 val
   );

/*
 * soc_qax_pp_oam_oamp_lm_dm_search -
 * Search LM and DM on the soc layer.
 *
 * INPUT:
 *   int  unit -
 *   uint32 endpoint_id - index of the endpoint in the OAMP
 *                        MEP DB (CCM entry) SOC_SAND_OUT
 *   uint32 found_bitmap - For every mep type, SOC_PPC_OAM_MEP_TYPE_XXX,
 *                          (found_bitmap &  SOC_PPC_OAM_MEP_TYPE_XXX)
 *                          iff such an entry is associated with the given mep.
 */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_search(
   int unit,
   uint32 endpoint_id,
   uint32 *found_bmp
   );


/*
 * soc_qax_pp_oam_oamp_lm_dm_shared_scan -
 * Prepares the data required for handeling the QAX style MEP-DB for
 * auxiliary functions.
 */
soc_error_t
soc_qax_pp_oam_oamp_lm_dm_shared_scan(
   int unit,
   int endpoint_id,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );

/* Get delay measurement data */
soc_error_t soc_qax_pp_oam_oamp_dm_get(
   int unit,
   SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
   uint8 *is_1DM
   );

/* Get loss measurement data. */
soc_error_t soc_qax_pp_oam_oamp_lm_get(
    int unit,
    SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info
    );


/* } */
#endif /* __QAX_PP_OAM_MEP_DB_INCLUDED__ */
