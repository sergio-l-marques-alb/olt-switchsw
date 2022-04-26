/*
 * $Id:$
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * PLISIM device API
 */

#ifndef __PLIDEV_H__
#define __PLIDEV_H__

#include <sal/types.h>

extern int
plidev_supported(uint16 pciVenID, uint16 pciDevID, uint8 pciRevID);

#endif /* __PLIDEV_H__ */
