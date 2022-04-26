/*! \file bcm56980_a0_pdma_attach.c
 *
 * Initialize PDMA driver resources.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef INCLUDE_PKTIO

#include <bcmcnet/bcmcnet_core.h>
#include <bcmcnet/bcmcnet_dev.h>
#include <bcmcnet/bcmcnet_cmicx.h>

int
bcm56980_a0_cnet_pdma_attach(struct pdma_dev *dev)
{
    return bcmcnet_cmicx_pdma_driver_attach(dev);
}

int
bcm56980_a0_cnet_pdma_detach(struct pdma_dev *dev)
{
    return bcmcnet_cmicx_pdma_driver_detach(dev);
}

#endif /* INCLUDE_PKTIO */
