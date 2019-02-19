/* $Id: cint_nph_egress.c,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * 
* File: cint_nph_egress.c
* Purpose: An example of the port extender mapping. 
*          The following CINT provides a calling sequence example NPH header egress processing.
*
* Calling sequence:
*
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_13.BCM88675=10GBase-R64.0:core_0.13
*        ucode_port_14.BCM88675=10GBase-R65.0:core_0.14
*        ucode_port_15.BCM88675=10GBase-R68.0:core_0.15
*        ucode_port_16.BCM88675=10GBase-R69.0:core_0.16
*        ucode_port_32.BCM88675=10GBase-R65.1:core_0.32
*        ucode_port_33.BCM88675=10GBase-R65.2:core_0.33
*        ucode_port_34.BCM88675=10GBase-R65.3:core_0.34
*        ucode_port_35.BCM88675=10GBase-R65.4:core_0.35
*        ucode_port_36.BCM88675=10GBase-R65.5:core_0.36
*        custom_feature_vendor_custom_pp_port_14=1
*        custom_feature_vendor_customer65=1
*  2. Set PP port mapping according to NPH:
*        - call bcm_port_extender_mapping_info_set()
*               user_define_value should be encoded as { NPH.U/M[0], NPH.SubType[3:0], NPH.DelLength[5:0] }
*           The API does two things, map the packet to a PP port according to the key above, 
*           and set outer_header_start according to the DelLength
*  3. Create VLAN port, match key is { NPH.PS(3), NPH.output_fp(17) }:
*        - call bcm_vlan_create()
*  4. Create VPWS tunnel:
*        - call bcm_mpls_port_add()
*  5. Cross-connect VPWS tunnel to the VLAN port:
*        - call bcm_vswitch_cross_connect_add()
*
* Service 1 (AC/NP->PWE):
*           PORT    +    PS         +   output_fp <---->    PWE/AC
*              14     +     7          +     0x1000  <----->     PWE

* Service 2 (PWE/NP->AC):
*           PORT    +    PS         +   output_fp <---->    PWE/AC   
*              32     +     0          +     0x2000  <----->     AC
*
* Traffic:
*  1. port 14 -> port 13
*        - NPH: 00000a0d c00e1000 f00e0000 05200000
*        - Payload:
*              -   00000100 00010010 94000002 81000064 08004500 006a0000 0000fffd 393ec055 0102c000 00010000
*  2. port 14 -> port 13
*        - NPH: 00000a0d de002000 f00e0000 05200000
*        - Payload:
*              -   00000000 00220000 00000011 81000014 88470002 80280001 40140002 81ff0000 01000001 00109400 
*              -   00028100 00640800 4500006a 00000000 fffd393e c0550102 c0000001
*
*
* LIF extension data:
*     { stag_prio_flag[0], ctag_prio_flag[0], stag_status[1:0], ctag_status[1:0], cos_profile[3:0], stag_pcp_dei[3:0], ctag_pri_cfi[3:0], ctag_vid[11:0]}
*
* run: 
*      cint utility/cint_utils_l2.c 
*      cint utility/cint_utils_l3.c 
*      cint utility/cint_utils_global.c 
*      cint utility/cint_utils_mpls.c  
*      cint utility/cint_utils_mpls_port.c 
*      cint utility/cint_utils_multicast.c 
*      cint cint_port_tpid.c 
*      cint cint_advanced_vlan_translation_mode.c 
*      cint cint_mpls_lsr.c 
*      cint cint_vswitch_metro_mp.c  
*      cint cint_vswitch_cross_connect_p2p.c 
*      cint cint_connect_to_np_vpws.c
*      cint cint_nph_egress.c 
*      BCM> cint
*      config_ivec_value_mapping(0, 0);
*      config_ivec_value_mapping(0, 1);
*      config_ivec_value_mapping(0, 2);
*      config_ivec_value_mapping(0, 3);
*
*      service_port_init(ac_port,pw_port,ilk_port);
*      config_port_ingress(0, ac_port);
*
*      data_entry_init(ac_port, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 0);
*      config_ingress_vpws(0, 0);
*      data_entry_init(pw_port, 0x2000, 0, ac_output_fp, ac_ps, 0, 0);
*      config_ingress_vpws(0, 1);
*
*      nph_vpws_service_egress_run_with_defaults(0);
*/

struct nph_map_info_s {
    bcm_port_t np_port;
    uint32 uc;
    uint32 sub_type;
    uint32 del_length;
    bcm_port_t np_pp_port;
};

struct nph_service_info_egress_s {
    bcm_port_t np_port;
    uint32 ps;
    uint32 output_fp;
    bcm_gport_t vlan_port_id;
    bcm_gport_t gport; /* cross connect gport */
};

enum nph_uc_mc_defines_e {
    nph_uc,
    nph_mc
};

enum nph_sub_type_defines_e {
    nph_sub_type_vpws,
    nph_sub_type_vpls,
    nph_sub_type_l3vpn,
    nph_sub_type_swap
};

enum nph_vlan_status_defines_e {
    nph_vlan_status_none,
    nph_vlan_status_stag,
    nph_vlan_status_ctag,
    nph_vlan_status_s_ctag
};

enum nph_tag_action_defines_e {
    nph_tag_action_keep_0,
    nph_tag_action_keep_1,
    nph_tag_action_untag,
    nph_tag_action_tag
};

enum nph_vlan_editing_pcp_dei_flag_e {
    nph_pcp_dei_flag_non_uniform,
    nph_pcp_dei_flag_uniform
};

enum nph_vlan_editing_ive_cmd_e {
    nph_ive_cmd_nop=0,

    nph_ive_cmd_vs_none_add_c=4, /* VS=0, stag=keep/untag, ctag=tag */
    nph_ive_cmd_vs_none_add_s, /* VS=0, stag=tag, ctag=keep/untag */
    nph_ive_cmd_vs_none_add_s_add_c, /* VS=0, stag=TAG, ctag=TAG */

    nph_ive_cmd_vs_s_add_c, /* VS=1, stag=keep, ctag=tag */
    nph_ive_cmd_vs_s_swap_s_non_uniform, /* VS=1, stag=tag, non-uniform, ctag=keep/untag */
    nph_ive_cmd_vs_s_swap_s_uniform, /* VS=1, stag=tag, uniform, ctag=keep/untag */
    nph_ive_cmd_vs_s_swap_s_non_uniform_add_c, /* VS=1, stag=tag, non-uniform, ctag=tag */
    nph_ive_cmd_vs_s_swap_s_uniform_add_c, /* VS=1, stag=tag, uniform, ctag=tag */
    nph_ive_cmd_vs_s_del_s, /* VS=1, stag=untag, ctag=keep/untag */
    nph_ive_cmd_vs_s_del_s_add_c, /* VS=1, stag=untag, ctag=tag */

    nph_ive_cmd_vs_c_add_s, /* VS=2, stag=keep/untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_swap_c_non_uniform, /* VS=2, stag=keep/untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_swap_c_uniform, /* VS=2, stag=keep/untag, ctag=tag, uniform */
    nph_ive_cmd_vs_c_add_s_swap_c_non_uniform, /* VS=2, stag=tag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_add_s_swap_c_uniform, /* VS=2, stag=tag, ctag=tag, uniform */
    nph_ive_cmd_vs_c_del_c, /* VS=2, stag=keep/untag, ctag=untag */
    nph_ive_cmd_vs_c_add_s_del_c, /* VS=2, stag=tag, ctag=untag */

    nph_ive_cmd_vs_s_c_swap_s_non_uniform, /* VS=3, stag=tag, non-uniform ctag=keep */
    nph_ive_cmd_vs_s_c_swap_s_uniform, /* VS=3, stag=tag, uniform ctag=keep */
    nph_ive_cmd_vs_s_c_swap_c_non_uniform, /* VS=3, stag=keep, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_c_uniform, /* VS=3, stag=keep, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_del_s, /* VS=3, stag=untag, ctag=keep */
    nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform, /* VS=3, stag=untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_del_s_swap_c_uniform, /* VS=3, stag=untag, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_del_c, /* VS=3, stag=keep, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c, /* VS=3, stag=tag, non-uniform, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_uniform_del_c, /* VS=3, stag=tag, uniform, ctag=untag */
    nph_ive_cmd_vs_s_c_del_s_del_c, /* VS=3, stag=untag, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform, /* VS=3, stag=tag, non-uniform, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform, /* VS=3, stag=tag, non-uniform, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform, /* VS=3, stag=tag, uniform, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform, /* VS=3, stag=tag, uniform, ctag=tag, uniform */
    nph_ive_cmd_nof_cmds /* number of commands */
};

struct nph_egress_vlan_editing_info_s {
    nph_tag_action_defines_e stag_action;
    nph_tag_action_defines_e ctag_action;
    uint32 cos_profile;
    uint32 stag_vid;
    uint32 ctag_vid;
    uint32 stag_pcp_dei;
    uint32 ctag_pri_cfi;
    uint32 stag_pcp_dei_flag;
    uint32 ctag_pri_cfi_flag;
};

struct nph_egress_vlan_editing_ive_action_s {
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_action_t outer_prio_action;
    bcm_vlan_action_t inner_prio_action;
};

struct nph_egress_vlan_editing_map_s {
    uint32 vlan_status;
    uint32 stag_pcp_dei_flag;
    uint32 ctag_pri_cfi_flag;
    uint32 stag_status;
    uint32 ctag_status;
    uint32 vlan_status_mask;
    uint32 stag_pcp_dei_flag_mask;
    uint32 ctag_pri_cfi_flag_mask;
    uint32 stag_status_mask;
    uint32 ctag_status_mask;
    nph_vlan_editing_ive_cmd_e action;
};

int pwe_ps=7;
int pwe_output_fp=0x1000;
int ac_ps=0;
int ac_output_fp=0x2000;
int pwe_tunnel_length=22;

int ilk_port=14;

nph_map_info_s nph_map_info[] = {
    { ilk_port, nph_uc, nph_sub_type_vpws, 0, ilk_port },
    { ilk_port, nph_uc, nph_sub_type_vpws, 18, 32 }, /* MPLSoETH */
    { ilk_port, nph_uc, nph_sub_type_vpws, 22, 33 }, /* MPLS*2oETH, MPLSoVLANoETH */
    { ilk_port, nph_uc, nph_sub_type_vpws, 26, 34 }, /* MPLS*3oETH, MPLS*2oVLANoETH, MPLSoVLAN*2oETH */
    { ilk_port, nph_uc, nph_sub_type_vpws, 30, 35 }, /* MPLS*3oVLANoETH, MPLS*2oVLAN*2oETH */
    { ilk_port, nph_uc, nph_sub_type_vpws, 34, 36 } /* MPLS*3oVLAN*2oETH */
};

nph_service_info_egress_s nph_service_egress_info[2];
nph_egress_vlan_editing_info_s nph_egress_vlan_editing_info[2] = {
    { nph_tag_action_tag, nph_tag_action_tag, 0, 100, 200, 0xE, 0, nph_pcp_dei_flag_uniform, nph_pcp_dei_flag_uniform },
    { nph_tag_action_tag, nph_tag_action_tag, 0, 100, 200, 0xE, 0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform }
};

nph_egress_vlan_editing_ive_action_s nph_egress_vlan_editing_cmd[nph_ive_cmd_nof_cmds-nph_ive_cmd_vs_none_add_c] = {
    /*                              command description                                     *//*                 outer_action           ,                 inner_action             ,             outer_prio_action          ,          inner_prio_action              */ /* nof_tags_to_remove, outer tag src, outer tag prio src, inner tag src, inner tag prio src */
    /* nph_ive_cmd_vs_none_add_c                                                     */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 0, EDIT 1, mapped, NOT-SUPPORTED by API */
    /* nph_ive_cmd_vs_none_add_s                                                     */ { bcmVlanActionAdd          , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 0, EDIT 0, mapped */
    /* nph_ive_cmd_vs_none_add_s_add_c                                           */ { bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 0, EDIT 0, mapped; EDIT 1, mapped */

    /* nph_ive_cmd_vs_s_add_c                                                           */ { bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 1, outer tag, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_swap_s_non_uniform                                     */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 0, outer tag; */
    /* nph_ive_cmd_vs_s_swap_s_uniform                                            */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped; */
    /* nph_ive_cmd_vs_s_swap_s_non_uniform_add_c                          */ { bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 1, EDIT 0, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_swap_s_uniform_add_c                                 */ { bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 1, EDIT 0, mapped; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_del_s                                                           */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 */
    /* nph_ive_cmd_vs_s_del_s_add_c                                                 */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 1, mapped, NOT-SUPPORTED */

    /* nph_ive_cmd_vs_c_add_s                                                         */{ bcmVlanActionAdd           , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped */
    /* nph_ive_cmd_vs_c_swap_c_non_uniform                                    */{ bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 1, outer tag, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_c_swap_c_uniform                                           */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 1, mapped, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_c_add_s_swap_c_non_uniform                          */{ bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionOuterAdd }, /* 1, EDIT 0, mapped; EDIT 1, outer tag */
    /* nph_ive_cmd_vs_c_add_s_swap_c_uniform                                 */{ bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 1, EDIT 0, mapped; EDIT 1, mapped */
    /* nph_ive_cmd_vs_c_del_c                                                           */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 - duplicated */
    /* nph_ive_cmd_vs_c_add_s_del_c                                                 */{ bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped; - duplicated */

    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform                                 */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 0, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform                                        */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped */
    /* nph_ive_cmd_vs_s_c_swap_c_non_uniform                                 */ { bcmVlanActionNone         , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionInnerAdd }, /* 2, outer tag, outer tag; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_c_uniform                                        */ { bcmVlanActionNone         , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 2, outer tag, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_c_del_s                                                        */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 - duplicated */
    /* nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform                        */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionInnerAdd     , bcmVlanActionNone }, /* 2, EDIT 1, inner tag, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_s_c_del_s_swap_c_uniform                               */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 2, EDIT 1, mapped, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_s_c_del_c                                                        */ { bcmVlanActionNone         , bcmVlanActionDelete       , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 2, outer tag, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c                        */ { bcmVlanActionReplace      , bcmVlanActionDelete       , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 2, EDIT 0, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_del_c                               */ { bcmVlanActionReplace      , bcmVlanActionDelete       , bcmVlanActionAdd          , bcmVlanActionNone }, /* 2, EDIT 0, mapped */
    /* nph_ive_cmd_vs_s_c_del_s_del_c                                               */ { bcmVlanActionDelete       , bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone }, /* 2 */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionInnerAdd }, /* 2, EDIT 0, outer tag; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform        */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 2, EDIT 0, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform        */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionInnerAdd }, /* 2, EDIT 0, mapped; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform               */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd } /* 2, EDIT 0, mapped; EDIT 1, mapped */
};

uint32 dont_care=0;
uint32 tag_keep_mask=0x2;
uint32 full_mask_2b=0x3;
uint32 full_mask_1b=0x1;

nph_egress_vlan_editing_map_s nph_egress_vlan_editing_map[] = {
    /* vlan_status,     stag_pcp_dei_flag,                                 ctag_pri_cfi_flag,                  stag_status,                      ctag_status,                  vlan_status_mask,  stag_pcp_dei_flag_mask, ctag_pri_cfi_flag_mask, stag_status_mask, ctag_status_mask, action  */
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care,            dont_care,     tag_keep_mask, full_mask_2b,  nph_ive_cmd_vs_none_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b,  nph_ive_cmd_vs_none_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            dont_care,            dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_none_add_s},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_none_add_s},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_none_add_s_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care,            dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_nop},

    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care,            dont_care,     tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_swap_s_non_uniform},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_non_uniform},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_swap_s_uniform},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_uniform},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_non_uniform_add_c},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_uniform_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_del_s},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_del_s},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_del_s_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     tag_keep_mask, full_mask_2b ,  nph_ive_cmd_nop},

    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            dont_care,            dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_c_add_s},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_swap_c_uniform},    
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_del_c},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_del_c},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_nop},

    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_swap_s_non_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care   ,  full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_swap_s_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_c_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care   ,  full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_del_s},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_swap_c_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_untag ,        3,            dont_care   ,         dont_care   ,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_del_c},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care   ,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c},    
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_del_c},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_del_c},
    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            full_mask_1b,         full_mask_1b,  tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop}
};

int nph_nof_services = 2;
uint32 nph_ps_mask = 7;
uint32 nph_output_fp_mask = 0x1FFFF;
uint32 nph_output_uc_mask = 1;
uint32 nph_sub_type_mask = 0x3;
uint32 nph_del_length_mask = 0x3F;

struct nph_egr_vlan_editing_grp_info_s {
    bcm_field_group_t grp;
    int vs_qual_id;
    int lif_ext_qual_id;
};

nph_egr_vlan_editing_grp_info_s nph_egr_vlan_editing_grp_info;
bcm_field_presel_set_t nph_egr_psset;

int inlifs[] = {4096, 4097};
int inlif_gports[2];
bcm_multicast_t nph_egr_mcast_id = 0x2000;        

/* Init np port info */
int nph_service_info_egress_init(uint32 index, bcm_port_t np_port, uint32 ps, uint32 output_fp, bcm_gport_t gport)
{
    if (index >= nph_nof_services) {
        return BCM_E_PARAM;
    }

    if ((ps & (~nph_ps_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    if ((output_fp & (~nph_output_fp_mask)) != 0) {
        return BCM_E_PARAM;
    }

    nph_service_egress_info[index].np_port = np_port;
    nph_service_egress_info[index].ps = ps;
    nph_service_egress_info[index].output_fp = output_fp;
    nph_service_egress_info[index].gport = gport;

    return 0;
}

/* Init nph map info */
int nph_map_info_init(uint32 index, bcm_port_t np_port, uint32 uc, uint32 sub_type, uint32 del_len, bcm_port_t np_pp_port)
{
    if (index >= sizeof(nph_map_info)/sizeof(nph_map_info[0])) {
        return BCM_E_PARAM;
    }
    
    if ((uc & (~nph_output_uc_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    if ((sub_type & (~nph_sub_type_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    if ((del_len & (~nph_del_length_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    nph_map_info[index].np_port = np_port;
    nph_map_info[index].uc = uc;
    nph_map_info[index].sub_type = sub_type;
    nph_map_info[index].del_length = del_len;
    nph_map_info[index].np_pp_port = np_pp_port;

    return 0;
}

int nph_pp_port_mapping_set(int unit, int uc, int sub_type, int del_length, bcm_port_t np_port, bcm_port_t np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (uc<<10)|(sub_type << 6)|del_length;

    mapping_info.user_define_value=user_define_value;
    mapping_info.pp_port=np_pp_port;
    mapping_info.phy_port=np_port;

    /* Map the source board and souce port to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    return rv;
}

/* Get np port mapping */
int nph_pp_port_mapping_get(int unit, int uc, int sub_type, int del_length, bcm_port_t np_port, bcm_port_t *np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (uc<<10)|(sub_type << 6)|del_length;

    sal_memset(&mapping_info, 0, sizeof(bcm_port_extender_mapping_info_t));
    mapping_info.user_define_value=user_define_value;
    mapping_info.phy_port=np_port;

    /* Get the mapped in pp port based on source board and souce port */
    rv = bcm_port_extender_mapping_info_get(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    *np_pp_port = mapping_info.pp_port;

    return rv;
}

int nph_pp_port_mapping_init(int unit)
{
    int rv = 0;
    int index;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        user_define_value = (nph_map_info[index].uc<<10)|(nph_map_info[index].sub_type << 6)|nph_map_info[index].del_length;

        mapping_info.user_define_value=user_define_value;
        mapping_info.pp_port=nph_map_info[index].np_pp_port;
        mapping_info.phy_port=nph_map_info[index].np_port;

        /* Map the source board and souce port to in pp port */
        rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_extender_mapping_info_set failed $rv\n");
            return rv;
        }

        rv = bcm_port_class_set(unit, nph_map_info[index].np_pp_port, bcmPortClassId, nph_map_info[index].np_port);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_class_set failed $rv\n");
            return rv;
        }
    }

    return rv;
}

int nph_vpws_service_egress_run(int unit)
{
    int rv = 0;
    int index;
    bcm_port_t port;
    bcm_vlan_port_t vp;
    bcm_vswitch_cross_connect_t gports;

    for (index=0; index<nph_nof_services; index++) {
        bcm_vlan_port_t_init(&vp);    
        vp.port = nph_service_egress_info[index].np_port;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        vp.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
        rv = bcm_vlan_port_create(0, &vp);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_create failed $rv\n");
            return rv;
        }

        nph_service_egress_info[index].vlan_port_id = vp.vlan_port_id;

        printf("Created VLAN port for {ps, output_fp} (%d, 0x%x)\n", nph_service_egress_info[index].ps, nph_service_egress_info[index].output_fp);

        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = nph_service_egress_info[index].vlan_port_id;
        gports.port2 = nph_service_egress_info[index].gport;

        /* cross connect the 2 ports */
        if(verbose >= 1) {
            printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
        }
        
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 * InLIF extension data is organized as follows:
 *     { 2'bstag_status[1:0], 2'bctag_status[1:0]}
 * IVE command ID is encoded as {NPH.VS[1:0], output_fp.STAG[1:0], output_fp.CTAG[1:0]}
 * IVE command should be defined according to the following table:
 *
 *     VS \ TAG status | Keep | TAG | UNTAG |
 *     ----------------------------------
 *               0            | NOP  | ADD |  NOP    |
 *     ----------------------------------
 *               1            | NOP  |SWAP|  DEL    |
 *     ----------------------------------
 *
 * stag_status:
 *     stag_status[1]
 *         0 - keep ctag
 *         1 - don't keep ctag
 *     stag_status[0]
 *         0 - UNTAG
 *         1 - TAG
 *
 * ctag_status:
 *     ctag_status[1]
 *         0 - keep ctag
 *         1 - don't keep ctag
 *     ctag_status[0]
 *         0 - UNTAG
 *         1 - TAG
 */    
int nph_egress_vlan_editing_info_init(int index, nph_tag_action_defines_e stag_action, nph_tag_action_defines_e ctag_action, uint32 stag_vid, uint32 ctag_vid, uint32 stag_pcp_dei, uint32 ctag_pri_cfi, uint32 stag_pcp_dei_flag, uint32 ctag_pri_cfi_flag)
{
    if (index>1) {
        printf("invalid index\n");
    }
    
    nph_egress_vlan_editing_info[index].stag_action = stag_action;
    nph_egress_vlan_editing_info[index].ctag_action = ctag_action;
    nph_egress_vlan_editing_info[index].stag_vid = stag_vid & 0xFFF;
    nph_egress_vlan_editing_info[index].ctag_vid = ctag_vid &0xFFF;
    nph_egress_vlan_editing_info[index].stag_pcp_dei = stag_pcp_dei & 0xF;
    nph_egress_vlan_editing_info[index].ctag_pri_cfi = ctag_pri_cfi & 0xF;
    nph_egress_vlan_editing_info[index].stag_pcp_dei_flag = stag_pcp_dei_flag & 0x1;
    nph_egress_vlan_editing_info[index].ctag_pri_cfi_flag = ctag_pri_cfi_flag & 0x1;

    return 0;
}

int nph_vpws_service_egr_vlan_edit_vid_set(int unit, bcm_gport_t gport, uint32 s_vid, uint32 c_vid)
{
    bcm_vlan_port_translation_t port_trans;
    uint64 data_w;
    uint32 data;
    int rv;
    
    bcm_vlan_port_translation_t_init(&port_trans);  
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    port_trans.gport = gport;
    port_trans.vlan_edit_class_id = 0;
    port_trans.new_outer_vlan = s_vid;
    port_trans.new_inner_vlan = 0;

    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_translation_set failed\n");
        return rv;
    }

    rv = bcm_port_wide_data_get(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, &data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_get failed: $rv\n", rv);
        return rv;
    }

    data = COMPILER_64_LO(data_w);
    data &= ~0xFFF;
    data |= c_vid & 0xFFF;

    COMPILER_64_SET(data_w, 0, data);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_set failed: $rv\n", rv);
        return rv;
    }

    return rv;
}

int nph_vpws_service_egr_vlan_edit_profile_set(int unit, bcm_gport_t gport, uint32 stag_prio_flag, uint32 ctag_prio_flag, uint32 cos_profile, uint32 stag_pcp_dei, uint32 ctag_pri_cfi, uint32 stag_staus, uint32 ctag_status)
{
    uint64 data_w;
    uint32 data;
    int rv;

    data = stag_prio_flag & 0x1;
    data <<= 1;
    data |= ctag_prio_flag & 0x1;
    data <<=2;
    data |= stag_staus;
    data <<=2;
    data |= ctag_status;
    data <<=4;
    data |= cos_profile & 0xF;
    data <<=4;
    data |= stag_pcp_dei & 0xF;
    data <<=4;
    data |= ctag_pri_cfi & 0xF;

    if (verbose) {
        printf("data = 0x%08x\n", data);
    }

    rv = bcm_port_wide_data_get(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, &data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_get failed: $rv\n", rv);
        return rv;
    }

    data <<=12;
    data |= COMPILER_64_LO(data_w) & 0xFFF;

    COMPILER_64_SET(data_w, 0, data);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_set failed: $rv\n", rv);
        return rv;
    }

    return rv;
}

int nph_vpws_service_egr_vlan_edit_presel_create(int unit)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int presel_id;
    int index;
    int rv;
          
    /* Cretae a presel entity */
    rv = bcm_field_presel_create_id(unit, presel_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_presel_create_id\n");
        return rv;
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        BCM_PBMP_PORT_ADD(pbm, nph_map_info[index].np_pp_port);
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }
    
    BCM_FIELD_PRESEL_INIT(nph_egr_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_psset, presel_id);      

    return rv;
}

int nph_vpws_service_egr_vlan_edit_command_group_set(int unit, int priority)
{
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[2];
    int index;
    int rv;

    /* NPH.VS */
    index=0;
    bcm_field_data_qualifier_t_init(&data_qual[index]);
    data_qual[index].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual[index].offset = 39;
    data_qual[index].length = 2;
    data_qual[index].offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    /* stag_prio_flag[0], ctag_prio_flag[0], stag_status[1:0], ctag_status[1:0] */
    index=1;
    bcm_field_data_qualifier_t_init(&data_qual[index]);
    data_qual[index].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual[index].offset = 24;
    data_qual[index].length = 6;
    data_qual[index].qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(0, &data_qual[index]);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    for (index=0; index<2; index++) {
        rv = bcm_field_qset_data_qualifier_add(0, &grp.qset, data_qual[index].qual_id);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
            return rv;
        }
    }

    grp.priority = priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirect;
    grp.preselset = nph_egr_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVlanActionSetNew);
    
    rv = bcm_field_group_action_set(0, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(0, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_vlan_editing_grp_info.grp = grp.group;
    nph_egr_vlan_editing_grp_info.vs_qual_id = data_qual[0].qual_id;
    nph_egr_vlan_editing_grp_info.lif_ext_qual_id = data_qual[1].qual_id;
    
    return rv;
}

int nph_vpws_service_egr_vlan_edit_command_entry_install(int unit)
{
    bcm_field_entry_t ent;
    uint8 data;
    uint8 mask;
    int index;
    int rv;

    for (index=0; index<sizeof(nph_egress_vlan_editing_map)/sizeof(nph_egress_vlan_editing_map[0]); index++) {
        rv = bcm_field_entry_create(0, nph_egr_vlan_editing_grp_info.grp, &ent);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_entry_create failed: %d\n", rv);
            return rv;
        }

        data = nph_egress_vlan_editing_map[index].vlan_status;
        mask = nph_egress_vlan_editing_map[index].vlan_status_mask;
        
        rv = bcm_field_qualify_data(unit, ent, nph_egr_vlan_editing_grp_info.vs_qual_id, &data, &mask, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_data: vs\n");
            return rv;
        } 

        data = nph_egress_vlan_editing_map[index].stag_pcp_dei_flag;
        mask = nph_egress_vlan_editing_map[index].stag_pcp_dei_flag_mask;
        data <<=1;
        mask <<=1;
        data |= nph_egress_vlan_editing_map[index].ctag_pri_cfi_flag;
        mask |= nph_egress_vlan_editing_map[index].ctag_pri_cfi_flag_mask;
        data <<=2;
        mask <<=2;
        data |= nph_egress_vlan_editing_map[index].stag_status;
        mask |= nph_egress_vlan_editing_map[index].stag_status_mask;
        data <<=2;
        mask <<=2;
        data |= nph_egress_vlan_editing_map[index].ctag_status;
        mask |= nph_egress_vlan_editing_map[index].ctag_status_mask;

        rv = bcm_field_qualify_data(unit, ent, nph_egr_vlan_editing_grp_info.lif_ext_qual_id, &data, &mask, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_data: lif_ext\n");
            return rv;
        } 

        rv = bcm_field_action_add(0, ent, bcmFieldActionVlanActionSetNew, nph_egress_vlan_editing_map[index].action, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_action_add failed: %d\n", rv);
            return rv;
        }
    }

    rv = bcm_field_group_install(0, nph_egr_vlan_editing_grp_info.grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
    }
    
    return rv;
}


int nph_vpws_service_egr_vlan_edit_ctag_ext_group_set(int unit, int priority)
{
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext;
    bcm_field_extraction_action_t extact;
    int rv;

    /* ctag_vid[11:0] */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 12;
    data_qual.qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(0, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionInnerVlanNew);
    
    rv = bcm_field_group_action_set(0, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create: $rv\n");
        return rv;
    }

    extact.action = bcmFieldActionInnerVlanNew;
    ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext.bits = 12;
    ext.lsb = 0;
    ext.qualifier = data_qual.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_direct_extraction_action_add: $rv\n");
        return rv;
    }

    rv = bcm_field_group_install(0, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }
    
    return rv;
}

int nph_vpws_service_egr_vlan_editing_table_init(int unit)
{
    bcm_vlan_action_set_t action;
    int vs;
    int stag_status;
    int ctag_status;
    int index;
    int ive_cmd;
    int rv;

    for (index=0, ive_cmd=nph_ive_cmd_vs_none_add_c; index<nph_ive_cmd_nof_cmds-nph_ive_cmd_vs_none_add_c; index++,ive_cmd++) {
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ive_cmd);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = nph_egress_vlan_editing_cmd[index].outer_action;
        action.dt_inner = nph_egress_vlan_editing_cmd[index].inner_action;
        action.dt_outer_pkt_prio = nph_egress_vlan_editing_cmd[index].outer_prio_action;
        action.dt_inner_pkt_prio = nph_egress_vlan_editing_cmd[index].inner_prio_action;
        action.outer_tpid = 0x8100; 

        rv = bcm_vlan_translate_action_id_set( unit, 
                                               BCM_VLAN_ACTION_SET_INGRESS,
                                               ive_cmd,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set %d\n", ive_cmd);
            return rv;
        }                  
    }

    return rv;
}

int nph_vpws_service_egress_run_with_defaults(int unit)
{
    int index;

    nph_pp_port_mapping_init(unit);
    nph_service_info_egress_init(0, ilk_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_service_info_egress_init(1, ilk_port, pwe_ps, pwe_output_fp, mpls_port.mpls_port_id);

    nph_vpws_service_egress_run(unit);

    for (index=0; index<2; index++) {
        nph_vpws_service_egr_vlan_edit_vid_set(unit, nph_service_egress_info[index].vlan_port_id, 
            nph_egress_vlan_editing_info[index].stag_vid, 
            nph_egress_vlan_editing_info[index].ctag_vid);
        nph_vpws_service_egr_vlan_edit_profile_set(unit, nph_service_egress_info[index].vlan_port_id, 
            nph_egress_vlan_editing_info[index].stag_pcp_dei_flag, 
            nph_egress_vlan_editing_info[index].ctag_pri_cfi_flag, 
            nph_egress_vlan_editing_info[index].cos_profile, 
            nph_egress_vlan_editing_info[index].stag_pcp_dei, 
            nph_egress_vlan_editing_info[index].ctag_pri_cfi, 
            nph_egress_vlan_editing_info[index].stag_action, 
            nph_egress_vlan_editing_info[index].ctag_action);
    }
    nph_vpws_service_egr_vlan_editing_table_init(unit);
    nph_vpws_service_egr_vlan_edit_presel_create(unit);
    nph_vpws_service_egr_vlan_edit_command_group_set(unit, 10);
    nph_vpws_service_egr_vlan_edit_command_entry_install(unit);
    nph_vpws_service_egr_vlan_edit_ctag_ext_group_set(unit, 20);
}

int nph_vpws_service_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        del_len = (index==0)?0:(14+index*4);
        nph_map_info_init(index, np_port, nph_uc, nph_sub_type_vpws, del_len, np_port+index);
    }
    
    nph_pp_port_mapping_init(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 20;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    
    nph_service_info_egress_init(0, np_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_nof_services = 1;

    return nph_vpws_service_egress_run(unit);
}

int nph_vpws_service_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].gport);

    return 0;
}

int nph_vpls_uc_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        del_len = (index==0)?0:(14+index*4);
        nph_map_info_init(index, np_port, nph_uc, nph_sub_type_vpls, del_len, np_port+index);
    }
    
    nph_pp_port_mapping_init(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 20;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    
    nph_service_info_egress_init(0, np_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_nof_services = 1;

    return nph_vpws_service_egress_run(unit);
}

int nph_vpls_uc_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].gport);

    return 0;
}

/* direct extract VSI from NPH.VPN_ID */
int nph_vpls_mc_egr_vsi_dir_ext(int unit, int priority)
{
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    /* NPH[44:32] VPN ID */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 84 /* 128-48+44-40 */;
    data_qual.length = 13;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(0, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVSwitchNew);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionVSwitchNew;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 13;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext[1].bits = 3;
    ext[1].lsb = 0;
    ext[1].value = 0;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(0, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }
    
    return rv;
}

/* direct extract system lif from NPH.SRC_FP */
int nph_vpls_mc_egr_inlif_dir_ext(int unit, int priority)
{
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    /* NPH[16:0], SRC_FP} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 104 /* 128-24+16-16 */;
    data_qual.length = 17;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(0, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionIngressGportSet;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 17;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext[1].bits = 1;
    ext[1].lsb = 0;
    ext[1].value = 0;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(0, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }
    
    return rv;
}

int nph_vpls_mc_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t gport;      

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        del_len = (index==0)?0:(14+index*4);
        nph_map_info_init(index, np_port, nph_mc, nph_sub_type_vpls, del_len, np_port+index);
    }
    
    nph_pp_port_mapping_init(unit);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 20;
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, inlifs[0]);    
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &nph_egr_mcast_id);     
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }    
    
    BCM_GPORT_LOCAL_SET(gport, ac_port);    
    rv = bcm_multicast_ingress_add(unit, nph_egr_mcast_id, gport, vlan_port.encap_id);       
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }    

    inlif_gports[0] = vlan_port.vlan_port_id;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 21;
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, inlifs[1]);    
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, nph_egr_mcast_id, gport, vlan_port.encap_id);       
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }    
    
    inlif_gports[1] = vlan_port.vlan_port_id;

    BCM_GPORT_MCAST_SET(gport, nph_egr_mcast_id);

    nph_service_info_egress_init(0, np_port, ac_ps, ac_output_fp, gport);
    nph_nof_services = 1;

    nph_vpws_service_egr_vlan_edit_presel_create(unit);
    nph_vpls_mc_egr_vsi_dir_ext(unit, 30);
    nph_vpls_mc_egr_inlif_dir_ext(unit, 40);

    return nph_vpws_service_egress_run(unit);
}

int nph_vpls_mc_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_multicast_destroy(unit, nph_egr_mcast_id);
    bcm_vlan_port_destroy(unit, inlif_gports[0]);
    bcm_vlan_port_destroy(unit, inlif_gports[1]);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);

    return 0;
}

/*
* For MPLS-UC or LSP P2MP traffic, forward code & forward offset needs to be configured 
* through PMF. Forwrd code should be set to "MPLS", and forward offset should be set 
* to header 1 offset 0 (header 0 represents NPH + DEL_LEN bytes).
*/
int nph_mpls_egr_fwd_code_and_offset_set(int unit, int priority) {
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    uint8 data;
    uint8 mask;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_action_core_config_t config;
    int rv;

    /* NPH[123:120], sub_type} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 3;
    data_qual.length = 4;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(0, &qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }
    
    rv = bcm_field_group_create(unit, qset, priority, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_field_entry_create(0, grp, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed $rv\n");
        return rv;
    }

    data = 3/*nph_sub_type_swap*/;
    mask=0xF;

    rv = bcm_field_qualify_data(unit, ent, data_qual.qual_id, &data, &mask, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_data: sub_type\n");
        return rv;
    } 

    config.param0 = bcmFieldForwardingTypeMpls;
    config.param1 = 1; /* forwarding header index */
    config.param2 = 0; /* forwarding offset in the header */

    rv = bcm_field_action_config_add(0, ent, bcmFieldActionForwardingTypeNew, 1, &config);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_action_config_add failed $rv\n");
        return rv;
    }
    
    rv = bcm_field_group_install(0, grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed $rv\n");
        return rv;
    }

    return rv;
}


/*
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_mpls.c 
 * cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c 
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
 * cint ../../../../src/examples/dpp/cint_qos.c 
 * cint ../../../../src/examples/dpp/cint_port_tpid.c 
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 * cint ../../../../src/examples/dpp/cint_mpls_lsr.c 
 * cint ../../../../src/examples/dpp/cint_mpls_tunnel_initiator.c
 * cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c 
 * cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c 
 * cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c 
 * cint ../../../../src/examples/dpp/cint_nph_egress.c 
 * cint
 *     nph_service_info_egress_init(0,np_port,3,0x2000,0);
 *     nph_mpls_egress_run_with_defaults_dvapi(unit, mpls_port, np_port);
 *
 * Packet injected to NP port:
 *     UC/MC=0
 *     sub_type=3
 *     PS=3
 *     output_fp = 0x2000
 * 
 * Example data:
 *     0300010012062000f00e000105201000000000000033000000000011810000c88847017701404500002e0000000040fff8cc010101017fffff030001020304050508090a
 *
 * Expected packet on MPLS port:
 *     DA=00:00:00:00:00:44
 *     SA=00:00:00:00:00:33
 *     MPLS label = 1001
 *     
 */
int nph_mpls_egress_run_with_defaults_dvapi(int unit, bcm_port_t mpls_port, bcm_port_t np_port)
{
    int index = 0;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
	bcm_vswitch_cross_connect_t gports;

    tunnel_initiator_swap_action=1;
    
    /* create EEDB MPLS SWAP entry */
    mpls_tunnel_initiator_run_with_defaults(unit,-1,mpls_port,mpls_port);

    for (index=0; index<sizeof(nph_map_info)/sizeof(nph_map_info[0]); index++) {
        del_len = (index==0)?0:(14+index*4);
        nph_map_info_init(index, np_port, nph_uc, nph_sub_type_swap, del_len, np_port+index);
    }
    
    nph_pp_port_mapping_init(unit);

    bcm_vlan_port_t_init(&vlan_port);    
    index=0;
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[0].vlan_port_id;
	gports.port2 = mpls_port;
	gports.encap2 = BCM_GPORT_TUNNEL_ID_GET(outlif_to_count);

    nph_service_egress_info[index].gport = mpls_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    return nph_mpls_egr_fwd_code_and_offset_set(0, 10);
}

int nph_mpls_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
	gports.encap2 = BCM_GPORT_TUNNEL_ID_GET(outlif_to_count);
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);

    return 0;
}

