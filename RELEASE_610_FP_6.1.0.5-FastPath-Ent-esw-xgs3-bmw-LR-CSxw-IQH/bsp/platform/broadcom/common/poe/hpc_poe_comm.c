/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  hpc_poe_comm.c
*
* @purpose   This file contains the communication code for PoE.
*
* @component hapi
*
* @comments
*
* @create    06 June 2008
*
* @author
*
* @end
*
**********************************************************************/
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef _L7_OS_VXWORKS_
#include <ioLib.h>
#include <errnoLib.h>
#endif

#include "l7_common.h"
#include "sysapi.h"
#include "hpc_poe.h"

#if defined(PLAT_POE_COMM_BSC)
#include "bcm/bcmi2c.h"
#include "soc/i2c.h"
#endif

#if !defined(PLAT_POE_COMM_UART) && !defined(PLAT_POE_COMM_I2C) && !defined(PLAT_POE_COMM_BSC)
#error "At least one of the communication mechanisms UART, I2C or BSC should be defined for PoE."
#endif

/*******************************************************************************
*
* @purpose To Open a Device for Communication with the PoE controller
*          a) UART  -   Serial Communication with PoE controller
*          b) I2C   -   I2C communication with the Controller
*          c) BSC   -   I2C Communication with Controller (Applicable to Broadcom)
*
* @param   L7_uint32              bcm_unit
* @param   SYSAPI_POE_COMM_TYPE_t comm_type     Communication Mechanism
* @param   HPC_POE_COMM_PARMS_t   parms         Parameters specific to communication method
* @param   L7_uchar8              *descriptor   UART (tyCo) or I2C Device Name
* @param   L7_uchar8              flags         Flags to be used for the Hardware Device
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
                         L7_uchar8 *descriptor, L7_uchar8 flags)
{
  L7_int32 file_descriptor = -1;
#if defined(PLAT_POE_COMM_UART)
  L7_uint32 poeErrno;
#endif

  if (comm_type == HPC_POE_COMM_TYPE_UART)
  {
#if defined(PLAT_POE_COMM_UART)
    file_descriptor = open(descriptor, O_RDWR, flags);

    poeErrno = errnoGet();

    if (-1 != file_descriptor)
    {
      /* Set the baud rate */
      if (-1 == ioctl(file_descriptor, FIOBAUDRATE, parms->uart.baud_rate))
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Could not set baud rate on %s.\r\n", descriptor);
      }
    }
#else
    printf("UART Communication Mechanism is not supported\n");
#endif
  }

  if (comm_type == HPC_POE_COMM_TYPE_I2C)
  {
#if defined(PLAT_POE_COMM_I2C)
    return 0;       /* No Need to do any initialization for I2C */
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }

  if (comm_type == HPC_POE_COMM_TYPE_BSC)
  {
#if defined(PLAT_POE_COMM_BSC)
    file_descriptor = bcm_i2c_open(parms->bsc.bcm_unit, descriptor, parms->bsc.flags, parms->bsc.speed);     /* First Param is BCM Unit and Last Param is Speed */
#else
    printf("BSC Communication Mechanism is not supported\n");
#endif
  }

  return file_descriptor;
}

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
L7_int32 poe_device_write(L7_uint32 cardIndex, L7_uchar8 *tx_buf, L7_uint32 len)
{
  int bytes = -1;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);

  if (L7_NULLPTR == cardData)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card Index %u", cardIndex);
    return -1;
  }

  if (cardData->type == HPC_POE_COMM_TYPE_UART)
  {
#ifdef PLAT_POE_COMM_UART
    bytes = write(cardData->poe_fd, tx_buf, len);
#else
    printf("UART Communication Mechanism is not supported\n");
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_I2C)
  {
#ifdef PLAT_POE_COMM_I2C
    bytes = i2cWrite(cardData->parms.i2c.phy_address, tx_buf, len);
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_BSC)
  {
#ifdef PLAT_POE_COMM_BSC
    if (cardData->poe_fd >=0) 
    {
        bytes = bcm_i2c_write(cardData->parms.bsc.bcm_unit, cardData->poe_fd,
                              0, tx_buf, len);
    }
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }

  return bytes;
}

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
L7_int32 poe_device_read(L7_uint32 cardIndex, L7_uchar8 *rx_buf, L7_uint32 len)
{
  int bytes = -1;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);

  if (L7_NULLPTR == cardData)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card Index %u", cardIndex);
    return -1;
  }

  if (cardData->type == HPC_POE_COMM_TYPE_UART)
  {
#ifdef PLAT_POE_COMM_UART
    bytes = read(cardData->poe_fd, rx_buf, len);
#else
    printf("UART Communication Mechanism is not supported\n");
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_I2C)
  {
#ifdef PLAT_POE_COMM_I2C
    bytes = i2cRead(cardData->parms.i2c.phy_address, rx_buf, len);
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_BSC)
  {
#ifdef PLAT_POE_COMM_BSC
    bytes = bcm_i2c_read(cardData->parms.bsc.bcm_unit, cardData->poe_fd,
                         0, rx_buf, &len);
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }

  return bytes;
}
