/*
*  C Implementation: switch_flow
*
* Description: 
*
*
* Author: Milton Ruas,,, <mruas@mruas-laptop>, (C) 2010
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <dapi_db.h>
#include <hpc_db.h>
#include <soc/counter.h>

#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_xlate.h"
#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_fp_bwpolicer.h"
#include "ptin_hapi_fp_counters.h"
#include "broad_policy.h"
#include "simapi.h"
#include "broad_group_bcm.h"

#include <bcmx/switch.h>
#include <bcmx/port.h>
#include <bcmx/l2.h>

/********************************************************************
 * DEFINES
 ********************************************************************/

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/

/********************************************************************
 * GLOBAL VARIABLES
 ********************************************************************/

/********************************************************************
 * INTERNAL VARIABLES
 ********************************************************************/

L7_int bcm_unit = 0;

/* Lookup map to provide internal port# based on physical port */
static DAPI_USP_t usp_map[PTIN_SYSTEM_N_PORTS];

BROAD_POLICY_t inband_policyId = 0;

#if (PTIN_BOARD==PTIN_BOARD_CXO640G)
 int ptin_sys_slotport_to_intf_map[PTIN_SYS_SLOTS_MAX][PTIN_SYS_INTFS_PER_SLOT_MAX];
 int ptin_sys_intf_to_slot_map[PTIN_SYSTEM_N_PORTS];
 int ptin_sys_intf_to_port_map[PTIN_SYSTEM_N_PORTS];
#endif

/********************************************************************
 * MACROS AND INLINE FUNCTIONS
 ********************************************************************/

/********************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 ********************************************************************/

static L7_RC_t hapi_ptin_portMap_init(void);

/**
 * Apply global switch configurations
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
//static L7_RC_t ptin_hapi_switch_init(void);


/********************************************************************
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initializes PTin HAPI data structures
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_data_init(void)
{
  L7_RC_t rc;

  /* Port mapping initializations (PLD mapping must be ok first!) */
  rc = hapi_ptin_portMap_init();
  if (rc != L7_SUCCESS)
    return L7_FAILURE;
  
  /* Field processor initializations */
  rc = hapi_ptin_bwPolicer_init();
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  rc = hapi_ptin_fpCounters_init();
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return rc;
}

/**
 * Initializes PTin HAPI configurations
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_config_init(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Switch initializations */
  if (ptin_hapi_switch_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* PHY initializations */
  if (ptin_hapi_phy_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* ptin_hapi_xlate initializations */
  if (ptin_hapi_xlate_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* ptin_hapi_bridge initializations */
  if (ptin_hapi_bridge_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  return rc;
}

/**
 * Initialize Switch control parameters
 * 
 * @author mruas (5/31/2012)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_switch_init(void)
{
  L7_RC_t    rc = L7_SUCCESS;

  if (bcmx_switch_control_set(bcmSwitchClassBasedMoveFailPktDrop,0x01)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error setting bcmSwitchClassBasedMoveFailPktDrop switch_control to 0x01");
    rc = L7_FAILURE;
  }

  LOG_INFO(LOG_CTX_PTIN_HAPI,"Switch %u initialized!", bcm_unit);

  return rc;
}

/**
 * Initialize PHY control parameters
 * 
 * @author asantos (07/02/2013)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_phy_init(void)
{
  L7_RC_t rc = L7_SUCCESS;

  #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
  int i, rv;
  L7_uint32 preemphasis;

  for (i=1; i<=PTIN_SYSTEM_N_PORTS; i++)
  {
//  rv = soc_phyctrl_control_set(0, i, SOC_PHY_CONTROL_8B10B, 0);
//
//  if (!SOC_SUCCESS(rv))
//  {
//    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error disabling 8b10b on port %u", i);
//    rc = L7_FAILURE;
//    break;
//  }

    #if 0
    /* Define preemphasis value according to port */
    /* Nearest slots, will use main=52, post=11 */
    if ( i <= 16 )
    {
      preemphasis = PTIN_PHY_PREEMPHASIS_NEAREST_SLOTS;
    }
    /* Farthest slots, will use main=44, post=19 */
    else if ( i > 40 )
    {
      preemphasis = PTIN_PHY_PREEMPHASIS_FARTHEST_SLOTS;
    }
    /* Middle slots, will use default main=48, post=15*/
    else
    {
      preemphasis = PTIN_PHY_PREEMPHASIS_DEFAULT;
    }
    #else
    /* Use these settings for all slots */
    preemphasis = PTIN_PHY_PREEMPHASIS_NEAREST_SLOTS;
    #endif
    
    rv = soc_phyctrl_control_set(0, i, SOC_PHY_CONTROL_PREEMPHASIS, preemphasis );

    if (!SOC_SUCCESS(rv))
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting preemphasis 0x%04X on port %u", preemphasis, i);
      rc = L7_FAILURE;
      break;
    }
  }
  #endif

  return rc;

}

/** 
 * Get bcm unit id for this switch. 
 * Normally is ZERO, but nervertheless it's better to be sure 
 * 
 * @param bcm_unit: switch unit id
 * 
 * @return L7_RC_t: always L7_SUCCESS
 */
L7_RC_t hapi_ptin_bcmUnit_get(L7_int *bcm_unit)
{
  if (bcm_unit!=L7_NULLPTR)
  {
    *bcm_unit = usp_map[0].unit;
  }

  return L7_SUCCESS;
}

/**
 * Get sdk port reference
 * 
 * @param port: PTin port id 
 *        bcm_port: Pointer to the location where the bcm_port
 *        will be stored
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bcmPort_get(L7_int port, L7_int *bcm_port)
{
  if (port>=PTIN_SYSTEM_N_PORTS)
    return L7_FAILURE;

  if (bcm_port!=L7_NULLPTR)
  {
    *bcm_port = usp_map[port].port;
  }

  return L7_SUCCESS;
}

/**
 * Get port if, from the sdk port reference
 * 
 * @param bcm_port: SDK port number 
 *        port: PTin port id
 *  
 * @return L7_RC_t: 
 *        L7_SUCCESS if found
 *        L7_FAILURE otherwise
 */
L7_RC_t hapi_ptin_port_get(L7_int bcm_port, L7_int *port)
{
  L7_int p;

  /* Search for the referenced bcm_port */
  for ( p = 0 ; p < PTIN_SYSTEM_N_PORTS && usp_map[p].port != bcm_port ; p++ );

  /* bcm_port was not found */
  if ( p >= PTIN_SYSTEM_N_PORTS )
    return L7_FAILURE;

  /* Return port number */
  if ( port != L7_NULLPTR )
    *port = p;

  return L7_SUCCESS;
}

/**
 * Get port bitmap in pbmp_t format for all physical ports
 * 
 * @param pbmp_mask : port bitmap
 */
void hapi_ptin_allportsbmp_get(pbmp_t *pbmp_mask)
{
  L7_int ptin_port;
  bcm_port_t bcm_port;

  /* Argument must not be a null pointer */
  if (pbmp_mask==L7_NULLPTR)
    return;

  /* Interfaces mask (for inports field) */
  BCM_PBMP_CLEAR(*pbmp_mask);
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS; ptin_port++)
  {
    if (hapi_ptin_bcmPort_get(ptin_port,&bcm_port)==L7_SUCCESS)
    {
      BCM_PBMP_PORT_ADD(*pbmp_mask,bcm_port);
      //LOG_TRACE(LOG_CTX_PTIN_HAPI,"Ptin port %d added to pbm_mask",ptin_port);
    }
  }
}

/**
 * Get port descriptor from ddUsp interface
 * 
 * @param ddUsp : unit, slot and port reference
 * @param dapi_g
 * @param intf_desc : interface descriptor with lport, bcm_port 
 *                  (-1 if not physical) and trunk_id (-1 if not
 *                  trunk)
 * @param pbmp : If is a physical port, it will be ADDED to this
 *             port bitmap.
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_portDescriptor_get(DAPI_USP_t *ddUsp, DAPI_t *dapi_g, ptin_hapi_intf_t *intf_desc, pbmp_t *pbmp)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcmx_lport_t  lport=-1;
  bcm_trunk_t   trunk_id=-1;
  bcm_port_t    bcm_port=-1;
  L7_uint32     class_port=0;

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
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Analysing interface {%d,%d,%d}: lport=0x%08x",ddUsp->unit,ddUsp->slot,ddUsp->port,lport);

  /* Extract Trunk id */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    trunk_id = hapiPortPtr->hapiModeparm.lag.tgid;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a lag: trunk_id = %d",ddUsp->unit,ddUsp->slot,ddUsp->port,trunk_id);
  }
  /* Extract Physical port */
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    bcm_port = hapiPortPtr->bcm_port;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Interface {%d,%d,%d} is a port: bcm_port = %d",ddUsp->unit,ddUsp->slot,ddUsp->port,bcm_port);
  }
  /* Not valid type */
  else
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Interface has a not valid type: error!");
    return L7_FAILURE;
  }

  /* Class port */
  class_port = (ddUsp->slot*L7_MAX_PORTS_PER_SLOT) + ddUsp->port + 1;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Classport of interface {%d,%d,%d} is %d",ddUsp->unit,ddUsp->slot,ddUsp->port,class_port);

  /* Add physical interface to port bitmap */
  if (pbmp!=L7_NULLPTR && bcm_port>=0)
  {
    BCM_PBMP_PORT_ADD(*pbmp,bcm_port);
  }

  /* Update interface descriptor */
  if (intf_desc!=L7_NULLPTR)
  {
    intf_desc->lport      = lport;
    intf_desc->trunk_id   = trunk_id;
    intf_desc->bcm_port   = bcm_port;
    intf_desc->class_port = class_port;
  }

  return L7_SUCCESS;
}


/**
 * Attribute L2 learning priority to specified port
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable : Enable MAC Learning 
 * @param stationMove_enable : Enable L2 Station Move 
 * @param stationMove_prio   : L2 Station Move priority
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_l2learn_port_set(ptin_dapi_port_t *dapiPort, L7_int macLearn_enable, L7_int stationMove_enable, L7_int stationMove_prio, L7_int stationMove_samePrio)
{
  L7_int    i, lclass;
  L7_uint32 flags;
  L7_BOOL   learn_class_move = L7_TRUE;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* MAC Learning enable */
  if (macLearn_enable>=0)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* LearnClass Enable/Disable */
      if (bcmx_port_control_set(hapiPortPtr->bcmx_lport, bcmPortControlLearnClassEnable,macLearn_enable & 1)!=BCM_E_NONE)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting bcmPortControlLearnClassEnable in port {%d,%d,%d} to %u",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, macLearn_enable);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Apply to all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_set(hapiPortPtr_member->bcmx_lport, bcmPortControlLearnClassEnable, macLearn_enable & 1)!=BCM_E_NONE)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
      }
    }
  }

  /* L2 Station move */
  if (stationMove_enable>=0)
  {
    flags = (stationMove_enable) ? (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD) : 0x00;

    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* L2 Station move */
      if (bcmx_port_control_set(hapiPortPtr->bcmx_lport, bcmPortControlL2Move, flags)!=BCM_E_NONE)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting bcmPortControlL2Move in port {%d,%d,%d} to 0x%02x",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, flags);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Apply to all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_set(hapiPortPtr_member->bcmx_lport, bcmPortControlL2Move, flags)!=BCM_E_NONE)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting bcmPortControlL2Move in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
      }
    }
  }

  /* Station Move with same priority ports */
  if (stationMove_samePrio>=0)
  {
    learn_class_move = stationMove_samePrio & 1;
  }

  /* Station move priority */
  if (stationMove_prio>=0)
  {
    /* Validate priority */
    if (stationMove_prio>3)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid priority: should be between 0 and 3");
      return L7_FAILURE;
    }

    /* Priority flags */
    flags = (learn_class_move) ? BCM_L2_LEARN_CLASS_MOVE : 0;

    lclass = stationMove_prio;

    /* Attribute priority to a class */
    if (bcmx_l2_learn_class_set(lclass, stationMove_prio, flags)!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting prio %d to class %d",stationMove_prio,lclass);
      return L7_FAILURE;
    }

    /* Associate class to the specified interface */
    if (bcmx_l2_learn_port_class_set(hapiPortPtr->bcmx_lport,lclass)!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class %d to port {%d,%d,%d}",lclass,
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
      return L7_FAILURE;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "L2Learn parameters attributed correctly to port {%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  return L7_SUCCESS;
}

/**
 * Get L2 learning attributes
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable    : Enable MAC Learning (output)
 * @param stationMove_enable : Enable L2 Station Move (output)
 * @param stationMove_prio   : L2 Station Move priority (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_l2learn_port_get(ptin_dapi_port_t *dapiPort, L7_int *macLearn_enable, L7_int *stationMove_enable, L7_int *stationMove_prio, L7_int *stationMove_samePrio)
{
  L7_int  lclass;
  L7_int  i, enable, enable_global, prio;
  L7_uint32     flags;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* MAC Learning enable */
  if (macLearn_enable!=L7_NULLPTR)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* LearnClass Enable/Disable */
      if (bcmx_port_control_get(hapiPortPtr->bcmx_lport, bcmPortControlLearnClassEnable,&enable_global)!=BCM_E_NONE)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d}",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
        return L7_FAILURE;
      }
    }
    /* If port is a lag, get all member port enables. If any has mac learning disabled, return disabled status */
    else
    {
      enable_global = 1;
      /* Run all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_get(hapiPortPtr_member->bcmx_lport, bcmPortControlLearnClassEnable, &enable)!=BCM_E_NONE)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* If not enabled, set global enable to FALSE, and break cycle */
        if (!enable)
        {
          enable_global = 0;
          break;
        }
      }
    }
    /* Save global enable status */
    *macLearn_enable = enable_global;
  }

  if (stationMove_enable!=L7_NULLPTR)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* L2 Station move */
      if (bcmx_port_control_get(hapiPortPtr->bcmx_lport, bcmPortControlL2Move, &flags)!=BCM_E_NONE)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcmPortControlL2Move flags in port {%d,%d,%d}",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
        return L7_FAILURE;
      }
      enable_global = (flags & BCM_PORT_LEARN_ARL);
    }
    /* If port is a lag, get all member port enables. If any has mac learning disabled, return disabled status */
    else
    {
      enable_global = 1;
      /* Run all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_get(hapiPortPtr_member->bcmx_lport, bcmPortControlL2Move, &flags)!=BCM_E_NONE)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcmPortControlL2Move in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* If not enabled, set global enable to FALSE, and break cycle */
        if (!(flags & BCM_PORT_LEARN_ARL))
        {
          enable_global = 0;
          break;
        }
      }
    }
    /* Save global enable status */
    *stationMove_enable = enable_global;
  }

  if (stationMove_prio!=L7_NULLPTR)
  {
    /* Get class id from the specified interface */
    if (bcmx_l2_learn_port_class_get(hapiPortPtr->bcmx_lport,&lclass)!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting classId from port {%d,%d,%d}",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
      return L7_FAILURE;
    }
    /* Get priority attribute */
    if (bcmx_l2_learn_class_get(lclass, &prio, &flags)!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting prio from classId %d",lclass);
      return L7_FAILURE;
    }
    *stationMove_prio = prio;

    if (stationMove_samePrio!=L7_NULLPTR)
    {
      *stationMove_samePrio = (flags & BCM_L2_LEARN_CLASS_MOVE);
    }
  }

  /* Station move for same priority ports? */

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "L2Learn parameters read correctly from port {%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  return L7_SUCCESS;
}

/**
 * Read counters (physical interfaces)
 *  
 * Note: currently masks are ignored, meaning that all values are read
 * 
 * @param portStats Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_read(ptin_HWEthRFC2819_PortStatistics_t *portStats)
{
  ptin_HWEthRFC2819_StatisticsBlock_t *rx, *tx;
  L7_uint64 tmp=0, tmp1=0, tmp2=0, tmp3=0;
  L7_uint64 mtuePkts=0;
  L7_uint64 pkts1519to2047, pkts2048to4095, pkts4096to9216, pkts9217to16383;
  L7_uint port, unit;
  L7_uint64 portbmp;

  if (portStats->Port >= PTIN_SYSTEM_N_PORTS)
    return L7_FAILURE;

  port = usp_map[portStats->Port].port;
  unit = usp_map[portStats->Port].unit;
  rx = &portStats->Rx;
  tx = &portStats->Tx;

  portbmp = ((L7_uint64)1) << portStats->Port;

  /* 1G or 2.5G Ethernet port ? */
  if (portbmp & (PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_PON_PORTS_MASK))
  {
    if (SOC_IS_VALKYRIE2(unit))
    {
      // Rx counters
      soc_counter_get(unit, port, GRMTUEr, 0, &mtuePkts);                             /* Packets > MTU bytes (good and bad) */
      /* PTin modified: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      soc_counter_get(unit, port, DROP_PKT_CNT_INGr, 0, &tmp1);
      rx->etherStatsDropEvents = tmp1 + mtuePkts;                                     /* Drop Events */
      #else
      soc_counter_get(unit, port, GRDROPr          , 0, &tmp1);
      soc_counter_get(unit, port, DROP_PKT_CNT_INGr, 0, &tmp2);
      rx->etherStatsDropEvents = tmp1 + tmp2 + mtuePkts;                              /* Drop Events */
      #endif
      soc_counter_get(unit, port, GRBYTr , 0, &tmp1);
      soc_counter_get(unit, port, RRBYTr , 0, &tmp2);
      rx->etherStatsOctets = tmp1 + tmp2;                                             /* Octets */
      soc_counter_get(unit, port, GRPKTr , 0, &rx->etherStatsPkts);                   /* Packets (>=64 bytes) */
      soc_counter_get(unit, port, GRBCAr , 0, &rx->etherStatsBroadcastPkts);          /* Broadcasts */
      soc_counter_get(unit, port, GRMCAr , 0, &rx->etherStatsMulticastPkts);          /* Muilticast */
      soc_counter_get(unit, port, GRFCSr , 0, &rx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
      rx->etherStatsCollisions = 0;                                                   /* Collisions */
      soc_counter_get(unit, port, GRUNDr , 0, &rx->etherStatsUndersizePkts);          /* Undersize */
      //soc_counter_get(unit, port, GROVRr,  0, &tmp1);
      //soc_counter_get(unit, port, GRMGVr,  0, &tmp2);
      //rx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
      soc_counter_get(unit, port, GROVRr,  0, &rx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */
      soc_counter_get(unit, port, GRFRGr , 0, &rx->etherStatsFragments);              /* Fragments */
      soc_counter_get(unit, port, GRJBRr , 0, &rx->etherStatsJabbers);                /* Jabbers */
      soc_counter_get(unit, port, GR64r  , 0, &rx->etherStatsPkts64Octets);           /* 64B packets */
      soc_counter_get(unit, port, GR127r , 0, &rx->etherStatsPkts65to127Octets);      /* 65-127B packets */
      soc_counter_get(unit, port, GR255r , 0, &rx->etherStatsPkts128to255Octets);     /* 128-255B packets */
      soc_counter_get(unit, port, GR511r , 0, &rx->etherStatsPkts256to511Octets);     /* 256-511B packets */
      soc_counter_get(unit, port, GR1023r, 0, &rx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */
      soc_counter_get(unit, port, GR1518r, 0, &rx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */

      soc_counter_get(unit, port, GR2047r, 0, &pkts1519to2047);                       /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, GR4095r, 0, &pkts2048to4095);                       /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, GR9216r, 0, &pkts4096to9216);                       /* 4096-9216 Bytes packets */
      pkts9217to16383 = 0;
      rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, GRBYTr , 0, &rx->Throughput);                  /* Throughput */

      // Tx counters
      soc_counter_get(unit, port, DROP_PKT_CNTr   , 0, &tmp1);
      //soc_counter_get(unit, port, HOLDROP_PKT_CNTr, 0, &tmp2);
      soc_counter_get(unit, port, EGRDROPPKTCOUNTr, 0, &tmp3);
      tx->etherStatsDropEvents = tmp1 + /*tmp2 +*/ tmp3;                              /* Drop Events */
      soc_counter_get(unit, port, GTBYTr , 0, &tx->etherStatsOctets);                 /* Octets */                   
      soc_counter_get(unit, port, GTPKTr , 0, &tx->etherStatsPkts);                   /* Packets */                  
      soc_counter_get(unit, port, GTBCAr , 0, &tx->etherStatsBroadcastPkts);          /* Broadcasts */               
      soc_counter_get(unit, port, GTMCAr , 0, &tx->etherStatsMulticastPkts);          /* Muilticast */               
      soc_counter_get(unit, port, GTFCSr , 0, &tx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
      tx->etherStatsCollisions = 0;                                                   /* Collisions */               
      tx->etherStatsUndersizePkts = 0;                                                /* Undersize */                
      //soc_counter_get(unit, port, GTOVRr,  0, &tmp1);
      //soc_counter_get(unit, port, GTMGVr,  0, &tmp2);
      //tx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
      soc_counter_get(unit, port, GTOVRr , 0, &tx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */               
      soc_counter_get(unit, port, GTFRGr , 0, &tx->etherStatsFragments);              /* Fragments */               
      soc_counter_get(unit, port, GTJBRr , 0, &tx->etherStatsJabbers);                /* Jabbers */                 
      soc_counter_get(unit, port, GT64r  , 0, &tx->etherStatsPkts64Octets);           /* 64B packets */             
      soc_counter_get(unit, port, GT127r , 0, &tx->etherStatsPkts65to127Octets);      /* 65-127B packets */         
      soc_counter_get(unit, port, GT255r , 0, &tx->etherStatsPkts128to255Octets);     /* 128-255B packets */        
      soc_counter_get(unit, port, GT511r , 0, &tx->etherStatsPkts256to511Octets);     /* 256-511B packets */        
      soc_counter_get(unit, port, GT1023r, 0, &tx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */       
      soc_counter_get(unit, port, GT1518r, 0, &tx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */

      soc_counter_get(unit, port, GT2047r, 0, &pkts1519to2047);                       /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, GT4095r, 0, &pkts2048to4095);                       /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, GT9216r, 0, &pkts4096to9216);                       /* 4096-9216 Bytes packets */
      pkts9217to16383 = 0;
      tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, GTBYTr , 0, &tx->Throughput);                  /* Throughput */
    }
    else if (SOC_IS_TRIDENT(unit))
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Trident SOC does not support GbE interfaces");
      return L7_FAILURE;
    }
  }
  /* 10G Ethernet port ? */
  else if (portbmp & PTIN_SYSTEM_10G_PORTS_MASK)
  {
    if (SOC_IS_VALKYRIE2(unit))
    {
      // Rx counters
      soc_counter_get(unit, port, IRMEGr , 0, &tmp1);
      soc_counter_get(unit, port, IRMEBr , 0, &tmp2);
      mtuePkts = tmp1 + tmp2;                                                         /* Packets > MTU bytes (good and bad) */
      /* PTin modified: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      soc_counter_get(unit, port, DROP_PKT_CNT_INGr, 0, &tmp1);
      tmp = tmp1;
      #else
      soc_counter_get(unit, port, IRDROPr          , 0, &tmp1);
      soc_counter_get(unit, port, DROP_PKT_CNT_INGr, 0, &tmp2);
      tmp = tmp1 + tmp2 + tmp3;
      #endif
      ( tmp >= mtuePkts ) ? ( tmp -= mtuePkts ) : ( tmp = 0 );
      rx->etherStatsDropEvents = tmp + mtuePkts;                                      /* Drop Events */
      soc_counter_get(unit, port, IRBYTr , 0, &rx->etherStatsOctets);                 /* Octets */                   
      soc_counter_get(unit, port, IRPKTr , 0, &rx->etherStatsPkts);                   /* Packets (>=64 bytes) */
      soc_counter_get(unit, port, IRBCAr , 0, &rx->etherStatsBroadcastPkts);          /* Broadcasts */               
      soc_counter_get(unit, port, IRMCAr , 0, &rx->etherStatsMulticastPkts);          /* Muilticast */               
      soc_counter_get(unit, port, IRFCSr , 0, &rx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
      rx->etherStatsCollisions = 0;                                                   /* Collisions */               
      soc_counter_get(unit, port, IRUNDr , 0, &rx->etherStatsUndersizePkts);          /* Undersize */                
      soc_counter_get(unit, port, IROVRr , 0, &rx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */ 
      soc_counter_get(unit, port, IRFRGr , 0, &rx->etherStatsFragments);              /* Fragments */
      soc_counter_get(unit, port, IRJBRr , 0, &rx->etherStatsJabbers);                /* Jabbers */                  
      soc_counter_get(unit, port, IR64r  , 0, &rx->etherStatsPkts64Octets);           /* 64B packets */              
      soc_counter_get(unit, port, IR127r , 0, &rx->etherStatsPkts65to127Octets);      /* 65-127B packets */          
      soc_counter_get(unit, port, IR255r , 0, &rx->etherStatsPkts128to255Octets);     /* 128-255B packets */         
      soc_counter_get(unit, port, IR511r , 0, &rx->etherStatsPkts256to511Octets);     /* 256-511B packets */         
      soc_counter_get(unit, port, IR1023r, 0, &rx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */        
      soc_counter_get(unit, port, IR1518r, 0, &rx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */       

      soc_counter_get(unit, port, IR2047r, 0, &pkts1519to2047);                       /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, IR4095r, 0, &pkts2048to4095);                       /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, IR9216r, 0, &pkts4096to9216);                       /* 4096-9216 Bytes packets */
      soc_counter_get(unit, port, IR16383r,0, &pkts9217to16383);                      /* 9217-16383 Bytes packets */
      rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, IRBYTr , 0, &rx->Throughput);                  /* Throughput */               

      // Tx counters
      soc_counter_get(unit, port, DROP_PKT_CNTr   , 0, &tmp1);
      //soc_counter_get(unit, port, HOLDROP_PKT_CNTr, 0, &tmp2);
      soc_counter_get(unit, port, EGRDROPPKTCOUNTr, 0, &tmp3);
      tx->etherStatsDropEvents = tmp1 + /*tmp2 +*/ tmp3;                              /* Drop Events */
      soc_counter_get(unit, port, ITBYTr , 0, &tx->etherStatsOctets);                 /* Octets */                   
      soc_counter_get(unit, port, ITPKTr , 0, &tx->etherStatsPkts);                   /* Packets */                  
      soc_counter_get(unit, port, ITBCAr , 0, &tx->etherStatsBroadcastPkts);          /* Broadcasts */               
      soc_counter_get(unit, port, ITMCAr , 0, &tx->etherStatsMulticastPkts);          /* Muilticast */               
      soc_counter_get(unit, port, ITFCSr , 0, &tx->etherStatsCRCAlignErrors);         /* FCS Errors (64-1518 bytes)*/
      tx->etherStatsCollisions = 0;                                                   /* Collisions */               
      tx->etherStatsUndersizePkts = 0;                                                /* Undersize */                
      soc_counter_get(unit, port, ITOVRr,  0, &tx->etherStatsOversizePkts);           /* Oversize: 1523-MTU bytes */ 
      soc_counter_get(unit, port, ITFRGr , 0, &tx->etherStatsFragments);              /* Fragments */                
      tx->etherStatsJabbers       = 0;                                                /* Jabbers */                  
      soc_counter_get(unit, port, IT64r  , 0, &tx->etherStatsPkts64Octets);           /* 64B packets */              
      soc_counter_get(unit, port, IT127r , 0, &tx->etherStatsPkts65to127Octets);      /* 65-127B packets */          
      soc_counter_get(unit, port, IT255r , 0, &tx->etherStatsPkts128to255Octets);     /* 128-255B packets */         
      soc_counter_get(unit, port, IT511r , 0, &tx->etherStatsPkts256to511Octets);     /* 256-511B packets */         
      soc_counter_get(unit, port, IT1023r, 0, &tx->etherStatsPkts512to1023Octets);    /* 512-1023B packets */        
      soc_counter_get(unit, port, IT1518r, 0, &tx->etherStatsPkts1024to1518Octets);   /* 1024-1518B packets */       

      soc_counter_get(unit, port, IT2047r, 0, &pkts1519to2047);                       /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, IT4095r, 0, &pkts2048to4095);                       /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, IT9216r, 0, &pkts4096to9216);                       /* 4096-9216 Bytes packets */
      soc_counter_get(unit, port, IT16383r,0, &pkts9217to16383);                      /* 9217-16383 Bytes packets */
      tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, ITBYTr , 0, &tx->Throughput);                  /* Throughput */
    }
    else if (SOC_IS_TRIDENT(unit))
    {
      /* Rx counters */
      soc_counter_get(unit, port, RMTUEr, 0, &mtuePkts);                              /* Packets > MTU bytes (good and bad) */
      soc_counter_get(unit, port, RDROPr           , 0, &tmp1);
      soc_counter_get(unit, port, DROP_PKT_CNT_INGr, 0, &tmp2);
      rx->etherStatsDropEvents = tmp1 + tmp2 + mtuePkts;                              /* Drop Events */
      soc_counter_get(unit, port, RBYTr , 0, &rx->etherStatsOctets);
      //soc_counter_get(unit, port, RBYTr , 0, &tmp1);
      //soc_counter_get(unit, port, RBYTr , 0, &tmp2);
      //rx->etherStatsOctets = tmp1 + tmp2;                                           /* Octets */
      soc_counter_get(unit, port, RPKTr , 0, &rx->etherStatsPkts);                    /* Packets (>=64 bytes) */
      soc_counter_get(unit, port, RBCAr , 0, &rx->etherStatsBroadcastPkts);           /* Broadcasts */
      soc_counter_get(unit, port, RMCAr , 0, &rx->etherStatsMulticastPkts);           /* Muilticast */
      soc_counter_get(unit, port, RFCSr , 0, &rx->etherStatsCRCAlignErrors);          /* FCS Errors (64-1518 bytes)*/
      rx->etherStatsCollisions = 0;                                                   /* Collisions */
      soc_counter_get(unit, port, RUNDr , 0, &rx->etherStatsUndersizePkts);           /* Undersize */
      //soc_counter_get(unit, port, ROVRr,  0, &tmp1);
      //soc_counter_get(unit, port, RMGVr,  0, &tmp2);
      //rx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
      soc_counter_get(unit, port, ROVRr,  0, &rx->etherStatsOversizePkts);            /* Oversize: 1523-MTU bytes */
      soc_counter_get(unit, port, RFRGr , 0, &rx->etherStatsFragments);               /* Fragments */
      soc_counter_get(unit, port, RJBRr , 0, &rx->etherStatsJabbers);                 /* Jabbers */
      soc_counter_get(unit, port, R64r  , 0, &rx->etherStatsPkts64Octets);            /* 64B packets */
      soc_counter_get(unit, port, R127r , 0, &rx->etherStatsPkts65to127Octets);       /* 65-127B packets */
      soc_counter_get(unit, port, R255r , 0, &rx->etherStatsPkts128to255Octets);      /* 128-255B packets */
      soc_counter_get(unit, port, R511r , 0, &rx->etherStatsPkts256to511Octets);      /* 256-511B packets */
      soc_counter_get(unit, port, R1023r, 0, &rx->etherStatsPkts512to1023Octets);     /* 512-1023B packets */
      soc_counter_get(unit, port, R1518r, 0, &rx->etherStatsPkts1024to1518Octets);    /* 1024-1518B packets */

      soc_counter_get(unit, port, R2047r, 0, &pkts1519to2047);                        /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, R4095r, 0, &pkts2048to4095);                        /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, R9216r, 0, &pkts4096to9216);                        /* 4096-9216 Bytes packets */
      pkts9217to16383 = 0;
      rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, RBYTr , 0, &rx->Throughput);                   /* Throughput */

      // Tx counters
      soc_counter_get(unit, port, DROP_PKT_CNTr   , 0, &tmp1);
      //soc_counter_get(unit, port, HOLDROP_PKT_CNTr, 0, &tmp2);
      soc_counter_get(unit, port, EGRDROPPKTCOUNTr, 0, &tmp3);
      tx->etherStatsDropEvents = tmp1 + /*tmp2 +*/ tmp3;                              /* Drop Events */
      soc_counter_get(unit, port, TBYTr , 0, &tx->etherStatsOctets);                  /* Octets */                   
      soc_counter_get(unit, port, TPKTr , 0, &tx->etherStatsPkts);                    /* Packets */                  
      soc_counter_get(unit, port, TBCAr , 0, &tx->etherStatsBroadcastPkts);           /* Broadcasts */               
      soc_counter_get(unit, port, TMCAr , 0, &tx->etherStatsMulticastPkts);           /* Muilticast */               
      soc_counter_get(unit, port, TFCSr , 0, &tx->etherStatsCRCAlignErrors);          /* FCS Errors (64-1518 bytes)*/
      tx->etherStatsCollisions = 0;                                                   /* Collisions */               
      tx->etherStatsUndersizePkts = 0;                                                /* Undersize */                
      //soc_counter_get(unit, port, TOVRr,  0, &tmp1);
      //soc_counter_get(unit, port, TMGVr,  0, &tmp2);
      //tx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
      soc_counter_get(unit, port, TOVRr,  0, &tx->etherStatsOversizePkts);            /* Oversize: 1523-MTU bytes */
      soc_counter_get(unit, port, TFRGr , 0, &tx->etherStatsFragments);               /* Fragments */               
      soc_counter_get(unit, port, TJBRr , 0, &tx->etherStatsJabbers);                 /* Jabbers */                 
      soc_counter_get(unit, port, T64r  , 0, &tx->etherStatsPkts64Octets);            /* 64B packets */             
      soc_counter_get(unit, port, T127r , 0, &tx->etherStatsPkts65to127Octets);       /* 65-127B packets */         
      soc_counter_get(unit, port, T255r , 0, &tx->etherStatsPkts128to255Octets);      /* 128-255B packets */        
      soc_counter_get(unit, port, T511r , 0, &tx->etherStatsPkts256to511Octets);      /* 256-511B packets */        
      soc_counter_get(unit, port, T1023r, 0, &tx->etherStatsPkts512to1023Octets);     /* 512-1023B packets */       
      soc_counter_get(unit, port, T1518r, 0, &tx->etherStatsPkts1024to1518Octets);    /* 1024-1518B packets */

      soc_counter_get(unit, port, T2047r, 0, &pkts1519to2047);                        /* 1519-2047 Bytes packets */
      soc_counter_get(unit, port, T4095r, 0, &pkts2048to4095);                        /* 2048-4095 Bytes packets */
      soc_counter_get(unit, port, T9216r, 0, &pkts4096to9216);                        /* 4096-9216 Bytes packets */
      pkts9217to16383 = 0;
      tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

      soc_counter_get_rate(unit, port, TBYTr , 0, &tx->Throughput);                   /* Throughput */
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "PTin port# %u is neither GbE or 10G interface", portStats->Port);
    return L7_FAILURE;
  }

/* PTin debug: drop events are not correctly read. Override with zero until this problem is solved! */
//LOG_WARNING(LOG_CTX_PTIN_HAPI, "RX and TX drop counters are always zero! Known bug to to solved!");
//rx->etherStatsDropEvents = 0;
//tx->etherStatsDropEvents = 0;

  /* Only missing undersize and fragment packets in packet counting */
  rx->etherStatsPkts += rx->etherStatsUndersizePkts + rx->etherStatsFragments;

  /* Adjust Throughput to Mbps */
  rx->Throughput *= 8;
  tx->Throughput *= 8;

  portStats->Mask = 0x03;
  portStats->RxMask = 0x7FBFF;  /* without Collisions, but include Pkts1024to1518Octets (0x20000) and Throughput (0x40000) */
  portStats->TxMask = 0x7FC9F;  /* without UndersizePkts, CRCAlignErrors, Fragments, Jabbers,
                                 * but include Pkts1024to1518Octets (0x20000) and Throughput (0x40000) */

  return L7_SUCCESS;
}


/**
 * Clears counters from a physical interface
 * 
 * @param port Port # (physical interface)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_clear(L7_uint phyPort)
{
  L7_uint port, unit;
  L7_uint64 portbmp;

  if (phyPort >= PTIN_SYSTEM_N_PORTS)
    return L7_FAILURE;

  port = usp_map[phyPort].port;
  unit = usp_map[phyPort].unit;

  portbmp = ((L7_uint64)1) << phyPort;

  /* 1G or 2.5G Ethernet port ? */
  if (portbmp & (PTIN_SYSTEM_ETH_PORTS_MASK | PTIN_SYSTEM_PON_PORTS_MASK))
  {
    if (SOC_IS_VALKYRIE2(unit))
    {
      /* Rx counters */
      soc_counter_set(unit, port, GRBYTr , 0, 0);
      soc_counter_set(unit, port, DROP_PKT_CNTr , 0, 0);
      soc_counter_set(unit, port, GRPKTr , 0, 0);
      soc_counter_set(unit, port, GRBCAr , 0, 0);
      soc_counter_set(unit, port, GRMCAr , 0, 0);
      soc_counter_set(unit, port, GRFCSr , 0, 0);
      soc_counter_set(unit, port, GRUNDr , 0, 0);
      soc_counter_set(unit, port, GROVRr , 0, 0);
      soc_counter_set(unit, port, GRFRGr , 0, 0);
      soc_counter_set(unit, port, GRJBRr , 0, 0);
      soc_counter_set(unit, port, GR64r  , 0, 0);
      soc_counter_set(unit, port, GR127r , 0, 0);
      soc_counter_set(unit, port, GR255r , 0, 0);
      soc_counter_set(unit, port, GR511r , 0, 0);
      soc_counter_set(unit, port, GR1023r, 0, 0);
      soc_counter_set(unit, port, GR1518r, 0, 0);

      /* Tx counters */
      soc_counter_set(unit, port, GTBYTr , 0, 0);
      soc_counter_set(unit, port, HOLDr ,  0, 0);
      soc_counter_set(unit, port, GTPKTr , 0, 0);
      soc_counter_set(unit, port, GTBCAr , 0, 0);
      soc_counter_set(unit, port, GTMCAr , 0, 0);
      soc_counter_set(unit, port, GTFCSr , 0, 0);
      soc_counter_set(unit, port, GTXCLr , 0, 0);
      soc_counter_set(unit, port, GTOVRr , 0, 0);
      soc_counter_set(unit, port, GTFRGr , 0, 0);
      soc_counter_set(unit, port, GTJBRr , 0, 0);
      soc_counter_set(unit, port, GT64r  , 0, 0);
      soc_counter_set(unit, port, GT127r , 0, 0);
      soc_counter_set(unit, port, GT255r , 0, 0);
      soc_counter_set(unit, port, GT511r , 0, 0);
      soc_counter_set(unit, port, GT1023r, 0, 0);
      soc_counter_set(unit, port, GT1518r, 0, 0);
    }
    else if (SOC_IS_TRIDENT(unit))
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Trident SOC does not support GbE interfaces");
      return L7_FAILURE;
    }
  }
  /* 10G Ethernet port ? */
  else if (portbmp & PTIN_SYSTEM_10G_PORTS_MASK)
  {
    if (SOC_IS_VALKYRIE2(unit))
    {
      /* Rx counters */
      soc_counter_set(unit, port, IRBYTr , 0, 0);
      soc_counter_set(unit, port, DROP_PKT_CNTr , 0, 0);
      soc_counter_set(unit, port, IRPKTr , 0, 0);
      soc_counter_set(unit, port, IRBCAr , 0, 0);
      soc_counter_set(unit, port, IRMCAr , 0, 0);
      soc_counter_set(unit, port, IRFCSr , 0, 0);
      soc_counter_set(unit, port, IRUNDr , 0, 0);
      soc_counter_set(unit, port, IROVRr , 0, 0);
      soc_counter_set(unit, port, IRFRGr , 0, 0);
      soc_counter_set(unit, port, IRJBRr , 0, 0);
      soc_counter_set(unit, port, IR64r  , 0, 0);
      soc_counter_set(unit, port, IR127r , 0, 0);
      soc_counter_set(unit, port, IR255r , 0, 0);
      soc_counter_set(unit, port, IR511r , 0, 0);
      soc_counter_set(unit, port, IR1023r, 0, 0);
      soc_counter_set(unit, port, IR1518r, 0, 0);

      /* Tx counters */
      soc_counter_set(unit, port, ITBYTr , 0, 0);
      soc_counter_set(unit, port, HOLDr ,  0, 0);
      soc_counter_set(unit, port, ITPKTr , 0, 0);
      soc_counter_set(unit, port, ITBCAr , 0, 0);
      soc_counter_set(unit, port, ITMCAr , 0, 0);
      soc_counter_set(unit, port, ITFCSr , 0, 0);
      //soc_counter_set(unit, port, ITXCLr , 0, 0);
      soc_counter_set(unit, port, ITOVRr , 0, 0);
      soc_counter_set(unit, port, ITFRGr , 0, 0);
      //soc_counter_set(unit, port, ITJBRr , 0, 0);
      soc_counter_set(unit, port, IT64r  , 0, 0);
      soc_counter_set(unit, port, IT127r , 0, 0);
      soc_counter_set(unit, port, IT255r , 0, 0);
      soc_counter_set(unit, port, IT511r , 0, 0);
      soc_counter_set(unit, port, IT1023r, 0, 0);
      soc_counter_set(unit, port, IT1518r, 0, 0);
    }
    else if (SOC_IS_TRIDENT(unit))
    {
      /* Rx counters */
      soc_counter_set(unit, port, RBYTr , 0, 0);
      soc_counter_set(unit, port, DROP_PKT_CNTr , 0, 0);
      soc_counter_set(unit, port, RPKTr , 0, 0);
      soc_counter_set(unit, port, RBCAr , 0, 0);
      soc_counter_set(unit, port, RMCAr , 0, 0);
      soc_counter_set(unit, port, RFCSr , 0, 0);
      soc_counter_set(unit, port, RUNDr , 0, 0);
      soc_counter_set(unit, port, ROVRr , 0, 0);
      soc_counter_set(unit, port, RFRGr , 0, 0);
      soc_counter_set(unit, port, RJBRr , 0, 0);
      soc_counter_set(unit, port, R64r  , 0, 0);
      soc_counter_set(unit, port, R127r , 0, 0);
      soc_counter_set(unit, port, R255r , 0, 0);
      soc_counter_set(unit, port, R511r , 0, 0);
      soc_counter_set(unit, port, R1023r, 0, 0);
      soc_counter_set(unit, port, R1518r, 0, 0);

      /* Tx counters */
      soc_counter_set(unit, port, TBYTr , 0, 0);
      soc_counter_set(unit, port, HOLDr ,  0, 0);
      soc_counter_set(unit, port, TPKTr , 0, 0);
      soc_counter_set(unit, port, TBCAr , 0, 0);
      soc_counter_set(unit, port, TMCAr , 0, 0);
      soc_counter_set(unit, port, TFCSr , 0, 0);
      //soc_counter_set(unit, port, TXCLr , 0, 0);
      soc_counter_set(unit, port, TOVRr , 0, 0);
      soc_counter_set(unit, port, TFRGr , 0, 0);
      //soc_counter_set(unit, port, TJBRr , 0, 0);
      soc_counter_set(unit, port, T64r  , 0, 0);
      soc_counter_set(unit, port, T127r , 0, 0);
      soc_counter_set(unit, port, T255r , 0, 0);
      soc_counter_set(unit, port, T511r , 0, 0);
      soc_counter_set(unit, port, T1023r, 0, 0);
      soc_counter_set(unit, port, T1518r, 0, 0);
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "PTin port# %u is neither GbE or 10G interface", phyPort);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Port# %u counters cleared", phyPort);
  return L7_SUCCESS;
}


/**
 * Get counters activity (physical interfaces)
 *  
 * Note: at the moment, masks are ignored, therefore all values 
 * are read for all ports)
 * 
 * @param portsActivity Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_activity_get(ptin_HWEth_PortsActivity_t *portsActivity)
{
  L7_uint port;
  L7_uint port_remap, unit;
  L7_uint32 old_mask;
  L7_uint64 rate;

  old_mask = portsActivity->ports_mask;
  portsActivity->ports_mask = 0;

  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++)
  {
    if (! (old_mask & (1<<port)))
      continue;

    port_remap = usp_map[port].port;
    unit       = usp_map[port].unit;
    portsActivity->ports_mask |= 1 << port;
    portsActivity->activity_bmap[port] = 0;

    /* The process used to read counters activity is to read its rate. This
     * may eventually fail to provide an accurate result in cases where rate
       it too low and return 0 */

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) {
      soc_counter_get_rate(unit, port_remap, GRBYTr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_ACTIVITY;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) {
      soc_counter_get_rate(unit, port_remap, GTBYTr, 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_TX_ACTIVITY;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_TX_COLLISIONS) {
      soc_counter_get_rate(unit, port_remap, GTXCLr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_TX_COLLISIONS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_CRC_ERRORS) {
      soc_counter_get_rate(unit, port_remap, GRFCSr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_CRC_ERRORS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_JABBERS) {
      soc_counter_get_rate(unit, port_remap, GRJBRr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_JABBERS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_FRAGMENTS) {
      soc_counter_get_rate(unit, port_remap, GRFRGr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_FRAGMENTS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS) {
      soc_counter_get_rate(unit, port_remap, GROVRr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_UNDERSIZEPACKETS) {
      soc_counter_get_rate(unit, port_remap, GRUNDr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_UNDERSIZEPACKETS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS) {
      soc_counter_get_rate(unit, port_remap, DROP_PKT_CNTr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS;
    }
  }

  return L7_SUCCESS;
}


L7_RC_t hapi_ptin_counters_read_debug(L7_uint phyPort)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  portStats.Port = phyPort;

  if (hapi_ptin_counters_read(&portStats) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error reading counter!");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_HAPI, "Rx.etherStatsPkts = %llu", portStats.Rx.etherStatsPkts);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, "Rx.Throughput     = %llu", portStats.Rx.Throughput);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, "Tx.etherStatsPkts = %llu", portStats.Tx.etherStatsPkts);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, "Tx.Throughput     = %llu", portStats.Tx.Throughput);

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_counters_activity_get_debug(L7_uint phyPort)
{
  ptin_HWEth_PortsActivity_t portsActivity;

  portsActivity.ports_mask = 1 << phyPort;
  portsActivity.activity_mask = 0xFFFF;

  if (hapi_ptin_counters_activity_get(&portsActivity) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error on hapi_ptin_counters_activity_get()");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_HAPI, "Port# %2u", phyPort);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, " .port_mask         = 0x%08X", portsActivity.ports_mask);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, " .activity_mask     = 0x%08X", portsActivity.activity_mask);
  LOG_DEBUG(LOG_CTX_PTIN_HAPI, " .activity_bmap[%02u] = 0x%08X", phyPort, portsActivity.activity_bmap[phyPort]);

  return L7_SUCCESS;
}

/**
 * Add a rate limiter to a particular traffic type
 * 
 * @param dapiPort : port
 * @param rateLimit : rate limit profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
#define RATE_LIMIT_MAX_VLANS 128
#define POLICY_VLAN_ID    0
#define POLICY_VLAN_MASK  1
#define POLICY_TRAF_TYPE  2

typedef struct
{
  L7_uint16       vlanId[3];
  BROAD_POLICY_t  policyId;
  L7_uint         ruleId;
} rateLimit_t;

static rateLimit_t rateLimit_list[RATE_LIMIT_MAX_VLANS];

L7_RC_t hapi_ptin_rateLimit_set(ptin_dapi_port_t *dapiPort, L7_BOOL enable, ptin_pktRateLimit_t *rateLimit)
{
  L7_RC_t                 result = L7_SUCCESS;
  static L7_BOOL          first_time = L7_TRUE;
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_uchar8               broadcast_mac[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                           FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uint16               vlanId, vlan_match = 0xfff, traffType;
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_uint16 index, index_free;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting RL processing");

  /* Initialization */
  if (first_time)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "First time processing... make some initializations");
    
    for (index=0; index<RATE_LIMIT_MAX_VLANS; index++)
    {
      rateLimit_list[index].vlanId[POLICY_VLAN_ID]    = L7_NULL;
      rateLimit_list[index].vlanId[POLICY_VLAN_MASK]  = L7_NULL;
      rateLimit_list[index].vlanId[POLICY_TRAF_TYPE]  = L7_NULL;

      rateLimit_list[index].policyId = BROAD_POLICY_INVALID;
      rateLimit_list[index].ruleId   = BROAD_POLICY_RULE_INVALID;
    }
    
    first_time   = L7_FALSE;
  }

  vlanId     = rateLimit->vlanId;
  vlan_match = 0xfff;
  traffType  = rateLimit->trafficType;

 #if (PTIN_SYSTEM_GROUP_VLANS)
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Original vlan = %u",vlanId);
  vlan_match = PTIN_VLAN_MASK(vlanId);
  vlanId &= vlan_match;
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"vlan = %u, mask=0x%04x",vlanId,vlan_match);
 #endif

    /* There are 3 set of policies for DHCP packets. Here is why.
   * When DHCP snooping is enabled, we will have a set of trusted and
   * untrusted ports. DHCP packets on trusted ports are copied to CPU
   * with elevated priorirty. DHCP packets on un-trusted ports are trapped
   * to CPU and are rate-limited (priority is not elevated).
   *
   * When DHCP snooping is disabled (default), priority of all DHCP packets
   * is elevated so that a bcast/mcast flood doesn't impact the DHCP leasing.
   * This is the default system-wide policy for DHCP packets, unless DHCP
   * snooping overrides this. Note, priority is elevated to just above
   * mcast/bcast/l3 miss packets.
   */
  /* DHCP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  if (traffType & PACKET_RATE_LIMIT_BROADCAST)
  {
    meterInfo.cir       = RATE_LIMIT_BCAST;
    meterInfo.cbs       = 128;
    meterInfo.pir       = RATE_LIMIT_BCAST;
    meterInfo.pbs       = 128;
    meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"No traffic type defined");
    return L7_FAILURE;
  }

  /* Validate vlan */
  if (vlanId==0 || vlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid Vlan (%u)",vlanId);
    return L7_FAILURE;
  }

  /* Find RL index */
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan provided is valid (%u). Enable=%u", vlanId, enable);

  index_free = (L7_uint16)-1;
  for (index=0; index<RATE_LIMIT_MAX_VLANS; index++)
  {
    if (index_free >= RATE_LIMIT_MAX_VLANS &&
       (rateLimit_list[index].vlanId[POLICY_VLAN_ID]==0 || rateLimit_list[index].vlanId[POLICY_VLAN_ID]>4095))
    {
      index_free = index;
    }
    if (vlanId == rateLimit_list[index].vlanId[POLICY_VLAN_ID] && traffType == rateLimit_list[index].vlanId[POLICY_TRAF_TYPE])
      break;
  }
  /* Not found... */
  if (index >= RATE_LIMIT_MAX_VLANS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Not found vlan %u within the configured ones", vlanId);

    /* If is going to add a new vlan, use first free index */
    if (enable)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to add vlan %u to table", vlanId);

      /* Check if a free index was found */
      if (index_free >= RATE_LIMIT_MAX_VLANS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "There is no room to add vlan %u", vlanId);
        return L7_TABLE_IS_FULL;
      }
      index = index_free;
      rateLimit_list[index].vlanId[POLICY_VLAN_ID]   = vlanId;     /* New vlan to be added */
      rateLimit_list[index].vlanId[POLICY_VLAN_MASK] = vlan_match;
      rateLimit_list[index].vlanId[POLICY_TRAF_TYPE] = traffType;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u added to table in cell %u", vlanId, index);
    }
    /* If it is to remove a vlan, and it was not found, return SUCCESS */
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Since it is to remove vlan %u, there is nothing to be done", vlanId);
      return L7_SUCCESS;
    }
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u was found in cell %u", vlanId, index);
    if (!enable)
    {
      rateLimit_list[index].vlanId[POLICY_VLAN_ID  ] = L7_NULL;    /* Vlan to be removed */
      rateLimit_list[index].vlanId[POLICY_VLAN_MASK] = L7_NULL;
      rateLimit_list[index].vlanId[POLICY_TRAF_TYPE] = L7_NULL;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u removed from cell %u", vlanId, index);
    }
    else
    {
      /* This Vlan already exists... nothing to be done */
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u already exists... nothing to be done!", vlanId);
      return L7_SUCCESS;
    }
  }

  /* If dhcp index is being used, at this point, delete it */
  if (rateLimit_list[index].policyId != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(rateLimit_list[index].policyId);
    rateLimit_list[index].policyId = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u deleted", index);
  }

  if (rateLimit_list[index].vlanId[POLICY_VLAN_ID]==0 || rateLimit_list[index].vlanId[POLICY_VLAN_ID]>4095)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "No further processing...");
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Processing cell #%u", index);

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)  break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u created", index);

    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&rateLimit_list[index].vlanId[POLICY_VLAN_ID], (L7_uchar8 *) &rateLimit_list[index].vlanId[POLICY_VLAN_MASK]);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, broadcast_mac, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
  } while ( 0 );

  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy of cell %u", index);
    if ((result=hapiBroadPolicyCommit(&policyId)) == L7_SUCCESS)
    {
      rateLimit_list[index].policyId  = policyId;
      rateLimit_list[index].ruleId    = ruleId;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy of cell %u commited successfully", index);
    }
  }
  else
  {
    hapiBroadPolicyCreateCancel();

    rateLimit_list[index].policyId = BROAD_POLICY_INVALID;
    rateLimit_list[index].ruleId   = BROAD_POLICY_RULE_INVALID;
    rateLimit_list[index].vlanId[POLICY_VLAN_ID  ] = L7_NULL;
    rateLimit_list[index].vlanId[POLICY_VLAN_MASK] = L7_NULL;
    rateLimit_list[index].vlanId[POLICY_TRAF_TYPE] = L7_NULL;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy of cell %u", index);
  }

  if (result != L7_SUCCESS && rateLimit_list[index].policyId != BROAD_POLICY_INVALID )
  {
    /* attempt to delete the policy in case it was created */
    (void)hapiBroadPolicyDelete(rateLimit_list[index].policyId);

    rateLimit_list[index].policyId = BROAD_POLICY_INVALID;
    rateLimit_list[index].ruleId   = BROAD_POLICY_RULE_INVALID;
    rateLimit_list[index].vlanId[POLICY_VLAN_ID  ] = L7_NULL;
    rateLimit_list[index].vlanId[POLICY_VLAN_MASK] = L7_NULL;
    rateLimit_list[index].vlanId[POLICY_TRAF_TYPE] = L7_NULL;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: deleting policy of cell %u", index);
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished dhcp trapping processing");

  return result;
}

/**
 * Dump list of bw policers
 */
void ptin_ratelimit_dump_debug(void)
{
  L7_int index;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;

  printf("Listing rate limiters list...\r\n");

  for (index=0; index<RATE_LIMIT_MAX_VLANS; index++)
  {
    if (rateLimit_list[index].vlanId[POLICY_VLAN_ID]==0)  continue;

    /* Also print hw group id and entry id*/
    if (l7_bcm_policy_hwInfo_get(0, rateLimit_list[index].policyId, rateLimit_list[index].ruleId, &group_id, &entry_id, L7_NULLPTR)==L7_SUCCESS)
    {
      printf(" Index#%-3u-> vlanId=%4u/0x%-4x: group=%-2d, entry=%-4d (PolicyId=%-4d RuleId %-4d)\r\n",
             index, rateLimit_list[index].vlanId[POLICY_VLAN_ID], rateLimit_list[index].vlanId[POLICY_VLAN_MASK],
             group_id, entry_id, rateLimit_list[index].policyId, rateLimit_list[index].ruleId);
    }
  }
  printf("Done!\r\n");
}

/********************************************************************
 * INTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initializes port mapping structures 
 *  
 * IMPORTANT: 
 *   On CXP360G, port mapping depends on the slot# (working vs protection)
 *   CPLD mapping MUST BE done prior to this function!
 * 
 * @return L7_RC_t L7_SUCCESS
 */
static L7_RC_t hapi_ptin_portMap_init(void)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
  L7_uint32                     slot, lane;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;
  #endif
  L7_uint i;
  
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;
  #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;
  #endif

/* Not necessary for CXO640G: sysbrds.c is already inverting slots for the protection matrix */
#if (PTIN_BOARD==PTIN_BOARD_CXP360G)
  const L7_uint32 portmap_work[] = PTIN_PORTMAP_SLOT_WORK;
  const L7_uint32 portmap_prot[] = PTIN_PORTMAP_SLOT_PROT;

  LOG_INFO(LOG_CTX_PTIN_HAPI, "Matrix board detected on %s slot",
           cpld_map->map[CPLD_SLOT_ID_REG] == PTIN_SLOT_WORK ? "working" : "protection");

  for (i = 0; i < min((sizeof(portmap_work)/portmap_work[0]), PTIN_SYSTEM_N_PORTS); i++)
  {
    /* Remap ports (only needed on protection slot) */
    if (cpld_map->map[CPLD_SLOT_ID_REG] == PTIN_SLOT_WORK)
      hapiSlotMapPtr[i].bcm_port = portmap_work[i];
    else if (cpld_map->map[CPLD_SLOT_ID_REG] == PTIN_SLOT_PROT)
      hapiSlotMapPtr[i].bcm_port = portmap_prot[i];
  }
#endif

  /* Initialize USP map */
  memset(usp_map, 0xff, sizeof(usp_map));   /* -1 for all values */

  #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
  /* Initialize slot/lane map */
  memset(ptin_sys_slotport_to_intf_map, 0xff, sizeof(ptin_sys_slotport_to_intf_map));   /* -1 for all values */
  memset(ptin_sys_intf_to_slot_map, 0xff, sizeof(ptin_sys_intf_to_slot_map));
  memset(ptin_sys_intf_to_port_map, 0xff, sizeof(ptin_sys_intf_to_port_map));
  #endif

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Port mapping:");
  for (i = 0; i < PTIN_SYSTEM_N_PORTS; i++)
  {
    /* It is assumed that: i = hapiSlotMapPtr[i].portNum
     * (check dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1 */

    usp_map[i].slot = hapiSlotMapPtr[i].slotNum;
    usp_map[i].unit = hapiSlotMapPtr[i].bcm_cpuunit;
    usp_map[i].port = hapiSlotMapPtr[i].bcm_port;

    #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
    /* Only 10/140/100Gbps ports */
    if ( hapiWCMapPtr[i].wcSpeedG > 1 )
    {
      slot = hapiWCMapPtr[i].slotNum - 1;
      lane = hapiWCMapPtr[i].wcLane;

      /* Update slot/lane to port map */
      if (slot<PTIN_SYS_SLOTS_MAX && lane<PTIN_SYS_INTFS_PER_SLOT_MAX)
      {
        ptin_sys_slotport_to_intf_map[slot][lane] = i;
      }
      ptin_sys_intf_to_slot_map[i] = slot+1;
      ptin_sys_intf_to_port_map[i] = lane;
    }
    #endif

    LOG_INFO(LOG_CTX_PTIN_HAPI, " Port# %2u => Remapped# bcm_port=%2u", i, usp_map[i].port);
  }

  #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
  printf("Slot to intf mapping:");
  for (slot=0; slot<PTIN_SYS_SLOTS_MAX; slot++)
  {
    printf("\n Slot %02u: ",slot+1);
    for (lane=0; lane<PTIN_SYS_INTFS_PER_SLOT_MAX; lane++)
    {
      printf(" %2d",ptin_sys_slotport_to_intf_map[slot][lane]);
    }
  }
  printf("\n");
  LOG_INFO(LOG_CTX_PTIN_HAPI,"Intf to slot/port map:");
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    LOG_INFO(LOG_CTX_PTIN_HAPI," Port# %2u => slot=%d/%d", i, ptin_sys_intf_to_slot_map[i], ptin_sys_intf_to_port_map[i]);
  }
  #endif

  /* BCM unit is globally accessible */
  bcm_unit = usp_map[0].unit;

  /* Initialize PTP interface port in XAUI mode (4 lanes) */
#if ( PTIN_BOARD == PTIN_BOARD_CXP360G )
  int ret;
  ret = bcm_port_control_set(0, PTIN_PTP_PORT, bcmPortControlLanes, 4);
  if (BCM_E_NONE != ret)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "bcm_port_control_set(0, %d, bcmPortControlLanes, 4) = %s\n", PTIN_PTP_PORT, bcm_errmsg(ret));
    return L7_FAILURE;
  }
  ret = bcm_port_autoneg_set(0, PTIN_PTP_PORT, FALSE);
  if (BCM_E_NONE != ret)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "bcm_port_autoneg_set(0, %d, FALSE) = %s\n", PTIN_PTP_PORT, bcm_errmsg(ret));
    return L7_FAILURE;
  }
  ret = bcm_port_speed_set(0, PTIN_PTP_PORT, 10000);
  if (BCM_E_NONE != ret)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "bcm_port_autoneg_set(0, %d, 10000) = %s\n", PTIN_PTP_PORT, bcm_errmsg(ret));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Port %d is now configured in XAUI mode (PTP interface)", PTIN_PTP_PORT);
#endif

  return L7_SUCCESS;
}

///**
// * Apply global switch configurations
// *
// * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
// */
//static L7_RC_t ptin_hapi_switch_init(void)
//{
//  L7_int port;
//  bcm_port_t bcm_port;
//  L7_BOOL error=FALSE;
//
//  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++)
//  {
//    /* Get bcm port */
//    if (hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)  continue;
//
//    /* Apply tpid values */
//    if (bcm_port_tpid_set(0, bcm_port, PTIN_TPID_OUTER_DEFAULT) != BCM_E_NONE)
//    {
//      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error applying Outer TPID (0x8100) in port %u",port);
//      error = TRUE;
//    }
//    if (bcm_port_inner_tpid_set(0, bcm_port, PTIN_TPID_INNER_DEFAULT) != BCM_E_NONE)
//    {
//      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error applying Inner TPID (0x8100) in port %u",port);
//      error = TRUE;
//    }
//  }
//
//  if (error)
//  {
//    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error applying TPID values");
//    return L7_FAILURE;
//  }
//
//  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Success applying TPID values");
//  return L7_SUCCESS;
//}

BROAD_POLICY_t policyId_trap = BROAD_POLICY_INVALID;
L7_int    trap_port = -1;
L7_uint16 trap_ovlan=0, trap_ivlan=0;
L7_uint8  trap_only_drops = 0;

void ptin_debug_trap_packets_state( void )
{
  /* Remove current policy */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    printf("No trap rule defined!\r\n");
    return;
  }

  printf("Trap rule defined:\r\n");

  if (trap_port>=0)
  {
    printf(" Inport = %d\r\n",trap_port);
  }
  if (trap_ovlan>0 && trap_ovlan<4096)
  {
    printf(" OVlan  = %u\r\n",trap_ovlan);
  }
  if (trap_ivlan>0 && trap_ivlan<4096)
  {
    printf(" IVlan  = %u\r\n",trap_ivlan);
  }
  if (trap_only_drops)
  {
    printf(" Only dropped packets are trapped.");
  }

  printf("Done!\r\n");
}


L7_RC_t ptin_debug_trap_packets_cancel( void )
{
  /* Remove current policy */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Trap Policy does not exist");
    return L7_SUCCESS;
  }

  /* Delete policy */
  if (hapiBroadPolicyDelete(policyId_trap)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Cannot delete Trap Policy");
    return L7_FAILURE;
  }

  policyId_trap = BROAD_POLICY_INVALID;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Trap Policy deleted");

  return L7_SUCCESS;
}


L7_RC_t ptin_debug_trap_packets( L7_int port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uint8 only_drops )
{
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  bcm_port_t          bcm_port;
  pbmp_t              pbm, pbm_mask;
  L7_uint16           vlan_mask = 0x0fff;
  L7_uint8            drop = 1, drop_mask = 1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (port<0 &&
      (ovlan==0 || ovlan>=4096) &&
      (ivlan==0 || ivlan>=4096) &&
      !only_drops)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "No rule provided!");
    return L7_SUCCESS;
  }

  if (port>=0)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Port %d was given",port);

    /* Validate port */
    if (hapi_ptin_bcmPort_get(port, &bcm_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcm_port of port %d",port);
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_port = %d",bcm_port);

    /* Define port bitmap */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm,bcm_port);
    hapi_ptin_allportsbmp_get(&pbm_mask);
  }
  else
  {
    port = -1;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "No port provided");
  }

  /* Remove current policy */
  if (ptin_debug_trap_packets_cancel()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error removing current trap policy");
    return L7_FAILURE;
  }

  meterInfo.cir       = 256;
  meterInfo.cbs       = 256;
  meterInfo.pir       = 256;
  meterInfo.pbs       = 256;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* Clear saved paremeters */
  trap_port  = -1;
  trap_ovlan =  0;
  trap_ivlan =  0;
  trap_only_drops = 0;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Cannot create trap policy");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "tRAP Policy created");

  /* Create rule */
  rc = hapiBroadPolicyRuleAdd(&ruleId /*, BROAD_POLICY_RULE_PRIORITY_HIGHEST*/);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding rule");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add source port qualifier */
  if (port>=0)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Adding port qualifier (port=%u, bcm_port=%d)",port,bcm_port);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding port qualifier (port=%u, bcm_port=%d)",port,bcm_port);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_port = port;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Port qualifier (port=%u, bcm_port=%d) added",port,bcm_port);
  }
  /* Add outer vlan qualifier */
  if (ovlan>0 && ovlan<4096)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Adding outer vlan qualifier (ovlan=%u)",ovlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ovlan, (L7_uchar8 *)&vlan_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding outer vlan qualifier (ovlan=%u/0x%03x)",ovlan,vlan_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ovlan = ovlan;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Outer vlan qualifier added (ovlan=%u/0x%03x)",ovlan,vlan_mask);
  }
  /* Add inner vlan qualifier */
  if (ivlan>0 && ivlan<4096)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Adding inner vlan qualifier (ivlan=%u)",ivlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uchar8 *)&ivlan, (L7_uchar8 *)&vlan_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding inner vlan qualifier (ivlan=%u/0x%03x)",ivlan,vlan_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ivlan = ivlan;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Inner vlan qualifier added (ivlan=%u/0x%03x)",ivlan,vlan_mask);
  }

  /* Add drop qualifer */
  if ( only_drops )
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Adding Drop qualifier (drop=%u)",drop);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uchar8 *)&drop, (L7_uchar8 *)&drop_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding drop qualifier (drop=%u/0x%02x)",drop,drop_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_only_drops = drop;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Drop qualifier added (drop=%u/0x%02x)",drop,drop_mask);
  }

  #if 0
  /* Ingress priority */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS, 0, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding action");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Ingress priority action added");
  #endif

  /* Trap to cpu action */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding trap_to_cpu action");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "trap_to_cpu action added");

  /* Drop all packets */
  rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding hard_drop action");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "hard_drop action added");

  /* Define meter action, to rate limit packets */
  rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding rate limit");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Rate limit added");

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting trap policy");
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting trap policy");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Trap policy commited successfully (policyId=%u)",policyId);

  /* Save policy id */
  policyId_trap = policyId;

  return L7_SUCCESS;
}

/**
 * Show trapped packets (to CPU) according to the configured 
 * trap rule 
 * 
 * @param bcm_port : Input port (bcm representation)
 * @param ovlan : Outer vlan
 * @param ivlan : Inner vlan
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_debug_trap_packets_show( L7_int bcm_port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uchar8 *packet_data )
{
  int i;
  int trap_bcm_port;

  /* Validate arguments */
  if ( bcm_port < 0 || packet_data == L7_NULLPTR )
  {
    return L7_SUCCESS;
  }

  /* Rule is defined? */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    return L7_SUCCESS;
  }

  /* Check if packet properties match the defined rule */
  if (trap_port>=0)
  {
    /* Validate port */
    if (hapi_ptin_bcmPort_get(trap_port, &trap_bcm_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcm_port of trap_port %d",trap_port);
      return L7_FAILURE;
    }
    if (bcm_port != trap_bcm_port)
      return L7_SUCCESS;
  }
  if (trap_ovlan>0 && trap_ovlan<4096)
  {
    if (ovlan != trap_ovlan)
      return L7_SUCCESS;
  }
  if (trap_ivlan>0 && trap_ivlan<4096)
  {
    if (ivlan != trap_ivlan)
      return L7_SUCCESS;
  }

  printf("Packet received on port %u (bcm_port %u), oVlan=%u, iVlan=%u:\r\n",
         trap_port, bcm_port, trap_ovlan, trap_ivlan);
  for (i=0; i<64; i++)
  {
    if (i%16==0)
    {
      if (i!=0)
        printf("\r\n");
      printf(" 0x%02x:",i);
    }
    printf(" %02x",packet_data[i]);
  }
  printf("\r\n");

  return L7_SUCCESS;
}

