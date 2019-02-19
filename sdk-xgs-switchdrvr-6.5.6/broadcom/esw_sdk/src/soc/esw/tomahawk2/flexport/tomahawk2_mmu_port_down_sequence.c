/*
* $Id: $
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
*
* $File:  tomahawk2_mmu_port_down_sequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
/*! @file tomahawk2_mmu_port_down_sequence.c
 *  @brief
 */

#include <soc/flexport/tomahawk2_flexport.h>


/*! @fn int soc_tomahawk2_mmu_port_down_sequence(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port down operation.
 */
int
soc_tomahawk2_flex_mmu_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    uint64 temp64;

    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            COMPILER_64_SET(temp64, 0, TH2_MMU_FLUSH_ON);
            soc_tomahawk2_mmu_vbs_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
            soc_tomahawk2_mmu_rqe_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
            soc_tomahawk2_mmu_mtro_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
            sal_usleep(TH2_CT_FIFO_WAIT +
                       (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_mmu_reconfig_tdm(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t) {
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to reconfigure TDM calendars for MMU.
 */
int
soc_tomahawk2_flex_mmu_reconfigure_phase1(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    soc_tomahawk2_tdm_flexport_mmu(unit, port_schedule_state_t);
    _soc_tomahawk2_tdm_mmu_calendar_set(unit, port_schedule_state_t);
    _soc_tomahawk2_tdm_mmu_hsp_set(unit, port_schedule_state_t);
    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK2_SUPPORT */
