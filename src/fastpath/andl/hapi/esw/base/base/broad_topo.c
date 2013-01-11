/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_topo.c
*
* @purpose   This file is for FASTPATH generic topology code
*
* @component hapi
*
* @comments
*
* @create    6/1/2008
*
* @author    mbaucom
*
* @end
*
**********************************************************************/

#include <broad_topo.h>
#include <sysbrds.h>
#include <bcm/types.h>
#include <bcm/error.h>

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/rpc/rpc.h>
#endif /* BCM_RPC_SUPPORT */

#include <appl/stktask/topo_brd.h>
#include <soc/drv.h>
#include <bcm_int/control.h>
#include <dapi_trace.h>
#include <sal/appl/config.h>

/* Trunk-id for the internal trunk used on dual xgs3 design. SDK stack trunk
 * handling code will avoid destroying this trunk-id when creating stack trunks
 */
L7_int32 lvl7_internal_hg_trunkid = -1;

/* by default, no customer topo needs to be registered */
static cust_topo_brd_f customer_topo_board_program = L7_NULLPTR;

/* Returns output stack port on src_unit to get to dest_modid 
   Customized to work with a 2 switch back to back configuration */
static int _lvl7_board_topomap_48g_stk(int src_unit, int dest_modid,
                                            bcm_port_t *exit_port)
{
    int iter, rv;
    const bcm_sys_board_t       *board_info;

    board_info = hpcBoardGet();

    if (!board_info) return BCM_E_FAIL;

    switch (src_unit) {
    case 0:
    case 1:
      /* check for local connections */
      for (iter = 0; iter < board_info->num_interconnects;iter++) 
      {
        if(board_info->interconnect_list[iter].to_unit != src_unit)
        {
          /* Step 1: Check if we are trying to reach a local unit. In which case, the macro sets
           * the exit port to the port specified in the connection list to reach that unit and
           * returns directly */
          BCM_BOARD_MOD_CHECK(board_info->interconnect_list[iter].to_unit, board_info->interconnect_list[iter].from_port, dest_modid, exit_port);
        }
      }

      /* Step 2: We are trying to reach a remote mod_id. So check if there is a direct route 
       * out of the local external port(s) on this unit */
      rv = bcm_board_topomap_stk(src_unit, dest_modid, exit_port);

      /* Step 3: We are trying to reach a remote mod_id and there is no direct route from this
       * unit. Assuming that we are dealing with a 2 unit b2b configuration, there must be a valid
       * route out of the other local unit to get to the specified mod_id. So, look in the 
       * connection list for an entry to go out from the current unit and set the exit port
       * from there */
      if(rv == BCM_E_NOT_FOUND)
      {
        for (iter = 0; iter < board_info->num_interconnects;iter++) 
        {
          if(board_info->interconnect_list[iter].from_unit == src_unit)
          {
            *exit_port = board_info->interconnect_list[iter].from_port;
            return BCM_E_NONE;
          }
        }
      }

      return rv;

    default:
        return BCM_E_UNIT;
    }
}

/*
 * Use the data in the connList and topology db to setup the mod maps and fabric trunks
 */
#define FB_UNIT0 0
#define FB_UNIT1 1
static int lvl7_48g_topo(topo_cpu_t *tp_cpu,cpudb_ref_t db_ref)
{
    cpudb_unit_port_t *stk_port;
    int sp_idx, m_idx;
    int mod_id;
    int src_unit,dst_unit;
    cpudb_entry_t *l_entry;
    int iter;
    bcm_trunk_add_info_t        trunk;
    bcm_trunk_chip_info_t       ti;
    bcm_port_config_t   config;
    const bcm_sys_board_t       *board_info;

    board_info = hpcBoardGet();

    if (!board_info) return BCM_E_FAIL;

    /* Register custom topomap function to handle b2b configuration */
    bcm_topo_map_set(_lvl7_board_topomap_48g_stk);

    l_entry = &tp_cpu->local_entry;

    /* setup the modids and trunks for the interconnect */
    for (src_unit = 0; src_unit < l_entry->base.num_units; src_unit++) {
      memset(&trunk,0,sizeof(trunk));
      trunk.psc        = BCM_TRUNK_PSC_SRCDSTMAC;
      trunk.dlf_index  = -1;
      trunk.mc_index   = -1;
      trunk.ipmc_index = -1;
      for (dst_unit = 0; dst_unit < l_entry->base.num_units; dst_unit++) {
        for (iter = 0; iter < board_info->num_interconnects;iter++) {
          if ((src_unit == board_info->interconnect_list[iter].from_unit) &&
              (dst_unit == board_info->interconnect_list[iter].to_unit)) {
              /* add to trunk */
              trunk.tp[trunk.num_ports++] = board_info->interconnect_list[iter].from_port;
          }
        }
      }

      if (trunk.num_ports > 0) {
        BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(src_unit, &ti));
        bcm_trunk_destroy(src_unit, ti.trunk_fabric_id_max);
        lvl7_internal_hg_trunkid = ti.trunk_fabric_id_max;
        BCM_IF_ERROR_RETURN(bcm_trunk_create_id(src_unit, ti.trunk_fabric_id_max));
        BCM_IF_ERROR_RETURN(bcm_trunk_set(src_unit, ti.trunk_fabric_id_max, &trunk));
      }
      BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(src_unit, l_entry->mod_ids[src_unit]));
    }

    /* setup the externally accessible modid map */
    /* Gets the MODIDs accessible via external stack ports */
    for (src_unit = 0; src_unit < l_entry->base.num_units; src_unit++) {
      TOPO_FOREACH_STK_PORT(stk_port, tp_cpu, sp_idx) {
        TOPO_FOREACH_TX_MODID(mod_id, tp_cpu, sp_idx, m_idx) {
          if (stk_port->unit == src_unit) {
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(src_unit, mod_id,stk_port->port));
            for (dst_unit = 0; dst_unit < l_entry->base.num_units; dst_unit++) {
              for (iter = 0; iter < board_info->num_interconnects;iter++) {
                if ((src_unit == board_info->interconnect_list[iter].from_unit) &&
                    (dst_unit == board_info->interconnect_list[iter].to_unit)) {
                  BCM_IF_ERROR_RETURN(bcm_stk_modport_set(dst_unit, mod_id,board_info->interconnect_list[iter].to_port));
                }
              }/* iter over connections */
            }/* for dst_unit */
          }/* (stk_port->unit == src_unit)  */
        }/* TOPO_FOREACH_TX_MODID */
      }/* TOPO_FOREACH_STK_PORT */
    }/* for src_unit */

    /* setup the internal interconnect modmap */
    for (src_unit = 0; src_unit < l_entry->base.num_units; src_unit++) {
      for (dst_unit = 0; dst_unit < l_entry->base.num_units; dst_unit++) {
        for (iter = 0; iter < board_info->num_interconnects;iter++) {
          if ((src_unit == board_info->interconnect_list[iter].from_unit) &&
              (dst_unit == board_info->interconnect_list[iter].to_unit)) {
            BCM_IF_ERROR_RETURN(bcm_stk_modport_set(src_unit,
                                                    l_entry->mod_ids[dst_unit],
                                                    board_info->interconnect_list[iter].from_port));

            BCM_IF_ERROR_RETURN(bcm_board_internal_stk_port_add(src_unit, board_info->interconnect_list[iter].from_port));
          }
        }
      }

      BCM_IF_ERROR_RETURN(bcm_port_config_get(src_unit, &config));
      BCM_IF_ERROR_RETURN(bcm_board_module_filter(src_unit, tp_cpu, db_ref,&config));
    }

    BCM_IF_ERROR_RETURN(bcm_board_trunk(l_entry, TRUE));
    BCM_IF_ERROR_RETURN(_bcm_board_dual_xgs3_e2e_set(FB_UNIT0, l_entry->mod_ids[FB_UNIT0],
                                                     FB_UNIT1, l_entry->mod_ids[FB_UNIT1]));

    return BCM_E_NONE;
}

#undef FB_UNIT0
#undef FB_UNIT1

int
static lvl7_bcm_board_topo_sdk_xgs3_48g(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    /* setup the topology, modmaps, and trunks for the chips */
    return lvl7_48g_topo(tp_cpu,db_ref);
}

#ifdef INCLUDE_RCPU
static int
_lvl7_bcm_board_topomap_dual_cpu(int src_unit, int dest_modid,
                            bcm_port_t *exit_port)
{
    *exit_port = soc_property_get(src_unit, spn_RCPU_PORT, 3);;    /* RCPU port = ge0 */
    return BCM_E_NONE;
}

static int 
lvl7_bcm_board_topo_sdk_dual_cpu (topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    bcm_topo_map_set(_lvl7_bcm_board_topomap_dual_cpu);
    /* NOTE: Standalone init only. No external stacking ports defined */

    return BCM_E_NONE;
}
#endif


/*
 * Function:
 *      lvl7_topo_interconnect
 * Purpose:
 *      Remove internal connections for the portmap and set the config parms for speed
 *      
 * Parameters:
 *      pbmp_xport_xe    - IN/OUT - the 10g port bitmap to be modified based on interconnect
 * Returns: 
 *      Void
 */
void  lvl7_topo_interconnect(pbmp_t  *pbmp_xport_xe)
{
    int iter;
    char      configString[64];
    int bcm_unit, bcm_port, speed;
    const bcm_sys_board_t       *board_info;

    board_info = hpcBoardGet();

    if (!board_info) return;

    /* No interconnect to work with */
    if (board_info->num_interconnects == 0) return;

    for (iter = 0; iter < board_info->num_interconnects;iter++) {
      bcm_unit = board_info->interconnect_list[iter].from_unit;
      bcm_port = board_info->interconnect_list[iter].from_port;
      speed    = board_info->interconnect_list[iter].speed;

      BCM_PBMP_PORT_REMOVE (pbmp_xport_xe[bcm_unit] , bcm_port);

      if ( speed == 10 ) {
        sal_config_set("xgxs_lcpll_12gbps","0");
        /* for "port" based config parms, the port is 1 based */
        sal_sprintf(configString,"port_init_speed.port%d.%d",bcm_port + 1,bcm_unit);
        sal_config_set(configString,"10000");
      }
      else if ( speed == 12 ) {
        sal_config_set("xgxs_lcpll_12gbps","1");
        /* for "port" based config parms, the port is 1 based */
        sal_sprintf(configString,"port_init_speed.port%d.%d",bcm_port + 1,bcm_unit);
        sal_config_set(configString,"12000");
      }
      else {
        /*
         * Nothing to do
         */
      }
    }
}

/*
 * Function:
 *      lvl7_topo_board_program
 * Purpose:
 *      Program a board given local topology info
 *      Should work simular to the code in topo_board_program
 * Parameters:
 *      db_ref    - Data base of current configuration
 *      topo_cpu  - Info for local board programming
 *      rv - return code (BCM_E_XXX)
 * Returns:
 *      0 - If no board found
 *      1 - If the board was handled
 * Notes:
 */
int lvl7_topo_board_program(cpudb_ref_t db_ref, topo_cpu_t *topo_cpu,int *rv)
{
  int handled = 0;
 
  /* callout to the customer routine prior to running our code */
  if (customer_topo_board_program != L7_NULLPTR)
  {
    handled = customer_topo_board_program(db_ref, topo_cpu,rv);
    dapiTraceStackEvent("customer_topo_board_program rv=%d handled=%d",*rv,handled);
  }
   
  if (handled != 0)
  {
    return handled;
  }
  /* Chassis LM board is programed below using board id */
#if !defined(INCLUDE_CHASSIS)
  else if (soc_ndev == 1 && SOC_IS_TR_VL(0)) {
    *rv = bcm_board_topo_sdk_xgs3_24g(topo_cpu, db_ref);
    dapiTraceStackEvent("bcm_board_topo_sdk_xgs3_24g rv=%d",*rv);
    handled = 1;
#ifdef INCLUDE_RCPU
  } else if ( soc_ndev == 2 && 
              SOC_IS_RAPTOR(0) && 
              SOC_IS_RAPTOR(1)) {
    /* Dual CPU board with RCPU comms */
    *rv = lvl7_bcm_board_topo_sdk_dual_cpu(topo_cpu, db_ref);
    dapiTraceStackEvent("lvl7_bcm_board_topo_sdk_dual_cpu rv=%d",*rv);
    handled = 1;
  } else if ( soc_ndev == 2 && 
              SOC_IS_RAVEN(0) && 
              SOC_IS_RAVEN(1)) {
    /* Dual CPU board with RCPU comms */
    *rv = lvl7_bcm_board_topo_sdk_dual_cpu(topo_cpu, db_ref);
    dapiTraceStackEvent("lvl7_bcm_board_topo_sdk_dual_cpu rv=%d",*rv);
    handled = 1;
#endif
  } else if (soc_ndev == 1 && SOC_IS_BRADLEY(0)) {
    /* Bradley 20 port box */
    *rv = bcm_board_topo_xgs3_20x(topo_cpu, db_ref);
    dapiTraceStackEvent("bcm_board_topo_xgs3_20x rv=%d",*rv);
    handled = 1;    
  } else if (soc_ndev == 1 && SOC_IS_SCORPION(0)) {
    /* Scorpion 24hg+4g port box */
    *rv = bcm_board_topo_xgs3_20x(topo_cpu, db_ref);
    dapiTraceStackEvent("bcm_board_topo_xgs3_20x rv=%d",*rv);
    handled = 1;    
  } else if ( soc_ndev == 2 && 
              SOC_IS_FB_FX_HX(0) && 
              SOC_IS_FB_FX_HX(1) ) {
    *rv = lvl7_bcm_board_topo_sdk_xgs3_48g(topo_cpu, db_ref);
    dapiTraceStackEvent("lvl7_bcm_board_topo_sdk_xgs3_48g rv=%d",*rv);
    handled = 1;
  }
#else /* INCLUDE_CHASSIS */
  else if (soc_ndev == 1 && SOC_IS_FIREBOLT(0)) {
    *rv = bcm_board_topo_sdk_xgs3_24g(topo_cpu, db_ref);
    dapiTraceStackEvent("bcm_board_topo_sdk_xgs3_24g rv=%d",*rv);
    handled = 1;
  }else{
    switch (db_ref->local_entry->base.board_id) {
      case cpudb_board_id_lm_56800_12x:
		*rv = bcm_board_topo_lm_56800_12x(topo_cpu, db_ref);
        dapiTraceStackEvent("bcm_board_topo_lm_56800_12x rv=%d",*rv);
        handled = 1;
        break;
      default:
		handled = 0;
    }
  }
#endif

  return handled;
}
