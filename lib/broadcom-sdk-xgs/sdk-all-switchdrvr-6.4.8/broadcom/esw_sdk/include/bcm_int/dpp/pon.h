/*
 * $Id: l2.h,v 1.26 Broadcom SDK $
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
 * File:        pon.h
 * Purpose:     PON internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_PON_H_
#define   _BCM_INT_DPP_PON_H_

#include <soc/dpp/PPD/ppd_api_general.h>


/*
 * MACROs for PON
 */
/* given gport ID is it working port*/
#define _BCM_PPD_GPORT_IS_WORKING_PORT_ID(gport) (((gport)%2) == 0)

/* number of PON ports */
#define _BCM_PPD_NOF_PON_PHY_PORT (8)

/* number of PON PP ports */
#define _BCM_PPD_NOF_PON_PP_PORT  (128)

#define _BCM_PPD_IS_PON_PP_PORT(pon_pp_port) (((pon_pp_port) >= 0) && ((pon_pp_port) < _BCM_PPD_NOF_PON_PP_PORT))

/* PON channel default profile */
#define _BCM_PPD_PON_CHANNEL_DEFAULT_PROFILE (0)

/* default number of PON channel profiles */
#define _BCM_PPD_DEFAULT_NOF_PON_CHANNEL_PROFILE (16)

/* number of PON channel profiles when PON port is channelized */
#define _BCM_PPD_NOF_PON_CHANNEL_PROFILE(nof_channels) (16/(1<<((nof_channels)-1)))

/* Default offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_DEFAULT_OFFSET (3)

/*  offset of PON channel profiles */
#define _BCM_PPD_GPORT_PON_PP_PORT_CHANNEL_PROFILE_OFFSET(nof_channels) (3+((nof_channels)-1))

/* given PON PP port returns physical port */
#define _BCM_PPD_GPORT_PON_TO_PHY_PORT(pon_pp_port) ((pon_pp_port)&0x7)

/* given PON PP port returns local port */
#define _BCM_PPD_GPORT_PON_TO_LOCAL_PORT(pon_pp_port, offset) ((pon_pp_port)&((1<<(offset))-1))

/* given PON PP port returns PON channel profile */
#define _BCM_PPD_GPORT_PON_PP_PORT_TO_CHANNEL_PROFILE(pon_pp_port, offset) (((pon_pp_port)>>(offset))&0xf)

/* given PON port and PON channel profile returns PON PP port */
#define _BCM_PPD_GPORT_PON_CHANNEL_PROFILE_TO_PON_PP_PORT(pon_port, pon_channel_profile, offset) (((pon_channel_profile)<<(offset))|(pon_port)) 


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
int _bcm_dpp_pon_port_is_pon_port(int unit, bcm_port_t port, int *is_pon_port);

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
int _bcm_dpp_pon_lif_is_3_tags_data(int unit, int out_lif_id, int *is_3_tags_data);

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
    int pcp, bcm_tunnel_id_t tunnel_id, uint32 out_ac_id);

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
    int *pcp, bcm_tunnel_id_t *tunnel_id, int *out_ac_id);

/*
 * Function:
 *      _bcm_dpp_gport_delete_global_and_local_lif
 * Purpose:
 *       Given global and local lif, removes them. If remove_glem_entry is set, removes the glem entry according to global lif as well.
 * Parameters:
 *      unit                - (IN) Device Number
 *      global_lif          - (IN) Global lif to be deallocated.
 *      local_inlif_id      - (IN) Local inlif to be deallocated.
 *      local_outlif_id     - (IN) Local outlif to be deallocated.
 *      remove_glem_entry   - (IN) True will remove Glem entry from HW, false will not.
 *  
 * Returns:
 *      BCM_E_XXX
 */

#endif /* _BCM_INT_DPP_PON_H_ */
