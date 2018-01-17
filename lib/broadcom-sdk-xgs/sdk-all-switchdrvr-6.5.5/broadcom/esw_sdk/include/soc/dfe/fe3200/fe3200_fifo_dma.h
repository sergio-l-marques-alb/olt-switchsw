/*
 * $Id: fe3200_fabric_cell.h,v 1.6 Broadcom SDK $
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
 * FE3200 FIFO DMA H
 */


#ifndef _SOC_FE3200_FIFO_DMA_H_
#define _SOC_FE3200_FIFO_DMA_H_

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

#include <soc/error.h>
#include <soc/dfe/cmn/dfe_fifo_dma.h>


/**********************************************************/
/*                  Defines                               */
/**********************************************************/

typedef enum soc_fe3200_fifo_dma_channel_e
{
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_0 = 0,
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_1 = 1,
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_2 = 2, 
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_3 = 3
} soc_fe3200_fifo_dma_channel_t;

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

soc_error_t soc_fe3200_fifo_dma_channel_init(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_deinit(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_clear(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_read_entries(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_fabric_cell_validate(int unit);


#endif /*!_SOC_FE3200_FABRIC_CELL_H_*/

