/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_l2_fdb.c
*
* Purpose: This file contains the functions to transform the L2 FDB
*          components's driver requests.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets:
*
* Created by: Shekhar Kalyanam 3/14/2001
*
*********************************************************************/
/*************************************************************

*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_MAC_GLOBALS               /* Enable global space   */
#include "dtlinclude.h"


#if DTLCTRL_COMPONENT_L2_FDB

extern L7_RC_t dtlFind(char *mac, L7_uint32 *intIfNum);
extern L7_BOOL dtlIsReady(void);
/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS
**********************************************************************
*/

/***
** Debug function to examine DTL queue size.
***/
int dtlFdbDebugQSizeGet(void)
{
  L7_int32 numOnQ;

  if (osapiMsgQueueGetNumMsgs( dtlAddrMsgQueue, &numOnQ) != L7_SUCCESS)
  {
    printf("osapiMsgQueueGetNumMsgs, Error.\n");
  } else
  {
    printf("DTL MAX Queue Limit = %d, Current Size = %d\n",
           DTL_ADDR_MSG_COUNT,
           numOnQ);
  }

  return 0;
}

/*********************************************************************
* @purpose  Learns entry callback from received frames puts it in a message queue
* @purpose  and releases the interrupt thread
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param      family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlFdbReceiveCallback(DAPI_USP_t *ddusp,
                                    DAPI_FAMILY_t family,
                                    DAPI_CMD_t cmd,
                                    DAPI_EVENT_t event,
                                    void *dapiEventInfo)
{


  L7_RC_t mrc = L7_SUCCESS;
  L7_RC_t dr= L7_SUCCESS;
  DTL_MSG_t  dtl_msg;

#if defined(FEAT_METRO_CPE_V1_0)
  if (!dtlIsReady())
  {
    return L7_SUCCESS;
  }
#endif

  if (event == DAPI_EVENT_ADDR_INTF_MAC_QUERY)
  {
    dr = dtlGenericCallback(ddusp,family,cmd,event,dapiEventInfo);
    return dr;
  }

  if (event == DAPI_EVENT_ADDR_LOCK_UNKNOWN_ADDRESS)
  {
    L7_int32 numOnQ;
    if (osapiMsgQueueGetNumMsgs( dtlAddrMsgQueue, &numOnQ) != L7_SUCCESS)
    {
      return dr;
    }
    if (numOnQ > ((DTL_ADDR_MSG_COUNT * 5) / 100))
    {
      return dr;
    }
  }


  memcpy(&(dtl_msg.cmdData.dapiAddrEventInfo),dapiEventInfo,sizeof(DAPI_ADDR_MGMT_CMD_t));
  memcpy(&(dtl_msg.ddusp),ddusp,sizeof(DAPI_USP_t));


  dtl_msg.cmdFamilyType = family;
  dtl_msg.cmdType = cmd;
  dtl_msg.eventType = event;


  if ((mrc = osapiMessageSend(dtlAddrMsgQueue,
                              (void *)&dtl_msg,
                              sizeof (dtl_msg),
                              L7_NO_WAIT,
                              L7_MSG_PRIORITY_NORM))  != L7_SUCCESS)
  {
      dr = L7_FAILURE;
  }

  return dr;
}

#ifdef L7_METRO_FLEX_PACKAGE
extern L7_RC_t dot1adServiceFdbInsertCallback(L7_uint32 intIfNum,
                   L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);
extern L7_RC_t dot1adServiceFdbAgeCallback(L7_enetMacAddr_t macAddr,
                                                L7_ushort16 vlanId);
#endif

/*********************************************************************
* @purpose  Learned entry callback from received frames
*
* @param    *ddusp         @b{(input)} Device driver reference to unit slot and port
* @param      family         @b{(input)} Device Driver family type
* @param    cmd            @b{(input)} Command of type DAPI_CMD_t
* @param    dapiEventInfo  @b{(input)} Pointer to dapiEvent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlFdbReceive(DAPI_USP_t *ddusp,
                      DAPI_FAMILY_t family,
                      DAPI_CMD_t cmd,
                      DAPI_EVENT_t event,
                      void *dapiEventInfo)
{

  DAPI_ADDR_MGMT_CMD_t *dei;
  L7_uint32 intIfNum;
  nimUSP_t usp;

  dei = (DAPI_ADDR_MGMT_CMD_t *)dapiEventInfo;

#if defined(FEAT_METRO_CPE_V1_0)
  if (!dtlIsReady())
  {
    return L7_SUCCESS;
  }
#endif

  if (event == DAPI_EVENT_ADDR_LEARNED_ADDRESS)
  {

    usp.unit = ddusp->unit;
    usp.slot = ddusp->slot;
    usp.port = ddusp->port + 1;

    if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "Received L2 FDB entry notification for invalid usp %d/%d/%u.",
                usp.unit, usp.slot, usp.port);
      return L7_FAILURE;
    }

#ifdef L7_MACLOCK_PACKAGE
    pmlLearnEntryCallBack(intIfNum,
                          dei->cmdData.unsolLearnedAddress.macAddr,
                          dei->cmdData.unsolLearnedAddress.vlanID);
#endif
  {
    fdbLearnEntryCallBack(dei->cmdData.unsolLearnedAddress.macAddr.addr,
                          intIfNum, dei->virtual_port, /* PTin added: virtual ports */
                          dei->cmdData.unsolLearnedAddress.vlanID,
                          FDB_ADD);
  }

#if defined(FEAT_METRO_CPE_V1_0)
#ifdef L7_MACLOCK_PACKAGE

  L7_uint32 pmlAdminMode;
  L7_uint32 pmlIntfMode;

   if ( ( (pmlAdminModeGet(&pmlAdminMode) == L7_SUCCESS) &&
          (pmlAdminMode == L7_DISABLE)
         ) &&
        ( (pmlIntfModeGet(intIfNum,&pmlIntfMode) == L7_SUCCESS) &&
          (pmlIntfMode == L7_DISABLE)
        )
      )
#endif
    {
       dot1adServiceFdbInsertCallback(intIfNum,
                                  dei->cmdData.unsolLearnedAddress.macAddr,
                                  dei->cmdData.unsolLearnedAddress.vlanID);
    }
#endif

  }
  else if (event==DAPI_EVENT_ADDR_AGED_ADDRESS)
  {
#ifdef L7_MACLOCK_PACKAGE
    pmlAgeEntryCallBack(dei->cmdData.unsolLearnedAddress.macAddr,
                        dei->cmdData.unsolLearnedAddress.vlanID);
#endif

#if defined(FEAT_METRO_CPE_V1_0)
      {
          dot1adServiceFdbAgeCallback(dei->cmdData.unsolLearnedAddress.macAddr,
                                      dei->cmdData.unsolLearnedAddress.vlanID);
      }
#endif

    fdbLearnEntryCallBack(dei->cmdData.unsolAgedAddress.macAddr.addr,
                          intIfNum, dei->virtual_port /* PTin added: virtual ports */,
                          dei->cmdData.unsolAgedAddress.vlanID,
                          FDB_DEL);
  }
  else if (event == DAPI_EVENT_ADDR_LOCK_UNKNOWN_ADDRESS)
  {
    usp.unit = ddusp->unit;
    usp.slot = ddusp->slot;
    usp.port = ddusp->port + 1;

    if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "invalid usp\n");
      return L7_FAILURE;
    }

#ifdef L7_MACLOCK_PACKAGE
    pmlUnknownAddrCallBack(intIfNum,
                           dei->cmdData.unsolIntfMacUnknownAddr.macAddr,
                           dei->cmdData.unsolIntfMacUnknownAddr.vlanID);
#endif
  }
  else if ( DAPI_EVENT_ADDR_DOT1X_UNAUTH == event )
  {

    usp.unit = ddusp->unit;
    usp.slot = ddusp->slot;
    usp.port = ddusp->port + 1;



    if ( nimGetIntIfNumFromUSP( &usp, &intIfNum ) != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "invalid usp\n");
      return L7_FAILURE;
    }

    dot1xUnauthAddrCallBack( intIfNum,
                             dei->cmdData.unsolIntfMacUnknownAddr.macAddr,
                             dei->cmdData.unsolIntfMacUnknownAddr.vlanID );
  }
  else if (DAPI_EVENT_ADDR_SYNC_COMPLETE == event)
  {
#ifdef L7_MACLOCK_PACKAGE
    pmlSyncCompleteCallback();
#endif
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the FDB address aging time out
*
* @param    filterDbID       @b{(input)} Filter data base id
* @param    time             @b{(input)} Aging time to be set
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    time is in seconds
*
* @end
*********************************************************************/
L7_RC_t dtlFdbAddressAgingTimeOutSet(L7_uint32 filterDbID,
                                     L7_uint32 time)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;
  L7_RC_t rc = L7_FAILURE;

  dapiCmd.cmdData.agingTime.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.agingTime.vlanID = filterDbID;
  dapiCmd.cmdData.agingTime.agingTime = time;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;



  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_AGING_TIME,&dapiCmd);

  if (dr==L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Flushes specific VLAN entries in fdb.
*
* @param    vlanId @b((input)) VLAN number.
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlFdbFlushByVlan(L7_ushort16 vlanId)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.portAddressFlushVlan.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.portAddressFlushVlan.vlanID = vlanId;
  return dapiCtl(&ddUsp, DAPI_CMD_ADDR_FLUSH_VLAN, &dapiCmd);
}

/*********************************************************************
* @purpose  Flushes All MAC specific entries in fdb.
*
* @param    mac @b((input)) MAC address
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t dtlFdbFlushByMac(L7_enetMacAddr_t mac)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.portAddressFlushMac.getOrSet = DAPI_CMD_SET;
  memcpy(dapiCmd.cmdData.portAddressFlushMac.macAddr.addr,
                 mac.addr, L7_ENET_MAC_ADDR_LEN);
  return dapiCtl(&ddUsp, DAPI_CMD_ADDR_FLUSH_MAC, &dapiCmd);
}

/*********************************************************************
* @purpose  Adds a mac address to the address management table
*
* @param    *macAddr    @b{(input)} MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    filterDbID  @b{(input)} Filter database ID
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacAddrAdd(L7_uchar8 *macAddr,
                         L7_uint32 intfNum,
                         L7_ulong32 filterDbID,
                         L7_FDB_ADDR_FLAG_t flag,
                         void *dtlPtr)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  DAPI_ADDR_FLAG_t dapiFlag;
  L7_RC_t dr;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port -1;

  memcpy(&(dapiCmd.cmdData.macAddressEntryAdd.macAddr.addr),macAddr, L7_ENET_MAC_ADDR_LEN);
  switch (flag)
  {
  case L7_FDB_ADDR_FLAG_STATIC:
    dapiFlag = DAPI_ADDR_FLAG_STATIC;
    break;
  case L7_FDB_ADDR_FLAG_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_L3_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_SELF:
    dapiFlag = DAPI_ADDR_FLAG_SELF;
    break;
  default:
    return L7_FAILURE;
  }

  dapiCmd.cmdData.macAddressEntryAdd.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.macAddressEntryAdd.flags = dapiFlag;
  dapiCmd.cmdData.macAddressEntryAdd.vlanID = filterDbID;

  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}


/*********************************************************************
* @purpose  Modifies a mac address in the address management table
*
* @param    *macAddr    @b{(input)} MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    filterDbID  @b{(input)} Filter database ID
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacAddrModify(L7_uchar8 *macAddr,
                            L7_uint32 intfNum,
                            L7_ulong32 filterDbID,
                            L7_FDB_ADDR_FLAG_t flag,
                            void *dtlPtr)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  DAPI_ADDR_FLAG_t dapiFlag;
  L7_RC_t dr;
  nimUSP_t usp;


  if (nimGetUnitSlotPort(intfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port -1;

  memcpy(&(dapiCmd.cmdData.macAddressEntryModify.macAddr.addr),macAddr, L7_ENET_MAC_ADDR_LEN);
  switch (flag)
  {
  case L7_FDB_ADDR_FLAG_STATIC:
    dapiFlag = DAPI_ADDR_FLAG_STATIC;
    break;
  case L7_FDB_ADDR_FLAG_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_L3_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_SELF:
    dapiFlag = DAPI_ADDR_FLAG_SELF;
    break;
  default:
    return L7_FAILURE;
  }

  dapiCmd.cmdData.macAddressEntryModify.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.macAddressEntryModify.flags = dapiFlag;
  dapiCmd.cmdData.macAddressEntryModify.vlanID = filterDbID;

  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;


}

/*********************************************************************
* @purpose  Deletes a mac address from the address management table
*
* @param    *macAddr    @b{(input)} Pointer to MAC address to be added
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    flag        @b{(input)} Type of flag for MAC address
* @param    filterDbID  @b{(input)} Filter database ID
* @param    *dtlPtr     @b{(input)} void pointer for passing a structure
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    The dtlPtr is for future use so that this API's
* @notes    interface need not change
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacAddrDelete(L7_uchar8 *macAddr,
                            L7_uint32 intfNum,
                            L7_ulong32 filterDbID,
                            L7_FDB_ADDR_FLAG_t flag,
                            void *dtlPtr)
{
  DAPI_USP_t ddUsp;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  DAPI_ADDR_FLAG_t dapiFlag;
  L7_RC_t dr;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port -1;

  memcpy(&(dapiCmd.cmdData.macAddressEntryDelete.macAddr.addr),macAddr, L7_ENET_MAC_ADDR_LEN);
  switch (flag)
  {
  case L7_FDB_ADDR_FLAG_STATIC:
    dapiFlag = DAPI_ADDR_FLAG_STATIC;
    break;
  case L7_FDB_ADDR_FLAG_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_L3_MANAGEMENT:
    dapiFlag = DAPI_ADDR_FLAG_L3_MANAGEMENT;
    break;
  case L7_FDB_ADDR_FLAG_GMRP_LEARNED:
    dapiFlag = DAPI_ADDR_FLAG_LEARNED;
    break;
  case L7_FDB_ADDR_FLAG_SELF:
    dapiFlag = DAPI_ADDR_FLAG_SELF;
    break;
  default:
    return L7_FAILURE;
  }

  dapiCmd.cmdData.macAddressEntryDelete.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.macAddressEntryDelete.flags = dapiFlag;
  dapiCmd.cmdData.macAddressEntryDelete.vlanID = filterDbID;

  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;


}


/*********************************************************************
* @purpose  Sets the system MAC address and management vlan id
*
* @param    *macAddr    @b{(input)} Pointer to MAC address to be added
* @param    type        @b{(input)} Type of system MAC address
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacAddrSystemSet(L7_uchar8 *macAddr, L7_ushort16 vlanId, DTL_MAC_TYPE_t type)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.systemMacAddress.getOrSet = DAPI_CMD_SET;
  memcpy(&(dapiCmd.cmdData.systemMacAddress.macAddr.addr),macAddr, L7_ENET_MAC_ADDR_LEN);
  dapiCmd.cmdData.systemMacAddress.vlanId = vlanId;

  dr = dapiCtl(&ddUsp,DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Clear the last system MAC that was set.
*
* @param    none
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacAddrSystemClear(void)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.systemMacAddress.getOrSet = DAPI_CMD_CLEAR;
  memset(&(dapiCmd.cmdData.systemMacAddress.macAddr.addr),0, L7_ENET_MAC_ADDR_LEN);
  dapiCmd.cmdData.systemMacAddress.vlanId = 0;

  dr = dapiCtl(&ddUsp,DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Debug function to test MAC address set performance.
*
* @param    count   Number of MAC address set commands.
* @param    interval Number of milliseconds between MAC sets.
*
* @returns  0
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 dtlDebugMacSet (L7_int32 count, L7_int32 interval)
{
  L7_uchar8 mac[6];
  L7_int32 i;

  for (i = 0; i < count; i++)
  {
    dtlFdbMacAddrSystemSet (mac, 1, DTL_MAC_TYPE_BURNED_IN);

    if ((interval > 0) && (interval < 1000))
    {
      osapiSleepMSec (interval);
    }

    if ((i % 100) == 0)
    {
      printf("Executed %d transactions.\n", i);
    }
  }

  return 0;

}

/*********************************************************************
* @purpose  Sets the MAC addr type for the interface
*
* @param        intIfNum  @b{(input)} Internal Interface Number
* @param        addrType  @b{(input)} Type of MAC address
*                     @b{DTL_MAC_TYPE_BURNED_IN,
*                        DTL_MAC_TYPE_LOCAL_ADMIN
*                       }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t dtlFdbMacAddrTypeSet(L7_uint32 intIfNum,
                             DTL_MAC_TYPE_t addrType)
{

  return L7_NOT_IMPLEMENTED_YET;



}

/*********************************************************************
* @purpose  Retrives the internal interface number associated with a MAC address
*
* @param    *mac        pointer to the mac address to search for
* @param    *intIfNum   pointer to the interface number to be returned
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    wrapper routine to retrive the internal interface number associated
*           with a MAC address from the arp table maintained in DTL
*
* @end
*********************************************************************/
L7_RC_t dtlFdbMacToIntfGet(L7_uchar8 *mac, L7_uint32 *intIfNum)
{
  return dtlFind(mac, intIfNum);

}

/*********************************************************************
* @purpose  Configures Port MAC Locking on an interface
*
* @param    intfNum          @b{(input)} Internal intf number
* @param    lockEnabled      @b{(input)} locking configuration
* @param    dynamicLimit     @b{(input)} dynamic limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t dtlPmlLockConfig(L7_uint32 intfNum, L7_BOOL lockEnabled)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.macLockConfig.getOrSet     = DAPI_CMD_SET;
  dapiCmd.cmdData.macLockConfig.lock         = lockEnabled;

  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_MAC_LOCK_CONFIG, &dapiCmd);

  if (dr==L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Indicates to the driver that it should synchronize
*           all of it's dynamic FDB entries with the application.
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @end
*********************************************************************/
L7_RC_t dtlFdbSync()
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc = L7_FAILURE;


  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_ADDR_SYNC, L7_NULL);

  return rc;
}
/*
**********************************************************************
*                           PRIVATE FUNCTIONS
**********************************************************************
*/


#endif /*DTLCTRL_COMPONENT_L2_FDB*/
