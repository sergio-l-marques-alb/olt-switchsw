/* $Id: arad_pp_trap.h,v 1.27 Broadcom SDK $
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
*/

#ifndef __JER_PP_TRAP_INCLUDED__
/* { */
#define __JER_PP_TRAP_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_footer.h>

#include <soc/dpp/PPC/ppc_api_eg_mirror.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define MIRROR_STRENGTH_MAX_VALUE  (3)
#define MIRROR_STRENGTH_MIN_VALUE  (0)
#define FWD_STRENGTH_MAX_VALUE     (3)
#define FWD_STRENGTH_MIN_VALUE     (0)
#define MIRROR_COMMAND_MAX_VALUE   (15)
#define MIRROR_COMMAND_MIN_VALUE   (0)



/* static array defines */

/* positions in the table */
#define ETPP_OUT_VPORT_DISCARD_INDEX                       (0)
#define ETPP_STP_STATE_FAIL_INDEX                          (1)
#define ETPP_PROTECTION_PATH_UNEXPECTED_INDEX              (2)
#define ETPP_VPORT_LOOKUP_FAIL_INDEX                       (3)
#define ETPP_MTU_FILTER                                    (4)
#define ETPP_ACC_FRAME_TYPE                                (5)
#define ETPP_SPLIT_HORIZON                                 (6)
#define ETPP_NOF_TRAPS                                     (7)


#define ETPP_MIRROR_CMD_INDEX                              (0)
#define ETPP_FWD_STRENGTH_INDEX                            (1)
#define ETPP_MIRROR_STRENGTH_INDEX                         (2)
#define ETPP_FWD_ENABLE_INDEX                              (3)
#define ETPP_MIRROR_ENABLE_INDEX                           (4)
#define ETPP_NUMBER_TRAP_FIELDS                            (5)


/*********************************************************************
* NAME:
 *   soc_jer_pp_eg_pmf_mirror_params_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mirror profile table to use by PMF when it matches (on
 *   action) a specific mirror profile.
 * INPUT:
 *   int    unit -
 *       Identifier of the device to access.
 *   uint32 mirror_profile -
 *       Mirror profile to use as index into table
 *   dpp_outbound_mirror_config_t *config -
 *       Parameters to load into table. Note that mirror_enable
 *       and fwd_enable are among these parameters.
 * REMARKS:
 *   For Jericho (and up) only.
 * See also:
 *   EPNI_PMF_MIRROR_PROFILE_TABLE
 *   soc_jer_pp_eg_pmf_mirror_params_set()
 *   bcm_petra_mirror_port_destination_add()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t
soc_jer_pp_eg_pmf_mirror_params_set(int unit, uint32 mirror_profile, dpp_outbound_mirror_config_t *config) ;

soc_error_t
soc_jer_eg_etpp_trap_set(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info);

soc_error_t
soc_jer_eg_etpp_trap_get(int unit, SOC_PPC_TRAP_ETPP_TYPE trap, SOC_PPC_TRAP_ETPP_INFO *entry_info);

soc_error_t
soc_jer_eg_etpp_verify_parmas(int unit, SOC_PPC_TRAP_ETPP_INFO *info);

void
soc_jer_eg_etpp_trap_get_array_index( SOC_PPC_TRAP_CODE trap, int *trap_index);


/* }__JER_PP_TRAP_INCLUDED__*/
#endif
