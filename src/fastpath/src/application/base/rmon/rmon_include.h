/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_include.h
*
* @purpose  includes the Initial EntryCreate calls for the RMON MIB groups
*           it also implements the callback method from NIM
*
* @component RMON
*
*
* @create  08/06/2001
*
* @author  Kumar Manish 
* @end
*
**********************************************************************/

#ifndef _RMON_INCLUDE_H_
#define _RMON_INCLUDE_H_

#include "l7_common.h"
#include "nimapi.h"

/* A LAG table is defined , which stores the internal interface numbers and the corresponding */
/* snmp-indices */
typedef struct rmonLagEntry_s{

    L7_uint32 lagInterface;
    L7_uint32 snmpIndexStat;
    L7_uint32 snmpIndexHist1;
    L7_uint32 snmpIndexHist2;

}rmonLagEntry_t;


/* Begin Function Prototypes */

/*********************************************************************
* @purpose  to re-create entries for RMON groups 
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void rmonReInitializeTables();

/*********************************************************************
* @purpose  to create entries for RMON groups 
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void rmonUnconfigureTables();

/*********************************************************************
* @purpose  Process Link state changes and create / delete entries in Statistics
*           and History tables as per those changes     
*
* @param    intIfnum    port changing state
* @param    event       new state
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t rmonIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                               NIM_CORRELATOR_t correlator);

L7_BOOL rmonIsValidIntfType(L7_INTF_TYPES_t intfType);
L7_BOOL rmonIsValidIntf(L7_uint32 intIfNum);
L7_RC_t rmonIntfCreate(L7_uint32 intIfNum);
L7_RC_t rmonIntfDelete(L7_uint32 intIfNum);
void rmonIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
*
* @purpose  timer function, which sleeps for one second and calls the 
*           callback function in a forever loop
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void 
rmonTimerCall();

/*****************************************************************************************
*
* @purpose  Callback function for timer to perform check whether a lag is configured
*           or not, if it is configured push the entry in the corresponding index mapping 
*           table for Stat and History groups, if lag is unconfigured(after being configured),
*           remove the entry from corresponding index mapping tables 
*
* @param    void
*
* @returns  void
 
* @notes    none 
*
* @end
*******************************************************************************************/
void
rmonLagTimerCallback();


/*********************************************************************
*
* @purpose  Callback function for timer to perform polling activities 
*           for Alarm  and raising alarm accordingly
* 
* @param    void          
*
* @returntype void
*          
* @notes      none
*
* @end
*********************************************************************/
void 
rmonAlarmTimerCallBack();



/********************************************************
** Test Code for hot-pluggability.
*********************************************************/
void rmonDeleteTest (void);

/*********************************************************************
*
* @purpose    Initialize the RMON Lag Array table.
*
* @returntype Always L7_SUCCESS    
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonLagArrayInitialize(void); 
      
/*********************************************************************
* @purpose Initialize RMON for Phase 1
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
L7_RC_t rmonPhaseOneInit();

/*********************************************************************
* @purpose Initialize RMON for Phase 2
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
L7_RC_t rmonPhaseTwoInit();

/*********************************************************************
* @purpose Initialize RMON for Phase 3
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
L7_RC_t rmonPhaseThreeInit();

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
void rmonPhaseOneFini(void);

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rmonPhaseTwoFini(void);

/*********************************************************************
* @purpose Unconfigure RMON for Phase 1
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
L7_RC_t rmonPhaseOneUnConfig();

/*********************************************************************
* @purpose Unconfigure RMON for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseTwoUnConfig();

/* End Function Prototypes */

#endif /* _RMON_INCLUDE_H_ */
