/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_poe.c
*
* @purpose   Declare and initialize storage for use in hpc card
*            and unit descriptor databases for PoE.
*
* @component hpc
*
* @create    02/12/2003
*
* @author    jeffr
* @end
*
*********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "unitmgr_api.h"
#include "broad_hpc_db.h"
#include "hpc_poe.h"

/*******************************************************************************
*
* @Data    hpc_unit_descriptor_db
*
* @purpose An array of descriptors with an entry for each unit type
*          supported.
*
* @end
*
*******************************************************************************/
/* main unit descriptor table */
/* The following include file should be defined in the platform specific
 * include directory if PoE is present on the switch. */
HPC_POE_UNIT_DESCRIPTOR_t hpc_poe_unit_descriptor_db[] =
{
#include "hpc_poe_db.h"
};

const L7_uint32 hpc_num_supported_poe_unit_types = (sizeof(hpc_poe_unit_descriptor_db)/sizeof(hpc_poe_unit_descriptor_db[0]));

/**************************************************************************
* @purpose  Return the number of supported units.
*
* @param    none
*
* @returns  Number of supported units.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 hpcPoeSupportedUnitsNumGet(void)
{
  return(sizeof(hpc_poe_unit_descriptor_db)/sizeof(hpc_poe_unit_descriptor_db[0]));
}

/**************************************************************************
*
* @purpose  Ensures a card index is within the valid range.
*
* @param    cardIndex   The card index on the local unit.
*
* @returns  L7_TRUE if valid, else L7_FALSE.
*
* @notes
*
* @end
*
*************************************************************************/
L7_BOOL hpcPoeCardIndexIsValid(L7_uint cardIndex)
{
  if (cardIndex >= hpcPoeLocalUnitDescriptorGet()->numNonRemovablePoeCards)
  {
    return L7_FALSE;
  }
  else
  {
    return L7_TRUE;
  }
}

/**************************************************************************
*
* @purpose  Returns the number of PoE cards on the local unit.
*
* @param    none
*
* @returns  Number of PoE cards, zero if local unit not found.
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint hpcPoeCardCountGet(void)
{
  HPC_POE_UNIT_DESCRIPTOR_t *unitDescr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULLPTR == unitDescr)
  {
    return 0;
  }

  return hpcPoeLocalUnitDescriptorGet()->numNonRemovablePoeCards;
}

/**************************************************************************
*
* @purpose  Retrieves the card data from the local unit index.
*
* @param    cardIndex   The card index on the local unit.
*
* @returns  pointer to data record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_POE_CARD_DATA_t *hpcPoeCardDataGet(L7_uint cardIndex)
{
  HPC_POE_UNIT_DESCRIPTOR_t *unitDescr = hpcPoeLocalUnitDescriptorGet();

  if ((L7_NULLPTR == unitDescr) ||
      (cardIndex >= unitDescr->numNonRemovablePoeCards))
  {
    return L7_NULLPTR;
  }

  return &unitDescr->nonRemovablePoeCardTable[cardIndex];
}

/**************************************************************************
*
* @purpose  Returns FD that corresponds to the
*           card Index provided.
*
* @param    cardIndex    Identifier for the PoE Card
*           fd           Returns the FD saved for this PoE Card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbFdGet(L7_uint32 cardIndex, L7_uint32 *fd)
{
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);

  if (L7_NULLPTR == cardData)
  {
    return L7_FAILURE;
  }

  if (-1 == cardData->poe_fd)
  {
    return L7_FAILURE;
  }

  *fd = cardData->poe_fd;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns Communication Mechanism (UART or I2C) that corresponds to the
*           card ID provided.
*
* @param    cardIndex   Identifier for the PoE Card
*           comm_type   Returns the communication type used by the card.
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbCommTypeGet(L7_uint32 cardIndex, HPC_POE_COMM_TYPE_t *comm_type)
{
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);

  if (L7_NULLPTR == cardData)
  {
    return L7_FAILURE;
  }

  *comm_type = cardData->type;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns Number of Power Banks that correspond to the
*           card ID provided.
*
* @param    cardIndex   Identifier for the PoE Card
*           num_banks   Returns the number of power banks used by the card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbPowerBanksNumGet(L7_uint32 cardIndex, L7_uint32 *num_banks)
{
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);

  if (L7_NULLPTR == cardData)
  {
    return L7_FAILURE;
  }
  else
  {
    *num_banks = cardData->num_of_power_banks;
  }
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Returns Card Id that corresponds to the
*           Physical Front Panel Port provided.
*
* @param    cardIndex    Return Identifier for the PoE Card
*           phy_port     Physical Front Panel Port Number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitCardIndexGetFromPhyPort(L7_uint32 phy_port, L7_uint32 *cardIndex)
{
  int i;
  HPC_POE_CARD_PORT_MAP_t   *portMap;
  HPC_POE_UNIT_DESCRIPTOR_t *poe_unit_ptr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULL == poe_unit_ptr)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot find registered PoE Cards on the Unit");
    return L7_FAILURE;
  }

  portMap = poe_unit_ptr->poeUnitInfo->portMap;

  for (i = 0; i < poe_unit_ptr->poeUnitInfo->numOfSlotMapEntries; i++)
  {
    if (phy_port == portMap[i].phyPortNum)
    {
      *cardIndex = portMap[i].cardIndex;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns Card Id that corresponds to the
*           Physical PoE Port provided.
*
* @param    cardIndex    Return Identifier for the PoE Card
*           phy_port     Physical PoE Port Number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitCardIndexGetFromPhyPoePort(L7_uint32 phy_port, L7_uint32 *cardIndex)
{
  int i;
  HPC_POE_CARD_PORT_MAP_t   *portMap;
  HPC_POE_UNIT_DESCRIPTOR_t *poe_unit_ptr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULL == poe_unit_ptr)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot find registered PoE Cards on the Unit");
    return L7_FAILURE;
  }

  portMap = poe_unit_ptr->poeUnitInfo->portMap;

  for (i = 0; i < poe_unit_ptr->poeUnitInfo->numOfSlotMapEntries; i++)
  {
    if (phy_port == portMap[i].phyPoePort)
    {
      *cardIndex = portMap[i].cardIndex;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns the Physical PoE Port that corresponds to the
*           Physical Front Panel Port provided.
*
* @param    poe_port     Return the Physical PoE port for the PoE Card
*           phy_port     Physical Front Panel Port Number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitPhyPortGetFromPoePort(L7_uint32 poe_port, L7_uchar8 *phy_port)
{
  int i;
  HPC_POE_CARD_PORT_MAP_t   *portMap;
  HPC_POE_UNIT_DESCRIPTOR_t *poe_unit_ptr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULL == poe_unit_ptr)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot find registered PoE Cards on the Unit");
    return L7_FAILURE;
  }

  portMap = poe_unit_ptr->poeUnitInfo->portMap;

  for (i = 0; i < poe_unit_ptr->poeUnitInfo->numOfSlotMapEntries; i++)
  {
    if (poe_port == portMap[i].phyPoePort)
    {
      *phy_port = portMap[i].phyPortNum;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns the Physical PoE Port that corresponds to the
*           Physical Front Panel Port provided.
*
* @param    poe_port     Return the Physical PoE port for the PoE Card
*           phy_port     Physical Front Panel Port Number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitPoePortGetFromPhyPort(L7_uint32 phy_port, L7_uchar8 *poe_port)
{
  int i;
  HPC_POE_CARD_PORT_MAP_t   *portMap;
  HPC_POE_UNIT_DESCRIPTOR_t *poe_unit_ptr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULL == poe_unit_ptr)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot find registered PoE Cards on the Unit");
    return L7_FAILURE;
  }

  portMap = poe_unit_ptr->poeUnitInfo->portMap;

  for (i = 0; i < poe_unit_ptr->poeUnitInfo->numOfSlotMapEntries; i++)
  {
    if (phy_port == portMap[i].phyPortNum)
    {
      *poe_port = portMap[i].phyPoePort;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns the Logical PoE Port that corresponds to the
*           Physical Front Panel Port provided.
*
* @param    logical_poe_port     Return the Logical PoE port for the PoE Card
*           phy_port     Physical Front Panel Port Number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitLogicalPoePortGetFromPhyPort(L7_uint32 phy_port, L7_uchar8 *logical_poe_port)
{
  int i;
  HPC_POE_CARD_PORT_MAP_t   *portMap;
  HPC_POE_UNIT_DESCRIPTOR_t *poe_unit_ptr = hpcPoeLocalUnitDescriptorGet();

  if (L7_NULL == poe_unit_ptr)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot find registered PoE Cards on the Unit");
    return L7_FAILURE;
  }

  portMap = poe_unit_ptr->poeUnitInfo->portMap;

  for (i = 0; i < poe_unit_ptr->poeUnitInfo->numOfSlotMapEntries; i++)
  {
    if (phy_port == portMap[i].phyPortNum)
    {
      *logical_poe_port = portMap[i].logicalPoePort;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Returns the bcm cpu_unit and port from the
*           Physical Front Panel Port provided.
*
* @param    slot         The slot of the port (from usp)
* @param    phy_port     Physical Front Panel port for the PoE Card
*           bcm_cpuunit  bcm unit id
*           bcm_port     bcu port number
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeBcmPortFromPhyPortGet(L7_uint32 slot, L7_uint32 phy_port, L7_uint32 *bcm_cpuunit, L7_uint32 *bcm_port)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(slot));

  if (L7_NULLPTR != sysapiHpcCardInfoPtr)
  {
    hapiSlotMapPtr = ((DAPI_CARD_ENTRY_t *)(sysapiHpcCardInfoPtr->dapiCardInfo))->slotMap;

    if (phy_port < sysapiHpcCardInfoPtr->numOfNiPorts)
    {
      *bcm_cpuunit = hapiSlotMapPtr[phy_port - 1].bcm_cpuunit;
      *bcm_port    = hapiSlotMapPtr[phy_port - 1].bcm_port;

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  Retrieves a unit descriptor from the support-unit database.
*
* @param    unitTypeId   cardTypeId of entry used for lookup
*
* @returns  pointer to descriptor record, L7_NULLPTR if not found
*
* @notes
*
* @end
*
*************************************************************************/
HPC_POE_UNIT_DESCRIPTOR_t *hpcPoeUnitDbLookup(L7_uint32 unitTypeId)
{
  HPC_POE_UNIT_DESCRIPTOR_t *desc_ptr;
  L7_uint32 index;
  L7_uint32 found_index = 0;
  L7_BOOL   item_found = L7_FALSE;

  /* scan the table for desired data entry */
  for (index = 0; index < hpcPoeSupportedUnitsNumGet(); index++)
  {
    if (hpc_poe_unit_descriptor_db[index].unitTypeId == unitTypeId)
    {
      item_found = L7_TRUE;
      found_index = index;
      break;
    }
  }

  /* if we found an item meeting the lookup criteria, provide the data and set appropriate return code */
  if (item_found == L7_TRUE)
  {
    desc_ptr = &hpc_poe_unit_descriptor_db[found_index];
  }
  else
  {
    desc_ptr = (HPC_POE_UNIT_DESCRIPTOR_t *)L7_NULLPTR;
  }

  return desc_ptr;
}

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
HPC_POE_UNIT_DESCRIPTOR_t *hpcPoeLocalUnitDescriptorGet(void)
{
  static HPC_POE_UNIT_DESCRIPTOR_t *desc_ptr = L7_NULLPTR;
  const bcm_sys_board_t            *board_info;

  if (L7_NULLPTR == desc_ptr)
  {
    board_info = hpcBoardGet();
    if (NULL != board_info)
    {
      desc_ptr = hpcPoeUnitDbLookup(board_info->unit_id);
    }
  }

  return desc_ptr;
}

/*********************************************************************
* @purpose Debug function for dumping unit data
*
* @param   none
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hpcPoeDebugUnitDump(const HPC_POE_UNIT_DESCRIPTOR_t *hpcPoeUnitDescr)
{
  HPC_POE_UNIT_ENTRY_t *poeUnitInfo;
  const HPC_POE_CARD_DATA_t  *cardData;
  L7_uint i;

  poeUnitInfo = hpcPoeUnitDescr->poeUnitInfo;

  POE_HPC_DPRINTLINE("\r\n"
                     "unitTypeId = 0x%x\r\n", hpcPoeUnitDescr->unitTypeId);
  POE_HPC_DPRINTLINE("numNonRemovablePoeCards = %d\r\n", hpcPoeUnitDescr->numNonRemovablePoeCards);
  POE_HPC_DPRINTLINE("Card Table:\r\n");
  for (i = 0; i < hpcPoeUnitDescr->numNonRemovablePoeCards; i++)
  {
    cardData = &hpcPoeUnitDescr->nonRemovablePoeCardTable[i];
    POE_HPC_DPRINTLINE("card ID           = 0x%x\r\n",   cardData->poeCardId);
    POE_HPC_DPRINTLINE("file_descr        = \"%s\"\r\n", cardData->file_descr);
    POE_HPC_DPRINTLINE("poe_fd            = 0x%x\r\n",   cardData->poe_fd);
    POE_HPC_DPRINTLINE("Slot Id           = 0x%x\r\n",   cardData->poeSlotId);
    POE_HPC_DPRINTLINE("\r\nCommunication Parms:\r\n");
    POE_HPC_DPRINTLINE("Card Comm Type    = ");
    switch (cardData->type)
    {
      case HPC_POE_COMM_TYPE_UART:
        POE_HPC_DPRINTLINE("UART\r\n");
        POE_HPC_DPRINTLINE("Baud Rate         = %d\r\n",   cardData->parms.uart.baud_rate);
        POE_HPC_DPRINTLINE("Flags             = 0x%x\r\n", cardData->parms.uart.flags);
        break;
      case HPC_POE_COMM_TYPE_I2C:
        POE_HPC_DPRINTLINE("I2C\r\n");
        POE_HPC_DPRINTLINE("I2C Address       = 0x%x\r\n", cardData->parms.i2c.phy_address);
        break;
      case HPC_POE_COMM_TYPE_BSC:
        POE_HPC_DPRINTLINE("BSC\r\n");
        POE_HPC_DPRINTLINE("BCM Unit          = 0x%x\r\n", cardData->parms.bsc.bcm_unit);
        POE_HPC_DPRINTLINE("BSC Address       = 0x%x\r\n", cardData->parms.bsc.phy_address);
        POE_HPC_DPRINTLINE("Flags             = 0x%x\r\n", cardData->parms.bsc.flags);
        POE_HPC_DPRINTLINE("Speed             = 0x%x\r\n", cardData->parms.bsc.speed);
        break;
      default:
        POE_HPC_DPRINTLINE("Invalid\r\n");
        break;
    }
    POE_HPC_DPRINTLINE("Number of Ports   = %d\r\n",     cardData->numOfPorts);
    POE_HPC_DPRINTLINE("Starting Phy Port = %d\r\n",     cardData->start_phy_port);
    POE_HPC_DPRINTLINE("Card Description  = \"%s\"\r\n", cardData->poeCardDescription);
    POE_HPC_DPRINTLINE("Default Power     = %d\r\n",     cardData->poe_default_power);
    POE_HPC_DPRINTLINE("Max Power         = %d\r\n",     cardData->poe_rps_power);
    POE_HPC_DPRINTLINE("Num. Power Banks  = %d\r\n",     cardData->num_of_power_banks);
    POE_HPC_DPRINTLINE("Map for power banks:\r\n");
    POE_HPC_DPRINTLINE("Number ID   Max. Power\r\n");
    POE_HPC_DPRINTLINE("------ ---- ----------\r\n");
    for (i = 0; i < cardData->num_of_power_banks; i++)
    {
      POE_HPC_DPRINTLINE("%-6d %-4d %d\r\n", i,
                         cardData->power_bank_map[i].powerBankId,
                         cardData->power_bank_map[i].maxPower);
    }
  }
  POE_HPC_DPRINTLINE("Slot Map Entries:\r\n");
  POE_HPC_DPRINTLINE("PortNum  PoePort  LogPort  CardIndex\r\n"
                     "-------  -------  -------  ---------\r\n");
  for (i = 0; i < poeUnitInfo->numOfSlotMapEntries; i++)
  {
    POE_HPC_DPRINTLINE("%7u  %7u  %7u  %u\r\n",
                       poeUnitInfo->portMap[i].phyPortNum,
                       poeUnitInfo->portMap[i].phyPoePort,
                       poeUnitInfo->portMap[i].logicalPoePort,
                       poeUnitInfo->portMap[i].cardIndex);
  }
}

L7_int hpcPoeDebugUnitDbDump(void)
{
  L7_uint i;

  for (i = 0; i < hpcPoeSupportedUnitsNumGet(); i++)
  {
    hpcPoeDebugUnitDump(&hpc_poe_unit_descriptor_db[i]);
  }

  return i;
}
