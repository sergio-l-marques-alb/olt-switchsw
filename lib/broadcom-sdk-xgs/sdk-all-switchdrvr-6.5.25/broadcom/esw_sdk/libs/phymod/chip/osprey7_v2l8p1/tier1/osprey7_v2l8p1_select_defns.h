/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2020 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/*********************************************************************************
 *********************************************************************************
 *  File Name  :  osprey7_v2l8p1_select_defns.h
 *  Created On :  29 Sep 2015
 *  Created By :  Brent Roberts
 *  Description:  Select header files for IP-specific definitions
 *  Revision   :
 *
 *********************************************************************************
 ********************************************************************************/

 /** @file
 * Select IP files to include for API
 */

#ifndef OSPREY7_V2L8P1_API_SELECT_DEFNS_H
#define OSPREY7_V2L8P1_API_SELECT_DEFNS_H

#include "osprey7_v2l8p1_ipconfig.h"
#include "osprey7_v2l8p1_field_access.h"

#   include "osprey7_api_uc_common.h"


/****************************************************************************
 * @name Defines for Serdes Unified APIs (UAPI)
 *
 * Unified APIs combine multiple IP variants into single UAPI.
 ****************************************************************************/




#define OSPREY7_V2L8P1_UAPI_INIT           srds_info_t const * const osprey7_v2l8p1_info_ptr = osprey7_v2l8p1_INTERNAL_get_osprey7_v2l8p1_info_ptr_with_check(sa__);
#define OSPREY7_V2L8P1_UAPI_SWITCH         (osprey7_v2l8p1_info_ptr->silicon_version)
#define OSPREY7_V2L8P1_UAPI_CASE0          (0xa0)
#define OSPREY7_V2L8P1_UAPI_CASE1          (0xc0)
#define OSPREY7_V2L8P1_UAPI_CASE2          (0xc1)
#define OSPREY7_V2L8P1_UAPI_TERMINATE


/****************************************************************************
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 ****************************************************************************/
/**@{*/

/**
 * This build includes register access macros for the OSPREY7_V2L8P1 core.
 */
#include "osprey7_v2l8p1_fields.h"

/**@}*/


/****************************************************************************
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 ****************************************************************************/
/**@{*/

/**
 * This build includes macros to access Osprey7 microcode RAM variables.
 */
#include "osprey7_api_uc_vars_rdwr_defns.h"
/**@}*/

#endif
