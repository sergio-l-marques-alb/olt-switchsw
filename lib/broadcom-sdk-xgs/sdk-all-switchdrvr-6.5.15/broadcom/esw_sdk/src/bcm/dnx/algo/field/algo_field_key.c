/** \file algo_field_key.c
 * $Id$
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for Field module
 * related to the PMF key and FFCs.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/field/ffc_alloc_mngr.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <sal/appl/sal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_access.h>

/*
 * }
 */

/**
* \brief
*  Init simple alloc algorithm for FFC id for PMF1
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_1_ffc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_ffc_ipmf_1_alloc_mngr_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for PMF2
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_2_ffc_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_ffc;
    data.flags = 0;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF_2_FFC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.create(unit, sub_resource_id, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for IPMF3
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ipmf_3_ffc_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_ffc;
    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF_3_FFC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for EPMF
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_epmf_ffc_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_ffc;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_EPMF_FFC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init simple alloc algorithm for FFC id for IFWD2
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_field_res_mngr_ifwd2_ffc_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    data.first_element = 0;
    data.nof_elements = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IFWD2_FFC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_f_occupation_bmp_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_F_OCC_BMP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_f_occ_bmp.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_f_occ_bmp.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_g_occupation_bmp_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_G_OCC_BMP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_g_occ_bmp.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_g_occ_bmp.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_h_occupation_bmp_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_H_OCC_BMP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_h_occ_bmp.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_h_occ_bmp.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_i_occupation_bmp_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_I_OCC_BMP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_i_occ_bmp.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_i_occ_bmp.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_res_mngr_ipmf1_initial_key_j_occupation_bmp_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;
    uint32 sub_resource_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_element = 0;

    /*
     * Key size here represents the 160 bit of data we can pass to ipfm2 as initial data buffer
     */
    data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;

    /** Need to allocate FFC per PMF program hence creating indexed pool */
    data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    data.nof_resource_pool_indexes = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    sal_strncpy(data.name, DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_J_OCC_BMP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_j_occ_bmp.alloc(unit, data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf1_initial_key_j_occ_bmp.create(unit, sub_resource_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_key_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**IPMF_1 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_1_ffc_init(unit));
    /**IPMF_2 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_2_ffc_init(unit));
    /**IPMF_3 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf_3_ffc_init(unit));
    /**EPMF FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_epmf_ffc_init(unit));
    /**IFWD2 FFC */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ifwd2_ffc_init(unit));

    /**IPMF1 INITIAL KEY F OCCUPATION BITMAP FOR FFC ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_f_occupation_bmp_init(unit));

    /**IPMF1 INITIAL KEY G OCCUPATION BITMAP FOR FFC ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_g_occupation_bmp_init(unit));

    /**IPMF1 INITIAL KEY H OCCUPATION BITMAP FOR FFC ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_h_occupation_bmp_init(unit));

    /**IPMF1 INITIAL KEY I OCCUPATION BITMAP FOR FFC ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_i_occupation_bmp_init(unit));

    /**IPMF1 INITIAL KEY J OCCUPATION BITMAP FOR FFC ALLOCATION */
    SHR_IF_ERR_EXIT(dnx_algo_field_res_mngr_ipmf1_initial_key_j_occupation_bmp_init(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_key_res_mngr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * All sw resources will be deinit together.
     */
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
/*
 * Procedures related to allocation of keys.
 * {
 */
/*
 * Verify procedure for dnx_algo_field_key_alloc_generic().
 * See its header for description of parameters.
 * \remark
 */
static shr_error_e
dnx_algo_field_key_alloc_generic_verify(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length,
    dnx_field_key_id_t *key_id_p,
    uint32 *key_allocation_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_allocation_id_p, _SHR_E_PARAM, "key_allocation_id_p");
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    if ((unsigned int) key_length >= DNX_FIELD_KEY_LENGTH_TYPE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length type (%d, larger than max: %d)\r\n", (unsigned int) key_length,
                     DNX_FIELD_KEY_LENGTH_TYPE_NOF - 1);
    }
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    if (flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID)
    {
        if (key_id_p->id[0] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key  (%d) while using with_id flag\r\n",
                         (unsigned int) key_id_p->id[0]);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_algo_field_key_bit_range_alloc_generic().
 * See its header for description of parameters.
 * \remark
 */
static shr_error_e
dnx_algo_field_key_bit_range_alloc_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 key_length,
    uint32 do_align,
    int16 aligned_bit,
    dnx_field_key_id_t *key_id_p,
    uint32 *bit_range_offset_within_key_p)
{
    uint8 supports_bitmap_allocation, availability_odd_only;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(bit_range_offset_within_key_p, _SHR_E_PARAM, "bit_range_offset_within_key_p");
    /*
     * Deduce the number of elements of the configuration array, keys_availability_info
     * (in sw state), directly from its definition, as created by sw state.
     */
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    if (key_length < DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length size (%d, smaller than minimum: %d)\r\n",
                     (unsigned int) key_length, DNX_FIELD_KEY_BIT_RANGE_MIN_LENGTH);
    }
    if ((unsigned int) key_length > (unsigned int) DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key length size (%d, larger than max: %d)\r\n", (unsigned int) key_length,
                     DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE);
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.supports_bitmap_allocation.get(unit, field_stage, fg_type,
                                                                                &supports_bitmap_allocation));
    if (!supports_bitmap_allocation)
    {
        /**Provide an error message specifically for the case of direct extraction*/
        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Direct extraction (%s) is not supported by 'field stage' (%s), "
                         "unless no bits are used on the key.\r\n",
                         dnx_field_group_type_e_get_name(fg_type), dnx_field_stage_e_get_name(field_stage));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "This combination of 'field stage' (%s) and 'field group type' (%s)\r\n"
                     "does not support 'bit-range' option. Quit.\r\n",
                     dnx_field_stage_e_get_name(field_stage), dnx_field_group_type_e_get_name(fg_type));
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.availability_odd_only.get(unit, field_stage, fg_type,
                                                                           &availability_odd_only));
    if (availability_odd_only)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "This combination of 'field stage' (%s) and 'field group type' (%s) supports\r\n"
                     "==> 'bit-range' option and may not be set to 'availability_odd_only' option. Quit.\r\n",
                     dnx_field_stage_e_get_name(field_stage), dnx_field_group_type_e_get_name(fg_type));
    }
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    if (do_align)
    {
        /*
         * Enter if alignment is required for some bit on the bit-range (so the
         * 'key select' mechanism can make it the first in a 32-bits 'chunk')
         */
        if (((int) aligned_bit < (int) DNX_FIELD_KEY_MIN_VALUE_FOR_ALIGNED_BIT)
            || ((int) aligned_bit > (int) DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Alignment is required but 'aligned_bit' (%d) is out of range (min %d, max %d)\r\n",
                         (int) aligned_bit, (int) DNX_FIELD_KEY_MIN_VALUE_FOR_ALIGNED_BIT,
                         (int) DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given 'field group type' and 'field stage', check whether this pair supports
 *   bit-range allocation.
 *   If it does, use 'field stage', 'context id' and 'half key' to update reqource
 *   management on swstate: Mark all 80 bits of this 'half key' as occupied.
 * \param [in] unit     - Device ID
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in] field_stage -
 *   dnx_field_stage_e. Type of stage requiring the key.
 * \param [in] context_id -
 *   Identifier of the Context ID on which key is to be allocated.
 * \param [in] half_key -
 *   Identifier of the 'half key' with a key, for which allocation was required.
 *   'Half key' numbering is done so that even indices are the MS part of
 *   the key and odd indices indicate the LS part. Overall, keys marking is done
 *   on a portion of the maximal available: 'A' to 'J'. The first key is assigned
 *   the value of 'zero' so, on stages which support only some of the keys
 *   (say, 'F' to 'J'), the 'half key' zero will indicate the MS part of key 'F'.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   dnx_algo_field_key_alloc_generic
 *   dnx_field_key_alloc_instruction_t
 *   dnx_field_key_alloc_detail_t
 *   DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF
 */
static shr_error_e
dnx_algo_field_key_capture_half_for_bit_range(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 half_key)
{
    uint8 supports_bitmap_allocation;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    uint8 key_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    key_index = first_available_key_on_stage + (half_key / DNX_FIELD_KEY_NUM_HALVES_IN_KEY);
    SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.get
                    (unit, field_stage, key_index, &supports_bitmap_allocation));
    if (supports_bitmap_allocation)
    {
        /*
         * 'bit-range' feature is enabled.
         */
        uint32 flags;
        int element;
        uint8 half_key_for_bit_range_resource;
        /*
         * Specify we have one bit range here (of 80 bits, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
         * and that this half key is occupied also on the 'bit-range' administration.
         */
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, half_key, 1));
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.is_half_key_range_occupied.set(unit, field_stage, context_id, half_key, TRUE));
        /*
         * Specify that the number of bits assigned, starting from bit '0'
         * of this half-key, is '80'.
         */
        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.set
                        (unit, field_stage, context_id, half_key, 0, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF));
        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.fg_type.set
                        (unit, field_stage, context_id, half_key, 0, (uint8) fg_type));
        flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
        if (half_key % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
        {
            /*
             * This is the LS part of the key ('half_key' is odd)
             * Note that the resource is allocated on odd half-keys (for
             * both halves).
             */
            element = 0;
            half_key_for_bit_range_resource = half_key;
        }
        else
        {
            /*
             * This is the MS part of the key ('half_key' is even)
             * Note that the resource is allocated on odd half-keys (for
             * both halves).
             */
            element = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
            half_key_for_bit_range_resource = half_key + 1;
        }
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.allocate_several
                        (unit, field_stage, context_id, half_key_for_bit_range_resource,
                         0, flags, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF, NULL, &element));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given 'field group type' and 'field stage', check whether this pair supports
 *   bit-range allocation.
 *   If it does, use 'field stage', 'context id' and 'half key' to update reqource
 *   management on swstate: Mark all 80 bits of this 'half key' as free.
 * \param [in] unit     - Device ID
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in] field_stage -
 *   dnx_field_stage_e. Type of stage requiring the key.
 * \param [in] context_id -
 *   Identifier of the Context ID on which key is to be allocated.
 * \param [in] half_key -
 *   Identifier of the 'half key' with a key, for which allocation was required.
 *   'Half key' numbering is done so that even indices are the MS part of
 *   the key and odd indices indicate the LS part. Overall, keys marking is done
 *   on a portion of the maximal available: 'A' to 'J'. The first key is assigned
 *   the value of 'zero' so, on stages which support only some of the keys
 *   (say, 'F' to 'J'), the 'half key' zero will indicate the MS part of key 'F'.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   dnx_algo_field_key_alloc_generic
 *   dnx_field_key_alloc_instruction_t
 *   dnx_field_key_alloc_detail_t
 *   DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF
 */
static shr_error_e
dnx_algo_field_key_free_half_for_bit_range(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 half_key)
{
    uint8 supports_bitmap_allocation;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    uint8 key_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    key_index = first_available_key_on_stage + (half_key / DNX_FIELD_KEY_NUM_HALVES_IN_KEY);
    SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.get
                    (unit, field_stage, key_index, &supports_bitmap_allocation));
    if (supports_bitmap_allocation)
    {
        /*
         * 'bit-range' feature is enabled.
         */
        uint8 num_bit_ranges, is_half_key_range_occupied, bit_range_size;
        int element;
        uint8 half_key_for_bit_range_resource;
        /*
         * Verify we have one bit range here (of 80 bits, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
         * and that this half key is occupied also on the 'bit-range' administration.
         * Set to '0' bit-ranges and mark as 'not occupied'.
         */
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.get(unit, field_stage, context_id, half_key, &num_bit_ranges));
        if (num_bit_ranges != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Trying to release a half-key on bit-range administration but half key is not marked\r\n"
                         "as having only one bit-range (of 80 bits). Perhaps caller is trying to release a half-key\r\n"
                         "which has not been allocated. field_stage %s, fg_type %s, context_id %d, half_key %d\r\n",
                         dnx_field_stage_e_get_name(field_stage),
                         dnx_field_group_type_e_get_name(fg_type), (int) context_id, (int) half_key);
        }
        num_bit_ranges = 0;
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, half_key, num_bit_ranges));
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.is_half_key_range_occupied.get
                        (unit, field_stage, context_id, half_key, &is_half_key_range_occupied));
        if (is_half_key_range_occupied != TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Trying to release a half-key on bit-range administration but half key is not marked\r\n"
                         "as 'occupied'. Perhaps caller is trying to release a half-key\r\n"
                         "which has not been allocated. field_stage %s, fg_type %s, context_id %d, half_key %d\r\n",
                         dnx_field_stage_e_get_name(field_stage),
                         dnx_field_group_type_e_get_name(fg_type), (int) context_id, (int) half_key);
        }
        is_half_key_range_occupied = FALSE;
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.is_half_key_range_occupied.set
                        (unit, field_stage, context_id, half_key, is_half_key_range_occupied));
        /*
         * Verify that the number of bits assigned, starting from bit '0'
         * of this half-key, is '80'.
         * Set to '0'.
         */
        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.
                        bit_range_size.get(unit, field_stage, context_id, half_key, 0, &bit_range_size));
        if (bit_range_size != DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Trying to release a half-key on bit-range administration but half key is not marked\r\n"
                         "as having a bit-range of size '80' on bit '0'. Perhaps caller is trying to release a half-key\r\n"
                         "which has not been allocated.\r\n"
                         "field_stage %s, fg_type %s, context_id %d, half_key %d bit_range_size %d\r\n",
                         dnx_field_stage_e_get_name(field_stage),
                         dnx_field_group_type_e_get_name(fg_type),
                         (int) context_id, (int) half_key, (int) bit_range_size);
        }
        bit_range_size = 0;
        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.
                        bit_range_size.set(unit, field_stage, context_id, half_key, 0, bit_range_size));
        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.
                        fg_type.set(unit, field_stage, context_id, half_key, 0, (uint8) fg_type));
        if (half_key % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
        {
            /*
             * This is the LS part of the key ('half_key' is odd)
             * Note that the resource is allocated on odd half-keys (for
             * both halves).
             */
            element = 0;
            half_key_for_bit_range_resource = half_key;
        }
        else
        {
            /*
             * This is the MS part of the key ('half_key' is even)
             * Note that the resource is allocated on odd half-keys (for
             * both halves).
             */
            element = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
            half_key_for_bit_range_resource = half_key + 1;
        }
        SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.free_several
                        (unit, field_stage, context_id, half_key_for_bit_range_resource,
                         0, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF, element));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Try to allocate a 'KEY' (or half key) given 'field group type', 'PMF
 *   program' (context), 'field stage' and required size.
 *   NOTE that failure to find a free key is not considered an error as
 *   far as 'error output' of this procedure is concerned. It is up to
 *   the caller to decide what to do about that. A error log print, however,
 *   is forced.
 *   See remarks.
 * \param [in] unit     - Device ID
 * \param [in] flags     - please look at dnx_algo_field_key_flags_e
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in] field_stage -
 *   dnx_field_stage_e. Type of stage requiring the key.
 * \param [in] context_id -
 *   Identifier of the Context ID on which key is to be allocated.
 * \param [in] key_length -
 *   dnx_field_key_length_type_e. Size of required key. May be:
 *   * DNX_FIELD_KEY_LENGTH_TYPE_HALF   ( 80 bits)
 *   * DNX_FIELD_KEY_LENGTH_TYPE_SINGLE (160 bits)
 *   * DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE (320 bits)
 * \param [out]  failed_to_find_p -
 *   dnx_field_key_failed_to_find_e. This procedure loads pointed memory by \n
 *   'DNX_FIELD_KEY_FAILED_NO_FAILURE' if a free key has been found and     \n
 *   successfully allocated. \n
 *   Otherwise, the reason for not finding a free key is as per             \n
 *   'dnx_field_key_failed_to_find_e'.
 * \param [in,out]  key_id_p -
 *   structure to hold an array of dbal_enum_value_field_field_key_e.       \n
 *   It contains as single keys as per 'key_length'                         \n
 *   (Currently, 'double' key is the largest)                               \n
 *   Currently, the largest number of keys indicated this way is                      \n
 *   DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE                                                 \n
 *   This procedure loads this structure with allocated single keys                   \n
 *   (of type dbal_enum_value_field_field_key_e).                                     \n
 *   Only meaningful if '*failed_to_find_p' is DNX_FIELD_KEY_FAILED_NO_FAILURE!
 *   - key_part from the key_id_p                                                      \n
 *     Represents the part (MSB or LSB) of the single key which has been allocated.    \n
 *     Only meaningful if 'key_length' is DNX_FIELD_KEY_LENGTH_TYPE_HALF and provided  \n
 *     '*failed_to_find_p' is DNX_FIELD_KEY_FAILED_NO_FAILURE!                         \n
 *   Important: Note that, unlike key_id_p->id[], key_part is NOT an array and only a  \n
 *   single value is returned.
 * \param [out]  key_allocation_id_p -
 *   uint32 *. This procedure loads pointed memory by the identifier of the     \n
 *   allocation details of the key (= handle). Caller may use this value for  \n
 *   fast access to allocation details (typically for 'delete/free' operation.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The whole allocation procedure is carried out within SW STATE.
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored.
 *   * See 'key_occupation' for full allocation state
 * \see
 *   dnx_field_keys_per_stage_allocation_t
 *   dnx_field_key_alloc_detail_t
 */
static shr_error_e
dnx_algo_field_key_alloc_generic(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length,
    dnx_field_key_failed_to_find_e *failed_to_find_p,
    dnx_field_key_id_t *key_id_p,
    uint32 *key_allocation_id_p)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_available_keys_on_stage;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    /*
     * First key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key;
    /*
     * Last key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 last_available_key;

    /**key id that currently being examined for potential allocation*/
    dbal_enum_value_field_field_key_e key_id_iter = DBAL_NOF_ENUM_FIELD_KEY_VALUES;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(failed_to_find_p, _SHR_E_PARAM, "failed_to_find_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_allocation_id_p, _SHR_E_PARAM, "key_allocation_id_p");

    /*
     * Initializations.
     */
    *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_PLACE;

    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_alloc_generic_verify
                          (unit, flags, fg_type, field_stage, context_id, key_length, key_id_p, key_allocation_id_p));

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input was not allocated any free keys.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NOT_INITIALIZED;
        SHR_ERR_EXIT(_SHR_E_NONE, "Uninitialized stage (%d, %s): No free keys have been assigned\r\n",
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * First, get the configuration structure (for stage and FG type).
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NOT_ASSIGNED_TO_FG;
        SHR_ERR_EXIT(_SHR_E_NONE, "Uninitialized field group type (%d, %s) under assigned stage (%d, %s)\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_FG_HAS_ZERO_KEYS;
        SHR_ERR_EXIT(_SHR_E_NONE,
                     "Initialized field group type (%d, %s) under assigned stage (%d, %s) was assigned zero keys\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.availability_going_up))
    {
        /*
         * We still do not support 'searching downwards'
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Initialized field group (%d, %s) under assigned stage (%d, %s) was assigned 'search downwards.\r\n"
                     "This is not supported yet.\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * Set range for available keys on this (stage,fg_type) combination.
     */
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    /*
     * Now search forward to get a free key.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Search for key which is not occupied: Procedure %s, line %d %s%s:\r\n",
                 __FUNCTION__, __LINE__, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> fg_type %d, field_stage %d context_id %d key_length %d\r\n",
                 fg_type, field_stage, context_id, key_length);
    {
        uint8 nof_half_keys, half_key, half_key_other, step_half_key, first_half_key, current_half;
        dnx_field_key_alloc_detail_t dnx_field_key_alloc_detail[DNX_FIELD_KEY_MAX_HALVES_IN_KEY];
        uint32 key_allocation_id;

        SHR_IF_ERR_EXIT(KEY_STAGE_PROG_OCCUPATION.
                        key_occupation_nof.get(unit, field_stage, context_id, &nof_half_keys));
        /*
         * Set 'first_half_key' as per first key allowed for this field group type.
         * Since we assign 'half key structures' per stage, we calculate the
         * offest ('first_half_key') from the beginning of allocated memory.
         */
        if (dnx_field_key_alloc_instruction.availability_odd_only)
        {
            /*
             * For 'odd_only, we go through 1,3,5,...
             */
            step_half_key = DNX_FIELD_KEY_NUM_HALVES_IN_KEY;
            first_half_key =
                ((first_available_key - first_available_key_on_stage) *
                 DNX_FIELD_KEY_NUM_HALVES_IN_KEY) + (DNX_FIELD_KEY_NUM_HALVES_IN_KEY - 1);
        }
        else
        {
            /*
             * For non-'odd_only, we go through 0,1,2,...
             */
            step_half_key = 1;
            first_half_key = ((first_available_key - first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY);
        }
        /*
         * For 'double key' the step is of the size of a double key. So, for instance,
         * it is possible to get keys A,B and C,D but not B,C
         */
        if (key_length == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
        {
            /*
             * For double key, we go through either 1,5,... or 0,4,...
             */
            step_half_key = DNX_FIELD_KEY_NUM_HALVES_IN_KEY * DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE;
        }
        for (half_key = first_half_key; half_key < nof_half_keys; half_key += step_half_key)
        {

            if (flags & DNX_ALGO_FIELD_KEY_FLAG_WITH_ID)
            {
                /*
                 * In case of WITH_ID we would like to check if the current evaluated key is the same as requested key
                 */
                key_id_iter = (dbal_enum_value_field_field_key_e) (first_available_key_on_stage +
                                                                   (half_key / DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                if (key_id_iter < key_id_p->id[0])
                {
                    continue;
                }
                if (key_id_iter > key_id_p->id[0])
                {
                    SHR_ERR_EXIT(_SHR_E_NONE,
                                 "Requested Key (%d) is taken and could not be allocated for (fg_type,stage)=(%d,%d)\r\n",
                                 key_id_iter, (unsigned int) fg_type, (unsigned int) field_stage);
                }
            }
            /*
             * 'current_half' is index into dnx_field_key_alloc_detail[] indicating
             * the various 'halves' (up to 4) on required key.
             */
            current_half = 0;
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                            (unit, field_stage, context_id, half_key,
                             &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
            if (dnx_field_key_alloc_detail[current_half].key_occupation_state == DNX_FIELD_KEY_OCCUPY_NONE)
            {
                /*
                 * A free half key was found. If only half key was required then we are done (DT is considered as half-key).
                 */
                if (key_length == DNX_FIELD_KEY_LENGTH_TYPE_HALF || key_length == DNX_FIELD_KEY_LENGTH_TYPE_DT)
                {
                    /*
                     * Only half key was required so we are done.
                     */
                    /*
                     * Identify the half key that is to be allocated.
                     * Note that, for 'odd_only', we have allocated the LSB (because of the
                     * (first_half_key,step_half_key) restriction above).
                     */
                    key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_LSB_HALF;
                    dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_LSB_HALF;
                    if (!(dnx_field_key_alloc_instruction.availability_odd_only))
                    {
                        /*
                         * For non-'odd_only', LSB/MSB depends on 'half_key':
                         * Odd values are LSB and even values are MSB.
                         */
                        if ((half_key % DNX_FIELD_KEY_NUM_HALVES_IN_KEY) == 0)
                        {
                            /*
                             * Enter for even values.
                             */
                            key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_MSB_HALF;
                            dnx_field_key_alloc_detail[current_half].key_occupation_state =
                                DNX_FIELD_KEY_OCCUPY_MSB_HALF;
                        }
                    }
                    dnx_field_key_alloc_detail[current_half].key_field_group_type = fg_type;
                    key_id_p->id[0] =
                        (dbal_enum_value_field_field_key_e) (first_available_key_on_stage +
                                                             (half_key / DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                    if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
                    {
                        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_PLACE;
                        SHR_ERR_EXIT(_SHR_E_NONE,
                                     "No free key within allowed range (from %d to %d) for (fg_type,stage)=(%d,%d). No key was assigned.\r\n",
                                     (unsigned int) first_available_key, (unsigned int) last_available_key,
                                     (unsigned int) fg_type, (unsigned int) field_stage);
                    }
                    CONSTRUCT_KEY_ALLOCATION_ID(key_allocation_id, fg_type, field_stage, context_id, key_id_p->id[0],
                                                key_id_p->id[1], key_id_p->key_part);
                    dnx_field_key_alloc_detail[current_half].key_allocation_id = key_allocation_id;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                    (unit, field_stage, context_id, half_key,
                                     (dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                    (unit, field_stage, context_id, half_key,
                                     (dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                    (unit, field_stage, context_id, half_key,
                                     (dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                    /*
                     * 'bit-range' section.
                     * {
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                    (unit, fg_type, field_stage, context_id, half_key));
                    /*
                     * }
                     */
                    *key_allocation_id_p = key_allocation_id;
                    *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_FAILURE;
                    break;
                }
                else if (key_length == DNX_FIELD_KEY_LENGTH_TYPE_SINGLE)
                {
                    /*
                     * A full single key was required.
                     * If the other half is free then we are done.
                     * The other half index depends on 'odd_only'. Also, for non-'odd_only',
                     * the 'half_key' must be even (since the whole allocation must be from a
                     * single key).
                     * Load 'half_key_other' by the index, in KEY_OCCUPATION, of the 'other' half.
                     */
                    if (dnx_field_key_alloc_instruction.availability_odd_only)
                    {
                        half_key_other = half_key - 1;
                    }
                    else
                    {
                        if (half_key % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
                        {
                            /*
                             * 'half_key' is odd. This means that the 'even' index of this
                             * single key is already occupied so we can not allocate full key under
                             * these conditions. Keep searching.
                             * Example:
                             *   dnx_field_key_alloc_detail[0] is occupied and we got to dnx_field_key_alloc_detail[1]
                             *   so a full single key may not be constructed.
                             */
                            continue;
                        }
                        half_key_other = half_key + 1;
                    }
                    current_half++;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                    (unit, field_stage, context_id, half_key_other,
                                     &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));

                    if (dnx_field_key_alloc_detail[current_half].key_occupation_state == DNX_FIELD_KEY_OCCUPY_NONE)
                    {
                        /*
                         * The other half is free as well. We are done.
                         */
                        if (dnx_field_key_alloc_instruction.availability_odd_only)
                        {
                            dnx_field_key_alloc_detail[current_half - 1].key_occupation_state =
                                DNX_FIELD_KEY_OCCUPY_SINGLE_LSB;
                            dnx_field_key_alloc_detail[current_half].key_occupation_state =
                                DNX_FIELD_KEY_OCCUPY_SINGLE_MSB;
                        }
                        else
                        {
                            dnx_field_key_alloc_detail[current_half - 1].key_occupation_state =
                                DNX_FIELD_KEY_OCCUPY_SINGLE_MSB;
                            dnx_field_key_alloc_detail[current_half].key_occupation_state =
                                DNX_FIELD_KEY_OCCUPY_SINGLE_LSB;
                        }

                        dnx_field_key_alloc_detail[current_half - 1].key_field_group_type = fg_type;
                        dnx_field_key_alloc_detail[current_half].key_field_group_type = fg_type;
                        /*
                         * Identify the key that has been used. Note that we have allocated both LSB and MSB.
                         */
                        key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_INVALID;
                        key_id_p->id[0] =
                            (dbal_enum_value_field_field_key_e)
                            (first_available_key_on_stage + (half_key / DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                        if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
                        {
                            *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_PLACE;
                            SHR_ERR_EXIT(_SHR_E_NONE,
                                         "No free key within allowed range (from %d to %d) for (fg_type,stage) (%d,%d)\r\n",
                                         (unsigned int) first_available_key, (unsigned int) last_available_key,
                                         (unsigned int) fg_type, (unsigned int) field_stage);
                        }
                        CONSTRUCT_KEY_ALLOCATION_ID(key_allocation_id, fg_type, field_stage, context_id,
                                                    key_id_p->id[0], key_id_p->id[1], key_id_p->key_part);
                        dnx_field_key_alloc_detail[current_half - 1].key_allocation_id = key_allocation_id;
                        dnx_field_key_alloc_detail[current_half].key_allocation_id = key_allocation_id;
                        /*
                         * Note that for 'availability_odd_only' then
                         *   'half_key'       is the index of the LSB
                         *   'half_key_other' is the index of the MSB
                         * Note that for non-'availability_odd_only' then
                         *   'half_key'       is the index of the MSB
                         *   'half_key_other' is the index of the LSB
                         * AND
                         * Note that for 'availability_odd_only' then
                         *   'current_half - 1'  is the index of the LSB
                         *   'current_half'      is the index of the MSB
                         * Note that for non-'availability_odd_only' then
                         *   'current_half - 1'  is the index of the MSB
                         *   'current_half'      is the index of the LSB
                         *
                         * So we correlate:
                         *   'half_key'       <--> 'current_half - 1'
                         *   'half_key_other' <--> 'current_half'
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                        (unit, field_stage, context_id, half_key,
                                         (dnx_field_key_alloc_detail[current_half - 1].key_occupation_state)));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                        (unit, field_stage, context_id, half_key,
                                         (dnx_field_key_alloc_detail[current_half - 1].key_field_group_type)));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                        (unit, field_stage, context_id, half_key,
                                         (dnx_field_key_alloc_detail[current_half - 1].key_allocation_id)));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                        (unit, field_stage, context_id, half_key_other,
                                         (dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                        (unit, field_stage, context_id, half_key_other,
                                         (dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                        (unit, field_stage, context_id, half_key_other,
                                         (dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                        /*
                         * 'bit-range' section.
                         * {
                         */
                        SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                        (unit, fg_type, field_stage, context_id, half_key));
                        SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                        (unit, fg_type, field_stage, context_id, half_key_other));
                        /*
                         * }
                         */
                        *key_allocation_id_p = key_allocation_id;
                        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_FAILURE;
                        break;
                    }
                    /*
                     * Reach here if no 'other' half was found. Keep cycling to search for
                     * a full single key (two halves)
                     */
                }
                else if (key_length == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
                {
                    uint8 half_key_dbl[DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE];
                    uint8 half_key_other_dbl[DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE];
                    uint8 ii;
                    /*
                     * A double (two full single) key was required.
                     * If the other half is free then we can go ahead.
                     * The other half index depends on 'odd_only'. Also, for non-'odd_only',
                     * the 'half_key' must be even (since the whole allocation must be from a
                     * single key).
                     * Load 'half_key_other' by the index, in KEY_OCCUPATION, of the 'other' half.
                     */
                    if (dnx_field_key_alloc_instruction.availability_odd_only)
                    {
                        half_key_other = half_key - 1;
                        /*
                         * Example: 'half_key' is '1' and 'half_key_other' is '0'
                         */
                    }
                    else
                    {
                        if (half_key % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
                        {
                            /*
                             * 'half_key' is odd. This means that the 'even' index of this
                             * single key is already occupied so we can not allocate full key under
                             * these conditions. Keep searching.
                             * Example:
                             *   dnx_field_key_alloc_detail[0] is occupied and we got to dnx_field_key_alloc_detail[1]
                             *   so a full single key may not be constructed.
                             */
                            continue;
                        }
                        half_key_other = half_key + 1;
                        /*
                         * Example: 'half_key' is '0' and 'half_key_other' is '1'
                         */
                    }
                    current_half++;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                    (unit, field_stage, context_id, half_key_other,
                                     &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                    if (dnx_field_key_alloc_detail[current_half].key_occupation_state == DNX_FIELD_KEY_OCCUPY_NONE)
                    {
                        /*
                         * The other half is free as well. We are done for the first single (out of 2).
                         * Now verify that the next pair of half keys (if not out of range) is not occupied.
                         */
                        /*
                         * Load half_key_dbl[ii] and half_key_other_dbl[ii] with indices of the two half keys for
                         * the two single keys.
                         */
                        for (ii = 0; ii < DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE; ii++)
                        {
                            half_key_dbl[ii] = half_key + (DNX_FIELD_KEY_NUM_HALVES_IN_KEY * ii);
                            half_key_other_dbl[ii] = half_key_other + (DNX_FIELD_KEY_NUM_HALVES_IN_KEY * ii);
                        }
                        if ((half_key_dbl[DNX_FIELD_KEY_NUM_HALVES_IN_KEY - 1] < nof_half_keys) &&
                            (half_key_other_dbl[DNX_FIELD_KEY_NUM_HALVES_IN_KEY - 1] < nof_half_keys))
                        {
                            /*
                             * Enter if the half keys on the potential second single are within range.
                             */
                            ii = DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE - 1;
                            current_half++;
                            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                            (unit, field_stage, context_id, half_key_dbl[ii],
                                             &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                            /*
                             * Verify that the two half keys, on the second single, are not already allocated.
                             */
                            if (dnx_field_key_alloc_detail[current_half].key_occupation_state ==
                                DNX_FIELD_KEY_OCCUPY_NONE)
                            {
                                current_half++;
                                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                                (unit, field_stage, context_id, half_key_other_dbl[ii],
                                                 &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                                if (dnx_field_key_alloc_detail[current_half].key_occupation_state ==
                                    DNX_FIELD_KEY_OCCUPY_NONE)
                                {
                                    if (dnx_field_key_alloc_instruction.availability_odd_only)
                                    {
                                        dnx_field_key_alloc_detail[current_half - 1 -
                                                                   DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state
                                            = DNX_FIELD_KEY_OCCUPY_DOUBLE_SECOND_HALF;
                                        dnx_field_key_alloc_detail[current_half -
                                                                   DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state
                                            = DNX_FIELD_KEY_OCCUPY_DOUBLE_FIRST_HALF;
                                        dnx_field_key_alloc_detail[current_half - 1].key_occupation_state =
                                            DNX_FIELD_KEY_OCCUPY_DOUBLE_FOURTH_HALF;
                                        dnx_field_key_alloc_detail[current_half].key_occupation_state =
                                            DNX_FIELD_KEY_OCCUPY_DOUBLE_THIRD_HALF;
                                    }
                                    else
                                    {
                                        dnx_field_key_alloc_detail[current_half - 1 -
                                                                   DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state
                                            = DNX_FIELD_KEY_OCCUPY_DOUBLE_FIRST_HALF;
                                        dnx_field_key_alloc_detail[current_half -
                                                                   DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state
                                            = DNX_FIELD_KEY_OCCUPY_DOUBLE_SECOND_HALF;
                                        dnx_field_key_alloc_detail[current_half - 1].key_occupation_state =
                                            DNX_FIELD_KEY_OCCUPY_DOUBLE_THIRD_HALF;
                                        dnx_field_key_alloc_detail[current_half].key_occupation_state =
                                            DNX_FIELD_KEY_OCCUPY_DOUBLE_FOURTH_HALF;
                                    }
                                    for (ii = 0; ii <= current_half; ii++)
                                    {
                                        dnx_field_key_alloc_detail[ii].key_field_group_type = fg_type;
                                    }
                                    /*
                                     * Identify the key that has been used. Note that we have allocated both LSB and MSB.
                                     */
                                    key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_INVALID;
                                    /*
                                     * For a double key, return an array of two single keys. For now, they are consequent.
                                     */
                                    for (ii = 0; ii < DNX_FIELD_KEY_NUM_KEYS_IN_DOUBLE; ii++)
                                    {
                                        key_id_p->id[ii] =
                                            (dbal_enum_value_field_field_key_e) (first_available_key_on_stage +
                                                                                 ((half_key +
                                                                                   (DNX_FIELD_KEY_NUM_HALVES_IN_KEY *
                                                                                    ii)) /
                                                                                  DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                                    }
                                    if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) (last_available_key - 1))
                                    {
                                        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_PLACE;
                                        SHR_ERR_EXIT(_SHR_E_NONE,
                                                     "No free key within allowed range (from %d to %d) for (fg_type,stage) (%d,%d)\r\n",
                                                     (unsigned int) first_available_key,
                                                     (unsigned int) last_available_key - 1, (unsigned int) fg_type,
                                                     (unsigned int) field_stage);
                                    }
                                    CONSTRUCT_KEY_ALLOCATION_ID(key_allocation_id, fg_type, field_stage, context_id,
                                                                key_id_p->id[0], key_id_p->id[1], key_id_p->key_part);
                                    for (ii = 0; ii <= current_half; ii++)
                                    {
                                        dnx_field_key_alloc_detail[ii].key_allocation_id = key_allocation_id;
                                    }
                                    /*
                                     * Note that for 'availability_odd_only' then
                                     *   'half_key'       is the index of the first LSB
                                     *   'half_key_other' is the index of the first MSB
                                     * Note that for non-'availability_odd_only' then
                                     *   'half_key'       is the index of the first MSB
                                     *   'half_key_other' is the index of the first LSB
                                     * AND
                                     * Note that for 'availability_odd_only' then
                                     *   'current_half - 1 - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'  is the index of the first LSB
                                     *   'current_half - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'      is the index of the first MSB
                                     * Note that for non-'availability_odd_only' then
                                     *   'current_half - 1 - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'  is the index of the first MSB
                                     *   'current_half - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'      is the index of the first LSB
                                     *
                                     * So we correlate:
                                     *   'half_key_dbl[0]'       <--> 'current_half - 1 - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'
                                     *   'half_key_other_dbl[0]' <--> 'current_half - DNX_FIELD_KEY_NUM_HALVES_IN_KEY'
                                     *   'half_key_dbl[1]'       <--> 'current_half - 1'
                                     *   'half_key_other_dbl[1]' <--> 'current_half'
                                     */
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                                    (unit, field_stage, context_id, half_key_dbl[0],
                                                     (dnx_field_key_alloc_detail
                                                      [current_half - 1 -
                                                       DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                                    (unit, field_stage, context_id, half_key_dbl[0],
                                                     (dnx_field_key_alloc_detail
                                                      [current_half - 1 -
                                                       DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_field_group_type)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                                    (unit, field_stage, context_id, half_key_dbl[0],
                                                     (dnx_field_key_alloc_detail
                                                      [current_half - 1 -
                                                       DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_allocation_id)));

                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                                    (unit, field_stage, context_id, half_key_other_dbl[0],
                                                     (dnx_field_key_alloc_detail
                                                      [current_half -
                                                       DNX_FIELD_KEY_NUM_HALVES_IN_KEY].key_occupation_state)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_field_group_type.set(unit, field_stage, context_id,
                                                                             half_key_other_dbl[0],
                                                                             (dnx_field_key_alloc_detail
                                                                              [current_half -
                                                                               DNX_FIELD_KEY_NUM_HALVES_IN_KEY].
                                                                              key_field_group_type)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_allocation_id.set(unit, field_stage, context_id,
                                                                          half_key_other_dbl[0],
                                                                          (dnx_field_key_alloc_detail
                                                                           [current_half -
                                                                            DNX_FIELD_KEY_NUM_HALVES_IN_KEY].
                                                                           key_allocation_id)));

                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_occupation_state.set(unit, field_stage, context_id,
                                                                             half_key_dbl[1],
                                                                             (dnx_field_key_alloc_detail
                                                                              [current_half -
                                                                               1].key_occupation_state)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_field_group_type.set(unit, field_stage, context_id,
                                                                             half_key_dbl[1],
                                                                             (dnx_field_key_alloc_detail
                                                                              [current_half -
                                                                               1].key_field_group_type)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_allocation_id.set(unit, field_stage, context_id,
                                                                          half_key_dbl[1],
                                                                          (dnx_field_key_alloc_detail
                                                                           [current_half - 1].key_allocation_id)));

                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_occupation_state.set(unit, field_stage, context_id,
                                                                             half_key_other_dbl[1],
                                                                             (dnx_field_key_alloc_detail
                                                                              [current_half].key_occupation_state)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_field_group_type.set(unit, field_stage, context_id,
                                                                             half_key_other_dbl[1],
                                                                             (dnx_field_key_alloc_detail
                                                                              [current_half].key_field_group_type)));
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                                    key_allocation_id.set(unit, field_stage, context_id,
                                                                          half_key_other_dbl[1],
                                                                          (dnx_field_key_alloc_detail[current_half].
                                                                           key_allocation_id)));
                                    /*
                                     * 'bit-range' section.
                                     * {
                                     */
                                    SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                                    (unit, fg_type, field_stage, context_id, half_key_dbl[0]));
                                    SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                                    (unit, fg_type, field_stage, context_id, half_key_other_dbl[0]));
                                    SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                                    (unit, fg_type, field_stage, context_id, half_key_dbl[1]));
                                    SHR_IF_ERR_EXIT(dnx_algo_field_key_capture_half_for_bit_range
                                                    (unit, fg_type, field_stage, context_id, half_key_other_dbl[1]));
                                    /*
                                     * }
                                     */
                                    *key_allocation_id_p = key_allocation_id;
                                    *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_FAILURE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            /*
                             * Reach here if we have overflown the number of available half keys
                             * while searching for half key. There is no point going on.
                             */
                            break;
                        }
                    }
                    /*
                     * Reach here if no 'other' half was found. Keep cycling to search for
                     * a double key (two full singles)
                     */
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                                 "This option (key_length = %d) is not supported. Acceptable values are: %d (half), %d (single), %d (double)\r\n",
                                 key_length, DNX_FIELD_KEY_LENGTH_TYPE_HALF, DNX_FIELD_KEY_LENGTH_TYPE_SINGLE,
                                 DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Try to allocate a range of bits on a 'KEY' (or half key) given
 *   'field group type', 'PMF program' (context), 'field stage' and required size.
 *   NOTE that failure to find a free key is not considered an error as
 *   far as 'error output' of this procedure is concerned. It is up to
 *   the caller to decide what to do about that. A error log print, however,
 *   is forced.
 *   See remarks.
 * \param [in] unit    -
 *   HW identifier of device.
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in] field_stage -
 *   dnx_field_stage_e. Type of stage requiring the key.
 * \param [in] context_id -
 *   Identifier of the Context ID on which key is to be allocated.
 * \param [in] key_length -
 *   uint32. Size of required bit-range. May be any value between 1 and     \n
 *   DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE. This version will only try to \n
 *   allocate 
 * \param [in] do_align      -
 *   uint32. Boolean. If FALSE then ignore 'aligned_bit'. Otherwise, force alignment
 *   as per 'aligned_bit'.
 * \param [in] aligned_bit      -
 *   int16. Number of bit, within the allocated 'key_length' bits, which
 *   should be aligned to 16 on the physical key. Could be positive or negative
 *   (between -16 to 'maximal key size' - 1). Numbering of bits starts from '0' at
 *   the LS bit.
 *   For example, if 'key_length' is 22 and we need BIT(5) to be aligned then
 *   the result may be:
 *                         MS                                              LS
 *                                                       BIT(5)
                                                            V
 *      Allocated 22 bits:                  ++++++++++++++++++++++
 *      Key              :   ------------------------------------------------
 *      Aligned bits     :                  |               |               |
 *                                                          ^
 *                                                       BIT(16)
 * \param [out] failed_to_find_p -
 *   dnx_field_key_failed_to_find_e. This procedure loads pointed memory by \n
 *   'DNX_FIELD_KEY_FAILED_NO_FAILURE' if a free key has been found and     \n
 *   successfully allocated. \n
 *   Otherwise, the reason for not finding a free key is as per             \n
 *   'dnx_field_key_failed_to_find_e'.
 * \param [out]  key_id_p -
 *   Pointer to dnx_field_key_id_t. This procedure loads this structure with the      \n
 *   identifier of the key that contains required bit-range (element 'id[0]' of type  \n
 *   dbal_enum_value_field_field_key_e). Caller is assumed to have initialized all    \n
 *   'id[]' elements by the value DBAL_NOF_ENUM_FIELD_KEY_VALUES.                     \n
 *   Only a single key is seached by this procedure (and not double key).             \n
 *   This procedure loads this structure with allocated single keys                   \n
 *   (of type dbal_enum_value_field_field_key_e).                                     \n
 *   Only meaningful if '*failed_to_find_p' is DNX_FIELD_KEY_FAILED_NO_FAILURE!       \n
 *   Returned structure also contains 'key_part' which indicates the part (MSB or LSB)\n
 *   of the single key which has been selected.                                       \n
 *   Only meaningful if 'key_length' is less than the size of one half-key            \n
 *   (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF) and the range was found to be contains   \n
 *   within a single half key (i.e., not occupying some bits on one half key and      \n
 *   others on the other half key). If a value of DNX_FIELD_KEY_PART_TYPE_INVALID is  \n
 *   returned then, depending on id[0], either a full single key was selected or no   \n
 *   key was found.                                                                   \n
 *   Important: Note that, unlike key_id_p->id[], key_part is NOT an array and only a \n
 *   single value is returned.                                                        \n
 * \param [out]  bit_range_offset_within_key_p -
 *   Pointer to uint32. This procedure loads pointed memory by the offset, within    \n
 *   selected key, starting from the LS bit, of the allocated bit-range. The maximal \n
 *   value is, then, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE-1
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The whole allocation procedure is carried out within SW STATE. \n
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored.                        \n
 *   * See 'key_occupation' for full allocation state
 * \see
 *   dnx_algo_field_key_alloc_generic()
 *   dnx_field_keys_per_stage_allocation_t
 *   dnx_field_key_alloc_detail_t
 */
static shr_error_e
dnx_algo_field_key_bit_range_alloc_generic(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 key_length,
    uint32 do_align,
    int16 aligned_bit,
    dnx_field_key_failed_to_find_e *failed_to_find_p,
    dnx_field_key_id_t *key_id_p,
    uint32 *bit_range_offset_within_key_p)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_available_keys_on_stage;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    /*
     * First key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key;
    /*
     * Last key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 last_available_key;
    /*
     * First and last key on combination of: specified stage and field group type (FP),
     * as offset from 'first_available_key_on_stage'. This is the actual index on
     * swstate.
     */
    uint8 last_half_key, first_half_key;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(failed_to_find_p, _SHR_E_PARAM, "failed_to_find_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(bit_range_offset_within_key_p, _SHR_E_PARAM, "bit_range_offset_within_key_p");
    /*
     * Initializations.
     */
    *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_PLACE;

    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_bit_range_alloc_generic_verify
                          (unit, fg_type, field_stage, context_id, key_length, do_align, aligned_bit, key_id_p,
                           bit_range_offset_within_key_p));

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input was not allocated any free keys.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NOT_INITIALIZED;
        SHR_ERR_EXIT(_SHR_E_NONE, "Uninitialized stage (%d, %s): No free keys have been assigned\r\n",
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * First, get the configuration structure (for stage and FG type).
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_NOT_ASSIGNED_TO_FG;
        SHR_ERR_EXIT(_SHR_E_NONE, "Uninitialized field group type (%d, %s) under assigned stage (%d, %s)\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        *failed_to_find_p = DNX_FIELD_KEY_FAILED_FG_HAS_ZERO_KEYS;
        SHR_ERR_EXIT(_SHR_E_NONE,
                     "Initialized field group type (%d, %s) under assigned stage (%d, %s) was assigned zero keys\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    if (!(dnx_field_key_alloc_instruction.availability_going_up))
    {
        /*
         * We still do not support 'searching downwards'
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Initialized field group (%d, %s) under assigned stage (%d, %s) was assigned 'search downwards.\r\n"
                     "This is not supported yet.\r\n",
                     (unsigned int) fg_type, dnx_field_group_type_e_get_name(fg_type),
                     (unsigned int) field_stage, dnx_field_stage_e_get_name(field_stage));
    }
    /*
     * Set range for available keys on this (stage,fg_type) combination.
     */
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    /*
     * Now search forward to get a free key.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Search for key which is not occupied: fg_type %d, field_stage %d context_id %d key_length %d\r\n",
                 fg_type, field_stage, context_id, key_length);

    {
        /*
         * Now go through all keys, including those that are marked 'occupied' and look for matching
         * bit-range. If one is found then do:
         *   If containing half key(s) are marked as 'busy' the there is nothing to do
         *   If containing half key(s) are not marked as occupied then mark them. That includes
         *     half key that may only be used for the 'tail' of the bit-range.
         */
        uint8 half_key_index;
        first_half_key = ((first_available_key - first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY);
        last_half_key = ((last_available_key - first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY) + 1;
        for (half_key_index = first_half_key; half_key_index <= last_half_key; half_key_index++)
        {
            int element;
            uint32 flags;
            shr_error_e rv;
            dnx_field_key_half_occupation_state_e key_occupation_state;
            uint8 num_bit_ranges, key_length_local;

            if (half_key_index % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
            {
                /*
                 * Note that we only relate to odd half keys (where info for the full key is stored).
                 * This testing is required because we want to avoid 'fail' messages. Calling
                 * with the 'SW_STATE_ALGO_RES_ALLOCATE_SIMULATION' flag does the job.
                 */

                if (do_align)
                {
                    /*
                     * Allocate at specified bits and just report whether allocation is possible.
                     */
                    uint32 found;
                    flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID | SW_STATE_ALGO_RES_ALLOCATE_SIMULATION;
                    if (aligned_bit < 0)
                    {
                        /*
                         * Enter if 'aligned_bit' is negative (alignment required on bit preceding
                         * the first).
                         *
                         * Since ABS('aligned_bit') is smaller than one 'step', we simply set
                         * 'element' to its absolute value.
                         */
                        element = (-aligned_bit);
                    }
                    else
                    {
                        /*
                         * Enter if 'aligned_bit' is positive or zero (alignment required on first 
                         * bit or a bit following the first.
                         */
                        int16 bit_on_multiple;

                        if (aligned_bit >= key_length)
                        {
                            /*
                             * Caller is asking for alignment of a bit which is beyond the bit-range.
                             * This makes no sense since alignment is required so that caller would
                             * get the following 32 bits as per 'key select'.
                             */
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Caller is asking for %d bits but alignment is required on bit %d\r\n"
                                         "which is beyond this bit-range.\r\n",
                                         (unsigned int) key_length, (unsigned int) aligned_bit);
                        }
                        /*
                         * Find the first multiple of 16 which is larger than the required 'aligned_bit'.
                         * Then, we shall try to allocate starting from that multiple minus 'aligned_bit'
                         * (It it possible to use 'and' operation and simplify the code but we are trying to not
                         * assume the step is a power of 2).
                         */
                        found = FALSE;
                        for (bit_on_multiple = 0;
                             bit_on_multiple < DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT;
                             bit_on_multiple += DNX_FIELD_KEY_STEP_VALUE_FOR_ALIGNED_BIT)
                        {
                            if (bit_on_multiple >= aligned_bit)
                            {
                                found = TRUE;
                                break;
                            }
                        }
                        if (found == FALSE)
                        {
                            /*
                             * Somehow, we failed to find aligned location which is larger than specified
                             * aligned bit. Quit with error.
                             */
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Failed to find aligned bit larger than %d while searching key up to bit %d.\r\n",
                                         (unsigned int) aligned_bit,
                                         (unsigned int) (DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT - 1));
                        }
                        /*
                         * Now 'bit_on_multiple' is the closest aligned bit which is larger than 'aligned_bit'.
                         * Set 'element' to start search with 'bit_on_multiple' as the first candidate.
                         */
                        element = (bit_on_multiple - aligned_bit);
                    }
                    {
                        found = FALSE;
                        /*
                         * Initial 'element' is set in the clause above.
                         */
                        for (; element < DNX_FIELD_KEY_MAX_VALUE_FOR_ALIGNED_BIT;
                             element += DNX_FIELD_KEY_STEP_VALUE_FOR_ALIGNED_BIT)
                        {
                            rv = KEY_OCCUPATION.bit_range_key_occ_bmp.allocate_several
                                (unit, field_stage, context_id, half_key_index, 0, flags, key_length, NULL, &element);
                            if (rv != _SHR_E_NONE)
                            {
                                /*
                                 * On failing to allocate required bit-range, skip to the next 'step'.
                                 */
                                continue;
                            }
                            else
                            {
                                found = TRUE;
                                break;
                            }
                        }
                        if (!found)
                        {
                            /*
                             * On failing to allocate required bit-range at required alignment,
                             * skip to the next key.
                             */
                            continue;
                        }
                        /*
                         * At this point, allocation was successful at required alignment. 'element'
                         * contains the location of the LS bit on bit-range.
                         */
                        /*
                         * Allocate at specified bits and just report whether allocation is possible.
                         */
                        flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.allocate_several
                                        (unit, field_stage, context_id, half_key_index,
                                         0, flags, key_length, NULL, &element));
                    }
                }
                else
                {
                    /*
                     * Reach here if no alignment is required.
                     */
                    flags = SW_STATE_ALGO_RES_ALLOCATE_SIMULATION;
                    rv = KEY_OCCUPATION.bit_range_key_occ_bmp.allocate_several
                        (unit, field_stage, context_id, half_key_index, 0, flags, key_length, NULL, &element);
                    if (rv != _SHR_E_NONE)
                    {
                        /*
                         * On failing to allocate required bit-range, skip to the next key.
                         */
                        continue;
                    }
                    /*
                     * At this point, allocation was successful at required alignment. 'element'
                     * contains the location of the LS bit on bit-range.
                     */
                    flags = 0;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.allocate_several
                                    (unit, field_stage, context_id, half_key_index,
                                     0, flags, key_length, NULL, &element));
                }
                /*
                 * Succeeded to allocate!!! 'element' contains the offset.
                 * Now mark indicated 'half key' as 'occupied'.
                 */
                *bit_range_offset_within_key_p = element;
                if (element < DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
                {
                    /*
                     * Enter if the allocation started on the LS half of the key.
                     */
                    {
                        /*
                         * This section is common both to a bit-range which is fully contained on the LS half key
                         * and to a bit-range which crosses the 'border' between the LS half key and the MS half key
                         *
                         * If the allocation started on the LS half of the key and
                         * also ended on that half then mark this half key as occupied
                         * and return.
                         */
                        key_occupation_state = DNX_FIELD_KEY_OCCUPY_LSB_HALF;
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                        (unit, field_stage, context_id, half_key_index, key_occupation_state));
                        /*
                         * Note that we write here the last fg_type used on this half key by 'bit-range'
                         * allocation. In theory, fg_type may vary on the same key. See 'KEY_OCCUPATION_BIT_RANGE.fg_type.set'
                         * below.
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                        (unit, field_stage, context_id, half_key_index, fg_type));
                        /*
                         * Specify we have one one more bit range here
                         * and that this half key is occupied also on the 'bit-range' administration.
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.get(unit, field_stage, context_id, half_key_index,
                                                                          &num_bit_ranges));
                        num_bit_ranges++;
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, half_key_index,
                                                                          num_bit_ranges));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                        is_half_key_range_occupied.set(unit, field_stage, context_id, half_key_index,
                                                                       TRUE));
                        /*
                         * Specify the 'key' and the contained 'half'. This is for display purposes only.
                         */
                        key_id_p->id[0] =
                            (dbal_enum_value_field_field_key_e) (first_available_key_on_stage +
                                                                 (half_key_index / DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                        key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_LSB_HALF;
                        /*
                         * Specify that the number of bits assigned, starting from bit 'element'
                         * of this half-key, is 'key_length'.
                         * (But, first, make sure this bit is not marked as 'occupied')
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.get
                                        (unit, field_stage, context_id, half_key_index, (uint32) element,
                                         &key_length_local));
                        if (key_length_local != 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "BIT %d on key %c (key part %s) is marked as starting a bit-range of %d bits while\r\n"
                                         "==> allocate_several() provided it as 'free'. Internal inconsistency.\r\n",
                                         (unsigned int) element, 'A' + (unsigned char) (key_id_p->id[0]),
                                         dnx_field_key_part_type_e_get_name(key_id_p->key_part),
                                         (int) key_length_local);
                        }
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.set
                                        (unit, field_stage, context_id, half_key_index, (uint32) element, key_length));
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.fg_type.set
                                        (unit, field_stage, context_id, half_key_index, (uint32) element,
                                         (uint8) fg_type));

                    }
                    if ((element + key_length - 1) >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
                    {
                        /*
                         * Enter if the newly allocated bit-range is partially contained within the LS half key
                         * and partially within the MS half key.
                         */
                        key_occupation_state = DNX_FIELD_KEY_OCCUPY_MSB_HALF;
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                        (unit, field_stage, context_id, half_key_index - 1, key_occupation_state));
                        /*
                         * Note that we write here the last fg_type used on this half key by 'bit-range'
                         * allocation. In theory, fg_type may vary on the same key. See 'KEY_OCCUPATION_BIT_RANGE.fg_type.set'
                         * above.
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                        (unit, field_stage, context_id, half_key_index - 1, fg_type));
                        /*
                         * Specify that this half key is occupied also on the 'bit-range' administration.
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.is_half_key_range_occupied.set(unit, field_stage, context_id,
                                                                                      half_key_index - 1, TRUE));
                        /*
                         * Specify we have one one more bit range here
                         * Note that the same bit-range is counted both on the LS half and on the MS half.
                         */
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                        num_bit_ranges.get(unit, field_stage, context_id, half_key_index - 1,
                                                           &num_bit_ranges));
                        num_bit_ranges++;
                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                        num_bit_ranges.set(unit, field_stage, context_id, half_key_index - 1,
                                                           num_bit_ranges));
                    }
                }
                else
                {
                    /*
                     * Enter if the allocation started on the MS half of the key (80 MS bits). Since we only have 160
                     * bits, the full bit-range must be between bit DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF and bit
                     * DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE-1
                     */
                    key_occupation_state = DNX_FIELD_KEY_OCCUPY_MSB_HALF;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                    (unit, field_stage, context_id, half_key_index - 1, key_occupation_state));
                    /*
                     * Note that we write here the last fg_type used on this half key by 'bit-range'
                     * allocation. In theory, fg_type may vary on the same key. See 'KEY_OCCUPATION_BIT_RANGE.fg_type.set'
                     * below.
                     */
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                    (unit, field_stage, context_id, half_key_index - 1, fg_type));
                    /*
                     * Specify we have one one more bit range here
                     * and that this half key is occupied also on the 'bit-range' administration.
                     */
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.get(unit, field_stage, context_id, half_key_index - 1,
                                                                      &num_bit_ranges));
                    num_bit_ranges++;
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, half_key_index - 1,
                                                                      num_bit_ranges));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    is_half_key_range_occupied.set(unit, field_stage, context_id, half_key_index - 1,
                                                                   TRUE));
                    /*
                     * Specify the 'key' and the contained 'half'
                     */
                    key_id_p->id[0] =
                        (dbal_enum_value_field_field_key_e) (first_available_key_on_stage +
                                                             ((half_key_index - 1) / DNX_FIELD_KEY_NUM_HALVES_IN_KEY));
                    key_id_p->key_part = DNX_FIELD_KEY_PART_TYPE_MSB_HALF;

                    /*
                     * Specify that the number of bits assigned, starting from bit
                     * 'element - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF' of this half-key,
                     * is 'key_length'. (But, first, make sure this bit is not marked as 'occupied')
                     */
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.get
                                    (unit, field_stage, context_id, half_key_index - 1,
                                     (uint32) (element - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF), &key_length_local));
                    if (key_length_local != 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "BIT %d on key %c (key part %s) is marked as starting a bit-range of %d bits while\r\n"
                                     "==> allocate_several() provided it as 'free'. Internal inconsistency.\r\n",
                                     (unsigned int) (element), 'A' + (unsigned char) (key_id_p->id[0]),
                                     dnx_field_key_part_type_e_get_name(key_id_p->key_part), (int) key_length_local);
                    }
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.set
                                    (unit, field_stage, context_id,
                                     half_key_index - 1, (uint32) (element - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF),
                                     key_length));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.fg_type.set
                                    (unit, field_stage, context_id, half_key_index - 1,
                                     (uint32) (element - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF), (uint8) fg_type));
                }
                *failed_to_find_p = DNX_FIELD_KEY_FAILED_NO_FAILURE;
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify procedure for dnx_algo_field_key_free_generic().
 * See its header for description of parameters.
 * \remark
 *  * None
 */
static shr_error_e
dnx_algo_field_key_free_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    uint8 nof_available_keys_on_stage;
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_half_keys;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    DNX_FIELD_KEY_ID_VERIFY(key_id_p->id[0]);
    if ((key_id_p->id[1] >= DBAL_NOF_ENUM_FIELD_KEY_VALUES) && (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key[1] identified (%d, larger than max: %d)\r\n", key_id_p->id[1],
                     DBAL_NOF_ENUM_FIELD_KEY_VALUES - 1);
    }
    if (key_id_p->key_part >= DNX_FIELD_KEY_PART_TYPE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key part (%d, larger than max: %d)\r\n", key_id_p->key_part,
                     DNX_FIELD_KEY_PART_TYPE_NOF - 1);
    }
    if ((key_id_p->key_part < DNX_FIELD_KEY_PART_TYPE_FIRST) && (key_id_p->key_part != DNX_FIELD_KEY_PART_TYPE_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key part (%d, lower than first: %d and NE invalid: %d)\r\n",
                     key_id_p->key_part, DNX_FIELD_KEY_PART_TYPE_FIRST, DNX_FIELD_KEY_PART_TYPE_INVALID);
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input stage was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Uninitialized stage (%d): No free keys have been assigned\r\n",
                     (unsigned int) field_stage);
    }
    /*
     * Get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Uninitialized field group type (%d) under assigned stage (%d)\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Initialized field group (%d) under assigned stage (%d) was assigned zero keys\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    SHR_IF_ERR_EXIT(KEY_STAGE_PROG_OCCUPATION.key_occupation_nof.get(unit, field_stage, context_id, &nof_half_keys));
    if (nof_half_keys == 0)
    {
        /*
         * Indicated input combination of (stage,PMF program) was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Uninitialized stage,PMF-program (%d,%d): No free keays have been assigned\r\n",
                     (unsigned int) field_stage, (unsigned int) context_id);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Verify procedure for dnx_algo_field_key_bit_range_free_generic().
 * See its header for description of parameters.
 * \remark
 *  * None
 */
static shr_error_e
dnx_algo_field_key_bit_range_free_generic_verify(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t key_id,
    uint32 bit_range_offset_within_key)
{
    uint8 nof_available_keys_on_stage;
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    uint8 nof_half_keys, supports_bitmap_allocation;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input
     */
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_FG_TYPE_VERIFY(fg_type);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    DNX_FIELD_KEY_ID_VERIFY(key_id.id[0]);
    SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.get
                    (unit, field_stage, key_id.id[0], &supports_bitmap_allocation));
    if (!supports_bitmap_allocation)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Illegal key for bit-range feature. key %c (on stage %s) does not support the feature.\r\n",
                     ('A' + (unsigned int) key_id.id[0]), dnx_field_stage_e_get_name(field_stage));
    }
    if (bit_range_offset_within_key >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Illegal offset within key (%d, larger than max: %d)\r\n",
                     (unsigned int) bit_range_offset_within_key, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE - 1);
    }
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    if (nof_available_keys_on_stage == 0)
    {
        /*
         * Indicated input stage was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Uninitialized stage (%d): No free keys have been assigned\r\n", (unsigned int) field_stage);
    }
    /*
     * Get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    if (!(dnx_field_key_alloc_instruction.structure_is_valid))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
         * yet been initialized.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Uninitialized field group type (%d) under assigned stage (%d)\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    if (!(dnx_field_key_alloc_instruction.nof_available_keys))
    {
        /*
         * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
         * been initialized but number of keys for this field-type has been set to '0'.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Initialized field group (%d) under assigned stage (%d) was assigned zero keys\r\n",
                     (unsigned int) fg_type, (unsigned int) field_stage);
    }
    SHR_IF_ERR_EXIT(KEY_STAGE_PROG_OCCUPATION.key_occupation_nof.get(unit, field_stage, context_id, &nof_half_keys));
    if (nof_half_keys == 0)
    {
        /*
         * Indicated input combination of (stage,PMF program) was not allocated any free keys.
         */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "\r\n"
                     "Uninitialized stage,PMF-program (%d,%d): No free keays have been assigned\r\n",
                     (unsigned int) field_stage, (unsigned int) context_id);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Free an allocated 'KEY' (or half key) given 'field_stage', PMF
 *   program,  key identifier (A - J) and 'half key' identifier (80 LSB
 *   or 80 MSB).
 *   Note that key must have been allocated or an error is flagged.
 * \param [in]  unit    -
 *   Device ID
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requiring the key.
 * \param [in]  field_stage -
 *   dnx_field_stage_e. Type of stage for which the key was allocated.
 * \param [in]  context_id -
 *   Identifier of Context ID for which the key was allocated.
 * \param [in]  key_id_p -
 *   structure holds an array of key Identifier and Part of key to free (80 MSB or 80 LSB).
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The whole allocation procedure is carried out within SW STATE.
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored.
 *   * See 'key_occupation' for full allocation state
 * \see
 *   dnx_field_keys_per_stage_allocation_t
 *   dnx_field_key_alloc_detail_t
 */
static shr_error_e
dnx_algo_field_key_free_generic(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    /*
     * First key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key;
    /*
     * Last key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 last_available_key;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_free_generic_verify(unit, fg_type, field_stage, context_id, key_id_p));

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    /*
     * First, get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    if (key_id_p->id[0] < (dbal_enum_value_field_field_key_e) first_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is smaller than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) first_available_key);
    }
    if (key_id_p->id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is larger than allowed (%d). Illegal. Quit\r\n",
                     key_id_p->id[0], (unsigned int) last_available_key);
    }
    /*
     * At this point, 'key_id' is within range.
     */
    /*
     * Now get info on that key: See that it is occupied.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Get info on key (is it occupied?): field_stage %d context_id %d key_id %d key_part_type %d\r\n",
                 field_stage, context_id, key_id_p->id[0], key_id_p->key_part);
    {
        uint8 half_key, free_one_half_key;
        uint8 current_half, current_half_index;
        dnx_field_key_alloc_detail_t dnx_field_key_alloc_detail[DNX_FIELD_KEY_MAX_HALVES_IN_KEY];
        uint32 key_allocation_id;

        half_key = (key_id_p->id[0] - first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY;
        free_one_half_key = TRUE;
        if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_LSB_HALF)
        {
            /*
             * If it is the LSB that needs to be freed then its half-key index is one
             * beyond the MSB-half-key.
             */
            half_key++;
        }
        else if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
        {
            /*
             * If it is the MSB that needs to be freed then its half-key index is exactly
             * right.
             */
        }
        else
        {
            /*
             * If no half key is specified then free all half-keys starting
             * from the MSB-half-key (which is the first, as per our arrangement).
             * This covers a full single key and a double key
             */
            free_one_half_key = FALSE;
        }
        if (free_one_half_key)
        {
            /*
             * Only one half key is required to be freed.
             */
            current_half = 0;
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                            (unit, field_stage, context_id, half_key,
                             &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get(unit, field_stage, context_id, half_key,
                                                                    &(dnx_field_key_alloc_detail
                                                                      [current_half].key_field_group_type)));
            if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d)\r\n",
                             (unsigned int) fg_type,
                             (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type));
            }
            if (dnx_field_key_alloc_detail[current_half].key_occupation_state == DNX_FIELD_KEY_OCCUPY_LSB_HALF)
            {
                if (key_id_p->key_part != DNX_FIELD_KEY_PART_TYPE_LSB_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input specified half (%d) and occupation state saved at allocation (%d)\r\n",
                                 key_id_p->key_part,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state));
                }
            }
            else
            {
                if (key_id_p->key_part != DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input specified half (%d) and occupation state saved at allocation (%d)\r\n",
                                 key_id_p->key_part,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state));
                }
            }
            /*
             * Finally! All is OK.
             */
            dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                            (unit, field_stage, context_id, half_key,
                             (dnx_field_key_alloc_detail[current_half].key_occupation_state)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set(unit, field_stage, context_id, half_key,
                                                                    (dnx_field_key_alloc_detail
                                                                     [current_half].key_field_group_type)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set(unit, field_stage, context_id, half_key,
                                                                 (dnx_field_key_alloc_detail
                                                                  [current_half].key_allocation_id)));
            /*
             * 'bit-range' section.
             * {
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_key_free_half_for_bit_range
                            (unit, fg_type, field_stage, context_id, half_key));
            /*
             * }
             */
        }
        else
        {
            /*
             * Caller requires to free more than one half key.
             */
            dnx_field_key_half_occupation_state_e key_occupation_state;
            current_half = 0;
            /*
             * Handle first 80 bits (MSB)
             */
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                            (unit, field_stage, context_id, half_key,
                             &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get(unit, field_stage, context_id, half_key,
                                                                    &(dnx_field_key_alloc_detail
                                                                      [current_half].key_field_group_type)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.get(unit, field_stage, context_id, half_key,
                                                                 &(dnx_field_key_alloc_detail
                                                                   [current_half].key_allocation_id)));

            if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d). current_half %d\r\n",
                             (unsigned int) fg_type,
                             (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type),
                             (unsigned int) current_half);
            }
            /*
             * Save info for requests further down (before updating the entry)
             */
            key_occupation_state = dnx_field_key_alloc_detail[current_half].key_occupation_state;
            /*
             * Store info for first half (80 MSB).
             */
            dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
            /*
             * Keep 'key_allocation_id' and use it to verify it is the same on all 'half keys' of this key.
             */
            key_allocation_id = dnx_field_key_alloc_detail[current_half].key_allocation_id;
            if (key_occupation_state == DNX_FIELD_KEY_OCCUPY_SINGLE_MSB)
            {
                /*
                 * Saved info indicates this was a single full key (total of 160 bits)
                 */
                current_half++;
                /*
                 * Handle second 80 bits (LSB)
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) fg_type,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type),
                                 (unsigned int) current_half);
                }
                if (dnx_field_key_alloc_detail[current_half].key_occupation_state != DNX_FIELD_KEY_OCCUPY_SINGLE_LSB)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between expected specified half (%d) and occupation state saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) DNX_FIELD_KEY_OCCUPY_SINGLE_LSB,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state),
                                 (unsigned int) current_half);
                }
                if (key_allocation_id != dnx_field_key_alloc_detail[current_half].key_allocation_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between first key_allocation_id (%d) and current one (%d). current_half %d\r\n",
                                 (unsigned int) key_allocation_id,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_allocation_id),
                                 (unsigned int) current_half);
                }
                /*
                 * All is OK for second half (80 LSB) which means all is OK for this single full key.
                 */
                dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
                current_half++;
                for (current_half_index = 0; current_half_index < current_half; current_half_index++)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_occupation_state)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_field_group_type)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_allocation_id)));
                    /*
                     * 'bit-range' section.
                     * {
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_key_free_half_for_bit_range
                                    (unit, fg_type, field_stage, context_id, half_key + current_half_index));
                    /*
                     * }
                     */
                }
            }
            else if (key_occupation_state == DNX_FIELD_KEY_OCCUPY_DOUBLE_FIRST_HALF)
            {
                /*
                 * Saved info indicates this was a double key (total of 320 bits)
                 */
                if (key_id_p->id[0] == (int) last_available_key)
                {
                    /*
                     * If key_id is the last_available_key then there is no place for another pair of 80 bits.
                     * Quit with error.
                     */
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Double key indicated by saved info but key_id (%d) is last key id (%d). current_half %d\r\n",
                                 (unsigned int) key_id_p->id[0], (unsigned int) last_available_key,
                                 (unsigned int) current_half);
                }
                current_half++;
                /*
                 * Handle second 80 bits (LSB) out of four.
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.get
                                (unit, field_stage, context_id, half_key + 1,
                                 &(dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) fg_type,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type),
                                 (unsigned int) current_half);
                }
                if (dnx_field_key_alloc_detail[current_half].key_occupation_state !=
                    DNX_FIELD_KEY_OCCUPY_DOUBLE_SECOND_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between expected specified half (%d) and occupation state saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) DNX_FIELD_KEY_OCCUPY_DOUBLE_SECOND_HALF,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state),
                                 (unsigned int) current_half);
                }
                if (key_allocation_id != dnx_field_key_alloc_detail[current_half].key_allocation_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between first key_allocation_id (%d) and current one (%d). current_half %d\r\n",
                                 (unsigned int) key_allocation_id,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_allocation_id),
                                 (unsigned int) current_half);
                }
                /*
                 * All is OK for second half (80 LSB) which means all is OK for first single key (out of two).
                 */
                dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
                current_half++;
                /*
                 * Handle third 80 bits (LSB) out of four.
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                (unit, field_stage, context_id, half_key + 2,
                                 &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get
                                (unit, field_stage, context_id, half_key + 2,
                                 &(dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.get
                                (unit, field_stage, context_id, half_key + 2,
                                 &(dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) fg_type,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type),
                                 (unsigned int) current_half);
                }
                if (dnx_field_key_alloc_detail[current_half].key_occupation_state !=
                    DNX_FIELD_KEY_OCCUPY_DOUBLE_THIRD_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between expected specified half (%d) and occupation state saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) DNX_FIELD_KEY_OCCUPY_DOUBLE_THIRD_HALF,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state),
                                 (unsigned int) current_half);
                }
                if (key_allocation_id != dnx_field_key_alloc_detail[current_half].key_allocation_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between first key_allocation_id (%d) and current one (%d). current_half %d\r\n",
                                 (unsigned int) key_allocation_id,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_allocation_id),
                                 (unsigned int) current_half);
                }
                /*
                 * All is OK for third half (80 LSB).
                 */
                dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
                current_half++;
                /*
                 * Handle fourth (last) 80 bits (LSB) out of four.
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get
                                (unit, field_stage, context_id, half_key + 3,
                                 &(dnx_field_key_alloc_detail[current_half].key_occupation_state)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get
                                (unit, field_stage, context_id, half_key + 3,
                                 &(dnx_field_key_alloc_detail[current_half].key_field_group_type)));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.get
                                (unit, field_stage, context_id, half_key + 3,
                                 &(dnx_field_key_alloc_detail[current_half].key_allocation_id)));
                if (dnx_field_key_alloc_detail[current_half].key_field_group_type != fg_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between input field_group_type (%d) and field_group_type saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) fg_type,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_field_group_type),
                                 (unsigned int) current_half);
                }
                if (dnx_field_key_alloc_detail[current_half].key_occupation_state !=
                    DNX_FIELD_KEY_OCCUPY_DOUBLE_FOURTH_HALF)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between expected specified half (%d) and occupation state saved at allocation (%d). current_half %d\r\n",
                                 (unsigned int) DNX_FIELD_KEY_OCCUPY_DOUBLE_FOURTH_HALF,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_occupation_state),
                                 (unsigned int) current_half);
                }
                if (key_allocation_id != dnx_field_key_alloc_detail[current_half].key_allocation_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mismatch between first key_allocation_id (%d) and current one (%d). current_half %d\r\n",
                                 (unsigned int) key_allocation_id,
                                 (unsigned int) (dnx_field_key_alloc_detail[current_half].key_allocation_id),
                                 (unsigned int) current_half);
                }
                /*
                 * All is OK for fourth half (80 LSB) which means all is OK with the double key.
                 */
                dnx_field_key_alloc_detail[current_half].key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
                current_half++;
                for (current_half_index = 0; current_half_index < current_half; current_half_index++)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_occupation_state)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_field_group_type)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                    (unit, field_stage, context_id, half_key + current_half_index,
                                     (dnx_field_key_alloc_detail[current_half_index].key_allocation_id)));
                    /*
                     * 'bit-range' section.
                     * {
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_key_free_half_for_bit_range
                                    (unit, fg_type, field_stage, context_id, half_key + current_half_index));
                    /*
                     * }
                     */
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Free an allocated bit range within a specified 'KEY' given 'field_stage', PMF
 *   program (context),  key identifier (A - J), field group type and offset of bit-range
 *   within 'key'.
 *   Note that key must have been allocated for bit-range or an error is flagged.
 * \param [in]  unit    -
 *   Device ID
 * \param [in] fg_type -
 *   dnx_field_group_type_e. Type of field group requesting to release bit-range within key.
 * \param [in]  field_stage -
 *   dnx_field_stage_e. Stage for which the key was allocated.
 * \param [in]  context_id -
 *   Identifier of Context ID for which the key was allocated.
 * \param [in]  key_id -
 *   Structure holding an array of key Identifier and Part of key to free (80 MSB or 80 LSB).
 *   Only the first key (key_id.id[0]) is referred to by this procedure. All the rest
 *   of the structure is ignored.
 * \param [in]  bit_range_offset_within_key -
 *   uint32. Offset, within specified key, of the LS bit of the bit-range to free.
 *   Value must correspond the the value returned by
 *   dnx_algo_field_key_bit_range_alloc_generic()
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * The whole allocation procedure is carried out within SW STATE.
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored.
 * \see
 *   dnx_algo_field_key_bit_range_alloc_generic()
 *   dnx_algo_field_key_bit_range_free_generic_verify()
 *   dnx_field_key_bit_range_detail_t
 */
static shr_error_e
dnx_algo_field_key_bit_range_free_generic(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t key_id,
    uint32 bit_range_offset_within_key)
{
    dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
    /*
     * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key_on_stage;
    /*
     * First key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 first_available_key;
    /*
     * Last key allowed on combination of: specified stage and field group type (FP).
     * See dbal_enum_value_field_field_key_e.
     */
    uint8 last_available_key;

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_key_bit_range_free_generic_verify
                          (unit, fg_type, field_stage, context_id, key_id, bit_range_offset_within_key));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    /*
     * First, get the configuration structure.
     */
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get(unit, field_stage, fg_type, &dnx_field_key_alloc_instruction));
    first_available_key = dnx_field_key_alloc_instruction.first_available_key;
    last_available_key = first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
    if (key_id.id[0] < (dbal_enum_value_field_field_key_e) first_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is smaller than allowed (%d). Illegal. Quit\r\n",
                     key_id.id[0], (unsigned int) first_available_key);
    }
    if (key_id.id[0] > (dbal_enum_value_field_field_key_e) last_available_key)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Input key_id (%d) is larger than allowed (%d). Illegal. Quit\r\n",
                     key_id.id[0], (unsigned int) first_available_key);
    }
    /*
     * At this point, 'key_id' is within range.
     *
     * Now free that key.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free bit-range on key: field_stage %s context_id %d key_id %d bit_range_offset_within_key %d\r\n",
                 dnx_field_stage_e_get_name(field_stage), context_id, key_id.id[0], bit_range_offset_within_key);
    {
        uint8 ms_half_key, ls_half_key;
        uint8 num_bit_ranges, key_length_local;

        ms_half_key = (key_id.id[0] - first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY;
        ls_half_key = ms_half_key + 1;
        if (bit_range_offset_within_key < DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
        {
            /*
             * Enter if the allocation started on the LS half of the key.
             */
            {
                /*
                 * Verify that this half key is marked as 'occupied'
                 */
                DNX_FIELD_KEY_VERIFY_HALF_KEY_OCCUPATION(unit, field_stage, context_id, ls_half_key, key_id,
                                                         bit_range_offset_within_key, "LS");
                /*
                 * Key is marked 'occupied' on the 'half key' system. 
                 * Now verify bit-range was indeed allocated at this offset.
                 * Also verify it was allocated with 'field group type' as specified by the caller.
                 */
                DNX_FIELD_KEY_VERIFY_FG_TYPE_FOR_BIT_RANGE(unit, field_stage, context_id, fg_type, ls_half_key, key_id,
                                                           bit_range_offset_within_key, "LS", key_length_local);
                /*
                 * 'key_length_local' now contains the number of bits that have been allocated at
                 * offset 'bit_range_offset_within_key'.
                 * Mark that offset as 'free'
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.set
                                (unit, field_stage, context_id, ls_half_key, (uint32) bit_range_offset_within_key, 0));
                /*
                 * Free corresponding bits on resource manager.
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.free_several
                                (unit, field_stage, context_id, ls_half_key,
                                 0, key_length_local, bit_range_offset_within_key));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.get(unit, field_stage, context_id, ls_half_key,
                                                                  &num_bit_ranges));
                if (num_bit_ranges == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "Stage %s context_id %d\r\n"
                                 "LS half on key %c is marked as having 'zero' ranges but it is not marked as 'free'. Internal error?.\r\n",
                                 dnx_field_stage_e_get_name(field_stage), context_id,
                                 'A' + (unsigned char) (key_id.id[0]));
                }
                num_bit_ranges--;
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, ls_half_key,
                                                                  num_bit_ranges));
                /*
                 * If this is the last bit range on this half key then mark this half key as 'free'
                 */
                if (num_bit_ranges == 0)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    is_half_key_range_occupied.set(unit, field_stage, context_id, ls_half_key, FALSE));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    key_occupation_state.set(unit, field_stage, context_id, ls_half_key,
                                                             DNX_FIELD_KEY_OCCUPY_NONE));
                }
            }
            if ((bit_range_offset_within_key + key_length_local - 1) >= DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)
            {
                /*
                 * Enter if this bit-range did cross into the MS half bit: It was
                 * NOT fully contained on the LS half bit.
                 */
                /*
                 * Verify that this half key is marked as 'occupied'
                 */
                DNX_FIELD_KEY_VERIFY_HALF_KEY_OCCUPATION(unit, field_stage, context_id, ms_half_key, key_id,
                                                         bit_range_offset_within_key, "MS");
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                num_bit_ranges.get(unit, field_stage, context_id, ms_half_key, &num_bit_ranges));
                if (num_bit_ranges == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "Stage %s context_id %d\r\n"
                                 "MS half on key %c is marked as having 'zero' ranges but it is not marked as 'free'. Internal error?.\r\n",
                                 dnx_field_stage_e_get_name(field_stage), context_id,
                                 'A' + (unsigned char) (key_id.id[0]));
                }
                num_bit_ranges--;
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, ms_half_key,
                                                                  num_bit_ranges));
                /*
                 * If this is the last bit range on this half key then mark this half key as 'free'
                 */
                if (num_bit_ranges == 0)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    is_half_key_range_occupied.set(unit, field_stage, context_id, ms_half_key, FALSE));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    key_occupation_state.set(unit, field_stage, context_id, ms_half_key,
                                                             DNX_FIELD_KEY_OCCUPY_NONE));
                }

            }
        }
        else
        {
            /*
             * Enter if the allocation started on the MS half of the key.
             */
            {
                /*
                 * Verify that this half key is marked as 'occupied'
                 */
                DNX_FIELD_KEY_VERIFY_HALF_KEY_OCCUPATION(unit, field_stage, context_id, ms_half_key, key_id,
                                                         bit_range_offset_within_key, "MS");
                /*
                 * Key is marked 'occupied' on the 'half key' system.
                 * Now verify bit-range was indeed allocated at this offset.
                 * Also verify it was actually allocated and with 'field group type' as specified by the caller.
                 */
                DNX_FIELD_KEY_VERIFY_FG_TYPE_FOR_BIT_RANGE(unit, field_stage, context_id, fg_type, ms_half_key, key_id,
                                                           bit_range_offset_within_key, "MS", key_length_local);
                /*
                 * 'key_length_local' now contains the number of bits that have been allocated at
                 * offset '(bit_range_offset_within_key - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF)'.
                 * Mark that offset as 'free'
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.bit_range_size.set
                                (unit, field_stage, context_id, ms_half_key,
                                 (uint32) (bit_range_offset_within_key - DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF), 0));
                /*
                 * Free corresponding bits on resource manager. Note that resource is for the whole key
                 * and is based on the ls half key.
                 */
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.free_several
                                (unit, field_stage, context_id, ls_half_key,
                                 0, key_length_local, bit_range_offset_within_key));
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.get(unit, field_stage, context_id, ms_half_key,
                                                                  &num_bit_ranges));
                if (num_bit_ranges == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "Stage %s context_id %d\r\n"
                                 "LS half on key %c is marked as having 'zero' ranges but it is not marked as 'free'. Internal error?.\r\n",
                                 dnx_field_stage_e_get_name(field_stage), context_id,
                                 'A' + (unsigned char) (key_id.id[0]));
                }
                num_bit_ranges--;
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set(unit, field_stage, context_id, ms_half_key,
                                                                  num_bit_ranges));
                /*
                 * If this is the last bit range on this half key then mark this half key as 'free'
                 */
                if (num_bit_ranges == 0)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    is_half_key_range_occupied.set(unit, field_stage, context_id, ms_half_key, FALSE));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.
                                    key_occupation_state.set(unit, field_stage, context_id, ms_half_key,
                                                             DNX_FIELD_KEY_OCCUPY_NONE));
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_field.h
 */
shr_error_e
dnx_algo_field_key_alloc_sw_state_init(
    int unit)
{
    /*
     * Array carrying the stages which are initialized by this procedure.
     * Used for looping on all supported stages.
     */
    dnx_field_stage_e supported_stages[DNX_FIELD_STAGE_NOF];
    /*
     * Number of meaningful entries in supported_stages[]
     */
    uint32 num_supported_stages;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_keys_per_stage_allocation_sw.init(unit));
    {
        /*
         * Initialize key allocation setup.
         */
        dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
        dnx_field_key_alloc_stage_instruction_info_t dnx_field_key_alloc_stage_instruction_info;
        uint32 nof_stages, nof_group_types, stage_index, group_type_index;
        /*
         * Deduce the number of elements of the configuration array, keys_availability_info
         * (in sw state), directly from its definition, as created by sw state.
         */
        nof_group_types =
            sizeof(dnx_field_key_alloc_stage_instruction_info.
                   keys_availability_stage_info[0].keys_availability_group_info) /
            sizeof(dnx_field_key_alloc_stage_instruction_info.
                   keys_availability_stage_info[0].keys_availability_group_info[0]);
        nof_stages =
            sizeof(dnx_field_key_alloc_stage_instruction_info.keys_availability_stage_info) /
            sizeof(dnx_field_key_alloc_stage_instruction_info.keys_availability_stage_info[0]);
        /*
         * This is not required but it is more elegant.
         */
        sal_memset(&dnx_field_key_alloc_instruction, 0, sizeof(dnx_field_key_alloc_instruction));
        /*
         * Create an entry which is not valid.
         */
        dnx_field_key_alloc_instruction.structure_is_valid = FALSE;
        /*
         * Make sure all configuration entries are marked as 'invalid'
         * We assume here that stages start from '0' and go up sequentially.
         */
        for (stage_index = 0; stage_index < nof_stages; stage_index++)
        {
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.range_fill(unit, stage_index, 0,
                                                                    nof_group_types, dnx_field_key_alloc_instruction));
            /*
             * Initialize number of keys on all stages to '0'. Fill the operational
             * values right below.
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.nof_available_keys_on_stage.set(unit, stage_index, 0));
        }
        /*
         * Now fill in valid configurations.
         */
        {
            num_supported_stages = 0;
            /*
             * Set DNX_FIELD_STAGE_IPMF1:
             * Total number of keys           : DNX_FIELD_KEY_NOF_FOR_IPMF1
             * First key on stage             : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Number of keys for TCAM        : DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_TCAM
             * First key on fg type           : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Last key                       : DBAL_ENUM_FVAL_FIELD_KEY_A + DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_TCAM - 1
             * availability_odd_only          : TRUE
             * Number of keys for TCAM_FORCE_B: 1
             * First key on fg type           : DBAL_ENUM_FVAL_FIELD_KEY_B
             * Last key                       : DBAL_ENUM_FVAL_FIELD_KEY_B
             * availability_odd_only          : TRUE
             * Number of keys for TCAM_FORCE_D: 1
             * First key on fg type           : DBAL_ENUM_FVAL_FIELD_KEY_D
             * Last key                       : DBAL_ENUM_FVAL_FIELD_KEY_D
             * availability_odd_only          : TRUE
             * Number of keys for EXEM        : DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_EXEM
             * First key on fg type           : DBAL_ENUM_FVAL_FIELD_KEY_A + Selected EXEM key for IPMF1.
             * Last key                       : DBAL_ENUM_FVAL_FIELD_KEY_E + DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_EXEM - 1
             * availability_odd_only          : TRUE
             */
            stage_index = DNX_FIELD_STAGE_IPMF1;
            supported_stages[num_supported_stages++] = stage_index;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            nof_available_keys_on_stage.set(unit, stage_index, DNX_FIELD_KEY_NOF_FOR_IPMF1(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_TCAM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * By hardware options, EXEM key may be any key from A to E. For this system, code specifies one of 
             * these keys to be used for EXEM. This key is indicated in DNX DATA for iPMF1 by 'large_ipmf1_key'
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF1_FOR_EXEM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_A +
                                                                                 dnx_data_field.exem.
                                                                                 large_ipmf1_key_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
        }
        {
            stage_index = DNX_FIELD_STAGE_IPMF2;
            /*
             * Set DNX_FIELD_STAGE_IPMF2:
             * Total number of keys   : DNX_FIELD_STAGE_IPMF2
             * First key on stage     : DBAL_ENUM_FVAL_FIELD_KEY_F
             * Number of keys for TCAM: DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_TCAM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_F
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_F + DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_TCAM - 1
             * availability_odd_only  : TRUE
             * Number of keys for EXEM: DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_EXEM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_J + Selected EXEM key for IPMF1.
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_J + DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_EXEM - 1
             * availability_odd_only  : TRUE
             * Number of keys for DIR EXT: DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_DIR_EXT
             * First key on fg type      : DBAL_ENUM_FVAL_FIELD_KEY_I
             * Last key                  : DBAL_ENUM_FVAL_FIELD_KEY_I + DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_DIR_EXT - 1
             * availability_odd_only     : FALSE
             */
            supported_stages[num_supported_stages++] = stage_index;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            nof_available_keys_on_stage.set(unit, stage_index, DNX_FIELD_KEY_NOF_FOR_IPMF2(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_F));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_TCAM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_F));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * By hardware options, EXEM key may be any key from F to J. For this system, code specifies one of 
             * these keys to be used for EXEM. This key for iPMF2 is indicated in DNX DATA 'small_ipmf2_key'
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_EXEM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.first_available_key.set(unit, stage_index, group_type_index,
                                                                                 DBAL_ENUM_FVAL_FIELD_KEY_F +
                                                                                 dnx_data_field.exem.
                                                                                 small_ipmf2_key_get(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * DIRECT_EXTRACTION
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF2_FOR_DIR_EXT
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_I));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, FALSE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, TRUE));
        }
        {
            stage_index = DNX_FIELD_STAGE_IPMF3;
            /*
             * Set DNX_FIELD_STAGE_IPMF3:
             * Total number of keys   : DNX_FIELD_STAGE_IPMF3
             * First key on stage     : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Number of keys for TCAM: DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_TCAM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_A + DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_TCAM - 1
             * availability_odd_only  : TRUE
             * Number of keys for EXEM: DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_EXEM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_C
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_C + DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_EXEM - 1
             * availability_odd_only  : TRUE
             * Number of keys for DIR EXT: DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_DIR_EXT
             * First key on fg type      : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Last key                  : DBAL_ENUM_FVAL_FIELD_KEY_A + DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_DIR_EXT - 1
             * availability_odd_only     : FALSE
             */
            supported_stages[num_supported_stages++] = stage_index;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            nof_available_keys_on_stage.set(unit, stage_index, DNX_FIELD_KEY_NOF_FOR_IPMF3(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_TCAM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * The EXEM key for IPMF3 cannot be changed, unlike in IPMF1/2.
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_EXEM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_C));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * DIRECT_EXTRACTION
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_IPMF3_FOR_DIR_EXT
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, FALSE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, TRUE));
        }
        {
            stage_index = DNX_FIELD_STAGE_EPMF;
            /*
             * Set DNX_FIELD_STAGE_EPMF:
             * Total number of keys   : DNX_FIELD_STAGE_EPMF
             * First key on stage     : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Number of keys for TCAM: DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_TCAM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_B
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_B + DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_TCAM - 1
             * availability_odd_only  : TRUE
             * Number of keys for EXEM: DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_EXEM
             * First key on fg type   : DBAL_ENUM_FVAL_FIELD_KEY_A
             * Last key               : DBAL_ENUM_FVAL_FIELD_KEY_A + DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_EXEM - 1
             * availability_odd_only  : TRUE
             */
            supported_stages[num_supported_stages++] = stage_index;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            nof_available_keys_on_stage.set(unit, stage_index, DNX_FIELD_KEY_NOF_FOR_EPMF(unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                            first_available_key_on_stage.set(unit, stage_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            /*
             * TCAM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_TCAM;
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_TCAM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_B));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
            /*
             * EXEM
             */
            group_type_index = DNX_FIELD_GROUP_TYPE_EXEM;
            /*
             * The EXEM key for EPMF cannot be changed, unlike in IPMF1/2.
             */
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.nof_available_keys.set(unit, stage_index, group_type_index,
                                                                                DNX_FIELD_KEY_NOF_FOR_EPMF_FOR_EXEM
                                                                                (unit)));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            first_available_key.set(unit, stage_index, group_type_index, DBAL_ENUM_FVAL_FIELD_KEY_A));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_going_up.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            availability_odd_only.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            structure_is_valid.set(unit, stage_index, group_type_index, TRUE));
            SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                            supports_bitmap_allocation.set(unit, stage_index, group_type_index, FALSE));
        }
    }
    {
        dnx_field_stage_e stage_index;
        dnx_field_context_t pmf_program_index;
        uint8 nof_half_keys, nof_keys, half_key_index;
        dnx_field_stage_e field_stage;
        dnx_field_key_alloc_detail_t dnx_field_key_alloc_detail;
        /*
         * This is not required but is more elegant.
         */
        memset(&dnx_field_key_alloc_detail, 0, sizeof(dnx_field_key_alloc_detail));
        /*
         * Prepare a structure of a 'free' (not allocated) entry.
         */
        dnx_field_key_alloc_detail.key_occupation_state = DNX_FIELD_KEY_OCCUPY_NONE;
        /*
         * General initialization for 'bit-range' feature.
         * Not required but nice-to-have.
         */
        dnx_field_key_alloc_detail.num_bit_ranges = 0;
        dnx_field_key_alloc_detail.is_half_key_range_occupied = 0;
        /*
         * Assign number of half-keys per PMF program per stage.
         * Assign all half-keys and 'free'.
         */
        for (stage_index = 0; stage_index < num_supported_stages; stage_index++)
        {
            field_stage = supported_stages[stage_index];
            for (pmf_program_index = 0; pmf_program_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS;
                 pmf_program_index++)
            {
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                                nof_available_keys_on_stage.get(unit, field_stage, &nof_keys));
                nof_half_keys = nof_keys * DNX_FIELD_KEY_NUM_HALVES_IN_KEY;
                SHR_IF_ERR_EXIT(KEY_OCCUPATION.alloc(unit, field_stage, pmf_program_index, nof_half_keys));
                SHR_IF_ERR_EXIT(KEY_STAGE_PROG_OCCUPATION.
                                key_occupation_nof.set(unit, field_stage, pmf_program_index, nof_half_keys));
                /*
                 * At this point, we have allocated memory space required for keys assigned to IPMF1.
                 * Now mark each entry as 'free' (= 'not containing valid info' = DNX_FIELD_KEY_OCCUPY_NONE)
                 */
                for (half_key_index = 0; half_key_index < nof_half_keys; half_key_index++)
                {
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.set
                                    (unit, field_stage, pmf_program_index, half_key_index,
                                     (dnx_field_key_alloc_detail.key_occupation_state)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.set
                                    (unit, field_stage, pmf_program_index, half_key_index,
                                     (dnx_field_key_alloc_detail.key_field_group_type)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_allocation_id.set
                                    (unit, field_stage, pmf_program_index, half_key_index,
                                     (dnx_field_key_alloc_detail.key_allocation_id)));

                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.num_bit_ranges.set
                                    (unit, field_stage, pmf_program_index, half_key_index,
                                     (dnx_field_key_alloc_detail.num_bit_ranges)));
                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.is_half_key_range_occupied.set
                                    (unit, field_stage, pmf_program_index, half_key_index,
                                     (dnx_field_key_alloc_detail.is_half_key_range_occupied)));
                }
            }
        }
        {
            /*
             * Initializations for 'bit-range' feature
             * Per stage per field group:
             *   Create OCC bit map per each half-keys that is marked as supporting 'bit-range'.
             *   Allocate and initialize 'bit_range_size[]' per each half-keys that is marked as supporting 'bit-range'.
             */
            char *stage_prefix;
            uint stage_prefix_len;
            char *group_type_prefix;
            uint group_type_prefix_len;
            char *program_prefix;
            dnx_field_group_type_e fg_type_index, fg_type_max;
            dnx_field_key_alloc_instruction_t dnx_field_key_alloc_instruction;
            uint8 nof_available_keys_on_stage;
            uint8 supports_bitmap_allocation;
            /*
             * First key allowed on specified stage. See dbal_enum_value_field_field_key_e.
             */
            uint8 first_available_key_on_stage;
            /*
             * First key allowed on combination of: specified stage and field group type (FP).
             * See dbal_enum_value_field_field_key_e.
             */
            uint8 first_available_key;
            /*
             * Last key allowed on combination of: specified stage and field group type (FP).
             * See dbal_enum_value_field_field_key_e.
             */
            uint8 last_available_key;
            /*
             * Prepare variables to be used in constructing the 'resource name' for 'bit-range'
             * feature.
             */
            uint8 key_index;
            stage_prefix = "DNX_FIELD_";
            stage_prefix_len = sal_strlen(stage_prefix);
            group_type_prefix = "DNX_FIELD_GROUP_TYPE";
            group_type_prefix_len = strlen(group_type_prefix);
            program_prefix = "_CTXT_";
            for (stage_index = 0; stage_index < num_supported_stages; stage_index++)
            {
                field_stage = supported_stages[stage_index];
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                                nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
                SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                                first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
                if (nof_available_keys_on_stage == 0)
                {
                    /*
                     * Input was not allocated any free keys.
                     * Should not happen but, for now, ignore.
                     */
                    continue;
                }
                /*
                 * 'Bit-range' feature. Mark all keys on this stage as 'not supporting 'bit-range'.
                 * This is the initial value. Below, some keys, which do support it, are so marked.
                 */
                for (key_index = first_available_key_on_stage;
                     key_index < (first_available_key_on_stage + nof_available_keys_on_stage); key_index++)
                {
                    SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.set(unit, field_stage, key_index, FALSE));
                }
                fg_type_max = DNX_FIELD_GROUP_TYPE_NOF;
                for (fg_type_index = DNX_FIELD_GROUP_TYPE_FIRST; fg_type_index < fg_type_max; fg_type_index++)
                {
                    /*
                     * First, get the configuration structure (for stage and FG type).
                     */
                    uint8 last_half_key, first_half_key;
                    sw_state_algo_res_create_data_t sw_state_data;
                    char bit_range_name[SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH];
                    char *bit_range_name_p;
                    char *base_bit_range_name_p;
                    char *bit_range_p;
                    uint32 sub_resource_id;

                    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.
                                    supports_bitmap_allocation.get(unit, field_stage, fg_type_index,
                                                                   &supports_bitmap_allocation));
                    if (supports_bitmap_allocation)
                    {
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "Initialization of 'bit-range' feature: stage %s FG type %s %s%s\r\n",
                                     dnx_field_stage_e_get_name(field_stage),
                                     dnx_field_group_type_e_get_name(fg_type_index), EMPTY, EMPTY);
                        SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_GROUP_INFO.get
                                        (unit, field_stage, fg_type_index, &dnx_field_key_alloc_instruction));
                        if (!(dnx_field_key_alloc_instruction.structure_is_valid))
                        {
                            /*
                             * Initialization seems to not have been completed. Stage was assigned keys but this field group type has not
                             * yet been initialized. 
                             *
                             * Should not happen but, for now, ignore.
                             */
                            continue;
                        }
                        if (!(dnx_field_key_alloc_instruction.nof_available_keys))
                        {
                            /*
                             * Initialization seems to not have been completed. Stage was assigned keys and this field group type has
                             * been initialized but number of keys for this field-type has been set to '0'.
                             *
                             * Should not happen but, for now, ignore.
                             */
                            continue;
                        }
                        first_available_key = dnx_field_key_alloc_instruction.first_available_key;
                        last_available_key =
                            first_available_key + dnx_field_key_alloc_instruction.nof_available_keys - 1;
                        /*
                         * For this stage, mark all keys that support bit-range feature. This will be
                         * used by other 'field group types', which do not support it but do use these keys,
                         * to indicate to the supporting 'field group types' that a half key is occupied.
                         */
                        for (key_index = first_available_key; key_index <= last_available_key; key_index++)
                        {
                            SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.set
                                            (unit, field_stage, key_index, TRUE));
                        }
                        sal_memset(&sw_state_data, 0, sizeof(sw_state_data));
                        sw_state_data.first_element = 0;
                        /*
                         * Key size here represents the 160 bit of data
                         */
                        sw_state_data.nof_elements = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE;
                        /*
                         * Specify flag for creating an indexed pool of similar resorces, one per key (on odd half keys)
                         * We currently use a pool of 1 per key.
                         */
                        sw_state_data.flags = SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
                        bit_range_p = (char *) dnx_field_stage_e_get_name(field_stage);
                        bit_range_p += stage_prefix_len;
                        /*
                         * Point 'bit_range_name_p' to the beginning of the 'name' and, following that,
                         * to the end of the 'name' text.
                         */
                        bit_range_name_p = &bit_range_name[0];
                        strcpy(bit_range_name_p, bit_range_p);
                        bit_range_name_p += strlen(bit_range_name_p);
                        bit_range_p = (char *) dnx_field_group_type_e_get_name(fg_type_index);
                        bit_range_p += group_type_prefix_len;
                        strcpy(bit_range_name_p, bit_range_p);
                        bit_range_name_p += strlen(bit_range_name_p);
                        strcpy(bit_range_name_p, program_prefix);
                        bit_range_name_p += strlen(bit_range_name_p);
                        sw_state_data.nof_resource_pool_indexes = 1;
                        base_bit_range_name_p = bit_range_name_p;
                        for (pmf_program_index = 0;
                             pmf_program_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; pmf_program_index++)
                        {
                            dnx_field_key_bit_range_detail_t bit_range_detail;

                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Initialization of 'bit-range' feature: stage %s FG type %s, context %d %s\r\n",
                                         dnx_field_stage_e_get_name(field_stage),
                                         dnx_field_group_type_e_get_name(fg_type_index), pmf_program_index, EMPTY);
                            bit_range_name_p = base_bit_range_name_p;
                            /*
                             * Now make the resource name:
                             * <stage name><context number>
                             */
                            sal_sprintf(bit_range_name_p, "%d", pmf_program_index);
                            bit_range_name_p += strlen(bit_range_name_p);
                            first_half_key =
                                ((first_available_key -
                                  first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY);
                            last_half_key =
                                ((last_available_key -
                                  first_available_key_on_stage) * DNX_FIELD_KEY_NUM_HALVES_IN_KEY) + 1;
                            /*
                             * At this point, we have the first half key and the number of half keys for specified 'field group type'
                             * and for specofied context (program).
                             * Note that the elements 'num_bit_ranges' and 'is_half_key_range_occupied'
                             * have already been initialized above.
                             */
                            sal_memset(&bit_range_detail, 0, sizeof(bit_range_detail));
                            for (half_key_index = first_half_key; half_key_index <= last_half_key; half_key_index++)
                            {
                                SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.alloc
                                                (unit, field_stage, pmf_program_index, half_key_index,
                                                 DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF));
                                /*
                                 * This may be redundant but, just to make sure, fill the 'bit_range_size' with
                                 * zeroes (which indicates 'empty). This array is loaded by the size of
                                 * bit-range allocated at that index. The size may, actually, extend to the other
                                 * half but, currently, not from key to key.
                                 */
                                SHR_IF_ERR_EXIT(KEY_OCCUPATION_BIT_RANGE.range_fill
                                                (unit, field_stage, pmf_program_index, half_key_index,
                                                 0, DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF, bit_range_detail));
                                if (half_key_index % DNX_FIELD_KEY_NUM_HALVES_IN_KEY)
                                {
                                    /*
                                     * Note that, on the resource name, the index of the half key is from the first
                                     * key ('A'). For example, key 'I' will occupy two half keys: The MS is
                                     * half key at index 16 and the LS is half key at index 17.
                                     */
                                    sal_sprintf(bit_range_name_p, "_HALF_KEY_%d",
                                                (first_available_key_on_stage * DNX_FIELD_KEY_NUM_HALVES_IN_KEY) +
                                                half_key_index);
                                    /*
                                     * Create name based on stage and context (program)
                                     */
                                    sal_strncpy(sw_state_data.name, bit_range_name,
                                                SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
                                    /*
                                     * Actually create the resource for odd half keys only (representing the corresponding
                                     * two half keys which, together, make a full key)
                                     */
                                    SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.alloc(unit, field_stage,
                                                                                               pmf_program_index,
                                                                                               half_key_index,
                                                                                               sw_state_data.nof_resource_pool_indexes));

                                    for (sub_resource_id = 0; sub_resource_id < sw_state_data.nof_resource_pool_indexes;
                                         sub_resource_id++)
                                    {
                                        SHR_IF_ERR_EXIT(KEY_OCCUPATION.bit_range_key_occ_bmp.create
                                                        (unit, field_stage, pmf_program_index, half_key_index,
                                                         sub_resource_id, &sw_state_data, NULL));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Standard 'generaic key allocation':
 *  Analyze 'failed_to_find' as per dnx_field_key_failed_to_find_e and print
 *  corresponding text. Important: It is assumed that 'failed_to_find'
 *  is NOT DNX_FIELD_KEY_FAILED_NO_FAILURE.
 * \param [in] unit -
 *   Device Id
 * \param [in] fg_type -
 *   Field group type corresponding to failure to find free key.
 * \param [in] context_id -
 *   Context ID corresponding to failure to find free key.
 * \param [in] key_length -
 *   Specified key length (80,160,320) corresponding to failure to find free key.
 * \param [in] field_stage -
 *   Field stage corresponding to failure to find free key.
 * \param [in] failed_to_find -
 *   Enum describing reason for failure to find free key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_field_stringify_failed_to_find(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length,
    dnx_field_stage_e field_stage,
    dnx_field_key_failed_to_find_e failed_to_find)
{
    SHR_FUNC_INIT_VARS(unit);

    if (failed_to_find == DNX_FIELD_KEY_FAILED_NO_FAILURE)
    {
        /*
         * All is OK. Return with allocated key.
         */
        SHR_EXIT();
    }
    switch (failed_to_find)
    {
        case DNX_FIELD_KEY_FAILED_NO_PLACE:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No free key: All keys occupied for fg_type %d (%s), stage %d (%s), context ID %d, "
                         "key_length %d (%s)\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length,
                         dnx_field_key_length_type_e_get_name(key_length));
            break;
        }
        case DNX_FIELD_KEY_FAILED_NOT_INITIALIZED:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No keys were made available for fg_type %d (%s), stage %d (%s), context ID %d, "
                         "key_length %d (%s)\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length,
                         dnx_field_key_length_type_e_get_name(key_length));
            break;
        }
        case DNX_FIELD_KEY_FAILED_NOT_ASSIGNED_TO_FG:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "Key configuration has not yet been assigned for fg_type %d (%s), stage %d (%s), "
                         "context ID %d, key_length %d (%s)\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length,
                         dnx_field_key_length_type_e_get_name(key_length));
            break;
        }
        case DNX_FIELD_KEY_FAILED_FG_HAS_ZERO_KEYS:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No key found because configuration is that specified field group type has zero\r\n"
                         "keys for fg_type %d (%s), stage %d (%s), context ID %d, key_length %d (%s)\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length,
                         dnx_field_key_length_type_e_get_name(key_length));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No keys were made available for fg_type %d, stage %d, context ID %d, key_length %d\n",
                         fg_type, field_stage, context_id, key_length);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_allocate(
    int unit,
    dnx_algo_field_key_flags_e flags,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_length_type_e key_length,
    dnx_field_key_id_t *key_id_p,
    uint32 *key_allocation_id_p)
{
    dnx_field_key_failed_to_find_e failed_to_find;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    failed_to_find = DNX_FIELD_KEY_FAILED_NO_FAILURE;
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF3:
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_alloc_generic
                            (unit, flags, fg_type, field_stage, context_id, key_length, &failed_to_find,
                             key_id_p, key_allocation_id_p));

            if (failed_to_find == DNX_FIELD_KEY_FAILED_NO_FAILURE)
            {
                /*
                 * All is OK. Return with allocated key.
                 */
                break;
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_stringify_failed_to_find
                            (unit, fg_type, context_id, key_length, field_stage, failed_to_find));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                         dnx_field_stage_e_get_name(field_stage));
            break;
        }
    }
    if ((unsigned int) (key_id_p->id[0]) >= (unsigned int) DBAL_NOF_ENUM_FIELD_KEY_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "For field_stage=%d FG_type=%d context_id=%d, key[0] was not allocated due to internal error\r\n",
                     field_stage, fg_type, context_id);
    }
    if (key_length > DNX_FIELD_KEY_LENGTH_TYPE_SINGLE)
    {
        if ((unsigned int) (key_id_p->id[1]) >= DBAL_NOF_ENUM_FIELD_KEY_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For field_stage=%d FG_type=%d context_id=%d, key[1] was not allocated due to internal error\r\n",
                         field_stage, fg_type, context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  'Bit-range' specific:
 *  Analyze 'failed_to_find' as per dnx_field_key_failed_to_find_e and print
 *  corresponding text. Important: It is assumed that 'failed_to_find'
 *  is NOT DNX_FIELD_KEY_FAILED_NO_FAILURE.
 * \param [in] unit -
 *   Device Id
 * \param [in] fg_type -
 *   Field group type corresponding to failure to find free key.
 * \param [in] context_id -
 *   Context ID corresponding to failure to find free key.
 * \param [in] key_length -
 *   Specified size o required bit-range within key corresponding to failure to find free key.
 * \param [in] field_stage -
 *   Field stage corresponding to failure to find free key.
 * \param [in] failed_to_find -
 *   Enum describing reason for failure to find free key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_field_stringify_bit_range_failed_to_find(
    int unit,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    uint8 key_length,
    dnx_field_stage_e field_stage,
    dnx_field_key_failed_to_find_e failed_to_find)
{
    SHR_FUNC_INIT_VARS(unit);

    if (failed_to_find == DNX_FIELD_KEY_FAILED_NO_FAILURE)
    {
        /*
         * All is OK. Return with allocated key.
         */
        SHR_EXIT();
    }
    switch (failed_to_find)
    {
        case DNX_FIELD_KEY_FAILED_NO_PLACE:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No free key: All keys occupied for fg_type %d (%s), stage %d (%s), PMF program %d, key_length %d\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length);
            break;
        }
        case DNX_FIELD_KEY_FAILED_NOT_INITIALIZED:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No keys were made available for fg_type %d (%s), stage %d (%s), PMF program %d, key_length %d\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length);
            break;
        }
        case DNX_FIELD_KEY_FAILED_NOT_ASSIGNED_TO_FG:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "Key configuration has not yet been assigned for fg_type %d (%s), stage %d (%s), PMF program %d, key_length %d\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length);
            break;
        }
        case DNX_FIELD_KEY_FAILED_FG_HAS_ZERO_KEYS:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No key found because configuration is that specified field group type has zero\r\n"
                         "keys for fg_type %d (%s), stage %d (%s), PMF program %d, key_length %d\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type),
                         field_stage, dnx_field_stage_e_get_name(field_stage), context_id, key_length);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "\r\n"
                         "No keys were made available for fg_type %d, stage %d, PMF program %d, key_length %d\n",
                         fg_type, field_stage, context_id, key_length);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_bit_range_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    uint8 key_length,
    uint32 do_align,
    int16 aligned_bit,
    dnx_field_key_id_t *key_id_p,
    uint32 *bit_range_offset_within_key_p)
{
    dnx_field_key_failed_to_find_e failed_to_find;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    failed_to_find = DNX_FIELD_KEY_FAILED_NO_FAILURE;

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF3:
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_bit_range_alloc_generic
                            (unit, fg_type, field_stage, context_id, key_length, do_align, aligned_bit, &failed_to_find,
                             key_id_p, bit_range_offset_within_key_p));

            if (failed_to_find == DNX_FIELD_KEY_FAILED_NO_FAILURE)
            {
                /*
                 * All is OK. Return with allocated key.
                 */
                break;
            }
            SHR_IF_ERR_EXIT(dnx_algo_field_stringify_bit_range_failed_to_find
                            (unit, fg_type, context_id, key_length, field_stage, failed_to_find));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal field_stage=%d (%s)\n", field_stage,
                         dnx_field_stage_e_get_name(field_stage));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_bit_range_free(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_context_t context_id,
    dnx_field_key_id_t key_id,
    uint32 bit_range_offset_within_key)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF3:
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_bit_range_free_generic
                            (unit, fg_type, field_stage, context_id, key_id, bit_range_offset_within_key));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                         dnx_field_stage_e_get_name(field_stage));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See H file algo_field.h
 */
shr_error_e
dnx_algo_field_key_id_free(
    int unit,
    uint32 key_allocation_id)
{
    /*
     * Essentially, the key_allocation_id contains, encoded, the following parameters:
     */
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    dnx_field_context_t context_id;
    dnx_field_key_id_t key_id;

    SHR_FUNC_INIT_VARS(unit);

    DECONSTRUCT_KEY_ALLOCATION_ID(key_allocation_id, fg_type, field_stage, context_id, key_id.id[0], key_id.id[1],
                                  key_id.key_part);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF3:
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_free_generic(unit, fg_type, field_stage, context_id, &key_id));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_stage=%d (%s) \r\n", field_stage,
                         dnx_field_stage_e_get_name(field_stage));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_1_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint8 ranges,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.allocate_single(unit, context_id, alloc_flags, &ranges, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf_3_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_epmf_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ifwd2_ffc_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.allocate_single(unit, context_id, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ifwd2_ffc_num_of_elements_get(
    int unit,
    dnx_field_context_t context_id,
    int *nof_elements_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_elements_p, _SHR_E_PARAM, "nof_elements_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.nof_free_elements_get(unit, context_id, nof_elements_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint32 qual_size,
    int *initial_offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_offset_p, _SHR_E_PARAM, "initial_offset_p");

    return algo_field_info_sw.ipmf1_initial_key_f_occ_bmp.allocate_several(unit, context_id, alloc_flags,
                                                                           qual_size, NULL, initial_offset_p);

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint32 qual_size,
    int *initial_offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_offset_p, _SHR_E_PARAM, "initial_offset_p");

    return algo_field_info_sw.ipmf1_initial_key_g_occ_bmp.allocate_several(unit, context_id, alloc_flags,
                                                                           qual_size, NULL, initial_offset_p);

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint32 qual_size,
    int *initial_offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_offset_p, _SHR_E_PARAM, "initial_offset_p");

    return algo_field_info_sw.ipmf1_initial_key_h_occ_bmp.allocate_several(unit, context_id, alloc_flags,
                                                                           qual_size, NULL, initial_offset_p);

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint32 qual_size,
    int *initial_offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_offset_p, _SHR_E_PARAM, "initial_offset_p");

    return algo_field_info_sw.ipmf1_initial_key_i_occ_bmp.allocate_several(unit, context_id, alloc_flags,
                                                                           qual_size, NULL, initial_offset_p);

exit:
    SHR_FUNC_EXIT;
}

/**
* see H file algo_field.h
*/
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    uint32 qual_size,
    int *initial_offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_offset_p, _SHR_E_PARAM, "initial_offset_p");

    /*
     * res_alloc_name = DNX_ALGO_FIELD_IPMF1_INITIAL_KEY_J_OCC_BMP;
     * 
     * return dnx_algo_res_allocate_several (unit, BCM_CORE_ALL, context_id, res_alloc_name, alloc_flags, qual_size,
     * NULL, initial_offset_p);
     */
    return algo_field_info_sw.ipmf1_initial_key_j_occ_bmp.allocate_several(unit, context_id, alloc_flags,
                                                                           qual_size, NULL, initial_offset_p);

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int initial_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_f_occ_bmp.free_several(unit, context_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int initial_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_g_occ_bmp.free_several(unit, context_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int initial_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_h_occ_bmp.free_several(unit, context_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int initial_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_i_occ_bmp.free_several(unit, context_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_deallocate(
    int unit,
    dnx_field_context_t context_id,
    uint32 size,
    int initial_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    ipmf1_initial_key_j_occ_bmp.free_several(unit, context_id, size, initial_offset));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_qual_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_qual_id_t * user_qual_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_qual_id_p, _SHR_E_PARAM, "user_qual_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_qual_id.allocate_single(unit, alloc_flags, NULL, (int *) user_qual_id_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_user_action_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_action_id_t * user_action_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_action_id_p, _SHR_E_PARAM, "user_action_id_p");

    SHR_IF_ERR_EXIT(algo_field_info_sw.
                    user_action_id.allocate_single(unit, alloc_flags, NULL, (int *) user_action_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_1_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_2_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}
/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ipmf_3_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_3_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_epmf_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.epmf_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_key_ifwd2_ffc_deallocate(
    int unit,
    dnx_field_context_t context_id,
    int ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ifwd2_ffc.free_single(unit, context_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}
/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_qual_id_deallocate(
    int unit,
    dnx_field_qual_id_t user_qual_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_qual_id.free_single(unit, user_qual_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_user_action_id_deallocate(
    int unit,
    dnx_field_action_id_t user_action_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.user_action_id.free_single(unit, user_action_id));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_compare_key_id_get(
    int unit,
    dnx_field_context_compare_mode_pair_e compare_pair,
    dnx_field_key_id_t *key_id_1_p,
    dnx_field_key_id_t *key_id_2_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Compare uses KEY F,G as first pair, and H,I as second pair
     */
    switch (compare_pair)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1:
            /** First pair gets F and G keys,*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_F;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_G;
            break;
        case DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_2:
            /** Second pair gets H and I keys*/
            key_id_1_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_H;
            key_id_2_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid compare key pair %d", compare_pair);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_hash_key_id_get(
    int unit,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    
    key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_I;
    key_id_p->id[1] = DBAL_ENUM_FVAL_FIELD_KEY_J;

exit:
    SHR_FUNC_EXIT;
}

/**see H file: algo_field.h */
shr_error_e
dnx_algo_field_state_table_key_id_get(
    int unit,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * State table uses initial key J.
     */
    key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_J;
    key_id_p->id[1] = DNX_FIELD_KEY_ID_INVALID;

exit:
    SHR_FUNC_EXIT;
}
