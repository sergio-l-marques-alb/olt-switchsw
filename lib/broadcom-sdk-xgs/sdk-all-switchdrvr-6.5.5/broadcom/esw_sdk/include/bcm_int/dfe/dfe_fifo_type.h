/*
 * $Id: dfe_fifo_type.h,v 1.3 Broadcom SDK $
 *
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
 * DFE FIFO TYPES H
 */

#ifndef _BCM_DFE_FIFO_TYPE_H_
#define _BCM_DFE_FIFO_TYPE_H_

#include <sal/types.h>

#include <soc/dfe/cmn/dfe_drv.h>

typedef soc_dfe_fifo_type_handle_t _bcm_dfe_fifo_type_handle_t;

int bcm_dfe_fifo_type_clear(int unit, _bcm_dfe_fifo_type_handle_t *h);

int bcm_dfe_fifo_type_is_rx(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_rx);
int bcm_dfe_fifo_type_is_tx(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_tx);
int bcm_dfe_fifo_type_is_fe1(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_fe1);
int bcm_dfe_fifo_type_is_fe3(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_fe3);
int bcm_dfe_fifo_type_is_primary(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_primary);
int bcm_dfe_fifo_type_is_secondary(int unit, _bcm_dfe_fifo_type_handle_t h, int* is_secondary);

int bcm_dfe_fifo_type_set(int unit, _bcm_dfe_fifo_type_handle_t* h, int is_rx, int is_tx, int is_fe1, int is_fe3, int is_primary, int is_secondary);

int bcm_dfe_fifo_type_get_id(int unit, _bcm_dfe_fifo_type_handle_t h, int* id);
int bcm_dfe_fifo_type_set_id(int unit, _bcm_dfe_fifo_type_handle_t* h, int fifo_id);

int bcm_dfe_fifo_type_set_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t* h);
int bcm_dfe_fifo_type_get_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_handle_flag);

int bcm_dfe_fifo_type_is_overlap(int unit, _bcm_dfe_fifo_type_handle_t h1, _bcm_dfe_fifo_type_handle_t  h2, int* is_overlap);
int bcm_dfe_fifo_type_add(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t  h2);
int bcm_dfe_fifo_type_sub(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t  h2);

#endif /*_BCM_DFE_FIFO_TYPE_H_*/
