
/*
 * $Id: dnxf_fabric_cell.c,v 1.14 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF FABRIC CELL
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell.h>
#include <soc/dnxf/cmn/dnxf_fabric_source_routed_cell.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <bcm_int/control.h>
#include <soc/dnxc/dnxc_fabric_cell.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/dnxc/dnxc_captured_buffer.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

#define SOC_DNXF_IS_COUNTER_OVERFLOW(counter, overflow_bit) (utilex_bitstream_test_bit(counter, overflow_bit))

/* Static functions */
static void
clear_data_out_strcute(
    soc_dnxc_captured_control_cell_t * data_out)
{
    sal_memset(data_out, 0, sizeof(soc_dnxc_captured_control_cell_t));

    data_out->dest_device = -1;
    data_out->source_device = -1;
    data_out->dest_port = -1;
    data_out->src_queue_num = -1;
    data_out->dest_queue_num = -1;
    data_out->sub_flow_id = -1;
    data_out->flow_id = -1;
    data_out->reachability_bitmap = -1;
    data_out->base_index = -1;
    data_out->source_link_number = -1;
}

int
soc_dnxf_cell_filter_init(
    int unit)
{
    uint32 reg_val32, dch_all_links_mask = 0;
    uint64 reg_val64;
    int dch_block_idx;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    /*
     * Create a mask for all DCH links
     */
    SHR_BITSET_RANGE(&dch_all_links_mask, 0, dnxf_data_device.blocks.nof_links_in_dch_get(unit));

    /*
     * Enable programmable counter for all links
     */
    for (dch_block_idx = 0; dch_block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_block_idx++)
    {
        /*
         * Scan all links
         */
        if (dnxf_data_device.blocks.nof_links_in_dch_get(unit) >= WORDS2BITS(1))
        {
            /*
             * If nof links in DCH is more than or equal to 32 then the register will be 64-bit and needs different access functions
             */
            SHR_IF_ERR_EXIT(soc_reg64_get(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, &reg_val64));
            soc_reg64_field32_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, &reg_val64, PC_MTCH_LINK_ONf, dch_all_links_mask);
            SHR_IF_ERR_EXIT(soc_reg64_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, reg_val64));

        }
        else
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, &reg_val32));
            soc_reg_field_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, &reg_val32, PC_MTCH_LINK_ONf, dch_all_links_mask);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCH_FILTER_MATCH_INPUT_LINKr, dch_block_idx, 0, reg_val32));
        }

        /*
         * Enable the programmable counter feature
         */
        SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_block_idx, &reg_val64));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val64, AUTO_DOC_NAME_5f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_block_idx, reg_val64));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_cell_filter_set(
    int unit,
    uint32 flags,
    uint32 array_size,
    soc_dnxc_filter_type_t * filter_type_arr,
    uint32 *filter_type_val)
{
    int i;
    uint32 table_size, start_bit, bit_length, mask;
    uint64 reg_val64;
    soc_reg_above_64_val_t filter_reg, mask_reg;
    soc_dnxf_cell_filter_mode_t filter_mode = soc_dnxf_cell_filter_copy_mode;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(filter_type_arr, _SHR_E_PARAM, "filter_type_arr");
    SHR_NULL_CHECK(filter_type_val, _SHR_E_PARAM, "filter_type_val");

    DNXF_UNIT_LOCK_TAKE(unit);

    if ((flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD) && (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP))
    {
        filter_mode = soc_dnxf_cell_filter_filter_mode;
    }
    else if (flags & SOC_CELL_FILTER_FLAG_DONT_FORWARD)
    {
        filter_mode = soc_dnxf_cell_filter_capture_mode;
    }
    else if (flags & SOC_CELL_FILTER_FLAG_DONT_TRAP)
    {
        filter_mode = soc_dnxf_cell_filter_normal_mode;
    }
    /*
     * Set filter mode
     */
    for (i = 0; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg_val64, DCH_MTCH_ACTf, filter_mode);
        SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, i, reg_val64));
    }

    SOC_REG_ABOVE_64_CLEAR(filter_reg);
    /*
     * mask of "1"s mean to ignore when using the filter
     */
    SOC_REG_ABOVE_64_ALLONES(mask_reg);

    for (i = 0; i < array_size; i++)
    {
        /*
         * Check valid inputs
         */
        switch (filter_type_arr[i])
        {
            case soc_dnxc_filter_type_source_id:
                mask = ~(dnxf_data_fabric.cell.cell_match_filter_reg_mask_get(unit, filter_type_arr[i])->mask);
                if ((filter_type_val[i] & mask) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "source id %d out of range", filter_type_val[i]);
                }
                break;

            case soc_dnxc_filter_type_multicast_id:
            case soc_dnxc_filter_type_dest_id:
                SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &table_size)));
                if (filter_type_val[i] >= table_size)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "dest id out of range");
                }
                break;

            case soc_dnxc_filter_type_priority:
                mask = ~(dnxf_data_fabric.cell.cell_match_filter_reg_mask_get(unit, filter_type_arr[i])->mask);
                if ((filter_type_val[i] & mask) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "the priority type is out of range");
                }
                break;

            case soc_dnxc_filter_type_traffic_cast:
                mask = ~(dnxf_data_fabric.cell.cell_match_filter_reg_mask_get(unit, filter_type_arr[i])->mask);
                if ((filter_type_val[i] & mask) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "the traffic cast is out of range");
                }
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Filter Type");
        }

        start_bit = dnxf_data_fabric.cell.cell_match_filter_reg_mask_get(unit, filter_type_arr[i])->start_bit;
        bit_length = dnxf_data_fabric.cell.cell_match_filter_reg_mask_get(unit, filter_type_arr[i])->bit_length;
        /*
         * Create bit masks per filter type to write into registers
         */
        SOC_REG_ABOVE_64_RANGE_COPY(filter_reg, start_bit, &(filter_type_val[i]), 0, bit_length);
        SHR_BITCLR_RANGE(mask_reg, start_bit, bit_length);
    }

    for (i = 0; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, filter_reg));
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, mask_reg));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_cell_filter_count_get(
    int unit,
    int *count)
{
    int i, counter;
    uint32 temp_count[1] = { 0 };
    uint32 overflow_bit = 0;
    uint32 reg32_val;
    soc_reg_t dch_prg_cell_cnt_reg[] = {
        DCH_PRG_CELL_CNT_0r, DCH_PRG_CELL_CNT_1r, DCH_PRG_CELL_CNT_2r
    };
    soc_field_t prg_cell_cnt_field[] = {
        PRG_CELL_CNT_0f, PRG_CELL_CNT_1f, PRG_CELL_CNT_2f
    };

    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(count, _SHR_E_PARAM, "count");

    DNXF_UNIT_LOCK_TAKE(unit);

    *count = 0;

    /*
     * The number of counter registers for a device is equal to the number of FMACs per DCH.
     */
    for (counter = 0; counter < dnxf_data_device.blocks.nof_dch_link_groups_get(unit); counter++)
    {
        for (i = 0; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, dch_prg_cell_cnt_reg[counter], i, 0, &reg32_val));
            temp_count[0] =
                soc_reg_field_get(unit, dch_prg_cell_cnt_reg[counter], reg32_val, prg_cell_cnt_field[counter]);
            overflow_bit = soc_reg_field_length(unit, dch_prg_cell_cnt_reg[counter], prg_cell_cnt_field[counter]) - 1;

            /*
             * Add to count if counter didn't overflow
             */
            if (SOC_DNXF_IS_COUNTER_OVERFLOW(temp_count, overflow_bit))
            {
                *count = -1;
                break;
            }
            else
            {
                *count += temp_count[0];
            }
        }

        if (*count == -1)
        {
            break;
        }
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

int
soc_dnxf_cell_filter_clear(
    int unit)
{
    int i;
    soc_reg_above_64_val_t reg_val;

    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val);
    for (i = 0; i < dnxf_data_device.blocks.nof_instances_dch_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, i, reg_val));
        SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILT_MASKr(unit, i, reg_val));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
soc_dnxf_cell_filter_receive(
    int unit,
    dnxc_captured_cell_t * data_out)
{
    int rv, is_empty;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");
    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * Check if cell exist in data base 
     */
    rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
    SHR_IF_ERR_EXIT(rv);

    /*
     * Load cells to data base 
     */
    /*
     * Stop loading if captured cell loaded or buffer is empty
     */
    while (is_empty)
    {
        rv = soc_dnxf_fabric_cell_load(unit);
        if (rv == _SHR_E_EMPTY)
        {
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        rv = dnxc_captured_buffer_is_empty(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, &is_empty);
        SHR_IF_ERR_EXIT(rv);
    }
    if (is_empty)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

    /*
     * Pull cell from data base
     */
    rv = dnxc_captured_buffer_get(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer, data_out);
    SHR_IF_ERR_EXIT(rv);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_control_cell_filter_set
 * Purpose:
 *      Set filter in order to capture a control cell (credit, flow status, reachability)
 * Parameters:
 *      unit                            - (IN)  Unit number
 *      cell_type                       - (IN)  Control cell type (credit, flow status or reachability cell
 *      array_size                      - (IN)  Number of filters
 *      control_cell_filter_type_arr    - (IN)  The filters array
 *      filter_type_val                 - (OUT) The filter that we should us to capture the control cell
 *  
 * Returns:
 *      _SHR_E_xxx
 */
int
soc_dnxf_control_cell_filter_set(
    int unit,
    uint32 flags,
    soc_dnxc_control_cell_types_t cell_type,
    uint32 array_size,
    soc_dnxc_control_cell_filter_type_t * control_cell_filter_type_arr,
    uint32 *filter_type_val)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    if (array_size != 0)
    {
        SHR_NULL_CHECK(control_cell_filter_type_arr, _SHR_E_PARAM, "control_cell_filter_type_arr");
        SHR_NULL_CHECK(filter_type_val, _SHR_E_PARAM, "filter_type_val");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_control_cell_filter_set,
                               (unit, flags, cell_type, array_size, control_cell_filter_type_arr, filter_type_val));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_control_cell_filter_clear
 * Purpose:
 *      Clear the control cell of filter and mask
 * Parameters:
 *      unit                            - (IN)  Unit number
 *  
 * Returns:
 *      _SHR_E_xxx
 */
int
soc_dnxf_control_cell_filter_clear(
    int unit)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL_NO_ARGS(unit, mbcm_dnxf_control_cell_filter_clear);
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_control_cell_filter_receive
 * Purpose:
 *      Recieve the content of the capture control cell. The capture cell can be one of the following:
 *      1. Credit
 *      2. FSM (Flow Status Message)
 *      3. Reachability cell
 *      4. FE2 filtered cell (Required fo FE2 normal operation.
 *  
 * Parameters:
 *      unit                           - (IN)  Unit number
 *      data_out                       - (OUT) The parsing of the control cell that match the filter
 *  
 * Comments: 
 *      # The data_out strcture includes valid bit that indicate if we were able to capture a cell
 *      For each control cell different fields are relevant:
 *      1. Credit - dest_queue_num, sub_flow_id, flow_id, source_device, dest_device
 *      2. FSM - dest_port, src_queue_num, flow_id, source_device, dest_device
 *      3. Reachability cell - reachability_bitmap, base_index, source_link_number, source_device
 *      4. FE2 filtered cell - cell type only.
 * Returns:
 *      _SHR_E_xxx
 */

int
soc_dnxf_control_cell_filter_receive(
    int unit,
    soc_dnxc_captured_control_cell_t * data_out)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    /*
     * Clear the strcure, if the output is "-1" this field is not avaialble on this specific control cell type 
     */
    clear_data_out_strcute(data_out);

    DNXF_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_control_cell_filter_receive, (unit, data_out));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

/*
 * Function:
 *      soc_dnxf_fabric_cell_parse
 * Purpose:
 *  Parse the received bit stream entry
 *  to a variable of type soc_dnxc_fabric_cell_info_t.
 *  The structure contains the two possible cell type structures
 *  source routed or regular captured cell (destination routed)
 *  and a control parameter to indicate which type is the captured cell
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      entry           - (IN)  Received data
 *      cell_info       - (OUT) The parse cell
 * Returns:
 *      _SHR_E_XXX
 */
static shr_error_e
soc_dnxf_fabric_cell_parse(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry,
    soc_dnxc_fabric_cell_info_t * cell_info)
{
    soc_dnxc_fabric_cell_parse_table_t *parse_table = NULL;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Alloc memory for parse_table. Plus 1 for soc_dnxc_fabric_cell_field_invalid.
     */
    SHR_ALLOC_SET_ZERO(parse_table, (soc_dnxc_fabric_cell_field_nof + 1) * sizeof(soc_dnxc_fabric_cell_parse_table_t),
                       "parse_table", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_cell_parse_table_get,
                                          (unit, entry, &cell_info->cell_type, parse_table)));

    SHR_IF_ERR_EXIT(soc_dnxc_fabric_cell_parser(unit, entry, parse_table, cell_info));

exit:
    SHR_FREE(parse_table);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_fabric_cell_load
 * Purpose:
 *      Loading captured cells (Source routed or destination routed) to cpu buffer.
 *      Load the cells using FIFO DMA if supported.
 *      The existed cells pushed to data base
 *      
 * Parameters:
 *      unit                            - (IN)  Unit number
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_dnxf_fabric_cell_load(
    int unit)
{
    int rv;
    soc_dnxc_fabric_cell_entry_t entry;
    soc_dnxc_fabric_cell_info_t cell_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cell_info, 0x00, sizeof(soc_dnxc_fabric_cell_info_t));

    /*
     * Get cell entry either by checking all FIFO DMA channels
     * or by checking directly the register
     * (common case is that the DMA channels are mapped to the same register
     *  that we read even without FIFO DMA enabled )
     */
    rv = soc_dnxf_fabric_cell_entry_get(unit, entry);
    if (rv == _SHR_E_EMPTY)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Parse cell
     * Translate the the received bit stream entry
     * to a variable of type soc_dnxc_fabric_cell_info_t
     */
    SHR_IF_ERR_EXIT(soc_dnxf_fabric_cell_parse(unit, entry, &cell_info));

    /*
     * Add to the dedicated SW data base per cell type
     * because we support receiving the two types at the same time
     * dnxc_cells_buffer_add and dnxc_captured_buffer_add are practically the same
     * but handle two different types of cell structures.
     */
    if (cell_info.cell_type == soc_dnxc_fabric_cell_type_sr_cell)
    {
        SHR_IF_ERR_EXIT(dnxc_cells_buffer_add(unit, &SOC_DNXF_CONTROL(unit)->sr_cells_buffer, &cell_info.cell.sr_cell));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnxc_captured_buffer_add(unit, &SOC_DNXF_CONTROL(unit)->captured_cells_buffer,
                                                 &cell_info.cell.captured_cell));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_fabric_cell_entry_get
 * Purpose:
 *      Get cell entry either by checking all FIFO DMA channels
 *      or by checking directly the register
 *      (common case is that the DMA channels are mapped to the same register
 *      that we read even without FIFO DMA enabled )
 *
 * Parameters:
 *      unit                            - (IN)  Unit number
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_dnxf_fabric_cell_entry_get(
    int unit,
    soc_dnxc_fabric_cell_entry_t entry)
{
    int rv;
    int channel;
    int found_cell = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get cell using FIFO DMA support
     */
    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        for (channel = 0; channel < dnxf_data_fabric.cell.fifo_dma_nof_channels_get(unit); channel++)
        {
            rv = soc_dnxf_fifo_dma_channel_entry_get(unit, channel,
                                                     SOC_DNXC_FABRIC_CELL_ENTRY_MAX_SIZE_UINT32 * sizeof(uint32),
                                                     dnxf_data_fabric.
                                                     cell.fifo_dma_fabric_cell_nof_entries_per_cell_get(unit),
                                                     (uint8 *) entry);
            if (rv == _SHR_E_EMPTY)
            {
                /*
                 * check next channel
                 */
                continue;
            }
            SHR_IF_ERR_EXIT(rv);

            found_cell = 1;
            break;
        }
    }
    else
    {
        /*
         * Get cell directly
         */
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_cell_get, (unit, entry));
        if (rv == _SHR_E_EMPTY)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(rv);
        found_cell = 1;
    }
    if (!found_cell)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
