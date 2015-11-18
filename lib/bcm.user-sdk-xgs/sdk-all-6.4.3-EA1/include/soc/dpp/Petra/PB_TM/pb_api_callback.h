/* $Id: soc_pb_api_callback.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_API_CALLBACK_INCLUDED__
/* { */
#define __SOC_PB_API_CALLBACK_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_api_interrupt_service.h>

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
*     soc_petra_callback_function_register
* TYPE:
*   PROC
* DATE:
*   Sep  9 2007
* FUNCTION:
*     This procedure registers the asked function
*     ('function_id') to be called later on (interrupt or
*     periodically)
* INPUT:
*  SOC_SAND_IN  uint32                 function_ndx -
*     function_ndx - Identifier of API service to be invoked.
*     SOC_SAND_IN
*  SOC_SAND_IN  SOC_SAND_USER_CALLBACK*       user_callback_structure -
*     Parameter the function induced by 'function_id', and
*     user supplied callback function SOC_SAND_IN
*  SOC_SAND_OUT  uint32*                 callback_id -
*     Loaded with identifier to the callback service. To be
*     used to unregistered this service.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_callback_function_register(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE        interrupt_cause,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK        *user_callback_structure,
    SOC_SAND_OUT uint32                  *callback_id
  );

/*********************************************************************
* NAME:
*     soc_petra_unregister_callback_function
* TYPE:
*   PROC
* DATE:
*   Sep  9 2007
* FUNCTION:
*     This procedure stop an ongoing deferred service, using
*     the 'callback_id' that was handed over to the user when
*     the service was registered.
* INPUT:
*  SOC_SAND_IN  uint32                 driver_callback_id -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_callback_function_unregister(
    SOC_SAND_IN  uint32                 driver_callback_id
  );

/*********************************************************************
* NAME:
*     soc_petra_callback_all_functions_unregister
* TYPE:
*   PROC
* DATE:
*   Sep  9 2007
* FUNCTION:
*     This procedure stop an all ongoing deferred services
*     from the device.
* INPUT:
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_callback_all_functions_unregister(
    SOC_SAND_IN  int                 unit
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_CALLBACK_INCLUDED__*/
#endif
