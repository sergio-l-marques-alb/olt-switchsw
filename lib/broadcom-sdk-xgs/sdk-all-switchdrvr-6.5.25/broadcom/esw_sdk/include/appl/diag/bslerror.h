/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom System Log Error
 */

#ifndef _DIAG_BSLERROR_H
#define _DIAG_BSLERROR_H

int
bslerror_init(void);

int
bslerror_enable_set(int enable);

int
bslerror_enable_restore(void);
#endif /* !_DIAG_BSLERROR_H */
