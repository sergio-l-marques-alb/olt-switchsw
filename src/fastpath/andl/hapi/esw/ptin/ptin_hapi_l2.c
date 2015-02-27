/*
* L2
*
* Description: Generic L2 mechanisms implementation
*
*
* Author: Joao Mateiro
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_l2.h"




#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/switch.h>
#include <bcmx/multicast.h>

#include "ptin_hapi_xconnect.h"
#include "logger.h"
#include "ipc.h"

/********************************************************************
 * DEFINES
 ********************************************************************/

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/

/********************************************************************
 * INTERNAL VARIABLES
 ********************************************************************/

typedef struct
{
  L7_uint8  mac_counter;
  L7_uint16 mac_total;
  L7_uint8  mac_limit;

  L7_BOOL     trap_sent;
  ptin_intf_t ptin_intf;    // PON interface
  L7_uint16   uni_ovid;     // GEM id
} mac_learn_info_t;

#define MAX_VLANS   4096
#define MAX_GPORTS  8192

static mac_learn_info_t macLearn_info_vlan[MAX_VLANS+1];
static mac_learn_info_t macLearn_info_flow[MAX_GPORTS+1];


/********************************************************************
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/


/************************************
 * MAC Learning Control
 ************************************/
/**
 * Init MAC learning data structures
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_hapi_maclimit_init(void)
{
  L7_uint i;

  /* MAC learning control at vlan level */
  for (i=0; i<MAX_VLANS; i++)
  {
    macLearn_info_vlan[i].mac_counter = 0;
    macLearn_info_vlan[i].mac_total   = 0;
    macLearn_info_vlan[i].mac_limit   = (L7_uint8)-1; /* no limit */

    macLearn_info_vlan[i].trap_sent           = L7_FALSE;
    macLearn_info_vlan[i].ptin_intf.intf_type = 0;
    macLearn_info_vlan[i].ptin_intf.intf_id   = 0;
    macLearn_info_vlan[i].uni_ovid            = 0;
  }

  /* MAC learning control at virtual port level */
  for (i=0; i<MAX_GPORTS; i++)
  {
    macLearn_info_flow[i].mac_counter = 0;
    macLearn_info_flow[i].mac_total   = 0;
    macLearn_info_flow[i].mac_limit   = (L7_uint8)-1; /* no limit */

    macLearn_info_flow[i].trap_sent           = L7_FALSE;
    macLearn_info_flow[i].ptin_intf.intf_type = 0;
    macLearn_info_flow[i].ptin_intf.intf_id   = 0;
    macLearn_info_flow[i].uni_ovid            = 0;
  }

  return L7_SUCCESS;
}

/**
 * Increment number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_inc(bcmx_l2_addr_t *bcmx_l2_addr)
{
  L7_uint vport_id = 0;
  L7_uint vlan_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_limit == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Do not accept more mac addresses, if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_limit)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x rejected (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

      macLearn_info_flow[vport_id].mac_total++;

      if (macLearn_info_flow[vport_id].trap_sent == L7_FALSE)
      {
        send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_START, macLearn_info_flow[vport_id].uni_ovid);
        macLearn_info_flow[vport_id].trap_sent = L7_TRUE;
      }

      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x accepted (flags 0x%x)\r\n",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    macLearn_info_flow[vport_id].mac_counter++;
    macLearn_info_flow[vport_id].mac_total++;

    /* Check if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_limit)
    {
      /* Enable the use of Pending Mechanism but disable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling FWD (GPORT=0x%x)", bcmx_l2_addr->lport);
      bcm_port_learn_set(0, bcmx_l2_addr->lport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }

  }
  else
  {
    vlan_id = bcmx_l2_addr->vid;

    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_limit == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Do not accept more MAC addresses, if maximum was reached */
    if (macLearn_info_vlan[vlan_id].mac_counter >= macLearn_info_vlan[vlan_id].mac_limit)
    {
      return L7_FAILURE;
    }

    macLearn_info_vlan[vlan_id].mac_counter++;
  }

  return L7_SUCCESS;
}

/**
 * Decrement number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_dec(bcmx_l2_addr_t *bcmx_l2_addr)
{
  L7_uint vport_id = 0;
  L7_uint vlan_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_limit == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x cleared (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    /* Do not accept more MAC addresses, if maximum was reached */
    /* if BCM_L2_PENDING is cleared, it means it is a aging of a learned MAC in the L2 table */
    if ( ((bcmx_l2_addr->flags & BCM_L2_PENDING) && !(bcmx_l2_addr->flags & BCM_L2_MOVE)) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x/VID %d with PENDING flag",__FUNCTION__,
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                bcmx_l2_addr->vid);

      /* Decrement, but only if greater than 0 */
      if ((macLearn_info_flow[vport_id].mac_total > macLearn_info_flow[vport_id].mac_counter) && (macLearn_info_flow[vport_id].mac_total > 0))
      {
        macLearn_info_flow[vport_id].mac_total--;
      }

      return L7_FAILURE;
    }

    /* Decrement, but only if greater than 0 */
    if (macLearn_info_flow[vport_id].mac_counter > 0)
    {
      macLearn_info_flow[vport_id].mac_counter--;
      macLearn_info_flow[vport_id].mac_total--;
    }

    /* Check if maximum was reached */
    if ( (macLearn_info_flow[vport_id].mac_counter <= macLearn_info_flow[vport_id].mac_limit) )
    {
      if (macLearn_info_flow[vport_id].trap_sent == L7_TRUE)
      {
        send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_END, macLearn_info_flow[vport_id].uni_ovid);
        macLearn_info_flow[vport_id].trap_sent = L7_FALSE;
      }
    }

    #if 0
    /* Check if maximum was reached */
    if ( (macLearn_info_flow[vport_id].mac_counter < macLearn_info_flow[vport_id].mac_limit) && (macLearn_info_flow[vport_id].mac_total < macLearn_info_flow[vport_id].mac_limit))
    {
      /* Enable the use of Pending Mechanism and enable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling FWD (GPORT=0x%x)", bcmx_l2_addr->lport);
      bcm_port_learn_set(0, bcmx_l2_addr->lport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }
    #endif
  }
  else
  {
    vlan_id = bcmx_l2_addr->vid;

    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_limit == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Decrement, but only if greater than 0 */
    if (macLearn_info_vlan[vlan_id].mac_counter > 0)
    {
      macLearn_info_vlan[vlan_id].mac_counter--;
    }
  }

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses
 * 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vport_maclimit_reset(bcm_gport_t gport)
{
  L7_uint vport_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling Pending Mechanism (GPORT=0x%x)", gport);

    /* Disable the use of Pending Mechanism */
    bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD );

    macLearn_info_flow[vport_id].mac_counter = 0;
    macLearn_info_flow[vport_id].mac_total = 0;
    macLearn_info_flow[vport_id].mac_limit = (L7_uint8)-1; /* no limit */

    /* Close the alarm */
    if (macLearn_info_flow[vport_id].trap_sent == L7_TRUE)
    {
      send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_END, macLearn_info_flow[vport_id].uni_ovid);
      macLearn_info_flow[vport_id].trap_sent = L7_FALSE;
    }

  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI, "GPORT is not valid! (vport_id)", vport_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vlan_maclimit_reset(bcm_vlan_t vlan_id)
{
  /* VLAN ID is valid? */
  if (vlan_id > MAX_VLANS)
  {
    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism (VLAN ID=%u)", vlan_id);

  macLearn_info_vlan[vlan_id].mac_counter = 0;
  macLearn_info_vlan[vlan_id].mac_limit = (L7_uint8)-1; /* no limit */
  macLearn_info_vlan[vlan_id].trap_sent = L7_FALSE; /* TODO: Close the alarm */

  return L7_SUCCESS;
}

/**
 * 
 * 
 * @param vlan_id : VLAN ID 
 * @param gport :   GPort (virtual port)
 * @param type :    BROAD_FLUSH_BY_PORT | BROAD_FLUSH_BY_VLAN | 
 *                  BROAD_FLUSH_BY_MAC
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_fdbFlush(bcm_vlan_t vlan_id, bcm_gport_t gport, BROAD_FLUSH_TYPE_t type)
{
  L7_uint vport_id = 0;

  vport_id = gport & 0xffff;

  macLearn_info_flow[vport_id].mac_total -= macLearn_info_flow[vport_id].mac_counter;
  macLearn_info_flow[vport_id].mac_counter = 0;

  /* Flush FDB */
  BROAD_L2ADDR_FLUSH_t  l2addr_vlan;
  /* Fill in the structure */
  l2addr_vlan.bcmx_lport = gport;
  l2addr_vlan.vlanID = vlan_id;
  l2addr_vlan.flushtype = type;
  l2addr_vlan.port_is_lag = L7_FALSE;
  l2addr_vlan.tgid        = 0;
  memset(l2addr_vlan.mac.addr, 0, L7_ENET_MAC_ADDR_LEN);

  /* Send a message to L2 address flushing task with the vlan info */
  hapiBroadL2FlushRequest(l2addr_vlan);

  return L7_SUCCESS;
}

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vport_maclimit_setmax(bcm_gport_t gport, L7_uint8 mac_limit)
{
  L7_uint vport_id = 0;
  L7_uint8 mac_limit_old;
  L7_RC_t rc = L7_SUCCESS;

  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    if (mac_limit == (L7_uint8)-1)
    {
      if (macLearn_info_flow[vport_id].mac_limit != (L7_uint8)-1)
      {
        rc = ptin_hapi_vport_maclimit_reset(gport);
      }
      return rc;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling Pending Mechanism (GPORT=0x%x) with MAC Learned limit set to %u", gport, mac_limit);

    mac_limit_old = macLearn_info_flow[vport_id].mac_limit;
    macLearn_info_flow[vport_id].mac_limit = mac_limit;

    /* Check if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_limit)
    {
      /* Enable the use of Pending Mechanism but disable FWD*/
      bcm_port_learn_set(0, gport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }
    else
    {
      /* Enable the use of Pending Mechanism and enable FWD */
      bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }

    #if 0
    /* New MAX value lower than old value. Clear the L2 table. */
    if (mac_limit < mac_limit_old)
    {

      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Performing fdbFlush");

      ptin_hapi_maclimit_fdbFlush(vlan_id, gport, BROAD_FLUSH_BY_VLAN);
    }
    #endif

  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI, "GPORT is not valid! (vport_id)", vport_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_setmax(DAPI_USP_t *ddUsp, L7_uint vlan_id, L7_int mac_limit, DAPI_t *dapi_g)
{
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  bcmx_lport_t          lport=-1;
  bcm_trunk_t           trunk_id=-1;
  bcm_port_t            bcm_port=-1;
  bcm_l2_learn_limit_t  limit;
  L7_RC_t               rc = L7_SUCCESS;
  bcm_error_t           rv;

  bcm_l2_learn_limit_t_init(&limit);

  /* Validate interface */
  if (ddUsp==L7_NULLPTR || (ddUsp->unit<0 && ddUsp->slot<0 && ddUsp->port<0))
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"No provided interface!");
    return L7_SUCCESS;
  }
  if (ddUsp->unit<0 || ddUsp->slot<0 || ddUsp->port<0)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"Invalid interface!");
    return L7_FAILURE;
  }

  dapiPortPtr = DAPI_PORT_GET( ddUsp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( ddUsp, dapi_g );

  /* Extract lport */
  lport = hapiPortPtr->bcmx_lport;
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Analysing interface {%d,%d,%d}: lport=0x%08x", ddUsp->unit, ddUsp->slot, ddUsp->port, lport);

  /* Extract Trunk id */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    trunk_id = hapiPortPtr->hapiModeparm.lag.tgid;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a lag: trunk_id = %d", ddUsp->unit, ddUsp->slot, ddUsp->port, trunk_id);    

    limit.flags = BCM_L2_LEARN_LIMIT_TRUNK | BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU

    limit.trunk = trunk_id;
    limit.limit = mac_limit;
  }
  /* Extract Physical port */
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    bcm_port = hapiPortPtr->bcm_port;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a port: bcm_port = %d", ddUsp->unit, ddUsp->slot, ddUsp->port, bcm_port);

    limit.flags = BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU

    limit.port = bcm_port;
    limit.limit = mac_limit;

    /* VLAN ID is valid? */
    if ((vlan_id > 0) && (vlan_id < MAX_VLANS))
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Valid VLAN ID=%u", MAX_VLANS);

      limit.flags |= BCM_L2_LEARN_LIMIT_VLAN;
      limit.vlan = vlan_id;
    }
  }
  /* VLAN type if VLAN ID is valid? */
  else if ((vlan_id != 0) && (vlan_id <= MAX_VLANS))
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Valid VLAN ID=%u", vlan_id);

    limit.flags = BCM_L2_LEARN_LIMIT_VLAN | BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU

    limit.vlan = vlan_id;
    limit.limit = mac_limit;
  }
  /* Limit is system wide. */
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Limit is system wide");

    limit.flags = BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU

    limit.limit = mac_limit;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "flags  0x%.4X", limit.flags);
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "trunk  %u", limit.trunk);
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "port   %u", limit.port);
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "vlan   %u", limit.vlan);
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "limit  %u", limit.limit);

  if ((rv=bcm_l2_learn_limit_set(0, &limit))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error (%d) setting L2 learn limit to %u", rv, mac_limit);
    return L7_FAILURE;
  }

  if (mac_limit == (L7_uint8)-1)
  {
    if (macLearn_info_vlan[vlan_id].mac_limit != (L7_uint8)-1)
    {
      rc = ptin_hapi_vlan_maclimit_reset(vlan_id);
    }
  }

  return rc;
}

/**
 * Configures the information needed to generate alarms
 *  
 * @param gport     : GPort (virtual port)
 * @param intf_id   : Physical port
 * @param outer_vid : VLAN ID
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vport_maclimit_alarmconfig(bcm_gport_t gport, int bcm_port, L7_uint16 outer_vid)
{
  L7_uint vport_id = 0;
  L7_int  port;
  
  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "(GPORT=0x%x) MAC Learned limit information %u, bcm_port %u, outer_vid %d", gport, bcm_port, outer_vid);

    hapi_ptin_port_get(bcm_port, &port);

    macLearn_info_flow[vport_id].ptin_intf.intf_type =  PTIN_EVC_INTF_PHYSICAL;
    macLearn_info_flow[vport_id].ptin_intf.intf_id =    port;                   // PON interface
    macLearn_info_flow[vport_id].uni_ovid =             outer_vid;              // GEM id

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "port %u", port);
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI, "GPORT is not valid! (vport_id)", vport_id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Configures the information needed to generate alarms
 *  
 * @param vlan_id   : VLAN id 
 * @param outer_vid : VLAN id 
 * @param intf_id   : Physical port
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vlan_maclimit_alarmconfig(bcm_vlan_t vlan_id, int bcm_port, L7_uint16 outer_vid)
{
  /* VLAN ID is valid? */
  if (vlan_id > MAX_VLANS)
  {
    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Dump MAC limit tables
 * 
 * @author alex (3/5/2014)
 */
void ptin_maclimit_dump(void)
{
  L7_uint i;

  printf("MAC limit tables\n");

  /* MAC learning control at vlan level */
  for (i=0; i<MAX_VLANS; i++)
  {
    if (macLearn_info_vlan[i].mac_limit != (L7_uint8)-1)
    {
      printf(" VLAN=%.4u   %u of %u (total: %u)\n", i, macLearn_info_vlan[i].mac_counter, macLearn_info_vlan[i].mac_limit, macLearn_info_vlan[i].mac_total);
    }
  }

  /* MAC learning control at virtual port level */
  for (i=0; i<MAX_GPORTS; i++) 
  {
    if (macLearn_info_flow[i].mac_limit != (L7_uint8)-1)
    {
      printf(" GPORT=0x%.4x  %u of %u (total: %u)\n", i, macLearn_info_flow[i].mac_counter, macLearn_info_flow[i].mac_limit, macLearn_info_flow[i].mac_total);
    }
  }

  printf("\n");
  fflush(stdout);
}

