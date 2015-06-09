/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_api.h
*
* @purpose    Generic Unit Synchronization API
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#ifndef L7_USL_API_H
#define L7_USL_API_H

#include "l7_common.h"
#include "bcmx/lplist.h"
#include "appl/cpudb/cpudb.h"



/*********************************************************************
* @purpose  Resume MAC table syncronization.
*
* @param    void 
*
* @returns  void
*
* @notes    Re-register the callbacks to BCM
*
* @end
*********************************************************************/
extern void usl_mac_table_sync_resume (void);

/*********************************************************************
* @purpose  Suspend MAC table syncronization.
*
* @param    void 
*
* @returns  void
*
* @notes    This function should be called before starting bcm_clear().
* @end
*********************************************************************/
extern void usl_mac_table_sync_suspend (void);

/*********************************************************************
* @purpose  Initiate table flush.
*
* @param    void 
*
* @returns  void
*
* @notes    This command should be issued before calling bcm_clear(). 
* @end
*********************************************************************/
extern void usl_mac_table_all_flush (void);





#endif
