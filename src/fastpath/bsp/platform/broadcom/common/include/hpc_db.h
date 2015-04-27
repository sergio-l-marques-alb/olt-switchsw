/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename hpc_db.h
*
* @purpose HPC component's internal data types and functions
*
* @component hpc
*
* @comments none
*
* @create 02/16/2003
*
* @author jeffr
* @end
*
**********************************************************************/
#ifndef INCLUDE_HPC_DB
#define INCLUDE_HPC_DB

#include "l7_common.h"
#include <bcm/types.h>


#define L7_PORT_DESC_BCOM_XAUI_10G       L7_IANA_10G_ETHERNET,              \
                                         L7_PORTCTRL_PORTSPEED_FULL_10GSX,  \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10G),   \
                                         L7_XAUI

#define L7_PORT_DESC_BCOM_XAUI_10G_NO_AN L7_IANA_10G_ETHERNET,              \
                                         L7_PORTCTRL_PORTSPEED_FULL_10GSX,  \
                                         L7_PHY_CAP_PORTSPEED_FULL_10G,     \
                                         L7_XAUI

/* Internal blade switch 10G port */
#define L7_PORT_DESC_BCOM_XAUI_10G_INT   L7_IANA_10G_ETHERNET,              \
                                         L7_PORTCTRL_PORTSPEED_FULL_10GSX,  \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10G |   \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000 |  \
                                          L7_PHY_CAP_INTERNAL),             \
                                         L7_XAUI


#define L7_PORT_DESC_BCOM_MTRJ_GIG    L7_IANA_GIGABIT_ETHERNET,          \
                                      L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                      (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                       L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                       L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                       L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                       L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                       L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                       L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                      L7_MTRJ


#define L7_PORT_DESC_BCOM_RJ45_GIG       L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                         L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY  L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                         L7_RJ45
 
#define L7_PORT_DESC_BCOM_RJ45_FE     L7_IANA_FAST_ETHERNET,             \
                                      L7_PORTCTRL_PORTSPEED_FULL_100TX,  \
                                      (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                       L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                       L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                       L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                       L7_PHY_CAP_PORTSPEED_FULL_100),   \
                                      L7_RJ45


#define L7_PORT_DESC_BCOM_POE_RJ45_GIG   L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000 |  \
                                          L7_PHY_CAP_POE_PSE            ),  \
                                         L7_RJ45

#define L7_PORT_DESC_BCOM_POE_PSE_RJ45_GIG   L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000 |  \
                                             L7_PHY_CAP_POE_PSE             ),  \
                                             L7_RJ45
#define L7_PORT_DESC_BCOM_POE_PD_RJ45_GIG   L7_IANA_GIGABIT_ETHERNET,          \
                                            L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                           (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                            L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                            L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                            L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                            L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                            L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                            L7_PHY_CAP_PORTSPEED_FULL_1000 |  \
                                            L7_PHY_CAP_POE_PD             ),  \
                                         L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_SFP   L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_SFP       |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                         L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_SFP_DETECT   L7_IANA_GIGABIT_ETHERNET,          \
                                                L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                                (L7_PHY_CAP_PORTSPEED_AUTO_NEG   |  \
                                                 L7_PHY_CAP_PORTSPEED_HALF_10    |  \
                                                 L7_PHY_CAP_PORTSPEED_FULL_10    |  \
                                                 L7_PHY_CAP_PORTSPEED_HALF_100   |  \
                                                 L7_PHY_CAP_PORTSPEED_FULL_100   |  \
                                                 L7_PHY_CAP_PORTSPEED_HALF_1000  |  \
                                                 L7_PHY_CAP_PORTSPEED_SFP        |  \
                                                 L7_PHY_CAP_PORTSPEED_SFP_DETECT |  \
                                                 L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                                L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_POE   L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_POE_PSE             |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                          L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_POE_SFP   L7_IANA_GIGABIT_ETHERNET,      \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                         (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_SFP       |  \
                                          L7_PHY_CAP_PORTSPEED_SFP_DETECT|  \
                                          L7_PHY_CAP_POE_PSE             |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                          L7_RJ45

#define L7_PORT_DESC_BCOM_RJ45_GIG_FIBER   L7_IANA_GIGABIT_ETHERNET,          \
                                           L7_PORTCTRL_PORTSPEED_FULL_1000SX, \
                                           (L7_PHY_CAP_PORTSPEED_AUTO_NEG  |  \
                                            L7_PHY_CAP_PORTSPEED_SFP       |  \
                                           L7_PHY_CAP_PORTSPEED_FULL_1000),   \
                                           L7_MTRJ 

#define L7_PORT_DESC_BCOM_SERDES       L7_IANA_GIGABIT_ETHERNET,            \
                                       L7_PORTCTRL_PORTSPEED_FULL_1000SX,   \
                                       (L7_PHY_CAP_PORTSPEED_AUTO_NEG |     \
                                        L7_PHY_CAP_PORTSPEED_FULL_1000),    \
                                       L7_RJ45

#define L7_PORT_DESC_BCOM_1G_AN          L7_IANA_GIGABIT_ETHERNET,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_1000SX,    \
                                         (/*L7_PHY_CAP_PORTSPEED_AUTO_NEG  |*/  \
                                          L7_PHY_CAP_PORTSPEED_HALF_10   |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10   |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_100  |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_100  |  \
                                          L7_PHY_CAP_PORTSPEED_HALF_1000 |  \
                                          /*L7_PHY_CAP_PORTSPEED_SFP       |*/  \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000),  \
                                         L7_RJ45

/* PTin added: Interfaces without Auto-Negotiation and connector */
#define L7_PORT_DESC_BCOM_1G_NO_AN     L7_IANA_GIGABIT_ETHERNET,            \
                                       L7_PORTCTRL_PORTSPEED_FULL_1000SX,   \
                                       (L7_PHY_CAP_PORTSPEED_FULL_1000 |    \
                                        L7_PHY_CAP_PORTSPEED_FULL_2500),    \
                                       L7_CONNECTOR_NONE

/* PTin added: Speed 2.5G */
#define L7_PORT_DESC_BCOM_2G5_NO_AN    L7_IANA_2G5_ETHERNET,                \
                                       L7_PORTCTRL_PORTSPEED_FULL_2P5FX,    \
                                       (L7_PHY_CAP_PORTSPEED_FULL_1000 |    \
                                        L7_PHY_CAP_PORTSPEED_FULL_2500),    \
                                       L7_CONNECTOR_NONE

/* PTin added: Speed 10G */
#define L7_PORT_DESC_BCOM_10G_KR       L7_IANA_10G_ETHERNET,                \
                                       L7_PORTCTRL_PORTSPEED_FULL_10GSX,    \
                                       (L7_PHY_CAP_PORTSPEED_AUTO_NEG |     \
                                       L7_PHY_CAP_PORTSPEED_FULL_10G),      \
                                       L7_CONNECTOR_NONE

/* PTin added: Speed 40G */
#define L7_PORT_DESC_BCOM_40G_KR4      L7_IANA_40G_ETHERNET,                \
                                       L7_PORTCTRL_PORTSPEED_FULL_40G_KR4,  \
                                       (L7_PHY_CAP_PORTSPEED_AUTO_NEG |     \
                                        L7_PHY_CAP_PORTSPEED_FULL_40G |     \
                                        L7_PHY_CAP_PORTSPEED_FULL_10G |     \
                                        L7_PHY_CAP_PORTSPEED_FULL_2500 |    \
                                        L7_PHY_CAP_PORTSPEED_FULL_1000 |    \
                                        L7_PHY_CAP_PORTSPEED_HALF_1000 |    \
                                        L7_PHY_CAP_PORTSPEED_FULL_100 |     \
                                        L7_PHY_CAP_PORTSPEED_HALF_100 |     \
                                        L7_PHY_CAP_PORTSPEED_FULL_10 |      \
                                        L7_PHY_CAP_PORTSPEED_HALF_10),      \
                                       L7_XLAUI

/* PTin added: Speed 100G */
#define L7_PORT_DESC_BCOM_100G_BKP     L7_IANA_100G_ETHERNET,               \
                                       L7_PORTCTRL_PORTSPEED_FULL_100G_BKP, \
                                       (L7_PHY_CAP_PORTSPEED_FULL_100G),    \
                                       L7_XLAUI

#define L7_PORT_DESC_BCOM_XAUI_10G_1G    L7_IANA_10G_ETHERNET,              \
                                         L7_PORTCTRL_PORTSPEED_FULL_10GSX,  \
                                         (/*L7_PHY_CAP_PORTSPEED_SFP | */       \
                                          L7_PHY_CAP_PORTSPEED_FULL_1000 |  \
                                          L7_PHY_CAP_PORTSPEED_FULL_10G),   \
                                         L7_XAUI

#define L7_PORT_DESC_BCOM_RJ45_FE_SFP    L7_IANA_FAST_ETHERNET_FX,          \
                                         L7_PORTCTRL_PORTSPEED_FULL_100FX,  \
                                         ( L7_PHY_CAP_PORTSPEED_FULL_100  | \
                                          L7_PHY_CAP_PORTSPEED_SFP),        \
                                         L7_RJ45

#define L7_PORT_DESC_BCOM_XAUI_10G_FIBER       L7_IANA_10G_ETHERNET,             \
                                               L7_PORTCTRL_PORTSPEED_FULL_10GSX, \
                                               L7_PHY_CAP_PORTSPEED_FULL_10G,    \
                                               L7_XAUI

L7_uint32 hpcLocalCardIdGet(int slotNum);
L7_RC_t   hpcLocalCardIdDiscover(void);
L7_RC_t   hpcDiagStart(void);


/* Code load types for different units. 
** For master driver the same code image supports all unit types.
*/

/* XGS devices.
** Note that XGS and Strata boxes can't stack together.
*/
#define  UNIT_BRXGS_CODE_TARGET_DEVICE    0x0100B000

/* Configuration load types for different units
*/
#define  UNIT_BRXGS_CONFIG_TARGET_DEVICE  0x0700B000


#define L7_LOGICAL_CARD_LAG_ID                       0xFF020001

/* This define is to avoid confusion with the maximum number of
* router interfaces which includes both physical interfaces
* associated with physical cards and logical interfaces
* associated with routers.
*/
#define L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID          0xFF030001
#define L7_LOGICAL_CARD_LOOPBACK_INTF_ID             0xFF040001
#define L7_LOGICAL_CARD_TUNNEL_INTF_ID               0xFF050001
#define L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID        0xFF060001
#define L7_LOGICAL_CARD_VLAN_PORT_INTF_ID            0xFF070001   /* PTin added: virtual ports */

typedef struct
{
  L7_uint32  slot;
  L7_BOOL    unique_mac_per_port;             /* specifies whether a different mac address should be assigned to each port in this slot */
  L7_uint32  num_mac_addrs_allocated_to_slot; /* number of mac addresses reserved for this slot */
  L7_uint32  system_base_mac_offset;          /* offset from system base mac for MAC addresses for this slot */
} HPC_BROAD_SLOT_MAC_ALLOCATION_t;

typedef struct
{
  L7_INTF_TYPES_t type;
  L7_uint32  system_base_l2_mac_offset;          /* L2 offset from system base mac for MAC addresses for this type */
  L7_uint32  system_base_l3_mac_offset;          /* L3 offset from system base mac for MAC addresses for this type */
} HPC_BROAD_INTF_TYPE_MAC_ALLOCATION_t;

typedef struct
{
  L7_uint32  unit;
  L7_uint32  port;
} HPC_BROAD_STK_PORT_TABLE_ENTRY_t;

typedef struct 
{
  /* mac address allocation parameters */
  HPC_BROAD_SLOT_MAC_ALLOCATION_t *mac_allocation_table;
  L7_uint32 num_mac_allocation_table_entries;

  /* Broadcom dedicated stack port information */
  L7_int32  mod_ids_req[BCM_MAX_NUM_UNITS]; /* number of module identifiers required by this unit (box) type */
  L7_uint32 slot_id;
  L7_uint32 num_units;                      /* total number of Broadcom switch/fabric chips (units) in this system */
  L7_uint32 dest_unit;                      /* unit used to communicate with CPU */
  L7_uint32 dest_port;                      /* port on unit used to communicate with CPU */
  L7_uint32 num_stk_ports;                  /* number of dedicated stack ports in this unit type */
  HPC_BROAD_STK_PORT_TABLE_ENTRY_t *stk_port_list; 
} HPC_BROAD_UNIT_DATA_t;

typedef struct
{
  L7_uint32   registrarID;
  void        (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeID, L7_uint32 event);
} hpcCardEventNotifyList_t;

#endif
