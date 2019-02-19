/*
 * ptinHapi_cfg.h
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 */

#ifndef PTINHAPI_CFG_H_
#define PTINHAPI_CFG_H_

#define PTINHAPI_DEBUG_CFG_VER_CURRENT  0
#define PTINHAPI_DEBUG_CFG_FILENAME "ptinHapi_debug.cfg"

typedef enum
{
  PTINHAPI_PHASE_INIT_0 = 0,
  PTINHAPI_PHASE_INIT_1,
  PTINHAPI_PHASE_INIT_2,
  PTINHAPI_PHASE_WMU,
  PTINHAPI_PHASE_INIT_3,
  PTINHAPI_PHASE_EXECUTE,
  PTINHAPI_PHASE_UNCONFIG_1,
  PTINHAPI_PHASE_UNCONFIG_2,
} ptinHapiCnfgrState_t;

typedef struct
{
   L7_uint32 dummy;
} ptinHapiCfgData_t;


typedef struct
{
  L7_fileHdr_t hdr;
  ptinHapiCfgData_t cfg;
  L7_uint32 checkSum;
} ptinHapiCfg_t;



typedef struct
{
  L7_BOOL ptinHapiSave;
  L7_BOOL ptinHapiRestore;
  L7_BOOL ptinHapiHasDataChanged;
  L7_BOOL ptinHapiCfgDump;
  L7_BOOL ptinHapiIntfChangeCallback;
  L7_BOOL ptinHapiVlanChangeCallback;
  L7_BOOL ptinHapiSysnetBpduRegister;
  L7_BOOL ptinHapiInstStateQueryCallback;
  L7_BOOL ptinHapiStateSetResponseCallback;
} ptinHapiDeregister_t;

typedef struct ptinHapiDebugCfgData_s
{
  L7_BOOL dot1sDebugPacketTraceTxFlag;
  L7_BOOL dot1sDebugPacketTraceRxFlag;
} ptinHapiDebugCfgData_t;

typedef struct ptinHapiDebugCfg_s
{
  L7_fileHdr_t          hdr;
  ptinHapiDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} ptinHapiDebugCfg_t;


/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************/
L7_RC_t ptinHapiDebugSave(void);
L7_BOOL ptinHapiDebugHasDataChanged(void);
L7_RC_t ptinHapiDebugRestore(void);
L7_RC_t ptinHapiApplyDebugConfigData(void);
void ptinHapiDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t ptinHapiSave(void);
L7_BOOL ptinHapiHasDataChanged(void);
void ptinHapiResetDataChanged(void);
L7_BOOL ptinHapiDebugHasDataChanged(void);
L7_RC_t ptinHapiCopyConfigData(void);


int ptinHapiconfigDebugTrace (st_DebugParams *input, int profileID);

#endif /* PTINHAPI_CFG_H_ */
