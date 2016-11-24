/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\packages\base\k_mib_lldp_api.h
*
* @purpose  Provide interface to LLDP MIB
*
* @component SNMP
*
* @comments
*
* @create 04/15/2005
*
* @author ikiran
* @end
*
**********************************************************************/
#ifndef __K_MIB_FASTPATH_LLDP_API_H__
#define __K_MIB_FASTPATH_LLDP_API_H__

#include <k_private_base.h>
#include "lldp_api.h"

L7_RC_t
snmpLldpIntfAdminModeGet( L7_uint32 intIfNum, L7_int32 *adminStatus);

L7_RC_t
snmpLldpIntfAdminModeSet( L7_uint32 intIfNum, L7_int32 adminStatus);


L7_RC_t
snmpLldpIntfNoticationEnableGet( L7_uint32 intIfNum, 
                                 L7_int32 *notification_enable);

L7_RC_t
snmpLldpIntfNoticationEnableSet( L7_uint32 intIfNum, 
                                 L7_int32 notification_enable);

L7_RC_t
snmpLldpIntfTLVsTxEnableGet( L7_uint32 intIfNum, 
                             L7_uchar8 *tlvstxenable);

L7_RC_t
snmpLldpIntfTLVsTxEnableSet( L7_uint32 intIfNum, 
                             L7_uchar8 *tlvstxenable);

L7_RC_t
snmpLldpAddrFamilyGet(L7_int32  subtype,
                       lldpIANAAddrFamilyNumber_t *addr_family);

L7_RC_t
snmpLldpAddrSubTypeGet(lldpIANAAddrFamilyNumber_t addr_family,
                      L7_int32  *subtype);

L7_RC_t
snmpLldpLocManAddrGet(L7_int32 lldpLocManAddrSubtype,
                      L7_uchar8 *address, L7_int32 *lldpLocManAddrLen);

L7_RC_t
snmpLldpLocManAddrGetNext(L7_int32 *lldpLocManAddrSubtype,
                          L7_uchar8 *address, L7_int32 *lldpLocManAddrLen);

L7_RC_t
snmpLldpNextExtIfNumber(L7_uint32 UnitIndex, L7_uint32 *extIfNum);

L7_RC_t
snmpLldpRemEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfIndex,
                    L7_int32 lldpRemIndex);

L7_RC_t
snmpLldpRemEntryGetNext(L7_uint32 *lldpRemTimeMark, L7_int32 *intIfIndex,
                        L7_int32 *lldpRemIndex);

L7_RC_t
snmpLldpRemManAddrEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfIndex,
                    L7_int32 lldpRemIndex,
                    lldpIANAAddrFamilyNumber_t lldpRemManAddrSubtype,
                    L7_uchar8 *lldpRemManAddr, L7_int32 length);

L7_RC_t
snmpLldpRemManAddrEntryGetNext(L7_uint32 *lldpRemTimeMark, L7_int32 *intIfIndex,
                              L7_int32 *lldpRemIndex,
                              lldpIANAAddrFamilyNumber_t *lldpRemManAddrSubtype,
                              L7_uchar8 *lldpRemManAddr, L7_int32 *length);

L7_RC_t
snmpLldpRemUnknownTLVEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfIndex,
                              L7_int32 lldpRemIndex,
                              L7_int32 lldpRemUnknownTLVType);

L7_RC_t
snmpLldpRemUnknownTLVEntryGetNext(L7_uint32 *lldpRemTimeMark,
                                  L7_int32 *intIfIndex, L7_int32 *lldpRemIndex,
                                  L7_int32 *lldpRemUnknownTLVType);

L7_RC_t
snmpLldpRemOrgDefInfoEntryGet(L7_uint32 lldpRemTimeMark, L7_int32 intIfNum,
                              L7_int32 lldpRemIndex,
                              L7_int32 lldpRemOrgDefInfoIndex,
                              L7_uchar8* oui,
                              L7_uint32* subtype,
                              L7_uchar8* info,
                              L7_uint32* length);

L7_RC_t
snmpLldpRemOrgDefInfoEntryGetNext(L7_uint32 *lldpRemTimeMark,
                                  L7_int32 *intIfNum, L7_int32 *lldpRemIndex,
                                  L7_int32 *lldpRemOrgDefInfoIndex,L7_uchar8* oui,
                                  L7_uint32* subtype,
                                  L7_uchar8* info,
                                  L7_uint32* length);

L7_ushort16 snmpSysCapReverse(L7_ushort16 org);

L7_RC_t
snmpLldpConfigManAddrPortsTxEnableGet(L7_uchar8 *portList, L7_uint32 *length);

L7_RC_t
snmpLldpConfigManAddrPortsTxEnableSet (L7_uchar8 *buf, L7_uint32 buf_len );


L7_RC_t
snmpLldpXdot3LocPortAutoNegSupportedGet(L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpLldpXdot3LocPortAutoNegEnabledGet(L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpLldpXdot3RemPortAutoNegSupportedGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val);

L7_RC_t
snmpLldpXdot3RemPortAutoNegEnabledGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val);

L7_RC_t
snmpLldpXdot3RemPortAutoNegAdvertisedCapGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uchar8 *buf);

L7_RC_t
snmpLldpXdot3RemPortOperMauTypeGet(L7_uint32 lldpRemTimeMark, L7_uint32 intIfNum, L7_uint32 lldpRemIndex, L7_uint32 *val);

L7_RC_t
snmpLldpXMedPortCapSupportedGet(L7_uint32 intIfNum, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedPortConfigTLVsTxEnableGet(L7_uint32 intIfNum, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedPortConfigTLVsTxEnableSet(L7_uint32 intIfNum, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedPortConfigNotifEnableGet(L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpLldpXMedPortConfigNotifEnableSet(L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpLldpXMedLocDeviceClassGet(L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocXPoeDeviceTypeGet(L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocXPoEPSEPowerSourceGet(L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocXPoEPDPowerSourceGet(L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocXPoEPDPowerPriorityGet(L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeIndexNextGet(L7_uchar8 *buf, L7_uint32 *buf_len);

L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(L7_uchar8 *buf, L7_uint32 buf_len, lldpXMedPolicyAppTypeValue_t *appType);

L7_RC_t
snmpLldpXMedLocMediaPolicyAppTypeToBitmask(lldpXMedPolicyAppTypeValue_t val, L7_uchar8 *buf, L7_uint32 *buf_len);

L7_RC_t
snmpLldpXMedLocMediaPolicyEntryIndexNextGet(L7_uint32 *intIfNum, L7_uchar8 *buf, L7_uint32 *buf_len);

L7_RC_t
snmpLldpXMedLocMediaPolicyEntryGet(lldpXMedLocMediaPolicyEntry_t *lldpXMedLocMediaPolicyEntryData,
                   int nominator);
L7_RC_t
snmpLldpXMedLocMediaPolicyEntryNextGet(lldpXMedLocMediaPolicyEntry_t *lldpXMedLocMediaPolicyEntryData,
                       int nominator);
L7_RC_t
snmpLldpXMedLocMediaPolicyUnknownGet(L7_uint32 intIfNum, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocMediaPolicyTaggedGet(L7_uint32 intIfNum, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val);

L7_RC_t
snmpLldpXMedLocLocationSubTypeGet(L7_uint32 val, L7_uint32 *subType);

L7_RC_t
snmpLldpXMedLocLocationEntryGet(L7_uint32 intIfNum, L7_uint32 subType, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedLocLocationEntryNextGet(L7_uint32 *intIfNum, L7_uint32 *subType, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedLocXPoePSEPortPDPriorityGet(L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemCapSupportedGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedRemCapCurrentGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedRemDeviceClassGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemLocationEntryGet(L7_uint32 timeMark, L7_uint32 intIfNum,
                                L7_uint32 remIndex, L7_uint32 subType, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedRemLocationEntryNextGet(L7_uint32 *timeMark, L7_uint32 *intIfNum,
                                    L7_uint32 *remIndex, L7_uint32 *subType, L7_uchar8 *buf);

L7_RC_t
snmpLldpXMedRemXPoEDeviceTypeGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemXPoEPSEPowerSourceGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemXPoEPSEPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemXPoEPDPowerSourceGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemXPoEPDPowerPriorityGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemMediaPolicyEntryGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, 
                                   lldpXMedPolicyAppTypeValue_t appType);

L7_RC_t
snmpLldpXMedRemMediaPolicyEntryNextGet(L7_uint32 *intIfNum, L7_uint32 *remIndex, L7_uint32 *timestamp, 
                                       L7_uchar8 *buf, L7_uint32 *buf_len);

L7_RC_t
snmpLldpXMedRemMediaPolicyUnknownGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val);

L7_RC_t
snmpLldpXMedRemMediaPolicyTaggedGet(L7_uint32 intIfNum, L7_uint32 remIndex, L7_uint32 timestamp, lldpXMedPolicyAppTypeValue_t appType, L7_uint32 *val);
L7_RC_t snmpLldpXMedLocXPoePDPowerReqGet(L7_uint32 *val);
#endif
