/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_cfg.h
*
* @purpose   Multiple Spanning tree configuration header
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_CFG_H
#define INCLUDE_DOT1S_CFG_H

#include "dot1s_exports.h"
#include "dot1s_db.h"
#include "nimapi.h"


#define DOT1S_CFG_FILENAME     "dot1s.cfg"
#define DOT1S_CFG_VER_1        0x1
#define DOT1S_CFG_VER_2        0x2
#define DOT1S_CFG_VER_3        0x3
#define DOT1S_CFG_VER_4        0x4
#define DOT1S_CFG_VER_5        0x5
#define DOT1S_CFG_VER_CURRENT  DOT1S_CFG_VER_5

#define DOT1S_DEBUG_CFG_FILENAME "dot1s_debug.cfg"
#define DOT1S_DEBUG_CFG_VER_1          0x1
#define DOT1S_DEBUG_CFG_VER_CURRENT    DOT1S_DEBUG_CFG_VER_1

typedef enum
{
  DOT1S_PHASE_INIT_0 = 0,
  DOT1S_PHASE_INIT_1,
  DOT1S_PHASE_INIT_2,
  DOT1S_PHASE_WMU,
  DOT1S_PHASE_INIT_3,
  DOT1S_PHASE_EXECUTE,
  DOT1S_PHASE_UNCONFIG_1,
  DOT1S_PHASE_UNCONFIG_2,
} dot1sCnfgrState_t;

#define DOT1S_IS_READY (((dot1sCnfgrState == DOT1S_PHASE_WMU) ||        \
                         (dot1sCnfgrState == DOT1S_PHASE_INIT_3) ||     \
                         (dot1sCnfgrState == DOT1S_PHASE_EXECUTE) ||    \
                         (dot1sCnfgrState == DOT1S_PHASE_UNCONFIG_1) || \
						 (dot1sCnfgrState == DOT1S_PHASE_UNCONFIG_2)) ? (L7_TRUE) : (L7_FALSE))

typedef struct portPerInstCfg_s
{
  L7_uint32 ExternalPortPathCost;
  L7_uint32 InternalPortPathCost;
  L7_BOOL autoInternalPortPathCost;
  L7_BOOL autoExternalPortPathCost;
  DOT1S_PORTID_t portPriority;
} DOT1S_PORT_PER_INST_CFG_t;

typedef struct portCommon_s
{
  nimConfigID_t configId;
  L7_uint32 adminEdge;
  L7_uint32 autoEdge;
  L7_uint32 restrictedRole;
  L7_uint32 loopGuard;  
  L7_uint32 restrictedTcn;

  L7_uint32 portAdminMode;
  L7_uint32 HelloTime;
  DOT1S_PORT_PER_INST_CFG_t portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES + 1];
  L7_BOOL  bpduFilterMode; /* Enable/Disable for bpdu filter Mode */ 
  L7_uint32  bpduFloodMode; /* Enable/Disable for bpdu flood Mode */ 
} DOT1S_PORT_COMMON_CFG_t;

typedef struct dot1s_mst_config_id_cfg_s
{
  L7_uchar8 pad[1];
  L7_uchar8 formatSelector;
  L7_uchar8 configName[DOT1S_MAX_CONFIG_NAME_SIZE];
  L7_ushort16 revLevel;

} DOT1S_MST_CONFIG_ID_CFG_t;

typedef struct dot1s_bridge_cfg_s
{
  L7_BOOL Mode;
  DOT1S_FORCE_VERSION_t ForceVersion;
  L7_uint32 FwdDelay;
  L7_uint32 TxHoldCount;
  L7_uint32 MigrateTime;        /*in secs */
  L7_uint32 MaxHops;
  L7_uint32 instanceCount;
  DOT1S_MST_CONFIG_ID_CFG_t MstConfigId;
  L7_BOOL bpduGuardMode; /* Enable/Disbale for BPDU Guard Mode */  
  L7_BOOL bpduFilterMode; /* Enable/Disbale for BPDU Filter Mode */  
} DOT1S_BRIDGE_CFG_t;

/*parms that are for the cist alone*/
typedef struct dot1s_cist_cfg_s
{
  L7_uint32 cistid;             /*should be zero */
  DOT1S_BRIDGEID_t BridgeIdentifier;
  DOT1S_CIST_BRIDGE_TIMES_t CistBridgeTimes;
} DOT1S_CIST_CFG_t;

/*parms that are for mstis only NOT for cist-- do not use index 0 if this typedef is to be an array*/
typedef struct dot1s_msti_cfg_s
{
  L7_uint32 mstid;
  DOT1S_BRIDGEID_t BridgeIdentifier;
  DOT1S_MSTI_BRIDGE_TIMES_t MstiBridgeTimes;
} DOT1S_MSTI_CFG_t;

/*parms specific to an instance of STP including CIST*/
typedef struct dot1s_instance_cfg_s
{
  DOT1S_CIST_CFG_t cist;
  DOT1S_MSTI_CFG_t msti[L7_MAX_MULTIPLE_STP_INSTANCES + 1];     /*do not use index 0 */
} DOT1S_INSTANCE_INFO_CFG_t;

typedef struct
{
  DOT1S_BRIDGE_CFG_t dot1sBridge;
  DOT1S_INSTANCE_INFO_CFG_t dot1sInstance;
  DOT1S_PORT_COMMON_CFG_t dot1sCfgPort[L7_DOT1S_MAX_INTERFACE_COUNT];
  DOT1S_INSTANCE_MAP_t dot1sInstanceMap[L7_MAX_MULTIPLE_STP_INSTANCES + 1];
  DOT1S_INST_VLAN_MAP_t dot1sInstVlanMap[L7_MAX_VLAN_ID + 2];
} dot1sCfgData_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1sCfgData_t cfg;
  L7_uint32 checkSum;
} dot1sCfg_t;

typedef struct
{
  L7_BOOL dot1sSave;
  L7_BOOL dot1sRestore;
  L7_BOOL dot1sHasDataChanged;
  L7_BOOL dot1sCfgDump;
  L7_BOOL dot1sIntfChangeCallback;
  L7_BOOL dot1sVlanChangeCallback;
  L7_BOOL dot1sSysnetBpduRegister;
  L7_BOOL dot1sInstStateQueryCallback;
  L7_BOOL dot1sStateSetResponseCallback;
} dot1sDeregister_t;

typedef struct dot1sDebugCfgData_s 
{
  L7_BOOL dot1sDebugPacketTraceTxFlag;
  L7_BOOL dot1sDebugPacketTraceRxFlag;
} dot1sDebugCfgData_t;

typedef struct dot1sDebugCfg_s
{
  L7_fileHdr_t          hdr;
  dot1sDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} dot1sDebugCfg_t;


/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t dot1sSave (void);
L7_RC_t dot1sRestore (void);
L7_BOOL dot1sHasDataChanged (void);
void dot1sResetDataChanged(void);
void dot1sConfiguredDataRemove ();
void dot1sPortDefaultConfigDataBuild (DOT1S_PORT_COMMON_CFG_t * portCfg);

/*********************************************************************
* @purpose  Build default dot1s config data
*
* @param    ver @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sBuildConfigData (L7_uint32 ver);

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void dot1sMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
L7_RC_t dot1sApplyConfigData (void);
L7_RC_t dot1sCopyConfigData ();
L7_RC_t dot1sInstanceDefaultPopulate (DOT1S_INSTANCE_INFO_t * Instance,
                                      L7_uint32 instIndex, L7_uint32 instId);
L7_RC_t dot1sInstanceMacAddrPopulate (DOT1S_INSTANCE_INFO_t * instance,
                                      L7_uint32 instIndex);
L7_RC_t dot1sPortDefaultPopulate (DOT1S_PORT_COMMON_t * pPort,
                                  L7_uint32 intIfNum);
L7_RC_t dot1sPortInstInfoDefaultPopulate (DOT1S_PORT_COMMON_t * pPort,
                                          L7_uint32 instIndex,
                                          L7_uint32 infIfNum);
L7_RC_t dot1sPortInstInfoMacAddrPopulate (DOT1S_PORT_COMMON_t * pPort,
                                          L7_uint32 instIndex);
L7_RC_t dot1sCfgUpdate (void);
L7_RC_t dot1sStatsCreate (L7_uint32 intIfNum);
void dot1sStatGet (void * statPtr);
L7_RC_t dot1sMstConfigIdCompute ();
L7_RC_t dot1sLagIntfInit (L7_uint32 intIfNum);
void dot1sInstVlanMapEndianSafeMake (DOT1S_INST_VLAN_MAP_t *
                                     dot1sInstVlanMapInput);
L7_RC_t dot1sInit (L7_CNFGR_CMD_DATA_t * pCmdData);
void dot1sInitUndo ();


/*********************************************************************
* @purpose  Saves dot1s configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1sDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot1sDebugSave(void);

/*********************************************************************
* @purpose  Restores dot1s debug configuration
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1sDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot1sDebugRestore(void);

/*********************************************************************
* @purpose  Checks if dot1s debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot1sDebugHasDataChanged(void);

/*********************************************************************
* @purpose  Build default dot1s config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot1sDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply dot1s debug config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot1sApplyDebugConfigData(void);



#endif /*INCLUDE_DOT1S_CFG_H */
