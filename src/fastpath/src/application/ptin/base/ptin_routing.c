/**
      printf("dl_queue_add_tail(queue=0x%x, elem=0x%x)\n", queue, elem);
      printf("dl_queue_add_tail(queue=0x%x, elem=0x%x)\n", queue, elem);
 * ptin_routing.c
 *  
 * Implements the Routing interface module 
 *
 * Created on: 2014/04/216
 * Author: Daniel Figueira
 *  
 * Notes:
 */

#include "ptin_routing.h"
#include "ptin_utils.h"
#include "ptin_intf.h"
#include "ptin_ipdtl0_packet.h"
#include "l7_ipmap_arp_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_ping_api.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include <string.h>
#include <linux/if.h>


#include "ping_exports.h" //Remove...


/*********************************************************** 
 * Defines
 ***********************************************************/
#define PTIN_DTL0_INTERFACE_NAME            "dtl0"
#define PTIN_DTL0_MTU_DEFAULT               2500
#define PTIN_ROUTING_INTERFACE_NAME_PREFIX  "rt1_2_"


/*********************************************************** 
 * Typedefs
 ***********************************************************/
typedef struct ptin_routing_arptable_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_routing_arptable_s *next;
  struct ptin_routing_arptable_s *prev;

  L7_uint16         intfNum;
  L7_uint8          type;
  L7_uint32         age;
  L7_inet_addr_t    ipAddr;
  L7_enetMacAddr_t  macAddr;
} ptin_routing_arptable_t;

typedef struct ptin_routing_routetable_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_routing_arptable_s *next;
  struct ptin_routing_arptable_s *prev;

  L7_uint16         intfNum;
  L7_uint8          protocol;
  L7_timespec       updateTime;
  L7_inet_addr_t    ipAddr;
  L7_uint32         subnetMask;
  L7_uint32         preference;
  L7_uint32         metric;
} ptin_routing_routetable_t;

typedef struct ptin_routing_pingsession_s
{
  L7_uint8  index;

  /* Session configuration */
  L7_uint32 ipAddr; 
  L7_uint16 probeCount; 
  L7_uint16 probeSize; 
  L7_uint16 probeInterval;

  /* Session status */
  L7_uint16 handle;
  L7_BOOL   isRunning;
  L7_uint16 probeSent;
  L7_uint16 probeSucc;
  L7_uint16 probeFail;
  L7_uint32 minRtt; 
  L7_uint32 maxRtt; 
  L7_uint32 avgRtt; 
} ptin_routing_pingsession_t;


/*********************************************************** 
 * Data
 ***********************************************************/
L7_uint32                   __ioctl_socket_fd = 0;
L7_BOOL                     __is_dtl0_enabled = L7_FALSE;
dl_queue_t                  __arptable_pool;              //Pool that holds free elements to use in the __arptable_snapshot queue
dl_queue_t                  __arptable_snapshot;          //Each element is of type ptin_routing_arptable_t
dl_queue_t                  __routetable_pool;            //Pool that holds free elements to use in the __routetable_snapshot queue
dl_queue_t                  __routetable_snapshot;        //Each element is of type ptin_routing_arptable_t
ptin_routing_pingsession_t* __ping_sessions;
L7_uint32                   __ping_sessions_max;


/*********************************************************** 
 * Static prototypes
 ***********************************************************/
/**
 * Enable dtl0 interface.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_enable(void);

/**
 * Disable dtl0 interface.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_disable(void);

/**
 * Set dtl0 interface MTU.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_mtu_set(L7_uint32);

/**
 * Enable a routing interface. 
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note The routing interface MUST be renamed to rt1_2_X prior to this method! 
 */
static L7_int __ioctl_vlanintf_enable(L7_uint16 intfId);

/**
 * Disable a routing interface. 
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note The routing interface MUST be renamed to rt1_2_INTFID prior to this method! 
 */
static L7_int __ioctl_vlanintf_disable(L7_uint16 intfId);

/**
 * Add a vlan on an existing interface. 
 *  
 * @param vlanId : Vlan ID to which the routing interface will be associated
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This creates a routing interface named dtl0.VLANID, where VLANID is the value passed through 'vlanId' 
 */
static L7_int __ioctl_vlanintf_add(L7_uint16 vlanId);

/**
 * Remove a vlan from the dtl0 interface.
 *  
 * @param vlanId : Vlan ID of the routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This removes the routing interface named dtl0.VLANID, where VLANID is the value passed through 'vlanId' 
 */
static L7_int __ioctl_vlanintf_remove(L7_uint16 vlanId);

/**
 * Rename an existing interface.
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * @param vlanId : Vlan ID of the interface to rename
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This method is only able to rename routing interfaces named dtl0.VLANID, where VLANID is the value passed through 'vlanId'
 */
static L7_int __ioctl_intf_rename_dtl2rt(L7_uint16 intfId, L7_uint16 vlanId);

/**
 * Rename an existing interface.
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * @param vlanId : Vlan ID of the interface to rename
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This method is only able to rename routing interfaces named rt1_2_INTFID, where INTFID is the value passed through 'intfId'
 */
static L7_int __ioctl_intf_rename_rt2dtl(L7_uint16 intfId, L7_uint16 vlanId);

/**
 * Initialize the current ARP table snapshot. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __arptable_snapshot_init(void);

/**
 * Refresh the current ARP table snapshot. 
 *  
 * @param intfNum : Desired intfNum 
 *  
 * @note If 'intfNum' is different from 0xFFFF (-1), the local snapshot will be refreshed only with entries for this specific interface 
 */
static void __arptable_snapshot_refresh(L7_uint32 intfNum);

/**
 * Initialize the current route table snapshot. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __routetable_snapshot_init(void);

/**
 * Refresh the current route table snapshot. 
 *  
 * @param intfNum : Desired intfNum 
 *  
 * @note If 'intfNum' is different from 0xFFFF (-1), the local snapshot will be refreshed only with entries for this specific interface 
 */
static void __routetable_snapshot_refresh(L7_uint32 intfNum);

/**
 * Initialize the ping sessions array. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __ping_sessions_init(void);

/**
 * Callback that is called when an existing ping sessions ends. 
 *  
 * @param userParam : User data provided to the callback
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __ping_session_finish_callback(void *userParam);


/*********************************************************** 
 * Functions
 ***********************************************************/

/**
 * Initialization
 *  
 * @return none
 */
L7_RC_t ptin_routing_init(void)
{
  /* Local variables initialization */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Starting initialization");
  if(__arptable_snapshot_init() != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to initialize arp table snapshot");
    return L7_FAILURE;
  }
  if(__routetable_snapshot_init() != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to initialize route table snapshot");
    return L7_FAILURE;
  }
  if(__ping_sessions_init() != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to initialize ping session array");
    return L7_FAILURE;
  }

  /* Create a new socket for ioctl interaction */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Creating new socket to interact with ioctl");
  __ioctl_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (__ioctl_socket_fd < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new socket to interact with ioctl");
    return L7_FAILURE;
  }

  /* Set dtl0 MTU */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Setting %s mtu to %u", PTIN_DTL0_INTERFACE_NAME, PTIN_DTL0_MTU_DEFAULT);
  if(__ioctl_dtl0_mtu_set(PTIN_DTL0_MTU_DEFAULT) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to set %s mtu", errno, PTIN_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  /* Enable routing on Fastpath */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Setting Fastpath's routing admin mode to L7_ENABLE");
  if(usmDbIpRtrAdminModeSet(1, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set Fastpath's routing admin mode to L7_ENABLE");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Deinitialization
 *  
 * @return none
 */
L7_RC_t ptin_routing_deinit(void)
{
  /* Disable routing on Fastpath */
  if(usmDbIpRtrAdminModeSet(1, L7_DISABLE) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set Fastpath's routing admin mode to L7_DISABLE");
    return L7_FAILURE;
  }

  /* Set the dtl0 interface to down */
  if(__is_dtl0_enabled == L7_TRUE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Disabling %s interface", PTIN_DTL0_INTERFACE_NAME);
    if(__ioctl_dtl0_disable() != 0)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to disable %s interface", errno, PTIN_DTL0_INTERFACE_NAME);
      return L7_FAILURE;
    }
    __is_dtl0_enabled = L7_FALSE;
  }

  /* Close ioctl socket */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Closing ioctl socket");
  if (0 != close(__ioctl_socket_fd))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to close ioctl socket");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Create a new routing interface.
 * 
 * @param intf           : Routing interface
 * @param routingVlanId  : Vlan ID to which the routing interface will be associated
 * @param internalVlanId : Fastpath's internal Vlan ID for the EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_create(ptin_intf_t* intf, L7_uint16 routingVlanId, L7_uint16 internalVlanId)
{
  L7_uint32 intfNum;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(intf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf->intf_type, intf->intf_id);
    return L7_FAILURE;
  }

  /* Ensure that the dtl0 interface is up */
  if(__is_dtl0_enabled == L7_FALSE)
  {
    LOG_INFO(LOG_CTX_PTIN_ROUTING, "Enabling %s interface", PTIN_DTL0_INTERFACE_NAME);
    if(__ioctl_dtl0_enable() != 0)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to enable %s interface", errno, PTIN_DTL0_INTERFACE_NAME);
      return L7_FAILURE;
    }
    __is_dtl0_enabled = L7_TRUE;
  }

  /* Create a new routing interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Adding vlan %u to interface %s", routingVlanId, PTIN_DTL0_INTERFACE_NAME);
  if(__ioctl_vlanintf_add(routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to add a new vlan %u to the %s interface", errno, routingVlanId, PTIN_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  /* Rename the new routing interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Renaming the new routing interface from %s.%u to %s%u", PTIN_DTL0_INTERFACE_NAME, routingVlanId, PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
  if(__ioctl_intf_rename_dtl2rt(intf->intf_id, routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to rename the new routing interface from %s.%u to %s%u", errno, PTIN_DTL0_INTERFACE_NAME, routingVlanId, PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
    return L7_FAILURE;
  }

  /* Enable the new interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Enabling %s%u interface", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
  if(__ioctl_vlanintf_enable(intf->intf_id) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to enable %s%u interface", errno, PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
    return L7_FAILURE;
  }
  
  /* Associate the new interface with the given vlanId in Fastpath's routing tables */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Associating %s%u with vlan %u on fastpath's routing tables", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, internalVlanId);
  if(usmDbIpVlanRoutingIntfCreate(1, internalVlanId, intf->intf_id+1) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to associate %s%u with vlan %u on fastpath's routing tables", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, internalVlanId);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX //Required because of 'ptin_ipdtl0_control'
  /* Allow IP/ARP packets through dtl0 for this vlan */
  if(L7_SUCCESS != ptin_ipdtl0_control(routingVlanId, routingVlanId, internalVlanId, intfNum, L7_TRUE))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to allow IP/ARP packets through dtl0 for this vlan");
    return L7_FAILURE;
  }
#endif /* PTIN_BOARD_IS_MATRIX */

  /* Set interface MAC address */
  L7_enetMacAddr_t macAddr;
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting %s%u interface MAC address", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
  if(ptin_routing_intf_macaddress_set(intf, &macAddr) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set %s%u interface MAC address", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove an existing routing interface.
 * 
 * @param intfId        : ID of the routing interface (this is not the intfNum)
 * @param routingVlanId : Vlan ID of the routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_remove(ptin_intf_t* intf, L7_uint16 routingVlanId)
{
  L7_uint32 intfNum;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(intf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf->intf_type, intf->intf_id);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX
  /* Allow IP/ARP packets through dtl0 for this vlan */
  if(L7_SUCCESS != ptin_ipdtl0_control(routingVlanId, routingVlanId, (L7_uint16)-1, intfNum, L7_FALSE))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to allow IP/ARP packets through dtl0 for this vlan");
    return L7_FAILURE;
  }
#endif /* PTIN_BOARD_IS_MATRIX */

  /* Disable the interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Disabling %s%u interface", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id);
  if(__ioctl_vlanintf_disable(intf->intf_id) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to disable %s.%u interface", errno, PTIN_DTL0_INTERFACE_NAME, routingVlanId);
    return L7_FAILURE;
  }

  /* Change the interface name to match the vconfig standards */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Renaming the routing interface from %s%u to %s.%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, PTIN_DTL0_INTERFACE_NAME, routingVlanId);
  if(__ioctl_intf_rename_rt2dtl(intf->intf_id, routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to rename the routing interface from %s%u to %s.%u", errno, PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, PTIN_DTL0_INTERFACE_NAME, routingVlanId);
    return L7_FAILURE;
  }

  /* Delete the vlan interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Removing vlan %u from interface %s", routingVlanId, PTIN_DTL0_INTERFACE_NAME);
  if(__ioctl_vlanintf_remove(routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to remove a vlan %u from the %s interface", errno, routingVlanId, PTIN_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set routing interface's ip address.
 * 
 * @param intf       : Routing interface
 * @param ipFamily   : IP address family [L7_AF_INET4; L7_AF_INET6]
 * @param ipAddr     : IP address
 * @param subnetMask : Subnet mask
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_ipaddress_set(ptin_intf_t* intf, L7_uchar8 ipFamily, L7_uint32 ipAddr, L7_uint32 subnetMask)
{
  char           ipAddrStr[IPV6_DISP_ADDR_LEN];
  L7_inet_addr_t inetIpAddr;
  L7_uint32      intfNum;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(intf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf->intf_type, intf->intf_id);
    return L7_FAILURE;
  }

  /* Configure the routing interface with the given IP address */
  inetAddressSet(ipFamily, &ipAddr, &inetIpAddr);
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting routing interface %s%u IP address to %s", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, inetAddrPrint(&inetIpAddr, ipAddrStr));
  if(usmDbIpRtrIntfIPAddressSet(1, intfNum, ipAddr, subnetMask, L7_INTF_IP_ADDR_METHOD_CONFIG) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set routing interface %s%u IP address to %s", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intf->intf_id, inetAddrPrint(&inetIpAddr, ipAddrStr));
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set routing interface's MAC address.
 * 
 * @param intfId  : Routing interface
 * @param macAddr : MAC address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_macaddress_set(ptin_intf_t* intf, L7_enetMacAddr_t* macAddr)
{
  static L7_uint8 offset = 1;
  L7_uchar8       macAddr_tmp[L7_ENET_MAC_ADDR_LEN];
  L7_uint32       intfNum;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(intf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf->intf_type, intf->intf_id);
    return L7_FAILURE;
  }

  if(macAddr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [macAddr:%p]", macAddr);
    return L7_ERROR;
  }

  /* Set MAC address */
  macAddr_tmp[0] = 0x00;
  macAddr_tmp[1] = 0x06;
  macAddr_tmp[2] = 0x91;
  macAddr_tmp[3] = 0x55;
  macAddr_tmp[4] = 0x55;
  macAddr_tmp[5] = offset++;
  memcpy(macAddr->addr, macAddr_tmp, L7_ENET_MAC_ADDR_LEN * sizeof(L7_uchar8));

  /* Configure the routing interface with the given MAC address */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Setting intfnum:%u MAC address to %02X:%02X:%02X:%02X:%02X:%02X\n", 
            intfNum, macAddr->addr[0], macAddr->addr[1], macAddr->addr[2], macAddr->addr[3], macAddr->addr[4], macAddr->addr[5]);
  nimSetIntfAddress(intfNum, L7_NULL, (void*)macAddr->addr); //Necessário?
  nimSetIntfL3MacAddress(intfNum, L7_NULL, (void*)macAddr->addr);
  
  return L7_SUCCESS;
}

/**
 * Get ARP table.
 * 
 * @param intfNum     : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_arptable_get(L7_uint32 intfNum, L7_uint32 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingArpTableResponse* buffer)
{
  L7_uint32                  currentIndex = 0;
  ptin_routing_arptable_t  *snapshotIterator;
  char                      ipAddrStr[IPV6_DISP_ADDR_LEN];
  ptin_intf_t               intf;

  if( (readEntries == L7_NULLPTR) || (buffer == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [readEntries:%p buffer:%p]", readEntries, buffer);
    return L7_ERROR;
  }

  /* If the first requested index is -1, refresh the local snapshot */
  if(firstIdx == (L7_uint32)-1)
  {
    __arptable_snapshot_refresh(intfNum);
  }

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__arptable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "ARP table snapshot is empty");
    return L7_SUCCESS;
  }

  /* Copy local snapshot contents */
  firstIdx += 1;
  while( (currentIndex < maxEntries) && (snapshotIterator != NULL) )
  {
    if(currentIndex >= firstIdx)
    {
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Copying local entry [idx:%u]"            , currentIndex);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  intfNum: %u"                           , snapshotIterator->intfNum);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  type:    %u"                           , snapshotIterator->type);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  age:     %u"                           , snapshotIterator->age);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ipAddr:  %s"                           , inetAddrPrint(&snapshotIterator->ipAddr, ipAddrStr));
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  macAddr: %02X:%02X:%02X:%02X:%02X:%02X", 
                snapshotIterator->macAddr.addr[0], snapshotIterator->macAddr.addr[1], snapshotIterator->macAddr.addr[2], 
                snapshotIterator->macAddr.addr[3], snapshotIterator->macAddr.addr[4], snapshotIterator->macAddr.addr[5]);

      if(L7_SUCCESS != ptin_intf_intIfNum2ptintf(snapshotIterator->intfNum, &intf))
      {
        LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intfNum %u to ptin_intf_t", snapshotIterator->intfNum);
        return L7_FAILURE;
      }
      buffer->index          = currentIndex;
      buffer->intf.intf_type = intf.intf_type;
      buffer->intf.intf_id   = intf.intf_id;
      buffer->type           = snapshotIterator->type;
      buffer->age            = snapshotIterator->age;
      buffer->ipAddr         = snapshotIterator->ipAddr.addr.ipv4.s_addr;
      memcpy(&buffer->macAddr, &snapshotIterator->macAddr.addr, L7_ENET_MAC_ADDR_LEN*sizeof(L7_uchar8));
      ++buffer;
    }

    /* Get next entry */
    snapshotIterator = (ptin_routing_arptable_t*) dl_queue_get_next(&__arptable_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
  *readEntries = currentIndex;
  
  return L7_SUCCESS;
}

/**
 * Remove ARP entry.
 * 
 * @param intfNum : Desired intfNum. Use -1 to delete all ARP entries for this IP address
 * @param ipAddr  : IP address 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_arpentry_purge(L7_uint32 intfNum, L7_uint32 ipAddr)
{
  if(intfNum == -1)
  {
    intfNum = L7_INVALID_INTF;
  }

  usmDbIpArpEntryPurge(1, ipAddr, intfNum);

  return L7_SUCCESS;
}

/**
 * Get route table.
 * 
 * @param intfNum     : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_routetable_get(L7_uint32 intfNum, L7_uint32 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingRouteTableResponse* buffer)
{
  L7_uint32                  currentIndex = 0;
  ptin_routing_routetable_t *snapshotIterator;
  char                       ipAddrStr[IPV6_DISP_ADDR_LEN];
  ptin_intf_t                intf;

  if( (readEntries == L7_NULLPTR) || (buffer == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [readEntries:%p buffer:%p]", readEntries, buffer);
    return L7_ERROR;
  }

  /* If the first requested index is -1, refresh the local snapshot */
  if(firstIdx == (L7_uint32)-1)
  {
    __routetable_snapshot_refresh(intfNum);
  }

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__routetable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Route table snapshot is empty");
    return L7_SUCCESS;
  }

  /* Copy local snapshot contents */
  firstIdx += 1;
  while( (currentIndex < maxEntries) && (snapshotIterator != NULL) )
  {
    if(currentIndex >= firstIdx)
    {
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Copying local entry [idx:%u]" , currentIndex);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  intfNum:    %u"             , snapshotIterator->intfNum);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  protocol:   %u"             , snapshotIterator->protocol);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  updateTime: %ud %uh %um %us", snapshotIterator->updateTime.days, snapshotIterator->updateTime.hours, snapshotIterator->updateTime.minutes, snapshotIterator->updateTime.seconds);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ipAddr:     %s"             , inetAddrPrint(&snapshotIterator->ipAddr, ipAddrStr));
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  subnetMask: %u"             , snapshotIterator->subnetMask);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  preference: %u"             , snapshotIterator->preference);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  metric:     %u"             , snapshotIterator->metric);

      if(L7_SUCCESS != ptin_intf_intIfNum2ptintf(snapshotIterator->intfNum, &intf))
      {
        LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intfNum %u to ptin_intf_t", snapshotIterator->intfNum);
        return L7_FAILURE;
      }
      buffer->index              = currentIndex;
      buffer->intf.intf_type     = intf.intf_type;
      buffer->intf.intf_id       = intf.intf_id;
      buffer->updateTime.days    = snapshotIterator->updateTime.days;
      buffer->updateTime.hours   = snapshotIterator->updateTime.hours;
      buffer->updateTime.minutes = snapshotIterator->updateTime.minutes;
      buffer->updateTime.seconds = snapshotIterator->updateTime.seconds;
      buffer->ipAddr             = snapshotIterator->ipAddr.addr.ipv4.s_addr;
      buffer->subnetMask         = snapshotIterator->subnetMask;
      buffer->preference         = snapshotIterator->preference;
      buffer->metric             = snapshotIterator->metric;
      ++buffer;
    }

    /* Get next entry */
    snapshotIterator = (ptin_routing_routetable_t*) dl_queue_get_next(&__routetable_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
  *readEntries = currentIndex;

  return L7_SUCCESS;
}

/**
 * Start a ping request.
 * 
 * @param index         : Ping session index
 * @param ipAddr        : IP address to ping
 * @param probeCount    : Probe count
 * @param probeSize     : Probe size
 * @param probeInterval : Probe interval
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_create(L7_uint8 index, L7_uint32 ipAddr, L7_uint16 probeCount, L7_uint16 probeSize, L7_uint16 probeInterval)
{
  if(index > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a ping session not in use */
  if(__ping_sessions[index].isRunning == L7_TRUE)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is already being used in an active ping session [index:%u]", index);
    return L7_FAILURE;
  }

  /* Save session configurations */
  __ping_sessions[index].index         = index;
  __ping_sessions[index].isRunning     = L7_TRUE;
  __ping_sessions[index].ipAddr        = ipAddr;
  __ping_sessions[index].probeCount    = probeCount;
  __ping_sessions[index].probeSize     = probeSize;
  __ping_sessions[index].probeInterval = probeInterval;

  /* Start a new ping session */
  if(L7_SUCCESS != usmDbPingStart("", "", L7_FALSE, 0, ipAddr, probeCount, probeSize, probeInterval, 0, L7_NULLPTR, __ping_session_finish_callback, (void*)&__ping_sessions[index].index, &__ping_sessions[index].handle))
  {
    memset(&__ping_sessions[index], 0x00, sizeof(ptin_routing_pingsession_t));

    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to start a new ping session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Query an existing ping session.
 * 
 * @param buffer : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_query(msg_RoutingPingSessionQuery* buffer)
{
  L7_BOOL   operStatus;
  L7_uint16 index;

  if( (buffer == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [buffer:%p]", buffer);
    return L7_ERROR;
  }

  index = buffer->index;
  if(index > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__ping_sessions[index].handle == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [index:%u]", index);
    return L7_FAILURE;
  }

  /* Get session status */
  usmDbPingQuery(__ping_sessions[index].handle, &operStatus, 
                 &__ping_sessions[index].probeSent, &__ping_sessions[index].probeSucc, &__ping_sessions[index].probeFail, 
                 &__ping_sessions[index].minRtt, &__ping_sessions[index].maxRtt, &__ping_sessions[index].avgRtt);

  buffer->isRunning = __ping_sessions[index].isRunning;
  buffer->probeSent = __ping_sessions[index].probeSent;
  buffer->probeSucc = __ping_sessions[index].probeSucc;
  buffer->probeFail = __ping_sessions[index].probeFail;
  buffer->minRtt    = __ping_sessions[index].minRtt;   
  buffer->maxRtt    = __ping_sessions[index].maxRtt;   
  buffer->avgRtt    = __ping_sessions[index].avgRtt;   

  return L7_SUCCESS;
}

/**
 * Free an existing ping session.
 * 
 * @param index : Ping session index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_free(L7_uint8 index)
{
  if(index > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__ping_sessions[index].handle == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [index:%u]", index);
    return L7_FAILURE;
  }

  usmDbPingSessionFree(__ping_sessions[index].handle);
  memset(&__ping_sessions[index], 0x00, sizeof(ptin_routing_pingsession_t));

  return L7_SUCCESS;
}


/*********************************************************** 
 * Static methods
 ***********************************************************/

/**
 * Enable dtl0 interface.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_enable(void)
{
  struct ifreq request; 
  L7_int       res = 0;

  memset(&request, 0x00, sizeof(request));

  strncpy(&request.ifr_name[0], PTIN_DTL0_INTERFACE_NAME, IFNAMSIZ);
  request.ifr_flags |= IFF_UP;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFFLAGS");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name  = %s",     request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_flags = 0x%04X", request.ifr_flags);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFFLAGS, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Disable dtl0 interface.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_disable(void)
{
  struct ifreq request; 
  L7_int       res = 0;

  memset(&request, 0x00, sizeof(request));

  strncpy(&request.ifr_name[0], PTIN_DTL0_INTERFACE_NAME, IFNAMSIZ);
  request.ifr_flags &= ~IFF_UP;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFFLAGS");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name  = %s",     request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_flags = 0x%04X", request.ifr_flags);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFFLAGS, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Set dtl0 interface MTU.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_mtu_set(L7_uint32 mtu)
{
  struct ifreq request; 
  L7_int       res = 0;

  memset(&request, 0x00, sizeof(request));

  snprintf(request.ifr_name, IFNAMSIZ, "%s", PTIN_DTL0_INTERFACE_NAME);
  request.ifr_addr.sa_family = AF_INET;
  request.ifr_mtu            = mtu;
  
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFMTU");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name = %s", request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_addr = %u", request.ifr_addr.sa_family);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_mtu  = %u", request.ifr_mtu);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFMTU, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Enable a routing interface. 
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note The routing interface MUST be renamed to rt1_2_X prior to this method! 
 */
static L7_int __ioctl_vlanintf_enable(L7_uint16 intfId)
{
  struct ifreq request; 
  L7_int       res = 0;
  char         ifName[IFNAMSIZ];

  memset(&request, 0x00, sizeof(request));

  snprintf(ifName, IFNAMSIZ, "%s%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intfId);
  
  strncpy(&request.ifr_name[0], &ifName[0], IFNAMSIZ);
  request.ifr_flags |= IFF_UP;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFFLAGS");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name  = %s",     request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_flags = 0x%04X", request.ifr_flags);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFFLAGS, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Disable a routing interface. 
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note The routing interface MUST be renamed to rt1_2_INTFID prior to this method! 
 */
static L7_int __ioctl_vlanintf_disable(L7_uint16 intfId)
{
  struct ifreq request; 
  L7_int       res = 0;
  char         ifName[IFNAMSIZ];

  memset(&request, 0x00, sizeof(request));

  snprintf(ifName, IFNAMSIZ, "%s%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intfId);
  
  strncpy(&request.ifr_name[0], &ifName[0], IFNAMSIZ);
  request.ifr_flags &= ~IFF_UP;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFFLAGS");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name  = %s",     request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_flags = 0x%04X", request.ifr_flags);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFFLAGS, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Add a vlan on to the dtl0 interface. 
 *  
 * @param vlanId : Vlan ID to which the routing interface will be associated
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This creates a routing interface named dtl0.VLANID, where VLANID is the value passed through 'vlanId' 
 */
static L7_int __ioctl_vlanintf_add(L7_uint16 vlanId)
{
  struct vlan_ioctl_args request; 
  L7_int                 res = 0;

  memset(&request, 0x00, sizeof(request));
  
  request.cmd = ADD_VLAN_CMD;
  strncpy(&request.device1[0], PTIN_DTL0_INTERFACE_NAME, 24);
  request.u.VID = vlanId;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCGIFVLAN");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  cmd     = %u", request.cmd);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  device1 = %s", request.device1);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  u.VID   = %u", request.u.VID);
  if((res = ioctl(__ioctl_socket_fd, SIOCGIFVLAN, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Remove a vlan from the dtl0 interface.
 *  
 * @param vlanId : Vlan ID of the routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This removes the routing interface named dtl0.VLANID, where VLANID is the value passed through 'vlanId' 
 */
static L7_int __ioctl_vlanintf_remove(L7_uint16 vlanId)
{
  struct vlan_ioctl_args request; 
  L7_int                 res = 0;

  memset(&request, 0x00, sizeof(request));
  
  request.cmd = DEL_VLAN_CMD;
  snprintf(&request.device1[0], 24, "%s.%u", PTIN_DTL0_INTERFACE_NAME, vlanId);
  request.u.VID = vlanId;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCGIFVLAN");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  cmd     = %u", request.cmd);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  device1 = %s", request.device1);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  u.VID   = %u", request.u.VID);
  if((res = ioctl(__ioctl_socket_fd, SIOCGIFVLAN, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Rename an existing interface.
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * @param vlanId : Vlan ID of the interface to rename
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This method is only able to rename routing interfaces named dtl0.VLANID, where VLANID is the value passed through 'vlanId'
 */
static L7_int __ioctl_intf_rename_dtl2rt(L7_uint16 intfId, L7_uint16 vlanId)
{
  struct ifreq request; 
  L7_int       res = 0;
  char         oldIfName[IFNAMSIZ];
  char         newIfName[IFNAMSIZ];

  memset(&request, 0x00, sizeof(request));

  snprintf(oldIfName, IFNAMSIZ, "%s.%u", PTIN_DTL0_INTERFACE_NAME, vlanId);
  snprintf(newIfName, IFNAMSIZ, "%s%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intfId);
  
  strncpy(&request.ifr_name[0],    &oldIfName[0], IFNAMSIZ);
  strncpy(&request.ifr_newname[0], &newIfName[0], IFNAMSIZ);

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFNAME");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name    = %s", request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_newname = %s", request.ifr_newname);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFNAME, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Rename an existing interface.
 *  
 * @param intfId : ID of the routing interface (this is not the intfNum)
 * @param vlanId : Vlan ID of the interface to rename
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This method is only able to rename routing interfaces named rt1_2_INTFID, where INTFID is the value passed through 'intfId'
 */
static L7_int __ioctl_intf_rename_rt2dtl(L7_uint16 intfId, L7_uint16 vlanId)
{
  struct ifreq request; 
  L7_int       res = 0;
  char         oldIfName[IFNAMSIZ];
  char         newIfName[IFNAMSIZ];

  memset(&request, 0x00, sizeof(request));

  snprintf(oldIfName, IFNAMSIZ, "%s%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, intfId);
  snprintf(newIfName, IFNAMSIZ, "%s.%u", PTIN_DTL0_INTERFACE_NAME, vlanId);
  
  strncpy(&request.ifr_name[0],    &oldIfName[0], IFNAMSIZ);
  strncpy(&request.ifr_newname[0], &newIfName[0], IFNAMSIZ);

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "ioctl request -> SIOCSIFNAME");
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_name    = %s", request.ifr_name);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  ifr_newname = %s", request.ifr_newname);
  if((res = ioctl(__ioctl_socket_fd, SIOCSIFNAME, &request)) < 0)
  {
    return res;
  }

  return res;
}

/**
 * Initialize the current ARP table snapshot. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __arptable_snapshot_init(void)
{
  L7_uint32           i;
  L7_arpCacheStats_t  arpTableStats;

  /* Determine ARP table max size */
  if(L7_SUCCESS != ipMapArpCacheStatsGet(&arpTableStats))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to get ARP table stats");
    return L7_FAILURE;
  }

  /* Create pool of free elements to use in the '__arptable_snapshot' */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Creating pool of %u elements for the __arptable_snapshot", arpTableStats.cacheMax);
  if(NOERR != dl_queue_init(&__arptable_pool))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for arp table snapshot");
    return L7_FAILURE;
  }
  for (i=0; i<arpTableStats.cacheMax; ++i)
  {
    ptin_routing_arptable_t *new_element = (ptin_routing_arptable_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, sizeof(ptin_routing_arptable_t));
    dl_queue_add(&__arptable_pool, (dl_queue_elem_t*)new_element);
  }

  /* Create the local snapshot queue */
  if(NOERR != dl_queue_init(&__arptable_snapshot))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for arp table snapshot");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Refresh the current ARP table snapshot. 
 *  
 * @param intfNum : Desired intfNum 
 *  
 * @note If 'intfNum' is different from 0xFFFF (-1), the local snapshot will be refreshed only with entries for this specific interface 
 */
static void __arptable_snapshot_refresh(L7_uint32 intfNum)
{
  L7_arpEntry_t            arpTablepEntry;
  ptin_routing_arptable_t *localSnapshotEntry;

  memset(&arpTablepEntry, 0x00, sizeof(L7_arpEntry_t));

  /* Clear the current __arptable_snapshot */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Clearing current snapshot and return all elements to the __arptable_pool");
  while(dl_queue_remove_tail(&__arptable_snapshot, (dl_queue_elem_t**)&localSnapshotEntry) == NOERR)
  {
    dl_queue_add_tail(&__arptable_pool, (dl_queue_elem_t*)localSnapshotEntry);
  }

  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Refreshing local snapshot");
  while(L7_ERROR != usmDbIpArpEntryNext(1, arpTablepEntry.ipAddr, arpTablepEntry.intIfNum, &arpTablepEntry))
  {
    /* If filtering is active, ensure we only accept entries with the desired intfNum */
    if( (intfNum != (L7_uint32)-1) && (arpTablepEntry.intIfNum != intfNum) )
    {
      continue;
    }

    /* Get free element from the __arptable_pool */
    dl_queue_remove_tail(&__arptable_pool, (dl_queue_elem_t**)&localSnapshotEntry);

    localSnapshotEntry->intfNum = arpTablepEntry.intIfNum;
    if(arpTablepEntry.flags & L7_ARP_LOCAL)
    {
      localSnapshotEntry->type = PTIN_ROUTING_ARPTABLE_TYPE_LOCAL;
    }
    else if(arpTablepEntry.flags & L7_ARP_GATEWAY)
    {
      localSnapshotEntry->type = PTIN_ROUTING_ARPTABLE_TYPE_GATEWAY;
    }
    else if(arpTablepEntry.flags & L7_ARP_STATIC)
    {
      localSnapshotEntry->type = PTIN_ROUTING_ARPTABLE_TYPE_STATIC;
    }
    else
    {
      localSnapshotEntry->type = PTIN_ROUTING_ARPTABLE_TYPE_DYNAMIC;
    }
    localSnapshotEntry->age = arpTablepEntry.ageSecs;
    inetAddressSet(L7_AF_INET, &arpTablepEntry.ipAddr, &localSnapshotEntry->ipAddr);
    memcpy(&localSnapshotEntry->macAddr, &arpTablepEntry.macAddr.addr.enetAddr, sizeof(L7_enetMacAddr_t));

    /* Add element to the local snapshot */
    dl_queue_add_tail(&__arptable_snapshot, (dl_queue_elem_t*)localSnapshotEntry);
  }
}

/**
 * Initialize the current route table snapshot. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __routetable_snapshot_init(void)
{
  L7_uint32 i;
  L7_uint32 routeTableMaxEntries;

  /* Determine route table max size */
  routeTableMaxEntries = platRtrRouteMaxEntriesGet();

  /* Create pool of free elements to use in the '__routetable_snapshot' */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Creating pool of %u elements for the __routetable_snapshot", routeTableMaxEntries);
  if(NOERR != dl_queue_init(&__routetable_pool))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for arp table snapshot");
    return L7_FAILURE;
  }
  for (i=0; i<routeTableMaxEntries; ++i)
  {
    ptin_routing_routetable_t *new_element = (ptin_routing_routetable_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, sizeof(ptin_routing_routetable_t));
    dl_queue_add(&__routetable_pool, (dl_queue_elem_t*)new_element);
  }

  /* Create the local snapshot queue */
  if(NOERR != dl_queue_init(&__routetable_snapshot))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for route table snapshot");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Refresh the current route table snapshot. 
 *  
 * @param intfNum : Desired intfNum 
 *  
 * @note If 'intfNum' is different from 0xFFFF (-1), the local snapshot will be refreshed only with entries for this specific interface 
 */
static void __routetable_snapshot_refresh(L7_uint32 intfNum)
{
  L7_routeEntry_t            routeTablepEntry;
  ptin_routing_routetable_t *localSnapshotEntry;
  L7_uint32                  currentTime;

  memset(&routeTablepEntry, 0x00, sizeof(L7_routeEntry_t));

  /* Clear the current __arptable_snapshot */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Clearing current snapshot and return all elements to the __routetable_pool");
  while(dl_queue_remove_tail(&__routetable_snapshot, (dl_queue_elem_t**)&localSnapshotEntry) == NOERR)
  {
    dl_queue_add_tail(&__routetable_pool, (dl_queue_elem_t*)localSnapshotEntry);
  }

  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Refreshing local snapshot");
  currentTime = simSystemUpTimeGet();
  while(L7_ERROR != usmDbNextRouteEntryGet(1, &routeTablepEntry, L7_FALSE))
  {
    /* If filtering is active, ensure we only accept entries with the desired intfNum */
    if(routeTablepEntry.ecmpRoutes.numOfRoutes == 0)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "There has to be at least one best route in this entry!");
      return;
    }

    /* If filtering is active, ensure we only accept entries with the desired intfNum */
    if( (intfNum != (L7_uint32)-1) && (routeTablepEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum != intfNum) )
    {
      continue;
    }

    /* Get free element from the __routetable_pool */
    dl_queue_remove_tail(&__routetable_pool, (dl_queue_elem_t**)&localSnapshotEntry);

    localSnapshotEntry->intfNum    = routeTablepEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    localSnapshotEntry->protocol   = routeTablepEntry.protocol;
    osapiConvertRawUpTime(currentTime-routeTablepEntry.updateTime, &localSnapshotEntry->updateTime);
    inetAddressSet(L7_AF_INET, &routeTablepEntry.ipAddr, &localSnapshotEntry->ipAddr);
    localSnapshotEntry->subnetMask = inetMaskLengthGet(routeTablepEntry.subnetMask);
    localSnapshotEntry->preference = routeTablepEntry.pref;
    localSnapshotEntry->metric     = routeTablepEntry.metric;

    /* Add element to the local snapshot */
    dl_queue_add_tail(&__routetable_snapshot, (dl_queue_elem_t*)localSnapshotEntry);
  }
}

/**
 * Initialize the ping sessions array. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __ping_sessions_init(void)
{
  L7_uint32 max_ping_sessions;

  if(L7_SUCCESS != usmDbMaxPingSessionsGet(&max_ping_sessions))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to determine max number of ping sessions");
    return L7_FAILURE;
  }

  __ping_sessions_max = max_ping_sessions;
  __ping_sessions     = (ptin_routing_pingsession_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, max_ping_sessions * sizeof(ptin_routing_pingsession_t));

  return L7_SUCCESS;
}

/**
 * Callback that is called when an existing ping sessions ends. 
 *  
 * @param userParam : User data provided to the callback
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __ping_session_finish_callback(void *userParam)
{
  L7_uint8 index;

  if(userParam == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [userParam:%p]", userParam);
    return L7_FAILURE;
  }
  index = *(L7_uint8*)userParam;

  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Ping session %u has finished", index);
  __ping_sessions[index].isRunning = L7_FALSE;

  return L7_SUCCESS;
}


/*********************************************************** 
 * Debug methods
 ***********************************************************/

/**
 * Dump the current status of the ping sessions array.
 * 
 * @param index : Ping session index. Use -1 to dump all sessions
 */
void ptin_routing_pingsession_dump(L7_uint8 index)
{
  L7_uint8 i;

  if(index > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return;
  }

  if(index != (L7_uint8)-1)
  {
    printf("Index[%u]--------------------\n", index);
    printf("Configurations:\n");
    printf("  IP Address:        %08X\n",     __ping_sessions[index].ipAddr);
    printf("  Probe Count:       %u\n",       __ping_sessions[index].probeCount);
    printf("  Probe Size:        %u\n",       __ping_sessions[index].probeSize);
    printf("  Probe Interval:    %u\n",       __ping_sessions[index].probeInterval);
    printf("Status:\n");
    printf("  Handle:            %08X\n",     __ping_sessions[index].handle);
    printf("  Is running:        %u\n",       __ping_sessions[index].isRunning);
    printf("  Probe Sent:        %u\n",       __ping_sessions[index].probeSent);
    printf("  Probe Success:     %u\n",       __ping_sessions[index].probeSucc);
    printf("  Probe Fail:        %u\n",       __ping_sessions[index].probeFail);
    printf("  Rtt (min/avg/max): %u/%u/%u\n", __ping_sessions[index].minRtt, __ping_sessions[index].avgRtt, __ping_sessions[index].maxRtt);
  }
  else
  {
    for(i=0; i<__ping_sessions_max; ++i)
    {
      printf("Index[%u]--------------------\n", i);
      printf("Configurations:\n");
      printf("  IP Address:        %08X\n",     __ping_sessions[i].ipAddr);
      printf("  Probe Count:       %u\n",       __ping_sessions[i].probeCount);
      printf("  Probe Size:        %u\n",       __ping_sessions[i].probeSize);
      printf("  Probe Interval:    %u\n",       __ping_sessions[i].probeInterval);
      printf("Status:\n");
      printf("  Handle:            %08X\n",     __ping_sessions[i].handle);
      printf("  Is running:        %u\n",       __ping_sessions[i].isRunning);
      printf("  Probe Sent:        %u\n",       __ping_sessions[i].probeSent);
      printf("  Probe Success:     %u\n",       __ping_sessions[i].probeSucc);
      printf("  Probe Fail:        %u\n",       __ping_sessions[i].probeFail);
      printf("  Rtt (min/avg/max): %u/%u/%u\n", __ping_sessions[i].minRtt, __ping_sessions[i].avgRtt, __ping_sessions[i].maxRtt);
    }
  }
}

