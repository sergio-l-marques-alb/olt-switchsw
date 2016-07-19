/* $Id: soc_pb_pp_ssr.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PP_SSR_H_INCLUDED__
/* { */
#define __SOC_PB_PP_SSR_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_api_ssr.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_ssr.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_SSR_BASE_SAND_VER 2830 /* first driver version supporting soc_petra SSR */

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

typedef struct
{
  SOC_PB_PP_SW_DB_DEVICE_SSR_V01  ssr_v01;

}  SOC_PB_PP_SSR_DATA_V01;

typedef struct
{
  SOC_PB_PP_SW_DB_DEVICE_SSR_V02  ssr_v01;

}  SOC_PB_PP_SSR_DATA_V02;

typedef SOC_PB_PP_SSR_DATA_V01 SOC_PB_PP_SSR_DATA;

typedef struct
{
  uint32
    prev_unit;

  uint32
    fap_id;

}  SOC_PB_PP_SSR_HEADER;

typedef struct
{
  SOC_PB_PP_SSR_HEADER device_header;

  SOC_SAND_SSR_HEADER   header;

  union
  {
    SOC_PB_PP_SSR_DATA ssr_data;

    uint8         stream[sizeof(SOC_PB_PP_SSR_DATA)];
  }info;

}  SOC_PB_PP_SSR_BUFF;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_SSR_BUFF_SIZE_GET = SOC_PB_PP_PROC_DESC_BASE_SSR_FIRST,
  SOC_PB_PP_SSR_BUFFER_SAVE,
  SOC_PB_PP_SSR_BUFFER_LOAD,
  SOC_PB_PP_SSR_IS_DEVICE_INIT_DONE,
  SOC_PB_PP_SSR_FORBIDDEN_VER_SIZE,
  SOC_PB_PP_SSR_FORBIDDEN_VER_TRANS,
  SOC_PB_PP_SSR_MAX_SW_DB_SIZE_GET,
  SOC_PB_PP_SSR_SW_DB_SIZE_GET,
  SOC_PB_PP_SSR_CFG_VERSION_GET,
  SOC_PB_PP_SSR_TRANSFORM_DB2CURR,
  SOC_PB_PP_SSR_DATA_LOAD,
  SOC_PB_PP_SSR_DATA_SAVE,
  SOC_PB_PP_SSR_IS_DEVICE_INIT_DONE_UNSAFE,
  /*
   * } Auto generated. Do not edit previous section.
   */
   

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_SSR_PROCEDURE_DESC_LAST
} SOC_PB_PP_SSR_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_SSR_FORBIDDEN_FUNCTION_CALL_ERR = SOC_PB_PP_ERR_DESC_BASE_SSR_FIRST,
  SOC_PB_PP_SSR_INCOMPATIBLE_SRC_VERSION_ERR,
  SOC_PB_PP_SW_DB_BUFF_SIZE_MISMATCH_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   
  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_SSR_ERR_LAST
} SOC_PB_PP_SSR_ERR;
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
  soc_pb_pp_ssr_sw_db_size_get(
    SOC_SAND_IN  uint32                      ver_idx,
    SOC_SAND_OUT uint32                       *size
  );

uint32
  soc_pb_pp_ssr_to_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_OUT uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  );

uint32
  soc_pb_pp_ssr_from_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_ssr_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_ssr module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_ssr_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_ssr_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_ssr module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_ssr_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_SSR_H_INCLUDED__*/
#endif
