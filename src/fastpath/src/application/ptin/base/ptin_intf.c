#include <unistd.h>
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_isdp_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_dvlantag_api.h"
#include "dot3ad_api.h"
#include "dot3ad_exports.h"
#include "nimapi.h"
#include "dot1s_api.h"
#include "usmdb_qos_cos_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_dai_api.h"

#include "dtlapi.h"
#include "ptin_include.h"
#include "ptin_control.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_xlate_api.h"
#include "ptin_xconnect_api.h"
#include "ptin_cnfgr.h"
#include "ptin_dhcp.h"
#include "ptin_pppoe.h"
#include "fw_shm.h"
#include "ptin_igmp.h" //Added for Admission Control Support
#include "ptin_fieldproc.h"
#include "ptin_fpga_api.h"

#define LINKSCAN_MANAGEABLE_BOARD (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)

/* Uplink protection */
#if (PTIN_BOARD_IS_MATRIX)
static L7_uint64 forcelinked_ports_bmp        = 0;
#endif

#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
static L7_uint64 uplink_protection_ports_bmp  = 0;
static L7_uint64 uplink_protection_ports_active_bmp = 0;
static L7_uint64 lag_uplink_protection_ports_bmp[PTIN_SYSTEM_N_LAGS - PTIN_SYSTEM_PROTECTION_LAGID_BASE +1];
#endif

L7_BOOL linkscan_update_control = L7_TRUE;
void ptin_linkscan_control_global(L7_BOOL enable)
{
#ifdef PTIN_LINKSCAN_CONTROL
  L7_uint port;

  printf("Applying linkscan state to all ports...\r\n");

  if (!enable)  linkscan_update_control = L7_FALSE;

  /* Apply configuration to all ports */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    if (ptin_intf_linkscan_control(port, enable) != L7_SUCCESS)
    {
      printf("Error applying linkscan state to port %u\r\n", port);
    }
  }

  if (enable)  linkscan_update_control = L7_TRUE;

  printf("Linkscan management state changed to %u\r\n", enable);
#else
  printf("Linkscan management is not active for this board\r\n");
#endif
}

#define MAP_EMTPY_ENTRY     0xFFFFFFFF  /* 32bits unsigned */


/**
 * Data Structures
 */
/* Data structure with the physical ports configuration */
/* IMPORTANT: The functions that manipulate this structure are serialized
 * (always called from ptin message handler). This avoids the use mutexes. */
static ptin_HWEthPhyConf_t phyConf_data[PTIN_SYSTEM_N_PORTS];
static ptin_HWPortExt_t     phyExt_data[PTIN_SYSTEM_N_PORTS];

/* Data structure with LAGs configuration
 * IMPORTANT:
 *  - lagId = MAP_EMTPY_ENTRY means LAG entry is empty */
static ptin_LACPLagConfig_t lagConf_data[PTIN_SYSTEM_N_LAGS];


/* IMPORTANT NOTE:
 * PTin ports mapping:
 *  [0..PTIN_SYSTEM_N_PORTS[                  - Physical PON+Eth ports
 *  [PTIN_SYSTEM_N_PORTS..PTIN_SYSTEM_N_LAGS[ - Logical LAG interfaces
 */

/* Map: ptin_port => intIfNum */
static L7_uint32 map_port2intIfNum[PTIN_SYSTEM_N_INTERF];

/* Map: intIfNum => ptin_port */
static L7_uint32 map_intIfNum2port[L7_MAX_INTERFACE_COUNT];


/*************** NGPON2 ***************/ 

/* MACROS NGPON 2*/

#define NGPON2_EMPTY_ENTRY     0xff
 
 
#define NGPON2_PORT_ADD(var, n)  ( var |= (0x1 << n))
#define NGPON2_PORT_REM(var, n)  ( var &= ~(0x1 << n))
#define NGPON2_BIT_PORT(var)     ( var & 0x1 )
                               

/*Data structure with groups information*/
static ptin_NGPON2_groups_t NGPON2_groups_info[PTIN_SYSTEM_MAX_NGPON2_GROUPS];

/**************************************/

#if (PTIN_BOARD_IS_MATRIX)
#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
static L7_uint16 ptin_slot_boardid[PTIN_SYS_SLOTS_MAX+1] = {PTIN_BOARD_TYPE_CXO160G};
#else
static L7_uint16 ptin_slot_boardid[PTIN_SYS_SLOTS_MAX+1];
#endif
#endif

/**
 * MACROS
 */

/* Updates both port/intIfNum maps */
#define UPDATE_PORT_MAP(port, intIfNum) { \
  map_port2intIfNum[port]     = intIfNum; \
  map_intIfNum2port[intIfNum] = port;     \
}

#define UPDATE_LAG_MAP(lag_idx, intIfNum) {                 \
  UPDATE_PORT_MAP(PTIN_SYSTEM_N_PORTS + lag_idx, intIfNum)  \
}

#define CLEAR_LAG_MAP(lag_idx)  { \
  map_intIfNum2port[map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx]] = MAP_EMTPY_ENTRY; \
  map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx] = MAP_EMTPY_ENTRY; \
}

#define CLEAR_LAG_CONF(lag_idx) { \
  memset(&lagConf_data[lag_idx], 0xFF, sizeof(lagConf_data[0]));  \
}

/**
 * Local Functions Prototypes
 */
static L7_RC_t ptin_intf_PhyConfig_read(ptin_HWEthPhyConf_t *phyConf);
//static L7_RC_t ptin_intf_LagConfig_read(ptin_LACPLagConfig_t *lagInfo);
static L7_RC_t ptin_intf_QoS_init(ptin_intf_t *ptin_intf);

/**
 * Initializes the ptin_intf module (structures) and several interfaces 
 * related configurations.
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_pre_init(void)
{
  L7_uint   i;
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;

  /* Reset structures (everything is set to 0xFF) */
  memset(map_port2intIfNum,   0xFF, sizeof(map_port2intIfNum));
  memset(map_intIfNum2port,   0xFF, sizeof(map_intIfNum2port));
  memset(lagConf_data,        0xFF, sizeof(lagConf_data));
  memset(phyExt_data,         0x00, sizeof(phyExt_data));
  memset(NGPON2_groups_info,  0x00, sizeof(NGPON2_groups_info));



  /* Initialize phy lookup tables */
  PT_LOG_TRACE(LOG_CTX_INTF, "Port <=> intIfNum lookup tables init:");
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    /* Get interface ID */
    if (usmDbIntIfNumFromUSPGet(1, 0, i+1, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to get interface of port# %u", i);
      return L7_FAILURE;
    }

    UPDATE_PORT_MAP(i, intIfNum);

    PT_LOG_TRACE(LOG_CTX_INTF, " Port# %02u => intIfNum# %02u", i, intIfNum);
  }

  PT_LOG_INFO(LOG_CTX_INTF, "Waiting for interfaces to be attached...");
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    while (nimGetIntfState(map_port2intIfNum[i])!=L7_INTF_ATTACHED)
    {
      osapiSleep(1);
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "Port %u attached!",i);
  }
  PT_LOG_INFO(LOG_CTX_INTF, "All interfaces attached!");

  /* LAG Lookup tables are not initialized because there are no LAGs created yet
   * L7_FEAT_LAG_PRECREATE must be cleared! (checked on ptin_globaldefs.h) */

  /* Initialize phy default TPID and MTU */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    rc = usmDbVlanMemberSet(1, 1, map_port2intIfNum[i], L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to remove port# %u from vlan 1", i);
      return L7_FAILURE;
    }
  }

  /* Wait until all requests are attended */
  do
    osapiSleepMSec(100);
  while (!dot1qQueueIsEmpty());

  return L7_SUCCESS;
}

/**
 * Initializes the ptin_intf module (structures) and several interfaces 
 * related configurations.
 *  
 * NOTE: This function must be invoked ONLY after fastpath initialization. 
 * During Init phase1/2/3 stages, the interfaces MAY NOT be initialized! 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_post_init(void)
{
  L7_uint   i;
  L7_RC_t   rc = L7_SUCCESS;
  ptin_intf_t ptin_intf;
  L7_uint32 mtu_size;

  /* Initialize phy default TPID and MTU */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf.intf_id   = i;

    phyExt_data[i].doubletag  = L7_TRUE;
    phyExt_data[i].outer_tpid = PTIN_TPID_OUTER_DEFAULT;
//    phyExt_data[i].inner_tpid = PTIN_TPID_INNER_DEFAULT;

    /* Disable front ports */
    if ((PTIN_SYSTEM_ETH_PORTS_MASK >> i) & 1)
    {
      rc = usmDbIfAdminStateSet(1, map_port2intIfNum[i], L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Failed to disable port# %u", i);
        return L7_FAILURE;
      }
      PT_LOG_INFO(LOG_CTX_INTF, "Port# %u (intIfNum %u) disabled", i, map_port2intIfNum[i]);
    }
    #if (PTIN_BOARD_IS_STANDALONE)
    else
    {
      if ((PTIN_SYSTEM_PON_PORTS_MASK >> i) & 1)
      {
        rc = usmDbIfAdminStateSet(1, map_port2intIfNum[i], L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Failed to enable port# %u", i);
          return L7_FAILURE;
        }
        PT_LOG_INFO(LOG_CTX_INTF, "Port# %u (intIfNum %u) enabled", i, map_port2intIfNum[i]);
      }
      else
      {
        if ((PTIN_SYSTEM_BL_INBAND_PORT_MASK >> i) & 1)
        {
          rc = usmDbIfAdminStateSet(1, map_port2intIfNum[i], L7_ENABLE);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Failed to enable port# %u", i);
            return L7_FAILURE;
          }
          PT_LOG_INFO(LOG_CTX_INTF, "Port# %u (intIfNum %u) enabled", i, map_port2intIfNum[i]);
        }
      }
    }
    #endif

    /* For internal ports (linecards only) */
  #if (PTIN_BOARD_IS_LINECARD)
    /* Internal interfaces of linecards, should always be trusted */
    if ((PTIN_SYSTEM_10G_PORTS_MASK >> i) & 1)
    {
      rc = usmDbDaiIntfTrustSet(map_port2intIfNum[i], L7_TRUE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to set DAI-trust mode for port# %u", i);
        return L7_FAILURE;
      }
    }
  #endif

    rc = usmDbDvlantagIntfModeSet(1, map_port2intIfNum[i], L7_ENABLE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to enable DVLAN mode on port# %u", i);
      return L7_FAILURE;
    }

    rc = usmDbDvlantagIntfEthertypeSet(1, map_port2intIfNum[i], PTIN_TPID_OUTER_DEFAULT, L7_TRUE);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to configure default TPID 0x%04X on port# %u (rc = %d)", PTIN_TPID_OUTER_DEFAULT, i, rc);
      return L7_FAILURE;
    }

    mtu_size = ((PTIN_SYSTEM_PON_PORTS_MASK >> i) & 1) ? PTIN_SYSTEM_PON_MTU_SIZE : PTIN_SYSTEM_ETH_MTU_SIZE;

    rc = usmDbIfConfigMaxFrameSizeSet(map_port2intIfNum[i], mtu_size);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to set max frame on port# %u", i);
      return L7_FAILURE;
    }

    #if 0
    /* Configure Oversized frame size */
    rc = ptin_intf_oversize_frame_set(map_port2intIfNum[i], 1518);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to set ovsersized frame size on port# %u", i);
      return L7_FAILURE;
    }
    #endif

    /* QoS initialization */
    if (ptin_intf_QoS_init(&ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Phy# %u: Error initializing QoS definitions",i);
      return L7_FAILURE;
    }

//#if (PTIN_BOARD == PTIN_BOARD_TOLT8G)
//    /* For TOLT8G, configure MAC learn priority with higher value on uplink interfaces */
//    if (PTIN_IS_PORT_PON(i))
//    {
//      rc = dtlPtinL2LearnPortSet(map_port2intIfNum[i], PTIN_SYSTEM_PON_PRIO);
//      if (rc != L7_SUCCESS)
//      {
//        PT_LOG_ERR(LOG_CTX_INTF, "Failed to set Mac Learn priority on port# %u", i);
//        return L7_FAILURE;
//      }
//    }
//#endif
  }

  /* MEF Ext defaults */
  if (ptin_intf_portExt_init()!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed initializing MEF Ext parameters");
    return L7_FAILURE;
  }
  PT_LOG_NOTICE(LOG_CTX_INTF, "MEF Ext defaults applied");

  /* Initialize phy conf structure (must be run after default configurations!) */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    phyConf_data[i].Port = i;
    if (ptin_intf_PhyConfig_read(&phyConf_data[i]) != L7_SUCCESS) {
      return L7_FAILURE;
    }
  }

#if (PTIN_BOARD_IS_MATRIX)
  /* Clear structures */
  forcelinked_ports_bmp = 0;
#endif
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  /* Clear structures */
  uplink_protection_ports_bmp = 0;
  uplink_protection_ports_active_bmp = 0;
  memset(lag_uplink_protection_ports_bmp, 0x00, sizeof(lag_uplink_protection_ports_bmp));
#endif

  return L7_SUCCESS;
}


/* For internal ports (linecards only) */
#if (PTIN_BOARD_IS_LINECARD)
/**
 * Restore DAI Setting for Internal Interfaces
 *  
 * @param none
 *  
 * @return none
 */
void ptin_intf_dai_restore_defaults(void)
{
  L7_RC_t rc;
  L7_uint32 i;
  for (i=0; i<ptin_sys_number_of_ports; i++)
  { 
    /* Internal interfaces of linecards, should always be trusted */
    if ((PTIN_SYSTEM_10G_PORTS_MASK >> i) & 1)
    {
      rc = usmDbDaiIntfTrustSet(map_port2intIfNum[i], L7_TRUE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Failed to set DAI-trust mode for port# %u", i);      
      }
    }  
  }
  return;
}
#endif

/****************************************************************************** 
 * PHY PORT FUNCTIONS
 ******************************************************************************/

/**
 * Init Port exitension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_portExt_init(void)
{
  L7_int          port;
  ptin_intf_t     ptin_intf;
  ptin_HWPortExt_t mefExt;
  L7_RC_t         rc = L7_SUCCESS;

  /* Default values */
  mefExt.Mask = PTIN_HWPORTEXT_MASK_DEFVID                         |
                PTIN_HWPORTEXT_MASK_DEFPRIO                        |
                 PTIN_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES        |
                 PTIN_HWPORTEXT_MASK_INGRESS_FILTER                |
                 /*PTIN_HWPORTEXT_MASK_RESTRICTED_VLAN_REG           |*/
                 /*PTIN_HWPORTEXT_MASK_VLAN_AWARE                    |*/
                 /*PTIN_HWPORTEXT_MASK_TYPE                          |*/
                 PTIN_HWPORTEXT_MASK_DOUBLETAG                     |
                 PTIN_HWPORTEXT_MASK_OUTER_TPID                    |
                 PTIN_HWPORTEXT_MASK_INNER_TPID                    |
                 PTIN_HWPORTEXT_MASK_EGRESS_TYPE                   |
                 PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE               |
                 PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE   |
                 PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO     |
                 PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO |
                 /*PTIN_HWPORTEXT_MASK_MAXCHANNELS_INTF              |*/
                 /*PTIN_HWPORTEXT_MASK_MAXBANDWIDTH_INTF             |*/
                 PTIN_HWPORTEXT_MASK_DHCP_TRUSTED;  
  mefExt.defVid                       = 1;
  mefExt.defPrio                      = 0;
  mefExt.acceptable_frame_types       = L7_DOT1Q_ADMIT_ALL;
  mefExt.ingress_filter               = L7_FALSE;
  mefExt.restricted_vlan_reg          = 0;            /* Not defined */
  mefExt.vlan_aware                   = L7_FALSE;     /* Not defined */
  mefExt.type                         = 0;            /* Not defined */
  mefExt.doubletag                    = L7_TRUE;
  mefExt.outer_tpid                   = 0x8100;
  mefExt.inner_tpid                   = 0x8100;
  mefExt.egress_type                  = PTIN_PORT_EGRESS_TYPE_ISOLATED;
  mefExt.macLearn_enable              = L7_TRUE;
  mefExt.macLearn_stationMove_enable  = L7_TRUE;
  mefExt.macLearn_stationMove_prio    = 0;
  mefExt.macLearn_stationMove_samePrio= L7_TRUE;
  mefExt.maxChannels                  = 0;            /* Not defined */
  mefExt.maxBandwidth                 = 0;            /* Not defined */
  mefExt.dhcp_trusted                 = L7_FALSE;   /* By default a port is untrusted */

  /* Only apply to physical interfaces */
  ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;

  /* Run all physical interfaces */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    ptin_intf.intf_id = port;

    /* L2 Station move priority defaults */
  #if ( PTIN_BOARD == PTIN_BOARD_CXO160G )
    if (port < PTIN_SYSTEM_N_LOCAL_PORTS)
    {
      mefExt.macLearn_stationMove_prio = 2;
      mefExt.egress_type = PTIN_PORT_EGRESS_TYPE_PROMISCUOUS;
    }
    else
    {
      mefExt.macLearn_stationMove_prio = 0;
      mefExt.egress_type = PTIN_PORT_EGRESS_TYPE_ISOLATED;
    }
  #elif ( !PTIN_BOARD_IS_MATRIX )
    if (port < PTIN_SYSTEM_N_PONS || port < PTIN_SYSTEM_N_ETH)
    {
      mefExt.macLearn_stationMove_prio = 0;
      mefExt.egress_type = PTIN_PORT_EGRESS_TYPE_ISOLATED;
    }
    else
    {
      mefExt.macLearn_stationMove_prio = 2;
      mefExt.egress_type = PTIN_PORT_EGRESS_TYPE_PROMISCUOUS;
    }
  #endif

    /* Only for linecards at slot systems */
    #if ( PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
    /* If is an internal/backplane port, set as trusted */
    if (!((PTIN_SYSTEM_PON_PORTS_MASK >> port) & 1) && !((PTIN_SYSTEM_ETH_PORTS_MASK >> port) & 1))
    {
      mefExt.dhcp_trusted = L7_TRUE;
    }
    #endif

    /* Apply MEF EXT defaults */
    if (ptin_intf_portExt_set(&ptin_intf, &mefExt)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed initializing MEF Ext parameters for interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Set Port exitension definitions
 * 
 * @param ptin_intf : Interface
 *        mefExt    : MEF Extension parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_portExt_set(const ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt)
{
  L7_uint32 intIfNum;
  L7_uint32 unit = 0;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || mefExt==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"MefExt parameters:");
  PT_LOG_TRACE(LOG_CTX_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  PT_LOG_TRACE(LOG_CTX_INTF," Mask = 0x%08x"                    , mefExt->Mask);
  PT_LOG_TRACE(LOG_CTX_INTF," defVid = %u"                      , mefExt->defVid);
  PT_LOG_TRACE(LOG_CTX_INTF," defPrio = %u"                     , mefExt->defPrio);
  PT_LOG_TRACE(LOG_CTX_INTF," acceptable_frame_types = %u"      , mefExt->acceptable_frame_types);
  PT_LOG_TRACE(LOG_CTX_INTF," ingress_filter = %u"              , mefExt->ingress_filter);
  PT_LOG_TRACE(LOG_CTX_INTF," restricted_vlan_reg = %u"         , mefExt->restricted_vlan_reg);
  PT_LOG_TRACE(LOG_CTX_INTF," vlan_aware = %u"                  , mefExt->vlan_aware);
  PT_LOG_TRACE(LOG_CTX_INTF," type       = %u"                  , mefExt->type);
  PT_LOG_TRACE(LOG_CTX_INTF," doubletag  = %u"                  , mefExt->doubletag);
  PT_LOG_TRACE(LOG_CTX_INTF," outer_tpid = 0x%04X"              , mefExt->outer_tpid);
  PT_LOG_TRACE(LOG_CTX_INTF," inner_tpid = 0x%04X"              , mefExt->inner_tpid);
  PT_LOG_TRACE(LOG_CTX_INTF," egress_type = %u"                 , mefExt->egress_type);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_enable = %u"             , mefExt->macLearn_enable);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_stationMove_enable  = %u", mefExt->macLearn_stationMove_enable);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_stationMove_prio    = %u", mefExt->macLearn_stationMove_prio);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_stationMove_samePrio= %u", mefExt->macLearn_stationMove_samePrio);
  PT_LOG_TRACE(LOG_CTX_INTF," MaxChannels  = %u"                , mefExt->maxChannels);
  PT_LOG_TRACE(LOG_CTX_INTF," MaxBandwidth = %llu bits/s", mefExt->maxBandwidth);
  PT_LOG_TRACE(LOG_CTX_INTF," dhcp_trusted = %u"                , mefExt->dhcp_trusted);

  /*Port Multicast Admission Control Support*/
  #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  {    
     /*If port is physical*/
    if (ptin_intf->intf_type == PTIN_EVC_INTF_PHYSICAL)
    {
      L7_uint32 ptin_port;
      L7_uint8  mask = 0x00;

      if (ptin_intf_ptintf2port(ptin_intf, &ptin_port) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Failed to obtain ptin_port from ptin_intf [ptin_intf.intf_type:%u ptin_intf:%u]",ptin_intf->intf_type, ptin_intf->intf_id);
        return L7_FAILURE;
      }
      
      /*If port is valid*/
      if (ptin_port < PTIN_SYSTEM_N_UPLINK_INTERF)
      {
        if ( (mefExt->Mask & PTIN_HWPORTEXT_MASK_MAXCHANNELS_INTF) == PTIN_HWPORTEXT_MASK_MAXCHANNELS_INTF)
          mask = PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;

        if ( (mefExt->Mask & PTIN_HWPORTEXT_MASK_MAXBANDWIDTH_INTF) == PTIN_HWPORTEXT_MASK_MAXBANDWIDTH_INTF)
          mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;

        /*If Mask Is Set */
        if (mask != 0x00)
        {
          if ( ( ((mefExt->Mask & PTIN_HWPORTEXT_MASK_MAXBANDWIDTH_INTF) == PTIN_HWPORTEXT_MASK_MAXBANDWIDTH_INTF) && 
                 (mefExt->maxBandwidth != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE && mefExt->maxBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS) ) ||
               ( ((mefExt->Mask & PTIN_HWPORTEXT_MASK_MAXCHANNELS_INTF) == PTIN_HWPORTEXT_MASK_MAXCHANNELS_INTF)
                 && (mefExt->maxChannels != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE && mefExt->maxChannels > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
          {
            PT_LOG_ERR(LOG_CTX_MSG, "Invalid Admission Control Parameters [ptin_port:%u mask:0x%04x maxBandwidth:%llu maxChannels:%hu", ptin_port, mefExt->Mask, mefExt->maxBandwidth, mefExt->maxChannels);
            return L7_FAILURE;
          }

          if (ptin_igmp_admission_control_port_set(ptin_port, mask, mefExt->maxChannels, mefExt->maxBandwidth) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_MSG,"Failed to set port admission control parameters");
            return L7_FAILURE;
          }      
        }
      }
#if 0 //This only applies for the internal ports and it is not considered an error.
      else
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Invalid Admission Control Port = %u/%u (ptin_port=%u)", ptin_intf->intf_type, ptin_intf->intf_id, ptin_port);
        return L7_FAILURE;
      }
#endif
    }
  }
  #endif
  /*End Port Multicast Admission Control Support*/

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u/%u to intIfNum", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  if (mefExt->Mask & PTIN_HWPORTEXT_MASK_DEFVID)
  {
    /* New VID: translation and verification */
    if ((mefExt->defVid == 0) || (ptin_xlate_PVID_set(intIfNum, mefExt->defVid) != L7_SUCCESS))
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error converting VID %u", mefExt->defVid);
      return L7_FAILURE;
    }
  }

  if (mefExt->Mask & PTIN_HWPORTEXT_MASK_DEFPRIO)
  {
    /* Priority verification */
    if (mefExt->defPrio > 7)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Invalid Priority %u", mefExt->defPrio);
      return L7_FAILURE;
    }

    /* Apply Default Priority configuration */
    if (usmDbDot1dPortDefaultUserPrioritySet(unit, intIfNum, mefExt->defPrio) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error applying Priority %u", mefExt->defPrio);
      return L7_FAILURE;
    }
  }

  if (mefExt->Mask & PTIN_HWPORTEXT_MASK_ACCEPTABLE_FRAME_TYPES)
  {
    //rc = usmDbQportsEnableIngressFilteringSet(unit, intIfNum, L7_ENABLE);
    if ( (mefExt->acceptable_frame_types != L7_DOT1Q_ADMIT_ALL)               && 
         (mefExt->acceptable_frame_types != L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED)  && 
         (mefExt->acceptable_frame_types != L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED)  )
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Invalid Acceptable Frame Type %d", mefExt->acceptable_frame_types);
      return L7_FAILURE;
    }

    /* Configure how to handle tagged/untagged frames */
    if (usmDbQportsAcceptFrameTypeSet(unit, intIfNum, mefExt->acceptable_frame_types) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error applying Ingress Filtering %d", mefExt->acceptable_frame_types);
      return L7_FAILURE;
    }
  }

  /* Set trusted state */
  if (mefExt->Mask & PTIN_HWPORTEXT_MASK_DHCP_TRUSTED)
  {
    ptin_dhcp_intfTrusted_set(intIfNum, mefExt->dhcp_trusted);
    ptin_pppoe_intfTrusted_set(intIfNum, mefExt->dhcp_trusted);
  }

  /* Apply configuration */
  if (dtlPtinL2PortExtSet(intIfNum, mefExt)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error SETTING MEF Ext of port %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "Success setting MEF Ext of port %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
  return L7_SUCCESS;
}

/**
 * Get Port exitension definitions
 * 
 * @param ptin_intf : Interface
 *        mefExt    : MEF Extension parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_portExt_get(const ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || mefExt==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Apply configuration */
  if (dtlPtinL2PortExtGet(intIfNum, mefExt)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Trusted state */
  mefExt->dhcp_trusted = ptin_dhcp_is_intfTrusted(intIfNum, L7_NULL);
  mefExt->Mask        |= PTIN_HWPORTEXT_MASK_DHCP_TRUSTED;

  PT_LOG_TRACE(LOG_CTX_INTF,"MefExt parameters:");
  PT_LOG_TRACE(LOG_CTX_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  PT_LOG_TRACE(LOG_CTX_INTF," Mask = 0x%08x"                    , mefExt->Mask);
  PT_LOG_TRACE(LOG_CTX_INTF," defVid = %u"                      , mefExt->defVid);
  PT_LOG_TRACE(LOG_CTX_INTF," defPrio = %u"                     , mefExt->defPrio);
  PT_LOG_TRACE(LOG_CTX_INTF," acceptable_frame_types = %u"      , mefExt->acceptable_frame_types);
  PT_LOG_TRACE(LOG_CTX_INTF," ingress_filter = %u"              , mefExt->ingress_filter);
  PT_LOG_TRACE(LOG_CTX_INTF," restricted_vlan_reg = %u"         , mefExt->restricted_vlan_reg);
  PT_LOG_TRACE(LOG_CTX_INTF," vlan_aware = %u"                  , mefExt->vlan_aware);
  PT_LOG_TRACE(LOG_CTX_INTF," type       = %u"                  , mefExt->type);
  PT_LOG_TRACE(LOG_CTX_INTF," doubletag  = %u"                  , mefExt->doubletag);
  PT_LOG_TRACE(LOG_CTX_INTF," outer_tpid = %u"                  , mefExt->outer_tpid);
  PT_LOG_TRACE(LOG_CTX_INTF," inner_tpid = %u"                  , mefExt->inner_tpid);
  PT_LOG_TRACE(LOG_CTX_INTF," egress_type = %u"                 , mefExt->egress_type);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_enable = %u"             , mefExt->macLearn_enable);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_stationMove_enable = %u" , mefExt->macLearn_stationMove_enable);
  PT_LOG_TRACE(LOG_CTX_INTF," macLearn_stationMove_prio   = %u" , mefExt->macLearn_stationMove_prio);
  PT_LOG_TRACE(LOG_CTX_INTF," Max Channels      = %u"           , mefExt->maxChannels);
  PT_LOG_TRACE(LOG_CTX_INTF," Max Bandwidth     = %u"           , mefExt->maxBandwidth);
  PT_LOG_TRACE(LOG_CTX_INTF," Interface trusted = %u"           , mefExt->dhcp_trusted);

  PT_LOG_TRACE(LOG_CTX_INTF, "Success getting MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

  return L7_SUCCESS;
}

/**
 * Set MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_portMAC_set(const ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || portMac==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"MAC address parameters:");
  PT_LOG_TRACE(LOG_CTX_INTF," Port = %u/%u"                       , ptin_intf->intf_type,ptin_intf->intf_id);
  PT_LOG_TRACE(LOG_CTX_INTF," Mask = 0x%04x"                      , portMac->Mask);
  PT_LOG_TRACE(LOG_CTX_INTF," MAC = %02x:%02x:%02x:%02x:%02x:%02x",
            portMac->macAddr[0],portMac->macAddr[1],portMac->macAddr[2],portMac->macAddr[3],portMac->macAddr[4],portMac->macAddr[5]);

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Set a new MAC address for the specified interface */
  /* Warning: if interface is removed and readded, MAC address will be set to default: For physical interfaces this shouldn't happen */
  if (nimSetIntfAddress(intIfNum,L7_NULL,portMac->macAddr)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error SETTING MAC address to port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "Success setting MAC address to port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

  return L7_SUCCESS;
}

/**
 * Get MAC address
 * 
 * @param ptin_intf : Interface
 *        portMac   : MAC address parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_portMAC_get(const ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || portMac==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Apply configuration */
  if (nimGetIntfAddress(intIfNum,L7_NULL,portMac->macAddr)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting MAC address of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"MAC address parameters:");
  PT_LOG_TRACE(LOG_CTX_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  PT_LOG_TRACE(LOG_CTX_INTF," Mask = 0x%04x"                    , portMac->Mask);
  PT_LOG_TRACE(LOG_CTX_INTF," MAC = %02x:%02x:%02x:%02x:%02x:%02x",
            portMac->macAddr[0],portMac->macAddr[1],portMac->macAddr[2],portMac->macAddr[3],portMac->macAddr[4],portMac->macAddr[5]);


  PT_LOG_TRACE(LOG_CTX_INTF, "Success getting MAC address of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

  return L7_SUCCESS;
}

/**
 * Configure a physical interface
 * 
 * @param phyConf Structure with port configuration
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_PhyConfig_set(ptin_HWEthPhyConf_t *phyConf)
{
  L7_uint   port;
  L7_uint32 value;
  L7_uint32 intIfNum = 0;
  L7_uint32 speed_mode;

  /* Validate arguments */
  if (phyConf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  port = phyConf->Port;

  /* Validate port range */
  if (port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  /* Get intIfNum */
  ptin_intf_port2intIfNum(port, &intIfNum);
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* PortEnable */
  if ( (phyConf->Mask & PTIN_PHYCONF_MASK_PORTEN) )     /* Enable mask bit */
  {
  #if (PTIN_BOARD_IS_MATRIX)
    /* Port should not have force link scheme applied */
    if ( (forcelinked_ports_bmp >> port) & 1 )
    {
      PT_LOG_INFO(LOG_CTX_INTF, "Port %u in forced link state... nothing to be done!");
    }
    else
  #endif
    {
      if (usmDbIfAdminStateGet(1, intIfNum, &value) == L7_SUCCESS && 
          (value != phyConf->PortEnable))
      {
        if (usmDbIfAdminStateSet(1, intIfNum, phyConf->PortEnable & 1) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Failed to set enable state on port# %u", port);
          return L7_FAILURE;
        }

        #if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
        /* Control txdisable for TA48GE */
        ptin_ta48ge_txdisable_control(port, !phyConf->PortEnable);
        #endif

        phyConf_data[port].PortEnable = phyConf->PortEnable & 1; /* update buffered conf data */
        PT_LOG_TRACE(LOG_CTX_INTF, " State:       %s", phyConf->PortEnable ? "Enabled":"Disabled");
      }
    }
  #if ( PTIN_BOARD_IS_STANDALONE )
    /* Update shared memory */
    pfw_shm->intf[port].admin = phyConf->PortEnable & 1;
  #endif
  }
  
  /* MaxFrame */
  #if 1
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_MAXFRAME) &&
      (usmDbIfConfigMaxFrameSizeGet(intIfNum, &value) == L7_SUCCESS) &&
      (value != phyConf->MaxFrame))
  {
    if (usmDbIfConfigMaxFrameSizeSet(intIfNum, phyConf->MaxFrame) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to set max frame on port# %u", port);
      return L7_FAILURE;
    }

    phyConf_data[port].MaxFrame = phyConf->MaxFrame; /* update buffered conf data */
    PT_LOG_TRACE(LOG_CTX_INTF, " MaxFrame:    %u", phyConf->MaxFrame);
  }
  #else
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_MAXFRAME) &&
      (ptin_intf_frame_oversize_get(intIfNum, &value) == L7_SUCCESS) &&
      (value != phyConf->MaxFrame))
  {
    if (ptin_intf_frame_oversize_set(intIfNum, phyConf->MaxFrame) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to set max frame on port# %u", port);
      return L7_FAILURE;
    }

    phyConf_data[port].MaxFrame = phyConf->MaxFrame; /* update buffered conf data */
    PT_LOG_TRACE(LOG_CTX_INTF, " MaxFrame:    %u", phyConf->MaxFrame);
  }
  #endif

  /* Loopback */
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_LOOPBACK) &&
      (usmDbIfLoopbackModeGet(intIfNum, &value) == L7_SUCCESS) &&
      (value != (phyConf->LoopBack != 0)))
  {
    if (usmDbIfLoopbackModeSet(intIfNum, (phyConf->LoopBack != 0)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to set loopback state on port# %u", port);
      return L7_FAILURE;
    }

    phyConf_data[port].LoopBack = phyConf->LoopBack; /* update buffered conf data */
    PT_LOG_TRACE(LOG_CTX_INTF, " Loopback:    %s", phyConf->LoopBack ? "Enabled":"Disabled");
  }

  /* Speed */
  if (phyConf->Mask & PTIN_PHYCONF_MASK_SPEED)
  {
    char speedstr[20];
    switch (phyConf->Speed)
    {
      case PHY_PORT_AUTONEG:
        speed_mode = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
        strcpy(speedstr, "AutoNeg");
        break;

      case PHY_PORT_100_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_100FX;
        strcpy(speedstr, "100Mbps");
        break;

      case PHY_PORT_1000_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_1000SX;
        strcpy(speedstr, "1000Mbps");
        break;

    case PHY_PORT_1000AN_GBPS:
      #if (PTIN_BOARD == PTIN_BOARD_TA48GE)
        /* AN should be always disabled: bug to be solved! */
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_1000SX; // L7_PORTCTRL_PORTSPEED_AUTO_NEG;  /* PTin modified: solve AN bug */
      #else
        speed_mode = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
      #endif
        strcpy(speedstr, "1000Mbps-AN");
        break;

      /* PTin added: Speed 2.5G */
      case PHY_PORT_2500_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
        strcpy(speedstr, "2.5G");
        break;

      /* PTin added: Speed 10G */
      case PHY_PORT_10_GBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_10GSX;
        strcpy(speedstr, "10G");
        break;

      /* PTin added: Speed 40G */
      case PHY_PORT_40_GBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_40G_KR4;
        strcpy(speedstr, "40G");
        break;

      /* PTin added: Speed 100G */
      case PHY_PORT_100_GBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_100G_BKP;
        strcpy(speedstr, "100G");
        break;

      default:
        speed_mode = L7_PORTCTRL_PORTSPEED_UNKNOWN;
        strcpy(speedstr, "UNKNOWN");
    }

    if ((speed_mode != L7_PORTCTRL_PORTSPEED_UNKNOWN) &&
        (usmDbIfDefaultSpeedGet(1, intIfNum, &value) == L7_SUCCESS) &&
        (value != speed_mode))
    {
      if (usmDbIfSpeedSet(1, intIfNum, speed_mode) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Failed to set port speed on port# %u", port);
        return L7_FAILURE;
      }

      phyConf_data[port].Speed = phyConf->Speed; /* update buffered conf data */
      PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       %s", speedstr);
    }
  }

  /* Media (value set based on port type)*/
  if (phyConf->Mask & PTIN_PHYCONF_MASK_MEDIA)
  {
    /* NOTE: it is assumed that the PON ports are mapped from port 0 to L7_SYSTEM_PON_PORTS-1 */
    if (port < PTIN_SYSTEM_N_PONS || port >= PTIN_SYSTEM_N_ETH)
    {
      phyConf_data[port].Media = PHY_PORT_MEDIA_INTERNAL;
    }
    else {
      phyConf_data[port].Media = PHY_PORT_MEDIA_OPTICAL;
    }
  }

  /* Always clear counters after a reconfiguration */
  if (ptin_intf_counters_clear(port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to clear counters on port# %u", port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, " Counters:    Cleared!");

  /* Wait for changes to be applied */
  while (!nimIntfRequestsDone())
  {
    osapiSleepMSec(10); /* Sleep for 10ms */
  }

  return L7_SUCCESS;
}


/**
 * Get physical interface configuration (read values from buffered config)
 * 
 * @param phyConf Structure to save port configuration (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_PhyConfig_get(ptin_HWEthPhyConf_t *phyConf)
{
  L7_uint port;

  /* Validate arguments */
  if (phyConf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  port = phyConf->Port;

  /* Validate port range */
  if (port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  *phyConf = phyConf_data[port];

  return L7_SUCCESS;
}


/**
 * Get physical interface state (read values from switch)
 * 
 * @param phyConf Structure to save port configuration (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_PhyState_read(ptin_HWEthPhyState_t *phyState)
{
  L7_uint   port;
  L7_uint32 value;
  L7_uint32 intIfNum = 0;
  L7_uint32 speed_mode;

  /* Validate arguments */
  if (phyState == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  port = phyState->Port;
  phyState->Mask = 0;

  /* Validate port range */
  if (port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  /* Get intIfNum ID */
  ptin_intf_port2intIfNum(port, &intIfNum);
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* Speed */
  if (usmDbIfSpeedGet(1, intIfNum, &speed_mode))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get speed of port# %u", port);
    return L7_FAILURE;
  }
  else {
    phyState->Mask |= PTIN_PHYSTATE_MASK_SPEED;

    switch (speed_mode)
    {
      case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        phyState->Speed = PHY_PORT_AUTONEG;
        phyState->AutoNegComplete = L7_FALSE;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       AutoNeg");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        phyState->Speed = PHY_PORT_100_MBPS;
        phyState->AutoNegComplete = L7_FALSE;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       100Mbps");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        phyState->Speed = PHY_PORT_1000_MBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       1000Mbps");
        break;

      /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        phyState->Speed = PHY_PORT_2500_MBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       2.5G");
        break;

      /* PTin added: Speed 10G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        phyState->Speed = PHY_PORT_10_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       10G");
        break;

      /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
        phyState->Speed = PHY_PORT_40_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       40G");
        break;

      /* PTin added: Speed 100G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
        phyState->Speed = PHY_PORT_100_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:      100G");
        break;

      /* PTin end */

      default:
        phyState->Speed = -1;
    }
  }

  /* Full-Duplex */
  phyState->Mask |= PTIN_PHYSTATE_MASK_DUPLEX;
  phyState->Duplex = 1;
  PT_LOG_TRACE(LOG_CTX_INTF, " Full-Duplex: %s", phyState->Duplex?"Enabled":"Disabled");

  /* Link State */
  if (nimGetIntfLinkState(intIfNum, &value) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get link state of port# %d", port);
    return L7_FAILURE;
  }
  else
  {
    phyState->Mask |= PTIN_PHYSTATE_MASK_LINKUP;
    phyState->LinkUp = (value == L7_UP);
    PT_LOG_TRACE(LOG_CTX_INTF, " Link State:  %s", phyState->LinkUp ? "Up":"Down");
  }

  /* Auto-negotiation complete? */
  if (phyConf_data[port].Speed == PHY_PORT_AUTONEG ||
      phyConf_data[port].Speed == PHY_PORT_1000AN_GBPS)
  {
    /* AN should be always disabled: bug to be solved! */
    phyState->Mask |= PTIN_PHYSTATE_MASK_AUTONEG;
    phyState->AutoNegComplete = (phyState->LinkUp) ? 1 : 0;
    PT_LOG_TRACE(LOG_CTX_INTF, " AutoNeg End: %s", phyState->AutoNegComplete?"Yes":"No");
  }

  return L7_SUCCESS;
}


/****************************************************************************** 
 * COUNTERS FUNCTIONS
 ******************************************************************************/

/**
 * Read counter of a specific physical interface
 * 
 * @param portStats Structure to save port counters (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_counters_read(ptin_HWEthRFC2819_PortStatistics_t *portStats)
{
  L7_uint port;

  /* Validate arguments */
  if (portStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  port = portStats->Port;

  /* Validate port range */
  if (port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  return dtlPtinCountersRead(portStats);
}


/**
 * Clear counters
 * 
 * @param ptin_port
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_counters_clear(L7_uint ptin_port)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;
  L7_RC_t rc;

  /* Validate port range */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", ptin_port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  memset(&portStats, 0x00, sizeof(portStats));
  portStats.Port = ptin_port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;

  rc = dtlPtinCountersClear(&portStats);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error clearing stats of Port# %u", ptin_port);
    return rc;
  }

#if (PTIN_BOARD_IS_MATRIX)
  /* Reset linkStatus data */
  rc = ptin_control_linkStatus_reset(ptin_port);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error clearing linkStatus of Port# %u", ptin_port);
    return rc;
  }
#endif /*PTIN_BOARD_IS_MATRIX*/

  return L7_SUCCESS;
}


/**
 * Read counters activity (of physical ports)
 * 
 * @param portActivity Structure to save port counters activity (at the 
 * moment, masks are ignored, therefore all values are read for all ports) 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_counters_activity_get(ptin_HWEth_PortsActivity_t *portActivity)
{
  /* Validate arguments */
  if (portActivity == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  return dtlPtinCountersActivityGet(portActivity);
}

/****************************************************************************** 
 * Board management
 ******************************************************************************/

/**
 * Get board id for a particular interface
 *  
 * @param ptin_port
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_boardid_get(L7_int ptin_port, L7_uint16 *board_id)
{
  /* Only applied to CXO640G boards */
  #if (PTIN_BOARD_IS_MATRIX)

  L7_uint16 slot_id;

  if (ptin_intf_port2SlotPort(ptin_port, &slot_id, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: ptin_port=%d", ptin_port);
    return L7_FAILURE;
  }
  /* Validate input params */
  if ((slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX) && (slot_id != 0))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid slot_id=%d", slot_id);
    return L7_FAILURE;
  }

  if (board_id != L7_NULLPTR)
  {
    *board_id = ptin_slot_boardid[slot_id]; 
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Set board id for a particular interface (will override 
 * board_id of other interfaces, if they belong to the same 
 * slot) 
 *  
 * @param ptin_port
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_boardid_set(L7_int ptin_port, L7_uint16 board_id)
{
  /* Only applied to CXO640G boards */
  #if (PTIN_BOARD_IS_MATRIX)

  L7_uint16 slot_id;

  if (ptin_intf_port2SlotPort(ptin_port, &slot_id, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: ptin_port=%d", ptin_port);
    return L7_FAILURE;
  }
  /* Validate input params */
  if ((slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX) && (slot_id != 0))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: slot_id=%d", slot_id);
    return L7_FAILURE;
  }

  ptin_slot_boardid[slot_id] = board_id;
  #endif

  return L7_SUCCESS;
}

/**
 * Get board id for a particular slot
 *  
 * @param slot_id
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_slot_boardid_get(L7_int slot_id, L7_uint16 *board_id)
{
  /* Only applied to CXO640G boards */
  #if (PTIN_BOARD_IS_MATRIX)

  /* Validate input params */
  if ((slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX) && (slot_id != 0))
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: slot_id=%d", slot_id);
    return L7_FAILURE;
  }

  if (board_id != L7_NULLPTR)
  {
    *board_id = ptin_slot_boardid[slot_id]; 
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Set board id for a particular slot
 *  
 * @param slot_id
 * @param board_id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_slot_boardtype_set(L7_int slot_id, L7_uint16 board_id)
{
  /* Only applied to CXO640G boards */
  #if (PTIN_BOARD_IS_MATRIX)

  /* Validate input params */
  if ((slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX) && (slot_id != 0))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: slot_id=%d", slot_id);
    return L7_FAILURE;
  }

  ptin_slot_boardid[slot_id] = board_id;
  #endif

  return L7_SUCCESS;
}

L7_RC_t ptin_intf_boardtype_dump(void)
{
  /* Only applied to CXO640G boards */
  #if (PTIN_BOARD_IS_MATRIX)

  L7_uint i;
  L7_uint16 board_id;

  printf("Boards at slots:\r\n");
  for (i=PTIN_SYS_LC_SLOT_MIN; i<=PTIN_SYS_LC_SLOT_MAX; i++)
  {
    printf("   Slot %2u: ", i);
    if (ptin_slot_boardid_get(i, &board_id) != L7_SUCCESS)
      printf("Error!\r\n");
    else if (board_id == 0)
      printf("empty\r\n");
    else 
      printf("%u\r\n", board_id);
  }
  printf("Boards at interfaces:\r\n");
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    printf("   Port %2u: ", i);
    if (ptin_intf_boardid_get(i, &board_id) != L7_SUCCESS)
      printf("Error!\r\n");
    else if (board_id == 0)
      printf("empty\r\n");
    else 
      printf("%u\r\n", board_id);
  }
  #else
  printf("Not supported!\r\n");
  #endif

  fflush(stdout);

  return L7_SUCCESS;
}


/****************************************************************************** 
 * Port, LAGs and Interfaces convertion functions
 ******************************************************************************/

/**
 * Get all interface formats
 * 
 * @param intf 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_any_format(ptin_intf_any_format_t *intf)
{
  L7_uint32 ptin_port, intIfNum, lag_idx;
  L7_INTF_TYPES_t intf_type;
  L7_RC_t rc, rc_global;

  /* Validate arguments */
  if (intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Null pointer");
    return L7_FAILURE;
  }
  /* If already in ALL format, there is nothing to do */
  if (intf->format == PTIN_INTF_FORMAT_ALL)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Format is already ALL type");
    return L7_SUCCESS;
  }

  intIfNum  = 0;
  ptin_port = (L7_uint32)-1;
  rc = L7_SUCCESS;

  /* Get reference ptin_port format */
  switch (intf->format)
  {
  case PTIN_INTF_FORMAT_PORT:
    ptin_port = intf->value.ptin_port;
    break;

  case PTIN_INTF_FORMAT_LAGID:
    rc = ptin_intf_lag2port(intf->value.lag_id, &ptin_port);
    break;

  case PTIN_INTF_FORMAT_TYPEID:
    rc = ptin_intf_typeId2port(intf->value.ptin_intf.intf_type, intf->value.ptin_intf.intf_id, &ptin_port);
    break;

  case PTIN_INTF_FORMAT_SYS_SLOTPORT:
    rc = ptin_intf_slotPort2port(intf->value.slot_port.system_slot, intf->value.slot_port.system_port, &ptin_port);
    break;

  case PTIN_INTF_FORMAT_USP:
    rc = nimGetIntIfNumFromUSP(&intf->value.usp, &intIfNum);
    if (rc == L7_SUCCESS)
    {
      rc = ptin_intf_intIfNum2port(intIfNum, &ptin_port);
    }
    break; 

  case PTIN_INTF_FORMAT_INTIFNUM:
    intIfNum = intf->value.intIfNum;
    rc = ptin_intf_intIfNum2port(intIfNum, &ptin_port);
    break;

  default:
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid format: %u", intf->format);
    rc = L7_FAILURE;
  }

  /* Validate operation result */
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"An error have occurred for given format (%u): rc=%d", intf->format, rc);
    return rc;
  }

  /* Validate ptin_port range */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"ptin_port is out of range: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Error to be returned */
  rc_global = L7_SUCCESS;

  /* Fill structure */
  /* PTin port */
  intf->value.ptin_port = ptin_port;

  /* LAG index format */
  rc = ptin_intf_port2lag(ptin_port, &lag_idx);
  if (rc == L7_SUCCESS)
  {
    intf->value.lag_id = lag_idx;
  }
  else
  {
    intf->value.lag_id = -1;
  }

  /* Type/ID format (not supposed to have errors) */
  rc = ptin_intf_port2typeId(ptin_port, &intf->value.ptin_intf.intf_type, &intf->value.ptin_intf.intf_id);
  if (rc == L7_SUCCESS)
  {
    /* Save port type (physical/LAG/...) */
    intf->port_type = intf->value.ptin_intf.intf_type;
  }
  else
  {
    rc_global = rc;
    intf->value.ptin_intf.intf_type = 0xff;
    intf->value.ptin_intf.intf_id   = 0xff;
  }

  /* Slot/Port format */
  rc = ptin_intf_port2SlotPort(ptin_port, &intf->value.slot_port.system_slot, &intf->value.slot_port.system_port, L7_NULLPTR);
  if (rc != L7_SUCCESS)
  {
    intf->value.slot_port.system_slot = (L7_uint16)-1;
    intf->value.slot_port.system_port = (L7_uint16)-1;
  }

  /* Get board id */
  rc = ptin_intf_boardid_get(ptin_port, &intf->board_id);
  if (rc != L7_SUCCESS)
  {
    intf->board_id = 0; /* Board not present */
  }

  /* intIfNum Format (not supposed to have errors) */
  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
    rc_global = rc;
    intIfNum = 0;
  }
  /* Save intIfNum */
  intf->value.intIfNum = intIfNum;

  /* If intIfNum is valid... */
  if (intIfNum > 0 && intIfNum < L7_ALL_INTERFACES)
  {
    /* USP format (not supposed to have errors) */
    rc = nimGetUnitSlotPort(intIfNum, &intf->value.usp);
    if (rc != L7_SUCCESS) 
    {
      rc_global = rc;
      intf->value.usp.unit = (L7_uchar8)-1;
      intf->value.usp.slot = (L7_uchar8)-1;
      intf->value.usp.port = (L7_uchar8)-1;
    }

    /* IntIfNum type (not supposed to have errors) */
    rc = nimGetIntfType(intIfNum, &intf_type);
    if (rc == L7_SUCCESS)
    {
      intf->intIfNum_type = intf_type;
    }
    else
    {
      rc_global = rc;
      intf->intIfNum_type = 0xff;
    }
  }

  /* Structure have all values calculated */
  intf->format = PTIN_INTF_FORMAT_ALL;

  /* Return global error */
  return rc_global;
}

/**
 * Get the intIfNum from the slot and port location in the 
 * system.
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param intIfNum (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_slotPort2IntIfNum(L7_uint16 slot, L7_uint16 port, L7_uint32 *intIfNum)
{
  ptin_intf_t ptin_intf;

  /* Get ptin_intf */
  if (ptin_intf_slotPort2ptintf(slot, port, &ptin_intf)!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Convert ptin_intf to intIfNum */
  if (ptin_intf_ptintf2intIfNum(&ptin_intf, intIfNum)!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get slot and port location in the system, from the intIfNum
 * 
 * @author mruas (3/14/2013)
 * 
 * @param intIfNum 
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_intIfNum2SlotPort(L7_uint32 intIfNum, L7_uint16 *slot, L7_uint16 *port, L7_uint16 *board_type)
{
  ptin_intf_t ptin_intf;

  /* Convert intIfNum to ptin_intf */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"I was here!");
    return L7_FAILURE;
  }

  /* Get slot and port */
  if (ptin_intf_ptintf2SlotPort(&ptin_intf, slot, port, board_type)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"I was here!");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get the ptin_port from the slot and port location in the 
 * system. 
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param ptin_port (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_slotPort2port(L7_uint16 slot, L7_uint16 port, L7_uint32 *ptin_port_ret)
{
#if (PTIN_BOARD_IS_MATRIX)
  L7_int ptin_port = -1;

  /* Calculate slot and port */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)

  /* Only CXO160G have frontal ports */
 #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  /* Local port format */
  if (slot == 0)
  {
    if (port >= PTIN_SYSTEM_N_LOCAL_PORTS)
    {
      return L7_FAILURE;
    }
    ptin_port = port;
  }
  /* Slot/port format */
  else if (slot == PTIN_SYS_MX1_SLOT)
  {
    if (port >= PTIN_SYSTEM_N_LOCAL_PORTS/2)
    {
      return L7_FAILURE;
    }
    ptin_port = port;
  }
  /* Slot/port format */
  else if (slot == PTIN_SYS_MX2_SLOT)
  {
    if (port >= PTIN_SYSTEM_N_LOCAL_PORTS/2)
    {
      return L7_FAILURE;
    }
    ptin_port = PTIN_SYSTEM_N_LOCAL_PORTS/2 + port;
  }
  else
 #endif
  {
    /* Validate slot and port */
    if (slot < PTIN_SYS_LC_SLOT_MIN || slot > PTIN_SYS_LC_SLOT_MAX || port >= PTIN_SYS_INTFS_PER_SLOT_MAX)
    {
      //PT_LOG_ERR(LOG_CTX_INTF,"slot %u / port %u is out of range",slot,port);
      return L7_FAILURE;
    }

    ptin_port = ptin_sys_slotport_to_intf_map[slot][port];
  }

#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
  /* Do not allow slot below PTIN_SYS_LC_SLOT_MIN */
  if (slot < 2)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"slot %u is invalid", slot);
    return L7_FAILURE;
  }

  ptin_port = (port==0) ? (slot-2) : (slot+18-2);

#else
  PT_LOG_ERR(LOG_CTX_INTF, "Not in a matrix board");
  return L7_FAILURE;
#endif

  /* Validate ptin_port */
  if (ptin_port < 0 || ptin_port >= ptin_sys_number_of_ports)
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Invalid ptin_port %u derived from slot %u / port %u", ptin_port, slot, port);
    return L7_FAILURE;
  }

  /* Return result */
  if (ptin_port_ret != L7_NULLPTR)
  {
    *ptin_port_ret = ptin_port;
  }

#else
  /* Validate port */
  if (port >= ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  /* Return result */
  if (ptin_port_ret != L7_NULLPTR)
  {
    *ptin_port_ret = port;
  }
#endif

  return L7_SUCCESS;
}

/**
 * Get slot and port location in the system, from the ptin_port
 * 
 * @author mruas (3/14/2013)
 * 
 * @param ptin_port
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_port2SlotPort(L7_uint32 ptin_port, L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type)
{
#if (PTIN_BOARD_IS_MATRIX)
  L7_uint slot, port;
  L7_uint16 board_id;

  /* Validate arguments */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Invalid port id (%u)", ptin_port);
    return L7_FAILURE;
  }

  /* Calculate slot and port */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)

  /* Only CXO160G have the local port concept */
  #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  if (ptin_port < PTIN_SYSTEM_N_LOCAL_PORTS)
  {
    slot = 0;
    port = ptin_port;
  }
  else
  #endif
  {
    /* Check if interface is used */
    if ( ptin_sys_intf_to_slot_map[ptin_port] < 0 || ptin_sys_intf_to_port_map[ptin_port] < 0 )
    {
      //PT_LOG_ERR(LOG_CTX_INTF,"ptin_intf=%u/%u is not mapped!", ptin_intf->intf_type, ptin_intf->intf_id);
      return L7_FAILURE;
    }
    slot = ptin_sys_intf_to_slot_map[ptin_port];
    port = ptin_sys_intf_to_port_map[ptin_port];
  }

#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
  slot = ptin_port + 2;
  port = 0;
  if (ptin_port >= (PTIN_SYSTEM_N_PORTS-1)/2 )
  {
    slot -= (PTIN_SYSTEM_N_PORTS-1)/2;
    port  = 1;
  }

#else
  PT_LOG_ERR(LOG_CTX_INTF, "Not in a matrix board");
  return L7_FAILURE;
#endif

  /* Validate slot and port */
 #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  if (slot <= 1)
  {
    if (port >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Invalid slot (%u) or port (%u) for ptin_port=%u", slot, port, ptin_port);
    }
  }
  else
 #endif
  if (slot < PTIN_SYS_LC_SLOT_MIN || slot > PTIN_SYS_LC_SLOT_MAX || port >= PTIN_SYS_INTFS_PER_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid slot (%u) or port (%u) for ptin_port=%u", slot, port, ptin_port);
    return L7_FAILURE;
  }

  /* Get board_id */
  if (ptin_slot_boardid_get(slot, &board_id) != L7_SUCCESS)
  {
    board_id = L7_NULL;
  }

  if (slot_ret  != L7_NULLPTR)  *slot_ret   = slot;
  if (port_ret  != L7_NULLPTR)  *port_ret   = port;
  if (board_type!= L7_NULLPTR)  *board_type = board_id;

#else
  /* Validate ptin_port */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  if (slot_ret  != L7_NULLPTR)  *slot_ret   = 0;
  if (port_ret  != L7_NULLPTR)  *port_ret   = ptin_port;
  if (board_type!= L7_NULLPTR)  *board_type = 0;
#endif

  return L7_SUCCESS;
}

/**
 * Get the ptin_intf from the slot and port location in the 
 * system. 
 * 
 * @param slot_ret 
 * @param intf_ret 
 * @param ptin_intf (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_slotPort2ptintf(L7_uint16 slot, L7_uint16 port, ptin_intf_t *ptin_intf)
{
  L7_uint32 ptin_port;

  /* Determine ptin_port */
  if (ptin_intf_slotPort2port(slot, port, &ptin_port)!=L7_SUCCESS)
  {
    //PT_LOG_ERR(LOG_CTX_INTF,"Error converting slot %u / port %u to ptin_port",slot,port);
    return L7_FAILURE;
  }

  if (ptin_intf != L7_NULLPTR)
  {
    ptin_intf->intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf->intf_id   = ptin_port;
  }

  return L7_SUCCESS;
}

/**
 * Get slot and port location in the system, from the ptin_intf
 * 
 * @author mruas (3/14/2013)
 * 
 * @param ptin_intf
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_ptintf2SlotPort(const ptin_intf_t *ptin_intf, L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type)
{
  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  return ptin_intf_typeId2SlotPort(ptin_intf->intf_type, ptin_intf->intf_id,
                                   slot_ret, port_ret, board_type);
}

/**
 * Get slot and port location in the system, from the ptin_intf
 * 
 * @author mruas (3/14/2013)
 * 
 * @param intf_type 
 * @param intf_id
 * @param slot_ret (output)
 * @param intf_ret (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_typeId2SlotPort(L7_uint8 intf_type, L7_uint8 intf_id,
                                  L7_uint16 *slot_ret, L7_uint16 *port_ret, L7_uint16 *board_type)
{
  /* Do not accept non physical interfaces */
  if (intf_type!=PTIN_EVC_INTF_PHYSICAL)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface (%u/%u)", intf_type, intf_id);
    return L7_FAILURE;
  }

  /* Validate interface id */
  if (intf_id >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface id (%u/%u)", intf_type, intf_id);
    return L7_FAILURE;
  }

  return ptin_intf_port2SlotPort(intf_id, slot_ret, port_ret, board_type);
}

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get current slot_id for this board
 * 
 * @param slot_id : slot index (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_slot_get(L7_uint8 *slot_id)
{
  /* Return management slot */
  if (slot_id != L7_NULLPTR)
  {
    *slot_id = ptin_fpga_board_slot_get();
  }

  return L7_SUCCESS;
}
#endif

/**
 * Converts PTin port mapping (including LAGs) to the FP interface#
 * 
 * @param ptin_port PTin port index
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_port2intIfNum(L7_uint32 ptin_port, L7_uint32 *intIfNum)
{
  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", ptin_port, PTIN_SYSTEM_N_INTERF-1);
    return L7_FAILURE;
  }

  /* Validate output */
  if (map_port2intIfNum[ptin_port] == 0 || map_port2intIfNum[ptin_port] >= L7_MAX_INTERFACE_COUNT)
  {
    //PT_LOG_WARN(LOG_CTX_INTF, "ptin_port# %u is not assigned to any interface",ptin_port);
    return L7_FAILURE;
  }

  if (intIfNum != L7_NULLPTR)
  {
    *intIfNum = map_port2intIfNum[ptin_port];
  }

  return L7_SUCCESS;
}

/**
 * Converts FP interface# to PTin port mapping (including LAGs)
 * 
 * @param intIfNum  FP intIfNum
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_intIfNum2port(L7_uint32 intIfNum, L7_uint32 *ptin_port)
{
  /* Validate arguments */
  if (intIfNum==0 || intIfNum >= L7_MAX_INTERFACE_COUNT)
  {
    //PT_LOG_ERR(LOG_CTX_INTF, "intIfNum# %u is out of range [1..%u]", intIfNum, L7_MAX_INTERFACE_COUNT);
    return L7_FAILURE;
  }

  /* Validate output */
  if (map_intIfNum2port[intIfNum] >= PTIN_SYSTEM_N_INTERF ||
      (map_intIfNum2port[intIfNum] >= ptin_sys_number_of_ports && map_intIfNum2port[intIfNum] < PTIN_SYSTEM_N_PORTS))
  {
    //PT_LOG_WARN(LOG_CTX_INTF, "intIfNum# %u is not assigned!", intIfNum);
    return L7_FAILURE;
  }

  if (ptin_port != L7_NULLPTR)
  {
    *ptin_port = map_intIfNum2port[intIfNum];
  }

  return L7_SUCCESS;
}

/**
 * Converts ptin_port index to LAG index
 * 
 * @param ptin_port PTin port index
 * @param lag_idx   LAG index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_port2lag(L7_uint32 ptin_port, L7_uint32 *lag_idx)
{
  L7_uint32 p_lag;

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "PTin port is out of range: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Is this a potential LAG? */
  if (ptin_port < PTIN_SYSTEM_N_PORTS)
  {
    return L7_FAILURE;
  }

  /* Check if this LAG exists */
  p_lag = ptin_port - PTIN_SYSTEM_N_PORTS;

  /* Validate LAG id */
  if (p_lag >= PTIN_SYSTEM_N_LAGS)
  {
    return L7_FAILURE;
  }

  /* Check if LAG is not created */
  if (lagConf_data[p_lag].lagId == MAP_EMTPY_ENTRY)
    return L7_FAILURE;

  /* Retrieve lag index */
  if (lag_idx != L7_NULLPTR)  *lag_idx = p_lag;

  return L7_SUCCESS;
}

/**
 * Converts LAG index to ptin_port
 *  
 * @param lag_idx   LAG index 
 * @param ptin_port PTin port index 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_lag2port(L7_uint32 lag_idx, L7_uint32 *ptin_port)
{
  L7_uint32 port;

  /* Validate LAG id */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    return L7_FAILURE;
  }

  /* Check if LAG is not created */
  if (lagConf_data[lag_idx].lagId == MAP_EMTPY_ENTRY)
  {
    return L7_FAILURE;
  }

  /* Check if this LAG exists */
  port = lag_idx + PTIN_SYSTEM_N_PORTS;

  /* Validate ptin_port */
  if (port >= PTIN_SYSTEM_N_INTERF ||
      (port >= ptin_sys_number_of_ports && port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "PTin port is out of range: %u", port);
    return L7_FAILURE;
  }

  /* Retrieve lag index */
  if (ptin_port != L7_NULLPTR)  *ptin_port = port;

  return L7_SUCCESS;
}

/**
 * Converts ptin_port index to PTin port type and id
 * 
 * @param ptin_port PTin port index
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_port2ptintf(L7_uint32 ptin_port, ptin_intf_t *ptin_intf)
{
  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return ptin_intf_port2typeId(ptin_port, &ptin_intf->intf_type, &ptin_intf->intf_id);
}

/**
 * Converts PTin port type and id to ptin_port index
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_ptintf2port(const ptin_intf_t *ptin_intf, L7_uint32 *ptin_port)
{
  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  return ptin_intf_typeId2port(ptin_intf->intf_type, ptin_intf->intf_id, ptin_port);
}

/**
 * Converts PTin port type and id to ptin_port index
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param ptin_port PTin port index
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_typeId2port(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 *ptin_port)
{
  L7_uint32 p_port=0;

  /* Calculate ptin_port index */
  if (intf_type == PTIN_EVC_INTF_PHYSICAL )
  {
    if (intf_id >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Physical port id is out of range: %u", intf_id);
      return L7_FAILURE;
    }
    p_port = intf_id;
  }
  else if (intf_type == PTIN_EVC_INTF_LOGICAL)
  {
    if (intf_id >= PTIN_SYSTEM_MAX_N_LAGS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Lag id is out of range: %u", intf_id);
      return L7_FAILURE;
    }
    p_port = PTIN_SYSTEM_N_PORTS + intf_id;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port type is not valid");
    return L7_FAILURE;
  }

  /* Validate final value */
  if (p_port >= PTIN_SYSTEM_N_INTERF ||
      (p_port >= ptin_sys_number_of_ports && p_port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port id is out of range: %u", p_port);
    return L7_FAILURE;
  }

  if (ptin_port != L7_NULLPTR)  *ptin_port = p_port;

  return L7_SUCCESS;
}

/**
 * Converts ptin_port index to port type and id
 * 
 * @param ptin_port PTin port index
 * @param intf_type PTin port type (out)
 * @param intf_id   PTin port id (out)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_port2typeId(L7_uint32 ptin_port, L7_uint8 *intf_type, L7_uint8 *intf_id)
{
  ptin_intf_t p_intf;

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "PTin port is out of range: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Convert ptin_port to type+id format */
  if (ptin_port < PTIN_SYSTEM_N_PORTS)
  {
    p_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    p_intf.intf_id   = ptin_port;
  }
  else
  {
    p_intf.intf_type = PTIN_EVC_INTF_LOGICAL;
    p_intf.intf_id   = ptin_port - PTIN_SYSTEM_N_PORTS;
  }

  if (intf_type != L7_NULLPTR)  *intf_type = p_intf.intf_type;
  if (intf_id   != L7_NULLPTR)  *intf_id   = p_intf.intf_id;

  return L7_SUCCESS;
}

/**
 * Converts FP interface# to PTin port type and id
 * 
 * @param intIfNum  FP intIfNum
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_intIfNum2ptintf(L7_uint32 intIfNum, ptin_intf_t *ptin_intf)
{
  L7_uint32       ptin_port;
  L7_RC_t         rc;
  L7_INTF_TYPES_t intfType;

  if(nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Unable to get interface type for intfNum %u", intIfNum);
    return L7_FAILURE;
  }

  if(intfType == L7_LOGICAL_VLAN_INTF)
  {
    L7_uint32 minimum, maximum;
    nimIntIfNumRangeGet(L7_LOGICAL_VLAN_INTF, &minimum, &maximum);

    if (ptin_intf != L7_NULLPTR)
    {
      ptin_intf->intf_type = PTIN_EVC_INTF_ROUTING;
      ptin_intf->intf_id   = intIfNum - minimum;
    }
  }
  else if(intfType == L7_LOOPBACK_INTF)
  {
    L7_uint32 minimum, maximum;
    nimIntIfNumRangeGet(L7_LOOPBACK_INTF, &minimum, &maximum);

    if (ptin_intf != L7_NULLPTR)
    {
      ptin_intf->intf_type = PTIN_EVC_INTF_LOOPBACK; 
      ptin_intf->intf_id   = intIfNum - minimum;
    }
  }
  else
  {
    /* Get ptin_port*/
    if ((rc=ptin_intf_intIfNum2port(intIfNum, &ptin_port))!=L7_SUCCESS)
      return rc;

    /* Validate ptin_port */
    if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
        (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS))
    {
      PT_LOG_ERR(LOG_CTX_INTF, "PTin port is out of range: %u", ptin_port);
      return L7_FAILURE;
    }

    /* Convert ptin_port to type+id format */
    if (ptin_intf_port2ptintf(ptin_port, ptin_intf)!=L7_SUCCESS)
    {
      //PT_LOG_ERR(LOG_CTX_INTF, "Error converting ptin_port %u to type+id format", ptin_port);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Converts PTin port type and id to FP interface#
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_ptintf2intIfNum(const ptin_intf_t *ptin_intf, L7_uint32 *intIfNum)
{
  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  return ptin_intf_typeId2intIfNum(ptin_intf->intf_type, ptin_intf->intf_id, intIfNum);
}

/**
 * Converts PTin port type and id to FP interface#
 * 
 * @param port_type PTin port type (0 is physical and 1 is LAG) 
 * @param port_id   PTin port id
 * @param intIfNum  FP intIfNum
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_typeId2intIfNum(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 *intIfNum)
{
  L7_uint32       ptin_port=0, intIfNum0;
  L7_RC_t         rc;

  if(intf_type == PTIN_EVC_INTF_ROUTING)
  {
    if(intf_id == (L7_uint8)-1)
    {
      intIfNum0 = (L7_uint32)-1;
    }
    else
    {
      L7_uint32 minimum, maximum;

      nimIntIfNumRangeGet(L7_LOGICAL_VLAN_INTF, &minimum, &maximum);
      intIfNum0 = minimum + intf_id;
    }
  }
  else if(intf_type == PTIN_EVC_INTF_LOOPBACK)
  {
    if(intf_id == (L7_uint8)-1)
    {
      intIfNum0 = (L7_uint32)-1;
    }
    else
    {
      L7_uint32 minimum, maximum;

      nimIntIfNumRangeGet(L7_LOOPBACK_INTF, &minimum, &maximum);
      intIfNum0 = minimum + intf_id;
    }
  }
  else
  {
    /* Calculate ptin_port index */
    if (ptin_intf_typeId2port(intf_type, intf_id, &ptin_port)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error converting ptin_intf %u/%u to ptin_port", intf_type, intf_id);
      return L7_FAILURE;
    }

    /* Get interface# */
    if ((rc=ptin_intf_port2intIfNum(ptin_port,&intIfNum0))!=L7_SUCCESS)
      return rc;
  }

  /* Validate intIfNum */
  if (intIfNum0 == 0 || intIfNum0 >= L7_MAX_INTERFACE_COUNT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "IntIfNum is out of range (%u)", intIfNum0);
    return L7_FAILURE;
  }

  /* Return intIfNum */
  if (intIfNum != L7_NULLPTR)  *intIfNum = intIfNum0;

  return L7_SUCCESS;
}

/**
 * Converts LAG index [0..PTIN_SYSTEM_N_LAGS[ to FP intIfNum
 * 
 * @param lag_idx   LAG index [0..PTIN_SYSTEM_N_LAGS[
 * @param intIfNum  FP intIfNum (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_lag2intIfNum(L7_uint32 lag_idx, L7_uint32 *intIfNum)
{
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Valid entry */
  if (map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx] == 0 ||
      map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx] >= L7_ALL_INTERFACES)
  {
    return L7_FAILURE;
  }

  if (intIfNum != L7_NULLPTR)
  {
    *intIfNum = map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx]; 
  }

  return L7_SUCCESS;
}

/**
 * Convert intIfNum to LAG index
 * 
 * @param intIfNum  FP intIfNum
 * @param lag_idx   LAG index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_intIfNum2lag(L7_uint32 intIfNum, L7_uint32 *lag_idx)
{
  L7_uint32 ptin_port;
  //L7_INTF_TYPES_t sysIntfType;

  /* Validate intIfNum */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  #if 0
  /* Get interface type */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Can't get intIfNum %u type", intIfNum);
    return L7_FAILURE;
  }
  /* This should be a LAG port */
  if (sysIntfType != L7_LAG_INTF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "intIfNum %u is not a lag (type=%u)", intIfNum, sysIntfType);
    return L7_FAILURE;
  }
  #endif

  /* Get port index (ptin_port representation) */
  ptin_port = map_intIfNum2port[intIfNum];

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "intIfNum %u / ptin_port %u is invalid", intIfNum, ptin_port);
    return L7_FAILURE;
  }
  /* Check again for LAG type */
  if (ptin_port < PTIN_SYSTEM_N_PORTS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "intIfNum %u / ptin_port %u is physical type", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* Return lag_index */
  if (lag_idx != L7_NULLPTR)
  {
    *lag_idx = ptin_port - PTIN_SYSTEM_N_PORTS;
  }

  return L7_SUCCESS;
}

#if PTIN_BOARD_IS_MATRIX
/**
 * Converts Slot to LAG index [0..PTIN_SYSTEM_N_LAGS[
 * 
 * @param slot      slot
 * @param lag_idx   LAG index [0..PTIN_SYSTEM_N_LAGS[ (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
inline L7_RC_t ptin_intf_slot2lagIdx(L7_uint16 slot, L7_uint32 *lag_idx)
{
  L7_uint32 aux;

  if ( (slot < PTIN_SYS_LC_SLOT_MIN) || (slot > PTIN_SYS_LC_SLOT_MAX) )
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Slot %u is out of range [%u..%u]", slot, PTIN_SYS_LC_SLOT_MIN, PTIN_SYS_LC_SLOT_MAX);
    return L7_FAILURE;
  }

  aux = slot + (PTIN_SYSTEM_INTERNAL_LAGID_BASE-PTIN_SYS_LC_SLOT_MIN);

  if (aux >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", aux, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  if (lag_idx != L7_NULLPTR)
  {
    *lag_idx = aux; 
  }

  return L7_SUCCESS;
} 
#endif  // PTIN_BOARD_IS_MATRIX

/**
 * Check if a LAG is created [0..PTIN_SYSTEM_N_LAGS[
 * 
 * @param lag_idx   LAG index [0..PTIN_SYSTEM_N_LAGS[
 * 
 * @return L7_RC_t L7_TRUE/L7_FALSE
 */
inline L7_RC_t ptin_intf_lag_exists(L7_uint32 lag_idx)
{
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG is not created */
  if (lagConf_data[lag_idx].lagId == MAP_EMTPY_ENTRY)
    return L7_FALSE;

  return L7_TRUE;
}


/****************************************************************************** 
 * LAGs FUNCTIONS
 ******************************************************************************/

/*
 * LAGs Manipulation Functions
 */ 
/**
 * Gets a LAGs configuration (buffered)
 * 
 * @param lagInfo Pointer to the output structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LagConfig_get(ptin_LACPLagConfig_t *lagInfo)
{
  L7_uint32 lag_idx;

  /* Validate arguments */
  if (lagInfo == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  lag_idx = lagInfo->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG is not created */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u does not exist", lag_idx);
    return L7_FAILURE;
  }

  /* Copy data to the output structure */
  *lagInfo = lagConf_data[lag_idx];

  return L7_SUCCESS;
}

/**
 * Check if a particular port is used for uplink protection
 * 
 * @author mruas (5/2/2014)
 * 
 * @param ptin_port 
 * 
 * @return L7_BOOL 
 */
L7_BOOL ptin_intf_is_uplinkProtection(L7_uint32 ptin_port)
{
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  return (((uplink_protection_ports_bmp >> ptin_port) & 1) == 1);
#else
  return L7_FALSE;
#endif
}

/**
 * Check if a protection port is active
 * 
 * @author mruas (5/28/2014)
 * 
 * @param ptin_port 
 * 
 * @return L7_BOOL 
 */
L7_BOOL ptin_intf_is_uplinkProtectionActive(L7_uint32 ptin_port)
{
  #ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  return (((uplink_protection_ports_active_bmp >> ptin_port) & 1) == 1); 
  #else
  return L7_FALSE;
  #endif
}

/**
 * Creates a LAG 
 *  
 * NOTES: 
 *  1. If LAG is already created, no error is reported and configs are updated 
 *  2. The operation sequence is based on "commandInterfaceAddPort()" function (cli_config.c) 
 *  3. No PON interfaces are allowed to form LAGs 
 *  4. All LAG members MUST have the same interface speed
 *  5. No LAG is created if the members list is empty
 *  6. All LAG members MUST have the same (outer) TPID
 *  7. Lag TPID is automatically configured based on the members TPID
 *  
 * @param lagInfo Pointer to the structure with LAG info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_Lag_create(ptin_LACPLagConfig_t *lagInfo)
{
  L7_uint64 members_pbmp_all;
  L7_uint64 members_pbmp;
  L7_uint32 lag_idx;
  L7_uint32 lag_intf;
  L7_uint32 port, i;
  L7_uint32 intIfNum=0;
  L7_uint32 maxFrame=0;
  L7_uint32 value;
  L7_char8  lag_name[DOT3AD_MAX_NAME];
  L7_RC_t   rc=L7_SUCCESS, res;
  L7_BOOL   newLag;
  //L7_uint32 ifSpeed;
  L7_uint16 lagEtherType;
  ptin_intf_t ptin_intf;
  #if 0
  ptin_LACPAdminState_t lacpAdminState;
  #endif

  #if 0
  ptin_intf_t     ptin_intf;
  ptin_HWMefExt_t mefExt_phy, mefExt_lag;

  /* MAC Learning definitions */
  memset(&mefExt_lag,0x00,sizeof(ptin_HWMefExt_t));
  #endif

  /* Validate arguments */
  if (lagInfo == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  lag_idx = lagInfo->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Uplink protection */
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  /* For Uplink ports, only disable linkscan and force link */
  if (lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
  {
    //L7_uint16 board_id;

    members_pbmp = lagInfo->members_pbmp64;

    /* Loop through all the phy ports and check if any is being added or removed */
    for (port = 0; port < ptin_sys_number_of_ports; port++, members_pbmp>>=1)
    {
      /* Get intIfNum */
      if (ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Error converting port %u to intIfNum", port);
        continue;
      }

      /* For non member ports, but are configured, they should be removed */
      if (!(members_pbmp & 1))
      {
        /* If port is a protection, but now is not meant to be, remove it */
        if ((lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE] >> port) & 1)
        {
        #ifdef PTIN_LINKSCAN_CONTROL
          #ifdef MAP_CPLD
          /* Only active matrix will manage linkscan and force links */
          if (ptin_fpga_mx_is_matrixactive())
          {
            /* Enable linkscan */
            ptin_intf_linkscan_set(intIfNum, L7_ENABLE);
          }
          #endif
        #endif
          /* Add port to all vlans at hardware */
          ptin_vlan_port_add(port, 0);

          /* Remove protection port */
          uplink_protection_ports_bmp &= ~((L7_uint64) 1 << port);
          /* Remove active protection port */
          uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << port);
          /* Clear lag bitmap */
          lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE] &= ~((L7_uint64) 1 << port);
        }
        continue;
      }

      #if 0
      /* Check if this port can be a protection port */
      if (ptin_intf_boardid_get(port, &board_id) != L7_SUCCESS || !PTIN_BOARD_IS_UPLINK(board_id))
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Port %u does not belong to an uplink board (board id %u)", port, board_id);
        continue;
      }
      #endif

      /* If this is already a protection port... */
      if ((uplink_protection_ports_bmp >> port) & 1)
      {
        if ((lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE] >> port) & 1)
        {
          PT_LOG_WARN(LOG_CTX_INTF,"Port %u already belongs to this protection lag", port);
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Port %u already belongs to another protection lag", port);
        }
        continue;
      }

      /* Remove port from all vlans at hardware */
      ptin_vlan_port_removeFlush(port, 0);
      PT_LOG_INFO(LOG_CTX_INTF,"Port %u removed from all vlans", port);

      /* Check if port is enabled */
      if (!phyConf_data[port].PortEnable)
      {
        if (usmDbIfAdminStateSet(0, intIfNum, L7_ENABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error enabling port %u", port);
          ptin_vlan_port_add(port, 0);
          continue;
        }
        phyConf_data[port].PortEnable = L7_TRUE;
        PT_LOG_INFO(LOG_CTX_INTF,"Port %u enabled", port);
      }
      else
      {
        PT_LOG_INFO(LOG_CTX_INTF,"Port %u is already enabled", port);
      }

    #ifdef PTIN_LINKSCAN_CONTROL
      #ifdef MAP_CPLD
      /* Only active matrix will manage linkscan and force links */
      if (ptin_fpga_mx_is_matrixactive())
      {
        /* Guarantee linkscan is enabled */
        if (ptin_intf_linkscan_set(intIfNum, L7_ENABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error disablink linkscan for intIfNum %u", intIfNum);
        }
        else
        {
          PT_LOG_INFO(LOG_CTX_INTF,"Linkscan successfully enabled for intIfNum %u (port %u)", intIfNum, port);
        }
      }
      #endif
    #endif

      /* New port */
      uplink_protection_ports_bmp |= (L7_uint64) 1 << port;
      /* Remove active protection port */
      uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << port);
      /* Save port to lag bitmap */
      lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE] |= (L7_uint64) 1 << port;

      PT_LOG_INFO(LOG_CTX_INTF,"Port %u is a protection port", port);
    }

    /* Update LAG structures */
    CLEAR_LAG_CONF(lag_idx);
    lagConf_data[lag_idx].admin = L7_ENABLE;
    lagConf_data[lag_idx].lagId = lag_idx;
    lagConf_data[lag_idx].members_pbmp64 = members_pbmp;

    PT_LOG_WARN(LOG_CTX_INTF, "Protection LAG %u configured", lag_idx);
    return L7_SUCCESS;
  }
#endif

  /* If members list is empty, report error */
  if (lagInfo->members_pbmp64 == 0)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: members list is empty", lag_idx);
    return L7_FAILURE;
  }

  /* Check if members have all the same interface speed, EtherType and MaxFrame size */
  members_pbmp = lagInfo->members_pbmp64 >> PTIN_SYSTEM_N_PONS;
  //ifSpeed      = 0;
  lagEtherType = 0;
  maxFrame     = 0;
  for (port = PTIN_SYSTEM_N_PONS; port < ptin_sys_number_of_ports; port++, members_pbmp>>=1)
  {
    if (members_pbmp & 1)
    {
      ptin_intf_port2intIfNum(port, &intIfNum);
  
      #if 0
      /* check to find the speed of the interface about to join a portChannel */
      if (usmDbIfSpeedGet(1, intIfNum, &value) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading ifSpeed from port# %u", lag_idx, port);
        return L7_FAILURE;
      }

      if (ifSpeed == 0)
        ifSpeed = value;

      if (value != ifSpeed)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: there are members with different if speed", lag_idx);
        return L7_FAILURE;
      }
      #endif

      if (lagEtherType == 0)
        lagEtherType = phyExt_data[port].outer_tpid;

      if (lagEtherType != phyExt_data[port].outer_tpid)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: there are members with different EtherType (0x%04X != 0x%04X)",
                lag_idx, lagEtherType, phyExt_data[port].outer_tpid);
        return L7_FAILURE;
      }

      /* Calculate max frame */
      if (phyConf_data[port].MaxFrame > maxFrame)
      {
        maxFrame = phyConf_data[port].MaxFrame;
      }
    }
  }

  /* Check if they do not belong to other LAGs */
  members_pbmp_all = 0;
  for (i=0; i<PTIN_SYSTEM_N_LAGS; i++)
    if ((i != lag_idx) && (lagConf_data[i].lagId != MAP_EMTPY_ENTRY))
      members_pbmp_all |= lagConf_data[i].members_pbmp64; /* All members from other LAGs */
  /* The two bitmaps must not overlap */
  if (lagInfo->members_pbmp64 & members_pbmp_all)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: the following members belong to other LAGs: 0x%010llX", lag_idx, lagInfo->members_pbmp64 & members_pbmp_all);
    return L7_FAILURE;
  }

  /* Check if members are not being used in any EVC */
  members_pbmp = lagInfo->members_pbmp64;
  for (i = 0; i < ptin_sys_number_of_ports; i++, members_pbmp>>=1)
  {
    if ((members_pbmp & 1) &&
        (ptin_evc_is_intf_in_use(i)))
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: member %u is being used in EVCs! Lag cannot be created/updated", lag_idx, i);
      return L7_FAILURE;
    }
  }

  /* If LAG does not exist, create it */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    osapiSnprintf(lag_name, DOT3AD_MAX_NAME, "lag%02u", lag_idx);

    /* Try to create an empty LAG */
    res = usmDbDot3adCreateSet(1, lag_idx, lag_name, FD_DOT3AD_ADMIN_MODE, 
                              FD_DOT3AD_LINK_TRAP_MODE, 0,
                              FD_DOT3AD_HASH_MODE, NULL, &lag_intf);
    if (res != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error on usmDbDot3adCreateSet()", lag_idx);
      return L7_FAILURE;
    }

    /* Wait until new lag is created */
    do {
      if ((usmDbIntfStatusGet(lag_intf, &value) == L7_SUCCESS) && (value == L7_INTF_ATTACHED))
        break;

      osapiSleepMSec(10);
    } while (1);

    /* Make sure the structure is empty */
    CLEAR_LAG_CONF(lag_idx);

    lagConf_data[lag_idx].lagId = lag_idx;
    UPDATE_LAG_MAP(lag_idx, lag_intf);
    newLag = L7_TRUE;

    PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %02u created with empty members (interface# %02u)", lag_idx, lag_intf);

    /* Configure Max Frame Size on LAG interface
     * IMPORTANT!!!
     *   Max Frame size on LAGs MUST be set before adding new members, otherwise
     *   values may not be applied!
     *   This happens because the process of adding/removing LAG members is
     *   asynchronous and the value taken on those messages might be the old one */
    if ((maxFrame != 0)
        && (usmDbIfConfigMaxFrameSizeGet(lag_intf, &value) == L7_SUCCESS)
        && (value != maxFrame))
    {
      if (usmDbIfConfigMaxFrameSizeSet(lag_intf, maxFrame) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: failed setting max frame (%u)", lag_idx, maxFrame);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF, " MaxFrameSize set to %u", maxFrame);
      }
    }

    /* QoS initialization */
    ptin_intf.intf_type = PTIN_EVC_INTF_LOGICAL;
    ptin_intf.intf_id   = lag_idx;

    if (ptin_intf_QoS_init(&ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: Error initializing QoS definitions", lag_idx);
      rc = L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u: Success initializing QoS definitions", lag_idx);
    }

    /* Set MAC learn attributes */
   #if (PTIN_BOARD_IS_MATRIX)
    if (lag_idx < PTIN_SYSTEM_N_LAGS_EXTERNAL)
    {
      ptin_HWPortExt_t port_ext;

      memset(&port_ext, 0x00, sizeof(ptin_HWPortExt_t));

      port_ext.macLearn_enable                = L7_TRUE;
      port_ext.macLearn_stationMove_enable    = L7_TRUE;
      port_ext.macLearn_stationMove_samePrio  = L7_TRUE;
      port_ext.macLearn_stationMove_prio      = 2;
      port_ext.Mask = PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE |
                      PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE |
                      PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO |
                      PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO;

      if (ptin_intf_portExt_set(&ptin_intf, &port_ext) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: Error setting MAC learning attributes", lag_idx);
      }
    }
   #else
    /* For Linecards, LAG 1/0 belongs to backplane... should be trusted */
    #if (PTIN_BOARD_IS_LINECARD)
    if (lag_idx == 0)
    #elif (PTIN_BOARD_IS_MATRIX)
    if (lag_idx >= PTIN_SYSTEM_INTERNAL_LAGID_BASE)
    #endif
    {
     #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
      ptin_dhcp_intfTrusted_set(lag_intf, L7_TRUE); 
      ptin_pppoe_intfTrusted_set(lag_intf, L7_TRUE);
      /* Internal interfaces of linecards, should always be trusted */
      PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u is DHCP/PPPoE trusted", lag_idx);
     #endif
     #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_MATRIX)
      usmDbDaiIntfTrustSet(lag_intf, L7_TRUE);
      PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u is DAI trusted", lag_idx);
     #endif
    }
   #endif
  }
  else
  {
    /* Get intIfNum assigned to this LAG */
    ptin_intf_lag2intIfNum(lag_idx, &lag_intf);
    newLag = L7_FALSE;

    /* Confirm that LAG is actually created */
    if (usmDbDot3adIsConfigured(1, lag_intf) == L7_FALSE)
    {
      CLEAR_LAG_CONF(lag_idx);
      CLEAR_LAG_MAP(lag_idx);

      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: no FP config found! Clearing PTin structs and aborting operation", lag_idx);
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %02u already exists (interface# %02u)", lag_idx, lag_intf);
  }

  /* Now lets procceed with the configuration...
   * The idea is to avoid traffic interruption (when adding or removing members) */

  rc = L7_SUCCESS;
  do
  {
    rc = L7_SUCCESS;

    /* LAG Admin Mode */
    lagInfo->admin &= 1;
    if (lagConf_data[lag_idx].admin != lagInfo->admin)
    {
      if (usmDbDot3adAdminModeSet(1, lag_intf, lagInfo->admin) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error setting Admin mode to %u", lag_idx, lagInfo->admin);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF, " .admin            = %u", lagInfo->admin);
        lagConf_data[lag_idx].admin = lagInfo->admin;
      }
    }
  
    /* LAG STP Mode */
    lagInfo->stp_enable &= 1;
    if (lagConf_data[lag_idx].stp_enable != lagInfo->stp_enable)
    {
      if (usmDbDot1sPortStateSet(1, lag_intf, lagInfo->stp_enable) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error setting STP mode to %u", lag_idx, lagInfo->stp_enable);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF, " .stp_enable       = %u", lagInfo->stp_enable);
        lagConf_data[lag_idx].stp_enable = lagInfo->stp_enable;
      }
    }
  
    /* LAG Static Mode */
    lagInfo->static_enable &= 1;
    if (lagConf_data[lag_idx].static_enable != lagInfo->static_enable)
    {
      if (usmDbDot3adLagStaticModeSet(1, lag_intf, lagInfo->static_enable) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error setting Static mode to %u", lag_idx, lagInfo->static_enable);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF, " .static_enable    = %u", lagInfo->static_enable);
        lagConf_data[lag_idx].static_enable = lagInfo->static_enable;
      }
    }
  
    /* LAG Balance Mode */
    if (lagConf_data[lag_idx].loadBalance_mode != lagInfo->loadBalance_mode)
    {
      if (usmDbDot3adLagHashModeSet(1, lag_intf, lagInfo->loadBalance_mode) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error setting Balance mode to %u", lag_idx, lagInfo->loadBalance_mode);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF, " .loadBalance_mode = %u", lagInfo->loadBalance_mode);
        lagConf_data[lag_idx].loadBalance_mode = lagInfo->loadBalance_mode;
      }
    }
  } while (0);

  /* Check if any error occured */
  if (rc != L7_SUCCESS)
  {
    /* If a new LAG was created, undo its creation */
    if (newLag)
    {
      if (usmDbDot3adRemoveSet(1, lag_intf) != L7_SUCCESS)
        PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: failed to undo LAG creation", lag_idx);

      /* Return to untrust state */
      #if (PTIN_BOARD_IS_LINECARD)
      if (lag_idx == 0)
      #endif
      {
        #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
        ptin_dhcp_intfTrusted_set(lag_intf, L7_FALSE); 
        ptin_pppoe_intfTrusted_set(lag_intf, L7_FALSE);
        PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u goes back to untrusted", lag_idx);
        #endif
      }

      CLEAR_LAG_CONF(lag_idx);
      CLEAR_LAG_MAP(lag_idx);

      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u was removed (undo creation)", lag_idx);
      return rc;
    }

    /* Otherwise continue, but report error in the end... */
  }

  if (lagConf_data[lag_idx].members_pbmp64 != lagInfo->members_pbmp64)
    PT_LOG_TRACE(LOG_CTX_INTF, " .members_pbmp     = 0x%010llX", lagInfo->members_pbmp64);

  /* Initialize vars needed in the loop */
  if (newLag)
    lagConf_data[lag_idx].members_pbmp64 = 0; /* Previously set to FFs */
  members_pbmp = lagInfo->members_pbmp64;

  rc = L7_SUCCESS;

  /* Loop through all the phy ports and check if any is being added or removed */
  for (port = 0; port < ptin_sys_number_of_ports; port++, members_pbmp>>=1)
  {
    /* If an error occured in a situation that a LAG has just been created, we can abort
     * because the LAG will be deleted anyway */
    if (rc != L7_SUCCESS && newLag)
      break;

    #if 0
    /* Get MEF Ext attributes for this interface */
    ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf.intf_id   = port;
    if (ptin_intf_mefExt_get(&ptin_intf,&mefExt_phy)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error getting MEF Ext. attributes of interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      rc = L7_FAILURE;
      continue;
    }
    #endif

    ptin_intf_port2intIfNum(port, &intIfNum);

    if (members_pbmp & 1)
    {
      /* If LAG is static, then no need to check if LACP is enabled or not on this interface */
      if ((usmDbDot3adIsStaticLag(1, lag_intf, &value) == L7_SUCCESS)
          && (value != L7_TRUE))
      {
        /* Is LACP enabled on this interface? */
        if ((usmDbDot3adAggPortLacpModeGet(1, intIfNum, &value) != L7_SUCCESS) || (value != L7_ENABLE)) {
          PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: port# %u does not have LACP enabled", lag_idx, port);
          rc = L7_FAILURE;
          continue;
        }
      }

      #if 0
      /* Determine MAC Learning attributes to be applied to LAG */
      if (!mefExt_lag.macLearn_enable && mefExt_phy.macLearn_enable)
      {
        mefExt_lag.macLearn_enable = L7_TRUE;
      }
      if (!mefExt_lag.macLearn_stationMove_enable && mefExt_phy.macLearn_stationMove_enable)
      {
        mefExt_lag.macLearn_stationMove_enable = L7_TRUE;
      }
      if (mefExt_lag.macLearn_stationMove_prio < mefExt_phy.macLearn_stationMove_prio)
      {
        mefExt_lag.macLearn_stationMove_prio = mefExt_phy.macLearn_stationMove_prio;
      }
      #endif

      /* Check if this interface already belongs to a LAG */
      if (dot3adAggGet(intIfNum, &value) == L7_SUCCESS)
      {
        /* Is the LAG a different one ? */
        if (value != lag_intf)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: port# %u already belongs to another LAG", lag_idx, port);
          rc = L7_FAILURE;
        }
        continue;
      }

      #if 0
      /* Get admin state of this interface */
      memset(&lacpAdminState,0x00,sizeof(lacpAdminState));
      lacpAdminState.port = port;
      if (ptin_intf_LACPAdminState_get(&lacpAdminState) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: Error getting admin state of port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }
      #endif

      /* PTin removed: With KR4 ports, link goes down! Is this procedure necessary? */
      #if 0
      /* Disable auto-negotiation */
      if (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, L7_DISABLE) != L7_SUCCESS)
      {
        PT_LOG_WARN(LOG_CTX_INTF, "LAG# %u: could not configure Auto-Neg off on port# %u", lag_idx, port);
        //rc = L7_FAILURE;
        //continue;
      }
      #endif

      /* Add member */
      //if (1 & lagConf_data[lag_idx].members_pbmp64>>port) {}//no need to add already added members
      //else
      if (usmDbDot3adMemberAddSet(1, lag_intf, intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: could not add member port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }

      #if 0
      /* Restore admin state of this interface */
      if (ptin_intf_LACPAdminState_set(&lacpAdminState) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: Error setting admin state to port# %u", lag_idx, port);
        //rc = L7_FAILURE;
        //continue;
      }
      #endif

      lagConf_data[lag_idx].members_pbmp64 |= (L7_uint64)1 << port;
      PT_LOG_TRACE(LOG_CTX_INTF, " Port# %02u added", port);

      /* To be removed */
      #if 0
      /* Uplink protection */
      #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
      if (newLag && lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
      {
        /* Disable linkscan */
        if (ptin_intf_linkscan_set(intIfNum, L7_DISABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error disablink linkscan for intIfNum %u", intIfNum);
        }
        else if (ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error forcing link-up for intIfNum %u", intIfNum);
        }
        else
        {
          PT_LOG_INFO(LOG_CTX_INTF,"Linkscan successfully disabled for intIfNum %u (port %u)", intIfNum, port);
        }
      }
      #endif
      #endif
    }
    /* Port not member (is it to be removed ?) */
    else
    {
      /* If this interface does not belong to any lag, do nothing */
      if (dot3adAggGet(intIfNum, &value) != L7_SUCCESS)
        continue;

      /* If belongs to a lag, but a different one, do nothing */
      if (value != lag_intf)
        continue;

      /* At this point, we know this interface belongs to this lag. Just remove it */
      if (usmDbDot3adMemberDeleteSet(1, lag_intf, intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: could not remove member port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }

      /* Update PortGroup for the member removed (reset to default value) */
      if (ptin_xlate_portgroup_set(intIfNum, PTIN_XLATE_PORTGROUP_INTERFACE) != L7_SUCCESS)
      {
        PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: could not update PortGroup for member port# %u", lag_idx, port);
        rc = L7_FAILURE;
      }

      lagConf_data[lag_idx].members_pbmp64 &= ~((L7_uint64)1 << port);
      PT_LOG_TRACE(LOG_CTX_INTF, " Port# %02u removed", port);
    }
  }

  /* Update PortGroups (used on egress translations) */
  if (ptin_xlate_portgroup_set(lag_intf, PTIN_XLATE_PORTGROUP_INTERFACE) != L7_SUCCESS)
  {
    /* NOTE: if any error occurs during PortGroup set, it originates inconsistency on
     * the portgroups lookup table, which means a CRITICAL or even FATAL situation!
     * However, any error should occur ONLY during debug or validation process, and
     * not during normal operation (under production) */ 
    PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: could not update PortGroup for this LAG", lag_idx);
    rc = L7_FAILURE;
  }

  /* Remove this interface from VLAN 1 (only if a new LAG was created)
   * The idea is to avoid accepting traffic comming on VLAN1 that is not LACP */
  if (rc == L7_SUCCESS && newLag)
  {
    if (usmDbVlanMemberSet(1, 1, lag_intf, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: could not remove interface# %u from VLAN 1", lag_idx, lag_intf);
      rc = L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, " Intf# %02u removed from VLAN# 1", lag_intf);
    }

    /* Enable DVLAN mode */
    if (usmDbDvlantagIntfModeSet(0, lag_intf, L7_ENABLE) != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to enable DVLAN mode on LAG# %u", lag_idx);
      rc = L7_FAILURE;
    }

    if (lagEtherType != 0)
    {
      res = usmDbDvlantagIntfEthertypeSet(0, lag_intf, lagEtherType, L7_TRUE);
      if ((res != L7_SUCCESS) && (res != L7_ALREADY_CONFIGURED))
      {
        PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to configure TPID 0x%04X on LAG# %u (rc = %d)", lagEtherType, lag_idx, rc);
        rc = L7_FAILURE;
      }
    }
  }

  #if 0
  /* In case of success, apply MAC learning attributes */
  if ( rc == L7_SUCCESS )
  {
    /* Use LAG intIfNum */
    if (ptin_intf_intIfNum2ptintf(lag_intf,&ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error getting ptin_intf from LAG intIfNum %u",lag_intf);
      rc = L7_FAILURE;
    }
    else
    {
      /* Only configure MAC Learning definitions */
      mefExt_lag.Mask = PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO;
      if (ptin_intf_mefExt_set(&ptin_intf,&mefExt_lag)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Error applying MEF Ext attributes to ptin_intf %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
        rc = L7_FAILURE;
      }
    }
  }
  #endif

  /* Check if any error occured */
  if (rc != L7_SUCCESS)
  {
    /* If a new LAG was created, undo its creation */
    if (newLag)
    {
      if (usmDbDot3adRemoveSet(1, lag_intf) != L7_SUCCESS)
        PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: failed to undo LAG creation", lag_idx);

      /* Return to untrust state */
      #if (PTIN_BOARD_IS_LINECARD)
      if (lag_idx == 0)
      #endif
      #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
      {
        ptin_dhcp_intfTrusted_set(lag_intf, L7_FALSE); 
        ptin_pppoe_intfTrusted_set(lag_intf, L7_FALSE);
        PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u goes back to untrusted", lag_idx);
      }
      #endif

      CLEAR_LAG_CONF(lag_idx);
      CLEAR_LAG_MAP(lag_idx);

      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u was removed (undo creation)", lag_idx);
      return rc;
    }
  }

  return rc;
}

/**
 * Deletes a LAG
 * 
 * @param lag_idx LAG Identifier
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_Lag_delete(L7_uint32 lag_idx)
{ 
  L7_uint32   lag_intIfNum;
  ptin_intf_t lag_ptin_intf;
  L7_uint     lag_port;
  L7_uint32   value;
  L7_uint     i;
  L7_uint64   ptin_pbmp;
  L7_uint32   intIfNum = 0;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  if (!ptin_intf_lag_exists(lag_idx))
  {
    PT_LOG_NOTICE(LOG_CTX_INTF, "LAG# %u is disabled!", lag_idx);
    return L7_SUCCESS;
  }
  
  lag_port = PTIN_SYSTEM_N_PORTS + lag_idx;

  /* Uplink protection */
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  L7_uint   port;

  /* For Uplink ports, only disable linkscan and force link */
  if (lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
  {
    ptin_pbmp = lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE];

    /* Loop through all the phy ports and check if any is being added or removed */
    for (port = 0; port < ptin_sys_number_of_ports; port++, ptin_pbmp>>=1)
    {
      if (!(ptin_pbmp & 1))
        continue;
      
      if (ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Error converting port %u to intIfNum", port);
        continue;
      }

    #ifdef PTIN_LINKSCAN_CONTROL
      #ifdef MAP_CPLD
      /* Only active matrix will manage linkscan and force links */
      if (ptin_fpga_mx_is_matrixactive())
      {
        /* Enable linkscan */
        if (ptin_intf_linkscan_set(intIfNum, L7_ENABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error enabling linkscan for intIfNum %u", intIfNum);
        }
        else
        {
          PT_LOG_INFO(LOG_CTX_INTF,"Linkscan successfully enabled for intIfNum %u (port %u)", intIfNum, port);
        }
      }
      #endif
    #endif

      /* Remove port from all vlans at hardware */
      ptin_vlan_port_add(port, 0);
      /* Clear port bits */
      uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << port);
    }

    /* Clear lag ports at general bitmap */
    uplink_protection_ports_bmp &= ~lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE];
    /* Clear lag ports at active bitmap */
    uplink_protection_ports_active_bmp &= ~lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE];
    /* Clear lag bitmap */
    lag_uplink_protection_ports_bmp[lag_idx - PTIN_SYSTEM_PROTECTION_LAGID_BASE] = 0;
    
    /* Update LAG structures */
    CLEAR_LAG_CONF(lag_idx);

    PT_LOG_WARN(LOG_CTX_INTF, "Protection lag %u removed", lag_idx);
    return L7_SUCCESS;
  }
#endif

  /* Check if LAG really exists */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    PT_LOG_WARN(LOG_CTX_INTF, "LAG# %u does not exist", lag_idx);
    return L7_SUCCESS;
  }

  if (ptin_intf_lag2intIfNum(lag_idx, &lag_intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error acquiring intIfNum of lag_idx %u", lag_idx);
    return L7_FAILURE;
  }
  if (ptin_intf_port2ptintf(lag_port, &lag_ptin_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error acquiring ptin_intf from lag_port %u", lag_port);
    return L7_FAILURE;
  }

  /* Check if LAG is being used in any EVC */
  if (ptin_evc_is_intf_in_use(lag_port))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is being used in EVCs! Cannot be removed", lag_idx);
    return L7_FAILURE;
  }

  /* Remove bandwidth profiles applied to this port (for all 8 CoS) */
  for (i = 0; i < 8; i++)
  {
    if (ptin_QoS_intf_cos_policer_clear(&lag_ptin_intf, i) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: Error removing bandwidth profile of CoS %u", lag_idx, i);
    }
  }

  /* Remove LAG */
  if (usmDbDot3adRemoveSet(1, lag_intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: failed to remove this LAG", lag_idx);
    return L7_FAILURE;
  }

  /* Wait until lag exists */
  do {
    if ((usmDbIntfStatusGet(lag_intIfNum, &value) == L7_SUCCESS) && (value == L7_INTF_UNINITIALIZED))
      break;

    osapiSleepMSec(10);
  } while (1);

  /* For Linecards, LAG 1/0 belongs to backplane... should return to untrusted state */
  #if (PTIN_BOARD_IS_LINECARD)
  if (lag_idx == 0)
  #endif
  #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
  {
    ptin_dhcp_intfTrusted_set(lag_intIfNum, L7_FALSE); 
    ptin_pppoe_intfTrusted_set(lag_intIfNum, L7_FALSE);
    PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u returns to untrusted", lag_idx);
  }
  #endif

  /* Update PortGroups (used on egress translations) */
  ptin_pbmp = lagConf_data[lag_idx].members_pbmp64;
  for (i=0; i<ptin_sys_number_of_ports; i++, ptin_pbmp >>= 1)
  {
    if (ptin_pbmp & 1)
    {
      ptin_intf_port2intIfNum(i, &intIfNum);
      /* Reset phy#i port group */
      if (ptin_xlate_portgroup_set(intIfNum, PTIN_XLATE_PORTGROUP_INTERFACE) != L7_SUCCESS)
      {
        /* NOTE: if any error occurs during PortGroup set, it originates inconsistency on
         * the portgroups lookup table, which means a CRITICAL or even FATAL situation!
         * However, any error should occur ONLY during debug or validation process, and
         * not during normal operation (under production) */ 
        PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: could not update PortGroup for member port# %u", lag_idx, i);
      }

      /* To be removed */
      #if 0
      /* Uplink protection */
      #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
      if (lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
      {
        /* Disable linkscan */
        if (ptin_intf_linkscan_set(intIfNum, L7_ENABLE) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF,"Error reenabling linkscan for intIfNum %u", intIfNum);
        }
      }
      #endif
      #endif
    }
  }

  /* Clear PTin structs */
  CLEAR_LAG_CONF(lag_idx);
  CLEAR_LAG_MAP(lag_idx);

  /* Check if inBand EVC exists. If yes, redefine it.
   * This procedure is necessary because the new inBand EVC must include the
   * new LAG instead of respective physical ports (not used in LAGs until now)
   */ 
// TODO

  PT_LOG_TRACE(LOG_CTX_INTF, "LAG# %u: successfully deleted", lag_idx);

  return L7_SUCCESS;
}

/**
 * Deletes all LAGs
 * 
 * @param 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_Lag_delete_all(void)
{
  L7_uint32 lag_idx;

  for (lag_idx = 0; lag_idx < PTIN_SYSTEM_N_LAGS; lag_idx++)
  {
    if (!ptin_intf_lag_exists(lag_idx))
      continue;
    ptin_intf_Lag_delete(lag_idx);
  }
  return L7_SUCCESS;
}

/**
 * Gets a LAG status
 * 
 * @param lagStatus Pointer to the output structure (or array)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LagStatus_get(ptin_LACPLagStatus_t *lagStatus)
{
  L7_uint32 lag_idx;
  L7_uint32 lag_intf;
  L7_uint32 value;
  L7_uint64 members_pbmp;
  L7_uint32 members_list[PTIN_SYSTEM_N_PORTS]; /* Internal interface numbers of portChannel members */
  L7_uint   i, nElems;

  /* Validate arguments */
  if (lagStatus == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  lag_idx = lagStatus->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG really exists */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u does not exist", lag_idx);
    return L7_FAILURE;
  }

#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  /* Protection lag */
  if (lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
  {
    lagStatus->admin                  = L7_ENABLE;
    lagStatus->link_status            = (uplink_protection_ports_active_bmp != 0);
    lagStatus->port_channel_type      = 0;
    lagStatus->members_pbmp64         = uplink_protection_ports_bmp;
    lagStatus->active_members_pbmp64  = uplink_protection_ports_active_bmp;
  }
  else
#endif

  /* Normal LAGs */
  {
    if (ptin_intf_lag2intIfNum(lag_idx, &lag_intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error acquiring intIfNum of lag_idx %u", lag_idx);
      return L7_FAILURE;
    }

    /* LAG admin */
    lagStatus->admin = lagConf_data[lag_idx].admin;

    /* LAG Static Mode */
    lagStatus->port_channel_type = lagConf_data[lag_idx].static_enable;

    /* Link status */
    if (nimGetIntfLinkState(lag_intf, &value) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Link State", lag_idx);
      return L7_FAILURE;
    }
    lagStatus->link_status = (value == L7_UP);

    /* Get members list */
    lagStatus->members_pbmp64 = lagConf_data[lag_idx].members_pbmp64;

    /* Get list of active ports */
    nElems = sizeof(members_list) / sizeof(members_list[0]);
    if (usmDbDot3adActiveMemberListGet(1, lag_intf, &nElems, members_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Active Members List", lag_idx);
      return L7_FAILURE;
    }

    /* Convert it to a bitmap */
    members_pbmp = 0;
    for (i=0; i<nElems; i++)
    {
      /* Validate interface number */
      if ((members_list[i] == 0)
          || (ptin_intf_intIfNum2port(members_list[i], &value))
          || (value <  PTIN_SYSTEM_N_PONS)
          || (value >= ptin_sys_number_of_ports))
      {
        PT_LOG_CRITIC(LOG_CTX_INTF, "LAG# %u: invalid active members found (port# %u; intf# %u)", lag_idx, value, members_list[i]);
        continue;
      }
      members_pbmp |= (L7_uint64)1 << value;
    }
    lagStatus->active_members_pbmp64 = members_pbmp;
  }

  return L7_SUCCESS;
}

/**
 * Sets a Port LACP Admin State
 * 
 * @param adminState Pointer to the input structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LACPAdminState_set(ptin_LACPAdminState_t *adminState)
{
  L7_uint32 port_idx;
  L7_uint32 port_intf=0;
  L7_uchar8 actor_state, partner_state;

  /* Validate arguments */
  if (adminState == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  port_idx = adminState->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port_idx, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Get Current Admin State */
  if ((usmDbDot3adAggPortActorAdminStateGet(1, port_intf, &actor_state) != L7_SUCCESS) ||
      (usmDbDot3adAggPortPartnerAdminStateGet(1, port_intf, &partner_state) != L7_SUCCESS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u: failed to read actor/partner states (intf=%u)", port_idx, port_intf);
    return L7_FAILURE;
  }

  /* Update flags */
  if (adminState->state_aggregation) {
    actor_state   |=   (L7_uchar8) DOT3AD_STATE_AGGREGATION;
    partner_state |=   (L7_uchar8) DOT3AD_STATE_AGGREGATION;
  }
  else {
    actor_state   &= ~((L7_uchar8) DOT3AD_STATE_AGGREGATION);
    partner_state &= ~((L7_uchar8) DOT3AD_STATE_AGGREGATION);
  }

  if (adminState->lacp_activity) {
    actor_state   |=   (L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY;
    partner_state |=   (L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY;
  }
  else {
    actor_state   &= ~((L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY);
    partner_state &= ~((L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY);
  }

  if (adminState->lacp_timeout) {
    actor_state   |=   (L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT;
    partner_state |=   (L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT;
  }
  else {
    actor_state   &= ~((L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT);
    partner_state &= ~((L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT);
  }

  /* Apply new state */
  if ((usmDbDot3adAggPortActorAdminStateSet(1, port_intf, &actor_state) != L7_SUCCESS) ||
      (usmDbDot3adAggPortPartnerAdminStateSet(1, port_intf, &partner_state) != L7_SUCCESS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: failed to set actor/partner states", port_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Gets a Port LACP Admin State
 * 
 * @param adminState Pointer to the output structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LACPAdminState_get(ptin_LACPAdminState_t *adminState)
{
  L7_uint32 port_idx;
  L7_uint32 port_intf=0;
  L7_uchar8 actor_state;

  /* Validate arguments */
  if (adminState == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  port_idx = adminState->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port_idx, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Get Current Admin State */
  if (usmDbDot3adAggPortActorAdminStateGet(1, port_intf, &actor_state) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u: failed to read actor state (intf=%u)", port_idx, port_intf);
    return L7_FAILURE;
  }

  adminState->state_aggregation = (actor_state & DOT3AD_STATE_AGGREGATION)   ? L7_TRUE : L7_FALSE;
  adminState->lacp_activity     = (actor_state & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
  adminState->lacp_timeout      = (actor_state & DOT3AD_STATE_LACP_TIMEOUT)  ? L7_TRUE : L7_FALSE;

  return L7_SUCCESS;
}

/**
 * Gets a Port LACP statistics info
 * 
 * @param lagStats Pointer to the output structure (or array)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LACPStats_get(ptin_LACPStats_t *lagStats)
{
  L7_uint32 port_idx;
  L7_uint32 port_intf=0;
  L7_uint32 rxStats, txStats;

  /* Validate arguments */
  if (lagStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  port_idx = lagStats->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port_idx, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  if (ptin_intf_port2intIfNum(port_idx, &port_intf)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting intIfNum value from Port# %u", port_idx);
    return L7_FAILURE;
  }

  /* Read values */
  if (usmDbDot3adAggPortStatsLACPDUsRxGet(1, port_intf, &rxStats) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u: error reading LACPDUs Rx stats", port_idx);
    return L7_FAILURE;
  }

  if (usmDbDot3adAggPortStatsLACPDUsTxGet(1, port_intf, &txStats) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u: error reading LACPDUs Tx stats", port_idx);
    return L7_FAILURE;
  }

  lagStats->LACPdus_rx = rxStats;
  lagStats->LACPdus_tx = txStats;

  return L7_SUCCESS;
}

/**
 * Clears a Port LACP statistics info
 * 
 * @param lagStats Structure that references the Port# stats to clear
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_LACPStats_clear(ptin_LACPStats_t *lagStats)
{
  L7_uint32 port_idx;
  L7_uint32 port_intf=0;

  /* Validate arguments */
  if (lagStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  port_idx = lagStats->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port_idx, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Clear stats */
  if (usmDbDot3adPortStatsClear(1, port_intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u: error clearing LACP stats", port_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Read stormcontrol data for Broadcast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param burst_size  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_bcast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units)
{
  L7_uint32       intIfNum;
  L7_BOOL         enable_status;
  L7_uint32       rate_limit_status, burst_size_status;
  L7_RATE_UNIT_t  rate_units_status;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Reading Broadcast stormcontrol data...");

  /* Read current enable status */
  rc = usmDbSwDevCtrlBcastStormModeIntfGet(intIfNum, &enable_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Broadcast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(intIfNum, &rate_limit_status, &burst_size_status, &rate_units_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Broadcast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  /* Return data */
  if (enable     != L7_NULLPTR)  *enable     = enable_status;
  if (rate_limit != L7_NULLPTR)  *rate_limit = rate_limit_status;
  if (burst_size != L7_NULLPTR)  *burst_size = burst_size_status;
  if (rate_units != L7_NULLPTR)  *rate_units = rate_units_status;

  return L7_SUCCESS;
}

/**
 * Read stormcontrol data for Multicast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param burst_size  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_mcast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units)
{
  L7_uint32       intIfNum;
  L7_BOOL         enable_status;
  L7_uint32       rate_limit_status, burst_size_status;
  L7_RATE_UNIT_t  rate_units_status;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Reading Multicast stormcontrol data...");

  /* Read current enable status */
  rc = usmDbSwDevCtrlMcastStormModeIntfGet(intIfNum, &enable_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Multicast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(intIfNum, &rate_limit_status, &burst_size_status, &rate_units_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Multicast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  /* Return data */
  if (enable     != L7_NULLPTR)  *enable     = enable_status;
  if (rate_limit != L7_NULLPTR)  *rate_limit = rate_limit_status;
  if (burst_size != L7_NULLPTR)  *burst_size = burst_size_status;
  if (rate_units != L7_NULLPTR)  *rate_units = rate_units_status;

  return L7_SUCCESS;
}

/**
 * Read stormcontrol data for unknown Unicast traffic
 * 
 * @param ptin_intf
 * @param enable      (output)
 * @param rate_value  (output)
 * @param burst_size  (output)
 * @param rate_units  (output)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_ucast_stormControl_get(const ptin_intf_t *ptin_intf, L7_BOOL *enable, L7_uint32 *rate_limit, L7_uint32 *burst_size, L7_uint8 *rate_units)
{
  L7_uint32       intIfNum;
  L7_BOOL         enable_status;
  L7_uint32       rate_limit_status, burst_size_status;
  L7_RATE_UNIT_t  rate_units_status;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Reading Unicast stormcontrol data...");

  /* Read current enable status */
  rc = usmDbSwDevCtrlUcastStormModeIntfGet(intIfNum, &enable_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Unicast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(intIfNum, &rate_limit_status, &burst_size_status, &rate_units_status);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Unicast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  /* Return data */
  if (enable     != L7_NULLPTR)  *enable     = enable_status;
  if (rate_limit != L7_NULLPTR)  *rate_limit = rate_limit_status;
  if (burst_size != L7_NULLPTR)  *burst_size = burst_size_status;
  if (rate_units != L7_NULLPTR)  *rate_units = rate_units_status;

  return L7_SUCCESS;
}


static L7_RC_t ptin_intf_stormcontrol_reset(L7_uint32 intIfNum, L7_uint8 rate_units)
{
  L7_RC_t rc, rc_global = L7_SUCCESS;

  /* Disable current stormcontrol */
  rc = usmDbSwDevCtrlBcastStormModeIntfSet(intIfNum, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error disabling Broadcast StormControl - rc=%d", intIfNum, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Broadcast StormControl disabled", intIfNum);

  rc = usmDbSwDevCtrlMcastStormModeIntfSet(intIfNum, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error disabling Multicast StormControl - rc=%d", intIfNum, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Multicast StormControl disabled", intIfNum);

  rc = usmDbSwDevCtrlUcastStormModeIntfSet(intIfNum, L7_DISABLE);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error disabling Unicast StormControl - rc=%d", intIfNum, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Unicast StormControl disabled", intIfNum);

  /* Check result */
  if (rc_global != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error disabling StormControl - rc_global=%d", intIfNum, rc_global);
    return rc_global;
  }

  /* Redefine units */
  rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(intIfNum, 0, 0, rate_units);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error setting Broadcast threashold units to %u - rc=%d", intIfNum, rate_units, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Broadcast threashold units defined to %u", intIfNum, rate_units);

  rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(intIfNum, 0, 0, rate_units);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error setting Multicast threashold units to %u - rc=%d", intIfNum, rate_units, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Multicast threashold units defined to %u", intIfNum, rate_units);

  rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(intIfNum, 0, 0, rate_units);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error setting Unicast threashold units to %u - rc=%d", intIfNum, rate_units, rc);
    rc_global = rc;
  }
  PT_LOG_TRACE(LOG_CTX_MSG,"intIfNum=%u: Unicast threashold units defined to %u", intIfNum, rate_units);

  /* Check results */
  if (rc_global != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"intIfNum=%u: Error redefining rate units - rc_global=%d", intIfNum, rc_global);
    return rc_global;
  }

  return rc_global;
}

/**
 * Configure stormcontrol for Broadcast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_bcast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units)
{
  L7_uint32       intIfNum;
  L7_INTF_TYPES_t intfType;
  L7_BOOL         enable_curr;
  L7_uint32       rate_value_curr;
  L7_RATE_UNIT_t  rate_units_curr;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Get interface type */
  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error acquiring interface type for intIfNum %u - ptin_intf %u/%u", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Ignore LAG configurations */
  if (intfType == L7_LAG_INTF)
  {
    PT_LOG_WARN(LOG_CTX_MSG,"intIfNum %u - ptin_intf %u/%u - is a LAG... ignoring configuration!", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Configuring Broadcast stormcontrol...");

  /* Read current enable status */
  rc = usmDbSwDevCtrlBcastStormModeIntfGet(intIfNum, &enable_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Broadcast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(intIfNum, &rate_value_curr, L7_NULLPTR, &rate_units_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Broadcast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  if (enable)
  {
    if (rate_units == L7_RATE_UNIT_NONE || rate_units >= L7_RATE_UNIT_TOTAL)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", rate_units);
      return L7_FAILURE;
    }

    /* If units change, deactivate current limiter */
    if (rate_units != rate_units_curr)
    {
      enable_curr = L7_DISABLE;

      /* Reset stormcontrol to apply same units to all traffic types */
      rc = ptin_intf_stormcontrol_reset(intIfNum, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"ptin_intf %u/%u: Error resetting StormControl to units %u - rc=%d",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units, rc);
        return rc;
      }
      PT_LOG_TRACE(LOG_CTX_MSG,"ptin_intf %u/%u: StormControl resetted to units %u",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units);
    }

    PT_LOG_TRACE(LOG_CTX_MSG,"Going to apply BC threshold of %u (units=%u)", rate_value, rate_units);

    /* Redefine threshold/units */
    rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(intIfNum, rate_value, rate_burst, rate_units);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Broadcast threashold to %u for ptin_intf %u/%u", rate_value,
              ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }
  /* If enable status changed, apply it */
  if (enable != enable_curr)
  {
    /* Read current enable status */
    rc = usmDbSwDevCtrlBcastStormModeIntfSet(intIfNum, enable);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Broadcast enable to %u for ptin_intf %u/%u", enable, ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Success applying Broadcast stormcontrol to ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);

  return L7_SUCCESS;
}

/**
 * Configure stormcontrol for Multicast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_mcast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units)
{
  L7_uint32       intIfNum;
  L7_INTF_TYPES_t intfType;
  L7_BOOL         enable_curr;
  L7_uint32       rate_value_curr;
  L7_RATE_UNIT_t  rate_units_curr;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Get interface type */
  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error acquiring interface type for intIfNum %u - ptin_intf %u/%u", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Ignore LAG configurations */
  if (intfType == L7_LAG_INTF)
  {
    PT_LOG_WARN(LOG_CTX_MSG,"intIfNum %u - ptin_intf %u/%u - is a LAG... ignoring configuration!", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Configuring Multicast stormcontrol for ptin_intf %u/%u, intIfNum %u...", ptin_intf->intf_type, ptin_intf->intf_id, intIfNum);

  /* Read current enable status */
  rc = usmDbSwDevCtrlMcastStormModeIntfGet(intIfNum, &enable_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Multicast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(intIfNum, &rate_value_curr, L7_NULLPTR, &rate_units_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Multicast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  if (enable)
  {
    if (rate_units == L7_RATE_UNIT_NONE || rate_units >= L7_RATE_UNIT_TOTAL)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", rate_units);
      return L7_FAILURE;
    }

    /* If units change, deactivate current limiter */
    if (rate_units != rate_units_curr)
    {
      enable_curr = L7_DISABLE;

      /* Reset stormcontrol to apply same units to all traffic types */
      rc = ptin_intf_stormcontrol_reset(intIfNum, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"ptin_intf %u/%u: Error resetting StormControl to units %u - rc=%d",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units, rc);
        return rc;
      }
      PT_LOG_TRACE(LOG_CTX_MSG,"ptin_intf %u/%u: StormControl resetted to units %u",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units);
    }

    PT_LOG_TRACE(LOG_CTX_MSG,"Going to apply BC threshold of %u (units=%u)", rate_value, rate_units);

    /* Redefine threshold/units */
    rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(intIfNum, rate_value, rate_burst, rate_units);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Multicast threashold to %u for ptin_intf %u/%u", rate_value,
              ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }
  /* If enable status changed, apply it */
  if (enable != enable_curr)
  {
    /* Read current enable status */
    rc = usmDbSwDevCtrlMcastStormModeIntfSet(intIfNum, enable);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Multicast enable to %u for ptin_intf %u/%u", enable, ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Success applying Multicast stormcontrol to ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);

  return L7_SUCCESS;
}


/**
 * Configure stormcontrol for unknown Unicast traffic
 * 
 * @param ptin_intf
 * @param enable
 * @param rate_value 
 * @param rate_burst 
 * @param rate_units 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_ucast_stormControl_set(const ptin_intf_t *ptin_intf, L7_BOOL enable, L7_uint32 rate_value, L7_uint32 rate_burst, L7_uint8 rate_units)
{
  L7_uint32       intIfNum;
  L7_INTF_TYPES_t intfType;
  L7_BOOL         enable_curr;
  L7_uint32       rate_value_curr;
  L7_RATE_UNIT_t  rate_units_curr;
  L7_RC_t         rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null pointer");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Get interface type */
  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error acquiring interface type for intIfNum %u - ptin_intf %u/%u", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  /* Ignore LAG configurations */
  if (intfType == L7_LAG_INTF)
  {
    PT_LOG_WARN(LOG_CTX_MSG,"intIfNum %u - ptin_intf %u/%u - is a LAG... ignoring configuration!", intIfNum, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Configuring Unicast stormcontrol...");

  /* Read current enable status */
  rc = usmDbSwDevCtrlUcastStormModeIntfGet(intIfNum, &enable_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Unicast enable status from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }
  /* Read current threshold data */
  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(intIfNum, &rate_value_curr, L7_NULLPTR, &rate_units_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading Unicast threshold data from ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return rc;
  }

  if (enable)
  {
    if (rate_units == L7_RATE_UNIT_NONE || rate_units >= L7_RATE_UNIT_TOTAL)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", rate_units);
      return L7_FAILURE;
    }

    /* If units change, deactivate current limiter */
    if (rate_units != rate_units_curr)
    {
      enable_curr = L7_DISABLE;

      /* Reset stormcontrol to apply same units to all traffic types */
      rc = ptin_intf_stormcontrol_reset(intIfNum, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"ptin_intf %u/%u: Error resetting StormControl to units %u - rc=%d",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units, rc);
        return rc;
      }
      PT_LOG_TRACE(LOG_CTX_MSG,"ptin_intf %u/%u: StormControl resetted to units %u",
                ptin_intf->intf_type,ptin_intf->intf_id, rate_units);
    }

    PT_LOG_TRACE(LOG_CTX_MSG,"Going to apply BC threshold of %u (units=%u)", rate_value, rate_units);

    /* Redefine threshold/units */
    rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(intIfNum, rate_value, rate_burst, rate_units);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Unicast threashold to %u for ptin_intf %u/%u", rate_value,
              ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }
  /* If enable status changed, apply it */
  if (enable != enable_curr)
  {
    /* Read current enable status */
    rc = usmDbSwDevCtrlUcastStormModeIntfSet(intIfNum, enable);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting Unicast enable to %u for ptin_intf %u/%u", enable, ptin_intf->intf_type, ptin_intf->intf_id);
      return rc;
    }
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Success applying Unicast stormcontrol to ptin_intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);

  return L7_SUCCESS;
}

/**
 * Apply a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_intf 
 * @param cos 
 * @param meter 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_QoS_intf_cos_policer_set(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_bw_meter_t *meter)
{
  L7_uint32         ptin_port;
  ptin_bw_profile_t profile;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Null pointer");
    return L7_FAILURE;
  }
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_intf=%u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  if (cos >= L7_COS_INTF_QUEUE_MAX_COUNT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid COS %u", cos);
    return L7_FAILURE;
  }

  memset(&profile, 0x00, sizeof(profile));

  profile.ptin_port = ptin_port;
  profile.cos       = cos;

  /* Apply policer */
  if ((rc = ptin_bwPolicer_set(&profile, meter, -1)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Remove a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_intf 
 * @param cos 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_QoS_intf_cos_policer_clear(const ptin_intf_t *ptin_intf, L7_uint8 cos)
{
  L7_uint32         ptin_port;
  ptin_bw_profile_t profile;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Null pointer");
    return L7_FAILURE;
  }
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_intf=%u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  if (cos >= L7_COS_INTF_QUEUE_MAX_COUNT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid COS %u", cos);
    return L7_FAILURE;
  }

  memset(&profile, 0x00, sizeof(profile));

  profile.ptin_port = ptin_port;
  profile.cos       = cos;

  /* Apply policer */
  if ((rc = ptin_bwPolicer_delete(&profile)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Configures interface properties for QoS
 * 
 * @param intf : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_intf_config_set(const ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2, cos;
  L7_uint32 intIfNum;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || intfQos == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Interface = %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  PT_LOG_TRACE(LOG_CTX_INTF,"Mask         = 0x%02x",intfQos->mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"TrustMode    = %u",intfQos->trust_mode);
  PT_LOG_TRACE(LOG_CTX_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  PT_LOG_TRACE(LOG_CTX_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  PT_LOG_TRACE(LOG_CTX_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.mask   =0x%02x",intfQos->pktprio.mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
            intfQos->pktprio.cos[0],
            intfQos->pktprio.cos[1],
            intfQos->pktprio.cos[2],
            intfQos->pktprio.cos[3],
            intfQos->pktprio.cos[4],
            intfQos->pktprio.cos[5],
            intfQos->pktprio.cos[6],
            intfQos->pktprio.cos[7]);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Is there any configuration to be applied? */
  if (intfQos->mask==0x00)
  {
    PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
    return L7_SUCCESS;
  }

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  // Get Trust mode
  rc = usmDbQosCosMapTrustModeGet(1, intIfNum, &trust_mode);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error with usmDbQosCosMapTrustModeGet (rc=%d)", rc);
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc_global = rc;
  }
  // Validate trust mode
  else if (trust_mode==L7_NULL || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid trust mode (%u)",trust_mode);
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc_global = L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Current trust mode is %u",trust_mode);

  /* Set Trust mode */
  if (intfQos->mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
  {
    // Define trust mode
    rc = usmDbQosCosMapTrustModeSet(1,intIfNum,intfQos->trust_mode);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapTrustModeSet (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      // Configuration successfull => change trust mode value
      trust_mode = intfQos->trust_mode;
      PT_LOG_TRACE(LOG_CTX_INTF, "New trust mode is %u",trust_mode);
    }
  }
  /* Bandwidth units */
  if (intfQos->mask & PTIN_QOS_INTF_BANDWIDTHUNIT_MASK)
  {
    /* Do nothing */
    PT_LOG_WARN(LOG_CTX_INTF, "Bandwidth units were not changed");
  }
  /* Shaping rate */
  if (intfQos->mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
  {
    rc = usmDbQosCosQueueIntfShapingRateSet(1,intIfNum,intfQos->shaping_rate);
    if (rc != L7_SUCCESS)
    {  
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "New shaping rate is %u",intfQos->shaping_rate);
    }
  }
  /* WRED decay exponent */
  if (intfQos->mask & PTIN_QOS_INTF_WRED_DECAY_EXP_MASK)
  {
    rc = usmDbQosCosQueueWredDecayExponentSet(1,intIfNum,intfQos->wred_decay_exponent);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueWredDecayExponentSet (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "New WRED Decay exponent is %u",intfQos->wred_decay_exponent);
    }
  }
  /* Packet priority mask */
  if (intfQos->mask & PTIN_QOS_INTF_PACKETPRIO_MASK &&
      trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
  {
    /* Run all priorities */
    for (prio=0; prio<8; prio++)
    {
      /* If priority mask active, attribute cos */
      if (intfQos->pktprio.mask[prio] == 0)  continue;
      
      // 802.1p trust mode
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        rc = usmDbDot1dTrafficClassSet(1,intIfNum,prio,cos);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbDot1dTrafficClassSet (prio=%u => cos=%u): rc=%d",prio,cos, rc);
          rc_global = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        rc = usmDbQosCosMapIpPrecTrafficClassSet(1, intIfNum, prio, cos);
        if (rc != L7_SUCCESS)
        { 
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassSet (IPprec=%u => cos=%u): rc=%d",prio,cos, rc); 
          rc_global = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          if ( !((intfQos->pktprio.mask[prio]>>prio2) & 1) )  continue;

          /* Map 64 different priorities (6 bits) to 8 CoS */
          cos = ((intfQos->pktprio.cos[prio])>>(4*prio2)) & 0x07;

          rc = usmDbQosCosMapIpDscpTrafficClassSet(1, intIfNum, prio*8+prio2, cos);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassSet (DscpPrio=%u => CoS=%u): rc=%d",prio*8+prio2,cos, rc);
            rc_global = rc;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc_global = L7_FAILURE;
      }
    }
  }

  if (rc_global==L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully applied to ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS configuration to ptin_intf=%u/%u (rc_global=%d)",ptin_intf->intf_type,ptin_intf->intf_id, rc_global);
  }

  return rc_global;
}

/**
 * Read interface properties for QoS
 * 
 * @param intf : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_intf_config_get(const ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2;
  L7_uint32 intIfNum, value, cos;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || intfQos == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Clear configurations to be returned */
  memset(intfQos,0x00,sizeof(ptin_QoS_intf_t));

  PT_LOG_TRACE(LOG_CTX_INTF,"Interface = %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  // Get Trust mode
  rc = usmDbQosCosMapTrustModeGet(1, intIfNum, &value);
  if (rc != L7_SUCCESS)
  {
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    PT_LOG_ERR(LOG_CTX_INTF,"Error with usmDbQosCosMapTrustModeGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    trust_mode = value;
    intfQos->trust_mode = (L7_uint8) value;
    intfQos->mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
  }

  /* Get units */
  intfQos->bandwidth_unit = (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_PERCENT) ? 0 : 1;
  intfQos->mask |= PTIN_QOS_INTF_BANDWIDTHUNIT_MASK;

  /* Shaping rate */
  rc = usmDbQosCosQueueIntfShapingRateGet(1,intIfNum,&value);
  if (rc != L7_SUCCESS)
  {  
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    intfQos->shaping_rate = value;
    intfQos->mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
  }

  /* WRED decay exponent */
  rc = usmDbQosCosQueueWredDecayExponentGet(1,intIfNum,&value);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueWredDecayExponentGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    intfQos->wred_decay_exponent = value;
    intfQos->mask |= PTIN_QOS_INTF_WRED_DECAY_EXP_MASK;
  }

  /* Only for non untrusted mode, we have priority map */
  if (trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
  {
    /* Run all priorities */
    for (prio=0; prio<8; prio++)
    {
      // 802.1p trust mode
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        rc = usmDbDot1dTrafficClassGet(1,intIfNum,prio,&cos);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbDot1dTrafficClassGet (prio=%u) (rc=%d)",prio, rc);
          rc_global = rc;
        }
        else
        {
          intfQos->pktprio.mask[prio] = 1;
          intfQos->pktprio.cos[prio]  = cos;
          PT_LOG_TRACE(LOG_CTX_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        rc = usmDbQosCosMapIpPrecTrafficClassGet(1, intIfNum, prio, &cos);
        if (rc != L7_SUCCESS)
        { 
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassGet (IPprec=%u) (rc=%d)",prio, rc);
          rc_global = rc;
        }
        else
        {
          intfQos->pktprio.mask[prio] = 1;
          intfQos->pktprio.cos[prio]  = cos;
          PT_LOG_TRACE(LOG_CTX_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          rc = usmDbQosCosMapIpDscpTrafficClassGet(1, intIfNum, prio*8+prio2, &cos);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassGet (DscpPrio=%u) (rc=%d)",prio*8+prio2, rc);
            rc_global = rc;
            break;
          }
          else
          {
            intfQos->pktprio.mask[prio] |= (L7_uint8) 1 << prio2;
            intfQos->pktprio.cos[prio]  |= ((L7_uint32) cos & 0x0f)<<(prio2*4);
            PT_LOG_TRACE(LOG_CTX_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc_global = L7_FAILURE;
      }
    }

    /* Packet priority mask */
    if (intfQos->pktprio.mask)
    {
      intfQos->mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Mask         = 0x%02x",intfQos->mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"TrustMode    = %u",intfQos->trust_mode);
  PT_LOG_TRACE(LOG_CTX_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  PT_LOG_TRACE(LOG_CTX_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  PT_LOG_TRACE(LOG_CTX_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.mask   ={0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x}",
            intfQos->pktprio.mask[0],intfQos->pktprio.mask[1],intfQos->pktprio.mask[2],intfQos->pktprio.mask[3],intfQos->pktprio.mask[4],intfQos->pktprio.mask[5],intfQos->pktprio.mask[6],intfQos->pktprio.mask[7]);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
            intfQos->pktprio.cos[0],
            intfQos->pktprio.cos[1],
            intfQos->pktprio.cos[2],
            intfQos->pktprio.cos[3],
            intfQos->pktprio.cos[4],
            intfQos->pktprio.cos[5],
            intfQos->pktprio.cos[6],
            intfQos->pktprio.cos[7]);

  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully read from ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error reading QoS configuration from ptin_intf=%u/%u (rc_global=%d)",ptin_intf->intf_type,ptin_intf->intf_id, rc_global);
  }

  return rc_global;
}


/**
 * Configures a class of service for QoS
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_cos_config_set(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, i, conf_index;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueWeightList_t    schedWeight_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || qosConf == L7_NULLPTR || (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get current configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading scheduler type (rc=%d)", rc);
    return rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading minimum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading maximum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Weights list */
  rc = usmDbQosCosQueueWeightListGet(1, intIfNum, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading weights list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Is there any configuration to be applied? */
    if (qosConf[conf_index].mask==0x00)
    {
      //PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
      continue;
    }

    /* Scheduler type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_SCHEDULER_MASK)
    {
      schedType_list.schedType[i] = qosConf[conf_index].scheduler_type;
      PT_LOG_TRACE(LOG_CTX_INTF,"Scheduler type in cos=%u, will be updated to %u",i,qosConf[conf_index].scheduler_type);
    }
    /* Minimum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MIN_MASK)
    {
      minBw_list.bandwidth[i] = qosConf[conf_index].min_bandwidth;
      PT_LOG_TRACE(LOG_CTX_INTF,"Minimum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].min_bandwidth);
    }
    /* Maximum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MAX_MASK)
    {
      maxBw_list.bandwidth[i] = qosConf[conf_index].max_bandwidth;
      PT_LOG_TRACE(LOG_CTX_INTF,"Maximum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].max_bandwidth);
    }
    /* Scheduler type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_WRR_WEIGHT_MASK)
    {
      schedWeight_list.queue_weight[i] = qosConf[conf_index].wrrSched_weight;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRR weight for cos=%u, will be updated to %u",i,qosConf[conf_index].wrrSched_weight);
    }
  }

  /* Apply new configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListSet(1, intIfNum, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying scheduler type (rc=%d)", rc);
    rc_global = rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListSet(1, intIfNum, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying minimum bandwith (rc=%d)", rc);
    rc_global = rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListSet(1, intIfNum, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying maximum bandwith (rc=%d)", rc);
    rc_global = rc;
  }
  /* WRR weights */
  rc = usmDbQosCosQueueWeightListSet(1, intIfNum, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying WRR weights (rc=%d)", rc);
    rc_global = rc;
  }

  /* Check result */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully applied to ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS configuration to ptin_intf=%u/%u, cos=%u (rc_global=%d)",ptin_intf->intf_type,ptin_intf->intf_id,cos, rc_global);
  }

  return rc_global;
}

/**
 * Reads a class of service QoS configuration
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_cos_config_get(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i;
  L7_RC_t   rc = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueWeightList_t    schedWeight_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || qosConf == L7_NULLPTR || (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Interface=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading scheduler type (rc=%d)", rc);
    return rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading minimum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading maximum bandwith (rc=%d)", rc);
    return rc;
  }
  /* WRR weights */
  rc = usmDbQosCosQueueWeightListGet(1, intIfNum, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading WRR weights (rc=%d)", rc);
    return rc;
  }

  /* Copy returned data to output */
  for (i=0; i<8; i++)
  {
    if ( cos == (L7_uint8)-1 )
    {
      conf_index = i;
    }
    else if (cos==i)
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Clear output structure */
    memset(&qosConf[conf_index],0x00,sizeof(ptin_QoS_cos_t));

    qosConf[conf_index].scheduler_type = (L7_uint8) schedType_list.schedType[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_SCHEDULER_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Scheduler type for cos=%u is %u",i,qosConf[conf_index].scheduler_type);

    qosConf[conf_index].min_bandwidth = minBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MIN_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Minimum bandwith for cos=%u is %u",i,qosConf[conf_index].min_bandwidth);

    qosConf[conf_index].max_bandwidth = maxBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MAX_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Maximum bandwith for cos=%u is %u",i,qosConf[conf_index].max_bandwidth);

    qosConf[conf_index].wrrSched_weight = schedWeight_list.queue_weight[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRR_WEIGHT_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"WRR weight for cos=%u is %u",i,qosConf[conf_index].wrrSched_weight);
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully read from ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  return L7_SUCCESS;
}

/**
 * Configures a class of service for QoS
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_drop_config_set(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_drop_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i, j;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosDropParmsList_t      dropParams_list;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || qosConf == L7_NULLPTR || (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Interface=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading mgmtType type (rc=%d)", rc);
    return rc;
  }

  /* Get drop params configurations */
  rc = usmDbQosCosQueueDropParmsListGet(1, intIfNum, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading dropParams list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Is there any configuration to be applied? */
    if (qosConf[conf_index].mask==0x00)
    {
      //PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
      continue;
    }

    /* Drop Management type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_QUEUE_MANGM_MASK)
    {
      dropParams_list.queue[i].mgmtType = qosConf[conf_index].queue_management_type + 1;
      PT_LOG_TRACE(LOG_CTX_INTF,"Mgmt type in cos=%u, will be updated to %u",i,qosConf[conf_index].queue_management_type);
    }
    /* WRED decay exponent */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_WRED_DECAY_EXP_MASK)
    {
      dropParams_list.queue[i].wred_decayExponent = qosConf[conf_index].wred_decayExp;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED decay Exp. in cos=%u, will be updated to %u",i,qosConf[conf_index].wred_decayExp);
    }

    /* Thresholds */
    if ((qosConf[conf_index].mask & PTIN_QOS_COS_WRED_THRESHOLDS_MASK) || (qosConf[conf_index].mask & PTIN_QOS_COS_TAIL_THRESHOLDS_MASK))
    {
      /* Run all DP levels */
      for (j = 0; j < 4; j++)
      {
        if (qosConf[conf_index].dp[j].local_mask == 0)  continue;

        /* Taildrop threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK)
        {
          dropParams_list.queue[i].tailDropMaxThreshold[j] = qosConf[conf_index].dp[j].taildrop_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"Taildrop threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].taildrop_threshold);
        }
        /* WRED min threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK)
        {
          dropParams_list.queue[i].minThreshold[j] = qosConf[conf_index].dp[j].wred_min_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED min threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_min_threshold);
        }
        /* WRED max threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK)
        {
          dropParams_list.queue[i].wredMaxThreshold[j] = qosConf[conf_index].dp[j].wred_max_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED max threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_max_threshold);
        }
        /* WRED drop probability */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK)
        {
          dropParams_list.queue[i].dropProb[j] = qosConf[conf_index].dp[j].wred_drop_prob;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED drop probability for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_drop_prob);
        }
      }
    }
  }

  /* Apply new configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListSet(1, intIfNum, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error setting new mgmtType list (rc=%d)", rc);
    rc_global = rc;
  }

  /* Drop params list */
  rc = usmDbQosCosQueueDropParmsListSet(1, intIfNum, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error setting new dropParams list (rc=%d)", rc);
    rc_global = rc;
  }

  /* Check result */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully applied to ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS drop configuration to ptin_intf=%u/%u, cos=%u (rc_global=%d)",ptin_intf->intf_type,ptin_intf->intf_id,cos, rc_global);
  }

  return rc_global;
}

/**
 * Reads a class of service QoS configuration
 * 
 * @param intf : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_drop_config_get(const ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_drop_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i, j;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosDropParmsList_t      dropParams_list;
  L7_RC_t rc;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR || qosConf == L7_NULLPTR || (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Interface=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading mgmtType type (rc=%d)", rc);
    return rc;
  }

  /* Get drop params configurations */
  rc = usmDbQosCosQueueDropParmsListGet(1, intIfNum, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading dropParams list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    memset(&qosConf[conf_index], 0x00, sizeof(ptin_QoS_drop_t));

    /* Drop Management type */
    qosConf[conf_index].queue_management_type = dropParams_list.queue[i].mgmtType - 1;
    qosConf[conf_index].mask |= PTIN_QOS_COS_QUEUE_MANGM_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Mgmt type at cos=%u is %u",i,qosConf[conf_index].queue_management_type);

    /* WRED decay exponent */
    qosConf[conf_index].wred_decayExp = dropParams_list.queue[i].wred_decayExponent;
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRED_DECAY_EXP_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"WRED decay Exp. at cos=%u is %u",i,qosConf[conf_index].wred_decayExp);

    /* Run all DP levels */
    for (j = 0; j < 4; j++)
    {
      /* Taildrop threshold */
      qosConf[conf_index].dp[j].taildrop_threshold = dropParams_list.queue[i].tailDropMaxThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"Taildrop threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].taildrop_threshold);

      /* WRED min threshold */
      qosConf[conf_index].dp[j].wred_min_threshold = dropParams_list.queue[i].minThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED min threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_min_threshold);

      /* WRED max threshold */
      qosConf[conf_index].dp[j].wred_max_threshold = dropParams_list.queue[i].wredMaxThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED max threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_max_threshold);

      /* WRED drop probability */
      qosConf[conf_index].dp[j].wred_drop_prob = dropParams_list.queue[i].dropProb[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED drop probability for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_drop_prob);
    }
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully applied to ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  return L7_SUCCESS;
}

/****************************************************************************** 
 * STATIC FUNCTIONS
 ******************************************************************************/

/**
 * Get physical interface configuration (read values from switch)
 * 
 * @param phyConf Structure to save port configuration (Port member 
 * must be set with the respective port; mask is ignored, but updated!)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_intf_PhyConfig_read(ptin_HWEthPhyConf_t *phyConf)
{
  L7_uint32 port;
  L7_uint32 value;
  L7_uint32 intIfNum = 0;
  L7_uint32 speed_mode/*, autoneg*/;

  /* Validate arguments */
  if (phyConf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  port = phyConf->Port;
  phyConf->Mask = 0;  /* Clear Mask */

  /* Validate port range */
  if (port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Port# %u is out of range [0..%u]", port, ptin_sys_number_of_ports-1);
    return L7_FAILURE;
  }

  /* Get interface ID */
  ptin_intf_port2intIfNum(port, &intIfNum);
  PT_LOG_TRACE(LOG_CTX_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* PortEnable */
  if (usmDbIfAdminStateGet(1, intIfNum, &value))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get enable state of port# %d", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0020;
    phyConf->PortEnable = value;

  #if ( PTIN_BOARD_IS_STANDALONE )
    /* Update shared memory */
    pfw_shm->intf[port].admin = value & 1;
  #endif
    PT_LOG_TRACE(LOG_CTX_INTF, " State:       %s", phyConf->PortEnable ? "Enabled":"Disabled");
  }

  /* MaxFrame */
  if (usmDbIfConfigMaxFrameSizeGet(intIfNum, &value))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get max frame value of port# %u", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0040;
    phyConf->MaxFrame = value;
    PT_LOG_TRACE(LOG_CTX_INTF, " MaxFrame:    %u", phyConf->MaxFrame);
  }

  /* Loopback */
  if (usmDbIfLoopbackModeGet(intIfNum, &value))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get loopback state of port# %u", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0008;
    phyConf->LoopBack = value;
    PT_LOG_TRACE(LOG_CTX_INTF, " Loopback:    %s", phyConf->LoopBack ? "Enabled":"Disabled");
  }

  /* Speed */
  if (usmDbIfDefaultSpeedGet(1, intIfNum, &speed_mode))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Failed to get speed of port# %u", port);
    return L7_FAILURE;
  }
  else {
    phyConf->Mask |= 0x0001;

    switch (speed_mode)
    {
      case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        phyConf->Speed = PHY_PORT_1000AN_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       Auto-Neg");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        phyConf->Speed = PHY_PORT_100_MBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       100Mbps");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        #if 0
        if (usmDbIfAutoNegAdminStatusGet(1, intIfNum, &autoneg)!=L7_SUCCESS)
        {
          phyConf->Speed = PHY_PORT_1000_MBPS;
        }
        else
        {
          phyConf->Speed = PHY_PORT_1000AN_GBPS;
        }
        #endif
        phyConf->Speed = PHY_PORT_1000_MBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       1000Mbps");
        break;

      /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        phyConf->Speed = PHY_PORT_2500_MBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       2.5G");
        break;

      /* PTin added: Speed 10G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        phyConf->Speed = PHY_PORT_10_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       10G");
        break;

      /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
        phyConf->Speed = PHY_PORT_40_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:       40G");
        break;

      /* PTin added: Speed 100G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
        phyConf->Speed = PHY_PORT_100_GBPS;
        PT_LOG_TRACE(LOG_CTX_INTF, " Speed:      100G");
        break;

      /* PTin end */

      default:
        phyConf->Speed = -1;
    }
  }

  /* Full-Duplex */
  phyConf->Mask |= 0x0004;
  phyConf->Duplex = 1;
  PT_LOG_TRACE(LOG_CTX_INTF, " Full-Duplex: %s", phyConf->Duplex?"Enabled":"Disabled");

  /* Media */
  /* NOTE: it is assumed that the PON ports are mapped from port 0 to L7_SYSTEM_PON_PORTS-1 */
  phyConf->Mask |= 0x0002;
  if ( port < PTIN_SYSTEM_N_PONS || port >= PTIN_SYSTEM_N_ETH )
  {
    phyConf->Media = PHY_PORT_MEDIA_INTERNAL;
    PT_LOG_TRACE(LOG_CTX_INTF, " Media:       Internal");
  }
  else
  {
    phyConf->Media = PHY_PORT_MEDIA_OPTICAL;
    PT_LOG_TRACE(LOG_CTX_INTF, " Media:       Optical");
  }

  return L7_SUCCESS;
}

/**
 * Apply default QoS configurations to provided interface
 * 
 * @param ptin_intf : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_intf_QoS_init(ptin_intf_t *ptin_intf)
{
  L7_int          i;
  ptin_QoS_intf_t qos_intf_cfg;
  ptin_QoS_cos_t  qos_cos_cfg;
  L7_RC_t         rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Define default QoS configuration */
  memset(&qos_intf_cfg,0x00,sizeof(ptin_QoS_intf_t));
  qos_intf_cfg.mask         = PTIN_QOS_INTF_TRUSTMODE_MASK | PTIN_QOS_INTF_PACKETPRIO_MASK;
  qos_intf_cfg.trust_mode   = L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
  
  /* Linear pbit->cos mapping */
  for (i=0; i<8; i++)
  {
    qos_intf_cfg.pktprio.mask[i] = PTIN_QOS_INTF_PACKETPRIO_COS_MASK;
    qos_intf_cfg.pktprio.cos[i]  = i;
  }
  /* Strict scheduler */
  memset(&qos_cos_cfg,0x00,sizeof(ptin_QoS_cos_t));
  qos_cos_cfg.mask           = PTIN_QOS_COS_SCHEDULER_MASK;
  qos_cos_cfg.scheduler_type = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;

  /* Apply configurations to interface */
  if (ptin_QoS_intf_config_set(ptin_intf, &qos_intf_cfg)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Intf %u/%u: failed QoS initialization of interface", ptin_intf->intf_type,ptin_intf->intf_id);
    rc = L7_FAILURE;
  }
  /* Apply configurations to CoS */
  for (i=0; i<8; i++)
  {
    if (ptin_QoS_cos_config_set (ptin_intf, i, &qos_cos_cfg)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Intf %u/%u: failed QoS initialization of CoS=%u", ptin_intf->intf_type,ptin_intf->intf_id, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}


/**
 * Activate PRBS generator/checker
 *  
 * @param intIfNum : Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pcs_prbs_enable(L7_uint32 intIfNum, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  dapiCmd.cmdData.prbsStatus.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.prbsStatus.enable   = enable;

  rc=dtlPtinPcsPrbs(intIfNum, &dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error setting PRBS enable of intIfNum %u to %u",intIfNum, enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Success applying global enable of intIfNum %u to %u",intIfNum,enable);

  return L7_SUCCESS;
}

/**
 * Read number of PRBS errors
 *  
 * @param intIfNum : Interface
 * @param enable   : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pcs_prbs_errors_get(L7_uint32 intIfNum, L7_uint32 *counter)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  dapiCmd.cmdData.prbsStatus.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.prbsStatus.enable   = 0;
  dapiCmd.cmdData.prbsStatus.rxErrors = (L7_uint32)-1;

  rc=dtlPtinPcsPrbs(intIfNum, &dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error getting PRBS errors of intIfNum %u",intIfNum);
    return rc;
  }

  /* Return result */
  if (counter!=L7_NULLPTR)
  {
    *counter = dapiCmd.cmdData.prbsStatus.rxErrors;
  }

  return L7_SUCCESS;
}

/**
 * Enable or disable linkscan control for a particular port
 * 
 * @param port 
 * @param enable 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_linkscan_control(L7_uint port, L7_BOOL enable)
{
  L7_RC_t rc = L7_SUCCESS;

  #ifdef PTIN_LINKSCAN_CONTROL
  #if (PTIN_BOARD_IS_MATRIX)
  #ifdef MAP_CPLD
  L7_uint16 board_id;
  L7_uint32 intIfNum;

  if (ptin_intf_port2intIfNum(port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting intIfNum from port %u", port);
    return L7_FAILURE;
  }

  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Get board id for this interface */
  if (ptin_intf_boardid_get(port, &board_id) != L7_SUCCESS)
  {
    board_id = 0;
  }

  do
  {
    /* Enable linkscan control: */
    if (enable) 
    {
      /* For uplink ports, enable linkscan (only for active matrix) */
      if (ptin_fpga_mx_is_matrixactive() && PTIN_BOARD_IS_UPLINK(board_id))
      {
        /* Disable force link-up */
        if ((rc=ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Uplink port %u: Error disabling force link-up", port);
          break;
        }
        /* Force link-down */
        if ((rc=ptin_intf_link_force(intIfNum, L7_FALSE, 0)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Uplink port %u: Error forcing link-down", port);
          break;
        }
        /* Enable linkscan */
        if ((rc=ptin_intf_linkscan_set(intIfNum, L7_ENABLE)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Uplink port %u: Error enabling linkscan", port);
          break;
        }
      }
      /* For other ports disable linkscan */
      else
      {
        /* Disable linkscan */
        if ((rc=ptin_intf_linkscan_set(intIfNum, L7_DISABLE)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Board port %u: Error disabling linkscan", port);
          break;
        }
        /* Disable force link-up */
        if ((rc=ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Board port %u: Error disabling force link-up", port);
          break;
        }
        /* Force link-down */
        if ((rc=ptin_intf_link_force(intIfNum, L7_FALSE, 0)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Board port %u: Error forcing link-down", port);
          break;
        }

        /* If a card is present, force link-up */
        if (PTIN_BOARD_IS_PRESENT(board_id))
        {
          /* Force link-up */
          if ((rc=ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE)) != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Board port %u: Error enabling force link-up", port);
            break;
          }
        }
      }
    }
    /* Disable linkscan control: */
    else
    {
      /* Disable force link-up */
      if ((rc=ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Port %u: Error disabling link-up force", port);
        break;
      }
      /* Force link-down */
      if ((rc=ptin_intf_link_force(intIfNum, L7_FALSE, 0)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Port %u: Error forcing link-down force", port);
        break;
      }
      /* Enable linkscan */
      if ((rc=ptin_intf_linkscan_set(intIfNum, L7_ENABLE)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Port %u: Error enabling linkscan", port);
        break;
      }
    }
  } while (0);

  osapiSemaGive(ptin_boardaction_sem);

  if (rc == L7_SUCCESS)
    PT_LOG_INFO(LOG_CTX_INTF, "Success setting linkscan to %u of port %u", enable, port); 
  else
    PT_LOG_ERR(LOG_CTX_INTF, "Error setting linkscan to %u of port %u", enable, port);
  #endif
  #endif
  #endif

  return rc;
}

/**
 * Reset warpcore associated to a specific slot 
 * 
 * @param slot_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_slot_reset(L7_int slot_id, L7_BOOL force_linkup)
{
  L7_uint16 index;
  L7_uint32 ptin_port;
  ptin_hwproc_t hw_proc;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate interface */
  if (slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid slot id %d", slot_id);
    return L7_FAILURE;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_WARPCORE_RESET;
  hw_proc.mask   = 0xff;
  hw_proc.param1 = (L7_int32) slot_id;
  hw_proc.param2 = 0;

  /* Apply procedure */
  rc = dtlPtinHwProc(L7_ALL_INTERFACES, &hw_proc);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying HW procedure to slot %d", slot_id);
    return rc;
  }

  /* Force link up if required */
  if (force_linkup)
  {
    PT_LOG_TRACE(LOG_CTX_CONTROL, "Forcing link up to all ports of slot %u", slot_id); 

    rc = ptin_slot_link_force(slot_id, -1, L7_TRUE, L7_ENABLE);
    if (rc != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_CONTROL, "Error setting force link to slot %u!", slot_id);
      return rc;
    }
  }

  /* Clear counters of all slot ports */
  for (index = 0; index < PTIN_SYS_INTFS_PER_SLOT_MAX; index++)
  {
    /* Get ptin_port */
    if (ptin_intf_slotPort2port(slot_id, index, &ptin_port)==L7_SUCCESS && ptin_port<ptin_sys_number_of_ports)
    {
      /* Reset counters */
      ptin_intf_counters_clear(ptin_port);
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"HW procedure applied to slot %d", slot_id);

  return L7_SUCCESS;
}

/**
 * read linkscan status
 *  
 * @param intIfNum : Interface
 * @param enable : enable (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_linkscan_get(L7_uint32 intIfNum, L7_uint8 *enable)
{
  ptin_hwproc_t hw_proc;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate interface */
  if (intIfNum == 0 || intIfNum > L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = DAPI_CMD_GET;
  hw_proc.procedure = PTIN_HWPROC_LINKSCAN;
  hw_proc.mask = 0xff;
  hw_proc.param1 = 0;
  hw_proc.param2 = 0;

  /* Apply procedure */
  rc = dtlPtinHwProc(intIfNum, &hw_proc);

  if (rc != L7_SUCCESS)
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying HW procedure to intIfNum=%u", intIfNum);
  else
  {
    if (enable != L7_NULLPTR)
    {
      *enable = (L7_uint8) hw_proc.param1;
    }
    PT_LOG_TRACE(LOG_CTX_INTF,"HW linkscan get from intIfNum=%u (%u)", intIfNum, hw_proc.param1);
  }

  return rc;
}

/**
 * Apply linkscan procedure
 *  
 * @param intIfNum : Interface
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_linkscan_set(L7_uint32 intIfNum, L7_uint8 enable)
{
  ptin_hwproc_t hw_proc;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate interface */
  if (intIfNum == 0 || intIfNum > L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_LINKSCAN;
  hw_proc.mask = 0xff;
  hw_proc.param1 = (L7_int32) enable;
  hw_proc.param2 = 0;

  /* Apply procedure */
  rc = dtlPtinHwProc(intIfNum, &hw_proc);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying HW procedure to intIfNum=%u", intIfNum);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"HW procedure applied to intIfNum=%u", intIfNum);

  return L7_SUCCESS;
}

/**
 * Apply linkscan procedure
 *  
 * @param intIfNum : Interface 
 * @param link : link status
 * @param enable : enable
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_link_force(L7_uint32 intIfNum, L7_uint8 link, L7_uint8 enable)
{
  L7_uint32 ptin_port;
  ptin_hwproc_t hw_proc;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate interface */
  if (intIfNum == 0 || intIfNum > L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  /* Get ptin_port format */
  if (ptin_intf_intIfNum2port(intIfNum, &ptin_port) != L7_SUCCESS || ptin_port >= ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u -> no ptin_port correspondence", intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  hw_proc.procedure = PTIN_HWPROC_FORCE_LINK;
  hw_proc.mask = 0xff;
  hw_proc.param1 = (L7_int32) link;
  hw_proc.param2 = 0;

  /* Apply procedure */
  rc = dtlPtinHwProc(intIfNum, &hw_proc);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying link force to %u for intIfNum=%u", enable, intIfNum);
    return rc;
  }

#if (PTIN_BOARD_IS_MATRIX)
  /* Track force link state for each port */
  if (link && enable)
  {
    forcelinked_ports_bmp |= (1ULL << ptin_port);
  }
  else
  {
    forcelinked_ports_bmp &= ~(1ULL << ptin_port);
  }
#endif

  PT_LOG_TRACE(LOG_CTX_INTF,"Force link to %u, applied to intIfNum=%u", enable, intIfNum);

  return rc;
}

/**
 * Apply linkscan procedure
 *  
 * @param slot_id : slot id 
 * @param slot_port : slot port index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_slot_linkscan_set(L7_int slot_id, L7_int slot_port, L7_uint8 enable)
{
  /* Only applied to CXO640G boards */
#if (LINKSCAN_MANAGEABLE_BOARD)

  L7_int    port_idx, ptin_port = -1;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate input params */
  if (slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX ||
      slot_port >= PTIN_SYS_INTFS_PER_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: slot_id=%d, slot_port=%d", slot_id, slot_port);
    return L7_FAILURE;
  }

  /* Apply to only one port of the slot */
  if ( slot_port >= 0)
  {
    port_idx = slot_port;
    ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

    /* Validate port */
    if (ptin_port < 0 || ptin_port >= ptin_sys_number_of_ports ||
        ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Invalid reference slot_id=%d, slot_port=%d -> port=%d", slot_id, port_idx, ptin_port);
      return L7_FAILURE;
    }

    /* Linkscan procedure */
    /* Apply to non protection ports, or being protection, they are active ones */
    if (!ptin_intf_is_uplinkProtection(ptin_port) ||
         ptin_intf_is_uplinkProtectionActive(ptin_port))
    {
      rc = ptin_intf_linkscan_set(intIfNum, enable); 

      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_INTF,"Error applying LS procedure to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, ptin_port, intIfNum);
      else
        PT_LOG_TRACE(LOG_CTX_INTF,"LS procedure applied to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, port_idx, intIfNum);
    }
  }
  /* Apply to all slot ports */
  else
  {
    /* Run all slot ports */
    for (port_idx = 0; port_idx < PTIN_SYS_INTFS_PER_SLOT_MAX; port_idx++)
    {
      ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

      /* If not used, skip */
      if (ptin_port < 0)
        continue;

      /* Validate port */
      if (ptin_port >= ptin_sys_number_of_ports ||
          ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Invalid reference slot_id=%d, slot_port=%d -> port=%d", slot_id, port_idx, ptin_port);
        return L7_FAILURE;
      }

      /* Uplink protection */
      /* Skip protection and inactive ports */
      if ( ptin_intf_is_uplinkProtection(ptin_port) &&
          !ptin_intf_is_uplinkProtectionActive(ptin_port))
      {
        continue;
      }

      /* Linkscan procedure */
      rc = ptin_intf_linkscan_set(intIfNum, enable);

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Error applying LS procedure to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, ptin_port, intIfNum);
        break;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF,"LS procedure applied to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, ptin_port, intIfNum);
        /* Next port */
      }
    }
  }

  if (rc != L7_SUCCESS)
    PT_LOG_ERR(LOG_CTX_INTF,"Terminated with error %d", rc);
  else
    PT_LOG_TRACE(LOG_CTX_INTF,"Finished successfully");

  /* Return execution state */
  return rc;
#else
  return L7_SUCCESS;
#endif
}

/**
 * Force link to all slot ports
 *  
 * @param slot_id : slot id 
 * @param link : link status 
 * @param slot_port : slot port index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_slot_link_force(L7_int slot_id, L7_int slot_port, L7_uint8 link, L7_uint8 enable)
{
  /* Only applied to CXO640G boards */
#if (LINKSCAN_MANAGEABLE_BOARD)

  L7_int    port_idx, ptin_port = -1;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate input params */
  if (slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX ||
      slot_port >= PTIN_SYS_INTFS_PER_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid inputs: slot_id=%d, slot_port=%d", slot_id, slot_port);
    return L7_FAILURE;
  }

  /* Apply to only one port of the slot */
  if ( slot_port >= 0)
  {
    port_idx = slot_port;
    ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

    /* Validate port */
    if (ptin_port < 0 || ptin_port >= ptin_sys_number_of_ports ||
        ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Invalid reference slot_id=%d, slot_port=%d -> port=%d", slot_id, port_idx, ptin_port);
      return L7_FAILURE;
    }

    /* Only do this for non protection ports */
    /* Apply to non protection ports, or being protection, they are active ones */
    if (!ptin_intf_is_uplinkProtection(ptin_port) ||
         ptin_intf_is_uplinkProtectionActive(ptin_port))
    {
      /* Linkscan procedure */
      rc = ptin_intf_link_force(intIfNum, link, enable);

      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_INTF,"Error forcing link to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, ptin_port, intIfNum);
      else
        PT_LOG_TRACE(LOG_CTX_INTF,"Link forced to %u to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", enable, slot_id, port_idx, port_idx, intIfNum);
    }
  }
  /* Apply to all slot ports */
  else
  {
    /* Run all slot ports */
    for (port_idx = 0; port_idx < PTIN_SYS_INTFS_PER_SLOT_MAX; port_idx++)
    {
      ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

      /* If not used, skip */
      if (ptin_port < 0)
        continue;

      /* Validate port */
      if (ptin_port >= ptin_sys_number_of_ports ||
          ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Invalid reference slot_id=%d, slot_port=%d -> port=%d", slot_id, port_idx, ptin_port);
        return L7_FAILURE;
      }

      /* Uplink protection */
      /* Skip protection and inactive ports */
      if ( ptin_intf_is_uplinkProtection(ptin_port) &&
          !ptin_intf_is_uplinkProtectionActive(ptin_port))
      {
        continue;
      }

      /* Linkscan procedure */
      rc = ptin_intf_link_force(intIfNum, link, enable);

      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF,"Error forcing link to slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", slot_id, port_idx, ptin_port, intIfNum);
        break;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_INTF,"Link forced to %u in slot_id=%d, slot_port=%d -> port=%d / intIfNum=%u", enable, slot_id, port_idx, ptin_port, intIfNum);
        /* Next port */
      }
    }
  }

  if (rc != L7_SUCCESS)
    PT_LOG_ERR(LOG_CTX_INTF,"Terminated with error %d", rc);
  else
    PT_LOG_TRACE(LOG_CTX_INTF,"Finished successfully");

  /* Return execution state */
  return rc;
#else
  return L7_SUCCESS;
#endif
}

/**
 * Procedure for board insertion
 * 
 * @param slot_id 
 * @param board_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_slot_action_insert(L7_uint16 slot_id, L7_uint16 board_id)
{
  L7_RC_t   rc_global = L7_SUCCESS;

/* Only applied to CXO640G boards */
#if (LINKSCAN_MANAGEABLE_BOARD)

  L7_int    port_idx, ptin_port = -1;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_uint16 board_id_current;
  L7_RC_t   rc;

  PT_LOG_DEBUG(LOG_CTX_INTF,"Inserting board %u at slot %u", board_id, slot_id);

  /* Validate input params */
  if (slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid slot_id=%d", slot_id);
    return L7_FAILURE;
  }
  if (board_id == 0 || board_id == (L7_uint16)-1)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid board_id %d", board_id);
    return L7_FAILURE;
  }

  /* Block board event processing */
  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Get current board id */
  rc = ptin_slot_boardid_get(slot_id, &board_id_current);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting board id for slot %u (rc=%d)", slot_id, rc);
    return L7_FAILURE;
  }
  /* If board is already present, do nothing */
  if (board_id_current != 0)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_WARN(LOG_CTX_INTF, "Card already present at slot %u (board id is %u)", slot_id, board_id_current);
    return L7_SUCCESS;
  }

  /* Register new board id */
  rc = ptin_slot_boardtype_set(slot_id, board_id);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_INTF, "Error inserting card %u at slot %u (%d)", board_id, slot_id, rc);
    return L7_FAILURE;
  }

  /* Execute some configs for this new board */
  for (port_idx = 0; port_idx < PTIN_SYS_INTFS_PER_SLOT_MAX; port_idx++)
  {
    ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

    /* If not used, skip */
    if (ptin_port < 0)
      continue;

    /* Validate port */
    if (ptin_port >= ptin_sys_number_of_ports ||
        ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
    {
      rc_global = max(L7_FAILURE, rc_global);
      PT_LOG_ERR(LOG_CTX_INTF,"Invalid ptin_port %d", ptin_port);
      continue;
    }

  #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
    /* If board is downlink, activate QoS egress remarking */
    rc = ptin_qos_egress_remark(intIfNum, PTIN_BOARD_IS_DOWNLINK(board_id));
    if (rc != L7_SUCCESS)
    {
      rc_global = max(rc, rc_global);
      PT_LOG_ERR(LOG_CTX_INTF, "Error configuring egress remark for port %u (%d)", ptin_port, rc);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "Egress remark for port %u set to %u", ptin_port, PTIN_BOARD_IS_DOWNLINK(board_id));
    }
  #endif
  }

  /* For CXO160G reset warpcore associated to this slot (only applied to 10G-SFI ports) */
#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  /* Mark this slot to be reseted */
 #if (PHY_RECOVERY_PROCEDURE)
  if (slots_to_be_reseted[slot_id])
  {
    /* Only reset warpcore, if new board is a TG16G board */
    if (board_id == PTIN_BOARD_TYPE_TG16G)
    {
      PT_LOG_INFO(LOG_CTX_INTF, "Going to reset warpcore of slot %u", slot_id);
      rc = ptin_intf_slot_reset(slot_id, L7_FALSE);
      if (rc == L7_SUCCESS)
      {
        PT_LOG_INFO(LOG_CTX_INTF, "Slot %d reseted", slot_id);
      }
      else if (rc == L7_NOT_EXIST)
      {
        PT_LOG_TRACE(LOG_CTX_INTF, "Nothing done to slot %u", slot_id);
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error reseting slot %u", slot_id);
      }
    }

    slots_to_be_reseted[slot_id] = L7_FALSE;
  }
 #endif
#endif

  #ifdef MAP_CPLD
  /* Only active matrix will manage linkscan and force links */
  if (!ptin_fpga_mx_is_matrixactive())
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "I am not active matrix");
    return L7_SUCCESS;
  }
  #endif

  /* Run all slot ports */
  for (port_idx = 0; port_idx < PTIN_SYS_INTFS_PER_SLOT_MAX; port_idx++)
  {
    ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

    /* If not used, skip */
    if (ptin_port < 0)
      continue;

    /* Validate port */
    if (ptin_port >= ptin_sys_number_of_ports ||
        ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
    {
      rc_global = max(L7_FAILURE, rc_global);
      PT_LOG_ERR(LOG_CTX_INTF,"Invalid ptin_port %d", ptin_port);
      continue;
    }

    #ifdef PTIN_LINKSCAN_CONTROL
    if (linkscan_update_control && PTIN_BOARD_LS_CTRL(board_id))
    {
      /* If downlink board, or protection port -> force link up */
      if (PTIN_BOARD_IS_DOWNLINK(board_id) || ptin_intf_is_uplinkProtection(ptin_port))
      {
        /* It it is going to force a link up, it is importante to avoid loops during that procedure.
           To guarantee that, this port will be removed from all vlans.
           Only protection ports at inactive state, don't need this procedure */
        if (!ptin_intf_is_uplinkProtection(ptin_port) ||
            ptin_intf_is_uplinkProtectionActive(ptin_port))
        {
          ptin_vlan_port_remove(ptin_port, 0);
        }

        rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          rc_global = max(rc, rc_global);
          PT_LOG_ERR(LOG_CTX_INTF, "Error enabling force linkup for port %u (%d)", ptin_port, rc);
        }

        /* Add port to vlans again */
        if (!ptin_intf_is_uplinkProtection(ptin_port) ||
            ptin_intf_is_uplinkProtectionActive(ptin_port))
        {
          ptin_vlan_port_add(ptin_port, 0);
        }
        PT_LOG_INFO(LOG_CTX_INTF, "Forced linkup for port %u", ptin_port);
      }
      /* Enable linkscan for uplink boards */
      else if (PTIN_BOARD_IS_UPLINK(board_id))
      {
        rc = ptin_intf_linkscan_set(intIfNum, L7_ENABLE); 
        if (rc != L7_SUCCESS)
        {
          rc_global = max(rc, rc_global);
          PT_LOG_ERR(LOG_CTX_INTF, "Error enabling linkscan for port %u (%d)", ptin_port, rc);
        }
        PT_LOG_INFO(LOG_CTX_INTF, "Linkscan enabled for port %u", ptin_port);
      }
    }
    #endif

    /* Reset counters */
    ptin_intf_counters_clear(ptin_port);
  }

  /* Unblock board event processing */
  osapiSemaGive(ptin_boardaction_sem);
#endif

  return rc_global;
}

/**
 * Procedure for board removal
 * 
 * @author mruas (5/28/2014)
 * 
 * @param slot_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_slot_action_remove(L7_uint16 slot_id)
{
  L7_RC_t   rc_global = L7_SUCCESS;

/* Only applied to CXO640G boards */
#if (LINKSCAN_MANAGEABLE_BOARD)

  L7_int    port_idx, ptin_port = -1;
  L7_uint32 intIfNum = L7_ALL_INTERFACES;
  L7_uint16 board_id;
  L7_RC_t   rc;

  PT_LOG_DEBUG(LOG_CTX_INTF,"Removing board from slot %u", slot_id);

  /* Validate input params */
  if (slot_id < PTIN_SYS_LC_SLOT_MIN || slot_id > PTIN_SYS_LC_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid slot_id=%d", slot_id);
    return L7_FAILURE;
  }

  /* Block board event processing */
  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Get current board id */
  rc = ptin_slot_boardid_get(slot_id, &board_id);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_INTF, "Error getting board type for slot %u (rc=%d)", slot_id, rc);
    return L7_FAILURE;
  }
  /* If board is not present, do nothing */
  if (board_id == 0 || board_id == (L7_uint16)-1)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_WARN(LOG_CTX_INTF, "No card present at slot %u (board id is %u)", slot_id, board_id);
    return L7_SUCCESS;
  }

  /* Register NULL board id */
  rc = ptin_slot_boardtype_set(slot_id, L7_NULL);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_INTF, "Error removing card %u from slot %u (%d)", board_id, slot_id, rc);
    return L7_FAILURE;
  }

  #ifdef MAP_CPLD
  /* Only active matrix will manage linkscan and force links */
  if (!ptin_fpga_mx_is_matrixactive())
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "I am not active matrix");
    return L7_SUCCESS;
  }
  #endif

  /* Run all slot ports */
  for (port_idx = 0; port_idx < PTIN_SYS_INTFS_PER_SLOT_MAX; port_idx++)
  {
    ptin_port = ptin_sys_slotport_to_intf_map[slot_id][port_idx];

    /* If not used, skip */
    if (ptin_port < 0)
      continue;

    /* Validate port */
    if (ptin_port >= ptin_sys_number_of_ports ||
        ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
    {
      rc_global = max(L7_FAILURE, rc_global);
      PT_LOG_ERR(LOG_CTX_INTF,"Invalid ptin_port %d", ptin_port);
      continue;
    }

    #ifdef PTIN_LINKSCAN_CONTROL
    if (linkscan_update_control && PTIN_BOARD_LS_CTRL(board_id))
    {
      /* If downlink board, or protection port -> force link up */
      if (PTIN_BOARD_IS_DOWNLINK(board_id) || ptin_intf_is_uplinkProtection(ptin_port))
      {
        /* Disable force link-up */
        rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          rc_global = max(rc, rc_global);
          PT_LOG_ERR(LOG_CTX_INTF, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
        }
        /* Cause link-down */
        rc = ptin_intf_link_force(intIfNum, L7_FALSE, 0);
        if (rc != L7_SUCCESS)
        {
          rc_global = max(rc, rc_global);
          PT_LOG_ERR(LOG_CTX_INTF, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
        }
        PT_LOG_INFO(LOG_CTX_INTF, "Force link-up disabled for port %u", ptin_port);
      }
      /* Enable linkscan for uplink boards */
      else if (PTIN_BOARD_IS_UPLINK(board_id))
      {
        rc = ptin_intf_linkscan_set(intIfNum, L7_DISABLE); 
        if (rc != L7_SUCCESS)
        {
          rc_global = max(rc, rc_global);
          PT_LOG_ERR(LOG_CTX_INTF, "Error disabling linkscan (%d)", rc);
        }
        PT_LOG_INFO(LOG_CTX_INTF, "Linkscan disabled for port %u", ptin_port);
      }
    }
    #endif
  }

#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
 #if (PHY_RECOVERY_PROCEDURE)
  /* Mark this slot to be reseted */
  if (rc_global == L7_SUCCESS)
  {
    slots_to_be_reseted[slot_id] = L7_TRUE;
    PT_LOG_INFO(LOG_CTX_INTF, "Slot %u marked to be reseted ", slot_id);
  }
 #endif
#endif

  /* Unblock board event processing */
  osapiSemaGive(ptin_boardaction_sem);
#endif

  return rc_global;
}

/**
 * Get interface status
 * 
 * @param ptin_intf : interface (input)
 * @param enable    : admin state (out)
 * @param link      : link state (out)
 * @param board_type: board_id (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_info_get(const ptin_intf_t *ptin_intf, L7_uint16 *enable, L7_uint16 *link, L7_uint16 *board_type)
{
  L7_uint32 intIfNum;
  L7_uint32 adminState, linkState;
  L7_uint16 slot_id, slot_port, board_id;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Intf %u/%u does not exist!", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get slot information (do not look to an eventual error) */
  if (ptin_intf_ptintf2SlotPort(ptin_intf, &slot_id, &slot_port, &board_id) != L7_SUCCESS)
  {
    slot_id   = 0;
    slot_port = 0;
    board_id  = 0;
    //PT_LOG_WARN(LOG_CTX_INTF,"Error getting slot information for intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
  }

  /* Admin state */
  if (nimGetIntfAdminState(intIfNum, &adminState) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error getting admin state for intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Link state */
  if (nimGetIntfLinkState(intIfNum, &linkState) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error getting link state for intf %u/%u", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Output data */
  if (enable    != L7_NULLPTR)  *enable = adminState;
  if (link      != L7_NULLPTR)  *link   = (linkState == L7_UP);
  if (board_type!= L7_NULLPTR)  *board_type = board_id;

  return L7_SUCCESS;
}

/**
 * Get LOS state of a given ptin_port 
 *  
 * @param ptin_intf : interface (input)
 * @param los       : LOS state (out)
 * 
 * @return L7_RC_t : L7_TRUE / L7_FALSE 
 * 
 * @author joaom (11/6/2014)
 */
L7_BOOL ptin_intf_los_get(L7_uint32 ptin_port)
{
  L7_BOOL los = L7_FALSE;

  /* Validate arguments */
  if (ptin_port > PTIN_SYSTEM_N_PORTS)
  {
    return L7_FALSE;
  }

  #if (PTIN_BOARD_IS_STANDALONE)
  los = pfw_shm->intf[ptin_port].port_state & 1;
  #endif

  return los;
}

/**
 * Get Link Down of a given ptin_port 
 *  
 * @param ptin_intf : interface (input)
 * @param link      : link state (out)
 * 
 * @return L7_RC_t : L7_TRUE / L7_FALSE 
 * 
 * @author joaom (11/6/2014)
 */
L7_BOOL ptin_intf_link_get(L7_uint32 ptin_port)
{
  L7_uint32 intIfNum;
  L7_uint32 adminState, linkState;
  L7_BOOL link = L7_FALSE;

  /* Validate arguments */
  if (ptin_port > PTIN_SYSTEM_N_PORTS)
  {
    return L7_FALSE;
  }

  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS || nimGetIntfAdminState(intIfNum, &adminState)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error getting admin state for ptin_port %d", ptin_port);
    return link;
  }

  /* Link state */
  if (nimGetIntfLinkState(intIfNum, &linkState) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error getting link state for ptin_port %d", ptin_port);
    return link;
  }

  link = (linkState == L7_UP);

  return link;
}

/**
 * Get slot mode list
 *  
 * @param slotmodes 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_slotMode_get(L7_uint32 *slotmodes)
{
  ptin_slotmode_t slot_mode;
  L7_RC_t rc;

  /* Validate arguments */
  if (slotmodes == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  memset(&slot_mode,0x00,sizeof(ptin_slotmode_t));
  slot_mode.operation = DAPI_CMD_GET;

  rc=dtlPtinSlotMode(&slot_mode);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading slot mode list");
    return rc;
  }

  /* Return list */
  memcpy(slotmodes, slot_mode.slotMode, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);

  return L7_SUCCESS;
}

/**
 * Validate a slot mode list
 *  
 * @param slotmodes 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_slotMode_validate(L7_uint32 *slotmodes)
{
  ptin_slotmode_t slot_mode;
  L7_RC_t rc;

  /* Validate arguments */
  if (slotmodes == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  memset(&slot_mode,0x00,sizeof(ptin_slotmode_t));

  slot_mode.operation = DAPI_CMD_SET;
  memcpy(slot_mode.slotMode, slotmodes, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);

  rc=dtlPtinSlotMode(&slot_mode);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error validating slot mode list");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 The following 2 functions identify, respectively
 @application/DTL level and @ANDL/hapi level the (static)
             interfaces constituing the internal (stati) LAG
             between TA48 and CXO/matrix.
 A LAG is a P2P structure. That's not exactly what we have, but
 TA48 "sees" a LAG with an (in future 2) interface to CXO slot1
 and another (interface) to CXO slot20. Every circuit/EVC
 enclosing CXOs uses this LAG. Manipulation of this LAG is done
 exclusively by TA48's matrix protection mechanism (so, no LACP
 nor any other one whatsoever).
 */
L7_BOOL ptin_intf_is_internal_lag_member(L7_uint32 intIfNum)
{
  /* Only applicable to TA48GE boards */
#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  L7_uint32 i, port, _intIfNum;

  /* We have 4 backplane ports */
  for (i=0; i<4; i++)
  {
    port = PTIN_SYSTEM_N_ETH+i;
    if (ptin_intf_port2intIfNum(port, &_intIfNum) != L7_SUCCESS)
    {
      PT_LOG_WARN(LOG_CTX_INTF, "Error getting intIfNum from port %u", port);
      return L7_FALSE;
    }

    /* Check if interface is backplane */
    if (intIfNum == _intIfNum)
    {
      return L7_TRUE;
    }
  }
#endif

  return L7_FALSE;
}

/**
 * Protection command
 * 
 * @param slot : board slot
 * @param port : board port
 * @param cmd : command
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_protection_cmd(L7_uint slot, L7_uint port, L7_uint cmd)
{
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  L7_uint lag_idx;
  L7_uint32 ptin_port, intIfNum, lag_intIfNum;

  /* Get intIfNum from slot/port */
  if (ptin_intf_slotPort2IntIfNum(slot, port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Cannot find a valid intIfNum from slot/port=%u/%u", slot, port);
    return L7_FAILURE;
  }

  /* Get lag which belongs this port */
  if (dot3adAggGet(intIfNum, &lag_intIfNum) != L7_SUCCESS || lag_intIfNum == 0 || lag_intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "intIfNum %u does not belong to any lag", intIfNum);
    return L7_FAILURE;
  }

  /* Get ptin_port format, and validate it */
  ptin_port = map_intIfNum2port[lag_intIfNum];

  if (ptin_port < PTIN_SYSTEM_N_PORTS || ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "intIfNum %u / ptin_port %u is not a lag", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  lag_idx = ptin_port - PTIN_SYSTEM_N_PORTS;

  /* Only apply commands to lags where the protection is related */
  if (lag_idx < PTIN_SYSTEM_PROTECTION_LAGID_BASE)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Protection not applicable to intIfNum %u / ptin_port %u (lag_idx=%u)", intIfNum, ptin_port, lag_idx);
    return L7_FAILURE;
  }

  /* Check if port is protected */
  if (!ptin_intf_is_uplinkProtection(ptin_port))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u is not a protection port", ptin_port);
    return L7_FAILURE;
  }

  /* Activate command: add port */
  if (cmd & 1)
  {
    if (dtlDot3adInternalPortAdd(lag_intIfNum, 1, &intIfNum, 1) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error adding intIfNum %u (ptin_port %u) to lag_intIfNum %u (lag_idx=%u)", intIfNum, ptin_port, lag_intIfNum, lag_idx);
      return L7_FAILURE;
    }
    /* Port is active */
    uplink_protection_ports_active_bmp |= ((L7_uint64) 1 << ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "intIfNum %u (ptin_port %u) added to lag_intIfNum %u (lag_idx=%u)", intIfNum, ptin_port, lag_intIfNum, lag_idx);
  }
  /* Innactivate command: remove port */
  else
  {
    if (dtlDot3adInternalPortDelete(lag_intIfNum, 1, &intIfNum, 1) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error removing intIfNum %u (ptin_port %u) from lag_intIfNum %u (lag_idx=%u)", intIfNum, ptin_port, lag_intIfNum, lag_idx);
      return L7_FAILURE;
    }
    /* Port inactive */
    uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "intIfNum %u (ptin_port %u) removed from lag_intIfNum %u (lag_idx=%u)", intIfNum, ptin_port, lag_intIfNum, lag_idx);
  }
#endif

  return L7_SUCCESS;
}


/**
 * Protection command
 * 
 * @param slot : board slot
 * @param port : board port
 * @param cmd : command
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_protection_cmd_planC(L7_uint slot, L7_uint port, L7_uint cmd)
{
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  L7_uint32 ptin_port, intIfNum;
  L7_RC_t rc;

  /* Get intIfNum from slot/port */
  if (ptin_intf_slotPort2port(slot, port, &ptin_port) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Slot/port=%u/%u is not valid", slot, port);
    return L7_FAILURE;
  }

  /* Check if port is protected */
  if (!ptin_intf_is_uplinkProtection(ptin_port))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u is not a protection port", ptin_port);
    return L7_FAILURE;
  }

  /* Activate command: add port */
  if (cmd & 1)
  {
    if (ptin_vlan_port_add(ptin_port, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error adding port %u to all vlans", ptin_port);
      return L7_FAILURE;
    }
    /* Port is active */
    uplink_protection_ports_active_bmp |= ((L7_uint64) 1 << ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u added to all vlans", ptin_port);

  #ifdef PTIN_LINKSCAN_CONTROL
    #ifdef MAP_CPLD
    /* Only active matrix will manage linkscan and force links */
    if (ptin_fpga_mx_is_matrixactive())
    {
      /* Enable linkscan for newly active port */
      rc = ptin_intf_linkscan_set(intIfNum, L7_ENABLE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error enabling linkscan for port %u (%d)", ptin_port, rc);
      }
      PT_LOG_TRACE(LOG_CTX_INTF, "Linkscan enabled for port %u", ptin_port);
    }
    #endif
  #else
    rc = L7_SUCCESS; /* avoid warning */
  #endif
  }
  /* Innactivate command: remove port */
  else
  {
    if (ptin_vlan_port_removeFlush(ptin_port, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error removing port %u from all vlans", ptin_port);
      return L7_FAILURE;
    }
    //fdbFlushByPort(intIfNum);
    /* Port inactive */
    uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u removed from all vlans", ptin_port);

  #ifdef PTIN_LINKSCAN_CONTROL
    #ifdef MAP_CPLD
    /* Only active matrix will manage linkscan and force links */
    if (ptin_fpga_mx_is_matrixactive())
    {
      /* Deactivate linkscan, and force link up for newly inactive port */
      rc = ptin_intf_linkscan_set(intIfNum, L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error disabling linkscan for port %u (%d)", ptin_port, rc);
      }
      PT_LOG_TRACE(LOG_CTX_INTF, "Linkscan enabled for port %u", ptin_port);
      if (rc == L7_SUCCESS)
      {
        rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE); 
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error forcing linkup for port %u (%d)", ptin_port, rc);
        }
        PT_LOG_TRACE(LOG_CTX_INTF, "Forced link-up for port %u", ptin_port);
      }
    }
    #endif
  #endif
  }
#endif

  return L7_SUCCESS;
}

/**
 * Protection command
 * 
 * @param slot_old : board slot
 * @param port_old : board port
 * @param slot_new : board slot
 * @param port_new : board port
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_protection_cmd_planD(L7_uint slot_old, L7_uint port_old, L7_uint slot_new, L7_uint port_new)
{
#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  L7_uint32 ptin_port_old, ptin_port_new;
  L7_uint32 intIfNum_old, intIfNum_new;
  L7_RC_t rc;

  /* Get intIfNum from slot/port */
  if (ptin_intf_slotPort2port(slot_old, port_old, &ptin_port_old) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port_old, &intIfNum_old) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Slot/port=%u/%u is not valid", slot_old, port_old);
    return L7_FAILURE;
  }
  if (ptin_intf_slotPort2port(slot_new, port_new, &ptin_port_new) != L7_SUCCESS ||
      ptin_intf_port2intIfNum(ptin_port_new, &intIfNum_new) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Slot/port=%u/%u is not valid", slot_new, port_new);
    return L7_FAILURE;
  }

  /* Check if ports are protection ones */
  if (!ptin_intf_is_uplinkProtection(ptin_port_old) ||
      !ptin_intf_is_uplinkProtection(ptin_port_new))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u or ptin_port %u is not a protection port", ptin_port_old, ptin_port_new);
    return L7_FAILURE;
  }

  /* Check if ports are protection ones */
  if (ptin_intf_is_uplinkProtectionActive(ptin_port_new))
  {
    PT_LOG_WARN(LOG_CTX_INTF, "ptin_port %u is already the active port", ptin_port_new);
    return L7_SUCCESS;
  }

  /* Switch ports */
  if (ptin_vlan_port_switch(ptin_port_old, ptin_port_new, 0) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error switching port %u to port %u", ptin_port_old, ptin_port_new);
    return L7_FAILURE;
  }
  /* Update active and inactive ports */
  uplink_protection_ports_active_bmp &= ~((L7_uint64) 1 << ptin_port_old);
  uplink_protection_ports_active_bmp |=  ((L7_uint64) 1 << ptin_port_new);
  PT_LOG_TRACE(LOG_CTX_INTF, "port %u successfully switched to port %u", ptin_port_old, ptin_port_new);

  /* Wait 200ms, to stabilise traffic flow */
  osapiSleepMSec(200);

#ifdef PTIN_LINKSCAN_CONTROL
  #ifdef MAP_CPLD
  /* Only active matrix will manage linkscan and force links */
  if (ptin_fpga_mx_is_matrixactive())
  {
    /* Deactivate linkscan, and force link up for newly inactive port */
    rc = ptin_intf_linkscan_set(intIfNum_old, L7_DISABLE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error disabling linkscan for port %u (%d)", ptin_port_old, rc);
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "Linkscan enabled for port %u", ptin_port_old);
    if (rc == L7_SUCCESS)
    {
      rc = ptin_intf_link_force(intIfNum_old, L7_TRUE, L7_ENABLE); 
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error forcing linkup for port %u (%d)", ptin_port_old, rc);
      }
      PT_LOG_TRACE(LOG_CTX_INTF, "Forced link-up for port %u", ptin_port_old);
    }

    /* Enable linkscan for newly active port */
    rc = ptin_intf_linkscan_set(intIfNum_new, L7_ENABLE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error enabling linkscan for port %u (%d)", ptin_port_new, rc);
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "Linkscan enabled for port %u", ptin_port_new);
  }
  #endif
#else
  rc = L7_SUCCESS; /* avoid warning */
#endif
#endif

  return L7_SUCCESS;
}

/**
 * Configure Default VLANs using VCAP rules
 * 
 * @param intIfNum 
 * @param outerVlan 
 * @param innerVlan  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_vcap_defvid(L7_uint32 intIfNum, L7_uint16 outerVlan, L7_uint16 innerVlan)
{
  L7_uint         i;
  L7_INTF_TYPES_t intf_type;
  L7_uint32       intIfNum_list_size;
  L7_uint32       intIfNum_list[PTIN_SYSTEM_N_PORTS];
  ptin_hwproc_t hw_proc;
  L7_RC_t         rc_global = L7_SUCCESS, rc;

  /* Validate ports */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* Interface type */
  if (nimGetIntfType(intIfNum, &intf_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Unable to get intfType from intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* List of ports to be configured */
  if (intf_type == L7_PHYSICAL_INTF)
  {
    intIfNum_list_size = 1;
    intIfNum_list[0] = intIfNum;
  }
  else if (intf_type == L7_LAG_INTF)
  {
    intIfNum_list_size = PTIN_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1, intIfNum, &intIfNum_list_size, intIfNum_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Unable to get LAG members from intIfNum %d", intIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Not supported type (%u) for intIfNum %d", intf_type, intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc, 0x00, sizeof(hw_proc)); 

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_VCAP_DEFVID;
  hw_proc.mask = 0xff;
  hw_proc.param1 = outerVlan;
  hw_proc.param2 = innerVlan;

  for (i = 0; i < intIfNum_list_size; i++)
  {
    /* Apply procedure */
    rc = dtlPtinHwProc(intIfNum_list[i], &hw_proc);

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error configuring defVid %u+%u for intIfNum %u", outerVlan, innerVlan, intIfNum_list[i]);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF,"defVid %u+%u for intIfNum %u configured", outerVlan, innerVlan, intIfNum_list[i]);
    }
  }

  return rc_global;
}


/**
 * Configure clock recovery references
 * 
 * @param ptin_port_main : main port
 * @param ptin_port_bckp : backup port
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_intf_clock_recover_set(L7_int ptin_port_main, L7_int ptin_port_bckp)
{
  ptin_hwproc_t hw_proc;
  L7_RC_t       rc = L7_SUCCESS;

  /* Validate ports */
  if (ptin_port_main < 0 || ptin_port_main >= ptin_sys_number_of_ports)
  {
    PT_LOG_WARN(LOG_CTX_INTF,"Invalid ptin_port %d", ptin_port_main);
    ptin_port_main = -1;
  }
  if (ptin_port_bckp < 0 || ptin_port_bckp >= ptin_sys_number_of_ports)
  {
    PT_LOG_WARN(LOG_CTX_INTF,"Invalid ptin_port %d", ptin_port_bckp);
    ptin_port_bckp = -1;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_CLK_RECVR;
  hw_proc.mask = 0xff;
  hw_proc.param1 = ptin_port_main;
  hw_proc.param2 = ptin_port_bckp;

  /* Apply procedure */
  rc = dtlPtinHwProc(L7_ALL_INTERFACES, &hw_proc);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error configuring recover clocks (main port %d and backup port %d)", ptin_port_main, ptin_port_bckp);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Recover clocks configured (main port %d and backup port %d)", ptin_port_main, ptin_port_bckp);

  return rc;
}

/**
 * Configure Maximum frame size
 * 
 * @param intIfNum 
 * @param frame_size 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
L7_RC_t ptin_intf_frame_oversize_set(L7_uint32 intIfNum, L7_uint32 frame_size)
{
  L7_uint         i;
  L7_INTF_TYPES_t intf_type;
  L7_uint32       intIfNum_list_size;
  L7_uint32       intIfNum_list[PTIN_SYSTEM_N_PORTS];
  ptin_hwproc_t hw_proc;
  L7_RC_t         rc_global = L7_SUCCESS, rc;

  /* Validate ports */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* Interface type */
  if (nimGetIntfType(intIfNum, &intf_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Unable to get intfType from intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* List of ports to be configured */
  if (intf_type == L7_PHYSICAL_INTF)
  {
    intIfNum_list_size = 1;
    intIfNum_list[0] = intIfNum;
  }
  else if (intf_type == L7_LAG_INTF)
  {
    intIfNum_list_size = PTIN_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1, intIfNum, &intIfNum_list_size, intIfNum_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Unable to get LAG members from intIfNum %d", intIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Not supported type (%u) for intIfNum %d", intf_type, intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc, 0x00, sizeof(hw_proc)); 

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_FRAME_OVERSIZE;
  hw_proc.mask = 0xff;
  hw_proc.param1 = frame_size;

  for (i = 0; i < intIfNum_list_size; i++)
  {
    /* Apply procedure */
    rc = dtlPtinHwProc(intIfNum_list[i], &hw_proc);

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error configuring frame_size %u for intIfNum %u", frame_size, intIfNum_list[i]);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF,"Max Frame size (%u) configured for intIfNum %u configured", frame_size, intIfNum_list[i]);
    }
  }

  return rc_global;
}

/**
 * Read Maximum frame size
 * 
 * @param intIfNum 
 * @param frame_size (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
 */
L7_RC_t ptin_intf_frame_oversize_get(L7_uint32 intIfNum, L7_uint32 *frame_size)
{
  L7_uint         i;
  L7_uint         fsize = L7_MAX_FRAME_SIZE;
  L7_INTF_TYPES_t intf_type;
  L7_uint32       intIfNum_list_size;
  L7_uint32       intIfNum_list[PTIN_SYSTEM_N_PORTS];
  ptin_hwproc_t   hw_proc;
  L7_RC_t         rc_global = L7_SUCCESS, rc;

  /* Validate ports */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* Interface type */
  if (nimGetIntfType(intIfNum, &intf_type) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Unable to get intfType from intIfNum %d", intIfNum);
    return L7_FAILURE;
  }

  /* List of ports to be configured */
  if (intf_type == L7_PHYSICAL_INTF)
  {
    intIfNum_list_size = 1;
    intIfNum_list[0] = intIfNum;
  }
  else if (intf_type == L7_LAG_INTF)
  {
    intIfNum_list_size = PTIN_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1, intIfNum, &intIfNum_list_size, intIfNum_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Unable to get LAG members from intIfNum %d", intIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Not supported type (%u) for intIfNum %d", intf_type, intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc, 0x00, sizeof(hw_proc)); 

  hw_proc.operation = DAPI_CMD_GET;
  hw_proc.procedure = PTIN_HWPROC_FRAME_OVERSIZE;
  hw_proc.mask = 0xff;
  hw_proc.param1 = 0;

  for (i = 0; i < intIfNum_list_size; i++)
  {
    /* Apply procedure */
    rc = dtlPtinHwProc(intIfNum_list[i], &hw_proc);

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error reading frame_size for intIfNum %u", intIfNum_list[i]);
      rc_global = rc;
    }
    else 
    {
      PT_LOG_TRACE(LOG_CTX_INTF,"Oversize frame_size for intIfNum %u is %u bytes", intIfNum_list[i], hw_proc.param1);

      /* Select minimum frame size */
      if (hw_proc.param1 < fsize)
        fsize = hw_proc.param1;
    }
  }

  /* Validate calculated frame size */
  if (rc_global == L7_SUCCESS && fsize > L7_MAX_FRAME_SIZE)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid frame_size (%u) for intIfNum %u", fsize, intIfNum_list[i]);
    rc_global = L7_FAILURE;
  }

  /* Return result */
  if (rc_global == L7_SUCCESS)
  {
    if (frame_size != L7_NULLPTR)
    {
      *frame_size = fsize;
      PT_LOG_TRACE(LOG_CTX_INTF,"Oversize frame_size for intIfNum %u is %u bytes", intIfNum_list[i], *frame_size);
    }
  }

  return rc_global;
}


int dapi_usp_is_internal_lag_member(DAPI_USP_t *dusp)
{
  /* Validate arguments */
  if (dusp == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Only applicable to TA48GE boards */
#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
   /* Internal LAG */
   if (1 == dusp->unit && 1 == dusp->slot && dusp->port == 0)
   {
     return L7_TRUE;
   }
#elif ( PTIN_BOARD_IS_MATRIX )
   nimUSP_t usp;
   L7_uint32 intIfNum;

   /* Interface should be a LAG */
   if (dusp->slot != L7_LAG_SLOT_NUM)
   {
     PT_LOG_ERR(LOG_CTX_INTF, "usp {%d,%d,%d} is not a LAG", dusp->unit, dusp->slot, dusp->port);
     return L7_FALSE;
   }

   /* Get intIfNum from dusp */
   usp.unit = dusp->unit;
   usp.slot = dusp->slot;
   usp.port = dusp->port + 1;

   if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
   {
     PT_LOG_ERR(LOG_CTX_INTF, "Error getting intIfNum from usp {%d,%d,%d}", usp.unit, usp.slot, usp.port);
     return L7_FALSE;
   }

   /* Uplink protection */
   #if 0
   L7_uint32 lag_idx;

   /* Convert to lag index (management point of view) */
   if (ptin_intf_intIfNum2lag(intIfNum, &lag_idx) != L7_SUCCESS)
   {
     PT_LOG_ERR(LOG_CTX_INTF, "Error getting intIfNum from usp {%d,%d,%d}", usp.unit, usp.slot, usp.port);
     return L7_FALSE;
   }

   /* If LAG a lag protection? */
   if (lag_idx >= PTIN_SYSTEM_PROTECTION_LAGID_BASE)
   {
     PT_LOG_TRACE(LOG_CTX_INTF, "intIfNum %u is a special port (lagIdx=%u)", intIfNum, lag_idx);
     return L7_TRUE;
   }
   PT_LOG_WARN(LOG_CTX_INTF, "intIfNum %u is a regular LAG (lagIdx=%u)", intIfNum, lag_idx);
   #endif
   
#endif
 return 0;
}

#if (PTIN_BOARD == PTIN_BOARD_TA48GE)

void ptin_ta48ge_led_control(L7_uint32 port, L7_uint8 color, L7_uint8 blink)
{
  #ifdef MAP_FPGA
  if (port >= PTIN_SYSTEM_N_ETH || port >= 64)
    return;

  if (port%2==0)  /* Pair ports */
  {
    fpga_map->reg.led_color_pairports[port/2] = color;
    fpga_map->reg.led_blink_pairports[port/2] = blink;
  }
  else
  {
    fpga_map->reg.led_color_oddports[(port-1)/2] = color;
    fpga_map->reg.led_blink_oddports[(port-1)/2] = blink;
  }
  #endif
}

void ptin_ta48ge_txdisable_control(L7_uint32 port, L7_uint8 state)
{
#ifdef MAP_FPGA
  L7_uint16 base_addr, offset_addr;

  if (port >= PTIN_SYSTEM_N_ETH || port >= 48)
    return;

  base_addr  = port / 8;
  offset_addr= port % 8;

  PT_LOG_TRACE(LOG_CTX_INTF, "port:%u state:%u base_addr:%u offset_addr:%u sfp_txdisable:%p", port, state, base_addr,offset_addr, fpga_map->reg.sfp_txdisable);
  
  if (state)
  {
    fpga_map->reg.sfp_txdisable[base_addr] |=  ((L7_uint8)1 << offset_addr); 
  }
  else
  {
    fpga_map->reg.sfp_txdisable[base_addr] &= ~((L7_uint8)1 << offset_addr); 
  }
#endif
}
#endif


/*****************************************************************************************************************/
/*                                                                                                               */  
/*                                                NGPON2                                                         */ 
/*                                                                                                               */ 
/*****************************************************************************************************************/
 
/**
 * Check if a NGPON2 group already exists 
 * 
 * @param group_idx       
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */ 
L7_RC_t ptin_intf_NGPON2_group_exists(L7_uint8 group_idx)
{

  /* Check if NGPON2 group already exists */
  if (NGPON2_groups_info[group_idx].admin)
    return L7_TRUE;                           

  return L7_FALSE;
}


/**
 * PTIN_INTF NGPON2 Add Group 
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_intf_NGPON2_add_group(ptin_NGPON2group_t *group_info)
{
  L7_uint32 group_idx;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId;

    /* check if the group already exists */
  if (ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "NGPON2 Group already exists");
    return L7_SUCCESS;
  }

    /* group is active */
  NGPON2_groups_info[group_idx].admin = 1;

  NGPON2_groups_info[group_idx].groupId = group_idx;

  return L7_SUCCESS;
}


/**
 * PTIN_INTF NGPON2 Remove Group 
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_intf_NGPON2_rem_group(ptin_NGPON2group_t *group_info)
{
  L7_uint32 group_idx;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId;

    /* check if the group already exists */
  if (!ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "NGPON2 Group does not exist!");
    return L7_SUCCESS;;
  }

      /* group is not active */
  NGPON2_groups_info[group_idx].admin = 0;

  NGPON2_groups_info[group_idx].groupId = NGPON2_EMPTY_ENTRY;

  return L7_SUCCESS;
}


/**
 * PTIN_INTF NGPON2 Add Group Port
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_intf_NGPON2_add_group_port(ptin_NGPON2group_t *group_info)
{
  L7_uint32 group_idx;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId;

    /* check if the group exists */
  if (!ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "NGPON2 Group does not exist!");
    return L7_FAILURE;
  }

  L7_uint8 i = 0;

  while ( i < group_info->numIntf )
  {
      /* set portId to the NGPON2 group */
    NGPON2_PORT_ADD(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64, group_info->NGPON2Port[i].id);
    i++;
  }

    /* increment number of ports for this group */
  L7_uint8 n_ports = 0, temp = 0;

  while(temp < 64)
  {
    if  (NGPON2_BIT_PORT(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64 >> temp))
      n_ports++;

    temp++;
  }

  NGPON2_groups_info[group_idx].nports = n_ports;


  return L7_SUCCESS;
}


/**
 * PTIN_INTF NGPON2 Remove Group Port
 * 
 * @param group_info      : Pointer to struct with group info 
 *                            
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_intf_NGPON2_rem_group_port(ptin_NGPON2group_t *group_info)
{
  L7_uint32 group_idx;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId;

    /* check if the group already exists */
  if (!ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "NGPON2 Group does not exist!");
    return L7_FAILURE;
  }

  L7_uint8 i = 0;

  while ( i < group_info->numIntf )
  {
      NGPON2_PORT_REM(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64, group_info->NGPON2Port[i].id);

    i++;
  }

    /* count number of ports for this group */
  L7_uint8 n_ports = 0, temp = 0;

  while(temp < 64)
  {
    if  (NGPON2_BIT_PORT(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64 >> temp))
    {
      n_ports++;
    }

    temp++;
  }

  NGPON2_groups_info[group_idx].nports = n_ports;

  return L7_SUCCESS;
}




#if 0
/**
 * Reads a LAGs configuration from FP
 * 
 * @param lagInfo Pointer to the output structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_intf_LagConfig_read(ptin_LACPLagConfig_t *lagInfo)
{
  L7_uint32 lag_idx,
            intIfNum,
            value;
  L7_uint32 i, port;
  L7_uint32 member_list[PTIN_SYSTEM_N_PORTS];
  L7_uint32 member_pbmp, nPorts;

  lag_idx = lagInfo->lagId;

  /* Determine loop range (LAG index [1..PTIN_SYSTEM_N_LAGS]) */
  if ((lag_idx == 0) || (lag_idx > PTIN_SYSTEM_N_LAGS))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u is out of range [1..%u]", lag_idx, PTIN_SYSTEM_N_LAGS);
    return L7_FAILURE;
  }

  /* Get LAG intIfNum */
  ptin_intf_lag2intf(lag_idx, &intIfNum); /* No error checking is necessary because
                                           * lag_idx was already validated */

  /* LAG admin */
  if (usmDbDot3adAdminModeGet(1, intIfNum, &value) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading admin field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->admin = value;

  /* LAG STP Mode */
  if (usmDbDot1sPortStateGet(1, intIfNum, &value) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Dot1sPortState field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->stp_enable = value;

  /* LAG Static Mode */
  if (usmDbDot3adIsStaticLag(1, intIfNum, &value) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading static mode field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->static_enable = value;

  /* LAG Balance Mode */
  if (usmDbDot3adLagHashModeGet(1, intIfNum, &value) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading balance mode field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->loadBalance_mode = value;

  nPorts = PTIN_SYSTEM_N_PORTS;
  if (usmDbDot3adMemberListGet(1, intIfNum, &nPorts, member_list) != L7_SUCCESS)
    return L7_FAILURE;

  member_pbmp = 0;
  for (i=0; i<nPorts; i++)
  {
    /* Validate interface number */
    if (member_list[i] == 0 ||
        ptin_intf_intf2port(member_list[i], &port) != L7_SUCCESS)
    {
      continue;
    }

    member_pbmp |= 1 << port;
  }
  lagInfo->members_pbmp = member_pbmp;

  return L7_SUCCESS;
}
#endif

void ptinIntfNumrangeGet(L7_INTF_TYPES_t intf_type)
{  
  L7_uint32 minId, maxId;

  if (nimIntIfNumRangeGet(intf_type,&minId, &maxId) != L7_SUCCESS)
  {
    return;
  }
  printf("intf_tyep:%u range [%u - %u]\n", intf_type, minId, maxId);  
  fflush(stdout);
  return;
}


void ptin_debug_intf_cos_policer_set(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint8 cos, L7_uint32 cir, L7_uint32 eir, L7_uint32 cbs, L7_uint32 ebs)
{
  ptin_intf_t     ptin_intf;
  ptin_bw_meter_t meter;
  L7_RC_t         rc;

  printf("Configuring policer for interface %u/%u + COS %u...", intf_type, intf_id, cos);

  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;

  memset(&meter, 0x00, sizeof(meter));
  meter.cir = cir;
  meter.eir = eir;
  meter.cbs = cbs;
  meter.ebs = ebs;

  rc = ptin_QoS_intf_cos_policer_set(&ptin_intf, cos, &meter);

  printf("Result of operation: rc=%d", rc);
}

void ptin_debug_intf_cos_policer_clear(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint8 cos)
{
  ptin_intf_t     ptin_intf;
  L7_RC_t         rc;

  printf("Removing policer from interface %u/%u + COS %u...", intf_type, intf_id, cos);

  ptin_intf.intf_type = intf_type;
  ptin_intf.intf_id   = intf_id;

  rc = ptin_QoS_intf_cos_policer_clear(&ptin_intf, cos);

  printf("Result of operation: rc=%d", rc);
}


void ptin_intf_stormcontrol_dump(void)
{
  L7_uint32   ptin_port, lag_idx;
  ptin_intf_t ptin_intf;
  L7_BOOL     enable;
  L7_uint32   rate_limit, burst_size;
  L7_uint8    rate_units;
  L7_uchar8   rate_units_str[5][8]={"NONE","PERCENT","KBPS","PPS","???"};

  printf("Dumping storm control configurations...\r\n");

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_INTERF; ptin_port++)
  {
    /* Skip non existent ports */
    if (ptin_port >= ptin_sys_number_of_ports && ptin_port < PTIN_SYSTEM_N_PORTS)
      continue;

    /* Skip non existent LAGs */
    if (PTIN_PORT_IS_LAG(ptin_port) && ptin_intf_port2lag(ptin_port, &lag_idx) != L7_SUCCESS)
      continue;

    /* ptin_intf format */
    if (ptin_intf_port2ptintf(ptin_port, &ptin_intf) != L7_SUCCESS)
    {
      printf("   Error converting ptin_port %u to ptin_intf format!\r\n", ptin_port);
      continue;
    }

    printf("port#%-2u - %u/%-2u:\r\n", ptin_port, ptin_intf.intf_type, ptin_intf.intf_id);

    /* Broadcast storm control data */
    if (ptin_intf_bcast_stormControl_get(&ptin_intf, &enable, &rate_limit, &burst_size, &rate_units) != L7_SUCCESS)
    {
      printf("   Error reading Broadcast stormcontrol data for ptin_port %u (ptin_intf=%u/%u)\r\n", ptin_port, ptin_intf.intf_type, ptin_intf.intf_id);
    }
    else
    {
      printf("   Broadcast:    enable=%u    rate_limit=%8u (%8s)    rate_burst=%6u bytes\r\n", enable, rate_limit, rate_units_str[rate_units], burst_size);
    }

    /* Multicast storm control data */
    if (ptin_intf_mcast_stormControl_get(&ptin_intf, &enable, &rate_limit, &burst_size, &rate_units) != L7_SUCCESS)
    {
      printf("   Error reading Multicast stormcontrol data for ptin_port %u (ptin_intf=%u/%u)\r\n", ptin_port, ptin_intf.intf_type, ptin_intf.intf_id);
    }
    else
    {
      printf("   Multicast:    enable=%u    rate_limit=%8u (%8s)    rate_burst=%6u bytes\r\n", enable, rate_limit, rate_units_str[rate_units], burst_size);
    }

    /* Unicast storm control data */
    if (ptin_intf_ucast_stormControl_get(&ptin_intf, &enable, &rate_limit, &burst_size, &rate_units) != L7_SUCCESS)
    {
      printf("   Error reading Unicast stormcontrol data for ptin_port %u (ptin_intf=%u/%u)\r\n", ptin_port, ptin_intf.intf_type, ptin_intf.intf_id);
    }
    else
    {
      printf("   Unicast  :    enable=%u    rate_limit=%8u (%8s)    rate_burst=%6u bytes\r\n", enable, rate_limit, rate_units_str[rate_units], burst_size);
    }
  }
}


/**
 * PTIN_INTF NGPON2 groups dump
 *  
 * @brief dump all ngpon2 active groups and their related ports 
 *  
 */
void ptin_intf_NGPON2_groups_dump(void)
{
  L7_uint16 i = 0;

  printf("Active groups:\n");

  while ( i < PTIN_SYSTEM_MAX_NGPON2_GROUPS)
  {
    if (NGPON2_groups_info[i].admin)
    {
      printf("GroupID: %u -- Ports Bitmap: %llu -- Number of ports: %u\n", NGPON2_groups_info[i].groupId, NGPON2_groups_info[i].ngpon2_groups_pbmp64, NGPON2_groups_info[i].nports);
    }
    i++;
  }
}

