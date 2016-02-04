/*
 * $Id: oam_dissect.h,v 1.23 Broadcom SDK $
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
 * OAM input analysis and validation internal header
 */

#ifndef _BCM_INT_DPP_OAM_DISSECT_H_
#define _BCM_INT_DPP_OAM_DISSECT_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>


/*
 * Defines and macros
 */

/* Trap code used for snooping to the CPU while keeping the original system-headers */
#define _BCM_OAM_TRAP_CODE_CPU_KEEP_ORIG_HEADERS(_trap_code)        ((_trap_code)==SOC_PPD_TRAP_CODE_OAM_CPU_SNOOP_UP)


#define _BCM_OAM_IS_SERVER_CLIENT(endpoint_info) (endpoint_info->tx_gport==BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)
#define _BCM_OAM_IS_SERVER_SERVER(endpoint_info) (endpoint_info->tx_gport!=BCM_GPORT_INVALID && endpoint_info->remote_gport !=BCM_GPORT_INVALID)

#define _BCM_OAM_IS_MEP_ID_DEFAULT(_unit, _epid)                                \
               (SOC_IS_ARAD_B1_AND_BELOW(_unit)                                 \
                  ? ((_epid) == -1)                                             \
                  : ((SOC_IS_ARADPLUS_AND_BELOW(_unit)||SOC_IS_JERICHO(_unit))  \
                      ? (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)       \
                         ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0))       \
/*QAX and above*/     : (((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS0)         \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS1)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS2)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_INGRESS3)      \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS0)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS1)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS2)       \
                          ||((_epid) == BCM_OAM_ENDPOINT_DEFAULT_EGRESS3))))

/* Is endpoint_info struct represents an accelerated endpoint */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(p_endpoint_info) \
                          (((p_endpoint_info)->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) == BCM_OAM_OPCODE_CCM_IN_HW)

/* Is endpoint_info struct represents an up-MEP */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_UPMEP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_UP_FACING) == BCM_OAM_ENDPOINT_UP_FACING)

/* Is endpoint_info struct represents a MIP */
#define _BCM_OAM_DISSECT_IS_ENDPOINT_MIP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) == BCM_OAM_ENDPOINT_INTERMEDIATE)

/* Is classifier struct represents an accelerated endpoint */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED) == SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED)

/* Is classifier struct represents an up-MEP */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_UPMEP(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP) == SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP)

/* Is classifier struct represents a MIP */
#define _BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(p_classifier_mep_entry) \
                          (((p_classifier_mep_entry)->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT) == 0)

/* Classification is done through the IHB_OAMB table in the following cases:
   1. Accelerated endpoint
   2. "Multiple endpoint on LIF" (Arad+), Endpoint only
   3. Server on the client side*/
#define _BCM_OAM_DISSECT_CLASSIFICATION_BY_OAM2(unit, p_classifier_mep_entry, endpoint_id)\
           (_BCM_OAM_DISSECT_IS_CLASSIFIER_MEP_ACCELERATED(p_classifier_mep_entry) ||  _BCM_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint_id) \
           || (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit) && !_BCM_OAM_DISSECT_IS_CLASSIFIER_MIP(p_classifier_mep_entry)) )

/*
 * Function declaration
 */


/* Validity checks used by default and non default endpoints alike. */
int _bcm_oam_endpoint_validity_checks_all(int unit, bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for all types of non-default endpoints */
int _bcm_oam_generic_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for OAMoETH endpoints */
int _bcm_oam_ethernet_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks for OAMoMPLS/PWE endpoints */
int _bcm_oam_mpls_pwe_endpoint_validity_checks(int unit, bcm_oam_endpoint_info_t *endpoint_info);

/* perform endpoint create validity check for default endpoint. */
int _bcm_oam_default_endpoint_validity_checks(int unit, const bcm_oam_endpoint_info_t *endpoint_info);

/* Validity checks before replacing a local MEP */
int _bcm_oam_endpoint_local_replace_validity_check(int unit, bcm_oam_endpoint_info_t *endpoint_info_lcl,
                                                   bcm_oam_endpoint_info_t *existing_endpoint_info,
                                                   SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

/* Validity checks before adding a loopback object to an endpoint (Arad+ only) */
int _bcm_oam_loopback_validity_check(int unit, bcm_oam_loopback_t *loopback_ptr,
                                     SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY *classifier_mep_entry);

#endif /* _BCM_INT_DPP_OAM_DISSECT_H_ */

