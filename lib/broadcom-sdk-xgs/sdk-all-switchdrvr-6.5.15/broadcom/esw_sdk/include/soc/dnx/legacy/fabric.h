/*
* $Id: fabric.h,v 1.5 Broadcom SDK $
* $Copyright: (c) 2018 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
* This file contains structures and functions declarations for 
* In-band cell configuration and Source Routed Cell.
* 
*/
#ifndef _SOC_DNX_FABRIC_H
#define _SOC_DNX_FABRIC_H

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*
 * Section 12.8: In-band cell configuration 
 * The following methods are SOC methods which are only relevant for DNX_SAND architecture devices. 
 * The External CPU that connects to the FAP can generate a CPU-to-CPU, source-routed data cell and send it via a specific route to the requested device on the route. 
 * This cell travels in the system and is captured by the destination device. The inband cell target is to configure the fabric device. 
 */

#include <soc/dnx/legacy/TMC/tmc_api_cell.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>
#include <soc/error.h>

#define SOC_DNX_FIRST_FABRIC_PHY_PORT(unit) 0

#define SOC_DNX_PROPERTY_UNAVAIL                   (-1)

typedef enum soc_dnx_fabric_force {
    socDnxFabricForceFabric = 0,     /* Force Fabric */
    socDnxFabricForceLocal0 = 1,     /* Force Local Core 0 */
    socDnxFabricForceLocal1 = 2,     /* Force Local Core 1 */
    socDnxFabricForceLocal = 3,     /* Force Local */
    socDnxFabricForceRestore = 4,     /* Restore default */
    socDnxFabricForceNotSupported = 5     /* Not supported option */
} soc_dnx_fabric_force_t;

#endif  /*_SOC_DNX_FABRIC_H*/
