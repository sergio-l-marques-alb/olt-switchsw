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
 * File: qax_pp_oam_mep_db.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/mbcm_pp.h>


#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam_mep_db.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/dpp_wb_engine.h>


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

/************************************
 * Static declerations
 ************************************/
/* { */

/* } */
/************************************/

/* Find where to add entries to the MEP-DB (May return REMOVE action in case of update) */
soc_error_t
  soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(int unit,
     ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{

    soc_error_t res;

    SOCDNX_INIT_FUNC_DEFS;

    if (lm_dm_info->lm_dm_entry->is_update) { /* UPDATE */

        /* Check if new entry is needed */
        if (lm_dm_info->lm_dm_entry->entry_type == SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT) {
            if (lm_dm_info->lm_stat_entry == 0) {
                /* Update LM -> LM+STAT */
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_STAT;
                res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(unit, lm_dm_info);
                SOCDNX_IF_ERR_EXIT(res);
            }
            /* else - Nothing to do   */
        }
        else {
            /* update LM+STAT -> LM */
            if (lm_dm_info->lm_stat_entry > 0) {
                lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT;
            }
            /* else - Nothing to do   */
        }

    } /* UPDATE end */

    else { /* ADD */

        int entries_to_allocate = 1;
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM;

        switch (lm_dm_info->lm_dm_entry->entry_type) {

        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT:
            /* Add LM+STAT */
            lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_WITH_STAT;
            entries_to_allocate = 2;
            /* No break on purpose */
        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM:
            /* ADD LM or LM+STAT */
            if (lm_dm_info->lm_entry) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                     (_BSL_SOCDNX_MSG("Loss measurement was already added to MEP")));
            }
            break;

        case SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM:
            /* ADD DM */
            if (lm_dm_info->dm_entry) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                                     (_BSL_SOCDNX_MSG("Delay measurement was already added to MEP")));
            }
            lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_2WAY;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Undefined action.")));

        }

        /* Add the decided nof. entries to the allocation list. */
        for (; entries_to_allocate > 0; --entries_to_allocate) {
            res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(unit, lm_dm_info);
            SOCDNX_IF_ERR_EXIT(res);
        }
    } /* ADD end */

exit:
    SOCDNX_FUNC_RETURN;
}

/* Find what to remove */
soc_error_t
  soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(int unit,
     ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{

    SOCDNX_INIT_FUNC_DEFS;

    switch (lm_dm_info->action_type) {


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_LM:
        /* Remove loss measurement */
        if (lm_dm_info->lm_entry == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND,
                                 (_BSL_SOCDNX_MSG("Loss entry not found.")));
        }
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM;
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_DM:
        /* Remove delay measurement */
        if (lm_dm_info->dm_entry == 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND,
                                 (_BSL_SOCDNX_MSG("Delay entry not found.")));
        }
        lm_dm_info->action = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Undefined action.")));

    }
exit:
    SOCDNX_FUNC_RETURN;
}

/* Scan the MEP-DB starting with the supplied MEP and find
   what has already been added to it and where */
soc_error_t
  soc_qax_pp_oam_oamp_lm_dm_mep_scan(int unit, ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info) {

    soc_error_t res = SOC_E_NONE;
    uint32 entry;
    uint8 last_bit = 0;
    int chain_count;
    SOC_PPC_OAM_MEP_TYPE entry_type;
    soc_reg_above_64_val_t reg_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    /* Is there even an entry chain? */
    if (lm_dm_info->mep_entry.lm_dm_ptr) {
        /* Scan it */
        entry = lm_dm_info->mep_entry.lm_dm_ptr;
        for (chain_count = 0;
              (chain_count < ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_SHARED_MAX_CHAIN_LEN(unit)) &&
              (last_bit == 0) &&
              (entry < ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit));
              ++chain_count) {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64);
            res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_above_64);
            SOCDNX_IF_ERR_EXIT(res);
            entry_type = soc_OAMP_MEP_DBm_field32_get(unit, reg_above_64, MEP_TYPEf);
            switch (entry_type) {
            case SOC_PPC_OAM_MEP_TYPE_DM:
                lm_dm_info->dm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY:
                lm_dm_info->dm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_LM:
                lm_dm_info->lm_entry = entry;
                last_bit = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            case SOC_PPC_OAM_MEP_TYPE_LM_STAT:
                lm_dm_info->lm_stat_entry = entry;
                last_bit = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_above_64, LAST_ENTRYf);
                break;
            default:
                /* Should never get here */
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                     (_BSL_SOCDNX_MSG("LM/DM chain ended abraptly without LAST_ENTRY bit set.")));
            }
            lm_dm_info->last_entry = entry;
            entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, entry);
        }
    }
    else {
        /* No chain, make sure last_entry shows that */
        lm_dm_info->last_entry = 0;
    }

    lm_dm_info->mep_scanned = TRUE;

exit:
    SOCDNX_FUNC_RETURN;
}

/* Find where to add or delete entries to/from the MEP-DB
   for loss/delay add/update/delete */
soc_error_t
  soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find(int unit,
           ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{

    SOCDNX_INIT_FUNC_DEFS;

    /* What do this MEP already has? */
    SOCDNX_IF_ERR_EXIT(
       soc_qax_pp_oam_oamp_lm_dm_mep_scan(unit, lm_dm_info));

    if (lm_dm_info->action_type == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_ADD_UPDATE) {
        SOCDNX_IF_ERR_EXIT(
           soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__add_update(unit,lm_dm_info));
    }
    else {
        SOCDNX_IF_ERR_EXIT(
           soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find__remove(unit,lm_dm_info));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/* Add allocation request to expand/create the LM/DM entry chain */
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_alloc_list_tail_add(int unit,
                      ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{
    uint32 entry;

    SOCDNX_INIT_FUNC_DEFS;

    if (lm_dm_info->num_entries_to_add) {
        /* This is not the 1st time this function is called
           for this action... */
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit,
                    lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add-1]);
    }
    else if (lm_dm_info->last_entry == 0) {
        /* No LM/DM entries yet.
         * Pointer to where to allocate the first is supplied
         */
        entry = lm_dm_info->lm_dm_entry->lm_dm_id;
    }
    else {
        /* Request to allocate the next entry in the chain if possible. */
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, lm_dm_info->last_entry);
    }

    if (entry > ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("No room to add another LM/DM statistics entry.")));
    }
    lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add++] = entry;

exit:
    SOCDNX_FUNC_RETURN;
}

/* Add entry to the delete and deallocate request list of lm_dm_info */
soc_error_t soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(int unit,
                      ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info, uint32 entry)
{
    SOCDNX_INIT_FUNC_DEFS;

    lm_dm_info->entries_to_remove[lm_dm_info->num_entries_to_remove++] = entry;

    SOC_EXIT;

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Add/Remove/Update LM/DM entries in the MEP-DB.
 * This assumes that soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find was
 * already called and that the required entries were allocated.
 */
soc_error_t soc_qax_pp_oam_oamp_lm_dm_set(int unit,
                      ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{
    soc_error_t res;
    soc_reg_above_64_val_t reg_data, min_delay_field;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    switch (lm_dm_info->action) {

    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NONE:
        break;  /* Nothing to do */


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_1WAY:
        /* Just set the neccessary entry with preliminary values and LAST bit
           Clearing the last bit from the previous last-entry is done later */

        
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("QAX 1-DM Not implemented yet")));
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_2WAY:
        /* Just set the neccessary entry with preliminary values and LAST bit
           Clearing the last bit from the previous last-entry is done later */
        soc_OAMP_MEP_DB_DM_STAT_ONE_WAYm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_DM);
        SOC_REG_ABOVE_64_CLEAR(min_delay_field);
        SHR_BITSET_RANGE(min_delay_field, 0, soc_mem_field_length(unit, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MIN_DELAYf));
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit, reg_data, MIN_DELAYf, min_delay_field);
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_set(unit, reg_data, LAST_ENTRYf, 1);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit, MEM_BLOCK_ALL,
                                                                         lm_dm_info->entries_to_add[0], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM:
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_WITH_STAT:
        /* First, add an LM entry. If not adding stat, set LAST bit. */
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_LM);
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, LAST_ENTRYf,
                                             (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM));
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_DBm(unit, MEM_BLOCK_ALL,
                                                             lm_dm_info->entries_to_add[0], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM) {
            /* Not adding stat, break the switch statment. */
            break;
        }
        /* else - No break on purpose - add LM-Stat   */
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_STAT:
        /* Just set the neccessary entry with preliminary values and LAST bit
           Clearing the last bit from the previous last-entry is done later */
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, MEP_TYPEf, SOC_PPC_OAM_MEP_TYPE_LM_STAT);
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, LAST_ENTRYf,1);
        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, MEM_BLOCK_ALL,
                        lm_dm_info->entries_to_add[lm_dm_info->num_entries_to_add-1], reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT:
        /* Delete already has a handling logic for that. */
        res = soc_qax_pp_oam_oamp_lm_dm_delete(unit, lm_dm_info);
        SOCDNX_IF_ERR_EXIT(res);
        break;


    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Unsupported MEP-DB add/update action.")));
    }

    if (lm_dm_info->action != ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NONE) {
        /* If this is the 1st LM/DM entry added, set the pointer from the MEP */
        if (lm_dm_info->mep_entry.lm_dm_ptr == 0) {
            res = soc_qax_pp_oam_mep_db_lm_dm_ptr_set(unit, lm_dm_info,
                                                      lm_dm_info->entries_to_add[0]);
            SOCDNX_IF_ERR_EXIT(res);

        }
        /* If this is not the first, and a new entry was added, clear the LAST
           bit in the previous chain-tail */
        else if (lm_dm_info->action != ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT) {
            res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, lm_dm_info->last_entry, 0);
            SOCDNX_IF_ERR_EXIT(res);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Remove LM/DM entries in the MEP-DB. This assumes that
 * soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find was already called.
 */
soc_error_t soc_qax_pp_oam_oamp_lm_dm_delete(int unit,
                       ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{

    soc_error_t res;
    uint32 entry;
    uint32 new_last_entry;
    int entries_to_remove = 1;

    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    switch (lm_dm_info->action) {


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM:
        /* How many entries should be freed */
        entries_to_remove += (lm_dm_info->lm_stat_entry > 0); /* LM_STAT exist => remove another.*/

        /* Only a DM entry may be left (LM is removed with LM_STAT if exists) */
        if (lm_dm_info->dm_entry) {
            if (lm_dm_info->dm_entry != lm_dm_info->last_entry) {
                /* Set the LAST bit on the DM entry */
                res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, lm_dm_info->dm_entry, 1);
                SOCDNX_IF_ERR_EXIT(res);
            }
            if (lm_dm_info->dm_entry != lm_dm_info->mep_entry.lm_dm_ptr) {
                /* Move the DM entry to the chain head */
                res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                                                              lm_dm_info->dm_entry, reg_data);
                SOCDNX_IF_ERR_EXIT(res);
                res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY,
                                                               lm_dm_info->mep_entry.lm_dm_ptr, reg_data);
                SOCDNX_IF_ERR_EXIT(res);
            }
        }
        else {
            /* No DM also. Nip the chain */
            res = soc_qax_pp_oam_mep_db_lm_dm_ptr_set(unit, lm_dm_info, 0);
            SOCDNX_IF_ERR_EXIT(res);
        }
        break;


    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM:
    case ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT:
        entry = (lm_dm_info->action == ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM) ?
            lm_dm_info->dm_entry :
            lm_dm_info->lm_stat_entry;
        /*
         * Suppose we want to remove a DM entry, there are 3 options:
         * 1. MEP -> DM (LAST)
         *          ==> only nip the chain (set MEP.ptr to 0)
         * 2. MEP -> STUFF , ... , DM (LAST)
         *          ==>  only turn the LAST bit on the new last entry
         * 3. MEP -> STUFF , ... , DM , More STUFF , ... , more STUFF (LAST)
         *          ==> copy everything after the DM backwards
         * For LM_STAT, the same logic holds.
         */
        if (entry == lm_dm_info->last_entry) { /* options (1) or (2) */
            if (lm_dm_info->mep_entry.lm_dm_ptr == entry) { /* option (1) */
                /* Nip the chain */
                res = soc_qax_pp_oam_mep_db_lm_dm_ptr_set(unit, lm_dm_info, 0);
                SOCDNX_IF_ERR_EXIT(res);
            }
            else { /* option (2) */
                /* Set the LAST bit on the new last entry */
                new_last_entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_PREV(unit, entry);
                res = soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(unit, lm_dm_info, new_last_entry, 1);
                SOCDNX_IF_ERR_EXIT(res);
            }
        }
        else { /* option (3) */
            /* Copy everything after entry backwards */
            while (entry < lm_dm_info->last_entry) {
                uint32 next_entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(unit, entry);
                SOC_REG_ABOVE_64_CLEAR(reg_data);
                res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, next_entry ,reg_data);
                SOCDNX_IF_ERR_EXIT(res);
                res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry ,reg_data);
                SOCDNX_IF_ERR_EXIT(res);
                entry = next_entry;
            }
        }
        break;


    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Unsupported MEP-DB remove action.")));
    }

    /* Add the freed entries to the deallocation list */
    res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(unit, lm_dm_info, lm_dm_info->last_entry);
    SOCDNX_IF_ERR_EXIT(res);
    entry = lm_dm_info->last_entry;
    while (--entries_to_remove) {
        entry = ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_PREV(unit, entry);
        res = soc_qax_pp_oam_oamp_lm_dm_shared_entry_remove_list_add(unit, lm_dm_info, entry);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* Set the pointer to the new LM/DM chain in the MEP entry */
soc_error_t
soc_qax_pp_oam_mep_db_lm_dm_ptr_set(int unit,
                                    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info,
                                    uint32 first_entry)
{

    soc_error_t res;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info->endpoint_id, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_OAMP_MEP_DBm_field32_set(unit, reg_data, FLEX_LM_DM_PTRf, first_entry);
    res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info->endpoint_id, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* Clear the 'LAST' bit from the former last entry after adding a new
   entry to the chain */
soc_error_t
soc_qax_pp_oam_mep_db_lm_dm_last_bit_write(int unit,
                                           ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info, /* To resolve the entry type */
                                           uint32 entry, uint8 val)
{

    soc_error_t res;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
    if (lm_dm_info->lm_entry == entry) {
        soc_OAMP_MEP_DB_LM_DBm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    }
    else if (lm_dm_info->lm_stat_entry == entry) {
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    }
    else if (lm_dm_info->dm_entry == entry) {
        soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field32_set(unit, reg_data, LAST_ENTRYf, val);
    }
    else {
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Something went wrong.")));

    }
    res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, entry, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

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
soc_qax_pp_oam_oamp_lm_dm_search(int unit, uint32 endpoint_id, uint32 *found_bmp)
{

    uint32 found_bitmap_lcl[1];

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* Preperations */
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             endpoint_id,
                                                             &lm_dm_info));


    /*
     * For every entry found, set the required bit.
     */
    *found_bitmap_lcl = 0;
    if (lm_dm_info.lm_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_LM);
    }
    if (lm_dm_info.lm_stat_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_LM_STAT);
    }
    if (lm_dm_info.dm_entry > 0) {
        SHR_BITSET(found_bitmap_lcl, SOC_PPC_OAM_MEP_TYPE_DM);
    }

    *found_bmp = *found_bitmap_lcl;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_pp_oam_oamp_lm_dm_shared_scan(int unit, int endpoint_id,
                                      ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info)
{
    soc_error_t res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    lm_dm_info->endpoint_id = endpoint_id;

    /* Get the MEP entry */
    res = arad_pp_oam_oamp_mep_db_entry_get_unsafe(unit, endpoint_id, &lm_dm_info->mep_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    /* Scan for LM/DM chain */
    res = soc_qax_pp_oam_oamp_lm_dm_mep_scan(unit, lm_dm_info);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


/* Read DM data and clears it from the reg_data buffer ('clear on
 * read'). The reg_data should be written back after this function
 */
STATIC void
_soc_qax_pp_oam_dm_two_way_entry_read(int unit, SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
        soc_reg_above_64_val_t reg_data)
{

    soc_reg_above_64_val_t reg_field;

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit,reg_data,LAST_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->last_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit,reg_data,MAX_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->max_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_get(unit,reg_data,MIN_DELAYf,reg_field);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_sub_seconds),0,reg_field,0,30);
    SHR_BITCOPY_RANGE( &(dm_info->min_delay_second),0,reg_field,30,12);

    SOC_REG_ABOVE_64_CLEAR(reg_field);
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit,reg_data, MAX_DELAYf, reg_field );
    SHR_BITSET_RANGE( reg_field, 0, soc_mem_field_length(unit, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MIN_DELAYf) );
    soc_OAMP_MEP_DB_DM_STAT_TWO_WAYm_field_set(unit,reg_data, MIN_DELAYf, reg_field );

}

/*
 * Get delay measurement data.
 * If the Delay entry is first in the chain, the dm_info->entry_id will
 * be overwritten with the entry to allow the API to return delay_id to
 * the user.
 */
soc_error_t
soc_qax_pp_oam_oamp_dm_get(int unit, SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info, uint8 *is_1DM)
{
    soc_error_t res;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = {0};

    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    /* Preperations */
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             dm_info->entry_id,
                                                             &lm_dm_info));

    /* Some validation */
    if (lm_dm_info.dm_entry == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("No DM entry for endpoint %d "),dm_info->entry_id));
    }

    /* Read the found entry */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.dm_entry, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    switch (soc_OAMP_MEP_DBm_field32_get(unit, reg_data, MEP_TYPEf)) {
    case SOC_PPC_OAM_MEP_TYPE_DM:
        *is_1DM = FALSE;
        _soc_qax_pp_oam_dm_two_way_entry_read(unit, dm_info, reg_data);
        res = SOC_QAX_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_DM_STAT_TWO_WAYm(unit,MEM_BLOCK_ALL, lm_dm_info.dm_entry , reg_data);
        SOCDNX_IF_ERR_EXIT(res);
        break;
    case SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY:
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("QAX 1-DM Not implemented yet")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                             (_BSL_SOCDNX_MSG("Something went wrong")));
    }

    /* Check if the entry needs to be returned */
    if (lm_dm_info.mep_entry.lm_dm_ptr == lm_dm_info.dm_entry) {
        /* DM entry is the 1st in the chain. Return the DM-entry as entry_id */
        dm_info->entry_id = lm_dm_info.dm_entry;
    }
    else {
        /* DM entry is later in the chain. Return entry_id=0 */
        dm_info->entry_id = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;

}

/* Read LM data */
STATIC void
_soc_qax_pp_oam_lm_entry_read(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
        soc_reg_above_64_val_t reg_data)
{
    lm_info->my_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_TXf);
    lm_info->my_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,MY_RXf);
    lm_info->peer_tx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_TXf);
    lm_info->peer_rx = soc_OAMP_MEP_DB_LM_DBm_field32_get(unit, reg_data,PEER_RXf);
}

/* Read LM-STAT data and clears it from the reg_data buffer ('clear on
 * read'). The reg_data should be written back after this function
 */
STATIC void
_soc_qax_pp_oam_lm_stat_entry_read(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info,
        soc_reg_above_64_val_t reg_data)
{
        lm_info->last_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_NEARf);
        lm_info->last_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,LAST_LM_FARf);
        lm_info->acc_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_NEARf);
        lm_info->acc_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,ACC_LM_FARf);
        lm_info->max_lm_near = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_NEARf);
        lm_info->max_lm_far = soc_OAMP_MEP_DB_LM_STATm_field32_get(unit, reg_data,MAX_LM_FARf);

        /* acc/max fields are to be reset when read */
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_NEARf , 0);
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, MAX_LM_FARf , 0);
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_NEARf , 0);
        soc_OAMP_MEP_DB_LM_STATm_field32_set(unit,reg_data, ACC_LM_FARf , 0);
}

/*
 * Get loss measurement data.
 * If the Loss entry is first in the chain, the lm_info->entry_id will
 * be overwritten with the entry to allow the API to return loss_id to
 * the user.
 */
soc_error_t
soc_qax_pp_oam_oamp_lm_get(int unit, SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info)
{
    soc_error_t res;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO lm_dm_info = {0};

    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    /* Preperations */
    SOCDNX_IF_ERR_EXIT(soc_qax_pp_oam_oamp_lm_dm_shared_scan(unit,
                                                             lm_info->entry_id,
                                                             &lm_dm_info));

    /* Some validation */
    if (lm_dm_info.lm_entry == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                             (_BSL_SOCDNX_MSG("No LM entry for endpoint %d "),lm_info->entry_id));
    }

    /* Read the found entry */
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.lm_entry, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
    _soc_qax_pp_oam_lm_entry_read(unit, lm_info, reg_data);

    /*Now see if extended statistics are available.*/
    lm_info->is_extended = (lm_dm_info.lm_stat_entry > 0);
    if (lm_info->is_extended) {
        /* Read the LM-STAT entry */
        SOC_REG_ABOVE_64_CLEAR(reg_data);
        res = _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.lm_stat_entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

        _soc_qax_pp_oam_lm_stat_entry_read(unit, lm_info, reg_data);

        res = _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, MEM_BLOCK_ANY, lm_dm_info.lm_stat_entry, reg_data);
        SOCDNX_IF_ERR_EXIT(res);

    }

    /* Check if the entry needs to be returned */
    if (lm_dm_info.mep_entry.lm_dm_ptr == lm_dm_info.lm_entry) {
        /* LM is the 1st in the chain. Return the LM-entry as entry_id */
        lm_info->entry_id = lm_dm_info.lm_entry;
    }
    else {
        /* LM entry is later in the chain. Return entry_id=0 */
        lm_info->entry_id = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;

}

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

