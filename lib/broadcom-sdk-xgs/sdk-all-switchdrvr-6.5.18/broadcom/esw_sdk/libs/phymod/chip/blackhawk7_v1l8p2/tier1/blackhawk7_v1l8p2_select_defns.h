/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/*********************************************************************************
 *********************************************************************************
 *  File Name  :  blackhawk7_v1l8p2_select_defns.h
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

#ifndef BLACKHAWK7_V1L8P2_API_SELECT_DEFNS_H
#define BLACKHAWK7_V1L8P2_API_SELECT_DEFNS_H

#include "blackhawk7_v1l8p2_ipconfig.h"
#include "blackhawk7_v1l8p2_field_access.h"

#   include "blackhawk7_api_uc_common.h"

/****************************************************************************
 * @name Register Access Macro Inclusions
 *
 * All cores provide access to hardware control/status registers.
 ****************************************************************************/
/**@{*/

/**
 * This build includes register access macros for the BLACKHAWK7_V1L8P2 core.
 */
#include "blackhawk7_v1l8p2_fields.h"

/**@}*/


/****************************************************************************
 * @name RAM Access Macro Inclusions
 *
 * Some cores also provide access to firmware control/status RAM variables.
 ****************************************************************************/
/**@{*/

/**
 * This build includes macros to access Blackhawk microcode RAM variables.
 */
#include "blackhawk7_api_uc_vars_rdwr_defns.h"
/**@}*/

#endif
