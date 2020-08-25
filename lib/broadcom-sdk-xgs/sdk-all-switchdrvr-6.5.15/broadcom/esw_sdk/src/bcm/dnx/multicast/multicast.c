/** \file src/bcm/dnx/multicast/multicast.c
 * $Id$
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/bier/bier.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx_dispatch.h>

#include "multicast_imp.h"
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

/**
 * \brief - verify that multicast group at the soc property range
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_mc_id_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_MULTICAST_IS_INGRESS_GROUP(flags) && (group > dnx_data_multicast.params.max_ing_mc_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress multicast ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.max_ing_mc_groups_get(unit));
    }
    if (DNX_MULTICAST_IS_EGRESS_GROUP(flags) && (group > dnx_data_multicast.params.max_egr_mc_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress multicast ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.max_egr_mc_groups_get(unit));
    }
    if (DNX_MULTICAST_IS_EGRESS_BITMAP(flags) && (group >= dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress bitmap ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit));
    }
    if (DNX_MULTICAST_IS_INGRESS_BITMAP(flags) && (group >= dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress bitmap ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_create()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group_id - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *  this verifies that the correct flags are used and that group id in appropriate range and that the group is not open
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_create_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t * group_id)
{
    uint32 is_open;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_id, _SHR_E_PARAM, "group_id");

    /*
     * Multicast without ID is not supported 
     */
    if (!(flags & BCM_MULTICAST_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MULTICAST_WITH_ID must be used\n");
    }

    /*
     * one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API 
     */
    if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * BIER verification
     */
    if (DNX_MULTICAST_IS_BIER(flags))
    {
        SHR_INVOKE_VERIFY_DNX(dnx_multicast_bfr_id_verify(unit, flags, *group_id));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_mc_id_verify(unit, flags, *group_id));

    /*
     * verify that the group is not open
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, *group_id, flags, &is_open));
    if (is_open)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Multicast ID is already created\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_destroy()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group_id - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *  this verifies that the group id in appropriate range and that the group/bitmap is open
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_destroy_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group_id)
{
    uint32 group_found = FALSE, group_is_open;
    int set_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * bitmap
     */
    if (DNX_MULTICAST_IS_BITMAP(flags))
    {
        if (group_id <= dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                            (unit, group_id, flags | BCM_MULTICAST_INGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }
        if (group_id <= dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                            (unit, group_id, flags | BCM_MULTICAST_EGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }
    }
    else
        /*
         * link list
         */
    {
        /*
         * ingress and egress multicast group are destroyed together cause flags doesn't have ing/egr indication 
         */
        if (group_id <= dnx_data_multicast.params.max_ing_mc_groups_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, BCM_MULTICAST_INGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }
        if (group_id <= dnx_data_multicast.params.max_egr_mc_groups_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, BCM_MULTICAST_EGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }

        /*
         * BIER
         */
        if (group_id < dnx_data_bier.params.nof_bfr_entries_get(unit) * dnx_data_bier.params.bfr_entry_size_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group_id, &set_size));
            group_found |= set_size ? TRUE : FALSE;
        }
    }

    /*
     * verify that at least one of the groups are open
     */
    if (group_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "group ID (%u) is not created\n", group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_add/remove/set/get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   this verifies that the the number of replications are bigger than zero and that
 *   the group id in appropriate range and that the group is open and that the flags are valid
 *   the bitmap/group creation is always with it so BCM_MULTICAST_WITH_ID always need to be used.
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_add_remove_verify(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint32 is_open;
    bcm_multicast_t group_id;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_replications > 0)
    {
        SHR_NULL_CHECK(rep_array, _SHR_E_PARAM, "replication array");
    }

    /*
     * verify that only supported flags are set
     */
    if (!_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_REMOVE_ALL | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags (0x%x)\n", flags);
    }

    /*
     * verify that number of replications are bigger than zero
     */
    if (nof_replications <= 0 && !(flags & BCM_MULTICAST_REMOVE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number of replications (%d) must be bigger than zero\n", nof_replications);
    }

    /*
     * adding bitmap flag for internal group verification
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        flags |= BCM_MULTICAST_TYPE_PORTS_GROUP;
    }

    group_id = _BCM_MULTICAST_ID_GET(group);
    SHR_INVOKE_VERIFY_DNX(dnx_multicast_mc_id_verify(unit, flags, group_id));

    /*
     * verify that one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags used with this API
     */
    if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * BCM_MULTICAST_EGRESS_GROUP & BCM_MULTICAST_INGRESS_GROUP flags should not be used together
     */
    if (DNX_MULTICAST_IS_INGRESS(flags) && DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * verify that the group is open
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, flags, &is_open));
    if (is_open == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Multicast ID is not created\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - creates multicast empty (ingress/egress) group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group - group/bitmap id.
 *   \param [in] core_id - core_id
 *
 * \remark
 * allocate an mcdb entry and move the existing one if needed
 * \see
 *   * None
 */
static int
dnx_multicast_create_allocation(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 core_id)
{
    uint8 is_allocated = FALSE;
    uint32 entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if desired entry already allocated, relocate the existing entry otherwise allocate it
     */
    SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, DNX_MULTICAST_IS_BITMAP(flags),
                                              DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_id));
    SHR_IF_ERR_EXIT(multicast_db.mcdb.is_allocated(unit, entry_id, &is_allocated));
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_entry_relocate(unit, flags, core_id, entry_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, TRUE, &entry_id));
    }

    /*
     * clear entry
     */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, entry_id, DNX_MULTICAST_IS_BITMAP(flags)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - creates multicast empty (ingress/egress) group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id. see remarks
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   the bitmap/group creation is always with it so BCM_MULTICAST_WITH_ID always need to be used.
 *   for bitmap the group encapsulated with bitmap indication. this way other API's will recognize this group as bitmap
 * \see
 *   * None
 */
int
bcm_dnx_multicast_create(
    int unit,
    uint32 flags,
    bcm_multicast_t * group)
{
    uint32 core_id;
    uint32 is_bmp = FALSE;
    uint32 bmp_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_create_verify(unit, flags, group));

    /*
     * create bier group
     */
    if (DNX_MULTICAST_IS_BIER(flags))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bier_create(unit, flags, *group));

        /*
         * mark group as open. HW for ingress & SW for egress
         */
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, *group, flags, TRUE));
        SHR_EXIT();
    }

    /*
     * set the representation for bitmap 
     */
    if (DNX_MULTICAST_IS_BITMAP(flags))
    {
        is_bmp = TRUE;
        bmp_flags = BCM_MULTICAST_TYPE_PORTS_GROUP;
    }

    /*
     * allocation
     */
    if (DNX_MULTICAST_IS_INGRESS(flags))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_create_allocation(unit, BCM_MULTICAST_INGRESS_GROUP | bmp_flags, *group, 0));
    }
    if (DNX_MULTICAST_IS_EGRESS(flags))
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_create_allocation
                            (unit, BCM_MULTICAST_EGRESS_GROUP | bmp_flags, *group, core_id));
        }
    }

    /*
     * mark group as open. HW for ingress & SW for egress 
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, *group, flags, TRUE));

    /*
     * set the representation for bitmap 
     */
    if (is_bmp == TRUE)
    {
        bmp_flags = 0;
        /** set the representation for bitmap */
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            bmp_flags |= _BCM_MULTICAST_TYPE_ING_BMP;
        }
        if (DNX_MULTICAST_IS_EGRESS(flags))
        {
            bmp_flags |= _BCM_MULTICAST_TYPE_EGR_BMP;
        }
        _BCM_MULTICAST_GROUP_SET(*group, bmp_flags, *group);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy the ingress and egress multicast for bitmap/groups id.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id created at bcm_multicast_create API
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_destroy(
    int unit,
    bcm_multicast_t group)
{
    uint32 entry_id;
    uint32 core_id = 0;
    uint32 is_bmp = FALSE;
    uint32 bmp_flag = 0;
    uint32 is_open = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        is_bmp = TRUE;
        bmp_flag = BCM_MULTICAST_TYPE_PORTS_GROUP;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_destroy_verify(unit, bmp_flag, _BCM_MULTICAST_ID_GET(group)));

    /*
     * try to destroy bier group. if this is not an bier group continue to multicast
     */
    if (!is_bmp)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bier_destroy(unit, group));
    }

    /*
     * free egress link lists and close the group if open.
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                    (unit, _BCM_MULTICAST_ID_GET(group), bmp_flag | BCM_MULTICAST_EGRESS_GROUP, &is_open));
    if (_SHR_MULTICAST_TYPE_GET(group) != _BCM_MULTICAST_TYPE_ING_BMP && is_open)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_delete
                        (unit, group, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_REMOVE_ALL, 0, NULL));
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                            (unit, _BCM_MULTICAST_ID_GET(group), is_bmp, FALSE, core_id, &entry_id));
            SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, entry_id));
        }
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_set
                        (unit, _BCM_MULTICAST_ID_GET(group), bmp_flag | BCM_MULTICAST_EGRESS_GROUP, FALSE));
    }

    /*
     * free ingress link list and close the group if open.
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                    (unit, _BCM_MULTICAST_ID_GET(group), bmp_flag | BCM_MULTICAST_INGRESS_GROUP, &is_open));
    if (_SHR_MULTICAST_TYPE_GET(group) != _BCM_MULTICAST_TYPE_EGR_BMP && is_open)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_multicast_delete
                        (unit, group, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_REMOVE_ALL, 0, NULL));
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, _BCM_MULTICAST_ID_GET(group), is_bmp, TRUE, core_id, &entry_id));
        SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, entry_id));
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_set
                        (unit, _BCM_MULTICAST_ID_GET(group), bmp_flag | BCM_MULTICAST_INGRESS_GROUP, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the content (replications) of a multicast group/bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_remove_verify(unit, group, flags, nof_replications, rep_array));

    /*
     * handle bitmap case 
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_create
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, TRUE, nof_replications, rep_array));
    }
    else
    {
        /*
         * sort egress replications by CUD in order to use one entry for 2 replications with same cud
         */

        /*
         * next link list with replications created and the old link list is removed
         */
        SHR_IF_ERR_EXIT(dnx_multicast_linklist_create(unit, group, flags, TRUE, nof_replications, rep_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the content (replications) of a multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] max_replications - specifies the rep_array size
 *   \param [in] out_rep_array - pointer to the the group/bitmap replications array
 *   \param [in] rep_count - pointer to number of replications
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   \param [out] out_rep_array - pointer to the the group/bitmap replications array read from HW
 *   \param [out] rep_count - pointer to number of replications read from HW
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * out_rep_array,
    int *rep_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_remove_verify(unit, group, flags, max_replications, out_rep_array));

    /*
     * handle bitmap case 
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_get
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, max_replications, out_rep_array, rep_count));
    }
    else
    {
        /*
         * Go through the link list in order to get the replications
         */
        SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter(unit, group, flags, NULL, DNX_MULTICAST_ENTRY_ITER_GET,
                                                    max_replications, out_rep_array, rep_count));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adds multicast destinations for multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_add(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_remove_verify(unit, group, flags, nof_replications, rep_array));

    /*
     * handle bitmap case 
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_create
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, FALSE, nof_replications, rep_array));
    }
    else
    {
        /*
         * sort egress replications by CUD in order to use one entry for 2 replications with same cud
         */

        /*
         * create link list with new replications
         */
        SHR_IF_ERR_EXIT(dnx_multicast_linklist_create(unit, group, flags, FALSE, nof_replications, rep_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Removes multicast destinations from multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint32 action, rep_exist;
    int rep_count_out;
    bcm_core_t core_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_remove_verify(unit, group, flags, nof_replications, rep_array));

    action = (flags & BCM_MULTICAST_REMOVE_ALL) ? DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL :
        DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST;

    /*
     * Go through the link list in order to find the replications and delete them
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_delete(unit, _BCM_MULTICAST_ID_GET(group), flags,
                                                    action, nof_replications, rep_array));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter(unit, group, flags, NULL, action,
                                                    nof_replications, rep_array, &rep_count_out));
        /*
         * update replication per core for egress group
         */
        if (!DNX_MULTICAST_IS_INGRESS_GROUP(flags))
        {
            for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_exr_core_rep_exist(unit, group, core_id, &rep_exist));
                SHR_IF_ERR_EXIT(dnx_multicast_egr_core_enable_set(unit, group, core_id, rep_exist));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_multicast_stat_control_set()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id  
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_stat_control_set_verify(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int arg)
{
    uint32 legal_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL also allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    legal_flags = BCM_MULICAST_STAT_EXTERNAL;

    if ((flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }
    if (type == bcmMulticastStatControlCountAllCopiesAsOne)
    {
        if (arg != FALSE && arg != TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal arg (=%d) value", arg);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_multicast_stat_control_get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id 
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_stat_control_get_verify(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int *arg)
{
    uint32 legal_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL not allowed for get API*/
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    legal_flags = BCM_MULICAST_STAT_EXTERNAL;

    if ((flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - statistics control API, related to gather statistics on multicast packets in CRPS/STIF.
 *      currently refer to ingress multicast only.
 *      valid types: 
 *      1. bcmMulticastStatControlCountAllCopiesAsOne - count all copies or just original. (for CRPS configured per command_id, for STIF single configuration)
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id 
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_stat_control_set(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_stat_control_set_verify(unit, core_id, flags, command_id, type, arg));

    switch (type)
    {
        case bcmMulticastStatControlCountAllCopiesAsOne:
            SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_set(unit, core_id, flags, command_id, arg));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - statistics control API, related to gather statistics on multicast packets in CRPS/STIF.
 *      currently refer to ingress multicast only.
 *      valid types: 
 *      1. bcmMulticastStatControlCountAllCopiesAsOne - count all copies or just original. (for CRPS configured per command_id, for STIF single configuration)
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id
 *   \param [in] type - type of control 
 *   \param [out] arg - arg of control
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_stat_control_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_stat_control_get_verify(unit, core_id, flags, command_id, type, arg));

    switch (type)
    {
        case bcmMulticastStatControlCountAllCopiesAsOne:
            SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_get(unit, core_id, flags, command_id, arg));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;

}
