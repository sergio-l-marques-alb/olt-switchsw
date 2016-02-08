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
* @comments   Improved by PTIN
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/

#include <unistd.h>
#include "datatypes.h"
#include "comm_structs.h"
#include "fdb_api.h"
#include "mfdb_api.h"

#include "snooping_db.h"
#include "snooping_util.h"
#include "snooping_ctrl.h"
#include "snooping_defs.h"
#include "snooping_api.h"
#include "ptin_translate_api.h"
#include "dtlinclude.h"

// These variables are defined outside functions, to prevent stack increasing too much
L7_uint16 number_of_channels;
L7_inet_addr_t channels_list[L7_MAX_GROUP_REGISTRATION_ENTRIES];

/**
 * Get IGMP Channels list based either on UNI VLAN or 
 * Client VLAN. 
 * 
 * @author M.Ruas 
 * 
 * @param cell_ptr      Pointer to the AVL Tree
 * @param vlanId        UNI VLAN
 * @param client_vlan   Client VLAN (!= UNI VLAN)
 * @param channel_list  Channels list (output)
 * @param num_channels  Number of channels (output)
 */
void igmp_get_channels_recursive(avlTreeTables_t *cell_ptr,
                                 L7_uint16 vlanId,
                                 L7_uint16 client_vlan,
                                 L7_inet_addr_t *channel_list,
                                 L7_uint16 *num_channels)
{
  snoopIpInfoData_t *entry;
  L7_uint direction;
  avlTreeTables_t *cell_ptr_prev;

  if (igmp_intercept_debug)
    printf("%s(%d): Input parameters: vlanId=%d  client_vlan=%d\n", __FUNCTION__, __LINE__, vlanId, client_vlan);

  if (cell_ptr == L7_NULLPTR)
    *num_channels = 0;

  /* Save current pointer to the tree */
  cell_ptr_prev = cell_ptr;

  for (direction = 0; direction <= 1; direction++) {

    cell_ptr = cell_ptr_prev;

    /* If maximum number of channels was reached, break for and abandon function */
    if (*num_channels >= L7_MAX_GROUP_REGISTRATION_ENTRIES)
      break;

    /* Get next entry in the AVL tree */
    entry = (snoopIpInfoData_t *) snoopEntryNext(&(snoopEBGet()->snoopIpAvlTree), &cell_ptr, direction);

    if (cell_ptr != L7_NULLPTR ) {

      /* Store channel ONLY if it meets svlan or cvlan filtering */
      if (entry != L7_NULLPTR && channel_list != L7_NULLPTR)
      {
        if (igmp_intercept_debug) {
          printf("%s(%d): entry->snoopIpInfoDataKey.vlanId=%u\n", __FUNCTION__, __LINE__, entry->snoopIpInfoDataKey.vlanId);
          printf("%s(%d): entry->clientList.clientVlan[client_vlan/32]>>(client_vlan%%32)=%u\n", __FUNCTION__, __LINE__,
                 entry->clientList.clientVlan[client_vlan / 32] >> (client_vlan % 32) & 1);
        }

        if ( ( vlanId == 0 || vlanId >= 4096 || entry->snoopIpInfoDataKey.vlanId == vlanId ) &&
             ( client_vlan == 0 || client_vlan >= 4096 || ((entry->clientList.clientVlan[client_vlan / 32] >> (client_vlan % 32)) & 1) ) ) {
          /* Add channel */
          channel_list[*num_channels].family = L7_AF_INET;
          channel_list[*num_channels].addr.ipv4.s_addr = entry->snoopIpInfoDataKey.channelIP;
          (*num_channels)++;

          if (igmp_intercept_debug)
            printf("%s(%d): Channel [vlanId=%u IP=%u] included on the list\n", __FUNCTION__, __LINE__, entry->snoopIpInfoDataKey.vlanId, entry->snoopIpInfoDataKey.channelIP);
        }
      }

      /* Recursive to the left */
      igmp_get_channels_recursive(cell_ptr, vlanId, client_vlan, channel_list, num_channels);

      if (cell_ptr_prev==L7_NULLPTR)
        break;
    }
  }
}

/* PTin added */

/**
 * Add a client VLAN to the clients' Bitmap of an AVL Tree entry
 * 
 * @author M.Ruas
 * 
 * @param entry       AVL Tree entry
 * @param client_vlan Client VLAN
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopClientVlanAdd(snoopIpInfoData_t *entry, L7_uint16 client_vlan, L7_uint32 intIfNum)
{
  L7_uint number_of_bits = sizeof(L7_uint32) * 8;
  L7_uint array_index, value_index;
  //L7_int  port;

  /* Validate interface */
  if (entry == L7_NULLPTR || client_vlan == 0 || client_vlan > 4095)
    return L7_FAILURE;

  array_index = client_vlan / number_of_bits;
  value_index = client_vlan % number_of_bits;

  /* Check if Client VLAN already exists */
  if ( !((entry->clientList.clientVlan[array_index] >> value_index) & 1) ) {
    /* Add new entry */
    entry->clientList.clientVlan[array_index] |= (L7_uint32) 1 << value_index;

    if (entry->clientList.number_of_clients < 4095)
      entry->clientList.number_of_clients++;

    // Increment number of clients
//  if (ptin_translate_intf2port(intIfNum, &port)==L7_SUCCESS && port<L7_SYSTEM_N_INTERF)
//  {
//    if (entry->clientList.number_of_clients_by_port[port]<(L7_uint16)-1)
//      entry->clientList.number_of_clients_by_port[port]++;
//  }
  }

  return L7_SUCCESS;
}


/**
 * Add a client VLAN to the Clients' Bitmap of an AVL Tree entry
 * 
 * @author M.Ruas
 * 
 * @param entry       AVL Tree entry
 * @param client_vlan Client VLAN
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopClientVlanRemove(snoopIpInfoData_t *entry, L7_uint16 client_vlan, L7_uint32 intIfNum)
{
  L7_uint number_of_bits = sizeof(L7_uint32) * 8;
  L7_uint array_index,
          value_index;
  //L7_int  port;

  /* Validate interface */
  if (entry == L7_NULLPTR || client_vlan == 0 || client_vlan > 4095)
    return L7_FAILURE;

  array_index = client_vlan / number_of_bits;
  value_index = client_vlan % number_of_bits;

  /* Check if Client VLAN exists */
  if ( ((entry->clientList.clientVlan[array_index]>>value_index) & 1) ) {    
    /* Remove entry */
    entry->clientList.clientVlan[array_index] &= ~((L7_uint32) 1 << value_index);

    if (entry->clientList.number_of_clients > 0)
      entry->clientList.number_of_clients--;

    // Decrement number of clients
//  if (ptin_translate_intf2port(intIfNum, &port)==L7_SUCCESS && port<L7_SYSTEM_N_INTERF)
//  {
//    if (entry->clientList.number_of_clients_by_port[port]>0)
//      entry->clientList.number_of_clients_by_port[port]--;
//  }
  }

  return L7_SUCCESS;
}


/**
 * Return L7_TRUE if there are no clients associated with a 
 * particular channel (AVL tree entry). 
 * 
 * @author M.Ruas
 * 
 * @param entry 
 * 
 * @return L7_BOOL 
 */
L7_BOOL snoopNoClientVlans(snoopIpInfoData_t *entry, L7_uint32 intIfNum)
{
  //L7_int port;

  /* Validate interface */
  if (entry == L7_NULLPTR)
    return L7_FAILURE;

  /* Check if table is empty */

//// Generic
//if (intIfNum==0 || intIfNum==L7_ALL_INTERFACES) {
    if (entry->clientList.number_of_clients == 0)
      return L7_TRUE;
//}
//// By port
//else if (ptin_translate_intf2port(intIfNum,&port)==L7_SUCCESS && port<L7_SYSTEM_N_INTERF)
//{
//  if (entry->clientList.number_of_clients_by_port[port] == 0)
//    return L7_TRUE;
//}
  
  /* Table is not empty */
  return L7_FALSE;
}

/**
 * Return L7_TRUE if there is a particular IP client associated 
 * with a given Channel (AVL tree entry). 
 * 
 * @author M.Ruas
 * 
 * @param entry 
 * 
 * @return L7_BOOL 
 */
L7_BOOL snoopIpClientExists(snoopInfoData_t *entry, L7_uint32 ipaddr)
{
  L7_uint number_of_bits = sizeof(L7_uint32) * 8;
  L7_uint value_index;
  L7_uint msb_5_9;

  /* Validate interface */
  if (entry == L7_NULLPTR || ipaddr == 0)
    return L7_FAILURE;

  /* Extract the 9 MSbits (23 LSb remain constant) */
  msb_5_9 = (ipaddr >> 23) & 0x1F;
  value_index = msb_5_9 % number_of_bits;

  if ( ((entry->ipList.channelIPs_bmp >> value_index) & 1) )
    return L7_TRUE;

  return L7_FALSE;
}


/**
 * Add an IP to the AVL tree entry.
 * 
 * @author M.Ruas
 * 
 * @param entry  AVL Tree entry
 * @param ipaddr IP Address
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopIpAdd(snoopInfoData_t *entry, L7_uint32 ipaddr)
{
  L7_uint number_of_bits = sizeof(L7_uint32) * 8;
  L7_uint value_index;
  L7_uint msb_5_9;

  /* Validate interface */
  if (entry == L7_NULLPTR || ipaddr == 0)
    return L7_FAILURE;

  /* Extract the 9 MSbits (23 LSb remain constant) */
  msb_5_9 = (ipaddr >> 23) & 0x1F;
  value_index = msb_5_9 % number_of_bits;

  /* If IP is not active, activate it */
  if ( !((entry->ipList.channelIPs_bmp>>value_index) & 1) ) {
    entry->ipList.channelIPs_bmp |= (L7_uint32) 1 << value_index;

    if (entry->ipList.number_of_ips < 32)
      entry->ipList.number_of_ips++;
  }

  return L7_SUCCESS;
}


/**
 * Remove an IP from the AVL tree entry.
 * 
 * @author M.Ruas
 * 
 * @param entry  AVL Tree entry
 * @param ipaddr IP Address
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopIpRemove(snoopInfoData_t *entry, L7_uint32 ipaddr)
{
  L7_uint number_of_bits = sizeof(L7_uint32) * 8;
  L7_uint value_index;
  L7_uint msb_5_9;

  /* Validate interface */
  if (entry == L7_NULLPTR || ipaddr == 0)
    return L7_FAILURE;

  /* Extract the 9 MSbits (23 LSb remain constant) */
  msb_5_9 = (ipaddr >> 23) & 0x1F;
  value_index = msb_5_9 % number_of_bits;

  /* If IP is active, deactivate it */
  if ( ((entry->ipList.channelIPs_bmp >> value_index) & 1) ) {
    entry->ipList.channelIPs_bmp &= ~((L7_uint32) 1<<value_index);

    if (entry->ipList.number_of_ips > 0)
      entry->ipList.number_of_ips--;
  }

  return L7_SUCCESS;
}


/**
 * Return L7_TRUE if there are IPs set in this AVL Tree entry
 * 
 * @author M.Ruas
 * 
 * @param entry 
 * 
 * @return L7_BOOL 
 */
L7_BOOL snoopNoIPs(snoopInfoData_t *entry)
{
  /* Validate interface */
  if (entry == L7_NULLPTR)
    return L7_FAILURE;

  /* Check if table is empty */
  if (entry->ipList.number_of_ips == 0)
    return L7_TRUE;
  
  /* Table is not empty */
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Finds next Multicast group membership entry
*
* @param    cell_ptr  @b{(input)} pointer to avl tree cell
* @param    direction @b{(input)} Direction to search for in the AVL tree
* 
* @returns  null pointer on failure
* @returns  multicast entry pointer on success
*
* @notes    none
*
* @end
*********************************************************************/
void *snoopEntryNext(avlTree_t *avlTree, avlTreeTables_t **cell_ptr, L7_uint direction)
{
  void *snoopEntry;
  //snoop_eb_t      *pSnoopEB;

  //pSnoopEB = snoopEBGet();

  snoopEntry = avlGetNextLVL7 (avlTree, cell_ptr, direction);

  if (snoopEntry == L7_NULL) {
    return L7_NULLPTR;
  }

  return snoopEntry;
}


/**
 * Finds an entry on the IP AVL Tree (key: Uni VLAN and 
 * Channel IP)
 * 
 * @author M.Ruas
 * 
 * @param svlan     Service VLAN
 * @param channelIP Channel IP
 * 
 * @return snoopIpInfoData_t* 
 */
snoopIpInfoData_t *snoopIpEntryFind(L7_uint16 vlanId, L7_uint32 channelIP)
{
  snoopIpInfoData_t    *snoopIpEntry;
  snoopIpInfoDataKey_t  key;
  snoop_eb_t           *pSnoopEB;

  pSnoopEB = snoopEBGet();
  memset((void *)&key, 0x00, sizeof(snoopIpInfoDataKey_t));

  key.vlanId     = vlanId;
  key.channelIP = channelIP;

  snoopIpEntry = avlSearchLVL7(&pSnoopEB->snoopIpAvlTree, &key, AVL_EXACT);

  if (snoopIpEntry == L7_NULL)
    return L7_NULLPTR;

  return snoopIpEntry;
}


/**
 * Add a channel to the AVL Tree
 * 
 * @author M.Ruas
 * 
 * @param vlanId       UNI VLAN
 * @param channelIP   Channel IP
 * @param staticEntry Is static (manual) entry ?
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopIpEntryAdd(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL staticEntry )
{
  snoopIpInfoData_t  snoopIpEntry;
  snoopIpInfoData_t *pData;
  snoop_eb_t        *pSnoopEB;

  memset(&snoopIpEntry, 0x00, sizeof(snoopIpInfoData_t));
  pSnoopEB = snoopEBGet();

  snoopIpEntry.snoopIpInfoDataKey.vlanId    = vlanId;
  snoopIpEntry.snoopIpInfoDataKey.channelIP = channelIP;
  snoopIpEntry.staticEntry                  = staticEntry;

  pData = avlInsertEntry(&pSnoopEB->snoopIpAvlTree, &snoopIpEntry);

  if (pData == L7_NULL) {
    /* Entry was successfully added into the avl tree */
    /* PTin removed: not necessary */
//  if ((pData = snoopIpEntryFind(vlanId, channelIP)) == L7_NULLPTR) {
//    LOG_MSG("snoopIpEntryAdd: Failed to find recently added entry in snoopIpTree\n");
//    return L7_FAILURE;
//  }

    return L7_SUCCESS;
  }
  else if (pData == &snoopIpEntry) {
    /* some error in avl tree addition */
    return L7_FAILURE;
  }
  else {
    /* entry already exists */
    return L7_FAILURE;
  }
}


/**
 * Remove a channel from the AVL Tree
 * 
 * @author M.Ruas
 * 
 * @param svlan     Service VLAN
 * @param channelIP Channel IP
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopIpEntryDelete(L7_uint16 vlanId, L7_uint32 channelIP)
{
  snoopIpInfoData_t *pData;
  snoopIpInfoData_t *snoopIpEntry;
  snoop_eb_t        *pSnoopEB;

  pSnoopEB = snoopEBGet();

  pData = snoopIpEntryFind(vlanId, channelIP);

  if (pData == L7_NULLPTR)
    return L7_FAILURE;

  snoopIpEntry = pData;

  pData = avlDeleteEntry(&pSnoopEB->snoopIpAvlTree, pData);

  if (pData == L7_NULL) {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  else if (pData == snoopIpEntry) {
    /* Entry deleted */
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}


/**
 * Create an IP entry in the AVL Tree
 * 
 * @author M.Ruas
 * 
 * @param vlanId 
 * @param channelIP 
 * @param intfList 
 * @param nIntf 
 * @param staticEntry 
 * @param sendJoins 
 * 
 * @return L7_RC_t 
 */
L7_RC_t snoopIpEntryCreate(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL staticEntry, L7_BOOL *sendJoins)
{
  L7_int port;
  L7_uint i;
  L7_uint16 vid;
  L7_uint32 intf;
  L7_uint32 ipaddr;
  snoopInfoData_t   *snoopEntry   = L7_NULLPTR;
  snoopIpInfoData_t *snoopIpEntry = L7_NULLPTR,
                    *snoopIpEntryTmp;
  L7_uchar8 macAddr[L7_ENET_MAC_ADDR_LEN];

  if (igmp_intercept_debug)
    printf("%s(%d) starting processing\n", __FUNCTION__, __LINE__);

  snoopEntry   = snoopEntryFind(macAddr, vlanId, L7_AF_INET, L7_MATCH_EXACT);
  snoopIpEntry = snoopIpEntryFind(vlanId, channelIP);

  /* Determine MAC address */
  macAddr[0] = 0x01;
  macAddr[1] = 0x00;
  macAddr[2] = 0x5E;
  macAddr[3] = (L7_uint8) ((channelIP>>16) & 0x7F);
  macAddr[4] = (L7_uint8) ((channelIP>> 8) & 0xFF);
  macAddr[5] = (L7_uint8) ( channelIP & 0xFF );

  /* For static entries, also create snoop entry (static) */

  /* If snoop entry don't exist, create it */
  if (snoopEntry == L7_NULLPTR) {
    if (snoopEntryCreate(macAddr, vlanId, L7_AF_INET, staticEntry) != L7_SUCCESS) {
      if (igmp_intercept_debug)
        printf("%s(%d) Error creating snoop entry\n", __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }

    if ((snoopEntry = snoopEntryFind(macAddr, vlanId, L7_AF_INET, L7_MATCH_EXACT)) == L7_NULLPTR) {
      if (igmp_intercept_debug)
        printf("%s(%d) I did not find snoop entry after creation\n", __FUNCTION__, __LINE__);

      return L7_FAILURE;
    }

    if (igmp_intercept_debug)
      printf("%s(%d) Snoop entry created successfully\n", __FUNCTION__, __LINE__);

    if (igmp_intercept_debug) {
      printf("%s(%d) dmac=%02X:%02X:%02X:%02X:%02X:%02X, svlan=%u, family=%u, cbHandle=%u\n", __FUNCTION__, __LINE__,
             macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5],
             vlanId, L7_AF_INET, (L7_uint32) snoopCBGet(L7_AF_INET));
    }
  }
  else {
    if (igmp_intercept_debug)
      printf("%s(%d) Snoop entry already exists\n", __FUNCTION__, __LINE__);
  }

  /* if new entry is dynamic, and there is an equal entry but static, return error */
  if (!staticEntry && snoopEntry->staticEntry) { 
    if (igmp_intercept_debug)
      printf("%s(%d) Cannot convert from static to dynamic\n", __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  /* If new entry is static, and there is an equal entry but dynamic, convert it to static */
  else if (staticEntry && !snoopEntry->staticEntry) {
    /* Convert dynamic to static */
    for (port=0; port<L7_SYSTEM_N_INTERF; port++) {
      if (ptin_translate_port2intf(port, &intf) == L7_SUCCESS &&
          L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intf)) {
        /* Stop timer */
        snoopTimerStop(snoopEntry, intf, vlanId, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET));

        /* Determine original vlan */
        if (ptin_intif_vlan_translate_get(intf, snoop_mcast_vlan[vlanId], &vid) == L7_SUCCESS) {
          mfdbEntryModifyType(vid, macAddr, L7_MFDB_TYPE_STATIC);
        }
      }
    }
    /* Snoop Entry is now static! */
    snoopEntry->staticEntry = L7_TRUE;

    /* Convert all dynamic IPs to static (of this snoopEntry) */
    for (i=0; i<32; i++) {
      ipaddr = ((L7_uint32) 0xE0000000) | (((L7_uint32) i & 0x1F) << 23) | (channelIP & 0x7FFFFF);

      if ((snoopIpEntryTmp = snoopIpEntryFind(vlanId, ipaddr)) != L7_NULLPTR)
        snoopIpEntryTmp->staticEntry = L7_TRUE;
    }
  }

  /* Add interfaces for static entries */
  if (staticEntry) {
    /* Add remaining interfaces */
    for (port=0; port<L7_SYSTEM_PON_PORTS; port++) {
      if (ptin_translate_port2intf(port, &intf) == L7_SUCCESS &&
          ptin_vlan_translate_get(port, vlanId, L7_NULLPTR) == L7_SUCCESS &&
          ptin_vlan_translate_get(port, snoop_mcast_vlan[vlanId], L7_NULLPTR) == L7_SUCCESS)
      {
        if (snoopIntfAdd(macAddr, vlanId, intf, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET)) != L7_SUCCESS) {
          if (igmp_intercept_debug)
            printf("%s(%d) Cannot add interface %u\n", __FUNCTION__, __LINE__, intf);

          return L7_FAILURE;
        }
        else if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intf)) {
          if (igmp_intercept_debug)
            printf("%s(%d) Interface %u added\n", __FUNCTION__, __LINE__, intf);
        }
        else {
          if (igmp_intercept_debug)
            printf("%s(%d) Strange... Some problem adding Interface %u\n", __FUNCTION__, __LINE__, intf);
        }       
      }
    }
  }

  /* Add IP, if it dows not exist */
  if (snoopIpEntry == L7_NULLPTR) {
    if (snoopIpEntryAdd(vlanId, channelIP, staticEntry) != L7_SUCCESS) {
      if (igmp_intercept_debug)
        printf("%s(%d) Error creating Ip entry\n", __FUNCTION__, __LINE__);

      return L7_FAILURE;
    }

    /* PTin removed: not necessary */
//  if ((snoopIpEntry = snoopIpEntryFind(vlanId, channelIP)) == L7_NULLPTR) {
//    if (igmp_intercept_debug)
//      printf("%s(%d) I did not find Ip entry after creation\n", __FUNCTION__, __LINE__);
//
//    return L7_FAILURE;
//  }

    if (igmp_intercept_debug)
      printf("%s(%d) Ip entry created successfully\n", __FUNCTION__, __LINE__);
  }
  else {
    if (igmp_intercept_debug)
      printf("%s(%d) IP entry already exists\n", __FUNCTION__, __LINE__);

    /* Otherwise, make sure it is converted to static */
    snoopIpEntry->staticEntry = staticEntry;

    if (igmp_intercept_debug)
      printf("%s(%d) IP entry is now static\n", __FUNCTION__, __LINE__);
  }

  /* Mark this IP in table */
  if (snoopIpAdd(snoopEntry, channelIP) != L7_SUCCESS) {
    if (igmp_intercept_debug)
      printf("%s(%d) Error adding IP to snoop entry\n", __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  if (igmp_intercept_debug)
    printf("%s(%d) Success\n", __FUNCTION__, __LINE__);

  if (sendJoins != L7_NULLPTR)
    *sendJoins = L7_TRUE;

  return L7_SUCCESS;
}


L7_RC_t snoopIpEntryRemove(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL *sendLeaves)
{
  L7_int port;
  L7_uint i;
  L7_uint16 vid;
  L7_uint32 intf;
  L7_uint32 ipaddr;
  snoopInfoData_t   *snoopEntry   = L7_NULLPTR;
  snoopIpInfoData_t *snoopIpEntry = L7_NULLPTR,
                    *snoopIpEntryTmp;
  L7_uchar8 macAddr[L7_ENET_MAC_ADDR_LEN];

  if (igmp_intercept_debug)
    printf("%s(%d) starting processing\n", __FUNCTION__, __LINE__);

  // Determine MAC address
  macAddr[0] = 0x01;
  macAddr[1] = 0x00;
  macAddr[2] = 0x5E;
  macAddr[3] = (L7_uint8) ((channelIP>>16) & 0x7F);
  macAddr[4] = (L7_uint8) ((channelIP>> 8) & 0xFF);
  macAddr[5] = (L7_uint8) ( channelIP      & 0xFF);

  snoopEntry   = snoopEntryFind(macAddr, vlanId, L7_AF_INET, L7_MATCH_EXACT);
  snoopIpEntry = snoopIpEntryFind(vlanId, channelIP);

  // IP exists...
  if (snoopIpEntry != L7_NULLPTR)
  {
    // If there are clients associated, convert this entry to dynamic
    if (snoopEntry!=L7_NULLPTR && snoopIpEntry->staticEntry && !snoopNoClientVlans(snoopIpEntry,L7_ALL_INTERFACES))
    {
      if (igmp_intercept_debug)
        printf("%s(%d) there are clients connected!\r\n", __FUNCTION__, __LINE__);

      for (port=0; port<L7_SYSTEM_N_INTERF; port++)
      {
        if (ptin_translate_port2intf(port, &intf) == L7_SUCCESS &&
            L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, intf))
        {
          // Determine original vlan
          if (ptin_intif_vlan_translate_get(intf, snoop_mcast_vlan[vlanId], &vid)==L7_SUCCESS)
          {
            mfdbEntryModifyType(vid, macAddr, L7_MFDB_TYPE_DYNAMIC);
          }
          // Start timer
          if (snoopTimerStart(snoopEntry, intf, vlanId, SNOOP_GROUP_MEMBERSHIP, snoopCBGet(L7_AF_INET))!= L7_SUCCESS)
            break;
        }
      }
      // Success
      if (port >= L7_SYSTEM_N_INTERF)
      {
        if (igmp_intercept_debug)
          printf("%s(%d) Change to dynamic successfull!\r\n", __FUNCTION__, __LINE__);

        snoopEntry->staticEntry = L7_FALSE;
        snoopIpEntry->staticEntry = L7_FALSE;

        // Convert all static IPs to dynamic (of this snoopEntry)
        for (i=0; i<32; i++)
        {
          ipaddr = ((L7_uint32) 0xE0000000) | ((L7_uint32) i & 0x1F)<<23 | (channelIP & 0x7FFFFF);
          if ((snoopIpEntryTmp = snoopIpEntryFind(vlanId, ipaddr)) != L7_NULLPTR)
            snoopIpEntryTmp->staticEntry = L7_FALSE;
        }

        // Done!
        if (sendLeaves != L7_NULLPTR)
          *sendLeaves = L7_FALSE;

        return L7_SUCCESS;
      }

      if (igmp_intercept_debug)
        printf("%s(%d) An error ocurred!\r\n", __FUNCTION__, __LINE__);

      // Convert all IPs of this snoopEntry to dynamic!
    }
    else {
      if (igmp_intercept_debug)
        printf("%s(%d) No Clients!\r\n", __FUNCTION__, __LINE__);
    }
    
    // Remove IP channel
    if (snoopIpEntryDelete(vlanId,channelIP)!=L7_SUCCESS)
    {
      if (igmp_intercept_debug)
        printf("%s(%d) Error deleting IP entry\n", __FUNCTION__, __LINE__);

      return L7_FAILURE;
    }
  }
  else
  {
    if (igmp_intercept_debug)
      printf("%s(%d) This IP entry does not exist\n", __FUNCTION__, __LINE__);
  }

  // Search for the associated snoop Entry
  if (snoopEntry != L7_NULLPTR)
  {  
    // Unmark this IP in snoop entry
    if (snoopIpRemove(snoopEntry, channelIP) != L7_SUCCESS)
    {
      if (igmp_intercept_debug)
        printf("%s(%d) Error removing IP from snoop entry\n", __FUNCTION__, __LINE__);
    }
    else
    {
      if (igmp_intercept_debug)
        printf("%s(%d) IP removed from snoop entry\n", __FUNCTION__, __LINE__);
    }
  
    // If snoopEntry exists, and is static, and there is no more IPs associated to it, remove it!
    if (snoopNoIPs(snoopEntry))
    {
      if (igmp_intercept_debug)
        printf("%s(%d) No IPs left... going to remove snoop entry\n", __FUNCTION__, __LINE__);
      if (snoopEntryRemove(macAddr, vlanId, L7_AF_INET)!=L7_SUCCESS)
      {
        if (igmp_intercept_debug)
          printf("%s(%d) Error removing snoop entry\n", __FUNCTION__, __LINE__);
        return L7_FAILURE;
      }
      else
      {
        if (igmp_intercept_debug)
          printf("%s(%d) snoop entry removed\n", __FUNCTION__, __LINE__);
      }
    }
  }

  if (igmp_intercept_debug)
    printf("%s(%d) Success\n", __FUNCTION__, __LINE__);

  if (sendLeaves != L7_NULLPTR)
    *sendLeaves = L7_TRUE;

  return L7_SUCCESS;
}


L7_RC_t snoopStaticIpAdd(L7_uint16 vlanId, L7_uint32 channelIP)
{
  L7_BOOL sendJoins = L7_TRUE;

  if (vlanId <= 1 || vlanId >= 4094 || channelIP == 0)
    return L7_FAILURE;

  if (igmp_intercept_debug)
    printf("%s(%d)\n", __FUNCTION__, __LINE__);

  if (snoopIpEntryCreate(vlanId, channelIP, L7_TRUE, &sendJoins) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Send two joins */
  if (sendJoins) {
    if (igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_MEMBERSHIP_REPORT, channelIP) != L7_SUCCESS)
      return L7_FAILURE;

    if (igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_MEMBERSHIP_REPORT, channelIP) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t snoopStaticIpRemove(L7_uint16 vlanId, L7_uint32 channelIP)
{
  L7_BOOL sendLeaves=L7_TRUE;

  if (vlanId<=1 || vlanId>=4094 || channelIP==0)
    return L7_FAILURE;

  // First of all, remove the IP from tables
  if (snoopIpEntryRemove(vlanId, channelIP, &sendLeaves) != L7_SUCCESS)
    return L7_FAILURE;

  if (sendLeaves) {
    // Now, we can send Leaves
    if (igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_LEAVE_GROUP, channelIP) != L7_SUCCESS)
      return L7_FAILURE;
    if (igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_LEAVE_GROUP, channelIP) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}


static void snoopStaticIpRemoveAll_recursive(avlTreeTables_t *cell_ptr)
{
  snoopIpInfoData_t *entry;
  L7_uint direction;
  avlTreeTables_t *cell_ptr_prev;

  // Save current pointer to tree
  cell_ptr_prev = cell_ptr;

  for (direction=0; direction<=1; direction++)  {

    cell_ptr = cell_ptr_prev;

    // Get next entry in the AVL tree

    entry=(snoopIpInfoData_t *) snoopEntryNext(&(snoopEBGet()->snoopIpAvlTree), &cell_ptr, direction);

    if (cell_ptr!=L7_NULLPTR)  {

      // If entry is static, remove it!
      if ( entry!=L7_NULLPTR && entry->staticEntry ) {
        (void) snoopStaticIpRemove(entry->snoopIpInfoDataKey.vlanId, entry->snoopIpInfoDataKey.channelIP);
      }

      // Recursive to the left
      snoopStaticIpRemoveAll_recursive(cell_ptr);

      if (cell_ptr_prev==L7_NULLPTR)  break;
    }
  }
}

static void snoopStaticIpRefreshAll_recursive(avlTreeTables_t *cell_ptr)
{
  snoopIpInfoData_t *entry;
  L7_uint direction;
  avlTreeTables_t *cell_ptr_prev;
  L7_uint16 vlanId;
  L7_uint32 channelIP;

  // Save current pointer to tree
  cell_ptr_prev = cell_ptr;

  for (direction=0; direction<=1; direction++)  {

    cell_ptr = cell_ptr_prev;

    // Get next entry in the AVL tree

    entry=(snoopIpInfoData_t *) snoopEntryNext(&(snoopEBGet()->snoopIpAvlTree), &cell_ptr, direction);

    if (cell_ptr!=L7_NULLPTR)  {

      // If entry is static, remove it!
      if ( entry!=L7_NULLPTR && entry->staticEntry ) {
        vlanId = entry->snoopIpInfoDataKey.vlanId;
        channelIP = entry->snoopIpInfoDataKey.channelIP;
        (void) snoopStaticIpRemove(vlanId,channelIP);
        (void) snoopStaticIpAdd(vlanId,channelIP);
      }

      // Recursive to the left
      snoopStaticIpRefreshAll_recursive(cell_ptr);

      if (cell_ptr_prev==L7_NULLPTR)  break;
    }
  }
}


// Remove all static channels
L7_RC_t snoopStaticIpRemoveAll(void)
{
  snoopStaticIpRemoveAll_recursive(L7_NULLPTR);

  return L7_SUCCESS;
}

// Remove all static channels
L7_RC_t snoopStaticIpRefreshAll(void)
{
  snoopStaticIpRefreshAll_recursive(L7_NULLPTR);

  return L7_SUCCESS;
}


// Remove one client from all channels
//  vlanId: this is the service vlan (UNI)
//  clientVlanId: this the client vlan
//
L7_RC_t snoopClientRemove(L7_uint16 vlanId, L7_uint16 clientVlanId, L7_uint32 intIfNum)
{
  L7_uint16 i;
//L7_uint16 number_of_channels, i;
//L7_inet_addr_t channels_list[L7_MAX_GROUP_REGISTRATION_ENTRIES];
  snoopIpInfoData_t *ptr;
  L7_uint32 channelIP;
  L7_BOOL sendLeaves;
  L7_RC_t result;

  if (igmp_intercept_debug)
    printf("%s(%d) vlanId=%u, cVlanId=%u\r\n",__FUNCTION__,__LINE__,vlanId,clientVlanId);

  // Validate arguments
  if (vlanId<=L7_DOT1Q_MIN_VLAN_ID || vlanId>L7_DOT1Q_MAX_VLAN_ID || clientVlanId==0 || clientVlanId>4095)
  {
    return L7_FAILURE;
  }

  if (igmp_intercept_debug)
    printf("%s(%d)\r\n",__FUNCTION__,__LINE__);

  // Extract list of channels
  igmp_get_channels_recursive(L7_NULLPTR, vlanId, clientVlanId, channels_list, &number_of_channels);

  if (igmp_intercept_debug)
    printf("%s(%d) number_of_channels=%u\r\n",__FUNCTION__,__LINE__,number_of_channels);

  // If no channels were found, there is nothing to do
  if (number_of_channels==0)
  {
    return L7_SUCCESS;
  }

  result = L7_SUCCESS;

  // For each channel:
  // 1. remove this client
  // 2. check if there is any client
  // 3. if there isn't:
  //   3.a. remove the channel
  //   3.b. send leaves
  for (i=0; i<number_of_channels; i++)
  {
    channelIP = channels_list[i].addr.ipv4.s_addr;

    if (igmp_intercept_debug)
      printf("%s(%d) Going to remove client %u in channel 0x%08X\r\n",__FUNCTION__,__LINE__,clientVlanId, channelIP);

    // Pointer in avl tree
    if ((ptr=snoopIpEntryFind(vlanId, channelIP))==L7_NULLPTR)
      continue;

    // Remove client
    if (snoopClientVlanRemove(ptr, clientVlanId, intIfNum)!=L7_SUCCESS)
    {
      result = L7_FAILURE;
      continue;
    }

    if (igmp_intercept_debug)
      printf("%s(%d) Client %u removed\r\n",__FUNCTION__,__LINE__,clientVlanId);

    // Is this channel still being consumed (only remove dynamic channels)
    if (snoopNoClientVlans(ptr,L7_ALL_INTERFACES) && !ptr->staticEntry)
    {
      if (igmp_intercept_debug)
        printf("%s(%d) There is no clients in channel 0x%08X\r\n",__FUNCTION__,__LINE__,channelIP);

      // remove the IP channel from tables
      if (snoopIpEntryRemove(vlanId, channelIP, &sendLeaves) != L7_SUCCESS)
      {
        result = L7_FAILURE;
        continue;
      }

      if (igmp_intercept_debug)
        printf("%s(%d) Channel 0x%08X removed\r\n",__FUNCTION__,__LINE__,channelIP);
      
      if (sendLeaves)
      {
        if (igmp_intercept_debug)
          printf("%s(%d) Going to send leaves\r\n",__FUNCTION__,__LINE__);

        // Now, we can send Leaves
        if (igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_LEAVE_GROUP, channelIP)!=L7_SUCCESS ||
            igmp_generate_packet_and_send(vlanId, L7_IGMP_V2_LEAVE_GROUP, channelIP)!=L7_SUCCESS)
        {
          result = L7_FAILURE;
          continue;
        }
        if (igmp_intercept_debug)
          printf("%s(%d) Leaves sent\r\n",__FUNCTION__,__LINE__);
      }
    }
//  else if (intIfNum!=0 && intIfNum!=L7_ALL_INTERFACES && snoopNoClientVlans(ptr,intIfNum))
//  {
//
//  }
    else
    {
      if (igmp_intercept_debug)
        printf("%s(%d) This channel still have clients... do no more\r\n",__FUNCTION__,__LINE__);
    }
  }

  return result;
}


// PTin end


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
L7_RC_t snoopEntryAdd(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticEntry)
{
  snoopInfoData_t    snoopEntry;
  snoopInfoData_t   *pData;
  L7_ushort16        vid;
  L7_uint32          ivlLength = 0, freeIdx;
  L7_FDB_TYPE_t      fdbType;
  snoop_eb_t        *pSnoopEB;

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

  // PTin added
  snoopEntry.staticEntry = staticEntry;

  // PTin added
  //if (!staticEntry)
  //{
    /* Create linked list for group membership timers for this snoop entry */
    if (SLLCreate(L7_SNOOPING_COMPONENT_ID, L7_SLL_NO_ORDER,
                 sizeof(L7_uint32), snoopTimerDataCmp, snoopTimerDataDestroy, 
                 &(snoopEntry.ll_timerList)) != L7_SUCCESS)
    {
      LOG_MSG("snoopEntryAdd: Failed to create timer linked list \n");
      return L7_FAILURE;
    }
//}
//else {
//  snoopEntry.ll_timerList.sllStart = L7_NULL;
//  snoopEntry.ll_timerList.sllEnd = L7_NULL;
//}

  pData = avlInsertEntry(&pSnoopEB->snoopAvlTree, &snoopEntry);

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/
    /* Create a TimerCB for this entry */
    if ((pData = snoopEntryFind(macAddr, vlanId, family, AVL_EXACT))  == L7_NULLPTR)
    {
      LOG_MSG("snoopEntryAdd: Failed to find recently added entry in snoopTree \n");
      return L7_FAILURE;
    }

    // PTin added
    // Only manage timers, if entry is dynamic
//  if (!staticEntry)
//  {
      freeIdx = pSnoopEB->snoopEntryTimerFreeIdx;
      if (freeIdx >= SNOOP_ENTRY_TIMER_BLOCKS_COUNT)
      {
        LOG_MSG("snoopEntryAdd: Free Timer CB Index out of bounds %d\n", freeIdx);
        (void)snoopEntryDelete(macAddr, vlanId, family);
        return L7_FAILURE;
      }
  
      pData->timerIdx = pSnoopEB->snoopEntryTimerFreeList[freeIdx];
      if (pData->timerIdx == -1)
      {
        LOG_MSG("snoopEntryAdd: Free Timer CB could not be found\n");
        (void)snoopEntryDelete(macAddr, vlanId, family);
        return L7_FAILURE;
      }
  
      pData->timerCB = pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].timerCB;
      pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].pSnoopEntry = pData;
      if (pData->timerCB == L7_NULLPTR)
      {
        LOG_MSG("snoopEntryAdd: Timer CB could not be initialized for the snoopEntry \n");
        (void)snoopEntryDelete(macAddr, vlanId, family);
        return L7_FAILURE;
      }
      pSnoopEB->snoopEntryTimerFreeList[freeIdx] = -1;
      pSnoopEB->snoopEntryTimerFreeIdx++;
      appTimerProcess(pData->timerCB);/* Kick the timer as it might have slept by now */
//  }
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
  L7_INTF_MASK_t    zeroMask;
  L7_uint32         freeIdx;

  pSnoopEB = snoopEBGet();

  pData = snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  snoopEntry = pData;

  memset(&zeroMask, 0x00, sizeof(L7_INTF_MASK_t));
  snoopNotifyL3Mcast(macAddr, vlanId, &zeroMask);

  // PTin added
//if (!pData->staticEntry)
//{
    if (SLLDestroy(L7_SNOOPING_COMPONENT_ID, &snoopEntry->ll_timerList)
                                                 != L7_SUCCESS)
    {
      LOG_MSG("snoopEntryDelete: Failed to destroy the timer linked list \n");
    }

    freeIdx = pSnoopEB->snoopEntryTimerFreeIdx;
    if (freeIdx == L7_NULL)
    {
      LOG_MSG("snoopEntryDelete: Invalid snoopEntryTimerFreeIdx expecting > 0 has %d\n",
               pSnoopEB->snoopEntryTimerFreeIdx);
    }

    freeIdx = pSnoopEB->snoopEntryTimerFreeIdx - 1;
      pData->timerCB = L7_NULLPTR;
    pSnoopEB->snoopEntryTimerCBList[pData->timerIdx].pSnoopEntry = L7_NULLPTR;
    if (pSnoopEB->snoopEntryTimerFreeList[freeIdx] != -1)
    {
      LOG_MSG("snoopEntryDelete: Mismatch in snoopEntryTimerFreeList expecting -1 has %d\n",
               pSnoopEB->snoopEntryTimerFreeList[freeIdx]);
    }
    pSnoopEB->snoopEntryTimerFreeList[freeIdx] = pData->timerIdx;
//}
  pData = avlDeleteEntry(&pSnoopEB->snoopAvlTree, pData);

  if (pData == L7_NULL)
  {
    /* Entry does not exist */
    return L7_FAILURE;
  }
  if (pData == snoopEntry)
  {
    // PTin added
    if (pSnoopEB->snoopEntryTimerFreeIdx!=L7_NULL)
      pSnoopEB->snoopEntryTimerFreeIdx--;
    /* Entry deleted */
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
                         L7_uchar8 family, L7_BOOL staticEntry)
{
  mfdbMemberInfo_t  mfdb;
  L7_RC_t           rc;
  L7_uint16         mfdbVlan, realVlan, vlans_checked;
  L7_int            port, xlate_done;

  /* Check for illegal IP multicast MAC addresses*/
  rc = snoopMacAddrCheck(macAddr, family);
  if (rc == L7_FAILURE)
  {
    /* Illegal IP Multicast MAC address */
    return L7_FAILURE;
  }

  /* Does an entry with the same mac addr and vlan id already exist? */
  if (snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT) != L7_NULLPTR)
  {
    /* Entry exists... don't add another one, return success */
    return L7_SUCCESS;
  }

  /* Add entry to the MFDB */
  memset((L7_uchar8 *)&mfdb, 0, sizeof(mfdb));
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId = vlanId;
  mfdb.user.componentId = (family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP 
                                                 : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type = (staticEntry) ? L7_MFDB_TYPE_STATIC : L7_MFDB_TYPE_DYNAMIC;  // PTin modified
  strncpy(mfdb.user.description, L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  // PTin added
  // Add all effective vlans to mfdb table
  mfdbVlan = (vlanId<4096) ? snoop_mcast_vlan[vlanId] : vlanId;
  xlate_done=L7_FALSE;
  vlans_checked=0;
  for (port=0; port<L7_SYSTEM_N_INTERF && rc==L7_SUCCESS; port++)
  {
    rc = L7_SUCCESS;

    if (ptin_vlan_translate_get(port,mfdbVlan,&realVlan)==L7_SUCCESS && realVlan>0 && realVlan<=4095)
    {
      xlate_done = L7_TRUE;
    }
    else if (port<L7_SYSTEM_N_INTERF-1 || xlate_done)
    {  
      continue;
    }
    else
    {  
      realVlan = mfdbVlan;
    }

    if (igmp_intercept_debug)
    {
      printf("%s(%d) Processing vlan %u (translated to %u) in port %d => realvlan=%u\n", __FUNCTION__, __LINE__,vlanId,mfdbVlan,port,realVlan);
    }

    mfdb.vlanId = realVlan;

    if (mfdbEntryExist(macAddr,realVlan /*vlanId*/) == L7_NOT_EXIST)
    {
      rc=mfdbEntryAdd(&mfdb);
    }

    if (rc==L7_SUCCESS)  {
      vlans_checked++;
    }
  }

  if (rc==L7_SUCCESS && vlans_checked>0)
  {
    rc = snoopEntryAdd(macAddr, vlanId, family, staticEntry);
  }
  else
  {
    rc = L7_FAILURE;
  }
  // PTin end

//rc = mfdbEntryExist(macAddr, mfdb.vlanId /*vlanId*/);
//if (rc == L7_NOT_EXIST)
//{
//  rc = mfdbEntryAdd(&mfdb);
//  if (rc == L7_SUCCESS)
//  {
//    rc = snoopEntryAdd(macAddr, vlanId, family);
//  }
//}
//else if (rc == L7_SUCCESS)
//{
//  rc = snoopEntryAdd(macAddr, vlanId, family);
//}
//else
//{
//  rc = L7_FAILURE;
//}
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
  L7_uint16         realVlan, mfdbVlan;

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
      // PTin added
      if (!snoopEntry->staticEntry)
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
    LOG_MSG("snoopIntfAdd: Invalid interface type \n");
    return L7_FAILURE;
  }

  /* Add interface to the MFDB */
  memset((L7_uchar8 *)&mfdb, 0x00, sizeof(mfdb));
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (pSnoopCB->family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP 
                                                 : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (snoopEntry->staticEntry) ? L7_MFDB_TYPE_STATIC : L7_MFDB_TYPE_DYNAMIC;  // PTin modified
  memcpy((void *)mfdb.user.description, (void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  /* Set the mask bit in the fwd mask for this interface */
  L7_INTF_SETMASKBIT(mfdb.user.fwdPorts, intIfNum);

  // PTin added
  mfdbVlan = (vlanId<4096) ? snoop_mcast_vlan[vlanId] : vlanId;
  // Determine translated vlan
  if (ptin_intif_vlan_translate_get(intIfNum,mfdbVlan,&realVlan)!=L7_SUCCESS || realVlan==0 || realVlan>4095)
  {
    realVlan = mfdbVlan;
  }
  mfdb.vlanId = realVlan;

  if (igmp_intercept_debug)
  {
    printf("%s(%d) Processing vlan %u (translated to %u) in port %d => realvlan=%u\n", __FUNCTION__, __LINE__,vlanId,mfdbVlan,intIfNum-1,realVlan);
  }

  /* Check if this entry exists */
  if (mfdbEntryExist(macAddr, realVlan /*vlanId*/) != L7_SUCCESS)
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
      // PTin modified
      if (!snoopEntry->staticEntry &&
          snoopTimerStart(snoopEntry, intIfNum, vlanId, intfType, pSnoopCB) != L7_SUCCESS )
      {
        LOG_MSG("snoopIntfAdd: Failed to start timer \n");
        /* Roll back the changes made */
        if (mfdbEntryPortsDelete(&mfdb) != L7_SUCCESS)
        {
          LOG_MSG("snoopIntfAdd: Failed to delete port from MFDB\n");
        }
        L7_INTF_NONZEROMASK(snoopEntry->snoopGrpMemberList, flag);
        if (flag == 0)
        {
          /* PTin added: IGMP snooping */
          #if 1
          if (!snoopEntry->staticEntry)
          #endif
          {
            if (snoopEntryRemove(macAddr, vlanId, pSnoopCB->family )
                                 != L7_SUCCESS)
            {
              LOG_MSG("snoopIntfAdd: Failed to remove snoop entry\n");
            }
          }
        }
        return L7_FAILURE;
      }
      L7_INTF_SETMASKBIT(snoopEntry->snoopGrpMemberList, intIfNum);
      snoopNotifyL3Mcast(macAddr, vlanId, &snoopEntry->snoopGrpMemberList);
    } 
    else
    {
      L7_INTF_SETMASKBIT(snoopEntry->snoopMrtrMemberList, intIfNum);
    }
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
  L7_uint16         realVlan, mfdbVlan;

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
    LOG_MSG("snoopIntfRemove: Invalid interface type \n");
    return L7_FAILURE;
  }

  /* Delete interface from MFDB */
  memset( (L7_uchar8 *)&mfdb, 0, sizeof(mfdb) );
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (pSnoopCB->family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP 
                          : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (snoopEntry->staticEntry) ? L7_MFDB_TYPE_STATIC : L7_MFDB_TYPE_DYNAMIC;  // PTin modified
  memcpy((void *)mfdb.user.description, (void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);
  /* Set the mask bit in the fwd mask for this interface */
  L7_INTF_SETMASKBIT(mfdb.user.fwdPorts, intIfNum);

  // PTin added
  mfdbVlan = (vlanId<4096) ? snoop_mcast_vlan[vlanId] : vlanId;
  //Determine translated vlan
  if (ptin_intif_vlan_translate_get(intIfNum,mfdbVlan,&realVlan)!=L7_SUCCESS || realVlan==0 || realVlan>4095)
  {
    realVlan = mfdbVlan;
  }
  if (igmp_intercept_debug)
  {
    printf("%s(%d) Processing vlan %u (translated into vlan %u) in port %u => realvlan=%u\n", __FUNCTION__, __LINE__,vlanId,mfdbVlan,intIfNum-1,realVlan);
  }
  mfdb.vlanId = realVlan;

  rc = mfdbEntryPortsDelete(&mfdb);
  if (rc == L7_SUCCESS)
  {
    // PTin modified
    if (intfType == SNOOP_GROUP_MEMBERSHIP)
    {
      if (!snoopEntry->staticEntry)
        snoopTimerStop(snoopEntry, intIfNum, vlanId, intfType, pSnoopCB);
      L7_INTF_CLRMASKBIT(snoopEntry->snoopGrpMemberList, intIfNum);
      snoopNotifyL3Mcast(macAddr, vlanId, &snoopEntry->snoopGrpMemberList);
    }
    else
    {
      L7_INTF_CLRMASKBIT(snoopEntry->snoopMrtrMemberList, intIfNum);
    }
  }
  
  if (rc == L7_SUCCESS)
  {
    /* Search for any non-multicast router interfaces in this entry */
    // PTin modified
    if (snoopEntry->ll_timerList.sllStart == L7_NULL)
    {
      /* PTin added: IGMP snooping */
      #if 1
      if (!snoopEntry->staticEntry)
      #endif
      {
        /* Only router interfaces left in this entry... delete it */
        snoopEntryRemove(macAddr, vlanId, pSnoopCB->family);
      }
    }
  } /* Delete operation success */

  return rc;
}


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
  L7_RC_t           rc = L7_SUCCESS, rc1;
  L7_uint16         mfdbVlan, realVlan, port;
  L7_int            xlate_done;
  L7_uint8          index;
  L7_uint32         tmp, ipAddr;

  snoopEntry = snoopEntryFind(macAddr, vlanId, family, L7_MATCH_EXACT);

  if (snoopEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Entry is found... delete it from the MFDB */
  memset( (L7_uchar8 *)&mfdb, 0, sizeof(mfdb) );
  memcpy(mfdb.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdb.vlanId           = vlanId;
  mfdb.user.componentId = (family == L7_AF_INET) ? L7_MFDB_PROTOCOL_IGMP 
                                                 : L7_MFDB_PROTOCOL_MLD;
  mfdb.user.type        = (snoopEntry->staticEntry) ? L7_MFDB_TYPE_STATIC : L7_MFDB_TYPE_DYNAMIC;  // PTin modified
  memcpy((void *)mfdb.user.description,(void *)L7_MFDB_NETWORK_ASSISTED,
         L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  // PTin added

  // Add all effective vlans to mfdb table
  mfdbVlan = (vlanId<4096) ? snoop_mcast_vlan[vlanId] : vlanId;
  xlate_done = L7_FALSE;
  for (port=0; port<L7_SYSTEM_N_INTERF; port++)
  {
    if (ptin_vlan_translate_get(port, mfdbVlan, &realVlan)==L7_SUCCESS && realVlan>0 && realVlan<=4095)  {
      xlate_done = L7_TRUE;
    }
    else if (port<L7_SYSTEM_N_INTERF-1 || xlate_done)
    {
      continue;
    }
    else
    {
      realVlan = mfdbVlan;
    }
    if (igmp_intercept_debug)
    {
      printf("%s(%d) Processing vlan %u (translated into vlan %u) in port %d => realvlan=%u\n", __FUNCTION__, __LINE__,vlanId,mfdbVlan,port,realVlan);
    }
    mfdb.vlanId = realVlan;

    rc1 = mfdbEntryDelete(&mfdb);
  }
  // PTin end


  //(void)mfdbEntryDelete(&mfdb);

  // PTin added
  // If there is some IP associated to this snoopEntry...
  if (snoopEntry->ipList.channelIPs_bmp!=0)
  {
    // ...run all IPs, and remove them from IP tree
    tmp = snoopEntry->ipList.channelIPs_bmp;
    for (index=0; index<sizeof(L7_uint32)*8; index++, tmp>>=1)
    {
      // If bitmap bit is null, skip to the next one
      if (!(tmp & 1))  continue;

      // IP in use... determine IP address
      ipAddr = ( ((L7_uint32) 0xE0000000) |
                 ((L7_uint32) (index & 0x1F)<<23) |
                 ((L7_uint32) (macAddr[3] & 0x7F) << 16) |
                 ((L7_uint32) (macAddr[4] & 0xFF) <<  8) |
                 ((L7_uint32) (macAddr[5] & 0xFF)) );
      // Remove IP from IP tree
      snoopIpEntryDelete(vlanId, ipAddr);
      if (igmp_intercept_debug)
      {
        printf("%s(%d): IP 0x%08X removed\n", __FUNCTION__, __LINE__,ipAddr);
      }
    }
  }
  // PTin end

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
  L7_uint32        cbIndex = 0, maxInstances;
  L7_RC_t          rc = L7_SUCCESS;

  if (igmp_intercept_debug)
  {
    printf("%s(%d) vlanId=%u\n", __FUNCTION__, __LINE__,vlanId);
  }

  maxInstances = maxSnoopInstancesGet();

  pSnoopCB = snoopCBFirstGet();

  for (cbIndex = L7_NULL; cbIndex < maxInstances; cbIndex++, pSnoopCB++)
  {
    memset(&snoopOperEntry, 0x00, sizeof(snoopOperData_t));
    snoopOperEntry.vlanId = (L7_ushort16)vlanId;
    snoopOperEntry.cbHandle = (void *)pSnoopCB;

    pData = avlInsertEntry(&pSnoopCB->snoopOperDataAvlTree, &snoopOperEntry);

    if (pData == &snoopOperEntry)
    {
      /*some error in avl tree addition*/
      LOG_MSG("snoopOperEntryAdd: Failed to add vlan id %d family %d",
              vlanId, pSnoopCB->family);
      rc = L7_FAILURE;
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
  L7_uint32               cbIndex = 0, maxInstances;
  snoopOperData_t        *pSnoopOperEntry;
  L7_RC_t                 rc = L7_SUCCESS;
  snoopMrtrTimerData_t   *pmrtrTimerData;
  snoopMrtrTimerDataKey_t key;

  maxInstances = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();

  for (cbIndex = L7_NULL; cbIndex < maxInstances; cbIndex++, pSnoopCB++)
  {
    pData = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT);
    
    if (pData == L7_NULLPTR)
    {
      rc = L7_NOT_EXIST;
      continue;
    }

    pSnoopOperEntry = pData;

    /* Stop all Mrtr timers for this vlan ID */
    while(L7_TRUE)
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

    if (pData == L7_NULL)
    {
      /* Entry does not exist */
      rc = L7_FAILURE;
    }
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
  }
}


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
  while(snoopL3Entry)
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
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}
