/*
 * $Id: l3.c,v 1.97 2013/09/19 21:02:14 Exp $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    l3.c
 * Purpose: Trident3 L3 functions implementation
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(INCLUDE_L3) && defined(BCM_TRIDENT3_SUPPORT) 

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/vlan.h>
#include <bcm/error.h>

#include <bcm/l3.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <soc/mem.h>
#include <soc/format.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <soc/apache.h>
#include <soc/esw/flow_db.h>
#include <bcm_int/esw/flow.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/qos.h>
#include <soc/trident3.h>


#ifdef BCM_RIOT_SUPPORT


/* RioT Overlay validity checks */
#define BCMI_RIOT_IS_ENABLED(_u) \
    ((&_bcm_l3_bk_info[_u])->riot_enable)

#define BCMI_L3_NH_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_nh_overlay_entries) > 0))

#define BCMI_L3_INTF_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_intf_overlay_entries) > 0))

#define BCMI_RIOT_VPN_VFI_IS_SET(_u, _vid) \
    ((BCMI_RIOT_IS_ENABLED(_u)) && (_BCM_VPN_VFI_IS_SET((_vid))))

#define BCMI_RIOT_VPN_VFI_IS_NOT_SET(_u, _vid) \
    ((!BCMI_RIOT_IS_ENABLED(_u)) || (!(_BCM_VPN_VFI_IS_SET((_vid)))))

#define BCMI_GPORT_TYPE_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)
/* initialize pointer for useful bokkkeeping information */
#define L3_BK_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

/* Set ul-ol link params */
#define BCM_L3_NH_NUM_UNDERLAY_BUCKETS    1024
td3_ul_nh_link_t *td3_ul_nh_assoc_head
    [BCM_MAX_NUM_UNITS][BCM_L3_NH_NUM_UNDERLAY_BUCKETS] = {{NULL}};

#endif

#define _BCM_TD3_L3_MEM_BANKS_ALL     (-1)
#define _BCM_TD3_HOST_ENTRY_NOT_FOUND (-1)

/* needed for software state of the vc_and_swap label */
extern _bcm_tr_mpls_bookkeeping_t  _bcm_tr_mpls_bk_info[BCM_MAX_NUM_UNITS];
#define MPLS_INFO(_unit_)   (&_bcm_tr_mpls_bk_info[_unit_])
#define VC_AND_SWAP_TABLE_RESV_INDEX 0

    
static soc_profile_mem_t *_bcm_td3_macda_oui_profile[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_td3_vntag_etag_profile[BCM_MAX_NUM_UNITS];
#define L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE 0


/*
 * Function:
 *      bcmi_nh_destination_set
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */

int bcm_td3_riot_l3_nh_dest_set(int unit, bcm_l3_egress_t *nh_entry, ing_l3_next_hop_entry_t *nh_dest)
{
    int gport_type = 0, port = -1, gport = -1;
    uint32 dest_type = SOC_MEM_FIF_DEST_DVP;
    uint32 shift_val = 0;

    gport = nh_entry->port;
    gport_type = BCMI_GPORT_TYPE_GET(gport);

    switch(gport_type) {
    case BCM_GPORT_VXLAN_PORT:
        port = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
        break;
    case BCM_GPORT_L2GRE_PORT:
        port = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
        break;
    case BCM_GPORT_MPLS_PORT:
        port = BCM_GPORT_MPLS_PORT_ID_GET(gport);
        break;
    case BCM_GPORT_MIM_PORT:
        port = BCM_GPORT_MIM_PORT_ID_GET(gport);
        break;
    default:
        if (nh_entry->flags & BCM_L3_TGID) {
            int tid_is_vp_lag = 0;
            if (soc_feature(unit, soc_feature_vp_lag)) {
                (void)_bcm_esw_trunk_id_is_vp_lag(unit, gport, &tid_is_vp_lag);
            }

            if (tid_is_vp_lag) {
                /* Get the VP value representing VP LAG */
                BCM_IF_ERROR_RETURN(_bcm_esw_trunk_tid_to_vp_lag_vp(unit,
                    gport, &port));

            } else {
                dest_type = SOC_MEM_FIF_DEST_LAG;
                port = gport;
            }
        } else {
            dest_type = SOC_MEM_FIF_DEST_DGPP;
            shift_val = SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            port = gport;
        }
    }

    soc_mem_field32_dest_set(unit, ING_L3_NEXT_HOPm, nh_dest,
       DESTINATIONf, dest_type,
       (nh_entry->module << shift_val |
             port));

    return BCM_E_NONE;
}

#ifdef BCM_RIOT_SUPPORT
/*
 * Function:
 *      bcmi_td3_l3_riot_bank_sel
 * Purpose:
 *      Initialize banks for L3 objects.
 *      The whole l3 table can be divided into 
 *      multiple layers and you can set different banks
 *      for different layers.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int 
bcmi_td3_l3_riot_bank_sel(int unit)
{   
    uint32 bank_sel_reg_val     = 0;
    uint32 num_ol_banks      = 0;
    uint32 l2_tunnel_parse_control = 0;
    _bcm_l3_bookkeeping_t *l3   = L3_BK_INFO(unit);
    uint32 ol_bank_bit_value = 0;
    int iter;
    int num_nh, num_l3_intf;
    int max_nh_banks, max_l3_intf_banks;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    /* The interface entries can only be allocated in terms of 2k
     * banks. if user is allocating in between boundries then we need
     * to throw error as hardware cannot divide overlay and underlay 
     * entries in between bank.
     * If SDK does not return error here then user can start using
     * different entries in same banks for overlay and underlay and may
     * see unextected results.
     */
    if (l3->l3_intf_overlay_entries % BCMI_TD3_L3_INTF_ENT_PER_BANK) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_TD3_L3_INTF_ENT_PER_BANK));

        return BCM_E_PARAM;
    }

    if (l3->l3_nh_overlay_entries %  BCMI_TD3_L3_NH_ENT_PER_BANK) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "Please allocate entries in the multiple of %d.\n"), 
            BCMI_TD3_L3_NH_ENT_PER_BANK));

        return BCM_E_PARAM;
    }
    num_nh = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    max_nh_banks = num_nh / BCMI_TD3_L3_NH_ENT_PER_BANK;
    max_l3_intf_banks = num_l3_intf / BCMI_TD3_L3_INTF_ENT_PER_BANK;

    if (max_nh_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No NH banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    if (max_l3_intf_banks == 0) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "No EGR_L3_INTF banks in system.\n")));
        return BCM_E_RESOURCE;
    }
    /* 
     * set banks for EGR_L3_INTF 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_intf_overlay_entries / BCMI_TD3_L3_INTF_ENT_PER_BANK) +
        ((l3->l3_intf_overlay_entries % BCMI_TD3_L3_INTF_ENT_PER_BANK) ? 1 : 0);

    if (num_ol_banks > max_l3_intf_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 interface(%d) > total interfaces(%d).\n"),
            l3->l3_intf_overlay_entries,
            max_l3_intf_banks * BCMI_TD3_L3_INTF_ENT_PER_BANK));

        return BCM_E_CONFIG;
    }

    /* Setup the bits for overlay banks. */
    for (iter = 0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_intf_overlay_alloc_mode > 0) {
        ol_bank_bit_value <<= (max_l3_intf_banks - num_ol_banks);
    }

    soc_reg_field_set(unit, EGR_L3_INTF_BANK_SELr, &bank_sel_reg_val,
        BANK_SELf, ol_bank_bit_value);
    /* set banks in EGR_L3_INTF_BANK_SELr */
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_INTF_BANK_SELr
                          (unit, bank_sel_reg_val));

    ol_bank_bit_value = 0;
    bank_sel_reg_val     = 0;
    /* 
     * set banks for ING/EGR/INITIAL_ING_L3_NEXT_HOP. 
     * By default ("alloc mode" is 0):
     * First set of banks are for overlay.
     * Second set of banks are for underlay.
     * If "alloc mode" is 1:
     * First set of banks are for underlay.
     * Second set of banks are for overlay.
     */
    num_ol_banks = (l3->l3_nh_overlay_entries / BCMI_TD3_L3_NH_ENT_PER_BANK) +
        ((l3->l3_nh_overlay_entries % BCMI_TD3_L3_NH_ENT_PER_BANK) ? 1 : 0);
    if (num_ol_banks > max_nh_banks) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
            "ERROR : Overlay l3 Next hop (%d) > total l3 next hop(%d).\n"),
            l3->l3_nh_overlay_entries,
            max_nh_banks * BCMI_TD3_L3_NH_ENT_PER_BANK));

        return BCM_E_CONFIG;
    }
    for (iter=0; iter < num_ol_banks; iter++) {
        ol_bank_bit_value |= (1 << iter) ;
    }
    if (l3->l3_nh_overlay_alloc_mode > 0) {
        ol_bank_bit_value <<= (max_nh_banks - num_ol_banks);
    }

    /* set banks in INITIAL_ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, INITIAL_ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_INITIAL_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in ING_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, ING_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);

    SOC_IF_ERROR_RETURN(WRITE_ING_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* set banks in EGR_L3_NEXT_HOP_BANK_SELr */
    soc_reg_field_set(unit, EGR_L3_NEXT_HOP_BANK_SELr,
        &bank_sel_reg_val, BANK_SELf, ol_bank_bit_value);
    SOC_IF_ERROR_RETURN(WRITE_EGR_L3_NEXT_HOP_BANK_SELr
        (unit, bank_sel_reg_val));

    /* Enable the control to look into inner l3 header for RioT */
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, IFP_L2_TUNNEL_PAYLOAD_FIELD_SELf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV4_PAYLOADf, 0x1);
    soc_reg_field_set(unit, ING_L2_TUNNEL_PARSE_CONTROLr,
        &l2_tunnel_parse_control, PARSE_IPV6_PAYLOADf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_ING_L2_TUNNEL_PARSE_CONTROLr
        (unit, l2_tunnel_parse_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_l3_flex_nh_add
 * Purpose:
 *      Set L3 multicast / unicast next hop flex entry.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      nh_index - (IN) Next hop index.
 *      l3_egress - (IN) L3 egress structure.
 *      ipmc  - (IN) multicast
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_td3_l3_flex_nh_add(int unit, int nh_index,
        bcm_l3_egress_t *l3_egress, uint32 *egr_nh,
        int ipmc)
{
    int rv = BCM_E_NONE;
    int i;
    int macda_index = -1;
#ifdef BCM_RIOT_SUPPORT
    uint32 vp;
#endif
    uint32  view_id;
    uint32  flex_data_array[128];
    uint32  flex_data_count;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;
    uint32 action_set_field = 0;
    uint8 process_l2_edit_controls = 0;
    uint8 process_switching_controls = 0;
    uint8 process_profiled_assignment_dst_mac = 0;
    uint8 process_higig_controls = 0;
    uint8 process_higig_change_dest = 0;
    uint8 process_ttl_a = 0;
    uint8 process_intf = 0;
    uint8 process_mac_addr = 0;
    uint8 process_vxlt = 0;
    uint8 process_class_id = 0;
    uint8 process_next_ptr = 0;

/*
    int flag_set;
    uint32 flag_array[] = {
        BCM_L3_MULTICAST_L2_DEST_PRESERVE,
        BCM_L3_MULTICAST_L2_SRC_PRESERVE,
        BCM_L3_MULTICAST_L2_VLAN_PRESERVE,
        BCM_L3_MULTICAST_TTL_PRESERVE,
        BCM_L3_MULTICAST_DEST_PRESERVE,
        BCM_L3_MULTICAST_SRC_PRESERVE,
        BCM_L3_MULTICAST_VLAN_PRESERVE,
        BCM_L3_MULTICAST_L3_DROP,
        BCM_L3_MULTICAST_L2_DROP
    };
    soc_field_t field_array[] = {
        L3MC__L2_MC_DA_DISABLEf,
        L3MC__L2_MC_SA_DISABLEf,
        L3MC__L2_MC_VLAN_DISABLEf,
        L3MC__L3_MC_TTL_DISABLEf,
        L3MC__L3_MC_DA_DISABLEf,
        L3MC__L3_MC_SA_DISABLEf,
        L3MC__L3_MC_VLAN_DISABLEf,
        L3MC__L3_DROPf,
        L3MC__L2_DROPf
    };
*/

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l3_egress->flow_handle,
                          l3_egress->flow_option_handle,
                          SOC_FLOW_DB_FUNC_EGRESS_OBJ_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Configure the egress next hop entry */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, view_id,
                MEM_BLOCK_ANY, nh_index, egr_nh));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, egr_nh);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l3_egress->logical_fields;
 
    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, egr_nh, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case L2_EDIT_CONTROLS_ACTION_SETf:
                process_l2_edit_controls = 1;
                break;
            case HIGIG_CONTROLS_ACTION_SETf:
                process_higig_controls = 1;
                break;
            case HIGIG_CHANGE_DEST_ACTION_SETf:
                process_higig_change_dest = 1;
                break;
            case SWITCHING_CTRLS_ACTION_SETf:
                process_switching_controls = 1;
                break;
            case TTL_A_ACTION_SETf:
                process_ttl_a = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case INTF_ACTION_SETf:
                process_intf = 1;
                break;
            case DIRECT_ASSIGNMENT_6_ACTION_SETf:
                process_mac_addr = 1;
                break;
            case PROFILED_ASSIGNMENT_DST_MAC_ADDRESS_ACTION_SETf:
                process_profiled_assignment_dst_mac = 1;
                break;
            case VXLT_CLASS_IDf:
                process_vxlt = 1;
                break;
            case NEXT_PTR_ACTION_SETf:
                process_next_ptr = 1;
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    if (process_vxlt) {
          soc_mem_field32_set(unit, view_id, egr_nh,
                              VXLT_CLASS_IDf, l3_egress->intf_class);
    }

    if (process_higig_controls) {
          action_set = 0;
          soc_format_field32_set(unit, HIGIG_CONTROLS_ACTION_SETfmt,
                               &action_set, HG_HDR_SELf, 1);

          soc_mem_field32_set(unit, view_id, egr_nh,
                              HIGIG_CONTROLS_ACTION_SETf, action_set);
    }

    /* Set interface id. */
    if (process_intf) {

          action_set = 0;
          soc_format_field32_set(unit, INTF_ACTION_SETfmt,
                               &action_set, INTF_TYPEf, 0);
          /* INTF_NUMf */
          soc_format_field32_set(unit, INTF_ACTION_SETfmt,
                               &action_set, INTF_PTRf, l3_egress->intf);
          soc_mem_field32_set(unit, view_id, egr_nh,
                              INTF_ACTION_SETf, action_set);
    }
    if (process_mac_addr) {
        /* L3UC__MAC_ADDRESSf  */
        soc_mem_mac_addr_set(unit, view_id, egr_nh,
                             DIRECT_ASSIGNMENT_6_ACTION_SETf,
                             l3_egress->mac_addr);
    }
#if 0
    for (i = 0; i < COUNTOF(flag_array); i++) {
        if (l3_egress->multicast_flags & flag_array[i]) {
            flag_set = 1;
        } else {
            flag_set = 0;
        }
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm, field_array[i])) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                    field_array[i], flag_set);
        } else {
            if (flag_set) {
                return BCM_E_PARAM;
            }
        }
    }
#endif

    if (process_l2_edit_controls) {
       action_set = 0;
       if (ipmc) {
          action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags,
                                              BCM_L3_MULTICAST_L2_VLAN_PRESERVE);
          soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, VLAN_DISABLEf,      /* L3MC__L2_MC_VLAN_DISABLEf */
                               action_set_field);
          action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags,
                                              BCM_L3_MULTICAST_L2_DEST_PRESERVE);
          soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, MACDA_DISABLEf,     /* L3MC__L2_MC_DA_DISABLEf */
                               action_set_field);
          action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags,
                                              BCM_L3_MULTICAST_L2_SRC_PRESERVE);
          soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, MACSA_DISABLEf,     /* L3MC__L2_MC_SA_DISABLEf */
                               action_set_field);
       }
       else {
           action_set_field = _SHR_IS_FLAG_SET(l3_egress->flags, BCM_L3_KEEP_VLAN);
           soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, VLAN_DISABLEf,      /* L3__L3_UC_VLAN_DISABLEf */
                               action_set_field);
           action_set_field = _SHR_IS_FLAG_SET(l3_egress->flags, BCM_L3_KEEP_DSTMAC);
           soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, MACDA_DISABLEf,     /* L3__L3_UC_DA_DISABLEf */
                               action_set_field);

           action_set_field = _SHR_IS_FLAG_SET(l3_egress->flags, BCM_L3_KEEP_SRCMAC);
           soc_format_field32_set(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                               &action_set, MACSA_DISABLEf,     /* L3__L3_UC_SA_DISABLEf */
                               action_set_field);
       }

       soc_mem_field32_set(unit, view_id, egr_nh,
                              L2_EDIT_CONTROLS_ACTION_SETf, action_set);
    }
    if (process_ttl_a) {
        if (ipmc) {
            soc_mem_field32_set(unit, view_id, egr_nh,
                              TTL_A_ACTION_SETf,   /* L3MC__L3_MC_TTL_DISABLEf */
                              _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_TTL_PRESERVE));
        }
        else {
            soc_mem_field32_set(unit, view_id, egr_nh,
                              TTL_A_ACTION_SETf,   /* L3__L3_UC_TTL_DISABLEf */
                              _SHR_IS_FLAG_SET(l3_egress->flags, BCM_L3_KEEP_TTL));
        }
    }

    if (process_class_id) {
            soc_mem_field32_set(unit, view_id, egr_nh,
                    CLASS_ID_ACTION_SETf,  /* L3__CLASS_IDf */
                    l3_egress->intf_class);
    }

    if (soc_feature(unit, soc_feature_virtual_port_routing)) {
        if (l3_egress->encap_id > 0 &&
            l3_egress->encap_id < BCM_XGS3_EGRESS_IDX_MIN) {
            /* encap_id contains the virtual port value if it's greater than 0 and
             * less than the starting value of egress object ID.
            */
            if (process_next_ptr) {
                action_set = 0;
                soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                     &action_set, NEXT_PTR_TYPEf,
                                     BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP);
                /* L3__DVPf */
                soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                     &action_set, NEXT_PTRf,
                                     l3_egress->encap_id);
                soc_mem_field32_set(unit, view_id, egr_nh,
                               NEXT_PTR_ACTION_SETf, action_set);
            }
            /* no support for _bcmVpTypeNiv, _bcmVpTypeExtender in flexflow */
        } else if (l3_egress->encap_id >= BCM_XGS3_EGRESS_IDX_MIN) {
            return BCM_E_PARAM;
        }
    }

    /* for SD-Tag & L3MC views */
    if (process_switching_controls) {
        if (ipmc) {
        action_set = 0;
            action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_L3_DROP);
            soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                               &action_set, L3_DROPf,      /* L3MC__L3_DROPf */
                               action_set_field);
            action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_L2_DROP);
            soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                               &action_set, L2_DROPf,      /* L3MC__L2_DROPf */
                               action_set_field);

        }
        else {
            action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_L3_DROP);
            soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                           &action_set, BC_DROPf,      /* SD_TAG__BC_DROPf */
                           action_set_field);

            action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_L2_DROP);
            soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                               &action_set, UUC_DROPf,      /* SD_TAG__UUC_DROPf */
                               action_set_field);

            action_set_field = _SHR_IS_FLAG_SET(l3_egress->multicast_flags, BCM_L3_MULTICAST_L2_DROP);
            soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                               &action_set, UMC_DROPf,      /* SD_TAG__UMC_DROPf */
                               action_set_field);

        }
        soc_mem_field32_set(unit, view_id, egr_nh,
                              SWITCHING_CTRLS_ACTION_SETf, action_set);
    }

    /* Write IPUC entry in caller _bcm_fb_nh_add, otherwise continue for IPMC */
    if (ipmc == 0) {
        return (BCM_E_NONE);
    }

#ifdef BCM_RIOT_SUPPORT
    /*
     * TD2Plus has multi layer routing.
     * The multicast packet can be routed in the overlay
     * layer and then underlay encapsulation is performed.
     * To achieve this, there will be overlay egr nexthop association
     * with undelray egress next hop.
     * the overlay egr nexthop will point to underlay next hop.
     */
    if (process_next_ptr &&
        BCMI_RIOT_IS_ENABLED(unit) &&
        _bcm_vp_vfi_type_vp_get(unit, l3_egress->port, (int *)&vp)) {
        /* This is a overlay NH case */
        ing_dvp_table_entry_t dvp;
        int vp_nh_index;
        /* Get the vp */
        SOC_IF_ERROR_RETURN
                (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        /* read vp to get next hop */
        vp_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                NEXT_HOP_INDEXf);

        /* 
         * check if nh-nh association is needed.
         * else set up nh-dvp association
         * NH-NH association will be done for all the 
         * nhs other than SDTAG which does not modify any packets field.
         * e.g. on access ports, it may be needed to change a vlan/tpid, 
         * in that case, we may have to set nh-nh association else we may
         * setup vlan/tpid from DVP.
         */
        if (bcmi_l3_nh_assoc_ol_ul_link_is_required(unit, vp_nh_index)) {
            action_set = 0;
            action_set_field = BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH;
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTR_TYPEf,       /* L3MC__NEXT_PTR_TYPEf */
                                 action_set_field);
            action_set_field = (uint32)vp_nh_index;
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTRf,
                                 action_set_field);  /* L3MC__NEXT_PTRf */
            /* Set up the association of NHs in sw */
            BCM_IF_ERROR_RETURN(
                bcmi_l3_nh_assoc_ol_ul_link_sw_add(unit, nh_index, 
                vp_nh_index));
        } else {
            action_set = 0;
            action_set_field = BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP;
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTR_TYPEf,       /* L3MC__NEXT_PTR_TYPEf */
                                 action_set_field);
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTRf,
                                 vp);  /* L3MC__NEXT_PTRf */
        }
        soc_mem_field32_set(unit, view_id, egr_nh,
                              NEXT_PTR_ACTION_SETf, action_set);
    }
#endif

    /* 
     * TD2Plus has a overlaid  mac_address.
     * This mac_address field may be overlaid by some other fields
     * used for CoE/HG feature. Meanwhile a mac_da_profile_index
     * is present for the actual mac_address.
     * This index points to EGR_MAC_DA_PROFILE, which is managed in
     * Profile mechanism.
     */
#ifdef BCM_HGPROXY_COE_SUPPORT
    if (process_higig_change_dest &&
        _bcm_xgs5_subport_coe_mod_port_local(unit, l3_egress->module,
                                             l3_egress->port)) {

        action_set = 0;
        action_set_field = l3_egress->module;
        soc_format_field32_set(unit, HIGIG_CHANGE_DEST_ACTION_SETfmt,
                             &action_set, HG_MC_DST_MODIDf,     /* L3MC__HG_MC_DST_MODIDf */
                             action_set_field);
        action_set_field = l3_egress->port;
        soc_format_field32_set(unit, HIGIG_CHANGE_DEST_ACTION_SETfmt,
                             &action_set, HG_MC_DST_PORT_NUMf,  /* L3MC__HG_MC_DST_PORT_NUMf */
                             action_set_field);
        soc_mem_field32_set(unit, view_id, egr_nh,
                            HIGIG_CHANGE_DEST_ACTION_SETf, action_set);
            
    }
#endif
    if (process_profiled_assignment_dst_mac) {
        egr_mac_da_profile_entry_t macda;
        void *entries[1] = { NULL };

        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, 
                             &macda, MAC_ADDRESSf, l3_egress->mac_addr);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *)&macda_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
       /* L3MC__MAC_DA_PROFILE_INDEXf */
       soc_mem_field32_set(unit, view_id, egr_nh,
                            EGR_MAC_ADDRESS_PROFILEf, macda_index);
    }
    else {
        if (!BCM_MAC_IS_ZERO(l3_egress->mac_addr)) {
            if (!soc_feature(unit, soc_feature_ipmc_use_configured_dest_mac)) {
                return BCM_E_CONFIG;
            }
        }
    }

    rv = soc_mem_write(unit, view_id,
                       MEM_BLOCK_ALL, nh_index, egr_nh);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    return BCM_E_NONE;

cleanup:
    if (macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, macda_index);
    }
    return rv;
}
/*
 * Function:
 *      __bcm_td3_l3_flex_nh_entry_parse
 * Purpose:
 *      Parse the Flex NextHop table entry info.
 * Parameters:
 *      unit          - (IN) SOC unit number.
 *      egr_entry_ptr - (IN) Egress next hop table entry.
 *      index         - (IN) Next hop index.
 *      nh_entry      - (OUT) Next hop entry info.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_td3_l3_flex_nh_entry_parse(int unit, uint32 *egr_entry_ptr, 
        uint32 index, bcm_l3_egress_t *nh_entry)
{

    int rv = BCM_E_NONE;
    uint32  view_id;
    uint32  flex_data_array[128];
    uint32  flex_data_count;
    int     ipmc=0;
    int     nh_index_out;
    int     i;
    _bcm_l3_intf_cfg_t intf_info;
    int ret_val;
    uint32 next_ptr_type;
    uint32 next_ptr;

    bcm_flow_logical_field_t* user_fields;
    uint32 opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 opaque_count;
    uint32 action_set = 0;
    uint32 action_set_field = 0;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;

    uint8 process_l2_edit_controls = 0;
    uint8 process_switching_controls = 0;
    uint8 process_profiled_assignment_dst_mac = 0;
    uint8 process_ttl_a = 0;
    uint8 process_next_ptr = 0;
    uint8 process_intf = 0;
    uint8 process_mac_addr = 0;
    uint8 process_class_id = 0;
    uint8 process_vxlt = 0;
    uint32 data_type;

    /* Input parameters check */
    if (NULL == nh_entry) {
        return (BCM_E_PARAM);
    }

    if (NULL == egr_entry_ptr) {
        return (BCM_E_PARAM);
    }

    /* Get view id corresponding to the specified flow. */
    if (nh_entry->flow_handle != 0) {
        BCM_IF_ERROR_RETURN(
            soc_flow_db_ffo_to_mem_view_id_get(unit,
                          nh_entry->flow_handle,
                          nh_entry->flow_option_handle,
                          SOC_FLOW_DB_FUNC_EGRESS_OBJ_ID,
                          &view_id));
    }
    else {
        data_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                        egr_entry_ptr, DATA_TYPEf);

        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_to_view_id_get(unit,
                             EGR_L3_NEXT_HOPm,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id));

        /* Get the flow handle, function id, option id */
        BCM_IF_ERROR_RETURN(
              soc_flow_db_mem_view_to_ffo_get(unit,
                                         view_id,
                                         SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                         ffo,
                                         &num_ffo));
        nh_entry->flow_handle = ffo[0].flow_handle;
        nh_entry->flow_option_handle = ffo[0].option_id;
    }

    /* Get logical field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = nh_entry->logical_fields;

    for (i=0; i<opaque_count; i++) {
        user_fields[i].id = opaque_array[i];
        user_fields[i].value = soc_mem_field32_get(unit, view_id, egr_entry_ptr, opaque_array[i]);
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case L2_EDIT_CONTROLS_ACTION_SETf:
                process_l2_edit_controls = 1; 
                break;
            case SWITCHING_CTRLS_ACTION_SETf:
                process_switching_controls = 1; 
                break;
            case TTL_A_ACTION_SETf:
                process_ttl_a = 1; 
                break;
            case HIGIG_CONTROLS_ACTION_SETf:
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1; 
                break;
            case INTF_ACTION_SETf:
                process_intf = 1; 
                break;
            case DIRECT_ASSIGNMENT_6_ACTION_SETf:
                process_mac_addr = 1; 
                break;
            case PROFILED_ASSIGNMENT_DST_MAC_ADDRESS_ACTION_SETf:
                process_profiled_assignment_dst_mac = 1; 
                break;
            case VXLT_CLASS_IDf:
                process_vxlt = 1;
                break;
            case NEXT_PTR_ACTION_SETf:
                process_next_ptr = 1;
                break;
            default:
                /* Unrecongnized acion set */
                return BCM_E_INTERNAL;
        }
    }

    if (process_vxlt) {
        nh_entry->intf_class = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                               VXLT_CLASS_IDf);
    }

   if (process_l2_edit_controls) {
       action_set = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                               L2_EDIT_CONTROLS_ACTION_SETf);

       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, L3_UC_VLAN_DISABLEf)) {
           /* L3__L3_UC_VLAN_DISABLEf */
           nh_entry->flags |= BCM_L3_KEEP_VLAN;
       }

       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, L3_UC_DA_DISABLEf)) {
           /* L3__L3_UC_DA_DISABLEf */
           nh_entry->flags |= BCM_L3_KEEP_DSTMAC;
       }

       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, L3_UC_SA_DISABLEf)) {
           /* L3__L3_UC_SA_DISABLEf */
           nh_entry->flags |= BCM_L3_KEEP_SRCMAC;
       }
       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, MACDA_DISABLEf)) {
           /* L3MC__L2_MC_DA_DISABLEf */
           nh_entry->flags |= BCM_L3_MULTICAST_L2_DEST_PRESERVE;
       }
       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, MACSA_DISABLEf)) {
           /* L3MC__L2_MC_SA_DISABLEf */
           nh_entry->flags |= BCM_L3_MULTICAST_L2_SRC_PRESERVE;
       }
       if (soc_format_field32_get(unit, L2_EDIT_CONTROLS_ACTION_SETfmt,
                                &action_set, VLAN_DISABLEf)) {
           /* L3MC__L2_MC_VLAN_DISABLEf */
           nh_entry->flags |= BCM_L3_MULTICAST_L2_VLAN_PRESERVE;
       }
   }

   if (process_intf) {
       action_set = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                               INTF_ACTION_SETf);

       action_set_field = soc_format_field32_get(unit, INTF_ACTION_SETfmt,
                               &action_set, INTF_PTRf); /* INTF_NUMf */
       nh_entry->intf = action_set_field;

        /* determine if this is an IPMC entry from the l3 interface */
        _bcm_th_ipmc_l3_intf_next_hop_get(unit, nh_entry->intf,
                                          &nh_index_out);
        if (nh_index_out == -2) {
            ipmc = 1;
        }

   }
   if (nh_entry->intf == BCM_XGS3_L3_L2CPU_INTF_IDX(unit)) {
        nh_entry->flags |= BCM_L3_L2TOCPU;
   }
   if (BCM_XGS3_L3_INTF_VLAN_SPLIT_EGRESS_MODE_ISSET(unit)) {
        /* Now get the vlan */
        sal_memset(&intf_info, 0x0, sizeof(intf_info));
        intf_info.l3i_index = nh_entry->intf;

        if (BCM_XGS3_L3_HWCALL_CHECK(unit, if_get)) {
            BCM_XGS3_L3_MODULE_LOCK(unit);
            ret_val = BCM_XGS3_L3_HWCALL_EXEC(unit, if_get) (unit,
                                                             &intf_info);
            BCM_XGS3_L3_MODULE_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(ret_val);
            nh_entry->vlan = intf_info.l3i_vid;
        }
    }

    if (process_profiled_assignment_dst_mac) {
       int macda_idx;
       egr_mac_da_profile_entry_t macda;

       macda_idx = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                            EGR_MAC_ADDRESS_PROFILEf);

       /* Read entry from MAC-DA-PROFILEm */
       BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_MAC_DA_PROFILEm,
                            MEM_BLOCK_ANY, macda_idx, &macda));

       /* Obtain Mac-Address */
       soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm,
                            &macda, MAC_ADDRESSf, nh_entry->mac_addr);

    }
    else if (process_mac_addr) {
        /* L3MC__MAC_ADDRESSf  */
        soc_mem_mac_addr_get(unit, view_id, egr_entry_ptr,
                             DIRECT_ASSIGNMENT_6_ACTION_SETf,
                             nh_entry->mac_addr);
    }
    
    if (process_ttl_a) {
       if (soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                               TTL_A_ACTION_SETf)) {
           if (ipmc) {
               /* L3MC__L3_MC_TTL_DISABLEf */
               nh_entry->flags |= BCM_L3_MULTICAST_TTL_PRESERVE;
           }
           else {
               /* L3__L3_UC_TTL_DISABLEf */
               nh_entry->flags |= BCM_L3_KEEP_TTL;
           }
       } 
    }

    if (process_switching_controls) {
       action_set = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                              SWITCHING_CTRLS_ACTION_SETf);

       if (soc_format_field32_get(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                                &action_set, L3_DROPf)) {
           /* L3MC__L3_DROPf */
           nh_entry->flags |= BCM_L3_MULTICAST_L3_DROP;
       }
       if (soc_format_field32_get(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                                &action_set, L2_DROPf)) {
           /* L3MC__L2_DROPf */
           nh_entry->flags |= BCM_L3_MULTICAST_L2_DROP;
       }
    }

    if (process_next_ptr) {
        action_set = soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                              NEXT_PTR_ACTION_SETf);

       /* L3MC__NEXT_PTR_TYPEf */
         action_set_field =
           soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTR_TYPEf);

         next_ptr_type = action_set_field;
         action_set_field =
           soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTRf);

         next_ptr = action_set_field;
         if (next_ptr_type == BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP) {
             nh_entry->encap_id = next_ptr;
         }
         else if (next_ptr_type ==
                  BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_VC_SWAP) {
              int vc_swap_idx = next_ptr;

              if (vc_swap_idx > 0) { /* 0th entry is the default */
                 /* Read entry from EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm */

                 BCM_IF_ERROR_RETURN (
                 _bcm_td3_mpls_gre_label_get (unit, vc_swap_idx, nh_entry));
             }
             else {
                nh_entry->mpls_label = BCM_MPLS_LABEL_INVALID;
             }
         }
    }

    if (process_class_id) {  /* L3__CLASS_IDf */
        nh_entry->intf_class =
                 soc_mem_field32_get(unit, view_id, egr_entry_ptr,
                    CLASS_ID_ACTION_SETf);
    }

    if (ipmc) {
        nh_entry->flags |= BCM_L3_IPMC; 
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *      bcm_td3_l3_riot_init
 * Purpose:
 *      Initialize internal L3 tables and enable L3.
 *      TD3 has configurable multi domain banks for 
 *      various l3 objects. This routine sets up h/w 
 *      tables for corresponding objects and then calls 
 *      common tables init routine.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcmi_td3_l3_riot_init(int unit)
{

#ifdef BCM_RIOT_SUPPORT
    /* Selects L3 banks based on user inputs */
    BCM_IF_ERROR_RETURN(bcmi_td3_l3_riot_bank_sel(unit));
#if 0
    /* selects ecmp level */
    BCM_IF_ERROR_RETURN(bcmi_l3_riot_ecmp_level_sel(unit));
#endif
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *       bcmi_l3_nh_assoc_ol_ul_link_hash_dump 
 * Purpose:
 *       dumps the whole hash table
 * Parameters:
 *           IN :  Unit
 * Returns:
 *        void
 */
void bcm_td3_l3_nh_assoc_ol_ul_link_hash_dump(int unit)
{   

    td3_ol_nh_link_t *temp;
    td3_ul_nh_link_t *hash;
    int i = 0;

    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
        "printing nh association hash table\n")));

    for (; i< BCM_L3_NH_NUM_UNDERLAY_BUCKETS; i++) {
        hash =  td3_ul_nh_assoc_head[unit][i];
        while (hash != NULL) {
            temp = hash->ol_nh_assoc;
            while (temp != NULL) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                    "bkt_idx = %d : ol index = %d : ul index = %d \n"),
                    i, hash->u_nh_idx, temp->o_nh_idx));
                temp = temp->next_link;
            }
            hash = hash->ul_nh_link;
        }
    }
}

/*
 * Function:
 *      bcmi_td3_nh_assoc_alloc_ol_link 
 * Purpose:
 *      Allocates link memory and sets overlay nh.
 * Parameters:
 *      ol_nh - overlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */

td3_ol_nh_link_t *
bcmi_td3_nh_assoc_alloc_ol_link(int ol_nh)
{
    td3_ol_nh_link_t *ptr ;

    ptr = (td3_ol_nh_link_t *) sal_alloc(sizeof(td3_ol_nh_link_t), "ol_nh_link" );

    sal_memset(ptr, 0, sizeof(td3_ol_nh_link_t));
    ptr->o_nh_idx = ol_nh;
    return ptr;
}

/*
 * Function:
 *      bcmi_td3_nh_assoc_alloc_ul_link 
 * Purpose:
 *      Allocates link memory and sets underlay nh.
 * Parameters:
 *      ul_nh - underlay nh number
 * Returns:
 *      pointer to allocated link memory              
 */
td3_ul_nh_link_t *
bcmi_td3_nh_assoc_alloc_ul_link(int ul_nh)
{   
    td3_ul_nh_link_t *ptr;

    ptr = (td3_ul_nh_link_t *) sal_alloc(sizeof(td3_ul_nh_link_t), "ul_nh_link");
    sal_memset(ptr, 0, sizeof(td3_ul_nh_link_t));

    ptr->u_nh_idx = ul_nh;
    return ptr;
}


/*
 * Function:
 *      bcmi_td3_nh_assoc_bkt_get
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_nh_assoc_bkt_get(int nh_idx)
{


    return( _shr_crc16b(0, (uint8 *)(&nh_idx),
        BYTES2BITS(sizeof(nh_idx))) %
        BCM_L3_NH_NUM_UNDERLAY_BUCKETS);

}
/*
 * Function:
 *      bcmi_td3_l3_nh_assoc_ol_ul_link_sw_add
 * Purpose:
 *      Adds a overlay/underlay association.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_l3_nh_assoc_ol_ul_link_sw_add(int unit, int ol_nh, int ul_nh)
{   

    td3_ul_nh_link_t *ul_assoc, *prev_assoc, *ul_alloc;
    td3_ol_nh_link_t *ol_link, *prev_link, *ol_alloc;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_td3_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = td3_ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link && (ol_link->o_nh_idx != ol_nh)) {
            prev_link = ol_link;
            ol_link = ol_link->next_link;
        }
        if (!ol_link) {
            ol_alloc =  bcmi_td3_nh_assoc_alloc_ol_link(ol_nh);
            if (ol_alloc == NULL) {
                return BCM_E_MEMORY;
            }
            prev_link->next_link = ol_alloc;
        }
    } else {

        ul_alloc = bcmi_td3_nh_assoc_alloc_ul_link(ul_nh);
        if (ul_alloc == NULL) {
            return BCM_E_MEMORY;
        }
        ol_alloc = bcmi_td3_nh_assoc_alloc_ol_link(ol_nh);
        if (ol_alloc == NULL) {
            sal_free(ul_alloc);
            return BCM_E_MEMORY;
        }

        if (prev_assoc) {
            prev_assoc->ul_nh_link = ul_alloc;
            prev_assoc->ul_nh_link->ol_nh_assoc = ol_alloc;
        } else {
            td3_ul_nh_assoc_head[unit][bkt_idx] = ul_alloc;
            td3_ul_nh_assoc_head[unit][bkt_idx]->ol_nh_assoc = ol_alloc;
        }
    }
    /* 
     * increase the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_INC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_l3_nh_del_nh_nh_assoc 
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 *      nh_idx - next hop index
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_td3_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx)
{
    soc_mem_t mem;                        /* Table location memory. */
    egr_l3_next_hop_entry_t  egr_entry;   /* Egress next hop entry. */
    int ent_type, next_ptr_type, next_nh_idx;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    mem = EGR_L3_NEXT_HOPm;
    sal_memset(&egr_entry, 0, sizeof(egr_l3_next_hop_entry_t));

    SOC_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, EGR_L3_NEXT_HOPm,
                                          nh_idx, &egr_entry));

    ent_type = soc_mem_field32_get(unit, mem, &egr_entry, DATA_TYPEf);

    /* chk if the entry is L3MC */
    if (ent_type == BCMI_TD3_L3_EGR_NH_MCAST_ENTRY_TYPE) {
        next_ptr_type = soc_mem_field32_get
            (unit, mem, &egr_entry, L3MC__NEXT_PTR_TYPEf);

        if (next_ptr_type  == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
            next_nh_idx = soc_mem_field32_get(unit, mem, &egr_entry,
                              L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_td3_l3_nh_assoc_ol_ul_link_sw_delete(unit, nh_idx, 
                next_nh_idx));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_l3_nh_assoc_ol_ul_link_sw_delete
 * Purpose:
 *      Deletes a overlay/underlay association.
 *      If there is only one underlay nh link, 
 *      then remove the overlay link also.
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh, int ul_nh)
{
    td3_ul_nh_link_t *ul_assoc, *prev_assoc;
    td3_ol_nh_link_t *link_nh, *prev_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_td3_nh_assoc_bkt_get(ul_nh);

    prev_assoc = ul_assoc = td3_ul_nh_assoc_head[unit][bkt_idx];

    while (ul_assoc && (ul_assoc->u_nh_idx != ul_nh)) {
        prev_assoc = ul_assoc;
        ul_assoc = ul_assoc->ul_nh_link;
    }

    if (ul_assoc) {
        prev_link = link_nh = ul_assoc->ol_nh_assoc;
        while (link_nh && (link_nh->o_nh_idx != ol_nh)) {
            prev_link = link_nh;
            link_nh = link_nh->next_link;
        }

        if (link_nh) {
            if (link_nh == ul_assoc->ol_nh_assoc) {
                ul_assoc->ol_nh_assoc = link_nh->next_link;
            } else {
                prev_link->next_link = link_nh->next_link;
            }
            sal_free(link_nh);
        } else {
            /* send an error here */
            return BCM_E_NOT_FOUND;
        }
    }

    if (ul_assoc && (ul_assoc->ol_nh_assoc == NULL)) {
        if (ul_assoc == td3_ul_nh_assoc_head[unit][bkt_idx]) {
            td3_ul_nh_assoc_head[unit][bkt_idx] = ul_assoc->ul_nh_link;
        } else {
            prev_assoc->ul_nh_link = ul_assoc->ul_nh_link;
        }
        sal_free(ul_assoc);
    } else if (ul_assoc == NULL) {
        return BCM_E_NOT_FOUND;
    }
    /* 
     * decrease the ref count of second next hop so that it
     * not deleted in case someone tries to delete it. 
     */
    BCM_XGS3_L3_ENT_REF_CNT_DEC 
        (BCM_XGS3_L3_TBL_PTR(unit, next_hop), ul_nh,
        _BCM_SINGLE_WIDE); 

 return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td3_l3_nh_assoc_ol_ul_link_replace
 * Purpose:
 *      Replace overlay/underlay association.
 *
 * Parameters:
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_l3_nh_assoc_ol_ul_link_replace(int unit, int old_ul, int new_ul)
{   

    int entry_type, next_ptr_type;
    egr_l3_next_hop_entry_t egr_nh;
    td3_ul_nh_link_t *ul_assoc;
    td3_ol_nh_link_t *ol_link;
    int bkt_idx = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    bkt_idx = bcmi_td3_nh_assoc_bkt_get(old_ul);

    if (td3_ul_nh_assoc_head[unit][bkt_idx] == NULL) {
        /* There is nothing to replace */
        return BCM_E_NONE;
    }
    ul_assoc = td3_ul_nh_assoc_head[unit][bkt_idx];
    while (ul_assoc && (ul_assoc->u_nh_idx != old_ul)) {
        ul_assoc = ul_assoc->ul_nh_link;
    }
    /* replace underlay nh index */
    if (ul_assoc) {
        ol_link = ul_assoc->ol_nh_assoc;
        while (ol_link) {

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                ol_link->o_nh_idx, &egr_nh));
            entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, DATA_TYPEf);

            if (entry_type == BCMI_TD3_L3_EGR_NH_MCAST_ENTRY_TYPE) {
                next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, L3MC__NEXT_PTR_TYPEf);
                if (next_ptr_type == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {

                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf, BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L3MC__NEXT_PTRf, new_ul);

                    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, ol_link->o_nh_idx, &egr_nh));
                }
            }
        /* For all overlay NHs, replace the underlay NH. */
        bcmi_td3_l3_nh_assoc_ol_ul_link_sw_delete(unit, ol_link->o_nh_idx, 
            old_ul);
        bcmi_td3_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_link->o_nh_idx, 
            new_ul);
        ol_link = ol_link->next_link;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_l3_nh_assoc_ol_ul_link_is_required 
 * Purpose:
 *      check if nh-nh association is needed.
 * Parameters:
 *      unit        - unit number
 *      vp nh index - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index)
{ 
    int entry_type = 0;
    egr_l3_next_hop_entry_t egr_nh;
    int action_present=0, action_not_present=0, vntag_actions = 0;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                  vp_nh_index, &egr_nh));
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);

    if (entry_type == BCMI_TD3_L3_EGR_NH_SD_TAG_ENTRY_TYPE) {
        /* check if sdtag egr nh has some tag actions */
        action_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        vntag_actions =
             soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         SD_TAG__VNTAG_ACTIONSf);

        if (vntag_actions || action_present || action_not_present) {
            /* 
             * this next hop can do some modifications and therefore
             * we should set nh-nh association for this NH.
             */
            return 1;
        } else {
            return 0;
        }
    }

    return 1;
}

/*
 * Function:
 *      bcmi_td3_l3_nh_assoc_ol_ul_link_reinit
 * Purpose:
 *      re-initializes overlay/underlay association.
 * Parameters:
 *      unit     - unit number
 *      ol_nh - overlay nh number
 *      ul_nh - underlay nh number
 * Returns:
 *      BCM_ERROR_T              
 */
int 
bcmi_td3_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx)
{
    int entry_type, next_ptr_type, ul_nh_idx;
    egr_l3_next_hop_entry_t egr_nh;

    if (!(BCMI_RIOT_IS_ENABLED(unit))) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, ol_nh_idx, &egr_nh));

    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);


    if (entry_type == BCMI_TD3_L3_EGR_NH_MCAST_ENTRY_TYPE) { 
        next_ptr_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
            &egr_nh, L3MC__NEXT_PTR_TYPEf);
        if (next_ptr_type == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
        
            ul_nh_idx =  soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                &egr_nh, L3MC__NEXT_PTRf);

            BCM_IF_ERROR_RETURN(
                bcmi_td3_l3_nh_assoc_ol_ul_link_sw_add(unit, ol_nh_idx, 
                ul_nh_idx));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_get_gport_from_destination
 * Purpose:
 *      Encode the dstination value based on the gport.
 * Parameters:
 *      unit - SOC device unit number
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               ERROR
 */
int bcmi_td3_get_port_from_destination(int unit, uint32 dest,
                                        bcm_l3_egress_t *nh)
{
    int vp;

    if (BCMI_TD3_L3_NH_DEST_IS_LAG(dest)) {
        nh->port = BCMI_TD3_L3_NH_PORT_LAG_GET(dest);
        nh->flags |= BCM_L3_TGID;
    } else if (BCMI_TD3_L3_NH_DEST_IS_DGLP(dest)) {
        nh->port = BCMI_TD3_L3_NH_PORT_DGLP_GET(dest);
    } else if (BCMI_TD3_L3_NH_DEST_IS_DVP(dest)) {
        vp = BCMI_TD3_L3_NH_PORT_DVP_GET(dest);
        _bcm_vp_encode_gport(unit, vp, &(nh->port));
    } else {
        nh->port = dest;
    }

    return BCM_E_NONE;

}

#endif /* BCM_RIOT_SUPPORT */

/*
 * Function:
 *      _bcm_td3_l3_flex_intf_opri_ocfi_set
 * Purpose:
 *      Set L3 Interface OVLAN QoS parameters
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - (IN) Pointer to buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_flex_intf_opri_ocfi_set(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    int hw_map_idx=0;

    uint32 action_set = 0;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY) {
        /* OPRI_OCFI_SELf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, PCP_DE_SELf, 0x0);

    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET) {
        /* OPRI_OCFI_SELf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, PCP_DE_SELf, 0x1);
        /* OPRIf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, PCPf, intf_info->vlan_qos.pri);
        /* OCFIf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, DEf, intf_info->vlan_qos.cfi);

    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK) {
        /* OPRI_OCFI_SELf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, PCP_DE_SELf, 0x2);

        BCM_IF_ERROR_RETURN
         (_bcm_tr2_qos_id2idx(unit, intf_info->vlan_qos.qos_map_id,
                             &hw_map_idx));
        /* OPRI_OCFI_MAPPING_PROFILEf */
        soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                             &action_set, PCP_DE_MAPPING_PTRf,
                             hw_map_idx);
    }

    soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                             OTAG_ACTION_SETf, action_set);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_intf_ipri_icfi_set
 * Purpose:
 *      Set L3 Interface IVLAN QoS parameters
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - (IN) Pointer to buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_flex_intf_ipri_icfi_set(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    int hw_map_idx=0;
    uint32 action_set = 0;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY) {
        /* IPRI_OCFI_SELf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt, &action_set,
                             PCP_DE_SELf, 0x0);

   } else if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET) {
        /* IPRI_OCFI_SELf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt, &action_set,
                             PCP_DE_SELf, 0x1);
        /* IPRIf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt,
                       &action_set, PCPf, intf_info->vlan_qos.pri);
        /* ICFIf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt, &action_set,
                             DEf, intf_info->vlan_qos.cfi);

   } else if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK) {
        /* IPRI_OCFI_SELf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt, &action_set,
                             PCP_DE_SELf, 0x2);

        BCM_IF_ERROR_RETURN
         (_bcm_tr2_qos_id2idx(unit, intf_info->vlan_qos.qos_map_id,
                             &hw_map_idx));
        /* IPRI_ICFI_MAPPING_PROFILEf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt, &action_set,
                             PCP_DE_MAPPING_PTRf, hw_map_idx);

   }

   soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                             ITAG_ACTION_SETf, action_set);

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_intf_dscp_set
 * Purpose:
 *      Set L3 Interface DSCP parameters
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - (IN) Pointer to buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_flex_intf_dscp_set(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    int hw_map_idx=0;
    uint32 action_set = 0;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

        if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_COPY) {
         /* DSCP_SELf */
         soc_format_field32_set(unit, QOS_A_ACTION_SETfmt,
                        &action_set, QOS_ACTION_SELf, 0x0);
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_SET) {
         /* DSCP_SELf */
         soc_format_field32_set(unit, QOS_A_ACTION_SETfmt, &action_set,
                              QOS_ACTION_SELf, 0x1);
         /* DSCPf */
         soc_format_field32_set(unit, QOS_A_ACTION_SETfmt,
                    &action_set, QOS_VALUE_OR_MAPPING_PTR_OR_ZONE_IDf,
                    intf_info->dscp_qos.dscp);
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_REMARK) {
         /* DSCP_SELf */
         soc_format_field32_set(unit, QOS_A_ACTION_SETfmt,
                       &action_set, QOS_ACTION_SELf, 0x2);
         BCM_IF_ERROR_RETURN
              (_bcm_tr2_qos_id2idx(unit, intf_info->dscp_qos.qos_map_id,
                                   &hw_map_idx));
         /* DSCP_MAPPING_PTRf */
         soc_format_field32_set(unit, QOS_A_ACTION_SETfmt, &action_set,
                              QOS_VALUE_OR_MAPPING_PTR_OR_ZONE_IDf,
                              hw_map_idx);
    }

    soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                        QOS_A_ACTION_SETf, action_set);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_l3_flex_intf_add
 * Purpose:
 *      Add an entry to L3 interface table.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN)Interface information.
 *      l3_if_entry_p - (IN)  Write buffer address.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_l3_flex_intf_add(int unit, _bcm_l3_intf_cfg_t *intf_info, uint32 *l3_if_entry_p)
{
    int rv = BCM_E_NONE;
    int i;
    uint32  view_id;
    uint32  flex_data_array[128];
    uint32  flex_data_count;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;
    uint8  process_fwd_domain = 0;
    uint8  process_otag = 0;
    uint8  process_itag = 0;
    uint8  process_dscp = 0;
    uint8  process_tag_action_profile = 0;
    uint8  process_tunnel_ptr = 0;
    uint8  process_class_id = 0;
    uint8  process_vxlt = 0;
    uint8  process_outer_sa = 0;
    /*uint8  process_inner_sa = 0;*/
    uint8  process_ttl_b = 0;
    uint8  process_svp = 0;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (NULL == l3_if_entry_p) {
        return (BCM_E_PARAM);
    }

    /* Get memory fields information.
    fld = (_bcm_l3_intf_fields_t *)BCM_XGS3_L3_MEM_FIELDS(unit, intf);
    */

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          intf_info->flow_handle,
                          intf_info->flow_option_handle,
                          SOC_FLOW_DB_FUNC_EGRESS_INTF_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, l3_if_entry_p);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = intf_info->logical_fields;

    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, l3_if_entry_p, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case FWD_DOMAIN_ACTION_SETf:
                process_fwd_domain = 1;
                break;
            case OTAG_ACTION_SETf:
                process_otag = 1;
                break;
            case ITAG_ACTION_SETf:
                process_itag = 1;
                break;
            case QOS_A_ACTION_SETf:
                process_dscp = 1;
                break;
            case TAG_ACTION_PROFILE_ACTION_SETf:
                process_tag_action_profile = 1;
                break;
            case TUNNEL_PTR_ACTION_SETf:
                process_tunnel_ptr = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case VXLT_CLASS_IDf:
                process_vxlt = 1;
                break;
            case DIRECT_ASSIGNMENT_7_ACTION_SETf:
                process_outer_sa = 1;
                break;
/*
            case DIRECT_ASSIGNMENT_5_ACTION_SETf:
                process_inner_sa = 1;
                break;
*/
            case TTL_B_ACTION_SETf:
                process_ttl_b = 1;
                break;
            case SVP_ACTION_SETf:
                process_svp = 1;
                break;
            default:
                /* Unrecongnized acion set */
                return BCM_E_INTERNAL;
        }
    }

    if (process_vxlt) {
          soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                              VXLT_CLASS_IDf, intf_info->l3i_class);
    }

    /* Set mac address. */
    if (process_outer_sa) {
        /* fld->mac_addr */
        soc_mem_mac_addr_set(unit, view_id, l3_if_entry_p,
                             DIRECT_ASSIGNMENT_7_ACTION_SETf,
                             intf_info->l3i_mac_addr);
    }
#ifdef BCM_RIOT_SUPPORT
    if (BCMI_L3_INTF_OVERLAY_VALID(unit)) {
        int svp = intf_info->l3i_source_vp;
        if (BCM_GPORT_IS_VXLAN_PORT(svp)) {
            svp = BCM_GPORT_VXLAN_PORT_ID_GET(svp);
        } else if (BCM_GPORT_IS_L2GRE_PORT(svp)) {
            svp = BCM_GPORT_L2GRE_PORT_ID_GET(svp);
        } else if (BCM_GPORT_IS_FLOW_PORT(svp)) {
            svp = BCM_GPORT_FLOW_PORT_ID_GET(svp);
        } else if (BCM_GPORT_IS_MIM_PORT(svp)) {
            svp = BCM_GPORT_MIM_PORT_ID_GET(svp);
        } else if (BCM_GPORT_IS_MPLS_PORT(svp)) {
            svp = BCM_GPORT_MPLS_PORT_ID_GET(svp);
        }

        if (process_svp) {
            soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            SVP_ACTION_SETf, svp);     /* SVPf */
        }

        if (process_fwd_domain) {
            action_set = 0;

            /* VLAN_VFI_OVERLAY_TYPEf */
            soc_format_field32_set(unit, FWD_DOMAIN_ACTION_SETfmt,
                &action_set, FORWARDING_DOMAIN_TYPEf,
                BCMI_RIOT_VPN_VFI_IS_SET(unit, intf_info->l3i_vid) ? 0x2 : 0x1 );

            if (BCMI_RIOT_VPN_VFI_IS_SET(unit, intf_info->l3i_vid)) {
                int vfi_idx = 0;
                _BCM_VPN_GET(vfi_idx, _BCM_VPN_TYPE_VFI, intf_info->l3i_vid);
                /* VFIf */
                soc_format_field32_set(unit, FWD_DOMAIN_ACTION_SETfmt,
                          &action_set, FORWARDING_DOMAINf, vfi_idx);
            } else {
                /* fld->vlan_id */
                soc_format_field32_set(unit, FWD_DOMAIN_ACTION_SETfmt,
                          &action_set, FORWARDING_DOMAINf, intf_info->l3i_vid);
            }
            soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            FWD_DOMAIN_ACTION_SETf, action_set);
        }
    } else
#endif
    {
        /* Set vlan id. */
        if (process_fwd_domain) {
            action_set = 0;
            /* fld->vlan_id */
            soc_format_field32_set(unit, FWD_DOMAIN_ACTION_SETfmt,
                       &action_set, FORWARDING_DOMAINf, intf_info->l3i_vid);
            soc_format_field32_set(unit, FWD_DOMAIN_ACTION_SETfmt,
                       &action_set, FORWARDING_DOMAIN_TYPEf, 0x1 );

            soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            FWD_DOMAIN_ACTION_SETf, action_set);
        }
    }
    /* Set Time To Live. */
    if (process_ttl_b) {
        action_set = 0;
        /* fld->ttl */
        soc_format_field32_set(unit, TTL_B_ACTION_SETfmt,
                       &action_set, TTL_THRESHOLDf, intf_info->l3i_ttl);

        soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            TTL_B_ACTION_SETf, action_set);
    }

    /* Set tunnel index. */
    if (process_tunnel_ptr) {
        action_set = 0;
        /* fld->tnl_id */
        soc_format_field32_set(unit, TUNNEL_PTR_ACTION_SETfmt,
                       &action_set, TUNNEL_INDEXf, intf_info->l3i_tunnel_idx);

        soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            TUNNEL_PTR_ACTION_SETf, action_set);
    }

    /* Set L2 bridging only flag. */
    if (intf_info->l3i_flags & BCM_L3_L2ONLY) {
        /* fld->l2_switch not supported in flex view */
        return BCM_E_PARAM;
    }

    if (process_class_id) {
        /* fld->class_id */
        soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            CLASS_ID_ACTION_SETf, intf_info->l3i_class);
    }

    if (process_itag && intf_info->l3i_inner_vlan) {

        if (!process_tag_action_profile) {
            return BCM_E_INTERNAL;
        }
        action_set = 0;
        /* IVIDf, IVID_VALIDf */
        soc_format_field32_set(unit, ITAG_ACTION_SETfmt,
                       &action_set, VIDf, intf_info->l3i_inner_vlan);

        soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            ITAG_ACTION_SETf, action_set);
    }

    /* QoS */
    if (process_otag) {
        BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_flex_intf_opri_ocfi_set(unit, view_id, l3_if_entry_p,
                                             intf_info));
        if (intf_info->l3i_vid) {  /* XXX need to check not VFI */
            action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                            OTAG_ACTION_SETf);
            soc_format_field32_set(unit, OTAG_ACTION_SETfmt,
                       &action_set, VIDf, intf_info->l3i_vid);
            soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                                OTAG_ACTION_SETf, action_set);
        }
    }
    if (process_itag) {
        BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_flex_intf_ipri_icfi_set(unit, view_id, l3_if_entry_p,
                                                 intf_info));
    }
    if (process_dscp) {
        BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_flex_intf_dscp_set(unit, view_id, l3_if_entry_p,
                                            intf_info));
    }

    bcm_vlan_action_set_t_init(&action);

    /* Program Inner VLAN handling actions */
    if (process_tag_action_profile) {
        if (intf_info->l3i_intf_flags & BCM_L3_INTF_INNER_VLAN_DO_NOT_MODIFY) {
            /* IVID_PRESENT_ACTIONf */
            action.dt_inner = bcmVlanActionNone;
            /* IVID_ABSENT_ACTIONf */
            action.ot_inner = bcmVlanActionNone;
        } else {
            if (intf_info->l3i_intf_flags & BCM_L3_INTF_INNER_VLAN_ADD) {
                if (intf_info->l3i_intf_flags &
                    BCM_L3_INTF_INNER_VLAN_REPLACE) {

                    /*IVID_PRESENT_ACTIONf*/
                    action.dt_inner = bcmVlanActionReplace;

                } else if (intf_info->l3i_intf_flags &

                    BCM_L3_INTF_INNER_VLAN_DELETE) {

                    /* IVID_PRESENT_ACTIONf */
                    action.dt_inner = bcmVlanActionDelete;
                } else {
                    action.dt_inner = bcmVlanActionNone;
                }
                /* IVID_ABSENT_ACTIONf */
                action.ot_inner = bcmVlanActionAdd;
            } else {
                if (intf_info->l3i_intf_flags & BCM_L3_INTF_INNER_VLAN_REPLACE) {

                    action.dt_inner = bcmVlanActionReplace;
                    action.ot_inner = bcmVlanActionNone;
                } else {
                    if (intf_info->l3i_intf_flags & BCM_L3_INTF_INNER_VLAN_DELETE) {

                        action.dt_inner = bcmVlanActionDelete;
                        action.ot_inner = bcmVlanActionNone;
                    }
                }
            }
        }
        if (intf_info->l3i_vid) {  /* XXX need to check not VFI */
            action.ut_outer = bcmVlanActionAdd;
            action.ut_outer_pkt_prio = bcmVlanActionAdd;
            action.ut_outer_cfi = bcmVlanActionAdd;
        }

        BCM_IF_ERROR_RETURN
         (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &profile_idx));

        action_set = 0;
        soc_format_field32_set(unit, TAG_ACTION_PROFILE_ACTION_SETfmt,
                       &action_set, TAG_ACTION_PROFILE_PTRf, profile_idx);

        soc_mem_field32_set(unit, view_id, l3_if_entry_p,
                            TAG_ACTION_PROFILE_ACTION_SETf, action_set);

    }

    return (BCM_E_NONE);

}

/*
 * Function:
 *      _bcm_td3_l3_intf_flex_opri_ocfi_get
 * Purpose:
 *      Get L3 Interface OVLAN QoS parameters for flex views
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - entry buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_intf_flex_opri_ocfi_get(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    uint32 action_set;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        OTAG_ACTION_SETf);

    if (soc_format_field32_get(unit, OTAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x0) { /* OPRI_OCFI_SELf */
          intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY;
    } else if (soc_format_field32_get(unit, OTAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x1) { /* OPRI_OCFI_SELf */
          intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET;
          /* OPRIf */
          intf_info->vlan_qos.pri =
               soc_format_field32_get(unit, OTAG_ACTION_SETfmt, &action_set, PCPf);
          /* OCFIf */
          intf_info->vlan_qos.cfi =
               soc_format_field32_get(unit, OTAG_ACTION_SETfmt, &action_set, DEf);
    } else if (soc_format_field32_get(unit, OTAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x2) { /* OPRI_OCFI_SELf */
          intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK;
          BCM_IF_ERROR_RETURN
               (_bcm_tr2_qos_idx2id(unit,
                    soc_format_field32_get(unit, OTAG_ACTION_SETfmt,
                            &action_set, PCP_DE_MAPPING_PTRf), 0x2,
                    &intf_info->vlan_qos.qos_map_id));
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_intf_flex_ipri_icfi_get
 * Purpose:
 *      Get L3 Interface IVLAN QoS parameters for flex views
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - entry buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_intf_flex_ipri_icfi_get(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    uint32 action_set;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        ITAG_ACTION_SETf);

    if (soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x0) { /* IPRI_ICFI_SELf */
          intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY;
    } else if (soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x1) { /* IPRI_ICFI_SELf */
          intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET;
          /* OPRIf */
          intf_info->inner_vlan_qos.pri =
               soc_format_field32_get(unit, ITAG_ACTION_SETfmt, &action_set, PCPf);
          /* OCFIf */
          intf_info->inner_vlan_qos.cfi =
               soc_format_field32_get(unit, ITAG_ACTION_SETfmt, &action_set, DEf);
    } else if (soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                   &action_set, PCP_DE_SELf) ==  0x2) { /* IPRI_ICFI_SELf */
          intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK;
          BCM_IF_ERROR_RETURN
               (_bcm_tr2_qos_idx2id(unit,
                    soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                            &action_set, PCP_DE_MAPPING_PTRf), 0x2,
                    &intf_info->inner_vlan_qos.qos_map_id));
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_intf_flex_dscp_get
 * Purpose:
 *      Get L3 Interface DSCP parameters for flex views
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      view_id   - (IN) memory view
 *      l3_if_entry_p - entry buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
_bcm_td3_l3_intf_flex_dscp_get(int unit, uint32 view_id, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    uint32 action_set;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        QOS_A_ACTION_SETf);

    if (soc_format_field32_get(unit, QOS_A_ACTION_SETfmt,
                  &action_set, PCP_DE_SELf) ==  0x0) { /* DSCP_SELf */
         intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_COPY;
    } else if (soc_format_field32_get(unit, QOS_A_ACTION_SETfmt,
                  &action_set, PCP_DE_SELf) ==  0x1) { /* DSCP_SELf */
         intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_SET;
          /* DSCPf */
         intf_info->dscp_qos.dscp =
              soc_format_field32_get(unit, QOS_A_ACTION_SETfmt, &action_set,
                                   QOS_VALUE_OR_MAPPING_PTR_OR_ZONE_IDf);
    } else if (soc_format_field32_get(unit, QOS_A_ACTION_SETfmt,
                  &action_set, PCP_DE_SELf) ==  0x2) { /* DSCP_SELf */
         intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_REMARK;
         BCM_IF_ERROR_RETURN
              (_bcm_tr2_qos_idx2id(unit,
                   soc_format_field32_get(unit, QOS_A_ACTION_SETfmt, &action_set,
                           QOS_VALUE_OR_MAPPING_PTR_OR_ZONE_IDf), 0x4,
                   &intf_info->dscp_qos.qos_map_id));
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_intf_get
 * Purpose:
 *      Get an flex view entry from L3 interface table.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN/OUT)Pointer to memory for interface information.
 *      l3_if_entry - Buffer to read interface info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_l3_flex_intf_get(int unit, _bcm_l3_intf_cfg_t *intf_info, uint32 *l3_if_entry_p)
{
    int rv = BCM_E_NONE;
    int i;
    uint32  view_id;
    uint32  flex_data_array[128];
    uint32  flex_data_count;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;
    uint8  process_fwd_domain = 0;
    uint8  process_otag = 0;
    uint8  process_itag = 0;
    uint8  process_dscp = 0;
    uint8  process_tag_action_profile = 0;
    uint8  process_tunnel_ptr = 0;
    uint8  process_class_id = 0;
    uint8  process_outer_sa = 0;
    /*uint8  process_inner_sa = 0;*/
    uint8 process_vxlt = 0;
    uint8  process_ttl_b = 0;
    uint8  process_svp = 0;
    uint8  data_type;
    soc_mem_t mem;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (NULL == l3_if_entry_p) {
        return (BCM_E_PARAM);
    }

    mem = BCM_XGS3_L3_MEM(unit,  intf);

    /* Get view id corresponding to the specified flow. */
    if (intf_info->flow_handle != 0) {
        BCM_IF_ERROR_RETURN(
            soc_flow_db_ffo_to_mem_view_id_get(unit,
                            intf_info->flow_handle,
                            intf_info->flow_option_handle,
                            SOC_FLOW_DB_FUNC_EGRESS_INTF_ID,
                            &view_id));
     }
    else {
        data_type = soc_mem_field32_get(unit, mem, l3_if_entry_p, DATA_TYPEf);

        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id));

        /* Get the flow handle, function id, option id */
        BCM_IF_ERROR_RETURN(
              soc_flow_db_mem_view_to_ffo_get(unit,
                                         view_id,
                                         SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                         ffo,
                                         &num_ffo));

        intf_info->flow_handle = ffo[0].flow_handle;
        intf_info->flow_option_handle = ffo[0].option_id;
    }

    /* Get logical field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = intf_info->logical_fields;

    for (i=0; i<opaque_count; i++) {
        user_fields[i].id = opaque_array[i];
        user_fields[i].value = soc_mem_field32_get(unit, view_id, l3_if_entry_p, opaque_array[i]);
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case FWD_DOMAIN_ACTION_SETf:
                process_fwd_domain = 1;
                break;
            case OTAG_ACTION_SETf:
                process_otag = 1;
                break;
            case ITAG_ACTION_SETf:
                process_itag = 1;
                break;
            case QOS_A_ACTION_SETf:
                process_dscp = 1;
                break;
            case TAG_ACTION_PROFILE_ACTION_SETf:
                process_tag_action_profile = 1;
                break;
            case TUNNEL_PTR_ACTION_SETf:
                process_tunnel_ptr = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case VXLT_CLASS_IDf:
                process_vxlt = 1;
                break;
            case DIRECT_ASSIGNMENT_7_ACTION_SETf:
                process_outer_sa = 1;
                break;
/*
            case DIRECT_ASSIGNMENT_5_ACTION_SETf:
                process_inner_sa = 1;
                break;
*/
            case TTL_B_ACTION_SETf:
                process_ttl_b = 1;
                break;
            case SVP_ACTION_SETf:
                process_svp = 1;
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    if (process_vxlt) {
        intf_info->l3i_class =
          soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                              VXLT_CLASS_IDf);
    }

    /* Get mac address. */
    if (process_outer_sa) {
       soc_mem_mac_addr_get(unit, view_id, l3_if_entry_p,
               DIRECT_ASSIGNMENT_7_ACTION_SETf, intf_info->l3i_mac_addr);
    }

    if (soc_feature(unit, soc_feature_riot)) {

        if (process_svp) {
            intf_info->l3i_source_vp =
                soc_mem_field32_get(unit, view_id, l3_if_entry_p, SVP_ACTION_SETf);
        }

        if (process_fwd_domain) {
            action_set = 0;

            action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                            FWD_DOMAIN_ACTION_SETf);

            /* VLAN_VFI_OVERLAY_TYPEf */
            if (soc_format_field32_get(unit, FWD_DOMAIN_ACTION_SETfmt,
                &action_set, FORWARDING_DOMAIN_TYPEf) == 0x2) {

            /* VFIf */
            _BCM_VPN_SET(intf_info->l3i_vid, _BCM_VPN_TYPE_VFI,
                soc_format_field32_get(unit, FWD_DOMAIN_ACTION_SETfmt,
                &action_set, FORWARDING_DOMAINf));
            } else {

            /* fld->vlan_id */
            intf_info->l3i_vid =
                soc_format_field32_get(unit, FWD_DOMAIN_ACTION_SETfmt,
                &action_set, FORWARDING_DOMAINf);
            }

        }

    } else
    {
        /* Get vlan id. */
        if (process_fwd_domain) {
            action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                            FWD_DOMAIN_ACTION_SETf);
            /* fld->vlan_id */
            intf_info->l3i_vid =
                soc_format_field32_get(unit, FWD_DOMAIN_ACTION_SETfmt,
                &action_set, FORWARDING_DOMAINf);
        }
    }
    /* Get Time To Live. */
    if (process_ttl_b) {
        action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        TTL_B_ACTION_SETf);
        /* fld->ttl */
        intf_info->l3i_ttl =
                soc_format_field32_get(unit, TTL_B_ACTION_SETfmt,
                &action_set, TTL_B_ACTION_SETf);
    }

    /* Get tunnel index. */
    if (process_tunnel_ptr) {
        action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        TUNNEL_PTR_ACTION_SETf);
        /* fld->tnl_id */
        intf_info->l3i_tunnel_idx =
                soc_format_field32_get(unit, TUNNEL_PTR_ACTION_SETfmt,
                &action_set, TUNNEL_INDEXf);
    }

    /* Set L2 bridging only flag. */
    /* not available in flex views */
    /*
    if (soc_mem_field32_get(unit, mem, l3_if_entry_p, fld->l2_switch)) {
        intf_info->l3i_flags |= BCM_L3_L2ONLY;
    }
    */

    /* QoS */
    if (process_otag) {
         BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_intf_flex_opri_ocfi_get(unit, view_id,
                                                 l3_if_entry_p, intf_info));
    }
    if (process_itag) {
         BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_intf_flex_ipri_icfi_get(unit, view_id,
                                                l3_if_entry_p, intf_info));
    }
    if (process_dscp) {
         BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_intf_flex_dscp_get(unit, view_id,
                                            l3_if_entry_p, intf_info));
    }

    if (process_itag) {
        action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                        ITAG_ACTION_SETf);

        /* IVIDf, IVID_VALIDf */
        intf_info->l3i_inner_vlan =
                soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                &action_set, VIDf);
    }

    /*Program Inner VLAN handling actions */
    if (process_tag_action_profile) {
        uint32 profile_idx;
        bcm_vlan_action_set_t action;

        action_set = soc_mem_field32_get(unit, view_id, l3_if_entry_p,
                                              TAG_ACTION_PROFILE_ACTION_SETf);

        profile_idx = soc_format_field32_get(unit, TAG_ACTION_PROFILE_ACTION_SETfmt,
                &action_set, TAG_ACTION_PROFILE_PTRf);

        _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

        /* IVID_PRESENT_ACTIONf */
        switch (action.dt_inner) {
            case bcmVlanActionNone:
                intf_info->l3i_intf_flags |= BCM_L3_INTF_INNER_VLAN_DO_NOT_MODIFY;
                break;
            case bcmVlanActionReplace:
                intf_info->l3i_intf_flags |= BCM_L3_INTF_INNER_VLAN_REPLACE;
                break;
            case bcmVlanActionDelete:
                intf_info->l3i_intf_flags |= BCM_L3_INTF_INNER_VLAN_DELETE;
                break;
            default :
                intf_info->l3i_intf_flags |= BCM_L3_INTF_INNER_VLAN_DO_NOT_MODIFY;
        }

        /*  IVID_ABSENT_ACTIONf */
        if (action.ot_inner == bcmVlanActionAdd) {
            intf_info->l3i_intf_flags |= BCM_L3_INTF_INNER_VLAN_ADD;
            intf_info->l3i_intf_flags &= ~(BCM_L3_INTF_INNER_VLAN_DO_NOT_MODIFY);
        }
        /* IVIDf */
        intf_info->l3i_inner_vlan =
                soc_format_field32_get(unit, ITAG_ACTION_SETfmt,
                &action_set, VIDf);
    }

    if (process_class_id) {
        /* fld->class_id */
        intf_info->l3i_class =
        soc_mem_field32_get(unit, view_id, l3_if_entry_p, CLASS_ID_ACTION_SETf);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_td3_l3_flex_ent_init
 * Purpose:
 *      TD2 helper routine used to init l3 host entry buffer
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry  lookup key & search result.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
STATIC int
_bcm_td3_l3_flex_ent_init(int unit, soc_mem_t view_id,
                      _bcm_l3_cfg_t *l3cfg, uint32 *buf_p)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;
    int rv;
    int i;
    int j;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get logical KEY field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l3cfg->l3c_logical_fields;

    for (i=0; i<opaque_count; i++) {
        for (j = 0; j < l3cfg->l3c_num_of_fields; j++) {
            if (user_fields[j].id == opaque_array[i]) {
                soc_mem_field32_set(unit, view_id, buf_p, user_fields[j].id,
                                user_fields[j].value);
                break;
            }
        }
    }

    soc_mem_field32_set(unit, view_id, buf_p, VRFf,
                        l3cfg->l3c_vrf);

    if (ipv6) { /* ipv6 entry */
        if (SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_LWR_64f)) {
            soc_mem_ip6_addr_set(unit, view_id, buf_p, IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
        }
        if (SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDR_UPR_64f)) {
            soc_mem_ip6_addr_set(unit, view_id, buf_p, IP_ADDR_UPR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
        }
    } else { /* ipv4 entry */
        if (SOC_MEM_FIELD_VALID(unit, view_id, IP_ADDRf)) {
            soc_mem_field32_set(unit, view_id, buf_p, IP_ADDRf,
                                           l3cfg->l3c_ip_addr);
        } else if (SOC_MEM_FIELD_VALID(unit, view_id, IPV4__DIPf)) {
            soc_mem_field32_set(unit, view_id, buf_p, IPV4__DIPf,
                                           l3cfg->l3c_ip_addr);
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_flex_get_extended_profile_index
 * Purpose:
 *      TD3 helper routine used to get profile index for EGR_MACDA_OUI_PROFILE
 *      and EGR_VNTAG_ETAG_PROFILE
 * Parameters:
 *      unit                      - (IN) SOC unit number.
 *      mem                       - (IN) L3 Extended table memory.
 *      l3cfg                     - (IN) L3 entry info.
 *      macda_oui_profile_index   - (out) EGR_MACDA_OUI profile index.
 *      vntag_etag_profile_index  - (out) EGR_VNTAG_ETAG profile index.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t _bcm_td3_flex_get_extended_profile_index(int unit, soc_mem_t view_id,
                                               _bcm_l3_cfg_t *l3cfg,
                                               int *macda_oui_profile_index,
                                               int *vntag_etag_profile_index,
                                               int *ref_count) {

    int rv = BCM_E_NONE;               /* Operation return status. */
    uint32 buf_key[SOC_MAX_MEM_WORDS];      /* Key buffer ptr */
    uint32 buf_entry[SOC_MAX_MEM_WORDS];    /* Entry buffer ptr */
    uint32 action_set;
#if 0
    l3_entry_ipv4_unicast_entry_t l3v4_key, l3v4_entry;            /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_key, l3v4_ext_entry; /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_key, l3v6_entry;           /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_key, l3v6_ext_entry; /* IPv6-Embedded */
#endif

    if (!SOC_MEM_FIELD_VALID(unit, view_id, EXTENDED_VIEW_MACDA_ACTION_SETf)) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(buf_key, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, buf_key);

    BCM_IF_ERROR_RETURN(rv);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN
    (_bcm_td3_l3_flex_ent_init(unit, view_id, l3cfg, buf_key));

    /* Perform lookup hw. */
    rv = soc_mem_generic_lookup(unit, view_id, MEM_BLOCK_ANY,
                                _BCM_TD3_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {

       action_set = soc_mem_field32_get(unit, view_id, buf_entry,
                               EXTENDED_VIEW_MACDA_ACTION_SETf);

       *macda_oui_profile_index = soc_format_field32_get(unit,
                                EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                                &action_set, MACDA_OUI_PROFILEf);

       *vntag_etag_profile_index = soc_format_field32_get(unit,
                                EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                                &action_set, VNTAG_ETAG_PROFILE_IDf);

        rv = soc_profile_mem_ref_count_get(unit,
                                           _bcm_td3_macda_oui_profile[unit],
                                           *macda_oui_profile_index,
                                           ref_count);
    }

    return rv;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_vp_entry_add
 * Purpose:
 *      TD3 helper routine used to parse hw flex l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      buf_p     - (IN) hw buffer.
 * Returns:
 *      BCM_X_XXX
 */
bcm_error_t
_bcm_td3_l3_flex_vp_entry_add(int unit, soc_mem_t view_id, _bcm_l3_cfg_t *l3cfg,
                        uint32 *buf_p, int *macda_oui_profile_index,
                        int *vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;
    int rv1 = BCM_E_NONE;
    uint32 *bufp;                 /* HW buffer address.          */
    uint32 profile_idx;
    uint32 mac_field;
    uint32 vntag_actions;
    void *entries[1];
    uint32  action_set = 0;

    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;

    /* Set table fields */
    bufp = buf_p;

    if (soc_feature(unit, soc_feature_virtual_port_routing) &&
        (l3cfg->l3c_encap_id > 0 &&
         l3cfg->l3c_encap_id < BCM_XGS3_EGRESS_IDX_MIN)) {
        int virtual_interface_id;
        uint32 hg_vntag_modify = 1;
        if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                             _bcmVpTypeNiv)) { /* NIV */
            bcm_niv_port_t niv_port;
            bcm_niv_egress_t niv_egress;
            int count;
            BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id,
                                      l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
            if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
                BCM_IF_ERROR_RETURN(bcm_esw_niv_egress_get(unit,
                                    niv_port.niv_port_id, 1,
                                    &niv_egress, &count));
                if (count == 0) {
                    return BCM_E_CONFIG;
                }
                if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_egress.virtual_interface_id;
            } else {
                if (niv_port.flags & BCM_NIV_PORT_MULTICAST) {
                    return BCM_E_PARAM;
                }
                virtual_interface_id = niv_port.virtual_interface_id;
            }
            /* fld->dst_vif */
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, DST_VIFf,
                           virtual_interface_id);

            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 1;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }
            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_td3_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, VNTAG_ETAG_PROFILE_IDf,
                           profile_idx);
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_OUI_PROFILE_VALIDf, 1);
        } else if (_bcm_vp_used_get(unit, l3cfg->l3c_encap_id,
                                    _bcmVpTypeExtender)) { /* PE */
            bcm_extender_port_t ep;
            int etag_dot1p_mapping_ptr = 0;
            BCM_GPORT_EXTENDER_PORT_ID_SET(ep.extender_port_id,
                                           l3cfg->l3c_encap_id);
            BCM_IF_ERROR_RETURN(bcm_esw_extender_port_get(unit, &ep));
            if (ep.flags & BCM_EXTENDER_PORT_MULTICAST) {
                return BCM_E_PARAM;
            }
            sal_memset(&vntag_etag_profile_entry, 0, sizeof(vntag_etag_profile_entry));
            vntag_actions = 2;
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  VNTAG_ACTIONSf, &vntag_actions);
            if (l3cfg->l3c_flags2 & BCM_L3_FLAGS2_NIV_ENCAP_LOCAL) {
                soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                      HG_VNTAG_MODIFY_ENABLEf, &hg_vntag_modify);
            }
            bcm_td2_qos_egr_etag_id2profile(unit, ep.qos_map_id,
                                            &etag_dot1p_mapping_ptr);
            soc_EGR_VNTAG_ETAG_PROFILEm_field_set(unit, &vntag_etag_profile_entry,
                                                  ETAG_DOT1P_MAPPING_PTRf,
                                                  (uint32 *)&etag_dot1p_mapping_ptr);

            entries[0] = &vntag_etag_profile_entry;
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, _bcm_td3_vntag_etag_profile[unit],
                                                    entries, 1, &profile_idx));
            *vntag_etag_profile_index = profile_idx;
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, VNTAG_ETAG_PROFILE_IDf,
                           profile_idx);
            virtual_interface_id = ep.extended_port_vid;
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, DST_VIFf,
                           virtual_interface_id);
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_OUI_PROFILE_VALIDf, 1);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_OUI_PROFILE_VALIDf, 0);
    }
    mac_field = ((l3cfg->l3c_mac_addr[3] << 16) |
                (l3cfg->l3c_mac_addr[4] << 8)  |
                (l3cfg->l3c_mac_addr[5] << 0));
    soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_23_0f, mac_field);
    mac_field = ((l3cfg->l3c_mac_addr[0] << 16) |
                 (l3cfg->l3c_mac_addr[1] << 8)  |
                 (l3cfg->l3c_mac_addr[2] << 0));
    sal_memset(&macda_oui_profile_entry, 0, sizeof(macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_set(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    entries[0] = &macda_oui_profile_entry;
    rv = soc_profile_mem_add(unit, _bcm_td3_macda_oui_profile[unit],
                             entries, 1, &profile_idx);
    if (BCM_FAILURE(rv)) {
        /* delete previously added entry or decrement ref count
         * from EGR_VNTAG_ETAG_PROFILE table
         */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (*vntag_etag_profile_index != 0) {
            rv1 = soc_profile_mem_delete(unit, _bcm_td3_vntag_etag_profile[unit],
                                         *vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv1)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv1;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
        return rv;
    }

    *macda_oui_profile_index = profile_idx;
    soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_OUI_PROFILEf, profile_idx);

    soc_mem_field32_set(unit, view_id, bufp,
                        EXTENDED_VIEW_MACDA_ACTION_SETf,
                        action_set);
    return rv;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_vp_entry_del
 * Purpose:
 *      Delete entry from profile table
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      ipv6     - (IN) IPV4 or IPv6 flag
 *      l3cfg    - (IN) L3 entry info.
 *      bufentry - (IN) hw buffer
 * Returns:
 */
bcm_error_t
_bcm_td3_l3_flex_vp_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                        int macda_oui_profile_index ,int vntag_etag_profile_index)
{

    int rv = BCM_E_NONE;

    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {

        /* delete an entry or decrement ref count from EGR_MACDA_OUI_PROFILE table */
        soc_mem_lock(unit, EGR_MACDA_OUI_PROFILEm);
        if (macda_oui_profile_index != -1) {
            rv = soc_profile_mem_delete(unit, _bcm_td3_macda_oui_profile[unit],
                                        macda_oui_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_MACDA_OUI_PROFILEm);

        /* delete an entry or decrement ref count from EGR_VNTAG_ETAG_PROFILE table */
        soc_mem_lock(unit, EGR_VNTAG_ETAG_PROFILEm);
        /* profile 0 is reserved for NON NIV/PE entries */
        if (vntag_etag_profile_index != 0) {
            rv = soc_profile_mem_delete(unit, _bcm_td3_vntag_etag_profile[unit],
                                        vntag_etag_profile_index);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);
            return rv;
        }
        soc_mem_unlock(unit, EGR_VNTAG_ETAG_PROFILEm);

    }

    return rv;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_vp_ent_parse
 * Purpose:
 *      TD3 helper routine used to parse hw flex l3 entry to api format
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN) L3 entry info.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 */
bcm_error_t
_bcm_td3_l3_flex_vp_ent_parse(int unit, soc_mem_t view_id, _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{

    uint32 profile_index;
    uint32 dst_vif;
    uint32 mac_field;
    bcm_gport_t vp_gport;
    uint32 vntag_actions;
    int rv = BCM_E_NONE;
    uint32 glp;                   /* Global port.                */
    uint32 *buf_p;                /* HW buffer address.          */
    egr_macda_oui_profile_entry_t macda_oui_profile_entry;
    egr_vntag_etag_profile_entry_t vntag_etag_profile_entry;
    int is_trunk = -1;
    uint32 HG_VNTAG_MODIFY_ENABLE_FLAG = 0;
    uint32 action_set;

    buf_p = (uint32 *)l3x_entry;

    action_set =  soc_mem_field32_get(unit, view_id, buf_p,
                            DESTINATION_ACTION_SETf);
    /* fld->glp */
    glp = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                               &action_set, DGPPf);

    _bcm_th_glp_resolve_embedded_nh(unit, glp, &l3cfg->l3c_modid,
                                    &l3cfg->l3c_port_tgid, &is_trunk);
    if (is_trunk == 1) {
        l3cfg->l3c_flags |= BCM_L3_TGID;
    }

    action_set = soc_mem_field32_get(unit, view_id, buf_p,
                        EXTENDED_VIEW_MACDA_ACTION_SETf);

    /* Set table fields */
    /* fld->macda_low */
    mac_field = soc_format_field32_get(unit,
                           EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                           &action_set, MACDA_23_0f);

    l3cfg->l3c_mac_addr[3] = (uint8) (mac_field >> 16 & 0xff);
    l3cfg->l3c_mac_addr[4] = (uint8) (mac_field >> 8 & 0xff);
    l3cfg->l3c_mac_addr[5] = (uint8) (mac_field & 0xff);
    /* Read upper 3 bytes of MAC address from profile table */
    /* fld->oui_profile_id */
    profile_index = soc_format_field32_get(unit,
                     EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                    &action_set, MACDA_OUI_PROFILEf);

    SOC_IF_ERROR_RETURN
        (READ_EGR_MACDA_OUI_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                     &macda_oui_profile_entry));
    soc_EGR_MACDA_OUI_PROFILEm_field_get(unit, &macda_oui_profile_entry,
                                         MACDA_OUIf, &mac_field);
    l3cfg->l3c_mac_addr[0] = (uint8) (mac_field >> 16 & 0xff);
    l3cfg->l3c_mac_addr[1] = (uint8) (mac_field >> 8 & 0xff);
    l3cfg->l3c_mac_addr[2] = (uint8) (mac_field & 0xff);

    /* fld->dst_vif */
    dst_vif = soc_format_field32_get(unit,
                     EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                    &action_set, DST_VIFf);
    /* fld->vntag_etag_profile_id */
    profile_index = soc_format_field32_get(unit,
                     EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                    &action_set, VNTAG_ETAG_PROFILE_IDf);

    if (profile_index != L3_EXT_VIEW_INVALID_VNTAG_ETAG_PROFILE) {
        SOC_IF_ERROR_RETURN
            (READ_EGR_VNTAG_ETAG_PROFILEm(unit, MEM_BLOCK_ANY, profile_index,
                                         &vntag_etag_profile_entry));
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              HG_VNTAG_MODIFY_ENABLEf,
                                              &HG_VNTAG_MODIFY_ENABLE_FLAG);
        l3cfg->l3c_flags2 = HG_VNTAG_MODIFY_ENABLE_FLAG;
        soc_EGR_VNTAG_ETAG_PROFILEm_field_get(unit, &vntag_etag_profile_entry,
                                              VNTAG_ACTIONSf, &vntag_actions);
        switch (vntag_actions) {
            case VNTAG_ACTIONS_NIV: /* NIV encap */
                if (_bcm_niv_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                       l3cfg->l3c_port_tgid, dst_vif,
                                       &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            case VNTAG_ACTIONS_ETAG: /* PE encap */
                if (_bcm_extender_gport_get(unit, is_trunk, l3cfg->l3c_modid,
                                            l3cfg->l3c_port_tgid, dst_vif,
                                            &vp_gport) == BCM_E_NONE) {
                    l3cfg->l3c_port_tgid = vp_gport;
                }
                break;
            default:
                break;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td3_l3_flex_entry_add
 * Purpose:
 *      Add and entry to flex view in TD3 L3 host table.
 * Parameters:
 *      unit   - (IN) SOC unit number.
 *      l3cfg  - (IN) L3 entry info.
 *      nh_idx - (IN) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_l3_flex_entry_add(int unit, _bcm_l3_cfg_t *l3cfg, int nh_idx)
{
    int ipv6;
    uint32 bufp[SOC_MAX_MEM_WORDS];   /* Hardware buffer ptr     */
    int rv = BCM_E_NONE;      /* Operation status.       */
    int embedded_nh = FALSE;
    uint32 glp = 0, port_id = 0, modid = 0;
    _bcm_l3_intf_cfg_t l3_intf;
#if 0
    l3_entry_ipv4_unicast_entry_t l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;   /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;   /* IPv6-Embedded */
#endif
    uint32 port_addr_max = 0, modid_max  = 0;
    int macda_oui_profile_index = -1;
    int macda_oui_profile_index1 = -1;
    int vntag_etag_profile_index = 0;
    int vntag_etag_profile_index1 = 0;
    int ref_count = 0;
#ifdef BCM_RIOT_SUPPORT
    int dvp = -1;
#endif
    soc_flow_db_mem_view_info_t mem_view_info;
    int     i;
    uint32  view_id;
    uint32  field_array[128];
    uint32  field_count;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;

    uint32 action_set = 0;
    uint32 mac_action_set[2];

    uint8 process_destination = 0;
    uint8 process_class_id = 0;
    uint8 process_qos = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_fwd_2_attributes = 0;
    uint8 process_qtag = 0;
    uint8 process_macda = 0;
    uint8 process_l3_if = 0;
    uint8 process_source = 0;

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l3cfg->l3c_flow_handle,
                          l3cfg->l3c_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_HOST_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_flow_db_mem_view_info_get(unit,
                          view_id,
                          &mem_view_info);

    BCM_IF_ERROR_RETURN(rv);

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

#if 0
    /* Assign entry buf based on table being used */
    BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                  l3v4_entry,
                                  l3v4_ext_entry,
                                  l3v6_entry,
                                  l3v6_ext_entry);
#endif

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, bufp);

    BCM_IF_ERROR_RETURN(rv);

    /* Prepare host entry for addition. */
    rv = _bcm_td3_l3_flex_ent_init(unit, view_id, l3cfg, bufp);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical PDD field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = l3cfg->l3c_logical_fields;

    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, bufp, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETf:
                process_fwd_2_attributes = 1;
                break;
            case QTAG_ACTION_SETf:
                process_qtag = 1;
                break;
            case EXTENDED_VIEW_MACDA_ACTION_SETf:
                process_macda = 1;
                break;
            case EXTENDED_VIEW_L3_OIF_ACTION_SETf:
                process_l3_if = 1;
                break;
            case SOURCE_ACTION_SETf:
                process_source = 1;
                break;
            default:
                break;
        }
    }

#ifdef BCM_RIOT_SUPPORT
    if ((BCMI_RIOT_IS_ENABLED(unit))
        && (_bcm_vp_is_vfi_type(unit, l3cfg->l3c_port_tgid))) {
        _bcm_vp_vfi_type_vp_get(unit, l3cfg->l3c_port_tgid, &dvp);
    } else
#endif
    {
        port_addr_max = SOC_PORT_ADDR_MAX(unit) + 1;
        modid_max = SOC_MODID_MAX(unit) + 1;
        if (l3cfg->l3c_flags & BCM_L3_TGID) {
            glp = (BCM_TGID_PORT_TRUNK_MASK(unit) & l3cfg->l3c_port_tgid);
            glp |= (port_addr_max * modid_max);
        } else {
            port_id = (SOC_PORT_ADDR_MAX(unit) & l3cfg->l3c_port_tgid);
            modid = (SOC_MODID_MAX(unit) & l3cfg->l3c_modid) * port_addr_max;
            glp = modid | port_id;
        }
    }

    /* Set hit bit. */
    if (l3cfg->l3c_flags & BCM_L3_HIT) {
        soc_mem_field32_set(unit, view_id, bufp, HITf, 1);
    }

    /*  Set priority. */
    action_set = 0;
    soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                           &action_set, PRIf, l3cfg->l3c_prio);
    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        if (!process_qos) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, QOS_ACTION_SETfmt,
                               &action_set, RPEf, 1);
    }

    if (process_qos) {
        soc_mem_field32_set(unit, view_id, bufp,
                        QOS_ACTION_SETf, action_set);
    }

    /* Set destination discard bit. */
    action_set = 0;
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        if (!process_fwd_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, DST_DISCARDf, 1);
    }

    if (process_fwd_attributes) {
        soc_mem_field32_set(unit, view_id, bufp,
                        FWD_ATTRIBUTES_ACTION_SETf, action_set);
    }

    /* Set local addr bit. */
    action_set = 0;
    if (l3cfg->l3c_flags & BCM_L3_HOST_LOCAL) {
        if (!process_fwd_2_attributes) {
            return BCM_E_PARAM;
        }
        soc_format_field32_set(unit, FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, LOCAL_ADDRESSf, 1);
    }

    if (process_fwd_2_attributes) {
        soc_mem_field32_set(unit, view_id, bufp,
                        FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETf, action_set);
    }

    /* Set classification group id. */
    if (process_class_id) {
        soc_mem_field32_set(unit, view_id, bufp, CLASS_ID_ACTION_SETf,
                            l3cfg->l3c_lookup_class);
    }

    if (!process_destination) {
        return BCM_E_INTERNAL;
    }

/* Check if embedded NH table is being used */
    embedded_nh = BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg->l3c_intf, nh_idx);

    if (l3cfg->l3c_flags & BCM_L3_IPMC) {
        if (process_source) {
            action_set = 0;

            if (dvp != -1) {
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                          &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_DVP );
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                          &action_set, DVPf, dvp);
            } else {
                if (l3cfg->l3c_flags & BCM_L3_TGID) {
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                             &action_set, DEST_TYPE2f, BCMI_TD3_DEST_TYPE2_LAG);
                } else {
                    soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                            &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_DGPP);
                }
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                          &action_set, DGPPf, glp);
            }
        soc_mem_field32_set(unit, view_id, bufp, SOURCE_ACTION_SETf,
                            action_set);
        }

        action_set = 0;
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_IPMC);
        soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, IPMC_GROUPf, l3cfg->l3c_ipmc_ptr);
        soc_mem_field32_set(unit, view_id, bufp,
                            DESTINATION_ACTION_SETf, action_set);
    } else if (embedded_nh) {
        if (!process_l3_if || !process_macda) {
            return BCM_E_PARAM;
        }
        sal_memset(&l3_intf, 0, sizeof(_bcm_l3_intf_cfg_t));
        BCM_XGS3_L3_INTF_IDX_SET(l3_intf, l3cfg->l3c_intf);

        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = BCM_XGS3_L3_HWCALL_EXEC(unit, if_get) (unit, &l3_intf);
        BCM_XGS3_L3_MODULE_UNLOCK(unit);

        soc_mem_field32_set(unit, view_id, bufp,
                            EXTENDED_VIEW_L3_OIF_ACTION_SETf,
                            l3cfg->l3c_intf);
        action_set = 0;
        if (dvp != -1) {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_DVP);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DVPf, dvp);
        } else {
            if (l3cfg->l3c_flags & BCM_L3_TGID) {
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE2f, BCMI_TD3_DEST_TYPE2_LAG);
            } else {
                soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_DGPP);
            }
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DGPPf, glp);
        }
        soc_mem_field32_set(unit, view_id, bufp,
                            DESTINATION_ACTION_SETf, action_set);

        mac_action_set[0] = 0;
        mac_action_set[1] = 0;
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            if (l3cfg->l3c_flags & BCM_L3_REPLACE) {
                /* For BCM_L3_REPLACE, get profile index for EGR_MACDA_OUI_PROFILE
                 * and EGR_VNTAG_ETAG_PROFILE table. Delete the profile entry if
                 * it's not used by any other host.
                 */
                rv = _bcm_td3_flex_get_extended_profile_index(unit, view_id, l3cfg,
                                                    &macda_oui_profile_index,
                                                    &vntag_etag_profile_index,
                                                    &ref_count);
                if (BCM_FAILURE(rv) && (rv!=BCM_E_NOT_FOUND)) {
                    return rv;
                }
                if (macda_oui_profile_index == -1) {
                    /* Host entry was done in regular L3_ENTRY table view */
                    BCM_IF_ERROR_RETURN
                    (_bcm_td3_l3_flex_vp_entry_add(unit, view_id, l3cfg, bufp,
                                             &macda_oui_profile_index1,
                                             &vntag_etag_profile_index1));
                } else {
                    if (ref_count > 1) {
                        rv = _bcm_td3_l3_flex_vp_entry_add(unit, view_id, l3cfg, bufp,
                                                     &macda_oui_profile_index1,
                                                     &vntag_etag_profile_index1);
                        if (BCM_SUCCESS(rv)) {
                            BCM_IF_ERROR_RETURN
                            (_bcm_td3_l3_flex_vp_entry_del(unit, l3cfg,
                                                    macda_oui_profile_index,
                                                    vntag_etag_profile_index));
                        } else {
                            return rv;
                        }
                    } else {
                        BCM_IF_ERROR_RETURN
                        (_bcm_td3_l3_flex_vp_entry_del(unit, l3cfg,
                                                macda_oui_profile_index,
                                                vntag_etag_profile_index));
                        BCM_IF_ERROR_RETURN
                        (_bcm_td3_l3_flex_vp_entry_add(unit, view_id, l3cfg, bufp,
                                                 &macda_oui_profile_index1,
                                                 &vntag_etag_profile_index1));

                    }
                }

            } else {
                BCM_IF_ERROR_RETURN
                (_bcm_td3_l3_flex_vp_entry_add(unit, view_id, l3cfg, bufp,
                                         &macda_oui_profile_index1,
                                         &vntag_etag_profile_index1));

            }

        } else
        {
            uint32              mac_field[2];
            /* fld->mac_addr */
            SAL_MAC_ADDR_TO_UINT32(l3cfg->l3c_mac_addr, mac_field);

            soc_format_field_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                                   mac_action_set,
                                   MAC_ADDRf, mac_field);
            soc_format_field32_set(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                                   mac_action_set,
                                   MACDA_OUI_PROFILE_VALIDf, 0);
        }
        soc_mem_field_set(unit, view_id, bufp,
                            EXTENDED_VIEW_MACDA_ACTION_SETf, mac_action_set);

        action_set = 0;
        if (process_qtag) {
            soc_format_field32_set(unit, QTAG_ACTION_SETfmt, &action_set,
                                   EH_TAG_TYPEf, l3cfg->l3c_eh_q_tag_type);
            soc_format_field32_set(unit, QTAG_ACTION_SETfmt, &action_set,
                                   EH_QUEUE_TAGf, l3cfg->l3c_eh_q_tag);
            soc_mem_field32_set(unit, view_id, bufp,
                                QTAG_ACTION_SETf, action_set);
        }

        /* Set copy_to_cpu bit. */
        if (l3cfg->l3c_flags & BCM_L3_COPY_TO_CPU) {
            if (!process_fwd_attributes) {
                 return BCM_E_PARAM;
            }
            soc_format_field32_set(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                               &action_set, COPY_TO_CPUf, 1);
            soc_mem_field32_set(unit, view_id, bufp,
                         FWD_ATTRIBUTES_ACTION_SETf, action_set);
        }
    } else {
        /* Set next hop index. */
        action_set = 0;
        if (l3cfg->l3c_flags & BCM_L3_MULTIPATH) {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE1f, BCMI_TD3_DEST_TYPE1_ECMP);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, ECMP_GROUPf, nh_idx);
        } else {
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f, BCMI_TD3_DEST_TYPE0_NH);
            soc_format_field32_set(unit, DESTINATION_FORMATfmt,
                               &action_set, NEXT_HOP_INDEXf, nh_idx);
        }
        soc_mem_field32_set(unit, view_id, bufp,
                            DESTINATION_ACTION_SETf, action_set);
    }

    /* Insert in table */
    rv = soc_mem_insert(unit, view_id, MEM_BLOCK_ANY, bufp);

    /* Handle host entry 'replace' actions */
    if ((BCM_E_EXISTS == rv) && (l3cfg->l3c_flags & BCM_L3_REPLACE)) {
        rv = BCM_E_NONE;
    }


    if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
        if (BCM_FAILURE(rv)) {
            /* if host add is not successful, then corresponding entries in the
             * profile table need to be removed for extended view hosts.
             */
            BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_flex_vp_entry_del(unit, l3cfg,
                                     macda_oui_profile_index1,
                                     vntag_etag_profile_index1));
        }
    }

    /* Write status check. */
    if ((rv >= 0) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)++ : BCM_XGS3_L3_IP4_CNT(unit)++;
    }

    return rv;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_ent_parse
 * Purpose:
 *      TD3 helper routine used to parse hw flex l3 entry to api format.
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      mem       - (IN) L3 table memory.
 *      l3cfg     - (IN/OUT) l3 entry key & parse result.
 *      nh_idx    - (IN/OUT) Next hop index.
 *      l3x_entry - (IN) hw buffer.
 * Returns:
 *      void
 */
int
_bcm_td3_l3_flex_ent_parse(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                        int *nh_idx, void *l3x_entry)
{
    int ipv6;                     /* Entry is IPv6 flag.         */
    uint32 hit = 0;               /* composite hit = hit_x|hit_y */
    uint32 *buf_p;                /* HW buffer address.          */
    uint32 glp;                   /* Global port. */
    int embedded_nh = FALSE;
    l3_entry_hit_only_x_entry_t hit_x;
    int idx, idx_max, idx_offset, hit_idx_shift;
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f};

    int     i;
    int     rv;
    uint32  view_id;
    uint32  field_array[128];
    uint32  field_count;
    uint32  action_set;
    uint8   data_type;

    uint8 process_destination = 0;
    uint8 process_class_id = 0;
    uint8 process_qos = 0;
    uint8 process_fwd_attributes = 0;
    uint8 process_fwd_2_attributes = 0;
    uint8 process_qtag = 0;
    uint8 process_macda = 0;
    uint8 process_l3_if = 0;

    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    buf_p = (uint32 *)l3x_entry;

    /* Reset entry flags */
    l3cfg->l3c_flags = (ipv6) ? BCM_L3_IP6 : 0;

    /* Read hit bits */
    idx_max = 1;
    idx_offset = (l3cfg->l3c_hw_index & 0x3);
    hit_idx_shift = 2;
    if (mem == L3_ENTRY_IPV4_MULTICASTm ||
        mem == L3_ENTRY_IPV6_UNICASTm ||
        (mem == L3_ENTRY_DOUBLEm)) {
        idx_max = 2;
        hit_idx_shift = 1;
        idx_offset = (l3cfg->l3c_hw_index & 0x1) << 1;
    } else if ((mem == L3_ENTRY_IPV6_MULTICASTm) ||
               (mem == L3_ENTRY_QUADm)) {
        idx_max = 4;
        hit_idx_shift = 0;
        idx_offset = 0;
    }

    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
        /* Get info from L3 and next hop table */
        BCM_IF_ERROR_RETURN(
            BCM_XGS3_MEM_READ(unit, L3_ENTRY_HIT_ONLYm,
              (l3cfg->l3c_hw_index >> hit_idx_shift), &hit_x));

        hit = 0;
        for (idx = idx_offset; idx < (idx_offset + idx_max); idx++) {
            hit |= soc_mem_field32_get(unit, L3_ENTRY_HIT_ONLYm,
                                       &hit_x, hitf[idx]);
        }
    }

    /* Get view id corresponding to the specified flow. */
    if (l3cfg->l3c_flow_handle != 0) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l3cfg->l3c_flow_handle,
                          l3cfg->l3c_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_HOST_ID,
                          &view_id);
    }
    else {
        data_type = soc_mem_field32_get(unit, mem, buf_p, DATA_TYPEf);

        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id);
    }

    BCM_IF_ERROR_RETURN(rv);

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            field_array,
                            &field_count);
    BCM_IF_ERROR_RETURN(rv);

    if (SOC_MEM_FIELD_VALID(unit, view_id, HITf)) {
        soc_mem_field32_set(unit, view_id, buf_p, HITf, hit);
    }

    for (i=0; i<field_count; i++) {
        switch (field_array[i]) {
            case DESTINATION_ACTION_SETf:
                process_destination = 1;
                break;
            case CLASS_ID_ACTION_SETf:
                process_class_id = 1;
                break;
            case QOS_ACTION_SETf:
                process_qos = 1;
                break;
            case FWD_ATTRIBUTES_ACTION_SETf:
                process_fwd_attributes = 1;
                break;
            case FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETf:
                process_fwd_2_attributes = 1;
                break;
            case QTAG_ACTION_SETf:
                process_qtag = 1;
                break;
            case EXTENDED_VIEW_MACDA_ACTION_SETf:
                process_macda = 1;
                break;
            case EXTENDED_VIEW_L3_OIF_ACTION_SETf:
                process_l3_if = 1;
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    if (!process_destination) {
       return BCM_E_INTERNAL;
    }

    embedded_nh = process_macda && process_l3_if;

    if (hit) {
        l3cfg->l3c_flags |= BCM_L3_HIT;
    }

    /* Get priority override flag, and priority. */
    if (process_qos) {
        /* fld->rpe */
        action_set = soc_mem_field32_get(unit, view_id, buf_p,
                     QOS_ACTION_SETf);
        if (soc_format_field32_get(unit, QOS_ACTION_SETfmt,
            &action_set, RPEf)) {
            l3cfg->l3c_flags |= BCM_L3_RPE;
        }
        /* fld->priority */
        l3cfg->l3c_prio = soc_format_field32_get(unit, QOS_ACTION_SETfmt,
            &action_set, PRIf);
    }

    /* Get destination discard flag. */
    if (process_fwd_attributes) {
        /* fld->dst_discard*/
        action_set = soc_mem_field32_get(unit, view_id, buf_p,
                     FWD_ATTRIBUTES_ACTION_SETf);
        if (soc_format_field32_get(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
            &action_set, DST_DISCARDf)) {
            l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
        }
    }

    /* Get local addr bit. */
    if (process_fwd_2_attributes) {
        /* fld->local_addr */
        action_set = soc_mem_field32_get(unit, view_id, buf_p,
                     FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETf);
        if (soc_format_field32_get(unit,
            FORWARDING_2_MISC_ATTRIBUTES_ACTION_SETfmt,
            &action_set, LOCAL_ADDRESSf)) {
            l3cfg->l3c_flags |= BCM_L3_HOST_LOCAL;
        }
    }

    /* Get classification group id. */
    if (process_class_id) {
        /* fld->class_id */
        l3cfg->l3c_lookup_class =
        soc_mem_field32_get(unit, view_id, buf_p,
                     CLASS_ID_ACTION_SETf);
    }

    /* Get virtual router id. */
    /* fld->vrf */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, view_id, buf_p, VRFf);

    if (embedded_nh) {
        uint32 port_addr_max = 0, modid_max  = 0;
#ifdef BCM_RIOT_SUPPORT
        int dvp = -1;
#endif

        l3cfg->l3c_flags |= BCM_L3_DEREFERENCED_NEXTHOP;
        if (nh_idx) {
            *nh_idx = BCM_XGS3_L3_INVALID_INDEX; /* Embedded NH */
        }
        /* fld->l3_intf */
        l3cfg->l3c_intf = soc_mem_field32_get(unit, view_id, buf_p,
                     EXTENDED_VIEW_L3_OIF_ACTION_SETf);
        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td3_l3_flex_vp_ent_parse(unit, view_id, l3cfg, buf_p));
        } else {
            uint32              mac_action_set[2];
            uint32              mac_field[2];
            uint32 dtype0;

            soc_mem_field_get(unit, view_id, buf_p,
                            EXTENDED_VIEW_MACDA_ACTION_SETf, mac_action_set);

            /* fld->mac_addr */
            soc_mem_field_get(unit, mem, buf_p, MAC_ADDRf, mac_field);

            soc_format_field_get(unit, EXTENDED_VIEW_MACDA_ACTION_SETfmt,
                                   mac_action_set,
                                   MAC_ADDRf, mac_field);

            SAL_MAC_ADDR_FROM_UINT32(l3cfg->l3c_mac_addr, mac_field);

            action_set = soc_mem_field32_get(unit, view_id, buf_p,
                            DESTINATION_ACTION_SETf);

            dtype0 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f);

            /* fld->dest */
            glp = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                               &action_set, DGPPf);

            port_addr_max = SOC_PORT_ADDR_MAX(unit) + 1;
            modid_max = SOC_MODID_MAX(unit) + 1;

#ifdef BCM_RIOT_SUPPORT
            if (BCMI_RIOT_IS_ENABLED(unit)) {
                    if (dtype0 == BCMI_TD3_DEST_TYPE0_DVP) {
                        dvp = glp; /*glp already holds the DEST value from earlier call */
                        _bcm_vp_encode_gport(unit, dvp, &(l3cfg->l3c_port_tgid));
                        l3cfg->l3c_modid = 0;
                    }
            }
            if (dvp == -1)
#endif
            {
                if (glp & (port_addr_max * modid_max)) {
                    l3cfg->l3c_flags |= BCM_L3_TGID;
                    l3cfg->l3c_port_tgid = glp & BCM_TGID_PORT_TRUNK_MASK(unit);
                    l3cfg->l3c_modid = 0;
                } else {
                    l3cfg->l3c_port_tgid = glp & SOC_PORT_ADDR_MAX(unit);
                    l3cfg->l3c_modid = (glp / port_addr_max) & SOC_MODID_MAX(unit);
                }
            }
        }
        if (process_qtag) {
            action_set = soc_mem_field32_get(unit, view_id, buf_p,
                     QTAG_ACTION_SETf);
            /* fld->eh_tag_type */
            l3cfg->l3c_eh_q_tag_type =
             soc_format_field32_get(unit, QTAG_ACTION_SETfmt,
             &action_set, EH_TAG_TYPEf);
            /* fld->eh_queue_tag */
            l3cfg->l3c_eh_q_tag =
             soc_format_field32_get(unit, QTAG_ACTION_SETfmt,
             &action_set, EH_QUEUE_TAGf);
        }
        /* Get copy_to_cpu bit. */
        /* fld->copy_to_cpu */
        if (process_fwd_attributes) {
            action_set = soc_mem_field32_get(unit, view_id, buf_p,
                     FWD_ATTRIBUTES_ACTION_SETf);
            if (soc_format_field32_get(unit, FWD_ATTRIBUTES_ACTION_SETfmt,
                 &action_set, COPY_TO_CPUf)) {
                l3cfg->l3c_flags |= BCM_L3_COPY_TO_CPU;
            }
        }
    } else {
       uint32 dtype0, dtype1;
        /* Get next hop info. */
        if (nh_idx) {

            action_set = soc_mem_field32_get(unit, view_id, buf_p,
                            DESTINATION_ACTION_SETf);

            dtype0 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                               &action_set, DEST_TYPE0f);

            /* fld->dest */
            *nh_idx = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                               &action_set, NEXT_HOP_INDEXf);

                if (dtype0 != BCMI_TD3_DEST_TYPE0_NH) {
                    /* Mark entry as ecmp */
                    l3cfg->l3c_ecmp = TRUE;
                    l3cfg->l3c_flags |= BCM_L3_MULTIPATH;
                }
        }
        dtype1 = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                           &action_set, DEST_TYPE1f);

        if (dtype1 == BCMI_TD3_DEST_TYPE1_ECMP) {
            /* Mark entry as ecmp */
            l3cfg->l3c_ecmp  = TRUE;
            l3cfg->l3c_flags |= BCM_L3_MULTIPATH;
        } else {
            l3cfg->l3c_ecmp  = FALSE;
            l3cfg->l3c_flags &= ~BCM_L3_MULTIPATH;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_l3_flex_entry_del
 * Purpose:
 *      Delete an flex entry from TD3 L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_l3_flex_entry_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    int ipv6;                     /* IPv6 entry indicator. */
    soc_mem_t view_id;            /* L3 table memory view. */
    soc_flow_db_mem_view_info_t mem_view_info;
    int rv = BCM_E_NONE;          /* Operation status.     */
    uint32 buf_entry[SOC_MAX_MEM_WORDS];    /* Entry buffer ptrs    */
#if 0
    l3_entry_ipv4_unicast_entry_t l3v4_entry;       /* IPv4 */
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;   /* IPv4-Embedded */
    l3_entry_ipv6_unicast_entry_t l3v6_entry;       /* IPv6 */
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;   /* IPv6-Embedded */
#endif
    int macda_oui_profile_index = -1;
    int vntag_etag_profile_index = 0;
    int ref_count = 0;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l3cfg->l3c_flow_handle,
                          l3cfg->l3c_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_HOST_ID,
                          (uint32 *) &view_id);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_flow_db_mem_view_info_get(unit,
                          view_id,
                          &mem_view_info);

    BCM_IF_ERROR_RETURN(rv);

    sal_memset(buf_entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, buf_entry);

    BCM_IF_ERROR_RETURN(rv);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_td3_l3_flex_ent_init(unit, view_id, l3cfg, buf_entry));

    rv = soc_mem_delete(unit, view_id, MEM_BLOCK_ANY, (void*) buf_entry);

    if (rv >= 0) {
        (ipv6) ?  BCM_XGS3_L3_IP6_CNT(unit)-- : BCM_XGS3_L3_IP4_CNT(unit)--;
    }

    if ((BCM_E_NOT_FOUND == rv) &&
        (soc_feature(unit, soc_feature_l3_extended_host_entry))) {
        /* Delete from  the extended tables only if the extended host entry
         * feature is supported and if the entry was not found in the
         * regular host entry */

        if (BCM_L3_BK_FLAG_GET(unit, BCM_L3_BK_ENABLE_MACDA_OUI_PROFILE)) {
            BCM_IF_ERROR_RETURN
            (_bcm_td3_flex_get_extended_profile_index(unit, view_id, l3cfg,
                                                &macda_oui_profile_index,
                                                &vntag_etag_profile_index,
                                                &ref_count));
       }

        BCM_IF_ERROR_RETURN
            (_bcm_td3_l3_flex_vp_entry_del(unit, l3cfg,
                                     macda_oui_profile_index,
                                     vntag_etag_profile_index));
    }

    return rv;
}
/*
 * Function:
 *      _bcm_td3_l3_clear_hit
 * Purpose:
 *      Clear hit bit on flex l3 entry
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      mem       - (IN)L3 table memory.
 *      l3cfg     - (IN)l3 entry info.
 *      l3x_entry - (IN)l3 entry filled hw buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_l3_clear_hit(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                   void *l3x_entry, int l3_entry_idx)
{
    int ipv6;                     /* Entry is IPv6 flag.      */
    int mcast;                    /* Entry is multicast flag. */
    uint32 *buf_p;                /* HW buffer address.       */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f };
    int idx;

    /* Input parameters check */
    if ((NULL == l3cfg) || (NULL == l3x_entry)) {
        return (BCM_E_PARAM);
    }

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    mcast = (l3cfg->l3c_flags & BCM_L3_IPMC);

    /* Init memory pointers. */
    buf_p = (uint32 *)l3x_entry;

    /* If entry was not hit  there is nothing to clear */
    if (!(l3cfg->l3c_flags & BCM_L3_HIT)) {
        return (BCM_E_NONE);
    }

    /* Reset hit bit. */
    soc_mem_field32_set(unit, mem, buf_p, HITf, 0);

    if (ipv6 && mcast) {
        /* IPV6 multicast entry hit reset. */
        for (idx = 0; idx < 4; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    } else if (ipv6 || mcast) {
        /* Reset IPV6 unicast and IPV4 multicast hit bits. */
        for (idx = 0; idx < 2; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    }

    /* Write entry back to hw. */
    return BCM_XGS3_MEM_WRITE(unit, mem, l3_entry_idx, buf_p);
}
/*
 * Function:
 *      _bcm_td3_l3_flex_entry_get
 * Purpose:
 *      Get an flex entry from TD3 L3 host table.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      l3cfg    - (IN/OUT) L3 entry  lookup key & search result.
 *      nh_index - (IN/OUT) Next hop index.
 *      embd     - (OUT) If TRUE, entry was found as embedded NH.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td3_l3_flex_entry_get(int unit, _bcm_l3_cfg_t *l3cfg, int *nh_idx, int *embd)
{
    int clear_hit;                              /* Clear hit bit.           */
    int rv = BCM_E_NONE;                        /* Operation return status. */
    uint32 buf_key[SOC_MAX_MEM_WORDS];          /* Key buffer ptr*/
    uint32 buf_entry[SOC_MAX_MEM_WORDS];        /* Entry buffer ptr*/
    uint32 view_id;
    soc_flow_db_mem_view_info_t mem_view_info;

    /* Indexed NH entry */
    *embd = _BCM_TD3_HOST_ENTRY_NOT_FOUND;

    /* Preserve clear_hit value. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Get view id corresponding to the specified flow. */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          l3cfg->l3c_flow_handle,
                          l3cfg->l3c_flow_option_handle,
                          SOC_FLOW_DB_FUNC_L3_HOST_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Get mem id corresponding to the view_id . */
    rv = soc_flow_db_mem_view_info_get(unit,
                          view_id,
                          &mem_view_info);

    BCM_IF_ERROR_RETURN(rv);

    sal_memset(buf_key, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, buf_key);

    BCM_IF_ERROR_RETURN(rv);

    /* Prepare lookup key. */
    BCM_IF_ERROR_RETURN(_bcm_td3_l3_flex_ent_init(unit, view_id, l3cfg, buf_key));

    /* Perform lookup */
    rv = soc_mem_generic_lookup(unit, view_id, MEM_BLOCK_ANY,
                                _BCM_TD3_L3_MEM_BANKS_ALL,
                                buf_key, buf_entry, &l3cfg->l3c_hw_index);
    if (BCM_SUCCESS(rv)) {
        /* Indexed NH entry */
        *embd = FALSE;
        /* Extract entry info. */
        BCM_IF_ERROR_RETURN(_bcm_td3_l3_flex_ent_parse(unit,
                                                  mem_view_info.mem, l3cfg,
                                                  nh_idx, buf_entry));
        /* Clear the HIT bit */
        if (clear_hit) {
            BCM_IF_ERROR_RETURN(_bcm_td3_l3_clear_hit(unit, view_id,
                                       l3cfg, buf_entry, l3cfg->l3c_hw_index));
        }
    }

    return rv;
}
/*
 * Function:
 *      _bcm_td3_mpls_get_vc_and_swap_flex_table_index
 * Purpose:
 *     Obtain Index into  VC_AND_SWAP_TABLE
 *     A software hash is not maintained, so duplicates may be created.
 * Parameters:
 *     IN  :  Unit
 *     IN  :  num_vc
 *     IN  :  mpls port entry
 *     IN  :  next hop entry
 *     IN  :  action
 *     OUT :  vc_swap_index
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td3_mpls_get_vc_and_swap_flex_table_index (int unit, int *vc_swap_index)
{
    int num_vc;
    int imin;
    int free_idx = 0, offset = 0;

    imin = soc_mem_index_min(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);

    /* we are dividng the table in half to manage counted and
     * non-counted entries.
     */
    num_vc /= 2;

    /* Try getting index from 2nd half of MPLS_VC_AND_SWAP table */
    for (free_idx = imin; free_idx < num_vc + imin; free_idx++) {
        if (!(_BCM_MPLS_VC_NON_COUNT_USED_GET(unit, free_idx) ||
              (soc_feature(unit, soc_feature_vc_and_swap_table_overlaid) &&
              _bcm_vp_used_get(unit, free_idx, _bcmVpTypeL2Gre)))) {
            break;
        }
    }

    if (free_idx == (num_vc + imin)) {

        /* Get index from 1st half of MPLS_VC_AND_SWAP table */
        for (free_idx = imin; free_idx < num_vc + imin; free_idx++) {
            if (!_BCM_MPLS_VC_COUNT_USED_GET(unit, free_idx - imin)) {
                break;
            }
        }
        if (free_idx == num_vc + imin) {
            return  BCM_E_RESOURCE;
        }
        _BCM_MPLS_VC_COUNT_USED_SET(unit, free_idx - imin);
        offset = imin;
    } else {
        _BCM_MPLS_VC_NON_COUNT_USED_SET(unit, free_idx - imin);
        offset = num_vc + imin;
    }

    *vc_swap_index = free_idx + offset;

    /* Increase refcount on table idx */
    _bcm_tr_mpls_vc_and_swap_ref_count_adjust(unit, *vc_swap_index , 1);

    return BCM_E_NONE;
}

/*
 * Function:
 *       _bcm_td3_mpls_vc_and_swap_flex_table_reset
 * Purpose:
 *       Reset  VC_AND_SWAP_TABLE entry
 * Parameters:
 *           IN :  Unit
 *           IN : vc_swap_index
 * Returns:
 *            BCM_E_XXX
 */
STATIC int
_bcm_td3_mpls_vc_and_swap_flex_table_index_reset (int unit, int vc_swap_index)
{
    egr_mpls_vc_and_swap_label_table_entry_t  vc_entry;
    int rv = BCM_E_NONE;
    _bcm_tr_mpls_bookkeeping_t *mpls_info = MPLS_INFO(unit);
    int num_vc;

    /* Entry 0 is reserved with _bcm_tr_mpls_default_entry_setup during init */
    if (vc_swap_index == VC_AND_SWAP_TABLE_RESV_INDEX) {
        LOG_INFO(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,"Not resetting reserved index: %d in "
                         "VC AND SWAP TABLE\n"), 0));
        return BCM_E_NONE;
    }
    if ( mpls_info->vc_swap_ref_count[vc_swap_index] != 0 ) {
         return BCM_E_NONE;
    }

    num_vc = soc_mem_index_count(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm);
    num_vc /= 2;

    if (vc_swap_index >= num_vc) {
        _BCM_MPLS_VC_NON_COUNT_USED_CLR(unit, vc_swap_index - num_vc);
    } else {
        _BCM_MPLS_VC_COUNT_USED_CLR(unit, vc_swap_index);
    }

    sal_memset(&vc_entry, 0,
              sizeof(egr_mpls_vc_and_swap_label_table_entry_t));

    rv = soc_mem_write(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
              MEM_BLOCK_ALL, vc_swap_index,
              &vc_entry);

    

    return rv;
}
/*
 * Function:
 *       _bcm_td3_mpls_vc_and_swap_table_flex_entry_set
 * Purpose:
 *       Set  VC_AND_SWAP_TABLE entry
 * Parameters:
 *       IN : Unit
 *       IN : egress object
 *       IN : View ID for memory
 *       IN : label action
 *       IN : vc_swap entry
 *       IN : hw_map_idx: qos map table
 *       IN : vc_swap_index
 * Returns:
 *            BCM_E_XXX
 */
STATIC int
_bcm_td3_mpls_vc_and_swap_table_flex_entry_set (int unit,
                                       bcm_l3_egress_t *egr_obj,
                                        soc_mem_t view_id,
                                       int label_action,
                                       egr_mpls_vc_and_swap_label_table_entry_t *vc_entry,
                                       int hw_map_idx, int vc_swap_index)
{
    int rv;
    bcm_mpls_label_t mpls_label = BCM_MPLS_LABEL_INVALID;
    uint8 mpls_ttl = 0;
    uint32 mpls_flags = 0;
    uint8 mpls_exp = 0;
    uint8 mpls_pkt_pri = 0;
    uint8 mpls_pkt_cfi = 0;

    uint32  flex_data_array[128];
    uint32  flex_data_count;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    bcm_flow_logical_field_t* user_fields;
    uint32  mpls_hdr;

    int i;
    uint32 action_set = 0;
    uint8 process_mpls_entry = 0;
    uint8 process_label = 0;

    if (egr_obj == NULL) {
        return BCM_E_PARAM;
    }

    mpls_label = egr_obj->mpls_label;
    mpls_ttl = egr_obj->mpls_ttl;
    mpls_flags = egr_obj->mpls_flags;
    mpls_pkt_pri = egr_obj->mpls_pkt_pri;
    mpls_pkt_cfi = egr_obj->mpls_pkt_cfi;
    mpls_exp = egr_obj->mpls_exp;

    if ((mpls_exp > 7) || (mpls_pkt_pri > 7) ||
        (mpls_pkt_cfi > 1)) {
        return BCM_E_PARAM;
    }

    sal_memset(vc_entry, 0,
               sizeof(egr_mpls_vc_and_swap_label_table_entry_t));

    /* Initialize control field list for this view id. */
    rv = soc_flow_db_mem_view_entry_init(unit, view_id, (uint32 *) vc_entry);

    BCM_IF_ERROR_RETURN(rv);

    /* Get logical field list for this view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                            _BCM_FLOW_LOGICAL_FIELD_MAX,
                            opaque_array,
                            &opaque_count);
    BCM_IF_ERROR_RETURN(rv);

    user_fields = egr_obj->logical_fields;

    for (i=0; i<opaque_count; i++) {
        if (user_fields[i].id == opaque_array[i]) {
            soc_mem_field32_set(unit, view_id, vc_entry, user_fields[i].id,
                                user_fields[i].value);
        }
    }

    /* Get PDD field list corresponding to the view id. */
    rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            128,
                            flex_data_array,
                            &flex_data_count);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i<flex_data_count; i++) {
        switch (flex_data_array[i]) {
            case MPLS_ENTRY_0_ACTION_SETf:
                process_mpls_entry = 1;
                break;
            case SD_TAG_ACTION_SETf:
                break;
            case DIRECT_ASSIGNMENT_21_ACTION_SETf:
                process_label = 1;
                break;
            default:
                /* Unrecognized action set */
                return BCM_E_INTERNAL;
        }
    }

    if (process_mpls_entry) {
        action_set = 0;
        if (_BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, mpls_flags)) {
                /* set label select to 1 to get label from metadata */
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, LABEL_SELECTf,
                                    1);
                /* CANCUN has fixed the container 1 for swap to self */
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, CONTAINER_IDf,
                                    1);
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, MPLS_PUSH_ACTIONf,
                                    _BCM_MPLS_ACTION_PUSH);
                MPLS_INFO(unit)->egr_vc_swap_lbl_action[vc_swap_index] =
                    _BCM_MPLS_ACTION_PRESERVED;
        } else {
            if (BCM_XGS3_L3_MPLS_LBL_VALID(mpls_label)) {
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, MPLS_LABELf,
                                    mpls_label);
                MPLS_INFO(unit)->egr_vc_swap_lbl_action[vc_swap_index] =
                            label_action;
                if (label_action == _BCM_MPLS_ACTION_SWAP) {
                    /*VC SWAP HW Tbl supports Label actions NOOP,PUSH in TD3.
                     *Record software defined label actions in software
                     *state to reuse existing code for L3MPLS*/
                    label_action =  _BCM_MPLS_ACTION_PUSH;
                }
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, MPLS_PUSH_ACTIONf,
                                    _BCM_MPLS_ACTION_PUSH);
            } else {
                soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, MPLS_PUSH_ACTIONf,
                                    _BCM_MPLS_ACTION_NOOP);
            }
        }

        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_TTLf,
                                mpls_ttl);
        } else {
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_TTLf,
                                0x0);
        }

        if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

            if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
                (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
                (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
                return  BCM_E_PARAM;
            }
            /* Use the specified EXP, PRI and CFI */
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_EXP_SELECTf,
                                0x0); /* USE_FIXED */
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_EXPf,
                                mpls_exp);
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, NEW_PRIf,
                                mpls_pkt_pri);
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, NEW_CFIf,
                                mpls_pkt_cfi);
        } else if (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
                return BCM_E_PARAM;
            }
            /* Use EXP-map for EXP, PRI and CFI */
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_EXP_SELECTf,
                                0x1); /* USE_MAPPING */
            soc_format_field32_set(unit, MPLS_ENTRY_ACTION_SETfmt,
                                &action_set, MPLS_EXP_MAPPING_PTRf,
                                hw_map_idx);
        } else if ((label_action != _BCM_MPLS_ACTION_SWAP) &&
                        (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ) {
            /* not support for MPLSoGRE flow */
            return BCM_E_PARAM;
        } else if ((label_action == _BCM_MPLS_ACTION_SWAP) &&
                        (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ) {
            /* not support for MPLSoGRE flow */
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, view_id, vc_entry,
                            MPLS_ENTRY_0_ACTION_SETf, action_set);
    }
    if (process_label) {
        mpls_hdr = mpls_label<<12 | mpls_exp<<9 | 1<<8 | mpls_ttl;
        soc_mem_field32_set(unit, view_id, vc_entry,
                            DIRECT_ASSIGNMENT_21_ACTION_SETf, mpls_hdr);
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *       _bcm_td3_mpls_gre_label_add
 * Purpose:
 *       Updates MPLSoGRE within VC_AND_SWAP_TABLE for a given EGR_NH Index
 * Parameters:
 *       IN :  Unit
 *       IN :  egr object
 *       IN :  nh_index, available when BCM_L3_REPLACE
 *       IN :  mpls_flags
 * Returns:
 *       BCM_E_XXX
 */
int
_bcm_td3_mpls_gre_label_add (int unit, bcm_l3_egress_t *egr, int nh_index, uint32 flags)
{
    int  rv=BCM_E_NONE;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    int      vc_swap_index=-1;
    uint32    view_id;
    egr_l3_next_hop_entry_t egr_nh;
    uint32    nh_entry_type;
    uint32    nh_view_id;
    int       hw_map_idx, num_mpls_map;
    uint32 action_set;

    if (egr == NULL) {
        return BCM_E_PARAM;
    }

    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                          egr->flow_handle,
                          egr->flow_label_option_handle,
                          SOC_FLOW_DB_FUNC_EGRESS_LABEL_ID,
                          &view_id);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&vc_entry, 0,
            sizeof(egr_mpls_vc_and_swap_label_table_entry_t));

    /* Retrieve EGR L3 NHOP Entry from given index */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &egr_nh));

    nh_entry_type =
        soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf);

    rv = soc_flow_db_mem_to_view_id_get(unit,
                         EGR_L3_NEXT_HOPm,
                         SOC_FLOW_DB_KEY_TYPE_INVALID,
                         nh_entry_type,
                         0,
                         NULL,
                         &nh_view_id);

    /* Be sure that the existing entry is already setup to
     * egress into an MPLS tunnel. If not, return BCM_E_PARAM.
     */

    if (!BCMI_L3_NH_FLEX_VIEW(nh_entry_type)) {
        return BCM_E_PARAM;
    }

    if ((egr->mpls_label != BCM_MPLS_LABEL_INVALID) &&
        (flags & BCM_L3_REPLACE)) {

        action_set = soc_mem_field32_get(unit, nh_view_id, &egr_nh,
                               NEXT_PTR_ACTION_SETf);

        if (soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf) !=
                               BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_VC_SWAP) {
            /* this entry doesn't have a vc_swap index */
            return BCM_E_PARAM;
        }
        /* Retrieve VC_AND_SWAP_INDEX */
        vc_swap_index = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                 &action_set, NEXT_PTRf);

        if (vc_swap_index == 0) {

            BCM_IF_ERROR_RETURN(
                _bcm_td3_mpls_get_vc_and_swap_flex_table_index(unit,
                    &vc_swap_index));
        } else {
            /* Retrieve VC_AND_SWAP_TABLE entry */
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, view_id,
                     MEM_BLOCK_ALL, vc_swap_index, &vc_entry));
        }
    } else if ((egr->mpls_label == BCM_MPLS_LABEL_INVALID) &&
              (flags & BCM_L3_REPLACE)) {
        rv = _bcm_td3_mpls_gre_label_delete (unit, nh_index, nh_view_id);
        return rv;
    } else {

        BCM_IF_ERROR_RETURN(
            _bcm_td3_mpls_get_vc_and_swap_flex_table_index(unit,
                &vc_swap_index));
    }
    /* Retrieve hardware index for mapping pointer */
    if ((egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {
        /* Mapping pointer not used */
        hw_map_idx = -1;
    } else {
        num_mpls_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
        rv = _egr_qos_id2hw_idx(unit,egr->mpls_qos_map_id,&hw_map_idx);
        if ((rv != BCM_E_NONE) || hw_map_idx < 0 || hw_map_idx >= num_mpls_map) {
            if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
                rv = BCM_E_PARAM;
                goto cleanup;
            } else {
                hw_map_idx = 0; /* use default */
            }
        }
    }

    /* Program the MPLS_VC_AND_SWAP table entry */
    rv = _bcm_td3_mpls_vc_and_swap_table_flex_entry_set(unit, egr, view_id,
                                             _BCM_MPLS_ACTION_PUSH, &vc_entry,
                                             hw_map_idx, vc_swap_index);
    if (rv < 0){
       goto cleanup;
    }

    rv = soc_mem_write(unit, view_id,
                   MEM_BLOCK_ALL, vc_swap_index,
                   &vc_entry);
    if (rv < 0){
       goto cleanup;
    }
/*
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                        &egr_nh, nh_entry_type_field,
                        entry_type);
*/

    action_set = 0;
    soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                         &action_set, NEXT_PTR_TYPEf,
                         BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_VC_SWAP);
    /* VC_AND_SWAP_INDEXf */
    soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                         &action_set, NEXT_PTRf,
                         vc_swap_index);
    soc_mem_field32_set(unit, nh_view_id, &egr_nh,
                   NEXT_PTR_ACTION_SETf, action_set);

    /* Enable HG.L3_bit
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
            &egr_nh, MPLS__HG_L3_OVERRIDEf, 0x1);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
             &egr_nh, MPLS__HG_MODIFY_ENABLEf, 0x1);
    */

    rv = soc_mem_write(unit, nh_view_id,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);

    if (rv < 0) {
       goto cleanup;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;

cleanup:
    if (vc_swap_index != -1) {
         _bcm_tr_mpls_vc_and_swap_ref_count_adjust (unit, vc_swap_index, -1);
        /* Free VC_AND_SWAP_LABEL entry */
        (void)_bcm_td3_mpls_vc_and_swap_flex_table_index_reset (unit, vc_swap_index);
    }
    return rv;
}
/*
 * Function:
 *		_bcm_td3_mpls_gre_label_get
 * Purpose:
 *		Retrieves the  Inner_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *           IN :  mpls_label
 *           IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_td3_mpls_gre_label_get (int unit, int vc_swap_index, bcm_l3_egress_t *egr)
{
    int rv=BCM_E_NONE;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    uint32 label_action;
    uint32 action_set;
    uint32  opaque_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  opaque_count;
    int8    data_type;
    uint32  view_id;
    bcm_flow_logical_field_t* user_fields;
    soc_flow_db_ffo_t ffo[SOC_FLOW_DB_MAX_VIEW_FFO_TPL];
    uint32 num_ffo = 0;
    uint32  mpls_hdr;
    int     i;

    if (vc_swap_index != -1) {
        /* Retrieve VC_AND_SWAP_TABLE entry */
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                         MEM_BLOCK_ALL, vc_swap_index, &vc_entry));

        data_type = soc_mem_field32_get(unit,
                         EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, &vc_entry, DATA_TYPEf);

        if (data_type == 0) {   /* fixed view */
            egr->mpls_label = BCM_MPLS_LABEL_INVALID;
            return rv;
        }

        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_to_view_id_get(unit,
                             EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id));

        BCM_IF_ERROR_RETURN(
             soc_flow_db_mem_view_to_ffo_get(unit,
                                         view_id,
                                         SOC_FLOW_DB_MAX_VIEW_FFO_TPL,
                                         ffo,
                                         &num_ffo));

        egr->flow_label_option_handle = ffo[0].option_id;

        /* Get logical field list for this view id. */
        BCM_IF_ERROR_RETURN(
            soc_flow_db_mem_view_field_list_get(unit,
                                 view_id,
                                 SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                                 _BCM_FLOW_LOGICAL_FIELD_MAX,
                                 opaque_array,
                                 &opaque_count));

         user_fields = egr->logical_fields;

         for (i=0; i<opaque_count; i++) {
             user_fields[i].id = opaque_array[i];
             user_fields[i].value = soc_mem_field32_get(unit, view_id, &vc_entry, user_fields[i].id);
         }

        if (SOC_MEM_FIELD_VALID(unit, view_id, MPLS_ENTRY_0_ACTION_SETf)) {
            egr->mpls_label = BCM_MPLS_LABEL_INVALID;

            action_set = soc_mem_field32_get(unit, view_id, &vc_entry,
                                   MPLS_ENTRY_0_ACTION_SETf);

            label_action = soc_format_field32_get(unit,
                                    MPLS_ENTRY_ACTION_SETfmt,
                                    &action_set, MPLS_PUSH_ACTIONf);

            if (label_action == _BCM_MPLS_ACTION_PUSH) { /* PUSH */
                egr->flags2 |= BCM_L3_MPLS_GRE_LABEL;
                egr->mpls_label =
                    soc_format_field32_get(unit, MPLS_ENTRY_ACTION_SETfmt,
                                        &action_set, MPLS_LABELf);
                egr->mpls_ttl =
                    soc_format_field32_get(unit, MPLS_ENTRY_ACTION_SETfmt,
                                        &action_set, MPLS_TTLf);
                if (egr->mpls_ttl) {
                    egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
                }

            } else {
                egr->mpls_label = BCM_MPLS_LABEL_INVALID;
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, view_id, DIRECT_ASSIGNMENT_21_ACTION_SETf)) {
            mpls_hdr = soc_mem_field32_get(unit, view_id, &vc_entry,
                            DIRECT_ASSIGNMENT_21_ACTION_SETf);
            egr->mpls_label = mpls_hdr & 0xfffff;
            egr->mpls_exp = (mpls_hdr >> 20) & 0xf;
            egr->mpls_ttl = (mpls_hdr >> 24) & 0xff;
        }
    } else {
        egr->mpls_label = BCM_MPLS_LABEL_INVALID;
    }
    egr->flags2 |= BCM_L3_MPLS_GRE_LABEL;

    return rv;
}
/*
 * Function:
 *		_bcm_td3_mpls_gre_label_delete
 * Purpose:
 *		Deletes  Inner_LABEL within VC_AND_SWAP_TABLE for a specific EGR_NH Index
 * Parameters:
 *		IN :  Unit
 *              IN :  nh_index
 *              IN :  view_id
 * Returns:
 *		BCM_E_XXX
 */
int
_bcm_td3_mpls_gre_label_delete (int unit, int index, soc_mem_t view_id)
{
    int rv=BCM_E_NONE;
    int  vc_swap_index=-1;
    egr_l3_next_hop_entry_t egr_nh;
    uint8  entry_type;
    /*int  macda_index=-1;*/
    uint32 action_set;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      index, &egr_nh));

    /* Retrieve  and Verify  ENTRY_TYPE */
    entry_type =
        soc_mem_field32_get(unit, view_id, &egr_nh, DATA_TYPEf);

    if (!BCMI_L3_NH_FLEX_VIEW(entry_type)) {
        return BCM_E_PARAM;
    }

    /* Don't change the Entry_Type to Normal since this is a flex view */

    /* Delete MAC_DA_PROFILE_INDEX
    if (soc_feature(unit, soc_feature_mpls_enhanced)) {
        macda_index =
              soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                         MPLS__MAC_DA_PROFILE_INDEXf);
    }

    if (macda_index != -1) {
        _bcm_mac_da_profile_entry_delete(unit, macda_index);
    }
   */

    /* Retrieve VC_AND_SWAP_INDEX */
    action_set = soc_mem_field32_get(unit, view_id, &egr_nh,
                           NEXT_PTR_ACTION_SETf);

    if (soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                           &action_set, NEXT_PTR_TYPEf) !=
                           BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_VC_SWAP) {
        /* this entry doesn't have a vc_swap index */
        return BCM_E_PARAM;
    }
    /* Retrieve VC_AND_SWAP_INDEX */
    vc_swap_index = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                             &action_set, NEXT_PTRf);

    /* Decrease refcount on table idx */
    _bcm_tr_mpls_vc_and_swap_ref_count_adjust (unit, vc_swap_index, -1);

    BCM_IF_ERROR_RETURN
         (_bcm_td3_mpls_vc_and_swap_flex_table_index_reset (unit, vc_swap_index));

    vc_swap_index = 0;
    soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                             &action_set, NEXT_PTRf,
                             vc_swap_index);

    soc_mem_field32_set(unit, view_id, &egr_nh,
                         NEXT_PTR_ACTION_SETf, action_set);

    rv = soc_mem_write(unit, view_id,
                       MEM_BLOCK_ALL, index, &egr_nh);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;
}

int
bcm_td3_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                        int cf_max, int *cf_count)
{
    _bcm_l3_cfg_t l3cfg;
    uint32 *bufp;      /* Hardware buffer ptr     */
    soc_mem_t mem;            /* L3 table memory type.   */
    int nh_idx, index, rv = BCM_E_NONE;      /* Operation status.       */
    _bcm_l3_fields_t *fld;    /* L3 table common fields. */
    l3_entry_ipv4_unicast_entry_t l3v4_entry;
    l3_entry_ipv4_multicast_entry_t l3v4_ext_entry;
    l3_entry_ipv6_unicast_entry_t l3v6_entry;
    l3_entry_ipv6_multicast_entry_t l3v6_ext_entry;
    uint8 i, bidx, num_ent;
    uint32 l3_entry[SOC_MAX_MEM_WORDS];
    uint32 ipmc, ipv6, embedded_nh;
    uint32 bucket;
    int num_banks;
    int hash_offset = 0, use_lsb = 0;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((NULL == ipkey) || (NULL == cf_count) ||
        (NULL == cf_array) || (cf_max <= 0)) {
        return (BCM_E_PARAM);
    }

    embedded_nh = ipkey->l3k_flags & BCM_L3_DEREFERENCED_NEXTHOP;
    nh_idx = (embedded_nh) ? BCM_XGS3_L3_INVALID_INDEX : 0;

    /* Translate lookup key to l3cfg format. */
    BCM_IF_ERROR_RETURN(_bcm_td2_ip_key_to_l3cfg(unit, ipkey, &l3cfg));

    /* Get entry type. */
    ipv6 = (l3cfg.l3c_flags & BCM_L3_IP6);
    ipmc = (l3cfg.l3c_flags & BCM_L3_IPMC);

    /* Get table memory. */
    BCM_TD3_L3_HOST_TABLE_MEM(unit, l3cfg.l3c_intf, ipv6, mem, nh_idx);
    BCM_TD3_L3_HOST_TABLE_FLD(unit, mem, ipv6, fld);
    BCM_TD3_L3_HOST_ENTRY_BUF(ipv6, mem, bufp,
                                    l3v4_entry,
                                    l3v4_ext_entry,
                                    l3v6_entry,
                                    l3v6_ext_entry);

    /* Prepare host entry for addition. */
    if (ipmc) {
        /*  Zero entry buffer. */
        sal_memcpy(bufp, soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit,mem) * 4);
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ipmc_ent_init(unit, bufp, &l3cfg));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td2_l3_ent_init(unit, mem, &l3cfg, bufp));
    }

    *cf_count = 0;

    /* Get entries of each banks */
    switch (mem) {
    case L3_ENTRY_SINGLEm:
        num_ent =  4;
        break;
    case L3_ENTRY_DOUBLEm:
        num_ent =  2;
        break;
    case L3_ENTRY_QUADm:
        num_ent =  1;
        break;

    /* coverity[dead_error_begin] */
    default:
        return BCM_E_PARAM;
    }

    /* Get number of L3 banks */
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, mem, &num_banks));

    /* bank 11 to 10 are dedicated L3 banks, 9 to 5 are dedicated, depends on
     * mode how many shared banks are assigned to l3
     */
    for (bidx = 11; bidx >= (11 - num_banks + 1); bidx--) {
        /* Calculate hash index */
        rv = soc_td3_hash_offset_get(unit, L3_ENTRY_ONLY_SINGLEm, bidx, &hash_offset,
                                    &use_lsb);

        if (BCM_FAILURE(rv)) {
            return BCM_E_INTERNAL;
        }

        /* coverity[overrun-buffer-val: FALSE] */
        bucket = soc_td3_l3x_entry_hash(unit, bidx, hash_offset, use_lsb, bufp);
        index = soc_td3_hash_index_get(unit, mem, bidx, bucket);

        /* Get conflicting IPs from all HW entries in current bank */
        for (i = 0; (i < num_ent) && (*cf_count < cf_max);) {
            rv = soc_mem_read(unit, mem, COPYNO_ALL, index + i, l3_entry);
            if (SOC_FAILURE(rv)) {
                return BCM_E_MEMORY;
            }
            if (soc_mem_field32_get(unit, mem, &l3_entry, fld->valid)) {
                switch(soc_mem_field32_get(unit, mem, &l3_entry, fld->key_type)) {
                case TD3_L3_HASH_DATA_TYPE_V4UC:
                      l3cfg.l3c_flags = 0;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V4UC_EXT:
                      l3cfg.l3c_flags = BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V4MC:
                      l3cfg.l3c_flags = BCM_L3_IPMC;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V6UC:
                      l3cfg.l3c_flags = BCM_L3_IP6;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V6UC_EXT:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_DEREFERENCED_NEXTHOP;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V6MC:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V4L2MC:
                case TD3_L3_HASH_DATA_TYPE_V4L2VPMC:
                      l3cfg.l3c_flags = BCM_L3_IPMC | BCM_L3_L2ONLY;
                      break;
                case TD3_L3_HASH_DATA_TYPE_V6L2MC:
                case TD3_L3_HASH_KEY_TYPE_V6L2VPMC:
                      l3cfg.l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC | BCM_L3_L2ONLY;
                default:
                      break;
                }
                /* Get host ip address. */
                rv = _bcm_td2_l3_get_by_idx(unit, NULL, index + i, &l3cfg, &nh_idx);
                if (BCM_FAILURE(rv)) {
                    continue;
                }

                BCM_IF_ERROR_RETURN
                    (_bcm_td2_l3cfg_to_ipkey(unit, cf_array + (*cf_count), &l3cfg));
                if ((++(*cf_count)) >= cf_max) {
                    break;
                }
            }
            /* increment based upon existing entry */
            i += (mem == L3_ENTRY_QUADm) ? 2 : 1;
        }

        /* Stop loop if conflicting IPs got from HW is no less than cf_max. */
        if ((*cf_count) >= cf_max) {
            break;
        }
    }
    return BCM_E_NONE;
}

#endif
