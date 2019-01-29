/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: k_mib_fastpathloopback.c
*
* Purpose: Private mibs for the switch
*
* Created by: nramesh 10/28/2005
*
* Component: IPV6
*
*********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include "k_private_base.h"
#include "sr_ip.h"
#include "usmdb_ip_api.h"
#include "usmdb_common.h"
#include "usmdb_rlim_api.h"

#include "k_mib_fastpathloopback_api.h"

agentLoopbackEntry_t *
k_agentLoopbackEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentLoopbackID)
{
  static agentLoopbackEntry_t agentLoopbackEntryData;
  L7_uint32 intIfIndex, ipval, subval;

  ZERO_VALID(agentLoopbackEntryData.valid);
  agentLoopbackEntryData.agentLoopbackID = agentLoopbackID;
  SET_VALID(I_agentLoopbackID, agentLoopbackEntryData.valid);

  if (searchType == EXACT ?
      (snmpAgentLoopbackEntryGet(USMDB_UNIT_CURRENT,
                                     agentLoopbackEntryData.agentLoopbackID) != L7_SUCCESS) :
      (snmpAgentLoopbackEntryGet(USMDB_UNIT_CURRENT,
                                     agentLoopbackEntryData.agentLoopbackID) != L7_SUCCESS &&
       snmpAgentLoopbackEntryNextGet(USMDB_UNIT_CURRENT,
                                       agentLoopbackEntryData.agentLoopbackID,
                                       &agentLoopbackEntryData.agentLoopbackID) != L7_SUCCESS))
  {
    ZERO_VALID(agentLoopbackEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  if (usmDbRlimLoopbackIntIfNumGet(agentLoopbackEntryData.agentLoopbackID, 
                                   &intIfIndex) != L7_SUCCESS)
  {
    ZERO_VALID(agentLoopbackEntryData.valid);
    return(NULL);
  }  

  switch (nominator)
  {
    case -1:
    case I_agentLoopbackID:
      if (nominator != -1 ) break;
      /* else pass through */

    case I_agentLoopbackIfIndex:
      if ((snmpLoopbackIfIndexGet(USMDB_UNIT_CURRENT,
                                      agentLoopbackEntryData.agentLoopbackID,
                                      &agentLoopbackEntryData.agentLoopbackIfIndex)) == L7_SUCCESS)
        SET_VALID(I_agentLoopbackIfIndex, agentLoopbackEntryData.valid);
      if (nominator != -1 ) break;
      /* else pass through */

    case I_agentLoopbackIPAddress:
      if(usmDbIpRtrIntfIpAddressGet(USMDB_UNIT_CURRENT, intIfIndex, 
                                    &ipval, &subval) == L7_SUCCESS)
      {
        agentLoopbackEntryData.agentLoopbackIPAddress = IPToOctetString(ipval);
        SET_VALID(I_agentLoopbackIPAddress, agentLoopbackEntryData.valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    case I_agentLoopbackIPSubnet:
      if(usmDbIpRtrIntfIpAddressGet(USMDB_UNIT_CURRENT, intIfIndex,
                                    &ipval, &subval) == L7_SUCCESS)
      {
        agentLoopbackEntryData.agentLoopbackIPSubnet = IPToOctetString(subval);
        SET_VALID(I_agentLoopbackIPSubnet, agentLoopbackEntryData.valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

   case I_agentLoopbackStatus:
     agentLoopbackEntryData.agentLoopbackStatus = D_agentLoopbackStatus_active;
     SET_VALID(I_agentLoopbackStatus, agentLoopbackEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;

  }

  if (nominator >= 0 && !VALID(nominator, agentLoopbackEntryData.valid))
    return(NULL);

   return(&agentLoopbackEntryData);
}

#ifdef SETS
int
k_agentLoopbackEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentLoopbackEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentLoopbackEntry_set_defaults(doList_t *dp)
{
    agentLoopbackEntry_t *data = (agentLoopbackEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentLoopbackEntry_set(agentLoopbackEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;
  L7_uint32 intIfIndex, ipval, subval;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (snmpAgentLoopbackEntryGet(USMDB_UNIT_CURRENT, data->agentLoopbackID) != L7_SUCCESS)
  {
    if (VALID(I_agentLoopbackStatus, data->valid) != L7_SUCCESS)
    {
      if(data->agentLoopbackStatus == D_agentLoopbackStatus_createAndGo)
      {
        if (snmpLoopbackCreate(USMDB_UNIT_CURRENT, data->agentLoopbackID) != L7_SUCCESS)
        {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return COMMIT_FAILED_ERROR;
        }
        else
        {
          isCreatedNew = L7_TRUE;
          SET_VALID(I_agentLoopbackStatus, tempValid);
        }
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
    }
  }

  usmDbRlimLoopbackIntIfNumGet(data->agentLoopbackID, &intIfIndex);

 /* LoopbackIPAddress and LoopbackIPSubnet both should be set, otherwise return a COMMIT_FAILED ERROR */
  if ((!VALID(I_agentLoopbackIPAddress, data->valid) && VALID(I_agentLoopbackIPSubnet, data->valid)) || 
         (!VALID(I_agentLoopbackIPSubnet, data->valid) && VALID(I_agentLoopbackIPAddress, data->valid)))
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }


  if(VALID(I_agentLoopbackIPAddress, data->valid) &&
     VALID(I_agentLoopbackIPSubnet, data->valid))
  {
    /*Both IP Address and the subnet need to be set in the same packet */
    ipval = OctetStringToIP(data->agentLoopbackIPAddress);
    subval = OctetStringToIP(data->agentLoopbackIPSubnet);
    if((ipval != 0) && 
       (usmDbIpRtrIntfIPAddressSet(USMDB_UNIT_CURRENT, intIfIndex, ipval,
                                   subval,
                                   L7_INTF_IP_ADDR_METHOD_CONFIG) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLoopbackIPAddress, tempValid);
      SET_VALID(I_agentLoopbackIPSubnet, tempValid);
    }
  }
  
  if (VALID(I_agentLoopbackStatus, data->valid) && (isCreatedNew != L7_TRUE))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->agentLoopbackStatus != D_agentLoopbackStatus_active)
         && (data->agentLoopbackStatus != D_agentLoopbackStatus_destroy ||
         (snmpLoopbackDelete(USMDB_UNIT_CURRENT, intIfIndex) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentLoopbackEntry_UNDO
/* add #define SR_agentLoopbackEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentLoopbackEntry family.
 */
int
agentLoopbackEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLoopbackEntry_UNDO */

#endif /* SETS */
