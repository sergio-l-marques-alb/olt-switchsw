/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmptrap.c
*
* Purpose: System Specific code for RFC1213, RFC2233 MIBs
*
* Created by: Colin Verne ?/?/2000
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
                         
**********************************************************************
*********************************************************************/

#include "k_private_base.h"
#include "k_mib_if_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_2233_stats_api.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

/* Local function prototype. */
static L7_RC_t snmpLocalIfTypeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

interfaces_t *
k_interfaces_get(int serialNum, ContextInfo *contextInfo,
                 int nominator)
{
  static interfaces_t interfacesData;

  ZERO_VALID(interfacesData.valid);

  switch(nominator)
  {
  case I_ifNumber:
    if ( usmDb1213IfNumberGet(USMDB_UNIT_CURRENT, &interfacesData.ifNumber) == L7_SUCCESS )
      SET_VALID(I_ifNumber, interfacesData.valid);
    break;
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, interfacesData.valid) )
    return(NULL);

  return(&interfacesData);
}

ifEntry_t *
k_ifEntry_get(int serialNum, ContextInfo *contextInfo,
              int nominator,
              int searchType,
              SR_INT32 ifIndex)
{
  static ifEntry_t ifEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len=SNMP_BUFFER_LEN;
  L7_uint32 snmp_int_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_int_buffer_len = SNMP_BUFFER_LEN;
  L7_uint32 intIfIndex;

  if (firstTime == L7_TRUE)
  {
    ifEntryData.ifDescr = NULL;
    ifEntryData.ifDescr = MakeOctetString(NULL, 0);
    ifEntryData.ifPhysAddress = NULL;
    ifEntryData.ifPhysAddress = MakeOctetString(NULL, 0);
    /* lvl7_@p1711 start */
    ifEntryData.ifSpecific = NULL;
    ifEntryData.ifSpecific = MakeOID(NULL, 0);
    ifEntryData.ifAlias = NULL;
    ifEntryData.ifAlias = MakeOctetString(NULL, 0);
#ifdef NOT_SUPPORTED
    ifEntryData.ifTestOwner = NULL;
    ifEntryData.ifTestOwner = MakeOctetString(NULL, 0);
#endif /* NOT_SUPPORTED */

    firstTime = L7_FALSE;
  }

  ZERO_VALID(ifEntryData.valid);
  ifEntryData.ifIndex = ifIndex;
  SET_VALID(I_ifIndex, ifEntryData.valid);

  if ((searchType == EXACT ? ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, ifEntryData.ifIndex) != 
       L7_SUCCESS ) : ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, ifEntryData.ifIndex) != L7_SUCCESS
       && usmDbGetNextVisibleExtIfNumber(ifEntryData.ifIndex, &ifEntryData.ifIndex) != L7_SUCCESS ) ) ||
       usmDbIntIfNumFromExtIfNum(ifEntryData.ifIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(ifEntryData.valid);
    return(NULL);
  }
   memset(snmp_buffer,0x00,sizeof(snmp_buffer));
/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
  switch (nominator)
  {
  case -1:
  case I_ifIndex:
    if(nominator != -1) break;
    /* else pass through */

  case I_ifDescr:
    if ( usmDbIfDescrGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer) == L7_SUCCESS &&
         SafeMakeOctetStringFromText(&ifEntryData.ifDescr, snmp_buffer) == L7_TRUE)
      SET_VALID(I_ifDescr, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifType:
    /* if ( usmDbIfTypeGet(USMDB_UNIT_CURRENT, intIfIndex, &ifEntryData.ifType) == L7_SUCCESS ) */
    if (snmpLocalIfTypeGet(USMDB_UNIT_CURRENT, intIfIndex, &ifEntryData.ifType) == L7_SUCCESS)
      SET_VALID(I_ifType, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifMtu:
    if ( usmDbIfMtuGet(USMDB_UNIT_CURRENT, intIfIndex,
                       &ifEntryData.ifMtu) == L7_SUCCESS )
      SET_VALID(I_ifMtu, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifSpeed:
    if ( snmpIfSpeedGet(USMDB_UNIT_CURRENT, intIfIndex,
                        &ifEntryData.ifSpeed) == L7_SUCCESS )
      SET_VALID(I_ifSpeed, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifPhysAddress:
    if ( usmDbIfPhysAddressGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer) == L7_SUCCESS &&
         SafeMakeOctetString(&ifEntryData.ifPhysAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE )
      SET_VALID(I_ifPhysAddress, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifAdminStatus:
    if ( snmpIfAdminStatusGet(USMDB_UNIT_CURRENT, intIfIndex,
                              &ifEntryData.ifAdminStatus) == L7_SUCCESS )
      SET_VALID(I_ifAdminStatus, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOperStatus:
    if ( snmpIfOperStatusGet(USMDB_UNIT_CURRENT, intIfIndex,
                             &ifEntryData.ifOperStatus) == L7_SUCCESS )
      SET_VALID(I_ifOperStatus, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifLastChange:
    if ( snmpIfLastChangeGet(USMDB_UNIT_CURRENT, intIfIndex,
                             &ifEntryData.ifLastChange) == L7_SUCCESS )
      SET_VALID(I_ifLastChange, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInOctets:
    if ( usmDbIfInOctetsGet(USMDB_UNIT_CURRENT, intIfIndex,
                            &ifEntryData.ifInOctets) != L7_SUCCESS )
      ifEntryData.ifInOctets = 0;

    SET_VALID(I_ifInOctets, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInUcastPkts:
    if ( usmDbIfInUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                               &ifEntryData.ifInUcastPkts) != L7_SUCCESS )
      ifEntryData.ifInUcastPkts = 0;

    SET_VALID(I_ifInUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInNUcastPkts:
    if ( usmDbIfInNUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                &ifEntryData.ifInNUcastPkts) != L7_SUCCESS )
      ifEntryData.ifInNUcastPkts = 0;

    SET_VALID(I_ifInNUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInDiscards:
    if ( usmDbSnmpIfInDiscardsGet(USMDB_UNIT_CURRENT, intIfIndex,
                              &ifEntryData.ifInDiscards) != L7_SUCCESS )
      ifEntryData.ifInDiscards = 0;

    SET_VALID(I_ifInDiscards, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInErrors:
    if ( usmDbIfInErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                            &ifEntryData.ifInErrors) != L7_SUCCESS )
      ifEntryData.ifInErrors = 0;

    SET_VALID(I_ifInErrors, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInUnknownProtos:
    if ( usmDbIfInUnknownProtosGet(USMDB_UNIT_CURRENT, intIfIndex,
                                   &ifEntryData.ifInUnknownProtos) != L7_SUCCESS )
      ifEntryData.ifInUnknownProtos = 0;

    SET_VALID(I_ifInUnknownProtos, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutOctets:
    if ( usmDbIfOutOctetsGet(USMDB_UNIT_CURRENT, intIfIndex,
                             &ifEntryData.ifOutOctets) != L7_SUCCESS )
      ifEntryData.ifOutOctets = 0;

    SET_VALID(I_ifOutOctets, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutUcastPkts:
    if ( usmDbIfOutUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                &ifEntryData.ifOutUcastPkts) != L7_SUCCESS )
      ifEntryData.ifOutUcastPkts = 0;

    SET_VALID(I_ifOutUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutNUcastPkts:
    if ( usmDbIfOutNUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                 &ifEntryData.ifOutNUcastPkts) != L7_SUCCESS )
      ifEntryData.ifOutNUcastPkts = 0;

    SET_VALID(I_ifOutNUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutDiscards:
    if ( usmDbIfOutDiscardsGet(USMDB_UNIT_CURRENT, intIfIndex,
                               &ifEntryData.ifOutDiscards) != L7_SUCCESS )
      ifEntryData.ifOutDiscards = 0;

    SET_VALID(I_ifOutDiscards, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutErrors:
    if ( usmDbIfOutErrorsGet(USMDB_UNIT_CURRENT, intIfIndex,
                             &ifEntryData.ifOutErrors) != L7_SUCCESS )
      ifEntryData.ifOutErrors = 0;

    SET_VALID(I_ifOutErrors, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */
/* lvl7_@p0069 start */
#ifdef NOT_SUPPORTED
  case I_ifOutQLen:
    if ( usmDbIfOutQLenGet(USMDB_UNIT_CURRENT, intIfIndex,
                           &ifEntryData.ifOutQLen) == L7_SUCCESS )
      SET_VALID(I_ifOutQLen, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */
/* lvl7_@p0069 end */

  case I_ifSpecific:
    FreeOID(ifEntryData.ifSpecific);
    if ( usmDbIfSpecificGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_int_buffer, &snmp_int_buffer_len) == 
         L7_SUCCESS )
    {
      if ( (ifEntryData.ifSpecific = MakeOID(snmp_int_buffer, snmp_buffer_len)) != NULL )
        SET_VALID(I_ifSpecific, ifEntryData.valid);
    }
    else
    {
      ifEntryData.ifSpecific = MakeOIDFromDot("0.0");
    }
    if(nominator != -1) break;
    /* else pass through */

  case I_ifName:
    if ( usmDbIfNameGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer) == L7_SUCCESS &&
         SafeMakeOctetStringFromText(&ifEntryData.ifName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_ifName, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInMulticastPkts:
    if ( usmDbIfInMulticastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                   &ifEntryData.ifInMulticastPkts) != L7_SUCCESS )
      ifEntryData.ifInMulticastPkts = 0;

    SET_VALID(I_ifInMulticastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifInBroadcastPkts:
    if ( usmDbIfInBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                   &ifEntryData.ifInBroadcastPkts) != L7_SUCCESS )
      ifEntryData.ifInBroadcastPkts = 0;

    SET_VALID(I_ifInBroadcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutMulticastPkts:
    if ( usmDbIfOutMulticastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    &ifEntryData.ifOutMulticastPkts) != L7_SUCCESS )
      ifEntryData.ifOutMulticastPkts = 0;

    SET_VALID(I_ifOutMulticastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifOutBroadcastPkts:
    if ( usmDbIfOutBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    &ifEntryData.ifOutBroadcastPkts) != L7_SUCCESS )
      ifEntryData.ifOutBroadcastPkts = 0;

    SET_VALID(I_ifOutBroadcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCInOctets:
    if ( (ifEntryData.ifHCInOctets == NULL && (ifEntryData.ifHCInOctets = MakeCounter64(0)) == NULL) ||
         usmDbIfHCInOctetsGet(USMDB_UNIT_CURRENT, intIfIndex,
                              &ifEntryData.ifHCInOctets->big_end,
                              &ifEntryData.ifHCInOctets->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCInOctets, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCInUcastPkts:
    if ( (ifEntryData.ifHCInUcastPkts == NULL && (ifEntryData.ifHCInUcastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCInUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                 &ifEntryData.ifHCInUcastPkts->big_end,
                                 &ifEntryData.ifHCInUcastPkts->little_end) == L7_SUCCESS)
      SET_VALID(I_ifHCInUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCInMulticastPkts:
    if ( (ifEntryData.ifHCInMulticastPkts == NULL && (ifEntryData.ifHCInMulticastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCInMulticastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                     &ifEntryData.ifHCInMulticastPkts->big_end,
                                     &ifEntryData.ifHCInMulticastPkts->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCInMulticastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCInBroadcastPkts:
    if ( (ifEntryData.ifHCInBroadcastPkts == NULL && (ifEntryData.ifHCInBroadcastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCInBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                     &ifEntryData.ifHCInBroadcastPkts->big_end,
                                     &ifEntryData.ifHCInBroadcastPkts->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCInBroadcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCOutOctets:
    if ( (ifEntryData.ifHCOutOctets == NULL && (ifEntryData.ifHCOutOctets = MakeCounter64(0)) == NULL) ||
         usmDbIfHCOutOctetsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                               &ifEntryData.ifHCOutOctets->big_end,
                               &ifEntryData.ifHCOutOctets->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCOutOctets, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCOutUcastPkts:
    if ( (ifEntryData.ifHCOutUcastPkts == NULL && (ifEntryData.ifHCOutUcastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCOutUcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                  &ifEntryData.ifHCOutUcastPkts->big_end,
                                  &ifEntryData.ifHCOutUcastPkts->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCOutUcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCOutMulticastPkts:
    if ( (ifEntryData.ifHCOutMulticastPkts == NULL && (ifEntryData.ifHCOutMulticastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCOutMulticastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                      &ifEntryData.ifHCOutMulticastPkts->big_end,
                                      &ifEntryData.ifHCOutMulticastPkts->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCOutMulticastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHCOutBroadcastPkts:
    if ( (ifEntryData.ifHCOutBroadcastPkts == NULL && (ifEntryData.ifHCOutBroadcastPkts = MakeCounter64(0)) == NULL) ||
         usmDbIfHCOutBroadcastPktsGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                      &ifEntryData.ifHCOutBroadcastPkts->big_end,
                                      &ifEntryData.ifHCOutBroadcastPkts->little_end) == L7_SUCCESS )
      SET_VALID(I_ifHCOutBroadcastPkts, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifLinkUpDownTrapEnable:
    if ( snmpIfLinkUpDownTrapEnableGet(USMDB_UNIT_CURRENT, intIfIndex,
                                       &ifEntryData.ifLinkUpDownTrapEnable) == L7_SUCCESS )
      SET_VALID(I_ifLinkUpDownTrapEnable, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifHighSpeed:
    if ( snmpIfHighSpeedGet(USMDB_UNIT_CURRENT, intIfIndex,
                            &ifEntryData.ifHighSpeed) == L7_SUCCESS )
      SET_VALID(I_ifHighSpeed, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifPromiscuousMode:
    if ( snmpIfPromiscuousModeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                  &ifEntryData.ifPromiscuousMode) == L7_SUCCESS )
      SET_VALID(I_ifPromiscuousMode, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifConnectorPresent:
    if ( snmpIfConnectorPresentGet(USMDB_UNIT_CURRENT, intIfIndex,
                                   &ifEntryData.ifConnectorPresent) == L7_SUCCESS )
      SET_VALID(I_ifConnectorPresent, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifAlias:
    if ( usmDbIfAliasGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer) == L7_SUCCESS &&
         SafeMakeOctetStringFromText(&ifEntryData.ifAlias, snmp_buffer) == L7_TRUE)
      SET_VALID(I_ifAlias, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifCounterDiscontinuityTime:
/* lvl7_@p1716 start */
    if ( usmDbIfCounterDiscontinuityTimeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                            &ifEntryData.ifCounterDiscontinuityTime) == L7_SUCCESS )
      SET_VALID(I_ifCounterDiscontinuityTime, ifEntryData.valid);
/* lvl7_@p1716 end */
#ifndef NOT_SUPPORTED
    break;
#else
    if(nominator != -1) break;
    /* else pass through */
#endif
/* lvl7_@p0078 end */

/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  case I_ifTestId:
    if ( usmDbIfTestIdGet(USMDB_UNIT_CURRENT, intIfIndex,
                          &ifEntryData.ifTestId) == L7_SUCCESS )
      SET_VALID(I_ifTestId, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifTestStatus:
    if ( usmDbIfTestStatusGet(USMDB_UNIT_CURRENT, intIfIndex,
                              &ifEntryData.ifTestStatus) == L7_SUCCESS )
      SET_VALID(I_ifTestStatus, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifTestType:
    FreeOID(ifEntryData.ifTestType);
    if ( usmDbIfTestTypeGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_int_buffer, &snmp_int_buffer_len) == L7_SUCCESS &&
         (ifEntryData.ifTestType = MakeOID(snmp_int_buffer, snmp_int_buffer_len)) != NULL )
    {
      SET_VALID(I_ifTestType, ifEntryData.valid);
    }
    else
    {
      ifEntryData.ifTestType = MakeOIDFromDot("0.0");
    }
    if(nominator != -1) break;
    /* else pass through */

  case I_ifTestResult:
    if ( usmDbIfTestResultGet(USMDB_UNIT_CURRENT, intIfIndex,
                              &ifEntryData.ifTestResult) == L7_SUCCESS )
      SET_VALID(I_ifTestResult, ifEntryData.valid);
    if(nominator != -1) break;
    /* else pass through */

  case I_ifTestCode:
    FreeOID(ifEntryData.ifTestCode);
    if ( usmDbIfTestCodeGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_int_buffer, &snmp_int_buffer_len) == L7_SUCCESS &&
         (ifEntryData.ifTestCode = MakeOID(snmp_int_buffer, snmp_int_buffer_len)) != NULL )
    {
      SET_VALID(I_ifTestCode, ifEntryData.valid);
    }
    else
    {
      ifEntryData.ifTestCode = MakeOIDFromDot("0.0");
    }
    if(nominator != -1) break;
    /* else pass through */

  case I_ifTestOwner:
    if ( usmDbIfTestOwnerGet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer) == L7_SUCCESS &&
        SaveMakeOctetStringFromText(&ifEntryData.ifTestOwner, snmp_buffer) == L7_TRUE )
      SET_VALID(I_ifTestOwner, ifEntryData.valid);
    break;
#endif /* NOT_SUPPORTED */
/* lvl7_@p0079 end */
/* lvl7_@p1711 end */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, ifEntryData.valid) )
    return(NULL);

  return(&ifEntryData);
}

#ifdef SETS
int
k_ifEntry_test(ObjectInfo *object, ObjectSyntax *value,
               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_ifEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_ifEntry_set_defaults(doList_t *dp)
{
  ifEntry_t *data = (ifEntry_t *) (dp->data);

  if ((data->ifDescr = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->ifPhysAddress = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->ifInOctets = (SR_UINT32) 0;
  data->ifInUcastPkts = (SR_UINT32) 0;
  data->ifInNUcastPkts = (SR_UINT32) 0;
  data->ifInDiscards = (SR_UINT32) 0;
  data->ifInErrors = (SR_UINT32) 0;
  data->ifInUnknownProtos = (SR_UINT32) 0;
  data->ifOutOctets = (SR_UINT32) 0;
  data->ifOutUcastPkts = (SR_UINT32) 0;
  data->ifOutNUcastPkts = (SR_UINT32) 0;
  data->ifOutDiscards = (SR_UINT32) 0;
  data->ifOutErrors = (SR_UINT32) 0;
  if ((data->ifSpecific = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_ifEntry_set(ifEntry_t *data,
              ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
 
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (  usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, data->ifIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_ifAdminStatus, data->valid))
  {
    if ( snmpIfAdminStatusSet(USMDB_UNIT_CURRENT, intIfIndex,
                            data->ifAdminStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifAdminStatus, tempValid);
    }
  }

  if ( VALID(I_ifLinkUpDownTrapEnable, data->valid))
  {
    if ( snmpIfLinkUpDownTrapEnableSet(USMDB_UNIT_CURRENT, intIfIndex,
                                     data->ifLinkUpDownTrapEnable) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifLinkUpDownTrapEnable, tempValid);
    }
  }

  if ( VALID(I_ifPromiscuousMode, data->valid))
  {
    if ( snmpIfPromiscuousModeSet(USMDB_UNIT_CURRENT, intIfIndex,
                                data->ifPromiscuousMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifPromiscuousMode, tempValid);
    }
  }

  if ( VALID(I_ifAlias, data->valid)) 
  {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      memcpy(snmp_buffer, data->ifAlias->octet_ptr, data->ifAlias->length);
      if (usmDbIfAliasSet(USMDB_UNIT_CURRENT, intIfIndex, snmp_buffer)
                                != L7_SUCCESS) {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return(COMMIT_FAILED_ERROR);
      }   
      else
      {
          SET_VALID(I_ifAlias, tempValid);
      } 
  }


/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  if ( VALID(I_ifTestId, data->valid))
  {
   if ( usmDbIfTestIdSet(USMDB_UNIT_CURRENT, intIfIndex,
                        data->ifTestId) != L7_SUCCESS )
   {
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return(COMMIT_FAILED_ERROR);
   }
   else
   {
     SET_VALID(I_ifTestId, tempValid);
   }
  }

  if ( VALID(I_ifTestStatus, data->valid))
  {
    if( usmDbIfTestStatusSet(USMDB_UNIT_CURRENT, intIfIndex,
                            data->ifTestStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifTestStatus, tempValid);
    }
  }

  if ( VALID(I_ifTestType, data->valid))
  {
    if( usmDbIfTestTypeSet(USMDB_UNIT_CURRENT, intIfIndex,
                          data->ifTestType->oid_ptr,
                          data->ifTestType->length) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifTestType, tempValid);
    }
  }

  if ( VALID(I_ifTestOwner, data->valid))
  {
    if( usmDbIfTestOwnerSet(USMDB_UNIT_CURRENT, intIfIndex,
                           data->ifTestOwner->octet_ptr) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_ifTestOwner, tempValid);
    }
  }
#endif /* NOT_SUPPORTED */
/* lvl7_@p0079 end */

  return NO_ERROR;
}

  #ifdef SR_ifEntry_UNDO
/* add #define SR_ifEntry_UNDO in sitedefs.h to
 * include the undo routine for the ifEntry family.
 */
int
ifEntry_undo(doList_t *doHead, doList_t *doCur,
             ContextInfo *contextInfo)
{

  ifEntry_t *data = (ifEntry_t *) doCur->data;
  ifEntry_t *undodata = (ifEntry_t *) doCur->undodata;
  ifEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

 /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));


  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;


  /* use the set method for the undo */
  if (k_ifEntry_set(setdata, contextInfo, function) == NO_ERROR) 
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_ifEntry_UNDO */

#endif /* SETS */

ifMIBObjects_t *
k_ifMIBObjects_get(int serialNum, ContextInfo *contextInfo,
                   int nominator)
{
  static ifMIBObjects_t ifMIBObjectsData;
  L7_uint32 seconds;

  switch (nominator)
  {
#ifdef NOT_SUPPORTED
  case I_ifStackLastChange:
    if ( usmDbifStackLastChangeGet(USMDB_UNIT_CURRENT, 
                                   &ifMIBObjectsData.ifStackLastChange) == L7_SUCCESS )
      SET_VALID(I_ifStackLastChange, ifMIBObjectsData.valid);
   break;
#endif
   case I_ifTableLastChange:
    if(usmDbIfTableLastChangeGet(USMDB_UNIT_CURRENT, &seconds) == L7_SUCCESS)
    {
      SafeMakeTimeTicksFromSeconds(seconds, 
              &(ifMIBObjectsData.ifTableLastChange));
      SET_VALID(I_ifTableLastChange, ifMIBObjectsData.valid);
    }
    break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, ifMIBObjectsData.valid) )
    return(NULL);

  return(&ifMIBObjectsData);
}

ifStackEntry_t *
k_ifStackEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 ifStackHigherLayer,
                   SR_INT32 ifStackLowerLayer)
{
/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  static ifStackEntry_t ifStackEntryData;

  ZERO_VALID(ifStackEntryData.valid);
  ifStackEntryData.ifStackHigherLayer = ifStackHigherLayer;
  SET_VALID(I_ifStackHigherLayer, ifStackEntryData.valid);
  ifStackEntryData.ifStackLowerLayer = ifStackLowerLayer;
  SET_VALID(I_ifStackLowerLayer, ifStackEntryData.valid);

  if ( ((searchType == EXACT) ?
        usmDbIfStackEntryGet(USMDB_UNIT_CURRENT, ifStackEntryData.ifStackHigherLayer,
                             ifStackEntryData.ifStackLowerLayer) :
        usmDbIfStackEntryNext(USMDB_UNIT_CURRENT, &ifStackEntryData.ifStackHigherLayer,
                              &ifStackEntryData.ifStackLowerLayer)) != L7_SUCCESS )
  {
    ZERO_VALID(ifStackEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
  switch (nominator)
  {
  case -1:
  case I_ifStackHigherLayer:
  case I_ifStackLowerLayer:
    if (nominator != -1) break;
    /* else pass through */
    
  case I_ifStackStatus:
    if ( usmDbIfStackStatusGet(USMDB_UNIT_CURRENT,
                               ifStackEntryData.ifStackHigherLayer,
                               ifStackEntryData.ifStackLowerLayer,
                               &ifStackEntryData.ifStackStatus) != L7_SUCCESS )
      CLR_VALID(I_ifStackStatus, ifStackEntryData.valid);
    break;
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, ifStackEntryData.valid) )
    return(NULL);

  return(&ifStackEntryData);
#endif /* NOT_SUPPORTED */  
  return(NULL);
/* lvl7_@p0079 start */
}

#ifdef SETS
int
k_ifStackEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_ifStackEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_ifStackEntry_set_defaults(doList_t *dp)
{
  ifStackEntry_t *data = (ifStackEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_ifStackEntry_set(ifStackEntry_t *data,
                   ContextInfo *contextInfo, int function)
{
/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(data->valid);

  if ( usmDbIfStackEntryGet(USMDB_UNIT_CURRENT, data->ifStackHigherLayer,
                            data->ifStackLowerLayer) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_ifStackStatus, data->valid) &&
       usmDbIfStackStatusSet(USMDB_UNIT_CURRENT, data->ifStackHigherLayer,
                             data->ifStackLowerLayer,
                             data->ifStackStatus) != L7_SUCCESS )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }
  return NO_ERROR;
#else /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
/* lvl7_@p0079 end */
}

  #ifdef SR_ifStackEntry_UNDO
/* add #define SR_ifStackEntry_UNDO in sitedefs.h to
 * include the undo routine for the ifStackEntry family.
 */
int
ifStackEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  ifStackEntry_t *data = (ifStackEntry_t *) doCur->data;
  ifStackEntry_t *undodata = (ifStackEntry_t *) doCur->undodata;
  ifStackEntry_t *setdata = NULL;
  int function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->ifStackStatus = D_ifStackStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->ifStackStatus == D_ifStackStatus_notReady)
         || (undodata->ifStackStatus == D_ifStackStatus_notInService)) 
      {
          undodata->ifStackStatus = D_ifStackStatus_createAndWait;
      } 
      else 
      {
         if(undodata->ifStackStatus == D_ifStackStatus_active) 
           undodata->ifStackStatus = D_ifStackStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_ifStackEntry_set(setdata, contextInfo, function) == NO_ERROR)) 
      return NO_ERROR;

  return UNDO_FAILED_ERROR;

#else  /* NOT_SUPPORTED */
  return NO_ERROR;
#endif /* NOT_SUPPORTED */
}
  #endif /* SR_ifStackEntry_UNDO */

#endif /* SETS */

ifRcvAddressEntry_t *
k_ifRcvAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 ifIndex,
                        OctetString * ifRcvAddressAddress)
{
/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  static ifRcvAddressEntry_t ifRcvAddressEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfIndex;
  
  if (firstTime == L7_TRUE)
  {
    ifRcvAddressEntryData.ifRcvAddressAddress = MakeOctetString(NULL, 0);

    firstTime = L7_FALSE;
  }
    
  ifRcvAddressEntryData.ifIndex = ifIndex;
  SET_VALID(I_ifRcvAddressEntryIndex_ifIndex, ifRcvAddressEntryData.valid);
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, ifRcvAddressAddress->octet_ptr, ifRcvAddressAddress->length);
  SET_VALID(I_ifRcvAddressAddress, ifRcvAddressEntryData.valid);
  
  if ( ((searchType == EXACT) ?
        usmDbIfRcvAddressEntryGet(USMDB_UNIT_CURRENT, ifRcvAddressEntryData.ifIndex, snmp_buffer) :
        usmDbIfRcvAddressEntryNext(USMDB_UNIT_CURRENT, &ifRcvAddressEntryData.ifIndex, snmp_buffer)) != 
        L7_SUCCESS || usmDbIntIfNumFromExtIfNum(ifRcvAddressEntryData.ifIndex, &intIfIndex) != L7_SUCCESS ||
       (SafeMakeOctetStringFromText(&ifRcvAddressEntryData.ifRcvAddressAddress, snmp_buffer) == L7_FALSE )
  {
    ZERO_VALID(ifRcvAddressEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
  switch (nominator)
  {
  case -1:
  case I_ifRcvAddressEntryIndex_ifIndex:
  case I_ifRcvAddressAddress:
   if( nominator != -1) break;
    /* else pass through */

  case I_ifRcvAddressStatus:
    if ( usmDbIfRcvAddressStatusGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    ifRcvAddressEntryData.ifRcvAddressAddress->octet_ptr,
                                    &ifRcvAddressEntryData.ifRcvAddressStatus) == L7_SUCCESS )
      SET_VALID(I_ifRcvAddressStatus, ifRcvAddressEntryData.valid);
    if( nominator != -1) break;
    /* else pass through */

  case I_ifRcvAddressType:
    if ( usmDbIfRcvAddressTypeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                  ifRcvAddressEntryData.ifRcvAddressAddress->octet_ptr,
                                  &ifRcvAddressEntryData.ifRcvAddressType) == L7_SUCCESS )
      SET_VALID(I_ifRcvAddressType, ifRcvAddressEntryData.valid);
    break;
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, ifRcvAddressEntryData.valid) )
    return(NULL);

  return(&ifRcvAddressEntryData);
#else  /* NOT_SUPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */
/* lvl7_@p0079 end */
}

#ifdef SETS
int
k_ifRcvAddressEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_ifRcvAddressEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_ifRcvAddressEntry_set_defaults(doList_t *dp)
{
  ifRcvAddressEntry_t *data = (ifRcvAddressEntry_t *) (dp->data);

  data->ifRcvAddressType = D_ifRcvAddressType_volatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_ifRcvAddressEntry_set(ifRcvAddressEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
/* lvl7_@p0079 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uint32 intIfIndex;

  bzero(tempValid, sizeof(data->valid);

  if ( usmDbIfRcvAddressEntryGet(USMDB_UNIT_CURRENT, intIfIndex,
                                 data->ifRcvAddressAddress->octet_ptr) != L7_SUCCESS )
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if ( VALID(I_ifRcvAddressStatus, data->valid) &&
       usmDbIfRcvAddressStatusSet(USMDB_UNIT_CURRENT, intIfIndex,
                                  data->ifRcvAddressAddress->octet_ptr,
                                  data->ifRcvAddressStatus) != L7_SUCCESS )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }
  return NO_ERROR;
#else /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
/* lvl7_@p0079 end */

}

  #ifdef SR_ifRcvAddressEntry_UNDO
/* add #define SR_ifRcvAddressEntry_UNDO in sitedefs.h to
 * include the undo routine for the ifRcvAddressEntry family.
 */
int
ifRcvAddressEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  ifRcvAddressEntry_t *data = (ifRcvAddressEntry_t *) doCur->data;
  ifRcvAddressEntry_t *undodata = (ifRcvAddressEntry_t *) doCur->undodata;
  ifRcvAddressEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL)
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->ifRcvAddressStatus = D_ifRcvAddressStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->ifRcvAddressStatus == D_ifRcvAddressStatus_notReady)
         || (undodata->ifRcvAddressStatus == D_ifRcvAddressStatus_notInService)) 
      {
          undodata->ifRcvAddressStatus = D_ifRcvAddressStatus_createAndWait;
      } 
      else 
      {
         if(undodata->ifRcvAddressStatus == D_ifRcvAddressStatus_active) 
           undodata->ifRcvAddressStatus = D_ifRcvAddressStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && k_ifRcvAddressEntry_set(setdata, contextInfo, function) == NO_ERROR) 
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
#else /* NOT_SUPPORTED */
  return NO_ERROR;
#endif /* NOT_SUPPORTED */
}
  #endif /* SR_ifRcvAddressEntry_UNDO */

#endif /* SETS */

/*********************************************************************
*
* @purpose  Local snmp function to get the proper interface type to
*           be returned via get ifType. This function acts as a wrapper
*           to map all Ethernet types to L7_IANA_ETHERNET (6).
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    interface   @b{(input)} interface
* @param    val         @b{(output)} pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments The type of interface, distinguished according to
*           the physical/link protocol(s) immediately `below'
*           the network layer in the protocol stack
*        
* @end
*********************************************************************/
static L7_RC_t snmpLocalIfTypeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  L7_RC_t rc;

  /* Get ifType. */
  if ((rc=usmDbIfTypeGet(USMDB_UNIT_CURRENT, interface, val)) == L7_SUCCESS)
  {
    /* For all Ethernet types. */
    if ((*val == L7_IANA_FAST_ETHERNET)
    || (*val == L7_IANA_FAST_ETHERNET_FX)
    || (*val == L7_IANA_GIGABIT_ETHERNET)
    || (*val == L7_IANA_2G5BIT_ETHERNET)
    || (*val == L7_IANA_10G_ETHERNET))
    {
      /* Map to Ethernet type (6). */
      *val = L7_IANA_ETHERNET;
    }
  }

  return(rc);
}

