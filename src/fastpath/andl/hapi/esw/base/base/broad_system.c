/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_system.c
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
#include <math.h>
#include "sysapi.h"
#include "osapi.h"
#include "dtl_exports.h"

#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_led.h"
#include "broad_l2_vlan.h"
#include "broad_l2_lag.h"
#include "phy_hapi.h"
#include "broad_policy.h"
#include "broad_debug.h"
#include "broad_llpf.h"
#include "broad_ptin.h"     /* PTin added: DAPI */

#include "bcm/port.h"
#include "bcm/l2.h"
#include "bcm/rate.h"
#include "bcm/mirror.h"
/* PTin modified: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcm/filter.h"
#endif
#include "soc/cmic.h"
#include "soc/drv.h"
#include "soc/macipadr.h"
#include "soc/mem.h"
#include "soc/cm.h"
#include "soc/robo.h"

#include "bcmx/l2.h"
#include "bcmx/port.h"
#include "bcmx/lport.h"
#include "bcmx/mirror.h"
#include "bcmx/rate.h"
/* PTin removed: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcmx/filter.h"
#endif
#include "bcmx/switch.h"
#include "bcmx/bcmx_int.h"
/* TODO: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
//#include "soc/ea/tk371x/igmp.h"
#else
#include "bcmx/igmp.h"
#endif
#include "bcmx/custom.h"
#include "bcm_int/esw/mbcm.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_api.h"
#include "l7_usl_bcmx_port.h"
#include "sysbrds.h"
#include "osapi_support.h"
#include "unitmgr_api.h"
#include "broad_mmu.h"
#include "platform_config.h"
#include "l7_usl_port_db.h"

#include "broad_group_bcm.h"

/* PTin added: includes */
#if 1
#include "logger.h"
#endif

#ifdef PC_LINUX_HOST
#include "bcmx/bcmx_int.h"
  extern int _bcm_esw_link_force(int unit, bcm_port_t port, int force, int link);
#endif
L7_BOOL hapiBroadRavenCheck ( );


extern L7_RC_t hapiBroadRoboDoubleVlanDenyRule(DAPI_t *dapi_g,L7_BOOL enableFilter);
/* cable diagnostic debugging */
static L7_BOOL debugCableDiag = L7_FALSE;

#define PFC_ENABLED 1
#define PFC_DISABLED 2
#define PAUSE_ENABLED 3
#define PAUSE_DISABLED 4

static int flow_control_count = 0;
static int pfc_count = 0;
static int pfc_max_pg = 0;

extern DAPI_t *dapi_g;


void hapiBroadDebugCableDiag(L7_BOOL flag)
{
  debugCableDiag = flag;
}

/*********************************************************************
*
* @purpose utility to determine whether we should be ingress or egress based
*
* @param    action   Whether or not pfc or pause is enabled or disabled
* @returns  MMU_DROP_MODE_INGRESS if there is at least one ingress based port 
  @returns  MMU_DROP_MODE_EGRESSS if there are not ingress based ports
*
* @notes   none
*
* @end
*
*********************************************************************/
int hapiBroadDropModeCheck(int action)
{
  int mode;

  switch(action)
  {
    case PFC_ENABLED:
      pfc_count++;
      break;
    case PFC_DISABLED:
      pfc_count--;
      break;
    case PAUSE_ENABLED:
      flow_control_count++;
      break;
    case PAUSE_DISABLED:
      flow_control_count--;
      break;
    default:
      break;
  }
  
  if (pfc_count < 0) pfc_count=0; 
  if (flow_control_count < 0) flow_control_count=0;

  if (pfc_count || flow_control_count) 
    mode = MMU_DROP_MODE_INGRESS;
  else
    mode = MMU_DROP_MODE_EGRESS;

  return mode;
}


/*********************************************************************
*
* @purpose Hooks in basic dapi cmds
*
* @param   DAPI_PORT_t *dapiPortPtr - used to hook in commands for a port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadStdPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_CONTROL_HW_APPLY_NOTIFY           ] = (HAPICTLFUNCPTR_t)hapiBroadControlHwApplyNotify;
  dapiPortPtr->cmdTable[DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY         ] = (HAPICTLFUNCPTR_t)hapiBroadControlUnitStatusNotify;
  dapiPortPtr->cmdTable[DAPI_CMD_CARD_PLUGIN                        ] = (HAPICTLFUNCPTR_t)hapiBroadCardPluginNotify;

  dapiPortPtr->cmdTable[DAPI_CMD_CARD_UNPLUG                        ] = (HAPICTLFUNCPTR_t)hapiBroadCardUnplugNotify;

  dapiPortPtr->cmdTable[DAPI_CMD_FRAME_SEND                        ] = (HAPICTLFUNCPTR_t)hapiBroadSend;

  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_MIRRORING                  ] = (HAPICTLFUNCPTR_t)hapiBroadSystemMirroring;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_MIRRORING_PORT_MODIFY      ] = (HAPICTLFUNCPTR_t)hapiBroadSystemMirroringPortModify;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS          ] = (HAPICTLFUNCPTR_t)hapiBroadSystemIpAddress;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS         ] = (HAPICTLFUNCPTR_t)hapiBroadSystemMacAddress;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET ] = (HAPICTLFUNCPTR_t)hapiBroadSystemBroadcastControlModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_FLOW_CONTROL               ] = (HAPICTLFUNCPTR_t)hapiBroadSystemFlowControl;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_SNOOP_CONFIG               ] = (HAPICTLFUNCPTR_t)hapiBroadSystemSnoopConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG          ] = (HAPICTLFUNCPTR_t)hapiBroadSystemDosControlConfig;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOSCONTROL_CONFIG            ] = (HAPICTLFUNCPTR_t)hapiBroadIntfDosControlConfig;  
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_SPEED_CONFIG                 ] = (HAPICTLFUNCPTR_t)hapiBroadIntfSpeedConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG        ] = (HAPICTLFUNCPTR_t)hapiBroadIntfAutoNegotiateConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_LOOPBACK_CONFIG              ] = (HAPICTLFUNCPTR_t)hapiBroadIntfLoopbackConfig;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_ISOLATE_PHY_CONFIG           ] = (HAPICTLFUNCPTR_t)hapiBroadIntfIsolatePhyConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_STP_STATE                    ] = (HAPICTLFUNCPTR_t)hapiBroadIntfStpState;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG        ] = (HAPICTLFUNCPTR_t)hapiBroadIntfMaxFrameSizeConfig;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET   ] = (HAPICTLFUNCPTR_t)hapiBroadIntfBroadcastControlModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1S_BPDU_FILTERING         ] = (HAPICTLFUNCPTR_t)hapiBroadIntfBpduFiltering;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1S_BPDU_GUARD             ] = (HAPICTLFUNCPTR_t)hapiBroadIntfBpduGuard;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_DOT1S_BPDU_FLOOD             ] = (HAPICTLFUNCPTR_t)hapiBroadIntfBpduFlood;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_CABLE_STATUS_GET             ] = (HAPICTLFUNCPTR_t)hapiBroadIntfCableTest;

  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY        ] = (HAPICTLFUNCPTR_t)hapiBroadSystemCpuSamplePriority;
  dapiPortPtr->cmdTable[DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED         ] = (HAPICTLFUNCPTR_t)hapiBroadSystemSampleRandomSeed;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_SAMPLE_RATE                  ] = (HAPICTLFUNCPTR_t)hapiBroadIntfSampleRate;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_FIBER_DIAG_GET               ] = (HAPICTLFUNCPTR_t)hapiBroadIntfFiberDiagTest;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_BLINK_SET                    ] = (HAPICTLFUNCPTR_t)hapiBroadIntfLedBlinkSet;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_PFC_CONFIG                   ] = (HAPICTLFUNCPTR_t)hapiBroadIntfPfcConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_PFC_STATS_GET                ] = (HAPICTLFUNCPTR_t)hapiBroadIntfPfcStatGet;
  dapiPortPtr->cmdTable[DAPI_CMD_INTF_PFC_STATS_CLEAR              ] = (HAPICTLFUNCPTR_t)hapiBroadIntfPfcStatsClear;
  dapiPortPtr->cmdTable[DAPI_CMD_ISDP_INTF_STATUS_SET              ] = (HAPICTLFUNCPTR_t)hapiBroadIsdpStatusSet;

  /* PTin added: DAPI */
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_INIT                         ] = (HAPICTLFUNCPTR_t)hapiBroadPtinInit;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_PORTEXT                      ] = (HAPICTLFUNCPTR_t)hapiBroadPtinPortExt;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_COUNTERS_READ                ] = (HAPICTLFUNCPTR_t)hapiBroadPtinCountersRead;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_COUNTERS_CLEAR               ] = (HAPICTLFUNCPTR_t)hapiBroadPtinCountersClear;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET        ] = (HAPICTLFUNCPTR_t)hapiBroadPtinCountersActivityGet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_SETTINGS                ] = (HAPICTLFUNCPTR_t)hapiBroadPtinVlanModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_DEFS                    ] = (HAPICTLFUNCPTR_t)hapiBroadPtinBridgeVlanModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_PORT_CONTROL            ] = (HAPICTLFUNCPTR_t)hapiBroadPtinBridgeVlanPortControl;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_XLATE                   ] = (HAPICTLFUNCPTR_t)hapiBroadPtinVlanTranslate;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP  ] = (HAPICTLFUNCPTR_t)hapiBroadPtinVlanTranslatePortGroups;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_CROSSCONNECT            ] = (HAPICTLFUNCPTR_t)hapiBroadPtinBridgeCrossconnect;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VLAN_MULTICAST               ] = (HAPICTLFUNCPTR_t)hapiBroadPtinBridgeVlanMulticastSet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_MULTICAST_EGRESS_PORT        ] = (HAPICTLFUNCPTR_t)hapiBroadPtinMulticastEgressPortSet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_VIRTUAL_PORT                 ] = (HAPICTLFUNCPTR_t)hapiBroadPtinVirtualPortSet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_HW_RESOURCES                 ] = (HAPICTLFUNCPTR_t)hapiBroadPtinResourcesGet;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_BW_POLICER                   ] = (HAPICTLFUNCPTR_t)hapiBroadPtinBwPolicer;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_FP_COUNTERS                  ] = (HAPICTLFUNCPTR_t)hapiBroadPtinFpCounters;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_STORM_CONTROL                ] = (HAPICTLFUNCPTR_t)hapiBroadStormControl;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU          ] = (HAPICTLFUNCPTR_t)hapiBroadSystemPacketTrapConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_PCS_PRBS                     ] = (HAPICTLFUNCPTR_t)hapiBroadSystemPTinPrbs;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_SLOT_MODE                    ] = (HAPICTLFUNCPTR_t)hapiBroadPtinSlotMode;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_HW_PROCEDURE                 ] = (HAPICTLFUNCPTR_t)hapiBroadHwApply;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_L3                           ] = (HAPICTLFUNCPTR_t)hapiBroadPtinL3Manage;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_MEP_CTRL                     ] = (HAPICTLFUNCPTR_t)hapiBroadPtinMEPControl;
  dapiPortPtr->cmdTable[DAPI_CMD_PTIN_GENERIC                      ] = (HAPICTLFUNCPTR_t)hapiBroadPtinGeneric;
  /* PTin end */

  return result;

}

/*********************************************************************
*
* @purpose Enable mirroring on the stack.
*
* @param   none
*
* @returns none
*
* @notes   This function is called every time a port is added to 
*          a mirror group in case the new port is located on a unit
*          which hasn't been enabled for miorroring yet.
*
* @end
*
*********************************************************************/
void hapiBroadMirrorEnable (void)
{
  int rv;
  bcm_chip_family_t board_family;

  hapiBroadGetSystemBoardFamily(&board_family);

  if(board_family == BCM_FAMILY_ROBO)
  {
    rv = bcmx_mirror_mode_set(BCM_MIRROR_L2);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }

    /* TODO: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    rv = BCM_E_NONE;    /* Always enabled */
    #else
    rv = bcmx_mirror_pfmt_set(1);
    #endif
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    }
  }
}


static void hapiBroadMirrorDisable (void)
{
  int rv;
  bcm_chip_family_t board_family;

  hapiBroadGetSystemBoardFamily(&board_family);

  if(board_family == BCM_FAMILY_ROBO)
  {
    rv = bcmx_mirror_mode_set(BCM_MIRROR_DISABLE);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }
  }
}

/*********************************************************************
*
* @purpose Check if the hardware/device supports TRUE egress mirroring
*
* @param   none
*
* @notes   This function relies on 'soc_feature_egr_mirror_true' feature
*          to determine whether TRUE egress mirroring is supported or
*          not. This way, we don't have to add/track new devices/family
*          in future. THIS FUNCTION CHECKS LOCAL DEVICES ONLY.
*
* @returns L7_BOOL L7_TRUE is supported, else L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL hapiBroadSystemTrueEgrMirroring(void)
{
#if defined (BCM_TRIUMPH2_SUPPORT) || defined (BCM_TRIUMPH3_SUPPORT)
  L7_short16 bcm_unit;
   
  for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
  {
    if (soc_feature(bcm_unit, soc_feature_egr_mirror_true))
    {
      return L7_TRUE;
    }
  }
#endif

  return L7_FALSE;
}


/*********************************************************************
*
* @purpose Configures source and probe ports
*
* @param   DAPI_USP_t *fromUsp    - identifies the source port
* @param   DAPI_USP_t *toUsp      - identifies the probe port
* @param   DAPI_t     *dapi_g     - the driver object
* @param   L7_BOOL    add         - action (add/remove)
* @param   L7_uint32  probeType   - specifies mirroring mode (ingress/egress/both)
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroringSet(DAPI_USP_t *fromUsp,
                                    DAPI_USP_t *toUsp,
                                    DAPI_t *dapi_g,
                                    L7_BOOL add,
                                    L7_uint32 probeType)
{

  L7_RC_t               result = L7_SUCCESS;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiMirrorFromPortPtr;
  BROAD_PORT_t          *hapiMirrorToPortPtr;
  bcmx_lport_t          bcmxMirrorFromLport;
  bcmx_lport_t          bcmxMirrorToLport, lport;
  BROAD_PORT_t          *hapiLagMemberPortPtr;
  int                              rv, i;
  usl_bcm_port_mirror_config_t     mirrorConfig;

  if (!isValidUsp(fromUsp, dapi_g))
    return L7_FAILURE;

  memset(&mirrorConfig, 0, sizeof(mirrorConfig));
 
  dapiPortPtr = DAPI_PORT_GET(fromUsp, dapi_g);
  hapiMirrorFromPortPtr = HAPI_PORT_GET(fromUsp, dapi_g);
  hapiMirrorToPortPtr = HAPI_PORT_GET(toUsp, dapi_g);

  bcmxMirrorFromLport = hapiMirrorFromPortPtr->bcmx_lport;
  bcmxMirrorToLport = hapiMirrorToPortPtr->bcmx_lport;

  mirrorConfig.probePort = hapiMirrorToPortPtr->bcmx_lport;
  mirrorConfig.stackUnit = toUsp->unit;

  if (add == L7_TRUE)
  {
    mirrorConfig.flags = BCM_MIRROR_PORT_ENABLE;
    switch (probeType)
    {
    case DAPI_MIRROR_INGRESS: 
      mirrorConfig.flags |= BCM_MIRROR_PORT_INGRESS;
      break;

    case DAPI_MIRROR_EGRESS:  
      if (hapiBroadSystemTrueEgrMirroring() == L7_TRUE)
      {
        mirrorConfig.flags |= BCM_MIRROR_PORT_EGRESS_TRUE;
      }
      else
      {
        mirrorConfig.flags |= BCM_MIRROR_PORT_EGRESS;
      }
      break;

    default:
      if (hapiBroadSystemTrueEgrMirroring() == L7_TRUE)
      {
        mirrorConfig.flags |= BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS_TRUE;
      }
      else
      {
        mirrorConfig.flags |= BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS;
      }
      break;
    }   
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      hapiBroadLagCritSecEnter ();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
          lport = hapiLagMemberPortPtr->bcmx_lport;

          rv = usl_bcmx_port_mirror_set(lport, mirrorConfig);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            hapiBroadLagCritSecExit ();
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "Failed to apply port mirroring configuration (enable from = NA, to = %d, rv %d)\n",
                    bcmxMirrorToLport, rv);

            return L7_FAILURE;
          }
        }
      }
      hapiBroadLagCritSecExit ();
    }
    else
    {
      rv = usl_bcmx_port_mirror_set(bcmxMirrorFromLport, mirrorConfig);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "Failed to apply port mirroring configuration (enable from = %d, to = %d, rv %d)\n",
                bcmxMirrorFromLport, bcmxMirrorToLport, rv);

        return L7_FAILURE;
      }
    }
  }
  else /*disable mirroring*/
  {
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      hapiBroadLagCritSecEnter ();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

          lport = hapiLagMemberPortPtr->bcmx_lport;
          rv = usl_bcmx_port_mirror_set(lport, mirrorConfig);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            hapiBroadLagCritSecExit ();
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "Failed to apply port mirroring configuration (enable from = NA, to = %d, rv %d)\n",
                    bcmxMirrorToLport, rv);

            return L7_FAILURE;
          }
        }
      }
      hapiBroadLagCritSecExit ();
    }
    else
    {
      rv = usl_bcmx_port_mirror_set(bcmxMirrorFromLport, mirrorConfig);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        return L7_FAILURE;
      }
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Implements mirroring
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_MIRRORING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.mirroring
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadSystemMirroring(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t              result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t   *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  L7_int32             i, add; 
  BROAD_PORT_t        *hapiMirrorToPortPtr;
  bcmx_lport_t         bcmxMirrorToLport;
  int                  rv;
  usl_bcm_port_filter_mode_t  mode;
  DAPI_PORT_t         *dapiPortPtr;
  DAPI_USP_t           cpuUsp;

  if (dapiCmd->cmdData.mirroring.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (CPU_USP_GET(&cpuUsp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (dapiCmd->cmdData.mirroring.probeSpecified == L7_TRUE)
  {
     dapi_g->system->probeUsp.unit = dapiCmd->cmdData.mirroring.probeUsp.unit;
     dapi_g->system->probeUsp.slot = dapiCmd->cmdData.mirroring.probeUsp.slot;
     dapi_g->system->probeUsp.port = dapiCmd->cmdData.mirroring.probeUsp.port;
  }

  add = dapiCmd->cmdData.mirroring.enable;
  /* If a valid probe port is defined, then setup mirroring for each source port */
  if(isValidUsp(&(dapi_g->system->probeUsp), dapi_g))
  {
    for (i = 0; i < dapiCmd->cmdData.mirroring.numMirrors; i++)
    {
      /* The CPU port as a src port is mirrored in software, so no need to configure
         HW in this case. */
      if ((cpuUsp.unit == dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].srcUsp.unit) &&
          (cpuUsp.slot == dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].srcUsp.slot) &&
          (cpuUsp.port == dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].srcUsp.port))
      {
        result = L7_SUCCESS;
      }
      else
      {
        result = hapiBroadSystemMirroringSet(&(dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].srcUsp),
                                             &(dapi_g->system->probeUsp),
                                             dapi_g, add, 
                                             dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].probeType);
      }
      if (result == L7_SUCCESS)
      {
        dapiPortPtr = 
           DAPI_PORT_GET(&(dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].srcUsp), dapi_g);
        dapiPortPtr->isMirrored = dapiCmd->cmdData.mirroring.enable;
        dapiPortPtr->mirrorType = dapiCmd->cmdData.mirroring.mirrorProbeUsp[i].probeType;
      }
    }
  }
  else
  {
    result = L7_FAILURE;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "\n%s %d: In %s, invalid probe port : %d:%d:%d\n",
                   __FILE__, __LINE__, __FUNCTION__,
                   dapi_g->system->probeUsp.unit,
                   dapi_g->system->probeUsp.slot,
                   dapi_g->system->probeUsp.port);
    return result;
  }

  /* Setup the probe port - one time configuration. When multiple source ports
   * are added, we just have to set the source port as only one probe port 
   * is supported
   */
  hapiMirrorToPortPtr = HAPI_PORT_GET(&(dapi_g->system->probeUsp), dapi_g);
  bcmxMirrorToLport = hapiMirrorToPortPtr->bcmx_lport;
  if((add == L7_TRUE) && (result == L7_SUCCESS))
  {
     hapiBroadMirrorEnable ();

     mode.flags = BCM_PORT_VLAN_MEMBER_INGRESS;
     mode.setFlags = L7_TRUE;
     rv = usl_bcmx_port_vlan_member_set(bcmxMirrorToLport, mode);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }

     /* Disable egress filtering. The mirrored packets may not be on the
      * same vlan as the probe port */
     mode.flags = BCM_PORT_VLAN_MEMBER_EGRESS;
     mode.setFlags = L7_FALSE;
     rv = usl_bcmx_port_vlan_member_set(bcmxMirrorToLport, mode);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }


     rv = usl_bcmx_port_untagged_vlan_set(bcmxMirrorToLport, HPC_STACKING_VLAN_ID);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }
 
     /*Session can be active only with an active probe*/
     dapi_g->system->mirrorEnable = L7_TRUE;
  }
  else if((add == L7_FALSE) && (result == L7_SUCCESS))
  {
     result = hapiBroadVlanIngressFilterSet(bcmxMirrorToLport,hapiMirrorToPortPtr->ingressFilteringEnabled);

     if (result != L7_SUCCESS)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }

     /* Re-enable egress filtering. */
     mode.flags = BCM_PORT_VLAN_MEMBER_EGRESS;
     mode.setFlags = L7_TRUE;
     rv = usl_bcmx_port_vlan_member_set(bcmxMirrorToLport, mode);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }


     rv = usl_bcmx_port_untagged_vlan_set(bcmxMirrorToLport,hapiMirrorToPortPtr->pvid);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       LOG_ERROR(bcmxMirrorToLport);
     }

     dapi_g->system->mirrorEnable = L7_FALSE;
     dapi_g->system->probeUsp.unit = 0;
     dapi_g->system->probeUsp.slot = 0;
     dapi_g->system->probeUsp.port = 0;

     hapiBroadMirrorDisable();
  }

  /* Upon failure, the error is logged.
   * Return quietly so that we don't cause the device
   * to reboot */
  return L7_SUCCESS; 
}

/*********************************************************************
*
* @purpose Modifies mirrored ports list
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_MIRRORING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.mirroringPortModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadSystemMirroringPortModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t              result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t   *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  DAPI_PORT_t         *dapiPortPtr;
  DAPI_USP_t           cpuUsp;

  if (dapiCmd->cmdData.mirroringPortModify.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (CPU_USP_GET(&cpuUsp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* If a valid probe port is defined, then setup mirroring for the given source port */
  if(isValidUsp(&(dapi_g->system->probeUsp), dapi_g))
  {
    /* Enable port mirroring in case it isn't already enabled on all units.
    */
    hapiBroadMirrorEnable ();
  
    /* The CPU port as a src port is mirrored in software, so no need to configure
       HW in this case. */
    if ((cpuUsp.unit != dapiCmd->cmdData.mirroringPortModify.mirrorUsp.unit) ||
        (cpuUsp.slot != dapiCmd->cmdData.mirroringPortModify.mirrorUsp.slot) ||
        (cpuUsp.port != dapiCmd->cmdData.mirroringPortModify.mirrorUsp.port))
    {
      (void) hapiBroadSystemMirroringSet(&(dapiCmd->cmdData.mirroringPortModify.mirrorUsp),
                                           &(dapi_g->system->probeUsp),
                                           dapi_g, 
                                           dapiCmd->cmdData.mirroringPortModify.add, 
                                           dapiCmd->cmdData.mirroringPortModify.probeType);
    }

    dapiPortPtr = DAPI_PORT_GET(&(dapiCmd->cmdData.mirroringPortModify.mirrorUsp), dapi_g);
    dapiPortPtr->isMirrored = dapiCmd->cmdData.mirroringPortModify.add;
    dapiPortPtr->mirrorType = dapiCmd->cmdData.mirroringPortModify.probeType;

  }

  /* Upon failure, the error is logged.
   * Return quietly so that we don't cause the device
   * to reboot */
  return L7_SUCCESS;
}                        

/*********************************************************************
*
* @purpose Returns Probe Status Active/Inactive 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadSystemMirroringUSPIsProbe(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_BOOL           result = L7_FALSE;
  BROAD_PORT_t     *hapiMirrorToPortPtr;
  BROAD_PORT_t     *hapiPortPtr;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  if(isValidUsp(&dapi_g->system->probeUsp, dapi_g) && dapi_g->system->mirrorEnable)
  {
      hapiMirrorToPortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp, dapi_g);
      if(hapiMirrorToPortPtr->bcmx_lport == hapiPortPtr->bcmx_lport)
      {
          result = L7_TRUE;
      }
  }

  return result;
}

/*********************************************************************
*
* @purpose Sets the IP address
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.systemIpAddress
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Used for L2 ONLY PRODUCTS
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemIpAddress(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t         result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;

  dapiCmd = dapiCmd;

  /* set the management IP address of the switch in the NP */

  return result;

}



/*********************************************************************
*
* @purpose Adds the System MAC address
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp 
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.systemMacAddress
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMacAddress(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t            *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  BROAD_SYSTEM_t               *hapiSystemPtr;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_USP_t                    cpuUsp;
  bcmx_l2_addr_t                l2Addr;
  L7_int32                      rc=0;
  bcm_mac_t                     mgmtMac;

  if ((dapiCmd->cmdData.systemMacAddress.getOrSet != DAPI_CMD_SET) &&
      (dapiCmd->cmdData.systemMacAddress.getOrSet != DAPI_CMD_CLEAR))
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (dapiCmd->cmdData.systemMacAddress.getOrSet == DAPI_CMD_CLEAR)
  {
    /* remove previous system MAC address / mgmt vlan id, if configured */    
#ifndef BCM_ROBO_SUPPORT
  hapiBroadFfpSysMacInstall(dapi_g, 0, L7_NULLPTR);
#else
  if (hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
  {
    hapiBroadFfpSysMacInstall(dapi_g, 0, L7_NULLPTR);
  }
#endif

#ifdef L7_DOT1AG_PACKAGE
   hapiBroadDot1agPolicyInstall(dapi_g, L7_NULLPTR);
#endif

    if (hapiSystemPtr->mgmtVlanId != 0)
    {
      memcpy(mgmtMac, hapiSystemPtr->bridgeMacAddr.addr, sizeof (bcm_mac_t)); 

      rc = usl_bcmx_l2_addr_delete(mgmtMac, hapiSystemPtr->mgmtVlanId);
      memcpy(hapiSystemPtr->bridgeMacAddr.addr, dapiCmd->cmdData.systemMacAddress.macAddr.addr, sizeof (mac_addr_t));
      hapiSystemPtr->mgmtVlanId = dapiCmd->cmdData.systemMacAddress.vlanId;
      if (L7_BCMX_OK(rc) == L7_FALSE)
      {
        LOG_ERROR(rc);
      }
    }

    return L7_SUCCESS;
  }

  /* If new MAC/VLAN is the same as the installed MAC/VLAN then don't do anything.
  */
  if ((memcmp(hapiSystemPtr->bridgeMacAddr.addr, 
              dapiCmd->cmdData.systemMacAddress.macAddr.addr, sizeof (mac_addr_t)) == 0) &&
      (hapiSystemPtr->mgmtVlanId == dapiCmd->cmdData.systemMacAddress.vlanId))
  {
    return L7_SUCCESS;
  }

  /* In order to change the management VLAN we need to add MAC address with the new VLAN 
  ** to the forwarding database. If the FDB is full then return an error and don't remove the 
  ** old MAC address from the FDB.
  */

  /* Add the system MAC address w/ mgmt vlan id to the ARL 
  */
  memset(&l2Addr, 0, sizeof (bcmx_l2_addr_t));
  memcpy(l2Addr.mac, dapiCmd->cmdData.systemMacAddress.macAddr.addr, sizeof (mac_addr_t));
  l2Addr.vid = dapiCmd->cmdData.systemMacAddress.vlanId;

  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
      
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
				   "\n%s %d: In %s call to 'CPU_USP_GET' - FAILED : %d\n",
				   __FILE__, __LINE__, __FUNCTION__, rc);
	  return L7_FAILURE;
  }
  
  hapiPortPtr = HAPI_PORT_GET(&cpuUsp, dapi_g);
  l2Addr.flags = (BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
  l2Addr.lport = hapiPortPtr->bcmx_lport;

  /* Add MAC addr to hw ARL table 
  */
  rc = usl_bcmx_l2_addr_add(&l2Addr, L7_NULL);
  if (L7_BCMX_OK(rc) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
           "\n%s %d: In %s call to 'bcm_l2_addr_add' - FAILED : %d."
           " Failed to add an L2 address to the MAC table.  This should ",
           "only happen when a hash collision occurs or the table is full."
                   __FILE__, __LINE__, __FUNCTION__, rc);
    return L7_FAILURE;
  }

  /* This function will remove the current MAC address if its there, and 
  ** install the new MAC address in the filtering database.
  */
#ifndef BCM_ROBO_SUPPORT
  hapiBroadFfpSysMacInstall (dapi_g,
                             dapiCmd->cmdData.systemMacAddress.vlanId,
                             dapiCmd->cmdData.systemMacAddress.macAddr.addr);
#else
  if (hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
  {
    hapiBroadFfpSysMacInstall (dapi_g,
                               dapiCmd->cmdData.systemMacAddress.vlanId,
                               dapiCmd->cmdData.systemMacAddress.macAddr.addr);
  }
#endif

#ifdef L7_DOT1AG_PACKAGE
   hapiBroadDot1agPolicyInstall(dapi_g, dapiCmd->cmdData.systemMacAddress.macAddr.addr);
#endif
  
  /* If previous system MAC address / mgmt vlan id was configured, remove it */
  if (hapiSystemPtr->mgmtVlanId != 0)
  {
    memcpy(mgmtMac, hapiSystemPtr->bridgeMacAddr.addr, sizeof (bcm_mac_t)); 

    rc = usl_bcmx_l2_addr_delete(mgmtMac, hapiSystemPtr->mgmtVlanId);
    if (L7_BCMX_OK(rc) == L7_FALSE)
    {
      LOG_ERROR(rc);
    }
  }

  /* Update the system data structure w/ mgmt mac/vlan */

  memcpy(hapiSystemPtr->bridgeMacAddr.addr, dapiCmd->cmdData.systemMacAddress.macAddr.addr, sizeof (mac_addr_t));
  hapiSystemPtr->mgmtVlanId = dapiCmd->cmdData.systemMacAddress.vlanId;
#ifdef BCM_ROBO_SUPPORT
#ifdef L7_DOT1AD_PACKAGE
      if (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
      {
         if (dapi_g->system->dvlanEnable == L7_TRUE)
         {
           hapiBroadRoboDoubleVlanDenyRule(dapi_g, L7_TRUE);
         }
         else
         {
           hapiBroadRoboDoubleVlanDenyRule(dapi_g, L7_FALSE);
         }
      }
#endif
#endif
  return result;
}

/*********************************************************************
*
* @purpose Configures broadcast control for the system
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.broadcastControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemBroadcastControlModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, 
                                               DAPI_t *dapi_g)
{
  L7_RC_t             result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t         *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  DAPI_USP_t searchUsp;

  if (dapiCmd->cmdData.broadcastControl.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  for (searchUsp.unit=0;searchUsp.unit<dapi_g->system->totalNumOfUnits;searchUsp.unit++)
  {
    for (searchUsp.slot=0;searchUsp.slot<dapi_g->unit[searchUsp.unit]->numOfSlots;searchUsp.slot++)
    {
      if ((dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE) &&
          (IS_SLOT_TYPE_PHYSICAL(&searchUsp, dapi_g)                       == L7_TRUE))
      {
        for (searchUsp.port=0;searchUsp.port<dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;searchUsp.port++)
        {
            if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
            {
              continue;
            }
            result = hapiBroadIntfBroadcastControlModeSet(&searchUsp, cmd, data, dapi_g);
        }
      }
    }
  }

  return result;

}


/*********************************************************************
*
* @purpose Configures storm control for an interface
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.broadcastControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBroadcastControlModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, 
                                             DAPI_t *dapi_g)
{
  static const char         *routine_name = "hapiBroadIntfBroadcastControlModeSet()";
  L7_RC_t               result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  L7_uint32             portSpeed, threshold, bucket_size;  /* PTin added: StormControl: bucket size */
  L7_int64              rate;
  L7_uint8              units;                              /* PTin added: StormControl: units */
  usl_bcm_port_rate_limit_t  rateLimit;
  int                   rv;

  memset(&rateLimit, 0, sizeof(rateLimit));

  if (dapiCmd->cmdData.broadcastControl.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, routine_name);
    return result;
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  threshold   = dapiCmd->cmdData.broadcastControl.threshold;
  bucket_size = dapiCmd->cmdData.broadcastControl.bucket_size;
  units       = dapiCmd->cmdData.broadcastControl.unit;

  if (dapiCmd->cmdData.broadcastControl.unit == L7_RATE_UNIT_PERCENT ||
      dapiCmd->cmdData.broadcastControl.unit == L7_RATE_UNIT_KBPS)
  {
     switch (hapiPortPtr->speed) 
     {
     case DAPI_PORT_SPEED_FE_10MBPS:
         portSpeed = 10000;
         break;
     case DAPI_PORT_SPEED_FE_100MBPS:
         portSpeed = 100000;
         break;
     case DAPI_PORT_SPEED_GE_1GBPS:
         portSpeed = 1000000;
         break;
     /* PTin added: Speed 2.5G */
     case DAPI_PORT_SPEED_GE_2G5BPS:
         portSpeed = 2500000;
         break;
     /* PTin end */
     case DAPI_PORT_SPEED_GE_10GBPS:
         portSpeed = 10000000;
         break;
     /* PTin added: Speed 40G */
     case DAPI_PORT_SPEED_GE_40GBPS:
         portSpeed = 40000000;
         break;
     /* PTin added: Speed 100G */
     case DAPI_PORT_SPEED_GE_100GBPS:
         portSpeed = 100000000;
         break;
     /* PTin end */
     default:
         portSpeed = 10000;
         break;
     }

    /* PTin modified: StormControl */
  #if 1
    if (dapiCmd->cmdData.broadcastControl.unit == L7_RATE_UNIT_PERCENT)
    {
      rate = ((L7_int64) portSpeed * threshold) / 100;
    }
    else
    {
      rate = (L7_int64) portSpeed;
    }

    /* Multiple of 64 Kbps */
    rate  = (rate * 64) / 64;

    /* Always KBPS */
    units = L7_RATE_UNIT_KBPS;

  #else
    if( (hapiBroadRoboCheck() == L7_TRUE))
    {
      rate = (portSpeed * threshold) /100;
    }
    else
    {
      /* Port speed is in kpbs. Apply threshold (1000 * threshold/100) */
      rate = ((L7_int64)portSpeed * 10 * threshold);

      /* Convert the bps value to lpps units assuming an average pkt size.
       * Account for IFG(12) and Preamble(8) bytes for accuracy.
       */
      rate =  rate/ ((L7_STORMCONTROL_AVG_PKT_SIZE+12+8) * 8);
    }
  #endif
  }
  else if (dapiCmd->cmdData.broadcastControl.unit == L7_RATE_UNIT_PPS) 
  {
    if( (hapiBroadRoboCheck() == L7_TRUE))
    {
      rate  = ((L7_int64)(dapiCmd->cmdData.broadcastControl.threshold)* 512 * 8) / (1000) ;
    }
    else
    {
      rate  = (L7_int64)dapiCmd->cmdData.broadcastControl.threshold;
    }

    /* PTin added: StormControl: units */
    units = L7_RATE_UNIT_PPS;
  }
  else
  {
     result = L7_FAILURE;
     SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                    "\n%s %d: In %s invalid Threshold unit\n",
                    __FILE__, __LINE__, routine_name);
     return result;
  }

  /* PTin added: Units */
  rateLimit.units = units;

  switch (dapiCmd->cmdData.broadcastControl.type) 
  {
  case STORMCONTROL_BCAST:
      if (dapiCmd->cmdData.broadcastControl.enable == L7_TRUE)
      {
        rateLimit.limit       = (L7_int32) rate;
        rateLimit.bucket_size = (L7_int32) bucket_size;   /* PTin added: StormControl: bucket size */
        rateLimit.flags       = BCM_RATE_BCAST;
      }
      else
      {
        rateLimit.limit       = 0;
        rateLimit.bucket_size = 0;                       /* PTin added: StormControl: bucket size */
        rateLimit.flags       = 0;
      }

	  rv = usl_bcmx_rate_bcast_set(hapiPortPtr->bcmx_lport, rateLimit);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR (rv);
      }

      break;

  case STORMCONTROL_MCAST:
      if (dapiCmd->cmdData.broadcastControl.enable == L7_TRUE)
      {
        rateLimit.limit       = (L7_int32) rate;
        rateLimit.bucket_size = (L7_int32) bucket_size;   /* PTin added: StormControl: bucket size */
        rateLimit.flags       = BCM_RATE_MCAST;
      }
      else
      {
        rateLimit.limit       = 0;
        rateLimit.bucket_size = 0;                       /* PTin added: StormControl: bucket size */
        rateLimit.flags       = 0;
      }

	  rv = usl_bcmx_rate_mcast_set(hapiPortPtr->bcmx_lport, rateLimit);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
         LOG_ERROR (rv);
      }

      break;

  case STORMCONTROL_UCAST:
      if (dapiCmd->cmdData.broadcastControl.enable == L7_TRUE)
      {  
        rateLimit.limit       = (L7_int32) rate;
        rateLimit.bucket_size = (L7_int32) bucket_size;   /* PTin added: StormControl: bucket size */
        rateLimit.flags       = BCM_RATE_DLF;
      }
      else
      {
        rateLimit.limit       = 0;
        rateLimit.bucket_size = 0;                       /* PTin added: StormControl: bucket size */
        rateLimit.flags       = 0;
      }

	  rv = usl_bcmx_rate_dlfbc_set(hapiPortPtr->bcmx_lport, rateLimit);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
         LOG_ERROR (rv);
      }

      break;

  default:
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s invalid STORM type\n",
                     __FILE__, __LINE__, routine_name);
      break;
  }

  return result;
}



/*********************************************************************
*
* @purpose Sets the flow control thresholds
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_FLOW_CONTROL
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.flowControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemFlowControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  L7_RC_t rc = L7_SUCCESS;
  int rv;
  L7_uchar8 l2mac[6];
  L7_uchar8 l3mac[6];
  int mode;

  dapi_g->system->flowControlEnable = dapiCmd->cmdData.flowControl.enable;

  rc = sysapiHpcIfaceMacGet(L7_CPU_INTF, L7_CPU_SLOT_NUM, 0,(L7_uchar8 *)l2mac,(L7_uchar8 *)l3mac);
  if (rc != L7_SUCCESS)
  {
    return rc;    
  }

  rv = usl_bcmx_flow_control_set(L7_ALL_UNITS,dapiCmd->cmdData.flowControl.enable,l2mac);


  if (L7_BCMX_OK(rv) == L7_TRUE) 
  {
    mode = hapiBroadDropModeCheck((dapiCmd->cmdData.flowControl.enable)?PAUSE_ENABLED:PAUSE_DISABLED);
    rv = usl_bcmx_mmu_dropmode_set(mode);
  }

  if (L7_BCMX_OK(rv) == L7_FALSE) rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose Sets the flow control thresholds
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_FLOW_CONTROL
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.flowControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadInterfaceFlowControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                              result  = L7_SUCCESS;
  BROAD_PORT_t                        *hapiPortPtr;
  int                                  rv;
  DAPI_INTF_MGMT_CMD_t                *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;;
  int                                  mode;
  usl_bcm_port_pause_config_t         pauseCmd;

   
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (hapiPortPtr->front_panel_stack == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (dapiCmd->cmdData.flowControl.enable)
  {
    /* Enable Rx/Tx pause */
    pauseCmd.pauseTx = L7_TRUE;
    pauseCmd.pauseRx = L7_TRUE;
    memcpy (pauseCmd.pauseMacAddr, ((BROAD_SYSTEM_t*)dapi_g->system->hapiSystem)->bridgeMacAddr.addr, sizeof(pauseCmd.pauseMacAddr));
  }
  else
  {
    /* Disable Rx/Tx pause */
    pauseCmd.pauseTx = L7_FALSE;
    pauseCmd.pauseRx = L7_FALSE;
    memcpy (pauseCmd.pauseMacAddr, ((BROAD_SYSTEM_t*)dapi_g->system->hapiSystem)->bridgeMacAddr.addr, sizeof(pauseCmd.pauseMacAddr));
  }

  rv = usl_bcmx_port_flow_control_set(hapiPortPtr->bcmx_lport, pauseCmd);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {  
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Failed to set the flow-control mode for usp %d/%d/%d rv %d\n",
                   usp->unit, usp->slot, usp->port, rv);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "Failed to set the flow-control mode for usp %d/%d/%d rv %d\n",
            usp->unit, usp->slot, usp->port, rv);
    result = L7_FAILURE;
  }
  else
  {
    if (dapiCmd->cmdData.flowControl.enable)
    {
      mode = hapiBroadDropModeCheck(PAUSE_ENABLED);
      hapiPortPtr->pause = L7_TRUE;
    }
    else
    {
      hapiPortPtr->pause = L7_FALSE;
      mode = hapiBroadDropModeCheck(PAUSE_DISABLED);
    }
  
    rv = usl_bcmx_mmu_dropmode_set(mode);
  }
  
  if (L7_BCMX_OK(rv) != L7_TRUE) result = L7_FAILURE;

  return result;
}



/*********************************************************************
*
* @purpose Sets the speed of the interface
*
* @param   DAPI_USP_t *usp    - usp for the current interface
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_SPEED_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portSpeedConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfSpeedConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_INTF_MGMT_CMD_t  *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
    return L7_FAILURE;

  if (dapiCmd->cmdData.portSpeedConfig.getOrSet == DAPI_CMD_SET)
  {
    if (hapiBroadPhyModeSet(usp,
                            dapiCmd->cmdData.portSpeedConfig.speed,
                            dapiCmd->cmdData.portSpeedConfig.duplex,
                            dapi_g) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    dapiCmd->cmdData.portSpeedConfig.isLinkUp = L7_FALSE;
    hapiPortPtr->speed   = dapiCmd->cmdData.portSpeedConfig.speed;
    hapiPortPtr->duplex  = dapiCmd->cmdData.portSpeedConfig.duplex;
  }
  else
  {
    dapiCmd->cmdData.portSpeedConfig.isLinkUp = hapiPortPtr->link_up;
    dapiCmd->cmdData.portSpeedConfig.speed = hapiPortPtr->speed;
    dapiCmd->cmdData.portSpeedConfig.duplex = hapiPortPtr->duplex;
    dapiCmd->cmdData.portSpeedConfig.isRxPauseAgreed = hapiPortPtr->rx_pause;
    dapiCmd->cmdData.portSpeedConfig.isTxPauseAgreed = hapiPortPtr->tx_pause;
    dapiCmd->cmdData.portSpeedConfig.isSfpLinkUp = hapiPortPtr->sfpLink;
  }

  return L7_SUCCESS;  
}

/*********************************************************************
 *
 * @purpose  Used to configure Auto Negotialtion on a physical port.
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param    cmd           @b{(input)} DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG
 * @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.autoNegotiateConfig
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfAutoNegotiateConfig(DAPI_USP_t *usp,
    DAPI_CMD_t cmd, void *data,
    DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                local_ability_mask = 0, ability_mask = 0;

#if !L7_FEAT_DUAL_PHY_COMBO
  if (dapiCmd->cmdData.autoNegotiateConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
        "\n%s %d: In %s invalid getOrSet Command\n",
        __FILE__, __LINE__, __FUNCTION__);
    return result;
  }
#endif

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Auto Negotiation is only valid for physical ports */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    return L7_FAILURE;
  }

#if L7_FEAT_DUAL_PHY_COMBO
  if (dapiCmd->cmdData.autoNegotiateConfig.getOrSet == DAPI_CMD_SET)
  {
#endif
  ability_mask = dapiCmd->cmdData.autoNegotiateConfig.abilityMask; 

  if (ability_mask & DAPI_NEG_10MB_HD)   local_ability_mask |= BCM_PORT_ABIL_10MB_HD;
  if (ability_mask & DAPI_NEG_10MB_FD)   local_ability_mask |= BCM_PORT_ABIL_10MB_FD;
  if (ability_mask & DAPI_NEG_100MB_HD)  local_ability_mask |= BCM_PORT_ABIL_100MB_HD;
  if (ability_mask & DAPI_NEG_100MB_FD)  local_ability_mask |= BCM_PORT_ABIL_100MB_FD;
  if (ability_mask & DAPI_NEG_1000MB_FD) local_ability_mask |= BCM_PORT_ABIL_1000MB_FD;
  if (ability_mask & DAPI_NEG_10GB_FD)   local_ability_mask |= BCM_PORT_ABIL_10GB_FD;

  hapiPortPtr->autonegotiationAbilityMask = local_ability_mask;

  /*
     if ability_mask == 0, disable the auto negotiation for this specific port
     else enable 
   */
  if(ability_mask == 0)
  {
      result = hapiBroadPhyModeSet(usp, hapiPortPtr->speed, hapiPortPtr->duplex, dapi_g);
    }
  else 
  {
    result = hapiBroadPhyModeSet(usp, DAPI_PORT_AUTO_NEGOTIATE, DAPI_PORT_DUPLEX_FULL, dapi_g);
  }
#if L7_FEAT_DUAL_PHY_COMBO
  }
  else
  {
    local_ability_mask = hapiPortPtr->autonegotiationAbilityMask;

    if (local_ability_mask & BCM_PORT_ABIL_10MB_HD)    ability_mask |= DAPI_NEG_10MB_HD;
    if (local_ability_mask & BCM_PORT_ABIL_10MB_FD)    ability_mask |= DAPI_NEG_10MB_FD;
    if (local_ability_mask & BCM_PORT_ABIL_100MB_HD)   ability_mask |= DAPI_NEG_100MB_HD;
    if (local_ability_mask & BCM_PORT_ABIL_100MB_FD)   ability_mask |= DAPI_NEG_100MB_FD;
    if (local_ability_mask & BCM_PORT_ABIL_1000MB_FD)  ability_mask |= DAPI_NEG_1000MB_FD;
    if (local_ability_mask & BCM_PORT_ABIL_10GB_FD)    ability_mask |= DAPI_NEG_10GB_FD;

    dapiCmd->cmdData.autoNegotiateConfig.abilityMask = ability_mask;
    result = L7_SUCCESS;
  }
#endif

  return result;
}

/*********************************************************************
*
* @purpose Registers a callback for a particular family
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_STP_STATE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.stpState
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfStpState(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result    = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t *dapiCmd   = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  L7_int32              state;
  L7_int32              i;
  int                   rv;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  /* We only support a set command (application doesn't use get so we don't support)
  */
  if (dapiCmd->cmdData.stpState.getOrSet != DAPI_CMD_SET)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s stp state get not supported\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  hapiPortPtr->stp_state =  dapiCmd->cmdData.stpState.state;

  switch (dapiCmd->cmdData.stpState.state)
  {
    case DAPI_PORT_STP_BLOCKED:
      state = BCM_PORT_STP_BLOCK;

    case DAPI_PORT_STP_LISTENING:
      state = BCM_PORT_STP_LISTEN;
      break;

    case DAPI_PORT_STP_LEARNING:
      state = BCM_PORT_STP_LEARN;
      break;

    case DAPI_PORT_STP_FORWARDING:
    case DAPI_PORT_STP_NOT_PARTICIPATING:
      state = BCM_PORT_STP_FORWARD;
      break;

    case DAPI_PORT_STP_ADMIN_DISABLED:
      state = BCM_PORT_STP_DISABLE;
      break;

    default:
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s invalid Spanning Tree State\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return result;
      break;
  }

  hapiPortPtr->hw_stp_state = state;

  /* if port is a LAG, set hw state for all member ports */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp,dapi_g);

        /* issue bcm call to set stp state */
        rv = bcmx_port_stp_set(hapiPortPtr->bcmx_lport, state);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          LOG_ERROR (rv);
        }
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    /* issue bcm call to set stp state */
    rv = bcmx_port_stp_set(hapiPortPtr->bcmx_lport, state);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result =  L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_port_stp_set' failed!\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return result;
    }
  }
  return result;
}

/*********************************************************************
*
* @purpose Configure Loopback for the interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_LOOPBACK_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portLoopbackConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   DEBUG mode for now.  The application does not call this yet.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLoopbackConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t            result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  L7_int32              loopback_val;
  L7_int32              i;
  int                   rv;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  if (dapiCmd->cmdData.portLoopbackConfig.getOrSet != DAPI_CMD_SET)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s Loopback Mode is not supported\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  hapiPortPtr->loopback_mode =  dapiCmd->cmdData.portLoopbackConfig.loopMode;

  hapiPortPtr->hw_lb_mode = dapiCmd->cmdData.portLoopbackConfig.loopMode;

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp,dapi_g);

        rv = bcmx_port_loopback_get(hapiPortPtr->bcmx_lport,  &loopback_val);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          result =  L7_FAILURE;
          SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                        "\n%s %d: In %s call to 'bcmx_port_loopback_get' failed!\n",
                        __FILE__, __LINE__, __FUNCTION__);
          return result;
        }
        if(loopback_val != dapiCmd->cmdData.portLoopbackConfig.loopMode)
        {
          rv = bcmx_port_loopback_set(hapiPortPtr->bcmx_lport, dapiCmd->cmdData.portLoopbackConfig.loopMode);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            LOG_ERROR (rv);
          }
        }
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    rv = bcmx_port_loopback_get(hapiPortPtr->bcmx_lport,  &loopback_val);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result =  L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_port_loopback_get' failed!\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return result;
    }

    if(loopback_val != dapiCmd->cmdData.portLoopbackConfig.loopMode)
    {
      rv = bcmx_port_loopback_set(hapiPortPtr->bcmx_lport, dapiCmd->cmdData.portLoopbackConfig.loopMode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        result =  L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'bcmx_port_loopback_set' failed!\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
      }
    }
  }
  return result;
}

/*********************************************************************
*
* @purpose Configure Isolate bit in Phy for the interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_ISOLATE_PHY_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portIsolatePhyConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIsolatePhyConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_PORT_t          *dapiPortPtr;
  L7_BOOL               enable;
  int                   rv;
#ifdef PC_LINUX_HOST
  int unit;
  bcm_port_t port;
#endif

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
     if (dapiCmd->cmdData.portIsolatePhyConfig.enable == L7_TRUE)
        hapiPortPtr->hapiModeparm.lag.isolatePending = L7_TRUE;
     else
        hapiPortPtr->hapiModeparm.lag.isolatePending = L7_FALSE;
  }

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if (hapiPortPtr->front_panel_stack == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (dapiCmd->cmdData.portIsolatePhyConfig.enable) 
  {
    enable = L7_FALSE;
  }
  else
  {
	enable = L7_TRUE;
  }

  hapiPortPtr->hapiModeparm.physical.admin_enabled = enable;

  rv = usl_bcmx_port_enable_set(hapiPortPtr->bcmx_lport, enable);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
	LOG_ERROR (rv);
  }

  /* PTin removed */
#if (0 /*PTIN_BOARD == PTIN_BOARD_TA48GE*/)
  /* Frontal ports */
  if (enable && hapiPortPtr->bcm_port <= 49)
  {
    rv = bcm_port_speed_set(0, hapiPortPtr->bcm_port, 1000);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting 1G speed to bcm_port %u", hapiPortPtr->bcm_port);
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "1G speed to bcm_port %u", hapiPortPtr->bcm_port);
    }
  }
#endif

#ifdef PC_LINUX_HOST
/* needed in order to set the linke state for simulation,sometimes requires two tries */
  bcmx_lport_to_unit_port(hapiPortPtr->bcmx_lport, &unit, &port); 
  (void)_bcm_esw_link_force(unit, port,
                            1, (dapiCmd->cmdData.portIsolatePhyConfig.enable == L7_FALSE)?1:0);

  (void)_bcm_esw_link_force(unit, port,
                            1, (dapiCmd->cmdData.portIsolatePhyConfig.enable == L7_FALSE)?1:0);
#endif


  return result;
}


/*********************************************************************
*
* @purpose Configure IGMP/MLD Snooping
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_IGMP_SNOOP_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.snoopConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemSnoopConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t             result = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t  *dapiCmd   = (DAPI_SYSTEM_CMD_t*)data;
  L7_BOOL             switchIgmp = L7_FALSE;  /* Variable added for readability */
  /* PTin added: IGMP snooping */
  #if 1
  //L7_uint8            cos = dapiCmd->cmdData.snoopConfig.CoS;
  L7_uint16           vlanId = dapiCmd->cmdData.snoopConfig.vlanId;
  #endif

  if (dapiCmd->cmdData.snoopConfig.getOrSet != DAPI_CMD_SET) 
  {
    result =  L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: Failure in %s!\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (dapiCmd->cmdData.snoopConfig.family == L7_AF_INET) /* IGMP Snooping */
  {
    if (dapiCmd->cmdData.snoopConfig.enable == L7_TRUE)
    {
      dapi_g->system->igmpSnoopingEnable = L7_TRUE;

      if (!dapi_g->system->igmpEnable)
      {
        /* Since IGMP is not yet enabled, filter does not exist. 
         * Do not flood/switch IGMP frames when snooping is configured. 
         * Application will forward these frames to router ports only.
         */
        switchIgmp = L7_FALSE;
      }
      else
      {
         /* L3 IGMP is enabled, remove the IGMP switch filter */
         hapiBroadConfigIgmpFilter (L7_FALSE, vlanId, L7_FALSE, dapi_g);    /* PTin modified: IGMP snooping */
         /* Also, do not flood/switch IGMP frames. Application will 
          * forward the frames to router ports 
          */
         switchIgmp = L7_FALSE;
      }

      hapiBroadConfigIgmpFilter(L7_TRUE, vlanId, switchIgmp, dapi_g);       /* PTin modified: IGMP snooping */
    }
    else
    {
      dapi_g->system->igmpSnoopingEnable = L7_FALSE;
      if (!dapi_g->system->igmpEnable)
      {
        /* Since IGMP is also not enabled, filter can be removed */
        hapiBroadConfigIgmpFilter(L7_FALSE, vlanId, L7_FALSE, dapi_g);      /* PTin modified: IGMP snooping */
      }
      else
      {
        /* Snooping is disabled, but L3 IGMP is enabled */
        /* Remove IGMP filter to trap frames */
        hapiBroadConfigIgmpFilter(L7_FALSE, vlanId, L7_FALSE, dapi_g);      /* PTin modified: IGMP snooping */

        /* Add IGMP filter to switch frames. Switching behaviour is required for L3 IGMP */
        switchIgmp = L7_TRUE;
        hapiBroadConfigIgmpFilter(L7_TRUE, vlanId, switchIgmp, dapi_g);     /* PTin modified: IGMP snooping */
      }
    }
  }
  else /* MLD Snooping */
  {
    if (dapiCmd->cmdData.snoopConfig.enable == L7_TRUE)
    {
      if (!dapi_g->system->mldEnable)
      {
        if (dapi_g->system->mldSnoopingEnable != L7_TRUE)
        {

#ifndef FEAT_METRO_CPE_V1_0
          /* Since L3 MLD  is not yet enabled, filter does not exist */
           if (hapiBroadConfigMldFilter(L7_TRUE,dapi_g) == L7_SUCCESS)
#endif
           {
             dapi_g->system->mldSnoopingEnable = L7_TRUE;
           }
        }
      }
    }
    else
    {
      if (!dapi_g->system->mldEnable)
      {
        if (dapi_g->system->mldSnoopingEnable != L7_FALSE)
        {
#ifndef FEAT_METRO_CPE_V1_0
          /* Since L3 MLD is also not enabled, filter can be removed */
          if (hapiBroadConfigMldFilter(L7_FALSE,dapi_g) == L7_SUCCESS)
#endif
          {
            dapi_g->system->mldSnoopingEnable = L7_FALSE;
          }
        }
      }
    }

  }
  return result;
}

/* PTin added: DHCP snooping */
#if 1
/*********************************************************************
*
* @purpose Configure DHCP Snooping
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_IGMP_SNOOP_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.snoopConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemPacketTrapConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_BOOL enable = L7_TRUE;
  DAPI_SYSTEM_CMD_t *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  L7_RC_t status=L7_SUCCESS;

  switch (dapiCmd->cmdData.snoopConfig.getOrSet)  {
    case DAPI_CMD_SET:
      enable = dapiCmd->cmdData.snoopConfig.enable;
      break;
    case DAPI_CMD_CLEAR:
      enable = L7_FALSE;
      break;
  default:
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid command type %u", dapiCmd->cmdData.snoopConfig.getOrSet);
      return L7_FAILURE;
  }

  switch (dapiCmd->cmdData.snoopConfig.packet_type)
  {
    /* IGMP packets */
    case PTIN_PACKET_IGMP:
    /* DHCP packets */
    case PTIN_PACKET_DHCP:
    /* PPPoE packets */
    case PTIN_PACKET_PPPOE:
    /* APS packets */
    case PTIN_PACKET_APS:
    /* ARP/IP dtl0 packets */
    case PTIN_PACKET_IPDTL0:
      status = hapiBroadConfigTrap(usp, &dapiCmd->cmdData.snoopConfig, L7_FALSE, dapi_g);
      break;

    /* MLD packets */
    case PTIN_PACKET_MLD:
      status = hapiBroadConfigMldFilter(enable, dapi_g );
      break;
    
    /* CCM packets */
    case PTIN_PACKET_CCM:
      status = hapiBroadConfigCcmFilter(usp, enable, dapiCmd->cmdData.snoopConfig.vlanId, dapiCmd->cmdData.snoopConfig.level, dapi_g);
      break;

    /* Not handled */
    default:
      status = L7_FAILURE;
      break;
  }

  /* Check for errors */
  if (status != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error: %d", status);
  }

  return status;
}
#endif

/* PTin added: PRBS */
#if 1
/*********************************************************************
*
* @purpose Configure PRBS tx/rx
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd
* @param   void       *data
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemPTinPrbs(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_SYSTEM_CMD_t *dapiCmd   = (DAPI_SYSTEM_CMD_t*)data;
  L7_RC_t status=L7_SUCCESS;

  switch (dapiCmd->cmdData.prbsStatus.getOrSet)  {
    case DAPI_CMD_SET:
      status = hapiBroadPTinPrbsEnable( usp, dapiCmd->cmdData.prbsStatus.enable, dapi_g );
      break;

    case DAPI_CMD_GET:
      status = hapiBroadPTinPrbsRxStatus( usp, &dapiCmd->cmdData.prbsStatus.rxErrors, dapi_g );
      break;

    default:
      status = L7_FAILURE;
  }

  return status;
}
#endif

/*********************************************************************
* @purpose Determine whether this code is running on platforms with MLD
*          issue 
*
* @param   none
*
* @returns L7_TRUE - Code is running on platforms having MLD hardware bug
* @returns L7_FALSE - Code is not running on platforms with MLD bug.
*
* @notes   This function is primarily used in common CPU RX code.
*          See GNATS 15333 and Hardware errata docs.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadMldHwIssueCheck (void)
{
  static L7_BOOL first_time = L7_FALSE;
  static L7_BOOL mldBug = L7_FALSE; 
  const bcm_sys_board_t *board_info;

  if (first_time == L7_FALSE)
  {
    board_info = hpcBoardGet();
    /* FB-2 and above have no issues */
    if ((board_info->npd_id == __BROADCOM_56304_ID) ||
        (board_info->npd_id == __BROADCOM_56314_ID) ||
        (board_info->npd_id == __BROADCOM_56800_ID) ||
        (board_info->npd_id == __BROADCOM_56504_ID))
    {
      mldBug = L7_TRUE;
    }

    first_time = L7_TRUE;
  }

  return mldBug;
}


/*********************************************************************
* @purpose Determine whether this code is running on Raptor.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Raptor devices
* @returns L7_FALSE - Code is not running on Raptor devices
*
* @notes   
*
* @end
*********************************************************************/
L7_BOOL hapiBroadRaptorCheck (void)
{
  static L7_BOOL first_time = L7_TRUE;
  static L7_BOOL raptor_board = L7_FALSE;
  const bcm_sys_board_t *board_info ;

  if (first_time == L7_TRUE)
  {
    /* First get the board info using the bcm call */
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL) 
    {
      return L7_FALSE;
    }

    switch (board_info->npd_id)
    {
      case __BROADCOM_56214_ID:
      case __BROADCOM_56218_ID:
        raptor_board = L7_TRUE;
        break;
      
      default:
        break;
    }

    first_time = L7_FALSE;
  }

  return raptor_board;
}

/*********************************************************************
* @purpose Determine whether this code is running on Ravan.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Ravan devices
* @returns L7_FALSE - Code is not running on Ravan devices
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL hapiBroadRavenCheck (void)
{
  static L7_BOOL first_time = L7_TRUE;
  static L7_BOOL raven_board = L7_FALSE;
  const bcm_sys_board_t *board_info ;

  if (first_time == L7_TRUE)
  {
    /* First get the board info using the bcm call */
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL)
    {
      return L7_FALSE;
    }

    switch (board_info->sys_brd_id)
    {
      case BCM_SYS_BOARD_56224:
      case BCM_SYS_BOARD_56228:
      case BCM_SYS_BOARD_53724:
        raven_board = L7_TRUE;
        break;

      default:
        break;
    }

    first_time = L7_FALSE;
  }

  return raven_board;
}

/*********************************************************************
* @purpose Determine whether this code is running on Hawkeye.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Hawkeye device
* @returns L7_FALSE - Code is not running on Hawkeye device
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL hapiBroadHawkeyeCheck ( )
{
  static L7_BOOL first_time = L7_TRUE;
  static L7_BOOL hawkeye_board = L7_FALSE;
  const bcm_sys_board_t *board_info ;

  if (first_time == L7_TRUE)
  {
    /* First get the board info using the bcm call */
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL)
    {
      return L7_FALSE;
    }

    switch (board_info->sys_brd_id)
    {
      case BCM_SYS_BOARD_53314:
        hawkeye_board = L7_TRUE;
        break;

      default:
        break;
    }

    first_time = L7_FALSE;
  }

  return hawkeye_board;
}



/*********************************************************************
* @purpose Determine whether this code is running on Robo.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Robo devices
* @returns L7_FALSE - Code is not running on Robo devices
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL hapiBroadRoboCheck (void)
{
#ifdef BCM_ROBO_SUPPORT
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

L7_int32 hapiBroadRoboVariantCheck(void)
{
#ifdef BCM_ROBO_SUPPORT
 static const bcm_sys_board_t *board_info = L7_NULL;

  /* First get the board info using the bcm call */
  if (board_info == L7_NULL)
  {
    board_info = hpcBoardGet() ;
  }

  if ((hapiBroadRoboCheck() == L7_TRUE) && (board_info != L7_NULL))
  {
    return board_info->npd_id;

  }
#endif

  return 0;

}

/*********************************************************************
*
* @purpose Get board family (Strata for 5615 and Draco(XGS) for 5690)
*
* @param   DAPI_USP_t *board_family - pointer to board_family
*
* @returns L7_RC_t result and 
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGetSystemBoardFamily(bcm_chip_family_t *board_family)
{
  L7_RC_t result = L7_SUCCESS;
  static const bcm_sys_board_t *board_info = L7_NULL;

  /* First get the board info using the bcm call */
  if (board_info == L7_NULL)
  {
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL) 
    {
      return L7_FAILURE;
    }
  }

  /* Based on the board, return the family */

  switch (board_info->npd_id)
  {
    case __BROADCOM_56214_ID:
    case __BROADCOM_56218_ID:
    case __BROADCOM_56224_ID:
    case __BROADCOM_56228_ID:
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_56514_ID:
    case __BROADCOM_53314_ID:
       *board_family = BCM_FAMILY_FIREBOLT;
      break ;
    case __BROADCOM_56800_ID:
       *board_family = BCM_FAMILY_BRADLEY;
       break;
    case __BROADCOM_56624_ID:
    case __BROADCOM_56680_ID:
    case __BROADCOM_56334_ID:
      *board_family = BCM_FAMILY_TRIUMPH;
      break;
    case __BROADCOM_56634_ID:
    case __BROADCOM_56524_ID:
    case __BROADCOM_56636_ID:
    case __BROADCOM_56685_ID:   /* PTin added: new switch 56689 (Valkyrie2) */
    case __BROADCOM_56640_ID:   /* PTin added: new switch 5664x (Triumph3) */
    case __BROADCOM_56340_ID:   /* PTin added: new switch 56340 (Helix4) */
      *board_family = BCM_FAMILY_TRIUMPH2;
      break;
    case __BROADCOM_56820_ID:
      *board_family = BCM_FAMILY_SCORPION;
      break;
    case __BROADCOM_53115_ID:
       *board_family = BCM_FAMILY_ROBO;
       break;
    case __BROADCOM_56843_ID:  /* PTin added: new switch 56843 (Trident) */
      *board_family = BCM_FAMILY_TRIDENT;
      break;
  default:
    {
      printf("\nhapiBroadGetSystemBoardFamily : Unknown BoardFamily, npd_id = %x\n", board_info->npd_id);
      result = L7_FAILURE;
    }
  }
  return result;
}

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  /* No Helix+ support for Metro! */
#else
/*********************************************************************
*
* @purpose Create a ingress filter to prioratize MLD packets but
*          for the Helix+ platform. Basically trap anything with the
*          MACDA = 33:33:00:00:00:16 + EHY_TYPe = 0x86DD. From testing
*          with VLC and reading the web, these messages are usually limited
*          MLD control pkts.
*
* @param
*
* @returns L7_RC_t result and
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadHelixConfigMldFilter(L7_BOOL enableFilter,DAPI_t *dapi_g)
{
  L7_int32         rv;
  L7_RC_t          rc = L7_SUCCESS;

  L7_ushort16             ipV6_ethtype     = L7_ETYPE_IPV6;
  BROAD_POLICY_RULE_t     ruleIdReport     = BROAD_POLICY_RULE_INVALID;
  static BROAD_POLICY_t   mldSnoopId       = BROAD_POLICY_INVALID;
  L7_uchar8               mldSnoop_macda[] = {0x33, 0x33, 0x00, 0x00, 0x00, 0x16};
  L7_uchar8               mldSnoopQuery_macda[] = {0x33, 0x33, 0x00, 0x00, 0x00, 0x01};
  L7_uchar8               exact_match[]    = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                              FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

  /* Note, instead of enabling individual controls for MLD reports and queries,
   * use bcmSwitchMldPktToCpu which enables both the controls (report + query).
   * Similarly, bcmSwitchMldPktDrop works on both report and query.
   */
  if (enableFilter == L7_TRUE)
  {
    if (mldSnoopId == BROAD_POLICY_INVALID)
    {
        rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    
        /* give MLD-Reports frames high priority and trap to the CPU. */
        if ((rc = hapiBroadPolicyRuleAdd(&ruleIdReport)) != L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                              BROAD_FIELD_MACDA,
                                              mldSnoop_macda, exact_match)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                                 BROAD_FIELD_ETHTYPE,
                                                (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        /* MLD packets are assigned MEDIUM priority (less than OSPF/RIP, BPDU range,
         * but higher than DHCP/Broadcast Flood).
         */
        if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport, BROAD_ACTION_SET_COSQ,
                                              HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport,
                                              BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        /* give MLD-query frames high priority and trap to the CPU. */
        if ((rc = hapiBroadPolicyRuleAdd(&ruleIdReport)) != L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                              BROAD_FIELD_MACDA,
                                              mldSnoopQuery_macda, exact_match)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                                 BROAD_FIELD_ETHTYPE,
                                                (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        /* MLD packets are assigned MEDIUM priority (less than OSPF/RIP, BPDU range,
         * but higher than DHCP/Broadcast Flood).
         */
        if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport, BROAD_ACTION_SET_COSQ,
                                              HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport,
                                              BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
    
        /* Commit the policy */
        if ((rc = hapiBroadPolicyCommit(&mldSnoopId)) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
    
        rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 1);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          rc = L7_FAILURE;
          LOG_ERROR(rv);
        }
    
        rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 1);
        if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
        {
          rc = L7_FAILURE;
          LOG_ERROR(rv);
        }
    
        /* Set the MLD packet priority. This is required only for FB2 platforms.
         * For other platforms, MLD packets will follow protocol priority.
         * The return code of E_UNAVAIL is masked by the API.
         *
         * NOTE: IGMP packets do not require this setting as we use policy for 
         * IGMP. MLD packets are copied to CPU using CPU_CONTROL register.
         */
        rv = bcmx_switch_control_set(bcmSwitchCpuProtoIgmpPriority,
                                        HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
    
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                          "\n%s %d: In %s Failed to set bcmSwitchCpuProtoIgmpPriority - %d\n",
                          __FILE__, __LINE__, __FUNCTION__, rv);
        }
    }
  }
  else
  {
    if (mldSnoopId != BROAD_POLICY_INVALID )
    {
      rc = hapiBroadPolicyDelete(mldSnoopId);
      mldSnoopId = BROAD_POLICY_INVALID;
    }

    rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      LOG_ERROR(rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      LOG_ERROR(rv);
    }

    /* Reset the MLD packet priority to default */
    rv = bcmx_switch_control_set(bcmSwitchCpuProtoIgmpPriority, 0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s Failed to set bcmSwitchCpuProtoIgmpPriority - %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rv);
    }
  }

  return rc;
}
#endif

/*********************************************************************
*
* @purpose Create a ingress filter to prioratize MLD packets
*
* @param
*
* @returns L7_RC_t result and
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadConfigMldFilter(L7_BOOL enableFilter,DAPI_t *dapi_g)
{
  L7_int32         rv;
  L7_RC_t          rc = L7_SUCCESS;

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  static BROAD_POLICY_t   mldSnoopId   = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId       = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_RULE_t     ruleIdReport = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_RULE_t     ruleIdReportDone = BROAD_POLICY_RULE_INVALID;

#if defined(FEAT_METRO_CPE_V1_0)
  L7_uchar8               ipV6_format  = BCM_FIELD_PKT_FMT_IPV6;
#endif
  L7_ushort16             ipV6_ethtype = L7_ETYPE_IPV6;
  L7_uchar8               ip_icmpv6    = IP_PROT_ICMPV6;
  L7_uchar8               icmpV6Query      = 130; /* MLD-Query type */
  L7_uchar8               icmpV6Report     = 131; /* MLD-Report type */
  L7_uchar8               icmpV6ReportDone = 132; /* MLD-ReportDone type */
  L7_uchar8               mldSnoop_macda[]      = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00};
  L7_uchar8               mldSnoop_macda_mask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_ALL,
                                                   FIELD_MASK_ALL,  FIELD_MASK_ALL,  FIELD_MASK_ALL};

  L7_uchar8               exact_match[]         = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                                   FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
#endif
  bcm_chip_family_t       board_family;

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  /* No Helix+ support for Metro! */
#else
  if( hapiBroadMldHwIssueCheck() == L7_TRUE)
  {
      /* Will get too messy trying to merge into this mess below! */
      rc = hapiBroadHelixConfigMldFilter(enableFilter,dapi_g);

      return rc;
  }
#endif


  hapiBroadGetSystemBoardFamily(&board_family);
  /* Note, instead of enabling individual controls for MLD reports and queries,
   * use bcmSwitchMldPktToCpu which enables both the controls (report + query).
   * Similarly, bcmSwitchMldPktDrop works on both report and query.
   */
  if (enableFilter == L7_TRUE)
  {
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
    /*On bcm53115 with DOT1AD package, dot1ad related rules in TCAM will be
      hit first for MLD packets (as DOT1AD rules are only based on VLAN tags).
      As a result MLD packets will not be trapped to CPU.
      To fix this problem, we need to install protocol snooping rules at higher
      priority compared to DOT1AD rules.*/
    if((hapiBroadRoboVariantCheck() ==  __BROADCOM_53115_ID) ||
       (board_family == BCM_FAMILY_TRIUMPH) || (board_family == BCM_FAMILY_TRIUMPH2))
    {
      if (mldSnoopId == BROAD_POLICY_INVALID)
      {

        if((hapiBroadRoboVariantCheck() ==  __BROADCOM_53115_ID) &&
            (dapi_g->system->dvlanEnable == L7_TRUE)
          ) 
        {
          rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_DOT1AD_SNOOP);
        }
        else if((board_family == BCM_FAMILY_TRIUMPH) || (board_family == BCM_FAMILY_TRIUMPH2))
        {
          rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
        }
        else
        {
          /* Don't create the policy now. */
          /* In CPE code if the MLD Snooping is enabled a call will be made to this
           function after dvlan is enabled through hapiBroadApplyConfig. We just need
           to remember that IGMP Snooping is enabled.
           */

          return rc;
        }
        /* give MLD-Query frames high priority and trap to the CPU. */
        if ((rc = hapiBroadPolicyRuleAdd(&ruleId)) != L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                 BROAD_FIELD_MACDA,
                                                 mldSnoop_macda, mldSnoop_macda_mask)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                 BROAD_FIELD_ETHTYPE,
                                                (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                               BROAD_FIELD_IP6_NEXTHEADER,
                                               (L7_uchar8 *)&ip_icmpv6, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                               BROAD_FIELD_ICMP_MSG_TYPE,
                                               (L7_uchar8 *)&icmpV6Query, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        /* give MLD-Reports frames high priority and trap to the CPU. */
        if ((rc = hapiBroadPolicyRuleAdd(&ruleIdReport)) != L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                              BROAD_FIELD_MACDA,
                                              mldSnoop_macda, mldSnoop_macda_mask)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                                 BROAD_FIELD_ETHTYPE,
                                                (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                               BROAD_FIELD_IP6_NEXTHEADER,
                                               (L7_uchar8 *)&ip_icmpv6, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReport,
                                               BROAD_FIELD_ICMP_MSG_TYPE,
                                               (L7_uchar8 *)&icmpV6Report, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        /* give MLD-ReportDone frames high priority and trap to the CPU. */
        if ((rc = hapiBroadPolicyRuleAdd(&ruleIdReportDone)) != L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }
        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReportDone,
                                              BROAD_FIELD_MACDA,
                                              mldSnoop_macda, mldSnoop_macda_mask)) !=
                                              L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReportDone,
                                                 BROAD_FIELD_ETHTYPE,
                                                (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                                 L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReportDone,
                                               BROAD_FIELD_IP6_NEXTHEADER,
                                               (L7_uchar8 *)&ip_icmpv6, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        if((rc = hapiBroadPolicyRuleQualifierAdd(ruleIdReportDone,
                                               BROAD_FIELD_ICMP_MSG_TYPE,
                                               (L7_uchar8 *)&icmpV6ReportDone, exact_match)) !=
                                               L7_SUCCESS)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }


#if defined(FEAT_METRO_CPE_V1_0)
        if(hapiBroadRoboVariantCheck() ==  __BROADCOM_53115_ID)
        {
          if((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
              BROAD_FIELD_PKT_FORMAT, (L7_uchar8 *)&ipV6_format, exact_match)) !=
              L7_SUCCESS)
          { 
            hapiBroadPolicyCreateCancel();
            return L7_FAILURE;
          }
        }
#endif
      }

      /* MLD packets are assigned MEDIUM priority (less than OSPF/RIP, BPDU range,
       * but higher than DHCP/Broadcast Flood).
       */
      if((rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ,
                                            HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport, BROAD_ACTION_SET_COSQ,
                                            HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReportDone, BROAD_ACTION_SET_COSQ,
                                            HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }

#if defined(FEAT_METRO_CPE_V1_0)
      if((rc = hapiBroadPolicyRuleActionAdd(ruleId,
                                        BROAD_ACTION_SET_REASON_CODE,
                                        8, 0, 0) != L7_SUCCESS))
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
#endif
     

      if((rc = hapiBroadPolicyRuleActionAdd(ruleId,
                                            BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReport,
                                            BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      if((rc = hapiBroadPolicyRuleActionAdd(ruleIdReportDone,
                                            BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                            L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }

      /* Commit the policy */
      if ((rc = hapiBroadPolicyCommit(&mldSnoopId)) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
   else
#endif
   {
     rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 1);
     if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
     {
       rc = L7_FAILURE;
       LOG_ERROR(rv);
     }
     rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 1);
     if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
     {
       rc = L7_FAILURE;
       LOG_ERROR(rv);
     }

     /* Set the MLD packet priority. This is required only for FB2 platforms.
      * For other platforms, MLD packets will follow protocol priority.
      * The return code of E_UNAVAIL is masked by the API.
      *
      * NOTE: IGMP packets do not require this setting as we use policy for 
      * IGMP. MLD packets are copied to CPU using CPU_CONTROL register.
      */
     rv = bcmx_switch_control_set(bcmSwitchCpuProtoIgmpPriority,
                                    HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
     if (L7_BCMX_OK(rv) != L7_TRUE)
     {
       SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s Failed to set bcmSwitchCpuProtoIgmpPriority - %d\n",
                       __FILE__, __LINE__, __FUNCTION__, rv);
     }
   }
  }
  else
  {
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
    if((hapiBroadRoboVariantCheck() ==  __BROADCOM_53115_ID) ||
       (board_family == BCM_FAMILY_TRIUMPH) || (board_family == BCM_FAMILY_TRIUMPH2))
    {
      if (mldSnoopId != BROAD_POLICY_INVALID )
      {
        rc = hapiBroadPolicyDelete(mldSnoopId);
        mldSnoopId = BROAD_POLICY_INVALID;
      }
    }
   else
#endif
  {
    rv = bcmx_switch_control_set(bcmSwitchMldPktDrop, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      LOG_ERROR(rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchMldPktToCpu, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      LOG_ERROR(rv);
    }

    /* Reset the MLD packet priority to default */
    rv = bcmx_switch_control_set(bcmSwitchCpuProtoIgmpPriority, 0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s Failed to set bcmSwitchCpuProtoIgmpPriority - %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rv);
    }
  }
  }
  return rc;
}

L7_RC_t hapiBroadConfigIgmpFilterRaptor(enableFilter)
{

  L7_RC_t          rc = L7_SUCCESS;
#ifdef BCM_RAPTOR_SUPPORT
  L7_int32         rv;

  if (enableFilter == L7_TRUE)
  {
    
    rv = bcmx_switch_control_set(bcmSwitchIgmpReportLeaveDrop, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchIgmpReportLeaveToCpu, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchIgmpQueryDrop, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpQueryToCpu, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpUnknownDrop, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpUnknownToCpu, 1);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
  
  }
  else
  {
    rv = bcmx_switch_control_set(bcmSwitchIgmpReportLeaveDrop, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchIgmpReportLeaveToCpu, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }

    rv = bcmx_switch_control_set(bcmSwitchIgmpQueryDrop, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpQueryToCpu, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpUnknownDrop, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }
    rv = bcmx_switch_control_set(bcmSwitchIgmpUnknownToCpu, 0);
    if ((rv != BCM_E_EXISTS) && (L7_BCMX_OK(rv) != L7_TRUE))
    {
      rc = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "%s:%d bcmx_switch_control_set failed: rv %d\n", __FUNCTION__, __LINE__, rv);
    }

  }
#endif
  return rc;
}

/* PTin added: IGMP */
/**
 * Configure IGMP packet trap rule
 * 
 * @param enable 
 * @param vlanId 
 * @param switchFrame 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigIgmpFilter(L7_BOOL enable, L7_uint16 vlanId, L7_BOOL switchFrame, DAPI_t *dapi_g)
{
  cmdData_snoopConfig_t snoopConfig;

  memset(&snoopConfig, 0x00, sizeof(snoopConfig));

  snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  snoopConfig.enable      = enable & 1;
  snoopConfig.family      = L7_AF_INET;
  snoopConfig.packet_type = PTIN_PACKET_IGMP;
  snoopConfig.CoS         = (L7_uint8) -1;
  snoopConfig.vlanId      = vlanId;
  snoopConfig.vlan_mask   = 0xfff;
  memset(&snoopConfig.macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  snoopConfig.level       = 0;

  return hapiBroadConfigTrap(L7_NULLPTR, &snoopConfig, switchFrame, dapi_g);
}

#if 1
typedef struct
{
  L7_BOOL             in_use;
  BROAD_POLICY_t      policyId;
  L7_uint16           vlan;
  L7_uint16           vlan_mask;
  L7_enetMacAddr_t    macAddr;
  L7_uint32           param;
  ptin_packet_type_t  packet_type;
  L7_uint8            ip_version;
} ptin_vlan_policy_trap_t;

static L7_BOOL ptin_trap_policy_global_enable[PTIN_PACKET_LAST];


#define PTIN_TRAP_POLICY_MAX_VLANS 200
ptin_vlan_policy_trap_t ptin_trap_policy[PTIN_TRAP_POLICY_MAX_VLANS];


L7_RC_t hapiBroadConfigTrap(DAPI_USP_t *usp, cmdData_snoopConfig_t *snoopConfig, L7_BOOL switchFrame, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  L7_uint16               index, search_index, search_index_free;
  static L7_BOOL          ptin_trap_policy_first_time = L7_TRUE;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting igmp trapping processing");

  /* Initialization */
  if (ptin_trap_policy_first_time)
  {
    memset(ptin_trap_policy_global_enable, 0x00, sizeof(ptin_trap_policy_global_enable));
    memset(ptin_trap_policy, 0x00, sizeof(ptin_trap_policy)); 

    ptin_trap_policy_first_time = L7_FALSE;
  }

  /* Validate arguments */
  if ( snoopConfig->vlanId >= 4096 || snoopConfig->packet_type >= PTIN_PACKET_LAST ||
      (snoopConfig->family != L7_AF_INET && snoopConfig->family != L7_AF_INET6))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid Vlan (%u) or packet type (%u) or ip_version (%u)", snoopConfig->vlanId, snoopConfig->packet_type, snoopConfig->family);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Original vlan = %u",snoopConfig->vlanId);
  snoopConfig->vlan_mask = PTIN_VLAN_MASK(snoopConfig->vlanId);
  snoopConfig->vlanId &= snoopConfig->vlan_mask;
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"vlan = %u, mask=0x%04x", snoopConfig->vlanId, snoopConfig->vlan_mask);

  /* If vlan value is valid, Find igmp index */
  if (snoopConfig->vlanId >= PTIN_VLAN_MIN && snoopConfig->vlanId <= PTIN_VLAN_MAX)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan provided is valid (%u). Enable=%u", snoopConfig->vlanId, snoopConfig->enable);

    search_index_free = (L7_uint16)-1;
    for (search_index = 0; search_index < PTIN_TRAP_POLICY_MAX_VLANS; search_index++)
    {
      /* Skip entries belonging to other packet types */
      if ((ptin_trap_policy[search_index].packet_type != snoopConfig->packet_type || ptin_trap_policy[search_index].ip_version != snoopConfig->family) &&
          (ptin_trap_policy[search_index].packet_type != PTIN_PACKET_NONE && ptin_trap_policy[search_index].packet_type < PTIN_PACKET_LAST))
        continue;

      /* Search for first empty cell */
      if (search_index_free >= PTIN_TRAP_POLICY_MAX_VLANS &&
          !ptin_trap_policy[search_index].in_use)
      {
        search_index_free = search_index;
      }

      /* Found? */
      if (ptin_trap_policy[search_index].in_use &&
          snoopConfig->vlanId == ptin_trap_policy[search_index].vlan)
        break;
    }

    /* Not found... */
    if (search_index >= PTIN_TRAP_POLICY_MAX_VLANS)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Not found vlan %u within the configured ones", snoopConfig->vlanId);

      /* If is going to add a new vlan, use first free index */
      if (snoopConfig->enable)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to add vlan %u to table", snoopConfig->vlanId);

        /* Check if a free index was found */
        if (search_index_free >= PTIN_TRAP_POLICY_MAX_VLANS)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "There is no room to add vlan %u", snoopConfig->vlanId);
          return L7_TABLE_IS_FULL;
        }
        search_index = search_index_free;
        ptin_trap_policy[search_index].in_use     = L7_TRUE;
        ptin_trap_policy[search_index].policyId   = BROAD_POLICY_INVALID;
        ptin_trap_policy[search_index].vlan       = snoopConfig->vlanId;     /* New vlan to be added */
        ptin_trap_policy[search_index].vlan_mask  = snoopConfig->vlan_mask;
        memcpy(&ptin_trap_policy[search_index].macAddr, &snoopConfig->macAddr, sizeof(L7_enetMacAddr_t));
        ptin_trap_policy[search_index].param      = snoopConfig->level;
        ptin_trap_policy[search_index].packet_type= snoopConfig->packet_type;
        ptin_trap_policy[search_index].ip_version = snoopConfig->family;
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u added to table in cell %u", snoopConfig->vlanId, search_index);
      }
      /* If it is to remove a vlan, and it was not found, return SUCCESS */
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Since it is to remove vlan %u, there is nothing to be done", snoopConfig->vlanId);
        return L7_SUCCESS;
      }
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u was found in cell %u", snoopConfig->vlanId, search_index);
      if (!snoopConfig->enable)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to remove cell %u with VLAN %u", search_index, snoopConfig->vlanId);
      }
      else
      {
        /* This Vlan already exists... nothing to be done */
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "Vlan %u already exists... nothing to be done!", snoopConfig->vlanId);
        return L7_SUCCESS;
      }
    }
  }
  /* Global (de)activation */
  else
  {
    ptin_trap_policy_global_enable[snoopConfig->packet_type] = snoopConfig->enable;
    snoopConfig->vlanId = L7_NULL;
    search_index        = L7_NULL;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Null vlan provided: Global enable is applied (enable=%u)", snoopConfig->enable);
  }

  /* Run all igmp indexes */
  for (index = 0; index < PTIN_TRAP_POLICY_MAX_VLANS; index++)
  {
    /* If vlan is null, run all indexes...
       Otherwise (if valid value), only run the searched index */
    if (snoopConfig->vlanId != L7_NULL && index != search_index)
      continue;

    /* Only process these type of packets */
    if (ptin_trap_policy[index].packet_type != snoopConfig->packet_type ||
        ptin_trap_policy[index].ip_version  != snoopConfig->family)
      continue;

    /* If policy exists, delete it */
    if (ptin_trap_policy[index].policyId != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(ptin_trap_policy[index].policyId);
      ptin_trap_policy[index].policyId = BROAD_POLICY_INVALID;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u deleted", index);
    }

    /* If entry is not active, do no more */
    if (!snoopConfig->enable)
    {
      /* Only for unique entry configurations, clear entry */
      if (snoopConfig->vlanId != L7_NULL)
        memset(&ptin_trap_policy[index], 0x00, sizeof(ptin_trap_policy[index]));

      /* Do not proceed */
      continue;
    }

    /* If global enable is disabled, nothing more is to be done */
    if (!ptin_trap_policy_global_enable[snoopConfig->packet_type])
    {
      continue;
    }

    /* We must have a valid entry at this point */
    if (!ptin_trap_policy[index].in_use ||
        (ptin_trap_policy[index].vlan < PTIN_VLAN_MIN || ptin_trap_policy[index].vlan > PTIN_VLAN_MAX))
      continue;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Processing cell #%u", index);

    /* Configure rules */
    switch (snoopConfig->packet_type)
    {
    case PTIN_PACKET_IGMP:
      if (hapiBroadRaptorCheck() == L7_TRUE || hapiBroadHawkeyeCheck() == L7_TRUE) 
      {
        result = hapiBroadConfigIgmpFilterRaptor(snoopConfig->enable);
        break;
      }
      if (hapiBroadRoboCheck() == L7_TRUE)
      {
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        /* Nothing to be done */
        #else
        bcmx_igmp_snooping_enable_set(snoopConfig->enable);
        #endif
      }

      result = hapiBroadConfigIgmpTrap(ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, switchFrame, dapi_g,
                                       &ptin_trap_policy[index].policyId);

      break;
    case PTIN_PACKET_DHCP:
      if (snoopConfig->family == L7_AF_INET6)
      {
        result = hapiBroadConfigDhcpV6Trap(ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, dapi_g,
                                           &ptin_trap_policy[index].policyId);
      }
      else
      {
        result = hapiBroadConfigDhcpV4Trap(ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, dapi_g,
                                           &ptin_trap_policy[index].policyId);
      }
      break; 
    case PTIN_PACKET_PPPOE:
      result = hapiBroadConfigPPPoETrap(ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, dapi_g,
                                        &ptin_trap_policy[index].policyId);
      break;
    case PTIN_PACKET_APS:
      result = hapiBroadConfigApsTrap(usp, ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, ptin_trap_policy[index].param, dapi_g,
                                      &ptin_trap_policy[index].policyId);
      break;
    case PTIN_PACKET_IPDTL0:
      result = hapiBroadConfigIpDtl0Trap(ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask, ptin_trap_policy[search_index].macAddr.addr, dapi_g,
                                         &ptin_trap_policy[index].policyId);
      break;
    default:
      result = L7_FAILURE;
      break;
    }

    /* If error, clear entry */
    if (result != L7_SUCCESS || ptin_trap_policy[index].policyId == BROAD_POLICY_INVALID)
    {
      memset(&ptin_trap_policy[index], 0x00, sizeof(ptin_trap_policy[index]));
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished igmp trapping processing (result=%d)", result);

  return result;
}
#endif


#if 1
/**
 * Create IFP rule to trap IGMP packets
 * 
 * @param vlanId      : VLAN ID value
 * @param vlan_match  : VLAN ID mask
 * @param switchFrame : Should packets be switched?
 * @param dapi_g 
 * @param policy_id   : Configured policy id (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigIgmpTrap(L7_uint16 vlanId, L7_uint16 vlan_match, L7_BOOL switchFrame, DAPI_t *dapi_g,
                                BROAD_POLICY_t *policy_id)
{
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             ip_ethtype  = L7_ETYPE_IP;
  L7_uchar8               igmp_proto[]  = {IP_PROT_IGMP};
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                           FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_METER_ENTRY_t     meterInfo;
  L7_RC_t                 result = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting igmp trapping processing");

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  do
  {
  #if defined(FEAT_METRO_CPE_V1_0)
    /*On bcm53115 with DOT1AD package, dot1ad related rules in TCAM will be
      hit first for IGMP packets (as DOT1AD rules are only based on VLAN tags).
      As a result IGMP packets will not be trapped to CPU.
      To fix this problem, we need to install protocol snooping rules at higher
      priority compared to DOT1AD rules.*/
    if( (hapiBroadRoboVariantCheck()==  __BROADCOM_53115_ID) &&
        (dapi_g->system->dvlanEnable == L7_TRUE) )
    {
     result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_DOT1AD_SNOOP);
    }
    else
    {
      /* Don't create the policy now. */
      /* In CPE code if the IGMP Snooping is enabled a call will be made to this 
         function after dvlan is enabled */ 
      return result;
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    }
  #else
    result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  #endif

    /* Check policy creation */
    if (result != L7_SUCCESS)
    {
      return result;
    }
      
    /* Rate limit */
    if (PTIN_VLAN_IS_QUATTRO(vlanId))
    {
      meterInfo.cir       = RATE_LIMIT_QUATTRO;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_QUATTRO;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }
    else
    {
      meterInfo.cir       = RATE_LIMIT_IGMP;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_IGMP;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }

    /* give IGMP frames high priority and trap to the CPU. */
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Qualifier OVID=%u/0x%04x defined",vlanId, vlan_match);
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Qualifier BROAD_FIELD_ETHTYPE defined");
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, igmp_proto, exact_match);
    if (result != L7_SUCCESS)  break;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Qualifier BROAD_FIELD_PROTO defined");
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
  #ifdef BCM_ROBO_SUPPORT 
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_REASON_CODE, 8, 0, 0);
  #endif
    if (result != L7_SUCCESS)  break;

    /* Check if IGMP frames should be switched or not */
    if (switchFrame == L7_FALSE)
    {
       /* Trap the frames to CPU, so that they are not switched */
       result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
       if (result != L7_SUCCESS)  break;
       result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
       if (result != L7_SUCCESS)  break;
       result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
       if (result != L7_SUCCESS)  break;
       result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
       if (result != L7_SUCCESS)  break;
    }
  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished igmp trapping processing");

  return result;
}
#endif

/* PTin added: DHCP snooping */
#if 1
/**
 * Create IFP rule to trap DHCPv4 packets
 * 
 * @param vlanId     : VLAN ID value
 * @param vlan_match : VLAN ID mask
 * @param dapi_g 
 * @param policy_id  : Configured policy id (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigDhcpV4Trap(L7_uint16 vlanId, L7_uint16 vlan_match, DAPI_t *dapi_g,
                                BROAD_POLICY_t *policy_id)
{
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             ip_ethtype  = L7_ETYPE_IP;
  L7_uchar8               udp_proto[]   = {IP_PROT_UDP};
  L7_ushort16             dhcpc_dport   = UDP_PORT_DHCP_CLNT;
  L7_ushort16             dhcps_dport   = UDP_PORT_DHCP_SERV;
  L7_uint8                ip_type;
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                           FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_RC_t                 result = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting dhcp trapping processing");

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  /* There are 3 set of policies for DHCP packets. Here is why.
   * When DHCP snoopinvlan_listg is enabled, we will have a set of trusted and
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

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)
      break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u created", index);

    /* Rate limit */
    if (PTIN_VLAN_IS_QUATTRO(vlanId))
    {
      meterInfo.cir       = RATE_LIMIT_QUATTRO;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_QUATTRO;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }
    else
    {
      meterInfo.cir       = RATE_LIMIT_DHCP;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_DHCP;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }

    /* give dhcp frames high priority and trap to the CPU. */

    /* DHCP packets from client */
    ip_type = BROAD_IP_TYPE_IPV4;
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpc_dport, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, (L7_uchar8*)&ip_type, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;
    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

    /* DHCP packets from server */
    ip_type = BROAD_IP_TYPE_IPV4;
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcps_dport, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, (L7_uchar8*)&ip_type, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;
    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished dhcp trapping processing");

  return result;
}

/**
 * Create IFP rule to trap DHCPv6 packets
 * 
 * @param vlanId     : VLAN ID value
 * @param vlan_match : VLAN ID mask
 * @param dapi_g 
 * @param policy_id  : Configured policy id (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigDhcpV6Trap(L7_uint16 vlanId, L7_uint16 vlan_match, DAPI_t *dapi_g,
                                BROAD_POLICY_t *policy_id)
{
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             ipv6_ethtype = L7_ETYPE_IPV6;
  L7_uchar8               udp_proto[]   = {IP_PROT_UDP};
  L7_ushort16             dhcpv6c_dport   = 546;
  L7_ushort16             dhcpv6s_dport   = 547;
  L7_uint8                ip_type;
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                           FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_RC_t                 result = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting dhcp trapping processing");

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  /* There are 3 set of policies for DHCP packets. Here is why.
   * When DHCP snoopinvlan_listg is enabled, we will have a set of trusted and
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

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)
      break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u created", index);

    /* Rate limit */
    if (PTIN_VLAN_IS_QUATTRO(vlanId))
    {
      meterInfo.cir       = RATE_LIMIT_QUATTRO;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_QUATTRO;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }
    else
    {
      meterInfo.cir       = RATE_LIMIT_DHCP;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_DHCP;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }

    /* give dhcp frames high priority and trap to the CPU. */

    /* DHCP packets from client */
    ip_type = BROAD_IP_TYPE_IPV6;
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipv6_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpv6c_dport, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, (L7_uchar8*)&ip_type, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;
    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

    /* DHCP packets from server */
    ip_type = BROAD_IP_TYPE_IPV6;
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipv6_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpv6s_dport, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, (L7_uchar8*)&ip_type, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;
    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished dhcp trapping processing");

  return result;
}
#endif

/* PTin added: PPPoE */
#if 1
/**
 * Create IFP rule to trap PPPoE packets
 * 
 * @param vlanId     : VLAN ID value
 * @param vlan_match : VLAN ID mask
 * @param dapi_g 
 * @param policy_id  : Configured policy id (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigPPPoETrap(L7_uint16 vlanId, L7_uint16 vlan_match, DAPI_t *dapi_g,
                                 BROAD_POLICY_t *policy_id)
{
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             pppoe_ethtype  = L7_ETYPE_PPPOE;
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                           FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_RC_t                 result = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting PPPoE trapping processing");

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)
      break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u created", index);

    /* Rate limit */
    if (PTIN_VLAN_IS_QUATTRO(vlanId))
    {
      meterInfo.cir       = RATE_LIMIT_QUATTRO;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_QUATTRO;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }
    else
    {
      meterInfo.cir       = RATE_LIMIT_PPPoE;
      meterInfo.cbs       = 128;
      meterInfo.pir       = RATE_LIMIT_PPPoE;
      meterInfo.pbs       = 128;
      meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
    }

    /* give dhcp frames high priority and trap to the CPU. */

    /* PPPoE packets from client */
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&pppoe_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;
    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished PPPoE trapping processing");

  return result;
}
#endif

/* PTin added: APS */
#if 1
/**
 * Create IFP rule to trap APS packets
 * 
 * @param vlanId       : VLAN ID value
 * @param vlan_match   : VLAN ID mask
 * @param ringId_level : Ring level
 * @param dapi_g 
 * @param policy_id    : Configured policy id (output)
 * 
 * @return L7_RC_t
 */
L7_RC_t hapiBroadConfigApsTrap(DAPI_USP_t *usp, L7_uint16 vlanId, L7_uint16 vlan_match, L7_uint8 ringId_level, DAPI_t *dapi_g,
                               BROAD_POLICY_t *policy_id)
{
#ifdef __APS_AND_CCM_COMMON_FILTER__
  if (L7_NULL==vlanId) return L7_SUCCESS;
  return hapiBroadConfigCcmFilter(usp, enable, vlanId, ringId_level, dapi_g);
#else
  L7_RC_t                 result = L7_SUCCESS;
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             aps_ethtype  = L7_ETYPE_CFM;
  L7_uchar8 aps_MacAddr[] = {0x01,0x19,0xA7,0x00,0x00,0x00};
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                          FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting APS trapping processing (vlanId %d, ringId %d)", vlanId, ringId_level);

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  /* APS packets on any port must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = RATE_LIMIT_APS;
  meterInfo.cbs       = 128;
  meterInfo.pir       = RATE_LIMIT_APS;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)
      break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy created");

    /* APS packets from client */
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  {
      break;
    }

    aps_MacAddr[5] = ringId_level;    /* Ring id */
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, aps_MacAddr, exact_match);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&aps_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;

    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  {
      break;
    }
    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  {
      break;
    }
    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  {
      break;
    }
    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  {
      break;
    }
  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished APS trapping processing");

  return result;
#endif //#ifdef __APS_AND_CCM_COMMON_FILTER__   #else
}
#endif

/* PTin added: Redirect ARP/IP packets to dtl0 */
#if 1
/**
 * Create IFP rule to trap APS packets
 * 
 * @param vlanId       : VLAN ID value
 * @param vlan_match   : VLAN ID mask
 * @param mac_addr     : MAC address
 * @param dapi_g 
 * @param policy_id    : Configured policy id (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadConfigIpDtl0Trap(L7_uint16 vlanId, L7_uint16 vlan_match, L7_uchar8 *mac_addr, DAPI_t *dapi_g,
                                  BROAD_POLICY_t *policy_id)
{
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
  L7_ushort16             ipdtl0_ethtype  = L7_ETYPE_ARP;
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                          FIELD_MASK_ALL, FIELD_MASK_ALL, FIELD_MASK_ALL};
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_RC_t                 result = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting dtl0 IP packets trapping processing (VLAN ID %d)", vlanId);

  /* Validate arguments */
  if (vlanId == 0 || vlanId >= 4096 || policy_id == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid VLAN (%u) or policyId pointer", vlanId);
  }

  /* IP packets on any port must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = RATE_LIMIT_IPDTL0;
  meterInfo.cbs       = 128;
  meterInfo.pir       = RATE_LIMIT_IPDTL0;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  do
  {
    result = hapiBroadPolicyCreate(policyType);
    if (result != L7_SUCCESS)
      break;

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy created");

    /* IP packets from client */
    result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (result != L7_SUCCESS)  {
      break;
    }

    if (mac_addr != L7_NULLPTR)
    {
      result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, mac_addr, exact_match);
      if (result != L7_SUCCESS)  break;
    }

    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *) &vlan_match);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipdtl0_ethtype, exact_match);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    if (result != L7_SUCCESS)  break;

    /* Trap the frames to CPU, so that they are not switched */
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (result != L7_SUCCESS)  break;

    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (result != L7_SUCCESS)  break;

  } while ( 0 );

  /* Commit policy */
  if (result == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy");

    result = hapiBroadPolicyCommit(&policyId);

    if (result == L7_SUCCESS)
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy %d commited successfully", *policy_id);
    else
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error commiting policy");
  }

  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    if (policy_id != L7_NULLPTR)  *policy_id = BROAD_POLICY_INVALID;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy");
  }
  else
  {
    if (policy_id != L7_NULLPTR)  *policy_id = policyId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished IP DTL0 trapping processing");

  return result;
}
#endif


/* PTin added: CCM */
#if 1
#define CCM_TRAP_MAX_VLANS 16

/* Policies related to CCM trap rules */
ptin_vlan_policy_trap_t ccm_policy[CCM_TRAP_MAX_VLANS];

L7_RC_t hapiBroadConfigCcmFilter(DAPI_USP_t *usp, L7_BOOL enable, L7_uint16 vlanId, L7_uchar8 oam_level, DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;
  static L7_BOOL          first_time = L7_TRUE;
  static struct {
      L7_ulong32    n_using;
      L7_ushort16   vid;
      L7_uchar8     lvl;
  } vid_lvl[CCM_TRAP_MAX_VLANS];
  BROAD_POLICY_RULE_t     ruleId = BROAD_POLICY_RULE_INVALID;
//L7_ushort16             ccm_ethtype  = L7_ETYPE_CCM;
#ifndef __APS_AND_CCM_COMMON_FILTER__
  L7_uchar8 ccm_MacAddr[] = {0x01,0x80,0xC2,0x00,0x00,0x37};
#endif
  L7_uchar8               exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                          FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uint16               vlan_match = 0xfff;
  BROAD_METER_ENTRY_t     meterInfo;
  BROAD_POLICY_TYPE_t     policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_ulong32 index, ccm_index, ccm_index_free;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Starting CCM trapping processing\tenable=%d\tvlanId=%u\toam_level=%u", enable, vlanId, oam_level);

  /* Initialization */
  if (first_time)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "First time processing... make some initializations");
    
    memset(ccm_policy, 0xff, sizeof(ccm_policy));
    memset(vid_lvl, 0, sizeof(vid_lvl));
    
    //ccm_enable = L7_FALSE;
    first_time   = L7_FALSE;
  } else {
    //LOG_TRACE(LOG_CTX_PTIN_HAPI, "CCM trapping is already configured");
    // Since vlanId is not used in HW rule only create rule once based on DMAC Addr
    //return L7_SUCCESS;
  }

 //#if (PTIN_SYSTEM_GROUP_VLANS)
 // LOG_TRACE(LOG_CTX_PTIN_HAPI,"Original vlan = %u",vlanId);
 // vlan_match = PTIN_VLAN_MASK(vlanId);
 // vlanId &= vlan_match;
 // LOG_TRACE(LOG_CTX_PTIN_HAPI,"vlan = %u, mask=0x%04x",vlanId,vlan_match);
 //#endif

  /* CCM packets on any port must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = RATE_LIMIT_CCM;
  meterInfo.cbs       = 128;
  meterInfo.pir       = RATE_LIMIT_CCM;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* If vlan value is valid, Find index */
  if (!(vlanId >= PTIN_VLAN_MIN && vlanId <= PTIN_VLAN_MAX)) return L7_FAILURE;
  if (oam_level>=8) return L7_FAILURE;

  for (index=0, ccm_index=ccm_index_free=-1; index<CCM_TRAP_MAX_VLANS; index++) {
      if (vid_lvl[index].n_using) {
          if (vid_lvl[index].vid!=vlanId    ||  vid_lvl[index].lvl!=oam_level) continue;

          if (enable) {
              vid_lvl[index].n_using++;
              return L7_SUCCESS;
          }
          else
          if (vid_lvl[index].n_using>1) {
              vid_lvl[index].n_using--;
              return L7_SUCCESS;
          }

          ccm_index=index;
      }
      else
      if (ccm_index_free>=CCM_TRAP_MAX_VLANS) ccm_index_free=index;
  }//for

  //If still here, we whether are...
  if (ccm_index>=CCM_TRAP_MAX_VLANS) {
      if (!enable) return L7_FAILURE;   //...deleting an inexistent entry
  }


  if (enable) { //...adding a new entry
      if (ccm_index_free>=CCM_TRAP_MAX_VLANS) return L7_TABLE_IS_FULL;
      index=ccm_index_free;
      vid_lvl[index].n_using=1;
      vid_lvl[index].vid=vlanId;
      vid_lvl[index].lvl=oam_level;
  }
  else {    // or deleting an entry with n==1
      index=ccm_index;
      vid_lvl[index].n_using=0;
      vid_lvl[index].vid=0;
      vid_lvl[index].lvl=0;
  }

 
  //WR to HW
  if (!enable) {
      (void)hapiBroadPolicyDelete(ccm_policy[index].policyId);
      ccm_policy[index].policyId = BROAD_POLICY_INVALID;
      return L7_SUCCESS;
  }


  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Processing cell #%u", index);

    do
    {
      result = hapiBroadPolicyCreate(policyType);
      if (result != L7_SUCCESS)
        break;

      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Policy of cell %u created", index);

      /* CCM packets from client */
      result = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
      if (result != L7_SUCCESS)  {
        break;
      }
      
#ifndef __APS_AND_CCM_COMMON_FILTER__
      ccm_MacAddr[5]&=0xf0;
      ccm_MacAddr[5]|=oam_level;
      result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, ccm_MacAddr, exact_match);
      if (result != L7_SUCCESS)  {
        break;
      }
#endif

      result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *)&vlan_match);
      if (result != L7_SUCCESS)  break;


      result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
      if (result != L7_SUCCESS)  {
        break;
      }

      /* Trap the frames to CPU, so that they are not switched */
      result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
      if (result != L7_SUCCESS)  {
        break;
      }
      result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
      if (result != L7_SUCCESS)  {
        break;
      }
      result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
      if (result != L7_SUCCESS)  {
        break;
      }
      result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
      if (result != L7_SUCCESS)  {
        break;
      }

#if defined (__APS_AND_CCM_COMMON_FILTER__) //|| defined(__LM_AND_DM_COMMON_FILTER__)
      {//MC DMAC can't be used for frames like LMR, DMR...
       BROAD_POLICY_RULE_t     ruleId2 = BROAD_POLICY_RULE_INVALID;
       L7_ushort16             ethtype;
#ifndef __APS_AND_CCM_COMMON_FILTER__
       L7_uchar8               this_prts_SMacAddr[6];

       {//SRC MAC ADDRESS
         L7_uint32 intIfNum;
         nimUSP_t  nim_usp;
             nim_usp.unit= usp->unit;
             nim_usp.slot= usp->slot;
             nim_usp.port= usp->port+1;
             if (L7_SUCCESS!= (result=nimGetIntIfNumFromUSP(&nim_usp, &intIfNum))
                 ||
                 L7_SUCCESS!= (result=nimGetIntfAddress(intIfNum, L7_SYSMAC_BIA, this_prts_SMacAddr))) {

                 LOG_ERR(LOG_CTX_PTIN_HAPI, "couldn't get SMAC");
                 break;
             }//memcpy(mep.src_mac_address, &s, 6);
       }//SRC MAC ADDRESS
#endif

#ifdef __APS_AND_CCM_COMMON_FILTER__
       ruleId2 = ruleId;
#else
       hapiBroadPolicyRuleCopy(ruleId, &ruleId2);
#endif

       //Realized just now: opposite to APSfilter, CCMs aren't filtering ETHtype; to change that, just move RuleCopy below this qualifier and change it to "ruleId", instead of "ruleId2"
       ethtype  = L7_ETYPE_CFM;
       result = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ethtype, exact_match);
       if (result != L7_SUCCESS)  break;

#ifndef __APS_AND_CCM_COMMON_FILTER__
       result = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_MACDA, this_prts_SMacAddr, exact_match);
       //in this other rule, the MC DMAC match is overwritten by this one
       if (result != L7_SUCCESS) break;
#else
       {//OAM ETH's MEL 3 bits (Check SDK's _bcm_tr3_oam_fp_create())
        bcm_ip6_t mdl_data, mdl_mask;

        memset(&mdl_data, 0, sizeof(bcm_ip6_t));
        mdl_data[0] = oam_level << 5;
        memset(&mdl_mask, 0, sizeof(bcm_ip6_t));
        mdl_mask[0] = 0xE0;

        result = hapiBroadPolicyRuleQualifierAdd(ruleId2, BROAD_FIELD_IP6_DST, mdl_data, mdl_mask);
        if (result != L7_SUCCESS) break;
       }
      }//MC DMAC can't be used for frames like LMR, DMR...
#endif
#endif //#if defined (__APS_AND_CCM_COMMON_FILTER__) //|| defined(__LM_AND_DM_COMMON_FILTER__)




    } while ( 0 );

    if (result == L7_SUCCESS)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Commiting policy of cell %u", index);
      if ((result=hapiBroadPolicyCommit(&ccm_policy[index].policyId)) == L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI, "policy of cell %u commited successfully", index);
      }
    }
    else
    {
      hapiBroadPolicyCreateCancel();

      ccm_policy[index].policyId = BROAD_POLICY_INVALID;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: canceling policy of cell %u", index);
    }

    if (result != L7_SUCCESS && ccm_policy[index].policyId != BROAD_POLICY_INVALID )
    {
      /* attempt to delete the policy in case it was created */
      (void)hapiBroadPolicyDelete(ccm_policy[index].policyId);

      ccm_policy[index].policyId = BROAD_POLICY_INVALID;
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Some error ocurred: deleting policy of cell %u", index);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished CCM trapping processing");

  return result;
}
#endif


#if 1
/**
 * Dump list of bw policers
 */
void ptin_traprules_dump(void)
{
  L7_int index, rule;
  //L7_uint16 (*vlan_list)[2];
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;
  int policer_id, counter_id;
  BROAD_POLICY_STATS_t stat;
  L7_RC_t rc;
  BROAD_SYSTEM_t *hapiSystem;

  /* Validate pointers */
  if (dapi_g != L7_NULLPTR && dapi_g->system != L7_NULLPTR && dapi_g->system->hapiSystem != L7_NULLPTR)
  {
    /* hapiSystem pointer */
    hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

    printf("ARP packets trap: ");

    if (hapiSystem->dynamicArpInspectUntrustedPolicyId == 0 || hapiSystem->dynamicArpInspectUntrustedPolicyId == BROAD_POLICY_INVALID)
    {
      printf("--not defined--\r\n");
    }
    else
    {
      printf("policyId=%u\r\n", hapiSystem->dynamicArpInspectUntrustedPolicyId);

      rule = 0;
      while (((rc=l7_bcm_policy_hwInfo_get(0, hapiSystem->dynamicArpInspectUntrustedPolicyId,
                                           rule, &group_id, &entry_id, &policer_id, &counter_id))==L7_SUCCESS))
      {
        printf("  rule=%u -> group=%-2d, entry=%-4d (PolicerId=%-4d CounterId=%-4d)",
               rule, group_id, entry_id, policer_id, counter_id);

        /* Check counter */
        if (counter_id > 0)
        {
          printf(": Packets=");
          /* Get stat data */
          if (hapiBroadPolicyStatsGet(hapiSystem->dynamicArpInspectUntrustedPolicyId, rule, &stat) != L7_SUCCESS)
            printf("Error");
          else
            printf("%llu", stat.statMode.counter.count);
        }
        printf("\r\n");

        rule++;
      }
    }
    printf("\r\n");
  }

  /* Run all indexes */
  for (index = 0; index < PTIN_TRAP_POLICY_MAX_VLANS; index++)
  {
    /* Entry is used? */
    if (!ptin_trap_policy[index].in_use)
    {
      continue;
    }

    group_id   = entry_id   = -1; 
    policer_id = counter_id = -1;

    /* Get hardware settings of first entry */
    if (ptin_trap_policy[index].policyId > 0 && ptin_trap_policy[index].policyId < BROAD_POLICY_INVALID)
    {
      rc = l7_bcm_policy_hwInfo_get(0, ptin_trap_policy[index].policyId, 0, &group_id, &entry_id, &policer_id, &counter_id);
    }
    else
    {
      rc = L7_NOT_EXIST;
    }

    /* Run all related entries */
    rule  = 0;
    do
    {
      /* Show an asterisk if configured in hardware */
      if (rc == L7_SUCCESS && rule == 0)
        printf("*");
      else
        printf(" ");

      /* Show policy and rule indexes */
      if (rc == L7_SUCCESS)
        printf("<%3u/%u> ", index, rule);
      else
        printf("<%3u/x> ", index);

      /* Packet type */
      switch (ptin_trap_policy[index].packet_type)
      {
        case PTIN_PACKET_IGMP:
          printf("[IGMP "); break;
        case PTIN_PACKET_MLD:
          printf("[MLD  "); break;
        case PTIN_PACKET_DHCP:
          printf("[DHCP "); break;
        case PTIN_PACKET_PPPOE:
          printf("[PPPoE"); break;
        case PTIN_PACKET_APS:
          printf("[APS  "); break;
        case PTIN_PACKET_CCM:
          printf("[CCM  "); break;
        case PTIN_PACKET_IPDTL0:
          printf("[IPDTL"); break;
        default:
          printf("[ ??? "); break;
      }
      /* IP version */
      if (ptin_trap_policy[index].ip_version == L7_AF_INET6)
        printf("/%s] ", "IPv6");
      else if (ptin_trap_policy[index].ip_version == L7_AF_INET)
        printf("/%s] ", "IPv4");
      else
        printf("]      ");

      /* VLAN */
      if (ptin_trap_policy[index].vlan > 0 && ptin_trap_policy[index].vlan < 4096)
      {
        printf("VLAN %4u/0x%03x: ", ptin_trap_policy[index].vlan, ptin_trap_policy[index].vlan_mask);
      }
      
      /* If hardware is configured... */      
      if (rc == L7_SUCCESS)
      {
        /* Also print hw group id and entry id*/
        printf("policyId=%-4u rule=%u -> group=%-2d, entry=%-4d (PolicerId=%-4d CounterId=%-4d)",
               ptin_trap_policy[index].policyId, rule, group_id, entry_id, policer_id, counter_id);

        /* Check counter */
        if (counter_id > 0)
        {
          printf(": Packets=");
          /* Get stat data */
          if (hapiBroadPolicyStatsGet(ptin_trap_policy[index].policyId, rule, &stat) != L7_SUCCESS)
            printf("Error");
          else
            printf("%llu", stat.statMode.counter.count);
        }
      }
      else
      {
        printf("Not active!");
      }
      printf("\r\n");

      /* Next rule */
      rule++;

    } while ((ptin_trap_policy[index].policyId > 0 && ptin_trap_policy[index].policyId < BROAD_POLICY_INVALID) &&
             ((rc=l7_bcm_policy_hwInfo_get(0, ptin_trap_policy[index].policyId, rule, &group_id, &entry_id, &policer_id, &counter_id))==L7_SUCCESS));
  }

  printf("Global admin:\r\n");
  for (index = 1; index < PTIN_PACKET_LAST; index++)
  {
    /* Packet type */
    switch (index)
    {
      case PTIN_PACKET_IGMP:
        printf("IGMP:"); break;
      case PTIN_PACKET_MLD:
        printf("MLD:"); break;
      case PTIN_PACKET_DHCP:
        printf("DHCP:"); break;
      case PTIN_PACKET_PPPOE:
        printf("PPPoE:"); break;
      case PTIN_PACKET_APS:
        printf("APS:"); break;
      case PTIN_PACKET_CCM:
        printf("CCM:"); break;
      case PTIN_PACKET_IPDTL0:
        printf("IPDTL:"); break;
      default:
        printf("???:"); break;
    }
    printf("%u  ", ptin_trap_policy_global_enable[index]);
  }

  printf("\r\nDone!\r\n");
  fflush(stdout);
}
#endif

/*********************************************************************
*
* @functions hapiBroadIntfMaxFrameSizeConfig
*
* @purpose config physical port layer 2 max frame size
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG */
L7_RC_t hapiBroadIntfMaxFrameSizeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t  *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  L7_uint32              index;
  usl_bcm_port_frame_size_t  maxFrameSize;
  int                    rc;

  if (dapiCmd->cmdData.frameSizeConfig.getOrSet == DAPI_CMD_GET)
  {
    result =  L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: Failure in %s!\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  maxFrameSize = dapiCmd->cmdData.frameSizeConfig.maxFrameSize;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Max Frame Size is allowed only on physical ports */
  if (IS_PORT_TYPE_PHYSICAL (dapiPortPtr) == L7_TRUE)
  {
    rc = usl_bcmx_port_frame_max_set(hapiPortPtr->bcmx_lport, maxFrameSize);
    if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc!=BCM_E_UNAVAIL))
    {
      result = L7_FAILURE;
    }
  }
  else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadLagCritSecEnter ();
    for (index=0;index < L7_MAX_MEMBERS_PER_LAG; index++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[index].inUse == L7_TRUE)
      {
        hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[index].usp, dapi_g);

        rc = usl_bcmx_port_frame_max_set(hapiPortPtr->bcmx_lport, maxFrameSize);
        if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc!=BCM_E_UNAVAIL))
        {
          result = L7_FAILURE;
        }
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s DAPI_CMD_GET_SET_t type not supported\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  return result;
}

/* For XGS3 parts prior to B1, there is a known issue with Opcode 0 (CPU-CPU data)
 * This function determines whether or not the patch is necessary for a specific
 * unit
 */
#include <bcm_int/control.h>
int hapiBroadXgs3HigigPriorityPatch(int unit)
{
   int valid_rev = 0;
   uint16              dev_id;
   uint8               rev_id;

    /* Check device type and revision */
    if ((!BCM_UNIT_VALID(unit)) || (!BCM_IS_LOCAL(unit))) {
     valid_rev = 0;    
   }
   else
   {
     /* Need to check the Revision ID of the device, not the driver */
     if ((unit < SOC_MAX_NUM_DEVICES) && SOC_UNIT_VALID(unit))
     {
        soc_cm_get_id(unit, &dev_id, &rev_id);

        if (SOC_IS_FIREBOLT(unit) &&
            ((rev_id == BCM56504_A0_REV_ID) ||
             (rev_id == BCM56504_A1_REV_ID))) {
           valid_rev = 1;
        } else if (SOC_IS_HELIX1(unit) &&
                   ((rev_id == BCM56304_A0_REV_ID) ||
                   (rev_id == BCM56304_B0_REV_ID))) {
           valid_rev = 1;
        } else if (SOC_IS_FELIX(unit) &&
                   (rev_id == BCM56107_A0_REV_ID)) {
           valid_rev = 1;
        }
     }
   }

   return valid_rev;
}

int hapiBroadBcmGroupRequired(unit)
{
  return hapiBroadXgs3HigigPriorityPatch(unit);
}


/* Used for the bcm driver to get the Stacking VLAN */
int hapiBroadStackingVlanGet(void)
{
  return HPC_STACKING_VLAN_ID;
}


/*********************************************************************
*
* @purpose To apply the Denial of Service configuration
*
* @param   DOSCONTROLCONFIG_t *dosCtlCfg - DOS configuration
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosControlConfigApply(DOSCONTROLCONFIG_t *dosCtlCfg)
{
  L7_RC_t               result       = L7_SUCCESS;
  int                   rv           = BCM_E_NONE;

  rv = usl_bcmx_doscontrol_set(dosCtlCfg->dosType, dosCtlCfg->enable, dosCtlCfg->arg);
   
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    result = L7_FAILURE;
  }

  return result;
}

/*********************************************************************
*
* @purpose To configure Denial of Service functions
*
* @param   DAPI_USP_t *usp    - usp of the port on which ipv6 provisioning
*                               feature is being enabled or disabled
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosControlConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result       = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t     *dapiCmd     = (DAPI_SYSTEM_CMD_t*)data;
  int                   arg          = L7_ENABLE;

  if (dapiCmd->cmdData.dosControlConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (dapiCmd->cmdData.dosControlConfig.enable != L7_TRUE)
  {
    arg = L7_FALSE;
  }

  switch (dapiCmd->cmdData.dosControlConfig.dosType)
  {
   case DOSCONTROL_PINGFLOODING:
       result = hapiBroadSystemDosPingFloodingFilter(usp,dapi_g,data,arg);
       break; 

   case DOSCONTROL_SMURFATTACK:
       result = hapiBroadSystemDosSmurfAttackFilter(usp,dapi_g,data,arg);
       break;

   case DOSCONTROL_SYNACKFLOODING: 
       result = hapiBroadSystemDosSynAckFloodingFilter(usp,dapi_g,data,arg);
       break;

   default:
       result = hapiBroadSystemDosControlConfigApply(&dapiCmd->cmdData.dosControlConfig);
       break;
  }
  return result;
}

/*********************************************************************
*
* @purpose To configure Per Port Denial of Service functions
*
* @param   DAPI_USP_t *usp    - usp of the port on which ipv6 provisioning
*                               feature is being enabled or disabled
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DOSCONTROL_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.dosControlConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDosControlConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result       = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t *dapiCmd     = (DAPI_INTF_MGMT_CMD_t*)data;
  int                   rv;
  int                   arg          = L7_ENABLE;

  if (dapiCmd->cmdData.dosControlConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }
 
  if (dapiCmd->cmdData.dosControlConfig.enable != L7_TRUE)
  {
    arg = L7_DISABLE;
  }

  switch (dapiCmd->cmdData.dosControlConfig.dosType)
  {
     case DOSCONTROL_PINGFLOODING:
       rv = hapiBroadSystemDosPingFloodingFilter(usp,dapi_g,data,arg);
       if (rv != L7_SUCCESS)
         return L7_FAILURE;
       break;  
     case DOSCONTROL_SMURFATTACK:
       rv = hapiBroadSystemDosSmurfAttackFilter(usp,dapi_g,data,arg);
       if (rv != L7_SUCCESS)
         return rv;
       break;
   case DOSCONTROL_SYNACKFLOODING:
       rv = hapiBroadSystemDosSynAckFloodingFilter(usp,dapi_g,data,arg);
       if (rv != L7_SUCCESS)
         return rv;
       break;
        default:
       result = L7_FAILURE;
       SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                      "\n%s %d: In %s invalid Denial of Service Type\n",
                      __FILE__, __LINE__, __FUNCTION__);
       return result;
       break;
   }
   return result;
}


/*********************************************************************
*
* @purpose Set the BPDU FIletring for the port
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_FILTERING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduFiltering
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduFiltering(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{     
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  L7_RC_t rc = L7_SUCCESS;
  BROAD_PORT_t         *hapiPortPtr;

  if( dapiCmd->cmdData.bpduFiltering.getOrSet != DAPI_CMD_SET )
  {
     rc = L7_FAILURE;
     SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                    "\n%s %d: In %s invalid getOrSet Command\n", __FILE__, 
                       __LINE__, __FUNCTION__ );
     return rc;
  }

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr->bpduFilterMode = dapiCmd->cmdData.bpduFiltering.enable;
  return rc;
}

/*********************************************************************
*
* @purpose Set the BPDU Guard mode for the port
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_GUARD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduGuardMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduGuard(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, 
                               DAPI_t *dapi_g)
{     
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  L7_RC_t rc = L7_FAILURE;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_PORT_t           *dapiPortPtr;
  
  if( dapiCmd->cmdData.bpduGuardMode.getOrSet != DAPI_CMD_SET )
  {
     SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                    "\n%s %d: In %s invalid getOrSet Command\n", __FILE__, 
                       __LINE__, __FUNCTION__ );
     return rc;
  }

  if (!isValidUsp(usp, dapi_g))
    return rc;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL (dapiPortPtr) != L7_TRUE) &&
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) != L7_TRUE))
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s DAPI_CMD_GET_SET_t type not supported\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return rc;
  }

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr->bpduGuardMode = dapiCmd->cmdData.bpduGuardMode.enable;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the BPDU Flood for the port
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_FLOOD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduFlood
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduFlood(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{     
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  L7_RC_t rc = L7_SUCCESS;
  BROAD_PORT_t         *hapiPortPtr;

  if( dapiCmd->cmdData.bpduFlood.getOrSet != DAPI_CMD_SET )
  {
     rc = L7_FAILURE;
     SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                    "\n%s %d: In %s invalid getOrSet Command\n", __FILE__, 
                       __LINE__, __FUNCTION__);
     return rc;
  }

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr->bpduFloodMode = dapiCmd->cmdData.bpduFlood.enable;
  return rc;
}

/*********************************************************************
*
* @functions hapiBroadCableDiagOkayLength
*
* @purpose   determine the length of a good cable within a range
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadCableDiagOkayLength(bcm_port_cable_diag_t cd, L7_uint32 *sl, L7_uint32 *ll)
{
  int i;
  int slen=0, llen=0;


  /* The cable test failed pair length should not consider to calculate 
   * cable length range 
   * Intialize slen and llen to first OK (passed) ed pair length
   */
  for (i = 0; i < cd.npairs; i++)
  {
    if(cd.pair_state[i] == BCM_PORT_CABLE_STATE_OK)
    {    
      slen = llen = cd.pair_len[i];
      break;
    }
  }

  /* find the shortest / longest length of any pair */
  for (i = 0; i < cd.npairs; i++)
  {
    if(cd.pair_state[i] != BCM_PORT_CABLE_STATE_OK)
       continue; 
    if (cd.pair_len[i] < slen)
      slen = cd.pair_len[i];
    if (cd.pair_len[i] > llen)
      llen = cd.pair_len[i];
  }

  /* adjust for fuzz */
  if (slen - cd.fuzz_len < 0)
    *sl = 0;
  else
    *sl = slen - cd.fuzz_len;

  *ll = llen + cd.fuzz_len;

  return L7_TRUE;
}

/*********************************************************************
*
* @functions hapiBroadCableDiagFailLength
*
* @purpose   determine location of failure on a cable
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadCableDiagFailLength(bcm_port_cable_diag_t cd, L7_uint32 *fl)
{
  int i;

  /* report the first failed pair only */
  for (i = 0; i < cd.npairs; i++)
  {
    if (cd.pair_state[i] != BCM_PORT_CABLE_STATE_OK)
    {
      /* We can only report one value so return the middle of the range. */
      *fl = cd.pair_len[i];
      
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @functions hapiBroadIntfCableTest
*
* @purpose check cable status
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfCableTest(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)

{
  L7_RC_t               result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_PORT_t          *dapiPortPtr;
  int                   rv, i;
  int                   portSpeed;
  int                   noOfActivePairs;
  bcm_port_cable_diag_t cd;

  DAPI_CABLE_STATUS_t   dapiStatus      = DAPI_CABLE_TEST_FAIL;
  L7_BOOL               dapiLengthKnown = L7_FALSE;
  L7_uint32             dapiShortLength = 0;
  L7_uint32             dapiLongLength  = 0;
  L7_uint32             dapiFailLength  = 0;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if (dapiCmd->cmdData.cableStatusGet.getOrSet == DAPI_CMD_SET)
  {
    return L7_FAILURE;
  }

  /* run diag */
  rv = usl_bcmx_port_copper_diag_get(hapiPortPtr->bcmx_lport, &cd);
  /* The api can return values > 0. So check for negative error codes only. */
  if (rv < BCM_E_NONE)
  {
    if (rv == BCM_E_UNAVAIL)
    {
      return L7_NOT_SUPPORTED;
    }
    else
    {
      return L7_FAILURE;
    }
  }

  /* Retain the debug information as it is more detailed than what is available
   * via the mgmt interface. It might be necessary to have this level of detail
   * when debugging a real issue.
   */
  if (debugCableDiag)
  {

    printf("\n\nCable diag result: %d\n", cd.state);
    for (i = 0; i < cd.npairs; i++)
    {
      printf("Pair %c is %d, length %2dm (+/-%d)\n",
             'A' + i,
             cd.pair_state[i],
             cd.pair_len[i],
             cd.fuzz_len);
    }
  }

  portSpeed = hapiPortPtr->speed;
  switch (portSpeed)
  {
    case DAPI_PORT_SPEED_GE_1GBPS:
      break;
    case DAPI_PORT_SPEED_FE_100MBPS:
    case DAPI_PORT_SPEED_FE_10MBPS:
      if (cd.npairs == 4 )
      {
        noOfActivePairs = 0;
        for (i = 0; i < cd.npairs; i++)
        {
          if(cd.pair_state[i] == BCM_PORT_CABLE_STATE_OK)
          {
            noOfActivePairs++;
          }
        }
        if (noOfActivePairs >= 2)
          cd.state = BCM_PORT_CABLE_STATE_OK;
      }  
      break;
    default:
      break;
  }


  switch (cd.state)
  {
    case BCM_PORT_CABLE_STATE_OK:
      dapiStatus      = DAPI_CABLE_NORMAL;
      dapiLengthKnown = hapiBroadCableDiagOkayLength(cd, &dapiShortLength, &dapiLongLength);
      break;
    case BCM_PORT_CABLE_STATE_OPEN:
      dapiStatus      = DAPI_CABLE_OPEN;
      dapiLengthKnown = hapiBroadCableDiagFailLength(cd, &dapiFailLength);
      break;
    case BCM_PORT_CABLE_STATE_SHORT:
    case BCM_PORT_CABLE_STATE_OPENSHORT:
      dapiStatus      = DAPI_CABLE_SHORT;
      dapiLengthKnown = hapiBroadCableDiagFailLength(cd, &dapiFailLength);
      break;
    default:
      dapiStatus = DAPI_CABLE_TEST_FAIL;
      break;
  }

  dapiCmd->cmdData.cableStatusGet.cable_status         = dapiStatus;
  dapiCmd->cmdData.cableStatusGet.length_known         = dapiLengthKnown;
  dapiCmd->cmdData.cableStatusGet.shortest_length      = dapiShortLength;
  dapiCmd->cmdData.cableStatusGet.longest_length       = dapiLongLength;
  dapiCmd->cmdData.cableStatusGet.cable_failure_length = dapiFailLength;

  /* Cable test on administratively disbaled (shutdown port) port 
   * causes port link up after the test. This is due the PHY reset 
   * during cable test.
   * After the cable test restore the port admini state if the port is 
   * administratively disabled.
   */
  if (hapiPortPtr->hapiModeparm.physical.admin_enabled == L7_FALSE)
  {
    rv = usl_bcmx_port_enable_set(hapiPortPtr->bcmx_lport, 
                                  hapiPortPtr->hapiModeparm.physical.admin_enabled);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Functions to add or delete a PING flooding filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which DOS Ping flooding filter 
*                               feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter  - L7_ENABLE:  To add a filter entry
*                                     L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosPingFloodingFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data, 
                                            L7_uint32 enableFilter)
{
      /* Any Destination Ip */
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_SYSTEM_CMD_t     *dapiCmd     = (DAPI_SYSTEM_CMD_t*)data;
  static BROAD_POLICY_t pingFlood_id=BROAD_POLICY_INVALID;
  static BROAD_POLICY_RULE_t   rule_id = BROAD_POLICY_RULE_INVALID;
  L7_RC_t               result = L7_SUCCESS;
  bcmx_lport_t          lport;
  L7_ushort16           ip_ethtype = L7_ETYPE_IP;
  L7_uchar8             icmp_proto[]  = {IP_PROT_ICMP}; 
  L7_uchar8             exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};  
  static L7_uint32      count = 0;
  static L7_uint32 rate=0;
  BROAD_METER_ENTRY_t meterDef = {0, 0, 0, 0, BROAD_METER_COLOR_BLIND};
  

  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);  
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (dapiCmd->cmdData.dosControlConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  meterDef.colorMode = BROAD_METER_COLOR_BLIND;
   /* used for all policing types: simple, single and two-rate  */
  meterDef.cir = dapiCmd->cmdData.dosControlConfig.arg;
  meterDef.cbs = 128;
 
  /* Get the logical BCMX port */
  lport  = hapiPortPtr->bcmx_lport;

  if(pingFlood_id == BROAD_POLICY_INVALID)
  {
    if (enableFilter == L7_ENABLE) 
    {
      int              bcm_unit;
      bcm_port_t       bcm_port;
      rate = meterDef.cir;
      if (BCM_E_NONE != bcmx_lport_to_unit_port(lport, &bcm_unit, &bcm_port))
      {
         return L7_FAILURE;
      }
     
      hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);

      hapiBroadPolicyRuleAdd(&rule_id);
      /* ETHER TYPE = IP */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match);
      /* Protocol ICMP */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_PROTO,   icmp_proto,  exact_match);
      hapiBroadPolicyRuleActionAdd(rule_id, BROAD_ACTION_PERMIT,   0, 0, 0);
      hapiBroadPolicyRuleNonConfActionAdd(rule_id,BROAD_ACTION_HARD_DROP,0,0,0);
      hapiBroadPolicyRuleMeterAdd(rule_id, &meterDef);

      result = hapiBroadPolicyCommit(&pingFlood_id);
      if (L7_SUCCESS == result)
      {
        result = hapiBroadPolicyApplyToIface(pingFlood_id, hapiPortPtr->bcmx_lport);
        if (L7_SUCCESS == result)
        {
          hapiPortPtr->pingFloodPolicyApplied = L7_TRUE;
          count++;
        }
      }
      else
      {
        (void)hapiBroadPolicyDelete(pingFlood_id);
        hapiPortPtr->pingFloodPolicyApplied = L7_FALSE;
        pingFlood_id = BROAD_POLICY_INVALID;
        rule_id = BROAD_POLICY_RULE_INVALID;
        count = 0;
        rate = 0;
      }
    }
  }
  else
  {
    if(enableFilter == L7_ENABLE)
    {
      if( meterDef.cir != rate)
      {
        rate = meterDef.cir;
        result = hapiBroadPolicyUpdateStart(pingFlood_id);
        if(result == L7_SUCCESS)
        {
          hapiBroadPolicyRuleMeterAdd(rule_id, &meterDef);
          result = hapiBroadPolicyUpdateFinish();
        }
        if(result != L7_SUCCESS)
        {
          (void)hapiBroadPolicyDelete(pingFlood_id);
          hapiPortPtr->pingFloodPolicyApplied = L7_FALSE;
          pingFlood_id = BROAD_POLICY_INVALID;
          rule_id = BROAD_POLICY_RULE_INVALID;
          count = 0;
          rate = 0;
        }
      }

      if(result == L7_SUCCESS)
      {
        if (hapiPortPtr->pingFloodPolicyApplied == L7_FALSE)
        {
          result = hapiBroadPolicyApplyToIface(pingFlood_id, hapiPortPtr->bcmx_lport);
          if (result == L7_SUCCESS)
          {
            hapiPortPtr->pingFloodPolicyApplied = L7_TRUE;
            count++;
          }
        }
      }
    }
    else if(enableFilter == L7_DISABLE)
    {
      if (hapiPortPtr->pingFloodPolicyApplied == L7_TRUE)
      {
        result = hapiBroadPolicyRemoveFromIface(pingFlood_id, hapiPortPtr->bcmx_lport);
        if(result == L7_SUCCESS)
        {
          hapiPortPtr->pingFloodPolicyApplied = L7_FALSE;
          --count;
          /* If there is no port on which current policy is active delete it */
          if(count <= 0)
          {
            (void)hapiBroadPolicyDelete(pingFlood_id);
            pingFlood_id = BROAD_POLICY_INVALID;
            rule_id = BROAD_POLICY_RULE_INVALID; 
            count = 0;
            rate = 0;
          }
        }
      }
    }
  }
  return result;
}
  

/*********************************************************************
*
* @purpose Functions to add or delete a Smurf attack filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which DOS smurf attack 
*                               feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter - L7_ENABLE:  To add a filter entry
*                                    L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosSmurfAttackFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
        L7_uint32 enableFilter)
{
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_SYSTEM_CMD_t     *dapiCmd     = (DAPI_SYSTEM_CMD_t*)data;
  static BROAD_POLICY_t smurfPolicy_id = BROAD_POLICY_INVALID;
  static L7_uint32      count = 0;
  BROAD_POLICY_RULE_t   rule_id;
  L7_RC_t               result = L7_SUCCESS;
  L7_ushort16           ip_ethtype = L7_ETYPE_IP;
  L7_uchar8             icmp_proto[]  = {IP_PROT_ICMP};
  L7_uchar8             exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uchar8             dstIpValue[] = {0x00, 0x00, 0x00, 0xFF};
  L7_uchar8             dstIpMask[]  = {0x00, 0x00, 0x00, 0xFF};



  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  if (dapiCmd->cmdData.dosControlConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* Policy does not exist */ 
  if(smurfPolicy_id == BROAD_POLICY_INVALID)
  {
    if(enableFilter == L7_ENABLE)
    {
      hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);

      hapiBroadPolicyRuleAdd(&rule_id);
      /* ETHER TYPE = IP */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match);
     
      /* Protocol ICMP */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_PROTO,   icmp_proto,  exact_match);
      /* block directed braodcast Ping (X.X.X.255) */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_DIP,(L7_uchar8 *)&dstIpValue,
      (L7_uchar8 *)&dstIpMask);
      /*Action */
      hapiBroadPolicyRuleActionAdd(rule_id, BROAD_ACTION_HARD_DROP,   0, 0, 0);
      
      result = hapiBroadPolicyCommit(&smurfPolicy_id);
      if (L7_SUCCESS == result)
      {
        result = hapiBroadPolicyApplyToIface(smurfPolicy_id, hapiPortPtr->bcmx_lport);
        if (L7_SUCCESS == result)
        {
          hapiPortPtr->smurfAttackPolicyApplied = L7_TRUE;
          count++;
        }
      }
      else
      {
        (void)hapiBroadPolicyDelete(smurfPolicy_id);
        hapiPortPtr->smurfAttackPolicyApplied = L7_FALSE;
        smurfPolicy_id = BROAD_POLICY_INVALID;
        count = 0;
      }
    }
  }
  /* Policy exist need to remove or add for old/new interface */
  else
  {
    if(enableFilter == L7_ENABLE)
    {
      if (hapiPortPtr->smurfAttackPolicyApplied == L7_FALSE)
      {
        result = hapiBroadPolicyApplyToIface(smurfPolicy_id, hapiPortPtr->bcmx_lport);
        if (result == L7_SUCCESS)
        {
          hapiPortPtr->smurfAttackPolicyApplied = L7_TRUE;
          count++;
        }
      }
    }
    else if(enableFilter == L7_DISABLE)
    {
      if (hapiPortPtr->smurfAttackPolicyApplied == L7_TRUE)
      {
        result = hapiBroadPolicyRemoveFromIface(smurfPolicy_id, hapiPortPtr->bcmx_lport);
        if(result == L7_SUCCESS)
        {
          hapiPortPtr->smurfAttackPolicyApplied = L7_FALSE;
          --count;        
          /* If there is no port on which current policy is active delete it */ 
          if(count <= 0)
          {
            (void)hapiBroadPolicyDelete(smurfPolicy_id);
            smurfPolicy_id = BROAD_POLICY_INVALID;
            count = 0;
          }
        }
      }
    }
  }
  return result;
}

/*********************************************************************
*
* @purpose Functions to add or delete a SYN ACK flooding filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which SYN ACK Flooding
*                               filter feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter - L7_ENABLE:  To add a filter entry
*                               L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosSynAckFloodingFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
                                              L7_uint32 enableFilter)
{
  DAPI_PORT_t          *dapiPortPtr;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_SYSTEM_CMD_t     *dapiCmd     = (DAPI_SYSTEM_CMD_t*)data;
  static BROAD_POLICY_t        synFlood_id = BROAD_POLICY_INVALID;
  static BROAD_POLICY_RULE_t   rule_id = BROAD_POLICY_RULE_INVALID;
  L7_uint32             result = L7_SUCCESS;
  bcmx_lport_t          lport;
  int                   bcm_unit;
  bcm_port_t            bcm_port;
  L7_uchar8             ip_ethtype[]  = {0x08, 0x00};
  L7_uchar8             tcp_proto[]  = {0x06}; 
  L7_uchar8             exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};  
  L7_uint32             dstIpValue = 0x0;
  L7_uint32             dstIpMask  = 0x0;
  static L7_uint32      count = 0;
  static L7_uint32    rate=0;

  L7_uchar8             tcp_synack[] = {0x12};
  L7_uchar8             tcp_synack_mask[] = {0x3f};
  BROAD_METER_ENTRY_t meterDef = {0, 0, 0, 0, BROAD_METER_COLOR_BLIND};
 
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);  
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (dapiCmd->cmdData.dosControlConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  meterDef.colorMode = BROAD_METER_COLOR_BLIND;
   /* used for all policing types: simple, single and two-rate  */
  meterDef.cir = dapiCmd->cmdData.dosControlConfig.arg;
  meterDef.cbs = 128;

  /* Get the logical BCMX port */
  lport  = hapiPortPtr->bcmx_lport;

  if( synFlood_id  == BROAD_POLICY_INVALID )
  {
    if (enableFilter == L7_ENABLE) 
    {
      rate =  meterDef.cir;
      if (BCM_E_NONE != bcmx_lport_to_unit_port(lport, &bcm_unit, &bcm_port))
      {  
        return L7_FAILURE;
      }

      hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
       
      hapiBroadPolicyRuleAdd(&rule_id);
      if  (hapiBroadRoboCheck() != L7_TRUE)
      {
        /* ETHER TYPE = IP */
        hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_ETHTYPE,ip_ethtype, exact_match);
        /* Protocol TCP */
        hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_PROTO,   tcp_proto,  exact_match);
      }
      /* Any Destination Ip */
      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_DIP,(L7_uchar8 *)&dstIpValue,(L7_uchar8 *)&dstIpMask);

      hapiBroadPolicyRuleQualifierAdd(rule_id, BROAD_FIELD_TCP_CONTROL,   tcp_synack ,  tcp_synack_mask );

      hapiBroadPolicyRuleMeterAdd(rule_id, &meterDef);
      hapiBroadPolicyRuleActionAdd(rule_id, BROAD_ACTION_PERMIT,   0, 0, 0);
      hapiBroadPolicyRuleNonConfActionAdd(rule_id,BROAD_ACTION_HARD_DROP,0,0,0);
      result = hapiBroadPolicyCommit(&synFlood_id );
      if (L7_SUCCESS == result)
      {
        result = hapiBroadPolicyApplyToIface(synFlood_id , hapiPortPtr->bcmx_lport);
        if (L7_SUCCESS == result)
        {
          hapiPortPtr->synAckPolicyApplied = L7_TRUE;
          count++;
        }
      }
      else
      {
        (void)hapiBroadPolicyDelete(synFlood_id );
        hapiPortPtr->synAckPolicyApplied = L7_FALSE;
        synFlood_id  = BROAD_POLICY_INVALID;
        rule_id = BROAD_POLICY_RULE_INVALID;
        count = 0;
        rate = 0;
      }
    }
  }
  else
  {
    if(enableFilter == L7_ENABLE)
    {
      if( meterDef.cir != rate)
      {
        rate = meterDef.cir;
        result = hapiBroadPolicyUpdateStart(synFlood_id);
        if(result == L7_SUCCESS)
        {
          hapiBroadPolicyRuleMeterAdd(rule_id, &meterDef);
          result = hapiBroadPolicyUpdateFinish();
        }
        if(result != L7_SUCCESS)
        {
          (void)hapiBroadPolicyDelete(synFlood_id );
          hapiPortPtr->synAckPolicyApplied = L7_FALSE;
          synFlood_id  = BROAD_POLICY_INVALID;
          rule_id = BROAD_POLICY_RULE_INVALID;
          count = 0;
          rate = 0;
        }
      }

      if(result == L7_SUCCESS)
      {
        if (hapiPortPtr->synAckPolicyApplied == L7_FALSE)
       {
         result = hapiBroadPolicyApplyToIface(synFlood_id, hapiPortPtr->bcmx_lport);
          if (result == L7_SUCCESS)
         {
            hapiPortPtr->synAckPolicyApplied = L7_TRUE;
            count++;
          }
        }
      }
    }
    else if(enableFilter == L7_DISABLE)
    {
      if (hapiPortPtr->synAckPolicyApplied == L7_TRUE)
      {
        result = hapiBroadPolicyRemoveFromIface(synFlood_id , hapiPortPtr->bcmx_lport);
        if(result == L7_SUCCESS)
        {
          hapiPortPtr->synAckPolicyApplied = L7_FALSE;
          --count;
          /* If there is no port on which current policy is active delete it */
          if(count <= 0)
          {
            (void)hapiBroadPolicyDelete(synFlood_id );
            synFlood_id   = BROAD_POLICY_INVALID;
            rule_id = BROAD_POLICY_RULE_INVALID;
            count = 0;
            rate = 0;
          }
        }
      }
    }
  }

  return result;
}


/*********************************************************************
*
* @purpose Sets the priority used for sampled packets sent to CPU (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Not called by application anymore. Parameters set during
*          system initialization.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemCpuSamplePriority(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t              result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  int                  rv;

  if (dapiCmd->cmdData.sFlowConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  rv = bcmx_switch_control_set(bcmSwitchCpuSamplePrio, dapiCmd->cmdData.sFlowConfig.sampleCpuPrio);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    LOG_ERROR(bcmSwitchCpuSamplePrio);
  }

  /* Upon failure, the error is logged.
   * Return quietly so that we don't cause the device
   * to reboot */
  return result; 
}

/*********************************************************************
*
* @purpose Sets the seed for the random number generator that determines if a packet
*          should be sampled (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Not called by application anymore. Parameters set during
*          system initialization.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemSampleRandomSeed(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t              result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t    *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  int                  rv;

  if (dapiCmd->cmdData.sFlowConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (dapiCmd->cmdData.sFlowConfig.direction == 0)
  {
    rv = bcmx_switch_control_set(bcmSwitchSampleIngressRandomSeed, dapiCmd->cmdData.sFlowConfig.RandomSeed);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR(bcmSwitchSampleIngressRandomSeed);
    }
  }

  if (dapiCmd->cmdData.sFlowConfig.direction == 1)
  {
    rv = bcmx_switch_control_set(bcmSwitchSampleEgressRandomSeed, dapiCmd->cmdData.sFlowConfig.RandomSeed);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR(bcmSwitchSampleEgressRandomSeed);
    }
  }


  /* Upon failure, the error is logged.
   * Return quietly so that we don't cause the device
   * to reboot */
  return result; 
}

/*********************************************************************
*
* @purpose Sets the sampling rate of packets ingressing or egressing a port (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_SAMPLE_RATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfSampleRate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                        result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t             *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  int                            rv;
  BROAD_PORT_t                  *hapiPortPtr;
  usl_bcm_port_sflow_config_t    sflowConfig;

  memset(&sflowConfig, 0, sizeof(sflowConfig));

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (dapiCmd->cmdData.sFlowConfig.getOrSet != DAPI_CMD_SET &&
      dapiCmd->cmdData.sFlowConfig.getOrSet != DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (dapiCmd->cmdData.sFlowConfig.getOrSet == DAPI_CMD_SET)
  {
    sflowConfig.ingressSamplingRate = dapiCmd->cmdData.sFlowConfig.ingressSamplingRate;
    sflowConfig.egressSamplingRate =  dapiCmd->cmdData.sFlowConfig.egressSamplingRate;
    rv = usl_bcmx_port_sample_rate_set(hapiPortPtr->bcmx_lport, &sflowConfig);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
    }
  }
  else
  {
    /* get configured rate */
    rv = usl_bcmx_port_sample_rate_get(hapiPortPtr->bcmx_lport, &sflowConfig);
                                      
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
    }
    else
    {
      dapiCmd->cmdData.sFlowConfig.ingressSamplingRate = sflowConfig.ingressSamplingRate;
      dapiCmd->cmdData.sFlowConfig.egressSamplingRate = sflowConfig.egressSamplingRate;
    }
    
  }

  /* Upon failure, the error is logged.
   * Return quietly so that we don't cause the device
   * to reboot */
  return result; 
}
/*********************************************************************
 *
 * @purpose  Used to Diagnostic Cable on a fiber interface.
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param    cmd           @b{(input)} DAPI_CMD_INTF_CABLE_DIAG_NAME
 * @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.CableDiagStatus
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfFiberDiagTest(DAPI_USP_t *usp,
    DAPI_CMD_t cmd, void *data,
    DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_FAILURE;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_int32                 temperature;
  L7_uint32                voltage;
  L7_uint32                current;
  L7_uint32                txPower;
  L7_uint32                rxPower;
  L7_uint32                los;
  L7_uint32                txFault;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    if (dapiCmd->cmdData.cableFiberDiag.getOrSet == DAPI_CMD_GET)
    {
      if (BCM_E_NONE == usl_bcmx_port_sfp_diag_get(hapiPortPtr->bcmx_lport,
                                                   &temperature,
                                                   &voltage,
                                                   &current,
                                                   &txPower,
                                                   &rxPower,
                                                   &txFault,
                                                   &los))
      {
        result = L7_SUCCESS;
      }

      dapiCmd->cmdData.cableFiberDiag.temperature = temperature;
      dapiCmd->cmdData.cableFiberDiag.voltage     = voltage;
      dapiCmd->cmdData.cableFiberDiag.current     = current;
      dapiCmd->cmdData.cableFiberDiag.powerOut    = txPower;
      dapiCmd->cmdData.cableFiberDiag.powerIn     = rxPower;
      dapiCmd->cmdData.cableFiberDiag.txFault     = (L7_BOOL)txFault;
      dapiCmd->cmdData.cableFiberDiag.los         = (L7_BOOL)los;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
              "\n%s %d: %s(usp=%d.%d.%d, cmd=%d) - Invalid getOrSet Command = %d\n"
              __FILE__, __LINE__, __func__, usp->unit, usp->slot, usp->port, cmd, result);
    }
  }

  return result;
}


/*********************************************************************
*
* @purpose Sets the admin mode for all the ports in a card
*
* @param   unit        Unit number of the card
* @param   slot        Slot number of the card
* @param   forceMode           L7_TRUE: Apply the 'forcedAdminMode' value to
*                              all the ports in the card.
*                              L7_FALSE: Apply the application issued adminMode
*                              (stored in hapiPort structure) value.
* @param   forcedAdminMode     Valid only if 'forceMode' is L7_TRUE.
*                              L7_TRUE: Enable the ports.
*                              L7_FALSE: Disable the ports.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Uses customx port infra-structure to send the message to the 
*          appropriate unit. The port number in the customx call is
*          dummy parameter.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemCardPortsAdminModeSet(L7_uint32 unit, L7_uint32 slot, 
                                             L7_BOOL forceMode, L7_BOOL forcedAdminMode,
                                             DAPI_t *dapi_g)
{
  DAPI_USP_t                                       usp;
  BROAD_PORT_t                                    *hapiPortPtr;
  L7_uint32                                        maxElems, numElems;
  L7_uchar8                                       *msg, *msgPtr;
  L7_RC_t                                          result = L7_SUCCESS;
  usl_bcm_port_admin_mode_t         element;
  uint32                                           args[BCM_CUSTOM_ARGS_MAX];
  int                                              localBcmUnitNum, rv, dummyLport = BCMX_LPORT_INVALID;
  
  usp.unit = unit;
  usp.slot = slot; 

  /* The first 4 byte of the message contains number of elements in the message */
  numElems = 0;
  maxElems = (sizeof(args) - sizeof(L7_uint32))/sizeof(usl_bcm_port_admin_mode_t);
  if (maxElems == 0) 
  {
    LOG_ERROR(0);
  }

  /* Setup the message */
  memset(args, 0, sizeof(args));
  msg = (L7_uchar8 *) args;
  msgPtr = msg + sizeof(L7_uint32);

  for (usp.port=0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    if (isValidUsp(&usp, dapi_g) != L7_TRUE) 
    {
      continue;
    }

    /* Get the local bcm unit number for each port */
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
    rv = bcm_unit_remote_unit_get(hapiPortPtr->bcm_unit, &localBcmUnitNum);
    if (rv != BCM_E_NONE) 
    {
      continue;
    }

    /* If dummyLport is unassigned, populate it */
    if (dummyLport == BCMX_LPORT_INVALID) 
    {
      dummyLport = hapiPortPtr->bcmx_lport;
    }

    memset(&element, 0, sizeof(element));
    element.bcmUnit = localBcmUnitNum;
    element.bcmPort = hapiPortPtr->bcm_port;
    if (forceMode == L7_TRUE)
    {
      element.adminMode = forcedAdminMode;    
    }
    else
    {
    element.adminMode =  hapiPortPtr->hapiModeparm.physical.admin_enabled;
    }
 
    /* Copy this element in the message */
    memcpy(msgPtr, &element, sizeof(element));
    msgPtr += sizeof(element);
    numElems++;

    /* Time to send the message */
    if (numElems == maxElems) 
    {
      *(L7_uint32 *)&msg[0] = numElems;
      /* PTin modified: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = bcmx_custom_port_set(dummyLport, USL_BCMX_PORT_ADMIN_MODE_SET, (sizeof(L7_uint32)+sizeof(element)*numElems)/sizeof(L7_uint32), args);
      #else
      rv = bcmx_custom_port_set(dummyLport, USL_BCMX_PORT_ADMIN_MODE_SET, args);
      #endif
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                      "Failed to set admin mode for ports in unit/slot %d/%d rv %d\n", usp.unit, usp.slot, rv);
      }

      numElems = 0;
      memset(args, 0, sizeof(args));
      msg = (L7_uchar8 *) args;
      msgPtr = msg + sizeof(L7_uint32);
    }
  }

  /* Send the remaining elements */
  if (numElems > 0) 
  {
    *(L7_uint32 *)&msg[0] = numElems;
    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    rv = bcmx_custom_port_set(dummyLport, USL_BCMX_PORT_ADMIN_MODE_SET, (sizeof(L7_uint32)+sizeof(element)*numElems)/sizeof(L7_uint32), args);
    #else
    rv = bcmx_custom_port_set(dummyLport, USL_BCMX_PORT_ADMIN_MODE_SET, args);
    #endif
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Failed to set admin mode for ports in unit/slot %d/%d rv %d\n", usp.unit, usp.slot, rv);
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Enables/disables the Blinking of port LEDs
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_BLINK_SET
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.ledConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLedBlinkSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t              result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t   *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  L7_BOOL blink;
#if L7_FEAT_CUSTOM_LED_BLINK
  int                  rv = BCM_E_NONE;
#endif

  blink = dapiCmd->cmdData.ledConfig.blink;

  if (dapiCmd->cmdData.ledConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "\n%s %d: In %s invalid getOrSet Command\n",
                  __FILE__, __LINE__, __FUNCTION__);
    return result;
  }
  if (cnfgrIsFeaturePresent(L7_DTL_COMPONENT_ID, L7_DTL_CUSTOM_LED_BLINK_FEATURE_ID))
  {
#if L7_FEAT_CUSTOM_LED_BLINK
    rv = HAPI_BROAD_LED_BLINK_ENABLE_DISABLE(0, blink);
    if (rv != BCM_E_NONE)
    {
      result = L7_ERROR;
      SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "\n%s %d: bcm_robo_port_led_blink_set failed, rv %d\n",
                  __FILE__, __LINE__, rv);
    }
#endif
  }
  return result;
}

/*********************************************************************
*
* @purpose Generate the link-up event for all the ports in a card
*
* @param   unit        Unit number of the card
* @param   slot        Slot number of the card
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemCardPortLinkupGenerate(L7_uint32 unit, L7_uint32 slot, 
                                              DAPI_t *dapi_g)
{
  DAPI_USP_t           usp;
  DAPI_PORT_SPEED_t    speed; 
  DAPI_PORT_DUPLEX_t   duplex;
  DAPI_PORT_t         *dapiPortPtr;
  BROAD_PORT_t        *hapiPortPtr;
  L7_BOOL              isLinkUp, isSfpLink;
  L7_BOOL              isTxPauseAgreed, isRxPauseAgreed;
  bcm_port_info_t      portInfo;
  

  memset(&portInfo, 0, sizeof(bcm_port_info_t));
  usp.unit = unit;
  usp.slot = slot; 

  for (usp.port=0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    if (isValidUsp(&usp, dapi_g) != L7_TRUE) 
    {
      continue;
    }

    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

    if (hapiBroadPhyModeGet(&usp, &speed, &duplex, &isLinkUp, &isSfpLink, &isTxPauseAgreed, &isRxPauseAgreed, dapi_g) == L7_SUCCESS)
    {
      if ((isLinkUp == L7_TRUE) &&
          (dapiPortPtr->modeparm.physical.isLinkUp == L7_FALSE))
      {
        portInfo.linkstatus = L7_TRUE;
        hapiBroadPortLinkStatusChange(hapiPortPtr->bcmx_lport, &portInfo);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Shutdown all the front-panel ports for a unit.
*
* @param    unitNumber {(input)} Unit for which the ports are to be shut.
*                                L7_ALL_UNITS indicates all the valid
*                                stack members.
*
*
* @returns  L7_RC_t
*
* @comments Called by UM before move-management
*
* @end
*********************************************************************/
L7_RC_t hpcHardwareShutdownPorts(L7_uint32 unitNumber)
{
  L7_uint32        unitIdx, slotIdx;
  unitMgrStatus_t  unitStatus;
  L7_RC_t          rc, rcFinal = L7_SUCCESS;

  if (unitNumber == L7_ALL_UNITS)
  {
    /* Loop through all the stack members */
    rc = unitMgrStackMemberGetFirst(&unitIdx);

    while (rc != L7_FAILURE)
    {
      rc = unitMgrUnitStatusGet (unitIdx, &unitStatus);
      if (unitStatus == L7_UNITMGR_UNIT_OK)
      {
        for (slotIdx = 0; slotIdx < L7_MAX_SLOTS_PER_UNIT; slotIdx++) 
        {
          /* Check if this is a line card */
          if ((dapi_g->unit[unitIdx]->slot[slotIdx]->cardPresent == L7_TRUE) &&
              (dapi_g->unit[unitIdx]->slot[slotIdx]->cardType == 
                                                       SYSAPI_CARD_TYPE_LINE))
          {
            rc = hapiBroadSystemCardPortsAdminModeSet(unitIdx, slotIdx,
                                                      L7_TRUE, L7_FALSE,
                                                      dapi_g);
            if (rc != L7_SUCCESS)
            {
              rcFinal = rc;    
            }
          }  
        }
      }
      
      rc = unitMgrStackMemberGetNext(unitIdx ,&unitIdx);
    }
  }
  else
  {
    unitIdx = unitNumber;

    for (slotIdx = 0; slotIdx < L7_MAX_SLOTS_PER_UNIT; slotIdx++) 
    {
      /* Check if this is a line card */
      if ((dapi_g->unit[unitIdx]->slot[slotIdx]->cardPresent == L7_TRUE) &&
          (dapi_g->unit[unitIdx]->slot[slotIdx]->cardType == 
                                                  SYSAPI_CARD_TYPE_LINE))
      {
        rc = hapiBroadSystemCardPortsAdminModeSet(unitIdx, slotIdx,
                                                  L7_TRUE, L7_FALSE,
                                                  dapi_g);
        if (rc != L7_SUCCESS)
        {
          rcFinal = rc;    
        }
      }  
    }
  }
  
  return rcFinal;
}

/*********************************************************************
*
* @purpose Get the Maximum number of Priority Groups allowed to be paused
*
* @param    none
*
* @returns  The number of lossless priority groups allowed
* 
**********************************************************************/
static int hapiBroadIntfPfcPgMaxGet(void)
{
  if (pfc_max_pg > platPfcMaxPgPerIntfGet()){
    return pfc_max_pg;
  }
  else{
    return platPfcMaxPgPerIntfGet();
  }
}

/*********************************************************************
*
* @purpose Set the Maximum number of Priority Groups allowed to be paused
*
* @param  max_pg    The maximum number of lossless priority groups
*
* @returns none
* 
**********************************************************************/
void hapiBroadIntfPfcPgMaxSet(int max_pg)
{
  if (max_pg > (L7_DOT1P_MAX_PRIORITY + 1)){
     max_pg = (L7_DOT1P_MAX_PRIORITY + 1);
  }

  pfc_max_pg = max_pg;
}

/*********************************************************************
*
* @purpose Configuration of Priority Based Flow Control
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_PFC_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.pfcConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_NOT_SUPPORTED if the port or hardware does not support feature
*          L7_SUCCESS or L7_FAILURE otherwise
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t *intfCmd = (DAPI_INTF_MGMT_CMD_t *)data; 
  BROAD_PORT_t    *hapiPortPtr = L7_NULLPTR;
  L7_uint32               idx = 0;
  int rv = BCM_E_NONE;
  int ppp_cnt = 0;
  int mode;
  usl_bcm_port_pfc_config_t pfcConfig;
  usl_bcm_port_pfc_config_t currPfcConfig;


  mode = hapiBroadDropModeCheck(0);

  do {
    if (intfCmd == L7_NULLPTR || usp == L7_NULLPTR || dapi_g == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      break;
    }

    pfcConfig.mode                  = intfCmd->cmdData.pfcConfig.enable;
    pfcConfig.no_drop_priority_bmp  = intfCmd->cmdData.pfcConfig.priority_bmp;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    if (hapiPortPtr == L7_NULLPTR) break;

    rc = usl_portdb_pfc_config_get(USL_CURRENT_DB, 
                                   hapiPortPtr->bcmx_lport, 
                                   &currPfcConfig);

    if (rc != BCM_E_NONE) 
    {
      rc = BCM_E_FAIL;
      break;
    }

    if ((pfcConfig.mode == currPfcConfig.mode) &&
        (pfcConfig.no_drop_priority_bmp == currPfcConfig.no_drop_priority_bmp))
    {
      break;
    }

    /* Make sure that the number of no-drop priorities does not exceed the 
     * capabilities of the chip.  The issue is related to the amount of hdrm
     * required to guarantee lossless behavior 
     */
    for (idx=0;idx < L7_DOT1P_MAX_PRIORITY+1;idx++)
    {
      if (pfcConfig.no_drop_priority_bmp & (1 << idx)) ppp_cnt++;
    }

    if (ppp_cnt > hapiBroadIntfPfcPgMaxGet()){
      rv = BCM_E_RESOURCE;
      break;
    }

    rv = usl_bcmx_port_pfc_config_set(hapiPortPtr->bcmx_lport, pfcConfig); 

    if (L7_BCMX_OK(rv) != L7_TRUE) break;

    /* If disabling PFC, see if congestion control can switch to egress.  
     * Egress based congestion control is only valid if there are no ports
     * participating in either PFC or Flow control
     * If any interfaces are enabled for pause (link or priority) the system 
     * the mmu will have to flip the congestion control to ingress
     */
    if (pfcConfig.mode)
    {
      if (pfcConfig.mode != currPfcConfig.mode)
      {
        mode = hapiBroadDropModeCheck(PFC_ENABLED);
      }
    }
    else
    {
      if (pfcConfig.mode != currPfcConfig.mode)
      {
        mode = hapiBroadDropModeCheck(PFC_DISABLED);
      }
    }
  
    rv = usl_bcmx_mmu_dropmode_set(mode);
      
  }while (0);

  /* Application must check explicity for L7_NOT_SUPPORTED */
  if (rv == BCM_E_UNAVAIL) rc =  L7_NOT_SUPPORTED;
  else if (rv == BCM_E_RESOURCE) rc = L7_TABLE_IS_FULL;
  else if (L7_BCMX_OK(rv) != L7_TRUE) rc = L7_FAILURE;
  else rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose Get a PFC statistic for an interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_PFC_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.pfcConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcStatGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  int rv = BCM_E_NONE;
  DAPI_INTF_MGMT_CMD_t *intfCmd = (DAPI_INTF_MGMT_CMD_t *)data; 
  BROAD_PORT_t    *hapiPortPtr = L7_NULLPTR;
  usl_bcm_port_pfc_stat_t stat;
  stat.stat_id = 0;
  stat.ctr = 0;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  if (hapiPortPtr == L7_NULLPTR) return L7_FAILURE;;

  if (intfCmd->cmdData.pfcStatGet.statType >= DAPI_STATS_PFC_ENTRIES)
  {
      rv = BCM_E_FAIL;
      rc = L7_FAILURE;
  }
  else
  {
    stat.stat_id = intfCmd->cmdData.pfcStatGet.statType;
  }

  if (rv == BCM_E_NONE && rc == L7_SUCCESS)
    rv = usl_bcmx_port_pfc_stat_get(hapiPortPtr->bcmx_lport,&stat);

  if (rv == BCM_E_UNAVAIL) rc =  L7_NOT_SUPPORTED;
  else if (L7_BCMX_OK(rv) != L7_TRUE) rc = L7_FAILURE;
  else rc = L7_SUCCESS;

  intfCmd->cmdData.pfcStatGet.statCounter = stat.ctr;

  return rc;
}

/*********************************************************************
*
* @purpose Clear the PFC stats for an interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_PFC_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.pfcConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcStatsClear(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  int rv = BCM_E_NONE;
  BROAD_PORT_t    *hapiPortPtr = L7_NULLPTR;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  if (hapiPortPtr == L7_NULLPTR) return L7_FAILURE;;

  rv = usl_bcmx_port_pfc_stats_clear(hapiPortPtr->bcmx_lport);

  if (rv == BCM_E_UNAVAIL) rc =  L7_NOT_SUPPORTED;
  else if (L7_BCMX_OK(rv) != L7_TRUE) rc = L7_FAILURE;
  else rc = L7_SUCCESS;

  return rc;
}

L7_RC_t hapiBroadIntfPfcConfigTest(int unit, int slot, int port, int enable, int bitmap)
{
  DAPI_USP_t usp;
  usp.unit = unit; usp.slot = slot; usp.port = port;
  DAPI_INTF_MGMT_CMD_t  cmd;

  cmd.cmdData.pfcConfig.enable = enable;
  cmd.cmdData.pfcConfig.priority_bmp = bitmap;

  return dapiCtl(&usp,DAPI_CMD_INTF_PFC_CONFIG,&cmd);
}


/*********************************************************************
 *
 * @purpose To Enable/Disable ISDP on a  port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_ISDP_INTF_STATUS_SET
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIsdpStatusSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 rc = L7_FAILURE;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_SYSTEM_t          *hapiSystem;

  if (dapiCmd->cmdData.isdpStatus.getOrSet != DAPI_CMD_SET)
  {
    return rc;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  rc = hapiBroadIsdpPolicySet(usp, 
                              LLPF_PORT_BLOCK_PROTO_ISSET(hapiPortPtr, L7_LLPF_BLOCK_TYPE_ISDP), 
                              LLPF_PORT_BLOCK_PROTO_ISSET(hapiPortPtr, L7_LLPF_BLOCK_TYPE_ISDP), 
                              hapiPortPtr->isdpEnable, 
                              dapiCmd->cmdData.isdpStatus.enable, 
                              dapi_g);
  if (rc == L7_SUCCESS)
  {
    hapiPortPtr->isdpEnable = dapiCmd->cmdData.isdpStatus.enable;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Sets the ISDP policy for the port, given the ISDP
*          config and the LLPF config
*
* @returns 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIsdpPortUpdate(DAPI_USP_t *usp, 
                                L7_BOOL     oldLlpfEnabled,
                                L7_BOOL     newLlpfEnabled,
                                L7_BOOL     oldIsdpEnabled,
                                L7_BOOL     newIsdpEnabled,
                                DAPI_t     *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  bcmx_lport_t    lport;
  L7_ushort16     temp16;
  BROAD_SYSTEM_t *hapiSystem;
  BROAD_POLICY_t  llpfPolicyId, isdpPolicyId;

  hapiSystem  = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  lport = hapiPortPtr->bcmx_lport;

  isdpPolicyId = hapiSystem->isdpSysId;
#ifdef L7_LLPF_PACKAGE
  llpfPolicyId = hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP];
#else
  llpfPolicyId = BROAD_POLICY_INVALID;
#endif

  temp16 =  oldLlpfEnabled ? (1 << 12) : 0;
  temp16 |= newLlpfEnabled ? (1 << 8)  : 0;
  temp16 |= oldIsdpEnabled ? (1 << 4)  : 0;
  temp16 |= newIsdpEnabled ? (1 << 0)  : 0;

  switch (temp16)
  {
  case 0x0000:
  case 0x0011:
  case 0x0111:
  case 0x1011:
  case 0x1100:
  case 0x1111:
    /* No change */
    break;

  case 0x0001:
  case 0x0101:
    /* Add this port to the ISDP policy. */
    if (isdpPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyApplyToIface(isdpPolicyId, lport);
    }
    break;

  case 0x0010:
  case 0x1010:
    /* Remove this port from the ISDP policy. */
    if (isdpPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyRemoveFromIface(isdpPolicyId, lport);
    }
    break;

  case 0x0100:
    /* Add this port to the LLPF policy. */
    if (llpfPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyApplyToIface(llpfPolicyId, lport);
    }
    break;

  case 0x1000:
    /* Remove this port from the LLPF policy. */
    if (llpfPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyRemoveFromIface(llpfPolicyId, lport);
    }
    break;

  case 0x0110:
  case 0x1110:
    /* Remove this port from the ISDP policy. */
    if (isdpPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyRemoveFromIface(isdpPolicyId, lport);
    }
    if (rc == L7_SUCCESS)
    {
      /* Add this port to the LLPF policy. */
      if (llpfPolicyId != BROAD_POLICY_INVALID)
      {
        rc = hapiBroadPolicyApplyToIface(llpfPolicyId, lport);
      }
    }
    break;

  case 0x1001:
  case 0x1101:
    /* Remove this port from the LLPF policy. */
    if (llpfPolicyId != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadPolicyRemoveFromIface(llpfPolicyId, lport);
    }
    if (rc == L7_SUCCESS)
    {
      /* Add this port to the ISDP policy. */
      if (isdpPolicyId != BROAD_POLICY_INVALID)
      {
        rc = hapiBroadPolicyApplyToIface(isdpPolicyId, lport);
      }
    }
    break;

  default:
    break;
  }
 
  return rc;
}

/*********************************************************************
*
* @purpose Sets the ISDP policy for the port, given the ISDP
*          config and the LLPF config
*
* @returns 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIsdpPolicySet(DAPI_USP_t *usp, 
                               L7_BOOL     oldLlpfEnabled,
                               L7_BOOL     newLlpfEnabled,
                               L7_BOOL     oldIsdpEnabled,
                               L7_BOOL     newIsdpEnabled,
                               DAPI_t     *dapi_g)
{
  L7_int32                 rc = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                i;


  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* assumes this function is only called w/ physical and LAG ports */
  if ( IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) )
  {
    if ( hapiPortPtr->hapiModeparm.physical.isMemberOfLag != L7_TRUE )
    {
      rc = hapiBroadIsdpPortUpdate(usp,
                                   oldLlpfEnabled,
                                   newLlpfEnabled,
                                   oldIsdpEnabled,
                                   newIsdpEnabled,
                                   dapi_g);
    }
  }
  else if ( IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) )
  {
    hapiBroadLagCritSecEnter();

    for ( i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++ )
    {
      if ( L7_TRUE == dapiPortPtr->modeparm.lag.memberSet[i].inUse )
      {
        rc = hapiBroadIsdpPortUpdate(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                     oldLlpfEnabled,
                                     newLlpfEnabled,
                                     oldIsdpEnabled,
                                     newIsdpEnabled,
                                     dapi_g);
      }
    }
    hapiBroadLagCritSecExit();
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}
