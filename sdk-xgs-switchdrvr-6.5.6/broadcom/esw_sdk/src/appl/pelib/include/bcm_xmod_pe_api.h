/*
 * $Id:$
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
 * File:       bcm_xmod_pe_api.h
 */

#include "bcm_xmod_api.h"


#define SBUS_LOCK_INIT if (sbus_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_sbus_mutex_init())
#define SBUS_LOCK bcm_mutex_take(sbus_lock, sal_mutex_FOREVER)
#define SBUS_UNLOCK bcm_mutex_give(sbus_lock)

#define AXI_LOCK_INIT if (axi_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_axi_mutex_init())
#define AXI_LOCK bcm_mutex_take(axi_lock, sal_mutex_FOREVER)
#define AXI_UNLOCK bcm_mutex_give(axi_lock)

#define XMOD_LOCK_INIT if (sbus_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_xmod_mutex_init())
#define XMOD_LOCK bcm_mutex_take(xmod_lock, sal_mutex_FOREVER)
#define XMOD_UNLOCK bcm_mutex_give(xmod_lock)

extern int bcm_initialize(void);
extern int bcm_sbus_mutex_init(void);
extern void bcm_sbus_mutex_cleanup(void);
extern int bcm_axi_mutex_init(void);
extern void bcm_axi_mutex_cleanup(void);
extern int bcm_xmod_mutex_init(void);
extern void bcm_xmod_mutex_cleanup(void);
extern int bcm_firmware_download(int unit, int module, void *data, int len);
extern int bcm_port_lane_mask(int unit, int module, int sys_port);
extern int bcm_xmod_pe_init(int unit, int module, xmod_pe_config_t *config);
extern int bcm_xmod_pe_status_get(int unit, int module, xmod_pe_status_t *status);
extern int bcm_xmod_pe_port_stats_get(int unit, int module, int pe_port, xmod_pe_port_stats_t *stats, int reset);
extern int bcm_xmod_pe_fw_ver_get(int unit, int module, xmod_pe_fw_ver_t *ver);
extern int bcm_device_rev_id(int unit, int module, int *rev_id);
extern int bcm_device_die_temp(int unit, int module, int *temp);
extern int bcm_tsc_reg_access(int unit, int module, int sys_port, int write, int reg, int *val);
extern int bcm_mdio_access(int unit, int module, int write, int reg, int *val);
extern int bcm_sbus_access(int unit, int module, uint32 reg, uint32 cmd, int ring, uint32 arg[]);
extern int bcm_axi_reg_access(int unit, int module, int write, uint32 reg, uint32 *val);
extern int bcm_top_reg_access(int unit, int module, int write, uint32 reg, uint32 *val);
extern int bcm_eyescan(int unit, int module, int sys_port);
extern int bcm_dscdump(int unit, int module, int sys_port);
extern int bcm_prbs(int unit, int module, int sys_port, int action, int dir, int poly, int invert);

