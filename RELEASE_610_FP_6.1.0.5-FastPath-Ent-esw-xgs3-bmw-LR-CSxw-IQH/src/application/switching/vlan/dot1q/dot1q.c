/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q.c
*
* @purpose dot1q main file
*
* @component dot1q
*
* @comments none
*
* @create 08/14/2000
*
* @author djohnson
*
* @end
*             
**********************************************************************/

#define DOT1Q_STATS

#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "l7_product.h"
#include "nimapi.h"
#include "dot3ad_api.h"
#include "log.h"
#include "statsapi.h"
#include "dtlapi.h"
#include "dot1q_intf.h"
#include "l7_cnfgr_api.h"
#include "dot1q_data.h"
#include "dot1q_control.h"
#include "dot1q_cnfgr.h"
#include "dot1q_cfg.h"
#include "dot1q_debug.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"

/* Externs */

extern dot1qCfgData_t      *dot1qCfg; 
extern dot1qVlanCheckNotifyList_t vlanRequestCheckList[L7_LAST_COMPONENT_ID];


/* Local declarations */
dot1qIntfData_t     *dot1qQports  = L7_NULLPTR;      
dot1qInfo_t         *dot1qInfo    = L7_NULLPTR;      
vlanTreeTables_t       *pVlanTreeHeap = L7_NULLPTR;
dot1q_vlan_dataEntry_t *pVlanDataHeap = L7_NULLPTR;
L7_uint32           *dot1qMapTbl   = L7_NULLPTR;
L7_uint32           *dot1qVlanCfgMapTbl = L7_NULLPTR;
dot1qVidList_t      *pVidList      = L7_NULLPTR;

/*Mask of vlans that are participating for a given port */
L7_VLAN_MASK_t 		dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];

/* Vlan intf structures */
dot1qVlanIntf_t		dot1qVlanIntf[DOT1Q_MAX_VLAN_INTF + 1];

/* Mask of valid ports */
NIM_INTF_MASK_t dot1qValidMemberPortsMask;


/*********************************************************************
* @purpose  Register dot1q system counters with the Statistics Manager
*
* @param    None    
*
* @returns  L7_SUCCESS or L7_FALIURE
*       
* @end
*********************************************************************/
L7_RC_t dot1qStatsSystemCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;
  L7_uint32 listSize_switch = (L7_uint32)(sizeof(dot1q_counters_switch) / sizeof(L7_uint32));
  statsParm_entry_t dot1qCtr;

  dot1qCtr.cKey = 1;
  dot1qCtr.cSize = C32_BITS;
  dot1qCtr.cType = ExternalCtr;
  dot1qCtr.isResettable = L7_FALSE;
  dot1qCtr.pMutlingsParmList = L7_NULL;
  dot1qCtr.pSource = (L7_VOIDFUNCPTR_t) dot1qStatGet;

  /* create the switch counters */
  for (i=0; (i< listSize_switch) && (rc == L7_SUCCESS); i++)
  {
    dot1qCtr.cId = dot1q_counters_switch[i];
    rc = statsCreate(1, (pStatsParm_list_t)(&dot1qCtr));
  }

  return rc;
}

/*********************************************************************
* @purpose  Register dot1q interface counters with the Statistics Manager
*
* @param    None    
*
* @returns  L7_SUCCESS or L7_FALIURE
*       
* @end
*********************************************************************/
L7_RC_t dot1qStatsIntfCreate(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_uint32 listSize_port = (L7_uint32)(sizeof(dot1q_counters_port) / sizeof(L7_uint32));
  statsParm_entry_t dot1qCtr;
  
  /* Create counters for interfaces which are valid participants in a VLAN */
  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  rc = L7_SUCCESS;
  dot1qCtr.cSize = C32_BITS;
  dot1qCtr.cType = ExternalCtr;
  dot1qCtr.isResettable = L7_FALSE;
  dot1qCtr.pMutlingsParmList = L7_NULL;
  dot1qCtr.pSource = (L7_VOIDFUNCPTR_t) dot1qStatGet;
  
  dot1qCtr.cKey = intIfNum;
  for (i=0; (i< listSize_port && rc == L7_SUCCESS);i++)
  {
    dot1qCtr.cId = dot1q_counters_port[i];
    rc = statsCreate(1, (pStatsParm_list_t)(&dot1qCtr));
  }
  /* Invoke USMDB to create its set of DOT1Q user counters */
  rc = usmDbUserStatsCreate(usmDbThisUnitGet(), L7_USMDB_USER_STAT_DOT1Q, intIfNum);

  return rc;
}



/*********************************************************************
* @purpose  Receive interface changes from NIM
*
* @param    event               Any one of L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot1qIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intIfEvent, NIM_CORRELATOR_t correlator)
{
    DOT1Q_MSG_t msg;
    NIM_EVENT_COMPLETE_INFO_t status;
	L7_RC_t rc;

    status.intIfNum     = intIfNum;
    status.component    = L7_DOT1Q_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
	status.response.reason   = NIM_ERR_RC_UNUSED ;
    status.event        = intIfEvent;
    status.correlator   = correlator;


    /* Do not submit unnecessary messages to the queue */
    if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    {
        status.response.rc = L7_SUCCESS;
        nimEventStatusCallback(status);
        return L7_SUCCESS;
    }

    if (intIfEvent != L7_CREATE &&
        intIfEvent != L7_ATTACH &&
        intIfEvent != L7_DETACH &&
        intIfEvent != L7_DELETE &&
        intIfEvent != L7_ACTIVE &&
        intIfEvent != L7_INACTIVE &&
        intIfEvent != L7_PROBE_SETUP &&
        intIfEvent != L7_PORT_ROUTING_ENABLED &&
        intIfEvent != L7_LAG_ACQUIRE &&
        intIfEvent != L7_DOT1X_ACQUIRE &&
        intIfEvent != L7_PROBE_TEARDOWN &&
        intIfEvent != L7_PORT_ROUTING_DISABLED &&
        intIfEvent != L7_LAG_RELEASE &&
        intIfEvent != L7_DOT1X_RELEASE)
    {
        status.response.rc = L7_SUCCESS;
        nimEventStatusCallback(status);
        return L7_SUCCESS;
    }




    /* Issue the  command to dot1q */
    
    msg.event = dot1q_intfChange ;
    msg.data.status = status;

    /* Check if Dot1q is ready to process the event or not */
    if (dot1qCnfgrStateCheck() != L7_TRUE)
    {
      LOG_MSG("Received a interface callback while outside the EXECUTE state");
      return L7_FAILURE;
    }

    rc = dot1qIssueCmd(&msg);

    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Process interface change event received from NIM
*
* @param    event               Any one of L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot1qIntfChangeProcess(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 vlanId = 0;
  L7_uint32 i;
  L7_INTF_TYPES_t intfType;
  L7_uint32 event = status.event;
  
 /* Check if Dot1q is ready to process the event or not */
  if (dot1qCnfgrStateCheck() != L7_TRUE)
  {
    LOG_MSG("Cannot process interface callback while outside the EXECUTE state");
    return L7_FAILURE;
  }

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    LOG_MSG("Invalid intIfNum");
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return rc;
  }

  DOT1Q_TRACE("received int change callback intIfNum (%d) event %d \n", intIfNum, event);

  switch (event)
  {
    case L7_CREATE:
      /* create interface default config if needed */
      rc = dot1qIntfCreate(intIfNum);
      
      /* create interface stats */
      dot1qStatsIntfCreate(intIfNum);

      break;
      
  case L7_ATTACH:
	  /* take whats in the config file, apply to the application */
      rc = dot1qApplyIntfConfigData(intIfNum);

      /* apply whats in application to hardware 
      rc = dot1qApplyIntfConfigToHW(intIfNum);  */
      break;

    case L7_DETACH:
      (void)dot1qVlanIntfIntfChange(intIfNum, L7_INACTIVE);

      rc = dot1qIntfUnapplyConfigData(intIfNum);

      break;
  
    case L7_DELETE: 
       {
         /*----------------------------------------------------------------------------------------*/
         /*  save the configuration for the next reboot                                            */
         /*----------------------------------------------------------------------------------------*/
         rc = nimGetIntfType(intIfNum,&intfType);
         if (rc != L7_SUCCESS)
         {
            return rc;
         }

         if (intfType == L7_LOGICAL_VLAN_INTF)
         {
           /*Get Vlan ID from intIfNum */
           for (i=1; (int)i <= DOT1Q_MAX_VLAN_INTF; i++)
           {
            if ((dot1qVlanIntf[i].inUse == L7_TRUE) && 
               (dot1qVlanIntf[i].intIfNum == intIfNum))
              {
               vlanId = dot1qVlanIntf[i].vlanId;
               break;
              }
           }

           L7_VLAN_CLRMASKBIT(dot1qCfg->QvlanIntf, vlanId);
           dot1qCfg->hdr.dataChanged = L7_TRUE;

         }

         rc = dot1qIntfDelete(intIfNum);
         break;
       }

    case L7_UP:
      break;
  
    case L7_DOWN:
      break;
  
    case L7_ACTIVE:
      rc = dot1qVlanIntfIntfChange(intIfNum, event);
      break;
  
    case L7_INACTIVE:
      rc = dot1qVlanIntfIntfChange(intIfNum, event);
      break;

  case L7_PROBE_SETUP:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_PORT_ROUTING_ENABLED:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_LAG_ACQUIRE:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
      break;
  case L7_DOT1X_ACQUIRE:
      COMPONENT_ACQ_SETMASKBIT(dot1qQports[intIfNum].acquiredList,L7_DOT1X_COMPONENT_ID);
      dot1qIntfUnapplyConfigData(intIfNum);
      break;
  case L7_PROBE_TEARDOWN:
      /* There are issues in the driver such that it is required that the operator
        manually reconfigure the vlans/garp/gmrp on probe teardown. We cannot
      automagically re-enable the vlan configuration. */
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;
  case L7_PORT_ROUTING_DISABLED:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID); 
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;
  case L7_LAG_RELEASE:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
      break;
  case L7_DOT1X_RELEASE:
      COMPONENT_ACQ_CLRMASKBIT(dot1qQports[intIfNum].acquiredList,L7_DOT1X_COMPONENT_ID);
      /* reapply membership from operational config */
      dot1qApplyIntfConfigData(intIfNum);
      break;

    default:
      break;
  }

  DOT1Q_TRACE("Calling nimStatusCB: rc = %d, reason = %d\n", status.response.rc, status.response.reason);
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Allow stats manager to get a dot1q related statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void dot1qStatGet(pCounterValue_entry_t c)
{
  c->status     = L7_SUCCESS;
  c->cSize      = C32_BITS;

  switch (c->cId)
  {
  case L7_PLATFORM_CTR_DYNAMIC_VLAN_COUNT:
    c->cValue.low = dot1qInfo->DynamicVLANCount;
    break;
  case L7_PLATFORM_CTR_MOST_VLAN_COUNT:
    c->cValue.low = dot1qInfo->MostVLANCount;
    break;
  case L7_PLATFORM_CTR_FAILED_VLAN_CREATIONS:
    c->cValue.low = dot1qInfo->FailedVLANCreations;
    break;
  case L7_PLATFORM_CTR_VLAN_DELETION_COUNT:
    c->cValue.low = dot1qInfo->VLANDeletionCount;
    break;
  default:
    c->status = dtlStatsGet(c->cKey, c->cId, c->cSize, &(c->cValue));
  }
}
