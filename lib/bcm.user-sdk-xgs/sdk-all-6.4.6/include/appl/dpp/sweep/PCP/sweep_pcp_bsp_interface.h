/* $Id: sweep_pcp_bsp_interface.h,v 1.4 Broadcom SDK $
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


#ifndef __SWEEP_PCP_BSP_INTERFACE_H_INCLUDED
/* { */
#define __SWEEP_PCP_BSP_INTERFACE_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <appl/diag/dpp/utils_host_board.h>

#include <soc/dpp/PCP/pcp_api_mgmt.h>

#include <appl/dpp/sweep/Utils/sweep_err_defs.h>

#include <appl/dpp/sweep/PCP/sweep_pcp_app.h>

#ifndef __KERNEL__
#include <stdio.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SWEEP_PCP_BSP_INTERFACE_GFA_BI   (1)
#define SWEEP_PCP_BSP_INTERFACE_WIN      (2)

#define SWEEP_PCP_DEFAULT_FILE_NAME      "pcp_sweep.txt"

#define SWEEP_PCP_MSG_SIZE 80

#ifdef __DUNE_GTO_BCM_CPU__
    #define SWEEP_PCP_BSP_INTERFACE        (SWEEP_PCP_BSP_INTERFACE_GFA_BI)
#else
	#ifdef __VXWORKS__
	  #define SWEEP_PCP_BSP_INTERFACE        (SWEEP_PCP_BSP_INTERFACE_GFA_BI)
	#else
	  #define SWEEP_PCP_BSP_INTERFACE        (SWEEP_PCP_BSP_INTERFACE_WIN)
	#endif
#endif

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

SOC_SAND_RET
  swp_pcp_bsp_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

SOC_SAND_RET
  swp_pcp_bsp_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 );

uint32
  sweep_pcp_bsp_base_address(
    SOC_SAND_IN  uint8 is_csr,
    SOC_SAND_OUT uint32  **base
  );

uint32
  sweep_pcp_bsp_system_pcp_id_get(
    SOC_SAND_OUT uint32 *system_pcp_id
  );

void
  sweep_pcp_bsp_usr_app_flavor_get(
     SOC_D_USR_APP_FLAVOR* usr_app_flavor
  );

SOC_SAND_RET
  swp_pcp_bsp_reset(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  remain_active
  );

/*
 * File functions
*/

#ifndef __KERNEL__
uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_open(
    SOC_SAND_OUT  FILE        **file_p,
    SOC_SAND_IN   uint8   silent
  );

uint32
  sweep_pcp_bsp_SWEEP_PCP_DEVICE_INIT_PARAMS_FILE_close(
    SOC_SAND_OUT  FILE        **file_p,
    SOC_SAND_IN   uint8   silent
  );
#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SWEEP_PCP_BSP_INTERFACE_H_INCLUDED*/
#endif
