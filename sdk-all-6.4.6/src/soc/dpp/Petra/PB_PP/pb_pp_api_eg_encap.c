/* $Id: pb_pp_api_eg_encap.c,v 1.11 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_eg_encap.c
*
* MODULE PREFIX:  soc_pb_pp
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_encap.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
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
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                     *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = soc_pb_pp_eg_encap_range_info_set_verify(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_range_info_set_unsafe(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_set()", 0, 0);
}

/*********************************************************************
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO                     *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  res = soc_pb_pp_eg_encap_range_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_range_info_get_unsafe(
          unit,
          range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_range_info_get()", 0, 0);
}

/*********************************************************************
*     Set LIF Editing entry to be NULL Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_encap_null_lif_entry_add_verify(
          unit,
          lif_eep_ndx,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_null_lif_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_null_lif_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold AC ID. Actually maps from
 *     CUD to AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ac_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ac_entry_add_verify(
          unit,
          lif_eep_ndx,
          ac_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_ac_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          ac_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ac_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR SWAP label.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                      *swap_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(swap_info);

  /* Semaphore is taken before verify, because verify accesses the device to
  get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_swap_command_entry_add_verify(
          unit,
          lif_eep_ndx,
          swap_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_swap_command_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          swap_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_swap_command_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold PWE info (VC label and
 *     push profile).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                       *pwe_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_pwe_entry_add_verify(
          unit,
          lif_eep_ndx,
          pwe_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_pwe_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          pwe_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR POP command.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                       *pop_info,
    SOC_SAND_IN  uint32                                      next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pop_info);

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_pop_command_entry_add_verify(
          unit,
          lif_eep_ndx,
          pop_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_pop_command_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          pop_info,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pop_command_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
 * invalid for Soc_petra B.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_vsi_entry_add_verify(
          unit,
          lif_eep_ndx,
          vsi_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_vsi_entry_add_unsafe(
          unit,
          lif_eep_ndx,
          vsi_info,
          next_eep_valid,
          next_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_vsi_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Add MPLS tunnels encapsulation entry to the Tunnels
 *     Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO                *mpls_encap_info,
    SOC_SAND_IN  uint32                                      ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_encap_info);

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_mpls_entry_add_verify(
          unit,
          tunnel_eep_ndx,
          mpls_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_mpls_entry_add_unsafe(
          unit,
          tunnel_eep_ndx,
          mpls_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_mpls_entry_add()", tunnel_eep_ndx, 0);
}

/*********************************************************************
*     Add IPv4 tunnels encapsulation entry to the Egress
 *     Encapsulation Tunnels Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO                *ipv4_encap_info,
    SOC_SAND_IN  uint32                                      ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ipv4_encap_info);

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_entry_add_verify(
          unit,
          tunnel_eep_ndx,
          ipv4_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_ipv4_entry_add_unsafe(
          unit,
          tunnel_eep_ndx,
          ipv4_encap_info,
          ll_eep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_entry_add()", tunnel_eep_ndx, 0);
}

/*********************************************************************
*     Add LL encapsulation entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                        *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  /* Semaphore is taken before verify, because verify accesses the device to
     get eg encap table ranges */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ll_entry_add_verify(
          unit,
          ll_eep_ndx,
          ll_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_ll_entry_add_unsafe(
          unit,
          ll_eep_ndx,
          ll_encap_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ll_entry_add()", ll_eep_ndx, 0);
}

/*********************************************************************
*     Remove entry from the encapsulation Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_entry_remove_verify(
          unit,
          eep_type_ndx,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_pp_eg_encap_entry_remove_unsafe(
          unit,
          eep_type_ndx,
          eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_remove()", 0, eep_ndx);
}

/*********************************************************************
*     Get entry information from the Egress encapsulation
 *     tables.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                     encap_entry_info[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                      next_eep[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_eg_encap_entry_get_verify(
          unit,
          eep_type_ndx,
          eep_ndx,
          depth
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_entry_get_unsafe(
          unit,
          eep_type_ndx,
          eep_ndx,
          depth,
          encap_entry_info,
          next_eep,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_entry_get()", 0, eep_ndx);
}

/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = soc_pb_pp_eg_encap_push_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_push_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = soc_pb_pp_eg_encap_push_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_push_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY                   *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);

  res = soc_pb_pp_eg_encap_push_exp_info_set_verify(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_push_exp_info_set_unsafe(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_set()", 0, 0);
}

/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY                   *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  res = soc_pb_pp_eg_encap_push_exp_info_get_verify(
          unit,
          exp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_push_exp_info_get_unsafe(
          unit,
          exp_key,
          exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_push_exp_info_get()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_eg_encap_pwe_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO                  *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_eg_encap_pwe_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_pwe_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_pwe_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_IN  uint32                                      src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT uint32                                      *src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_ip);

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
          unit,
          entry_ndx,
          src_ip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get()", entry_ndx, 0);
}

/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl);

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
          unit,
          entry_ndx,
          ttl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tos);

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
          unit,
          entry_ndx,
          tos
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get()", entry_ndx, 0);
}

void
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_LL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->entry_type = SOC_PB_PP_NOF_EG_ENCAP_ENTRY_TYPES;
  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_EEP_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_EXP_MARK_MODE_to_string(enum_val);
}

const char*
  SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_TYPE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_ENTRY_TYPE_to_string(enum_val);
}

void
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_RANGE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_SWAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_LL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_VALUE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

