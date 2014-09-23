/*********************************************************************
 * <pre>
 * LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename  hpc_poe_pwrdsne.c
 *
 * @purpose   This file contains the PoerDSine POE Controller specific code
 *
 * @component hpc
 *
 * @comments
 *
 * @create    10/2/2003
 *
 * @author    colinw
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
#include <selectLib.h>

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "hpc_db.h"
#include "defaultconfig.h"
#include "unitmgr_api.h"
#include "poe_exports.h"
#include "hpc_poe_pwrdsne.h"
#include "hpc_poe.h"
#include "hpc_poe_platform.h"
#ifdef PLAT_POE_FW_UPDATE
#include "hpc_poe_firmware.h"
#endif

#define POE_CONTROLLER_MSG_SIZE      15
#define POE_CONTROLLER_QUEUE_SIZE    10
#define POE_CONTROLLER_WRITE_RETRIES 3

/* timeouts for message responses -- units are in milliseconds */
#define POE_CONTROLLER_DEFAULT_TIMEOUT      1000
#define POE_CONTROLLER_RESET_TIMEOUT        5000
#define POE_CONTROLLER_ACCESS_TIMEOUT       1000

typedef struct
{
  L7_uint32 size;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE * 2];
} HPC_POE_CONTROLLER_MESSAGE_t;

typedef struct
{
  L7_uint32   cardIndex;
  void       *messageQueue;
} HPC_POE_RX_TASK_DATA_t;

typedef struct
{
  /* from Get Port Measurements */
  L7_uint32                        powerConsumed;       /* unit is in milliWatts */
  L7_uint32                        voltage;             /* unit is in Volts */
  L7_uint32                        current;             /* unit is in milliAmps */
  /* from Get Single Port Status */
  L7_uchar8                        enabled;
  L7_uchar8                        status;
  L7_uchar8                        latch;
  L7_POE_PORT_POWER_CLASS_t        powerClass;
  /* indirectly from Get Single Port Status */
  L7_POE_PORT_DETECTION_STATUS_t   detectionStatus;
  /* from Get Port Power Limit */
  L7_uchar8                        powerLimit;
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
  L7_uchar8   temperature;              /* Celsius */
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
  HPC_POE_PORT_STATUS_t     poePortStatus    [L7_POE_MAX_CHANNELS + 1];
  HPC_POE_PORT_STATISTICS_t poePortStatistics[L7_POE_MAX_CHANNELS + 1];
  HPC_POE_CARD_STATUS_t     cardStatus       [L7_MAX_POE_CARDS_PER_UNIT];
  L7_uint32                 usageThreshold;
  L7_POE_POWER_MGMT_t       powerMgmtMode;
  L7_POE_DETECTION_t        detectionType;
} HPC_POE_GLOBAL_STATUS_t;

/* debug stuff */
static L7_uint32  poeMonitorDebugEnabled   = 0;
static L7_uint32  poeDebugEnabled          = 0;
static L7_uint32  poeReadDebugEnabled      = 0;
static L7_uint32  poeFirmwareDebugEnabled  = 0;
static L7_uint32  poe_tx_retry_count       = 0;
static L7_uint32  poe_tx_no_response_count = 0;
static L7_uint32  poe_bad_checksum_count   = 0;
static L7_uint32  poe_bad_echo_count       = 0;

/* operational stuff */
static L7_uchar8  echo_num                 = 0;
static L7_BOOL    poeMonitorEnabled        = L7_TRUE;
static L7_BOOL    poeInitComplete          = L7_FALSE;
static L7_BOOL    poeInitialized           = L7_FALSE;
static L7_uint32  poeMonitorTaskId         = L7_NULL;
static void      *poe_access_sem           = L7_NULL;
static void      *poe_tx_sem               = L7_NULL;
static void      *poe_rx_msgq[L7_MAX_POE_CARDS_PER_UNIT];
static L7_char8   versionString[80];    /* Firmware version */

static HPC_POE_GLOBAL_STATUS_t poeGlobalStatus_g;

#ifdef PLAT_POE_FW_UPDATE
#include "hpc_poe_firmware.h"
static L7_BOOL    poeFWDownloadInProgress[L7_MAX_POE_CARDS_PER_UNIT];
#endif
static L7_BOOL    poeFWInvalid           [L7_MAX_POE_CARDS_PER_UNIT];

/*
 * This is an array of pointers to message handling functions.
 */
static L7_RC_t (*poeMsgHandlers[SYSAPI_HPC_POE_MSG_ID_MAX])(SYSAPI_POE_MSG_t *msg);

/*********************************************************************
 *********************************************************************
 *
 * Beginning of utility functions.
 *
 *********************************************************************
 *********************************************************************/

#define HPCPOE_PRINTF_QUEUE "PRINTF-Q"
#define HPCPOE_PRINTF_QUEUE_MESSAGE_COUNT 256
#define HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE  180

#ifdef HPC_POE_PWRDSNE_DEBUG
static void *hpcPoePrintfQueueID = L7_NULLPTR;
static L7_int32 hpcPoePrintfTaskID = L7_NULL;

/**************************************************************************
*
* @purpose  Task that creates, then reads an osapiMsgQueue and prints
*           any data in the queue to the screen.
*
* @param    none
*
* @returns  L7_SUCCESS  If task runs correctly, this will never return.
* @returns  L7_ERROR    If queue fails to be created.
*
* @notes This is used with HPCPOE_PRINTF in order to solve the problem
*        with printing in interrupt context.
*
* @end
*
*************************************************************************/
L7_RC_t hpcPoePrintfMsgQueueTask(void)
{
  L7_BOOL done = L7_FALSE;
  L7_uint32 status;
  L7_uchar8 buf[HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE];

  memset(buf, 0, sizeof(buf));
  hpcPoePrintfQueueID = osapiMsgQueueCreate(HPCPOE_PRINTF_QUEUE, HPCPOE_PRINTF_QUEUE_MESSAGE_COUNT, sizeof(buf));
  if (hpcPoePrintfQueueID == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_POE_COMPONENT_ID,
            "%s: Failed to create queue, exiting....", __FUNCTION__);
    return(L7_ERROR);
  }

  osapiTaskInitDone(L7_POE_TASK_SYNC);

  while (L7_TRUE != done)
  {
    do
    {
      status = osapiMessageReceive(hpcPoePrintfQueueID, buf, sizeof(buf), L7_WAIT_FOREVER);
      if (status == L7_SUCCESS)
      {
        printf("%s", buf);
        osapiSleepMSec(5);
      }
    } while (status != L7_ERROR);
  }

  return(L7_SUCCESS);
}

/* This is a debug routine that allows user to buffer
 * the hpcPoe prints in the message Q and print cleanly later
 */
void hpcPoePrintfTaskStart(void)
{
  if (hpcPoePrintfQueueID != L7_NULL)
  {
    return;
  }

  hpcPoePrintfTaskID = osapiTaskCreate("hpcPoePrintf",
                                       hpcPoePrintfMsgQueueTask,
                                       0,
                                       L7_NULLPTR,
                                       L7_DEFAULT_STACK_SIZE,
                                       L7_DEFAULT_TASK_PRIORITY - 5,
                                       L7_DEFAULT_TASK_SLICE);

  osapiWaitForTaskInit(L7_POE_TASK_SYNC, L7_WAIT_FOREVER);

  return;
}

/* non zero value will buffer the debug prints
 * into the hpcPoe message Q and cleanly displays on console
 */
int hpcPoeBufferInQEnable = 1; /* By Default print all to queue */

/* This is used only for debugging */
void hpcPoeDebugBuffer(int val)
{
  printf("Help:\n");
  printf("\t 0     - stop printing the hpcPoePrints on the console\n");
  printf("\t 1     - buffers the hpcPoePrints output into a msgQ\n");
  printf("\t         and cleanly displays it without overwriting\n");
  printf("\t other - prints the hpcPoePrints on the console directly\n");

  hpcPoeBufferInQEnable = val;

  if (hpcPoeBufferInQEnable == 1)
  {
    hpcPoePrintfTaskStart();
  }

  return;
}

/**************************************************************************
*
* @purpose  Enqueues a string on the Printf queue.
*
* @param    buf  String to enqueue
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void hpcPoePrintfMsgEnqueue(L7_uchar8 *buf)
{
  if (hpcPoePrintfQueueID != L7_NULLPTR)
  {
    if (osapiMessageSend(hpcPoePrintfQueueID, buf, HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE,
                         L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_POE_COMPONENT_ID,
              "%s: Failed to send....", __FUNCTION__);
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_POE_COMPONENT_ID,
            "%s: hpcPoePrintfMsgQueueTask Not Ready", __FUNCTION__);
  }
  return;
}

/**************************************************************************
*
* @purpose
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*************************************************************************/
void hpcPoePrintf(const L7_char8 *format, ...)
{
  L7_uchar8 buffer[HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE*2];
  va_list   arg;

  if (hpcPoeBufferInQEnable == 0)
  {
    return;
  }

  va_start(arg, format);

  if (osapiIntContext() == L7_TRUE || hpcPoeBufferInQEnable == 1)
  {
    if (vsprintf(buffer, format, arg) > (HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE - 1))
    {
      buffer[HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE-1] = 0;
    }
    hpcPoePrintfMsgEnqueue(buffer);
  }
  else
  {
    vprintf(format, arg);
  }

  va_end(arg);

  return;
}
#endif  /* HPC_POE_PWRDSNE_DEBUG */

#undef L7_LOGF
#undef POE_HPC_DPRINTF
#undef POE_HPC_DPRINTLINE

#ifdef HPC_POE_PWRDSNE_DEBUG
#define L7_LOGF(sev, comp, format, ...)  hpcPoePrintf("\r\n" format "\r\n", ## __VA_ARGS__)
#define POE_HPC_DPRINTF(format, ...)     hpcPoePrintf("\r\n%s -- " format "\r\n", __FUNCTION__, ## __VA_ARGS__)
#define POE_HPC_DPRINTLINE(format, ...)  hpcPoePrintf(format, ## __VA_ARGS__)
#define hpcPoeDebugMsgDump(buffer, size, tx)            \
do                                                      \
{                                                       \
  L7_uint32 i;                                          \
                                                        \
  taskLock();                                           \
                                                        \
  hpcPoePrintf("\r\n");                                 \
  if (tx == L7_TRUE)                                    \
  {                                                     \
    hpcPoePrintf("%s -- sending:\r\n", __FUNCTION__);   \
  }                                                     \
  else                                                  \
  {                                                     \
    hpcPoePrintf("%s -- receiving:\r\n", __FUNCTION__); \
  }                                                     \
                                                        \
  for (i = 0; i < size; i++)                            \
  {                                                     \
    hpcPoePrintf("%2.2x ", buffer[i]);                  \
  }                                                     \
                                                        \
  hpcPoePrintf("\r\n");                                 \
                                                        \
  taskUnlock();                                         \
} while (0)

#else

#define hpcPoePrintf printf

#define L7_LOGF(sev, comp, format, ...)  taskLock();fprintf(stdout, "\r\n" format "\r\n", ## __VA_ARGS__);fflush(stdout);taskUnlock()

#define POE_HPC_DPRINTF(format, ...)    taskLock();                     \
                                        fprintf(stdout, "\r\n%s -- " format "\r\n", __FUNCTION__, ## __VA_ARGS__); \
                                        fflush(stdout);                 \
                                        taskUnlock()

#define POE_HPC_DPRINTLINE(format, ...) taskLock();fprintf(stdout, format, ## __VA_ARGS__);taskUnlock()

#define hpcPoeDebugMsgDump(buffer, size, tx)                \
do                                                          \
{                                                           \
  L7_uint32 i;                                              \
                                                            \
  taskLock();                                               \
                                                            \
  fprintf(stdout, "\r\n");                                  \
  if (tx == L7_TRUE)                                        \
  {                                                         \
    fprintf(stdout, "%s -- sending:\r\n", __FUNCTION__);    \
  }                                                         \
  else                                                      \
  {                                                         \
    fprintf(stdout, "%s -- receiving:\r\n", __FUNCTION__);  \
  }                                                         \
                                                            \
  for (i = 0; i < size; i++)                                \
  {                                                         \
    fprintf(stdout, "%2.2x ", buffer[i]);                   \
  }                                                         \
                                                            \
  fprintf(stdout, "\r\n");                                  \
  fflush(stdout);                                           \
                                                            \
  taskUnlock();                                             \
} while (0)

#endif  /* HPC_POE_PWRDSNE_DEBUG */

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
static L7_RC_t hpcPwrdsinePoeGetAccess(L7_int32 timeout)
{
  L7_RC_t rc;

  /* might be being used elsewhere */
  rc  = osapiSemaTake(poe_access_sem, timeout);

  if (poeMonitorDebugEnabled)
  {
    if (rc == L7_FAILURE)
    {
      POE_HPC_DPRINTF("#", rc);
    }
    else
    {
      POE_HPC_DPRINTF("$", rc);
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
static L7_RC_t hpcPwrdsinePoeReleaseAccess(void)
{
  if (poeMonitorDebugEnabled)
  {
    POE_HPC_DPRINTF("*");
  }

  return osapiSemaGive(poe_access_sem);
}

/*********************************************************************
 *
 * @purpose Adds Checksum to a POE message buffer before sending it to the
 *     POE controller
 *
 * @param   L7_uchar   *buf - POE buffer to which Checksum needs to be added
 *
 * returns  none
 *
 ********************************************************************/
static void hpcPwrdsinePoeChecksumInsert(L7_uchar8 *buf)
{
  L7_uint32   i;
  L7_ushort16 checksum = 0;

  for (i = 0; i < 13; i++)
  {
    checksum += buf[i];
  }

  buf[13] = checksum >> 8;
  buf[14] = checksum & 0xff;

  return;
}

/*********************************************************************
 *
 * @purpose Verifies Checksum on a message received from
 *     POE controller
 *
 * @param   L7_uchar   *buf - POE buffer whose Checksum needs to be verified
 *
 * returns  L7_RC_t result
 *
 ********************************************************************/
static L7_RC_t hpcPwrdsinePoeChecksumVerify(L7_uchar8 *buf)
{
  L7_uint32   i;
  L7_ushort16 checksum = 0;

  for (i = 0; i < 13; i++)
  {
    checksum += buf[i];
  }

  if (buf[13] != (checksum >> 8))
  {
    poe_bad_checksum_count++;
    return L7_FAILURE;
  }
  if (buf[14] != (checksum & 0xff))
  {
    poe_bad_checksum_count++;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

#if 0
/*********************************************************************
 *
 * @purpose Converts a PoE physical port number to a device number
 *
 * @param   L7_uchar8  phyPort   The physical port number
 * @param   L7_uchar8  device    The number of the device powering a port
 *
 * returns  L7_RC_t result
 *
 ********************************************************************/
static L7_RC_t hpcPwrdsinePhyPortToDeviceNumGet(L7_uchar8 phyPort, L7_uchar8 *device)
{
}
#endif

/*********************************************************************
 *
 * @purpose Transmit PoE message to the PoE controller
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *tx_buf,        Message to be transmitted.
 *          L7_uchar8   *rx_buf,        Message Received.
 *          L7_uint32   timeout         Time to wait for a response.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeTransReceiveMsg(L7_uint32 cardIndex, L7_uchar8 *tx_buf, L7_uchar8 *rx_buf, L7_uint32 tx_length, L7_uint32 timeout)
{
  L7_RC_t      result = L7_SUCCESS;
  L7_int32     rc;
  L7_uint32    retry_count;
  L7_ushort16  report_bytes;
  L7_uint32    prev, diff_time;
  HPC_POE_CONTROLLER_MESSAGE_t message;

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

  if (tx_length > 0)
  {
    /* Flush message queue */
    while (L7_SUCCESS == osapiMessageReceive(poe_rx_msgq[cardIndex], &message, sizeof(message), L7_NO_WAIT))
    {
      if (poeDebugEnabled)
      {
        hpcPoeDebugMsgDump(rx_buf, message.size, L7_FALSE);
        POE_HPC_DPRINTF("Flushed previous message.");
      }
    }
  }

  prev = osapiTimeMillisecondsGet();

  for (retry_count = 0; retry_count < POE_CONTROLLER_WRITE_RETRIES; retry_count++)
  {
    if (tx_length > 0)
    {
      if ((POE_CODE_COMMAND == tx_buf[POE_KEY]) ||
          (POE_CODE_PROGRAM == tx_buf[POE_KEY]) ||
          (POE_CODE_REQUEST == tx_buf[POE_KEY]))
      {
        tx_buf[POE_ECHO] = echo_num++;

        hpcPwrdsinePoeChecksumInsert(tx_buf);
      }

      if (poeDebugEnabled)
      {
        hpcPoeDebugMsgDump(tx_buf, tx_length, L7_TRUE);
      }

      rc = poe_device_write(cardIndex, tx_buf, tx_length);
      if (rc < 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Could not write to PoE Firmware Controller!");
        result = L7_FAILURE;
      }
    }

    result = osapiMessageReceive(poe_rx_msgq[cardIndex], &message, sizeof(message), timeout);
    if (result == L7_SUCCESS)
    {
      diff_time = osapiTimeMillisecondsGet() - prev;
      if (poeDebugEnabled)
      {
        POE_HPC_DPRINTF("The time diff is %d.", diff_time);
      }
      break;
    }
    poe_tx_retry_count++;
  }

  if (result != L7_SUCCESS)
  {
    poe_tx_no_response_count++;

    /*
     * need to add additional communication recovery logic here.
     * See Serial Communication Protocol User Guide for example of recovery protocol.
     */
    if (L7_SUCCESS != osapiSemaGive(poe_tx_sem))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not give Poe TX semaphore", __FUNCTION__);
    }
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POE_COMPONENT_ID, "No response from PoE Controller!");
    return result;
  }

  memcpy(rx_buf, message.buf, POE_CONTROLLER_MSG_SIZE);

  if (poeDebugEnabled)
  {
    hpcPoeDebugMsgDump(rx_buf, message.size, L7_FALSE);
  }

  /*
   * The following checks should only be done for "normal" messages.  I.e., not
   * for download messages which are less than full length.
   */
  if (POE_CONTROLLER_MSG_SIZE == message.size)
  {
    /* verify echo and checksum on response */
    if (((0 == tx_length) ||
         (tx_buf[POE_ECHO] != rx_buf[POE_ECHO])) &&
        (0xFF             != rx_buf[POE_ECHO]))
    {
      poe_bad_echo_count++;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POE_COMPONENT_ID, "Bad Echo from PoE Controller!");
      result = L7_FAILURE;
    }
    else if (hpcPwrdsinePoeChecksumVerify(rx_buf) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Bad Checksum from PoE Controller!");
      result = L7_FAILURE;
    }
    else
    {
      if (rx_buf[POE_KEY] == POE_CODE_REPORT)
      {
        report_bytes = (rx_buf[2] << 8) | rx_buf[3];
        if (report_bytes == 0)
        {
          result = L7_SUCCESS;
        }
        else
        {
          result = L7_FAILURE;
          if (report_bytes == 0xFFFF)
          {
            if ((rx_buf[4]    == 0xFF) &&
                (rx_buf[5]    == 0xFF))
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Command Received/Wrong Checksum", __FUNCTION__);
            }
            else
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Failed Execution/Undefined KEY Value", __FUNCTION__);
            }
          }
          else if ((report_bytes >= 0x7000) &&
                   (report_bytes <= 0x7FFF))
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Failed Execution/Conflict in Subject Bytes", __FUNCTION__);
          }
          else if ((report_bytes >= 0x8000) &&
                   (report_bytes <= 0x8FFF))
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Failed Execution/Wrong Data Byte Value", __FUNCTION__);
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Failed Execution/Unknown Error Code 0x%x", __FUNCTION__, report_bytes);
          }
          if (tx_length > 0)
          {
            L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "Command follows:");
            L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x ", tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4], tx_buf[5], tx_buf[6], tx_buf[7], tx_buf[8], tx_buf[9], tx_buf[10], tx_buf[11], tx_buf[12], tx_buf[13], tx_buf[14]);
          }
        }
      }
    }
  }

  if (L7_SUCCESS != osapiSemaGive(poe_tx_sem))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not give Poe TX semaphore", __FUNCTION__);
  }

  return result;
}

/*********************************************************************
* @purpose   Platform specific function to decide if the status obtained
*            from PoE controller should be deemed okay for Delivering Power
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t hpcPwrdsineHwDeliveringPower(L7_uchar8 status)
{
  if ((status == 0x00) || (status == 0x01))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose   Platform specific function to decide if the status obtained
*            from PoE controller should be deemed okay for Detecting state
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t hpcPwrdsineHwDetecting(L7_uchar8 status)
{
  if (status == 0x26 || status == 0x1B)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose   Platform specific function to decide if the status obtained
*            from PoE controller should be deemed okay for Overload State
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t hpcPwrdsineHwOverload(L7_uchar8 status)
{
  if (status == 0x1F || status == 0x34 || status == 0x24)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose   Platform specific function to decide if the status obtained
*            from PoE controller should be deemed okay for Exceeding power Budget state
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t hpcPwrdsineHwExceedPowerBudget(L7_uchar8 status)
{
  if (status == 0x20)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 *********************************************************************
 *
 * Beginning of controller message functions.
 *
 *********************************************************************
 *********************************************************************/

#if (PD_SYSTEM == PD64008)
/*********************************************************************
 *
 * @purpose Get Power Source 1 settings.
 *          Only Applicable for PD64008
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_ushort16 *max_shutdown_voltage,  Max Shutdown Voltage at which Overload is triggered.
 *          L7_ushort16 *min_shutdown_voltage,  Min Shutdown Voltage at which Underload is triggered.
 *          L7_ucshort16    *max_power_available,   Maximum Power Available on the Power Bank.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerSource1Get(L7_uint32 cardIndex, L7_ushort16 *max_shutdown_voltage,
                                                L7_ushort16 *min_shutdown_voltage, L7_ushort16 *max_power_available)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_SUPPLY1, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *max_shutdown_voltage = ((L7_ushort16)(rx_buf[4]) << 8) | (rx_buf[5]);
    *min_shutdown_voltage = ((L7_ushort16)(rx_buf[6]) << 8) | (rx_buf[7]);
    *max_power_available  = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Source 2 settings.
 *          Only Applicable for PD64008
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_ushort16 *max_shutdown_voltage,  Max Shutdown Voltage at which Overload is triggered.
 *          L7_ushort16 *min_shutdown_voltage,  Min Shutdown Voltage at which Underload is triggered.
 *          L7_ucshort16    *max_power_available,   Maximum Power Available on the Power Bank.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerSource2Get(L7_uint32 cardIndex, L7_ushort16 *max_shutdown_voltage,
                                                L7_ushort16 *min_shutdown_voltage, L7_ushort16 *max_power_available)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_SUPPLY2, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *max_shutdown_voltage = ((L7_ushort16)(rx_buf[4]) << 8) | (rx_buf[5]);
    *min_shutdown_voltage = ((L7_ushort16)(rx_buf[6]) << 8) | (rx_buf[7]);
    *max_power_available  = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Set the Max Power for Power Source 1.
 *          Only Applicable to PD64008 Controller
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_ushort16 power,          Max Power.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerSource1Set(L7_uint32 cardIndex, L7_ushort16 power)
{
  L7_RC_t     rc;
  L7_uchar8   buf[POE_CONTROLLER_MSG_SIZE + 1]     = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_SUPPLY1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, POE_CODE_POWERGUARDBAND, POE_CODE_N};
  L7_uchar8   rx_buf[POE_CONTROLLER_MSG_SIZE + 1];
  L7_ushort16 min_voltage = (POE_MIN_VOLTAGE - 1);
  L7_ushort16 max_voltage = (POE_MAX_VOLTAGE - 1);

  buf[5] = power >> 8;
  buf[6] = power & 0x00ff;
  buf[7] = max_voltage >> 8;
  buf[8] = max_voltage & 0x00ff;
  buf[9] = min_voltage >> 8;
  buf[10]= min_voltage & 0x00ff;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set the Max Power for Power Source 2.
 *          Only Applicable to PD64008 Controller
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_ushort16 power,          Max Power
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerSource2Set(L7_uint32 cardIndex, L7_ushort16 power)
{
  L7_RC_t     rc;
  L7_uchar8   buf[POE_CONTROLLER_MSG_SIZE + 1]     = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_SUPPLY2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, POE_CODE_POWERGUARDBAND, POE_CODE_N};
  L7_uchar8   rx_buf[POE_CONTROLLER_MSG_SIZE + 1];
  L7_ushort16 min_voltage = (POE_MIN_VOLTAGE - 1);
  L7_ushort16 max_voltage = (POE_MAX_VOLTAGE - 1);

  buf[5] = power >> 8;
  buf[6] = power & 0x00ff;
  buf[7] = max_voltage >> 8;
  buf[8] = max_voltage & 0x00ff;
  buf[9] = min_voltage >> 8;
  buf[10]= min_voltage & 0x00ff;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}
#endif  /* (PD_SYSTEM == PD64008) */

#if (PD_SYSTEM == PD64012)
/*********************************************************************
 *
 * @purpose Set Power Management Method.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPMMethodGet(L7_uint32 cardIndex, L7_POE_POWER_MGMT_t *mode)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_POWERMANAGE_MODE, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Did not receive response to PM Mode request.");
    return rc;
  }

  if (POE_CODE_TELEMETRY != rx_buf[POE_KEY])
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Did not receive telemetry response to PM Mode request.");
    return L7_FAILURE;
  }

  if (0x00 == rx_buf[3])
  {
    *mode = L7_POE_POWER_MGMT_STATIC;
  }
  else if (0x00 == rx_buf[2])
  {
    *mode = L7_POE_POWER_MGMT_DYNAMIC;
  }
  else
  {
    *mode = L7_POE_POWER_MGMT_CLASS;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Set Power Management Method.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPMMethodSet(L7_uint32 cardIndex, L7_POE_POWER_MGMT_t mode)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_POWERMANAGE_MODE, 0x00, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  switch (mode)
  {
    case L7_POE_POWER_MGMT_DYNAMIC:
      buf[6] = 0x02;
      break;
    case L7_POE_POWER_MGMT_CLASS:
      buf[5] = 0x04;
      buf[6] = 0x01;
      break;
    case L7_POE_POWER_MGMT_STATIC:
    default:
      /* all other values use predefined default */
      break;
  };

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Save System Config for the PoE controller.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSaveSystemConfig(L7_uint32 cardIndex)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_PROGRAM, 0x00, POE_CODE_E2, POE_CODE_SAVECONFIG, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set Max Power/Threshold on a Power Bank
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerBanksSet(L7_uint32 cardIndex, L7_uint32 power, L7_uchar8 bank, L7_uint32 threshold)
{
  L7_RC_t     rc;
  L7_uchar8   buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_POWERBUDGET, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, POE_CODE_POWERGUARDBAND};
  L7_uchar8   rx_buf[POE_CONTROLLER_MSG_SIZE + 1];
  L7_ushort16 min_voltage = POE_MIN_VOLTAGE;
  L7_ushort16 max_voltage = POE_MAX_VOLTAGE;
  L7_ushort16 PowerLimit  = 0;

  if (power > 0)
  {
    PowerLimit = (L7_ushort16)((power * threshold) / 100);
  }

  buf[5]  = bank;
  buf[6]  = PowerLimit >> 8;
  buf[7]  = PowerLimit & 0x00ff;
  buf[8]  = max_voltage >> 8;
  buf[9]  = max_voltage & 0x00ff;
  buf[10] = min_voltage >> 8;
  buf[11] = min_voltage & 0x00ff;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}
#endif  /* (PD_SYSTEM == PD64012) */

/*********************************************************************
 *
 * @purpose Reset the PoE Controller.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t hpcPwrdsinePoeMsgReset(L7_uint32 cardIndex, L7_BOOL *fwProgRequired, L7_uint32 timeout)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_RESET, 0x00, POE_CODE_RESET, 0x00, POE_CODE_RESET, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  if (L7_FALSE == hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_FAILURE;
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "No response to RESET command");
  }
  else if (POE_CODE_REPORT != rx_buf[POE_KEY])
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "No report response to RESET command");
  }

  /* Now wait for the system telemetry */
  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, 0, timeout);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "No system telemetry response to RESET command");
    return L7_FAILURE;
  }

  /* This should be a System Status Telemetry message. */
  if ((rx_buf[POE_KEY] == POE_CODE_TELEMETRY) && (rx_buf[POE_ECHO] == 0xFF))
  {
    if (rx_buf[2] & POE_PROG_REQD)      /* Controller does not have valid firmware */
    {
      *fwProgRequired = L7_TRUE;
    }
    else
    {
      *fwProgRequired = L7_FALSE;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "RESET command response was invalid.");
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Enable/Disable a PoE port.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port.
 *          L7_uchar8   enable,         Enable/Disable Admin mode.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgChannelOnOffSet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 enable)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, POE_CODE_ONOFF, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;
  buf[5] = enable;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set Power Limit on a PoE Port.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port
 *          L7_ushort16 power_limit,    Power Limit
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgChannelPowerLimitSet(L7_uint32 cardIndex, L7_uchar8 channel, L7_ushort16 power_limit)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, POE_CODE_SUPPLY, 0x00, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;
  buf[5] = power_limit >> 8;
  buf[6] = power_limit & 0x00ff;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set Priority on a PoE Port.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port
 *          L7_ushort16 priority,       Priority
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t hpcPwrdsinePoeMsgChannelPrioritySet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 priority)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, POE_CODE_PRIORITY, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;
  buf[5] = priority;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set PoE Port Parameters
 *
 * @param   L7_uint32   cardIndex,      The Card Index of the PoE Controller
 *          L7_uchar8   channel,        PoE Port
 *          L7_ushort16 priority,       Priority
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgChannelParmsSet(L7_uint32 cardIndex, L7_uchar8 channel,
                                                L7_uchar8 enable, L7_ushort16 power_limit, L7_uchar8 priority)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, POE_CODE_PORTFULLINIT, 0x00, 0x00, 0x00, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;
  buf[5] = enable;
  buf[6] = power_limit >> 8;
  buf[7] = power_limit & 0x00ff;
  buf[8] = priority;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Set System Mask.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   mask,           Mask
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSystemMasksSet(L7_uint32 cardIndex, L7_uchar8 mask)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_MASKZ, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = mask;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Get System Status.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *boot_error,    Indicate Boot Error Code
 *          L7_uchar8   *kernel_error,  Indicate Kernel Error Code
 *          L7_uchar8   *internal_error,    Indicate Internal Error Code
 *          L7_uchar8   *private_label, Indicate Private Label
 *          L7_uchar8   *user_byte,     User Byte
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSystemStatusGet(L7_uint32 cardIndex,
                                                L7_uchar8 *boot_error,
                                                L7_uchar8 *kernel_error,
                                                L7_uchar8 *kernel_status,
                                                L7_uchar8 *internal_error,
                                                L7_uchar8 *private_label,
                                                L7_uchar8 *user_byte,
                                                L7_uchar8 *deviceFail,
                                                L7_uchar8 *tempDisconnect,
                                                L7_uchar8 *tempAlarm)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SYSTEMSTATUS, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *boot_error     = rx_buf[2];
    *kernel_error   = rx_buf[3];
    *kernel_status  = rx_buf[4];
    *internal_error = rx_buf[5];
    *private_label  = rx_buf[6];
    *user_byte      = rx_buf[7];
    *deviceFail     = rx_buf[8];
    *tempDisconnect = rx_buf[9];
    *tempAlarm      = rx_buf[10];
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Single Port PoE status.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port
 *          L7_uchar8   *on_off,        Indicate Admin Mode
 *          L7_uchar8   *status,        Indicate PoE Port status (providing power, overload etc)
 *          L7_uchar8   *latch,         Indicate Event latch
 *          L7_uchar8   *power_class,   Indicate Power Class of the Device (PD Class 1, 2 etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSinglePortStatusGet(L7_uint32 cardIndex,
                                                    L7_uchar8 channel,
                                                    L7_uchar8 *on_off,
                                                    L7_uchar8 *status,
                                                    L7_uchar8 *auto_test,
                                                    L7_uchar8 *latch,
                                                    L7_uchar8 *power_class)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_PORTSTATUS, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *on_off = rx_buf[2];
    *status = rx_buf[3];
    *auto_test = rx_buf[4];
    *latch = rx_buf[5];
    *power_class = rx_buf[6];
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get System Mask Status.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *mask,          System Mask
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgMasksStatusGet(L7_uint32 cardIndex, L7_uchar8 *mask)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, 0x2b, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *mask = rx_buf[2];
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Bank Settings.
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_ushort16 *power_consumption,     Total Power Consumed
 *          L7_ushort16 *max_shutdown_voltage,  Max Shutdown Voltage at which Overload is triggered.
 *          L7_ushort16 *min_shutdown_voltage,  Min Shutdown Voltage at which Underload is triggered.
 *          L7_uchar8   *powerBank,             Active Power Bank being used
 *          L7_ucshort16    *max_power_available,   Maximum Power Available on the Power Bank.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgMainPowerSourceSupplyParmGet(L7_uint32   cardIndex,
                                                             L7_ushort16 *power_consumption,
                                                             L7_ushort16 *max_shutdown_voltage,
                                                             L7_ushort16 *min_shutdown_voltage,
                                                             L7_uchar8   *powerBank,
                                                             L7_ushort16 *max_power_available)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_MAIN, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *power_consumption    = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
    *max_shutdown_voltage = ((L7_ushort16)(rx_buf[4]) << 8) | (rx_buf[5]);
    *min_shutdown_voltage = ((L7_ushort16)(rx_buf[6]) << 8) | (rx_buf[7]);
    *powerBank            = rx_buf[9];
    *max_power_available  = ((L7_ushort16)(rx_buf[10]) << 8) | (rx_buf[11]);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Main Voltage.
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_ushort16 *vmain_volatge,         Main Voltage.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgMainSupplyVoltageGet(L7_uint32 cardIndex, L7_ushort16 *vmain_voltage)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_MEASUREMENTZ, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *vmain_voltage = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Consumed per port
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_uchar8   channel,                PoE Port.
 *          L7_ushort16 *power,                 Power Consumed.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPortPowerSettingsGet(L7_uint32 cardIndex, L7_uchar8 channel, L7_ushort16 *power)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_SUPPLY, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *power = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Parameters per Port
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_uchar8   channel,                PoE Port
 *          L7_ushort16 *voltage,               Voltage across the PoE port.
 *          L7_ushort16 *current,               Current Supplied through the PoE port.
 *          L7_ushort16 *power,                 Power Consumed.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPortParmMeasurementGet(L7_uint32    cardIndex,
                                                       L7_uchar8    channel,
                                                       L7_ushort16 *voltage,
                                                       L7_ushort16 *current,
                                                       L7_ushort16 *power)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_PARAMZ, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *voltage = ((L7_ushort16)(rx_buf[2]) << 8) | (rx_buf[3]);
    *current = ((L7_ushort16)(rx_buf[4]) << 8) | (rx_buf[5]);
    *power   = ((L7_ushort16)(rx_buf[6]) << 8) | (rx_buf[7]);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Parameters of the PoE controller
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_uchar8   *num_channel,           Num Of Ports supported on the PoE controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSerialNumberGet(L7_uint32    cardIndex,
                                                L7_uchar8   *num_channels,
                                                L7_uchar8   *subcontractor,
                                                L7_uchar8   *year,
                                                L7_uchar8   *week,
                                                L7_ushort16 *part_num,
                                                L7_uint32   *serial_num)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_PRODUCTINFOZ, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *num_channels  = rx_buf[2];
    *subcontractor = rx_buf[3];
    *year          = rx_buf[4];
    *week          = rx_buf[5];
    *part_num      = ((L7_ushort16)(rx_buf[6]) << 8) | (rx_buf[7]);
    *serial_num    = ((L7_ushort16)(rx_buf[8]) << 16) | ((L7_ushort16)(rx_buf[9]) << 8) | (rx_buf[10]);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Software Version of the PoE controller
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgSoftwareVersionGet(L7_uint32    cardIndex,
                                                   L7_uchar8   *hw_ver,
                                                   L7_ushort16 *sw_ver,
                                                   L7_uchar8   *build_num)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_VERSIONZ, POE_CODE_SOFTWAREVERSION, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *hw_ver    = rx_buf[2];
    *sw_ver    = ((L7_ushort16)(rx_buf[5]) << 8) | (rx_buf[6]);
    *build_num = rx_buf[7];
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Device Version of the PoE Controller
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgDeviceStatusGet(L7_uint32    cardIndex,     L7_uchar8  CSNum,
                                                L7_ushort16 *deviceVersion,  L7_uchar8 *asicStatus,
                                                L7_uchar8   *portsExpected,  L7_uchar8 *portsVerified,
                                                L7_uchar8   *portsAllocated, L7_uchar8 *temperature,
                                                L7_uchar8   *tempSwitchHigh, L7_uchar8 *midPowerData,
                                                L7_uchar8   *commStatus)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_DEVICE_PARAMS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = CSNum;
  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    if (0xFF == rx_buf[9])              /* device does not exist */
    {
      rc = L7_NOT_EXIST;
    }
    else
    {
      *deviceVersion  = (L7_ushort16)((rx_buf[3] << 8) | rx_buf[4]);
      *asicStatus     = rx_buf[5];
      *portsExpected  = rx_buf[6];
      *portsVerified  = rx_buf[7];
      *portsAllocated = rx_buf[8];
      *temperature    = rx_buf[9];
      *tempSwitchHigh = rx_buf[10];
      *midPowerData   = rx_buf[11];
      *commStatus     = rx_buf[12];
    }
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get UDL Counters
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgUDLCountersGet(L7_uint32 cardIndex, L7_uchar8 *ChnlCtrArray, L7_int32 CounterNum)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  if (CounterNum == 1)
  {
    buf[3] = POE_CODE_UDLCOUNTER;
  }
  if (CounterNum == 2)
  {
    buf[3] = POE_CODE_UDLCOUNTER2;
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    memcpy(ChnlCtrArray, &rx_buf[2], 6);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Get Enable/Disable Mode of the PoE ports
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPortStatusInfoGet(L7_uint32 cardIndex, L7_uint32 *on_off_status_24port, L7_uint32 *on_off_status_48port)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_ONOFF, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *on_off_status_24port = (((L7_uint32)(rx_buf[2]) << 24) |
                             ((L7_uint32)(rx_buf[3]) << 16) |
                             ((L7_uint32)(rx_buf[4]) << 8)  |
                             (L7_uint32)(rx_buf[5]));

    *on_off_status_48port = (((L7_uint32)(rx_buf[6]) << 24) |
                             ((L7_uint32)(rx_buf[7]) << 16) |
                             ((L7_uint32)(rx_buf[8]) << 8)  |
                             (L7_uint32)(rx_buf[9]));
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Priority of the PoE port
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPortPriorityGet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 *priority)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_PRIORITY, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *priority = rx_buf[2];
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Bank Settings
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *          L7_uchar8   *num_channel,           Num Of Ports supported on the PoE controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPowerBanksGet(L7_uint32 cardIndex, L7_uchar8 bank, L7_ushort16 *PowerBudget,
                                              L7_ushort16 *MaxShutdownVoltage, L7_ushort16 *MinShutdownVoltage)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_SUPPLY, POE_CODE_POWERBUDGET, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  *PowerBudget = *MaxShutdownVoltage = *MinShutdownVoltage = 0;
  buf[5] = bank;
  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *PowerBudget        = (rx_buf[2] << 8) | rx_buf[3];
    *MaxShutdownVoltage = (rx_buf[4] << 8) | rx_buf[5];
    *MinShutdownVoltage = (rx_buf[6] << 8) | rx_buf[7];
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Initialize PoE LED Code
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeLedInit(L7_uint32 cardIndex)
{
  /* PoE LED Initialization is very hardware specific. If this platform requires LED Init,
   * please do so in the following function call, defined in platform specific directory */
  hpcPlatformHwPoeLedInit(cardIndex);
}

/*********************************************************************
* @purpose   Platform specific function to check Legacy Mode
*
* @param void
*
* @returns L7_FALSE     If Legacy mode is disabled
*          L7_TRUE      If Legacy mode is enabled
*
* @notes none
*
* @end
*
*********************************************************************/
static BOOL hpcPwrdsineCheckLegacyMode(L7_uint32 cardId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 mask;

  rc = hpcPwrdsinePoeMsgMasksStatusGet(cardId, &mask);
  if (rc == L7_SUCCESS)
  {
    if (mask & 0x02)
    {
        return L7_TRUE;     /* Capacitor detection is ENABLED */
    }
    else
    {
        return L7_FALSE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
 *
 * @purpose Get Counters from the Port Status Cache
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeSinglePortStatusCache(L7_uchar8 channel,
                                                   L7_uchar8 on_off,
                                                   L7_uchar8 status,
                                                   L7_uchar8 auto_test,
                                                   L7_uchar8 latch,
                                                   L7_uchar8 power_class)
{
  L7_uint32 underload_counter = 0;

  switch (status)
  {
    case 8:
    case 26:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_DISABLED;
      break;
    case 27:
    case 30:
    case 31:
    case 37:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_SEARCHING;
      break;
    case 0:
    case 1:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER;
      break;
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_FAULT;
      break;
    case 43:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_TEST;
      break;
    case 6:
    case 7:
    case 12:
    case 17:
    case 18:
    case 28:
    case 29:
    case 32:
    case 33:
    case 36:
    case 38:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT;
      break;
    default:
      poeGlobalStatus_g.poePortStatus[channel].detectionStatus = L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT;
      break;
  }

  if (L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER == poeGlobalStatus_g.poePortStatus[channel].detectionStatus)
  {
    switch (power_class)
    {
      case 0:
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS0;
        break;
      case 1:
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS1;
        break;
      case 2:
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS2;
        break;
      case 3:
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS3;
        break;
      case 4:
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS4;
        break;
      default:
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unknown powerClass %d for POE channel %d", power_class, channel);
        poeGlobalStatus_g.poePortStatus[channel].powerClass      = L7_POE_PORT_POWER_CLASS0;
        break;
    }
  }
  else                                  /* not delivering power so invalid class */
  {
    poeGlobalStatus_g.poePortStatus[channel].powerClass = L7_POE_PORT_POWER_INVALID;
  }

  switch (status)
  {
    case 28:
    case 36:
    case 37:
    case 38:
      poeGlobalStatus_g.poePortStatistics[channel].invalidSignature++;
      break;

    case 6:
    case 7:
    case 32:
    case 45:
    case 46:
    case 53:
    case 54:
    case 57:
    case 58:
    case 60:
    case 61:
    case 62:
    case 63:
      poeGlobalStatus_g.poePortStatistics[channel].powerDenied++;
      break;

    default:
      break;
  }
  if (latch & 0x20)
  {
    poeGlobalStatus_g.poePortStatistics[channel].shortCounter++;
  }


  if (latch & 0x02)
  {
    poeGlobalStatus_g.poePortStatistics[channel].overLoad++;
  }

  underload_counter = ((latch >> 3) & 0x03);

  if (underload_counter)
  {
    poeGlobalStatus_g.poePortStatistics[channel].mpsAbsent += underload_counter;
  }

  return L7_SUCCESS;
}

static L7_RC_t hpcPwrdsinePoePortParmMeasurementCache(L7_uchar8   channel,
                                                      L7_ushort16 voltage,
                                                      L7_ushort16 current,
                                                      L7_ushort16 power)
{
  poeGlobalStatus_g.poePortStatus[channel].powerConsumed = power;
  poeGlobalStatus_g.poePortStatus[channel].voltage       = voltage / 10; /* convert deciVolts to Volts  */
  poeGlobalStatus_g.poePortStatus[channel].current       = current;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set Port mapping.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgGlobalMatrixProgram(L7_uint32 cardIndex)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_GLOBAL, POE_CODE_TEMPORARYCHANNELMATRIX, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  if (L7_FALSE == hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_FAILURE;
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc != L7_SUCCESS)
  {
    if ((rx_buf[POE_KEY]  == POE_CODE_TELEMETRY) &&
        (rx_buf[POE_ECHO] == 0xFF))
    {
      /* verify checksum of unsolicited system status msg */
      rc = hpcPwrdsinePoeChecksumVerify(rx_buf);

      if (rx_buf[2] & POE_PROG_REQD)
      {
        return L7_NOT_EXIST;
      }
      return L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Set Port mapping.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port
 *          L7_uchar8,  physical_number Logical PoE Port
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgTempMatrixSet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 physical_number)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, POE_CODE_TEMPORARYCHANNELMATRIX, 0x00, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;
  buf[5] = physical_number;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

static L7_RC_t hpcPwrdsinePoeChannelToPhysicalPortMapCard(L7_uint cardIndex, L7_uint numChannels)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uchar8 channels[numChannels];
  L7_uchar8 ports[numChannels];
  L7_uint   nextPort = 0;
  L7_int    i;
  L7_uchar8 channel;
  L7_uint   index;

#define POE_NOT_USED 0xff

  memset(channels, POE_NOT_USED, numChannels*sizeof(L7_uchar8));
  memset(ports, POE_NOT_USED, numChannels*sizeof(L7_uchar8));

  /*
   * Map the known front panel ports to physical PoE ports.
   */
  for (i = 0; i < numChannels; i++)
  {
    if ((L7_SUCCESS == hpcPoeUnitPhyPortGetFromPoePort(i, &channel)) &&
        (L7_SUCCESS == hpcPoeUnitCardIndexGetFromPhyPoePort(i, &index)) &&
        (index      == cardIndex))
    {
      channels[channel] = i;
      ports[i]          = 0;
    }
  }

  /*
   * Now ensure that each logical port is mapped to a unique physical port.
   * This is necessary so that when the global matrix is programmed, it does
   * not indicate an error.
   */
  for (i = 0; i < numChannels; i++)
  {
    if (POE_NOT_USED == channels[i])
    {
      while (0 == ports[nextPort])
      {
        nextPort++;
      }
      channels[i] = nextPort;
      nextPort++;
    }

    if (L7_SUCCESS != hpcPwrdsinePoeMsgTempMatrixSet(cardIndex, i, channels[i]))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcPoeMsgTempMatrixSet failed for channel %d", i);
    }
  }

  rc = hpcPwrdsinePoeMsgGlobalMatrixProgram(cardIndex);
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to program global matrix for card %u, rc = %d.", cardIndex, rc);
  }

  return rc;
}

L7_RC_t hpcPwrdsinePoeChannelToPhysicalPortMap(void)
{
  L7_RC_t     rc = L7_FAILURE;
  L7_uint     cardIndex;
  L7_uchar8   num_channels;
  L7_uchar8   subcontractor;
  L7_uchar8   year;
  L7_uchar8   week;
  L7_ushort16 part_num;
  L7_uint32   serial_num;

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    rc = hpcPwrdsinePoeMsgSerialNumberGet(cardIndex, &num_channels,
                                          &subcontractor,
                                          &year,
                                          &week,
                                          &part_num,
                                          &serial_num);
    if (L7_SUCCESS != rc)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to retrieve the number of supported channels on card %u.", cardIndex);
      continue;
    }

    rc = hpcPwrdsinePoeChannelToPhysicalPortMapCard(cardIndex, num_channels);
  }

  hpcPwrdsinePoeReleaseAccess();

  return rc;
}

#ifdef POE_FUNCTIONS_NOT_USED                       /* These functions are not used. */

static L7_RC_t hpcPwrdsinePoeMsgPortFromTempMatrixGet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 *physical_number)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_TEMPORARYCHANNELMATRIX, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get physical PoE port from matrix.");
  }
  else
  {
    *physical_number = rx_buf[2];
  }
  return rc;
}

static L7_RC_t hpcPwrdsinePoeMsgPortFromActiveMatrixGet(L7_uint32 cardIndex, L7_uchar8 channel, L7_uchar8 *physical_number)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_CHANNEL, POE_CODE_ACTIVECHANNELMATRIX, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get physical PoE port from matrix.");
  }
  else
  {
    *physical_number = rx_buf[2];
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Get Device Version of the PoE Controller
 *
 * @param   L7_uint32   cardIndex,             The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgDeviceVersionGet(L7_uint32 cardIndex, L7_ushort16 *device0Version,
                                                 L7_ushort16 *device1Version, L7_ushort16 *device2Version,
                                                 L7_ushort16 *device3Version)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_VERSIONZ, POE_CODE_POE_DEVICE_VERSION, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *device0Version = (L7_ushort16)((rx_buf[2] << 8) | rx_buf[3]);
    *device1Version = (L7_ushort16)((rx_buf[4] << 8) | rx_buf[5]);
    *device2Version = (L7_ushort16)((rx_buf[6] << 8) | rx_buf[7]);
    *device3Version = (L7_ushort16)((rx_buf[8] << 8) | rx_buf[9]);
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose Get Power Consumed for ports 22 to 25.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port22Power,   Indicate Power Consumed on the port.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgPorts22To25PowerGet(L7_uint32 cardIndex,
                                                    L7_uchar8 *port22Power,
                                                    L7_uchar8 *port23Power,
                                                    L7_uchar8 *port24Power,
                                                    L7_uchar8 *port25Power,
                                                    L7_ushort16 *Vmain_Volt,
                                                    L7_ushort16 *PowerConsumption,
                                                    L7_ushort16 *MaxPowerAvail)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_PORTSPOWER3, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port22Power      = rx_buf[2];
    *port23Power      = rx_buf[3];
    *port24Power      = rx_buf[10];
    *port25Power      = rx_buf[11];
    *Vmain_Volt       = (rx_buf[4] << 8) | rx_buf[5];
    *PowerConsumption = (rx_buf[6] << 8) | rx_buf[7];
    *MaxPowerAvail    = (rx_buf[8] << 8) | rx_buf[9];
  }
  else
  {
    *port22Power = *port23Power = *port24Power = *port25Power = 0xFF;
    *Vmain_Volt = *PowerConsumption = *MaxPowerAvail = 0;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Restore the PoE Controller to Factory Defaults.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgRestoreFactoryDefaults(L7_uint32 cardIndex)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_PROGRAM, 0x00, POE_CODE_RESTOREFACT, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  if (L7_FALSE == hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_FAILURE;
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc != L7_SUCCESS)
  {
    /* verify checksum of unsolicited system status msg */
    rc = hpcPwrdsinePoeChecksumVerify(rx_buf);
    return rc;
  }

  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Clear PoE Event Latches.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   channel,        PoE Port.
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeMsgChannelEventLatchClear(L7_uint32 cardIndex, L7_uchar8 channel)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_COMMAND, 0x00, POE_CODE_CHANNEL, 0x3a, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  buf[4] = channel;

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
  return rc;
}

/*********************************************************************
 *
 * @purpose Get Status of ports 0 to 10.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port0Status,   Indicate PoE Port status (providing power, overload etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeMsgPorts0To10StatusGet(L7_uint32 cardIndex,
                                                 L7_uchar8 *port0Status,
                                                 L7_uchar8 *port1Status,
                                                 L7_uchar8 *port2Status,
                                                 L7_uchar8 *port3Status,
                                                 L7_uchar8 *port4Status,
                                                 L7_uchar8 *port5Status,
                                                 L7_uchar8 *port6Status,
                                                 L7_uchar8 *port7Status,
                                                 L7_uchar8 *port8Status,
                                                 L7_uchar8 *port9Status,
                                                 L7_uchar8 *port10Status)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, 0x31, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port0Status = rx_buf[2];
    *port1Status = rx_buf[3];
    *port2Status = rx_buf[4];
    *port3Status = rx_buf[5];
    *port4Status = rx_buf[6];
    *port5Status = rx_buf[7];
    *port6Status = rx_buf[8];
    *port7Status = rx_buf[9];
    *port8Status = rx_buf[10];
    *port9Status = rx_buf[11];
    *port10Status  = rx_buf[12];
  }
  else
  {
    *port0Status = *port1Status = *port2Status = *port3Status = 0xFF;
    *port4Status = *port5Status = *port6Status = *port7Status = 0xFF;
    *port8Status = *port9Status = *port10Status = 0xFF;
  }
  return;
}

/*********************************************************************
 *
 * @purpose Get Status of ports 11 to 21.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port11Status,  Indicate PoE Port status (providing power, overload etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeMsgPorts11To21StatusGet(L7_uint32 cardIndex,
                                                  L7_uchar8 *port11Status,
                                                  L7_uchar8 *port12Status,
                                                  L7_uchar8 *port13Status,
                                                  L7_uchar8 *port14Status,
                                                  L7_uchar8 *port15Status,
                                                  L7_uchar8 *port16Status,
                                                  L7_uchar8 *port17Status,
                                                  L7_uchar8 *port18Status,
                                                  L7_uchar8 *port19Status,
                                                  L7_uchar8 *port20Status,
                                                  L7_uchar8 *port21Status)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, 0x32, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port11Status = rx_buf[2];
    *port12Status = rx_buf[3];
    *port13Status = rx_buf[4];
    *port14Status = rx_buf[5];
    *port15Status = rx_buf[6];
    *port16Status = rx_buf[7];
    *port17Status = rx_buf[8];
    *port18Status = rx_buf[9];
    *port19Status = rx_buf[10];
    *port20Status = rx_buf[11];
    *port21Status = rx_buf[12];
  }
  else
  {
    *port11Status = *port12Status = *port13Status = *port14Status = 0xFF;
    *port15Status = *port16Status = *port17Status = *port18Status = 0xFF;
    *port19Status = *port20Status = *port21Status = 0xFF;
  }
  return;
}

/*********************************************************************
 *
 * @purpose Get Status of ports 22 to 25.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port22Status,  Indicate PoE Port status (providing power, overload etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeMsgPorts22To25StatusGet(L7_uint32 cardIndex,
                                                  L7_uchar8 *port22Status,
                                                  L7_uchar8 *port23Status,
                                                  L7_uchar8 *port24Status,
                                                  L7_uchar8 *port25Status)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_PORTSSTATUS3, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port22Status = rx_buf[2];
    *port23Status = rx_buf[3];
    *port24Status     = rx_buf[7];
    *port25Status     = rx_buf[8];
  }
  else
  {
    *port22Status = *port23Status = *port24Status = *port25Status = 0xFF;
  }

  return;
}

/*********************************************************************
 *
 * @purpose Get Status of ports 26 to 36.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port26Status,  Indicate PoE Port status (providing power, overload etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeMsgPorts26To36StatusGet(L7_uint32 cardIndex,
                                                  L7_uchar8 *port26Status,
                                                  L7_uchar8 *port27Status,
                                                  L7_uchar8 *port28Status,
                                                  L7_uchar8 *port29Status,
                                                  L7_uchar8 *port30Status,
                                                  L7_uchar8 *port31Status,
                                                  L7_uchar8 *port32Status,
                                                  L7_uchar8 *port33Status,
                                                  L7_uchar8 *port34Status,
                                                  L7_uchar8 *port35Status,
                                                  L7_uchar8 *port36Status)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_PORTSSTATUS4, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port26Status = rx_buf[2];
    *port27Status = rx_buf[3];
    *port28Status = rx_buf[4];
    *port29Status = rx_buf[5];
    *port30Status = rx_buf[6];
    *port31Status = rx_buf[7];
    *port32Status = rx_buf[8];
    *port33Status = rx_buf[9];
    *port34Status = rx_buf[10];
    *port35Status = rx_buf[11];
    *port36Status = rx_buf[12];
  }
  else
  {
    *port26Status = *port27Status = *port28Status = *port29Status = 0xFF;
    *port30Status = *port31Status = *port32Status = *port33Status = 0xFF;
    *port34Status = *port35Status = *port36Status = 0xFF;
  }
  return;
}

/*********************************************************************
 *
 * @purpose Get Status of ports 37 to 47.
 *
 * @param   L7_uint32   cardIndex,     The Card ID of the PoE Controller
 *          L7_uchar8   *port37Status,  Indicate PoE Port status (providing power, overload etc)
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
static void hpcPwrdsinePoeMsgPorts37To47StatusGet(L7_uint32 cardIndex,
                                                  L7_uchar8 *port37Status,
                                                  L7_uchar8 *port38Status,
                                                  L7_uchar8 *port39Status,
                                                  L7_uchar8 *port40Status,
                                                  L7_uchar8 *port41Status,
                                                  L7_uchar8 *port42Status,
                                                  L7_uchar8 *port43Status,
                                                  L7_uchar8 *port44Status,
                                                  L7_uchar8 *port45Status,
                                                  L7_uchar8 *port46Status,
                                                  L7_uchar8 *port47Status)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, POE_CODE_PORTSSTATUS5, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    *port37Status = rx_buf[2];
    *port38Status = rx_buf[3];
    *port39Status = rx_buf[4];
    *port40Status = rx_buf[5];
    *port41Status = rx_buf[6];
    *port42Status = rx_buf[7];
    *port43Status = rx_buf[8];
    *port44Status = rx_buf[9];
    *port45Status = rx_buf[10];
    *port46Status = rx_buf[11];
    *port47Status = rx_buf[12];
  }
  else
  {
    *port37Status = *port38Status = *port39Status = *port40Status = 0xFF;
    *port41Status = *port42Status = *port43Status = *port44Status = 0xFF;
    *port45Status = *port46Status = *port47Status = 0xFF;
  }
  return;
}

static L7_RC_t hpcPwrdsinePoeMsgLatchesGet(L7_uint32 cardIndex, L7_uchar8 *LatchesArray, L7_int32 LatchesNum)
{
  L7_RC_t   rc;
  L7_uchar8 buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_REQUEST, 0x00, POE_CODE_GLOBAL, 0x00, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  L7_uchar8 rx_buf[POE_CONTROLLER_MSG_SIZE + 1];

  if (LatchesNum == 1)
  {
    buf[3] = POE_CODE_LATCH1;
  }
  if (LatchesNum == 2)
  {
    buf[3] = POE_CODE_LATCH2;
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);

  if (rc == L7_SUCCESS)
  {
    memcpy(LatchesArray, &rx_buf[2], 3);
  }
  return rc;
}

#endif  /* POE_FUNCTIONS_NOT_USED  -- unused functions */

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
static L7_RC_t hpcPwrdsineNotSupported(SYSAPI_POE_MSG_t *msg)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
 *
 * @purpose Enables/Disables Support for Legacy Devices.
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
static L7_RC_t hpcPwrdsinePoeSystemLegacySupport(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET == msg->cmdType)
  {
    msg->cmdData.mainDetectionType.detectionType = poeGlobalStatus_g.detectionType;
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    if (L7_POE_DETECTION_4PT_DOT3AF_LEG == msg->cmdData.mainDetectionType.detectionType)
    {
      result = hpcPwrdsinePoeMsgSystemMasksSet(cardIndex, POE_PM_PRIORITY | POE_CD_RESCAP_MODE);
    }
    else
    {
      result = hpcPwrdsinePoeMsgSystemMasksSet(cardIndex, POE_PM_PRIORITY | POE_CD_RES_MODE);
    }
    if (L7_SUCCESS != result)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set detection mode for card %u.", cardIndex);
    }
  }
  /* All done */
  hpcPwrdsinePoeReleaseAccess();

  if (result != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to set POE System Mask");
    return L7_FAILURE;
  }
  else
  {
    poeGlobalStatus_g.detectionType  = msg->cmdData.mainDetectionType.detectionType;
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Enables/Disables Support for Legacy Devices.
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
static L7_RC_t hpcPwrdsinePoeSystemFwVersion(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

  strcpy(msg->cmdData.mainFwVersion.version, versionString);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the POE status for the system
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
static L7_RC_t hpcPwrdsinePoeSystemStatus(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_uint32             on_off_status_24port, on_off_status_48port;

  if (poeInitComplete == L7_FALSE)
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

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  /* use the first card index since all cards should be the same */
  if (0 != poeGlobalStatus_g.cardStatus[0].generalInternalError)
  {
    msg->cmdData.mainOperationalStatus.status = L7_POE_SYSTEM_OPER_STATUS_FAULTY;
  }
  else
  {
    /* If any port is enabled, return ON. If all ports are disabled, return OFF. */
    result = hpcPwrdsinePoeMsgPortStatusInfoGet(0, &on_off_status_24port, &on_off_status_48port);

    if (result == L7_SUCCESS)
    {
      if ((on_off_status_24port != 0) ||
          (on_off_status_48port != 0))
      {
        msg->cmdData.mainOperationalStatus.status = L7_POE_SYSTEM_OPER_STATUS_ON;
      }
      else
      {
        msg->cmdData.mainOperationalStatus.status = L7_POE_SYSTEM_OPER_STATUS_OFF;
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Port Status Info Get failed");
    }
  }

  /* All done */
  hpcPwrdsinePoeReleaseAccess();
  return result;
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
static L7_RC_t hpcPwrdsinePoeSystemPowerConsumption(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_ushort16           power_consumption;
  L7_int                i;

  if (poeInitComplete == L7_FALSE)
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
  for (i = 0; i < L7_MAX_POE_CARDS_PER_UNIT; i++)
  {
    power_consumption += poeGlobalStatus_g.cardStatus[i].powerConsumption;
  }
  msg->cmdData.mainPowerConsumption.power = power_consumption;

  return result;
}

/*********************************************************************
 *
 * @purpose Gets/Sets the POE power management mode for the system
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
static L7_RC_t hpcPwrdsinePoeSystemPowerMgmtMode(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
  {
    return L7_REQUEST_DENIED;
  }

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if ((SYSAPI_HPC_POE_CMD_GET != msg->cmdType) &&
           (SYSAPI_HPC_POE_CMD_SET != msg->cmdType))
  {
    return L7_NOT_SUPPORTED;
  }

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  if (SYSAPI_HPC_POE_CMD_SET != msg->cmdType)
  {
    /* use the first card index since all cards should be the same */
    result = hpcPwrdsinePoeMsgPMMethodGet(0, &msg->cmdData.mainPowerMgmtMode.mode);
    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get POE PM Method");
    }
  }
  else
  {
    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      result = hpcPwrdsinePoeMsgPMMethodSet(cardIndex, msg->cmdData.mainPowerMgmtMode.mode);
      if (result != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to set POE PM Method");
        break;
      }
    }
  }

  /* All done */
  hpcPwrdsinePoeReleaseAccess();

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
static L7_RC_t hpcPwrdsinePoeSystemPowerNominal(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_ushort16           maxPowerAvail;
  L7_uint               i;

  if (poeInitComplete == L7_FALSE)
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
  for (i = 0; i < L7_MAX_POE_CARDS_PER_UNIT; i++)
  {
    maxPowerAvail += poeGlobalStatus_g.cardStatus[i].maxPowerAvailable;
  }
  msg->cmdData.mainPowerNominal.power = maxPowerAvail;

  return result;
}

/*********************************************************************
 *
 * @purpose Sets the system power usage threshold used by the notification
 *          callback.
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
static L7_RC_t hpcPwrdsinePoeSystemUsageThreshold(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result  = L7_SUCCESS;
  L7_uchar8             powerBank;
  L7_uint32             cardIndex;
  HPC_POE_CARD_DATA_t  *cardData;
#if (PD_SYSTEM == PD64008)
  L7_ushort16           actualThreshold;
#endif
#if (PD_SYSTEM == PD64012)
  L7_uint32             num_banks;
#endif
  L7_ushort16           power_consumption;
  L7_ushort16           max_shutdown_voltage;
  L7_ushort16           min_shutdown_voltage;
  L7_ushort16           max_power_available;

  if (poeInitComplete == L7_FALSE)
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

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);

#if (PD_SYSTEM == PD64008)
    actualThreshold = (L7_ushort16)((cardData->poe_default_power * poeGlobalStatus_g.usageThreshold) / 100);
    if (hpcPwrdsinePoeMsgPowerSource1Set(cardIndex, actualThreshold))
    {
      result = L7_FAILURE;
      break;
    }
    if (hpcPwrdsinePoeMsgPowerSource2Set(cardIndex, actualThreshold))
    {
      result = L7_FAILURE;
    }
#endif

#if (PD_SYSTEM == PD64012)
    if (hpcPoeCardDbPowerBanksNumGet(cardIndex, &num_banks) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex);
      result = L7_FAILURE;
      break;
    }

    if (num_banks > 1)
    {
      for (powerBank = 1; powerBank <= num_banks; powerBank++)
      {
        if (hpcPwrdsinePoeMsgPowerBanksSet(cardIndex,
                                           cardData->power_bank_map[powerBank - 1].maxPower,
                                           powerBank,
                                           poeGlobalStatus_g.usageThreshold) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power bank %d set failed", powerBank);
        }
      }
    }
    else
    {
      result = hpcPwrdsinePoeMsgMainPowerSourceSupplyParmGet(cardIndex,
                                                             &power_consumption,
                                                             &max_shutdown_voltage,
                                                             &min_shutdown_voltage,
                                                             &powerBank,
                                                             &max_power_available);
      if (L7_SUCCESS != result)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to retrive active POE Power Bank");
      }
      else
      {
        result = hpcPwrdsinePoeMsgPowerBanksSet(cardIndex,
                                                cardData->poe_default_power,
                                                powerBank,
                                                poeGlobalStatus_g.usageThreshold);
        if (L7_SUCCESS != result)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power bank %d set failed", powerBank);
        }
      }
    }
#endif
  }

  /* All done */
  hpcPwrdsinePoeReleaseAccess();
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
static L7_RC_t hpcPwrdsinePoePortInit(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_uchar8             enable;
  L7_ushort16           power_limit;
  L7_uchar8             priority;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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

  if (0 != msg->cmdData.portInit.adminState)
  {
    enable = 1;
  }
  else
  {
    enable = 0;
  }

  power_limit = msg->cmdData.portInit.powerLimit;

  switch (msg->cmdData.portInit.powerPriority)
  {
    case L7_POE_PRIORITY_CRITICAL:
      priority = 1;
      break;
    case L7_POE_PRIORITY_HIGH:
      priority = 2;
      break;
    case L7_POE_PRIORITY_LOW:
      priority = 3;
      break;
    default:
      return L7_FAILURE;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      result = hpcPwrdsinePoeMsgChannelParmsSet(cardIndex, POE_CHANNEL_ALL, enable, power_limit, priority);
      if (result != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set parms for all POE channels on cardIndex 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcPwrdsinePoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    result = hpcPwrdsinePoeMsgChannelParmsSet(cardIndex, msg->usp.port, enable, power_limit, priority);

    /* All done */
    hpcPwrdsinePoeReleaseAccess();

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set parms for POE channel %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Enables/Disables Power on a port
 *
 * @param   SYSAPI_POE_MSG_t  *msg
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoePortAdminEnable(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_uchar8             enable;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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

  if (0 != msg->cmdData.portAdminEnable.adminState)
  {
    enable = 1;
  }
  else
  {
    enable = 0;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      result = hpcPwrdsinePoeMsgChannelOnOffSet(cardIndex, POE_CHANNEL_ALL, enable);
      if (result != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to enable/disable all POE channels on cardIndex 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcPwrdsinePoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    result = hpcPwrdsinePoeMsgChannelOnOffSet(cardIndex, msg->usp.port, enable);

    /* All done */
    hpcPwrdsinePoeReleaseAccess();

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to enable/disable POE channel %d", msg->usp.port);
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
static L7_RC_t hpcPwrdsinePoePortDetectionStatus(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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
static L7_RC_t hpcPwrdsinePoePortStatistics(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8 port;

  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portCounters.mpsAbsentCounter        = poeGlobalStatus_g.poePortStatistics[port].mpsAbsent;
  msg->cmdData.portCounters.invalidSignatureCounter = poeGlobalStatus_g.poePortStatistics[port].invalidSignature;
  msg->cmdData.portCounters.powerDeniedCounter      = poeGlobalStatus_g.poePortStatistics[port].powerDenied;
  msg->cmdData.portCounters.overLoadCounter         = poeGlobalStatus_g.poePortStatistics[port].overLoad;
  msg->cmdData.portCounters.shortCounter            = poeGlobalStatus_g.poePortStatistics[port].shortCounter;

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
static L7_RC_t hpcPwrdsinePoePortCounterInvSig(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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
static L7_RC_t hpcPwrdsinePoePortCounterMpsAbsent(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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
static L7_RC_t hpcPwrdsinePoePortCounterOverload(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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
static L7_RC_t hpcPwrdsinePoePortCounterPowerDenied(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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
static L7_RC_t hpcPwrdsinePoePortCounterShort(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

/*********************************************************************
 *
 * @purpose Gets the POE status of the port
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
static L7_RC_t hpcPwrdsinePoePortStatus(SYSAPI_POE_MSG_t *msg)
{
  L7_uchar8 port;

  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portOperationalStatuses.detectionStatus = poeGlobalStatus_g.poePortStatus[port].detectionStatus;
  msg->cmdData.portOperationalStatuses.powerClass      = poeGlobalStatus_g.poePortStatus[port].powerClass;
  msg->cmdData.portOperationalStatuses.powerConsumed   = poeGlobalStatus_g.poePortStatus[port].powerConsumed;
  msg->cmdData.portOperationalStatuses.voltage         = poeGlobalStatus_g.poePortStatus[port].voltage;
  msg->cmdData.portOperationalStatuses.current         = poeGlobalStatus_g.poePortStatus[port].current;

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
static L7_RC_t hpcPwrdsinePoePortClassification(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

/*********************************************************************
 *
 * @purpose Sets the power limit for a port
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
static L7_RC_t hpcPwrdsinePoePortPowerLimit(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_ushort16           power_limit;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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
    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      result = hpcPwrdsinePoeMsgChannelPowerLimitSet(cardIndex, POE_CHANNEL_ALL, power_limit);

      if (result != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set power limit for all POE channels on cardIndex 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcPwrdsinePoeReleaseAccess();
  }
  else
  {
    /* Power Limit is set in mW, not Watts */
    if ((power_limit < L7_POE_PORT_LIMIT_MIN) || (power_limit > L7_POE_PORT_LIMIT_MAX))
    {
      return L7_FAILURE;
    }

    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    result = hpcPwrdsinePoeMsgChannelPowerLimitSet(cardIndex, msg->usp.port, power_limit);

    /* All done */
    hpcPwrdsinePoeReleaseAccess();

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Power limit for POE channel %d", msg->usp.port);
    }
  }
  return result;
}

/*********************************************************************
 *
 * @purpose Gets the max power limit for a port
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
static L7_RC_t hpcPwrdsinePoePortPowerLimitMax(SYSAPI_POE_MSG_t *msg)
{
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Id for port %u", msg->usp.port);
    return L7_FAILURE;
  }

  msg->cmdData.portPowerMaxLimit.limit = L7_POE_PORT_LIMIT_MAX;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the min power limit for a port
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
static L7_RC_t hpcPwrdsinePoePortPowerLimitMin(SYSAPI_POE_MSG_t *msg)
{
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Id for port %u", msg->usp.port);
    return L7_FAILURE;
  }

  msg->cmdData.portPowerMinLimit.limit = L7_POE_PORT_LIMIT_MIN;

  return L7_SUCCESS;
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
static L7_RC_t hpcPwrdsinePoePortPower(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portPowerOutput.output = poeGlobalStatus_g.poePortStatus[msg->usp.port].powerConsumed;

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
static L7_RC_t hpcPwrdsinePoePortPowerCurrent(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portPowerOutputCurrent.current = poeGlobalStatus_g.poePortStatus[msg->usp.port].current;

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
static L7_RC_t hpcPwrdsinePoePortVoltage(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portPowerOutputVoltage.voltage = poeGlobalStatus_g.poePortStatus[msg->usp.port].voltage;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Gets the power pairs to be used by the port. Also gets the
 *          power pairs capabilities of the device.
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
static L7_RC_t hpcPwrdsinePoePortPowerPairs(SYSAPI_POE_MSG_t *msg)
{
  if (poeInitComplete == L7_FALSE)
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

  msg->cmdData.portPowerPairs.capable = L7_FALSE;
  msg->cmdData.portPowerPairs.control = L7_POE_PORT_POWER_PAIRS_SPARE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Sets power priority level for a port
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
static L7_RC_t hpcPwrdsinePoePortPowerPriority(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_uchar8             priority;
  L7_uint32             cardIndex;

  if (poeInitComplete == L7_FALSE)
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

  switch (msg->cmdData.portPowerPriority.priority)
  {
    case L7_POE_PRIORITY_CRITICAL:
      priority = 1;
      break;
    case L7_POE_PRIORITY_HIGH:
      priority = 2;
      break;
    case L7_POE_PRIORITY_LOW:
      priority = 3;
      break;
    default:
      return L7_FAILURE;
  }

  if (L7_ALL_INTERFACES == msg->usp.port)
  {
    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      result = hpcPwrdsinePoeMsgChannelPrioritySet(cardIndex, POE_CHANNEL_ALL, priority);

      if (result != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set priority for all POE channels on cardIndex 0x%x", cardIndex);
      }
    }

    /* All done */
    hpcPwrdsinePoeReleaseAccess();
  }
  else
  {
    if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
      return L7_FAILURE;
    }

    if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

      if (poeFirmwareDebugEnabled)
        POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

      return L7_FAILURE;
    }

    result = hpcPwrdsinePoeMsgChannelPrioritySet(cardIndex, msg->usp.port, priority);

    /* All done */
    hpcPwrdsinePoeReleaseAccess();

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Priority for POE channel %d", msg->usp.port);
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Gets the temperature of the device controlling a port
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
static L7_RC_t hpcPwrdsinePoePortTemperature(SYSAPI_POE_MSG_t *msg)
{
  L7_RC_t               result = L7_SUCCESS;
  L7_uchar8             phyPort;
  L7_uint32             cardIndex;
  L7_uint32             poeDevice;

  L7_ushort16 deviceVersion;
  L7_uchar8   asicStatus;
  L7_uchar8   portsExpected;
  L7_uchar8   portsVerified;
  L7_uchar8   portsAllocated;
  L7_uchar8   temperature;
  L7_uchar8   tempSwitchHigh;
  L7_uchar8   midPowerData;
  L7_uchar8   commStatus;

  if (poeInitComplete == L7_FALSE)
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

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Id for port %u", msg->usp.port);
    return L7_FAILURE;
  }

  if (hpcPoeUnitPoePortGetFromPhyPort(msg->usp.port, &phyPort) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the Physical PoE Port");
    return L7_FAILURE;
  }

  poeDevice = phyPort / 12;

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get acccess to POE controller.");

    if (poeFirmwareDebugEnabled)
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");

    return L7_FAILURE;
  }

  result = hpcPwrdsinePoeMsgDeviceStatusGet(cardIndex, poeDevice,
                                            &deviceVersion,  &asicStatus,
                                            &portsExpected,  &portsVerified,
                                            &portsAllocated, &temperature,
                                            &tempSwitchHigh, &midPowerData,
                                            &commStatus);

  /* All done */
  hpcPwrdsinePoeReleaseAccess();

  if (result != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set Priority for POE Port %d", phyPort);
  }
  else
  {
    msg->cmdData.portTemperature.temperature = temperature;
  }

  return result;
}

#ifdef PLAT_POE_FW_UPDATE
/*********************************************************************
 *
 * @purpose Gets the POE Firmware download status for the system
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
static L7_RC_t hpcPwrdsinePoeSystemDownloadProgressIndicate(SYSAPI_POE_MSG_t *msg)
{
  L7_uint32                  cardIndex;

  if (SYSAPI_HPC_POE_CMD_QUERY == msg->cmdType)
  {
    return L7_SUCCESS;
  }
  else if (SYSAPI_HPC_POE_CMD_GET != msg->cmdType)
  {
    return L7_NOT_SUPPORTED;
  }

  if (L7_SUCCESS != hpcPoeUnitCardIndexGetFromPhyPort(msg->usp.port, &cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to get the PoE Card Index for port %u", msg->usp.port);
    return L7_FAILURE;
  }

  msg->cmdData.fwDownloadProgress.progress = poeFWDownloadInProgress[cardIndex];

  return L7_SUCCESS;
}
#endif  /* PLAT_POE_FW_UPDATE */

/*********************************************************************
 *********************************************************************
 *
 * Beginning of task, FW download and init functions.
 *
 *********************************************************************
 *********************************************************************/

/*********************************************************************
 *
 * @purpose POE Monitor Task. This task continously checks the status of the POE
 *          ports and if status of any port changes, then activates the
 *          corresponding POE LED.  It also collects data about the status
 *          of the ports and devices and caches them so that they don't have
 *          to be read from the device when the application queries them.
 *
 * @param   L7_uint32 numArgs -- Number of arguments
 *
 * @returns none
 *
 * @notes   none
 *
 ********************************************************************/
static void hpcPwrdsinePoeMonitorTask(L7_uint32 numArgs)
{
  L7_uchar8             deliveringPower[L7_POE_MAX_CHANNELS + 1];
  L7_BOOL               systemUsageExceeded                    = L7_FALSE;
  L7_uint32             i;
  L7_RC_t               rc;
  L7_uchar8             on_off;
  L7_uchar8             status;
  L7_uchar8             auto_test;
  L7_uchar8             latch;
  L7_uchar8             power_class;
  L7_ushort16           power_consumption;
  L7_ushort16           max_shutdown_voltage;
  L7_ushort16           min_shutdown_voltage;
  L7_uchar8             powerBank;
  L7_ushort16           max_power_available;
  L7_ushort16           voltage;
  L7_ushort16           current;
  L7_ushort16           power;
  HPC_POE_CARD_DATA_t  *cardData;
  L7_uint32             start_port = 0;
  L7_uint32             max_port = 0;
  L7_uint32             cardIndex;
  L7_uint32             num_banks = 0;
  L7_uint32             poe_max_power = 0;
  L7_BOOL               current_rps_state[L7_MAX_POE_CARDS_PER_UNIT];
  L7_BOOL               previous_rps_state[L7_MAX_POE_CARDS_PER_UNIT];

  L7_uchar8             cpuStatus1;
  L7_uchar8             cpuStatus2;
  L7_uchar8             factoryDefault;
  L7_uchar8             generalInternalError;
  L7_uchar8             privateLabel;
  L7_uchar8             userByte;
  L7_uchar8             deviceFail;
  L7_uchar8             tempDisconnect;
  L7_uchar8             tempAlarm;

  L7_ushort16           deviceVersion;
  L7_uchar8             asicStatus;
  L7_uchar8             portsExpected;
  L7_uchar8             portsVerified;
  L7_uchar8             portsAllocated;
  L7_uchar8             temperature;
  L7_uchar8             tempSwitchHigh;
  L7_uchar8             midPowerData;
  L7_uchar8             commStatus;

  SYSAPI_POE_TRAP_DATA_THRESHOLD_CROSSED_t trapMainThresholdCrossed;
  SYSAPI_POE_TRAP_DATA_PORT_CHANGE_t       trapPortChange;

  memset(deliveringPower, 0, sizeof(deliveringPower));

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    current_rps_state[cardIndex] = L7_DISABLE;
    previous_rps_state[cardIndex] = L7_DISABLE;
  }

  while (1)
  {
    osapiSleep(POE_TASK_MON_SLEEP);

    /* legacy testing */
    if (poeMonitorEnabled != L7_TRUE)
    {
      continue;
    }

    if (poeMonitorDebugEnabled)
    {
      POE_HPC_DPRINTF("(1)");
    }

    for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
    {
      cardData = hpcPoeCardDataGet(cardIndex);

      start_port = cardData->start_phy_port;
      max_port = start_port + cardData->numOfPorts;

      /* check if any port has changed status  - each loop iteration takes ~100ms so on a 48 port box ~= 5 sec's */
      for (i = start_port; i < max_port; i++)
      {
        /* might be being used elsewhere */
        if (hpcPwrdsinePoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
          return;
        }

        rc = hpcPwrdsinePoeMsgSinglePortStatusGet(cardIndex, i,
                                                  &on_off,
                                                  &status,
                                                  &auto_test,
                                                  &latch,
                                                  &power_class);
        if (rc == L7_SUCCESS)
        {
          /* save the results for use by the application */
          rc = hpcPwrdsinePoeSinglePortStatusCache(i,
                                                   on_off,
                                                   status,
                                                   auto_test,
                                                   latch,
                                                   power_class);

          /* if port is now delivering power */
          if (hpcPwrdsineHwDeliveringPower(status) == L7_SUCCESS)
          {
            hpcPlatformHwPoeLedDeliveringPower(i);      /* LED Green --- Default */
            /* if port was not delivering power before, call back to application */
            if (deliveringPower[i] == 0)
            {
              trapPortChange.state  = L7_POE_PORT_ON;
              trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
              sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
            }
            deliveringPower[i] = 1;
          }
          else if (hpcPwrdsineHwDetecting(status) == L7_SUCCESS)
          {
            hpcPlatformHwPoeLedDetecting(i);        /* LED OFF --- Default */
            if (deliveringPower[i] == 1)
            {
              trapPortChange.state  = L7_POE_PORT_OFF;
              trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
              sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
            }
            deliveringPower[i] = 0;
          }
          else if (hpcPwrdsineHwOverload(status) == L7_SUCCESS)
          {
            hpcPlatformHwPoeLedOverload(i);         /* LED Amber ---- Default */
            if (deliveringPower[i] == 1)
            {
              trapPortChange.state  = L7_POE_PORT_OFF;
              trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
              sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
            }
            deliveringPower[i] = 0;
          }
          else if (hpcPwrdsineHwExceedPowerBudget(status) == L7_SUCCESS)
          {
            hpcPlatformHwPoeLedExceedPower(i);      /* LED Blinking Amber --- Default */
            if (deliveringPower[i] == 1)
            {
              trapPortChange.state  = L7_POE_PORT_OFF;
              trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
              sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
            }
            deliveringPower[i] = 0;
          }
          else
          {
            hpcPlatformHwPoeLedDisable(i);      /* LED OFF --- Default */
            /* if port was delivering power before, call back to application */
            if (deliveringPower[i] == 1)
            {
              trapPortChange.state  = L7_POE_PORT_OFF;
              trapPortChange.status = poeGlobalStatus_g.poePortStatus[i].detectionStatus;
              sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_PORT_CHANGE, &trapPortChange);
            }
            deliveringPower[i] = 0;
          }

          if (status == 0x00)
          {
            /* Looks like Legacy mode has been disabled but we are still doing capacitor detection.
             * Toggle the admin mode of the port, so that capacitor detection is not done
             */
            if (hpcPwrdsineCheckLegacyMode(cardIndex) == L7_FALSE)
            {
              rc = hpcPwrdsinePoeMsgChannelOnOffSet(cardIndex, i, L7_FALSE);
              if (rc == L7_SUCCESS)
              {
                osapiSleepMSec(1);
                rc = hpcPwrdsinePoeMsgChannelOnOffSet(cardIndex, i, L7_TRUE);
                if (rc != L7_SUCCESS)
                {
                  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "Cannot Enable admin mode for channel %d", i);
                }
              }
              else
              {
                L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "Cannot disable admin mode for channel %d", i);
              }
            }
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Port status get failed for port %d", i);
        }

        rc = hpcPwrdsinePoeMsgPortParmMeasurementGet(cardIndex, i, &voltage, &current, &power);

        if (rc == L7_SUCCESS)
        {
          /* save the results for use by the application */
          rc = hpcPwrdsinePoePortParmMeasurementCache(i, voltage, current, power);
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Port measurement get failed for channel %d", i);
        }

        hpcPwrdsinePoeReleaseAccess();
        osapiSleepMSec(50);
      } /* end: for loop of all ports. */

      if (hpcPwrdsinePoeGetAccess(L7_WAIT_FOREVER) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to gain access to the controller.");
        return;
      }

      if (poeMonitorDebugEnabled)
      {
        POE_HPC_DPRINTF("(2)");
      }

      /* check if the system has crossed the usage threshold */
      rc = hpcPwrdsinePoeMsgMainPowerSourceSupplyParmGet(cardIndex,
                                                         &power_consumption,
                                                         &max_shutdown_voltage,
                                                         &min_shutdown_voltage,
                                                         &powerBank,
                                                         &max_power_available);

      if (rc == L7_SUCCESS)
      {
        /* Cache values */
        poeGlobalStatus_g.cardStatus[cardIndex].powerConsumption   = power_consumption;
        poeGlobalStatus_g.cardStatus[cardIndex].maxShutdownVoltage = max_shutdown_voltage;
        poeGlobalStatus_g.cardStatus[cardIndex].minShutdownVoltage = min_shutdown_voltage;
        poeGlobalStatus_g.cardStatus[cardIndex].activePowerBank    = powerBank;
        poeGlobalStatus_g.cardStatus[cardIndex].maxPowerAvailable  = max_power_available;

        /* if consumption is greater than threshold */
        if (power_consumption > ((max_power_available * poeGlobalStatus_g.usageThreshold) / 100))
        {
          /* if we haven't already notified the application, do so now */
          if (systemUsageExceeded == L7_FALSE)
          {
            trapMainThresholdCrossed.direction  = L7_POE_THRESHOLD_ABOVE;
            trapMainThresholdCrossed.allocPower = poeGlobalStatus_g.poePortStatus[i].powerLimit;
            sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED, &trapMainThresholdCrossed);
          }
          systemUsageExceeded = L7_TRUE;
        }
        else
        {
          /* if we haven't already notified the application, do so now */
          if (systemUsageExceeded == L7_TRUE)
          {
            trapMainThresholdCrossed.direction  = L7_POE_THRESHOLD_BELOW;
            trapMainThresholdCrossed.allocPower = poeGlobalStatus_g.poePortStatus[i].powerLimit;
            sysapiHpcPoeTrapSend(i, SYSAPI_HPC_POE_TRAP_MAIN_THRESHOLD_CROSSED, &trapMainThresholdCrossed);
          }
          systemUsageExceeded = L7_FALSE;
        }

        if (hpcPoeCardDbPowerBanksNumGet(cardIndex, &num_banks) == L7_FAILURE)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex);
        }

        if (num_banks == 1)
        {
          current_rps_state[cardIndex] = L7_DISABLE;
          if (hpcPlatformHwExternalSupplyPresent() == L7_SUCCESS)     /* Platform specific function to detect if RPS is present */
          {
            current_rps_state[cardIndex] = L7_ENABLE;
          }
          if (current_rps_state[cardIndex] != previous_rps_state[cardIndex])
          {
            previous_rps_state[cardIndex] = current_rps_state[cardIndex];

            if (current_rps_state[cardIndex] == L7_ENABLE)
            {
              poe_max_power  = cardData->poe_rps_power;
            }
            else
            {
              poe_max_power = cardData->poe_default_power;
            }
#if (PD_SYSTEM == PD64012)
            rc = hpcPwrdsinePoeMsgPowerBanksSet(cardIndex, poe_max_power, powerBank, poeGlobalStatus_g.usageThreshold);
            if (rc != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power Bank %d set failed", powerBank);
            }
#endif
          }
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Main Power Supply Param Get failed");
      }

      if (L7_SUCCESS == hpcPwrdsinePoeMsgMainSupplyVoltageGet(cardIndex, &voltage))
      {
        poeGlobalStatus_g.cardStatus[cardIndex].powerSupplyVoltage = voltage;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE Main Power Supply Voltage Get failed");
      }

      if (L7_SUCCESS == hpcPwrdsinePoeMsgSystemStatusGet(cardIndex, &cpuStatus1,
                                                         &cpuStatus2, &factoryDefault,
                                                         &generalInternalError,
                                                         &privateLabel, &userByte,
                                                         &deviceFail, &tempDisconnect,
                                                         &tempAlarm))
      {
        poeGlobalStatus_g.cardStatus[cardIndex].cpuStatus1           = cpuStatus1;
        poeGlobalStatus_g.cardStatus[cardIndex].cpuStatus2           = cpuStatus2;
        poeGlobalStatus_g.cardStatus[cardIndex].factoryDefault       = factoryDefault;
        poeGlobalStatus_g.cardStatus[cardIndex].generalInternalError = generalInternalError;
        poeGlobalStatus_g.cardStatus[cardIndex].privateLabel         = privateLabel;
        poeGlobalStatus_g.cardStatus[cardIndex].userByte             = userByte;
        poeGlobalStatus_g.cardStatus[cardIndex].deviceFail           = deviceFail;
        poeGlobalStatus_g.cardStatus[cardIndex].tempDisconnect       = tempDisconnect;
        poeGlobalStatus_g.cardStatus[cardIndex].tempAlarm            = tempAlarm;


        for (i = 0; i < L7_MAX_POE_DEVICES_PER_CARD; i++)
        {
          if ((0          == (poeGlobalStatus_g.cardStatus[cardIndex].deviceFail & (1 << i))) &&
              (L7_SUCCESS == hpcPwrdsinePoeMsgDeviceStatusGet(cardIndex, i,
                                                              &deviceVersion,  &asicStatus,
                                                              &portsExpected,  &portsVerified,
                                                              &portsAllocated, &temperature,
                                                              &tempSwitchHigh, &midPowerData,
                                                              &commStatus)))
          {
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].present = L7_TRUE;

            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].deviceVersion  = deviceVersion;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].asicStatus     = asicStatus;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].portsExpected  = portsExpected;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].portsVerified  = portsVerified;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].portsAllocated = portsAllocated;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].temperature    = temperature;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].tempSwitchHigh = tempSwitchHigh;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].midPowerData   = midPowerData;
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].commStatus     = commStatus;
          }
          else
          {
            poeGlobalStatus_g.cardStatus[cardIndex].deviceStatus[i].present = L7_FALSE;
          }
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "POE System Status Get failed");
      }

      hpcPwrdsinePoeReleaseAccess();

      if (poeMonitorDebugEnabled)
      {
        POE_HPC_DPRINTF("(3)");
      }
    }
  } /* end: Task while forever loop. */
}

/*********************************************************************
 *
 * @purpose Complete the initialization of the POE component
 *
 * @param   void
 *
 * @returns L7_RC_t result
 *
 * @notes   There is no correlation of this function's name
 *          to the configurator's phases.
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeInitPhase2(L7_uint32 cardIndex)
{
  L7_uint32   i;
  L7_RC_t     rc;
#if (PD_SYSTEM == PD64012)
  L7_ushort16 power_consumption;
  L7_ushort16 max_shutdown_voltage;
  L7_ushort16 min_shutdown_voltage;
  L7_ushort16 max_power_available;
  L7_uchar8   powerBank;
  L7_uint32   num_banks = 0;
#endif
  L7_uint32   start_port;
  L7_uint32   max_port;
  HPC_POE_CARD_DATA_t *cardData = hpcPoeCardDataGet(cardIndex);
  SYSAPI_POE_TRAP_DATA_MAIN_INIT_t trapData;
  L7_BOOL     initComplete = L7_TRUE;
  L7_uchar8   mask;

  if (L7_NULLPTR == cardData)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_SUCCESS;
  }

  if (L7_NOT_EXIST == hpcPwrdsinePoeChannelToPhysicalPortMap())
  {
    /* Needs FW update but it should have already happened */
    return L7_NOT_EXIST;
  }

  /* Initialize mechanism to use POE LED's */
  hpcPwrdsinePoeLedInit(cardIndex);

  /* Disable all ports */
  rc = hpcPwrdsinePoeMsgChannelOnOffSet(cardIndex, POE_CHANNEL_ALL, 0);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to turn all ports off");
  }

#if (PD_SYSTEM == PD64012)

  if (hpcPoeCardDbPowerBanksNumGet(cardIndex, &num_banks) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex);
    return L7_SUCCESS;
  }

  if (num_banks > 1)
  {
    for (powerBank = 1; powerBank <= num_banks; powerBank++)
    {
      rc = hpcPwrdsinePoeMsgPowerBanksSet(cardIndex, cardData->power_bank_map[powerBank - 1].maxPower,
                                          powerBank, poeGlobalStatus_g.usageThreshold);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to set POE Power bank %d", powerBank);
      }
    }
  }
  else
  {
    rc = hpcPwrdsinePoeMsgMainPowerSourceSupplyParmGet(cardIndex,
                                                       &power_consumption,
                                                       &max_shutdown_voltage,
                                                       &min_shutdown_voltage,
                                                       &powerBank,
                                                       &max_power_available);
    rc = hpcPwrdsinePoeMsgPowerBanksSet(cardIndex, cardData->poe_default_power,
                                        powerBank, poeGlobalStatus_g.usageThreshold);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to set POE Power bank %d", powerBank);
      return L7_FAILURE;
    }
  }
#endif

#if (PD_SYSTEM == PD64008)
  /* set power source 1 */
  rc = hpcPwrdsinePoeMsgPowerSource1Set(cardIndex, cardData->poe_default_power);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power Source 1 set failed");
    return L7_FAILURE;
  }

  /* set power source 2 */
  rc = hpcPwrdsinePoeMsgPowerSource2Set(cardIndex, cardData->poe_default_power);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Power Source 2 set failed");
    return L7_FAILURE;
  }
#endif

  start_port = cardData->start_phy_port;
  max_port = start_port + cardData->numOfPorts;

  for (i = start_port; i < max_port; i++)
  {
    poeGlobalStatus_g.poePortStatus[i].detectionStatus      = L7_POE_PORT_DETECTION_STATUS_DISABLED;
    poeGlobalStatus_g.poePortStatus[i].powerClass           = L7_POE_PORT_POWER_CLASS0;
    poeGlobalStatus_g.poePortStatus[i].powerConsumed        = 0;
    poeGlobalStatus_g.poePortStatus[i].voltage              = 0;
    poeGlobalStatus_g.poePortStatus[i].current              = 0;

    poeGlobalStatus_g.poePortStatistics[i].mpsAbsent        = 0;
    poeGlobalStatus_g.poePortStatistics[i].invalidSignature = 0;
    poeGlobalStatus_g.poePortStatistics[i].powerDenied      = 0;
    poeGlobalStatus_g.poePortStatistics[i].overLoad         = 0;
    poeGlobalStatus_g.poePortStatistics[i].shortCounter     = 0;
  }

  /*
   * Now set initialization configuration.
   */
  hpcPwrdsinePoeMsgPMMethodSet(cardIndex, poeGlobalStatus_g.powerMgmtMode);

  if (L7_POE_DETECTION_4PT_DOT3AF_LEG == poeGlobalStatus_g.detectionType)
  {
    mask = POE_PM_PRIORITY | POE_CD_RESCAP_MODE;
  }
  else
  {
    mask = POE_PM_PRIORITY | POE_CD_RES_MODE;
  }

  rc = hpcPwrdsinePoeMsgSystemMasksSet(cardIndex, mask);
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Failed to set detection mode for card %u.", cardIndex);
  }

#if (PD_SYSTEM == PD64012)
  rc = hpcPwrdsinePoeMsgSaveSystemConfig(cardIndex);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Unable to save PoE config in controller");
  }
#endif

  /*
   * Only need one monitor task since it will iterate through all the cards.
   */
  if (L7_NULL == poeMonitorTaskId)
  {
    /* create the POE monitor task */
    poeMonitorTaskId = osapiTaskCreate("poe_monitor",
                                       hpcPwrdsinePoeMonitorTask,
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
    trapData.usageThreshold = poeGlobalStatus_g.usageThreshold;
    trapData.powerMgmtMode  = poeGlobalStatus_g.powerMgmtMode;

    sysapiHpcPoeTrapSend(L7_ALL_INTERFACES, SYSAPI_HPC_POE_TRAP_MAIN_INIT_COMPLETE, &trapData);

    poeInitComplete = L7_TRUE;
  }

  return L7_SUCCESS;
}

#ifdef PLAT_POE_FW_UPDATE
/*********************************************************************
 *
 * @purpose Erase the firmware
 *
 * @param   cardIndex
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hpcPwrdsinePoeFirmwareErase(L7_uint32 cardIndex)
{
  L7_RC_t       rc;
  L7_int32      i;
  L7_uchar8     rx_buf[POE_CONTROLLER_MSG_SIZE + 1];
  L7_uchar8     tx_buf[POE_CONTROLLER_MSG_SIZE + 1] = {POE_CODE_PROGRAM, 0x00, POE_CODE_FLASH, 0x99, 0x15, 0x16, 0x16, 0x99, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N, POE_CODE_N};
  /*
   * Messages used for download procedure.
   */
  static L7_uchar8 msgCmdEnter               [] = "ENTR";
  static L7_uchar8 msgCmdErase               [] = "E";

  static L7_uchar8 msgResponseBoot           [] = "TPE\r\n";
  static L7_uchar8 msgResponseErase          [] = "TOE\r\n";
  static L7_uchar8 msgResponseEraseInProgress[] = "TE\r\n";

  if (L7_TRUE != hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_SUCCESS;
  }

  /*
    The controller was reset during driver init. If that failed because the firmware is
    bad, unexpected responses to another reset have been seen hard to handle, so don't reset again.
  */

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("(1)");
  }

  if (hpcPwrdsinePoeGetAccess(POE_CONTROLLER_ACCESS_TIMEOUT) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s: Unable to get acccess to POE controller.", __FUNCTION__);

    if (poeFirmwareDebugEnabled)
    {
      POE_HPC_DPRINTF("Unable to get acccess to POE controller.");
    }

    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("(2)");
  }

  /*
   * If the firmware is invalid or non-existent
   */
  if (L7_TRUE == poeFWInvalid[cardIndex])
  {
    /* a firmware download is required. */
#if 0
    if (poeFirmwareDebugEnabled)
    {
      POE_HPC_DPRINTF("HW Reset of controller...");
    }

    hpcPlatformHwPoeReset();            /* HW reset of PoE controller. */

    /*
     * Look for system status telemetry.  Should be received within 4s.
     */
    i = 0;
    do
    {
      rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, tx_buf, rx_buf, 0, 400); /* Nothing to send, but get response */
      i++;
    } while (((L7_SUCCESS != rc) ||
              (POE_CODE_TELEMETRY != rx_buf[0]) ||
              (0xFF               != rx_buf[1])) &&
             (i < 10));
#endif
    if (poeFirmwareDebugEnabled)
    {
      POE_HPC_DPRINTF("Sending E-N-T-R command...");
    }

    for (i = 0; i < strlen(msgCmdEnter); i++)
    {
      if (poe_device_write(cardIndex, &msgCmdEnter[i], 1) == (-1))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Could not write '%c' to PoE Firmware Controller.", __FUNCTION__, msgCmdEnter[i]);
        poeFWDownloadInProgress[cardIndex] = L7_FALSE;

        hpcPwrdsinePoeReleaseAccess();
        return L7_FAILURE;
      }
      if (poeFirmwareDebugEnabled)  /* Arvind */
      {
        POE_HPC_DPRINTF("Sending %d command...", i);
      }
      /* Wait for minimum inter-character delay */
      osapiSleepMSec(50);   /* Arvind */
    }
  }
  else
  {
    /*
     * E N T E R   N O R M A L   P R O G R A M   M O D E
     */
    if (poeFirmwareDebugEnabled)
    {
      POE_HPC_DPRINTF("Entering normal programming mode...");
    }
    rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, tx_buf, rx_buf, POE_CONTROLLER_MSG_SIZE, POE_CONTROLLER_DEFAULT_TIMEOUT);
    if ((L7_SUCCESS      != rc) ||
        (POE_CODE_REPORT != rx_buf[POE_KEY]) ||
        (0               != rx_buf[2]) ||
        (0               != rx_buf[3]))
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s: Normal programming mode failed.", __FUNCTION__);
      poeFWDownloadInProgress[cardIndex] = L7_FALSE;

      hpcPwrdsinePoeReleaseAccess();
      return rc;
    }
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok.");
  }

  /*
   * A W A I T   F O R   B O O T   S E C T I O N   R E S P O N S E
   */
  i = 0;

  do
  {
    rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, tx_buf, rx_buf, 0, 200); /* Nothing to send, but get response */
    i++;
  } while ((0 != memcmp(msgResponseBoot, rx_buf, strlen(msgResponseBoot))) &&
           (i < 10));

  if ((L7_SUCCESS != rc) ||
      (0 != memcmp(msgResponseBoot, rx_buf, strlen(msgResponseBoot))))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Invalid boot section response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok");
  }

  /*
   * E R A S E   E E P R O M
   */
  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("Sending erase command 'E'...");
  }

  if (poe_device_write(cardIndex, msgCmdErase, strlen(msgCmdErase)) == (-1))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Could not write '%s' to PoE Firmware Controller!", __FUNCTION__, msgCmdErase);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  i = 0;

  do
  {
    rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, tx_buf, rx_buf, 0, 200); /* Nothing to send, but get response */
    i++;
  } while ((0 != memcmp(msgResponseErase, rx_buf, strlen(msgResponseErase))) &&
           (i < 10));

  if ((L7_SUCCESS != rc) &&
      (0 != memcmp(msgResponseErase, rx_buf, strlen(msgResponseErase))))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Invalid erase command response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok\r\nAwaiting erasure second response...");
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, msgCmdErase, rx_buf, 0, 5000); /* Wait 5 seconds for second response */
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Timeout waiting for second erase response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (0 != memcmp(msgResponseEraseInProgress, rx_buf, strlen(msgResponseEraseInProgress)))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Invalid second erase response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok\r\nAwaiting erasure third response...");
  }

  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, msgCmdErase, rx_buf, 0, 100); /* Wait 100 ms for third response */
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Timeout waiting for third erase response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (0 != memcmp(msgResponseBoot, rx_buf, strlen(msgResponseBoot)))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Invalid third erase response.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok.");
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Download new firmware to the controller
 *
 * @param   *cardIndex
 *
 * @returns L7_RC_t result
 *
 * @notes   Upon successful completion, this task will finish
 *          the PoE initialization.
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeFirmwareDownloadTask(L7_uint32 args, L7_uint32 *argv)
{
  L7_RC_t       rc;
  char          s19_line[128];
  L7_ushort16   sw_ver;
  L7_uchar8     hw_ver;
  L7_uchar8     build_num;
  L7_int32      s19_record;
  L7_uchar8     rx_buf[POE_CONTROLLER_MSG_SIZE + 1];
  L7_uint32     cardIndex = argv[0];
  L7_int        i;

  /*
   * Messages used for download procedure.
   */
  static L7_uchar8 msgCmdProgram          [] = "P";
  static L7_uchar8 msgCmdReset            [] = "RST";

  static L7_uchar8 msgResponseProgramming [] = "TOP\r\n";
  static L7_uchar8 msgResponseLineReceived[] = "T*\r\n";
  static L7_uchar8 msgResponseProgComplete[] = "TP\r\n";

  if (L7_TRUE != hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_SUCCESS;
  }

  rc = hpcPwrdsinePoeFirmwareErase(cardIndex);
  if (L7_SUCCESS != rc)
  {
    return rc;
  }

  /*
   * P R O G R A M
   */
  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("Sending program command 'P'...");
  }

  if (poe_device_write(cardIndex, msgCmdProgram, strlen(msgCmdProgram)) == (-1))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Could not write '%s' to PoE Firmware Controller!", __FUNCTION__, msgCmdProgram);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  i = 0;

  do
  {
    rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, L7_NULL, rx_buf, 0, 200); /* Nothing to send, but get response */
    i++;
  } while ((0 != memcmp(msgResponseProgramming, rx_buf, strlen(msgResponseProgramming))) &&
           (i < 10));

  if ((L7_SUCCESS != rc) &&
      (0 != memcmp(msgResponseProgramming, rx_buf, strlen(msgResponseProgramming))))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Invalid response to '%s' command.", __FUNCTION__, msgCmdProgram);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;

    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok.");
  }

  /*
   * P R O G R A M   E A C H   L I N E   O F   T H E   S 1 9   F I L E
   */

  printf("\r\nStarting to program %d records.\r\n", numOfRecs);
  printf("Programming record.......");
#if 0
  poeDebugEnabled          = 0;
  poeReadDebugEnabled      = 0;
  poeFirmwareDebugEnabled  = 0;
#endif

  s19_record = 0;
  do
  {
    memset(s19_line, 0, sizeof(s19_line));
    strncpy(s19_line, PowerDsine_firmware[s19_record], sizeof(s19_line));
    strncat(s19_line, "\r\n", sizeof(s19_line) - strlen(s19_line));
    if ((S19_RECORD_TYPE_DATA == s19_line[S19_RECORD_TYPE_OFFSET]) ||
        (S19_RECORD_TYPE_EOF  == s19_line[S19_RECORD_TYPE_OFFSET]))
    {
#if 0
      if (S19_RECORD_TYPE_EOF == PowerDsine_firmware[s19_record + 1][S19_RECORD_TYPE_OFFSET])
      {
        poeDebugEnabled          = 1;
        poeReadDebugEnabled      = 1;
        poeFirmwareDebugEnabled  = 1;
      }
#endif

      /*
       * S E N D   L I N E
       */
      printf("\b\b\b\b%4d", s19_record);

      rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, s19_line, rx_buf, strlen(s19_line), 1000); /* Wait 100 ms for response */

      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s: Timeout waiting for an S19 line response for line %d.", __FUNCTION__, s19_record);
        poeFWDownloadInProgress[cardIndex] = L7_FALSE;

        hpcPwrdsinePoeReleaseAccess();
        return L7_FAILURE;
      }

      /* Wait for POE_CTLR_RSP_TSPLAT or POE_CTLR_RSP_TP on last line. */
      if (S19_RECORD_TYPE_EOF == s19_line[S19_RECORD_TYPE_OFFSET])
      {
        if (0 != memcmp(msgResponseProgComplete, rx_buf, strlen(msgResponseProgComplete)))
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Did not receive correct S19 EOF line response for line %d.", __FUNCTION__, s19_record);
          poeFWDownloadInProgress[cardIndex] = L7_FALSE;

          hpcPwrdsinePoeReleaseAccess();
          return L7_FAILURE;
        }
      }
      else                              /* must be data record */
      {
        if (0 != memcmp(msgResponseLineReceived, rx_buf, strlen(msgResponseLineReceived)))
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Did not receive correct S19 line response for line %d.", __FUNCTION__, s19_record);
          poeFWDownloadInProgress[cardIndex] = L7_FALSE;

          hpcPwrdsinePoeReleaseAccess();
          return L7_FAILURE;
        }
      }
    }

    s19_record++;
  } while (s19_line[S19_RECORD_TYPE_OFFSET] != S19_RECORD_TYPE_EOF);

#if 0
  poeDebugEnabled          = 1;
  poeReadDebugEnabled      = 1;
  poeFirmwareDebugEnabled  = 1;
#endif

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("Waiting at least 400ms before reset...");
  }

  /* POE spec says we should wait at least 400ms. */
  osapiSleep(2);
  printf("\nok.");

  /*
   * R E S E T   T H E   C O N T R O L L E R
   */

  printf("\r\nResetting controller...");

  /* Issue the reset and wait 10 seconds for the response. */
  rc = hpcPwrdsinePoeTransReceiveMsg(cardIndex, msgCmdReset, rx_buf, strlen(msgCmdReset), 10000); /* Wait 10 s for response */
  if (L7_SUCCESS != rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Timeout waiting for response to 'RST' command.", __FUNCTION__);
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;
    hpcPwrdsinePoeReleaseAccess();
    return L7_FAILURE;
  }
  /* This should be a System Status Telemetry message. */
  else if ((rx_buf[POE_KEY] == POE_CODE_TELEMETRY) && (rx_buf[POE_ECHO] == 0xFF))
  {
    if (rx_buf[2] & POE_PROG_REQD)      /* Controller does not have valid firmware */
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "%s: Firmware programming failed.", __FUNCTION__);
      poeFWInvalid[cardIndex] = L7_TRUE;
      poeFWDownloadInProgress[cardIndex] = L7_FALSE;
      hpcPwrdsinePoeReleaseAccess();
      return L7_FAILURE;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "RST command response was invalid.");
    rc = L7_FAILURE;
  }

  if (poeFirmwareDebugEnabled)
  {
    POE_HPC_DPRINTF("ok\r\n\n");
  }

  /*
   * Now that programming is complete, get the software version again.
   */
  hpcPwrdsinePoeMsgSoftwareVersionGet(cardIndex, &hw_ver, &sw_ver, &build_num);
  sprintf(versionString, "%d_%d", sw_ver, build_num);

  poeFWDownloadInProgress[cardIndex] = L7_FALSE;

  hpcPwrdsinePoeReleaseAccess();
  if (poeMonitorTaskId == 0)
  {
    hpcPwrdsinePoeInitPhase2(cardIndex);
  }

  printf("Done.\n PoE Firmware upgraded to %s\n", versionString);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Initiate a firmware download via a task creation.
 *
 * @param   void
 *
 * @returns L7_RC_t result
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hpcPwrdsinePoeFirmwareDownloadBegin(L7_uint32 cardIndex)
{
  L7_uchar8 fw_dwnld_task_name[80];
  static L7_uint32 card_id[L7_MAX_POE_CARDS_PER_UNIT];

  if (L7_TRUE != hpcPoeCardIndexIsValid(cardIndex))
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return L7_SUCCESS;
  }

  snprintf(fw_dwnld_task_name, sizeof(fw_dwnld_task_name), "PoeFirmwareDownload.%d", cardIndex);
  card_id[cardIndex] = cardIndex;

  poeFWDownloadInProgress[cardIndex] = L7_TRUE;

  /* The priority of the task that does Firmware download should be the highest */
  if (L7_ERROR == osapiTaskCreate(fw_dwnld_task_name,
                                  hpcPwrdsinePoeFirmwareDownloadTask,
                                  1,
                                  &card_id[cardIndex],
                                  L7_DEFAULT_STACK_SIZE,
                                  L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY - 1),
                                  L7_DEFAULT_TASK_SLICE))
  {
    poeFWDownloadInProgress[cardIndex] = L7_FALSE;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif  /* PLAT_POE_FW_UPDATE */

/*********************************************************************
 *
 * @purpose POE read task, reads messages obtained from a POE controller
 *     into a response buffer
 *
 * @param   L7_uint32 numArgs -- Number of arguments
 *
 * returns  none
 *
 ********************************************************************/
static void hpcPwrdsinePoeReadTask(L7_uint32 numArgs, L7_uint32 *argv)
{
  L7_int        byte_count = 0;
  L7_int        bytes_read;
  L7_uint32     cardIndex;
  L7_int        count;
  L7_int        width;
  L7_int        i, j;
  L7_uchar8     buf[HPCPOE_PRINTF_QUEUE_MESSAGE_SIZE * 2];
  void         *messageQueue;
  HPC_POE_RX_TASK_DATA_t *taskData;
  static struct timeval timeout = {0, (POE_CONTROLLER_DEFAULT_TIMEOUT - 50) * 1000};
  HPC_POE_CONTROLLER_MESSAGE_t message;
  HPC_POE_CARD_DATA_t *cardData;
  taskData = (HPC_POE_RX_TASK_DATA_t *)argv;

  cardIndex   = taskData->cardIndex;
  messageQueue = taskData->messageQueue;

  cardData = hpcPoeCardDataGet(cardIndex);
  if (L7_NULLPTR == cardData)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Invalid card index %u", cardIndex);
    return;
  }

  do
  {
    struct fd_set readFds;

    FD_ZERO(&readFds);
    FD_SET(cardData->poe_fd, &readFds);
    width = cardData->poe_fd + 1;
    /*
     * If a partial message has been read, set a timeout.
     */
    if (-1 == select(width, &readFds, L7_NULL, L7_NULL, (byte_count > 0) ? &timeout : L7_NULL))
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Select on read descriptor failed.");
      break;
    }

    bytes_read = poe_device_read(cardIndex, &message.buf[byte_count], POE_CONTROLLER_MSG_SIZE - byte_count);
    if (bytes_read < 0)
    {
      if (0 == byte_count)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Error during read!");
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_POE_COMPONENT_ID, "Partial message received!");
        byte_count = 0;
      }
    }
    else
    {
      if (0 != poeReadDebugEnabled)
      {
        strcpy(buf, "Bytes read:\r\n");
        j = strlen(buf);
        for (i = 0; i < bytes_read; i++)
        {
          j += sprintf(&buf[j], "%2.2x ", message.buf[i + byte_count]);
        }
        strcat(buf, "\r\n");
        hpcPoePrintf(buf);
      }

      byte_count += bytes_read;

/*      if (L7_TRUE == poeFWDownloadInProgress[cardIndex]) */
      if ('T' == message.buf[0])
      {
        /*
         * Extract firmware download messages which are terminated with \r\n.
         *
         * Start at 1 since there must be at least one character in the message.
         */
        count = 1;
        while (count < byte_count - 1)
        {
          if (('\r' == message.buf[count]) &&
              ('\n' == message.buf[count + 1]))
          {
            count += 2;

            if (0 != poeReadDebugEnabled)
            {
              j = sprintf(buf, "%s -- sending:\r\n", __FUNCTION__);
              for (i = 0; i < count; i++)
              {
                j += sprintf(&buf[j], "%2.2x ", message.buf[i]);
              }
              strcat(buf, "\r\n");
              hpcPoePrintf(buf);
            }

            message.size = count;
            if (L7_SUCCESS != osapiMessageSend(messageQueue, &message, sizeof(message),
                                               L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM))
            {
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID,
                      "%s -- Could not send received string", __FUNCTION__);
            }

            if (byte_count > count)
            {
              memmove(message.buf, &message.buf[count], byte_count - count);
            }
            byte_count -= count;
            count = 0;
          }
          count++;
        }

        if (byte_count < 0)
        {
          byte_count = 0;
        }
      }
      else if ((POE_CODE_TELEMETRY != message.buf[0]) &&
               (POE_CODE_REPORT    != message.buf[0]))
      {
        /*
         * Valid messages from the controller only begin with 'T' or the telemetry code or
         * the report code.  The first character is none of those so drop "invalid"
         * characters until we find one that is the valid beginning of a message.
         */
        for (i = 1; i < byte_count; i++)
        {
          if ((POE_CODE_TELEMETRY == message.buf[0]) ||
              (POE_CODE_REPORT    == message.buf[0]) ||
              ('T'                == message.buf[0]))
          {
            break;
          }
        }

        if (i < byte_count)
        {
          memmove(message.buf, &message.buf[i], byte_count - i);
          byte_count -= i;
        }
        else
        {
          byte_count = 0;
        }
      }

      if ((byte_count >= POE_CONTROLLER_MSG_SIZE) ||
          (bytes_read == 0))
      {
        if (0 != poeReadDebugEnabled)
        {
          j = sprintf(buf, "%s -- sending:\r\n", __FUNCTION__);
          for (i = 0; i < min(byte_count, POE_CONTROLLER_MSG_SIZE); i++)
          {
            j += sprintf(&buf[j], "%2.2x ", message.buf[i]);
          }
          strcat(buf, "\r\n");
          hpcPoePrintf(buf);
        }

        message.size = min(byte_count, POE_CONTROLLER_MSG_SIZE);

        if (L7_SUCCESS != osapiMessageSend(messageQueue, &message, sizeof(message), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM))
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "%s -- Could not send received message", __FUNCTION__);
        }

        byte_count = 0;
      }
    }
  } while (1);
}

/*********************************************************************
 *
 * @purpose Initialize POE component
 *
 * @param   void
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hpcPwrdsinePoeInit(SYSAPI_POE_MSG_t *const msg)
{
  HPC_POE_CARD_DATA_t       *cardData;
  HPC_POE_RX_TASK_DATA_t     taskData;
  L7_RC_t     rc;
  L7_uchar8   hw_ver;
  L7_ushort16 sw_ver;
  L7_uchar8   build_num;
  L7_uchar8   readTaskName[80];
  L7_uchar8   readQName[80];
  L7_uint32   cardIndex = 0;
  L7_BOOL     cardInitialized  = L7_FALSE;
  L7_BOOL     fwUpdateRequired = L7_FALSE;
  SYSAPI_POE_TRAP_DATA_MAIN_INIT_t trapData = {0};

  if (L7_TRUE != poeInitialized)
  {
#ifdef HPC_POE_PWRDSNE_DEBUG
    hpcPoePrintfTaskStart();
#endif
    /*
     * This is the first time here so initialize all data structures.
     */
    memset(poe_rx_msgq,             0, sizeof(poe_rx_msgq));
#ifdef PLAT_POE_FW_UPDATE
    memset(poeFWDownloadInProgress, 0, sizeof(poeFWDownloadInProgress));
#endif
    memset(poeFWInvalid,            0, sizeof(poeFWInvalid));
    memset(versionString,           0, sizeof(versionString));
    memset(&poeGlobalStatus_g,      0, sizeof(poeGlobalStatus_g));
    memset(poeMsgHandlers,          0, sizeof(poeMsgHandlers));
  }
  else if (L7_TRUE == msg->cmdData.mainInit.warmRestart)
  {
    trapData.usageThreshold = poeGlobalStatus_g.usageThreshold;
    trapData.powerMgmtMode  = poeGlobalStatus_g.powerMgmtMode;

    sysapiHpcPoeTrapSend(L7_ALL_INTERFACES, SYSAPI_HPC_POE_TRAP_MAIN_INIT_COMPLETE, &trapData);

    return L7_SUCCESS;
  }

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
  poeGlobalStatus_g.detectionType  = msg->cmdData.mainInit.detectionType;

  /* This function should be defined in Platform specific directory if PoE is present.
     This function will have any platform specific init code necessary. */
  if (hpcPlatformHwPoeInit() == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "PoE Initialization failed in Hardware");
    return L7_FAILURE;
  }

  if (L7_TRUE == poeInitialized)
  {
    /*
     * Don't need to do a complete init here.  All the tasks, etc. are already set up.
     * Just need to reset all values to defaults.
     */
    /* L7_uint i;

    for (i = 0; i < hpcPoeCardCountGet(); i++)
    {
      poeGlobalStatus_g.cardStatus[i].initialized = L7_FALSE;
    }  */
  }
  else
  {
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

      cardData->poe_fd = poe_device_open(cardData->type,
                                         &cardData->parms,
                                         cardData->file_descr,
                                         0);
      if (-1 == cardData->poe_fd)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "Could not open %s for writing on card %d.", cardData->file_descr, cardIndex);
        continue;
      }

      snprintf(readQName, sizeof(readQName), "poeReadQ%d", cardIndex);

      poe_rx_msgq[cardIndex] = osapiMsgQueueCreate(readQName, POE_CONTROLLER_QUEUE_SIZE, sizeof(HPC_POE_CONTROLLER_MESSAGE_t));
      if (L7_NULLPTR == poe_rx_msgq[cardIndex])
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "Read queue not initialized for card %d.", cardIndex);
        continue;
      }

      taskData.cardIndex   = cardIndex;
      taskData.messageQueue = poe_rx_msgq[cardIndex];

      snprintf(readTaskName, sizeof(readTaskName), "poeRead.%d", cardIndex);
      /*
       * Create Read Task for PowerDsine chipsets, these chipsets sometimes get
       * unsolicited messages from PoE controller, which need to be read separately in a
       * task or else they would be lost. Also, UART communication does not have any
       * timeout in reading, which means if for some reason, UART fails, we would lock up
       * the switch. Reading in a separate task avoids this complication.  This task runs
       * at a higher priority than the default since it must receive messages in a timely
       * fashion.  It has be seen to be starved in certain situations if it runs at the default.
       */
      if (L7_ERROR == osapiTaskCreate(readTaskName,
                                      hpcPwrdsinePoeReadTask,
                                      1,
                                      &taskData,
                                      L7_DEFAULT_STACK_SIZE,
                                      L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY - 2),
                                      L7_DEFAULT_TASK_SLICE))
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_POE_COMPONENT_ID, "Read task not initialized for card %d.", cardIndex);
        continue;
      }

      cardInitialized = L7_TRUE;
    }

    if (L7_TRUE != cardInitialized)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID, "No cards initialized, PoE not operational.");
      return L7_FAILURE;                  /* No cards were initialized so don't continue. */
    }
  }

  /*
   * This needs to be performed every time PoE is initialized since phase 2 needs to be
   * performed for every card.  Even though we check again for FW update, it will not need
   * to be done on a re-initialize.
   */
  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    if (poeGlobalStatus_g.cardStatus[cardIndex].initialized == L7_FALSE)
    {
      fwUpdateRequired = L7_FALSE;

      /* restore the POE chip to factory defaults */
      rc = hpcPwrdsinePoeMsgReset(cardIndex, &poeFWInvalid[cardIndex], POE_CONTROLLER_RESET_TIMEOUT);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Controller reset command failed");
        poeFWInvalid[cardIndex] = L7_TRUE;
      }

      if ((L7_SUCCESS == rc) &&
          (L7_FALSE   == poeFWInvalid[cardIndex]))
      {
        rc = hpcPwrdsinePoeMsgSoftwareVersionGet(cardIndex, &hw_ver, &sw_ver, &build_num);
        if (rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "POE Controller version get command failed");
          poeFWInvalid[cardIndex] = L7_TRUE;
        }
        else
        {
          snprintf(versionString, sizeof(versionString), "%d_%d", sw_ver, build_num);
#ifdef PLAT_POE_FW_UPDATE
          if ((FIRMWARE_VERSION  != sw_ver) ||
              (FIRMWARE_BUILDNUM != build_num))
          {
            fwUpdateRequired = L7_TRUE;
          }
#endif
        }
      }

      if ((L7_TRUE == fwUpdateRequired) ||
          (L7_TRUE == poeFWInvalid[cardIndex]))
      {
#ifdef PLAT_POE_FW_UPDATE
        if (sizeof(PowerDsine_firmware) != 0)
        {
          hpcPwrdsinePoeFirmwareDownloadBegin(cardIndex);
        }
        else
#endif
        {
        /*
         * The controller requires programming based on the system telemetry
         * message, but there is no embedded firmware.
         */
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_POE_COMPONENT_ID,
                  "%s: PoE controller requires programming - no firmware available in code.", __FUNCTION__);
          return L7_FAILURE;
        }
      }
      else                              /* This will be done after firmware download. */
      {
        if (hpcPwrdsinePoeInitPhase2(cardIndex) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcPwrdsinePoeInitPhase2 failed.");
          return L7_FAILURE;
        }
      }
    }
    else        /* If Poe Cards are already initialized, then only do Phase 2 Init */
    {
      poeGlobalStatus_g.cardStatus[cardIndex].initialized = L7_FALSE;
      if (hpcPwrdsinePoeInitPhase2(cardIndex) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "hpcPwrdsinePoeInitPhase2 failed.");
        return L7_FAILURE;
      }
    }
  }

  /*
   * Enough initialization has now been completed to populate the function table.
   */
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_AUTO_RESET_MODE]           = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_DETECTION_MODE]            = hpcPwrdsinePoeSystemLegacySupport;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_FW_VERSION]                = hpcPwrdsinePoeSystemFwVersion;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_NOTIFICATION_CONTROL]      = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_OPERATIONAL_STATUS]        = hpcPwrdsinePoeSystemStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_CONSUMPTION]         = hpcPwrdsinePoeSystemPowerConsumption;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_MGMT_MODE]           = hpcPwrdsinePoeSystemPowerMgmtMode;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_POWER_NOMINAL]             = hpcPwrdsinePoeSystemPowerNominal;
  poeMsgHandlers[SYSAPI_HPC_POE_MAIN_USAGE_THRESHOLD]           = hpcPwrdsinePoeSystemUsageThreshold;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_INIT]                      = hpcPwrdsinePoePortInit;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_ADMIN_STATE]               = hpcPwrdsinePoePortAdminEnable;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTERS]                  = hpcPwrdsinePoePortStatistics;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_INVALID_SIGNATURE] = hpcPwrdsinePoePortCounterInvSig;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_MPS_ABSENT]        = hpcPwrdsinePoePortCounterMpsAbsent;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_OVERLOAD]          = hpcPwrdsinePoePortCounterOverload;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_POWER_DENIED]      = hpcPwrdsinePoePortCounterPowerDenied;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_COUNTER_SHORT]             = hpcPwrdsinePoePortCounterShort;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_DETECTION_MODE]            = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_DETECTION_STATUS]          = hpcPwrdsinePoePortDetectionStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_ERROR_CODE]                = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_HIGH_POWER_MODE]           = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_OPERATIONAL_STATUSES]      = hpcPwrdsinePoePortStatus;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_CLASSIFICATION]      = hpcPwrdsinePoePortClassification;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_CUR_LIMIT]           = hpcPwrdsinePoePortPowerLimit;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_MAX_LIMIT]           = hpcPwrdsinePoePortPowerLimitMax;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_MIN_LIMIT]           = hpcPwrdsinePoePortPowerLimitMin;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT]              = hpcPwrdsinePoePortPower;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT_CURRENT]      = hpcPwrdsinePoePortPowerCurrent;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_OUTPUT_VOLTAGE]      = hpcPwrdsinePoePortVoltage;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_PAIRS_CONTROL]       = hpcPwrdsinePoePortPowerPairs;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_POWER_PRIORITY]            = hpcPwrdsinePoePortPowerPriority;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_RESET]                     = hpcPwrdsineNotSupported;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_TEMPERATURE]               = hpcPwrdsinePoePortTemperature;
  poeMsgHandlers[SYSAPI_HPC_POE_PORT_VIOLATION_TYPE]            = hpcPwrdsineNotSupported;
#ifdef PLAT_POE_FW_UPDATE
  poeMsgHandlers[SYSAPI_HPC_POE_FIRMWARE_DOWNLOAD_PROGRESS]     = hpcPwrdsinePoeSystemDownloadProgressIndicate;
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
    rc = hpcPwrdsinePoeInit(msg);
  }
  else if ((SYSAPI_HPC_POE_MSG_ID_MIN < msg->msgId) &&
           (SYSAPI_HPC_POE_MSG_ID_MAX > msg->msgId) &&
           (L7_NULL != poeMsgHandlers[msg->msgId]))
  {
    rc = poeMsgHandlers[msg->msgId](msg);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POE_COMPONENT_ID,
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

void hpcPoeDebugOff(void)
{
  poeMonitorDebugEnabled   = 0;
  poeDebugEnabled          = 0;
  poeReadDebugEnabled      = 0;
  poeFirmwareDebugEnabled  = 0;
}

int hpcPoeDebugEnable(L7_uint32 enabled)
{
  poeDebugEnabled = enabled;

  return poeDebugEnabled;
}

int hpcPoeReadDebugEnable(L7_uint32 enabled)
{
  poeReadDebugEnabled = enabled;

  return poeReadDebugEnabled;
}

int hpcPoeDebugFirmwareEnable(L7_uint32 enabled)
{
  poeFirmwareDebugEnabled = enabled;

  return poeFirmwareDebugEnabled;
}

int hpcPoeDebugMonitorEnable(L7_BOOL enabled)
{
  poeMonitorEnabled = enabled;

  return poeMonitorEnabled;
}

int hpcPoeDebugMonitorDebugEnable(L7_BOOL enabled)
{
  poeMonitorDebugEnabled = enabled;

  return poeMonitorDebugEnabled;
}

/*********************************************************************
 *
 * @purpose Debug Message for PoE counters
 *
 *
 * @returns  L7_SUCCESS, if success
 * @returns  L7_FAILURE, if other failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
int hpcPoeDebugDump(void)
{
  printf("      poe_tx_retry_count == %d\r\n", poe_tx_retry_count);
  printf("poe_tx_no_response_count == %d\r\n", poe_tx_no_response_count);
  printf("  poe_bad_checksum_count == %d\r\n", poe_bad_checksum_count);
  printf("      poe_bad_echo_count == %d\r\n", poe_bad_echo_count);

  return 0;
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
void hpcPwrdsinePoeDebugSysDump(L7_uint32 cardIndex)
{
  L7_uchar8   hw_ver;
  L7_ushort16 sw_ver;
  L7_uchar8   build_num;

  /* System Status */
  L7_uchar8   boot_error;
  L7_uchar8   kernel_error;
  L7_uchar8   kernel_status;
  L7_uchar8   internal_error;
  L7_uchar8   private_label;
  L7_uchar8   user_byte;
  L7_uchar8   deviceFail;
  L7_uchar8   tempDisconnect;
  L7_uchar8   tempAlarm;

  printf("Hardware Status for the PoE Card 0x%x\r\n", cardIndex);

  if (L7_SUCCESS == hpcPwrdsinePoeMsgSoftwareVersionGet(cardIndex, &hw_ver,
                                                        &sw_ver,
                                                        &build_num))
  {
    printf("\r\nSW Version: %4d_%-2d\r\n", sw_ver, build_num);
  }
  else
  {
    printf("Could not read SW Version.\r\n");
  }

  if (L7_SUCCESS == hpcPwrdsinePoeMsgSystemStatusGet(cardIndex, &boot_error,
                                                     &kernel_error,
                                                     &kernel_status,
                                                     &internal_error,
                                                     &private_label,
                                                     &user_byte,
                                                     &deviceFail,
                                                     &tempDisconnect,
                                                     &tempAlarm))
  {
    printf("\r\nSystem Status:\r\n");
    printf("     boot_error: 0x%2.2x\r\n", boot_error);
    printf("   kernel_error: 0x%2.2x\r\n", kernel_error);
    printf("  kernel_status: 0x%2.2x\r\n", kernel_status);
    printf(" internal_error: 0x%2.2x\r\n", internal_error);
    printf("  private_label: 0x%2.2x\r\n", private_label);
    printf("      user_byte: 0x%2.2x\r\n", user_byte);
    printf("    device fail: 0x%2.2x\r\n", deviceFail);
    printf("temp disconnect: 0x%2.2x\r\n", tempDisconnect);
    printf("     temp alarm: 0x%2.2x\r\n", tempAlarm);
  }
  else
  {
    printf("Could not read system status.\r\n");
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
int hpcPwrdsinePoeDebugHwDump(void)
{
  /* Port Status */
  L7_uchar8   on_off;
  L7_uchar8   status;
  L7_uchar8   auto_test;
  L7_uchar8   latch;
  L7_uchar8   power_class;

  /* Mask Status */
  L7_uchar8   mask;

  /* Main Power Source Supply Parameters */
  L7_ushort16 power_consumption;
  L7_ushort16 max_shutdown_voltage;
  L7_ushort16 min_shutdown_voltage;
  L7_uchar8   powerBank;
  L7_ushort16 max_power_available;

  /* Main Supply Parameters */
  L7_ushort16 vmain_voltage;

  /* Port Power Settings */
  L7_ushort16 power;

  /* UDL counters */
  L7_uchar8   UDLcounters[6];

  /* Port Parameter Measurements */
  L7_ushort16 voltage;
  L7_ushort16 current;

  /* Serial Number */
  L7_uchar8   num_channels;
  L7_uchar8   subcontractor;
  L7_uchar8   year;
  L7_uchar8   week;
  L7_ushort16 part_num;
  L7_uint32   serial_num;

  /* Port Status Info */
  L7_uint32   on_off_status_24port;
  L7_uint32   on_off_status_48port;

  /* Port Priority */
  L7_uchar8   priority;

  L7_uchar8   channel;
  L7_RC_t     rc;

  L7_uint32   i;

  HPC_POE_CARD_DATA_t *cardData;
  L7_uint32   cardIndex;
  L7_uint32   start_port;
  L7_uint32   max_port;
  L7_uint32   num_banks;
  L7_uchar8   start_bank;
  L7_uchar8   end_bank;

  L7_ushort16 deviceVersion;
  L7_uchar8   asicStatus;
  L7_uchar8   portsExpected;
  L7_uchar8   portsVerified;
  L7_uchar8   portsAllocated;
  L7_uchar8   temperature;
  L7_uchar8   tempSwitchHigh;
  L7_uchar8   midPowerData;
  L7_uchar8   commStatus;

  for (cardIndex = 0; cardIndex < hpcPoeCardCountGet(); cardIndex++)
  {
    cardData = hpcPoeCardDataGet(cardIndex);

    hpcPwrdsinePoeDebugSysDump(cardIndex);

    printf("\r\nSingle Port Status:\r\n");
    printf("Port on-off status auto-test latch power class\r\n");
    printf("---- ------ ------ --------- ----- -----------\r\n");

    start_port = 1;
    max_port = start_port + cardData->numOfPorts;

    for (channel = start_port; channel < max_port; channel++)
    {
      rc = hpcPwrdsinePoeMsgSinglePortStatusGet(cardIndex, channel,
                                                &on_off,
                                                &status,
                                                &auto_test,
                                                &latch,
                                                &power_class);

      if (rc == L7_SUCCESS)
      {
        printf("%02d   0x%2.2x   0x%2.2x   0x%2.2x      0x%2.2x  0x%2.2x\r\n",
               channel, on_off, status, auto_test, latch, power_class);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Single port status for POE port %02d", channel);
      }
    }

    rc = hpcPwrdsinePoeMsgMasksStatusGet(cardIndex, &mask);

    if (rc == L7_SUCCESS)
    {
      printf("\r\nMasks Status:\r\n");
      printf("\nIf Total Power Consumption is in Guard Band Range:\n");
      if (mask & 0x01)
      {
          printf("\nPM Mode: A new port attempting to connect is denied access.\n");
      }
      else
      {
          printf("\nPM Mode: Lowest Priority port is shutdown when a higher priority port tries to connect.\n");
      }
      if (mask & 0x02)
      {
          printf("\nLegacy Mode: Enabled, using Capacitor detection\n");
      }
      else
      {
          printf("\nLegacy Mode: Disabled\n");
      }
    }

    rc = hpcPwrdsinePoeMsgMainPowerSourceSupplyParmGet(cardIndex, &power_consumption,
                                                       &max_shutdown_voltage,
                                                       &min_shutdown_voltage,
                                                       &powerBank,
                                                       &max_power_available);

    if (rc == L7_SUCCESS)
    {
      printf("\r\nMain Power Source Supply Parameters:\r\n");
      printf("                  power_consumption: %-4.0d\r\n", power_consumption);
      printf("               max_shutdown_voltage: %-4.0d\r\n", max_shutdown_voltage);
      printf("               min_shutdown_voltage: %-4.0d\r\n", min_shutdown_voltage);
      printf("                          powerBank: %2.2d\r\n",  powerBank);
      printf("                max_power_available: %-4.0d\r\n", max_power_available);
    }

    rc = hpcPwrdsinePoeMsgMainSupplyVoltageGet(cardIndex, &vmain_voltage);
    if (rc == L7_SUCCESS)
    {
      printf("\r\nMain Supply Parameters: (in decivolts)\r\n");
      printf("         vmain_voltage: %-4.0d\r\n", vmain_voltage);
    }

    printf("\r\nPower Bank Settings:\r\n");
    printf("Bank    max shutdown voltage   min shutdown voltage   max power available\r\n");
    printf("----    --------------------   --------------------   -------------------\r\n");

    if (hpcPoeCardDbPowerBanksNumGet(cardIndex, &num_banks) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get number of Power Banks for cardId 0x%x", cardIndex);
      return L7_SUCCESS;
    }
    if (num_banks > 1)
    {
      start_bank = 1;
      end_bank   = num_banks;
    }
    else
    {
      start_bank = powerBank;
      end_bank   = powerBank;
    }

    for (i = start_bank; i <= end_bank; i++)
    {
      rc =  hpcPwrdsinePoeMsgPowerBanksGet(cardIndex, (L7_uchar8)i, &max_power_available, &max_shutdown_voltage, &min_shutdown_voltage);
      if (rc == L7_SUCCESS)
      {
        printf("%d       %-4.0d                   %-4.0d                   %-4.0d\r\n",
               i,
               max_shutdown_voltage,
               min_shutdown_voltage,
               max_power_available);
      }
    }

    printf("\r\nPort Power Settings:\r\n");
    printf("port power(W)    port power(W)\r\n");
    printf("---- --------    ---- --------\r\n");
    for (channel = start_port; channel < max_port; channel++)
    {
      rc = hpcPwrdsinePoeMsgPortPowerSettingsGet(cardIndex, channel, &power);
      if (rc == L7_SUCCESS)
      {
        printf("%-2d   %-5.3f       ",
               channel, power/1000.000);
        if (0 == ((channel - start_port + 1) % 2))
        {
          printf("\r\n");
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Power settings for POE port %02d", channel);
      }
    }
    printf("\r\n");

    printf("\r\nPort Parameter Measurements:\r\n");
    printf("port voltage(V) current(mA) power    port voltage(V) current(mA) power(mW)\r\n");
    printf("---- ---------- ----------- -----    ---- ---------- ----------  --------\r\n");

    for (channel = start_port; channel < max_port; channel++)
    {
      rc = hpcPwrdsinePoeMsgPortParmMeasurementGet(cardIndex, channel,
                                                   &voltage,
                                                   &current,
                                                   &power);
      if (rc == L7_SUCCESS)
      {
        printf("%02d   %-4.1f       %-4d        %-4d     ",
               channel, voltage/10.0f, current, power);
        if (0 == ((channel - start_port + 1) % 2))
        {
          printf("\r\n");
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Port Param measurements for POE port %02d", channel);
      }
    }

    rc = hpcPwrdsinePoeMsgSerialNumberGet(cardIndex, &num_channels,
                                          &subcontractor,
                                          &year,
                                          &week,
                                          &part_num,
                                          &serial_num);
    if (rc == L7_SUCCESS)
    {
      printf("\r\n\nSerial Number:\r\n");
      printf(" num_channels: %d\r\n", num_channels);
      printf("subcontractor: 0x%2.2x\r\n", subcontractor);
      printf("         year: 0x%2.2x\r\n", year);
      printf("         week: 0x%2.2x\r\n", week);
      printf("     part_num: 0x%4.4x (%04d)\r\n", part_num, part_num);
      printf("   serial_num: 0x%8.8x (%08d)\r\n", serial_num, serial_num);
    }

    printf("\r\nPoE Device Status:\r\n");
    printf("Device Version ASIC St PortsEx PortsVer PortsAll Temp TSH  MiPData CommSt\r\n");
    printf("------ ------- ------- ------- -------- -------- ---- ---- ------- ------\r\n");
    for (i = 0; i < 8; i++)
    {
      if (L7_SUCCESS == hpcPwrdsinePoeMsgDeviceStatusGet(cardIndex, i,
                                                         &deviceVersion,  &asicStatus,
                                                         &portsExpected,  &portsVerified,
                                                         &portsAllocated, &temperature,
                                                         &tempSwitchHigh, &midPowerData,
                                                         &commStatus))
      {
        printf(" %-5d 0x%4.4x  0x%2.2x    %-7d %-8d %-8d %3dC %3dC 0x%2.2x    0x%2.2x\r\n", i,
               deviceVersion,  asicStatus,
               portsExpected,  portsVerified,
               portsAllocated, temperature,
               tempSwitchHigh, midPowerData,
               commStatus);
      }
    }

    rc = hpcPwrdsinePoeMsgUDLCountersGet(cardIndex, UDLcounters, 1);
    if (rc == L7_SUCCESS)
    {
      printf("\r\nPoE UDL Counters Ports 1-24:\r\n");
      printf("1-4:   %02x\r\n5-8:   %02x\r\n9-12:  %02x\r\n13-16: %02x\r\n17-20: %02x\r\n21-24: %02x\r\n",
             UDLcounters[0], UDLcounters[1], UDLcounters[2], UDLcounters[3], UDLcounters[4], UDLcounters[5]);
    }
    rc = hpcPwrdsinePoeMsgUDLCountersGet(cardIndex, UDLcounters, 2);
    if (rc == L7_SUCCESS)
    {
      printf("\r\nPoE UDL Counters Ports 25-48:\r\n");
      printf("25-28: %02x\r\n29-32: %02x\r\n33-36: %02x\r\n37-40: %02x\r\n41-44: %02x\r\n45-48: %02x\r\n",
             UDLcounters[0], UDLcounters[1], UDLcounters[2], UDLcounters[3], UDLcounters[4], UDLcounters[5]);
    }

    rc = hpcPwrdsinePoeMsgPortStatusInfoGet(cardIndex, &on_off_status_24port, &on_off_status_48port);
    if (rc == L7_SUCCESS)
    {
      printf("\r\nPort Status Info:\r\n");
      printf("   on/off status ports1-8:   0x%2.2x\r\n", (on_off_status_24port & 0x7F100000) >> 23);
      printf("   on/off status ports7-16:  0x%2.2x\r\n", (on_off_status_24port & 0x007F1000) >> 15);
      printf("   on/off status ports17-24: 0x%2.2x\r\n", (on_off_status_24port & 0x00007F10) >> 7);

      printf("   on/off status ports25-32: 0x%2.2x\r\n", (on_off_status_48port & 0x7F100000) >> 23);
      printf("   on/off status ports33-40: 0x%2.2x\r\n", (on_off_status_48port & 0x007F1000) >> 15);
      printf("   on/off status ports41-47: 0x%2.2x\r\n", (on_off_status_48port & 0x00007F10) >> 7);
    }

    printf("\r\nPort Priority:\r\n");
    printf("port priority    port priority    port priority    port priority\r\n");
    printf("---- --------    ---- --------    ---- --------    ---- --------\r\n");
    for (channel = start_port; channel < max_port; channel++)
    {
      rc = hpcPwrdsinePoeMsgPortPriorityGet(cardIndex, channel, &priority);
      if (rc == L7_SUCCESS)
      {
        printf("%02d   0x%2.2x        ",
               channel, priority);
        if (0 == ((channel - start_port + 1) % 4))
        {
          printf("\r\n");
        }
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_POE_COMPONENT_ID, "Cannot get Port Priority for POE port %02d", channel);
      }
    }
  }

  return 0;
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
int hpcPwrdsinePoeDebugStatusDump(void)
{
  L7_uint i, j;

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

    for (j = 0; j < L7_MAX_POE_DEVICES_PER_CARD; j++)
    {
      printf("\r\nStatus for device %d:\r\n", j);
      if (L7_FALSE == poeGlobalStatus_g.cardStatus[i].deviceStatus[j].present)
      {
        printf("Not present.\r\n");
      }
      else
      {
        printf(" deviceVersion = 0x%4.4x\r\n", poeGlobalStatus_g.cardStatus[i].deviceStatus[j].deviceVersion);
        printf("    asicStatus = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].deviceStatus[j].asicStatus);
        printf(" portsExpected = %u\r\n",      poeGlobalStatus_g.cardStatus[i].deviceStatus[j].portsExpected);
        printf(" portsVerified = %u\r\n",      poeGlobalStatus_g.cardStatus[i].deviceStatus[j].portsVerified);
        printf("portsAllocated = %u\r\n",      poeGlobalStatus_g.cardStatus[i].deviceStatus[j].portsAllocated);
        printf("   temperature = %uC\r\n",     poeGlobalStatus_g.cardStatus[i].deviceStatus[j].temperature);
        printf("tempSwitchHigh = %uC\r\n",     poeGlobalStatus_g.cardStatus[i].deviceStatus[j].tempSwitchHigh);
        printf("  midPowerData = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].deviceStatus[j].midPowerData);
        printf("    commStatus = 0x%2.2x\r\n", poeGlobalStatus_g.cardStatus[i].deviceStatus[j].commStatus);
      }
    }
  }

  printf("\r\nPort Status:\r\n");
  printf("Port Power Voltage Current Enabled Status Latch Class Detect. PwrLimit PwrLimTmp Priority\r\n");
  printf("---- ----- ------- ------- ------- ------ ----- ----- ------- -------- --------- --------\r\n");
  for (i = 1; i < L7_POE_MAX_CHANNELS; i++)
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
  for (i = 1; i < L7_POE_MAX_CHANNELS; i++)
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
