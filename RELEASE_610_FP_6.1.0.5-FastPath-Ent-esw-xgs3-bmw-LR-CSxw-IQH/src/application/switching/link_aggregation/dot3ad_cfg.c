/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3adcfg.c
*
* @purpose implements CFG
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @comments none
*
* @create 6/7/2001
*
* @author djohnson
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#define DOT3AD_STATS
#include "dot3ad_include.h"
#include "nvstoreapi.h"
#include "log.h"
#include "simapi.h"
#include "sysnet_api.h"
#include "defaultconfig.h"
#include "dot3ad_debug_api.h"
#include "support_api.h"
#include "dot3ad_exports.h"


dot3adCfg_t dot3adCfg;    /* Configuration File Overlay */
dot3adDebugCfg_t dot3adDebugCfg;    /* Debug Configuration File Overlay */
extern void dot3adDebugCfgUpdate(void);
L7_BOOL     defaultsBuilt = L7_FALSE;
L7_uint32   numLagIntfInit = 0;

extern dot3ad_agg_t    dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern dot3ad_port_t   dot3adPort[L7_MAX_PORT_COUNT + 1];
extern dot3ad_system_t dot3adSystem;
extern dot3ad_LagCnt_t dot3adLagCnt;
extern void * dot3adCreateSyncSema; 
extern void * dot3adAggIntfCreateSema;
extern dot3ad_stats_t  dot3ad_stats[L7_MAX_INTERFACE_COUNT];

/* provides intf->array index map */
extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1];

extern void * dot3adTaskSyncSema;
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];


L7_RC_t dot3adDefaultNameGet(L7_uint32 lagId, L7_uchar8* name, L7_uint32 size)
{
  osapiSnprintf(name, size, "%s%d", FD_DOT3AD_DEFAULT_LAG_NAME, lagId);
  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Saves dot3ad configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (dot3adHasDataChanged() == L7_TRUE)
  {
    rc = dot3adCfgUpdate();
    dot3adCfg.hdr.dataChanged = L7_FALSE;
    dot3adCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&dot3adCfg,
                                      (L7_uint32)(sizeof(dot3adCfg) - sizeof(dot3adCfg.checkSum)));                      
        /* call save NVStore routine */
    if ((rc = sysapiCfgFileWrite(L7_DOT3AD_COMPONENT_ID, DOT3AD_CFG_FILENAME , 
                            (L7_char8 *)&dot3adCfg, (L7_uint32)sizeof(dot3adCfg_t))) == L7_ERROR)
    {
      LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n",DOT3AD_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores dot3ad configuration
*           start the task
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adRestore(void)
{
  L7_RC_t rc;
  /* remove current config if necessary */
  /* Take the dot3adTaskSyncSema for data integrity*/
  (void)osapiSemaTake(dot3adTaskSyncSema, L7_WAIT_FOREVER);
  rc = aggConfiguredLagsRemove();
  (void)osapiSemaGive(dot3adTaskSyncSema);
  /* The data integrity is needed only as long as there are valid lag
   * present in the system. The above will ensure all the lags have 
   * been unconfigured. Once all lags have been deleted we are safe
   * to build defaults and apply them without the danger of compromising
   * current configured lags, hence the semaphore protection is limited
   * to the removal of configured lags.
   */
  dot3adBuildConfigData(DOT3AD_CFG_VER_CURRENT);
  rc = dot3adApplyConfigData();
  dot3adCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if dot1q user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adHasDataChanged(void)
{
  return dot3adCfg.hdr.dataChanged;
}
void dot3adResetDataChanged(void)
{
  dot3adCfg.hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Build default dot3ad config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adBuildConfigData(L7_uint32 ver)
{
  L7_uint32 i,j;
  L7_uint32 maxIntf,maxLagIntf;
  
  /* setup file header */
  dot3adCfg.hdr.version = ver;
  dot3adCfg.hdr.componentID = L7_DOT3AD_COMPONENT_ID;
  dot3adCfg.hdr.type = L7_CFG_DATA;
  dot3adCfg.hdr.length = (L7_uint32)sizeof(dot3adCfg);
  strcpy((L7_char8 *)dot3adCfg.hdr.filename, DOT3AD_CFG_FILENAME);
  dot3adCfg.hdr.dataChanged = L7_FALSE;

  /* zero overlays */
  memset((void*)&dot3adCfg.cfg.dot3adAgg, 0,    sizeof(dot3adCfg.cfg.dot3adAgg));
  memset((void*)&dot3adCfg.cfg.dot3adPort, 0,   sizeof(dot3adCfg.cfg.dot3adPort));
  memset((void*)&dot3adCfg.cfg.dot3adAggIdx, 0, sizeof(dot3adCfg.cfg.dot3adAggIdx));
  memset((void*)&dot3adCfg.cfg.dot3adPortIdx, 0 ,sizeof(dot3adCfg.cfg.dot3adPortIdx));
  /*Reset the lag counters */
  memset((void*)&dot3adLagCnt,0,sizeof(dot3ad_LagCnt_t));

  maxIntf = L7_MAX_PORT_COUNT + 1;
  maxLagIntf = platIntfLagIntfMaxCountGet();

  /* setup the LAGs */
  for (j=0 ;j < maxLagIntf; j++)
  {
#if 0
    (void)dot3adLagIntfDefaultBuild(0,j,ver,&dot3adCfg.cfg.dot3adAgg[j],&dot3adCfg.cfg.dot3adAggIdx[0]);
#else
    (void)dot3adLagIntfDefaultBuild(j,j,ver,&dot3adCfg.cfg.dot3adAgg[j],&dot3adCfg.cfg.dot3adAggIdx[j]);
#endif
  }

  /* setup the Physical interfaces */
  for (i=0; i < maxIntf; i++)
  {
    (void)dot3adPhysIntfDefaultBuild(i,ver,&dot3adCfg.cfg.dot3adPort[i],&dot3adCfg.cfg.dot3adPortIdx[i]);
  }

  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(dot3adCfg.cfg.dot3adSystem.actorSys.addr);
  else
    simGetSystemIPLocalAdminMac(dot3adCfg.cfg.dot3adSystem.actorSys.addr);

  dot3adCfg.cfg.dot3adSystem.actorSysPriority = FD_DOT3AD_DEFAULT_PRIORITY;
  dot3adCfg.cfg.dot3adSystem.hashMode = FD_DOT3AD_HASH_MODE;
  
  defaultsBuilt = L7_TRUE;
}

/*********************************************************************
* @purpose  Apply dot3ad config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot3adApplyConfigData(void)
{
  L7_uint32 intf, l, index, intIfNum;
  L7_uint32 tmpList[L7_MAX_MEMBERS_PER_LAG];
  L7_RC_t rc;
  L7_uint32 adminMode, lagId;

  bzero((char*)&dot3adAgg, (L7_int32)sizeof(dot3ad_agg_t));
  memcpy(&dot3adAgg, dot3adCfg.cfg.dot3adAgg, L7_MAX_NUM_LAG_INTF*sizeof(dot3ad_agg_t));

  bzero((char*)&dot3adPort, (L7_int32)sizeof(dot3ad_port_t)*(L7_MAX_PORT_COUNT + 1));
  memcpy(&dot3adPort, dot3adCfg.cfg.dot3adPort, (L7_MAX_PORT_COUNT + 1)*sizeof(dot3ad_port_t));

  bcopy((char*)&dot3adCfg.cfg.dot3adSystem, (char*)&dot3adSystem, (L7_int32)sizeof(dot3ad_system_t));

  bcopy((char*)&dot3adCfg.cfg.dot3adAggIdx, (char*)&dot3adAggIdx, (L7_int32)sizeof(dot3adAggIdx));
  bcopy((char*)&dot3adCfg.cfg.dot3adPortIdx, (char*)&dot3adPortIdx, (L7_int32)sizeof(dot3adPortIdx));


  if (defaultsBuilt == L7_FALSE)
  {
    rc = nimFirstValidIntfNumberByType(L7_PHYSICAL_INTF, &intIfNum);
    while (rc == L7_SUCCESS)
    {
      index = dot3adPortIdx[intIfNum];
      bzero((char*)&dot3adPort[index], (L7_int32)sizeof(dot3ad_port_t));

      dot3adPort[index].actorPortNum = dot3adCfg.cfg.dot3adPort[index].actorPortNum;
      dot3adPort[index].actorPortPri = dot3adCfg.cfg.dot3adPort[index].actorPortPri;

      dot3adPort[index].actorAdminPortKey = dot3adCfg.cfg.dot3adPort[index].actorPortNum;
      dot3adPort[index].actorAdminPortState = dot3adCfg.cfg.dot3adPort[index].actorAdminPortState;

      dot3adPort[index].actorOperPortKey = dot3adCfg.cfg.dot3adPort[index].actorPortNum;/*dot3adCfg.cfg.dot3adPort[index].actorOperPortKey;*/
      dot3adPort[index].actorOperPortState = dot3adCfg.cfg.dot3adPort[index].actorOperPortState;

      dot3adPort[index].lacpEnabled = dot3adCfg.cfg.dot3adPort[index].lacpEnabled;
      dot3adPort[index].portIndividualAgg = dot3adCfg.cfg.dot3adPort[index].portIndividualAgg;

      rc =  nimNextValidIntfNumberByType(L7_PHYSICAL_INTF,intIfNum,&intIfNum);
    }

    /* 2nd loop to ensure port struct loading is completed first */
    rc = nimFirstValidIntfNumberByType(L7_LAG_INTF, &intIfNum);
    while (rc == L7_SUCCESS)
    {

      index = dot3adAggIdx[intIfNum];
      bzero((char*)&dot3adAgg[index].partnerSys, (L7_int32)sizeof(L7_enetMacAddr_t));
      dot3adAgg[index].partnerSysPri = 0;
      dot3adAgg[index].partnerOperAggKey = 0;
      dot3adAgg[index].rxState = 0;
      dot3adAgg[index].txState = 0;

      dot3adAgg[index].currNumMembers = 0;
      dot3adAgg[index].currNumSelectedMembers = 0;
      dot3adAgg[index].activeNumMembers = 0;
      bzero((char*)&dot3adAgg[index].aggPortList, sizeof(L7_uint32) * L7_MAX_MEMBERS_PER_LAG);
      bzero((char*)&dot3adAgg[index].aggSelectedPortList, sizeof(L7_uint32) * L7_MAX_MEMBERS_PER_LAG);
      bzero((char*)&dot3adAgg[index].aggActivePortList, sizeof(L7_uint32) * L7_MAX_MEMBERS_PER_LAG);

      rc = nimGetIntfAddress(intIfNum, L7_NULL, dot3adAgg[index].aggMacAddr.addr);
      dot3adAgg[index].individualAgg = L7_TRUE;
      dot3adAgg[index].collectorMaxDelay = dot3adCfg.cfg.dot3adAgg[index].collectorMaxDelay;
      dot3adAgg[index].aggId = intIfNum;

      if (dot3adAgg[index].inuse == L7_TRUE)
      {
        dot3adAgg[index].inuse = L7_FALSE;
        bzero((char*)tmpList,sizeof(L7_uint32)*L7_MAX_MEMBERS_PER_LAG);
        for (l = 0; l<dot3adAgg[index].currNumWaitSelectedMembers; l++ )
        {
          rc = nimGetIntIfNumFromUSP((nimUSP_t *)&dot3adAgg[index].aggPortListUsp[l],&tmpList[l]);
        }

        /* save list and count */
        /*memcpy(&tmpList, &dot3adAgg[index].aggWaitSelectedPortList, sizeof(L7_uint32) * L7_MAX_MEMBERS_PER_LAG);
        tmpNum = dot3adAgg[index].currNumWaitSelectedMembers; */

        /* clear list and count */
        bzero((char*)&dot3adAgg[index].aggWaitSelectedPortList[0],
              sizeof(L7_uint32) * L7_MAX_MEMBERS_PER_LAG);
        bzero((char *)&dot3adAgg[index].aggPortListUsp[0],
              sizeof(dot3ad_usp_t) * L7_MAX_MEMBERS_PER_LAG);
        dot3adAgg[index].currNumWaitSelectedMembers = 0;
        adminMode = dot3adAgg[index].adminMode;
        dot3adAgg[index].adminMode = L7_ENABLE;

        /* reset tthe static mode of the Lag interface to false */
        dot3adAgg[index].isStatic = FD_DOT3AD_STATIC_MODE;


        /* recreate lag from saved values */
        lagId = index + 1;

		rc = dot3adPrivateLagCreate(lagId, (L7_char8 *)dot3adAgg[index].name, tmpList,
                                    adminMode, L7_ENABLE, L7_TRUE, dot3adAgg[index].hashMode, &intf);
	    /* Setting the linkTrap to ENABLE does not matter as this is ignored as the cfgCreated is set to TRUE*/
		

      }
      rc =  nimNextValidIntfNumberByType(L7_LAG_INTF,intIfNum,&intIfNum);
    }
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Update dot3ad information in the Cfg file
*
* @param    vlanID      VLAN ID 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
L7_RC_t   dot3adCfgUpdate(void)
{
  L7_uint32 idx,max;
  NIM_INTF_MASK_t mask;
  L7_uint32 intIfNum;

  /* update the physical interfaces */
  max = L7_MAX_PORT_COUNT;

  (void)nimPhysicalIntfMaskGet(&mask); 

  for (idx=1;idx <= max; idx++)
  {
    if (NIM_INTF_ISMASKBITSET(mask, idx))
    {
      memcpy(&dot3adCfg.cfg.dot3adPort[idx], &dot3adPort[idx], sizeof(dot3ad_port_t));
      dot3adCfg.cfg.dot3adPortIdx[idx] = dot3adPortIdx[idx];
    }
  }

  /* copy the lag interfaces now */
  max = platIntfLagIntfMaxCountGet();

  for (idx = 0;idx < max; idx++)
  {
    if (dot3adAgg[idx].inuse)
    {
      memcpy(&dot3adCfg.cfg.dot3adAgg[idx],  &dot3adAgg[idx],  sizeof(dot3ad_agg_t));

      intIfNum = dot3adAgg[idx].aggId;

      dot3adCfg.cfg.dot3adAggIdx[intIfNum] = dot3adAggIdx[intIfNum];
    }
  }

  /* update the system parms */
  memcpy(&dot3adCfg.cfg.dot3adSystem,&dot3adSystem,(L7_int32)sizeof(dot3ad_system_t));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void dot3adNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t            handle;

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT3AD_COMPONENT_ID,
            "DOT3AD: notification failed for event(%d), intf(%d), reason(%d)."
            " The event sent to NIM was not completed successfully",
            retVal.event,retVal.intIfNum,retVal.response.reason);
  }
  else
  {
    /* successful event notification */
    eventInfo.component = L7_DOT3AD_COMPONENT_ID;
    eventInfo.intIfNum  = retVal.intIfNum;
    eventInfo.pCbFunc   = dot3adNimEventCompletionCallback;

    switch (retVal.event)
    {
      case L7_CREATE:
        eventInfo.event     = L7_ATTACH;
        nimEventIntfNotify(eventInfo,&handle); 
        break;

      case L7_DETACH:
        eventInfo.event     = L7_DELETE;
        nimEventIntfNotify(eventInfo,&handle); 
        break;

      default:
        /* only care about create and detach for now */
        break;
    }
  }
}



/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This function is a wrapper for diff action on the completion callback
*           for the intf create event when port channel is created through usmdb.
*            
*       
* @end
*********************************************************************/
void dot3adNimEventCreateCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal)
{

  if (retVal.response.rc != L7_SUCCESS)
  {
    /* Failed to complete the request */
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT3AD_COMPONENT_ID,
            "DOT3AD: notification failed for event(%d), intf(%d), reason(%d)."
            " The event sent to NIM was not completed successfully",
            retVal.event,retVal.intIfNum,retVal.response.reason);
  }
  else
  {
   
    switch (retVal.event)
    {
	  case L7_CREATE:

        osapiSemaGive(dot3adCreateSyncSema);
		dot3adNimEventCompletionCallback(retVal);
        break;

	  default:
		LOG_MSG("%s: notification received for unknown event(%d), intf(%d), reason(%d)\n",
            retVal.event,retVal.intIfNum,retVal.response.reason);
        /* only care about create for now */
        break;
    }
  }
}

/*********************************************************************
* @purpose  Build default dot3ad data for a physical interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the phys interface
* @param    ver       @b{(input)}   Software version of Config Data
* @param    intf      @b{(output)}  the pointer to the interface
* @param    idx       @b{(output)}  the pointer to the appropriate idx array
*
* @returns  void
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
L7_RC_t dot3adPhysIntfDefaultBuild( L7_uint32 intIfNum, L7_uint32 ver, dot3ad_port_t *intf, L7_uint32 *idx)
{
  /* preset the port to zero */
  memset((void *)intf,0,sizeof(dot3ad_port_t));

  intf->portEnabled  = L7_FALSE;       
  intf->lacpEnabled  = FD_DOT3AD_DEFAULT_LACP_ENABLED;
  intf->begin       = L7_FALSE;
  intf->actorAdminPortKey = intIfNum;
  intf->actorOperPortKey = intIfNum;
  intf->actorPortNum = intIfNum;
  intf->actorPortPri = FD_DOT3AD_DEFAULT_PORT_PRIORITY;
  intf->portIndividualAgg = L7_TRUE;
  intf->actorAdminPortState = (L7_uchar8)(FD_DOT3AD_ACTOR_ADMIN_PORT_STATE);

  intf->partnerAdminPortState = (L7_uchar8)(FD_DOT3AD_PARTNER_ADMIN_PORT_STATE);

	intf->partnerOperPortState = intf->partnerAdminPortState;

  intf->actorOperPortState = intf->actorAdminPortState;

  *idx = intIfNum;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
* @param    ver       @b{(input)}   Software version of Config Data
* @param    agg       @b{(output)}  The agg port pointer to be modified
* @param    idx       @b{(output)}  The agg idx pointer to be modified
*
* @returns  L7_SUCCESS
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
L7_RC_t dot3adLagIntfDefaultBuild(L7_uint32 intIfNum, L7_uint32 lagId, L7_uint32 ver, 
                                  dot3ad_agg_t *agg, L7_uint32 *idx)
{
  L7_uchar8   defName[L7_DOT3AD_MAX_NAME];


  /* preset the agg to zero */
  memset((void *)&dot3adAgg[lagId],0,sizeof(dot3ad_agg_t));
  memset(defName, 0, sizeof(defName));

  if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_LAG_PRECREATE_FEATURE_ID) )
  {
    agg->inuse = L7_TRUE;
    if (lagId < platIntfLagIntfMaxCountGet())
    {
      dot3adDefaultNameGet((lagId+1), defName, L7_DOT3AD_MAX_NAME);
      osapiStrncpySafe(agg->name, defName, L7_DOT3AD_MAX_NAME);
    }
    agg->isStatic =  L7_TRUE;
  }
  else
  {
    agg->inuse = L7_FALSE;
    /* add default value for static mode of Lag */
    agg->isStatic =  FD_DOT3AD_STATIC_MODE;

  }
  agg->ready = L7_FALSE;

  agg->aggId = intIfNum;

  agg->actorAdminAggKey = intIfNum;
  agg->actorOperAggKey = intIfNum;

  agg->individualAgg = L7_TRUE;
  agg->collectorMaxDelay = 0;
  agg->adminMode = FD_DOT3AD_ADMIN_MODE;

  
  agg->hashMode = FD_DOT3AD_HASH_MODE;

  /* setup internal interface <-> index mapping */
  *idx = lagId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Interface creation from NIM
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    The LAG create is handled by the generator of the event.  This is done to insure 
*           lag has done the necessary creation before other components are made aware of
*           of the interface
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfCreate(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_INTF_TYPES_t type = 0;
  statsParm_entry_t dot3adCtr;
  L7_uint32 counterIdx;
  L7_uint32 listSize_port = (L7_uint32)(sizeof(dot3ad_counters_port) / sizeof(L7_uint32));
  L7_BOOL changedMode = L7_FALSE;

  if (nimGetIntfType(intIfNum, &type) != L7_SUCCESS)
  {
    /* failed to get the interface type, let's setup the callback */
    rc = L7_FAILURE;
    status->response.reason = NIM_ERR_RC_INTERNAL;
  }
  else if (type == L7_PHYSICAL_INTF)
  {
    /* set the config for the interface */
    memset((void *)&dot3adPort[intIfNum],0,sizeof(dot3ad_port_t));
	memset((void *)&dot3adOperPort[intIfNum], 0, sizeof(dot3adOperPort_t));

    /* create the counters for physcical interfaces and 3ad */
    dot3adCtr.cKey = 1;
    dot3adCtr.cSize = C32_BITS;
    dot3adCtr.cType = ExternalCtr;
    dot3adCtr.isResettable = L7_FALSE;
    dot3adCtr.pMutlingsParmList = L7_NULL;
    dot3adCtr.pSource = (L7_VOIDFUNCPTR_t) dot3adStatGet;

    dot3adCtr.cKey = intIfNum;
    for (counterIdx=0; (counterIdx < listSize_port && rc == L7_SUCCESS) ;counterIdx++)
    {
      dot3adCtr.cId = dot3ad_counters_port[counterIdx];
      rc = statsCreate(1, (pStatsParm_list_t)(&dot3adCtr));
    }

    /* get the config for the interface */
    dot3adPort[intIfNum].actorPortNum = dot3adCfg.cfg.dot3adPort[intIfNum].actorPortNum;
    dot3adPort[intIfNum].actorPortPri = dot3adCfg.cfg.dot3adPort[intIfNum].actorPortPri;

    dot3adPort[intIfNum].actorAdminPortKey = dot3adCfg.cfg.dot3adPort[intIfNum].actorPortNum;
    dot3adPort[intIfNum].actorAdminPortState = dot3adCfg.cfg.dot3adPort[intIfNum].actorAdminPortState;

    dot3adPort[intIfNum].actorOperPortKey = dot3adCfg.cfg.dot3adPort[intIfNum].actorPortNum;/*dot3adCfg.cfg.dot3adPort[index].actorOperPortKey;*/
    dot3adPort[intIfNum].actorOperPortState = dot3adCfg.cfg.dot3adPort[intIfNum].actorOperPortState;

    dot3adPort[intIfNum].lacpEnabled = dot3adCfg.cfg.dot3adPort[intIfNum].lacpEnabled;
    dot3adPort[intIfNum].portIndividualAgg = dot3adCfg.cfg.dot3adPort[intIfNum].portIndividualAgg;

   	/*Need to restore Partner Admin values*/
	  dot3adPort[intIfNum].partnerAdminPortNumber = dot3adCfg.cfg.dot3adPort[intIfNum].partnerAdminPortNumber;

	  dot3adPort[intIfNum].partnerAdminPortPri = dot3adCfg.cfg.dot3adPort[intIfNum].partnerAdminPortPri;

	  memcpy(dot3adPort[intIfNum].partnerOperSys.addr, dot3adCfg.cfg.dot3adPort[intIfNum].partnerOperSys.addr, L7_MAC_ADDR_LEN);

	  dot3adPort[intIfNum].partnerAdminSysPri = dot3adCfg.cfg.dot3adPort[intIfNum].partnerAdminSysPri;
    
	  dot3adPort[intIfNum].partnerAdminKey = dot3adCfg.cfg.dot3adPort[intIfNum].partnerAdminKey;

    dot3adPort[intIfNum].partnerAdminPortState = dot3adCfg.cfg.dot3adPort[intIfNum].partnerAdminPortState;

    dot3adPortIdx[intIfNum] = dot3adCfg.cfg.dot3adPortIdx[intIfNum];

    /* if the interface belongs to a lag, add it now */
    if (dot3adCfg.cfg.dot3adPort[intIfNum].actorPortWaitSelectedAggId != 0)
    {
      if (nimGetIntfType(dot3adCfg.cfg.dot3adPort[intIfNum].actorPortWaitSelectedAggId,&type) == L7_SUCCESS)
      {
        /* we must belong to a LAG interface */
        if (type == L7_LAG_INTF)
        {
          dot3ad_agg_t *agg;
          agg = dot3adAggIntfFind(dot3adCfg.cfg.dot3adPort[intIfNum].actorPortWaitSelectedAggId);

		  /* By definition we cannot manipulate lags which are administratively disable, checks are made further down,
		   * But in this case as we are trying to restore membership on a disabled lag and not change membership
		   * during normal operation hence we have to let this configuration go through. 
		   * To facilitate the processing of this command we change the adminmode of the aggregator to enable 
		   * and set it back to disable after placing this message in the lag message queue. 
		   * Since we are already in the dot3ad thread (and all operations on the lag happen via this message queue and task)
		   * , we are gaurenteed that this operation (of changing the adminmode)
		   * will NOT have any adverse effect. 
		   */

		  if(agg)
          {
			if (agg->adminMode == L7_DISABLE)
			{
              agg->adminMode = L7_ENABLE;
			  changedMode = L7_TRUE;
			}

            /* add the member to the agg, no hardware is touched here */
            rc = dot3adLagMemeberAdd(dot3adCfg.cfg.dot3adPort[intIfNum].actorPortWaitSelectedAggId, 1, &intIfNum);

			if (changedMode == L7_TRUE)
			{
			  agg->adminMode = L7_DISABLE;
			}
          }
        }
      }
    }
  }
  else
  {
    rc = L7_SUCCESS;
  }

  status->response.rc = rc;
  *callback = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfAttach(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback)
{
  L7_RC_t rc;
  L7_uint32 index;
  L7_uint32 tmpIntIfNum;
  nimUSP_t tmpUsp;
  L7_INTF_TYPES_t type = 0;
  dot3ad_port_t *p;


  if (nimGetIntfType(intIfNum, &type) != L7_SUCCESS)
  {
    /* failed to get the interface type, let's setup the callback */
    rc = L7_FAILURE;
  }
  else if (type == L7_PHYSICAL_INTF)
  { 
    /* 
     * call dtl to enable/disable lacp on the interface 
     * the dtl called is valid due to the event being an interface ATTACH 
     */
    if (dot3adPort[intIfNum].lacpEnabled == L7_TRUE)
      rc = dtlDot3adLacpSet(dot3adPort[intIfNum].actorPortNum,L7_ENABLE);
    else
      rc = dtlDot3adLacpSet(dot3adPort[intIfNum].actorPortNum,L7_DISABLE);

    /* if the interface belongs to a lag, add it now */
    if (dot3adPort[intIfNum].actorPortWaitSelectedAggId != 0)
    {
      if (nimGetIntfType(dot3adPort[intIfNum].actorPortWaitSelectedAggId,&type) == L7_SUCCESS)
      {
        /* we must belong to a LAG interface */
        if (type == L7_LAG_INTF)
        {
          rc = LACIssueCmd(lacpBegin, intIfNum, L7_NULL);
        }
      }
    }
  }
  else if (type == L7_LAG_INTF)
  {

  /* 
   * We are taking advantage of the fact that Config created LAGs are the only 
   * will have the cfg set.  In addition, there is not a problem with adding a 
   * member to the LAG more than once.  We are doing this here so that we insure 
   * we don't miss any interfaces before the LAG is finished being created
   */

    /* syncronize the agg creation between the caller and the event handler */
    osapiSemaTake(dot3adAggIntfCreateSema,L7_WAIT_FOREVER);

    /* for all interfaces that were in the lags config and are present, add them */
    for (index = 0; index <dot3adCfg.cfg.dot3adAgg[dot3adAggIdx[intIfNum]].currNumWaitSelectedMembers; index++ )
    {
      tmpUsp.unit = (L7_uchar8)dot3adCfg.cfg.dot3adAgg[dot3adAggIdx[intIfNum]].aggPortListUsp[index].unit;
      tmpUsp.slot = (L7_uchar8)dot3adCfg.cfg.dot3adAgg[dot3adAggIdx[intIfNum]].aggPortListUsp[index].slot;
      tmpUsp.port = (L7_ushort16)dot3adCfg.cfg.dot3adAgg[dot3adAggIdx[intIfNum]].aggPortListUsp[index].port;

      if (nimGetIntIfNumFromUSP(&tmpUsp, &tmpIntIfNum) == L7_SUCCESS)
      {
        p = dot3adPortIntfFind(tmpIntIfNum);
        if(p != L7_NULLPTR)
        (void)dot3adLagMemeberAdd(intIfNum, 1, &tmpIntIfNum);
      }
    }

    /* Apply the admin mode now that the interface is attached */
    rc = dot3adAdminModeSet(intIfNum,dot3adAgg[dot3adAggIdx[intIfNum]].adminMode);

    /* syncronize the agg creation between the caller and the event handler */
    osapiSemaGive(dot3adAggIntfCreateSema);
  }
  else
  {
    /* do nothing */
    rc = L7_SUCCESS;
  }

  status->response.rc = rc;
  *callback = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfDetach(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback)
{
  L7_RC_t rc;
  L7_INTF_TYPES_t type = 0;
  dot3ad_agg_t *a;
  dot3ad_port_t *p;
  L7_uint32 numMembers=0;
  L7_uint32 memberList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 idx;

  do
  {
    if (nimGetIntfType(intIfNum, &type) != L7_SUCCESS)
    {
      /* failed to get the interface type, let's setup the callback */
      rc = L7_FAILURE;
    }
    else if (type == L7_PHYSICAL_INTF)
    {
      /* also perform the linkdown sequence */
      /* only process DOWN events on Physical intf that are part of a LAG */
      if (dot3adIsLagMember(intIfNum) == L7_FALSE)
      {
        rc = L7_SUCCESS;
        break;
      }

      p = dot3adPortIntfFind(intIfNum);  

      if (p == L7_NULL)
      {
        rc = L7_SUCCESS;
        break;
      }

      /* call dtl to disable lacp on the interface 
       * the dtl called is valid due to the event being an interface DETACH 
       */
      rc = dtlDot3adLacpSet(dot3adPort[intIfNum].actorPortNum,L7_DISABLE);

      rc = aggPortResetValues(p);

      rc = dot3adLihMacroPortAssignmentReset(p->actorPortNum);

      rc = dot3adLihNotifySystem(p->actorPortNum, L7_LAG_RELEASE);

    }
    else if (type == L7_LAG_INTF)
    {
  
      a = dot3adAggIntfFind(intIfNum);

      if (a == L7_NULLPTR)
      {
        rc =  L7_FAILURE;
        break;
      }

      bzero((char *)memberList,sizeof(L7_uint32)* L7_MAX_MEMBERS_PER_LAG);

      rc = dot3adMemberListGet(intIfNum, &numMembers, memberList);

      /* Release each of the configured interfaces in the lag */
      if (rc != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
      else
      {
        for (idx = 0; idx < numMembers ; idx++)
        {
          p = dot3adPortIntfFind(memberList[idx]);  

          if (p != L7_NULL)
          {

            rc = aggPortResetValues(p);

            /*remove from waiting to be selected list*/
            rc = dot3adAggWaitSelectedDelete(intIfNum,memberList[idx]);

            rc = dot3adLihMacroPortAssignmentReset(memberList[idx]);

            rc = dot3adLihNotifySystem(memberList[idx], L7_LAG_RELEASE);

            p->actorPortWaitSelectedAggId = 0;

            /* default config the interface */
            (void)dot3adPhysIntfDefaultBuild(memberList[idx],
                                             DOT3AD_CFG_VER_CURRENT,
                                             p,
                                             &dot3adCfg.cfg.dot3adPortIdx[memberList[idx]]);

          }
        }

       /* decrement lag counters*/
        if (a->isStatic == L7_TRUE)
          dot3adLagCnt.StaticLagCnt--;
        else
          dot3adLagCnt.DynamicLagCnt--;

        /* notify the system of the LAG going down */
        dot3adLihNotifySystem(intIfNum,L7_DOWN);
      }

    }
    else
    {
      rc = L7_SUCCESS;
    }
  } while ( 0 );

  status->response.rc = rc;
  *callback = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfDelete(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_INTF_TYPES_t type = 0;
  dot3ad_port_t *p;

  do
  {

    if (nimGetIntfType(intIfNum, &type) != L7_SUCCESS)
    {
      /* failed to get the interface type, let's setup the callback */
        rc = L7_FAILURE;
    }
    else if (type == L7_PHYSICAL_INTF)
    {
      /* also perform the linkdown sequence */
      /* only process DOWN events on Physical intf that are part of a LAG */
      if (dot3adIsLagMember(intIfNum) == L7_FALSE) break;

      p = dot3adPortIntfFind(intIfNum);  

      if (p == L7_NULL) break;

      /*remove from waiting to be selected list*/
      rc = dot3adAggWaitSelectedDelete(p->actorPortWaitSelectedAggId,p->actorPortNum);

      p->actorPortWaitSelectedAggId = 0;

      p->actorAdminPortKey = intIfNum;

      /* we must have an interface that needs to be deleted */
      memset((void *)&dot3adCfg.cfg.dot3adPort[intIfNum],0,sizeof(dot3ad_port_t));
      memset((void *)&dot3adPort[intIfNum],0,sizeof(dot3ad_port_t));
	  memset((void *)&dot3adOperPort[intIfNum], 0, sizeof(dot3adOperPort_t));
      dot3adPortIdx[intIfNum] = 0;
      dot3adCfg.cfg.dot3adPortIdx[intIfNum] = 0;

      /* default config the interface */
      (void)dot3adPhysIntfDefaultBuild(intIfNum,
                                       DOT3AD_CFG_VER_CURRENT,
                                       &dot3adCfg.cfg.dot3adPort[intIfNum],
                                       &dot3adCfg.cfg.dot3adPortIdx[intIfNum]);
    }
    else if (type == L7_LAG_INTF)
    {

      (void)aggInuseSet(intIfNum, L7_FALSE);
      memset((void *)&dot3adCfg.cfg.dot3adAgg[dot3adAggIdx[intIfNum]],0,sizeof(dot3ad_agg_t));
      memset((void *)&dot3adAgg[dot3adAggIdx[intIfNum]],0,sizeof(dot3ad_agg_t));
      dot3adAggIdx[intIfNum] = 0;

    }
    else
    {
      rc = L7_SUCCESS;
    }
  } while ( 0 );

  status->response.rc = rc;
  *callback = L7_TRUE;

  return rc;
}



/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/

                      
                      
/*--------------  Start  of raw configuration parms -----------------------*/
/*  The following parms are what would ideally be in the configuration
     data structures instead of the entire operational data structure. 
*/     
      
#if 0    
   
typedef struct dot3ad_port_s
{
  L7_uint32         actorPortNum;
  L7_uint32         actorPortPri;
  L7_uint32         actorPortWaitSelectedAggId;                 /* TEST: zero default */
  
  L7_uint32         actorAdminPortKey;      /* intIfNum */
  L7_uchar8         actorAdminPortState;

  L7_enetMacAddr_t  partnerAdminSys;                            /* TEST: zero default */
  L7_uint32         partnerAdminSysPri;                         /* TEST: zero default */
  L7_uint32         partnerAdminKey;                            /* TEST: zero default */
  L7_uint32         partnerAdminPortNumber;                     /* TEST: zero default */
  L7_uint32         partnerAdminPortPri;                        /* TEST: zero default */
  L7_uchar8         partnerAdminPortState;              
  
  L7_BOOL           lacpEnabled;            /* to forward or drop LACPDUs */
  L7_BOOL           portIndividualAgg; /* L7_TRUE-aggregatable, L7_FALSE-individual, default */
} dot3ad_port_t;
typedef struct dot3ad_agg_s
{
  L7_uint32           aggId;            /* intIfNum of this logical interface */
  L7_BOOL             individualAgg;    /* L7_TRUE: default, aggregator represents an aggregate */
  L7_uint32           actorAdminAggKey; /* intIfNum of this logical interface */
  
  L7_enetMacAddr_t    partnerSys;                               /* TEST: zero default */
  L7_uint32           partnerSysPri;                            /* TEST: zero default */
  L7_uint32           partnerOperAggKey;                        /* TEST: zero default */

  L7_uint32           collectorMaxDelay;
  /* ports actively participating in aggregation*/
  /* ports attached to this aggregator */
  /* ports that have selected this aggregator */
  /* ports waiting to meet all selection requirements */
  L7_uint32           aggWaitSelectedPortList[L7_MAX_MEMBERS_PER_LAG];      /* TEST: zero default */ 
  L7_uint32           currNumWaitSelectedMembers;               /* TEST: zero default */  
  dot3ad_usp_t        aggPortListUsp[L7_MAX_MEMBERS_PER_LAG];   /* TEST: zero default */

  L7_uchar8           name[L7_DOT3AD_MAX_NAME];                 /* TEST: zero default */
  L7_uint32           adminMode; /*L7_ENABLE enabled, L7_DISABLE disabled*/
  L7_uint32           unused;
  L7_uint32           stpMode;                                  /* TEST: zero default */
} dot3adCfg_agg_t;
typedef struct dot3ad_system_s
{
  L7_uint32         actorSysPriority;
  L7_uint32			staticLag; /* Note : system wide static capability has been obsoleted*/
}dot3adCfg_system_t;
typedef struct
{
  dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];      /* holds per port into       */
  dot3adCfg_agg_t  dot3adAgg[L7_MAX_NUM_LAG_INTF];     /* holds per aggregator info */
  dot3adCfg_system_t dot3adSystem;                     /* holds system info         */
} dot3adCfg_Data_t;

*/
#endif
/*--------------  End of raw configuration parms -----------------------*/


/*  Function prototypes */
void dot3adBuildTestConfigData(void);  
void dot3adLagIntfTestBuild(L7_uint32 intIfNum, L7_uint32 lagId, 
                            dot3ad_agg_t *agg, L7_uint32 *idx);  
void dot3adPhysIntfTestBuild( L7_uint32 intIfNum, dot3ad_port_t *intf, L7_uint32 *idx); 
void dot3adConfigDataTestShow(void);
void dot3adLagIntfTestConfigDataShow(L7_uint32 intIfNum, L7_uint32 lagId);
void dot3adPhysIntfTestConfigDataShow( L7_uint32 intIfNum);
/* debug routine to view received LACPDUs dropped for members of static lag */
void dot3adDebugStaticLagIntfStatShow(L7_uint32 intIfNum);
/* debug routine to view number of static and dynamic lags in the system */
void dot3adDebugNumOfLagsShow();
/*********************************************************************
* @purpose  Build test dot3ad config data  
*
* @param    void   
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adBuildTestConfigData(void)
{
  L7_uint32 i,j;
  L7_uint32 maxIntf,maxLagIntf;
  

  maxIntf = L7_MAX_PORT_COUNT + 1;
  maxLagIntf = platIntfLagIntfMaxCountGet();

  /* setup the LAGs */
  for (j=0 ;j < maxLagIntf; j++)
  {
    (void)dot3adLagIntfTestBuild(j,j,&dot3adCfg.cfg.dot3adAgg[j],&dot3adCfg.cfg.dot3adAggIdx[j]);
  }

  /* setup the Physical interfaces */
  for (i=0; i < maxIntf; i++)
  {
    (void)dot3adPhysIntfTestBuild(i,&dot3adCfg.cfg.dot3adPort[i],&dot3adCfg.cfg.dot3adPortIdx[i]);
  }


#ifdef _NOT_A_CONFIG_PARM__
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(dot3adCfg.cfg.dot3adSystem.actorSys.addr);
  else
    simGetSystemIPLocalAdminMac(dot3adCfg.cfg.dot3adSystem.actorSys.addr);
#endif

  dot3adCfg.cfg.dot3adSystem.actorSysPriority = 0xABCD;
   
  /* Force write of config file */
  dot3adCfg.hdr.dataChanged = L7_TRUE;  
  sysapiPrintf("Built test config data\n");

}


/*********************************************************************
* @purpose  Build test dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
* @param    agg       @b{(output)}  The agg port pointer to be modified
* @param    idx       @b{(output)}  The agg idx pointer to be modified
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adLagIntfTestBuild(L7_uint32 intIfNum, L7_uint32 lagId, 
                                  dot3ad_agg_t *agg, L7_uint32 *idx)
{
  L7_uchar8 i;


#ifdef _NOT_A_CONFIG_PARM__

  agg->inuse = L7_FALSE;
  agg->ready = L7_FALSE;

  agg->aggId = intIfNum;
#endif

  agg->actorAdminAggKey = 0xABCD;


#ifdef _NOT_A_CONFIG_PARM__
  agg->actorOperAggKey = intIfNum;
#endif

  agg->individualAgg = L7_FALSE;
  agg->collectorMaxDelay = 7;
  agg->adminMode = L7_DISABLE;
  
  
#ifdef _NOT_A_CONFIG_PARM__
  /* setup internal interface <-> index mapping */
  *idx = lagId;
#endif


  /*--------------------------------------------------------------------*/
  /*  START OF ITEMS WHICH ARE NOT IN NORMAL xxxxBuildDefaultConfigData */
  /*--------------------------------------------------------------------*/

  /* Ensure at least one entry in each array has a unique value to validate
     accurate migration */
  
  memset( (void *)&(agg->partnerSys), 0xbb, 6);

  agg->partnerSysPri = 0xABCD + lagId;
  agg->partnerOperAggKey = 0xABCD + lagId + 1;

  for (i=0; i < L7_MAX_MEMBERS_PER_LAG; i++) 
  {
      agg->aggWaitSelectedPortList[i] = (i+1);
  }

  agg->currNumWaitSelectedMembers = 3 + lagId;


  agg->aggPortListUsp[0].unit = 0;
  agg->aggPortListUsp[0].slot = 0;
  agg->aggPortListUsp[0].port = 22;

      
  sprintf( (void *)&(agg->name), "LAG ID - %u", lagId);

  agg->stpMode = L7_DISABLE;

  /* add non default configuration for static mode of lag */
  agg->isStatic = L7_TRUE;


}



/*********************************************************************
* @purpose  Build default dot3ad data for a physical interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the phys interface
* @param    intf      @b{(output)}  the pointer to the interface
* @param    idx       @b{(output)}  the pointer to the appropriate idx array
*
* @returns  void
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
void dot3adPhysIntfTestBuild( L7_uint32 intIfNum, dot3ad_port_t *intf, L7_uint32 *idx)
{


    /* Ensure at least one entry in each array has a unique value to validate
       accurate migration */
   
#ifdef _NOT_A_CONFIG_PARM__
  intf->portEnabled  = L7_FALSE;
#endif
  intf->lacpEnabled  = L7_FALSE;


#ifdef _NOT_A_CONFIG_PARM__
  intf->begin       = L7_FALSE;
#endif
  intf->actorAdminPortKey = 0xABCD + intIfNum;

#ifdef _NOT_A_CONFIG_PARM__
  intf->actorOperPortKey = intIfNum;
#endif
  intf->actorPortNum = 0xABCD + intIfNum + 1;
  intf->actorPortPri = 0x20 + intIfNum;
  intf->portIndividualAgg = L7_FALSE;
  intf->actorAdminPortState = 0xFF;

  intf->partnerAdminPortState = 0xFE;


#ifdef _NOT_A_CONFIG_PARM__
	intf->partnerOperPortState = intf->partnerAdminPortState;

  intf->actorOperPortState = intf->actorAdminPortState;

  *idx = intIfNum;

#endif

  /*--------------------------------------------------------------------*/
  /*  START OF ITEMS WHICH ARE NOT IN NORMAL xxxxBuildDefaultConfigData */
  /*--------------------------------------------------------------------*/

  intf->actorPortWaitSelectedAggId = 0xABCD;
  memset( (void *)&(intf->partnerAdminSys), 0xaa, 6);

  intf->partnerAdminSysPri = 0xABCD + intIfNum;
  intf->partnerAdminKey = 0xABCD + intIfNum + 1;
  intf->partnerAdminPortNumber = 0xABCD + intIfNum + 2;
  intf->partnerAdminPortPri = 0xABCD + intIfNum + 3 ;

  return;
}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot3adConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32 i,j;
    L7_uint32 maxIntf,maxLagIntf;

    
    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &dot3adCfg.hdr;
    sysapiPrintf("pFileHdr->filename    : %s\n", (char *)pFileHdr->filename);
    sysapiPrintf("pFileHdr->version     : %u\n", pFileHdr->version);
    sysapiPrintf("pFileHdr->componentID : %u\n", pFileHdr->componentID);
    sysapiPrintf("pFileHdr->type        : %u\n", pFileHdr->type);
    sysapiPrintf("pFileHdr->length      : %u\n", pFileHdr->length);
    sysapiPrintf("pFileHdr->dataChanged : %u\n", pFileHdr->dataChanged);

    /* Start of release I file header changes */

    sysapiPrintf("pFileHdr->savePointId : %u\n", pFileHdr->savePointId);
    sysapiPrintf("pFileHdr->targetDevice: %u\n", pFileHdr->targetDevice);
    for (i = 0; i < L7_FILE_HDR_PAD_LEN; i++)
    {
        sysapiPrintf("pFileHdr->pad[i]      : %u\n", pFileHdr->pad[i]);
    }

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    sysapiPrintf("\n");
    sysapiPrintf("DOT3AD\n");
    sysapiPrintf("=====\n");



    sysapiPrintf("dot3adCfg.cfg.dot3adSystem.actorSysPriority = %x\n",
                 dot3adCfg.cfg.dot3adSystem.actorSysPriority);


    maxIntf = L7_MAX_PORT_COUNT + 1;
    maxLagIntf = platIntfLagIntfMaxCountGet();

    /* setup the LAGs */
    for (j=0 ;j < maxLagIntf; j++)
    {
      (void)dot3adLagIntfTestConfigDataShow(j,j);
    }

    /* setup the Physical interfaces */
    for (i=0; i < maxIntf; i++)
    {
      (void)dot3adPhysIntfTestConfigDataShow(i);
    }




    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("dot3adCfg->checkSum : %u\n", dot3adCfg.checkSum);

}

   



/*********************************************************************
* @purpose  Show dot3ad config data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adLagIntfTestConfigDataShow(L7_uint32 intIfNum, L7_uint32 lagId)
{
  L7_uchar8 i;
  dot3ad_agg_t *agg;

  agg = &dot3adCfg.cfg.dot3adAgg[intIfNum];

  sysapiPrintf("\n");
  sysapiPrintf("LAG ID %d  Config Data\n", lagId);
  sysapiPrintf("----------------------\n");

  sysapiPrintf("agg->inuse = %x\n",agg->inuse);
  sysapiPrintf("agg->ready = %x\n",agg->ready);
  sysapiPrintf("agg->aggId = %x\n",agg->aggId);

  agg->actorAdminAggKey = 0xABCD;

  sysapiPrintf("agg->actorAdminAggKey = %x\n",agg->actorAdminAggKey);


#ifdef _NOT_A_CONFIG_PARM__
  agg->actorOperAggKey = intIfNum;
#endif

  sysapiPrintf("agg->individualAgg = %x\n",agg->individualAgg);
  sysapiPrintf("agg->collectorMaxDelay = %x\n",agg->collectorMaxDelay);
  sysapiPrintf("agg->adminMode = %x\n",agg->adminMode);
  sysapiPrintf("agg->actorAdminAggKey = %x\n",agg->actorAdminAggKey);
 

#ifdef _NOT_A_CONFIG_PARM__
  /* setup internal interface <-> index mapping */
  *idx = lagId;
#endif


  /*--------------------------------------------------------------------*/
  /*  START OF ITEMS WHICH ARE NOT IN NORMAL xxxxBuildDefaultConfigData */
  /*--------------------------------------------------------------------*/

  sysapiPrintf("agg->partnerSys = ");
  for (i = 0;i < 5 ;i++)
  {
    sysapiPrintf("%0.2x:",agg->partnerSys.addr[i]);
  }

  sysapiPrintf("%0.2x\n",agg->partnerSys.addr[5]);
  
  
  sysapiPrintf("agg->partnerSysPri = %x\n",agg->partnerSysPri);
  sysapiPrintf("agg->partnerOperAggKey = %x\n",agg->partnerOperAggKey);


  for (i=0; i < L7_MAX_MEMBERS_PER_LAG; i++) 
  {

      sysapiPrintf("agg->partnerOperAggKey[%d] = %x\n",i, agg->aggWaitSelectedPortList[i]);
  }


  sysapiPrintf("agg->currNumWaitSelectedMembers = %x\n",agg->currNumWaitSelectedMembers);



  for (i=0; i < L7_MAX_MEMBERS_PER_LAG; i++) 
  {

      sysapiPrintf("agg->aggPortListUsp[%d]:  usp = %x/%x/%x\n", i, 
                   agg->aggPortListUsp[i].unit,
                   agg->aggPortListUsp[i].slot,
                   agg->aggPortListUsp[i].port);
  }


  sysapiPrintf("agg->currNumWaitSelectedMembers = %x\n",agg->currNumWaitSelectedMembers);
  
  sysapiPrintf("agg->name = %s \n",agg->name);

  sysapiPrintf("agg->stpMode = %x\n",agg->stpMode);

  /* print the static mode per lag */
  sysapiPrintf("agg->isStatic = %x\n",agg->isStatic);

  /* print the hashing mode */
  sysapiPrintf("agg->hashMode = %x\n",agg->hashMode);

}



/*********************************************************************
* @purpose  Show  dot3ad config data for a physical interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the phys interface
* @param    intf      @b{(output)}  the pointer to the interface
* @param    idx       @b{(output)}  the pointer to the appropriate idx array
*
* @returns  void
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
void dot3adPhysIntfTestConfigDataShow( L7_uint32 intIfNum)
{
  L7_uchar8 i;
  dot3ad_port_t *intf;

  intf=  &dot3adCfg.cfg.dot3adPort[intIfNum];

  sysapiPrintf("\n");
  sysapiPrintf("intIfNum %x Config Data\n", intIfNum);
  sysapiPrintf("-----------------------\n");

#ifdef _NOT_A_CONFIG_PARM__
  intf->portEnabled  = L7_FALSE;
#endif

  sysapiPrintf("intf->lacpEnabled = %x\n",intf->lacpEnabled);  

#ifdef _NOT_A_CONFIG_PARM__
  intf->begin       = L7_FALSE;
#endif

  sysapiPrintf("intf->actorAdminPortKey = %x\n",intf->actorAdminPortKey);  

#ifdef _NOT_A_CONFIG_PARM__
  intf->actorOperPortKey = intIfNum;
#endif

  sysapiPrintf("intf->actorPortNum = %x\n",intf->actorPortNum);  
  sysapiPrintf("intf->actorPortPri = %x\n",intf->actorPortPri);  
  sysapiPrintf("intf->portIndividualAgg = %x\n",intf->portIndividualAgg);  
  sysapiPrintf("intf->actorAdminPortState = %x\n",intf->actorAdminPortState);  
  sysapiPrintf("intf->partnerAdminPortState = %x\n",intf->partnerAdminPortState); 

  sysapiPrintf("intf->actorAdminPortKey = %x\n",intf->actorAdminPortKey);
  sysapiPrintf("intf->actorAdminPortKey = %x\n",intf->actorAdminPortKey);  
  sysapiPrintf("intf->actorAdminPortKey = %x\n",intf->actorAdminPortKey);

#ifdef _NOT_A_CONFIG_PARM__
	intf->partnerOperPortState = intf->partnerAdminPortState;

  intf->actorOperPortState = intf->actorAdminPortState;

  *idx = intIfNum;

#endif

  /*--------------------------------------------------------------------*/
  /*  START OF ITEMS WHICH ARE NOT IN NORMAL xxxxBuildDefaultConfigData */
  /*--------------------------------------------------------------------*/


  sysapiPrintf("intf->actorPortWaitSelectedAggId = %x\n",intf->actorPortWaitSelectedAggId);

  for (i = 0;i < 5 ;i++)
  {
    sysapiPrintf("%0.2x:",intf->partnerAdminSys.addr[i]);
  }

  sysapiPrintf("%0.2x\n",intf->partnerAdminSys.addr[5]);
  


  sysapiPrintf("intf->partnerAdminSysPri = %x\n",intf->partnerAdminSysPri);  
  sysapiPrintf("intf->partnerAdminKey = %x\n",intf->partnerAdminKey);
  sysapiPrintf("intf->partnerAdminPortNumber = %x\n",intf->partnerAdminPortNumber);
  sysapiPrintf("intf->partnerAdminPortPri = %x\n",intf->partnerAdminPortPri);

  
  return;
}

/*********************************************************************
* @purpose  Show  dot3ad RxLACPDUsDropped statistic for a given 
*           physical interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the phys interface
*
* @returns  void
*
* @notes    This function only prints the value of the statistic if 
*           the interface is a member of a lag and the lag is static
*
* @end
*********************************************************************/
void dot3adDebugStaticLagIntfStatShow(L7_uint32 intIfNum)
{
  dot3ad_port_t *p;
  dot3ad_agg_t *agg;

  p = dot3adPortIntfFind(intIfNum);
  if (p != L7_NULLPTR)
  {
    if (p->actorPortWaitSelectedAggId != 0)
    {
      agg = dot3adAggIntfFind(p->actorPortWaitSelectedAggId);
      if (agg != L7_NULLPTR)
      {
        if (agg->isStatic == L7_TRUE)
        {
          sysapiPrintf("\n Port : %d\n ",intIfNum);
          sysapiPrintf("RxLACPDUsDropped : %d\n ",dot3ad_stats[intIfNum].RxLACPDUsDropped);

        }
        else
          sysapiPrintf("\nError ! Lag is not static.\n");
       }
      else 
        sysapiPrintf("\nError ! Lag not configured.\n");
    } 
    else
      sysapiPrintf("\nPort %d does not belong to a lag.\n ",intIfNum);
  }/* p! = Null*/
  else
    sysapiPrintf("\nInvalid port number.\n");
  return;
}

/*********************************************************************
* @purpose  Show number of static and dynamic lags in the system 
*
* @param   
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
 void dot3adDebugNumOfLagsShow()
{

  sysapiPrintf("\n Number of Static Lags : %d ",dot3adLagCnt.StaticLagCnt);
  sysapiPrintf("\n Number of Dynamic Lags : %d",dot3adLagCnt.DynamicLagCnt); 
}


/*********************************************************************
* @purpose  Saves dot3ad configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  dot3adDebugCfgUpdate();

  if (dot3adDebugHasDataChanged() == L7_TRUE)
  {
    dot3adDebugCfg.hdr.dataChanged = L7_FALSE;
    dot3adDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&dot3adDebugCfg,
                                      (L7_uint32)(sizeof(dot3adDebugCfg) - sizeof(dot3adDebugCfg.checkSum)));
        /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_DOT3AD_COMPONENT_ID, DOT3AD_DEBUG_CFG_FILENAME , 
                            (L7_char8 *)&dot3adDebugCfg, (L7_uint32)sizeof(dot3adDebugCfg_t))) == L7_ERROR)
    {
      LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n",DOT3AD_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores dot3ad debug configuration
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adDebugRestore(void)
{
  L7_RC_t rc;

  dot3adDebugBuildDefaultConfigData(DOT3AD_DEBUG_CFG_VER_CURRENT);

  dot3adDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = dot3adApplyDebugConfigData();

  return rc;
}
/*********************************************************************
* @purpose  Checks if dot3ad debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adDebugHasDataChanged(void)
{
  return dot3adDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default dot3ad config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adDebugBuildDefaultConfigData(L7_uint32 ver)
{
  
  /* setup file header */
  dot3adDebugCfg.hdr.version = ver;
  dot3adDebugCfg.hdr.componentID = L7_DOT3AD_COMPONENT_ID;
  dot3adDebugCfg.hdr.type = L7_CFG_DATA;
  dot3adDebugCfg.hdr.length = (L7_uint32)sizeof(dot3adDebugCfg);
  strcpy((L7_char8 *)dot3adDebugCfg.hdr.filename, DOT3AD_DEBUG_CFG_FILENAME);
  dot3adDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&dot3adDebugCfg.cfg, 0, sizeof(dot3adDebugCfg.cfg));
}                     

/*********************************************************************
* @purpose  Apply dot3ad debug config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot3adApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = dot3adDebugPacketTraceFlagSet(dot3adDebugCfg.cfg.dot3adDebugPacketTraceFlag);

  return rc;
}

