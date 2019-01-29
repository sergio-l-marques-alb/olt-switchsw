/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_db_util.c
*
* @purpose   Utility functions for the access to hpc database elements.
*
* @component hpc
*
* @create    08/08/2003
*
* @author    jeffr
* @end
*
*********************************************************************/


#include "sysapi.h"
#include "sysapi_hpc.h"

#include "hpc_db.h"
#include "registry.h"
#include "l7_packet.h"
#include "log.h"
#include "string.h"
#include "bspcpu_api.h"
#include "sysbrds.h"
#include "simapi.h"
#include <soc/drv.h>

/* PTin added: logger */
#include "logger.h"

HPC_CARD_DESCRIPTOR_t *hpcCardDbLookup(L7_uint32 cardTypeId);
extern HPC_BROAD_INTF_TYPE_MAC_ALLOCATION_t uni_mac_allocation_table[];
extern L7_uint32 uni_mac_allocation_table_num_entries;
static L7_enetMacAddr_t sys_mac = {{0, 0, 0, 0, 0, 0}};

static hpcCardEventNotifyList_t hpcCardEventNotifyList[L7_LAST_COMPONENT_ID];
static L7_uint32                localCardIDs[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
static L7_uint32                localPortCnt[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
static L7_uint32                localPhysicalPortCount = 0;


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
L7_RC_t hpcHardwareLocalUnitIdentifierMacGet(L7_enetMacAddr_t *mac)
{
  L7_enetMacAddr_t zero_mac;
  L7_RC_t rc = L7_SUCCESS;

  memset(&zero_mac, 0, sizeof(zero_mac));

  /* if the static variable sys_mac is zeros, fetch MAC from registry, once we get a non-zero
  ** MAC from registry, we never change is during this boot
  */
  if(memcmp(&sys_mac.addr, &zero_mac.addr, sizeof(sys_mac.addr)) == 0)
  {
    if (bspapiMacAddrGet((L7_char8 *)&sys_mac) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
              "%s: bspapiMacAddrGet returned Error\n", __FUNCTION__);
      rc = L7_ERROR;
    }
  }

  memcpy((void *)&(mac->addr), (void *)&(sys_mac.addr), sizeof(sys_mac.addr));
  return(rc);
}

/**************************************************************************
*
* @purpose  Gets the MAC address allocation policy for the platform.  The
*           policy returned is one of the ENUM SYSAPI_MAC_POLICY_t.
*
* @param    none
*
* @returns  SYSAPI_MAC_POLICY_t
*
* @notes
*
* @end
*
*************************************************************************/
SYSAPI_MAC_POLICY_t hpcHardwareMacAllocationPolicyGet(void)
{
  return(SYSAPI_MAC_POLICY_MGMT_UNIT_POOL);
}

/**************************************************************************
*
* @purpose  Gets the MAC address for a given (slot, port) on the local
*           unit.
*
* @param    none
*
* @returns  L7_SUCCESS  no problems getting MAC address
* @returns  L7_ERROR    (slot, port) data invalid, or other error in MAC
*                       address retrieval
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareIfaceMacGet(L7_INTF_TYPES_t type, L7_uint32 slot, L7_uint32 port,
                               L7_uchar8 *l2_mac_addr, L7_uchar8 *l3_mac_addr,
                               HPC_UNIT_DESCRIPTOR_t *local_unit_desc)
{
  L7_char8 sysMac[6];
  HPC_BROAD_UNIT_DATA_t *hpc_data_ptr;
  L7_uint32 totalPortMacOffset = 0;
  L7_uint32 i;
  L7_uint32 p;
  L7_RC_t rc = L7_ERROR;

  simMacAddrGet(sysMac);

  switch(hpcHardwareMacAllocationPolicyGet())
  {
  case SYSAPI_MAC_POLICY_MGMT_UNIT_POOL:
    for (i=0; i < uni_mac_allocation_table_num_entries; i++)
    {
      if (uni_mac_allocation_table[i].type == type)
      {
        if (l2_mac_addr != L7_NULLPTR)
        {
          sysapiSetMacAddress(l2_mac_addr, sysMac,
                              uni_mac_allocation_table[i].system_base_l2_mac_offset);
        }

        if (l3_mac_addr != L7_NULLPTR)
        {
          sysapiSetMacAddress(l3_mac_addr, sysMac,
                              uni_mac_allocation_table[i].system_base_l3_mac_offset);
        }

        rc = L7_SUCCESS;
      }
    }
    break;
  case SYSAPI_MAC_POLICY_REMOTE_UNIT_POOL:
    hpc_data_ptr = local_unit_desc->hpcPlatformData;
    /* make sure the slot/port we are being asked about will keep us in the mac allocation table */
    if (slot < hpc_data_ptr->num_mac_allocation_table_entries)
    {
      if (hpc_data_ptr->mac_allocation_table[slot].unique_mac_per_port == L7_TRUE)
      {
        /* the API uses 1-based port numbering... check lower bound of legal port numbering */
        if (port >= 1)
        {
          /* decrement port number since caller uses 1-based port numbering and code here uses 0-based port numbering */
          p = port - 1;
          if (p < hpc_data_ptr->mac_allocation_table[slot].num_mac_addrs_allocated_to_slot)
          {
            /* start with base offset for first port in this slot */
            totalPortMacOffset = hpc_data_ptr->mac_allocation_table[slot].system_base_mac_offset;
            /* add the port number to the slot base offset to get the total offset from system base mac
            ** to the interesting port
            */
            totalPortMacOffset += p;

            /* Set L2 and L3 MAC address to be same */
            sysapiSetMacAddress(l2_mac_addr, sysMac, totalPortMacOffset);
            sysapiSetMacAddress(l3_mac_addr, sysMac, totalPortMacOffset);
            rc = L7_SUCCESS;
          }
        }
      }
      else
      {
        sysapiSetMacAddress(l2_mac_addr, sysMac, hpc_data_ptr->mac_allocation_table[slot].system_base_mac_offset);
        sysapiSetMacAddress(l3_mac_addr, sysMac, hpc_data_ptr->mac_allocation_table[slot].system_base_mac_offset);
        rc = L7_SUCCESS;
      }
    }
    break;
  default:
    /* if we don't understand the value returned from hpcHardwareMacAllocatioPolicyGet(), just allow L7_ERROR to be our RC */
    break;
  }
  return(rc);
}

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
L7_BOOL hpcHardwareDriverSyncSupported(void)
{
  return L7_TRUE;
}


/**************************************************************************
*
* @purpose  Determine which cards are present in this unit.
*
* @param    NONE.
*
* @returns  L7_SUCCESS   if able to determine card types
* @returns  L7_FAILURE   if unable to determine card types
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcLocalCardIdDiscover(void)
{
  HPC_UNIT_DESCRIPTOR_t *local_unit_descriptor_ptr;
  HPC_CARD_DESCRIPTOR_t *card_descriptor_ptr;
  L7_uint32 i;
  int cardId;

  local_unit_descriptor_ptr = hpcLocalUnitDescriptorGet();
  if (local_unit_descriptor_ptr != L7_NULLPTR)
  {
    /* Store the card ID for any physical cards present */
    for (i = 0; i <local_unit_descriptor_ptr->unitTypeDescriptor.numPhysSlots; i++)
    {
      cardId = bspCpuReadCardID(i);
      if (cardId != -1) /* Card is present */
      {
        localCardIDs[i] = local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].supported_cards[cardId];
        card_descriptor_ptr = hpcCardDbLookup(localCardIDs[i]);
        if (card_descriptor_ptr != L7_NULL)
        {
          localPortCnt[i] = card_descriptor_ptr->cardTypeDescriptor.numOfNiPorts;
          localPhysicalPortCount += localPortCnt[i];
        }
        PT_LOG_TRACE(LOG_CTX_STARTUP,"Card id 0x%x present!",cardId);
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_STARTUP,"Card not present!");
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,"Cannot find board");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Return the local card id, given a slot number.
*
* @param    slotNum      slot number for requested card ID.
*
* @returns  cardId       if slot number is valid.
* @returns  0            if not card present or invalid slot number.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcLocalCardIdGet(int slotNum)
{
  L7_uint32 cardId = 0;

  if (slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT)
  {
    cardId =  localCardIDs[slotNum];
  }
  return(cardId);
}

/**************************************************************************
*
* @purpose  Register a callback function to be invoked when a card event
*           occurs.
*
* @param    notify       pointer to function
* @param    registrarID  one of enum L7_COMPONENT_IDS_t identifying the component
*                        to be called back
*
* @returns  L7_SUCCESS   if registrarID is valid
* @returns  L7_FAILURE   if registrarID is invalid
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareCardEventCallbackRegister(void (*notify)(L7_uint32 slotNum, L7_uint32 cardTypeId, L7_uint32 event),
                                             L7_uint32 registrarID)
{
  HPC_UNIT_DESCRIPTOR_t *local_unit_descriptor_ptr;
  L7_uint32 i;
  int cardId;

  /* record data about registrant in callback table */
  hpcCardEventNotifyList[registrarID].notify =  notify;
  hpcCardEventNotifyList[registrarID].registrarID = registrarID;

  local_unit_descriptor_ptr = hpcLocalUnitDescriptorGet();
  if ((notify != L7_NULLPTR) && (local_unit_descriptor_ptr != L7_NULLPTR))
  {
    /* generate plug events for any logical cards listed in the unit descriptor for the local unit */
    for (i = 0; i <local_unit_descriptor_ptr->unitTypeDescriptor.numNonRemovableCards; i++)
    {
      hpcCardEventNotifyList[registrarID].notify(local_unit_descriptor_ptr->unitTypeDescriptor.nonRemovableCardTable[i].slot_number,
                                                 local_unit_descriptor_ptr->unitTypeDescriptor.nonRemovableCardTable[i].cardTypeId,
                                                 SYSAPI_CARD_EVENT_PLUG);
    }
    /* generate plug events for any physical cards present and ready */
    for (i = 0; i <local_unit_descriptor_ptr->unitTypeDescriptor.numPhysSlots; i++)
    {
      if (local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].pluggable == L7_FALSE)
      {
        /* For non-pluggable cards, only ID 0 is supported */
        cardId = 0;
      }
      else
      {
        cardId = bspCpuReadCardID(i);
      }
      if (cardId != -1) /* Card is present */
      {
        hpcCardEventNotifyList[registrarID].notify(local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].slot_number,
                                                   local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].supported_cards[cardId],
                                                   SYSAPI_CARD_EVENT_PLUG);
      }
    }
  }
  else
  {
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Notifies the plug-in events for physical card & logical card
*           plug-in to callback function.
*
* @param    notify       pointer to function
*
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t hpcHardwareCardEventCallbackNotify(void (*notify)(L7_uint32 slotNum,
                                   L7_uint32 cardTypeId, hpcEvent_t eventInfo))
{
  /* Since this is hardware specific functionality, this is implemented in BSP
   * folder only. For all platforms, this needs be implemented in BSP folder
   * only.
   */
  HPC_UNIT_DESCRIPTOR_t *local_unit_descriptor_ptr;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *cardTypeDb;
  L7_uint32 cardTypeId;
  L7_uint32 i;
  int cardId;
  hpcEvent_t hpcEventInfo;

  local_unit_descriptor_ptr = hpcLocalUnitDescriptorGet();
  if ((notify != L7_NULLPTR) && (local_unit_descriptor_ptr != L7_NULLPTR))
  {
    /* generate plug events for any logical cards listed in the unit
     * descriptor for the local unit
     */
    for (i = 0;
        i < local_unit_descriptor_ptr->unitTypeDescriptor.numNonRemovableCards;
             i++)
    {
      hpcEventInfo.event =  SYSAPI_CARD_EVENT_PLUG;
      cardTypeId = local_unit_descriptor_ptr->unitTypeDescriptor.nonRemovableCardTable[i].
                                                                 cardTypeId;
      cardTypeDb = sysapiHpcCardDbEntryGet(cardTypeId);
      if (cardTypeDb == NULL)
      {
        return(L7_FAILURE);
      }
      hpcEventInfo.moduleType  = cardTypeDb->type;
      hpcEventInfo.moduleIndex = SYSAPI_CARD_INDEX_INVALID;

      notify(
        local_unit_descriptor_ptr->unitTypeDescriptor.nonRemovableCardTable[i].
                                                                slot_number,
        cardTypeId,
        hpcEventInfo);
    }
    /* generate plug events for any physical cards present and ready */
    for (i = 0; i <local_unit_descriptor_ptr->unitTypeDescriptor.numPhysSlots;
                    i++)
    {
      hpcEventInfo.event =  SYSAPI_CARD_EVENT_PLUG;

      if (local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].pluggable == L7_FALSE)
      {
        /* For non-pluggable cards, only ID 0 is supported */
        cardId = 0;
      }
      else
      {
        cardId = bspCpuReadCardID(i);
      }
      if (cardId != -1) /* Card is present */
      {
        cardTypeId = local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].supported_cards[cardId];
        cardTypeDb = sysapiHpcCardDbEntryGet(cardTypeId);
        if (cardTypeDb == NULL)
        {
          return(L7_FAILURE);
        }
        hpcEventInfo.moduleType  = cardTypeDb->type;
        hpcEventInfo.moduleIndex = SYSAPI_CARD_INDEX_INVALID;

        notify(local_unit_descriptor_ptr->unitTypeDescriptor.physSlot[i].slot_number,
               cardTypeId,
               hpcEventInfo);
      }
    }
  }
  else
  {
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

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
L7_uint32 hpcLocalPhysPortCntGet(void)
{
  return(localPhysicalPortCount);
}

/**************************************************************************
*
* @purpose  Return the number of physical ports, given a local unit/slot number.
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
L7_uint32 hpcLocalPhysPortsInSlotGet(int slotNum)
{
  L7_uint32 portCount = 0;

  if (slotNum < L7_MAX_PHYSICAL_SLOTS_PER_UNIT)
  {
    portCount =  localPortCnt[slotNum];
  }
  return(portCount);
}

extern L7_int32 cpu_card_id;
L7_int32 unit_type_id = -1;
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
L7_RC_t hpcLocalCardDbInit(void *board_info)
{

  L7_uint32 temp32;
  L7_char8  serialString[128] = "none";
  L7_char8  maintLevel[8] = "A";
  L7_int32  rc = L7_SUCCESS;
  L7_ushort16 bcmManuId = 0xbc00;
  HPC_UNIT_DESCRIPTOR_t *unitDescriptPtr;
  L7_uchar8 bcm_device_name[20] = { '\0', };
  L7_int32 npd_type_id;
  L7_int32 vendorId, deviceId;
  size_t revIndex;
#ifdef FEAT_METRO_CPE_V1_0
  L7_char8  hw_ver[6] = {'\0'};
#endif
  const bcm_sys_board_t *sysBoardPtr = board_info;


  /* Read ids from bcm_sys_board_t */
  unit_type_id = sysBoardPtr->unit_id;
  npd_type_id  = sysBoardPtr->npd_id;

  vendorId = 0x14e4; /*Broadcom VendorId*/
  deviceId = sysBoardPtr->dev_id[sysBoardPtr->num_units-1]; /*Broadcom VendorId*/


#ifdef FEAT_METRO_CPE_V1_0
  {
    L7_uchar8 board_id;
    if (unit_type_id == UNIT_BROAD_6_GIG_53115_REV_1_ID &&
        bspCpuReadBoardID(&board_id) == L7_SUCCESS)
    {
      if (board_id)
      {
        unit_type_id = UNIT_BROAD_6_GIG_53115_REV_2_ID;
      }
    }
  }
#endif
  /*
   *now write the registry
   */
  sysapiRegistryPut(CPU_CARDID,   U32_ENTRY, (void *)&cpu_card_id);
  sysapiRegistryPut(UNIT_TYPE_ID, U32_ENTRY, (void *)&unit_type_id);
  sysapiRegistryPut(BASE_CARDID,  U32_ENTRY, (void *)&unit_type_id);

 /*
  *add the broadcom chip as the NPD device
  */
  sprintf(bcm_device_name, "%s", soc_dev_name(0));
  sysapiRegistryPut(NPD_TYPE_ID,     U32_ENTRY, (void *)&npd_type_id);
  sysapiRegistryPut(NPD_TYPE_STRING, STR_ENTRY, (void *)bcm_device_name);

  revIndex = strcspn(bcm_device_name, "_");
  bcm_device_name[revIndex] = '\0';
  sysapiRegistryPut(PART_NUM,        STR_ENTRY, (void *)bcm_device_name);

  temp32 = L7_COLDRESET;
  sysapiRegistryPut(RESET_TYPE, U32_ENTRY, (void *) &temp32);


  temp32 = L7_MAX_FRAME_SIZE;
  sysapiRegistryPut(MTU_SIZE,  U32_ENTRY, (void *) &temp32);
  temp32 = 0;
  sysapiRegistryPut(PHYS_SIZE, U32_ENTRY, (void *) &temp32);

  FeatureKeyInit();

  /*
  * Call the cpu read function for serial number.
  */
  bspCpuReadSerialNum(serialString);
  sysapiRegistryPut(SERIAL_NUM,    STR_ENTRY, &serialString);

  /* Call the cpu read functions for MAC address & serial number. */
  sysapiRegistryPut(MFGR,         U16_ENTRY, &bcmManuId);
  sysapiRegistryPut(MAINT_LEVEL,  STR_ENTRY, &maintLevel);

  /* To fill VendorID, DeviceID */
  sysapiRegistryPut(VENDOR_ID,  U32_ENTRY, &vendorId);
  sysapiRegistryPut(DEVICE_ID,  U32_ENTRY, &deviceId);

#ifdef FEAT_METRO_CPE_V1_0
  /* To fill Hardware Version */
  bspCpuReadHwVersion(hw_ver);
  sysapiRegistryPut(HW_VERSION, STR_ENTRY, &hw_ver);
#endif
  sysapiRegistryPut(SYSTEM_DESC, STR_ENTRY, L7_DEFAULT_SYSTEM_DESCRIPTION);

  /* Retreive the machine type & model from the card database */
  unitDescriptPtr = hpcLocalUnitDescriptorGet();
  if (unitDescriptPtr)
  {
    sysapiRegistryPut(MACHINE_TYPE,  STR_ENTRY,
                      &unitDescriptPtr->unitTypeDescriptor.unitDescription);
    sysapiRegistryPut(MACHINE_MODEL, STR_ENTRY,
                      &unitDescriptPtr->unitTypeDescriptor.unitModel);
  }

  hpcLocalCardIdDiscover();

  sysapiHpcSlotPortCountPopulate();

  return(rc);
}
