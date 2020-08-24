/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom System Log (BSL)
 *
 * BSL functions for EXTERNAL (application) usage.
 */

#ifndef _SHR_BSLEXT_H_
#define _SHR_BSLEXT_H_

#include <shared/bsltypes.h>

extern void
bsl_config_t_init(bsl_config_t *config);

extern int
bsl_init(bsl_config_t *config);

/* PTin added: BSL LOG */
extern int
bsl_vectors_get(bsl_config_t * dst);

#endif /* _SHR_BSLEXT_H_ */

