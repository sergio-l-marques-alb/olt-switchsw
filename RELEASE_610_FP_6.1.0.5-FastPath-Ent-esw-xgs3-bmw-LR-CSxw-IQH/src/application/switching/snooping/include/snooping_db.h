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

/******************************************************************************
  SNOOP Entry Processing Routines
*******************************************************************************/

// PTin added
void igmp_get_channels_recursive(avlTreeTables_t *cell_ptr,
                                 L7_uint16 vlanId,
                                 L7_uint16 client_vlan,
                                 L7_inet_addr_t *channel_list,
                                 L7_uint16 *num_channels);

L7_RC_t snoopClientVlanAdd(snoopIpInfoData_t *entry, L7_uint16 clientVlan, L7_uint32 intIfNum);
L7_RC_t snoopClientVlanRemove(snoopIpInfoData_t *entry, L7_uint16 clientVlan, L7_uint32 intIfNum);
L7_BOOL snoopNoClientVlans(snoopIpInfoData_t *entry, L7_uint32 intIfNum);

L7_BOOL snoopIpClientExists(snoopInfoData_t *entry, L7_uint32 ipaddr);
L7_RC_t snoopIpAdd(snoopInfoData_t *entry, L7_uint32 ipaddr);
L7_RC_t snoopIpRemove(snoopInfoData_t *entry, L7_uint32 ipaddr);
L7_BOOL snoopNoIPs(snoopInfoData_t *entry);

void *snoopEntryNext(avlTree_t *avlTree, avlTreeTables_t **cell_ptr, L7_uint direction);

snoopIpInfoData_t *snoopIpEntryFind(L7_uint16 vlanId, L7_uint32 channelIP);
L7_RC_t snoopIpEntryAdd(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL staticEntry);
L7_RC_t snoopIpEntryDelete(L7_uint16 vlanId, L7_uint32 channelIP);
L7_RC_t snoopIpEntryRemove(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL *sendLeaves);
L7_RC_t snoopIpEntryCreate(L7_uint16 vlanId, L7_uint32 channelIP, L7_BOOL staticEntry, L7_BOOL *sendJoins);
// PTin end

/* Snoop AVL tree modification rotuines */
snoopInfoData_t *snoopEntryFind(L7_uchar8 *macAddr, L7_uint32 vlanId,
                                L7_uchar8 family, L7_uint32 flag);
L7_RC_t snoopFirstGet(L7_uchar8 *macAddr, L7_uint32 *vlanId, L7_uchar8 family);
L7_RC_t snoopNextGet(L7_uchar8  *macAddr,L7_uint32  vlanId,L7_uchar8  *nextAddr,
                     L7_uint32  *nextVlanId, L7_uchar8 family);

/* Snoop Entry AVL tree modifications */
L7_RC_t snoopEntryAdd(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticEntry);
L7_RC_t snoopEntryDelete(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family);

/* Snoop Entry interface modifications */
L7_RC_t snoopIntfAdd(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum,
                     snoop_interface_type_t interfaceType, snoop_cb_t *pSnoopCB);
L7_RC_t snoopIntfRemove(L7_uchar8* macAddr, L7_uint32 vlanId, 
                        L7_uint32 intIfNum, snoop_interface_type_t intfType,
                        snoop_cb_t *pSnoopCB);

/* Snoop Entry interraction with MFDB */
L7_RC_t snoopEntryRemove(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uchar8 family);
L7_RC_t snoopEntryCreate(L7_uchar8* macAddr, L7_uint32 vlanId, L7_uchar8 family, L7_BOOL staticEntry);

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
