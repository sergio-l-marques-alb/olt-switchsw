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
#include "bcmx/vlan.h"
#include "bcmx/stg.h"
#include "bcm_int/esw/mbcm.h"
#include "bcm_int/esw/draco.h"
#include "ibde.h"
#include <bcmx/bcmx_int.h>
#include "broad_common.h"

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

/* L2 forwarding database age time in seconds.
*/
L7_uint32 uslMacAgeTime = 300;


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
int usl_bcmx_l2_addr_remove_by_trunk (bcm_trunk_t tgid)
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


  memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
  l2addr_msg.tgid = tgid;
  l2addr_msg.port_is_lag = L7_TRUE;
  hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);

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
int usl_bcmx_l2_addr_remove_by_port (bcmx_lport_t lport)
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

  memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
  l2addr_msg.bcmx_lport = lport;
  l2addr_msg.port_is_lag = L7_FALSE;
  hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);


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
int usl_bcmx_l2_addr_remove_all ()
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
		  memset((void *)&l2addr_msg, 0, sizeof(l2addr_msg));
		  l2addr_msg.bcmx_lport = lport;
		  l2addr_msg.port_is_lag = L7_FALSE;
		  hapiBroadFlushL2LearnModeSet(l2addr_msg, L7_ENABLE);
	  }
  }

  return rc;
}


/*********************************************************************
* @purpose Synchronize all the dynamic L2 entries w/ the FDB application
*
* @param    none.
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_sync()
{
  return BCM_E_UNAVAIL;
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
* @purpose  Flush dynamic MAC addresses matching specified Mac.
*
* @param    vid - BCM mac identifier.
*
* @returns  BCM_E_NONE
*
* @note     Defined by the Broadcom driver.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_mac (bcm_mac_t mac)
{
  return bcmx_l2_addr_delete_by_mac(mac, 0);
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
* @purpose  Set bit in the uslTgidMask
*
* @param    
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void usl_trunk_learn_mode_set(L7_uint32 tgid, L7_uint32 learningLocked)
{
  return;
}


