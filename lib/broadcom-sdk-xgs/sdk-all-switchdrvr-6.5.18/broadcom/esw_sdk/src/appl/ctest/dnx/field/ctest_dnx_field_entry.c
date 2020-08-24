/** \file diag_dnx_field_entry.c
 * $Id$
 *
 * Semantic test for entry DNX APIs.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_entry.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

 /*
  * Number of PMF stages 
  */
#define NUM_STAGES                 4
#define NUM_QUALS_AND_ACTIONS      3
#define NUM_FGS                    1
#define NUM_ENTRIES                3
 /*
  * FG IDs we also use for selecting qualifiers and actions arrays 
  */
#define FG_ID_80B                 15
#define FG_ID_160B                20

#define FIRST_LAYER                1

#define ENTRY_PRIORITY             5

/*
 * The various values available for 'mode' input.
 */

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */

/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for test type on 'group' command (data base testing)
 *   Type can be either BCM or DNX
 */
#define DNX_DIAG_FIELD_ENTRY_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for stage of test on 'group' command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define DNX_DIAG_FIELD_ENTRY_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"
/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'entry' shell command
 * \remark
 *   
 */
sh_sand_option_t dnx_field_entry_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_FIELD_ENTRY_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",                  "DNX", (void *)Field_level_enum_table},
    {DNX_DIAG_FIELD_ENTRY_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",             "ipmf1", (void *)Field_stage_enum_table},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                      "1"},
    {NULL}
    /** End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'entry' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_entry_tests[] = {
    {"DNX_field_entry_add_1", "type=DNX stage=ipmf1 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_field_entry_add_2", "type=DNX stage=ipmf2 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_field_entry_add_3", "type=DNX stage=ipmf3 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_field_entry_add_e", "type=DNX stage=epmf count=6 ",  CTEST_POSTCOMMIT},
    {"BCM_field_entry_add_1", "type=BCM stage=ipmf1 count=6 ", CTEST_POSTCOMMIT},
    /* Temporary moving BCM entry_add_2 to POSTCOMMIT since it adds same entry multiple times
     * and it's now illegal, so it fails and needs to be fixed.
     */
    {"BCM_field_entry_add_2", "type=BCM stage=ipmf2 count=6 ", CTEST_POSTCOMMIT},
    {"BCM_field_entry_add_3", "type=BCM stage=ipmf3 count=6 ", CTEST_POSTCOMMIT},
    {"BCM_field_entry_add_e", "type=BCM stage=epmf count=6 ",  CTEST_POSTCOMMIT},
    {NULL}
};
/*
 * }
 */
/**
 *  Details for semantic field entry test.
 */
sh_sand_man_t sh_dnx_field_entry_man = {
    "Field entry related test utilities",
    "Start a semantic test for field entry, add, get, compare and destroy, with verification. "
        "Currently there is only DNX-level testing."
        "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n",
    "ctest field entry type=<BCM | DNX> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>",
    "type=DNX stage=IPMF1\r\n",
};
/* Qualifers per stage add up to 80b , so the FG we create with them will be 80b */
static dnx_field_qual_t qual_array[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
    { {DNX_FIELD_HEADER_QUAL_MPLS_LABEL, DNX_FIELD_HEADER_QUAL_MPLS_TC, DNX_FIELD_HEADER_QUAL_MPLS_TTL},
{DNX_FIELD_IPMF2_QUAL_CMP_KEY_0_DECODED, DNX_FIELD_IPMF2_QUAL_TRJ_HASH, DNX_FIELD_IPMF2_QUAL_NASID},
{DNX_FIELD_IPMF3_QUAL_SNOOP_CODE, DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_0, DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_0},
{DNX_FIELD_EPMF_QUAL_PPH_NWK_QOS, DNX_FIELD_EPMF_QUAL_OUT_TM_PORT, DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT}
};
/* Qualifers per stage add up to 80b , so the FG we create with them will be 80b */
static bcm_field_qualify_t bcm_qual_array[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
{
    {bcmFieldQualifyMplsLabel1, bcmFieldQualifyMplsLabel1Exp, bcmFieldQualifyMplsLabel1Ttl},
    {bcmFieldQualifyCompareKeysResult0, bcmFieldQualifyHashValue, bcmFieldQualifyCompareKeysResult1},
    {bcmFieldQualifyRxTrapCodeForSnoop, bcmFieldQualifyInVPort0, bcmFieldQualifyKeyGenVar},
    {bcmFieldQualifyIntPriority, bcmFieldQualifySrcClassField, bcmFieldQualifyL4PortRangeCheck}
};
/* Qualifers per stage add up to 160b , so the FG we create with them will be 160b */
static dnx_field_qual_t qual_array2[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
{ {DNX_FIELD_HEADER_QUAL_IPV4_SRC, DNX_FIELD_HEADER_QUAL_IPV4_DST, DNX_FIELD_HEADER_QUAL_ETHERTYPE},
{DNX_FIELD_IPMF2_QUAL_LAG_LB_KEY, DNX_FIELD_IPMF2_QUAL_TRJ_HASH, DNX_FIELD_IPMF2_QUAL_CMP_KEY_0_DECODED},
{DNX_FIELD_IPMF3_QUAL_RPF_DST, DNX_FIELD_IPMF3_QUAL_PTC_KEY_GEN_VAR, DNX_FIELD_IPMF3_QUAL_CONTEXT_KEY_GEN_VAR},
{DNX_FIELD_EPMF_QUAL_OUT_TM_PORT, DNX_FIELD_EPMF_QUAL_PPH_NWK_QOS, DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT}
};
/* Qualifers per stage add up to 160b , so the FG we create with them will be 160b */
static bcm_field_qualify_t bcm_qual_array2[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
{
    {bcmFieldQualifySrcIp, bcmFieldQualifyDstIp, bcmFieldQualifyEtherTypeUntagged},
    {bcmFieldQualifyEcmpLoadBalanceKey0, bcmFieldQualifyEcmpLoadBalanceKey1, bcmFieldQualifyEcmpLoadBalanceKey2},
    {bcmFieldQualifyDstRpfGport, bcmFieldQualifyPortClassPacketProcessing, bcmFieldQualifyKeyGenVar},
    {bcmFieldQualifyIntPriority, bcmFieldQualifyDstClassField, bcmFieldQualifyL4PortRangeCheck}
};
/* Actions per stage add up to 32b , so the FG we create with them will be 32b */
static dnx_field_action_t action_array[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
    { {DBAL_ENUM_FVAL_IPMF1_ACTION_TC, DBAL_ENUM_FVAL_IPMF1_ACTION_DP, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT},
{DBAL_ENUM_FVAL_IPMF1_ACTION_TC, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0,
 DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET},
{DBAL_ENUM_FVAL_IPMF3_ACTION_DP, DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN, DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0},
{DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE, DBAL_ENUM_FVAL_EPMF_ACTION_TC, DBAL_ENUM_FVAL_EPMF_ACTION_DP}
};
/* Actions per stage add up to 32b , so the FG we create with them will be 32b */
static bcm_field_action_t bcm_action_array[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
{ {bcmFieldActionPphPresentSet, bcmFieldActionDoNotLearn, bcmFieldActionTtlSet},
{bcmFieldActionPphPresentSet, bcmFieldActionStatId0, bcmFieldActionInVportClass0},
{bcmFieldActionPrioIntNew, bcmFieldActionSystemHeaderSet, bcmFieldActionStatId0},
{bcmFieldActionQosMapIdNew, bcmFieldActionPrioIntNew, bcmFieldActionStatId0}
};
/* Actions per stage add up to 32b , so the FG we create with them will be 32b(Supposed to be 64b , but 64b not yet supported) */
static dnx_field_action_t action_array2[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
    { {DBAL_ENUM_FVAL_IPMF1_ACTION_DP, DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND,
       DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN},
{DBAL_ENUM_FVAL_IPMF1_ACTION_TC, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1,
 DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET},
{DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1, DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE,
 DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN},
{DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR,
 DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR}
};
/* Actions per stage add up to 32b , so the FG we create with them will be 32b(Supposed to be 64b , but 64b not yet supported) */
static bcm_field_action_t bcm_action_array2[NUM_STAGES][NUM_QUALS_AND_ACTIONS] =
{ {bcmFieldActionPrioIntNew, bcmFieldActionUsePolicerResult, bcmFieldActionMirrorIngressRaw},
{bcmFieldActionPphPresentSet, bcmFieldActionStatId0, bcmFieldActionInVportClass0},
{bcmFieldActionStatId0, bcmFieldActionPphPresentSet, bcmFieldActionTrunkHashKeySet},
{bcmFieldActionQosMapIdNew, bcmFieldActionPrioIntNew, bcmFieldActionStatId0}
};

/* Qualifier class param per stage */
static dnx_field_qual_class_e qual_class[NUM_STAGES] =
    { DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_QUAL_CLASS_META };
/* Storing DNX entry info structs for compare */
static dnx_field_entry_t dnx_entry_info_array[NUM_FGS][NUM_ENTRIES];
/* Storing BCM entry info structs for compare */
static bcm_field_entry_info_t bcm_entry_info_array[NUM_FGS][NUM_ENTRIES];
/* Storing entry handles for get and compare */
static uint32 entry_handle_array[NUM_FGS][NUM_ENTRIES];
static bcm_field_entry_t bcm_entry_handle_array[NUM_FGS][NUM_ENTRIES];
static dnx_field_group_t field_group[NUM_FGS] = { FG_ID_160B };
/* Qualifier array of qualifier values for entry_add */
static uint32 qual_val_array[NUM_STAGES][NUM_ENTRIES] = { {0x400, 6, 0x01},
{1, 10, 3},
{2, 5, 10},
{4, 10, 14}
};
/* Qualifier array of qualifier values for entry_add */
static uint32 qual_val_array2[NUM_STAGES][NUM_ENTRIES] = { {0xFAF90C0B, 0xFA090C0C, 0xFFFF},
{1, 9, 6},
{0x90D0B, 0x0B, 0xFFFF},
{5, 2, 100}
};
/* Qualifier array of action values for entry_add */
static uint32 action_val_array[NUM_STAGES][NUM_ENTRIES] = { {2, 1, 15},
{3, 0x12345, 5},
{1, 245, 0x6789},
{3, 1, 10}
};
/* Qualifier array of action values for entry_add */
static uint32 action_val_array2[NUM_STAGES][NUM_ENTRIES] = { {1, 4, 1},
{0,  0x12345, 32},
{0x6789, 1, 2543},
{6, 1, 10}
};
/* *INDENT-ON* */

static shr_error_e
field_entry_add_dnx(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *entry_handle,
    dnx_field_entry_t * entry_info,
    uint32 cur_fg_entry)
{
    int entry_itterator;
    dnx_field_stage_e field_stage;
    dnx_field_action_id_t action_id = 0;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, entry_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    entry_info->priority = ENTRY_PRIORITY * cur_fg_entry;

    for (entry_itterator = 0; entry_itterator <= cur_fg_entry; entry_itterator++)
    {
        if (fg_id == FG_ID_80B)
        {
            entry_info->key_info.qual_info[entry_itterator].dnx_qual =
                DNX_QUAL(qual_class[field_stage], field_stage, qual_array[field_stage][entry_itterator]);
            entry_info->key_info.qual_info[entry_itterator].qual_value[0] =
                qual_val_array[field_stage][entry_itterator];
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, field_stage, entry_info->key_info.qual_info[entry_itterator].dnx_qual, &qual_size));
            utilex_bitstream_set_bit_range(&entry_info->key_info.qual_info[entry_itterator].qual_mask[0], 0,
                                           qual_size - 1);

            action_id = action_array[field_stage][entry_itterator];
            entry_info->payload_info.action_info[entry_itterator].action_value[0] =
                action_val_array[field_stage][entry_itterator];
        }
        else if (fg_id == FG_ID_160B)
        {
            entry_info->key_info.qual_info[entry_itterator].dnx_qual =
                DNX_QUAL(qual_class[field_stage], field_stage, qual_array2[field_stage][entry_itterator]);
            entry_info->key_info.qual_info[entry_itterator].qual_value[0] =
                qual_val_array2[field_stage][entry_itterator];
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, field_stage, entry_info->key_info.qual_info[entry_itterator].dnx_qual, &qual_size));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (&entry_info->key_info.qual_info[entry_itterator].qual_mask[0], 0, qual_size - 1));

            action_id = action_array2[field_stage][entry_itterator];
            entry_info->payload_info.action_info[entry_itterator].action_value[0] =
                action_val_array2[field_stage][entry_itterator];
        }
        entry_info->payload_info.action_info[entry_itterator].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, action_id);

    }
    entry_info->payload_info.action_info[entry_itterator].dnx_action = DNX_FIELD_ACTION_INVALID;
    entry_info->key_info.qual_info[entry_itterator].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing entry add on entry : %d \r\n",
                __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), cur_fg_entry);
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, fg_id, entry_info, entry_handle));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_entry_add_bcm(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_entry_t * entry_handle,
    bcm_field_entry_info_t * entry_info,
    uint32 cur_fg_entry)
{
    int entry_itterator;
    bcm_field_stage_t bcm_field_stage;
    uint32 qual_size;
    dnx_field_qual_t dnx_qual;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_entry_info_t_init(entry_info);

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));

    entry_info->priority = ENTRY_PRIORITY * cur_fg_entry;
    entry_info->nof_entry_quals = 0;
    entry_info->nof_entry_actions = 0;

    for (entry_itterator = 0; entry_itterator <= cur_fg_entry; entry_itterator++)
    {
        if (fg_id == FG_ID_80B)
        {
            entry_info->entry_qual[entry_itterator].type = bcm_qual_array[dnx_field_stage][entry_itterator];
            entry_info->entry_qual[entry_itterator].value[0] = qual_val_array[dnx_field_stage][entry_itterator];
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_field_stage,
                                                          bcm_qual_array[dnx_field_stage][entry_itterator], &dnx_qual));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, dnx_field_stage, dnx_qual, &qual_size));
            utilex_bitstream_set_bit_range(&entry_info->entry_qual[entry_itterator].mask[0], 0, qual_size - 1);

            entry_info->entry_action[entry_itterator].type = bcm_action_array[dnx_field_stage][entry_itterator];
            entry_info->entry_action[entry_itterator].value[0] = action_val_array[dnx_field_stage][entry_itterator];
        }
        else if (fg_id == FG_ID_160B)
        {
            entry_info->entry_qual[entry_itterator].type = bcm_qual_array2[dnx_field_stage][entry_itterator];
            entry_info->entry_qual[entry_itterator].value[0] = qual_val_array2[dnx_field_stage][entry_itterator];
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_field_stage,
                                                          bcm_qual_array2[dnx_field_stage][entry_itterator],
                                                          &dnx_qual));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, dnx_field_stage, dnx_qual, &qual_size));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (&entry_info->entry_qual[entry_itterator].mask[0], 0, qual_size - 1));

            entry_info->entry_action[entry_itterator].type = bcm_action_array2[dnx_field_stage][entry_itterator];
            entry_info->entry_action[entry_itterator].value[0] = action_val_array2[dnx_field_stage][entry_itterator];
        }
        entry_info->nof_entry_quals++;
        entry_info->nof_entry_actions++;
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing entry add on entry : %d \r\n",
                __FUNCTION__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), cur_fg_entry);
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, entry_info, entry_handle));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
entry_compare_dnx(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * set_entry,
    uint32 set_entry_handle,
    int cur_fg_entry)
{
    dnx_field_entry_t get_entry;
    int entry_itterator;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &get_entry));
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, fg_id, set_entry_handle, &get_entry));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    /*
     * if(set_entry->priority != get_entry.priority) { SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry priority (%d) is not the
     * same as Get_entry priority(%d)\r\n", set_entry->priority, get_entry.priority); } 
     */
    LOG_INFO_EX(BSL_LOG_MODULE, "PRIORITY SET_VAL: %x  ---- PRIORITY GET_VAL: %x\n %s%s", set_entry->priority,
                get_entry.priority, EMPTY, EMPTY);
    for (entry_itterator = 0; entry_itterator <= cur_fg_entry; entry_itterator++)
    {

        if (set_entry->key_info.qual_info[entry_itterator].dnx_qual !=
            get_entry.key_info.qual_info[entry_itterator].dnx_qual)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_type (%d) is not the same as Get_entry qual_type(%d)\r\n",
                         set_entry->key_info.qual_info[entry_itterator].dnx_qual,
                         get_entry.key_info.qual_info[entry_itterator].dnx_qual);
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "QUAL SET_VAL: %x  ---- QUAL GET_VAL: %x\n %s%s",
                    set_entry->key_info.qual_info[entry_itterator].qual_value[0],
                    get_entry.key_info.qual_info[entry_itterator].qual_value[0], EMPTY, EMPTY);
        LOG_INFO_EX(BSL_LOG_MODULE, "QUAL MASK SET_VAL: %x  ---- QUAL MASK GET_VAL: %x\n %s%s",
                    set_entry->key_info.qual_info[entry_itterator].qual_mask[0],
                    get_entry.key_info.qual_info[entry_itterator].qual_mask[0], EMPTY, EMPTY);
        LOG_INFO_EX(BSL_LOG_MODULE, "ACTION SET_VAL: %x  ---- ACTION GET_VAL: %x\n %s%s",
                    set_entry->payload_info.action_info[entry_itterator].action_value[0],
                    get_entry.payload_info.action_info[entry_itterator].action_value[0], EMPTY, EMPTY);

        if (set_entry->key_info.qual_info[entry_itterator].qual_value[0] !=
            get_entry.key_info.qual_info[entry_itterator].qual_value[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_value (%d) is not the same as Get_entry qual_value(%d)\r\n",
                         set_entry->key_info.qual_info[entry_itterator].qual_value[0],
                         get_entry.key_info.qual_info[entry_itterator].qual_value[0]);
        }

        if (set_entry->key_info.qual_info[entry_itterator].qual_mask[0] !=
            get_entry.key_info.qual_info[entry_itterator].qual_mask[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_mask (%d) is not the same as Get_entry qual_mask(%d)\r\n",
                         set_entry->key_info.qual_info[entry_itterator].qual_mask[0],
                         get_entry.key_info.qual_info[entry_itterator].qual_mask[0]);
        }

        if (set_entry->payload_info.action_info[entry_itterator].dnx_action !=
            get_entry.payload_info.action_info[entry_itterator].dnx_action)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry dnx_action (%d) is not the same as Get_entry dnx_action(%d)\r\n",
                         set_entry->payload_info.action_info[entry_itterator].dnx_action,
                         get_entry.payload_info.action_info[entry_itterator].dnx_action);
        }
        if (set_entry->payload_info.action_info[entry_itterator].action_value[0] !=
            get_entry.payload_info.action_info[entry_itterator].action_value[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry action_value (%d) is not the same as Get_entry action_value(%d)\r\n",
                         set_entry->payload_info.action_info[entry_itterator].action_value[0],
                         get_entry.payload_info.action_info[entry_itterator].action_value[0]);
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Entry compare finished successfully for entry : %d \r\n",
                    __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), cur_fg_entry);

    }
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
entry_compare_bcm(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_entry_info_t * set_entry,
    bcm_field_entry_t set_entry_handle,
    int cur_fg_entry)
{
    bcm_field_entry_info_t get_entry;
    int entry_itterator;
    dnx_field_stage_e dnx_field_stage;
    bcm_field_stage_t bcm_field_stage;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_entry_info_t_init(&get_entry);
    SHR_IF_ERR_EXIT(bcm_field_entry_info_get(unit, fg_id, set_entry_handle, &get_entry));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));
    /** Convert DNX to BCM Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));

    if (set_entry->priority != get_entry.priority)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry priority (%d) is not the same as Get_entry priority(%d)\r\n",
                     set_entry->priority, get_entry.priority);
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "PRIORITY SET_VAL: %x  ---- PRIORITY GET_VAL: %x\n %s%s", set_entry->priority,
                get_entry.priority, EMPTY, EMPTY);

    for (entry_itterator = 0; entry_itterator <= cur_fg_entry; entry_itterator++)
    {

        if (set_entry->entry_qual[entry_itterator].type != get_entry.entry_qual[entry_itterator].type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_type (%s) is not the same as Get_entry qual_type(%s)\r\n",
                         dnx_field_bcm_qual_text(unit, set_entry->entry_qual[entry_itterator].type),
                         dnx_field_bcm_qual_text(unit, get_entry.entry_qual[entry_itterator].type));
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "QUAL SET_VAL: %x  ---- QUAL GET_VAL: %x\n %s%s",
                    set_entry->entry_qual[entry_itterator].value[0],
                    get_entry.entry_qual[entry_itterator].value[0], EMPTY, EMPTY);
        LOG_INFO_EX(BSL_LOG_MODULE, "QUAL MASK SET_VAL: %x  ---- QUAL MASK GET_VAL: %x\n %s%s",
                    set_entry->entry_qual[entry_itterator].mask[0],
                    get_entry.entry_qual[entry_itterator].mask[0], EMPTY, EMPTY);
        LOG_INFO_EX(BSL_LOG_MODULE, "ACTION SET_VAL: %x  ---- ACTION GET_VAL: %x\n %s%s",
                    set_entry->entry_action[entry_itterator].value[0],
                    get_entry.entry_action[entry_itterator].value[0], EMPTY, EMPTY);

        if (set_entry->entry_qual[entry_itterator].value[0] != get_entry.entry_qual[entry_itterator].value[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_value (%d) is not the same as Get_entry qual_value(%d)\r\n",
                         set_entry->entry_qual[entry_itterator].value[0],
                         get_entry.entry_qual[entry_itterator].value[0]);
        }

        if (set_entry->entry_qual[entry_itterator].mask[0] != get_entry.entry_qual[entry_itterator].mask[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry qual_mask (%d) is not the same as Get_entry qual_mask(%d)\r\n",
                         set_entry->entry_qual[entry_itterator].mask[0], get_entry.entry_qual[entry_itterator].mask[0]);
        }

        if (set_entry->entry_action[entry_itterator].type != get_entry.entry_action[entry_itterator].type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry bcm_action (%s) is not the same as Get_entry bcm_action(%s)\r\n",
                         dnx_field_bcm_action_text(unit, set_entry->entry_action[entry_itterator].type),
                         dnx_field_bcm_action_text(unit, get_entry.entry_action[entry_itterator].type));
        }
        if (set_entry->entry_action[entry_itterator].value[0] != get_entry.entry_action[entry_itterator].value[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Set_entry action_value (%d) is not the same as Get_entry action_value(%d)\r\n",
                         set_entry->entry_action[entry_itterator].value[0],
                         get_entry.entry_action[entry_itterator].value[0]);
        }

        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Entry compare finished successfully for entry : %d \r\n",
                    __FUNCTION__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), cur_fg_entry);

    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   
 *   The function sets-up two field groups(80b and 160b).
 *   The field groups are attached to the same context.
 *   We create 3 entries in each group and performs semantic operations
 *   
 *
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_field_entry_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e dnx_field_stage;
    bcm_field_stage_t bcm_field_stage;
    int field_entry_test_type_name;
    int fg_index, cur_fg_entry;
    int assignment_index;
    dnx_field_action_id_t action_id = 0;
    dnx_field_group_info_t dnx_fg_info;
    bcm_field_group_info_t bcm_fg_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM("type", field_entry_test_type_name);
    SH_SAND_GET_ENUM("stage", dnx_field_stage);

    if (field_entry_test_type_name == 0)
    {
        for (fg_index = 0; fg_index < NUM_FGS; fg_index++)
        {
            bcm_field_group_info_t_init(&bcm_fg_info);

            /** Convert DNX to BCM Field Stage */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));

            bcm_fg_info.stage = bcm_field_stage;
            bcm_fg_info.fg_type = bcmFieldGroupTypeTcam;
            bcm_fg_info.nof_actions = NUM_QUALS_AND_ACTIONS;
            bcm_fg_info.nof_quals = NUM_QUALS_AND_ACTIONS;
            for (assignment_index = 0; assignment_index < NUM_QUALS_AND_ACTIONS; assignment_index++)
            {
                if (field_group[fg_index] == FG_ID_80B)
                {
                    bcm_fg_info.qual_types[assignment_index] = bcm_qual_array[dnx_field_stage][assignment_index];
                    bcm_fg_info.action_types[assignment_index] = bcm_action_array[dnx_field_stage][assignment_index];
                }
                else if (field_group[fg_index] == FG_ID_160B)
                {
                    bcm_fg_info.qual_types[assignment_index] = bcm_qual_array2[dnx_field_stage][assignment_index];
                    bcm_fg_info.action_types[assignment_index] = bcm_action_array2[dnx_field_stage][assignment_index];
                }
            }

            SHR_IF_ERR_EXIT(bcm_field_group_add
                            (unit, (BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_MSB_RESULT_ALIGN), &bcm_fg_info,
                             (bcm_field_group_t *) & (field_group[fg_index])));

            for (cur_fg_entry = 0; cur_fg_entry < NUM_ENTRIES; cur_fg_entry++)
            {
                SHR_IF_ERR_EXIT(field_entry_add_bcm
                                (unit, (bcm_field_group_t) (field_group[fg_index]),
                                 &bcm_entry_handle_array[fg_index][cur_fg_entry],
                                 &bcm_entry_info_array[fg_index][cur_fg_entry], cur_fg_entry));
                SHR_IF_ERR_EXIT(entry_compare_bcm
                                (unit, (bcm_field_group_t) (field_group[fg_index]),
                                 &bcm_entry_info_array[fg_index][cur_fg_entry],
                                 bcm_entry_handle_array[fg_index][cur_fg_entry], cur_fg_entry));
            }
        }
    }
    else if (field_entry_test_type_name == 1)
    {
        for (fg_index = 0; fg_index < NUM_FGS; fg_index++)
        {
            dnx_field_group_info_t_init(unit, &dnx_fg_info);

            dnx_fg_info.field_stage = dnx_field_stage;
            dnx_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
            for (assignment_index = 0; assignment_index < NUM_QUALS_AND_ACTIONS; assignment_index++)
            {
                if (field_group[fg_index] == FG_ID_80B)
                {
                    dnx_fg_info.dnx_quals[assignment_index] =
                        DNX_QUAL(qual_class[dnx_field_stage], dnx_field_stage,
                                 qual_array[dnx_field_stage][assignment_index]);
                    action_id = action_array[dnx_field_stage][assignment_index];
                }
                else if (field_group[fg_index] == FG_ID_160B)
                {
                    dnx_fg_info.dnx_quals[assignment_index] =
                        DNX_QUAL(qual_class[dnx_field_stage], dnx_field_stage,
                                 qual_array2[dnx_field_stage][assignment_index]);
                    action_id = action_array2[dnx_field_stage][assignment_index];
                }
                dnx_fg_info.dnx_actions[assignment_index] =
                    DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_field_stage, action_id);
            }

            SHR_IF_ERR_EXIT(dnx_field_group_add
                            (unit, DNX_FIELD_GROUP_ADD_FLAG_WITH_ID, &dnx_fg_info, &field_group[fg_index]));
            for (cur_fg_entry = 0; cur_fg_entry < NUM_ENTRIES; cur_fg_entry++)
            {
                SHR_IF_ERR_EXIT(field_entry_add_dnx
                                (unit, field_group[fg_index], &entry_handle_array[fg_index][cur_fg_entry],
                                 &dnx_entry_info_array[fg_index][cur_fg_entry], cur_fg_entry));
                SHR_IF_ERR_EXIT(entry_compare_dnx
                                (unit, field_group[fg_index], &dnx_entry_info_array[fg_index][cur_fg_entry],
                                 entry_handle_array[fg_index][cur_fg_entry], cur_fg_entry));
            }

        }
    }
exit:
    {
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - run TCAM Look-up sequence in diag shell
 */
shr_error_e
sh_dnx_field_entry_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_field_entry_semantic(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }
exit:
    SHR_FUNC_EXIT;
}
