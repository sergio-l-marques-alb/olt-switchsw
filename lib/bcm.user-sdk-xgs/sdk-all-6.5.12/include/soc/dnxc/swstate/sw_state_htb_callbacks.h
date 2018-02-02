/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _SW_STATE_HTB_CALLBACKS_H
#define _SW_STATE_HTB_CALLBACKS_H

#include <sal/types.h>

int dnx_algo_lif_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data);

int sw_state_htb_example_entry_print(
    int unit,
    void *key,
    void *data);

#endif /* _SW_STATE_LL_CALLBACKS_H */
