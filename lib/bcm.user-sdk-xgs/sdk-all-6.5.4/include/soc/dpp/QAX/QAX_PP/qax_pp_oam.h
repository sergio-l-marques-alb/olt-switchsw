/*
 * $Id: $
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
 * File: qax_pp_oam.h
 */

#ifndef __QAX_PP_OAM_INCLUDED__
#define __QAX_PP_OAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER      0
#define _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER      1
#define _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF            2
#define _QAX_PP_OAM_OEM1_KEY_SEL_NULL               3

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* Same conversion as in JER */
#define _QAX_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(mdl_mp_type) _JER_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(mdl_mp_type)

/* MDL-MP-TYPE encoding - Same as JER */
#define _QAX_PP_OAM_MDL_MP_TYPE_NO_MP               _JER_PP_OAM_MDL_MP_TYPE_NO_MP
#define _QAX_PP_OAM_MDL_MP_TYPE_MIP                 _JER_PP_OAM_MDL_MP_TYPE_MIP
#define _QAX_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH        _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH
#define _QAX_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH       _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH

/* OEM1 Key select payload by components */
#define _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(_key_sel_inner, _key_sel_outer, _mp_profile_sel, _your_disc_inner, _your_disc_outer) \
            (((_key_sel_inner)<<5)|((_key_sel_outer)<<3)|((_mp_profile_sel)<<2)|((_your_disc_inner)<<1)|(_your_disc_outer))

/* OEM1 Key select key components ({OAM-LIF-Outer-Valid, OAM-LIF-Inner-Valid, Lif-Equal-To-OAM-LIF-Outer, Lif-Equal-To-OAM-LIF-Inner, Your-Disc})*/
#define _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_OUTER_VALID(_key)      (((_key)&(1<<4))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_INNER_VALID(_key)      (((_key)&(1<<3))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(_key)  (((_key)&(1<<2))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_INNER(_key)  (((_key)&(1<<1))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(_key)                (((_key)&(1<<0))!=0)

/* OEM1 Key select key components ({OAM-LIF-Outer-Valid, OAM-LIF-Inner-Valid, Packet-Is-OAM, Counter-Pointer-Valid, OAM-Injection})*/
#define _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_OUTER_VALID(_key)       (((_key)&(1<<4))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_INNER_VALID(_key)       (((_key)&(1<<3))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_PACKET_IS_OAM(_key)             (((_key)&(1<<2))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_COUNTER_PTR_VALID(_key)         (((_key)&(1<<1))!=0)
#define _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_INJECTION(_key)             (((_key)&(1<<0))!=0)

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

/* QAX specific OAM initialization */
soc_error_t soc_qax_pp_oam_init(int unit);

/* Punt handling profiles for QAX */
uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                 profile_ndx,
    SOC_SAND_IN SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  );

uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  );


/**
 * Configure OAMP supported trap codes per endpoint type
 * 
 * @author sinai (24/12/2015)
 * 
 * @param unit 
 * @param mep_type Endpoint type that is associated with the given trap code
 * @param trap_code Trap code that will be recognized by the OAMP as valid trap code.
 * 
 * @return soc_error_t 
 */
soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    );

soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) ;


soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_set(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  );

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  );

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_set(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 d_excess_thresh,
    SOC_SAND_IN  uint8                                 clr_low_thresh,
    SOC_SAND_IN  uint8                                 clr_high_thresh,
    SOC_SAND_IN  uint8                                 num_entry
  );

  soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_INOUT  uint8                                 *d_excess_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_low_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_high_thresh,
    SOC_SAND_INOUT  uint8                                 *num_entry
  );


soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_scanner_set(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint8                           scan_index
  );

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                rmep_index,
    SOC_SAND_IN  uint32                y1711_sd_sf_id,
    SOC_SAND_IN  uint32                sd_sf_1711_db_format,
    SOC_SAND_IN  uint8                 ccm_tx_rate,
    SOC_SAND_IN  uint8                 alert_method
  );

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY  *sd_sf_1711_entry
  );

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   sd_sf_db_index,
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY       *sd_sf_entry
  );

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                sd_sf_db_index,    
     SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY   *sd_sf_entry
  );

soc_error_t
  soc_qax_pp_oam_oamp_rmep_db_ext_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY  *rmep_db_ext_entry
  );


soc_error_t
  soc_qax_pp_oam_slm_set(
    int unit,
    int is_slm
  );

/* } */
#endif /* __QAX_PP_OAM_INCLUDED__ */
