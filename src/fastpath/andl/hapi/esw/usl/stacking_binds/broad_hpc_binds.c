/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_binds.c
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component 
*
* @create    04/14/2003
*
* @author    jeffr 
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "buff_api.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "osapi.h"
#include "osapi_support.h"
#include "simapi.h"
#include "registry.h"
#include "log.h"
#include "unitmgr_api.h"
#include "l7_usl_bcm.h"
#include "nvstoreapi.h"
#include "hpc_hw_api.h"

#include "broad_common.h"

#include "ibde.h"
#include "bcm/vlan.h"
#include "bcm/cosq.h"
#include "soc/cmext.h"
#include "sysbrds.h"
#include "appl/diag/sysconf.h"
#include "appl/stktask/topo_brd.h"
#include "appl/stktask/topo_pkt.h"
#include "bcmx/bcmx_int.h"

/* PTin added: logger */
#include "logger.h"

static cpudb_ref_t system_cpudb = L7_NULLPTR;
static topo_cpu_t  system_topo;
extern void hapiBroadSocFileLoad(char *file_name, L7_BOOL suppressFileNotAvail);
/*********************************************************************
* @purpose  Determine whether specified BCM port is a stack port.
*
* @param    bcm_unit  - BCM unit number of the port.
* @param    bcm_port  - BCM port number of the port.
*
* @returns  L7_TRUE - BCM port is used for stacking.
* @returns  L7_FALSE - BCM port is not used for stacking.
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL hpcIsBcmPortStacking (L7_uint32 bcm_unit, L7_uint32 bcm_port)
{
  return L7_FALSE;
}


#ifndef L7_ROBO_SUPPORT
static cpudb_t *hpcBroadLocalCpudbCreate(void)
{
  cpudb_t *cpudbPtr;
  cpudb_entry_t *cpudbLclEntryPtr;
  cpudb_key_t local_cpu_key;
  HPC_UNIT_DESCRIPTOR_t *lclUnitDescriptorPtr;
  HPC_BROAD_UNIT_DATA_t *hpcPlatformDataPtr;
  L7_uint32 i;

  /* initialize and build local CPU database entry */
  if ((cpudbPtr = cpudb_create()) == NULL)
  {
    sysapiPrintf("hpcBroadLocalCpudbCreate: Error returned from cpudb_create().\n");
    LOG_ERROR(2);
  }

  (void)memset((void *)&local_cpu_key, 0, sizeof(cpudb_key_t));
  if (hpcLocalUnitIdentifierMacGet((L7_enetMacAddr_t *)&(local_cpu_key.key)) != L7_SUCCESS)
  {
    sysapiPrintf("hpcBroadLocalCpudbCreate: Error returned from hpcLocalUnitIdentifierMacGet().\n");
    LOG_ERROR(3);
  }

  if ((cpudbLclEntryPtr = cpudb_entry_create(cpudbPtr, local_cpu_key, L7_TRUE)) == NULL)
  {
    sysapiPrintf("hpcBroadLocalCpudbCreate: Error returned from cpudb_entry_create().\n");
    LOG_ERROR(3);
  }

  if ((lclUnitDescriptorPtr = hpcLocalUnitDescriptorGet()) == L7_NULLPTR)
  {
    sysapiPrintf("hpcBroadLocalCpudbCreate: Error returned from hpcLocalUnitDescriptorGet().\n");
    LOG_ERROR(3);
  }

  /* load local cpu database entry with data */
  sal_memcpy((void *)&(cpudbLclEntryPtr->base.mac), (void *)&(local_cpu_key.key), sizeof(bcm_mac_t));

  hpcPlatformDataPtr = (HPC_BROAD_UNIT_DATA_t *)lclUnitDescriptorPtr->hpcPlatformData;
  cpudbLclEntryPtr->base.num_units      = hpcPlatformDataPtr->num_units;
  cpudbLclEntryPtr->base.dest_port      = hpcPlatformDataPtr->dest_port;
  cpudbLclEntryPtr->base.dest_unit      = hpcPlatformDataPtr->dest_unit;

  for (i=0;i<BCM_MAX_NUM_UNITS;i++)
  {
    cpudbLclEntryPtr->base.mod_ids_req[i] = hpcPlatformDataPtr->mod_ids_req[i];
    cpudbLclEntryPtr->base.pref_mod_id[i] = -1; /* Don't set any preference at boot time */
  }

  cpudbLclEntryPtr->base.num_stk_ports = hpcPlatformDataPtr->num_stk_ports;

  for (i=0; i<hpcPlatformDataPtr->num_stk_ports; i++)
  {
    cpudbLclEntryPtr->base.stk_ports[i].unit = hpcPlatformDataPtr->stk_port_list[i].unit;
    cpudbLclEntryPtr->base.stk_ports[i].port = hpcPlatformDataPtr->stk_port_list[i].port;
  }

  /*
  ** always initially start Broadcom discovery with priority set to lower range.  This
  ** assures we will not usurp an incumbent management unit before the unit manager
  ** component finishes its protocol and sets the role for this box in the stack
  */
  cpudbLclEntryPtr->flags = CPUDB_F_BASE_INIT_DONE;

  return(cpudbPtr);
}
#endif

/* PTin added: application control */
void hpcHardwareFini(void)
{
  sal_core_term();

  sdk_term(0);
}

/**************************************************************************
*
* @purpose  Initialize the hardware-specific HPC system service.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems in initialization
* @returns  L7_FAILURE  encountered error in initialization
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareInit(void (*stack_event_callback_func)(hpcStackEventMsg_t event), 
                        void (*msg_recv_notify)(L7_enetMacAddr_t src_key,
                                                L7_uint32 receive_id, 
                                                L7_uchar8* buffer, 
                                                L7_uint32 msg_len))
{
  L7_RC_t   rc;
  L7_uint32 i;
  int rv;
  cpudb_key_t                  cpu_key;
  const bcm_sys_board_t       *board_info;
  soc_cm_init_t                init_data;

  sal_assert_set((sal_assert_func_t)hapiBroadAssert);

  /* Initialise the System Abstraction Layer(SAL) of the Broadcom vendor 
   * Driver
   */
  sal_core_init();
  sal_appl_init();

  sysconf_debug_vectors_get(&init_data);

  hapiBroadDebugBcmTrace(0);
  hapiBroadDebugBcmPrint(0);

  /* override the default output */
  init_data.debug_out = (void *)hapiBroadCmPrint;

  /* PTin added: new switch 56340 (Helix4) */
  #if (PTIN_BOARD==PTIN_BOARD_OLT1T0)
  /* PCI device ID override: when switch id is uncorrectly identified as 0xb34f */
  (void) sal_config_set(spn_PCI_OVERRIDE_DEV, "0xb340");

  LOG_TRACE(LOG_CTX_STARTUP,"b340 id imposed for Helix4 switch");
  #endif

  if (soc_cm_init(&init_data) != SOC_E_NONE)
  {
    LOG_ERROR (0);
  }
  
  hapiBroadSocFileLoad("sdk-preinit.soc", L7_TRUE);
 
  /* 
   * Initialize all devices on the PCI bus.
   */
  if (sysconf_probe() != 0)
  {
    LOG_ERROR (1);
  }
  board_info = hpcBoardGet();
  
  if (board_info==L7_NULL) {
      SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                     "\n%s %d: In %s call to 'hpcBoardGet' - FAILED\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
  }

  hpcLocalCardDbInit((void*)board_info);

  if ((rc = hpcBroadInit()) != L7_SUCCESS) 
  {
    LOG_ERROR (1);
  }


  if (hpcLocalUnitIdentifierMacGet((L7_enetMacAddr_t *)&(cpu_key.key)) != L7_SUCCESS)
  {
    LOG_ERROR(0);
  }

  /* 
  ** Set topology.  This is used by mirroring
  ** when more than one unit is present in
  ** the system.
  ** NOTE: this needs to be enhanced for future chips
  ** NOTE: this will not affect strata when its only
  ** one chip.
  */
#ifndef L7_ROBO_SUPPORT
  {
     memset(&system_topo, 0, sizeof(system_topo));

     system_cpudb = hpcBroadLocalCpudbCreate ();
     if (system_cpudb == L7_NULLPTR)
     {
       LOG_ERROR(0);    
     }

     /* Set the local cpu as master */
     cpudb_master_set(system_cpudb, cpu_key);

     if ((rv = topology_mod_ids_assign(system_cpudb)) != BCM_E_NONE)
     {
       LOG_ERROR(rv);
     }

     if ((rv = bcm_stack_topo_create(system_cpudb, &system_topo)) != BCM_E_NONE)
     {
       LOG_ERROR(rv);
     }

     /* PTin added: new SDK */
     if ((rv = topo_board_register(topo_board_default_board_program, NULL)) != BCM_E_NONE)
     {
       LOG_ERROR(rv);
     }
     /* PTin end */

     if ((rv = topo_board_program (system_cpudb, &system_topo)) != BCM_E_NONE)
     {
       LOG_ERROR(rv);
     }
  }
#endif

/* Added this for non-stackable, it was lifted from hpcBroadTransportInit */
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {

    hapiBroadMapDbCpuUnitEntryAdd(i, &cpu_key, i);
    (void) bcmx_device_attach(i);

    rv = bcm_rx_init(i);
    if (rv < 0)
    {
      printk("RX init failed, unit %d\n", i);
      break;
    }

    if (!bcm_rx_active(i))
    {
      rv = bcm_rx_start(i, NULL);
      if (rv < 0)
      {
        printk("RX start failed unit %d\n", i);
        break;
      }
    }

  }

  hpcBroadRpcInit();

  usl_init();

  hpcHardwareDefaultConfigApply ();

  return L7_SUCCESS;
}
   
/*********************************************************************
* @purpose  Sets the local unit's current role in the stack.  If setting
*           represents a change in role, reconfigures the vendor driver
*           stacking code to comply with caller's intended role.
*
*           If the role being set is SYSAPI_STACK_ROLE_MANAGEMENT_UNIT
*           and the vendor driver stacking code is not currently in 
*           that configuration, this function returns L7_ERROR and
*           takes steps to reconfigure the vendor stack code. The
*           caller should retry the set after a delay.  (Unit manager
*           state machine handles this.)
*
* @param    role        local unit's assigned role in the stack
*
* @returns  L7_SUCCESS  no problems in save, vendor stacking code state
*                       matches caller's intended role
* @returns  L7_ERROR    encountered error or caller requested 
*                       SYSAPI_STACK_ROLE_MANAGEMENT_UNIT and vendor
*                       driver needs to be reconfigured; retry after delay
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t hpcHardwareTopOfStackSet(SYSAPI_STACK_ROLE_t role)
{
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Send a message to all other units in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in any of the
*           remote systems, the message is silently discarded there and
*           no notification is sent to the sending caller.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in buffer 
* @param    buffer  pointer to the payload to be sent
*                                       
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareBroadcastMessageSend( L7_COMPONENT_IDS_t registrarID, L7_uint32 msg_length, L7_uchar8* buffer)
{
  return L7_SUCCESS;
}

void hpcTransportMessageRecvHandler(L7_uint32 receive_id, L7_uchar8* buffer, L7_uint32 msg_len);
/*********************************************************************
* @purpose  Send a message to a specific unit in the stack.  Message is delivered
*           to corresponding registrarID in remote system.  If there is
*           no registration of the registrarID in the local system, an
*           error is returned.  If there is no registration in the
*           remote systems, the message is silently discarded there and
*           an error is returned to the sending caller.  This function will
*           hold the caller's thread until the message is either successfully
*           acknowledged or the send times out.
*
*           If the message payload exceeds the platform transport's
*           maximum message size, this function will return an error
*           without sending anything.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in payload buffer
* @param    buffer  pointer to the payload to be sent
*                                       
* @returns  L7_SUCCESS message successfully dispatched to harware transport
* @returns  L7_FAILURE problem occured in transport, message not acknowledged
* @returns  L7_ERROR registrar is not known to HPC message transport
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareMessageSend(L7_COMPONENT_IDS_t registrarID, L7_uint32 unit,
                               L7_uint32 msg_length, L7_uchar8* buffer,
                               L7_uint32 hdr_length, L7_uchar8* header,
                               fastpath_client_id_t atpClientId)
{
  L7_uint32 localUnit;

  if ((unitMgrNumberGet(&localUnit) == L7_SUCCESS) &&
      (unit == localUnit))
  {
    hpcTransportMessageRecvHandler(registrarID, buffer, msg_length);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Init the ASF mode.
*          
* @param    mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareAsfInit(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  if (mode == L7_ENABLE)
  {
    rc = hpcHardwareDriverAsfEnable();
  }

  return rc;
}

/*********************************************************************
* @purpose  Set (Enable/Disable) the ASF mode.
*          
* @param    mode        @b{(input)} mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareAsfModeSet(L7_uint32 mode)
{
  L7_RC_t rv;
  hpcAsfCfg_t asfCfg;

  asfCfg.version = HPC_ASF_CONFIG_VER_CURRENT;
  asfCfg.configAsfMode = mode;
  asfCfg.crc = nvStoreCrc32((L7_uchar8 *)&asfCfg, (L7_uint32)(sizeof (hpcAsfCfg_t) - sizeof (asfCfg.crc)));
  rv = osapiFsWrite(HPC_ASF_CFG_FILENAME, (L7_char8 *)&asfCfg, sizeof(hpcAsfCfg_t));

  return rv;
}

/*********************************************************************
 * @purpose  This routine determines if there are any stack ports.
 *           If no stack ports, then unit manager can use this
 *           info to speed up it's state machines.
 *
 * @returns  L7_BOOL
 *
 * @comments    
 *       
 * @end
 *********************************************************************/
L7_BOOL hpcHardwareNoStackPorts()
{
  return L7_TRUE;
}

/*********************************************************************
* @purpose  On Standby, UM informs the driver that a new stack topology 
*           is available at the lower layer. Driver analyzes the new
*           stack topology to check if the current manager of the stack 
*           has failed. If so then the standby of the stack takes over 
*           as the manager.
*           
*
* @returns  L7_BOOL *managerFailed L7_TRUE: Manager has failed
*                                  L7_FALSE: Manager is present
*
* @comments Routine called by UM task
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareProcessStackTopoChange(L7_BOOL *managerFailed)
{
  *managerFailed = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  On Standby, UM receives stack port link down notifications 
*           from immediate neighbours of the Manager. UM in-turn passes
*           the information to the driver which analyzes the current
*           stack topology to check if the current manager of the stack 
*           has failed. If so then the standby of the stack takes over 
*           as the manager.
*
* @param    reportingUnit: Unit number of Reporting Manager neighbour
* @param    reportingUnitKey: Pointer to CPU key of the Manager neighbour
* @param    stkPortHpcIndex: Hpc index of the stack port that is down
* @param    nhopCpuKey: Pointer to CPU key of the reporting unit neighbour 
*                       (should be the current manager)
*           
*
* @returns  L7_BOOL *managerFailed L7_TRUE: Manager has failed
*                                  L7_FALSE: Manager is present
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareProcessManagerLinkDown(L7_uint32 reportingUnit, L7_enetMacAddr_t *reportingUnitKey, 
                                         L7_uint32 stkPortHpcIndex, L7_enetMacAddr_t *nhopCpuKey, 
                                         L7_BOOL *managerFailed)
{
  *managerFailed = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stby_present {(input)} L7_TRUE: There is a valid stby
*                                L7_FALSE: There is no valid stby
* @param   
* @param    stby_key {(input)} Key of the stby unit
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareNotifyStandbyStatus(L7_BOOL stby_present, 
                                       L7_enetMacAddr_t stby_key)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  UM on each unit informs the driver that the current manager 
*           of the stack is no longer present. Driver tries to remove 
*           the manager key from ATP/Next-hop transport layers so that 
*           pending/future communication with the failed manager is
*           prevented.
*
* @param    managerKey: CPU key of the Manager 
*           
*
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcHardwareRemoveStackManager(L7_enetMacAddr_t managerKey)
{
  return;
}

/*********************************************************************
* @purpose  Get the Master cpu dest modid/modport
*
*
* @param    modid   {(output)} modid to reach the master cpu
* @param    cpuport {(output)} cpu modport
*           
*
* @returns  BCM_E_xx
*
* @comments    
*       
* @end
*********************************************************************/
L7_int32 hpcBroadMasterCpuModPortGet(L7_int32 *modid, L7_int32 *cpuport)
{
  L7_int32       rv = BCM_E_FAIL;

#if 0
  cpudb_key_t    cpu_key;
  cpudb_entry_t *master_entry = L7_NULLPTR;
  

  do
  {
    if (hpcLocalUnitIdentifierMacGet((L7_enetMacAddr_t *)&(cpu_key.key)) != L7_SUCCESS)
    {
      break;
    }

    CPUDB_KEY_SEARCH(system_cpudb, cpu_key, master_entry);
    if (master_entry == L7_NULLPTR)
    {
      break;    
    }

    *modid = master_entry->dest_mod;
    *cpuport = master_entry->dest_port;

    rv = BCM_E_NONE;

  } while (0);
#else

  rv = bcmx_lport_to_modid_port(BCMX_LPORT_LOCAL_CPU_GET(0),
                                modid,
                                cpuport);
#endif

  return rv;
}
