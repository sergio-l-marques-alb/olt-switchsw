/** \file l2_traverse.c
 * $Id$
 *
 * L2 procedures for DNX.
 *
 * This file contains functions for using the traverse machine.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l2/l2_addr.h>
#include <bcm/l2.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <src/soc/dnx/flush/flush_internal.h>
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */
/*
 * Defines.
 * {
 */
#define DNX_L2_MATCH_STATIC                 (1 << 0)    /* Match L2 static entries, all the strength values that are
                                                         * considered static. */
#define DNX_L2_MATCH_DYNAMIC                (1 << 1)    /* Match L2 dynamic entries, all the strength values that are
                                                         * considered dynamic. */
#define DNX_L2_MATCH_MAC                    (1 << 2)    /* Match L2 entries with given MAC address */
#define DNX_L2_MATCH_VSI                    (1 << 3)    /* Match L2 entries with given VSI */
#define DNX_L2_MATCH_DEST                   (1 << 4)    /* Match L2 entries with given modid port/tgid */
#define DNX_L2_MATCH_HIT                    (1 << 5)    /* Match entries according to the hit-bit indication */
#define DNX_L2_MATCH_AGE                    (1 << 6)    /* Match entries of a given age */
#define DNX_L2_REPLACE_DELETE               (1 << 7)    /* Delete the matching entries */
#define DNX_L2_MATCH_GROUP                  (1 << 8)    /* Match entries according to group field */
#define DNX_L2_MATCH_IVL                    (1 << 9)    /* Match entries from the IVL table */

#define DNX_L2_REPLACE_DES_HIT_CLEAR        (1 << 11)   /* Clear Hit-bit of matched entries. */
#define DNX_L2_REPLACE_DYNAMIC_SET          (1 << 12)   /* Change the matched entries to be dynamic */
#define DNX_L2_REPLACE_STATIC_SET           (1 << 13)   /* Change the matched entries to be static */
#define DNX_L2_REPLACE_DESTINATION          (1 << 14)   /* Change the destination of the matched entries */
#define DNX_L2_NO_CALLBACKS                 (1 << 15)   /* Don't call user callback for returned entries */

/** Define currently supported traverse flags */
#define L2_TRAVERSE_SUPPORTED_BCM_FLAGS  (BCM_L2_TRAVERSE_MATCH_STATIC | BCM_L2_TRAVERSE_MATCH_MAC | BCM_L2_TRAVERSE_MATCH_VLAN | \
                                          BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_IGNORE_DES_HIT | BCM_L2_TRAVERSE_MATCH_GROUP | \
                                          BCM_L2_TRAVERSE_MATCH_IVL)

/** Define currently supported replace flags */
#define L2_REPLACE_SUPPORTED_BCM_FLAGS  (BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN | \
                                          BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_AGE | \
                                          BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_DES_HIT_CLEAR | BCM_L2_REPLACE_DYNAMIC_SET | \
                                          BCM_L2_REPLACE_DYNAMIC_CLEAR | BCM_L2_REPLACE_MIGRATE_PORT | BCM_L2_REPLACE_NEW_TRUNK | \
                                          BCM_L2_REPLACE_PROTECTION_RING | BCM_L2_REPLACE_NO_CALLBACKS | BCM_L2_REPLACE_MATCH_GROUP | \
                                          BCM_L2_REPLACE_VLAN_AND_VPN_TYPE)

#define L2_DELETE_SUPPORTED_BCM_FLAGS  (BCM_L2_DELETE_NO_CALLBACKS | BCM_L2_DELETE_STATIC)

#define FLUSH_INVALID_OPERATION -1

/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/**
 * \brief - Init the DMA channel of the flush machine
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_flush_dma_init(
    int unit)
{
    int flush_channel;
    soc_dnx_fifodma_config_t fifodma_info;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if we are during warmboot the is_started may be incorrect, it 
     * may have been turned off during init, but this information 
     * was not synced to the external storage and this info was 
     * overriten with incorrect info during warmboot recovery
     */
    if (!SOC_WARM_BOOT(unit))
    {
        /** Stop and free the channel if it is already started */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_event_fifo, &is_started));

        if (is_started)
        {
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &flush_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_stop(unit, flush_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, flush_channel));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, soc_dnx_fifodma_src_event_fifo, &flush_channel));
    /*
     * Set initial values into 'fifodma_info' before loading new meaningful
     * data in some of the elements.
     */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &fifodma_info));
    fifodma_info.copyno = REG_PORT_ANY;
    fifodma_info.force_entry_size = 0;
    fifodma_info.is_mem = 0;
    fifodma_info.is_poll = 1;
    fifodma_info.max_entries = dnx_data_l2.dma.flush_nof_dma_entries_get(unit);;
    fifodma_info.reg = MACT_LARGE_EM_REPLYr;
    fifodma_info.threshold = 0;
    fifodma_info.timeout = 0;

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start(unit, flush_channel, &fifodma_info, NULL));

    /** Pause the flush DMA */
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_pause(unit, flush_channel));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Stop and free the dma channel of the flush
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_flush_dma_deinit(
    int unit)
{
    int olp_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_event_fifo, &olp_channel));

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, olp_channel));

exit:
    SHR_FUNC_EXIT;
}

/** Set the flags in the bcm_l2_addr_t structs according to the replace flags */
static shr_error_e
dnx_l2_traverse_format_addresses(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_addr,
    bcm_l2_addr_t * mask_addr,
    bcm_l2_addr_t * replace_l2addr,
    bcm_l2_addr_t * replace_mask_l2addr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_L2_REPLACE_NEW_TRUNK)
    {
        match_addr->flags |= BCM_L2_TRUNK_MEMBER;
        mask_addr->flags |= BCM_L2_TRUNK_MEMBER;
        replace_l2addr->flags |= BCM_L2_TRUNK_MEMBER;
        replace_mask_l2addr->flags |= BCM_L2_TRUNK_MEMBER;

        /** Make sure that trunk and multicast flags are not set in the same time */
        if (match_addr->flags & BCM_L2_MCAST || mask_addr->flags & BCM_L2_MCAST ||
            replace_l2addr->flags & BCM_L2_MCAST || replace_mask_l2addr->flags & BCM_L2_MCAST)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "bcm_l2_addr_t flags are not in synch with the replace flags. BCM_L2_REPLACE_NEW_TRUNK can't be used with BCM_L2_MCAST in bcm_l2_addr_t.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify that the traverse flags used are supported
 * 
 * \param [in] unit - unit id
 * \param [in] traverse_flags - flags to verify
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_traverse_flags_verify(
    int unit,
    uint32 traverse_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(traverse_flags, ~L2_TRAVERSE_SUPPORTED_BCM_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal traverse flags are used.");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify that the replace flags used are supported
 * 
 * \param [in] unit - unit id
 * \param [in] replace_flags - flags to verify
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_replace_flags_verify(
    int unit,
    uint32 replace_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(replace_flags, ~L2_REPLACE_SUPPORTED_BCM_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal replace flags are used.");
    }

    /** Make sure that replace and delete are not set together */
    if ((replace_flags & BCM_L2_REPLACE_DELETE) &&
        (replace_flags & (BCM_L2_REPLACE_DYNAMIC_CLEAR | BCM_L2_REPLACE_DYNAMIC_SET | BCM_L2_REPLACE_NEW_TRUNK |
                          BCM_L2_REPLACE_DES_HIT_CLEAR | BCM_L2_REPLACE_MIGRATE_PORT | BCM_L2_REPLACE_PROTECTION_RING)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal replace flags are used. Replcae and delete can't be set together.");
    }

    /** Make sure that dynamic set and dynamic clear are not set together */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC_SET) && (replace_flags & BCM_L2_REPLACE_DYNAMIC_CLEAR))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal replace flags are used. BCM_L2_REPLACE_DYNAMIC_CLEAR and BCM_L2_REPLACE_DYNAMIC_SET can't be set together.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify traverse parameters
 * 
 * \param [in] unit - unit id
 * \param [in] flags - traverse flags
 * \param [in] match_l2addr - l2 address for filter rule
 * \param [in] match_mask_l2addr - l2 address used as a mask for the filter
 * \param [in] replace_l2addr - l2 address for replace data
 * \param [in] replace_mask_l2addr - l2 address for mast to the replace data
 * \param [in] trav_fn - user callback
 * \param [in] user_data - context sent to the callback
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_traverse_verify(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_l2addr,
    bcm_l2_addr_t * match_mask_l2addr,
    bcm_l2_addr_t * replace_l2addr,
    bcm_l2_addr_t * replace_mask_l2addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** match_l2addr can't be NULL */
    SHR_NULL_CHECK(match_l2addr, _SHR_E_PARAM, "match_l2addr");

    /** Verify traverse flags */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_traverse_flags_verify(unit, flags));

    if ((match_l2addr->modid != 0) && ((flags & BCM_L2_TRAVERSE_MATCH_IVL) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid!=0 requires the use of BCM_L2_TRAVERSE_MATCH_IVL.");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify replace parameters
 * 
 * \param [in] unit - unit id
 * \param [in] flags - traverse flags
 * \param [in] match_l2addr - l2 address for filter rule
 * \param [in] match_mask_l2addr - l2 address used as a mask for the filter
 * \param [in] replace_l2addr - l2 address for replace data
 * \param [in] replace_mask_l2addr - l2 address for mast to the replace data
 * \param [in] trav_fn - user callback
 * \param [in] user_data - context sent to the callback
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_l2_replace_verify(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_l2addr,
    bcm_l2_addr_t * match_mask_l2addr,
    bcm_l2_addr_t * replace_l2addr,
    bcm_l2_addr_t * replace_mask_l2addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** match_l2addr can't be NULL */
    SHR_NULL_CHECK(match_l2addr, _SHR_E_PARAM, "match_l2addr");

    if ((flags & BCM_L2_REPLACE_NO_CALLBACKS) && trav_fn)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_L2_REPLACE_NO_CALLBACKS can't be added when a callback is set.");
    }

    if (flags & (BCM_L2_REPLACE_PROTECTION_RING | BCM_L2_REPLACE_NEW_TRUNK | BCM_L2_REPLACE_MIGRATE_PORT))
    {
        SHR_NULL_CHECK(replace_l2addr, _SHR_E_PARAM, "replace_l2addr");
    }

    /** Verify replace flags */
    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_flags_verify(unit, flags));

    if ((match_l2addr->modid != 0) && ((flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid!=0 requires the use of BCM_L2_REPLACE_VLAN_AND_VPN_TYPE.");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_l2_delete_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, ~L2_DELETE_SUPPORTED_BCM_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Illegal flags are used. Only BCM_L2_DELETE_NO_CALLBACKS and BCM_L2_DELETE_STATIC are supported.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - transalte from BCM_L2_TRAVERSE_* flags to local traverse flags.
 * 
 * \param [in] unit - unit id 
 * \param [in] match_addr - use the match address in order to decide about the dynamic/static rule
 * \param [in] traverse_flags - BCM_L2_TRAVERSE_* flags used as an input for the translation.
 * \param [in,out] local_flags - set the bits of local_flags that coresponds to traverse_flags. Don't set the param to 0 in the beginning.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_traverse_flags_to_local_flags(
    int unit,
    bcm_l2_addr_t * match_addr,
    uint32 traverse_flags,
    uint32 *local_flags)
{
    uint8 set_static = 0;
    uint8 set_dynamic = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure the dynamic and static rule dynamic logic: flags & BCM_L2_TRAVERSE_MATCH_STATIC == 0 &&
     * match_addr->flags & BCM_L2_STATIC == 0 --> match dynamic entry only flags & BCM_L2_TRAVERSE_MATCH_STATIC == 0 && 
     * match_addr->flags & BCM_L2_STATIC == 1 -->match static entry only flags & BCM_L2_TRAVERSE_MATCH_STATIC == 1
     * -->match both static and dynamic entries 
     */
    if (traverse_flags & BCM_L2_TRAVERSE_MATCH_STATIC || match_addr == NULL)
    {
        set_static = 1;
        set_dynamic = 1;
    }
    else
    {
        if (match_addr->flags & BCM_L2_STATIC)
        {
            set_static = 1;
        }
        else
        {
            set_dynamic = 1;
        }
    }

    *local_flags |= (set_static ? DNX_L2_MATCH_STATIC : 0);
    *local_flags |= (set_dynamic ? DNX_L2_MATCH_DYNAMIC : 0);
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_MATCH_MAC) ? DNX_L2_MATCH_MAC : 0);
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_MATCH_VLAN) ? DNX_L2_MATCH_VSI : 0);
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_MATCH_DEST) ? DNX_L2_MATCH_DEST : 0);
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_MATCH_GROUP) ? DNX_L2_MATCH_GROUP : 0);
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_MATCH_IVL) ? DNX_L2_MATCH_IVL : 0);

    /** Hit bit is not supported in adapter mode */
#ifndef ADAPTER_SERVER_MODE
    *local_flags |= ((traverse_flags & BCM_L2_TRAVERSE_IGNORE_DES_HIT) ? 0 : DNX_L2_MATCH_HIT);
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - transalte from BCM_L2_REPLACE_* flags to local traverse flags.
 * 
 * \param [in] unit - unit id 
 * \param [in] match_addr - use the match address in order to decide about the dynamic/static rule 
 * \param [in] replace_flags - BCM_L2_REPLACE_* flags used as an input for the translation.
 * \param [in,out] local_flags - set the bits of local_flags that coresponds to replace_flags. Don't set the param to 0 in the beginning.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_replace_flags_to_local_flags(
    int unit,
    bcm_l2_addr_t * match_addr,
    uint32 replace_flags,
    uint32 *local_flags)
{
    uint8 set_static = 0;
    uint8 set_dynamic = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure the dynamic and static rule dynamic logic: flags & BCM_L2_REPLACE_MATCH_STATIC == 0 &&
     * match_addr->flags & BCM_L2_STATIC == 0 --> match dynamic entry only flags & BCM_L2_REPLACE_MATCH_STATIC == 0 &&
     * match_addr->flags & BCM_L2_STATIC == 1 -->match static entry only flags & BCM_L2_REPLACE_MATCH_STATIC == 1
     * -->match both static and dynamic entries 
     */
    if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC || match_addr == NULL)
    {
        set_static = 1;
        set_dynamic = 1;
    }
    else
    {
        if (match_addr->flags & BCM_L2_STATIC)
        {
            set_static = 1;
        }
        else
        {
            set_dynamic = 1;
        }
    }

    *local_flags |= (set_static ? DNX_L2_MATCH_STATIC : 0);
    *local_flags |= (set_dynamic ? DNX_L2_MATCH_DYNAMIC : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_MATCH_MAC) ? DNX_L2_MATCH_MAC : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_MATCH_VLAN) ? DNX_L2_MATCH_VSI : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_MATCH_DEST) ? DNX_L2_MATCH_DEST : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_MATCH_AGE) ? DNX_L2_MATCH_AGE : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_MATCH_GROUP) ? DNX_L2_MATCH_GROUP : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) ? DNX_L2_MATCH_IVL : 0);

    /** Hit bit is not supported in adapter mode */
#ifndef ADAPTER_SERVER_MODE
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_IGNORE_DES_HIT) ? 0 : DNX_L2_MATCH_HIT);

    *local_flags |= ((replace_flags & BCM_L2_REPLACE_DES_HIT_CLEAR) ? DNX_L2_REPLACE_DES_HIT_CLEAR : 0);
#endif

    *local_flags |= ((replace_flags & BCM_L2_REPLACE_DELETE) ? DNX_L2_REPLACE_DELETE : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_DYNAMIC_SET) ? DNX_L2_REPLACE_DYNAMIC_SET : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_DYNAMIC_CLEAR) ? DNX_L2_REPLACE_STATIC_SET : 0);
    *local_flags |=
        ((replace_flags & (BCM_L2_REPLACE_MIGRATE_PORT | BCM_L2_REPLACE_NEW_TRUNK | BCM_L2_REPLACE_PROTECTION_RING)) ?
         DNX_L2_REPLACE_DESTINATION : 0);
    *local_flags |= ((replace_flags & BCM_L2_REPLACE_NO_CALLBACKS) ? DNX_L2_NO_CALLBACKS : 0);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set actions to the rule handle according to the replace structs and flags.
 * 
 * \param [in] unit - unit id
 * \param [in] flags - flags defining which actions should be performed. (DNX_L2_REPLACE_XX)
 * \param [in] replace_l2addr - data for the replace actions.
 * \param [in] replace_mask_l2addr - mask for the replace data. 
 * \param [in] rules_handle_id - iterator handle which will be configured with the actions
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_traverse_set_actions(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * replace_l2addr,
    bcm_l2_addr_t * replace_mask_l2addr,
    uint32 *rules_handle_id)
{
    uint32 destination, destination_mask;
    uint32 outlif = 0;
    uint32 outlif_mask = 0;
    uint32 result_type_dummy, result_type_mask_dummy;
    uint32 entry_strength;
    uint32 entry_strength_mask = 0xffffffff;
    uint8 no_mask = 0;
    uint32 group_mask;
    uint32 group;
    uint32 eei = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Configure actions according to the flags */

    /** Add delete action */
    if (flags & DNX_L2_REPLACE_DELETE)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, *rules_handle_id, DBAL_ITER_ACTION_DELETE));
    }

    if (!replace_mask_l2addr)
    {
        no_mask = 1;
        group_mask = 0;
    }
    else
    {
        group_mask = replace_mask_l2addr->group;
    }

    /** Update destination */
    if (flags & DNX_L2_REPLACE_DESTINATION)
    {
        SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest
                        (unit, replace_l2addr, &destination, &outlif, &result_type_dummy, &eei));

        if (no_mask)
        {
            destination_mask = 0x1FFFFF;
            outlif_mask = 0x3FFFFF;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest
                            (unit, replace_mask_l2addr, &destination_mask, &outlif_mask, &result_type_mask_dummy,
                             &eei));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, *rules_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_DESTINATION, INST_SINGLE,
                         &destination, &destination_mask));

        if (outlif_mask != 0)
        {
            /** Update outlif  */
            SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                            (unit, *rules_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                             &outlif, &outlif_mask));
        }
    }

    /** Clear hit bit */
    if (flags & DNX_L2_REPLACE_DES_HIT_CLEAR)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, *rules_handle_id, DBAL_ITER_ACTION_HIT_CLEAR));
    }

    /** Set dynamic */
    if (flags & DNX_L2_REPLACE_DYNAMIC_SET)
    {
        entry_strength = DYNAMIC_MACT_ENTRY_STRENGTH;
        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, *rules_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE,
                         &entry_strength, &entry_strength_mask));
    }

    /** Set static */
    if (flags & DNX_L2_REPLACE_STATIC_SET)
    {
        entry_strength = STATIC_MACT_ENTRY_STRENGTH;
        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, *rules_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE,
                         &entry_strength, &entry_strength_mask));
    }

    if (replace_l2addr && (group_mask != 0))
    {
        group = (uint32) replace_l2addr->group;
        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, *rules_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE,
                         &group, &group_mask));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get mask and values for traverse inside bcm_l2_addr_t struct and configure the iterator handle with the rules.
 * 
 * \param [in] unit - unit id
 * \param [in] flags - flags defining which values are relevant for the traverse filter. (DNX_L2_MATCH_XX)
 * \param [in] match_l2addr - filter rules
 * \param [in] match_mask_l2addr - mask for the filter rules 
 * \param [in] rules_handle_id - iterator handle which will be configured with the rules
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_traverse_set_rules(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_l2addr,
    bcm_l2_addr_t * match_mask_l2addr,
    uint32 *rules_handle_id)
{
    uint32 key_rule_value;
    uint32 key_rule_mask;
    bcm_mac_t mac_rule_value;
    bcm_mac_t mac_rule_mask;
    uint32 destination, destination_mask;
    uint32 outlif = 0;
    uint32 outlif_mask = 0;
    uint32 result_type_dummy, result_type_mask_dummy;
    uint8 hit_value = 0;
    /** Use this struct in case the function's argument is null */
    bcm_l2_addr_t match_mask_l2addr_local;
    uint8 no_mask = 0;
    uint32 eei = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Configure rules according to the flags */

    key_rule_value = 0;
    key_rule_mask = 0;

    if (!match_mask_l2addr)
    {
        sal_memset((void *) &match_mask_l2addr_local, 0xFF, sizeof(bcm_l2_addr_t));
        match_mask_l2addr = &match_mask_l2addr_local;
        no_mask = 1;
    }

    /** Configure the dynamic and static rule */
    if (flags & DNX_L2_MATCH_STATIC)
    {
        if (flags & DNX_L2_MATCH_DYNAMIC)
        {
            /** Return dynamic and static entries */
            key_rule_value = 0;
            key_rule_mask = 0;
        }
        else
        {
            /** Return only static entries (strength 2 or 3 == 1X)   */
            key_rule_value = STATIC_MACT_ENTRY_STRENGTH;
            key_rule_mask = STATIC_MACT_ENTRY_STRENGTH;
        }
    }
    else
    {
        /** Only dynamic entries return (strength 0 or 1 == 0X) */
        key_rule_value = 0;
        key_rule_mask = STATIC_MACT_ENTRY_STRENGTH;
    }
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, *rules_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                     &key_rule_value, &key_rule_mask));

    /** Compare the mac address if needed */
    if (flags & DNX_L2_MATCH_MAC)
    {
        sal_memcpy((void *) &mac_rule_value, &(match_l2addr->mac), sizeof(bcm_mac_t));
        sal_memcpy((void *) &mac_rule_mask, &(match_mask_l2addr->mac), sizeof(bcm_mac_t));
    }
    else
    {
        sal_memset(mac_rule_value, 0, sizeof(bcm_mac_t));
        sal_memset(mac_rule_mask, 0, sizeof(bcm_mac_t));
    }
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr8_rule_add
                    (unit, *rules_handle_id, DBAL_FIELD_L2_MAC, DBAL_CONDITION_EQUAL_TO, (uint8 *) &mac_rule_value,
                     (uint8 *) &mac_rule_mask));

    /** Compare vsi */
    if (flags & DNX_L2_MATCH_VSI)
    {
        key_rule_value = match_l2addr->vid;
        key_rule_mask = match_mask_l2addr->vid;
    }
    else
    {
        key_rule_mask = 0;
    }
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, *rules_handle_id, DBAL_FIELD_VSI, DBAL_CONDITION_EQUAL_TO, &key_rule_value, &key_rule_mask));

    if (flags & DNX_L2_MATCH_GROUP)
    {
        /** Compare group */
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, *rules_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         (uint32 *) &match_l2addr->group, (uint32 *) &match_mask_l2addr->group));
    }

    if ((flags & DNX_L2_MATCH_IVL) && (match_l2addr->modid != 0))
    {
        /** Compare vlan (modid) */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, *rules_handle_id, DBAL_FIELD_VID, DBAL_CONDITION_EQUAL_TO,
                         (uint32 *) &match_l2addr->modid, (uint32 *) &match_mask_l2addr->modid));
    }

    /** Compare destination */
    if (flags & DNX_L2_MATCH_DEST)
    {
        SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest
                        (unit, match_l2addr, &destination, &outlif, &result_type_dummy, &eei));

        if (no_mask)
        {
            destination_mask = 0xFFFFFFFF;
            outlif_mask = 0xFFFFFFFF;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_convert_l2_addr_to_dest
                            (unit, match_mask_l2addr, &destination_mask, &outlif_mask, &result_type_mask_dummy, &eei));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, *rules_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         &destination, &destination_mask));

        if ((outlif_mask != 0) && (result_type_dummy != DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_NO_OUTLIF))
        {
            /** Compare outlif if needed */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, *rules_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &outlif, &outlif_mask));
        }
    }

    /** Compare hit bit if needed */
    if ((flags & DNX_L2_MATCH_HIT) && (match_mask_l2addr->flags & BCM_L2_HIT))
    {
        if (match_l2addr->flags & BCM_L2_HIT)
        {
            hit_value = 1;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, *rules_handle_id,
                                                         (DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY |
                                                          DBAL_ENTRY_ATTR_HIT_GET), hit_value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Call user callbacks. Call the callback given in the traverse/replace APIs and to the callbacks registered 
 * using bcm_l2_addr_register 
 * 
 * \param [in] unit - unit id
 * \param [in] flags - local traverse and replace flags
 * \param [in] l2addr - fetched MAC entry for the user's callback
 * \param [in] trav_fn - user callback given to the traverse/replace APIs
 * \param [in] user_data - context given by the user and return as a parameter to the user's callback
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_traverse_callbacks_invoke(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * l2addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    int cb_index;
    int operation = FLUSH_INVALID_OPERATION;

    SHR_FUNC_INIT_VARS(unit);

    if (trav_fn)
    {
        SHR_IF_ERR_EXIT(trav_fn(unit, l2addr, user_data));
    }
    else
    {
        if (flags & DNX_L2_REPLACE_DELETE)
        {
            operation = BCM_L2_CALLBACK_DELETE;
        }
        else if (flags &
                 (DNX_L2_REPLACE_DES_HIT_CLEAR | DNX_L2_REPLACE_DYNAMIC_SET | DNX_L2_REPLACE_STATIC_SET |
                  DNX_L2_REPLACE_DESTINATION))
        {
            operation = BCM_L2_CALLBACK_MOVE_EVENT;
        }

        /** Make sure that no callbacks option is not set and replace or delete operations are done */
        if (~flags & DNX_L2_NO_CALLBACKS && operation != FLUSH_INVALID_OPERATION)
        {
            /** Call other user callbacks ?! */
            for (cb_index = 0; cb_index < DNX_L2_CB_MAX; cb_index++)
            {
                if (dnx_l2_cbs[unit].entry[cb_index].callback != NULL)
                {
                    dnx_l2_cbs[unit].entry[cb_index].callback(unit, l2addr, operation,
                                                              dnx_l2_cbs[unit].entry[cb_index].userdata);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - impelement traverse and replace logic according to match filters and flags.
 * 
 * \param [in] unit - unit id
 * \param [in] flags - DNX_L2_MATCH_... and DNX_L2_REPLAC_... see more details in the flags definitions.
 * \param [in] match_l2addr - details for the filter rules. Use fields according to the flags parameter.
 * \param [in] match_mask_l2addr - mask for the filter rules.
 * \param [in] replace_l2addr - new values for transplant operation. Transplant the parameters according to the flags param.
 * \param [in] replace_mask_l2addr - mask for the replace_l2addr parameter.
 * \param [in] trav_fn - user callback to apply on all the matched entries.
 * \param [in] user_data - context passed to the user's callback. 
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_addr_traverse_by_rule(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_l2addr,
    bcm_l2_addr_t * match_mask_l2addr,
    bcm_l2_addr_t * replace_l2addr,
    bcm_l2_addr_t * replace_mask_l2addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end = 0;
    bcm_l2_addr_t l2addr;
    uint32 table_id = DBAL_TABLE_FWD_MACT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table handle */
    if (flags & DNX_L2_MATCH_IVL)
    {
        table_id = DBAL_TABLE_FWD_MACT_IVL;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Configure rules according to the flags */
    SHR_IF_ERR_EXIT(dnx_l2_traverse_set_rules(unit, flags, match_l2addr, match_mask_l2addr, &entry_handle_id));

    /** Configure actions according to the flags */
    SHR_IF_ERR_EXIT(dnx_l2_traverse_set_actions(unit, flags, replace_l2addr, replace_mask_l2addr, &entry_handle_id));

    /*
     * When DNX_L2_NO_CALLBACKS is set no iteration over the entries is needed.
     * Action commit is used only when no iteration done. Commit is performed in the first call to dbal_iterator_get_next */
    if (flags & DNX_L2_NO_CALLBACKS)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            /** Fetch l2 address fields */
            SHR_IF_ERR_EXIT(dnx_l2_addr_from_dbal_to_struct(unit, entry_handle_id, table_id, &l2addr));

            /** Call user callbacks */
            SHR_IF_ERR_EXIT(dnx_traverse_callbacks_invoke(unit, flags, &l2addr, trav_fn, user_data));

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Call trav_fn for all the entries in FWD_MACT.
 * 
 * \param [in] unit - unit id.
 * \param [in] trav_fn - user callback.
 * \param [in] user_data - context to add to the user callback.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_traverse(
    int unit,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    uint32 traverse_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    /** Return all the entries in FWD_MACT regardless of the hit bit. Static and dynamic entries. */
    traverse_flags = BCM_L2_TRAVERSE_IGNORE_DES_HIT | BCM_L2_TRAVERSE_MATCH_STATIC;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_traverse_verify
                          (unit, traverse_flags, &match_addr, NULL, NULL, NULL, trav_fn, user_data));

    SHR_IF_ERR_EXIT(dnx_l2_traverse_flags_to_local_flags(unit, &match_addr, traverse_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse the MAC table according to filter rules and call a callback for each entry.
 * 
 * \param [in] unit - unit id
 * \param [in] flags - rules for the traverse filter. \n
 * BCM_L2_TRAVERSE_MATCH_STATIC - traverse static and dynamic entries, otherwise traverse only static or dynamic entries according to
 * match_addr.flags. \n
 * BCM_L2_TRAVERSE_MATCH_MAC - filter by a given mac address taken from match_addr. \n
 * BCM_L2_TRAVERSE_MATCH_VLAN - filter by a given VSI taken from match_addr. \n
 * BCM_L2_TRAVERSE_MATCH_DEST - filter by a given port and encap_id taken from match_addr. \n
 * BCM_L2_TRAVERSE_MATCH_GROUP - filter by a given group taken from match_addr. \n 
 * BCM_L2_TRAVERSE_IGNORE_DES_HIT - If present, ignore hit status in compare. Otherwise traverse only entries with hit status that matches 
 * match_addr.flags. \n 
 * \param [in] match_addr - filter rules for the traverse.  \n
 * [in] match_addr.flags - configures which fields to use. \n
 * [in] match_addr.mac - mac address to filter. \n
 * [in] match_addr.vid - VSI to filter. \n
 * [in] match_addr.port - port param fot destination filtering. \n 
 * [in] match_addr.tgid - trunk id for filtering according to destination. \n
 * [in] match_addr.group - filter according to group field. \n
 * \param [in] trav_fn - user callback that is called for every filtered MAC entry.
 * \param [in] user_data - context to add to the user callback.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_matched_traverse(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    uint32 local_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_traverse_verify(unit, flags, match_addr, NULL, NULL, NULL, trav_fn, user_data));

    SHR_IF_ERR_EXIT(dnx_l2_traverse_flags_to_local_flags(unit, match_addr, flags, &local_flags));

    /** Return MAC table entries according to the flags and filter rules */
    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, match_addr, NULL, NULL, NULL, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse the MAC table according to filter rules and mask, call a callback for each entry.
 * 
 * \param [in] unit - unit id
 * \param [in] flags - rules for the traverse filter. \n
 * BCM_L2_TRAVERSE_MATCH_STATIC - traverse static and dynamic entries, otherwise traverse only static or dynamic entries according to
 * match_addr.flags. \n
 * BCM_L2_TRAVERSE_MATCH_MAC - filter by a given mac address taken from match_addr. \n
 * BCM_L2_TRAVERSE_MATCH_VLAN - filter by a given VSI taken from match_addr. \n
 * BCM_L2_TRAVERSE_MATCH_DEST - filter by a given port and encap_id taken from match_addr. \n 
 * BCM_L2_TRAVERSE_MATCH_GROUP - filter by a given group taken from match_addr. \n 
 * BCM_L2_TRAVERSE_IGNORE_DES_HIT - If present, ignore hit status in compare. Otherwise traverse only entries with hit status that matches 
 * match_addr.flags. \n 
 * \param [in] match_addr - data for filter rules. \n
 * [in] match_addr.flags - configures which fields to use. \n 
 * [in] match_addr.mac - mac address to filter. \n
 * [in] match_addr.vid - VSI to filter. \n
 * [in] match_addr.port - port param fot destination filtering. \n
 * [in] match_addr.tgid - trunk id for filtering according to destination. \n
 * [in] match_addr.group - filter according to group field. \n
 * \param [in] mask_addr - mask for filter rules. Same field description like match_addr. Only set bits are compared.  
 * \param [in] trav_fn - user callback that is called for every filtered MAC entry.
 * \param [in] user_data - context to add to the user callback.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_match_masked_traverse(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_addr,
    bcm_l2_addr_t * mask_addr,
    bcm_l2_traverse_cb trav_fn,
    void *user_data)
{
    uint32 local_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_traverse_verify(unit, flags, match_addr, mask_addr, NULL, NULL, trav_fn, user_data));

    SHR_IF_ERR_EXIT(dnx_l2_traverse_flags_to_local_flags(unit, match_addr, flags, &local_flags));

    /** Return MAC table entries according to the flags and filter rules */
    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule
                    (unit, local_flags, match_addr, mask_addr, NULL, NULL, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace destination or delete multiple/all L2 entries.
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Replace/delete flags BCM_L2_REPLACE_* \n
 * Supported flags: \n
 * BCM_L2_REPLACE_MATCH_STATIC - Replace static and dynamic entries \n
 * BCM_L2_REPLACE_MATCH_MAC - Replace entries with a given MAC address \n
 * BCM_L2_REPLACE_MATCH_VLAN - Replace entries with a given VSI \n
 * BCM_L2_REPLACE_MATCH_DEST - Replace entries having a given destination \n
 * BCM_L2_REPLACE_IGNORE_DES_HIT - Ignore the hit bit in the filter rule \n
 * BCM_L2_REPLACE_DELETE - Delete the entries according to the filters \n
 * BCM_L2_REPLACE_DES_HIT_CLEAR - Clear hit bit \n
 * BCM_L2_REPLACE_DYNAMIC_SET - Change the entry to be dynamic \n
 * BCM_L2_REPLACE_DYNAMIC_CLEAR - Change the entry to be static \n
 * BCM_L2_REPLACE_MIGRATE_PORT - Update destination port \n
 * BCM_L2_REPLACE_NEW_TRUNK - Update trunk destination \n
 * BCM_L2_REPLACE_PROTECTION_RING - Update protection ring desstination \n
 * BCM_L2_REPLACE_MATCH_GROUP - Replace entries with a given group \n 
 * BCM_L2_REPLACE_NO_CALLBACKS - Do not call user callbacks for every entry replaced \n
 * \param [in] match_addr - L2 parameters to match on delete/replace \n
 * [in] match_addr.flags - configures which fields to use. \n
 * [in] match_addr.mac - mac address to filter. \n
 * [in] match_addr.vid - VSI to filter. \n
 * [in] match_addr.port - port param fot destination filtering. \n
 * [in] match_addr.tgid - trunk id for filtering according to destination. \n
 * [in] match_addr.group - filter according to group field. \n 
 * \param [in] new_module - not used
 * \param [in] new_port - New port for a replace
 * \param [in] new_trunk - New trunk ID for a replace
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_replace(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk)
{
    uint32 local_flags = 0;
    bcm_l2_addr_t replace_addr;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&replace_addr, 0x0, sizeof(replace_addr));
    replace_addr.modid = new_module;
    replace_addr.port = new_port;
    replace_addr.tgid = new_trunk;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify
                          (unit, flags, match_addr, match_addr, &replace_addr, &replace_addr, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_traverse_format_addresses
                    (unit, flags, match_addr, match_addr, &replace_addr, &replace_addr));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, match_addr, flags, &local_flags));

    /** Update MAC entries according to the flags */
    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, match_addr, NULL, &replace_addr, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * 
 * \param [in] unit - unit id
 * \param [in] flags - Replace/delete flags BCM_L2_REPLACE_* \n
 * Supported replace flags: \n
 * BCM_L2_REPLACE_MATCH_STATIC - Replace static and dynamic entries \n
 * BCM_L2_REPLACE_MATCH_MAC - Replace entries with a given MAC address \n    
 * BCM_L2_REPLACE_MATCH_VLAN - Replace entries with a given VSI \n
 * BCM_L2_REPLACE_MATCH_DEST - Replace entries having a given destination \n
 * BCM_L2_REPLACE_IGNORE_DES_HIT - Ignore the hit bit in the filter rule \n
 * BCM_L2_REPLACE_DELETE - Delete the entries according to the filters \n
 * BCM_L2_REPLACE_DES_HIT_CLEAR - Clear hit bit \n
 * BCM_L2_REPLACE_DYNAMIC_SET - Change the entry to be dynamic \n
 * BCM_L2_REPLACE_DYNAMIC_CLEAR - Change the entry to be static \n
 * BCM_L2_REPLACE_MIGRATE_PORT - Update destination port \n
 * BCM_L2_REPLACE_NEW_TRUNK - Update trunk destination \n
 * BCM_L2_REPLACE_PROTECTION_RING - Update protection ring desstination \n
 * BCM_L2_REPLACE_MATCH_GROUP - Replace entries with a given group \n 
 * BCM_L2_REPLACE_NO_CALLBACKS - Do not call user callbacks for every entry replaced \n
 * \param [in] match_addr - L2 parameters to match on delete/replace \n 
 * [in] match_addr.flags - configures which fields to use. \n
 * [in] match_addr.mac - mac address to filter. \n
 * [in] match_addr.vid - VSI to filter. \n
 * [in] match_addr.port - port param fot destination filtering. \n
 * [in] match_addr.tgid - trunk id for filtering according to destination. \n
 * [in] match_addr.group - filter according to group field.
 * \param [in] mask_addr - mask for the filter rules. Only set bits are compared. See match_addr for fields description.
 * \param [in] replace_addr - new data to be replaced in the entries
 * \param [in] replace_mask_addr - mask to the new data. Only set bits are updated.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * match_addr,
    bcm_l2_addr_t * mask_addr,
    bcm_l2_addr_t * replace_addr,
    bcm_l2_addr_t * replace_mask_addr)
{
    uint32 local_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify
                          (unit, flags, match_addr, mask_addr, replace_addr, replace_mask_addr, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_traverse_format_addresses
                    (unit, flags, match_addr, mask_addr, replace_addr, replace_mask_addr));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, match_addr, flags, &local_flags));

    /** Update MAC entries according to the flags */
    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule
                    (unit, local_flags, match_addr, mask_addr, replace_addr, replace_mask_addr, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given port.
 * 
 * \param [in] unit - unit id
 * \param [in] mod - not used
 * \param [in] port - port for filtering which entries to delete
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS and BCM_L2_DELETE_STATIC are handled in the function. When no callbacks flag exists 
 *        the API will not call user callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_port(
    int unit,
    bcm_module_t mod,
    bcm_port_t port,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    match_addr.port = port;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE
        | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given MAC address.
 * 
 * \param [in] unit - unit id 
 * \param [in] mac - mac for filtering which entries to delete
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS is handled in the function. When no callbacks flag exists the API will not call user 
 *        callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_mac(
    int unit,
    bcm_mac_t mac,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    bcm_l2_addr_t_init(&match_addr, mac, 0);

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_DELETE |
        replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given VSI.
 * 
 * \param [in] unit - unit id 
 * \param [in] vid - vsi for filtering which entries to delete
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS and BCM_L2_DELETE_STATIC are handled in the function. When no callbacks flag exists 
 *        the API will not call user callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_vlan(
    int unit,
    bcm_vlan_t vid,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    match_addr.vid = vid;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_DELETE
        | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given trunk id.
 * 
 * \param [in] unit - unit id 
 * \param [in] tid - trunk id for filtering which entries to delete
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS is handled in the function. When no callbacks flag exists the API will not call user 
 *        callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_trunk(
    int unit,
    bcm_trunk_t tid,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_GPORT_TRUNK_SET(match_addr.port, match_addr.tgid);
    match_addr.encap_id = BCM_FORWARD_ENCAP_ID_INVALID;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE
        | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given MAC address and port.
 * 
 * \param [in] unit - unit id 
 * \param [in] mac - mac for filtering which entries to delete 
 * \param [in] mod - not used 
 * \param [in] port - port for filtering the MAC entries 
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS is handled in the function. When no callbacks flag exists the API will not call user 
 *        callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_mac_port(
    int unit,
    bcm_mac_t mac,
    bcm_module_t mod,
    bcm_port_t port,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    bcm_l2_addr_t_init(&match_addr, mac, 0);

    match_addr.port = port;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_MAC |
        BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given trunk id and VSI.
 * 
 * \param [in] unit - unit id 
 * \param [in] vid - VSI for filtering which entries to delete 
 * \param [in] tid - trunk id for filtering which entries to delete
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS is handled in the function. When no callbacks flag exists the API will not call user 
 *        callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_vlan_trunk(
    int unit,
    bcm_vlan_t vid,
    bcm_trunk_t tid,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    match_addr.encap_id = BCM_FORWARD_ENCAP_ID_INVALID;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE
        | BCM_L2_REPLACE_MATCH_VLAN | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete MAC entries (static and dynamic) according to a given MAC address and port.
 * 
 * \param [in] unit - unit id 
 * \param [in] vid - VSI for filtering which entries to delete 
 * \param [in] mod - not used 
 * \param [in] port - port for filtering the MAC entries 
 * \param [in] flags - Only BCM_L2_DELETE_NO_CALLBACKS is handled in the function. When no callbacks flag exists the API will not call user 
 *        callbacks.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_addr_delete_by_vlan_port(
    int unit,
    bcm_vlan_t vid,
    bcm_module_t mod,
    bcm_port_t port,
    uint32 flags)
{
    uint32 replace_flags;
    uint32 local_flags = 0;
    bcm_l2_addr_t match_addr;
    uint32 replace_no_callbacks_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l2_delete_verify(unit, flags));

    if (flags & BCM_L2_DELETE_NO_CALLBACKS)
    {
        replace_no_callbacks_flag |= BCM_L2_REPLACE_NO_CALLBACKS;
    }

    sal_memset((void *) &match_addr, 0, sizeof(bcm_l2_addr_t));

    match_addr.vid = vid;
    match_addr.port = port;

    /** Delete all the entries in FWD_MACT regardless of the hit bit. Static and dynacmic entries according to port. */
    replace_flags =
        BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_VLAN |
        BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_DELETE | replace_no_callbacks_flag;

    SHR_INVOKE_VERIFY_DNX(dnx_l2_replace_verify(unit, replace_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(dnx_l2_replace_flags_to_local_flags(unit, &match_addr, replace_flags, &local_flags));

    SHR_IF_ERR_EXIT(dnx_l2_addr_traverse_by_rule(unit, local_flags, &match_addr, NULL, NULL, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

 /*
  * }
  */
