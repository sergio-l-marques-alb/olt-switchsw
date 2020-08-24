/** \file sw_state_sem_plain.h
 * This module contains the definition for sw_state semaphore
 * basic sw state semaphore functions 
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_SEM_H_
/* { */
#define _DNXC_SW_STATE_SEM_H_

/*
 * Include files
 * {
 */
#include <sal/core/sync.h>
/*
 * }
 */

#define SW_STATE_SEM_DESC_LENGTH 128

typedef struct
{
    sal_sem_t sem;
    int is_binary;
    int initial_count;
    char description[SW_STATE_SEM_DESC_LENGTH];
} sw_state_sem_t;

#endif /* _DNXC_SW_STATE_SEM_H_ */
