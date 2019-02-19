/*
 * Copyright: (c)  Broadcom Corp.
 * All Rights Reserved.
 *
 * File:    compat_658_convert.c
 * Purpose: Convert datatypes from/to version 6.5.8 
 * Generator:   mkdispatch 1.54
 */

#ifdef  BCM_RPC_SUPPORT

#include <shared/alloc.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/compat/compat_658.h>
#include <bcm_int/compat/compat_658_convert.h>

#include <bcm/ecn.h>
#include <bcm/field.h>
#include <bcm/mpls.h>
#include <bcm/oam.h>
#include <bcm/policer.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm/stat.h>
#include <bcm/trunk.h>
#include <bcm/types.h>
#include <shared/phyconfig.h>
#include <shared/phyreg.h>
#include <shared/port.h>
#include <shared/port_ability.h>
#include <shared/portmode.h>
#include <shared/switch.h>

#define MIN(_a_,_b_) ((_a_) < (_b_) ? (_a_) : (_b_))

#define BCM_COMPAT_DIRECT_COPY(_var_)                   \
    do { to->_var_ = from->_var_; } while(0)

#define BCM_COMPAT_LOOP_COPY(_var_, _loop_cnt_)         \
    do {                                                \
        sal_memcpy(&to->_var_[0],                       \
                   &from->_var_[0],                     \
                   (sizeof(to->_var_[0]) * _loop_cnt_));\
    } while(0)

/*
 * Function: 
 *     _bcm_compat658in_field_group_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_field_group_config_t (
    bcm_compat658_field_group_config_t *from,
    bcm_field_group_config_t *to)
{
    bcm_field_group_config_t_init(to);

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(qset);
    BCM_COMPAT_DIRECT_COPY(priority);
    BCM_COMPAT_DIRECT_COPY(mode);
    BCM_PBMP_ASSIGN(to->ports, from->ports);
    BCM_COMPAT_DIRECT_COPY(group);
    BCM_COMPAT_DIRECT_COPY(aset);
    BCM_COMPAT_DIRECT_COPY(preselset);
    BCM_COMPAT_DIRECT_COPY(group_ref);
    BCM_COMPAT_DIRECT_COPY(max_entry_priorities);
    BCM_COMPAT_DIRECT_COPY(hintid);
    BCM_COMPAT_DIRECT_COPY(action_res_id);
    BCM_COMPAT_LOOP_COPY(name, MIN(BCM_FIELD_MAX_NAME_LEN, 100));
    to->cycle = 0;

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_field_group_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_field_group_config_t (
    bcm_field_group_config_t *from,
    bcm_compat658_field_group_config_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    /* Propogate the flags but remove one's which are unsupported in 6.5.8 */
    to->flags = from->flags & ~BCM_FIELD_GROUP_CREATE_WITH_CYCLE;

    /* Remove all Qualifiers which are unsupported in 6.5.8 */
    BCM_FIELD_QSET_REMOVE(
        from->qset, bcmFieldQualifyEgressBridgedForwardingDataHigh);
    BCM_FIELD_QSET_REMOVE(
        from->qset, bcmFieldQualifyEgressBridgedForwardingDataLow);
    BCM_FIELD_QSET_REMOVE(from->qset, bcmFieldQualifyEgressBridgedAclDataHigh);
    BCM_FIELD_QSET_REMOVE(from->qset, bcmFieldQualifyEgressBridgedAclDataLow);
    BCM_COMPAT_DIRECT_COPY(qset);

    BCM_COMPAT_DIRECT_COPY(priority);
    BCM_COMPAT_DIRECT_COPY(mode);
    BCM_PBMP_ASSIGN(to->ports, from->ports);
    BCM_COMPAT_DIRECT_COPY(group);

    /* Remove all actions which are unsupported in 6.5.8 */
    BCM_FIELD_ASET_REMOVE(from->aset, bcmFieldActionMacSecRedirectIpmc);
    BCM_COMPAT_DIRECT_COPY(aset);

    BCM_COMPAT_DIRECT_COPY(preselset);
    BCM_COMPAT_DIRECT_COPY(group_ref);
    BCM_COMPAT_DIRECT_COPY(max_entry_priorities);
    BCM_COMPAT_DIRECT_COPY(hintid);
    BCM_COMPAT_DIRECT_COPY(action_res_id);
    BCM_COMPAT_LOOP_COPY(name, MIN(BCM_FIELD_MAX_NAME_LEN, 100));

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658in_oam_endpoint_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_oam_endpoint_info_t (
    bcm_compat658_oam_endpoint_info_t *from,
    bcm_oam_endpoint_info_t *to)
{
    bcm_oam_endpoint_info_t_init(to);

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(flags2);
    BCM_COMPAT_DIRECT_COPY(opcode_flags);
    BCM_COMPAT_DIRECT_COPY(lm_flags);
    BCM_COMPAT_DIRECT_COPY(id);
    BCM_COMPAT_DIRECT_COPY(type);
    BCM_COMPAT_DIRECT_COPY(group);
    BCM_COMPAT_DIRECT_COPY(name);
    BCM_COMPAT_DIRECT_COPY(local_id);
    BCM_COMPAT_DIRECT_COPY(level);
    BCM_COMPAT_DIRECT_COPY(ccm_period);
    BCM_COMPAT_DIRECT_COPY(vlan);
    BCM_COMPAT_DIRECT_COPY(inner_vlan);
    BCM_COMPAT_DIRECT_COPY(gport);
    BCM_COMPAT_DIRECT_COPY(tx_gport);
    BCM_COMPAT_DIRECT_COPY(trunk_index);
    BCM_COMPAT_DIRECT_COPY(intf_id);
    BCM_COMPAT_DIRECT_COPY(mpls_label);
    BCM_COMPAT_DIRECT_COPY(egress_label);
    BCM_COMPAT_DIRECT_COPY(mpls_network_info);
    BCM_COMPAT_LOOP_COPY(dst_mac_address, 6);
    BCM_COMPAT_LOOP_COPY(src_mac_address, 6);
    BCM_COMPAT_DIRECT_COPY(pkt_pri);
    BCM_COMPAT_DIRECT_COPY(inner_pkt_pri);
    BCM_COMPAT_DIRECT_COPY(inner_tpid);
    BCM_COMPAT_DIRECT_COPY(outer_tpid);
    BCM_COMPAT_DIRECT_COPY(int_pri);
    BCM_COMPAT_DIRECT_COPY(cpu_qid);
    BCM_COMPAT_LOOP_COPY(pri_map, MIN(BCM_OAM_INTPRI_MAX, 16));
    BCM_COMPAT_DIRECT_COPY(faults);
    BCM_COMPAT_DIRECT_COPY(persistent_faults);
    BCM_COMPAT_DIRECT_COPY(clear_persistent_faults);
    BCM_COMPAT_DIRECT_COPY(ing_map);
    BCM_COMPAT_DIRECT_COPY(egr_map);
    BCM_COMPAT_DIRECT_COPY(ms_pw_ttl);
    BCM_COMPAT_DIRECT_COPY(port_state);
    BCM_COMPAT_DIRECT_COPY(interface_state);
    BCM_COMPAT_DIRECT_COPY(vccv_type);
    BCM_COMPAT_DIRECT_COPY(vpn);
    BCM_COMPAT_DIRECT_COPY(lm_counter_base_id);
    BCM_COMPAT_DIRECT_COPY(loc_clear_threshold);
    BCM_COMPAT_DIRECT_COPY(timestamp_format);
    BCM_COMPAT_DIRECT_COPY(subport_tpid);
    BCM_COMPAT_DIRECT_COPY(remote_gport);
    BCM_COMPAT_DIRECT_COPY(mpls_out_gport);
    BCM_COMPAT_DIRECT_COPY(sampling_ratio);
    BCM_COMPAT_DIRECT_COPY(lm_payload_offset);
    BCM_COMPAT_DIRECT_COPY(lm_cos_offset);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_type);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_sample_size);
    BCM_COMPAT_DIRECT_COPY(pri_map_id);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_pool_id);
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_base_id, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_offset, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_action, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_DIRECT_COPY(ccm_tx_update_lm_counter_size);
    BCM_COMPAT_DIRECT_COPY(session_id);
    BCM_COMPAT_DIRECT_COPY(session_num_entries);
    BCM_COMPAT_DIRECT_COPY(lm_count_profile);
    BCM_COMPAT_DIRECT_COPY(mpls_exp);
    BCM_COMPAT_DIRECT_COPY(action_reference_id);
    to->acc_profile_id = 0;

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_oam_endpoint_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_oam_endpoint_info_t (
    bcm_oam_endpoint_info_t *from,
    bcm_compat658_oam_endpoint_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(flags2);
    BCM_COMPAT_DIRECT_COPY(opcode_flags);
    BCM_COMPAT_DIRECT_COPY(lm_flags);
    BCM_COMPAT_DIRECT_COPY(id);
    BCM_COMPAT_DIRECT_COPY(type);
    BCM_COMPAT_DIRECT_COPY(group);
    BCM_COMPAT_DIRECT_COPY(name);
    BCM_COMPAT_DIRECT_COPY(local_id);
    BCM_COMPAT_DIRECT_COPY(level);
    BCM_COMPAT_DIRECT_COPY(ccm_period);
    BCM_COMPAT_DIRECT_COPY(vlan);
    BCM_COMPAT_DIRECT_COPY(inner_vlan);
    BCM_COMPAT_DIRECT_COPY(gport);
    BCM_COMPAT_DIRECT_COPY(tx_gport);
    BCM_COMPAT_DIRECT_COPY(trunk_index);
    BCM_COMPAT_DIRECT_COPY(intf_id);
    BCM_COMPAT_DIRECT_COPY(mpls_label);
    BCM_COMPAT_DIRECT_COPY(egress_label);
    BCM_COMPAT_DIRECT_COPY(mpls_network_info);
    BCM_COMPAT_LOOP_COPY(dst_mac_address, 6);
    BCM_COMPAT_LOOP_COPY(src_mac_address, 6);
    BCM_COMPAT_DIRECT_COPY(pkt_pri);
    BCM_COMPAT_DIRECT_COPY(inner_pkt_pri);
    BCM_COMPAT_DIRECT_COPY(inner_tpid);
    BCM_COMPAT_DIRECT_COPY(outer_tpid);
    BCM_COMPAT_DIRECT_COPY(int_pri);
    BCM_COMPAT_DIRECT_COPY(cpu_qid);
    BCM_COMPAT_LOOP_COPY(pri_map, MIN(BCM_OAM_INTPRI_MAX, 16));
    BCM_COMPAT_DIRECT_COPY(faults);
    BCM_COMPAT_DIRECT_COPY(persistent_faults);
    BCM_COMPAT_DIRECT_COPY(clear_persistent_faults);
    BCM_COMPAT_DIRECT_COPY(ing_map);
    BCM_COMPAT_DIRECT_COPY(egr_map);
    BCM_COMPAT_DIRECT_COPY(ms_pw_ttl);
    BCM_COMPAT_DIRECT_COPY(port_state);
    BCM_COMPAT_DIRECT_COPY(interface_state);
    BCM_COMPAT_DIRECT_COPY(vccv_type);
    BCM_COMPAT_DIRECT_COPY(vpn);
    BCM_COMPAT_DIRECT_COPY(lm_counter_base_id);
    BCM_COMPAT_DIRECT_COPY(loc_clear_threshold);
    BCM_COMPAT_DIRECT_COPY(timestamp_format);
    BCM_COMPAT_DIRECT_COPY(subport_tpid);
    BCM_COMPAT_DIRECT_COPY(remote_gport);
    BCM_COMPAT_DIRECT_COPY(mpls_out_gport);
    BCM_COMPAT_DIRECT_COPY(sampling_ratio);
    BCM_COMPAT_DIRECT_COPY(lm_payload_offset);
    BCM_COMPAT_DIRECT_COPY(lm_cos_offset);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_type);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_sample_size);
    BCM_COMPAT_DIRECT_COPY(pri_map_id);
    BCM_COMPAT_DIRECT_COPY(lm_ctr_pool_id);
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_base_id, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_offset, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_LOOP_COPY(
        ccm_tx_update_lm_counter_action, MIN(BCM_OAM_LM_COUNTER_MAX, 3));
    BCM_COMPAT_DIRECT_COPY(ccm_tx_update_lm_counter_size);
    BCM_COMPAT_DIRECT_COPY(session_id);
    BCM_COMPAT_DIRECT_COPY(session_num_entries);
    BCM_COMPAT_DIRECT_COPY(lm_count_profile);
    BCM_COMPAT_DIRECT_COPY(mpls_exp);
    BCM_COMPAT_DIRECT_COPY(action_reference_id);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658in_stat_lif_counting_source_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_stat_lif_counting_source_t (
    bcm_compat658_stat_lif_counting_source_t *from,
    bcm_stat_lif_counting_source_t *to)
{
    BCM_COMPAT_DIRECT_COPY(type);
    BCM_COMPAT_DIRECT_COPY(command_id);
    BCM_COMPAT_DIRECT_COPY(stif_counter_id);
    BCM_COMPAT_DIRECT_COPY(offset);
    to->engine_range_offset = 0;

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658in_stat_lif_counting_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_stat_lif_counting_t (
    bcm_compat658_stat_lif_counting_t *from,
    bcm_stat_lif_counting_t *to)
{
    _bcm_compat658in_stat_lif_counting_source_t(&from->source, &to->source);
    BCM_COMPAT_DIRECT_COPY(range);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_stat_lif_counting_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_stat_lif_counting_t (
    bcm_stat_lif_counting_t *from,
    bcm_compat658_stat_lif_counting_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    BCM_COMPAT_DIRECT_COPY(source.type);
    BCM_COMPAT_DIRECT_COPY(source.command_id);
    BCM_COMPAT_DIRECT_COPY(source.stif_counter_id);
    BCM_COMPAT_DIRECT_COPY(source.offset);
    BCM_COMPAT_DIRECT_COPY(range);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_port_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_port_config_t (
    bcm_port_config_t *from,
    bcm_compat658_port_config_t *to)
{
    int i;

    sal_memset(to, 0, sizeof(*to));

    BCM_PBMP_ASSIGN(to->fe, from->fe);
    BCM_PBMP_ASSIGN(to->ge, from->ge);
    BCM_PBMP_ASSIGN(to->xe, from->xe);
    BCM_PBMP_ASSIGN(to->ce, from->ce);
    BCM_PBMP_ASSIGN(to->e, from->e);
    BCM_PBMP_ASSIGN(to->hg, from->hg);
    BCM_PBMP_ASSIGN(to->sci, from->sci);
    BCM_PBMP_ASSIGN(to->sfi, from->sfi);
    BCM_PBMP_ASSIGN(to->spi, from->spi);
    BCM_PBMP_ASSIGN(to->spi_subport, from->spi_subport);
    BCM_PBMP_ASSIGN(to->port, from->port);
    BCM_PBMP_ASSIGN(to->cpu, from->cpu);
    BCM_PBMP_ASSIGN(to->all, from->all);
    BCM_PBMP_ASSIGN(to->stack_int, from->stack_int);
    BCM_PBMP_ASSIGN(to->stack_ext, from->stack_ext);
    BCM_PBMP_ASSIGN(to->tdm, from->tdm);
    BCM_PBMP_ASSIGN(to->pon, from->pon);
    BCM_PBMP_ASSIGN(to->llid, from->llid);
    BCM_PBMP_ASSIGN(to->il, from->il);
    BCM_PBMP_ASSIGN(to->xl, from->xl);
    BCM_PBMP_ASSIGN(to->rcy, from->rcy);
    BCM_PBMP_ASSIGN(to->sat, from->sat);
    BCM_PBMP_ASSIGN(to->ipsec, from->ipsec);
    for(i=0 ; i<4 ; i++) {
        BCM_PBMP_ASSIGN(to->per_pipe[i], from->per_pipe[i]);
    }
    BCM_PBMP_ASSIGN(to->nif, from->nif);
    BCM_PBMP_ASSIGN(to->control, from->control);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658in_trunk_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_trunk_info_t (
    bcm_compat658_trunk_info_t *from,
    bcm_trunk_info_t *to)
{
    bcm_trunk_info_t_init(to);

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(psc);
    sal_memset(&to->psc_info, 0, sizeof(to->psc_info));
    BCM_COMPAT_DIRECT_COPY(ipmc_psc);
    BCM_COMPAT_DIRECT_COPY(dlf_index);
    BCM_COMPAT_DIRECT_COPY(mc_index);
    BCM_COMPAT_DIRECT_COPY(ipmc_index);
    BCM_COMPAT_DIRECT_COPY(dynamic_size);
    BCM_COMPAT_DIRECT_COPY(dynamic_age);
    BCM_COMPAT_DIRECT_COPY(dynamic_load_exponent);
    BCM_COMPAT_DIRECT_COPY(dynamic_expected_load_exponent);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_trunk_info_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_trunk_info_t (
    bcm_trunk_info_t *from,
    bcm_compat658_trunk_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    to->flags = from->flags &
            ~(BCM_TRUNK_FLAG_LOCAL | BCM_TRUNK_FLAG_DONT_ALLOCATE_PP_PORTS);
    to->psc =
        from->psc & ~(BCM_TRUNK_PSC_C_LAG | BCM_TRUNK_PSC_WEIGHTED_PROFILE);
    BCM_COMPAT_DIRECT_COPY(ipmc_psc);
    BCM_COMPAT_DIRECT_COPY(dlf_index);
    BCM_COMPAT_DIRECT_COPY(mc_index);
    BCM_COMPAT_DIRECT_COPY(ipmc_index);
    BCM_COMPAT_DIRECT_COPY(dynamic_size);
    BCM_COMPAT_DIRECT_COPY(dynamic_age);
    BCM_COMPAT_DIRECT_COPY(dynamic_load_exponent);
    BCM_COMPAT_DIRECT_COPY(dynamic_expected_load_exponent);

    return BCM_E_NONE;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat658in_ecn_map_t
 * Purpose:
 *      Convert the bcm_ecn_map_t datatype from <=6.5.8 to
 *      SDK 6.5.8+
 * Parameters:
 *      from        - (IN) The <=6.5.8 version of the datatype
 *      to          - (OUT) The SDK 6.5.8+ version of the datatype
 * Returns:
 *      BCM_E_NONE
 */
extern int 
_bcm_compat658in_ecn_map_t(bcm_compat658_ecn_map_t *from,
                           bcm_ecn_map_t *to)
{
    bcm_ecn_map_t_init(to);
    BCM_COMPAT_DIRECT_COPY(action_flags);
    BCM_COMPAT_DIRECT_COPY(int_cn);
    BCM_COMPAT_DIRECT_COPY(inner_ecn);
    BCM_COMPAT_DIRECT_COPY(ecn);
    BCM_COMPAT_DIRECT_COPY(exp);
    BCM_COMPAT_DIRECT_COPY(new_ecn);
    BCM_COMPAT_DIRECT_COPY(new_exp);
    to->nonip_action_flags = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_compat658out_ecn_map_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from 6.5.8+ to
 *      <=6.5.8
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The <=6.5.8 version of the datatype
 * Returns:
 *      BCM_E_NONE
 */
extern int 
_bcm_compat658out_ecn_map_t(bcm_ecn_map_t *from,
                            bcm_compat658_ecn_map_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    BCM_COMPAT_DIRECT_COPY(action_flags);
    BCM_COMPAT_DIRECT_COPY(int_cn);
    BCM_COMPAT_DIRECT_COPY(inner_ecn);
    BCM_COMPAT_DIRECT_COPY(ecn);
    BCM_COMPAT_DIRECT_COPY(exp);
    BCM_COMPAT_DIRECT_COPY(new_ecn);
    BCM_COMPAT_DIRECT_COPY(new_exp);

    return BCM_E_NONE;
}


#endif



/*
 * Function: 
 *     _bcm_compat658in_policer_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_policer_config_t (
    bcm_compat658_policer_config_t *from,
    bcm_policer_config_t *to)
{
    bcm_policer_config_t_init(to);

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(mode);
    BCM_COMPAT_DIRECT_COPY(ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(max_ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(max_cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(ckbits_burst);
    BCM_COMPAT_DIRECT_COPY(cbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(max_pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(max_pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(pkbits_burst);
    BCM_COMPAT_DIRECT_COPY(pbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(kbits_current);
    BCM_COMPAT_DIRECT_COPY(bits_current_lower);
    BCM_COMPAT_DIRECT_COPY(action_id);
    BCM_COMPAT_DIRECT_COPY(sharing_mode);
    BCM_COMPAT_DIRECT_COPY(entropy_id);
    BCM_COMPAT_DIRECT_COPY(pool_id);
    BCM_COMPAT_DIRECT_COPY(bucket_width);
    BCM_COMPAT_DIRECT_COPY(token_gran);
    BCM_COMPAT_DIRECT_COPY(bucket_init_val);
    BCM_COMPAT_DIRECT_COPY(bucket_rollover_val);
    BCM_COMPAT_DIRECT_COPY(core_id);
    BCM_COMPAT_DIRECT_COPY(ncoflow_policer_id);
    BCM_COMPAT_DIRECT_COPY(npoflow_policer_id);
    BCM_COMPAT_DIRECT_COPY(actual_ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(actual_cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(actual_ckbits_burst);
    BCM_COMPAT_DIRECT_COPY(actual_cbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(actual_pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(actual_pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(actual_pkbits_burst);
    BCM_COMPAT_DIRECT_COPY(actual_pbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(average_pkt_size);
    BCM_COMPAT_DIRECT_COPY(mark_drop_as_black);
    to->color_resolve_profile = 0;

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658in_policer_group_mode_attr_selector_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Old datatype in 6.5.8
 *     to   - (OUT) Current datatype
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658in_policer_group_mode_attr_selector_t (
    bcm_compat658_policer_group_mode_attr_selector_t *from,
    bcm_policer_group_mode_attr_selector_t *to)
{
    int i;

    bcm_policer_group_mode_attr_selector_t_init(to);

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(policer_offset);
    BCM_COMPAT_DIRECT_COPY(attr);
    BCM_COMPAT_DIRECT_COPY(attr_value);
    BCM_COMPAT_DIRECT_COPY(attr_value_mask);
    BCM_COMPAT_DIRECT_COPY(udf_id);
    BCM_COMPAT_DIRECT_COPY(offset);
    BCM_COMPAT_DIRECT_COPY(width);
    BCM_COMPAT_DIRECT_COPY(multi_attr_count);
    BCM_COMPAT_LOOP_COPY(
        multi_attr_flags, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(multi_attr, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(
        multi_attr_value, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(
        multi_attr_value_mask, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(multi_udf_id, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(multi_offset, MIN(bcmPolicerGroupModeAttrCount, 16));
    BCM_COMPAT_LOOP_COPY(multi_width, MIN(bcmPolicerGroupModeAttrCount, 16));

    for (i=16 ; i<bcmPolicerGroupModeAttrCount ; i++) {
        to->multi_attr_flags[i] = 0;
        to->multi_attr[i] = 0;
        to->multi_attr_value[i] = 0;
        to->multi_attr_value_mask[i] = 0;
        to->multi_udf_id[i] = 0;
        to->multi_offset[i] = 0;
        to->multi_width[i] = 0;
    }

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_policer_config_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_policer_config_t (
    bcm_policer_config_t *from,
    bcm_compat658_policer_config_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(mode);
    BCM_COMPAT_DIRECT_COPY(ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(max_ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(max_cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(ckbits_burst);
    BCM_COMPAT_DIRECT_COPY(cbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(max_pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(max_pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(pkbits_burst);
    BCM_COMPAT_DIRECT_COPY(pbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(kbits_current);
    BCM_COMPAT_DIRECT_COPY(bits_current_lower);
    BCM_COMPAT_DIRECT_COPY(action_id);
    BCM_COMPAT_DIRECT_COPY(sharing_mode);
    BCM_COMPAT_DIRECT_COPY(entropy_id);
    BCM_COMPAT_DIRECT_COPY(pool_id);
    BCM_COMPAT_DIRECT_COPY(bucket_width);
    BCM_COMPAT_DIRECT_COPY(token_gran);
    BCM_COMPAT_DIRECT_COPY(bucket_init_val);
    BCM_COMPAT_DIRECT_COPY(bucket_rollover_val);
    BCM_COMPAT_DIRECT_COPY(core_id);
    BCM_COMPAT_DIRECT_COPY(ncoflow_policer_id);
    BCM_COMPAT_DIRECT_COPY(npoflow_policer_id);
    BCM_COMPAT_DIRECT_COPY(actual_ckbits_sec);
    BCM_COMPAT_DIRECT_COPY(actual_cbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(actual_ckbits_burst);
    BCM_COMPAT_DIRECT_COPY(actual_cbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(actual_pkbits_sec);
    BCM_COMPAT_DIRECT_COPY(actual_pbits_sec_lower);
    BCM_COMPAT_DIRECT_COPY(actual_pkbits_burst);
    BCM_COMPAT_DIRECT_COPY(actual_pbits_burst_lower);
    BCM_COMPAT_DIRECT_COPY(average_pkt_size);
    BCM_COMPAT_DIRECT_COPY(mark_drop_as_black);

    return BCM_E_NONE;
}


/*
 * Function: 
 *     _bcm_compat658out_policer_group_mode_attr_selector_t
 * Purpose:
 *     Convert the datatype changed from version SDK 6.5.8
 * Parameters:
 *     from - (IN) Current datatype
 *     to   - (OUT) Old datatype in 6.5.8
 * Returns:
 *     int: rv ( BCM_E_NONE | BCM_E_UNAVAIL )
 */
extern int 
_bcm_compat658out_policer_group_mode_attr_selector_t (
    bcm_policer_group_mode_attr_selector_t *from,
    bcm_compat658_policer_group_mode_attr_selector_t *to)
{
    int i;

    sal_memset(to, 0, sizeof(*to));

    BCM_COMPAT_DIRECT_COPY(flags);
    BCM_COMPAT_DIRECT_COPY(policer_offset);
    BCM_COMPAT_DIRECT_COPY(attr);
    BCM_COMPAT_DIRECT_COPY(attr_value);
    BCM_COMPAT_DIRECT_COPY(attr_value_mask);
    BCM_COMPAT_DIRECT_COPY(udf_id);
    BCM_COMPAT_DIRECT_COPY(offset);
    BCM_COMPAT_DIRECT_COPY(width);

#define BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(_field_)              \
    do {                                                        \
        to->_field_[to->multi_attr_count] = from->_field_[i];   \
    } while(0)

    /* Filter out the unsupported multi attributes */
    to->multi_attr_count = 0;
    for (i=0 ; i<from->multi_attr_count ; i++) {
        if (from->multi_attr[i] < 16) {
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_attr_flags);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_attr);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_attr_value);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_attr_value_mask);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_udf_id);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_offset);
            BCM_COMPAT_COPY_MULTI_ATTR_FIELDS(multi_width);
            to->multi_attr_count++;
        }
    }
#undef BCM_COMPAT_COPY_MULTI_ATTR_FIELDS

    return BCM_E_NONE;
}


#endif  /* BCM_RPC_SUPPORT */

