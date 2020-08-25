/*
 * $Id: dnxf_fabric_source_routed_cell.h,v 1.4 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF FABRIC SOURCE ROUTED CELL H
 */
 
#ifndef _SOC_DNXF_FABRIC_SOURCE_ROUTED_CELL_H_
#define _SOC_DNXF_FABRIC_SOURCE_ROUTED_CELL_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

/*************
 * INCLUDES *
 *************/
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/legacy/vsc256_fabric_cell.h>
#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnxc/legacy/dnxc_fabric_source_routed_cell.h>

/*************
 * DEFINES   *
 *************/

#define DNXF_CELL_NOF_BYTES_IN_UINT32                (4)
#define DNXF_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL    (3)


/*************
 * TYPE DEFS *
 *************/


/*************
 * FUNCTIONS *
 *************/

shr_error_e soc_dnxf_cpu2cpu_write(int unit, const dnxc_sr_cell_link_list_t  *sr_link_list, uint32 data_in_size, uint32 *data_in); 
shr_error_e soc_dnxf_route2sr_link_list(int unit, const soc_dnxc_fabric_route_t *route, dnxc_sr_cell_link_list_t *sr_link_list);

shr_error_e soc_dnxf_sr_cell_receive(int unit, dnxc_vsc256_sr_cell_t* cell);
shr_error_e soc_dnxf_sr_cell_payload_receive(int unit, uint32 flags, uint32 data_out_max_size, uint32 *data_out_size, uint32 *data_out);
shr_error_e soc_dnxf_sr_cell_send(int unit, uint32 flags, soc_dnxc_fabric_route_t* route, uint32 data_in_size, uint32 *data_in);


#endif /*_SOC_DNXF_FABRIC_SOURCE_ROUTED_CELL_H_*/
