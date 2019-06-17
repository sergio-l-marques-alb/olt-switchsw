/*
 * $Id: fabric.h,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        fabric.h
 * Purpose:     FABRIC internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNX_FABRIC_H_
#define   _BCM_INT_DNX_FABRIC_H_

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

#include <bcm/debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>


/*
 * Defines
 */
#define BCM_FABRIC_MODID_IS_VALID(unit, modid)  \
        ((modid >= 0) && (modid < dnx_data_device.general.nof_faps_get(unit))) /* True if modid is between 0 and the max 
                                                                              limit of FAP's in the device  */
#define BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links)  \
        ((num_of_links >= 0) && (num_of_links <= dnx_data_fabric.links.nof_links_get(unit))) /* True if num_of_links is between 0 and the max 
                                                                                            fabric links in the device  */

/*
 * Set / Get FMQ scheduler mode: 1 - enhanced, 0 - simple
 */
int dnx_fabric_multicast_scheduler_mode_set(int unit, int mode);
int dnx_fabric_multicast_scheduler_mode_get(int unit, int *mode);

shr_error_e dnx_fabric_dtqs_contexts_init(int unit);

#endif /* _BCM_INT_DNX_FABRIC_H_ */
