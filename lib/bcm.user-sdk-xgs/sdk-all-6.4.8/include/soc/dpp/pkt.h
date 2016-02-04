/*
* $Id: pkt.h,v 1.4 Broadcom SDK $
* $Copyright: Copyright 2015 Broadcom Corporation.
* This program is the proprietary software of Broadcom Corporation
* and/or its licensors, and may only be used, duplicated, modified
* or distributed pursuant to the terms and conditions of a separate,
* written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized
* License, Broadcom grants no license (express or implied), right
* to use, or waiver of any kind with respect to the Software, and
* Broadcom expressly reserves all rights in and to the Software
* and all intellectual property rights therein.  IF YOU HAVE
* NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
* IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
* ALL USE OF THE SOFTWARE.  
*  
* Except as expressly set forth in the Authorized License,
*  
* 1.     This program, including its structure, sequence and organization,
* constitutes the valuable trade secrets of Broadcom, and you shall use
* all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of
* Broadcom integrated circuit products.
*  
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
* PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
* REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
* OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
* DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
* NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
* ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
* CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
* OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
* 
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
* BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
* INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
* ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
* TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
* THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
* WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
* ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*
* This file contains structures and functions declarations for 
* processing Packets.
* 
*/
#ifndef _SOC_DPP_PKT_H
#define _SOC_DPP_PKT_H

#include <soc/error.h>


/* bcm_mac_t */
typedef uint8 soc_mac_t[6];

#ifdef BCM_ARAD_SUPPORT
#endif /* BCM_ARAD_SUPPORT */

/* PTCH1 */
typedef struct  {
    _shr_gport_t src_gport;      /* PTCH-1.Source-System-Port. */
    uint32 opaque_attr;         /* PTCH-1.Opaque-PT-Attributes. */
    uint8 is_port_header_type;  /* If set, the next header type is set according
                                   to the Source-local Port. Otherwise, the next
                                   header must be ITMH
                                   (PTCH-1.Parser-Program-Control). */
} soc_pkt_dnx_ptch1_t;

 
/* PTCH2 */
typedef struct  {
    _shr_gport_t src_local_port; /* Source-local-port (PTCH-2.PP-SSP). */
    uint32 opaque_attr;         /* PTCH-2.Opaque-PT-Attributes. */
    uint8 is_port_header_type;  /* If set, the next header type is set according
                                   to the Source-local-Port. Otherwise, the next
                                   header must be ITMH
                                   (PTCH-2.Parser-Program-Control) */
} soc_pkt_dnx_ptch2_t;
/* Itmh destination. */
typedef struct  {
    _shr_pkt_dnx_itmh_dest_type_t dest_type; /* Destination type */
    _shr_pkt_dnx_itmh_dest_type_t dest_extension_type; /* Destination Extension type */
    _shr_gport_t destination;            /* Destination Gport */
    soc_multicast_t multicast_id;       /* Destination multicast */
    _shr_gport_t destination_ext;        /* Destination-Extension Gport */
} soc_pkt_dnx_itmh_dest_t;

/* Itmh */
typedef struct  {
    uint8 inbound_mirror_disable;   /* If set, disable inbound mirroring
                                       (ITMH.IN_MIRR_DISABLE) */
    uint32 snoop_cmnd;              /* snoop command (ITMH.SNOOP_CMD) */
    uint32 prio;                    /* Traffic Class (ITMH.FWD_TRAFFIC_CLASS) */
    _shr_color_t color;              /* Color (aka Drop precedence, ITMH.FWD_DP) */
    soc_pkt_dnx_itmh_dest_t dest;   /* Destination information */
} soc_pkt_dnx_itmh_t;
/* ftmh lb extension. */
typedef struct  {
    uint8 valid;    /* Set if the extension is present */
    uint32 lb_key;  /* Load Balancing Key (FTMH.LB-Key) */
} soc_pkt_dnx_ftmh_lb_extension_t;

/* ftmh dest extension. */
typedef struct  {
    uint8 valid;                /* Set if the extension is present */
    _shr_gport_t dst_sysport;    /* Destination System Port
                                   (FTMH.Destination-Sys-Port)) */
} soc_pkt_dnx_ftmh_dest_extension_t;

/* ftmh stack extension. */
typedef struct  {
    uint8 valid;                    /* Set if the extension is present */
    uint32 stack_route_history_bmp; /* Route bitmap to prevent loops in stacking
                                       system
                                       (FTMH.Stacking-Route-History-Bitmap) */
} soc_pkt_dnx_ftmh_stack_extension_t;    
/* ftmh */
typedef struct  {
    uint32 packet_size;                 /* Packet size in bytes
                                           (FTMH.Packet-Size) */
    uint32 prio;                        /* Traffic class (FTMH.Traffic-Class) */
    _shr_gport_t src_sysport;            /* Source System port
                                           (FTMH.Source-System-Port-Aggr) */
    _shr_gport_t dst_port;               /* Destination local port (FTMH.PP_DSP) */
    _shr_color_t ftmh_dp;                /* Drop precedence (FTMH.DP) */
    _shr_pkt_dnx_ftmh_action_type_t action_type; /* Action type (FTMH.TM-Action-Type) */
    uint8 out_mirror_disable;           /* Disable Outbound mirroring
                                           (FTMH.Out-Mirror-Disable) */
    uint8 is_mc_traffic;                /* Indicate if the traffic is multicast
                                           (FTMH.TM-Action-Is-Multicast) */
    soc_multicast_t multicast_id;       /* Multicast ID (FTMH.Multicast-ID).
                                           Valid only if is_mc_traffic is set */
    _shr_gport_t out_vport;              /* Virtual port (FTMH.Out-LIF). Valid
                                           only if is_mc_traffic is unset */
    uint32 cni;                         /* Congestion indication (FTMH.CNI) */
    soc_pkt_dnx_ftmh_lb_extension_t lb_ext; /* FTMH Load Balancing Key extension */
    soc_pkt_dnx_ftmh_dest_extension_t dest_ext; /* FTMH Destination System Port
                                           Extension */
    soc_pkt_dnx_ftmh_stack_extension_t stack_ext; /* FTMH Stacking extension
                                           (Stacking-Route-History-Bitmap) */
} soc_pkt_dnx_ftmh_t;

typedef struct  {
    uint8 valid;    /* Set if the extension is present */
    uint8 is_mim;   /* Set if the packet is MAC-in-MAC (PPH.EEI.MIM-P2P-VSI) */
    uint32 i_sid;   /* I-SID (PPH.EEI.I-SID) */
    uint32 command; /* Command (PPH.EEI.Command) */
    uint32 data;    /* Data (PPH.EEI.Data) */
} soc_pkt_dnx_pph_eei_extension_t;

/* pph learn extension */
typedef struct _shr_pkt_dnx_pph_learn_extension_s {
    uint8 valid;                        /* Set if the extension is present */
    _shr_port_t phy_gport;               /* Learn Destination port
                                           (PPH.Learn.Learn-Destination) */
    uint8 is_eei_valid;                 /* Set if the EEI info is valid */
    soc_pkt_dnx_pph_eei_extension_t eei; /* EEI information (PPH.Learn.Learn-ASD) */
    uint8 is_encap_id_valid;            /* Set if the encap-id is valid */
    int encap_id;                       /* Encap-Id (PPH.Learn.Learn-ASD). Valid
                                           if is_encap_id_valid is set */
    uint64 raw_data;                    /* Learn-extension raw data (PPH.Learn) */
} soc_pkt_dnx_pph_learn_extension_t;
/* pph fhei bridge extension */
typedef struct  {
    uint8 ive_cmd;      /* Ingress-VLAN-Edit-Command
                           (FHEI.Bridge.Ingress-VLAN-Edit-Command) */
    uint8 pcp1;         /* PCP-1 (FHEI.Bridge.Edit-PCP1) */
    uint8 dei1;         /* DEI-1 (FHEI.Bridge.Edit-DEI1) */
    _shr_vlan_t vlan1;   /* VLAN-ID 1 (FHEI.Bridge.Edit-VID1) */
    uint8 pcp2;         /* PCP-2 (FHEI.Bridge.Edit-PCP2) */
    uint8 dei2;         /* DEI-2 (FHEI.Bridge.Edit-DEI2) */
    _shr_vlan_t vlan2;   /* VLAN-ID 2 (FHEI.Bridge.Edit-VID2) */
} soc_pkt_dnx_pph_fhei_bridge_extension_t;

typedef struct  {
    uint32 trap_qualifier;  /* Trap Qualifier(FHEI.Trap.CPU-Trap-Code-Qualifier) */
    _shr_gport_t trap_id;    /* Trap ID (FHEI.Trap.CPU-Trap-Code) */
} soc_pkt_dnx_pph_fhei_trap_extension_t;

/* pph fhei ip extension */
typedef struct  {
    uint32 ive_cmd; /* Ingress-VLAN-Edit-Command
                       (FHEI.IP.Ingress-VLAN-Edit-Command) */
    uint8 pri;      /* Priority (FHEI.IP.In-DSCP/In-TC) */
    uint8 in_ttl;   /* Time To Live (FHEI.IP.In-TTL) */
} soc_pkt_dnx_pph_fhei_ip_extension_t;
/* pph fhei mpls extension */
typedef struct  {
    uint32 tpid_profile;                /* TPID profile (FHEI.MPLS.TPID-Profile) */
    uint32 label;                       /* MPLS label (FHEI.MPLS.Label) */
    _shr_forwarding_type_t upper_layer_protocol; /* Protocol after MPLS
                                           (FHEI.MPLS.Upper-Layer-Protocol) */
    uint8 is_pipe_model;                /* If set, the model is pipe. Otherwise,
                                           it is uniform. (FHEI.MPLS.Model) */
    uint32 cw;                          /* Control Word (FHEI.MPLS.Control-Word) */
    uint32 label_cmd;                   /* MPLS Label Command
                                           (FHEI.MPLS.MPLS-Label-Command) */
    uint8 in_exp;                       /* n-EXP (FHEI.MPLS.In-EXP) */
    uint8 in_ttl;                       /* In-TTL (FHEI.MPLS.In-TTL) */
} soc_pkt_dnx_pph_fhei_mpls_extension_t;
typedef struct  {
    uint8 in_ttl;   /* FHEI TRILL(In-TTL) */
} soc_pkt_dnx_pph_fhei_trill_extension_t;


/* pph fhei extension */
typedef struct  {
    _shr_pkt_dnx_pph_fhei_type_t fhei_type; /* FHEI Type. */
    soc_pkt_dnx_pph_fhei_bridge_extension_t bridge; /* FHEI bridge Extension (PPH.FHEI
    soc                                   bridge Extension) */
    soc_pkt_dnx_pph_fhei_trap_extension_t trap; /* FHEI trap Extension (PPH.FHEI trap
    soc                                   Extension) */
    soc_pkt_dnx_pph_fhei_ip_extension_t ip; /* FHEI IP Extension (PPH.FHEI IP
    soc                                   Extension) */
    soc_pkt_dnx_pph_fhei_mpls_extension_t mpls; /* FHEI MPLS Extension (PPH.FHEI MPLS
    soc                                   Extension) */
    soc_pkt_dnx_pph_fhei_trill_extension_t trill; /* FHEI trill Extension (PPH.FHEI trill
                                           Extension) */
} soc_pkt_dnx_pph_fhei_extension_t;
/* pph fhei extension */
typedef struct  {
    uint32 fhei_size;                   /* If present, FHEI extension size in
                                           bytes (PPH.FHEI-Size). */
    _shr_forwarding_type_t forwarding_type; /* Forwarding header type
                                           (PPH.Forwarding-Code)) */
    uint32 forwarding_header_offset;    /* Distance in bytes from the end of DNX
                                           internal headers to the for-warding
                                           header (PPH.Forwarding-Header-Offset) */
    uint32 bypass_filter;               /* If set, filtering is bypassed
                                           (PPH.Packet-Is-Control)) */
    uint32 snoop_cmnd;                  /* Snoop command (bits 3:2)
                                           (PPH.Snoop-Cpu-Code) */
    uint32 vport_orientation;           /* VPort orientation
                                           (PPH.In-LIF-Orientation */
    uint8 unknown_address;              /* Unknown Address (PPH.Unknown-Address) */
    uint8 learn_allowed;                /* If set, learning is allowed
                                           (PPH.Learn-Allowed) */
    uint32 vswitch;                     /* Packet VSI (PPH.VSI-VRF). Set if
                                           packet is not IP routed. */
    uint32 vrf;                         /* Packet VRF (PPH.VSI-VRF). Set if
                                           packet is IP routed. */
    _shr_gport_t in_vport;               /* In-VPort (PPH.In-LIF /In-RIF). */
    soc_pkt_dnx_pph_fhei_extension_t fhei; /* FHEI Extension (PPH.FHEI-Extension). */
    soc_pkt_dnx_pph_eei_extension_t eei; /* EEI Extension (PPH.EEI-Extension). */
    soc_pkt_dnx_pph_learn_extension_t learn; /* Learn-Extension
                                           (PPH.Learn-Extension)). */
} soc_pkt_dnx_pph_t;
/* otsh */
typedef struct  {
    _shr_pkt_dnx_otsh_type_t otsh_type;  /* OAM-TS(Type). */
    _shr_pkt_dnx_otsh_oam_subtype_t oam_sub_type; /* OAM-TS(OAM-Sub-Type). Applies only
                                           when otsh_type is oam. */
    uint32 oam_up_mep;                  /* OAM-TS(MEP-Type). Applies only when
                                           otsh_type is oam. */
    uint32 tp_cmd;                      /* OAM-TS(TP-Cmd). Applies only when
                                           otsh_type is 1588. */
    uint8 ts_encap;                     /* OAM-TS(TS-Encaps). Applies only when
                                           otsh_type is 1588. */
    uint64 oam_ts_data;                 /* OAM-TS(OAM-TS-Data) */
    uint32 latency_flow_ID;             /* Latency flow ID generated by the PMF. */
    uint32 offset;                      /* OAM-TS(offset). Applies only when
                                           otsh_type is oam.. */
} soc_pkt_dnx_otsh_t;
   
typedef struct  {
    uint8 valid;                /* Set if the extension is present */
    _shr_gport_t src_sysport;    /* Source System Port (OTMH.Source-System-Port) */
} _shr_pkt_dnx_otmh_src_sysport_extension_t;

/* otmh vport extension */
typedef struct  {
    uint8 valid;            /* Set if the extension is present */
    _shr_gport_t out_vport;  /* Virtual port (OTMH.Out-LIF/CUD) */
} soc_pkt_dnx_otmh_vport_extension_t;
typedef struct {
    _shr_pkt_dnx_ftmh_action_type_t action_type; /* Action type (OTMH.TM-Action-Type) */
    _shr_color_t ftmh_dp;                /* Drop precedence (OTMH.DP) */
    uint8 is_mc_traffic;                /* AIndicate if the traffic is multicast
                                           (OTMH.System-Multicast) */
    uint32 prio;                        /* Traffic class (OTMH.Traffic-Class) */
    _shr_gport_t dst_port;               /* Destination local port
                                           (OTMH.Destination-Port) */
    _shr_pkt_dnx_otmh_src_sysport_extension_t src_sysport_ext; /* OTMH Source System Port Extension */
    soc_pkt_dnx_otmh_vport_extension_t out_vport_ext; /* OTMH Source System Port Extension */
} soc_pkt_dnx_otmh_t;
 

typedef struct  {
    uint8 data[_SHR_PKT_DNX_RAW_SIZE_MAX]; /* RAW Data */
    int len;                            /* Length of RAW Data */
} soc_pkt_dnx_raw_t;
/* dnx packet */
typedef struct  soc_pkt_dnx_s {
    _shr_pkt_dnx_type_t type;    /* DNX Header type */
    soc_pkt_dnx_ptch1_t ptch1;  /* PTCH-1 Header */
    soc_pkt_dnx_ptch2_t ptch2;  /* PTCH-2 Header */
    soc_pkt_dnx_itmh_t itmh;    /* ITMH Header */
    soc_pkt_dnx_ftmh_t ftmh;    /* FTMH Header */
    soc_pkt_dnx_pph_t pph;      /* PPH Header */
    soc_pkt_dnx_otsh_t otsh;    /* OAM-TS Header (OTSH) */
    soc_pkt_dnx_otmh_t otmh;    /* OTMH Header */
    soc_pkt_dnx_raw_t raw;      /* Raw Header */
} soc_pkt_dnx_t;

typedef struct soc_pkt_s{ /* the structure represents pkt_t */
    soc_pkt_dnx_t dnx_header_stack[_SHR_PKT_NOF_DNX_HEADERS]; /* DNX Header stack */
    uint8 dnx_header_count;             /* Number of DNX headers */
} soc_pkt_t;


#endif  /*_SOC_DPP_PKT_H*/
