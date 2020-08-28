/*
 * $Id: dnxf_port.h,v 1.5 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF WARM BOOT H
 */

#ifndef _SOC_DNXF_WARM_BOOT_H
#define _SOC_DNXF_WARM_BOOT_H

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

/**********************************************************/
/*                     Includes                           */
/**********************************************************/

#include <soc/error.h>
#include <shared/shrextend/shrextend_error.h>

/**********************************************************/
/*                     Defines                            */
/**********************************************************/

/**********************************************************/
/*                     Enums                              */
/**********************************************************/


/**********************************************************/
/*                     Functions                          */
/**********************************************************/

shr_error_e soc_dnxf_ha_init(int unit);
shr_error_e soc_dnxf_ha_deinit(int unit);
shr_error_e soc_dnxf_warm_boot_init(int unit);
shr_error_e soc_dnxf_warm_boot_deinit(int unit);
shr_error_e soc_dnxf_warm_boot_engine_init_buffer_struct(int unit, int buffer_id);
shr_error_e soc_dnxf_warm_boot_buffer_id_create(int unit, int buffer_id);

/**********************************************************/
/*                     Constants                          */
/**********************************************************/

#endif /*!_SOC_DNXF_WARM_BOOT_H*/
