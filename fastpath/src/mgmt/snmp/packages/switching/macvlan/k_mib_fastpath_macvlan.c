/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan.c
*
* @purpose    System-Specific code to support Mac VLAN component
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     akulkarni
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "k_private_base.h"
#include "k_mib_fastpath_macvlan_api.h"
#include "usmdb_vlan_mac_api.h"


agentSwitchVlanMacAssociationEntry_t *
k_agentSwitchVlanMacAssociationEntry_get(int serialNum,
                          ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          OctetString * agentSwitchVlanMacAssociationMacAddress,
                          SR_UINT32 agentSwitchVlanMacAssociationVlanId)
{
   static agentSwitchVlanMacAssociationEntry_t agentSwitchVlanMacAssociationEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   if(firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;
     agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationMacAddress =
     MakeOctetString(NULL, 0);
   }
   ZERO_VALID(agentSwitchVlanMacAssociationEntryData.valid);

   memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
   memcpy(snmp_buffer, agentSwitchVlanMacAssociationMacAddress->octet_ptr,
          agentSwitchVlanMacAssociationMacAddress->length);

   agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationVlanId
          = agentSwitchVlanMacAssociationVlanId;
   SET_VALID(I_agentSwitchVlanMacAssociationVlanId,
             agentSwitchVlanMacAssociationEntryData.valid);

   if(snmpSwitchVlanMacAssociationEntryGet(snmp_buffer,
     agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationVlanId)
      != L7_SUCCESS)
   {
     if((searchType == EXACT) ||
        (snmpSwitchVlanMacAssociationEntryGetNext(snmp_buffer,
           &agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationVlanId)
         != L7_SUCCESS))
     {
       ZERO_VALID(agentSwitchVlanMacAssociationEntryData.valid);
       return(NULL);
     }
   }
   if (SafeMakeOctetString(&agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationMacAddress,
                           snmp_buffer, L7_MAC_ADDR_LEN) != L7_TRUE)
   {
     return(NULL);
   }
   SET_VALID(I_agentSwitchVlanMacAssociationMacAddress,
             agentSwitchVlanMacAssociationEntryData.valid);

   switch (nominator)
   {
     case -1:
     case I_agentSwitchVlanMacAssociationVlanId:
     case I_agentSwitchVlanMacAssociationMacAddress:
            if (nominator != -1 ) break;
              /* else pass through */
     case I_agentSwitchVlanMacAssociationRowStatus:
       agentSwitchVlanMacAssociationEntryData.agentSwitchVlanMacAssociationRowStatus =
         D_agentSwitchVlanMacAssociationRowStatus_active;
       SET_VALID(I_agentSwitchVlanMacAssociationRowStatus,
                 agentSwitchVlanMacAssociationEntryData.valid);
       break;
     default:
       return(NULL);
   }

   if (nominator >= 0 &&
         !VALID(nominator, agentSwitchVlanMacAssociationEntryData.valid))
     return(NULL);

   return(&agentSwitchVlanMacAssociationEntryData);
}

#ifdef SETS
int
k_agentSwitchVlanMacAssociationEntry_test(ObjectInfo *object,
                                          ObjectSyntax *value,
                                          doList_t *dp,
                                          ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentSwitchVlanMacAssociationEntry_ready(ObjectInfo *object,
                                           ObjectSyntax *value,
                                           doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchVlanMacAssociationEntry_set_defaults(doList_t *dp)
{
    agentSwitchVlanMacAssociationEntry_t *data =
             (agentSwitchVlanMacAssociationEntry_t *) (dp->data);
    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchVlanMacAssociationEntry_set(
                                   agentSwitchVlanMacAssociationEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_char8 tempValid[sizeof(data->valid)];
   memset(tempValid, 0, sizeof(tempValid));
   if(data->agentSwitchVlanMacAssociationMacAddress == L7_NULLPTR)
     return COMMIT_FAILED_ERROR;
   memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
   memcpy(snmp_buffer, data->agentSwitchVlanMacAssociationMacAddress->octet_ptr,
          data->agentSwitchVlanMacAssociationMacAddress->length);
   if(VALID(I_agentSwitchVlanMacAssociationRowStatus,data->valid))
   {
     if(data->agentSwitchVlanMacAssociationRowStatus ==
                           D_agentSwitchVlanMacAssociationRowStatus_createAndGo)
     {
       if((snmpSwitchVlanMacAssociationEntryGet(snmp_buffer,
          data->agentSwitchVlanMacAssociationVlanId) == L7_SUCCESS) ||
          (snmpSwitchVlanMacAssociationEntryAdd(snmp_buffer,
           data->agentSwitchVlanMacAssociationVlanId) != L7_SUCCESS))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
       }
     }
     else if(data->agentSwitchVlanMacAssociationRowStatus ==
                           D_agentSwitchVlanMacAssociationRowStatus_destroy)
     {
       if((snmpSwitchVlanMacAssociationEntryGet(snmp_buffer,
          data->agentSwitchVlanMacAssociationVlanId) != L7_SUCCESS) ||
          (snmpSwitchVlanMacAssociationEntryDelete(snmp_buffer,
           data->agentSwitchVlanMacAssociationVlanId) != L7_SUCCESS))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
       }
     }
     else if(data->agentSwitchVlanMacAssociationRowStatus !=
                          D_agentSwitchVlanMacAssociationRowStatus_active)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
   }
   return NO_ERROR;
}


#ifdef SR_agentSwitchVlanMacAssociationEntry_UNDO
/* add #define SR_agentSwitchVlanMacAssociationEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchVlanMacAssociationEntry family.
 */
int
agentSwitchVlanMacAssociationEntry_undo(doList_t *doHead, doList_t *doCur,
                                        ContextInfo *contextInfo)
{

  agentSwitchVlanMacAssociationEntry_t *data = (agentSwitchVlanMacAssociationEntry_t *) doCur->data;
  agentSwitchVlanMacAssociationEntry_t *undodata = (agentSwitchVlanMacAssociationEntry_t *) doCur->undodata;
  agentSwitchVlanMacAssociationEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
   */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) &&
      (k_agentSwitchVlanMacAssociationEntry_set(setdata, contextInfo,
						function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchVlanMacAssociationEntry_UNDO */


#endif /* SETS */


