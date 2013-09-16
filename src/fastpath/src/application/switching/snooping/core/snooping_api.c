/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_api.c
*
* @purpose    Contains definitions to APIs used by USMDB and snoop
*             component
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
#include "osapi_support.h"
#include "l7utils_inet_addr_api.h"

#include "l7_product.h"
#include "snooping_defs.h"
#include "snooping_api.h"
#include "snooping_util.h"
#include "snooping_outcalls.h"
#include "snooping_db.h"
#include "snooping_proto.h"

/* PTin added: IGMP snooping */
#if 1
#include "logger.h"

/**
 * Get a list of channels consumed by a particular vlan and 
 * client index 
 * 
 * @param vlanId       : Group Vlan
 * @param client_index : Client index
 * @param channels     : Channels array
 * @param nChannels    : max number of channels (input) 
 *                       number of channels (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_snoop_activeChannels_get(L7_uint16 vlanId, L7_uint16 client_index, ptin_igmpChannelInfo_t *channels, L7_uint16 *nChannels)
{
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX || channels==L7_NULLPTR || nChannels==L7_NULLPTR)
    return L7_FAILURE;

  /* Get list of channels */
  snoopChannelsListGet(vlanId, client_index, channels, nChannels);

  return L7_SUCCESS;
}

/**
 * Get a list of clients (client indexes) watching a particular 
 * channel 
 * 
 * @param channelIP         : channel
 * @param sVlan             : group vlan
 * @param client_list_bmp   : client list
 * @param number_of_clients : number of clients
 * 
 * @return L7_RC_t L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_snoop_clientsList_get(L7_inet_addr_t *channelIP, L7_uint16 sVlan, L7_uint32 *client_list_bmp, L7_uint16 *number_of_clients)
{
  snoopInfoData_t *snoopEntry;
  L7_uint16 channel_index;

  if (channelIP==L7_NULL || sVlan<PTIN_VLAN_MIN || sVlan>PTIN_VLAN_MAX || client_list_bmp==L7_NULLPTR)
    return L7_FAILURE;

  if (channelIP->family!=L7_AF_INET)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Only IPv4 is supported!");
    return L7_FAILURE;
  }

  // Search for channel
  if (!snoopChannelExist4VlanId(sVlan,channelIP,&snoopEntry))
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Channel %u.%u.%u.%u does not exist for vlan %u",
            (channelIP->addr.ipv4.s_addr>>24) & 0xff,
            (channelIP->addr.ipv4.s_addr>>16) & 0xff,
            (channelIP->addr.ipv4.s_addr>> 8) & 0xff,
             channelIP->addr.ipv4.s_addr & 0xff,
            sVlan);
    if (client_list_bmp!=L7_NULLPTR)
    {
      memset(client_list_bmp,
             0x00,
             sizeof(snoopEntry->channel_list[channel_index].clients_list));
    }
    if (number_of_clients!=L7_NULLPTR)
    {
      *number_of_clients = 0;
    }
  }
  else
  {
    /* Get channel index */
    channel_index = (channelIP->addr.ipv4.s_addr>>23) & 0x1f;

    // Copy clients bitmap
    if (client_list_bmp!=L7_NULLPTR)
    {
      memcpy(client_list_bmp,
             snoopEntry->channel_list[channel_index].clients_list,
             sizeof(snoopEntry->channel_list[channel_index].clients_list));
    }
    /* Number of clients */
    if (number_of_clients!=L7_NULLPTR)
    {
      *number_of_clients = snoopEntry->channel_list[channel_index].number_of_clients;
    }
  }

  return L7_SUCCESS;
}

/**
 * Remove a client from all channels allocated in a particular 
 * interface of a channel group 
 * 
 * @param vlanId       : Service vlan (0 to not be considered)
 * @param client_index : client index
 * @param intIfNum     : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_client_remove(L7_uint16 sVlanId, L7_uint16 client_index, L7_uint32 intIfNum)
{
  L7_uint16 vlanId, channel_index;
  L7_uchar8 macAddr[L7_FDB_MAC_ADDR_LEN];
  L7_inet_addr_t channel;
  snoopInfoData_t *snoopEntry;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ((sVlanId!=0 && (sVlanId<PTIN_VLAN_MIN || sVlanId>PTIN_VLAN_MAX)) ||
      client_index>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE ||
      intIfNum>=L7_MAX_INTERFACE_COUNT)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  vlanId = 1;
  memset(macAddr,0x00,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

  while ((snoopEntry=snoopEntryFind(macAddr,vlanId,L7_AF_INET,L7_MATCH_GETNEXT))!=L7_NULLPTR)
  {
    /* Extract vlan and MAC address */
    vlanId = ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8) |
             ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1]);
    memcpy(macAddr,&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN],sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

    /* If vlan does not match, skip to the next one */
    if ( sVlanId!=0 && sVlanId!=vlanId )  continue;
    
    /* Run all active channels */
    for (channel_index=0; channel_index<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_index++)
    {
      if (!snoopEntry->channel_list[channel_index].active)  continue;

      channel.family = L7_AF_INET;
      channel.addr.ipv4.s_addr = snoopEntry->channel_list[channel_index].ipAddr;

      /* Remove client, and update snooping database */
      if (snoop_client_remove_procedure(macAddr,vlanId,&channel,client_index,intIfNum,L7_NULLPTR)==L7_SUCCESS)
      {
        /* Success removing client fromt this channel */
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with snoop_client_remove_procedure");
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

/**
 * Add a static channel
 * 
 * @param vlanId    : Service vlan
 * @param channel   : Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_static_channel_add(L7_uint16 vlanId, L7_inet_addr_t *channel)
{
  L7_BOOL sendJoin;
  L7_uchar8 dmac[L7_MAC_ADDR_LEN]={ 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };

  /* Validate arguments */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX || channel==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }
  if (channel->family!=L7_AF_INET || channel->addr.ipv4.s_addr==0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid channel");
    return L7_FAILURE;
  }

  /* Determine MAC associated to this channel */
  dmac[0] = 0x01;
  dmac[1] = 0x00;
  dmac[2] = 0x5E;
  dmac[3] = (channel->addr.ipv4.s_addr>>16) & 0x7f;
  dmac[4] = (channel->addr.ipv4.s_addr>> 8) & 0xff;
  dmac[5] =  channel->addr.ipv4.s_addr & 0xff;

  /* Add channel */
  if (snoop_channel_add_procedure(dmac,vlanId,channel,L7_TRUE,&sendJoin)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding static channel");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove a channel
 * 
 * @param vlanId    : Service vlan
 * @param channel   : Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_channel_remove(L7_uint16 vlanId, L7_inet_addr_t *channel)
{
  L7_uchar8 dmac[L7_MAC_ADDR_LEN]={ 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };

  /* Validate arguments */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX || channel==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }
  if (channel->family!=L7_AF_INET || channel->addr.ipv4.s_addr==0)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid channel");
    return L7_FAILURE;
  }

  /* Determine MAC associated to this channel */
  dmac[0] = 0x01;
  dmac[1] = 0x00;
  dmac[2] = 0x5E;
  dmac[3] = (channel->addr.ipv4.s_addr>>16) & 0x7f;
  dmac[4] = (channel->addr.ipv4.s_addr>> 8) & 0xff;
  dmac[5] =  channel->addr.ipv4.s_addr & 0xff;

  /* Add channel */
  if (snoop_channel_remove_procedure(dmac,vlanId,channel)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove all channels associated to a particualr vlan
 * 
 * @param vlanId    : Service vlan (0 to not be considered)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_snoop_channel_removeAll(L7_uint16 sVlanId)
{
  L7_uint16 vlanId, channel_idx;
  L7_uchar8 macAddr[L7_FDB_MAC_ADDR_LEN];
  L7_inet_addr_t channel;
  snoopInfoData_t *snoopEntry;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (sVlanId!=0 && (sVlanId<PTIN_VLAN_MIN || sVlanId>PTIN_VLAN_MAX))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid arguments");
    return L7_FAILURE;
  }

  vlanId = 1;
  memset(macAddr,0x00,sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

  while ((snoopEntry=snoopEntryFind(macAddr,vlanId,L7_AF_INET,L7_MATCH_GETNEXT))!=L7_NULLPTR)
  {
    /* Extract vlan and MAC address */
    vlanId = ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[0]<<8) |
             ((L7_uint16) snoopEntry->snoopInfoDataKey.vlanIdMacAddr[1]);
    memcpy(macAddr,&snoopEntry->snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN],sizeof(L7_uchar8)*L7_FDB_MAC_ADDR_LEN);

    /* If vlan does not match, skip to the next one */
    if ( sVlanId!=0 && sVlanId!=vlanId )  continue;

    /* Run all channels */
    for (channel_idx=0; channel_idx<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel_idx++)
    {
      if (!snoopEntry->channel_list[channel_idx].active)  continue;

      channel.family = L7_AF_INET;
      channel.addr.ipv4.s_addr = snoopEntry->channel_list[channel_idx].ipAddr;

      /* Remove channel */
      if (snoop_channel_remove_procedure(macAddr,vlanId,&channel)==L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel removed from vlan %u",vlanId);
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing channel from vlan %u",vlanId);
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

#endif

/*********************************************************************
* @purpose  Gets the Snooping Admin mode for the specified snooping
*           instance
*
* @param    adminMode  @b{(output)} Snooping admin mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopAdminModeGet(L7_uint32 *adminMode, L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    *adminMode = pSnoopCB->snoopCfgData->snoopAdminMode;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Snooping Admin mode for the specified snooping
*           instance
*
* @param    adminMode  @b{(input)} Snooping admin mode
* @param    family     @b{(input)} L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopAdminModeSet(L7_uint32 adminMode, L7_uchar8 family)
{
  /* Apply the config change */
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB;

  /* Validate Input */
  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    if (adminMode == pSnoopCB->snoopCfgData->snoopAdminMode)
      return L7_SUCCESS; /* No Change in admin mode */
    else
      pSnoopCB->snoopCfgData->snoopAdminMode = adminMode;
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* Post an event to the snoopQueue to Apply admin mode */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopAdminModeChange;
  msg.u.mode   = adminMode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopAdminModeSet: Failed to set global igmp snooping mode to  %d."
        " Failed to set global IGMP Snooping mode due to message queue being full",
        adminMode);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopAdminModeSet: Failed to give msgQueue semaphore");
    }
  }

  return L7_SUCCESS;
}

/* PTin added: IGMP */
#if 1
/*********************************************************************
* @purpose  Sets the Snooping Priority for the specified snooping
*           instance
*
* @param    prio       @b{(input)} Snooping priority
* @param    family     @b{(input)} L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopPrioModeSet(L7_uint8 prio, L7_uchar8 family)
{
  /* Apply the config change */
  snoop_cb_t     *pSnoopCB;

  /* Validate Input */
  if (prio > 7)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    pSnoopCB->snoopCfgData->snoopAdminIGMPPrio = prio;
  }
  
  return L7_SUCCESS;
}
#endif

/******************************************************************************
* @purpose  Gets the interface snoop operational mode of a specified interface
*           and VLAN for a specified snooping instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    vlanId    @b{(input)} VLAN ID
* @param    mode      @b{(output)} Snooping interface operational mode
* @param    family    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopIntfModeGet(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 *mode,
                         L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry = L7_NULLPTR;

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_FAILURE;
  }

  if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum))
  {
    *mode = L7_ENABLE;
  }
  else
  {
    *mode = L7_DISABLE;
  }

  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Snooping interface configuration mode for the
*           specified interface and snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mode      @b{(input)} Snooping intf mode
* @param    family    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE If interface was invalid or if invalid mode
*                      was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfCfgModeSet(L7_uint32 intIfNum, L7_uint32 mode,
                            L7_uchar8 family)
{
  L7_uint32           currentIntfMode;
  snoopIntfCfgData_t *pCfg;
  snoopMgmtMsg_t      msg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* get the configuration structure */
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  currentIntfMode = (pCfg->intfMode & SNOOP_VLAN_MODE) ? L7_ENABLE : L7_DISABLE;
  if (mode == currentIntfMode)
  {
    return L7_SUCCESS;      /* No Change in interface admin mode */
  }

  if (mode == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_MODE;
  }

  /* Apply the config change */

  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopIntfModeChange;
  msg.intIfNum = intIfNum;
  msg.u.mode   = mode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
          "snoopIntfModeSet: Failed to set igmp snooping mode %d for interface %s."
          " Failed to set interface IGMP Snooping mode due to message queue being full",
              mode, ifName);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfModeSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Snooping interface configuration mode for the
*           specified interface and snoop instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Snooping intf mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE If interface was invalid or if invalid mode
*                      was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfCfgModeGet(L7_uint32 intIfNum, L7_uint32 *mode,
                            L7_uchar8 family)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  /* Check if it is of supported interface type*/
  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get the interface configuration structure */
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  *mode = (pCfg->intfMode & SNOOP_VLAN_MODE) ? L7_ENABLE : L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets list of interfaces that are enabled for snooping for
*           a specified snoop instance
*
* @param    numIntf   @b{(output)} Number of interfaces
* @param    intfList  @b{(output)} Densely packed array of interfaces
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    numIntf will be returned as zero if there is no interfaces enabled.
*           Only the first numIntf entries in the intfList have valid
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t snoopIntfEnabledListGet(L7_uint32 *numIntf, L7_uint32 intfList[],
                                L7_uchar8 family)
{
  L7_uint32       intIfNum;
  L7_uint32       cfgIndex;
  nimConfigID_t   configIdNull;
  L7_uint32       count    = 0;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  snoopCfgData_t *pSnoopCfgData = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Point to the configuration structure */
  pSnoopCfgData = pSnoopCB->snoopCfgData;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pSnoopCfgData->snoopIntfCfgData[cfgIndex].configId,
                               &configIdNull))
    {
      continue;
    }

    /* Get the corresponding intIfNum */
    if (nimIntIfFromConfigIDGet(&(pSnoopCfgData->snoopIntfCfgData[cfgIndex].configId),
                                &intIfNum) != L7_SUCCESS)
    {
      continue;
    }

    /* Is interface mode enabled ? */
    if (pSnoopCfgData->snoopIntfCfgData[cfgIndex].intfMode & SNOOP_VLAN_MODE)
    {
      intfList[count] = intIfNum;
      count++;
    }
  }

  *numIntf = count;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the IGMP group membership interval for the specified interface
*           of a snoop instance
*
* @param    intIfNum                  @b{(input)} Internal interface number
* @param    groupMembershipInterval   @b{(input)} New Group Membership Interval
* @param    family                    @b{(input)} L7_AF_INET  => IGMP Snooping
*                                                 L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   If the specified interval is out of range
*
* @notes    This will set the configured interval time, but will not change
*           the current running timers of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t snoopIntfApiGroupMembershipIntervalSet(L7_uint32 intIfNum,
                                               L7_uint32 groupMembershipInterval,
                                               L7_uchar8 family)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get the interface configuration structure */
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  /* Validate limits */
  if (family == L7_AF_INET)
  {
    if (groupMembershipInterval < L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL ||
        groupMembershipInterval > L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (groupMembershipInterval < L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL ||
        groupMembershipInterval > L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL)
    {
      return L7_FAILURE;
    }
  }

  /* Group Membership Interval time must be greater than Max Response time */
  if (groupMembershipInterval <= (L7_uint32)pCfg->responseTime)
  {
    return L7_FAILURE;
  }

  pCfg->groupMembershipInterval = (L7_ushort16)groupMembershipInterval;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the group membership interval for the specified interface
*           of a snoop instance
*
* @param    intIfNum                 @b{(input)}  Internal interface number
* @param    groupMembershipInterval  @b{(output)} Group Membership Interval
* @param    family                   @b{(input)} L7_AF_INET  => IGMP Snooping
*                                                L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfGroupMembershipIntervalGet(L7_uint32 intIfNum,
                                            L7_uint32 *groupMembershipInterval,
                                            L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  *groupMembershipInterval = (L7_uint32)pCfg->groupMembershipInterval;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the query response time for the specified interface
*           of a snoop instance
*
* @param    intIfNum      @b{(input)} Internal interface number
* @param    responseTime  @b{(input)} New response time
* @param    family        @b{(input)} L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If the response time was set
* @returns  L7_FAILURE  If the specified response time is out of range
*
* @notes    This will set the configured response time, but, will not update
*           the response time of any existing Snooping entries.
*
* @end
*********************************************************************/
L7_RC_t snoopIntfApiResponseTimeSet(L7_uint32 intIfNum, L7_uint32 responseTime,
                                    L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get the interface configuration structure */
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  /* Validate limits */
  if (family == L7_AF_INET)
  {
    if (responseTime < L7_IGMP_SNOOPING_MIN_RESPONSE_TIME ||
        responseTime > L7_IGMP_SNOOPING_MAX_RESPONSE_TIME)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (responseTime < L7_MLD_SNOOPING_MIN_RESPONSE_TIME ||
        responseTime > L7_MLD_SNOOPING_MAX_RESPONSE_TIME)
    {
      return L7_FAILURE;
    }
  }

  /* Max Response time must be less than Query Interval time */
  if (responseTime >= (L7_uint32)pCfg->groupMembershipInterval)
  {
    return L7_FAILURE;
  }

  pCfg->responseTime = (L7_ushort16)responseTime;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the query response time for the specified interface of
*           a snoop instance
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    responseTime @b{(output)} Response time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfResponseTimeGet(L7_uint32 intIfNum, L7_uint32 *responseTime,
                                 L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  *responseTime = (L7_uint32)pCfg->responseTime;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the mcast router expiration time for the specified interface
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    expiryTime   @b{(output)} Expiry time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @notes    An expiryTime of zero indicates infinite timeout
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMcastRtrExpiryTimeGet(L7_uint32 intIfNum, L7_uint32 *expiryTime,
                                       L7_uchar8 family)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  *expiryTime = pCfg->mcastRtrExpiryTime;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the mcast router expiration time for the specified interface
*           of a snoop instance
*
* @param    intIfNum     @b{(input)}  Internal interface number
* @param    expiryTime   @b{(input)}  Expiry time
* @param    family       @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE  If time value out of range
*
* @notes    An expiryTime of zero indicates infinite timeout
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMcastRtrExpiryTimeSet(L7_uint32 intIfNum, L7_uint32 expiryTime,
                                       L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  if (family == L7_AF_INET)
  {
    if (expiryTime < L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME ||
        expiryTime > L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (expiryTime < L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME ||
        expiryTime > L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME)
    {
      return L7_FAILURE;
    }
  }

  pCfg->mcastRtrExpiryTime = expiryTime;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Fast-Leave Admin mode for the specified interface
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Fast-Leave intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfFastLeaveAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode,
                                       L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  *mode = (pCfg->intfMode & SNOOP_VLAN_FAST_LEAVE_MODE) ?
    L7_ENABLE : L7_DISABLE;

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Sets the Fast-Leave Admin mode for the specified interface
*           for a snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mode      @b{(input)} Fast-Leave intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE  If interface was invalid or if invalid mode was
*                       specified
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t snoopIntfFastLeaveAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode,
                                       L7_uchar8 family)
{
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;
  snoopIntfCfgData_t  *pCfg;
  L7_uint32            currentFastLeaveIntfMode;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  currentFastLeaveIntfMode = (pCfg->intfMode & SNOOP_VLAN_FAST_LEAVE_MODE) ?
    L7_ENABLE : L7_DISABLE;

  if (mode == currentFastLeaveIntfMode)
  {
    return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_FAST_LEAVE_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_FAST_LEAVE_MODE;
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the specified interface as a multicast router interface
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(input)}  Mrouter intf admin mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE  If interface was invalid or if invalid mode was
*                       specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfMrouterSet(L7_uint32 intIfNum, L7_uint32 mode,
                            L7_uchar8 family)
{
  snoopIntfCfgData_t *pCfg;
  snoopMgmtMsg_t      msg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  if (mode != L7_ENABLE && mode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Compare with current mode */
  if (mode == pCfg->intfMcastRtrAttached)
  {
    return L7_SUCCESS;
  }

  pCfg->intfMcastRtrAttached = mode;

  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopIntfMrouterModeChange;
  msg.intIfNum = intIfNum;
  msg.u.mode   = mode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
          "snoopIntfMrouterSet: Failed to set igmp mrouter mode %d for interface %s."
          " Failed to set interface muticast router mode due to IGMP Snooping message "
          "queue being full", mode, ifName);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfMrouterSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Gets the operational Multicast Router Detected mode for the
*           specified interface of a snoop instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    vlanId    @b{(input)}  VLAN Id
* @param    mode      @b{(output)} Multicast Router Detected mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE  If interface out of range
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t snoopIntfMrouterGet(L7_uint32 intIfNum, L7_uint32 vlanId,
                            L7_uint32 *mode, L7_uchar8 family)
{
  snoop_cb_t       *pSnoopCB = L7_NULLPTR;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get the index to access the configuration structure */
  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_FAILURE;
  }

  /* Check if enable flag is set */
  if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
  {
    *mode = L7_ENABLE;
  }
  else
  {
    *mode = L7_DISABLE;
  }

  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the configured Multicast Router Detected mode for the
*           specified interface of a snoop Instance
*
* @param    intIfNum  @b{(input)}  Internal interface number
* @param    mode      @b{(output)} Multicast Router Detected mode
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE  If interface out of range
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopIntMrouterStatusGet(L7_uint32 intIfNum, L7_uint32 *mode,
                                 L7_uchar8 family)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  *mode = pCfg->intfMcastRtrAttached;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Flush Snooping Table of a snoop instance
*
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
L7_RC_t snoopEntriesFlush(L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopMgmtMsg_t   msg;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopFlush;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopEntriesFlush: Failed to send snooping table flush");
    return L7_FAILURE;
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopEntriesFlush: Failed to give msgQueue semaphore");
      return L7_FAILURE;
    }
  }
  /* Flush all entries */
  pSnoopCB->counters.controlFramesProcessed = 0;
  return L7_SUCCESS;
}
#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  Notify Snooping about a L3 Mcast Entry addition
*
* @param    mcastGroupAddr     @b{(input)}  IP Address of mcast group
* @param    mcastSrcAddr       @b{(input)}  IP Address of data source
* @param    srcVlan            @b{(input)}  Ingress VLAN Interface
* @param   *outVlanRtrIntfMask @b{(input)}  Rtr Interface Mask
* @param    flag               @b{(input)}  flag = L7_TRUE for add notify
*                                           flag = L7_FALSE for delete notify
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopMcastNotify(L7_inet_addr_t *mcastGroupAddr,
                         L7_inet_addr_t *mcastSrcAddr,
                         L7_int32        srcVlan,
                         L7_VLAN_MASK_t *outVlanRtrIntfMask,
                         L7_uchar8 flag)
{
  snoop_eb_t     *pSnoopEB;
  snoopMgmtMsg_t  msg;

  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );

  memcpy(&msg.u.mcastChangeParams.mcastGroupAddr, mcastGroupAddr,
         sizeof(L7_inet_addr_t));

  memcpy(&msg.u.mcastChangeParams.mcastSrcAddr, mcastSrcAddr,
         sizeof(L7_inet_addr_t));


  if (flag == L7_TRUE)
  {
    msg.msgId = snoopL3McastAdd;
    memcpy(&msg.u.mcastChangeParams.outVlanRtrIntfMask,
           outVlanRtrIntfMask, sizeof(L7_VLAN_MASK_t));
    msg.u.mcastChangeParams.srcIntfVlan = srcVlan;
  }
  else
  {
    msg.msgId = snoopL3McastDelete;
  }

  if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopMcastNotify: Failed to send mcast entry notify");
    return L7_FAILURE;
  }
  else
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "snoopMcastNotify: Failed to give msgQueue semaphore");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
#endif /* L7_MCAST_PACKAGE */

/*****************************************************************************
* @purpose  Get the number of multicast control frames processed by the CPU
*           of a snoop instance
*
* @param    controlFramesProcessed  @b{(output)} Number of control frames
*                                                processed
* @param    family                  @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                L7_AF_INET6 => MLD Snooping
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopControlFramesProcessed(L7_uint32 *controlFramesProcessed,
                                    L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *controlFramesProcessed = pSnoopCB->counters.controlFramesProcessed;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Get the number of multicast data frames forwarded by the CPU of
*           a snoop instance
*
* @param    dataFramesForwarded  @b{(output)} Number of data frames forwarded
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This API is for reference only and is currently not used.
*
* @end
*****************************************************************************/
L7_RC_t snoopDataFramesForwarded(L7_uint32 *dataFramesForwarded, L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *dataFramesForwarded = 0;
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Gets the configured Snooping mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId     @b{(input)}  vlan Id
* @param    vlanMode   @b{(output)} Configured Snooping vlan mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
***************************************************************************/
L7_RC_t snoopVlanModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                         L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;
  L7_RC_t      rc;

  rc = snoopVlanCheckValid(vlanId);
  if (L7_SUCCESS != rc)
  {
    return rc;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
  {
    *vlanMode = L7_ENABLE;
  }
  else
  {
    *vlanMode = L7_DISABLE;
  }

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Sets the configured Snooping mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId     @b{(input)}  vlan Id
* @param    vlanMode   @b{(output)} Configured Snooping vlan mode
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopVlanModeSet(L7_uint32 vlanId, L7_uint32 vlanMode, L7_uchar8 family)
{
  snoopMgmtMsg_t msg;
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;
  L7_uint32    currMode;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanModeGet(vlanId, &currMode, family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* No Change in snooping mode */
  if (vlanMode == currMode)
  {
    return L7_SUCCESS;
  }

  if (vlanMode == L7_ENABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] |= SNOOP_VLAN_MODE;
  }
  else if (vlanMode == L7_DISABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] &= (~SNOOP_VLAN_MODE);
  }
  else
  {
    return L7_FAILURE;
  }

  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopVlanModeChange;
  msg.vlanId   = vlanId;
  msg.u.mode   = vlanMode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopVlanModeSet: Failed to set igmp snooping mode %d for vlan %d."
        " Failed to set VLAN IGM Snooping mode due to message queue being full",
            vlanMode, vlanId);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfMrouterSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Sets the configured Snooping fast leave mode for the specified VLAN
*           of a snoop instance
*
* @param    vlanId          @b{(input)}  vlan Id
* @param    fastLeaveMode   @b{(input)}  Configured Snooping Fast Leave Mode
* @param    family          @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                        L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS If mode was set successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopVlanFastLeaveModeSet(L7_uint32 vlanId, L7_uint32 fastLeaveMode,
                                  L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if (fastLeaveMode == L7_ENABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanMode[vlanId]
      |= SNOOP_VLAN_FAST_LEAVE_MODE;
  }
  else if (fastLeaveMode == L7_DISABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanMode[vlanId]
      &= (~SNOOP_VLAN_FAST_LEAVE_MODE);
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the configured Snooping fast leave mode for the
*           specified VLAN of a snoop instance
*
* @param    vlanId         @b{(input)}  vlan Id
* @param    fastLeaveMode  @b{(output)} Configured Snooping Fast Leave Mode
* @param    family         @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If mode was retrieved successfully
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopVlanFastLeaveModeGet(L7_uint32 vlanId, L7_BOOL *fastLeaveMode,
                                  L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *fastLeaveMode = (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] &
                    SNOOP_VLAN_FAST_LEAVE_MODE)
    ? L7_TRUE : L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the configured Snooping Group Membership Interval
*           for the specified VLAN of a snoop instance
*
* @param    vlanId                    @b{(input)}  vlan Id
* @param    groupMembershipInterval   @b{(input)}  Configured IGMP Snooping
*                                                  Fast Leave Mode
* @param    family                    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanGroupMembershipIntervalSet(L7_uint32 vlanId,
                                            L7_ushort16 groupMembershipInterval,
                                            L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (family == L7_AF_INET)
  {
    if (groupMembershipInterval < L7_IGMP_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL ||
        groupMembershipInterval > L7_IGMP_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (groupMembershipInterval < L7_MLD_SNOOPING_MIN_GROUP_MEMBERSHIP_INTERVAL ||
        groupMembershipInterval > L7_MLD_SNOOPING_MAX_GROUP_MEMBERSHIP_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  /* Group Membership Interval time must be greater than Max Response time */
  if (groupMembershipInterval <=
      pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime)
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].groupMembershipInterval =
    groupMembershipInterval;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the configured Snooping Group Membership Interval
*           for the specified VLAN of a snoop instance
*
* @param    vlanId                    @b{(input)}  vlan Id
* @param    groupMembershipInterval   @b{(output)} Configured Snooping Group
*                                                  Membership Interval
* @param    family                    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanGroupMembershipIntervalGet(L7_uint32 vlanId,
                                            L7_ushort16 *groupMembershipInterval,
                                            L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *groupMembershipInterval =
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].groupMembershipInterval;

  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the configured Snooping Maximum Response Time for the
*           specified VLAN of a snoop instance
*
* @param    vlanId           @b{(input)} vlan Id
* @param    responseTime     @b{(input)} Configured Snooping Maximum
*                                        response Time
* @param    family           @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopVlanMaximumResponseTimeSet(L7_uint32 vlanId,
                                        L7_ushort16 responseTime,
                                        L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (family == L7_AF_INET)
  {
    if (responseTime < L7_IGMP_SNOOPING_MIN_RESPONSE_TIME ||
        responseTime > L7_IGMP_SNOOPING_MAX_RESPONSE_TIME)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (responseTime < L7_MLD_SNOOPING_MIN_RESPONSE_TIME ||
        responseTime > L7_MLD_SNOOPING_MAX_RESPONSE_TIME)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  /* Group Membership Interval time must be greater than Max Response time */
  if (responseTime >=
      pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].groupMembershipInterval)
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime =
    responseTime;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the configured Snooping Maximum Response Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId           @b{(input)}  vlan Id
* @param    responseTime     @b{(output)} Configured Snooping Maximum
*                                         response Time
* @param    family           @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanMaximumResponseTimeGet(L7_uint32 vlanId,
                                        L7_ushort16 *responseTime,
                                        L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *responseTime =
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the configured Snooping Mcast Router Expiry Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId               @b{(input)}  vlan Id
* @param    mcastRtrExpiryTime   @b{(input)}  Configured Snooping Maximum
*                                             response Time
*
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanMcastRtrExpiryTimeSet(L7_uint32 vlanId,
                                       L7_ushort16 mcastRtrExpiryTime,
                                       L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;
  L7_uint32  u32Val;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  u32Val = mcastRtrExpiryTime;
  if (family == L7_AF_INET)
  {
    if (u32Val < L7_IGMP_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME ||
        u32Val > L7_IGMP_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (u32Val < L7_MLD_SNOOPING_MIN_MCAST_RTR_EXPIRY_TIME ||
        u32Val > L7_MLD_SNOOPING_MAX_MCAST_RTR_EXPIRY_TIME)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].mcastRtrExpiryTime =
    mcastRtrExpiryTime;
  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/***************************************************************************
* @purpose  Sets the configured Snooping Mcast Router Expiry Time
*           for the specified VLAN of a snoop instance
*
* @param    vlanId               @b{(input)}  vlan Id
* @param    mcastRtrExpiryTime   @b{(output)} Configured Snooping Maximum
*                                             response Time
* @param    family               @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                             L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
***************************************************************************/
L7_RC_t snoopVlanMcastRtrExpiryTimeGet(L7_uint32 vlanId,
                                       L7_ushort16 *mcastRtrExpiryTime,
                                       L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *mcastRtrExpiryTime =
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].mcastRtrExpiryTime;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Set the static mcast router attached status for the specified
*           interface/vlan of a snoop instance
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    vlanId      @b{(input)}  vlan Id
* @param    status      @b{(input)}  Mcast router attached mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrSet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                          L7_uchar8 status, L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;
  snoopMgmtMsg_t       msg;
  L7_uchar8            currentStatus;

  if(status != L7_ENABLE && status != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get pointer to the interface configiration structure */
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  if (L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
  {
    currentStatus = L7_ENABLE;
  }
  else
  {
    currentStatus = L7_DISABLE;
  }

  /* No Change in mode */
  if (currentStatus == status)
  {
    return L7_SUCCESS;
  }

  /* Set/reset the mod flag */
  if (status == L7_ENABLE)
  {
    L7_VLAN_SETMASKBIT(pCfg->vlanStaticMcastRtr, vlanId);
  }
  else
  {
    L7_VLAN_CLRMASKBIT(pCfg->vlanStaticMcastRtr, vlanId);
  }

  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopVlanMrouterModeChange;
  msg.intIfNum = intIfNum;
  msg.u.mode   = status;
  msg.vlanId   = vlanId;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
        "snoopIntfApiVlanStaticMcastRtrSet: Failed to set igmp mrouter mode %d for interface %s on Vlan %d."
        " Failed to set VLAN multicast router mode due to IGMP Snooping message queue being full",
        status, ifName, vlanId);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfApiVlanStaticMcastRtrSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Get the static mcast router attached status for the specified
*           interface/vlan of a snoop instance
*
* @param    intIfNum    @b{(input)}   Internal interface number
* @param    vlanId      @b{(input)}   vlan Id
* @param    status      @b{(output)}  Mcast router attached mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrGet(L7_uint32 intIfNum, L7_uint32 vlanId,
                                          L7_uchar8 *status, L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }

  if (L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
  {
    *status = L7_ENABLE;
  }
  else
  {
    *status = L7_DISABLE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate whether a snoop instance is supported or not
*
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_TRUE    Corresponding snoop instance exists
* @returns  L7_FALSE   Corresponding snoop instance not supported
*
*
* @end
*********************************************************************/
L7_BOOL snoopProtocolGet(L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;
  /* Get Snoop Control Block. If control block exists then feature
     is supported */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Get next supported snoop instance
*
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
* @param    nextFamily  @b{(output)}  Next valid snoop instance
*                                     L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_TRUE   Found a valid snoop instance
* @returns  L7_FALSE  No more snoop instances are present
*
*
* @end
*********************************************************************/
L7_BOOL snoopProtocolNextGet(L7_uchar8 family, L7_uchar8 *nextFamily)
{
  snoop_cb_t *pSnoopCB;
  L7_uint32   cbIndex;
  L7_uint32   maxInstances;

  /* Get next existing CB */
  if (family == 0)
  {
    if ((pSnoopCB = snoopCBFirstGet()) != L7_NULLPTR)
    {
      *nextFamily = pSnoopCB->family;
      return L7_TRUE;
    }
    else
    {
      *nextFamily = 0;
      return L7_FALSE;
    }
  }
  else if ((pSnoopCB = snoopCBGet(family)) != L7_NULLPTR)
  {
    maxInstances = maxSnoopInstancesGet();
    for (cbIndex = pSnoopCB->cbIndex + 1; cbIndex < maxInstances; cbIndex++)
    {
      pSnoopCB++;
      if (pSnoopCB != L7_NULLPTR)
      {
        *nextFamily = pSnoopCB->family;
        return L7_TRUE;
      }
      else
      {
        *nextFamily = 0;
        return L7_FALSE;
      }
    }/* iterate to the next instance */
  }
  *nextFamily = 0;
  return L7_FALSE;
}

/* Snooping Querier APIs */
/**********************************************************************
* @purpose  Sets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(input)}   Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t snoopQuerierAdminModeSet(L7_uint32 adminMode, L7_uchar8 family)
{
  /* Apply the config change */
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB;

  /* Validate Input */
  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    if (adminMode == pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAdminMode)
    {
      return L7_SUCCESS; /* No Change in admin mode */
    }
    else
    {
      pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAdminMode = adminMode;
    }
  }

  /* Post an event to the snoopQueue to Apply admin mode */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierModeChange;
  msg.u.mode   = adminMode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierAdminModeSet: Failed to set global snooping querier mode to %d ",
            adminMode);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierAdminModeSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/***********************************************************************
* @purpose  Gets the Snooping Querier Admin mode of a snoop instance
*
* @param    adminMode   @b{(output)}  Snooping Querier admin mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
***********************************************************************/
L7_RC_t snoopQuerierAdminModeGet(L7_uint32 *adminMode, L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    *adminMode = pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAdminMode;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Snooping Querier Configured Address of a snoop
*           instance
*
* @param    snoopQuerierAddr @b{(output)}  Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAddressGet(void *snoopQuerierAddr,
                               L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      snoopQuerierAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return inetAddressGet(family,
                        &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress,
                        snoopQuerierAddr);
}

/*********************************************************************
* @purpose  Sets the Snooping Querier Address of a snoop instance
*
* @param    snoopQuerierAddr @b{(input)}   Querier Address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED  If the passed address is not a (IPv4 /
*                             Link-local IPv6) unicast address
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAddressSet(void *snoopQuerierAddr,
                               L7_uchar8 family)
{
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  L7_inet_addr_t  addr;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopQuerierAddr == L7_NULL)
  {
    return L7_FAILURE;
  }


  if (inetAddressSet(family, snoopQuerierAddr, &addr) != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  if ((L7_INET_IS_ADDR_BROADCAST(&addr)) ||
      inetIsInMulticast(&addr) == L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  if (family == L7_AF_INET6)
  {
    if (inetIsAddressZero(&addr) == L7_FALSE)
    {
      /* Only Link local addresses are allowed as MLD query source address */
      if (!(L7_IP6_IS_ADDR_LINK_LOCAL(snoopQuerierAddr)))
      {
        return L7_ERROR;
      }
    }
  }

  /* No Change in snooping Address */
  if (memcmp(&addr,
             &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress
             , sizeof(L7_inet_addr_t))  == 0)
  {
    return L7_SUCCESS;
  }

  memcpy(&pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress,
         &addr, sizeof(L7_inet_addr_t));

  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierAddressChange;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierAddressSet: Failed to post a message to queue");
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierAddressSet: Failed to give msgQueue semaphore\n");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Snooping Querier Configured version of a snoop
*           instance
*
* @param    version     @b{(output)}  Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVersionGet(L7_uint32 *version, L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *version =
    pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Snooping Querier version of a snoop instance
*
* @param    version     @b{(input)}   Configured querier version
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVersionSet(L7_uint32 version, L7_uchar8 family)
{
  /* Apply the config change */
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB;

  /* Validate Input */
  if (family == L7_AF_INET)
  {
    if (version < L7_IGMP_SNOOPING_QUERIER_MIN_VERSION ||
        version > L7_IGMP_SNOOPING_QUERIER_MAX_VERSION)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    if (version < L7_MLD_SNOOPING_QUERIER_MIN_VERSION ||
        version > L7_MLD_SNOOPING_QUERIER_MAX_VERSION)
    {
      return L7_FAILURE;
    }
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  else
  {
    if (version == pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion)
    {
      return L7_SUCCESS; /* No Change in version */
    }
    else
    {
      pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion = version;
    }
  }

  /* Post an event to the snoopQueue to Apply admin mode */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierVersionChange;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierVersionSet: Failed to set snooping querier version to  %d",
            version);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierVersionSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierQueryIntervalGet(L7_uint32 *queryInterval,
                                     L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *queryInterval =
    pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierQueryInterval;

  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the Snooping Querier Query Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for sending periodic queries
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierQueryIntervalSet(L7_uint32 queryInterval, L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (family == L7_AF_INET)
  {
    if (queryInterval < L7_IGMP_SNOOPING_QUERIER_MIN_QUERY_INTERVAL ||
        queryInterval > L7_IGMP_SNOOPING_QUERIER_MAX_QUERY_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (queryInterval < L7_MLD_SNOOPING_QUERIER_MIN_QUERY_INTERVAL ||
        queryInterval > L7_MLD_SNOOPING_QUERIER_MAX_QUERY_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierQueryInterval =
    queryInterval;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(output)}  Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierExpiryIntervalGet(L7_uint32 *expiryInterval,
                                      L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *expiryInterval =
    pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierExpiryInterval;

  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the other Querier expiry Interval of a snoop instance
*
* @param    queryInterval @b{(input)}   Interval for declaring last querier
*                                       as expired
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierExpiryIntervalSet(L7_uint32 expiryInterval,
                                      L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (family == L7_AF_INET)
  {
    if (expiryInterval < L7_IGMP_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL ||
        expiryInterval > L7_IGMP_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (expiryInterval < L7_MLD_SNOOPING_QUERIER_MIN_EXPIRY_INTERVAL ||
        expiryInterval > L7_MLD_SNOOPING_QUERIER_MAX_EXPIRY_INTERVAL)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierExpiryInterval =
    expiryInterval;

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      & SNOOP_QUERIER_MODE)
  {
    *vlanMode = L7_ENABLE;
  }
  else
  {
    *vlanMode = L7_DISABLE;
  }

  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the configured Snooping Querier mode for the specified vlanId
*           of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL  When max number of querier vlan's supported
*                             is reached
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                L7_uchar8 family)
{
  snoopMgmtMsg_t    msg;
  snoop_cb_t       *pSnoopCB = L7_NULLPTR;
  L7_uint32         currMode;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  currMode = (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
              & SNOOP_QUERIER_MODE) ? L7_ENABLE : L7_DISABLE;

  /* No Change in snooping mode */
  if (vlanMode == currMode)
  {
    return L7_SUCCESS;
  }

  if (vlanMode == L7_ENABLE)
  {
    if (pSnoopCB->enabledSnoopQuerierVlans >= (L7_uint32)(SNOOP_QUERIER_VLAN_MAX))
    {
      return L7_TABLE_IS_FULL;
    }
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      |= SNOOP_QUERIER_MODE;
    pSnoopCB->enabledSnoopQuerierVlans++;
  }
  else if (vlanMode == L7_DISABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      &= (~SNOOP_QUERIER_MODE);
    pSnoopCB->enabledSnoopQuerierVlans--;
  }
  else
  {
    return L7_FAILURE;
  }
  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierVlanModeChange;
  msg.vlanId   = vlanId;
  msg.u.mode   = vlanMode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierVlanModeSet: Failed to set snooping querier mode %d for vlan %d",
            vlanMode, vlanId);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierVlanModeSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Gets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(output)} Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanElectionModeGet(L7_uint32 vlanId, L7_uint32 *vlanMode,
                                        L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      & SNOOP_QUERIER_ELECTION_PARTICIPATE)
  {
    *vlanMode = L7_ENABLE;
  }
  else
  {
    *vlanMode = L7_DISABLE;
  }

  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the configured Snooping Querier election mode for the
*           specified vlanId of a snoop instance
*
* @param    vlanId      @b{(input)}  vlan Id
* @param    vlanMode    @b{(input)}  Configured Snooping Querier vlan election
*                                    participate mode
* @param    family      @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                    L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t snoopQuerierVlanElectionModeSet(L7_uint32 vlanId, L7_uint32 vlanMode,
                                        L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;
  L7_uint32    currMode;
  snoopMgmtMsg_t msg;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  currMode = (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
              & SNOOP_QUERIER_ELECTION_PARTICIPATE) ? L7_ENABLE : L7_DISABLE;

  /* No Change in querier mode */
  if (vlanMode == currMode)
  {
    return L7_SUCCESS;
  }

  if (vlanMode == L7_ENABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      |= SNOOP_QUERIER_ELECTION_PARTICIPATE;
  }
  else if (vlanMode == L7_DISABLE)
  {
    pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      &= (~SNOOP_QUERIER_ELECTION_PARTICIPATE);
  }
  else
  {
    return L7_FAILURE;
  }
  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierVlanElectionModeChange;
  msg.vlanId   = vlanId;
  msg.u.mode   = vlanMode;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierVlanElectionModeSet: Failed to set snooping querier election mode %d for vlan %d",
            vlanMode, vlanId);
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierVlanElectionModeSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}   vlan Id
* @param    snoopQuerierAddr      @b{(output)}  Querier vlan address
* @param    family                @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                               L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierVlanAddressGet(L7_uint32 vlanId,
                                   void *snoopQuerierAddr,
                                   L7_uchar8 family)
{
  snoop_cb_t  *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      snoopQuerierAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return inetAddressGet(family,
                        &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress,
                        snoopQuerierAddr);
}

/******************************************************************************
* @purpose  Gets the Snooping Querier Configured Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId                @b{(input)}  vlan Id
* @param    snoopQuerierAddr      @b{(input)}  Querier vlan address
* @param    family                @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                              L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED  If the passed address is not a (IPv4 /
*                             Link-local IPv6) unicast address
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierVlanAddressSet(L7_uint32 vlanId,
                                   void *snoopQuerierAddr,
                                   L7_uchar8 family)
{
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  L7_inet_addr_t  addr;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (snoopQuerierAddr == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (inetAddressSet(family, snoopQuerierAddr, &addr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((L7_INET_IS_ADDR_BROADCAST(&addr)) ||
      inetIsInMulticast(&addr) == L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  if (family == L7_AF_INET6)
  {
    if (inetIsAddressZero(&addr) == L7_FALSE)
    {
      /* Only Link local addresses are allowed as MLD query source address */
      if (!(L7_IP6_IS_ADDR_LINK_LOCAL(snoopQuerierAddr)))
      {
        return L7_NOT_SUPPORTED;
      }
    }
  }

  /* No Change in snooping Address */
  if (memcmp(&addr,
             &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress
             , sizeof(L7_inet_addr_t))  == 0)
  {
    return L7_SUCCESS;
  }

  memcpy(&pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress,
         &addr, sizeof(L7_inet_addr_t));

  /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierVlanAddressChange;
  msg.vlanId   = vlanId;
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "snoopQuerierVlanAddressSet: Failed to post a message to queue");
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopQuerierVlanAddressSet: Failed to give msgQueue semaphore");
    }
  }

  /* set flag to indicate configuration has changed */
  pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the detected last Querier Address for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    querierAddr      @b{(output)}  last Querier vlan address
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierLastQuerierAddressGet(L7_uint32 vlanId,
                                          void *querierAddr,
                                          L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry;
  L7_RC_t          rc;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      querierAddr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  if (inetIsAddressZero(&pSnoopOperEntry->snoopQuerierInfo.querierAddress))
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_FAILURE;
  }

  rc = inetAddressGet(family,
                      &pSnoopOperEntry->snoopQuerierInfo.querierAddress,
                      querierAddr);
  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return rc;
}

/******************************************************************************
* @purpose  Gets the detected last Querier's version for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    querierVersion   @b{(output)}  last Querier protocol version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierLastQuerierVersionGet(L7_uint32 vlanId,
                                          L7_uint32 *querierVersion,
                                          L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      querierVersion == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  if (pSnoopOperEntry->snoopQuerierInfo.querierVersion == 0)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  *querierVersion = pSnoopOperEntry->snoopQuerierInfo.querierVersion;

  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the Operationa version of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    version          @b{(output)}  Operational Version
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperVersionGet(L7_uint32 vlanId, L7_uint32 *version,
                                   L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      version == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion)
  {
    *version = pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion;
  }
  else
  {
    *version = pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;
  }

  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the Operational State of Querier  for specified vlan Id
*           of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    state            @b{(output)}  Operational State
*                                          L7_SNOOP_QUERIER_STATE_t
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperStateGet(L7_uint32 vlanId, L7_uint32 *state,
                                 L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      state == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  switch (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState)
  {
    case SNOOP_QUERIER_DISABLED:
      *state = L7_SNOOP_QUERIER_DISABLED;
      break;
    case  SNOOP_QUERIER_NON_QUERIER:
      *state = L7_SNOOP_QUERIER_NON_QUERIER;
      break;
    case SNOOP_QUERIER_QUERIER:
      *state = L7_SNOOP_QUERIER_QUERIER;
      break;
    default:
      osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
      return L7_FAILURE;
  }
  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the Operational value of max response time for specified
*           vlan Id of a snoop instance
*
* @param    vlanId           @b{(input)}   vlan Id
* @param    maxRespTime      @b{(output)}  Operational Max Response Time value
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    When success, if maxRespTime == 0, it means that the max response
*           time is less than 1 sec
*
* @end
******************************************************************************/
L7_RC_t snoopQuerierOperMaxRespTimeGet(L7_uint32 vlanId, L7_uint32 *maxRespTime,
                                       L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry;
  L7_ushort16      shortVal;
  L7_uchar8        charVal;
  L7_uint32        uintVal;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR ||
      maxRespTime == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (snoopVlanCheckValid(vlanId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_NOT_EXIST;
  }

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState !=
      SNOOP_QUERIER_QUERIER
      && pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion)
  {
    if (family == L7_AF_INET) /* IGMP */
    {
      if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_IGMP_VERSION_2)
      {
        shortVal = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
        *maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(shortVal,SNOOP_IGMP_FP_DIVISOR);
      }
      else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_IGMP_VERSION_3)
      {
        charVal = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
        snoopFPDecode(charVal, &shortVal);
        *maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(shortVal,SNOOP_IGMP_FP_DIVISOR);
      }
      else
      {
        *maxRespTime =
          pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;
      }
    }
    else /* MLD */
    {
      if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_MLD_VERSION_1)
      {
        shortVal = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
        *maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(shortVal,SNOOP_MLD_FP_DIVISOR);
      }
      else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_MLD_VERSION_2)
      {
        snoopMLDFPDecode(pSnoopOperEntry->snoopQuerierInfo.maxResponseCode,
                         &uintVal);
        *maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(uintVal,SNOOP_MLD_FP_DIVISOR);
      }
      else
      {
        *maxRespTime = pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;
      }
    }/* End of protocol check */
  }
  else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState ==
           SNOOP_QUERIER_QUERIER)

  {
    *maxRespTime =
      pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;
  }
  else
  {
    osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
    return L7_FAILURE;
  }
  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets whether the optional Router Alert field is required.
*
*
* @param    checkRtrAlert   @b{(input)}   L7_TRUE/L7_FALSE to enable/disable
*                                          router alert checking in IGMP frames
* @param    family          @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                         L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    Family field is not used currently
*
* @end
*********************************************************************/
L7_RC_t snoopRouterAlertMandatorySet(L7_BOOL checkRtrAlert, L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_NOT_EXIST;
  }

  if (checkRtrAlert != L7_TRUE && checkRtrAlert != L7_FALSE)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check = checkRtrAlert;
  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicates whether the optional Router Alert field is required.
*
* @param    checkRtrAlert   @b{(output)}   L7_TRUE/L7_FALSE to enable/disable
*                                          router alert checking in IGMP frames
* @param    family           @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                          L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
* @returns  L7_FAILURE
*
* @notes    Family field is not used currently
*
* @end
*********************************************************************/
L7_RC_t snoopRouterAlertMandatoryGet(L7_BOOL *checkRtrAlert, L7_uchar8 family)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_NOT_EXIST;
  }

  if (checkRtrAlert == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopOperDataAvlTree.semId, L7_WAIT_FOREVER);
  *checkRtrAlert = pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check;
  osapiSemaGive(pSnoopCB->snoopOperDataAvlTree.semId);
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Get the static mcast router attached status for the specified
*           interface of a snoop instance. List of VLANs on which
*           static mrouter is enabled are turned on in the vlanStaticMcastRtr
*           mask.
*
* @param    intIfNum    @b{(input)}   Internal interface number
* @param    vlanMask    @b{(output)}  Mcast router attached mode mask
* @param    family      @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopIntfApiVlanStaticMcastRtrMaskGet(L7_uint32 intIfNum,
                                              L7_VLAN_MASK_t *vlanStaticMcastRtr,
                                              L7_uchar8 family)
{
  snoopIntfCfgData_t  *pCfg;
  snoop_cb_t          *pSnoopCB = L7_NULLPTR;
 
  if (!vlanStaticMcastRtr)
  {
    return L7_ERROR;
  }
 
  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
 
  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
  {
    return L7_ERROR;
  }
 
  memcpy(vlanStaticMcastRtr, &pCfg->vlanStaticMcastRtr, sizeof(L7_VLAN_MASK_t));
  return L7_SUCCESS;
}

 

