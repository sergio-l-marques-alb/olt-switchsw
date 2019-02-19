/*
 * $Id: drv.h,v 1.187 Broadcom SDK $
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DNX_DRV_H
#define _SOC_DNX_DRV_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <soc/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dnx/dnx_config_imp_defs.h>

typedef struct soc_dnx_config_jer_2_s {
	char dummy;/*Remove it after adding the first struct*/
} soc_dnx_config_jer_2_t;

typedef struct soc_dnx_config_s {
  soc_dnx_config_jer_2_t *jer2;
} soc_dnx_config_t;

typedef struct soc_dnx_control_s {
  soc_dnx_config_t *cfg;
} soc_dnx_control_t;

#define SOC_IS_DNX_TYPE(dev_type) \
    ((dev_type) == BCM88690_DEVICE_ID)
 
#define SOC_DNX_CONTROL(unit) ((soc_dnx_control_t *)SOC_CONTROL(unit)->drv)
#define SOC_DNX_CONFIG(unit)  (SOC_DNX_CONTROL(unit)->cfg) 
#define SOC_DNX_RESET_MODE_REG_ACCESS                               SOC_DCMN_RESET_MODE_REG_ACCESS
    
soc_driver_t* soc_dnx_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id);
int soc_dnx_info_config(int unit);
int soc_dnx_chip_type_set(int unit, uint16 dev_id);
int soc_dnx_info_config_blocks(int unit);
int soc_dnx_init_reset(int unit, int reset_action);
int soc_dnx_init_reg_reset(int unit, int reset_action);
int soc_dnx_device_reset(int unit, int mode, int action);
extern int soc_dnx_attach(int unit);
extern int soc_dnx_detach(int unit);

#endif  /* _SOC_DNX_DRV_H */

