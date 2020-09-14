/* $Id$
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#if !defined(_TEST_CANCUN_H)
#define _TEST_CANCUN_H

#if defined(BCM_ESW_SUPPORT) && defined(CANCUN_SUPPORT)
extern int soc_cancun_pre_misc_init_load(int unit);
extern int soc_cancun_post_misc_init_load(int unit);
#endif

#endif
