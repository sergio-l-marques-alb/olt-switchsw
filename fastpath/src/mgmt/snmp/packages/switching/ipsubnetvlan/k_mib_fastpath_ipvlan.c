/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan.c
*
* @purpose    System-Specific code to support IP subnet VLAN component
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
#include "k_mib_fastpath_ipvlan_api.h"
#include "usmdb_vlan_ipsubnet_api.h"

agentSwitchVlanSubnetAssociationEntry_t *
k_agentSwitchVlanSubnetAssociationEntry_get(int serialNum,
                           ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 agentSwitchVlanSubnetAssociationIPAddress,
                           SR_UINT32 agentSwitchVlanSubnetAssociationSubnetMask,
                           SR_UINT32 agentSwitchVlanSubnetAssociationVlanId)
{
   static agentSwitchVlanSubnetAssociationEntry_t agentSwitchVlanSubnetAssociationEntryData;

   ZERO_VALID(agentSwitchVlanSubnetAssociationEntryData.valid);

   agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationIPAddress
                  = agentSwitchVlanSubnetAssociationIPAddress;
   agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationSubnetMask
                  = agentSwitchVlanSubnetAssociationSubnetMask;
   agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationVlanId
                  = agentSwitchVlanSubnetAssociationVlanId;

   SET_VALID(I_agentSwitchVlanSubnetAssociationIPAddress,
             agentSwitchVlanSubnetAssociationEntryData.valid);
   SET_VALID(I_agentSwitchVlanSubnetAssociationSubnetMask,
             agentSwitchVlanSubnetAssociationEntryData.valid);
   SET_VALID(I_agentSwitchVlanSubnetAssociationVlanId,
             agentSwitchVlanSubnetAssociationEntryData.valid);
   if(snmpSwitchVlanSubnetAssociationEntryGet(agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationIPAddress,
              agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationSubnetMask,
              agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationVlanId) != L7_SUCCESS)
   {
     if((searchType == EXACT) ||
         (snmpSwitchVlanSubnetAssociationEntryGetNext(&agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationIPAddress,
         &agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationSubnetMask,
         &agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationVlanId) != L7_SUCCESS))
     {
       ZERO_VALID(agentSwitchVlanSubnetAssociationEntryData.valid);
       return(NULL);
     }
   }
   switch(nominator)
   {
     case -1:
     case I_agentSwitchVlanSubnetAssociationIPAddress:
     case I_agentSwitchVlanSubnetAssociationSubnetMask:
     case I_agentSwitchVlanSubnetAssociationVlanId:
            if (nominator != -1 ) break;
         /* else pass through */
     case I_agentSwitchVlanSubnetAssociationRowStatus:
       agentSwitchVlanSubnetAssociationEntryData.agentSwitchVlanSubnetAssociationRowStatus
               = D_agentSwitchVlanSubnetAssociationRowStatus_active;
            SET_VALID(I_agentSwitchVlanSubnetAssociationRowStatus,
                      agentSwitchVlanSubnetAssociationEntryData.valid);
            break;
     default:
            return(NULL);
            break;
   }
   if (nominator >= 0 &&
         !VALID(nominator, agentSwitchVlanSubnetAssociationEntryData.valid))
            return(NULL);

   return(&agentSwitchVlanSubnetAssociationEntryData);
}

#ifdef SETS
int
k_agentSwitchVlanSubnetAssociationEntry_test(ObjectInfo *object,
                                             ObjectSyntax *value,
                                             doList_t *dp,
                                             ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchVlanSubnetAssociationEntry_ready(ObjectInfo *object,
                                              ObjectSyntax *value,
                                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchVlanSubnetAssociationEntry_set_defaults(doList_t *dp)
{
    agentSwitchVlanSubnetAssociationEntry_t *data =
               (agentSwitchVlanSubnetAssociationEntry_t *) (dp->data);
    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchVlanSubnetAssociationEntry_set(agentSwitchVlanSubnetAssociationEntry_t *data,
                                            ContextInfo *contextInfo, int function)
{
   L7_char8 tempValid[sizeof(data->valid)];
   memset(tempValid, 0, sizeof(tempValid));
   if(VALID(I_agentSwitchVlanSubnetAssociationRowStatus,data->valid))
   {
     if(data->agentSwitchVlanSubnetAssociationRowStatus ==
               D_agentSwitchVlanSubnetAssociationRowStatus_createAndGo)
     {
       if((snmpSwitchVlanSubnetAssociationEntryGet(
               data->agentSwitchVlanSubnetAssociationIPAddress,
               data->agentSwitchVlanSubnetAssociationSubnetMask,
               data->agentSwitchVlanSubnetAssociationVlanId) == L7_SUCCESS) ||
          (snmpSwitchVlanSubnetAssociationEntryAdd(
               data->agentSwitchVlanSubnetAssociationIPAddress,
               data->agentSwitchVlanSubnetAssociationSubnetMask,
               data->agentSwitchVlanSubnetAssociationVlanId) != L7_SUCCESS))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
       }
     }
     else if(data->agentSwitchVlanSubnetAssociationRowStatus ==
               D_agentSwitchVlanSubnetAssociationRowStatus_destroy)
     {
       if((snmpSwitchVlanSubnetAssociationEntryGet(
               data->agentSwitchVlanSubnetAssociationIPAddress,
               data->agentSwitchVlanSubnetAssociationSubnetMask,
               data->agentSwitchVlanSubnetAssociationVlanId)!= L7_SUCCESS) ||
          (snmpSwitchVlanSubnetAssociationEntryDelete(
               data->agentSwitchVlanSubnetAssociationIPAddress,
               data->agentSwitchVlanSubnetAssociationSubnetMask,
               data->agentSwitchVlanSubnetAssociationVlanId)!= L7_SUCCESS))
       {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
       }
     }
     else if(data->agentSwitchVlanSubnetAssociationRowStatus !=
               D_agentSwitchVlanSubnetAssociationRowStatus_active)
     {
         memcpy(data->valid, tempValid, sizeof(data->valid));
         return COMMIT_FAILED_ERROR;
     }
   }
   return NO_ERROR;
}

#ifdef SR_agentSwitchVlanSubnetAssociationEntry_UNDO
/* add #define SR_agentSwitchVlanSubnetAssociationEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchVlanSubnetAssociationEntry family.
 */
int
agentSwitchVlanSubnetAssociationEntry_undo(doList_t *doHead, doList_t *doCur,
                                           ContextInfo *contextInfo)
{
   agentSwitchVlanSubnetAssociationEntry_t *data = (agentSwitchVlanSubnetAssociationEntry_t *) doCur->data;
   agentSwitchVlanSubnetAssociationEntry_t *undodata = (agentSwitchVlanSubnetAssociationEntry_t *) doCur->undodata;
   agentSwitchVlanSubnetAssociationEntry_t *setdata = NULL;
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
       (k_agentSwitchVlanSubnetAssociationEntry_set(setdata, contextInfo,
						    function) == NO_ERROR))
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchVlanSubnetAssociationEntry_UNDO */

#endif /* SETS */

