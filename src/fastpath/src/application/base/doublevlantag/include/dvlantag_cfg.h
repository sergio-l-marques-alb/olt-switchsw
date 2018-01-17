/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_cfg.h
*
* @purpose   Double Vlan Tagging configuration header
*
* @component DvlanTag
*
* @comments
*
* @create    8/14/2003
*
* @author    skalyanam
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_DTAG_CFG_H
#define INCLUDE_DTAG_CFG_H

#define L7_DVLANTAG_MAX_INTERFACE_COUNT  (L7_MAX_INTERFACE_COUNT + 1)

/* Configuration data */
typedef struct dvlantag_intf_cfg_s
{
  nimConfigID_t   configId;   /* NIM config ID for this interface*/
  L7_uint32       mode;
  L7_ushort16     etherType[L7_DVLANTAG_MAX_TPIDS];
  L7_uint32       etherValMask;
  L7_ushort16     custId;
} DVLANTAG_INTF_CFG_t;

/* Operational data */
typedef struct dvlantag_port_s
{
  L7_uchar8     etherTypeMask;
} DVLANTAG_PORT_t;

typedef struct  dvlantag_cfg_s
{
  L7_fileHdr_t         cfgHdr;
  DVLANTAG_INTF_CFG_t  intfCfg[L7_DVLANTAG_MAX_INTERFACE_COUNT];
  L7_ushort16          etherType[L7_DVLANTAG_MAX_TPIDS];  
  L7_uint32            checkSum;      /* check sum of config file NOTE: needs to be last entry */
} DVLANTAG_CFG_t;

#define DVLANTAG_CFG_FILENAME     "dvlantag.cfg"
#define DVLANTAG_CFG_VER_1        0x1
#define DVLANTAG_CFG_VER_2        0x2
#define DVLANTAG_CFG_VER_3        0x3
#define DVLANTAG_CFG_VER_4        0x4
#define DVLANTAG_CFG_VER_CURRENT  DVLANTAG_CFG_VER_4

#define DVLANTAG_IS_READY (((dvlantagCnfgrState == DVLANTAG_PHASE_WMU) || \
                            (dvlantagCnfgrState == DVLANTAG_PHASE_INIT_3) || \
                            (dvlantagCnfgrState == DVLANTAG_PHASE_EXECUTE) || \
                            (dvlantagCnfgrState == DVLANTAG_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

typedef enum {
  DVLANTAG_PHASE_INIT_0 = 0,
  DVLANTAG_PHASE_INIT_1,
  DVLANTAG_PHASE_INIT_2,
  DVLANTAG_PHASE_WMU,
  DVLANTAG_PHASE_INIT_3,
  DVLANTAG_PHASE_EXECUTE,
  DVLANTAG_PHASE_UNCONFIG_1,
  DVLANTAG_PHASE_UNCONFIG_2,
} dvlantagCnfgrState_t;

L7_RC_t dtagMemoryMalloc();
void dtagMemoryFree();

void dvlantagBuildDefaultConfigData(L7_uint32 ver);
void dvlantagBuildDefaultIntfConfigData(nimConfigID_t *configId, DVLANTAG_INTF_CFG_t *pCfg);
L7_RC_t dvlantagCfgDump();
L7_BOOL dvlantagHasDataChanged();
void dvlantagResetDataChanged(void);
L7_RC_t dvlantagSave();
L7_RC_t dvlantagApplyConfigData();
L7_RC_t dvlantagApplyIntfConfigData(L7_uint32 intIfNum);
void dvlantagCfgRemove();

void dvlantagApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t dvlantagCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t dvlantagCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );
L7_RC_t dvlantagCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );
void dvlantagCnfgrFiniPhase1Process();
void dvlantagCnfgrFiniPhase2Process();
void dvlantagCnfgrFiniPhase3Process();
L7_RC_t dvlantagCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t dvlantagCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);


/* dvlantag_migrate.c */
void dvlantagMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);


#endif/* INCLUDE_DTAG_CFG_H */

