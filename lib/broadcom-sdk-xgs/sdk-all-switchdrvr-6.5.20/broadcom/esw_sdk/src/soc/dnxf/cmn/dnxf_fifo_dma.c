/*
 * $Id: dnxf_port.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC FIFO DMA
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/fifodma.h>

#ifdef BCM_DNXF_SUPPORT

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_fifo_dma.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

/*FIFO DMA data base*/
/*should not restored by warm boot*/
soc_dnxf_fifo_dma_t *_soc_dnxf_fifo_dma[SOC_MAX_NUM_DEVICES] = {0};

shr_error_e
soc_dnxf_fifo_dma_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*Initialize cmic fifodma*/
    SHR_IF_ERR_EXIT(soc_fifodma_init(unit));

    /* Allocate FIFO DMA data base */
    SHR_ALLOC_SET_ZERO(_soc_dnxf_fifo_dma[unit], sizeof(soc_dnxf_fifo_dma_t) * dnxf_data_fabric.cell.fifo_dma_nof_channels_get(unit), "_soc_dnxf_fifo_dma[unit]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_deinit(int unit)
{
    int channel;
    SHR_FUNC_INIT_VARS(unit);
    
    if (_soc_dnxf_fifo_dma[unit] != NULL)
    {
        for (channel = 0; channel  < dnxf_data_fabric.cell.fifo_dma_nof_channels_get(unit); channel++)
        {
            if (_soc_dnxf_fifo_dma[unit][channel].enable)
            {
                SHR_IF_ERR_CONT(soc_dnxf_fifo_dma_channel_deinit(unit, channel));
            }
        }

        /* Deinit FIFO DMA data base */
        SHR_FREE(_soc_dnxf_fifo_dma[unit]);
    }

    /*Deinitialize cmic fifo dma*/
    SHR_IF_ERR_CONT(soc_fifodma_deinit(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_channel_init(int unit, int channel)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_init, (unit, channel, &_soc_dnxf_fifo_dma[unit][channel]));
    SHR_IF_ERR_EXIT(rv);

    _soc_dnxf_fifo_dma[unit][channel].enable = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_channel_deinit(int unit, int channel)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_deinit, (unit, channel, &_soc_dnxf_fifo_dma[unit][channel]));
    SHR_IF_ERR_CONT(rv);

    _soc_dnxf_fifo_dma[unit][channel].enable = 0;

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_fifo_dma_channel_entry_get
 * Purpose:
 *      FIFO DMA pull <nof_fifo_dma_buf_entries> entries
 * Parameters:
 *      unit                            - (IN)  Unit number.
 *      channel                         - (IN)  fifo dma channel
 *      max_entry_size                  - (IN)  size of data_in
 *      nof_fifo_dma_buf_entries        - (IN)  num of entries to copy from _soc_dnxf_fifo_dma[unit][channel].buffer to entry
 *      entry                           - (OUT) data out
 * Returns:
 *      _SHR_E_XXX
 */
shr_error_e
soc_dnxf_fifo_dma_channel_entry_get(int unit, int channel, uint32  max_entry_size, uint32 nof_fifo_dma_buf_entries, uint8 *entry)
{
    int rv;
    uint32 buffer_offset;
    SHR_FUNC_INIT_VARS(unit);

    /*If no available entries in the hostmem - try to load from the fifo*/
    if (_soc_dnxf_fifo_dma[unit][channel].nof_unread_entries < nof_fifo_dma_buf_entries)
    {
        /*nof entries to read from fifo_dma*/
        _soc_dnxf_fifo_dma[unit][channel].read_entries = nof_fifo_dma_buf_entries - _soc_dnxf_fifo_dma[unit][channel].nof_unread_entries;
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_read_entries, (unit, channel, &_soc_dnxf_fifo_dma[unit][channel]));
        SHR_IF_ERR_EXIT(rv);
        /*wait for entry load*/
        sal_usleep(100);
    }

    /*copy entry and update data base*/
    if (_soc_dnxf_fifo_dma[unit][channel].nof_unread_entries >= nof_fifo_dma_buf_entries)
    {
        if (_soc_dnxf_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries > max_entry_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Required bigger entry\n");
        }

        buffer_offset = _soc_dnxf_fifo_dma[unit][channel].current_entry_id * _soc_dnxf_fifo_dma[unit][channel].config.entry_size;
        sal_memcpy(entry, &_soc_dnxf_fifo_dma[unit][channel].buffer[buffer_offset], sizeof(uint8) * _soc_dnxf_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries);
        _soc_dnxf_fifo_dma[unit][channel].current_entry_id = (_soc_dnxf_fifo_dma[unit][channel].current_entry_id + nof_fifo_dma_buf_entries) % _soc_dnxf_fifo_dma[unit][channel].config.max_entries; /*Cyclic buffer*/

        _soc_dnxf_fifo_dma[unit][channel].nof_unread_entries -= nof_fifo_dma_buf_entries;
        _soc_dnxf_fifo_dma[unit][channel].read_entries = 0;

    } else {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#endif /* BCM_DNXF_SUPPORT */

#undef BSL_LOG_MODULE

