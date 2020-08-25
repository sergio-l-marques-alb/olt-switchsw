/*
 * $Id: gport_mgmt.h,v 1.71 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        gport_mgmt.h
 * Purpose:     GPORT Management internal definitions to the BCM library.
 */

#ifndef  INCLUDE_DNX_GPORT_MGMT_H
#define  INCLUDE_DNX_GPORT_MGMT_H

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

#include <bcm/types.h>

#include <soc/dnx/legacy/TMC/tmc_api_general.h>

#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_hash_tbl.h>


/*
 * Function:
 *    _bcm_dnx_gport_to_tm_dest_info
 * Description:
 *    convert gport from TM dest information
 * Parameters:
 *  unit -           [IN] DNX device unit number (driver internal).
 *  gport -          [OUT] general port
 *  soc_dnx_dest_info - [OUT] Soc_dnx destination info
 * Returns:
 *    _SHR_E_XXX
 */
int
_bcm_dnx_gport_from_tm_dest_info(int unit, bcm_gport_t *gport, SOC_DNX_DEST_INFO  *soc_dnx_dest_info);



#endif /* INCLUDE_DNX_GPORT_MGMT_H */

