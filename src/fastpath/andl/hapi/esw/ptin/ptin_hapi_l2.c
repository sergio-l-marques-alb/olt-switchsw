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
#include "l7_common.h"



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
  L7_uint32   mac_counter;
  L7_uint32   mac_total;
  L7_uint32   mac_limit;
  L7_uint32   old_limit;
  L7_uint32   mask;
  
  L7_uint8    enable;  
  L7_uint8    send_trap;    // If send trap feature is enable
  L7_uint8    trap_sent;    // If a trap was sent
  ptin_intf_t ptin_intf;    // PON interface
  L7_uint16   uni_ovid;     // GEM id
} mac_learn_info_t;

#define MAX_VLANS     4096
#define MAX_GPORTS    8192
#define DEFAULT_VALUE 500

static mac_learn_info_t macLearn_info_system;
static mac_learn_info_t macLearn_info_vlan[MAX_VLANS];
static mac_learn_info_t macLearn_info_flow[MAX_GPORTS];
static mac_learn_info_t macLearn_info_physical[L7_MAX_PORT_COUNT];
static mac_learn_info_t macLearn_info_lag[PTIN_SYSTEM_N_LAGS];

L7_BOOL ptin_hapi_l2_enable = 0;

void ptin_debug_hapi_l2_enable(L7_BOOL enable)
{
  ptin_hapi_l2_enable = enable;
}



/********************************************************************
 * STATIC FUNCTIONS DECLARATION
 ********************************************************************/
#if 0
/**
 * Reset number of learned MAC addresses (VLAN level)
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_hapi_maclimit_vlan_reset(bcm_vlan_t vlan_id);

/**
 * Reset number of learned MAC addresses (LAG level)
 * 
 * @param trunkId : Trunk id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_hapi_maclimit_lag_reset(bcm_trunk_t trunk_id);

/**
 * Reset number of learned MAC addresses (Physical Port level)
 * 
 * @param port_id : BCM port 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_hapi_maclimit_physical_reset(L7_uint port_id);

/**
 * Reset number of learned MAC addresses (System level)
 *  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
static L7_RC_t ptin_hapi_maclimit_system_reset(void);
#endif

/**
 * Reset number of learned MAC addresses 
 * 
 * @param mac_learn_info_t : mac_learn_info_ptr id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_reset( mac_learn_info_t *mac_learn_info_ptr);

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
    macLearn_info_vlan[i].mac_limit   = DEFAULT_VALUE; /* no limit */
    macLearn_info_vlan[i].enable      = L7_FALSE;
    macLearn_info_vlan[i].mask        = 0x0;

    macLearn_info_vlan[i].send_trap           = L7_FALSE;
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
    macLearn_info_flow[i].mac_limit   = DEFAULT_VALUE; /* no limit */
    macLearn_info_flow[i].mask        = 0x0;

    macLearn_info_flow[i].send_trap           = L7_FALSE;
    macLearn_info_flow[i].trap_sent           = L7_FALSE;
    macLearn_info_flow[i].ptin_intf.intf_type = 0;
    macLearn_info_flow[i].ptin_intf.intf_id   = 0;
    macLearn_info_flow[i].uni_ovid            = 0;
  }

  /* MAC learning control at physical port level */
  for (i=0; i<L7_MAX_PORT_COUNT; i++)
  {
    macLearn_info_physical[i].mac_counter = 0;
    macLearn_info_physical[i].mac_total   = 0;
    macLearn_info_physical[i].mac_limit   = DEFAULT_VALUE;  /*no limit */
    macLearn_info_physical[i].enable      = L7_FALSE;
    macLearn_info_physical[i].mask        = 0x0;

    macLearn_info_physical[i].send_trap           = L7_FALSE;
    macLearn_info_physical[i].trap_sent           = L7_FALSE;
    macLearn_info_physical[i].ptin_intf.intf_type = 0;
    macLearn_info_physical[i].ptin_intf.intf_id   = 0;
    macLearn_info_physical[i].uni_ovid            = 0;
  }

  /* MAC learning control at LAG port level */
  for (i=0; i<PTIN_SYSTEM_N_LAGS; i++)
  {
    macLearn_info_lag[i].mac_counter = 0;
    macLearn_info_lag[i].mac_total   = 0;
    macLearn_info_lag[i].mac_limit   = DEFAULT_VALUE;  /*no limit */
    macLearn_info_lag[i].enable      = L7_FALSE;
    macLearn_info_lag[i].mask        = 0x0;

    macLearn_info_lag[i].send_trap           = L7_FALSE;
    macLearn_info_lag[i].trap_sent           = L7_FALSE;
    macLearn_info_lag[i].ptin_intf.intf_type = 1;
    macLearn_info_lag[i].ptin_intf.intf_id   = 0;
    macLearn_info_lag[i].uni_ovid            = 0;
  }

  /* MAC learning control at System level */
  {
    macLearn_info_system.mac_counter          = 0;
    macLearn_info_system.mac_total            = 0;
    macLearn_info_system.mac_limit            = DEFAULT_VALUE;  /*no limit */
    macLearn_info_system.enable               = L7_FALSE;
    macLearn_info_system.mask                 = 0x0;
                  
    macLearn_info_system.send_trap            = L7_FALSE;
    macLearn_info_system.trap_sent            = L7_FALSE;
    macLearn_info_system.ptin_intf.intf_type  = 1;
    macLearn_info_system.ptin_intf.intf_id    = 0;
    macLearn_info_system.uni_ovid             = 0;
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
  L7_uint vport_id      = 0;
  L7_uint vlan_id       = 0;
  bcm_port_t bcm_port   = 0;
  L7_uint physical_port = 0; 
   
  
  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport)) // Check if a Virtual port level
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id >= MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].enable == L7_FALSE)
    {
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Count %d in %d ", macLearn_info_flow[vport_id].mac_counter, vport_id);

      macLearn_info_flow[vport_id].mac_counter++;
      macLearn_info_flow[vport_id].mac_total++;
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

      /* Enable the use of Pending Mechanism but disable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling FWD (GPORT=0x%x)", bcmx_l2_addr->lport);
      bcm_port_learn_set(0, bcmx_l2_addr->lport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );

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

    if (macLearn_info_flow[vport_id].mac_counter == macLearn_info_flow[vport_id].mac_limit)
    {
      if (macLearn_info_flow[vport_id].trap_sent == L7_FALSE)
      {
        send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_START, macLearn_info_flow[vport_id].uni_ovid);
        macLearn_info_flow[vport_id].trap_sent = L7_TRUE;
      }
    }
    return L7_SUCCESS;
  }
  /* Check if is a LAG port level */
  else if((bcmx_l2_addr->tgid >= 0) && ((bcmx_l2_addr->tgid < PTIN_SYSTEM_N_LAGS)))
  {
    L7_int tgid = bcmx_l2_addr->tgid;
     
    /* Feature enabled? */
    if (macLearn_info_lag[tgid].enable == L7_FALSE)
    { 
      macLearn_info_lag[tgid].mac_counter++;
      macLearn_info_lag[tgid].mac_total++;
      return L7_FAILURE;
    }
     
    #if (PTIN_BOARD == PTIN_BOARD_CXO640G)   
    /* Do not accept more mac addresses, if maximum was reached */
    if (macLearn_info_lag[tgid].mac_counter >= macLearn_info_lag[tgid].mac_limit)
    {    
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on LAG %d and GPORT 0x%x rejected (flags 0x%x)",
                __FUNCTION__, 
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                tgid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "LAG %d Over the limit", tgid);
       
      macLearn_info_lag[tgid].mac_total++;
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Increased total of MAC to %d in LAG %d", macLearn_info_lag[tgid].mac_total, tgid);
         
      if ((macLearn_info_lag[tgid].trap_sent == L7_FALSE && macLearn_info_lag[tgid].send_trap == L7_TRUE) && (macLearn_info_lag[tgid].mac_limit!=0))
      {
        send_trap_switch_event(macLearn_info_lag[tgid].ptin_intf.intf_type, macLearn_info_lag[tgid].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_START, 0);
        macLearn_info_lag[tgid].trap_sent = L7_TRUE;
      }
      return L7_FAILURE;
    }
    #endif 
      
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on LAG %d and GPORT 0x%x accepted (flags 0x%x)\r\n",
                __FUNCTION__, 
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                tgid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    if (!(bcmx_l2_addr->flags & BCM_L2_MOVE))
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Increase MAC Learned in LAG %d", tgid);
      macLearn_info_lag[tgid].mac_counter++;
      macLearn_info_lag[tgid].mac_total++;
    }

    if (macLearn_info_lag[tgid].mac_counter >= macLearn_info_lag[tgid].mac_limit)
    { 
      if ((macLearn_info_lag[tgid].trap_sent == L7_FALSE && macLearn_info_lag[tgid].send_trap == L7_TRUE) && (macLearn_info_lag[tgid].mac_limit!=0))
      {
        send_trap_switch_event(macLearn_info_lag[tgid].ptin_intf.intf_type, macLearn_info_lag[tgid].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_START, 0);
        macLearn_info_lag[tgid].trap_sent = L7_TRUE;
      }
    }

    #if (PTIN_BOARD == PTIN_BOARD_CXO640G)     
    /* Check if maximum was reached */
    if (macLearn_info_lag[tgid].mac_counter >= macLearn_info_lag[tgid].mac_limit)
    {
      /* Enable the use of Pending Mechanism but disable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling FWD (Physical Port=0x%x)", bcmx_l2_addr->lport);
                 bcm_port_learn_set(0, bcmx_l2_addr->lport, /*BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
      return L7_FAILURE;
    }
    #endif

    return L7_SUCCESS;     
  }
  /* Check if is a physical port */
  else if(BCMX_LPORT_VALID(bcmx_l2_addr->lport))
  {
    bcm_port = BCMX_LPORT_BCM_PORT(bcmx_l2_addr->lport);
    hapi_ptin_port_get(bcm_port, &physical_port);

    if(ptin_hapi_l2_enable)
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Is a Physical port");
   
    /* Physical port ID is valid? */
    if (physical_port >= L7_MAX_PORT_COUNT)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Physical is out of range! (physical_id=%u max=%u)", physical_port, L7_MAX_PORT_COUNT);
      return L7_FAILURE;
    }
    else
    {     
      /* Feature enabled? */
      if (macLearn_info_physical[physical_port].enable == L7_FALSE)
      { 
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Feature not enable");
        macLearn_info_physical[physical_port].mac_counter++;
        macLearn_info_physical[physical_port].mac_total++;
        return L7_FAILURE;
      }
      
      #if (PTIN_BOARD == PTIN_BOARD_CXO640G)    
      /* Do not accept more mac addresses, if maximum was reached */
      if (macLearn_info_physical[physical_port].mac_counter >= macLearn_info_physical[physical_port].mac_limit)
      { 
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on Physical port %d and GPORT 0x%x rejected (flags 0x%x)",
                __FUNCTION__, 
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                physical_port, bcmx_l2_addr->lport, bcmx_l2_addr->flags);
       
        macLearn_info_physical[physical_port].mac_total++;

        if(ptin_hapi_l2_enable)
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Increased total");
         
        if ((macLearn_info_physical[physical_port].trap_sent == L7_FALSE && macLearn_info_physical[physical_port].send_trap == L7_TRUE) && (macLearn_info_physical[physical_port].mac_limit!=0))
        {
          send_trap_switch_event(macLearn_info_physical[physical_port].ptin_intf.intf_type, physical_port, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_START, 0);
          macLearn_info_physical[physical_port].trap_sent = L7_TRUE;
        }
        
        if(ptin_hapi_l2_enable)
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Not increase the learned MAC in %d");
        return L7_FAILURE;
      }
      #endif

      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on Physical port %d and GPORT 0x%x accepted (flags 0x%x)\r\n",
                __FUNCTION__, 
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                physical_port, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

      macLearn_info_physical[physical_port].mac_counter++;
      macLearn_info_physical[physical_port].mac_total++;

      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Increase MAC Learned in Port %d to %d", physical_port, macLearn_info_physical[physical_port].mac_counter);

      if (macLearn_info_physical[physical_port].mac_counter >= macLearn_info_physical[physical_port].mac_limit)
      { 
        if (macLearn_info_physical[physical_port].trap_sent == L7_FALSE && macLearn_info_physical[physical_port].send_trap == L7_TRUE && (macLearn_info_physical[physical_port].mac_limit!=0))
        {
          send_trap_switch_event(macLearn_info_physical[physical_port].ptin_intf.intf_type, physical_port, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_START, 0);
          macLearn_info_physical[physical_port].trap_sent = L7_TRUE;
        }
      }
      #if (PTIN_BOARD == PTIN_BOARD_CXO640G)  
      /* Check if maximum was reached */
      if (macLearn_info_physical[physical_port].mac_counter >= macLearn_info_physical[physical_port].mac_limit)
      {
        /* Enable the use of Pending Mechanism but disable FWD */
        LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling FWD (Physical Port=0x%x)", bcmx_l2_addr->lport);
              bcm_port_learn_set(0, bcmx_l2_addr->lport, /*BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
      }
      #endif

      return L7_SUCCESS;    
    }
  }
  else 
  {  
    /* Check if is a VLAN port */
    vlan_id = bcmx_l2_addr->vid;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Check if is a VLAN port");

    /* VLAN ID is valid? */
    if (vlan_id >= MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }
    /* Feature enabled? */
    if (macLearn_info_vlan[vlan_id].enable == L7_FALSE)
    {
      return L7_FAILURE;
    }
    /* Do not accept more MAC addresses, if maximum was reached */
    if (macLearn_info_vlan[vlan_id].mac_counter >= macLearn_info_vlan[vlan_id].mac_limit)
    {
      if (macLearn_info_vlan[vlan_id].trap_sent == L7_FALSE && macLearn_info_vlan[vlan_id].send_trap == L7_TRUE)
      {
        send_trap_switch_event(macLearn_info_vlan[vlan_id].ptin_intf.intf_type, vlan_id, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_START, 0);
        macLearn_info_vlan[vlan_id].trap_sent = L7_TRUE;
      }

      return L7_FAILURE;
    }
    macLearn_info_vlan[vlan_id].mac_counter++;
    return L7_SUCCESS; 
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
  bcm_port_t bcm_port = 0; 
  L7_uint physical_port = 0;
  
  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id >= MAX_GPORTS)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Virtual port is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].enable == L7_FALSE)
    {
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Count %d in %d ", macLearn_info_flow[vport_id].mac_counter, vport_id);
      macLearn_info_flow[vport_id].mac_counter--;
      macLearn_info_flow[vport_id].mac_total--;
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
    if ( ((macLearn_info_flow[vport_id].mac_counter <= macLearn_info_flow[vport_id].mac_limit)) && (macLearn_info_flow[vport_id].mac_limit!=0) )
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
    return L7_SUCCESS;
  }

  /* Check if is a LAG port level */
  else if((bcmx_l2_addr->tgid >= 0) && ((bcmx_l2_addr->tgid < PTIN_SYSTEM_N_LAGS)))
  {
    L7_int tgid = bcmx_l2_addr->tgid;

    /* Feature enabled? */
    if (macLearn_info_lag[tgid].enable == L7_FALSE)
    {
      macLearn_info_lag[tgid].mac_counter--;
      macLearn_info_lag[tgid].mac_total--;
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on LAG %d and GPORT 0x%x cleared (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              tgid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    #if (PTIN_BOARD == PTIN_BOARD_CXO640G) 
    /* Do not accept more MAC addresses, if maximum was reached */
    /* if BCM_L2_PENDING is cleared, it means it is a aging of a learned MAC in the L2 table */
    if ( ((bcmx_l2_addr->flags & BCM_L2_PENDING) && !(bcmx_l2_addr->flags & BCM_L2_MOVE)) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x/LAG %d with PENDING flag",__FUNCTION__,
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                tgid);

      /* Decrement, but only if greater than 0 */
      if ((macLearn_info_lag[tgid].mac_total > macLearn_info_lag[tgid].mac_counter) && (macLearn_info_lag[tgid].mac_total > 0))
      {
        macLearn_info_lag[tgid].mac_total--;
      }
      return L7_FAILURE;
    }
    #endif

    /* Decrement, but only if greater than 0 */
    if (macLearn_info_lag[tgid].mac_counter > 0)
    {
      macLearn_info_lag[tgid].mac_counter--;
      macLearn_info_lag[tgid].mac_total--;
    }
  
    /* Check if maximum was reached */
    if ( (macLearn_info_lag[tgid].mac_counter <= macLearn_info_lag[tgid].mac_limit))
    {
      if (macLearn_info_lag[tgid].trap_sent == L7_TRUE)
      {
        send_trap_switch_event(macLearn_info_lag[tgid].ptin_intf.intf_type, tgid, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_END, 0);
        macLearn_info_lag[tgid].trap_sent = L7_FALSE;
      }
    }
  }
  /* Check if is a physical port */ 
  else if(BCMX_LPORT_VALID(bcmx_l2_addr->lport))
  {
    bcm_port = BCMX_LPORT_BCM_PORT(bcmx_l2_addr->lport);
    hapi_ptin_port_get(bcm_port, &physical_port); 

    /* Physical port ID is valid?  */
    if ( (physical_port >= L7_MAX_PORT_COUNT) )
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Physical is out of range! (physical_id=%u max=%u)", hapi_ptin_port_get(physical_port, &physical_port), L7_MAX_PORT_COUNT);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_physical[physical_port].enable == L7_FALSE)
    {
      macLearn_info_physical[physical_port].mac_counter--;
      macLearn_info_physical[physical_port].mac_total--;
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on Physical Port %d and GPORT 0x%x cleared (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              physical_port, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

     #if (PTIN_BOARD == PTIN_BOARD_CXO640G)  
     /*Do not accept more MAC addresses, if maximum was reached 
     if BCM_L2_PENDING is cleared, it means it is a aging of a learned MAC in the L2 table */
      
     if (((bcmx_l2_addr->flags & BCM_L2_PENDING) && !(bcmx_l2_addr->flags & BCM_L2_MOVE)))
     {
       LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x/VID %d with PENDING flag",__FUNCTION__,
                  bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                  bcmx_l2_addr->vid);

       /* Decrement, but only if greater than 0*/
       if ((macLearn_info_physical[physical_port].mac_total > macLearn_info_physical[physical_port].mac_counter) && (macLearn_info_physical[physical_port].mac_total > 0))
       {
          macLearn_info_physical[physical_port].mac_total--;
       }    
       return L7_FAILURE;
     }  
     #endif

     /* Decrement, but only if greater than 0 */
     if (macLearn_info_physical[physical_port].mac_counter > 0)
     {
       macLearn_info_physical[physical_port].mac_counter--;
       macLearn_info_physical[physical_port].mac_total--;
     }

     #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
     if ((macLearn_info_physical[physical_port].mac_limit) == macLearn_info_physical[physical_port].mac_counter)
     {
       /* Enable the FWD */
       LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enable FWD (Physical Port=0x%x)", bcmx_l2_addr->lport);
                 bcm_port_learn_set(0, bcmx_l2_addr->lport, BCM_PORT_LEARN_FWD /*BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_PENDING*/ | BCM_PORT_LEARN_ARL );
     }
     #endif
         
     /* Check if maximum was reached */
     if ((macLearn_info_physical[physical_port].mac_counter <= macLearn_info_physical[physical_port].mac_limit) && (macLearn_info_physical[physical_port].mac_limit!=0))
     {
       if (macLearn_info_physical[physical_port].trap_sent == L7_TRUE )
       {
         send_trap_switch_event(macLearn_info_physical[physical_port].ptin_intf.intf_type, physical_port, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_END, 0);
         macLearn_info_physical[physical_port].trap_sent = L7_FALSE;
       }
     }
   }
  /* Check if is a VLAN port level */
  else if (bcmx_l2_addr->vid !=0)
  {
   vlan_id = bcmx_l2_addr->vid;
   /* VLAN ID is valid? */
   if (vlan_id >= MAX_VLANS)
   {
     LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
     return L7_FAILURE;
   }
  
   /* Feature enabled? */
   if (macLearn_info_vlan[vlan_id].enable == L7_FALSE)
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
    if (vport_id >= MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Disabling Pending Mechanism (GPORT=0x%x)", gport);

    /* Disable the use of Pending Mechanism */
    bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU BCM_PORT_LEARN_PENDING |*/ BCM_PORT_LEARN_ARL );

    macLearn_info_flow[vport_id].mac_counter = 0;
    macLearn_info_flow[vport_id].mac_total = 0;
    macLearn_info_flow[vport_id].mac_limit = -1; /* no limit */

    macLearn_info_flow[vport_id].enable = L7_FALSE;

    /* Close the alarm */
    if (macLearn_info_flow[vport_id].trap_sent == L7_TRUE)
    {
      send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_END, 0);
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

#if 0
/**
 * Reset number of learned MAC addresses (VLAN level)
 * 
 * @param vlanId : VLAN id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_vlan_reset(bcm_vlan_t vlan_id)
{
  /* VLAN ID is valid? */
  if (vlan_id >= MAX_VLANS)
  {
    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (VLAN ID=%u max=%u)", vlan_id, MAX_VLANS);
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism (VLAN ID=%u)", vlan_id);

  macLearn_info_vlan[vlan_id].mac_counter = 0;
  macLearn_info_vlan[vlan_id].mac_limit = DEFAULT_VALUE; /* no limit */
  macLearn_info_vlan[vlan_id].trap_sent = L7_FALSE; /* TODO: Close the alarm */
  macLearn_info_vlan[vlan_id].enable = L7_FALSE; 

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses (Physical Port level)
 * 
 * @param port_id : BCM port 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_physical_reset(L7_uint port_id)
{
 
  /* Physical ID is valid? */
  if ((port_id < 0) && (port_id >= L7_MAX_PORT_COUNT))
  {
    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Physical is out of range! (Port ID=%u max=%u)", port_id, L7_MAX_PORT_COUNT);
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism (Port ID=%u)", port_id);

  macLearn_info_physical[port_id].mac_counter = 0;
  macLearn_info_physical[port_id].mac_limit = DEFAULT_VALUE; /* no limit */
  macLearn_info_physical[port_id].trap_sent = L7_FALSE; /* TODO: Close the alarm */
  macLearn_info_physical[port_id].enable = L7_FALSE; 

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses (LAG Port level)
 * 
 * @param trunk_id : Trunk id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_lag_reset(bcm_trunk_t trunk_id)
{
  /* Trunk ID is valid? */
  if (( trunk_id < 0) && (trunk_id >= PTIN_SYSTEM_N_LAGS))
  {
    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "LAG is out of range! (LAG ID=%u max=%u)", trunk_id, PTIN_SYSTEM_N_LAGS);
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism (LAG ID=%u)", trunk_id);

  macLearn_info_lag[trunk_id].mac_counter = 0;
  macLearn_info_lag[trunk_id].mac_limit = DEFAULT_VALUE; /* no limit */
  macLearn_info_lag[trunk_id].trap_sent = L7_FALSE; /* TODO: Close the alarm */
  macLearn_info_lag[trunk_id].enable = L7_FALSE; 

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses (System level)
 *  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_system_reset(void)
{
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism (System)");
    
  macLearn_info_system.mac_counter = 0;
  macLearn_info_system.mac_limit = DEFAULT_VALUE; /* no limit */
  macLearn_info_system.trap_sent = L7_FALSE; /* TODO: Close the alarm */
  macLearn_info_system.enable = L7_FALSE; 

  return L7_SUCCESS;
}
#endif

/**
 * Reset number of learned MAC addresses 
 * 
 * @param mac_learn_info_t : mac_learn_info_ptr id
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_reset( mac_learn_info_t *mac_learn_info_ptr)
{
  if (mac_learn_info_ptr == L7_NULLPTR)  
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid input parameters: mac_learn_info_ptr=%p", mac_learn_info_ptr);  
  }
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling MAC Limit Mechanism");  

  mac_learn_info_ptr->mac_counter = 0;
  mac_learn_info_ptr->mac_limit = DEFAULT_VALUE; /* no limit */
  mac_learn_info_ptr->trap_sent = L7_FALSE; /* TODO: Close the alarm */
  mac_learn_info_ptr->enable = L7_FALSE; 

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
    if (vport_id >= MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }
    if (mac_limit == (L7_uint8)-1)
    {
      rc = ptin_hapi_vport_maclimit_reset(gport);
      return rc;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling Pending Mechanism (GPORT=0x%x) with MAC Learned limit set to %u", gport, mac_limit);

    mac_limit_old = macLearn_info_flow[vport_id].mac_limit;
    macLearn_info_flow[vport_id].mac_limit = mac_limit;

    macLearn_info_flow[vport_id].enable = L7_TRUE;

    /* Check if maximum was reached */
    //if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_limit)
    //{
      /* Enable the use of Pending Mechanism but disable FWD*/
      //bcm_port_learn_set(0, gport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    //}
    //else
    //{

      /* Enable the use of Pending Mechanism and enable FWD */
      bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    //}

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
L7_RC_t ptin_hapi_maclimit_setmax(DAPI_USP_t *ddUsp, L7_uint16 vlan_id, L7_uint32 mac_limit, L7_uint8 action, L7_uint16 send_trap, DAPI_t *dapi_g )
{
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  bcm_l2_learn_limit_t   l2_learn_limit;     
  mac_learn_info_t      *macLearn_info_ptr;
  L7_uint32              old_flags;
  L7_uint32              old_limit;
  L7_uint8               old_send_trap;
  L7_RC_t                rc             = L7_SUCCESS;
  
  #if(PTIN_BOARD != PTIN_BOARD_CXO640G) /*Not supported in Trident(Plus). */
  bcm_error_t           rv;
  #endif

  bcm_l2_learn_limit_t_init(&l2_learn_limit);

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

  dapiPortPtr   = DAPI_PORT_GET( ddUsp, dapi_g );
  hapiPortPtr   = HAPI_PORT_GET( ddUsp, dapi_g );

  /* Extract lport */
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Analysing interface {%d,%d,%d}: lport=0x%08x", ddUsp->unit, ddUsp->slot, ddUsp->port, hapiPortPtr->bcmx_lport);

  /* Extract Physical port */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    L7_uint  physical_port = -1;

    hapi_ptin_port_get((bcm_port_t) hapiPortPtr->bcm_port, &physical_port);
    if ( (physical_port < 0) || (physical_port >= L7_MAX_PORT_COUNT) )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Port is out of range! (physical_port=%u max=%u)", physical_port, L7_MAX_PORT_COUNT);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a physical_port = %d", ddUsp->unit, ddUsp->slot, ddUsp->port, physical_port);
   
    /*Get Mac Limit Pointer*/
    macLearn_info_ptr = &macLearn_info_physical[physical_port];

    /*Set Port Id*/
    l2_learn_limit.port = (bcm_port_t) hapiPortPtr->bcm_port;

    /*Set Flags*/
    l2_learn_limit.flags   = BCM_L2_LEARN_LIMIT_PORT ;

   if(action == 1) 
   {
      l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU
   }

  }
  /* Extract Trunk id */
  else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    bcm_trunk_t trunk_id  = (bcm_trunk_t) hapiPortPtr->hapiModeparm.lag.tgid;

    /* Trunk ID is valid? */
    if (( trunk_id < 0) && (trunk_id >= PTIN_SYSTEM_N_LAGS))
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "LAG is out of range! (LAG ID=%u max=%u)", trunk_id, PTIN_SYSTEM_N_LAGS);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a lag: trunk_id = %d", ddUsp->unit, ddUsp->slot, ddUsp->port, trunk_id);    

    /*Get Mac Limit Pointer*/
    macLearn_info_ptr = &macLearn_info_lag[trunk_id];

    /*Set Trunk Id*/
    l2_learn_limit.trunk   = trunk_id;

    /*Set Flags*/
    l2_learn_limit.flags   = BCM_L2_LEARN_LIMIT_TRUNK;
    
    if(action == 1) 
    {
      l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU
    }
    
  }
  /* VLAN type if VLAN ID is valid? */
  else if ((vlan_id != 0) && (vlan_id < MAX_VLANS))
  {
    if(ptin_hapi_l2_enable)
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Valid VLAN ID=%d", vlan_id);
    
    /*Get Mac Limit Pointer*/
    macLearn_info_ptr = &macLearn_info_vlan[vlan_id];

    /*Set VLAN Id*/
    l2_learn_limit.vlan = (bcm_vlan_t) vlan_id;

    /*Set Flags*/
    l2_learn_limit.flags = BCM_L2_LEARN_LIMIT_VLAN;

    if(action == 1) 
    {
      l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU
    }

  }
  /* Limit is system wide. */
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Limit is system wide");

    /*Get Mac Limit Pointer*/
    macLearn_info_ptr = &macLearn_info_system;

    /*Set Flags*/
    l2_learn_limit.flags = BCM_L2_LEARN_LIMIT_SYSTEM;
  }

  if (action == (L7_uint8) -1)
  {
    // Use last action value
    l2_learn_limit.flags |= (macLearn_info_ptr->mask & 0x10) ;
  }

  /*Save Existing Values*/
  old_flags     =  macLearn_info_ptr->mask;
  old_limit     =  macLearn_info_ptr->old_limit;
  old_send_trap =  macLearn_info_ptr->send_trap;

  /*Disable Mac Limit*/
  if (mac_limit == (L7_uint32) -1 )
  {
    if (macLearn_info_ptr->enable == L7_DISABLE)
    {
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Mac Limiting is already disabled"); 
      return L7_SUCCESS;
    }

    l2_learn_limit.limit = (int) -1;
   
    /*Reset Mac Limit*/
    ptin_hapi_maclimit_reset(macLearn_info_ptr);
  }    
  else 
  {
    if(mac_limit == 0)/* Last limit */
    {      
      if ( old_limit == (L7_uint32) -1 )
      {
        l2_learn_limit.limit = (int) -1;
      }
      else
      {
        l2_learn_limit.limit = old_limit;
      }
    }      
    else  
    {
      l2_learn_limit.limit = mac_limit;
    }

    if(action == 1) 
    {
      l2_learn_limit.flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP; // BCM_L2_LEARN_LIMIT_ACTION_CPU
    }

    /* Update structure */
    macLearn_info_ptr->old_limit =   macLearn_info_ptr->mac_limit;
    macLearn_info_ptr->mac_limit  =  mac_limit;
    macLearn_info_ptr->mask       =  l2_learn_limit.flags;
    macLearn_info_ptr->enable     =  L7_ENABLE;

    /*True or False*/
    if ( (macLearn_info_ptr->send_trap != send_trap) && (send_trap == (send_trap & 0x01)) )
    {
      macLearn_info_ptr->send_trap = send_trap;
    }
  }
  
  if ( (l2_learn_limit.limit == old_limit) && (l2_learn_limit.flags == old_flags) )
  {
    if (macLearn_info_ptr->send_trap == old_send_trap)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Already Configured: limit:%u action:%u send_trap:%u flags:0x%.4X", mac_limit, action, send_trap, l2_learn_limit.flags);
      return L7_SUCCESS;//L7_ALREADY_CONFIGURED
    }
    else
    {
      /*Changed Trap Configuration*/
      return L7_SUCCESS;
    }    
  }
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "newLimit:%u newFlags:0x%.4X (oldLimit:%u oldFlags:0x%.4X)",l2_learn_limit.limit, l2_learn_limit.flags, old_limit, old_flags);
  
  
  #if(PTIN_BOARD != PTIN_BOARD_CXO640G) /*Not supported in Trident(Plus). */
  if (l2_learn_limit.flags == 0x00)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid flag parameters: 0x%.4X", l2_learn_limit.flags);
    return L7_FAILURE;
  }
  if ((rv=bcm_l2_learn_limit_set(0, &l2_learn_limit))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error (%d) setting L2 learn limit to %u", rv, mac_limit);
    return L7_FAILURE;
  }
  #else
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Not supported Yet!");
  #endif

 return rc;
}

/**
 * Get the status of the interface
 * 
 * @param  *mac_learned : Number of Mac learned 
 * @param  *status      : Status of the interface
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_maclimit_status(DAPI_USP_t *ddUsp, L7_uint32 *mac_learned, L7_uint32 *status, DAPI_t *dapi_g)
{
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  bcmx_lport_t          lport=-1;
  bcm_port_t            bcm_port=-1;
  L7_RC_t               rc = L7_SUCCESS;
  L7_uint               physical_port = 0;
  bcm_trunk_t           trunk_id=-1;
  //L7_uint               vlan_id = 0;
  //bcm_error_t           rv;
  
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

    *mac_learned = macLearn_info_lag[trunk_id].mac_counter;  
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Number of MAC Learned is %d in LAG %d (Limit %d, Total %d)", macLearn_info_lag[trunk_id].mac_counter, trunk_id, macLearn_info_lag[trunk_id].mac_limit,macLearn_info_lag[trunk_id].mac_total);

    if (macLearn_info_lag[trunk_id].mac_counter >= macLearn_info_lag[trunk_id].mac_limit && (macLearn_info_lag[trunk_id].mac_limit!=0))
    {
      *status = 1;
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Status = %d (Over the limit)", *status);
    }
    else
    {
      *status = 0;
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Status = %d (Within the limit)", *status);
    } 
  }
  /* Extract Physical port */
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    bcm_port = hapiPortPtr->bcm_port;
    hapi_ptin_port_get(bcm_port, &physical_port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a port: bcm_port = %d", ddUsp->unit, ddUsp->slot, ddUsp->port, bcm_port);

    *mac_learned = macLearn_info_physical[physical_port].mac_counter;  
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Number of MAC Learned is %d in physical port %d (Limit %d, Total %d)", macLearn_info_physical[physical_port].mac_counter, physical_port, macLearn_info_physical[physical_port].mac_limit,macLearn_info_physical[physical_port].mac_total);

    if ((macLearn_info_physical[physical_port].mac_counter >= macLearn_info_physical[physical_port].mac_limit) && (macLearn_info_physical[physical_port].mac_limit!=0) )
    {
      *status = 1;
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Status = %d (Over the limit)", *status);
    }
    else
    {
      *status = 0;
      if(ptin_hapi_l2_enable)
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Status = %d (Within the limit)", *status);
    }
  }

  /* VLAN ID is valid? 
  
  else if (vlan_id > MAX_VLANS)
  {

  }
  else if(BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;
  }*/
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Error providing the status of the interface {%d,%d,%d}", ddUsp->unit, ddUsp->slot, ddUsp->port);
    return rc=L7_FAILURE;
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
    if (vport_id >= MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "(GPORT=0x%x) MAC Learned limit information %u, bcm_port %u, outer_vid %d", gport, bcm_port, outer_vid);

    hapi_ptin_port_get(bcm_port, &port);

    macLearn_info_flow[vport_id].ptin_intf.intf_type =  PTIN_EVC_INTF_PHYSICAL;
    macLearn_info_flow[vport_id].ptin_intf.intf_id =    port;                   // PON interface
    macLearn_info_flow[vport_id].uni_ovid =             outer_vid;              // GEM id

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
  if (vlan_id >= MAX_VLANS)
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
void ptin_l2_maclimit_dump(void)
{
  L7_uint i;

  printf("MAC limit tables\n");

  /* MAC learning control at vlan level */
  for (i=0; i<MAX_VLANS; i++)
  {
    if (macLearn_info_vlan[i].enable != 0)
    {
      printf(" VLAN=%.4u   (enable:%u send_trap:%u old_limit:%u mask:0x%08x) %u of %u (total: %u) macs learned\n", i, macLearn_info_vlan[i].enable, macLearn_info_vlan[i].send_trap, macLearn_info_vlan[i].old_limit, macLearn_info_vlan[i].mask, macLearn_info_vlan[i].mac_counter, macLearn_info_vlan[i].mac_limit, macLearn_info_vlan[i].mac_total);
    }
  }

  /* MAC learning control at virtual port level */
  for (i=0; i<MAX_GPORTS; i++) 
  {
    if (macLearn_info_flow[i].enable != 0)
    {
      printf(" GPORT=0x%.4x  (enable:%u send_trap:%u old_limit:%u mask:0x%08x) %u of %u (total: %u) macs learned\n", i, macLearn_info_flow[i].enable, macLearn_info_flow[i].send_trap, macLearn_info_flow[i].old_limit, macLearn_info_flow[i].mask, macLearn_info_flow[i].mac_counter, macLearn_info_flow[i].mac_limit, macLearn_info_flow[i].mac_total);
    }
  }

   /* MAC learning control at physical port level */
  for (i=0; i<L7_MAX_PORT_COUNT; i++) 
  {
    if (macLearn_info_physical[i].enable != 0)
    {
      printf(" Physical PORT=0x%.4x  (enable:%u send_trap:%u old_limit:%u mask:0x%08x)  %u of %u (total: %u) macs learned\n", i, macLearn_info_physical[i].enable, macLearn_info_physical[i].send_trap, macLearn_info_physical[i].old_limit, macLearn_info_physical[i].mask,  macLearn_info_physical[i].mac_counter, macLearn_info_physical[i].mac_limit, macLearn_info_physical[i].mac_total);
    }
  }

  /* MAC learning control at physical port level */
  for (i=0; i<PTIN_SYSTEM_N_LAGS; i++) 
  {
    if (macLearn_info_lag[i].enable != 0)
    {
      printf(" LAG=0x%.4x  (enable:%u send_trap:%u old_limit:%u mask:0x%08x) %u of %u (total: %u) macs learned\n", i, macLearn_info_lag[i].enable, macLearn_info_lag[i].send_trap, macLearn_info_lag[i].old_limit, macLearn_info_lag[i].mask, macLearn_info_lag[i].mac_counter, macLearn_info_lag[i].mac_limit, macLearn_info_lag[i].mac_total);
    }
  }
  printf("\n");
  fflush(stdout);
}

L7_RC_t ptin_hapi_maclimit_trap( L7_uint16 int_f, L7_uint16 physical_port)
{
  send_trap_switch_event(int_f, physical_port, TRAP_ALARM_MAC_LIMIT_INTERFACE, TRAP_ALARM_STATUS_END, 0);
  return 1;
}


/* Enable port bridging for specified port. */
bcm_error_t port_bridge(L7_int8 unit, bcm_port_t port, L7_BOOL enable)
{
   bcm_error_t rv;

   if (BCM_FAILURE(rv = bcm_port_control_set(unit, port, bcmPortControlBridge, enable?1:0))) {
       printf("Failed to %s port bridging for port %d [%s]\n", enable? "enable":"disable", port,
              bcm_errmsg(rv));
       return rv;
   }
   printf("%s port bridging on port %d\n", enable? "Enabled":"Disabled", port);
   return BCM_E_NONE;
}

