/*
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
 *  $Id$
 */

/* $Id: furia_ram_regs.h 451 2014-04-17 09:32:21Z binliu $
 */

/** @file furia_ram_regs.h
 * Address of registers used to access the RAM of on-chip processors.
 */

#ifndef FURIA_RAM_REGS_H_
#define FURIA_RAM_REGS_H_

#define FUR_M0ACCESS_ADDR_master_cram_mem_add_Adr     (0x8400)
#define FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr     (0x8401)
#define FUR_M0ACCESS_ADDR_slave_cram_mem_add_Adr      (0x8402)
#define FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr      (0x8403)

#define FUR_M0ACCESS_DATA_master_cram_lsb_wdata_Adr   (0x8410)
#define FUR_M0ACCESS_DATA_master_cram_lsb_rdata_Adr   (0x8411)
#define FUR_M0ACCESS_DATA_master_cram_msb_wdata_Adr   (0x8412)
#define FUR_M0ACCESS_DATA_master_cram_msb_rdata_Adr   (0x8413)
#define FUR_M0ACCESS_DATA_master_dram_wdata_Adr       (0x8414)
#define FUR_M0ACCESS_DATA_master_dram_rdata_Adr       (0x8415)
#define FUR_M0ACCESS_DATA_slave_cram_lsb_wdata_Adr    (0x8416)
#define FUR_M0ACCESS_DATA_slave_cram_lsb_rdata_Adr    (0x8417)
#define FUR_M0ACCESS_DATA_slave_cram_msb_wdata_Adr    (0x8418)
#define FUR_M0ACCESS_DATA_slave_cram_msb_rdata_Adr    (0x8419)
#define FUR_M0ACCESS_DATA_slave_dram_wdata_Adr        (0x841A)
#define FUR_M0ACCESS_DATA_slave_dram_rdata_Adr        (0x841B)

#endif /* FURIA_RAM_REGS_H_ */
