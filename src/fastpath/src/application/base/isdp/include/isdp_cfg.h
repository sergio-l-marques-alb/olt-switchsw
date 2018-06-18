/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_cfg.h
*
* @purpose   ISDP configuration header
*
* @component isdp
*
* @comments
*
* @create    8/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_CFG_H
#define ISDP_CFG_H

#define ISDP_CFG_FILENAME          "isdp.cfg"
#define ISDP_CFG_VER_1             0x1
#define ISDP_CFG_VER_CURRENT       ISDP_CFG_VER_1

#define ISDP_DEBUG_CFG_FILENAME    "isdp_debug.cfg"
#define ISDP_DEBUG_CFG_VER_1       0x1
#define ISDP_DEBUG_CFG_VER_CURRENT ISDP_DEBUG_CFG_VER_1

typedef struct isdpGlobalCfgData_s
{
  L7_uint32            mode;
  L7_uint32            timer;
  L7_uint32            holdTime;
  L7_uint32            v2Mode;
} isdpGlobalCfgData_t;

typedef struct isdpIntfCfgData_s
{
  nimConfigID_t        cfgId;
  L7_uint32            mode;
} isdpIntfCfgData_t;

typedef struct isdpCfgData_s
{
  isdpGlobalCfgData_t  globalCfgData;
  isdpIntfCfgData_t    intfCfgData[L7_ISDP_INTF_MAX_COUNT];
} isdpCfgData_t;

typedef struct isdpCfg_s
{
  L7_fileHdr_t hdr;
  isdpCfgData_t cfg;
  L7_uint32 checkSum;
} isdpCfg_t;

typedef struct isdpDebugCfgData_s
{
  L7_uint32 isdpDebugPacketTraceTxFlag;
  L7_uint32 isdpDebugPacketTraceRxFlag;
  L7_uint32 isdpDebugEventTraceFlag;
} isdpDebugCfgData_t;

typedef struct isdpDebugCfg_s
{
  L7_fileHdr_t       hdr;
  isdpDebugCfgData_t cfg;
  L7_uint32          checkSum;
} isdpDebugCfg_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t isdpSave (void);
L7_BOOL isdpHasDataChanged (void);
void isdpResetDataChanged (void);
L7_RC_t isdpInit (L7_CNFGR_CMD_DATA_t * pCmdData);
void isdpInitUndo ();
void isdpBuildDefaultConfigData(L7_uint32 ver);
void isdpBuildDefaultIntfConfigData(nimConfigID_t *cfgId,
    isdpIntfCfgData_t *pCfg);

#endif /*ISDP_CFG_H */
