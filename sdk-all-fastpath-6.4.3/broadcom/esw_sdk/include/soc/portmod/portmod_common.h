/*
 *         
 * $Id:$
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
 *     
 *
 */

#ifndef _PORTMOD_COMMON_H_
#define _PORTMOD_COMMON_H_


int portmod_common_phy_prbs_config_set(int unit, int port, int flags, const phymod_prbs_t* config);
int portmod_common_phy_prbs_config_get(int unit, int port, int flags, phymod_prbs_t* config);
int portmod_common_phy_prbs_enable_set(int unit, int port, int flags, int enable);
int portmod_common_phy_prbs_enable_get(int unit, int port, int flags, int* enable);
int portmod_common_phy_prbs_status_get(int unit, int port, int flags, phymod_prbs_status_t* status);
int portmod_common_phy_loopback_set(int unit, int port, portmod_loopback_mode_t loopback_type, int enable);
int portmod_common_phy_loopback_get(int unit, int port, portmod_loopback_mode_t loopback_type, int *enable);
int portmod_common_phy_firmware_mode_set(int unit, int port, phymod_firmware_mode_t fw_mode);
int portmod_common_phy_firmware_mode_get(int unit, int port, phymod_firmware_mode_t *fw_mode);

int portmod_common_phy_sbus_reg_write(soc_mem_t reg_access_mem, void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val);
int portmod_common_phy_sbus_reg_read(soc_mem_t reg_access_mem, void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val);
int portmod_common_mutex_take(void* user_acc);
int portmod_common_mutex_give(void* user_acc);

int portmod_commmon_portmod_to_phymod_loopback_type(int unit, portmod_loopback_mode_t loopback_type, phymod_loopback_mode_t *phymod_lb_type);

/* FIRMWARE LOAD */

typedef enum portmod_ucode_buf_order_e {
    portmod_ucode_buf_order_straight = 0,
    portmod_ucode_buf_order_half,
    portmod_ucode_buf_order_reversed
} portmod_ucode_buf_order_t;

typedef struct portmod_ucode_buf_s {
    void        *ucode_dma_buf;  /* DMA buffer for firmware load */
    uint32      ucode_alloc_size; /* DMA buffer firmware size */
} portmod_ucode_buf_t;

int
portmod_firmware_set(int unit, 
                     int blk_id,
                     const uint8 *array, 
                     uint32 datalen, 
                     portmod_ucode_buf_order_t data_swap, 
                     portmod_ucode_buf_t* buf,
                     soc_mem_t ucmem_data, 
                     soc_reg_t ucmem_ctrl);

/* Simulator */
extern phymod_bus_t sim_bus;

#endif /*_PORTMOD_COMMON_H_*/
