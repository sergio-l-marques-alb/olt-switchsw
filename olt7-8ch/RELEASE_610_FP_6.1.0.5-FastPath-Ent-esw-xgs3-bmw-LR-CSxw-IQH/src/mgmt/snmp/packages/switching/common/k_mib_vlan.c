/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_vlan.c
*
* @purpose System Specific code for RFC1493, RFC2674 MIBs
*
* @component SNMP
*
* @comments
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#define SNMP_VLAN_BUFFER_LEN 512

#include "k_private_base.h"
#include "k_mib_bridge_api.h"
#include "k_mib_vlan_api.h"
#include "garp_exports.h"
#include "usmdb_common.h"
#include "dot1s_exports.h"
#include "fdb_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_garp.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

dot1dBase_t *
k_dot1dBase_get(int serialNum, ContextInfo *contextInfo,
                int nominator)
{
  static dot1dBase_t dot1dBaseData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1dBaseData.dot1dBaseBridgeAddress = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1dBaseData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_dot1dBaseBridgeAddress:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1dBaseBridgeAddressGet(USMDB_UNIT_CURRENT, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        SafeMakeOctetString(&dot1dBaseData.dot1dBaseBridgeAddress, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_dot1dBaseBridgeAddress, dot1dBaseData.valid);
    break;

  case I_dot1dBaseNumPorts:
    if (usmDbDot1dBaseNumPortsGet(USMDB_UNIT_CURRENT, &dot1dBaseData.dot1dBaseNumPorts) == L7_SUCCESS)
      SET_VALID(I_dot1dBaseNumPorts, dot1dBaseData.valid);
    break;

  case I_dot1dBaseType:
    if (usmDbDot1dBaseTypeGet(USMDB_UNIT_CURRENT, &dot1dBaseData.dot1dBaseType) == L7_SUCCESS)
      SET_VALID(I_dot1dBaseType, dot1dBaseData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator && !VALID(nominator, dot1dBaseData.valid))
    return(NULL);

  return(&dot1dBaseData);
}

dot1dBasePortEntry_t *
k_dot1dBasePortEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 dot1dBasePort)
{
  static dot1dBasePortEntry_t dot1dBasePortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_int[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_int_len = SNMP_VLAN_BUFFER_LEN;
/* lvl7_@p1299 start */
/* lvl7_@p0363 start */
  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
/* lvl7_@p1698 start */
    dot1dBasePortEntryData.dot1dBasePortCircuit = MakeOIDFromDot("0.0");
/* lvl7_@p1698 end */
    dot1dBasePortEntryData.dot1dPortCapabilities = MakeOctetString(NULL, 0);
    dot1dBasePortEntryData.dot1dPortGmrpLastPduOrigin = MakeOctetString(NULL, 0);
    dot1dBasePortEntryData.dot1qPortGvrpLastPduOrigin = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1dBasePortEntryData.valid);
  dot1dBasePortEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1dBasePort, dot1dBasePortEntryData.valid);

  if ( searchType == EXACT ? ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort) != L7_SUCCESS) :
       ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort) != L7_SUCCESS &&
         snmpDot1dBasePortEntryNextGet(USMDB_UNIT_CURRENT, &dot1dBasePortEntryData.dot1dBasePort) != L7_SUCCESS ) )
  {
    ZERO_VALID(dot1dBasePortEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1dBasePort:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dBasePortIfIndex:
    if (usmDbDot1dBasePortIfIndexGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                     &dot1dBasePortEntryData.dot1dBasePortIfIndex) == L7_SUCCESS)
      SET_VALID(I_dot1dBasePortIfIndex, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dBasePortCircuit:
    FreeOID(dot1dBasePortEntryData.dot1dBasePortCircuit);
    if (usmDbDot1dBasePortCircuitGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                     snmp_buffer_int, &snmp_buffer_int_len) == L7_SUCCESS)
    {
      if ((dot1dBasePortEntryData.dot1dBasePortCircuit = MakeOID(snmp_buffer_int, snmp_buffer_int_len)) != NULL)
        SET_VALID(I_dot1dBasePortCircuit, dot1dBasePortEntryData.valid);
    }
    else
    {
      dot1dBasePortEntryData.dot1dBasePortCircuit = MakeOIDFromDot("0.0");
    }
    if (nominator != -1) break;
    /* else pass through */

/* lvl7_@p0361 start */
#ifdef NOT_SUPPORTED
  case I_dot1dBasePortDelayExceededDiscards:
    if (usmDbDot1dBasePortDelayExceededDiscardsGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                                   &dot1dBasePortEntryData.dot1dBasePortDelayExceededDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1dBasePortDelayExceededDiscards, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */

/* lvl7_@p0361 end */

/* lvl7_@p0362 start */
#ifdef NOT_SUPPORTED
  case I_dot1dBasePortMtuExceededDiscards:
    if (usmDbDot1dBasePortMtuExceededDiscardsGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                                 &dot1dBasePortEntryData.dot1dBasePortMtuExceededDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1dBasePortMtuExceededDiscards, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */

/* lvl7_@p0362 end */

  case I_dot1dPortCapabilities:
    CLR_VALID(I_dot1dPortCapabilities, dot1dBasePortEntryData.valid);
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1dPortCapabilitiesGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1dBasePortEntryData.dot1dPortCapabilities, snmp_buffer,1) == L7_TRUE))
      SET_VALID(I_dot1dPortCapabilities, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortDefaultUserPriority:
    if (usmDbDot1dPortDefaultUserPriorityGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                             &dot1dBasePortEntryData.dot1dPortDefaultUserPriority) == L7_SUCCESS)
      SET_VALID(I_dot1dPortDefaultUserPriority, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortNumTrafficClasses:
    if (usmDbDot1dPortNumTrafficClassesGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                           &dot1dBasePortEntryData.dot1dPortNumTrafficClasses) == L7_SUCCESS)
      SET_VALID(I_dot1dPortNumTrafficClasses, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGarpJoinTime:
    if (usmDbGarpJoinTimeGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                             &dot1dBasePortEntryData.dot1dPortGarpJoinTime) == L7_SUCCESS)
      SET_VALID(I_dot1dPortGarpJoinTime, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGarpLeaveTime:
    if (usmDbGarpLeaveTimeGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                              &dot1dBasePortEntryData.dot1dPortGarpLeaveTime) == L7_SUCCESS)
      SET_VALID(I_dot1dPortGarpLeaveTime, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGarpLeaveAllTime:
    if (usmDbGarpLeaveAllTimeGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                 &dot1dBasePortEntryData.dot1dPortGarpLeaveAllTime) == L7_SUCCESS)
      SET_VALID(I_dot1dPortGarpLeaveAllTime, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGmrpStatus:
    if (snmpDot1dPortGmrpStatusGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                    &dot1dBasePortEntryData.dot1dPortGmrpStatus) == L7_SUCCESS)
    {
      SET_VALID(I_dot1dPortGmrpStatus, dot1dBasePortEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGmrpFailedRegistrations:
    if (usmDbDot1dPortGmrpFailedRegistrationsGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                                 &dot1dBasePortEntryData.dot1dPortGmrpFailedRegistrations) == L7_SUCCESS)
      SET_VALID(I_dot1dPortGmrpFailedRegistrations, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dPortGmrpLastPduOrigin:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1dPortGmrpLastPduOriginGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1dBasePortEntryData.dot1dPortGmrpLastPduOrigin, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_dot1dPortGmrpLastPduOrigin, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPvid:
    if (usmDbQportsPVIDGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                           &dot1dBasePortEntryData.dot1qPvid) == L7_SUCCESS)
      SET_VALID(I_dot1qPvid, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPortAcceptableFrameTypes:
/* lvl7_@p0802 start */
    if (snmpDot1qPortAcceptableFrameTypesGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                             &dot1dBasePortEntryData.dot1qPortAcceptableFrameTypes) == L7_SUCCESS)
      SET_VALID(I_dot1qPortAcceptableFrameTypes, dot1dBasePortEntryData.valid);
/* lvl7_@p0802 end */
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPortIngressFiltering:
/* lvl7_@p0803 start */
    if (snmpDot1qPortIngressFilteringGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                         &dot1dBasePortEntryData.dot1qPortIngressFiltering) == L7_SUCCESS)
      SET_VALID(I_dot1qPortIngressFiltering, dot1dBasePortEntryData.valid);
/* lvl7_@p0803 end */
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPortGvrpStatus:
/* lvl7_@p0804 start */
    if (snmpDot1qPortGvrpStatusGet(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                   &dot1dBasePortEntryData.dot1qPortGvrpStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qPortGvrpStatus, dot1dBasePortEntryData.valid);
/* lvl7_@p0804 end */
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPortGvrpFailedRegistrations:
    if (usmDbDot1qPortGvrpFailedRegistrations(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort,
                                              &dot1dBasePortEntryData.dot1qPortGvrpFailedRegistrations) == L7_SUCCESS)
      SET_VALID(I_dot1qPortGvrpFailedRegistrations, dot1dBasePortEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qPortGvrpLastPduOrigin:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);  
    if (usmDbDot1qPortGvrpLastPduOrigin(USMDB_UNIT_CURRENT, dot1dBasePortEntryData.dot1dBasePort, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1dBasePortEntryData.dot1qPortGvrpLastPduOrigin, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_dot1qPortGvrpLastPduOrigin, dot1dBasePortEntryData.valid);
    break;
/* lvl7_@p0363 end */
/* lvl7_@p1299 end */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dBasePortEntryData.valid))
    return(NULL);

  return(&dot1dBasePortEntryData);
}

#ifdef SETS
int
k_dot1dBasePortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1dBasePortEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1dBasePortEntry_set_defaults(doList_t *dp)
{
  dot1dBasePortEntry_t *data = (dot1dBasePortEntry_t *) (dp->data);

  if ((data->dot1dBasePortCircuit = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->dot1dBasePortDelayExceededDiscards = (SR_UINT32) 0;
  data->dot1dBasePortMtuExceededDiscards = (SR_UINT32) 0;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1dBasePortEntry_set(dot1dBasePortEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

/* lvl7_@p1299 start */
  if (snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, data->dot1dBasePort) != L7_SUCCESS)
  {
    CLR_VALID(I_dot1qPvid, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1dPortNumTrafficClasses, data->valid)) 
  {
    /* this object supported as READ-ONLY in FASTPATH */
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1dPortDefaultUserPriority, data->valid))
  {
    if(usmDbDot1dPortDefaultUserPrioritySet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dPortDefaultUserPriority) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  
    else
    {
       SET_VALID(I_dot1dPortDefaultUserPriority, tempValid);
    }
  }

  if (VALID(I_dot1dPortGarpJoinTime,data->valid))
  {
    if (  ( data->dot1dPortGarpJoinTime < L7_GARP_MIN_JOIN_TIME) || 
               ( data->dot1dPortGarpJoinTime > L7_GARP_MAX_JOIN_TIME)  )
      return COMMIT_FAILED_ERROR;
 
    if(usmDbGarpJoinTimeSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dPortGarpJoinTime) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1dPortGarpJoinTime, tempValid);
    }
  }

  if (VALID(I_dot1dPortGarpLeaveTime,data->valid))
  {
    if (  (data->dot1dPortGarpLeaveTime < L7_GARP_MIN_LEAVE_TIME) || (data->dot1dPortGarpLeaveTime > L7_GARP_MAX_LEAVE_TIME)  )
      return COMMIT_FAILED_ERROR;

    if(usmDbGarpLeaveTimeSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dPortGarpLeaveTime) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  
    else
    {
       SET_VALID(I_dot1dPortGarpLeaveTime, tempValid);
    }
  }

  if (VALID(I_dot1dPortGarpLeaveAllTime,data->valid)) 
  {
    if (  (data->dot1dPortGarpLeaveAllTime < L7_GARP_MIN_LEAVE_ALL_TIME) || 
                  (data->dot1dPortGarpLeaveAllTime > L7_GARP_MAX_LEAVE_ALL_TIME))
      return COMMIT_FAILED_ERROR;
    if(usmDbGarpLeaveAllTimeSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dPortGarpLeaveAllTime) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1dPortGarpLeaveAllTime, tempValid);
    }
  }

  if (VALID(I_dot1dPortGmrpStatus,data->valid))
  {
    if(snmpDot1dPortGmrpStatusSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dPortGmrpStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1dPortGmrpStatus, tempValid);
    }
  }

  if (VALID(I_dot1qPvid, data->valid))
  {
    if(usmDbQportsPVIDSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1qPvid) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1qPvid, tempValid);
    }
  }

/* lvl7_@p0802 start */
  if (VALID(I_dot1qPortAcceptableFrameTypes, data->valid))
  { 
    if(snmpDot1qPortAcceptableFrameTypesSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1qPortAcceptableFrameTypes) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1qPortAcceptableFrameTypes, tempValid);
    }
  }
/* lvl7_@p0802 end */

/* lvl7_@p0803 start */
  if (VALID(I_dot1qPortIngressFiltering, data->valid))
  {
    if(snmpDot1qPortIngressFilteringSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1qPortIngressFiltering) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

    else
    {
       SET_VALID(I_dot1qPortIngressFiltering, tempValid);
    }
  }
/* lvl7_@p0803 end */

/* lvl7_@p0804 start */
  if (VALID(I_dot1qPortGvrpStatus, data->valid))
  {
    if(snmpDot1qPortGvrpStatusSet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1qPortGvrpStatus) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

/* lvl7_@p0804 end */
/* lvl7_@p1299 end */

  return(NO_ERROR);
}

  #ifdef SR_dot1dBasePortEntry_UNDO
/* add #define SR_dot1dBasePortEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1dBasePortEntry family.
 */
int
dot1dBasePortEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  dot1dBasePortEntry_t *data = (dot1dBasePortEntry_t *) doCur->data;
  dot1dBasePortEntry_t *undodata = (dot1dBasePortEntry_t *) doCur->undodata;
  dot1dBasePortEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dBasePortEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot1dBasePortEntry_UNDO */

#endif /* SETS */

dot1dStp_t *
k_dot1dStp_get(int serialNum, ContextInfo *contextInfo,
               int nominator)
{
  static dot1dStp_t dot1dStpData;
  static L7_BOOL firstTime = L7_TRUE;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_TRUE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1dStpData.dot1dStpDesignatedRoot = MakeOctetString(NULL, 0);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dStpProtocolSpecification:
    /* else pass through */

  case I_dot1dStpPriority:
    /* else pass through */

  case I_dot1dStpTimeSinceTopologyChange:
    /* else pass through */

  case I_dot1dStpTopChanges:
    /* else pass through */

  case I_dot1dStpDesignatedRoot:
    /* else pass through */

  case I_dot1dStpRootCost:
    /* else pass through */

  case I_dot1dStpRootPort:
    /* else pass through */

  case I_dot1dStpMaxAge:
    /* else pass through */

  case I_dot1dStpHelloTime:
    /* else pass through */

  case I_dot1dStpHoldTime:
    /* else pass through */

  case I_dot1dStpForwardDelay:
    /* else pass through */

/* lvl7_@p1656 start */
  case I_dot1dStpBridgeMaxAge:
    /* else pass through */

  case I_dot1dStpBridgeHelloTime:
    /* else pass through */

  case I_dot1dStpBridgeForwardDelay:
   /* Dot1d STP is no loger part of base code, refer MIBs for dot1s STP*/

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dStpData.valid))
    return(NULL);

  return(&dot1dStpData);
}

#ifdef SETS
int
k_dot1dStp_test(ObjectInfo *object, ObjectSyntax *value,
                doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_dot1dStp_ready(ObjectInfo *object, ObjectSyntax *value, 
                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_dot1dStp_set(dot1dStp_t *data,
               ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }

  /* use dot1s MIB to set spanning tree*/
  return(COMMIT_FAILED_ERROR);
}

  #ifdef SR_dot1dStp_UNDO
/* add #define SR_dot1dStp_UNDO in sitedefs.h to
 * include the undo routine for the dot1dStp family.
 */
int
dot1dStp_undo(doList_t *doHead, doList_t *doCur,
              ContextInfo *contextInfo)
{
  dot1dStp_t *data = (dot1dStp_t *) doCur->data;
  dot1dStp_t *undodata = (dot1dStp_t *) doCur->undodata;
  dot1dStp_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dStp_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_dot1dStp_UNDO */

#endif /* SETS */

dot1dStpPortEntry_t *
k_dot1dStpPortEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 dot1dStpPort)
{
  /*
  static dot1dStpPortEntry_t dot1dStpPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
  */
/* lvl7_@p0365 start */

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_TRUE)
  {
    return(NULL);
  }

  return(NULL);
}

#ifdef SETS
int
k_dot1dStpPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_dot1dStpPortEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_dot1dStpPortEntry_set_defaults(doList_t *dp)
{
  dot1dStpPortEntry_t *data = (dot1dStpPortEntry_t *) (dp->data);

  if ((data->dot1dStpPortDesignatedRoot = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }
  if ((data->dot1dStpPortDesignatedBridge = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }
  if ((data->dot1dStpPortDesignatedPort = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }
  data->dot1dStpPortForwardTransitions = (SR_UINT32) 0;

  SET_ALL_VALID(data->valid);
  return(NO_ERROR);
}

int
k_dot1dStpPortEntry_set(dot1dStpPortEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }
  return(COMMIT_FAILED_ERROR);
}

  #ifdef SR_dot1dStpPortEntry_UNDO
/* add #define SR_dot1dStpPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1dStpPortEntry family.
 */
int
dot1dStpPortEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
  dot1dStpPortEntry_t *data = (dot1dStpPortEntry_t *) doCur->data;
  dot1dStpPortEntry_t *undodata = (dot1dStpPortEntry_t *) doCur->undodata;
  dot1dStpPortEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dStpPortEntry_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }
  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_dot1dStpPortEntry_UNDO */

#endif /* SETS */

dot1dTp_t *
k_dot1dTp_get(int serialNum, ContextInfo *contextInfo,
              int nominator)
{
  static dot1dTp_t dot1dTpData;
  L7_FDB_TYPE_t vlType;
  
  ZERO_VALID(dot1dTpData.valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
   if (nominator != -1) break;
    /* else pass through */

  case I_dot1dTpAgingTime:
    /* Ensure SVL is supported */
    if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) == L7_SUCCESS) &&
         (vlType == L7_SVL) )
    {
      if (usmDbDot1dTpAgingTimeGet(USMDB_UNIT_CURRENT, &dot1dTpData.dot1dTpAgingTime) == L7_SUCCESS)
        SET_VALID(I_dot1dTpAgingTime, dot1dTpData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTpData.valid))
    return(NULL);

  return(&dot1dTpData);
}

#ifdef SETS
int
k_dot1dTp_test(ObjectInfo *object, ObjectSyntax *value,
               doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_dot1dTp_ready(ObjectInfo *object, ObjectSyntax *value, 
                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_dot1dTp_set(dot1dTp_t *data,
              ContextInfo *contextInfo, int function)
{

  L7_FDB_TYPE_t vlType;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  /* Ensure SVL is supported */
  if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) != L7_SUCCESS) || (vlType != L7_SVL) )
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_dot1dTpAgingTime, data->valid))
  {
    if(usmDbDot1dTpAgingTimeSet(USMDB_UNIT_CURRENT, data->dot1dTpAgingTime) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return(NO_ERROR);    
}

  #ifdef SR_dot1dTp_UNDO
/* add #define SR_dot1dTp_UNDO in sitedefs.h to
 * include the undo routine for the dot1dTp family.
 */
int
dot1dTp_undo(doList_t *doHead, doList_t *doCur,
             ContextInfo *contextInfo)
{
  dot1dTp_t *data = (dot1dTp_t *) doCur->data;
  dot1dTp_t *undodata = (dot1dTp_t *) doCur->undodata;
  dot1dTp_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dTp_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_dot1dTp_UNDO */

#endif /* SETS */

dot1dTpFdbEntry_t *
k_dot1dTpFdbEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      OctetString * dot1dTpFdbAddress)
{
  static dot1dTpFdbEntry_t dot1dTpFdbEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_FDB_TYPE_t vlType;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1dTpFdbEntryData.dot1dTpFdbAddress = MakeOctetString(NULL, 0);
  }

  /* Ensure SVL is supported */
  if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) != L7_SUCCESS) ||
       (vlType != L7_SVL) )
    return(NULL);

  ZERO_VALID(dot1dTpFdbEntryData.valid);
/* lvl7_@p0375 start */
  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1dTpFdbAddress->octet_ptr, dot1dTpFdbAddress->length);
  SET_VALID(I_dot1dTpFdbAddress, dot1dTpFdbEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1dTpFdbEntryGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) :
         ( (usmDbDot1dTpFdbEntryGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) &&
           (usmDbDot1dTpFdbEntryNextGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetString(&dot1dTpFdbEntryData.dot1dTpFdbAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE))
  {
    ZERO_VALID(dot1dTpFdbEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1dTpFdbAddress:
    break;

  case I_dot1dTpFdbPort:
    if (snmpDot1dTpFdbPortGet(USMDB_UNIT_CURRENT, dot1dTpFdbEntryData.dot1dTpFdbAddress->octet_ptr,
                              &dot1dTpFdbEntryData.dot1dTpFdbPort) == L7_SUCCESS)
      SET_VALID(I_dot1dTpFdbPort, dot1dTpFdbEntryData.valid);
    break;

  case I_dot1dTpFdbStatus:
    if (snmpDot1dTpFdbStatusGet(USMDB_UNIT_CURRENT, dot1dTpFdbEntryData.dot1dTpFdbAddress->octet_ptr,
                                &dot1dTpFdbEntryData.dot1dTpFdbStatus) == L7_SUCCESS)

      SET_VALID(I_dot1dTpFdbStatus, dot1dTpFdbEntryData.valid);
    break;
/* lvl7_@p0375 end */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTpFdbEntryData.valid))
    return(NULL);

  return(&dot1dTpFdbEntryData);
}

dot1dTpPortEntry_t *
k_dot1dTpPortEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 dot1dTpPort)
{
  static dot1dTpPortEntry_t dot1dTpPortEntryData;
/* lvl7_@p1299 start */

  ZERO_VALID(dot1dTpPortEntryData.valid);
  dot1dTpPortEntryData.dot1dTpPort = dot1dTpPort;
  SET_VALID(I_dot1dTpPort, dot1dTpPortEntryData.valid);

  if ( searchType == EXACT ? ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort) != L7_SUCCESS) :
       ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort) != L7_SUCCESS &&
         snmpDot1dBasePortEntryNextGet(USMDB_UNIT_CURRENT, &dot1dTpPortEntryData.dot1dTpPort) != L7_SUCCESS ) )
  {
    ZERO_VALID(dot1dTpPortEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1dTpPort:
    break;

  case I_dot1dTpPortMaxInfo:
    if (usmDbDot1dTpPortMaxInfoGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort, 
                                   &dot1dTpPortEntryData.dot1dTpPortMaxInfo) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortMaxInfo, dot1dTpPortEntryData.valid);
    break;

  case I_dot1dTpPortInFrames:
    if (usmDbDot1dTpPortInFramesGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort, 
                                    &dot1dTpPortEntryData.dot1dTpPortInFrames) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortInFrames, dot1dTpPortEntryData.valid);
    break;

  case I_dot1dTpPortOutFrames:
    if (usmDbDot1dTpPortOutFramesGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort, 
                                     &dot1dTpPortEntryData.dot1dTpPortOutFrames) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortOutFrames, dot1dTpPortEntryData.valid);
    break;

  case I_dot1dTpPortInDiscards:
    if (usmDbDot1dTpPortInDiscardsGet(USMDB_UNIT_CURRENT, dot1dTpPortEntryData.dot1dTpPort, 
                                      &dot1dTpPortEntryData.dot1dTpPortInDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortInDiscards, dot1dTpPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTpPortEntryData.valid))
    return(NULL);

  return(&dot1dTpPortEntryData);
}

dot1dTpHCPortEntry_t *
k_dot1dTpHCPortEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 dot1dTpPort)
{
  static dot1dTpHCPortEntry_t dot1dTpHCPortEntryData;

  ZERO_VALID(dot1dTpHCPortEntryData.valid);
  dot1dTpHCPortEntryData.dot1dTpPort = dot1dTpPort;
  SET_VALID(I_dot1dTpHCPortEntryIndex_dot1dTpPort, dot1dTpHCPortEntryData.valid);

  if ( searchType == EXACT ? ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpHCPortEntryData.dot1dTpPort) != L7_SUCCESS) :
       ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpHCPortEntryData.dot1dTpPort) != L7_SUCCESS &&
         snmpDot1dBasePortEntryNextGet(USMDB_UNIT_CURRENT, &dot1dTpHCPortEntryData.dot1dTpPort) != L7_SUCCESS ) )
  {
    ZERO_VALID(dot1dTpHCPortEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1dTpHCPortEntryIndex_dot1dTpPort:
    break;


  case I_dot1dTpHCPortInFrames:
    if ( (dot1dTpHCPortEntryData.dot1dTpHCPortInFrames != NULL ||
          (dot1dTpHCPortEntryData.dot1dTpHCPortInFrames = MakeCounter64(0)) != NULL) &&
         dot1dTpHCPortInFramesGet(USMDB_UNIT_CURRENT, dot1dTpHCPortEntryData.dot1dTpPort,
                                  &dot1dTpHCPortEntryData.dot1dTpHCPortInFrames->big_end,
                                  &dot1dTpHCPortEntryData.dot1dTpHCPortInFrames->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1dTpHCPortInFrames, dot1dTpHCPortEntryData.valid);
    break;

  case I_dot1dTpHCPortInDiscards:
    if ( (dot1dTpHCPortEntryData.dot1dTpHCPortInDiscards != NULL ||
          (dot1dTpHCPortEntryData.dot1dTpHCPortInDiscards = MakeCounter64(0)) != NULL) &&
         dot1dTpHCPortInDiscardsGet(USMDB_UNIT_CURRENT, dot1dTpHCPortEntryData.dot1dTpPort,
                                    &dot1dTpHCPortEntryData.dot1dTpHCPortInDiscards->big_end,
                                    &dot1dTpHCPortEntryData.dot1dTpHCPortInDiscards->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1dTpHCPortInDiscards, dot1dTpHCPortEntryData.valid);
    break;

  case I_dot1dTpHCPortOutFrames:
    if ( (dot1dTpHCPortEntryData.dot1dTpHCPortOutFrames != NULL ||
          (dot1dTpHCPortEntryData.dot1dTpHCPortOutFrames = MakeCounter64(0)) != NULL) &&
         dot1dTpHCPortOutFramesGet(USMDB_UNIT_CURRENT, dot1dTpHCPortEntryData.dot1dTpPort,
                                   &dot1dTpHCPortEntryData.dot1dTpHCPortOutFrames->big_end,
                                   &dot1dTpHCPortEntryData.dot1dTpHCPortOutFrames->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1dTpHCPortOutFrames, dot1dTpHCPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTpHCPortEntryData.valid))
    return(NULL);

  return(&dot1dTpHCPortEntryData);
}

dot1dTpPortOverflowEntry_t *
k_dot1dTpPortOverflowEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 dot1dTpPort)
{
  static dot1dTpPortOverflowEntry_t dot1dTpPortOverflowEntryData;

  ZERO_VALID(dot1dTpPortOverflowEntryData.valid);
  dot1dTpPortOverflowEntryData.dot1dTpPort = dot1dTpPort;
  SET_VALID(I_dot1dTpPortOverflowEntryIndex_dot1dTpPort, dot1dTpPortOverflowEntryData.valid);

  if ( searchType == EXACT ? ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpPortOverflowEntryData.dot1dTpPort) != L7_SUCCESS) :
       ( snmpDot1dBasePortEntryGet(USMDB_UNIT_CURRENT, dot1dTpPortOverflowEntryData.dot1dTpPort) != L7_SUCCESS &&
         snmpDot1dBasePortEntryNextGet(USMDB_UNIT_CURRENT, &dot1dTpPortOverflowEntryData.dot1dTpPort) != L7_SUCCESS ) )
  {
    ZERO_VALID(dot1dTpPortOverflowEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1dTpPortOverflowEntryIndex_dot1dTpPort:
    break;

  case I_dot1dTpPortInOverflowFrames:
    if (dot1dTpPortInOverflowFramesGet(USMDB_UNIT_CURRENT, dot1dTpPortOverflowEntryData.dot1dTpPort,
                                       &dot1dTpPortOverflowEntryData.dot1dTpPortInOverflowFrames) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortInOverflowFrames, dot1dTpPortOverflowEntryData.valid);
    break;

  case I_dot1dTpPortInOverflowDiscards:
    if (dot1dTpPortInOverflowDiscardsGet(USMDB_UNIT_CURRENT, dot1dTpPortOverflowEntryData.dot1dTpPort,
                                         &dot1dTpPortOverflowEntryData.dot1dTpPortInOverflowDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortInOverflowDiscards, dot1dTpPortOverflowEntryData.valid);
    break;

  case I_dot1dTpPortOutOverflowFrames:
    if (dot1dTpPortOutOverflowFramesGet(USMDB_UNIT_CURRENT, dot1dTpPortOverflowEntryData.dot1dTpPort,
                                        &dot1dTpPortOverflowEntryData.dot1dTpPortOutOverflowFrames) == L7_SUCCESS)
      SET_VALID(I_dot1dTpPortOutOverflowFrames, dot1dTpPortOverflowEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTpPortOverflowEntryData.valid))
    return(NULL);

  return(&dot1dTpPortOverflowEntryData);
}

dot1dStaticEntry_t *
k_dot1dStaticEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       OctetString * dot1dStaticAddress,
                       SR_INT32 dot1dStaticReceivePort)
{
#ifdef NOT_SUPPORTED
  static dot1dStaticEntry_t dot1dStaticEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1dStaticEntryData.dot1dStaticAddress = MakeOctetString(NULL, 0);
    dot1dStaticEntryData.dot1dStaticAllowedToGoTo = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1dStaticEntryData.valid);
  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1dStaticAddress->octet_ptr, dot1dStaticAddress->length);
  SET_VALID(I_dot1dStaticAddress, dot1dStaticEntryData.valid);
  dot1dStaticEntryData.dot1dStaticReceivePort = dot1dStaticReceivePort;
  SET_VALID(I_dot1dStaticReceivePort, dot1dStaticEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1dStaticEntryGet(USMDB_UNIT_CURRENT, snmp_buffer, dot1dStaticEntryData.dot1dStaticReceivePort) != L7_SUCCESS) :
         ( (usmDbDot1dStaticEntryGet(USMDB_UNIT_CURRENT, snmp_buffer, dot1dStaticEntryData.dot1dStaticReceivePort) != L7_SUCCESS) &&
           (usmDbDot1dStaticEntryNextGet(USMDB_UNIT_CURRENT, snmp_buffer, &dot1dStaticEntryData.dot1dStaticReceivePort) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetStringFromText(&dot1dStaticEntryData.dot1dStaticAddress, snmp_buffer) == L7_TRUE))
  {
    ZERO_VALID(dot1dStaticEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1dStaticAddress:
  case I_dot1dStaticReceivePort:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dStaticAllowedToGoTo:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1dStaticAllowedToGoToGet(USMDB_UNIT_CURRENT, dot1dStaticEntryData.dot1dStaticAddress->octet_ptr,
                                         dot1dStaticEntryData.dot1dStaticReceivePort, 
                                         snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1dStaticEntryData.dot1dStaticAllowedToGoTo, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1dStaticAllowedToGoTo, dot1dStaticEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dStaticStatus:
    if (usmDbDot1dStaticStatusGet(USMDB_UNIT_CURRENT, dot1dStaticEntryData.dot1dStaticAddress->octet_ptr,
                                  dot1dStaticEntryData.dot1dStaticReceivePort, 
                                  &dot1dStaticEntryData.dot1dStaticStatus) == L7_SUCCESS)
      SET_VALID(I_dot1dStaticStatus, dot1dStaticEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dStaticEntryData.valid))
    return(NULL);

  return(&dot1dStaticEntryData);
#else  /* NOT_SUPPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */
}

#ifdef SETS
int
k_dot1dStaticEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_dot1dStaticEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_dot1dStaticEntry_set_defaults(doList_t *dp)
{
  dot1dStaticEntry_t *data = (dot1dStaticEntry_t *) (dp->data);

  if ((data->dot1dStaticAllowedToGoTo = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }

  SET_ALL_VALID(data->valid);
  return(NO_ERROR);
}

int
k_dot1dStaticEntry_set(dot1dStaticEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1dStaticEntryGet(USMDB_UNIT_CURRENT, data->dot1dStaticAddress->octet_ptr,
                               data->dot1dStaticReceivePort) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_dot1dStaticAllowedToGoTo, data->valid))
  {
    if(usmDbDot1dStaticAllowedToGoToSet(USMDB_UNIT_CURRENT, data->dot1dStaticAddress->octet_ptr,
                                       data->dot1dStaticReceivePort,
                                       data->dot1dStaticAllowedToGoTo->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_dot1dStaticAllowedToGoTo, tempValid);
    }
  }

  if (VALID(I_dot1dStaticStatus, data->valid))
  {
    if(usmDbDot1dStaticStatusSet(USMDB_UNIT_CURRENT, data->dot1dStaticAddress->octet_ptr,
                                data->dot1dStaticReceivePort,
                                data->dot1dStaticStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return(NO_ERROR);
#else  /* NOT_SUPPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
}

  #ifdef SR_dot1dStaticEntry_UNDO
/* add #define SR_dot1dStaticEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1dStaticEntry family.
 */
int
dot1dStaticEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  dot1dStaticEntry_t *data = (dot1dStaticEntry_t *) doCur->data;
  dot1dStaticEntry_t *undodata = (dot1dStaticEntry_t *) doCur->undodata;
  dot1dStaticEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dStaticEntry_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_dot1dStaticEntry_UNDO */

#endif /* SETS */

dot1dExtBase_t *
k_dot1dExtBase_get(int serialNum, ContextInfo *contextInfo, int nominator)
{
  static dot1dExtBase_t dot1dExtBaseData;
  L7_char8 snmp_buffer;

  /*
  * if ( nominator != -1 ) condition is added to all the case statements
  * to support the undo functionality for getting all the values.
  */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dDeviceCapabilities:
    if (usmDbDot1dDeviceCapabilitiesGet(USMDB_UNIT_CURRENT, &snmp_buffer) == L7_SUCCESS)
    {
      if(L7_TRUE == SafeMakeOctetString(&dot1dExtBaseData.dot1dDeviceCapabilities, &snmp_buffer, 1))
        SET_VALID(I_dot1dDeviceCapabilities, dot1dExtBaseData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dTrafficClassesEnabled:
    if (snmpDot1dTrafficClassesEnabledGet(USMDB_UNIT_CURRENT, &dot1dExtBaseData.dot1dTrafficClassesEnabled) == L7_SUCCESS)
      SET_VALID(I_dot1dTrafficClassesEnabled, dot1dExtBaseData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dGmrpStatus:
    if (snmpDot1dGmrpStatusGet(USMDB_UNIT_CURRENT, &dot1dExtBaseData.dot1dGmrpStatus) == L7_SUCCESS)
      SET_VALID(I_dot1dGmrpStatus, dot1dExtBaseData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ((nominator >= 0) && !VALID(nominator, dot1dExtBaseData.valid))
    return(NULL);

  return(&dot1dExtBaseData);
}

#ifdef SETS
int
k_dot1dExtBase_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1dExtBase_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1dExtBase_set(dot1dExtBase_t *data,
                   ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_dot1dDeviceCapabilities, data->valid))
  {
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot1dDeviceCapabilities->octet_ptr, data->dot1dDeviceCapabilities->length);
    if (usmDbDot1dDeviceCapabilitiesSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_dot1dDeviceCapabilities, tempValid);
    }

  }

  if (VALID(I_dot1dTrafficClassesEnabled, data->valid))
  {
    if(snmpDot1dTrafficClassesEnabledSet(USMDB_UNIT_CURRENT, data->dot1dTrafficClassesEnabled) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_dot1dTrafficClassesEnabled, tempValid);
    }
  }

  if (VALID(I_dot1dGmrpStatus, data->valid))
  {
    if(snmpDot1dGmrpStatusSet(USMDB_UNIT_CURRENT, data->dot1dGmrpStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

  #ifdef SR_dot1dExtBase_UNDO
/* add #define SR_dot1dExtBase_UNDO in sitedefs.h to
 * include the undo routine for the dot1dExtBase family.
 */
int
dot1dExtBase_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
  dot1dExtBase_t *data = (dot1dExtBase_t *) doCur->data;
  dot1dExtBase_t *undodata = (dot1dExtBase_t *) doCur->undodata;
  dot1dExtBase_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dExtBase_set(setdata, contextInfo, function) == NO_ERROR)
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot1dExtBase_UNDO */

#endif /* SETS */

dot1dUserPriorityRegenEntry_t *
k_dot1dUserPriorityRegenEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 dot1dBasePort,
                                  SR_INT32 dot1dUserPriority)
{
/*lvl7_@p0616 start */
#ifdef NOT_SUPPORTED
  static dot1dUserPriorityRegenEntry_t dot1dUserPriorityRegenEntryData;

  ZERO_VALID(dot1dUserPriorityRegenEntryData.valid);
  dot1dUserPriorityRegenEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1dUserPriorityRegenEntryIndex_dot1dBasePort, dot1dUserPriorityRegenEntryData.valid);
  dot1dUserPriorityRegenEntryData.dot1dUserPriority = dot1dUserPriority;
  SET_VALID(I_dot1dUserPriority, dot1dUserPriorityRegenEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1dUserPriorityRegenEntryGet(USMDB_UNIT_CURRENT, dot1dUserPriorityRegenEntryData.dot1dBasePort,
                                            dot1dUserPriorityRegenEntryData.dot1dUserPriority) != L7_SUCCESS) :
       ( (usmDbDot1dUserPriorityRegenEntryGet(USMDB_UNIT_CURRENT, dot1dUserPriorityRegenEntryData.dot1dBasePort,
                                              dot1dUserPriorityRegenEntryData.dot1dUserPriority) != L7_SUCCESS) &&
         (usmDbDot1dUserPriorityRegenEntryNextGet(USMDB_UNIT_CURRENT, &dot1dUserPriorityRegenEntryData.dot1dBasePort,
                                                  &dot1dUserPriorityRegenEntryData.dot1dUserPriority) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1dUserPriorityRegenEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1dUserPriorityRegenEntryIndex_dot1dBasePort:
  case I_dot1dUserPriority:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dRegenUserPriority:
    if (usmDbDot1dRegenUserPriorityGet(USMDB_UNIT_CURRENT, dot1dUserPriorityRegenEntryData.dot1dBasePort,
                                       dot1dUserPriorityRegenEntryData.dot1dUserPriority,
                                       &dot1dUserPriorityRegenEntryData.dot1dRegenUserPriority) == L7_SUCCESS)
      SET_VALID(I_dot1dRegenUserPriority, dot1dUserPriorityRegenEntryData.valid);
      break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dUserPriorityRegenEntryData.valid))
    return(NULL);

  return(&dot1dUserPriorityRegenEntryData);
#else /* NOT_SUPPORTED  */
  return(NULL);
#endif /* NOT_SUPPORTED  */

/* lvl7_@p0616 end  */
}

#ifdef SETS
int
k_dot1dUserPriorityRegenEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1dUserPriorityRegenEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1dUserPriorityRegenEntry_set_defaults(doList_t *dp)
{
  dot1dUserPriorityRegenEntry_t *data = (dot1dUserPriorityRegenEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1dUserPriorityRegenEntry_set(dot1dUserPriorityRegenEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{
/*lvl7_@p0616 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1dUserPriorityRegenEntryGet(USMDB_UNIT_CURRENT, data->dot1dBasePort,
                                          data->dot1dUserPriority) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1dRegenUserPriority, data->valid))
  {
    if(usmDbDot1dRegenUserPrioritySet(USMDB_UNIT_CURRENT, data->dot1dBasePort,
                                     data->dot1dUserPriority,
                                     data->dot1dRegenUserPriority) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0616 end */
}

  #ifdef SR_dot1dUserPriorityRegenEntry_UNDO
/* add #define SR_dot1dUserPriorityRegenEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1dUserPriorityRegenEntry family.
 */
int
dot1dUserPriorityRegenEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1dUserPriorityRegenEntry_t *data = (dot1dUserPriorityRegenEntry_t *) doCur->data;
  dot1dUserPriorityRegenEntry_t *undodata = (dot1dUserPriorityRegenEntry_t *) doCur->undodata;
  dot1dUserPriorityRegenEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dUserPriorityRegenEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
}
  #endif /* SR_dot1dUserPriorityRegenEntry_UNDO */

#endif /* SETS */

dot1dTrafficClassEntry_t *
k_dot1dTrafficClassEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 dot1dBasePort,
                             SR_INT32 dot1dTrafficClassPriority)
{
  static dot1dTrafficClassEntry_t dot1dTrafficClassEntryData;

  ZERO_VALID(dot1dTrafficClassEntryData.valid);
  dot1dTrafficClassEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1dTrafficClassEntryIndex_dot1dBasePort, dot1dTrafficClassEntryData.valid);
  dot1dTrafficClassEntryData.dot1dTrafficClassPriority = dot1dTrafficClassPriority;
  SET_VALID(I_dot1dTrafficClassPriority, dot1dTrafficClassEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1dTrafficClassEntryGet(USMDB_UNIT_CURRENT, dot1dTrafficClassEntryData.dot1dBasePort,
                                       dot1dTrafficClassEntryData.dot1dTrafficClassPriority) != L7_SUCCESS) :
       ( (usmDbDot1dTrafficClassEntryGet(USMDB_UNIT_CURRENT, dot1dTrafficClassEntryData.dot1dBasePort,
                                         dot1dTrafficClassEntryData.dot1dTrafficClassPriority) != L7_SUCCESS) &&
         (usmDbDot1dTrafficClassEntryNextGet(USMDB_UNIT_CURRENT, &dot1dTrafficClassEntryData.dot1dBasePort,
                                             &dot1dTrafficClassEntryData.dot1dTrafficClassPriority) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1dTrafficClassEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1dTrafficClassEntryIndex_dot1dBasePort:
  case I_dot1dTrafficClassPriority:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1dTrafficClass:
    if (usmDbDot1dTrafficClassGet(USMDB_UNIT_CURRENT, dot1dTrafficClassEntryData.dot1dBasePort,
                                  dot1dTrafficClassEntryData.dot1dTrafficClassPriority,
                                  &dot1dTrafficClassEntryData.dot1dTrafficClass) == L7_SUCCESS)
      SET_VALID(I_dot1dTrafficClass, dot1dTrafficClassEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dTrafficClassEntryData.valid))
    return(NULL);

  return(&dot1dTrafficClassEntryData);
}

#ifdef SETS
int
k_dot1dTrafficClassEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1dTrafficClassEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1dTrafficClassEntry_set_defaults(doList_t *dp)
{
  dot1dTrafficClassEntry_t *data = (dot1dTrafficClassEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1dTrafficClassEntry_set(dot1dTrafficClassEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1dTrafficClassEntryGet(USMDB_UNIT_CURRENT, data->dot1dBasePort, data->dot1dTrafficClassPriority) != L7_SUCCESS ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1dTrafficClass, data->valid))
  {
    if(usmDbDot1dTrafficClassSet(USMDB_UNIT_CURRENT, data->dot1dBasePort,
                                data->dot1dTrafficClassPriority,
                                data->dot1dTrafficClass) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

  #ifdef SR_dot1dTrafficClassEntry_UNDO
/* add #define SR_dot1dTrafficClassEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1dTrafficClassEntry family.
 */
int
dot1dTrafficClassEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  dot1dTrafficClassEntry_t *data = (dot1dTrafficClassEntry_t *) doCur->data;
  dot1dTrafficClassEntry_t *undodata = (dot1dTrafficClassEntry_t *) doCur->undodata;
  dot1dTrafficClassEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1dTrafficClassEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot1dTrafficClassEntry_UNDO */

#endif /* SETS */

dot1dPortOutboundAccessPriorityEntry_t *
k_dot1dPortOutboundAccessPriorityEntry_get(int serialNum, ContextInfo *contextInfo,
                                           int nominator,
                                           int searchType,
                                           SR_INT32 dot1dBasePort,
                                           SR_INT32 dot1dRegenUserPriority)
{
/*lvl7_@p618 start */
#ifdef NOT_SUPPORTED
  static dot1dPortOutboundAccessPriorityEntry_t dot1dPortOutboundAccessPriorityEntryData;

  ZERO_VALID(dot1dPortOutboundAccessPriorityEntryData.valid);
  dot1dPortOutboundAccessPriorityEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1dPortOutboundAccessPriorityEntryIndex_dot1dBasePort, dot1dPortOutboundAccessPriorityEntryData.valid);
  dot1dPortOutboundAccessPriorityEntryData.dot1dRegenUserPriority = dot1dRegenUserPriority;
  SET_VALID(I_dot1dPortOutboundAccessPriorityEntryIndex_dot1dRegenUserPriority, dot1dPortOutboundAccessPriorityEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1dPortOutboundAccessPriorityEntryGet(USMDB_UNIT_CURRENT, dot1dPortOutboundAccessPriorityEntryData.dot1dBasePort,
                                                     dot1dPortOutboundAccessPriorityEntryData.dot1dRegenUserPriority) != L7_SUCCESS) :
       ( (usmDbDot1dPortOutboundAccessPriorityEntryGet(USMDB_UNIT_CURRENT, dot1dPortOutboundAccessPriorityEntryData.dot1dBasePort,
                                                       dot1dPortOutboundAccessPriorityEntryData.dot1dRegenUserPriority) != L7_SUCCESS) &&
         (usmDbDot1dPortOutboundAccessPriorityEntryNextGet(USMDB_UNIT_CURRENT, &dot1dPortOutboundAccessPriorityEntryData.dot1dBasePort,
                                                           &dot1dPortOutboundAccessPriorityEntryData.dot1dRegenUserPriority) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1dPortOutboundAccessPriorityEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1dPortOutboundAccessPriorityEntryIndex_dot1dBasePort:
  case I_dot1dPortOutboundAccessPriorityEntryIndex_dot1dRegenUserPriority:
    break;

  case I_dot1dPortOutboundAccessPriority:
    if (usmDbDot1dPortOutboundAccessPriorityGet(USMDB_UNIT_CURRENT, dot1dPortOutboundAccessPriorityEntryData.dot1dBasePort,
                                                dot1dPortOutboundAccessPriorityEntryData.dot1dRegenUserPriority,
                                                &dot1dPortOutboundAccessPriorityEntryData.dot1dPortOutboundAccessPriority) == L7_SUCCESS)
      SET_VALID(I_dot1dPortOutboundAccessPriority, dot1dPortOutboundAccessPriorityEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1dPortOutboundAccessPriorityEntryData.valid))
    return(NULL);

  return(&dot1dPortOutboundAccessPriorityEntryData);
#else  /*NOT_SUPPORTED */
  return(NULL);
#endif  /*NOT_SUPPORTED */

/*lvl7_@p618 end */
}

dot1qBase_t *
k_dot1qBase_get(int serialNum, ContextInfo *contextInfo,
                int nominator)
{
  static dot1qBase_t dot1qBaseData;

  ZERO_VALID(dot1qBaseData.valid);

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanVersionNumber:
    if (snmpDot1qVlanVersionNumberGet(USMDB_UNIT_CURRENT, &dot1qBaseData.dot1qVlanVersionNumber) == L7_SUCCESS)
      SET_VALID(I_dot1qVlanVersionNumber, dot1qBaseData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qMaxVlanId:
    if (usmDbDot1qMaxVlanIdGet(USMDB_UNIT_CURRENT, &dot1qBaseData.dot1qMaxVlanId) == L7_SUCCESS)
      SET_VALID(I_dot1qMaxVlanId, dot1qBaseData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qMaxSupportedVlans:
    if (usmDbDot1qMaxSupportedVlansGet(USMDB_UNIT_CURRENT, &dot1qBaseData.dot1qMaxSupportedVlans) == L7_SUCCESS)
      SET_VALID(I_dot1qMaxSupportedVlans, dot1qBaseData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qNumVlans:
    if (usmDbDot1qNumVlansGet(USMDB_UNIT_CURRENT, &dot1qBaseData.dot1qNumVlans) == L7_SUCCESS)
      SET_VALID(I_dot1qNumVlans, dot1qBaseData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qGvrpStatus:
    if (snmpDot1qGvrpStatusGet(USMDB_UNIT_CURRENT, &dot1qBaseData.dot1qGvrpStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qGvrpStatus, dot1qBaseData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qBaseData.valid))
    return(NULL);

  return(&dot1qBaseData);
}

#ifdef SETS
int
k_dot1qBase_test(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qBase_ready(ObjectInfo *object, ObjectSyntax *value, 
                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qBase_set(dot1qBase_t *data,
                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_dot1qGvrpStatus, data->valid))
  {
    if(snmpDot1qGvrpStatusSet(USMDB_UNIT_CURRENT, data->dot1qGvrpStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

  #ifdef SR_dot1qBase_UNDO
/* add #define SR_dot1qBase_UNDO in sitedefs.h to
 * include the undo routine for the dot1qBase family.
 */
int
dot1qBase_undo(doList_t *doHead, doList_t *doCur,
               ContextInfo *contextInfo)
{
  dot1qBase_t *data = (dot1qBase_t *) doCur->data;
  dot1qBase_t *undodata = (dot1qBase_t *) doCur->undodata;
  dot1qBase_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qBase_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_dot1qBase_UNDO */

#endif /* SETS */

dot1qFdbEntry_t *
k_dot1qFdbEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 dot1qFdbId)
{
  static dot1qFdbEntry_t dot1qFdbEntryData;
  L7_FDB_TYPE_t vlType;

  /* Ensure IVL is supported */
  if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) != L7_SUCCESS) ||
       (vlType != L7_IVL) )
    return(NULL);

  ZERO_VALID(dot1qFdbEntryData.valid);
  dot1qFdbEntryData.dot1qFdbId = dot1qFdbId;
  SET_VALID(I_dot1qFdbId, dot1qFdbEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbFdbIdGet(USMDB_UNIT_CURRENT, dot1qFdbEntryData.dot1qFdbId) != L7_SUCCESS) :
       ( (usmDbFdbIdGet(USMDB_UNIT_CURRENT, dot1qFdbEntryData.dot1qFdbId) != L7_SUCCESS) &&
         (usmDbFdbIdNextGet(USMDB_UNIT_CURRENT, dot1qFdbEntryData.dot1qFdbId, &dot1qFdbEntryData.dot1qFdbId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qFdbEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qFdbId:
    break;

  case I_dot1qFdbDynamicCount:
    if (usmDbDot1qFdbDynamicCountGet(USMDB_UNIT_CURRENT, dot1qFdbEntryData.dot1qFdbId,
                                     &dot1qFdbEntryData.dot1qFdbDynamicCount) == L7_SUCCESS)
      SET_VALID(I_dot1qFdbDynamicCount, dot1qFdbEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qFdbEntryData.valid))
    return(NULL);

  return(&dot1qFdbEntryData);
}

dot1qTpFdbEntry_t *
k_dot1qTpFdbEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_UINT32 dot1qFdbId,
                      OctetString * dot1qTpFdbAddress)
{
  static dot1qTpFdbEntry_t dot1qTpFdbEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_FDB_TYPE_t vlType;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qTpFdbEntryData.dot1qTpFdbAddress = MakeOctetString(NULL, 0);
  }

  /* Ensure IVL is supported */
  if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) != L7_SUCCESS) ||
       (vlType != L7_IVL) )
    return(NULL);

  ZERO_VALID(dot1qTpFdbEntryData.valid);
  dot1qTpFdbEntryData.dot1qFdbId = dot1qFdbId;
  SET_VALID(I_dot1qTpFdbEntryIndex_dot1qFdbId, dot1qTpFdbEntryData.valid);

  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1qTpFdbAddress->octet_ptr, dot1qTpFdbAddress->length);
  SET_VALID(I_dot1qTpFdbAddress, dot1qTpFdbEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1qTpFdbEntryGet(USMDB_UNIT_CURRENT, dot1qTpFdbEntryData.dot1qFdbId, snmp_buffer) != L7_SUCCESS) :
         ( (usmDbDot1qTpFdbEntryGet(USMDB_UNIT_CURRENT, dot1qTpFdbEntryData.dot1qFdbId, snmp_buffer) != L7_SUCCESS) &&
           (usmDbDot1qTpFdbEntryNextGet(USMDB_UNIT_CURRENT, &dot1qTpFdbEntryData.dot1qFdbId, snmp_buffer) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetString(&dot1qTpFdbEntryData.dot1qTpFdbAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE) )
  {
    ZERO_VALID(dot1qTpFdbEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qTpFdbEntryIndex_dot1qFdbId:
  case I_dot1qTpFdbAddress:
    break;

  case I_dot1qTpFdbPort:
    if (usmDbDot1qTpFdbPortGet(USMDB_UNIT_CURRENT, dot1qTpFdbEntryData.dot1qFdbId,
                               dot1qTpFdbEntryData.dot1qTpFdbAddress->octet_ptr,
                               &dot1qTpFdbEntryData.dot1qTpFdbPort) == L7_SUCCESS)
      SET_VALID(I_dot1qTpFdbPort, dot1qTpFdbEntryData.valid);
    break;

  case I_dot1qTpFdbStatus:
    if (snmpDot1qTpFdbStatusGet(USMDB_UNIT_CURRENT, dot1qTpFdbEntryData.dot1qFdbId,
                                dot1qTpFdbEntryData.dot1qTpFdbAddress->octet_ptr,
                                &dot1qTpFdbEntryData.dot1qTpFdbStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qTpFdbStatus, dot1qTpFdbEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qTpFdbEntryData.valid))
    return(NULL);

  return(&dot1qTpFdbEntryData);
}

dot1qTpGroupEntry_t *
k_dot1qTpGroupEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_UINT32 dot1qVlanIndex,
                        OctetString * dot1qTpGroupAddress)
{
  static dot1qTpGroupEntry_t dot1qTpGroupEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qTpGroupEntryData.dot1qTpGroupAddress = MakeOctetString(NULL, 0);
    dot1qTpGroupEntryData.dot1qTpGroupEgressPorts = MakeOctetString(NULL, 0);
    dot1qTpGroupEntryData.dot1qTpGroupLearnt = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qTpGroupEntryData.valid);
  dot1qTpGroupEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qTpGroupEntryIndex_dot1qVlanIndex, dot1qTpGroupEntryData.valid);
  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1qTpGroupAddress->octet_ptr, dot1qTpGroupAddress->length);
  SET_VALID(I_dot1qTpGroupAddress, dot1qTpGroupEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1qTpGroupEntryGet(USMDB_UNIT_CURRENT, dot1qTpGroupEntryData.dot1qVlanIndex, snmp_buffer) != L7_SUCCESS) :
         ( (usmDbDot1qTpGroupEntryGet(USMDB_UNIT_CURRENT, dot1qTpGroupEntryData.dot1qVlanIndex, snmp_buffer) != L7_SUCCESS) &&
           (usmDbDot1qTpGroupEntryNextGet(USMDB_UNIT_CURRENT, &dot1qTpGroupEntryData.dot1qVlanIndex, snmp_buffer) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetString(&dot1qTpGroupEntryData.dot1qTpGroupAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_FALSE) )
  {
    ZERO_VALID(dot1qTpGroupEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qTpGroupEntryIndex_dot1qVlanIndex:
  case I_dot1qTpGroupAddress:
    break;

  case I_dot1qTpGroupEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qTpGroupEgressPortsGet(USMDB_UNIT_CURRENT, dot1qTpGroupEntryData.dot1qVlanIndex,
                                        dot1qTpGroupEntryData.dot1qTpGroupAddress->octet_ptr,
                                        snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qTpGroupEntryData.dot1qTpGroupEgressPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qTpGroupEgressPorts, dot1qTpGroupEntryData.valid);
    break;

  case I_dot1qTpGroupLearnt:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qTpGroupLearntGet(USMDB_UNIT_CURRENT, dot1qTpGroupEntryData.dot1qVlanIndex,
                                   dot1qTpGroupEntryData.dot1qTpGroupAddress->octet_ptr,
                                   snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qTpGroupEntryData.dot1qTpGroupLearnt, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qTpGroupLearnt, dot1qTpGroupEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qTpGroupEntryData.valid))
    return(NULL);

  return(&dot1qTpGroupEntryData);

/* lvl7_@p0635 end */
}

dot1qForwardAllEntry_t *
k_dot1qForwardAllEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 dot1qVlanIndex)
{
/* lvl7_@p0636 start */
#ifdef NOT_SUPPORTED
  static dot1qForwardAllEntry_t dot1qForwardAllEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qForwardAllEntryData.dot1qForwardAllPorts = MakeOctetString(NULL, 0);
    dot1qForwardAllEntryData.dot1qForwardAllStaticPorts = MakeOctetString(NULL, 0);
    dot1qForwardAllEntryData.dot1qForwardAllForbiddenPorts = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qForwardAllEntryData.valid);
  dot1qForwardAllEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qForwardAllEntryIndex_dot1qVlanIndex, dot1qForwardAllEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qForwardAllEntryGet(USMDB_UNIT_CURRENT, dot1qForwardAllEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qForwardAllEntryGet(USMDB_UNIT_CURRENT, dot1qForwardAllEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qForwardAllEntryNextGet(USMDB_UNIT_CURRENT, &dot1qForwardAllEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qForwardAllEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qForwardAllEntryIndex_dot1qVlanIndex:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardAllPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qForwardAllPortsGet(USMDB_UNIT_CURRENT, dot1qForwardAllEntryData.dot1qVlanIndex,
                                     snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardAllEntryData.dot1qForwardAllPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardAllPorts, dot1qForwardAllEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardAllStaticPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qForwardAllStaticPortsGet(USMDB_UNIT_CURRENT, dot1qForwardAllEntryData.dot1qVlanIndex,
                                           snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardAllEntryData.dot1qForwardAllStaticPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardAllStaticPorts, dot1qForwardAllEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardAllForbiddenPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDdot1qForwardAllForbiddenPortsGet(USMDB_UNIT_CURRENT, dot1qForwardAllEntryData.dot1qVlanIndex,
                                               snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardAllEntryData.dot1qForwardAllForbiddenPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardAllForbiddenPorts, dot1qForwardAllEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qForwardAllEntryData.valid))
    return(NULL);

  return(&dot1qForwardAllEntryData);
#else /* NOT_SUPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0636 end */
}

#ifdef SETS
int
k_dot1qForwardAllEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qForwardAllEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qForwardAllEntry_set_defaults(doList_t *dp)
{
  dot1qForwardAllEntry_t *data = (dot1qForwardAllEntry_t *) (dp->data);

  if ((data->dot1qForwardAllPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qForwardAllStaticPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qForwardAllForbiddenPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qForwardAllEntry_set(dot1qForwardAllEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
/*lvl7_@p0636 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1qForwardAllEntryGet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1qForwardAllStaticPorts, data->valid))
  {
    if(usmDbDot1qForwardAllStaticPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                         data->dot1qForwardAllStaticPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qForwardAllStaticPorts, tempValid);
    }
  }

  if (VALID(I_dot1qForwardAllForbiddenPorts, data->valid))
  {
    if(usmDbDdot1qForwardAllForbiddenPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                             data->dot1qForwardAllForbiddenPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0636 end  */
}

  #ifdef SR_dot1qForwardAllEntry_UNDO
/* add #define SR_dot1qForwardAllEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qForwardAllEntry family.
 */
int
dot1qForwardAllEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qForwardAllEntry_t *data = (dot1qForwardAllEntry_t *) doCur->data;
  dot1qForwardAllEntry_t *undodata = (dot1qForwardAllEntry_t *) doCur->undodata;
  dot1qForwardAllEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qForwardAllEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

}
#endif /* SR_dot1qForwardAllEntry_UNDO */

#endif /* SETS */

dot1qForwardUnregisteredEntry_t *
k_dot1qForwardUnregisteredEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_UINT32 dot1qVlanIndex)
{
/* lvl7_@p0637 start */
#ifdef NOT_SUPPORTED
  static dot1qForwardUnregisteredEntry_t dot1qForwardUnregisteredEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredPorts = MakeOctetString(NULL, 0);
    dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredStaticPorts = MakeOctetString(NULL, 0);
    dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredForbiddenPorts = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qForwardUnregisteredEntryData.valid);
  dot1qForwardUnregisteredEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qForwardUnregisteredEntryIndex_dot1qVlanIndex, dot1qForwardUnregisteredEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qForwardUnregisteredEntryGet(USMDB_UNIT_CURRENT, dot1qForwardUnregisteredEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qForwardUnregisteredEntryGet(USMDB_UNIT_CURRENT, dot1qForwardUnregisteredEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qForwardUnregisteredEntryNextGet(USMDB_UNIT_CURRENT, &dot1qForwardUnregisteredEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qForwardUnregisteredEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qForwardUnregisteredEntryIndex_dot1qVlanIndex:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardUnregisteredPorts:
    if (usmDbDot1qForwardUnregisteredPortsGet(USMDB_UNIT_CURRENT, dot1qForwardUnregisteredEntryData.dot1qVlanIndex,
                                              snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardUnregisteredPorts, dot1qForwardUnregisteredEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardUnregisteredStaticPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qForwardUnregisteredStaticPortsGet(USMDB_UNIT_CURRENT, dot1qForwardUnregisteredEntryData.dot1qVlanIndex,
                                                    snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredStaticPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardUnregisteredStaticPorts, dot1qForwardUnregisteredEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qForwardUnregisteredForbiddenPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDdot1qForwardUnregisteredForbiddenPortsGet(USMDB_UNIT_CURRENT, dot1qForwardUnregisteredEntryData.dot1qVlanIndex,
                                                        snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qForwardUnregisteredEntryData.dot1qForwardUnregisteredForbiddenPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qForwardUnregisteredForbiddenPorts, dot1qForwardUnregisteredEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qForwardUnregisteredEntryData.valid))
    return(NULL);

  return(&dot1qForwardUnregisteredEntryData);
#else  /* NOT_SUPPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0637 end */
}

#ifdef SETS
int
k_dot1qForwardUnregisteredEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qForwardUnregisteredEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                      doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qForwardUnregisteredEntry_set_defaults(doList_t *dp)
{
  dot1qForwardUnregisteredEntry_t *data = (dot1qForwardUnregisteredEntry_t *) (dp->data);

  if ((data->dot1qForwardUnregisteredPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  if ((data->dot1qForwardUnregisteredStaticPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  if ((data->dot1qForwardUnregisteredForbiddenPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qForwardUnregisteredEntry_set(dot1qForwardUnregisteredEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
/*lvl7_@p0637 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1qForwardUnregisteredEntryGet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1qForwardUnregisteredStaticPorts, data->valid))
  {
    if(usmDbDot1qForwardUnregisteredStaticPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                                  data->dot1qForwardUnregisteredStaticPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qForwardUnregisteredStaticPorts, tempValid);
    }
  }

  if (VALID(I_dot1qForwardUnregisteredForbiddenPorts, data->valid))
  {
    if(usmDbDdot1qForwardUnregisteredForbiddenPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                                      data->dot1qForwardUnregisteredForbiddenPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0637 end  */
}

  #ifdef SR_dot1qForwardUnregisteredEntry_UNDO
/* add #define SR_dot1qForwardUnregisteredEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qForwardUnregisteredEntry family.
 */
int
dot1qForwardUnregisteredEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qForwardUnregisteredEntry_t *data = (dot1qForwardUnregisteredEntry_t *) doCur->data;
  dot1qForwardUnregisteredEntry_t *undodata = (dot1qForwardUnregisteredEntry_t *) doCur->undodata;
  dot1qForwardUnregisteredEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qForwardUnregisteredEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }
  return UNDO_FAILED_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

}
  #endif /* SR_dot1qForwardUnregisteredEntry_UNDO */

#endif /* SETS */

dot1qStaticUnicastEntry_t *
k_dot1qStaticUnicastEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 dot1qFdbId,
                              OctetString * dot1qStaticUnicastAddress,
                              SR_INT32 dot1qStaticUnicastReceivePort)
{

/* lvl7_@p0638 start */
#ifdef NOT_SUPPORTED
  static dot1qStaticUnicastEntry_t dot1qStaticUnicastEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qStaticUnicastEntryData.dot1qStaticUnicastAddress = MakeOctetString(NULL, 0);
    dot1qStaticUnicastEntryData.dot1qStaticUnicastAllowedToGoTo = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qStaticUnicastEntryData.valid);
  dot1qStaticUnicastEntryData.dot1qFdbId = dot1qFdbId;
  SET_VALID(I_dot1qStaticUnicastEntryIndex_dot1qFdbId, dot1qStaticUnicastEntryData.valid);

  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1qStaticUnicastAddress->octet_ptr, dot1qStaticUnicastAddress->length);
  SET_VALID(I_dot1qStaticUnicastAddress, dot1qStaticUnicastEntryData.valid);

  dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort = dot1qStaticUnicastReceivePort;
  SET_VALID(I_dot1qStaticUnicastReceivePort, dot1qStaticUnicastEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1qStaticUnicastEntryGet(USMDB_UNIT_CURRENT, dot1qStaticUnicastEntryData.dot1qFdbId,
                                          snmp_buffer, dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort) != L7_SUCCESS) :
         ( (usmDbDot1qStaticUnicastEntryGet(USMDB_UNIT_CURRENT, dot1qStaticUnicastEntryData.dot1qFdbId,
                                            snmp_buffer, dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort) != L7_SUCCESS) &&
           (usmDbDot1qStaticUnicastEntryNextGet(USMDB_UNIT_CURRENT, &dot1qStaticUnicastEntryData.dot1qFdbId,
                                                snmp_buffer, &dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetStringFromText(&dot1qStaticUnicastEntryData.dot1qStaticUnicastAddress, snmp_buffer) == L7_FALSE) )
  {
    ZERO_VALID(dot1qStaticUnicastEntryData.valid);
    return(NULL);
  }
  else
  {
    SET_VALID(I_dot1qStaticUnicastAddress, dot1qStaticUnicastEntryData.valid);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qStaticUnicastEntryIndex_dot1qFdbId:
  case I_dot1qStaticUnicastAddress:
  case I_dot1qStaticUnicastReceivePort:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qStaticUnicastAllowedToGoTo:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qStaticUnicastAllowedToGoToGet(USMDB_UNIT_CURRENT, dot1qStaticUnicastEntryData.dot1qFdbId,
                                                dot1qStaticUnicastEntryData.dot1qStaticUnicastAddress->octet_ptr,
                                                dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort,
                                                snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qStaticUnicastEntryData.dot1qStaticUnicastAllowedToGoTo, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qStaticUnicastAllowedToGoTo, dot1qStaticUnicastEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qStaticUnicastStatus:
    if (usmDbDot1qStaticUnicastStatusGet(USMDB_UNIT_CURRENT, dot1qStaticUnicastEntryData.dot1qFdbId,
                                         dot1qStaticUnicastEntryData.dot1qStaticUnicastAddress->octet_ptr,
                                         dot1qStaticUnicastEntryData.dot1qStaticUnicastReceivePort,
                                         &dot1qStaticUnicastEntryData.dot1qStaticUnicastStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qStaticUnicastStatus, dot1qStaticUnicastEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qStaticUnicastEntryData.valid))
    return(NULL);

  return(&dot1qStaticUnicastEntryData);
#else  /* NOT_SUPPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0638 start */
}

#ifdef SETS
int
k_dot1qStaticUnicastEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qStaticUnicastEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qStaticUnicastEntry_set_defaults(doList_t *dp)
{
  dot1qStaticUnicastEntry_t *data = (dot1qStaticUnicastEntry_t *) (dp->data);

  if ((data->dot1qStaticUnicastAllowedToGoTo = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->dot1qStaticUnicastStatus = D_dot1qStaticUnicastStatus_permanent;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qStaticUnicastEntry_set(dot1qStaticUnicastEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
/*lvl7_@p0638 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1qStaticUnicastEntryGet(USMDB_UNIT_CURRENT, data->dot1qFdbId,
                                      data->dot1qStaticUnicastAddress->octet_ptr, 
                                      data->dot1qStaticUnicastReceivePort) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1qStaticUnicastAllowedToGoTo, data->valid))
  {
    if(usmDbDot1qStaticUnicastAllowedToGoToSet(USMDB_UNIT_CURRENT, data->dot1qFdbId,
                                              data->dot1qStaticUnicastAddress->octet_ptr, 
                                              data->dot1qStaticUnicastReceivePort,
                                              data->dot1qStaticUnicastAllowedToGoTo->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qStaticUnicastAllowedToGoTo, tempValid);
    }
  }

  if (VALID(I_dot1qStaticUnicastStatus, data->valid))
  {
    if(usmDbDot1qStaticUnicastStatusSet(USMDB_UNIT_CURRENT, data->dot1qFdbId,
                                       data->dot1qStaticUnicastAddress->octet_ptr, 
                                       data->dot1qStaticUnicastReceivePort,
                                       data->dot1qStaticUnicastStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0638 end  */
}

  #ifdef SR_dot1qStaticUnicastEntry_UNDO
/* add #define SR_dot1qStaticUnicastEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qStaticUnicastEntry family.
 */
int
dot1qStaticUnicastEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qStaticUnicastEntry_t *data = (dot1qStaticUnicastEntry_t *) doCur->data;
  dot1qStaticUnicastEntry_t *undodata = (dot1qStaticUnicastEntry_t *) doCur->undodata;
  dot1qStaticUnicastEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qStaticUnicastEntry_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

}
  #endif /* SR_dot1qStaticUnicastEntry_UNDO */

#endif /* SETS */

dot1qStaticMulticastEntry_t *
k_dot1qStaticMulticastEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_UINT32 dot1qVlanIndex,
                                OctetString * dot1qStaticMulticastAddress,
                                SR_INT32 dot1qStaticMulticastReceivePort)
{

/* lvl7_@p0639 start */
#ifdef NOT_SUPPORTED
  static dot1qStaticMulticastEntry_t dot1qStaticMulticastEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qStaticMulticastEntryData.dot1qStaticMulticastAddress = MakeOctetString(NULL, 0);
    dot1qStaticMulticastEntryData.dot1qStaticMulticastStaticEgressPorts = MakeOctetString(NULL, 0);
    dot1qStaticMulticastEntryData.dot1qStaticMulticastForbiddenEgressPorts = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qStaticMulticastEntryData.valid);
  dot1qStaticMulticastEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qStaticMulticastEntryIndex_dot1qVlanIndex, dot1qStaticMulticastEntryData.valid);

  bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
  memcpy(snmp_buffer, dot1qStaticMulticastAddress->octet_ptr, dot1qStaticMulticastAddress->length);
  SET_VALID(I_dot1qStaticMulticastAddress, dot1qStaticMulticastEntryData.valid);

  dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort = dot1qStaticMulticastReceivePort;
  SET_VALID(I_dot1qStaticMulticastReceivePort, dot1qStaticMulticastEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (usmDbDot1qStaticMulticastEntryGet(USMDB_UNIT_CURRENT, dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                            snmp_buffer, dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort) != L7_SUCCESS) :
         ( (usmDbDot1qStaticMulticastEntryGet(USMDB_UNIT_CURRENT, dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                              snmp_buffer, dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort) != L7_SUCCESS) &&
           (usmDbDot1qStaticMulticastEntryNextGet(USMDB_UNIT_CURRENT, &dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                                  snmp_buffer, &dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort) != L7_SUCCESS) ) ) ||
       (SafeMakeOctetStringFromText(&dot1qStaticMulticastEntryData.dot1qStaticMulticastAddress, snmp_buffer) == L7_FALSE) )
  {
    ZERO_VALID(dot1qStaticMulticastEntryData.valid);
    return(NULL);
  }
  else
  {
    SET_VALID(I_dot1qStaticMulticastAddress, dot1qStaticMulticastEntryData.valid);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qStaticMulticastEntryIndex_dot1qVlanIndex:
  case I_dot1qStaticMulticastAddress:
  case I_dot1qStaticMulticastReceivePort:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qStaticMulticastStaticEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qStaticMulticastStaticEgressPortsGet(USMDB_UNIT_CURRENT, dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                                      dot1qStaticMulticastEntryData.dot1qStaticMulticastAddress->octet_ptr,
                                                      dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort,
                                                      snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qStaticMulticastEntryData.dot1qStaticMulticastStaticEgressPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qStaticMulticastStaticEgressPorts, dot1qStaticMulticastEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qStaticMulticastForbiddenEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qStaticMulticastForbiddenEgressPortsGet(USMDB_UNIT_CURRENT, dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                                         dot1qStaticMulticastEntryData.dot1qStaticMulticastAddress->octet_ptr,
                                                         dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort,
                                                         snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qStaticMulticastEntryData.dot1qStaticMulticastForbiddenEgressPorts, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qStaticMulticastForbiddenEgressPorts, dot1qStaticMulticastEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qStaticMulticastStatus:
    if (usmDbDot1qStaticMulticastStatusGet(USMDB_UNIT_CURRENT, dot1qStaticMulticastEntryData.dot1qVlanIndex,
                                           dot1qStaticMulticastEntryData.dot1qStaticMulticastAddress->octet_ptr,
                                           dot1qStaticMulticastEntryData.dot1qStaticMulticastReceivePort,
                                           &dot1qStaticMulticastEntryData.dot1qStaticMulticastStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qStaticMulticastStatus, dot1qStaticMulticastEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qStaticMulticastEntryData.valid))
    return(NULL);

  return(&dot1qStaticMulticastEntryData);
#else  /* NOT_SUPPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0639 start */
}

#ifdef SETS
int
k_dot1qStaticMulticastEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qStaticMulticastEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qStaticMulticastEntry_set_defaults(doList_t *dp)
{
  dot1qStaticMulticastEntry_t *data = (dot1qStaticMulticastEntry_t *) (dp->data);

  if ((data->dot1qStaticMulticastStaticEgressPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qStaticMulticastForbiddenEgressPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->dot1qStaticMulticastStatus = D_dot1qStaticMulticastStatus_permanent;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qStaticMulticastEntry_set(dot1qStaticMulticastEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
/*lvl7_@p0639 start */
#ifdef NOT_SUPPORTED
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbDot1qStaticMulticastEntryGet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                        data->dot1qStaticMulticastAddress->octet_ptr,
                                        data->dot1qStaticMulticastReceivePort) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1qStaticMulticastStaticEgressPorts, data->valid))
  {
    if(usmDbDot1qStaticMulticastStaticEgressPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                                    data->dot1qStaticMulticastAddress->octet_ptr,
                                                    data->dot1qStaticMulticastReceivePort,
                                                    data->dot1qStaticMulticastStaticEgressPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qStaticMulticastStaticEgressPorts, tempValid);
    }
  }

  if (VALID(I_dot1qStaticMulticastForbiddenEgressPorts, data->valid))
  {
    if(usmDbDot1qStaticMulticastForbiddenEgressPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                                       data->dot1qStaticMulticastAddress->octet_ptr,
                                                       data->dot1qStaticMulticastReceivePort,
                                                       data->dot1qStaticMulticastForbiddenEgressPorts->octet_ptr) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qStaticMulticastForbiddenEgressPorts, tempValid);
    }
  }

  if (VALID(I_dot1qStaticMulticastStatus, data->valid))
  {
    if(usmDbDot1qStaticMulticastStatusSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                         data->dot1qStaticMulticastAddress->octet_ptr,
                                         data->dot1qStaticMulticastReceivePort,
                                         &data->dot1qStaticMulticastStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0639 end  */
}

  #ifdef SR_dot1qStaticMulticastEntry_UNDO
/* add #define SR_dot1qStaticMulticastEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qStaticMulticastEntry family.
 */
int
dot1qStaticMulticastEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qStaticMulticastEntry_t *data = (dot1qStaticMulticastEntry_t *) doCur->data;
  dot1qStaticMulticastEntry_t *undodata = (dot1qStaticMulticastEntry_t *) doCur->undodata;
  dot1qStaticMulticastEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qStaticMulticastEntry_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }
  return UNDO_FAILED_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */
}
#endif /* SR_dot1qStaticMulticastEntry_UNDO */

#endif /* SETS */

dot1qVlan_t *
k_dot1qVlan_get(int serialNum, ContextInfo *contextInfo,
                int nominator)
{
  static dot1qVlan_t dot1qVlanData;

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanNumDeletes:
    if (usmDbDot1qVlanNumDeletesGet(USMDB_UNIT_CURRENT, &dot1qVlanData.dot1qVlanNumDeletes) == L7_SUCCESS)
      SET_VALID(I_dot1qVlanNumDeletes, dot1qVlanData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qNextFreeLocalVlanIndex:
    if (usmDbDot1qNextFreeLocalVlanIndexGet(USMDB_UNIT_CURRENT, &dot1qVlanData.dot1qNextFreeLocalVlanIndex) == L7_SUCCESS)
      SET_VALID(I_dot1qNextFreeLocalVlanIndex, dot1qVlanData.valid);
#ifdef NOT_SUPPORTED
    if (nominator != -1)
    /* else pass through */
#endif /* NOT_SUPPORTED */
     break;

#ifdef NOT_SUPPORTED
  case I_dot1qConstraintSetDefault:
    if (usmDbDot1qConstraintSetDefaultGet(USMDB_UNIT_CURRENT, &dot1qVlanData.dot1qConstraintSetDefault) == L7_SUCCESS)
      SET_VALID(I_dot1qConstraintSetDefault, dot1qVlanData.valid);
    if (nominator != -1) break;

  case I_dot1qConstraintTypeDefault:
    if (usmDbDot1qConstraintTypeDefaultGet(USMDB_UNIT_CURRENT, &dot1qVlanData.dot1qConstraintTypeDefault) == L7_SUCCESS)
      SET_VALID(I_dot1qConstraintTypeDefault, dot1qVlanData.valid);
    break;
#endif /* NOT_SUPPORTED */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qVlanData.valid))
    return(NULL);

  return(&dot1qVlanData);
}

#ifdef SETS
int
k_dot1qVlan_test(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qVlan_ready(ObjectInfo *object, ObjectSyntax *value, 
                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qVlan_set(dot1qVlan_t *data,
                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_dot1qNextFreeLocalVlanIndex, data->valid))
  {
    if(usmDbDot1qNextFreeLocalVlanIndexGet(USMDB_UNIT_CURRENT,
                                          &data->dot1qNextFreeLocalVlanIndex) != L7_SUCCESS)
    {
      CLR_VALID(I_dot1qNextFreeLocalVlanIndex, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }

#ifdef NOT_SUPPORTED
  if (VALID(I_dot1qConstraintSetDefault, data->valid) &&
      usmDbDot1qConstraintSetDefaultGet(USMDB_UNIT_CURRENT,
                                        &data->dot1qConstraintSetDefault) != L7_SUCCESS)
  {
    CLR_VALID(I_dot1qConstraintSetDefault, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_dot1qConstraintTypeDefault, data->valid) &&
      usmDbDot1qConstraintTypeDefaultGet(USMDB_UNIT_CURRENT,
                                         &data->dot1qConstraintTypeDefault) != L7_SUCCESS)
  {
    CLR_VALID(I_dot1qConstraintTypeDefault, data->valid);
    return COMMIT_FAILED_ERROR;
  }
#endif /* NOT_SUPPORTED */

  return NO_ERROR;
}

  #ifdef SR_dot1qVlan_UNDO
/* add #define SR_dot1qVlan_UNDO in sitedefs.h to
 * include the undo routine for the dot1qVlan family.
 */
int
dot1qVlan_undo(doList_t *doHead, doList_t *doCur,
               ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qVlan_t *data = (dot1qVlan_t *) doCur->data;
  dot1qVlan_t *undodata = (dot1qVlan_t *) doCur->undodata;
  dot1qVlan_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qVlan_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }
#endif /* NOT_SUPPORTED */
  return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1qVlan_UNDO */

#endif /* SETS */

dot1qVlanCurrentEntry_t *
k_dot1qVlanCurrentEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_UINT32 dot1qVlanTimeMark,
                            SR_UINT32 dot1qVlanIndex)
{
  static dot1qVlanCurrentEntry_t dot1qVlanCurrentEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qVlanCurrentEntryData.dot1qVlanCurrentEgressPorts = MakeOctetString(NULL, 0);
    dot1qVlanCurrentEntryData.dot1qVlanCurrentUntaggedPorts = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qVlanCurrentEntryData.valid);

  dot1qVlanCurrentEntryData.dot1qVlanTimeMark = dot1qVlanTimeMark;
  SET_VALID(I_dot1qVlanTimeMark, dot1qVlanCurrentEntryData.valid);

  dot1qVlanCurrentEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qVlanIndex, dot1qVlanCurrentEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qVlanCurrentEntryGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                      dot1qVlanCurrentEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qVlanCurrentEntryGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                        dot1qVlanCurrentEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qVlanCurrentEntryNextGet(USMDB_UNIT_CURRENT, &dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                            &dot1qVlanCurrentEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qVlanCurrentEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qVlanTimeMark:
  case I_dot1qVlanIndex:
    break;

  case I_dot1qVlanFdbId:
    if (usmDbDot1qVlanFdbIdGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                               dot1qVlanCurrentEntryData.dot1qVlanIndex,
                               &dot1qVlanCurrentEntryData.dot1qVlanFdbId) == L7_SUCCESS)
      SET_VALID(I_dot1qVlanFdbId, dot1qVlanCurrentEntryData.valid);
    break;

  case I_dot1qVlanCurrentEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qVlanCurrentEgressPortsGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                            dot1qVlanCurrentEntryData.dot1qVlanIndex, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qVlanCurrentEntryData.dot1qVlanCurrentEgressPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qVlanCurrentEgressPorts, dot1qVlanCurrentEntryData.valid);
    break;

  case I_dot1qVlanCurrentUntaggedPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
    if (usmDbDot1qVlanCurrentUntaggedPortsGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                              dot1qVlanCurrentEntryData.dot1qVlanIndex, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qVlanCurrentEntryData.dot1qVlanCurrentUntaggedPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qVlanCurrentUntaggedPorts, dot1qVlanCurrentEntryData.valid);
    break;

  case I_dot1qVlanStatus:
    if (snmpDot1qVlanStatusGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                               dot1qVlanCurrentEntryData.dot1qVlanIndex,
                               &dot1qVlanCurrentEntryData.dot1qVlanStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qVlanStatus, dot1qVlanCurrentEntryData.valid);
    break;


  case I_dot1qVlanCreationTime:
    if (snmpDot1qVlanCreationTimeGet(USMDB_UNIT_CURRENT, dot1qVlanCurrentEntryData.dot1qVlanTimeMark,
                                     dot1qVlanCurrentEntryData.dot1qVlanIndex,
                                     &dot1qVlanCurrentEntryData.dot1qVlanCreationTime) == L7_SUCCESS)
      SET_VALID(I_dot1qVlanCreationTime, dot1qVlanCurrentEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qVlanCurrentEntryData.valid))
    return(NULL);

  return(&dot1qVlanCurrentEntryData);
}

dot1qVlanStaticEntry_t *
k_dot1qVlanStaticEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 dot1qVlanIndex)
{
  static dot1qVlanStaticEntry_t dot1qVlanStaticEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dot1qVlanStaticEntryData.dot1qVlanStaticName = MakeOctetString(NULL, 0);
    dot1qVlanStaticEntryData.dot1qVlanStaticEgressPorts = MakeOctetString(NULL, 0);
    dot1qVlanStaticEntryData.dot1qVlanForbiddenEgressPorts = MakeOctetString(NULL, 0);
    dot1qVlanStaticEntryData.dot1qVlanStaticUntaggedPorts = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dot1qVlanStaticEntryData.valid);
  dot1qVlanStaticEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qVlanStaticEntryIndex_dot1qVlanIndex, dot1qVlanStaticEntryData.valid);

  if ( (searchType == EXACT) ?                                                                                
       (usmDbDot1qVlanStaticEntryGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qVlanStaticEntryGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qVlanStaticEntryNextGet(USMDB_UNIT_CURRENT, &dot1qVlanStaticEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qVlanStaticEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qVlanStaticEntryIndex_dot1qVlanIndex:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanStaticName:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    if (usmDbDot1qVlanStaticNameGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex,
                                    snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromText(&dot1qVlanStaticEntryData.dot1qVlanStaticName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_dot1qVlanStaticName, dot1qVlanStaticEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanStaticEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
    if (usmDbDot1qVlanStaticEgressPortsGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex,
                                           snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qVlanStaticEntryData.dot1qVlanStaticEgressPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qVlanStaticEgressPorts, dot1qVlanStaticEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanForbiddenEgressPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
    if (usmDbDot1qVlanForbiddenEgressPortsGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex,
                                              snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qVlanStaticEntryData.dot1qVlanForbiddenEgressPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qVlanForbiddenEgressPorts, dot1qVlanStaticEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanStaticUntaggedPorts:
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    snmp_buffer_len = SNMP_VLAN_BUFFER_LEN;
    if (usmDbDot1qVlanStaticUntaggedPortsGet(USMDB_UNIT_CURRENT, dot1qVlanStaticEntryData.dot1qVlanIndex,
                                             snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&dot1qVlanStaticEntryData.dot1qVlanStaticUntaggedPorts, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_dot1qVlanStaticUntaggedPorts, dot1qVlanStaticEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qVlanStaticRowStatus:
/* lvl7_@p0837 start */
    dot1qVlanStaticEntryData.dot1qVlanStaticRowStatus = D_dot1qVlanStaticRowStatus_active;
    SET_VALID(I_dot1qVlanStaticRowStatus, dot1qVlanStaticEntryData.valid);
/* lvl7_@p0837 end */
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qVlanStaticEntryData.valid))
    return(NULL);

  return(&dot1qVlanStaticEntryData);
}

#ifdef SETS
int
k_dot1qVlanStaticEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{
  ZERO_VALID(((dot1qVlanStaticEntry_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_dot1qVlanStaticEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qVlanStaticEntry_set_defaults(doList_t *dp)
{
  dot1qVlanStaticEntry_t *data = (dot1qVlanStaticEntry_t *) (dp->data);

  if ((data->dot1qVlanStaticName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qVlanStaticEgressPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qVlanForbiddenEgressPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->dot1qVlanStaticUntaggedPorts = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  data->dot1qVlanStaticRowStatus = D_dot1qVlanStaticRowStatus_active;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qVlanStaticEntry_set(dot1qVlanStaticEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

  L7_char8 snmp_buffer[SNMP_VLAN_BUFFER_LEN];
  L7_BOOL isNewRowCreated = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

/* lvl7_@p0837 start */
  if (usmDbDot1qVlanStaticEntryGet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex) != L7_SUCCESS)
  {
    /* if the entry doesn't exist, try to create it */
    if (VALID(I_dot1qVlanStaticRowStatus, data->valid) &&
        (data->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_createAndGo ||
         data->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_createAndWait))
    {
      if (usmDbVlanCreate(USMDB_UNIT_CURRENT, data->dot1qVlanIndex) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
         SET_VALID(I_dot1qVlanStaticRowStatus, tempValid);
         isNewRowCreated = L7_TRUE;
      }
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
/* lvl7_@p0837 end */

  if (VALID(I_dot1qVlanStaticName, data->valid))
  {
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot1qVlanStaticName->octet_ptr, data->dot1qVlanStaticName->length);
    if(usmDbStringAlphaNumericCheck(snmp_buffer)!= L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);   
    }
    else
    if (usmDbDot1qVlanStaticNameSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_dot1qVlanStaticName, tempValid);
    }
  }

  if (VALID(I_dot1qVlanStaticEgressPorts, data->valid))
  {
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot1qVlanStaticEgressPorts->octet_ptr, data->dot1qVlanStaticEgressPorts->length);
    if (usmDbDot1qVlanStaticEgressPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qVlanStaticEgressPorts, tempValid);
    }
  }

  if (VALID(I_dot1qVlanForbiddenEgressPorts, data->valid) )
  {
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot1qVlanForbiddenEgressPorts->octet_ptr, data->dot1qVlanForbiddenEgressPorts->length);
    if(usmDbDot1qVlanForbiddenEgressPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex, snmp_buffer) != L7_SUCCESS)
    { 
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qVlanForbiddenEgressPorts, tempValid);
    }
  }

  if (VALID(I_dot1qVlanStaticUntaggedPorts, data->valid))
  {
    bzero(snmp_buffer, SNMP_VLAN_BUFFER_LEN);
    memcpy(snmp_buffer, data->dot1qVlanStaticUntaggedPorts->octet_ptr, data->dot1qVlanStaticUntaggedPorts->length);
    if(usmDbDot1qVlanStaticUntaggedPortsSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,snmp_buffer) != L7_SUCCESS)
    { 
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_dot1qVlanStaticUntaggedPorts, tempValid);
    }
  }

/* lvl7_@p0837 start */
  if (VALID(I_dot1qVlanStaticRowStatus, data->valid))
  {
     if( !(data->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_createAndGo ||
         data->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_createAndWait) &&
         (isNewRowCreated != L7_TRUE)&&
      snmpDot1qVlanStaticRowStatusSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                      data->dot1qVlanStaticRowStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
/* lvl7_@p0837 end */

  return NO_ERROR;
}

  #ifdef SR_dot1qVlanStaticEntry_UNDO
/* add #define SR_dot1qVlanStaticEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qVlanStaticEntry family.
 */
int
dot1qVlanStaticEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  dot1qVlanStaticEntry_t *data = (dot1qVlanStaticEntry_t *) doCur->data;
  dot1qVlanStaticEntry_t *undodata = (dot1qVlanStaticEntry_t *) doCur->undodata;
  dot1qVlanStaticEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && data->dot1qVlanStaticRowStatus != D_dot1qVlanStaticRowStatus_destroy)
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  if ( data->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_destroy)
      memset(undodata->valid, 0xff, sizeof(undodata->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL ) 
  {
      /* undoing an add, so delete */
      data->dot1qVlanStaticRowStatus = D_dot1qVlanStaticRowStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  } 
  else 
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_notReady)
      || (undodata->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_notInService)) 
      {
          undodata->dot1qVlanStaticRowStatus = D_dot1qVlanStaticRowStatus_createAndWait;
      } 
      else
      if(undodata->dot1qVlanStaticRowStatus == D_dot1qVlanStaticRowStatus_active) 
      {
          undodata->dot1qVlanStaticRowStatus = D_dot1qVlanStaticRowStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qVlanStaticEntry_set(setdata, contextInfo, function) == NO_ERROR) 
  {
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
#endif /* SR_dot1qVlanStaticEntry_UNDO */

#endif /* SETS */

dot1qPortVlanStatisticsEntry_t *
k_dot1qPortVlanStatisticsEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 dot1dBasePort,
                                   SR_UINT32 dot1qVlanIndex)
{
/* lvl7_@p2069 start */
#ifdef NOT_SUPPORTED
  static dot1qPortVlanStatisticsEntry_t dot1qPortVlanStatisticsEntryData;

  ZERO_VALID(dot1qPortVlanStatisticsEntryData.valid);
  dot1qPortVlanStatisticsEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1qPortVlanStatisticsEntryIndex_dot1dBasePort, dot1qPortVlanStatisticsEntryData.valid);
  dot1qPortVlanStatisticsEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qPortVlanStatisticsEntryIndex_dot1qVlanIndex, dot1qPortVlanStatisticsEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qPortVlanStatisticsEntryGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                             dot1qPortVlanStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qPortVlanStatisticsEntryGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                               dot1qPortVlanStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qPortVlanStatisticsEntryNextGet(USMDB_UNIT_CURRENT, &dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                                   &dot1qPortVlanStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qPortVlanStatisticsEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qPortVlanStatisticsEntryIndex_dot1dBasePort:
  case I_dot1qPortVlanStatisticsEntryIndex_dot1qVlanIndex:
    break;

  case I_dot1qTpVlanPortInFrames:
    if (usmDbDot1qTpVlanPortInFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                        dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                        &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortInFrames) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortInFrames, dot1qPortVlanStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortOutFrames:
    if (usmDbDot1qTpVlanPortOutFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                         dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                         &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortOutFrames) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortOutFrames, dot1qPortVlanStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortInDiscards:
    if (usmDbDot1qTpVlanPortInDiscardsGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                          dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                          &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortInDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortInDiscards, dot1qPortVlanStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortInOverflowFrames:
    if (usmDbDot1qTpVlanPortInOverflowFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                                dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                                &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortInOverflowFrames) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortInOverflowFrames, dot1qPortVlanStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortOutOverflowFrames:
    if (usmDbDot1qTpVlanPortOutOverflowFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                                 dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                                 &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortOutOverflowFrames) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortOutOverflowFrames, dot1qPortVlanStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortInOverflowDiscards:
    if (usmDbDot1qTpVlanPortInOverflowDiscardsGet(USMDB_UNIT_CURRENT, dot1qPortVlanStatisticsEntryData.dot1dBasePort,
                                                  dot1qPortVlanStatisticsEntryData.dot1qVlanIndex,
                                                  &dot1qPortVlanStatisticsEntryData.dot1qTpVlanPortInOverflowDiscards) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortInOverflowDiscards, dot1qPortVlanStatisticsEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qPortVlanStatisticsEntryData.valid))
    return(NULL);

  return(&dot1qPortVlanStatisticsEntryData);
#else
  return(NULL);
#endif
/* lvl7_@p2069 end */
}

dot1qPortVlanHCStatisticsEntry_t *
k_dot1qPortVlanHCStatisticsEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_INT32 dot1dBasePort,
                                     SR_UINT32 dot1qVlanIndex)
{
/* lvl7_@p2069 start */
#ifdef NOT_SUPPORTED
  static dot1qPortVlanHCStatisticsEntry_t dot1qPortVlanHCStatisticsEntryData;

  ZERO_VALID(dot1qPortVlanHCStatisticsEntryData.valid);
  dot1qPortVlanHCStatisticsEntryData.dot1dBasePort = dot1dBasePort;
  SET_VALID(I_dot1qPortVlanHCStatisticsEntryIndex_dot1dBasePort, dot1qPortVlanHCStatisticsEntryData.valid);
  dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_dot1qPortVlanHCStatisticsEntryIndex_dot1qVlanIndex, dot1qPortVlanHCStatisticsEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qPortVlanHCStatisticsEntryGet(USMDB_UNIT_CURRENT, dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                               dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) :
       ( (usmDbDot1qPortVlanHCStatisticsEntryGet(USMDB_UNIT_CURRENT, dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                                 dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) &&
         (usmDbDot1qPortVlanHCStatisticsEntryNextGet(USMDB_UNIT_CURRENT, &dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                                     &dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qPortVlanHCStatisticsEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_dot1qPortVlanHCStatisticsEntryIndex_dot1dBasePort:
  case I_dot1qPortVlanHCStatisticsEntryIndex_dot1qVlanIndex:
    break;

  case I_dot1qTpVlanPortHCInFrames:
    if ( (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInFrames != NULL ||
          (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInFrames = MakeCounter64(0)) != NULL) &&
         usmDbDot1qTpVlanPortHCInFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                           dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex,
                                           &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInFrames->big_end,
                                           &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInFrames->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortHCInFrames, dot1qPortVlanHCStatisticsEntryData.valid);

    if (nominator != -1)
    /* else pass through */
      break;

  case I_dot1qTpVlanPortHCOutFrames:
    if ( (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCOutFrames != NULL ||
          (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCOutFrames = MakeCounter64(0)) != NULL) &&
         usmDbDot1qTpVlanPortHCOutFramesGet(USMDB_UNIT_CURRENT, dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                            dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex,
                                            &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCOutFrames->big_end,
                                            &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCOutFrames->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortHCOutFrames, dot1qPortVlanHCStatisticsEntryData.valid);
    break;

  case I_dot1qTpVlanPortHCInDiscards:
    if ( (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInDiscards != NULL ||
          (dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInDiscards = MakeCounter64(0)) != NULL) &&
         usmDbDot1qTpVlanPortHCInDiscardsGet(USMDB_UNIT_CURRENT, dot1qPortVlanHCStatisticsEntryData.dot1dBasePort,
                                             dot1qPortVlanHCStatisticsEntryData.dot1qVlanIndex,
                                             &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInDiscards->big_end,
                                             &dot1qPortVlanHCStatisticsEntryData.dot1qTpVlanPortHCInDiscards->little_end) == L7_SUCCESS)
      SET_VALID(I_dot1qTpVlanPortHCInDiscards, dot1qPortVlanHCStatisticsEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qPortVlanHCStatisticsEntryData.valid))
    return(NULL);

  return(&dot1qPortVlanHCStatisticsEntryData);
#else
  return(NULL);
#endif
/* lvl7_@p2069 end */
}

dot1qLearningConstraintsEntry_t *
k_dot1qLearningConstraintsEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_UINT32 dot1qConstraintVlan,
                                    SR_INT32 dot1qConstraintSet)
{
/* lvl7_@p0809 start */
#ifdef NOT_SUPPORTED
  static dot1qLearningConstraintsEntry_t dot1qLearningConstraintsEntryData;

  ZERO_VALID(dot1qLearningConstraintsEntryData.valid);
  dot1qLearningConstraintsEntryData.dot1qConstraintVlan = dot1qConstraintVlan;
  SET_VALID(I_dot1qConstraintVlan, dot1qLearningConstraintsEntryData.valid);
  dot1qLearningConstraintsEntryData.dot1qConstraintSet = dot1qConstraintSet;
  SET_VALID(I_dot1qConstraintSet, dot1qLearningConstraintsEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDot1qLearningConstraintsEntryGet(USMDB_UNIT_CURRENT, dot1qLearningConstraintsEntryData.dot1qConstraintVlan,
                                              dot1qLearningConstraintsEntryData.dot1qConstraintSet) != L7_SUCCESS) :
       ( (usmDbDot1qLearningConstraintsEntryGet(USMDB_UNIT_CURRENT, dot1qLearningConstraintsEntryData.dot1qConstraintVlan,
                                                dot1qLearningConstraintsEntryData.dot1qConstraintSet) != L7_SUCCESS) &&
         (usmDbDot1qLearningConstraintsEntryNextGet(USMDB_UNIT_CURRENT, &dot1qLearningConstraintsEntryData.dot1qConstraintVlan,
                                                    &dot1qLearningConstraintsEntryData.dot1qConstraintSet) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dot1qLearningConstraintsEntryData.valid);
    return(NULL);
  }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

  switch (nominator)
  {
  case -1:
  case I_dot1qConstraintVlan:
  case I_dot1qConstraintSet:
    if (nominator != -1) break;
    /* else pass through */

  case I_dot1qConstraintType:
    if (usmDbDot1qConstraintTypeGet(USMDB_UNIT_CURRENT, dot1qLearningConstraintsEntryData.dot1qConstraintVlan,
                                    dot1qLearningConstraintsEntryData.dot1qConstraintSet,
                                    &dot1qLearningConstraintsEntryData.dot1qConstraintType) == L7_SUCCESS)
      SET_VALID(I_dot1qConstraintType, dot1qLearningConstraintsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */


  case I_dot1qConstraintStatus:
    if (usmDbDot1qConstraintStatusGet(USMDB_UNIT_CURRENT, dot1qLearningConstraintsEntryData.dot1qConstraintVlan,
                                      dot1qLearningConstraintsEntryData.dot1qConstraintSet,
                                      &dot1qLearningConstraintsEntryData.dot1qConstraintStatus) == L7_SUCCESS)
      SET_VALID(I_dot1qConstraintStatus, dot1qLearningConstraintsEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dot1qLearningConstraintsEntryData.valid))
    return(NULL);

  return(&dot1qLearningConstraintsEntryData);
#else  /* NOT_SUPPORTED */
  return(NULL);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0809 end */
}

#ifdef SETS
int
k_dot1qLearningConstraintsEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_dot1qLearningConstraintsEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                      doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_dot1qLearningConstraintsEntry_set_defaults(doList_t *dp)
{
  dot1qLearningConstraintsEntry_t *data = (dot1qLearningConstraintsEntry_t *) (dp->data);


  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_dot1qLearningConstraintsEntry_set(dot1qLearningConstraintsEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

/* lvl7_@p0809 start */
#ifdef NOT_SUPPORTED
  if (usmDbDot1qLearningConstraintsEntryGet(USMDB_UNIT_CURRENT, data->dot1qConstraintVlan,
                                            data->dot1qConstraintSet) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(NULL);
  }

  if (VALID(I_dot1qConstraintType, data->valid))
  {
    if(usmDbDot1qConstraintTypeSet(USMDB_UNIT_CURRENT, data->dot1qConstraintVlan,
                                  data->dot1qConstraintSet,
                                  data->dot1qConstraintType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
#else  /* NOT_SUPPORTED */
  return(COMMIT_FAILED_ERROR);
#endif /* NOT_SUPPORTED */

/* lvl7_@p0809 end */
}

  #ifdef SR_dot1qLearningConstraintsEntry_UNDO
/* add #define SR_dot1qLearningConstraintsEntry_UNDO in sitedefs.h to
 * include the undo routine for the dot1qLearningConstraintsEntry family.
 */
int
dot1qLearningConstraintsEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
#ifdef NOT_SUPPORTED
  dot1qLearningConstraintsEntry_t *data = (dot1qLearningConstraintsEntry_t *) doCur->data;
  dot1qLearningConstraintsEntry_t *undodata = (dot1qLearningConstraintsEntry_t *) doCur->undodata;
  dot1qLearningConstraintsEntry_t *setdata = NULL;
  L7_uint32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data 
  */
  if ( data->valid == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add or a delete which is not possible */
  if ( undodata == NULL )
     return NO_ERROR;

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && k_dot1qLearningConstraintsEntry_set(setdata, contextInfo, function) == NO_ERROR) {
      return NO_ERROR;
  }
  return UNDO_FAILED_ERROR;
#else /* NOT_SUPPORTED */
  return UNDO_FAILED_ERROR;
#endif /* NOT_SUPPORTED */
}
  #endif /* SR_dot1qLearningConstraintsEntry_UNDO */

#endif /* SETS */

