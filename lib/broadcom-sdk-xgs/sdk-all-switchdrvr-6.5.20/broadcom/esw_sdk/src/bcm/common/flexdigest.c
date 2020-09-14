/*! \file flexdigest.c
 *
 * Flex Digest common interface.
 * This file contains the common interfaces for Flex Digest.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <sal/core/libc.h>
#include <bcm/flexdigest.h>

/*!
 * \brief Initialize bcm_flexdigest_qset_t structure.
 *
 * \param [in] qset Flex digest qualifier set.
 */
void
bcm_flexdigest_qset_t_init(bcm_flexdigest_qset_t *qset)
{
    if (qset) {
        sal_memset(qset, 0, sizeof(bcm_flexdigest_qset_t));
    }
}
