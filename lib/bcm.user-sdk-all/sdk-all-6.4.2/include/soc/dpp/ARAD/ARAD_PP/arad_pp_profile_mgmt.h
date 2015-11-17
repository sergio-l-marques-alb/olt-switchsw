/* $Id: arad_pp_lif.h,v 1.18 Broadcom SDK $
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

#ifndef __ARAD_PP_PROFILE_MGMT_INCLUDED__
/* { */
#define __ARAD_PP_PROFILE_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/PPC/ppc_api_profile_mgmt.h>

#include <soc/dpp/PPC/ppc_api_lif.h>

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


/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_if_profile_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Init profile management mechanism, allocate lif&rif resources
 * INPUT:
 *     int                   unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_init(
   int                                                          unit
   );

/** 
 * In the mechanism below, we use a mask that determines for 
 * each application which buts of the profile were reserved for 
 * it. The functions below get application type and interface 
 * type, Find the mask rellevant for thte given application, and 
 * fill the value in the full profile given by the user 
 * according to the mask. The unmasked bits remain untouched. In
 * case of advanced functions the mask is the non-used bits of 
 * the profile. 
 * Example - Given: interface_type = inlif 
 *                  applicaton_type = same interface val = 1
 *                  full_profile = 0b1000
 *                 and the mask of the same interface
 *                 application is 0b0001
 *  The result will be: 0b1001
 *  
 *  Diagram:
 *  
 * mask = 0000010000001000000001000000, val = 0b101 
 *  
 * 101
 * ||\------------------\
 * |\----------\        |
 * \----\      |        | 
 *      |      |        |
 * 0000010000000000000001000000 
 * Compare: 
 * 0000010000001000000001000000 
 */

/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_if_profile_advanced_val_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the given value in the free bits of the given interface.
 * INPUT:
 *     int                   unit -
 *        Identifier of the device to access.
 *     SOC_PPC_PROFILE_MGMT_TYPE    interface_type -
 *        InLif/Outlif/Rif
 *     uint32                       val -
 *        value that should be set in the free bits of the given profile 
 *     uint32                      *full_profile -
 *        given profile (inlif/outlif/rif) in which the val bits should be set in the free (advanced) bits.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_advanced_val_set(
   int                                                            unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                      interface_type,
   uint32                                                         val,
   uint32                                                         *full_profile
   );

/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_if_profile_advanced_get_val_from_map
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Extract value from the given profile according to the free bits of the given interface.
 * INPUT:
 *     int                          unit -
 *        Identifier of the device to access.
 *     SOC_PPC_PROFILE_MGMT_TYPE    interface_type -
 *        InLif/Outlif/Rif
 *     uint32                       full_profile -
 *        given profile (inlif/outlif/rif) from which the val bits should be extracted.
 *     uint32                      *val -
 *        value that was exracted from the free bits of the given profile      
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_advanced_get_val_from_map(
   int                                                          unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                        interface_type,
   uint32                                                            full_profile,
   uint32                                                            *val
   );

/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_if_profile_val_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the given value in the reserved bits of the given interface, according to application type.
 * INPUT:
 *     int                   unit -
 *        Identifier of the device to access.
 *     SOC_PPC_PROFILE_MGMT_TYPE    interface_type -
 *        InLif/Outlif/Rif
 *     uint32                       applicaton_type -
 *        Application for which some part of the profile is reserved
 *     uint32                       val -
 *        value that should be set in the reserved bits of the given profile 
 *     uint32                      *full_profile -
 *        given profile (inlif/outlif/rif) in which the val bits should be set in the reserved bits.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_val_set(
   int                                                          unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          applicaton_type,
   uint32                                                          val,
   uint32                                                          *full_profile
   );

/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_if_profile_get_val_from_map
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Extract value from the given profile according to application type bits of the given interface.
 * INPUT:
 *     int                          unit -
 *        Identifier of the device to access.
 *     SOC_PPC_PROFILE_MGMT_TYPE    interface_type -
 *        InLif/Outlif/Rif
 *     uint32                       applicaton_type -
 *        Application for which some part of the profile is reserved
 *     uint32                       full_profile -
 *        given profile (inlif/outlif/rif) from which the val bits should be extracted.
 *     uint32                      *val -
 *        value that was exracted from the reserved bits of the given profile      
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_get_val_from_map(
   int                                                          unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          applicaton_type,
   uint32                                                            full_profile,
   uint32                                                            *val
   );


/*********************************************************************
* NAME:
 *   arad_pp_profile_mgmt_lif_profile_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Extract the profile bits values of InLIF, OutLIF and RIF
 * INPUT:
 *     int                          unit -
 *        Identifier of the device to access.
 *     SOC_PPD_DIAG_LIF_PROFILE_INFO  *info,
 *        Bits that were extracted
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_diag_info_get(
   int                                                              unit,
   SOC_PPC_DIAG_LIF_PROFILE_INFO                                   *info
   );


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_PROFILE_MGMT_INCLUDED__*/
#endif

