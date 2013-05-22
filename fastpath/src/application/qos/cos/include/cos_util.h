/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_util.h
*
* @purpose    COS component utility function prototypes
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_COS_UTIL_H
#define INCLUDE_COS_UTIL_H

#include "l7_common.h"
#include "l7_cos_api.h" 
#include "cos_config.h"
#include "usmdb_qos_cos_api.h"
#include "l7_cnfgr_api.h"
    
/* printable strings used for COS debug functions */
extern char *cosMapIntfModeStr[];
extern char *cosQueueSchedTypeStr[];
extern char *cosQueueMgmtTypeStr[];
extern char *cosFeatureString[]; 

/*---------------------------------*/
/*  COS INFO STRUCTURES            */
/*---------------------------------*/

typedef struct cosInfo_s     
{
  L7_uint32       msgLvl;             /* debug message level (operational) */

  /* operational information indicated by the dot1p component
   *
   * NOTE:  Dot1p uses [L7_MAX_INTERFACE_COUNT+1] for its per-interface arrays.
   */

  /* active number of traffic classes for COS mapping */
  L7_uchar8       globalNumTrafficClasses;
  L7_uchar8       numTrafficClasses[L7_MAX_INTERFACE_COUNT+1];

  /* port default priority value COS mapping */
  L7_uchar8       globalPortDefaultPriority;
  L7_uchar8       globalPortDefaultTrafficClass;
  L7_uchar8       portDefaultPriority[L7_MAX_INTERFACE_COUNT+1];
  L7_uchar8       portDefaultTrafficClass[L7_MAX_INTERFACE_COUNT+1];

} cosInfo_t;

extern cosInfo_t    *pCosInfo_g;
      
typedef struct cosIntfInfo_s
{
  L7_BOOL           intfIsActive;       /* indicates whether L7_ACTIVE was seen */
} cosIntfInfo_t;


/****************************************
*
*  DEBUGGING MACROS, FUNCTION PROTOTYPES
*
*****************************************/

/* Debug message levels */
#define COS_MSGLVL_ON    0              /* use where printf is desired */
#define COS_MSGLVL_HI    1
#define COS_MSGLVL_MED   50
#define COS_MSGLVL_LO    100
#define COS_MSGLVL_OFF   10000          /* some arbitrarily large value */

#define COS_PRT(mlvl, fmt, args...) \
        { \
          if (pCosInfo_g->msgLvl >= (mlvl)) \
          { \
            sysapiPrintf(fmt, ##args); \
          } \
        }


/* special-use structures */

typedef struct
{
  L7_qosCosQueueBwList_t          minBwList;
  L7_qosCosQueueBwList_t          maxBwList;
  L7_qosCosQueueSchedTypeList_t   schedTypeList;

} L7_cosQueueSchedParms_t;
      

/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* cos_cnfgr.c */
L7_RC_t cosCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t cosCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t cosCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason);
void cosCnfgrFiniPhase1Process(void);
void cosCnfgrFiniPhase2Process(void);
void cosCnfgrFiniPhase3Process(void);
L7_RC_t cosCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason);
L7_RC_t cosCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason);

/* cos_config.c */
L7_RC_t cosMapTableInitLookup(L7_uint32 numTrafficClasses, L7_uint32 precLevel,
                              L7_uint32 *pVal);
void cosBuildDefaultConfigData(L7_uint32 ver);
void cosBuildDefaultIntfConfigData(nimConfigID_t *configId, L7_cosCfgIntfParms_t *pCfg);
void cosDefaultMappingConfigBuild(L7_uint32 intIfNum, L7_cosMapCfg_t *pCfgMap);
L7_RC_t cosDefaultMappingIpPrecGet(L7_uint32 intIfNum, L7_uint32 prec,
                                   L7_uint32 *pVal);
L7_RC_t cosDefaultMappingIpDscpGet(L7_uint32 intIfNum, L7_uint32 dscp,
                                   L7_uint32 *pVal);
void cosDefaultIntfConfigBuild(L7_cosIntfCfg_t *pCfgIntf);
void cosDefaultQueueConfigAllBuild(L7_cosQueueCfg_t *pCfgQ);
void cosDefaultQueueConfigBuild(L7_cosQueueCfg_t *pCfgQ, L7_uint32 queueId);
void cosResetDefaultIntfConfigData(L7_uint32 intIfNum, 
                                   nimConfigID_t *pConfigId, 
                                   L7_cosCfgIntfParms_t *pCfgIntf);
L7_RC_t cosConfigDataApply(void);
L7_RC_t cosConfigIntfDataApply(L7_uint32 intIfNum, L7_cosCfgIntfParms_t *pCfgIntf);
L7_RC_t cosConfigIntfMapTableDataApply(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg, 
                                       L7_BOOL forceDtl);
L7_RC_t cosConfigIntfQueueCfgDataApply(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg);
L7_RC_t cosConfigIntfDataUnapply(L7_uint32 intIfNum, L7_cosCfgIntfParms_t *pCfgIntf);
L7_RC_t cosSave(void);
L7_RC_t cosRestore(void);
L7_BOOL cosHasDataChanged(void);
void cosResetDataChanged(void);
L7_BOOL cosIntfIsConfigurable(L7_uint32 intIfNum, L7_cosCfgIntfParms_t **ppCfgIntf);
L7_BOOL cosIntfIsSettable(L7_uint32 intIfNum);
L7_BOOL cosIntfIsWriteable(L7_uint32 intIfNum, L7_uint32 featureId);
L7_BOOL cosIntfIsWriteableCond(L7_uint32 intIfNum, L7_BOOL globalOnly, L7_uint32 featureId);
L7_RC_t cosIntfMgmtUnitFirstFind(L7_uint32 *pIntIfNum);
L7_BOOL cosIntfConfigEntryGet(L7_uint32 intIfNum, L7_cosCfgIntfParms_t **ppCfgIntf);
L7_RC_t cosIntfCreate(L7_uint32 intIfNum);
L7_RC_t cosIntfDelete(L7_uint32 intIfNum);
L7_RC_t cosIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
void cosStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);
L7_BOOL cosIsIntfAcquired(L7_uint32 intIfNum);
void cosConfigDataShow(L7_uint32 intIfNum);
L7_RC_t cosConfigDataShowAll(void);
void cosConfigIntfOverrideShow(void);
void cosBuildTestIntfConfigData(L7_uint32 intIfNum, L7_cosCfgParms_t *pCfg);
void cosBuildTestConfigData(void);
void cosSaveTestConfigData(void);
void cosConfigDataTestShow(void);

/* cos_maptable_api.c */
L7_BOOL cosMapTableContentIsValid(L7_uint32 intIfNum, L7_cosMapCfg_t *pCfgMap);
void cosMapTableShow(L7_uint32 intIfNum);
void cosMapPortDefaultPriorityTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd);
void cosMapDot1pTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd);
void cosMapIpPrecTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd);
void cosMapIpDscpTableShow(L7_uint32 intIfNum, L7_uint32 msgLvlReqd);

/* cos_migrate.c */
void cosMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/* cos_queue_api.c */
L7_BOOL cosQueueMgmtTypeIsActive(L7_uint32 intIfNum, L7_uint32 queueId,
                                 L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtType);
void cosQueueConfigShow(L7_uint32 intIfNum);

/* cos_util.c */
L7_RC_t cosCfgPtrFind(L7_uint32 intIfNum, L7_cosCfgParms_t **ppCfg);
L7_BOOL cosIntfIsValid(L7_uint32 intIfNum);
L7_BOOL cosIntfTypeIsValid(L7_uint32 intIfNum);
L7_RC_t cosIntfIndexGet(L7_uint32 intIfNum);
L7_RC_t cosIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext);
L7_uint32 cosMsgLvlGet(void);
void cosMsgLvlSet(L7_uint32 msgLvl);
L7_RC_t cosMapIpPrecTrafficClassApply(L7_uint32 intIfNum, L7_uint32 prec,
                                      L7_uint32 val);
L7_RC_t cosMapIpPrecTableApply(L7_uint32 intIfNum, L7_uchar8 *pVal);
L7_RC_t cosMapIpDscpTrafficClassApply(L7_uint32 intIfNum, L7_uint32 dscp,
                                      L7_uint32 val);
L7_RC_t cosMapIpDscpTableApply(L7_uint32 intIfNum, L7_uchar8 *pVal);
L7_RC_t cosMapIntfTrustModeApply(L7_uint32 intIfNum, 
                                 L7_cosCfgParms_t *pCfg,
                                 L7_BOOL forceDtl);
L7_RC_t cosQueueIntfConfigApply(L7_uint32 intIfNum, 
                                L7_uint32 intfShapingRate,
                                L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtTypeIntf,
                                L7_uint32 wredDecayExp);
L7_RC_t cosIntfShapingStatusGet(L7_uint32 intIfNum, 
                                L7_uint32 *intfShapingRate,
                                L7_uint32 *intfShapingBurstSize);
L7_RC_t cosQueueSchedConfigApply(L7_uint32 intIfNum, 
                                 L7_cosQueueSchedParms_t *pQParms);
L7_RC_t cosQueueDropParmsApply(L7_uint32 intIfNum, 
                               L7_qosCosDropParmsList_t *pVal);
L7_RC_t cosQueueDropParmsValidate(L7_qosCosDropParmsList_t *pVal);
L7_BOOL cosQueueDropParmsDiffer(L7_qosCosDropParmsList_t *pVal, L7_cosCfgParms_t *pCfg);
void cosQueueDropConfigUpdate(L7_qosCosDropParmsList_t *pVal, L7_cosCfgParms_t *pCfg);
void cosDebugHelp(void);
void cosFeatureShow(void);
void cosInfoShow(void);

#endif /* INCLUDE_COS_UTIL_H */
