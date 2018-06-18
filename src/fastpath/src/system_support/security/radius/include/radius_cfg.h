/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_cfg.h
*
* @purpose RADIUS Client configuration header
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_RADIUS_CFG_H
#define INCLUDE_RADIUS_CFG_H

#include "l7_cnfgr_api.h"

#define RADIUS_CFG_FILENAME    "radius.cfg"
#define RADIUS_CFG_VER_1       0x1
#define RADIUS_CFG_VER_2       0x2
#define RADIUS_CFG_VER_3       0x3
#define RADIUS_CFG_VER_CURRENT RADIUS_CFG_VER_3

#define RADIUS_LG_BUF_LABEL "RADIUS Lg Bufs"
#define RADIUS_LG_BUF_SIZE RADIUS_MAX_MESSAGE_LENGTH + L7_RADIUS_MAX_SECRET

#define RADIUS_SM_BUF_LABEL "RADIUS Sm Bufs"
#define RADIUS_SM_BUF_SIZE (max(\
                           max(\
                                max(sizeof(radiusValuePair_t),\
                                    sizeof(radiusRequestInfo_t)),\
                                max(sizeof(radiusDictValue_t),\
                                    sizeof(radiusServerEntry_t))),\
                           max(\
                                max(sizeof(radiusRequest_t),\
                                    sizeof(radiusDictAttr_t)),\
                                RADIUS_MAX_TOKEN_SIZE)))

typedef struct
{
  radiusClient_t radiusClient;

} radiusCfgData_t;

typedef struct
{
  L7_fileHdr_t    hdr;
  radiusCfgData_t cfg;
  L7_uint32       checkSum;

} radiusCfg_t;

typedef enum {
  RADIUS_PHASE_INIT_0 = 0,
  RADIUS_PHASE_INIT_1,
  RADIUS_PHASE_INIT_2,
  RADIUS_PHASE_WMU,
  RADIUS_PHASE_INIT_3,
  RADIUS_PHASE_EXECUTE,
  RADIUS_PHASE_UNCONFIG_1,
  RADIUS_PHASE_UNCONFIG_2,
} radiusCnfgrState_t;

/*
** Internal function prototypes
*/

L7_RC_t radiusSave(void);

L7_RC_t radiusRestore(void);

L7_BOOL radiusHasDataChanged(void);
void radiusResetDataChanged(void);
void radiusConfiguredDataRemove(void);

void radiusBuildConfigData(L7_uint32 ver);

void radiusApplyConfigData(void);

void radiusCfgUpdate(void);

L7_RC_t radiusInit(L7_CNFGR_CMD_DATA_t *pCmdData);

void radiusInitUndo();

void radiusCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData);

L7_RC_t radiusCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

L7_RC_t radiusCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

L7_RC_t radiusCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason );

void radiusCnfgrFiniPhase1Process();

void radiusCnfgrFiniPhase2Process();

void radiusCnfgrFiniPhase3Process();

L7_RC_t radiusCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse, 
                               L7_CNFGR_ERR_RC_t *pReason );

L7_RC_t radiusCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t *pReason );

L7_RC_t radiusBufferPoolInit(void);


/* radius_migrate.c */

void radiusMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* INCLUDE_RADIUS_CFG_H */

