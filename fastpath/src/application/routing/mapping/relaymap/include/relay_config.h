/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  relay_config.h
*
* @purpose   Configuration header file
*
*
* @create    11/27/2001
*
* @author
*
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef _RELAY_CONFIG_H
#define _RELAY_CONFIG_H

#include "l7_common.h"
#include "l3_comm_structs.h"
#include "l3_default_cnfgr.h"
#include "avl_api.h"
#include "dhcp_relay_exports.h"

#define L7_DHCP_RELAY_CFG_VER_1      0x1
#define L7_DHCP_RELAY_CFG_VER_2      0x2
                                                                                                                                                             
#define L7_DHCP_RELAY_CFG_VER_CURRENT    L7_DHCP_RELAY_CFG_VER_2


typedef struct ihServerEntry_s
{
  struct ihServerEntry_s *nextServer;
  struct ihServerEntry_s *prevServer;
  L7_uint32 serverAddr;
} ihServerEntry_t;

/* Node in AVL tree of relay entries. Key is the combination of 
 * {intIfNum, udpPort}. */
typedef struct ihRelayEntry_s
{
  /* internal interface number of ingress interface. If IH_INTF_ANY,
   * this is a global entry that applies to all interfaces.  */
  L7_uint32 intIfNum;

  /* force alignment */
  L7_ushort16 pad;

  /* Destination UDP port number */
  L7_ushort16 udpPort;

  /* L7_TRUE if this is a discard entry. Only interface entries 
   * (intIfNum != IH_INTF_ANY) can be discard entries. */
  L7_BOOL discard;

  /* Number of times this entry has been used to relay or discard a packet */
  L7_uint32 hitCount;

  /* Doubly-linked list of server IPv4 addresses, sorted from smallest to 
   * largest IP address. */
  ihServerEntry_t *serverList;

  void *next;    /* RESERVED FOR AVL USE */

} ihRelayEntry_t;

typedef struct ihRelayEntryList_s
{
  L7_uint32           maxEntries;
  avlTreeTables_t     *treeHeap;
  L7_uint32           treeHeapSize;
  void                *dataHeap;       /* ihRelayEntry_t* */
  L7_uint32           dataHeapSize;
  avlTree_t           treeData;
} ihRelayEntryList_t;

typedef struct dhcpRelayIntfCfg_s
{
  dhcpRelayIntfValue_t  circuitIDIntfOptionMode;
  dhcpRelayIntfValue_t  cidOptionIntfCheckMode;
} dhcpRelayIntfCfg_t;

/* No longer compatible with binary persistent storage */
typedef struct dhcpRelayCfg_s
{
    L7_uint32          adminMode;
    L7_uint32          maxHopCount;
    L7_uint32          minWaitTime;
    L7_uint32          circuitIDOptionMode;
    L7_BOOL            cidOptionCheckMode;   
    dhcpRelayIntfCfg_t relayrtrIntfCfgData[L7_RTR_MAX_RTR_INTERFACES+1];
    ihRelayEntryList_t relayEntryList;
} dhcpRelayCfg_t;


#endif
