/* $Id: soc_pb_profile.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_PROFILE_INCLUDED__
/* { */
#define __SOC_PB_PROFILE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  /*
   *  TM-Port index in case of Port-Egress-Profile
   *  (SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR).
   *  Range: 0 - 79.
   */
  uint32 port_ndx;
  /*
   *  PMF-Program (SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE).
   *  Range: 0 - 31.
   */
  uint32 pmf_pgm_ndx;
  /*
   *  PP-Port index in case of PMF-Port-Profile
   *  SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE
   *  Range: 0 - 63.
   */
  uint32 pp_port_ndx;
  /*
   * PP-Port and PFG group (i.e., Lookup Profiles)
   * for the SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE
   */
  SOC_PB_PMF_PGM_MGMT_NDX pmf_pgm_mgmt_ndx;

} SOC_PB_PROFILE_NDX;

typedef struct
{
  /*
   *  Profile in case of Port-Egress-Profile (SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR).
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE eg_profile;
  /*
   *  Header profile (SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE).
   */
  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA header_profile;
  /*
   *  Header profile (SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE).
   */
  SOC_PB_PORT_PP_PORT_INFO pp_port_info;
  /*
   * PP Port info and PFG info for PMF Program
   * (SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE)
   */
  SOC_PB_PMF_PGM_MGMT_PARAMS pmf_pgm_mgmt_params;

} SOC_PB_PROFILE_PARAMS;


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

uint32
  soc_pb_profile_to_hw_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_IN  SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_IN  uint32                    internal_hw_ndx,
    SOC_SAND_IN  uint8                    first_appear
  );

uint32
  soc_pb_profile_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_OUT SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_OUT uint32                     *internal_hw_ndx
  );

uint32
  soc_pb_profile_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_IN  SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_IN  uint8                    no_remove_previous_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );

uint32
  soc_pb_profile_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx
  );

uint32
  soc_pb_profile_init(
    SOC_SAND_IN  int                                 unit
  );

void
  SOC_PB_PROFILE_PARAMS_clear(
    SOC_SAND_OUT SOC_PB_PROFILE_PARAMS *info
  );

void
  SOC_PB_PROFILE_NDX_clear(
    SOC_SAND_OUT SOC_PB_PROFILE_NDX *info
  );


#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PROFILE_INCLUDED__*/
#endif

