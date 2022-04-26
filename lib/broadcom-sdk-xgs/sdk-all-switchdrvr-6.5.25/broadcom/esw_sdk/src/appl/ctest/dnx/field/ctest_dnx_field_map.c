/** \file diag_dnx_field_map.c
 * $Id$
 *
 * Semantic tests for BCM to DNX, DNX to HW data
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "ctest_dnx_field_map.h"
#include <bcm_int/dnx/field/field_map.h>
#include <../src/bcm/dnx/field/map/field_map_local.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>

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

extern const dnx_field_map_stage_info_t dnx_field_map_stage_info[DNX_FIELD_STAGE_NOF];
extern const dnx_field_action_map_t dnx_global_action_map[bcmFieldActionCount];
static sh_sand_man_t field_map_port_profile_test_man = {
    .brief = "Verify validity of PP and TM port class profiles",
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
    dnx_field_map_stage_info_t field_map_stage_info;
    dnx_field_qual_map_t qual_map;

    SHR_FUNC_INIT_VARS(unit);

    for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_global_qual_map_get_dispatch(unit, bcm_qual, &qual_map));
        /*
         * Check of it has global mapping
         */
        if ((dnx_qual = qual_map.dnx_qual) != DNX_FIELD_QUAL_TYPE_INVALID)
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
                    DNX_FIELD_STAGE_LAYER_QUAL_ITERATOR(stage, field_map_stage_info)
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
                        /** Verify the qualifier is supported by this device */
                        if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, NULL, NULL, &dnx_qual_size)))
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
                    /** Verify the qualifier is supported by this device */
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, NULL, NULL, &dnx_qual_size)))
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
                    /** Verify the qualifier is supported by this device */
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, NULL, NULL, &dnx_qual_size)))
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
            DNX_FIELD_STAGE_QUAL_ITERATOR(stage, field_map_stage_info)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_meta_qual_map_get_dispatch(unit, stage, bcm_qual, &qual_map));
                if ((qual_id = qual_map.dnx_qual) != DNX_FIELD_QUAL_TYPE_INVALID)
                {
                    LOG_CLI((BSL_META("BCM qualifier:%s has both global and stage:%s mapping to DNX\n"),
                             dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, stage)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
        else
        {
            DNX_FIELD_STAGE_QUAL_ITERATOR(stage, field_map_stage_info)
            {
                if (stage == DNX_FIELD_STAGE_IPMF2)
                {
                    class = DNX_FIELD_QUAL_CLASS_META2;
                }
                else
                {
                    class = DNX_FIELD_QUAL_CLASS_META;
                }
                SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, stage, &field_map_stage_info));
                SHR_IF_ERR_EXIT(dnx_field_map_meta_qual_map_get_dispatch(unit, stage, bcm_qual, &qual_map));
                if ((qual_id = qual_map.dnx_qual) != DNX_FIELD_QUAL_TYPE_INVALID)
                {
                    dnx_qual = DNX_QUAL(class, stage, qual_id);
                    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                    /** Verify the qualifier is supported by this device */
                    if (SHR_FAILURE(dnx_field_map_dnx_qual_size(unit, stage, dnx_qual, NULL, NULL, &dnx_qual_size)))
                    {
                        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                        continue;
                    }
                    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
                    SHR_IF_ERR_CONT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &qual_dbal_field));
                    FIELD_MAP_VERIFY_DNX_QUALIFIER(unit, DNX_FIELD_QUAL_ID_FIRST, DNX_FIELD_QUAL_NOF,
                                                   dnx_qual_size, qual_dbal_field, bcm_qual, qual_id, -1, class);
                }
            }
        }
    }
exit:
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
    dnx_field_map_stage_info_t field_map_stage_info;
    dnx_field_qual_map_t qual_map;
    dbal_fields_e qual_dbal = DBAL_FIELD_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_QUAL_ITERATOR(stage, field_map_stage_info)
    {
        for (qual_id = DNX_FIELD_QUAL_ID_FIRST; qual_id < DNX_FIELD_QUAL_NOF; qual_id++)
        {

            /*
             * Offset -1 means qualifier is not supported
             */
            if (dnx_data_field_map.qual.params_get(unit, stage, qual_id)->offset == -1)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.field_id.get(unit, qual_id, &qual_dbal));
            /*
             * Check that size is not more than 160
             */
            size = dnx_data_field_map.qual.params_get(unit, stage, qual_id)->size;
            if (size > 160)
            {
                LOG_CLI((BSL_META("%s qual id:%s(%d) size:%d is out of range\n"), dnx_field_stage_text(unit, stage),
                         dbal_field_to_string(unit, qual_dbal), qual_id, size));
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            /*
             * Now check bcm mapping
             */
            bcm_count = 0;
            bcm_basic_count = 0;
            for (bcm_qual = 0; bcm_qual < bcmFieldQualifyCount; bcm_qual++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_meta_qual_map_get_dispatch(unit, stage, bcm_qual, &qual_map));
                if (qual_map.dnx_qual == qual_id)
                {
                    bcm_count++;
                    if (BCM_TO_DNX_IS_BAISC_OBJ(qual_map.flags))
                    {
                        bcm_basic_count++;
                    }
                }
            }
            /*
             * If qualifier is mapped checked that it has only 1 Main mapped object
             */
            if (bcm_count > 0)
            {
                if (bcm_basic_count == 0)
                {
                    LOG_CLI((BSL_META("%s qual id:%s(%d) No basic bcm qualifier chosen\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, qual_dbal), qual_id));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else if (bcm_basic_count > 1)
                {
                    LOG_CLI((BSL_META("%s qual id:%s(%d) %d basic bcm qualifiers chosen\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, qual_dbal),
                             qual_id, bcm_basic_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }

            }
        }
    }
exit:
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
    const dnx_field_action_map_t *action_map_p;
    dnx_field_base_action_info_t sw_action_info;
    int size = 0;
    dbal_fields_e dbal;
    SHR_FUNC_INIT_VARS(unit);

    for (bcm_action = 0; bcm_action < bcmFieldActionCount; bcm_action++)
    {
        /*
         * Check of it has global mapping
         */
        action_map_p = &dnx_global_action_map[bcm_action];
        if (action_map_p->dnx_action != DNX_FIELD_ACTION_ID_INVALID)
        {
            dnx_action = action_map_p->dnx_action;
            /*
             * For global bcm mapping verify class and associated DNX qualifier
             */
            class = DNX_ACTION_CLASS(dnx_action);
            action_id = DNX_QUAL_ID(dnx_action);
            switch (class)
            {
                case DNX_FIELD_ACTION_CLASS_SW:
                    SHR_IF_ERR_EXIT(dnx_field_map_sw_action_info_get_dispatch(unit, action_id, &sw_action_info));
                    if (sw_action_info.field_id == 0)
                    {
                        LOG_CLI((BSL_META("Global action id:%d has zero DBAL field ID\n"), action_id));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    /*
                     * Check that size is not NULL or more than 160
                     */
                    if ((sw_action_info.field_id == DBAL_FIELD_EMPTY) || (sw_action_info.size_sw > 32))
                    {
                        LOG_CLI((BSL_META("Global action id:%s(%d) size:%d is out of range\n"),
                                 dbal_field_to_string(unit, sw_action_info.field_id), action_id,
                                 sw_action_info.size_sw));
                        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                    }
                    break;
                default:
                    LOG_CLI((BSL_META("Class:%s is not eligible for global BCM action:%s\n"),
                             dnx_field_action_class_text(class), dnx_field_bcm_action_text(unit, bcm_action)));
                    break;
            }
            /*
             * Check that there is no double mapping - to global and per stage
             */
            DNX_FIELD_STAGE_ACTION_ITERATOR(stage, dnx_field_map_stage_info)
            {

                action_map_p = &dnx_field_map_stage_info[stage].static_action_id_map[bcm_action];
                if (action_map_p->dnx_action != DNX_FIELD_ACTION_ID_INVALID)
                {
                    LOG_CLI((BSL_META("BCM action:%s has both global and stage:%s mapping to DNX\n"),
                             dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit, stage)));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
        else
        {
            DNX_FIELD_STAGE_ACTION_ITERATOR(stage, dnx_field_map_stage_info)
            {

                action_map_p = &dnx_field_map_stage_info[stage].static_action_id_map[bcm_action];
                action_id = action_map_p->dnx_action;

                if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage, action_id) == FALSE)
                {
                    continue;
                }

                size = DNX_FIELD_DNX_ACTION_PARAM_GET(stage, action_id, size);
                SHR_IF_ERR_EXIT(dnx_field_action_sw_db.predefined.field_id.get(unit, action_id, &dbal));
                if (size < 1 || size > 32)
                {
                    LOG_CLI((BSL_META("BCM TEST: %s action id:%s(%d) size:%d is out of range\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, dbal), action_id, size));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
    }
exit:
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
    const dnx_field_action_map_t *action_map_p;
    dbal_fields_e dbal;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_ACTION_ITERATOR(stage, dnx_field_map_stage_info)
    {
        for (action_id = DNX_FIELD_ACTION_FIRST; action_id < DNX_FIELD_ACTION_NOF; action_id++)
        {
            /*
             * Check if action configured on stage
             */
            if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage, action_id) == FALSE)
            {
                continue;
            }

            size = DNX_FIELD_DNX_ACTION_PARAM_GET(stage, action_id, size);

            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.predefined.field_id.get(unit, action_id, &dbal));
            /*
             * Check that size is not NULL or more than 32
             */
            if (size > 32)
            {
                LOG_CLI((BSL_META("DNX TEST: %s action id:%s(%d) size:(%d) is out of range\n"),
                         dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, dbal), action_id, size));
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
                action_map_p = &dnx_field_map_stage_info[stage].static_action_id_map[bcm_action];
                if (action_map_p->dnx_action == DNX_FIELD_ACTION_ID_INVALID)
                    continue;

                if (action_map_p->dnx_action == action_id)
                {
                    bcm_count++;
                    if (BCM_TO_DNX_IS_BAISC_OBJ(action_map_p->flags))
                    {
                        bcm_basic_count++;
                    }
                }
            }
            /*
             * If action is mapped checked that it has only 1 Main mapped object
             */
            if (bcm_count > 0)
            {
                if (bcm_basic_count == 0)
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) No basic bcm action chosen in %d\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, dbal),
                             action_id, bcm_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else if (bcm_basic_count > 1)
                {
                    LOG_CLI((BSL_META("%s action id:%s(%d) %d basic bcm actions chosen from:%d\n"),
                             dnx_field_stage_text(unit, stage), dbal_field_to_string(unit, dbal),
                             action_id, bcm_basic_count, bcm_count));
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
            }
        }
    }
exit:
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

                SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

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
                    return (_SHR_E_FAIL);
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
                    return (_SHR_E_FAIL);
                }
            }
        }
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE MAP CALLBACK FUNCTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_map_port_profile_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv = BCM_E_NONE;
    int pclass_idx;
    int port = 201;
    bcm_gport_t gport_local;
    bcm_gport_t gport_system;
    int pclass_size;
    uint32 old_val;
    uint32 val_get;
    uint32 new_val_pass;
    uint32 new_val_fail;
    dnx_field_stage_e dnx_stage;

    ctest_field_map_neg_cs_port_profile_testing_t pclass_arr[] = {
        {bcmPortClassFieldIngressPMF1PacketProcessingPort, bcmFieldQualifyPortClassPacketProcessing,
         bcmFieldStageIngressPMF1},
        {bcmPortClassFieldIngressPMF1PacketProcessingPortCs, bcmFieldQualifyPortClassPacketProcessing,
         bcmFieldStageIngressPMF1, TRUE},
        {bcmPortClassFieldIngressPMF3PacketProcessingPort, bcmFieldQualifyPortClassPacketProcessing,
         bcmFieldStageIngressPMF3},
        {bcmPortClassFieldIngressPMF3PacketProcessingPortCs, bcmFieldQualifyPortClassPacketProcessing,
         bcmFieldStageIngressPMF3, TRUE},
        {bcmPortClassFieldEgressPacketProcessingPort, bcmFieldQualifyPortClassPacketProcessing, bcmFieldStageEgress},
        {bcmPortClassFieldEgressPacketProcessingPortCs, bcmFieldQualifyPortClassPacketProcessing, bcmFieldStageEgress,
         TRUE},
        {bcmPortClassFieldExternalPacketProcessingPortCs, bcmFieldQualifyPortClassPacketProcessing,
         bcmFieldStageExternal, TRUE},
        {bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData},
        {bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh},
        {bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData},
        {bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh},
        {bcmPortClassFieldIngressPMF1TrafficManagementPort, bcmFieldQualifyPortClassTrafficManagement,
         bcmFieldStageIngressPMF1},
        {bcmPortClassFieldIngressPMF1TrafficManagementPortCs, bcmFieldQualifyPortClassTrafficManagement,
         bcmFieldStageIngressPMF1, TRUE},
        {bcmPortClassFieldIngressPMF3TrafficManagementPort, bcmFieldQualifyPortClassTrafficManagement,
         bcmFieldStageIngressPMF3},
        {bcmPortClassFieldIngressPMF3TrafficManagementPortCs, bcmFieldQualifyPortClassTrafficManagement,
         bcmFieldStageIngressPMF3, TRUE},
        {bcmPortClassFieldEgressTrafficManagementPort, bcmFieldQualifyPortClassTrafficManagement, bcmFieldStageEgress},
        {bcmPortClassFieldEgressTrafficManagementPortCs, bcmFieldQualifyPortClassTrafficManagement, bcmFieldStageEgress,
         TRUE},
        {bcmPortClassFieldExternalTrafficManagementPortCs, bcmFieldQualifyPortClassTrafficManagement,
         bcmFieldStageExternal, TRUE}
    };

    int nof_pclass = sizeof(pclass_arr) / sizeof(pclass_arr[0]);
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "PORT PROFILE MAP CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

    BCM_GPORT_LOCAL_SET(gport_local, port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport_system, port);

    /*
     * Set the profiles. 
     */
    for (pclass_idx = 0; pclass_idx < nof_pclass; pclass_idx++)
    {
        /*
         * PacketProcessingPortGeneralData and PacketProcessingPortGeneralDataHigh share the same qualifier of size 40
         */
        if (pclass_arr[pclass_idx].pclass == bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData
            || pclass_arr[pclass_idx].pclass == bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData)
        {
            pclass_size = 32;
        }
        else if (pclass_arr[pclass_idx].pclass == bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh
                 || pclass_arr[pclass_idx].pclass == bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh)
        {
            pclass_size = 8;
        }
        /*
         * CS port classes have different sizes than entry port classes
         */
        else if (pclass_arr[pclass_idx].is_cs)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, pclass_arr[pclass_idx].stage, &dnx_stage));
            SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_size_get
                            (unit, dnx_stage, pclass_arr[pclass_idx].qual_type, 0, &pclass_size));
        }
        else
        {
            bcm_field_qualifier_info_get_t qual_info;
            SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                            (unit, pclass_arr[pclass_idx].qual_type, pclass_arr[pclass_idx].stage, &qual_info));
            pclass_size = qual_info.size;
        }
        if (pclass_size > 32)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error, size of port class %d is %d, larger than 32.\r\n",
                         pclass_arr[pclass_idx].pclass + 0, pclass_size);
        }

        /*
         * Not all port classes are supported for all devices
         */
        if (pclass_size == 0)
        {
            continue;
        }
        new_val_pass = 1 << (pclass_size - 1);
        new_val_fail = 1 << pclass_size;
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_local, pclass_arr[pclass_idx].pclass, &old_val));

        /*
         * Negative testing
         */
        /*
         * Try to set port class larger than allowed
         */
        if (pclass_size < 32)
        {
            rv = bcm_port_class_set(unit, gport_local, pclass_arr[pclass_idx].pclass, new_val_fail);
            if (rv == BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Error, in bcm_port_class_set, pclass %d value 0x%x, expected to fail.\r\n",
                             pclass_arr[pclass_idx].pclass + 0, new_val_fail);
            }
        }

        /*
         * Positive testing
         */
        /*
         * Set new value and compare to get
         */
        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_local, pclass_arr[pclass_idx].pclass, new_val_pass));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_local, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != new_val_pass)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error, after setting value, value is 0x%x instead of 0x%x.\r\n", val_get,
                         new_val_pass);
        }

        /*
         * Return old value and compare to get
         */
        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_local, pclass_arr[pclass_idx].pclass, old_val));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_local, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != old_val)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Error, after returning value to its previous value, value is 0x%x instead of 0x%x.\r\n",
                         val_get, old_val);
        }

        /*
         * Set using system port
         */
        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_system, pclass_arr[pclass_idx].pclass, new_val_pass));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_system, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != new_val_pass)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error, after setting value, value is 0x%x instead of 0x%x.\r\n", val_get,
                         new_val_pass);
        }

        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_system, pclass_arr[pclass_idx].pclass, old_val));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_system, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != old_val)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Error, after returning value to its previous value, value is 0x%x instead of 0x%x.\r\n",
                         val_get, old_val);
        }

        /*
         * Set using non gport port (taken as logical port).
         */
        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, pclass_arr[pclass_idx].pclass, new_val_pass));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, port, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != new_val_pass)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error, after setting value, value is 0x%x instead of 0x%x.\r\n", val_get,
                         new_val_pass);
        }
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, gport_local, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != new_val_pass)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error, value read as local port gport is 0x%x instead of 0x%x.\r\n", val_get,
                         new_val_pass);
        }
        SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, pclass_arr[pclass_idx].pclass, old_val));
        SHR_IF_ERR_EXIT(bcm_port_class_get(unit, port, pclass_arr[pclass_idx].pclass, &val_get));
        if (val_get != old_val)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Error, after returning value to its previous value, value is 0x%x instead of 0x%x.\r\n",
                         val_get, old_val);
        }
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "PORT PROFILE MAP CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

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
    {"port_profile", field_map_port_profile_test_cmd, NULL, NULL, &field_map_port_profile_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"negative", NULL,  dnx_field_map_negative_test_cmds},
    {NULL}
};
/* *INDENT-OFF* */

