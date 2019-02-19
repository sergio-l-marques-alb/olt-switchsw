/*
 * $Id: macsec_cmn.c,v 1 2016/11/18 vmenon Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    xflow_macsec_cmn.c
 * Purpose: XFLOW_MACSEC software module intergation support
 */

#ifdef INCLUDE_XFLOW_MACSEC

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/register.h>
#include <soc/memory.h>

#include <sal/core/libc.h>
#include <bcm/debug.h>

#include <bcm_int/common/xflow_macsec_cmn.h>

#include <bcm_int/control.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/oam.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}

/*
 * Function:
 *      bcm_common_xflow_macsec_init
 * Purpose:
 *      Initialize XFLOW_MACSEC for the specified unit.
 */
int _bcm_common_xflow_macsec_init(int unit)
{
    UNIT_VALID(unit);


    /* Initilaize the MACSEC module */
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        /* Register counter collection callback. */
        soc_counter_extra_register(unit, _xflow_macsec_counters_collect);
        return _bcm_xflow_macsec_init(unit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_xflow_macsec_deinit
 * Purpose:
 *      Deinitialize XFLOW_MACSEC for the specified unit.
 */
int _bcm_common_xflow_macsec_deinit(int unit)
{
    UNIT_VALID(unit);

    /* Deinitilaize the MACSEC module */
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        /* De-Register counter collection callback. */
        soc_counter_extra_unregister(unit, _xflow_macsec_counters_collect);
        return _bcm_xflow_macsec_deinit(unit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_xflow_macsec_sync
 * Purpose:
 *      Warmboot sync XFLOW_MACSEC for the specified unit.
 */
int _bcm_common_xflow_macsec_sync(int unit)
{
    UNIT_VALID(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Sync the MACSEC module */
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        return xflow_macsec_wb_sync(unit);
    }
#endif
    return BCM_E_NONE;
}

/* Security Channel */

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_create
 * Purpose:
 *      Create an XFLOW_MACSEC security channel
 */
int
bcm_common_xflow_macsec_secure_chan_create(int unit,
        uint32 flags,
        bcm_xflow_macsec_secure_chan_info_t *chan_info,
        int priority,
        bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_create(unit, flags,
                                        (xflow_macsec_secure_chan_info_t *)chan_info, 
                                        priority,
                                        (xflow_macsec_secure_chan_id_t *)chan_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_set
 * Purpose:
 *      Sets/Resets the value of one of the parameters 
 *      of a given security channel.
 */
int
bcm_common_xflow_macsec_secure_chan_set(int unit,
        uint32 flags,
        bcm_xflow_macsec_secure_chan_id_t chan_id,
        bcm_xflow_macsec_secure_chan_info_t *chan_info,
        int priority)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_set(unit, flags, chan_id,
                                        (xflow_macsec_secure_chan_info_t *)chan_info,
                                        priority);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_get
 * Purpose:
 *      Get the security channel configuration for a given SC index.
 */
int
bcm_common_xflow_macsec_secure_chan_get(int unit,
        bcm_xflow_macsec_secure_chan_id_t chan_id,
        bcm_xflow_macsec_secure_chan_info_t *chan_info,
        int *priority)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_get(unit, chan_id, chan_info, priority);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_destroy
 * Purpose:
 *      Delete the given security channel. The corresponding SA must be
 *      destroyed first.
 */
int
bcm_common_xflow_macsec_secure_chan_destroy(int unit,
        bcm_xflow_macsec_secure_chan_id_t chan_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_destroy(unit, chan_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_info_t_init
 * Purpose:
 *      Initialize the data structure.
 */
void
bcm_xflow_macsec_secure_chan_info_t_init(
        bcm_xflow_macsec_secure_chan_info_t *chan_info)
{
    xflow_macsec_secure_chan_info_t_init(chan_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_enable_set
 * Purpose:
 *      Enable the Security Channel.
 */
int
bcm_common_xflow_macsec_secure_chan_enable_set(int unit,
        bcm_xflow_macsec_secure_chan_id_t chan_id,
        int enable)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_enable_set(unit, chan_id, enable);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_chan_enable_get
 * Purpose:
 *      Check if the Security Channel is enabled.
 */
int
bcm_common_xflow_macsec_secure_chan_enable_get(int unit,
        bcm_xflow_macsec_secure_chan_id_t chan_id,
        int *enable)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_enable_get(unit, chan_id, enable);
}



/* Security Association */

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_assoc_create
 * Purpose:
 *      Create the Security Association for the 
 *      given Security Channel.
 */
int
bcm_common_xflow_macsec_secure_assoc_create(int unit,
        uint32 flags,
        bcm_xflow_macsec_secure_chan_id_t chan_id,
        bcm_xflow_macsec_secure_assoc_info_t *assoc_info,
        bcm_xflow_macsec_secure_assoc_id_t *assoc_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_assoc_create(unit, flags,
                        chan_id, assoc_info, assoc_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_assoc_set
 * Purpose:
 *      Sets/Resets the value of any of the parameters of 
 *      a given security association.
 */
int
bcm_common_xflow_macsec_secure_assoc_set(int unit,
        bcm_xflow_macsec_secure_assoc_id_t assoc_id,
        bcm_xflow_macsec_secure_assoc_info_t *assoc_info)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_assoc_set(unit, assoc_id,
                    (xflow_macsec_secure_assoc_info_t *)assoc_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_assoc_get
 * Purpose:
 *      Retrieve the Security Association configuration and the
 *      SC index from the SA index.
 */
int
bcm_common_xflow_macsec_secure_assoc_get(int unit,
        bcm_xflow_macsec_secure_assoc_id_t assoc_id,
        bcm_xflow_macsec_secure_assoc_info_t *assoc_info,
        bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_assoc_get(unit, assoc_id, assoc_info, chan_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_assoc_destroy
 * Purpose:
 *      Deletes the SA entry corresponding to the Security Association.
 */
int
bcm_common_xflow_macsec_secure_assoc_destroy(int unit,
        bcm_xflow_macsec_secure_assoc_id_t assoc_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_assoc_destroy(unit, assoc_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_secure_assoc_info_t_init
 * Purpose:
 *      Initialize the Security Association data structure.
 */
void
bcm_xflow_macsec_secure_assoc_info_t_init(
        bcm_xflow_macsec_secure_assoc_info_t *assoc_info)
{
    xflow_macsec_secure_assoc_info_t_init(assoc_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_policy_create
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_policy_create(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info,
    bcm_xflow_macsec_policy_id_t *policy_id)
{
    UNIT_VALID(unit);
    flags |= XFLOW_MACSEC_DECRYPT;
    return xflow_macsec_policy_create(unit, flags,
                        policy_info, policy_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_policy_destroy
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_policy_destroy(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_policy_destroy(unit, policy_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_policy_get
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_policy_get(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info)
{
    UNIT_VALID(unit);
    return xflow_macsec_policy_get(unit, policy_id, policy_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_policy_set
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_policy_set(
    int unit,
    bcm_xflow_macsec_policy_id_t policy_id,
    bcm_xflow_macsec_decrypt_policy_info_t *policy_info)

{
    UNIT_VALID(unit);
    return xflow_macsec_policy_set(unit,
                        policy_id, policy_info);
}

/*
 * Function:
 *      bcm_xflow_macsec_decrypt_policy_info_t_init
 * Purpose:
 *      Initialize the data structure.
 */
void
bcm_xflow_macsec_decrypt_policy_info_t_init(
        bcm_xflow_macsec_decrypt_policy_info_t *policy_info)
{
    xflow_macsec_policy_info_t_init(policy_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_create
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_create(int unit,
    uint32 flags,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int priority,
    bcm_xflow_macsec_flow_id_t *flow_id)
{
    UNIT_VALID(unit);
    flags |= XFLOW_MACSEC_DECRYPT;
    return xflow_macsec_flow_create(unit, flags,
                        flow_info, priority, flow_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_destroy
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_destroy(int unit,
    bcm_xflow_macsec_flow_id_t flow_id)
{
    UNIT_VALID(unit);
    return xflow_macsec_flow_destroy(unit, flow_id);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_enable_get
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_enable_get(int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    int *enable)

{
    UNIT_VALID(unit);
    return xflow_macsec_flow_enable_get(unit, flow_id,
                                        enable);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_enable_set
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_enable_set(int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    int enable)
{
    UNIT_VALID(unit);
    return xflow_macsec_flow_enable_set(unit, flow_id,
                                        enable);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_get
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_get(int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int *priority)
{
    UNIT_VALID(unit);
    return xflow_macsec_flow_get(unit, flow_id,
                                 flow_info, priority);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_decrypt_flow_set
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_decrypt_flow_set(int unit,
    bcm_xflow_macsec_flow_id_t flow_id,
    bcm_xflow_macsec_decrypt_flow_info_t *flow_info,
    int priority)

{
    UNIT_VALID(unit);
    return xflow_macsec_flow_set(unit, flow_id,
                                 flow_info, priority);
}

/*
 * Function:
 *      bcm_xflow_macsec_decrypt_flow_info_t_init
 * Purpose:
 *      Initialize the data structure.
 */
void
bcm_xflow_macsec_decrypt_flow_info_t_init(
        bcm_xflow_macsec_decrypt_flow_info_t *flow_info)
{
    xflow_macsec_flow_info_t_init(flow_info);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_subport_num_get
 * Purpose:
 *      
 */
int
bcm_common_xflow_macsec_subport_num_get(int unit,
    bcm_xflow_macsec_id_t id,
    bcm_xflow_macsec_subport_num_t *macsec_subport_num)
{
    UNIT_VALID(unit);
    return xflow_macsec_subport_num_get(unit, id,
                                        macsec_subport_num);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_control_set
 * Purpose:
 *      Sets the value in HW for the macsec control type provided.
 *
 */
int
bcm_common_xflow_macsec_control_set(int unit, uint32 flags,
                    bcm_xflow_macsec_control_t type, int value)
{
    UNIT_VALID(unit);

    return xflow_macsec_control_set(unit, flags, type, value);
}

/*
 * Function:
 *      bcm_common_xflow_macsec_control_get
 * Purpose:
 *      Gets the value from HW for the macsec control type provided.
 *
 */
int
bcm_common_xflow_macsec_control_get(int unit, uint32 flags,
                    bcm_xflow_macsec_control_t type, int *value)
{
    UNIT_VALID(unit);

    return xflow_macsec_control_get(unit, flags, type, value);
}

int
bcm_common_xflow_macsec_stat_get(
    int unit, 
    uint32 flags, 
    bcm_xflow_macsec_id_t id, 
    bcm_xflow_macsec_stat_type_t  stat_type, 
    uint64 *value)
{
    UNIT_VALID(unit);
    return xflow_macsec_stat_get(unit, flags, id, stat_type, value);
}

int
bcm_common_xflow_macsec_stat_set(
    int unit, 
    uint32 flags, 
    bcm_xflow_macsec_id_t id, 
    bcm_xflow_macsec_stat_type_t  stat_type, 
    uint64 value)
{
    UNIT_VALID(unit);
    return xflow_macsec_stat_set(unit, flags, id, stat_type, value);
}

int
bcm_common_xflow_macsec_stat_multi_get(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    uint32 num_stats,
    bcm_xflow_macsec_stat_type_t  *stat_type_array,
    uint64 *value_array)
{
    UNIT_VALID(unit);
    return xflow_macsec_stat_multi_get(unit, flags, id, num_stats, (xflow_macsec_stat_type_t *)stat_type_array, value_array);
}

int
bcm_common_xflow_macsec_stat_multi_set(
    int unit,
    uint32 flags,
    bcm_xflow_macsec_id_t id,
    uint32 num_stats,
    bcm_xflow_macsec_stat_type_t  *stat_type_array,
    uint64 *value_array)
{
    UNIT_VALID(unit);
    return xflow_macsec_stat_multi_set(unit, flags, id, num_stats, (xflow_macsec_stat_type_t *)stat_type_array, value_array);
}

int
bcm_common_xflow_macsec_vlan_tpid_array_set(
    int unit,
    bcm_xflow_macsec_vlan_tpid_t *vlan_tpid)
{
    UNIT_VALID(unit);
    return xflow_macsec_vlan_tpid_array_set(unit, vlan_tpid);
}

int
bcm_common_xflow_macsec_vlan_tpid_array_get(
    int unit,
    bcm_xflow_macsec_vlan_tpid_t *vlan_tpid)
{
    UNIT_VALID(unit);
    return xflow_macsec_vlan_tpid_array_get(unit, vlan_tpid);
}

int
bcm_common_xflow_macsec_vlan_tpid_array_index_get(
    int unit,
    uint32 vlan_tpid,
    uint8 *tpid_index_sel)
{
    UNIT_VALID(unit);
    return xflow_macsec_vlan_tpid_array_index_get(unit, vlan_tpid, tpid_index_sel);
}

int bcm_common_xflow_macsec_mtu_set(
    int unit,
    int flags,
    uint32 mtu,
    uint32 *mtu_index)
{
    UNIT_VALID(unit);
    return xflow_macsec_mtu_set(unit, flags, mtu, mtu_index);
}

int bcm_common_xflow_macsec_mtu_get(
    int unit,
    int flags,
    uint32 mtu_index,
    uint32 *mtu)
{
    UNIT_VALID(unit);
    return xflow_macsec_mtu_get(unit, flags, mtu_index, mtu);
}

int bcm_common_xflow_macsec_sectag_etype_set(
    int unit,
    int flags,
    uint32 sectag_etype,
    uint32 *sectag_etype_index)
{
    UNIT_VALID(unit);
    return xflow_macsec_sectag_etype_set(unit, flags, sectag_etype,
                                         sectag_etype_index);
}

int bcm_common_xflow_macsec_sectag_etype_get(
    int unit,
    uint32 sectag_etype_index,
    uint32 *sectag_etype)
{
    UNIT_VALID(unit);
    return xflow_macsec_sectag_etype_get(unit, sectag_etype_index, sectag_etype);
}

int bcm_common_xflow_macsec_secure_chan_info_traverse (
    int unit,
    bcm_xflow_macsec_chan_traverse_cb callback,
    void *user_data)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_chan_info_traverse(unit, callback, user_data);
}

int bcm_common_xflow_macsec_secure_assoc_traverse(
    int unit,
    bcm_xflow_macsec_secure_chan_id_t chan_id,
    bcm_xflow_macsec_secure_assoc_traverse_cb callback,
    void *user_data)
{
    UNIT_VALID(unit);
    return xflow_macsec_secure_assoc_traverse(unit, chan_id, callback, user_data);
}


#else
int _xflow_macsec_cmn_not_empty;
#endif /* INCLUDE_XFLOW_MACSEC */
