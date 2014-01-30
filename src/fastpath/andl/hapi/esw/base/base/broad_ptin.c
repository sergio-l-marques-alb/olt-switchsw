#include "broad_ptin.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_l3.h"
#include "ptin_hapi_xlate.h"
#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_fp_bwpolicer.h"
#include "ptin_hapi_fp_counters.h"
#include "ptin_hapi_fp_utils.h"
#include "ptin_structs.h"
#include "ptin_globaldefs.h"
#include "broad_common.h"

#include <dapi_db.h>
#include <hpc_db.h>

/**
 * Initialize HAPI PTin data structures
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinDataInit(void)
{
  L7_RC_t rc;

  LOG_INFO(LOG_CTX_PTIN_HAPI, "PTin HAPI Applying configs...");

  /* Initialize PTIN HAPI files */
  rc = hapi_ptin_data_init();

  LOG_INFO(LOG_CTX_PTIN_HAPI, "PTin HAPI data structs config: %d",rc);

  return rc;
}

/**
 * Initialize HAPI PTin module
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinInit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "PTin HAPI Applying configs...");

  /* Initialize PTIN HAPI files */
  rc = hapi_ptin_config_init();

  //hapi_ptin_phy_config_init();

  LOG_INFO(LOG_CTX_PTIN_HAPI, "PTin HAPI Configuration: %d",rc);

  return rc;
}

/**
 * Apply hardware procedure
 * 
 * @param usp 
 * @param cmd 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadHwApply(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_hwproc_t *hwproc = (ptin_hwproc_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_HAPI, "PTin HAPI Configuration: procedure=%u, param1=%u, param2=%u", hwproc->procedure, hwproc->param1, hwproc->param2);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  if (hwproc->procedure == PTIN_HWPROC_NONE)
  {
    LOG_INFO(LOG_CTX_PTIN_HAPI, "Nothing to do");
    return L7_SUCCESS;
  }

  /* Validate operation */
  if (hwproc->operation != DAPI_CMD_SET && hwproc->operation != DAPI_CMD_CLEAR)
  {
    LOG_INFO(LOG_CTX_PTIN_HAPI, "Operation not recognized: %u", hwproc->operation);
    return L7_SUCCESS;
  }

  switch (hwproc->procedure)
  {
  case PTIN_HWPROC_LINKSCAN:
    if (hwproc->operation == DAPI_CMD_GET)
    {
      rc = ptin_hapi_linkscan_get(usp, dapi_g, &hwproc->param1); 
      if (rc != L7_SUCCESS)
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with ptin_hapi_linkscan_get");
    }
    else if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_linkscan_set(usp, dapi_g, hwproc->param1);
      if (rc != L7_SUCCESS)
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with ptin_hapi_linkscan_set (%u)", hwproc->param1);
    }
    break;

  case PTIN_HWPROC_FORCE_LINK:
    if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_link_force(usp, dapi_g, hwproc->param1, L7_ENABLE);
      if (rc != L7_SUCCESS)
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with ptin_hapi_force_link (link=%u, enable=1)", hwproc->param1);
    }
    else if (hwproc->operation == DAPI_CMD_CLEAR)
    {
      rc = ptin_hapi_link_force(usp, dapi_g, hwproc->param1, L7_DISABLE);
      if (rc != L7_SUCCESS)
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with ptin_hapi_force_link (link=%u, enable=0)", hwproc->param1);
    }
    break; 

  default:
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid procedure: %u", hwproc->procedure);
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Get slot mode list
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinSlotMode(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc = L7_SUCCESS;

  #if (PTIN_BOARD==PTIN_BOARD_CXO640G)
  ptin_slotmode_t *slotmode;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  L7_uint32                    *wcSlotMode;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;
  wcSlotMode           = dapiCardPtr->wcSlotMode;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Slot mode operation");

  slotmode = (ptin_slotmode_t *) data;

  /* Get operation */
  if (slotmode->operation == DAPI_CMD_GET)
  {
    memcpy(slotmode->slotMode, wcSlotMode, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);
  }
  /* Make a validation */
  else if (slotmode->operation == DAPI_CMD_SET)
  {
    /* Return failure if map is not valid */
    rc = hpcConfigWCmap_build(slotmode->slotMode, L7_NULLPTR);
  }

  //LOG_INFO(LOG_CTX_PTIN_HAPI, "PTin HAPI Configuration: %d",rc);
  #endif

  return rc;
}

/**
 * Set MEF Extension parameters
 * 
 * @param usp : interface
 * @param cmd : no meaning
 * @param data : priority (int)
 * @param dapi_g : port definitions
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinPortExt(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_HWPortExt_t  *portExt;
  L7_RC_t rc = L7_SUCCESS;
  L7_int  macLearn_enable, stationMove_enable, stationMove_prio, stationMove_samePrio, port_type;

  /* Extract priority */
  portExt = (ptin_HWPortExt_t *) data;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort,usp,dapi_g);

  /* Set operation */
  if (portExt->operation==DAPI_CMD_GET)
  {
    portExt->Mask = 0;

    /* MAC learning attributes */
    rc = hapi_ptin_l2learn_port_get( &dapiPort, &macLearn_enable, &stationMove_enable, &stationMove_prio, &stationMove_samePrio );

    if (rc==L7_SUCCESS)
    {
      portExt->macLearn_enable               = macLearn_enable;
      portExt->macLearn_stationMove_enable   = stationMove_enable;
      portExt->macLearn_stationMove_prio     = stationMove_prio;
      portExt->macLearn_stationMove_samePrio = stationMove_samePrio;
      portExt->Mask |=  PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO;

      /* Egress port type */
      rc = hapi_ptin_egress_port_type_get(&dapiPort, &port_type);

      if (rc==L7_SUCCESS)
      {
        portExt->egress_type = port_type;
        portExt->Mask       |= PTIN_HWPORTEXT_MASK_EGRESS_TYPE;
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        rc = L7_SUCCESS;
      }
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Result for L2LearnPortSet: %d",rc);
  }
  else if (portExt->operation==DAPI_CMD_SET)
  {
    /* MAC learning attributes */
    rc = hapi_ptin_l2learn_port_set(&dapiPort,
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE)               ? portExt->macLearn_enable : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE)   ? portExt->macLearn_stationMove_enable : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO)     ? portExt->macLearn_stationMove_prio : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO) ? portExt->macLearn_stationMove_samePrio : -1) );
    if (rc == L7_SUCCESS)
    {
      /* Egress port type */
      if (portExt->Mask & PTIN_HWPORTEXT_MASK_EGRESS_TYPE)
      {
        /* TODO: Set to egress_type */
        rc = hapi_ptin_egress_port_type_set(&dapiPort, portExt->egress_type);
      }
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Result for L2LearnPortSet: %d",rc);
  }

  return rc;
}


/**
 * Read counters from a physical interface
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinCountersRead(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  rc = hapi_ptin_counters_read((ptin_HWEthRFC2819_PortStatistics_t *)data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/**
 * Clear counters
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinCountersClear(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_HWEthRFC2819_PortStatistics_t *stat = (ptin_HWEthRFC2819_PortStatistics_t *) data;
  L7_uint port;

  port = stat->Port;

  if ( hapi_ptin_counters_clear(port) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error on hapi_ptin_counters_clear() on port# %u", port);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Get counters activity summary
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEth_PortsActivity_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinCountersActivityGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  rc = hapi_ptin_counters_activity_get((ptin_HWEth_PortsActivity_t*)data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Uses vlan translation functionalities
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlanXlate_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanTranslate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_vlanXlate_t *xlate = (ptin_vlanXlate_t *) data;
  L7_RC_t rc = L7_SUCCESS;
  ptin_dapi_port_t dapiPort;
  ptin_hapi_xlate_t hapi_xlate;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "usp={%d,%d,%d}, Stage=%u, operation=%u, group=%u, oVlanId=%u, iVlanId=%u, newOVlanId=%u(%u), newIVlanId=%u(%u)",
            usp->unit, usp->slot, usp->port, xlate->stage, xlate->oper, xlate->portgroup,
            xlate->outerVlan, xlate->innerVlan,
            xlate->outerVlan_new,xlate->outerAction,
            xlate->innerVlan_new,xlate->innerAction);

  /* Validate interface */
  if ( ( xlate->portgroup==PTIN_XLATE_PORTGROUP_INTERFACE ) &&
       ( usp->unit<0 || usp->slot<0 || usp->port<0 ) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort,usp,dapi_g);

  /* Copy vlan and action information */
  hapi_xlate.outerVlanId    = xlate->outerVlan;
  hapi_xlate.innerVlanId    = xlate->innerVlan;
  hapi_xlate.newOuterVlanId = xlate->outerVlan_new;
  hapi_xlate.newInnerVlanId = xlate->innerVlan_new;
  hapi_xlate.outerAction    = xlate->outerAction;
  hapi_xlate.innerAction    = xlate->innerAction;

  switch ((L7_int) xlate->stage)
  {
  case PTIN_XLATE_STAGE_INGRESS:

    if ( xlate->oper == DAPI_CMD_GET )
    {
      rc = ptin_hapi_xlate_ingress_get(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_get(&dapiPort, xlate->outerVlan, xlate->innerVlan, &(xlate->outerVlan_new));
    }
    else if ( xlate->oper == DAPI_CMD_SET )
    {
      rc = ptin_hapi_xlate_ingress_add(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_add(&dapiPort, xlate->outerVlan, xlate->innerVlan, xlate->outerVlan_new);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      rc = ptin_hapi_xlate_ingress_delete(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_delete(&dapiPort, xlate->outerVlan, xlate->innerVlan);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR_ALL )
    {
      rc = ptin_hapi_xlate_ingress_delete_all();
      //rc = ptin_hapi_xlate_ingress_action_delete_all();
    }
    break;

  case PTIN_XLATE_STAGE_EGRESS:
    if ( xlate->oper == DAPI_CMD_GET )
    {
      rc = ptin_hapi_xlate_egress_get(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_get(xlate->portgroup, xlate->outerVlan, xlate->innerVlan, &(xlate->outerVlan_new));
    }
    else if ( xlate->oper == DAPI_CMD_SET )
    {
      rc = ptin_hapi_xlate_egress_add(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_add(xlate->portgroup, xlate->outerVlan, xlate->innerVlan, xlate->outerVlan_new);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      rc = ptin_hapi_xlate_egress_delete(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_delete(xlate->portgroup, xlate->outerVlan, xlate->innerVlan);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR_ALL )
    {
      rc = ptin_hapi_xlate_egress_delete_all();
      //rc = ptin_hapi_xlate_egress_action_delete_all();
    }
    break;

  case PTIN_XLATE_STAGE_ALL:
    if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "All translations will be removed");

      if ( (ptin_hapi_xlate_ingress_delete_all() != L7_SUCCESS) ||
           (ptin_hapi_xlate_egress_delete_all() != L7_SUCCESS) )
        rc = L7_FAILURE;
//    if ( (ptin_hapi_xlate_ingress_action_delete_all() != L7_SUCCESS) ||
//         (ptin_hapi_xlate_egress_action_delete_all() != L7_SUCCESS) )
//      rc = L7_FAILURE;
    }
    break;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define port groups for egress translation
 * 
 * @param usp 
 * @param cmd 
 * @param data: structure ptin_vlanXlate_classId_t
 * @param dapi_g 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanTranslatePortGroups(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_vlanXlate_classId_t *classSt = (ptin_vlanXlate_classId_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "usp={%d,%d,%d}, Oper=%d, classId=%d",
            usp->unit, usp->slot, usp->port, classSt->oper, classSt->class_id);

  /* Validate usp */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid usp");
    return L7_FAILURE;
  }

  switch ((L7_int) classSt->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_xlate_egress_portsGroup_set(classSt->class_id, &usp, 1, dapi_g);
    break;

  case DAPI_CMD_GET:
    rc = ptin_hapi_xlate_egress_portsGroup_get(&(classSt->class_id), usp, dapi_g);
    break;
  }
  
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define general vlan settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  bcm_pbmp_t upbmp;
  ptin_vlan_mode_t *mode = (ptin_vlan_mode_t *) data;
  DAPI_USP_t cpuUsp;
  bcm_error_t rv;

  if (mode->vlanId<=1 || mode->vlanId>=4094)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Management cpu port is added when the vlan is created. Update the Hw 
  *  Vlan member mask
  */
  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error obtaining CPU interfaces");
    return L7_FAILURE;
  }

  BCM_PBMP_CLEAR(upbmp);
  if (mode->cpu_include)
  {
    /* Add CPU interface to vlan */
    rv = bcm_vlan_port_add(0, mode->vlanId, PBMP_CMIC(0), upbmp);
  }
  else
  {
    /* Remove CPU interface to vlan */
    rv = bcm_vlan_port_remove(0, mode->vlanId, PBMP_CMIC(0));
  }

  /* Any error? */
  if (L7_BCMX_OK(rv) != L7_TRUE)
    return L7_FAILURE;

  /* Add/remove all the local CPUs to the vlan */
  for (cpuUsp.port=0; cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
  {
    if (mode->cpu_include)
      BROAD_HW_VLAN_MEMBER_SET(&cpuUsp,mode->vlanId,dapi_g);
    else
      BROAD_HW_VLAN_MEMBER_CLEAR(&cpuUsp,mode->vlanId,dapi_g);
  }

  return L7_SUCCESS;
}

/**
 * Define vlan mode settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bridge_vlan_mode_t *mode = (ptin_bridge_vlan_mode_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"vlanId=%u, mask=0x%02X, fwdVlan=%u, outerTPID=0x%04X, mac_learn=%u, crossc=%u",
            mode->vlanId, mode->mask, mode->fwdVlanId, mode->outer_tpid, mode->learn_enable, mode->cross_connects_enable);

  /* Validate vlan */
  if (mode->vlanId==0 || mode->vlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid vlan (%d)", mode->vlanId);
    return L7_FAILURE;
  }

  /* If mask is empty there is nothing to be done */
  if (mode->mask==PTIN_BRIDGE_VLAN_MODE_MASK_NONE)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"Mask is empty... nothing to be done");
    return L7_SUCCESS;
  }

  /* Forward vlan configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_FWDVLAN)
  {
    if (ptin_hapi_bridge_vlan_mode_fwdVlan_set(mode->vlanId, mode->fwdVlanId)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  /* Multicast configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_MC_GROUP)
  {
    if (ptin_hapi_bridgeVlan_multicast_set(mode->vlanId, &mode->multicast_group)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  /* Outer TPID configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_OTPID)
  {
    if (ptin_hapi_bridge_vlan_mode_outerTpId_set(mode->vlanId, mode->outer_tpid)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }
  /* MAC learning enable */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_LEARN_EN)
  {
    if (ptin_hapi_bridge_vlan_mode_macLearn_set(mode->vlanId, mode->learn_enable)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }
  /* Cross-connect enable */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_CROSSCONN_EN)
  {
    if (ptin_hapi_bridge_vlan_mode_crossconnect_set(mode->vlanId, mode->cross_connects_enable, mode->double_tag)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define vlan mode settings related to Multicast groups
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeVlanMulticastSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_bridge_vlan_multicast_t *mode = (ptin_bridge_vlan_multicast_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (mode->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_bridgeVlan_multicast_set(mode->vlanId, &mode->multicast_group);
    break;
  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_bridgeVlan_multicast_reset(mode->vlanId, mode->multicast_group, mode->destroy_on_clear);
    break;
  case DAPI_CMD_CLEAR_ALL:
    rc = ptin_hapi_bridgeVlan_multicast_reset(mode->vlanId, mode->multicast_group, mode->destroy_on_clear);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Configure Multicast egress ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinMulticastEgressPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_bridge_vlan_multicast_t *mode = (ptin_bridge_vlan_multicast_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (mode->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_multicast_egress_port_add(&mode->multicast_group, &dapiPort);
    break;
  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_multicast_egress_port_remove(mode->multicast_group, &dapiPort);
    break;
  case DAPI_CMD_CLEAR_ALL:
    rc = ptin_hapi_multicast_egress_clean(mode->multicast_group, mode->destroy_on_clear);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Configure Virtual ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVirtualPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_vport_t *vport = (ptin_vport_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (vport->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_vp_create(&dapiPort,
                             vport->ext_ovid, vport->ext_ivid,
                             vport->int_ovid, vport->int_ivid,
                             &vport->multicast_group,
                             &vport->virtual_gport);
    break;
  case DAPI_CMD_CLEAR:
  case DAPI_CMD_CLEAR_ALL:
    rc = ptin_hapi_vp_remove(&dapiPort, vport->ext_ovid, vport->ext_ivid, vport->virtual_gport, vport->multicast_group);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Set crossconnections
 * 
 * @param usp 
 * @param cmd 
 * @param data: ptin_bridge_crossconnect_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeCrossconnect(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bridge_crossconnect_t *crossc = (ptin_bridge_crossconnect_t *) data;
  ptin_dapi_port_t dapiPort1, dapiPort2;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"SrcPort={%d,%d,%d}, DstPort={%d,%d,%d}, outerVlanId=%u, innerVlanId=%u",
            usp->unit, usp->slot, usp->port,
            crossc->dstUsp.unit, crossc->dstUsp.slot, crossc->dstUsp.port,
            crossc->outerVlanId, crossc->innerVlanId);

  /* Prepare dapiPortx structures */
  DAPIPORT_SET(&dapiPort1, usp, dapi_g);
  DAPIPORT_SET(&dapiPort2, &(crossc->dstUsp), dapi_g);

  /* Execute the correct operation */
  switch ((L7_int) crossc->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_bridge_crossconnect_add(crossc->outerVlanId, crossc->innerVlanId, &dapiPort1, &dapiPort2);
    break;

  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_bridge_crossconnect_delete(crossc->outerVlanId, crossc->innerVlanId);
    break;

  case DAPI_CMD_CLEAR_ALL:
    rc  = ptin_hapi_bridge_crossconnect_delete_all();
    break;
  }

  /* Return the operation result */
  return rc;
}

/**
 * BW Policers implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinBwPolicer(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bwPolicer_t *bwPolicer = (ptin_bwPolicer_t *) data;
  L7_RC_t status=L7_SUCCESS;

  switch (bwPolicer->operation)  {
    case DAPI_CMD_GET:
      status=hapi_ptin_bwPolicer_get(&bwPolicer->profile, bwPolicer->policy_ptr);
      break;

    case DAPI_CMD_SET:
      status=hapi_ptin_bwPolicer_set(&bwPolicer->profile, &(bwPolicer->policy_ptr), dapi_g);
      break;

    case DAPI_CMD_CLEAR:
      status=hapi_ptin_bwPolicer_delete(bwPolicer->policy_ptr);
      break;
            
  case DAPI_CMD_CLEAR_ALL:
    status=hapi_ptin_bwPolicer_deleteAll();
    break;

    default:
      status = L7_FAILURE;
  }

  return status;
}

/**
 * Counters implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinFpCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_evcStats_t *fpCounter = (ptin_evcStats_t *) data;
  L7_RC_t status=L7_SUCCESS;

  switch (fpCounter->operation)  {
    case DAPI_CMD_GET:
      status=hapi_ptin_fpCounters_get(&fpCounter->counters, fpCounter->policy_ptr);
      break;

    case DAPI_CMD_SET:
      status=hapi_ptin_fpCounters_set(&fpCounter->profile, &(fpCounter->policy_ptr), dapi_g);
      break;

    case DAPI_CMD_CLEAR:
      status=hapi_ptin_fpCounters_delete(fpCounter->policy_ptr);
      break;
            
  case DAPI_CMD_CLEAR_ALL:
    status=hapi_ptin_fpCounters_deleteAll();
    break;

    default:
      status = L7_FAILURE;
  }

  return status;
}

/**
 * Traffic Rate limiters
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadStormControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t    dapiPort;
  ptin_stormControl_t *stormControl = (ptin_stormControl_t *) data;
  L7_RC_t status=L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (stormControl->operation)  {
    case DAPI_CMD_SET:
      status=hapi_ptin_stormControl_set(&dapiPort, L7_ENABLE, stormControl);
      break;

    case DAPI_CMD_CLEAR:
      status=hapi_ptin_stormControl_set(&dapiPort, L7_DISABLE, stormControl);
      break;

    default:
      status = L7_FAILURE;
  }

  return status;
}

/**
 * Enable PRBS generator/checker
 * 
 * @param usp : portInfo
 * @param enable : enable
 * @param dapi_g : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsEnable(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  bcm_port_t  bcm_port;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  bcm_port = hapiSlotMapPtr[usp->port].bcm_port;

  if (bcm_port_control_set(0, bcm_port, bcmPortControlPrbsTxEnable, enable & 1)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR applying enable state %u to port {%d,%d,%d}, bcm_port=%d",enable,usp->unit,usp->slot,usp->port,bcm_port);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "PRBS TX/RX %s for port {%d,%d,%d}, bcm_port=%d",
            ((enable) ? "enabled" : "disabled"),
            usp->unit,usp->slot,usp->port,
            bcm_port);

  return L7_SUCCESS;
}

/**
 * Read number of PRBS errors
 * 
 * @param usp : portInfo
 * @param rxErrors : number of errors (-1 if no lock)
 * @param dapi_g   : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsRxStatus(DAPI_USP_t *usp, L7_uint32 *rxErrors, DAPI_t *dapi_g)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  bcm_port_t  bcm_port;
  int         rxStatus;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  bcm_port = hapiSlotMapPtr[usp->port].bcm_port;

  if (bcm_port_control_get(0, bcm_port, bcmPortControlPrbsRxStatus, &rxStatus)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR reading rx status from port {%d,%d,%d}, bcm_port=%d",usp->unit,usp->slot,usp->port,bcm_port);
    return L7_FAILURE;
  }

  if (rxErrors!=L7_NULLPTR)
  {
    *rxErrors = (rxStatus==-1)  ? ((L7_uint32) -1) : rxStatus;
  }

  return L7_SUCCESS;
}


/**
 * Get system resources
 * 
 * @param usp 
 * @param cmd 
 * @param data : (st_ptin_policy_resources)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinResourcesGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_policy_resources *resources = (st_ptin_policy_resources *) data;

  return ptin_hapi_policy_resources_get(resources);
}


/**
 * Add L3 Host IP
 * 
 * @param usp 
 * @param cmd 
 * @param data :
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinL3Manage(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_l3 *ptr = (st_ptin_l3 *) data;
  ptin_dapi_port_t dapiPort;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  if (ptr->oper == PTIN_L3_MANAGE_HOST)
  {
    switch (ptr->cmd) 
    {
      case DAPI_CMD_SET:
        rc = ptin_hapi_l3_host_add(&dapiPort, ptr);
        break;

      case DAPI_CMD_CLEAR:
        rc = ptin_hapi_l3_host_remove(&dapiPort, ptr);
        break;

      default:
        rc = L7_FAILURE;
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Command not handled (%u)", ptr->cmd);
    }
  }
  else if (ptr->oper == PTIN_L3_MANAGE_ROUTE)
  {
    switch (ptr->cmd) 
    {
      case DAPI_CMD_SET:
        rc = ptin_hapi_l3_route_add(&dapiPort, ptr);
        break;

      case DAPI_CMD_CLEAR:
        rc = ptin_hapi_l3_route_remove(&dapiPort, ptr);
        break;

      default:
        rc = L7_FAILURE;
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Command not handled (%u)", ptr->cmd);
    }
  }
  else
  {
    rc = L7_FAILURE;
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Operation not implemented (%u)", ptr->oper);
  }

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error: rc=%u", rc);
  }

  return rc;
}

/*
L7_RC_t hapiBroadPtinStart(void)
{
  printf("hapiBroadPtinStart start\n");

  if (hapi_ptin_flow_init())  return L7_FAILURE;

  printf("hapiBroadPtinStart end\n");

  return L7_SUCCESS;
}
*/

