/*
 * $Id: schanmsg_internal.h,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * CMICx MIIM (MDIO) functions
 */

#ifndef _SOC_CMICX_MIIM_H
#define _SOC_CMICX_MIIM_H

#include <sal/types.h>


/*******************************************
* @function soc_cmicx_miim_init
* purpose Initialization required for MIIM module
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
* @comments This function give MDIO access to IPROC
********************************************/
extern int soc_cmicx_miim_init(int unit);

/*******************************************
* @function soc_cmicx_miim_divider_set
* purpose set MDIO frequency related properties
*
* @param unit [in] unit #
* @param unit [in] int_divider - The clock divider configuration register for Internal MDIO. use -1 to keep current configuration.
* @param unit [in] ext_divider - The clock divider configuration register for External MDIO. use -1 to keep current configuration.
* @param unit [in] out_delay - MDIO Output Delay. use -1 to keep current configuration.
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
* @comments This function configure all MDIO rings
********************************************/
extern int soc_cmicx_miim_divider_set(int unit, int int_divider, int ext_divider, int out_delay);

/*******************************************
* @function soc_cmicx_miim_cl45_operation
* purpose perform CL45 read or write operation
*
* @param unit [in] unit #
* @param unit [in] is_write - whether to perform read or write operation
* @param unit [in] phy_id - phy to access. phy_id encoding:
*                            bit7, 1: internal MDIO bus, 0: external MDIO bus
*                            bit9,8,6,5, mdio bus number
*                            bit4-0,   mdio addresses
* @param unit [in] phy_reg_addr - CL45 register address. Encoded according to SOC_PHY_CLAUSE45_ADDR macro.
* @param unit [inout] phy_data - data to wrote in case is_write is true, data read from register otehrwise. 
* 
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
* @comments
*     This function takes MIIM lock
*     Waiting for MIIM done is done by polling (interrupt mode isn't supported)
********************************************/
extern int soc_cmicx_miim_cl45_operation(int unit, int is_write, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 *phy_data);



#endif  /* !_SOC_CMICX_MIIM_H */

