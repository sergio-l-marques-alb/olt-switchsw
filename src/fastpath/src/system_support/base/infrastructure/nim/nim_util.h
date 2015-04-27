/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_util.h
*
* @purpose NIM utilities
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author mbaucom
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef NIM_UTIL_H
#define NIM_UTIL_H

#include "l7_common.h"
#include "nim_data.h"
#include "nim_ifindex.h"
#include "nimapi.h"
#include "registry.h"
#include "statsapi.h"
#include "sysapi_hpc.h"
#include "platform_config.h"
#include "nim_debug.h"

/*********************************************************************
* @purpose  return the configuration phase
*
* @param     none
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
L7_CNFGR_STATE_t nimConfigPhaseGet(void);


/*********************************************************************
* @purpose  create a unit slot port mapping to interface number
*
* @param    usp         Pointer to nimUSP_t structure
* @param    intIntfNum    interface number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimUnitSlotPortToIntfNumSet(nimUSP_t *usp, L7_uint32 intIntfNum);



/*********************************************************************
* @purpose  Calculates the length in bytes of the largest payload given
*           the max frame length and encapsulation configured on this
*           interface. If the length changes from what was previously 
*           calculated, send a L7_SET_MTU_SIZE event.
*
* @param    intIfNum    Internal Interface Number
*
* @returns  MTU size of the interface
*
* @notes    Should be called whenever the max frame length or encapsulation
*           type changes on an interface. 
*
* @end
*********************************************************************/
L7_uint32 nimCalcMtuSize(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the counter size in bits
*
* @param    id
*
* @returns  counter size in bits
*
* @notes    none 
*       
* @end
*********************************************************************/
counter_size_t nimGetCounterSize(counter_id_t id);


/*********************************************************************
* @purpose  Creates an NIM interface counters
*
* @param    intIfNum    Internal Interface Number 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimInterfaceCounters ( L7_uint32 intIfNum ,L7_BOOL create);

/*********************************************************************
* @purpose  Resets counters for an NIM interface
*
* @param    intIfNum    Internal Interface Number 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimResetInterfaceCounters ( L7_uint32 intIfNum );

/*********************************************************************
* @purpose  Creates an interface of the specified type
*
* @param    intfType    Type of interface to create
*                       (@b{  L7_PHYSICAL_INTF,
*                             L7_STACK_INTF,
*                             L7_CPU_INTF,
*                             L7_LAG_INTF or				
*                             L7_LOGICAL_VLAN_INTF})				
*
* @param    intIfNum    pointer to requested Internal Interface Number,
*                       (@b{Returns: a Internal Interface Number, if 
*                       requested Internal Interface Number was L7_NULL}).
*
* @param    usp         Pointer to numUSP_t structure, if applicable
*                                                                   
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimCreateInterface(L7_INTF_TYPES_t intfType, L7_uint32 *intIfNum, 
                           nimUSP_t *usp,SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);


/*********************************************************************
* @purpose  Deletes the specified interface 
*
* @param    intIfNum Internal Interface Number 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimDeleteInterface(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Create the internal interface number for the interface
*
* @param    configId  @b{(input)}  The configuration ID to be searched
* @param    intIfNum  @b{(output)} Internal Interface Number created
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimIntIfNumCreate(nimConfigID_t configId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Check to see if the number of interfaces per type are exceeded
*
* @param    intfType  @b{(input)} one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_BOOL nimNumberOfInterfaceExceeded(L7_INTF_TYPES_t intfType);



/*********************************************************************
* @purpose  Get the maximum number of interfaces for an interface type
*
* @param    intfType  @b{(input)} one of L7_INTF_TYPES_t
*
* @returns  the maximum number of interfaces for an interface type, 0 or greater
*
* @notes    Returns zero if an interface type is unrecognized
*       
* @end
*********************************************************************/
L7_int32 nimMaxIntfForIntfTypeGet(L7_INTF_TYPES_t intfType);


/*********************************************************************
* @purpose  Delete the internal interface number for the interface
*
* @param    configId  @b{(input)}  The configuration ID to be removed
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimConfigInterfaceDelete(nimConfigID_t configId);


/*********************************************************************
* @purpose  Set the state of an interface
*
* @param    intIfNum    The internal interface number 
* @param    state       The state of the interface from the L7_INTF_STATES_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimUtilIntfStateSet(L7_uint32 intIfNum, L7_INTF_STATES_t state);

/*********************************************************************
* @purpose  Get the state of an interface
*
* @param    intIfNum    The internal interface number 
*
* @returns  The state of the interface
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_INTF_STATES_t nimUtilIntfStateGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Create the internal interface number for the interface
*
* @param    intIfNum  @b{(input)} Internal Interface Number deleted
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If the action fails, the intIfNum will be set to zero 
*       
* @end
*********************************************************************/
L7_RC_t nimIntIfNumDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine the next State to transition to
*          
* @param    currState    @b{(input)}    The current state
* @param    event        @b{(input)}    The event being processed
* @param    nextState    @b{(output)}   The next state to transition to
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimUtilIntfNextStateGet(L7_INTF_STATES_t currState,L7_PORT_EVENTS_t event,L7_INTF_STATES_t *nextState);  


/*********************************************************************
* @purpose  Add an entry to the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryAdd(  nimConfigIdTreeData_t  *pConfigIdInfo);

/*********************************************************************
* @purpose  Delete an entry from the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryDelete(  nimConfigIdTreeData_t  *pConfigIdInfo); 

/*********************************************************************
* @purpose  Get an entry from the nim nimConfigIdTreeData AVL tree
*          
* @param    *pConfigId    @b{(input)} pointer to a nimConfigID_t structure
* @param    *pConfigIdInfo    @b{(input)} pointer to a nimConfigIdTreeData_t structure
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t nimConfigIdTreeEntryGet(nimConfigID_t  *pConfigId, 
                                nimConfigIdTreeData_t  *pConfigIdInfo );
 
/*********************************************************************
* @purpose  Set either the saved config or the default config in the interface
*
* @param    port        @b{(input)}   The interface being manipulated
* @param    cfgVersion  @b{(input)}   version of the config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
void nimConfigIdTreePopulate(void);


/*********************************************************************
* @purpose  Populate valid ranges for intIfNums for intf types
*
* @param    void
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
void nimIntIfNumRangePopulate(void);

/*********************************************************************
* @purpose  delete a unit slot port mapping to interface number
*
* @param    usp         Pointer to nimUSP_t structure
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This function is not re-entrant, the caller must ensure integrety of data 
*       
* @end
*********************************************************************/
L7_RC_t nimUnitSlotPortToIntfNumClear(nimUSP_t *usp);

/*********************************************************************
* @purpose  Log NIM Message 
*
* @param     logError - is this a log error format
* @param     fileName - file
* @param     lineNum - Line
* @param     format - Format of the output.
* @param     ... - Variable output list.
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
void nimLogMsg (L7_BOOL logError, L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...);

/*********************************************************************
* @purpose  Trace NIM Message 
*
* @param     fileName - file
* @param     lineNum - Line
* @param     format - Format of the output.
* @param     ... - Variable output list.
*
* @returns  the phase number
*
* @notes    
*       
* @end
*********************************************************************/
void nimTraceMsg (L7_char8 * fileName, L7_uint32 lineNum, L7_char8 * format, ...);

/*******************************************************************************
* @purpose  to convert the new added interface speed values to that of old values
*
*
* @param    intIfNum           Internal Interface Number
* @param    newintfSpeed       the speed value as per the new added values into
*                              L7_PORT_SPEEDS_t
* @param    *intfSpeed         the pointer to place the old value that is determined
*                              from the new intf speeds values added into L7_PORT_SPEEDS_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    this function is added for the IS-CLI compliance.The newintfSpeed has the value
*           that is equivalent to the new enum values that are added into L7_PORT_SPEEDS_t
*           the new values are { L7_PORT_CAPABILITY_HALF        = 0x100,
*                                L7_PORT_CAPABILITY_FULL        = 0x200,
*                                L7_PORT_CAPABILITY_DUPLEX_AUTO = 0x400,
*                                L7_PORT_CAPABILITY_10          = 0x800,
*                                L7_PORT_CAPABILITY_100         = 0x1000,
*                                L7_PORT_CAPABILITY_1000          = 0x2000,
*                                L7_PORT_CAPABILITY_10G         = 0x4000,
*                                L7_PORT_CAPABILITY_SPEED_AUTO  = 0x8000 }
*
*
*
* @end
*********************************************************************************/
L7_RC_t nimConvertoOldSpeedvalue(L7_uint32 intIfNum, L7_uint32 newintfSpeed, L7_uint32 *intfSpeed);

/****************************************
*
*  NIM MACROS                    
*
*******************************************/
#define GET_UNIT(usp,value) \
  { \
    value = (usp->unit << 20 | usp->slot <<12 | usp->port); \
  }

#define NIM_LOG_MSG(format, args...) \
          nimLogMsg (L7_FALSE,__FILE__, __LINE__, format, ## args)

#define NIM_LOG_ERROR(format, args...) \
          nimLogMsg (L7_TRUE,__FILE__, __LINE__, format, ## args); \
          LOG_ERROR(0xbadd) 

#define IS_INTIFNUM_PRESENT(_intIfNum,_rc) \
   { \
      if ( ( _intIfNum < 1 ) || ( _intIfNum > platIntfTotalMaxCountGet()) ) \
      {  \
         _rc =  ( L7_FAILURE ); \
      } else \
      if ( nimCtlBlk_g->nimPorts[_intIfNum].present != L7_TRUE ) \
      { \
         _rc =  ( L7_ERROR ); \
      } \
      else \
      { \
        _rc = L7_SUCCESS; \
      } \
   }

#define NIM_CRIT_SEC_READ_ENTER() \
  { \
    osapiReadLockTake(nimCtlBlk_g->rwLock, L7_WAIT_FOREVER);\
  }
 
#define NIM_CRIT_SEC_READ_EXIT() \
  { \
    osapiReadLockGive(nimCtlBlk_g->rwLock); \
  }
#define NIM_CRIT_SEC_WRITE_ENTER() \
  { \
  	osapiWriteLockTake(nimCtlBlk_g->rwLock, L7_WAIT_FOREVER);\
  }

#define NIM_CRIT_SEC_WRITE_EXIT() \
  { \
  	osapiWriteLockGive(nimCtlBlk_g->rwLock); \
  }

/* Trace messages are logged and printed only if the nimPrintLog is set to true */
#define NIM_TRACE_MSG(format, args...) \
          nimTraceMsg (__FILE__, __LINE__, format, ## args)

#endif /* NIM_UTIL_H */
