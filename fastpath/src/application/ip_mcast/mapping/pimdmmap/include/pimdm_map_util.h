/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_map_util.h
*
* @purpose    PIMDM Mapping layer internal function prototypes
*
* @component  PIMDM Mapping Layer
*
* @comments   none
*
* @create     02/04/2002
*
* @author     gkiran
* @end
*
**********************************************************************/
#ifndef _PIMDM_MAP_UTIL_H_
#define _PIMDM_MAP_UTIL_H_

#include "l3_mcast_commdefs.h"
#include "pimdm_map_config.h"
#include "pimdm_map_cnfgr.h"
#include "pimdm_debug_api.h"


/* Enumeration for the Maximum number of Control Blocks that
 * PIM-DM supports.
 */
typedef enum
{
  PIMDM_MAP_IPV4_CB = 0,
  PIMDM_MAP_IPV6_CB,
  PIMDM_MAP_CB_MAX
}PIMDM_MAP_CB_TYPE_t;

typedef enum
{
  PIMDM_APP_TIMER_Q = 0,
  PIMDM_EVENT_Q,
  PIMDM_CTRL_PKT_Q,
  PIMDM_MAX_Q
}PIMDM_QUEUE_ID_t;


/*-------------------------------------*/
/*  PIMDM MAP INFO STRUCTURES            */
/*-------------------------------------*/

typedef struct pimdmInfo_s
{
  L7_BOOL         pimdmInitialized; /* indicates vendor PIMDM stack init'd */
  L7_BOOL         pimdmheapAlloc;   /* indicates heap alloc done or not */  
} pimdmInfo_t;

typedef struct pimdmMapQueue_s
{
  void        *QPointer;
  L7_uchar8   QName[32];
  L7_uint32   QSize;
  L7_uint32   QCount;
  L7_uint32   QCurrentMsgCnt;
  L7_uint32   QSendFailedCnt;
  L7_uint32   QMaxRx;
  L7_uint32   QRxSuccess;
  void        *QRecvBuffer;

}pimdmMapQueue_t;

typedef struct pimdmMapIntfInfo_s
{
  L7_BOOL      pimdmIsOperational; /* Indicates whether interface 
                                       is activated in vendor code */
  L7_uint32    intIfNum;         /* Interface number */

} pimdmMapIntfInfo_t;

typedef enum
{
  PIMDM_MAP_NUM_MFC_EVENTS = 0,
  PIMDM_MAP_NUM_MGMD_EVENTS,
  PIMDM_MAP_NUM_MFC_OVERFLOW_EVENTS,
  PIMDM_MAP_NUM_MGMD_OVERFLOW_EVENTS,
  PIMDM_MAP_COUNTERS_TYPE_MAX
} PIMDM_MAP_COUNTERS_TYPE_t;

typedef enum
{
  PIMDM_MAP_COUNTERS_INCREMENT,
  PIMDM_MAP_COUNTERS_DECREMENT,
  PIMDM_MAP_COUNTERS_RESET,
  PIMDM_MAP_COUNTERS_ACTION_MAX
} PIMDM_MAP_COUNTERS_ACTION_t;

typedef struct pimdmGblVars_s
{
  void                      *msgQSema;
  pimdmMapQueue_t            pimdmQueue[PIMDM_MAX_Q];
  void                       *queueCountersSemId;
  pimdmCnfgrState_t          pimdmCnfgrState;
  L7_int32                   pimdmMapTaskId;
  void                       *eventCountersSemId;
  L7_uint32                  eventCounters[PIMDM_MAP_COUNTERS_TYPE_MAX];
  L7_BOOL                    warmRestart; /* L7_TRUE if last restart was a warm restart */
}pimdmGblVars_t;

typedef struct pimdmMapCB_s
{
  pimdmMapIntfInfo_t        pimdmIntfInfo[L7_MAX_INTERFACE_COUNT];
  pimdmInfo_t               pimdmInfo;
  L7_uint32                 pimdmMapCfgMapTbl[L7_MAX_INTERFACE_COUNT];
  L7_pimdmMapCfg_t          pimdmMapCfgData;

  pimdmMapIntfInfo_t        *pPimdmIntfInfo;
  pimdmInfo_t               *pPimdmInfo;
  L7_uint32                 *pPimdmMapCfgMapTbl;
  L7_pimdmMapCfg_t          *pPimdmMapCfgData;
  L7_uchar8                  familyType;
  L7_BOOL                    warmRestartInProgress;       /* L7_TRUE if operating in warm Restart mode */
  osapiTimerDescr_t         *pimdmMapStartupTimer;
  MCAST_CB_HNDL_t            cbHandle;
  pimdmGblVars_t            *gblVars;
} pimdmMapCB_t;

extern pimdmGblVars_t pimdmGblVariables_g;
extern pimdmMapCB_t *pimdmMapCB_g;

/*  PIMDM Debug Trace Info */

#define PIMDM_DEBUG_CFG_FILENAME      "pimdm_debug.cfg"
#define PIMDM_DEBUG_CFG_VER_1          0x1
#define PIMDM_DEBUG_CFG_VER_CURRENT    PIMDM_DEBUG_CFG_VER_1

#define PIMDM_DEBUG_TRACE_FLAG_BYTES     ((PIMDM_DEBUG_LAST_TRACE + 7) / 8)
#define PIMDM_DEBUG_TRACE_FLAG_BITS_MAX      8
#define PIMDM_DEBUG_TRACE_FLAG_VALUE         1

typedef  L7_uchar8 pimdmDebugTraceFlags_t[PIMDM_DEBUG_TRACE_FLAG_BYTES];

typedef struct pimdmDebugCfgData_s 
{
  pimdmDebugTraceFlags_t pimdmDebugTraceFlag[PIMDM_MAP_CB_MAX];
} pimdmDebugCfgData_t;

typedef struct pimdmDebugCfg_s
{
  L7_fileHdr_t          hdr;
  pimdmDebugCfgData_t    cfg;
  L7_uint32             checkSum;
} pimdmDebugCfg_t;

/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* pimdm_util.c */
/*********************************************************************
* @purpose  Enable the PIMDM Routing Function
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    pimdmDoInit      @b{(input)} Flag to check whether PIM-DM
*                                        Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeEnable(pimdmMapCB_t *pimdmMapCbPtr,
                                     L7_BOOL pimdmDoInit);

/*********************************************************************
* @purpose  Disable the PIMDM Routing Function
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    pimdmDoInit      @b{(input)} Flag to check whether PIM-DM
*                                        Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapPimdmAdminModeDisable(pimdmMapCB_t *pimdmMapCbPtr,
                                      L7_BOOL pimdmDoInit);

/*********************************************************************
* @purpose  Set the PIMDM admin mode for the specified interface
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} Administrative mode 
*                                       (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMDM is enabled
*           2. Interface has been created and configured for PIMDM at
*              the PIMDM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfModeApply(L7_uint32 intIfNum, 
                              pimdmMapCB_t *pimdmMapCbPtr, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the PIMDM Hello interval for the specified interface
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    helloIntvl      @b{(input)} Hello Interval 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMDM is enabled
*           2. Interface has been created and configured for PIMDM at
*              the PIMDM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimdmMapIntfHelloIntervalApply(pimdmMapCB_t *pimdmMapCbPtr,
                          L7_uint32 intIfNum, L7_uint32 helloIntvl);

/*********************************************************************
* @purpose  Determine if the PIMDM component has been initialized
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapPimdmIsInitialized(pimdmMapCB_t *pimdmMapCbPtr);

/*********************************************************************
* @purpose  Determine if the PIMDM component has been initialized for
*           the specified interface.
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum         @b{(input)}  Interface Number.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapIntfIsOperational (pimdmMapCB_t *pimdmMapCbPtr, 
                                   L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface is valid for PIMDM configuration 
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    pCfg           @b{(input)} Output pointer location, 
*                                      or L7_NULL if not needed
*                          @b{(output)} Pointer to PIMDM interface 
*                                       config structure
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the PIMDM component has been 
*           started (regardless of whether the PIMDM protocol is enabled
*           or not).
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*       
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pimdmMapIntfIsConfigurable(pimdmMapCB_t *pimdmMapCbPtr, 
            L7_uint32 intIfNum, pimdmCfgCkt_t **pCfg);

/*********************************************************************
* @purpose  To get the mapping layer CB based on family Type
*
* @param    familyType     @b{(input)}  Address Family type.
* @param    pimdmMapCbPtr  @b{(output)} Mapping Control Block.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapCtrlBlockGet(L7_uchar8 familyType,
                             pimdmMapCB_t **pimdmMapCbPtr);

L7_RC_t
pimdmMapCountersUpdate (PIMDM_MAP_COUNTERS_TYPE_t counterType,
                        PIMDM_MAP_COUNTERS_ACTION_t counterAction);

L7_uint32
pimdmMapCountersValueGet (PIMDM_MAP_COUNTERS_TYPE_t counterType);

#endif /* _PIMDM_MAP_UTIL_H_ */
