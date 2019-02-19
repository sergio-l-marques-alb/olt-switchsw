/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_util.h
*
* @purpose    PIM-SM Mapping layer internal function prototypes
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     03/07/2002
*
* @author    gkiran/dsatyanarayana
* @end
*
**********************************************************************/


#ifndef _PIMSM_UTIL_H_
#define _PIMSM_UTIL_H_

#include "async_event_api.h"
#include "mfc_api.h"
#include "l7_mgmd_api.h"
#include "pimsmuictrl.h"
#include "pimsm_cnfgr.h"
#include "pimsm_debug_api.h"


typedef enum {
   PIMSM_MAP_IPV4_CB = 0,
   PIMSM_MAP_IPV6_CB, 
   PIMSM_MAP_CB_MAX
} PIMSM_MAP_CB_t;

typedef enum
{
  PIMSM_APP_TIMER_Q = 0,
  PIMSM_EVENT_Q,
  PIMSM_CTRL_PKT_Q,
  PIMSM_DATA_PKT_Q,
  PIMSM_MAX_Q
}PIMSM_QUEUE_ID_t;


typedef struct pimsmOperCandBSRdata_s {
   L7_BOOL        operMode;               /* TRUE - add, FALSE - delete */
   L7_inet_addr_t candBSRAddress;   
} pimsmOperCandBSRdata_t;

typedef struct pimsmOperCandRPdata_s {
    L7_BOOL        operMode;               /* TRUE - add, FALSE - delete */
   L7_inet_addr_t candRPAddress;   
} pimsmOperCandRPdata_t;
/*-------------------------------------*/
/*  PIM-SM MAP INFO STRUCTURES         */
/*-------------------------------------*/

typedef struct pimsmInfo_s
{
  L7_BOOL         pimsmOperational;  /* indicates vendor stack is **
                                     ** operational               */

  L7_BOOL         pimsmHeapAlloc;    /* indicates heap allocated or not  */

                                      
  L7_BOOL         pimsmStaticRPOper[L7_MAX_STATIC_RP_NUM];
                                        /* Indicates whether the static RP was
                                           added to vendor  stack.
                                        */
  L7_BOOL         pimsmSsmIsEmpty[L7_MAX_SSM_RANGE];    

  pimsmOperCandBSRdata_t  pimsmCandBSROper;
  pimsmOperCandRPdata_t pimsmCandRPOper[L7_PIMSM_MAX_CAND_RP_NUM];
} pimsmInfo_t;

typedef struct pimsmIntfInfo_s
{
  L7_BOOL         pimsmOperational;  /* indicates protocol is operational **
                                     ** on interface                      */
  AcquiredMask    acquiredList;      /* Mask of components "acquiring" **
                                     ** an interface {LAG)*/
} pimsmIntfInfo_t;

typedef struct pimsmMapQueue_s
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

}pimsmMapQueue_t;

typedef enum
{
  PIMSM_MAP_NUM_MFC_EVENTS = 0,
  PIMSM_MAP_NUM_MGMD_EVENTS,
  PIMSM_MAP_NUM_MFC_OVERFLOW_EVENTS,
  PIMSM_MAP_NUM_MGMD_OVERFLOW_EVENTS,
  PIMSM_MAP_COUNTERS_TYPE_MAX
} PIMSM_MAP_COUNTERS_TYPE_t;

typedef enum
{
  PIMSM_MAP_COUNTERS_INCREMENT = 0,
  PIMSM_MAP_COUNTERS_DECREMENT,
  PIMSM_MAP_COUNTERS_RESET,
  PIMSM_MAP_COUNTERS_ACTION_MAX
} PIMSM_MAP_COUNTERS_ACTION_t;

typedef struct pimsmGblVariables_s
{
  void                      *tempBuffer; /* TBD: To temporarily resolve the PIM-SM MsgQ Memory Corruption */

  pimsmMapQueue_t            pimsmQueue[PIMSM_MAX_Q];
  void                       *queueCountersSemId;
  void                       *msgQSema;
  pimsmCnfgrState_t          pimsmCnfgrState;
  L7_uint32                  pimsmMapTaskId;
  void                       *eventCountersSemId;
  L7_uint32                  eventCounters[PIMSM_MAP_COUNTERS_TYPE_MAX];
} pimsmGblVariables_t;

typedef struct pimsmMapCB_s
{
  L7_pimsmMapCfg_t           pimsmMapCfgData;
  L7_uint32                  pimsmMapCfgMapTbl[L7_MAX_INTERFACE_COUNT];
  pimsmInfo_t                pimsmInfo;
  pimsmIntfInfo_t            pimsmIntfInfo[L7_MAX_INTERFACE_COUNT];

  /* following are the pointers to above structs */
  L7_pimsmMapCfg_t          *pPimsmMapCfgData; 
  L7_uint32                 *pPimsmMapCfgMapTbl;
  pimsmInfo_t               *pPimsmInfo;
  pimsmIntfInfo_t           *pPimsmIntfInfo;

  L7_uchar8                  cfgFilename[L7_MAX_FILENAME];    /* cfg file name */
  L7_uint32                  familyType;
  PIMSM_MAP_CB_t             cbIndex;
  MCAST_CB_HNDL_t            cbHandle;
  pimsmGblVariables_t       *gblVars;
} pimsmMapCB_t;

typedef struct pimsmCfgDefs_s {
  L7_uint32     familyType;
  L7_uchar8     cfgFilename[L7_MAX_FILENAME];    /* cfg file name */
  L7_char8      regName[80];
  L7_RC_t       (*rtoRegisterFn)(L7_uchar8 *, void (*funcPtr)(void));
  void          (*rtoCallbackFn)(void);
  L7_RC_t       (*rtoDeRegisterFn) (L7_uchar8 *, void (*funcPtr)(void));  
  L7_uint32     ipMapRegId;
  L7_RC_t       (*ipMapRegisterFn)(L7_uint32 ,  L7_char8 *name, 
                                       L7_RC_t (*notify)());
  L7_RC_t       (*ipMapCallbackFn)(L7_uint32 , L7_uint32 ,
                                void *, ASYNC_EVENT_NOTIFY_INFO_t *);
  L7_RC_t       (*ipMapDeRegisterFn)(L7_uint32 );
  void          (*buildDefaultConfigData)(L7_uint32 ver);
  void          (*migrateConfigData) (L7_uint32 oldVer, L7_uint32 ver, 
                                       L7_char8 * pCfgBuffer);   
} pimsmCfgDefs_t;


/*  PIMSM Debug Trace Info */

#define PIMSM_DEBUG_CFG_FILENAME      "pimsm_debug.cfg"
#define PIMSM_DEBUG_CFG_VER_1          0x1
#define PIMSM_DEBUG_CFG_VER_CURRENT    PIMSM_DEBUG_CFG_VER_1

#define PIMSM_DEBUG_TRACE_FLAG_BYTES     ((PIMSM_DEBUG_LAST_TRACE + 7) / 8)
#define PIMSM_DEBUG_TRACE_FLAG_BITS_MAX      8
#define PIMSM_DEBUG_TRACE_FLAG_VALUE         1

typedef  L7_uchar8 pimsmDebugTraceFlags_t[PIMSM_DEBUG_TRACE_FLAG_BYTES];

typedef struct pimsmDebugCfgData_s 
{
  pimsmDebugTraceFlags_t pimsmDebugTraceFlag[PIMSM_MAP_CB_MAX];
} pimsmDebugCfgData_t;

typedef struct pimsmDebugCfg_s
{
  L7_fileHdr_t          hdr;
  pimsmDebugCfgData_t    cfg;
  L7_uint32             checkSum;
} pimsmDebugCfg_t;


/*-------------------------------------------------*/
/*              FUNCTION PROTOTYPES                */
/*-------------------------------------------------*/

/*********************************************************************
* @purpose  Enable the PIM-SM Routing Function
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    pimsmDoInit     @b{(input)} Flag to check whether PIM-SM
*                                       Memory can be Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Admin mode is enabled and the Mapping layer Init function
*           is called so that vendor stack can be initialized and
*           protocol made operational if all configurability conditions
*           are satisfied
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeEnable(pimsmMapCB_t *pimsmMapCbPtr,
                                     L7_BOOL pimsmDoInit);

/*********************************************************************
* @purpose  Disable the PIM-SM Routing Function
*
* @param    pimsmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    pimsmDoInit     @b{(input)} Flag to check whether PIM-SM
*                                       Memory can be De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeDisable(pimsmMapCB_t *pimsmMapCbPtr,
                                      L7_BOOL pimsmDoInit);
/*********************************************************************
* @purpose  Apply the static RP information
*
* @param    pimsmMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    index          @b{(input)}  Index into the config structure 
*                                       for this static RP
* @param    rpIpAddr       @b{(input)}  Ip address of the RP
* @param    rpGrpAddr      @b{(input)}  Group address supported by the RP
* @param    rpGrpMask      @b{(input)}  Group mask for the group address
* @param    conflict       @b{(input)}  conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPApply(pimsmMapCB_t *pimsmMapCbPtr, 
                        L7_inet_addr_t *rpIpAddr, L7_inet_addr_t *rpGrpAddr, 
                        L7_inet_addr_t *rpGrpMask, L7_BOOL conflict, L7_uint32 index);

/*********************************************************************
* @purpose  Remove the static RP information
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    index         @b{(input)}       Index into the config structure 
*                                           for this static RP
* @param    rpIpAddr      @b{(inout)}       Ip address of the RP
* @param    rpGrpAddr     @b{(inout)}       Group address supported by the RP
* @param    rpGrpMask     @b{(inout)}       Group mask for the group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRemoveApply(pimsmMapCB_t *pimsmMapCbPtr,
        L7_inet_addr_t *rpIpAddr, 
        L7_inet_addr_t *rpGrpAddr, L7_inet_addr_t *rpGrpMask, L7_uint32 index);

/*********************************************************************
* @purpose  Set the PIMSM admin mode for the specified interface
*
* @param    pimsmMapCbPtr @b{(input)}       Mapping Control Block.
* @param    intIfNum      @b{(input)}       Internal Interface Number
* @param    mode          @b{(input)}       Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes:
*           1. PIMSM is enabled
*           2. Interface has been created and configured for PIMSM at
*              the PIMSM vendor layer
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeApply(pimsmMapCB_t *pimsmMapCbPtr,
                            L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Candidate RP preference for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    crpPreference     @b{(output)} Candidate RP Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCRPPreferenceApply(pimsmMapCB_t *pimsmMapCbPtr, 
                            L7_uint32 intIfNum, L7_int32 crpPreference);

/*********************************************************************
* @purpose  Determine if the interface is valid for PIM-SM configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    pimsmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    pCfg           @b{(output)}Output pointer to PIM-SM interface 
*                                      config structure or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the PIM-SM component has been
*           started (regardless of whether the PIM-SM protocol is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pimsmMapInterfaceIsConfigurable(pimsmMapCB_t *pimsmMapCbPtr,
                     L7_uint32 intIfNum, pimsmCfgCkt_t **pCfg);

/*********************************************************************
* @purpose  Apply the RP candidate (ENABLE/DISABLE)
*
* @param    pimsmMapCbPtr  @b{(input)}      Mapping Control Block.
* @param    rpAddr         @b{(input)}      candidate RP address.
* @param    rpGrpAddr      @b{(input)}      Group address supported by the RP
* @param    rpGrpMask      @b{(input)}      Group mask for the group address
* @param    mode           @b{(input)}      mode (ENABLE/DISABLE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPApply(pimsmMapCB_t *pimsmMapCbPtr, 
           L7_uint32 intIfNum,L7_inet_addr_t *rpAddr, 
           L7_inet_addr_t *rpGrpAddr, 
             L7_inet_addr_t *rpGrpMask, L7_uint32 mode, L7_uint32 index);

/*********************************************************************
* @purpose  Apply the BSR candidate (ENABLE/DISABLE)
*
* @param    pimsmMapCbPtr  @b{(input)}      Mapping Control Block.
* @param    BsrAddr        @b{(input)}      BSR Address
* @param    priority       @b{(input)}      Priority
* @param    mode           @b{(input)}      mode (ENABLE/DISABLE).
* @param    length         @b{(input)}      maskLen
* @param    scope          @b{(input)}      scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRApply(pimsmMapCB_t *pimsmMapCbPtr, 
                        L7_inet_addr_t *bsrAddr, L7_uint32 intIfNum, L7_uint32 priority, 
                        L7_uint32 mode, L7_uint32 length, L7_uint32 scope);

/*********************************************************************
* @purpose  Set the DR Priority for the specified interface
*
* @param    pimsmMapCbPtr     @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    priority          @b{(input)} Designated Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPriorityApply(pimsmMapCB_t *pimsmMapCbPtr,
                             L7_uint32 intIfNum, L7_uint32 priority);

/*********************************************************************
* @purpose  Set the hello interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    helloIntvl           Hello Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceHelloIntervalApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                            L7_uint32 intIfNum, L7_uint32 helloIntvl);

/*********************************************************************
* @purpose  Set the join prune interval for an interface
*
* @param    pimsmMapCbPtr        Mapping Control Block.
* @param    intIfNum             Interface num.
* @param    joinPruneIntvl       Join-prune Interval in secs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceJoinPruneIntervalApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                            L7_uint32 intIfNum, L7_uint32 joinPruneIntvl);
/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr     @b{(input)}      Mapping Control Block.
* @param    dataThresholdRate @b{(input)}      Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                       L7_uint32 dataThresholdRate);
/*********************************************************************
* @purpose  Set the data threshold rate for switching to SPT
*
* @param    pimsmMapCbPtr    @b{(input)}    Mapping Control Block.
* @param    regThresholdRate @b{(input)}    Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done before this function is invoked
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateApply(pimsmMapCB_t *pimsmMapCbPtr, 
                                               L7_uint32 regThresholdRate);

L7_RC_t
pimsmMapCountersUpdate (PIMSM_MAP_COUNTERS_TYPE_t counterType,
                        PIMSM_MAP_COUNTERS_ACTION_t counterAction);

L7_uint32
pimsmMapCountersValueGet (PIMSM_MAP_COUNTERS_TYPE_t counterType);

#endif /* _PIMSM_UTIL_H_ */
