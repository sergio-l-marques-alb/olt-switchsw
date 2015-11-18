/* $Id: petra_dram.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_dram.h
*
* MODULE PREFIX:  soc_petra_dram
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

#ifndef __SOC_PETRA_DRAM_INCLUDED__
/* { */
#define __SOC_PETRA_DRAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_dram.h>

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
*   soc_petra_dram_info_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   This function configures the dram according to the
*   provided sets of parameters. This function is called
*   during the initialization sequence and must not be
*   called afterwards
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                 dram_ndx -
*     Dram index. Range: 0-5
*   SOC_SAND_IN  uint32                 dram_freq -
*     Dram frequency (MHz)
*   SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type -
*     One of three supported dram types (DDR2, DDR3, GDDR3)
*   SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info -
*     Dram configuration information.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dram_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info
  );

/*********************************************************************
* NAME:
*   soc_petra_dram_info_verify
* TYPE:
*   PROC
* FUNCTION:
*   This function configures the dram according to the
*   provided sets of parameters. This function is called
*   during the initialization sequence and must not be
*   called afterwards
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                 dram_freq -
*     Dram frequency (MHz)
*   SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type -
*     One of three supported dram types (DDR2, DDR3, GDDR3)
*   SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info -
*     Dram configuration information.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dram_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info
  );

/*********************************************************************
* NAME:
*   soc_petra_dram_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   This function configures the dram according to the
*   provided sets of parameters. This function is called
*   during the initialization sequence and must not be
*   called afterwards
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                 dram_ndx -
*     Dram index. Range: 0-5
*   SOC_SAND_IN  uint32                 dram_freq -
*     Dram frequency (MHz)
*   SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type -
*     One of three supported dram types (DDR2, DDR3, GDDR3)
*   SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *info -
*     Dram configuration information.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dram_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_petra_dram_window_validity_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Diagnostic tool to get the DRAM valid window size
 *   through BIST commands. .
 * INPUT:
 *   SOC_SAND_IN  int                                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                    dram_ndx -
 *     Dram index. Range: 0 - 5.
 *   SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                           *pattern -
 *     Pattern used for the BIST commands.
 *   SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                       *window_info -
 *     Parameters characterizing the DRAM window validity.
 * REMARKS:
 *   1. This API is used for diagnostic only. NO traffic can
 *   go through the device after the call to this API and no
 *   other API must be called after this API.2. The expected
 *   ratio is between 50% and 70%.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dram_window_validity_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern,
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                    *window_info
  );

uint32
  soc_petra_dram_window_validity_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern
  );

#if SOC_PETRA_DEBUG_IS_LVL1
uint32
  soc_petra_dram_pckt_from_buff_verify(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  );
uint32
  soc_petra_dram_pckt_from_buff_read_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  );
#endif /* SOC_PETRA_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_DRAM_INCLUDED__*/
#endif
