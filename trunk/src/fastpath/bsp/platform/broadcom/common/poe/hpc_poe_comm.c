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

#if defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
#include <termios.h> 
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
#if defined (PLAT_POE_COMM_UART) && (defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_))
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios serial_device;
  int baud;
#endif
#endif
#if defined(PLAT_POE_COMM_UART) && defined(_L7_OS_VXWORKS_)
  L7_uint32 poeErrno;
#endif

  if (comm_type == HPC_POE_COMM_TYPE_UART)
  {
#if defined(PLAT_POE_COMM_UART)
    if(parms->uart.mode == HPC_POE_COMM_MODE_NONBLOCKING)
    {
      file_descriptor = open(descriptor, O_RDWR|O_NONBLOCK, flags);
    }
    else
    {
      file_descriptor = open(descriptor, O_RDWR, flags);
    }
#if defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
    if (-1 != file_descriptor)
    {
    /* PTin removed: serial port */
    #if (L7_SERIAL_COM_ATTR)
      tcgetattr(file_descriptor, &serial_device);
      serial_device.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP |INLCR|IGNCR|ICRNL|IXON);
      serial_device.c_oflag &= ~OPOST;
      serial_device.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
      serial_device.c_cflag &= ~(CSIZE|PARENB);
      serial_device.c_cflag |= CS8;
        /* Set the baud rate */
      switch( parms->uart.baud_rate)
      {
         case 2400:
           baud = B2400;
         break;
         case 4800:
           baud = B4800;
         break;
         case 9600:
           baud = B9600;
         break;
         case 19200:
           baud = B19200;
         break;
         case 38400:
           baud = B38400;
         break;
         case 57600:
           baud = B57600;
         break;
         case 115200:
           baud = B115200;
         break;
         case 230400:
           baud = B230400;
         break;
         case 460800:
           baud = B460800;
         break;
         case 500000:
           baud = B500000;
         break;
         case 576000:
           baud = B576000;
         break;
         case 921600:
           baud = B921600;
         break;
         case 1000000:
           baud = B1000000;
         break;
         case 1152000:
           baud = B1152000;
         break;
         case 1500000:
           baud = B1500000;
         break;
         case 2000000:
           baud = B2000000;
         break;
         case 2500000:
           baud = B2500000;
         break;
         case 3000000:
           baud = B3000000;
         break;
         case 3500000:
           baud = B3500000;
         break;
         default:
           baud = 0;
         break;
      }
      cfsetispeed(&serial_device, baud);
      cfsetospeed(&serial_device, baud);
      tcsetattr(file_descriptor,TCSANOW, &serial_device);
    #endif
    }
#elif defined(_L7_OS_VXWORKS_)
    poeErrno = errnoGet();
    if (-1 != file_descriptor)
    {
      /* Set the baud rate */
      if (-1 == ioctl(file_descriptor, FIOBAUDRATE, parms->uart.baud_rate))
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Could not set baud rate on %s.\r\n", descriptor);
      }
    }
#endif
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
        if(bytes == 0)
        {
          bytes = len;
        }
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
#ifdef PLAT_POE_COMM_UART
  int startTime, currentTime;
  unsigned int msecs;
  int temp_len;
#endif
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
    if(cardData->parms.uart.mode == HPC_POE_COMM_MODE_NONBLOCKING)
    {
			temp_len = 0;
			startTime = osapiTimeMillisecondsGet();
			while(temp_len < len)
			{ 
				 bytes = read(cardData->poe_fd, rx_buf+temp_len, len-temp_len);
				 if(bytes < 0) return -1;
				 temp_len += bytes;
				 currentTime = osapiTimeMillisecondsGet();
				 msecs = currentTime - startTime;
				 if(msecs > HPC_POE_COMM_UART_RX_TIMEOUT)
				 {
					 return -1;
				 }
			}
			bytes = len;
   }
   else
   {
     bytes = read(cardData->poe_fd, rx_buf, len);
   }
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
    if(bytes != 0)
    {
      bytes = -1;
    }
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }

  return bytes;
}

/*******************************************************************************
*
* @purpose To Fulsh data from the physical Device 
*
* @param   L7_uint32              CardIndex     PoE Card Index
*          L7_uint32              flag          0 -- IN , 1 -- OUT , 2 -- BOTH 
*
* @returns Number of the bytes written
*          -1, if command fails
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_int32 poe_device_flush(L7_uint32 cardIndex)
{
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
#if defined(_L7_OS_VXWORKS_)
#else
  L7_uchar8 buf=0;
  int read_byte=0;
  int startTime, currentTime;
  unsigned int msecs = 0;
#endif
  if (L7_NULLPTR == cardData)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card Index %u", cardIndex);
    return -1;
  }

  if (cardData->type == HPC_POE_COMM_TYPE_UART)
  {
#if defined(_L7_OS_VXWORKS_)

      if (-1 == ioctl(cardData->poe_fd,FIOFLUSH,0))
      {
        return -1;
      }
#else
    if(cardData->parms.uart.mode == HPC_POE_COMM_MODE_NONBLOCKING)
    {
			startTime = osapiTimeMillisecondsGet();
			while(msecs < HPC_POE_COMM_UART_FLUSH_TIMEOUT)
			{ 
				 read_byte = read(cardData->poe_fd, &buf, 1);
				 if(read_byte < -1) return -1;
				 else if (read_byte > 0)
				 {
					 read_byte = 0;
					 startTime = osapiTimeMillisecondsGet();
				 }
				 currentTime = osapiTimeMillisecondsGet();
				 msecs = currentTime - startTime;
			}
    }
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_I2C)
  {
#ifdef PLAT_POE_COMM_I2C
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }
  else if (cardData->type == HPC_POE_COMM_TYPE_BSC)
  {
#ifdef PLAT_POE_COMM_BSC
#else
    printf("I2C Communication Mechanism is not supported\n");
#endif
  }

  return 0;
}


