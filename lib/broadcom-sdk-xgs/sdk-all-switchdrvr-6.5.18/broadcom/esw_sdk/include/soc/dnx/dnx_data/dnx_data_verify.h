/** \file dnx_data_verify.h
 * 
 * MODULE DATA VERIFY -
 * Per device data verification
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 */
/* *INDENT-OFF* */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNX_DATA_VERIFY_H_
/*{*/
#define _DNX_DATA_VERIFY_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e
dnx_data_property_unsupported_verify(int unit);

/*}*/
#endif /*_DNX_DATA_VERIFY_H_*/
/* *INDENT-ON* */
