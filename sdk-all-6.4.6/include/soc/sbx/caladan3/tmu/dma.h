/*
 * $Id: dma.h,v 1.5.134.1 Broadcom SDK $
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
 * Purpose: DMA command manager
 */

#ifndef _SBX_CALADN3_TMU_DMA_H_
#define _SBX_CALADN3_TMU_DMA_H_

/*
 *
 * Function:
 *     tmu_dma_tx
 * Purpose:
 *     TMU Transmit DMA
 */
extern int tmu_dma_tx(int unit, 
                      int fifoid, 
                      soc_sbx_caladan3_tmu_cmd_t *cmd,
                      soc_sbx_tmu_cmd_post_flag_e_t flag);

extern int tmu_dma_tx_share_cache_cmd(int unit, int *slave_units, int num_slaves, int fifoid, 
               soc_sbx_caladan3_tmu_cmd_t *cmd); 

extern int tmu_dma_tx_master_flush_cmd(int unit, int *slave_units, int num_slaves, int fifoid);


/*
 *
 * Function:
 *     tmu_dma_rx
 * Purpose:
 *  TMU response manager thread
 *  Waits for a timeout to see if device responds. Waits wise enought to 
 *  make sure that response is consumed till a point where TMU can consume.
 *  DOES NOT MOVE ring pointers. TMU moves the ring pointer for now.  
 */
extern int tmu_dma_rx(int unit, int fifoid, soc_sbx_caladan3_tmu_cmd_t *response) ;

/*
 *
 * Function:
 *     tmu_dma_rx_advance_ring
 * Purpose:
 *     advance dma rx ring pointer
 */
extern int tmu_dma_rx_advance_ring(int unit, int fifoid, int num_bytes);

/*
 *
 * Function:
 *     tmu_dma_rx_copy_data
 * Purpose:
 *     copy received data from dma rx buffer
 */
extern int tmu_dma_rx_copy_data(int unit, 
                                int fifoid, 
                                uint32 *data_buffer,
                                int data_buffer_len, 
                                int resp_data_size_64b); 

/*
 *
 * Function:
 *     tmu_cmd_dma_mgr_destroy
 * Purpose:
 *     destroy TMU command DMA manager
 */
extern int tmu_cmd_dma_mgr_destroy(int unit, int fifoid);

/*
 *
 * Function:
 *     tmu_cmd_dma_mgr_init
 * Purpose:
 *     Initialize command TMU DMA manager
 */
extern int tmu_cmd_dma_mgr_init(int unit, int fifoid);
/*
 *
 * Function:
 *     tmu_cmd_dma_mgr_uninit
 * Purpose:
 *     Cleanup
 */
extern int tmu_cmd_dma_mgr_uninit(int unit, int fifoid);

/*
 *
 * Function:
 *     tmu_resp_dma_mgr_destroy
 * Purpose:
 *     destroy TMU response DMA manager
 */
extern int tmu_resp_dma_mgr_destroy(int unit, int fifoid);

/*
 *
 * Function:
 *     tmu_resp_dma_mgr_init
 * Purpose:
 *     Initialize response TMU DMA manager
 */
extern int tmu_resp_dma_mgr_init(int unit, int fifoid);
/*
 *
 * Function:
 *     tmu_resp_dma_mgr_uninit
 * Purpose:
 *     cleanup
 */
extern int tmu_resp_dma_mgr_uninit(int unit, int fifoid);

extern unsigned int tmu_dma_skip_tx, tmu_dma_skip_rx;
#endif
