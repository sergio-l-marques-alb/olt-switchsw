
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.c
*
* @purpose    USL BCM API Implementation
*
* @component  HAPI
*
* @comments   none
*
* @create     1/11/2009
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "broad_common.h"
#include "l7_usl_bcm_l2.h"
#include "l7_usl_l2_db.h"
#include "l7_usl_sm.h"

#include "ibde.h"
#include "bcm/port.h"
#include "bcm/stack.h"
#include "bcm/trunk.h"
#include "bcm/l2.h"
#include "bcm/mcast.h"
#include "bcm/vlan.h"
#include "bcm/stg.h"
#include "soc/drv.h"
#include "bcm_int/common/multicast.h" /* PTin modified: new SDK  (esw->common) */
#include "broad_mmu.h"

#include "ptin_globaldefs.h"

/* Check whether device supports enhanced DOS controls */
L7_BOOL usl_bcm_enhanced_doscontrol_get(void)
{ 
  static L7_BOOL dos_ctrl   = L7_FALSE;
  static L7_BOOL first_time = L7_TRUE;
  L7_uint8 i = 0;

  if (first_time == L7_TRUE)
  {
    first_time = L7_FALSE;
    /* Instead of checking whether device is raptor/fb2/raven/triumph,
     * query the local devices to see if the enhanced doc ctrl feature
     * is supported or not. This assumes identical devices are stacked.
     */
    for (i=0; i < soc_ndev; i++)
    {
      if (soc_feature(i, soc_feature_enhanced_dos_ctrl))
      {
        dos_ctrl = L7_TRUE;
        break;
      }
    }
  }
  
  return dos_ctrl;
}

/*********************************************************************
*
* @purpose  USL bcm function to remove a port from protected group
*
* @param   deleteInfo  @b{(input)}   Deleted protected port information
*
* @returns Defined by Broadcom driver
* @notes   none
*
* @end
*
*********************************************************************/
int usl_bcm_protected_group_port_remove(usl_bcm_protected_group_delete_t 
                                          *deleteInfo) 
{
  int                 i, modport;
  int                 rv = BCM_E_NONE;
  bcm_port_config_t   config;

  /* Check if hw should be updated */
  if (USL_BCM_CONFIGURE_HW(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i)) 
      {
        modport = (SOC_IS_TUCANA(i)) ? 
                   SOC_PORT_MOD_OFFSET(unit, deleteInfo->bcmPort) : deleteInfo->bcmPort; 

        rv = bcm_port_config_get(i, &config);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }

        rv = bcm_port_egress_set(i, modport, deleteInfo->modid, config.all);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE))
  {
    rv = usl_db_protected_group_port_remove(USL_CURRENT_DB, deleteInfo);    
  }
  
  return rv;
}


/*********************************************************************
*
* @purpose  USL bcm function to configure a group of protected ports
*
* @param   groupInfo  {(input)}   Pointer to list of protected ports
* @param   hwCommit   {(input)}   Commit the data to hardware
*
* @returns Defined by Broadcom driver
* @notes   none
*
* @end
*
*********************************************************************/
int usl_bcm_protected_group_set(usl_bcm_protected_group_t *groupInfo)
{
  L7_int32   my_modid = 0, mod, port, rv;
  L7_int32   unit;
  bcm_pbmp_t my_pbmp, tmp_pbmp;

  rv = BCM_E_NONE;


  if (USL_BCM_CONFIGURE_HW(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE)
  {
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
    {
      if (SOC_IS_XGS_FABRIC(unit)) 
        continue;

      rv = bcm_stk_my_modid_get(unit, &my_modid);
      if (L7_BCMX_OK(rv) != L7_TRUE) 
      {
	    break;
      }

      BCM_PBMP_CLEAR(tmp_pbmp);
      BCM_PBMP_CLEAR(my_pbmp);

      BCM_PBMP_ASSIGN(tmp_pbmp, groupInfo->mod_pbmp[my_modid]);
      BCM_PBMP_NEGATE(my_pbmp, tmp_pbmp);

      for (mod = 0; mod < L7_MOD_MAX; mod++) 
      {
        BCM_PBMP_CLEAR(tmp_pbmp);
	    BCM_PBMP_ASSIGN(tmp_pbmp, groupInfo->mod_pbmp[mod]);
	    if (BCM_PBMP_IS_NULL(tmp_pbmp))
	    {
	      continue;
	    }

	    BCM_PBMP_ITER(tmp_pbmp, port)
	    {
          if (SOC_IS_TUCANA(unit)) 
	      {
	        rv = bcm_port_egress_set(unit, SOC_PORT_MOD_OFFSET(unit, port), mod, my_pbmp);
          }
          else 
	      {
	        rv = bcm_port_egress_set(unit, port, mod, my_pbmp);
          }

	      if (L7_BCMX_OK(rv) != L7_TRUE) 
	      {
	        break;
	      }
	    }
      } /* End for mod */
    } /* End for all local units */
  } /* End if hwCommit */

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE))
  {
    rv =  usl_db_protected_group_set(USL_CURRENT_DB, groupInfo);
  }


  return rv;
}

/* Add cpu port to the vlan on one of the local units */
int usl_bcm_vlan_cpu_port_update(bcm_vlan_t vlan)
{
  int i, rv = BCM_E_NONE;
  bcm_pbmp_t upbmp;

  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    if (!SOC_IS_XGS_FABRIC(i))
    {
      BCM_PBMP_CLEAR(upbmp);
  
      rv = bcm_vlan_port_add(i, vlan, PBMP_CMIC(i), upbmp);

      /* Add cpu port on only one unit */
      if (rv == BCM_E_NONE)
      {
        break;    
      }
    }
  }

  return rv;
}
/*********************************************************************
* @purpose  Create or delete a vlan
*
* @params   vid    {(input)} vlan-id
* @params   create {(input)} L7_TRUE: Create the vlan
*                            L7_FALSE: Delete the vlan
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_update(bcm_vlan_t vid, L7_BOOL create)
{
  L7_int32          i, rv = BCM_E_NONE;
  USL_CMD_t  dbCmd;
  bcm_pbmp_t        upbmp;
  L7_BOOL           cpuPortAdded = L7_FALSE;

  if (create == L7_TRUE)
  {
    dbCmd = USL_CMD_ADD;    
  }
  else 
  {
    dbCmd = USL_CMD_REMOVE;        
  }
 
 
  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (create == L7_TRUE)
        {
          rv = bcm_vlan_create(i, vid);
          /* Ignore BCM_E_EXISTS for vlan 1*/
          if ((vid == 1) && (rv == BCM_E_EXISTS))
          {
            rv = BCM_E_NONE;
          }

          /* PTin added: Only add cpu ports for system vlans */
          if (vid==1 || vid>=4094)
          {
            /* On management unit add one cpu port to the vlan */
            if ((USL_BCM_CONFIGURE_DB(USL_L2_VLAN_DB_ID) == L7_FALSE) 
                && (cpuPortAdded == L7_FALSE))
            {
              cpuPortAdded = L7_TRUE;
              BCM_PBMP_CLEAR(upbmp);
              rv = bcm_vlan_port_add(i, vid, PBMP_CMIC(i), upbmp);
              if (L7_BCMX_OK(rv) != L7_TRUE)
                break;
            }
          } 
        }
        else 
        {
          rv = bcm_vlan_destroy(i, vid);
        }

        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  if (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_DB_ID) == L7_TRUE)
  {
    /* Update the USL table even if hw delete failed */
    if (create == L7_FALSE)
    {
      rv = usl_db_vlan_update(USL_CURRENT_DB, vid, dbCmd);
    }
    else  if (L7_BCMX_OK(rv) == L7_TRUE)
    {
      rv = usl_db_vlan_update(USL_CURRENT_DB, vid, dbCmd);
    }
  }

  return rv;
}    

/*********************************************************************
* @purpose  Set the mcast flood mode for a vlan
*
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcm_vlan_mcast_flood_set(bcm_vlan_t vid, 
                                 bcm_vlan_mcast_flood_t mcastFloodMode)
{
  int i, rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_vlan_mcast_flood_set(i, vid, mcastFloodMode);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_DB_ID) == L7_TRUE))
  {
    rv = usl_db_vlan_mcast_flood_set(USL_CURRENT_DB, vid, mcastFloodMode);
  }

  return rv;
}


/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} USL_CMD_ADD: Add the specified flags
*                                                to existing flags
*                                   USL_CMD_REMOVE: Remove the flags
*                                                   from the existing flags
*                                   USL_CMD_SET: Override the existing flags
*                                                with the specified flags.
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcm_vlan_control_flag_update(bcm_vlan_t vid, L7_uint32 flags,
                                     USL_CMD_t cmd)
{
  int                     i, rv = BCM_E_NONE;
  bcm_vlan_control_vlan_t control;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_vlan_control_vlan_get(i, vid, &control);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          if (rv == BCM_E_UNAVAIL)
          {
            rv = BCM_E_NONE;
            continue;    
          }
          else
          {
            break;    
          }
        }

        if (cmd == USL_CMD_ADD)
        {
          control.flags |= flags;    
        }
        else if (cmd == USL_CMD_REMOVE)
        {
          control.flags &= ~flags;
        }
        else if (cmd == USL_CMD_SET)
        {
          control.flags = flags;    
        }

        rv = bcm_vlan_control_vlan_set(i, vid, control);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_DB_ID) == L7_TRUE))
  {
    rv = usl_db_vlan_control_flag_update(USL_CURRENT_DB, vid, flags, cmd);
  }

  return rv;

}

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    vid                    @{(input)} 
* @param    forwarding_mode        @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcm_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode)
{
  int                     i, rv = BCM_E_NONE;
  bcm_vlan_control_vlan_t control;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_vlan_control_vlan_get(i, vid, &control);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          if (rv == BCM_E_UNAVAIL)
          {
            rv = BCM_E_NONE;
            continue;    
          }
          else
          {
            break;    
          }
        }

        control.forwarding_mode = forwarding_mode;

        rv = bcm_vlan_control_vlan_set(i, vid, control);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_DB_ID) == L7_TRUE))
  {
    rv = usl_db_vlan_control_fwd_mode_set(USL_CURRENT_DB, vid, forwarding_mode);
  }

  return rv;

}

/*********************************************************************
* @purpose  Sets hashing mechanism for existing trunk
*
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_trunk_psc_set(L7_uint32 appId, bcm_trunk_t tid, int psc)
{
  int i, rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_trunk_psc_set(i, tid, psc);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_TRUNK_DB_ID) == L7_TRUE))
  {
    rv = usl_db_trunk_hash_set(USL_CURRENT_DB, appId, tid, psc);
  }
  
  return rv;
}

/*********************************************************************
* @purpose  Create a trunk
*
* @param    appId   @{(input)} Application identifier for the trunk
* @param    flags   @{(input)}   Trunk flags 
* @param    tid     @{(input/output)} Tid
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_create(L7_uint32 appId, L7_uint32 flags, bcm_trunk_t *tid)
{
  int rv = BCM_E_NONE, i;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (flags & USL_BCM_TRUNK_CREATE_WITH_ID)
        {
          /* PTin modified: SDK 6.3.0 */
          #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
          rv = bcm_trunk_create(i, BCM_TRUNK_FLAG_WITH_ID, tid);
          #else
          rv = bcm_trunk_create_id(i, *tid);
          #endif
        }
        else
        {
          /* PTin modified: SDK 6.3.0 */
          #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
          rv = bcm_trunk_create(i, 0, tid);
          #else
          rv = bcm_trunk_create(i, tid);
          #endif
        }
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_TRUNK_DB_ID) == L7_TRUE))
  {
    rv = usl_db_create_trunk(USL_CURRENT_DB, appId, *tid);
  }
  
  return rv;
}

/*********************************************************************
* @purpose  Delete a trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid)
{
  int         i, rv = BCM_E_NONE, tmpRv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_trunk_destroy(i, tid);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db  */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_TRUNK_DB_ID) == L7_TRUE))
  {
    tmpRv = usl_db_delete_trunk(USL_CURRENT_DB, appId, tid);
  }

  rv = min(rv, tmpRv);
  
  return rv;
}


/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                      bcm_trunk_add_info_t * addInfo)
{
  int                  i, rv = BCM_E_NONE;

  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_trunk_info_t trunk_info;
  int member_count, count;
  bcm_trunk_member_t member_array[BCM_TRUNK_MAX_PORTCNT];
  #endif

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        memset(&trunk_info ,0, sizeof(trunk_info));
        memset(member_array,0, sizeof(member_array));
        trunk_info.flags        = addInfo->flags;
        trunk_info.psc          = addInfo->psc;
        trunk_info.ipmc_psc     = addInfo->ipmc_psc;
        trunk_info.dlf_index    = addInfo->dlf_index;
        trunk_info.mc_index     = addInfo->mc_index;
        trunk_info.ipmc_index   = addInfo->ipmc_index;
        trunk_info.dynamic_size = addInfo->dynamic_size;
        trunk_info.dynamic_age  = addInfo->dynamic_age;
        trunk_info.dynamic_load_exponent          = addInfo->dynamic_load_exponent;
        trunk_info.dynamic_expected_load_exponent = addInfo->dynamic_expected_load_exponent;
        member_count = addInfo->num_ports;
        for (count=0; count<member_count && count<BCM_TRUNK_MAX_PORTCNT; count++)
        {
          BCM_GPORT_LOCAL_SET(member_array[count].gport, addInfo->tp[count]);
        }
        rv = bcm_trunk_set(i, tid, &trunk_info, member_count, member_array);
        #else
        rv = bcm_trunk_set(i, tid, addInfo); 
        #endif

        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_TRUNK_DB_ID) == L7_TRUE))
  {
    rv = usl_db_update_trunk_info(USL_CURRENT_DB, appId, tid, addInfo); 
  }

  return rv;  
}


/*********************************************************************
* @purpose  Set the learning mode for trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    learnMode    @{(input)} 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_trunk_learn_mode_set(L7_uint32 appId, bcm_trunk_t tid, 
                                 L7_BOOL learnMode)
{
  
  usl_trunk_learn_mode_set(tid, learnMode);

  usl_db_trunk_learn_mode_set(USL_CURRENT_DB, appId, tid, learnMode);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  USL BCM API to create Stg in hardware
*
*
* @param appId @{(input)} Unique application identifier for 
*                         this stg
* @param stg @{(input/output)} Pointer the Spanning Tree Group
* @param flags @{(input)} USL_BCM stg flag
*
*
* @param    flags       
* @returns  Defined by the Broadcom driver
*
* @end
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_stg_create(L7_uint32 appId, L7_uint32 flags, L7_int32 *stg)
{
  int rv = BCM_E_NONE, i;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (flags & USL_BCM_STG_CREATE_WITH_ID)
        {
          rv = bcm_stg_create_id(i, *stg);    
        }
        else
        {
          rv = bcm_stg_create(i, stg);
        }

        if (L7_BCMX_OK(rv) != L7_TRUE)
          break; 
      }
    }
  }
  
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_STG_DB_ID) == L7_TRUE))
  {
    rv = usl_db_create_stg(USL_CURRENT_DB, appId, *stg);
  }
 
  return rv; 
}



/*********************************************************************
* @purpose Destroy a spanning-tree group
*
* @param    appInstId  @{(input)} Application Inst Id
* @param    stg        @{(input)} Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_stg_destroy(L7_uint32 appInstId, bcm_stg_t stgId)
{
  int         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  int         i;

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_stg_destroy(i, stgId);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          rv = tmpRv;
      }
    }
  }


  /* Update the USL Db  */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_STG_DB_ID) == L7_TRUE))
  {
    tmpRv = usl_db_destroy_stg(USL_CURRENT_DB, appInstId, stgId);
  }

  /* return the worst error code */
  rv = min(rv, tmpRv);
  
  return rv;
}

/*********************************************************************
* @purpose  Add/Remove a VLAN to/from a Spanning-tree Group
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
* @param    cmd          @{(input)} Add or Remove vlan
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_stg_vlan_update(L7_uint32 appInstId,
                            bcm_stg_t stgId, 
                            bcm_vlan_t vid, 
                            USL_CMD_t cmd)
{
  int i, rv = BCM_E_NONE;

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (cmd == USL_CMD_ADD)
        {
          rv = bcm_stg_vlan_add(i, stgId, vid);
        }
        else if (cmd == USL_CMD_REMOVE)
        {
          rv = bcm_stg_vlan_remove(i, stgId, vid);
        }
        else
        {
          rv = BCM_E_FAIL;
        }

        if (L7_BCMX_OK(rv) != L7_TRUE)
         break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_STG_DB_ID) == L7_TRUE))
  {
    rv = usl_db_stg_vlan_update(USL_CURRENT_DB, appInstId, stgId, vid, cmd);
  }
  
  return rv;
}


/*********************************************************************
* @purpose  Adds the Ip Subnet Vlan entry to the  HW table
*
* @param    ipSubnetData @{(input)} IP Subnet vlan data
*
* @returns  BCM Return code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_ip4_add(usl_bcm_vlan_ipsubnet_t *ipSubnetData)
{
  int i, rv = BCM_E_NONE;
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_vlan_ip_t vlan_ip;
  #endif

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        vlan_ip.flags = 0;    /* BCM_VLAN_SUBNET_IP6 for IPv6 */
        vlan_ip.vid   = ipSubnetData->vlanId;
        vlan_ip.ip4   = ipSubnetData->ipSubnet;
        vlan_ip.mask  = ipSubnetData->netMask;
        memset(vlan_ip.ip6, 0x00, sizeof(bcm_ip6_t));
        vlan_ip.prefix= 0;
        vlan_ip.prio  = ipSubnetData->prio;

        rv = bcm_vlan_ip_add(i, &vlan_ip);

        #else

        rv = bcm_vlan_ip4_add(i, ipSubnetData->ipSubnet, ipSubnetData->netMask,
                              ipSubnetData->vlanId, ipSubnetData->prio);
        #endif
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE))
  {
    rv = usl_db_vlan_ip4_add(USL_CURRENT_DB, ipSubnetData->ipSubnet, 
                             ipSubnetData->netMask, ipSubnetData->vlanId, 
                             ipSubnetData->prio);
  }
  
  return rv;
}


/*********************************************************************
* @purpose  Deletes the Ip Subnet Vlan entry from the HW table
*
* @param    ipSubnetData @{(input)} IP Subnet vlan data
*
* @returns  BCM Return code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_vlan_ip4_delete(usl_bcm_vlan_ipsubnet_t *ipSubnetData)
{
  int                        rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_int32                   i;
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_vlan_ip_t vlan_ip;
  #endif

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        vlan_ip.flags = 0;    /* BCM_VLAN_SUBNET_IP6 for IPv6 */
        vlan_ip.vid   = ipSubnetData->vlanId;
        vlan_ip.ip4   = ipSubnetData->ipSubnet;
        vlan_ip.mask  = ipSubnetData->netMask;
        memset(vlan_ip.ip6, 0x00, sizeof(bcm_ip6_t));
        vlan_ip.prefix= 0;
        vlan_ip.prio  = ipSubnetData->prio;

        rv = bcm_vlan_ip_delete(i, &vlan_ip);

        #else
        tmpRv = bcm_vlan_ip4_delete(i, ipSubnetData->ipSubnet, ipSubnetData->netMask);
        #endif
        if (L7_BCMX_OK(tmpRv) != L7_TRUE)
        {
          break;    
        }
      }
    }
  }

  /* Update the USL Db even if hw delete failed */ 
  if (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE)
  {
    rv = usl_db_vlan_ip4_delete(USL_CURRENT_DB, ipSubnetData->ipSubnet,
                                ipSubnetData->netMask);
  }

  rv = min(tmpRv, rv);
  
  return rv;
}


/*********************************************************************
* @purpose  Adds the Vlan mac entry to the HW 

* @param   macData  @b{(input)} Mac vlan data
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_mac_add(usl_bcm_vlan_mac_t *macData)
{
  int rv = BCM_E_NONE, i;

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {

        rv = bcm_vlan_mac_add(i, macData->mac, macData->vlanId, macData->prio);
                                  
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE))
  {
    rv = usl_db_vlan_mac_add(USL_CURRENT_DB, macData->mac, macData->vlanId, 
                             macData->prio);
  }

    
  return rv;
}

/*********************************************************************
* @purpose  Deletes the Vlan mac entry from the HW 

* @param   macData  @b{(input)} Mac vlan data
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_mac_delete(usl_bcm_vlan_mac_t *macData)
{
  int                         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_uint32                   i;

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {

        tmpRv = bcm_vlan_mac_delete(i, macData->mac);
                                  
        if (L7_BCMX_OK(tmpRv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db even if hw delete failed */ 
  if (USL_BCM_CONFIGURE_DB(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE)
  {
    rv = usl_db_vlan_mac_delete(USL_CURRENT_DB, macData->mac);
  }

  rv = min(rv, tmpRv);
  
  return rv;
}

/*********************************************************************
* @purpose  Convert from USL BCM to SDK BCM format
*
* @param    mcAddr  @{(input)} USL BCM address
* @param    bcmUnit @{(input)} Bcm unit number
* @param    bcmAddr @{(output)} SDK BCM Mcast address
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int l7_l2mcast_to_bcm(usl_bcm_mcast_addr_t *mcAddr,
                      int                   bcmUnit,
                      bcm_mcast_addr_t     *bcmAddr)
{
  int rv = BCM_E_NONE, myModid;

  memset(bcmAddr, 0, sizeof(*bcmAddr));

  rv = bcm_stk_my_modid_get(bcmUnit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  memcpy(bcmAddr->mac, mcAddr->mac, sizeof(bcm_mac_t));
  memcpy(&(bcmAddr->vid), &(mcAddr->vid), sizeof(mcAddr->vid));

  BCM_PBMP_ASSIGN(bcmAddr->pbmp, mcAddr->mod_pbmp[myModid]);
  BCM_PBMP_ASSIGN(bcmAddr->ubmp, mcAddr->mod_ubmp[myModid]);
  BCM_PBMP_OR(bcmAddr->pbmp,PBMP_HG_ALL(bcmUnit));
  BCM_PBMP_OR(bcmAddr->pbmp,PBMP_HL_ALL(bcmUnit));

  bcmAddr->l2mc_index = mcAddr->l2mc_index;

  return rv;
}

/*********************************************************************
* @purpose  Convert from USL BCM to SDK BCM format
*
* @param    mcAddr  @{(input)} USL BCM address
* @param    bcmUnit @{(input)} Bcm unit number
* @param    bcmAddr @{(output)} SDK BCM l2 address
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int l7_l2mcast_to_bcm_l2addr(usl_bcm_mcast_addr_t *mcAddr,
                             int                   bcmUnit,
                             bcm_l2_addr_t        *bcmAddr)
{
  int rv = BCM_E_NONE;

  memset(bcmAddr, 0, sizeof(*bcmAddr));

  memcpy(bcmAddr->mac, mcAddr->mac, sizeof(bcm_mac_t));
  memcpy(&(bcmAddr->vid), &(mcAddr->vid), sizeof(mcAddr->vid));
  
  bcmAddr->flags |= (BCM_L2_STATIC | BCM_L2_MCAST | BCM_L2_REPLACE_DYNAMIC);
  bcmAddr->l2mc_index = mcAddr->l2mc_index;

  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to add L2 multicast addresses
*
* @param    mcAddr  @{(input)} Mcast Group Info
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int usl_bcm_mcast_addr_add(usl_bcm_mcast_addr_t *mcAddr)
{
  int                 rv = BCM_E_NONE;
  int                 bcm_unit, port;
  bcm_mcast_addr_t    bcm_mc;
  bcm_l2_addr_t       bcm_l2;
  bcm_pbmp_t          pbmp;
  L7_uint32           flags, myModid, encap_id;
  bcmx_lport_t        gport;
 
  

  if (USL_BCM_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
  {
    /* Add the mcast entry to any non-fabric switches that
     * do not already have it 
     */

    for (bcm_unit=0; bcm_unit< bde->num_devices(BDE_SWITCH_DEVICES); bcm_unit++)
    {
      if (SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }

      rv = l7_l2mcast_to_bcm(mcAddr, bcm_unit,  &bcm_mc);
      if (rv != BCM_E_NONE)
      {
        break;    
      }
      rv = l7_l2mcast_to_bcm_l2addr(mcAddr, bcm_unit,  &bcm_l2);
      if (rv != BCM_E_NONE)
      {
        break;    
      }

      flags = BCM_MULTICAST_WITH_ID;
      if (_BCM_MULTICAST_IS_WLAN(bcm_l2.l2mc_index))
      {
        flags |= BCM_MULTICAST_TYPE_WLAN;
      }
      else
      {
        flags |= BCM_MULTICAST_TYPE_L2;
      }

      /* create the group */
      rv = bcm_multicast_create(bcm_unit, flags, &bcm_l2.l2mc_index);
      if (L7_BCMX_OK(rv) != L7_TRUE) 
      {
        break;    
      }

      /* add the l2 entry */
      rv = bcm_l2_addr_add(bcm_unit, &bcm_l2);
      if (L7_BCMX_OK(rv) != L7_TRUE) 
      {
        break;    
      }

      rv = bcm_stk_my_modid_get(bcm_unit, &myModid);
      if (rv != BCM_E_NONE)
      {
        return rv;
      }

      /* add any ports initially, like higig */
      BCM_PBMP_ITER(bcm_mc.pbmp, port)
      {
        BCM_GPORT_MODPORT_SET(gport, myModid, port);
        rv = bcm_multicast_egress_add(bcm_unit, mcAddr->l2mc_index, gport, BCM_IF_INVALID);
      }

      BROAD_WLAN_PBMP_ITER(mcAddr->wlan_pbmp, port)
      {
        BROAD_WLAN_ID_TO_GPORT(gport, port);
        if ((rv = bcm_multicast_wlan_encap_get(bcm_unit, mcAddr->l2mc_index, gport, gport, &encap_id)) == BCM_E_NONE)
        {
          rv = bcm_multicast_egress_add(bcm_unit, mcAddr->l2mc_index, gport, encap_id);
        }
      }

    }

    /* Add the mcast entry to fabric switches */
    for (bcm_unit=0; ((bcm_unit<soc_ndev) && (L7_BCMX_OK(rv) == L7_TRUE)); 
         bcm_unit++)
    {   
      if (!SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }
      BCM_PBMP_CLEAR(pbmp);
      BCM_PBMP_OR(pbmp,PBMP_HG_ALL(bcm_unit));
      BCM_PBMP_ITER(pbmp,port) 
      {
        rv = bcm_mcast_bitmap_set(bcm_unit, bcm_mc.l2mc_index, port, pbmp);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  } /* End if USL_BCM_CONFIGURE_HW */

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_MCAST_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l2mc_addr_update(USL_CURRENT_DB, mcAddr, USL_CMD_ADD);  
  }

  return rv;
}    

/*********************************************************************
* @purpose  USL BCM function to remove L2 multicast address 
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int usl_bcm_mcast_addr_remove(usl_bcm_mcast_addr_t *mcAddr)
{
  int                 rv = BCM_E_NONE, tmp_rv;
  int                 bcm_unit, mcindex, port;
  bcm_mcast_addr_t    bcm_mca;
  bcm_pbmp_t          pbmp,empty;

  mcindex = -1;
  if (USL_BCM_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0; bcm_unit< bde->num_devices(BDE_SWITCH_DEVICES); bcm_unit++)
    {
      if (SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }
      if (mcindex < 0) 
      {
        /* get mcast index if available */
        tmp_rv = bcm_mcast_port_get(bcm_unit, mcAddr->mac, mcAddr->vid, &bcm_mca);
        if (tmp_rv >= 0) 
        {
          mcindex = bcm_mca.l2mc_index;
        }
      }

      tmp_rv = bcm_l2_addr_delete(bcm_unit, mcAddr->mac, mcAddr->vid);
      if ((tmp_rv != BCM_E_NONE) && (tmp_rv != BCM_E_UNAVAIL))
      {
        rv = tmp_rv;    
      }
      tmp_rv = bcm_multicast_destroy(bcm_unit, mcAddr->l2mc_index);
      if ((tmp_rv != BCM_E_NONE) && (tmp_rv != BCM_E_UNAVAIL))
      {
        rv = tmp_rv;    
      }
    }


    if (mcindex >= 0) 
    {
      /* Delete mcast index from fabric */
      for (bcm_unit=0; bcm_unit<soc_ndev; bcm_unit++)
      {
        if (!SOC_IS_XGS_FABRIC(bcm_unit))
        {
          continue;
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(empty);
        BCM_PBMP_OR(pbmp,PBMP_HG_ALL(bcm_unit));
        BCM_PBMP_ITER(pbmp, port)
        {
          tmp_rv = bcm_mcast_bitmap_set(bcm_unit, mcindex, port, empty);
          if ((tmp_rv != BCM_E_NONE) && (tmp_rv != BCM_E_UNAVAIL))
          {
            rv = tmp_rv;    
          }
        }
      }
    }
  } /* End if USL_BCM_CONFIGURE_HW */
 
  /* Update the USL Db  */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_MCAST_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l2mc_addr_update(USL_CURRENT_DB, mcAddr, USL_CMD_REMOVE);  
  }

  return rv;
}

/*********************************************************************
* @purpose Handle L2MC Address Port Add/Remove commands
*
* @param    transaction_id - transaction id
* @param    buf - buffer pointer to egress object information
* @param    buf_len - buffer length
* @param    status - return code 
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_mcast_ports_add_remove(usl_bcm_mcast_addr_t *mcAddr,
                                   USL_CMD_t     updateCmd)
{
  L7_int32             bcmUnit;
  L7_int32             rv = BCM_E_NONE;
  bcm_gport_t          gport;
  L7_uint32            port;
  L7_int32             myModid;
  L7_int32             encap_id;

  if (USL_BCM_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
  {
    for (bcmUnit=0; bcmUnit< bde->num_devices(BDE_SWITCH_DEVICES); bcmUnit++)
    {
      if (SOC_IS_XGS_FABRIC(bcmUnit))
      {
        continue;
      }

      rv = bcm_stk_my_modid_get(bcmUnit, &myModid);
      if (rv != BCM_E_NONE)
      {
        break;    
      }

      if (updateCmd == USL_CMD_ADD)
      {
        BCM_PBMP_ITER(mcAddr->mod_pbmp[myModid], port)
        {
          BCM_GPORT_MODPORT_SET(gport, myModid, port);
          rv = bcm_multicast_egress_add(bcmUnit, mcAddr->l2mc_index, gport, BCM_IF_INVALID);
        }
        BROAD_WLAN_PBMP_ITER(mcAddr->wlan_pbmp, port)
        {
          BROAD_WLAN_ID_TO_GPORT(gport, port);
          if ((rv = bcm_multicast_wlan_encap_get(bcmUnit, mcAddr->l2mc_index, gport, gport, &encap_id)) == BCM_E_NONE)
          {
            rv = bcm_multicast_egress_add(bcmUnit, mcAddr->l2mc_index, gport, encap_id);
          }
        }
      }
      else if (updateCmd == USL_CMD_REMOVE)
      {
        BCM_PBMP_ITER(mcAddr->mod_pbmp[myModid], port)
        {
          BCM_GPORT_MODPORT_SET(gport, myModid, port);
          rv = bcm_multicast_egress_delete(bcmUnit, mcAddr->l2mc_index, gport, BCM_IF_INVALID);
        }
        BROAD_WLAN_PBMP_ITER(mcAddr->wlan_pbmp, port)
        {
          BROAD_WLAN_ID_TO_GPORT(gport, port);
          if ((rv = bcm_multicast_wlan_encap_get(bcmUnit, mcAddr->l2mc_index, gport, gport, &encap_id)) == BCM_E_NONE)
          {
            rv = bcm_multicast_egress_delete(bcmUnit, mcAddr->l2mc_index, gport, encap_id);
          }
        }
      }
      else
      {
        rv = BCM_E_PARAM;
      }

      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break; 
      }
    }
  } /* End if USL_BCM_CONFIGURE_HW */

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_MCAST_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l2mc_update_ports(USL_CURRENT_DB, mcAddr, updateCmd);
  }

  return rv;
}

/*********************************************************************
* @purpose  Handle L2MC Address Port Add/Remove commands
*
* @param    unit         - bcm unit
* @param    port         - bcm port
* @param    l2mc_index[] - array of L2MC indices
* @param    l2mc_index   - count of L2MC indices 
* @param    updateCmd    - add/remove
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_port_mcast_groups_update(int unit, bcm_port_t port, L7_uint32 *l2mc_index, L7_uint32 l2mc_index_count, USL_CMD_t updateCmd)
{
  L7_int32             rv = BCM_E_NONE;
  bcm_gport_t          gport;
  L7_int32             i;

  rv = bcm_port_gport_get(unit, port, &gport);
  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  if (USL_BCM_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < l2mc_index_count; i++)
    {
      if (updateCmd == USL_CMD_ADD)
      {
        rv = bcm_multicast_egress_add(unit, l2mc_index[i], gport, BCM_IF_INVALID);
      }
      else if (updateCmd == USL_CMD_REMOVE)
      {
        rv = bcm_multicast_egress_delete(unit, l2mc_index[i], gport, BCM_IF_INVALID);
      }
      else
      {
        rv = BCM_E_FAIL;
        break;
      }
    }
  } /* End if USL_BCM_CONFIGURE_HW */

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L2_MCAST_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l2mc_port_update_groups(USL_CURRENT_DB, gport, l2mc_index, l2mc_index_count, updateCmd);
  }

  return rv;
}

/*********************************************************************
* @purpose Set the flow control mode
*
* @param    mode {(input)} flow-control mode
* @param    mac  {(input)} System mac
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_flow_control_set(L7_uint32 mode, L7_uchar8 *mac)
{
  int rv = BCM_E_NONE;

  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    rv = hapiBroadFlowControlModeSet(mode, mac);
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_flow_control_set(USL_CURRENT_DB, mode, mac);      
  }

  return rv;
}

/*********************************************************************
* @purpose Set the DOS control mode
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
int usl_bcm_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg)
{
  int                  rv = BCM_E_NONE;
  L7_int32             bcmUnit;
  bcm_switch_control_t dosType;

  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    for (bcmUnit = 0; bcmUnit < bde->num_devices(BDE_SWITCH_DEVICES); bcmUnit++)
    {
      switch (type)
      {
      case DOSCONTROL_SIPDIP:
        dosType = bcmSwitchDosAttackSipEqualDip;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_SMACDMAC:
        dosType = bcmSwitchDosAttackMACSAEqualMACDA;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_FIRSTFRAG:
        if (usl_bcm_enhanced_doscontrol_get() == L7_TRUE)
        {
          dosType = bcmSwitchDosAttackTcpHdrPartial;
        }
        else
        {
          dosType = bcmSwitchDosAttackV4FirstFrag;
        }   
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        dosType = bcmSwitchDosAttackMinTcpHdrSize;
        rv = bcm_switch_control_set(bcmUnit, dosType, arg);
        break;

      case DOSCONTROL_TCPFRAG:
        if (usl_bcm_enhanced_doscontrol_get() == L7_TRUE)
        {
          /* FB2 has enhanced DOS control support */
          dosType = bcmSwitchDosAttackTcpOffset;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }

          dosType = bcmSwitchDosAttackTcpHdrPartial;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        else
        {
          dosType = bcmSwitchDosAttackTcpFrag;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        break;

      case DOSCONTROL_TCPOFFSET:
        dosType = bcmSwitchDosAttackTcpOffset;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_TCPFLAG:
        if (usl_bcm_enhanced_doscontrol_get() == L7_TRUE)
        {
          dosType = bcmSwitchDosAttackFlagZeroSeqZero;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }

          dosType = bcmSwitchDosAttackTcpFlagsSF;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }

          dosType = bcmSwitchDosAttackSynFrag;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }

          dosType = bcmSwitchDosAttackTcpFlagsFUP;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        else
        {
          dosType = bcmSwitchDosAttackTcpFlags;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        break;

      case DOSCONTROL_TCPFLAGSEQ:
        dosType = bcmSwitchDosAttackFlagZeroSeqZero;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_TCPPORT:
        dosType = bcmSwitchDosAttackTcpPortsEqual;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_UDPPORT:
        dosType = bcmSwitchDosAttackUdpPortsEqual;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_TCPSYN:
        dosType = bcmSwitchDosAttackSynFrag;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        dosType = bcmSwitchDosAttackV4FirstFrag;

        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_TCPSYNFIN:
        dosType = bcmSwitchDosAttackTcpFlagsSF;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_TCPFINURGPSH:
        dosType = bcmSwitchDosAttackTcpFlagsFUP;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      case DOSCONTROL_L4PORT:
        if (usl_bcm_enhanced_doscontrol_get() == L7_TRUE)
        {
          dosType = bcmSwitchDosAttackUdpPortsEqual;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }

          dosType = bcmSwitchDosAttackTcpPortsEqual;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        else
        {
          dosType = bcmSwitchDosAttackL4Port;
          rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        }
        break;

      case DOSCONTROL_ICMP:
        dosType = bcmSwitchDosAttackIcmp;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        dosType = bcmSwitchDosAttackIcmpPktOversize;
        rv = bcm_switch_control_set(bcmUnit, dosType, arg);
        break;

      case DOSCONTROL_ICMPV6:
        dosType = bcmSwitchDosAttackIcmp;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        dosType = bcmSwitchDosAttackIcmpV6PingSize;
        rv = bcm_switch_control_set(bcmUnit, dosType, arg);
        break;

      case DOSCONTROL_ICMPFRAG:
        dosType = bcmSwitchDosAttackIcmpFragments;
        rv = bcm_switch_control_set(bcmUnit, dosType, mode);
        break;

      default:
        /* Other types may be handled by field processor policies... ignore these here. */
        break;
      }
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_doscontrol_set(USL_CURRENT_DB, type, mode, arg);
  }

  return rv;
}

/*********************************************************************
* @purpose  Custom RPC function to handle modifications to the
*           source interface when processing RPF failures.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_enable_set (L7_uint32 enable)
{
  L7_uint32            bcm_unit;
  int                  rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
    {
      if (SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }
   
      rv = bcm_ipmc_enable(bcm_unit, enable);
      if (L7_BCMX_OK(rv) != L7_TRUE)
        break;
    }
  }
   
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_mode_set(USL_CURRENT_DB, enable);
  }
 
  return rv;
}

/*********************************************************************
* @purpose  Sets the DVLAN mode for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_mode_set(L7_uint32 mode)
{
  L7_uint32            bcm_unit, port;
  int                  rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
    {
      if (SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }
   
      PBMP_E_ITER(bcm_unit, port)
      {
        rv = bcm_port_dtag_mode_set(bcm_unit, port, mode);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }
   
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_dvlan_mode_set(USL_CURRENT_DB, mode);
  }
 
  return rv;
}

/*********************************************************************
* @purpose  Sets the DVLAN TPID for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_tpid_set(L7_ushort16 tpid)
{
  L7_uint32            bcm_unit, port;
  int                  rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    for (bcm_unit = 0; bcm_unit < soc_ndev; bcm_unit++)
    {
      if (SOC_IS_XGS_FABRIC(bcm_unit))
      {
        continue;
      }
   
      PBMP_E_ITER(bcm_unit, port)
      {
        rv = bcm_port_tpid_set(bcm_unit, port, tpid);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }
   
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_dvlan_tpid_set(USL_CURRENT_DB, tpid);
  }
 
  return rv;
}

/*********************************************************************
* @purpose  Sets the DVLAN translation for all ports
*
* @param    
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcm_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable)
{
  L7_uint32            unit, port;
  int                  rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
    {
      if (!SOC_IS_XGS_FABRIC(unit))
      {
        PBMP_E_ITER(unit, port)
        {
          if (direction == L7_TRUE)
          {
            /* Enabling the Ingress translation */
            rv = bcm_vlan_control_port_set(unit, port,
                                           bcmVlanTranslateIngressEnable,
                                           enable);
            if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
              break;
          }
          else if (direction == L7_FALSE)
          {
            /* Enabling the Egress translation */
            rv = bcm_vlan_control_port_set(unit, port,
                                           bcmVlanTranslateEgressEnable,
                                           enable);
            if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
              break;

            /* Enabling the drop FLAG on EVT miss */
            rv = bcm_vlan_control_port_set(unit, port,
                                           bcmVlanTranslateEgressMissDrop,
                                           enable);
            if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
              break;
          }
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_dvlan_translation_set(USL_CURRENT_DB, direction, enable);
  }

  return rv;
}


/*********************************************************************
* @purpose  Custom RPC function to handle setting the dropmode (ingress/egress)
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_mmu_dropmode_set (L7_uint32 mode)
{
  int                  rv = BCM_E_NONE;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
  {
    rv = hapiBroadMmuDropModeSet(mode);
  }
   
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_SYSTEM_DB_ID) == L7_TRUE))
  {
    rv = usl_db_mmu_dropmode_set(USL_CURRENT_DB, mode);
  }
 
  return rv;
}
