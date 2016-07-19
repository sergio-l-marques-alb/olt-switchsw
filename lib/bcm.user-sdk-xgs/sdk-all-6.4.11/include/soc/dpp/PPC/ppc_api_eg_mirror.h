/* $Id: ppc_api_eg_mirror.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_eg_mirror.h
*
* MODULE PREFIX:  soc_ppc_eg
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

#ifndef __SOC_PPC_API_EG_MIRROR_INCLUDED__
/* { */
#define __SOC_PPC_API_EG_MIRROR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

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

typedef struct
{
    uint32 mirror_command;  /* mirror profile index*/
    uint8 forward_strength; /* forward strength value for forwarding competition with other apps */
    uint8 mirror_strength;  /* mirror strength  value for mirroring competition with other apps */
    uint8 forward_en;       /*enable forwarding flag*/
    uint8 mirror_en;        /*enable mirroring flag*/

} dpp_outbound_mirror_config_t;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The default of the port to enable/disable mirroring
   *  Relevant only for Soc_petra
   */
  uint8 enable_mirror;
  /*
   *  Default mirroring profile. Range 0-15. (0 means no mirroring)
   *  Not relevant for Soc_petra
   */
  uint32 dflt_profile;

} SOC_PPC_EG_MIRROR_PORT_DFLT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /* 
   *  Outbound mirror enable/disable
   *  Not relevant for Soc_petra
   */
  uint8 outbound_mirror_enable;
  /*
   *  The allocated internal port for outbound mirroring.
   *  Not relevant for Soc_petra
   */
  SOC_PPC_PORT outbound_port_ndx;  

} SOC_PPC_EG_MIRROR_PORT_INFO;


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

void
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  );

void
  SOC_PPC_EG_MIRROR_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_INFO *info
  );
#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  );

void
  SOC_PPC_EG_MIRROR_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_INFO *info
  );


#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_EG_MIRROR_INCLUDED__*/
#endif

