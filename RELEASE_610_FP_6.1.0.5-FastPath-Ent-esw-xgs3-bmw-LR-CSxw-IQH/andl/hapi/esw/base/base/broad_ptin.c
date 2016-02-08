/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_PTIN.c
*
* @purpose   This file contains the hapi interface for system parameters
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#include <string.h>
#include <unistd.h>
#include <string.h>
#include "sysapi.h"
#include "osapi.h"
#include "dtl_exports.h"

#include "broad_common.h"
#include "phy_hapi.h"

#include "bcm/port.h"
#include "bcm/l2.h"
#include "bcm/rate.h"
#include "bcm/mirror.h"
#include "bcm/filter.h"
#include "soc/cmic.h"
#include "soc/drv.h"
#include "soc/macipadr.h"
#include "soc/mem.h"
#include "soc/cm.h"
#include "soc/robo.h"

#include "bcmx/l2.h"

#include "sysbrds.h"
#include "osapi_support.h"
#include "unitmgr_api.h"
#include "broad_mmu.h"
#include "platform_config.h"
//#include "l7_usl_port_db.h"
#include "broad_debug.h"
#include "ptin_hapi.h"


typedef struct _st_monitor {
  //st_HWEthPhyConf           PhyConf;
  //st_HWEthPhyState  PhyState;
  //st_HWEthSFPstatus_Block SfpStatus;
  //st_HWSFFInfo_Block      SfpInfo;
  st_HWEthRFC2819_PortStatistics stats;
} st_monitor;

st_monitor monitor[SYSTEM_N_PORTS];

// Semaphore for counters structure
void *sem_monitor_access=L7_NULL;
void *sem_shell_access=L7_NULL;

static L7_RC_t monitor_update_stats(uint8 port)
{
  L7_RC_t error=L7_SUCCESS;
  st_HWEthRFC2819_PortStatistics stats;

  if ( port>=SYSTEM_N_PORTS ) {
    return -1;
  }

  // Clear structure
  memset(&stats,0x00,sizeof(st_HWEthRFC2819_PortStatistics));
  stats.Port = port;
  stats.Mask = 0xFF;

  if ( (error=hapi_ptin_readCounters(port, &stats.Rx, &stats.Tx))!=L7_SUCCESS )  {
    memset(&stats,0x00,sizeof(st_HWEthRFC2819_PortStatistics));
  }

  // Copy stats to database
  osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  memcpy(&monitor[port].stats,&stats,sizeof(st_HWEthRFC2819_PortStatistics));
  osapiSemaGive(sem_monitor_access);

  return error;
}

#if 0
static void hapiBroadPtinCountersTask(uint32 numArgs, void *unit)
{
  uint8 port;

  sleep(5);

  while (1) {
    OSAPI_SEC_SLEEP(5);

    // Only update counters with a 10s period
    for ( port=0; port<SYSTEM_N_PORTS; port++ ) {
      monitor_update_stats(port);
      //OSAPI_MSEC_SLEEP(100);
    }
    // Flow counters will be updated with a 5s period
    hapi_ptin_flow_counters_update();
    OSAPI_MSEC_SLEEP(100);
  }
}
#endif

L7_RC_t hapiBroadPtinInit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  hapi_ptin_phy_config_init();

  // Create semaphore for counters access
  sem_monitor_access = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL);
  if (sem_monitor_access==L7_NULL) {
    LOG_ERROR(0);
  }
  // Create semaphore for bshell access
  sem_shell_access = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL);
  if (sem_shell_access==L7_NULL) {
    LOG_ERROR(0);
  }

  #if 0
  if (osapiTaskCreate("hapi: Counters", hapiBroadPtinCountersTask, 0, 0,
                      (64*1024),
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    printf("ERROR: Failed to start counters collector task\n");
    return L7_FAILURE;
  }
  #endif

  return L7_SUCCESS;
}

L7_RC_t hapiBroadPtinPhySet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_uint8 tx_disable = (L7_uint8) *((L7_uint8 *) data);

  printf("%s(%d) i am here: unit=%u, slot=%u, port=%u\r\n",__FUNCTION__,__LINE__,usp->unit,usp->slot,usp->port);

  // Only for physical interfaces
  if (usp->unit!=1 || usp->slot!=0) {
    return L7_SUCCESS;
  }

  // Control tx disable
  return hapi_ptin_phy_set(usp->port,tx_disable);
}

L7_RC_t hapiBroadPtinStart(void)
{
  printf("hapiBroadPtinStart\n");

  return hapi_ptin_flow_init();
}

L7_RC_t hapiBroadPtinGetHwResources(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_hw_resources *resources = (st_ptin_hw_resources *) data;

  return hapi_ptin_get_hw_resources(resources);
}


L7_RC_t hapiBroadPtinVlanDefs(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_vlan_defs *vlan_defs = (st_ptin_vlan_defs *) data;

  return hapi_ptin_vlan_defs(vlan_defs->create_vlan_1, vlan_defs->include_cpu_intf);
}


L7_RC_t hapiBroadPtinVlanTranslate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_vlan_translation *vlan_xlate = (st_vlan_translation *) data;

  return hapi_ptin_vlan_translate(vlan_xlate->operation, vlan_xlate->port, vlan_xlate->vlan_old, vlan_xlate->vlan_new);
}


L7_RC_t hapiBroadPtinFPentry(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t status=L7_SUCCESS;
  st_fpentry_oper *fpentry = (st_fpentry_oper *) data;

  switch (fpentry->operation)  {
    case FPENTRY_OPER_READ:
      status=hapi_ptin_fpentry_read(fpentry->profile,fpentry->fpentry_ptr);
      break;

    case FPENTRY_OPER_CREATE:
      status=hapi_ptin_fpentry_create(fpentry->profile,&(fpentry->fpentry_ptr));
      break;

    case FPENTRY_OPER_DESTROY:
      status=hapi_ptin_fpentry_destroy(fpentry->fpentry_ptr);
      break;
            
    case FPENTRY_OPER_COUNTERS:
      status=hapi_ptin_fpentry_counters(fpentry->counters, fpentry->fpentry_ptr);
      break;

    default:
      status = L7_FAILURE;
  }

  return status;
}


L7_RC_t hapiBroadPtinFlowCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t status=L7_SUCCESS;
  st_ptin_fcounters_oper *fcounters_oper = (st_ptin_fcounters_oper *) data;

  switch (fcounters_oper->oper)  {
    case PTIN_ADD_FLOW_COUNTERS:
      status=hapi_ptin_flow_counters_add(fcounters_oper->flow_id,fcounters_oper->flow_type,
                                         fcounters_oper->pon_ports.port_bmp,fcounters_oper->eth_ports.port_bmp,
                                         fcounters_oper->pon_ports.vlan,fcounters_oper->eth_ports.vlan,
                                         fcounters_oper->client_channel);
      break;

    case PTIN_REMOVE_FLOW_COUNTERS:
      status=hapi_ptin_flow_counters_remove(fcounters_oper->flow_id,fcounters_oper->client_channel);
      break;

    case PTIN_READ_FLOW_ABS_COUNTERS:
      status=hapi_ptin_flow_absCounters_read(fcounters_oper->flow_id,fcounters_oper->client_channel,&fcounters_oper->stats);
      break;
            
    case PTIN_READ_FLOW_DIFF_COUNTERS:
      status=hapi_ptin_flow_diffCounters_read(fcounters_oper->flow_id,fcounters_oper->client_channel,&fcounters_oper->stats);
      break;

    default:
      status = L7_FAILURE;
  }

  return status;
}


L7_RC_t hapiBroadPtinBcastPktLimit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_args *args = (st_ptin_args *) data;

  return hapi_ptin_flow_bcastpack_limit(usp, args->admin, args->vlanId, dapi_g);
}

L7_RC_t hapiBroadPtinBitstreamUpFwd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_args *args = (st_ptin_args *) data;

  return hapi_ptin_flow_bitstream_upFwd(usp, args->admin, args->vlanId, dapi_g);
}

L7_RC_t hapiBroadPtinBitstreamLagRecheck(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_args *args = (st_ptin_args *) data;

  return hapi_ptin_flow_bitstream_lagRecheck(usp, args->admin, dapi_g);
}

L7_RC_t hapiBroadPtinGetCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_HWEthRFC2819_PortStatistics *stat = (st_HWEthRFC2819_PortStatistics *) data;
  uint8 port;
  int unit;
  uint8   Mask;
  uint32  RxMask;
  uint32  TxMask;

  port = stat->Port;
  unit = usp->unit-1;

  Mask = stat->Mask;
  RxMask = stat->RxMask;
  TxMask = stat->TxMask;

  if ( port>=SYSTEM_N_PORTS ) {
    printf("%s: error with arguments\n", __FUNCTION__);
    return L7_FAILURE;
  }

  /* Update port counters */
  monitor_update_stats(port);

  osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  memcpy(stat,&monitor[port].stats,sizeof(st_HWEthRFC2819_PortStatistics));
  osapiSemaGive(sem_monitor_access);
  stat->Port = port;
  stat->Mask = Mask;
  stat->RxMask = RxMask;
  stat->TxMask = TxMask;

  return L7_SUCCESS;
}

L7_RC_t hapiBroadPtinClearCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_HWEthRFC2819_PortStatistics *stat = (st_HWEthRFC2819_PortStatistics *) data;
  uint8 port;
  int unit;
  L7_RC_t status=L7_SUCCESS;

  port = stat->Port;
  unit = usp->unit-1;

  if ( port>=SYSTEM_N_PORTS ) {
    printf("%s: error with arguments\n", __FUNCTION__);
    return L7_FAILURE;
  }
  
  if ( (status=hapi_ptin_clearCounters(port)) )  {
    printf("%s: error with hapi_ptin_clearCounters in port %u\n", __FUNCTION__,port);
    return status;
  }  

  osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  memset(&monitor[port].stats,0x00,sizeof(st_HWEthRFC2819_PortStatistics));
  monitor[port].stats.Port=port;
  monitor[port].stats.Mask=0xFF;
  monitor[port].stats.RxMask=0xFFFF;
  monitor[port].stats.TxMask=0xFFFF;
  osapiSemaGive(sem_monitor_access);

  return L7_SUCCESS;
}

L7_RC_t hapiBroadPtinStatisticStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  uint8 port;
  st_HWEth_Statistics_State *status = (st_HWEth_Statistics_State *) data;

  /* Update all port counters */
  for (port=0; port<SYSTEM_N_PORTS; port++)  {
    monitor_update_stats(port);
  }

  osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  for (port=0; port<SYSTEM_N_PORTS; port++)  {
    status->status_value[port] = 0;
    if ((status->status_mask>>BIT_RX_DROPPACKETS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsDropEvents>0)<<BIT_RX_DROPPACKETS;
    if ((status->status_mask>>BIT_RX_UNDERSIZEPACKETS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsUndersizePkts>0)<<BIT_RX_UNDERSIZEPACKETS;
    if ((status->status_mask>>BIT_RX_OVERSIZEPACKETS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsOversizePkts>0)<<BIT_RX_OVERSIZEPACKETS;
    if ((status->status_mask>>BIT_RX_FRAGMENTS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsFragments>0)<<BIT_RX_FRAGMENTS;
    if ((status->status_mask>>BIT_RX_JABBERS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsJabbers>0)<<BIT_RX_JABBERS;
    if ((status->status_mask>>BIT_RX_CRC_ERRORS) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.etherStatsCRCAlignErrors>0)<<BIT_RX_CRC_ERRORS;
    if ((status->status_mask>>BIT_TX_COLLISIONS) & 1)
      status->status_value[port] |= (monitor[port].stats.Tx.etherStatsCollisions>0)<<BIT_TX_COLLISIONS;
    if ((status->status_mask>>BIT_TX_ACTIVITY) & 1)
      status->status_value[port] |= (monitor[port].stats.Tx.Throughput>0)<<BIT_TX_ACTIVITY;
    if ((status->status_mask>>BIT_RX_ACTIVITY) & 1)
      status->status_value[port] |= (monitor[port].stats.Rx.Throughput>0)<<BIT_RX_ACTIVITY;  
  }
  osapiSemaGive(sem_monitor_access);

  return L7_SUCCESS;
}

