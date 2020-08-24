/** \file tdm.c
 * $Id$
 *
 * TDM common procedures
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <sal/core/libc.h>
#include <bcm/tdm.h>

/*
 * Function:
 *     bcm_tdm_ingress_editing_context_t_init
 * Description:
 *     Initialize an ingress context structure
 * Parameters:
 *     ingress_context - pointer to ingress context structure
 * Return: none
 */
void
bcm_tdm_ingress_editing_context_t_init(bcm_tdm_ingress_context_t *ingress_context)
{
    sal_memset(ingress_context, 0, sizeof(bcm_tdm_ingress_context_t));
}
