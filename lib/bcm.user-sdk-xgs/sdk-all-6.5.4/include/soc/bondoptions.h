/*
 * $Id$
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File for SOC Bond Options Info structures and routines
 */

#ifndef _SOC_BONDOPTIONS_H
#define _SOC_BONDOPTIONS_H


typedef enum {

    /*  Feature Enables */
    socBondInfoFeatureNoL3,           /*  L3  */
    socBondInfoFeatureNoEcmp,           /*  Ecmp */
    socBondInfoFeatureNoAlpm,           /*  Alpm */
    socBondInfoFeatureNoL3Tunnel,       /*  L3 Tunnel */
    socBondInfoFeatureNoStaticNat,      /*  NAT */
    socBondInfoFeatureIpmcDisabled,   /*  IPMC */
    socBondInfoFeatureNoMim,            /*  MIM */
    socBondInfoFeatureNoMpls,           /*  MPLS */
    socBondInfoFeatureNoTrill,          /*  TRILL */
    socBondInfoFeatureNoVxlan,          /*  Vxlan */
    socBondInfoFeatureNoL2Gre,          /*  L2GRE */
    socBondInfoFeatureNoRiot,           /*  RIOT */
    socBondInfoFeatureNoEtherTm,        /*  Ether TM */
    socBondInfoFeatureNoFcoe,           /*  FCOE */
    socBondInfoFeatureNoTimeSync,       /*  TimeSync */
    socBondInfoFeatureNoHgProxyCoe,     /*  TD2+ COE */
    socBondInfoFeatureNoCoeVlanPause,   /*  COE Vlan Pause */
    socBondInfoFeatureNoCoeSubtag8100,  /*  COE can use 0x8100 */
    socBondInfoFeatureNoFpOam,          /*  FP-Based OAM */
    socBondInfoFeatureNoOsubCT,         /*  OS+CT support */
    socBondInfoFeatureNoCutThru,      /*  Cut-thru support */
    socBondInfoFeatureNoSat,            /*  SAT */
    socBondInfoFeatureNoFpRouting,      /*  IFP Based Routing */
    socBondInfoFeatureNoOam,            /*  OAM */
    socBondInfoFeatureNoLinkPhy,        /*  Link PHY */
    socBondInfoFeatureNoSubTagCoe,      /*  Subtag COE */
    socBondInfoFeatureNoPTP,            /*  PTP */
    socBondInfoFeatureNoServiceMeter,   /*  Service meter */
    socBondInfoFeatureNoAvs,            /*  AVS */
    socBondInfoFeatureNoOamTrueUpMep,   /*  OAM TRUE UPMEP       */
    socBondInfoFeatureNoEpRedirectV2,   /*  EP REDIRECT V2.0     */
    socBondInfoFeatureNoMultiLevelProt, /*  MULTI LEVEL PROT     */
    socBondInfoFeatureNoFpSat,          /*  FP-SAT (SAT Hooks)   */
    socBondInfoFeatureNoSegmentRouting, /*  Segment Routing      */
    socBondInfoFeatureNoCAUI10Support,  /*  CAUI10 Support       */
    socBondInfoFeatureHqos3Levels,    /*  HQOS 3 levels Support*/
    socBondInfoFeatureNoQCN,            /*  QCN                  */
    socBondInfoFeatureNoECN,            /*  ECN                  */
    socBondInfoFeatureNoDMVoq,          /*  DMVOQ                */
    socBondInfoFeatureNoDPVoq,          /*  DPVOQ                */


    /*  Table size updates */
    socBondInfoFeatureVrf1k,         /*  1K VRF */
    socBondInfoFeatureVrf2k,         /*  2K VRF */
    socBondInfoFeatureVrf4k,         /*  4K VRF */
    socBondInfoFeatureL3Entry1k,     /*  L3_ENTRY */
    socBondInfoFeatureL3Iif8k,       /*  8K IIF */
    socBondInfoFeatureL3Iif12k,      /*  12K IIF */
    socBondInfoFeatureUft128k,       /*  Half UFT - 128K */
    socBondInfoFeatureUft256k,       /*  Full UFT - 256K */
    socBondInfoFeatureIpmc4k,        /*  4K IPMC */
    socBondInfoFeatureIpmc8k,        /*  8K IPMC */
    socBondInfoFeatureMmu1Mb,        /*  1MB Buffers */
    socBondInfoFeatureMmu2Mb,        /*  2MB Buffers */
    socBondInfoFeatureMmu6Mb,        /*  6MB Buffers */
    socBondInfoFeatureMmu9Mb,        /*  9MB Buffers */
    socBondInfoFeatureMmu12Mb,       /*  12MB Buffers */
    socBondInfoFeatureIfpSliceHalf,  /*  IFP Half slice */
    socBondInfoFeatureEfp512HalfSlice,/* EFP Half Slice */
    socBondInfoFeatureVfp512HalfSlice,/* VFP Half Slice */
    socBondInfoFeatureMpls1k,         /* 1K MPLS Table*/
    socBondInfoFeatureMpls2k,         /* 2K MPLS Table*/
    socBondInfoFeatureL2Entry8k,      /* 8K L2_ENTRY Table*/
    socBondInfoFeatureL2Entry32k,     /* 32K L2_ENTRY Table*/
    socBondInfoFeatureIpVlanXlate1k,  /* 1K IP VLAN_XLATE Table*/
    socBondInfoFeatureIpVlanXlate4k,  /* 4K IP VLAN_XLATE Table*/
    socBondInfoFeatureL3NextHop1k,    /* 1K L3_NEXT_HOP Table*/
    socBondInfoFeatureL3NextHop2k,    /* 2K L3_NEXT_HOP Table*/
    socBondInfoFeatureL3DefIpDisable,/*  L3_DEF_IP Table*/
    socBondInfoFeatureL2mc4k,        /*  4K L2MC */
    socBondInfoFeatureSVP4k,         /*  4K SVP */
    socBondInfoFeatureL3HostMax4k,   /*  4K L3 Max Host */
    socBondInfoFeatureVrf16,         /*  16 VRFs */
    socBondInfoFeatureL3NHop8k,      /*  8k NHOPs */
    socBondInfoFeatureL3NHop32k,     /* 32k NHOPs */
    socBondInfoFeatureL3Intf8k,      /*  8k L3_INTF */
    socBondInfoFeatureL3Intf12k,     /* 12k L3_INTF */

    /*  iProc Related */
    socBondInfoFeatureUc0Active,     /*  UC0 Active */
    socBondInfoFeatureUc1Active,     /*  UC1 Active */
    socBondInfoFeatureSramPwrDwn,    /*  SRAM Powered Down */
    socBondInfoFeatureSpiCodeSize32KB, /*  SPI - 32KB */
    socBondInfoFeatureSpiCodeSize64KB, /*  SPI - 64KB */
    socBondInfoFeatureSpiCodeSize128KB,/*  SPI - 128KB */

    /* Chip Specific */
    socBondInfoFeatureFamilyApache,
    socBondInfoFeatureFamilyMaverick,
    socBondInfoFeatureFamilyFirebolt5,
    socBondInfoFeatureApacheCoSProfile0,
    socBondInfoFeatureApacheCoSProfile1,
    socBondInfoFeatureApacheCoSProfile2,
    socBondInfoFeatureApachePkg42p5mm,
    socBondInfoFeatureApachePkg50mmTDpCompat,
    socBondInfoFeatureApachePkg50mmTD2pCompat,
    socBondInfoFeatureMetroliteL1SyncE,
    socBondInfoFeatureMetroliteTelecomDPLL,

    socBondInfoFeatureCount /* Alyways Last */
} soc_bond_info_features_t;


/*
 * Bond Options structure.
 *
 * New elements that cannot be fit into the 'features' definition should be
 * defined as spearate elements in the structure.
 */
typedef struct soc_bond_info_s {
    uint16 dev_id;                  /*  16b:   DEVICE_ID               */
    uint16 rev_id;                  /*   8b:   REV_ID                  */

    uint32 device_skew;             /*   4b:   DEVICE_SKEW             */
    uint32 max_core_freq;           /*   3b:   MAX_CORE_FREQUENCY      */
    uint32 default_core_freq;       /*   3b:   DEFAULT_CORE_FREQUENCY  */

    uint32 defip_cam_map;           /*   8b:   DEFIP_CAM_ENABLE        */
    uint32 ifp_slice_map;           /*  12b:   IFP_SLICE_ENABLE        */
    uint32 efp_slice_map;           /*   4b:   EFP_SLICE_ENABLE        */
    uint32 vfp_slice_map;           /*   4b:   VFP_SLICE_ENABLE        */
    uint32 tsc_force_hg;            /*   18b:  FORCE_HG                */
    uint32 tsc_enable;              /*   18b:  TSC_ENABLE              */
    uint32 clmac_map;               /*   6b:   CLMAC_ENABLE            */
    uint32 xlmac_map;               /*  18b:   XLMAC_ENABLE            */

    /*  Bitmap of bondoption features */
    SHR_BITDCL features[_SHR_BITDCLSIZE(socBondInfoFeatureCount)];
} soc_bond_info_t;

extern soc_bond_info_t *soc_bond_info[SOC_MAX_NUM_DEVICES];

#define SOC_BOND_INFO(unit) (soc_bond_info[unit])

/* Bond Info Features cache  - Access macros */
#define SOC_BOND_INFO_FEATURE_GET(unit, feat)   \
        SHR_BITGET(SOC_BOND_INFO(unit)->features, feat)
#define SOC_BOND_INFO_FEATURE_SET(unit, feat)   \
        SHR_BITSET(SOC_BOND_INFO(unit)->features, feat)
#define SOC_BOND_INFO_FEATURE_CLEAR(unit, feat) \
        SHR_BITCLR(SOC_BOND_INFO(unit)->features, feat)

#define SOC_BOND_INFO_GET(unit, attrib)     (SOC_BOND_INFO(unit)->attrib)
#define SOC_BOND_INFO_SET(unit, attrib)     (SOC_BOND_INFO(unit)->attrib)

#define SOC_BOND_INFO_REG_MAX       16
#define SOC_BOND_INFO_REG_WIDTH     32
#define SOC_BOND_INFO_MAX_BITS      (SOC_BOND_INFO_REG_MAX * SOC_BOND_INFO_REG_WIDTH)

#define SOC_BOND_OPTIONS_COPY(_toval_, __cast__, _fromval_, _sbit_, _ebit_)  \
    do {                                                                     \
        {                                                                    \
            uint32 bitrange_val = 0;                                         \
            SHR_BITCOPY_RANGE(&bitrange_val, 0,                              \
                              (CONST SHR_BITDCL *)(_fromval_),               \
                              (_sbit_), ((_ebit_) - (_sbit_) + 1));          \
            _toval_ = (__cast__)bitrange_val;                                \
        }                                                                    \
    } while (0)


extern int soc_bond_info_registers[SOC_BOND_INFO_REG_MAX];

#endif /* !_SOC_BONDOPTIONS_H */
