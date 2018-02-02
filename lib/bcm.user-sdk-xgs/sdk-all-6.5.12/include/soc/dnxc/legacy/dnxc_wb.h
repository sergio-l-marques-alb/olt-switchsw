/*
 * $Id: dnxf_port.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXC CMICD IPROC
 */

#ifndef _SOC_DNXC_WB_H_
#define _SOC_DNXC_WB_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define _DNXC_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit)            do {} while(0)
#define _DNXC_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit)   do {} while(0)
/* in case it is not DNX WB TEST defining the Macro to do nothing */
#define _DNXC_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)            do {} while(0)

#define _DNXC_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_STOP(unit)       do {} while(0)
#define _DNXC_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit)   do {} while(0)
#define _DNXC_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_STOP(unit)          do {} while(0)
#define _DNXC_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit)      do {} while(0)

#endif /*!_SOC_DNXC_WB_H_*/

