/*
 * $Id: fe1600_fabric_multicast.h,v 1.2 Broadcom SDK $
 *
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
 *
 * FE1600 FABRIC MULTICAST H
 */
 
#ifndef _SOC_FE1600_FABRIC_MULTICAST_H_
#define _SOC_FE1600_FABRIC_MULTICAST_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

soc_error_t soc_fe1600_fabric_multicast_low_prio_drop_select_priority_set(int unit, soc_dfe_fabric_priority_t prio);
soc_error_t soc_fe1600_fabric_multicast_low_prio_drop_select_priority_get(int unit, soc_dfe_fabric_priority_t* prio);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_validate(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_set(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_prio_threshold_get(int unit, bcm_fabric_control_t type, int* arg);
soc_error_t soc_fe1600_fabric_multicast_low_priority_drop_enable_set(int unit, int arg);
soc_error_t soc_fe1600_fabric_multicast_low_priority_drop_enable_get(int unit, int* arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_validate(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_set(int unit, bcm_fabric_control_t type, int arg);
soc_error_t soc_fe1600_fabric_multicast_priority_range_get(int unit, bcm_fabric_control_t type, int* arg);
soc_error_t soc_fe1600_fabric_multicast_multi_set(int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
soc_error_t soc_fe1600_fabric_multicast_multi_get(int unit, uint32 flags, uint32 ngroups, bcm_multicast_t *groups, bcm_fabric_module_vector_t *dest_array);
#endif /*_SOC_FE1600_FABRIC_MULTICAST_H_*/
