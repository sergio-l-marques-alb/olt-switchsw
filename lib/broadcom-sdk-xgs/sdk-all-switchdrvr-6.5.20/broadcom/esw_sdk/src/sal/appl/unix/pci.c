/*
 * $Id: pci.c,v 1.25 Broadcom SDK $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * pci: PCI abstraction common to all platforms.
 */
#include <sal/appl/pci.h>

/*
 * This function will be used only if the application does not provide
 * its own version (see e.g. systems/linux/user/common/socdiag.c).
 *
 * It is required by the PCI utility functions in pci_common.c, but
 * for UNIX/Linux builds, these functions are only used by a
 * deprecated CLI command and not by the core SDK.
 */
uint32 pci_config_getw(pci_dev_t *dev, uint32 addr)
{
    return 0;
}
