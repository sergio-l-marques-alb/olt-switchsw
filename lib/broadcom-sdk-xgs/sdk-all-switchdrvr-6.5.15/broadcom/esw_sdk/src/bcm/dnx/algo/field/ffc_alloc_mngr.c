/** \file ffc_alloc_mngr.c
 *
 * Implementation for advanced ffc resource manager.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <bcm_int/dnx/algo/field/ffc_alloc_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_access.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#define DNX_FFC_REGION(i) (DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE * (i))

shr_error_e
dnx_algo_ffc_ipmf_1_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    uint8 tag = TRUE;
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create the ffc bitmap.
     */
    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.low_id = 0;
    res_tag_bitmap_create_info.count = DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT * DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;
    res_tag_bitmap_create_info.grain_size = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;
    res_tag_bitmap_create_info.max_tag_value = TRUE;
    res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;

    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_create_info, nof_elements, alloc_flags));

    /*
     * Tag whole region as available.
     */
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set(unit, module_id, *res_tag_bitmap, tag, FALSE,
                                                         0,
                                                         DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE *
                                                         DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_ffc_ipmf_1_bitmap_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *ranges_v,
    int *element)
{
    uint8 *ranges;
    uint32 tag;
    uint32 enable_tag = TRUE;
    uint32 disable_tag = FALSE;
    int ii;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    ranges = (uint8 *) ranges_v;

    

    /*
     * Loop on available ranges and set enable/disable tag to enable/disable the current range
     * according to input from field_map.c
     */
    for (ii = 0; ii < DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT; ii++)
    {
        tag = (*ranges & SAL_BIT(ii)) ? enable_tag : disable_tag;

        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set
                        (unit, module_id, res_tag_bitmap, tag, FALSE, DNX_FFC_REGION(ii),
                         DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE));
    }

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(res_tag_bitmap_alloc_info));
    res_tag_bitmap_alloc_info.tag = enable_tag;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.flags = flags | SW_STATE_RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_ffc_ipmf_1_alloc_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t create_data;
    uint32 sub_resource_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /*
     * Set bitmap info
     */
    create_data.flags = SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE;
    create_data.first_element = 0;
    create_data.nof_resource_pool_indexes =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
    create_data.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE * DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT;
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FFC;
    sal_strncpy(create_data.name, DNX_ALGO_FIELD_IPMF_1_FFC, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create ffc bitmap
     */

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.alloc(unit, create_data.nof_resource_pool_indexes));
    for (sub_resource_id = 0; sub_resource_id < create_data.nof_resource_pool_indexes; sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.create(unit, sub_resource_id, &create_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}
