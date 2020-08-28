/*
 * $Id: ramon_fabric_cell.h,v 1.6 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RAMON FIFO DMA H
 */


#ifndef _SOC_RAMON_FIFO_DMA_H_
#define _SOC_RAMON_FIFO_DMA_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_fifo_dma.h>

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

shr_error_e soc_ramon_fifo_dma_channel_init(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_channel_deinit(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_channel_read_entries(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_fabric_cell_validate(int unit);


#endif /*!_SOC_RAMON_FABRIC_CELL_H_*/

