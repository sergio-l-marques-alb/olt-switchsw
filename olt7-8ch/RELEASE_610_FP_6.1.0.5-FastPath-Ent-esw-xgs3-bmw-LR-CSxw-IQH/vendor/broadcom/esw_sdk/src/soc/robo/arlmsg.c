/*
 * $Id: arlmsg.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:	arlmsg.c
 * Purpose:	Keep a synchronized ARL shadow table.
 *		Provide a reliable stream of ARL insert/delete messages.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/time.h>


#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>

#include <soc/mcm/robo/driver.h>
#include <soc/arl.h>


/****************************************************************************
 *
 * ARL Message Registration
 *
 ****************************************************************************/

#define ARL_CB_MAX		3

typedef struct arl_cb_entry_s {
    soc_robo_arl_cb_fn   fn;
    void    *fn_data;
} arl_cb_entry_t;

typedef struct arl_data_s {
    arl_cb_entry_t  cb[ARL_CB_MAX];
    int                 cb_count;
} arl_data_t;

STATIC arl_data_t arl_data[SOC_MAX_NUM_SWITCH_DEVICES];

STATIC void soc_arl_thread(void *unit_vp);

static int *delete_list = NULL;

/*
 * Function:
 *  soc_robo_arl_register
 * Purpose:
 *  Register a callback routine to be notified of all inserts,
 *  deletes, and updates to the ARL.
 * Parameters:
 *  unit - RoboSwitch unit number
 *  fn - Callback function to register
 *  fn_data - Extra data passed to callback function
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_MEMORY - Too many callbacks registered
 */
int
soc_robo_arl_register(int unit, soc_robo_arl_cb_fn fn, void *fn_data)
{
    arl_data_t      *ad = &arl_data[unit];

    if (ad->cb_count >= ARL_CB_MAX) {
        return SOC_E_MEMORY;
    }

    ad->cb[ad->cb_count].fn = fn;
    ad->cb[ad->cb_count].fn_data = fn_data;

    ad->cb_count++;

    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_robo_arl_unregister
 * Purpose:
 *  Unregister a callback routine; requires same args as when registered
 * Parameters:
 *  unit - RoboSwitch unit number
 *  fn - Callback function to unregister; NULL to unregister all
 *  fn_data - Extra data passed to callback function;
 *  must match registered value unless fn is NULL
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_NOT_FOUND - Matching registered routine not found
 */
int
soc_robo_arl_unregister(int unit, soc_robo_arl_cb_fn fn, void *fn_data)
{
    arl_data_t      *ad = &arl_data[unit];
    int         i;

    if (fn == NULL) {
        ad->cb_count = 0;
        return SOC_E_NONE;
    }

    for (i = 0; i < ad->cb_count; i++) {
        if ((ad->cb[ad->cb_count].fn == fn &&
             ad->cb[ad->cb_count].fn_data == fn_data)) {

            for (ad->cb_count--; i < ad->cb_count; i++) {
                sal_memcpy(&ad->cb[i], &ad->cb[i + 1],
                    sizeof (arl_cb_entry_t));
            }

            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}
static int arl_sw_entry_count = 0;
static int arl_sw_entry_count_prev = 0;
/*
 * Function:
 *  soc_arl_callback
 * Purpose:
 *  Routine to execute all callbacks on the list.
 * Parameters:
 *  unit - unit number.
 *  entry_del - deleted or updated entry, NULL if none.
 *  entry_add - added or updated entry, NULL if none.
 */
void
soc_robo_arl_callback(int unit, 
                 l2_arl_sw_entry_t *entry_del, l2_arl_sw_entry_t *entry_add)
{
    arl_data_t      *ad = &arl_data[unit];
    int         i;

    if( entry_del == NULL)
        arl_sw_entry_count++;
    if( entry_add == NULL)
        arl_sw_entry_count--;

    for (i = 0; i < ad->cb_count; i++) {
        (*ad->cb[i].fn)(unit, entry_del, entry_add, ad->cb[i].fn_data);
    }
    if (arl_sw_entry_count != arl_sw_entry_count_prev) {
        arl_sw_entry_count_prev = arl_sw_entry_count;
        soc_cm_debug(DK_ARL,"ARL callback entry count %d\n",
            arl_sw_entry_count);
    }
}

/****************************************************************************
 *
 * ARL Message Processing
 *
 ****************************************************************************/

/*
 * Function:
 *  soc_arl_start (internal)
 * Purpose:
 *      Start ARL-related threads
 * Parameters:
 *  unit     - unit number.
 *  interval - time between resynchronization passes
 * Returns:
 *  BCM_E_MEMORY if can't create threads.
 */
STATIC int
soc_arl_start(int unit, sal_usecs_t interval)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int index_count;

    soc_cm_debug(DK_ARL,
		 "soc_arl_start: unit=%d interval=%d\n",
		 unit, interval);

    if (!soc_property_get(unit, spn_REG_WRITE_LOG, FALSE)) {  
        sal_snprintf(soc->arl_name, sizeof(soc->arl_name), "bcmARL.%d", unit);
    }

    soc->arl_interval = interval;

    if (interval == 0) {
        return SOC_E_NONE;
    }

    if(soc->arl_table == NULL) {
        index_count = SOC_MEM_SIZE(unit, L2_ARLm);
        ARL_SW_TABLE_LOCK(soc);
        soc->arl_table = sal_alloc(index_count * sizeof (l2_arl_sw_entry_t),
			"arl_table");
        
        /* clear memory to 0 */
        sal_memset(&soc->arl_table[0], 0, sizeof (l2_arl_sw_entry_t) * index_count);
        ARL_SW_TABLE_UNLOCK(soc);
    }

    if (soc->arl_pid == SAL_THREAD_ERROR) {
        soc->arl_pid = sal_thread_create(soc->arl_name,
					 (SAL_THREAD_STKSZ ),
					 50,
					 soc_arl_thread,
					 INT_TO_PTR(unit));

        if (soc->arl_pid == SAL_THREAD_ERROR) {
            soc_cm_debug(DK_ERR,
        		 "soc_arl_start: Could not start arl thread\n");
            return SOC_E_MEMORY;
        }
    }

    return SOC_E_NONE;

}

/*
 * Function:
 *  soc_arl_stop (internal)
 * Purpose:
 *  Stop ARL-related threads
 * Parameters:
 *  unit - unit number.
 * Returns:
 *  BCM_E_xxx
 */
STATIC int
soc_arl_stop(int unit)
{
    soc_control_t		*soc = SOC_CONTROL(unit);
    int			rv = SOC_E_NONE;
    soc_timeout_t	to;

    soc_cm_debug(DK_ARL, "soc_arl_stop: unit=%d\n", unit);

    soc->arl_interval = 0;	/* Request exit */

    if (soc->arl_pid != SAL_THREAD_ERROR) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(soc->arl_notify);

        /* Give thread a few seconds to wake up and exit */
        soc_timeout_init(&to, 5000000, 0);

        while (soc->arl_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                soc_cm_debug(DK_ERR, "soc_arl_stop: thread will not exit\n");
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    if(soc->arl_table !=NULL) {
        ARL_SW_TABLE_LOCK(soc);
        sal_free(soc->arl_table);
        soc->arl_table = NULL;
        ARL_SW_TABLE_UNLOCK(soc);
    }

    return rv;
}


/*
 * Function:
 *  soc_arl_thread (internal)
 * Purpose:
 *      Thread control for ARL message handling.
 * Parameters:
 *  unit_vp - StrataSwitch unit # (asa  void *).
 * Returns:
 *  Nothing
 * Notes:
 *  Exits when soc->arl_exit flag is set and semaphore is given.
 */
STATIC void 
soc_arl_thread(void *unit_vp)
{
    int				unit = PTR_TO_INT(unit_vp);
    soc_control_t			*soc = SOC_CONTROL(unit);
    int				rv;
    l2_arl_sw_entry_t   *old_tab = NULL;
    int				index_min, index_max, index_count;
    int				index_current, interval;
    l2_arl_sw_entry_t   	sw_arl, null_entry, sw_arl1;
    int                         *exist_list, *current_list;
    uint32          		index,flags,idx;
    uint32                      valid;
    l2_arl_sw_entry_t output;
    int sync;

    index_min = SOC_MEM_BASE(unit, L2_ARLm);
    index_max = SOC_MEM_BASE(unit, L2_ARLm) + SOC_MEM_SIZE(unit, L2_ARLm);
    index_count = SOC_MEM_SIZE(unit, L2_ARLm);

    old_tab = (l2_arl_sw_entry_t *)(soc->arl_table);
    exist_list = sal_alloc(index_count * sizeof (int),
			"arl_exist_list");
    current_list = sal_alloc(index_count * sizeof (int),
			"arl_current_list");
    delete_list = sal_alloc(index_count * sizeof (int),
			"arl_delete_list");

    if ((exist_list == NULL) || (current_list == NULL) || 
        (delete_list == NULL)) {
        soc_cm_debug(DK_ERR, "soc_arl_thread: not enough memory, exiting\n");
        goto cleanup_exit;
    }

    /*
     * Start with initially empty shadow table.
     */
    sal_memset(old_tab, 0, index_count * sizeof (l2_arl_sw_entry_t));
    sal_memset(exist_list, 0, index_count * sizeof (int));
    sal_memset(current_list, 0, index_count * sizeof (int));
    sal_memset(delete_list, 0, index_count * sizeof (int));


    index_current = index_min;

    while ((interval = soc->arl_interval / index_count) != 0) {

        flags = DRV_MEM_OP_SEARCH_DONE;
        rv  = (DRV_SERVICES(unit)->mem_search)
	    (unit, DRV_MEM_ARL, NULL, NULL, NULL, flags);

        if (rv == SOC_E_NONE) {
            /* Sync software database */
            sal_memset(&sw_arl, 0, sizeof(l2_arl_sw_entry_t));
            sal_memset(&null_entry, 0, sizeof(l2_arl_sw_entry_t));
            for (idx = index_min; idx < index_count; idx++) {
                /* Update the aged out and deleted entries */
                if ((exist_list[idx]) && (!current_list[idx])) {
                    ARL_SW_TABLE_LOCK(soc);
                    (DRV_SERVICES(unit)->arl_sync)(unit, (uint32 *)&idx, 
                        (l2_arl_sw_entry_t *)&old_tab[idx],
                        (l2_arl_sw_entry_t *)&sw_arl);
                    ARL_SW_TABLE_UNLOCK(soc);
                }
                if ((delete_list[idx]) && (current_list[idx])) {
                    ARL_SW_TABLE_LOCK(soc);
                    (DRV_SERVICES(unit)->arl_sync)(unit, (uint32 *)&idx, 
                        (l2_arl_sw_entry_t *)&old_tab[idx],
                        (l2_arl_sw_entry_t *)&null_entry);
                    delete_list[idx] = 0;
                    ARL_SW_TABLE_UNLOCK(soc);
                }
            }
            sal_memcpy(exist_list, current_list, index_count * sizeof(int));
            sal_memset(current_list, 0, index_count * sizeof(int));

            flags = DRV_MEM_OP_SEARCH_VALID_START;
            if ((rv = (DRV_SERVICES(unit)->mem_search)
                (unit, DRV_MEM_ARL, NULL, NULL, NULL, flags)) < 0) {
                goto cleanup_exit;
            }
        }
        sal_memset(&sw_arl, 0, sizeof(l2_arl_sw_entry_t));
        sal_memset(&sw_arl1, 0, sizeof(l2_arl_sw_entry_t));
        flags = DRV_MEM_OP_SEARCH_VALID_GET;
        rv  = (DRV_SERVICES(unit)->mem_search)
	    (unit, DRV_MEM_ARL, (uint32 *)&index, (uint32 *)&sw_arl, 
	    (uint32 *)&sw_arl1, flags);
        if (rv != SOC_E_EXISTS){
            sal_sem_take(soc->arl_notify, interval);
            continue;
        } else {
            if (SOC_IS_ROBO5396(unit)||SOC_IS_ROBO5395(unit)
                || SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                /* Search valid mechanism return 2 entries each time */
                index = index * 2;
            }
            (DRV_SERVICES(unit)->mem_field_get)
                (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID,
                (uint32 *)&sw_arl, &valid);
            if (valid) {
                ARL_SW_TABLE_LOCK(soc);
                current_list[index] = TRUE;
                /* If deletion is happen, issue arl callback first */
                if (delete_list[index]) {
                    /* Issue ARL callback first */
                    if((rv = (DRV_SERVICES(unit)->arl_sync)
                         (unit, (uint32 *)&index, 
                         (l2_arl_sw_entry_t *)&old_tab[index],
                           (l2_arl_sw_entry_t *)&null_entry)) < 0){
                           ARL_SW_TABLE_UNLOCK(soc);
                           goto cleanup_exit;
                    }
                    /* Clear delete and exist bit */
                    delete_list[index] = 0;
                    exist_list[index] = 0;
                } else {

                    sync = 1;
                    /* bcm5396 sw workaround
                      * ARL search valid might get entry 
                      * which did not exist in hw table.
                      * Check the entry again before sync
                      * to sw table */

                    if (SOC_IS_ROBO5396(unit)) {
                        flags = DRV_MEM_OP_BY_HASH_BY_MAC | 
                                DRV_MEM_OP_BY_HASH_BY_VLANID;
                        sal_memset(&output, 0, sizeof(l2_arl_sw_entry_t));
                        rv=0;
                        rv  = (DRV_SERVICES(unit)->mem_search)
                            (unit, DRV_MEM_ARL, (uint32 *)&sw_arl, 
                            (uint32 *)&output, NULL, flags);
                        if(rv == SOC_E_NOT_FOUND) {
                            sync = 0;
                        }
                    }
                     /* Sync entry */
                    if (sync) {
                        if((rv = (DRV_SERVICES(unit)->arl_sync)(unit, (uint32 *)&index, 
                             (l2_arl_sw_entry_t *)&old_tab[index],
                               (l2_arl_sw_entry_t *)&sw_arl)) < 0){
                               ARL_SW_TABLE_UNLOCK(soc);
                               goto cleanup_exit;
                        }
                    }
                }
                ARL_SW_TABLE_UNLOCK(soc);
            }
            
            /* Deal the second search entry */
            if (SOC_IS_ROBO5396(unit) ||SOC_IS_ROBO5395(unit) 
                || SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                (DRV_SERVICES(unit)->mem_field_get)
                    (unit, DRV_MEM_ARL, DRV_MEM_FIELD_VALID,
                    (uint32 *)&sw_arl1, &valid);
                if (valid) {
                    ARL_SW_TABLE_LOCK(soc);
                    index = index + 1;
                    current_list[index] = TRUE;
                    /* If deletion is happen, issue arl callback first */
                    if (delete_list[index]) {
                        /* Issue ARL callback first */
                        if((rv = (DRV_SERVICES(unit)->arl_sync)(unit, 
                            (uint32 *)&index, 
                            (l2_arl_sw_entry_t *)&old_tab[index],
                               (l2_arl_sw_entry_t *)&null_entry)) < 0){
                               ARL_SW_TABLE_UNLOCK(soc);
                               goto cleanup_exit;
                        }
                        /* Clear delete and exist bit */
                        delete_list[index] = 0;
                        exist_list[index] = 0;
                    } else {

                        sync = 1;
                        /* bcm5396 sw workaround
                          * ARL search valid might get entry 
                          * which did not exist in hw table.
                          * Check the entry again before sync
                          * to sw table */

                        if (SOC_IS_ROBO5396(unit)) {
                            flags = DRV_MEM_OP_BY_HASH_BY_MAC | 
                                DRV_MEM_OP_BY_HASH_BY_VLANID;
                            sal_memset(&output, 0, sizeof(l2_arl_sw_entry_t));
                            rv=0;
                            rv  = (DRV_SERVICES(unit)->mem_search)
                                (unit, DRV_MEM_ARL, (uint32 *)&sw_arl1, 
                                (uint32 *)&output, NULL, flags);
                            if(rv == SOC_E_NOT_FOUND) {
                                sync = 0;
                            }
                        }
                         /* Sync entry */
                        if (sync) {
                            if((rv = (DRV_SERVICES(unit)->arl_sync)
                                 (unit, (uint32 *)&index, 
                                 (l2_arl_sw_entry_t *)&old_tab[index],
                                   (l2_arl_sw_entry_t *)&sw_arl1)) < 0){
                                   ARL_SW_TABLE_UNLOCK(soc);
                                   goto cleanup_exit;
                            }
                        }
                    }
                    ARL_SW_TABLE_UNLOCK(soc);
                }
            }
        }


        /*
         * Implement the sleep using a semaphore timeout so if the task
         * is requested to exit, it can do so immediately.
         */
        soc_cm_debug(DK_ARL+DK_VERBOSE,
        	     "soc_arl_thread: unit=%d: SLEEP\n", unit);
        sal_sem_take(soc->arl_notify, interval);
    }

cleanup_exit:
    if (exist_list != NULL) {
        sal_free(exist_list);
    }
    if (current_list  != NULL) {
        sal_free(current_list);
    }
    if (delete_list  != NULL) {
        sal_free(delete_list);
    }

    soc_cm_debug(DK_ARL, "soc_arl_thread: exiting\n");

    soc->arl_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 *	soc_robo_arl_mode_set
 * Purpose:  
 *	Configure ARL message processing mode.
 * Parameters:
 *	unit - unit number.
 *	mode - one of ARL_MODE_xxx
 * Returns:
 *	SOC_E_xxx
 * Notes:
 *	This starts and stops the ARL message thread.
 *	The ARL thread runs when the mode is not ARL_MODE_NONE.
 */

int
soc_robo_arl_mode_set(int unit, int mode)
{
    /*
     * Stop existing thread.
     * Set new mode and restart thread if necessary.
     */

    sal_usleep(MILLISECOND_USEC);

    SOC_IF_ERROR_RETURN(soc_arl_stop(unit));

    SOC_CONTROL(unit)->arlMode = mode;

    switch (mode) {
    case ARL_MODE_NONE:
	break;
    case ARL_MODE_ROBO_POLL:
	SOC_IF_ERROR_RETURN(soc_arl_start(unit, ARL_ROBO_POLL_INTERVAL));
	break;
    default:
	assert(0);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_robo_arl_mode_get
 * Purpose:  
 *	Return currently configured ARL mode (ARL_MODE_xxx)
 * Parameters:
 *	unit - unit number.
 *	mode - (OUT) current mode.
 * Returns:
 *	SOC_E_xxx
 */

int
soc_robo_arl_mode_get(int unit, int *mode)
{
    *mode = SOC_CONTROL(unit)->arlMode;

    return SOC_E_NONE;
}


/*
 * Function:
 *	soc_arl_database_dump
 * Purpose:  
 *	Dump an ARL entry from software shadow table
 * Parameters:
 *	unit - unit number.
 *    index - the entry index number
 *	entry - the pointer of the entry buffer
 * Returns:
 *	SOC_E_NONE
 * Notes:
 */

int
soc_arl_database_dump(int unit, uint32 index, l2_arl_sw_entry_t *entry)
{
    soc_control_t			*soc = SOC_CONTROL(unit);
    
    if (&(soc->arl_table[index]) != NULL) {
        sal_memcpy(entry, &(soc->arl_table[index]), sizeof(l2_arl_sw_entry_t));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_arl_database_delete
 * Purpose:  
 *	Set a delete bit to the corresponding ARL shadow table
 * Parameters:
 *	unit - unit number.
 *    index - the entry index number
 * Returns:
 *	SOC_E_NONE
 * Notes:
 */

int
soc_arl_database_delete(int unit, uint32 index)
{ 

    if (delete_list != NULL) {
        delete_list[index] = TRUE;
    }
    return SOC_E_NONE;
}



