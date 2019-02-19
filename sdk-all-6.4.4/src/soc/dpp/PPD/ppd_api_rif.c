/* $Id: ppd_api_rif.c,v 1.13 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_rif.c
*
* MODULE PREFIX:  ppd
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_rif.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_rif.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_rif.h>
#endif

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_rif[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_MPLS_LABELS_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_MPLS_LABELS_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_MPLS_LABEL_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_MPLS_LABEL_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_MPLS_LABEL_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_TERM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_TERM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_IP_TUNNEL_TERM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_VSID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_VSID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_VSID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_VSID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_TTL_SCOPE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_TTL_SCOPE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_TTL_SCOPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_TTL_SCOPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_RIF_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Set the MPLS labels that may be mapped to Router
 *     Interfaces
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_mpls_labels_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_MPLS_LABELS_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_mpls_labels_range_set,(unit, rif_labels_range));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_mpls_labels_range_set_print,(unit,rif_labels_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_labels_range_set()", 0, 0);
}

/*********************************************************************
*     Set the MPLS labels that may be mapped to Router
 *     Interfaces
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_mpls_labels_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_MPLS_LABELS_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_mpls_labels_range_get,(unit, rif_labels_range));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_mpls_labels_range_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_labels_range_get()", 0, 0);
}

/*********************************************************************
*     Enable MPLS labels termination and setting the Router
 *     interface according to the terminated MPLS label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_mpls_label_map_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPD_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_MPLS_LABEL_MAP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_mpls_label_map_add,(unit, mpls_key, lif_index, term_info, rif_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_mpls_label_map_add_print,(unit,mpls_key,lif_index,term_info,rif_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_add()", 0, 0);
}

/*********************************************************************
*     Remove MPLS label that was mapped to a RIF-Tunnel
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_mpls_label_map_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT SOC_PPD_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_MPLS_LABEL_MAP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_mpls_label_map_remove,(unit, mpls_key, lif_index));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_mpls_label_map_remove_print,(unit,mpls_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_remove()", 0, 0);
}

/*********************************************************************
*     Get MPLS label termination and Router interface info
 *     according to the terminated MPLS label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_mpls_label_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPD_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_MPLS_LABEL_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_mpls_label_map_get,(unit, mpls_key, lif_index, term_info, rif_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_mpls_label_map_get_print,(unit,mpls_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_mpls_label_map_get()", 0, 0);
}

/*********************************************************************
*     Enable IP Tunnels termination and setting the Router
 *     interface according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_map_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_MAP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);
  
  SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_add,(unit, dip_key, lif_index, term_info, rif_info, success));
  
  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_ip_tunnel_map_add_print,(unit,dip_key,lif_index,term_info,rif_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_map_add()", dip_key, 0);
}

/*********************************************************************
*     Remove the IP Tunnel
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_map_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_MAP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_remove,(unit, dip_key, lif_index));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_ip_tunnel_map_remove_print,(unit,dip_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_map_remove()", dip_key, 0);
}

/*********************************************************************
*     Get IP Tunnels termination and Router interface info
 *     according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPD_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_get,(unit, dip_key, lif_index, term_info, rif_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_ip_tunnel_map_get_print,(unit,dip_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_map_get()", dip_key, 0);
}


/* IP tunnel termination according to IP-term key */

/*********************************************************************
*     Enable IP Tunnels termination and setting the Router
 *     interface according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_term_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_TERM_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB){
      SOC_SAND_CHECK_NULL_INPUT(rif_info);
      /* termination only for DIP in soc_petra-B*/
      if(term_key->flags != SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP) {
          SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 999, exit);
      }
      SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_add,(unit, term_key->dip, lif_index, term_info, rif_info, success));
  }
  else if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD){
      /* IF rif_info null then rif info is not updated */
      SOC_PPD_ARAD_ONLY_DEVICE_CALL(rif_ip_tunnel_term_add,(unit, term_key, lif_index, term_info, rif_info, success));
  }
  else{
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit);
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_add()", 0, 0);
}

/*********************************************************************
*     Remove the IP Tunnel
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_term_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_KEY              *term_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                           *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_TERM_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
 
  if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB){
      /* termination only for DIP in soc_petra-B*/
      if(term_key->flags != SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP) {
          SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 999, exit);
      }

      SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_remove,(unit, term_key->dip, lif_index));
  }
  else if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD){

      SOC_PPD_ARAD_ONLY_DEVICE_CALL(rif_ip_tunnel_term_remove,(unit, term_key, lif_index));
  }
  else{
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit);
  }

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_remove()", 0, 0);
}

/*********************************************************************
*     Get IP Tunnels termination and Router interface info
 *     according to the terminated IP tunnel.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ip_tunnel_term_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT SOC_PPD_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PPD_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PPD_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_IP_TUNNEL_TERM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB){
      /* termination only for DIP in soc_petra-B*/
      if(term_key->flags != SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP) {
          SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 999, exit);
      }

      SOC_PPD_DEVICE_CALL(rif_ip_tunnel_map_get,(unit, term_key->dip, lif_index, term_info, rif_info, found));
  }
  else if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD){
      SOC_PPD_ARAD_ONLY_DEVICE_CALL(rif_ip_tunnel_term_get,(unit, term_key, lif_index, term_info, rif_info, found));
  }
  else{
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit);
  }


  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ip_tunnel_term_get()", 0, 0);
}


/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_vsid_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_VSID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_vsid_map_set,(unit, vsid_ndx, rif_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_vsid_map_set_print,(unit,vsid_ndx,rif_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_vsid_map_set()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_vsid_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PPD_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_VSID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_vsid_map_get,(unit, vsid_ndx, rif_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_vsid_map_get_print,(unit,vsid_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_vsid_map_get()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_info_set,(unit, rif_ndx, rif_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_info_set_print,(unit,rif_ndx,rif_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_info_set()", rif_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PPD_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_info_get,(unit, rif_ndx, rif_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_info_get_print,(unit,rif_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_info_get()", rif_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ttl_scope_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_TTL_SCOPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_ttl_scope_set,(unit, ttl_scope_ndx, ttl_val));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_ttl_scope_set_print,(unit,ttl_scope_ndx,ttl_val));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ttl_scope_set()", ttl_scope_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_rif_ttl_scope_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                          *ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_RIF_TTL_SCOPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl_val);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(rif_ttl_scope_get,(unit, ttl_scope_ndx, ttl_val));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_rif_ttl_scope_get_print,(unit,ttl_scope_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_ttl_scope_get()", ttl_scope_ndx, 0);
}


 
/*********************************************************************
*     set native vsi compensation per rif
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
soc_ppd_rif_native_routing_vlan_tags_set(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPD_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(rif_native_routing_vlan_tags_set, (unit, intf_id, native_routing_vlan_tags)); 
    
    SOC_PPD_DO_NOTHING_AND_EXIT; 
    
exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_rif_native_routing_vlan_tags_set, (unit, intf_id, native_routing_vlan_tags)); 
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_rif_native_routing_vlan_tags_set()", 0, 0);
} 



/*********************************************************************
*     get native vsi compensation per rif
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
soc_ppd_rif_native_routing_vlan_tags_get(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  SOC_PPD_RIF_ID                         intf_id, 
   SOC_SAND_OUT  uint8                                 *native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(rif_native_routing_vlan_tags_get, (unit, intf_id, native_routing_vlan_tags)); 
    
    SOC_PPD_DO_NOTHING_AND_EXIT; 
    
exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_rif_native_routing_vlan_tags_get, (unit, intf_id, native_routing_vlan_tags)); 
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_native_routing_vlan_tags_get()", 0, 0);
}



/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_rif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_rif_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_rif;
}
void
  SOC_PPD_RIF_MPLS_LABELS_RANGE_clear(
    SOC_SAND_OUT SOC_PPD_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MPLS_LABEL_RIF_KEY_clear(
    SOC_SAND_OUT SOC_PPD_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_INFO_clear(
    SOC_SAND_OUT SOC_PPD_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_IP_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPD_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_IP_TERM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_IP_TERM_KEY_clear(
    SOC_SAND_OUT SOC_PPD_RIF_IP_TERM_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_RIF_IP_TERM_KEY_clear(key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_RIF_ROUTE_ENABLE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_RIF_ROUTE_ENABLE_TYPE enum_val
  )
{
  return SOC_PPC_RIF_ROUTE_ENABLE_TYPE_to_string(enum_val);
}

void
  SOC_PPD_RIF_MPLS_LABELS_RANGE_print(
    SOC_SAND_IN  SOC_PPD_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_MPLS_LABELS_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MPLS_LABEL_RIF_KEY_print(
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_LABEL_RIF_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_INFO_print(
    SOC_SAND_IN  SOC_PPD_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_IP_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_IP_TERM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_RIF_IP_TERM_KEY_print(
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_RIF_IP_TERM_KEY_print(key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_rif_mpls_labels_range_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rif_labels_range:")));
  SOC_PPD_RIF_MPLS_LABELS_RANGE_print((rif_labels_range));

  return;
}
void
  soc_ppd_rif_mpls_labels_range_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_rif_mpls_label_map_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPD_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mpls_key:")));
  SOC_PPD_MPLS_LABEL_RIF_KEY_print((mpls_key));

  LOG_CLI((BSL_META_U(unit,
                      "lif_index: %lu\n\r"),lif_index));

  LOG_CLI((BSL_META_U(unit,
                      "term_info:")));
  SOC_PPD_MPLS_TERM_INFO_print((term_info));

  LOG_CLI((BSL_META_U(unit,
                      "rif_info:")));
  SOC_PPD_RIF_INFO_print((rif_info));

  return;
}
void
  soc_ppd_rif_mpls_label_map_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mpls_key:")));
  SOC_PPD_MPLS_LABEL_RIF_KEY_print((mpls_key));

  return;
}
void
  soc_ppd_rif_mpls_label_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MPLS_LABEL_RIF_KEY                  *mpls_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mpls_key:")));
  SOC_PPD_MPLS_LABEL_RIF_KEY_print((mpls_key));

  return;
}
void
  soc_ppd_rif_ip_tunnel_map_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PPD_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "dip_key: %lu\n\r"),dip_key));

  LOG_CLI((BSL_META_U(unit,
                      "lif_index: %lu\n\r"),lif_index));

  LOG_CLI((BSL_META_U(unit,
                      "term_info:")));
  SOC_PPD_RIF_IP_TERM_INFO_print((term_info));

  LOG_CLI((BSL_META_U(unit,
                      "rif_info:")));
  SOC_PPD_RIF_INFO_print((rif_info));

  return;
}
void
  soc_ppd_rif_ip_tunnel_map_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "dip_key: %lu\n\r"),dip_key));

  return;
}
void
  soc_ppd_rif_ip_tunnel_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                dip_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "dip_key: %lu\n\r"),dip_key));

  return;
}
void
  soc_ppd_rif_vsid_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsid_ndx: %lu\n\r"),vsid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "rif_info:")));
  SOC_PPD_RIF_INFO_print((rif_info));

  return;
}
void
  soc_ppd_rif_vsid_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsid_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsid_ndx: %lu\n\r"),vsid_ndx));

  return;
}
void
  soc_ppd_rif_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PPD_RIF_INFO                            *rif_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rif_ndx: %lu\n\r"),rif_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "rif_info:")));
  SOC_PPD_RIF_INFO_print((rif_info));

  return;
}
void
  soc_ppd_rif_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_RIF_ID                              rif_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rif_ndx: %lu\n\r"),rif_ndx));

  return;
}
void
  soc_ppd_rif_ttl_scope_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl_val
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "ttl_scope_ndx: %u\n\r"),ttl_scope_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ttl_val: %u\n\r"), ttl_val));

  return;
}
void
  soc_ppd_rif_ttl_scope_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               ttl_scope_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "ttl_scope_ndx: %u\n\r"),ttl_scope_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

