/*
 * $Id: switch.c,v 1.1.2.2 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/switch.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/port.h>

/* Helper routines for argument translation */

static int
_bool_invert(int unit, int arg, int set)
{
    /* Same for both set/get */
    return !arg;
}

/*
 * Chip 0 means no TH or TD2P
 */
bcm_switch_binding_t trident3_bindings[] =
{

#if defined(BCM_TRIUMPH3_SUPPORT)
    {bcmSwitchWlanClientAuthorizeAll, 0,
        AXP_WRX_MASTER_CTRLr, WCD_DISABLEf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    { bcmSwitchWlanClientUnauthToCpu, 0,
        CPU_CONTROL_Mr, WLAN_DOT1X_DROPf,
        NULL, 0},
    { bcmSwitchWlanClientRoamedOutErrorToCpu, 0,
        CPU_CONTROL_Mr, WLAN_ROAM_ERRORf,
        NULL, 0},
    { bcmSwitchWlanClientSrcMacMissToCpu, 0,
        CPU_CONTROL_Mr, WCD_SA_MISSf,
        NULL, 0},
    { bcmSwitchWlanClientDstMacMissToCpu, 0,
        CPU_CONTROL_Mr, WCD_DA_MISSf,
        NULL, 0},
    { bcmSwitchOamHeaderErrorToCpu, 1,
        CPU_CONTROL_Mr, OAM_HEADER_ERROR_TOCPUf,
        NULL, 0},
    { bcmSwitchOamUnknownVersionToCpu, 1,
        CPU_CONTROL_Mr, OAM_UNKNOWN_OPCODE_VERSION_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SrcBindFailToCpu, 1,
        CPU_CONTROL_Mr, MAC_BIND_FAILf,
        NULL, 0},
    { bcmSwitchTunnelIp4IdShared, 1,
        EGR_TUNNEL_ID_MASKr, SHARED_FRAG_ID_ENABLEf,
        NULL, 0},
    { bcmSwitchIpfixRateViolationDataInsert, 0,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_INSERT_DISABLEf,
        _bool_invert, soc_feature_ipfix_rate },
    { bcmSwitchIpfixRateViolationPersistent, 0,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_CONVERT_DISABLEf,
        NULL, 0},
    { bcmSwitchCustomerQueuing, 0,
        ING_MISC_CONFIGr, PHB2_COS_MODEf,
        NULL, soc_feature_vlan_queue_map},
    { bcmSwitchOamUnknownVersionDrop,  1,
        OAM_DROP_CONTROLr, OAM_UNKNOWN_OPCODE_VERSION_DROPf,
        NULL, 0},
    { bcmSwitchDirectedMirroring, 0,
        EGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 0,
        IEGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
#endif

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchOamUnexpectedPktToCpu, 1,
        CPU_CONTROL_Mr, OAM_UNEXPECTED_PKT_TOCPUf,
        NULL, 0},
#endif

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchOamCcmToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, ERROR_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconCcmToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, XCON_CCM_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchOamXconOtherToCpu, 1,
        CCM_COPYTO_CPU_CONTROLr, XCON_OTHER_COPY_TOCPUf,
        NULL, 0},
#endif /* !BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
    { bcmSwitchL3InterfaceUrpfEnable, 1,
       ING_CONFIG_64r, L3IIF_URPF_SELECTf,
        NULL, soc_feature_urpf },
#endif

#if defined(BCM_TRX_SUPPORT)
    { bcmSwitchClassBasedMoveFailPktToCpu, 1,
        CPU_CONTROL_1r, CLASS_BASED_SM_PREVENTED_TOCPUf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchL3UrpfFailToCpu,            1,
        CPU_CONTROL_1r, URPF_MISS_TOCPUf,
        NULL, soc_feature_urpf },
    { bcmSwitchClassBasedMoveFailPktDrop,  1,
        ING_MISC_CONFIG2r, CLASS_BASED_SM_PREVENTED_DROPf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchSTPBlockedFieldBypass,      1,
        ING_MISC_CONFIG2r, BLOCKED_PORTS_FP_DISABLEf,
        NULL, 0},
    { bcmSwitchRateLimitLinear,            1,
        MISCCONFIGr, ITU_MODE_SELf,
        _bool_invert, 0},
    { bcmSwitchRemoteLearnTrust,           1,
        ING_CONFIG_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchMldDirectAttachedOnly,      1,
        ING_CONFIG_64r, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanL2McastEnable,    1,
        ING_CONFIG_64r, LOOKUP_L2MC_WITH_FID_IDf,
        NULL, 0},
    { bcmSwitchMldUcastEnable,             1,
        ING_CONFIG_64r, MLD_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanEnable, 1,
        ING_CONFIG_64r, SVL_ENABLEf,
        NULL, 0},
    { bcmSwitchIgmpReservedMcastEnable,    1,
        ING_CONFIG_64r, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable,     1,
        ING_CONFIG_64r, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpUcastEnable,            1,
        ING_CONFIG_64r, IGMP_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchPortEgressBlockL3,          1,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L3f,
        NULL, 0},
    { bcmSwitchPortEgressBlockL2,          1,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L2f,
        NULL, 0},
    { bcmSwitchHgHdrExtLengthEnable,       1,
        ING_CONFIG_64r, IGNORE_HG_HDR_HDR_EXT_LENf,
        _bool_invert, 0 },
    { bcmSwitchIp4McastL2DestCheck,        1,
        ING_CONFIG_64r, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchIp6McastL2DestCheck,        1,
        ING_CONFIG_64r, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchL3TunnelUrpfMode,           1,
        ING_CONFIG_64r, TUNNEL_URPF_MODEf,
        NULL, soc_feature_urpf },
    { bcmSwitchL3TunnelUrpfDefaultRoute,   1,
        ING_CONFIG_64r, TUNNEL_URPF_DEFAULTROUTECHECKf,
        _bool_invert, soc_feature_urpf },
    { bcmSwitchMirrorStackMode, 1,
        ING_CONFIG_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 1,
        ING_CONFIG_64r, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchMirrorSrcModCheck, 1,
        ING_CONFIG_64r, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchProxySrcKnockout, 1,
        IHG_LOOKUPr, REMOVE_MH_SRC_PORTf,
        NULL, soc_feature_proxy_port_property },
#endif /* BCM_1_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    { bcmSwitchDosAttackToCpu, 1,
        CPU_CONTROL_0r, DOSATTACK_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, 0,
        CPU_CONTROL_0r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchUnknownVlanToCpu, 1,
        CPU_CONTROL_0r, UVLAN_TOCPUf,
        NULL, 0 },
    { bcmSwitchSourceMacZeroDrop, 0,
        CPU_CONTROL_0r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
    { bcmSwitchMplsSequenceErrToCpu, 0,
        ING_MISC_CONFIGr, MPLS_SEQ_NUM_FAIL_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLabelMissToCpu, 0,
        CPU_CONTROL_Mr, MPLS_LABEL_MISSf,
        NULL, 0},
    { bcmSwitchMplsTtlErrToCpu, 1,
        CPU_CONTROL_Mr, MPLS_TTL_CHECK_FAILf,
        NULL, 0},
    { bcmSwitchMplsInvalidL3PayloadToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_PAYLOADf,
        NULL, 0},
    { bcmSwitchMplsInvalidActionToCpu, 0,
        CPU_CONTROL_Mr, MPLS_INVALID_ACTIONf,
        NULL, 0},
    { bcmSwitchSharedVlanMismatchToCpu, 0,
        CPU_CONTROL_0r, PVLAN_VID_MISMATCH_TOCPUf ,
        NULL, 0},
    { bcmSwitchForceForwardFabricTrunk, 0,
        ING_MISC_CONFIGr, LOCAL_SW_DISABLE_HGTRUNK_RES_ENf,
        NULL, 0},
    { bcmSwitchFieldMultipathHashSeed, 0,
        FP_ECMP_HASH_CONTROLr, ECMP_HASH_SALTf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange1, 0,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 1,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 1,
       GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
       NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 1,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchL3UrpfRouteEnableExternal, 1,
       L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
       NULL, soc_feature_urpf},
    { bcmSwitchMplsPWControlWordToCpu, 0,
       ING_MISC_CONFIGr, PWACH_TOCPUf,
       NULL, soc_feature_mpls},
    { bcmSwitchMplsPWControlTypeToCpu, 0,
       ING_MISC_CONFIGr, OTHER_CW_TYPE_TOCPUf,
       NULL, soc_feature_mpls},
    { bcmSwitchMplsPWCountPktsAll, 0,
       ING_MISC_CONFIGr, PW_COUNT_ALLf,
       NULL, soc_feature_mpls},
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchCpuToCpuEnable, 1,
        ING_MISC_CONFIGr, DO_NOT_COPY_FROM_CPU_TO_CPUf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
    { bcmSwitchCpuProtocolPrio, 0,
        CPU_CONTROL_2r, CPU_PROTOCOL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT)
    { bcmSwitchAlternateStoreForward, 0,
        ASFCONFIGr, ASF_ENf,
        NULL, soc_feature_asf },
   { bcmSwitchRemoteLearnTrust, 0,
        ING_CONFIGr, DO_NOT_LEARN_ENABLEf,
        NULL, soc_feature_remote_learn_trust },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3UrpfRouteEnable, 1,
        L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
        NULL, soc_feature_urpf},
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchStationMoveOverLearnLimitToCpu, 1,
        CPU_CONTROL_1r, MACLMT_STNMV_TOCPUf,
        NULL, soc_feature_mac_learn_limit},
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchControlOpcodePriority, 0,
        CPU_CONTROL_2r, CPU_MH_CONTROL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */


#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchBpduInvalidVlanDrop, 1,
        EGR_CONFIG_1_64r, BPDU_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMirrorInvalidVlanDrop, 1,
        EGR_CONFIG_1_64r, MIRROR_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMcastFloodBlocking, 1,
        IGMP_MLD_PKT_CONTROLr, PFM_RULE_APPLYf,
        NULL, soc_feature_igmp_mld_support },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchIpmcTtl1ToCpu, 1,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcastTtl1ToCpu, 1,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchDhcpLearn, 1,
        ING_MISC_CONFIG2r, DO_NOT_LEARN_DHCPf,
        NULL, 0 },
    { bcmSwitchUnknownIpmcAsMcast, 1,
        ING_MISC_CONFIG2r, IPMC_MISS_AS_L2MCf,
        NULL, 0 },
    { bcmSwitchTunnelUnknownIpmcDrop, 1,
        ING_MISC_CONFIG2r, UNKNOWN_TUNNEL_IPMC_DROPf,
        NULL, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchMcastUnknownErrToCpu, 1,
        CPU_CONTROL_1r, MC_INDEX_ERROR_TOCPUf,
        NULL,0},
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) ||\
    defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchDosAttackMACSAEqualMACDA, 1,
        DOS_CONTROLr, MACSA_EQUALS_MACDA_DROPf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackFlagZeroSeqZero, 1,
        DOS_CONTROLr, TCP_FLAGS_CTRL0_SEQ0_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackSynFrag, 1,
        DOS_CONTROLr, TCP_FLAGS_SYN_FRAG_ENABLEf,
        NULL, 0},
    { bcmSwitchDosAttackIcmp, 1,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmp, 1,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV4, 1,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6, 1,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpFragments, 1,
        DOS_CONTROL_2r, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpOffset, 1,
        DOS_CONTROL_2r, TCP_HDR_OFFSET_EQ1_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackUdpPortsEqual, 1,
        DOS_CONTROL_2r, UDP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpPortsEqual, 1,
        DOS_CONTROL_2r, TCP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsSF, 1,
        DOS_CONTROL_2r, TCP_FLAGS_SYN_FIN_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsFUP, 1,
        DOS_CONTROL_2r, TCP_FLAGS_FIN_URG_PSH_SEQ0_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpHdrPartial, 1,
        DOS_CONTROL_2r, TCP_HDR_PARTIAL_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackV6MinFragEnable, 1,
        DOS_CONTROL_2r, IPV6_MIN_FRAG_SIZE_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackV6MinFragSize, 1,
        IPV6_MIN_FRAG_SIZEr, PKT_LENGTHf,
        NULL, soc_feature_enhanced_dos_ctrl },

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined (BCM_TRIDENT_SUPPORT) || defined (BCM_KATANA_SUPPORT)
    { bcmSwitchDosAttackIcmpPktOversize, 1,
        DOS_CONTROL_3r, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpV6PingSize, 1,
        DOS_CONTROL_3r, BIG_ICMPV6_PKT_SIZEf,
        NULL, soc_feature_big_icmpv6_ping_check },
    { bcmSwitchDosAttackMinTcpHdrSize, 1,
        DOS_CONTROL_3r, MIN_TCPHDR_SIZEf,
        NULL, 0 },
    { bcmSwitchHashNivSrcIfEtagSvidSelect0, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivSrcIfEtagSvidSelect1, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_0_SELECT_Bf,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect0, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Af,
        NULL, 0 },
    { bcmSwitchHashNivDstIfEtagVidSelect1, 1,
        RTAG7_HASH_CONTROL_2r, RTAG7_HASH_BIN_1_SELECT_Bf,
        NULL, 0 },
#endif /* BCM_TRIDENT_SUPPORT */


#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_BRADLEY_SUPPORT)
    { bcmSwitchDirectedMirroring, 0,
        ING_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDirectedMirroring, 1,
        EGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, 1,
        IEGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, 1,
        ING_CONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, 0,
        MISCCONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorSrcModCheck, 0,
        ING_CONFIGr, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorCopy, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_COPYf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorOnly, 0,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_ONLY_PKTf,
        NULL, soc_feature_src_modid_blk },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_RAVEN_SUPPORT)
    { bcmSwitchDestPortHGTrunk, 0,
        ING_MISC_CONFIGr, USE_DEST_PORTf,
        NULL, soc_feature_port_trunk_index },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_HELIX_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchDosAttackIcmpPktOversize, 1, 
        DOS_CONTROLr, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },
    { bcmSwitchDirectedMirroring, 1,
        MISCCONFIGr, DRACO_1_5_MIRRORING_MODE_ENf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDosAttackIcmpV6PingSize, 1,
        DOS_CONTROL_2r, BIG_ICMPV6_PKT_SIZEf,
        NULL, soc_feature_big_icmpv6_ping_check },
    { bcmSwitchMirrorUnmarked, 1,
        EGR_CONFIG_1_64r, RING_MODEf,
        _bool_invert, 0 },
    { bcmSwitchMirrorStackMode, 1,
        EGR_CONFIG_1_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMeterAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        FP_METER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        EFP_METER_CONTROL_2r, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 1,
        EGR_COUNTER_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, 0,
        MTRI_IFGr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchEgressBlockUcast, 1,
        ING_MISC_CONFIGr, APPLY_EGR_MASK_ON_UC_ONLYf,
        NULL, soc_feature_egress_blk_ucast_override },
    { bcmSwitchSourceModBlockUcast, 1,
        ING_MISC_CONFIGr, APPLY_SRCMOD_BLOCK_ON_UC_ONLYf,
        NULL, soc_feature_src_modid_blk_ucast_override },
    { bcmSwitchHgHdrMcastFloodOverride, 1,
        EGR_CONFIG_1_64r, FORCE_STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchShaperAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchShaperAdjust, 1,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTES_2f,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchShaperAdjust, 0,
        MTRI_IFGr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_HELIX_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchHgHdrErrToCpu, 1,
        CPU_CONTROL_1r, HG_HDR_ERROR_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchClassTagPacketsToCpu, 1,
        CPU_CONTROL_1r, HG_HDR_TYPE1_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchRemoteLearnTrust, 1,
        EGR_CONFIG_1_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchSourceModBlockControlOpcode, 1,
        ING_MISC_CONFIGr, DO_NOT_APPLY_SRCMOD_BLOCK_ON_SCf,
        _bool_invert, soc_feature_src_modid_blk_opcode_override },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
    { bcmSwitchCpuProtoBpduPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_BPDU_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoArpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_ARP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIgmpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IGMP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoDhcpPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_DHCP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpmcReservedPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IPMC_RESERVED_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpOptionsPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_IP_OPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoExceptionsPriority, 0,
        CPU_CONTROL_3r, CPU_PROTO_EXCEPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchUnknownMcastToCpu, 1,
    	CPU_CONTROL_1r, UMC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownUcastToCpu, 1,
    	CPU_CONTROL_1r, UUCAST_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2StaticMoveToCpu, 1,
    	CPU_CONTROL_1r, STATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3HeaderErrToCpu, 1, /* compat */
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3SrcToCpu, 1,
	    CPU_CONTROL_1r, UNRESOLVEDL3SRC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3DestToCpu, 1, /* compat */
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcPortMissToCpu, 1,
    	CPU_CONTROL_1r, IPMCPORTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcErrorToCpu, 1,
    	CPU_CONTROL_1r, IPMCERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2NonStaticMoveToCpu, 1, 
	    CPU_CONTROL_1r, NONSTATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3ErrToCpu, 1,
     	CPU_CONTROL_1r, V6L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3DstMissToCpu, 1,
	    CPU_CONTROL_1r, V6L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3ErrToCpu, 1,
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3DstMissToCpu, 1,
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchTunnelErrToCpu, 1,
    	CPU_CONTROL_1r, TUNNEL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchMartianAddrToCpu, 1,
     	CPU_CONTROL_1r, MARTIAN_ADDR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcTtlErrToCpu, 1,
    	CPU_CONTROL_1r, L3UC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3SlowpathToCpu, 1,
    	CPU_CONTROL_1r, L3_SLOWPATH_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcTtlErrToCpu, 1,
    	CPU_CONTROL_1r, IPMC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchSampleIngressRandomSeed, 1,
	    SFLOW_ING_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchSampleEgressRandomSeed, 1,
    	SFLOW_EGR_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchArpReplyToCpu, 1,
	    PROTOCOL_PKT_CONTROLr, ARP_REPLY_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpReplyDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REPLY_DROPf,
        NULL, 0 },
    { bcmSwitchArpRequestToCpu, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpRequestDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_DROPf,
        NULL, 0 },
    { bcmSwitchNdPktToCpu, 1,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchNdPktDrop, 1,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDhcpPktToCpu, 1,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchDhcpPktDrop, 1,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDosAttackSipEqualDip, 1,
        DOS_CONTROLr, DROP_IF_SIP_EQUALS_DIPf,
        NULL, 0 },
    { bcmSwitchDosAttackMinTcpHdrSize, 1,
        DOS_CONTROLr, MIN_TCPHDR_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackV4FirstFrag, 1,
        DOS_CONTROLr, IPV4_FIRST_FRAG_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchNonIpL3ErrToCpu, 1, 
        CPU_CONTROL_1r, NIP_L3ERR_TOCPUf,
        NULL, soc_feature_nip_l3_err_tocpu },
    { bcmSwitchSourceRouteToCpu, 1,
	    CPU_CONTROL_1r, SRCROUTE_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, 0,
        CPU_CONTROL_1r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchDirectedMirroring, 1,
        EGR_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror},
#endif /* BCM_HELIX_SUPPORT || BCM_FIREBOLT_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAVEN_SUPPORT) ||  \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3MtuFailToCpu, 1,
        CPU_CONTROL_1r, L3_MTU_FAIL_TOCPUf,
        NULL, soc_feature_l3mtu_fail_tocpu },
    { bcmSwitchIpmcSameVlanL3Route, 1,
        EGR_CONFIG_1_64r, IPMC_ROUTE_SAME_VLANf,
        NULL, soc_feature_l3},
    { bcmSwitchIpmcSameVlanPruning, 1,
        MMU_SCFG_TOQ_MC_CFG0r, IPMC_IND_MODEf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_KATANA_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    { bcmSwitchPrioDropToCpu, 1,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivPrioDropToCpu, 1,
        CPU_CONTROL_0r, DOT1P_ADMITTANCE_DROP_TOCPUf,
        NULL, soc_feature_niv},
#if 0 /* TD3TBD NIV_FORWARDING_DROP_TOCPUf is removed */
    { bcmSwitchNivInterfaceMissToCpu, 1,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_niv},
#endif
    { bcmSwitchNivRpfFailToCpu, 1,
        CPU_CONTROL_0r, RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagInvalidToCpu, 1,
        CPU_CONTROL_0r, VNTAG_FORMAT_DROP_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivTagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchNivUntagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_niv},
    { bcmSwitchTrillTtlErrToCpu, 1,
        CPU_CONTROL_0r, TRILL_HOPCOUNT_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderErrToCpu, 1,
        CPU_CONTROL_0r, TRILL_ERROR_FRAMES_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillMismatchToCpu, 1,
        CPU_CONTROL_0r, TRILL_UNEXPECTED_FRAMES_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameMissToCpu, 1,
        CPU_CONTROL_0r, TRILL_RBRIDGE_LOOKUP_MISS_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillRpfFailToCpu, 1,
        CPU_CONTROL_0r, TRILL_RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillOptionsToCpu, 1,
        CPU_CONTROL_0r, TRILL_OPTIONS_TOCPUf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameErrDrop, 1,
        TRILL_DROP_CONTROLr, INGRESS_RBRIDGE_EQ_EGRESS_RBRIDGE_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderVersionErrDrop, 1,
        TRILL_DROP_CONTROLr, TRILL_HDR_VERSION_NON_ZERO_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillNameMissDrop, 1,
        TRILL_DROP_CONTROLr, UNKNOWN_INGRESS_RBRIDGE_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillAdjacencyFailDrop, 1,
        TRILL_DROP_CONTROLr, TRILL_ADJACENCY_FAIL_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillHeaderErrDrop, 1,
        TRILL_DROP_CONTROLr, UC_TRILL_HDR_MC_MACDA_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchTrillRpfFailDrop, 1,
        TRILL_DROP_CONTROLr, RPF_CHECK_FAIL_DROPf,
        NULL, soc_feature_trill},
    { bcmSwitchIngParseL2TunnelTermDipSipSelect, 1, 
        ING_L2_TUNNEL_PARSE_CONTROLr, IFP_L2_TUNNEL_PAYLOAD_FIELD_SELf ,
        NULL, 0},
    { bcmSwitchIngParseL3L4IPv4, 1,
        ING_L2_TUNNEL_PARSE_CONTROLr, IGMP_ENABLEf ,
        NULL, 0},
    { bcmSwitchIngParseL3L4IPv6, 1,
        ING_L2_TUNNEL_PARSE_CONTROLr, MLD_ENABLEf ,
        NULL, 0},
    { bcmSwitchV6L3LocalLinkDrop, 1,
        ING_MISC_CONFIG2r, IPV6_SIP_LINK_LOCAL_DROPf,
        NULL, 0},
    /* This switch control is valid for TD+ only.
     * 'soc_feature_l3_ecmp_1k_groups' is used to
     * distinguish between TD and TD+ */
    { bcmSwitchEcmpMacroFlowHashEnable, 0,
        ING_CONFIG_2r, ECMP_HASH_16BITSf,
        NULL, soc_feature_l3_ecmp_1k_groups },
#endif /* BCM_TRIDENT_SUPPORT || BCM_KATANA_SUPPORT || BCM_KATANA2_SUPPORT*/

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
#if 0 /* TD3TBD NIV_FORWARDING_DROP_TOCPUf is removed */
    { bcmSwitchExtenderInterfaceMissToCpu, 1,
        CPU_CONTROL_0r, NIV_FORWARDING_DROP_TOCPUf,
        NULL, soc_feature_port_extension},
#endif
    { bcmSwitchExtenderRpfFailToCpu, 1,
        CPU_CONTROL_0r, RPF_CHECK_FAIL_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchEtagInvalidToCpu, 1,
        CPU_CONTROL_0r, VNTAG_FORMAT_DROP_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchEtagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchNonEtagDropToCpu, 1,
        CPU_CONTROL_0r, DISCARD_VNTAG_NOT_PRESENT_TOCPUf,
        NULL, soc_feature_port_extension},
    { bcmSwitchMplsGalAlertLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_GAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsRalAlertLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_RAL_EXPOSED_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsIllegalReservedLabelToCpu, 1,
        CPU_CONTROL_Mr, MPLS_ILLEGAL_RSVD_LABEL_TO_CPUf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchMplsUnknownAchTypeToCpu, 1,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_TYPE_TO_CPUf,
        NULL, 0},
    { bcmSwitchMplsUnknownAchVersionToCpu, 1,
        CPU_CONTROL_Mr, MPLS_UNKNOWN_ACH_VERSION_TOCPUf,
        NULL, 0},
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchFailoverDropToCpu, 0,
        CPU_CONTROL_1r, PROTECTION_DATA_DROP_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsReservedEntropyLabelToCpu, 0,
        CPU_CONTROL_Mr, ENTROPY_LABEL_IN_0_15_RANGE_TO_CPUf,
        NULL, 0},
    { bcmSwitchL3SrcBindMissToCpu, 1,
        CPU_CONTROL_Mr, MAC_IP_BIND_LOOKUP_MISS_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLookupsExceededToCpu, 0,
        CPU_CONTROL_Mr, MPLS_OUT_OF_LOOKUPS_TOCPUf,
        NULL, 0},
    { bcmSwitchTimesyncUnknownVersionToCpu, 1,
        CPU_CONTROL_Mr, UNKNOWN_1588_VERSION_TO_CPUf,
        NULL, 0},
    { bcmSwitchTimesyncIngressVersion, 1,
        ING_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchTimesyncEgressVersion, 1,
        EGR_1588_PARSING_CONTROLr, VERSION_CONTROLf,
        NULL, 0},
    { bcmSwitchCongestionCnmToCpu, 1,
        IE2E_CONTROLr, ICNM_TO_CPUf,
        NULL, 0},
    { bcmSwtichCongestionCnmProxyErrorToCpu, 1,
        ING_MISC_CONFIGr, QCN_CNM_PRP_DLF_TO_CPUf,
        NULL, 0},
    { bcmSwtichCongestionCnmProxyToCpu, 1,
        ING_MISC_CONFIGr, QCN_CNM_PRP_TO_CPUf,
        NULL, 0},
    { bcmSwitchL2GreTunnelMissToCpu, 1,
        CPU_CONTROL_Mr, L2GRE_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL2GreVpnIdMissToCpu, 1,
        CPU_CONTROL_Mr, L2GRE_VPNID_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchWlanTunnelErrorDrop, 0,
        ING_CONFIG_64r, WLAN_DECRYPT_OFFLOAD_ENABLEf,
        _bool_invert, 0},
    { bcmSwitchMplsReservedEntropyLabelDrop, 0,
        DROP_CONTROL_0r, ENTROPY_LABEL_IN_0_15_RANGE_DO_NOT_DROPf,
        _bool_invert, 0},
    { bcmSwitchRemoteProtectionTrust, 1,
        ING_CONFIG_64r, USE_PROT_STATUSf,
        NULL, 0},
    { bcmSwitchVxlanTunnelMissToCpu, 1,
        CPU_CONTROL_Mr, VXLAN_SIP_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchVxlanVnIdMissToCpu, 1,
        CPU_CONTROL_Mr, VXLAN_VN_ID_LOOKUP_FAIL_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchHashL2GreKeyMask0, 1,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Ar, L2GRE_TUNNEL_GRE_KEY_MASK_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreKeyMask1, 1,
        RTAG7_HASH_CONTROL_L2GRE_MASK_Br, L2GRE_TUNNEL_GRE_KEY_MASK_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchRemoteProtectionEnable, 1 ,
        EGR_HG_HDR_PROT_STATUS_TX_CONTROLr, SET_PROT_STATUSf,
        NULL, 0},

#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
    { bcmSwitchL3NATEnable, 1,
        ING_CONFIG_64r, NAT_ENABLEf,
        NULL, 0},
    { bcmSwitchL3DNATHairpinToCpu, 1,
        CPU_CONTROL_Mr, DNAT_HAIRPIN_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3DNATMissToCpu, 1,
        CPU_CONTROL_Mr, DNAT_MISS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SNATMissToCpu, 1,
        CPU_CONTROL_Mr, SNAT_MISS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatOtherToCpu, 1,
        CPU_CONTROL_Mr, NAT_OTHER_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatRealmCrossingIcmpToCpu, 1,
        CPU_CONTROL_Mr, NAT_REALM_CROSSING_ICMP_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchL3NatFragmentsToCpu, 1,
        CPU_CONTROL_Mr, NAT_FRAGMENTS_COPY_TOCPUf,
        NULL, 0},
    { bcmSwitchHashUseL2GreTunnelGreKey0, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashUseL2GreTunnelGreKey1, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_TUNNEL_USE_GRE_KEY_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GrePayloadSelect0, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_PAYLOAD_HASH_SELECT_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GrePayloadSelect1, 1,
        RTAG7_HASH_CONTROL_2r, L2GRE_PAYLOAD_HASH_SELECT_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreNetworkPortPayloadDisable0, 1,
        RTAG7_HASH_CONTROL_2r, DISABLE_HASH_L2GRE_Af,
        NULL, soc_feature_l2gre},
    { bcmSwitchHashL2GreNetworkPortPayloadDisable1, 1,
        RTAG7_HASH_CONTROL_2r, DISABLE_HASH_L2GRE_Bf,
        NULL, soc_feature_l2gre},
    { bcmSwitchEcmpMacroFlowHashEnable, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
    { bcmSwitchMcQueueSchedMode, 1,
        HSP_SCHED_PORT_CONFIGr, MC_GROUP_MODEf,
        NULL, 0},
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchHashField0Ip6FlowLabel, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Af,
        NULL, 0},
    { bcmSwitchHashField1Ip6FlowLabel, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_FLOW_LABEL_IPV6_Bf,
        NULL, 0},
    { bcmSwitchMacroFlowHashUseMSB, 1,
       RTAG7_HASH_CONTROL_2r, MACRO_FLOW_HASH_BYTE_SELf,
        NULL, 0},
#endif
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    { bcmSwitchHashHg2PktFieldsEnable, 1,
       RTAG7_HASH_CONTROL_2r, TREAT_PPD2_AS_KNOWN_PPDf,
        NULL, 0},
    { bcmSwitchHashField0OverlayCntagRpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Af,
        NULL, 0},
    { bcmSwitchMimPayloadTpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, MIM_PAYLOAD_TPID_ENABLEf,
        NULL, soc_feature_mim},
    { bcmSwitchHashMimUseTunnelHeader0, 1,
       RTAG7_HASH_CONTROL_2r, DISABLE_HASH_MIM_INNER_L2_Af,
        NULL, soc_feature_mim},
    { bcmSwitchHashMimUseTunnelHeader1, 1,
       RTAG7_HASH_CONTROL_2r, DISABLE_HASH_MIM_INNER_L2_Bf,
        NULL, soc_feature_mim},
    { bcmSwitchHashField1OverlayCntagRpidEnable, 1,
       RTAG7_HASH_CONTROL_2r, ENABLE_BIN_12_OVERLAY_Bf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 0,
       RTAG7_HASH_CONTROL_2r, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelTrunkUc , 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_TRUNK_UCf,
        NULL, 0},
    { bcmSwitchHashUseFlowSelEcmp, 1,
       RTAG7_HASH_SELr, USE_FLOW_SEL_ECMPf,
        NULL, 0},
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    { bcmSwitchMplsPortIndependentLowerRange1, 1,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, 1,
        GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, 1,
        GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, 1,
        GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
        NULL, 0},
#endif
    { bcmSwitchHashUseFlowSelMplsEcmp, 1,
        RTAG7_HASH_SELr, USE_FLOW_SEL_MPLS_ECMPf,
        NULL, 0},
    { bcmSwitchHashMPLSUseLabelStack, 1,
       RTAG7_HASH_CONTROLr, USE_MPLS_STACK_FOR_HASHINGf,
        NULL, 0},
};

int
_bcm_td3_switch_control_port_binding_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg, int *found)
{
    uint64 oval64, val64;
    int i;
    bcm_switch_binding_t *b;
    uint32 max;
    int fval, fwidth, prt, idx;

    *found = 0;

    for (i = 0; i < COUNTOF(trident3_bindings); i++) {
        b = &trident3_bindings[i];
        /* b->chip == 1 check will be removed after we done bringup and final clean up */
        if ((b->type == type) && (b->chip == 1)) {
            /* We don't check the b->chip here since the table is specific for Greyhound*/
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }
            if (!soc_reg_field_valid(unit, b->reg, b->field)) {
                continue;
            }
            if (b->xlate_arg) {
                if ((fval = (b->xlate_arg)(unit, arg, 1)) < 0) {
                    return fval;
                }
            } else {
                fval = arg;
            }
            /* Negative values should be treated as errors */
            if (fval < 0) {
                return BCM_E_PARAM;
            }
            if ((b->reg == PROTOCOL_PKT_CONTROLr) ||
                (b->reg == IGMP_MLD_PKT_CONTROLr)) {
                int index;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_get(unit, port,
                    PROTOCOL_PKT_INDEXf, &index));
                prt = port;
                idx = index;
            } else {
                prt = port;
                idx = 0;
            }

            fwidth = soc_reg_field_length(unit, b->reg, b->field);
            if (fwidth < 32) {
                max = (1 << fwidth) - 1;
            } else {
                max = -1;
            }
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
            oval64 = val64;
            soc_reg64_field32_set(unit, b->reg, &val64, b->field,
                                ((uint32)fval > max) ? max : (uint32)fval);
            if (COMPILER_64_NE(val64, oval64)) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, b->reg, prt, idx, val64));
            }

            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}

int
_bcm_td3_switch_control_port_binding_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg, int *found)
{
    uint64 val64;
    int i;
    bcm_switch_binding_t *b;
    int prt, idx;

    *found = 0;

    for (i = 0; i < COUNTOF(trident3_bindings); i++) {
        b = &trident3_bindings[i];
        /* b->chip == 1 check will be removed after we done bringup and final clean up */
        if ((b->type == type) && (b->chip == 1)) {
            if (b->feature && !soc_feature(unit, b->feature)) {
                continue;
            }
            if (!SOC_REG_IS_VALID(unit, b->reg)) {
                continue;
            }

            if (!soc_reg_field_valid(unit, b->reg, b->field)) {
                continue;
            }
            if ((b->reg == PROTOCOL_PKT_CONTROLr) ||
                (b->reg == IGMP_MLD_PKT_CONTROLr)) {
                int index;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_get(unit, port, 
                    PROTOCOL_PKT_INDEXf, &index));
                prt = port;
                idx = index;
            } else {
                prt = port;
                idx = 0;
            }
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, b->reg, prt, idx, &val64));
            *arg = soc_reg64_field32_get(unit, b->reg, val64, b->field);

            if (b->xlate_arg) {
                *arg = (b->xlate_arg)(unit, *arg, 0);
            }
            *found = 1;
            break;
        }
    }
    return BCM_E_NONE;
}
