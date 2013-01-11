/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename dot1ag_exports.h
*
* @purpose  Defines constants and feature definitions that are shared 
*           by Management and the application
*
* @component
*
* @comments
*
*
* @Notes
*
* @created
*
* @author
* @end
*
 **********************************************************************/

#ifndef __DOT1AG_EXPORTS_H_
#define __DOT1AG_EXPORTS_H_

#define L7_DOT1AG_INTF_MAX_COUNT         (L7_MAX_PORT_COUNT+L7_MAX_NUM_LAG_INTF+1)
#define L7_DOT1AG_MAINTENANCE_DOMANS_MAX               8
#define L7_DOT1AG_NUM_MA_PER_DOMAIN                    256
#define L7_DOT1AG_NUM_MEP_PER_MA                       8
#define L7_DOT1AG_MAX_MEPS                             L7_DOT1AG_MAINTENANCE_DOMANS_MAX * \
                                                       L7_DOT1AG_NUM_MA_PER_DOMAIN * \
                                                       L7_DOT1AG_NUM_MEP_PER_MA

#define L7_DOT1AG_MAX_MEP_COUNT                        (L7_DOT1AG_MAX_MEPS / 32)
#define L7_DOT1AG_MAX_MA_COUNT                         L7_DOT1AG_NUM_MA_PER_DOMAIN

#define DOT1AG_RMEP_ENTRIES_MAX                        L7_DOT1AG_MAX_MEP_COUNT
#define DOT1AG_LTR_ENTRIES_MAX                         256

#define L7_DOT1AG_MA_NAME_LEN_MIN                      1
#define L7_DOT1AG_MA_NAME_LEN                          45
#define L7_DOT1AG_MD_NAME_LEN_MIN                      1
#define L7_DOT1AG_MD_NAME_LEN                          43
#define L7_DOT1AG_MEPID_MIN                            1
#define L7_DOT1AG_MEPID_MAX                            8191
#define L7_DOT1AG_MD_LEVEL_MIN                         0
#define L7_DOT1AG_MD_LEVEL_MAX                         7
#define L7_DOT1AG_MD_LEVEL_COUNT                       L7_DOT1AG_MD_LEVEL_MAX+1
#define L7_DOT1AG_FNG_ALARM_TIME_MIN                   250
#define L7_DOT1AG_FNG_ALARM_TIME_MAX                   1000
#define L7_DOT1AG_FNG_ALARM_RESET_TIME_MIN             250
#define L7_DOT1AG_FNG_ALARM_RESET_TIME_MAX             1000

#define L7_DOT1AG_NUM_LBM_MIN                          1
#define L7_DOT1AG_NUM_LBM_MAX                          255
#define L7_DOT1AG_LBM_DEFAULT_LBM                      1
#define L7_DOT1AG_LBM_TIMEOUT_IN_SECS                  5

#define L7_DOT1AG_LTM_TTL_MIN                          1
#define L7_DOT1AG_LTM_TTL_MAX                          255
#define L7_DOT1AG_LTM_TIMEOUT_IN_SECS                  1
#define L7_DOT1AG_LTM_DEFAULT_TTL                      64

#define L7_DOT1AG_CCM_ARCHIVE_HOLD_TIME_MIN            1
#define L7_DOT1AG_CCM_ARCHIVE_HOLD_TIME_MAX            65535

#define L7_DOT1AG_MAX_VLANS                            L7_MAX_VLANS

#define L7_DOT1AG_VLANID_MIN                           1
#define L7_DOT1AG_VLANID_MAX                           L7_PLATFORM_MAX_VLAN_ID


/* Default.h */
#define FD_DOT1AG_ADMIN_MODE                           L7_DISABLE
#define FD_DOT1AG_ARCHIVE_HOLD_TIME                    600
#define FD_DOT1AG_FNG_ALARM_TIME                       250
#define FD_DOT1AG_FNG_ALARM_RESET_TIME                 1000
#define FD_DOT1AG_MEP_CCI_ENABLED_MODE                 L7_FALSE
#define FD_DOT1AG_MEP_ACTIVE_MODE                      L7_FALSE
#define FD_DOT1AG_MIP_MODE                             L7_DISABLE
#define FD_DOT1AG_FNG_LOWEST_ALARM_PRI                 1

#define L7_DOT1AG_LLDP_PORT_ID_LEN                     255

/* OPCODE For CFM Messages */
#define DOT1AG_EGRESS_ID_LEN                    8
#define DOT1AG_LLDP_CHASSIS_ID_LEN              255
#define DOT1AG_LLDP_PORT_ID_LEN                 255
#define DOT1AG_LTR_ORG_TLV_LEN                  1500

/* Sender ID TLV holder */
#define DOT1AG_TLV_FIELD_LEN_MAX_CHASSIS_ID        255
#define DOT1AG_TLV_FIELD_LEN_MAX_MGMT_ADDR_DOMAIN  255
#define DOT1AG_TLV_FIELD_LEN_MAX_MGMT_ADDR         32

typedef enum
{
L7_DOT1AG_CC_TRAP_CONFIG          = 0x01,
L7_DOT1AG_CC_TRAP_CROSS_CONNECT   = 0x02,
L7_DOT1AG_CC_TRAP_LOOP            = 0x04,
L7_DOT1AG_CC_TRAP_MEP_UP          = 0x08,
L7_DOT1AG_CC_TRAP_MEP_DOWN        = 0x10
}Do1agCcmSnmpTraps_t;

typedef enum {
/* 802.1ag clauses 12.14.6.1.3:e, 20.8.1 and 21.6.1.3 */
  DOT1AG_CCM_intervalInvalid = 0, /* No CCMs are sent (disabled). */
  DOT1AG_CCM_interval300Hz   = 1, /* CCMs are sent every 3 1/3 milliseconds (300Hz). */
  DOT1AG_CCM_interval10ms    = 2, /* CCMs are sent every 10 milliseconds. */
  DOT1AG_CCM_interval100ms   = 3, /* CCMs are sent every 100 milliseconds. */
  DOT1AG_CCM_interval1s      = 4, /* CCMs are sent every 1 second. */
  DOT1AG_CCM_interval10s     = 5, /* CCMs are sent every 10 seconds. */
  DOT1AG_CCM_interval1min    = 6, /* CCMs are sent every minute. */
  DOT1AG_CCM_interval10min   = 7

} Dot1agCfmCcmInterval_t;
#define L7_DOT1AG_CCM_INTERVAL_MIN                     DOT1AG_CCM_interval1s
#define L7_DOT1AG_CCM_INTERVAL_MAX                     DOT1AG_CCM_interval10min
#define FD_DOT1AG_CCM_INTERVAL                         DOT1AG_CCM_interval1s

typedef enum {
  DOT1AG_MP_DIRECTION_DOWN = 1,
  DOT1AG_MP_DIRECTION_UP   = 2

} Dot1agCfmMpDirection_t;

typedef enum 
{
  MEP_PARAM_dot1agCfmMepIdentifier = 0,              
  MEP_PARAM_dot1agCfmMepIfIndex,
  MEP_PARAM_dot1agCfmMepDirection,                  
  MEP_PARAM_dot1agCfmMepPrimaryVid,                 
  MEP_PARAM_dot1agCfmMepActive,                     
  MEP_PARAM_dot1agCfmMepFngState,                   
  MEP_PARAM_dot1agCfmMepCciEnabled,                 
  MEP_PARAM_dot1agCfmMepCcmLtmPriority,             
  MEP_PARAM_dot1agCfmMepMacAddress,                 
  MEP_PARAM_dot1agCfmMepLowPrDef,                   
  MEP_PARAM_dot1agCfmMepFngAlarmTime,               
  MEP_PARAM_dot1agCfmMepFngResetTime,               
  MEP_PARAM_dot1agCfmMepHighestPrDefect,            
  MEP_PARAM_dot1agCfmMepDefects,                    
  MEP_PARAM_dot1agCfmMepErrorCcmLastFailure,        
  MEP_PARAM_dot1agCfmMepXconCcmLastFailure,         
  MEP_PARAM_dot1agCfmMepCcmSequenceErrors,          
  MEP_PARAM_dot1agCfmMepCciSentCcms,                
  MEP_PARAM_dot1agCfmMepNextLbmTransId,             
  MEP_PARAM_dot1agCfmMepLbrIn,                      
  MEP_PARAM_dot1agCfmMepLbrInOutOfOrder,            
  MEP_PARAM_dot1agCfmMepLbrBadMsdu,                 
  MEP_PARAM_dot1agCfmMepLtmNextSeqNumber,           
  MEP_PARAM_dot1agCfmMepUnexpLtrIn,                 
  MEP_PARAM_dot1agCfmMepLbrOut,                     
  MEP_PARAM_dot1agCfmMepTransmitLbmStatus,          
  MEP_PARAM_dot1agCfmMepTransmitLbmDestMacAddress,  
  MEP_PARAM_dot1agCfmMepTransmitLbmDestMepId,       
  MEP_PARAM_dot1agCfmMepTransmitLbmDestIsMepId,     
  MEP_PARAM_dot1agCfmMepTransmitLbmMessages,        
  MEP_PARAM_dot1agCfmMepTransmitLbmDataTlv,         
  MEP_PARAM_dot1agCfmMepTransmitLbmVlanPriority,    
  MEP_PARAM_dot1agCfmMepTransmitLbmVlanDropEnable,  
  MEP_PARAM_dot1agCfmMepTransmitLbmResultOK,        
  MEP_PARAM_dot1agCfmMepTransmitLbmSeqNumber,       
  MEP_PARAM_dot1agCfmMepTransmitLtmStatus,          
  MEP_PARAM_dot1agCfmMepTransmitLtmFlags,           
  MEP_PARAM_dot1agCfmMepTransmitLtmTargetMacAddress,
  MEP_PARAM_dot1agCfmMepTransmitLtmTargetMepId,     
  MEP_PARAM_dot1agCfmMepTransmitLtmTargetIsMepId,   
  MEP_PARAM_dot1agCfmMepTransmitLtmTtl,             
  MEP_PARAM_dot1agCfmMepTransmitLtmResult,          
  MEP_PARAM_dot1agCfmMepTransmitLtmSeqNumber,       
  MEP_PARAM_dot1agCfmMepTransmitLtmEgressIdentifier

} Dot1agCfmMepEntry_t; 

typedef enum
{
  LTR_PARAM_dot1agCfmLtrSeqNumber,
  LTR_PARAM_dot1agCfmLtrReceiveOrder,
  LTR_PARAM_dot1agCfmLtrTtl,
  LTR_PARAM_dot1agCfmLtrForwarded,
  LTR_PARAM_dot1agCfmLtrTerminalMep,         
  LTR_PARAM_dot1agCfmLtrLastEgressIdentifier,
  LTR_PARAM_dot1agCfmLtrNextEgressIdentifier,
  LTR_PARAM_dot1agCfmLtrRelay,
  LTR_PARAM_dot1agCfmLtrChassisIdLen,
  LTR_PARAM_dot1agCfmLtrChassisIdSubtype,
  LTR_PARAM_dot1agCfmLtrChassisId,
  LTR_PARAM_dot1agCfmLtrManAddressDomainLen,
  LTR_PARAM_dot1agCfmLtrManAddressDomain,
  LTR_PARAM_dot1agCfmLtrManAddressLen,
  LTR_PARAM_dot1agCfmLtrManAddress,
  LTR_PARAM_dot1agCfmLtrIngress,
  LTR_PARAM_dot1agCfmLtrIngressMac,
  LTR_PARAM_dot1agCfmLtrIngressPortIdLen,
  LTR_PARAM_dot1agCfmLtrIngressPortIdSubtype,
  LTR_PARAM_dot1agCfmLtrIngressPortId,
  LTR_PARAM_dot1agCfmLtrEgress,
  LTR_PARAM_dot1agCfmLtrEgressMac,
  LTR_PARAM_dot1agCfmLtrEgressPortIdLen,
  LTR_PARAM_dot1agCfmLtrEgressPortIdSubtype,
  LTR_PARAM_dot1agCfmLtrEgressPortId,		
  LTR_PARAM_dot1agCfmLtrOrganizationSpecificTlv
 
} Dot1agCfmLtrEntry_t;

typedef enum
{
   RMEP_PARAM_dot1agCfmMepDbRMepState,       
   RMEP_PARAM_dot1agCfmMepDbRMepFailedOkTime,
   RMEP_PARAM_dot1agCfmMepDbMacAddress,     
   RMEP_PARAM_dot1agCfmMepDbRdi,
   RMEP_PARAM_dot1agCfmMepDbPortStatusTlv,
   RMEP_PARAM_dot1agCfmMepDbInterfaceStatusTlv,
   RMEP_PARAM_dot1agCfmMepDbChassisIdLen,   
   RMEP_PARAM_dot1agCfmMepDbChassisIdSubtype,   
   RMEP_PARAM_dot1agCfmMepDbChassisId,     
   RMEP_PARAM_dot1agCfmMepDbManAddressDomainLen,
   RMEP_PARAM_dot1agCfmMepDbManAddressDomain,
   RMEP_PARAM_dot1agCfmMepDbManAddressLen,
   RMEP_PARAM_dot1agCfmMepDbManAddress,
   /* User defined parameters */
   RMEP_PARAM_dot1agCfmMepDbCcmExpiryTime /* RMEP entry expiry timer */

} Dot1agCfmMepDbEntry_t;

typedef enum
{
   CFM_MP_PARAM_dot1agCfmStackifIndex,
   CFM_MP_PARAM_dot1agCfmStackVlanIdOrNone,
   CFM_MP_PARAM_dot1agCfmStackMdLevel,
   CFM_MP_PARAM_dot1agCfmStackDirection,
   CFM_MP_PARAM_dot1agCfmStackMdIndex,
   CFM_MP_PARAM_dot1agCfmStackMaIndex,
   CFM_MP_PARAM_dot1agCfmStackMepId,
   CFM_MP_PARAM_dot1agCfmStackMacAddress,
  /* params that are not part of standard MIB follows */
   CFM_MP_PARAM_dot1agCfmStackMepOperMode,

} Dot1agCfmStackEntry_t;

typedef enum dot1agLtrRelayAction_e
{
  L7_DOT1AG_LTR_RELAY_ACTION_HIT = 1,
  L7_DOT1AG_LTR_RELAY_ACTION_FDB,
  L7_DOT1AG_LTR_RELAY_ACTION_MPDB
}Dot1agLtrRelayAction_t;

typedef enum dot1agLtrIngressAction_e
{
  L7_DOT1AG_LTR_INGRESS_OK = 1,
  L7_DOT1AG_LTR_INGRESS_DOWN,
  L7_DOT1AG_LTR_INGRESS_BLOCKED,
  L7_DOT1AG_LTR_INGRESS_VID
}Dot1agLtrIngressAction_t;

typedef enum dot1agLtrEgressAction_e
{
  L7_DOT1AG_LTR_EGRESS_OK = 1,
  L7_DOT1AG_LTR_EGRESS_DOWN,
  L7_DOT1AG_LTR_EGRESS_BLOCKED,
  L7_DOT1AG_LTR_EGRESS_VID
}Dot1agLtrEgressAction_t;

typedef enum
{

  DOT1AG_CHASSIS_ID_SUBTYPE_chassisComponent = 1,
  DOT1AG_CHASSIS_ID_SUBTYPE_interfaceAlias   = 2,
  DOT1AG_CHASSIS_ID_SUBTYPE_portComponent    = 3,
  DOT1AG_CHASSIS_ID_SUBTYPE_macAddress       = 4,
  DOT1AG_CHASSIS_ID_SUBTYPE_networkAddress   = 5,
  DOT1AG_CHASSIS_ID_SUBTYPE_interfaceName    = 6,
  DOT1AG_CHASSIS_ID_SUBTYPE_local            = 7

} Dot1agLldpChassisIdSubtype_t;

typedef enum
{
  DOT1AG_MEP_DEFECT_BIT_bDefRDICCM    = 0x01,		
  DOT1AG_MEP_DEFECT_BIT_bDefMACstatus = 0x02,        
  DOT1AG_MEP_DEFECT_BIT_bDefRemoteCCM = 0x04,        
  DOT1AG_MEP_DEFECT_BIT_bDefErrorCCM  = 0x08,        
  DOT1AG_MEP_DEFECT_BIT_bDefXconCCM   = 0x10
} Dot1agCfmMepDefectsBitMask_t;

typedef enum
{
  DOT1AG_DEFECT_none         = 0,
  DOT1AG_DEFECT_defRDICCM    = 1,
  DOT1AG_DEFECT_defMACstatus = 2,
  DOT1AG_DEFECT_defRemoteCCM = 3,
  DOT1AG_DEFECT_defErrorCCM  = 4,
  DOT1AG_DEFECT_defXconCCM   = 5
} Dot1agCfmMepDefects_t; 
#endif /* __DOT1AG_EXPORTS_H_*/

