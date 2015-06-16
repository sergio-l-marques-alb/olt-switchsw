#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_fp_fem.c,v 1.66 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * $
*/
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP 
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/swstate/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_FP_FEM_DB_ID_NDX_MAX                             (ARAD_PP_FP_NOF_DBS-1)
#define ARAD_PP_FP_FEM_ACTION_TYPE_MAX                           (ARAD_PP_NOF_FP_ACTION_TYPES - 1)
#define ARAD_PP_FP_FEM_DB_STRENGTH_MAX                           (ARAD_PP_FP_NOF_DBS-1)
#define ARAD_PP_FP_FEM_DB_ID_MAX                                 (ARAD_PP_FP_FEM_DB_ID_NDX_MAX)
#define ARAD_PP_FP_FEM_ENTRY_STRENGTH_MAX                        (SOC_SAND_U16_MAX)

#define ARAD_PP_FP_FEM_SAND_U64_NOF_BITS                         (64)
#define ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS                       (4)

#define ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY                        (5)
#define ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE                (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width))

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

typedef struct
{
  uint32 indx;
  uint32 size;
} action_inf;


typedef enum
{
  /*
   *  Mapping from Actions to buffer.
   */
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER = 0,
  /*
    *  Mapping from buffer to Actions.
   */
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION = 1,
  /*
   *  Number of types in ARAD_PP_FP_ACTION_BUFFER_DIRECTION
   */
  ARAD_PP_NOF_FP_ACTION_BUFFER_DIRECTIONS = 2
}ARAD_PP_FP_ACTION_BUFFER_DIRECTION;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_TAG_ACTION_TYPE_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_DUPLICATE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURATION_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REORGANIZE),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'db_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR",
    "The parameter 'db_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'entry_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};




/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


void arad_pp_fp_action_sort(
    action_inf action_array[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    int nof_actions
   )
{
    int i, j, k;
    action_inf action;
    for (i = 0; i < nof_actions; ++i)
    {
        /* ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY needs to be first*/
        if (action_array[i].indx == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY)
        {
            action = action_array[i];
            for (k = i; k > 0; k--) {
                action_array[k]=action_array[k-1];
            }
            action_array[0] = action;
            continue;
        }
        
        for (j = i + 1; j < nof_actions; ++j) 
        {
            if (action_array[i].size < action_array[j].size)
            {
                action =  action_array[i];
                action_array[i] = action_array[j];
                action_array[j] = action;
            }
        }
    }
}


/*
 * allocate FES in program for given action 
 * move exist FES if needed (to preserve order)
 * write to hardware if required (according to flags)
 * update FES-info. (in order to affect next action)
 */
uint32
  arad_pp_fp_action_alloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_type, /* action*/
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  uint32                       my_priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_IN  uint32                       action_lsb,/* lsb of current action */
    SOC_SAND_IN  uint32                       action_len,/* length of current action */
    SOC_SAND_INOUT ARAD_PMF_FES               fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                        *found
  )
{
    uint32
        fem_id_ndx,
        fes_tm_lsb,
        new_fes_id = 0;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    ARAD_FP_DATABASE_STAGE
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 
    ARAD_PP_FP_FEM_ENTRY
        fem_entry2,
      fem_entry;
    ARAD_PP_FP_FEM_CYCLE
      fem_cycle;
    uint8
        is_fem_blocking,
      is_fes_free;
    SOC_SAND_SUCCESS_FAILURE
        success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    *found = 0;

    /* 
     * Use a standard way to insert FES / FEM 
     * See in the function for documentation 
     */
    ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
    ARAD_PP_FP_FEM_CYCLE_clear(&fem_cycle);
    fem_entry.is_for_entry = FALSE;
    fem_entry.db_id = db_id;
    fem_entry.db_strength = my_priority;
    fem_entry.entry_id = prog_id; /* Use entry-id for PMF-Program ID */
    fem_entry.action_type[0] = action_type;

    /* Check there is a place in some FES */
    is_fes_free = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FESS) && (!is_fes_free); ++fem_id_ndx)
    {
        if (!fes_info[fem_id_ndx].is_used) {
            new_fes_id = fem_id_ndx;
            is_fes_free = TRUE;
        }
    }

    success = SOC_SAND_SUCCESS;
    if (is_fes_free == FALSE)
    {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM not found for db_id:%d\n\r"), db_id));
      success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      *found = 0;
      ARAD_DO_NOTHING_AND_EXIT;
    }

    /* in TM mode, any free FEM is valid: the FP action will appear only once, no resolution */
    if  ( (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) == 0 ) && (flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) ) {
        success = SOC_SAND_SUCCESS;
        goto exit_tm;
    }

    /*
     * Set it
     */
    res = arad_pp_fp_fem_insert_unsafe(
            unit,
            &fem_entry,
            &fem_cycle,
            flags,
            NULL,
            fes_info,
            &new_fes_id,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* 
     * In case of failure: 
     * - see if the FEM 1st group blocks (presence of the same action type) 
     * - otherwise, consider the 2 FES groups as one group and try again 
     */
    if (success != SOC_SAND_SUCCESS) {
        /* Find if the 1st FEM group blocks */
        is_fem_blocking = FALSE;
        for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP) && (!is_fem_blocking); ++fem_id_ndx)
        {
              ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry2);
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                      unit,
                      stage,
                      fem_id_ndx,
                      &fem_entry2
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
              if ((fem_entry2.is_for_entry == TRUE) && (fem_entry2.action_type[0] == action_type)) {
                  is_fem_blocking = TRUE;
              }
         }

        /* Re-arrange the 32 FESes */
        if (!is_fem_blocking) {
            res = arad_pp_fp_fem_insert_unsafe(
                    unit,
                    &fem_entry,
                    &fem_cycle,
                    (flags | ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP),
                    NULL,
                    fes_info,
                    &new_fes_id,
                    &success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit_tm:
    *found = (success == SOC_SAND_SUCCESS);


    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   allocated FES-id:%d, \n\r"), new_fes_id));

    /* found and this only for check, then calculate action size */
    if(*found && (flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FES action:%s, length: %d, lsb: %d\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));
    }
    /* write new FES to allocated place */
    else if(*found  && !(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
        /* new_fes_id */ 

        ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
        ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
        
        /* Configure the FES differently in TM mode */
        if ((flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) && (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) ==0 ) ) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = 0 /* always key-A */;
            /* Get the closest 16b multiply */
            fes_tm_lsb = (ARAD_PP_FP_KEY_LENGTH_TM_IN_BITS - action_lsb /* offset-lsb */ - 1);
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(fes_tm_lsb); 
            fes_input_info.src_arad.lookup_cycle_id = 1; /* Always first cycle */

            my_fes_info.shift = fes_tm_lsb - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = TRUE;
        }
        else if (flags & ARAD_PP_FP_FEM_ALLOC_FES_FROM_KEY) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[0];
            /* Get the closest 16b multiply */
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(constraint->tcam_res_id[1]); 
            fes_input_info.src_arad.lookup_cycle_id = 1; /* Always first cycle */

            my_fes_info.shift = constraint->tcam_res_id[1] - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = (flags & ARAD_PP_FP_FEM_ALLOC_FES_KEY_IS_CONDITIONAL_VALID)? FALSE: TRUE;
        }
        else {
            fes_input_info.src_arad.is_key_src = FALSE;
            
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID(action_lsb)];
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(action_lsb); 
            fes_input_info.src_arad.lookup_cycle_id = constraint->cycle;

            my_fes_info.shift = ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT(action_lsb);
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = FALSE;
        }

        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   commit changes to HW: "
                              "FES action :%s, length: %d, "
                              "next action-lsb:%d  \n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));

        res = arad_pmf_db_fem_input_set_unsafe(
                unit,
                prog_id,
                TRUE, /* is FES*/
                new_fes_id,
                &fes_input_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        res = arad_pmf_db_fes_set_unsafe(
                unit,
                prog_id,
                new_fes_id,
                &my_fes_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        fes_sw_db_info.action_type = action_type;
        fes_sw_db_info.db_id = db_id;
        fes_sw_db_info.entry_id = entry_id;
        fes_sw_db_info.is_used = 1;

        /* 
         * Update if this FES can be moved to the other 
         * group - for the future FESs 
         */

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                    unit,
                    stage,
                    prog_id,
                    new_fes_id,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_fes()", 0, 0);
}


uint32
  arad_pp_fp_action_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
    uint32 entry_id = -1;

    return arad_pp_fp_action_alloc_in_prog_with_entry(
                unit,
                db_id,
                prog_id,
                entry_id,
                flags,
                action_types,
                priority,
                constraint,
                action_alloced);

}

uint32
  arad_pp_fp_action_alloc_in_prog_with_entry(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint8
      fes_alloced = 0;
  uint32
      action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      action_size,
      nof_actions,
      action_indx,
      fes_idx;
  ARAD_PMF_FES
      fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS];
  uint32
    res = SOC_SAND_OK;
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 
  SOC_SAND_SUCCESS_FAILURE  
      success;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                unit,
                stage,
                prog_id,
                fes_idx,
                &fes_info[fes_idx]
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /* 
   * Get the minimal size for the actions and their LSBs
   */
  res = arad_pp_fp_action_to_lsbs(
          unit,
          stage,
          action_types,
          action_lsbs,
          action_lengths,
          &action_size,
          &nof_actions,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* 
   * Get the action length and see if it is OK 
   */ 
  /* check doesn't exceed Tcam action size */
  if((action_size > constraint->action_size) || (success != SOC_SAND_SUCCESS)) {
    LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "Unit %d, Program Id %d, Failed to allocate action in program.\n\r"
                          "Action size exceed the maximal size in the constraint.\n\r"
                          "Action size %d, Maximal size in the constraint %d.\n\r"), unit, prog_id, action_size, constraint->action_size));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 80, exit); /* need more bits in TCAM action as expected */
  }
  
  /* get relevant programs for the DB */
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != ARAD_PP_ACTION_TYPE_INVALID; ++action_indx) {
      if (action_types[action_indx] == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) {
          /* no FES needed for this action type */
          continue;
      }

      res = arad_pp_fp_action_alloc_fes(
                unit,
                db_id,
                prog_id,
                entry_id,
                action_types[action_indx],
                flags,
                priority,
                constraint,
                action_lsbs[action_indx],
                action_lengths[action_indx], /* Action length */
                fes_info,
                &fes_alloced
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + action_indx, exit);

      if(!fes_alloced) {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "    "
                                "FES: fail to allocate for DB %d, program %d, action %s, flags %d, priority %d, constraint action size %d \n\r"),
                     db_id, prog_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_types[action_indx]), flags, priority, constraint->action_size));
          *action_alloced = 0; /* fail to allocate FES */
          goto exit;
      }
  }
  
  /* all allocation successed */
  *action_alloced = 1;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_alloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  uint32                       selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_INOUT  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs[1],
      prog_result;
  uint8
      action_alloced_in_prog = 0;
  uint32
    res = SOC_SAND_OK;
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  /* same management for new/old DBs */
  /* on all new programs allocate FES for each action */

  /* get first new program */
  cur_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  while(prog_result != 0) {
      constraint->cycle = selected_cycle[cur_prog];
      res = arad_pp_fp_action_alloc_in_prog(
                unit,
                db_id,
                cur_prog,
                flags,
                action_types,
                priority,
                constraint,
                &action_alloced_in_prog
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if(!action_alloced_in_prog) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    "
                              "FES: fail to allocate for DB %d, program %d, flags %d, priority %d, constraint action size %d \n\r"),
                   db_id, cur_prog, flags, priority, constraint->action_size));
          *action_alloced = 0;  /* fail to action allocate in this program */
          goto exit; /* done with FAIL*/
      }
      /* get next program */
      ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS-cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,TRUE,prog_result);
  }

  *action_alloced = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc()", 0, 0);
}


/*
 * de-allocate FES in program for given action 
 * This FES has to exist 
 */
uint32
  arad_pp_fp_action_dealloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_type /* action*/
  )
{
    uint32
        new_fes_id = 0;
    int32
        indx = 0;
    uint8
        found;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    ARAD_FP_DATABASE_STAGE
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "De-allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    found = 0;
    /* find the FES with this action */
    for(indx = 0; indx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++indx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                    unit,
                    stage,
                    prog_id,
                    indx,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

        if(fes_sw_db_info.is_used
           && (fes_sw_db_info.db_id == db_id)
           && (fes_sw_db_info.action_type == action_type)) {
            new_fes_id = indx;
            found = 1;
            break;
        }
    }


    if(found == 0) {/* not such a FES */
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "FES deallocation failed:%d  - no FES with this action %s for DB %d and PMF-Program %d \n\r"), 
                 found, SOC_PPC_FP_ACTION_TYPE_to_string(action_type), db_id, prog_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR, 7, exit); 
    }

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Commit changes to HW: "
                          "De-allocate FES %d action :%s, "), new_fes_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
    ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
    my_fes_info.action_type = SOC_PPC_FP_ACTION_TYPE_NOP;

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            prog_id,
            TRUE, /* is FES*/
            new_fes_id,
            &fes_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_pmf_db_fes_set_unsafe(
            unit,
            prog_id,
            new_fes_id,
            &my_fes_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Update SW DB for the other actions */
    fes_sw_db_info.is_used = 0;
    fes_sw_db_info.db_id = 0;
    fes_sw_db_info.action_type = ARAD_PP_ACTION_TYPE_INVALID;
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                unit,
                stage,
                prog_id,
                new_fes_id,
                &fes_sw_db_info
              );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);


    /* 
     * Re-order the FESes in this group to be at the beginning
     */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_fes()", 0, 0);
}




uint32
  arad_pp_fp_action_dealloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] /* actions*/
  )
{
  uint32
      action_indx;
  uint32
    res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get relevant programs for the DB */
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != ARAD_PP_ACTION_TYPE_INVALID; ++action_indx) {
      if (action_types[action_indx] == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) {
          /* no FES needed for this action type */
          continue;
      }

      /* Find the FES to deallocate */
      res = arad_pp_fp_action_dealloc_fes(
                unit,
                db_id,
                prog_id,
                action_types[action_indx]
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + action_indx, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_dealloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] /* actions*/
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
            db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  /* same management for new/old DBs */
  /* on all new programs allocate FES for each action */

  /* get first new program */
  for (cur_prog = 0; cur_prog < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); cur_prog++) {
      if ((1 << cur_prog) & exist_progs) {
          res = arad_pp_fp_action_dealloc_in_prog(
                    unit,
                    db_id,
                    cur_prog,
                    action_types
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc()", 0, 0);
}




/*
 * given DB id return list of action for this DB
 */
uint32
  arad_pp_fp_fem_db_actions_get(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   action_lsbs[2]
  )
{
   return 0;
   
}

/*
 * given list of action returns lsb of each action
 */
uint32
  arad_pp_fp_action_to_lsbs(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE    action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
      SOC_SAND_OUT  uint32                   action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT  uint32                   action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT  ARAD_TCAM_ACTION_SIZE  *action_size,
      SOC_SAND_OUT  uint32                   *nof_actions,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
    int32
        action_indx = 0;
    uint32
        action_lsb_ingress_tcam = 0,
      action_lsb_egress,
        nof_bits, /* actually maximum number of bits needed */
        action_len_complete,
      action_len,
        zone_length[2] = {0, 0}, /* Number of bits taken per first 40b zone */
      res = SOC_SAND_OK;
    uint8 
      action_change_key_loop;
    int algorithm_indx = 0;

    
    action_inf action_array[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    
    ARAD_PP_FP_ACTION_TYPE* non_const_action_types = (ARAD_PP_FP_ACTION_TYPE*) action_types;
   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*in case action allocation fails we will sort the actions acording to their sizes and then try to allocate again */
    
    for (algorithm_indx = 0; algorithm_indx < 2; algorithm_indx++) {

        /*if the first try to allocate actions fail sort the action*/
        if (algorithm_indx == 1) {
            arad_pp_fp_action_sort(action_array, *nof_actions);
            for (action_indx = 0;  action_indx < *nof_actions ; action_indx++)
                non_const_action_types[action_indx]= action_array[action_indx].indx;

            sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
            sal_memset(action_lengths, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));

            action_lsb_ingress_tcam = 0;
            zone_length[0] = 0;
            zone_length[1] = 1;

        }


        nof_bits = 0;
        *nof_actions = 0;
        *success = SOC_SAND_SUCCESS;

    
    action_lsbs[0] = 0;
    action_lengths[0] = 0;
    /* 
     * For cascaded lookups, the action must be at the LSB of the action table
     * Loop first on the cascaded action, after on the other actions.                                                                                                       {
     */
    for (action_change_key_loop = 0; action_change_key_loop <= 1; action_change_key_loop++) {
        if ((stage != ARAD_FP_DATABASE_STAGE_INGRESS_PMF) && (action_change_key_loop == 0)) {
            /* no cascaded databases at egress - single lookup stage */
            continue;
        }
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_types[action_indx] != ARAD_PP_ACTION_TYPE_INVALID); ++action_indx) {
            if ((action_change_key_loop == 0) && (action_types[action_indx] != ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }
            else if ((action_change_key_loop == 1) && (action_types[action_indx] == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }

                res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_types[action_indx],
                    stage,
                    &action_len,
                    &action_lsb_egress
                    );

                action_array[action_indx].size=action_len;
                action_array[action_indx].indx=action_types[action_indx];

                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                if (action_types[action_indx] == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) {
                    action_lsbs[action_indx] = 0;
                    action_lsb_ingress_tcam = 0;
                    action_len_complete = action_len; /* not +1 for valid bit since it does not go the FES */
                } else if (stage != ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
                    action_len_complete = action_len; /* not +1 for valid bit since the encoding is hard-coded */
                    action_lsbs[action_indx] = action_lsb_egress; /* HW lsb hard-coded at egress */
                } else {
                    /* Check if the action is in 31:0 or 39:8. Otherwise, try the second part */
                    if (((action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) || (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2))
                        && (action_len == 32)) {
                        /* Special case User-Header: consider it as 31b action length since 32b action length + 1b validity not supported in FES HW */
                        action_len = 31;
                    }
                    action_len_complete = action_len + 1; /* +1 for my valid */
                    action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(zone_length[0], action_len_complete);
                    if ((zone_length[0] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE /* Valid usage of macros */)
                        && (action_lsb_ingress_tcam + action_len_complete <= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE)) {
                        action_lsbs[action_indx] = action_lsb_ingress_tcam; /* my action lsb */
                    } else {
                        action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(zone_length[1], action_len_complete);
                        action_lsbs[action_indx] = ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE + action_lsb_ingress_tcam; /* my action lsb */
                    }
                }
                /* Update 1st zone size if needed */
                if (action_lsbs[action_indx] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                    zone_length[0] = action_len_complete + action_lsb_ingress_tcam;
                } else {
                    zone_length[1] = action_len_complete + action_lsb_ingress_tcam;
                }
                nof_bits = SOC_SAND_MAX(nof_bits, action_lsbs[action_indx] + action_len_complete);
                /* action length: cannot be extrapolated by the LSBs due to the bit 40 jump */
                action_lengths[action_indx] = action_len_complete;
                *nof_actions += 1;
            }
        }

        
        action_lsbs[action_indx] = nof_bits;

        if ((stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) || (stage == ARAD_FP_DATABASE_STAGE_EGRESS)) {
            if (nof_bits <= SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
                break; /* action allocation succeed */
            } else if (nof_bits <= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
                break; /* action allocation succeed */
            } else if (nof_bits <= 3 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS;
                break; /* action allocation succeed */ 
            } else if (nof_bits <= 4 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS;
                break; /* action allocation succeed */ 
            } else if (algorithm_indx==1){
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "    "
                                      "Error in action computation: "
                                      "For stage %s, total action size %d bits \n\r"),
                           SOC_PPC_FP_DATABASE_STAGE_to_string(stage), nof_bits));
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "\n\r")));

                *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                ARAD_DO_NOTHING_AND_EXIT;
            }
        } else {
            break;
        }

    }

     

    /* Fixed action size needed for Egress */
    if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
        *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_to_lsbs()", action_indx, 0);
}



/* 
 * Map between action values and buffer of the TCAM Action table
 */
uint32
  arad_pp_fp_action_value_buffer_mapping(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_BUFFER_DIRECTION direction,
      SOC_SAND_INOUT ARAD_PP_FP_ACTION_VAL   action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_INOUT uint32                  buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE]
  )
{
    uint32
        action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        nof_actions,
        action_val_indx,
        action_val_indx2,
        action_indx,
        is_valid_bit,
        len;
    uint8
        action_found;
    uint32
      res = SOC_SAND_OK;
    ARAD_PP_FP_ACTION_VAL   
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    ARAD_FP_DATABASE_STAGE
        stage; 
    ARAD_TCAM_ACTION_SIZE  
        action_size;
    ARAD_PP_FP_DATABASE_INFO               
        fp_db_info;
    SOC_SAND_SUCCESS_FAILURE  
        success;
    uint8 is_vt_match_ipv4 = FALSE;
    uint8 is_fp_db_extended_fwrd = FALSE;
    uint8 is_fp_flp_match = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* Get the Database info */
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

    if ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_STAGE_VT_EXTENDED)){
         is_vt_match_ipv4 = TRUE;
    }
    if ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL)){        
        if (fp_db_info.action_types[0]== SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) {/* FLP action */
            is_fp_flp_match = TRUE;
        } else {
			is_vt_match_ipv4 = TRUE;
		}
    }


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_DATABASES  && (fp_db_info.internal_table_id != ARAD_KBP_FRWRD_TBL_ID_IPV4_MC)){
         is_fp_db_extended_fwrd = TRUE;
    }
#endif 

    if ( is_vt_match_ipv4 ) /* In case of Flexible QinQ - there's only 1 action, which is the inlif value*/
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 16 ; /*Length of Inlif */
        action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
        nof_actions = 1 ; 
        fp_db_info.action_types[0] = SOC_PPC_FP_ACTION_TYPE_IN_LIF;
    }else if ( is_fp_flp_match ) /* In case of FLP - DBAL - we use only action */
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 19 ;         

        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        fp_db_info.action_types[1] = SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;

        nof_actions = 2 ; 

        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            buffer[0] = action_vals[0].val; 
            buffer[1] = action_vals[1].val;
            goto exit;
        }
    }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    else if (is_fp_db_extended_fwrd) {
        action_lsbs[0] = 64;
        action_lengths[0] = 24 ;
        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        action_lsbs[2] = 0;
        action_lengths[2] = 32 ;
        action_lsbs[3] = 88;
        action_lengths[3] = 32 ;
        nof_actions = 4;
    }
#endif 
    else
    {
      /* get from action vals lsbs */
        res = arad_pp_fp_action_to_lsbs(
                unit,
                stage,
                fp_db_info.action_types,
                action_lsbs,
                action_lengths,
                &action_size,
                &nof_actions,
                &success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        if (success != SOC_SAND_SUCCESS) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB-Id %d, Invalid action composition.\n\r"),
                     unit, db_id));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }

    /* Special case for FLP: all the actions must be encoded from bit 0, since they enter in different ELK DBs */
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && (!is_fp_db_extended_fwrd)) {
        sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
    }

    /* 
     * Copy the action types and values to a common local array
     */
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        ARAD_PP_FP_ACTION_VAL_clear(&action_vals_lcl[action_val_indx]);
        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            action_vals_lcl[action_val_indx].type = action_vals[action_val_indx].type;
            action_vals_lcl[action_val_indx].val = action_vals[action_val_indx].val;
        }
        else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
            /* Presence of a valid bit for each TCAM action*/
            is_valid_bit = ((stage == ARAD_FP_DATABASE_STAGE_EGRESS) 
                            || (stage == ARAD_FP_DATABASE_STAGE_INGRESS_FLP) 
                            || (is_vt_match_ipv4 )
                            || (fp_db_info.action_types[action_val_indx] == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY))? 0: 1;

            /* See if the action is set in the buffer - no verification at egress */
            if ((action_val_indx < nof_actions) && (SHR_BITGET(buffer,action_lsbs[action_val_indx]) || (!is_valid_bit))) {
                action_vals_lcl[action_val_indx].type = fp_db_info.action_types[action_val_indx];
                len = action_lengths[action_val_indx];
                SHR_BITCOPY_RANGE(&(action_vals_lcl[action_val_indx].val), 0, buffer, action_lsbs[action_val_indx]+is_valid_bit, len-is_valid_bit); /* skip valid bit at ingress */
            }
            else
            {
                action_vals_lcl[action_val_indx].type = ARAD_PP_ACTION_TYPE_INVALID;
            }
        }
    }

    /* map from action type to placement in DB actions */
    action_val_indx2 = 0; /* Encode the number of valid actions up to now */
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        if (action_vals_lcl[action_val_indx].type == ARAD_PP_ACTION_TYPE_INVALID) {
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                /* Stop the mapping if no more actions */
                break;
            }
            else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                /* This action is invalid*/
                action_vals[action_val_indx2].type = ARAD_PP_ACTION_TYPE_INVALID;
                action_vals[action_val_indx2].val = 0;
                continue;
            }
        }

        action_found = 0;
        for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && fp_db_info.action_types[action_indx] != ARAD_PP_ACTION_TYPE_INVALID; ++action_indx) {
            /* 
             * The list of actions between the buffer and the Database is identical: 
             * thus, we can go to the correct iindex directly 
             */
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                action_indx = action_val_indx;
            }

            if(action_vals_lcl[action_val_indx].type == fp_db_info.action_types[action_indx]) { /* if this is the action*/
                if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                    /* Presence of a valid bit for each TCAM action*/
                    is_valid_bit = ((stage != ARAD_FP_DATABASE_STAGE_INGRESS_PMF)
                                    || (is_vt_match_ipv4 )
                                    || (action_vals_lcl[action_val_indx].type == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY))? 0: 1;
                    len = action_lengths[action_indx];
                    if (is_valid_bit) {
                        SHR_BITSET(buffer, action_lsbs[action_indx]); /* set valid bit at LSB of the action (FES lsb) */
                    }
                    SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx]+is_valid_bit,&(action_vals_lcl[action_val_indx].val), 0, len-is_valid_bit); /* Copy action value */
                }
                else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                    action_vals[action_val_indx2].type = action_vals_lcl[action_val_indx].type;
                    action_vals[action_val_indx2].val = action_vals_lcl[action_val_indx].val;
                }
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "   "
                                      "Action Value to buffer: %d, "
                                      "Action type:%s, "
                                      "Action val:%d \n\r"), direction, 
                           ARAD_PP_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type), 
                           action_vals_lcl[action_val_indx].val));
                action_val_indx2 ++;
                action_found = 1;
                break;
            }
        }
        /* if action  Value is not exist in DB action Error */
        if(!action_found) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB id %d Action Type %s : action Value type does not exist in DB action.\n\r"),
                     unit, db_id, ARAD_PP_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type)));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_buffer_mapping()", db_id, 0);
}



/* 
 * Insert the Action values in the correct buffer for ex. for the TCAM Action table
 */
uint32
  arad_pp_fp_action_value_to_buffer(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE]
  )
{
    uint32
        fld_val,
        key_bitmap_constraint_cascaded,
        action_indx,
        action_size,
        action_lsb_egress,
      res = SOC_SAND_OK;
    ARAD_PP_FP_ACTION_VAL     
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    ARAD_FP_DATABASE_STAGE
        stage; 
    ARAD_PP_FP_DATABASE_INFO
      fp_database_info; 
    uint8
        egress_action_ndx,
        key_ndx,
        is_key_fixed;
    ARAD_PP_FP_ACTION_TYPE
        action_egress_disabled[] = {SOC_PPC_FP_ACTION_TYPE_ACE_POINTER, SOC_PPC_FP_ACTION_TYPE_EGR_OFP};
    uint8 is_vt_match_ipv4 = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Init all the buffer to 0: no action done */
    sal_memset(buffer,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE);
    sal_memcpy(action_vals_lcl, action_vals, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX * sizeof(ARAD_PP_FP_ACTION_VAL));

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* 
     * Special cases: 
     * - at egress, disable the destination by setting all ones 
     * - at ingress, build the correct format of the Cascaded action 
     */
    if (stage == ARAD_FP_DATABASE_STAGE_EGRESS) {
        for (egress_action_ndx = 0; egress_action_ndx < sizeof(action_egress_disabled) / sizeof(ARAD_PP_FP_ACTION_TYPE); egress_action_ndx++) {
            /* The buffer of the Destination and ACE-Pointer must be set to -1 to be disabled */
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_egress_disabled[egress_action_ndx],
                    ARAD_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            fld_val = (1 << action_size) - 1; /* Destination disabled */
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }

        if (SOC_IS_JERICHO(unit))
        {
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    SOC_PPC_FP_ACTION_TYPE_ACE_TYPE,
                    ARAD_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            fld_val = 0x1; /* ACE-TYPE value = stat only*/
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }
    }
    else if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
                 unit,
                 stage,
                 db_id,
                 &fp_database_info
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        if (fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_STAGE_VT_EXTENDED )
        {
             is_vt_match_ipv4 = TRUE;
        }
        if (!is_vt_match_ipv4) {
            for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (fp_database_info.action_types[action_indx] != ARAD_PP_ACTION_TYPE_INVALID); ++action_indx) {
                if (action_vals_lcl[action_indx].type == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) {
                    /* 
                     * Get the coupled Database ID Key-Id - uniform 
                     * for all the PMF-Programs 
                     */ 

                    res = arad_pp_fp_key_alloc_key_cascaded_key_get(
                            unit,
                            fp_database_info.cascaded_coupled_db_id,
                            &is_key_fixed,
                            &key_ndx,
                            &key_bitmap_constraint_cascaded
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
                    if (!is_key_fixed) {
                      LOG_ERROR(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   Error in entry add: "
                                            "For database %d, stage %s, the database is indicated to be cascaded with DB-Id %d."
                                            "This latest Database is not set as cascaded. \n\r"),
                                 db_id, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), fp_database_info.cascaded_coupled_db_id));
                        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit); /* must have active program */
                    }

                    /* 
                     * Encoding of the Cascaded action: 
                     * bits 19:4 for the action value itself 
                     * bits 3:2 to indicate the destination key 
                     * bits 1:0 for the operation type (0 - none, 1 - and, 2 - or, 3 - replace) 
                     */
                    action_vals_lcl[action_indx].val = action_vals_lcl[action_indx].val << 4;
                    action_vals_lcl[action_indx].val |= key_ndx << 2;
                    action_vals_lcl[action_indx].val |= 2; /* OR the result - the LSBs are 0 there, the MSBs are 0 here */
                }
            }
        }
    }

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER,
            action_vals_lcl,
            buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_to_buffer()", db_id, 0);
}

/* 
 * Retrieve the Action values from a buffer (the TCAM Action table value)
 */
uint32
  arad_pp_fp_action_buffer_to_value(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  ARAD_PP_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]
  )
{
    uint32
        action_indx,
      res = SOC_SAND_OK;
    uint32                   
        buffer_lcl[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE];
    ARAD_FP_DATABASE_STAGE
        stage; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* Init all the action_vals to Invalid */
    for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_indx) {
        ARAD_PP_FP_ACTION_VAL_clear(&action_vals[action_indx]);
    }
    sal_memcpy(buffer_lcl, buffer, ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE * sizeof(uint32));

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION,
            action_vals,
            buffer_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Special processing for the Change-Key action to hide the special encoding */
    if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_PMF) {
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_vals[action_indx].type != ARAD_PP_ACTION_TYPE_INVALID); ++action_indx) {
            if (action_vals[action_indx].type == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) {
               /* 
                * Encoding of the Cascaded action: 
                * bits 19:4 for the action value itself 
                */
               action_vals[action_indx].val = action_vals[action_indx].val >> 4;
           }
       }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_buffer_to_value()", db_id, 0);
}


/* Direct extraction legacy code */
/*
 *  Return the Action output size (number of
 *  significant bits)
 */
uint32
  arad_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  )
{
  uint32
      action_lsb_egress,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_TYPE_MAX_SIZE_GET);

  res = arad_pmf_db_fes_action_size_get_unsafe(
            unit,
            fp_action_type,
            ARAD_FP_DATABASE_STAGE_INGRESS_PMF, /* only ingress PMF has FEMs */
            action_size_in_bits,
            &action_lsb_egress
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *action_size_in_bits_in_fem = *action_size_in_bits;
  /* Special case for Change Key where the LSB are not encoded by the user */
  if (fp_action_type == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY)
  {
    *action_size_in_bits_in_fem = *action_size_in_bits + ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
  }

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_max_size_get()", 0, 0);
}


/* 
 * Get the location inside the 32b Direct extraction key 
 * of a specific field and its length 
 * Assumption: no field is split into 2 Copy Engines (of 16b) 
 */
uint32
  arad_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_IN  ARAD_PP_FP_QUAL_TYPE         qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  )
{
  uint32
    qual_lsb_lcl = 0,
      exist_progs[1],
      db_prog,
      prog_result,
      ce_indx,
    res;
  uint32
    qual_size_in_bits_no_padding = 0;
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; /* Single stage with FEMs */
  ARAD_PP_FP_KEY_DP_PROG_INFO
      db_prog_info;
  ARAD_PMF_CE
    sw_db_ce;
  ARAD_PP_FP_DATABASE_INFO
    database_info;
  uint8
      is_equal = FALSE,
      is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_QUAL_LSB_AND_LENGTH_GET);

  if (SOC_IS_ARADPLUS(unit)) {
      /*
       * Get the DB info
       */
      ARAD_PP_FP_DATABASE_INFO_clear(&database_info);
      res = arad_pp_fp_database_get_unsafe(
              unit,
              db_id_ndx,
              &database_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      if ((database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB)
          && (qual_type == SOC_PPC_FP_QUAL_IS_EQUAL))
      {
          is_equal = TRUE;
      }
  }

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id_ndx,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  db_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,db_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  if(prog_result == 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 20, exit); /* must have active program */
  }

  /* get CE used for this DB */
  res = arad_pp_fp_db_prog_info_get(
          unit,
          stage,
          db_id_ndx,
          db_prog,
          &db_prog_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  is_found = FALSE;
  *qual_lsb = 0;
  *qual_length_no_padding = 0;

  if (SOC_IS_ARADPLUS(unit)
      && is_equal) 
  {
      /* No need to look for CE for this qualifier, since
       * it's not a part of the key construction, but overrides 
       * whatever is located in specific location (bits 31:27) 
       * of the direct extraction key.  
       */
      *qual_lsb = 27;
      *qual_length_no_padding = 4;
  }
  else 
  {
      /* for CE, get which bits of qualifier it writes and where */
      for(ce_indx = 0; ce_indx < db_prog_info.nof_ces; ++ce_indx) 
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
              unit,
              stage,
              db_prog,
              db_prog_info.cycle,
              db_prog_info.ces[db_prog_info.nof_ces - ce_indx - 1], /* Start by the highest CE-ID */
              &sw_db_ce
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

        if (sw_db_ce.qual_type == qual_type)
        {
            /*
            * Get the qualifier size without padding: 
            */
            is_found = TRUE;
            qual_lsb_lcl += sw_db_ce.lsb; /* sw_db_ce.lsb is the LSB inside the Key */
            qual_size_in_bits_no_padding = (sw_db_ce.msb - sw_db_ce.lsb + 1); /* Number of intersting bits in the CE */
            break;
        }

        /* If not found, add the size of the Copy Engine to the qualifier */
        qual_lsb_lcl += (sw_db_ce.msb + 1);
      }

      if (!is_found) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                   unit, db_id_ndx, db_prog, db_prog_info.cycle, ARAD_PP_FP_QUAL_TYPE_to_string(qual_type)));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit);
      }

      *qual_lsb = qual_lsb_lcl;
      *qual_length_no_padding = qual_size_in_bits_no_padding;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_lsb_and_length_get()", 0, 0);
}



/* 
 * Given the Database and a specific bit index in the key, 
 * retrieve the qualifier type and the Qual LSB 
 */
STATIC
    uint32
      arad_pp_fp_qual_type_and_local_lsb_get(
        SOC_SAND_IN  int                  unit,
        SOC_SAND_IN  uint32                   bit_ndx,
        SOC_SAND_IN  uint32                   db_id_ndx,
        SOC_SAND_OUT ARAD_PP_FP_QUAL_TYPE         *qual_type,
        SOC_SAND_OUT uint32                   *qual_lsb
      )
{
  uint32
      qual_indx,
      qual_length_no_padding,
      key_lsb,
    res;
  ARAD_PP_FP_DATABASE_INFO
    database_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_QUAL_TYPE_AND_LOCAL_LSB_GET);

  *qual_type = ARAD_PP_NOF_FP_QUAL_TYPES;
  *qual_lsb = 0;

  /*
   * Get the DB info
   */
  ARAD_PP_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(qual_indx = 0;  (qual_indx < ARAD_PP_FP_NOF_QUALS_PER_DB_MAX) 
       && (database_info.qual_types[qual_indx]!= ARAD_PP_NOF_FP_QUAL_TYPES); ++qual_indx) {
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                db_id_ndx,
                database_info.qual_types[qual_indx],
                &key_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        if ((bit_ndx >= key_lsb) && (bit_ndx < key_lsb + qual_length_no_padding)) {
            *qual_type = database_info.qual_types[qual_indx];
            *qual_lsb = bit_ndx - key_lsb; 
            break;
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_type_and_local_lsb_get()", 0, 0);
}

uint32
  arad_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  )
{
  uint32
    action_size_in_bits,
    action_size_in_bits_in_fem,
    action_lsb_lcl = 0,
    res;
  uint32
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_LSB_GET);

  for (action_type_ndx = 0; action_type_ndx < ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (fp_database_info->action_types[action_type_ndx] != action_type)
    {
      /*
       * Get the action size
       */
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              fp_database_info->action_types[action_type_ndx],
              &action_size_in_bits,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      action_lsb_lcl += action_size_in_bits;
    }
    else
    {
      break;
    }
  }

  *action_lsb = action_lsb_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_lsb_get()", 0, 0);
}


uint32
  arad_pp_fp_fem_duplicate(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_orig,
    SOC_SAND_IN  uint32                    fem_id_dest
  )
{
  uint32
      pmf_pgm_ndx,
    res = SOC_SAND_OK;
  ARAD_PMF_FEM_NDX
    fem_ndx_orig,
    fem_ndx_dest;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    action_format_map_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    selected_bits_info;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PP_FP_FEM_ENTRY
    fem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_DUPLICATE);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Duplicate FEM %d to FEM %d \n\r"), 
             fem_id_orig, fem_id_dest));

  /*
   * Duplicate in the counter-packet-flow order:
   * Action (Ethernet: #2), Action map, 4b select and then input
   */
  ARAD_PMF_FEM_NDX_clear(&fem_ndx_orig);
  fem_ndx_orig.id = fem_id_orig;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
  res = arad_pmf_fem_action_format_get_unsafe(
          unit,
          &fem_ndx_orig,
          ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx_dest);
  fem_ndx_dest.id = fem_id_dest;
  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          &fem_ndx_dest,
          ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&action_format_map_info);
    res = arad_pmf_fem_action_format_map_get_unsafe(
            unit,
            &fem_ndx_orig,
            ARAD_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            &fem_ndx_dest,
            ARAD_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_bits_info);
  res = arad_pmf_fem_select_bits_get_unsafe(
          unit,
          &fem_ndx_orig,
          ARAD_PMF_FEM_PGM_FOR_ETH,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          &fem_ndx_dest,
          ARAD_PMF_FEM_PGM_FOR_ETH,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); ++pmf_pgm_ndx)
  {
    ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
    res = arad_pmf_db_fem_input_get_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_orig,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_dest,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  /* 
   * Replicate the SW DB and cancel the original FEM
   */
  ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_id_dest,
          fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

  ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_id_orig,
          fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_duplicate()", 0, 0);
}



/* 
 * Check if entry_ndx_to_insert importance vs entry_ndx_to_compare 
 * FEM is blocking if  entry_ndx_to_compare is more important than 
 *  entry_ndx_to_insert
 */
uint32
  arad_pp_fp_fem_is_fem_blocking_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx_to_insert, 
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx_to_compare,
    SOC_SAND_OUT uint8                    *is_fem_blocking
  )
{
  uint32
    db_strength_strong,
    db_strength_weak,
    entry_strength_strong,
    entry_strength_weak,
    res = SOC_SAND_OK;
  uint8
    forbidden_fem_cycle2_found = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_insert);
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_compare);

  /*
   * Verify if the second FEM info is  stronger
   */
  if (entry_ndx_to_insert->action_type[0] == entry_ndx_to_compare->action_type[0])
  {
    /*
     * Compare the DB strengths
     */
    db_strength_strong = entry_ndx_to_insert->db_strength;
    db_strength_weak = entry_ndx_to_compare->db_strength;

    /* The strongest strength is the closest to zero */
    if (db_strength_weak > db_strength_strong)
    {
      forbidden_fem_cycle2_found = TRUE;
    }
    else if ((db_strength_strong == db_strength_weak)
              && (entry_ndx_to_insert->db_id == entry_ndx_to_compare->db_id)
              && (entry_ndx_to_insert->is_for_entry == TRUE))
    {
      /*
       * Compare the Entry strengths
       */
      entry_strength_strong = entry_ndx_to_insert->entry_strength;
      entry_strength_weak = entry_ndx_to_compare->entry_strength;

      /* The strongest strength is the closest to zero */
      if (entry_strength_weak > entry_strength_strong)
      {
        forbidden_fem_cycle2_found = TRUE;
      }
    }
  }

  *is_fem_blocking = forbidden_fem_cycle2_found;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_fem_blocking_get()", 0, 0);
}

/*
 *  Conversion FP to PMF actions
 */
uint32
  arad_pp_fp_action_type_to_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE    fp_action_type,
    SOC_SAND_OUT uint32  *pmf_fem_action_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_TYPE_TO_PMF_CONVERT);

  /* 1x1 mapping in Arad, conversion in lower level */
    *pmf_fem_action_type = fp_action_type;

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_to_pmf_convert()", 0, 0);
}

uint32
  arad_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               pmf_fem_action_type,
    SOC_SAND_OUT ARAD_PP_FP_ACTION_TYPE    *fp_action_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_TYPE_FROM_PMF_CONVERT);

  /* 1x1 mapping in Arad, conversion in lower level */
  *fp_action_type = pmf_fem_action_type;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_from_pmf_convert()", 0, 0);
}





STATIC
  uint32
    arad_pp_fp_fem_set(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  ARAD_PMF_FEM_NDX             *fem,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE        action_type,
      SOC_SAND_IN  uint32                    extraction_lsb,
      SOC_SAND_IN  uint32                    nof_bits,
      SOC_SAND_IN  uint32                    base_value,
      SOC_SAND_IN  uint32                   db_id
    )
{
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  uint32
    res;
  uint32
    bit_loc_lsb,
    bit_ndx,
    bit_loc_ndx,
    selected_bits_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_SET);

  /*
   *    Selected-bits: no influence
   */
  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
  fem_selected_bits_info.sel_bit_msb = 3;
  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          fem,
          ARAD_PMF_FEM_PGM_FOR_ETH,
          &fem_selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *    Maps to Action 2
   */
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
  fem_action_format_map_info.map_data = 0;
  fem_action_format_map_info.action_fomat_id = ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX;
  fem_action_format_map_info.is_action_valid = TRUE;
  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            fem,
            ARAD_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &fem_action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  /*
   *    Set the action
   */
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
  res = arad_pp_fp_action_type_to_pmf_convert(
          unit,
          action_type,
          &(fem_action_format_info.type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  fem_action_format_info.size = nof_bits;
  fem_action_format_info.base_value = base_value;

  /*
   *    1st stage: only one-to-one mapping from the lsb
   */
  bit_loc_lsb = 0;


  bit_ndx = bit_loc_lsb;
  for (bit_loc_ndx = 0; bit_loc_ndx < nof_bits - bit_loc_lsb; ++bit_loc_ndx)
  {
    fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
    fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + bit_loc_ndx;

    bit_ndx ++;
  }

  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          fem,
          ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &fem_action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_set()", 0, 0);
}


uint32
  arad_pp_fp_fem_configure(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_IN  ARAD_PP_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    selected_bits_ndx_msb,
    selected_bits_ndx_lsb,
    select_msb,
    select_lsb,
    expected_select,
    get_select,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    qual_length_no_padding,
    extraction_lsb,
    action_lsb,
    qual_lsb,
    res = SOC_SAND_OK;
  ARAD_PP_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_loc_ndx,
    bit_ndx,
    fld_ndx,
      pmf_pgm_ndx,
      exist_progs,
    selected_bits_ndx;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  int32
    u64_ndx;
  uint8
    is_qual_bit_valid,
    is_action_applied;
  ARAD_PP_FP_KEY_DP_PROG_INFO
      db_prog_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURE);

  /*
   * Get the DB info
   */
  ARAD_PP_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * 2 cases: Direct Extraction entries or TCAM/Direct Table Database
   * Otherwise, return error
   * The configuration of the FEM must be anti-packet-flow
   */
  /*
   * Set all except FEM input
   */
  res = arad_pp_fp_action_lsb_get(
          unit,
          entry_ndx->action_type[0],
          &database_info,
          &action_lsb
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_pp_fp_action_type_max_size_get(
          unit,
          entry_ndx->action_type[0],
          &action_size_in_bits_max,
          &action_size_in_bits_in_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_ndx;
  if (
      ((database_info.db_type == ARAD_PP_FP_DB_TYPE_TCAM)
        ||(database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_TABLE))
      && (entry_ndx->is_for_entry == FALSE)
      )
  {
    res = arad_pp_fp_fem_set(
            unit,
            &fem_ndx,
            entry_ndx->action_type[0],
            action_lsb,
            action_size_in_bits_in_fem,
            0 /* Base value */,
            entry_ndx->db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (
            (database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == TRUE)
          )
  {
    /*
     *  Set the action according to the params
     */
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pp_fp_action_type_to_pmf_convert(
            unit,
            entry_ndx->action_type[0],
            &(fem_action_format_info.type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    fem_action_format_info.size = action_size_in_bits_in_fem;
    fem_action_format_info.base_value = fem_info->base_val;

    /*
     *	Get the bit mapping
     */
    bit_ndx = 0;


    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
        if (fem_info->fld_ext[fld_ndx].type != SOC_PPC_NOF_FP_QUAL_TYPES) {
          res = arad_pp_fp_qual_lsb_and_length_get(
                  unit,
                  entry_ndx->db_id,
                  fem_info->fld_ext[fld_ndx].type,
                  &extraction_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                  &qual_length_no_padding
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

          if (fem_info->fld_ext[fld_ndx].nof_bits + fem_info->fld_ext[fld_ndx].fld_lsb > qual_length_no_padding) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                  "Field number of bits %d, Field lsb %d, Field length without padding %d.\n\r"),
                       unit, ARAD_PP_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), fem_info->fld_ext[fld_ndx].nof_bits,
                       fem_info->fld_ext[fld_ndx].fld_lsb, qual_length_no_padding));
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 43, exit);
          }
        }
        else {
            extraction_lsb = 0;
        }

      for (bit_loc_ndx = 0; bit_loc_ndx < fem_info->fld_ext[fld_ndx].nof_bits; ++bit_loc_ndx)
      {
          /* Constant Value case - only with NOF for action type */
          if ((fem_info->fld_ext[fld_ndx].cst_val != 0) && (fem_info->fld_ext[fld_ndx].type == SOC_PPC_NOF_FP_QUAL_TYPES))
          {
            fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_CST;
            fem_action_format_info.bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(fem_info->fld_ext[fld_ndx].cst_val, bit_loc_ndx);
          }
          else {
            fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
            fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + bit_loc_ndx;
          }


        bit_ndx ++;
      }
    }

    res = arad_pmf_fem_action_format_set_unsafe(
            unit,
            &fem_ndx,
            ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     * Set the 4b selection and the mapping according to the mask
     */
    /*
     *	Selected-bits: get the MSB of the four bits mask
     *  In practice, assumption the mask is only in the first uint32 of u64
     */
    /* Get the first valid bit from MSB */
    for (u64_ndx = ARAD_PP_FP_FEM_SAND_U64_NOF_BITS - 1; u64_ndx >= 0; u64_ndx--)
    {
      if (u64_ndx >= 32)
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[1], (u64_ndx - 32));
      }
      else
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[0], u64_ndx);
      }

      if (is_qual_bit_valid > 0)
      {
        break;
      }
    }

    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    fem_selected_bits_info.sel_bit_msb = 3;
    qual_lsb = 0;
    qual_length_no_padding = 0;
    if (is_qual_bit_valid) { /* Filter found, verify validity */
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                entry_ndx->db_id,
                qual_info->type,
                &qual_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        if (u64_ndx > qual_length_no_padding) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                  "Field number of bits %d, Field length without padding %d.\n\r"),
                       unit, ARAD_PP_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), u64_ndx,
                       qual_length_no_padding));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 53, exit);
        }
        fem_selected_bits_info.sel_bit_msb = SOC_SAND_MAX(qual_lsb + u64_ndx, 3);
    }
    res = arad_pmf_fem_select_bits_set_unsafe(
            unit,
            &fem_ndx,
            ARAD_PMF_FEM_PGM_FOR_ETH,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /*
     *  Maps to Action 2 according to the mask
     */
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
    fem_action_format_map_info.map_data = 0;
    fem_action_format_map_info.action_fomat_id = ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX;
    /* See which bits to consider in the qualifier bitspace */
    select_msb = (fem_selected_bits_info.sel_bit_msb > qual_lsb)? (fem_selected_bits_info.sel_bit_msb - qual_lsb) : 0;
    select_lsb = (fem_selected_bits_info.sel_bit_msb > qual_lsb + 3)? (select_msb - 3) : 0;
    /* See which bits to consider in the selected_bits_ndx bitspace */
    selected_bits_ndx_lsb = (qual_lsb > (fem_selected_bits_info.sel_bit_msb - 3))? (qual_lsb - (fem_selected_bits_info.sel_bit_msb - 3)) : 0;
    selected_bits_ndx_msb = 3;
    expected_select = SOC_SAND_GET_BITS_RANGE(qual_info->is_valid.arr[0], select_msb, select_lsb)
                      & SOC_SAND_GET_BITS_RANGE(qual_info->val.arr[0], select_msb, select_lsb);
    for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
    {
      get_select = SOC_SAND_GET_BITS_RANGE(qual_info->is_valid.arr[0], select_msb, select_lsb) 
                    & SOC_SAND_GET_BITS_RANGE(selected_bits_ndx, selected_bits_ndx_msb, selected_bits_ndx_lsb);
      if ((expected_select == get_select) || (!is_qual_bit_valid))
      {
        is_action_applied = TRUE;
      }
      else
      {
        is_action_applied = FALSE;
      }

      fem_action_format_map_info.is_action_valid = is_action_applied;
      res = arad_pmf_fem_action_format_map_set_unsafe(
              unit,
              &fem_ndx,
              ARAD_PMF_FEM_PGM_FOR_ETH,
              selected_bits_ndx,
              &fem_action_format_map_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

  }
  else if (
            (database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == FALSE)
          )
  {
    /* Skip this step */
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 100, exit);
  }

  /*
   * Set the input according to the Database type
   */
  ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
    /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

  for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); ++pmf_pgm_ndx)
  {
    if ((exist_progs & (1 << pmf_pgm_ndx)) == 0)
    {
      continue;
    }
    fem_input_info.db_id = ARAD_PP_FP_NOF_DBS;
    fem_input_info.pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH;
    /* get CE used for this DB */
    res = arad_pp_fp_db_prog_info_get(
            unit,
            ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            pmf_pgm_ndx,
            &db_prog_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

    fem_input_info.db_id = entry_ndx->db_id; /* No importance, just for prints */
    if ((database_info.db_type == ARAD_PP_FP_DB_TYPE_TCAM)
        || (database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_TABLE))
    {
        fem_input_info.src_arad.is_key_src = FALSE; /* Form TCAM PDs */
        fem_input_info.src_arad.key_lsb = 0; 
        fem_input_info.src_arad.key_tcam_id = db_prog_info.key_id[0]; 
        fem_input_info.src_arad.lookup_cycle_id = db_prog_info.cycle;
    }
    else if (database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
    {
      fem_input_info.src_arad.is_key_src = TRUE; 
      fem_input_info.src_arad.key_tcam_id = db_prog_info.key_id[0];
      fem_input_info.src_arad.lookup_cycle_id = db_prog_info.cycle;

      if (database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB) {
          /* In case this Database is for compare, take 32 MSB bits of the key */
          fem_input_info.src_arad.key_lsb = 128; 
      }
      else{
          /* Always take the first 32b of the half-Key */
          fem_input_info.src_arad.key_lsb = (db_prog_info.alloc_place == ARAD_PP_FP_KEY_CE_HIGH)? 80 : 0; 
      }
    }
    else
    {
        fem_input_info.src_arad.is_nop = TRUE; 
    }
    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_ndx,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);
  }

  /*
   * SW DB set
   */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_id_ndx,
          *entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configure()", 0, 0);
}

/*
 * Only for Direct Extraction entries
 */
uint32
  arad_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_OUT ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT ARAD_PP_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    bit_ndx_lsb,
    local_lsb,
    key_bit_last,
      fem_id_absolute,
      action_size_in_bits_max,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  ARAD_PP_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_ndx,
      selected_bits_ndx_mask_last,
    selected_bits_ndx_all,
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  int32
    fld_ndx,
    u64_ndx,
    u64_ndx2;
  uint8
      has_a_valid_entry[2],
      is_one,
    mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS],
    val_mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS];
  ARAD_PMF_FEM_BIT_LOC_TYPE
    type_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURATION_GET);

  fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
  /*
   * Get the DB info
   */
  ARAD_PP_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * SW DB get
   */
  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_absolute;
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_id_absolute,
          entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (
       (database_info.db_type == ARAD_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
       && (entry_ndx->is_for_entry == TRUE)
      )
  {
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[0],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


    /*
     * Get the action
     */
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pmf_fem_action_format_get_unsafe(
            unit,
            &fem_ndx,
            ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pp_fp_action_type_from_pmf_convert(
            unit,
            fem_action_format_info.type,
            &(fem_info->type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    fem_info->base_val = fem_action_format_info.base_value;

    /*
     *  Get the bit mapping
     */
    type_previous = ARAD_NOF_PMF_FEM_BIT_LOC_TYPES;
    key_bit_last = 0;
    fld_ndx = -1; /* -1 Due to the ++ for each new field */

    /* Special case for the Change Key action */
    bit_ndx_lsb = 0;
    if (fem_info->type == ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY)
    {
      bit_ndx_lsb = ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
    }

    for (bit_ndx = bit_ndx_lsb; bit_ndx < action_size_in_bits_in_fem; ++bit_ndx)
    {
      if (fem_action_format_info.bit_loc[bit_ndx].type == ARAD_PMF_FEM_BIT_LOC_TYPE_KEY)
      {
        if (
            (type_previous == fem_action_format_info.bit_loc[bit_ndx].type)
            && (key_bit_last + 1 == fem_action_format_info.bit_loc[bit_ndx].val)
            && (fld_ndx >= 0)
           )
        {
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          continue;
        }
        else
        {
          /*
           * New Field
           */
          fld_ndx ++;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          res = arad_pp_fp_qual_type_and_local_lsb_get(
                  unit,
                  key_bit_last,
                  entry_ndx->db_id,
                  &(fem_info->fld_ext[fld_ndx].type),
                  &(fem_info->fld_ext[fld_ndx].fld_lsb)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
        }
      }
      else if (fem_info->type != ARAD_PP_FP_ACTION_TYPE_CHANGE_KEY) /* CST, none for Key change */
      {
        if (type_previous == fem_action_format_info.bit_loc[bit_ndx].type && fld_ndx >= 0)
        {
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, fem_info->fld_ext[fld_ndx].nof_bits);
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          continue;
        }
        else
        {
          /*
           * New field
           */
          fld_ndx ++;
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, 0);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
        }
      }
    }
    fem_info->nof_fields = fld_ndx + 1;


    /*
     * Get the mask
     */
    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    res = arad_pmf_fem_select_bits_get_unsafe(
            unit,
            &fem_ndx,
            ARAD_PMF_FEM_PGM_FOR_ETH,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /* 
     * Get the Value and Mask, of type 0x1x 
     * Assumption that the configuration of the 16 possible values 
     * are with this TCAM format 
     * Loop on the four bits, 
     *   loop on 0/1
     *     see if there is at least a valid value (xxx1 and xxx0)  
     */
    selected_bits_ndx_mask_last = 0;
    for (selected_bits_ndx = 0; selected_bits_ndx < ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; ++selected_bits_ndx)
    {
        for (is_one = FALSE; is_one <= TRUE; ++is_one)
        {
            has_a_valid_entry[is_one] = FALSE;
            for (selected_bits_ndx_all = 0; (selected_bits_ndx_all <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX) && (!has_a_valid_entry[is_one]); ++selected_bits_ndx_all)
            {
                if (SOC_SAND_GET_BIT(selected_bits_ndx_all, selected_bits_ndx) != is_one) {
                    /* The intersting bit has not the correct value is_one */
                    continue;
                }

              ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
              res = arad_pmf_fem_action_format_map_get_unsafe(
                      unit,
                      &fem_ndx,
                      ARAD_PMF_FEM_PGM_FOR_ETH,
                      selected_bits_ndx_all,
                      &fem_action_format_map_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

              has_a_valid_entry[is_one] |= fem_action_format_map_info.is_action_valid;
            }
        }

      /* Update the mask */
      if (has_a_valid_entry[0] && has_a_valid_entry[1])
      {
        mask[selected_bits_ndx] = 0;
        val_mask[selected_bits_ndx] = 0;
      }
      else 
      {
        selected_bits_ndx_mask_last = selected_bits_ndx;
        mask[selected_bits_ndx] = 0x1;
        val_mask[selected_bits_ndx] = (has_a_valid_entry[1])? 1 : 0;
      }
    }

    res = arad_pp_fp_qual_type_and_local_lsb_get(
            unit,
            fem_selected_bits_info.sel_bit_msb - 3 + selected_bits_ndx_mask_last,
            entry_ndx->db_id,
            &(qual_info->type),
            &(local_lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    /* Report the valid bits in the result */
    for (bit_ndx = ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; bit_ndx > 0; bit_ndx--)
    {
      u64_ndx = local_lsb - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      u64_ndx2 = selected_bits_ndx_mask_last - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      SOC_SAND_SET_BIT(qual_info->is_valid.arr[0], mask[u64_ndx2], u64_ndx);
      SOC_SAND_SET_BIT(qual_info->val.arr[0], val_mask[u64_ndx2], u64_ndx);
      if (u64_ndx == 0)
      {
        break;
      }
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configuration_de_get()", 0, 0);
}






uint32
  arad_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      pmf_pgm_ndx,
    cycle_ndx,
      fem_id_absolute,
    fem_id_ndx;
  ARAD_PP_FP_FEM_ENTRY
    fem_entry;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_NDX
    fem_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_REMOVE);

  /*
   * Look at the two cycles for the relevant FEMs
   */
  /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
  /* coverity[same_on_both_sides] */
  for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    for (fem_id_ndx = 0; fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP; ++fem_id_ndx)
    {
        fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
      ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
              unit,
              ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
              fem_id_absolute,
              &fem_entry
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      if (entry_ndx->db_id == fem_entry.db_id)
      {
        if (
            (entry_ndx->is_for_entry == FALSE)
            || (entry_ndx->entry_id == fem_entry.entry_id)
           )
        {
          /*
           * Disable this FEM: SW DB and NOP in input for all PFGs
           */
          ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
                  unit,
                  ARAD_FP_DATABASE_STAGE_INGRESS_PMF,
                  fem_id_absolute,
                  fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

          ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
          for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); ++pmf_pgm_ndx)
          {
            fem_input_info.db_id = 0; /* No importance */
            fem_input_info.pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH;
            fem_input_info.src_arad.is_nop = TRUE;
            res = arad_pmf_db_fem_input_set_unsafe(
                    unit,
                    pmf_pgm_ndx,
                    FALSE, /* is_for_fes */
                    fem_id_absolute,
                    &fem_input_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          }

          /*
           * Set all the mapping to action 3
           */
          ARAD_PMF_FEM_NDX_clear(&fem_ndx);
          fem_ndx.id = fem_id_absolute;

          ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
          fem_action_format_map_info.map_data = 0;
          fem_action_format_map_info.action_fomat_id = ARAD_PMF_FEM_ACTION_DEFAULT_NOP_3; /* No real need for a NOP - valid bit instead */
          fem_action_format_map_info.is_action_valid = FALSE;
          for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
          {
            res = arad_pmf_fem_action_format_map_set_unsafe(
                    unit,
                    &fem_ndx,
                    ARAD_PMF_FEM_PGM_FOR_ETH,
                    selected_bits_ndx,
                    &fem_action_format_map_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
        }
      }
    }

  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_remove()", 0, 0);
}


/*********************************************************************
*     Compute the best configuration to add greedily Direct
 *     Extraction entries (preference to the new
 *     Database-ID). If set, set all the FEM (selected bits,
 *     actions) and its input. Look at the previous FEM
 *     configuration to shift the FEMs if necessary. The FEM
 *     input can be changed again upon the new TCAM DB
 *     creation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *fem_info,
    SOC_SAND_INOUT ARAD_PMF_FES                 fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint32                         *fes_fem_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32
    output_size_in_bits,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
      nof_fem_fes_per_group,
    res = SOC_SAND_OK;
  uint8
      is_for_fem,
      is_for_tm,
      /* indexing according to 32: 0-15 in general are used, and 0-31 when FES-one-group (cycle=0) */
      is_fem_higher[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    is_fem_lower[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    place_found;
  uint32
    action_ndx,
      fes_prio,
      fem_free_min_ndx,
    fem_free_min,
    fem_free_first=0,
    fem_free_last,
    cycle_ndx,
    nof_free_fems[ARAD_PMF_NOF_CYCLES],
    cycle_to_use,
      fes_id_indx,
      fes_id_indx_2,
    fem_id_ndx;
  ARAD_PP_FP_FEM_ENTRY
    entry_replicated,
    fem_entry;
  ARAD_PP_FP_FEM_CYCLE
    fem_cycle_lcl;
  ARAD_PMF_FEM_NDX
    fem;
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_UNSAFE);

  /* 
   * Common implementation of FES / FEM insertion and computation 
   * Common steps: 
   * - divide the space in 2 cycles 
   * - for each cycle, see if there is some space:
   *    - at least a free FES / FEM
   *    - no blocking FES / FEM in the other group
   *    - no blocking FEM / FES in the other FES/FEM space
   * - if both cycles are free: 
   *    - Use cycle 0 for FEM, 1 for FES (more options for future FES / FEM)
   * - in each group / cycle, 
   *    - Compute the minimum legal FES/FEM: 0 for FES,
   *        action-type size dependent for FEM  
   *    - Compute the blocking FES / FEM of the same group 
   *    - Find the first empty slot and move the blocking FES/FEM upward
   *    - Insert the new FES/FEM in its slot
   * In case of FES with flag check-only: 
   *  - Same logic, just do not make in practice the move upward of
   *  blocking FES / FEM
   *  - Do not insert in practice the new FES/FEM in its slot
   * At removal: 
   *  - find the FES/FEM and remove it
   *  - compress the other FES/FEM in the same group backward
   */

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify
   */
  res = arad_pp_fp_fem_insert_verify(
          unit,
          entry_ndx,
          fem_cycle,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* 
   * For the moment, FEM for Direct extraction entries, 
   * FES for TCAM / Direct table DBs 
   */
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:
                             ((flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)? 
                                    ARAD_PMF_LOW_LEVEL_NOF_FESS:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP);

  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }
  else {
      SOC_SAND_CHECK_NULL_INPUT(fem_info);
  }

  /*
   * Verify if there is a place, otherwise
   * return failure
   */
  place_found = TRUE;
  is_for_tm = ( flags & ARAD_PP_FP_FEM_ALLOC_FES_TM ) ? TRUE : FALSE ; 
  if (!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)) {
      /* no need to check in case of oen FES block - checked before */
      res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              fem_cycle,
              is_for_tm,
              fes_info,
              &place_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (place_found == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
      LOG_DEBUG(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "FEM not found for db_id:%d, entry:%d \n\r"), 
                 entry_ndx->db_id, entry_ndx->entry_id));
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }


  /*
   * Set the cycle to insert it: if fixed cycle, this one
   * Otherwise, the one with least full FEMs
   */
  if (flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP) {
      cycle_to_use = 0;
  }
  else if (fem_cycle->is_cycle_fixed)
  {
    cycle_to_use = fem_cycle->cycle_id;
  }
  else
  {
    /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
    /* coverity[same_on_both_sides] */
    for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
    {
      ARAD_PP_FP_FEM_CYCLE_clear(&fem_cycle_lcl);
      fem_cycle_lcl.is_cycle_fixed = TRUE;
      fem_cycle_lcl.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);
      res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              &fem_cycle_lcl,
              is_for_tm,
              fes_info,
              &place_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      nof_free_fems[cycle_ndx] = 0;

      if (place_found == FALSE)
      {
        continue;
      }

      /* Get the number of free FEMs in this group */
      for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
      {
        fes_id_indx = fem_id_ndx + (fem_cycle_lcl.cycle_id * nof_fem_fes_per_group);
        ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
        if (is_for_fem) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                    unit,
                    stage,
                    fes_id_indx,
                    &fem_entry
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
        else {
            fem_entry.action_type[0] = fes_info[fes_id_indx].is_used? 
                fes_info[fes_id_indx].action_type:SOC_PPC_FP_ACTION_TYPE_INVALID;
        }

        if (fem_entry.action_type[0] == SOC_PPC_FP_ACTION_TYPE_INVALID)
        {
          nof_free_fems[cycle_ndx] ++;
        }
      }
    }

    /* Use preferably cycle 0 for FEM, cycle 1 for FES */
    cycle_ndx = (is_for_fem)? 0:1;
    if (nof_free_fems[cycle_ndx])
    {
      cycle_to_use = cycle_ndx;
    }
    else
    {
      cycle_to_use = 1 - cycle_ndx;
    }
  }

  /*
   * If success, re-organize the FEMs:
   * Find the FEMs to transfer (i.e. duplicate) before inserting it
   * Find the first available FEM, and starts from it
   */
  for (action_ndx = 0; action_ndx < ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    /*
     * Get the first FEM to consider
     * Assumption that the first free FEM means there is no action afterwards
     */
    fem_free_min = 0; /* Good value for FES, since every FES is HW similar */
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && is_for_fem; ++fem_id_ndx)
    {
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = cycle_to_use;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (action_size_in_bits_in_fem <= output_size_in_bits)
      {
        /* Avoid FEM 0 / 1 when the Base-offset is positive */
        if ((output_size_in_bits != 4) || (!(entry_ndx->is_base_positive[action_ndx])))
        {
          fem_free_min = fem_id_ndx;
          break;
        }
      }
    }

    /* 
     * Init the higher-FEM array 
     */
    for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
    {
      is_fem_higher[fem_id_ndx] = 0;
    }

    /*
     *  Assumption: there IS a free FEM
     *  Get the higher priority FEMs/FESs bitmap
     */
    fem_free_min_ndx = nof_fem_fes_per_group;
    for (fem_free_first = fem_free_min; fem_free_first < nof_fem_fes_per_group; ++fem_free_first)
    {
        if (is_for_fem) {
          ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                  unit,
                  stage,
                  fem_free_first + (cycle_to_use * nof_fem_fes_per_group),
                  &fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

          ARAD_PP_FP_FEM_ENTRY_clear(&entry_replicated);
          ARAD_COPY(&entry_replicated, entry_ndx, ARAD_PP_FP_FEM_ENTRY, 1);
          entry_replicated.action_type[0] = entry_ndx->action_type[action_ndx];
          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &entry_replicated,
                  &fem_entry,
                  &(is_fem_higher[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &fem_entry,
                  &entry_replicated,
                  &(is_fem_lower[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          if ((fem_entry.action_type[0] == ARAD_PP_ACTION_TYPE_INVALID) 
              && (fem_free_min_ndx == nof_fem_fes_per_group))
          {
            fem_free_min_ndx = fem_free_first;
          }
        }
        else /* For FES */ {
            fes_id_indx = fem_free_first + (cycle_to_use * nof_fem_fes_per_group);
            if(fes_info[fes_id_indx].is_used && (fes_info[fes_id_indx].action_type == entry_ndx->action_type[action_ndx])) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fes_prio);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
                /* A highest strength is stronger (127 is the strongest) */
                if (fes_prio > entry_ndx->db_strength) {
                    is_fem_higher[fem_free_first] = TRUE;
                }
                else if (fes_prio < entry_ndx->db_strength) {
                    is_fem_lower[fem_free_first] = TRUE;
                }
                
            }
            if ((!fes_info[fes_id_indx].is_used) 
                && (fem_free_min_ndx == nof_fem_fes_per_group)) {
                fem_free_min_ndx = fem_free_first;
            }
        }
    }

    /*
     * Duplication if necessary: 
     * - backward for lower priority 
     * - forward for higher priority 
     */
      fem_free_first = fem_free_min_ndx;
      fem_free_last = fem_free_first;
      /* Loop from next-after-free to end of group */
      for (fem_id_ndx = fem_free_first + 1; fem_id_ndx < nof_fem_fes_per_group; fem_id_ndx++)
      {
        if (is_fem_lower[fem_id_ndx] == TRUE)
        {
          fem_free_last = fem_id_ndx;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP)
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          }
          else {
              /* Copy in the current info */
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); /* From */
              fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); /* To */

                sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                fes_info[fes_id_indx].is_used = 0;
            /* update hardware if needed */
            if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                res = arad_pmf_db_fes_move_unsafe(
                        unit,
                        entry_ndx->entry_id, /* prog_id, */
                        fes_id_indx,
                        fes_id_indx_2,
                        &fes_info[fes_id_indx_2]
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
            }
          }

          fem_free_first = fem_id_ndx;
        }
      }

      /* Backward - in fact loop from first-1 to 0 - cause of all the -1 */
      fem_free_last = fem_free_first;
      for (fem_id_ndx = fem_free_first; fem_id_ndx > 0; fem_id_ndx--)
      {
        if (is_fem_higher[fem_id_ndx - 1] == TRUE)
        {
          fem_free_last = fem_id_ndx - 1;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP)
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
          }
          else {
              /* Copy in the current info */
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); /* From */
              fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); /* To */
              
                sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                fes_info[fes_id_indx].is_used = 0;
            /* update hardware if needed */
            if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                res = arad_pmf_db_fes_move_unsafe(
                        unit,
                        entry_ndx->entry_id, /* prog_id, */
                        fes_id_indx,
                        fes_id_indx_2,
                        &fes_info[fes_id_indx_2]
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
            }
          }

          fem_free_first = fem_id_ndx - 1;
        }
      }

        *fes_fem_id = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);

        /*
         * Configure the last free FEM - in case of FEM
         */
        if (is_for_fem) {
            res = arad_pp_fp_fem_configure(
                    unit,
                    fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                    &entry_replicated,
                    &(fem_info->actions[action_ndx]),
                    &(fem_info->qual_vals[0])
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
        }
        else {
            fes_info[*fes_fem_id].is_used = 1;
            fes_info[*fes_fem_id].db_id = entry_ndx->db_id;
            fes_info[*fes_fem_id].action_type = entry_ndx->action_type[0];
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_VERIFY);

  res = ARAD_PP_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FP_FEM_CYCLE, fem_cycle, 15, exit);
  if (entry_ndx->is_for_entry) {
    /* Verify only for FEM */
      res = ARAD_PP_FP_DIR_EXTR_ENTRY_INFO_verify(unit, fem_info, ARAD_FP_DATABASE_STAGE_INGRESS_PMF, FALSE /* is_large_direct_extraction*/);
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_verify()", 0, 0);
}

uint32
  arad_pp_fp_fem_is_place_get_for_cycle(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE            *fem_info,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                    *place_found
  )
{
  uint32
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    output_size_in_bits,
    res = SOC_SAND_OK;
  uint32
      fem_fes_group_ndx,
      nof_fem_fes_per_group,
      nof_fem_fes_groups,
      nof_fem_fes_per_group_other_type,
    action_ndx,
    fem_id_ndx;
  uint8
      is_higher_prio, /* boolean - if FALSE, then lower priority - no dont care */
      is_other_type_fes_fem, /* If FES, 1 for FEM and conversely */
      is_for_fem,
      fes_is_found = FALSE,
    fem_is_found = FALSE;
  ARAD_PP_FP_FEM_ENTRY
      fem_entry_reference,
    fem_entry;
  ARAD_PMF_FEM_NDX
    fem;
  ARAD_PP_FP_ACTION_TYPE
    fem_action_type_bmp[ARAD_PMF_LOW_LEVEL_FEM_ID_MAX + 1];
  ARAD_PP_FP_FEM_ENTRY
    fem_entry_high,
    fem_entry_low;
  uint8
    forbidden_fem_cycle2_found = FALSE;
  uint32
      fes_id_indx,
      cycle_curr, /* Cycle of the current FES / FEM */
    other_cycle;
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Try to find FEM/FES for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  /* 
   * For the moment, FEM for Direct extraction entries, 
   * FES for TCAM / Direct table DBs 
   */
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_per_group_other_type = (!is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_groups = 2; /* Always 2 groups */

  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }

  /*
   * Verify if there is a stronger / weaker FEM in the
   * other cycle.
   * Assumption: do not remove FEMs from the other cycle at the end (HW set) 
   *  
   * Besides, verify if there is a blocking FES (in case of FEM or conversely) 
   * for each cycle  
   */
  other_cycle = ARAD_PMF_NOF_CYCLES - fem_info->cycle_id - 1;
  for (action_ndx = 0; action_ndx < ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry_reference);
    ARAD_COPY(&fem_entry_reference, entry_ndx, ARAD_PP_FP_FEM_ENTRY, 1);
    fem_entry_reference.action_type[0] = entry_ndx->action_type[action_ndx];
    /*
     * Loop on: 
     * - Type to look at: current type first (FES/FEM), other type then (FEM/FES) 
     * - Number of groups to look at (1 for current type), 2 for the other type 
     * - For each index, look if blocking by: 
     *  - see if higher or lower priority (cannot be dont care since any other group
     *      has a priority vs the current group)
     *  - Copy into FEM entry type
     *  - Use blocking
     * - When blocking found, stop everything
     */
    for (is_other_type_fes_fem = FALSE; (is_other_type_fes_fem <= TRUE)
                                 && (forbidden_fem_cycle2_found == FALSE); is_other_type_fes_fem++) {
        for (fem_fes_group_ndx = 0; fem_fes_group_ndx < (is_other_type_fes_fem?nof_fem_fes_groups:1 /* Only other cycle otherwise */)
                                 && (forbidden_fem_cycle2_found == FALSE); fem_fes_group_ndx++) {
            for (fem_id_ndx = 0; (fem_id_ndx < (is_other_type_fes_fem? nof_fem_fes_per_group_other_type:nof_fem_fes_per_group)) 
                                 && (forbidden_fem_cycle2_found == FALSE); ++fem_id_ndx)
            {
                /* 
                 * Compute when the other FES/FEM is at higher priority
                 */
                if (!is_other_type_fes_fem) {
                    /* Between FEMs or FESs, cycle 1 higher priority */
                    is_higher_prio = (other_cycle == 1);
                }
                else if (is_for_fem && (other_cycle == 0)) {
                    /* FEMs at cycle 1 are always higher priority */
                    is_higher_prio = FALSE;
                }
                else if (is_for_fem && (other_cycle == 1)) {
                    /* FEMs at cycle 0 are higher priority if FES < 16 */
                    is_higher_prio = fem_fes_group_ndx;
                }
                else if ((!is_for_fem) && (other_cycle == 1)) {
                    /* FESs at cycle 0 are lowest priority */
                    is_higher_prio = TRUE;
                }
                else /* (!is_for_fem) && (other_cycle == 0) */ {
                    /* FES at cycle 1 are more important than FEM cycle 0 but not cycle 1*/
                    is_higher_prio = fem_fes_group_ndx;
                }
                /* Cycle of this FES / FEM */
                cycle_curr = is_other_type_fes_fem? fem_fes_group_ndx:other_cycle;

                /* 
                 * See if this FEM or FES is blocking
                 */
                ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry_high);
                ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry_low);
                ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
                /* Get the current FES/FEM absolute index */
                fes_id_indx = fem_id_ndx + (cycle_curr * (is_other_type_fes_fem? nof_fem_fes_per_group_other_type: nof_fem_fes_per_group));
                if (is_for_fem? (!is_other_type_fes_fem): is_other_type_fes_fem) {
                    /* The object to compare with is a FEM */
                  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                          unit,
                          stage,
                          fes_id_indx,
                          &fem_entry
                        );
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
                }
                else /* For FES */ {
                    if(fes_info[fes_id_indx].is_used) {
                        fem_entry.is_for_entry = FALSE;
                        fem_entry.db_id = fes_info[fes_id_indx].db_id;
                        fem_entry.action_type[0] = fes_info[fes_id_indx].action_type;

                        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fem_entry.db_strength);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
                    }
                }

              /*
               * Switch the Higher FEM info according to the priority
               */
              if (is_higher_prio)
              {
                ARAD_COPY(&fem_entry_high, &fem_entry, ARAD_PP_FP_FEM_ENTRY, 1);
                ARAD_COPY(&fem_entry_low, &fem_entry_reference, ARAD_PP_FP_FEM_ENTRY, 1);
              }
              else
              {
                ARAD_COPY(&fem_entry_high, &fem_entry_reference, ARAD_PP_FP_FEM_ENTRY, 1);
                ARAD_COPY(&fem_entry_low, &fem_entry, ARAD_PP_FP_FEM_ENTRY, 1);
              }

              res = arad_pp_fp_fem_is_fem_blocking_get(
                      unit,
                      &fem_entry_high,
                      &fem_entry_low,
                      &forbidden_fem_cycle2_found
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }
  }
  if (forbidden_fem_cycle2_found == TRUE)
  {
    *place_found = FALSE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   * No forbidden FEM found, now look if there is enough free FEMs
   * with the minimal size in the same cycle
   * Build a bitmap to fill all along
   */
  for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fes_is_found == FALSE); ++fem_id_ndx)
  {
      if (is_for_fem) {
        /*
         *  Get the FEM info
         */
        ARAD_PP_FP_FEM_ENTRY_clear(&fem_entry);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                unit,
                stage,
                fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group),
                &fem_entry
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        fem_action_type_bmp[fem_id_ndx] = fem_entry.action_type[0];
      }
      else /* For FES */ {
          fes_id_indx = fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group);
          if (!fes_info[fes_id_indx].is_used) {
              fes_is_found = TRUE;
              LOG_DEBUG(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "   "
                                    "FES found for db_id:%d, entry:%d, action: %s , FES-ID: %d\n\r"), 
                         entry_ndx->db_id, entry_ndx->entry_id, 
                         SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), fes_id_indx));
          }
      }
  }

  for (action_ndx = 0; (action_ndx < ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX); action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    fem_is_found = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fem_is_found == FALSE) && is_for_fem; ++fem_id_ndx)
    {
      /*
       *  Check the FEM has sufficient place to copy all the needed bits
       *  Get the FEM size and the action type size
       */
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = fem_info->cycle_id;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Avoid FEM 0 / 1 when the Base-offset is positive */
      if (action_size_in_bits_in_fem > output_size_in_bits)
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is too small \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits));
        continue;
      }

      if ((output_size_in_bits == 4) && (entry_ndx->is_base_positive[action_ndx] == TRUE))
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is 4b and has a positive base (0/1) %d \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits,
                     entry_ndx->is_base_positive[action_ndx]));
        continue;
      }

      if (fem_action_type_bmp[fem_id_ndx] == ARAD_PP_ACTION_TYPE_INVALID)
      {
        fem_is_found = TRUE;
        fem_action_type_bmp[fem_id_ndx] = entry_ndx->action_type[action_ndx];
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM found for db_id:%d, entry:%d, action: %s (size: %d). FEM-ID: %d, max-size %d  \n\r"), 
                   entry_ndx->db_id, entry_ndx->entry_id, 
                   SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                   fem_id_ndx, output_size_in_bits));

      }
    }
    if ( (is_for_fem && (fem_is_found == FALSE)) || ((!is_for_fem) && (!fes_is_found)))
    {
      *place_found = FALSE;
      ARAD_PP_DO_NOTHING_AND_EXIT;
    }
  }

  *place_found = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_for_cycle()", 0, 0);
}


/*********************************************************************
*     Check out if there is an empty FEM for this entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE            *fem_info,
    SOC_SAND_IN  uint8                        is_for_tm,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                    *place_found
  )
{
  uint32
      program_rsrc = 0,
    res = SOC_SAND_OK;
  uint32
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
    pmf_pgm_ndx,
    cycle_ndx,
    fes_idx;
  uint8
      is_for_fem,
      fem_is_found_lcl,
    fem_is_found = FALSE;
  ARAD_PP_FP_FEM_CYCLE
    fem_info_cycle;
  ARAD_PMF_FES         
      fes_info_lcl[ARAD_PMF_LOW_LEVEL_NOF_FESS]; /* The current FES info if FES, per PMF-Program if FEM */
  ARAD_FP_DATABASE_STAGE
      stage = ARAD_FP_DATABASE_STAGE_INGRESS_PMF; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Find FEM for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  /*
   * Verify
   */
  res = arad_pp_fp_fem_is_place_get_verify(
          unit,
          entry_ndx,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Copy in case of FES the FES-info*/
  is_for_fem = entry_ndx->is_for_entry;
  if (!is_for_fem) {
      sal_memcpy(fes_info_lcl, fes_info, ARAD_PMF_LOW_LEVEL_NOF_FESS * sizeof(ARAD_PMF_FES));
  }
  else {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  }

  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            stage,
            is_for_tm /* is_for_tm */, 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  /*
   * If cycle fixed, try it.
   * If not, try both cycles and return success if at least one succeeded
   */
  /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
  /* coverity[same_on_both_sides] */
  for (cycle_ndx = 0; (cycle_ndx < ARAD_PMF_NOF_CYCLES) && (fem_is_found == FALSE); ++cycle_ndx)
  {
    if ((fem_info->is_cycle_fixed == TRUE) && (cycle_ndx != fem_info->cycle_id))
    {
      continue;
    }
    
    ARAD_PP_FP_FEM_CYCLE_clear(&fem_info_cycle);
    fem_info_cycle.is_cycle_fixed = TRUE; /* Look only for this cycle */
    fem_info_cycle.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);

    
    fem_is_found_lcl = TRUE;
    for (pmf_pgm_ndx = pmf_pgm_ndx_min; (pmf_pgm_ndx < pmf_pgm_ndx_max) && (fem_is_found_lcl == TRUE); ++pmf_pgm_ndx)
    {
        /* Go over this only once for FES */
        if ((!is_for_fem) && (pmf_pgm_ndx != pmf_pgm_ndx_min)) {
            break;
        }

        /* For FEM, get the FES info. */
        if (is_for_fem) {
            if (!SHR_BITGET(&program_rsrc, pmf_pgm_ndx)) {
                /* Program not valid, continue */
                continue;
            }

            for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                          unit,
                          stage,
                          pmf_pgm_ndx,
                          fes_idx,
                          &fes_info_lcl[fes_idx]
                      );
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
            }
        }

        res = arad_pp_fp_fem_is_place_get_for_cycle(
                unit,
                entry_ndx,
                &fem_info_cycle,
                fes_info_lcl,
                &fem_is_found_lcl
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    /* False if failed for a Program, True otherwise */
    fem_is_found = fem_is_found_lcl;
  }

  *place_found = fem_is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE            *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY);

  res = ARAD_PP_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FP_FEM_CYCLE, fem_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_fem_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_fp_fem;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_fem_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_fp_fem;
}
uint32
  ARAD_PP_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY *info
  )
{
  uint32
    action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_strength, ARAD_PP_FP_FEM_DB_STRENGTH_MAX, ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, ARAD_PP_FP_FEM_DB_ID_MAX, ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_strength, ARAD_PP_FP_FEM_ENTRY_STRENGTH_MAX, ARAD_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->entry_id, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit), ARAD_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR, 14, exit);
  for (action_ndx = 0; action_ndx < ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      if (action_ndx && (!info->is_for_entry)) {
          /* FES, single action */
          break;
      }

    if (info->action_type[action_ndx] == ARAD_PP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(info->action_type[action_ndx], ARAD_PP_FP_FEM_ACTION_TYPE_MAX, ARAD_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 20 + action_ndx, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FP_FEM_ENTRY_verify()",0,0);
}

uint32
  ARAD_PP_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FP_FEM_CYCLE_verify()",0,0);
}


void
  ARAD_PP_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT ARAD_PP_FP_FEM_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_ENTRY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FEM_ACTIONS_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FEM_ACTIONS_CONSTRAINT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_PP_FEM_ACTIONS_CONSTRAINT));
  info->cycle = ARAD_PP_FP_KEY_CYCLE_ANY;
  info->action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  ARAD_PP_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT ARAD_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_CYCLE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

void
  ARAD_PP_FP_FEM_ENTRY_print(
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_ENTRY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FP_FEM_CYCLE_print(
    SOC_SAND_IN  ARAD_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_CYCLE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif /* of #if defined(BCM_88650_A0) */

