/* $Id: chip_sim_task.h,v 1.4 Broadcom SDK $
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

#ifndef _CHIP_SIM_TASK_H_
/* { */
#define _CHIP_SIM_TASK_H_


#ifdef  __cplusplus
extern "C" {
#endif

#include "chip_sim.h"
#include <appl/diag/dpp/dune_chips.h>
#include <appl/diag/dpp/utils_defx.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
/* } */
#else
/* { */
/* } */
#endif



typedef UINT32 (*UINT32FUNCPTR) (void);
typedef int (*FUNCPTR_TO_SCREEN) (const char *out_string,int  add_cr_lf);

extern UINT32FUNCPTR chip_sim_task_get_time_in_microseconds;
extern FUNCPTR chip_sim_to_screen;

STATUS chip_sim_start(
    SOC_SAND_IN char *sim_task_name,
    SOC_SAND_IN int  sim_task_prio,
    SOC_SAND_IN int  sim_task_stack_size,
    SOC_SAND_IN int  sim_task_task_id,
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
    SOC_SAND_IN unsigned int     chip_ver,
    SOC_SAND_IN UINT32 start_address[CHIP_SIM_NOF_CHIPS],
    SOC_SAND_IN DESC_FE_REG  *desc,
    FUNCPTR_TO_SCREEN to_screen,
    UINT32FUNCPTR get_time_in_microseconds,
    SOC_SAND_IN uint8 hock_int_handler
    );
STATUS chip_sim_end(char msg[]);
uint8 chip_sim_task_is_alive(void);
UINT32 chip_sim_task_get_mili_time(void);
int    chip_sim_task_get_tid(void);
void chip_sim_task_set_sleep_time(SOC_SAND_IN UINT32 mili_sec);
UINT32 chip_sim_task_get_work_time(void);
UINT32 chip_sim_task_get_sleep_time(void);
void   chip_sim_task_wake_up(void);


#ifdef  __cplusplus
}
#endif


/* } _CHIP_SIM_TASK_H_*/
#endif


