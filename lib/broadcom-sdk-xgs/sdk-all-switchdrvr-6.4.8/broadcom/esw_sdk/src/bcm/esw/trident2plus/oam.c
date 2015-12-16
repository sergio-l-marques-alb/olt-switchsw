/* $Id: oam.c,v 1.0.0 Broadcom SDK $
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
 * File:
 *  oam.c
 *
 * Purpose:
 *  OAM implementation for trident2p family of devices.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/hash.h>

#include <bcm/oam.h>
#include <bcm/field.h>

#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/oam.h>


#define _BCM_TD2P_OLP_HDR_TYPE_RX    1

/* OLP_HDR_SUBTYPE filled with one of the following values */
/* Down mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM             0x01
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM              0x02
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM              0x03
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC        0x04
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM                 0x05
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM                  0x06
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM                  0x07
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC            0x08
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BFD                     0x09
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM           0x0A
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM            0x0B
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM        0x0C
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM           0x0D
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM        0x0E
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT                     0x0F
#define _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH                 0x10

/* Up mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP             0x11
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP              0x12
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP              0x13
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP        0x14
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP                     0x15

typedef struct _bcm_td2p_olp_hdr_type_mapping_s {
    bcm_field_olp_header_type_t     field_olp_hdr_type;
    uint8                           subtype;
} _bcm_td2p_olp_hdr_type_mapping_t;

STATIC _bcm_td2p_olp_hdr_type_mapping_t td2p_olp_hdr_type_mapping[] = {

    /* Field OLP header type to Subtype mapping table
     * Note: Do not change the order, as EGR_OLP_HEADER_TYPE_MAPPING table is
     *       programmed in the same order.
     */

/* BFD */
{bcmFieldOlpHeaderTypeBfdOam, _BCM_TD2P_OLP_HDR_SUBTYPE_BFD},

/* ETH OAM Down MEP*/
{bcmFieldOlpHeaderTypeEthOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM},
{bcmFieldOlpHeaderTypeEthOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM},
{bcmFieldOlpHeaderTypeEthOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM},
{bcmFieldOlpHeaderTypeEthOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC},

/* BHH */
{bcmFieldOlpHeaderTypeBhhOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM},
{bcmFieldOlpHeaderTypeBhhOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM},
{bcmFieldOlpHeaderTypeBhhOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM},
{bcmFieldOlpHeaderTypeBhhOamOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC},

/* MPLS LM/DM */
{bcmFieldOlpHeaderTypeRfc6374Dlm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM},
{bcmFieldOlpHeaderTypeRfc6374Dm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM},
{bcmFieldOlpHeaderTypeRfc6374DlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM},
{bcmFieldOlpHeaderTypeRfc6374Ilm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM},
{bcmFieldOlpHeaderTypeRfc6374IlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM},

/* Down SAT */
{bcmFieldOlpHeaderTypeSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT},

/* Other ACH */
{bcmFieldOlpHeaderTypeOtherAch, _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH},

/* ETH-OAM Up MEP */
{bcmFieldOlpHeaderTypeEthOamUpMepCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP},

/* Up SAT*/
{bcmFieldOlpHeaderTypeUpSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP},

};

static uint8 td2p_olp_hdr_type_count = sizeof(td2p_olp_hdr_type_mapping) /
                                           sizeof(td2p_olp_hdr_type_mapping[0]);

/* Magic port used for remote OLP over HG communication */
#define _BCM_OAM_TD2P_OLP_MAGIC_PORT 0x7F

/*
 * Function:
 *    _bcm_td2p_oam_olp_header_type_mapping_set
 * Purpose:
 *     Miscellaneous OAM configurations:
 *         1. Enable IFP lookup on the CPU port.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_olp_header_type_mapping_set(int unit)
{
    int                                 index           = 0;
    int                                 mem_index_count = 0;
    egr_olp_header_type_mapping_entry_t entry;

    mem_index_count = soc_mem_index_count(unit, EGR_OLP_HEADER_TYPE_MAPPINGm);

    if (td2p_olp_hdr_type_count > mem_index_count) {
        return BCM_E_RESOURCE;
    }

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                     &entry,
                                                     HDR_TYPEf,
                                                     _BCM_TD2P_OLP_HDR_TYPE_RX);

        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(
                                            unit,
                                            &entry,
                                            HDR_SUBTYPEf,
                                            td2p_olp_hdr_type_mapping[index].subtype);

        SOC_IF_ERROR_RETURN(
                     WRITE_EGR_OLP_HEADER_TYPE_MAPPINGm(unit,  MEM_BLOCK_ALL,
                                                        index, &entry));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_oam_hg_olp_enable
 * Purpose:
 *    Enable OLP on HG ports by default
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_hg_olp_enable(int unit)
{
    bcm_pbmp_t                     ports;
    bcm_port_t                     port;
    iarb_ing_port_table_entry_t    entry;

    BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));

    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PORT_TABLEm,
                                             MEM_BLOCK_ANY, port, &entry));

            soc_IARB_ING_PORT_TABLEm_field32_set(unit, &entry, OLP_ENABLEf, 1);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PORT_TABLEm,
                                              MEM_BLOCK_ALL, port, &entry));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_td2p_oam_olp_magic_port_set
 * Purpose:
 *     Set Magic port used for remote OLP over HG communication
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_olp_magic_port_set(int unit)
{
    int    modid;

    /* Configure modid and the magic port */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));

    SOC_IF_ERROR_RETURN(
          soc_reg_field32_modify(unit, IARB_OLP_CONFIG_1r,
                                 REG_PORT_ANY, MY_MODIDf, modid));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                               IARB_OLP_CONFIG_1r,
                                               REG_PORT_ANY,
                                               MY_PORT_NUMf,
                                               _BCM_OAM_TD2P_OLP_MAGIC_PORT));

    return (BCM_E_NONE);
}

/* * * * * * * * * * * * * * * * * * * *
 *            OAM BCM APIs             *
 * * * * * * * * * * * * * * * * * * * */
/*
 * Function: bcm_td2p_oam_init
 *
 * Purpose:
 *     Initialize OAM module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     CM_E_XXX     - Error code from bcm_XX_oam_init()
 *     BCM_E_NONE    - Success
 */
int
bcm_td2p_oam_init(int unit)
{
    int rv;             /* Operation return value.    */

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) 
#endif    
    {
        /* Set default olp header type mapping */
        rv = _bcm_td2p_oam_olp_header_type_mapping_set(unit); 
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        /* Enable OLP on HG ports */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_hg_olp_enable(unit));

        /* Set Magic port used for remote OLP over HG communication */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_olp_magic_port_set(unit));
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_td2p_oam_olp_fp_hw_index_get
 *
 * Purpose:
 *     Get OLP_HDR_TYPE_COMPRESSED corresponding to subtype
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_fp_hw_index_get(int unit,
                                  bcm_field_olp_header_type_t olp_hdr_type,
                                  int *hwindex)
{
    int index;

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        if (td2p_olp_hdr_type_mapping[index].field_olp_hdr_type == olp_hdr_type) {
            *hwindex = index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function: _bcm_td2p_oam_olp_hw_index_olp_type_get
 *
 * Purpose:
 *     Get subtype corresponding to OLP_HDR_TYPE_COMPRESSED
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_hw_index_olp_type_get(int unit,
                                        int hwindex,
                                        bcm_field_olp_header_type_t *olp_hdr_type)
{
    if (hwindex >= td2p_olp_hdr_type_count) {
        return BCM_E_PARAM;
    }

    *olp_hdr_type = td2p_olp_hdr_type_mapping[hwindex].field_olp_hdr_type;

    return BCM_E_NONE;
}
