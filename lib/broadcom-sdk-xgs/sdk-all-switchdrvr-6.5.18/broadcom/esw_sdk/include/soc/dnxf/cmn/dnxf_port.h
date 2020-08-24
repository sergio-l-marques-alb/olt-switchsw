/*
 * $Id: dnxf_port.h,v 1.5 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF PORT H
 */
 
#ifndef _SOC_DNXF_PORT_H_
#define _SOC_DNXF_PORT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <soc/types.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/portmod/portmod.h>

#define SOC_DNXF_PBMP_PORT_REMOVE               0
#define SOC_DNXF_PBMP_PORT_ADD                  1

typedef enum soc_dnxf_port_update_type_s
{
    soc_dnxf_port_update_type_sfi,
    soc_dnxf_port_update_type_port,
    soc_dnxf_port_update_type_all,
    soc_dnxf_port_update_type_sfi_disabled,
    soc_dnxf_port_update_type_port_disabled,
    soc_dnxf_port_update_type_all_disabled
} soc_dnxf_port_update_type_t;


/**********************************************************/
/*                     Functions                          */
/**********************************************************/

shr_error_e soc_dnxf_port_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback);
shr_error_e soc_dnxf_port_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t* loopback);
#endif /*_SOC_DNXF_PORT_H_*/
