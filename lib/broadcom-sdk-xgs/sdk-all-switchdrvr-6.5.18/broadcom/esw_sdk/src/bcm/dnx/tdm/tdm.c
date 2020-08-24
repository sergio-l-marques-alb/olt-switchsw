/** \file tdm.c
 * $Id$
 *
 * TDM procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TDM

/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>

#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tdm.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <bcm_int/dnx/tdm/tdm.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_out_header_type_set(
    int unit,
    int switch_tdm_header_type)
{
    uint32 tdm_header_is_ftmh;
    /*
     * The following variable holds the value of OTMH_4B_WITH_CUD_FORMAT field in PEMLA_OTMHFORMATS DBAL Table
     */
    uint32 tdm_header_is_otmh_4byte_with_cud;
    int tdm_mode;

    SHR_FUNC_INIT_VARS(unit);
    switch (switch_tdm_header_type)
    {
        case BCM_TDM_CONTROL_OUT_HEADER_OTMH:
        {
            tdm_header_is_ftmh = 0;
            tdm_header_is_otmh_4byte_with_cud = 0;
            break;
        }
        case BCM_TDM_CONTROL_OUT_HEADER_FTMH:
        {
            tdm_header_is_ftmh = 1;
            tdm_header_is_otmh_4byte_with_cud = 0;
            break;
        }
        case BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD:
        {
            tdm_header_is_ftmh = 1;
            tdm_header_is_otmh_4byte_with_cud = 1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch header type %d is not supported!", switch_tdm_header_type);
            break;
        }
    }

    tdm_mode = dnx_data_tdm.params.mode_get(unit);

    /*
     * "OTMH 4Byte with CUD" outgoing TDM header is supported only when tdm_mode==TDM_MODE_OPTIMIZED
     */
    if ((tdm_header_is_otmh_4byte_with_cud == 1) && (tdm_mode != TDM_MODE_OPTIMIZED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "OTMH-4Byte-With-CUD Outgoing TDM Header is supported only when tdm_mode=TDM_MODE_OPTIMIZED !");
    }

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_PEMLA_TDM, 0, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_HEADER_IS_FTMH, INST_SINGLE, tdm_header_is_ftmh,
                                     GEN_DBAL_FIELD_LAST_MARK));

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_PEMLA_OTMHFORMATS, 0, 1, GEN_DBAL_FIELD32 /* type */ , DBAL_FIELD_OTMH_4B_WITH_CUD_FORMAT /* Field 
                                      * Id */ , INST_SINGLE /* instance */ ,
                                     tdm_header_is_otmh_4byte_with_cud /* value */ ,
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_out_header_type_get(
    int unit,
    int *switch_tdm_header_type)
{

    uint32 tdm_header_is_ftmh = 0;
    /*
     * The following variable holds the value of OTMH_4B_WITH_CUD_FORMAT field in PEMLA_OTMHFORMATS DBAL Table
     * The initialization is not mandatoty. Good practice.
     */
    uint32 tdm_header_is_otmh_4b_with_cud_format = 0;

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TDM, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_HEADER_IS_FTMH, INST_SINGLE, &tdm_header_is_ftmh);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_OTMHFORMATS, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OTMH_4B_WITH_CUD_FORMAT, INST_SINGLE,
                               &tdm_header_is_otmh_4b_with_cud_format);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (tdm_header_is_ftmh)
    {

        case 0:
        {
            *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_OTMH;
            break;
        }
        case 1:
        {
            if (tdm_header_is_otmh_4b_with_cud_format == 1)
            {
                *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_OTMH_4B_WITH_CUD;
            }
            else
            {
                *switch_tdm_header_type = BCM_TDM_CONTROL_OUT_HEADER_FTMH;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch header type %d is not supported!", *switch_tdm_header_type);
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_system_ftmh_type_set(
    int unit,
    uint32 use_optimized_ftmh)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_TDM, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_OPTIMIZED_FTMH, INST_SINGLE, use_optimized_ftmh);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_TDM_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_FTMH_OPTIMIZED, INST_SINGLE, use_optimized_ftmh);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Enable Egress TDM
 */
static shr_error_e
dnx_tdm_global_config(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    int tdm_tc, tdm_dp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (enable == TRUE)
    {
        tdm_tc = dnx_data_tdm.params.tdm_tc_get(unit);
        tdm_dp = dnx_data_tdm.params.tdm_dp_get(unit);
    }
    else
    {
        tdm_tc = 0;
        tdm_dp = 0;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_EGRESS_MODE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PACKET_TC, INST_SINGLE, tdm_tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PACKET_DP, INST_SINGLE, tdm_dp);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_system_ftmh_type_get(
    int unit,
    uint32 *use_optimized_ftmh_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_FTMH_OPTIMIZED, INST_SINGLE, use_optimized_ftmh_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_stream_mc_prefix_set(
    int unit,
    int stream_mc_prefix)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((unsigned int) stream_mc_prefix >= dnx_data_tdm.params.global_sid_offset_nof_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Required TDM global sid offset size, %d, is out of allowed range:%d-%d\n",
                     stream_mc_prefix, 0, dnx_data_tdm.params.global_sid_offset_nof_get(unit) - 1);
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_GLOBAL_SID_OFFSET, INST_SINGLE, stream_mc_prefix,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_stream_mc_prefix_get(
    int unit,
    int *stream_mc_prefix_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_GLOBAL_SID_OFFSET, INST_SINGLE,
                                     (uint32 *) stream_mc_prefix_p, GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_max_pkt_get(
    int unit,
    int *max_size_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(max_size_p, _SHR_E_PARAM, "max_size_p");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_MAX_SIZE, INST_SINGLE, (uint32 *) max_size_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_max_pkt_set(
    int unit,
    int max_size)
{
    uint32 entry_handle_id;
    int pkt_size_upper_limit, pkt_size_lower_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    pkt_size_lower_limit = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    pkt_size_upper_limit = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);

    if ((max_size > pkt_size_upper_limit) || (max_size < pkt_size_lower_limit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM Packet maximal size:%d is out of allowed range:%d-%d\n", max_size,
                     pkt_size_lower_limit, pkt_size_upper_limit);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_MAX_SIZE, INST_SINGLE, max_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_min_pkt_get(
    int unit,
    int *min_size_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(min_size_p, _SHR_E_PARAM, "min_size_p");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_MIN_SIZE, INST_SINGLE, (uint32 *) min_size_p);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_min_pkt_set(
    int unit,
    int min_size)
{
    uint32 entry_handle_id;
    int pkt_size_upper_limit, pkt_size_lower_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    pkt_size_lower_limit = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    pkt_size_upper_limit = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);

    if ((min_size > pkt_size_upper_limit) || (min_size < pkt_size_lower_limit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM Packet minimal size:%d is out of allowed range:%d-%d\n", min_size,
                     pkt_size_lower_limit, pkt_size_upper_limit);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_MIN_SIZE, INST_SINGLE, min_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see bcm_int/dnx/tdm/tdm.h
 */
shr_error_e
dnx_tdm_init(
    int unit)
{
    int tdm_mode;
    int min_size, max_size;
    uint32 use_optimized_ftmh;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
    {
        /*
         * TDM is not supported.
         * Just mark TDM is not enabled and leave
         */
        int enable;

        enable = 0;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_CONFIGURATION, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TDM_EGRESS_MODE, INST_SINGLE, enable,
                                         GEN_DBAL_FIELD_LAST_MARK));
        SHR_EXIT();
    }
    if (((tdm_mode = dnx_data_tdm.params.mode_get(unit)) == TDM_MODE_NONE))
    {
        /*
         * Disable Egress TDM and leave
         */
        SHR_IF_ERR_EXIT(dnx_tdm_global_config(unit, FALSE));
        SHR_EXIT();
    }
    /*
     * check that flexe is disabled - otherwise failure
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        if (dnx_data_nif.flexe.flexe_mode_get(unit) != DNX_FLEXE_MODE_DISABLED)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM can not coexist with FLEXE, disable one of them\n");
        }
    }
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_supported))
    {
        uint32 enable_fabric_links_mask;

        /*
         * A value of '2' stands for enabling the action of the selected mask
         * on TDM_DIRECT_LINKS. If a value of '0' is selected then those
         * masks areignored.
         */
        enable_fabric_links_mask = 2;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_LINK_BITMAP_CONFIGURATION, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TDM_MASK_MODE, INST_SINGLE,
                                         enable_fabric_links_mask, GEN_DBAL_FIELD_LAST_MARK));
    }
    {
        /*
         * Reset 'full packet mode' so that, even under TDM, any number of entries may be
         * added to multicast tables. However, this implies rplicator engines may be interleaved.
         */
        int full_packet_mode;

        full_packet_mode = 0;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_FULL_PACKET_MODE, 0, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FULL_PACKET_MODE, INST_SINGLE, full_packet_mode,
                                         GEN_DBAL_FIELD_LAST_MARK));

    }
    if (tdm_mode == TDM_MODE_OPTIMIZED)
    {
        use_optimized_ftmh = 1;
    }
    else if ((tdm_mode == TDM_MODE_STANDARD) || (tdm_mode == TDM_MODE_PACKET))
    {
        use_optimized_ftmh = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bad TDM mode:%d\n", tdm_mode);
    }
    SHR_IF_ERR_EXIT(dnx_tdm_global_config(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_set(unit, use_optimized_ftmh));
    /*
     * Configure Min and Max Bypass TDM packet size to relatively lower and upper limit
     */
    min_size = dnx_data_tdm.params.pkt_size_lower_limit_get(unit);
    max_size = dnx_data_tdm.params.pkt_size_upper_limit_get(unit);
    SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_set(unit, min_size));
    SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_set(unit, max_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TDM related device wide parameters
 *
 * \param [in] unit - unit id
 * \param [in] type - configuration option taken from bcm_tdm_control_t
 * \param [in] arg - argument for the configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_control_set(
    int unit,
    bcm_tdm_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmTdmBypassMinPacketSize:        /* Minimum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_set(unit, arg));
            break;
        }
        case bcmTdmBypassMaxPacketSize:        /* Maximum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_set(unit, arg));
            break;
        }
        case bcmTdmOutHeaderType:      /* One of BCM_TDM_CONTROL_OUT_HEADER_XXX. */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_out_header_type_set(unit, arg));
            break;
        }
        case bcmTdmStreamMulticastPrefix:      /* Set 5 Bit prefix from Stream ID to Multicast ID */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_stream_mc_prefix_set(unit, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Obtain TDM related device wide parameters
 *
 * \param [in] unit - unit id
 * \param [in] type - configuration option taken from bcm_tdm_control_t
 * \param [in] arg  - pointer where parameter value will be put into
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_control_get(
    int unit,
    bcm_tdm_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmTdmBypassMinPacketSize:        /* Minimum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_min_pkt_get(unit, arg));
            break;
        }
        case bcmTdmBypassMaxPacketSize:        /* Maximum packet size for Bypass TDM packet */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_max_pkt_get(unit, arg));
            break;
        }
        case bcmTdmOutHeaderType:      /* One of BCM_TDM_CONTROL_OUT_HEADER_XXX. */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_out_header_type_get(unit, arg));
            break;
        }
        case bcmTdmStreamMulticastPrefix:      /* Set 5 Bit prefix from Stream ID to Multicast ID */
        {
            SHR_IF_ERR_EXIT(dnx_tdm_stream_mc_prefix_get(unit, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
