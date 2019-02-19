#include <unistd.h>
#include "ptin_interface.h"
#include "usmdb_nim_api.h"
#include "nimapi.h"
#include "usmdb_util_api.h"
#include "fw_shm.h"
#include "traces.h"
#include "dot1s_api.h"

L7_uint32 qos_prio_map[L7_SYSTEM_N_INTERF][8];

L7_BOOL ptin_interface_qos_valid(L7_uint32 port, L7_uint32 *qos_list)
{
  L7_uint32 rules = 0;
  L7_uint32 i, j;
  L7_uint64 interface_check = 0;
  static L7_int first_time  = L7_TRUE;

  /* Initialize static structures */
  if (first_time)
  {
    for (i=0; i<L7_SYSTEM_N_INTERF; i++)
    {
      for (j=0; j<8; j++)
      {
        qos_prio_map[i][j]=j;
      }
    }
    first_time = L7_FALSE;
  }

  #if 0
  if (port<L7_SYSTEM_N_INTERF && qos_list)
  {
    printf("Port ref %u: %02u %02u %02u %02u %02u %02u %02u %02u\r\n",port,
           qos_list[0],qos_list[1],qos_list[2],qos_list[3],qos_list[4],qos_list[5],qos_list[6],qos_list[7]);
  }
  for (i=0; i<L7_SYSTEM_N_INTERF; i++)
  {
    printf("Port %02u :",i);
    for (j=0; j<8; j++)
    {
      printf(" %02u",qos_prio_map[i][j]);
    }
    printf("\r\n");
  }
  #endif

  /* If new list is provided, start using it for rules calculation */
  if (qos_list!=L7_NULLPTR)
  {
    /* First 8 rules */
    rules += 8;

    /* Run all other interfaces, and exclude the ones with the same qos list */
    for (j=0; j<L7_SYSTEM_N_INTERF; j++)
    {
      if (port==j)  continue;

      /* If this interface as the same qos list, check it, to not be used again */
      if (memcmp(qos_list,qos_prio_map[j],sizeof(L7_uint32)*8)==0)
      {
        interface_check |= (L7_uint64) 1<<j;
      }
    }
  }

  /* Calculate number of necessary rules */
  for (i=0; i<L7_SYSTEM_N_INTERF; i++)
  {
    if (port==i)  continue;

    /* Was this interface previously used for comparison? */
    if ((interface_check>>i) & 1)  continue;

    /* interface checked */
    interface_check |= (L7_uint64) 1<<i;

    /* More 8 rules for this qos list */
    rules += 8;

    /* Mark for exclusion, other interface lists of same content */
    for (j=i+1; j<L7_SYSTEM_N_INTERF; j++)
    {
      if (port==j)  continue;

      if ((interface_check>>j) & 1)  continue;

      /* If this interface as the same qos list, check it, to not be used again */
      if (memcmp(qos_prio_map[i],qos_prio_map[j],sizeof(L7_uint32)*8)==0)
      {
        interface_check |= (L7_uint64) 1<<j;
      }
    }
  }

  /* Do not allow more than 48 rules */
  if (rules>40)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: Excessive resources... %u rules needes!\n", __FUNCTION__,rules);
    return L7_FALSE;
  }

  /* Save new list */
  if (port<L7_SYSTEM_N_INTERF && qos_list!=L7_NULLPTR)
  {
    memcpy(qos_prio_map[port],qos_list,sizeof(L7_uint32)*8);
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: %u rules needes!\n", __FUNCTION__,rules);
  return L7_TRUE;
}


L7_RC_t ptin_phy_config_set(L7_uint8 port, L7_uint8 enable, L7_uint8 speed, L7_uint8 autoneg, L7_uint8 full_duplex, L7_uint16 frame_max, L7_uint8 lb, L7_uint8 macLearn)
{
  L7_BOOL valueb;
  L7_uint32 valued;
  L7_uint32 interface;
  L7_uint32 speed_mode;

  // Get interface ID
  if (usmDbIntIfNumFromUSPGet(1, 0, port+1, &interface) != L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%d\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Interface aquired (port %u): %u",__FUNCTION__,port,interface );

  // Enable port
  if (enable!=(L7_uint8) -1 && usmDbIfAdminStateGet(1, interface, &valued)==L7_SUCCESS && valued!=(enable & 1))  {
    if (usmDbIfAdminStateSet(1, interface, enable & 1)) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_enable_set failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }
    // Update shared memory
    pfw_shm->intf[port].admin = enable & 1;
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Enable %u applied",__FUNCTION__,enable);
  }

  // Switch on/off laser
//if (dtlPtinPhySet(interface,!enable)!=L7_SUCCESS) {
//  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: dtlPtinPhySet failed in port %u\n", __FUNCTION__,port);
//  return L7_FAILURE;
//}

  // Frame max
  if (frame_max!=(L7_uint16) -1 && usmDbIfConfigMaxFrameSizeGet(interface,&valued)==L7_SUCCESS && valued!=frame_max)  {
    if (usmDbIfConfigMaxFrameSizeSet(interface, frame_max)) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_frame_max_set failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Maxframe %u applied",__FUNCTION__,frame_max);
  }

  // loopback
  if (lb!=(L7_uint8) -1 && usmDbIfLoopbackModeGet(interface,&valued)==L7_SUCCESS && valued!=(lb!=0))  {
    if (usmDbIfLoopbackModeSet(interface,(lb!=0))) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_loopback_set failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Loopback %u applied",__FUNCTION__,lb);
  }

  // Speed
  if (speed!=(L7_uint8) -1)  {
    switch (speed)  {
    case PHY_PORT_1000_MBPS:
      speed_mode=L7_PORTCTRL_PORTSPEED_FULL_1000SX;
      break;
    case PHY_PORT_2500_MBPS:
      speed_mode=L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
      break;
    case PHY_PORT_10000_MBPS:
      speed_mode=L7_PORTCTRL_PORTSPEED_FULL_10GSX;
      break;
    default:
      speed_mode = L7_PORTCTRL_PORTSPEED_UNKNOWN;
    }
    if (speed_mode!=L7_PORTCTRL_PORTSPEED_UNKNOWN && usmDbIfSpeedGet(1,interface,&valued)==L7_SUCCESS && valued!=speed_mode) {
      if (usmDbIfSpeedSet(1,interface,speed_mode))  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_speed_set failed in port %u\n", __FUNCTION__,port);
        return L7_FAILURE;
      }
      DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: Speed %u applied",__FUNCTION__,speed);
    }
  }

  // Define MAC learning
  if (macLearn!=(L7_uint8)-1 && dot1sPortMacLearningGet(interface,&valueb)==L7_SUCCESS && valueb!=(macLearn & 1)) {
    if (dot1sPortMacLearningSet(interface,(macLearn & 1))!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: dot1sPortMacLearningSet failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: MacLearn %u applied",__FUNCTION__,macLearn);
  }

  // Always clear counters after a reconfiguration
  if ( ptin_clearCounters(port) )  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm56580_clearCounters failed in port %u\n", __FUNCTION__,port);
    return L7_FAILURE;
  }

  // Wait for changes to be applied
  do {
    usleep(1000);
  } while (!nimNotifyIntf_done());

  return L7_SUCCESS;
}

// Configure a single phy port
//
L7_RC_t ptin_phy_config_get(L7_uint8 port, L7_uint8 *enable, L7_uint8 *speed, L7_uint8 *autoneg, L7_uint8 *full_duplex, L7_uint16 *frame_max, L7_uint8 *lb, L7_uint8 *macLearn)
{
  L7_uint32 value;
  L7_uint32 interface;
  L7_uint32 speed_mode;
  L7_BOOL   learn;

  // Get interface ID
  if (usmDbIntIfNumFromUSPGet(1, 0, port+1, &interface) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  // Enable port
  if (enable!=NULL)  {
    if (usmDbIfAdminStateGet(1, interface, &value)) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_enable_get failed in port %u\n", __FUNCTION__,port);
      *enable=0;
    } else *enable=(L7_uint8) value;
  }

  // Frame max
  if (frame_max!=NULL)  {
    if (usmDbIfConfigMaxFrameSizeGet(interface, &value))  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_frame_max_get failed in port %u\n", __FUNCTION__,port);
      *frame_max=0;
    } else *frame_max=(L7_uint16) value;
  }

  // loopback
  if (lb!=NULL)  {
    if (usmDbIfLoopbackModeGet(interface,&value))  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_loopback_get failed in port %u\n", __FUNCTION__,port);
      *lb=0;
    } else *lb = (value!=0);
  }

  // Speed
  if (speed!=NULL)  {
    if (usmDbIfSpeedGet(1,interface,&speed_mode))  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: bcm_port_speed_get failed in port %u\n", __FUNCTION__,port);
      speed_mode=(L7_uint32)-1;
    }
    switch (speed_mode)  {
    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      *speed=PHY_PORT_1000_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
      *speed=PHY_PORT_2500_MBPS;
      break;
    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      *speed=PHY_PORT_10000_MBPS;
      break;
    default:
      *speed=(L7_uint8)-1;
    }
  }
  // Autoneg feature
  if (autoneg!=NULL)  {
    *autoneg=0;
  }
  // Full-duplex feature
  if (full_duplex!=NULL)  {
    *full_duplex=1;
  }

  // Read MAC learning state
  if (macLearn!=NULL) {
    if (dot1sPortMacLearningGet(interface,&learn)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: dot1sPortMacLearningGet failed in port %u\n", __FUNCTION__,port);
      return L7_FAILURE;
    }
    else {
      *macLearn = learn;
    }
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_phy_state_get(L7_uint8 port, L7_uint8 *linkup, L7_uint8 *autoneg_complete)
{
  L7_uint32 value;
  L7_uint32 interface;

  // Get interface ID
  if (usmDbIntIfNumFromUSPGet(1, 0, port+1, &interface) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  // Link status
  if (linkup!=NULL) {
    //if (usmDbIfActiveStateGet(1,interface,&value))  return L7_FAILURE;
    if (nimGetIntfLinkState(interface,&value)!=L7_SUCCESS)  return L7_FAILURE;
    *linkup = (value==L7_UP);
  }
  // Auto-negotiation completion
  if (autoneg_complete!=NULL) {
    *autoneg_complete=1;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_readCounters(L7_HWEthRFC2819_PortStatistics *stat)
{
  return dtlPtinGetCounters( stat );
}

L7_RC_t ptin_clearCounters( L7_uint32 port )
{
  L7_HWEthRFC2819_PortStatistics stat;

  memset(&stat,0x00,sizeof(L7_HWEthRFC2819_PortStatistics));

  stat.Port = port;
  stat.Mask = 0xFF;
  stat.RxMask = 0xFFFF;
  stat.TxMask = 0xFFFF;

  return dtlPtinClearCounters( &stat );
}

L7_RC_t dtlPTinIntfCallback(DAPI_USP_t *ddusp, 
                          DAPI_FAMILY_t family, 
                          DAPI_CMD_t cmd, 
                          DAPI_EVENT_t event,
                          void *dapiEventInfo)
{
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Slot=%d, Port=%d with link %u\n", __FUNCTION__, __LINE__,ddusp->slot,ddusp->port,event);

  // Process only learned or aged addresses
  if (event!=DAPI_EVENT_INTF_LINK_UP && event!=DAPI_EVENT_INTF_LINK_DOWN) {
    return L7_SUCCESS;
  }

  if (ddusp->slot!=0 && ddusp->port>=L7_SYSTEM_N_PORTS)
    return L7_SUCCESS;
  
  // Update shared memory
  pfw_shm->intf[ddusp->port].link = (event==DAPI_EVENT_INTF_LINK_UP);

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Port %d with link %u\n", __FUNCTION__, __LINE__,ddusp->port,pfw_shm->intf[ddusp->port].link);

  return L7_SUCCESS;
}



