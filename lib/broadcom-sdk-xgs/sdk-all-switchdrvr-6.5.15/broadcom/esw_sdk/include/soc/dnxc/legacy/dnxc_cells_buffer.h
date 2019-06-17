/*
 * $Id$ 
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXC CELLS BUFFER H
 */
 
#ifndef _SOC_DNXC_CELLS_BUFFER_H_
#define _SOC_DNXC_CELLS_BUFFER_H_

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif /* BCM_DNXF_SUPPORT */
#include <shared/cyclic_buffer.h>
#include <soc/dnxc/legacy/vsc256_fabric_cell.h>

shr_error_e dnxc_cells_buffer_create(int unit, cyclic_buffer_t* cells_buffer, int max_buffered_cells);
shr_error_e dnxc_cells_buffer_destroy(int unit, cyclic_buffer_t* cells_buffer);
shr_error_e dnxc_cells_buffer_add(int unit, cyclic_buffer_t* cells_buffer, const dnxc_vsc256_sr_cell_t* new_cell);
shr_error_e dnxc_cells_buffer_get(int unit, cyclic_buffer_t* cells_buffer, dnxc_vsc256_sr_cell_t* received_cell);
shr_error_e dnxc_cells_buffer_is_empty(int unit, const cyclic_buffer_t* cells_buffer, int* is_empty);

#endif /*_SOC_DNXC_CELLS_BUFFER_H_*/

