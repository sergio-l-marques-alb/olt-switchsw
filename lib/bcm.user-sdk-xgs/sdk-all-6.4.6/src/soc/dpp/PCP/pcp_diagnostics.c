/* $Id: pcp_diagnostics.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_diagnostics.h>


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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_diagnostics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_STS_SYNC_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_STS_SYNC_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_STS_SYNC_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_STS_SYNC_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_DMA_RX_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_DMA_RX_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_DMA_RX_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_DMA_RX_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAGNOSTICS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAGNOSTICS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_diagnostics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     This API returns the sync status of the statistics
 *     interface
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_diag_sts_sync_status_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint8                       *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_DIAG_STS_SYNC_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_diag_sts_sync_status_get_unsafe()", 0, 0);
}

uint32
  pcp_diag_sts_sync_status_get_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_DIAG_STS_SYNC_STATUS_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_diag_sts_sync_status_get_verify()", 0, 0);
}

/*********************************************************************
*     This API returns if there are any packets waiting to be
 *     handled by the CPU on the class specified
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_diag_dma_rx_status_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                      traffic_class,
    SOC_SAND_OUT uint8                       *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_DIAG_DMA_RX_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_diag_dma_rx_status_get_unsafe()", 0, 0);
}

uint32
  pcp_diag_dma_rx_status_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_PP_TC                      traffic_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_DIAG_DMA_RX_STATUS_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_diag_dma_rx_status_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_diagnostics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_diagnostics_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_diagnostics;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_diagnostics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_diagnostics_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_diagnostics;
}
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

