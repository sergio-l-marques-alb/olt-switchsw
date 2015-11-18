/* $Id: chip_sim.h,v 1.8 Broadcom SDK $
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
#ifndef _CHIP_SIM_H_
/* { */
#define _CHIP_SIM_H_

#include <stdio.h>

/*
 * General include file for reference design.
 */

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

/*#if !(defined(LINUX) || defined(UNIX))
  #include <appl/dpp/../H/DuneTypeDef.h>
#endif*/

#if (defined(LINUX) || defined(UNIX))
  #include <appl/diag/dpp/utils_defx.h>
#endif

#ifdef NUM_FE_ON_BOARD
  #define CHIP_SIM_NOF_CHIPS NUM_FE_ON_BOARD
#else
  #define CHIP_SIM_NOF_CHIPS 2
#endif

#define FUNC_EXIT_POINT exit_point
#define GOTO_FUNC_EXIT_POINT goto exit_point ;


#define INVALID_ADDRESS 0xFFFFFFFF


#ifndef UINT32
  #define UINT32 unsigned int
#endif

#ifndef INT32
  #define INT32 signed int
#endif

#ifndef STATUS
  #define STATUS int
#endif

#ifndef OK
  #define OK 0
#endif

#ifndef ERROR
  #define ERROR 1
#endif

#ifdef _MSC_VER
typedef int (*FUNCPTR) ();     /* ptr to function returning int */
#endif

#if 0
  #define CHIP_SIM_MALLOC(x) malloc(x)
  #define CHIP_SIM_FREE(x)   free(x)
#else
  #define CHIP_SIM_MALLOC(x,str) soc_sand_os_malloc_any_size(x, str)
  #define CHIP_SIM_FREE(x)   soc_sand_os_free(x)
#endif

#ifdef _MSC_VER
  #define CHIP_SIM_BYTE_SWAP(x) (x)
#else
  #define CHIP_SIM_BYTE_SWAP(x) (x)
#endif

#if defined(PLISIM) && defined(DUNE_BCM) && LINK_PB_LIBRARIES
/* Dune ChipSim memory buffer */
#include <soc/dpp/Petra/petra_chip_defines.h>

extern uint32
  Soc_pb_reg_buffer[CHIP_SIM_NOF_CHIPS][SOC_PB_TOTAL_SIZE_OF_REGS];
#endif

/* }  _CHIP_SIM_H_ */
#endif

