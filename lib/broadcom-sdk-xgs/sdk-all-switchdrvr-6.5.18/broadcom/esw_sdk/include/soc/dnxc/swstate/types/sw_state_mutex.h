/** \file sw_state_mutex_plain.h
 * This module contains the definition for sw_state mutex
 * basic sw state mutex functions 
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_MUTEX_H_
/* { */
#define _DNXC_SW_STATE_MUTEX_H_

/*
 * Include files
 * {
 */
#include <sal/core/sync.h>
/*
 * }
 */

#define SW_STATE_MUTEX_DESC_LENGTH 128

typedef struct
{
    sal_mutex_t mtx;
    char description[SW_STATE_MUTEX_DESC_LENGTH];
} sw_state_mutex_t;

#endif /* _DNXC_SW_STATE_MUTEX_H_ */
