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
#ifndef HPC_POE_H
#define HPC_POE_H

#include "l7_product.h"
#include "sysapi_hpc.h"

#ifdef POE_HPC_DEBUG
#define POE_HPC_DPRINTF(format, ...)    fprintf(stdout, "%s -- " format "\r\n", __FUNCTION__, ## __VA_ARGS__); \
                                        fflush(stdout)
#define POE_HPC_DPRINTLINE(format, ...) fprintf(stdout, format, ## __VA_ARGS__)
#else
#define POE_HPC_DPRINTF(format, ...)    printf(format "\r\n", ## __VA_ARGS__)
#define POE_HPC_DPRINTLINE(format, ...) printf(format, ## __VA_ARGS__)
#endif

typedef enum
{
  HPC_POE_COMM_TYPE_UART,
  HPC_POE_COMM_TYPE_I2C,
  HPC_POE_COMM_TYPE_BSC,
  HPC_POE_COMM_TYPE_INVALID = 254
} HPC_POE_COMM_TYPE_t;

typedef struct
{
  L7_uchar8   powerBankId;
  L7_uint32   maxPower;
} HPC_POE_POWER_BANK_MAP_t;

typedef struct
{
  L7_uint32             baud_rate;             /* baud rate for serial port */
  L7_uint32             flags;
} HPC_POE_UART_PARMS_t;

typedef struct
{
  L7_uint32             phy_address;           /* I2C Address */
} HPC_POE_I2C_PARMS_t;

typedef struct
{
  L7_uint32             bcm_unit;               /* The BCM Unit used to communicate with the Card */
  L7_uint32             phy_address;           /* I2C Address */
  L7_uint32             flags;
  L7_uint32             speed;
} HPC_POE_BSC_PARMS_t;

typedef union
{
  HPC_POE_UART_PARMS_t uart;
  HPC_POE_I2C_PARMS_t  i2c;
  HPC_POE_BSC_PARMS_t  bsc;
} HPC_POE_COMM_PARMS_t;

typedef struct
{
  L7_uint32                poeCardId;
  L7_uchar8                *file_descr;           /* UART/I2C Read  Description (filename) */
  L7_int32                 poe_fd;                /* File descriptor placeholder */
  L7_uint32                poeSlotId;             /* Device Id for this card, bcm_unit in case of BRCM Controller */
  HPC_POE_COMM_TYPE_t      type;                  /* Communication Type */
  HPC_POE_COMM_PARMS_t     parms;
  L7_uint32                numOfPorts;            /* number of PoE ports supported on this card */
  L7_uint32                start_phy_port;        /* Starting Physical Front Panel port number for this card */
  /*
   * text describing this card type
   */
  L7_uchar8                poeCardDescription[SYSAPI_HPC_CARD_DESCRIPTION_SIZE];

  L7_uint32                poe_default_power;     /* Default Power that can be supplied per PoE Card */
  L7_uint32                poe_rps_power;         /* Maximum Power that can be supplied */
  L7_uint32                num_of_power_banks;    /* Number of Power Banks a card supports */
  HPC_POE_POWER_BANK_MAP_t power_bank_map[L7_MAX_POE_POWER_BANKS_PER_CARD];

} HPC_POE_CARD_DATA_t;

typedef struct
{
  L7_uint32  phyPortNum;                /* Front Panel port number (from u/s/p) */
  L7_uchar8  phyPoePort;                /* PoE controller physical port number */
  L7_uint32  cardIndex;                 /* Index into nonRemovablePoeCardTable. */
  L7_uchar8  logicalPoePort;            /* Logical PoE port */
} HPC_POE_CARD_PORT_MAP_t;

typedef struct
{
  HPC_POE_CARD_PORT_MAP_t  *portMap;     /* Contains the Physical port to PoE port mapping */
  L7_ushort16              numOfSlotMapEntries;

} HPC_POE_UNIT_ENTRY_t;

typedef struct
{
  L7_uint32                unitTypeId;            /* unique identifier of this unit type */

  L7_uint32                numNonRemovablePoeCards;  /* number of entries in the non-removable card table */

  /* Slot mapping table for logical card types that are always created
  ** on a platform. The logical cards are not removable.
  */
  HPC_POE_CARD_DATA_t       nonRemovablePoeCardTable[L7_MAX_POE_CARDS_PER_UNIT];
  HPC_POE_UNIT_ENTRY_t     *poeUnitInfo;
} HPC_POE_UNIT_DESCRIPTOR_t;

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
L7_uint32 hpcPoeSupportedUnitsNumGet(void);

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
L7_uint hpcPoeCardCountGet(void);

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
HPC_POE_CARD_DATA_t *hpcPoeCardDataGet(L7_uint cardIndex);

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
L7_BOOL hpcPoeCardIndexIsValid(L7_uint cardIndex);

/**************************************************************************
*
* @purpose  Returns FD that corresponds to the
*           card ID provided.
*
* @param    cardIndex    Identifier for the PoE Card
*           write_fd     Return the Write FD saved for this PoE Card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbFdGet(L7_uint32 cardIndex, L7_uint32 *write_fd);

/**************************************************************************
*
* @purpose  Returns Communication Mechanism (UART or I2C) that corresponds to the
*           card ID provided.
*
* @param    cardIndex    Identifier for the PoE Card
*           read_fd  Return the Read FD saved for this PoE Card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbCommTypeGet(L7_uint32 cardIndex, HPC_POE_COMM_TYPE_t *comm_type);

/**************************************************************************
*
* @purpose  Returns Number of Power Banks that correspond to the
*           card ID provided.
*
* @param    cardIndex    Identifier for the PoE Card
*           read_fd  Return the Read FD saved for this PoE Card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeCardDbPowerBanksNumGet(L7_uint32 cardIndex, L7_uint32 *num_banks);

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
L7_RC_t hpcPoeUnitCardIndexGetFromPhyPort(L7_uint32 phy_port, L7_uint32 *cardIndex);

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
L7_RC_t hpcPoeUnitCardIndexGetFromPhyPoePort(L7_uint32 phy_port, L7_uint32 *cardIndex);

/**************************************************************************
*
* @purpose  Returns the Physical Front Panel Port that corresponds to the
*           Physical PoE Port provided.
*
* @param    phy_port     Return the Physical Front Panel Number
*           poe_port     Physical PoE port for the PoE Card
*
* @returns  L7_SUCCESS  Identifier is Valid
* @returns  L7_FAILURE  the index was not valid
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoeUnitPhyPortGetFromPoePort(L7_uint32 poe_port, L7_uchar8 *phy_port);

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
L7_RC_t hpcPoeUnitPoePortGetFromPhyPort(L7_uint32 phy_port, L7_uchar8 *poe_port);

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
L7_RC_t hpcPoeUnitLogicalPoePortGetFromPhyPort(L7_uint32 phy_port, L7_uchar8 *logical_poe_port);

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
L7_RC_t hpcPoeBcmPortFromPhyPortGet(L7_uint32 slot, L7_uint32 phy_port, L7_uint32 *bcm_cpuunit, L7_uint32 *bcm_port);

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
HPC_POE_UNIT_DESCRIPTOR_t *hpcPoeUnitDbLookup(L7_uint32 unitTypeId);

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
HPC_POE_UNIT_DESCRIPTOR_t *hpcPoeLocalUnitDescriptorGet(void);

/*******************************************************************************
*
* @purpose To Open a Device for Communication with the PoE controller
*          a) UART  -   Serial Communication with PoE controller
*          b) I2C   -   I2C communication with the Controller
*          c) BSC   -   I2C Communication with Controller (Applicable to Broadcom)
*
* @param   HPC_POE_COMM_TYPE_t    comm_type     Communication Mechanism
*          L7_uint32              address       I2C/UART Physical Address
*          L7_uchar8              *descriptor   UART (tyCo) or I2C Device Name
*          DEVICE_FLAGS_t         rw_flag       Write_Only, Read_Only or Read/Write
*          L7_uchar8              flags         Flags to be used for the Hardware Device
*
* @returns File Descriptor
*          -1, if command fails
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_int32 poe_device_open(HPC_POE_COMM_TYPE_t comm_type, const HPC_POE_COMM_PARMS_t *parms,
                         L7_uchar8 *descriptor, L7_uchar8 flags);

/*******************************************************************************
*
* @purpose To send data to the PoE subsystem
*
* @param   L7_uint32              CardIndex     PoE Card Index
*          L7_uchar8              *tx_buf       Transmit Buffer
*          L7_uint32              len           Length of the Transmit Buffer
*
* @returns Number of the bytes written
*          -1, if command fails
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_int32 poe_device_write(L7_uint32 cardIndex, L7_uchar8 *tx_buf, L7_uint32 len);

/*******************************************************************************
*
* @purpose To receive data from the PoE subsystem
*
* @param   L7_uint32              CardIndex     PoE Card Index
*          L7_uchar8              *rx_buf       Receive Buffer
*          L7_uint32              len           Length of the Receive Buffer
*
* @returns Number of the bytes written
*          -1, if command fails
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_int32 poe_device_read(L7_uint32 cardIndex, L7_uchar8 *rx_buf, L7_uint32 len);

#endif  /* HPC_POE_H */
