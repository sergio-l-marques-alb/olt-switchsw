/*
 * $Id: attach.h,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Attach Application
 *
 * A callback suitable for use in the stack task database update
 * callback is provided.  It automatically attaches and detaches
 * units through the BCM RPC attachments.  A callback mechanism
 * is provided to notify users that a unit has
 * been added or removed.
 */

#ifndef _STKTASK_ATTACH_H
#define _STKTASK_ATTACH_H

#include <appl/cpudb/cpudb.h>

typedef void (*bcm_stack_attach_cb_f)(int unit,
                                      int attach,
                                      cpudb_entry_t *cpuent,
                                      int cpuunit);

extern int bcm_stack_attach_init(void);
extern int bcm_stack_attach_register(bcm_stack_attach_cb_f callback);
extern int bcm_stack_attach_unregister(bcm_stack_attach_cb_f callback);

extern int bcm_stack_attach_update(cpudb_ref_t db_ref);
extern int bcm_stack_attach_running(void);
extern int bcm_stack_attach(cpudb_ref_t db_ref);
extern int bcm_stack_detach(cpudb_ref_t db_ref);

#ifdef LVL7_FIXUP
int bcm_stack_attach_manager_set(int manager);
void bcm_stack_attach_fn(cpudb_ref_t topology_db, int local_cpu_only, 
			 int detach_only);
#endif

#endif /* _STKTASK_ATTACH_H */
