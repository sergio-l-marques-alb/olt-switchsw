/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: vrrp_map.c
*
* Purpose: This file contains the VRRP Mapping system infrastructure.
*
* Component: Virtual Router redundancy protocol(VRRP)
*
* Comments:
*
* @create    28/11/2001
*
* @author    Kavleen Kaur et. al.
*
**********************************************************************/

#define L7_MAC_ENET_VRRP                          /* vrrp mac addr      */

#include "l7_common.h"

#include "nimapi.h"

#include "l3_commdefs.h"
#include "l3_comm_structs.h"
#include "default_cnfgr.h"
#include "l3_default_cnfgr.h"

#include "l7_vrrpinclude.h"
/* #include "l7_vrrp_api.h" */
/* #include "vrrp_config.h" */


#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "vrrp_util.h"
#include "vrrptask.h"

#include "l7_packet.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "l3_defaultconfig.h"
#include "vrrp_debug.h"
#include "vrrp_debug_api.h"
#include "vrrp_cnfgr.h"
#include "support_api.h"


/* VRRP mapping component globals */
L7_int32                L7_vrrp_task_id;
L7_vrrpMapCfg_t        *pVrrpMapCfgData = L7_NULL;

/* Cannot hold this semaphore when calling an IP MAP API. IP MAP can call
 * ARP, which can call VRRP. Can get a deadlock. */
void  *                 VrrpConfigSema;
L7_int32                vrrpTaskDaemonId;
L7_uint32              *vrrpIntfMapTbl;


/*global variable*/
extern vrrpList_t* paramHead_g;
extern L7_short16 totalRouters_g;
extern vrrpList_t* paramCurrent_g;
extern vrrpRouterBasicConfiguration_t  vrrpBasicConfig_g;
extern vrrpList_t * listMemPool_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpRouterInfo_t * routersArray_g[L7_VRRP_INTF_MAX_COUNT + 1];
extern vrrpIntfInformation_t * vrrpIntfInfoTbl;
extern L7_BOOL isChanged_g;

extern vrrpCnfgrState_t vrrpCnfgrState;
vrrpDebugCfg_t vrrpDebugCfg;

extern void vrrpVRIdToIndexMapTblInit();
extern void vrrpDebugCfgUpdate(void);

static void vrrpBuildDefaultVrConfigData(L7_uint32 vrIndex);

/*********************************************************************
* @purpose  Start vrrp tasks
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpStartTasks(void)
{
  L7_RC_t  rc = L7_SUCCESS;

  vrrpTaskDaemonId = osapiTaskCreate( "VRRPdaemon", VRRPDaemonTask, 0, 0,
                                      L7_DEFAULT_STACK_SIZE,
                                      L7_DEFAULT_TASK_PRIORITY,
                                      L7_DEFAULT_TASK_SLICE);

  if (vrrpTaskDaemonId == L7_ERROR)
  {
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_VRRP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                    "%s: %d: Unable to synchronize VRRP daemon task\n", __FILE__, __LINE__);
            return L7_FAILURE;
        }



  return(rc);
}


/*********************************************************************
* @purpose  Save VRRP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpSave(void)
{
  vrrpListMapping();
  if (pVrrpMapCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_FALSE;
    pVrrpMapCfgData->checkSum =
    nvStoreCrc32((L7_char8 *)pVrrpMapCfgData,
                 sizeof(L7_vrrpMapCfg_t) - sizeof(pVrrpMapCfgData->checkSum));


    if (sysapiCfgFileWrite(L7_VRRP_MAP_COMPONENT_ID, L7_VRRP_CFG_FILENAME,
                           (L7_char8 *)pVrrpMapCfgData, sizeof (L7_vrrpMapCfg_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
              "vrrpSave: Error during osapiFsWrite for config file %s\n",
              L7_VRRP_CFG_FILENAME);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore VRRP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpRestoreProcess(void)
{
  L7_uchar8 nextVrId,vrID;
  L7_int32 checkAvailability;
  L7_uint32 nextInfNum,infNum;
  /* Stop all the Virtual Router */
  L7_vrrpStopAll();

  /* Delete Virtual Router configuration*/
  if (L7_vrrpFirstGet(&vrID,&infNum)== L7_SUCCESS)
  {
    do
    {
      checkAvailability=L7_vrrpNextGet(vrID,infNum,&nextVrId,&nextInfNum);
      L7_vrrpVirtualRouterConfigDelete( vrID,infNum);
      vrID = nextVrId;
      infNum = nextInfNum;

    }while (checkAvailability==L7_SUCCESS);

  }

  vrrpVRIdToIndexMapTblInit();

  vrrpBuildDefaultConfigData(L7_VRRP_CFG_VER_CURRENT);
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  return(vrrpApplyConfigData());


}

/*********************************************************************
* @purpose  Check if VRRP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL vrrpHasDataChanged(void)
{
  return pVrrpMapCfgData->cfgHdr.dataChanged;
}
void vrrpResetDataChanged(void)
{
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Build default VRRP config data for an element in the virtual router array.
*
* @param    vrIndex    @b{(input)}  Index of VR in virtRouter configuration array
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void vrrpBuildDefaultVrConfigData(L7_uint32 vrIndex)
{
  L7_uint32 k;
  L7_uint32 ipAddrCount;

  pVrrpMapCfgData->virtRouter[vrIndex].vrid = FD_VRRP_DEFAULT_VRID;
  pVrrpMapCfgData->virtRouter[vrIndex].priority = FD_VRRP_DEFAULT_PRIORITY;
  pVrrpMapCfgData->virtRouter[vrIndex].preempt_mode = FD_VRRP_DEFAULT_PREEMPT;
  pVrrpMapCfgData->virtRouter[vrIndex].adver_int = FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL;
  pVrrpMapCfgData->virtRouter[vrIndex].auth_types = FD_VRRP_DEFAULT_AUTH_TYPE;
  pVrrpMapCfgData->virtRouter[vrIndex].vr_start_state = FD_VRRP_DEFAULT_START_STATE;
  pVrrpMapCfgData->virtRouter[vrIndex].timer_learn   = FD_VRRP_DEFAULT_TIMER_LEARN_MODE;
  pVrrpMapCfgData->virtRouter[vrIndex].preempt_delay = FD_VRRP_DEFAULT_PREEMPT_DELAY;
  memset(pVrrpMapCfgData->virtRouter[vrIndex].description,0x00,L7_VRRP_MAX_DESCRIPTION);
  for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS; ipAddrCount++)
  {
    pVrrpMapCfgData->virtRouter[vrIndex].ipaddress[ipAddrCount] = FD_VRRP_DEFAULT_IP_ADDRESS;
  }
  for (k = 0; k < L7_VRRP_MAX_TRACK_INTF_PER_VR; k++)
  {
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_intf[k].intIfNum = 0;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_intf[k].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_intf[k].stateUp = L7_FALSE;
  }
  for (k = 0; k < L7_VRRP_MAX_TRACK_RT_PER_VR; k++)
  {
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_route[k].ipAddr = FD_VRRP_DEFAULT_IP_ADDRESS;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_route[k].subnetMask = FD_VRRP_DEFAULT_IP_ADDRESS;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_route[k].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_route[k].reachable = L7_FALSE;
    pVrrpMapCfgData->virtRouter[vrIndex].vr_track_route[k].inUse = L7_FALSE;
  }
  for (k = 0; k < L7_VRRP_MAX_AUTH_DATA; k++)
  {
    pVrrpMapCfgData->virtRouter[vrIndex].auth_data[k] = FD_VRRP_DEFAULT_AUTH_DATA_KEY_CHAR;
  }
}

/*********************************************************************
* @purpose  Build default VRRP config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpBuildDefaultConfigData(L7_uint32 ver)
{

  L7_uint32 v;   /* virtual router index */

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/
  memset(pVrrpMapCfgData, 0, sizeof(L7_vrrpMapCfg_t));

  strcpy(pVrrpMapCfgData->cfgHdr.filename, L7_VRRP_CFG_FILENAME);
  pVrrpMapCfgData->cfgHdr.version       = ver;
  pVrrpMapCfgData->cfgHdr.componentID   = L7_VRRP_MAP_COMPONENT_ID;
  pVrrpMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pVrrpMapCfgData->cfgHdr.length        = sizeof(L7_vrrpMapCfg_t);
  pVrrpMapCfgData->cfgHdr.dataChanged   = L7_FALSE;
  pVrrpMapCfgData->cfgHdr.version       = ver;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic VRRP cfg */
  /* If VRRP was enabled, deregister with RTO */
  if (pVrrpMapCfgData->rtr.vrrpAdminMode)
  {
    vrrpRtoRegister();
  }
  pVrrpMapCfgData->rtr.vrrpAdminMode = FD_VRRP_DEFAULT_ADMIN_MODE;

  /* VRRP router interface configuration parameters */
  for (v = 0; v < L7_VRRP_MAX_VIRT_ROUTERS; v++)
  {
    vrrpBuildDefaultVrConfigData(v);
  }
}

/*********************************************************************
* @purpose  Apply VRRP config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied, or not a VRRP interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpApplyConfigData(void)
{
  vrrpList_t* listTail = L7_NULL;
  vrrpList_t* temp = L7_NULL;
  vrrpRouterInfo_t *  vrrpPkt = L7_NULL;
  L7_uint32 intIfNum;
  L7_uint32 count=0;
  L7_short16 intfInfoTblIndex=0;
  L7_uchar8 vrid, arrayIdxForVRId;
  L7_RC_t   rc;
  L7_uint32 ipAddrCount;

  if (vrrpSysnetRegister(pVrrpMapCfgData->rtr.vrrpAdminMode) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
            "vrrpApplyConfigData: Error registering with sysnet\n");
    return L7_FAILURE;
  }


  if(VRRP_IS_READY)
  {
      while (pVrrpMapCfgData->virtRouter[count].vrid !=0)
      {
        if (nimIntIfFromConfigIDGet(&pVrrpMapCfgData->virtRouter[count].configId,&intIfNum) != L7_SUCCESS)
        {
          continue;
        }

        if(vrrpIntfIsConfigurable(intIfNum, L7_NULLPTR) == L7_TRUE)
        {
            continue;
        }

        vrid = pVrrpMapCfgData->virtRouter[count].vrid;
        intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);

        if (intfInfoTblIndex == -1)
        {
          /* Invalid Interface Number */
          return L7_FAILURE;
        }



        rc = vrrpVRIdToIndexMap(vrid,intIfNum);
        if(rc ==  L7_TABLE_IS_FULL)
            return L7_FAILURE;
        arrayIdxForVRId = vrrpVRIdToIndexGet(vrid, intIfNum);

        temp = &(listMemPool_g[intfInfoTblIndex][arrayIdxForVRId-1]);
        vrrpPkt = &(routersArray_g[intfInfoTblIndex][arrayIdxForVRId-1]);
        memset(vrrpPkt,0, sizeof(vrrpRouterInfo_t));
        (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)++;
        vrrpIntfInfoTbl[intfInfoTblIndex].intIfNum = intIfNum;

        memcpy(&(vrrpPkt->vrrpCfgInfo), &(pVrrpMapCfgData->virtRouter[count]),
               sizeof(vrrpVirtRouterCfg_t));


        bzero((vrrpPkt->vrrpVirtRouterOperInfo.vmac),L7_MAC_ADDR_LEN);
        memcpy( vrrpPkt->vrrpVirtRouterOperInfo.vmac , &(L7_ENET_VRRP_MAC_ADDR) ,L7_MAC_ADDR_LEN - 1);
        vrrpPkt->vrrpVirtRouterOperInfo.vmac[5] =vrid;
        vrrpPkt->vrrpVirtRouterOperInfo.intIfNum = intIfNum;
        vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount=0;
        vrrpPkt->vrrpVirtRouterOperInfo.master_down_int = 3*vrrpPkt->vrrpCfgInfo.adver_int;
        vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds=0;
        vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds=0;
        vrrpPkt->vrrpVirtRouterOperInfo.protocol_type = L7_VRRP_IP;
        vrrpPkt->vrrpVirtRouterOperInfo.assoc_ip_rowstatus = L7_VRRP_ACTIVE;
        vrrpPkt->vrrpVirtRouterOperInfo.oper_rowstatus = L7_VRRP_ACTIVE;
        vrrpPkt->vrrpVirtRouterOperInfo.master_advt_int = vrrpPkt->vrrpCfgInfo.adver_int;
        vrrpPkt->vrrpVirtRouterOperInfo.preempt_delay_config = FD_VRRP_DEFAULT_PREEMPT_DELAY_CONFIG;
        vrrpPkt->vrrpVirtRouterOperInfo.preempt_startTime.seconds=0;
        vrrpPkt->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds=0;

        for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS ; ipAddrCount ++)
        {
          if ( vrrpPkt->vrrpCfgInfo.ipaddress[ipAddrCount] != FD_VRRP_DEFAULT_IP_ADDRESS)
          {
            vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount =
                                   vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount + 1;
          }
        }

        temp->object = vrrpPkt;
        temp->next = L7_NULL;
        if ( count == 0)
        {
          paramHead_g = temp;
          listTail = paramHead_g;
          temp->previous = L7_NULL;
        }
        else
        {
          listTail->next = temp;
          temp->previous = listTail;
          listTail = temp;
        }
        if (L7_SUCCESS == L7_vrrpCheckPrecondition(vrrpPkt))
        {
          L7_vrrpRouterStart(vrrpPkt);
        }
        else
        {
          vrrpPkt->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_INIT;
        }
        totalRouters_g++;
        count++;
      }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Register for Sysnet Intercept hook points
*
* @param    mode  @b{(input)} mode of registration (register or de-register)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpSysnetRegister(L7_BOOL mode)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  if (mode == L7_TRUE)
  {
    /* Register for all IP frames to see if any should be dropped due to VRRP criteria */
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_VRRP_FILTER_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = vrrpIpFrameFilter;
    strcpy(sysnetPduIntercept.interceptFuncName, "vrrpIpFrameFilter");
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
      return L7_FAILURE;

    /* Register for valid IP frames to see if any are VRRP advertisements */
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_VALID_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_VRRP_ADV_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = vrrpPacketIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "vrrpPacketIntercept");
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
      return L7_FAILURE;
  }
  else
  {
    /* De-register all IP frames intercept function from sysnet */
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_VRRP_FILTER_PRECEDENCE;
    (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);

    /* De-register valid IP frames intercept function from sysnet */
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_VALID_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_VRRP_ADV_PRECEDENCE;
    (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register with RTO for best route changes
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpRtoRegister(void)
{
  if (pVrrpMapCfgData->rtr.vrrpAdminMode)
  {
    /* VRRP enabled. Register for best route changes. */
    if ((rtoBestRouteClientRegister("VRRP", vrrpMapRouteCallback)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
             "Failed to register for best route changes.");
    }
  }
  else
  {
    /* VRRP disabled. Deregister. */
    if (rtoBestRouteClientDeregister("VRRP", vrrpMapRouteCallback) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
             "Failed to deregister for best route changes.");
    }
  }
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
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t vrrpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t         configId;
  vrrpIntfInformation_t *pIntfInfo;
  L7_short16 intfTblIndex;
  L7_RC_t   rc;

  if (L7_vrrpIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;


  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;


  intfTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
  if(intfTblIndex == -1)
  {
      /* Couldnt allocate any space to maintain the Intf's info
       * Which means max VRs reached.
       * Return L7_SUCCESS for the NIM event handling to succeed */
      return L7_SUCCESS;
  }


  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (vrrpIntfIsConfigurable(intIfNum, &pIntfInfo) != L7_TRUE)
  {
    if (vrrpIntfInfoTblEntryGet(intIfNum, &pIntfInfo) != L7_TRUE)
        return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pIntfInfo != L7_NULL)
      vrrpIntfBuildDefaultConfigData(intIfNum, pIntfInfo);
  }

  rc = vrrpIntfCreateOperInfo(intIfNum);

  return rc;
}

/*********************************************************************
*
* @purpose  Updates the operation information structure for the interface
*           `intIfNum'
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The operational information is maintained in a doublylinked list
*           whose head (left end) is paramHead_g and the list is ordered by
*           increasing intIfNum primarily and increasing VRId if the `intIfNum'
*           is the same.
*
* @end
*
*********************************************************************/
L7_RC_t vrrpIntfCreateOperInfo(L7_uint32 intIfNum)
{
    vrrpList_t         *intfHead; /* Reference to the point in the ordered list from where
                                     virtual routers running on `intIfNum' will be stored */
    vrrpList_t         *intfHeadPrev;
    vrrpList_t          *insBefore, *insAfter;
    vrrpList_t*         temp = L7_NULL;
    vrrpRouterInfo_t *  vrrpPkt = L7_NULL;
    L7_short16          intfInfoTblIndex=0;
    L7_uchar8           vrid;
    L7_uint32           count;
    L7_uint32           ipAddrCount;
    nimConfigID_t       configId;
    L7_RC_t             rc;
    L7_uchar8           arrayIdxForVRId;

    intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
    if (intfInfoTblIndex == -1)
    {
        /* Invalid Interface Number */
        return L7_FAILURE;
    }

    if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    /* Advance `intfHead' to the point from where entries for virtual routers running on
     `intIfNum'  should be inserted. If other entries exist for this interface, intfHead
     will point to first such entry; else it will point to the entry before which the new
     entry should be inserted.  */
    intfHead = paramHead_g;
    intfHeadPrev = L7_NULLPTR;
    while(intfHead != L7_NULLPTR && intfHead->object->vrrpVirtRouterOperInfo.intIfNum < intIfNum)
    {
        intfHeadPrev = intfHead;
        intfHead = intfHead->next;
    }

    for (count = 0; count < L7_VRRP_MAX_VIRT_ROUTERS; count++)
    {
        if (NIM_CONFIG_ID_IS_EQUAL(&pVrrpMapCfgData->virtRouter[count].configId, &configId))
        {
            vrid = pVrrpMapCfgData->virtRouter[count].vrid;

            rc = vrrpVRIdToIndexMap(vrid,intIfNum);
            if(rc ==  L7_TABLE_IS_FULL)
                return L7_FAILURE;
            arrayIdxForVRId = vrrpVRIdToIndexGet(vrid, intIfNum);

            temp = &(listMemPool_g[intfInfoTblIndex][arrayIdxForVRId-1]);
            vrrpPkt = &(routersArray_g[intfInfoTblIndex][arrayIdxForVRId-1]);
            memset(vrrpPkt,0, sizeof(vrrpRouterInfo_t));
            (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)++;
            vrrpIntfInfoTbl[intfInfoTblIndex].intIfNum = intIfNum;

            memcpy(&(vrrpPkt->vrrpCfgInfo), &(pVrrpMapCfgData->virtRouter[count]),
                   sizeof(vrrpVirtRouterCfg_t));


            bzero((vrrpPkt->vrrpVirtRouterOperInfo.vmac),L7_MAC_ADDR_LEN);
            memcpy( vrrpPkt->vrrpVirtRouterOperInfo.vmac , &(L7_ENET_VRRP_MAC_ADDR) ,L7_MAC_ADDR_LEN - 1);
            vrrpPkt->vrrpVirtRouterOperInfo.vmac[5] =vrid;
            vrrpPkt->vrrpVirtRouterOperInfo.intIfNum = intIfNum;
            vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount=0;
            vrrpPkt->vrrpVirtRouterOperInfo.master_down_int = 3*vrrpPkt->vrrpCfgInfo.adver_int;
            vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds=0;
            vrrpPkt->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds=0;
            vrrpPkt->vrrpVirtRouterOperInfo.protocol_type = L7_VRRP_IP;
            vrrpPkt->vrrpVirtRouterOperInfo.assoc_ip_rowstatus = L7_VRRP_ACTIVE;
            vrrpPkt->vrrpVirtRouterOperInfo.oper_rowstatus = L7_VRRP_ACTIVE;
            vrrpPkt->vrrpVirtRouterOperInfo.preempt_delay_config = FD_VRRP_DEFAULT_PREEMPT_DELAY_CONFIG;
            vrrpPkt->vrrpVirtRouterOperInfo.preempt_startTime.seconds=0;
            vrrpPkt->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds=0;
            for (ipAddrCount = 0; ipAddrCount < L7_L3_NUM_IP_ADDRS ; ipAddrCount ++)
            {
              if ( vrrpPkt->vrrpCfgInfo.ipaddress[ipAddrCount] != FD_VRRP_DEFAULT_IP_ADDRESS)
              {
                vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount =
                                   vrrpPkt->vrrpVirtRouterOperInfo.ipAddressCount + 1;
              }
            }

            temp->object = vrrpPkt;
            temp->next = L7_NULL;

            insBefore = intfHead;
            insAfter  = intfHeadPrev;

            while(insBefore != L7_NULLPTR
                  && insBefore->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum
                  && insBefore->object->vrrpCfgInfo.vrid < vrid)
            {
                insAfter    = insBefore;
                insBefore   = insBefore->next;
            }

            if(insAfter == L7_NULLPTR)
            {
                /* Insert at the beginning of the list */
                paramHead_g         = temp;
            }

            temp->next      = insBefore;
            temp->previous  = insAfter;
            if(insAfter != L7_NULLPTR)
                insAfter->next  = temp;
            if(insBefore != L7_NULLPTR)
                insBefore->previous = temp;

            /* Update `intfHead' if necessary, if `temp' inserted before `intfHead' */
            if(insBefore == intfHead)
                intfHead = temp;

            vrrpPkt->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_INIT;
            totalRouters_g++;
        }
    }
    return L7_SUCCESS;
}


/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t vrrpIntfDelete(L7_uint32 intIfNum)
{
  vrrpIntfInformation_t *pIntfInfo;
  L7_uint32 i;
  nimConfigID_t configId;
  L7_short16 intfTblIndex;

  if (vrrpIntfIsConfigurable(intIfNum, &pIntfInfo) == L7_TRUE)
  {
      if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
      {
          return L7_FAILURE;
      }

      intfTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
      if(intfTblIndex == -1)
      {
          return L7_FAILURE;
      }

      /* Remove Virtual Routers Operational Info. */
      vrrpIntfDeleteOperInfo(intIfNum);

      memset(&(vrrpIntfInfoTbl[intfTblIndex]),0,sizeof(vrrpIntfInformation_t));

    /* ksriniv: Run through the pVrrpMapCfgData->virtRouter[]
       and delete entry for each virtRouter  configured for
       this interface ????? */
    for(i = 0; i < L7_VRRP_MAX_VIRT_ROUTERS; i++)
    {
        if(NIM_CONFIG_ID_IS_EQUAL(&pVrrpMapCfgData->virtRouter[i].configId, &configId))
        {
          memset((void *)&pVrrpMapCfgData->virtRouter[i].configId, 0, sizeof(nimConfigID_t));
          vrrpBuildDefaultVrConfigData(i);
        }
    }

    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  /* If vrrpIntfIsConfigurable() fails, that's because we reached max VR limit,
   * we need not return L7_FAILURE */

  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Removes all Virtual Router's Operational Information
*           associated with 'intIfNum'.
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The operational information is maintained in a doublylinked list
*           whose head (left end) is paramHead_g and the list is ordered by
*           increasing intIfNum primarily and increasing VRId if the `intIfNum'
*           is the same.
*
* @end
*
*********************************************************************/
L7_RC_t vrrpIntfDeleteOperInfo(L7_uint32 intIfNum)
{
  vrrpList_t *listTail;
  L7_short16  intfInfoTblIndex = 0;
  L7_uchar8   vrid;

  osapiSemaTake (VrrpConfigSema, L7_WAIT_FOREVER);
  listTail = paramHead_g;
  while ( listTail!= L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum<intIfNum )
  {
    listTail = listTail->next;
  }

  while (listTail!=L7_NULLPTR && listTail->object->vrrpVirtRouterOperInfo.intIfNum == intIfNum )
  {

    if ( listTail->object->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_INIT)
    {
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }
    if ( listTail->previous == L7_NULLPTR)
    {
      paramHead_g = listTail->next;
    }
    else
    {
      listTail->previous->next = listTail->next;
    }
    if ( listTail->next!= L7_NULLPTR)
    {
      listTail->next->previous = listTail->previous;
    }

    vrid = listTail->object->vrrpCfgInfo.vrid;
    listTail->object->vrrpVirtRouterOperInfo.intIfNum = L7_NULL;
    listTail->object->vrrpCfgInfo.vrid = L7_NULL;

    (void)vrrpVRIdToIndexUnmap(vrid,intIfNum);

    intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);

    if (intfInfoTblIndex == -1)
    {
      osapiSemaGive(VrrpConfigSema);
      /* Invalid Interface Number */
      return L7_FAILURE;
    }

    (vrrpIntfInfoTbl[intfInfoTblIndex].totalConfigured)--;
    totalRouters_g--;
    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;

    listTail = listTail->next;
  }


    osapiSemaGive(VrrpConfigSema);



    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default VRRP config data for an intf
*
* @param    configId     configID associated with an interface
* @param    *pCfg        pointer to VRRP interface configuration structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpIntfBuildDefaultConfigData(L7_uint32 intIfNum, vrrpIntfInformation_t *pIntfInfo)
{
    L7_uint32 i;
    nimConfigID_t configId;

    memset(pIntfInfo, 0, sizeof(vrrpIntfInformation_t));

    pIntfInfo->intIfNum = intIfNum;
    pIntfInfo->totalConfigured = 0;

    if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    {
        return;
    }

    for (i=0;i<L7_VRRP_MAX_VIRT_ROUTERS;i++)
    {
        if (NIM_CONFIG_ID_IS_EQUAL(&pVrrpMapCfgData->virtRouter[i].configId, &configId))
        {
            NIM_CONFIG_ID_COPY(&(pVrrpMapCfgData->virtRouter[i].configId), &configId);
            vrrpBuildDefaultVrConfigData(i);
        }
    }

    pVrrpMapCfgData->cfgHdr.dataChanged = L7_TRUE;
}



/*********************************************************************
*
* @functions vrrpInfoShow
*
* @purpose   Displays a all config and nonconfig info for specific
*            vrid, interface combination
*
* @param     vrid        virtual router id
* @param     intf        interface number
*
* @returns   void
*
* @comments  none
*
* @end
*
*********************************************************************/
void vrrpInfoShow(L7_uint32 intIfNum, L7_uchar8 vrid)
{
  vrrpList_t* vrrpInfo = NULL;
  L7_short16 intfInfoTblIndex = vrrpIntfInfoTblIndexGet(intIfNum);
  L7_uint32 vridTemp = vrid;
  L7_uint32 i;
  L7_char8 stat[80];
  L7_char8 buf[80];
  nimConfigID_t  configId;
  L7_uchar8           arrayIdxForVRId;
  L7_uint32  ipAddrCount;

  if ( intfInfoTblIndex < 0 || vridTemp < L7_ROUTING_VRRP_MIN_VRID || vridTemp > L7_ROUTING_VRRP_MAX_VRID )
  {
    /* Maximum capacity reach */
    return ;
  }

  if(vrrpIntfIsConfigurable(intIfNum, L7_NULLPTR) == L7_FALSE)
  {
      return;
  }

  /* TBD: KAVLEEN, in save/restore test, ensure that this works appropriately */
#if 0 /* smanders stacking */
  if (nimGetUnitSlotPort(intIfNum, &(vrrpInfo->object->vrrpCfgInfo.usp) )  != L7_SUCCESS)
#else
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
#endif
  {
    /* Interface does not exist */
    return ;
  }

  arrayIdxForVRId = vrrpVRIdToIndexGet(vrid, intIfNum);
  if(arrayIdxForVRId == 0)
      return;

  /* Find the list element from the linklist pool of memory*/
  vrrpInfo = &(listMemPool_g[intfInfoTblIndex][arrayIdxForVRId-1]);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|              VRRP NON-CONFIGURATION DATA               |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+\n\n");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nintIfNum .............................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.intIfNum);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nnum_time_become_master................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.num_time_become_master);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nadvert_received.......................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.advert_received);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_advert_interval_error ............%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_advert_interval_error);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_auth_failure......................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_auth_failure);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_ttl_incorrect.....................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_ttl_incorrect);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_zero_priority................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_zero_priority);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_sent_zero_priority................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_sent_zero_priority);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_invalid_type.................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_invalid_ip_address_list......%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_ip_address_list);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_invalid_auth_type............%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_auth_type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_mismatched_auth_type.........%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_mismatched_auth_type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_invalid_length...............%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_length);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_rcvd_wrong_subnet.................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_rcvd_wrong_subnet);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npkt_addr_count........................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.pkt_addr_count);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipAddressCount........................%d", (L7_uint32)vrrpInfo->object->vrrpVirtRouterOperInfo.ipAddressCount);         /* L7_uchar8   */
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvmac[L7_MAC_ADDR_LEN].................");
  for (i=0; i<L7_MAC_ADDR_LEN;i++)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "%X", vrrpInfo->object->vrrpVirtRouterOperInfo.vmac[i]);  /* L7_uchar8   */
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nmaster_down_int.......................%d", (L7_uint32)vrrpInfo->object->vrrpVirtRouterOperInfo.master_down_int);        /* L7_short16  */

  switch (vrrpInfo->object->vrrpVirtRouterOperInfo.vr_state)
  {
  case L7_VRRP_STATE_INIT:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_state..............................STATE_INIT");
    break;
  case L7_VRRP_STATE_MASTER:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_state..............................STATE_MAST");
    break;
  case L7_VRRP_STATE_BACKUP:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_state..............................STATE_BACKUP");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_state..............................err");
    break;
  }

  switch (vrrpInfo->object->vrrpVirtRouterOperInfo.protocol_type)
  {
  case L7_VRRP_IP:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nprotocol_type.........................L7_IP");
    break;
  case L7_VRRP_BRIDGE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nprotocol_type.........................BRIDGE");
    break;
  case L7_VRRP_DECNET:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nprotocol_type.........................DECNET");
    break;
  case L7_VRRP_OTHER:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nprotocol_type.........................OTHER");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nprotocol_type.........................err");
    break;
  }

  switch (vrrpInfo->object->vrrpVirtRouterOperInfo.oper_rowstatus)
  {
  case L7_VRRP_ACTIVE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................ACTIVE");
    break;
  case L7_VRRP_NOTINSERVICE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................NOTINSERVICE");
    break;
  case L7_VRRP_NOTREADY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................NOTREADY");
    break;
  case L7_VRRP_CREATEANDGO:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................CREATEANDGO");
    break;
  case L7_VRRP_CREATEANDWAIT:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................CREATEANDWAIT");
    break;
  case L7_VRRP_DESTROY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................DESTROY");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noper_rowstatus........................err");
    break;
  }

  switch (vrrpInfo->object->vrrpVirtRouterOperInfo.assoc_ip_rowstatus)
  {
  case L7_VRRP_ACTIVE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................ACTIVE");
    break;
  case L7_VRRP_NOTINSERVICE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................NOTINSERVICE");
    break;
  case L7_VRRP_NOTREADY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................NOTREADY");
    break;
  case L7_VRRP_CREATEANDGO:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................CREATEANDGO");
    break;
  case L7_VRRP_CREATEANDWAIT:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................CREATEANDWAIT");
    break;
  case L7_VRRP_DESTROY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................DESTROY");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nassoc_ip_rowstatus....................ERROR");
    break;
  }

  if (vrrpInfo->object->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag == L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nzerp_priority_pkt_rcvd_flag...........TRUE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nzero_priority_pkt_rcvd_flag...........FALSE");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nlast_packet_recvtime (seconds)........%u", vrrpInfo->object->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nlast_packet_recvtime (nanoseconds)....%u", vrrpInfo->object->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvrrp_uptime (seconds).................%u", vrrpInfo->object->vrrpVirtRouterOperInfo.vrrp_uptime.seconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvrrp_uptime (nanoseconds).............%u", vrrpInfo->object->vrrpVirtRouterOperInfo.vrrp_uptime.nanoseconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nlastAdvt_Sendtime (seconds)...........%u", vrrpInfo->object->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.seconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nlastAdvt_Sendtime (nanoseconds).......%u", vrrpInfo->object->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.nanoseconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nmaster IP addr........................0x%X", vrrpInfo->object->vrrpVirtRouterOperInfo.masterIpAddr);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noperational priority........................%d", vrrpInfo->object->vrrpVirtRouterOperInfo.oper_priority);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\noperational timer interval........................%d", vrrpInfo->object->vrrpVirtRouterOperInfo.master_advt_int);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt delay configuration........................%d", vrrpInfo->object->vrrpVirtRouterOperInfo.preempt_delay_config);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt_startTime (seconds)........%u", vrrpInfo->object->vrrpVirtRouterOperInfo.preempt_startTime.seconds);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt_startTime (nanoseconds)....%u", vrrpInfo->object->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds);


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n|                 VRRP CONFIGURATION DATA                |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvrid..................................%d", (L7_uint32)vrrpInfo->object->vrrpCfgInfo.vrid);                  /* Virtual Router ID           */
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ndescription..................................%s", (L7_uint32)vrrpInfo->object->vrrpCfgInfo.description);           /* Virtual Router ID  description   */
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npriority..............................%d", (L7_uint32)vrrpInfo->object->vrrpCfgInfo.priority);              /* Priority                    */
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nadver_int.............................%d", (L7_uint32)vrrpInfo->object->vrrpCfgInfo.adver_int);             /* Advertisement Interval      */
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nauth_data.............................");
  for (i=0;i<L7_VRRP_MAX_AUTH_DATA;i++)
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "%c", vrrpInfo->object->vrrpCfgInfo.auth_data[i]);

  if (vrrpInfo->object->vrrpCfgInfo.preempt_mode == L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt_mode..........................TRUE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt_mode..........................FALSE");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\npreempt_delay.............................%d", (L7_uint32)vrrpInfo->object->vrrpCfgInfo.preempt_delay);             /* Preemption  Delay    */
  if (vrrpInfo->object->vrrpCfgInfo.timer_learn == L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ntimer_learn...........................TRUE");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\ntimer_learn...........................FALSE");
  }

  switch (vrrpInfo->object->vrrpCfgInfo.auth_types)
  {
  case L7_AUTH_TYPE_NONE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nauth_types............................L7_AUTH_TYPE_NONE");
    break;
  case L7_AUTH_TYPE_SIMPLE_PASSWORD:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nauth_types............................L7_AUTH_TYPE_SIMPLE_PASSWORD");
    break;
  case L7_AUTH_TYPE_MD5:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nauth_types............................L7_AUTH_TYPE_MD5");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nauth_types............................err");
    break;
  }

  osapiInetNtoa(vrrpInfo->object->vrrpCfgInfo.ipaddress[0], buf);
  sprintf(stat,"%-17s", buf);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nipaddress.............................%s", stat);  /* L7_IP_ADDR_t   */

  for ( ipAddrCount = 1; ipAddrCount < L7_L3_NUM_IP_ADDRS ; ipAddrCount++)
  {
    if (vrrpInfo->object->vrrpCfgInfo.ipaddress[ipAddrCount] != FD_VRRP_DEFAULT_IP_ADDRESS)
    {
      osapiInetNtoa(vrrpInfo->object->vrrpCfgInfo.ipaddress[ipAddrCount], buf);
      sprintf(stat,"%-17s", buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n         .............................%s", stat);  /* L7_IP_ADDR_t   */
    }
  }

  switch (vrrpInfo->object->vrrpCfgInfo.vr_start_state)
  {
  case L7_VRRP_UP:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_start_state........................UP");
    break;
  case L7_VRRP_DOWN:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_start_state........................DOWN");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nvr_start_state........................err\n\n");
    break;
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nTracking interfaces   state decrement priority");
  for ( ipAddrCount = 0; ipAddrCount < L7_VRRP_MAX_TRACK_INTF_PER_VR ; ipAddrCount++)
  {
    if (vrrpInfo->object->vrrpCfgInfo.vr_track_intf[ipAddrCount].intIfNum != 0)
    {

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n %-16.5d %-6.5d %-6.5d ",vrrpInfo->object->vrrpCfgInfo.vr_track_intf[ipAddrCount].intIfNum, vrrpInfo->object->vrrpCfgInfo.vr_track_intf[ipAddrCount].stateUp, vrrpInfo->object->vrrpCfgInfo.vr_track_intf[ipAddrCount].prio_dec );

    }
  }


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nTracking Routes   state decrement priority");
  for ( ipAddrCount = 0; ipAddrCount < L7_VRRP_MAX_TRACK_RT_PER_VR ; ipAddrCount++)
  {
    if (vrrpInfo->object->vrrpCfgInfo.vr_track_route[ipAddrCount].inUse)
    {
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(vrrpInfo->object->vrrpCfgInfo.vr_track_route[ipAddrCount].ipAddr, ipAddrStr);
      osapiInetNtoa(vrrpInfo->object->vrrpCfgInfo.vr_track_route[ipAddrCount].subnetMask, maskStr);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,  "\n %s/%s,  %s,  %u ",
                    ipAddrStr, maskStr,
                    vrrpInfo->object->vrrpCfgInfo.vr_track_route[ipAddrCount].reachable ? "TRUE" : "FALSE",
                    vrrpInfo->object->vrrpCfgInfo.vr_track_route[ipAddrCount].prio_dec);
    }
  }
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/

#include "sysapi.h"

static void vrrpCfgDataShow();

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
void vrrpBuildTestConfigData()
{

  L7_uint32 nudge;
  L7_uint32 i, j;
  nimConfigID_t dummyConfigId;


  /*---------------------------*/
  /* build non-default config data */
  /*---------------------------*/

  /* generic VRRP cfg */
  pVrrpMapCfgData->rtr.vrrpAdminMode = L7_ENABLE;

  dummyConfigId.type = L7_LOGICAL_VLAN_INTF;

  /* VRRP router interface configuration parameters */
  for (i = 0; i < L7_VRRP_MAX_VIRT_ROUTERS; i++)
  {
    nudge = i + 1;
    dummyConfigId.configSpecifier.vlanId         = nudge;
    pVrrpMapCfgData->virtRouter[i].configId      = dummyConfigId;
    pVrrpMapCfgData->virtRouter[i].vrid          = FD_VRRP_DEFAULT_VRID + nudge;
    pVrrpMapCfgData->virtRouter[i].priority      = FD_VRRP_DEFAULT_PRIORITY + nudge;
    pVrrpMapCfgData->virtRouter[i].preempt_mode  = L7_FALSE;
    pVrrpMapCfgData->virtRouter[i].adver_int     = FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL + nudge;
    pVrrpMapCfgData->virtRouter[i].auth_types    = (nudge % 2 ? L7_AUTH_TYPE_SIMPLE_PASSWORD : L7_AUTH_TYPE_MD5);
    pVrrpMapCfgData->virtRouter[i].vr_start_state= L7_VRRP_UP;

    for (j = 0; j < L7_L3_NUM_IP_ADDRS ; j++)
    {
      pVrrpMapCfgData->virtRouter[i].ipaddress[j]  = FD_VRRP_DEFAULT_IP_ADDRESS + j + 1;
    }
    for (j = 0; j < L7_VRRP_MAX_TRACK_INTF_PER_VR; j++)
    {
      pVrrpMapCfgData->virtRouter[i].vr_track_intf[j].intIfNum = 0;
      pVrrpMapCfgData->virtRouter[i].vr_track_intf[j].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
      pVrrpMapCfgData->virtRouter[i].vr_track_intf[j].stateUp = L7_FALSE;
    }
    for (j = 0; j < L7_VRRP_MAX_TRACK_RT_PER_VR; j++)
    {
      pVrrpMapCfgData->virtRouter[i].vr_track_route[j].ipAddr = 0;
      pVrrpMapCfgData->virtRouter[i].vr_track_route[j].subnetMask = 0;
      pVrrpMapCfgData->virtRouter[i].vr_track_route[j].prio_dec = FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT;
      pVrrpMapCfgData->virtRouter[i].vr_track_route[j].reachable = L7_FALSE;
      pVrrpMapCfgData->virtRouter[i].vr_track_route[j].inUse = L7_FALSE;
    }

    for (j = 0; j < L7_VRRP_MAX_AUTH_DATA; j++)
    {
      pVrrpMapCfgData->virtRouter[i].auth_data[j]   = FD_VRRP_DEFAULT_AUTH_DATA_KEY_CHAR + j + 1;
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
void vrrpConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(pVrrpMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*--------------------------------*/
   /* cfgParms                       */
   /*--------------------------------*/

    vrrpCfgDataShow();

    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    sysapiPrintf( "\n\n");
    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_VRRP_MAX_VIRT_ROUTERS - %d\n", L7_VRRP_MAX_VIRT_ROUTERS);
    sysapiPrintf( "L7_VRRP_MAX_AUTH_DATA    - %d\n", L7_VRRP_MAX_AUTH_DATA);
    sysapiPrintf( "L7_L3_NUM_IP_ADDRS       - %d\n", L7_L3_NUM_IP_ADDRS);


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("\npVrrpMapCfgData->checkSum : %u\n", pVrrpMapCfgData->checkSum);


}

static void vrrpCfgDataShow()
{
  L7_uint32 i, j;
  L7_char8 buf[80];
  L7_char8 stat[80];

  sysapiPrintf("\n+--------------------------------------------------------+");
  sysapiPrintf("\n|                 VRRP CONFIGURATION DATA                |");
  sysapiPrintf("\n+--------------------------------------------------------+");

  sysapiPrintf("\nvrrpAdminMode.........................%d", (L7_uint32)pVrrpMapCfgData->rtr.vrrpAdminMode);                  /* admin mode */
  sysapiPrintf("\n");

  for (i = 0; i < L7_VRRP_MAX_VIRT_ROUTERS; i++)
  {
    sysapiPrintf("\n");
    sysapiPrintf("\nvrid..................................%d", (L7_uint32)pVrrpMapCfgData->virtRouter[i].vrid);                  /* Virtual Router ID           */
    sysapiPrintf("\nconfigId..............................%d %d",
                 (L7_uint32)pVrrpMapCfgData->virtRouter[i].configId.type,
                 (L7_uint32)pVrrpMapCfgData->virtRouter[i].configId.configSpecifier.vlanId);
    sysapiPrintf("\npriority..............................%d", (L7_uint32)pVrrpMapCfgData->virtRouter[i].priority);              /* Priority                    */
    sysapiPrintf("\nadver_int.............................%d", (L7_uint32)pVrrpMapCfgData->virtRouter[i].adver_int);             /* Advertisement Interval      */
    sysapiPrintf("\nauth_data.............................");

    for (j = 0; j < L7_VRRP_MAX_AUTH_DATA; j++)
      sysapiPrintf("%x", pVrrpMapCfgData->virtRouter[i].auth_data[j]);

    if (pVrrpMapCfgData->virtRouter[i].preempt_mode == L7_TRUE)
      sysapiPrintf("\npreempt_mode..........................TRUE");
    else
      sysapiPrintf("\npreempt_mode..........................FALSE");

    switch (pVrrpMapCfgData->virtRouter[i].auth_types)
    {
    case L7_AUTH_TYPE_NONE:
      sysapiPrintf("\nauth_types............................L7_AUTH_TYPE_NONE");
      break;
    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
      sysapiPrintf("\nauth_types............................L7_AUTH_TYPE_SIMPLE_PASSWORD");
      break;
    case L7_AUTH_TYPE_MD5:
      sysapiPrintf("\nauth_types............................L7_AUTH_TYPE_MD5");
      break;
    default:
      sysapiPrintf("\nauth_types............................err");
      break;
    }

    osapiInetNtoa(pVrrpMapCfgData->virtRouter[i].ipaddress[0], buf);
    sprintf(stat,"%-17s", buf);

    sysapiPrintf("\nipaddress.............................%s", stat);  /* L7_IP_ADDR_t  */

    for (j = 1; j < L7_L3_NUM_IP_ADDRS ; j++)
    {
      if (pVrrpMapCfgData->virtRouter[i].ipaddress[j] != FD_VRRP_DEFAULT_IP_ADDRESS)
      {
        osapiInetNtoa(pVrrpMapCfgData->virtRouter[i].ipaddress[j], buf);
        sprintf(stat,"%-17s", buf);
        sysapiPrintf("\n         .............................%s", stat);  /* L7_IP_ADDR_t  */
      }
    }

    switch (pVrrpMapCfgData->virtRouter[i].vr_start_state)
    {
    case L7_VRRP_UP:
      sysapiPrintf("\nvr_start_state........................UP");
      break;
    case L7_VRRP_DOWN:
      sysapiPrintf("\nvr_start_state........................DOWN");
      break;
    default:
      sysapiPrintf("\nvr_start_state........................err\n\n");
      break;
    }
  }
}

/*********************************************************************
* @purpose  Save VRRP test config data file to NVStore
*
* @param    void
*
*
* @notes    none
*
* @end
*********************************************************************/
void vrrpSaveTestConfigData(void)
{
  pVrrpMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  pVrrpMapCfgData->checkSum =
  nvStoreCrc32((L7_char8 *)pVrrpMapCfgData,
               sizeof(L7_vrrpMapCfg_t) - sizeof(pVrrpMapCfgData->checkSum));


  if (sysapiCfgFileWrite(L7_VRRP_MAP_COMPONENT_ID, L7_VRRP_CFG_FILENAME,
                         (L7_char8 *)pVrrpMapCfgData, sizeof (L7_vrrpMapCfg_t)) == L7_ERROR)
  {
    sysapiPrintf("vrrpSave: Error during osapiFsWrite for config file %s\n",
            L7_VRRP_CFG_FILENAME);
    return;
  }

  sysapiPrintf("\nSaved VRRP test config data to NVStore\n\n");
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/********************** VRRP DEBUG TRACES *************************************/

/*********************************************************************
 * @purpose  Saves vrrp configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    vrrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t vrrpDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  vrrpDebugCfgUpdate();

  if (vrrpDebugHasDataChanged() == L7_TRUE)
  {
    vrrpDebugCfg.hdr.dataChanged = L7_FALSE;
    vrrpDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&vrrpDebugCfg,
        (L7_uint32)(sizeof(vrrpDebugCfg) - sizeof(vrrpDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_VRRP_MAP_COMPONENT_ID, VRRP_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&vrrpDebugCfg, (L7_uint32)sizeof(vrrpDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",VRRP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores vrrp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    vrrpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t vrrpDebugRestore(void)
{
  L7_RC_t rc;

  vrrpDebugBuildDefaultConfigData(VRRP_DEBUG_CFG_VER_CURRENT);

  vrrpDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = vrrpApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if vrrp debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL vrrpDebugHasDataChanged(void)
{
  return vrrpDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default vrrp config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void vrrpDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  vrrpDebugCfg.hdr.version = ver;
  vrrpDebugCfg.hdr.componentID = L7_VRRP_MAP_COMPONENT_ID;
  vrrpDebugCfg.hdr.type = L7_CFG_DATA;
  vrrpDebugCfg.hdr.length = (L7_uint32)sizeof(vrrpDebugCfg);
  strcpy((L7_char8 *)vrrpDebugCfg.hdr.filename, VRRP_DEBUG_CFG_FILENAME);
  vrrpDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&vrrpDebugCfg.cfg, 0, sizeof(vrrpDebugCfg.cfg));
}

/*********************************************************************
 * @purpose  Apply vrrp debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t vrrpApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc =vrrpDebugPacketTraceFlagSet(vrrpDebugCfg.cfg.vrrpDebugPacketTraceFlag);

  return rc;
}








