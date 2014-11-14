/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_db.c
*
* @purpose    Contains definitions to snooping database handling
*             rotuines
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "comm_structs.h"
#include "fdb_api.h"
#include "mfdb_api.h"

#include "snooping_db.h"
#include "snooping_util.h"
#include "snooping_ctrl.h"
#include "snooping_defs.h"
#include "snooping_ptin_sourcetimer.h"

#ifdef L7_NSF_PACKAGE
  #include "snooping_ckpt.h"
#endif /* L7_NSF_PACKAGE */

/* PTin added: IGMP snooping */
#if 1
  #include "ptin_igmp.h"
  #include "ptin_intf.h"
  #include "ptin_debug.h"
  #include "logger.h"
  #include "usmdb_mfdb_api.h"

  #include "snooping_ptin_db.h"
  #include "l3_addrdefs.h"//MMelo
#endif

/*********************************************************************
* @purpose  Finds an Multicast group membership entry
*
* @param    macAddr  @b{(input)} Multicast MAC Address
* @param    vlanId   @b{(input)} Vlan ID for the MAC Address
* @param    flag     @b{(input)} Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
* @returns  null pointer on failure
* @returns  multicast entry pointer on success
*
* @notes    none
*
* @end
*********************************************************************/
snoopInfoData_t *snoopEntryFind(L7_uchar8 *macAddr, L7_uint32 vlanId,
                                L7_uchar8 family, L7_uint32 flag)
{
  snoopInfoData_t    *snoopEntry;
  snoopInfoDataKey_t  key;
  L7_ushort16         vid;
  L7_uint32           ivlLength = 0;
  L7_FDB_TYPE_t       fdbType;
  snoop_eb_t         *pSnoopEB;

  vid = (L7_ushort16)vlanId;
  pSnoopEB = snoopEBGet();
  memset((void *)&key, 0x00, sizeof(snoopInfoDataKey_t));

  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(&(key.vlanIdMacAddr[0]), &vid, ivlLength);
  }
  memcpy(&key.vlanIdMacAddr[ivlLength], macAddr, L7_FDB_MAC_ADDR_LEN);
  key.family = family;
  snoopEntry = avlSearchLVL7(&pSnoopEB->snoopAvlTree, &key, flag);
  while (snoopEntry)
  {
    if (snoopEntry->snoopInfoDataKey.family == family)
    {
      break;
    }
    memcpy(&key, &snoopEntry->snoopInfoDataKey, sizeof(snoopInfoDataKey_t));
    snoopEntry = avlSearchLVL7(&pSnoopEB->snoopAvlTree, &key, flag);
  }

  if (snoopEntry == L7_NULL)
  {
    return L7_NULLPTR;
  }
  else
  {
    return snoopEntry;
  }
}
/*********************************************************************
* @purpose  Adds a VID-Multicast MAC entry to snoop database
*
* @param    macAddr  @b{(input)} Multicast MAC Address
* @param    vlanId   @b{(input)} Vlan ID
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopEntryAdd(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticGroup)
{
  snoopInfoData_t    snoopEntry;
  snoopInfoData_t   *pData;
  L7_ushort16        vid;
  L7_uint32          ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t      fdbType;
  snoop_eb_t        *pSnoopEB;

  ptin_timer_start(19,"snoopEntryAdd-Inits");

  vid = (L7_ushort16)vlanId;

  memset(&snoopEntry, 0x00, sizeof(snoopInfoData_t));
  pSnoopEB = snoopEBGet();

  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(snoopEntry.snoopInfoDataKey.vlanIdMacAddr, &vid, ivlLength);
  }
  memcpy(&snoopEntry.snoopInfoDataKey.vlanIdMacAddr[ivlLength], macAddr,
         L7_FDB_MAC_ADDR_LEN);
  snoopEntry.snoopInfoDataKey.family = family;

  /* PTin added: IGMP snooping */
#if 1
  snoopEntry.staticGroup = staticGroup;
#endif
  ptin_timer_stop(19);

  ptin_timer_start(20,"snoopEntryAdd-SLLCreate");
  /* Create linked list for group membership timers for this snoop entry */
  if (SLLCreate(L7_SNOOPING_COMPONENT_ID, L7_SLL_NO_ORDER,
                sizeof(L7_uint32), snoopTimerDataCmp, snoopTimerDataDestroy,
                &(snoopEntry.ll_timerList)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "snoopEntryAdd: Failed to create timer linked list");
    return L7_FAILURE;
  }
  ptin_timer_stop(20);

  ptin_timer_start(21,"snoopEntryAdd-avlInsertEntry");
  pData = avlInsertEntry(&pSnoopEB->snoopAvlTree, &snoopEntry);
  ptin_timer_stop(21);

  if (pData == L7_NULL)
  {
    ptin_timer_start(22,"snoopEntryAdd-snoopEntryFind");
    /*entry was added into the avl tree*/
    /* Create a TimerCB for this entry */
    if ((pData = snoopEntryFind(macAddr, vlanId, family, AVL_EXACT))  == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: Failed to find recently added entry in snoopTree");
      return L7_FAILURE;
    }
    ptin_timer_stop(22);

    ptin_timer_start(23,"snoopEntryAdd-Compare1");
    freeIdx = pSnoopEB->snoopEntryTimerFreeIdx;
    if (freeIdx >= SNOOP_ENTRY_TIMER_BLOCKS_COUNT)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: Free Timer CB Index out of bounds %d", freeIdx);
      (void)snoopEntryDelete(macAddr, vlanId, family);
      return L7_FAILURE;
    }
    ptin_timer_stop(23);

    ptin_timer_start(24,"snoopEntryAdd-Compare2");
    pData->timerIdx = pSnoopEB->snoopEntryTimerFreeList[freeIdx];
    if (pData->timerIdx == -1)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: Free Timer CB could not be found");
      (void)snoopEntryDelete(macAddr, vlanId, family);
      return L7_FAILURE;
    }
    ptin_timer_stop(24);

    ptin_timer_start(25,"snoopEntryAdd-Compare3");
    pData->timerCB = pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].timerCB;
    pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].pSnoopEntry = pData;
    if (pData->timerCB == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: Timer CB could not be initialized for the snoopEntry");
      (void)snoopEntryDelete(macAddr, vlanId, family);
      return L7_FAILURE;
    }
    ptin_timer_stop(25);

    ptin_timer_start(26,"snoopEntryAdd-appTimerProcess");
    pSnoopEB->snoopEntryTimerFreeList[freeIdx] = -1;
    pSnoopEB->snoopEntryTimerFreeIdx++;
    appTimerProcess(pData->timerCB);/* Kick the timer as it might have slept by now */
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_ADD_DATA, macAddr, 
                               vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */

    /* PTin added: IGMP snooping */
#if 1
    memset(&pData->global,0x00,sizeof(pData->global));
    memset(pData->port_list,0x00,sizeof(pData->port_list));
    memset(pData->channel_list,0x00,sizeof(pData->channel_list));
#endif

    ptin_timer_stop(26);

    return L7_SUCCESS;
  }

  if (pData == &snoopEntry)
  {
    /*some error in avl tree addition*/
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: some error in avl tree addition");
    return L7_FAILURE;
  }

  /*entry already exists*/
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopEntryAdd: entry already exists");
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Removes a node entry from the registry
*
* @param    macAddr  @b{(input)} Multicast MAC Address
* @param    vlanId   @b{(input)} Vlan ID for the MAC Address
* @param    family   @b{(input)} L7_AF_INET  => IGMP Snooping
*                                L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopEntryDelete(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family)
{
  snoopInfoData_t  *pData;
  snoopInfoData_t  *snoopEntry;
  snoop_eb_t       *pSnoopEB;
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t    zeroMask;
#endif /* L7_MCAST_PACKAGE */
  L7_uint32         freeIdx;

  pSnoopEB = snoopEBGet();
  pData = snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  snoopEntry = pData;
#ifdef L7_MCAST_PACKAGE
  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);
#endif /* L7_MCAST_PACKAGE */
  if (SLLDestroy(L7_SNOOPING_COMPONENT_ID, &snoopEntry->ll_timerList)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoopEntryDelete: Failed to destroy the timer linked list");
  }

  freeIdx = pSnoopEB->snoopEntryTimerFreeIdx;
  if (freeIdx == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoopEntryDelete: Invalid snoopEntryTimerFreeIdx expecting > 0 has %d",
            pSnoopEB->snoopEntryTimerFreeIdx);
  }

  freeIdx = pSnoopEB->snoopEntryTimerFreeIdx - 1;
  pData->timerCB = L7_NULLPTR;
  pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].pSnoopEntry = L7_NULLPTR;
  if (pSnoopEB->snoopEntryTimerFreeList[freeIdx] != -1)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoopEntryDelete: Mismatch in snoopEntryTimerFreeList expecting -1 has %d",
            pSnoopEB->snoopEntryTimerFreeList[freeIdx]);
  }
  pSnoopEB->snoopEntryTimerFreeList[freeIdx] = pData->timerIdx;
  pData = avlDeleteEntry(&pSnoopEB->snoopAvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    pSnoopEB->snoopEntryTimerFreeIdx--;
    /* Entry deleted */

#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_DEL_DATA, macAddr, vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */

    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Creates a Multicast MAC Group entry
*
* @param    macAddr   @b{(input)}  IP Multicast MAC Address
* @param    vlanId    @b{(input)}  Vlan ID
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopEntryCreate(L7_uchar8* macAddr, L7_uint32 vlanId,
                         L7_uchar8 family, L7_BOOL staticGroup)
{
  mfdbMemberInfo_t  mfdb;
  L7_RC_t           rc;

  ptin_timer_start(10,"snoopEntryCreate-snoopMacAddrCheck");
  /* Check for illegal IP multicast MAC addresses*/
  rc = snoopMacAddrCheck(macAddr, family);
  if (rc == L7_FAILURE)
  {
    /* Illegal IP Multicast MAC address */
    return L7_FAILURE;
  }
  ptin_timer_stop(10);

  ptin_timer_start(11,"snoopEntryCreate-snoopEntryFind");
  /* Does an entry with the same mac addr and vlan id already exist? */
  if (snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT) != L7_NULLPTR)
  {
    /* Entry exists... don't add another one, return success */
    return L7_SUCCESS;
  }
  ptin_timer_stop(11);

  ptin_timer_start(12,"snoopEntryCreate-attribs");
  /* Add entry to the MFDB */
  memset((L7_uchar8 *)&mfdb, 0, sizeof(mfdb));
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId = vlanId;
  mfdb.user.componentId = (family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP
                          : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type = (!staticGroup) ? L7_MFDB_TYPE_DYNAMIC : L7_MFDB_TYPE_STATIC;
  strncpy(mfdb.user.description, L7_MFDB_NETWORK_ASSISTED,
          L7_MFDB_COMPONENT_DESCR_STRING_LEN);
  ptin_timer_stop(12);

  ptin_timer_start(13,"snoopEntryCreate-mfdbEntryExist");
  rc = mfdbEntryExist(macAddr, vlanId);
  ptin_timer_stop(13);
  if (rc == L7_NOT_EXIST)
  {
    ptin_timer_start(14,"snoopEntryCreate-mfdbEntryAdd");
    rc = mfdbEntryAdd(&mfdb);
    ptin_timer_stop(14);
    if (rc == L7_SUCCESS)
    {
      ptin_timer_start(15,"snoopEntryCreate-snoopEntryAdd");
      rc = snoopEntryAdd(macAddr, vlanId, family, staticGroup);
      ptin_timer_stop(15);
    }
  }
  else if (rc == L7_SUCCESS)
  {
    ptin_timer_start(15,"snoopEntryCreate-snoopEntryAdd");
    rc = snoopEntryAdd(macAddr, vlanId, family, staticGroup);
    ptin_timer_stop(15);
  }
  else
  {
    rc = L7_FAILURE;
  }
  return rc;
}
/*****************************************************************************
* @purpose  Adds an interface to the port list of the multicast address entry
*
* @param    macAddr   @b{(input)} Multicast MAC Address
* @param    vlanId    @b{(input)} Vlan ID for the MAC Address
* @param    intIfNum  @b{(input)} Internal Interface number
* @param    intfType  @b{(input)} Interface type
* @param    pSnoopCB  @b{(input)} Control Block
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    adds interface to group membership entry and starts
*           corresponding timer
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfAdd(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum,
                     snoop_interface_type_t intfType, snoop_cb_t *pSnoopCB)
{
  snoopInfoData_t  *snoopEntry;
  L7_uint32         timerValue = 0, flag;
  mfdbMemberInfo_t  mfdb;
  L7_RC_t           rc;
  snoop_eb_t       *pSnoopEB;
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t    snoopMemberList;
#endif /* L7_MCAST_PACKAGE */

  pSnoopEB = snoopEBGet();
  snoopEntry = snoopEntryFind(macAddr, vlanId, pSnoopCB->family, L7_MATCH_EXACT);
  if (snoopEntry == L7_NULLPTR)
  {
    /*entry not found*/
    return L7_FAILURE;
  }

  if (intfType == SNOOP_GROUP_MEMBERSHIP)
  {
    if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intIfNum))
    {
      /* PTin added: IGMP snooping */
#if 0
      if (!snoopEntry->staticGroup)
#endif
      {
        /*Already a group member interface. Update the group membership timer */
        timerValue = snoopCheckPrecedenceParamGet(vlanId, intIfNum,
                                                  SNOOP_PARAM_GROUP_MEMBERSHIP_INTERVAL,
                                                  pSnoopCB->family);
        snoopTimerUpdate(snoopEntry, intIfNum, vlanId, intfType, timerValue,
                         pSnoopCB);
      }
      return L7_SUCCESS;
    }
  }
  else if (intfType == SNOOP_STATIC_MCAST_ROUTER_ATTACHED ||
           intfType == SNOOP_MCAST_ROUTER_ATTACHED)
  {
    /* Already a mrtr interface */
    if (L7_INTF_ISMASKBITSET(snoopEntry->snoopMrtrMemberList, intIfNum))
    {
      return L7_SUCCESS;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoopIntfAdd: Invalid interface type");
    return L7_FAILURE;
  }
  /* Add interface to the MFDB */
  memset((L7_uchar8 *)&mfdb, 0x00, sizeof(mfdb));
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (pSnoopCB->family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP
                          : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (!snoopEntry->staticGroup) ? L7_MFDB_TYPE_DYNAMIC : L7_MFDB_TYPE_STATIC;  /* PTin modified: IGMP snooping */
  memcpy((void *)mfdb.user.description, (void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  /* Set the mask bit in the fwd mask for this interface */
  L7_INTF_SETMASKBIT(mfdb.user.fwdPorts, intIfNum);

  /* Check if this entry exists */
  if (mfdbEntryExist(macAddr, vlanId) != L7_SUCCESS)
  {
    rc = mfdbEntryAdd(&mfdb);
  }
  else
  {
    rc = mfdbEntryPortsAdd(&mfdb);
  }

  /* Update the corresponding entry only on successful MFDB execution */
  if (rc == L7_SUCCESS)
  {
    /* Start Group membership Timer */
    if (SNOOP_GROUP_MEMBERSHIP == intfType)
    {
      /* PTin added: IGMP snooping */
#if 0
      if (!snoopEntry->staticGroup)
#endif
      {
        if (snoopTimerStart(snoopEntry, intIfNum, vlanId,
                            intfType, pSnoopCB) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                  "snoopIntfAdd: Failed to start timer \n");
          /* Roll back the changes made */
          if (mfdbEntryPortsDelete(&mfdb) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                    "snoopIntfAdd: Failed to delete port from MFDB\n");
          }
          L7_INTF_NONZEROMASK(snoopEntry->snoopGrpMemberList, flag);

          if (flag == 0)
          {
            /* PTin added: IGMP snooping */
#if 1
            if (!snoopEntry->staticGroup)
#endif
            {
              if (snoopEntryRemove(macAddr, vlanId, pSnoopCB->family)
                  != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                        "snoopIntfAdd: Failed to remove snoop entry\n");
              }
            }
          }
          return L7_FAILURE;
        }
      }
      L7_INTF_SETMASKBIT(snoopEntry->snoopGrpMemberList, intIfNum);

#ifdef L7_NSF_PACKAGE
      if (pSnoopEB->snoopBackupElected == L7_TRUE)
      {
        snoopCheckpointCallback1(SNOOP_CKPT_MSG_PORT_ADD_DATA, macAddr, vlanId, pSnoopCB->family, intIfNum, L7_NULL, L7_NULL, L7_NULL);
      }
#endif /* L7_NSF_PACKAGE */

    }
    else
    {
      L7_INTF_SETMASKBIT(snoopEntry->snoopMrtrMemberList, intIfNum);

#ifdef L7_NSF_PACKAGE
      if (pSnoopEB->snoopBackupElected == L7_TRUE)
      {
        snoopCheckpointCallback1(SNOOP_CKPT_MSG_MRTR_ADD_DATA, macAddr, vlanId, pSnoopCB->family, intIfNum, L7_NULL, L7_NULL, L7_NULL);
      }
#endif /* L7_NSF_PACKAGE */
    }
#ifdef L7_MCAST_PACKAGE
    memcpy(&snoopMemberList, &snoopEntry->snoopGrpMemberList, sizeof(snoopMemberList));
    L7_INTF_MASKOREQ(snoopMemberList, snoopEntry->snoopMrtrMemberList);
    snoopNotifyL3Mcast(macAddr, vlanId, &snoopMemberList);
#endif /* L7_MCAST_PACKAGE */

    /* PTin added: IGMP Snooping */
#if 1
    if (!snoopEntry->port_list[intIfNum].active)
    {
      if (snoopEntry->global.number_of_ports<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
        snoopEntry->global.number_of_ports++;

      snoopEntry->port_list[intIfNum].active = L7_TRUE;
      snoopEntry->port_list[intIfNum].number_of_channels = 0;
      snoopEntry->port_list[intIfNum].number_of_clients  = 0;
    }
#endif
  }
  return rc;
}
/***************************************************************************
* @purpose  Deletes an interface from the port list of the multicast address
*           entry and stops the corresponding timer
*
* @param    macAddr   @b{(input)} Multicast MAC Address
* @param    vlanId    @b{(input)} Vlan ID for the MAC Address
* @param    intIfNum  @b{(input)} Internal Interface number
* @param    intfType  @b{(input)} Interface type
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopIntfRemove(L7_uchar8* macAddr, L7_uint32 vlanId,
                        L7_uint32 intIfNum, snoop_interface_type_t intfType,
                        snoop_cb_t *pSnoopCB)
{
  L7_RC_t           rc = L7_SUCCESS;
  snoopInfoData_t  *snoopEntry;
  mfdbMemberInfo_t  mfdb;
  snoop_eb_t       *pSnoopEB;
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t    snoopMemberList;
#endif /* L7_MCAST_PACKAGE */

  pSnoopEB = snoopEBGet();
  snoopEntry = snoopEntryFind(macAddr, vlanId, pSnoopCB->family,
                              L7_MATCH_EXACT);
  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* entry is found... proceed to delete */
  if (intfType == SNOOP_GROUP_MEMBERSHIP)
  {
    if ((L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intIfNum)) == 0)
    {
      return L7_FAILURE;
    }
  }
  else if (intfType == SNOOP_STATIC_MCAST_ROUTER_ATTACHED ||
           intfType == SNOOP_MCAST_ROUTER_ATTACHED)
  {
    if (L7_INTF_ISMASKBITSET(snoopEntry->snoopMrtrMemberList, intIfNum))
    {
      /* If it has a group membership assigned to the interface,
         simply clear the mrtr bit and return. MFDB will be updated when
         group membership is removed */
      if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intIfNum))
      {
        L7_INTF_CLRMASKBIT(snoopEntry->snoopMrtrMemberList, intIfNum);

#ifdef L7_NSF_PACKAGE
        if (pSnoopEB->snoopBackupElected == L7_TRUE)
        {
          snoopCheckpointCallback1(SNOOP_CKPT_MSG_MRTR_DEL_DATA, macAddr, vlanId, pSnoopCB->family, intIfNum, L7_NULL, L7_NULL, L7_NULL);
        }
#endif /* L7_NSF_PACKAGE */

        return L7_SUCCESS;
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
            "snoopIntfRemove: Invalid interface type");
    return L7_FAILURE;
  }

  /* Delete interface from MFDB */
  memset( (L7_uchar8 *)&mfdb, 0, sizeof(mfdb) );
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (pSnoopCB->family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP
                          : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (!snoopEntry->staticGroup) ? L7_MFDB_TYPE_DYNAMIC : L7_MFDB_TYPE_STATIC;  /* PTin modified: IGMP Snooping */
  memcpy((void *)mfdb.user.description, (void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);
  /* Set the mask bit in the fwd mask for this interface */
  L7_INTF_SETMASKBIT(mfdb.user.fwdPorts, intIfNum);

  rc = mfdbEntryPortsDelete(&mfdb);
  if (rc == L7_SUCCESS)
  {
    if (intfType == SNOOP_GROUP_MEMBERSHIP)
    {
      /* PTin added: IGMP Snooping */
#if 0
      if (!snoopEntry->staticGroup)
#endif
      {
        snoopTimerStop(snoopEntry, intIfNum, vlanId, intfType, pSnoopCB);
      }
      L7_INTF_CLRMASKBIT(snoopEntry->snoopGrpMemberList, intIfNum);

#ifdef L7_NSF_PACKAGE
      if (pSnoopEB->snoopBackupElected == L7_TRUE)
      {
        snoopCheckpointCallback1(SNOOP_CKPT_MSG_PORT_DEL_DATA, macAddr, vlanId, pSnoopCB->family, intIfNum, L7_NULL, L7_NULL, L7_NULL);
      }
#endif /* L7_NSF_PACKAGE */

    }
    else
    {
      L7_INTF_CLRMASKBIT(snoopEntry->snoopMrtrMemberList, intIfNum);

#ifdef L7_NSF_PACKAGE
      if (pSnoopEB->snoopBackupElected == L7_TRUE)
      {
        snoopCheckpointCallback1(SNOOP_CKPT_MSG_MRTR_DEL_DATA, macAddr, vlanId, pSnoopCB->family, intIfNum, L7_NULL, L7_NULL, L7_NULL);
      }
#endif /* L7_NSF_PACKAGE */
    }

#ifdef L7_MCAST_PACKAGE
    memcpy(&snoopMemberList, &snoopEntry->snoopGrpMemberList, sizeof(snoopMemberList));
    L7_INTF_MASKOREQ(snoopMemberList, snoopEntry->snoopMrtrMemberList);
    snoopNotifyL3Mcast(macAddr, vlanId, &snoopMemberList);
#endif /* L7_MCAST_PACKAGE */

    /* PTin added: IGMP Snooping */
#if 1
    if (snoopEntry->port_list[intIfNum].active)
    {
      snoopIntfClean(snoopEntry,intIfNum);
    }
#endif
  }

  if (rc == L7_SUCCESS)
  {
    /* Search for any non-multicast router interfaces in this entry */
    if (snoopEntry->ll_timerList.sllStart == L7_NULL)
    {
      /* PTin added: IGMP snooping */
#if 1
      /* Entry deletion is only possible, when group is not static */
      if (!snoopEntry->staticGroup)
#endif
      {
        /* Only router interfaces left in this entry... delete it */
        snoopEntryRemove(macAddr, vlanId, pSnoopCB->family);
      }
    }
  } /* Delete operation success */

  return rc;
}

/* PTin added: IGMP snooping */
#if 1

  #define PTIN_IGMP_DEBUG 0

  #define PTIN_CLEAR_ARRAY(array)         memset((array),0x00,sizeof(array))
  #define PTIN_IS_MASKBITSET(array,idx)   ((array[(idx)/(sizeof(L7_uint32)*8)] >> ((idx)%(sizeof(L7_uint32)*8))) & 1)
  #define PTIN_SET_MASKBIT(array,idx)     { array[(idx)/(sizeof(L7_uint32)*8)] |=   (L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8)) ; }
  #define PTIN_UNSET_MASKBIT(array,idx)   { array[(idx)/(sizeof(L7_uint32)*8)] &= ~((L7_uint32) 1 << ((idx)%(sizeof(L7_uint32)*8))); }

  #define PTIN_NONZEROMASK(array, result)                              \
{                                                                    \
    L7_uint32 _i_;                                                   \
                                                                     \
    for(_i_ = 0; _i_ < sizeof(array)/sizeof(L7_uint32); _i_++)       \
        if(array[_i_] != 0)                                          \
        {                                                            \
            result = 1;                                              \
            break;                                                   \
        }                                                            \
        else                                                         \
            result = 0;                                              \
}

  #define PTIN_INCREMENT_COUNTER(counter,val)   { counter+=val; }
  #define PTIN_DECREMENT_COUNTER(counter,val)   { ((counter)>=(val)) ? (counter-=val) : (counter=0); }

  #define PTIN_CHANNEL_INDEX_GET(ipv4_addr) (((ipv4_addr)->addr.ipv4.s_addr>>23) & 0x1f)

  #if PTIN_IGMP_DEBUG
static void ptin_dump_snoop_entry(snoopInfoData_t *snoopEntry);
  #endif
static L7_RC_t snoopValidateArguments(snoopInfoData_t *snoopEntry,
                                      L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint16 client);
static void snoopChannelsListGet_v2_recursive(avlTreeTables_t *cell_ptr,
                                              L7_uint16 vlanId,
                                              L7_uint16 client_index,
                                              ptin_igmpChannelInfo_t *channel_list,
                                              L7_uint16 *num_channels,
                                              L7_uint16 max_num_channels);

/***************************************************************************
* @purpose  Check if a particular channel group have no interfaces attached
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopIntfNone(snoopInfoData_t *snoopEntry)
{
  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  return(snoopEntry->global.number_of_ports==0);
}

/***************************************************************************
* @purpose  Check if a particular channel group have no channels
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelsNone(snoopInfoData_t *snoopEntry)
{
  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  return(snoopEntry->global.number_of_channels==0);
}

/***************************************************************************
* @purpose  Check if a particular channel group have no channels within an
*           interface
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    intIfNum   @b{(input)} Interface number
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelsIntfNone(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum)
{
  /* Validate interface */
  if (intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    return L7_FALSE;
  }
  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* If interface is not active, there is no channels */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    return L7_TRUE;
  }

  return(snoopEntry->port_list[intIfNum].number_of_channels==0);
}

/***************************************************************************
* @purpose  Check if a particular channel group have no clients within an
*           IP channel
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IP channel
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelClientsNone(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel)
{
  L7_uint           channel_index;

  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* Not ready for IPv6 */
  if (IPchannel->family == L7_AF_INET6)
    return L7_FALSE;

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* If channel is not active, there is no clients */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    return L7_TRUE;
  }

  return(snoopEntry->channel_list[channel_index].number_of_clients==0);
}

/***************************************************************************
* @purpose  Check if a particular channel group have no clients within an
*           IP channel and an interface
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    intIfNum   @b{(input)} Interface number
* @param    IPchannel  @b{(input)} IP channel
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelClientsIntfNone(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel)
{
  L7_uint           channel_index;

  /* Validate interface */
  if (intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    return L7_FAILURE;
  }
  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* Not ready for IPv6 */
  if (IPchannel->family == L7_AF_INET6)
    return L7_FALSE;

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* If channel is not active, there is no clients */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    return L7_TRUE;
  }

  return(snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum]==0);
}

/***************************************************************************
* @purpose  Clean all channels and clients for a particular snoop entry
*           and interface
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    intIfNum   @b{(input)} Interface number
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopIntfClean(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum)
{
  L7_uint n_clients;
  L7_uint16 vlan;
  ptin_intf_t ptin_intf;
  L7_uint channel_index,i;
  ptin_client_id_t clientData;
  L7_inet_addr_t ip_addr;
  L7_uint igmp_network_version;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,intIfNum,0,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIntfChannelsRemoveAll: Invalid arguments");
    return L7_FAILURE;
  }

  /* Vlan and interface */
  vlan = (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8 | (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1];
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIntfChannelsRemoveAll: Invalid intIfNum (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlan, intIfNum, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /* Run all channels */
  for (channel_index=0; channel_index<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_index++)
  {
    /* IP channel must be active */
    if (!snoopEntry->channel_list[channel_index].active)  continue;

    if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum))
      continue;
    /* Number of clients to be removed */
    n_clients = snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum];
    /* Update number of clients for this channel */
    PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_clients, n_clients);
    /* Update global number of clients */
    PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_clients, n_clients);

    /* Clear only clients which are using this interface */
    for (i=0; i<PTIN_SYSTEM_IGMP_MAXCLIENTS; i++)
    {
      /*Check if this position is free*/
      if(snoopEntry->channel_list[channel_index].clients_list[i] == 0)
      {
        /*Move to Next Position -1 because of the for*/
        i += (sizeof(snoopEntry->channel_list[channel_index].clients_list[i])*8) -1;
        continue;
      }

      if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,i))  continue;

      /* Get client information */
      if (ptin_igmp_clientData_get(intIfNum, i, &clientData) != L7_SUCCESS)  continue;

      /* If this client is not attached to this interface, do nothing */
      if (clientData.ptin_intf.intf_type!=ptin_intf.intf_type ||
          clientData.ptin_intf.intf_id  !=ptin_intf.intf_id)  continue;

      /* Clear client bit */
      PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].clients_list,i);
      /* Decrement number of active channels for this client */
      ptin_igmp_stat_decrement_field(0, vlan, i, SNOOP_STAT_FIELD_ACTIVE_GROUPS);
    }

    /* No clients for this channel and interface */
    snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum] = 0;

    /* Deactivate port for this channel */
    PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum);
    /* One less port for this channel */
    PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_ports,1);

    /* If this channel does not have any client, remove it */
    if (snoopEntry->channel_list[channel_index].number_of_clients==0)
    {
      /* Deactivate channel (only for dynamic entries) */
      if (!snoopEntry->staticGroup)
      {
        /* Send LEAVES upsteam (only for IGMP v2) */
        /* Send two leave messages */
        if (igmp_network_version <= 2)
        {
          ip_addr.family = L7_AF_INET;
          ip_addr.addr.ipv4.s_addr = snoopEntry->channel_list[channel_index].ipAddr;
          if (igmp_generate_packet_and_send(vlan,L7_IGMP_V2_LEAVE_GROUP,&ip_addr)!=L7_SUCCESS /*||
              igmp_generate_packet_and_send(vlan,L7_IGMP_V2_LEAVE_GROUP,&ip_addr)!=L7_SUCCESS*/)
          {
            if (ptin_debug_igmp_snooping)
              LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending leaves to router interfaces");
          }
        }

        memset(&snoopEntry->channel_list[channel_index],0x00,sizeof(ptinSnoopChannelInfo_t));
        snoopEntry->channel_list[channel_index].active = L7_FALSE;
        /* One less channel (globally) */
        PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_channels,1);
      }
    }

    /* At the end no channels should exist for this interface */
    snoopEntry->port_list[intIfNum].number_of_clients  = 0;
    snoopEntry->port_list[intIfNum].number_of_channels = 0;
    snoopEntry->port_list[intIfNum].active = L7_FALSE;

    /* One less port globally */
    PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_ports,1);

#if PTIN_IGMP_DEBUG
    printf("[%s function]\r\n",__FUNCTION__);
    ptin_dump_snoop_entry(snoopEntry);
#endif
  }

  /* Validate arguments */
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Check if a particular channel exists in a Vlan+MAC entry
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IP channel
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelExist(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel)
{
  L7_uint           channel_index;

  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  return(snoopEntry->channel_list[channel_index].active);
}

/***************************************************************************
* @purpose  Create a new IP channel.
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IPv4 channel
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelCreate(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel)
{
  L7_uint channel_index;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelCreate: Invalid arguments");
    return L7_FAILURE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* Verify if this channel is already in use */
  if (snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopIPChannelCreate: IP channel 0x%8X is already enabled",IPchannel->addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  /* Fill IP info */
  snoopEntry->channel_list[channel_index].ipAddr = IPchannel->addr.ipv4.s_addr;
  snoopEntry->channel_list[channel_index].number_of_ports   = 0;
  PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intIfNum_mask);
  snoopEntry->channel_list[channel_index].number_of_clients = 0;
  PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].clients_list);
  PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intf_number_of_clients);

  snoopEntry->channel_list[channel_index].active = L7_TRUE;

  /* One new channel */
  PTIN_INCREMENT_COUNTER(snoopEntry->global.number_of_channels,1);

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Delete an IP channel.
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IPv4 channel
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelDelete(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel)
{
  L7_uint channel_index, i;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelDelete: Invalid arguments");
    return L7_FAILURE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* Verify if this channel is already OFF */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopIPChannelDelete: IP channel 0x%8X is already disabled",IPchannel->addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  /* Run all interfaces, and update number of channels for each interface */
  for (i=0; i<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; i++)
  {
    /*Check if this position is free*/
    if(snoopEntry->channel_list[channel_index].intIfNum_mask[i] == 0)
    {
      /*Move to Next Position -1 because of the for*/
      i += (sizeof(snoopEntry->channel_list[channel_index].intIfNum_mask[i])*8) -1;
      continue;
    }

    if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,i))
      continue;
    /* One less interface for this channel */
    PTIN_DECREMENT_COUNTER(snoopEntry->port_list[i].number_of_channels,1);
    /* Update number of clients at the interface level */
    PTIN_DECREMENT_COUNTER(snoopEntry->port_list[i].number_of_clients, snoopEntry->channel_list[channel_index].intf_number_of_clients[i]);
  }
  /* Update global number of clients */
  PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_clients, snoopEntry->channel_list[channel_index].number_of_clients);

  /* Clear all information, and set active flag to FALSE */
  memset(&snoopEntry->channel_list[channel_index],0x00,sizeof(ptinSnoopChannelInfo_t));
  snoopEntry->channel_list[channel_index].active = L7_FALSE;

  /* One less channel */
  PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_channels,1);

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Delete all IP channels and all clients.
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelDeleteAll(snoopInfoData_t *snoopEntry)
{
  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,L7_NULLPTR,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelDelete: Invalid arguments");
    return L7_FAILURE;
  }

  /* Clear all channels and clients */

  snoopEntry->global.number_of_channels = 0;
  snoopEntry->global.number_of_clients  = 0;

  PTIN_CLEAR_ARRAY(snoopEntry->channel_list);
  PTIN_CLEAR_ARRAY(snoopEntry->port_list   );

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  return L7_SUCCESS;
}


/***************************************************************************
* @purpose  Add an interface to an IP channel. If the IP does not exist, it
*           will be created.
*
* @param    snoopEntry    @b{(input)} Snooping entry in AVL tree
* @param    intIfNum      @b{(input)} Interface number
* @param    IPchannel     @b{(input)} IPv4 channel
* @param    isProtection  @b{(input)} Protection Entry
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelIntfAdd(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_BOOL isProtection)
{
  L7_uint channel_index;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,intIfNum,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelIntfAdd: Invalid arguments");
    return L7_FAILURE;
  }

  /* Interface must be active */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelIntfAdd: Interface %u not active in this channel group",intIfNum);
    return L7_FAILURE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* If this IP does not exist, create it (only for dynamic groups) */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (!snoopEntry->staticGroup)
    {
      /* Fill IP info */
      snoopEntry->channel_list[channel_index].ipAddr = IPchannel->addr.ipv4.s_addr;
      snoopEntry->channel_list[channel_index].number_of_ports   = 0;
      PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intIfNum_mask);
      #if PTIN_BOARD_IS_MATRIX
      PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].protection_mask);
      #endif
      snoopEntry->channel_list[channel_index].number_of_clients = 0;
      PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].clients_list);
      PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intf_number_of_clients);

      snoopEntry->channel_list[channel_index].active = L7_TRUE;

      /* One new channel */
      PTIN_INCREMENT_COUNTER(snoopEntry->global.number_of_channels,1);
    }
    else
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Static channel does not exist!");
      return L7_FAILURE;
    }
  }

  /* Activate interface using it */
  if (intIfNum!=0)
  {
    if (intIfNum==L7_ALL_INTERFACES)
    {
      /* Do not add all interfaces... */
    }
    else
    {
      if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum))
      {
        PTIN_SET_MASKBIT(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum);
        PTIN_INCREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_ports,1);
        PTIN_INCREMENT_COUNTER(snoopEntry->port_list[intIfNum].number_of_channels,1);
      }
      else
      {
        if (ptin_debug_igmp_snooping)
          LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Interface %u is already active",intIfNum);
      }
      #if PTIN_BOARD_IS_MATRIX
      if (isProtection == L7_TRUE)
      {
        if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].protection_mask,intIfNum))
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Protection Entry Add: IP channel 0x%8X intIfNum %u",IPchannel->addr.ipv4.s_addr, intIfNum);
          PTIN_SET_MASKBIT(snoopEntry->channel_list[channel_index].protection_mask,intIfNum);        
        }
        else
        {
          if (ptin_debug_igmp_snooping)
            LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Interface %u is already in protection",intIfNum);
        }
      }
      #endif
    }
  }

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Remove an interface from an IP channel, and remove IP if all
*           interfaces are removed.
*           This can only be done, if there is no clients attached to this
*           channel and interface! Otherwise, error will be returned.
*
* @param    snoopEntry    @b{(input)} Snooping entry in AVL tree
* @param    intIfNum      @b{(input)} Interface number
* @param    IPchannel     @b{(input)} IPv4 channel
* @param    isProtection  @b{(input)} Protection Entry
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelIntfRemove(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_BOOL isProtection)
{
  L7_uint channel_index, i;
  L7_uint exist_interfaces;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,intIfNum,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelIntfRemove: Invalid arguments");
    return L7_FAILURE;
  }

  /* Interface must be in use */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelIntfRemove: Interface %u not active in this channel group",intIfNum);
    return L7_FAILURE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* Verify if this channel is already OFF */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopIPChannelIntfRemove: IP channel 0x%8X is already disabled",IPchannel->addr.ipv4.s_addr);
    return L7_SUCCESS;
  }

  if (intIfNum!=0)
  {
    if (intIfNum==L7_ALL_INTERFACES)
    {
      for (i=0; i<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; i++)
      {
        /*Check if this position is free*/
        if(snoopEntry->channel_list[channel_index].intIfNum_mask[i] == 0)
        {
          /*Move to Next Position -1 because of the for*/
          i += (sizeof(snoopEntry->channel_list[channel_index].intIfNum_mask[i])*8) -1;
          continue;
        }

        if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,i))
          continue;

        /* If there is attached clients to to this channel and interface, interface cannot be removed */
        if (snoopEntry->channel_list[channel_index].intf_number_of_clients[i]>0)
        {
          rc = L7_FAILURE;
          continue;
        }

        /* Remove interface from this channel */
        PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].intIfNum_mask,i);
        PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_ports,1);

        /* One less interface for this channel */
        PTIN_DECREMENT_COUNTER(snoopEntry->port_list[i].number_of_channels,1);
      }
    }
    else
    {
      if (PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum))
      {
        #if PTIN_BOARD_IS_MATRIX
        if (PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].protection_mask,intIfNum))
        {
          if (isProtection == L7_TRUE || snoopEntry->channel_list[channel_index].number_of_ports == 1)
          {
            /* Remove interface from protection list */
            PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].protection_mask,intIfNum);        
          }
          else
          {            
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Protection Entry Remove (Ignore): IP channel 0x%8X intIfNum %u",IPchannel->addr.ipv4.s_addr, intIfNum);
            return L7_SUCCESS;
          }
        }
        #endif

        /* If there is attached clients to to this channel and interface, interface cannot be removed */
        if (snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum]==0)
        {
          /* Remove interface from this channel */
          PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].intIfNum_mask,intIfNum);
          PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_ports,1);

          /* One less interface for this channel */
          PTIN_DECREMENT_COUNTER(snoopEntry->port_list[intIfNum].number_of_channels,1);
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
    }
  }

  /* Only remove the channel, if group is dynamic */
  if (!snoopEntry->staticGroup)
  {
    /* If there is no interfaces within this channel, remove channel */
    PTIN_NONZEROMASK(snoopEntry->channel_list[channel_index].intIfNum_mask,exist_interfaces);
    if (!exist_interfaces)
    {
      /* Clear all information, and set active flag to FALSE */
      memset(&snoopEntry->channel_list[channel_index],0x00,sizeof(ptinSnoopChannelInfo_t));

      /* One less channel */
      PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_channels,1);
    }
  }

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  return rc;
}

/***************************************************************************
* @purpose  Verify if a client (index) is consuming an IP channel of a
*           Vlan+MAC group
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IPv4 channel
* @param    client     @b{(input)} client index
*
* @returns  L7_TRUE/L7_FALSE
*
* @notes    none
*
* @end
****************************************************************************/
L7_BOOL snoopChannelClientExists(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel, L7_uint client)
{
  L7_uint channel_index;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,IPchannel,client)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: Invalid arguments");
    return L7_FALSE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* IP channel must be active */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: IP channel 0x%8X is disabled",IPchannel->addr.ipv4.s_addr);
    return L7_FALSE;
  }

  /* Check if this client already exists */
  if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,client))
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/***************************************************************************
* @purpose  Add a new client (index) to an IP channel of a Vlan+MAC group
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    intIfNum   @b{(input)} Interface number
* @param    IPchannel  @b{(input)} IPv4 channel
* @param    client     @b{(input)} client index
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelClientAdd(snoopInfoData_t *snoopEntry,
                              L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint client)
{
  L7_uint16 vlan;
  L7_uint channel_index;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,intIfNum,IPchannel,client)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: Invalid arguments");
    return L7_FAILURE;
  }

  /* This is the MC vlan */
  vlan = (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8 | (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1];

  /* Check if interface is active */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: Interface %u is not active in this channel group",intIfNum);
    return L7_FAILURE;
  }

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* IP channel must be active */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: IP channel 0x%8X is disabled",IPchannel->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Check if this client already exists */
  if (PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,client))
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopClientAdd: Client index %u is already enabled",client);
    return L7_SUCCESS;
  }

  /* Verify if one more client can be added */
  if (snoopEntry->channel_list[channel_index].number_of_clients>=PTIN_SYSTEM_IGMP_MAXCLIENTS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: No more room to add a new client",client);
    return L7_FAILURE;
  }

  /* Activate new client for this IP channel */
  PTIN_SET_MASKBIT(snoopEntry->channel_list[channel_index].clients_list,client);
  PTIN_INCREMENT_COUNTER(snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum],1);
  PTIN_INCREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_clients,1);
  PTIN_INCREMENT_COUNTER(snoopEntry->port_list[intIfNum].number_of_clients,1);
  PTIN_INCREMENT_COUNTER(snoopEntry->global.number_of_clients,1);

  /* Increment number of active channels for this client */
  ptin_igmp_stat_increment_field(0, vlan, client, SNOOP_STAT_FIELD_ACTIVE_GROUPS);

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  /* Validate arguments */
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Remove a client (index) from an IP channel of a Vlan+MAC group
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    intIfNum   @b{(input)} Interface number
* @param    IPchannel  @b{(input)} IPv4 channel
* @param    client     @b{(input)} client index
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelClientRemove(snoopInfoData_t *snoopEntry,
                                 L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint client)
{
  L7_uint16 vlan;
  L7_uint channel_index;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,intIfNum,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: Invalid arguments");
    return L7_FAILURE;
  }

  /* Check if interface is active */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: Interface %u is not active in this channel group",intIfNum);
    return L7_FAILURE;
  }

  /* MC vlan */
  vlan = (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8 | (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1];

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* IP channel must be active */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientRemove: IP channel 0x%8X is disabled",IPchannel->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Check if this client is not already in use */
  if ( !PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,client) )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopClientRemove: Client index %u is already disabled",client);
    return L7_SUCCESS;
  }

  /* Deactivate client for this IP channel */
  PTIN_UNSET_MASKBIT(snoopEntry->channel_list[channel_index].clients_list,client);
  PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].intf_number_of_clients[intIfNum],1);
  PTIN_DECREMENT_COUNTER(snoopEntry->channel_list[channel_index].number_of_clients,1);
  PTIN_DECREMENT_COUNTER(snoopEntry->port_list[intIfNum].number_of_clients,1);
  PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_clients,1);

  /* Decrement number of active channels for this client */
  ptin_igmp_stat_decrement_field(0, vlan, client, SNOOP_STAT_FIELD_ACTIVE_GROUPS);

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  /* Validate arguments */
  return L7_SUCCESS;
}

/**
 * Add channel and update Snoop Entry database. Also determine 
 * if a leave should be sent to network 
 * 
 * @param dmac                  : MAC address 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param staticChannel         : channel is static
 * @param send_leave_to_network : send leave? (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoop_channel_add_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                    L7_inet_addr_t *mgmdGroupAddr, L7_BOOL staticChannel,
                                    L7_BOOL *send_leave_to_network)
{
  L7_inet_addr_t ip_addr;
  L7_uint16 channel_index;
  snoopInfoData_t *snoopEntry;
  L7_BOOL fwdFlag = L7_FALSE;
  L7_uint32 igmp_network_version;
  L7_uint32 intIfNum;
  L7_INTF_MASK_t mcastClientAttached;

  /* Validate arguments */
  if (dmac==L7_NULLPTR || vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize output values */
  if (send_leave_to_network!=L7_NULLPTR)
  {
    *send_leave_to_network = L7_FALSE;
  }

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlanId, L7_ALL_INTERFACES, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /*Get list of clients */
  if (staticChannel)
  {
    if (ptin_igmp_clientIntfs_getList(vlanId, &mcastClientAttached)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting client interfaces of vlan %u",vlanId);
      return L7_FAILURE;
    }
  }

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "snoop entry does not exist");
    /* Entry does not exist... give back the semaphore and create new entry */
    if (snoopEntryCreate(dmac, vlanId, L7_AF_INET, staticChannel)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryCreate failed. Table full");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry successfully created!");
    /* Check if snooping entry exists */
    if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryFind failed.");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry exists now");

    /* Add interfaces for static snoopEntry */
    if (staticChannel)
    {
      /* Forward frame to all interfaces in this VLAN with multicast routers attached */
      for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if ( (L7_INTF_ISMASKBITSET(mcastClientAttached,intIfNum)) )
        {
          if (snoopIntfAdd(dmac, vlanId, intIfNum, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET))!=L7_SUCCESS)
          {
            if (ptin_debug_igmp_snooping)
              LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding intIfNum %u to vlanId %u",intIfNum,vlanId);
          }
        }
      }
    }
  }
  else
  {
    if (snoopEntry->staticGroup && !staticChannel)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Dynamic channels cannot be added to static groups");
      return L7_FAILURE;
    }
    else if (!snoopEntry->staticGroup && staticChannel)
    {
      /* Remove all dynamic channels */
      for (channel_index=0; channel_index<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_index++)
      {
        if (!snoopEntry->channel_list[channel_index].active)  continue;
        /* Remove all channels */
        ip_addr.family = L7_AF_INET;
        ip_addr.addr.ipv4.s_addr = snoopEntry->channel_list[channel_index].ipAddr;
        if (snoop_channel_remove_procedure(dmac,vlanId,&ip_addr)!=L7_SUCCESS)
        {
          if (ptin_debug_igmp_snooping)
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel");
          return L7_FAILURE;
        }
      }
      /* Make sure, snooping entry was removed */
      if (snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT) != L7_NULLPTR)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snooping entry still exists");
        return L7_FAILURE;
      }
      /* Recreate snooping entry */
      if (snoopEntryCreate(dmac, vlanId, L7_AF_INET, staticChannel)!=L7_SUCCESS)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryCreate failed. Table full");
        return L7_FAILURE;
      }
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry successfully created!");
      /* Check if snooping entry exists */
      if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryFind failed.");
        return L7_FAILURE;
      }
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry exists as static");
    }
  }

  /* Check if channel exists... mark to forward join to network */
  if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
  {
    if (snoopChannelCreate(snoopEntry,mgmdGroupAddr)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelCreate failed");
      return L7_FAILURE;
    }

    /* Add interfaces for static channel */
    if (staticChannel)
    {
      /* Forward frame to all interfaces in this VLAN with multicast routers attached */
      for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if ( (L7_INTF_ISMASKBITSET(mcastClientAttached,intIfNum)) )
        {
          if (snoopChannelIntfAdd(snoopEntry, intIfNum, mgmdGroupAddr, L7_FALSE)!=L7_SUCCESS)
          {
            if (ptin_debug_igmp_snooping)
              LOG_ERR(LOG_CTX_PTIN_IGMP, "Error adding intIfNum %u to channel 0x%08x",intIfNum,mgmdGroupAddr);
          }
        }
      }
    }

    /* Only send joins if in matrix or standalone */
#if (PTIN_BOARD_IS_LINECARD)
    if (!staticChannel)
#endif
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Message will be sent to network");
      fwdFlag = L7_TRUE;
    }
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"channel already exists");
  }

  /* Send two joins */
  if (fwdFlag && igmp_network_version<=2)
  {
    if (igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_MEMBERSHIP_REPORT,mgmdGroupAddr)!=L7_SUCCESS /*||
        igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_MEMBERSHIP_REPORT,mgmdGroupAddr)!=L7_SUCCESS*/)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending joins to router interfaces");
      return L7_FAILURE;
    }
  }

  /* Forward leave to network? */
  if (send_leave_to_network!=L7_NULLPTR)
  {
    *send_leave_to_network = fwdFlag;
  }

  return L7_SUCCESS;
}

/**
 * Remove channel and update Snoop Entry database. 
 * Also determine if a leave should be sent to network
 * 
 * @param dmac                  : MAC address 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoop_channel_remove_procedure(L7_uchar8 *dmac, L7_uint16 vlanId, L7_inet_addr_t *mgmdGroupAddr)
{
  snoopInfoData_t *snoopEntry;
  L7_uint          igmp_network_version;
  L7_BOOL          static_group;

  /* Validate arguments */
  if (dmac==L7_NULLPTR || vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlanId, L7_ALL_INTERFACES, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  snoopEntry = snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT);
  if ( snoopEntry == L7_NULLPTR )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopEntryFind failed!");
    return L7_SUCCESS;
  }

  /* Static group */
  static_group = snoopEntry->staticGroup;

  /* Verify if this channel exists */
  if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This channel does not exist");
    return L7_SUCCESS;
  }

  /* Remove all clients for this channel */
  if (snoopChannelClientsRemoveAll(snoopEntry,mgmdGroupAddr)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing clients from channel");
    return L7_FAILURE;
  }

  /* Remove this channel */
  if (snoopChannelDelete(snoopEntry,mgmdGroupAddr)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelDelete failed");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Interface removed for this channel");

  /* If there is no channels for this group, remove interface from group */
  if (snoopChannelsNone(snoopEntry))
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"No channels for this group");
    if (snoopEntryRemove(dmac,vlanId,L7_AF_INET)!= L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to remove group membership");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"interface was removed from group");
  }

  /* Only send leaves upstream if standalone or matrix */
#if (PTIN_BOARD_IS_LINECARD)
  if (!static_group)
#endif
  {
    /* Send two leave messages */
    if (igmp_network_version <= 2)
    {
      if (igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,mgmdGroupAddr)!=L7_SUCCESS /*||
          igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,mgmdGroupAddr)!=L7_SUCCESS*/)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending leaves to router interfaces");
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Add client and update Snoop Entry database. Also determine 
 * if a leave should be sent to network
 * 
 * @param dmac                  : MAC address 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param client                : client index
 * @param intIfNum              : interface 
 * @param send_leave_to_network : send leave? (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoop_client_add_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                   L7_inet_addr_t *mgmdGroupAddr,
                                   L7_uint client, L7_uint32 intIfNum,
                                   L7_BOOL *send_leave_to_network)
{
  snoopInfoData_t *snoopEntry;
  L7_BOOL fwdFlag = L7_FALSE;
  L7_uint32 igmp_network_version;

  ptin_timer_start(3,"snoop_client_add_procedure-start");

  /* Validate arguments */
  if (dmac==L7_NULLPTR || vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET ||
      client>=PTIN_SYSTEM_IGMP_MAXCLIENTS ||
      intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize output values */
  if (send_leave_to_network!=L7_NULLPTR)
  {
    *send_leave_to_network = L7_FALSE;
  }

  ptin_timer_start(5,"snoop_client_add_procedure-snoopEntryCreate");

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlanId, intIfNum, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "snoop entry does not exist");
    /* Entry does not exist... give back the semaphore and create new entry */
    if (snoopEntryCreate(dmac, vlanId, L7_AF_INET, L7_FALSE)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryCreate failed. Table full");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry successfully created!");
    /* Check if snooping entry exists */
    if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryFind failed.");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry exists now");
  }
  ptin_timer_stop(5);

  ptin_timer_start(6,"snoop_client_add_procedure-snoopChannelExist");
  /* Check if channel exists... mark to forward join to network */
  if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
  {
    if (!snoopEntry->staticGroup)
    {
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Message will be sent to network");
      fwdFlag = L7_TRUE;
    }
    else
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "This channel does not exist (static group)");
      return L7_FAILURE;
    }
  }
  ptin_timer_stop(6);

  ptin_timer_start(7,"snoop_client_add_procedure-snoopIntfAdd");
  /* If the entry already existed or it was just successfully created, add the interface */
  if (snoopIntfAdd(dmac, vlanId, intIfNum, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET))!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIntfAdd failed");
    return L7_FAILURE;
  }
  ptin_timer_stop(7);

  ptin_timer_start(8,"snoop_client_add_procedure-snoopChannelIntfAdd");
  /* Add interface for this channel */
  if (snoopChannelIntfAdd(snoopEntry,intIfNum,mgmdGroupAddr, L7_FALSE)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelIntfAdd failed");
    return L7_FAILURE;
  }
  ptin_timer_stop(8);

  ptin_timer_start(9,"snoop_client_add_procedure-snoopChannelClientAdd");
  /* Add client */
  if (snoopChannelClientAdd(snoopEntry,intIfNum,mgmdGroupAddr,client)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelClientAdd failed");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"client added");
  ptin_timer_stop(9);

  ptin_timer_stop(3);

  ptin_timer_start(4,"snoop_client_add_procedure-igmp_generate_packet_and_send");
  /* Send one join */
  if (fwdFlag && igmp_network_version<=2)
  {
    if (igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_MEMBERSHIP_REPORT,mgmdGroupAddr)!=L7_SUCCESS /*||
        igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_MEMBERSHIP_REPORT,mgmdGroupAddr)!=L7_SUCCESS*/)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending join to router interfaces");
      return L7_FAILURE;
    }
  }
  ptin_timer_stop(4);

  /* Forward leave to network? */
  if (send_leave_to_network!=L7_NULLPTR)
  {
    *send_leave_to_network = fwdFlag;
  }

  return L7_SUCCESS;
}

/**
 * Remove client, and update Snoop Entry database. Also 
 * determine if a leave should be sent to network 
 * 
 * @param dmac                  : MAC address 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param client                : client index
 * @param intIfNum              : interface 
 * @param send_leave_to_network : send leave? (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoop_client_remove_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                      L7_inet_addr_t *mgmdGroupAddr,
                                      L7_uint client, L7_uint32 intIfNum,
                                      L7_BOOL *send_leave_to_network)
{
  snoopInfoData_t *snoopEntry;
  L7_BOOL fwdFlag = L7_FALSE;
  L7_uint         igmp_network_version;

  /* Validate arguments */
  if (dmac==L7_NULLPTR || vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET ||
      client>=PTIN_SYSTEM_IGMP_MAXCLIENTS ||
      intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize output values */
  if (send_leave_to_network!=L7_NULLPTR)
  {
    *send_leave_to_network = L7_FALSE;
  }

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlanId, intIfNum, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  snoopEntry = snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT);
  if ( snoopEntry == L7_NULLPTR )
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"snoopEntryFind failed!");
    return L7_SUCCESS;
  }

  /* If interface is not active for this group, do nothing */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Interface %u is not active.",intIfNum);
    return L7_SUCCESS;
  }

  /* Verify if this channel exists */
  if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This channel does not exist");
    return L7_SUCCESS;
  }

  /* Is this client consuming this channel? If not, there is nothing to be done */
  if (!snoopChannelClientExists(snoopEntry,mgmdGroupAddr,client))
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This client does not exist!");
    return L7_SUCCESS;
  }

  /* Remove client */
  if (snoopChannelClientRemove(snoopEntry,intIfNum,mgmdGroupAddr,client)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelClientRemove failed");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client removed");

  /* Procedures when no one is watching a channel in a particular interface... */
  /* Is there no clients for this channel+interface? */
  if (snoopChannelClientsIntfNone(snoopEntry,intIfNum,mgmdGroupAddr))
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"No clients for this channel and interface");

    /* For static channels, the channel/intf won't be removed */
    if (!snoopEntry->staticGroup)
    {
      /* Remove interface from this channel */
      if (snoopChannelIntfRemove(snoopEntry,intIfNum,mgmdGroupAddr, L7_FALSE)!=L7_SUCCESS)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelIntfRemove failed");
        return L7_FAILURE;
      }
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Interface removed for this interface");

      /* If channel was removed, send LEAVE to network */
      if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
      {
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Message will be forward to network");
        fwdFlag = L7_TRUE;
      }

      /* If there is no channels for this interface, remove interface from group */
      if (snoopChannelsIntfNone(snoopEntry,intIfNum))
      {
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"No channels for this interface");
        if (snoopIntfRemove(dmac,vlanId,intIfNum,SNOOP_GROUP_MEMBERSHIP,snoopCBGet(L7_AF_INET))!= L7_SUCCESS)
        {
          if (ptin_debug_igmp_snooping)
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to remove group membership");
          return L7_FAILURE;
        }
        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"interface was removed from group");
      }
    }
  }

  /* Send one leave */
  if (igmp_network_version <= 2)
  {
    if (fwdFlag)
    {
      if (igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,mgmdGroupAddr)!=L7_SUCCESS /*||
          igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,mgmdGroupAddr)!=L7_SUCCESS*/)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending leave to router interfaces");
        return L7_FAILURE;
      }
    }

    /* Forward leave to network? */
    if (send_leave_to_network!=L7_NULLPTR)
    {
      *send_leave_to_network = fwdFlag;
    }
  }

  return L7_SUCCESS;
}


/**
 * Add IPv4 channel and update Snoop Entry database.
 * 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param intIfNum              : interface 
 * @param isStatic              : Static Entry 
 * @param isProtection          : Protection Entry 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoopGroupIntfAdd(L7_uint16 vlanId, L7_inet_addr_t* mgmdGroupAddr, L7_uint32 intIfNum, L7_BOOL isStatic, L7_BOOL isProtection)
{
  snoopInfoData_t *snoopEntry;
  char dmac[L7_ENET_MAC_ADDR_LEN];

  /* Validate arguments */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET ||
      intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Determine DMAC */
  dmac[0] = 0x01;
  dmac[1] = 0x00;
  dmac[2] = 0x5E;
  dmac[3] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr>>16) & 0x7f);
  dmac[4] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr>> 8) & 0xff);
  dmac[5] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr    ) & 0xff);

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry does not exist");
    /* Entry does not exist... give back the semaphore and create new entry */
    if (snoopEntryCreate(dmac, vlanId, L7_AF_INET, isStatic)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryCreate failed. Table full");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry successfully created!");
    /* Check if snooping entry exists */
    if ((snoopEntry=snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopEntryFind failed.");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoop entry exists now");
  }

  /* If the entry already existed or it was just successfully created, add the interface */
  if (snoopIntfAdd(dmac, vlanId, intIfNum, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET))!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIntfAdd failed");
    return L7_FAILURE;
  }

  
  if(isStatic==L7_TRUE)
    snoopEntry->staticGroup=L7_FALSE;
  /* Add interface for this channel */
  if (snoopChannelIntfAdd(snoopEntry,intIfNum,mgmdGroupAddr, isProtection)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelIntfAdd failed");
    return L7_FAILURE;
  }
  if(isStatic==L7_TRUE)
    snoopEntry->staticGroup=L7_TRUE;

  return L7_SUCCESS;
}

/**
 * Remove IPv4 channel, and update Snoop Entry database. 
 * 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param intIfNum              : interface 
 * @param isProtection          : protection
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoopGroupIntfRemove(L7_uint16 vlanId, L7_inet_addr_t *mgmdGroupAddr, L7_uint32 intIfNum, L7_BOOL isProtection)
{
  snoopInfoData_t *snoopEntry;
  char dmac[L7_ENET_MAC_ADDR_LEN];

  /* Validate arguments */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX ||
      mgmdGroupAddr==L7_NULLPTR || mgmdGroupAddr->family!=L7_AF_INET ||
      intIfNum==0 || intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Determine DMAC */
  dmac[0] = 0x01;
  dmac[1] = 0x00;
  dmac[2] = 0x5E;
  dmac[3] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr>>16) & 0x7f);
  dmac[4] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr>> 8) & 0xff);
  dmac[5] = (L7_uchar8) ((mgmdGroupAddr->addr.ipv4.s_addr    ) & 0xff);

  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  snoopEntry = snoopEntryFind(dmac, vlanId, L7_AF_INET, L7_MATCH_EXACT);
  if ( snoopEntry == L7_NULLPTR )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"snoopEntryFind failed!");
    return L7_SUCCESS;
  }

  /* If interface is not active for this group, do nothing */
  if (!snoopEntry->port_list[intIfNum].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Interface %u is not active.",intIfNum);
    return L7_SUCCESS;
  }

  /* Verify if this channel exists */
  if (!snoopChannelExist(snoopEntry,mgmdGroupAddr))
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"This channel does not exist");
    return L7_SUCCESS;
  }

  /* Procedures when no one is watching a channel in a particular interface... */
  /* Remove interface from this channel */
  if (snoopChannelIntfRemove(snoopEntry,intIfNum,mgmdGroupAddr, isProtection)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelIntfRemove failed");
    return L7_FAILURE;
  }
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Interface removed for this interface");

  /* If there is no channels for this interface, remove interface from group */
  if (snoopChannelsIntfNone(snoopEntry,intIfNum))
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"No channels for this interface");
    if (snoopIntfRemove(dmac,vlanId,intIfNum,SNOOP_GROUP_MEMBERSHIP,snoopCBGet(L7_AF_INET))!= L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to remove group membership");
      return L7_FAILURE;
    }
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"interface was removed from group");
  }

  return L7_SUCCESS;
}


/***************************************************************************
* @purpose  Remove all clients from an IP channel of a Vlan+MAC group
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    IPchannel  @b{(input)} IPv4 channel
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopChannelClientsRemoveAll(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel)
{
  L7_uint16 vlan;
  L7_uint channel_index, client, i;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,IPchannel,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: Invalid arguments");
    return L7_FAILURE;
  }

  /* MC vlan */
  vlan = (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8 | (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1];

  /* Calculate index on IP base */
  channel_index = PTIN_CHANNEL_INDEX_GET(IPchannel);

  /* IP channel must be active */
  if (!snoopEntry->channel_list[channel_index].active)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientRemove: IP channel 0x%8X is disabled",IPchannel->addr.ipv4.s_addr);
    return L7_FAILURE;
  }

  /* Update number of clients, at the interface level */
  for (i=0; i<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; i++)
  {
    /*Check if this position is free*/
    if(snoopEntry->channel_list[channel_index].intIfNum_mask[i] == 0)
    {
      /*Move to Next Position -1 because of the for*/
      i += (sizeof(snoopEntry->channel_list[channel_index].intIfNum_mask[i])*8) -1;
      continue;
    }

    if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,i))
      continue;
    PTIN_DECREMENT_COUNTER(snoopEntry->port_list[i].number_of_clients, snoopEntry->channel_list[channel_index].intf_number_of_clients[i]);
  }
  /* Update global number of clients */
  PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_clients, snoopEntry->channel_list[channel_index].number_of_clients);

  /* Run all clients, and update number of active channels for each one */
  for (client=0; client<PTIN_SYSTEM_IGMP_MAXCLIENTS; client++)
  {
    /*Check if this position is free*/
    if(snoopEntry->channel_list[channel_index].clients_list[client] == 0)
    {
      /*Move to Next Position -1 because of the for*/
      client += (sizeof(snoopEntry->channel_list[channel_index].clients_list[client])*8) -1;
      continue;
    }

    if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,client))  continue;

    /* Decrement number of active channels for this client */
    ptin_igmp_stat_decrement_field(0, vlan, client, SNOOP_STAT_FIELD_ACTIVE_GROUPS);
  }

  /* Clear all clients */
  snoopEntry->channel_list[channel_index].number_of_clients = 0;
  PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].clients_list);
  PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intf_number_of_clients);

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  /* Validate arguments */
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Remove all clients of all IP channels associated to a Vlan+MAC
*           group
*
* @param    snoopEntry @b{(input)} Snooping entry in AVL tree
* @param    vlanId     @b{(input)} Vlan ID for the MAC Address
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopClientsRemoveAll(snoopInfoData_t *snoopEntry)
{
  L7_uint16 vlan;
  L7_uint channel_index, client, i;

  /* Validate arguments */
  if (snoopValidateArguments(snoopEntry,0,L7_NULLPTR,0)!=L7_SUCCESS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: Invalid arguments");
    return L7_FAILURE;
  }

  /* MC vlan */
  vlan = (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8 | (L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1];

  /* Run all (active) channels */
  for (channel_index=0; channel_index<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_index++)
  {
    if (!snoopEntry->channel_list[channel_index].active)  continue;

    /* Update number of clients, at the interface level */
    for (i=0; i<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; i++)
    {
      /*Check if this position is free*/
      if(snoopEntry->channel_list[channel_index].intIfNum_mask[i] == 0)
      {
        /*Move to Next Position -1 because of the for*/
        i += (sizeof(snoopEntry->channel_list[channel_index].intIfNum_mask[i])*8) -1;
        continue;
      }

      if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].intIfNum_mask,i))
        continue;
      PTIN_DECREMENT_COUNTER(snoopEntry->port_list[i].number_of_clients, snoopEntry->channel_list[channel_index].intf_number_of_clients[i]);
    }
    /* Update global number of clients */
    PTIN_DECREMENT_COUNTER(snoopEntry->global.number_of_clients, snoopEntry->channel_list[channel_index].number_of_clients);

    /* Run all clients, and update number of active channels for each one */
    for (client=0; client<PTIN_SYSTEM_IGMP_MAXCLIENTS; client++)
    {
      /*Check if this position is free*/
      if(snoopEntry->channel_list[channel_index].clients_list[client] == 0)
      {
        /*Move to Next Position -1 because of the for*/
        client += (sizeof(snoopEntry->channel_list[channel_index].clients_list[client])*8) -1;
        continue;
      }

      if (!PTIN_IS_MASKBITSET(snoopEntry->channel_list[channel_index].clients_list,client))  continue;

      /* Decrement number of active channels for this client */
      ptin_igmp_stat_decrement_field(0, vlan, client, SNOOP_STAT_FIELD_ACTIVE_GROUPS);
    }

    /* Clear all clients for this channel */
    snoopEntry->channel_list[channel_index].number_of_clients = 0;
    PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].clients_list);
    PTIN_CLEAR_ARRAY(snoopEntry->channel_list[channel_index].intf_number_of_clients);
  }

#if PTIN_IGMP_DEBUG
  printf("[%s function]\r\n",__FUNCTION__);
  ptin_dump_snoop_entry(snoopEntry);
#endif

  /* Validate arguments */
  return L7_SUCCESS;
}

/**
 * Check if a particular IP exists
 * 
 * @param vlanId    : Internal vlan
 * @param channel   : Channel IP 
 * @param snoopEntry: snooping entry pointer (output) 
 *  
 * @return L7_BOOL : L7_TRUE/L7_FALSE 
 */
L7_BOOL snoopChannelExist4VlanId(L7_uint16 vlanId, L7_inet_addr_t *channel, snoopInfoData_t **snoopEntry)
{
  snoopInfoData_t *entry;
  L7_uint16 channel_index;
  L7_uchar8 dmac[L7_MAC_ADDR_LEN] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };

  /* Verify arguments */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX || channel==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FALSE;
  }

  /* Verify family */
  if (channel->family!=L7_AF_INET)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Only IPv4 family is supported!");
    return L7_FALSE;
  }

  /* IP channel must not be null */
  if (channel->addr.ipv4.s_addr==0)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Channel IP is null!");
    return L7_FALSE;
  }

  /* MAC Address */
  dmac[0] = 0x01;
  dmac[1] = 0x00;
  dmac[2] = 0x5E;
  dmac[3] = (L7_uchar8) ((channel->addr.ipv4.s_addr>>16) & 0x7f);
  dmac[4] = (L7_uchar8) ((channel->addr.ipv4.s_addr>> 8) & 0xff);
  dmac[5] = (L7_uchar8) ((channel->addr.ipv4.s_addr    ) & 0xff);

  /* Channel index */
  channel_index = PTIN_CHANNEL_INDEX_GET(channel);

  /* Find snoop entry */
  if ((entry=snoopEntryFind(dmac,vlanId,L7_AF_INET,AVL_EXACT))==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"VLAN+MAC (%u+%02x:02x:02x:02x:02x:02x) do not exist!",vlanId,dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5]);
    return L7_FALSE;
  }

  /* Output snoop entry pointer */
  if (snoopEntry!=L7_NULLPTR)
  {
    *snoopEntry = entry;
  }

  /* Check if this channel exists */
  return(entry->channel_list[channel_index].active);
}

/**
 * Get IGMP Channels list based either on UNI VLAN or 
 * Client VLAN. 
 * 
 * @param vlanId            UNI VLAN
 * @param client_index      Client index
 * @param channel_list      Channels list (output)
 * @param num_channels      Max number of channels (input) 
 *                          Number of channels (output)
 * @param max_num_channels  Maximum number of channels
 */
void snoopChannelsListGet(L7_uint16 vlanId,
                          L7_uint16 client_index,
                          ptin_igmpChannelInfo_t *channel_list,
                          L7_uint16 *num_channels)
{
  L7_uint16             max_num_channels = L7_MAX_GROUP_REGISTRATION_ENTRIES;
 
  if (num_channels==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return;
  }
 
  snoopChannelsListGet_v2_recursive(L7_NULLPTR, vlanId, client_index, channel_list, num_channels, max_num_channels);
 
  
}

/**
 * Validate snooping arguments for IP channel/client 
 * manipulation 
 * 
 * @param macAddr   : MAC address (should always be not a
 *                    null pointer and start with 01:00:5E)
 * @param vlanId    : Vlan range (should always be between 
 *                    PTIN_VLAN_MIN and PTIN_VLAN_MAX)
 * @param intIfNum  : FP interface# (0 will always be valid)
 * @param IPchannel : IP channel (Null pointer to not validate)
 * @param client    : client index (0 will always be valid)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t snoopValidateArguments(snoopInfoData_t *snoopEntry,
                                      L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint16 client)
{
  L7_uint32 ipv4_addr_23bits;
  L7_uchar8 macAddr[L7_FDB_MAC_ADDR_LEN];
  L7_uint16 vlanId;

  /* Avoid null pointers */
  if (snoopEntry==L7_NULLPTR)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopValidateArguments: snoop entry is a null pointer");
    return L7_FAILURE;
  }

  /* Vlan */
  vlanId = ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8) |
           ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1]);
  /* MAC address */
  memcpy(macAddr,&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN],sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

  /* Validate MAC address 25 MSB */
  if (macAddr[0]!=0x01 || macAddr[1]!=0x00 || macAddr[2]!=0x5E || (macAddr[3] & 0x80)!=0x00)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopValidateArguments: MAC 25 MSb is not 01:00:5E");
    return L7_FAILURE;
  }

  /* Validate vlan range */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopValidateArguments: Vlan %u is out of range",vlanId);
    return L7_FAILURE;
  }

  /* Only validate interface, if is not null */
  /* Validate Interface range */
  if (intIfNum!=L7_ALL_INTERFACES && intIfNum>=PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelRemove: Invalid interface (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Only validate IP if is not a null pointer */
  if (IPchannel!=L7_NULLPTR)
  {
    /* Not ready for IPv6 */
    if (IPchannel->family != L7_AF_INET)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: IPv6 address... not valid!");
      return L7_FAILURE;
    }

    /* Check if this IP address is compatible with MAC entry */
    ipv4_addr_23bits = (((L7_uint32) macAddr[3] & 0x7f)<<16) | ((L7_uint32) macAddr[4]<<8) | ((L7_uint32) macAddr[5]);

    if ((IPchannel->addr.ipv4.s_addr & 0x7fffff)!=ipv4_addr_23bits)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopIPChannelAdd: IP channel is incompatible with this channel group (0x%8X)",IPchannel->addr.ipv4.s_addr);
      return L7_FAILURE;
    }
  }

  /* Validate client range (0 value will always be valid) */
  if (client>=PTIN_SYSTEM_IGMP_MAXCLIENTS)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopClientAdd: Invalid client index (%u)",client);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get IGMP Channels list based either on UNI VLAN or 
 * Client VLAN. 
 * 
 * @param cell_ptr          Pointer to the AVL Tree element
 * @param vlanId            UNI VLAN
 * @param client_index      Client index
 * @param channel_list      Channels list (output)
 * @param num_channels      Number of channels (output) 
 * @param max_num_channels  Maximum number of channels
 */
static void snoopChannelsListGet_v2_recursive(avlTreeTables_t *cell_ptr,
                                              L7_uint16 vlanId,
                                              L7_uint16 client_index,
                                              ptin_igmpChannelInfo_t *channel_list,
                                              L7_uint16 *num_channels,
                                              L7_uint16 max_num_channels)
{
  snoopInfoData_t *entry;
  L7_uint direction;
  avlTreeTables_t *cell_ptr_prev;
  L7_uint16 vlanId_fromAvl;
  L7_uint16 channel_index;

  if (cell_ptr == L7_NULLPTR)
    *num_channels = 0;

  /* Save current pointer to the tree */
  cell_ptr_prev = cell_ptr;

  for (direction = 0; direction <= 1; direction++)
  {

    cell_ptr = cell_ptr_prev;

    /* If maximum number of channels was reached, break for and abandon function */
    if (*num_channels >= max_num_channels)
      break;

    /* Get next entry in the AVL tree */
    entry = (snoopInfoData_t *) avlGetNextEntry(&(snoopEBGet()->snoopAvlTree), &cell_ptr, direction);

    if (cell_ptr != L7_NULLPTR )
    {

      /* Store channel ONLY if it meets svlan or cvlan filtering */
      if (entry != L7_NULLPTR && channel_list != L7_NULLPTR)
      {
        vlanId_fromAvl = ((L7_uint16) entry->snoopInfoDataKey.vlanIdMacAddr[0]<<8) |
                         ((L7_uint16) entry->snoopInfoDataKey.vlanIdMacAddr[1]);
        /* Only process entry, if vlan is the expected (group match) */
        if (vlanId == vlanId_fromAvl)
        {
          /* Run all (active channels) */
          for (channel_index=0; channel_index<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_index++)
          {
            /* If channel is not active, skip to the next one */
            if (!entry->channel_list[channel_index].active)
              continue;
            /* If client_index is provided, only consider channels used by this client index */
            if (client_index<PTIN_SYSTEM_IGMP_MAXCLIENTS &&
                !PTIN_IS_MASKBITSET(entry->channel_list[channel_index].clients_list,client_index))
              continue;
            channel_list[*num_channels].groupAddr.family            = L7_AF_INET;
            channel_list[*num_channels].groupAddr.addr.ipv4.s_addr  = entry->channel_list[channel_index].ipAddr;
            channel_list[*num_channels].static_type                 = entry->staticGroup;
            inetAddressReset(&channel_list[*num_channels].sourceAddr);
            (*num_channels)++;
          }
        }
      }

      /* Recursive to the left */
      snoopChannelsListGet_v2_recursive(cell_ptr, vlanId, client_index, channel_list, num_channels, max_num_channels);

      if (cell_ptr_prev==L7_NULLPTR)
        break;
    }
  }
}

/**
 * Get IGMP Channels list based either on UNI VLAN or 
 * Client VLAN. 
 * 
 * @param cell_ptr          Pointer to the AVL Tree element
 * @param vlanId            UNI VLAN 
 * @param vlanId            intIfNum 
 * @param client_index      Client index
 * @param channel_list      Channels list (output)
 * @param num_channels      Number of channels (output) 
 * @param max_num_channels  Maximum number of channels
 */
void snoopChannelsGet(L7_uint16 vlanId,
                             L7_uint32 intIfNum,
                             L7_uint16 client_index,
                             ptin_igmpChannelInfo_t *channel_list,
                             L7_uint16 *num_channels)
{
  snoopPTinL3InfoDataKey_t avlTreeKey;
  snoopPTinL3InfoData_t    *avlTreeEntry;
  snoop_eb_t               *pSnoopEB;
  L7_uint32                max_num_channels;
  char                     debug_buf[IPV6_DISP_ADDR_LEN]={};
  L7_BOOL                  channelAdded=L7_FALSE;

  if (channel_list == L7_NULLPTR || num_channels==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return;
  }

  max_num_channels = L7_MAX_GROUP_REGISTRATION_ENTRIES*PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP;
  *num_channels    = 0;

  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopEBGet()");
    return;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Starting IGMP channel search (vlanId:%u clientidx:%u max_num_channels:%u)",vlanId,client_index,max_num_channels);

  /* Run all cells in AVL tree */    
  memset(&avlTreeKey,0x00,sizeof(snoopPTinL3InfoDataKey_t));
  while ( ( avlTreeEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &avlTreeKey, L7_MATCH_GETNEXT) ) != L7_NULLPTR )
  {

    /* Prepare next key */
    memcpy(&avlTreeKey, &avlTreeEntry->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));

    /* If maximum number of channels was reached, break */
    if (*num_channels >= max_num_channels)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Max number of channels reached...stopping search");
      break;  
    }

    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Group Address Number %u",*num_channels);

    //Copy group/source if vlans match
    if (vlanId == avlTreeKey.vlanId &&       
        avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_TRUE && 
        snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients)==L7_ALREADY_CONFIGURED)
    {
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Found group :%s", inetAddrPrint(&avlTreeEntry->snoopPTinL3InfoDataKey.mcastGroupAddr, debug_buf));

      snoopPTinL3Interface_t  *interface_ptr;
      L7_uint8                sourceIdx;

      LOG_TRACE(LOG_CTX_PTIN_IGMP,"\tInterface:%u Clients:0x%0*X", 8*PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);

      //Add an entry for clients that have requested this group but with no source in particular.
      interface_ptr = &avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM];

      
      for (sourceIdx=0; sourceIdx < PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP; ++sourceIdx)
      {
        snoopPTinL3Source_t *source_ptr;

        source_ptr = &interface_ptr->sources[sourceIdx];        

#if 0
//Only consider sources for which traffic forwarding is enabled
        if (snoop_ptin_sourcetimer_isRunning(&source_ptr->sourceTimer) == L7_FALSE)
        {
          continue;
        }

        //Filter by client (if requested)
        if ((client_index == (L7_uint16)-1) || (PTIN_IS_MASKBITSET(interface_ptr->clients, client_index)))
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"\t\tSource:0x%08X Clients:0x%0*X", 8*PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE, source_ptr->sourceAddr);
          inetCopy(&channel_list[*num_channels].groupAddr, &avlTreeKey.mcastGroupAddr);
          inetCopy(&channel_list[*num_channels].sourceAddr, &source_ptr->sourceAddr);
          ++(*num_channels);
        }
#else
//Only consider sources for which traffic forwarding is enabled
        if (avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].status==PTIN_SNOOP_SOURCESTATE_ACTIVE &&  
            avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].sourceTimer.isRunning==L7_TRUE &&
            snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].sources[sourceIdx].clients)==L7_ALREADY_CONFIGURED)
        { 
          //Filter by client (if requested)
          if ((client_index == (L7_uint16)-1) || (PTIN_IS_MASKBITSET(avlTreeEntry->interfaces[intIfNum].sources[sourceIdx].clients, client_index)))
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"\t\tSource:0x%08X Clients:0x%0*X", 8*PTIN_SYSTEM_IGMP_CLIENT_BITMAP_SIZE, source_ptr->sourceAddr);
            inetCopy(&channel_list[*num_channels].groupAddr, &avlTreeKey.mcastGroupAddr);
            inetCopy(&channel_list[*num_channels].sourceAddr, &source_ptr->sourceAddr);
            /* If group address is static, get static information to source channel */
            if (avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic)
            {
              channel_list[*num_channels].static_type = source_ptr->isStatic;
            }
            else
            {
              channel_list[*num_channels].static_type = L7_FALSE;
            }
            ++(*num_channels);
            channelAdded=L7_TRUE;
          }
        }
#endif        
      }
      if (interface_ptr->numberOfClients != 0)
      {
        //Filter by client (if requested)
        if (channelAdded==L7_FALSE && ((client_index == (L7_uint16)-1 )  || (PTIN_IS_MASKBITSET(avlTreeEntry->interfaces[intIfNum].clients, client_index))))
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"\t\tSource: ANY_SOURCE");
          inetCopy(&channel_list[*num_channels].groupAddr, &avlTreeKey.mcastGroupAddr);
          inetAddressReset(&channel_list[*num_channels].sourceAddr);
          channel_list[*num_channels].static_type = avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].isStatic;
          ++(*num_channels);          
        }
      }
      if(channelAdded==L7_TRUE)
        channelAdded=L7_FALSE;
    }
  }
}

  #if PTIN_IGMP_DEBUG
static void ptin_dump_snoop_entry(snoopInfoData_t *snoopEntry)
{
  L7_uchar8 macAddr[L7_FDB_MAC_ADDR_LEN];
  L7_uint16 vlanId, i, j;

  if (snoopEntry==L7_NULLPTR)
  {
    return;
  }

  vlanId = ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8) |
           ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1]);
  memcpy(macAddr,&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN],sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

  printf("VLAN=%u, MAC=%02X:%02X:%02X:%02X:%02X:%02X\r\n",vlanId,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
  printf("  Global: #ports=%u, #channels=%u, #clients=%u\r\n",snoopEntry->global.number_of_ports,snoopEntry->global.number_of_channels,snoopEntry->global.number_of_clients);
  printf("  Port list:\r\n");
  for (i=0; i<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; i++)
  {
    if (!snoopEntry->port_list[i].active)  continue;
    printf("    IntIfNum=%u: #channels=%u, #clients=%u\r\n",i,snoopEntry->port_list[i].number_of_channels,snoopEntry->port_list[i].number_of_clients);
  }
  printf("  Channel list:\r\n");
  for (i=0; i<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; i++)
  {
    if (!snoopEntry->channel_list[i].active)  continue;
    printf("    IPv4addr=%u.%u.%u.%u\r\n",(snoopEntry->channel_list[i].ipAddr>>24) & 0xff,(snoopEntry->channel_list[i].ipAddr>>16) & 0xff,(snoopEntry->channel_list[i].ipAddr>>8) & 0xff,snoopEntry->channel_list[i].ipAddr & 0xff);
    printf("    #ports=%u, #clients=%u\r\n",snoopEntry->channel_list[i].number_of_ports,snoopEntry->channel_list[i].number_of_clients);
    printf("    Intf list: ");
    for (j=0; j<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; j++)
    {
      if (PTIN_IS_MASKBITSET(snoopEntry->channel_list[i].intIfNum_mask,j))
        printf("%u ",j);
    }
    printf("\r\n");
    printf("    Clients list: ");
    for (j=0; j<PTIN_SYSTEM_IGMP_MAXCLIENTS; j++)
    {
      if (PTIN_IS_MASKBITSET(snoopEntry->channel_list[i].clients_list,j))
        printf("%u ",j);
    }
    printf("\r\n");
    printf("    #clients(intf): ");
    for (j=0; j<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; j++)
    {
      if (snoopEntry->channel_list[i].intf_number_of_clients[j]>0)
        printf("intf%u=>%u ",j,snoopEntry->channel_list[i].intf_number_of_clients[j]);
    }
    printf("\r\n");
  }
}
  #endif

/**
 * Dumps Snoop Table
 * If index is -1, all entries are dumped 
 * 
 * @param index 
 */
void ptin_igmp_snoop_dump(L7_uint16 index)
{
  L7_BOOL   isProtection = 0;
  L7_uint16 intVlan;
  L7_uint32 intIfNum;
  L7_uint i, j, i_client;
  snoopInfoData_t *avl_info;
  snoopInfoDataKey_t avl_key, *key;
  ptin_intf_t ptin_intf;
  ptin_client_id_t clientData;

  if (index>=L7_MAX_GROUP_REGISTRATION_ENTRIES)
  {
    printf("Showing all Snooping entries\r\n");
  }
  else
  {
    printf("Showing Snooping entry %u\r\n",index);
  }

  /* First entry */
  i_client = 0;
  memset(&avl_key,0x00,sizeof(snoopInfoDataKey_t));
  while ( ( avl_info = (snoopInfoData_t *)
            avlSearchLVL7( &(snoopEBGet()->snoopAvlTree), (void *) &avl_key, AVL_NEXT) 
          ) != L7_SUCCESS )
  {
    /* Prepare next key */
    memcpy( &avl_key, &avl_info->snoopInfoDataKey, sizeof(snoopInfoDataKey_t) );

    /* Show all entries */
    if (index>=L7_MAX_GROUP_REGISTRATION_ENTRIES)
    {
      key  = &avl_info->snoopInfoDataKey;
      printf("Entry#%-4u: Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s\r\n",
             i_client,
             ((key->family==L7_AF_INET) ? "IPv4" : "IPv6"),
             ((L7_uint16) key->vlanIdMacAddr[0]<<8 | (L7_uint16) key->vlanIdMacAddr[1]),
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5],
             ((avl_info->staticGroup) ? "Static" : "Dynamic"));
      printf("            #ports=%-5u    #channels=%-5u    #clients=%-5u\r\n",
             avl_info->global.number_of_ports,
             avl_info->global.number_of_channels,
             avl_info->global.number_of_clients);
    }
    /* Show one particular entry */
    else if (i_client==index)
    {
      key  = &avl_info->snoopInfoDataKey;
      intVlan = (L7_uint16) key->vlanIdMacAddr[0]<<8 | (L7_uint16) key->vlanIdMacAddr[1];
      /* Global information */
      printf("Entry#%-4u:\r\n",index);
      printf("  Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s\r\n",
             ((key->family==L7_AF_INET) ? "IPv4" : "IPv6"),
             intVlan,
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4],
             key->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5],
             ((avl_info->staticGroup) ? "Static" : "Dynamic"));
      printf("  #ports=%-5u    #channels=%-5u    #clients=%-5u\r\n",
             avl_info->global.number_of_ports,
             avl_info->global.number_of_channels,
             avl_info->global.number_of_clients);
      /* Ports information */
      printf("  Ports information:\r\n");
      printf("  IntfNUm:");
      uint32 intf;
      for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
      {
       if (L7_INTF_ISMASKBITSET(avl_info->snoopGrpMemberList,intf))
       {
         printf(" %u",intf);
       }
      }
      printf("\r\n");
      
      printf("    port[#channels]=");
      for (intIfNum=0; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
      {
        if (!avl_info->port_list[intIfNum].active)  continue;
        if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)  continue;
        printf(" %u/%u[%u]",ptin_intf.intf_type,ptin_intf.intf_id,avl_info->port_list[intIfNum].number_of_channels);
      }
      printf("\r\n");
      printf("    port[#clients ]=");
      for (intIfNum=0; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
      {
        if (!avl_info->port_list[intIfNum].active)  continue;
        if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)  continue;
        printf(" %u/%u[%u]",ptin_intf.intf_type,ptin_intf.intf_id,avl_info->port_list[intIfNum].number_of_clients);
      }
      printf("\r\n");
      /* Channels information */
      printf("  Channels information:\r\n");
      for (i=0; i<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; i++)
      {
        if (!avl_info->channel_list[i].active)  continue;
        printf("    Channel#%-2u:       IpAddr=%u.%u.%u.%u\r\n",i,
               (avl_info->channel_list[i].ipAddr>>24) & 0xff,
               (avl_info->channel_list[i].ipAddr>>16) & 0xff,
               (avl_info->channel_list[i].ipAddr>>8) & 0xff,
               avl_info->channel_list[i].ipAddr & 0xff);
        printf("      #ports=%-5u    #clients=%u\r\n",
               avl_info->channel_list[i].number_of_ports,
               avl_info->channel_list[i].number_of_clients);
        printf("      Port[Protection][#clients]:");
        for (intIfNum=0; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
        {
          if (!PTIN_IS_MASKBITSET(avl_info->channel_list[i].intIfNum_mask,intIfNum))  continue;
          if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)  continue;
          #if PTIN_BOARD_IS_MATRIX
          isProtection = PTIN_IS_MASKBITSET(avl_info->channel_list[i].protection_mask,intIfNum);
          #endif
          printf(" %u/%u[%u][%u]",ptin_intf.intf_type,ptin_intf.intf_id, isProtection, avl_info->channel_list[i].intf_number_of_clients[intIfNum]);
        }
        printf("\r\n");
        printf("      Clients list:");
        for (intIfNum=0; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
        {
          if (!PTIN_IS_MASKBITSET(avl_info->channel_list[i].intIfNum_mask, intIfNum))  continue;

          for (j=0; j<PTIN_SYSTEM_IGMP_MAXCLIENTS; j++)
          {
            if (!PTIN_IS_MASKBITSET(avl_info->channel_list[i].clients_list,j))  continue;
            if (ptin_igmp_clientData_get(intIfNum, j, &clientData) != L7_SUCCESS)
            {
              printf(" {id=%u}",j);
            }
            else
            {
              printf(" {");
              if (clientData.mask & PTIN_CLIENT_MASK_FIELD_INTF)
                printf("intf=%u/%u",clientData.ptin_intf.intf_type,clientData.ptin_intf.intf_id);
              if (clientData.mask & PTIN_CLIENT_MASK_FIELD_OUTERVLAN)
                printf(",ovid=%u",clientData.outerVlan);
              if (clientData.mask & PTIN_CLIENT_MASK_FIELD_INNERVLAN)
                printf(",ivid=%u",clientData.innerVlan);
              if (clientData.mask & PTIN_CLIENT_MASK_FIELD_IPADDR)
                printf(",ipAddr=%u.%u.%u.%u",(clientData.ipv4_addr>>24) & 0xff,(clientData.ipv4_addr>>16) & 0xff,(clientData.ipv4_addr>>8) & 0xff,clientData.ipv4_addr & 0xff);
              if (clientData.mask & PTIN_CLIENT_MASK_FIELD_MACADDR)
                printf(",mac=%02x:%02x:%02x:%02x:%02x:%02x",clientData.macAddr[0],clientData.macAddr[1],clientData.macAddr[2],clientData.macAddr[3],clientData.macAddr[4],clientData.macAddr[5]);
              printf("}");
            }
          }
        }
        printf("\r\n");
      }
    }

    /* Next entry */
    i_client++;
  }

  fflush(stdout);
}

/**
 * Dumps MFDB Table 
 */
void ptin_igmp_mfdb_dump(void)
{
  ptin_intf_t ptin_intf;
  L7_uint32 intIfNum;
  L7_uint i=0, n_entries;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  usmdbMfdbUserInfo_t info;

  /* Get number of used entries */
  usmDbMfdbCurrEntriesGet(1,&n_entries);

  printf("MFDB table: %u entries\r\n",n_entries);

  memset(vidMac,0x00,sizeof(vidMac));
  while (usmDbMfdbComponentEntryGetNext(1,vidMac,L7_MFDB_PROTOCOL_IGMP,&info)==L7_SUCCESS)
  {
    printf("Entry#%-4u: Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x    Type=%s    Desc=\"%s\"\r\n",
           i,
           (L7_uint16) vidMac[0]<<8 | (L7_uint16) vidMac[1],
           vidMac[L7_MFDB_VLANID_LEN+0],
           vidMac[L7_MFDB_VLANID_LEN+1],
           vidMac[L7_MFDB_VLANID_LEN+2],
           vidMac[L7_MFDB_VLANID_LEN+3],
           vidMac[L7_MFDB_VLANID_LEN+4],
           vidMac[L7_MFDB_VLANID_LEN+5],
           ((info.usmdbMfdbType==L7_MFDB_TYPE_STATIC) ? "Static " : "Dynamic"),
           info.usmdbMfdbDescr);
    printf("  Forward ports:");
    for (intIfNum=1; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
    {
      if (!L7_INTF_ISMASKBITSET(info.usmdbMfdbFwdMask,intIfNum))  continue;
      if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)  continue; 
      #if 0          
      printf(" %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      #else      
      printf(" %u",intIfNum);
      #endif
    }
    printf("\r\n");
    printf("  Filter ports :");
    for (intIfNum=1; intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP; intIfNum++)
    {
      if (!L7_INTF_ISMASKBITSET(info.usmdbMfdbFltMask,intIfNum))  continue;
      if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)  continue;
      #if 0      
      printf(" %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      #else
      printf(" %u",intIfNum);
      #endif
    }
    printf("\r\n");
    i++;
  }

  fflush(stdout);
}

#endif

/*********************************************************************
* @purpose  Removes an Multicast MAC group membership entry
*
* @param    macAddr  @b{(input)}   Multicast MAC Address
* @param    vlanId    @b{(input)}  Vlan ID
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopEntryRemove(L7_uchar8 *macAddr, L7_uint32 vlanId,
                         L7_uchar8 family)
{
  snoopInfoData_t  *snoopEntry;
  mfdbMemberInfo_t  mfdb;
  L7_RC_t           rc = L7_SUCCESS;
  L7_uint           igmp_network_version;

  snoopEntry = snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT);

  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* PTin added: IGMP snooping */
#if 1
  L7_uint channel_idx;
  L7_inet_addr_t channel;

  /* Get igmp network version */
  igmp_network_version = snoopCheckPrecedenceParamGet(vlanId, L7_ALL_INTERFACES, SNOOP_PARAM_IGMP_NETWORK_VERSION, L7_AF_INET);

  /* Remove all channels (and send leaves) */
  for (channel_idx=0; channel_idx<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_idx++)
  {
    if (!snoopEntry->channel_list[channel_idx].active)  continue;

    channel.family = L7_AF_INET;
    channel.addr.ipv4.s_addr = snoopEntry->channel_list[channel_idx].ipAddr;

    /* Remove all clients for this channel */
    if (snoopChannelClientsRemoveAll(snoopEntry,&channel)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing clients from channel");
    }

    /* Remove this channel */
    if (snoopChannelDelete(snoopEntry,&channel)!=L7_SUCCESS)
    {
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP,"snoopChannelDelete failed");
      continue;
    }

    /* Send two leave messages */
    if (igmp_network_version <= 2)
    {
      if (igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,&channel)!=L7_SUCCESS /*||
          igmp_generate_packet_and_send(vlanId,L7_IGMP_V2_LEAVE_GROUP,&channel)!=L7_SUCCESS*/)
      {
        if (ptin_debug_igmp_snooping)
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Error sending leaves to router interfaces");
        continue;
      }
    }
  }
#endif

  /* Entry is found... delete it from the MFDB */
  memset( (L7_uchar8 *)&mfdb, 0, sizeof(mfdb) );
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP
                          : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (!snoopEntry->staticGroup) ? L7_MFDB_TYPE_DYNAMIC : L7_MFDB_TYPE_STATIC;  /* PTin modified: IGMP snooping */
  memcpy((void *)mfdb.user.description,(void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  (void)mfdbEntryDelete(&mfdb);

  /* Not checking for return code. Remove the snoopEntry even
   * if mfdbEntryDelete returns failure */
  rc = snoopEntryDelete(macAddr, vlanId, family);

  return rc;
}
/******************************************************************************
* @purpose  Gets the first learnt Multicast MAC Address and VLAN ID
*
* @param    macAddr   @b{(output)} Multicast MAC Address
* @param    vlanId    @b{(output)} Vlan ID for the group entry
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there are no entries
*
* @notes    macAddr is returned as L7_NULL if there are no entries
*
* @end
******************************************************************************/
L7_RC_t snoopFirstGet(L7_uchar8 *macAddr, L7_uint32 *vlanId, L7_uchar8 family)
{
  snoopInfoData_t *snoopEntry = L7_NULLPTR;
  L7_uchar8        addr[L7_MAC_ADDR_LEN];
  L7_ushort16      vid = 1;

  memset((void *)addr, 0x00, L7_MAC_ADDR_LEN);
  vid = L7_NULL;

  snoopEntry = snoopEntryFind(addr, 1, family, L7_MATCH_GETNEXT);
  if (snoopEntry)
  {
    /*first 2 bytes are vlan id*/
    memcpy((void *)&vid,
           (void *)&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0], 2);
    *vlanId = (L7_uint32)vid;
    /*next six bytes are mac address*/
    memcpy((void *)macAddr,
           (void *)&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[2],
           L7_MAC_ADDR_LEN);

    return L7_SUCCESS;
  }
  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  *vlanId = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the next Multicast MAC Address and VLAN ID
*
* @param    macAddr     @b{(input)}  Multicast MAC Address
* @param    vlanId      @b{(input)}  Vlan ID for the MAC Address
* @param    nextAddr    @b{(output)}  Next MAC Address
* @param    nextVlanId  @b{(output)} Vlan ID for the next MAC Address
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there are no more group entries
*
* @notes    nextAddr is returned as L7_NULL if there is no more group entries
*
* @end
*********************************************************************/
L7_RC_t snoopNextGet(L7_uchar8  *macAddr,L7_uint32  vlanId,L7_uchar8  *nextAddr,
                     L7_uint32  *nextVlanId, L7_uchar8 family)
{
  snoopInfoData_t  *nextSnoopEntry = L7_NULLPTR;
  L7_ushort16       nextVid = 0;

  nextSnoopEntry = snoopEntryFind(macAddr, vlanId, family, L7_MATCH_GETNEXT);

  if (nextSnoopEntry == L7_NULLPTR)
  {
    memset((void *)nextAddr, 0, L7_MAC_ADDR_LEN);
    *nextVlanId = 0;
    return L7_FAILURE;
  }

  /*get the next entries values*/
  /*next Entry is valid*/
  /*first 2 bytes are vlan id*/
  memcpy((void *)&nextVid,
         (void *)&nextSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[0], 2);
  *nextVlanId = (L7_uint32)nextVid;

  /*next six bytes are mac address*/
  memcpy((void *)nextAddr,
         (void *)&nextSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[2],
         L7_MAC_ADDR_LEN);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a snoop operational entry for specified vlan
*
* @param    vlanId     @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes     Create entries one for each snoop instance
*
* @end
*********************************************************************/
L7_RC_t snoopOperEntryAdd(L7_ushort16 vlanId)
{
  snoopOperData_t *pData = L7_NULLPTR;
  snoopOperData_t  snoopOperEntry;
  snoop_cb_t      *pSnoopCB;
  snoop_eb_t      *pSnoopEB;
  L7_uint32        cbIndex = 0, maxInstances;
  L7_RC_t          rc = L7_SUCCESS;

  maxInstances = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();
  pSnoopEB = snoopEBGet();

  for (cbIndex = L7_NULL; cbIndex < maxInstances; cbIndex++, pSnoopCB++)
  {
    memset(&snoopOperEntry, 0x00, sizeof(snoopOperData_t));
    snoopOperEntry.vlanId = (L7_ushort16)vlanId;
    snoopOperEntry.cbHandle = (void *)pSnoopCB;

    osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
    pData = avlInsertEntry(&pSnoopCB->snoopOperDataAvlTree, &snoopOperEntry);
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);

    if (pData == &snoopOperEntry)
    {
      /*some error in avl tree addition*/
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopOperEntryAdd: Failed to add vlan id %d family %d",
              vlanId, pSnoopCB->family);
      rc = L7_FAILURE;
      return rc;
    }

  }/* End of snoop instance iterations */

  return rc;
}
/*********************************************************************
* @purpose  Delete a snoop operational entry for specified vlan
*
* @param    vlanId     @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Delete entries one for each snoop instance. Also stop
*           any running timers
*
* @end
*********************************************************************/
L7_RC_t snoopOperEntryDelete(L7_ushort16 vlanId)
{
  snoopOperData_t        *pData = L7_NULLPTR;
  snoop_cb_t             *pSnoopCB;
  snoop_eb_t             *pSnoopEB;
  L7_uint32               cbIndex = 0, maxInstances;
  snoopOperData_t        *pSnoopOperEntry;
  L7_RC_t                 rc = L7_SUCCESS;
  snoopMrtrTimerData_t   *pmrtrTimerData;
  snoopMrtrTimerDataKey_t key;

  maxInstances = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();
  pSnoopEB = snoopEBGet();

  for (cbIndex = L7_NULL; cbIndex < maxInstances; cbIndex++, pSnoopCB++)
  {
    osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
    pData = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT);

    if (pData == L7_NULLPTR)
    {
      osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
      rc = L7_NOT_EXIST;
      continue;
    }

    pSnoopOperEntry = pData;

    /* Stop all Mrtr timers for this vlan ID */
    while (L7_TRUE)
    {
      key.intIfNum = L7_NULL;
      key.vlanId   = vlanId;

      pmrtrTimerData = (snoopMrtrTimerData_t *)
                       avlSearchLVL7(&pSnoopCB->snoopMrtrTimerAvlTree, &key,
                                     L7_MATCH_GETNEXT);
      if (pmrtrTimerData)
      {
        if (pmrtrTimerData->snoopMrtrTimerDataKey.vlanId == vlanId)
        {
          key.vlanId   = pmrtrTimerData->snoopMrtrTimerDataKey.vlanId;
          key.intIfNum = pmrtrTimerData->snoopMrtrTimerDataKey.intIfNum;

          snoopTimerStop(L7_NULL, key.intIfNum, vlanId ,
                         SNOOP_MCAST_ROUTER_ATTACHED, pSnoopCB);
        }
        else
        {
          break;
        }
      }
      else
      {
        break;
      }
    }

    /* Stop Querier Timers */
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer != L7_NULL)
    {
      snoopQuerierTimerStop(pSnoopOperEntry, SNOOP_QUERIER_QUERIER_EXPIRY_TIMER);
    }

    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer != L7_NULL)
    {
      snoopQuerierTimerStop(pSnoopOperEntry, SNOOP_QUERIER_QUERY_INTERVAL_TIMER);
    }

    pData = avlDeleteEntry(&pSnoopCB->snoopOperDataAvlTree, pData);
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);

    if (pData == L7_NULL)
    {
      /* Entry does not exist */
      rc = L7_FAILURE;
    }
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_OD_DEL_DATA, NULL, vlanId,
                               pSnoopCB->family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */
  } /* End of snoop Instance iterations */

  return rc;
}
/*********************************************************************
* @purpose  Get first snoop operational entry of a snoop instance
*
* @param    pSnoopCB     @b{(input)} Control Block
*
* @returns  Pointer to the first operational entry
* @returns  Null pointer in case of failure
*
* @notes    none
*
* @end
*********************************************************************/
snoopOperData_t *snoopOperEntryFirstGet(snoop_cb_t *pSnoopCB)
{
  return snoopOperEntryGet(0, pSnoopCB, L7_MATCH_GETNEXT);
}
/*********************************************************************
* @purpose  Get snoop operational entry for a specified vlan and
*           a snoop instance
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    pSnoopCB   @b{(input)} Control Block
* @param    flag       @b{(input)} Exact or Next entry
*
* @returns  L7_SUCCESS  Match found
* @returns  L7_FAILURE  No such entry found
*
* @notes    none
*
* @end
*********************************************************************/
snoopOperData_t *snoopOperEntryGet(L7_ushort16 vlanId, snoop_cb_t *pSnoopCB,
                                   L7_uint32 flag)
{
  snoopOperData_t    *pSnoopOperEntry = L7_NULLPTR;

  pSnoopOperEntry = avlSearchLVL7(&pSnoopCB->snoopOperDataAvlTree,
                                  (L7_uchar8 *)&vlanId, flag);
  if (pSnoopOperEntry == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  else
  {
    return pSnoopOperEntry;
  }
}
/*********************************************************************
* @purpose  De-Initialize a snoop operational entry for specified
*           entry
*
* @param    pSnoopOperEntry     @b{(input)} Snoop Operational entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Clear operational info. Also stop any running timers
*
* @end
*********************************************************************/
void snoopOperEntryDeInit(snoopOperData_t  *pSnoopOperEntry)
{
  if (pSnoopOperEntry != L7_NULLPTR)
  {
    /* Stop Timers */
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer
        != L7_NULL)
    {
      snoopQuerierTimerStop(pSnoopOperEntry, SNOOP_QUERIER_QUERIER_EXPIRY_TIMER);
    }

    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer
        != L7_NULL)
    {
      snoopQuerierTimerStop(pSnoopOperEntry, SNOOP_QUERIER_QUERY_INTERVAL_TIMER);
    }

    memset(&pSnoopOperEntry->snoopQuerierInfo, 0x00, sizeof(snoopQuerierInfo_t));
    inetAddressZeroSet(((snoop_cb_t *)(pSnoopOperEntry->cbHandle))->family,
                       &pSnoopOperEntry->snoopQuerierInfo.querierAddress);
  }
}
#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  Get the outgoing Vlan rtr interfaces for give group addr
*
*
* @param    mcastMacAddr       @b{(input)} Multicast MAC Address
* @param    inVlanRtrIntfMask @b{(output)} Union of all ingress vlan routing
*                                          intfs for this group address
* @param    outVlanRtrIntfMask @b{(output)} Union of all vlan routing
*                                          intfs for this group address
*
* @returns  If atleast one src or dst vlan is found returns
*           flag SNOOP_L3_INGRESS_VLAN_MASK_PRESENT or
*                SNOOP_L3_EGRESS_VLAN_MASK_PRESENT.
*           Else
*               returns NULL
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 snoopL3EntryInOutVlanMaskGet(L7_uchar8 *mcastMacAddr,
                                       L7_VLAN_MASK_t *inVlanRtrIntfMask,
                                       L7_VLAN_MASK_t *outVlanRtrIntfMask)
{
  snoopL3InfoData_t    *snoopL3Entry;
  snoopL3InfoDataKey_t  key;
  snoop_eb_t           *pSnoopEB;
  L7_BOOL               flag = L7_FALSE;
  L7_VLAN_MASK_t        tempInVlanMask;
  L7_VLAN_MASK_t        tempOutVlanMask;
  L7_uint32             result = L7_NULL;

  pSnoopEB = snoopEBGet();

  memset((void *)&key, 0x00, sizeof(snoopL3InfoDataKey_t));
  memcpy(key.macAddrSuffix, mcastMacAddr+SNOOP_MAC_ADDR_PREFIX_LEN,
         SNOOP_MAC_ADDR_SUFFIX_LEN);
  memset(&tempInVlanMask, 0x00, sizeof(L7_VLAN_MASK_t));
  memset(&tempOutVlanMask, 0x00, sizeof(L7_VLAN_MASK_t));

  snoopL3Entry = avlSearchLVL7(&pSnoopEB->snoopL3AvlTree, &key, AVL_NEXT);
  while (snoopL3Entry)
  {
    if (memcmp(mcastMacAddr+(SNOOP_MAC_ADDR_PREFIX_LEN),
               snoopL3Entry->snoopL3InfoDataKey.macAddrSuffix,
               SNOOP_MAC_ADDR_SUFFIX_LEN) == 0)
    {
      flag = L7_TRUE;
      L7_VLAN_MASKOREQ(tempOutVlanMask, snoopL3Entry->outVlanRtrIntfMask);
      if (snoopL3Entry->srcIntfVlan)
      {
        L7_VLAN_SETMASKBIT(tempInVlanMask, snoopL3Entry->srcIntfVlan);
      }
    }
    else
    {
      break;
    }
    memcpy(&key.mcastGroupAddr, &snoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr,
           sizeof(L7_inet_addr_t));
    memcpy(&key.mcastSrcAddr, &snoopL3Entry->snoopL3InfoDataKey.mcastSrcAddr,
           sizeof(L7_inet_addr_t));
    snoopL3Entry = avlSearchLVL7(&pSnoopEB->snoopL3AvlTree, &key, AVL_NEXT);
  }

  if (flag == L7_TRUE)
  {
    L7_VLAN_NONZEROMASK(tempOutVlanMask, flag);
    if (flag)
    {
      memcpy(outVlanRtrIntfMask, &tempOutVlanMask, sizeof(L7_VLAN_MASK_t));
      result |= SNOOP_L3_EGRESS_VLAN_MASK_PRESENT;
    }

    L7_VLAN_NONZEROMASK(tempInVlanMask, flag);
    if (flag)
    {
      result |= SNOOP_L3_INGRESS_VLAN_MASK_PRESENT;
      memcpy(inVlanRtrIntfMask, &tempInVlanMask, sizeof(L7_VLAN_MASK_t));
    }
  }
  return result;
}
/*********************************************************************
* @purpose  Finds an Multicast group membership entry in L3 notified
*           databse
*
* @param    macAddr  @b{(input)} Multicast MAC Address
* @param    flag     @b{(input)} Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
* @returns  null pointer on failure
* @returns  multicast entry pointer on success
*
* @notes    none
*
* @end
*********************************************************************/
snoopL3InfoData_t *snoopL3EntryFind(L7_inet_addr_t *mcastGroupAddr,
                                    L7_inet_addr_t *mcastSrcAddr,
                                    L7_uint32 flag)
{
  snoopL3InfoData_t    *snoopL3Entry;
  snoopL3InfoDataKey_t  key;
  snoop_eb_t           *pSnoopEB;
  L7_uchar8             mcastMacAddr[L7_MAC_ADDR_LEN];

  pSnoopEB = snoopEBGet();
  memset((void *)&key, 0x00, sizeof(snoopL3InfoDataKey_t));

  snoopMulticastMacFromIpAddr(mcastGroupAddr, mcastMacAddr);
  memcpy(key.macAddrSuffix, &mcastMacAddr[SNOOP_MAC_ADDR_PREFIX_LEN],
         SNOOP_MAC_ADDR_SUFFIX_LEN);
  memcpy(&key.mcastGroupAddr, mcastGroupAddr, sizeof(L7_inet_addr_t));
  memcpy(&key.mcastSrcAddr, mcastSrcAddr, sizeof(L7_inet_addr_t));
  snoopL3Entry = avlSearchLVL7(&pSnoopEB->snoopL3AvlTree, &key, flag);

  if (snoopL3Entry == L7_NULL)
  {
    return L7_NULLPTR;
  }
  else
  {
    return snoopL3Entry;
  }
}
/*********************************************************************
* @purpose  Adds a L3 Notified Multicast MAC entry to snoop database
*
* @param    mcastGroupAddr     @b{(input)} Multicast Grp Address
* @param    mcastSrcAddr       @b{(input)} Multicast Source Address
* @param    srcVlan            @b{(input)} Ingress VLAN Interface
* @param    outVlanRtrIntfMask @b{(input)} Vlan Rtr interfaces mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopL3EntryAdd(L7_inet_addr_t *mcastGroupAddr,
                        L7_inet_addr_t *mcastSrcAddr,
                        L7_int32        srcVlan,
                        L7_VLAN_MASK_t  *outVlanRtrIntfMask)
{
  snoopL3InfoData_t    snoopL3Entry;
  snoopL3InfoData_t   *pData;
  snoop_eb_t          *pSnoopEB;
  L7_uchar8            mcastMacAddr[L7_MAC_ADDR_LEN];

  memset(&snoopL3Entry, 0x00, sizeof(snoopL3InfoData_t));
  pSnoopEB = snoopEBGet();
  snoopMulticastMacFromIpAddr(mcastGroupAddr, mcastMacAddr);
  memcpy(snoopL3Entry.snoopL3InfoDataKey.macAddrSuffix,
         &mcastMacAddr[SNOOP_MAC_ADDR_PREFIX_LEN], SNOOP_MAC_ADDR_SUFFIX_LEN);

  memcpy(&snoopL3Entry.snoopL3InfoDataKey.mcastGroupAddr, mcastGroupAddr,
         sizeof(L7_inet_addr_t));

  memcpy(&snoopL3Entry.snoopL3InfoDataKey.mcastSrcAddr, mcastSrcAddr,
         sizeof(L7_inet_addr_t));

  snoopL3Entry.srcIntfVlan = srcVlan;
  memcpy(&snoopL3Entry.outVlanRtrIntfMask, outVlanRtrIntfMask,
         sizeof(L7_VLAN_MASK_t));

  pData = avlInsertEntry(&pSnoopEB->snoopL3AvlTree, &snoopL3Entry);

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/

#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback2(SNOOP_CKPT_MSG_L3GA_ADD_DATA, snoopL3Entry.snoopL3InfoDataKey.macAddrSuffix,
                               *mcastGroupAddr, *mcastSrcAddr, srcVlan, outVlanRtrIntfMask);
    }
#endif /* L7_NSF_PACKAGE */

    return L7_SUCCESS;
  }

  if (pData == &snoopL3Entry)
  {
    /*some error in avl tree addition*/
    return L7_FAILURE;
  }

  /*entry already exists*/
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Removes a node entry from the L3 Mcast database
*
* @param    macAddr  @b{(input)} Multicast MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopL3EntryDelete(L7_inet_addr_t *mcastGroupAddr,
                           L7_inet_addr_t *mcastSrcAddr)
{
  snoopL3InfoData_t  *pData;
  snoopL3InfoData_t  *snoopL3Entry;
  snoop_eb_t         *pSnoopEB;
#ifdef L7_NSF_PACKAGE
  L7_uchar8           mcastMacAddr[L7_MAC_ADDR_LEN];
#endif /* L7_NSF_PACKAGE */

  pSnoopEB = snoopEBGet();
  pData = snoopL3EntryFind(mcastGroupAddr, mcastSrcAddr,
                           L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  snoopL3Entry = pData;

  pData = avlDeleteEntry(&pSnoopEB->snoopL3AvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  if (pData == snoopL3Entry)
  {
    /* Entry deleted */

#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopMulticastMacFromIpAddr(mcastGroupAddr, mcastMacAddr);
      snoopCheckpointCallback2(SNOOP_CKPT_MSG_L3GA_DEL_DATA, &mcastMacAddr[SNOOP_MAC_ADDR_PREFIX_LEN],
                               *mcastGroupAddr, *mcastSrcAddr, 0, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */

    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}


/************************************************************************************************************/
/*MGMD*/


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* PTin added: IGMPv3 snooping */
  #if SNOOP_PTIN_IGMPv3_GLOBAL

    #if SNOOP_PTIN_IGMPv3_ROUTER
/**
 * @purpose Finds an entry with the given mcastGroupAddr and vlanId
 *
 * @param groupAddr  Multicast IP Address
 * @param vlanId          VLAN ID
 * @param flag            Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
 * @return  Matching entry or NULL on failure
 */
snoopPTinL3InfoData_t *snoopPTinL3EntryFind(L7_uint32 vlanId, L7_inet_addr_t* groupAddr, L7_uint32 flag)
{
  snoopPTinL3InfoData_t *snoopEntry;
  snoopPTinL3InfoDataKey_t key;
#if 0
  L7_uint32 ivlLength = 0;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  /* Argument validation */
  if (groupAddr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  memset((void *) &key, 0x00, sizeof(snoopPTinL3InfoDataKey_t));

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);
#endif

  memcpy(&key.mcastGroupAddr, groupAddr, sizeof(L7_inet_addr_t));
  memcpy(&key.vlanId, &vlanId, sizeof(L7_uint32));  
  snoopEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &key, flag);
  if (flag == L7_MATCH_GETNEXT)
  {
    while (snoopEntry)
    {
#if 0
      if (snoopEntry->snoopInfoDataKey.family == family)
      {
        break;
      }
#endif
      memcpy(&key, &snoopEntry->snoopPTinL3InfoDataKey, sizeof(snoopPTinL3InfoDataKey_t));
      snoopEntry = avlSearchLVL7(&pSnoopEB->snoopPTinL3AvlTree, &key, flag);
    }
  }

  if (snoopEntry == L7_NULL)
  {
    return L7_NULLPTR;
  }
  else
  {
    return snoopEntry;
  }
}

/**
 * @purpose Add a new entry to the PTin L3 AVL Tree
 *
 * @param groupAddr
 * @param vlanId
 *
 * @return  L7_SUCCESS or L7_FAILURE
 */
L7_RC_t snoopPTinL3EntryAdd(L7_uint32 vlanId, L7_inet_addr_t* groupAddr)
{
  snoopPTinL3InfoData_t snoopEntry;
  snoopPTinL3InfoData_t *pData;
#if 0
  L7_uint32 ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  pSnoopEB = snoopEBGet(); 

  /* Argument validation */
  if (groupAddr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_ERROR;
  }

#if 0
  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(snoopEntry.snoopInfoDataKey.vlanIdMacAddr, &vid, ivlLength);
  }
#endif

  memset(&snoopEntry, 0x00, sizeof(snoopPTinL3InfoData_t));  
  memcpy(&snoopEntry.snoopPTinL3InfoDataKey.mcastGroupAddr, groupAddr, sizeof(L7_inet_addr_t));  
  memcpy(&snoopEntry.snoopPTinL3InfoDataKey.vlanId,         &vlanId,         sizeof(L7_uint32));    
  pData = avlInsertEntry(&pSnoopEB->snoopPTinL3AvlTree, &snoopEntry);  

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/
    if ((pData = snoopPTinL3EntryFind(vlanId, groupAddr, AVL_EXACT)) == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  if (pData == &snoopEntry)
  {
    /*some error in avl tree addition*/
    return L7_FAILURE;
  }

  /*entry already exists*/
  return L7_FAILURE;
}

/**
 * @purpose Remove an existing entry to the PTin L3 AVL Tree
 *
 * @param groupAddr
 * @param vlanId
 *
 * @return L7_SUCCESS or L7_FAILURE
 */
L7_RC_t snoopPTinL3EntryDelete(L7_uint32 vlanId, L7_inet_addr_t* groupAddr)
{
  snoopPTinL3InfoData_t *pData;
  snoopPTinL3InfoData_t *snoopEntry;
  snoop_eb_t *pSnoopEB;
  char       debug_buf[IPV6_DISP_ADDR_LEN]={};
#if 0
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t zeroMask;
#endif /* L7_MCAST_PACKAGE */
  L7_uint32 freeIdx;
#endif

  /* Argument validation */
  if (groupAddr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_ERROR;
  }

  pSnoopEB = snoopEBGet();
  pData = snoopPTinL3EntryFind(vlanId, groupAddr, L7_MATCH_EXACT);
  if (pData == L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Unable to find requested entry");
    return L7_FAILURE;
  }
  snoopEntry = pData;

#if 0
#ifdef L7_MCAST_PACKAGE
  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);
#endif /* L7_MCAST_PACKAGE */
#endif

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to remove Multicast Group Address from the AVL Tree (vlanId:%u groupAddr:%s )",vlanId, inetAddrPrint(groupAddr, debug_buf));
  pData = avlDeleteEntry(&pSnoopEB->snoopPTinL3AvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Entry does not exist");
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    /* Entry deleted */
#if 0
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_DEL_DATA, macAddr, vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */
#endif
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}

    #endif
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    #if SNOOP_PTIN_IGMPv3_PROXY

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*Begin Source*/
/**
 * @purpose Finds an entry with the given mcastGroupAddr and vlanId
 *
 * @param mcastGroupAddr  Multicast IP Address
 * @param vlanId          VLAN ID
 * @param flag            Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
 * @return  Matching entry or NULL on failure
 */
snoopPTinProxySource_t *snoopPTinProxySourceEntryFind(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr,L7_uint32 flag)
{
  snoopPTinProxySource_t *pData;
  snoopPTinProxySourceKey_t key;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
#if 0
  L7_uint32 ivlLength = 0;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  /* Argument validation */
  if (groupPtr == L7_NULLPTR || sourceAddr==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  memset((void *) &key, 0x00, sizeof(snoopPTinProxySourceKey_t));

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);
#endif

#if 1
  memcpy(&key.groupPtr, &groupPtr, sizeof(snoopPTinProxyGroup_t*));
#else
  memcpy(&key.groupAddr,groupAddr,sizeof(L7_inet_addr_t));
#endif
  memcpy(&key.sourceAddr,sourceAddr,sizeof(L7_inet_addr_t));
  pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxySourceAvlTree, &key, flag);
  if (flag == L7_MATCH_GETNEXT)
  {
    while (pData)
    {
#if 0
      if (snoopEntry->snoopInfoDataKey.family == family)
      {
        break;
      }
#endif
      memcpy(&key, &pData->key, sizeof(snoopPTinProxySourceKey_t));
      pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxySourceAvlTree, &key, flag);
    }
  }

  if (pData == L7_NULL)
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Unable to find Source Address in the AVL Tree (groupAddr:%s sourceAddr:%s)", inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
    return L7_NULLPTR;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address found in the AVL Tree (groupAddr:%s sourceAddr:%s)", inetAddrPrint(&pData->key.groupPtr->key.groupAddr, debug_buf),inetAddrPrint(&pData->key.sourceAddr, debug_buf2));
    return pData;
  }
}

/**
 * @purpose Add a new entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return  L7_SUCCESS or L7_FAILURE
 */
snoopPTinProxySource_t* snoopPTinProxySourceEntryAdd(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr, L7_BOOL* newEntry,L7_uint8 robustnessVariable)
{
  snoopPTinProxySource_t snoopEntry;
  snoopPTinProxySource_t *pData;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
#if 0
  L7_uint32 ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  /* Argument validation */
  if (groupPtr == L7_NULLPTR || sourceAddr==L7_NULLPTR || newEntry==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  *newEntry=L7_FALSE; 

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(snoopEntry.snoopInfoDataKey.vlanIdMacAddr, &vid, ivlLength);
  }
#endif

  memset(&snoopEntry, 0x00, sizeof(snoopPTinProxySource_t));
  memcpy(&snoopEntry.key.groupPtr, &groupPtr, sizeof(snoopPTinProxyGroup_t*));
//  memcpy(&snoopEntry.key.groupAddr, &(groupPtr->key.groupAddr), sizeof(L7_inet_addr_t)); 
  memcpy(&snoopEntry.key.sourceAddr, sourceAddr, sizeof(L7_inet_addr_t));

#if ( !PTIN_BOARD_IS_MATRIX )
    snoopEntry.robustnessVariable=PTIN_MIN_ROBUSTNESS_VARIABLE;
#else
    snoopEntry.robustnessVariable=robustnessVariable;
#endif
    

//snoopEntry.groupPtr=groupPtr;
  pData = avlInsertEntry(&pSnoopEB->snoopPTinProxySourceAvlTree, &snoopEntry);

  if (pData == L7_NULL)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Source Address added to the AVL Tree (groupAddr:%s sourceAddr:%s)",inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
    /*entry was added into the avl tree*/
    if ((pData = snoopPTinProxySourceEntryFind(groupPtr, sourceAddr,AVL_EXACT)) == L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find Source Address in the AVL Tree, after adding it! (groupAddr:%s sourceAddr:%s)", inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));    
      return L7_NULLPTR;
    }
    *newEntry=L7_TRUE; 
    return pData;
  }

  if (pData == &snoopEntry)
  {
    /*some error in avl tree addition*/
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to add Source Address to the AVL Tree (groupAddr:%s sourceAddr:%s)",inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
    return L7_NULLPTR;
  }

  /*entry already exists*/
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Source Address  previouly added to the AVL Tree (groupAddr:%s sourceAddr:%s)",inetAddrPrint(&pData->key.groupPtr->key.groupAddr, debug_buf),inetAddrPrint(&pData->key.sourceAddr, debug_buf2));

#if 0
  if (pData->groupPtr != groupPtr)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Fixing groupPtr");
    pData->groupPtr=groupPtr;
  }
#endif

  return pData;
}

/**
 * @purpose Remove an existing entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return L7_SUCCESS or L7_FAILURE
 */
L7_RC_t snoopPTinProxySourceEntryDelete(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr)
{
  snoopPTinProxySource_t *pData;
  snoopPTinProxySource_t *snoopEntry;
  snoop_eb_t *pSnoopEB;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={},debug_buf2[IPV6_DISP_ADDR_LEN]={};
#if 0
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t zeroMask;
#endif /* L7_MCAST_PACKAGE */
  L7_uint32 freeIdx;
#endif

  /* Argument validation */
  if (groupPtr == L7_NULLPTR || sourceAddr==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_ERROR;
  }

  pSnoopEB = snoopEBGet();
  pData = snoopPTinProxySourceEntryFind(groupPtr, sourceAddr, L7_MATCH_EXACT);
  if (pData == L7_NULLPTR)
  {
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Unable to Source Addressfind requested entry");
    return L7_FAILURE;
  }
  snoopEntry = pData;

#if 0
#ifdef L7_MCAST_PACKAGE
  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);
#endif /* L7_MCAST_PACKAGE */
#endif

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to remove Source Address from the AVL Tree (groupAddr:%s sourceAddr:%s)",inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
  pData = avlDeleteEntry(&pSnoopEB->snoopPTinProxySourceAvlTree, pData);
  if (pData == L7_NULL)
  {
    /* Entry does not exist */    
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Source Address does not exist in the AVL Tree (groupAddr:%s sourceAddr:%s)",inetAddrPrint(&groupPtr->key.groupAddr, debug_buf),inetAddrPrint(sourceAddr, debug_buf2));
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    /* Entry deleted */    
#if 0
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_DEL_DATA, macAddr, vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */
#endif
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*End Source*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/





/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*Begin Group*/
/**
 * @purpose Finds an entry with the given mcastGroupAddr and vlanId
 *
 * @param mcastGroupAddr  Multicast IP Address
 * @param vlanId          VLAN ID
 * @param flag            Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
 * @return  Matching entry or NULL on failure
 */
snoopPTinProxyGroup_t *snoopPTinProxyGroupEntryFind(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType, L7_uint32 flag)
{
  snoopPTinProxyGroup_t *pData;
  snoopPTinProxyGroupKey_t key;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};
#if 0
  L7_uint32 ivlLength = 0;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  /* Argument validation */
  if (groupAddr==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid arguments");
    return L7_NULLPTR;
  }

  memset((void *) &key, 0x00, sizeof(snoopPTinProxyGroupKey_t));

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);
#endif

//memcpy(&key.interfacePtr,&interfacePtr, sizeof(snoopPTinProxyInterface_t*));
  memcpy(&key.vlanId,&vlanId, sizeof(L7_uint32*));  
  memcpy(&key.groupAddr,groupAddr,sizeof(L7_inet_addr_t));
  memcpy(&key.recordType,&recordType,sizeof(L7_uint8));
  pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxyGroupAvlTree, &key, flag);
  if (flag == L7_MATCH_GETNEXT)
  {
    while (pData)
    {
#if 0
      if (snoopEntry->snoopInfoDataKey.family == family)
      {
        break;
      }
#endif
      memcpy(&key, &pData->key, sizeof(snoopPTinProxyGroupKey_t));
      pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxyGroupAvlTree, &key, flag);
    }
  }

  if (pData == L7_NULL)
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Unable to find Group Record in the AVL Tree (vlanId:%u groupAddr:%s recordtype:%u)",vlanId, inetAddrPrint(groupAddr, debug_buf),recordType);
    return L7_NULLPTR;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record found in the AVL Tree (vlanId:%u groupAddr:%s recordtype:%u)",pData->key.vlanId, inetAddrPrint(&pData->key.groupAddr, debug_buf),pData->key.recordType);
    return pData;
  }
}

/**
 * @purpose Add a new entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return  L7_SUCCESS or L7_FAILURE
 */
snoopPTinProxyGroup_t* snoopPTinProxyGroupEntryAdd(snoopPTinProxyInterface_t* interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType, L7_BOOL* newEntry,L7_uint8 robustnessVariable)
{
  snoopPTinProxyGroup_t snoopEntry;
  snoopPTinProxyGroup_t *pData;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};
#if 0
  L7_uint32 ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  /*Arguments Validation*/
  if (interfacePtr==L7_NULLPTR || groupAddr==L7_NULLPTR || newEntry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_NULLPTR;
  }
  *newEntry=L7_FALSE;
  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(snoopEntry.snoopInfoDataKey.vlanIdMacAddr, &vid, ivlLength);
  }
#endif

  memset(&snoopEntry, 0x00, sizeof(snoopPTinProxyGroup_t));
//memcpy(&snoopEntry.key.interfacePtr, &interfacePtr, sizeof(snoopPTinProxyInterface_t*));
  memcpy(&snoopEntry.key.vlanId, &(interfacePtr->key.vlanId), sizeof(L7_uint32)); 
  memcpy(&snoopEntry.key.groupAddr, groupAddr, sizeof(L7_inet_addr_t));  
  memcpy(&snoopEntry.key.recordType, &recordType, sizeof(L7_uint8));   
  snoopEntry.interfacePtr=interfacePtr;

#if ( !PTIN_BOARD_IS_MATRIX )
    snoopEntry.robustnessVariable=PTIN_MIN_ROBUSTNESS_VARIABLE;
#else
    snoopEntry.robustnessVariable=robustnessVariable;
#endif

  snoopEntry.recordType=recordType;//We may need to modify the recordType value during the packet processing, since we use it as a key we need to have a way to change it without affecting the key.

  pData = avlInsertEntry(&pSnoopEB->snoopPTinProxyGroupAvlTree, &snoopEntry);

  if (pData == L7_NULL)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Group Record added to the AVL Tree(vlanId:%u groupAddr:%s recordtype:%u)",interfacePtr->key.vlanId,inetAddrPrint(groupAddr, debug_buf),recordType);
    /*entry was added into the avl tree*/
    if ((pData = snoopPTinProxyGroupEntryFind(interfacePtr->key.vlanId, groupAddr,recordType, AVL_EXACT)) == L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find Group Record in the AVL Tree, after adding it! (vlanId:%u groupAddr:%s recordtype:%u)",interfacePtr->key.vlanId, inetAddrPrint(groupAddr, debug_buf),recordType);
      return L7_NULLPTR;
    }
    *newEntry=L7_TRUE;
    return pData;
  }

  if (pData == &snoopEntry)
  {
    /*some error in avl tree addition*/
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to add Group Record to the AVL Tree(vlanId:%u groupAddr:%s recordtype:%u)",interfacePtr->key.vlanId,inetAddrPrint(groupAddr, debug_buf),recordType);
    return L7_NULLPTR;
  }

  /*entry already exists*/
  LOG_WARNING(LOG_CTX_PTIN_IGMP, "Group Record previouly added to the AVL Tree(vlanId:%u groupAddr:%s recordtype:%u)",pData->key.vlanId,inetAddrPrint(&pData->key.groupAddr, debug_buf),pData->key.recordType);    

  if (pData->interfacePtr != interfacePtr)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Fixing interfacePtr");
    pData->interfacePtr=interfacePtr;
  }

  return pData;
}

/**
 * @purpose Remove an existing entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return L7_SUCCESS or L7_FAILURE
 */
L7_RC_t snoopPTinProxyGroupEntryDelete(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType)
{
  snoopPTinProxyGroup_t *pData;
  snoopPTinProxyGroup_t *snoopEntry;
  snoop_eb_t *pSnoopEB;
  char                debug_buf[IPV6_DISP_ADDR_LEN]={};
#if 0
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t zeroMask;
#endif /* L7_MCAST_PACKAGE */
  L7_uint32 freeIdx;
#endif

  /*Arguments Validation*/
  if (vlanId<PTIN_IGMP_MIN_VLAN_ID || vlanId>PTIN_IGMP_MAX_VLAN_ID || groupAddr==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  pSnoopEB = snoopEBGet();
  pData = snoopPTinProxyGroupEntryFind(vlanId, groupAddr,recordType, L7_MATCH_EXACT);
  if (pData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  snoopEntry = pData;

#if 0
#ifdef L7_MCAST_PACKAGE
  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);
#endif /* L7_MCAST_PACKAGE */
#endif

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to remove Group Record from the AVL Tree (vlanId:%u groupAddr:%s recordtype:%u)",vlanId, inetAddrPrint(groupAddr, debug_buf),recordType);
  pData = avlDeleteEntry(&pSnoopEB->snoopPTinProxyGroupAvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */    
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Group Record does not exist in the AVL Tree (vlanId:%u groupAddr:%s recordtype:%u)",vlanId, inetAddrPrint(groupAddr, debug_buf),recordType);
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    /* Entry deleted */    
#if 0
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_DEL_DATA, macAddr, vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */
#endif
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*End Group*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*Begin Interface*/
/**
 * @purpose Finds an entry with the given mcastGroupAddr and vlanId
 *
 * @param mcastGroupAddr  Multicast IP Address
 * @param vlanId          VLAN ID
 * @param flag            Flag type for search
*                                L7_MATCH_EXACT   - Exact match
*                                L7_MATCH_GETNEXT - Next entry greater
*                                                   than this one
*
 * @return  Matching entry or NULL on failure
 */
snoopPTinProxyInterface_t *snoopPTinProxyInterfaceEntryFind(L7_uint32 vlanId, L7_uint32 flag)
{
  snoopPTinProxyInterface_t *pData;
  snoopPTinProxyInterfaceKey_t key;
#if 0
  L7_uint32 ivlLength = 0;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;

  memset((void *) &key, 0x00, sizeof(snoopPTinProxyInterfaceKey_t));

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);
#endif

  memcpy(&key.vlanId,&vlanId,sizeof(L7_uint32));
  pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, &key, flag);
  if (flag == L7_MATCH_GETNEXT)
  {
    while (pData)
    {
#if 0
      if (snoopEntry->snoopInfoDataKey.family == family)
      {
        break;
      }
#endif
      memcpy(&key, &pData->key, sizeof(snoopPTinProxyInterfaceKey_t));
      pData = avlSearchLVL7(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, &key, flag);
    }
  }

  if (pData == L7_NULL)
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Unable to find root vlan in the AVL Tree (vlanId:%u)",vlanId);
    return L7_NULLPTR;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Root vlan  found in the AVL Tree (vlanId:%u)",vlanId);
    return pData;
  }
}

/**
 * @purpose Add a new entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return  L7_SUCCESS or L7_FAILURE
 */
snoopPTinProxyInterface_t* snoopPTinProxyInterfaceEntryAdd(L7_uint32 vlanId, L7_BOOL* newEntry)
{
  snoopPTinProxyInterface_t snoopEntry;
  snoopPTinProxyInterface_t *pData;
#if 0
  L7_uint32 ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t fdbType;
#endif
  snoop_eb_t *pSnoopEB;


  /*Arguments Validation*/
  if (newEntry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_NULLPTR;
  }
  *newEntry=L7_FALSE; 

  pSnoopEB = snoopEBGet();

#if 0
  fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(snoopEntry.snoopInfoDataKey.vlanIdMacAddr, &vid, ivlLength);
  }
#endif

  memset(&snoopEntry, 0x00, sizeof(snoopPTinProxyInterface_t));  
  memcpy(&snoopEntry.key.vlanId, &vlanId, sizeof(L7_uint32));     
//snoopEntry.retransmissions=PTIN_IGMP_DEFAULT_ROBUSTNESS; //MMELO: Fixme
  snoopEntry.robustnessVariable=PTIN_IGMP_DEFAULT_ROBUSTNESS; //MMELO: Fixme
  pData = avlInsertEntry(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, &snoopEntry);

  if (pData == L7_NULL)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Root vlan added to the AVL Tree(vlanId:%u)",vlanId);
    /*entry was added into the avl tree*/
    if ((pData = snoopPTinProxyInterfaceEntryFind(vlanId, AVL_EXACT)) == L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to find root vlan in the AVL Tree, after adding it! (vlanId:%u)",vlanId);
      return L7_NULLPTR;
    }
    *newEntry=L7_TRUE; 
    return pData;
  }

  if (pData == &snoopEntry)
  {
    /*some error in avl tree addition*/
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to add root vlan to the AVL Tree(vlanId:%u)",vlanId);
    return L7_NULLPTR;
  }

  /*entry already exists*/
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Root vlan previouly added to the AVL Tree(vlanId:%u)",pData->key.vlanId);
  return pData;
}

/**
 * @purpose Remove an existing entry to the PTin L3 AVL Tree
 *
 * @param mcastGroupAddr
 * @param vlanId
 *
 * @return L7_SUCCESS or L7_FAILURE
 */
L7_RC_t snoopPTinProxyInterfaceEntryDelete(L7_uint32 vlanId)
{
  snoopPTinProxyInterface_t *pData;
  snoopPTinProxyInterface_t *snoopEntry;
  snoop_eb_t *pSnoopEB;
#if 0
#ifdef L7_MCAST_PACKAGE
  L7_INTF_MASK_t zeroMask;
#endif /* L7_MCAST_PACKAGE */
  L7_uint32 freeIdx;
#endif

  pSnoopEB = snoopEBGet();
  pData = snoopPTinProxyInterfaceEntryFind(vlanId, L7_MATCH_EXACT);
  if (pData == L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Unable to find requested entry");
    return L7_FAILURE;
  }
  snoopEntry = pData;

#if 0
#ifdef L7_MCAST_PACKAGE
  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);
#endif /* L7_MCAST_PACKAGE */
#endif
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Going to remove root vlan from the AVL Tree (vlanId:%u)",vlanId);
  pData = avlDeleteEntry(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */    
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Root vlan does not exist in the AVL Tree (vlanId:%u)",vlanId);
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    /* Entry deleted */    
#if 0
#ifdef L7_NSF_PACKAGE
    if (pSnoopEB->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_GA_DEL_DATA, macAddr, vlanId, family, 0, L7_NULL, L7_NULL, L7_NULL);
    }
#endif /* L7_NSF_PACKAGE */
#endif
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
/*End Interface*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    #endif


/*End MGMD Proxy*/
/************************************************************************************************************/


  #endif

#endif /* L7_MCAST_PACKAGE */
