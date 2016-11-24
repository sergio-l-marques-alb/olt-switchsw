/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename registry.h
*
* @purpose Place holder for all system registry information
*
* @component sysapi
*
* @comments none
*
* @create 7/27/2000
*
* @author paulq
* @end
*
**********************************************************************/

#ifndef REGISTRY_H
#define REGISTRY_H

/************************************************************************************
* Please put a comment off to the side of any added enum as to the component owner
* and the entry's size type.
* Do not add entries past LAST_REGISTRY_INDEX!
* This enum is used in array, don;t use any specific values for individual elements
* The FIRST_REGISTRY_INDEX can be changed if you want
************************************************************************************/

typedef enum
{
  FIRST_REGISTRY_INDEX = 100,  /* don't use very big value to save space when -fshort-enums is used */
  CPU_CARDID,                  /* sysapi component entry type=U32_ENTRY */
  CPU_SPEED,                   /* sysapi component entry type=U32_ENTRY */
  CPU_I_CACHE,                 /* sysapi component entry type=U32_ENTRY */
  CPU_D_CACHE,                 /* sysapi component entry type=U32_ENTRY */
  BASE_CARDID,                 /* entry type=U32_ENTRY                  */
  SYSTEM_DESC,                 /* sysapi component entry type=STR_ENTRY */
  SYS_MAC,                     /* sysapi component entry type=MAC_ENTRY */
  SYS_MAC_COUNT,               /* sysapi component entry type=U32_ENTRY */
  SYS_MAC_TYPE,                /* sysapi component entry type=U32_ENTRY */
  SYS_HOSTNAME,                /* sysapi component entry type=STR_ENTRY */
  SYS_IPADDR,                  /* sysapi component entry type=U32_ENTRY */
/*SLOTS_SUPPORTED,                sysapi component entry type=U32_ENTRY */
/*SLOT_DATA_STRUCT,               sysapi component entry type=SLOT_ENTRY */
/*PORT_DATA_STRUCT,               dapi   component entry type=PORT_ENTRY */
/*NUM_CPU_INTF,                   sysapi component entry type=U32_ENTRY */
/*NUM_STACK_INTF,                 sysapi component entry type=U32_ENTRY */
/*NUM_LAG_INTF,                   ??? component entry type=U32_ENTRY    */
/*NUM_VLAN_INTF,                  ??? component entry type=U32_ENTRY    */
  SYSTEM_MEMORY_BASEADDR,      /* sysapi component entry type=U32_ENTRY */
  SYSTEM_MEMORY_SIZE,          /* sysapi component entry type=U32_ENTRY */
/*  BULK_STORAGE_BASEADDR,        sysapi component entry type=U32_ENTRY */
/*  BULK_STORAGE_TYPE,            sysapi component entry type=U32_ENTRY */
/*  BULK_STORAGE_SIZE,            sysapi component entry type=U32_ENTRY */
  MEMORY_PKTBUFFER_BASEADDR,   /* sysapi component entry type=U32_ENTRY */
  MEMORY_PKTBUFFER_SIZE,       /* sysapi component entry type=U32_ENTRY */
  MEMORY_PKTPROC_BASEADDR,     /* sysapi component entry type=U32_ENTRY */
  MEMORY_PKTPROC_SIZE,         /* sysapi component entry type=U32_ENTRY */
  L7_MBUF_SIZE,                /* sysapi component entry type=U32_ENTRY */
  MTU_SIZE,                    /* ??? component entry type=U32_ENTRY    */
  PHYS_SIZE,                   /* ??? component entry type=U32_ENTRY    */
  RESET_TYPE,                  /* ??? component entry type=U32_ENTRY    */
  NP0_BASEADDR,
  NP1_BASEADDR,
  MACHINE_TYPE,                /* entry type=STR_ENTRY */
  MACHINE_MODEL,               /* entry type=STR_ENTRY */
  SERIAL_NUM,                  /* entry type=STR_ENTRY */
  FRU,                         /* entry type=STR_ENTRY */
  PART_NUM,                    /* entry type=STR_ENTRY */
  MAINT_LEVEL,                 /* entry type=STR_ENTRY */
  MFGR,                        /* entry type=U16_ENTRY */
  SW_VERSION,                  /* entry type=STR_ENTRY */
  SW_FILENAME,                 /* entry type=STR_ENTRY */
  SW_FILE_TIMESTAMP,           /* entry type=STR_ENTRY */
  SW_CODE_SIZE,                /* entry type=U32_ENTRY */
  OS_HEAPADDR,                 /* osapi component entry type=U32_ENTRY */
  OS_HEAPSIZE,                 /* osapi component entry type=U32_ENTRY */
  OS,                          /* entry type=STR_ENTRY */
  OS_TYPE,                     /* entry type=U32_ENTRY - see os_entry_type enums below */
  NPD_TYPE_STRING,             /* entry type=STR_ENTRY */
  NPD_TYPE_ID,                 /* entry type=U32_ENTRY */
  UNIT_TYPE_ID,                /* entry type=U32_ENTRY */
  OID_STRING,                  /* entry type=STR_ENTRY */
  SERVICE_TAG,                 /* entry type=STR_ENTRY */
  ASSET_TAG,                   /* entry type=STR_ENTRY */
  VENDOR_ID,                /* entry type=U32_ENTRY */
  DEVICE_ID,                /* entry type=U32_ENTRY */
  HW_VERSION,                 /* entry type=STR_ENTRY */
  LAST_REGISTRY_INDEX
} registry_t;

/*****************************************************************************
* See the OSAPI component owner if a new registry entry TYPE is needed.
*****************************************************************************/

typedef enum
{
  U8_ENTRY=1, /* entry contains an 8 bit value */
  U16_ENTRY,  /* entry contains a 16 bit value */
  U32_ENTRY,  /* entry contains a 32 bit value */
  MAC_ENTRY,  /* entry contains a ptr to a MAC address */
  STR_ENTRY,  /* entry contains a ptr to a NULL terminated string */
  PORT_ENTRY, /* entry contains a ptr to a slot[x].port_registry struct */
  SLOT_ENTRY, /* entry contains a ptr to a slot[x].slot_registry struct */
  IGNORE_SVC_PORT,
  LAST_REG_ENTRY_TYPE
} reg_entry_type;


typedef enum
{
  OS_VXWORKS=1,
  OS_LINUX
} os_entry_type;

#define  SW_FILENAME_MAX_LEN 128

/*****************************************************************************
* Prototype function definitions to get and put a registry entry's value.
*****************************************************************************/

/**************************************************************************
* @purpose  Initialize the system Registry
*
* @param    none.
*
* @returns  L7_SUCCESS
*
* @comments  Does minimal initialization
*
* @end
*************************************************************************/
L7_RC_t sysapiRegistryInit(void);

/**************************************************************************
* @purpose  Put a value into the registry
*
* @param reg_key     @b{(input)} registry key to put information
* @param entry_type  @b{(input)} registry key type to set entry to
* @param *buffer     @b{(output)} ptr to key information to place in registry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE key is out of range
*
* @comments    none
*
* @end
*************************************************************************/
L7_RC_t sysapiRegistryPut(registry_t reg_key, reg_entry_type entry_type, void *buffer);

/**************************************************************************
* @purpose  Pull a value out of the registry
*
* @param   reg_key    @b{(input)} registry key from which to get information
* @param   entry_type @b{(input)} registry key type
* @param   buffer     @b{(output)} user's mem - where to place key information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE key is out of range or entry type does not match current type of that key
*
* @comments    none
*
* @end
*************************************************************************/
L7_RC_t sysapiRegistryGet(registry_t reg_key, reg_entry_type entry_type, void *buffer);

/*****************************************************************************
* Registry data structures
*****************************************************************************/
typedef struct
{
  L7_uint32 port_type;
  L7_uint32 diag_state;
  L7_uint32 phy_capability;
  L7_uint32 connector_type;
  L7_uchar8 pad[2];
  L7_uchar8 mac[6];
} port_info;


typedef struct
{
  L7_uint32 numOfPortsInSlot;
  port_info port_info[L7_MAX_PORTS_PER_SLOT+1];    /* arrary index 0 not used */
} port_registry;


typedef struct slot_registry_t
{
  /*****************************************************************************************
  * The cardIdRev will be 32 bits in length. The value will be the concatentation
  * of the actual 2 byte CARDID and the 1 byte revision from the VPD.
  *****************************************************************************************/
  L7_uint32 cardIdRev;
  L7_uint32 baseAddress;
  L7_uchar8 baseMac[6];
  L7_uint32 macCount;
  L7_uint32 intlevel;

} slot_registry;

#endif
