/*
 * $Id: oam.c,v 1.148 Broadcom SDK $
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
 * File:    pon.c
 * Purpose: Manages pon interface
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT

#include <shared/bsl.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/common/debug.h>


/*
 * Function:
 *      _bcm_dpp_pon_port_is_pon_port
 * Purpose:
 *      Check if the port is PON port
 * Parameters:
 *      unit        - (IN)  Device Number
 *      port        - (IN)  Device PP port Number
 *      is_pon_port - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_port_is_pon_port(int unit, bcm_port_t port, int *is_pon_port)
{
    if (is_pon_port == NULL) {
        return BCM_E_PARAM;
    }

    *is_pon_port = FALSE;

    if (SOC_IS_ARAD(unit) && SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
        if ((_BCM_PPD_IS_PON_PP_PORT(port)) && (IS_PON_PORT(unit,_BCM_PPD_GPORT_PON_TO_PHY_PORT(port)))) {
            *is_pon_port = TRUE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_dpp_pon_lif_is_3_tags_data
 * Purpose:
 *      Check if the given lif index is 3 TAGs manipulation.
 * Parameters:
 *      unit           - (IN)  Device Number
 *      out_lif_id     - (IN)  out lif index
 *      is_3_tags_data - (OUT) TRUE/FALSE
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_lif_is_3_tags_data(int unit, int lif_id, int *is_3_tags_data)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_EG_ENCAP_ENTRY_TYPE entry_type;
    
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(is_3_tags_data);

    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_eg_encap_entry_type_get(soc_sand_dev_id,
                                         lif_id,
                                         &entry_type);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* DATA entry for PON 3 TAGs manipulation */
    if (entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA) {
        *is_3_tags_data = TRUE;
    } else {
        *is_3_tags_data = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_add
 * Purpose:
 *       Add an EEDB entry with PON Tunnel info
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (IN)  Tpid for out Tunnel Tag
 *      pcp         - (IN)  Pcp for out Tunnel Tag
 *      tunnel_id   - (IN)  Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (IN)  Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_add(int unit, uint32 entry_index, uint16 tpid,
    int pcp, bcm_tunnel_id_t tunnel_id, uint32 out_ac_id)
{

    uint32 soc_sand_rv;
    SOC_PPD_EG_ENCAP_DATA_INFO data_info;

    BCMDNX_INIT_FUNC_DEFS;

    /* currently only support PCP 0 */
    if (pcp != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid pcp, it should be 0")));
    }

    unit = (unit);

    /* build data entry and fill with PON Tunnel info */
    SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_info);
    SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_SET(unit, tpid, pcp, tunnel_id, &data_info);
    
    /* add enry to allocated place */
    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, TRUE, out_ac_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_pon_encap_data_enty_get
 * Purpose:
 *       Get PON Tunnel info of an EEDB entry 
 * Parameters:
 *      unit        - (IN)  Device Number
 *      entry_index - (IN)  Data entry index
 *      tpid        - (OUT) Tpid for out Tunnel Tag
 *      pcp         - (OUT) Pcp for out Tunnel Tag
 *      tunnel_id   - (OUT) Tunnel_id for out Tunnel Tag
 *      out_ac_id   - (OUT) Out AC index
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_pon_encap_data_enty_get(int unit, uint32 entry_index, uint16 *tpid,
    int *pcp, bcm_tunnel_id_t *tunnel_id, int *out_ac_id)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES], nof_entries;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(tpid);
    BCMDNX_NULL_CHECK(pcp);
    BCMDNX_NULL_CHECK(tunnel_id);
    BCMDNX_NULL_CHECK(out_ac_id);

    soc_sand_dev_id = (unit);

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    *tpid = SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TPID_GET(soc_sand_dev_id, &encap_entry_info[0].entry_val.data_info);

    *pcp = SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_PCP_GET(soc_sand_dev_id, &encap_entry_info[0].entry_val.data_info);

    *tunnel_id = SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(soc_sand_dev_id, &encap_entry_info[0].entry_val.data_info);
    
    *out_ac_id = next_eep[0];

exit:
    BCMDNX_FUNC_RETURN;
}

