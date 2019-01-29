/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  voice_vlan_db.c
*
* @purpose   voice vlan data store file
*
* @component voice_vlan
*
* @comments  none
*
* @create    05/21/2008
*
* @author    SNK
*
* @end
*
**********************************************************************/

#include "voice_vlan.h"
#include "avl_api.h"


/* Global parameters */
static avlTree_t                voiceVlanDeviceInfoTreeDb;
static avlTreeTables_t          *voiceVlanDeviceInfoTreeHeap=L7_NULLPTR;
static voiceVlanDeviceInfo_t    *voiceVlanDeviceInfoDataHeap=L7_NULLPTR;


/*********************************************************************
* @purpose  Initialize voice vlan Device Info Database
*
* @param    nodeCount    @b{(input)} The number of nodes to be created.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDBInit(L7_uint32 nodeCount)
{
  /* Allocate the Heap structures */
  voiceVlanDeviceInfoTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_VOICE_VLAN_COMPONENT_ID,
                                                            nodeCount * sizeof(avlTreeTables_t));

  voiceVlanDeviceInfoDataHeap  = (voiceVlanDeviceInfo_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                                                    nodeCount * sizeof(voiceVlanDeviceInfo_t));

  /* validate the pointers*/
  if ((voiceVlanDeviceInfoDataHeap == L7_NULLPTR) ||(voiceVlanDeviceInfoDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "voiceVlanDeviceInfoDBInit: Error allocating data for dot1x DB Tree \n");
    return L7_FAILURE;
  }

  /* AVL Tree creations - voiceVlanDeviceInfoTreeDb*/
  avlCreateAvlTree(&(voiceVlanDeviceInfoTreeDb),  voiceVlanDeviceInfoTreeHeap,
                   voiceVlanDeviceInfoDataHeap, nodeCount,
                   sizeof(voiceVlanDeviceInfo_t), 0x10,
                   sizeof(L7_uchar8) * L7_MAC_ADDR_LEN );
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  DeInitialize voice vlan device Info Database
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDBDeInit(void)
{
  /* Destroy the AVL Tree */
  if(voiceVlanDeviceInfoTreeDb.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&voiceVlanDeviceInfoTreeDb);
  }

  /* Give up the memory */
  if (voiceVlanDeviceInfoTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanDeviceInfoTreeHeap);
    voiceVlanDeviceInfoTreeHeap = L7_NULLPTR;
  }

  if (voiceVlanDeviceInfoDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_VOICE_VLAN_COMPONENT_ID, voiceVlanDeviceInfoDataHeap);
    voiceVlanDeviceInfoDataHeap = L7_NULLPTR;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Take lock for the voice vlan device Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in
*           the dot1x threads context.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoTakeLock(void)
{
  return osapiSemaTake(voiceVlanDeviceInfoTreeDb.semId, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  To Giveup lock for the voice vlan device  Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in
*           the dot1x threads context.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoGiveLock(void)
{
  return osapiSemaGive(voiceVlanDeviceInfoTreeDb.semId);
}

/*********************************************************************
* @purpose  To allocate a voice vlan Device Info Node
*
* @param    mac_addr        @b{(input)} Voip device mac address
* @param    vlanId          @b{(input)} Voice Vlan Id associated with this mac address
* @param    intIfNum        @b{(input)} The internal interface on which this
*                                   voip device was identified
* @param    clientAuthState @b{(input)} indicates if this device is an unauthenticated device
* @param    sourceType      @b{(input)} sourceType of this device (LLDP, etc.)
*
* @returns  L7_SUCCESS    if the entry was created sucessfully
*           L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoAlloc(L7_uchar8                 *mac_addr,
                                 L7_uint32                  vlanId,
                                 L7_uint32                  intIfNum,
                                 L7_BOOL                    clientAuthState,
                                 VOICE_VLAN_SOURCE_TYPES_t  sourceType)
{
  L7_uchar8   zeroMac[L7_MAC_ADDR_LEN];
  voiceVlanDeviceInfo_t  newNode,*tmpNode=L7_NULLPTR,*retNode = L7_NULLPTR;


   /* validate input */
   memset (zeroMac ,0,L7_MAC_ADDR_LEN);

   if ((memcmp(zeroMac,mac_addr,L7_MAC_ADDR_LEN)==0)||
        (( clientAuthState!= L7_TRUE)&&(clientAuthState != L7_FALSE)))
   {
      return L7_FAILURE;
   }

   tmpNode = voiceVlanDeviceInfoGet(mac_addr);

    if (tmpNode == L7_NULLPTR)
    {
      /* found one - use it */
      memset(&newNode,0,sizeof(voiceVlanDeviceInfo_t));
      memcpy(newNode.voipMac,mac_addr,L7_MAC_ADDR_LEN);
      newNode.vlanId = vlanId;
      newNode.intIfNum = intIfNum;
      newNode.clientAuthState = clientAuthState;
      newNode.sourceType = sourceType;

      /* add the node to the tree */
      osapiSemaTake(voiceVlanDeviceInfoTreeDb.semId, L7_WAIT_FOREVER);
      retNode  = avlInsertEntry(&voiceVlanDeviceInfoTreeDb,&newNode);
      osapiSemaGive(voiceVlanDeviceInfoTreeDb.semId);
      if (retNode == &newNode)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
                "%s:%d Error Adding the node to the Voice Vlan Device Info Tree for mac address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ,phyintf %d, %s, voice vlan : %d \n",
                __FUNCTION__,__FILE__,
                mac_addr[0],
                mac_addr[1],
                mac_addr[2],
                mac_addr[3],
                mac_addr[4],
                mac_addr[5],
                intIfNum, ifName,
                vlanId);
        return L7_FAILURE;
      }
      return L7_SUCCESS;
    }
    else
    {
      /* mac address already present in the database */
      return L7_FAILURE;
    }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Deallocate a voice Vlan Device Info Node
*
* @param    node  pointer to voiceVlanDeviceInfo structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDeviceInfoDeAlloc(voiceVlanDeviceInfo_t *node)
{
  if(node != L7_NULLPTR)
  {
     osapiSemaTake(voiceVlanDeviceInfoTreeDb.semId, L7_WAIT_FOREVER);
     avlDeleteEntry(&voiceVlanDeviceInfoTreeDb,node);
     osapiSemaGive(voiceVlanDeviceInfoTreeDb.semId);
     return L7_SUCCESS;

  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get a voice vlan Device Info Node
*
* @param    mac_addr  @b{(input)} The mac address of the voip device
*
* @returns  voice vlan device info node
*
* @comments none
*
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGet(L7_uchar8* mac_addr_key)
{
  L7_uchar8 zeroMac[L7_MAC_ADDR_LEN],key[L7_MAC_ADDR_LEN];
  voiceVlanDeviceInfo_t  *entry = L7_NULLPTR;

  /*validate mac address*/
  memset(zeroMac,0,L7_MAC_ADDR_LEN);
  if (memcmp(mac_addr_key,zeroMac,L7_MAC_ADDR_LEN)==0)
  {
    return  entry;
  }

  /* copy the mac address to the key*/
  memset(key,0,L7_MAC_ADDR_LEN);
  memcpy(key,mac_addr_key,L7_MAC_ADDR_LEN);
  entry = (voiceVlanDeviceInfo_t *)avlSearchLVL7(&voiceVlanDeviceInfoTreeDb,key,AVL_EXACT);
  return entry;
}

/*********************************************************************
* @purpose  To Get Next voice vlan Device Info Node
*
* @param    mac_addr_key  @b{(input)} The mac address of the voice vlan device
*
* @returns  Logical Internal Interface node
*
* @comments none
*
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGetNext(L7_uchar8 *mac_addr_key)
{
  L7_uchar8 key[L7_MAC_ADDR_LEN];
  voiceVlanDeviceInfo_t  *entry = L7_NULLPTR;

   /* copy the mac address to the key*/
  memset(key,0,L7_MAC_ADDR_LEN);
  memcpy(key,mac_addr_key,L7_MAC_ADDR_LEN);
  entry = (voiceVlanDeviceInfo_t *)avlSearchLVL7(&voiceVlanDeviceInfoTreeDb,key,AVL_NEXT);
  return entry;
}



/*********************************************************************
* @purpose  To get First voice vlan device  for a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface
*
* @returns  Voice Vlan Device node
*
* @comments
*
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoFirstGet(L7_uint32 intIfNum)
{
  L7_uchar8 mac_key[L7_MAC_ADDR_LEN];
  voiceVlanDeviceInfo_t *node;

  memset(mac_key,0,L7_MAC_ADDR_LEN);

  voiceVlanDeviceInfoTakeLock();
  node=voiceVlanDeviceInfoGetNext(mac_key);
  while(node!=L7_NULLPTR)
  {
    if (node->intIfNum == intIfNum)
    {
      voiceVlanDeviceInfoGiveLock();
      return node;
    }
    memset(mac_key,0,L7_MAC_ADDR_LEN);
    memcpy(mac_key,node->voipMac,L7_MAC_ADDR_LEN);
    node=voiceVlanDeviceInfoGetNext(mac_key);
  }

  voiceVlanDeviceInfoGiveLock();
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  To iterate all the voice vlan devices of a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface
* @param    mac_addr  @b{(input/output)} The mac address of the last learnt voip device
*
* @returns  voice vlan device node
*
* @comments Give the last learnt mac address on that interface to get the next one
*
* @end
*********************************************************************/
voiceVlanDeviceInfo_t *voiceVlanDeviceInfoGetNextNode(L7_uint32 intIfNum,
                                                        L7_uchar8 *mac_addr)
{
  voiceVlanDeviceInfo_t *node;

   voiceVlanDeviceInfoTakeLock();
   node=voiceVlanDeviceInfoGetNext(mac_addr);
   while(node!=L7_NULLPTR)
  {
    if (node->intIfNum == intIfNum &&
        memcmp(mac_addr,node->voipMac,L7_MAC_ADDR_LEN)!=0)
    {
      voiceVlanDeviceInfoGiveLock();
      return node;
    }
    memset(mac_addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr,node->voipMac,L7_MAC_ADDR_LEN);
    node=voiceVlanDeviceInfoGetNext(mac_addr);
  }

  voiceVlanDeviceInfoGiveLock();
  return L7_NULLPTR;
}


/*********************************************************************
* @purpose  Debug Info of the voice vlan DB
*
* @param    None
*
* @returns  None
*
* @comments none
*
* @end
*********************************************************************/
void voiceVlanDebugDeviceInfo(void)
{
  if(voiceVlanDeviceInfoTreeHeap != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "The Voice Vlan Device Database has %d Nodes\n",
    voiceVlanDeviceInfoTreeDb.count);
  }
}

/*********************************************************************
* @purpose  Debug Info of the voice vlan DB
*
* @param    None
*
* @returns  None
*
* @comments none
*
* @end
*********************************************************************/
void voiceVlanDebugDeviceInfoList(void)
{
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uchar8 mac_addr[L7_MAC_ADDR_LEN];

  /*list device info in the tree*/
  memset(mac_addr,0,L7_MAC_ADDR_LEN);

  node=voiceVlanDeviceInfoGetNext(mac_addr);
  while(node != L7_NULLPTR)
  {
    sysapiPrintf("\r\n Voip Mac address:%02X:%02X:%02X:%02X:%02X:%02X",
                 node->voipMac[0],
                 node->voipMac[1],
                 node->voipMac[2],
                 node->voipMac[3],
                 node->voipMac[4],
                 node->voipMac[5]);
    sysapiPrintf("\n Interface: %d",node->intIfNum);
    sysapiPrintf("\r\n Voice Vlan:%d",node->vlanId );
    sysapiPrintf("\r\n Client Auth State:%d ",node->clientAuthState);
    sysapiPrintf("\r\n ---------------------------------------------------------------------");

    memset(mac_addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr,node->voipMac,L7_MAC_ADDR_LEN);

     /*checking the get command*/
    node = voiceVlanDeviceInfoGet(mac_addr);
    if (node != L7_NULLPTR)
    {
       sysapiPrintf("\r\n Voip Mac address:%02X:%02X:%02X:%02X:%02X:%02X",
                   node->voipMac[0],
                   node->voipMac[1],
                   node->voipMac[2],
                   node->voipMac[3],
                   node->voipMac[4],
                   node->voipMac[5]);
      sysapiPrintf("\n Interface: %d",node->intIfNum);
      sysapiPrintf("\r\n Voice Vlan:%d",node->vlanId );
      sysapiPrintf("\r\n Client Auth State:%d ",node->clientAuthState);
      sysapiPrintf("\r\n ---------------------------------------------------------------------");
    }

    node=voiceVlanDeviceInfoGetNext(mac_addr);
  }
}

/*********************************************************************
* @purpose  Debug Info of the voice vlan DB
*
* @param    None
*
* @returns  None
*
* @comments none
*
* @end
*********************************************************************/
void voiceVlanDebugDeviceInfoIntfList(L7_uint32 intIfNum)
{
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uchar8 mac_addr[L7_MAC_ADDR_LEN];

  /*list device info in the tree*/
  memset(mac_addr,0,L7_MAC_ADDR_LEN);

  node= voiceVlanDeviceInfoFirstGet(intIfNum);
  while(node != L7_NULLPTR)
  {
    sysapiPrintf("\r\n Voip Mac address:%02X:%02X:%02X:%02X:%02X:%02X",
                 node->voipMac[0],
                 node->voipMac[1],
                 node->voipMac[2],
                 node->voipMac[3],
                 node->voipMac[4],
                 node->voipMac[5]);
    sysapiPrintf("\n Interface: %d",node->intIfNum);
    sysapiPrintf("\r\n Voice Vlan:%d",node->vlanId );
    sysapiPrintf("\r\n Client Auth State:%d ",node->clientAuthState);
    sysapiPrintf("\r\n ---------------------------------------------------------------------");
    memset(mac_addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr,node->voipMac,L7_MAC_ADDR_LEN);
    node=voiceVlanDeviceInfoGetNextNode(intIfNum,mac_addr);
  }

}
