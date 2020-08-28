/*! \file port_sit.c
 * $Id$
 *
 * sit profile management  for DNX.
 * Allows to alloc/free sit profile for PP port,
 * configure the tag swap parameter for PP port.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/pbmp.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dbal/dbal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/port/port_pp.h>

/*
 * }
 */

#define DNX_TAG_SWAP_DOT1BR_TAG_OFFSET                    (12)
#define DNX_TAG_SWAP_COE_TAG_OFFSET                       (12)

/**
 * \brief
 *   allocate SIT profile .
 */
shr_error_e
dnx_port_sit_profile_alloc(
    int unit,
    dnx_sit_profile_t * sit_profile_info,
    uint8 *sit_profile,
    uint8 *is_first_sit_profile_reference)
{
    int value;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sit_profile_info, _SHR_E_PARAM, "sit_profile_info");
    SHR_NULL_CHECK(is_first_sit_profile_reference, _SHR_E_PARAM, "is_first_sit_profile_reference");
    SHR_NULL_CHECK(sit_profile, _SHR_E_PARAM, "sit_profile");

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.allocate_single
                    (unit, 0, sit_profile_info, NULL, &value, is_first_sit_profile_reference));

    *sit_profile = (uint8) value;
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   frree SIT profile .
 */
shr_error_e
dnx_port_sit_profile_free(
    int unit,
    int sit_profile,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_last, _SHR_E_PARAM, "is_last");

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.free_single(unit, sit_profile, is_last));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get SIT profile for SIT type according to para.
 */
shr_error_e
dnx_port_sit_get_sit_profile(
    int unit,
    int type,
    void *para,
    uint8 *sit_profile)
{
    dnx_sit_profile_t sit_profile_info, sit_profile_info_get;
    int i, ii, arg;
    uint8 is_first;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sit_profile, _SHR_E_PARAM, "sit_profile");
    SHR_NULL_CHECK(para, _SHR_E_PARAM, "para");

    switch (type)
    {
        case DNX_SIT_TYPE_DOT1BR:
        {
            arg = *((int *) para);
            /** default profile for BR is static and other profile is allocated dynamically */
            *sit_profile = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE;

            /** create new profile if non-default and no exist */
            if (arg > DNX_SIT_PCP_DEI_SRC_AC_OUTER_TAG)
            {
                sal_memset(&sit_profile_info, 0, sizeof(dnx_sit_profile_t));
                sal_memset(&sit_profile_info_get, 0, sizeof(dnx_sit_profile_t));
                sit_profile_info.tag_type = DBAL_ENUM_FVAL_SIT_TAG_TYPE_802_1BR;
                sit_profile_info.tpid = DBAL_DEFINE_TPID_DOT1BR;
                /** VID_SRC fixed */
                sit_profile_info.vid_src = DBAL_ENUM_FVAL_SIT_VID_SRC_AC_THIRD_VID;
                *sit_profile = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE;
                if (arg == DNX_SIT_PCP_DEI_SRC_AC_INNER_TAG)
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_INNER_PCP_DEI;
                }
                else if (arg == DNX_SIT_PCP_DEI_SRC_AC_THIRD_TAG)
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_THIRD_TAG_PCP_DEI;
                }
                else if (arg == DNX_SIT_PCP_DEI_SRC_PORT_VALUE)
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_PORT;
                }
                else if (arg == DNX_SIT_PCP_DEI_SRC_NWK_QOS_INNER_TAG)
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_NETWORK_QOS_INNER_TAG;
                }
                else if (arg == DNX_SIT_PCP_DEI_SRC_NWK_QOS_OUTER_TAG)
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_NETWORK_QOS_OUTER_TAG;
                }
                else
                {
                    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_OUTER_PCP_DEI;
                }
                /** if profile is already allocated */
                for (i = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE; i <= DNX_PORT_MAX_SIT_PROFILE; i++)
                {
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_get(unit, i, &sit_profile_info_get));
                    if (!sal_memcmp(&sit_profile_info, &sit_profile_info_get, sizeof(dnx_sit_profile_t)))
                    {
                        *sit_profile = i;
                        break;
                    }
                }
                /** allocate a new profile */
                if (i > DNX_PORT_MAX_SIT_PROFILE)
                {
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_alloc(unit, &sit_profile_info, sit_profile, &is_first));
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_set(unit, *sit_profile, &sit_profile_info));
                }
            }
            break;
        }

        case DNX_SIT_TYPE_DOT1Q:
        case DNX_SIT_TYPE_DOT1T:
        {
            /*
             * Dynamically get sit profile according to input para:
             *
             *  para - non-zero: get a sit profile  
             *      1. first check if profile exist, return profile ID if exist
             *      2. Else, get a free sit profile for it.
             *
             *  para - zero: check existing sit profile  
             */
            sal_memset(&sit_profile_info, 0, sizeof(dnx_sit_profile_t));
            sal_memset(&sit_profile_info_get, 0, sizeof(dnx_sit_profile_t));
            sit_profile_info.tag_type = DBAL_ENUM_FVAL_SIT_TAG_TYPE_802_1Q;
            sit_profile_info.tpid = 0x8100;
            sit_profile_info.vid_src = DBAL_ENUM_FVAL_SIT_VID_SRC_PORT_DEFAULT;

            for (ii = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE; ii <= DNX_PORT_MAX_SIT_PROFILE; ii++)
            {
                SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_get(unit, ii, &sit_profile_info_get));
                if (!sal_memcmp(&sit_profile_info, &sit_profile_info_get, sizeof(dnx_sit_profile_t)))
                {
                    *sit_profile = ii;
                    break;
                }
            }

            arg = *((int *) para);
            if (arg)
            {
                /** enable, allocate a new profile */
                if (ii > DNX_PORT_MAX_SIT_PROFILE)
                {
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_alloc(unit, &sit_profile_info, sit_profile, &is_first));
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_set(unit, *sit_profile, &sit_profile_info));
                }
            }
            else
            {
                if (ii > DNX_PORT_MAX_SIT_PROFILE)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Can't find SIT Profile for it!\n");
                }
            }

            break;
        }

        case DNX_SIT_TYPE_PON:
        {
            /*
             * Dynamically get sit profile according to input para:
             *
             *  para - non-zero: get a sit profile  
             *      1. first check if profile exist, return profile ID if exist
             *      2. Else, get a free sit profile for it.
             *
             *  para - zero: check existing sit profile  
             */
            sal_memset(&sit_profile_info, 0, sizeof(dnx_sit_profile_t));
            sal_memset(&sit_profile_info_get, 0, sizeof(dnx_sit_profile_t));
            sit_profile_info.tag_type = DBAL_ENUM_FVAL_SIT_TAG_TYPE_802_1Q;
            sit_profile_info.tpid = 0x8100;
            sit_profile_info.vid_src = DBAL_ENUM_FVAL_SIT_VID_SRC_AC_THIRD_VID;

            for (ii = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE; ii <= DNX_PORT_MAX_SIT_PROFILE; ii++)
            {
                SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_get(unit, ii, &sit_profile_info_get));
                if (!sal_memcmp(&sit_profile_info, &sit_profile_info_get, sizeof(dnx_sit_profile_t)))
                {
                    *sit_profile = ii;
                    break;
                }
            }

            arg = *((int *) para);
            if (arg)
            {
                /** enable, allocate a new profile */
                if (ii > DNX_PORT_MAX_SIT_PROFILE)
                {
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_alloc(unit, &sit_profile_info, sit_profile, &is_first));
                    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_set(unit, *sit_profile, &sit_profile_info));
                }
            }
            else
            {
                if (ii > DNX_PORT_MAX_SIT_PROFILE)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Can't find SIT Profile for it!\n");
                }
            }

            break;
        }

        default:
        {

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_init(
    int unit)
{

    uint32 entry_handle_id;
    dnx_sit_profile_t sit_profile_info;
    uint8 is_first;
    int sit_profile = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initiate the tag swap table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));

    /*
     * tpid0: used for recognizing and swap of 802.1BR service
     */

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID, DNX_TAG_SWAP_TPID_INDEX_DOT1BR,
                                 DBAL_DEFINE_TPID_DOT1BR);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_OFFSET, DNX_TAG_SWAP_TPID_INDEX_DOT1BR,
                                 DNX_TAG_SWAP_DOT1BR_TAG_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_SIZE, DNX_TAG_SWAP_TPID_INDEX_DOT1BR,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_SIZE_8_BYTES);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_DOT1BR,
                                 0xFFFF);

    /*
     * tpid1: used for recognizing and swap of CoE(PON) service
     */

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID, DNX_TAG_SWAP_TPID_INDEX_COE,
                                 DNX_TPID_VALUE_COE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_OFFSET, DNX_TAG_SWAP_TPID_INDEX_COE,
                                 DNX_TAG_SWAP_COE_TAG_OFFSET);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_SIZE, DNX_TAG_SWAP_TPID_INDEX_COE,
                                 DBAL_ENUM_FVAL_SIT_TAG_SWAP_SIZE_4_BYTES);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_COE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Sit profile 0 is used for default(no sit)
     */
    sal_memset(&sit_profile_info, 0, sizeof(dnx_sit_profile_t));
    sit_profile_info.tag_type = DBAL_ENUM_FVAL_SIT_TAG_TYPE_NO_SIT;
    sit_profile = DNX_PORT_DEFAULT_SIT_PROFILE;

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.allocate_single
                    (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &sit_profile_info, NULL, &sit_profile, &is_first));
    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_set(unit, sit_profile, &sit_profile_info));

    /*
     * Sit profile 1 is used for 802.1BR default
     */
    sal_memset(&sit_profile_info, 0, sizeof(dnx_sit_profile_t));
    sit_profile_info.tag_type = DBAL_ENUM_FVAL_SIT_TAG_TYPE_802_1BR;
    sit_profile_info.tpid = DBAL_DEFINE_TPID_DOT1BR;
    sit_profile_info.pcp_dei_src = DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_OUTER_PCP_DEI;
    sit_profile_info.vid_src = DBAL_ENUM_FVAL_SIT_VID_SRC_AC_THIRD_VID;
    sit_profile = DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE;

    SHR_IF_ERR_EXIT(algo_port_pp_db.egress_pp_port_sit_profile.allocate_single
                    (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &sit_profile_info, NULL, &sit_profile, &is_first));
    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_set(unit, sit_profile, &sit_profile_info));

exit:
    DBAL_FUNC_FREE_VARS;

    SHR_FUNC_EXIT;

}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_tag_swap_set(
    int unit,
    bcm_gport_t port,
    int enable,
    dbal_enum_value_field_sit_tag_swap_mode_e tpid_sel)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;
    bcm_port_t local_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** Set tag swap parameter to HW. Iterate on all the tm ports available on the gport */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, (bcm_core_t *) & tm_core_id, &port_tm));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);

        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_ENABLE, INST_SINGLE, enable);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_OP_MODE, INST_SINGLE, tpid_sel);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;

    SHR_FUNC_EXIT;

}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_tag_swap_get(
    int unit,
    bcm_gport_t port,
    int *enable,
    dbal_enum_value_field_sit_tag_swap_mode_e * tpid_sel)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tm_core_id, port_tm;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    /** Get tag swap parameter from HW. Use first local port to get tm port */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, (bcm_core_t *) & tm_core_id, &port_tm));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, tm_core_id);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PTC, port_tm);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_ENABLE, INST_SINGLE, (uint32 *) enable);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TAG_SWAP_OP_MODE, INST_SINGLE, (uint32 *) tpid_sel);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;

    SHR_FUNC_EXIT;

}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_tag_swap_tpid_set(
    int unit,
    int tpid_index,
    uint16 tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set tag swap parameter to HW. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID, tpid_index, tpid);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;

    SHR_FUNC_EXIT;

}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_tag_swap_tpid_get(
    int unit,
    int tpid_index,
    uint16 *tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get tag swap parameter from HW. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID, tpid_index, tpid);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;

    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_profile_hw_set(
    int unit,
    uint8 sit_profile,
    dnx_sit_profile_t * sit_profile_info)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_PROFILE, sit_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIT_TAG_TYPE, INST_SINGLE,
                                 sit_profile_info->tag_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIT_TPID, INST_SINGLE, sit_profile_info->tpid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIT_VID_SRC, INST_SINGLE, sit_profile_info->vid_src);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIT_PCP_DEI_SRC, INST_SINGLE,
                                 sit_profile_info->pcp_dei_src);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * See prototype definition in port_sit.h for function description
 */

shr_error_e
dnx_port_sit_profile_hw_get(
    int unit,
    uint8 sit_profile,
    dnx_sit_profile_t * sit_profile_info)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write default values to DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_PROFILE, sit_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIT_TAG_TYPE, INST_SINGLE, &sit_profile_info->tag_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                    (unit, entry_handle_id, DBAL_FIELD_SIT_TPID, INST_SINGLE, &sit_profile_info->tpid));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIT_VID_SRC, INST_SINGLE, &sit_profile_info->vid_src));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SIT_PCP_DEI_SRC, INST_SINGLE, &sit_profile_info->pcp_dei_src));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_profile_hw_clear(
    int unit,
    uint8 sit_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear the values written in DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_EGRESS_PROFILE_TABLE, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_PROFILE, sit_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_egress_port_set(
    int unit,
    bcm_gport_t port,
    uint8 *sit_profile,
    uint8 *sit_pcp,
    uint8 *sit_dei,
    uint16 *sit_vid)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!sit_profile && !sit_pcp && !sit_dei && !sit_vid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid configuration: at least 1 field should be set");
    }

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write default values to DBAL_TABLE_EGRESS_PP_PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);

        if (sit_profile)
        {
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_PROFILE, INST_SINGLE, *sit_profile);
        }
        if (sit_pcp)
        {
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_PCP, INST_SINGLE, *sit_pcp);
        }
        if (sit_dei)
        {
            dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_SIT_DEI, INST_SINGLE, *sit_dei);
        }
        if (sit_vid)
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SIT_VID, INST_SINGLE, *sit_vid);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_egress_port_get(
    int unit,
    bcm_gport_t port,
    uint8 *sit_profile,
    uint8 *sit_pcp,
    uint8 *sit_dei,
    uint16 *sit_vid)
{

    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!sit_profile && !sit_pcp && !sit_dei && !sit_vid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid configuration: at least 1 field should be retreived");
    }

    /** Get pp-port and core-id first. */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Write default values to DBAL_TABLE_EGRESS_PP_PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);

    if (sit_profile)
    {
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SIT_PROFILE, INST_SINGLE, sit_profile);
    }
    if (sit_pcp)
    {
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SIT_PCP, INST_SINGLE, sit_pcp);
    }
    if (sit_dei)
    {
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_SIT_DEI, INST_SINGLE, sit_dei);
    }
    if (sit_vid)
    {
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_SIT_VID, INST_SINGLE, sit_vid);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_port_sit_profile_is_used(
    int unit,
    uint8 sit_profile,
    uint8 *is_used)
{
    uint8 sit_profile_tmp;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_used, _SHR_E_PARAM, "is_used");

    *is_used = 0;

    /** traverse all port to see if this profile is used */
    {
        int port_min, port_max;
        pbmp_t *pbmp_p;

        SHR_IF_ERR_EXIT(dnx_drv_soc_port_min(unit, ETHER_PTYPE, &port_min));
        SHR_IF_ERR_EXIT(dnx_drv_soc_port_max(unit, ETHER_PTYPE, &port_max));
        /*
         * Get pointer to bitmap corresponding to ETHERNET ports.
         */
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_E_ALL_BITMAP, &pbmp_p));
        for (port = port_min; port >= 0 && port <= port_max; port++)
        {
            if (_SHR_PBMP_MEMBER(*pbmp_p, port))
            {
                SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_get(unit, port, &sit_profile_tmp, NULL, NULL, NULL));
                if (sit_profile_tmp == sit_profile)
                {
                    *is_used = 1;
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_sit_tag_pcp_dei_src_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    dbal_enum_value_field_sit_tag_swap_mode_e tpid_sel = 0;
    uint8 sit_profile = 0;
    dnx_sit_profile_t sit_profile_info;
    int swap_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_get(unit, port, &swap_enable, &tpid_sel));
    if (!swap_enable)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't get PCP_DEI source as port isn't swap enable!\n");
    }

    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_get(unit, port, &sit_profile, NULL, NULL, NULL));
    *value = (swap_enable && (sit_profile != DNX_PORT_DEFAULT_SIT_PROFILE));
    if (*value)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_get(unit, sit_profile, &sit_profile_info));
        if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_OUTER_PCP_DEI)
        {
            *value = DNX_SIT_PCP_DEI_SRC_AC_OUTER_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_INNER_PCP_DEI)
        {
            *value = DNX_SIT_PCP_DEI_SRC_AC_INNER_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_AC_THIRD_TAG_PCP_DEI)
        {
            *value = DNX_SIT_PCP_DEI_SRC_AC_THIRD_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_PORT)
        {
            *value = DNX_SIT_PCP_DEI_SRC_PORT_VALUE;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_OUTER_TAG)
        {
            *value = DNX_SIT_PCP_DEI_SRC_OUTER_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_INNER_TAG)
        {
            *value = DNX_SIT_PCP_DEI_SRC_INNER_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_NETWORK_QOS_OUTER_TAG)
        {
            *value = DNX_SIT_PCP_DEI_SRC_NWK_QOS_OUTER_TAG;
        }
        else if (sit_profile_info.pcp_dei_src == DBAL_ENUM_FVAL_SIT_PCP_DEI_SRC_NETWORK_QOS_INNER_TAG)
        {
            *value = DNX_SIT_PCP_DEI_SRC_NWK_QOS_INNER_TAG;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_sit_tag_pcp_dei_src_set(
    int unit,
    bcm_port_t port,
    int value)
{
    uint8 sit_profile, sit_profile_old;
    uint8 is_sit_profile_used;
    uint8 is_first;
    dnx_sit_profile_t sit_profile_info;
    dbal_enum_value_field_sit_tag_swap_mode_e tpid_sel = 0;
    int swap_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_get(unit, port, &swap_enable, &tpid_sel));
    if (!swap_enable)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Can't set PCP_DEI source as port isn't swap enable!\n");
    }

    /*
     * default profile for BR is static and other profile is allocated dynamically 
     * arg: support with following 
     *  0 - enable sit with PCP/DEI from outer AC
     *  1 - enable sit with PCP/DEI from inner AC
     *  2 - enable sit with PCP/DEI from third AC
     *  3 - enable sit with PCP/DEI from port default value
     */
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_get(unit, port, &sit_profile_old, NULL, NULL, NULL));
    SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_get(unit, sit_profile_old, &sit_profile_info));

    if (sit_profile_info.tag_type == DBAL_ENUM_FVAL_SIT_TAG_TYPE_802_1BR)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_get_sit_profile(unit, DNX_SIT_TYPE_DOT1BR, &value, &sit_profile));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port can't support to set PCP/DEI source for this type of TAG!\n");
    }

    /** set profile info into hardware */
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_set(unit, port, &sit_profile, NULL, NULL, NULL));

    /** free SIT profile if non-used, keep the default profile */
    if ((sit_profile_old != DNX_PORT_DEFAULT_SIT_PROFILE) && (sit_profile_old != DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE))
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_profile_is_used(unit, sit_profile_old, &is_sit_profile_used));
        if (!is_sit_profile_used)
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_free(unit, sit_profile_old, &is_first));
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_clear(unit, sit_profile_old));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_sit_prepend_tag_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    dbal_enum_value_field_sit_tag_swap_mode_e tpid_sel = 0;
    uint8 sit_profile = 0;
    int swap_enable = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_get(unit, port, &swap_enable, &tpid_sel));
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_get(unit, port, &sit_profile, NULL, NULL, NULL));
    *value = (swap_enable && (sit_profile != DNX_PORT_DEFAULT_SIT_PROFILE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_sit_prepend_tag_set(
    int unit,
    bcm_port_t port,
    int value)
{
    uint8 sit_profile, sit_profile_old;
    uint8 is_sit_profile_used;
    uint8 is_first;

    SHR_FUNC_INIT_VARS(unit);

    sit_profile = value ? DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE : 0;
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_get(unit, port, &sit_profile_old, NULL, NULL, NULL));
    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_set(unit, port, value, DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID0_TRIGGER));
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_set(unit, port, &sit_profile, NULL, NULL, NULL));

    /** free SIT profile if non-used, keep the default profile */
    if (!value)
    {
        if ((sit_profile_old != DNX_PORT_DEFAULT_SIT_PROFILE)
            && (sit_profile_old != DNX_PORT_DEFAULT_DOT1BR_SIT_PROFILE))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_is_used(unit, sit_profile_old, &is_sit_profile_used));
            if (!is_sit_profile_used)
            {
                SHR_IF_ERR_EXIT(dnx_port_sit_profile_free(unit, sit_profile_old, &is_first));
                SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_clear(unit, sit_profile_old));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_port_flag_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_app_flags_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_port_flag_get(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flag");

    SHR_IF_ERR_EXIT(dnx_algo_port_app_flags_get(unit, port, flags));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Traverse all ports if other ports is set expect input port.
 * If the input port is invalid, then check all port.
 */
static shr_error_e
dnx_port_sit_port_status_get(
    int unit,
    bcm_port_t port,
    int port_info,
    uint8 *is_other_set)
{
    int port_i;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_other_set, _SHR_E_PARAM, "is_other_set");

    *is_other_set = 0;

    {
        int port_i_min, port_i_max;
        pbmp_t *pbmp_p;

        SHR_IF_ERR_EXIT(dnx_drv_soc_port_min(unit, ETHER_PTYPE, &port_i_min));
        SHR_IF_ERR_EXIT(dnx_drv_soc_port_max(unit, ETHER_PTYPE, &port_i_max));
        /*
         * Get pointer to bitmap corresponding to ETHERNET ports.
         */
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_E_ALL_BITMAP, &pbmp_p));
        for (port_i = port_i_min; port_i >= 0 && port_i <= port_i_max; port_i++)
        {
            if (_SHR_PBMP_MEMBER(*pbmp_p, port_i))
            {
                if (port_i != port)
                {
                    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port_i, &flags));
                    if (port_info == DNX_PORT_TYPE_COE)
                    {
                        if (DNX_PORT_IS_COE_PORT(flags))
                        {
                            *is_other_set = 1;
                            break;
                        }
                    }
                    else if (port_info == DNX_PORT_TYPE_PON)
                    {
                        if (DNX_PORT_IS_PON_PORT(flags))
                        {
                            *is_other_set = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_sit_coe_set_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    uint32 flags;
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (!is_phy_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local port is needed to set COE!\n");
    }

    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port, &flags));
    if (value)
    {
        if (DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "This port is already COE type for enable opeartion!\n");
        }
    }
    else
    {
        if (!DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "This port is not COE type for disable opeartion!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_virtual_port_tcam_sw_set(
    int unit,
    uint32 access_id,
    uint8 is_use,
    uint16 priority,
    uint16 pp_port,
    uint16 sys_port,
    uint32 key)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, access_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_USE, INST_SINGLE, is_use);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, sys_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_KBR_INFO_HALF, INST_SINGLE, key);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get virtural port TCAM management sw information according to access id.
 *
 * \param [in] unit - unit ID
 * \param [in] access_id - access id for TCAM entry
 * \param [out] is_use  - if use of TCAM entry
 * \param [out] priority - priority of TCAM entry
 * \param [out] pp_port - Result.pp_port of TCAM entry
 * \param [out] sys_port - Result.sys_port of TCAM entry
 * \param [out] key - Key of TCAM entry
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_sit_virtual_port_tcam_sw_get(
    int unit,
    uint32 access_id,
    uint8 *is_use,
    uint16 *priority,
    uint16 *pp_port,
    uint16 *sys_port,
    uint32 *key)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW, &entry_handle_id));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INDEX, access_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (is_use)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_IS_USE, INST_SINGLE, is_use));
    }
    if (priority)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority));
    }
    if (pp_port)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port));

    }
    if (sys_port)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field16_get
                        (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, sys_port));
    }
    if (key)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PRT_KBR_INFO_HALF, INST_SINGLE, key));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_virtual_port_access_id_get_by_port(
    int unit,
    bcm_port_t port,
    uint32 *entry_access_id)
{
    bcm_core_t core;
    uint32 pp_port;
    uint16 pp_port_get;
    int max_capacity, idx;
    uint8 is_use;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &core, &pp_port));
    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

    for (idx = 0; idx < max_capacity; idx++)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_get(unit, idx, &is_use, NULL, &pp_port_get, NULL, NULL));
        if (is_use && (pp_port_get == (uint16) pp_port))
        {
            *entry_access_id = idx;
            break;
        }
    }

    if (idx >= max_capacity)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Can't find access_id for this port!\n");
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * See port_sit.h
 */
shr_error_e
dnx_port_sit_virtual_port_access_id_alloc(
    int unit,
    uint32 key,
    uint32 mask,
    uint32 *entry_access_id)
{
    int max_capacity;
    int idx;

    uint32 key_get;
    uint8 is_exist;
    uint8 is_use;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_access_id, _SHR_E_PARAM, "entry_access_id");

    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

    /*
     * first check if exist, return access_id if exist
     */
    for (idx = 0; idx < max_capacity; idx++)
    {

        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_get(unit, idx, &is_use, NULL, NULL, NULL, &key_get));
        is_exist = (key_get == key) ? 1 : 0;
        if (is_use && is_exist)
        {
            *entry_access_id = idx;
            SHR_EXIT();
        }
    }

    /*
     * Get the first free index 
     */
    for (idx = 0; idx < max_capacity; idx++)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_get(unit, idx, &is_use, NULL, NULL, NULL, NULL));
        if (!is_use)
        {
            *entry_access_id = idx;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_coe_enable_set(
    int unit,
    bcm_port_t port,
    int value)
{
    uint8 is_other_set;
    uint32 entry_handle_id;
    int enable, channel_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 port_pp;
    bcm_core_t core_id;
    bcm_port_t local_port;
    uint8 is_sit_profile_used, is_last;
    uint32 virtual_tcam_app_db_id = 0;
    uint32 flags;
    bcm_pbmp_t pbm_same_interface;
    uint32 ptc;
    bcm_port_t port_i, master_port;
    uint8 ptc_profile, sit_profile, sit_profile_old;
    uint32 access_id = 0;
    int rv;
    int ii;
    dbal_field_types_defs_e field_type;
    CONST dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    enable = value ? 1 : 0;

    SHR_IF_ERR_EXIT(dnx_port_sit_coe_set_verify(unit, port, value));

    /** check if first COE port */
    SHR_IF_ERR_EXIT(dnx_port_sit_port_status_get(unit, port, DNX_PORT_TYPE_COE, &is_other_set));

    /** first get PTC: TM port of channel 0 */
    ptc = PTC_INVALID;
    master_port = port;
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &pbm_same_interface));
    BCM_PBMP_ITER(pbm_same_interface, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port_i, &channel_id));
        if (channel_id == 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port_i, &core_id, &ptc));
            master_port = port_i;
            break;
        }
    }
    if (ptc == PTC_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "First channel of interface is not defined!\n");
    }

    /** set tpid */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));
    if (enable && !is_other_set)
    {
        /**create profile info */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_COE,
                                     0xFFFF);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (!enable && !is_other_set)
    {
        /** free profile info */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_COE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set swap */
    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_set
                    (unit, master_port, enable, DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID1_TRIGGER));

    /*
     * select PTC profile
     */
    ptc_profile =
        enable ? DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE : DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;

    /*
     * Get the childfield PRTTCAM_KEY_VP_COE AppDb ID 
     */
    
    SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, "PRT_KBR_INFO_HALF", &field_type));
    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
    for (ii = 0; ii < field_type_info->nof_child_fields; ii++)
    {
        if (field_type_info->sub_field_info[ii].sub_field_id == DBAL_FIELD_PRTTCAM_KEY_VP_COE)
        {
            virtual_tcam_app_db_id = field_type_info->sub_field_info[ii].encode_info.input_param;
            break;
        }
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));

    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, local_port, &core_id, &port_pp));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, ptc);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                                    ptc_profile);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FWD_PTC_PROFILE, INST_SINGLE, ptc_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER_RAW, INST_SINGLE,
                                     virtual_tcam_app_db_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set SIT profile for port */
    sit_profile_old = 0;
    SHR_IF_ERR_EXIT(dnx_port_sit_get_sit_profile(unit, DNX_SIT_TYPE_DOT1Q, &enable, &sit_profile));
    if (!enable)
    {
        sit_profile_old = sit_profile;
        sit_profile = DNX_PORT_DEFAULT_SIT_PROFILE;
    }
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_set(unit, port, &sit_profile, NULL, NULL, NULL));

    /** update port SW TBL */
    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port, &flags));
    if (enable)
    {
        flags = flags | DNX_ALGO_PORT_APP_FLAG_COE;
    }
    else
    {
        flags = flags & (~DNX_ALGO_PORT_APP_FLAG_COE);
    }
    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_set(unit, port, flags));

    /**free sit profile if it's last COE port*/
    if (!enable)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_profile_is_used(unit, sit_profile_old, &is_sit_profile_used));
        if (!is_sit_profile_used)
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_free(unit, sit_profile_old, &is_last));
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_clear(unit, sit_profile_old));
        }
    }

    /** clear virtual port mapping on TCAM when disable*/
    if (!enable)
    {

        BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
        {
            rv = dnx_port_sit_virtual_port_access_id_get_by_port(unit, local_port, &access_id);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                /** clear TCAM entry */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

                /** clear TCAM SW table */
                SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_set(unit, access_id, 0, 0, 0, 0, 0));
            }
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_coe_enable_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    uint32 flags;
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (!is_phy_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local port is needed for COE!\n");
    }

    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port, &flags));
    *value = DNX_PORT_IS_COE_PORT(flags) ? 1 : 0;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_port_sit_pon_set_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    uint32 flags;
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (!is_phy_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local port is needed to set COE!\n");
    }

    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port, &flags));
    if (value)
    {
        if (DNX_PORT_IS_PON_PORT(flags))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "This port is already PON type for enable opeartion!\n");
        }
    }
    else
    {
        if (!DNX_PORT_IS_PON_PORT(flags))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "This port is not PON type for disable opeartion!\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_pon_enable_set(
    int unit,
    bcm_port_t port,
    int value)
{
    uint8 is_other_set;
    uint32 entry_handle_id;
    int enable;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core_id;
    bcm_port_t local_port;
    uint8 is_sit_profile_used, is_last;
    uint32 flags;
    uint32 ptc, virtual_tcam_app_db_id = 0;
    uint8 ptc_profile, sit_profile, sit_profile_old;
    int ii;
    dbal_field_types_defs_e field_type;
    CONST dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    enable = value ? 1 : 0;

    SHR_IF_ERR_EXIT(dnx_port_sit_pon_set_verify(unit, port, value));

    /** check if first PON port */
    SHR_IF_ERR_EXIT(dnx_port_sit_port_status_get(unit, port, DNX_PORT_TYPE_PON, &is_other_set));

    /** set tpid */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SIT_INGRESS_GENERAL_CONFIGURATION, &entry_handle_id));
    if (enable && !is_other_set)
    {
        /**create profile info */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_PON,
                                     0xFFFF);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (!enable && !is_other_set)
    {
        /** free profile info */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SWAP_TPID_MASK, DNX_TAG_SWAP_TPID_INDEX_PON, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set swap */
    SHR_IF_ERR_EXIT(dnx_port_sit_tag_swap_set(unit, port, enable, DBAL_ENUM_FVAL_SIT_TAG_SWAP_MODE_TPID1_TRIGGER));

    /*
     * Get the childfield PRTTCAM_KEY_VP_PON AppDb ID 
     */
    
    SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, "PRT_KBR_INFO_HALF", &field_type));
    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
    for (ii = 0; ii < field_type_info->nof_child_fields; ii++)
    {
        if (field_type_info->sub_field_info[ii].sub_field_id == DBAL_FIELD_PRTTCAM_KEY_VP_PON)
        {
            virtual_tcam_app_db_id = field_type_info->sub_field_info[ii].encode_info.input_param;
            break;
        }
    }

    /*
     * Set PTC, PTC profile and VP_PON AppDb 
     */
    ptc_profile =
        enable ? DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON : DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PTC_PORT, &entry_handle_id));
    BCM_PBMP_ITER(gport_info.local_port_bmp, local_port)
    {
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_id));
        dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PTC, ptc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, ptc);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE,
                                     virtual_tcam_app_db_id);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, INST_SINGLE,
                                    ptc_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** set SIT profile for port */
    sit_profile_old = 0;
    SHR_IF_ERR_EXIT(dnx_port_sit_get_sit_profile(unit, DNX_SIT_TYPE_PON, &enable, &sit_profile));
    if (!enable)
    {
        sit_profile_old = sit_profile;
        sit_profile = DNX_PORT_DEFAULT_SIT_PROFILE;
    }
    SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_set(unit, port, &sit_profile, NULL, NULL, NULL));

    /** update port SW TBL */
    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, port, &flags));
    if (enable)
    {
        flags = flags | DNX_ALGO_PORT_APP_FLAG_PON;
    }
    else
    {
        flags = flags & (~DNX_ALGO_PORT_APP_FLAG_PON);
    }
    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_set(unit, port, flags));

    /**free sit profile if it's last COE port*/
    if (!enable)
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_profile_is_used(unit, sit_profile_old, &is_sit_profile_used));
        if (!is_sit_profile_used)
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_free(unit, sit_profile_old, &is_last));
            SHR_IF_ERR_EXIT(dnx_port_sit_profile_hw_clear(unit, sit_profile_old));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_pon_enable_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    int rv;
    uint32 flags;
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    *value = 0;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (!is_phy_port)
    {
        SHR_EXIT();
    }

    rv = dnx_port_sit_port_flag_get(unit, port, &flags);
    if (rv != _SHR_E_PORT)
    {
        SHR_IF_ERR_EXIT(rv);
        *value = DNX_PORT_IS_PON_PORT(flags) ? 1 : 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_application_mapping_info_set(
    int unit,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{
    uint32 entry_handle_id;
    uint32 entry_access_id = 0;

    int max_capacity;

    uint32 complete_field_val;
    uint32 field_in_struct_val;

    uint16 pp_port;
    bcm_port_t local_port;
    int core_id, valid;
    uint32 ptc;
    bcm_core_t core_tmp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    if (type == bcmPortExtenderMappingTypePonTunnel)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

        local_port = mapping_info->phy_port & 0xff;
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_tmp));

        complete_field_val = 0;
        /** PTC attribute: PTC */
        field_in_struct_val = ptc;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_PON, DBAL_FIELD_PTC, &field_in_struct_val,
                         &complete_field_val));
        /** PON_SUBPORT_ID */
        field_in_struct_val = PON_SUBPORT_ID_GET(mapping_info->tunnel_id);
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_PON, DBAL_FIELD_PON_TAG_PON_SUBPORT_ID, &field_in_struct_val,
                         &complete_field_val));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRTTCAM_KEY_VP_PON, complete_field_val);

        /*
         * in pp_port mapping
         */
        pp_port = (uint16) mapping_info->pp_port;
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);

        /*
         * src system_port mapping, useless
         */
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, 0);
        entry_access_id = max_capacity;
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_access_id_alloc(unit, complete_field_val, 0, &entry_access_id));
        if (entry_access_id >= max_capacity)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "Can't install entry to Table, it's full, capacity is: %d!\n", max_capacity);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

        /** Use flag - DBAL_COMMIT_FORCE to support DPP */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

        /*
         * Update SW table for TCAM management
         */
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_set
                        (unit, entry_access_id, 1, 0, pp_port, 0, complete_field_val));
    }
    else if (type == bcmPortExtenderMappingTypePortVlan)
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

        

        complete_field_val = 0;
        /** PTC attribute: PTC */
        field_in_struct_val = mapping_info->phy_port & 0x3f;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_COE, DBAL_FIELD_PTC_ATTRIBUTE, &field_in_struct_val,
                         &complete_field_val));
        /** VID */
        field_in_struct_val = mapping_info->vlan & 0xff;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_COE, DBAL_FIELD_VID, &field_in_struct_val,
                         &complete_field_val));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, complete_field_val);

        /*
         * in pp_port mapping
         */
        pp_port = (uint16) mapping_info->pp_port;
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);

        /*
         * src system_port mapping, useless
         */
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, 0);
        entry_access_id = max_capacity;
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_access_id_alloc(unit, complete_field_val, 0, &entry_access_id));
        if (entry_access_id >= max_capacity)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "Can't install entry to Table, it's full, capacity is: %d!\n", max_capacity);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

        /** Use flag - DBAL_COMMIT_FORCE to support DPP */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

        /*
         * Update SW table for TCAM management
         */
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_set
                        (unit, entry_access_id, 1, 0, pp_port, 0, complete_field_val));

        /*
         * set mapping for egress 
         */
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.valid.get(unit, core_id, mapping_info->pp_port, &valid));
            if (!valid)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get(unit, core_id, mapping_info->pp_port, &local_port));
            /*
             * master port should equal to input PTC
             */
            SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_tmp));
            if ((mapping_info->phy_port & 0xff) != ptc)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_port_sit_egress_port_set(unit, local_port, NULL, NULL, NULL, &mapping_info->vlan));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in port_sit.h for function description
 */
shr_error_e
dnx_port_sit_application_mapping_info_get(
    int unit,
    bcm_port_extender_mapping_type_t type,
    bcm_port_extender_mapping_info_t * mapping_info)
{
    int max_capacity;
    uint32 entry_handle_id;
    int idx;

    uint32 complete_field_val;
    uint32 field_in_struct_val;

    uint32 key_get;
    uint16 pp_port;
    uint8 is_use;
    bcm_port_t local_port;
    uint32 ptc;
    bcm_core_t core_tmp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mapping_info, _SHR_E_PARAM, "mapping_info");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

    if (type == bcmPortExtenderMappingTypePonTunnel)
    {
        local_port = mapping_info->phy_port & 0xff;
        SHR_IF_ERR_EXIT(dnx_port_ptc_get(unit, local_port, &ptc, &core_tmp));

        /** get pp_port according to phy_port + VID */
        complete_field_val = 0;
        field_in_struct_val = ptc;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_PON, DBAL_FIELD_PTC, &field_in_struct_val,
                         &complete_field_val));
        field_in_struct_val = PON_SUBPORT_ID_GET(mapping_info->tunnel_id);
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_PON, DBAL_FIELD_PON_TAG_PON_SUBPORT_ID, &field_in_struct_val,
                         &complete_field_val));

        for (idx = 0; idx < max_capacity; idx++)
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_get(unit, idx, &is_use, NULL, &pp_port, NULL, &key_get));
            if ((key_get == complete_field_val) && is_use)
            {
                mapping_info->pp_port = pp_port;
                break;
            }
        }
    }
    else if (type == bcmPortExtenderMappingTypePortVlan)
    {
        /** get pp_port according to phy_port + VID */
        complete_field_val = 0;
        field_in_struct_val = mapping_info->phy_port & 0xff;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_COE, DBAL_FIELD_PTC_ATTRIBUTE, &field_in_struct_val,
                         &complete_field_val));
        field_in_struct_val = mapping_info->vlan & 0xff;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_PRTTCAM_KEY_VP_COE, DBAL_FIELD_VID, &field_in_struct_val,
                         &complete_field_val));

        for (idx = 0; idx < max_capacity; idx++)
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_tcam_sw_get(unit, idx, &is_use, NULL, &pp_port, NULL, &key_get));
            if ((key_get == complete_field_val) && is_use)
            {
                mapping_info->pp_port = pp_port;
                break;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
