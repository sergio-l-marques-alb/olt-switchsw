/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_poe.c
*
* @purpose   This file contains the POE code. It uses POE driver API's
*            to achieve the desired POE functionality.
*
* @component hapi
*
* @comments
*
* @create    09 Feb 2007
*
* @author    drajendra / msiva
*
* @end
*
**********************************************************************/

/*********************************************************************
 *
 * The code in this file is in this order:
 *
 * 1) include files
 * 2) #define macro declarations
 * 3) typedefs
 * 4) global variable declarations
 * 5) functions
 *
 * The functions are grouped as follows:
 *
 * a) utility
 * b) controller message handling
 * c) sysapi message handling
 * d) task, initialization and firmware download
 * e) the public function to receive sysapi messages
 * f) debug
 *
 *********************************************************************/
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "hpc_db.h"
#include "unitmgr_api.h"

#include "bcm/bcmi2c.h"
#include "soc/i2c.h"

#include "poe_exports.h"
#include "hpc_poe_brcm.h"
#include "hpc_poe.h"
#include "hpc_poe_platform.h"

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
static L7_BOOL   hpcPoeInitComplete   = L7_FALSE;
static L7_BOOL   hpcPoeMonitorEnabled = L7_TRUE;
static L7_BOOL   hpcPoeAutoResetMode  = L7_ENABLE;

/****************      Semaphores    ******************************************/
static void      *poe_access_sem           = L7_NULL;
static void      *poe_tx_sem               = L7_NULL;

static L7_BOOL    poeInitialized           = L7_FALSE;
static L7_uint32  poeMonitorTaskId         = 0;
static L7_char8   versionString[80];    /* Firmware version */

typedef struct
{
  /* from Get Port Measurements */
  L7_ushort16                        powerConsumed;       /* unit is in milliWatts */
  L7_ushort16                        voltage;             /* unit is in Volts */
  L7_ushort16                        current;             /* unit is in milliAmps */
  L7_ushort16                          temperature;         /* Celsius */
  /* from Get Single Port Status */
  L7_uchar8                        enabled;
  L7_uchar8                        status;
  L7_uchar8                        latch;
  L7_POE_PORT_POWER_CLASS_t        powerClass;
  L7_uchar8                        errorCode;
  /* indirectly from Get Single Port Status */
  L7_POE_PORT_DETECTION_STATUS_t   detectionStatus;
  /* from Get Port Power Limit */
  L7_uchar8                        powerLimit;
  L7_uchar8                        powerLimitType;      /* Power Limit Violation Type */
  L7_uchar8                        powerLimitTemp;
  /* from Get Port Priority */
  L7_uchar8                        priority;
} HPC_POE_PORT_STATUS_t;

typedef struct
{
  L7_uint32 mpsAbsent;
  L7_uint32 invalidSignature;
  L7_uint32 powerDenied;
  L7_uint32 overLoad;
  L7_uint32 shortCounter;
} HPC_POE_PORT_STATISTICS_t;

typedef struct
{
  L7_uchar8   present;
  /* from Get PoE Device Status */
  L7_ushort16 deviceVersion;
  L7_uchar8   asicStatus;
  L7_uchar8   portsExpected;
  L7_uchar8   portsVerified;
  L7_uchar8   portsAllocated;
  L7_uchar8   tempSwitchHigh;
  L7_uchar8   midPowerData;
  L7_uchar8   commStatus;
} HPC_POE_DEVICE_STATUS_t;

typedef struct
{
  L7_BOOL                 initialized;
  HPC_POE_DEVICE_STATUS_t deviceStatus[L7_MAX_POE_DEVICES_PER_CARD];
  /* from Get System Status */
  L7_uchar8               cpuStatus1;
  L7_uchar8               cpuStatus2;
  L7_uchar8               factoryDefault;
  L7_uchar8               generalInternalError;
  L7_uchar8               privateLabel;
  L7_uchar8               userByte;
  L7_uchar8               deviceFail;
  L7_uchar8               tempDisconnect;
  L7_uchar8               tempAlarm;
  /* from Get Power Supply Parameters */
  L7_ushort16             powerConsumption; /* Watts */
  L7_ushort16             maxShutdownVoltage;
  L7_ushort16             minShutdownVoltage;
  L7_uchar8               activePowerBank;
  L7_ushort16             maxPowerAvailable;
  /* from Get Power Supply Voltage */
  L7_ushort16             powerSupplyVoltage; /* deciVolts */
} HPC_POE_CARD_STATUS_t;

typedef struct
{
  HPC_POE_PORT_STATUS_t     poePortStatus[L7_POE_MAX_CHANNELS + 1];
  HPC_POE_PORT_STATISTICS_t poePortStatistics[L7_POE_MAX_CHANNELS + 1];
  HPC_POE_CARD_STATUS_t     cardStatus[L7_MAX_POE_CARDS_PER_UNIT];
  L7_uint32                 usageThreshold;
  L7_POE_POWER_MGMT_t       powerMgmtMode;
} HPC_POE_GLOBAL_STATUS_t;

static HPC_POE_GLOBAL_STATUS_t poeGlobalStatus_g;

/* debug stuff */
static L7_uint32  brcmPoeDebugEnabled          = 1;
static L7_uint32  brcmPoeMonitorDebugEnabled   = 0;
static L7_uint32  brcmPoeFirmwareDebugEnabled  = 0;

/*
 * This is an array of pointers to message handling functions.
 */
static L7_RC_t (*poeMsgHandlers[SYSAPI_HPC_POE_MSG_ID_MAX])(SYSAPI_POE_MSG_t *msg);

static unsigned char poeTxMsg[BSC_MSG_LEN];
static unsigned char poeRxMsg[BSC_MSG_LEN];

static int           poeTxDebug = FALSE;
static int           poeRxDebug = FALSE;

#ifdef POWER_MANAGEMENT
/* Maximum power that can be taken by different class PD's */
static L7_uint32 powerAllocForClass[8] = { 154, 40, 70, 154, 154, 154, 154, 154 };
#endif

/*********************************************************************
 *
 * @purpose Grants access to the POE controller
 *
 * @param   void
 *
 * @returns L7_RC_t result
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoeGetAccess(L7_int32 timeout)
{
  L7_RC_t rc;

  /* might be being used elsewhere */
  rc  = osapiSemaTake(poe_access_sem, timeout);

  if (brcmPoeMonitorDebugEnabled)
  {
    if (rc == L7_SUCCESS)
    {
      POE_HPC_DPRINTF("$");
    }
    else
    {
      POE_HPC_DPRINTF("#");
    }
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Releases access to the POE controller
 *
 * @param   void
 *
 * @returns L7_RC_t result
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoeReleaseAccess(void)
{
  if (brcmPoeMonitorDebugEnabled)
  {
    POE_HPC_DPRINTF("*");
  }

  return osapiSemaGive(poe_access_sem);
}

/*******************************************************************************
**                             Debug Routines                                 **
*******************************************************************************/

/*******************************************************************************
*
* @purpose To enable or disable Tx/Rx debug selectively
*
* @param   txEnable        @b{(input)}  Enable or disable Tx debug
* @param   rxEnable        @b{(input)}  Enable or disable Rx debug
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmDebugPoePkt(int txEnable, int rxEnable)
{
  poeTxDebug = txEnable;
  poeRxDebug = rxEnable;
}

/*******************************************************************************
*
* @purpose Helper function to print the given packet in a nice format
*
* @param   pkt             @b{(input)}  packet that needs to be printed
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmDebugPoePktDump(L7_uchar8* pkt)
{
  int column;

  for (column = 0; column < BSC_MSG_LEN; column++)
  {
    printf("%2.2x ", pkt[column]);
  }
  printf("\n");

}

static unsigned char poeDebugTxMsg[BSC_MSG_LEN];
static unsigned char poeDebugRxMsg[BSC_MSG_LEN];

/*******************************************************************************
*
* @purpose Helper function to fill the Tx buffer
*
* @param   uch1-uch12      @b{(input)}  12 bytes of the packet
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmDebugMsgSet(
    unsigned char uch1, unsigned char uch2, unsigned char uch3, unsigned char uch4,
    unsigned char uch5, unsigned char uch6, unsigned char uch7, unsigned char uch8,
    unsigned char uch9, unsigned char uch10, unsigned char uch11, unsigned char uch12
    )
{
  memset(poeDebugTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  poeDebugTxMsg[0] = uch1; poeDebugTxMsg[1] = uch2; poeDebugTxMsg[2] = uch3; poeDebugTxMsg[3] = uch4;
  poeDebugTxMsg[4] = uch5; poeDebugTxMsg[5] = uch6; poeDebugTxMsg[6] = uch7; poeDebugTxMsg[7] = uch8;
  poeDebugTxMsg[8] = uch9; poeDebugTxMsg[9] = uch10; poeDebugTxMsg[10] = uch11; poeDebugTxMsg[11] = uch12;

}

/*******************************************************************************
*
* @purpose Debug routine to test Tx and Rx of the I2C driver
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   addr            @b{(input)}  Handle to the I2C driver
*
* @returns POE_E_NONE, on successful sending the data
*          POE_E_SEND, otherwise
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmDebugI2CDriver(int unit, char addr)
{
  int rv;

  osapiSemaTake(poe_tx_sem, L7_WAIT_FOREVER);

  memset(poeDebugRxMsg, 0, BSC_MSG_LEN);

  printf("\n===================\n");
  printf("%s ", "RQST: ");
  hpcBrcmDebugPoePktDump(poeDebugTxMsg);

  rv = soc_i2c_write_test(unit, addr, poeDebugTxMsg, BSC_MSG_LEN);
  printf("%s:%d - write rv: %d\n", __func__, __LINE__, rv);

  osapiSleepMSec(POE_WAIT_TIME_TO_READ_RESPONSE); /* delay before reading for response */

  rv = soc_i2c_read_test(unit, addr, poeDebugRxMsg, BSC_MSG_LEN);
  printf("%s:%d - read  rv: %d\n", __func__, __LINE__, rv);

  printf("%s ", "RESP: ");
  hpcBrcmDebugPoePktDump(poeDebugRxMsg);
  printf("===================\n");

  osapiSemaGive(poe_tx_sem);
}

/* APIs interracting with BCM5910x */
/* PSE Power Bank configuration set */
/*******************************************************************************
**              API's interacting with the BCM5910x                           **
*******************************************************************************/

/*******************************************************************************
*
* @purpose To calculate the checksum for the given data and update the data with
*          the calculated checksum
*
* @param   buf             @b{(output)} buffer containing the data
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hpcBrcmPoeChecksumInsert(unsigned char *buf)
{
  register unsigned char checksum = 0;
  int   i;
  /* Last byte carries the check sum */
  for (i = 0; i < BSC_MSG_LEN - 1; i++)
  {
    checksum += buf[i];
  }

  buf[POE_CHKSM_FIELD] = checksum;
  return;
}

/*******************************************************************************
*
* @purpose To verify the checksum of the given data
*
* @param   buf             @b{(output)} buffer containing the data to be 
*                                       verified for checksum
*
* @returns POE_E_NONE, if checksum is valid
*          POE_E_CHECKSUM, otherwise
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeChecksumVerify(unsigned char *buf)
{
  int   i;
  register unsigned char  checksum = 0;

  for (i = 0; i < BSC_MSG_LEN - 1; i++)
  {
    checksum += buf[i];
  }

  if (buf[i] != checksum)
  {
    return POE_E_CHECKSUM;
  }
  return POE_E_NONE;
}

/*******************************************************************************
*
* @purpose To send data to the PoE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   txBuf           @b{(input)}  buffer that contains Tx data
*
* @returns POE_E_NONE, on successful sending the data
*          POE_E_SEND, otherwise
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeSendMsg(L7_uint32 cardIndex, unsigned char *txBuf)
{
  unsigned int retry_count = 0;
  int rv;

  /* Insert checksum into the message */
  hpcBrcmPoeChecksumInsert(txBuf);

  /* Write poe control message to the i2c bus */
  do
  {
    if( (rv = poe_device_write(cardIndex, txBuf, BSC_MSG_LEN)) == 0)
    {
      break;
    }
    retry_count++;
  } while(retry_count < POE_MSG_TX_RETRY_COUNT);

  if(retry_count == POE_MSG_TX_RETRY_COUNT)
  {
    return POE_E_SEND;
  }
  return POE_E_NONE;

}

/*******************************************************************************
*
* @purpose To receive data from the PoE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   rxBuf           @b{(output)} buffer to hold the Rx data
*
* @returns POE_E_NONE, if received successfully
*          POE_E_RECEIVE, unable to receive the message
*          POE_E_CHECKSUM, incorrect checksum in the received data
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeReceiveMsg(L7_uint32 cardIndex, unsigned char *rxBuf)
{
  unsigned int  nbytes = BSC_MSG_LEN;

  if( (poe_device_read(cardIndex, rxBuf, nbytes)) < 0)
  {
    return POE_E_RECEIVE; /* I2C Bus Error */
  }

  /* Verify the checksum */
  if (hpcBrcmPoeChecksumVerify(rxBuf) != POE_E_NONE)
  {
    return POE_E_CHECKSUM; /* Checksum verification failed */
  }
  return POE_E_NONE;
}

/*******************************************************************************
*
* @purpose To communicate with the POE subsystem. It does it in this way
*          1. Sends data
*          2. Wait for some time
*          3. Receive data
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   txBuf           @b{(input)}  buffer that contains Tx data
* @param   rxBuf           @b{(output)} buffer to hold the Rx data
*
* @returns POE_E_NONE, on successful transmit and receive
*          POE_E_SEND, failed in sending the message
*          POE_E_CODE, firmware is bad in the PoE subsystem
*          POE_E_MAX_RETRIES, unable to communicate in the maximum retries also
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeTransReceiveMsg(L7_uint32 cardIndex, 
                           unsigned char *txBuf, unsigned char *rxBuf)
{
  unsigned char retry_count = 0;
  L7_BOOL   skip_send = FALSE;
  int  rc;

  if (L7_FALSE == hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_FAILURE;
  }

  if (poe_tx_sem == L7_NULL)
  {
    return L7_FAILURE;
  }

  if (L7_SUCCESS != osapiSemaTake(poe_tx_sem, L7_WAIT_FOREVER))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not take Poe TX semaphore", __FUNCTION__);
  }

  while(retry_count < POE_DRVR_COMMAND_MAX_RETRIES)
  {
    if(skip_send == FALSE)
    {
      if(poeTxDebug)
      {
        printf("\n===================\n");
        printf("%s ", "RQST: ");
        hpcBrcmDebugPoePktDump(txBuf);
      }

      if(hpcBrcmPoeSendMsg(cardIndex, txBuf) != POE_E_NONE)
      {
        /* We have already failed to send the message thrice */
        if (L7_SUCCESS != osapiSemaGive(poe_tx_sem))
        {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not give Poe TX semaphore", __FUNCTION__);
        }
        return POE_E_SEND;
      }

      osapiSleepMSec(POE_WAIT_TIME_TO_READ_RESPONSE); /* delay before reading for response */
    }

    skip_send = FALSE;

    rc = hpcBrcmPoeReceiveMsg(cardIndex, rxBuf);

    if(poeRxDebug)
    {
      printf("%s ", "RESP: ");
      hpcBrcmDebugPoePktDump(rxBuf);
      printf("===================\n");
    }

    osapiSleepMSec(POE_WAIT_TIME_BEF_REQUEST); /* delay before issuing request */

    if((rc == POE_E_NONE)
        && (rxBuf[0] != POE_DRVR_ERROR_BAD_CODE)
        && (rxBuf[0] != POE_DRVR_ERROR_RESPONSE_NOT_READY)
        && (rxBuf[0] != POE_DRVR_ERROR_BAD_REQUEST_CHKSM))
    {
      break;
    }

    if(rxBuf[0] == POE_DRVR_ERROR_BAD_CODE)
    {
      printf("Code downloaded onto the PoE chip is bad. Need new Code\n");
      rc = POE_E_CODE;
      break;
    }

    retry_count++;

    if(rxBuf[0] == POE_DRVR_ERROR_RESPONSE_NOT_READY)
    {
      if(poeTxDebug)
        printf("%s - %d: Response is not ready at the PoE Chip side\n", __func__, __LINE__);
      skip_send = TRUE;
    }

  } /* End of while */

  if(retry_count == POE_DRVR_COMMAND_MAX_RETRIES)
  {
    if(poeTxDebug)
      printf("%s - %d: Max retries tried...\n", __func__, __LINE__);

    rc = POE_E_MAX_RETRIES;
  }

  if (L7_SUCCESS != osapiSemaGive(poe_tx_sem))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not give Poe TX semaphore", __FUNCTION__);
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To send the custom data to the PoE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   data            @b{(input)}  array of data to be sent
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgCustomDataSend(L7_uint32 cardIndex, 
                             unsigned char *data)
{
  int    rc;

  memcpy(poeTxMsg, data, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[3] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To set the total power and guard band for a given power bank
*          combination
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   powerBankComb   @b{(input)}  Power bank combination
* @param   totalPower      @b{(input)}  Total power for a given PB combination
* @param   guardBandPower  @b{(input)}  Guard band power for a given PB 
*                                       combination
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPowerBankConfigSet(L7_uint32 cardIndex, 
                       unsigned char powerBankComb, unsigned short totalPower , 
                       unsigned short guardBandPower)
{
  unsigned short ushortVal;
  int            rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_POWER_SOURCE_CONFIG_CMD;
  poeTxMsg[2] = powerBankComb;
  /* Max power allocated */
  ushortVal = osapiHtons(totalPower  * 10);
  memcpy(&poeTxMsg[3], &ushortVal, 2);
  /* Guard Band allowed */
  ushortVal = osapiHtons(guardBandPower * 10);
  memcpy(&poeTxMsg[5], &ushortVal, 2);

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[3] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To set the Power Management mode of the system
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   mode            @b{(input)}  Power management mode
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPowerSourceModeSet(L7_uint32 cardIndex, 
                                 unsigned char mode)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_POWER_MGMT_MODE_CONFIG_CMD;
  poeTxMsg[2] = mode;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To enable/disable logical port map mode
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   mode            @b{(input)}  enable or disable
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgLogicalPortMapModeSet(L7_uint32 cardIndex, 
                                    unsigned char mode)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_LOGICAL_PORT_MAP_ENABLE_CMD;
  poeTxMsg[2] = mode;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To set logical port mapping for a given physical port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  physical port number
* @param   logicalPsePort  @b{(input)}  logical port number
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgLogicalPortMapCfgSet(L7_uint32 cardIndex, 
                            unsigned char psePort, unsigned char logicalPsePort)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_PORT_MAP_CONFIG_CMD;
  poeTxMsg[2] = psePort;
  poeTxMsg[3] = logicalPsePort;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[3] == 0 && poeRxMsg[2] == psePort) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To enable/disable rapid power down mode for a given port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   mode            @b{(input)}  enable or disable
* @param   priority        @b{(input)}  priority level threshold for rapid 
*                                       power down
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgRapidPowerDownModeSet(L7_uint32 cardIndex, 
        unsigned char mode, unsigned char priority)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_RAPID_POWER_DOWN_ENABLE_CMD;
  poeTxMsg[2] = mode;
  poeTxMsg[3] = priority;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To set the PSE functionality on a given port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  PSE mode on a port
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortOnOffSet(L7_uint32 cardIndex, 
                           unsigned char psePort, unsigned char mode)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_PSE_ENABLE_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = mode;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_PSE_ENABLE_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = mode;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = mode;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = mode;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = mode;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure auto mode on a given port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  auto mode
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgPortAutoModeSet(L7_uint32 cardIndex, 
                              unsigned char psePort, unsigned char mode)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_AUTO_POWER_UP_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = mode;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_AUTO_POWER_UP_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = mode;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = mode;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = mode;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = mode;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To allow or disallow Power Up on the specified power requesting port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  power up mode
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgPortManualPowerUpSet(L7_uint32 cardIndex, 
                                   unsigned char psePort, unsigned char mode)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_UP_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = mode;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_UP_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = mode;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = mode;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = mode;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = mode;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To reset the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortReset(L7_uint32 cardIndex, 
                        unsigned char psePort)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_RESET_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = 1;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_RESET_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = 1;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = 1;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = 1;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = 1;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure the detection type on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   type            @b{(input)}  detection type
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortDetectionTypeSet(L7_uint32 cardIndex, 
                                   unsigned char psePort, unsigned char type)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_DETECTION_TYPE_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = type;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_DETECTION_TYPE_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = type;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = type;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = type;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = type;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure the detection type on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   type            @b{(input)}  detection type
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgPortClassificationTypeSet(L7_uint32 cardIndex, 
                                      unsigned char psePort, unsigned char type)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_CLASSIFICATION_TYPE_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = type;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_CLASSIFICATION_TYPE_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = type;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = type;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = type;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = type;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure the disconnect type on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   type            @b{(input)}  disconnect type
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortDisconnectTypeSet(L7_uint32 cardIndex, 
                                    unsigned char psePort, unsigned char type)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_DISCONNECT_TYPE_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = type;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_DISCONNECT_TYPE_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = type;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = type;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = type;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = type;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure the pair used to deliver power on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   pair            @b{(input)}  power pair value
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortPairSet(L7_uint32 cardIndex, 
                          unsigned char psePort, unsigned char pair)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_PAIR_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = pair;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_PAIR_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = pair;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = pair;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = pair;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = pair;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure the Power Up Mode on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  power up mode
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortPowerUpModeSet(L7_uint32 cardIndex, 
                                 unsigned char psePort, unsigned char mode)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_UP_MODE_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = mode;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      } 
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_UP_MODE_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = mode;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = mode;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = mode;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = mode;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure Port Priority on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   priority        @b{(input)}  priority value
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortPrioritySet(L7_uint32 cardIndex, 
                              unsigned char psePort, unsigned char priority)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_PRIORITY_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = priority;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_PRIORITY_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = priority;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = priority;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = priority;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = priority;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure Maximum Threshold on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   powerLimit      @b{(input)}  power Limit
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortPowerLimitSet(L7_uint32 cardIndex, 
                              unsigned char psePort, unsigned char powerLimit)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_MAX_POWER_THRESHOLD_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = powerLimit/2;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_MAX_POWER_THRESHOLD_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = powerLimit/2;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = powerLimit/2;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = powerLimit/2;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = powerLimit/2;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To configure Power Threshold Type on the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  power threshold type
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortPowerThresholdTypeSet(L7_uint32 cardIndex, 
                                     unsigned char psePort, unsigned char mode)
{
  int    rc = POE_E_NONE;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  unsigned char channel = 0;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_THRESHOLD_TYPE_CONFIG_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = mode;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          } 
      }
  }
  else
  {
      channel = (cardData->numOfPorts - 1);
      while (channel != 0xFF) 
      {
          memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
          memset(poeRxMsg, 0, BSC_MSG_LEN);
          poeTxMsg[POE_CMD_FIELD] = POE_PORT_POWER_THRESHOLD_TYPE_CONFIG_CMD;
          poeTxMsg[2] = channel--;
          poeTxMsg[3] = mode;
          poeTxMsg[4] = channel--;
          poeTxMsg[5] = mode;
          poeTxMsg[6] = channel--;
          poeTxMsg[7] = mode;
          poeTxMsg[8] = channel--;
          poeTxMsg[9] = mode;

          rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

          if (L7_POE_OK(rc) == TRUE)
          {
              if((poeRxMsg[3] == 0) && (poeRxMsg[5] == 0) && (poeRxMsg[7] == 0) && (poeRxMsg[9] == 0)) /* 0 - Ack */
              {
                  rc = POE_E_NONE;
              }
              else
              {
                  return POE_E_ACK;
              }
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the system status of POE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   modePins        @b{(output)} mode pins status
* @param   poePorts        @b{(output)} maximum ports in the PoE subsystem
* @param   portMap         @b{(output)} logical port map status
* @param   hwVer           @b{(output)} device Id
* @param   swVer           @b{(output)} software version
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgSystemStatusGet(L7_uint32 cardIndex, 
    unsigned char *modePins, unsigned char *poePorts, unsigned char *portMap, 
        unsigned short *hwVer, unsigned char *swVer, unsigned char *eepromStatus,
        unsigned char *configStatus)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset (poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_SYSTEM_STATUS_QUERY_CMD;
  poeTxMsg[1] = 0xFF;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
  if (L7_POE_OK(rc) == TRUE)
  {
    *modePins = poeRxMsg[2];
    *poePorts = poeRxMsg[3];
    *portMap  = poeRxMsg[4];
    memcpy(hwVer, &poeRxMsg[5], 2);
    *hwVer = osapiNtohs(*hwVer);
    *swVer   = poeRxMsg[7];
    *eepromStatus = poeRxMsg[8];
    *configStatus = poeRxMsg[9];
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the total Power available in the PoE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   powerAllocated  @b{(output)} power allocated in the PoE subsystem
* @param   powerAvailable  @b{(output)} power available in the PoE subsystem
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPowerSourceTotalPowerGet(L7_uint32 cardIndex,
                                       unsigned short *powerAllocated,
                                       unsigned short *powerAvailable,
                                       unsigned char  *mpsmStatus)
{
  int            rc;
  unsigned short ushortVal;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_TOTAL_POWER_ALLOC_QUERY_CMD;
  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    memcpy(&ushortVal, &poeRxMsg[2], 2);
    ushortVal = osapiNtohs(ushortVal);
    *powerAllocated = ushortVal;

    memcpy(&ushortVal, &poeRxMsg[4], 2);
    ushortVal = osapiNtohs(ushortVal);
    *powerAvailable = ushortVal;

    *mpsmStatus = poeRxMsg[6];      /* Multiple Power Source Status*/
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Status of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   pdStatus        @b{(output)} status of the connected PD
* @param   classInfo       @b{(output)} class information
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortStatusGet(L7_uint32 cardIndex, 
                            unsigned char psePort, unsigned char *pdStatus,
                            unsigned char *classInfo, unsigned char *detectInfo,
                            unsigned char *remotePd, unsigned char *mpssMask)
{
  int    rc = POE_E_NONE;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_STATUS_QUERY_CMD;
      poeTxMsg[2] = psePort;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          *pdStatus  = poeRxMsg[3]; /* Power Device detection status */
          *classInfo = poeRxMsg[4]; /* dot3af classification info */
          *detectInfo = poeRxMsg[5]; /* Pd Detection Status */
          *remotePd = poeRxMsg[6];  /* remote Power Device Type */
          *mpssMask = poeRxMsg[7];  /* Port Mask for MPSS */
          rc = POE_E_NONE;
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Status of Multiple ports (Should be multiple of 4 always)
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  start port number referred in the POE system
* @param   numPorts        @b{(input)}  number of Ports for which Status needs to be Polled.
* @param   pdStatus        @b{(output)} status of the connected PD
* @param   classInfo       @b{(output)} class information
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgMultiplePortStatusGet(L7_uint32 cardIndex, 
                            unsigned char psePort, unsigned char *pdStatus, unsigned char *classInfo)
{
  int    rc;

    memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
    memset(poeRxMsg, 0, BSC_MSG_LEN);
    poeTxMsg[POE_CMD_FIELD] = POE_MULTIPLE_PORT_STATUS_QUERY_CMD;
    poeTxMsg[2] = psePort;
    poeTxMsg[3] = 1;
    poeTxMsg[4] = psePort+1;
    poeTxMsg[5] = 1;
    poeTxMsg[6] = psePort+2;
    poeTxMsg[7] = 1;
    poeTxMsg[8] = psePort+3;
    poeTxMsg[9] = 1;
    rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
    if (L7_POE_OK(rc) == TRUE)
    {
          pdStatus[0]  = poeRxMsg[3] & 0x0F; /* Power Device detection status */
          classInfo[0] = (poeRxMsg[3] & (0x70))>>4; /* dot3af classification info */

          pdStatus[1] =  poeRxMsg[5] & 0x0F;
          classInfo[1] = (poeRxMsg[5] & (0x70))>>4; /* dot3af classification info */

          pdStatus[2] =  poeRxMsg[7] & 0x0F;
          classInfo[2] = (poeRxMsg[7] & (0x70))>>4; /* dot3af classification info */

          pdStatus[3] =  poeRxMsg[9] & 0x0F;
          classInfo[3] = (poeRxMsg[9] & (0x70))>>4; /* dot3af classification info */
          rc = POE_E_NONE;
    }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Measurements of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   portVoltage     @b{(output)} voltage
* @param   portCurr        @b{(output)} current
* @param   portPower       @b{(output)} Power
* @param   portTemp        @b{(output)} Temperature
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortMeasurementGet(L7_uint32 cardIndex, 
                           unsigned char psePort, unsigned short *portVoltage,
                           unsigned short *portCurr, unsigned short *portPower,
                           unsigned short *portTemp)
{
  unsigned short ushortVal;
  int            rc = POE_E_NONE;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_MEASUREMENT_QUERY_CMD;
      poeTxMsg[2] = psePort;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          memcpy(&ushortVal, &poeRxMsg[3], 2);  /* Voltage */
          ushortVal = osapiNtohs(ushortVal);
          *portVoltage = ushortVal;

          memcpy(&ushortVal, &poeRxMsg[5], 2);  /* Current */
          ushortVal = osapiNtohs(ushortVal);
          *portCurr = ushortVal;

          memcpy(&ushortVal, &poeRxMsg[7], 2);  /* Temperature */
          ushortVal = osapiNtohs(ushortVal);
          *portTemp = ushortVal;

          memcpy(&ushortVal, &poeRxMsg[9], 2);  /* Power */
          ushortVal = osapiNtohs(ushortVal);
          *portPower = ushortVal;
          rc = POE_E_NONE;
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Statistics of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mpsAbsentCntr   @b{(output)} MPS absent counter
* @param   overloadCntr    @b{(output)} Overload counter
* @param   shortCntr       @b{(output)} Short counter
* @param   powerDeniedCntr @b{(output)} Power Denied counter
* @param   invalidSignCntr @b{(output)} Invalid signature counter
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortStatisticsGet(L7_uint32 cardIndex, 
    unsigned char psePort, unsigned char *mpsAbsentCntr,
    unsigned char *overloadCntr, unsigned char *shortCntr,
    unsigned char *powerDeniedCntr,
    unsigned char *invalidSignCntr)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_PORT_STATS_QUERY_CMD;
  poeTxMsg[2] = psePort;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    *mpsAbsentCntr     = poeRxMsg[3];   /* MPS Absent */
    *overloadCntr      = poeRxMsg[4];   /* Overload Counter */
    *shortCntr         = poeRxMsg[5];   /* Short Counter */
    *powerDeniedCntr   = poeRxMsg[6];   /* Power Denied Counter */
    *invalidSignCntr   = poeRxMsg[7];   /* Invalid Signature */
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To reset the Statistics of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortStatisticsReset(L7_uint32 cardIndex, 
                                  unsigned char psePort)
{
  int    rc;

  if (psePort != POE_CHANNEL_ALL) 
  {
      memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
      memset(poeRxMsg, 0, BSC_MSG_LEN);
      poeTxMsg[POE_CMD_FIELD] = POE_PORT_STATS_RESET_CMD;
      poeTxMsg[2] = psePort;
      poeTxMsg[3] = 1;

      rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

      if (L7_POE_OK(rc) == TRUE)
      {
          if(poeRxMsg[3] == 0) /* 0 - Ack */
          {
              rc = POE_E_NONE;
          }
      }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Statistics of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   pseMode         @b{(output)} PSE functionality of the port
* @param   autoMode        @b{(output)} auto mode
* @param   detectionType   @b{(output)} detection type configuration
* @param   classifyType    @b{(output)} classification type configuration
* @param   disconType      @b{(output)} disconnect type configuration
* @param   pairConfig      @b{(output)} power pair configuration
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortConfigGet(L7_uint32 cardIndex, unsigned char psePort, unsigned char *pseMode,
    unsigned char *autoMode, unsigned char *detectionType,
    unsigned char *classifyType, unsigned char *disconType,
    unsigned char *pairConfig)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_PORT_CONFIG_QUERY_CMD;
  poeTxMsg[2] = psePort;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    *pseMode        = poeRxMsg[3];   /* PSE Mode */
    *autoMode       = poeRxMsg[4];   /* Auto mode */
    *detectionType  = poeRxMsg[5];   /* Detection Type */
    *classifyType   = poeRxMsg[6];   /* Classification Type */
    *disconType     = poeRxMsg[7];   /* Disconnect Type */
    *pairConfig     = poeRxMsg[8];   /* Power Pair Config */
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Port Statistics of the specified port
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   powerMode       @b{(output)} power up mode
* @param   violationType   @b{(output)} power limit violation type
* @param   maxPower        @b{(output)} user defined maximum power threshold
* @param   priority        @b{(output)} port priority configured
* @param   physicalPort    @b{(output)} Physical port in the POE subsytem mapped
*                                       to this logical port
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPortExtendedConfigGet(L7_uint32 cardIndex, 
    unsigned char psePort, unsigned char *powerMode,
    unsigned char *violationType, unsigned char *maxPower,
    unsigned char *priority, unsigned char *physicalPort)
{
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_PORT_EXTEN_CONFIG_QUERY_CMD;
  poeTxMsg[2] = psePort;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    *powerMode      = poeRxMsg[3];   /* High Power Mode */
    *violationType  = poeRxMsg[4];   /* violation type */
    *maxPower       = poeRxMsg[5];   /* Max Allowed Power */
    *priority       = poeRxMsg[6];   /* Port Priority */
    *physicalPort  = poeRxMsg[7];   /* Physically Mapped  Port */
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To get the Power Management Configuration of the PoE subsystem
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   powerMode       @b{(output)} power management mode
* @param   maxPower        @b{(output)} total power
* @param   guardBand       @b{(output)} guard power band
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgPowerSourceConfigGet(L7_uint32 cardIndex, unsigned char power_bank,
    unsigned char *powerMode, unsigned short *maxPower, 
    unsigned short *guardBand)
{
  unsigned short ushortVal;
  int            rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_POWER_MGMT_CONFIG_QUERY_CMD;
  poeTxMsg[1] = power_bank;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    *powerMode     = poeRxMsg[2];   /* Power Management Mode */

     memcpy(&ushortVal, &poeRxMsg[3], 2);  /* Max Configured Power */
     ushortVal = osapiNtohs(ushortVal);
    *maxPower = ushortVal;

     memcpy(&ushortVal, &poeRxMsg[5], 2);  /* Guard Band */
     ushortVal = osapiNtohs(ushortVal);
    *guardBand = ushortVal;
    rc = POE_E_NONE;
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To Clear EEPROM configuration in Poe Controller
*
* @param   cardIndex            @b{(input)}  PoE Card Index
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgClearEepromConfig(L7_uint32 cardIndex)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_EEPROM_CONFIG_CMD;
  poeTxMsg[POE_SUB_CMD_FIELD] = POE_EEPROM_CLEAR_CONFIG_SUB_CMD;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To Clear EEPROM Application image in Poe Controller
*
* @param   cardIndex            @b{(input)}  PoE Card Index
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgClearEepromApplicationImage(L7_uint32 cardIndex)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_EEPROM_CONFIG_CMD;
  poeTxMsg[POE_SUB_CMD_FIELD] = POE_EEPROM_CLEAR_APPLICATION_SUB_CMD;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To Save EEPROM Configuration on PoE controller
*
* @param   cardIndex            @b{(input)}  PoE Card Index
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgSaveEepromConfig(L7_uint32 cardIndex)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_EEPROM_CONFIG_CMD;
  poeTxMsg[POE_SUB_CMD_FIELD] = POE_EEPROM_SAVE_CONFIG_SUB_CMD;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);

  if (L7_POE_OK(rc) == TRUE) 
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE; 
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose To Save EEPROM Application Image on PoE controller
*
* @param   cardIndex            @b{(input)}  PoE Card Index
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
static int hpcBrcmPoeMsgSaveEepromApplicationImage(L7_uint32 cardIndex)
{
  int rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);

  poeTxMsg[POE_CMD_FIELD] = POE_EEPROM_CONFIG_CMD;
  poeTxMsg[POE_SUB_CMD_FIELD] = POE_EEPROM_SAVE_APPLICATION_SUB_CMD;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
 
  if (L7_POE_OK(rc) == TRUE) 
  {
    if(poeRxMsg[2] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
}

/*******************************************************************************
*
* @purpose Switch PoE port from 803af to highpower mode(803at) 
*
* @param   unit            @b{(input)}  Broadcom Unit number
* @param   fd              @b{(input)}  Handle to the I2C driver
* @param   psePort         @b{(input)}  port number referred in the POE system
* @param   mode            @b{(input)}  power up mode
*
* @returns POE_E_NONE, if successfull
*
* @notes   none
*
* @end
*
*******************************************************************************/
int hpcBrcmPoeMsgPortSwitchTo803AT(L7_uint32 cardIndex, 
                                 unsigned char psePort, unsigned char mode)
{
#if 0
  int    rc;

  memset(poeTxMsg, POE_NA_FIELD_VALUE, BSC_MSG_LEN);
  memset(poeRxMsg, 0, BSC_MSG_LEN);
  poeTxMsg[POE_CMD_FIELD] = POE_PORT_SWITCH_HIGH_POWER;
  poeTxMsg[2] = psePort;
  poeTxMsg[3] = mode;

  rc = hpcBrcmPoeTransReceiveMsg(cardIndex, poeTxMsg, poeRxMsg);
/* debug purpose remove before actual implementation */
  printf("\nInterface:%u\t mode:%d\n,\tFunction:%s\tLine:%d",psePort, mode, __FUNCTION__,__LINE__);
  rc = POE_E_NONE 
  poeRxMsg[3] = 0;
/*end-debug*/
  if (L7_POE_OK(rc) == TRUE)
  {
    if(poeRxMsg[3] == 0) /* 0 - Ack */
    {
      rc = POE_E_NONE;
    }
  }

  return rc;
#endif
  return POE_E_NONE;
}

/*******************************************************************************
**                             POE LED Support                                **
*******************************************************************************/

/*******************************************************************************
* @purpose Initialize POE LED 
*
* @param   port         @b{(input)}  Port number
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hpcBrcmPoeLedInit(L7_uint32 cardIndex)
{
  hpcPlatformHwPoeLedInit(cardIndex);
  return;
}

/*********************************************************************
 *********************************************************************
 *
 * Beginning of sysapi message handling functions.
 *
 * Messages are in the order defined in sysapi_hpc.h.
 *
 *********************************************************************
 *********************************************************************/

/*********************************************************************
 *
 * @purpose Placeholder for unsupported messages
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcBrcmNotSupported(SYSAPI_POE_MSG_t *msg)
{
  return L7_NOT_SUPPORTED;
}

/*******************************************************************************
*
* @purpose Clears EEPROM Configuration
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hpcBrcmPoeSystemClearEepromConfig(void)
{
  L7_int32  rv;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 cardIndex;
  HPC_POE_CARD_DATA_t       *cardData;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (brcmPoeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    rv = hpcBrcmPoeMsgClearEepromConfig(cardIndex); 
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcBrcmPoeSystemClearEepromConfig failed for card %d", cardIndex);
      rc = L7_FAILURE;
      break;
    }
  }

  hpcBrcmPoeReleaseAccess();
  
  return rc;
}

/*******************************************************************************
*
* @purpose Clears EEPROM Application Image
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hpcBrcmPoeSystemClearEepromApplicationImage(void)
{
  L7_int32  rv;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 cardIndex;
  HPC_POE_CARD_DATA_t       *cardData;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (brcmPoeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    rv = hpcBrcmPoeMsgClearEepromApplicationImage(cardIndex);
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcBrcmPoeSystemClearEepromApplicationImage failed for card %d", cardIndex);
      rc = L7_FAILURE;
      break;
    }
  }

  hpcBrcmPoeReleaseAccess();

  return rc;
}

/*******************************************************************************
*
* @purpose Saves EEPROM Configuration
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hpcBrcmPoeSystemSaveEepromConfig(void)
{
  L7_int32  rv;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 cardIndex;
  HPC_POE_CARD_DATA_t       *cardData;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (brcmPoeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    rv = hpcBrcmPoeMsgSaveEepromConfig(cardIndex);
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcBrcmPoeSystemSaveEepromConfig failed for card %d", cardIndex);
      rc = L7_FAILURE;
      break; 
    }
  }

  hpcBrcmPoeReleaseAccess(); 

  return rc;
}

/*******************************************************************************
*
* @purpose Saves EEPROM Application Image
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hpcBrcmPoeSystemSaveEepromApplicationImage(void)
{ 
  L7_int32  rv;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 cardIndex;
  HPC_POE_CARD_DATA_t       *cardData;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (brcmPoeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  { 
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    rv = hpcBrcmPoeMsgSaveEepromApplicationImage(cardIndex);
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcBrcmPoeSystemSaveEepromApplicationImage failed for card %d", cardIndex);
      rc = L7_FAILURE; 
      break;
    }
  }

  hpcBrcmPoeReleaseAccess(); 

  return rc;
}

/* Beginning of System Level sysapi function calls */
/*******************************************************************************
*
* @purpose Sets the Logical to Physical Port mapping
*
* @param   L7_uint32    cardIndex    - Valid PoE card Index
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeChannelToPhysicalPortMap(L7_uint32 cardIndex)
{
  L7_RC_t     rc = L7_FAILURE;
  L7_int32     rv;
  L7_uint32   i;    
  L7_uchar8   logical_poe_port;
  L7_uchar8   physical_poe_port;
  L7_uint32   start_port;
  L7_uint32   max_port;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex); 

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (brcmPoeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  start_port = cardData->start_phy_port;
  max_port = start_port + cardData->numOfPorts;

  for (i = start_port; i < max_port; i++)
  {
      if ((hpcPoeUnitLogicalPoePortGetFromPhyPort(i, &logical_poe_port) == L7_SUCCESS) &&
          (hpcPoeUnitPoePortGetFromPhyPort(i, &physical_poe_port) == L7_SUCCESS))
      {
          rv = hpcBrcmPoeMsgLogicalPortMapCfgSet(cardIndex, physical_poe_port, logical_poe_port);
          if (L7_POE_OK(rv) != L7_TRUE)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcBrcmPoeMsgLogicalPortMapCfgSet failed for channel %d", i);
              rc = L7_FAILURE;
              break;
          }
      }
  }

  hpcBrcmPoeReleaseAccess();

  return rc;
}

/*******************************************************************************
*
* @purpose Sets the system port auto reset mode
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_AUTO_RESET_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poeSystemAutoResetMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeSystemResetAutoMode(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  hpcPoeAutoResetMode = msg->cmdData.mainAutoResetMode.enable;

  return result;
}

/*******************************************************************************
*
* @purpose Sets the Logical Port Mapping mode
*
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_SYSTEM_POWER_MGMT_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poeSystemPowerMgmtMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeSystemLogicalMapMode(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (msg->cmdData.mainLogicalMapMode.enable)
  {
    mode = 1;
  }
  else
  {
    mode = 0;
  }

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    rv = hpcBrcmPoeMsgLogicalPortMapModeSet(cardIndex, mode);
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      hpcBrcmPoeReleaseAccess();
      result = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power Configuration set failed");
    }
  }

  hpcBrcmPoeReleaseAccess();
  return result;
}

/*********************************************************************
 *
 * @purpose Gets the Firmware Version.
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcBrcmPoeSystemFwVersion(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.mainFwVersion.version = versionString;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Gets the POE status for the system
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_SYSTEM_STATUS
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poeSystemStatus
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeSystemStatus(SYSAPI_POE_MSG_t *msg)
{
  HPC_POE_CARD_DATA_t       *cardData;
  L7_uint32   cardIndex = 0;
  L7_uint32   start_port;
  L7_uint32   max_port;
  L7_uchar8   i = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  /* Get the PSE oper status */
  /* If any port is enabled, return ON. If all ports are disabled, return OFF. */
  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
      cardData = hpcPoeCardDataGet(cardIndex);
      if (L7_NULLPTR == cardData)
      {
          hpcBrcmPoeReleaseAccess();
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
          return L7_SUCCESS;
      }

      start_port = cardData->start_phy_port;
      max_port = start_port + cardData->numOfPorts;

      for (i = start_port; i < max_port; i++)
      {
          if (poeGlobalStatus_g.poePortStatus[i].detectionStatus == L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER)
          {
              msg->cmdData.mainOperationalStatus.status = L7_POE_SYSTEM_OPER_STATUS_ON;
              hpcBrcmPoeReleaseAccess();
              return L7_SUCCESS;
          }
      }
  }

  msg->cmdData.mainOperationalStatus.status = L7_POE_SYSTEM_OPER_STATUS_OFF;
  hpcBrcmPoeReleaseAccess();
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE power consumption for the system
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoeSystemPowerConsumption(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_ushort16           power_consumption;
  L7_int                i;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  power_consumption = 0;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  for (i = 0; i < L7_MAX_POE_CARDS_PER_UNIT; i++)
  {
    power_consumption += poeGlobalStatus_g.cardStatus[i].powerConsumption;
  }
  msg->cmdData.mainPowerConsumption.power = BCM5910X_PORT_POWER_IN_WATTS(power_consumption);

  hpcBrcmPoeReleaseAccess();

  return result;
}

/*******************************************************************************
*
* @purpose Sets the system power management mode
*
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_SYSTEM_POWER_MGMT_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poeSystemPowerMgmtMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeSystemPowerMgmt(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch (msg->cmdData.mainPowerMgmtMode.mode)
  {
     case L7_POE_POWER_MGMT_NONE:
       mode = 0;
       break;
     case L7_POE_POWER_MGMT_STATIC_PRI:
       mode = 1;
       break;
     case L7_POE_POWER_MGMT_DYNAMIC_PRI:
       mode = 2;
       break;
     case L7_POE_POWER_MGMT_STATIC:
       mode = 3;
       break;
     case L7_POE_POWER_MGMT_DYNAMIC:
       mode = 4;
       break;

     default:
       return L7_FAILURE;
       break;
  }

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    rv = hpcBrcmPoeMsgPowerSourceModeSet(cardIndex, mode);
    if (L7_POE_OK(rv) != L7_TRUE)
    {
      hpcBrcmPoeReleaseAccess();
      result = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power Configuration set failed");
    }
  }

  hpcBrcmPoeReleaseAccess();
  return result;
}

/*********************************************************************
 *
 * @purpose Gets the POE nominal power for the system
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoeSystemPowerNominal(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_ushort16           maxPowerAvail;
  L7_uint               i;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  maxPowerAvail = 0;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  for (i = 0; i < L7_MAX_POE_CARDS_PER_UNIT; i++)
  {
    maxPowerAvail += poeGlobalStatus_g.cardStatus[i].maxPowerAvailable;
  }
  msg->cmdData.mainPowerNominal.power = BCM5910X_PORT_POWER_IN_WATTS(maxPowerAvail);

  hpcBrcmPoeReleaseAccess();

  return result;
}

/*******************************************************************************
*
* @purpose Sets the system power usage threshold used by the notification
*          callback.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_SYSTEM_USAGE_THRESHOLD
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poeSystemUsageThreshold
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeSystemUsageThreshold(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_uint32             cardIndex;
  HPC_POE_CARD_DATA_t  *cardData;
  L7_uint32             powerBank = 0;
  L7_uint32             guard_band = 0;
  L7_uint32             num_banks = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (msg->cmdData.mainUsageThreshold.percent > L7_POE_USAGETHRESHOLD_MAX)
  {
    return L7_FAILURE;
  }

  poeGlobalStatus_g.usageThreshold = msg->cmdData.mainUsageThreshold.percent;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      hpcBrcmPoeReleaseAccess();
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_FAILURE;
    }

    /* If this PoE card is not detected, continue to the next card */
    if (cardData->poe_fd < 0) 
    {
        continue;
    }

    if (hpcPoeCardDbPowerBanksNumGet(cardIndex, &num_banks) == L7_FAILURE)
    {
        hpcBrcmPoeReleaseAccess();
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex);
        return L7_FAILURE;
    }

    for (powerBank = 0; powerBank < num_banks; powerBank++)
    {
      guard_band = (cardData->power_bank_map[powerBank].maxPower * (100 - poeGlobalStatus_g.usageThreshold)) / 100;
      if (hpcBrcmPoeMsgPowerBankConfigSet(cardIndex, powerBank, cardData->power_bank_map[powerBank].maxPower, guard_band)
          != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to set POE Power bank %d", powerBank);
          continue;
      }
    }
  }

  hpcBrcmPoeReleaseAccess();
  return result;
}


/* Beginning of Port Level sysapi function calls */

/*******************************************************************************
*
* @purpose Enables/Disables Power on a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_POWER_LIMIT
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortPowerLimit
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortAdminEnable(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             enable;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (msg->cmdData.portAdminEnable.adminState)
  {
    enable = PORT_POE_ON;
  }
  else
  {
    enable = PORT_POE_OFF;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortOnOffSet(cardIndex, POE_CHANNEL_ALL, enable);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set Enable/Disable Mode for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortOnOffSet(cardIndex, poe_channel, enable);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set Enable/Disable Mode for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Gets POE statistics for a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_STATISTICS
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortStatistics
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   BCM5910x supports port statistics. For now this driver
*          does not use it.
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortStatistics(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8 port;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  port = msg->usp.port;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  msg->cmdData.portCounters.mpsAbsentCounter        = poeGlobalStatus_g.poePortStatistics[port].mpsAbsent;
  msg->cmdData.portCounters.invalidSignatureCounter = poeGlobalStatus_g.poePortStatistics[port].invalidSignature;
  msg->cmdData.portCounters.powerDeniedCounter      = poeGlobalStatus_g.poePortStatistics[port].powerDenied;
  msg->cmdData.portCounters.overLoadCounter         = poeGlobalStatus_g.poePortStatistics[port].overLoad;
  msg->cmdData.portCounters.shortCounter            = poeGlobalStatus_g.poePortStatistics[port].shortCounter;

  /* All done */
  hpcBrcmPoeReleaseAccess();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets POE statistics for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortCounterInvSig(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portCounterInvalidSignature.counter = poeGlobalStatus_g.poePortStatistics[msg->usp.port].invalidSignature;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets POE statistics for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortCounterMpsAbsent(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portCounterMpsAbsent.counter = poeGlobalStatus_g.poePortStatistics[msg->usp.port].mpsAbsent;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets POE statistics for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortCounterOverload(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portCounterOverload.counter = poeGlobalStatus_g.poePortStatistics[msg->usp.port].overLoad;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets POE statistics for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortCounterPowerDenied(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portCounterPowerDenied.counter = poeGlobalStatus_g.poePortStatistics[msg->usp.port].powerDenied;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets POE statistics for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortCounterShort(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portCounterShort.counter = poeGlobalStatus_g.poePortStatistics[msg->usp.port].shortCounter;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Sets the detection mode for a POE port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_DETECTION_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortDetectionMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortDetectionMode(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch (msg->cmdData.portDetectionMode.mode)
  {
    case L7_POE_DETECTION_NONE:
      mode = 0;
      break;
    case L7_POE_DETECTION_LEGACY:
      mode = 1;
      break;
    case L7_POE_DETECTION_4PT_DOT3AF:
      mode = 2;
      break;
    case L7_POE_DETECTION_4PT_DOT3AF_LEG:
      mode = 3;
      break;
    case L7_POE_DETECTION_2PT_DOT3AF:
      mode = 4;
      break;
    case L7_POE_DETECTION_2PT_DOT3AF_LEG:
      mode = 5;
      break;
    default:
      return L7_FAILURE;
      break;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortDetectionTypeSet(cardIndex, POE_CHANNEL_ALL, mode);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set port detection type for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortDetectionTypeSet(cardIndex, poe_channel, mode);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set port detection type for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Sets the Disconnect Type for a POE port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_DETECTION_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortDetectionMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortDisconnectType(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch (msg->cmdData.portDisconnectType.type)
  {
    case L7_POE_POWER_DISC_NONE:
      mode = 0;
      break;
    case L7_POE_POWER_DISC_AC:
      mode = 1;
      break;
    case L7_POE_POWER_DISC_DC:
      mode = 2;
      break;
    default:
      return L7_FAILURE;
      break;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortDisconnectTypeSet(cardIndex, POE_CHANNEL_ALL, mode);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set port detection type for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortDisconnectTypeSet(cardIndex, poe_channel, mode);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set port detection type for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Gets POE power detection status for a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortDetectionStatus(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portDetectionStatus.status = poeGlobalStatus_g.poePortStatus[msg->usp.port].detectionStatus;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE error code of a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortErrorCode(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portErrorCode.code = poeGlobalStatus_g.poePortStatus[msg->usp.port].errorCode;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Sets the high power mode for a POE port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_HIGH_POWER_MODE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortHighPowerMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortHighPowerMode(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch (msg->cmdData.portHighPowerMode.mode)
  {
    case L7_ENABLE:
      mode = 1;
      break;
    case L7_DISABLE:
      mode = 0;
      break;
    default:
      return L7_FAILURE;
      break;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortPowerUpModeSet(cardIndex, POE_CHANNEL_ALL, mode);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set High Power Mode for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortPowerUpModeSet(cardIndex, poe_channel, mode);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set High Power Mode for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Gets the POE status of the port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_STATUS
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortStatus
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortStatus(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8 port;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  port = msg->usp.port;

  if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
  }

  msg->cmdData.portOperationalStatuses.detectionStatus = poeGlobalStatus_g.poePortStatus[port].detectionStatus;
  msg->cmdData.portOperationalStatuses.powerClass      = poeGlobalStatus_g.poePortStatus[port].powerClass;
  msg->cmdData.portOperationalStatuses.powerConsumed   = BCM5910X_PORT_POWER_IN_WATTS(poeGlobalStatus_g.poePortStatus[port].powerConsumed);
  msg->cmdData.portOperationalStatuses.voltage         = BCM5910X_PORT_VOLTAGE(poeGlobalStatus_g.poePortStatus[port].voltage);
  msg->cmdData.portOperationalStatuses.current         = poeGlobalStatus_g.poePortStatus[port].current;
  msg->cmdData.portErrorCode.code                      = poeGlobalStatus_g.poePortStatus[port].errorCode;
  msg->cmdData.portTemperature.temperature             = BCM5910X_PORT_TEMPERATURE(poeGlobalStatus_g.poePortStatus[port].temperature);

  /* All done */
  hpcBrcmPoeReleaseAccess();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE power classification of a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortClassification(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portPowerClassification.class = poeGlobalStatus_g.poePortStatus[msg->usp.port].powerClass;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Sets the power limit for a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_POWER_LIMIT
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortPowerLimit
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortPowerLimit(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             power_limit;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  power_limit = msg->cmdData.portPowerCurLimit.limit;

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortPowerLimitSet(cardIndex, POE_CHANNEL_ALL, power_limit);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set power limit for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortPowerLimitSet(cardIndex, poe_channel, power_limit);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set power limit for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Gets the POE power consumed (mW)
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortPower(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portPowerOutput.output = BCM5910X_PORT_POWER(poeGlobalStatus_g.poePortStatus[msg->usp.port].powerConsumed);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE amperage used by a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortPowerCurrent(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portPowerOutputCurrent.current = BCM5910X_PORT_CURRENT(poeGlobalStatus_g.poePortStatus[msg->usp.port].current);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE voltage used by a port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortVoltage(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portPowerOutputVoltage.voltage = BCM5910X_PORT_VOLTAGE(poeGlobalStatus_g.poePortStatus[msg->usp.port].voltage);

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Sets the power pairs to be used by the port. Also gets the
*          power pairs capabilities of the device.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_POWER_PAIRS
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortPowerPairs
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   BCM 5910x supports port power pairs. For now this driver
*          does not use it.
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortPowerPairs(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             powerPairs;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (msg->cmdData.portPowerPairs.control == L7_POE_PORT_POWER_PAIRS_SIGNAL)
  {
    powerPairs  = 0;
  }
  else
  {
    powerPairs  = 1;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortPairSet(cardIndex, POE_CHANNEL_ALL, powerPairs);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set power pairs for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortPairSet(cardIndex, poe_channel, powerPairs);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set power pairs for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Sets power priority level for a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_POWER_LIMIT
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortPowerLimit
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortPowerPriority(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             priority;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  HPC_POE_CARD_DATA_t       *cardData;
  L7_uint32             start_port, max_port;
  L7_uint32             i;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch ( msg->cmdData.portPowerPriority.priority )
  {
    case L7_POE_PRIORITY_CRITICAL:
      priority = 3;
      break;
    case L7_POE_PRIORITY_HIGH:
      priority = 2;
      break;
    case L7_POE_PRIORITY_MEDIUM:
      priority = 1;
      break;
    case L7_POE_PRIORITY_LOW:
      priority = 0;
      break;
    default:
      return L7_FAILURE;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortPrioritySet(cardIndex, POE_CHANNEL_ALL, priority);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set priority for all POE channels on card 0x%x", cardIndex);
      }
      else
      {
          cardData = hpcPoeCardDataGet(cardIndex);
          if (L7_NULLPTR == cardData)
          {
              hpcBrcmPoeReleaseAccess();
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
              return L7_FAILURE;
          }

          start_port = cardData->start_phy_port;
          max_port = start_port + cardData->numOfPorts;
          for (i = start_port; i < max_port; i++)
          {
              poeGlobalStatus_g.poePortStatus[i].priority = priority;
          }
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortPrioritySet(cardIndex, poe_channel, priority);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set priority for POE Port %d", msg->usp.port);
    }
    else
    {
        poeGlobalStatus_g.poePortStatus[msg->usp.port].priority = priority;
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Resets the port to default configuration
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_POWER_LIMIT
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortPowerLimit
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortReset(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_uint32             cardIndex;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortReset(cardIndex, POE_CHANNEL_ALL);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to reset all POE channels on cardIndex 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortReset(cardIndex, poe_channel);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to reset POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Gets the temperature of a PoE Port
 *
 * @param   SYSAPI_POE_MSG_t *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortTemperature(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portTemperature.temperature = BCM5910X_PORT_TEMPERATURE(poeGlobalStatus_g.poePortStatus[msg->usp.port].temperature);

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Sets the violation type for a POE port 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_POE_PORT_VIOLATION_TYPE
* @param   void       *data   - DAPI_POE_CMD_t.cmdData.poePortViolationType
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoePortViolationType(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8             mode;
  L7_uint32             cardIndex;
  L7_RC_t               result = L7_SUCCESS;
  L7_int32              rv;
  L7_uchar8             poe_channel = 0;

  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  switch (msg->cmdData.portViolationType.type)
  {
    case L7_POE_LIMIT_TYPE_NONE: 
      mode = 0;
      break;
    case L7_POE_LIMIT_TYPE_DOT3AF: 
      mode = 1;
      break;
    case L7_POE_LIMIT_TYPE_USER: 
      mode = 2;
      break;
    default:
      return L7_FAILURE;
      break;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      rv = hpcBrcmPoeMsgPortPowerThresholdTypeSet(cardIndex, POE_CHANNEL_ALL, mode);
      if (L7_POE_OK(rv) != L7_TRUE)
      {
          result = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                  "Failed to set port threshold type for all POE channels on card 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcBrcmPoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(msg->usp.port, &poe_channel)) 
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcBrcmPoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (brcmPoeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPortPowerThresholdTypeSet(cardIndex, poe_channel, mode);

    /* All done */
    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) != L7_TRUE)
    {
        result = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                "Failed to set port threshold type for POE Port %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Provides the initial configuration settings from the application
 *
 * @param   SYSAPI_POE_MSG_t  *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   These parameters cannot be set on PowerDsine:
 *          powerPairs
 *          powerLimitType   (violation type)
 *          detectionMode
 *          highPowerMode
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcBrcmPoePortInit(SYSAPI_POE_MSG_t *msg)
{
  if (hpcPoeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  msg->cmdData.portAdminEnable.adminState = msg->cmdData.portInit.adminState;
  if (hpcBrcmPoePortAdminEnable(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Admin State.");
      return L7_FAILURE;
  }

  msg->cmdData.portPowerPairs.control = msg->cmdData.portInit.powerPairs;
  if (hpcBrcmPoePortPowerPairs(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Power Pairs Control.");
      return L7_FAILURE;
  }

  msg->cmdData.portPowerPriority.priority = msg->cmdData.portInit.powerPriority;
  if (hpcBrcmPoePortPowerPriority(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Power Priority.");
      return L7_FAILURE;
  }

  msg->cmdData.portPowerCurLimit.limit = msg->cmdData.portInit.powerLimit;
  if (hpcBrcmPoePortPowerLimit(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Power Priority Limit.");
      return L7_FAILURE;
  }

  msg->cmdData.portViolationType.type = msg->cmdData.portInit.powerLimitType;
  if (hpcBrcmPoePortViolationType(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Violation Type.");
      return L7_FAILURE;
  }

  msg->cmdData.portDetectionMode.mode = msg->cmdData.portInit.detectionMode;
  if (hpcBrcmPoePortDetectionMode(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Detection Mode.");
      return L7_FAILURE;
  }

  msg->cmdData.portDisconnectType.type = msg->cmdData.portInit.disconnectType;
  if (hpcBrcmPoePortDisconnectType(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Disconnect Type.");
      return L7_FAILURE;
  }

  msg->cmdData.portHighPowerMode.mode = msg->cmdData.portInit.highPowerMode;
  if (hpcBrcmPoePortHighPowerMode(msg) != L7_SUCCESS) 
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set High Power Mode.");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#ifdef POWER_MANAGEMENT
/*******************************************************************************
* @purpose To adjust the given power from low priority port to the high priority
*          port
*
* @param   lowPort      @b{(input)}  Low priority port number
* @param   hiPort       @b{(input)}  High priority port number
* @param   adjustPower  @b{(input)}  Amount of power that needs to be adjusted
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hpcBrcmPoeGlobalPowerSourceUpdate(L7_uint32 lowPort, L7_uint32 hiPort,
                                    L7_uint32 adjustPower)
{
  L7_uint32     cardIndex_LO;
  L7_uint32     cardIndex_HI;
  L7_uchar8     powerBank;
  L7_uint32     num_HI_banks;
  L7_uint32     num_LO_banks;
  L7_ushort16   guard_band = 0;
  HPC_POE_CARD_DATA_t *cardData_HI;
  HPC_POE_CARD_DATA_t *cardData_LO;

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(lowPort, &cardIndex_LO))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", lowPort);
    return;
  }

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(hiPort, &cardIndex_HI))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", hiPort);
    return;
  }

  cardData_HI = hpcPoeCardDataGet(cardIndex_HI);
  cardData_LO = hpcPoeCardDataGet(cardIndex_LO);
  
  if (adjustPower % 10)
  {
    adjustPower /= 10;
    adjustPower++;
  }
  else
  {
    adjustPower /= 10;
  }

  if (hpcPoeCardDbPowerBanksNumGet(cardIndex_HI, &num_HI_banks) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex_HI);
    return;
  }

  if (hpcPoeCardDbPowerBanksNumGet(cardIndex_LO, &num_LO_banks) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex_LO);
    return;
  }

  for (powerBank = 0; powerBank < num_HI_banks; powerBank++)
  {
      cardData_HI->power_bank_map[powerBank].maxPower += adjustPower;
      guard_band = (cardData_HI->power_bank_map[powerBank].maxPower * (100 - poeGlobalStatus_g.usageThreshold)) / 100;
      if (hpcBrcmPoeMsgPowerBankConfigSet(cardIndex_HI, powerBank, cardData_HI->power_bank_map[powerBank].maxPower, guard_band)
          != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to set POE Power bank %d", powerBank);
          continue;
      }
  }

  for (powerBank = 0; powerBank < num_LO_banks; powerBank++)
  {
      cardData_LO->power_bank_map[powerBank].maxPower -= adjustPower;
      guard_band = (cardData_LO->power_bank_map[powerBank].maxPower * (100 - poeGlobalStatus_g.usageThreshold)) / 100;
      if (hpcBrcmPoeMsgPowerBankConfigSet(cardIndex_LO, powerBank, cardData_LO->power_bank_map[powerBank].maxPower, guard_band)
          != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to set POE Power bank %d", powerBank);
          continue;
      }
  }

}
#endif

/*********************************************************************
 *********************************************************************
 *
 * Beginning of task, FW download and init functions.
 *
 *********************************************************************
 *********************************************************************/

/*******************************************************************************
*
* @purpose POE Monitor Task. This task performs the following activities:
*
*          1. Continuosly checks the status of the POE ports
*          2. Updates the LED depending on the POE port status
*          3. Issue trap, if power consumed goes above or below the configured 
*             threshold
*          4. Issue trap, if status of a port changes from non-delivering to 
*             delivering and vice versa
*          5. Collect port statistics
*          6. Global Power Management of the system (if enabled)
*
* @param   numArgs          @b{(input)}  Number of arguments
* @param   dapi_g           @b{(input)}  The driver object
*
* @returns none
*
* @notes   none
*
*******************************************************************************/
static void hpcBrcmPoeMonitorTask(L7_uint32 numArgs)
{
  L7_uchar8       deliveringPower[L7_POE_MAX_CHANNELS + 1];
  L7_RC_t         result;
  L7_uchar8       pd_status, class_info, detect_info, remote_pd, port_mask;
  L7_uint32       i;
  L7_ushort16     port_voltage, port_curr;
  L7_ushort16     port_power, port_temp;
  L7_ushort16     allocPower, availPower;
  L7_uchar8       mpsmStatus;
  L7_BOOL         systemUsageExceeded = L7_FALSE;
  L7_uchar8       mps_absent_ctr = L7_NULL, overload_ctr = L7_NULL, short_ctr = L7_NULL, power_denied_ctr = L7_NULL;
  L7_uchar8       invalid_sign_ctr = L7_NULL;
#ifdef POWER_MANAGEMENT
  L7_uchar8       low_prio_delvr_port, low_prio_delvr_port0, low_prio_delvr_port1;
  L7_uchar8       hi_prio_req_port, hi_prio_req_port0, hi_prio_req_port1;
  L7_uint32       adjustPower;
  L7_uint32       cardIndex_HI, cardIndex_LO;
#endif
  L7_uint32       cardIndex;
  HPC_POE_CARD_DATA_t  *cardData;
  L7_uint32       start_port, max_port;
  L7_uchar8       poe_channel = 0;
  L7_uint32       rv;
  SYSAPI_POE_TRAP_DATA_THRESHOLD_CROSSED_t trapMainThresholdCrossed;
  SYSAPI_POE_TRAP_DATA_PORT_CHANGE_t       trapPortChange;


  memset(deliveringPower, 0, sizeof(deliveringPower));

  while (1)
  {
    osapiSleep(HPC_POE_TASK_MON_SLEEP); 
    if (hpcPoeMonitorEnabled == L7_TRUE)
    {
      for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
      {
          cardData = hpcPoeCardDataGet(cardIndex);

          start_port = cardData->start_phy_port;
          max_port = start_port + cardData->numOfPorts;

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return;
          }

          result = hpcBrcmPoeMsgPowerSourceTotalPowerGet(cardIndex,
                                                &allocPower, &availPower, &mpsmStatus);
          if (L7_POE_OK(result) == L7_TRUE)
          {
              /* Cache values */
              poeGlobalStatus_g.cardStatus[cardIndex].powerConsumption   = allocPower;
              poeGlobalStatus_g.cardStatus[cardIndex].maxPowerAvailable  = availPower;

              if (allocPower > ((availPower * poeGlobalStatus_g.usageThreshold) / 100))
              {
                  /* if we haven't already notified the application, do so now */
                  if ( systemUsageExceeded == L7_FALSE )
                  {
                      trapMainThresholdCrossed.direction  = L7_POE_THRESHOLD_ABOVE;
                      trapMainThresholdCrossed.allocPower = poeGlobalStatus_g.cardStatus[cardIndex].powerConsumption;
                      sysapiHpcPoeTrapSend(cardIndex, SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED, &trapMainThresholdCrossed);
                  }
                  systemUsageExceeded = L7_TRUE;
              }
              else
              {
                  /* if we haven't already notified the application, do so now */
                  if (systemUsageExceeded == L7_TRUE)
                  {
                      trapMainThresholdCrossed.direction  = L7_POE_THRESHOLD_BELOW;
                      trapMainThresholdCrossed.allocPower = poeGlobalStatus_g.cardStatus[cardIndex].powerConsumption;
                      sysapiHpcPoeTrapSend(cardIndex, SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED, &trapMainThresholdCrossed);
                  }
                  systemUsageExceeded = L7_FALSE;
              }
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Main Power Supply Param Get failed");
          }
          hpcBrcmPoeReleaseAccess();

#ifdef POWER_MANAGEMENT
          hi_prio_req_port  = low_prio_delvr_port  = 0xFF;
          hi_prio_req_port0 = low_prio_delvr_port0 = 0xFF;
          hi_prio_req_port1 = low_prio_delvr_port1 = 0xFF;
#endif
          /* check if any port has changed status */
          for (i = start_port; i < max_port; i++)
          {
              if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
              {
                  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
                  return;
              }
              /* Port Status Query Command */
              if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(i, &poe_channel)) 
              {
                  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", i);
              } 

              rv = hpcBrcmPoeMsgPortStatusGet(cardIndex, poe_channel, &pd_status, &class_info,
                                              &detect_info, &remote_pd, &port_mask);

              if (L7_POE_OK(rv) == L7_TRUE)
              {
                  poeGlobalStatus_g.poePortStatus[i].detectionStatus = pd_status;

                  poeGlobalStatus_g.poePortStatus[i].errorCode = L7_POE_ERROR_NONE;
                  if (pd_status == L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER ||
                      pd_status == L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER)
                  {
                      poeGlobalStatus_g.poePortStatus[i].powerClass = class_info;
#ifdef POWER_MANAGEMENT
                      /* Power Management - Data Collection - Start */
                      /* 1. Find low priority delivering port and high priority requesting port on each POE card */
                      if(cardIndex == 0)
                      {
                          if(pd_status == L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER)
                          {
                              if(hi_prio_req_port0 == 0xFF)
                              {
                                  hi_prio_req_port0 = i;
                              }
                              else if(poeGlobalStatus_g.poePortStatus[i].priority > poeGlobalStatus_g.poePortStatus[hi_prio_req_port0].priority)
                              {
                                  hi_prio_req_port0 = i;
                              }
                          }
                          else
                          {
                              if(low_prio_delvr_port0 == 0xFF)
                              {
                                  low_prio_delvr_port0 = i;
                              }
                              else if(poeGlobalStatus_g.poePortStatus[i].priority >= poeGlobalStatus_g.poePortStatus[low_prio_delvr_port0].priority)
                              {
                                  low_prio_delvr_port0 = i;
                              }
                          }
                      }
                      else
                      {
                          if(pd_status == L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER)
                          {
                              if(hi_prio_req_port1 == 0xFF)
                              {
                                  hi_prio_req_port1 = i;
                              }
                              else if(poeGlobalStatus_g.poePortStatus[i].priority > poeGlobalStatus_g.poePortStatus[hi_prio_req_port1].priority)
                              {
                                  hi_prio_req_port1 = i;
                              }
                          }
                          else
                          {
                              if(low_prio_delvr_port1 == 0xFF)
                              {
                                  low_prio_delvr_port1 = i;
                              }
                              else if(poeGlobalStatus_g.poePortStatus[i].priority >= poeGlobalStatus_g.poePortStatus[low_prio_delvr_port1].priority)
                              {
                                  low_prio_delvr_port1 = i;
                              }
                          }
                      }

                      /* Power Management - Data Collection - End */
#endif
                  }
                  else 
                  {
                      poeGlobalStatus_g.poePortStatus[i].powerClass = L7_POE_PORT_POWER_CLASS0;
                      if (pd_status == L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT||
                          pd_status == L7_POE_PORT_DETECTION_STATUS_FAULT)
                      {
                          poeGlobalStatus_g.poePortStatus[i].errorCode = class_info;
                      }
                  }

                  /* Send out port power delivering/not delivering trap */
                  if (pd_status == L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER)
                  {
                      if (deliveringPower[i] == 0)
                      {
                          trapPortChange.state  = L7_POE_PORT_ON;
                          trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
                          sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
                          hpcPlatformHwPoeLedDeliveringPower(i);      /* LED Green --- Default */
                      }
                      deliveringPower[i] = 1;
                  }
                  else 
                  {
                      if (deliveringPower[i] == 1)
                      {
                          trapPortChange.state  = L7_POE_PORT_OFF;
                          trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
                          sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
                      }
                      deliveringPower[i] = 0;
                      switch (pd_status)
                      {
                      case L7_POE_PORT_DETECTION_STATUS_FAULT:
                      case L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT:
                          if (hpcPoeAutoResetMode == L7_ENABLE)
                          {
                              hpcBrcmPoeMsgPortReset(cardIndex, poe_channel);
                          }
                      case L7_POE_PORT_DETECTION_STATUS_TEST:
                          hpcPlatformHwPoeLedOverload(i);         /* LED Amber ---- Default */
                          break;
                      case L7_POE_PORT_DETECTION_STATUS_DISABLED:
                      case L7_POE_PORT_DETECTION_STATUS_SEARCHING:
                      case L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER:
                      default:
                          hpcPlatformHwPoeLedDisable(i);      /* LED OFF --- Default */
                          break;
                      }
                  }
              }/* End of rc check */
              else
              {
                  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Port status get failed for port %d", i);
              }

              /* Port Measurement Query Command */
              rv = hpcBrcmPoeMsgPortMeasurementGet(cardIndex, poe_channel, 
                                                   &port_voltage, &port_curr, &port_power, &port_temp);

              if (L7_POE_OK(rv) == L7_TRUE)
              {
                  poeGlobalStatus_g.poePortStatus[i].powerConsumed = port_power;
                  poeGlobalStatus_g.poePortStatus[i].voltage = port_voltage;
                  poeGlobalStatus_g.poePortStatus[i].current = port_curr;
                  poeGlobalStatus_g.poePortStatus[i].temperature = port_temp;
              }
              else
              {
                  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Port measurements get failed for port %d\n", i);
              }

              /* Port statistics query command */
              rv = hpcBrcmPoeMsgPortStatisticsGet(cardIndex, poe_channel, 
                                               &mps_absent_ctr, &overload_ctr, &short_ctr, &power_denied_ctr,
                                               &invalid_sign_ctr);
              if (L7_POE_OK(rv) == L7_TRUE)
              {
                  /* accumulate the counters */
                  poeGlobalStatus_g.poePortStatistics[i].mpsAbsent         += mps_absent_ctr;
                  poeGlobalStatus_g.poePortStatistics[i].overLoad          += overload_ctr;
                  poeGlobalStatus_g.poePortStatistics[i].shortCounter      += short_ctr;
                  poeGlobalStatus_g.poePortStatistics[i].powerDenied       += power_denied_ctr;
                  poeGlobalStatus_g.poePortStatistics[i].invalidSignature  += invalid_sign_ctr;
                  /* clear the counters */
                  rv = hpcBrcmPoeMsgPortStatisticsReset(cardIndex, poe_channel);

                  if(L7_POE_OK(rv) != L7_TRUE)
                  {
                      LOG_MSG("POE Port statistics reset failed for port %d\n", i);
                  }
              }
              else
              {
                  LOG_MSG("POE Port statistics get failed for port %d\n", i);
              }
              hpcBrcmPoeReleaseAccess();
              osapiSleepMSec(50);
          } /* End of poe ports loop */
      }
      
#ifdef POWER_MANAGEMENT
      /* Power Management - Process collected data - Start */
      /* We dont need power management in the following cases *
       * 1. None of the ports are in Requesting or in Delivering Power state *
       * 2. All the ports in Requesting Power state                          *
       * 3. All the ports in Delivering Power state                          */

      /*** Find least priority delivering power port***/
      if (low_prio_delvr_port0 == 0xFF && low_prio_delvr_port1 != 0xFF)
      {
        low_prio_delvr_port = low_prio_delvr_port1;
      }
      else if (low_prio_delvr_port1 == 0xFF && low_prio_delvr_port0 != 0xFF)
      {
        low_prio_delvr_port = low_prio_delvr_port0;
      }
      else if (low_prio_delvr_port1 != 0xFF && low_prio_delvr_port0 != 0xFF)
      {
        if (hapiPoePortStatus[low_prio_delvr_port0].config.priority < hapiPoePortStatus[low_prio_delvr_port1].config.priority)
        {  
          low_prio_delvr_port = low_prio_delvr_port0;
        }
        else
        {
          low_prio_delvr_port = low_prio_delvr_port1;
        }
      }
      else
      {
        low_prio_delvr_port = 0xFF;
      }

      /*** Find high priority requesting power port***/
      if (hi_prio_req_port0 == 0xFF && hi_prio_req_port1 != 0xFF)
      {
        hi_prio_req_port = hi_prio_req_port1;
      }
      else if (hi_prio_req_port1 == 0xFF && hi_prio_req_port0 != 0xFF)
      {
        hi_prio_req_port = hi_prio_req_port0;
      }
      else if (hi_prio_req_port1 != 0xFF && hi_prio_req_port0 != 0xFF)
      {
        if (hapiPoePortStatus[hi_prio_req_port1].config.priority > hapiPoePortStatus[hi_prio_req_port0].config.priority)
        {  
          hi_prio_req_port  =    hi_prio_req_port1;
        }
        else
        {
          hi_prio_req_port  =   hi_prio_req_port0;
        }
      }
      else
      {
        hi_prio_req_port = 0xFF;
      }

      if(!(low_prio_delvr_port == 0xFF || hi_prio_req_port == 0xFF))
      {
        hpcPoeUnitCardIndexGetFromPhyPort(low_prio_delvr_port, &cardIndex_LO);
        hpcPoeUnitCardIndexGetFromPhyPort(hi_prio_req_port, &cardIndex_HI);
        if(cardIndex_LO != cardIndex_HI)
        {
          /* Power Readjustment is required */
          if(poeGlobalStatus_g.poePortStatus[low_prio_delvr_port].powerLimitType == L7_POE_LIMIT_TYPE_DOT3AF)
          {
            adjustPower = powerAllocForClass[poeGlobalStatus_g.poePortStatus[low_prio_delvr_port].powerClass];
          }
          else
          {
            adjustPower = poeGlobalStatus_g.poePortStatus[low_prio_delvr_port].powerLimit;
          }
          adjustPower += ((adjustPower * (100-poeGlobalStatus_g.usageThreshold))/100);
          hpcBrcmPoeGlobalPowerSourceUpdate(low_prio_delvr_port, hi_prio_req_port, adjustPower);
        }
      }
#endif
      /* Power Management - Process collected data - End */
    } /* End of port monitor task enable check */
  }/* End of eternal loop */
}

/*******************************************************************************
*
* @purpose Initializes POE chip to default state
*
* @param   void
*
* @returns L7_RC_t result
*
* @notes   Caution! This is running off the configurator's thread - 
*          do very little processing if possible. 
*          There is no correlation of this function's name
*          to the configurator's phases.
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeInitPhase2(L7_uint32 cardIndex)
{
  L7_uint32    i;
  L7_int32     rv;
  L7_uint32   start_port;
  L7_uint32   max_port;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  SYSAPI_POE_TRAP_DATA_MAIN_INIT_t trapData;
  SYSAPI_POE_MSG_t msg;
  L7_BOOL     initComplete = L7_TRUE;

  /* Set Power Management to factory default */
  rv = hpcBrcmPoeMsgPowerSourceModeSet(cardIndex, poeGlobalStatus_g.powerMgmtMode);
  if (L7_POE_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to set PoE PSE power management mode for PoE Card %u", cardIndex);
  }

  /* enable logical port map mode */
  rv = hpcBrcmPoeMsgLogicalPortMapModeSet(cardIndex, PORT_POE_ON);
  if (L7_POE_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to set PoE PSE logical port map mode for PoE Card %u", cardIndex);
  }

  if (L7_NOT_EXIST == hpcBrcmPoeChannelToPhysicalPortMap(cardIndex))
  {
    /* Needs FW update but it should have already happened */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to set PoE PSE logical port map for PoE Card %u", cardIndex);
  }

  start_port = cardData->start_phy_port;
  max_port = start_port + cardData->numOfPorts;

  for (i = start_port; i < max_port; i++)
  {
    poeGlobalStatus_g.poePortStatus[i].detectionStatus      = L7_POE_PORT_DETECTION_STATUS_DISABLED;
    poeGlobalStatus_g.poePortStatus[i].powerClass           = L7_POE_PORT_POWER_CLASS0;
    poeGlobalStatus_g.poePortStatus[i].powerConsumed        = 0;
    poeGlobalStatus_g.poePortStatus[i].voltage              = 0;
    poeGlobalStatus_g.poePortStatus[i].current              = 0;
    poeGlobalStatus_g.poePortStatus[i].temperature          = 0;

    poeGlobalStatus_g.poePortStatistics[i].mpsAbsent        = 0;
    poeGlobalStatus_g.poePortStatistics[i].invalidSignature = 0;
    poeGlobalStatus_g.poePortStatistics[i].powerDenied      = 0;
    poeGlobalStatus_g.poePortStatistics[i].overLoad         = 0;
    poeGlobalStatus_g.poePortStatistics[i].shortCounter     = 0;
  }

  /* create the POE monitor task */
  if (L7_NULL == poeMonitorTaskId) 
  {
      poeMonitorTaskId = osapiTaskCreate("poe_monitor",
                                         hpcBrcmPoeMonitorTask,
                                         0,
                                         L7_NULLPTR,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                         L7_DEFAULT_TASK_SLICE);

  }

  poeGlobalStatus_g.cardStatus[cardIndex].initialized = L7_TRUE;

  for (i = 0; i < hpcPoeCardCountGet(); i++)
  {
    if (L7_TRUE != poeGlobalStatus_g.cardStatus[i].initialized)
    {
      initComplete = L7_FALSE;
      break;
    }
  }

  /*
   * If all cards have been initialized, then tell the application.
   */
  if (L7_TRUE == initComplete)
  {
    hpcPoeInitComplete = L7_TRUE;
    /*
     * Now set initialization configuration.  Threshold can only be set for
     * all cards at once so it is set here after all cards have been
     * initialized.
     */
    msg.cmdType = SYSAPI_HPC_POE_CMD_SET;
    msg.cmdData.mainUsageThreshold.percent = poeGlobalStatus_g.usageThreshold;
    hpcBrcmPoeSystemUsageThreshold(&msg);

    trapData.usageThreshold = poeGlobalStatus_g.usageThreshold;
    trapData.powerMgmtMode  = poeGlobalStatus_g.powerMgmtMode;
    sysapiHpcPoeTrapSend(L7_ALL_INTERFACES, SYSAPI_HPC_POE_TRAP_MAIN_INIT_COMPLETE, &trapData);
  }

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Initialize POE component
*
* @param   void
*
* @returns L7_RC_t result
*
* @notes   Caution! This is running off the configurator's thread - 
*          do very little processing if possible. 
*
* @end
*
*******************************************************************************/
static L7_RC_t hpcBrcmPoeInit(SYSAPI_POE_MSG_t *const msg)
{
  HPC_POE_CARD_DATA_t       *cardData;
  L7_int32     result;
  L7_ushort16 hw_ver; 
  L7_uchar8   sw_ver, mode_pins, port_map;
  L7_uchar8   eeprom, config;
  L7_char8    versionString[80];
  L7_uint32   cardIndex = 0;
  L7_BOOL     cardInitialized  = L7_FALSE;
  char temp;

  if (L7_TRUE == poeInitialized)
  {
    return L7_SUCCESS;
  }

  memset(versionString,           0, sizeof(versionString));
  memset(poeMsgHandlers,          0, sizeof(poeMsgHandlers));

  if (sysapiHpcLocalPoeSupported() == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POE_COMPONENT_ID, "Poe will not be initalized on non-poe boards");
    return L7_SUCCESS;
  }

  /*
   * Save initialization values.  These will be set once initialization is complete.
   */
  poeGlobalStatus_g.usageThreshold = msg->cmdData.mainInit.usageThreshold;
  poeGlobalStatus_g.powerMgmtMode  = msg->cmdData.mainInit.powerMgmtMode;

  /* This function should be defined in Platform specific directory if PoE is present.
     This function will have any platform specific init code necessary. */
  if (hpcPlatformHwPoeInit() == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "PoE Initialization failed in Hardware");
    return L7_FAILURE;
  }

  /* shared between downloader, init code and monitor task as needed */
  poe_access_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (poe_access_sem == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "Could not create PoE Access Semaphore");
    return L7_FAILURE;
  }

  poe_tx_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (poe_tx_sem == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "Could not create PoE TX Semaphore");
    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    /* Even though there are two cards, we detect both cards through the first unit,
     * So, when we open the BSC device, we have to use the first unit, which is unit 0,
     * May need to expand the PoE Unit structure to include the unit */
    cardData->poe_fd = poe_device_open(cardData->type,
                                             &cardData->parms,
                                             cardData->file_descr, 0x0);
    if (cardData->poe_fd < 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Could not open %s for writing on card %d.", cardData->file_descr, cardIndex);
      continue;
    }

    cardInitialized = L7_TRUE;
  }

  if (L7_TRUE != cardInitialized)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "No cards initialized, PoE not operational.");
    return L7_FAILURE;                  /* No cards were initialized so don't continue. */
  }
  
  
  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);
    if (L7_NULLPTR == cardData)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
      return L7_SUCCESS;
    }

    /* Query the POE controller to get the number of poe_ports supported */
    result = hpcBrcmPoeMsgSystemStatusGet(cardIndex, 
            &mode_pins, &temp, &port_map, &hw_ver, &sw_ver, &eeprom, &config);

    if (L7_POE_OK(result) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Unable to Communicate with the PoE Card %u", cardIndex);
      if (brcmPoeDebugEnabled)
        POE_HPC_DPRINTF("Unable to Communicate with the PoE Card %u", cardIndex);
    }
    else
    {
      if (brcmPoeDebugEnabled)
        POE_HPC_DPRINTF("card: %d mode_pins: 0x%0x poe_ports: %d port_map: %d hw_ver: 0x%0x sw_ver: 0x%0x eeprom: %d  config: %d",
                        cardIndex, mode_pins, cardData->numOfPorts, port_map, hw_ver, sw_ver, eeprom, config);
    }
  }

  /* Fill into the registry so box services can retrieve it later. */
  sprintf(versionString, "%d_%d", hw_ver, sw_ver);

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
      /* Initialize mechanism to use POE LED's */
      hpcBrcmPoeLedInit(cardIndex);
      /* initialize poe controller */
      if (hpcBrcmPoeInitPhase2(cardIndex) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "hpcBrcmPoeInitPhase2 failed for PoE Card %u.", cardIndex);
      }
  }

  if (brcmPoeDebugEnabled)
    POE_HPC_DPRINTF("POE ports init done");

  /*
   * Enough initialization has now been completed to populate the function table.
   */
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_AUTO_RESET_MODE]           = hpcBrcmPoeSystemResetAutoMode;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_LOGICAL_MAP_MODE]          = hpcBrcmPoeSystemLogicalMapMode;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_DETECTION_MODE]            = hpcBrcmNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_FW_VERSION]                = hpcBrcmPoeSystemFwVersion;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_NOTIFICATION_CONTROL]      = hpcBrcmNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_OPERATIONAL_STATUS]        = hpcBrcmPoeSystemStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_CONSUMPTION]         = hpcBrcmPoeSystemPowerConsumption;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_MGMT_MODE]           = hpcBrcmPoeSystemPowerMgmt;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_NOMINAL]             = hpcBrcmPoeSystemPowerNominal;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_USAGE_THRESHOLD]           = hpcBrcmPoeSystemUsageThreshold;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_INIT]                      = hpcBrcmPoePortInit;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_ADMIN_STATE]               = hpcBrcmPoePortAdminEnable;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTERS]                  = hpcBrcmPoePortStatistics;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_INVALID_SIGNATURE] = hpcBrcmPoePortCounterInvSig;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_MPS_ABSENT]        = hpcBrcmPoePortCounterMpsAbsent;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_OVERLOAD]          = hpcBrcmPoePortCounterOverload;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_POWER_DENIED]      = hpcBrcmPoePortCounterPowerDenied;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_SHORT]             = hpcBrcmPoePortCounterShort;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_DETECTION_MODE]            = hpcBrcmPoePortDetectionMode;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_DISCONNECT_TYPE]           = hpcBrcmPoePortDisconnectType;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_DETECTION_STATUS]          = hpcBrcmPoePortDetectionStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_ERROR_CODE]                = hpcBrcmPoePortErrorCode;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_HIGH_POWER_MODE]           = hpcBrcmPoePortHighPowerMode;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_OPERATIONAL_STATUSES]      = hpcBrcmPoePortStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_CLASSIFICATION]      = hpcBrcmPoePortClassification;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_CUR_LIMIT]           = hpcBrcmPoePortPowerLimit;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_MAX_LIMIT]           = hpcBrcmNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_MIN_LIMIT]           = hpcBrcmNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT]              = hpcBrcmPoePortPower;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT_CURRENT]      = hpcBrcmPoePortPowerCurrent;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT_VOLTAGE]      = hpcBrcmPoePortVoltage;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_PAIRS_CONTROL]       = hpcBrcmPoePortPowerPairs;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_PRIORITY]            = hpcBrcmPoePortPowerPriority;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_RESET]                     = hpcBrcmPoePortReset;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_TEMPERATURE]               = hpcBrcmPoePortTemperature;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_VIOLATION_TYPE]            = hpcBrcmPoePortViolationType;
#ifdef PLAT_POE_FW_UPDATE
  poeMsgHandlers[SYSAPI_HPC_POE_FIRMWARE_DOWNLOAD_PROGRESS]     = hpcBrcmPoeSystemDownloadProgressIndicate;
#endif

  poeInitialized = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
 *********************************************************************
 *
 * Beginning of sysapi message receive function.
 *
 *********************************************************************
 *********************************************************************/

/*********************************************************************
* @purpose   To handle all poe Controller messages
*
* @param   SYSAPI_POE_MSG_t *msg  pointer to PoE message
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hpcPoeControllerMsg(SYSAPI_POE_MSG_t *const msg)
{
  L7_RC_t rc = L7_FAILURE;

  /*
   * Have to check for the Init message since the function pointers will not
   * be initialized yet.
   */
  if (SYSAPI_HPC_POE_MAIN_INIT == msg->msgId)
  {
    rc = hpcBrcmPoeInit(msg);
  }
  else if ((SYSAPI_HPC_POE_MSG_ID_MIN < msg->msgId) &&
           (SYSAPI_HPC_POE_MSG_ID_MAX > msg->msgId) &&
           (L7_NULL != poeMsgHandlers[msg->msgId]))
  {
    rc = poeMsgHandlers[msg->msgId](msg);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
            "%s: Invalid message received msgId = %u", __FUNCTION__, msg->msgId);
    rc = L7_NOT_SUPPORTED;
  }

  return rc;
}

/*********************************************************************
 *********************************************************************
 *
 * Beginning of Debug functions.
 *
 *********************************************************************
 *********************************************************************/

int hpcBrcmPoeDebugEnable(L7_uint32 enabled)
{
  brcmPoeDebugEnabled = enabled;

  return 0;
}

int hpcBrcmPoeDebugMonitorEnable(L7_BOOL enabled)
{
  brcmPoeMonitorDebugEnabled = enabled;

  return 0;
}

int hpcBrcmPoeDebugFirmwareEnable(L7_BOOL enabled)
{
  brcmPoeFirmwareDebugEnabled = enabled;

  return 0;
}

/*******************************************************************************
**                             Debug Routines                                 **
*******************************************************************************/

/*******************************************************************************
* @purpose To debug the POE LED functionality
*
* @param   port         @b{(input)}  port number
* @param   value        @b{(input)}  indicates the LED color
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmPoeDebugLed(int port, int value)
{
  switch (value)
  {
  case 0:
    hpcPlatformHwPoeLedDisable(port);
    break;
  case 1:
    hpcPlatformHwPoeLedDeliveringPower(port);
    break;
  case 2:
    hpcPlatformHwPoeLedOverload(port);
    break;
  case 3:
    hpcPlatformHwPoeLedOverload(port);
  default:
    break;
  }
}

/*******************************************************************************
* @purpose Debug function used to check the system validation
*
* @param   devId        @b{(input)}  PoE card identifier
* @param   cmd          @b{(input)}  multipurpose variable
* @param   lport        @b{(input)}  lport number
* @param   var1         @b{(input)}  multipurpose variable
* @param   var2         @b{(input)}  multipurpose variable
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
void hpcBrcmDebugPoeDriver(L7_uchar8 psePort, int cmd, L7_uchar8 val1, int val2)
{
  L7_uchar8    ch1, ch2, ch3, ch4, ch5;
  L7_ushort16  sh1, sh2, sh3, sh4;
  L7_int32     rv;
  L7_uint32    cardIndex;
  L7_RC_t      rc = L7_FAILURE;

  ch1 = ch2 = ch3 = ch4 = ch5 = 0xBB;
  sh1=sh2=sh3=sh4 = 0xBB; 

  hpcPoeUnitCardIndexGetFromPhyPort(psePort, &cardIndex);

  switch(cmd)
  {
    case 0: /* To enable or disable POE port */
      rv = hpcBrcmPoeMsgPortOnOffSet(cardIndex, psePort, val1);
      if(L7_POE_OK(rv) == L7_TRUE)
      {
        sysapiPrintf("hapiBroadDebugPoeDriver: lport: %d admin status: %d\n", psePort, val1);
      }
      break;

    case 1: /* To get the pd_status and class_info for a PoE port */
      rv = hpcBrcmPoeMsgPortStatusGet(cardIndex, psePort, &ch1, &ch2, &ch3, &ch4, &ch5);
      if(L7_POE_OK(rv) == L7_TRUE)
      {
        sysapiPrintf("hapiBroadDebugPoeDriver: lport: %d pd_status: 0x%0x class_info: 0x%0x\n", 
               psePort, ch1, ch2);
      }
      break;

    case 2: /* To view the Port measurements */
      rv = hpcBrcmPoeMsgPortMeasurementGet(cardIndex, psePort, &sh1, &sh2, &sh3, &sh4);
      if(L7_POE_OK(rv) == L7_TRUE)
      {
        sysapiPrintf("hapiBroadDebugPoeDriver: lport: %d: port_voltage: 0x%0x port_curr: 0x%0x port_power: 0x%0x port_temp: 0x%0x\n", 
            psePort, sh1, sh2, sh3, sh4);
      }
      break;

    case 3: /* To get the system Power Parameters */
      rc = hpcBrcmPoeMsgPowerSourceTotalPowerGet(cardIndex, &sh1, &sh2, &ch1);
      if(rc == L7_SUCCESS)
      {
        sysapiPrintf("hapiBroadDebugPoeDriver: power_consumed: 0x%0x  power_avilable: 0x%0x\n", sh1, sh2);
      }
      break;

    case 4: /* To get the basic parameters of the PoE chip */
      {
        L7_ushort16 hw_ver;
        L7_uchar8   sw_ver, mode_pins, port_map, poe_prts, eeprom, config;

        rv = hpcBrcmPoeMsgSystemStatusGet(cardIndex, &mode_pins,
            &poe_prts,
            &port_map,
            &hw_ver,
            &sw_ver,
            &eeprom,
            &config);

          sysapiPrintf("rv value: %d\n", rv);
          sysapiPrintf("hapiBroadDebugPoeDriver: mode_pins: %d poe_ports: %d port_map: %d hw_ver: 0x%0x sw_ver: 0x%0x  eeprom: %d  config: %d\n",
              mode_pins, poe_prts, port_map, hw_ver, sw_ver, eeprom, config);
      }

    default:
      sysapiPrintf("hapiBroadDebugPoeDriver: Invalid input. Enter (0-4)\n");
  }
}

/*********************************************************************
 *
 * @purpose Debug function to dump the stored status.
 *
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
int hpcBrcmPoeDebugStatusDump(void)
{
  L7_uint i;

  for (i = 0; i < L7_MAX_POE_CARDS_PER_UNIT; i++)
  {
    printf("\r\nStatus for card %d:\r\n", i);
    printf("          cpuStatus1 = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].cpuStatus1);
    printf("          cpuStatus2 = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].cpuStatus2);
    printf("      factoryDefault = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].factoryDefault);
    printf("generalInternalError = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].generalInternalError);
    printf("        privateLabel = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].privateLabel);
    printf("            userByte = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].userByte);
    printf("          deviceFail = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].deviceFail);
    printf("      tempDisconnect = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].tempDisconnect);
    printf("           tempAlarm = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].tempAlarm);
    printf("    powerConsumption = %u\r\n",      poeGlobalStatus_g.cardStatus[i].powerConsumption);
    printf("  maxShutdownVoltage = %u\r\n",      poeGlobalStatus_g.cardStatus[i].maxShutdownVoltage);
    printf("  minShutdownVoltage = %u\r\n",      poeGlobalStatus_g.cardStatus[i].minShutdownVoltage);
    printf("     activePowerBank = %u\r\n",      poeGlobalStatus_g.cardStatus[i].activePowerBank);
    printf("   maxPowerAvailable = %u\r\n",      poeGlobalStatus_g.cardStatus[i].maxPowerAvailable);
    printf("  powerSupplyVoltage = %u\r\n",      poeGlobalStatus_g.cardStatus[i].powerSupplyVoltage);

  }

  printf("\r\nPort Status:\r\n");
  printf("Port Power Voltage Current Enabled Status Latch Class Detect. PwrLimit PwrLimTmp Priority\r\n");
  printf("---- ----- ------- ------- ------- ------ ----- ----- ------- -------- --------- --------\r\n");
  for (i = 0; i < L7_POE_MAX_CHANNELS; i++)
  {
    printf(" %-3u %-5u %-7u %-7u %7.7s 0x%2.2x   0x%2.2x  %-5u %-7u %-8u %-9u %-8u\r\n",
           i,
           poeGlobalStatus_g.poePortStatus[i].powerConsumed,
           poeGlobalStatus_g.poePortStatus[i].voltage,
           poeGlobalStatus_g.poePortStatus[i].current,
           L7_TRUE == poeGlobalStatus_g.poePortStatus[i].enabled ? "Yes" : "No",
           poeGlobalStatus_g.poePortStatus[i].status,
           poeGlobalStatus_g.poePortStatus[i].latch,
           poeGlobalStatus_g.poePortStatus[i].powerClass,
           poeGlobalStatus_g.poePortStatus[i].detectionStatus,
           poeGlobalStatus_g.poePortStatus[i].powerLimit,
           poeGlobalStatus_g.poePortStatus[i].powerLimitTemp,
           poeGlobalStatus_g.poePortStatus[i].priority);
  }

  printf("\r\nPort Statistics:\r\n");
  printf("Port Underload    Invalid Sig  Power Denied Overload     Short\r\n");
  printf("---- ------------ ------------ ------------ ------------ ------------\r\n");
  for (i = 0; i < L7_POE_MAX_CHANNELS; i++)
  {
    printf(" %-3u %-12u %-12u %-12u %-12u %-12u\r\n",
           i,
           poeGlobalStatus_g.poePortStatistics[i].mpsAbsent,
           poeGlobalStatus_g.poePortStatistics[i].invalidSignature,
           poeGlobalStatus_g.poePortStatistics[i].powerDenied,
           poeGlobalStatus_g.poePortStatistics[i].overLoad,
           poeGlobalStatus_g.poePortStatistics[i].shortCounter);
  }

  return i;
}

/*********************************************************************
 *
 * @purpose Debug Message to get Hardware counters of the PoE controllers.
 *
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
void hpcBrcmPoeDebugSysDump(L7_uint32 cardIndex)
{
  L7_ushort16 hw_ver;
  L7_uchar8   sw_ver, mode_pins, port_map, poe_prts, eeprom, config;
  L7_int32     rv;

  printf("Hardware Status for the PoE Card 0x%x\r\n", cardIndex);

  if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
      return;
  }

  rv = hpcBrcmPoeMsgSystemStatusGet(cardIndex, &mode_pins,
            &poe_prts,
            &port_map,
            &hw_ver,
            &sw_ver,
            &eeprom,
            &config);

  hpcBrcmPoeReleaseAccess();

  if (L7_POE_OK(rv) == L7_TRUE)
  {
    printf("\r\nSystem Status:\r\n");
    printf("        SW Version: 0x%2.2x\r\n", sw_ver);
    printf("         Device ID: 0x%2.2x\r\n", hw_ver);
    printf("  Num Of PoE Ports: %d\r\n", poe_prts);
    if (port_map == 0x01) 
    {
        printf("  Logical Port Map: Enabled\r\n");
    }
    else
    {
        printf("  Logical Port Map: Disabled\r\n");
    }
    if (eeprom == 0x01) 
    {
        printf("  EEPROM Status: Update in Progress\r\n");
    }
    else
    {
        printf("  EEPROM Status: Update is Done\r\n");
    }
    if (config == 0x01) 
    {
        printf("  Config Status: Dirty\r\n");
    }
    else
    {
        printf("  Config Status: Done\r\n");
    }
    printf("  Mode Pins Status: 0x%2.2x\r\n", mode_pins);
  }
  else
  {
    printf("Could not read System Status.\r\n");
  }
}

/*********************************************************************
 *
 * @purpose Debug Message to get Hardware counters of the PoE controllers.
 *
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
int hpcBrcmPoeDebugHwDump(L7_uint32 cardIndex, L7_uchar8 poe_port)
{
  L7_int32     rv;
  L7_uchar8   channel;

  /* Port Status */
  L7_uchar8   status;
  L7_uchar8   power_class;
  L7_uchar8   detect_info;
  L7_uchar8   remote_pd;
  L7_uchar8   port_mask;

  /* Port Statistics */
  L7_uchar8   mps_absent_ctr = L7_NULL; 
  L7_uchar8   overload_ctr = L7_NULL; 
  L7_uchar8   short_ctr = L7_NULL; 
  L7_uchar8   power_denied_ctr = L7_NULL;
  L7_uchar8   invalid_sign_ctr = L7_NULL;

  /* Power Counters */
  L7_ushort16     allocPower; 
  L7_ushort16     availPower;
  L7_uchar8       mpsmStatus;

  /* Port Configuration */
  L7_uchar8   pse_mode;
  L7_uchar8   auto_mode;
  L7_uchar8   detection_type;
  L7_uchar8   classify_type;
  L7_uchar8   disconnect_type;
  L7_uchar8   pair_config;

  /* Port Extended Configuration */
  L7_uchar8   power_mode;
  L7_uchar8   violation_type;
  L7_uchar8   max_power;
  L7_uchar8   priority;
  L7_uchar8   physical_port;

  /* Port Measurements */
  L7_ushort16     port_voltage; 
  L7_ushort16     port_curr;
  L7_ushort16     port_power; 
  L7_ushort16     port_temp;

  /* Multiple Power Source Settings */
  L7_uint32       num_banks;
  L7_uchar8       power_mgmt_mode;
  L7_ushort16     max_source_power;
  L7_ushort16     guard_band;

  HPC_POE_CARD_DATA_t *cardData;
  L7_uint32   start_port;
  L7_uint32   max_port;
  L7_uchar8   poe_channel = 0;

    cardData = hpcPoeCardDataGet(cardIndex);

    hpcBrcmPoeDebugSysDump(cardIndex);

    printf("\r\nSingle Port Status (0x21):\r\n");
    printf("Port Status Power Class  Detect Info  Remote PD  Port Mask\r\n");
    printf("---- ------ -----------  -----------  ---------  ----------\r\n");

    start_port = cardData->start_phy_port;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      if (((poe_port != POE_CHANNEL_ALL) && (poe_port == channel))
          || (poe_port == POE_CHANNEL_ALL)) 
      {
          /* Port Status Query Command */
          if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(channel, &poe_channel)) 
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", channel);
          }

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return L7_FAILURE;
          }

          rv = hpcBrcmPoeMsgPortStatusGet(cardIndex, poe_channel, &status, &power_class, &detect_info,
                                          &remote_pd, &port_mask);

          hpcBrcmPoeReleaseAccess();

          if (L7_POE_OK(rv) == L7_TRUE)
          {
              printf("%02d    0x%2.2x     0x%2.2x         0x%2.2x         0x%2.2x        0x%2.2x\r\n",
                     channel, status, power_class, detect_info, remote_pd, port_mask);
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port status for POE port %02d", channel);
          }
      }
    }

    printf("\r\nSingle Port Statistics (0x22):\r\n");
    printf("Port  MPS Absent Ctr  Overload Ctr  Short Ctr  Power Denied Ctr   Invalid Sign Ctr\r\n");
    printf("---- ---------------  ------------  ---------  ----------------   ----------------\r\n");   

    start_port = cardData->start_phy_port;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      if (((poe_port != POE_CHANNEL_ALL) && (poe_port == channel))
          || (poe_port == POE_CHANNEL_ALL)) 
      {
          /* Port Statistics Query Command */
          if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(channel, &poe_channel)) 
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", channel);
          }

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return L7_FAILURE;
          }

          rv = hpcBrcmPoeMsgPortStatisticsGet(cardIndex, poe_channel, 
                                                &mps_absent_ctr, &overload_ctr, &short_ctr, &power_denied_ctr,
                                                &invalid_sign_ctr);

          hpcBrcmPoeReleaseAccess();

          if (L7_POE_OK(rv) == L7_TRUE)
          {
              printf("%02d       0x%2.2x             0x%2.2x              0x%2.2x           0x%2.2x               0x%2.2x\r\n",
                     channel, mps_absent_ctr, overload_ctr, short_ctr, power_denied_ctr, invalid_sign_ctr);
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port statistics for POE port %02d", channel);
          }
      }
    }

    if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
      return L7_FAILURE;
    }

    rv = hpcBrcmPoeMsgPowerSourceTotalPowerGet(cardIndex,
                                                &allocPower, &availPower, &mpsmStatus);

    hpcBrcmPoeReleaseAccess();

    if (L7_POE_OK(rv) == L7_TRUE)
    {
      printf("\r\nSystem Power Status (0x23):\r\n");
      printf("        Total Allocated Power: 0x%2.2x\r\n", allocPower);
      printf("        Total Available Power: 0x%2.2x\r\n", availPower);
      printf("        Multiple Power Source: 0x%2.2x\r\n", mpsmStatus);
    }

    printf("\r\nSingle Port Configuration (0x25):\r\n");
    printf("Port  PSE Mode  Auto Mode  Detection  Classification  Disconnect  Power Pair\r\n");
    printf("----  --------  ---------  ---------  --------------  ----------  ----------\r\n");
    
    start_port = cardData->start_phy_port;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      if (((poe_port != POE_CHANNEL_ALL) && (poe_port == channel))
          || (poe_port == POE_CHANNEL_ALL)) 
      {
          /* Port Configuration Query Command */
          if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(channel, &poe_channel)) 
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", channel);
          }

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return L7_FAILURE;
          }

          rv = hpcBrcmPoeMsgPortConfigGet(cardIndex, poe_channel, 
                                                &pse_mode, &auto_mode, &detection_type, &classify_type,
                                                &disconnect_type, &pair_config);

          hpcBrcmPoeReleaseAccess();

          if (L7_POE_OK(rv) == L7_TRUE)
          {
              printf("%02d     0x%2.2x       0x%2.2x         0x%2.2x           0x%2.2x             0x%2.2x          0x%2.2x\r\n",
                     channel, pse_mode, auto_mode, detection_type, classify_type, disconnect_type, pair_config);
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port Configuration for POE port %02d", channel);
          }
      }
    }

    printf("\r\nSingle Port Extended Configuration (0x26):\r\n");
    printf("Port  High Power Mode   Violation Type   Max Power   Priority   Logical Port\r\n");
    printf("----  ---------------   --------------   ---------   --------   -------------\r\n");
    
    start_port = cardData->start_phy_port;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      if (((poe_port != POE_CHANNEL_ALL) && (poe_port == channel))
          || (poe_port == POE_CHANNEL_ALL)) 
      {
          /* Port Extended Configuration Query Command */
          if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(channel, &poe_channel)) 
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", channel);
          }

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return L7_FAILURE;
          }

          rv = hpcBrcmPoeMsgPortExtendedConfigGet(cardIndex, poe_channel, 
                                                  &power_mode, &violation_type, &max_power, &priority,
                                                  &physical_port);

          hpcBrcmPoeReleaseAccess();

          if (L7_POE_OK(rv) == L7_TRUE)
          {
              printf("%02d        0x%2.2x            0x%2.2x             0x%2.2x           0x%2.2x            0x%2.2x\r\n",
                     channel, power_mode, violation_type, max_power, priority, physical_port);
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port Extended Configuration for POE port %02d", channel);
          }
      }
    }

    printf("\r\nSingle Port Measurements (0x30):\r\n");
    printf("Port  Voltage   Current   Temperature  Power\r\n");
    printf("----  -------   -------   -----------  --------\r\n");
    
    start_port = cardData->start_phy_port;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      if (((poe_port != POE_CHANNEL_ALL) && (poe_port == channel))
          || (poe_port == POE_CHANNEL_ALL)) 
      {
          /* Port Measurement Query Command */
          if (L7_SUCCESS != hpcPoeUnitLogicalPoePortGetFromPhyPort(channel, &poe_channel)) 
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Logical Port for port %u", channel);
          }

          if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
              return L7_FAILURE;
          }

          rv = hpcBrcmPoeMsgPortMeasurementGet(cardIndex, poe_channel, 
                                                    &port_voltage, &port_curr, &port_power, &port_temp);

          hpcBrcmPoeReleaseAccess();

          if (L7_POE_OK(rv) == L7_TRUE)
          {
              printf("%02d    0x%2.2x     0x%2.2x        0x%2.2x          0x%2.2x\r\n",
                     channel, port_voltage, port_curr, port_temp, port_power);
          }
          else
          {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port Measurements for POE port %02d", channel);
          }
      }
    }

    printf("\r\nSystem Power Management Configuration (0x27):\r\n");
    printf("Power Source   Power Mgmt Mode   Max Power   Guard Band\r\n");
    printf("------------   ---------------   ---------   ----------\r\n");

    for (num_banks = 0; num_banks < cardData->num_of_power_banks; num_banks++) 
    {
        if (hpcBrcmPoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
            return L7_FAILURE;
        }

        /* System Power Management Configuration Quesry Command */
        rv = hpcBrcmPoeMsgPowerSourceConfigGet(cardIndex, num_banks, &power_mgmt_mode, &max_source_power, &guard_band);

        hpcBrcmPoeReleaseAccess();

        if (L7_POE_OK(rv) == L7_TRUE) 
        {
            printf("  %02d            0x%2.2x                 0x%2.2x              0x%2.2x\r\n",
               num_banks, power_mgmt_mode, max_source_power, guard_band);
        }
    }

  return 0;
}

int hpcBrcmPoeDebugHwDumpAll(void)
{
   L7_uint32   cardIndex;

   for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
   {
       hpcBrcmPoeDebugHwDump(cardIndex, POE_CHANNEL_ALL);
       printf("\n\n\n");
   }

   return 0;
}

#ifdef PLAT_POE_FW_UPDATE
/*******************************************************************************
* @purpose To upgrade the POE firmware
*
* @param   devId        @b{(input)}  PoE card identifier
*
* @returns none
*
* @notes   Currently this feature is available from devshell utility
*
* @end
*
*******************************************************************************/
void hpcBrcmPoeUpgradeFirmware(L7_uint32 cardIndex)
{
  #define MEMMAPSIZE 0x4000
  FILE *fpHexfile  = NULL;
  L7_uchar8 strTemp[6];
  L7_ushort16 nByteCount, nAddress, nCheckSum, i,j =0;
  L7_uchar8 *bData;
  L7_ushort16 offSet = 0;
  L7_ushort16 counter;
  L7_uchar8  txBuf[15], buffer[256];
  L7_int32      rv;

  if (hpcPoeCardIndexIsValid(cardIndex) == L7_FALSE)
  {
    sysapiPrintf("\hpcBrcmPoeUpgradeFirmware: Bad device ID!");
    return;
  }
  bData = (L7_uchar8 *)osapiMalloc(L7_POE_COMPONENT_ID, MEMMAPSIZE);
  if (bData == L7_NULLPTR)
  {
    sysapiPrintf("\hpcBrcmPoeUpgradeFirmware: Failed to allocated memory!");
    return;
  }

  fpHexfile = fopen("/usr/local/ptin/log/fastpath/IMAGE.HEX", "r");
  if (fpHexfile)
  {
     memset(bData, 0x00, MEMMAPSIZE);
     while(fgets(buffer, sizeof(buffer), fpHexfile))
     {
        if (strlen(buffer) == 0)
         continue;

        buffer[strlen(buffer)-2] = '\0';
        if (buffer[0] == ':')
        {
          strncpy(strTemp, buffer + 1, 2);
          strTemp[2] = '\0';
          nByteCount = strtol(strTemp, NULL, 16);
          strncpy(strTemp, buffer + 3, 4);
          strTemp[4] = '\0';
          nAddress = strtol(strTemp, NULL, 16);
          if(nByteCount == 0)
          {
            break;
          }
          if (MEMMAPSIZE < (nAddress+nByteCount))
          {
            sysapiPrintf("\nError: Bad Address");
            osapiFree(L7_POE_COMPONENT_ID, bData);
            return;
          }

          if(strlen(buffer) < (2 * nByteCount + 11))
          {
            sysapiPrintf("\nError: Short Line");
            osapiFree(L7_POE_COMPONENT_ID, bData);
            return;
          }

          nCheckSum = nByteCount + nAddress;
          for (i = 0; i < nByteCount; i++, nAddress++)
          {
             strncpy(strTemp, buffer + (2*i+9), 2);
             strTemp[2] = '\0';
             bData[nAddress] = strtol(strTemp, NULL, 16);
             nCheckSum += bData[nAddress];
          }
          strncpy(strTemp, buffer + (2*i+9), 2);
          strTemp[2] = '\0';
          nCheckSum = nCheckSum & 0xFF;
          if (nCheckSum != strtol(strTemp, NULL, 16))
          {
#if 0
            sysapiPrintf("\nCheck sum error");
#endif
          }
        }
     }
     fclose(fpHexfile);

/* Disable PoE operations while sending online upgrade commands */
     hpcPoeMonitorEnabled = L7_FALSE;
     hpcPoeInitComplete = L7_FALSE;

   sysapiPrintf("\hpcBrcmPoeUpgradeFirmware: Upgrade in progress...");
   while (offSet < MEMMAPSIZE)
   {
     txBuf[0] = 0xE0;

     txBuf[1] = (offSet & 0xFF00) >> 8;
     txBuf[2] = (offSet & 0xFF);

     for (counter = 3; counter < 11; counter++)
     {
       txBuf[counter] = bData[offSet];
       offSet++;
     }

     /* Send it to the PoE controler */
     rv = hpcBrcmPoeMsgCustomDataSend(cardIndex, txBuf);
     if (L7_POE_OK(rv) != L7_TRUE)
     {
       sysapiPrintf("\hpcBrcmPoeUpgradeFirmware: Command send failed for offSet 0x%04X  count-> %u!", (offSet - 8),
                    ++j);
     }
     osapiSleepMSec(75);
   }
   sysapiPrintf("\hpcBrcmPoeUpgradeFirmware: Upgrade complete!");
  
/* Re-Enable PoE operations while sending online upgrade commands */
     hpcPoeMonitorEnabled = L7_TRUE;
     hpcPoeInitComplete = L7_TRUE;
  }
  else
  {
    sysapiPrintf("\nPoE IMAGE.HEX not found in /usr/local/ptin/log/fastpath");
  }
  osapiFree(L7_POE_COMPONENT_ID, bData);
}
#endif
