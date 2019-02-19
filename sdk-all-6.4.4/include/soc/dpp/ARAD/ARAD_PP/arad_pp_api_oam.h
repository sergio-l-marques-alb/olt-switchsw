/* $Id: arad_pp_api_oam.h,v 1.23 Broadcom SDK $
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

#ifndef __ARAD_PP_API_OAM_INCLUDED__
/* { */
#define __ARAD_PP_API_OAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_oam.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

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
 *   arad_pp_oam_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   init oam
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO init_trap_info_oam -
 *     Initial TRAP id info for OAM.
 *   SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO init_trap_info_bfd -
 *     Initial TRAP id info for BFD. 
 *   SOC_SAND_IN  uint8                                  is_bfd -
 *     Init BFD or OAM indication
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_init(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  SOC_PPD_OAM_INIT_TRAP_INFO * init_trap_info_oam,
	SOC_SAND_IN  SOC_PPD_BFD_INIT_TRAP_INFO * init_trap_info_bfd,
	SOC_SAND_IN  uint8                                  is_bfd
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_deinit
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function deinitializes oam registers and tables
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    is_bfd -
 *     Init BFD or OAM indication 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_deinit(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                    is_bfd,
    SOC_SAND_IN  uint8                    tcam_db_destroy
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_icc_map_register_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Icc Map Register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      icc_ndx -
 *     Index of ICC register to access.
 *   SOC_SAND_IN   SOC_PPD_OAM_ICC_MAP_DATA  *    data -
 *     Data to write to register. 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_icc_map_register_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_ICC_MAP_DATA     * data
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_icc_map_register_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Icc Map Register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      icc_ndx -
 *     Index of ICC register to access.
 *   SOC_SAND_OUT   SOC_PPD_OAM_ICC_MAP_DATA  *    data -
 *     Data to fill from register. 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_icc_map_register_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_ICC_MAP_DATA     * data
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oam1_entries_insert_default_profile
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OAM1 table default profile entries
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_PROFILE_DATA  * profile_data -
 *     New non-accelerated profile data.
 *   SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map -
 *     Mapping from trap code to mirror profile.
 *   SOC_SAND_IN  uint8                            is_bfd -
 *     Is bfd profile
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
	SOC_SAND_IN  SOC_PPD_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map,
    SOC_SAND_IN  uint8                            is_bfd
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OAM1 and OAM2 tables in the classifier
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry -
 *     Data to set into classifier.
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_PROFILE_DATA  * profile_data -
 *     New non-accelerated profile data.
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_PROFILE_DATA  * profile_data_acc -
 *     New accelerated profile data.
 *     This pointer is NULL in case of passive profile configuration.
 *   SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map -
 *     Mapping from trap code to mirror profile.  
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oam1_2_entries_insert_according_to_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN uint8                         is_server,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPD_OAM_LIF_PROFILE_DATA       *profile_data_acc,
	SOC_SAND_IN  SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP  cpu_trap_code_to_mirror_profile_map
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem_mep_profile_replace
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Replace rellevant entries in OEM1 and OEM2
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry -
 *     Data containinfg the key and payload of the mep. 
 *  SOC_SAND_IN  uint32                           update_mp_type
 *   	1 if the mp-type is to be updated, 0 if only the mp profile is to be updated
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem_mep_profile_replace(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem_mep_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MEP data in classifier
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      mep_index -
 *     Index of the mep
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry -
 *     Data to set into classifier.
 *	SOC_SAND_IN  uint8                    update -
 *    Flag to indicate that entry already exist 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem_mep_add(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry,
	SOC_SAND_IN  uint8                    update
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_mep_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get MEP data from classifier
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      mep_index -
 *     Index of the mep
 *   SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry -
 *     Data to fill from classifier. 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
/*uint32
  arad_pp_oam_classifier_mep_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry
  );
*/

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_mep_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Delete MEP data from classifier
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  int                      mep_index -
 *     Index of the mep 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_mep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem1_entry_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OEM1 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key -
 *     Key struct
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload -
 *     Payload struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem1_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OEM1 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key -
 *     Key struct
 *   SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload -
 *     Payload struct
 *   SOC_SAND_OUT  uint8                                      *is_found -
 *     Indication if the entry was found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem1_entry_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Delete OEM1 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key -
 *     Key struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem1_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem2_entry_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OEM2 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key -
 *     Key struct
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload -
 *     Payload struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem2_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OEM2 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key -
 *     Key struct
 *   SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload -
 *     Payload struct
 *   SOC_SAND_OUT  uint8                                      *is_found -
 *     Indication if the entry was found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_oem2_entry_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Delete OEM1 entry
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key -
 *     Key struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_oem2_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPD_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_rmep_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set RMEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   rmep_index -
 *     RMEP index in db
 *   SOC_SAND_IN  uint16                   rmep_id -
 *     RMEP protocol id
 *   SOC_SAND_IN  uint32                   mep_index -
 *     Index of a local MEP connected with this RMEP
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type -
 *     Type of endpoint
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry -
 *     RMEP db entry
 *    SOC_SAND_IN  uint8                    update -
 *      Indication whether to update the entry or insert new one 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
	SOC_SAND_IN  uint8                    update
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_rmep_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get RMEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   rmep_index -
 *     RMEP index in db
  *   SOC_SAND_OUT  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY rmep_db_entry -
 *     RMEP db entry
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_rmep_index_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get RMEP index from Exact match
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint16                   rmep_id -
 *     RMEP protocol id
 *   SOC_SAND_IN  uint32                   mep_index -
 *     Index of a local MEP connected with this RMEP
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type -
 *     Type of endpoint
 *   SOC_SAND_OUT  uint32                  *rmep_index -
 *     RMEP index in db found in EM
 *   SOC_SAND_OUT  uint8                   *is_found -
 *     was match in EM
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_index_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_rmep_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Delete RMEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   rmep_index -
 *     RMEP index in db
 *   SOC_SAND_IN  uint16                   rmep_id -
 *     RMEP protocol id
 *   SOC_SAND_IN  uint32                   mep_index -
 *     Index of a local MEP connected with this RMEP
 *   SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type -
 *     Type of endpoint 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_rmep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPD_OAM_MEP_TYPE     mep_type
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_mep_db_entry_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   mep_index -
 *     RMEP index in db
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry -
 *     MEP db entry
 *   SOC_SAND_IN  uint8                      allocate_icc_ndx -
 *     Flag idicating if new icc index was allocated
 *   SOC_SAND_IN  SOC_PPD_OAM_MA_NAME          name -
 *     Name of MA assosiated with the MEP
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
	SOC_SAND_IN  uint8                    allocate_icc_ndx,
	SOC_SAND_IN  SOC_PPD_OAM_MA_NAME      name
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_mep_db_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get MEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   mep_index -
 *     RMEP index in db
  *   SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY   mep_db_entry -
 *     MEP db entry
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_mep_db_entry_delete
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MEP
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   mep_index -
 *     MEP index in db
 *   SOC_SAND_IN  uint8                    deallocate_icc_ndx -
 *     Set if icc index was deallocated
 *   SOC_SAND_IN  uint8                    is_last_mep -
 *     Set if removing the last mep
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_mep_db_entry_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	SOC_SAND_IN  uint8                    is_last_mep
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Finding if mep with given lif&mdlevel exists
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   lif -
 *     Lif to search by
 *   SOC_SAND_IN  uint8                    md_level -
 *     Level to search by
 *   SOC_SAND_IN uint8                    is_upmep -
 *     Direction to search by
 *   SOC_SAND_OUT uint8                    *found -
 *     Indication to whether a mep was found
 * 	SOC_SAND_OUT uint32                   *profile -
 *     Profile existing on this lif
 *	SOC_SAND_OUT uint8                    *found_profile -
 *     Indication if lif found in OEM1
 *  SOC_SAND_OUT uint8                    *is_mp_type_flexible -
 *     Indication if the mp_type can be changed
 *      (there are no constrains given by existing meps and mips)
 *  SOC_SAND_OUT  uint8                    *is_mip -
 *     Indication if the found endpoint is mip 
 *     ARAD+ use only
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
	SOC_SAND_IN  uint8                    md_level,
	SOC_SAND_IN  uint8                    is_upmep,
	SOC_SAND_OUT uint8                    *found_mep,
	SOC_SAND_OUT uint32                   *profile,
	SOC_SAND_OUT uint8                    *found_profile,
	SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT  uint8                    *is_mip
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_counter_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OAM counter range
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   counter_range_min -
 *     Min counter range
 *   SOC_SAND_IN  uint32                   counter_range_max -
 *     Max counter range
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_counter_range_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  arad_pp_oam_counter_range_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_event_fifo_read
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read oam event fifo
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  uint32                    *rmeb_db_ndx -
 *     Rmep db index
 *   SOC_SAND_OUT  uint32                    *event_id -
 *     ID of event
 *   SOC_SAND_OUT  uint32                    *valid -
 *     Is valid
 *   SOC_SAND_OUT  uint32                    *event_data -
 *     Data convayed in the event 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_event_fifo_read(
    SOC_SAND_IN  int                                        unit,
	SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
	SOC_SAND_OUT  uint32                                       *event_id,
	SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_data
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_pp_pct_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set PP_PCT oam port profile field
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  uint32                    local_port_ndx -
 *     Rort index
 *   SOC_SAND_OUT  uint32                    oam_profile -
 *     Binary profile
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_pp_pct_profile_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  arad_pp_oam_pp_pct_profile_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_eth_oam_opcode_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure mapping of network opcode to internal opcode.
 *   The information is taken from WB variables.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_eth_oam_opcode_map_set(
    SOC_SAND_IN   int                                     unit
  );

/*
 * ARAD BFD APIs
*/

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_tos_ttl_select_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Ipv4 Tos Ttl Select register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   ipv4_tos_ttl_select_index -
 *     Entry index.
 *   SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA      *tos_ttl_data -
 *     Data struct
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPD_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_src_addr_select_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Ipv4 Src Addr Select register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   ipv4_src_addr_select_index -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  src_addr -
 *     Data - src address.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_tx_rate_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Tx Rate register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   bfd_tx_rate_index -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  tx_rate -
 *     TX rate data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_tx_rate_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_req_interval_pointer_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Req Interval Pointer register (MPLS Push profile)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   req_interval_pointer -
 *     Entry index.
 *   SOC_SAND_IN  uint32                  req_interval -
 *     Rate data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_req_interval_pointer_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_req_interval_pointer_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Req Interval Pointer register (MPLS Push profile)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   push_profile -
 *     Entry index.
 *   SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA *push_data -
 *     Push profile data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_mpls_pwe_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPD_MPLS_PWE_PROFILE_DATA            *push_data
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_mpls_udp_sport_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd MPLS UDP Sport register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   udp_sport -
 *     register data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_mpls_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_ipv4_udp_sport_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd IPV4 UDP Sport register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                   udp_sport -
 *     register data.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_pdu_static_register_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd pdu static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER  bfd_pdu -
 *     register data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_pdu_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_cc_packet_static_register_set / get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd CC packet static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_BFD_PDU_STATIC_REGISTER  bfd_pdu -
 *     register data struct.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_cc_packet_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPD_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_discriminator_range_registers_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd pdu static register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint16                   range -
 *     range of your discriminator
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_discriminator_range_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_my_bfd_dip_ip_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set My Bfd Dip table with IPv4 values register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   dip_index -
 *     Entry index
 *	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS						*dip	 - 
 *     IPv6 address
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS					                 *dip							
  );

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS					*dip							
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_tx_mpls_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Bfd Tx Mpls register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES  *tx_mpls_att -
 *     Register values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_tx_priority_registers_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set OAMP priority TC and DP registers
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    priority -
 *     Priority profile (0-7)
 *   SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES    *tx_oam_att -
 *     Register values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_tx_priority_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_IN  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_OUT  SOC_PPD_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_enable_interrupt_message_event_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oamp_enable_interrupt_message_event register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    interrupt_message_event_bmp -
 *     Bitmap of the events to set
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                        *interrupt_message_event_bmp
  );

/* ARAD+ functions */
/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_diag_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set bfd diag profile
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  uint32                   diag_profile -
 *     Value to setin the enable register
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_diag_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *diag_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_bfd_flags_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set bfd flags profile
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  uint32                   flags_profile -
 *     Value to setin the enable register
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_bfd_flags_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *flags_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_mep_passive_active_enable_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oam_mep_passive_active_enable register - MP_type of each profile
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  uint8                    enable -
 *     Value to setin the enable register
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_mep_passive_active_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_oam_mep_passive_active_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_punt_event_hendling_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set oamp_punt_event_hendling register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                   profile_ndx -
 *     Profile index
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data -
 *     Profile fields
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_error_trap_id_and_destination_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set PORT2CPU OAMP register fields trap id and system port with the given
 *   values according to the error type.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type -
 *     Type of the error to set
 *   SOC_SAND_IN  uint32                                 trap_id -
 *     Id 0-255 to put in trap id field
 *   SOC_SAND_IN  uint32                                 dest_system_port -
 *     20bit system port to put in port field
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  uint32                                 dest_system_port
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  uint32                                *dest_system_port
  );



/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_lm_dm_set/get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Manage LM and DM, e.g add MEP DB entry if possible.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_OAM_OAMP_MEP_DB_ENTRY     *mep_db_entry -
 *     mep db fields
 *   SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
 * REMARKS:
 *   Used for Arad+ only.
 *   Functions are shared by LM and DM due to the similarity in the implementation of the two functionalities.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_lm_dm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_lm/dm_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Return statistics form soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PPD_OAM_OAMP_(L|D)M_INFO_GET     *(l|d)m_info
 * statistics go in there.
 *   SOC_SAND_OUT uint8                                      * is_1dm
 * (DM only) returns 1 if the entry is set as 1DM.
 * REMARKS:
 *   Used for Arad+.
 *   Functions are shared by LM and DM due to the similarity in the implementation of the two functionalities.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_lm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_LM_INFO_GET     *lm_info
  );

uint32
  arad_pp_oam_oamp_dm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPD_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_next_index_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_OUT uint32                               *next_index
 * Next available index for additional LM/DM entries. returns 0 if none available.
 * REMARKS:
 *   Used for Arad+.
 *   Function is used to create LM and DM entries. Function changes nothing in the HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_next_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_eth1731_and_oui_profiles_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_IN  uint8                          remove_mode,
 *  SOC_SAND_OUT uint32                               *eth1731_prof
 *  SOC_SAND_OUT uint32                               *da_oui_prof
 *      Profiles associated with mep.
 * REMARKS:
 *   Used for Arad+. Function returns both because the oui profile cannot be recovered without
 * First finding the eth1731 profile.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_nic_profile_get, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *  SOC_SAND_OUT uint32                               *da_nic_prof
 *      Profile associated with mep.
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_oam_oamp_nic_profile_get(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );



/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_search_for_lm_dm, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN  uint32                                 endpoint_id,
 * index of the endpoint in the OAMP MEP DB (CCM entry)
 *   SOC_SAND_IN  uint8                          search_mode,
 *   1 to find out if a DM entry exists,  0 to find out if a LM entry exists, 2 for either one.
 *   SOC_SAND_OUT uint32                               * found_bitmap,
 *    For every mep type, SOC_PPC_OAM_MEP_TYPE_XXX,  (found_bitmap &  SOC_PPC_OAM_MEP_TYPE_XXX)
 * iff such an entry is associated with the given mep.
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_oam_oamp_search_for_lm_dm(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               * found_bitmap
       );


/*********************************************************************
* NAME:
 *   soc_ppd_oam_oamp_create_new_eth1731_profile, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
 *      
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_create_new_eth1731_profile(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_set_oui_nic_registers, 
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage OUI, NIC registers (MAC DA addresses)
 * INPUT:
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
 *      
 * REMARKS:
 *   Used for Arad+.
 *  OUI profile should be written seperately by the create_new_eth1731_profile() function.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_set_oui_nic_registers(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );


/*********************************************************************
* NAME:
 *   arad_pp_oam_oamp_lm_dm_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage LM and DM on the soc layer.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *     SOC_SAND_IN uint32                                        endpoint_id -
 * Entry number of the endpoint.
 *   SOC_SAND_IN  uint8                          is_lm,
 *   1 if lm needs removing, 0 otherwise
 *   SOC_SAND_OUT uint8                               * num_removed,
 *    2 if LM + LM-STAT were removed, 1 if only an LM/DM was removed, 0 if none were removed.
 *   SOC_SAND_OUT uint32                              * removed_index
 * The index thatwas freed.
 * REMARKS:
 *   Used for Arad+.
 *   Functions are shared by LM and DM due to the similarity in the implementation of the two functionalities.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_oamp_lm_dm_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_oam_classifier_counter_disable_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set classifier counter disable map.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                  packet_is_oam -
 *     Bit to signal OAM/Data packets setting
 *   SOC_SAND_IN  uint8                                  profile -
 *     OAMA MP-Profile to set 
 *   SOC_SAND_IN  uint8                                  counter_enable -
 *     Value of the map - counter enable/disable
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_classifier_counter_disable_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  );


/*********************************************************************
* NAME:
 *    arad_pp__oam_oamp_loopback_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Start an OAM Loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *     SOC_SAND_IN uint32                                        endpoint_id -
 * Entry number of the endpoint.
 *      SOC_SAND_IN  uint8                                                      tx_period,
 *   Tx period (same "units" as in CCMs)
 *     SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                mac_address,
 * DA MAC address of ioutgoing LBMs
 *      SOC_SAND_INOUT uint8                                                   *is_added
 * see PPD file
 * REMARKS:
 *   Used for Arad+.
 *   Only one loopback session may be used. If a session is already in use then is_added returns 0 and no further changes are made.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_set(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_IN uint32                                                     endpoint_id,
       SOC_SAND_IN  uint8                                                      tx_period,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *mac_address,
       SOC_SAND_INOUT uint8                                                   *is_added
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Get statistics on OAM loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *      SOC_SAND_OUT uint32                                              *rx_packet_count,
 *    SOC_SAND_OUT uint32                                              *tx_packet_count,
 *     SOC_SAND_OUT uint32                                              *discard_count,
 *     SOC_SAND_OUT uint32                                              *fifo_count
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_get(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *rx_packet_count,
       SOC_SAND_OUT uint32                                              *tx_packet_count,
       SOC_SAND_OUT uint32                                              *discard_count,
       SOC_SAND_OUT uint32                                              *fifo_count
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_reset
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+. To be used after WB.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
     arad_pp_oam_dma_reset(
       SOC_SAND_IN  int                                                  unit
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
     arad_pp_oam_dma_clear(
       SOC_SAND_IN  int                                                  unit
       );

/*********************************************************************
* NAME:
 *    arad_pp_oam_register_dma_event_handler_callback
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *   SOC_SAND_IN        int (*event_handler_cb)(int)
 Function called upon DMA interrupt. Paramater is declared as SOC_SAND_INOUT as opposed to SOC_SAND_IN
 for compilation.
 * REMARKS:
 *   Used for Arad+. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
   arad_pp_oam_register_dma_event_handler_callback(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        int (*event_handler_cb)(int)
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_rx_trap_codes_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Configure OAMP supported trap codes per endpoint type
 * INPUT:
 *   SOC_SAND_IN  int                   unit
 *   SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE  mep_type -
 *                 Endpoint type that is associated with the given trap cpde 
 *    SOC_SAND_IN  uint32               trap_code -
 *                 Trap code that will be recognized by the OAMP as valid trap code.
 * REMARKS:
 *   Used for Arad+ only. 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_rx_trap_codes_set(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );

uint32 
    arad_pp_oam_oamp_rx_trap_codes_delete(
       SOC_SAND_IN          int                                 unit,
       SOC_SAND_IN          SOC_PPC_OAM_MEP_TYPE                mep_type,
       SOC_SAND_IN          uint32                              trap_code
       );


/*********************************************************************
* NAME:
 *    arad_pp_oam_dma_event_handler
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Reset the DMA.
 * INPUT:
      SOC_SAND_INOUT  void        *     unit,
 *  device id stored in void* form.
 
      SOC_SAND_INOUT  void        *     unused1,
      SOC_SAND_INOUT  void        *     unused2,
      SOC_SAND_INOUT  void        *     unused3,
       SOC_SAND_INOUT  void        *     unused4
 * REMARKS:
 *   Used for Arad+.  To be called when DMA related interrupt is triggered.
 * RETURNS:
 *   Nothing.
*********************************************************************/
void 
   arad_pp_oam_dma_event_handler(
      SOC_SAND_INOUT  void        *     unit,
      SOC_SAND_INOUT  void        *     unused1,
      SOC_SAND_INOUT  void        *     unused2,
      SOC_SAND_INOUT  void        *     unused3,
       SOC_SAND_INOUT  void        *     unused4
       );



/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_get_period
 * TYPE:
 *   PROC
 * FUNCTION:
 *    Return period of LB session (in miliseconds)
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
       SOC_SAND_OUT uint32                                              *period
 * REMARKS:
 *   Used for Arad+. Probably will be used only in one place.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_get_period(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_OUT uint32                                              *period
       );



/*********************************************************************
* NAME:
 *    arad_pp_oam_oamp_loopback_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *    End an OAM Loopback session
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32 
    arad_pp_oam_oamp_loopback_remove(
       SOC_SAND_IN  int                                                  unit
       );




/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_lookup
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print lookup information from OAM exact matches.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used by oam diagnostics
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_lookup(
     SOC_SAND_IN int unit
   );


/*********************************************************************
* NAME:
 *   arad_pp_get_crps_counter
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get value from crps crps counter register
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                    crps_counter_number -
 *     Number of the crps crps counter to display. Presumed to be 0,1,2 or 3.
 *   SOC_SAND_IN uint32          reg_number -
 *  Register number. Presumed to be between 0 and (32K-1).
 *     SOC_SAND_OUT uint64*           value
 *   Result goes here.
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
    arad_pp_get_crps_counter(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      );



/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_rx
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam rx information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_rx(
     SOC_SAND_IN int unit
   );


/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_print_em
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam EM-1 information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
     SOC_SAND_IN int LIF, use to  construct key
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_em(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   );


/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_print_ak
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam OAM-1/2 lookup key information.
 * INPUT:
 *   SOC_SAND_IN int unit
     SOC_SAND_IN ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_ak(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_OAM_ACTION_KEY_PARAMS *key_params
   );

/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_print_debug
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oamp counter information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_oamp_counter(
     SOC_SAND_IN int unit
   );

/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_print_debug
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam id debug information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
     SOC_SAND_IN int cfg, whether to  configure debug mode
     SOC_SAND_IN int mode, use to  configure debug mode
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_debug(
     SOC_SAND_IN int unit,
	 SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   );

/*********************************************************************
* NAME:
 *   arad_pp_oam_diag_print_oam_id
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print oam id debug information.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
     SOC_SAND_IN int mode, use to  configure debug mode
 * REMARKS:
 *   Used by oam diagnostics.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_oam_diag_print_oam_id(
     SOC_SAND_IN int unit
   );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_OAM_INCLUDED__*/
#endif
