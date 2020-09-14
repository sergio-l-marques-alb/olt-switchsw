/** \file diag_dnx_field_map.c
 * $Id$
 *
 * Semantic tests for BCM to DNX, DNX to HW data
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "ctest_dnx_field_map.h"
#include <bcm_int/dnx/field/field_map.h>
#include <../src/bcm/dnx/field/map/field_map_local.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static sh_sand_man_t field_map_qual_bcm_test_man = {
    .brief = "Verify validity of qualifier mapping BCM to DNX",
};

static sh_sand_man_t field_map_qual_dnx_test_man = {
    .brief = "Verify validity of DNX qualifiers and reverse mapping DNX to BCM",
};

static sh_sand_man_t field_map_action_bcm_test_man = {
    .brief = "Verify validity of action mapping BCM to DNX",
};

static sh_sand_man_t field_map_action_dnx_test_man = {
    .brief = "Verify validity of DNX actions and reverse mapping DNX to BCM",
};

static sh_sand_man_t field_map_negative_callbacks_test_man = {
    .brief = "Verify wrong input produces correct errors in map callback functions",
};

#define FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, first_mc, nof_mc, size_mc, field_mc, bcm_mc, id_mc, stage_mc, class_mc)   \
    if((id_mc < first_mc) || (id_mc >= nof_mc))                                                                        \
    {                                                                                                                  \
        LOG_CLI((BSL_META("%s %s qual id:%d bcm:%s is out of range\n"),                                                \
            dnx_field_stage_text(unit,stage_mc), dnx_field_qual_class_text(class_mc),                                  \
                id_mc, dnx_field_bcm_qual_text(unit, bcm_mc)));                                                              \
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);                                                                             \
    }                                                                                                                  \
    if(size_mc == 0)                                                                                                   \
    {                                                                                                                  \
        LOG_CLI((BSL_META("%s %s qual id:%d bcm:%s has zero size\n"),                                                  \
            dnx_field_stage_text(unit,stage_mc), dnx_field_qual_class_text(class_mc),                                  \
                id_mc, dnx_field_bcm_qual_text(unit, bcm_mc)));                                                              \
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);                                                                             \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        if(size_mc > 160)                                                                                              \
        {                                                                                                              \
            LOG_CLI((BSL_META("%s %s qual id:%d bcm:%s size:%d is to big\n"),                                          \
                dnx_field_stage_text(unit,stage_mc), dnx_field_qual_class_text(class_mc), id_mc,                       \
                    dnx_field_bcm_qual_text(unit, bcm_mc), size_mc));                                                        \
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);                                                                         \
        }                                                                                                              \
    }                                                                                                                  \
    if(field_mc == 0)                                                                                                  \
    {                                                                                                                  \
        LOG_CLI((BSL_META("%s %s qual id:%d bcm:%s has zero DBAL field ID\n"),                                         \
            dnx_field_stage_text(unit,stage_mc), dnx_field_qual_class_text(class_mc),                                  \
                id_mc, dnx_field_bcm_qual_text(unit, bcm_mc)));                                                              \
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM); \
    }

static shr_error_e
field_map_qual_bcm_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage;
    dnx_field_qual_class_e class;
    bcm_field_qualify_t bcm_qual;
    dnx_field_qual_t dnx_qual;
    dnx_field_qual_id_t qual_id;
    uint32 dnx_qual_size;
    dbal_fields_e qual_dbal_field;
    
    bsl_severity_t original_severity_fld_proc = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        /*
         * Check of it has global mapping
         */
        if ((dnx_qual = dnx_global_qual_map[bcm_qual].dnx_qual) != 0)
        {
            /*
             * For global bcm mapping verify class and associated DNX qualifier
             */
            class = DNX_QUAL_CLASS(dnx_qual);
            qual_id = DNX_QUAL_ID(dnx_qual);
            /*
             * Set default stage and update qualifier
             */
            stage = DNX_FIELD_STAGE_IPMF1;
            dnx_qual |= (stage << DNX_QUAL_STAGE_SHIFT);

            switch (class)
            {
                case DNX_FIELD_QUAL_CLASS_LAYER_RECORD:
                    DNX_FIELD_STAGE_LAYER_QUAL_ITERATOR(stage)
                {
                    dnx_field_map_qual_layer_record_pbus_info_t lr_qual_info = {.valid = FALSE };
                    /*
                     * Verify that the layer qualifier is valid for the stage.
                     */
                    SHR_IF_ERR_CONT(dnx_field_map_qual_layer_record_info_get(unit, stage, qual_id, &lr_qual_info));
                    if (lr_qual_info.valid)
                    {
                        /*
                         * Update qualifier per stage
                         */
                        dnx_qual = DNX_QUAL(class, stage, qual_id);
                        
                        SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                        
                        if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, &dnx_qual_size)))
                        {
                            
                            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                            continue;
                        }
                        
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        SHR_IF_ERR_CONT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &qual_dbal_field));
                        FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, DNX_FIELD_LR_QUAL_FIRST, DNX_FIELD_LR_QUAL_NOF,
                                                       dnx_qual_size, qual_dbal_field, bcm_qual, qual_id, -1, class);
                    }
                }
                    break;
                case DNX_FIELD_QUAL_CLASS_HEADER:
                    
                    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                    
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, &dnx_qual_size)))
                    {
                        
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        continue;
                    }
                    
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_IF_ERR_CONT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &qual_dbal_field));
                    FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, DNX_FIELD_HEADER_QUAL_FIRST, DNX_FIELD_HEADER_QUAL_NOF,
                                                   dnx_qual_size, qual_dbal_field, bcm_qual, qual_id, -1, class);
                    /*
                     * For header qualifier name is requested additionally to general qual params
                     */
                    if (ISEMPTY(dnx_field_header_qual_name(unit, dnx_qual)))
                    {
                        LOG_CLI((BSL_META("header qual id:%d bcm:%s has no name\n"),
                                 qual_id, dnx_field_bcm_qual_text(unit, bcm_qual)));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    break;
                case DNX_FIELD_QUAL_CLASS_SW:
                    
                    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                    
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, &dnx_qual_size)))
                    {
                        
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        continue;
                    }
                    
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_IF_ERR_CONT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &qual_dbal_field));
                    FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, DNX_FIELD_SW_QUAL_FIRST, DNX_FIELD_SW_QUAL_NOF,
                                                   dnx_qual_size, qual_dbal_field, bcm_qual, qual_id, -1, class);
                    break;
                default:
                    LOG_CLI((BSL_META("Class:%s is not eligible for global BCM Qualifier:%s\n"),
                             dnx_field_qual_class_text(class), dnx_field_bcm_qual_text(unit, bcm_qual)));
                    break;
            }
            /*
             * Check that there is no double mapping - to global and per stage
             */
            DNX_FIELD_STAGE_QUAL_ITERATOR(stage)
            {
                if ((qual_id = dnx_field_map_stage_info[stage].meta_qual_map[bcm_qual].dnx_qual) != 0)
                {
                    LOG_CLI((BSL_META("BCM qualifier:%s has both by global in stage:%s mapping to DNX\n"),
                             dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
        else
        {
            DNX_FIELD_STAGE_QUAL_ITERATOR(stage)
            {
                if (stage == DNX_FIELD_STAGE_IPMF2)
                {
                    class = DNX_FIELD_QUAL_CLASS_META2;
                }
                else
                {
                    class = DNX_FIELD_QUAL_CLASS_META;
                }
                if ((qual_id = dnx_field_map_stage_info[stage].meta_qual_map[bcm_qual].dnx_qual) != 0)
                {
                    dnx_qual = DNX_QUAL(class, stage, qual_id);
                    
                    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                    
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, &dnx_qual_size)))
                    {
                        
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        continue;
                    }
                    
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_IF_ERR_CONT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &qual_dbal_field));
                    FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, DNX_FIELD_QUAL_ID_FIRST,
                                                   dnx_field_map_stage_info[stage].meta_qual_nof,
                                                   dnx_qual_size, qual_dbal_field, bcm_qual, qual_id, -1, class);
                }
            }
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
field_map_qual_dnx_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage;
    bcm_field_qualify_t bcm_qual;
    dnx_field_qual_id_t qual_id;
    int bcm_count;
    int bcm_basic_count;
    uint32 size;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_QUAL_ITERATOR(stage)
    {
        for (qual_id = DNX_FIELD_QUAL_ID_FIRST; qual_id < dnx_field_map_stage_info[stage].meta_qual_nof; qual_id++)
        {
            dnx_field_meta_qual_info_t *meta_qual_info = &dnx_field_map_stage_info[stage].meta_qual_info[qual_id];
            if (qual_id >= DNX_FIELD_L4OPS_QUAL_STATISTICS_OBJ_12
                && qual_id <= DNX_FIELD_L4OPS_QUAL_OAM_STATISTICS_OBJ_0)
            {
                /*
                 * Skip statistical object qualifiers as currently they have no mapping
                 */
                continue;
            }
            /*
             * Check there is DBAL field attached
             */
            if (meta_qual_info->field_id == 0)
            {
                LOG_CLI((BSL_META("%s qual id:%d has zero DBAL field ID\n"), dnx_field_stage_text(unit, stage),
                         qual_id));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            /*
             * Check that size is not NULL or more than 160
             */
            rv = dbal_field_struct_field_size_get(unit, dnx_field_map_stage_info[stage].container_qual_field_type,
                                                  meta_qual_info->field_id, &size);
            if (rv != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("%s qual id:%s(%d) dbal_field_id:%d cannot obtain size from DBAL\n"),
                         dnx_field_stage_text(unit, stage),
                         dbal_field_to_string(unit, meta_qual_info->field_id), qual_id, meta_qual_info->field_id));
                SHR_SET_CURRENT_ERR(rv);
            }
            if ((size <= 0) || (size > 160))
            {
                LOG_CLI((BSL_META("%s qual id:%s(%d) size:%d is out of range\n"), dnx_field_stage_text(unit, stage),
                         dbal_field_to_string(unit, meta_qual_info->field_id), qual_id, size));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            /*
             * Now check bcm mapping
             */
            bcm_count = 0;
            bcm_basic_count = 0;
            for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
            {
                if (dnx_field_map_stage_info[stage].meta_qual_map[bcm_qual].dnx_qual == qual_id)
                {
                    bcm_count++;
                    if (dnx_field_map_stage_info[stage].meta_qual_map[bcm_qual].flags & BCM_TO_DNX_BASIC_OBJECT)
                    {
                        bcm_basic_count++;
                    }
                }
            }
            if (bcm_count == 0)
            {
                
            }
            else if (bcm_count > 1)
            {
                if (bcm_basic_count == 0)
                {
                    LOG_CLI((BSL_META("%s qual id:%s(%d) No basic bcm qualifier chosen\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, meta_qual_info->field_id),
                             qual_id));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else if (bcm_basic_count > 1)
                {
                    LOG_CLI((BSL_META("%s qual id:%s(%d) %d basic bcm qualifiers chosen\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, meta_qual_info->field_id),
                             qual_id, bcm_basic_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                /*
                 * BCM Qualifier set on DNX should be basic one - verify it
                 */
                if (!(dnx_field_map_stage_info[stage].meta_qual_map[meta_qual_info->bcm_qual].flags &
                      BCM_TO_DNX_BASIC_OBJECT))
                {
                    LOG_CLI((BSL_META("%s qual id:%s(%d) from from non-basic bcm:%s\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, meta_qual_info->field_id),
                             qual_id, dnx_field_bcm_qual_text(unit, meta_qual_info->bcm_qual)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
field_map_action_bcm_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage;
    dnx_field_action_class_e class;
    bcm_field_action_t bcm_action;
    dnx_field_action_t dnx_action;
    dnx_field_action_id_t action_id;
    dnx_field_base_action_info_t *base_action_info;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_action = 0; bcm_action < bcmFieldActionCount; bcm_action++)
    {
        /*
         * Check of it has global mapping
         */
        if (dnx_global_action_map[bcm_action].dnx_action != 0)
        {
            dnx_action = dnx_global_action_map[bcm_action].dnx_action;
            /*
             * For global bcm mapping verify class and associated DNX qualifier
             */
            class = DNX_ACTION_CLASS(dnx_action);
            action_id = DNX_QUAL_ID(dnx_action);
            switch (class)
            {
                case DNX_FIELD_ACTION_CLASS_SW:
                    /*
                     * Check id validity
                     */
                    if ((action_id < DNX_FIELD_SW_ACTION_FIRST) || (action_id >= DNX_FIELD_SW_ACTION_NOF))
                    {
                        LOG_CLI((BSL_META("SW action id:%d bcm:%s is out of range\n"),
                                 action_id, dnx_field_bcm_action_text(unit, bcm_action)));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    base_action_info = &dnx_sw_action_info[action_id];
                    if (base_action_info->field_id == 0)
                    {
                        LOG_CLI((BSL_META("Global action id:%d has zero DBAL field ID\n"), action_id));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    /*
                     * Check that size is not NULL or more than 160
                     */
                    if ((base_action_info->field_id == DBAL_FIELD_EMPTY) || (base_action_info->size_sw > 32))
                    {
                        LOG_CLI((BSL_META("Global action id:%s(%d) size:%d is out of range\n"),
                                 dbal_field_to_string(unit, base_action_info->field_id), action_id,
                                 base_action_info->size_sw));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    break;
                default:
                    LOG_CLI((BSL_META("Class:%s is not eligible for global BCM action:%s\n"),
                             dnx_field_qual_class_text(class), dnx_field_bcm_action_text(unit, bcm_action)));
                    break;
            }
            /*
             * Check that there is no double mapping - to global and per stage
             */
            DNX_FIELD_STAGE_ACTION_ITERATOR(stage)
            {
                if (dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].dnx_action != 0)
                {
                    LOG_CLI((BSL_META("BCM qualifier:%s has both by global in stage:%s mapping to DNX\n"),
                             dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit, stage)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
        else
        {
            DNX_FIELD_STAGE_ACTION_ITERATOR(stage)
            {
                uint32 size;
                if (dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].dnx_action == 0)
                    continue;

                action_id = dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].dnx_action;
                if (action_id >= dnx_field_map_stage_info[stage].static_action_nof)
                {
                    LOG_CLI((BSL_META("%s action id:%d bcm:%s, is out of range\n"),
                             dnx_field_stage_text(unit, stage), action_id, dnx_field_bcm_action_text(unit,
                                                                                                     bcm_action)));
                }
                base_action_info = &dnx_field_map_stage_info[stage].static_action_info[action_id];
                if (base_action_info->field_id == 0)
                {
                    LOG_CLI((BSL_META("%s action id:%d bcm:%s has zero DBAL field ID\n"),
                             dnx_field_stage_text(unit, stage), action_id, dnx_field_bcm_action_text(unit,
                                                                                                     bcm_action)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                /*
                 * Check that size is not NULL or more than 32
                 */
                if (dbal_field_struct_field_size_get(unit,
                                                     dnx_field_map_stage_info[stage].container_act_field_type,
                                                     base_action_info->field_id, &size) != _SHR_E_NONE)
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) bcm;%s cannot fetch size\n"),
                             dnx_field_stage_text(unit, stage),
                             dbal_field_to_string(unit, base_action_info->field_id),
                             action_id, dnx_field_bcm_action_text(unit, bcm_action)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    continue;
                }

                if ((base_action_info->field_id == DBAL_FIELD_EMPTY) || (size > 32))
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) bcm;%s size:%d is out of range\n"),
                             dnx_field_stage_text(unit, stage),
                             dbal_field_to_string(unit, base_action_info->field_id),
                             action_id, dnx_field_bcm_action_text(unit, bcm_action), size));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
field_map_action_dnx_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage;
    bcm_field_action_t bcm_action;
    dnx_field_action_id_t action_id;
    uint32 size;
    int bcm_count;
    int bcm_basic_count;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_ACTION_ITERATOR(stage)
    {
        for (action_id = 1; action_id < dnx_field_map_stage_info[stage].static_action_nof; action_id++)
        {
            dnx_field_base_action_info_t *base_action_info =
                &dnx_field_map_stage_info[stage].static_action_info[action_id];
            /*
             * Check there is DBAL field attached
             */
            if (base_action_info->field_id == DBAL_FIELD_EMPTY)
            {
                continue;
            }

            if (dbal_field_struct_field_size_get(unit,
                                                 dnx_field_map_stage_info[stage].container_act_field_type,
                                                 base_action_info->field_id, &size) != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("%s action id:%s(%d) failed to obtain size\n"), dnx_field_stage_text(unit, stage),
                         dbal_field_to_string(unit, base_action_info->field_id), action_id));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                continue;
            }
            /*
             * Check that size is not NULL or more than 32
             */
            if ((size == 0) || (size > 32))
            {
                LOG_CLI((BSL_META("%s action id:%s(%d) size:%d is out of range\n"), dnx_field_stage_text(unit, stage),
                         dbal_field_to_string(unit, base_action_info->field_id), action_id, size));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                continue;
            }
            /*
             * Now check bcm mapping
             */
            bcm_count = 0;
            bcm_basic_count = 0;
            for (bcm_action = 0; bcm_action < bcmFieldActionCount; bcm_action++)
            {
                if (dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].dnx_action == 0)
                    continue;

                if (dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].dnx_action == action_id)
                {
                    bcm_count++;
                    if (dnx_field_map_stage_info[stage].static_action_id_map[bcm_action].flags
                        & BCM_TO_DNX_BASIC_OBJECT)
                    {
                        bcm_basic_count++;
                    }
                }
            }
            if (bcm_count == 0)
            {
                
            }
            else if (bcm_count > 1)
            {
                if (bcm_basic_count == 0)
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) No basic bcm action chosen in %d\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, base_action_info->field_id),
                             action_id, bcm_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else if (bcm_basic_count > 1)
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) %d basic bcm actions chosen from:%d\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, base_action_info->field_id),
                             action_id, bcm_basic_count, bcm_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                /*
                 * BCM Action set on DNX should be basic one - verify it
                 */
                if (!(dnx_field_map_stage_info[stage].static_action_id_map[base_action_info->bcm_action].flags &
                      BCM_TO_DNX_BASIC_OBJECT))
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) from from non-basic bcm:%s\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, base_action_info->field_id),
                             action_id, dnx_field_bcm_action_text(unit, base_action_info->bcm_action)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
field_map_negative_callbacks_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t entry_id;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    int rv = BCM_E_NONE;
    int action_idx;
    int stage_idx;
    ctest_field_map_neg_testing_t
        all_actions_array[CTEST_DNX_FIELD_MAP_NEGATIVE_NOF_STAGES][CTEST_DNX_FIELD_MAP_NEGATIVE_IPMF1_ACTIONS] =
    {
        {
            {
            bcmFieldActionLearnKey2, _SHR_E_PARAM, 0xFFACCAA},
            {
            bcmFieldActionRpfOutInterface, _SHR_E_PARAM, 0xFFAA},
            {
            bcmFieldActionRpfOutVport, _SHR_E_PARAM, 0xFFAA},
            {
            bcmFieldActionStatOamLM, _SHR_E_PARAM, 0xFFFFCA},
            {
            bcmFieldActionExtStat1, _SHR_E_PARAM, 0xFFFFCA},
            {
            bcmFieldActionExtStat0, _SHR_E_PARAM, 0xFFFFFFFA},
            {
            bcmFieldActionStatProfile0, _SHR_E_PARAM, 0xFFC},
            {
            bcmFieldActionVrfSet, _SHR_E_PARAM, 0xFFFAA},
            {
            bcmFieldActionOam, _SHR_E_PARAM, 0xFFA},
            {
            bcmFieldActionMirrorIngress, _SHR_E_PARAM, 0xFFA},
            {
            bcmFieldActionInInterface0, _SHR_E_INTERNAL, 0x1AAAFFF},
            {
            bcmFieldActionInVport0, _SHR_E_PARAM, 0xFFA},
            {
            bcmFieldActionLatencyFlowId, _SHR_E_PARAM, 0xFFA},
            {
            bcmFieldActionSrcGportNew, _SHR_E_PORT, 0x1FC},
            {
            bcmFieldActionFabricHeaderSet, _SHR_E_UNAVAIL, 0xF1},
            {
            bcmFieldActionL3Switch, _SHR_E_PARAM, 0xFFFA},
            {
            bcmFieldActionRedirectMcast, _SHR_E_PORT, 0x3EB},
            {
            bcmFieldActionDropPrecedence, _SHR_E_PARAM, 0xFF},
            {
            bcmFieldActionSnoop, _SHR_E_PARAM, 0xFF},
            {
            bcmFieldActionTrap, _SHR_E_PARAM, 0xFF}
        },
        {
            {
            bcmFieldActionStartPacketStrip, _SHR_E_PARAM, 63},
            {
            bcmFieldActionParsingStartType, _SHR_E_PARAM, 0xFFF},
            {
            bcmFieldActionParsingStartOffset, _SHR_E_INTERNAL, 128},
            {
            bcmFieldActionIPTCommand, _SHR_E_PARAM, 0xFFABB}
        },
        {
            {
            bcmFieldActionTrap, _SHR_E_PARAM, 0xFFAAB},
            {
            bcmFieldActionTrapStrength, _SHR_E_PARAM, 0xFFAAB},
            {
            bcmFieldActionSnoop, _SHR_E_PARAM, 0xFFAAB},
            {
        bcmFieldActionSnoopStrength, _SHR_E_PARAM, 0xFFAAB},},
        {
            {
            bcmFieldActionAceContextValue, _SHR_E_UNAVAIL, 60},
            {
            bcmFieldActionStat0, _SHR_E_PARAM, 0x60},
            {
            bcmFieldActionStat1, _SHR_E_PARAM, 0x60}
        }
    };
    int nof_actions[CTEST_DNX_FIELD_MAP_NEGATIVE_NOF_STAGES] = { 20, 4, 4, 3 };
    bcm_field_stage_t stage_list[CTEST_DNX_FIELD_MAP_NEGATIVE_NOF_STAGES] =
        { bcmFieldStageIngressPMF1, bcmFieldStageIngressPMF3, bcmFieldStageEgress, bcmFieldStageEgressExtension };

    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE MAP CALLBACK FUNCTIONS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);
    for (stage_idx = 0; stage_idx < 4; stage_idx++)
    {
        for (action_idx = 0; action_idx < nof_actions[stage_idx]; action_idx++)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Testing stage %s - Index %d             \n"),
                                          dnx_field_bcm_stage_text(stage_list[stage_idx]), action_idx));

            if (stage_list[stage_idx] != bcmFieldStageEgressExtension)
            {
                rv = bcm_field_action_info_get(unit, all_actions_array[stage_idx][action_idx].action_type,
                                               stage_list[stage_idx], &action_info);
                if (rv == BCM_E_NOT_FOUND)
                {
                    LOG_INFO(BSL_LS_APPL_COMMON,
                             (BSL_META_U(unit, "Skipping action %d, not found on this device.\n"), action_idx));
                    continue;
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv);
                }

                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "          Testing action - %s             \n"), (char *) action_info.name));
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

                bcm_field_group_info_t_init(&fg_info);

                fg_info.fg_type = bcmFieldGroupTypeTcam;
                fg_info.stage = stage_list[stage_idx];
                fg_info.nof_actions = 1;
                fg_info.nof_quals = 1;
                fg_info.action_with_valid_bit[0] = TRUE;
                fg_info.action_types[0] = all_actions_array[stage_idx][action_idx].action_type;
                fg_info.qual_types[0] = bcmFieldQualifyVrf;

                SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

                bcm_field_entry_info_t_init(&entry_info);

                entry_info.priority = 10;
                entry_info.nof_entry_actions = fg_info.nof_actions;
                entry_info.nof_entry_quals = fg_info.nof_quals;

                entry_info.entry_action[0].type = fg_info.action_types[0];
                if (all_actions_array[stage_idx][action_idx].action_type == bcmFieldActionParsingStartOffset)
                {
                    all_actions_array[stage_idx][action_idx].value = utilex_power_of_2(action_info.size);
                }
                entry_info.entry_action[0].value[0] = all_actions_array[stage_idx][action_idx].value;

                entry_info.entry_qual[0].type = fg_info.qual_types[0];
                entry_info.entry_qual[0].value[0] = 0x64;
                entry_info.entry_qual[0].mask[0] = 0xFF;

                rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_id);
                if (rv != all_actions_array[stage_idx][action_idx].error_type)
                {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit, "bcm_field_entry_add should fail! . Error:%d (%s), "
                                          "expected %d (%s).\n"),
                               rv, bcm_errmsg(rv), all_actions_array[stage_idx][action_idx].error_type,
                               bcm_errmsg(all_actions_array[stage_idx][action_idx].error_type)));
                    return BCM_E_FAIL;
                }

                SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));
            }
            else
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Testing CB ACE            \n")));
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

                /** Configure the ACE format */
                bcm_field_ace_format_info_t_init(&ace_format_info);

                ace_format_info.nof_actions = 1;
                ace_format_info.action_types[0] = all_actions_array[stage_idx][action_idx].action_type;

                /*
                 * We do not give the TTL action a valid bit.
                 */
                ace_format_info.action_with_valid_bit[0] = FALSE;

                SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

                bcm_field_ace_entry_info_t_init(&ace_entry_info);

                ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

                ace_entry_info.entry_action[0].type = ace_format_info.action_types[0];
                ace_entry_info.entry_action[0].value[0] = all_actions_array[stage_idx][action_idx].value;

                rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
                if (rv != all_actions_array[stage_idx][action_idx].error_type)
                {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit, "bcm_field_entry_add should fail! . Error:%d (%s), "
                                          "expected %d (%s).\n"),
                               rv, bcm_errmsg(rv), all_actions_array[stage_idx][action_idx].error_type,
                               bcm_errmsg(all_actions_array[stage_idx][action_idx].error_type)));
                    return BCM_E_FAIL;
                }
            }
        }
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE MAP CALLBACK FUNCTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_field_map_qual_test_cmds[] = {
    {"bcm", field_map_qual_bcm_test_cmd,  NULL, NULL, &field_map_qual_bcm_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"dnx", field_map_qual_dnx_test_cmd,  NULL, NULL, &field_map_qual_dnx_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};

sh_sand_cmd_t dnx_field_map_action_test_cmds[] = {
    {"bcm", field_map_action_bcm_test_cmd,  NULL, NULL, &field_map_action_bcm_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"dnx", field_map_action_dnx_test_cmd,  NULL, NULL, &field_map_action_dnx_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};

sh_sand_cmd_t dnx_field_map_negative_test_cmds[] = {
    {"callbacks", field_map_negative_callbacks_test_cmd,  NULL, NULL, &field_map_negative_callbacks_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};

sh_sand_cmd_t dnx_field_map_test_cmds[] = {
    {"qualifier", NULL,  dnx_field_map_qual_test_cmds},
    {"action", NULL,  dnx_field_map_action_test_cmds},
    {"negative", NULL,  dnx_field_map_negative_test_cmds},
    {NULL}};
/* *INDENT-OFF* */

