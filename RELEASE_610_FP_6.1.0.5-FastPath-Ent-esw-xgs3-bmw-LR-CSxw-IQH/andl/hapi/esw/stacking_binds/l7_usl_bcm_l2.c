/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.c
*
* @purpose    Stubs to Handle synchronization responsibilities for Address,Trunk,VLAN, STG
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "comm_mask.h"
#include "soc/drv.h"
#include "soc/l2x.h"
#include "bcm/types.h"
#include "bcmx/trunk.h"
#include "bcmx/l2.h"
#include "bcmx/mcast.h"
#include "bcmx/multicast.h"
#include "bcmx/vlan.h"
#include "bcmx/wlan.h"
#include "bcmx/stg.h"
#include "bcm_int/esw/mbcm.h"
#include "bcm_int/esw/draco.h"
#include "bcm_int/esw/multicast.h"
#include "ibde.h"
#include <bcmx/bcmx_int.h>
#include "broad_l2_protected_group.h"
#include "broad_common.h"
#include "l7_usl_bcm_l2.h"

/* Local Structure used for VLAN based L2 addr flush */
typedef struct usl_unit_vlan_info_s {
  L7_uint32     unit;
  bcm_vlan_t    vid;
} usl_unit_vlan_info_t;

/* Local Structure used for Trunk based L2 addr flush */
typedef struct usl_unit_tgid_info_s {
  L7_uint32     unit;
  bcm_trunk_t   tgid;
} usl_unit_tgid_info_t;


/* Forward Declarations */
int usl_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid);
extern int _bcm_fb2_port_evc_tpid_index_set(int unit, bcm_port_t port, uint16 tpid);

/*********************************************************************
* @purpose  Initialize the Layer 2 tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_init()
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Release all resources allocated during usl_l2_init()
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were released
* @returns  L7_ERROR   - if any resourses were not released 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usl_l2_fini()
{
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Remove the MAC address if it matches the criteria
*
* @param   void           *user_data  - 
* @param   shr_avl_datum  *datum      - the MAC entry 
* @param   void           *extra_data - the logical port
*
* @returns L7_RC_t result
*
* @notes   
*
* @end
*
*********************************************************************/
int usl_l7_remove_l2_addr_by_trunk (void *user_data, shr_avl_datum_t *datum , void *extra_data)
{
#ifdef L7_ROBO_SUPPORT /*CHANGE DURING HAPI PORT*/ 
 return BCM_E_NONE;
#else
  L7_uint32             unit;
  L7_uint32             l2entry_tgid, l2entry_vlanid, l2entry_static;
  mac_addr_t            macAddr;
  l2x_entry_t          *l2x_entry;
  usl_unit_tgid_info_t *tgid_unit_info;


  if ((NULL == extra_data) || (NULL == datum))
    return(BCM_E_NONE);

  /* Initialize */
  l2entry_tgid = l2entry_vlanid = 0;

  tgid_unit_info = ((usl_unit_tgid_info_t *)extra_data);
  l2x_entry = (l2x_entry_t *) datum;

  /* Get the unit */
  unit = tgid_unit_info->unit;

  /* Now get the entry details */
  l2entry_tgid = soc_L2Xm_field32_get(unit, l2x_entry, TGID_PORTf);
  l2entry_vlanid = soc_L2Xm_field32_get(unit, l2x_entry, VLAN_IDf);
  l2entry_static = soc_L2Xm_field32_get(unit, l2x_entry, STATIC_BITf);

  /* don't flush static entries */
  if (l2entry_static)
  {
    return(BCM_E_NONE);
  }

  if (l2entry_tgid & BCM_TGID_TRUNK_INDICATOR(unit))
  {
    l2entry_tgid = l2entry_tgid & BCM_TGID_PORT_TRUNK_MASK(unit);
  }
  else
    return (BCM_E_NONE);

  soc_L2Xm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, macAddr);

  /* Now check the flush type */
  if (l2entry_tgid == tgid_unit_info->tgid)
  {
    /* Now call the broadcom driver to delete the entry */
    usl_bcmx_l2_addr_delete(macAddr, l2entry_vlanid);
  }

  return(BCM_E_NONE);
#endif
}

/*********************************************************************
*
* @purpose Remove the MAC address if it matches the criteria
*
* @param   void           *user_data  - 
* @param   shr_avl_datum  *datum      - the MAC entry 
* @param   void           *extra_data - the logical port
*
* @returns L7_RC_t result
*
* @notes   
*
* @end
*
*********************************************************************/
int usl_l7_remove_l2_addr_by_port (void *user_data, shr_avl_datum_t *datum , void *extra_data)
{
#ifdef L7_ROBO_SUPPORT /*CHANGE DURING HAPI PORT*/
  return BCM_E_NONE; 
#else
  L7_uint32             unit, port, modid;
  L7_uint32             l2entry_port, l2entry_modid, l2entry_vlanid, l2entry_static;
  mac_addr_t            macAddr;
  l2x_entry_t          *l2x_entry;
  bcmx_lport_t          lport;


  if ((NULL == extra_data) || (NULL == datum))
    return(BCM_E_NONE);

  /* Initialize */
  l2entry_port = l2entry_modid = l2entry_vlanid = 0;

  lport = *((bcmx_lport_t *)extra_data);
  l2x_entry = (l2x_entry_t *) datum;

  /* Get the unit, port and modid */
  unit = BCMX_LPORT_BCM_UNIT(lport);
  port = BCMX_LPORT_BCM_PORT(lport);
  modid = BCMX_LPORT_MODID (lport);

  /* Now get the entry details */
  l2entry_port = soc_L2Xm_field32_get(unit, l2x_entry, TGID_PORTf);
  l2entry_modid = soc_L2Xm_field32_get(unit, l2x_entry, MODULE_IDf);
  l2entry_vlanid = soc_L2Xm_field32_get(unit, l2x_entry, VLAN_IDf);
  l2entry_static = soc_L2Xm_field32_get(unit, l2x_entry, STATIC_BITf);

  /* don't flush static entries */
  if (l2entry_static)
  {
    return(BCM_E_NONE);
  }

  soc_L2Xm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, macAddr);

  /* Now check the flush type */
  if ((l2entry_modid == modid) && (l2entry_port == port))
  {
    /* Now call the broadcom driver to delete the entry */
    usl_bcmx_l2_addr_delete(macAddr, l2entry_vlanid);
  }

  return(BCM_E_NONE);
#endif
}


/*********************************************************************
*
* @purpose Remove the MAC address if it matches the criteria
*
* @param   void           *user_data  - 
* @param   shr_avl_datum  *datum      - the MAC entry 
* @param   void           *extra_data - the logical port
*
* @returns L7_RC_t result
*
* @notes   
*
* @end
*
*********************************************************************/
int usl_l7_remove_l2_addr_by_vlan (void *user_data, shr_avl_datum_t *datum , void *extra_data)
{
#ifdef L7_ROBO_SUPPORT /*CHANGE DURING HAPI PORT*/
return BCM_E_NONE;
#else
  L7_uint32             unit;
  L7_uint32             l2entry_port, l2entry_modid, l2entry_vlanid, l2entry_static;
  mac_addr_t            macAddr;
  l2x_entry_t          *l2x_entry;
  bcm_vlan_t            vid;
  usl_unit_vlan_info_t *vlan_unit_info;


  if ((NULL == extra_data) || (NULL == datum))
    return(BCM_E_NONE);

  /* Initialize */
  l2entry_port = l2entry_modid = l2entry_vlanid = 0;

  vlan_unit_info = (usl_unit_vlan_info_t *)extra_data;
  l2x_entry = (l2x_entry_t *) datum;

  unit = vlan_unit_info->unit;
  vid = vlan_unit_info->vid;

  /* Now get the entry details */
  l2entry_port = soc_L2Xm_field32_get(unit, l2x_entry, TGID_PORTf);
  l2entry_modid = soc_L2Xm_field32_get(unit, l2x_entry, MODULE_IDf);
  l2entry_vlanid = soc_L2Xm_field32_get(unit, l2x_entry, VLAN_IDf);
  l2entry_static = soc_L2Xm_field32_get(unit, l2x_entry, STATIC_BITf);

  /* don't flush static entries */
  if (l2entry_static)
  {
    return(BCM_E_NONE);
  }

  soc_L2Xm_mac_addr_get(unit, l2x_entry, MAC_ADDRf, macAddr);

  if (l2entry_vlanid == vid)
  {
    /* Now delete the entry */
    usl_bcmx_l2_addr_delete(macAddr, l2entry_vlanid);
  }

  return(BCM_E_NONE);
#endif
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified trunk.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_trunk (bcm_trunk_t tgid, L7_uint32 reactivateLearning)    /* PTin added */
{
  int                  rc = BCM_E_NONE;
  int                  i, bcm_unit;
  soc_control_t               *soc;
  usl_unit_tgid_info_t tgid_unit_info;
  
  BROAD_L2ADDR_FLUSH_t l2addr_msg;
  
  BCMX_UNIT_ITER(bcm_unit, i) {
	if (SOC_IS_HERCULES(bcm_unit))
	{
	  continue;
	}
    /* If 5690 then do the manual removal of all the learnt addresses
     * on the given port */
    if (SOC_IS_DRACO1(bcm_unit))
    {
      tgid_unit_info.unit = bcm_unit;
      tgid_unit_info.tgid = tgid;

      soc = SOC_CONTROL (bcm_unit);
      sal_mutex_take(soc->arlShadowMutex, -1);
	  shr_avl_traverse (soc->arlShadow, usl_l7_remove_l2_addr_by_trunk, &tgid_unit_info);
      sal_mutex_give(soc->arlShadowMutex);
    }
    else
    {
  rc =  bcm_l2_addr_delete_by_trunk (bcm_unit, tgid, 0);
    }
  }

  if (reactivateLearning)   // PTin modified
  {
    memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
    l2addr_msg.tgid = tgid;
    l2addr_msg.port_is_lag = L7_TRUE;
    hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);
  }

 return rc;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified wlan port.
*
* @param    port - BCMX wlan vp identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_wlan_port (bcmx_lport_t port)
{
  int          rc = BCM_E_NONE;

  rc = bcmx_l2_addr_delete_by_port(port, 0);

  return rc;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified lport.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_port (bcmx_lport_t lport, L7_uint32 reactivateLearning)    /* PTin added */
{
  int          rc = BCM_E_NONE;
  L7_uint32    unit;
  soc_control_t       *soc;
  BROAD_L2ADDR_FLUSH_t l2addr_msg;
  int modid; 
  bcm_port_t modport; 

  /* Get the soc structure for the unit */
  unit = BCMX_LPORT_BCM_UNIT(lport);
  soc = SOC_CONTROL (unit);
 
  /* If 5690 then do the manual removal of all the learnt addresses
   * on the given port */
  if (SOC_IS_DRACO1(unit))
  {
    sal_mutex_take(soc->arlShadowMutex, -1);
    shr_avl_traverse (soc->arlShadow, usl_l7_remove_l2_addr_by_port, &lport);
    sal_mutex_give(soc->arlShadowMutex);
  }
  else
  { /* In all other cases call the broadcom call to flush */
    bcmx_lport_to_modid_port(lport, &modid, &modport);
    rc = bcm_l2_addr_delete_by_port(unit, modid, modport, 0);
  }

  if (reactivateLearning)   // PTin modified
  {
    memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
    l2addr_msg.bcmx_lport = lport;
    l2addr_msg.port_is_lag = L7_FALSE;
    hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for all lport.
*
* @param    none.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_all (L7_uint32 reactivateLearning)    /* PTin added */
{
  int          rc = BCM_E_NONE;
  L7_uint32    unit;
  soc_control_t       *soc;
  BROAD_L2ADDR_FLUSH_t l2addr_msg;
  int modid; 
  bcm_port_t modport;
  bcmx_lport_t lport;

  BCMX_FOREACH_LPORT(lport) {
	  if ((BCMX_LPORT_FLAGS(lport) & BCMX_PORT_F_FE) ||
		  (BCMX_LPORT_FLAGS(lport) & BCMX_PORT_F_GE) ||
		  (BCMX_LPORT_FLAGS(lport) & BCMX_PORT_F_XE) ||
          (BCMX_LPORT_FLAGS(lport) & BCMX_PORT_F_HG)) {

		  /* Disable Hw Learning*/
		  memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
		  l2addr_msg.bcmx_lport = lport;
		  l2addr_msg.port_is_lag = L7_FALSE;
		  hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_DISABLE);

		  /* Get the soc structure for the unit */
		  unit = BCMX_LPORT_BCM_UNIT(lport);
		  soc = SOC_CONTROL (unit);
 
		 /* If 5690 then do the manual removal of all the learnt addresses
		  * on the given port */
		  if (SOC_IS_DRACO1(unit))
		  {
			  sal_mutex_take(soc->arlShadowMutex, -1);
			  shr_avl_traverse (soc->arlShadow, usl_l7_remove_l2_addr_by_port, &lport);
			  sal_mutex_give(soc->arlShadowMutex);
		  }
		  else
		  { /* In all other cases call the broadcom call to flush */
			  bcmx_lport_to_modid_port(lport, &modid, &modport);
			  rc = bcm_l2_addr_delete_by_port(unit, modid, modport, 0);
		  }

		  /* Re-Enable Learning */
      if (reactivateLearning)   // PTin modified
      {
        memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
        l2addr_msg.bcmx_lport = lport;
        l2addr_msg.port_is_lag = L7_FALSE;
        hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);
      }
	  }
  }

  return rc;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses matching specified vlan.
*
* @param    vid - BCMX VLAN identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_vlan (bcm_vlan_t vid)
{
  int                  rc = BCM_E_NONE;
  int                  i, bcm_unit;
  soc_control_t               *soc;
  usl_unit_vlan_info_t vlan_unit_info;

  BCMX_UNIT_ITER(bcm_unit, i) {
    if (SOC_IS_HERCULES(bcm_unit))
	{
	  continue;
	}
    /* If 5690 then do the manual removal of all the learnt addresses
     * on the given port */
    if (SOC_IS_DRACO1(bcm_unit))
    {
      vlan_unit_info.unit = bcm_unit;
      vlan_unit_info.vid = vid;

      soc = SOC_CONTROL (bcm_unit);

      sal_mutex_take(soc->arlShadowMutex, -1);
      shr_avl_traverse (soc->arlShadow, usl_l7_remove_l2_addr_by_vlan, &vlan_unit_info);
      sal_mutex_give(soc->arlShadowMutex);
    }
    else
    {
      rc = bcm_l2_addr_delete_by_vlan(bcm_unit, vid, 0);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Add a L2 MAC address to the HW's forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_add(bcmx_l2_addr_t * l2addr,bcmx_lplist_t *port_block)
{
  bcmx_lport_t	lport;
  int rc = BCM_E_FAIL;

  if ((l2addr->flags & BCM_L2_MCAST) && (_BCM_MULTICAST_IS_L2(l2addr->l2mc_index)))
  {
    BCMX_FOREACH_QUALIFIED_LPORT(lport,
                                 BCMX_PORT_F_HG|
                                 BCMX_PORT_F_STACK_INT|
                                 BCMX_PORT_F_STACK_EXT)
    {
      rc = bcmx_multicast_egress_add(l2addr->l2mc_index, lport, BCM_IF_INVALID);
      if (rc != BCM_E_NONE)
      {
        sysapiPrintf(" %s: rv %d; l2mc_index %d, lport %d \n ", __FUNCTION__,rc,l2addr->l2mc_index,lport);
      }
    }
  }
  return bcmx_l2_addr_add(l2addr,port_block);
}

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid)
{
  return bcmx_l2_addr_delete(mac_addr, vid);
}

/*********************************************************************
* @purpose  Set the aging time
*
* @param    ageTime     @{(input)} The aging time in seconds
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_age_timer_set(L7_int32 ageTime)
{
  return bcmx_l2_age_timer_set(ageTime);
}

/*********************************************************************
* @purpose  Create a VLAN in the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_vlan_create(bcm_vlan_t vid)
{
  return bcmx_vlan_create(vid);
}

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_destroy(bcm_vlan_t vid)
{
  return bcmx_vlan_destroy(vid);
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    stg          @{(input)} Pointer the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_create(bcm_stg_t *pStg)
{
  return bcmx_stg_create(pStg);
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_destroy(bcm_stg_t stg)
{
  return bcmx_stg_destroy(stg);
}

/*********************************************************************
* @purpose  Get the existing list of vlans for this spanning tree group
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vlanList     @{(input)} the list of vlans belonging to STG
* @param    vlanCount    @{(input)} Count of Vlans associated to STG
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_stg_vlan_list_get(bcm_stg_t stgId, bcm_vlan_t **vlanList, int *vlanCount)
{
  return bcmx_stg_vlan_list(stgId,vlanList,vlanCount);
}

/*********************************************************************
* @purpose  Destroy the vlan list obtained through list_get
*
* @param    vlanList     @{(input)} the list of vlans
* @param    vlanCount    @{(input)} Count of Vlans
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_stg_vlan_list_destroy(bcm_vlan_t *list, int count)
{
  return bcmx_stg_vlan_list_destroy(list,count);
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_vlan_add(bcm_stg_t stg,bcm_vlan_t vid)
{
  return bcmx_stg_vlan_add(stg, vid);
}

/*********************************************************************
* @purpose  Remove an association between a Spanning Tree group and a VLAN
*
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_vlan_remove(bcm_stg_t stg,bcm_vlan_t vid)
{
  return bcmx_stg_vlan_remove(stg,vid);
}

/*********************************************************************
* @purpose  Set hashing mechanism for existing trunk.
*
* @param    tid          @{(output)} The Trunk ID
* @param    psc          @{(output)} The hashing mechanism.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_psc_set (bcm_trunk_t tid, int psc)
{
  return bcmx_trunk_psc_set (tid, psc);
}

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_create(bcm_trunk_t * tid)
{
  return bcmx_trunk_create(tid);
}

/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_destroy(bcm_trunk_t tid)
{
  return bcmx_trunk_destroy(tid);
}


/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_set(bcm_trunk_t tid,bcmx_trunk_add_info_t * add_info)
{
  return bcmx_trunk_set(tid,add_info);
}

/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param    mcaddr       @{(input)} The multicast address, and flags to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_addr_add(bcmx_mcast_addr_t * mcaddr)
{
  return bcmx_mcast_addr_add(mcaddr);
}

/*********************************************************************
* @purpose  Remove a L2 Multicast address from the HW's database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_addr_remove( bcm_mac_t mac, bcm_vlan_t vid)
{
  return bcmx_mcast_addr_remove(mac, vid);
}

/*********************************************************************
* @purpose  Add a port to a L2 Multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    port         @{(input)} The port to be removed 
* @param    mcaddr       @{(input)} the MAC address
* @param    allrtr       @{(input)} the MAC address
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_join( bcm_mac_t mac,
                                bcm_vlan_t vid,
                                bcmx_lport_t port,
                                bcmx_mcast_addr_t * mcaddr,
                                bcmx_lplist_t * allrtr)
{
  return bcmx_mcast_join(mac, vid,port,mcaddr,allrtr);
}

/*********************************************************************
* @purpose  Add a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_join_ports(bcmx_mcast_addr_t *mcaddr)
{
  return bcmx_mcast_port_add(mcaddr);
}

/*********************************************************************
* @purpose  Add a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_join_ports(bcmx_l2_addr_t *mcaddr, bcmx_lplist_t addPorts)
{
  int rc = BCM_E_NONE;
  int lpcount;
  bcmx_lport_t lpport;

  BCMX_LPLIST_ITER(addPorts, lpport, lpcount)
  {
    rc = usl_bcmx_multicast_egress_add(mcaddr, lpport);
  }

  /* if we failed before the bcmx call, return code */
  return(rc);
}
/*********************************************************************
* @purpose  Remove a port from a L2 Multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    port         @{(input)} The port to be removed 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_leave ( bcm_mac_t mac, bcm_vlan_t vid , bcmx_lport_t port)
{
  return bcmx_mcast_leave(mac, vid, port);
}

/*********************************************************************
* @purpose  Create a multicast group
*
* @param    flags       @{(input)} flags for type of group
* @param    group       @{(output)} The multicast index
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_create(uint32 flags, bcm_multicast_t *group)
{
  return bcmx_multicast_create(flags, group);
}

/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param    mcaddr       @{(input)} The multicast address, and flags to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_addr_add(bcmx_l2_addr_t * mcaddr)
{
  int rc = BCM_E_NONE;
  L7_uint32 flags = BCM_MULTICAST_WITH_ID;
  bcmx_lport_t lport;

  if (_BCM_MULTICAST_IS_WLAN(mcaddr->l2mc_index))
  {
    flags |= (BCM_MULTICAST_TYPE_WLAN);
  }
  else
  {
    flags |= (BCM_MULTICAST_TYPE_L2);
  }

  if ((rc = bcmx_multicast_create(flags, &mcaddr->l2mc_index)) == BCM_E_NONE)
  {
    rc = bcmx_l2_addr_add(mcaddr, L7_NULL);

    if (rc == BCM_E_NONE)
    {
      BCMX_FOREACH_QUALIFIED_LPORT(lport,
                                   BCMX_PORT_F_HG|
                                   BCMX_PORT_F_STACK_INT|
                                   BCMX_PORT_F_STACK_EXT)
      {
        rc = bcmx_multicast_egress_add(mcaddr->l2mc_index, lport, BCM_IF_INVALID);
        if (rc != BCM_E_NONE)
        {
          sysapiPrintf(" %s: rv %d; l2mc_index %d, lport %d \n ", __FUNCTION__,rc,mcaddr->l2mc_index,lport);
        }
      }
    }
  }


  return rc;
}

/*********************************************************************
* @purpose  Remove a L2 Multicast address from the HW's database
*
* @param    mcaddr          @{(input)} the MAC address entry
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_addr_remove(bcmx_l2_addr_t * mcaddr)
{

  (void) bcmx_l2_addr_delete(mcaddr->mac, mcaddr->vid);

  return bcmx_multicast_destroy(mcaddr->l2mc_index);
}

/*********************************************************************
* @purpose  Add a port to a L2 Multicast address 
*
* @param    group          @{(input)} the mcast group id
* @param    port           @{(input)} the gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_egress_add(bcmx_l2_addr_t *l2addr,
                                  bcm_gport_t port)
{
  int rc = BCM_E_NONE;
  bcm_if_t encap_id;
#ifdef INCLUDE_L3
  bcm_gport_t wlanPort = 0;

  if (BCM_GPORT_IS_WLAN_PORT(port))
  {
    bcm_wlan_port_t wlan_port_info;

    memset(&wlan_port_info, 0, sizeof(wlan_port_info));

    rc = bcmx_multicast_wlan_encap_get(l2addr->l2mc_index, port, port, &encap_id);
    if (rc == BCM_E_NONE)
    {
      rc = bcmx_wlan_port_get(port, &wlan_port_info);
      wlanPort = port;
      port = wlan_port_info.port;
    }
  }
  else
#endif /* INCLUDE_L3 */
  {
    encap_id = BCM_IF_INVALID;
    rc = BCM_E_NONE;
  }

  if (rc != BCM_E_NONE)
  {
    return rc;
  }

  return bcmx_multicast_egress_add(l2addr->l2mc_index, port, encap_id);
}

/*********************************************************************
* @purpose  Remove a port from a L2 Multicast address 
*
* @param    group          @{(input)} the mcast group id
* @param    port           @{(input)} the gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_egress_delete(bcmx_l2_addr_t *l2addr, 
                                     bcm_gport_t port)
{
  int rc = BCM_E_NONE;
  bcm_if_t encap_id;
#ifdef INCLUDE_L3
  bcm_gport_t wlanPort = 0;

  if (BCM_GPORT_IS_WLAN_PORT(port))
  {
    bcm_wlan_port_t wlan_port_info;

    memset(&wlan_port_info, 0, sizeof(wlan_port_info));

    rc = bcmx_multicast_wlan_encap_get(l2addr->l2mc_index, port, port, &encap_id);
    if (rc == BCM_E_NONE)
    {
      rc = bcmx_wlan_port_get(port, &wlan_port_info);
      wlanPort = port;
      port = wlan_port_info.port;
    }
  }
  else
#endif /* INCLUDE_L3 */
  {
    encap_id = BCM_IF_INVALID;
    rc = BCM_E_NONE;
  }

  if (rc != BCM_E_NONE)
  {
    return rc;
  }

  return bcmx_multicast_egress_delete(l2addr->l2mc_index, port, encap_id);
}

/*********************************************************************
* @purpose  Remove a list of ports from a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_leave_ports (bcmx_mcast_addr_t *mcaddr)
{
  return bcmx_mcast_port_remove(mcaddr);
}

/*********************************************************************
* @purpose  Remove a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_multicast_leave_ports(bcmx_l2_addr_t *mcaddr, bcmx_lplist_t removePorts)
{
  int rc = BCM_E_NONE;
  int lpcount;
  bcmx_lport_t lpport;

  BCMX_LPLIST_ITER(removePorts, lpport, lpcount)
  {
    rc = usl_bcmx_multicast_egress_delete(mcaddr, lpport);
  }

  /* if we failed before the bcmx call, return code */
  return(rc);
}

/*********************************************************************
* @purpose  Sets the DVLAN MODE for all the ports 
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_mode_set(L7_uint32 mode,L7_ushort16 ethertype)
{
  int unit,port;
  int rv_final = BCM_E_NONE;
  int rv;

#ifdef BCM_ROBO_SUPPORT
  for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
#else
  for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++)
#endif
  {
    if (!SOC_IS_XGS_FABRIC(unit))
    {
      PBMP_E_ITER(unit,port)
      {
        rv = bcm_port_dtag_mode_set(unit, port, mode);
        if (rv < rv_final)
        {
          rv_final = rv;
        }
      }            
    }
  }

  return rv_final;
}

/*********************************************************************
* @purpose  Used to indicate to all units whether learns are allowed
*           on the specified trunk.
*
* @param    
* @param    
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_learn_mode_set(L7_uint32 tgid, L7_BOOL learningLocked)
{
    return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Create a protected port group 
*
* @param    protectedGroupList    @{(input)}  List of protected port
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
int usl_bcmx_protected_group_set(bcmx_lplist_t protectedGroupList)
{
  int              dest_unit, i = 0, modid = 0;
  int              tmprv, rv = BCM_E_NONE;
  protectedGroup_t group_list;
  bcm_pbmp_t       pbmp;

  memset(&group_list, 0, sizeof (protectedGroup_t));

  /* Build the pbmp of protected ports for every mod */
  BCMX_UNIT_ITER(dest_unit, i) 
  {
	BCMX_LPLIST_TO_PBMP(protectedGroupList, dest_unit, pbmp);
	tmprv = hapiBroadBcmUnitToModid(dest_unit, &modid);
	if (tmprv == BCM_E_NONE) 
	{
	  BCM_PBMP_ASSIGN(group_list.mod_pbmp[modid],pbmp);
	}
  }


#ifdef BCM_ROBO_SUPPORT
  for (dest_unit = 0; dest_unit < bde->num_devices(BDE_SWITCH_DEVICES); dest_unit++)
#else
  for (dest_unit = 0; dest_unit < bde->num_devices(BDE_ALL_DEVICES); dest_unit++)
#endif
  {
	if (!SOC_IS_XGS_FABRIC(dest_unit)) {
		  tmprv = l7_bcm_protected_group_set(dest_unit, &group_list);
		  if (tmprv < rv)
			  rv = tmprv;
	}
  }

  return rv;
}

/*********************************************************************
* @purpose  Create/Delete a list of vlans in the HW
*
* @param    bulkCmd          @{(input)} L7_TRUE: Create the Vlans
*                                       L7_FALSE: Delete the Vlans
* @param    vlanMask         @{(input)} Pointer to mask of vlan-ids to be created
* @param    numVlans         @{(input)} number of vlans to be created
* @param    vlanMaskFailure  @{(output)} Pointer to mask populated by usl with 
*                                       failed vlan-ids
* @param    vlanFailureCount @{(output)} Pointer to integer to pass the number 
*                                        of vlans that could not be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_vlan_bulk_configure(L7_BOOL bulkCmd,
                                 L7_VLAN_MASK_t *vlanMask, 
                                 L7_ushort16 numVlans,
                                 L7_VLAN_MASK_t *vlanMaskFailure,
                                 L7_uint32 *vlanFailureCount)
{
  L7_uint32 vidx, tmpVlanMask, pos = 0, vlanId, countVlans;
  int       rv = BCM_E_NONE, tmpRv;
  
  countVlans = numVlans;

  /*  Walk through the vlan mask and create/delete the vlans. If vlan 
  **  creation/deletion fails, then populate the vlanMaskFailure 
  **  for the vid.
  */
  for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
  {
    pos = 0;
	tmpVlanMask = vlanMask->value[vidx];
	while (tmpVlanMask) 
	{
	  pos++;
	  if (tmpVlanMask & 0x1) 
	  {
	    vlanId = (vidx*8) + pos;

        if (bulkCmd == L7_TRUE) 
        {
          tmpRv = bcmx_vlan_create(vlanId);
        }
        else
        {
          tmpRv = bcmx_vlan_destroy(vlanId);
        }

        /* Set the vlanMaskFailure if creation/deletion failed */
        if (tmpRv != BCM_E_NONE) 
        {
          (*vlanFailureCount)++;
          L7_VLAN_SETMASKBIT(*vlanMaskFailure, vlanId);
        }

        if (tmpRv < rv) 
          rv = tmpRv;

	    countVlans--;
	  }

	  tmpVlanMask >>= 1;
    }

	if (countVlans == 0) 
	{
      break;
    }
  }
  
  return rv;
}

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    mode    @{{input}}  enable=1,disable=0
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_flow_control_set(L7_uint32 unit, L7_uint32 mode,L7_uchar8 *mac)
{
   return hapiBroadFlowControlModeSet(mode,mac);
}


/*********************************************************************
* @purpose  Sets the DVLAN Translation for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable)
{
    int unit,port;
    int rv_final = BCM_E_NONE;
    int rv;

  #ifdef BCM_ROBO_SUPPORT
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
  #else
    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++)
  #endif
    {
       if (!SOC_IS_XGS_FABRIC(unit))
       {
         PBMP_E_ITER(unit,port)
         {
           if (direction == L7_TRUE)
           {
             /* Eabling the Ingress translation */
              rv = bcm_vlan_control_port_set(unit,port,
                                             bcmVlanTranslateIngressEnable,
                                             enable);
              if (rv < rv_final)
              {
                rv_final = rv;
              }
           }
           else if (direction == L7_FALSE)
           {
             /* Eabling the Egress translation */
             rv = bcm_vlan_control_port_set(unit,port,
                                            bcmVlanTranslateEgressEnable,
                                           enable);
             if (rv < rv_final)
             {
               rv_final = rv;
             }
           }
         }
       }
    }
    return rv_final;
}

/*********************************************************************
* @purpose  Sets the Default TPID for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_default_tpid_set(L7_ushort16 etherType)
{

    int unit,port;
    int rv_final = BCM_E_NONE;
    int rv;
    L7_ushort16 tpid = etherType;

  #ifdef BCM_ROBO_SUPPORT
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
  #else
    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++)
  #endif
    {
      if (!SOC_IS_XGS_FABRIC(unit))
      {
        PBMP_E_ITER(unit,port)
        {
          rv = bcm_port_tpid_set(unit,port,tpid);
          if (rv < rv_final)
          {
            rv_final = rv;
          }
          rv = _bcm_fb2_port_evc_tpid_index_set(unit,port,tpid);
          if (rv < rv_final)
          {
             rv_final = rv;
          }
        }
      }
    }
    return rv_final;
}
