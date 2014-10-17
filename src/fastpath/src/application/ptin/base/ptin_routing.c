/**
 * ptin_routing.h
 *  
 * Implements the Routing interface module 
 *
 * Created on: 2014/04/16
 * Author: Daniel Figueira
 *  
 * Notes:
 */

#include "ptin_routing.h"
#include "ptin_utils.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_ipdtl0_packet.h"
#include "l7_ipmap_arp_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_ping_api.h"
#include "usmdb_traceroute_api.h"
#include "usmdb_nim_api.h"
#include "ping_exports.h"
#include "traceroute_exports.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_vlan.h>
#include <linux/sockios.h>
#include <string.h>
#include <linux/if.h>


/*********************************************************** 
 * Defines
 ***********************************************************/
#define PTIN_ROUTING_USMDB_UNITINDEX        1
#define PTIN_ROUTING_DTL0_INTERFACE_NAME    "dtl0"
#define PTIN_ROUTING_DTL0_MTU_DEFAULT       L7_MAX_FRAME_SIZE
#define PTIN_ROUTING_INTERFACE_NAME_PREFIX  "rt1_2_"  /* This is derived from rt$UNIT_$SLOT_ */
#define PTIN_ROUTING_TRACEROUTE_MAX_HOPS    (TRACEROUTE_DEFAULT_MAX_TTL - TRACEROUTE_DEFAULT_INIT_TTL)  /* This is a copy from traceroute.h, which I'm not able to include here */


/*********************************************************** 
 * Typedefs
 ***********************************************************/
typedef struct ptin_routing_intf_s
{
  L7_uint8  type;
  L7_uint16 routingVlanId;
  L7_uint16 internalVlanId;
  L7_uint16 physicalIntfNum;
} ptin_routing_intf_t;

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
  L7_inet_addr_t    networkIpAddr;
  L7_uint32         subnetMask;
  L7_inet_addr_t    gwIpAddr;
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

typedef struct ptin_routing_traceroutesession_t
{
  L7_uint8  index;

  /* Session configuration */
  L7_uint32 ipAddr; 
  L7_uint16 probePerHop; 
  L7_uint16 probeSize; 
  L7_uint32 probeInterval;
  L7_BOOL   dontFrag;
  L7_uint16 port;
  L7_uint16 maxTtl;
  L7_uint16 initTtl;
  L7_uint16 maxFail;

  /* Session status */
  L7_uint16 handle;
  L7_BOOL   isRunning;
  L7_uint16 currTtl;
  L7_uint16 currHopCount;
  L7_uint16 currProbeCount;
  L7_uint16 testAttempt; 
  L7_uint16 testSuccess; 
} ptin_routing_traceroutesession_t;

typedef struct ptin_routing_traceroutehop_s
{
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct ptin_routing_arptable_s *next;
  struct ptin_routing_arptable_s *prev;

  L7_uint16 sessionHandle;
  L7_uint16 ttl;
  L7_uint32 ipAddr;
  L7_uint32 minRtt;
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_uint16 probeSent;
  L7_uint16 probeRecv;
} ptin_routing_traceroutehop_t;


/*********************************************************** 
 * Data
 ***********************************************************/
L7_uint32                         __ioctl_socket_fd = 0;
L7_BOOL                           __is_dtl0_enabled = L7_FALSE;
L7_uint32                         __routing_interfaces_max;
ptin_routing_intf_t*              __routing_interfaces;
dl_queue_t                        __arptable_pool;              //Pool that holds free elements to use in the __arptable_snapshot queue
dl_queue_t                        __arptable_snapshot;          //Each element is of type ptin_routing_arptable_t
L7_uint32                         __arptable_entries_max;          
dl_queue_t                        __routetable_pool;            //Pool that holds free elements to use in the __routetable_snapshot queue
dl_queue_t                        __routetable_snapshot;        //Each element is of type ptin_routing_arptable_t
L7_uint32                         __routetable_entries_max;          
ptin_routing_pingsession_t*       __ping_sessions;
L7_uint32                         __ping_sessions_max;
ptin_routing_traceroutesession_t* __traceroute_sessions;
L7_uint32                         __traceroute_sessions_max;
L7_uint32                         __traceroute_hops_max;
dl_queue_t                        __traceroutehops_pool;        //Pool that holds free elements to use in the __traceroutehops_snapshot queue
dl_queue_t                        __traceroutehops_snapshot;    //Each element is of type ptin_routing_traceroutehop_t


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
 * Set routing interface's MAC address.
 * 
 * @param intfId  : Routing interface
 * @param macAddr : MAC address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __intf_macaddress_set(ptin_intf_t* intf, L7_enetMacAddr_t* macAddr);

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

/**
 * Initialize the traceroute sessions array. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __traceroute_sessions_init(void);

/**
 * Refresh the current traceroute hops snapshot. 
 *  
 * @param sessionIdx : Traceroute session index
 */
static void __traceroutehops_snapshot_refresh(L7_uint32 sessionIdx);


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
  L7_uint32 minRoutingIntfId, maxRoutingIntfId;

  nimIntIfNumRangeGet(L7_LOGICAL_VLAN_INTF, &minRoutingIntfId, &maxRoutingIntfId);

  /* Local variables initialization */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Starting initialization");
  __routing_interfaces_max = maxRoutingIntfId - minRoutingIntfId + 1;
  __routing_interfaces     = (ptin_routing_intf_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, __routing_interfaces_max*sizeof(ptin_routing_intf_t));
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
  if(__traceroute_sessions_init() != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to initialize traceroute session array");
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
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Setting %s mtu to %u", PTIN_ROUTING_DTL0_INTERFACE_NAME, PTIN_ROUTING_DTL0_MTU_DEFAULT);
  if(__ioctl_dtl0_mtu_set(PTIN_ROUTING_DTL0_MTU_DEFAULT) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to set %s mtu", errno, PTIN_ROUTING_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  /* Enable routing on Fastpath */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Setting Fastpath's routing admin mode to L7_ENABLE");
  if(usmDbIpRtrAdminModeSet(PTIN_ROUTING_USMDB_UNITINDEX, L7_ENABLE) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set Fastpath's routing admin mode to L7_ENABLE");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Create a new routing interface.
 * 
 * @param routingIntf    : Routing interface
 * @param intfType       : Routing interface type
 * @param physicalIntf   : Physical interface
 * @param routingVlanId  : Vlan ID to which the routing interface will be associated
 * @param internalVlanId : Fastpath's internal Vlan ID for the EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_create(ptin_intf_t* routingIntf, L7_uint16 internalVlanId)
{
  L7_uint32            routingIntfNum;
  ptin_HwEthMef10Evc_t evc;
  L7_uint32            i;
  L7_uint8             intfType = PTIN_ROUTING_INTF_TYPE_UNKNOWN;
  L7_uint16            routingVlanId;
  ptin_intf_t          physicalIntf; //Only used for UPLINK routing interfaces 

  if( (routingIntf == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [routingIntf:%p]", routingIntf);
    return L7_ERROR;
  }

  if(routingIntf->intf_id >= __routing_interfaces_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested routing interface ID exceeds the allowed range [id:%u max:%u]", routingIntf->intf_id, __routing_interfaces_max);
    return L7_FAILURE;
  }

  /* Ensure the requested interface does not exist (required for flushes) */
  if(__routing_interfaces[routingIntf->intf_id].type != PTIN_ROUTING_INTF_TYPE_UNKNOWN)
  {
    LOG_NOTICE(LOG_CTX_PTIN_ROUTING, "Received request to create an interface that already exists. Ignored. [intf_id:%u]", routingIntf->intf_id);
    return L7_SUCCESS;
  }

  /* Determine routing interface type, based on the intf mef_type of this evcId */
  if(L7_SUCCESS != ptin_evc_get_fromIntVlan(internalVlanId, &evc))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert internalVlanId %u to an EVC", internalVlanId);
    return L7_FAILURE;
  }
  for(i=0; i<evc.n_intf; ++i)
  {
     /* If we find at least one ROOT interface, then this is an uplink routing interface. Otherwise, it is considered a loopback routing interface */
     if(evc.intf[i].mef_type == 0)
     {
        intfType               = PTIN_ROUTING_INTF_TYPE_UPLINK;
        routingVlanId          = evc.intf[i].vid;
        physicalIntf.intf_type = evc.intf[i].intf_type;
        physicalIntf.intf_id   = evc.intf[i].intf_id;
        break;
     }
  }
  if(intfType == PTIN_ROUTING_INTF_TYPE_UNKNOWN) //If we enter here it's because there are no ROOT interfaces in this EVC
  {
     intfType = PTIN_ROUTING_INTF_TYPE_LOOPBACK;
     routingVlanId = evc.intf[0].vid; //There are only leaf interfaces, and they all MUST have the same external VID
  }

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(routingIntf, &routingIntfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert routingIntf %u/%u to intfNum", routingIntf->intf_type, routingIntf->intf_id);
    return L7_FAILURE;
  }

  /* Ensure that the dtl0 interface is up */
  if(__is_dtl0_enabled == L7_FALSE)
  {
    LOG_INFO(LOG_CTX_PTIN_ROUTING, "Enabling %s interface", PTIN_ROUTING_DTL0_INTERFACE_NAME);
    if(__ioctl_dtl0_enable() != 0)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to enable %s interface", errno, PTIN_ROUTING_DTL0_INTERFACE_NAME);
      return L7_FAILURE;
    }
    __is_dtl0_enabled = L7_TRUE;
  }

  /* Create a new routing interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Adding vlan %u to interface %s", routingVlanId, PTIN_ROUTING_DTL0_INTERFACE_NAME);
  if(__ioctl_vlanintf_add(routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to add a new vlan %u to the %s interface", errno, routingVlanId, PTIN_ROUTING_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  /* Rename the new routing interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Renaming the new routing interface from %s.%u to %s%u", PTIN_ROUTING_DTL0_INTERFACE_NAME, routingVlanId, PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
  if(__ioctl_intf_rename_dtl2rt(routingIntf->intf_id, routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to rename the new routing interface from %s.%u to %s%u", errno, PTIN_ROUTING_DTL0_INTERFACE_NAME, routingVlanId, PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
    return L7_FAILURE;
  }

  /* Enable the new interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Enabling %s%u interface", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
  if(__ioctl_vlanintf_enable(routingIntf->intf_id) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to enable %s%u interface", errno, PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
    return L7_FAILURE;
  }
  
  /* Associate the new interface with the given vlanId in Fastpath's routing tables */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Associating %s%u with vlan %u on fastpath's routing tables", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id, internalVlanId);
  if(usmDbIpVlanRoutingIntfCreate(PTIN_ROUTING_USMDB_UNITINDEX, internalVlanId, routingIntf->intf_id+1) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to associate %s%u with vlan %u on fastpath's routing tables", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id, internalVlanId);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX //Required because of 'ptin_ipdtl0_control'
  /* Allow IP/ARP packets through dtl0 for this vlan */
  if(L7_SUCCESS != ptin_ipdtl0_control(routingVlanId, routingVlanId, internalVlanId, routingIntfNum, L7_TRUE))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to allow IP/ARP packets through dtl0 for this vlan");
    return L7_FAILURE;
  }
#endif /* PTIN_BOARD_IS_MATRIX */

  /* 
     For uplink routing interfaces, set the MAC address to match the MAC address of the physical interface.
     For loopback routing interfaces, set the MAC address to match the MAC address of the dtl0 interface.
  */
  __routing_interfaces[routingIntf->intf_id].type           = intfType;
  __routing_interfaces[routingIntf->intf_id].routingVlanId  = routingVlanId;
  __routing_interfaces[routingIntf->intf_id].internalVlanId = internalVlanId;
  if(intfType == PTIN_ROUTING_INTF_TYPE_UPLINK)
  {
    L7_enetMacAddr_t macAddr;
    L7_uint32        physicalIntfNum;

    if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&physicalIntf, &physicalIntfNum))
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert physicalIntf %u/%u to intfNum", physicalIntf.intf_type, physicalIntf.intf_id);
      return L7_FAILURE;
    }

    if (nimGetIntfAddress(physicalIntfNum, L7_NULL, &macAddr.addr[0]) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to get physical interface MAC address [physicalIntf:%u/%u]", physicalIntf.intf_type, physicalIntf.intf_id);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting %s%u interface MAC address", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
    if(__intf_macaddress_set(routingIntf, &macAddr) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set %s%u interface MAC address", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
      return L7_FAILURE;
    }

    /* Save the physical interface to which we are connected so we can access it later */
    __routing_interfaces[routingIntf->intf_id].physicalIntfNum = physicalIntfNum;
  }

  return L7_SUCCESS;
}

/**
 * Remove an existing routing interface.
 * 
 * @param routingIntf : Routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_remove(ptin_intf_t* routingIntf)
{
  L7_uint32 intfNum;
  L7_uint16 routingVlanId;
  L7_uint16 internalVlanId;

  if( (routingIntf == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [routingIntf:%p]", routingIntf);
    return L7_ERROR;
  }

  if(routingIntf->intf_id >= __routing_interfaces_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested routing interface ID exceeds the allowed range [id:%u max:%u]", routingIntf->intf_id, __routing_interfaces_max);
    return L7_FAILURE;
  }
  routingVlanId  = __routing_interfaces[routingIntf->intf_id].routingVlanId;
  internalVlanId = __routing_interfaces[routingIntf->intf_id].internalVlanId;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(routingIntf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert routingIntf %u/%u to intfNum", routingIntf->intf_type, routingIntf->intf_id);
    return L7_FAILURE;
  }

  /* Delete vlan routing interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Deleting routing interface associated with internal vlan %u", internalVlanId);
  if(usmDbIpVlanRoutingIntfDelete(PTIN_ROUTING_USMDB_UNITINDEX, internalVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to delete routing interface associated with internal vlan %u", internalVlanId);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX
  /* Disable IP/ARP packets through dtl0 for this vlan */
  if(L7_SUCCESS != ptin_ipdtl0_control(routingVlanId, routingVlanId, (L7_uint16)-1, intfNum, L7_FALSE))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to allow IP/ARP packets through dtl0 for this vlan [routingVlanId:%u intfNum:%u]", routingVlanId, intfNum);
    return L7_FAILURE;
  }
#endif /* PTIN_BOARD_IS_MATRIX */

  /* Disable the interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Disabling %s%u interface", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id);
  if(__ioctl_vlanintf_disable(routingIntf->intf_id) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to disable %s.%u interface", errno, PTIN_ROUTING_DTL0_INTERFACE_NAME, routingVlanId);
    return L7_FAILURE;
  }

  /* Change the interface name to match the vconfig standards */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Renaming the routing interface from %s%u to %s.%u", PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id, PTIN_ROUTING_DTL0_INTERFACE_NAME, routingVlanId);
  if(__ioctl_intf_rename_rt2dtl(routingIntf->intf_id, routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to rename the routing interface from %s%u to %s.%u", errno, PTIN_ROUTING_INTERFACE_NAME_PREFIX, routingIntf->intf_id, PTIN_ROUTING_DTL0_INTERFACE_NAME, routingVlanId);
    return L7_FAILURE;
  }

  /* Delete the vlan interface */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Removing vlan %u from interface %s", routingVlanId, PTIN_ROUTING_DTL0_INTERFACE_NAME);
  if(__ioctl_vlanintf_remove(routingVlanId) != 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "ioctl error (errno:%d). Unable to remove a vlan %u from the %s interface", errno, routingVlanId, PTIN_ROUTING_DTL0_INTERFACE_NAME);
    return L7_FAILURE;
  }

  /* Reset saved interface info */
  memset(&__routing_interfaces[routingIntf->intf_id], 0x00, sizeof(ptin_routing_intf_t));

  return L7_SUCCESS;
}

/**
 * Remove all routing interfaces.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
void ptin_routing_intf_remove_all(void)
{
   L7_uint32   i;
   ptin_intf_t intf;

   intf.intf_type = PTIN_EVC_INTF_ROUTING;
   LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Removing all routing interfaces");
   for(i=0; i<__routing_interfaces_max; ++i)
   {
      if(__routing_interfaces[i].type != PTIN_ROUTING_INTF_TYPE_UNKNOWN)
      {
         intf.intf_id = i;
         if(L7_SUCCESS != ptin_routing_intf_remove(&intf))
         {
            LOG_WARNING(LOG_CTX_PTIN_ROUTING, "Unable to remove routing interface %u. Continuing", i);
         }
      }
   }
}

/**
 * Set routing interface's ip address.
 * 
 * @param routingIntf : Routing interface
 * @param ipFamily    : IP address family [L7_AF_INET4; L7_AF_INET6]
 * @param ipAddr      : IP address
 * @param subnetMask  : Subnet mask
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_ipaddress_set(ptin_intf_t* routingIntf, L7_uchar8 ipFamily, L7_uint32 ipAddr, L7_uint32 subnetMask)
{
  char           ipAddrStr[IPV6_DISP_ADDR_LEN];
  char           ipSubnetStr[IPV6_DISP_ADDR_LEN];
  L7_inet_addr_t inetIpAddr;
  L7_inet_addr_t inetIpSubnet;
  L7_uint32      intfNum;
  L7_RC_t        rc;

  if( (routingIntf == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [routingIntf:%p]", routingIntf);
    return L7_ERROR;
  }

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(routingIntf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert routingIntf %u/%u to intfNum", routingIntf->intf_type, routingIntf->intf_id);
    return L7_FAILURE;
  }

  /* Configure the routing interface with the given IP address */
  inetAddressSet(ipFamily, &ipAddr, &inetIpAddr);
  inetAddressSet(ipFamily, &subnetMask, &inetIpSubnet);
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting routing interface %u IP address to %s/%s", intfNum, inetAddrPrint(&inetIpAddr, ipAddrStr), inetAddrPrint(&inetIpSubnet, ipSubnetStr));
  if((rc = usmDbIpRtrIntfIPAddressSet(PTIN_ROUTING_USMDB_UNITINDEX, intfNum, ipAddr, subnetMask, L7_INTF_IP_ADDR_METHOD_CONFIG)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set routing interface %s%u IP address to %s/%s (rc = %u)", intfNum, inetAddrPrint(&inetIpAddr, ipAddrStr), inetAddrPrint(&inetIpSubnet, ipSubnetStr), rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set routing interface's MTU.
 * 
 * @param routingIntf : Routing interface
 * @param mtu         : MTU
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_mtu_set(ptin_intf_t* routingIntf, L7_uint32 mtu)
{
  L7_uint32 intfNum;
  L7_RC_t   rc;

  if( (routingIntf == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [routingIntf:%p]", routingIntf);
    return L7_ERROR;
  }

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(routingIntf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert routingIntf %u/%u to intfNum", routingIntf->intf_type, routingIntf->intf_id);
    return L7_FAILURE;
  }

  /* Configure the routing interface with the given MTU */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting routing interface %u MTU to %d", intfNum, mtu);
  if((rc = usmDbIntfIpMtuSet(PTIN_ROUTING_USMDB_UNITINDEX, intfNum, mtu)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to set routing interface %u MTU to %d (rc = %u)", intfNum, mtu, rc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get the physical interface currently associated with the requested routing interface.
 * 
 * @param routingIntfNum  : Routing interface
 * @param physicalIntfNum : Ptr to the physical intfNum
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note PTIN_ROUTING_INTF_TYPE_LOOPBACK routing interfaces do not have a specific physical interface associated with them.
 *       In those cases, 'physicalIntfNum' is set to (L7_uint16)-1.
 *  
 * @note If this prototype is modified, do not forget to update 'l3_intf.c' file as I was required to place an extern 
 *       prototype declaration of this method there.
 */
L7_RC_t ptin_routing_intf_physicalport_get(L7_uint16 routingIntfNum, L7_uint16 *physicalIntfNum)
{
  ptin_intf_t routingIntf;

  if( (physicalIntfNum == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [physicalIntfNum:%p]", physicalIntfNum);
    return L7_ERROR;
  }

  if(L7_SUCCESS != ptin_intf_intIfNum2ptintf(routingIntfNum, &routingIntf))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to convert requested intIfNum [routingIntfNum:%u]", routingIntfNum);
    return L7_FAILURE;
  }

  if(routingIntf.intf_id >= __routing_interfaces_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested routing interface ID exceeds the allowed range [id:%u max:%u]", routingIntf.intf_id, __routing_interfaces_max);
    return L7_FAILURE;
  }

  if(__routing_interfaces[routingIntf.intf_id].type == PTIN_ROUTING_INTF_TYPE_UPLINK)
  {
    *physicalIntfNum = __routing_interfaces[routingIntf.intf_id].physicalIntfNum;
  }
  else
  {
    *physicalIntfNum = (L7_uint16)-1;
  }

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
L7_RC_t ptin_routing_arptable_getnext(L7_uint32 intfNum, L7_uint32 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingArpTableResponse* buffer)
{
  L7_uint32                 currentIndex = 0;
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
    *readEntries = 0;
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

  usmDbIpArpEntryPurge(PTIN_ROUTING_USMDB_UNITINDEX, ipAddr, intfNum);

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
    *readEntries = 0;
    return L7_SUCCESS;
  }

  /* Copy local snapshot contents */
  firstIdx += 1;
  while( (currentIndex < maxEntries) && (snapshotIterator != NULL) )
  {
    if(currentIndex >= firstIdx)
    {
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Copying local entry [idx:%u]" , currentIndex);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  intfNum:       %u"             , snapshotIterator->intfNum);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  protocol:      %u"             , snapshotIterator->protocol);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  updateTime:    %ud %uh %um %us", snapshotIterator->updateTime.days, snapshotIterator->updateTime.hours, snapshotIterator->updateTime.minutes, snapshotIterator->updateTime.seconds);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  networkIpAddr: %s"             , inetAddrPrint(&snapshotIterator->networkIpAddr, ipAddrStr));
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  subnetMask:    %u"             , snapshotIterator->subnetMask);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  gwIpAddr:      %s"             , inetAddrPrint(&snapshotIterator->gwIpAddr, ipAddrStr));
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  preference:    %u"             , snapshotIterator->preference);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  metric:        %u"             , snapshotIterator->metric);

      if(L7_SUCCESS != ptin_intf_intIfNum2ptintf(snapshotIterator->intfNum, &intf))
      {
        LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intfNum %u to ptin_intf_t", snapshotIterator->intfNum);
        return L7_FAILURE;
      }
      buffer->index              = currentIndex;
      buffer->intf.intf_type     = intf.intf_type;
      buffer->intf.intf_id       = intf.intf_id;
      buffer->protocol           = snapshotIterator->protocol;
      buffer->updateTime.days    = snapshotIterator->updateTime.days;
      buffer->updateTime.hours   = snapshotIterator->updateTime.hours;
      buffer->updateTime.minutes = snapshotIterator->updateTime.minutes;
      buffer->updateTime.seconds = snapshotIterator->updateTime.seconds;
      buffer->networkIpAddr      = snapshotIterator->networkIpAddr.addr.ipv4.s_addr;
      buffer->subnetMask         = snapshotIterator->subnetMask;
      buffer->gwIpAddr           = snapshotIterator->gwIpAddr.addr.ipv4.s_addr;
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
 * Configure a static route.
 * 
 * @param dstIpAddr   : Destination IP address
 * @param subnetMask  : Subnet mask
 * @param nextHopRtr  : Gateway router
 * @param pref        : Route preference value
 * @param isNullRoute : Set to L7_TRUE to redirect all traffic that matches this route to the null0 interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note If a static route with the same destination IP address, subnet mask and next hop router exists, its preference will be updated to the provided value. 
 */
L7_RC_t ptin_routing_staticroute_add(L7_uint32 dstIpAddr, L7_uint32 subnetMask, L7_uint32 nextHopRtr, L7_uint8 pref, L7_BOOL isNullRoute)
{
  L7_RT_ENTRY_FLAGS_t routeFlags = 0;

  if(isNullRoute == L7_TRUE)
  {
    routeFlags |= L7_RTF_REJECT;
  }

  return usmDbIpStaticRouteAdd(PTIN_ROUTING_USMDB_UNITINDEX, dstIpAddr, subnetMask, nextHopRtr, pref, L7_INVALID_INTF, routeFlags);
}

/**
 * Delete an existing static route.
 * 
 * @param dstIpAddr   : Destination IP address
 * @param subnetMask  : Subnet mask
 * @param nextHopRtr  : Gateway router
 * @param isNullRoute : Set to L7_TRUE if this static route is a null route
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note Next hop router is not a mandatory parameter (use the 0 when none is specified). 
 */
L7_RC_t ptin_routing_staticroute_delete(L7_uint32 dstIpAddr, L7_uint32 subnetMask, L7_uint32 nextHopRtr, L7_BOOL isNullRoute)
{
  L7_RT_ENTRY_FLAGS_t routeFlags = 0;

  if(isNullRoute == L7_TRUE)
  {
    routeFlags |= L7_RTF_REJECT;
  }

  return usmDbIpStaticRouteDelete(PTIN_ROUTING_USMDB_UNITINDEX, dstIpAddr, subnetMask, nextHopRtr, L7_INVALID_INTF, routeFlags);
}

/**
 * Start a ping request.
 * 
 * @param sessionIdx    : Ping session index
 * @param ipAddr        : IP address to ping
 * @param probeCount    : Probe count
 * @param probeSize     : Probe size
 * @param probeInterval : Probe interval
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_create(L7_uint8 sessionIdx, L7_uint32 ipAddr, L7_uint16 probeCount, L7_uint16 probeSize, L7_uint16 probeInterval)
{
  L7_uchar8 bufferStr[PING_MAX_INDEX_LEN];

  if(sessionIdx > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", sessionIdx);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a ping session not in use */
  if(__ping_sessions[sessionIdx].isRunning == L7_TRUE)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is already being used in an active ping session [index:%u]", sessionIdx);
    return L7_FAILURE;
  }

  /* Save session configurations */
  __ping_sessions[sessionIdx].index         = sessionIdx;
  __ping_sessions[sessionIdx].isRunning     = L7_TRUE;
  __ping_sessions[sessionIdx].ipAddr        = ipAddr;
  __ping_sessions[sessionIdx].probeCount    = probeCount;
  __ping_sessions[sessionIdx].probeSize     = probeSize;
  __ping_sessions[sessionIdx].probeInterval = probeInterval;
  memset(bufferStr, 0x00, PING_MAX_INDEX_LEN * sizeof(L7_uchar8));
  snprintf(bufferStr, PING_MAX_INDEX_LEN, "ping#%u", sessionIdx); //We need a session name that differs in all created sessions

  /* Start a new ping session */
  if(L7_SUCCESS != usmDbPingStart(bufferStr, bufferStr, L7_FALSE, 0, ipAddr, probeCount, probeSize, probeInterval, 
                                  0, L7_NULLPTR, __ping_session_finish_callback, (void*)&__ping_sessions[sessionIdx].index, &__ping_sessions[sessionIdx].handle))
  {
    memset(&__ping_sessions[sessionIdx], 0x00, sizeof(ptin_routing_pingsession_t));

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

  index = buffer->sessionIdx;
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
 * @param sessionIdx : Ping session index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_free(L7_uint8 sessionIdx)
{
  if(sessionIdx > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", sessionIdx);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__ping_sessions[sessionIdx].handle == 0)
  {
    LOG_INFO(LOG_CTX_PTIN_ROUTING, "Requested index does not belong to a created session [index:%u]", sessionIdx);
    return L7_SUCCESS;
  }

  usmDbPingSessionFree(__ping_sessions[sessionIdx].handle);
  memset(&__ping_sessions[sessionIdx], 0x00, sizeof(ptin_routing_pingsession_t));

  return L7_SUCCESS;
}

/**
 * Free all existing ping session.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_freeall()
{
  L7_uint8 i;

  /* Free all existing sessions */
  for(i=0; i<__ping_sessions_max; ++i)
  {
    ptin_routing_pingsession_free(i);
  }

  return L7_SUCCESS;
}

/**
 * Start a traceroute request.
 * 
 * @param sessionIdx    : Traceroute session index
 * @param ipAddr        : IP address to ping
 * @param probeSize     : Probe size
 * @param probePerHop   : Probes per hop
 * @param probeInterval : Probe interval
 * @param dontFrag      : Don't frag
 * @param port          : Destination port
 * @param maxTtl        : Max ttl
 * @param initTtl       : Initial ttl
 * @param maxFail       : Max fails
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_create(L7_uint8 sessionIdx, L7_uint32 ipAddr, L7_uint16 probeSize, L7_uint16 probePerHop, L7_uint32 probeInterval,
                                              L7_BOOL dontFrag, L7_uint16 port, L7_ushort16 maxTtl, L7_ushort16 initTtl, L7_ushort16 maxFail)
{
  L7_uchar8 bufferStr[TRACEROUTE_MAX_INDEX_LEN];

  if(sessionIdx > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of traceroute sessions", sessionIdx);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a traceroute session not in use */
  if(__traceroute_sessions[sessionIdx].isRunning == L7_TRUE)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is already being used in an active traceroute session [index:%u]", index);
    return L7_FAILURE;
  }

  /* Save session configurations */
  __traceroute_sessions[sessionIdx].index         = sessionIdx;
  __traceroute_sessions[sessionIdx].isRunning     = L7_TRUE;
  __traceroute_sessions[sessionIdx].ipAddr        = ipAddr;
  __traceroute_sessions[sessionIdx].probeSize     = probeSize;
  __traceroute_sessions[sessionIdx].probePerHop   = probePerHop;
  __traceroute_sessions[sessionIdx].probeInterval = probeInterval;
  __traceroute_sessions[sessionIdx].dontFrag      = dontFrag;
  __traceroute_sessions[sessionIdx].port          = port;
  __traceroute_sessions[sessionIdx].maxTtl        = maxTtl;
  __traceroute_sessions[sessionIdx].initTtl       = initTtl;
  __traceroute_sessions[sessionIdx].maxFail       = maxFail;
  memset(bufferStr, 0x00, TRACEROUTE_MAX_INDEX_LEN * sizeof(L7_uchar8));
  snprintf(bufferStr, TRACEROUTE_MAX_INDEX_LEN, "traceroute#%u", sessionIdx); //We need a session name that differs in all created sessions

  /* Start a new traceroute session */
  if(L7_SUCCESS != usmDbTraceRoute(bufferStr, bufferStr, L7_FALSE, 0, ipAddr, probeSize, probePerHop, probeInterval, dontFrag, port, maxTtl, initTtl, maxFail,
                                   L7_NULLPTR, (void*)&__traceroute_sessions[sessionIdx].index, &__traceroute_sessions[sessionIdx].handle))
  {
    memset(&__traceroute_sessions[sessionIdx], 0x00, sizeof(ptin_routing_traceroutesession_t));

    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to start a new traceroute session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Query an existing traceroute session.
 * 
 * @param buffer : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_query(msg_RoutingTracertSessionQuery* buffer)
{
  ptin_routing_traceroutesession_t *session;

  if( (buffer == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [buffer:%p]", buffer);
    return L7_ERROR;
  }

  if(buffer->sessionIdx > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", buffer->sessionIdx);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__traceroute_sessions[buffer->sessionIdx].handle == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [index:%u]", buffer->sessionIdx);
    return L7_FAILURE;
  }
  session = &__traceroute_sessions[buffer->sessionIdx];

  /* Get session status */
  usmDbTraceRouteQuery(session->handle, &session->isRunning, 
                       &session->currTtl, &session->currHopCount, &session->currProbeCount, 
                       &session->testAttempt, &session->testSuccess);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Query [index:%u]",  buffer->sessionIdx);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Handle:       %u", session->handle);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Status:       %u", session->isRunning);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  CurrTtl:      %u", session->currTtl);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Hop Count:    %u", session->currHopCount);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Probe Count:  %u", session->currProbeCount);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Test Attempt: %u", session->testAttempt);
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Test Success: %u", session->testSuccess);
  buffer->isRunning      = session->isRunning;
  buffer->currTtl        = session->currTtl;
  buffer->currHopCount   = session->currHopCount;
  buffer->currProbeCount = session->currProbeCount;
  buffer->testAttempt    = session->testAttempt;
  buffer->testSuccess    = session->testSuccess;

  return L7_SUCCESS;
}

/**
 * Get known hops for an existing traceroute session.
 * 
 * @param sessionIdx  : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_gethops(L7_uint32 sessionIdx, L7_uint16 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingTracertSessionHopsResponse* buffer)
{
  L7_uint32                     currentIndex = 0;
  ptin_routing_traceroutehop_t *snapshotIterator;

  if( (readEntries == L7_NULLPTR) || (buffer == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Abnormal context [readEntries:%p buffer:%p]", readEntries, buffer);
    return L7_ERROR;
  }

  if(sessionIdx > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__traceroute_sessions[sessionIdx].handle == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [sessionIdx:%u]", sessionIdx);
    return L7_FAILURE;
  }

  /* If the first requested index is -1, refresh the local snapshot */
  if(firstIdx == (L7_uint16)-1)
  {
    __traceroutehops_snapshot_refresh(sessionIdx);
  }

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__traceroutehops_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "There are not hops for this traceroute session");
    *readEntries = 0;
    return L7_SUCCESS;
  }

  /* Ensure that the local snapshot contents belong to the requested session */
  if((snapshotIterator != NULL) && (snapshotIterator->sessionHandle != __traceroute_sessions[sessionIdx].handle))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Local snapshot handle does not match with the requested session [local:%u requested:%u]!", snapshotIterator->sessionHandle, __traceroute_sessions[sessionIdx].handle);
    *readEntries = 0;
    return L7_FAILURE;
  }

  /* Copy local snapshot contents */
  firstIdx += 1;
  while( (currentIndex < maxEntries) && (snapshotIterator != NULL) )
  {
    if(currentIndex >= firstIdx)
    {
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Copying Hop[idx:%u]", currentIndex);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  TTL:        %u",    snapshotIterator->ttl);      
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  IP Address: %08X",  snapshotIterator->ipAddr);   
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Min rtt:    %u",    snapshotIterator->minRtt);   
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Max rtt:    %u",    snapshotIterator->maxRtt);   
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Avg rtt:    %u",    snapshotIterator->avgRtt);   
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Probe Sent: %u",    snapshotIterator->probeSent);
      LOG_TRACE(LOG_CTX_PTIN_ROUTING, "  Probe Rcvd: %u",    snapshotIterator->probeRecv);

      buffer->hopIdx     = currentIndex;
      buffer->ttl        = snapshotIterator->ttl;        
      buffer->ipAddr     = snapshotIterator->ipAddr;     
      buffer->minRtt     = snapshotIterator->minRtt;     
      buffer->maxRtt     = snapshotIterator->maxRtt;     
      buffer->avgRtt     = snapshotIterator->avgRtt;     
      buffer->probeSent  = snapshotIterator->probeSent;  
      buffer->probeRecv  = snapshotIterator->probeRecv;  

      ++buffer;
    }

    /* Get next entry */
    snapshotIterator = (ptin_routing_traceroutehop_t*) dl_queue_get_next(&__traceroutehops_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
  *readEntries = currentIndex;

  return L7_SUCCESS;
}

/**
 * Free an existing traceroute session.
 * 
 * @param sessionIdx : Traceroute session index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_free(L7_uint8 sessionIdx)
{
  if(sessionIdx > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of traceroute sessions", sessionIdx);
    return L7_FAILURE;
  }

  /* Ensure that the requested index belongs to a created session */
  if(__traceroute_sessions[sessionIdx].handle == 0)
  {
    LOG_INFO(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [index:%u]", sessionIdx);
    return L7_SUCCESS;
  }

  usmDbTraceRouteFree(__traceroute_sessions[sessionIdx].handle);
  memset(&__traceroute_sessions[sessionIdx], 0x00, sizeof(ptin_routing_traceroutesession_t));

  return L7_SUCCESS;
}

/**
 * Free all existing traceroute sessions.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_freeall()
{
  L7_uint8 i;

  for(i=0; i<__traceroute_sessions_max; ++i)
  {
    ptin_routing_traceroutesession_free(i);
  }

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

  /* Enable DTL0 */
  memset(&request, 0x00, sizeof(request));
  strncpy(&request.ifr_name[0], PTIN_ROUTING_DTL0_INTERFACE_NAME, IFNAMSIZ);
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
 * Set dtl0 interface MTU.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_int __ioctl_dtl0_mtu_set(L7_uint32 mtu)
{
  struct ifreq request; 
  L7_int       res = 0;

  memset(&request, 0x00, sizeof(request));

  snprintf(request.ifr_name, IFNAMSIZ, "%s", PTIN_ROUTING_DTL0_INTERFACE_NAME);
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
  strncpy(&request.device1[0], PTIN_ROUTING_DTL0_INTERFACE_NAME, 24);
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
  snprintf(&request.device1[0], 24, "%s.%u", PTIN_ROUTING_DTL0_INTERFACE_NAME, vlanId);
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

  snprintf(oldIfName, IFNAMSIZ, "%s.%u", PTIN_ROUTING_DTL0_INTERFACE_NAME, vlanId);
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
 * Set routing interface's MAC address.
 * 
 * @param intfId  : Routing interface
 * @param macAddr : MAC address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t __intf_macaddress_set(ptin_intf_t* intf, L7_enetMacAddr_t* macAddr)
{
  L7_uint32 intfNum;

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

  /* Configure the routing interface with the given MAC address */
  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Setting intfnum:%u MAC address to %02X:%02X:%02X:%02X:%02X:%02X\n", 
            intfNum, macAddr->addr[0], macAddr->addr[1], macAddr->addr[2], macAddr->addr[3], macAddr->addr[4], macAddr->addr[5]);
  nimSetIntfAddress(intfNum, L7_NULL, (void*)macAddr->addr);
  nimSetIntfL3MacAddress(intfNum, L7_NULL, (void*)macAddr->addr);
  
  return L7_SUCCESS;
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
  snprintf(newIfName, IFNAMSIZ, "%s.%u", PTIN_ROUTING_DTL0_INTERFACE_NAME, vlanId);
  
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
  __arptable_entries_max = arpTableStats.cacheMax;

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
  L7_uint32                insertedEntries = 0;

  memset(&arpTablepEntry, 0x00, sizeof(L7_arpEntry_t));

  /* Clear the current snapshot */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Clearing current snapshot and return all elements to the pool");
  while(dl_queue_remove_tail(&__arptable_snapshot, (dl_queue_elem_t**)&localSnapshotEntry) == NOERR)
  {
    dl_queue_add_tail(&__arptable_pool, (dl_queue_elem_t*)localSnapshotEntry);
  }

  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Refreshing local snapshot");
  while(L7_ERROR != usmDbIpArpEntryNext(PTIN_ROUTING_USMDB_UNITINDEX, arpTablepEntry.ipAddr, arpTablepEntry.intIfNum, &arpTablepEntry))
  {
    /* If filtering is active, ensure we only accept entries with the desired intfNum */
    if( (intfNum != (L7_uint32)-1) && (arpTablepEntry.intIfNum != intfNum) )
    {
      continue;
    }

    /* Do we have enough space in our local snapshot to save this entry? */
    ++insertedEntries;
    if(insertedEntries > __arptable_entries_max)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Not enough free space in the local snapshot to save all os usmDb contents [insertedEntries:%u __arptable_entries_max:%u]", insertedEntries, __arptable_entries_max);
      return;
    }

    /* Get free element from the pool */
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
  __routetable_entries_max = routeTableMaxEntries;

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
  L7_uint32                  insertedEntries = 0;

  memset(&routeTablepEntry, 0x00, sizeof(L7_routeEntry_t));

  /* Clear the current snapshot */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Clearing current snapshot and return all elements to the pool");
  while(dl_queue_remove_tail(&__routetable_snapshot, (dl_queue_elem_t**)&localSnapshotEntry) == NOERR)
  {
    dl_queue_add_tail(&__routetable_pool, (dl_queue_elem_t*)localSnapshotEntry);
  }

  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Refreshing local snapshot");
  currentTime = simSystemUpTimeGet();
  while(L7_ERROR != usmDbNextRouteEntryGet(PTIN_ROUTING_USMDB_UNITINDEX, &routeTablepEntry, L7_FALSE))
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

    /* Do we have enough space in our local snapshot to save this entry? */
    ++insertedEntries;
    if(insertedEntries > __routetable_entries_max)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Not enough free space in the local snapshot to save all os usmDb contents [insertedEntries:%u __routetable_entries_max:%u]", insertedEntries, __routetable_entries_max);
      return;
    }

    /* Get free element from the pool */
    dl_queue_remove_tail(&__routetable_pool, (dl_queue_elem_t**)&localSnapshotEntry);

    localSnapshotEntry->intfNum    = routeTablepEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
    localSnapshotEntry->protocol   = routeTablepEntry.protocol;
    osapiConvertRawUpTime(currentTime-routeTablepEntry.updateTime, &localSnapshotEntry->updateTime);
    inetAddressSet(L7_AF_INET, &routeTablepEntry.ipAddr, &localSnapshotEntry->networkIpAddr);
    localSnapshotEntry->subnetMask = inetMaskLengthGet(routeTablepEntry.subnetMask);
    inetAddressSet(L7_AF_INET, &routeTablepEntry.ecmpRoutes.equalCostPath[0], &localSnapshotEntry->gwIpAddr);
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
  L7_uint32 max_sessions;

  if(L7_SUCCESS != usmDbMaxPingSessionsGet(&max_sessions))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to determine max number of ping sessions");
    return L7_FAILURE;
  }

  __ping_sessions_max = max_sessions;
  __ping_sessions     = (ptin_routing_pingsession_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, max_sessions * sizeof(ptin_routing_pingsession_t));

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

/**
 * Initialize the traceroute sessions array. 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
static L7_RC_t __traceroute_sessions_init(void)
{
  L7_uint32 max_sessions;
  L7_uint32 i;

  if(L7_SUCCESS != usmDbTraceRouteMaxSessionsGet(&max_sessions))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to determine max number of traceroute sessions");
    return L7_FAILURE;
  }

  __traceroute_sessions_max = max_sessions;
  __traceroute_sessions     = (ptin_routing_traceroutesession_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, max_sessions * sizeof(ptin_routing_traceroutesession_t));
  __traceroute_hops_max     = PTIN_ROUTING_TRACEROUTE_MAX_HOPS;

  /* Create pool of free elements to use in the '__traceroutehops_snapshot' */
  LOG_INFO(LOG_CTX_PTIN_ROUTING, "Creating pool of %u elements for the __traceroutehops_snapshot", __traceroute_hops_max);
  if(NOERR != dl_queue_init(&__traceroutehops_pool))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for traceroute hops snapshot");
    return L7_FAILURE;
  }
  for (i=0; i<__traceroute_hops_max; ++i)
  {
    ptin_routing_traceroutehop_t *new_element = (ptin_routing_traceroutehop_t*) osapiMalloc(L7_PTIN_COMPONENT_ID, sizeof(ptin_routing_traceroutehop_t));
    dl_queue_add(&__traceroutehops_pool, (dl_queue_elem_t*)new_element);
  }

  /* Create the local snapshot queue */
  if(NOERR != dl_queue_init(&__traceroutehops_snapshot))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to create new dl_queue for traceroute hops snapshot");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Refresh the current traceroute hops snapshot. 
 *  
 * @param sessionIdx : Traceroute session index
 */
static void __traceroutehops_snapshot_refresh(L7_uint32 sessionIdx)
{
  ptin_routing_traceroutehop_t *localSnapshotEntry;
  L7_uint16                     sessionHandle;
  L7_uint16                     lastHopIdx;
  L7_uint16                     ttl;
  L7_uint32                     ipAddr;
  L7_uint32                     minRtt, avgRtt, maxRtt;
  L7_uint16                     probeSent, probeRecv;
  L7_uint32                     insertedEntries = 0;

  /* Clear the current snapshot */
  LOG_TRACE(LOG_CTX_PTIN_ROUTING, "Clearing current snapshot and return all elements to the pool");
  while(dl_queue_remove_tail(&__traceroutehops_snapshot, (dl_queue_elem_t**)&localSnapshotEntry) == NOERR)
  {
    dl_queue_add_tail(&__traceroutehops_pool, (dl_queue_elem_t*)localSnapshotEntry);
  }

  /* Get the handle for this traceroute session */
  if(sessionIdx > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return;
  }
  if(__traceroute_sessions[sessionIdx].handle == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index is does not belong to a created session [sessionIdx:%u]", sessionIdx);
    return;
  }
  sessionHandle = __traceroute_sessions[sessionIdx].handle;

  LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Refreshing local snapshot");
  insertedEntries = 0;
  lastHopIdx      = (L7_uint16)-1;
  while((L7_SUCCESS == usmDbTraceRouteHopGetNext(sessionHandle, &lastHopIdx, &ttl, &ipAddr,  &minRtt, &maxRtt, &avgRtt, &probeSent, &probeRecv)))
  {
    /* Do we have enough space in our local snapshot to save this entry? */
    ++insertedEntries;
    if(insertedEntries > __traceroute_hops_max)
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Not enough free space in the local snapshot to save all os usmDb contents [insertedEntries:%u __traceroute_hops_max:%u]", insertedEntries, __traceroute_hops_max);
      return;
    }

    /* Get free element from the pool */
    dl_queue_remove_tail(&__traceroutehops_pool, (dl_queue_elem_t**)&localSnapshotEntry);

    localSnapshotEntry->sessionHandle = sessionHandle; 
    localSnapshotEntry->ttl           = ttl; 
    localSnapshotEntry->ipAddr        = ipAddr; 
    localSnapshotEntry->minRtt        = minRtt; 
    localSnapshotEntry->maxRtt        = maxRtt; 
    localSnapshotEntry->avgRtt        = avgRtt; 
    localSnapshotEntry->probeSent     = probeSent; 
    localSnapshotEntry->probeRecv     = probeRecv;

    /* Add element to the local snapshot */
    dl_queue_add_tail(&__traceroutehops_snapshot, (dl_queue_elem_t*)localSnapshotEntry);
  }
}

/*********************************************************** 
 * Debug methods
 ***********************************************************/

/**
 * Dump the current status of the routing interfaces.
 */
void ptin_routing_intf_dump(void)
{
  L7_uint32 i;

  for(i=0; i<__routing_interfaces_max; ++i)
  {
    if(__routing_interfaces[i].type != 0) //Means it is currently used
    {
      printf("Interface [id:%u]\n",     i);
      printf("  type:            %u\n", __routing_interfaces[i].type);
      printf("  routingVlanId:   %u\n", __routing_interfaces[i].routingVlanId);
      printf("  internalVlanId:  %u\n", __routing_interfaces[i].internalVlanId);
      printf("  physicalIntfNum: %u\n", __routing_interfaces[i].physicalIntfNum);
    }
  }
}

/**
 * Dump the local ARP table snapshot.
 */
void ptin_routing_arptablesnapshot_dump(void)
{
  L7_uint32                currentIndex = 0;
  ptin_routing_arptable_t *snapshotIterator;
  char                     ipAddrStr[IPV6_DISP_ADDR_LEN];

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__arptable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "There are not hops for this traceroute session");
    return;
  }

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__arptable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "ARP table snapshot is empty");
    return;
  }

  /* Copy local snapshot contents */
  while(snapshotIterator != NULL)
  {
    printf("Entry [idx:%u]\n"                          , currentIndex);
    printf("  intfNum: %u\n"                           , snapshotIterator->intfNum);
    printf("  type:    %u\n"                           , snapshotIterator->type);
    printf("  age:     %u\n"                           , snapshotIterator->age);
    printf("  ipAddr:  %s\n"                           , inetAddrPrint(&snapshotIterator->ipAddr, ipAddrStr));
    printf("  macAddr: %02X:%02X:%02X:%02X:%02X:%02X\n", snapshotIterator->macAddr.addr[0], snapshotIterator->macAddr.addr[1], snapshotIterator->macAddr.addr[2], 
                                                       snapshotIterator->macAddr.addr[3], snapshotIterator->macAddr.addr[4], snapshotIterator->macAddr.addr[5]);

    /* Get next entry */
    snapshotIterator = (ptin_routing_arptable_t*) dl_queue_get_next(&__routetable_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
}

/**
 * Dump the local Route table snapshot.
 */
void ptin_routing_routetablesnapshot_dump(void)
{
  L7_uint32                  currentIndex = 0;
  ptin_routing_routetable_t *snapshotIterator;
  char                       ipAddrStr[IPV6_DISP_ADDR_LEN];

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__routetable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "There are not hops for this traceroute session");
    return;
  }

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__routetable_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    LOG_DEBUG(LOG_CTX_PTIN_ROUTING, "Route table snapshot is empty");
    return;
  }

  /* Copy local snapshot contents */
  while(snapshotIterator != NULL)
  {
    printf("Copying local entry [idx:%u]\n" , currentIndex);
    printf("  intfNum:       %u\n"             , snapshotIterator->intfNum);
    printf("  protocol:      %u\n"             , snapshotIterator->protocol);
    printf("  updateTime:    %ud %uh %um %us\n", snapshotIterator->updateTime.days, snapshotIterator->updateTime.hours, snapshotIterator->updateTime.minutes, snapshotIterator->updateTime.seconds);
    printf("  networkIpAddr: %s\n"             , inetAddrPrint(&snapshotIterator->networkIpAddr, ipAddrStr));
    printf("  subnetMask:    %u\n"             , snapshotIterator->subnetMask);
    printf("  gwIpAddr:      %s\n"             , inetAddrPrint(&snapshotIterator->gwIpAddr, ipAddrStr));
    printf("  preference:    %u\n"             , snapshotIterator->preference);
    printf("  metric:        %u\n"             , snapshotIterator->metric);

    /* Get next entry */
    snapshotIterator = (ptin_routing_routetable_t*) dl_queue_get_next(&__routetable_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
}

/**
 * Dump the current status of the ping sessions array.
 * 
 * @param index : Ping session index. Use -1 to dump all sessions
 */
void ptin_routing_pingsession_dump(L7_uint32 index)
{
  L7_uint8 i;

  if(index > __ping_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of ping sessions", index);
    return;
  }

  if(index != (L7_uint32)-1)
  {
    printf("Index[%u]--------------------\n", index);
    printf("Configurations:\n");
    printf("  IP Address:        %08X\n",      __ping_sessions[index].ipAddr);
    printf("  Probe Count:       %u\n",        __ping_sessions[index].probeCount);
    printf("  Probe Size:        %u\n",        __ping_sessions[index].probeSize);
    printf("  Probe Interval:    %u\n",        __ping_sessions[index].probeInterval);
    printf("Status:\n");
    printf("  Handle:             %08X\n",     __ping_sessions[index].handle);
    printf("  Is running:        %u\n",        __ping_sessions[index].isRunning);
    printf("  Probe Sent:        %u\n",        __ping_sessions[index].probeSent);
    printf("  Probe Success:     %u\n",        __ping_sessions[index].probeSucc);
    printf("  Probe Fail:        %u\n",        __ping_sessions[index].probeFail);
    printf("  Rtt (min/avg/max): %u/%u/%u\n",  __ping_sessions[index].minRtt, __ping_sessions[index].avgRtt, __ping_sessions[index].maxRtt);
  }
  else
  {
    for(i=0; i<__ping_sessions_max; ++i)
    {
      printf("Index[%u]--------------------\n", i);
      printf("Configurations:\n");
      printf("  IP Address:        %08X\n",       __ping_sessions[i].ipAddr);
      printf("  Probe Count:       %u\n",         __ping_sessions[i].probeCount);
      printf("  Probe Size:        %u\n",         __ping_sessions[i].probeSize);
      printf("  Probe Interval:    %u\n",         __ping_sessions[i].probeInterval);
      printf("Status:\n");
      printf("  Handle:            0x%08X\n",     __ping_sessions[i].handle);
      printf("  Is running:        %u\n",         __ping_sessions[i].isRunning);
      printf("  Probe Sent:        %u\n",         __ping_sessions[i].probeSent);
      printf("  Probe Success:     %u\n",         __ping_sessions[i].probeSucc);
      printf("  Probe Fail:        %u\n",         __ping_sessions[i].probeFail);
      printf("  Rtt (min/avg/max): %u/%u/%u\n",   __ping_sessions[i].minRtt, __ping_sessions[i].avgRtt, __ping_sessions[i].maxRtt);
    }
  }
}

/**
 * Dump the current status of the traceroute sessions array.
 * 
 * @param index : Traceroute session index. Use -1 to dump all sessions
 */
void ptin_routing_traceroutesession_dump(L7_uint32 index)
{
  L7_uint32 i;

  if(index > __traceroute_sessions_max)
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Requested index[%u] is higher than the maximum allowed number of traceroute sessions", index);
    return;
  }

  if(index != (L7_uint8)-1)
  {
    printf("Index[%u]--------------------\n", index);
    printf("Configurations:\n");
    printf("  IP Address:          %08X\n",   __traceroute_sessions[index].ipAddr);
    printf("  Probe Count:         %u\n",     __traceroute_sessions[index].probePerHop);
    printf("  Probe Size:          %u\n",     __traceroute_sessions[index].probeSize);
    printf("  Probe Interval:      %u\n",     __traceroute_sessions[index].probeInterval);
    printf("  Don't Frag:          %u\n",     __traceroute_sessions[index].dontFrag);
    printf("  Port:                %u\n",     __traceroute_sessions[index].port);
    printf("  Max TTL:             %u\n",     __traceroute_sessions[index].maxTtl);
    printf("  Init TTL:            %u\n",     __traceroute_sessions[index].initTtl);
    printf("  Max Fail:            %u\n",     __traceroute_sessions[index].maxFail);
    printf("Status:\n");           
    printf("  Handle:              0x%08X\n", __traceroute_sessions[index].handle);
    printf("  Is running:          %u\n",     __traceroute_sessions[index].isRunning);
    printf("  Current TTL:         %u\n",     __traceroute_sessions[index].currTtl);
    printf("  Current Hop Count:   %u\n",     __traceroute_sessions[index].currHopCount);
    printf("  Current Probe Count: %u\n",     __traceroute_sessions[index].currProbeCount);
    printf("  Test Attempts:       %u\n",     __traceroute_sessions[index].testAttempt);
    printf("  Test Success:        %u\n",     __traceroute_sessions[index].testSuccess);
  }
  else
  {
    for(i=0; i<__traceroute_sessions_max; ++i)
    {
      printf("Index[%u]--------------------\n", i);
      printf("Configurations:\n");
      printf("  IP Address:          %08X\n",   __traceroute_sessions[i].ipAddr);
      printf("  Probe Count:         %u\n",     __traceroute_sessions[i].probePerHop);
      printf("  Probe Size:          %u\n",     __traceroute_sessions[i].probeSize);
      printf("  Probe Interval:      %u\n",     __traceroute_sessions[i].probeInterval);
      printf("  Don't Frag:          %u\n",     __traceroute_sessions[i].dontFrag);
      printf("  Port:                %u\n",     __traceroute_sessions[i].port);
      printf("  Max TTL:             %u\n",     __traceroute_sessions[i].maxTtl);
      printf("  Init TTL:            %u\n",     __traceroute_sessions[i].initTtl);
      printf("  Max Fail:            %u\n",     __traceroute_sessions[i].maxFail);
      printf("Status:\n");           
      printf("  Handle:              0x%08X\n", __traceroute_sessions[i].handle);
      printf("  Is running:          %u\n",     __traceroute_sessions[i].isRunning);
      printf("  Current TTL:         %u\n",     __traceroute_sessions[i].currTtl);
      printf("  Current Hop Count:   %u\n",     __traceroute_sessions[i].currHopCount);
      printf("  Current Probe Count: %u\n",     __traceroute_sessions[i].currProbeCount);
      printf("  Test Attempts:       %u\n",     __traceroute_sessions[i].testAttempt);
      printf("  Test Success:        %u\n",     __traceroute_sessions[i].testSuccess);
    }
  }
}

/**
 * Dump the local traceroute hops snapshot.
 */
void ptin_routing_traceroutehopssnapshot_dump(void)
{
  L7_uint32                     currentIndex = 0;
  ptin_routing_traceroutehop_t *snapshotIterator;

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&__traceroutehops_snapshot, (dl_queue_elem_t**)&snapshotIterator))
  {
    printf("There are not hops for this traceroute session");
    return;
  }

  /* Copy local snapshot contents */
  while(snapshotIterator != NULL)
  {
    printf("Copying Hop[idx:%u]\n",  currentIndex);
    printf("  Handle:     0x%08X\n", snapshotIterator->sessionHandle);      
    printf("  TTL:        %u\n",     snapshotIterator->ttl);      
    printf("  IP Address: %08X\n",   snapshotIterator->ipAddr);   
    printf("  Min rtt:    %u\n",     snapshotIterator->minRtt);   
    printf("  Max rtt:    %u\n",     snapshotIterator->maxRtt);   
    printf("  Avg rtt:    %u\n",     snapshotIterator->avgRtt);   
    printf("  Probe Sent: %u\n",     snapshotIterator->probeSent);
    printf("  Probe Rcvd: %u\n",     snapshotIterator->probeRecv);

    /* Get next entry */
    snapshotIterator = (ptin_routing_traceroutehop_t*) dl_queue_get_next(&__traceroutehops_snapshot, (dl_queue_elem_t*)snapshotIterator);
    ++currentIndex;
  }
}

