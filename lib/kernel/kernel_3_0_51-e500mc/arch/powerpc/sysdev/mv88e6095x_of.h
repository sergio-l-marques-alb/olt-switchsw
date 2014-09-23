
#ifndef __MARVELL_88E6095X_DRIVER__
#define __MARVELL_88E6095X_DRIVER__

#include <linux/phy.h>

#define REG_PORT(p)		(0x10 + (p))
#define REG_GLOBAL		0x1b
#define REG_GLOBAL2		0x1c

extern struct phy_device * mv88e6095_detect(struct mii_bus *bus, int sw_addr);

#endif  //__MARVELL_88E6095X_DRIVER__
