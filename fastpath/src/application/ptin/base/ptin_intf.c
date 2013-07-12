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

#include "ptin_include.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_xlate_api.h"
#include "fw_shm.h"

#define MAP_EMTPY_ENTRY     0xFFFFFFFF  /* 32bits unsigned */

#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
static const int ptin_sys_slotport_to_intf_map[PTIN_SYS_SLOTS_MAX][PTIN_SYS_INTFS_PER_SLOT_MAX] = PTIN_SLOTPORT_TO_INTF_MAP;

static const int ptin_sys_intf_to_slot_map[PTIN_SYSTEM_N_PORTS] = PTIN_INTF_TO_SLOT_MAP;

static const int ptin_sys_intf_to_port_map[PTIN_SYSTEM_N_PORTS] = PTIN_INTF_TO_PORT_MAP;
#endif

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
 * NOTE: This function must be invoked ONLY after fastpath initialization. 
 * During Init phase1/2/3 stages, the interfaces MAY NOT be initialized! 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_init(void)
{
  L7_uint   i;
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;
  ptin_intf_t ptin_intf;

  /* Reset structures (everything is set to 0xFF) */
  memset(map_port2intIfNum,   0xFF, sizeof(map_port2intIfNum));
  memset(map_intIfNum2port,   0xFF, sizeof(map_intIfNum2port));
  memset(lagConf_data,        0xFF, sizeof(lagConf_data));
  memset(phyExt_data,         0x00, sizeof(phyExt_data));

  /* Initialize phy lookup tables */
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port <=> intIfNum lookup tables init:");
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    /* Get interface ID */
    if (usmDbIntIfNumFromUSPGet(1, 0, i+1, &intIfNum) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get interface of port# %u", i);
      return L7_FAILURE;
    }

    UPDATE_PORT_MAP(i, intIfNum);

    LOG_TRACE(LOG_CTX_PTIN_INTF, " Port# %02u => intIfNum# %02u", i, intIfNum);
  }

  LOG_INFO(LOG_CTX_PTIN_INTF, "Waiting for interfaces to be attached...");
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    while (nimGetIntfState(map_port2intIfNum[i])!=L7_INTF_ATTACHED)
    {
      osapiSleepMSec(100);
    }
    LOG_TRACE(LOG_CTX_PTIN_INTF, "Port %u attached!",i);
  }
  LOG_INFO(LOG_CTX_PTIN_INTF, "All interfaces attached!");

  /* LAG Lookup tables are not initialized because there are no LAGs created yet
   * L7_FEAT_LAG_PRECREATE must be cleared! (checked on ptin_globaldefs.h) */

  /* Initialize phy default TPID and MTU */
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf.intf_id   = i;

    phyExt_data[i].doubletag  = L7_TRUE;
    phyExt_data[i].outer_tpid = PTIN_TPID_OUTER_DEFAULT;
//    phyExt_data[i].inner_tpid = PTIN_TPID_INNER_DEFAULT;

    rc = usmDbDvlantagIntfModeSet(0, map_port2intIfNum[i], L7_ENABLE);
    if (rc != L7_SUCCESS)
    {
      LOG_CRITICAL(LOG_CTX_PTIN_INTF, "Failed to enable DVLAN mode on port# %u", i);
      return L7_FAILURE;
    }

    rc = usmDbDvlantagIntfEthertypeSet(0, map_port2intIfNum[i], PTIN_TPID_OUTER_DEFAULT, L7_TRUE);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      LOG_CRITICAL(LOG_CTX_PTIN_INTF, "Failed to configure default TPID 0x%04X on port# %u (rc = %d)", PTIN_TPID_OUTER_DEFAULT, i, rc);
      return L7_FAILURE;
    }

    rc = usmDbIfConfigMaxFrameSizeSet(map_port2intIfNum[i], PTIN_SYSTEM_MTU_SIZE);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set max frame on port# %u", i);
      return L7_FAILURE;
    }

    rc = usmDbVlanMemberSet(0,1,map_port2intIfNum[i],L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to remove port# %u from vlan 1", i);
      return L7_FAILURE;
    }

    /* QoS initialization */
    if (ptin_intf_QoS_init(&ptin_intf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Phy# %u: Error initializing QoS definitions",i);
      return L7_FAILURE;
    }

//#if (PTIN_BOARD == PTIN_BOARD_TOLT8G)
//    /* For TOLT8G, configure MAC learn priority with higher value on uplink interfaces */
//    if (PTIN_IS_PORT_PON(i))
//    {
//      rc = dtlPtinL2LearnPortSet(map_port2intIfNum[i], PTIN_SYSTEM_PON_PRIO);
//      if (rc != L7_SUCCESS)
//      {
//        LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set Mac Learn priority on port# %u", i);
//        return L7_FAILURE;
//      }
//    }
//#endif

  }

  /* MEF Ext defaults */
  if (ptin_intf_portExt_init()!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed initializing MEF Ext parameters");
    return L7_FAILURE;
  }

  /* Initialize phy conf structure (must be run after default configurations!) */
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    phyConf_data[i].Port = i;
    if (ptin_intf_PhyConfig_read(&phyConf_data[i]) != L7_SUCCESS) {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}


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
  mefExt.Mask                         = 0xffff;
  mefExt.defVid                       = 1;
  mefExt.defPrio                      = 0;
  mefExt.acceptable_frame_types       = 0;
  mefExt.ingress_filter               = L7_FALSE;
  mefExt.restricted_vlan_reg          = 0;
  mefExt.vlan_aware                   = L7_FALSE;
  mefExt.type                         = 0;
  mefExt.doubletag                    = L7_TRUE;
  mefExt.outer_tpid                   = 0x8100;
  mefExt.inner_tpid                   = 0x8100;
  mefExt.macLearn_enable              = L7_TRUE;
  mefExt.macLearn_stationMove_enable  = L7_TRUE;
  mefExt.macLearn_stationMove_prio    = 1;
  mefExt.macLearn_stationMove_samePrio= L7_TRUE;

  /* Only apply to physical interfaces */
  ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;

  /* Run all physical interfaces */
  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++)
  {
    ptin_intf.intf_id = port;

    /* L2 Station move priority defaults */
    #if ( !PTIN_BOARD_IS_MATRIX )
    if (port<PTIN_SYSTEM_N_PONS)
    {
      mefExt.macLearn_stationMove_prio = 1;
    }
    else
    {
      mefExt.macLearn_stationMove_prio = 2;
    }
    #endif

    /* Apply MEF EXT defaults */
    if (ptin_intf_portExt_set(&ptin_intf, &mefExt)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed initializing MEF Ext parameters for interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
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
L7_RC_t ptin_intf_portExt_set(ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || mefExt==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"MefExt parameters:");
  LOG_TRACE(LOG_CTX_PTIN_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  LOG_TRACE(LOG_CTX_PTIN_INTF," Mask = 0x%04x"                    , mefExt->Mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF," defVid = %u"                      , mefExt->defVid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," defPrio = %u"                     , mefExt->defPrio);
  LOG_TRACE(LOG_CTX_PTIN_INTF," acceptable_frame_types = %u"      , mefExt->acceptable_frame_types);
  LOG_TRACE(LOG_CTX_PTIN_INTF," ingress_filter = %u"              , mefExt->ingress_filter);
  LOG_TRACE(LOG_CTX_PTIN_INTF," restricted_vlan_reg = %u"         , mefExt->restricted_vlan_reg);
  LOG_TRACE(LOG_CTX_PTIN_INTF," vlan_aware = %u"                  , mefExt->vlan_aware);
  LOG_TRACE(LOG_CTX_PTIN_INTF," type       = %u"                  , mefExt->type);
  LOG_TRACE(LOG_CTX_PTIN_INTF," doubletag  = %u"                  , mefExt->doubletag);
  LOG_TRACE(LOG_CTX_PTIN_INTF," outer_tpid = %u"                  , mefExt->outer_tpid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," inner_tpid = %u"                  , mefExt->inner_tpid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_enable = %u"             , mefExt->macLearn_enable);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_stationMove_enable  = %u", mefExt->macLearn_stationMove_enable);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_stationMove_prio    = %u", mefExt->macLearn_stationMove_prio);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_stationMove_samePrio= %u", mefExt->macLearn_stationMove_samePrio);

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Apply configuration */
  if (dtlPtinL2PortExtSet(intIfNum, mefExt)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error SETTING MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF, "Success setting MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

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
L7_RC_t ptin_intf_portExt_get(ptin_intf_t *ptin_intf, ptin_HWPortExt_t *mefExt)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || mefExt==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Apply configuration */
  if (dtlPtinL2PortExtGet(intIfNum, mefExt)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error getting MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"MefExt parameters:");
  LOG_TRACE(LOG_CTX_PTIN_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  LOG_TRACE(LOG_CTX_PTIN_INTF," Mask = 0x%04x"                    , mefExt->Mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF," defVid = %u"                      , mefExt->defVid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," defPrio = %u"                     , mefExt->defPrio);
  LOG_TRACE(LOG_CTX_PTIN_INTF," acceptable_frame_types = %u"      , mefExt->acceptable_frame_types);
  LOG_TRACE(LOG_CTX_PTIN_INTF," ingress_filter = %u"              , mefExt->ingress_filter);
  LOG_TRACE(LOG_CTX_PTIN_INTF," restricted_vlan_reg = %u"         , mefExt->restricted_vlan_reg);
  LOG_TRACE(LOG_CTX_PTIN_INTF," vlan_aware = %u"                  , mefExt->vlan_aware);
  LOG_TRACE(LOG_CTX_PTIN_INTF," type       = %u"                  , mefExt->type);
  LOG_TRACE(LOG_CTX_PTIN_INTF," doubletag  = %u"                  , mefExt->doubletag);
  LOG_TRACE(LOG_CTX_PTIN_INTF," outer_tpid = %u"                  , mefExt->outer_tpid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," inner_tpid = %u"                  , mefExt->inner_tpid);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_enable = %u"             , mefExt->macLearn_enable);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_stationMove_enable = %u" , mefExt->macLearn_stationMove_enable);
  LOG_TRACE(LOG_CTX_PTIN_INTF," macLearn_stationMove_prio   = %u" , mefExt->macLearn_stationMove_prio);

  LOG_TRACE(LOG_CTX_PTIN_INTF, "Success getting MEF Ext of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

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
L7_RC_t ptin_intf_portMAC_set(ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || portMac==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"MAC address parameters:");
  LOG_TRACE(LOG_CTX_PTIN_INTF," Port = %u/%u"                       , ptin_intf->intf_type,ptin_intf->intf_id);
  LOG_TRACE(LOG_CTX_PTIN_INTF," Mask = 0x%04x"                      , portMac->Mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF," MAC = %02x:%02x:%02x:%02x:%02x:%02x",
            portMac->macAddr[0],portMac->macAddr[1],portMac->macAddr[2],portMac->macAddr[3],portMac->macAddr[4],portMac->macAddr[5]);

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Set a new MAC address for the specified interface */
  /* Warning: if interface is removed and readded, MAC address will be set to default: For physical interfaces this shouldn't happen */
  if (nimSetIntfAddress(intIfNum,L7_NULL,portMac->macAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error SETTING MAC address to port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF, "Success setting MAC address to port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

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
L7_RC_t ptin_intf_portMAC_get(ptin_intf_t *ptin_intf, ptin_HWPortMac_t *portMac)
{
  L7_uint32 intIfNum;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || portMac==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid argument");
    return L7_FAILURE;
  }

  /* Get intIfNum */
  if (ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting port %u/%u to intIfNum",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %u/%u: intIfNum# %2u", ptin_intf->intf_type,ptin_intf->intf_id, intIfNum);

  /* Apply configuration */
  if (nimGetIntfAddress(intIfNum,L7_NULL,portMac->macAddr)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error getting MAC address of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"MAC address parameters:");
  LOG_TRACE(LOG_CTX_PTIN_INTF," Port = %u/%u"                     , ptin_intf->intf_type,ptin_intf->intf_id);
  LOG_TRACE(LOG_CTX_PTIN_INTF," Mask = 0x%04x"                    , portMac->Mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF," MAC = %02x:%02x:%02x:%02x:%02x:%02x",
            portMac->macAddr[0],portMac->macAddr[1],portMac->macAddr[2],portMac->macAddr[3],portMac->macAddr[4],portMac->macAddr[5]);


  LOG_TRACE(LOG_CTX_PTIN_INTF, "Success getting MAC address of port %u/%u", ptin_intf->intf_type,ptin_intf->intf_id);

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
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  port = phyConf->Port;

  /* Validate port range */
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  /* Get intIfNum */
  ptin_intf_port2intIfNum(port, &intIfNum);
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* PortEnable */
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_PORTEN) &&
      (usmDbIfAdminStateGet(1, intIfNum, &value) == L7_SUCCESS) &&
      (value != phyConf->PortEnable))
  {
    if (usmDbIfAdminStateSet(1, intIfNum, phyConf->PortEnable & 1) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set enable state on port# %u", port);
      return L7_FAILURE;
    }

    #if ( PTIN_BOARD_IS_STANDALONE )
    /* Update shared memory */
    pfw_shm->intf[port].admin = phyConf->PortEnable & 1;
    #endif

    phyConf_data[port].PortEnable = phyConf->PortEnable & 1; /* update buffered conf data */
    LOG_TRACE(LOG_CTX_PTIN_INTF, " State:       %s", phyConf->PortEnable ? "Enabled":"Disabled");
  }

  /* MaxFrame */
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_MAXFRAME) &&
      (usmDbIfConfigMaxFrameSizeGet(intIfNum, &value) == L7_SUCCESS) &&
      (value != phyConf->MaxFrame))
  {
    if (usmDbIfConfigMaxFrameSizeSet(intIfNum, phyConf->MaxFrame) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set max frame on port# %u", port);
      return L7_FAILURE;
    }

    phyConf_data[port].MaxFrame = phyConf->MaxFrame; /* update buffered conf data */
    LOG_TRACE(LOG_CTX_PTIN_INTF, " MaxFrame:    %u", phyConf->MaxFrame);
  }

  /* Loopback */
  if ((phyConf->Mask & PTIN_PHYCONF_MASK_LOOPBACK) &&
      (usmDbIfLoopbackModeGet(intIfNum, &value) == L7_SUCCESS) &&
      (value != (phyConf->LoopBack != 0)))
  {
    if (usmDbIfLoopbackModeSet(intIfNum, (phyConf->LoopBack != 0)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set loopback state on port# %u", port);
      return L7_FAILURE;
    }

    phyConf_data[port].LoopBack = phyConf->LoopBack; /* update buffered conf data */
    LOG_TRACE(LOG_CTX_PTIN_INTF, " Loopback:    %s", phyConf->LoopBack ? "Enabled":"Disabled");
  }

  /* Speed */
  if (phyConf->Mask & PTIN_PHYCONF_MASK_SPEED)
  {
    char speedstr[20];
    switch (phyConf->Speed)
    {
      case PHY_PORT_1000_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_1000SX;
        strcpy(speedstr, "1000Mbps");
        break;

      case PHY_PORT_2500_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
        strcpy(speedstr, "2.5G");
        break;

      case PHY_PORT_10000_MBPS:
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_10GSX;
        strcpy(speedstr, "10G");
        break;

      default:
        speed_mode = L7_PORTCTRL_PORTSPEED_UNKNOWN;
        strcpy(speedstr, "UNKNOWN");
    }

    if ((speed_mode != L7_PORTCTRL_PORTSPEED_UNKNOWN) &&
        (usmDbIfSpeedGet(1, intIfNum, &value) == L7_SUCCESS) &&
        (value != speed_mode))
    {
      if (usmDbIfSpeedSet(1, intIfNum, speed_mode) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to set port speed on port# %u", port);
        return L7_FAILURE;
      }

      phyConf_data[port].Speed = phyConf->Speed; /* update buffered conf data */
      LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       %s", speedstr);
    }
  }

  /* Media (value set based on port type)*/
  if (phyConf->Mask & PTIN_PHYCONF_MASK_MEDIA)
  {
    /* NOTE: it is assumed that the PON ports are mapped from port 0 to L7_SYSTEM_PON_PORTS-1 */
    if ( port < PTIN_SYSTEM_N_PONS ) {
      phyConf_data[port].Media = PHY_PORT_MEDIA_INTERNAL;
//      LOG_TRACE(LOG_CTX_PTIN_INTF, " Media:       Internal");
    }
    else {
      phyConf_data[port].Media = PHY_PORT_MEDIA_OPTICAL;
//      LOG_TRACE(LOG_CTX_PTIN_INTF, " Media:       Optical");
    }
  }

  /* Always clear counters after a reconfiguration */
  portStats.Port = port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_clear(&portStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to clear counters on port# %u", port);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, " Counters:    Cleared!");

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
  L7_uint port = phyConf->Port;

  /* Validate port range */
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port, PTIN_SYSTEM_N_PORTS);
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

  port = phyState->Port;
  phyState->Mask = 0;

  /* Validate port range */
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  /* Get intIfNum ID */
  ptin_intf_port2intIfNum(port, &intIfNum);
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* Speed */
  if (usmDbIfSpeedGet(1, intIfNum, &speed_mode))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get speed of port# %u", port);
    return L7_FAILURE;
  }
  else {
    phyState->Mask |= PTIN_PHYSTATE_MASK_SPEED;

    switch (speed_mode)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        phyState->Speed = PHY_PORT_1000_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       1000Mbps");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        phyState->Speed = PHY_PORT_2500_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       2.5G");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        phyState->Speed = PHY_PORT_10000_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       10G");
        break;

      default:
        phyState->Speed = -1;
    }
  }

  /* Full-Duplex */
  phyState->Mask |= PTIN_PHYSTATE_MASK_DUPLEX;
  phyState->Duplex = 1;
  LOG_TRACE(LOG_CTX_PTIN_INTF, " Full-Duplex: %s", phyState->Duplex?"Enabled":"Disabled");

  /* Link State */
  if (nimGetIntfLinkState(intIfNum, &value) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get link state of port# %d", port);
    return L7_FAILURE;
  }
  else
  {
    phyState->Mask |= PTIN_PHYSTATE_MASK_LINKUP;
    phyState->LinkUp = (value == L7_UP);
    LOG_TRACE(LOG_CTX_PTIN_INTF, " Link State:  %s", phyState->LinkUp ? "Up":"Down");
  }

  /* Auto-negotiation complete */
  phyState->Mask |= PTIN_PHYSTATE_MASK_AUTONEG;
  phyState->AutoNegComplete = 1;
  LOG_TRACE(LOG_CTX_PTIN_INTF, " AutoNeg End: %s", phyState->AutoNegComplete?"Yes":"No");

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
  L7_uint port = portStats->Port;

  /* Validate port range */
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  return dtlPtinCountersRead(portStats);
}


/**
 * Clear counters
 * 
 * @param portStats portStats->Port must be defined
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_counters_clear(ptin_HWEthRFC2819_PortStatistics_t *portStats)
{
  /* Validate port range */
  if (portStats->Port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", portStats->Port, PTIN_SYSTEM_N_PORTS-1);
    return L7_FAILURE;
  }

  return dtlPtinCountersClear(portStats);
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
  return dtlPtinCountersActivityGet(portActivity);
}


/****************************************************************************** 
 * Port, LAGs and Interfaces convertion functions
 ******************************************************************************/

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
L7_RC_t ptin_intf_intIfNum2SlotPort(L7_uint32 intIfNum, L7_uint16 *slot, L7_uint16 *port)
{
  ptin_intf_t ptin_intf;

  /* Convert intIfNum to ptin_intf */
  if (ptin_intf_intIfNum2ptintf(intIfNum,&ptin_intf)!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get slot and port */
  if (ptin_intf_ptintf2SlotPort(&ptin_intf, slot, port)!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

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
  /* Validate slot and port */
  if (slot<PTIN_SYS_LC_SLOT_MIN || slot>PTIN_SYS_LC_SLOT_MAX || port>=PTIN_SYS_INTFS_PER_SLOT_MAX)
  {
    //LOG_ERR(LOG_CTX_PTIN_SSM,"slot %u / port %u is out of range",slot,port);
    return L7_FAILURE;
  }

  /* Determine ptin_intf */

  /* Calculate slot and port */
  #if ( PTIN_BOARD == PTIN_BOARD_CXO640G )

  /* Check if interface is valid */
  if ( ptin_sys_slotport_to_intf_map[slot][port] < 0 )
  {
    //LOG_ERR(LOG_CTX_PTIN_SSM,"slot %u / port %u is not mapped",slot,port);
    return L7_FAILURE;
  }

  if (ptin_intf!=L7_NULLPTR)
  {
    ptin_intf->intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf->intf_id   = ptin_sys_slotport_to_intf_map[slot][port];
  }

  #else
  if (ptin_intf!=L7_NULLPTR)
  {
    ptin_intf->intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf->intf_id   = (port==0) ? (slot-2) : (slot+18-2);
  }
  #endif

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
L7_RC_t ptin_intf_ptintf2SlotPort(ptin_intf_t *ptin_intf, L7_uint16 *slot_ret, L7_uint16 *port_ret)
{
  L7_uint slot, port;

  /* Validate arguments */
  if (ptin_intf == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Do not accept non physical interfaces */
  if (ptin_intf->intf_type!=PTIN_EVC_INTF_PHYSICAL)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid interface (%u/%u)", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Validate interface id */
  if (ptin_intf->intf_id >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid interface id (%u/%u)", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Calculate slot and port */
  #if ( PTIN_BOARD == PTIN_BOARD_CXO640G )

  /* Check if interface is used */
  if ( ptin_sys_intf_to_slot_map[ptin_intf->intf_id] < 0 ||
       ptin_sys_intf_to_port_map[ptin_intf->intf_id] < 0 )
  {
    //LOG_ERR(LOG_CTX_PTIN_SSM,"ptin_intf=%u/%u is not mapped!", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  slot = ptin_sys_intf_to_slot_map[ptin_intf->intf_id];
  port = ptin_sys_intf_to_port_map[ptin_intf->intf_id];

  #else
  slot = ptin_intf->intf_id + 2;
  port = 0;
  if (ptin_intf->intf_id >= (PTIN_SYSTEM_N_PORTS-1)/2 )
  {
    slot -= (PTIN_SYSTEM_N_PORTS-1)/2;
    port  = 1;
  }
  #endif

  /* Validate slot and port */
  if (slot<PTIN_SYS_LC_SLOT_MIN || slot>PTIN_SYS_LC_SLOT_MAX || port>=PTIN_SYS_INTFS_PER_SLOT_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_SSM,"Invalid slot (%u) or port (%u) for ptin_intf=%u/%u", slot, port, ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }

  if (slot_ret!=L7_NULLPTR)  *slot_ret = slot;
  if (port_ret!=L7_NULLPTR)  *port_ret = port;

  return L7_SUCCESS;
}


/**
 * Get current slot_id for this board
 * 
 * @param slot_id : slot index (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_slot_get(L7_uint8 *slot_id)
{
  L7_uint8 raw_slot = 0;

  #ifdef MAP_CPLD
  /* Is CPLD mapped? */
  if (cpld_map==L7_NULLPTR || cpld_map==(void *)-1)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"CPLD 0x%04x is not mapped in memory",CPLD_ID);
    return L7_FAILURE;
  }
  /* Is CPLD id the expected one? */
  if (cpld_map->reg.id == 0x0000 || cpld_map->reg.id == 0xffff)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"CPLD id 0x%04x is not valid",cpld_map->reg.id);
    return L7_FAILURE;
  }
  /* Get raw slot id */
  raw_slot = cpld_map->reg.slot_id;
  #endif

  /* Validate range */
  if (raw_slot>=PTIN_SYSTEM_MAX_N_FULLSLOTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid raw slot (%u)",raw_slot);
    return L7_FAILURE;
  }

  /* Return management slot */
  if (slot_id!=L7_NULLPTR)
  {
    *slot_id = raw_slot+2;
  }

  return L7_SUCCESS;
}

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
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u]", ptin_port, PTIN_SYSTEM_N_INTERF-1);
    return L7_FAILURE;
  }

  /* Validate output */
  if (map_port2intIfNum[ptin_port] == 0 || map_port2intIfNum[ptin_port] >= L7_MAX_INTERFACE_COUNT)
  {
    //LOG_WARNING(LOG_CTX_PTIN_INTF, "ptin_port# %u is not assigned to any interface",ptin_port);
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
    LOG_ERR(LOG_CTX_PTIN_INTF, "intIfNum# %u is out of range [1..%u]", intIfNum, L7_MAX_INTERFACE_COUNT);
    return L7_FAILURE;
  }

  /* Validate output */
  if (map_intIfNum2port[intIfNum] >= PTIN_SYSTEM_N_INTERF)
  {
    //LOG_WARNING(LOG_CTX_PTIN_INTF, "intIfNum# %u is not assigned!", intIfNum);
    return L7_FAILURE;
  }

  if (ptin_port != L7_NULLPTR)
  {
    *ptin_port = map_intIfNum2port[intIfNum];
  }

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
  ptin_intf_t p_intf;

  /* Validate ptin_port */
  if (ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "PTin port is out of range: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Convert ptin_port to type+id format */
  if (ptin_port<PTIN_SYSTEM_N_PORTS)
  {
    p_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    p_intf.intf_id   = ptin_port;
  }
  else
  {
    p_intf.intf_type = PTIN_EVC_INTF_LOGICAL;
    p_intf.intf_id   = ptin_port - PTIN_SYSTEM_N_PORTS;
  }

  if (ptin_intf!=L7_NULLPTR)  *ptin_intf = p_intf;

  return L7_SUCCESS;
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
inline L7_RC_t ptin_intf_ptintf2port(ptin_intf_t *ptin_intf, L7_uint32 *ptin_port)
{
  L7_uint32 p_port=0;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"ptin_intf is a null pointer");
    return L7_FAILURE;
  }

  /* Calculate ptin_port index */
  if (ptin_intf->intf_type==PTIN_EVC_INTF_PHYSICAL)
  {
    if (ptin_intf->intf_id>=PTIN_SYSTEM_N_PORTS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Physical port id is out of range: %u", ptin_intf->intf_id);
      return L7_FAILURE;
    }
    p_port = ptin_intf->intf_id;
  }
  else if (ptin_intf->intf_type==PTIN_EVC_INTF_LOGICAL)
  {
    if (ptin_intf->intf_id>=PTIN_SYSTEM_MAX_N_LAGS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Lag id is out of range: %u", ptin_intf->intf_id);
      return L7_FAILURE;
    }
    p_port = PTIN_SYSTEM_N_PORTS + ptin_intf->intf_id;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port type is not valid");
    return L7_FAILURE;
  }

  /* Validate final value */
  if (p_port>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port id is out of range: %u", p_port);
    return L7_FAILURE;
  }

  if (ptin_port!=L7_NULLPTR)  *ptin_port = p_port;

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
  L7_uint32 ptin_port;
  L7_RC_t rc;

  /* Get ptin_port*/
  if ((rc=ptin_intf_intIfNum2port(intIfNum, &ptin_port))!=L7_SUCCESS)
    return rc;

  /* Validate ptin_port */
  if (ptin_port>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "PTin port is out of range: %u", ptin_port);
    return L7_FAILURE;
  }

  /* Convert ptin_port to type+id format */
  if (ptin_intf_port2ptintf(ptin_port,ptin_intf)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting ptin_port %u to type+id format", ptin_port);
    return L7_FAILURE;
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
inline L7_RC_t ptin_intf_ptintf2intIfNum(ptin_intf_t *ptin_intf, L7_uint32 *intIfNum)
{
  L7_uint32 ptin_port=0, intIfNum0;
  L7_RC_t rc;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"ptin_intf is a null pointer");
    return L7_FAILURE;
  }

  /* Calculate ptin_port index */
  if (ptin_intf_ptintf2port(ptin_intf,&ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error converting ptin_intf %u/%u to ptin_port", ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get interface# */
  if ((rc=ptin_intf_port2intIfNum(ptin_port,&intIfNum0))!=L7_SUCCESS)
    return rc;
  /* Validate intIfNum */
  if (intIfNum0==0 || intIfNum0>=L7_MAX_INTERFACE_COUNT)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "IntIfNum is out of range (%u)",intIfNum);
    return L7_FAILURE;
  }

  /* Return intIfNum */
  if (intIfNum!=L7_NULLPTR)  *intIfNum = intIfNum0;

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
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  *intIfNum = map_port2intIfNum[PTIN_SYSTEM_N_PORTS + lag_idx];
  return L7_SUCCESS;
}

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
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
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
  L7_uint32 lag_idx = lagInfo->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG is not created */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u does not exist", lag_idx);
    return L7_FAILURE;
  }

  /* Copy data to the output structure */
  *lagInfo = lagConf_data[lag_idx];

  return L7_SUCCESS;
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
  L7_uint32 maxFrame;
  L7_uint32 value;
  L7_char8  lag_name[DOT3AD_MAX_NAME];
  L7_RC_t   rc;
  L7_BOOL   newLag;
  L7_uint32 ifSpeed;
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

  lag_idx = lagInfo->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* If members list is empty, report error */
  if (lagInfo->members_pbmp64 == 0)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: members list is empty", lag_idx);
    return L7_FAILURE;
  }

  /* Check if members have all the same interface speed, EtherType and MaxFrame size */
  members_pbmp = lagInfo->members_pbmp64 >> PTIN_SYSTEM_N_PONS;
  ifSpeed      = 0;
  lagEtherType = 0;
  maxFrame     = 0;
  for (port=PTIN_SYSTEM_N_PONS; port<PTIN_SYSTEM_N_PORTS; port++, members_pbmp>>=1)
  {
    if (members_pbmp & 1)
    {
      ptin_intf_port2intIfNum(port, &intIfNum);
  
      /* check to find the speed of the interface about to join a portChannel */
      if (usmDbIfSpeedGet(1, intIfNum, &value) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading ifSpeed from port# %u", lag_idx, port);
        return L7_FAILURE;
      }

      if (ifSpeed == 0)
        ifSpeed = value;

      if (value != ifSpeed)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: there are members with different if speed", lag_idx);
        return L7_FAILURE;
      }

      if (lagEtherType == 0)
        lagEtherType = phyExt_data[port].outer_tpid;

      if (lagEtherType != phyExt_data[port].outer_tpid)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: there are members with different EtherType (0x%04X != 0x%04X)",
                lag_idx, lagEtherType, phyExt_data[port].outer_tpid);
        return L7_FAILURE;
      }

      if (maxFrame == 0)
        maxFrame = phyConf_data[port].MaxFrame;

      if (maxFrame != phyConf_data[port].MaxFrame)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: there are members with different MaxFrame sizes! (%d != %d)",
                lag_idx, maxFrame, phyConf_data[port].MaxFrame);
        return L7_FAILURE;
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
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: the following members belong to other LAGs: 0x%010llX", lag_idx, lagInfo->members_pbmp64 & members_pbmp_all);
    return L7_FAILURE;
  }

  /* Check if members are not being used in any EVC */
  members_pbmp = lagInfo->members_pbmp64;
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++, members_pbmp>>=1)
  {
    if ((members_pbmp & 1) &&
        (ptin_evc_is_intf_in_use(i)))
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: member %u is being used in EVCs! Lag cannot be created/updated", lag_idx, i);
      return L7_FAILURE;
    }
  }

  /* If LAG does not exist, create it */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    osapiSnprintf(lag_name, DOT3AD_MAX_NAME, "lag%02u", lag_idx);

    /* Try to create an empty LAG */
    rc = usmDbDot3adCreateSet(1, lag_name, FD_DOT3AD_ADMIN_MODE, 
                              FD_DOT3AD_LINK_TRAP_MODE, 0,
                              FD_DOT3AD_HASH_MODE, NULL, &lag_intf);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error on usmDbDot3adCreateSet()", lag_idx);
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

    LOG_TRACE(LOG_CTX_PTIN_INTF, "LAG# %02u created with empty members (interface# %02u)", lag_idx, lag_intf);

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
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: failed setting max frame (%u)", lag_idx, maxFrame);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_INTF, " MaxFrameSize set to %u", maxFrame);
      }
    }

    /* QoS initialization */
    ptin_intf.intf_type = PTIN_EVC_INTF_LOGICAL;
    ptin_intf.intf_id   = lag_idx;
    if (ptin_intf_QoS_init(&ptin_intf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: Error initializing QoS definitions", lag_idx);
      rc = L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_INTF, "LAG# %u: Success initializing QoS definitions", lag_idx);
    }
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

      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: no FP config found! Clearing PTin structs and aborting operation", lag_idx);
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_INTF, "LAG# %02u already exists (interface# %02u)", lag_idx, lag_intf);
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
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error setting Admin mode to %u", lag_idx, lagInfo->admin);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_INTF, " .admin            = %u", lagInfo->admin);
        lagConf_data[lag_idx].admin = lagInfo->admin;
      }
    }
  
    /* LAG STP Mode */
    lagInfo->stp_enable &= 1;
    if (lagConf_data[lag_idx].stp_enable != lagInfo->stp_enable)
    {
      if (usmDbDot1sPortStateSet(1, lag_intf, lagInfo->stp_enable) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error setting STP mode to %u", lag_idx, lagInfo->stp_enable);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_INTF, " .stp_enable       = %u", lagInfo->stp_enable);
        lagConf_data[lag_idx].stp_enable = lagInfo->stp_enable;
      }
    }
  
    /* LAG Static Mode */
    lagInfo->static_enable &= 1;
    if (lagConf_data[lag_idx].static_enable != lagInfo->static_enable)
    {
      if (usmDbDot3adLagStaticModeSet(1, lag_intf, lagInfo->static_enable) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error setting Static mode to %u", lag_idx, lagInfo->static_enable);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_INTF, " .static_enable    = %u", lagInfo->static_enable);
        lagConf_data[lag_idx].static_enable = lagInfo->static_enable;
      }
    }
  
    /* LAG Balance Mode */
    if (lagConf_data[lag_idx].loadBalance_mode != lagInfo->loadBalance_mode)
    {
      if (usmDbDot3adLagHashModeSet(1, lag_intf, lagInfo->loadBalance_mode) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error setting Balance mode to %u", lag_idx, lagInfo->loadBalance_mode);
        rc = L7_FAILURE;
      }
      else
      {
        LOG_TRACE(LOG_CTX_PTIN_INTF, " .loadBalance_mode = %u", lagInfo->loadBalance_mode);
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
        LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: failed to undo LAG creation", lag_idx);

      CLEAR_LAG_CONF(lag_idx);
      CLEAR_LAG_MAP(lag_idx);

      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u was removed (undo creation)", lag_idx);
      return rc;
    }

    /* Otherwise continue, but report error in the end... */
  }

  if (lagConf_data[lag_idx].members_pbmp64 != lagInfo->members_pbmp64)
    LOG_TRACE(LOG_CTX_PTIN_INTF, " .members_pbmp     = 0x%010llX", lagInfo->members_pbmp64);

  /* Initialize vars needed in the loop */
  if (newLag)
    lagConf_data[lag_idx].members_pbmp64 = 0; /* Previously set to FFs */
  members_pbmp = lagInfo->members_pbmp64;
  rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_INTF, "members_pbmp = 0x%016x", members_pbmp);

  /* Loop through all the phy ports and check if any is being added or removed */
  for (port=0; port<PTIN_SYSTEM_N_PORTS; port++, members_pbmp>>=1)
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
      LOG_ERR(LOG_CTX_PTIN_INTF,"Error getting MEF Ext. attributes of interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
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
          LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: port# %u does not have LACP enabled", lag_idx, port);
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
          LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: port# %u already belongs to another LAG", lag_idx, port);
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
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: Error getting admin state of port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }
      #endif

      /* Disable auto-negotiation */
      if (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, L7_DISABLE) != L7_SUCCESS)
      {
        LOG_WARNING(LOG_CTX_PTIN_INTF, "LAG# %u: could not configure Auto-Neg off on port# %u", lag_idx, port);
        //rc = L7_FAILURE;
        //continue;
      }

      /* Add member */
      if (usmDbDot3adMemberAddSet(1, lag_intf, intIfNum) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: could not add member port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }

      #if 0
      /* Restore admin state of this interface */
      if (ptin_intf_LACPAdminState_set(&lacpAdminState) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: Error setting admin state to port# %u", lag_idx, port);
        //rc = L7_FAILURE;
        //continue;
      }
      #endif

      lagConf_data[lag_idx].members_pbmp64 |= (L7_uint64)1 << port;
      LOG_TRACE(LOG_CTX_PTIN_INTF, " Port# %02u added", port);
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
        LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: could not remove member port# %u", lag_idx, port);
        rc = L7_FAILURE;
        continue;
      }

      /* Update PortGroup for the member removed (reset to default value) */
      if (ptin_xlate_portgroup_set(intIfNum, PTIN_XLATE_PORTGROUP_INTERFACE) != L7_SUCCESS)
      {
        LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: could not update PortGroup for member port# %u", lag_idx, port);
        rc = L7_FAILURE;
      }

      lagConf_data[lag_idx].members_pbmp64 &= ~((L7_uint64)1 << port);
      LOG_TRACE(LOG_CTX_PTIN_INTF, " Port# %02u removed", port);
    }
  }

  /* Update PortGroups (used on egress translations) */
  if (ptin_xlate_portgroup_set(lag_intf, PTIN_XLATE_PORTGROUP_INTERFACE) != L7_SUCCESS)
  {
    /* NOTE: if any error occurs during PortGroup set, it originates inconsistency on
     * the portgroups lookup table, which means a CRITICAL or even FATAL situation!
     * However, any error should occur ONLY during debug or validation process, and
     * not during normal operation (under production) */ 
    LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: could not update PortGroup for this LAG", lag_idx);
    rc = L7_FAILURE;
  }

  /* Remove this interface from VLAN 1 (only if a new LAG was created)
   * The idea is to avoid accepting traffic comming on VLAN1 that is not LACP */
  if (newLag)
  {
    if (usmDbVlanMemberSet(1, 1, lag_intf, L7_DOT1Q_FORBIDDEN, DOT1Q_SWPORT_MODE_NONE) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: could not remove interface# %u from VLAN 1", lag_idx, lag_intf);
      rc = L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_INTF, " Intf# %02u removed from VLAN# 1", lag_intf);
    }

    /* Enable DVLAN mode */
    rc = usmDbDvlantagIntfModeSet(0, lag_intf, L7_ENABLE);
    if (rc != L7_SUCCESS)
    {
      LOG_CRITICAL(LOG_CTX_PTIN_INTF, "Failed to enable DVLAN mode on LAG# %u", lag_idx);
      rc = L7_FAILURE;
    }

    rc = usmDbDvlantagIntfEthertypeSet(0, lag_intf, lagEtherType, L7_TRUE);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      LOG_CRITICAL(LOG_CTX_PTIN_INTF, "Failed to configure TPID 0x%04X on LAG# %u (rc = %d)", lagEtherType, lag_idx, rc);
      rc = L7_FAILURE;
    }
    else
      rc = L7_SUCCESS;
  }

  #if 0
  /* In case of success, apply MAC learning attributes */
  if ( rc == L7_SUCCESS )
  {
    /* Use LAG intIfNum */
    if (ptin_intf_intIfNum2ptintf(lag_intf,&ptin_intf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF,"Error getting ptin_intf from LAG intIfNum %u",lag_intf);
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
        LOG_ERR(LOG_CTX_PTIN_INTF,"Error applying MEF Ext attributes to ptin_intf %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
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
        LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: failed to undo LAG creation", lag_idx);

      CLEAR_LAG_CONF(lag_idx);
      CLEAR_LAG_MAP(lag_idx);

      LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u was removed (undo creation)", lag_idx);
      return rc;
    }
  }

  return rc;
}

/**
 * Deletes a LAG
 * 
 * @param lagInfo Structure that references the LAG to destroy
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_Lag_delete(ptin_LACPLagConfig_t *lagInfo)
{
  L7_uint32 lag_idx;
  L7_uint32 lag_intIfNum;
  L7_uint   lag_port;
  L7_uint32 value;
  L7_uint   i;
  L7_uint64 ptin_pbmp;
  L7_uint32 intIfNum = 0;

  lag_idx = lagInfo->lagId;
  lag_port = PTIN_SYSTEM_N_PORTS + lag_idx;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG really exists */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    LOG_WARNING(LOG_CTX_PTIN_INTF, "LAG# %u does not exist", lag_idx);
    return L7_SUCCESS;
  }

  ptin_intf_lag2intIfNum(lag_idx, &lag_intIfNum);

  /* Check if LAG is being used in any EVC */
  if (ptin_evc_is_intf_in_use(lag_port))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is being used in EVCs! Cannot be removed", lag_idx);
    return L7_FAILURE;
  }

  /* Remove LAG */
  if (usmDbDot3adRemoveSet(1, lag_intIfNum) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: failed to remove this LAG", lag_idx);
    return L7_FAILURE;
  }

  /* Wait until lag exists */
  do {
    if ((usmDbIntfStatusGet(lag_intIfNum, &value) == L7_SUCCESS) && (value == L7_INTF_UNINITIALIZED))
      break;

    osapiSleepMSec(10);
  } while (1);

  /* Update PortGroups (used on egress translations) */
  ptin_pbmp = lagConf_data[lag_idx].members_pbmp64;
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++, ptin_pbmp >>= 1)
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
        LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: could not update PortGroup for member port# %u", lag_idx, i);
      }
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

  LOG_TRACE(LOG_CTX_PTIN_INTF, "LAG# %u: successfully deleted", lag_idx);

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

  lag_idx = lagStatus->lagId;

  /* Validate LAG range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lag_idx >= PTIN_SYSTEM_N_LAGS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [0..%u]", lag_idx, PTIN_SYSTEM_N_LAGS-1);
    return L7_FAILURE;
  }

  /* Check if LAG really exists */
  if (!ptin_intf_lag_exists(lag_idx))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u does not exist", lag_idx);
    return L7_FAILURE;
  }

  ptin_intf_lag2intIfNum(lag_idx, &lag_intf);

  /* LAG admin */
  lagStatus->admin = lagConf_data[lag_idx].admin;

  /* LAG Static Mode */
  lagStatus->port_channel_type = lagConf_data[lag_idx].static_enable;

  /* Link status */
  if (nimGetIntfLinkState(lag_intf, &value) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading Link State", lag_idx);
    return L7_FAILURE;
  }
  lagStatus->link_status = (value == L7_UP);

  /* Get members list */
  lagStatus->members_pbmp64 = lagConf_data[lag_idx].members_pbmp64;

  /* Get list of active ports */
  nElems = sizeof(members_list) / sizeof(members_list[0]);
  if (usmDbDot3adActiveMemberListGet(1, lag_intf, &nElems, members_list) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading Active Members List", lag_idx);
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
        || (value >= PTIN_SYSTEM_N_PORTS))
    {
      LOG_CRITICAL(LOG_CTX_PTIN_INTF, "LAG# %u: invalid active members found (port# %u; intf# %u)", lag_idx, value, members_list[i]);
      continue;
    }
    members_pbmp |= (L7_uint64)1 << value;
  }
  lagStatus->active_members_pbmp64 = members_pbmp;

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

  port_idx = adminState->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port_idx, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Get Current Admin State */
  if ((usmDbDot3adAggPortActorAdminStateGet(1, port_intf, &actor_state) != L7_SUCCESS) ||
      (usmDbDot3adAggPortPartnerAdminStateGet(1, port_intf, &partner_state) != L7_SUCCESS))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u: failed to read actor/partner states (intf=%u)", port_idx, port_intf);
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
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: failed to set actor/partner states", port_idx);
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

  port_idx = adminState->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port_idx, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Get Current Admin State */
  if (usmDbDot3adAggPortActorAdminStateGet(1, port_intf, &actor_state) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u: failed to read actor state (intf=%u)", port_idx, port_intf);
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

  port_idx = lagStats->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port_idx, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  if (ptin_intf_port2intIfNum(port_idx, &port_intf)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error getting intIfNum value from Port# %u", port_idx);
    return L7_FAILURE;
  }

  /* Read values */
  if (usmDbDot3adAggPortStatsLACPDUsRxGet(1, port_intf, &rxStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u: error reading LACPDUs Rx stats", port_idx);
    return L7_FAILURE;
  }

  if (usmDbDot3adAggPortStatsLACPDUsTxGet(1, port_intf, &txStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u: error reading LACPDUs Tx stats", port_idx);
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

  port_idx = lagStats->port;

  /* Validate Port range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (port_idx > PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port_idx, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  ptin_intf_port2intIfNum(port_idx, &port_intf);

  /* Clear stats */
  if (usmDbDot3adPortStatsClear(1, port_intf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u: error clearing LACP stats", port_idx);
    return L7_FAILURE;
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
L7_RC_t ptin_QoS_intf_config_set(ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2, cos;
  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || intfQos==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"Interface = %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"Mask         = 0x%02x",intfQos->mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"TrustMode    = %u",intfQos->trust_mode);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"PrioMap.mask   =0x%02x",intfQos->pktprio.mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
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
    LOG_ERR(LOG_CTX_PTIN_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Is there any configuration to be applied? */
  if (intfQos->mask==0x00)
  {
    LOG_WARNING(LOG_CTX_PTIN_INTF, "Empty mask: no configuration to be applied");
    return L7_SUCCESS;
  }

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  // Get Trust mode
  if (usmDbQosCosMapTrustModeGet( 1, intIfNum, &trust_mode)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error with usmDbQosCosMapTrustModeGet");
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc = L7_FAILURE;
  }
  // Validate trust mode
  else if (trust_mode==L7_NULL || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid trust mode (%u)",trust_mode);
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc = L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Current trust mode is %u",trust_mode);

  /* Set Trust mode */
  if (intfQos->mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
  {
    // Define trust mode
    if (usmDbQosCosMapTrustModeSet(1,intIfNum,intfQos->trust_mode)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosMapTrustModeSet");
      rc = L7_FAILURE;
    }
    else
    {
      // Configuration successfull => change trust mode value
      trust_mode = intfQos->trust_mode;
      LOG_TRACE(LOG_CTX_PTIN_INTF, "New trust mode is %u",trust_mode);
    }
  }
  /* Bandwidth units */
  if (intfQos->mask & PTIN_QOS_INTF_BANDWIDTHUNIT_MASK)
  {
    /* Do nothing */
    LOG_WARNING(LOG_CTX_PTIN_INTF, "Bandwidth units was not changed");
  }
  /* Shaping rate */
  if (intfQos->mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
  {
    if (usmDbQosCosQueueIntfShapingRateSet(1,intIfNum,intfQos->shaping_rate)!=L7_SUCCESS)
    {  
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet");
      rc = L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_INTF, "New shaping rate is %u",intfQos->shaping_rate);
    }
  }
  /* WRED decay exponent */
  if (intfQos->mask & PTIN_QOS_INTF_WRED_DECAY_EXP_MASK)
  {
    if (usmDbQosCosQueueWredDecayExponentSet(1,intIfNum,intfQos->wred_decay_exponent)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosQueueWredDecayExponentSet");
      rc = L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_INTF, "New WRED Decay exponent is %u",intfQos->wred_decay_exponent);
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
      if (!((intfQos->pktprio.mask>>prio) & 1))  continue;
      
      // 802.1p trust mode
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        if (usmDbDot1dTrafficClassSet(1,intIfNum,prio,cos)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbDot1dTrafficClassSet (prio=%u => cos=%u)",prio,cos);
          rc = L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        if (usmDbQosCosMapIpPrecTrafficClassSet(1, intIfNum, prio, cos)!=L7_SUCCESS)
        { 
          LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassSet (IPprec=%u => cos=%u)",prio,cos); 
          rc = L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          /* Map 64 different priorities (6 bits) to 8 CoS */
          cos = ((intfQos->pktprio.cos[prio])>>(4*prio2)) & 0x07;

          if (usmDbQosCosMapIpDscpTrafficClassSet(1, intIfNum, prio*8+prio2, cos)!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassSet (DscpPrio=%u => CoS=%u)",prio*8+prio2,cos);
            rc = L7_FAILURE;
          }
          else
          {
            LOG_TRACE(LOG_CTX_PTIN_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc = L7_FAILURE;
      }
    }
  }

  if (rc==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_INTF, "QoS configuration successfully applied to ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error applying QoS configuration to ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }

  return rc;
}

/**
 * Read interface properties for QoS
 * 
 * @param intf : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_QoS_intf_config_get(ptin_intf_t *ptin_intf, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2;
  L7_uint32 intIfNum, value, cos;
  L7_RC_t   rc = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || intfQos==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Clear configurations to be returned */
  memset(intfQos,0x00,sizeof(ptin_QoS_intf_t));

  LOG_TRACE(LOG_CTX_PTIN_INTF,"Interface = %u/%u",ptin_intf->intf_type,ptin_intf->intf_id);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  // Get Trust mode
  if (usmDbQosCosMapTrustModeGet( 1, intIfNum, &value)!=L7_SUCCESS)
  {
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error with usmDbQosCosMapTrustModeGet");
    rc = L7_FAILURE;
  }
  else
  {
    trust_mode = value;
    intfQos->trust_mode = (L7_uint8) value;
    intfQos->mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
  }

  /* Shaping rate */
  if (usmDbQosCosQueueIntfShapingRateGet(1,intIfNum,&value)!=L7_SUCCESS)
  {  
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosQueueIntfShapingRateGet");
    rc = L7_FAILURE;
  }
  else
  {
    intfQos->shaping_rate = value;
    intfQos->mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
  }

  /* WRED decay exponent */
  if (usmDbQosCosQueueWredDecayExponentGet(1,intIfNum,&value)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosQueueWredDecayExponentGet");
    rc = L7_FAILURE;
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
        if (usmDbDot1dTrafficClassGet(1,intIfNum,prio,&cos)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbDot1dTrafficClassGet (prio=%u)",prio);
          rc = L7_FAILURE;
        }
        else
        {
          intfQos->pktprio.mask |= (L7_uint8) 1<<prio;
          intfQos->pktprio.cos[prio] = cos;
          LOG_TRACE(LOG_CTX_PTIN_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        if (usmDbQosCosMapIpPrecTrafficClassGet(1, intIfNum, prio, &cos)!=L7_SUCCESS)
        { 
          LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassGet (IPprec=%u)",prio);
          rc = L7_FAILURE;
        }
        else
        {
          intfQos->pktprio.mask |= (L7_uint8) 1<<prio;
          intfQos->pktprio.cos[prio] = cos;
          LOG_TRACE(LOG_CTX_PTIN_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          if (usmDbQosCosMapIpDscpTrafficClassGet(1, intIfNum, prio*8+prio2, &cos)!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassGet (DscpPrio=%u)",prio*8+prio2);
            rc = L7_FAILURE;
            break;
          }
          else
          {
            intfQos->pktprio.cos[prio] |= ((L7_uint32) cos & 0x0f)<<(prio2*4);
            LOG_TRACE(LOG_CTX_PTIN_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
        if (prio2>=8)
        {
          intfQos->pktprio.mask |= (L7_uint8) 1<<prio;
        }
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc = L7_FAILURE;
      }
    }

    /* Packet priority mask */
    if (intfQos->pktprio.mask)
    {
      intfQos->mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"Mask         = 0x%02x",intfQos->mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"TrustMode    = %u",intfQos->trust_mode);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"PrioMap.mask   =0x%02x",intfQos->pktprio.mask);
  LOG_TRACE(LOG_CTX_PTIN_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
            intfQos->pktprio.cos[0],
            intfQos->pktprio.cos[1],
            intfQos->pktprio.cos[2],
            intfQos->pktprio.cos[3],
            intfQos->pktprio.cos[4],
            intfQos->pktprio.cos[5],
            intfQos->pktprio.cos[6],
            intfQos->pktprio.cos[7]);

  if (rc==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_INTF, "QoS configuration successfully read from ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error reading QoS configuration from ptin_intf=%u/%u",ptin_intf->intf_type,ptin_intf->intf_id);
  }

  return rc;
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
L7_RC_t ptin_QoS_cos_config_set(ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, i, conf_index;
  L7_RC_t   rc = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || qosConf==L7_NULLPTR || (cos!=(L7_uint8)-1 && cos>7))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get current configurations */
  /* Scheduler type */
  if (usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, &schedType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading scheduler type");
    return L7_FAILURE;
  }
  /* Queue management type */
  if (usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, &mgmtType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading queue management type");
    return L7_FAILURE;
  }
  /* Minimum bandwidth */
  if (usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, &minBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading minimum bandwith");
    return L7_FAILURE;
  }
  /* Maximum bandwidth */
  if (usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, &maxBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading maximum bandwith");
    return L7_FAILURE;
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
      //LOG_WARNING(LOG_CTX_PTIN_INTF, "Empty mask: no configuration to be applied");
      continue;
    }

    /* Scheduler type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_SCHEDULER_MASK)
    {
      schedType_list.schedType[i] = qosConf[conf_index].scheduler_type;
      LOG_TRACE(LOG_CTX_PTIN_INTF,"Scheduler type in cos=%u, will be updated to %u",i,qosConf[conf_index].scheduler_type);
    }
    /* Management type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_QUEUE_MANGM_MASK)
    {
      mgmtType_list.mgmtType[i] = qosConf[conf_index].queue_management_type;
      LOG_TRACE(LOG_CTX_PTIN_INTF,"Queue management type in cos=%u, will be updated to %u",i,qosConf[conf_index].queue_management_type);
    }
    /* Minimum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MIN_MASK)
    {
      minBw_list.bandwidth[i] = qosConf[conf_index].min_bandwidth;
      LOG_TRACE(LOG_CTX_PTIN_INTF,"Minimum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].min_bandwidth);
    }
    /* Maximum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MAX_MASK)
    {
      maxBw_list.bandwidth[i] = qosConf[conf_index].max_bandwidth;
      LOG_TRACE(LOG_CTX_PTIN_INTF,"Maximum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].max_bandwidth);
    }
  }

  /* Apply new configurations */
  /* Scheduler type */
  if (usmDbQosCosQueueSchedulerTypeListSet(1, intIfNum, &schedType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error applying scheduler type");
    rc = L7_FAILURE;
  }
  /* Queue management type */
  if (usmDbQosCosQueueMgmtTypeListSet(1, intIfNum, &mgmtType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error applying queue management type");
    rc = L7_FAILURE;
  }
  /* Minimum bandwidth */
  if (usmDbQosCosQueueMinBandwidthListSet(1, intIfNum, &minBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error applying minimum bandwith");
    rc = L7_FAILURE;
  }
  /* Maximum bandwidth */
  if (usmDbQosCosQueueMaxBandwidthListSet(1, intIfNum, &maxBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error applying maximum bandwith");
    rc = L7_FAILURE;
  }

  /* Check result */
  if (rc==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_INTF, "QoS configuration successfully applied to ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error applying QoS configuration to ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }

  return rc;
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
L7_RC_t ptin_QoS_cos_config_get(ptin_intf_t *ptin_intf, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i;
  L7_RC_t   rc = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_intf==L7_NULLPTR || qosConf==L7_NULLPTR || (cos!=(L7_uint8)-1 && cos>7))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_INTF,"Interface=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);

  /* Validate interface */
  if (ptin_intf_ptintf2intIfNum(ptin_intf,&intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Interface %u/%u is intIfNum=%u",ptin_intf->intf_type,ptin_intf->intf_id,intIfNum);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_intf=%u/%u)",intIfNum,ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Scheduler type */
  if (usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, &schedType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading scheduler type");
    return L7_FAILURE;
  }
  /* Queue management type */
  if (usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, &mgmtType_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading queue management type");
    return L7_FAILURE;
  }
  /* Minimum bandwidth */
  if (usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, &minBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading minimum bandwith");
    return L7_FAILURE;
  }
  /* Maximum bandwidth */
  if (usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, &maxBw_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF,"Error reading maximum bandwith");
    return L7_FAILURE;
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
    LOG_TRACE(LOG_CTX_PTIN_INTF,"Scheduler type for cos=%u is %u",i,qosConf[conf_index].scheduler_type);

    qosConf[conf_index].queue_management_type = (L7_uint8) mgmtType_list.mgmtType[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_QUEUE_MANGM_MASK;
    LOG_TRACE(LOG_CTX_PTIN_INTF,"Queue mangement type for cos=%u is %u",i,qosConf[conf_index].queue_management_type);

    qosConf[conf_index].min_bandwidth = minBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MIN_MASK;
    LOG_TRACE(LOG_CTX_PTIN_INTF,"Minimum bandwith for cos=%u is %u",i,qosConf[conf_index].min_bandwidth);

    qosConf[conf_index].max_bandwidth = maxBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MAX_MASK;
    LOG_TRACE(LOG_CTX_PTIN_INTF,"Maximum bandwith for cos=%u is %u",i,qosConf[conf_index].max_bandwidth);
  }

  if (rc==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_INTF, "QoS configuration successfully read from ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Error reading QoS configuration from ptin_intf=%u/%u, cos=%u",ptin_intf->intf_type,ptin_intf->intf_id,cos);
  }

  return rc;
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
  L7_uint32 speed_mode;

  port = phyConf->Port;
  phyConf->Mask = 0;  /* Clear Mask */

  /* Validate port range */
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Port# %u is out of range [0..%u[", port, PTIN_SYSTEM_N_PORTS);
    return L7_FAILURE;
  }

  /* Get interface ID */
  ptin_intf_port2intIfNum(port, &intIfNum);
  LOG_TRACE(LOG_CTX_PTIN_INTF, "Port# %2u:     intIfNum# %2u", port, intIfNum);

  /* PortEnable */
  if (usmDbIfAdminStateGet(1, intIfNum, &value))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get enable state of port# %d", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0020;
    phyConf->PortEnable = value;
    LOG_TRACE(LOG_CTX_PTIN_INTF, " State:       %s", phyConf->PortEnable ? "Enabled":"Disabled");
  }

  /* MaxFrame */
  if (usmDbIfConfigMaxFrameSizeGet(intIfNum, &value))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get max frame value of port# %u", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0040;
    phyConf->MaxFrame = value;
    LOG_TRACE(LOG_CTX_PTIN_INTF, " MaxFrame:    %u", phyConf->MaxFrame);
  }

  /* Loopback */
  if (usmDbIfLoopbackModeGet(intIfNum, &value))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get loopback state of port# %u", port);
    return L7_FAILURE;
  }
  else
  {
    phyConf->Mask |= 0x0008;
    phyConf->LoopBack = value;
    LOG_TRACE(LOG_CTX_PTIN_INTF, " Loopback:    %s", phyConf->LoopBack ? "Enabled":"Disabled");
  }

  /* Speed */
  if (usmDbIfSpeedGet(1, intIfNum, &speed_mode))
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Failed to get speed of port# %u", port);
    return L7_FAILURE;
  }
  else {
    phyConf->Mask |= 0x0001;

    switch (speed_mode)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        phyConf->Speed = PHY_PORT_1000_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       1000Mbps");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
        phyConf->Speed = PHY_PORT_2500_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       2.5G");
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
        phyConf->Speed = PHY_PORT_10000_MBPS;
        LOG_TRACE(LOG_CTX_PTIN_INTF, " Speed:       10G");
        break;

      default:
        phyConf->Speed = -1;
    }
  }

  /* Full-Duplex */
  phyConf->Mask |= 0x0004;
  phyConf->Duplex = 1;
  LOG_TRACE(LOG_CTX_PTIN_INTF, " Full-Duplex: %s", phyConf->Duplex?"Enabled":"Disabled");

  /* Media */
  /* NOTE: it is assumed that the PON ports are mapped from port 0 to L7_SYSTEM_PON_PORTS-1 */
  phyConf->Mask |= 0x0002;
  if ( port < PTIN_SYSTEM_N_PONS ) {
    phyConf->Media = PHY_PORT_MEDIA_INTERNAL;
    LOG_TRACE(LOG_CTX_PTIN_INTF, " Media:       Internal");
  }
  else {
    phyConf->Media = PHY_PORT_MEDIA_OPTICAL;
    LOG_TRACE(LOG_CTX_PTIN_INTF, " Media:       Optical");
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

  /* Define default QoS configuration */
  memset(&qos_intf_cfg,0x00,sizeof(ptin_QoS_intf_t));
  qos_intf_cfg.mask         = PTIN_QOS_INTF_TRUSTMODE_MASK | PTIN_QOS_INTF_PACKETPRIO_MASK;
  qos_intf_cfg.trust_mode   = L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
  qos_intf_cfg.pktprio.mask = PTIN_QOS_INTF_PACKETPRIO_COS_MASK;
  /* Linear pbit->cos mapping */
  for (i=0; i<8; i++)
  {
    qos_intf_cfg.pktprio.cos[i] = i;
  }
  /* Strict scheduler */
  memset(&qos_cos_cfg,0x00,sizeof(ptin_QoS_cos_t));
  qos_cos_cfg.mask           = PTIN_QOS_COS_SCHEDULER_MASK;
  qos_cos_cfg.scheduler_type = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;

  /* Apply configurations to interface */
  if (ptin_QoS_intf_config_set(ptin_intf, &qos_intf_cfg)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_INTF, "Intf %u/%u: failed QoS initialization of interface", ptin_intf->intf_type,ptin_intf->intf_id);
    rc = L7_FAILURE;
  }
  /* Apply configurations to CoS */
  for (i=0; i<8; i++)
  {
    if (ptin_QoS_cos_config_set (ptin_intf, i, &qos_cos_cfg)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_INTF, "Intf %u/%u: failed QoS initialization of CoS=%u", ptin_intf->intf_type,ptin_intf->intf_id, i);
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
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting PRBS enable of intIfNum %u to %u",intIfNum, enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable of intIfNum %u to %u",intIfNum,enable);

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
    LOG_ERR(LOG_CTX_PTIN_API,"Error getting PRBS errors of intIfNum %u",intIfNum);
    return rc;
  }

  /* Return result */
  if (counter!=L7_NULLPTR)
  {
    *counter = dapiCmd.cmdData.prbsStatus.rxErrors;
  }

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
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u is out of range [1..%u]", lag_idx, PTIN_SYSTEM_N_LAGS);
    return L7_FAILURE;
  }

  /* Get LAG intIfNum */
  ptin_intf_lag2intf(lag_idx, &intIfNum); /* No error checking is necessary because
                                           * lag_idx was already validated */

  /* LAG admin */
  if (usmDbDot3adAdminModeGet(1, intIfNum, &value) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading admin field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->admin = value;

  /* LAG STP Mode */
  if (usmDbDot1sPortStateGet(1, intIfNum, &value) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading Dot1sPortState field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->stp_enable = value;

  /* LAG Static Mode */
  if (usmDbDot3adIsStaticLag(1, intIfNum, &value) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading static mode field", lag_idx);
    return L7_FAILURE;
  }
  lagInfo->static_enable = value;

  /* LAG Balance Mode */
  if (usmDbDot3adLagHashModeGet(1, intIfNum, &value) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_PTIN_INTF, "LAG# %u: error reading balance mode field", lag_idx);
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
