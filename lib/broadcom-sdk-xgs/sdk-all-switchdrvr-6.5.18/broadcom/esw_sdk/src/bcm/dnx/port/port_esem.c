/*
 ** \file port_esem.c $Id$ Per PORT ESEM command mgmt procedures for DNX.
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
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/switch/switch_svtag.h>

/*
 * }
 */

/**
 * \brief -
 *  Set ESEM access command data to hardware table.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_data_set(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t esem_cmd_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to esem-cmd table with the given esem_cmd
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_ACCESS_CMD_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, esem_cmd);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_1_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].default_result_profile);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_2_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].default_result_profile);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_3_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].default_result_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Clear the ESEM access command data from hardware table.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_data_clear(
    int unit,
    int esem_cmd)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Remove the attributes from esem-cmd table in the cmd-id*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_ACCESS_CMD_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, esem_cmd);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access command data according to the given command ID.
    See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_data_sw_get(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    int *ref_count)
{
    int ref_count_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for esem access cmd data is needed!\n");

    sal_memset(esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));

    /** Check the esem cmd is in range.*/
    if (esem_cmd < 0 || esem_cmd >= dnx_data_esem.access_cmd.nof_cmds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem access cmd (%d) is out of range (< %d)\n",
                     esem_cmd, dnx_data_esem.access_cmd.nof_cmds_get(unit));
    }

    /** Rerieve the cmd data from profile data base.*/
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.profile_data_get(unit, esem_cmd, &ref_count_i, esem_cmd_data));

    if (ref_count != NULL)
    {
        *ref_count = ref_count_i;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM command tags according to the given port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - DNX_PORT_ESEM_CMD_XXX.
 * \param [in] port - the given port
 * \param [out] esem_cmd_tags - Available ESEM access command tags for the port.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_esem_cmd_tags_get(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    dnx_esem_cmd_tag_bmp_range_t * esem_cmd_tags)
{
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd_tags, _SHR_E_INTERNAL, "Valid pointer for esem_cmd_tags is needed!\n");

    *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63;

    /*
     * Get the right esem cmd pool according to lif type
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_RANGE_0_3))
        {
            *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_0_3;
        }
        else if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_RANGE_0_7))
        {
            *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_0_7;
        }
        else if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_RANGE_0_15))
        {
            *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_0_15;
        }
        else if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_RANGE_60_63))
        {
            *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_60_63;
        }
        else if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_RANGE_0_63))
        {
            *esem_cmd_tags = DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63;
        }
        else
        {
            dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
            dbal_table_field_info_t table_field_info;
            int tag_bmp, field_tags, field_tags_shift, grain_size;

            /** Get the local-out-lif information.*/
            SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                        (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                         &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

            /** Consider to get the tags by analyzing the field size and prefix in the next step.*/
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, gport_hw_resources.outlif_dbal_table_id,
                                                       DBAL_FIELD_ESEM_COMMAND, FALSE,
                                                       gport_hw_resources.outlif_dbal_result_type, 0,
                                                       &table_field_info));
            /*
             * Calcualate the tag according to the prefix
             * ESEM CMD HW field size is 6. We assume the minimum table field size is 2, so we have 2^(6-2)=16 tags,
             * and the tag grain size should be 2^2=4. We calculate the available cmd range which is represented by
             * tags in bitmap. 1 bit in the bitmap represents 1 tag.
             */

            /** Calculate tag grain size*/
            grain_size = dnx_data_esem.access_cmd.nof_cmds_get(unit) / DNX_ALGO_ESEM_CMD_RES_NOF_TAGS;

            /** Calculate the cmd tags bitmap the field can use.*/
            field_tags = (1 << ((1 << table_field_info.field_nof_bits) / grain_size)) - 1;

            /** Calculate the range the tags should locate at.*/
            field_tags_shift = (table_field_info.arr_prefix << table_field_info.field_nof_bits) / grain_size;

            /** Calculate the final range tags bitmap.*/
            tag_bmp = field_tags << field_tags_shift;

            
            if ((tag_bmp != DNX_ALGO_ESEM_ACC_CMD_RANGE_0_3) &&
                (tag_bmp != DNX_ALGO_ESEM_ACC_CMD_RANGE_0_7) &&
                (tag_bmp != DNX_ALGO_ESEM_ACC_CMD_RANGE_0_15) &&
                (tag_bmp != DNX_ALGO_ESEM_ACC_CMD_RANGE_60_63) && (tag_bmp != DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated tag bitmap (0x%04X) is not valid. port 0x%08X, "
                             "Result type %s, ESEM command field size %d, arr prefix size %d, arr prefix 0x%02X.",
                             tag_bmp, port, dbal_result_type_to_string(unit, gport_hw_resources.outlif_dbal_table_id,
                                                                       gport_hw_resources.outlif_dbal_result_type),
                             table_field_info.field_nof_bits, table_field_info.arr_prefix_size,
                             table_field_info.arr_prefix);
            }
            *esem_cmd_tags = tag_bmp;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access type according to ESEM default result profile.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_access_type_get(
    int unit,
    int esem_default_result_profile,
    dnx_esem_access_type_t * access_type)
{
    uint32 entry_handle_id;
    uint32 result_type = 0;
    uint8 last_layer = 0;
    int res;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(access_type, _SHR_E_INTERNAL, "A valid pointer for access_type is needed!\n");

    /** Check the default result profile is in range*/
    if (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "default_esem_result_profile (%d) is out of range [0, %d)!\n",
                     esem_default_result_profile, dnx_data_esem.default_result_profile.nof_profiles_get(unit));
    }

    /** Get the access type*/
    if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_ac_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_ETH_AC;
    }
    else if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_native_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_ETH_NATIVE_AC;
    }
    else if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_dual_homing_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_DUAL_HOME;
    }
    else
    {
        /** Get the access type from hw table if the default result profile is not predefined. */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                                   esem_default_result_profile);

        res = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (res == _SHR_E_NOT_FOUND)
        {
            *access_type = ESEM_ACCESS_TYPE_COUNT;
            SHR_ERR_EXIT(_SHR_E_PARAM, "Esem default result entry(%d) should be create first!\n",
                         esem_default_result_profile);
        }
        else
        {
            SHR_IF_ERR_EXIT(res);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        }

        if (result_type == DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id,
                                                               DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &last_layer));
            if (last_layer == TRUE)
            {
                *access_type = ESEM_ACCESS_TYPE_ETH_AC;
            }
            else
            {
                *access_type = ESEM_ACCESS_TYPE_ETH_NATIVE_AC;
            }
        }
        else if (result_type == DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING)
        {
            *access_type = ESEM_ACCESS_TYPE_DUAL_HOME;
        }
        else
        {
            *access_type = ESEM_ACCESS_TYPE_COUNT;
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid result type(%d) in ESEM default result table!\n", result_type);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access interface.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_access_if_get(
    int unit,
    dnx_esem_access_type_t access_type,
    dnx_esem_cmd_data_t * esem_cmd_data,
    dnx_esem_access_if_t * esem_if,
    dbal_enum_value_field_esem_offset_e * esem_entry_offset)
{
    dnx_esem_access_type_t old_access_type;
    dbal_enum_value_field_esem_offset_e esem_designated_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_if, _SHR_E_INTERNAL, "A valid pointer for esem_if is needed!\n");

    /*
     * Init esem access interface and esem cmd pool
     */
    *esem_if = ESEM_ACCESS_IF_1;
    old_access_type = ESEM_ACCESS_TYPE_COUNT;
    esem_designated_index = DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE;

    /*
     * Get esem access interface according to access type
     */
    switch (access_type)
    {
        case ESEM_ACCESS_TYPE_ETH_AC:
        case ESEM_ACCESS_TYPE_PORT_SA:
        {
            *esem_if = ESEM_ACCESS_IF_1;
            break;
        }
        case ESEM_ACCESS_TYPE_ETH_NATIVE_AC:
        {
            /*
             * In case of ESEM2 is dedicated for tunnel encap (Vxlan.VNI, IP-tunnel.sip, etc),
             * ESEM1 may be used for outer AC or native AC, but not both at the same time.
             * If it is used for native AC, outer AC should be created explicitly in EEDB,
             * and no default outer AC from port can be used.
             */
            *esem_if = ESEM_ACCESS_IF_2;
            if ((esem_cmd_data != NULL) && (esem_cmd_data->esem[*esem_if].valid == TRUE))
            {
                /** Get the original access type in the cmd access*/
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                (unit, esem_cmd_data->esem[*esem_if].default_result_profile, &old_access_type));

                /** Try to alloc ESEM_ACCESS_IF_1 for native AC*/
                if (old_access_type != access_type)
                {
                    if (esem_cmd_data->esem[ESEM_ACCESS_IF_1].valid == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                        (unit, esem_cmd_data->esem[ESEM_ACCESS_IF_1].default_result_profile,
                                         &old_access_type));
                        if (old_access_type != access_type)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available ESEM access for access type (%d)!\n",
                                         access_type);
                        }
                    }
                    /** ESEM_ACCESS_IF_1 is available for native AC.*/
                    *esem_if = ESEM_ACCESS_IF_1;
                }
            }
            break;
        }
        case ESEM_ACCESS_TYPE_FODO_NETWORK:
        case ESEM_ACCESS_TYPE_VXLAN_VNI:
        case ESEM_ACCESS_TYPE_GRE_KEY:
        case ESEM_ACCESS_TYPE_MIM_ISID:
        {
            /** ESEM_ACCESS_IF_2 should be used for current application. */
            *esem_if = ESEM_ACCESS_IF_2;

            /** Try to move the original application in ESEM_ACCESS_IF_2 to other access interface.*/
            if ((esem_cmd_data != NULL) && (esem_cmd_data->esem[*esem_if].valid == TRUE))
            {
                /** Get the original access type in the cmd access*/
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                (unit, esem_cmd_data->esem[*esem_if].default_result_profile, &old_access_type));
                if (old_access_type != access_type)
                {
                    if (esem_cmd_data->esem[ESEM_ACCESS_IF_1].valid == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                        (unit, esem_cmd_data->esem[ESEM_ACCESS_IF_1].default_result_profile,
                                         &old_access_type));
                        if (old_access_type != access_type)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available ESEM access for access type (%d)!\n",
                                         access_type);
                        }
                        else
                        {
                            *esem_if = ESEM_ACCESS_IF_1;
                        }
                    }
                    else
                    {
                        /** IF_1 is not in use now, move the original access from IF_2 to IF_1*/
                        sal_memcpy(&(esem_cmd_data->esem[ESEM_ACCESS_IF_1]), &(esem_cmd_data->esem[ESEM_ACCESS_IF_2]),
                                   sizeof(dnx_esem_cmd_if_data_t));
                    }
                }
            }
            break;
        }
        case ESEM_ACCESS_TYPE_DUAL_HOME:
        {
            /** ESEM_ACCESS_IF_1 must be used for current application. */
            *esem_if = ESEM_ACCESS_IF_1;
            esem_designated_index = DBAL_ENUM_FVAL_ESEM_OFFSET_BELOW;

            /** Try to move the original application in ESEM_ACCESS_IF_1 to other access interface.*/
            if ((esem_cmd_data != NULL) && (esem_cmd_data->esem[*esem_if].valid == TRUE))
            {
                /** Get the original access type in the cmd access*/
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                (unit, esem_cmd_data->esem[*esem_if].default_result_profile, &old_access_type));
                if (old_access_type != access_type)
                {
                    if (esem_cmd_data->esem[ESEM_ACCESS_IF_2].valid == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_type_get
                                        (unit, esem_cmd_data->esem[ESEM_ACCESS_IF_2].default_result_profile,
                                         &old_access_type));
                        if (old_access_type != access_type)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available ESEM access for access type (%d)!\n",
                                         access_type);
                        }
                        else
                        {
                            *esem_if = ESEM_ACCESS_IF_2;
                        }
                    }
                    else
                    {
                        /** IF_2 is not in use now, move the original access from IF_1 to IF_2*/
                        sal_memcpy(&(esem_cmd_data->esem[ESEM_ACCESS_IF_2]), &(esem_cmd_data->esem[ESEM_ACCESS_IF_1]),
                                   sizeof(dnx_esem_cmd_if_data_t));
                    }
                }
            }
            break;
        }
        case ESEM_ACCESS_TYPE_SVTAG:
        {
            *esem_if = DNX_ALGO_ESEM_SVTAG_ESEM_CMD_IF;
            esem_designated_index = DBAL_ENUM_FVAL_ESEM_OFFSET_LAST;
            if (esem_cmd_data->esem[*esem_if].valid)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem access type(%d), require interface 2!\n", access_type);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unavailable esem access type(%d), see dnx_esem_access_type_t for available options!\n",
                         access_type);
        }
    }

    /** Fill the entry offset ABOVE/BELOW/BELOW+1 its original EES entry */
    if (esem_entry_offset != NULL)
    {
        *esem_entry_offset = esem_designated_index;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Fill ESEM command data according to esem access app-db-id, access_type.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - DNX_PORT_ESEM_CMD_
 * \param [in] esem_app_db - esem access app-db-id.
 * \param [in] access_type - esem access type.
 *             See dnx_esem_access_type_t for the available types.
 * \param [in] default_result_profile - esem default result profile.
 *             DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID means using the previous
 *             default result profile.
 * \param [in] old_esem_cmd - original esem cmd used by the port.
 * \param [out] esem_cmd_data - Esem access data.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  dnx_port_esem_cmd_access_if_get
 */
static shr_error_e
dnx_port_esem_cmd_info_collect(
    int unit,
    uint32 flags,
    dbal_enum_value_field_esem_app_db_id_e esem_app_db,
    dnx_esem_access_type_t access_type,
    int default_result_profile,
    int old_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data)
{
    dnx_esem_access_if_t esem_if;
    dbal_enum_value_field_esem_offset_e esem_entry_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for esem_cmd_data is needed!\n");

    /*
     * Get esem access interface accoring to access type
     */
    /** By default, we assume esem result is above current entry in the etps.*/
    esem_entry_offset = DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_if_get(unit, access_type, esem_cmd_data, &esem_if, &esem_entry_offset));

    /*
     * Assigne esem cmd data
     */
    esem_cmd_data->esem[esem_if].valid = TRUE;
    esem_cmd_data->esem[esem_if].designated_offset = esem_entry_offset;
    esem_cmd_data->esem[esem_if].app_db_id = esem_app_db;

    if (default_result_profile == DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID)
    {
        /** Get old esem-default-result-profile for old-esem-cmd */
        dnx_esem_cmd_data_t old_esem_cmd_data;

        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, &old_esem_cmd_data, NULL));
        esem_cmd_data->esem[esem_if].default_result_profile = old_esem_cmd_data.esem[esem_if].default_result_profile;
    }
    else
    {
        /** Use the given default profile if it is valid */
        esem_cmd_data->esem[esem_if].default_result_profile = default_result_profile;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify the inputs to dnx_port_esem_cmd_algo_exchange.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - flags, DNX_ALGO_TEMPLATE_ALLOCATE_*.
 * \param [in] esem_cmd_data - ESEM access command data.
 * \param [in] esem_cmd_tags - Tags used for ESEM command allocation.
 * \param [out] new_esem_cmd - pointer to the ESEM command allocated.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_esem_cmd_algo_exchange_verify(
    int unit,
    uint32 flags,
    dnx_esem_cmd_data_t * esem_cmd_data,
    dnx_esem_cmd_tag_bmp_range_t esem_cmd_tags,
    int *new_esem_cmd,
    uint8 *is_first,
    uint8 *is_last)
{
    int rv;
    int profile_tag;
    int tmp_esem_cmd;
    SHR_FUNC_INIT_VARS(unit);

    /** The given pointer should not be NULL*/
    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for esem access cmd data is needed");
    SHR_NULL_CHECK(new_esem_cmd, _SHR_E_INTERNAL, "A valid pointer for newly allocated esem access cmd is needed");
    SHR_NULL_CHECK(is_first, _SHR_E_INTERNAL, "A valid pointer for is_first is needed");
    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed");

    /** Validate the tag bitmap.*/
    if (esem_cmd_tags == DNX_ALGO_ESEM_ACC_CMD_RANGE_NONE || esem_cmd_tags >= (1 << DNX_ALGO_ESEM_CMD_RES_NOF_TAGS))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Given tag bitmap (0x%04X) is not valid.", esem_cmd_tags);
    }

    /** Validate the profile.*/
    if (flags & DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID)
    {
        if (*new_esem_cmd >= dnx_data_esem.access_cmd.nof_cmds_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Given esem access cmd (%d) is out of range ( < %d).",
                         *new_esem_cmd, dnx_data_esem.access_cmd.nof_cmds_get(unit));
        }

        profile_tag = *new_esem_cmd * DNX_ALGO_ESEM_CMD_RES_NOF_TAGS / dnx_data_esem.access_cmd.nof_cmds_get(unit);

        if (!((esem_cmd_tags >> profile_tag) & 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Given esem access cmd (%d) and the tag bitmap (0x%04X) mismatch.",
                         *new_esem_cmd, esem_cmd_tags);
        }

        rv = algo_port_pp_db.esem.access_cmd.profile_get(unit, esem_cmd_data, &tmp_esem_cmd);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);
            if (*new_esem_cmd != tmp_esem_cmd)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "This data already exists in cmd %d. Given cmd %d", tmp_esem_cmd,
                             *new_esem_cmd);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange out an ESEM access cmd with the cmd data.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - flags, DNX_ALGO_TEMPLATE_ALLOCATE_*.
 * \param [in] prefix - The prefix of the ESEM command.
 * \param [in] esem_cmd_data - ESEM access command data.
 * \param [in] esem_cmd_tags - Tags used for ESEM command allocation.
 * \param [in] old_esem_cmd - The esem command used now by the port/LIF.
 * \param [out] new_esem_cmd - pointer to the esem command allocated.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  We have 64 esem commands which are marked by 16 different tags per ARR prefix.
 *  There are 4 commands in each tag. Tables can apply for esem command with some
 *  or all tags per the prefix in current entry.
 *  The allocation is trying to allocate an esem command for the applicant in the
 *  tag with least overlaps. The overlaps are calculated by counting the number of
 *  different applications that can use this tag.
 *
 * \see
 *  None
 */
shr_error_e
dnx_port_esem_cmd_algo_exchange(
    int unit,
    uint32 flags,
    uint32 prefix,
    dnx_esem_cmd_data_t * esem_cmd_data,
    dnx_esem_cmd_tag_bmp_range_t esem_cmd_tags,
    int old_esem_cmd,
    int *new_esem_cmd,
    uint8 *is_first,
    uint8 *is_last)
{
    int rv, tmp_esem_cmd;
    uint8 with_id;
    uint32 tag;
    uint32 res_tag_bitmap[DNX_ALGO_ESEM_CMD_NOF_DIFFER_PREFIX] =
        { DNX_ALGO_ESEM_ACC_CMD_RANGE_0_3, DNX_ALGO_ESEM_ACC_CMD_RANGE_0_7,
        DNX_ALGO_ESEM_ACC_CMD_RANGE_0_15, DNX_ALGO_ESEM_ACC_CMD_RANGE_60_63,
        DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63
    };
    int tag_bmp_index, tag_index, nof_avail_tags, avail_tag_index, sorting_tag;
    int nof_overlaps, tag_overlaps[DNX_ALGO_ESEM_CMD_RES_NOF_TAGS] = { 0 };
    int sorted_tags[DNX_ALGO_ESEM_CMD_RES_NOF_TAGS] = { 0 };
    smart_template_alloc_info_t extra_alloc_info;

    int grain_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_esem_cmd_algo_exchange_verify
                          (unit, flags, esem_cmd_data, esem_cmd_tags, new_esem_cmd, is_first, is_last));

    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID);

    /*
     * If the given profile data matches with default profile, no need to allocate it.
     */
    rv = algo_port_pp_db.esem.access_cmd.profile_get(unit, esem_cmd_data, &tmp_esem_cmd);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        if ((tmp_esem_cmd == dnx_data_esem.access_cmd.default_ac_get(unit)) ||
            (tmp_esem_cmd == dnx_data_esem.access_cmd.default_native_get(unit)))
        {
            /** Given profile data matches with a default profile, check if the range matches.*/
            tag = tmp_esem_cmd * DNX_ALGO_ESEM_CMD_RES_NOF_TAGS / dnx_data_esem.access_cmd.nof_cmds_get(unit);
            if ((esem_cmd_tags >> tag) & 0x1)
            {
                *new_esem_cmd = tmp_esem_cmd;
                *is_last = *is_first = FALSE;
                SHR_EXIT();
            }
        }
    }

    /*
     * If the old_esem_cmd is allocated in init, do not exchange it to avoid de-alloc it.
     */
    if ((old_esem_cmd == dnx_data_esem.access_cmd.default_native_get(unit)) ||
        (old_esem_cmd == dnx_data_esem.access_cmd.default_ac_get(unit)))
    {
        tmp_esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);
    }
    else
    {
        tmp_esem_cmd = old_esem_cmd;
    }

    /*
     * Prepare the extra allocation info.
     */
    sal_memset(&extra_alloc_info, 0, sizeof(smart_template_alloc_info_t));
    extra_alloc_info.resource_alloc_info.align = 0;
    extra_alloc_info.resource_flags = RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    grain_size = dnx_data_esem.access_cmd.nof_cmds_get(unit) / DNX_ALGO_ESEM_CMD_RES_NOF_TAGS;

    /** If WITH_ID, try to alloc the given profile*/
    if (with_id)
    {
        tag = *new_esem_cmd * DNX_ALGO_ESEM_CMD_RES_NOF_TAGS / dnx_data_esem.access_cmd.nof_cmds_get(unit);
        extra_alloc_info.resource_alloc_info.range_start = tag * grain_size;
        extra_alloc_info.resource_alloc_info.range_end = tag * grain_size + grain_size;

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange
                        (unit, flags, esem_cmd_data, tmp_esem_cmd, (void *) &extra_alloc_info, new_esem_cmd, is_first,
                         is_last));
    }
    else
    {
        /** Pick the available tags out and Calculate the tag overlaps.*/
        nof_avail_tags = 0;
        for (tag = 0; tag < DNX_ALGO_ESEM_CMD_RES_NOF_TAGS; tag++)
        {
            nof_overlaps = 0;
            if ((esem_cmd_tags >> tag) & 0x1)
            {
                for (tag_bmp_index = 0; tag_bmp_index < DNX_ALGO_ESEM_CMD_NOF_DIFFER_PREFIX; tag_bmp_index++)
                {
                    nof_overlaps += (res_tag_bitmap[tag_bmp_index] >> tag) & 1;
                }

                tag_overlaps[nof_avail_tags] = nof_overlaps;
                sorted_tags[nof_avail_tags] = tag;
                nof_avail_tags++;
            }
        }

        /** Sort the available tags from properest(with least overlaps) to worst.*/
        for (tag = 0; tag < nof_avail_tags; tag++)
        {
            nof_overlaps = tag_overlaps[tag];
            avail_tag_index = tag;
            for (tag_index = tag + 1; tag_index < nof_avail_tags; tag_index++)
            {
                if (nof_overlaps > tag_overlaps[tag_index])
                {
                    nof_overlaps = tag_overlaps[tag_index];
                    avail_tag_index = tag_index;
                }
            }

            tag_overlaps[avail_tag_index] = tag_overlaps[tag];
            sorting_tag = sorted_tags[avail_tag_index];
            sorted_tags[avail_tag_index] = sorted_tags[tag];

            tag_overlaps[tag] = nof_overlaps;
            sorted_tags[tag] = sorting_tag;
        }

        /** If no WITH_ID, try to alloc the profile from the properest tag*/
        for (tag = 0; tag < nof_avail_tags; tag++)
        {
            extra_alloc_info.resource_alloc_info.range_start = prefix | (sorted_tags[tag] * grain_size);
            extra_alloc_info.resource_alloc_info.range_end = prefix | (sorted_tags[tag] * grain_size + grain_size);
            rv = algo_port_pp_db.esem.access_cmd.exchange
                (unit, flags, esem_cmd_data, tmp_esem_cmd, (void *) &extra_alloc_info, new_esem_cmd, is_first, is_last);
            if (rv == _SHR_E_NONE)
            {
                break;
            }
            else if (rv == _SHR_E_FULL)
            {
                /** No available esem cmd in current tag, try to allocate one in next tag.*/
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Initialize ESEM default commands. The default command should be allocated
 *  Before the Initialization.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  This procedure allocate the predefined default ESEM access commands by
 *  dnx-data for outer AC and native AC. If the ESEM access commands are allocated
 *  already, it update the commands by increasing reference count. If the commands
 *  are allocated for the first time, write the command information to hardware.
 *
 *  This procedure should be called to update the default ESEM command when they
 *  are quoted.
 *
 * \see
 *  dnx_pp_port_init
 */
static shr_error_e
dnx_port_esem_cmd_default_init(
    int unit)
{
    uint32 flags;
    uint8 is_first, is_last;
    int esem_cmd_default, esem_cmd_alloc;
    dnx_esem_cmd_data_t esem_cmd_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init the esem cmd for no actions
     */
    flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;

    /** Allocate the cmd ID*/
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    esem_cmd_default = dnx_data_esem.access_cmd.no_action_get(unit);
    esem_cmd_alloc = esem_cmd_default;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, flags, 0, &esem_cmd_data, DNX_ALGO_ESEM_ACC_CMD_RANGE_0_3,
                                                    esem_cmd_default, &esem_cmd_alloc, &is_first, &is_last));

    /** Write to esem-cmd table */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));

    /*
     * Init the esem cmd for ip tunnel no actions
     */
    /** For ip_tunnel_no_action cmd (use a different default_result_profile from no_action cmd.)*/
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    esem_cmd_alloc = dnx_data_esem.access_cmd.ip_tunnel_no_action_get(unit);
    esem_cmd_data.esem[ESEM_ACCESS_IF_2].default_result_profile =
        dnx_data_esem.default_result_profile.default_ac_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, flags, 0, &esem_cmd_data, DNX_ALGO_ESEM_ACC_CMD_RANGE_60_63,
                                                    esem_cmd_default, &esem_cmd_alloc, &is_first, &is_last));

    /** Write to esem-cmd table */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));

    /*
     * Init the esem cmd for default native
     */
    /** Set the esem-cmd info for default native*/
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    esem_cmd_alloc = dnx_data_esem.access_cmd.default_native_get(unit);

    esem_cmd_data.esem[ESEM_ACCESS_IF_2].valid = TRUE;
    esem_cmd_data.esem[ESEM_ACCESS_IF_2].app_db_id = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_OUTLIF;
    esem_cmd_data.esem[ESEM_ACCESS_IF_2].default_result_profile =
        dnx_data_esem.default_result_profile.default_native_get(unit);
    esem_cmd_data.esem[ESEM_ACCESS_IF_2].designated_offset = DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, flags, 0, &esem_cmd_data, DNX_ALGO_ESEM_ACC_CMD_RANGE_0_7,
                                                    esem_cmd_default, &esem_cmd_alloc, &is_first, &is_last));

    /** Write to esem-cmd table */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));

    /*
     * Init the esem cmd for default ac
     */
    /** Set the esem-cmd info for default native*/
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    esem_cmd_alloc = dnx_data_esem.access_cmd.default_ac_get(unit);

    esem_cmd_data.esem[ESEM_ACCESS_IF_1].valid = TRUE;
    esem_cmd_data.esem[ESEM_ACCESS_IF_1].app_db_id = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_NAMESPACE_CTAG;
    esem_cmd_data.esem[ESEM_ACCESS_IF_1].default_result_profile =
        dnx_data_esem.default_result_profile.default_ac_get(unit);
    esem_cmd_data.esem[ESEM_ACCESS_IF_1].designated_offset = DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, flags, 0, &esem_cmd_data, DNX_ALGO_ESEM_ACC_CMD_RANGE_0_63,
                                                    esem_cmd_default, &esem_cmd_alloc, &is_first, &is_last));

    /** Write to esem-cmd table */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Initialize the default esem cmds and profiles for native and outer AC.
 *  See port_esem.h for information.
 */
shr_error_e
dnx_port_esem_default_resource_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate esem-default-result-profile used in case of no match in esem
     */
    /** See dnx_tune_switch_default_ac_init*/

    /*
     * Init default esem-access-cmd
     */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_default_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange an ESEM access command from sw algo.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_exchange(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    uint32 prefix,
    dnx_port_esem_command_accesses_t * esem_accesses,
    int old_esem_cmd,
    int *new_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *is_first,
    uint8 *is_last)
{
    int access_iter;
    dnx_esem_cmd_tag_bmp_range_t esem_cmd_tags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(new_esem_cmd, _SHR_E_INTERNAL, "A valid pointer for new_esem_cmd is needed!\n");
    SHR_NULL_CHECK(is_first, _SHR_E_INTERNAL, "A valid pointer for is_first is needed!\n");
    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed!\n");
    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for receiving esem_cmd_data is needed!\n");

    /*
     * Prepare the cmd data according to inputs and old esem cmd
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, esem_cmd_data, NULL));

        /*
         * Remove all the requested interfaces.
         */
        if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_REMOVE))
        {
            for (access_iter = 0; access_iter < esem_accesses->nof_interfaces_to_remove; access_iter++)
            {
                sal_memset(&(esem_cmd_data->esem[esem_accesses->remove_interfaces[access_iter]]), 0,
                           sizeof(dnx_esem_cmd_if_data_t));
            }
        }
    }
    else
    {
        sal_memset(esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    }

    for (access_iter = 0; access_iter < esem_accesses->nof_accesses; access_iter++)
    {
        /** Collect the right esem cmd information for the allocation. */
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_info_collect
                        (unit, flags, esem_accesses->accesses[access_iter].esem_app_db,
                         esem_accesses->accesses[access_iter].access_type,
                         esem_accesses->accesses[access_iter].esem_default_result_profile, old_esem_cmd,
                         esem_cmd_data));
    }

    /** Get the right esem cmd range tags*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_tags_get(unit, flags, port, &esem_cmd_tags));

    /*
     * Exchange for an esem access cmd
     */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, 0, prefix, esem_cmd_data, esem_cmd_tags, old_esem_cmd,
                                                    new_esem_cmd, is_first, is_last));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Free an ESEM access command from sw algo.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_free(
    int unit,
    int esem_cmd,
    uint8 *is_last)
{
    uint8 free_needed, is_first;
    int tmp_esem_cmd;
    dnx_esem_cmd_data_t esem_cmd_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed\n!");

    /*
     * Check if the cmd is one of the default. If TRUE, no need to free it.
     */
    free_needed = TRUE;
    if ((esem_cmd == dnx_data_esem.access_cmd.default_ac_get(unit)) ||
        (esem_cmd == dnx_data_esem.access_cmd.default_native_get(unit)) ||
        (esem_cmd == dnx_data_esem.access_cmd.no_action_get(unit)))
    {
        free_needed = FALSE;
    }

    /*
     * Free the given esem_cmd by exchanging it to default.
     */
    *is_last = FALSE;
    if (free_needed == TRUE)
    {
        tmp_esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);

        sal_memset(&esem_cmd_data, 0, sizeof(esem_cmd_data));
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, tmp_esem_cmd, &esem_cmd_data, NULL));

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange
                        (unit, 0, &esem_cmd_data, esem_cmd, NULL, &tmp_esem_cmd, &is_first, is_last));
        if (tmp_esem_cmd != dnx_data_esem.access_cmd.no_action_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem_cmd(%d) is not released correctly. Expect exchanging to (%d) but (%d)",
                         esem_cmd, dnx_data_esem.access_cmd.no_action_get(unit), tmp_esem_cmd);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_port_esem_command_add(
    int unit,
    dbal_enum_value_field_esem_app_db_id_e esem_app_db,
    dnx_esem_access_type_t access_type,
    int esem_default_result_profile,
    dnx_port_esem_command_accesses_t * esem_accesses)
{
    SHR_FUNC_INIT_VARS(unit);

    if (esem_accesses->nof_accesses >= DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Up to %d ESEM access commands are supported",
                     DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES);
    }

    esem_accesses->accesses[esem_accesses->nof_accesses].esem_app_db = esem_app_db;
    esem_accesses->accesses[esem_accesses->nof_accesses].access_type = access_type;
    esem_accesses->accesses[esem_accesses->nof_accesses].esem_default_result_profile = esem_default_result_profile;

    esem_accesses->nof_accesses++;
exit:
    SHR_FUNC_EXIT;
}
