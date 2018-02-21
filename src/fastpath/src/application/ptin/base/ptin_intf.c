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

#include "dtlapi.h"
#include "ptin_include.h"
#include "ptin_control.h"
#include "ptin_intf.h"
#include "ptin_cnfgr.h"
#include "fw_shm.h"
#include "ptin_fpga_api.h"
#include "ptin_msg.h"

#ifdef NGPON2_SUPPORTED 
/*   */
static ptin_NGPON2_groups_t NGPON2_groups_info[PTIN_SYSTEM_MAX_NGPON2_GROUPS];
#endif

L7_BOOL linkscan_update_control = L7_TRUE;
void ptin_linkscan_control_global(L7_BOOL enable)
{
  printf("Linkscan management is not active for this board\r\n");
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

#ifdef NGPON2_SUPPORTED

#define NGPON2_EMPTY_ENTRY     0xff

#define NGPON2_PORT_ADD(var, n)  ( var |= (1LL << n))
#define NGPON2_PORT_REM(var, n)  ( var &= ~(1LL << n))
#define NGPON2_BIT_PORT(var)     ( var & 1LL )
                               
#endif                               

/**************************************/

/* MAX interface rate to limit shaping (percentage value) */
/* Index 1: Effective configuration from manager */
/* Index 2: Max shaper value */
#define PTIN_INTF_SHAPER_MNG_VALUE  0
#define PTIN_INTF_SHAPER_MAX_VALUE  1
#define PTIN_INTF_FEC_VALUE         2
L7_uint32 ptin_intf_shaper_max[PTIN_SYSTEM_N_INTERF][3];
L7_uint32 ptin_burst_size[PTIN_SYSTEM_N_INTERF];

#define MAX_BURST_SIZE 16000

//ptin_intf_shaper_t shaper_max_burst[PTIN_SYSTEM_N_INTERF];

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
//static L7_RC_t ptin_intf_QoS_init(ptin_intf_t *ptin_intf);

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
#ifdef NGPON2_SUPPORTED
  memset(NGPON2_groups_info,  0x00, sizeof(NGPON2_groups_info));
#endif

  /* For all interfaces, max rate is 100% */
  for (i = 0; i < PTIN_SYSTEM_N_INTERF; i++)
  {
    ptin_intf_shaper_max[i][PTIN_INTF_SHAPER_MNG_VALUE] = 100;   /* Shaper value from management */
    ptin_intf_shaper_max[i][PTIN_INTF_SHAPER_MAX_VALUE] = 100; /* Max. Shaper value */
    ptin_intf_shaper_max[i][PTIN_INTF_FEC_VALUE]        = 100; /* FEC value value */
    ptin_burst_size[i] = MAX_BURST_SIZE; //default bcm value for port max burst rate

  }
  
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
    PT_LOG_INFO(LOG_CTX_INTF, "Initializing port %u", i);

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

    rc = usmDbDvlantagIntfModeSet(1, map_port2intIfNum[i], L7_ENABLE);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to enable DVLAN mode on port# %u", i);
      return L7_FAILURE;
    }

    #if 0
    rc = usmDbDvlantagIntfEthertypeSet(1, map_port2intIfNum[i], PTIN_TPID_OUTER_DEFAULT, L7_TRUE);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      PT_LOG_CRITIC(LOG_CTX_INTF, "Failed to configure default TPID 0x%04X on port# %u (rc = %d)", PTIN_TPID_OUTER_DEFAULT, i, rc);
      return L7_FAILURE;
    }
    #endif

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
  #if (!PTIN_BOARD_IS_DNX)
    if (ptin_intf_QoS_init(&ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Phy# %u: Error initializing QoS definitions",i);
      return L7_FAILURE;
    }
  #endif
  }

  /* Initialize phy conf structure (must be run after default configurations!) */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    phyConf_data[i].Port = i;
    if (ptin_intf_PhyConfig_read(&phyConf_data[i]) != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed reading PHY config");
      return L7_FAILURE;
    }
  }

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
    {
      if (usmDbIfAdminStateGet(1, intIfNum, &value) == L7_SUCCESS && 
          (value != phyConf->PortEnable))
      {
        if (usmDbIfAdminStateSet(1, intIfNum, phyConf->PortEnable & 1) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Failed to set enable state on port# %u", port);
          return L7_FAILURE;
        }

        phyConf_data[port].PortEnable = phyConf->PortEnable & 1; /* update buffered conf data */
        PT_LOG_TRACE(LOG_CTX_INTF, " State:       %s", phyConf->PortEnable ? "Enabled":"Disabled");
      }
    }
  }
  
  /* MaxFrame */
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
        speed_mode = L7_PORTCTRL_PORTSPEED_FULL_40G;
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
      L7_BOOL apply_speed = L7_TRUE;

      if (apply_speed)
      {
        if (usmDbIfSpeedSet(1, intIfNum, speed_mode) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Failed to set port speed on port# %u", port);
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_WARN(LOG_CTX_INTF, "port %u: New speed mode (%u) was stored, but not applied", port, speed_mode);
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

  if (cnfgrBaseTechnologyTypeGet() != L7_BASE_TECHNOLOGY_TYPE_BROADCOM_DNX)
  {
    /* Always clear counters after a reconfiguration */
    if (ptin_intf_counters_clear(port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to clear counters on port# %u", port);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, " Counters:    Cleared!");
  }

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
      case L7_PORTCTRL_PORTSPEED_FULL_40G:
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
 * @author mruas (07/09/17)
 * 
 * @param ptin_port 
 * 
 * @return L7_int : board_id or -1 if error
 */
L7_int ptin_intf_board(L7_int ptin_port)
{
  L7_uint16 board_id = 0;

  if (ptin_intf_boardid_get(ptin_port,  &board_id) != L7_SUCCESS)
  {
    return -1;
  }

  return (L7_int) board_id;
}

/**
 * Get board id for a particular slot
 * 
 * @author mruas (07/09/17)
 * 
 * @param slot_id 
 * 
 * @return L7_int : board_id or -1 if error
 */
L7_int ptin_slot_board(L7_int slot_id)
{
  L7_uint16 board_id = 0;

  if (ptin_slot_boardid_get(slot_id,  &board_id) != L7_SUCCESS)
  {
    return -1;
  }

  return (L7_int) board_id;
}


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

  return L7_FAILURE;
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

  return L7_FAILURE;
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

  return L7_SUCCESS;
}

L7_RC_t ptin_intf_boardtype_dump(void)
{
  /* Only applied to CXO640G boards */
  printf("Not supported!\r\n");

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
  /* Validate ptin_port */
  if (ptin_port >= ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  if (slot_ret  != L7_NULLPTR)  *slot_ret   = 0;
  if (port_ret  != L7_NULLPTR)  *port_ret   = ptin_port;
  if (board_type!= L7_NULLPTR)  *board_type = 0;

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
  return L7_FALSE;
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
  return L7_FALSE;
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
  L7_uint32 ptin_port, intIfNum;
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
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port) != L7_SUCCESS ||
      ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid", ptin_intf->intf_type, ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is ptin_port %u, intIfNum=%u", ptin_intf->intf_type, ptin_intf->intf_id, ptin_port, intIfNum);

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
    PT_LOG_NOTICE(LOG_CTX_INTF, "New shaping rate is %u", intfQos->shaping_rate);

    if(intfQos->shaping_rate == 0)
    {
      intfQos->shaping_rate = 100;
    }

    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = %u",ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE]);
    PT_LOG_TRACE(LOG_CTX_INTF, "intfQos->shaping_rate = %u",intfQos->shaping_rate);

    //rc = usmDbQosCosQueueIntfShapingRateSet(1, intIfNum, (intfQos->shaping_rate * ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE])/100);
    ptin_intf_shaper_t   entry;

    memset(&entry, 0x00, sizeof(ptin_intf_shaper_t));

    entry.ptin_port  = ptin_port;
    entry.burst_size = ptin_burst_size[ptin_port]; 

		if (intfQos->shaping_rate <= (ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE]))
		{
      entry.max_rate   = intfQos->shaping_rate;
		}
    else
    {
      entry.max_rate   = ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE];
    }

    PT_LOG_NOTICE(LOG_CTX_INTF, "ptin_port:  %u", entry.ptin_port);
    PT_LOG_NOTICE(LOG_CTX_INTF, "burst_size: %u", entry.burst_size);
    PT_LOG_NOTICE(LOG_CTX_INTF, "max_rate:   %u", entry.max_rate);

    dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_MAX_BURST, DAPI_CMD_SET, sizeof(ptin_intf_shaper_t), &entry);

    if (rc == L7_SUCCESS)
    {
      ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE] = intfQos->shaping_rate;
      ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = entry.max_rate;
      PT_LOG_NOTICE(LOG_CTX_INTF, "New shaping rate is %u",intfQos->shaping_rate);
    }
    else
    {  
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet (rc=%d)", rc);
      rc_global = rc;
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
  L7_uint32 ptin_port, intIfNum, value, cos;
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
  if (ptin_intf_ptintf2port(ptin_intf, &ptin_port) != L7_SUCCESS ||
      ptin_intf_ptintf2intIfNum(ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid",ptin_intf->intf_type,ptin_intf->intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is ptin_port %u, intIfNum=%u", ptin_intf->intf_type, ptin_intf->intf_id, ptin_port, intIfNum);

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
  rc = usmDbQosCosQueueIntfShapingRateGet(1, intIfNum, &value);
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
      case L7_PORTCTRL_PORTSPEED_FULL_40G:
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
 * Apply linkfaults enable procedure
 *  
 * @param intIfNum : Interface
 * @param local_enable : Local faults processing enable 
 * @param remote_enable : Remote faults processing enable 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_intf_linkfaults_enable(L7_uint32 intIfNum, L7_BOOL local_enable, L7_BOOL remote_enable)
{
  ptin_hwproc_t hw_proc;
  L7_INTF_TYPES_t sysIntfType;
  L7_RC_t   rc = L7_SUCCESS;

  /* Validate interface */
  if (intIfNum == 0 || intIfNum > L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  memset(&hw_proc,0x00,sizeof(hw_proc));

  hw_proc.operation = DAPI_CMD_SET;
  hw_proc.procedure = PTIN_HWPROC_LINKFAULTS_ENABLE;
  hw_proc.mask = 0xff;
  hw_proc.param1 = (L7_int32) local_enable;
  hw_proc.param2 = (L7_int32) remote_enable;

  /* Get interface type */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error obtaining interface type for intIfNum=%u", intIfNum);
    return L7_FAILURE;
  }
  
  /* Physical interface? */
  if (sysIntfType == L7_PHYSICAL_INTF)
  {
    /* Apply procedure */
    rc = dtlPtinHwProc(intIfNum, &hw_proc);
  }
  /* LAG interface? */
  else if (sysIntfType == L7_LAG_INTF)
  {
    L7_uint32 i, number_of_members = PTIN_SYSTEM_N_PORTS;
    L7_uint32 memberList[PTIN_SYSTEM_N_PORTS];

    /* Get LAG members */
    if (usmDbDot3adMemberListGet(1, intIfNum, &number_of_members, memberList) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error obtaining list of members for intIfNum=%u", intIfNum);
      return L7_FAILURE;
    }

    /* Apply procedure  to all LAG members */
    for (i = 0; i < number_of_members; i++)
    {
      rc = dtlPtinHwProc(memberList[i], &hw_proc);
      if (rc != L7_SUCCESS)  break;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Unknown interface type (%u) for intIfNum=%u", sysIntfType, intIfNum);
    return L7_FAILURE;
  }
  
  /* Check return value */
  if (rc == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF,"HW procedure applied to intIfNum=%u", intIfNum);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying HW procedure to intIfNum=%u", intIfNum);
  }

  return rc;
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

  PT_LOG_INFO(LOG_CTX_INTF,"HW-RESET procedure applied to slot %d", slot_id);

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
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying HW procedure to intIfNum=%u", intIfNum);
  }
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

  PT_LOG_DEBUG(LOG_CTX_INTF,"HW-Linkscan procedure applied to intIfNum=%u", intIfNum);

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

  PT_LOG_DEBUG(LOG_CTX_INTF,"Force link to %u, applied to intIfNum=%u", enable, intIfNum);

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
  return L7_SUCCESS;
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
 return L7_SUCCESS;
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

  los = pfw_shm->intf[ptin_port].port_state & 1;

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

 return 0;
}


/*****************************************************************************************************************/
/*                                                                                                               */  
/*                                                NGPON2                                                         */ 
/*                                                                                                               */ 
/*****************************************************************************************************************/
 
#ifdef NGPON2_SUPPORTED
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

  group_idx = group_info->GroupId ; // internal index

    /* check if the group already exists */
  if (ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "NGPON2 Group already exists");
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
  L7_uint8 i = 0;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId; // internal index

  /* check if the group already exists */
  if (!ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "NGPON2 Group does not exist!");
    return L7_SUCCESS;;
  }

  /* Remove all ports from the group */
  while ( i < NGPON2_groups_info[group_idx].nports) // group_info->numIntf 
  {    
    NGPON2_PORT_REM(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64, i); //group_info->NGPON2Port[i].id
    i++;
  }

  /* group is not active */
  NGPON2_groups_info[group_idx].admin   = 0;
  NGPON2_groups_info[group_idx].nports  = 0;
  NGPON2_groups_info[group_idx].groupId = NGPON2_EMPTY_ENTRY;

  return L7_SUCCESS;
}


/**
 * PTIN_INTF NGPON2 clear 
 * 
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_intf_NGPON2_clear()
{
  /* Reset defaults */
  memset(NGPON2_groups_info,  0x00, sizeof(NGPON2_groups_info));

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

  L7_uint32 group_idx, src_port= -1;
  L7_uint8 i = 0, slot,new_group = 0;

  /* Validate arguments */
  if (group_info == L7_NULLPTR || group_info->numIntf  == 0)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId; // internal index
  PT_LOG_TRACE(LOG_CTX_INTF, "NGPON2 Group ID %d have %d interfaces",group_idx, group_info->numIntf);

  /* Add port to NGPON2 port bmp (in this moment group_info->numIntf is always one) */
  for(i = 0; i < group_info->numIntf; i++)
  { 
    ptin_intf_slot_get(&slot);

#if !PTIN_BOARD_IS_MATRIX
    if(group_info->NGPON2Port[i].slot != slot)
    {
      continue;
    }
#endif

    /* check if the group exists */
    if (!ptin_intf_NGPON2_group_exists(group_idx))
    {
      PT_LOG_ERR(LOG_CTX_INTF, "NGPON2 Group does not exist!");     
      /* Check if is a new group */
#if !PTIN_BOARD_IS_MATRIX
      if(group_info->NGPON2Port[i].slot == slot)
      {
        new_group = 1; 
        NGPON2_groups_info[group_idx].admin = 1; //Enable group
        PT_LOG_WARN(LOG_CTX_INTF, "Enabling group!");
      }
#else
      new_group = 1; 
      PT_LOG_NOTICE(LOG_CTX_INTF, "Enabling group!");
      NGPON2_groups_info[group_idx].admin = 1; //Enable group
#endif
    }

    /* update port bitmap */
    if(!NGPON2_BIT_PORT(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64 >>group_info->NGPON2Port[i].id))
    {
     /* set portId to the NGPON2 group */
     NGPON2_PORT_ADD(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64,(group_info->NGPON2Port[i].id));
     NGPON2_groups_info[group_idx].nports++;
    }

    /* increment number of ports for this group */  
    PT_LOG_TRACE(LOG_CTX_INTF, "GROUP Id %d have %d ports",group_idx, NGPON2_groups_info[group_idx].nports);

    L7_uint8 temp = 0;

    /* Find a already configured port in the NGPON2 to replicate configuration to newly added port(s)*/
    while(temp < 64) // bit map size
    {
      if  (NGPON2_BIT_PORT(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64 >> temp))
      {    
        /*Find a already configured NGPON2 port to replicate the configurations*/
        if( temp != group_info->NGPON2Port[i].id )
        {
          src_port = temp;
          break;
        }    
      }
     temp++;
    }
  }

  PT_LOG_NOTICE(LOG_CTX_INTF, "Number of ports %d !", NGPON2_groups_info[group_idx].nports);

  /* Avoid mistakes in configurations */
  if(NGPON2_groups_info[group_idx].nports == 0)
  {
    PT_LOG_NOTICE(LOG_CTX_INTF, "Disable group!");
    NGPON2_groups_info[group_idx].admin = 0; 
  }
    
  for(i = 0; ((i < group_info->numIntf) && NGPON2_groups_info[group_idx].admin); i++)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Replicating configuration from port %d to %d !", src_port, group_info->NGPON2Port[i].id);

#if !PTIN_BOARD_IS_MATRIX
    if (group_info->NGPON2Port[i].slot != slot)
    {
      continue;
    }
#endif
    if ( new_group )
    {
      /* if is a new group replicate all the offline configuration from this group*/
      ptin_msg_apply_ngpon2_configuration(group_info->GroupId);
    }
    else if ( (src_port != group_info->NGPON2Port[i].id) && (NGPON2_groups_info[group_idx].nports > 1) )
    {
      /* If the group already exists replicate configuration from a already configured port*/
      if( ptin_msg_replicate_port_configuration((group_info->NGPON2Port[i].id), src_port, group_info->GroupId ) != L7_SUCCESS)
      {
        /* Remove port form port bitmap */
        NGPON2_PORT_REM(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64, (group_info->NGPON2Port[i].id));
        NGPON2_groups_info[group_idx].nports--;

        PT_LOG_ERR(LOG_CTX_INTF, "Error replicating configuration from port %d to %d !", src_port, group_info->NGPON2Port[i].id);
        PT_LOG_ERR(LOG_CTX_INTF, "NGPON2_groups_info[group_idx].nports %d !",            NGPON2_groups_info[group_idx].nports);
        return L7_SUCCESS;/* Do not return error to the manager*/
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "Success replication configurations from port %d to %d !", src_port, group_info->NGPON2Port[i].id);

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
  L7_uint8  i = 0,slot;

  /* Validate arguments */
  if (group_info == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid arguments");
    return L7_FAILURE;
  }

  group_idx = group_info->GroupId; // internal index

  /* check if the group already exists */
  if (!ptin_intf_NGPON2_group_exists(group_idx))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "NGPON2 Group does not exist! %d ",group_idx);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "Going to remove configuration from port %d ", group_info->NGPON2Port[i].id);
  PT_LOG_TRACE(LOG_CTX_INTF, "group_info->numIntf %d ", group_info->numIntf);

  for (i=0;i < group_info->numIntf; i++)
  {
    ptin_intf_slot_get(&slot);

#if !PTIN_BOARD_IS_MATRIX
    if(group_info->NGPON2Port[i].slot != slot)
    {
      continue;
    }
#endif

    NGPON2_PORT_REM(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64,(group_info->NGPON2Port[i].id));

    /* count number of ports for this group */
    L7_uint8 n_ports = 0, temp = 0;

    while(temp < 64)
    {
      if(NGPON2_BIT_PORT(NGPON2_groups_info[group_idx].ngpon2_groups_pbmp64 >> temp))
      {
        n_ports++;
      }
      temp++;
    }

    PT_LOG_TRACE(LOG_CTX_INTF, "Going to remove configuration from port %d from slot %d ", group_info->NGPON2Port[i].id, group_info->NGPON2Port[i].slot);

    if(ptin_msg_remove_port_configuration(group_info->NGPON2Port[i].id, group_info->GroupId) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error remove configuration from port %d", group_info->NGPON2Port[i].id);
      return L7_FAILURE;
    }
 
    NGPON2_groups_info[group_idx].nports = n_ports;
    PT_LOG_TRACE(LOG_CTX_INTF, "NGPON2_groups_info[group_idx].nports %d", NGPON2_groups_info[group_idx].nports);

    if(NGPON2_groups_info[group_idx].nports == 0)
    {
      NGPON2_groups_info[group_idx].admin = 0;
    }
  }
  return L7_SUCCESS;
}


/**
 * PTIN_INTF get NGPON2 group info 
 * 
 * @param group_info      : Pointer to struct with group info 
 * @param group_index     : NGPON2 group index                  
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t get_NGPON2_group_info(ptin_NGPON2_groups_t *group_info, L7_uint8 group_index)
{
  L7_uint32 i=0;

  L7_uint8 index = group_index; 

  group_info->admin                   = NGPON2_groups_info[index].admin;
  group_info->groupId                 = NGPON2_groups_info[index].groupId;
  group_info->nports                  = NGPON2_groups_info[index].nports;
  group_info->ngpon2_groups_pbmp64    = NGPON2_groups_info[index].ngpon2_groups_pbmp64;
  group_info->number_services         = NGPON2_groups_info[index].number_services;

  PT_LOG_TRACE(LOG_CTX_MSG, "group_info->number_services %d", group_info->number_services );
  PT_LOG_TRACE(LOG_CTX_MSG, "group_info->nports          %d", group_info->nports );
  PT_LOG_TRACE(LOG_CTX_MSG, "group_info->admin           %d", group_info->admin );

  for(i=0; i< ((PTIN_SYSTEM_N_EVCS)/(8*sizeof(L7_uint8))); i++)
  {
    group_info->evc_groups_pbmp[i]    = NGPON2_groups_info[index].evc_groups_pbmp[i];
  }

  return L7_SUCCESS;
}

/**
 * PTIN_INTF set NGPON2 group info 
 * 
 * @param group_info      : Pointer to struct with group info 
 * @param group_index     : NGPON2 group index                  
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t set_NGPON2_group_info(ptin_NGPON2_groups_t *group_info, L7_uint8 group_index)
{
  L7_uint8  groupIndex = group_index ; // internal index
  NGPON2_groups_info[groupIndex].number_services = group_info->number_services;
  //NGPON2_groups_info[groupIndex].evc_groups_pbmp = group_info->evc_groups_pbmp;
  PT_LOG_NOTICE(LOG_CTX_MSG, "Save NGPON2 configuration ");

  memcpy(NGPON2_groups_info[groupIndex].evc_groups_pbmp, group_info->evc_groups_pbmp, sizeof(NGPON2_groups_info[groupIndex].evc_groups_pbmp));


  return L7_SUCCESS;
}

/**
 * PTIN_INTF NGPON2 check intf
 *  
 * @brief check if a physical port belongs to a NGPON2 group 
 *  
 */
L7_RC_t ptin_intf_NGPON2_group_check(L7_uint8 intf_index, L7_uint8 *group_index)
{
  L7_uint16 i = 0;

  while ( i < PTIN_SYSTEM_MAX_NGPON2_GROUPS)
  {
    if (NGPON2_groups_info[i].admin)
    {
      if ( (NGPON2_groups_info[i].ngpon2_groups_pbmp64 >> intf_index) & 0x1)
      {
        *group_index = NGPON2_groups_info[i].groupId;
        return L7_SUCCESS; 
      }
    }
    i++;
  }

  *group_index = -1;

  return L7_FAILURE;
}

/**
 * PTIN_INTF NGPON2 groups dump
 *  
 * @brief dump all ngpon2 active groups and their related ports 
 *  
 */
void ptin_intf_NGPON2_groups_dump(void)
{
#ifdef NGPON2_SUPPORTED
  L7_uint16 i = 0;

  printf("Active groups:\n");

  while ( i < PTIN_SYSTEM_MAX_NGPON2_GROUPS)
  {
    if (NGPON2_groups_info[i].admin)
    {
      printf("GroupID: %u -- Ports Bitmap: %llu -- Number of ports: %u N of services --%d \n", NGPON2_groups_info[i].groupId, NGPON2_groups_info[i].ngpon2_groups_pbmp64, NGPON2_groups_info[i].nports, NGPON2_groups_info[i].number_services);
    }
    i++;
  }
#else
  printf("Not supported on this board!\r\n");
#endif
}

#endif /*NGPON2_SUPPORTED*/


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
 * Get the maximum bandwidth associated to a interface (physical
 * or LAG) 
 * 
 * @param intIfNum 
 * @param bandwidth : bandwidth in Kbps 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_ERROR
 */
L7_RC_t ptin_intf_max_bandwidth(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  L7_INTF_TYPES_t intf_type;
  L7_uint32 intf_speed, total_speed = 0;
  L7_uint32 i, number_of_ports, ports_list[PTIN_SYSTEM_N_LAGS];

  /* Validate intIfNum */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  /* Only process physical and logical interfaces */
  if (nimGetIntfType(intIfNum, &intf_type) != L7_SUCCESS ||
      (intf_type != L7_PHYSICAL_INTF && intf_type != L7_LAG_INTF))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Only physical and logical interfaces are allowed");
    return L7_FAILURE;
  }

  if (intf_type == L7_LAG_INTF)
  {
    number_of_ports = PTIN_SYSTEM_N_LAGS;
    if (usmDbDot3adMemberListGet(0, intIfNum, &number_of_ports, ports_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Can't get members list of intIfNum %u", intIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    number_of_ports = 1;
    ports_list[0] = intIfNum;
  }

  total_speed = 0;
  for (i = 0; i < number_of_ports; i++)
  {
    if (nimGetIntfSpeedStatus(ports_list[i], &intf_speed) == L7_SUCCESS)
    {
      switch (intf_speed)
      {
        case L7_PORTCTRL_PORTSPEED_HALF_10T:
        case L7_PORTCTRL_PORTSPEED_FULL_10T:
          total_speed += 10;
          break;
        case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100FX:
          total_speed += 100;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
          total_speed += 1000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
          total_speed += 2500;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
          total_speed += 10000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_40G:
          total_speed += 40000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
          total_speed += 100000;
          break;
        case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        case L7_PORTCTRL_PORTSPEED_UNKNOWN:
        default:
          break;
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Maximum bandwidth: %u", total_speed);

  if (bandwidth != L7_NULLPTR)
    *bandwidth = total_speed;

  return L7_SUCCESS;
}

/**
 * Get the AVAILABLE bandwidth of an interface (physical or LAG)
 * 
 * @param intIfNum 
 * @param bandwidth : bandwidth in Kbps 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_ERROR
 */
L7_RC_t ptin_intf_active_bandwidth(L7_uint32 intIfNum, L7_uint32 *bandwidth)
{
  L7_INTF_TYPES_t intf_type;
  L7_uint32 intf_speed, total_speed = 0;
  L7_uint32 i, number_of_ports = 0, ports_list[PTIN_SYSTEM_N_LAGS];

  /* Validate intIfNum */
  if (intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid intIfNum %u", intIfNum);
    return L7_FAILURE;
  }

  /* Only process physical and logical interfaces */
  if (nimGetIntfType(intIfNum, &intf_type) != L7_SUCCESS ||
      (intf_type != L7_PHYSICAL_INTF && intf_type != L7_LAG_INTF))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Only physical and logical interfaces are allowed");
    return L7_FAILURE;
  }

  if (intf_type == L7_LAG_INTF)
  {
    number_of_ports = PTIN_SYSTEM_N_LAGS;
    if (usmDbDot3adActiveMemberListGet(0, intIfNum, &number_of_ports, ports_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Can't get members list of intIfNum %u", intIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    L7_uint32 admin_state, link_state;

    if (nimGetIntfAdminState(intIfNum, &admin_state) == L7_SUCCESS && nimGetIntfLinkState(intIfNum, &link_state) == L7_SUCCESS)
    {
      if (admin_state == L7_ENABLE && link_state == L7_UP)
      {
        number_of_ports = 1; 
        ports_list[0] = intIfNum;
      }
      else
      {
        number_of_ports = 0;
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_INTF,"Error reading interface state");
      return L7_FAILURE;
    }
  }

  total_speed = 0;
  for (i = 0; i < number_of_ports; i++)
  {
    if (nimGetIntfSpeedStatus(ports_list[i], &intf_speed) == L7_SUCCESS)
    {
      switch (intf_speed)
      {
        case L7_PORTCTRL_PORTSPEED_HALF_10T:
        case L7_PORTCTRL_PORTSPEED_FULL_10T:
          total_speed += 10;
          break;
        case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100FX:
          total_speed += 100;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
          total_speed += 1000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
          total_speed += 2500;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
          total_speed += 10000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_40G:
          total_speed += 40000;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
          total_speed += 100000;
          break;
        case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
        case L7_PORTCTRL_PORTSPEED_UNKNOWN:
        default:
          break;
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Available bandwidth: %u", total_speed);

  if (bandwidth != L7_NULLPTR)
    *bandwidth = total_speed;

  return L7_SUCCESS;
}


/**
 * Set the maximum rate for a port
 * 
 * @author mruas (16/08/17)
 * 
 * @param intf_type 
 * @param intf_id 
 * @param max_rate : Percentage
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_shaper_max_set(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 max_rate, L7_uint32 burst_size)
{
  L7_uint32 ptin_port, intIfNum;
  ptin_intf_shaper_t   entry;

  /* Validate interface */
  if (ptin_intf_typeId2port(intf_type, intf_id, &ptin_port) != L7_SUCCESS ||
      ptin_intf_typeId2intIfNum(intf_type, intf_id, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid", intf_type, intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is ptin_port %u, intIfNum %u", intf_type, intf_id, ptin_port, intIfNum);

  /* Limit max rate */
  if (max_rate > 100)
  {
    max_rate = 100;
  }

  /* Limit max rate */
  if (burst_size == 0)
  {
    burst_size = MAX_BURST_SIZE;
  }

  memset(&entry, 0x00, sizeof(ptin_intf_shaper_t));

  entry.ptin_port  = ptin_port;
  entry.burst_size = burst_size;
  
  if (ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE] <= max_rate)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port:  %u", entry.ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "burst_size: %u", entry.burst_size);
    PT_LOG_TRACE(LOG_CTX_INTF, "max_rate:   %u", entry.max_rate);

    entry.max_rate   = ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE];
    ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE];

    dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_MAX_BURST, DAPI_CMD_SET, sizeof(ptin_intf_shaper_t), &entry);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port:  %u", entry.ptin_port);
    PT_LOG_TRACE(LOG_CTX_INTF, "burst_size: %u", entry.burst_size);
    PT_LOG_TRACE(LOG_CTX_INTF, "max_rate:   %u", entry.max_rate);

    entry.max_rate   = max_rate;
    ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = max_rate;

    dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_MAX_BURST, DAPI_CMD_SET, sizeof(ptin_intf_shaper_t), &entry);
  }

  /* Save max rate for this interface */
  ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE] = max_rate;
  ptin_burst_size[ptin_port] = burst_size;

  return L7_SUCCESS;
}

/**
 * Get the maximum rate for a port
 * 
 * @author mruas (16/08/17)
 * 
 * @param intf_type 
 * @param intf_id 
 * @param max_rate : Percentage
 * @param eff_max_rate : Percentage
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_intf_shaper_max_get(L7_uint8 intf_type, L7_uint8 intf_id, L7_uint32 *max_rate, L7_uint32 *eff_max_rate, L7_uint32 *burst_size)
{
  L7_uint32 ptin_port, intIfNum;
  ptin_intf_shaper_t   entry;

  /* Validate interface */
  if (ptin_intf_typeId2port(intf_type, intf_id, &ptin_port) != L7_SUCCESS ||
      ptin_intf_typeId2intIfNum(intf_type, intf_id, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Interface %u/%u invalid", intf_type, intf_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "Interface %u/%u is ptin_port %u, intIfNum=%u => max_rate=%u",
               intf_type, intf_id, ptin_port, intIfNum, ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE]);

  /* Limit max rate */
  if (max_rate != L7_NULLPTR)
  {
    *max_rate = ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE];
  }

  /* Effective max rate */
  if (eff_max_rate != L7_NULLPTR)
  {
    if (usmDbQosCosQueueIntfShapingRateGet(1, intIfNum, eff_max_rate) != L7_SUCCESS)
    {
      *eff_max_rate = 0;
    }
  }

  memset(&entry, 0x00, sizeof(ptin_intf_shaper_t));

  entry.ptin_port  = ptin_port;

  dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_MAX_BURST_GET, DAPI_CMD_GET, sizeof(ptin_intf_shaper_t), &entry);

  *burst_size = entry.burst_size;

  PT_LOG_TRACE(LOG_CTX_INTF, "burst_size: %u", *burst_size);

  /* Save the read value (burst_size) if it's different from the value stored on ptin_burst_size[ptin_port] */
  if (ptin_burst_size[ptin_port] != *burst_size)
  {
    ptin_burst_size[ptin_port] = *burst_size;
  }

  return L7_SUCCESS;
}

/**
 * Dump the maximum rate for all interfaces
 * 
 * @author mruas (16/08/17)
 */
void ptin_intf_shaper_max_dump(void)
{
  L7_uint32 port, intIfNum, shaper_rate;
  L7_RC_t rc;

  printf("------------------------------------------------\r\n");
  printf("| Port | Max rate | Mng max | Eff max | Burst   |\r\n");
  for (port = 0; port < PTIN_SYSTEM_N_PORTS; port++)
  {
    if (ptin_intf_port2intIfNum(port, &intIfNum) == L7_SUCCESS)
    {
      rc = usmDbQosCosQueueIntfShapingRateGet(1, intIfNum, &shaper_rate);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateGet: rc=%d", rc);
        shaper_rate = 0;
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u to intIfNum", port);
      shaper_rate = 0;
    }

    printf("|  %2u  |    %3u   |   %3u   |   %3u   |   %3u   |\r\n", port,
           ptin_intf_shaper_max[port][PTIN_INTF_SHAPER_MAX_VALUE], ptin_intf_shaper_max[port][PTIN_INTF_SHAPER_MNG_VALUE], shaper_rate, ptin_burst_size[port]);
  }
  printf("------------------------------------------------\r\n");

}

