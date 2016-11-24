/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename traputil_api.h
*
* @purpose Trap Manager Utilities Header File
*
* @component trapmgr
*
* @comments none
*
* @create 10/02/2003
*
* @author cpverne
*
* @end
*
**********************************************************************/

#ifndef _TRAPUTIL_API_H
#define _TRAPUTIL_API_H

#include "l7_common.h"
#include "trapmgr_exports.h"
#include "osapi.h"
#include "dtlapi.h"
#include "trapmgr_exports.h"

/*********************************************************************
* @purpose  Saves trapmgr user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSave(void);

/*********************************************************************
* @purpose  Restores trapmgr user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrRestore(void);

/*********************************************************************
* @purpose  Checks if trapmgr user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL trapMgrHasDataChanged(void);
void trapMgrResetDataChanged(void);
/*********************************************************************
* @purpose  Print the current Trap Manager config values to
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrConfigDump(void);

/*********************************************************************
* @purpose Initialize trapMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t trapMgrPhaseOneInit();

/*********************************************************************
* @purpose Initialize the trapMgr for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t trapMgrPhaseTwoInit();

/*********************************************************************
* @purpose Initialize the trapMgr for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t trapMgrPhaseThreeInit();

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrPhaseOneFini(void);

/*********************************************************************
* @purpose Unconfigure trapMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t trapMgrPhaseOneUnConfig();

/*********************************************************************
* @purpose Unconfigure trapMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t trapMgrPhaseTwoUnConfig();

/*********************************************************************
* @purpose Called when trapMgr goes into MGMT Ready State
*
* @param   void
*
* @returns nothing
*
* @notes  none
*
* @end
*********************************************************************/
void trapMgrUsmDbReadyCallback();

/*********************************************************************
* @purpose  Build trap system uptime string
*
* @param    time        system time
* @param    buf         buffer to build system uptime string
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrGetTimeString(L7_timespec time, L7_char8 *buf);

void trapMgrLinkChangeCallBack();

/*********************************************************************
 * @purpose  Register a trap SNMP send routine
 *
 * @param    trapMgrFunctionList   pointer to structure with function pointer
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t trapMgrRegister(trapMgrFunctionList_t *trapMgrFunctionList);

/*********************************************************************
 * @purpose  Deregister a trap from trapMgr
 *
 * @param    registrar_ID     one of L7_TRAP_ID_t
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t trapMgrDeregister(L7_TRAP_ID_t registrar_ID);

#endif /* _TRAPUTIL_API_H */
