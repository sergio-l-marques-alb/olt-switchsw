/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sysapi_hpc.h
*
* @purpose sysapi hardware platform control defines and structure definitions
*
* @component sysapi
*
* @comments none
*
* @create 02/12/2003
*
* @author jeffr
* @end
*
**********************************************************************/

#ifndef SYSAPIHPCPROTO_HEADER
#define SYSAPIHPCPROTO_HEADER

#include "l7_product.h"
#include <platform_counters.h>
#include <l7_common.h>
#include "compdefs.h"
#include "spm_api.h"

#include <stdio.h>

#ifdef L7_POE_PACKAGE
#include "poe_exports.h"
#include "nimapi.h"
#include "datatypes.h"
#endif

#define SYSAPI_HPC_UNIT_MODEL_SIZE              32
#define SYSAPI_HPC_UNIT_DESCRIPTION_SIZE        80
#define SYSAPI_HPC_CARD_MODEL_SIZE              32
#define SYSAPI_HPC_CARD_DESCRIPTION_SIZE        80
#define SYSAPI_HPC_SYSTEM_OID_SIZE                      32

#define SYSAPI_HPC_MAX_LOGICAL_SLOTS_PER_UNIT (L7_MAX_LOGICAL_SLOTS_PER_UNIT +  \
                                               L7_MAX_CPU_SLOTS_PER_UNIT)

/* Setting for units and cards that don't support code load.
** In general all units support code load.
** In general cards don't support code load, however cards wwith CPUs in a chassis
** may support a code load.
*/
#define SYSAPI_NO_CODE_LOAD   0

/*  Setting for units and cards that don't support configuration load.
*/
#define SYSAPI_NO_CONFIG_LOAD   0

typedef L7_uint32 L7_EVENT_HANDLE_t;

/*
** sysapi hardware platform control structures
*/
typedef enum
{
  SYSAPI_CARD_TYPE_NOT_PRESENT,
  SYSAPI_CARD_TYPE_LINE,
  SYSAPI_CARD_TYPE_LAG,
  SYSAPI_CARD_TYPE_VLAN_ROUTER,
  SYSAPI_CARD_TYPE_CPU,
  SYSAPI_CARD_TYPE_LOGICAL_CPU,
  SYSAPI_CARD_TYPE_FAN,
  SYSAPI_CARD_TYPE_POWER_SUPPLY,
  SYSAPI_CARD_TYPE_CONTROL,
  SYSAPI_CARD_TYPE_CONTROL_STANDBY,
  SYSAPI_CARD_TYPE_LOOPBACK,
  SYSAPI_CARD_TYPE_TUNNEL,
  SYSAPI_CARD_TYPE_CAPWAP_TUNNEL,
  SYSAPI_CARD_TYPE_LAST,
  SYSAPI_CARD_TYPE_INVALID = 254,
  SYSAPI_CARD_TYPE_ALL = 255
} SYSAPI_CARD_TYPE_t;

typedef enum
{
  SYSAPI_STACK_ROLE_UNASSIGNED = 0,
  SYSAPI_STACK_ROLE_MANAGEMENT_UNIT,
  SYSAPI_STACK_ROLE_STACK_UNIT
} SYSAPI_STACK_ROLE_t;

typedef enum
{
  SYSAPI_CARD_EVENT_PLUG = 1,
  SYSAPI_CARD_EVENT_UNPLUG,
  SYSAPI_CARD_EVENT_FAILURE,
  SYSAPI_CARD_EVENT_HEALTHY,
  SYSAPI_CARD_EVENT_INIT_DONE,
  SYSAPI_CARD_EVENT_ACTIVE,
  SYSAPI_CARD_EVENT_STANDBY,
  SYSAPI_CARD_EVENT_VER_MISMATCH,
  SYSAPI_CARD_EVENT_LAST,
  SYSAPI_CARD_EVENT_INVALID = 254,
  SYSAPI_CARD_EVENT_ALL = 255
} SYSAPI_CARD_EVENT_t;

typedef enum
{
  SYSAPI_CARD_INDEX_1 = 0,
  SYSAPI_CARD_INDEX_2,
  SYSAPI_CARD_INDEX_3,
  SYSAPI_CARD_INDEX_4,
  SYSAPI_CARD_INDEX_LAST,
  SYSAPI_CARD_INDEX_INVALID= 254,
  SYSAPI_CARD_INDEX_ALL = 255
}SYSAPI_CARD_INDEX_t;

typedef enum
{
  SYSAPI_MAC_POLICY_MGMT_UNIT_POOL = 1,     /* MAC addresses for all interfaces are allocated from management unit's
                                            ** pool of addresses
                                            */
  SYSAPI_MAC_POLICY_REMOTE_UNIT_POOL        /* MAC addresses for phyiscal interfaces are allocated from the address pool
                                            ** of the unit inwhich the interface exists
                                            */
} SYSAPI_MAC_POLICY_t;

typedef struct
{
  L7_uint32       slot_number;
  L7_uint32       cardTypeId;
} SYSAPI_HPC_CARD_SLOT_MAP_t;

/* Physical slot descriptors.
*/
typedef struct
{
  L7_uint32 slot_number;
  L7_BOOL   pluggable;   /* L7_TRUE - card can be plugged into the slot. */
  L7_BOOL   power_down;  /* L7_TRUE - Slot can be powered down.          */
  L7_uint32 num_supported_card_types; /* Number of supported card types in this slot */

  /* List of physical cards that are supported in this slot.
   */
  L7_uint32 supported_cards [L7_MAX_SUPPORTED_PHYSICAL_CARD_TYPES];
} SYSAPI_PHYS_SLOT_MAP_t;


typedef struct
{
  L7_uint32 fps; /* 1 - Front-panel, 0 - Dedicated */
  L7_uint32 slot, port; /* 0,0 - Dedicated, Slot and Port for FPS */
  L7_uchar8 tag[SPM_STACK_PORT_TAG_SIZE]; /* Port identifier */
  L7_uint32 speed;   /* Speed in Gigabits per second */
  L7_uint32 h1, h2, h3, h4; /* Hardware-specific identifiers for this port */

} SYSAPI_PHYSICAL_STACK_PORT_t;

typedef struct
{
  L7_uint32           unitTypeId;            /* unique identifier of this unit type */

                /* text with the model number of this unit type
                */
  L7_uchar8           unitModel[SYSAPI_HPC_UNIT_MODEL_SIZE];

                /* text describing this unit type
                */
  L7_uchar8           unitDescription[SYSAPI_HPC_UNIT_DESCRIPTION_SIZE];

  L7_uint32           managementPreference;  /* the default management preference setting for this unit type */
  L7_uchar8           systemOID[SYSAPI_HPC_SYSTEM_OID_SIZE];   /* default system OID */
  L7_uint32           numPowerSupply;        /* number of power supplies in this unit */
  L7_uint32           numFans;               /* number of fans in this unit */
  L7_uint32           poeSupport;            /* Does this unit support POE */
  L7_uint32           numPhysSlots;          /* number of physical slots this unit type has */
  L7_uint32           numNonRemovableCards;  /* number of entries in the non-removable card table */

  L7_uint32           codeLoadTargetId;   /* What code needs to be loaded on this device */
  L7_uint32           configLoadTargetId; /* What configuration must be loaded on this device */

  /* Slot mapping table for logical card types that are always created
  ** on a platform. The ligical cards are not removable.
  */
  SYSAPI_HPC_CARD_SLOT_MAP_t  nonRemovableCardTable[SYSAPI_HPC_MAX_LOGICAL_SLOTS_PER_UNIT];

  SYSAPI_PHYS_SLOT_MAP_t physSlot[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];

  /* Stacking port information.
  */
  L7_uint32           num_stack_ports;
  SYSAPI_PHYSICAL_STACK_PORT_t stack_port_list [L7_MAX_STACK_PORTS_PER_UNIT];

  /* Ports Interconnecting two or more chips.
  */
  L7_uint32           num_internal_stack_ports;
  SYSAPI_PHYSICAL_STACK_PORT_t int_stack_port_list [L7_MAX_STACK_PORTS_PER_UNIT];

} SYSAPI_HPC_UNIT_DESCRIPTOR_t;

typedef struct
{
  L7_IANA_INTF_TYPE_t   type;
  L7_PORT_SPEEDS_t      defaultSpeed;
  L7_uint32             phyCapabilities; /* combination of all applicable L7_PHY_CAPABILITIES_t */
  L7_CONNECTOR_TYPES_t  connectorType;
} SYSAPI_HPC_PORT_DESCRIPTOR_t;

typedef struct
{
  L7_uint32            cardTypeId;    /* numerical identifier unique to this card hardware, used as key to matching
                                      ** this database entry to physical cards found in the system
                                      */
  /* text with the model number of this card type
   */
  L7_uchar8            cardModel[SYSAPI_HPC_CARD_MODEL_SIZE];

  /* text describing this card type
   */
  L7_uchar8            cardDescription[SYSAPI_HPC_CARD_DESCRIPTION_SIZE];

  SYSAPI_CARD_TYPE_t   type;
  L7_ulong32           numOfNiPorts;    /* number of network interface ports on this card type */

  L7_uint32           codeLoadTargetId;   /* What code needs to be loaded on this device */
  L7_uint32           configLoadTargetId; /* What configuration must be loaded on this device */

  SYSAPI_HPC_PORT_DESCRIPTOR_t *portInfo;      /* pointer to array of port descriptors for the ports on this card type */

  void                *dapiCardInfo;       /* the driver for the card entry will define what this data type is */

} SYSAPI_HPC_CARD_DESCRIPTOR_t;

typedef enum
{
  HPC_EVENT_BCAST_TRANSPORT_UP = 0,
  HPC_EVENT_BCAST_TRANSPORT_DOWN,
  HPC_EVENT_UNIT_SPECIFIC_TRANSPORT_UP,
  HPC_EVENT_UNIT_SPECIFIC_TRANSPORT_DOWN,
  HPC_EVENT_LOCAL_STACK_PORT_LINK_STATE,
  HPC_EVENT_STACK_TOPOLOGY_CHANGE,

  /* Always last */
  HPC_EVENT_LAST

} HPC_EVENT_t;

typedef struct
{
  HPC_EVENT_t hpcStackEvent;

  union
  {
    struct
    {
      L7_BOOL           linkUp;
      L7_uint32         hpcIndex;
      L7_enetMacAddr_t *nhopCpuKey;
    } stackPortLinkEvent;

  } msgData;

} hpcStackEventMsg_t;

typedef struct
{
  SYSAPI_CARD_TYPE_t    moduleType;
  SYSAPI_CARD_INDEX_t   moduleIndex;
  SYSAPI_CARD_EVENT_t   event;
} hpcEvent_t;

typedef struct
{
  SYSAPI_HPC_UNIT_DESCRIPTOR_t unitTypeDescriptor;   /* public portion of unit type descriptor */
  void                         *hpcPlatformData;     /* pointer to data structure used by platform specific services,
                                                        contents vary by platform and are initialized at run time */
} HPC_UNIT_DESCRIPTOR_t;

typedef struct
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t cardTypeDescriptor;   /* public portion of the card type descriptor */
} HPC_CARD_DESCRIPTOR_t;

#if L7_FEAT_PLUGIN_XFP_MODULE
extern L7_RC_t hpcNotify10GBASETPresence(L7_uint32 unit, L7_uint32 xfpPort, L7_BOOL present);
extern L7_RC_t hpcNotifySfpPlusPresence(L7_uint32 unit, L7_uint32 sfpPort);
#define HPC_SENSOR_10GBASET_PRSNCE(unit, xfpPort, present) hpcNotify10GBASETPresence(unit, xfpPort, present)
#define HPC_SENSOR_SFPPLUS_PRSNCE(unit, sfpPort) hpcNotifySfpPlusPresence(unit, sfpPort)

#else
#define HPC_SENSOR_10GBASET_PRSNCE(unit, xfpPort, present) (L7_NOT_SUPPORTED)
#define HPC_SENSOR_SFPPLUS_PRSNCE(unit, sfpPort)  (L7_NOT_SUPPORTED)
#endif

#if L7_FEAT_SF10GBT
/*********************************************************************
* @purpose  Enables and disables power on SF10GbT using XFP registers
*           
*
* @param    index- indicates which of the four XFP ports is being set
* @param    turnOn  on or off (1 / 0)
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t hpcXfpSF10GbTModulePowerSet(L7_uint32 xfpIndex, L7_uint32 turnOn);
/**************************************************************************
*
* @purpose  Initialize SF 10GbaseT modules, if detected 
*
* @param    socUnit @b{(input)}  sdk silicon unit number
* @returns  void
*
* @comments  For some reason, SF10G module is detected as XFP but not as 10GbaseT 
*            SF10GbT is not XFP module but 10GBaseT, but we have to use XFP power register to init module  
*
* @end
*
*************************************************************************/
void    hpcXfpSF10GbTModuleInit(L7_int32 socUnit);
#endif
/**************************************************************************
*
* @purpose  Reports the local unit's MAC address for use in identifying
*           this unit across the distributed system.
*
* @param    mac   pointer to storage inwhich to store the mac address
*
* @returns  L7_SUCCESS  address retrieved with no problems
* @returns  L7_ERROR  problem encountered in platform specific retrieval function
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac);

/**************************************************************************
*
* @purpose  Retrieve the unit descriptor from the database
*           for the unit type of the local unit.
*
* @param    none
*
* @returns  pointer to descriptor record, L7_NULLPTR if problem retrieving
*
* @notes
*
* @end
*
*************************************************************************/
HPC_UNIT_DESCRIPTOR_t *hpcLocalUnitDescriptorGet(void);

/*
** sysapi hardware platform control api functions
*/

/**************************************************************************
*
* @purpose  Initialize the Hardware Platform Control facility of the
*           sysapi component.
*
* @param    none
*
* @returns  L7_SUCCESS if initialization completes with no problems
* @returns  L7_FAILURE if problem was found during initialization
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcInit(void);

/**************************************************************************
*
* @purpose  Retrieve information from the unit descriptor database for the
*           given unit type identifier.
*
* @param    unitTypeId   @b((in)) unit type identifier the caller wants
*                        information about
*
* @returns  pointer to the unit descriptor record if unitTypeId
*           was found in the database
* @returns  L7_NULLPTR if unitTypeId was not found in the database
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcUnitDbEntryGet(L7_uint32 unitTypeId);

/**************************************************************************
*
* @purpose  Retrieve information from the unit descriptor database for the
*           given unit index.
*
* @param    unitTypeIdIndex  Index in the unit table.
*
* @returns  pointer to the unit descriptor record if unitTypeIdIndex
*           is valid.
* @returns  L7_NULLPTR if unitTypeIdIndex is invalid.
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcUnitDbEntryByIndexGet(L7_uint32 unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Return the unit index of the local unit.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbEntryLocalUnitIndexGet(L7_uint32 *unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid unitTypeIdIndex
*           value.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbEntryIndexCheck(L7_uint32 unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns the next valid unitTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    unitTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbEntryIndexNextGet(L7_uint32 *unitTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns unit type index that corresponds to
*           the unit type identifier provided.
*
* @param    unitTypeId        unit type identifier
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  the ID is valid and the index retrieved
* @returns  L7_FAILURE  the ID was not valid
* @returns  *unitTypeIndex  pointer to the index
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbUnitTypeIndexGet(L7_uint32 unitTypeId, L7_uint32 *unitTypeIndex);

/**************************************************************************
*
* @purpose  Returns unit type identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *unitTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbUnitIdGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitTypeId);

/**************************************************************************
*
* @purpose  Returns unit model identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitModel
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbModelGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitModel);

/**************************************************************************
*
* @purpose  Returns unit description identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitDescription
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbDescriptionGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitDescription);

/**************************************************************************
*
* @purpose  Returns unit management preference identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitMgmtPref
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbMgmtPrefGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitMgmtPref);

/**************************************************************************
*
* @purpose  Returns unit OID that corresponds to the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitOid
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbOidGet(L7_uint32 unitTypeIdIndex, L7_uchar8 *unitOid);

/**************************************************************************
*
* @purpose  Returns PoE is supported on the unit type that
*           corresponds to the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitPoeSupported
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbPoeSupportGet(L7_uint32 unitTypeIdIndex, L7_BOOL *unitPoeSupported);

/**************************************************************************
*
* @purpose  Returns unit code load target identifier that corresponds to
*           the index provided.
*
* @param    unitTypeIdIndex   index into the unit descriptor table
* @param    unitCodeLoadTargetId
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcUnitDbCodeTargetIdGet(L7_uint32 unitTypeIdIndex, L7_uint32 *unitCodeLoadTargetId);

/**************************************************************************
*
* @purpose  Return a pointer to the unit descriptor for the local unit.
*
* @param    none
*
* @returns  pointer to the unit descriptor record for the local unit
* @returns  L7_NULLPTR if some problem was encountered in retrieving
*                      the descriptor pointer
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_UNIT_DESCRIPTOR_t *sysapiHpcLocalUnitDbEntryGet(void);

/**************************************************************************
*
* @purpose  Retrieve information from the card descriptor database for the
*           given card type identifier.
*
* @param    cardTypeId   @b((in)) card type identifier the caller wants
*                        information about
*
* @returns  pointer to the card descriptor record if cardTypeId
*           was found in the database
* @returns  L7_NULLPTR if cardTypeId was not found in the database
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardDbEntryGet(L7_uint32 cardTypeId);

/**************************************************************************
*
* @purpose  Retrieve information from the card descriptor database for the
*           given card index.
*
* @param    cardTypeIdIndex
*
* @returns  pointer to the card descriptor record if cardTypeId
*           was found in the database
* @returns  L7_NULLPTR if card index is not valid.
*
* @comments
*
* @end
*
*************************************************************************/
SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardDbEntryByIndexGet(L7_uint32 cardTypeIdIndex);

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid cardTypeIdIndex
*           value.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbEntryIndexCheck(L7_uint32 cardTypeIdIndex);

/**************************************************************************
*
* @purpose  Returns the next valid cardTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
* @returns  cardTypeIdIndex value of next index if found
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbEntryIndexNextGet(L7_uint32 *cardTypeIdIndex);

/*****************************************
** Supported Slot Table Access Functions
*****************************************/

/**************************************************************************
*
* @purpose  Returns the next valid slotTypeIdIndex of greater value
*           than the one provided if it exists.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryIndexNextGet(L7_uint32 * unitIndex, L7_uint32 *slotIndex);

/**************************************************************************
*
* @purpose  Validates whether the provided index is a valid for the slot.
*
* @param    unitIndex
* @param    slotIndex
*
* @returns  L7_SUCCESS  index is valid
* @returns  L7_FAILURE  index is invalid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryIndexCheck(L7_uint32 unitIndex, L7_uint32 slotIndex);

/**************************************************************************
*
* @purpose  Returns the slot number of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    slotNumber  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntrySlotNumberGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *slotNumber);

/**************************************************************************
*
* @purpose  Returns the pluggable indicator of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    pluggable  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryPluggableGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *pluggable);

/**************************************************************************
*
* @purpose  Returns the pluggable indicator of the specified unit/slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    powerdown  Slot number of this slot.
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcSlotDbEntryPowerdownGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 *powerdown);

/*************************************************
** Supported-Card-In-slot Table Access Functions
*************************************************/

/**************************************************************************
*
* @purpose  Indicates whether specified card is supported in
*           the specified unit and specified slot.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    cardIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  Card is supported
* @returns  L7_FAILURE  Card is not supported
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardInSlotDbEntryGet(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 cardIndex);

/**************************************************************************
*
* @purpose  Returns the next valid CardInSlotIndex.
*
* @param    unitIndex   index into the unit descriptor table
* @param    slotIndex   index into the card descriptor table
* @param    cardIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  an entry with a greater index value exists
* @returns  L7_FAILURE  an entry with a greater index value does not exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardInSlotDbEntryIndexNextGet(L7_uint32 * unitIndex, L7_uint32 *slotIndex, L7_uint32 *cardIndex);

/**********************************************************
** Support-Card Table Access Functions
**********************************************************/

/**************************************************************************
*
* @purpose  Returns card type identifier that corresponds to
*           the index provided.
*
* @param    cardTypeIdIndex   index into the card descriptor table
*
* @returns  L7_SUCCESS  the index is valid and the string retrieved
* @returns  L7_FAILURE  the index was not valid
* @returns  *cardTypeId  pointer to the identifier
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcCardDbCardIdGet(L7_uint32 cardTypeIdIndex, L7_uint32 *cardTypeId);

/*********************************************************************
* @purpose  Register a routine to be called when a card is plugged
*           or unplugged in the local unit.
*
* @param    *notify      Notification routine with the following parm
*                        @param    slotNum             internal interface number
*                        @param    cardTypeId          card type identifier
*                        @param    event               SYSAPI_CARD_PLUG_EVENT or
*                                                      SYSAPI_CARD_UNPLUG_EVENT
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                           L7_uint32 registrarID);

/**************************************************************************
*
* @purpose  Reports the local unit's MAC address for use in identifying
*           this unit across the distributed system.
*
* @param    mac   pointer to storage inwhich to store the mac address
*
* @returns  L7_SUCCESS  address retrieved with no problems
* @returns  L7_ERROR  problem encountered in platform specific retrieval function
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac);

/*********************************************************************
* @purpose  In forms caller if stack is formed and HPC is ready
*           to accept messages for transport amongst stack units.
*
* @param    None
*
* @returns  L7_TRUE      stack is formed, transport is ready
* @returns  L7_FALSE     stack is not formed, transport is not ready
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcBcastTransportIsReady(void);

/*********************************************************************
* @purpose  Sets the unit number assigned to the local system.
*
* @param    unit  Unit number to be assigned to the local system
*
* @returns  Nothing
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcUnitNumberSet(L7_uint32 unit);

/*********************************************************************
* @purpose  Gets the unit number assigned to the local system. Returns
*           0 if not set.
*
* @param    None
*
* @returns  Unit number assigned to local system (0 if unassigned)
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcUnitNumberGet(void);

/*********************************************************************
* @purpose  Sets the management preference
*
* @param    admin_pref Preference
*
* @returns  Nothing
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcAdminPrefSet(L7_uint32 admin_pref);

/*********************************************************************
* @purpose  Gets the configured management preference
*
* @param    None
*
* @returns  Configured management preference
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcAdminPrefGet(void);

/*********************************************************************
* @purpose  Given a unit, gets the unit's system identifier key
*
* @param    unit_number unit number
* @param    *key   pointer to unit key storage
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if unit_number invalid
*
* @notes    will copy L7_HPC_UNIT_ID_KEY_LEN bytes into *key
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcUnitIdentifierKeyGet(L7_uint32 unit, L7_enetMacAddr_t *key);

/**************************************************************************
*
* @purpose  Gets the MAC address allocation policy for the platform.  The
*           policy returned is one of the ENUM SYSAPI_MAC_POLICY_t.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems getting policy
* @returns  L7_ERROR    (slot, port) data invalid, or other error in MAC
*                       address retrieval
*
* @notes
*
* @end
*
*************************************************************************/
SYSAPI_MAC_POLICY_t sysapiHpcMacAllocationPolicyGet(void);

/*********************************************************************
* @purpose  Gets the MAC address for the port in the local unit
*           for the specified slot and port.
*
* @param    type     interface type of the interface
* @param    slot     slot number of interface for requested MAC address
* @param    port     port number of interface for requested MAC address
* @param    macaddr  pointer to storage for the retrieved MAC address
*
* @returns  L7_SUCCESS   the MAC for the slot,port successfully found
* @returns  L7_ERROR     problem encountered in getting MAC for slot,port
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port, L7_uchar8 *mac_addr);

/*********************************************************************
* @purpose  Sets the local system's status with respect to top of stack
*           role.
*
* @param    role  one of the enum SYSAPI_HPC_TOP_OF_STACK_t
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcTopOfStackSet(SYSAPI_STACK_ROLE_t role);

/*********************************************************************
* @purpose  Returns the role of this system in the stack.
*
* @param    None
*
* @returns  SYSAPI_STACK_ROLE_UNASSIGNED - stand alone or stack not resolved
* @returns  SYSAPI_STACK_ROLE_MANAGEMENT_UNIT - the local system is the management unit
* @returns  SYSAPI_STACK_ROLE_STACK_UNIT - the local system is a stack member but not
*                                the management unit
*
* @comments
*
* @end
*********************************************************************/
SYSAPI_STACK_ROLE_t sysapiHpcTopOfStackGet(void);

/*********************************************************************
* @purpose  Gets whether network port is used for stacking.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - OUT: boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if specified port is not found in the database.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcStackPortGet(L7_uint32 slot, L7_uint32 port, L7_BOOL *stacking_enable);

/*********************************************************************
* @purpose  Enables or disables a network port for use in stacking.
*           This information is saved in the HPC component's persistent
*           storage configuration so it is available after reboots.
*
* @param    slot - slot number of port be configured for stacking
* @param    port - port number to be configured for stacking
* @param    stacking_enable - boolean to enable (L7_TRUE) or disable (L7_FALSE)
*
* @returns  L7_SUCCESS - if all goes well
* @returns  L7_FAILURE - if problem occurs
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcStackPortSet(L7_uint32 slot, L7_uint32 port, L7_BOOL stacking_enable);

/*********************************************************************
* @purpose  Queries the platform component as to the largest message
*           size in bytes that the transport can handle.
*
* @param    none
*
* @returns  the max size of message payload in bytes
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 sysapiHpcTransportMaxMessageLengthGet(void);

/*********************************************************************
* @purpose  Register a routine to be called when an intrastack message
*           is received for the registrar.
*
* @param    *notify      Notification routine with the following parm
*                        @param    *buffer             pointer to buffer containing message
*                        @param    msg_len             length in bytes of message
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcReceiveCallbackRegister(void (*notify)(L7_uchar8* buffer, L7_uint32 msg_len),
                                         L7_uint32 registrarID);

/**************************************************************************
*
* @purpose  Function registered with HPC to get stack event callbacks.
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    event        Stack event to register
* @param    *notify      Notification routine with the hpcStackEventMsg_t parm
*
* @returns  L7_SUCCESS callback successfully registered
* @returns  L7_FAILURE registrarID is not a valid component ID or invalid eventId
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcStackEventRegisterCallback(L7_uint32 registrarId,
                                            HPC_EVENT_t event,
                                            void (*notify)(hpcStackEventMsg_t eventMsg));

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
*           The message payload consists of the following:
*              4 bytes:  the sending component's registrarID (this is HPC's
*                        PDU header
*              (sysapiHpcMaxMessageLength() - 4) bytes: the sending component's
*                        data
*
* @param    registrarID  routine registrar ID (See L7_COMPONENT_IDS_t)
* @param    msg_length  number of bytes in buffer including the HPC header
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
L7_RC_t sysapiHpcBroadcastMessageSend(L7_uint32 registrarID, L7_uint32 msg_length, L7_uchar8* buffer);

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
*           The message payload consists of the following:
*              4 bytes:  the sending component's registrarID (this is HPC's
*                        PDU header
*              (sysapiHpcMaxMessageLength() - 4) bytes: the sending component's
*                        data
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
L7_RC_t sysapiHpcMessageSend(L7_uint32 registrarID, L7_uint32 unit, L7_uint32 msg_length, L7_uchar8* buffer);

/*********************************************************************
* @purpose  Resets the Driver to a know state
*
* @param    void
*
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverReset(void);

/*********************************************************************
* @purpose  Determine whether the driver is responsible for resyncing new devices
*
* @param    void
*
* @returns  L7_TRUE   if the driver is responsible
* @returns  L7_FALSE  if the driver is not responsible
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL sysapiHpcDriverSyncSupported(void);

/*********************************************************************
* @purpose  Get status and statistics for specified stacking port.
*
* @param    hpc_index - HPC index of the stacking port for which to
*                       get information.
* @param    port_info - Port information and statistics.
*                       Caller must allocate space for this structure.
*
* @returns  none
*
* @comments Each time the function is called it computes the average
* @comments data rate and error rate. In order to keep these counters
* @comments fairly accurate the function should be called at least
* @comments once before the hardware counters wrap.
*
* @end
*********************************************************************/
L7_RC_t hpcStackPortInfoGet(L7_uint32 hpc_index,
                            SPM_STACK_PORT_INFO_t *port_info);

/*********************************************************************
* @purpose  Set special QOS mode for all FPS ports
*
* @param    hpc_index - HPC index of the stacking port for which to
*                       get information.
*
* @returns  L7_SUCCESS - QOS Mode Enabled.
* @returns  L7_FAILURE - QOS Mode is not enabled.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t hpcFpsQosModeSet(L7_BOOL qos_mode_enable);

/**************************************************************************
*
* @purpose  Return the local physical port count.
*
* @returns  localPhysicalPortCount   Number of physical ports on local unit
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcLocalPhysPortCntGet(void);

/**************************************************************************
*
* @purpose  Return the number of physical ports, given a local slot number.
*
* @param    slotNum      slot number for requested card ID.
*
* @returns  portCount    Number of physical ports in slot number
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcLocalPhysPortsInSlotGet(int slotNum);

/**************************************************************************
*
* @purpose  Return the number of physical ports, given a slot number.
*
* @param    slotNum      slot number for requested card ID.
*
* @returns  portCount    Number of physical ports in slot number
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcPhysPortsInSlotGet(int slotNum);

/**************************************************************************
*
* @purpose  From the HPC database, populate hpcSlotPortCount array. This array
*           contains the count of maximum number of physical ports supported
*           for each slot.
*
* @param    none
*
* @returns  none
*
*
* @end
*
*************************************************************************/
void sysapiHpcSlotPortCountPopulate(void);

/**************************************************************************
*
* @purpose  Return the physical slot count for the local unit's base CPU board.
*
* @param    none
*
* @returns  L7_uint32   Number of physical slots for this base CPU card,
*                       zero indicates error in retrieval
*
* @comments
*
* @end
*
*************************************************************************/
L7_uint32 hpcLocalUnitSlotCountGet(void);


/**************************************************************************
*
* @purpose  Return the physical port count for a given card ID.
*
* @returns  physicalPortCount   Number of physical ports on given card ID.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcPhysPortsOnCardGet(L7_uint32 cardId);

/**************************************************************************
*
* @purpose  Return the physical port count for a given card ID.
*
* @returns  physicalPortCount   Number of physical ports on given card ID.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcPhysPortsOnCardGet(L7_uint32 cardId);

/**************************************************************************
*
* @purpose  Initialize the local card database give in the sysbrd type.
*
* @param    board_info   pointer to board information.
*
* @returns  L7_SUCCESS - Card database initialized.
* @returns  L7_FAILURE - Card database failed to initialize
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcLocalCardDbInit(void *board_info);

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
L7_BOOL sysapiHpcNoStackPorts();

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
L7_BOOL hpcNoStackPorts();


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
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverProcessStackTopoChange(L7_BOOL *managerFailed);

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
L7_RC_t sysapiHpcDriverProcessManagerLinkDown(L7_uint32 reportingUnit, L7_enetMacAddr_t *reportingUnitKey,
                                              L7_uint32 stkPortHpcIndex, L7_enetMacAddr_t *nhopCpuKey,
                                              L7_BOOL *managerFailed);

/*********************************************************************
* @purpose  Inform the driver that this unit is the standby of the stack
*
* @param    stbyStatus L7_TRUE: This unit is the standby of the stack
* @param               L7_FALSE: This unit is not the standby of the stack
*
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDriverNotifyStandbyStatus(L7_BOOL stbyStatus);

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
void sysapiHpcDriverRemoveStackManager(L7_enetMacAddr_t managerKey);

/*********************************************************************
* @purpose  Instruct the driver to shutdown all the front-panel ports 
*           for a unit.
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
L7_RC_t sysapiHpcDriverShutdownPorts(L7_uint32 unitNumber);


#define HPC_ASF_CFG_FILENAME "asf.cfg"
#define HPC_ASF_CONFIG_VER_CURRENT 1
typedef struct
{
  L7_uint32     version;
  L7_uint32     currAsfMode;
  L7_uint32     configAsfMode;
  L7_uint32     crc;
} hpcAsfCfg_t;

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
L7_RC_t sysapiHpcAsfModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get (Enable/Disable) the ASF mode.
*
* @param    currMode    @b{(input)} Current mode L7_ENABLE/L7_DISABLE
* @param    configMode  @b{(input)} Configured mode L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcAsfModeGet(L7_uint32 *currMode, L7_uint32 *configMode);

/********************* API for box services *********************/

/********************************************************************/
/******** public type definitions for Box Services component ********/
/********************************************************************/

typedef enum
{
  HPC_ITEM_FAN,
  HPC_ITEM_POWERSUPPLY,
  HPC_ITEM_PTEMP_SENSOR,
  HPC_ITEM_SFP,
  HPC_ITEM_XFP
} HPC_ITEM_t;

/* Item state definition - for power supplies and fans */
typedef enum
{
  HPC_ITEMSTATE_NONE = 0,       /* N/A */
  HPC_ITEMSTATE_OPERATIONAL,
  HPC_ITEMSTATE_FAILED,
  HPC_ITEMSTATE_NOT_PRESENT,
  HPC_STATE_LAST
} HPC_BOXS_ITEM_STATE_t;

typedef enum
{
  HPC_TYPE_NONE = 0,        /* N/A */
  HPC_TYPE_FIXED,
  HPC_TYPE_REMOVABLE,
  HPC_TYPE_LAST
} HPC_BOXS_ITEM_TYPE_t;

/* Item state definition - for thermal sensors */
typedef enum
{
  HPC_TEMPSENSOR_STATE_NONE = 0,    /* N/A */
  HPC_TEMPSENSOR_STATE_NORMAL,
  HPC_TEMPSENSOR_STATE_WARNING,
  HPC_TEMPSENSOR_STATE_CRITICAL,
  HPC_TEMPSENSOR_STATE_SHUTDOWN,
  HPC_TEMPSENSOR_STATE_NOTPRESENT,
  HPC_TEMPSENSOR_STATE_NOTOPERATIONAL,
  HPC_TEMPSENSOR_STATE_LAST
} HPC_BOXS_TEMPSENSOR_STATE_t;

#define BOXS_TEMP_RANGE_START   (-100)
#define BOXS_TEMP_RANGE_END     (100)

/* Box services items data description */

typedef struct
{
  HPC_BOXS_ITEM_TYPE_t        itemType;
  HPC_BOXS_TEMPSENSOR_STATE_t tempSensorState;
  L7_int32                    temperature;
} HPC_TEMP_SENSOR_DATA_t;

typedef struct
{
  HPC_BOXS_ITEM_TYPE_t        itemType;
  HPC_BOXS_ITEM_STATE_t       itemState;
} HPC_POWER_SUPPLY_DATA_t;

typedef struct
{
  HPC_BOXS_ITEM_TYPE_t        itemType;
  HPC_BOXS_ITEM_STATE_t       itemState;
  L7_uint32                   fanSpeed;
  L7_uint32                   fanDuty;
} HPC_FAN_DATA_t;

typedef struct
{
  HPC_BOXS_ITEM_TYPE_t        itemType;
  HPC_BOXS_ITEM_STATE_t       itemState;
  L7_uint32                   port;      /* Which port (1-based) on this unit correlates to this item. */
} HPC_SFP_DATA_t;

typedef struct
{
  HPC_BOXS_ITEM_TYPE_t        itemType;
  HPC_BOXS_ITEM_STATE_t       itemState;
  L7_uint32                   port;      /* Which port (1-based) on this unit correlates to this item. */
} HPC_XFP_DATA_t;

/**************************************************************************
*
* @purpose  Get total count of temperature sensors, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalTempSensorCountGet(void);

/**************************************************************************
*
* @purpose  Get total count of power supplies, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalPowerSupplyCountGet(void);

/**************************************************************************
*
* @purpose  Get total count of fans, which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalFanCountGet(void);

/**************************************************************************
*
* @purpose  Get total count of SFPs which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalSfpCountGet(void);

/**************************************************************************
*
* @purpose  Get total count of XFPs which could be present in the system
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalXfpCountGet(void);

/**************************************************************************
*
* @purpose  Get temperature sensor status data
*
* @param    in - @b{(input)}        Number of sensor
* @param    data - @b{(output)}     Temperature sensor status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalTempSensorDataGet(L7_uint32 id, HPC_TEMP_SENSOR_DATA_t* data);

/**************************************************************************
*
* @purpose  Get power supply status data
*
* @param    in - @b{(input)}        Number of power supply
* @param    data - @b{(output)}     Power supply status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalPowerSupplyDataGet(L7_uint32 id, HPC_POWER_SUPPLY_DATA_t* data);

/**************************************************************************
*
* @purpose  Get fan status data
*
* @param    in - @b{(input)}        Number of fan
* @param    data - @b{(output)}     fan status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalFanDataGet(L7_uint32 id, HPC_FAN_DATA_t* data);

/**************************************************************************
*
* @purpose  Get SFP status data
*
* @param    in - @b{(input)}        SFP index
* @param    data - @b{(output)}     SFP status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalSfpDataGet(L7_uint32 id, HPC_SFP_DATA_t *data);

/**************************************************************************
*
* @purpose  Get XFP status data
*
* @param    in - @b{(input)}        XFP index
* @param    data - @b{(output)}     XFP status data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalXfpDataGet(L7_uint32 id, HPC_XFP_DATA_t *data);

/**************************************************************************
*
* @purpose  Returns value indicating if POE is supported by this platfrom.
*
* @param    none
*
* @returns  L7_uint32
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 sysapiHpcLocalPoeSupported(void);

/**************************************************************************
*
* @purpose  Returns the system OID for this platform.
*
* @param    *sysOID pointer to where the system OID should be copied.
*
* @returns  L7_SUCCESS - valid system OID returned.
* @returns  L7_FAILURE - unable to determine system OID.
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcLocalSystemOidGet(L7_uchar8 *sysOID);

/*********************************************************************
* @purpose  Reads the temperature of the SFP.
*
* @param    unit        - @b{(input)}   bcm unit Number.
* @param    port        - @b{(input)}   bcm Port Number.
* @param    temperature - @b{(output)}  temperature value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    temperature is returned in degrees Celsius
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTempRead(L7_uint32 unit, L7_uint32 port, L7_int32 *temperature);

/*********************************************************************
* @purpose  Reads the voltage of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    voltage  - @b{(output)}  voltage value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    voltage is returned in millivolts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagVoltageRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *voltage);

/*********************************************************************
* @purpose  Reads the current of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    current  - @b{(output)}  current value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    temperature is returned in degrees Celsius
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagCurrentRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *current);

/*********************************************************************
* @purpose  Reads the tx Power of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    txPower  - @b{(output)}  Transmit Power value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTxPwrRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txPower);

/*********************************************************************
* @purpose  Reads the rx Power of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    rxPower  - @b{(output)}  Receive Power value
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagRxPwrRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *rxPower);

/*********************************************************************
* @purpose  Reads the tx fault status of the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    txFault  - @b{(output)}  Transmit fault status
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    Power is returned in degrees microWatts
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagTxFaultRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txFault);

/*********************************************************************
* @purpose  Reads the signal LOS on the SFP port.
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    port     - @b{(input)}   bcm Port Number.
* @param    los      - @b{(output)}  los
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_NOT_SUPPORTED
*
* @notes    'los' is returned as 1 if LOS is asserted.
*
* @end
*********************************************************************/
L7_RC_t sysapiHpcDiagLosRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *los);

/*********************************************************************
* @purpose  SFP power control through CPLD/or other means
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    sfpPort  - @b{(input)}   bcm Port Number.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcDiagPowerCtrl(L7_uint32 unit, L7_uint32 port);

/*********************************************************************
* @purpose  SFP power control through CPLD/or other means
*
* @param    unit  - @b{(input)}   bcm unit Number.
* @param    port  - @b{(input)}   bcm port Number.
* @param    link  - @b{(input)}   link status.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcLedModeCustomization(int unit, int port, int link);

/*********************************************************************
* @purpose  XFP power control through CPLD/or other means
*
* @param    unit     - @b{(input)}   bcm unit Number.
* @param    xfpPort  - @b{(input)}   bcm Port Number.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcXfpPowerCtrl(L7_uint32 unit, L7_uint32 xfpPort);

/*********************************************************************
* @purpose  Notify power controller of presence of 10GBASE-T

* @param    unit     - @b{(input)}   bcm unit Number.
* @param    xfpPort  - @b{(input)}   bcm Port Number.
* @param    present  - @b{(input)}   present or not.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcNotify10GBASETPresence(L7_uint32 unit, L7_uint32 xfpPort,
                                     L7_BOOL present);
/*********************************************************************
* @purpose  Notify power controller of presence of SFP+

* @param    unit     - @b{(input)}   bcm unit Number.
* @param    xfpPort  - @b{(input)}   bcm Port Number.
* @param    present  - @b{(input)}   present or not.
*
* @returns  void
*
* @end
*********************************************************************/
void sysapiHpcNotifySfpPlusPresence(L7_uint32 unit, L7_uint32 sfpPort);

#ifdef L7_POE_PACKAGE
typedef enum
{
  SYSAPI_HPC_POE_MSG_ID_MIN = 0,
  SYSAPI_HPC_POE_MAIN_INIT,
  SYSAPI_HPC_POE_MAIN_AUTO_RESET_MODE,
  SYSAPI_HPC_POE_MAIN_DETECTION_MODE,
  SYSAPI_HPC_POE_MAIN_FW_VERSION,
  SYSAPI_HPC_POE_MAIN_LOGICAL_MAP_MODE,
  SYSAPI_HPC_POE_MAIN_NOTIFICATION_CONTROL,
  SYSAPI_HPC_POE_MAIN_OPERATIONAL_STATUS,
  SYSAPI_HPC_POE_MAIN_POWER_CONSUMPTION,
  SYSAPI_HPC_POE_MAIN_POWER_MGMT_MODE,
  SYSAPI_HPC_POE_MAIN_POWER_NOMINAL,
  SYSAPI_HPC_POE_MAIN_USAGE_THRESHOLD,
  SYSAPI_HPC_POE_PORT_INIT,
  SYSAPI_HPC_POE_PORT_ADMIN_STATE,
  SYSAPI_HPC_POE_PORT_COUNTERS,
  SYSAPI_HPC_POE_PORT_COUNTER_INVALID_SIGNATURE,
  SYSAPI_HPC_POE_PORT_COUNTER_MPS_ABSENT,
  SYSAPI_HPC_POE_PORT_COUNTER_OVERLOAD,
  SYSAPI_HPC_POE_PORT_COUNTER_POWER_DENIED,
  SYSAPI_HPC_POE_PORT_COUNTER_SHORT,
  SYSAPI_HPC_POE_PORT_DETECTION_MODE,
  SYSAPI_HPC_POE_PORT_DISCONNECT_TYPE,
  SYSAPI_HPC_POE_PORT_DETECTION_STATUS,
  SYSAPI_HPC_POE_PORT_ERROR_CODE,
  SYSAPI_HPC_POE_PORT_HIGH_POWER_MODE,
  SYSAPI_HPC_POE_PORT_OPERATIONAL_STATUSES,
  SYSAPI_HPC_POE_PORT_POWER_CLASSIFICATION,
  SYSAPI_HPC_POE_PORT_POWER_CUR_LIMIT,
  SYSAPI_HPC_POE_PORT_POWER_MAX_LIMIT,
  SYSAPI_HPC_POE_PORT_POWER_MIN_LIMIT,
  SYSAPI_HPC_POE_PORT_POWER_OUTPUT,
  SYSAPI_HPC_POE_PORT_POWER_OUTPUT_CURRENT,
  SYSAPI_HPC_POE_PORT_POWER_OUTPUT_VOLTAGE,
  SYSAPI_HPC_POE_PORT_POWER_PAIRS_CONTROL,
  SYSAPI_HPC_POE_PORT_POWER_PRIORITY,
  SYSAPI_HPC_POE_PORT_RESET,
  SYSAPI_HPC_POE_PORT_TEMPERATURE,
  SYSAPI_HPC_POE_PORT_VIOLATION_TYPE,
  SYSAPI_HPC_POE_TRAP_MAIN_INIT_COMPLETE,
  SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED,
  SYSAPI_HPC_POE_TRAP_PORT_CHANGE,
#ifdef PLAT_POE_FW_UPDATE
  SYSAPI_HPC_POE_FIRMWARE_DOWNLOAD_PROGRESS,
#endif
  SYSAPI_HPC_POE_MSG_ID_MAX
} SYSAPI_POE_CONTROLLER_MSG_t;

typedef enum
{
  SYSAPI_HPC_POE_CMD_GET = 0,
  SYSAPI_HPC_POE_CMD_SET,
  SYSAPI_HPC_POE_CMD_QUERY,             /* To determine if message is supported */
  SYSAPI_HPC_POE_CMD_TRAP,
  SYSAPI_HPC_POE_CMD_TYPE_MAX
} SYSAPI_HPC_POE_CMD_TYPE_t;

typedef enum
{
  SYSAPI_HPC_POE_CMD_REQUEST = 0,
  SYSAPI_HPC_POE_CMD_RESPONSE,
  SYSAPI_HPC_POE_CMD_DIR_MAX
} SYSAPI_HPC_POE_CMD_DIRECTION_t;

typedef struct
{
  L7_POE_TRAP_MAIN_THRESHOLD_t direction;
  L7_uint32                    allocPower;
} SYSAPI_POE_TRAP_DATA_THRESHOLD_CROSSED_t;

typedef struct
{
  L7_POE_TRAP_PORT_CHANGE_t      state;
  L7_POE_PORT_DETECTION_STATUS_t status;
} SYSAPI_POE_TRAP_DATA_PORT_CHANGE_t;

typedef struct
{
  L7_uint32           usageThreshold;
  L7_POE_POWER_MGMT_t powerMgmtMode;
  L7_uint32           autoResetMode;
  L7_uint32           notificationControlEnable;
  L7_POE_DETECTION_t  detectionType;
} SYSAPI_POE_TRAP_DATA_MAIN_INIT_t;

typedef struct SYSAPI_POE_MSG_s
{
  SYSAPI_POE_CONTROLLER_MSG_t    msgId;
  SYSAPI_HPC_POE_CMD_TYPE_t      cmdType;
  SYSAPI_HPC_POE_CMD_DIRECTION_t direction;
  L7_uint32                      correlator;
  L7_RC_t                        rc;
  nimUSP_t                       usp;
  union
  {
                                        /* SYSAPI_HPC_POE_MAIN_INIT */
    SYSAPI_POE_TRAP_DATA_MAIN_INIT_t mainInit;
    struct                              /* SYSAPI_HPC_POE_MAIN_AUTO_RESET_MODE */
    {
      L7_uint32 enable;
    } mainAutoResetMode;
    struct                              /* SYSAPI_HPC_POE_MAIN_DETECTION_MODE */
    {
      L7_POE_DETECTION_t detectionType;
    } mainDetectionType;
    struct                              /* SYSAPI_HPC_POE_MAIN_FW_VERSION */
    {
      L7_uchar8 *version;
    } mainFwVersion;
    struct                              /* SYSAPI_HPC_POE_MAIN_LOGICAL_MAP_MODE */
    {
      L7_uint32 enable;
    } mainLogicalMapMode;
    struct                              /* SYSAPI_HPC_POE_MAIN_NOTIFICATION_CONTROL */
    {
      L7_uint32 enable;
    } mainNotificationControl;
    struct                              /* SYSAPI_HPC_POE_MAIN_OPERATIONAL_STATUS */
    {
      L7_POE_SYSTEM_OPER_STATUS_t status;
    } mainOperationalStatus;
    struct                              /* SYSAPI_HPC_POE_MAIN_POWER_CONSUMPTION */
    {
      L7_uint32 power;
    } mainPowerConsumption;
    struct                              /* SYSAPI_HPC_POE_MAIN_POWER_MGMT_MODE */
    {
      L7_POE_POWER_MGMT_t mode;
    } mainPowerMgmtMode;
    struct                              /* SYSAPI_HPC_POE_MAIN_POWER_NOMINAL */
    {
      L7_uint32 power;
    } mainPowerNominal;
    struct                              /* SYSAPI_HPC_POE_MAIN_USAGE_THRESHOLD */
    {
      L7_uint32 percent;
    } mainUsageThreshold;
    struct                              /* SYSAPI_HPC_POE_PORT_INIT */
    {
      L7_uint32                 adminState;
      L7_POE_PORT_POWER_PAIRS_t powerPairs;
      L7_POE_PRIORITY_t         powerPriority;
      L7_uint32                 powerLimit;
      L7_POE_LIMIT_TYPE_t       powerLimitType;   /* violation type */
      L7_POE_DETECTION_t        detectionMode;
      L7_POE_POWER_DISC_t       disconnectType;   /* Port Disconnect Mechanism */
      L7_uint32                 highPowerMode;
    } portInit;
    struct                              /* SYSAPI_HPC_POE_PORT_ADMIN_STATE */
    {
      L7_uint32 adminState;
    } portAdminEnable;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTERS */
    {
      L7_uint32 mpsAbsentCounter;
      L7_uint32 invalidSignatureCounter;
      L7_uint32 powerDeniedCounter;
      L7_uint32 overLoadCounter;
      L7_uint32 shortCounter;
    } portCounters;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTER_INVALID_SIGNATURE */
    {
      L7_uint32 counter;
    } portCounterInvalidSignature;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTER_MPS_ABSENT */
    {
      L7_uint32 counter;
    } portCounterMpsAbsent;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTER_OVERLOAD */
    {
      L7_uint32 counter;
    } portCounterOverload;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTER_POWER_DENIED */
    {
      L7_uint32 counter;
    } portCounterPowerDenied;
    struct                              /* SYSAPI_HPC_POE_PORT_COUNTER_SHORT */
    {
      L7_uint32 counter;
    } portCounterShort;
    struct                              /* SYSAPI_HPC_POE_PORT_DETECTION_MODE */
    {
      L7_POE_DETECTION_t mode;
    } portDetectionMode;
    struct                              /* SYSAPI_HPC_POE_PORT_DISCONNECT_TYPE */
    {
      L7_POE_POWER_DISC_t type;
    } portDisconnectType;
    struct                              /* SYSAPI_HPC_POE_PORT_DETECTION_STATUS */
    {
      L7_POE_PORT_DETECTION_STATUS_t status;
    } portDetectionStatus;
    struct                              /* SYSAPI_HPC_POE_PORT_ERROR_CODE */
    {
      L7_uint32 code;
    } portErrorCode;
    struct                              /* SYSAPI_HPC_POE_PORT_HIGH_POWER_MODE */
    {
      L7_uint32 mode;
    } portHighPowerMode;
    struct                              /* SYSAPI_HPC_POE_PORT_OPERATIONAL_STATUSES */
    {
      L7_POE_PORT_DETECTION_STATUS_t detectionStatus;
      L7_POE_PORT_POWER_CLASS_t      powerClass;
      L7_uint32                      powerConsumed;
      L7_uint32                      voltage;
      L7_uint32                      current;
    } portOperationalStatuses;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_CLASSIFICATION */
    {
      L7_POE_PORT_POWER_CLASS_t class;
    } portPowerClassification;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_CUR_LIMIT */
    {
      L7_uint32 limit;
    } portPowerCurLimit;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_MAX_LIMIT */
    {
      L7_uint32 limit;
    } portPowerMaxLimit;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_MIN_LIMIT */
    {
      L7_uint32 limit;
    } portPowerMinLimit;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_OUTPUT */
    {
      L7_uint32 output;
    } portPowerOutput;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_OUTPUT_CURRENT */
    {
      L7_uint32 current;
    } portPowerOutputCurrent;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_OUTPUT_VOLTAGE */
    {
      L7_uint32 voltage;
    } portPowerOutputVoltage;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_PAIRS_CONTROL */
    {
      L7_uint32                 capable; /* get */
      L7_POE_PORT_POWER_PAIRS_t control; /* get or set */
    } portPowerPairs;
    struct                              /* SYSAPI_HPC_POE_PORT_POWER_PRIORITY */
    {
      L7_POE_PRIORITY_t priority;
    } portPowerPriority;
    struct                              /* SYSAPI_HPC_POE_PORT_TEMPERATURE */
    {
      L7_uint32 temperature;
    } portTemperature;
    struct                              /* SYSAPI_HPC_POE_PORT_VIOLATION_TYPE */
    {
      L7_POE_LIMIT_TYPE_t type;
    } portViolationType;
                                        /* SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED */
    SYSAPI_POE_TRAP_DATA_THRESHOLD_CROSSED_t trapMainThresholdCrossed;
                                        /* SYSAPI_HPC_POE_TRAP_PORT_CHANGE */
    SYSAPI_POE_TRAP_DATA_PORT_CHANGE_t trapPortChange;
#ifdef PLAT_POE_FW_UPDATE
    struct                              /* SYSAPI_HPC_POE_FIRMWARE_DOWNLOAD_PROGRESS */
    {
      L7_BOOL progress;
    } fwDownloadProgress;
#endif
  } cmdData;
} SYSAPI_POE_MSG_t;

/**************************************************************************
*
* @purpose  Send a PoE message to the HPC layer.
*
* @param    PoE Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE or other error code returned from controller
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcPoeMsgSend(SYSAPI_POE_MSG_t *msg);

/**************************************************************************
*
* @purpose  Send a PoE trap to the application layer.
*
* @param    L7_uint   port number
* @param    enum      trapType
* @param    void      pointer to trap data
*
* @returns  L7_SUCCESS  if trap successfully sent
* @returns  L7_FAILURE  otherwise
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t sysapiHpcPoeTrapSend(L7_uint port, SYSAPI_POE_CONTROLLER_MSG_t trapType, void *data);

#endif  /* L7_POE_PACKAGE */

int hpcLedCustomization(int unit, int port, int link);

#endif  /* SYSAPIHPCPROTO_HEADER */
