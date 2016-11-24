/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    filter_cfg.c
* @purpose     Static MAC Filtering configuration functions
* @component   Filter
* @comments    none
* @create      1/7/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "flex.h"
#include "l7_common.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "sysapi.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "dtlapi.h"
#include "filter_api.h"
#include "filter.h"
#include "avl_api.h"
#include "osapi.h"
#include "dot1q_api.h"

// PTin added
L7_FILTER_VLAN_FILTER_MODE_t ptin_filter_vlan_mode = FD_VLAN_FILTERING_MODE;

/*
filterInfoData_t* firstFilter = L7_NULLPTR;
filterInfoData_t* lastFilter = L7_NULLPTR;
L7_uint32 countFilter=0;
*/

avlTree_t          filterAvlTree;
avlTreeTables_t    *filterTreeHeap  = L7_NULLPTR;
filterInfoData_t   *filterDataHeap  = L7_NULLPTR;
filterCfgData_t    *filterCfgData   = L7_NULLPTR;
L7_uint32          *filterMapTbl    = L7_NULLPTR;
filterDeregister_t filterDeregister = {L7_FALSE, L7_FALSE, L7_FALSE, L7_FALSE};

extern filterCnfgrState_t filterCnfgrState;

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t filterRC;
  L7_uint32 maxNumEntries;
  
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  filterRC   = L7_SUCCESS;

  /* determine maximum number of entries alllowed */
  maxNumEntries =  L7_MAX_STATIC_FILTER_ENTRIES;
  /* allocate memory for global data and make sure allocation succeded */
  filterCfgData = osapiMalloc(L7_FILTER_COMPONENT_ID, sizeof(filterCfgData_t));
  filterMapTbl  = osapiMalloc(L7_FILTER_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
 
  filterTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_FILTER_COMPONENT_ID, sizeof(avlTreeTables_t) * 
                                                  maxNumEntries);
  filterDataHeap = (filterInfoData_t *)osapiMalloc(L7_FILTER_COMPONENT_ID, sizeof(filterInfoData_t) * 
                                                   maxNumEntries); 
  
  if ((filterCfgData == L7_NULLPTR)  || (filterMapTbl == L7_NULLPTR) || 
      (filterTreeHeap == L7_NULLPTR) || (filterDataHeap == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    filterRC   = L7_ERROR;

    LOG_MSG("Filter: Unable to allocate resources\n");
    return filterRC;
  }

  memset((void *)filterCfgData, 0x00, sizeof(filterCfgData_t));
  memset((void *)filterMapTbl, 0x00, sizeof(L7_uint32) * platIntfMaxCountGet());
  
  memset((void *)filterTreeHeap, 0x00, sizeof(avlTreeTables_t) * maxNumEntries );
  memset((void *)filterDataHeap, 0x00, sizeof(filterInfoData_t) * maxNumEntries);

  avlCreateAvlTree(&filterAvlTree, filterTreeHeap, filterDataHeap, maxNumEntries,
                   (L7_uint32)sizeof(filterInfoData_t), 0x10, sizeof(L7_uchar8)*L7_FDB_KEY_SIZE);


  filterCnfgrState = FILTER_PHASE_INIT_1;

  return filterRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t filterRC;
  nvStoreFunctionList_t notifyFunctionList;
  L7_uint32 eventMask = 0;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  filterRC   = L7_SUCCESS;

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_FILTER_COMPONENT_ID;
  notifyFunctionList.notifySave     = filterSave;
  notifyFunctionList.hasDataChanged = filterHasDataChanged;
  notifyFunctionList.resetDataChanged = filterResetDataChanged;

  filterRC = nvStoreRegister(notifyFunctionList);
  if ( filterRC != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    LOG_MSG("Filter: Unable to register with nvStore\n");
    return filterRC;
  }

  filterDeregister.filterSave = L7_FALSE;
  filterDeregister.filterRestore = L7_FALSE;
  filterDeregister.filterHasDataChanged = L7_FALSE;

  /* register NIM callback to support interface changes */
  if ((filterRC = nimRegisterIntfChange(L7_FILTER_COMPONENT_ID,
                                        filterIntfChangeCallback)) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    LOG_MSG("Filter: Unable to register with NIM\n");
    return filterRC;
  }

  filterDeregister.filterIntfChangeCallback = L7_FALSE;
  eventMask = VLAN_ADD_NOTIFY;
  /*register Vlan callback to support creation of staic muticast filter entries -defect 64455*/
  if ((filterRC=vlanRegisterForChange(filterVlanChangeCallback,
                                      L7_FILTER_COMPONENT_ID, eventMask)) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    LOG_MSG("Filter unable to register for Vlan Change callback");

  }
  
  filterDeregister.filterVlanChangeCallback = L7_FALSE; 
  

  filterCnfgrState = FILTER_PHASE_INIT_2;

  return filterRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t filterRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  filterRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_FILTER_COMPONENT_ID, FILTER_CFG_FILENAME,
                       (L7_char8 *)filterCfgData, sizeof(filterCfgData_t),
                       &(filterCfgData->checkSum), FILTER_CFG_VER_CURRENT,
                       filterBuildDefaultConfigData, filterMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    return filterRC;
  }

  if (filterApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    return filterRC;
  }

  filterCnfgrState = FILTER_PHASE_INIT_3;

  return filterRC;
}


/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase1Process()
{
  if (filterCfgData != L7_NULLPTR)
  {
    osapiFree(L7_FILTER_COMPONENT_ID, (void *)filterCfgData);
    filterCfgData = L7_NULLPTR;
  }

  if (filterMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_FILTER_COMPONENT_ID, (void *)filterMapTbl);
    filterMapTbl = L7_NULLPTR;
  }

  if (filterTreeHeap != L7_NULLPTR)
  {
    (void)memset(filterTreeHeap, 0x00, sizeof(avlTreeTables_t)*L7_MAX_STATIC_FILTER_ENTRIES);
    osapiFree(L7_FILTER_COMPONENT_ID, (void *)filterTreeHeap);
    filterTreeHeap = L7_NULLPTR;
  }

  if (filterDataHeap != L7_NULLPTR)
  {
    (void)memset(filterDataHeap, 0x00, sizeof(filterInfoData_t)*L7_MAX_STATIC_FILTER_ENTRIES);
    osapiFree(L7_FILTER_COMPONENT_ID, (void *)filterDataHeap);
    filterDataHeap = L7_NULLPTR;
  }

  avlPurgeAvlTree(&filterAvlTree, L7_MAX_STATIC_FILTER_ENTRIES);
  if (filterAvlTree.semId != L7_NULLPTR)
    if (osapiSemaDelete((void *)filterAvlTree.semId) != L7_SUCCESS)
      LOG_MSG("filterCnfgrFiniPhase1Process:  could not delete avl semaphore\n");

  filterCnfgrState = FILTER_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase2Process()
{
  /* 
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the filterDeregister is set to L7_FALSE;
   */

  filterDeregister.filterSave = L7_TRUE;
  filterDeregister.filterRestore = L7_TRUE;
  filterDeregister.filterHasDataChanged = L7_TRUE;
  filterDeregister.filterIntfChangeCallback = L7_TRUE;

  filterCnfgrState = FILTER_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes filterCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void filterCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  filterCnfgrUconfigPhase1(&response, &reason);
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t filterRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(filterRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request 
*           pair Unconfigure Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t filterCnfgrUconfigPhase1(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t filterRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  filterRC   = L7_SUCCESS;

  /* Even though filters are system-wide they have a per-interface component
   * so they must be deleted in Unconfig Phase 1.
   */
  if (filterDeleteAllEntries() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    filterRC   = L7_ERROR;

    return filterRC;
  }

  memset((void *)filterCfgData, 0, sizeof(filterCfgData_t));
  filterCnfgrState = FILTER_PHASE_WMU;

  return filterRC;
}

/*********************************************************************
* @purpose  Delete all configured filters
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterDeleteAllEntries(void)
{
  filterInfoData_t* filterEntry;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_RC_t rc;

  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  rc = osapiSemaTake(filterAvlTree.semId, L7_WAIT_FOREVER);
  
  /* get the first entry */
  filterEntry = filterEntryFind(macAddr, 1, L7_MATCH_GETNEXT);
  while (filterEntry != L7_NULLPTR)
  {
    /* extract the vlanId and Macaddress */
    rc = filterMacVlanIdExtract(filterEntry->vlanidmacAddr, macAddr, &vlanId);
    rc = osapiSemaGive(filterAvlTree.semId);

    /* delete this entry */
    rc = filterRemove(macAddr, vlanId);

    /* above routine will remove this entry from avl tree 
     * and send msg to delete it from the microcode as well 
     */
    rc = osapiSemaTake(filterAvlTree.semId, L7_WAIT_FOREVER);
    filterEntry = filterEntryFind(macAddr, vlanId, L7_MATCH_GETNEXT);
  }

  rc = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
* @purpose  Saves filter user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterSave(void)
{
  L7_RC_t rc=L7_SUCCESS;

  if (filterDeregister.filterSave == L7_TRUE)
    return L7_SUCCESS;

  if (filterCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    /*copy the latest user data from the filterInfoData to the filterCfgData structure*/
    rc = filterDataCopy();

    filterCfgData->cfgHdr.dataChanged = L7_FALSE;
    filterCfgData->checkSum = nvStoreCrc32((L7_uchar8 *)filterCfgData, 
                                           (L7_uint32)(sizeof(filterCfgData_t) - sizeof(filterCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_FILTER_COMPONENT_ID, FILTER_CFG_FILENAME, 
                           (L7_char8 *)filterCfgData, 
                           ((L7_uint32)sizeof(filterCfgData_t))) != L7_SUCCESS)
      LOG_MSG("filterSave: Error on call to sysapiCfgFileWrite routine on file %s\n", FILTER_CFG_FILENAME);
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores filter user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterRestore(void)
{
  L7_RC_t rc;

  if (filterDeregister.filterRestore == L7_TRUE)
    return L7_SUCCESS;

  /* delete all configured filters */
  rc = filterDeleteAllEntries();

  filterBuildDefaultConfigData(filterCfgData->cfgHdr.version);
  rc = filterApplyConfigData();
  filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  return rc;
}

/*********************************************************************
* @purpose  Checks if filter user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL filterHasDataChanged(void)
{
  if (filterDeregister.filterHasDataChanged == L7_TRUE)
    return L7_FALSE;

  return filterCfgData->cfgHdr.dataChanged;
}
void filterResetDataChanged(void)
{
  filterCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply Filter Configuration Data (less interface info)
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterApplyConfigData(void)
{
  L7_uint32 i;
  L7_RC_t rc1, rc = L7_SUCCESS;
  filterInfoData_t* filterEntry;

  i = 0;
  while (filterCfgData->filterData[i].inUse == L7_TRUE)
  {
    /*creating the mac filter in the operational data structure*/
    rc = filterCreate(filterCfgData->filterData[i].macAddr, 
                      filterCfgData->filterData[i].vlanId);

    filterEntry = filterEntryFind(filterCfgData->filterData[i].macAddr, 
                                  filterCfgData->filterData[i].vlanId,L7_MATCH_EXACT);
    if (filterEntry !=L7_NULLPTR)
    {
        /*adding the relevant interfaces to the operational data structure*/
        rc1 = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
        filterEntry->numSrc = filterCfgData->filterData[i].numSrc;
        memcpy(&filterEntry->srcIntfMask, &filterCfgData->filterData[i].srcIntfMask,
                sizeof(filterEntry->srcIntfMask));
        memcpy(&filterEntry->dstIntfMask, &filterCfgData->filterData[i].dstIntfMask,
               sizeof(filterEntry->dstIntfMask));
        rc1 = osapiSemaGive(filterAvlTree.semId);
    }

    i++;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply Filter Interface Config Data
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t filterApplyIntfConfigData(L7_uint32 intIfNum)
{
  L7_uint32 vlanId, prevVlanId ;
 /* filterIntfCfgData_t *pCfg;*/
  filterInfoData_t* filterEntry;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN], prevMacAddr[L7_MAC_ADDR_LEN];
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i = 0;
  filterData_t filterCfg;
  L7_uchar8 cfgMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32 cfgVlanId;

  rc = filterFirstGet(macAddr, &vlanId);

  while (rc == L7_SUCCESS)
  {
	memset(&filterCfg, 0, sizeof(filterData_t));

	filterEntry = filterEntryFind(macAddr, vlanId, L7_MATCH_EXACT);

	  if (filterEntry != L7_NULLPTR)
	  {
		for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
		{
		  if (filterCfgData->filterData[i].inUse == L7_TRUE)
		  {
			if (filterMacVlanIdExtract(filterEntry->vlanidmacAddr,cfgMacAddr,&cfgVlanId) == L7_SUCCESS)
			{
			  if (memcmp(filterCfgData->filterData[i].macAddr,cfgMacAddr,L7_MAC_ADDR_LEN) == 0 &&
				  filterCfgData->filterData[i].vlanId == cfgVlanId)
			  {
                memcpy(&filterCfg, &filterCfgData->filterData[i], sizeof(filterData_t));
				break;
			  }
			}
		  }
		}
      }

      if (filterEntry != L7_NULLPTR && 
		  ((L7_INTF_ISMASKBITSET(filterEntry->dstIntfMask, intIfNum)) ||
		   (filterCfg.inUse == L7_TRUE && 
			L7_INTF_ISMASKBITSET(filterCfg.dstIntfMask, intIfNum))))
        rc = filterDstIntfAdd(macAddr, vlanId, intIfNum);

	  if (filterEntry != L7_NULLPTR && 
		  ((L7_INTF_ISMASKBITSET(filterEntry->srcIntfMask, intIfNum)) ||
		   (filterCfg.inUse == L7_TRUE &&
			L7_INTF_ISMASKBITSET(filterCfg.srcIntfMask, intIfNum))))
		rc = filterSrcIntfAdd(macAddr, vlanId, intIfNum);

      prevVlanId = vlanId;
      memcpy(prevMacAddr, macAddr, L7_MAC_ADDR_LEN);
      rc =filterNextGet(prevMacAddr, prevVlanId, macAddr, &vlanId);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default filter config data  
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    default is NO filters 
*
* @end
*********************************************************************/
void filterBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_FILTER_INTF_MAX_COUNT];

  memset((void *)&configId[0], 0, sizeof(nimConfigID_t) * L7_FILTER_INTF_MAX_COUNT);

  /* save config ids since interfaces will not be recreated after a restore */
  for (cfgIndex = 1; cfgIndex < L7_FILTER_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &filterCfgData->filterIntfCfgData[cfgIndex].configId);

  memset((void *)filterCfgData, 0, sizeof(filterCfgData_t)); 

  /* Initialize all interface-related entries in the config file to default values. */
  for (cfgIndex = 1; cfgIndex < L7_FILTER_INTF_MAX_COUNT; cfgIndex++)
    filterBuildDefaultIntfConfigData(&configId[cfgIndex], &filterCfgData->filterIntfCfgData[cfgIndex]);

  strcpy((char *)filterCfgData->cfgHdr.filename, FILTER_CFG_FILENAME);
  filterCfgData->cfgHdr.version = ver;
  filterCfgData->cfgHdr.componentID = L7_FILTER_COMPONENT_ID;
  filterCfgData->cfgHdr.type = L7_CFG_DATA;
  filterCfgData->cfgHdr.length = (L7_uint32)sizeof(filterCfgData_t);
  filterCfgData->cfgHdr.dataChanged = L7_FALSE;
  filterCfgData->cfgHdr.savePointId = 0;
  filterCfgData->cfgHdr.targetDevice = 0;
  memset((void *)filterCfgData->cfgHdr.pad, 0x00, sizeof(filterCfgData->cfgHdr.pad));

  filterCfgData->filteringEnabled = FD_STATIC_FILTERING_ENABLED;
  memset((void *)filterCfgData->filteringVlanCfgData,0,sizeof(filterCfgData->filteringVlanCfgData));
  /* populate the entry 0 for default vlan*/
  filterCfgData->filteringVlanCfgData[0].vlanId = L7_DOT1Q_DEFAULT_VLAN;
  filterCfgData->filteringVlanCfgData[0].mode = FD_VLAN_FILTERING_MODE;

  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
  {
    memset((void *)filterCfgData->filterData[i].macAddr, 0x00, L7_MAC_ADDR_LEN);
    filterCfgData->filterData[i].vlanId = L7_NULL;
    memset((void *)&filterCfgData->filterData[i].srcIntfMask, 0x00, 
                                  sizeof(filterCfgData->filterData[i].srcIntfMask));
    filterCfgData->filterData[i].numSrc = L7_NULL;
    memset((void *)&filterCfgData->filterData[i].dstIntfMask, 0x00, 
                                  sizeof(filterCfgData->filterData[i].dstIntfMask));
    filterCfgData->filterData[i].inUse = L7_FALSE;
  }
  
}

/*********************************************************************
* @purpose  Copies filter user config file from Info to the Cfg structure
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t filterDataCopy()
{
  L7_uint32 i;
  filterInfoData_t* filterEntry;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 intf;
  L7_uint32 cfgIndex;
  nimConfigID_t configId;
  L7_INTF_MASK_t tempMask;
  L7_RC_t rc;

  /* copy information about all configured filters into the filterCfgData */

  bzero((char *)filterCfgData->filterData, (L7_int32)sizeof(filterCfgData->filterData));
  
  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  rc = osapiSemaTake(filterAvlTree.semId, L7_WAIT_FOREVER);
  
  filterEntry = filterEntryFind(macAddr,1,L7_MATCH_GETNEXT);
  
  i = 0;
  while (filterEntry != L7_NULLPTR)
  {
    filterCfgData->filterData[i].inUse = L7_TRUE;

    /* extract the vlanId and Macaddress */
    rc = filterMacVlanIdExtract(filterEntry->vlanidmacAddr,macAddr,&vlanId);
    memcpy(filterCfgData->filterData[i].macAddr,macAddr,L7_MAC_ADDR_LEN);
    filterCfgData->filterData[i].vlanId = vlanId;

    /* store destination interfaces */
    filterCfgData->filterData[i].numSrc = filterEntry->numSrc;

    memcpy(&filterCfgData->filterData[i].dstIntfMask, 
           &filterEntry->dstIntfMask, 
           sizeof(filterCfgData->filterData[i].dstIntfMask));

    memcpy(&filterCfgData->filterData[i].srcIntfMask, 
           &filterEntry->srcIntfMask, 
           sizeof(filterCfgData->filterData[i].srcIntfMask));

    /* Copy the config Id for interfaces that are part of 
    either the src intf list or the dst Intf list*/
    memcpy(&tempMask, &filterEntry->dstIntfMask, sizeof(tempMask));
    L7_INTF_MASKOREQ(tempMask,filterEntry->srcIntfMask);

    L7_INTF_FHMASKBIT(tempMask, intf);
    while (intf != 0)
    {

      if (nimConfigIdGet(intf, &configId) != L7_SUCCESS)
      {
        rc = osapiSemaGive(filterAvlTree.semId);
        return L7_FAILURE;
      }

      cfgIndex = filterMapTbl[intf];
      NIM_CONFIG_ID_COPY(&filterCfgData->filterIntfCfgData[cfgIndex].configId, &configId);


      L7_INTF_CLRMASKBIT(tempMask,intf);
      L7_INTF_FHMASKBIT(tempMask, intf);
      
    }

    i++;
    filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_GETNEXT);
  }

  rc = osapiSemaGive(filterAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default filter intf config data  
*
* @parms    configId  the id to be placed into intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void filterBuildDefaultIntfConfigData(nimConfigID_t *configId, filterIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to filter interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterMapIntfConfigEntryGet(L7_uint32 intIfNum, filterIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(FILTER_IS_READY))
  {
    LOG_MSG("Error on call to filterMapIntfIsConfigEntryGet routine outside the EXECUTE state\n");
    return L7_FALSE;
  }

  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    for (i = 1; i < L7_FILTER_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&filterCfgData->filterIntfCfgData[i].configId, &configIdNull))
      {
        filterMapTbl[intIfNum] = i;
        *pCfg = &filterCfgData->filterIntfCfgData[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Get persistent configuration entry for a vlan
*
* @param    L7_ushort16   vlanId    @b((input))  VLAN identifier
* @param    filterVlanCfgData_t *vlanCfg  @b((output)) persistent cfg entry
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t filterVlanCfgEntryGet(L7_ushort16 vlanId, filterVlanCfgData_t **vlanCfg)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_MAX_VLANS; i++)
  {
    if (filterCfgData->filteringVlanCfgData[i].vlanId == vlanId)
    {
      *vlanCfg = &filterCfgData->filteringVlanCfgData[i];
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration 
*           data for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr to filter interface config structure
*                                  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL filterMapIntfIsConfigurable(L7_uint32 intIfNum, filterIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(FILTER_IS_READY))
  {
    LOG_MSG("Error on call to filterMapIntfIsConfigurable routine outside the EXECUTE state\n");
    return L7_FALSE;
  }

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = filterMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(filterCfgData->filterIntfCfgData[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between filterCfgData and filterMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      return L7_FALSE;
    }
  }

  *pCfg = &filterCfgData->filterIntfCfgData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Process the Callback for L7_CREATE
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  filterIntfCfgData_t *pCfg;
  L7_uint32 i;

  if (filterIntfCheck(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_FILTER_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&filterCfgData->filterIntfCfgData[i].configId, &configId))
    {
      filterMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (filterMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(filterMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id and build the default config */
    if (pCfg != L7_NULL)
      filterBuildDefaultIntfConfigData(&configId, pCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process the Callback for L7_DETACH
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfDetach(L7_uint32 intIfNum)
{
  L7_uint32 vlanId, prevVlanId ;
 /* filterIntfCfgData_t *pCfg;*/
  filterInfoData_t* filterEntry;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN], prevMacAddr[L7_MAC_ADDR_LEN];
  L7_RC_t rc = L7_SUCCESS;

  rc = filterFirstGet(macAddr, &vlanId);

  while (rc == L7_SUCCESS)
  {
      filterEntry = filterEntryFind(macAddr, vlanId, L7_MATCH_EXACT);

      if (filterEntry != L7_NULLPTR && L7_INTF_ISMASKBITSET(filterEntry->dstIntfMask, intIfNum))
        rc = filterDstIntfDelete(macAddr, vlanId, intIfNum);

	  if (filterEntry != L7_NULLPTR && L7_INTF_ISMASKBITSET(filterEntry->srcIntfMask, intIfNum))
		rc = filterSrcIntfDelete(macAddr, vlanId, intIfNum);

      prevVlanId = vlanId;
      memcpy(prevMacAddr, macAddr, L7_MAC_ADDR_LEN);
      rc = filterNextGet(prevMacAddr, prevVlanId, macAddr, &vlanId);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process the Callback for L7_DELETE
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfDelete(L7_uint32 intIfNum)
{
  L7_uint32 vlanId, prevVlanId ;
  filterInfoData_t* filterEntry;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN], prevMacAddr[L7_MAC_ADDR_LEN];
  filterIntfCfgData_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;

  if (filterMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
      rc = filterFirstGet(macAddr, &vlanId);

      while (rc == L7_SUCCESS)
      {
          filterEntry = filterEntryFind(macAddr, vlanId, L7_MATCH_EXACT);

          if (filterEntry != L7_NULLPTR && L7_INTF_ISMASKBITSET(filterEntry->dstIntfMask, intIfNum))
              rc = filterDstIntfDelete(macAddr, vlanId, intIfNum);

          if (filterEntry != L7_NULLPTR && L7_INTF_ISMASKBITSET(filterEntry->srcIntfMask, intIfNum))
              rc = filterSrcIntfDelete(macAddr, vlanId, intIfNum);

          prevVlanId = vlanId;
          memcpy(prevMacAddr, macAddr, L7_MAC_ADDR_LEN);
          rc = filterNextGet(prevMacAddr, prevVlanId, macAddr, &vlanId);
      }
      memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
      memset((void *)&filterMapTbl[intIfNum], 0, sizeof(L7_uint32));
      filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process NIM events.
*
* @param    intIfNum    internal interface number
* @param    event       event, defined by L7_PORT_EVENTS_t
* @param    correlator  event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;    

  memset(&status,  0x00,  sizeof(status));
  status.intIfNum   = intIfNum;
  status.component  = L7_FILTER_COMPONENT_ID;
  status.event      = event;
  status.correlator = correlator;

  if (filterDeregister.filterIntfChangeCallback == L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (filterIntfCheck(intIfNum) != L7_SUCCESS)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (FILTER_IS_READY)
  {
    switch (event)
    {
      case L7_CREATE:
        rc = filterIntfCreate(intIfNum);
        break;

      case L7_ATTACH:
        rc = filterApplyIntfConfigData(intIfNum);
        break;

      case L7_DETACH:
        rc = filterIntfDetach(intIfNum);
        break;

      case L7_DELETE:
        rc = filterIntfDelete(intIfNum);
        break;

      default:
        rc = L7_SUCCESS;
        break;
    }
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;
}

/*********************************************************************
*
* @purpose  Update the vlan config information we store about the new vlan
*           If the filtering is enabled apply the vlan config settings in the driver
*
* @param    L7_uint32  vlanId     @b((input)) VLAN ID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void filterVlanAdd(L7_uint32 vlanId)
{
  L7_uint32 i = 0;
  L7_uint32 index = 0;
  L7_uint32 type = 0;

  if ((vlanId == L7_DOT1Q_DEFAULT_VLAN) ||  /* default vlan always applied */
      (dot1qOperVlanTypeGet(vlanId, &type) != L7_SUCCESS) ||
      (type == L7_DOT1Q_DYNAMIC))
  {
    return;
  }
  
  for (i = 0; i < L7_MAX_VLANS; i++)
  {
    if (filterCfgData->filteringVlanCfgData[i].vlanId == vlanId)
    {
      if (filterCfgData->filteringEnabled)
      {
        /* apply configured vlan mode to the HW */
        if (dtlL2McastVlanFloodModeSet(vlanId, ptin_filter_vlan_mode /*filterCfgData->filteringVlanCfgData[i].mode*/) != L7_SUCCESS)
        {
          LOG_MSG("mfdbVlanAdd: failed to apply VLAN %u PFM configuration\n", vlanId);
        }
      } else
      {
        /* apply default vlan mode to the HW */
        if (dtlL2McastVlanFloodModeSet(vlanId, ptin_filter_vlan_mode /* PTin modified : FD_VLAN_FILTERING_MODE*/) != L7_SUCCESS)
        {
          LOG_MSG("mfdbVlanAdd: failed to apply VLAN %u PFM default\n", vlanId);
        }
      }
      return;
    }
    else if (filterCfgData->filteringVlanCfgData[i].vlanId == 0)
    {
      index = i;
      break;
    }
  }

  /* If a vlan configuration entry is not already
     assigned to the vlan, assign one */
  if (index == 0)    /* 0 is the default VLAN, didn't find entry */
  {
    return;
  }
  /* create default vlan configuration and apply */
  filterCfgData->filteringVlanCfgData[index].vlanId = vlanId;
  filterCfgData->filteringVlanCfgData[index].mode = ptin_filter_vlan_mode; // PTin modified: FD_VLAN_FILTERING_MODE;
  if (dtlL2McastVlanFloodModeSet(vlanId, ptin_filter_vlan_mode /* PTin modified : FD_VLAN_FILTERING_MODE*/) != L7_SUCCESS)
  {
    LOG_MSG("mfdbVlanAdd: failed to apply VLAN %u PFM default\n", vlanId);
  }
  return;
}


/*********************************************************************
* @purpose  To process Vlan change events.
*
* @param    vlanID      vlanId 
* @param    intIfNum     internal interface whose state has changed
* @param    event       event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t filterVlanChangeCallback(dot1qNotifyData_t *vlanData,L7_uint32 intIfNum,L7_uint32 event)
{
  L7_RC_t rc=L7_FAILURE,rc1,rc2;
  filterInfoData_t* filterEntry;
  L7_uint32 i ,j;
  
  L7_uint32 srcList[L7_FILTER_MAX_INTF];
  L7_uint32 numSrc;
  L7_uint32 dstList[L7_FILTER_MAX_INTF];
  L7_uint32 numDst;
  L7_uint32 x = 0, vlanId = 0, numVlans = 0;

  if (filterDeregister.filterVlanChangeCallback == L7_TRUE)
  {
	LOG_MSG("filterVlanChangeCallback is deregistered\n");
	return L7_FAILURE;
  }
  if (!(FILTER_IS_READY))
  {
	LOG_MSG("Rcvd a vlan callback in incorrect state %d vlan %d event %d int %d\n",
			filterCnfgrState, vlanId, event, intIfNum);
    return L7_FAILURE;
  }
  
  for (x = 1; x<=L7_VLAN_MAX_MASK_BIT; x++) 
  {
      if (vlanData->numVlans == 1) 
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          x = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,x)) 
          {
              vlanId = x;
          }
          else
          {
              if (numVlans == vlanData->numVlans) 
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }

      switch (event)
      {
      case VLAN_ADD_NOTIFY:
         /* Vlan has been added. check to see if there are any configured multicast filter entries 
            for this vlanID and apply the configuration to the operational structure*/
          /* iUpdate the vlan config structure and 
         if neccessary apply the vlan filtering mode to the driver
      */
      filterVlanAdd(vlanId);

          rc = L7_SUCCESS;
          for (i=0;i<L7_MAX_FDB_STATIC_FILTER_ENTRIES;i++)
          {
            if(filterCfgData->filterData[i].inUse == L7_TRUE)
            {
              if (filterCfgData->filterData[i].vlanId == vlanId)
              {
                /* check to see if the filter entry already present */
                  filterEntry = filterEntryFind(filterCfgData->filterData[i].macAddr, 
                                                filterCfgData->filterData[i].vlanId,L7_MATCH_EXACT);
                  if (filterEntry ==L7_NULLPTR)
                  {
                    /*filter not present. Create filter in the operational data structure*/
                    rc = filterCreate(filterCfgData->filterData[i].macAddr, 
                                      filterCfgData->filterData[i].vlanId);
                    if (rc==L7_SUCCESS)
                    {
                        filterEntry = filterEntryFind(filterCfgData->filterData[i].macAddr, 
                                                      filterCfgData->filterData[i].vlanId,L7_MATCH_EXACT);  
                        if (filterEntry != L7_NULLPTR)
                        {
                          /* copy interface configuration to operational data structure */
                          rc1 = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
                          filterEntry->numSrc = filterCfgData->filterData[i].numSrc;
                           memcpy(&filterEntry->srcIntfMask, &filterCfgData->filterData[i].srcIntfMask,
                             sizeof(filterEntry->srcIntfMask));
                      memcpy(&filterEntry->dstIntfMask, &filterCfgData->filterData[i].dstIntfMask,
                             sizeof(filterEntry->dstIntfMask));

                          rc1 = osapiSemaGive(filterAvlTree.semId);
    
                          /*apply the copied interface configuration */
                          /* for source interfaces*/
                          rc2=  filterSrcIntfListGet(filterCfgData->filterData[i].macAddr,
                                                     filterCfgData->filterData[i].vlanId,
                                                     &numSrc,
                                                     srcList);
                          if (rc2==L7_SUCCESS)
                          {
                            for(j=0;j<numSrc;j++)
                            {
                              filterSrcIntfAdd(filterCfgData->filterData[i].macAddr,
                                               filterCfgData->filterData[i].vlanId, srcList[j]);
                            }
                          }
    
                          /*for destination interfaces */
                          rc2=  filterDstIntfListGet(filterCfgData->filterData[i].macAddr,
                                                     filterCfgData->filterData[i].vlanId,
                                                     &numDst,
                                                     dstList);
                          if (rc2==L7_SUCCESS)
                          {
                            for(j=0;j<numDst;j++)
                            {
                              filterDstIntfAdd(filterCfgData->filterData[i].macAddr,
                                               filterCfgData->filterData[i].vlanId, dstList[j]);
                            }
                          }
    
                        }
                    
                    }
                    else
                    {
                      LOG_MSG("filterVlanCallback:Could not create filter for macaddr %02x:%02x:%02x:%02x:%02x:%02x , vlan %d",
                                filterCfgData->filterData[i].macAddr[0],
                                filterCfgData->filterData[i].macAddr[1],
                                filterCfgData->filterData[i].macAddr[2],
                                filterCfgData->filterData[i].macAddr[3],
                                filterCfgData->filterData[i].macAddr[4],
                                filterCfgData->filterData[i].macAddr[5],
                                filterCfgData->filterData[i].vlanId);
    
                    }
    
                  }
                }
                else
                {
                  rc=L7_SUCCESS;
                }
               
             }/* if cfgdata inuse*/
            else
                {
                  rc=L7_SUCCESS;
                }
           }/*for*/
    
            
          break;
      case  VLAN_DELETE_PENDING_NOTIFY:
      case VLAN_DELETE_NOTIFY:
      case VLAN_ADD_PORT_NOTIFY:
      case VLAN_DELETE_PORT_NOTIFY:
      case VLAN_START_TAGGING_PORT_NOTIFY:
      case VLAN_STOP_TAGGING_PORT_NOTIFY:
      case VLAN_INITIALIZED_NOTIFY:
      case VLAN_RESTORE_NOTIFY:
           rc= L7_SUCCESS;
           break;
      default:
        rc=L7_SUCCESS;
        break;
      }
      numVlans++;
  }
  return rc;
}


/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void filterBuildTestConfigData(void)
{
    L7_uint32 i, j ;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  
    sysapiPrintf("Not building filterIntfCfgData because it is extraneous\n");

    for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
    {
      for (j = 0; j < (L7_MAC_ADDR_LEN-1); j++) 
      {
         filterCfgData->filterData[i].macAddr[j] = 0xaa;
      }

      filterCfgData->filterData[i].macAddr[(L7_MAC_ADDR_LEN-1)] = i;
      filterCfgData->filterData[i].vlanId = i ;

      /* Arbitrarily vary the values of the entries */
#if 0
      for (k = 0; k < 4 ; k++) 
      {
         filterCfgData->filterData[i].srcifIndex[k] = k + j;
      }

      filterCfgData->filterData[i].numSrc = 4;

      /* Arbitrarily vary the values of the entries */

      for (k = 0; k < 7 ; k++) 
      {
         filterCfgData->filterData[i].dstifIndex[k] = k + j + 3;
      }
#endif
      filterCfgData->filterData[i].inUse = L7_TRUE;
    }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   filterCfgData->cfgHdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


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
void filterConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32 i, j, k, index;
    L7_char8 buf[32];
    L7_uchar8    *byteEntry;
    L7_INTF_MASK_t tempMask;


    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(filterCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/
  
   sysapiPrintf( "\n");
   for (i=0; i< L7_MAX_FDB_STATIC_FILTER_ENTRIES; i++)
   {

       sysapiPrintf( "\n");
       if (filterCfgData->filterData[i].inUse == L7_TRUE) 
       {
           sysapiPrintf( "Filter Entry  %d:", i);

           sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
                   filterCfgData->filterData[i].macAddr[0],
                   filterCfgData->filterData[i].macAddr[1],
                   filterCfgData->filterData[i].macAddr[2],
                   filterCfgData->filterData[i].macAddr[3],
                   filterCfgData->filterData[i].macAddr[4],
                   filterCfgData->filterData[i].macAddr[5]);

           sysapiPrintf ("    macAddr - %s",buf);
           sysapiPrintf ("    vlanId - %d",  filterCfgData->filterData[i].vlanId);
           sysapiPrintf( "\n");

           /*  Source Interfaces */
           sysapiPrintf ("numSrc = %d",  filterCfgData->filterData[i].numSrc);
           sysapiPrintf( "\n");
           sysapiPrintf( "Source List: ");
           sysapiPrintf( "\n");

           memcpy(&tempMask, &filterCfgData->filterData[i].srcIntfMask, sizeof(tempMask));
           L7_INTF_FHMASKBIT(tempMask, index);
           while (index != 0)
           {
             sysapiPrintf ("  %d, ",  index);

             L7_INTF_CLRMASKBIT(tempMask,index);
         	/* Get the next valid interface */
             L7_INTF_FHMASKBIT(tempMask, index);
           }

           /*  Dest Interfaces */
           sysapiPrintf( "\n");
           sysapiPrintf( "Dest List: ");
           sysapiPrintf( "\n");

           memcpy(&tempMask, &filterCfgData->filterData[i].dstIntfMask, sizeof(tempMask));
           L7_INTF_FHMASKBIT(tempMask, index);
           while (index != 0)
           {
             sysapiPrintf ("  %d, ",  index);

             L7_INTF_CLRMASKBIT(tempMask,index);
         	/* Get the next valid interface */
             L7_INTF_FHMASKBIT(tempMask, index);
           }
           

       }  /* inUse == L7_TRUE */
   }  /* L7_MAX_FDB_STATIC_FILTER_ENTRIES */


   /* Raw Dump nimConfigID table just in case there is a change in data */
   
   for (i=0; i< L7_FILTER_INTF_MAX_COUNT; i++)
   {
       
       sysapiPrintf( "Raw dump of filterIntfCfgData Entry %d\n", i);
       sysapiPrintf( "\n");
       byteEntry = (L7_uchar8 *)&filterCfgData->filterIntfCfgData[i].configId;
       k = sizeof(nimConfigID_t);
       for (j = 0; j < k; j++)
       {

           sysapiPrintf("%02X:",*byteEntry);
           byteEntry++;
       }
   } /* L7_FILTER_INTF_MAX_COUNT */


   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_MAX_FDB_STATIC_FILTER_ENTRIES - %d\n", L7_MAX_FDB_STATIC_FILTER_ENTRIES);
    sysapiPrintf( "L7_FILTER_INTF_MAX_COUNT - %d\n", L7_FILTER_INTF_MAX_COUNT);
    sysapiPrintf( "L7_FILTER_MAX_INTF - %d\n", L7_FILTER_MAX_INTF);
    sysapiPrintf( "L7_MAC_ADDR_LEN - %d\n", L7_MAC_ADDR_LEN);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("filterCfgData->checkSum : %u\n", filterCfgData->checkSum);


}


/*********************************************************************
* @purpose  Saves filter user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
void filterSaveTestConfigData(void)
{

  filterCfgData->cfgHdr.dataChanged = L7_FALSE;
  filterCfgData->checkSum = nvStoreCrc32((L7_uchar8 *)filterCfgData, 
                                         (L7_uint32)(sizeof(filterCfgData_t) - sizeof(filterCfgData->checkSum)));

  if (sysapiCfgFileWrite(L7_FILTER_COMPONENT_ID, FILTER_CFG_FILENAME, 
                         (L7_char8 *)filterCfgData, 
                         ((L7_uint32)sizeof(filterCfgData_t))) != L7_SUCCESS)
    LOG_MSG("filterSave: Error on call to sysapiCfgFileWrite routine on file %s\n", FILTER_CFG_FILENAME);

}


/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




