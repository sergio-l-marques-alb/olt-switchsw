/*
 * Bsafe uHSM regs definitions
 *
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * $Id: bsafe_regs.h,v 1.1 2011/04/18 17:10:59 mruas Exp $
 */

#ifndef __BSAFE_REGS_H_
#define __BSAFE_REGS_H_

#define BSAFE_BASE_OFFSET       0x200
#define BSAFE_MAX_REG_SIZE      4
#define BSAFE_MAX_REGS          8

typedef enum bsafe_glb_register {
    GLB_TIMER                 = 0x200,
    GLB_PRESCALE              = 0x204,
    GLB_UHSM_CFG              = 0x208,
    GLB_PROD_CFG              = 0x20c,
    GLB_DEV_STATUS            = 0x210,
    GLB_CMD_CTL               = 0x214,
    GLB_CMD_DATA_IN           = 0x218,
    GLB_CMD_DATA_OUT          = 0x21c,
    GLB_INT_CTRL              = 0x220
} bsafe_glb_register_t;

typedef enum bsafe_glb_cmd_cfg {
    GLB_CMD_CTL_CMD_IRDY      = 1<<0,
    GLB_CMD_CTL_CMD_ISYNC     = 1<<1,
    GLB_CMD_CTL_CMD_ORDY      = 1<<2,
    GLB_CMD_CTL_CMD_OSYNC     = 1<<3,
    GLB_CMD_CTL_CMD_SOFTRESET = 1<<7
    
} bsafe_glb_cmd_cfg_t;

typedef enum bsafe_glb_dev_status_t {

    GLB_DEV_STATUS_INIT_DONE  = 1<<4
    
} bsafe_glb_dev_status_t;

#endif /* __BSAFE_REGS_H_ */
