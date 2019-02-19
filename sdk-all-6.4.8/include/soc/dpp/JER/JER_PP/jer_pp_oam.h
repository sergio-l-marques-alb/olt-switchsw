/* $Id: arad_pp_oam.h,v 1.27 Broadcom SDK $
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
*/

#ifndef __JER_PP_OAM_INCLUDED__
/* { */
#define __JER_PP_OAM_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



/* use maximal mirror, forward strength*/
#define _JER_PP_OAM_MIRROR_STRENGTH 3
#define _JER_PP_OAM_FORWARD_STRENGTH 3

/* } */
/*************
 * MACROS    *
 *************/
/* { */



/* MDL-MP-TYPE encoding*/
#define _JER_PP_OAM_MDL_MP_TYPE_NO_MP 0
#define _JER_PP_OAM_MDL_MP_TYPE_MIP 1
#define _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH 2
#define _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH 3


/**
 * mp-type-vector is composed of 8 pair of bits, one pair per level.  Each pair of bits represent the mp-type. Getter and setter macros. 
 * Returns one of the defines above 
 */
#define JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(mp_type_vector,level) (((mp_type_vector) >> ((level)*2)) & 0x3)

#define JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(mp_type_vector,mp_type,level)\
do {\
    uint32 temp_mp_type_vector = mp_type_vector, mp_type_temp=mp_type;\
    SHR_BITCOPY_RANGE(&temp_mp_type_vector, (level)*2 ,&mp_type_temp,0,2);\
    mp_type_vector = temp_mp_type_vector;\
}\
while (0)


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


/**
 * Jerihco specific OAM initialization.
 * 
 * @author sinai (30/06/2014)
 * 
 * @param unit 
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_init(int unit);


/**
 * Sets the new OEM1-payload according to the previous payload and new MEP.
 * 
 * @author sinai (29/06/2014)
 * 
 * @param unit 
 * @param classifier_mep_entry 
 * @param prev_payload 
 * @param new_payload 
 * @param is_active 
 * @param update 
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_oem1_mep_add(
        int unit,
        const SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
        const SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * prev_payload,
        SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * new_payload,
        uint8 is_active,
        uint8 update);


/**
 * Adds a default profile to the OAM classifier
 *
 * @param unit            - (IN) Device to be configured.
 * @param mep_index       - (IN) profile mep id.
 * @param classifier_mep_entry - (IN) Classifier entry
 *                             information
 * @param update_action_only - (IN) is it an update for the
 * action
 *
 * @return soc_error_t
 */
soc_error_t
  soc_jer_pp_oam_classifier_default_profile_add(
     SOC_SAND_IN  int                                unit,
     SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
     SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
     SOC_SAND_IN  uint8                              update_action_only
  );

/**
 * Removes a default profile from the OAM classifier
 *
 * @param unit (IN) Device to be configured.
 * @param mep_index profile mep id.
 *
 * @return soc_error_t
 */
soc_error_t
  soc_jer_pp_oam_classifier_default_profile_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index
  );


/**
 * Sets an action for a default OAM endpoint
 *
 * @param unit Device to be configured.
 * @param mep_index profile mep id.
 * @param classifier_mep_entry Classifier entry information
 *
 * @return soc_error_t
 */
soc_error_t
  soc_jer_pp_oam_classifier_default_profile_action_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

/**
 * soc_jer_pp_oam_set_inlif_profile_map
 * Adds a mapping from an inlif profile (4b) to an OAM lif profile (2b)
 */
soc_error_t
  soc_jer_pp_oam_inlif_profile_map_set(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_IN  uint32                                               oam_profile
  );

soc_error_t
  soc_jer_pp_oam_inlif_profile_map_get(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_OUT uint32                                               *oam_profile
  );

/**
 * Read/Write on the MEP-Profile table.
 * 
 * @author sinai (30/06/2014)
 * 
 * @param unit 
 * @param profile_indx - must be between 0 and 128.
 * @param eth1731_profile 
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_set(
    int                                 unit,
    uint8                          profile_indx,
    const SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );

soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_get(
    int                                 unit,
    uint8                          profile_indx,
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );



/**
 * Initialize the TOD configurations at the ECI. Used for enabling NTP, 1588 timestamping.
 * 
 * @author sinai (30/06/2014)
 * 
 * @param unit
 * @param init_ntp - initialize the NTP block
 * @param init_1588 - initialize the IEEE_1588 block
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_init_eci_tod(
    int                                 unit,
   uint8                                init_ntp,
   uint8                                init_1588
   );



/**
 * Set the TOD configurations at the ECI. Used for setting NTP, 1588 timestamping.
 *
 * @author rmantelm (16/07/2015)
 *
 * @param unit
 * @param init_ntp - initialize the NTP block
 * @param data - values to be set: LO 32 bits - time_frac, Hi 32 bits - time_sec
 *
 * @return uint32
 */
uint32 soc_jer_pp_oam_tod_set(
    int                                 unit,
   uint8                                is_ntp,
   uint64                               data
   );



/**
 * Set MSBs of global SA address of outgoing PDUs.
 * 
 * @author sinai (26/06/2014)
 * 
 * @param unit 
 * @param profile - 0 or 1 
 * @param msbs - Assumed to be an array of 5 bytes
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_sa_addr_msbs_set(
    int unit,
    int profile,
   const uint8 * msbs);


/**
 * Get MSBs of global SA address of outgoing PDUs.
 * 
 * @author sinai (26/06/2014)
 * 
 * @param unit 
 * @param profile - 0 or 1 
 * @param msbs - Assumed to be an array of 5 bytes
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_sa_addr_msbs_get(
    int unit,
    int profile,
    uint8 * msbs);



/**
 * Trigger a one shot DMM, per endpoint. 
 * 
 * @author sinai (08/07/2014)
 * 
 * @param unit 
 * @param endpoint_id 
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_dm_trigger_set(
   int unit,
   int endpoint_id);



/**
 * Set the table EPNI_CFG_MAPPING_TO_OAM_PCP. 
 *  Determines the mapping of
 *  outlif profile + packet TC ---> OAM PCP.
 *  
 * 
 * @author sinai (14/10/2014)
 * 
 * @param unit 
 * @param tc 
 * @param outlif_profile 
 * @param oam_pcp 
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_egress_pcp_set_by_profile_and_tc(
   int unit,
   uint8 tc,
   uint8 outlif_profile,
   uint8 oam_pcp);

/* Init sat on appl */
soc_error_t soc_jer_pp_oam_sat_init(int unit);

/* config tst/lb sat header*/  
 soc_error_t 
    soc_jer_pp_oam_oamp_lb_tst_header_set (SOC_SAND_IN int unit, 
     SOC_SAND_INOUT uint8* header_buffer, 
     SOC_SAND_IN uint32 endpoint_id, 
     SOC_SAND_IN bcm_mac_t dest_mac,
     SOC_SAND_IN int flag,
     SOC_SAND_OUT int *header_offset);

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_OAM_INCLUDED__*/
#endif
