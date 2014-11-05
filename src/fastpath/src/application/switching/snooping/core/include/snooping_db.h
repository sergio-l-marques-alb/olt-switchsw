/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_db.h
*
* @purpose    Contains prototypes to snooping database handling
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
#ifndef SNOOPING_DB_H
#define SNOOPING_DB_H

#include "snooping.h"
#include "ptin_structs.h"

/******************************************************************************
  SNOOP Entry Processing Routines
*******************************************************************************/
/* Snoop AVL tree modification rotuines */
snoopInfoData_t *snoopEntryFind(L7_uchar8 *macAddr, L7_uint32 vlanId,
                                L7_uchar8 family, L7_uint32 flag);
L7_RC_t snoopFirstGet(L7_uchar8 *macAddr, L7_uint32 *vlanId, L7_uchar8 family);
L7_RC_t snoopNextGet(L7_uchar8  *macAddr,L7_uint32  vlanId,L7_uchar8  *nextAddr,
                     L7_uint32  *nextVlanId, L7_uchar8 family);

/* Snoop Entry AVL tree modifications */
L7_RC_t snoopEntryAdd(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticGroup);
L7_RC_t snoopEntryDelete(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family);

/* Snoop Entry interface modifications */
L7_RC_t snoopIntfAdd(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum,
                     snoop_interface_type_t interfaceType, snoop_cb_t *pSnoopCB);
L7_RC_t snoopIntfRemove(L7_uchar8* macAddr, L7_uint32 vlanId, 
                        L7_uint32 intIfNum, snoop_interface_type_t intfType,
                        snoop_cb_t *pSnoopCB);

/* PTin added: IGMP snooping */
#if 1
/// Check if there is no interfaces associated to a VLAN+MAC group
L7_BOOL snoopIntfNone(snoopInfoData_t *snoopEntry);
/// Check if there is no channels associated to a VLAN+MAC group
L7_BOOL snoopChannelsNone(snoopInfoData_t *snoopEntry);
/// Check if there is no channels in a particular interface, for a VLAN+MAC group
L7_BOOL snoopChannelsIntfNone(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum);
/// Check if there is no clients in a particular channel, for a VLAN+MAC group
L7_BOOL snoopChannelClientsNone(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel);
/// Check if there is no clients in a particular channel and interface, for a VLAN+MAC group
L7_BOOL snoopChannelClientsIntfNone(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel);

/// Remove all channels+clients for a particular interface
L7_BOOL snoopIntfClean(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum);

/// Check if a channel exists in a VLAN+MAC entry
L7_BOOL snoopChannelExist(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel);
/// Create a channel if it does not exist (empty clients)
L7_RC_t snoopChannelCreate(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel);
/// Remove a channel if it exists (and all its clients)
L7_RC_t snoopChannelDelete(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel);
/// Delete all channels (and all clients) for a VLAN+MAC group
L7_RC_t snoopChannelDeleteAll(snoopInfoData_t *snoopEntry);

/// Add a new interface for a particular channel
L7_RC_t snoopChannelIntfAdd(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_BOOL isProtection);
/// Remove an interface for a particular channel (only if there is no clients attached to that interface)
L7_RC_t snoopChannelIntfRemove(snoopInfoData_t *snoopEntry, L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_BOOL isProtection);


/// Verify if a client (index) is consuming an IP channel of a Vlan+MAC group
L7_BOOL snoopChannelClientExists(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel, L7_uint client);
/// Add a new client for a particular channel and interface
L7_RC_t snoopChannelClientAdd(snoopInfoData_t *snoopEntry,
                              L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint client);
/// Remove a client from a particular channel and interface
L7_RC_t snoopChannelClientRemove(snoopInfoData_t *snoopEntry,
                                 L7_uint32 intIfNum, L7_inet_addr_t *IPchannel, L7_uint client);
/// Remove all clients from a particular channel (applied to all interfaces)
L7_RC_t snoopChannelClientsRemoveAll(snoopInfoData_t *snoopEntry, L7_inet_addr_t *IPchannel);
/// Remove all clients for a VLAN+MAC group (all channels and interfaces)
L7_RC_t snoopClientsRemoveAll(snoopInfoData_t *snoopEntry);

/// Add channel procedure
L7_RC_t snoop_channel_add_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                   L7_inet_addr_t *mgmdGroupAddr, L7_BOOL staticChannel,
                                   L7_BOOL *send_leave_to_network);
/// Delete channel procedure
L7_RC_t snoop_channel_remove_procedure(L7_uchar8 *dmac, L7_uint16 vlanId, L7_inet_addr_t *mgmdGroupAddr);
/// Add client procedure
L7_RC_t snoop_client_add_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                   L7_inet_addr_t *mgmdGroupAddr,
                                   L7_uint client, L7_uint32 intIfNum,
                                   L7_BOOL *send_leave_to_network);
/// Delete client procedure
L7_RC_t snoop_client_remove_procedure(L7_uchar8 *dmac, L7_uint16 vlanId,
                                      L7_inet_addr_t *mgmdGroupAddr,
                                      L7_uint client, L7_uint32 intIfNum,
                                      L7_BOOL *send_leave_to_network);

/// Check if a particular IP exists
L7_BOOL snoopChannelExist4VlanId(L7_uint16 vlanId, L7_inet_addr_t *channel, snoopInfoData_t **snoopEntry);

/// Get IGMP Channels list based either on VLAN and Client index
void snoopChannelsListGet(L7_uint16 vlanId,
                          L7_uint16 client_index,
                          ptin_igmpChannelInfo_t *channel_list,
                          L7_uint16 *num_channels);

/// Get IGMPv3 Channels list based on VLAN, intIfNum and Client index
void snoopChannelsGet(L7_uint16 vlanId,
                             L7_uint32 intIfNum,
                             L7_uint16 client_index,
                             ptin_igmpChannelInfo_t *channel_list,
                             L7_uint16 *num_channels); 

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
L7_RC_t snoopGroupIntfAdd(L7_uint16 vlanId, L7_inet_addr_t* mgmdGroupAddr, L7_uint32 intIfNum, L7_BOOL isStatic, L7_BOOL isProtection);

/**
 * Remove IPv4 channel, and update Snoop Entry database. 
 * 
 * @param vlanId                : Vlan id
 * @param mgmdGroupAddr         : channel IP
 * @param intIfNum              : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILRE
 */
L7_RC_t snoopGroupIntfRemove(L7_uint16 vlanId, L7_inet_addr_t *mgmdGroupAddr, L7_uint32 intIfNum, L7_BOOL isProtection);

#endif

/* Snoop Entry interraction with MFDB */
L7_RC_t snoopEntryRemove(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uchar8 family);
L7_RC_t snoopEntryCreate(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticGroup);


#if SNOOP_PTIN_IGMPv3_GLOBAL
/******************************************************************************
  PTin - SNOOP L3 Mcast DB Entry Processing Routines
*******************************************************************************/
#if SNOOP_PTIN_IGMPv3_ROUTER
snoopPTinL3InfoData_t *snoopPTinL3EntryFind(L7_uint32 vlanId, L7_inet_addr_t* mcastGroupAddr,L7_uint32 flag);
L7_RC_t snoopPTinL3EntryAdd(L7_uint32 vlanId,L7_inet_addr_t* mcastGroupAddr);
L7_RC_t snoopPTinL3EntryDelete(L7_uint32 vlanId,L7_inet_addr_t* mcastGroupAddr);
#endif

#if SNOOP_PTIN_IGMPv3_PROXY
/******************************************************************************
  PTin - SNOOP Proxy Mcast DB Entry Processing Routines
*******************************************************************************/
snoopPTinProxySource_t *snoopPTinProxySourceEntryFind(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr,L7_uint32 flag);
snoopPTinProxySource_t *snoopPTinProxySourceEntryAdd(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr, L7_BOOL* newEntry,L7_uint8 robustnessVariable);
L7_RC_t snoopPTinProxySourceEntryDelete(snoopPTinProxyGroup_t* groupPtr, L7_inet_addr_t* sourceAddr);

snoopPTinProxyGroup_t *snoopPTinProxyGroupEntryFind(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType, L7_uint32 flag);
snoopPTinProxyGroup_t *snoopPTinProxyGroupEntryAdd(snoopPTinProxyInterface_t * interfacePtr, L7_inet_addr_t* groupAddr,L7_uint8 recordType, L7_BOOL* newEntry,L7_uint8 robustnessVariable);
L7_RC_t snoopPTinProxyGroupEntryDelete(L7_uint32 vlanId, L7_inet_addr_t* groupAddr,L7_uint8 recordType);

snoopPTinProxyInterface_t *snoopPTinProxyInterfaceEntryFind(L7_uint32 vlanId, L7_uint32 flag);
snoopPTinProxyInterface_t *snoopPTinProxyInterfaceEntryAdd(L7_uint32 vlanId, L7_BOOL* newEntry);
L7_RC_t snoopPTinProxyInterfaceEntryDelete(L7_uint32 vlanId);

#endif

#endif
/******************************************************************************
  SNOOP L3 Mcast DB Entry Processing Routines
*******************************************************************************/
snoopL3InfoData_t *snoopL3EntryFind(L7_inet_addr_t *mcastGroupAddr,
                                    L7_inet_addr_t *mcastSrcAddr, L7_uint32 flag);
L7_RC_t snoopL3EntryAdd(L7_inet_addr_t *mcastGroupAddr,
                        L7_inet_addr_t *mcastSrcAddr,
                        L7_int32        srcVlan,
                        L7_VLAN_MASK_t  *outVlanRtrIntfMask);
L7_RC_t snoopL3EntryDelete(L7_inet_addr_t *mcastGroupAddr,
                           L7_inet_addr_t *mcastSrcAddr);
L7_uint32 snoopL3EntryInOutVlanMaskGet(L7_uchar8 *mcastMacAddr,
                                     L7_VLAN_MASK_t *inVlanRtrIntfMask,
                                     L7_VLAN_MASK_t *outVlanRtrIntfMask);

/******************************************************************************
  SNOOP Operational Entry Processing Routines
*******************************************************************************/
L7_RC_t snoopOperEntryAdd(L7_ushort16 vlanId);
L7_RC_t snoopOperEntryDelete(L7_ushort16 vlanId);
snoopOperData_t *snoopOperEntryFirstGet(snoop_cb_t *pSnoopCB);
snoopOperData_t *snoopOperEntryGet(L7_ushort16 vlanId, snoop_cb_t *pSnoopCB,
                                   L7_uint32 flag);
void snoopOperEntryDeInit(snoopOperData_t  *pSnoopOperEntry);
#endif /* SNOOPING_DB_H */
