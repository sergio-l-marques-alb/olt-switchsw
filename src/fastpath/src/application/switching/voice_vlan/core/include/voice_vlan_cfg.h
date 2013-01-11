/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  voice_vlan_cfg.h
*
* @purpose   voice vlan configuration include file
*
* @component voice vlan
*
* @comments  none
*
* @create    
*
* @author   
*
* @end
*             
**********************************************************************/

#ifndef INCLUDE_VOICE_VLAN_CFG_H
#define INCLUDE_VOICE_VLAN_CFG_H

#include "nimapi.h"
#include "default_cnfgr.h"
                                 
#define VOICE_VLAN_CFG_FILENAME     "voicevlan.cfg"
#define VOICE_VLAN_CFG_VER_1        0x1

#define VOICE_VLAN_CFG_VER_CURRENT  VOICE_VLAN_CFG_VER_1

#define VOICE_VLAN_READY (((voiceVlanPortCnfgrState == VOICE_VLAN_INIT_3) || \
                        (voiceVlanPortCnfgrState == VOICE_VLAN_EXECUTE) || \
                        (voiceVlanPortCnfgrState == VOICE_VLAN_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))
                        
typedef enum {
  VOICE_VLAN_INIT_0 = 0,
  VOICE_VLAN_INIT_1,
  VOICE_VLAN_INIT_2,
  VOICE_VLAN_WMU,
  VOICE_VLAN_INIT_3,
  VOICE_VLAN_EXECUTE,
  VOICE_VLAN_UNCONFIG_1,
  VOICE_VLAN_UNCONFIG_2,
} voiceVlanPortCnfgrState_t;


typedef struct voiceVlanPortCfg_s
{
  nimConfigID_t configId;   
  L7_uint32	    adminMode;
  L7_uint32	    voiceVlanId;
  L7_uint32	    portPriority;		
  L7_uint32	    noneMode;		
  L7_BOOL	    untaggingEnabled;
  L7_BOOL	    defaultCosOverride;
  L7_BOOL       authState;
  L7_uint32     dscp;
} voiceVlanPortCfg_t;

typedef struct voiceVlanCfg_s
{
  L7_fileHdr_t          cfgHdr;
  L7_uint32              voiceVlanAdminMode;
  L7_uint32              voiceVlanTraceMode;
  voiceVlanPortCfg_t      portCfg[L7_VOICE_VLAN_INTF_MAX_COUNT];
  L7_uint32              checkSum;        
} voiceVlanCfg_t;

L7_RC_t voiceVlanSave(void);
L7_RC_t voiceVlanRestore(void);
L7_BOOL voiceVlanHasDataChanged(void);
void voiceVlanResetDataChanged(void);
L7_RC_t voiceVlanCfgDump(void);
void voiceVlanBuildDefaultConfigData(L7_uint32 ver);
void voiceVlanBuildDefaultIntfConfigData(nimConfigID_t *configId, voiceVlanPortCfg_t *pCfg);
L7_RC_t voiceVlanApplyConfigData(void);
L7_RC_t voiceVlanApplyPortConfigData(L7_uint32 intIfNum);
voiceVlanCfg_t *voiceVlanGetCfg(void);

  
#endif /* INCLUDE_VOICE_VLAN_CFG_H */



