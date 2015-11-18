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


typedef struct _bcm_oam_td2p_olp_hdr_type_map_s {
    uint32  mem_index;
    uint32  hdr_type;
    uint32  hdr_subtype; 
} _bcm_oam_td2p_olp_hdr_type_map_t;

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
    int rv = BCM_E_NONE;
    int index = 0;
    int entry_mem_size;    /* Size of table entry. */
    int entry_count = 0;

    egr_olp_header_type_mapping_entry_t *entry_buf;
    egr_olp_header_type_mapping_entry_t *entry;
    _bcm_oam_td2p_olp_hdr_type_map_t olp_hdr_type;

    _bcm_oam_td2p_olp_hdr_type_map_t olp_hdr_type_map[] = {

        /* DOWNMEP ENTRIES */

        /* BFD */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_BFD_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_BFD
        },

        /* ETH OAM */
        /* CCM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_CCM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_CCM
        },
        /* LM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_LM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_LM
        },
        /* DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_DM
        },
        /* Other opcodes */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_OTHER_OC_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_OTHER_OC
        },

        /* BHH OAM */
        /* CCM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_BHH_CCM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_BHH_CCM
        },
        /* LM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_BHH_LM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_BHH_LM
        },
        /* DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_BHH_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_BHH_DM
        },
        /* Other opcodes */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_BHH_OTHER_OC_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_BHH_OTHER_OC
        },

        /* MPLS LM DM */
        /* DLM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_MPLS_LMDM_DLM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_MPLS_LMDM_DLM
        },
        /* DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_MPLS_LMDM_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_MPLS_LMDM_DM
        },
        /* DLM + DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_MPLS_LMDM_DLM_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_MPLS_LMDM_DLM_DM
        },
        /* ILM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_MPLS_LMDM_ILM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_MPLS_LMDM_ILM
        },
        /* ILM + DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_MPLS_LMDM_ILM_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_MPLS_LMDM_ILM_DM
        },

        /* SAT */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_SAT_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_SAT
        },
        /* OTH ACH */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                _BCM_TD2P_OTH_ACH_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_OTH_ACH
        },

        /* UP Mep Entries */

        /* ETH OAM */
        /* CCM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_UPMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_CCM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_CCM_UP
        },
        /* LM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_UPMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_LM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_LM_UP
        },
        /* DM */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_UPMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_DM_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_DM_UP
        },
        /* Other opcodes */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_UPMEP_KEY_FORM(
                _BCM_TD2P_ETH_OAM_OTHER_OC_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_ETH_OAM_OTHER_OC_UP
        },
        /* SAT */
        {_BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_UPMEP_KEY_FORM(
                _BCM_TD2P_SAT_OLP_HDR_TYPE_COMPRESSED),
        _BCM_TD2P_OLP_HEADER_TYPE_RX, _BCM_TD2P_OLP_HEADER_SUBTYPE_SAT_UP
        },

        /* 0xff indicates end of table */ 
        {0xff}
    };

    entry_count = soc_mem_index_count(unit, EGR_OLP_HEADER_TYPE_MAPPINGm);
    entry_mem_size = sizeof(egr_olp_header_type_mapping_entry_t);
    /* Allocate buffer to store the DMAed table entries. */
    entry_buf = soc_cm_salloc(unit, entry_mem_size * entry_count,
            "olp header type mapping buffer");
    if (NULL == entry_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(entry_buf, 0, sizeof(entry_mem_size) * entry_count);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, MEM_BLOCK_ALL,
            0, (entry_count-1), entry_buf);
    if (BCM_FAILURE(rv)) {
        if (entry_buf) {
            soc_cm_sfree(unit, entry_buf);
        }
        return rv;
    }

    for (index = 0; ;index++) { 
        olp_hdr_type = olp_hdr_type_map[index];

        if(olp_hdr_type.mem_index == 0xff) {
            /* End of table */
            break;
        }
        if (olp_hdr_type.mem_index >= entry_count) {
            soc_cm_sfree(unit, entry_buf);
            return BCM_E_INTERNAL;
        }
        entry = soc_mem_table_idx_to_pointer (unit, 
                EGR_OLP_HEADER_TYPE_MAPPINGm, 
                egr_olp_header_type_mapping_entry_t *,
                entry_buf, olp_hdr_type.mem_index);
        soc_mem_field_set(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
                (uint32 *)entry, HDR_TYPEf, 
                &(olp_hdr_type.hdr_type));
        soc_mem_field_set(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
                (uint32 *)entry, HDR_SUBTYPEf, 
                &(olp_hdr_type.hdr_subtype));
    }

    rv = soc_mem_write_range(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
            MEM_BLOCK_ALL, 0, (entry_count - 1), entry_buf); 
    soc_cm_sfree(unit, entry_buf);
    return rv;
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
    }
    return (BCM_E_NONE);
}

int
_bcm_td2p_oam_olp_fp_hw_index_get(
        int unit,
        bcm_field_olp_header_type_t olp_hdr_type,
        int *hwindex)
{
    switch(olp_hdr_type) {
        case bcmFieldOlpHeaderTypeEthCC:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_ETH_OAM_CCM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeEthOthers:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_ETH_OAM_OTHER_OC_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeBfdOam:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_BFD_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeEthOamLm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_ETH_OAM_LM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeEthOamDm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_ETH_OAM_DM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeBhhOamCcm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_BHH_CCM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeBhhOamLm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_BHH_LM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeBhhOamDm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_BHH_DM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeBhhOamOthers:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_BHH_OTHER_OC_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeRfc6374Dlm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_MPLS_LMDM_DLM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeRfc6374Dm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_MPLS_LMDM_DM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeRfc6374DlmPlusDm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_MPLS_LMDM_DLM_DM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeRfc6374Ilm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_MPLS_LMDM_ILM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeRfc6374IlmPlusDm:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_MPLS_LMDM_ILM_DM_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeSat:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_SAT_OLP_HDR_TYPE_COMPRESSED);
            break;
        case bcmFieldOlpHeaderTypeOtherAch:
            *hwindex = _BCM_TD2P_OAM_OLP_HDR_TYPE_MAP_DOWNMEP_KEY_FORM(
                    _BCM_TD2P_OTH_ACH_OLP_HDR_TYPE_COMPRESSED);
            break;

        default:
            return BCM_E_NOT_FOUND;       
            break;
    }
    return BCM_E_NONE;
}


int
_bcm_td2p_oam_olp_hw_index_olp_type_get(
        int unit,
        int hwindex,
        bcm_field_olp_header_type_t *olp_hdr_type )
{

    switch(hwindex) {
        case _BCM_TD2P_BFD_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeBfdOam;
            break;
        case _BCM_TD2P_ETH_OAM_CCM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeEthCC;
            break;
        case _BCM_TD2P_ETH_OAM_LM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeEthOamLm;
            break;
        case _BCM_TD2P_ETH_OAM_DM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeEthOamDm;
            break;
        case _BCM_TD2P_ETH_OAM_OTHER_OC_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeEthOthers;
            break;
        case _BCM_TD2P_BHH_CCM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeBhhOamCcm;
            break;
        case _BCM_TD2P_BHH_LM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeBhhOamLm;
            break;
        case _BCM_TD2P_BHH_DM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeBhhOamDm;
            break;
        case _BCM_TD2P_BHH_OTHER_OC_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeBhhOamOthers;
            break;
        case _BCM_TD2P_MPLS_LMDM_DLM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeRfc6374Dlm;
            break;
        case _BCM_TD2P_MPLS_LMDM_DM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeRfc6374Dm;
            break;
        case _BCM_TD2P_MPLS_LMDM_DLM_DM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeRfc6374DlmPlusDm;
            break;
        case _BCM_TD2P_MPLS_LMDM_ILM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeRfc6374Ilm;
            break;
        case _BCM_TD2P_MPLS_LMDM_ILM_DM_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeRfc6374IlmPlusDm;
            break;
        case _BCM_TD2P_SAT_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeSat;
            break;
        case _BCM_TD2P_OTH_ACH_OLP_HDR_TYPE_COMPRESSED:
            *olp_hdr_type = bcmFieldOlpHeaderTypeOtherAch;
            break;
        default:
            return BCM_E_NOT_FOUND;
            break;
    }
    return BCM_E_NONE;
}
