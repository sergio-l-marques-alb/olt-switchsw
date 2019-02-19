/*
 * $Id: phymod_tsc_iblk.h,v 1.1.2.1 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __PHYMOD_TSC_IBLK_H__
#define __PHYMOD_TSC_IBLK_H__

#include <phymod/phymod_acc.h>
#include <phymod/phymod_reg.h>

/* Special lane values for broadcast and dual-lane multicast */
#define PHYMOD_TSC_IBLK_MCAST01    4
#define PHYMOD_TSC_IBLK_MCAST23    5
#define PHYMOD_TSC_IBLK_BCAST      6

/* Special lane values for broadcast and dual-lane multicast for TSC blackhawk */
#define PHYMOD_TSCBH_IBLK_MCAST01       0x4
#define PHYMOD_TSCBH_IBLK_MCAST23       0x5
#define PHYMOD_TSCBH_IBLK_MCAST45       0x24
#define PHYMOD_TSCBH_IBLK_MCAST67       0x25
#define PHYMOD_TSCBH_IBLK_MCAST0123     0x6
#define PHYMOD_TSCBH_IBLK_MCAST4567     0x26
#define PHYMOD_TSCBH_IBLK_BCAST         0x36


/* Special lane values for broadcast and dual-lane multicast for blackhawk PMD register */
#define PHYMOD_BH_IBLK_MCAST01       0x20
#define PHYMOD_BH_IBLK_MCAST23       0x21
#define PHYMOD_BH_IBLK_MCAST45       0x22
#define PHYMOD_BH_IBLK_MCAST67       0x23
#define PHYMOD_BH_IBLK_MCAST0123     0x40
#define PHYMOD_BH_IBLK_MCAST4567     0x41
#define PHYMOD_BH_IBLK_BCAST         0xff


extern int
phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tsc_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

extern int
phymod_tscbh_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

extern int
phymod_tscbh_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);


#endif /* __PHYMOD_TSC_IBLK_H__ */
