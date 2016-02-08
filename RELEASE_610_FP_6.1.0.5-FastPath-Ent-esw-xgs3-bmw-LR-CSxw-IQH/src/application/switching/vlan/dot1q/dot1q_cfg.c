/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_cfg.c
*
* @purpose 802.1Q Main File
*
* @component dot1Q
*
* @comments none
*
* @create 09/15/2000
*
* @author w. jacobs
*
* @end
*             
**********************************************************************/

#include "commdefs.h"
#define DOT1D_PRIORITY_TABLE FACTORY_DEFAULT_DEFINE
#include <string.h>
#include "flex.h"
#include "dot1q_common.h"
#include "l7_product.h"
#include "osapi.h"
#include "nimapi.h"
#include "log.h"
#include "trapapi.h"
#include "dtlapi.h"
#include "dot1q_intf.h"
#include "dot1q_control.h"
#include "dot1q_data.h"
#include "dot1q.h"
#include "dot1q_cnfgr.h"
#include "dot1q_cfg.h"
#include "dot1q_outcalls.h"
#include "dot1q_debug.h"
#include "dot1q_qports.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"


dot1qCfgData_t    *dot1qCfg = L7_NULLPTR;

/* Externs */
extern dot1qCfgData_t    *dot1q1st;
extern dot1qIntfData_t   *dot1qQports;
extern dot1qInfo_t       *dot1qInfo;
extern L7_uint32         *dot1qMapTbl;
extern L7_uint32         *dot1qVlanCfgMapTbl;
extern vlanTree_t        *pVlanTree;
extern  L7_VLAN_MASK_t 		dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];
extern  osapiRWLock_t     dot1qCfgRWLock;


static void dot1qCfgShow(void);
L7_RC_t dot1qCfgCleanOperData(L7_uint32 intIfNum);

static nimConfigID_t defCfgConfigId[DOT1Q_INTF_MAX_COUNT];

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
L7_BOOL dot1qHasDataChanged(void)
{
  return dot1qCfg->hdr.dataChanged;
}
void dot1qResetDataChanged(void)
{
  dot1qCfg->hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Saves dot1q VLAN user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    dot1qCfg is the config file overlay 
*       
* @end
*********************************************************************/
L7_RC_t   dot1qSave(void)
{

  if (dot1qCfg->hdr.dataChanged == L7_FALSE)
    return L7_SUCCESS;

  dot1qCfg->hdr.dataChanged = L7_FALSE;
  
  
  osapiSemaGive(dot1qSem); 
  dot1qCfg->checkSum = nvStoreCrc32((L7_uchar8*)dot1qCfg,
                                    (L7_uint32)(sizeof(dot1qCfgData_t) - sizeof(dot1qCfg->checkSum) ));

  if ( (sysapiCfgFileWrite(L7_DOT1Q_COMPONENT_ID, DOT1Q_CFG_FILENAME, 
                           (L7_char8*)dot1qCfg, (L7_int32)sizeof(dot1qCfgData_t))) != L7_SUCCESS)
  {
    LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n",DOT1Q_CFG_FILENAME);
  }
  
  return L7_SUCCESS;
}
        
/*********************************************************************
* @purpose  Build default dot1q config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Builds Garp, VLAN and port default data which will be
*           applied in dot1qApplyConfigData
*           Also inits the config file header 
*
* @end
*********************************************************************/
void dot1qBuildDefaultConfigData(L7_uint32 ver)
{
  dot1qVlanCfgData_t *vCfg;
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 i, intIfNum, maskOffset;
  L7_RC_t rc;

  /* save config ids since the interfaces will not be recreated after a restore */
  for (i=0; i<DOT1Q_INTF_MAX_COUNT; i++)
  {
    NIM_CONFIG_ID_COPY(&defCfgConfigId[i], &dot1qCfg->Qports[i].configId);
  }
  
  memset( (void*)dot1qCfg, 0, sizeof(dot1qCfgData_t) );

  for (i=0;i<DOT1Q_INTF_MAX_COUNT;i++)
  {
    NIM_CONFIG_ID_COPY(&dot1qCfg->Qports[i].configId, &defCfgConfigId[i]);
  }
  
  /* build header */
  strcpy((char*)dot1qCfg->hdr.filename, DOT1Q_CFG_FILENAME);
  dot1qCfg->hdr.version = ver;
  dot1qCfg->hdr.componentID = L7_DOT1Q_COMPONENT_ID;
  dot1qCfg->hdr.type = L7_CFG_DATA;
  dot1qCfg->hdr.length = (L7_uint32)sizeof(dot1qCfgData_t);
  dot1qCfg->hdr.dataChanged = L7_FALSE;

  /* Initialize the default VLAN */

  vCfg = &dot1qCfg->Qvlan[0];
   
  vCfg->vlanId = FD_DOT1Q_DEFAULT_VLAN;
  strcpy( (char*)&vCfg->vlanCfg.name, FD_DOT1Q_DEFAULT_VLAN_NAME);

  /* NOTE:  The current design point assumes the number of traffic classes
   *        must never be set less than the init value, since the init value
   *        is always used to generate the default dot1p mapping table.
   *        Any change to this design point will require adjustments to
   *        be made not only in dot1p, but in QOS as well.
   */
  #if (L7_DOT1P_NUM_TRAFFIC_CLASSES < L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT)
  #error L7_DOT1P_NUM_TRAFFIC_CLASSES in platform.h must be >= L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT
  #endif

  /* build dot1p global priority defaults */
  dot1qCfg->Qglobal.priority.NumTrafficClasses   = FD_DOT1P_NUM_TRAFFIC_CLASSES;
  dot1qCfg->Qglobal.priority.DefaultUserPriority = FD_DOT1P_DEFAULT_USER_PRIORITY;
  memcpy(dot1qCfg->Qglobal.priority.Mapping,
         dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1],
         sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1]));

  dot1qCfg->Qglobal.baseInternalVlan = FD_DOT1Q_BASE_INTERNAL_VLAN;
  dot1qCfg->Qglobal.internalVlanPolicy = FD_DOT1Q_INTERNAL_VLAN_POLICY;

  /* build defaults for already existing interfaces
     newly created interfaces' defaults will be created by the l7_create */

  if (dot1qCnfgrStateCheck() == L7_TRUE)
  {
    rc = nimFirstValidIntfNumber(&intIfNum);
    while (rc == L7_SUCCESS)
    {
      if ((dot1qMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
      {
        /* Configure the port in the default VLAN */ 
        nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);
        dot1qIntfBuildDefaultVlanConfigData(maskOffset, vCfg);

        /* Build the interface config */
        dot1qIntfBuildDefaultConfigData(pCfg);
      }
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }
  }
  return;
}


/*********************************************************************
* @purpose  Build default config data for an intf 
*
* @param    pCfg  pointer to the interface cfg data structure
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void dot1qIntfBuildDefaultConfigData(dot1qIntfCfgData_t *pCfg)
{
  L7_uint32 pvid = FD_DOT1Q_DEFAULT_PVID;
  L7_uint32 accessVlan = FD_DOT1Q_DEFAULT_ACCESS_VLAN;
  L7_BOOL ingressFilter = FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT;
  L7_uint32 acceptFrameType = FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE;

    pCfg->portCfg.switchport_mode         = FD_DOT1Q_DEFAULT_SWPORT_MODE;
  switch (pCfg->portCfg.switchport_mode)
  {
    case DOT1Q_SWPORT_MODE_ACCESS:
      pvid = accessVlan;
      acceptFrameType = L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED;
      /* Fall thru */
    case DOT1Q_SWPORT_MODE_TRUNK:
      ingressFilter = L7_ENABLE;
      /* check for mode due to fall thru*/
      if (pCfg->portCfg.switchport_mode == DOT1Q_SWPORT_MODE_TRUNK)
      {
        acceptFrameType = L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED;
      }
      /* Fall thru */
    default:
      pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID] = pvid;
      pCfg->portCfg.acceptFrameType         = acceptFrameType;
      pCfg->portCfg.enableIngressFiltering  = ingressFilter;
      pCfg->portCfg.access_vlan             = accessVlan;
      break;
  }
    /* copy from dot1p global values to establish interface defaults */
    memcpy(&pCfg->portCfg.priority, &dot1qCfg->Qglobal.priority, sizeof(dot1p_prio_t));

}



/*********************************************************************
* @purpose  Build default VLAN config data for an intf 
*
* @param    intIfNum    internal interface number
* @param    vCfg        pointer to the vlan cfg data structure
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void dot1qIntfBuildDefaultVlanConfigData(L7_uint32 intIfNum, dot1qVlanCfgData_t *vCfg)
{
    NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts, intIfNum);
    NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet,intIfNum);
}

/*********************************************************************
* @purpose  Apply DOT1Q Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t dot1qApplyConfigData(void)
{
  L7_uint32 cfgIndex, intIfNum;
  dot1qVlanCfgData_t *vCfg;
  nimConfigID_t configIdNull;
  L7_uint32 idx;
 
  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  
  /* apply global data */
  for (idx=0;idx<L7_MAX_VLANS;idx++)
  {
    vCfg = &dot1qCfg->Qvlan[idx];

    
    if (vCfg->vlanId != L7_NULL)
    { 

        /* Create the VLAN and its VLAN cfg mapping */
        dot1qVlanCfgMapTbl[vCfg->vlanId] = idx;

        dot1qCurrentVlanCountUpdate(L7_TRUE);
        dot1qVlanCreateProcess(DOT1Q_DEFAULT_FDB_ID , vCfg->vlanId, DOT1Q_ADMIN);
            

      /* The remainder of the VLAN  cfg is interface-related.  Thus the configuration
         will be applied when the interface is attached */
    }
  }

  (void)dot1dPortDefaultUserPriorityGlobalProcess(dot1qCfg->Qglobal.priority.DefaultUserPriority);
  
  if (dot1qCnfgrState == DOT1Q_PHASE_EXECUTE)
  {
    /* apply configuration for all configured interfaces, used for clear,restore only */
    for (cfgIndex = 0; cfgIndex < DOT1Q_INTF_MAX_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&dot1qCfg->Qports[cfgIndex].configId, &configIdNull))
         continue;
      if (nimIntIfFromConfigIDGet(&(dot1qCfg->Qports[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
       continue;
      dot1qApplyIntfConfigData(intIfNum);
    }
  }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply configuration (or defaults) for an interface
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invoked for an L7_ATTACH or during an apply config
*
* @end
*
*********************************************************************/
L7_RC_t dot1qApplyIntfConfigData(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_uint32 idx;
  dot1qIntfCfgData_t *pCfg;
  dot1qVlanCfgData_t *vCfg;
  L7_uint32 maskOffset;
  L7_VLAN_MASK_t memberVlans;
  L7_VLAN_MASK_t taggedVlans;
  NIM_INTF_MASK_t tempMask;

  DOT1Q_TRACE("%s(%d) intf %d \n", __FUNCTION__, __LINE__, intIfNum);

  memset(&memberVlans, 0x0, sizeof(L7_VLAN_MASK_t));
  memset(&taggedVlans, 0x0, sizeof(L7_VLAN_MASK_t));
  if ((dot1qMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
  {
    
    nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

    /* If this port is access port a lot of processing can be saved*/
    if (pCfg->portCfg.switchport_mode == DOT1Q_SWPORT_MODE_ACCESS)
    {
        dot1qSwitchPortAccessModeSet(intIfNum,pCfg->portCfg.access_vlan,
                                      DOT1Q_ADMIN);
    }
    else
    {
        memset(&tempMask, 0, sizeof(tempMask));
        NIM_INTF_SETMASKBIT(tempMask, intIfNum);
        dot1qSwitchPortModeProcess(&tempMask, pCfg->portCfg.switchport_mode, L7_FALSE);
        /* apply membership */
        if (pCfg->configId.type != L7_CAPWAP_TUNNEL_INTF)
        {
        for (idx=0;idx<L7_MAX_VLANS;idx++)
        {
          vCfg = &dot1qCfg->Qvlan[idx];
    
          if (vCfg->vlanId != L7_NULL)
          {
            /* Check for pre-existence of the static VLAN */
            if (dot1qVlanCheckValid(vCfg->vlanId) != L7_SUCCESS)
            {
                /* Create the VLAN and its VLAN cfg mapping 
                   Increment the currentVlanCount first as the 
                   createProcess will take corrective action if create fails
                */
                dot1qInfo->CurrentVLANCount++;
                if (dot1qVlanCreateProcess(DOT1Q_DEFAULT_FDB_ID , vCfg->vlanId, DOT1Q_ADMIN) 
                    == L7_SUCCESS)
                {
                    dot1qVlanCfgMapTbl[vCfg->vlanId] = idx;
                }
            }
    
            /* update membership */
            if (NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.staticEgressPorts, maskOffset))
            {
    
              L7_VLAN_SETMASKBIT(memberVlans,vCfg->vlanId);
              if (NIM_INTF_ISMASKBITSET(vCfg->vlanCfg.taggedSet, maskOffset))
                      L7_VLAN_SETMASKBIT(taggedVlans,vCfg->vlanId);
            }
    
            
          } /* if vlanCfg != NULL*/
    
        } /* for idx */
        }
    
        dot1qIntfVlanListPopulate(intIfNum,&memberVlans, &taggedVlans);
    
            
    } /* else access port*/

    /* pre-init the operational number of traffic classes so that this 'set'
     * function picks up the dot1p mappings from the config instead of 
     * regenerating defaults
     */
    dot1qQports[intIfNum].priority.NumTrafficClasses = pCfg->portCfg.priority.NumTrafficClasses;
    dot1dPortNumTrafficClassesProcess(intIfNum, pCfg->portCfg.priority.NumTrafficClasses);
    rc = dot1dPortDefaultUserPriorityProcess(intIfNum, pCfg->portCfg.priority.DefaultUserPriority);

    /* All ports are capable of setting tagging (0x80), acceptable frame
     * types (0x40), and ingress filtering (0x20). 
     */
    dot1qQports[intIfNum].portCapabilities = 0xe0;

  } /* if intf is configurable*/
  else
  {
      rc = L7_FAILURE;
  }
  
  return rc;
}

/*********************************************************************
* @purpose  For a given interface, apply current configuration to hardware
*
* @param    intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invoked for an L7_ATTACH or during an apply config
*           This Function will apply the config from vlan->admin to
*           vlan->current (and the driver).
* @end
*
*********************************************************************/
L7_RC_t dot1qApplyIntfConfigToHW(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 maskOffset, msNum, tsNum;
  L7_uint32 pvid, aftype, status;
  dot1q_vlan_dataEntry_t *pDataEntry;
  L7_ushort16 tempIntfNum = (L7_ushort16)intIfNum;
  dot1qNotifyData_t vlanData;
  
  rc = L7_SUCCESS;

  if ((dot1qMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
  {
    nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);
    
    
    /* only apply configuration for attached interfaces */
    if (dot1qIntfIsSettable(intIfNum) == L7_TRUE)
    {
      /* apply membership */
      osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
      pDataEntry = (dot1q_vlan_dataEntry_t*)vlanFirstDataEntry(pVlanTree);
      while (pDataEntry != L7_NULLPTR)
      {
        msNum = 0;
        tsNum = 0;
          
        /* update membership of admin configured members. Dynamic membership will be learnt
         * as and when join-in packets come */

        if (pDataEntry->current.status == L7_DOT1Q_DYNAMIC ||
            pDataEntry->current.status == L7_DOT1Q_WS_DYNAMIC)
        {
            pDataEntry = (dot1q_vlan_dataEntry_t*)vlanNextDataEntry(pVlanTree, pDataEntry->vlanId);
            continue;
        }

        if (pDataEntry->admin != L7_NULL) 
        {
            if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.staticEgressPorts, maskOffset))
            {
                msNum = 1;
                if (!NIM_INTF_ISMASKBITSET(pDataEntry->current.currentEgressPorts, maskOffset))
                {
                    NIM_INTF_SETMASKBIT(pDataEntry->current.currentEgressPorts,maskOffset);
                    L7_VLAN_SETMASKBIT(dot1qIntfVidList[intIfNum],pDataEntry->vlanId);

                }


            }

            if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.taggedSet, maskOffset))
              tsNum = 1;
        } /* pDataEntry->admin != L7_NULL */

          
		if (msNum != 0 || tsNum != 0)
		{
            if (dtlDot1qAddPort(0, (L7_ushort16)pDataEntry->vlanId, &tempIntfNum, msNum, &tempIntfNum, tsNum, L7_NULLPTR, 0) == L7_SUCCESS)
            {
                vlanData.numVlans = 1;
                vlanData.data.vlanId = pDataEntry->vlanId;
                (void)dot1qVlanIntfVlanChange(&vlanData,intIfNum,VLAN_ADD_PORT_NOTIFY);
    		}
            else
            {
                LOG_MSG("DTL call failure adding intIfNum %d to vlan %d\n", intIfNum, pDataEntry->vlanId);
            }
		}

        pDataEntry = (dot1q_vlan_dataEntry_t*)vlanNextDataEntry(pVlanTree, pDataEntry->vlanId);
      }
      osapiSemaGive(dot1qSem); 
    
      dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &pvid);
      (void)dot1qQportsVIDProcess(intIfNum, pvid, L7_DOT1Q_DEFAULT_VID_INDEX,DOT1Q_ADMIN);
    
      dot1qQportsAcceptFrameTypeGet(intIfNum, &aftype);
      (void)dot1qQportsAcceptFrameTypeProcess(intIfNum, aftype);
    
      dot1qQportsIngressFilterGet(intIfNum, &status);
      (void)dot1qQportsIngressFilterProcess(intIfNum, status);
    
      status = dot1dPortNumTrafficClassesGet(intIfNum);
      (void)dot1dPortNumTrafficClassesProcess(intIfNum, status);

      status = dot1dPortDefaultUserPriorityGet(intIfNum);
      (void)dot1dPortDefaultUserPriorityProcess(intIfNum, status);
    
    }
  }
  else
      rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
* @purpose  Deconfigure an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH. This Function will delete all the config 
*           from the pvlan->current and the driver. But will maintain as is 
*           the config in pvlan->admin so that apply can restore whenever 
*           relevant event happens.
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfUnapplyConfigData(L7_uint32 intIfNum)
{
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 maskOffset;
  dot1q_vlan_dataEntry_t *pDataEntry;
  L7_RC_t rc;
  L7_NIM_QUERY_DATA_t queryData;
  L7_uint32 portCfgFlag;
  dot1qNotifyData_t vlanData;
          
  if ((dot1qMapIntfIsConfigurable(intIfNum, &pCfg)) != L7_TRUE)
      return L7_SUCCESS;
  
  nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

  /* only remove configuration for attached interfaces */
  queryData.intIfNum = intIfNum;
  queryData.request = L7_NIM_QRY_RQST_STATE;
  rc = nimIntfQuery(&queryData);
  if ((rc == L7_SUCCESS) && ((queryData.data.state == L7_INTF_ATTACHED) ||
                             (queryData.data.state == L7_INTF_DETACHING)))
  {

    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);


    if (dtlDot1qUnConfigInterface(intIfNum,(L7_uchar8 *)&dot1qIntfVidList[intIfNum].value)
        != L7_SUCCESS)
    {
        LOG_MSG("dtl request failure when deleting intIfNum %d \n", 
                intIfNum );
    }
    /* Send the delete port notification regardless of whether DTL succeeded.  
       The rest of the system should act conservatively and no longer act as 
      if the port is in the VLAN */

    memset(&dot1qIntfVidList[intIfNum], 0x0, sizeof(L7_VLAN_MASK_t));
    
	pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
    while (pDataEntry != L7_NULL)
    {
       NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts, intIfNum);
       NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet, intIfNum);
	
      if (pDataEntry->current.status != L7_DOT1Q_DYNAMIC &&
          pDataEntry->current.status != L7_DOT1Q_WS_DYNAMIC)
      {
          if (pDataEntry->admin != L7_NULL) 
          {
              if (NIM_INTF_ISMASKBITSET(pDataEntry->admin->vlanCfg.staticEgressPorts, maskOffset))
              {
                NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts,maskOffset);

                vlanData.numVlans = 1;
                vlanData.data.vlanId = pDataEntry->vlanId;
                (void)dot1qVlanIntfVlanChange(&vlanData,intIfNum,VLAN_DELETE_PORT_NOTIFY);

              }
          } /* pDataEntry->admin != L7_NULL */

      }


      pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry->vlanId);
    }
    osapiSemaGive(dot1qSem); 

    portCfgFlag = 0;
    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_VID_SET;
    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_ACCEPT_FRAME_TYPE;
    portCfgFlag |=  DTL_L2_DOT1Q_PORT_CONFIG_INGRESS_FILTER;

    /*Change the oper structure*/
    dot1qQports[intIfNum].ingressFilter = FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT;
    dot1qQports[intIfNum].pvid[DOT1Q_DEFAULT_PVID] = FD_DOT1Q_DEFAULT_PVID;
    dot1qQports[intIfNum].switchport_mode = DOT1Q_SWPORT_MODE_NONE;

    rc = dtlDot1qPortConfig(intIfNum, portCfgFlag,
                            FD_DOT1Q_DEFAULT_PVID,
                            FD_DOT1Q_DEFAULT_ACCEPT_FRAME_TYPE_ADMITALL, 
                            FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT);
    if (rc != L7_SUCCESS) 
    {
       LOG_MSG("dtl failure reverting to default state for intIfNum %d\n",intIfNum);
    }

  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dot1q interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL dot1qMapIntfIsConfigurable(L7_uint32 intIfNum, dot1qIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (dot1qCnfgrStateCheck() != L7_TRUE)
    return L7_FALSE;

  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = dot1qMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;
  
  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   * the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(dot1qCfg->Qports[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between snoopCfgData and snoopMapTbl or
       * there is synchronization issue between NIM and components w.r.t. interface creation/deletion
       */
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1Q_COMPONENT_ID,
                "Error accessing DOT1Q config data for interface %d in dot1qMapIntfIsConfigurable."
                " A default configuration does not exist for this interface. Typically a case when a"
                " new interface is created and has no pre-configuration.", intIfNum);
        return L7_FALSE;
    }
  }
  
  *pCfg = &dot1qCfg->Qports[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL dot1qMapIntfConfigEntryGet(L7_uint32 intIfNum, dot1qIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (dot1qCnfgrStateCheck() != L7_TRUE)
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&dot1qCfg->Qports[i].configId, &configIdNull))
      {
        dot1qMapTbl[intIfNum] = i;
        *pCfg = &dot1qCfg->Qports[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Allocates intf config storage if a config does not exist
*
* @end
*
*********************************************************************/
L7_RC_t dot1qIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  dot1qIntfCfgData_t *pCfg;
  dot1qVlanCfgData_t *vCfg;
  L7_uint32 maskOffset, i, idx;


  if (dot1qIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < DOT1Q_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dot1qCfg->Qports[i].configId, &configId))
    {
      dot1qMapTbl[intIfNum] = i;
      break;
    }
  }
  
  pCfg = L7_NULL;

  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    /* config does not exist, build defaults */
    if(dot1qMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* port defaults */
    
    dot1qIntfBuildDefaultConfigData(pCfg);

    if (pCfg->configId.type != L7_CAPWAP_TUNNEL_INTF)
    {
    /* TBD vlan defaults */
    for(idx=0;idx<L7_MAX_VLANS;idx++)
    {
      vCfg = &dot1qCfg->Qvlan[idx];
      if (vCfg->vlanId == L7_DOT1Q_DEFAULT_VLAN)
      {
        nimConfigIdMaskOffsetGet(intIfNum,&maskOffset);

        dot1qIntfBuildDefaultVlanConfigData(maskOffset, vCfg);

        break;
        }
      }
    }    

  }   /* dot1qMapIntfIsConfigurable */
  
  if (pCfg != L7_NULL)
  {
    NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1qIntfDelete(L7_uint32 intIfNum)
{
  dot1qIntfCfgData_t *pCfg;
  dot1qVlanCfgData_t *vCfg;
  L7_uint32 idx;
/*  L7_uint32 val, nextvid;
  L7_RC_t rc;
  */

  if (dot1qMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    dot1qMapTbl[intIfNum] = 0;
    
    /* clear out dot1p operational config data for this interface */
    memset(&dot1qQports[intIfNum].priority, 0, sizeof(dot1p_prio_t));

    /* clear the acquiredList on an L7_DELETE */
    memset(&(dot1qQports[intIfNum].acquiredList), 0, sizeof(AcquiredMask));


        /* apply global data */
    for (idx=0;idx<L7_MAX_VLANS;idx++)
    {
      vCfg = &dot1qCfg->Qvlan[idx];

      if (vCfg != L7_NULL && vCfg->vlanId != L7_NULL)
      { 
          /* Check for pre-existence of the static VLAN */
          if (dot1qVlanCheckValid(vCfg->vlanId) == L7_SUCCESS)
          {
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts, intIfNum);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts, intIfNum);
              NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, intIfNum);
          }
      }
    }


    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Initializes the dot1p priority to traffic class mapping table
*
* @param    intf               interface number
* @param    numTrafficClasses  number of traffic classes
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    An intIfNum of L7_ALL_INTERFACES denotes global config.
*       
* @end
*********************************************************************/
L7_RC_t dot1qQportNumTrafficClassesUpdate(L7_uint32 intf, L7_uint32 numTrafficClasses)
{
  dot1p_prio_t *pPriority;

  if ((numTrafficClasses > L7_DOT1P_NUM_TRAFFIC_CLASSES) || 
      (numTrafficClasses < L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT))
  {
    return(L7_FAILURE);
  }


  if (intf == L7_ALL_INTERFACES)
    pPriority = &dot1qCfg->Qglobal.priority;
  else
    pPriority = &dot1qQports[intf].priority;
  

  /* always using constant init value to generate default mapping
   * regardless of the number of traffic classes (assumes number of
   * traffic classes will never be less than the constant init value)
   */
  memcpy(pPriority->Mapping,
         dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1],
         sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1]));

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Unconfigure what was configured in phase 3 init
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Interfaces are deleted by this time
*       
* @end
*********************************************************************/
L7_RC_t dot1qRestore(void)
{
  L7_RC_t   rc;
  L7_RC_t   failures;
  L7_uint32 vlanId, nextVlanId;
  L7_uint32 intIfNum;
  L7_BOOL   maskNonZero;
  dot1qNotifyData_t vlanData;

  failures = 0;
  rc = L7_SUCCESS;
  vlanData.numVlans = 1;
  vlanData.data.vlanId = DOT1Q_NULL_VLAN;
  /* Notify registered users about change in vlan configuration */
  vlanNotifyRegisteredUsers(&vlanData, 0, VLAN_RESTORE_NOTIFY);

  vlanId     = 0;
  nextVlanId = 0;

  /* now, we'll remove all static VLANs.
   * Dynamic VLANs have been deleted by calling dot1qGVRPEnable(L7_DISABLE);

   */

  osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
   /* Vlan restore takes a lot of time in the driver trying to delete each vlan 
       port member.
       One way to speed it up is to ask the driver to clear its config for eah interface
   */
  for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
  {

      /* This function might be called during clear configduring which 
      the interface would already be cleared by the detach*/
      L7_VLAN_NONZEROMASK(dot1qIntfVidList[intIfNum],maskNonZero);
      if ((maskNonZero == L7_TRUE) && 
          (dtlDot1qUnConfigInterface(intIfNum,(L7_uchar8 *)&dot1qIntfVidList[intIfNum].value)
            != L7_SUCCESS))
      {
          LOG_MSG("dtl request failure when deleting intIfNum %d \n", 
                  intIfNum );
      }
  }

  osapiSemaGive(dot1qSem);
  while(dot1qNextStaticVlanGet(vlanId, &nextVlanId) == L7_SUCCESS)
  {           
      rc = dot1qVlanDeleteProcess(nextVlanId, DOT1Q_ADMIN);

      if (rc == L7_SUCCESS) 
      {
         (void)osapiWriteLockTake(dot1qCfgRWLock, L7_WAIT_FOREVER);
         (void)dot1qVlanConfigEntryReset(nextVlanId);
         (void)osapiWriteLockGive(dot1qCfgRWLock);
		  dot1qCurrentVlanCountUpdate(L7_FALSE);
      }
	  else
	  {
          failures++;
	  }

      vlanId = nextVlanId;
  }

  if (failures)
  {
    LOG_MSG("Failures occurred in setting default VLAN configuration\n");    
    trapMgrDefaultVlanCfgFailureLogTrap(vlanId);
    rc = L7_FAILURE;
  }

  dot1qBuildDefaultConfigData(dot1qCfg->hdr.version);

  dot1qCfg->hdr.dataChanged = L7_TRUE;


  return(rc);
}



/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    vlanId      @b{(input)} VLAN ID
* @param    **pVCfg     @b{(output)}  Ptr  to VLAN cfg structure
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes   
*
*
* @end
*********************************************************************/
L7_BOOL dot1qVlanConfigEntryGet(L7_uint32 vlanId, dot1qVlanCfgData_t **pVCfg)
{
  L7_uint32 i;

  if (dot1qCnfgrStateCheck() != L7_TRUE)
    return L7_FALSE;

  for (i = 1; i < L7_MAX_VLANS; i++)
  {
    if (dot1qCfg->Qvlan[i].vlanId == L7_NULL)
    {
      dot1qVlanCfgMapTbl[vlanId] =  i;
      *pVCfg = &dot1qCfg->Qvlan[i];
      return L7_TRUE;
    }
  }

   return L7_FALSE;
}



/*********************************************************************
* @purpose  Reset the config structure for the VLAN
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL dot1qVlanConfigEntryReset(L7_uint32 vlanId)
{
    dot1qVlanCfgData_t *vCfg;

    /* Check for pre-existence of the static VLAN */
    if (dot1qVlanIsConfigurable(vlanId, &vCfg) != L7_TRUE)
    {
        /* Static vlan cfg does not exist */
        return L7_FAILURE;
    }
      
    /*  Update Config */
    memset((void *)vCfg, 0, sizeof(dot1qVlanCfgData_t));
    dot1qVlanCfgMapTbl[vlanId] = 0xFFFFFFFF;

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Request admin VLAN configuration
*
* @param    vid      VLAN ID being requested
* @param    *pVCfg    pointer to dot1qVlanCfgData_t structure containing in which to
*                     the configuration
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1qVlanIsConfigurable(L7_uint32 vid, dot1qVlanCfgData_t **pVCfg)      
{

    L7_uint32 index;

    if (vid < L7_DOT1Q_MIN_VLAN_ID || vid > L7_DOT1Q_MAX_VLAN_ID)
      return L7_FALSE;

    index = dot1qVlanCfgMapTbl[vid];

    if (index == 0xFFFFFFFF)
      return L7_FALSE;

    /* verify that the vlanId associated with the config data table entry matches the vlanId
      that is under consideration
     */

    if ( vid != dot1qCfg->Qvlan[index].vlanId)
    {
      /* if we get here, either we have a table management error between dot1qCfgData and 
         dot1qVlanCfgMapTbl or there is synchronization issue between dot1q and components 
         w.r.t. vlan creation/deletion
       */
        LOG_MSG("Error accessing DOT1Q config data for VLAN %d in dot1qVlanIsConfigurable\n", 
                vid);
        return L7_FALSE;

    }


    *pVCfg = &dot1qCfg->Qvlan[index];

    return L7_TRUE;


 
}

/*********************************************************************
* @purpose  Set VLAN port membership configuration
*
* @param    vid         vlan ID
* @param    intf        physical or logical interface to be added
*                       or L7_ALL_INTERFACES add all interfaces
* @param    mode        mode of participation
*                       (@b{  L7_DOT1Q_FIXED, L7_DOT1Q_FORBIDDEN, or
*                             L7_DOT1Q_NORMAL_REGISTRATION})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    requestor DOT1Q_ADMIN is for all non GVRP requests
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanMemberSetCfgUpdate(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 mode)
{
    L7_uint32 maskOffset;
    dot1qVlanCfgData_t *vCfg;
    L7_uint32 i;
    L7_uint32 start,end;


    /* Get pointer to VLAN configuration */
    vCfg = L7_NULL;
    (void) dot1qVlanIsConfigurable(vid, &vCfg);
    if (vCfg == L7_NULL) 
        return L7_FAILURE;


    /* Determine the range of interfaces to configure */
    if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
    {
      return L7_NOT_EXISTS;
    }

    if (intIfNum == L7_ALL_INTERFACES)
    {
      start = 1;
      end = intIfNum;
    }
    else
    {
      start = intIfNum;
      end = intIfNum+1;
    }



    /* Configure the VLAN */
    for (i = start; i < end; i++)
    {
        /* Check for participating interfaces */
        if (dot1qIntfIsValid(i) != L7_TRUE) 
            continue;

        /* Convert intIfNum to nimConfigIDMaskOffSet for storage in the config */
        if (nimConfigIdMaskOffsetGet(i, &maskOffset) != L7_SUCCESS)
            continue;

        switch (mode)
        {
          case L7_DOT1Q_FIXED:
          {
            NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts,maskOffset);
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts,maskOffset);
          }
          break;

          case L7_DOT1Q_NORMAL_REGISTRATION:
          {
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts,maskOffset);
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts,maskOffset);
          }
          break;

          case L7_DOT1Q_FORBIDDEN:
          {
            NIM_INTF_SETMASKBIT(vCfg->vlanCfg.forbiddenEgressPorts,maskOffset);
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.staticEgressPorts,maskOffset);
          }
          break;

         default:
            break;
        }

     }  /* for (i = start; i < end; i++) */ 


    return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Set the tagging configuration for a member port
*
* @param    vid         vlan ID
* @param    intIfNum        physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param    tagMode     tagging mode
*                       (@b{  L7_DOT1Q_UNTAGGED or 
*                             L7_DOT1Q_TAGGED})
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if VLAN does not exist
* @returns  L7_ERROR, if attempt to configure a dynamic vlan
* @returns  L7_FAILURE, if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t   dot1qVlanTaggedMemberSetCfgUpdate(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 tagMode)
{
    L7_uint32 maskOffset;
    dot1qVlanCfgData_t *vCfg;
    L7_uint32 i;
    L7_uint32 start,end;

    /* Get pointer to VLAN configuration */
    vCfg = L7_NULL;
    (void) dot1qVlanIsConfigurable(vid, &vCfg);
    if (vCfg == L7_NULL) 
        return L7_FAILURE;

    /* Determine the range of interfaces to configure */
    if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
    {
      return L7_NOT_EXISTS;
    }

    if (intIfNum == L7_ALL_INTERFACES)
    {
      start = 1;
      end = intIfNum;
    }
    else
    {
      start = intIfNum;
      end = intIfNum+1;
    }


    /* Configure the VLAN */
    for (i = start; i < end; i++)
    {
        /* Check for participating interfaces */
        if (dot1qIntfIsValid(i) != L7_TRUE) 
            continue;

        /* Convert intIfNum to nimConfigIDMaskOffSet for storage in the config */
        if (nimConfigIdMaskOffsetGet(i, &maskOffset) != L7_SUCCESS)
            continue;

        if (tagMode == L7_DOT1Q_TAGGED)
            NIM_INTF_SETMASKBIT(vCfg->vlanCfg.taggedSet, maskOffset);
        else
            NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, maskOffset);

     }  /* for (i = start; i < end; i++) */ 


    return L7_SUCCESS;

}


L7_RC_t dot1qCfgCleanOperData(L7_uint32 intIfNum)
{
    dot1q_vlan_dataEntry_t *pDataEntry = L7_NULLPTR;
    L7_uint32 vlanId;

    osapiSemaTake(dot1qSem, L7_WAIT_FOREVER);
    pDataEntry = (dot1q_vlan_dataEntry_t*)(vlanFirstDataEntry(pVlanTree));
    while (pDataEntry != NULL)
    {
        vlanId = pDataEntry->vlanId;
        NIM_INTF_CLRMASKBIT(pDataEntry->current.currentEgressPorts, intIfNum);
        NIM_INTF_CLRMASKBIT(pDataEntry->current.taggedSet, intIfNum);
        

        pDataEntry = vlanNextDataEntry(pVlanTree, pDataEntry ->vlanId);



    } /* end for vlanId = NULL  */
    osapiSemaGive(dot1qSem); 

#if 0
    if (dtlDot1qConfigInterface(intIfNum,(L7_uchar8 *)memberVlans, (L7_uchar8 *)taggedVlans)
        != L7_SUCCESS)
    {
        LOG_MSG("dtl request failure when deleting intIfNum %d \n", 
                intIfNum );

        return L7_FAILURE;

    }
#endif

    return L7_SUCCESS;

}

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/

/* TBD:  Make these generic utilities */
static void dot1qBuildRandomIntfMask (L7_uint32 vlanId, NIM_INTF_MASK_t *pMask, L7_uint32 seed);
static void dot1qVlanMaskDump(L7_VLAN_MASK_t *pMask);
static void dot1qIntfMaskDump(NIM_INTF_MASK_t *pMask);

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
void dot1qBuildTestConfigData(void)
{

    L7_char8 buf[32];
    dot1qVlanCfgData_t *vCfg;
    dot1qIntfCfgData_t *pCfg;
    L7_uint32 i, intIfNum;
    L7_RC_t rc;
    L7_uint32 unit;

    unit = usmDbThisUnitGet();

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  
    for (i = 1; i <= L7_MAX_VLANS; i++)
    {

        vCfg = &dot1qCfg->Qvlan[i];

        vCfg->vlanId = i;  
        
        sprintf(buf,"Test VlanID %d", i);
        strcpy( (char*)&vCfg->vlanCfg.name, buf);

       /* vCfg->vlanCfg.flags = i;  */
        
        dot1qBuildRandomIntfMask (i, &vCfg->vlanCfg.staticEgressPorts, i);
        dot1qBuildRandomIntfMask (i, &vCfg->vlanCfg.forbiddenEgressPorts, i+1);
        dot1qBuildRandomIntfMask (i, &vCfg->vlanCfg.taggedSet, i+2);

#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
        dot1qBuildRandomIntfMask (i, &vCfg->vlanCfg.groupFilter.forwardAllMask, i+3);
        dot1qBuildRandomIntfMask (i, &vCfg->vlanCfg.groupFilter.forwardUnregMask, i+4);
#endif
    }

    /* build dot1p global priority defaults */
    dot1qCfg->Qglobal.priority.DefaultUserPriority = 3;

    /* build defaults for already existing interfaces
       newly created interfaces' defaults will be created by the l7_create */

    if (dot1qCnfgrStateCheck() == L7_TRUE)
    {
      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
        if ((rc = dot1qMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
        {

          pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID] = 3;
          pCfg->portCfg.VID[DOT1Q_NETBIOS_PVID] = 2;
          pCfg->portCfg.acceptFrameType         = L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED;

          if (L7_TRUE == usmDbFeaturePresentCheck(unit, L7_DOT1Q_COMPONENT_ID, L7_DOT1Q_INGRESSFILTER_FEATURE_ID))
          {
            pCfg->portCfg.enableIngressFiltering = L7_ENABLE;
          } else {
            pCfg->portCfg.enableIngressFiltering = L7_FALSE;
          }

        }
        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }
    }
  return;


 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   dot1qCfg->hdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}


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
void dot1qBuildRandomIntfMask (L7_uint32 vlanId, NIM_INTF_MASK_t *pMask, L7_uint32 seed)
{

    L7_uint32  j, k;
    L7_uint32 min, max; 
    NIM_INTF_MASK_t temp;


    memcpy(&temp, pMask, NIM_INTF_INDICES);

    /* For test purposes, only turn on a subset of eligible ports */

    k = vlanId + seed%L7_MAX_PORT_COUNT;  /*  seed to randomize mask */

    
    (void) nimIntIfNumRangeGet(L7_LAG_INTF, &min, &max);

    for (j = 1; j <= max; j++)
    {
 
      /* LAGs are the highest intIfNums eligible to participate in VLANs */

      /*  Force each group to have a different port list range */
      if (j < (2*k) ) 
      {
          continue;
      }


      /*  Force each group to have a different port list range */
      if (j > (4*k) ) 
      {
          continue;
      }
      NIM_INTF_SETMASKBIT( (NIM_INTF_MASK_t) temp, j);    

      k++;
    }


    memcpy(pMask, &temp, NIM_INTF_INDICES);
}


#if defined(DOT1Q_DEBUG_UNIT_TEST) && DOT1Q_DEBUG_UNIT_TEST

/*********************************************************************
* @purpose  Build test intf Mask
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dot1qDebugTestIntfMask(void)
{
    L7_INTF_MASK_t temp;

    /* Build arbitrary interface Mask */
    dot1qBuildRandomIntfMask (4, (NIM_INTF_MASK_t *)&temp, 6);
    dot1qDebugTestIntfMaskDump (&temp);

}

#endif


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
void dot1qCfgShow(void)
{

    dot1qVlanCfgData_t *vCfg;
    dot1qIntfCfgData_t *pCfg;
    L7_uint32 intIfNum; 
    L7_uint32 i, j;
    nimUSP_t usp;

   sysapiPrintf( "\n");


   /*---------------------------------------------------------------------*/
   /*
          
           Qvlan
   */    
   /*---------------------------------------------------------------------*/


   for (i = 0; i <= L7_MAX_VLANS; i++)
   {
       vCfg = &dot1qCfg->Qvlan[i];
       
       sysapiPrintf( "ENTRY %d: \n", i );
       sysapiPrintf( "--------- \n" );
       sysapiPrintf( "--------- \n" );

       sysapiPrintf( "vlanId  %d\n", vCfg->vlanId);
       sysapiPrintf( "name  %s\n", vCfg->vlanCfg.name);
       /*sysapiPrintf( "flags  %d\n", vCfg->vlanCfg.flags);*/

       sysapiPrintf( "staticEgressPorts\n");
       sysapiPrintf( "-----------------\n");

       dot1qIntfMaskDump (&vCfg->vlanCfg.staticEgressPorts);

       sysapiPrintf( "forbiddenEgressPorts\n");
       sysapiPrintf( "--------------------\n");

       dot1qIntfMaskDump (&vCfg->vlanCfg.forbiddenEgressPorts);

       sysapiPrintf( "taggedSet\n");
       sysapiPrintf( "----------\n");

       dot1qIntfMaskDump (&vCfg->vlanCfg.taggedSet);

       sysapiPrintf( "groupFilter.forwardAllMask\n");
       sysapiPrintf( "--------------------------\n");


#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
       dot1qIntfMaskDump (&vCfg->vlanCfg.groupFilter.forwardAllMask);

       sysapiPrintf( "groupFilter.forwardUnregMask\n");
       sysapiPrintf( "----------------------------\n");

       dot1qIntfMaskDump (&vCfg->vlanCfg.groupFilter.forwardUnregMask);
#endif
   }

   /*---------------------------------------------------------------------*/
   /*
          
           Qglobal
   */    
   /*---------------------------------------------------------------------*/


   sysapiPrintf( "Qglobal\n");
   sysapiPrintf( "-------\n");


   sysapiPrintf( "DefaultUserPriority   = %d\n", dot1qCfg->Qglobal.priority.DefaultUserPriority);
   sysapiPrintf( "NumTrafficClasses   = %d\n",dot1qCfg->Qglobal.priority.NumTrafficClasses);


   sysapiPrintf( "Mapping  :\n");
   for (i = 0; i <= L7_DOT1P_MAX_PRIORITY; i++) 
   {
       sysapiPrintf( "  %02X:\n", dot1qCfg->Qglobal.priority.Mapping[i] );
   }

   sysapiPrintf( "\n");

   sysapiPrintf("\nBase internal VLAN:  %u", dot1qCfg->Qglobal.baseInternalVlan);
   sysapiPrintf("\nInternal VLAN policy:  %s", 
                (dot1qCfg->Qglobal.internalVlanPolicy == DOT1Q_INT_VLAN_POLICY_DESCENDING) ?
                "Descending" : "Ascending");

   /*---------------------------------------------------------------------*/
   /*
          
           Qports
   */    
   /*---------------------------------------------------------------------*/


   sysapiPrintf( "Qglobal\n");
   sysapiPrintf( "-------\n");


   for (i = 1; i <= DOT1Q_INTF_MAX_COUNT; i++)
   {
       pCfg = &dot1qCfg->Qports[i];

       if (nimIntIfFromConfigIDGet(&(pCfg->configId), &intIfNum) != L7_SUCCESS)
         continue;
       
       if (dot1qIntfIsValid(intIfNum) == L7_FALSE)
           continue; 
                     
       memset(&usp, 0, sizeof(nimUSP_t));
       if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
           continue; 

       sysapiPrintf( "ENTRY %d:  %d/%d/%d \n", i, usp.unit, usp.slot, usp.port );
       sysapiPrintf( "--------- \n" );


       sysapiPrintf( "VID array:   \n");
       for (j = 0; j < DOT1Q_MAX_PVID; j++)
       {
           sysapiPrintf( "  %02X:\n", pCfg->portCfg.VID[j] );
       }
       sysapiPrintf( "\n");

       sysapiPrintf( "acceptFrameType  %d\n", pCfg->portCfg.acceptFrameType);
       sysapiPrintf( "enableIngressFiltering  %d\n", pCfg->portCfg.enableIngressFiltering);


       sysapiPrintf( "\n");
       sysapiPrintf( "priority \n");
       sysapiPrintf( "DefaultUserPriority   = %d\n", pCfg->portCfg.priority.DefaultUserPriority);
       sysapiPrintf( "NumTrafficClasses   = %d\n",pCfg->portCfg.priority.NumTrafficClasses);
       sysapiPrintf( "Mapping   :\n");
       for (j = 0; j <= L7_DOT1P_MAX_PRIORITY; j++) 
       {
           sysapiPrintf( "  %02X:\n", pCfg->portCfg.priority.Mapping[j] );
       }

       sysapiPrintf( "\n");
       sysapiPrintf( "\n");

       /* WPJ TBD:  This should be shown in the status show */
       sysapiPrintf( "portCapabilities  %d\n", dot1qQports[intIfNum].portCapabilities);

   }

   
   /*---------------------------------------------------------------------*/
   /*
           QvlanIntf
   */    
   /*---------------------------------------------------------------------*/


   sysapiPrintf( "VLAN Interface Mask\n");
   sysapiPrintf( "-------------------\n");

   dot1qVlanMaskDump (&dot1qCfg->QvlanIntf);

}


/*********************************************************************
*
* @purpose  Dump the contents of the interface mask
*
* @param    pMask   Pointer to a NIM_INTF_MASK_t mask
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot1qIntfMaskDump(NIM_INTF_MASK_t *pMask)
{

    nimUSP_t usp;
    L7_uint32 i, j;
    NIM_INTF_MASK_t temp;


    memcpy(&temp, pMask, NIM_INTF_INDICES);

    for (i=1,j=0; i < L7_MAX_INTERFACE_COUNT; i++)
    {
        if (NIM_INTF_ISMASKBITSET(temp, i) )
        {
            memset((void *)&usp, 0, sizeof(nimUSP_t));
            if (nimGetUnitSlotPort(i, &usp) == L7_SUCCESS)
            {
                sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
            }
            else
            {
                sysapiPrintf("Bit %d unknown", i);
            }
            
            /* Print no more than 10 ports on a line */

            j++;
            if (j%10 == 0)    /* Sleepy hack*/
            {
                sysapiPrintf( "\n");
            }
        }
    }
}



/*********************************************************************
*
* @purpose  Dump the contents of the VLAN mask
*
* @param    pMask   Pointer to a DOT1Q_VLAN_MASK_t mask
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot1qVlanMaskDump(L7_VLAN_MASK_t *pMask)
{

    L7_uint32 i, j;
    L7_VLAN_MASK_t temp;



    memcpy(&temp, pMask, sizeof(temp)); 

    sysapiPrintf( "\n");
    for (i=0, j=0; i < L7_PLATFORM_MAX_VLAN_ID; i++)
    {
        if (L7_VLAN_ISMASKBITSET(temp, i) )
        {

            sysapiPrintf("  %d, ",  i );

            /* Print no more than 10 ports on a line */

            j++;
            if (j%10 == 0 )    /* Sleepy hack*/
            {
                sysapiPrintf( "\n");
            }
        }
    }
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
void dot1qConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(dot1qCfg->hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    dot1qCfgShow();


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_DOT1Q_MAX_INTF - %d\n", DOT1Q_INTF_MAX_COUNT);
    sysapiPrintf( "L7_MAX_VLANS - %d\n", L7_MAX_VLANS);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("dot1qCfg->checkSum : %u\n", dot1qCfg->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




