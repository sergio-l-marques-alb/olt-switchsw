/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_util.h
*
* @purpose    DiffServ component utility function prototypes
*
* @component  diffserv
*
* @comments   none
*
* @create     03/20/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_UTIL_H
#define INCLUDE_DIFFSERV_UTIL_H

#include "l7_common.h"
#include "l7_diffserv_api.h" 
#include "usmdb_qos_diffserv_api.h"
#include "l7_cnfgr_api.h"
    

/****************************************
*
*  DIFFSERV GENERAL CONSTANTS
*
*****************************************/

#define DIFFSERV_MBPS_TO_KBPS                   1000    /* multiplier Mb->Kb  */
#define DIFFSERV_PERCENTAGE_UNITS               100     /* unit = 1% of total */

/* DiffServ notification events (internal definitions) */
typedef enum
{
  DIFFSERV_EVENT_CREATE = 1,
  DIFFSERV_EVENT_MODIFY,
  DIFFSERV_EVENT_DELETE,
  DIFFSERV_EVENT_RENAME
} diffServEvent_t;

/* DiffServ callback table identifiers */
typedef enum
{
  DIFFSERV_CALLBACK_TABLE_ID_FIRST = 0,         /* range checking alias for first table ID */
  DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT = 0,  /* must be assigned to same value as ID_FIRST */
  DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL,
  DIFFSERV_CALLBACK_TABLE_ID_TOTAL              /* total number of callback tables supported */
} diffServCallbackTableId_t;

/* Maximum number of DiffServ callback registrants allowed */
#define DIFFSERV_CALLBACK_REGISTRANTS_MAX_NOTIFY_EVENT      FD_CNFGR_DIFFSERV_MAX_CALLBACK_REGISTRATIONS
#define DIFFSERV_CALLBACK_REGISTRANTS_MAX_DELETE_APPROVAL   FD_CNFGR_DIFFSERV_MAX_CALLBACK_REGISTRATIONS


/*--------------------------------------*/
/*  DIFFSERV INFO STRUCTURES            */
/*--------------------------------------*/

typedef struct diffServInfo_s     
{
    L7_BOOL         diffServInitialized;
    L7_BOOL         diffServReady;
    L7_uint32       diffServTraceMode;
    L7_uint32       diffServTraceHandle;  /* from trace utility registration */
    L7_uint32       diffServMsgLvl;
} diffServInfo_t;

extern diffServInfo_t         *pDiffServInfo_g;
      
typedef struct 
{
  AcquiredMask    acquiredList;   /* bit mask of components that have acquired the interface */
} diffServIntfInfo_t;

extern diffServIntfInfo_t     *pDiffServIntfInfo_g;

/* DiffServ callback table entry definition */
#define DIFFSERV_CALLBACK_DISPLAY_STR_MAX       63
typedef struct
{
  L7_VOIDFUNCPTR_t      funcPtr;                                          /* callback function */
  L7_COMPONENT_IDS_t    compId;                                           /* registrant component ID */
  L7_uchar8             displayStr[DIFFSERV_CALLBACK_DISPLAY_STR_MAX+1];  /* display string (for debugging) */
} diffServCallbackEntry_t;

typedef struct
{
  diffServCallbackEntry_t     *pStart;          /* pointer to first entry in table */
  L7_uint32                   maxEntries;       /* maximum number of entries supported */
} diffServCallbackTable_t;

/* DiffServ callback table control information */
typedef struct
{
  diffServCallbackTable_t table[DIFFSERV_CALLBACK_TABLE_ID_TOTAL];
  L7_BOOL                 isInitialized;
  void *                  semId;
} diffServCallbackInfo_t;

/* General-purpose null pointer check (useful for API parm checking) */
#define DIFFSERV_NULLPTR_CHECK(_ptr)\
{ \
  if ((_ptr) == L7_NULLPTR) \
    return L7_FAILURE; \
}

/* Semaphore take/give macros
 *
 * NOTE: DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
 *       where awareness of the time expiration is needed.  Make a direct
 *       call to osapiSemaTake() instead.
 */
#define DIFFSERV_SEMA_TAKE(_s, _t) \
  diffServSemaTake((_s), (_t), __FILE__, __LINE__)

#define DIFFSERV_SEMA_GIVE(_s) \
  diffServSemaGive((_s), __FILE__, __LINE__)


/****************************************
*
*  DEBUGGING MACROS, FUNCTION PROTOTYPES
*
*****************************************/

/* Debug message levels */
#define DIFFSERV_MSGLVL_ON    0         /* use where printf is desired */
#define DIFFSERV_MSGLVL_HI    1
#define DIFFSERV_MSGLVL_MED   50
#define DIFFSERV_MSGLVL_LO    100
#define DIFFSERV_MSGLVL_LO_2  200
#define DIFFSERV_MSGLVL_OFF   10000     /* some arbitrarily large value */

#define DIFFSERV_PRT(mlvl, fmt, args...) \
          { \
          if (pDiffServInfo_g->diffServMsgLvl >= (mlvl)) \
            sysapiPrintf(fmt, ##args); \
          }


/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* diffserv.c */
L7_BOOL diffServHasDataChanged(void);
void diffservResetDataChanged(void);
L7_RC_t diffServSave(void);
L7_RC_t diffServRestore(void);
L7_RC_t diffServReplace(L7_diffServCfg_t *pCfgData);
void diffServBuildDefaultIntfConfigData(nimConfigID_t *configId, L7_diffServIntfCfg_t *pCfg);
void    diffServBuildDefaultConfigData(L7_uint32 ver);
void diffServMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
L7_RC_t diffServConfigDataApply(void);
L7_RC_t diffServAdminModeApply(L7_uint32 mode);
L7_RC_t diffServIntfCreate(L7_uint32 intIfNum);
L7_RC_t diffServApplyIntfConfigToMIB(L7_uint32 intIfNum);
L7_RC_t diffServCollateralConfigApply(L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t diffServIntfDetach(L7_uint32 intIfNum, L7_BOOL noMsg);
L7_RC_t diffServIntfDelete(L7_uint32 intIfNum);
L7_uint32 diffServIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);
L7_BOOL diffServIsIntfAcquired(L7_uint32 intIfNum);
L7_BOOL diffServMapIntfIsConfigurable(L7_uint32 intIfNum, L7_diffServIntfCfg_t **pCfg);
L7_BOOL diffServMapIntfConfigEntryGet(L7_uint32 intIfNum, L7_diffServIntfCfg_t **pCfg);
void diffServBuildTestIntfConfigData(nimConfigID_t *configId, L7_diffServIntfCfg_t *pCfg);
void diffServBuildTestConfigData(void);
void diffServSaveTestConfigData(void);
void diffServConfigDataTestShow(void);

/* diffserv_callback.c */
L7_RC_t diffServCallbackInit(void);
void    diffServCallbackFini(void);
void    diffServCallbackPolicyAllSend(void);
L7_BOOL diffServCallbackRegisterIsReady(void);
L7_BOOL diffServCallbackPolicyIsReady(L7_uint32 policyIndex);
void    diffServChangeNotify(L7_uint32 policyIndex, diffServEvent_t event, L7_uchar8 *policyOldName);
void    diffServDeleteNotify(L7_uint32 policyIndex, dsmibPolicyType_t policyType, L7_uchar8 *policyName);
void    diffServNotifyEventSend(L7_DIFFSERV_NOTIFY_EVENT_t callbackEvent, 
                                dsmibPolicyType_t policyType, 
                                L7_uint32 policyIndex, 
                                L7_uchar8 *policyName, 
                                L7_uchar8 *policyOldName);
L7_RC_t diffServDeleteApprovalQuery(L7_uint32 policyIndex);

/* diffserv_cnfgr.c */
L7_RC_t diffServCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t diffServCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t diffServCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );
void diffServCnfgrFiniPhase1Process();
void diffServCnfgrFiniPhase2Process();
void diffServCnfgrFiniPhase3Process();
L7_RC_t diffServCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t diffServCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

/* diffserv_config.c */
L7_RC_t diffServBuildUserConfigData(L7_uint32 ver, L7_diffServCfg_t *pCfgData);
L7_RC_t diffServGlobalConfig(void);
L7_RC_t diffServClassConfig(void);
L7_RC_t diffServPolicyConfig(void);
L7_RC_t diffServServiceConfig(void);
L7_RC_t diffServServiceIntfConfig(L7_uint32 intIfNum);
L7_RC_t diffServResetCleanUp(void);
void    dsmibConfigDataShow(L7_diffServCfg_t *pCfgData);
void    diffServConfigDataShow(void);

/* diffserv_outcalls.c */
L7_BOOL diffServIsAclPresent(void);
L7_RC_t diffServAclNumCheckValid(dsmibClassAclType_t aclType, L7_uint32 aclNum);
L7_RC_t diffServAclNumFirstGet(L7_uint32 *val);
L7_RC_t diffServAclExtract(dsmibClassEntryCtrl_t *pRow, 
                           dsmibClassAclType_t aclType, L7_uint32 aclNum);
L7_RC_t diffServAclIpExtract(dsmibClassEntryCtrl_t *pRow, L7_uint32 aclNum);
L7_RC_t diffServAclMacExtract(dsmibClassEntryCtrl_t *pRow, L7_uint32 aclNum);
L7_RC_t diffServAclIpClassRuleSetup(dsmibClassEntryCtrl_t *pClass, 
                                    L7_uint32 aclNum,
                                    L7_uint32 ruleNum);
L7_RC_t diffServAclMacClassRuleSetup(dsmibClassEntryCtrl_t *pClass, 
                                     L7_uint32 aclNum,
                                     L7_uint32 ruleNum);
L7_RC_t diffServAclClassRuleBuild(dsmibClassEntryCtrl_t *pClass, 
                                  dsmibClassAclType_t aclType,
                                  L7_uint32 aclNum,
                                  L7_uint32 ruleNum,
                                  dsmibClassRuleType_t entryType,
                                  dsmibTruthValue_t excludeFlag,
                                  void *pVal,
                                  void *pVal2);

/* diffserv_util.c */
void    diffServSemaTake(void *semId, L7_int32 timeout, char *file, L7_ulong32 line);
void    diffServSemaGive(void *semId, char *file, L7_ulong32 line);
void    diffServFeatureShow(void);

/* diffserv_stdmib.c */
L7_RC_t diffServStandardMibInit(void);
void    diffServStandardMibFini(void);
L7_RC_t diffServStandardMibReset(void);
L7_RC_t diffServDataPathCreate(L7_uint32 intIfNum,                            
                          L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_RC_t diffServDataPathDelete(L7_uint32 intIfNum,                            
                          L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

#endif /* INCLUDE_DIFFSERV_UTIL_H */
