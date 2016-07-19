/* -*- mode:c++; c-style:k&r; c-basic-offset:2; indent-tabs-mode: nil; -*- */
/* vi:set expandtab cindent shiftwidth=2 cinoptions=\:0l1(0t0g0: */
/*
 * $Id: g2xx_pp.h,v 1.7 Broadcom SDK $
 *
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
 */

#ifndef _SB_G2XX_FE_PP_BURNER_H_
#define _SB_G2XX_FE_PP_BURNER_H_
#include <soc/sbx/g2xx/g2xx_pp_rule_encode.h>

#define SB_G2XX_FE_PP_MAX_LSM_ENTRIES          64
#define SB_G2XX_FE_PP_MAX_L2CP_TUNNEL_ENTRIES  3
#define SB_G2XX_FE_PP_MAX_IPV4_TUNNELS         40
#define SB_G2XX_FE_PP_MAX_IPV6_TUNNELS         40
#define SB_G2XX_FE_PP_MAX_TCP_FILTER_ENTRIES   10
#define SB_G2XX_FE_PP_MAX_UDP_FILTER_ENTRIES   10
#define SB_G2XX_FE_PP_MAX_ETYPE_PRTL_VLANS     8
#define SB_G2XX_FE_PP_MAX_LLCTYPE_PRTL_VLANS   8

#define SB_G2XX_FE_PP_PORT_GLOBAL               (0xffffffff)
#define SB_G2XX_FE_PP_INVALID_LSM_ENTRY         (0xFFF)
#define SB_G2XX_FE_PP_INVALID_TUNNEL_ENTRY      (0xFFE)
#define SB_G2XX_FE_PP_INVALID_PVLAN_ENTRY       (0xFFD)
#define SB_G2XX_FE_PP_INVALID_TUNNEL_TYPE       (0xFFC)
#define SB_G2XX_FE_PP_INVALID_TPID_STACK        (0xFFB)
#define SB_G2XX_FE_PP_INVALID_L2CP_TUNNEL_ENTRY (0xFFA)
#define SB_G2XX_FE_PP_VRF_LSM_ID                (63)

#define SB_G2XX_FE_PP_DEFAULT_SET0_STAG0           (0x9100)
#define SB_G2XX_FE_PP_DEFAULT_SET0_STAG1           (0x88a8)
#define SB_G2XX_FE_PP_DEFAULT_SET0_CTAG            (0x8100)
#define SB_G2XX_FE_PP_DEFAULT_SET1_STAG0           (0x9300)
#define SB_G2XX_FE_PP_DEFAULT_SET1_STAG1           (0x9200)
#define SB_G2XX_FE_PP_DEFAULT_SET1_CTAG            (0x8100)

#define SB_G2XX_FE_PP_MAC_DA_BYTE_OFFSET        0
#define SB_G2XX_FE_PP_MAC_SA_BYTE_OFFSET        6
#define SB_G2XX_FE_IPV4_PROTOCOL_BYTE_OFFSET    9
#define SB_G2XX_FE_IPV4_SA_BYTE_OFFSET         12
#define SB_G2XX_FE_IPV4_DA_BYTE_OFFSET         16
#define SB_G2XX_FE_IPV6_SA_BYTE_OFFSET          8
#define SB_G2XX_FE_IPV6_DA_BYTE_OFFSET         24


#define SB_G2XX_FE_CTPID_INDEX    0
#define SB_G2XX_FE_STPID0_INDEX   1
#define SB_G2XX_FE_STPID1_INDEX   2

/* 
 * MPLS Label Space User Configuration Notes:
 *-------------------------------------------
 * The restriction on the user configuration of this structure are
 * 
 * 1. uNumberOfPWELabels and uNumberOfLERLabels MUST be a power of 2.
 * 
 * 2. If the highest bit set in the binary notation of the uNumberOfPWELabels
 *    is at BIT X then you shouldn't have any bit Y set in the uPWEBaseLabel 
 *    for which Y <= X. 
 *    
 * 3. If the highest bit set in the binary notation of the uNumberOfLERLabels
 *    is at BIT X then you shouldn't have any bit Y set in the uLERBaseLabel 
 *    for which Y <= X. 
 * 
 * For easy construction of this label bases and number of labels the rule of thumb
 *      uNumberOfPWELabels      = pow(2,A);
 *      uNumberOfLERLabels    = pow(2,B); 
 * Then choose 
 *      uPWEBaseLabel   = uNumberOfBeomLabels* pow(2, C); 
 *      uLERBaseLabel = uNumberOfTunnelLabels* pow(2, D); 
 * Where A, B are obtained from the system BEOM and TUNNEL label space requirements.
 * Typicall C and D can be 1 assuming that BEOM and LER label ranges are selected in
 * non overlapping regions.
 * 4. uNumberOfSwitchLabel > (uNumberOfPWELabels+uNumberOfLERLabels);
 * 
 * 5. uPWEBaseLabel+uNumberOfBeomLabels < uNumberOfSwitchLabels;
 * 
 * 6. uLERBaseLabel+uNumberOfTunnelLabels < uNumberOfSwitchLabels;
 */

typedef struct soc_sbx_g2xx_ppMplsLabelSpaceConfiguration_s soc_sbx_g2xx_pp_mpls_label_space_configuration_t;

struct soc_sbx_g2xx_ppMplsLabelSpaceConfiguration_s
{
  uint32 uNumberOfMplsLabels;
  uint32 uNumberOfPWELabels;
  uint32 uNumberOfLERLabels;
  uint32 uNumberOfStatsLabels;

  uint32 uPWEBaseLabel;
  uint32 uLERBaseLabel;
  uint32 uStatsBaseLabel;

  uint32 uPWETPID;
  uint8  uMplsOAMEnabled;
};

typedef uint32 soc_sbx_g2xx_pp_lsmid_t;
typedef uint32 soc_sbx_g2xx_pp_l2cptunnelid_t;
typedef uint32 soc_sbx_g2xx_pp_tunnel_id_t;
typedef uint32 soc_sbx_g2xx_pp_tcp_filter_id_t;
typedef uint32 soc_sbx_g2xx_pp_udp_filter_id_t;

/**
 * This function enables/disables the broadshield checks performed
 * by the PP hardward block. A PP generated exception is always
 * enabled/disabled on a global basis not on per port basis.
 *
 * @param pFe         Pointer to the soc_sbx_g2xx_state_t state
 * @param eType       Check of type sbFe2000ExcType_t
 * @param bEnable     Enable/Disable
 * @return            Return status of the type sbStatus_t
 */
sbStatus_t 
soc_sbx_g2xx_pp_broad_shield_check_set( soc_sbx_g2xx_state_t *pFe, 
                             int eType,
                             uint8 bEnable);


/**
 * This function returns the current state of the broad shield check
 * by the PP hardward block.
 *
 * see ilib/include/sbG2Fe.h for the enumeration of the pp 
 * generated exception. Notice that only the values from 
 * @param pFe         Pointer to the soc_sbx_g2xx_state_t state
 * @param eType       Check of type sbFe2000ExcType_t
 * @param pbEnable    Enable/Disable
 * @return            Return status of the type sbStatus_t
 */
sbStatus_t 
soc_sbx_g2xx_pp_broad_shield_check_get( soc_sbx_g2xx_state_t *pFe, 
                             int eType,
                             uint8 *pbEnable);

/* Bit mask fields used in fieldMask argument to psc hash template get/set functions */
#define SB_G2XX_PSC_MAC_DA   0x00000001
#define SB_G2XX_PSC_MAC_SA   0x00000002
#define SB_G2XX_PSC_IP_DA    0x00000004
#define SB_G2XX_PSC_IP_SA    0x00000008
#define SB_G2XX_PSC_L4SS     0x00000010
#define SB_G2XX_PSC_L4DS     0x00000020
#define SB_G2XX_PSC_VID      0x00000040

/**
 * Configure the fields used in the aggregate hash calculation.
 *
 * @param pFe         Pointer to the soc_sbx_g2xx_state_t struct
 * @param uFieldMask  Bit mask of fields to be included
 * @return            Return Status of type sbStatus_t
 */
sbStatus_t
soc_sbx_g2xx_pp_hash_templates_set (soc_sbx_g2xx_state_t *pFe, uint32 fieldMask);

/**
 * Get the fields used in the aggregate hash calculation.
 *
 * @param pFe         Pointer to the soc_sbx_g2xx_state_t struct
 * @param uFieldMask  Bit mask of fields to be included
 * @return            Return Status of type sbStatus_t
 */
sbStatus_t
soc_sbx_g2xx_pp_hash_templates_get (soc_sbx_g2xx_state_t *pFe, uint32 *fieldMask);

/**
 * Set PSC per Unit
 *
 * @param pFe         Pointer to the soc_sbx_g2xx_state_t struct
 * @param uFieldMask  Bit mask of fields to be included
 * @return            Return Status of type sbStatus_t
 */
extern sbStatus_t
soc_sbx_g2xx_pp_psc_set (int unit, uint32 fieldMask);

/**
 * Enable Egress PPE rules for up MEP processing
 *
 * @param unit        unit number
 * @param uValid      Enable/Disable flag
 * @return            Return Status of type sbStatus_t
 */
extern sbStatus_t
soc_sbx_g2xx_pp_oam_up_match_set (int unit, uint32 uValid);

extern sbStatus_t
soc_sbx_g2xx_pp_queue_state_initialize(soc_sbx_g2xx_state_t *pFe);

extern sbStatus_t
soc_sbx_g2xx_pp_cam_reset(int unit);

extern sbStatus_t
soc_sbx_g2xx_pp_cam_default_init(int unit);

extern sbStatus_t
soc_sbx_g2xx_pp_cam_db_recover(int unit);

extern sbStatus_t
soc_sbx_g2xx_pp_hash_templates_recover(soc_sbx_g2xx_state_t *pFe);

extern sbStatus_t
soc_sbx_g2xx_pp_hash_templates_init(soc_sbx_g2xx_state_t *pFe);

#endif
