/*
 * $Id: dnxf_port.h,v 1.5 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF FIFO DMA H
 */

#ifndef _SOC_DNXF_FIFO_DMA_H_
#define _SOC_DNXF_FIFO_DMA_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/dnxc_defs.h>

/**********************************************************/
/*                     Constants                          */
/**********************************************************/

/**********************************************************/
/*                     Structures                         */
/**********************************************************/

typedef struct soc_dnxf_fifo_dma_config_s
{
    uint32 max_entries;
    uint32 entry_size;          /* byte size */
    int is_mem;                 /* True if memory connected to FIFO DMA, False if register connected to FIFO DMA */
    soc_mem_t mem;              /* Memory, if relevant */
    soc_reg_t reg;              /* Register if relevant */
} soc_dnxf_fifo_dma_config_t;

typedef struct soc_dnxf_fifo_dma_s
{
    soc_dnxf_fifo_dma_config_t config;  /* fifo dma basic configuration */
    uint32 enable;              /* fifo dma channel enable */
    uint32 current_entry_id;    /* entry id of the next entry to read */
    uint32 nof_unread_entries;  /* number of entries available */
    uint32 read_entries;        /* number of read entries from the last hostmem flush */
    uint8 *buffer;
} soc_dnxf_fifo_dma_t;

/**********************************************************/
/*                     Functions                          */
/**********************************************************/

shr_error_e soc_dnxf_fifo_dma_init(
    int unit);

shr_error_e soc_dnxf_fifo_dma_deinit(
    int unit);

shr_error_e soc_dnxf_fifo_dma_channel_entry_get(
    int unit,
    int channel,
    uint32 max_entry_size,
    uint32 nof_fifo_dma_buf_entries,
    uint8 *entry);

shr_error_e soc_dnxf_fifo_dma_channel_init(
    int unit,
    int channel);

shr_error_e soc_dnxf_fifo_dma_channel_deinit(
    int unit,
    int channel);

#endif /*_SOC_DNXF_FIFO_DMA_H_*/
