/* $Id: ppc_api_lif.h,v 1.28 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_profile_mgmt.h
*
* MODULE PREFIX:  soc_ppc_api_profile_mgmt
*
* FILE DESCRIPTION: 
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PPC_API_PROFILE_MGMT_INCLUDED__
/* { */
#define __SOC_PPC_API_PROFILE_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/dpp_config_imp_defs.h>

#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS 4
#define SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_NOF_BITS(unit) (SOC_DPP_IMP_DEFS_GET(unit, outlif_profile_nof_bits))
#define SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_NOF_BITS_MAX 6 /* Maximal number of Outlif bits (used only for diagnostic print) */
#define SOC_PPC_PROFILE_MGMT_RIF_PROFILE_NOF_BITS 1 /*TBD*/
/* We use only 2 bits of inlif profile */
#define SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS   (4)
/* This is a max of a bitmap */
#define SOC_PPC_PROFILE_MGMT_EG_FILTER_INLIF_PROFILE_BITMAP_MAX         ((1 << SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) - 1)

#define SOC_PPC_PROFILE_MGMT_NOF_DSCP_BITS 2

/* Inlif profile management. */
typedef enum {
    /* EVB / MPLS MP (Same interface filtering) - used in Arad, Arad+, Jericho */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_SAME_INTERFACE = 0,
    /* OAM default lif profile - used in Jericho */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_OAM,
    /* PWE tagged mode - used in Jericho */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_TAGGED_MODE,
    /* DSCP/EXP marking profile when bridging - used in Arad+, Jericho */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_DSCP_MARKING,
    /* URPF - used in Arad */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_STRICT_URPF,   
    /* The user part of the inlif profile. */
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_USER,
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_NOF
} SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE;

/* Outlif profile management. */
typedef enum {
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_SIMPLE_SAME_INTERFACE = 0,
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_OAM,
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_TAGGED_MODE,
    /* The user part of the outlif profile. */
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_USER,
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_NOF
} SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE;

/* Rif profile management - TBD */
typedef enum {
    /* The user part of the rif profile. */
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE_USER = 0,
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE_NOF
} SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE;

/* Tree types of interfaces: InLif, OutLif, Rif */
typedef enum {
    /* The user part of the rif profile. */
    SOC_PPC_PROFILE_MGMT_TYPE_INLIF = 0,
    SOC_PPC_PROFILE_MGMT_TYPE_OUTLIF,
    SOC_PPC_PROFILE_MGMT_TYPE_RIF
} SOC_PPC_PROFILE_MGMT_TYPE;


/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
const char *
    SOC_PPC_INLIF_PROFILE_TYPE_to_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE enum_val
    );

const char *
    SOC_PPC_INLIF_PROFILE_TYPE_to_short_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE enum_val
    );

const char *
    SOC_PPC_OUTLIF_PROFILE_TYPE_to_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE enum_val
    );

const char *
    SOC_PPC_OUTLIF_PROFILE_TYPE_to_short_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE enum_val
    );

const char *
    SOC_PPC_RIF_PROFILE_TYPE_to_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE enum_val
    );

const char *
    SOC_PPC_RIF_PROFILE_TYPE_to_short_string(
       SOC_SAND_IN SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE enum_val
    );



/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_LIF_INCLUDED__*/
#endif
