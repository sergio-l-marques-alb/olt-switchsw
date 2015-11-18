/*
 * $Id: drv.h,v 1.187 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DPP_DRV_H
#define _SOC_DPP_DRV_H

#include <sal/types.h>

#include <shared/cyclic_buffer.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>

#include <soc/dcmn/dcmn_defs.h>

#include <soc/dpp/fabric.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/Petra/petra_drv.h>
#include <soc/dpp/JER/jer_drv.h>



#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_init.h>
#endif
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>

#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>

#define SOC_DPP_MAX_NOF_CHANNELS    (256)
#define SOC_DPP_DRV_TDM_OPT_SIZE    (78)

/* Reset flags */
#define SOC_DPP_RESET_ACTION_IN_RESET                               SOC_DCMN_RESET_ACTION_IN_RESET
#define SOC_DPP_RESET_ACTION_OUT_RESET                              SOC_DCMN_RESET_ACTION_OUT_RESET
#define SOC_DPP_RESET_ACTION_INOUT_RESET                            SOC_DCMN_RESET_ACTION_INOUT_RESET

#define SOC_DPP_RESET_MODE_HARD_RESET                               SOC_DCMN_RESET_MODE_HARD_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_RESET                             SOC_DCMN_RESET_MODE_BLOCKS_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET                        SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET                SOC_DCMN_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET                 SOC_DCMN_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET
#define SOC_DPP_RESET_MODE_INIT_RESET                               SOC_DCMN_RESET_MODE_INIT_RESET
#define SOC_DPP_RESET_MODE_REG_ACCESS                               SOC_DCMN_RESET_MODE_REG_ACCESS
#define SOC_DPP_RESET_MODE_ENABLE_TRAFFIC                           SOC_DCMN_RESET_MODE_ENABLE_TRAFFIC
#define SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET             SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET     SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET      SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET
#define SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT                 SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT


#define SOC_DPP_MAX_PADDING_SIZE 0x7f

/* 0-511 pbmp 512-1023 internal 1024-1535 interface */ 

/* Different port types have different ranges                   */
/* each port range is sized at SOC_DPP_PORT_RANGE_NUM_ENTRIES   */
/* pbmp ports start at 0.                                       */
#define SOC_DPP_PORT_RANGE_NUM_ENTRIES 512 /* 288 required for ARAD */

/* Internal ports */
#ifdef  BCM_88675_A0
#define SOC_DPP_PORT_INTERNAL_START           548
#define SOC_DPP_PORT_INTERNAL_OLP(core)             (SOC_DPP_PORT_INTERNAL_START + core)
#define SOC_DPP_PORT_INTERNAL_OAMP(core)            (SOC_DPP_PORT_INTERNAL_START + 1*SOC_DPP_DEFS_MAX(NOF_CORES) + core)
#define SOC_DPP_PORT_INTERNAL_ERP(core)             (SOC_DPP_PORT_INTERNAL_START + 2*SOC_DPP_DEFS_MAX(NOF_CORES) + core)
#elif defined BCM_88375_A0
#define SOC_DPP_PORT_INTERNAL_START           548
#define SOC_DPP_PORT_INTERNAL_OLP(core)             (SOC_DPP_PORT_INTERNAL_START + core)
#define SOC_DPP_PORT_INTERNAL_OAMP(core)            (SOC_DPP_PORT_INTERNAL_START + 1*SOC_DPP_DEFS_MAX(NOF_CORES) + core)
#define SOC_DPP_PORT_INTERNAL_ERP(core)             (SOC_DPP_PORT_INTERNAL_START + 2*SOC_DPP_DEFS_MAX(NOF_CORES) + core)
#else
#define SOC_DPP_PORT_INTERNAL_START           324
#define SOC_DPP_PORT_INTERNAL_OLP(core)       (SOC_DPP_PORT_INTERNAL_START + 0)
#define SOC_DPP_PORT_INTERNAL_OAMP(core)      (SOC_DPP_PORT_INTERNAL_START + 1)
#define SOC_DPP_PORT_INTERNAL_ERP(core)       (SOC_DPP_PORT_INTERNAL_START + 2)
#endif

#define SOC_DPP_PORT_INTERNAL_END                   (SOC_DPP_PORT_INTERNAL_START + SOC_DPP_PORT_RANGE_NUM_ENTRIES - 1)

/* Interface ports */
#define SOC_DPP_PORT_INTERFACE_START 1024
#define SOC_DPP_PORT_INTERFACE_END   (SOC_DPP_PORT_INTERFACE_START + (SOC_DPP_PORT_RANGE_NUM_ENTRIES) - 1)

/* Number of MACT limit table ranges to be mapped */
#define SOC_DPP_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES    (2)

/* Number of FAP id limit to be mapped to destination */
#define SOC_DPP_MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT     (2)

/* encap-type are in bits 22-23 (two bits, below gport-type)*/
#define _DPP_ENCAP_ID_SHIFT   (_SHR_GPORT_TYPE_SHIFT-4)

/* bit 21 is for working path in 1 + 1 protection*/
#define _DPP_ENCAP_ID_PLUS1_WORKING_SHIFT   (_DPP_ENCAP_ID_SHIFT-1)

/* value for lif encap */
#define _DPP_ENCAP_ID_LIF_VAL   (2)

/* value for EEP encap */
#define _DPP_ENCAP_ID_EEP_VAL   (0)

/* value for FORWARD encap */
#define _DPP_ENCAP_ID_FORWARD_VAL   (1)

/* value for VC encap */
#define _DPP_ENCAP_ID_MC_VAL   (3)

/* value for push profile. Relevant only for MPLS_PORT gport */
#define _DPP_ENCAP_ID_PUSH_PROFILE   (4)

/* Trill port encap values (aka sub type) */
#define _DPP_ENCAP_ID_TRILL_OUTLIF   (2)

/* Defines for Asymmetric LIF gport encoding
   Bits 20-21 define Ingress-only or Egress-only LIFs
   Bit 22-23 value 01 define FEC only LIFs */
#define _DPP_ENCAP_ID_NO_EGRESS_VAL         (0x1)
#define _DPP_ENCAP_ID_NO_INGRESS_VAL        (0x2)
#define _DPP_ENCAP_ID_EXCLUSIVE_LIF_SHIFT   (_DPP_ENCAP_ID_SHIFT - 2)

/* defines out-LIF */
/* defines to use encap_id/gport to include out-LIF */

#define _DPP_ENCAP_ID_SIGNATURE   ((1|2)<<_DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_LIF_TYPE   (_DPP_ENCAP_ID_LIF_VAL<<_DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_EEP_TYPE   (_DPP_ENCAP_ID_EEP_VAL<<_DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_MC_TYPE   (_DPP_ENCAP_ID_MC_VAL<<_DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_MC_WORKING   (1<<_DPP_ENCAP_ID_PLUS1_WORKING_SHIFT)
#define _DPP_ENCAP_ID_FORWARD_GROUP_TYPE        (_DPP_ENCAP_ID_FORWARD_VAL << _DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_PUSH_PROFILE_TYPE        (_DPP_ENCAP_ID_PUSH_PROFILE << _DPP_ENCAP_ID_SHIFT)
#define _DPP_ENCAP_ID_TRILL_OUTLIF_TYPE        (_DPP_ENCAP_ID_TRILL_OUTLIF << _DPP_ENCAP_ID_SHIFT)

#define SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT  (256)
#define FABRIC_LOGICAL_PORT_BASE(unit)            (SOC_INFO(unit).fabric_logical_port_base)

/* Encap ID values at the exclusive LIF bits location */
#define _DPP_ENCAP_ID_LIF_INGRESS_ONLY    (_DPP_ENCAP_ID_NO_EGRESS_VAL << _DPP_ENCAP_ID_EXCLUSIVE_LIF_SHIFT)
#define _DPP_ENCAP_ID_LIF_EGRESS_ONLY     (_DPP_ENCAP_ID_NO_INGRESS_VAL << _DPP_ENCAP_ID_EXCLUSIVE_LIF_SHIFT)

#define SOC_DPP_NUM_OF_ROUTES (2048)
#define SOC_DPP_MAX_NUM_OF_ROUTE_GROUPS           (32)

#define _DPP_MAX_NUM_OF_LIFS           (SOC_DPP_DEFS_MAX(NOF_LOCAL_LIFS) - 2) /* highest lif (16*1024-1) is used only for MiM */

#define SOC_DPP_NOF_DIRECT_PORTS  (32)

#define SOC_DPP_FIRST_DIRECT_PORT(unit)   (FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, nof_fabric_links))

#define SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED     (0)
#define SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_DEDICATED  (1)

#define SOC_DPP_INVALID_NOF_REMOTE_CORES            (0xffff)
#define SOC_DPP_INVALID_MAPPING_MODE                (0xffff)

#define SOC_DPP_MAX_RIF_ID                          (32*1024 - 1)

#define SOC_DPP_PHY_GPORT_LANE_VALID(unit, gport)                                                                                                               \
                                        (                                                                                                                       \
                                            BCM_PHY_GPORT_IS_LANE(gport)                                                                                    &&  \
                                            SOC_PORT_VALID(unit, BCM_PHY_GPORT_LANE_PORT_PORT_GET(port))                                                    &&  \
                                            SOC_INFO(unit).port_num_lanes!=NULL                                                                            &&  \
                                            SOC_INFO(unit).port_num_lanes[BCM_PHY_GPORT_LANE_PORT_PORT_GET(port)]  > BCM_PHY_GPORT_LANE_PORT_LANE_GET(port)    \
                                        )

#define SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) ((port) - FABRIC_LOGICAL_PORT_BASE(unit))
#define SOC_DPP_FABRIC_LINK_TO_PORT(unit, port) ((port) + FABRIC_LOGICAL_PORT_BASE(unit))

/* Encap ID details */
/* 
   ENCAP ID:
 
   31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01
   ____________________________________________________________________________________________
  |                 |           |  |              |                                            |  
  |      Gport      | Sub type  | P|              |              Multicast-ID(16)              |  PS bit: 1 - Primary
  |      type       | 0  0  1  1| S| 0  0  0  0  0|              (1+1 Protection)              |          0 - Secondary
  |_________________|___________|__|______________|____________________________________________|
  |                 |           |                    |                                         |  
  |      Gport      | Sub type  |                    |              FEC Pointer(15)            |  
  |      type       | 0  0  0  0| 0  0  0  0  0  0  0|              (1:1 Protection)           |  
  |_________________|___________|____________________|_________________________________________|
  |                 |           |                    |                                         |  
  |      Gport      | Sub type  |                    |              FEC Pointer(15)            |  Forwarding Group
  |      type       | 0  0  0  1| 0  0  0  0  0  0  0|                                         |  
  |_________________|___________|____________________|_________________________________________|  
  |                 |           |     |           |                                            |  Exclude bits:
  |      Gport      | Sub type  | Exc |           |                 LIF-ID(16)                 |  bit 21: 1 - No Ingress 
  |      type       | 0  0  1  0|     | 0  0  0  0|              (No Protection)               |  bit 20: 1 - No Egress
  |_________________|___________|_____|___________|____________________________________________|
  |                 |           |                                                   |          |
  |      Gport      | Sub type  |                                                   |   PUSH   | 
  |  type MPLS      | 0  1  0  0|  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|PROFILE(3)|
  |_________________|___________|___________________________________________________|__________|
                                                  | <---------        LIF ID       ----------> |
                                | <---------------------------       LIF Info      ----------> |
                    | <---------------------------------------       Encap ID      ----------> |
  | <---------------------------------------------------------     VLAN Port ID    ----------> |
 
*/ 

/* Encap valid is either LIF or Multicast-ID */
#define SOC_DPP_IS_VALID_ENCAP_ID(_encap_id) \
    (((_encap_id) & _DPP_ENCAP_ID_SIGNATURE) != 0)

/* Get the encap type - LIF/Multicast-ID/FEC */
#define SOC_DPP_ENCAP_TYPE_GET(_encap_id) \
    (((_encap_id) & _DPP_ENCAP_ID_SIGNATURE) >> _DPP_ENCAP_ID_SHIFT)

/* Set an encap id to a given symmetric LIF Info */
#define SOC_DPP_ENCAP_ID_FROM_LIF(_lif) \
    (_DPP_ENCAP_ID_LIF_TYPE | (_lif))

/* Set an encap id to a given fec */
#define SOC_DPP_ENCAP_ID_FROM_FORWARD_GROUP(_fec) \
    (_DPP_ENCAP_ID_FORWARD_GROUP_TYPE | (_fec))

/* Set an encap id to a given push profile */
#define SOC_DPP_ENCAP_ID_FROM_PUSH_PROFILE(_push_profile) \
    (_DPP_ENCAP_ID_PUSH_PROFILE_TYPE | (_push_profile))

/* Set an encap id to a given Ingress only LIF */
#define SOC_DPP_ENCAP_ID_INGRESS_ONLY_FROM_LIF(_lif) \
    (SOC_DPP_ENCAP_ID_FROM_LIF(_lif) | _DPP_ENCAP_ID_LIF_INGRESS_ONLY)

/* Set an encap id to a given Egress only LIF */
#define SOC_DPP_ENCAP_ID_EGRESS_ONLY_FROM_LIF(_lif) \
    (SOC_DPP_ENCAP_ID_FROM_LIF(_lif) | _DPP_ENCAP_ID_LIF_EGRESS_ONLY)

/* Set an encap id to a given MC Group and MC protection state */
#define SOC_DPP_ENCAP_ID_FROM_MC(__mc_id, working) \
    (_DPP_ENCAP_ID_MC_TYPE | ((working) << _DPP_ENCAP_ID_PLUS1_WORKING_SHIFT) | (__mc_id))

/* Set an encap id to a FEC pointer */
#define SOC_DPP_ENCAP_ID_FROM_EEP(_eep) \
    (_DPP_ENCAP_ID_EEP_TYPE | (_eep))


/* Get a LIF Info from a LIF type encap ID */
#define SOC_DPP_LIF_FROM_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~_DPP_ENCAP_ID_SIGNATURE)

/* Get a LIF ID from a LIF type encap ID */
#define SOC_DPP_LIF_ID_FROM_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~(_DPP_ENCAP_ID_SIGNATURE | _DPP_ENCAP_ID_LIF_INGRESS_ONLY | _DPP_ENCAP_ID_LIF_EGRESS_ONLY))

/* Get FEC pointer from a forward group type encap ID */
#define SOC_DPP_FORWARD_GROUP_FROM_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~_DPP_ENCAP_ID_SIGNATURE)

/* Get a MC Group ID from a MC type encap ID */
#define SOC_DPP_MC_GROUP_FROM_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~(_DPP_ENCAP_ID_SIGNATURE|_DPP_ENCAP_ID_MC_WORKING))

/* Get a MC Protection state from a MC type encap ID */
#define SOC_DPP_MC_WORKING_FROM_ENCAP_ID(_encap_id) \
    (((_encap_id) & (_DPP_ENCAP_ID_MC_WORKING)) >> _DPP_ENCAP_ID_PLUS1_WORKING_SHIFT)

/* Get a push profile from a push profile type encap ID */
#define SOC_DPP_PUSH_PROFILE_FROM_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~_DPP_ENCAP_ID_PUSH_PROFILE_TYPE)

/* Check whether the an encap ID is for Ingress only */
#define SOC_DPP_ENCAP_ID_IS_INGRESS_ONLY(_encap_id) \
    ((_DPP_ENCAP_ID_LIF_INGRESS_ONLY & (_encap_id)) != 0)

/* Check whether the an encap ID is for Egress only */
#define SOC_DPP_ENCAP_ID_IS_EGRESS_ONLY(_encap_id) \
    ((_DPP_ENCAP_ID_LIF_EGRESS_ONLY & (_encap_id)) != 0)

/* Check whether the an encap ID is for FEC only */
#define SOC_DPP_ENCAP_ID_IS_FORWARD_GROUP(_encap_id) \
    (SOC_DPP_ENCAP_TYPE_GET(_encap_id) == _DPP_ENCAP_ID_FORWARD_VAL)

/* Check whether the an encap ID is of push profile sub-type */
#define SOC_DPP_ENCAP_ID_IS_PUSH_PROFILE(_encap_id) \
    ((_DPP_ENCAP_ID_PUSH_PROFILE_TYPE & (_encap_id)) != 0)


/* Trill Gport Encap ID details */
/* 
   ENCAP ID:
 
   31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00 
   _______________________________________________________________________________________________ 
  |                 |           |            |                                                    |
  |      Gport      | Sub type  |            |                         out-lif                    |
  |  type TRILL     | 0  0  1  0|  0  0  0  0|                                                    |
  |_________________|___________|____________|____________________________________________________|
                    | <---------------------------------------       Encap ID         ----------> |
*/ 

/* Set an encap id (aka sub-type) to a Trill out LIF (aka eep) */
#define SOC_DPP_ENCAP_ID_FROM_TRILL_OUTLIF(_eep) \
     (_DPP_ENCAP_ID_TRILL_OUTLIF_TYPE | (_eep))

/* Get an outlif from a trill outlif type encap ID */
#define SOC_DPP_TRILL_OUTLIF_FROM_ENCAP_ID(_encap_id) \
     ((_encap_id) & ~_DPP_ENCAP_ID_TRILL_OUTLIF_TYPE)

/* Check whether the encap ID is of Trill outlif sub-type */
#define SOC_DPP_ENCAP_ID_IS_TRILL_OUTLIF(_encap_id) \
    ((_DPP_ENCAP_ID_TRILL_OUTLIF_TYPE & (_encap_id)) != 0)

/* Get a LIF Info from a trill outlif type encap ID */
#define SOC_DPP_LIF_FROM_TRILL_OUTLIF_ENCAP_ID(_encap_id) \
    ((_encap_id) & ~_DPP_ENCAP_ID_TRILL_OUTLIF_TYPE)

/*
 * Definitions of various SOC properties
 */
#define SOC_DPP_FC_CAL_MODE_DISABLE                             0
#define SOC_DPP_FC_CAL_MODE_RX_ENABLE                           0x1
#define SOC_DPP_FC_CAL_MODE_TX_ENABLE                           0x2

#define SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE          0
#define SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE1                 1 /* calender 0 */
#define SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE2                 2 /* calender 0, 16, ... */

#define SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_DISABLE               0
#define SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_MASK                  0xFF

#define SOC_DPP_FC_INBAND_INTLKN_CHANNEL_MUB_DISABLE            0
#define SOC_DPP_FC_INBAND_INTLKN_CHANNEL_MUB_MASK               0xFF

#define SOC_DPP_FABRIC_TDM_PRIORITY_NONE                        (-1) /* system does not support TDM */

/*
 * dpp tm config declaration
 */

#define SOC_DPP_MAX_INTERLAKEN_PORTS                            SOC_DPP_DEFS_MAX(NOF_INTERLAKEN_PORTS)
#define SOC_DPP_MAX_CAUI_PORTS                                  SOC_DPP_DEFS_MAX(NOF_CAUI_PORTS)
#define SOC_DPP_MAX_OOB_PORTS                                   2
#define SOC_DPP_NOF_LINKS                                      36 /* for ARAD only */
#define SOC_DPP_NOF_INSTANCES_MAC                               9 /* for ARAD only */

#define SOC_PETRAB_MULTICAST_TABLE_MODE   (32*1024)
#define ARAD_MULTICAST_TABLE_MODE         ARAD_MULT_NOF_MULTICAST_GROUPS
#define JER_MULT_NOF_MULTICAST_GROUPS     (128*1024)

#define DPP_MC_ALLOW_DUPLICATES_MODE       1  /* allow duplicate replications in multicast groups */
#define DPP_MC_EGR_17B_CUDS_127_PORTS_MODE 4  /* Arad+ encoding mode */
#define DPP_MC_CUD_EXTENSION_MODE          8  /* CUD extension node */
#define DPP_MC_EGR_CORE_FDA_MODE           16 /* FDA hardware controls whether each egress group core is replicated to */
#define DPP_MC_EGR_CORE_MESH_MODE          32 /* Mesh MC hardware controls whether each egress group core is replicated to */

/* Macros for multicast CUD (outlif) and destination encoding */

#define ARAD_MC_DEST_ENCODING_0 0 /* supported on all Arads, 16bit CUDs, 96K flows, 32K sysports, 32K MCIDs */
#define ARAD_MC_DEST_ENCODING_1 1 /* supported on Arad+, 17bit CUDs, 64K-1 flows, 32K sysports */
#define ARAD_MC_DEST_ENCODING_2 2 /* supported on Arad+, 18bit CUDs, 32K-1 flows, 16K sysports */
#define ARAD_MC_DEST_ENCODING_3 3 /* supported on Arad+, 17bit CUDs, 96K flows */

/* maximum number of queue that the encoding supports (also limited by no more than 96K queues) */
#define ARAD_MC_DEST_ENCODING_0_MAX_QUEUE 0x17fff
#define ARAD_MC_DEST_ENCODING_1_MAX_QUEUE 0xfffe /* 0xffff may cause no replication */
#define ARAD_MC_DEST_ENCODING_2_MAX_QUEUE 0x7ffe /* 0x7fff may cause no replication */
#define ARAD_MC_DEST_ENCODING_3_MAX_QUEUE 0x17fff

/* maximum CUD value that the ingress encoding supports (also limited by no more than 96K queues) */
#define ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD  0xffff
#define ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD 0x1ffff
#define ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD 0x3ffff
#define ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD 0x1ffff
#define SOC_PETRAB_MAX_CUD ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD
/* maximum CUD value that the egress encoding supports (also limited by no more than 96K queues) */
#define ARAD_MC_16B_MAX_EGR_CUD  0xffff
#define ARAD_MC_17B_MAX_EGR_CUD 0x1ffff

#define JER_MC_MAX_EGR_CUD  0x3ffff
#define JER_MC_MAX_ING_CUD  0x7ffff

/* number of system ports that the encoding supports */
#define ARAD_MC_DEST_ENCODING_0_NOF_SYSPORT 0x8000
#define ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT 0x8000
#define ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT 0x4000
#define ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT 0

#define ARAD_MC_MAX_BITMAPS 8191

/* base queue to module x port mapping modes: */
#define VOQ_MAPPING_DIRECT   0
#define VOQ_MAPPING_INDIRECT 1

/* Reserve Unique Device ID for TDM settings. All FAP-IDs must be different than this ID */
#define DPP_TDM_FAP_DEVICE_ID_UNIQUE      (0x80)

/* supported values for the action_type_source_mode field */
#define ACTION_TYPE_FROM_QUEUE_SIGNATURE 0   /* get action type from queue signature (default) */
#define ACTION_TYPE_FROM_FORWARDING_ACTION 1 /* get action type from packet header */

#define ARAD_IS_VOQ_MAPPING_DIRECT(unit) (SOC_DPP_CONFIG(unit)->arad->voq_mapping_mode == VOQ_MAPPING_DIRECT)
#define ARAD_IS_VOQ_MAPPING_INDIRECT(unit) (SOC_DPP_CONFIG(unit)->arad->voq_mapping_mode != VOQ_MAPPING_DIRECT)

#define IS_ACTION_TYPE_FROM_QUEUE_SIGNATURE(unit) (SOC_DPP_CONFIG(unit)->arad->action_type_source_mode == ACTION_TYPE_FROM_QUEUE_SIGNATURE)

/* VLAN Translation */
#define SOC_DPP_VLAN_TRANSLATE_NORMAL_MODE_NORMAL   (0)
#define SOC_DPP_VLAN_TRANSLATE_NORMAL_MODE_ADVANCED (1)

#define SOC_DPP_IS_VLAN_TRANSLATE_MODE_NORMAL(unit)    (SOC_DPP_CONFIG(unit)->pp.vlan_translate_mode == SOC_DPP_VLAN_TRANSLATE_NORMAL_MODE_NORMAL)
#define SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)  (SOC_DPP_CONFIG(unit)->pp.vlan_translate_mode == SOC_DPP_VLAN_TRANSLATE_NORMAL_MODE_ADVANCED)

#define SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit)             (SOC_DPP_CONFIG(unit)->pp.nof_ive_action_ids)
#define SOC_DPP_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit)    (SOC_DPP_CONFIG(unit)->pp.nof_ive_reserved_action_ids)

#define SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS(unit)         (SOC_DPP_CONFIG(unit)->pp.nof_eve_action_mappings)
#define SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit)              (SOC_DPP_CONFIG(unit)->pp.nof_eve_action_ids)
#define SOC_DPP_NOF_EGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit)     (SOC_DPP_CONFIG(unit)->pp.nof_eve_reserved_action_ids)

/* Protection */
#define SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit)                 (SOC_DPP_JER_CONFIG(unit)->pp.protection_ingress_coupled_mode)
#define SOC_DPP_IS_PROTECTION_EGRESS_COUPLED(unit)                  (SOC_DPP_JER_CONFIG(unit)->pp.protection_egress_coupled_mode)
#define SOC_DPP_IS_PROTECTION_FEC_ACCELERATED_REROUTE_MODE(unit)    (SOC_DPP_JER_CONFIG(unit)->pp.protection_fec_accelerated_reroute_mode)

#define SOC_DPP_IS_SYS_RSRC_MGMT_ADVANCED(unit)  (SOC_DPP_CONFIG(unit)->pp.sys_rsrc_mgmt == 1)


/* supported values for the credit_watchdog_mode field, must be the same as in include/bcm/fabric.h */
#define CREDIT_WATCHDOG_UNINITIALIZED 0 /* BCM_FABRIC_WATCHDOG_QUEUE_DISABLE */
/* BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL the default mode supporting both fsm and aging (queue deletion) */
#define CREDIT_WATCHDOG_NORMAL_MODE 1
/* BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE support aggressive (small)
   watchdog status message thresholds and not support aging (queue deletion) */
#define CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE 2
/* BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE Arad+ mode whic common FSM time for the chip */
#define CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE 3

#define IS_CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE(unit) (SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode == CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE)
#define IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit) (SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode >= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE)
#define IS_CREDIT_WATCHDOG_UNINITIALIZED(unit) (SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode == CREDIT_WATCHDOG_UNINITIALIZED)

#define GET_CREDIT_WATCHDOG_MODE_BASE(mode) ((mode) < CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE ? (mode) : CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE)
#define GET_CREDIT_WATCHDOG_MODE(unit) GET_CREDIT_WATCHDOG_MODE_BASE(SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode)
#define GET_CREDIT_WATCHDOG_MODE_EXACT(unit) (SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode)
#define IS_CREDIT_WATCHDOG_MODE(unit, mode) (GET_CREDIT_WATCHDOG_MODE_BASE(mode) == GET_CREDIT_WATCHDOG_MODE_BASE(SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode))
#define SET_CREDIT_WATCHDOG_MODE(unit, mode) do {SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode = (mode);} while (0)

#define ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS 2
#define ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS (1000000 * ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS)
#define ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP 12
#define ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES 4
#define ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS (ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS >> ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES)

/* Using this number for the start and end of the queue scan range in arad_itm_cr_wd_set/get,
 * marks to update the delete time exponents for all profiles, in the common FSM mode */
#define ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP ((uint32)(-2))

#define DPP_VARIOUS_BM_FORCE_MBIST_TEST 1

/* VLAN match DB mode */
#define SOC_DPP_VLAN_DB_MODE_DEFAULT         (0)
#define SOC_DPP_VLAN_DB_MODE_FULL_DB         (1)
#define SOC_DPP_VLAN_DB_MODE_PCP             (3)

/* IP Tunnel termination bitmap */
/* IP tunnel */
#define SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT     (1) /* IP tunnel without L2 above DIP only */
#define SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP     (2) /* IP tunnel without L2 above (DIP,SIP) */
#define SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL (32)    /* IP tunnel with {dip, sip, port property, next protocol} as key. */
#define SOC_DPP_IP_TUNNEL_TERM_DB_TUNNEL_MASK (35) /* both */

/* L2oIP enable*/
#define SOC_DPP_IP_TUNNEL_TERM_DB_NVGRE       (4)
#define SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN       (8)
#define SOC_DPP_IP_TUNNEL_TERM_DB_ETHER       (16) /* EoIP */

/* L2oIP lookup mode */
#define SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_DISABLE           (0)
#define SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_SEPERATED (1)
#define SOC_DPP_IP_TUNNEL_L2_LKUP_MODE_SIP_DIP_JOIN      (2)

#define SOC_DPP_GUARANTEED_Q_RESOURCE_MEMORY  0 /* memory amount will be guaranteed. This leads to waste of resources due to buffer fragmentation */
#define SOC_DPP_GUARANTEED_Q_RESOURCE_BUFFERS 1 /* number of buffers (DBs) will be guaranteed. */
#define SOC_DPP_GUARANTEED_Q_RESOURCE_BDS     2 /* number of buffer descriptors (BDs) will be guaranteed. This is the recommended mode */

#define ARAD_PLUS_MIN_ALPHA -7
#define ARAD_PLUS_MAX_ALPHA 7
#define ARAD_PLUS_ALPHA_NOT_SUPPORTED 255

#define ARAD_PLUS_CREDIT_VALUE_MODES_PER_WORD (sizeof(uint32) * 8)
#define ARAD_PLUS_CREDIT_VALUE_MODE_WORDS (ARAD_NOF_FAPS_IN_SYSTEM / ARAD_PLUS_CREDIT_VALUE_MODES_PER_WORD)

/* L3 source bind mode */
#define SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE  (0)
#define SOC_DPP_L3_SOURCE_BIND_MODE_IPV4     (1)
#define SOC_DPP_L3_SOURCE_BIND_MODE_IPV6     (2)
#define SOC_DPP_L3_SOURCE_BIND_MODE_IP       (3) /* Both IPv4 and IPv6 */
#define SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_DISABLE  (0)
#define SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV4     (1)
#define SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV6     (2)
#define SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IP       (3) /* Both IPv4 and IPv6 subnet */
#define SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_DOWN      (0x1)
#define SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_UP        (0x2)
#define SOC_DPP_L3_SRC_BIND_FOR_ARP_RELAY_BOTH      (0x3)
#define SOC_DPP_L3_SRC_BIND_ARP_RELAY_ENABLE(unit) (SOC_DPP_CONFIG(unit)->pp.l3_src_bind_arp_relay)
#define SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_ENABLE(unit)\
           ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV4) || \
            (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IP))      
#define SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit)\
           ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV6) || \
            (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IP)) 
#define SOC_DPP_L3_SRC_BIND_IPV4_HOST_ENABLE(unit)\
           ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV4) || \
            (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IP))      
#define SOC_DPP_L3_SRC_BIND_IPV6_HOST_ENABLE(unit)\
           ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV6) || \
            (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IP)) 
#define SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit) \
           (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_ENABLE(unit) || SOC_DPP_L3_SRC_BIND_IPV4_HOST_ENABLE(unit))
#define SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit) \
           (SOC_DPP_L3_SRC_BIND_IPV6_SUBNET_ENABLE(unit) || SOC_DPP_L3_SRC_BIND_IPV6_HOST_ENABLE(unit))
#define SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_OR_ARP_ENABLE(unit) \
           (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_ENABLE(unit) || SOC_DPP_L3_SRC_BIND_ARP_RELAY_ENABLE(unit))
#define SOC_DPP_L3_SOURCE_BIND_DISABLE(unit) \
           ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) && \
             (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_DISABLE) && \
             !SOC_DPP_L3_SRC_BIND_ARP_RELAY_ENABLE(unit))

/* XGS Diffserv, HQOS system port encoding mode */
#define SOC_DPP_XGS_TM_7_MODID_8_PORT        (0)
#define SOC_DPP_XGS_TM_8_MODID_7_PORT        (1)
#define SOC_DPP_WARMBOOT_TIME_OUT            (5000000) /*5 sec*/

#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH    (3) /* PTCH-1: 3 bytes */
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2  (2) /* PTCH-2: 2 bytes */
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_FRC_PPD (16) 
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_HIGIG_FB (1) /* NIF-Higig 0xFB prefix: 1 byte */
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VENDOR_PP (12)
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_COE       (4)
#define SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PON       (4)


#define SOC_DPP_ARAD_IS_HG_SPEED_ONLY(speed) ((speed == 21000) || (speed == 42000)|| (speed == 106000) || (speed == 127000))

/* put these defenitions under #if 0 and defined those above because of mutex deadlock they cause */    \

#define SOC_DPP_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)

#define SOC_DPP_ALLOW_WARMBOOT_WRITE_NO_ERR(operation, _rv) \
        do { \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv); \
            if (_rv != SOC_E_UNIT) { \
                _rv = SOC_E_NONE; \
            } \
        } while(0)

#define SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(_rv)\
        do {\
            _rv = soc_schan_override_disable(unit); \
        } while(0)

/* iterate over all cores */
#define SOC_DPP_CORES_ITER(core_id, index) \
    for(index = ((core_id == _SHR_CORE_ALL) ? 0 : core_id);\
        index < ((core_id == _SHR_CORE_ALL) ?  SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores : (core_id + 1));\
        index++)

#define SOC_DPP_ASSYMETRIC_CORES_ITER(core_id, index)\
    for(index = ((core_id == _SHR_CORE_ALL || SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)) ? 0 : core_id);\
        index < ((core_id == _SHR_CORE_ALL) ?  \
                    (SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit) ? 1 : SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) : \
                    (core_id + 1));\
        index++)

/* validate core in range */
#define SOC_DPP_CORE_VALIDATE(unit, core, allow_all) \
        if((core >= SOC_DPP_DEFS_GET(unit, nof_cores) || (core < 0)) && \
           (!allow_all || core != _SHR_CORE_ALL)) { \
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_BCM_MSG("Invalid Core %d"), core)); \
        }

/* converts modid to base modid */
#define SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, base_modid) \
           ((base_modid <= modid) && (base_modid + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > modid))

#define SOC_DPP_CORE_TO_MODID(base_modid, core) (base_modid + core)

#define SOC_DPP_MODID_TO_CORE(unit, base_modid, modid) \
           modid - base_modid; \
           SOC_DPP_CORE_VALIDATE(unit, modid - base_modid, FALSE) 

typedef enum soc_dpp_core_mode_type_e {
    soc_dpp_core_symmetric = 0x0,
    soc_dpp_core_asymmetric = 0x1
} soc_dpp_core_mode_type_t;

typedef struct soc_dpp_config_core_mode_s {
    uint8 nof_active_cores;
    soc_dpp_core_mode_type_t type;
} soc_dpp_config_core_mode_t;

#define SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores == 1)
#define SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit) (SOC_DPP_CONFIG(unit)->core_mode.type == soc_dpp_core_symmetric)
#define SOC_DPP_CORE_ALL(unit) (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit) ? 0 : SOC_CORE_ALL)

typedef struct soc_dpp_guaranteed_q_resource_s {
   uint32  total;/* book keeping of the total guaranteed resource for VOQs */
   uint32  used; /* book keeping of the used guaranteed resource for VOQs */
} soc_dpp_guaranteed_q_resource_t;

typedef struct soc_dpp_tm_config_s {
  uint32  *base_address;
  int8    max_interlaken_ports;
  int8    max_oob_ports;
  int8    fc_oob_type[SOC_DPP_MAX_OOB_PORTS];
  int8    fc_oob_mode[SOC_DPP_MAX_OOB_PORTS];
  int16   fc_oob_calender_length[SOC_DPP_MAX_OOB_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_oob_calender_rep_count[SOC_DPP_MAX_OOB_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_oob_tx_speed[SOC_DPP_MAX_OOB_PORTS];
  int8    fc_inband_intlkn_type[SOC_DPP_MAX_INTERLAKEN_PORTS];
  int8    fc_inband_intlkn_mode[SOC_DPP_MAX_INTERLAKEN_PORTS];
  int16   fc_inband_intlkn_calender_length[SOC_DPP_MAX_INTERLAKEN_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_inband_intlkn_calender_rep_count[SOC_DPP_MAX_INTERLAKEN_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_inband_intlkn_calender_llfc_mode[SOC_DPP_MAX_INTERLAKEN_PORTS];
  int8    fc_inband_intlkn_llfc_mub_enable_mask[SOC_DPP_MAX_INTERLAKEN_PORTS];
  int8    fc_inband_intlkn_channel_mub_enable_mask[SOC_DPP_MAX_INTERLAKEN_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_inband_mode[SOC_MAX_NUM_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int8    fc_ilkn_rt_calendar_mode[SOC_DPP_MAX_INTERLAKEN_PORTS][SOC_TMC_CONNECTION_DIRECTION_BOTH];
  int     fc_cl_sch_enable;   /* Enable CL SCHEDULER Flow Control */
  int     fc_calendar_coe_mode;
  int     fc_calendar_pause_resolution;  /* unit: usec */
  int     fc_calendar_e2e_status_of_entries;
  int     fc_calendar_indication_invert;  
  int16   wred_packet_size;
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE multicast_egress_bitmap_group_range;
  int8    is_petrab_in_system;
  int8    is_immediate_sync;
  uint8   guaranteed_q_mode; /* which resource can be guaranteed for VOQs. must be one of SOC_DPP_GUARANTEED_Q_RESOURCE_* */
  uint8   mc_mode; /* should hold DPP_MC_*_MODE bits. */
  uint32  nof_mc_ids; /* number of egress MC IDs */
  uint32  nof_ingr_mc_ids; /* number of ingress MC IDs */
  uint32  ingress_mc_id_alloc_range_start;
  uint32  ingress_mc_id_alloc_range_end;
  uint32  egress_mc_id_alloc_range_start;
  uint32  egress_mc_id_alloc_range_end;
  uint32  ingress_mc_max_queue;    /* the maximum queue allowed as an ingress multicast destination */
  uint32  ingress_mc_nof_sysports; /* the number of allowed system ports as an ingress multicast destination */
  bcm_if_t ingress_mc_max_cud;     /* the maximum CUD allowed for ingress replications */
  bcm_if_t egress_mc_max_cud;      /* the maximum CUD allowed for egress replications */
  /*
   * Delete FIFO congestion thresholds
   */
  int     delete_fifo_almost_full_multicast_low_priority;
  int     delete_fifo_almost_full_multicast;
  int     delete_fifo_almost_full_all;
  int     delete_fifo_almost_full_except_tdm;
  soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;/* book keeping of the guaranteed resource for VOQs */
  int32   flow_base_q;
  int8    is_fe1600_in_system;
  uint8   mc_ing_encoding_mode; /* should hold ARAD_MC_DEST_ENCODING_* values. */
  uint16  max_ses; /* maximum SEs (Scheduling elements) */
  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_config_mode;  /*the ipf to inverse/proportinal mode*/
  uint16  cl_se_min;
  uint16  cl_se_max;
  uint16  fq_se_min;
  uint16  fq_se_max;
  uint16  hr_se_min;
  uint16  hr_se_max;
  uint16  port_hr_se_min;
  uint16  port_hr_se_max;
  int32   max_connectors; /* maximim connector resources */
  int8    max_egr_q_prio; /* Number of Egress queue priorities */
  int16   invalid_port_id_num; /* Invalid port id (outside the allowed range) */
  uint16  invalid_se_id_num; /* Invalid scheduling element id (outside the allowed range) */
  int32   invalid_voq_connector_id_num; /* Invalid VOQ connector id (outside the allowed range) */
  int8    nof_vsq_category; /* Number of VSQ categories */
  int16   hr_isq[SOC_DPP_DEFS_MAX(NOF_CORES)]; /* Reserved HR for ISQ */
  int     hr_fmqs[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_MULT_FABRIC_NOF_CLS]; /* Reserved HRs for FMQ */
  int8    is_port_tc_enable; /* Set is port TC feature to enable , disable */
  int8    cnm_mode;
  int8    cnm_voq_mapping_mode;
  uint8   various_bm;
  int8    ftmh_outlif_enlarge;
  uint8   otmh_header_used; /* Is the OTMH header used for any port */
  uint8   l2_encap_external_cpu_used; /* Is the L2 encapsulation is used for any port */
} soc_dpp_tm_config_t;

typedef struct soc_dpp_config_qos_s {
  int8    nof_ing_elsp;
  int8    nof_ing_lif_cos;
  int8    nof_ing_pcp_vlan;
  int8    nof_egr_remark_id;
  int8    nof_egr_mpls_php;
  int8    nof_egr_pcp_vlan;
  int8    nof_ing_cos_opcode;
  int8    nof_egr_l2_i_tag;
  int8    nof_egr_dscp_exp_marking;
  uint32  mpls_elsp_label_range_min;
  uint32  mpls_elsp_label_range_max;
  int8    egr_pcp_vlan_dscp_exp_profile_id;
  int8    egr_pcp_vlan_dscp_exp_enable;
  int8    egr_remark_encap_enable;
  int8    dp_max;
  int8    ecn_mpls_one_bit_mode; /* If set ECN for MPLS encapsulation is in 1-bit mode, else 2-bits mode */
  int8    ecn_mpls_enabled; /* If set, ECN for MPLS encapsulation is enabled, otherwise, ECN is ignored */
  int8    ecn_ip_enabled;   /* If set, ECN for IP encapsulation is enabled, otherwise, ECN is ignored */
} soc_dpp_config_qos_t;


typedef struct soc_dpp_config_meter_s {
  int32    nof_meter_a; /* number of meters in processor A*/
  int32    nof_meter_b; /* number of meters in processor B*/
  int32    nof_meter_profiles; /* number of meter profiles */
  int32    nof_high_rate_profiles; /* number of high profiles */
  int32    policer_min_rate; /* global minum rate, kbits */
  int32    meter_min_rate; /* global minum rate, kbits */
  int32    max_rate; /* global max rate, kbits (shared for meter and policer) */
  int32    min_burst;
  int32    max_burst;
  int32    lr_max_burst; /* low rate max burst, bytes */
  int32    lr_max_rate; /* low rate max rate, bytes */
} soc_dpp_config_meter_t;

typedef struct soc_dpp_config_l3_s {
  int32     max_nof_vrfs; /* number of Vrfs*/
  int32     nof_rifs; /* number of RIFs */
  int       ecmp_max_paths; /* max number of paths in ECMP group */
  int       eep_db_size;
  int       fec_db_size;
  int       fec_ecmp_reserved; /* number of FEC enties reserved for ECMP, FEC allocated from this area only if no other FECs available*/
  int       vrrp_max_vid; /* VRRP: max number of VRIDs supported on each VLAN */
  int       vrrp_ipv6_distinct; /* VRRP: enable separate VRID configuration for IPv4 and IPv6 */
  uint8     multiple_mymac_enabled; /* Enable multiple mymac termination */
  uint8     multiple_mymac_mode;    /* Multiple mymac termination mode */
  uint8     ip_enable;
  uint32    nof_rps; /* number of RP-IDs for IPMC BIDIR, 0 if feature not supported */ 
  uint32    nof_bidir_vrfs; /* number of VRFs that support BIDIR */ 
  uint8     ipmc_vpn_lookup_enable; /* 1 - Per-VRF-IPMC, 0 - Global-IPMC */
  uint8     actual_nof_routing_enablers_bm; /* total routing enablers in the rif profile */
  uint8     intf_class_max_value; /* the intf class value in the rif profile */
  uint8     bit_offset_of_the_routing_enablers_profile; /*the position of the routing enablers bm in the rif profile */
} soc_dpp_config_l3_t;


typedef struct soc_dpp_config_pp_general_s {
  int32    nof_user_define_traps; /* number of User define traps*/
  int32    nof_ingress_trap_codes; /* number of User define traps*/
  int32    use_hw_trap_id_flag;  /* use the HW trap id*/
} soc_dpp_config_pp_general_t;


typedef struct soc_dpp_config_l2_s {
  int32    nof_vsis; /* number of VSIs*/
  int32    nof_lifs; /* number of LIFs */
  int32    mac_size; /* mac table size */
  int32    mac_nof_event_handlers; /* mac table size */
  uint8    learn_limit_mode;       /* Learn Limit mode: VSI / LIF */
  int32    learn_limit_lif_range_base[SOC_DPP_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES];    /* Learn Limit Base Location for each mapped LIF range */
} soc_dpp_config_l2_t;

typedef struct soc_dpp_config_trill_s {
  SOC_PPD_TRILL_MODE mode;
  uint8    mc_prune_mode;
  uint8    designated_vlan_inlif_enable;  /* Enable VT lookup based on port VD X Designated-VLAN for Trill ports */
  uint8    mc_id;  /* Enable use of sw db mc-id -> nickname (old implementation)*/
  uint8    create_ecmp_port; /* configure trill ecmp by creating ecmp trill port (old implementation) */
  uint8    mult_adjacency;   /* add multicast adjacency as a part of trill_port_add (old implementation) */
  uint8    transparent_service; /*Trill transparent service enable the Trill campus to carry customer VLAN information for remote processing */ 
} soc_dpp_config_trill_t;

typedef struct soc_dpp_config_am_s {
    uint16 nof_am_resource_ids;
    uint16 nof_am_lif_resource_ids;
    uint16 nof_am_cosq_resource_ids;
    uint16 nof_am_template_ids;
} soc_dpp_config_am_t;

typedef struct soc_dpp_config_tdm_s {
  int8    max_user_define_bits;
  int16   min_cell_size;
  int16   max_cell_size;
  int8    is_fixed_opt_cell_size;
  int8    is_bypass;
  int8    is_packet;  /* if 0  packet tdm disabled for  device  else  packet tdm  enabled for device */
  int8    is_tdm_over_primary_pipe;  
} soc_dpp_config_tdm_t;

typedef struct soc_dpp_config_pp_s {
  SOC_PPD_MPLS_TERM_LABEL_RANGE label_ranges[SOC_PPD_MPLS_TERM_MAX_NOF_TERM_LABELS];
  int16   min_egress_encap_ip_tunnel_range;
  int16   max_egress_encap_ip_tunnel_range;
  int16   nof_pp_local_ports;
  int8    pvlan_enable;
  uint8   default_trap_strength;
  uint8   default_snoop_strength;  
  uint32   drop_dest;
  uint8    ipmc_enable; /* If set, IPMC should be initialize */
  uint8   initial_vid_enable; /* If set, Initial VID support is set */
  uint8   mpls_label_range_enable; /* If set, device support MPLS termination by range */
  uint8   mpls_label_index_enable; /* If set, device support MPLS termination by index */
  uint8   oamp_enable;
  uint8   vlan_match_db_mode; /* If 0, default application. 1. Support more than 32K entries (full_db). 3, Q-in-Q only (qinq). 4. PCP parallel lookup. */
  uint8   extend_p2p_port_enable[SOC_MAX_NUM_PORTS]; /* Extend P2P settings */
  uint8   extend_p2p_global_enable; /* If set, Extended P2P might exist */
  uint8   extend_p2p_mpls_enable; /* If set, entries for PWE will be added in LEM instead of SEM */
  uint8   extend_p2p_vlan_enable; /* If set, entries for AC will be added in LEM instead of SEM */
  uint8   extend_p2p_mim_enable; /* If set, entries for MIM will be added in LEM instead of SEM */
  /* 
   * If set, for each creation of VSI, allocates a match VSI-LIF where LIF.VSI = VSI.
   * In case unset, we allocate only one global LIF (global_lif_index) which set VSI = VLAN. 
   * In Petra-B , it is done by setting LIF.VSI = -1. In ARAD, it is done by setting VSI-Assignment.                                                                                      .
   */
  uint8   lif_per_vsi_enable; 
  SOC_PPD_LIF_ID global_lif_index_simple; /* Global LIF index used in case lif_per_vsi_enable is unset. Provides default LIF for simple bridging */ 
  SOC_PPD_LIF_ID global_lif_index_drop; /* Global LIF index used in case port default is to drop in case ingress VLAN lookup fails */
  SOC_PPD_LIF_ID local_lif_index_simple; /* Local lif for the simple global lif. */ 
  SOC_PPD_LIF_ID local_lif_index_drop; /* Local lif for the drop global lif. */ 
  uint8   pon_application_enable; /* If set ARAD is functional as part of PON application */
  uint8   pon_tcam_lkup_enable; /* If set addition lookup in TCAM for PON */  
  uint8   pon_tls_in_tcam; /* If set TLS lookup in TCAM for ARAD PON */ 
  uint8   pon_custom_enable; /* If set custom feature enable for ARAD PON */
  uint8   local_switching_enable; /* If set enable the local switching feature */
  /* 
   *  Bit 0 : IP Tunnel (no L2 above) DIP only. Bit 1: IP Tunnel (SIP and DIP) no L2 above.
   *  Bit 2: Vxlan. Bit 3: NVGRE. Bit 4: EoIP
   */ 
  uint8   ipv4_tunnel_term_bitmap_enable;   
  uint8   ingress_ipv4_tunnel_term_mode; /* In case NVGRE/VXLAN/EoIP is enabled support either lookups: 0 - (SIP,DIP) or 1 - (SIP),(DIP) */
  uint8   ipv6_tunnel_enable; /* If set IPV6 tunnel termination and encapsulation enabled */
  /* > ARAD-B, if set then msb of tos index used for DF, thus only 8 profiles for TOS*/
  uint8   ip_tunnel_defrag_set;
  uint8   l3_source_bind_mode; /* L3 source bind mode, it will be disable,ipv4, ipv6 and ip */
  uint8   mim_vsi_mode; /* 0 for default, 1 for 32K */
  /* Fcoe*/
  uint8   fcoe_enable;
  uint32  fcoe_reserved_vrf;
  uint8   unbound_uninstalled_elk_entries_number;
  uint8   evb_enable;
  uint8   next_hop_mac_extension_enable; /* ARP extender */
  SOC_PPD_LIF_ID    mim_global_lif_ndx; /* default lif, used only for MiM */
  SOC_PPD_LIF_ID    mim_local_lif_ndx; /* default local lif, used only for MiM */
  SOC_PPD_AC_ID     mim_global_out_ac; /* default out-ac, used only for MiM */
  SOC_PPD_AC_ID     mim_local_out_ac; /* default out-ac, used only for MiM */
  /* Vlan Translation */
  uint8   vlan_translate_mode; /* VLAN translation mode: 0: Normal, 1: Advanced */
  int16   nof_ive_action_ids;           /* Maximum values for Ingress/Egress Vlan edit actions */
  int16   nof_ive_reserved_action_ids;
  int16   nof_eve_action_mappings;
  int16   nof_eve_action_ids;
  int16   nof_eve_reserved_action_ids;

  /* VMAC */
  uint8   vmac_enable; /* Disable/enable VMAC, 0 - disable 1- enable */
  SOC_SAND_PP_MAC_ADDRESS  vmac_encoding_val;  /* VMAC encoding value */
  SOC_SAND_PP_MAC_ADDRESS  vmac_encoding_mask; /* VMAC encoding mask */
  uint8   sys_rsrc_mgmt;          /* system resource management: 0 all global. 1: advanced */

  uint8   mpls_eli_enable; /* If set, device support special Label ELI in MPLS termination. Requires VTT programs */
  uint8   oam_ccm_2_fhei_enable;
  uint8   oam_ccm_2_fhei_eg_default_mirror_profile;
  uint8   oam_ccm_2_fhei_eg_fake_injection_port;
  uint8   oam_classifier_advanced_mode; /* Relevant only to ARAD+ */
  uint8   oam_use_fifo_dma; /* Relevant only to ARAD+ */
  uint8   bfd_ipv4_single_hop; /* Relevant only to ARAD+ */
  uint8   bfd_mpls_lsp_support; 
  uint8   bfd_ipv6_enable; /* Relevant only to ARAD+ */
  uint8   bfd_extended_ipv4_src_ip; /* Enable configuring of SIP for BFD over IPv4. */
  uint8   oam_statistics; /* Relevant only to ARAD+ */

  uint8   port_use_initial_vlan_only_enabled; /* If set, device can support port to use Initial-VID 
                                               * for both tagged and untagged packets. In case of unset, 
                                               * device support for all ports Initial-VID for untagged packets 
                                               * and Outer-VID for tagged packets.
                                               */
  uint8   port_use_outer_vlan_and_initial_vlan_enabled; /* If set, device can support port to use Outer-VID 
                                                         * for tagged packets and Initial-VID for untagged packets.
                                                         * In case unset, no need for programs to different between Initial and Outer.
                                                         */
  uint8   udh_port_exists; /* If set, then device is configured with port header type User Defined Header                             
                             * tagged on the outgoing traffic. Need special EGR Prog. Editor program.                     
                             */                                                                             
  uint8   port_extender_map_lc_exists;  /* set if port extender interfacing via out-going prepend 1B header
                                         * contains the Out-PP-Port mapped on the extender LC
                                         */
  uint8   ipv6_with_rpf_2pass_exists;   /* ipv6 with RPF tm-port-header-exists */
  soc_port_t   ipv6_fwd_pass2_rcy_local_port; /* The local Recycle port used for ipv6 (L3) FWD PASS2 */

  uint8   xgs_port_exists;  /* If set, then in device exist at least one XGS_MAC_EXTENDER port */
  uint8   egress_encap_bank_phase[SOC_DPP_DEFS_MAX(EG_ENCAP_NOF_BANKS)]; /* Phase access of egress encapsulation bank */
  uint8   mpls_databases[3]; /* 0 - ISEM-A, 1 - ISEM-B, 2 - TCAM, 0xFF - Invalid */
  SOC_PPC_MPLS_TERM_NAMESPACE_TYPE mpls_namespaces[3];
  uint8   flexible_qinq_enable; 
  uint8   ipv6_ext_header;
  uint8   l3_source_bind_subnet_mode; /* L3 source bind subnet mode, it will be disable,ipv4, ipv6 and ip */
  uint8   l3_src_bind_arp_relay; /*  Enable/disable IP anti-spoofing function for ARP request  */
  SOC_PPD_LIF_ID ip_lif_dummy_id; /* In-LIF-ID of IP-LIF-dummy for termination of IP-Overlay bud multicast traffic */
  uint8   custom_feature_vt_tst1;
  uint8   custom_feature_vt_tst2;
  uint8   pon_port_channelization_enable; /* If set PON ports of ARAD are channelized  */
  uint8   pon_port_channelization_num; /* number of max channelized ports per PON port */  
  uint8   roo_enable;  /* indicate if roo application is enabled */
  uint8   tunnel_termination_in_tt_only; /* If set all Tunnel termination looks are defined in TT stage */
  uint8   gal_support; /* Indicate if MPLS termination support GAL */
  uint8   explicit_null_support; /* If set explicit NULL is supported */
  SOC_PPD_LIF_ID explicit_null_support_lif_id; /* In-LIF-ID for termination of Explicit NULL label, 
  												  must be valid when explicit_null_support is set */
  uint8   otmh_cud_ext_used; /* Indicate that the OTMH CUD extension is enabled for at least one port */
  uint8   custom_feature_pp_port;
  uint8   coe_port_exists; /* If set, then device is configured with port header type CoE Header                             
                                              * tagged on the outgoing traffic. Need special EGR Prog. Editor program.
                                              */
  uint8   em_shadow_enable; /* Enable/disable shadow maintanence for EM tables */	
#ifdef BCM_88660_A0
  uint8   ptp_48b_stamping_enable; /* Enable/disable 48 bits stamping used for 1588 packets (by deault 32b stamping is used) */	
#endif /* BCM_88660_A0 */
										  
  uint8   mldp_support;
  SOC_PPD_LIF_ID mldp_dummy_lif_id[2]; /* In-LIF-ID of IN-LIF-dummy for temination of mLDP,must be set if mLDP is support */
  uint8   compression_spoof_ip6_enable; /* Enable compression sip6 sav function */
  uint8   compression_ip6_enable; /* Enable compression ip6 function, the compression result will be used in PMF */
} soc_dpp_config_pp_t;


#ifdef BCM_PETRAB_SUPPORT
typedef struct soc_dpp_config_pb_s {
  SOC_PB_MGMT_OPERATION_MODE  op_mode;
  SOC_PB_HW_ADJUSTMENTS       hw_adjust;
  SOC_PB_INIT_BASIC_CONF      basic_conf;
  SOC_PB_INIT_PORTS           fap_ports;
  SOC_PB_INIT_OOR             oor;
  SOC_PB_PP_MGMT_OPERATION_MODE pp_op_mode;
} soc_dpp_config_pb_t;
#endif /* BCM_PETRAB_SUPPORT */


/* Main Ardon/ATMF configuration structure */
typedef struct soc_dpp_config_ardon_s { 
    int  enable; 
} soc_dpp_config_ardon_t; 



typedef struct soc_dpp_config_s {
#ifdef BCM_PETRAB_SUPPORT
  soc_dpp_config_pb_t *pb;
#endif /* BCM_PETRAB_SUPPORT */
  soc_dpp_config_arad_t *arad;
  soc_dpp_config_jer_t *jer;
  soc_dpp_config_ardon_t *ardon;
  soc_dpp_tm_config_t  tm;
  soc_dpp_config_tdm_t tdm;
  soc_dpp_config_pp_t  pp;
  soc_dpp_config_qos_t qos;
  soc_dpp_config_meter_t meter;
  soc_dpp_config_l3_t l3;
  soc_dpp_config_pp_general_t dpp_general;
  soc_dpp_config_l2_t l2;
  soc_dpp_config_trill_t trill;
  soc_dpp_config_am_t   am;
  uint8 emulation_system;
  soc_dpp_config_core_mode_t core_mode;
} soc_dpp_config_t;

typedef void (*soc_dpp_isr_f)(void *);

typedef struct soc_dpp_route_entry_s { 
  soc_fabric_inband_route_t route;
  uint8                     is_valid;
  uint8                     group;
  uint8                     is_group_valid;
} soc_dpp_route_entry_t;

typedef struct soc_dpp_port_params_s { 
  uint32 comma_burst_conf[SOC_DPP_NOF_LINKS]; 
  uint32 control_burst_conf[SOC_DPP_NOF_INSTANCES_MAC]; 
  uint32 cl72_configured[SOC_MAX_NUM_PORTS];
} soc_dpp_port_params_t; 

typedef struct soc_dpp_control_s {
  soc_dpp_config_t *cfg;
  soc_dpp_isr_f     isr;
  soc_dpp_route_entry_t inband_route_entries[SOC_DPP_NUM_OF_ROUTES];
  void *state; /* bcm state */
  soc_dpp_port_params_t port_params;
  uint8 is_silent_init;
  uint8 is_modid_set_called;
} soc_dpp_control_t;


typedef struct
{
   uint32    write_value;
   uint32    period;
   uint32    run_count;
   uint32    incr_run_count;
   int       fault_inject;
   int       skip_reset_and_write;
   uint32     include_port_macros;  /* when set 1: test include port macros(nbih,nbil) other skipping port macros*/
   uint32    start_from;  /*skip meme when mem-id<start_from*/
   int       max_size;    /* skip mems when mem size in byte > max_size*/
} tr8_dbase_t;

/* Set L2 learn limit mode. 
 * 0 - per VLAN
 * 1 - per VLAN PORT
 * 2 - PON port and Tunnel-ID */
typedef enum soc_dpp_mac_limit_mode_e {
    mac_limit_per_vlan = 0,
    mac_limit_per_vlan_port,
    mac_limit_per_tunnel_port
}soc_dpp_mac_limit_mode_t;

/* Egress encap bank phase in soc property. */
typedef enum soc_dpp_egress_encap_bank_phase_e {
    egress_encap_bank_phase_dynamic = 0,
    egress_encap_bank_phase_static_1,
    egress_encap_bank_phase_static_2,
    egress_encap_bank_phase_static_3,
    egress_encap_bank_phase_static_4,
    egress_encap_bank_phase_static_5,
    egress_encap_bank_phase_static_6,
    egress_encap_bank_phase_nof
}soc_dpp_egress_encap_bank_phase_t;


#define SOC_IS_DPP_TYPE(dev_type) \
    (((dev_type) == PETRAB_DEVICE_ID) || \
     ((dev_type & 0xfff0) == (ARAD_DEVICE_ID & 0xfff0)) || \
     ((dev_type & 0xfff0) == (ARADPLUS_DEVICE_ID & 0xfff0)) || \
     ((dev_type) == BCM88675_DEVICE_ID) || \
     ((dev_type) == BCM88375_DEVICE_ID) || \
     ((dev_type) == BCM88378_DEVICE_ID) || \
     ((dev_type) == BCM88202_DEVICE_ID) || \
     ((dev_type) == ACP_PCI_DEVICE_ID)  || \
     ((dev_type) == BCM88350_DEVICE_ID) || \
     ((dev_type) == BCM88351_DEVICE_ID) || \
     ((dev_type) == BCM88450_DEVICE_ID) || \
     ((dev_type) == BCM88451_DEVICE_ID) || \
     ((dev_type) == BCM88550_DEVICE_ID) || \
     ((dev_type) == BCM88551_DEVICE_ID) || \
     ((dev_type) == BCM88552_DEVICE_ID) || \
     ((dev_type) == BCM88360_DEVICE_ID) || \
     ((dev_type) == BCM88361_DEVICE_ID) || \
     ((dev_type) == BCM88363_DEVICE_ID) || \
     ((dev_type) == BCM88460_DEVICE_ID) || \
     ((dev_type) == BCM88461_DEVICE_ID) || \
     ((dev_type) == BCM88560_DEVICE_ID) || \
     ((dev_type) == BCM88561_DEVICE_ID) || \
     ((dev_type) == BCM88562_DEVICE_ID) || \
     ((dev_type) == BCM88850_DEVICE_ID))


#define SOC_IS_DPP_CP_TYPE(dev_type) \
    ((dev_type) == ACP_PCI_DEVICE_ID)

#define SOC_DPP_CONTROL(unit) ((soc_dpp_control_t *)SOC_CONTROL(unit)->drv)
#define SOC_DPP_CONFIG(unit)  (SOC_DPP_CONTROL(unit)->cfg)
#define SOC_DPP_IS_MESH(unit) ((SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_BACK2BACK)\
                                || (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH))
#define SOC_DPP_STATE(unit)   ((bcm_dpp_state_t *)(SOC_DPP_CONTROL(unit)->state))
#define SOC_DPP_PORT_PARAMS(unit)  (SOC_DPP_CONTROL(unit)->port_params)

#if defined(BCM_PETRAB_SUPPORT) && defined(BCM_ARAD_SUPPORT)
#define SOC_DPP_PP_ENABLE(unit) ((SOC_IS_PETRAB(unit))?(SOC_DPP_CONFIG(unit)->pb->op_mode.pp_enable):    \
                                                        (SOC_DPP_CONFIG(unit)->arad->init.pp_enable))
#define SOC_DPP_SINGLE_FAP(unit) ((SOC_IS_PETRAB(unit))?                                                                                    \
                                       (SOC_DPP_CONFIG(unit)->pb->hw_adjust.fabric.connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP):   \
                                       (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP))
#define SOC_IS_DPP_TDM_STANDARD(unit) ((SOC_IS_PETRAB(unit))?                                                              \
                                            (SOC_DPP_CONFIG(unit)->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_STA):   \
                                            (SOC_DPP_CONFIG(unit)->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA))
#define SOC_IS_DPP_TDM_OPTIMIZE(unit) ((SOC_IS_PETRAB(unit))?                                                              \
                                            (SOC_DPP_CONFIG(unit)->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT):   \
                                            (SOC_DPP_CONFIG(unit)->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT))
#elif defined(BCM_PETRAB_SUPPORT)
#define SOC_DPP_PP_ENABLE(unit)             (SOC_DPP_CONFIG(unit)->pb->op_mode.pp_enable)
#define SOC_DPP_SINGLE_FAP(unit)            (SOC_DPP_CONFIG(unit)->pb->hw_adjust.fabric.connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_SINGLE_FAP)
#define SOC_IS_DPP_TDM_STANDARD(unit)       (SOC_DPP_CONFIG(unit)->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
#define SOC_IS_DPP_TDM_OPTIMIZE(unit)       (SOC_DPP_CONFIG(unit)->pb->op_mode.tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
#elif defined(BCM_ARAD_SUPPORT)
#define SOC_DPP_PP_ENABLE(unit)             (SOC_DPP_CONFIG(unit)->arad->init.pp_enable)
#define SOC_DPP_SINGLE_FAP(unit)            (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP)
#define SOC_IS_DPP_TDM_STANDARD(unit)       (SOC_DPP_CONFIG(unit)->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA)
#define SOC_IS_DPP_TDM_OPTIMIZE(unit)       (SOC_DPP_CONFIG(unit)->arad->init.tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)
#endif

#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_OAMP_ENABLE(unit) ((SOC_IS_ARAD(unit)) && (SOC_DPP_CONFIG(unit)->arad->init.ports.oamp_port_enable))




#define SOC_IS_DPP_TRILL_FGL(unit) (SOC_IS_ARAD(unit) && (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL))



#endif

#define SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) \
      ((mac_limit_per_tunnel_port == SOC_DPP_CONFIG(unit)->l2.learn_limit_mode) || SOC_DPP_CONFIG(unit)->pp.vmac_enable)
#define SOC_MAC_LIMIT_PER_TUNNEL_DISABLE_GET(unit) \
       ((mac_limit_per_tunnel_port != SOC_DPP_CONFIG(unit)->l2.learn_limit_mode) && !(SOC_DPP_CONFIG(unit)->pp.vmac_enable))

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#define SOC_DPP_IS_ELK_ENABLE(unit) ((SOC_IS_ARAD(unit)) && (SOC_DPP_CONFIG(unit)->arad->init.elk.enable))
#else
#define SOC_DPP_IS_ELK_ENABLE(unit) FALSE
#endif

 

#ifdef PLISIM
#define SOC_DPP_IS_EM_SIM_ENABLE(unit)  TRUE
#define SOC_DPP_IS_EM_HW_ENABLE         FALSE
#else
#define SOC_DPP_IS_EM_SIM_ENABLE(unit)  SOC_DPP_CONFIG(unit)->pp.em_shadow_enable
#define SOC_DPP_IS_EM_HW_ENABLE         TRUE
#endif



#ifdef BCM_ARAD_SUPPORT
#define SOC_IS_ROO_ENABLE(unit) (SOC_IS_ARADPLUS(unit) && (SOC_DPP_CONFIG(unit)->pp.roo_enable))
#else 
#define SOC_IS_ROO_ENABLE(unit) 0
#endif   

#ifdef BCM_JERICHO_SUPPORT
/* arp entry msbs for roo host format. */
#define SOC_ROO_HOST_ARP_MSBS(unit) (SOC_DPP_CONFIG(unit)->jer->pp.roo_host_arp_msbs)
#else 
#define SOC_ROO_HOST_ARP_MSBS(unit) 0
#endif   


#define DPP_ADD_PORT(ptype, nport) \
            si->ptype.port[si->ptype.num++] = nport; \
            if ( (si->ptype.min < 0) || (si->ptype.min > nport) ) {     \
                si->ptype.min = nport; \
            } \
            if (nport > si->ptype.max) { \
                si->ptype.max = nport; \
            } \
            SOC_PBMP_PORT_ADD(si->ptype.bitmap, nport);

            
#define DPP_ADD_DISABLED_PORT(ptype, nport) \
            si->ptype.port[si->ptype.num++] = nport; \
            if ( (si->ptype.min < 0) || (si->ptype.min > nport) ) {     \
                si->ptype.min = nport; \
            } \
            if (nport > si->ptype.max) { \
                si->ptype.max = nport; \
            } \
            SOC_PBMP_PORT_ADD(si->ptype.disabled_bitmap, nport);


int soc_dpp_info_config_common_tm(int unit);
int soc_dpp_str_prop_parse_tm_port_header_type(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE *p_val_incoming, SOC_TMC_PORT_HEADER_TYPE *p_val_outgoing, uint32* is_hg_header);
int soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(int unit, soc_port_t port, SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *p_val);
int soc_dpp_str_prop_parse_flow_control_type(int unit, soc_port_t port, SOC_TMC_PORTS_FC_TYPE *p_val);
int soc_dpp_str_prop_fabric_connect_mode_get(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode);
int soc_dpp_str_prop_fabric_ftmh_outlif_extension_get(int unit, SOC_TMC_PORTS_FTMH_EXT_OUTLIF *fabric_ftmh_outlif_extension);
int soc_dpp_str_prop_ext_ram_type(int unit, SOC_TMC_DRAM_TYPE *soc_petra_dram_type);
int soc_dpp_prop_parse_dram_number_of_columns(int unit, SOC_TMC_DRAM_NUM_COLUMNS *p_val);
int soc_dpp_prop_parse_dram_number_of_banks(int unit, SOC_TMC_DRAM_NUM_BANKS *p_val);
int soc_dpp_prop_parse_dram_ap_bit_pos(int unit, SOC_TMC_DRAM_AP_POSITION *p_val);
int soc_dpp_prop_parse_dram_burst_size(int unit, SOC_TMC_DRAM_BURST_SIZE *p_val);
int soc_dpp_dram_prop_get(int unit, char *prop, uint32 *p_val);
int soc_dpp_str_prop_parse_mpls_context(int unit, uint8 *include_inrif, uint8 *include_port);
int soc_dpp_str_prop_parse_logical_port_mim(int unit, SOC_PPD_LIF_ID *p_val_incoming, SOC_PPD_AC_ID *p_val_outgoing);
int soc_dpp_common_init(int unit);
int soc_dpp_common_tm_init(int unit, SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *multicast_egress_bitmap_group_range);
int soc_dpp_device_specific_info_config_direct(int unit);
int soc_dpp_device_specific_info_config_derived(int unit);
int soc_dpp_prop_parse_dtm_nof_remote_cores(int unit, int core, int region);
int soc_dpp_prop_parse_dtm_mapping_mode(int unit, int core, int region, int default_mode, int *mapping_mode);

soc_driver_t* soc_dpp_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id);
int soc_dpp_info_config(int unit);
int soc_dpp_chip_type_set(int unit, uint16 dev_id);

extern int soc_dpp_attach(int unit);
extern int soc_dpp_detach(int unit);
extern int soc_dpp_init(int unit, int reset);
extern int soc_dpp_deinit(int unit);
extern int soc_dpp_reinit(int unit, int reset);
extern int soc_dpp_dump(int unit, char *pfx);
extern void soc_dpp_chip_dump(int unit, soc_driver_t *d);
extern int soc_dpp_device_reset(int unit, int mdoe, int action);

/* Read/write internal registers */
extern int soc_dpp_reg_read(int unit, soc_reg_t reg, uint32 addr, 
                            uint64 *data);
extern int soc_dpp_reg_write(int unit, soc_reg_t reg, uint32 addr, 
                             uint64 data);

extern int soc_dpp_reg32_read(int unit, uint32 addr, uint32 *data);
extern int soc_dpp_reg32_write(int unit, uint32 addr, uint32 data);

extern int soc_dpp_reg64_read(int unit, uint32 addr, uint64 *data);
extern int soc_dpp_reg64_write(int unit, uint32 addr, uint64 data);

/* Read/Write internal memories */
extern int soc_dpp_mem_read(int unit, soc_mem_t mem, int copyno, int index, 
                            void *entry_data);
/*
extern int soc_dpp_mem_array_read(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index,
                            void *entry_data);
*/
extern int soc_dpp_mem_write(int unit, soc_mem_t mem, int copyno, int index, 
                             void *entry_data);
/*
extern int soc_dpp_mem_array_write(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index,
                             void *entry_data);
*/
#ifdef BCM_DNX_P3_SUPPORT
extern int soc_dpp_p3_mem_insert(int unit, soc_mem_t mem, int copyno,void *key_result);
extern int soc_dpp_p3_mem_lkup(int unit, soc_mem_t mem, int copyno, void *key_result);
extern int soc_dpp_p3_mem_delete(int unit, soc_mem_t mem, int copyno, void *key);
#endif /*BCM_DNX_P3_SUPPORT*/


/* 
 * Set fap ID 
 */
int
soc_dpp_petra_stk_modid_config_set(int unit, int fap_id);
/* 
 * Set fap ID 
 * Call pb init phase 2 
 */ 
int
soc_dpp_petra_stk_modid_set(int unit, int fap_id);

/* 
 * Enable traffic 
 */ 
int
soc_dpp_petra_stk_module_enable(int unit, int enable);
/*
 * Map block type to number of block instance. 
 */
int
soc_dpp_nof_block_instances(int unit,  soc_block_types_t block_types, int *nof_block_instances) ;

/*
 * Get unit AVS value 
 */
int
soc_dpp_avs_value_get(int unit, uint32* avs_value);

uint32
_soc_dpp_property_port_get(int unit, soc_port_t port,
                      const char *name, uint32 defl);

#ifdef BCM_WARM_BOOT_SUPPORT
uint8 
_bcm_dpp_switch_is_immediate_sync(int unit);
#endif /*BCM_WARM_BOOT_SUPPORT*/

#ifdef BCM_ARAD_SUPPORT
/*init ofp rates configuration*/
int
_soc_dpp_arad_ofp_rates_set(int unit);
#endif

int
soc_dpp_cache_enable_init(int unit);

/* check whether next hop mac extension feature work in arad mode:
   for compatibility with ARAD, Disable hardware computation of Host-Index for DA.
   Instead, do it the Arad way: PMF will add pph learn extension (system header ), egress program editor will stamp the DA  
   if this soc property is disabled, then use hardware computation using the chicken bit */ 
#define SOC_IS_NEXT_HOP_MAC_EXT_ARAD_COMPATIBLE(unit) (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "next_hop_mac_ext_arad_compatible", 0))


#endif  /* _SOC_DPP_DRV_H */
