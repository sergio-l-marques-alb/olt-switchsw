/* $Id: pcp_statistics.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_statistics.h>
#include <soc/dpp/PCP/pcp_oam_general.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_STATISTICS_COUNTER_NDX_MAX                         (16384)
#define PCP_STATISTICS_BYTES_MAX                               (SOC_SAND_U32_MAX)
#define PCP_STATISTICS_PACKETS_MAX                             (SOC_SAND_U32_MAX)

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
    Pcp_procedure_desc_element_statistics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTER_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_ING_COUNTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTER_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTER_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTER_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STS_EGR_COUNTER_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STATISTICS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_STATISTICS_GET_ERRS_PTR),
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
    Pcp_error_desc_element_statistics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_STATISTICS_COUNTER_NDX_OUT_OF_RANGE_ERR,
    "PCP_STATISTICS_COUNTER_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'counter_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_STATISTICS_BYTES_OUT_OF_RANGE_ERR,
    "PCP_STATISTICS_BYTES_OUT_OF_RANGE_ERR",
    "The parameter 'bytes' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR,
    "PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR",
    "The parameter 'packets' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
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
*     This API retrieves statistics on the specified ingress
 *     counter index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_sts_ing_counter_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx,
    SOC_SAND_OUT PCP_STS_ING_COUNTER_INFO        *value
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_STS_ING_COUNTERS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(value);

  PCP_STS_ING_COUNTER_INFO_clear(value);

  res = pcp_sts_ing_counters_tbl_get_unsafe(
          unit,
          counter_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  value->bytes.arr[0] = tbl_data.octets[0];
  value->bytes.arr[1] = tbl_data.octets[1];
  value->packets.arr[0] = tbl_data.packets[0];
  value->packets.arr[1] = tbl_data.packets[1];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_ing_counter_get_unsafe()", counter_ndx, 0);
}

uint32
  pcp_sts_ing_counter_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_ING_COUNTER_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, PCP_STATISTICS_COUNTER_NDX_MAX, PCP_STATISTICS_COUNTER_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_ing_counter_get_verify()", counter_ndx, 0);
}

/*********************************************************************
*     This API retrieves statistics on the specified egress
 *     counter index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_sts_egr_counter_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx,
    SOC_SAND_OUT PCP_STS_EGR_COUNTER_INFO        *value
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_STS_EGR_COUNTERS_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(value);

  PCP_STS_EGR_COUNTER_INFO_clear(value);

  res = pcp_sts_egr_counters_tbl_get_unsafe(
          unit,
          counter_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  value->packets.arr[0] = tbl_data.packets[0];
  value->packets.arr[1] = tbl_data.packets[1];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_egr_counter_get_unsafe()", counter_ndx, 0);
}

uint32
  pcp_sts_egr_counter_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        counter_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_STS_EGR_COUNTER_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, PCP_STATISTICS_COUNTER_NDX_MAX, PCP_STATISTICS_COUNTER_NDX_OUT_OF_RANGE_ERR, 10, exit);

   /* IMPLEMENTED */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_sts_egr_counter_get_verify()", counter_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_statistics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_statistics_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_statistics;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_statistics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_statistics_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_statistics;
}
uint32
  PCP_STS_ING_COUNTER_INFO_verify(
    SOC_SAND_IN  PCP_STS_ING_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bytes.arr[0], PCP_STATISTICS_BYTES_MAX, PCP_STATISTICS_BYTES_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bytes.arr[1], PCP_STATISTICS_BYTES_MAX, PCP_STATISTICS_BYTES_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->packets.arr[0], PCP_STATISTICS_PACKETS_MAX, PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->packets.arr[1], PCP_STATISTICS_PACKETS_MAX, PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_STS_ING_COUNTER_INFO_verify()",0,0);
}

uint32
  PCP_STS_EGR_COUNTER_INFO_verify(
    SOC_SAND_IN  PCP_STS_EGR_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->packets.arr[0], PCP_STATISTICS_PACKETS_MAX, PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->packets.arr[1], PCP_STATISTICS_PACKETS_MAX, PCP_STATISTICS_PACKETS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_STS_EGR_COUNTER_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

