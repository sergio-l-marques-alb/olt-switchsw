
/* $Id: arad_pp_api_frwrd_fec.h,v 1.7 Broadcom SDK $
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

#ifndef __ARAD_PP_API_FRWRD_FEC_INCLUDED__
/* { */
#define __ARAD_PP_API_FRWRD_FEC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_fec.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_FEC_LB_CRC_17111   SOC_PPC_FEC_LB_CRC_17111
#define ARAD_PP_FEC_LB_CRC_10491   SOC_PPC_FEC_LB_CRC_10491
#define ARAD_PP_FEC_LB_CRC_155F5   SOC_PPC_FEC_LB_CRC_155F5
#define ARAD_PP_FEC_LB_CRC_19715   SOC_PPC_FEC_LB_CRC_19715
#define ARAD_PP_FEC_LB_CRC_13965   SOC_PPC_FEC_LB_CRC_13965
#define ARAD_PP_FEC_LB_CRC_1698D   SOC_PPC_FEC_LB_CRC_1698D
#define ARAD_PP_FEC_LB_CRC_1105D   SOC_PPC_FEC_LB_CRC_1105D
#define ARAD_PP_FEC_LB_KEY         SOC_PPC_FEC_LB_KEY      
#define ARAD_PP_FEC_LB_ROUND_ROBIN SOC_PPC_FEC_LB_ROUND_ROBIN
#define ARAD_PP_FEC_LB_2_CLOCK     SOC_PPC_FEC_LB_2_CLOCK  
#define ARAD_PP_FEC_LB_CRC_10861   SOC_PPC_FEC_LB_CRC_10861
#define ARAD_PP_FEC_LB_CRC_10285   SOC_PPC_FEC_LB_CRC_10285
#define ARAD_PP_FEC_LB_CRC_101A1   SOC_PPC_FEC_LB_CRC_101A1
#define ARAD_PP_FEC_LB_CRC_12499   SOC_PPC_FEC_LB_CRC_12499
#define ARAD_PP_FEC_LB_CRC_1F801   SOC_PPC_FEC_LB_CRC_1F801
#define ARAD_PP_FEC_LB_CRC_172E1   SOC_PPC_FEC_LB_CRC_172E1
#define ARAD_PP_FEC_LB_CRC_1EB21   SOC_PPC_FEC_LB_CRC_1EB21
#define ARAD_PP_FEC_LB_CRC_0x8003   SOC_PPC_FEC_LB_CRC_0x8003
#define ARAD_PP_FEC_LB_CRC_0x8011   SOC_PPC_FEC_LB_CRC_0x8011
#define ARAD_PP_FEC_LB_CRC_0x8423   SOC_PPC_FEC_LB_CRC_0x8423
#define ARAD_PP_FEC_LB_CRC_0x8101   SOC_PPC_FEC_LB_CRC_0x8101
#define ARAD_PP_FEC_LB_CRC_0x84a1   SOC_PPC_FEC_LB_CRC_0x84a1
#define ARAD_PP_FEC_LB_CRC_0x9019   SOC_PPC_FEC_LB_CRC_0x9019

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

#define ARAD_PP_FEC_TYPE_DROP                                SOC_PPC_FEC_TYPE_DROP
#define ARAD_PP_FEC_TYPE_IP_MC                               SOC_PPC_FEC_TYPE_IP_MC
#define ARAD_PP_FEC_TYPE_IP_UC                               SOC_PPC_FEC_TYPE_IP_UC
#define ARAD_PP_FEC_TYPE_TRILL_MC                            SOC_PPC_FEC_TYPE_TRILL_MC
#define ARAD_PP_FEC_TYPE_TRILL_UC                            SOC_PPC_FEC_TYPE_TRILL_UC
#define ARAD_PP_FEC_TYPE_BRIDGING_INTO_TUNNEL                SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL
#define ARAD_PP_FEC_TYPE_BRIDGING_WITH_AC                    SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC
#define ARAD_PP_FEC_TYPE_MPLS_LSR                            SOC_PPC_FEC_TYPE_MPLS_LSR
#define ARAD_PP_NOF_FEC_TYPES                                SOC_PPC_NOF_FEC_TYPES_ARAD
#define ARAD_PP_FEC_TYPE_SIMPLE_DEST                         SOC_PPC_FEC_TYPE_SIMPLE_DEST
#define ARAD_PP_FEC_TYPE_ROUTING                             SOC_PPC_FEC_TYPE_ROUTING
#define ARAD_PP_FEC_TYPE_TUNNELING                           SOC_PPC_FEC_TYPE_TUNNELING
#define ARAD_PP_FEC_TYPE_TUNNELING_EEI                       SOC_PPC_FEC_TYPE_TUNNELING_EEI
#define ARAD_PP_FEC_TYPE_FORWARD                             SOC_PPC_FEC_TYPE_FORWARD
typedef SOC_PPC_FEC_TYPE                                       ARAD_PP_FEC_TYPE;

#define ARAD_PP_FRWRD_FEC_RPF_MODE_NONE                      SOC_PPC_FRWRD_FEC_RPF_MODE_NONE
#define ARAD_PP_FRWRD_FEC_RPF_MODE_UC_STRICT                 SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT
#define ARAD_PP_FRWRD_FEC_RPF_MODE_UC_LOOSE                  SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE
#define ARAD_PP_FRWRD_FEC_RPF_MODE_MC_EXPLICIT               SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT
#define ARAD_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP                SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP
#define ARAD_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP      SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP
#define ARAD_PP_NOF_FRWRD_FEC_RPF_MODES                      SOC_PPC_NOF_FRWRD_FEC_RPF_MODES
typedef SOC_PPC_FRWRD_FEC_RPF_MODE                             ARAD_PP_FRWRD_FEC_RPF_MODE;

#define ARAD_PP_FRWRD_FEC_PROTECT_TYPE_NONE                  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE
#define ARAD_PP_FRWRD_FEC_PROTECT_TYPE_FACILITY              SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY
#define ARAD_PP_FRWRD_FEC_PROTECT_TYPE_PATH                  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH
#define ARAD_PP_NOF_FRWRD_FEC_PROTECT_TYPES                  SOC_PPC_NOF_FRWRD_FEC_PROTECT_TYPES
typedef SOC_PPC_FRWRD_FEC_PROTECT_TYPE                         ARAD_PP_FRWRD_FEC_PROTECT_TYPE;

#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_NONE                SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_NONE
#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC             SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC
#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_ECMP                SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP
#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT        SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT
#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT    SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT
#define ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_ECMP_PROTECTED      SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP_PROTECTED
#define ARAD_PP_NOF_FRWRD_FEC_ENTRY_USE_TYPES                SOC_PPC_NOF_FRWRD_FEC_ENTRY_USE_TYPES
typedef SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE                       ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE;

#define ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_ALL                SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL
#define ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE           SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE
#define ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED           SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED
#define ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED     SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED
#define ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED
#define ARAD_PP_NOF_FRWRD_FEC_MATCH_RULE_TYPES               SOC_PPC_NOF_FRWRD_FEC_MATCH_RULE_TYPES
typedef SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE                      ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE;

typedef SOC_PPC_FRWRD_FEC_GLBL_INFO                            ARAD_PP_FRWRD_FEC_GLBL_INFO;
typedef SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO                       ARAD_PP_FRWRD_FEC_ENTRY_RPF_INFO;
typedef SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO                       ARAD_PP_FRWRD_FEC_ENTRY_APP_INFO;
typedef SOC_PPC_FRWRD_FEC_ENTRY_INFO                           ARAD_PP_FRWRD_FEC_ENTRY_INFO;
typedef SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO                  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO;
typedef SOC_PPC_FRWRD_FEC_PROTECT_INFO                         ARAD_PP_FRWRD_FEC_PROTECT_INFO;
typedef SOC_PPC_FRWRD_FEC_ECMP_INFO                            ARAD_PP_FRWRD_FEC_ECMP_INFO;
typedef SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO                       ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO;
typedef SOC_PPC_FRWRD_FEC_MATCH_RULE                           ARAD_PP_FRWRD_FEC_MATCH_RULE;
typedef SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO                ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO;
typedef SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO                  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO;

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
 *   arad_pp_frwrd_fec_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the FEC table (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_glbl_info_set" API.
 *     Refer to "arad_pp_frwrd_fec_glbl_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add FEC entry. May include protection of type Facility
 *   or Path.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Has to be even
 *     for protected FECs in Arad-B.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_TYPE              protect_type -
 *     Protection type may be None, Path, or Facility.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *working_fec -
 *     Working FEC entry.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec -
 *     Protecting FEC entry. Relevant when there is protection,
 *     i.e., protection type is not
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_INFO              *protect_info -
 *     Protection information. Relevant only for Path protected
 *     FEC, i.e., if the protection type is
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH. Includes the
 *     OAM-instance that the above FECs are associated with and
 *     determines which FEC to use in order to forward the
 *     packets.
 *   SOC_SAND_OUT uint8                                 *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the FEC DB (LEM).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add ECMP to the FEC table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     The FEC array (ECMP).
 *   SOC_SAND_IN  uint32                                  nof_entries -
 *     Size of ECMP and number of valid entries in fec_array.
 *   SOC_SAND_OUT uint8                                 *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the FEC DB (LEM).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_update
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Update the content of block of entries from the ECMP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     FEC array to update the selected range of the ECMP. The
 *     size of this array should be as the length of the array
 *     (fec_range.end - fec_rang.start + 1)
 *   SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range -
 *     Range includes start FEC and end FEC to update.
 *     fec_range.end has to be smaller than the updated ECMP
 *     size.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_update(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );


/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   set ECMP attributres
 *   relevant: Arad only.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              ecmp_ndx -
 *     ECMP index. Arad Range: 0 - 32K.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_INFO            *ecmp_info -
 *     ECMP info, including, size, base-fec,...
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_info_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_PP_FEC_ID ecmp_ndx,
    SOC_SAND_IN ARAD_PP_FRWRD_FEC_ECMP_INFO *ecmp_info
  ); 

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get ECMP attributres
 *   relevant: Arad only.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              ecmp_ndx -
 *     ECMP index. Arad Range: 0 - 32K.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_INFO            *ecmp_info -
 *     ECMP info, including, size, base-fec,...
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_info_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_PP_FEC_ID ecmp_ndx,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_INFO *ecmp_info
  ); 

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_entry_use_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the usage/allocation of the FEC entry pointed by
 *   fec_ndx (ECMP/FEC/protected FEC/none).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info -
 *     The usage of the FEC entry. - For ECMP/protection
 *     returns pointer to first FEC in the ECMP/protection.-
 *     For ECMP returns the size of the ECMP
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_entry_use_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get FEC entry from the FEC table. May include
 *   protection.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Has to be even
 *     for Protected FECs in Arad-B.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type -
 *     Protection type may be None, Path, or Facility.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *working_fec -
 *     Working FEC entry.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec -
 *     Protecting FEC entry. Relevant when there is protection,
 *     i.e., protection type is not
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_PROTECT_INFO              *protect_info -
 *     Protection information. Relevant only for Path protected
 *     FEC, i.e., if the protection type is
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH. Includes the
 *     OAM-instance that the above FECs are associated with and
 *     determines which FEC to use in order to forward the
 *     packets.
 * REMARKS:
 *   - fec_ndx should be allocated and not point to ECMP,
 *   Error will be returned if so.- In case of protection
 *   fec_ndx has to point to the first FEC in the
 *   protection.- In order to get the allocation type of the
 *   FEC, use soc_ppd_frwrd_fec_entry_alloc_info_get(fec_ndx) -
 *   to get ECMP use soc_ppd_frwrd_fec_ecmp_get().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Update content of range of the ECMP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even
 *     and start of ECMP.
 *   SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range -
 *     Range includes start FEC and end FEC to return.
 *     fec_range.end has to be smaller than the ECMP size.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     Block of FEC entries of the ECMP.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of entries in the returned fec_array
 * REMARKS:
 *   - fec_ndx has to point to fec of type
 *   SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE_ECMP. Error will be
 *   returned otherwise.- fec_ndx has to point to the first
 *   FEC in the ECMP.- In order to get the allocation type of
 *   the FEC, use soc_ppd_frwrd_fec_entry_alloc_info_get(fec_ndx)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove FEC entry/entries associated with fec_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 * REMARKS:
 *   - When removing ECMP, then the fec_ndx must point to the
 *   first FEC of this ECMP (error will be returned
 *   otherwise), and then all FEC entries of the ECMP will be
 *   removed. - When removing protected FEC (path/facility),
 *   then the fec_ndx must point to the first FEC (even
 *   index) (error will be returned otherwise), and then both
 *   FEC entries will be removed.- User cannot remove FEC
 *   that is part of the ECMP/protection without moving all
 *   the FEC entries in the ECMP/protection.- It is the user
 *   responsibility to remove all usages/pointers to the FEC
 *   entry before removing it.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_protection_oam_instance_status_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the status of the OAM instance. For all PATH
 *   protected FECs that point to this instance, the working
 *   FEC will be used if up is TRUE, and the protect FEC will
 *   be used otherwise.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  oam_instance_ndx -
 *     OAM instance ID. Range: Arad-B: 0-4K-1, T20E:0-16K-1.
 *   SOC_SAND_IN  uint8                                 up -
 *     OAM instance status.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_protection_oam_instance_status_set"
 *     API.
 *     Refer to
 *     "arad_pp_frwrd_fec_protection_oam_instance_status_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_protection_oam_instance_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_protection_sys_port_status_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the status of the System Port (LAG or Physical
 *   port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx -
 *     System Port (LAG or Physical port).
 *   SOC_SAND_IN  uint8                                 up -
 *     System port status.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_protection_sys_port_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_protection_sys_port_status_set" API.
 *     Refer to
 *     "arad_pp_frwrd_fec_protection_sys_port_status_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_protection_sys_port_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the FEC table (in the specified range) and get
 *   all the FEC entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_MATCH_RULE                *rule -
 *     Get only entries that match this rule.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Range for iteration
 *   SOC_SAND_OUT uint32                                  *fec_array -
 *     Array to include FEC IDs.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in fec_array.
 * REMARKS:
 *   - if rule is according to application type, then for
 *   ECMP and protection only the id of the first FEC of the
 *   ECMP/protection is returned.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_entry_accessed_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set action to do by the device when a packet accesses
 *   the FEC entry.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_id_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info -
 *     Actions to do when packet accesses the FEC.
 * REMARKS:
 *   - May be set only for FEC entries in the Range 0-63- Not
 *   supported in T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_entry_accessed_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_entry_accessed_info_set" API.
 *     Refer to "arad_pp_frwrd_fec_entry_accessed_info_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_entry_accessed_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                                 clear_access_stat,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_hashing_global_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ECMP hashing global attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info -
 *     ECMP Hashing global settings
 * REMARKS:
 *   The hashing function result is 16 bit value. The PP use
 *   the value to choose the ECMP FEC member, to which the
 *   packet is sent
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_ecmp_hashing_global_info_set" API.
 *     Refer to "arad_pp_frwrd_fec_ecmp_hashing_global_info_set"
 *     API for details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_hashing_global_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fec_ecmp_hashing_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ECMP hashing per-port attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                port_ndx -
 *     Port ID.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info -
 *     ECMP Hashing per-port settings
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fec_ecmp_hashing_port_info_set" API.
 *     Refer to "arad_pp_frwrd_fec_ecmp_hashing_port_info_set"
 *     API for details.
*********************************************************************/
uint32
  arad_pp_frwrd_fec_ecmp_hashing_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

void
  ARAD_PP_FRWRD_FEC_GLBL_INFO_clear(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_GLBL_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_RPF_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_APP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_APP_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_PROTECT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_PROTECT_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_MATCH_RULE *info
  );

void
  ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_FEC_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FEC_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_FEC_RPF_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_RPF_MODE enum_val
  );

const char*
  ARAD_PP_FRWRD_FEC_PROTECT_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_USE_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_MATCH_RULE_TYPE enum_val
  );

void
  ARAD_PP_FRWRD_FEC_GLBL_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_GLBL_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_RPF_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_APP_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_APP_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_PROTECT_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_PROTECT_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_MATCH_RULE *info
  );

void
  ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

void
  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_FRWRD_FEC_INCLUDED__*/
#endif



