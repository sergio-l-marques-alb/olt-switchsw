/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_config.h
*
* @purpose Contains IPv6 Provisioning config data structures and prototypes
*
* @component ipv6_provisioning
*
* @comments 
*
* @create 09/02/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/

#ifndef INCLUDE_IPV6_PROVISIONING_CONFIG_H
#define INCLUDE_IPV6_PROVISIONING_CONFIG_H

#define L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT  (L7_MAX_INTERFACE_COUNT + 1)

typedef struct 
{
  nimConfigID_t configId;   /* NIM config ID for this interface (src interface for IPv6 frames) */
  L7_uint32     ipv6Mode;
  nimConfigID_t ipv6DstId;  /* NIM config ID for the destination interface */
  L7_uint32     ipv6copyToCpu; /* To indicate whether packets should be copied
                                  to cpu when bridging between ports */
} ipv6PortInfo_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr; 
  ipv6PortInfo_t ipv6PortMappings[L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT];
  L7_uint32      checkSum;    
} ipv6ProvCfgData_t;

#define IPV6_PROVISIONING_CFG_FILENAME     "ipv6_provisioning.cfg"
#define IPV6_PROVISIONING_CFG_VER_1        0x1
#define IPV6_PROVISIONING_CFG_VER_2        0x2
#define IPV6_PROVISIONING_CFG_VER_3        0x3
#define IPV6_PROVISIONING_CFG_VER_CURRENT  IPV6_PROVISIONING_CFG_VER_3

#define IPV6_PROVISIONING_IS_READY (((ipv6ProvCnfgrState == IPV6_PROVISIONING_PHASE_INIT_3) || \
                                   (ipv6ProvCnfgrState == IPV6_PROVISIONING_PHASE_EXECUTE) || \
                                   (ipv6ProvCnfgrState == IPV6_PROVISIONING_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

typedef enum {
  IPV6_PROVISIONING_PHASE_INIT_0 = 0,
  IPV6_PROVISIONING_PHASE_INIT_1,
  IPV6_PROVISIONING_PHASE_INIT_2,
  IPV6_PROVISIONING_PHASE_WMU,
  IPV6_PROVISIONING_PHASE_INIT_3,
  IPV6_PROVISIONING_PHASE_EXECUTE,
  IPV6_PROVISIONING_PHASE_UNCONFIG_1,
  IPV6_PROVISIONING_PHASE_UNCONFIG_2,
} ipv6ProvCnfgrState_t;


void ipv6ProvBuildDefaultConfigData(L7_uint32 ver);
void ipv6ProvBuildDefaultIntfConfigData(nimConfigID_t *configId, ipv6PortInfo_t *pCfg);
L7_RC_t ipv6ProvApplyConfigData(void);
L7_RC_t ipv6ProvApplyIntfConfigData(L7_uint32 intIfNum);
L7_BOOL ipv6ProvHasDataChanged(void);
void ipv6ProvResetDataChanged(void);
L7_RC_t ipv6ProvSave(void);
L7_RC_t ipv6ProvRecvPkt(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
L7_RC_t ipv6ProvIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_BOOL ipv6ProvIntfIsConfigurable(L7_uint32 intIfNum, ipv6PortInfo_t **pCfg);
L7_BOOL ipv6ProvIntfConfigEntryGet(L7_uint32 intIfNum, ipv6PortInfo_t **pCfg);
L7_RC_t ipv6ProvIntfCreate(L7_uint32 intIfNum);
L7_RC_t ipv6ProvIntfDetach(L7_uint32 intIfNum);
L7_RC_t ipv6ProvIntfDelete(L7_uint32 intIfNum);
L7_RC_t ipv6ProvCfgDump(void);

void ipv6ProvApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t ipv6ProvCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t ipv6ProvCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );
L7_RC_t ipv6ProvCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason );
void ipv6ProvCnfgrFiniPhase1Process();
void ipv6ProvCnfgrFiniPhase2Process();
void ipv6ProvCnfgrFiniPhase3Process();
L7_RC_t ipv6ProvCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t ipv6ProvCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason);

/* ipv6_provisioning_migrate.c */
void ipv6ProvMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* INCLUDE_IPV6_PROVISIONING_CONFIG_H */
