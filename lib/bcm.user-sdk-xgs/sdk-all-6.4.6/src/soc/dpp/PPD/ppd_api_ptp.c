/* $Id: ppd_api_ptp.c,v 1.4 Broadcom SDK $
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
#include <soc/dpp/PPD/ppd_api_ptp.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_ptp.h>

#include <soc/dpp/PPD/ppd_api_general.h>
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
    Ppd_procedure_desc_element_ptp[] =
{
  /*
  * Auto generated. Do not edit following section {
  */
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PTP_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PTP_PORT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PTP_PORT_GET),

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
 *   This function is used to initialize all the general 1588 
 *   registers/tables need for 1588 configuration.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_ptp_init
(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PTP_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(ptp_init,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_ptp_init, (unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_ptp_init()", 0, 0);
}


/*********************************************************************
 *   This function is used to initialize all the general 1588 
 *   registers/tables need for 1588 configuration.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_ptp_port_set
(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_IN  SOC_PPD_PTP_PORT_INFO                  *info,
    SOC_SAND_IN  SOC_PPD_PTP_IN_PP_PORT_PROFILE        profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PTP_PORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(ptp_port_set,(unit, local_port_ndx, info, profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_ptp_init, (unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_ptp_port_set()", 0, 0);
}


/*********************************************************************
 *   This function is used to initialize all the general 1588 
 *   registers/tables need for 1588 configuration.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_ptp_port_get
(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_OUT SOC_PPD_PTP_IN_PP_PORT_PROFILE         *profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PTP_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(ptp_port_get,(unit, local_port_ndx, profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_ptp_init, (unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_ptp_port_get()", 0, 0);
}







/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_port module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_ptp_get_procs_ptr(void)
{
  return Ppd_procedure_desc_element_ptp;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

