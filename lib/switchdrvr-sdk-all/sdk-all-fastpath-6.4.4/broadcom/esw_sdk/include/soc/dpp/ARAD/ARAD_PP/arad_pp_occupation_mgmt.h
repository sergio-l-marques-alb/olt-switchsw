/* $Id: arad_pp_occupation_mgmt.h,v 1.28 Broadcom SDK $
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
*/
#ifndef __ARAD_PP_OCCUPATION_MGMT_INCLUDED__
/* { */
#define __ARAD_PP_OCCUPATION_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/dpp_config_imp_defs.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_OCC_MGMT_FLAGS_NONE                 (0)
#define SOC_OCC_MGMT_APP_USER                   (-1)

#define SOC_OCC_MGMT_INLIF_SIZE         (4)
#define SOC_OCC_MGMT_OUTLIF_SIZE(unit)  (SOC_DPP_IMP_DEFS_GET(unit, outlif_profile_nof_bits))
#define SOC_OCC_MGMT_OUTLIF_SIZE_MAX    (6) /* Maximal number of Outlif bits (used only for diagnostic print) */
#define SOC_OCC_MGMT_RIF_SIZE           (1)

/* Two inlif profile bits are transferred to the egress */
#define SOC_OCC_MGMT_NOF_INLIF_BITS_TRANSFERED_TO_EGRESS        (2)
/* These two bits have a total of four different configurations */
#define SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS    (4)
/* This is a max of a bitmap */
#define SOC_OCC_MGMT_EG_FILTER_INLIF_PROFILE_BITMAP_MAX         ((1 << SOC_OCC_MGMT_NOF_INLIF_BITS_TRANSFERED_TO_EGRESS) - 1)

/* } */
/*************
 *   ENUMS   *
 *************/
/* { */
/* Inlif occupation management. */
typedef enum {
    /* EVB / MPLS MP (Same interface filtering) - used in Arad, Arad+, Jericho */
    SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE = 0,
    /* OAM default lif occupation - used in Jericho */
    SOC_OCC_MGMT_INLIF_APP_OAM,
    /* PWE tagged mode - used in Jericho */
    SOC_OCC_MGMT_INLIF_APP_TAGGED_MODE,
    /* DSCP/EXP marking occupation when bridging - used in Arad+, Jericho */
    SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING,
    /* URPF - used in Arad */
    SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, 
    /* Split Horizon Orientation - used in Jericho */
    SOC_OCC_MGMT_INLIF_APP_ORIENTATION,  
    SOC_OCC_MGMT_INLIF_APP_NOF
} SOC_OCC_MGMT_INLIF_APP;

/* Outlif occupation management. */
typedef enum {
    SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP = 0,
    SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP,
    SOC_OCC_MGMT_OUTLIF_APP_TAGGED_MODE,
    /* Split Horizon Orientation - used in Jericho */
    SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
    /* EVPN - used in Jericho */
    SOC_OCC_MGMT_OUTLIF_APP_EVPN,
    SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL,
    SOC_OCC_MGMT_OUTLIF_APP_NOF
} SOC_OCC_MGMT_OUTLIF_APP;

/* Rif occupation management - TBD */
typedef enum {
    SOC_OCC_MGMT_RIF_APP_NOF
} SOC_OCC_MGMT_RIF_APP;

/* Types of occupation maps */
typedef enum {
    SOC_OCC_MGMT_TYPE_INLIF = 0,
    SOC_OCC_MGMT_TYPE_OUTLIF,
    SOC_OCC_MGMT_TYPE_RIF,
    SOC_OCC_MGMT_TYPE_NOF
} SOC_OCC_MGMT_TYPE;

/************* 
 * STRUCTS
 *************/
typedef struct {
    /* In all profiles allocate one extra bit */
    int inlif_profile  [SOC_OCC_MGMT_INLIF_SIZE + 1];
    int outlif_profile [SOC_DPP_IMP_DEFS_MAX(OUTLIF_PROFILE_NOF_BITS) + 1];
    int rif_profile    [SOC_OCC_MGMT_RIF_SIZE + 1];
} ARAD_PP_OCCUPATION;

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

soc_error_t
arad_pp_occ_mgmt_init (int unit);

soc_error_t
arad_pp_occ_mgmt_deinit (int unit);

soc_error_t
arad_pp_occ_mgmt_get_app_mask(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   SHR_BITDCL                  *mask);

soc_error_t
arad_pp_occ_mgmt_app_get(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   SHR_BITDCL                  *full_occupation,
   uint32                      *val);

soc_error_t
arad_pp_occ_mgmt_app_set(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   uint32                      val,
   SHR_BITDCL                  *full_occupation);

soc_error_t 
arad_pp_occ_mgmt_diag_info_get(int unit, SOC_OCC_MGMT_TYPE occ_type, char* apps[]);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_OCCUPATION_MGMT_INCLUDED__*/
#endif
