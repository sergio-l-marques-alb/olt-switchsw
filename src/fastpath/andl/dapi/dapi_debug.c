/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi_debug.c
*
* @purpose   This file contains debug functions particular to the dapi layer.
*
* @component dapi
*
* @comments
*
* @create    4/10/2003
*
* @author    gator
*
* @end
*
**********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "l7_common.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "log.h"
#include "osapi.h"
#include "osapi_support.h"
#include "dapi.h"
#include "dapi_struct.h"
#include "dapi_debug.h"
#include "tlv_api.h"
#include "l7utils_api.h"

/* Globals */
extern DAPI_t *dapi_g;
extern DAPI_NAME_CONTAINER_t dapi_family_name_g[DAPI_NUM_OF_FAMILIES];

static char       *dapiDebugNameIgnoreSetGet[] = {"IGNORE","SET","GET"};
/*static char       dapiDebugNamePortMode[5][12] = {"NOT USED", "PHYSICAL", "CPU", "LOGICAL LAG", "VLAN ROUTER"};*/
/*static char       dapiDebugNamePortType[3][9]  = {"Internal", "FE", "GE"};*/
static char       *dapiDebugNameAddrFlag[] = {"STATIC", "LEARNED", "MANGEMENT", "SELF"};
/*static char       dapiDebugNameLif[5][12] = {"Unused", "Physical", "CPU", "LAG", "VLAN Router"};*/
static char       *dapiDebugNameGarpBehave[] = {"Forward", "Filter", "Dynamic"};
static char       *dapiDebugNameResult[] = {"Success", "Failure", "Error", "Not Implemented", "Not Supported",
                  "Does Not Exist", "Already Configured", "Table Full", "Request Denied", "Asynch Response"};

static L7_ulong32 dapiDebugMessageLevel    = 0;
static L7_ulong32 dapiDebugDecodeLevel     = 0;
static char       dapiDebugBuffer[DAPI_DEBUG_NUM_BUFFER_MSGS][DAPI_DEBUG_MSG_SIZE + 2];
static L7_int32   dapiDebugBufferHead      = 0;
static L7_int32   dapiDebugBufferTail      = 0;
void             *dapiDebugBufferSemaphore = L7_NULLPTR;
void             *dapiDebugQueue;
static void       dapiDebugTask(DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Initialize DAPI debug ability including semaphores, queues and tasks.
*
* @param   *dapi_g    @b{(input)}  The driver object
* @param    cmdDecode @b{(input)}  Flag indicating whether to initialize the command
*                                  decode message queue
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If DAPI debug is not enabled (DAPI_DEBUG_MSGLVL == DAPI_DEBUG_NONE), this function is not
*           called.
*
* @end
*
****************************************************************************************/
L7_RC_t dapiDebugInit(DAPI_t *dapi_g, L7_BOOL cmdDecode)
{
  if (cmdDecode == L7_TRUE)
  {
    dapiDebugMessageLevel = DAPI_DEBUG_INDICATIONS;
    dapiDebugDecodeLevel  = DAPI_DEBUG_DECODE;

    dapiDebugBufferHead   = 0;
    dapiDebugBufferTail   = 0;

    dapiDebugBufferSemaphore  = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

    if (dapiDebugBufferSemaphore == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    dapiDebugQueue = (void *)osapiMsgQueueCreate("dapiDebugQueue", DAPI_DEBUG_NUM_BUFFER_MSGS,
                                                 (L7_uint32)sizeof(L7_int32));
    if (dapiDebugQueue == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    if (osapiTaskCreate("DapiDebugTask", (void *)dapiDebugTask, 1 ,dapi_g, L7_DEFAULT_STACK_SIZE,
                        L7_DEFAULT_TASK_PRIORITY, L7_DEFAULT_TASK_SLICE) == L7_ERROR)
    {
      return L7_FAILURE;
    }
  }

  dapi_g->message_level = DAPI_DEBUG_MSGLVL_INIT;

  return L7_SUCCESS;
}

/****************************************************************************************
*
* @purpose  Intialize DAPI debug structures for a particular port.
*
* @param   *usp       @b{(input)}  The USP of the port that is to be set
* @param   *dapi_g    @b{(input)}  The driver object
*
* @returns  nothing
*
* @comments If DAPI command decode is not enabled (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS), this
*           function is not called.
*
* @end
*
****************************************************************************************/
void dapiDebugPortInit(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  DAPI_CMD_t    index;
  DAPI_FAMILY_t familyIndex;
  L7_uchar8     family_message_level[DAPI_NUM_OF_FAMILIES + 1] =
  {
    0,
    DAPI_DEBUG_MSGLVL_CONTROL,
    DAPI_DEBUG_MSGLVL_SYSTEM,
    DAPI_DEBUG_MSGLVL_FRAME,
    DAPI_DEBUG_MSGLVL_INTF,
    DAPI_DEBUG_MSGLVL_LOGICAL_INTF,
    DAPI_DEBUG_MSGLVL_LAG,
    DAPI_DEBUG_MSGLVL_ADDR,
    DAPI_DEBUG_MSGLVL_QVLAN,
    DAPI_DEBUG_MSGLVL_GARP,
    DAPI_DEBUG_MSGLVL_ROUTING_ROUTE,
    DAPI_DEBUG_MSGLVL_ROUTING_ARP,
    DAPI_DEBUG_MSGLVL_ROUTING_INTF,
    DAPI_DEBUG_MSGLVL_QOS_ACL,
    DAPI_DEBUG_MSGLVL_QOS_DIFFSERV,
    DAPI_DEBUG_MSGLVL_UNSPECIFIED
  };

  for (index = 0; index < DAPI_NUM_OF_CMDS; index++)
  {
    familyIndex = dapi_g->name->cmdToFamilyTable[index];
    /*
     * No matter what the setting, do not enable stats. It will swamp the system.
     */
    if (index == DAPI_CMD_INTF_STATISTICS)
    {
      dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[index] = DAPI_DEBUG_NONE;
    }
    else
    {
      dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[index] =
                      family_message_level[familyIndex];
    }
  }

  return;
}

/****************************************************************************************
*
* @purpose  Process all messages sent to the DAPI debug print queue.
*
* @param   *dapi_g    @b{(input)}  The driver object
*
* @returns  nothing
*
* @end
*
****************************************************************************************/
static void dapiDebugTask(DAPI_t *dapi_g)
{
  L7_int32 dapiDebugMsgNumber;

  while (1)
  {
    if (osapiMessageReceive(dapiDebugQueue, (void *)&dapiDebugMsgNumber, (L7_uint32)sizeof(L7_int32),
                            L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      DAPI_INFO_MSG(dapi_g, "Cannot dequeue DAPI debug message.\n");
      return;
    }

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, dapiDebugBuffer[dapiDebugMsgNumber]);

    /*
     * If the semaphore cannot be obtained, record the error for retrieval later.
     */
    if (osapiSemaTake(dapiDebugBufferSemaphore, L7_WAIT_FOREVER) != L7_SUCCESS) {
      L7_LOG_ERROR(DAPI_ERROR_GENERAL);
    }

    dapiDebugBufferTail = dapiDebugMsgNumber;

    /*
     * If the semaphore cannot be released, give the error message time to post.
     * Then log this particular error.
     */
    if (osapiSemaGive(dapiDebugBufferSemaphore) != L7_SUCCESS)
    {
      osapiSleep(5);
      L7_LOG_ERROR(DAPI_ERROR_GENERAL);
    }
  }
}

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is enabled to the appropriate extent, this function will
*           be called by various debug functions to add a debug message to the DAPI debug
*           print queue.
*
* @param   *format    @b{(input)}  The printf-like format string of the text destined
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the
*                                  format string. For each argument implied by the format
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by dapiMsgCmdDisp, dapiMsgCallbackDisp, & dapiDebugMsgLvl
*           to send a debug message to the DAPI debug print queue. Queueing is done because
*           many different tasks can be sending DAPI commands simultaneously. The
*           free-for-all method results in a jumble of messages particularly at startup.
*
* @end
*
****************************************************************************************/
void dapiDebugSendMsg(const char *format, ...)
{
  va_list arg;

  /*
   * If the semaphore cannot be obtained, record the error for retrieval later.
   */
  if (osapiSemaTake(dapiDebugBufferSemaphore, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    DAPI_INFO_MSG(dapi_g, "Cannot acquire DAPI debug semaphore.\n");
    return;
  }

  if (((dapiDebugBufferHead + 1) % DAPI_DEBUG_NUM_BUFFER_MSGS) != dapiDebugBufferTail)
  {
    /* Space exists */
    if (((dapiDebugBufferHead + 2) % DAPI_DEBUG_NUM_BUFFER_MSGS) == dapiDebugBufferTail)
    {
      /* last message, tag as lost message */
      osapiSnprintf(dapiDebugBuffer[dapiDebugBufferHead], sizeof(dapiDebugBuffer[0]), "DAPI debug messages were lost!");
    }
    else
    {
      /* space exists for this message */
                                                                      /*@ignore@*/
      memset(&arg, 0, sizeof(arg));
      va_start (arg, format);
                                                                      /*@end@*/

      osapiVsnprintf(dapiDebugBuffer[dapiDebugBufferHead], sizeof(dapiDebugBuffer[0]), format, arg);

      va_end (arg);
    }

    if (osapiMessageSend(dapiDebugQueue, (void *)&dapiDebugBufferHead, (L7_uint32)sizeof(L7_int32), L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      DAPI_INFO_MSG(dapi_g, "Cannot enqueue DAPI debug message #%d.\n", dapiDebugBufferHead);
      if (osapiSemaGive(dapiDebugBufferSemaphore) != L7_SUCCESS)
      {
        DAPI_INFO_MSG(dapi_g, "Cannot release DAPI debug semaphore.\n");
      }
      return;
    }

    dapiDebugBufferHead = (dapiDebugBufferHead + 1) % DAPI_DEBUG_NUM_BUFFER_MSGS;

  }

  /*
   * If the semaphore cannot be released, log this particular error.
   */
  if (osapiSemaGive(dapiDebugBufferSemaphore) != L7_SUCCESS)
  {
    DAPI_INFO_MSG(dapi_g, "Cannot release DAPI debug semaphore.\n");
    return;
  }

  return;
}


/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is enabled to the appropriate extent, this function will
*           be called by DAPI_INFO_MSG and will display the warning information to the
*           console. Formatting is from a string formed from a printf-like format string
*           and argument list.
*
* @param   *format    @b{(input)}  The printf-like format string of the text destined
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the
*                                  format string. For each argument implied by the format
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_INFO_MSG and is typically not called
*           by as a standalone function call.
*
* @end
*
****************************************************************************************/
void dapiInfoMsg(const char *format, ...)
{
  L7_uchar8     dapiInfoBuf[DAPI_DEBUG_INFO_MSG_SIZE];
  L7_int32 length;
                                                                      /*@ignore@*/
  va_list arg;

  memset(&arg, 0, sizeof(arg));
  va_start(arg, format);

  length = osapiVsnprintf((char *)dapiInfoBuf, sizeof(dapiInfoBuf), format, arg);

  va_end(arg);
                                                                      /*@end@*/

  /* if the buffer overflows, stack corruption has occurred. */
  if (length > DAPI_DEBUG_INFO_MSG_SIZE) {
    L7_LOG_ERROR(DAPI_ERROR_GENERAL|length);
  }

  dapiDebugSendMsg("%s", dapiInfoBuf);
#if 0
  if (osapiIntContext() == L7_TRUE)
  {
    /*
     * The sysapiPrintfQueueID queue only accepts "SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE"
     * length messages. The DAPI warning buffer allows longer messages, neatly trim.
     */
    if (SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE < DAPI_DEBUG_INFO_MSG_SIZE)
    {
      dapiInfoBuf[SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE-1] = 0;
    }
    sysapiPrintfMsgEnqueue(dapiInfoBuf);
  }
  else
  {
    printf("%s", (char *)dapiInfoBuf);
  }
#endif

  return;
}

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is enabled to the appropriate extent, this function will
*           be called by DAPI_WARN_MSG and will display the warning information to the
*           console as opposed to recoding in the event log. Formatting includes the file,
*           line number and a string formed from a printf-like format string and argument
*           list.
*
* @param   *fname     @b{(input)}  The filename recorded by the DAPI_WARN_MSG macro.
* @param    line      @b{(input)}  The line number recorded by the DAPI_WARN_MSG macro.
* @param   *format    @b{(input)}  The printf-like format string of the text destined
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the
*                                  format string. For each argument implied by the format
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_WARN_MSG and is typically not called
*           by as a standalone function call.
*
* @end
*
****************************************************************************************/
void dapiWarnMsg(const char *fname, L7_long32 line, const char *format, ...)
{
  L7_uchar8     dapiWarnBuf[DAPI_DEBUG_WARN_MSG_SIZE];
  L7_int32 length;
                                                                      /*@ignore@*/
  va_list  arg;

  l7utilsFilenameStrip((L7_char8 **)&fname);
  osapiSnprintf((char *)dapiWarnBuf, sizeof(dapiWarnBuf), "WARNING (%s: %ld) ", fname, line);
  length = (L7_int32)strlen((char *)dapiWarnBuf);

  memset(&arg, 0, sizeof(arg));
  va_start(arg, format);

  length = osapiVsnprintf((char *)&dapiWarnBuf[length], sizeof(dapiWarnBuf), format, arg);

  va_end(arg);
                                                                      /*@end@*/

  /* if the buffer overflows, stack corruption has occurred. */
  if (length > DAPI_DEBUG_WARN_MSG_SIZE) {
    L7_LOG_ERROR(DAPI_ERROR_GENERAL|length);
  }

  dapiDebugSendMsg("%s", dapiWarnBuf);
#if 0
  if (osapiIntContext() == L7_TRUE)
  {
    /*
     * The sysapiPrintfQueueID queue only accepts "SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE"
     * length messages. The DAPI warning buffer allows longer messages, neatly trim.
     */
    if (SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE < DAPI_DEBUG_WARN_MSG_SIZE)
    {
      dapiWarnBuf[SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE-1] = 0;
    }
    sysapiPrintfMsgEnqueue(dapiWarnBuf);
  }
  else
  {
    printf("%s", (char *)dapiWarnBuf);
  }
#endif

  return;
}

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is not enabled to the sufficient extent, this function
*           will be called by DAPI_WARN_MSG and will route the formatted warning
*           information to the message log. Formatting includes the file, line number and
*           a string formed from a printf-like format string and argument list.
*
* @param   *fname     @b{(input)}  The filename recorded by the DAPI_WARN_MSG macro.
* @param    line      @b{(input)}  The line number recorded by the DAPI_WARN_MSG macro.
* @param   *format    @b{(input)}  The printf-like format string of the text destined
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the
*                                  format string. For each argument implied by the format
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_WARN_MSG and is typically not called
*           by as a standalone function call.
*
* @end
*
****************************************************************************************/
void dapiLogMsg(const char *fname, L7_ulong32 line, const char *format, ...)
{
  char   dapiLogBuf[DAPI_DEBUG_MSG_SIZE];
  L7_int32    length;
                                                                      /*@ignore@*/
  va_list     arg;

  memset(&arg, 0, sizeof(arg));
  va_start(arg, format);

  length = osapiVsnprintf(dapiLogBuf, sizeof(dapiLogBuf), format, arg);

  va_end(arg);
                                                                      /*@end@*/

  if (length > DAPI_DEBUG_MSG_SIZE) {
    L7_LOG_ERROR(DAPI_ERROR_GENERAL|length);
  }

  /* If the log message is longer than the event log buffer, it will be trimmed */
  l7utilsFilenameStrip((L7_char8 **)&fname);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
          "(%s: %ld) %s\n", (char *)fname, line, dapiLogBuf);

  return;
}

/****************************************************************************************
*
* @purpose  Process each DAPI command and display decoded command info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command. For
*                               interface directed commands, this should be the port to
*                               act on. For non-port specific commands, this should be
*                               0.0.0.
* @param    cmd     @b{(input)} command to execute
* @param   *data    @b{(input)} data for the control function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCmdDisp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_ulong32  message_level = DAPI_DEBUG_NONE;

  if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_TRUE)
  {
    message_level = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[cmd];
  }
  /* slot not operational. Only callback register can be run at this time */
  else if (cmd == DAPI_CMD_CONTROL_CALLBACK_REGISTER)
  {
    message_level = DAPI_DEBUG_MSGLVL_CONTROL;
  }

  if (message_level >= dapiDebugMessageLevel)
  {
    dapiDebugSendMsg("\nDAPI: Command %4.40s issued to port %d.%d.%d\n",
                   dapi_g->name->cmd[cmd].name, usp->unit, usp->slot, usp->port);
  }

  if (message_level >= dapiDebugDecodeLevel)
  {
    DAPI_DEBUG_COMMAND_DECODE(cmd, data, dapi_g);
  }

  return;
}

/****************************************************************************************
*
* @purpose  Process each DAPI command and display decoded command info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command. For
*                               interface directed commands, this should be the port to
*                               act on. For non-port specific commands, this should be
*                               0.0.0.
* @param    cmd     @b{(input)} command to execute
* @param   *data    @b{(input)} data for the control function
* @param    result  @b{(input)} Command return value
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCmdRspDisp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, L7_RC_t result, DAPI_t *dapi_g)
{
  DAPI_UNDEFINED_CMD_t   *cmdUndef = (DAPI_UNDEFINED_CMD_t*)data;
  L7_ulong32              message_level = DAPI_DEBUG_NONE;

  if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_TRUE)
  {
    message_level = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[cmd];
  }
  /* slot not operational. Only callback register can be run at this time */
  else if (cmd == DAPI_CMD_CONTROL_CALLBACK_REGISTER)
  {
    message_level = DAPI_DEBUG_MSGLVL_CONTROL;
  }

  if (cmdUndef->cmdData.undefined.getOrSet==DAPI_CMD_GET)
  {
    if (message_level >= dapiDebugMessageLevel)
    {
      dapiDebugSendMsg("\nDAPI: Command Response %4.40s received = %d (%s)\n",
                     dapi_g->name->cmd[cmd].name, result, dapiDebugNameResult[result]);
    }

    if (message_level >= dapiDebugDecodeLevel)
    {
      DAPI_DEBUG_COMMAND_DECODE(cmd, data, dapi_g);
    }
  }

  else if (result != L7_SUCCESS)
  {
    if (message_level >= dapiDebugMessageLevel)
    {
      dapiDebugSendMsg("DAPI: Command Response = %d (%s)\n", result, dapiDebugNameResult[result]);
    }
  }

  return;
}

/****************************************************************************************
*
* @purpose  Display decoded command information.
*
* @param    cmd     @b{(input)} The command to execute
* @param   *cmdInfo @b{(input)} data for the command function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugCommandDecode(DAPI_CMD_t cmd, void *cmdInfo, DAPI_t *dapi_g)
{
  DAPI_CONTROL_CALLBACK_CMD_t   *cmdControlCb    = (DAPI_CONTROL_CALLBACK_CMD_t *)cmdInfo;
  DAPI_SYSTEM_CMD_t             *cmdSystem       = (DAPI_SYSTEM_CMD_t*)cmdInfo;
  DAPI_INTF_MGMT_CMD_t          *cmdIntfMgmt     = (DAPI_INTF_MGMT_CMD_t*)cmdInfo;
  DAPI_FRAME_CMD_t              *cmdTxInfo       = (DAPI_FRAME_CMD_t*)cmdInfo;
  DAPI_ADDR_MGMT_CMD_t          *cmdAddrMgmt     = (DAPI_ADDR_MGMT_CMD_t*)cmdInfo;
  DAPI_QVLAN_MGMT_CMD_t         *cmdVlanMgmt     = (DAPI_QVLAN_MGMT_CMD_t*)cmdInfo;
  DAPI_GARP_MGMT_CMD_t          *cmdGarpMgmt     = (DAPI_GARP_MGMT_CMD_t*)cmdInfo;
/*DAPI_LOGICAL_INTF_MGMT_CMD_t  *cmdLifMgmt      = (DAPI_LOGICAL_INTF_MGMT_CMD_t*)cmdInfo;*/
  DAPI_LAG_MGMT_CMD_t           *cmdLagMgmt      = (DAPI_LAG_MGMT_CMD_t*)cmdInfo;
  DAPI_ROUTING_MGMT_CMD_t       *cmdRtMgmt       = (DAPI_ROUTING_MGMT_CMD_t*)cmdInfo;
  DAPI_ROUTING_INTF_MGMT_CMD_t  *cmdRtIntfMgmt   = (DAPI_ROUTING_INTF_MGMT_CMD_t*)cmdInfo;
  DAPI_ROUTING_ARP_CMD_t        *cmdArpMgmt      = (DAPI_ROUTING_ARP_CMD_t*)cmdInfo;
  DAPI_QOS_CMD_t                *cmdQos          = (DAPI_QOS_CMD_t *)cmdInfo;
  DAPI_DOT1AD_MGMT_CMD_t          *cmdDot1adMgmt     = (DAPI_DOT1AD_MGMT_CMD_t*)cmdInfo;

  L7_uint32                      frameLength;
  L7_uchar8                     *frameDataPtr;
  L7_uint32                      blockHandle = L7_NULL;
  L7_int32                       index, jindex;
  L7_BOOL                        flag;
  L7_tlv_t                      *pTLV;
  char                          *str = L7_NULL;
  char                           buffer[SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE];

  switch (cmd)
  {
  case DAPI_CMD_CONTROL_CALLBACK_REGISTER:
    str = (char *)dapi_family_name_g[cmdControlCb->family].name;
    dapiDebugSendMsg("Family: %d (%s),  Function: 0x%08lX\n",
                  cmdControlCb->family, str, cmdControlCb->funcPtr);
    break;

  case DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.systemMacAddress.getOrSet],
                  MAC_EXPAND(cmdSystem->cmdData.systemMacAddress.macAddr.addr));
    break;

  case DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  IP: %d.%d.%d.%d\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.systemIpAddress.getOrSet],
                  IP_EXPAND(cmdSystem->cmdData.systemIpAddress.ipAddress));
    break;

  case DAPI_CMD_SYSTEM_MIRRORING:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  Probe Port: %d.%d.%d  Type: 0x%04X\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.mirroring.getOrSet],
                  cmdSystem->cmdData.mirroring.enable==L7_TRUE?"Enable":"Disable",
                  USP_EXPAND(&cmdSystem->cmdData.mirroring.probeUsp),
                  cmdSystem->cmdData.mirroring.probeType);
    if (cmdSystem->cmdData.mirroring.mirrorProbeUsp > 0)
    {
       dapiDebugSendMsg("      Source Entries: %d.%d.%d\n",
                       USP_EXPAND(&cmdSystem->cmdData.mirroring.mirrorProbeUsp[0].srcUsp));
    }
    for (index = 1; index < (int)cmdSystem->cmdData.mirroring.numMirrors; index++)
    {
      dapiDebugSendMsg("                      %d.%d.%d\n",
                       USP_EXPAND(&cmdSystem->cmdData.mirroring.mirrorProbeUsp[index].srcUsp));
    }

    break;

  case DAPI_CMD_SYSTEM_MIRRORING_PORT_MODIFY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  Mirrored Port: %d.%d.%d  Type: 0x%04X\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.mirroringPortModify.getOrSet],
                  cmdSystem->cmdData.mirroringPortModify.add==L7_TRUE?"Enable":"Disable",
                  USP_EXPAND(&cmdSystem->cmdData.mirroringPortModify.mirrorUsp),
                  cmdSystem->cmdData.mirroringPortModify.probeType);
    break;

  case DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.broadcastControl.getOrSet],
                  cmdSystem->cmdData.broadcastControl.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.broadcastControl.getOrSet],
                  cmdSystem->cmdData.broadcastControl.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_INTF_DOT1S_BPDU_FILTERING:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.bpduFiltering.getOrSet],
                  cmdIntfMgmt->cmdData.bpduFiltering.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_INTF_DOT1S_BPDU_GUARD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.bpduGuardMode.getOrSet],
                  cmdIntfMgmt->cmdData.bpduGuardMode.enable==L7_TRUE?"Enable":"Disable");

    break;
  case DAPI_CMD_INTF_DOT1S_BPDU_FLOOD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.bpduFlood.getOrSet],
                  cmdIntfMgmt->cmdData.bpduFlood.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_SYSTEM_FLOW_CONTROL:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.flowControl.getOrSet],
                  cmdSystem->cmdData.flowControl.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_SYSTEM_SNOOP_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s) %d  %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.snoopConfig.getOrSet],
                  cmdSystem->cmdData.snoopConfig.family == L7_AF_INET?"IGMP Snooping":"MLD Snooping",
                  cmdSystem->cmdData.snoopConfig.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE:
  case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Instance: %d\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dot1sInstanceCreate.getOrSet],
                  cmdSystem->cmdData.dot1sInstanceCreate.instNumber);
    break;

  case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Instance: %d  VLAN Id: %d\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dot1sInstanceCreate.getOrSet],
                  cmdSystem->cmdData.dot1sInstanceVlanAdd.instNumber, cmdSystem->cmdData.dot1sInstanceVlanAdd.vlanId);
    break;

  case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Instance: %d  VLAN Id: %d\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dot1sInstanceCreate.getOrSet],
                  cmdSystem->cmdData.dot1sInstanceVlanRemove.instNumber, cmdSystem->cmdData.dot1sInstanceVlanRemove.vlanId);
    break;


  case DAPI_CMD_SYSTEM_DOT1X_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dot1xConfig.getOrSet],
                  cmdSystem->cmdData.dot1xConfig.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dosControlConfig.getOrSet],
                  cmdSystem->cmdData.dosControlConfig.enable==L7_TRUE?"Enable":"Disable");
    break;
 case DAPI_CMD_INTF_DOSCONTROL_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.dosControlConfig.getOrSet],
                  cmdIntfMgmt->cmdData.dosControlConfig.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_INTF_STP_STATE:
    switch (cmdIntfMgmt->cmdData.stpState.state)
    {
    case DAPI_PORT_STP_BLOCKED          : str="Blocking";                  break;
    case DAPI_PORT_STP_LISTENING        : str="Listening";                 break;
    case DAPI_PORT_STP_LEARNING         : str="Learning";                  break;
    case DAPI_PORT_STP_FORWARDING       : str="Forwarding";                break;
    case DAPI_PORT_STP_ADMIN_DISABLED   : str="Administratively Disabled"; break;
    case DAPI_PORT_STP_NOT_PARTICIPATING: str="Not Participating";         break;
    default                             : str="Unknown";                   break;
    }
    dapiDebugSendMsg("App: 0x%08lX  (%s)  State: %s (%d)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.stpState.getOrSet],
                  str, cmdIntfMgmt->cmdData.stpState.state);
    break;

  case DAPI_CMD_INTF_SPEED_CONFIG:
    switch (cmdIntfMgmt->cmdData.portSpeedConfig.speed)
    {
    case DAPI_PORT_AUTO_NEGOTIATE   : str="Auto";     break;
    case DAPI_PORT_SPEED_FE_10MBPS  : str="10 Mbps";  break;
    case DAPI_PORT_SPEED_FE_100MBPS : str="100 Mbps"; break;
    case DAPI_PORT_SPEED_GE_1GBPS   : str="1 Gbps";   break;
    case DAPI_PORT_SPEED_GE_2G5BPS  : str="2.5 Gbps"; break;    /* PTin added: Speed 2.5G */
    case DAPI_PORT_SPEED_GE_10GBPS  : str="10 Gbps";  break;
    case DAPI_PORT_SPEED_GE_40GBPS  : str="40 Gbps";  break;    /* PTin added: Speed 40G */
    case DAPI_PORT_SPEED_GE_100GBPS : str="100 Gbps"; break;    /* PTin added: Speed 100G */
    default                         : str="Unknown";  break;
    }
    (void)osapiStrncpy(buffer, str, sizeof(buffer));
    osapiSnprintf(&buffer[strlen(buffer)], sizeof(buffer)-strlen(buffer), " (%d) ", cmdIntfMgmt->cmdData.portSpeedConfig.speed);

    switch (cmdIntfMgmt->cmdData.portSpeedConfig.duplex)
    {
    case DAPI_PORT_DUPLEX_FULL      : str=" - Full";    break;
    case DAPI_PORT_DUPLEX_HALF      : str=" - Half";    break;
    default                         : str=" - Unknown"; break;
    }
    (void)osapiStrncat(buffer, str, sizeof(buffer)-strlen(buffer)-1);
    osapiSnprintf(&buffer[strlen(buffer)], sizeof(buffer)-strlen(buffer), " (%d) ", cmdIntfMgmt->cmdData.portSpeedConfig.duplex);

    dapiDebugSendMsg("App: 0x%08lX  (%s) %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.portSpeedConfig.getOrSet],
                  buffer);
    break;
  case DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s) 0x%x\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.autoNegotiateConfig.getOrSet],
                  cmdIntfMgmt->cmdData.autoNegotiateConfig.abilityMask);
    break;
  case DAPI_CMD_INTF_LOOPBACK_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.portLoopbackConfig.getOrSet],
                  cmdIntfMgmt->cmdData.portLoopbackConfig.loopMode==DAPI_PORT_LOOPBACK_NONE?"Disable":"MAC-PHY");
    break;
  case DAPI_CMD_INTF_ISOLATE_PHY_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.portIsolatePhyConfig.getOrSet],
                  cmdIntfMgmt->cmdData.portIsolatePhyConfig.enable==L7_TRUE?"Enable":"Disable");
    break;
  case DAPI_CMD_INTF_STATISTICS:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Type: %ld (%s)  ID: %d  Ptr: 0x%08lX\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.statistics.getOrSet],
                  cmdIntfMgmt->cmdData.statistics.type,
                  cmdIntfMgmt->cmdData.statistics.enable==L7_TRUE?"Enable":"Disable",
                  cmdIntfMgmt->cmdData.statistics.counterId,
                  cmdIntfMgmt->cmdData.statistics.valuePtr);
    break;
  case DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s \n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dhcpSnoopingConfig.getOrSet],
                  cmdIntfMgmt->cmdData.dhcpSnoopingConfig.enabled==L7_TRUE?"Enabled":"Disabled");
    break;
  case DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dynamicArpInspectionConfig.getOrSet],
                  cmdIntfMgmt->cmdData.dynamicArpInspectionConfig.trusted==L7_TRUE?"Trusted":"Untrusted");
    break;
  case DAPI_CMD_INTF_IPSG_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.ipsgEnable.getOrSet],
                  cmdIntfMgmt->cmdData.ipsgEnable.enabled==L7_TRUE?"Enabled":"Disabled");
    break;
  case DAPI_CMD_INTF_IPSG_STATS_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   Ptr: 0x%x\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.ipsgStats.getOrSet],
                  cmdIntfMgmt->cmdData.ipsgStats.droppedPackets);
    break;
  case DAPI_CMD_INTF_IPSG_CLIENT_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x IPv4: 0x%8.8x IPv6: %8.8x:%8.8x:%8.8x:%8.8x\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.ipsgClientAdd.getOrSet],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.macAddr.addr[5],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.ip4Addr,
                  cmdIntfMgmt->cmdData.ipsgClientAdd.ip6Addr.in6.addr32[0],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.ip6Addr.in6.addr32[1],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.ip6Addr.in6.addr32[2],
                  cmdIntfMgmt->cmdData.ipsgClientAdd.ip6Addr.in6.addr32[3]);
    break;
  case DAPI_CMD_INTF_IPSG_CLIENT_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   IPv4: 0x%8.8x IPv6: %8.8x:%8.8x:%8.8x:%8.8x\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.ipsgClientDelete.getOrSet],
                  cmdIntfMgmt->cmdData.ipsgClientDelete.ip4Addr,
                  cmdIntfMgmt->cmdData.ipsgClientDelete.ip6Addr.in6.addr32[0],
                  cmdIntfMgmt->cmdData.ipsgClientDelete.ip6Addr.in6.addr32[1],
                  cmdIntfMgmt->cmdData.ipsgClientDelete.ip6Addr.in6.addr32[2],
                  cmdIntfMgmt->cmdData.ipsgClientDelete.ip6Addr.in6.addr32[3]);
    break;
  case DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot3ahEnable.getOrSet],
                  cmdIntfMgmt->cmdData.dot3ahEnable.dot3ahRemLbEnabled==L7_TRUE?"Enabled":"Disabled");
    break;

  case DAPI_CMD_INTF_FLOW_CONTROL:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.flowControl.getOrSet],
                  cmdSystem->cmdData.flowControl.enable==L7_TRUE?"Enable":"Disable");
    break;
  case DAPI_CMD_INTF_CABLE_STATUS_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.cableStatusGet.getOrSet]);
    break;
  case DAPI_CMD_INTF_FIBER_DIAG_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.cableFiberDiag.getOrSet]);
    break;

  case DAPI_CMD_INTF_DOT1S_STATE:
    switch (cmdIntfMgmt->cmdData.dot1sState.state)
    {
    case DAPI_PORT_DOT1S_DISCARDING       : str="Discarding";                break;
    case DAPI_PORT_DOT1S_LEARNING         : str="Learing";                   break;
    case DAPI_PORT_DOT1S_FORWARDING       : str="Forwarding";                break;
    case DAPI_PORT_DOT1S_ADMIN_DISABLED   : str="Administratively Disabled"; break;
    case DAPI_PORT_DOT1S_NOT_PARTICIPATING: str="Not Participating";         break;
    }
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Instance: %d  State: %s (%d)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1sState.getOrSet],
                  cmdIntfMgmt->cmdData.dot1sState.instNumber, str, cmdIntfMgmt->cmdData.dot1sState.state);
    break;

  case DAPI_CMD_INTF_DOT1X_STATUS:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  (%d)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xStatus.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xStatus.status==L7_DOT1X_PORT_STATUS_AUTHORIZED?"Authorized":"Unauthorized",
                  cmdIntfMgmt->cmdData.dot1xStatus.status);
    break;

  case DAPI_CMD_INTF_DOT1X_CLIENT_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x VLAN: %d pTlv: 0x%x\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xClientAdd.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.macAddr.addr[5],
                  cmdIntfMgmt->cmdData.dot1xClientAdd.vlanId,
                  cmdIntfMgmt->cmdData.dot1xClientAdd.pTLV);
    break;

  case DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xClientRemove.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.dot1xClientRemove.macAddr.addr[5]);
    break;



  case DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT:
      dapiDebugSendMsg("App: 0x%08lX  (%s)  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x %d\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xClientTimeout.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.dot1xClientTimeout.macAddr.addr[5],
                       cmdIntfMgmt->cmdData.dot1xClientTimeout.timeout );
      break;

  case DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK:
      dapiDebugSendMsg("App: 0x%08lX  (%s)  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x %d\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xClientTimeout.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.macAddr.addr[5],
                  cmdIntfMgmt->cmdData.dot1xClientBlock.vlanId );
      break;

  case DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK:
      dapiDebugSendMsg("App: 0x%08lX  (%s)  %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x %d\n",
                  cmdIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.dot1xClientTimeout.getOrSet],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[0],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[1],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[2],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[3],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[4],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.macAddr.addr[5],
                  cmdIntfMgmt->cmdData.dot1xClientUnblock.vlanId );
      break;

  case DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  (0x%x) (0x%x)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.doubleVlanTagConfig.getOrSet],
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.enable==L7_TRUE ? "Enable" : "Disable",
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.etherType,
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.customerId);
    break;
  case DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  (0x%x) (0x%x)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.doubleVlanTagConfig.getOrSet],
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.enable==L7_TRUE ? "Enable" : "Disable",
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.etherType,
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.customerId);
    break;

  case DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s) (0x%x) \n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.doubleVlanTagConfig.getOrSet],
                  cmdIntfMgmt->cmdData.doubleVlanTagConfig.etherType
                  );
    break;

  case DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MaxFrameSize: (%d)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.frameSizeConfig.getOrSet],
                  cmdIntfMgmt->cmdData.frameSizeConfig.maxFrameSize);
    break;

  case DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  (%d)\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.captivePortalPortState.getOrSet], cmdIntfMgmt->cmdData.captivePortalPortState.cpState);
    break;

  case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD:
  case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY:
  case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MAC: %02X:%02X:%02X:%02X:%02X:%02X  vlan: %d  flags: 0x%X\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.macAddressEntryAdd.getOrSet],
                  MAC_EXPAND(cmdAddrMgmt->cmdData.macAddressEntryAdd.macAddr.addr), cmdAddrMgmt->cmdData.macAddressEntryAdd.vlanID,
                  cmdAddrMgmt->cmdData.macAddressEntryAdd.flags);
    break;

  case DAPI_CMD_ADDR_MAC_FILTER_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MAC: %02X:%02X:%02X:%02X:%02X:%02X  vlan: %d\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.macFilterAdd.getOrSet],
                  MAC_EXPAND(cmdAddrMgmt->cmdData.macFilterAdd.macAddr.addr), cmdAddrMgmt->cmdData.macFilterAdd.vlanID);
    if (cmdAddrMgmt->cmdData.macFilterAdd.numOfSrcPortEntries > 0)
    {
      dapiDebugSendMsg("      Source Entries: %d.%d.%d\n",
                       USP_EXPAND(&cmdAddrMgmt->cmdData.macFilterAdd.srcPorts[0]));
    }
    for (index = 0; index < (int)cmdAddrMgmt->cmdData.macFilterAdd.numOfSrcPortEntries; index++)
    {
      dapiDebugSendMsg("                      %d.%d.%d\n",
                       USP_EXPAND(&cmdAddrMgmt->cmdData.macFilterAdd.srcPorts[index]));
    }
    if (cmdAddrMgmt->cmdData.macFilterAdd.numOfDestPortEntries > 0)
    {
      dapiDebugSendMsg(" Destination Entries: %d.%d.%d\n",
                       USP_EXPAND(&cmdAddrMgmt->cmdData.macFilterAdd.destPorts[0]));
    }
    for (index = 0; index < (int)cmdAddrMgmt->cmdData.macFilterAdd.numOfDestPortEntries; index++)
    {
      dapiDebugSendMsg("                      %d.%d.%d\n",
                       USP_EXPAND(&cmdAddrMgmt->cmdData.macFilterAdd.destPorts[index]));
    }
    break;

  case DAPI_CMD_ADDR_MAC_FILTER_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MAC: %02X:%02X:%02X:%02X:%02X:%02X  vlan: %d\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.macFilterDelete.getOrSet],
                  MAC_EXPAND(cmdAddrMgmt->cmdData.macFilterDelete.macAddr.addr), cmdAddrMgmt->cmdData.macFilterDelete.vlanID);
    break;

  case DAPI_CMD_ADDR_AGING_TIME:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  vlan: %ld  age time: %ld(s)\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.agingTime.getOrSet],
                  cmdAddrMgmt->cmdData.agingTime.vlanID, cmdAddrMgmt->cmdData.agingTime.agingTime);
    break;

///* PTin added: MAC learning */
//case DAPI_CMD_ADDR_SET_LEARN_MODE:
//  dapiDebugSendMsg("App: 0x%08lX  (%s)  age time: %u\n",
//                cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.portAddressSetLearnMode.getOrSet],
//                cmdAddrMgmt->cmdData.portAddressSetLearnMode.learn_enabled);
//  break;
///* PTin end */

  case DAPI_CMD_ADDR_FLUSH:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.portAddressFlush.getOrSet]);
    break;

  case DAPI_CMD_ADDR_FLUSH_VLAN:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  vlan: %ld\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.portAddressFlushMac.getOrSet],
                  cmdAddrMgmt->cmdData.portAddressFlushVlan.vlanID );
    break;

  case DAPI_CMD_ADDR_FLUSH_MAC:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  cmdAddrMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdAddrMgmt->cmdData.portAddressFlushVlan.getOrSet],
                  MAC_EXPAND(cmdAddrMgmt->cmdData.macFilterAdd.macAddr.addr) );
    break;

  case DAPI_CMD_QVLAN_VLAN_CREATE:
  case DAPI_CMD_QVLAN_VLAN_PURGE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  vlan: %d\n",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.vlanCreate.getOrSet],
                  cmdVlanMgmt->cmdData.vlanCreate.vlanID);
    break;

  case DAPI_CMD_QVLAN_VLAN_PORT_ADD:
  case DAPI_CMD_QVLAN_VLAN_PORT_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  vlan: %d  members: %d  tagged members: %d\n",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.vlanPortAdd.getOrSet],
                  cmdVlanMgmt->cmdData.vlanPortAdd.vlanID,
                  cmdVlanMgmt->cmdData.vlanPortAdd.numOfMemberSetEntries,
                  cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries);

    if (cmdVlanMgmt->cmdData.vlanPortAdd.numOfMemberSetEntries > 0)
    {
      flag = L7_FALSE;
      if (cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries > 0)
      {
        for (jindex = 0; jindex < (int)cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries; jindex++)
        {
          if ((cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[0].unit == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].unit) &&
              (cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[0].slot == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].slot) &&
              (cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[0].port == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].port) )
          {
            flag = L7_TRUE;
            break;
          }
        }
      }
      dapiDebugSendMsg("        Members: %d.%d.%d %s\n",
                       USP_EXPAND(&cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[0]),
                       flag == L7_TRUE?"(tagged)":" ");
    }
    for (index = 0; index < (int)cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries; index++)
    {
      flag = L7_FALSE;
      if (cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries > 0)
      {
        for (jindex = 0; jindex < (int)cmdVlanMgmt->cmdData.vlanPortAdd.numOfTagSetEntries; jindex++)
        {
          if ((cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].unit == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].unit) &&
              (cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].slot == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].slot) &&
              (cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].port == cmdVlanMgmt->cmdData.vlanPortAdd.operationalTagSet[jindex].port) )
          {
            flag = L7_TRUE;
            break;
          }
        }
      }
      dapiDebugSendMsg("                 %d.%d.%d %s\n",
                       cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].unit,
                       cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].slot,
                       cmdVlanMgmt->cmdData.vlanPortAdd.operationalMemberSet[index].port,
                       flag == L7_TRUE?"(tagged)":" ");
    }

    break;

  case DAPI_CMD_QVLAN_PORT_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.portConfig.getOrSet]);
    if (cmdVlanMgmt->cmdData.portConfig.vlanIDModify == L7_TRUE)
    {
      dapiDebugSendMsg("vlan (pvid): %d  ", cmdVlanMgmt->cmdData.portConfig.vlanID);
    }
    if (cmdVlanMgmt->cmdData.portConfig.ingressFilteringEnabledModify == L7_TRUE)
    {
      dapiDebugSendMsg("ingress filter: %s  ", (cmdVlanMgmt->cmdData.portConfig.ingressFilteringEnabled==L7_TRUE)?"True":"False");
    }
    if (cmdVlanMgmt->cmdData.portConfig.acceptFrameTypeModify == L7_TRUE)
    {
      dapiDebugSendMsg("tagged only: %s  ", (cmdVlanMgmt->cmdData.portConfig.acceptFrameType==L7_TRUE)?"True":"False");
    }
    if (cmdVlanMgmt->cmdData.portConfig.protocolModify == L7_TRUE)
    {
      dapiDebugSendMsg("protocol pvids:\n");
      for (index=0; index<L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; index++)
      {
        dapiDebugSendMsg("  %d  ", cmdVlanMgmt->cmdData.portConfig.protocol[index]);
      }
    }
    dapiDebugSendMsg("\n");
    break;

  case DAPI_CMD_QVLAN_PBVLAN_CONFIG:
     dapiDebugSendMsg("App: 0x%08lX  (%s) protocolId %d protocolIndex %d\n",
                  cmdSystem->appHandle, dapiDebugNameIgnoreSetGet[cmdSystem->cmdData.pbVlanConfig.getOrSet],
                  cmdSystem->cmdData.pbVlanConfig.protoNum, cmdSystem->cmdData.pbVlanConfig.protoIndex);
    break;


  case DAPI_CMD_QVLAN_PORT_PRIORITY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  priority: %ld\n",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.portPriority.getOrSet],
                  cmdVlanMgmt->cmdData.portPriority.priority);
    break;

  case DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  priority: %ld  Traffic Class:%ld\n",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.portPriorityToTcMap.getOrSet],
                  cmdVlanMgmt->cmdData.portPriorityToTcMap.priority, cmdVlanMgmt->cmdData.portPriorityToTcMap.traffic_class);
    break;

  case DAPI_CMD_QVLAN_VLAN_STATS_GET:
    switch (cmdVlanMgmt->cmdData.vlanStatsGet.statsType)
    {
    case DAPI_STATS_VLAN_IN_FRAMES:          str = "In Frames"         ; break;
    case DAPI_STATS_VLAN_IN_DISCARDS:        str = "In Discards"       ; break;
    case DAPI_STATS_VLAN_POLICING_DISCARDS:  str = "Policing Discards" ; break;
    case DAPI_STATS_VLAN_THRESHOLD_DISCARDS: str = "Threshold Discards"; break;
    case DAPI_STATS_VLAN_OUT_FRAMES:         str = "Out Frames"        ; break;
    default:                                 str = "Undefined"         ;
    }
    dapiDebugSendMsg("App: 0x%08lX  (%s)  vlan: %d  port: %d.%d.%d  Stat Type: %s  Stat Counter: %p\n",
                  cmdVlanMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdVlanMgmt->cmdData.vlanStatsGet.getOrSet],
                  cmdVlanMgmt->cmdData.vlanStatsGet.vlanID, USP_EXPAND(&cmdVlanMgmt->cmdData.vlanStatsGet.portUsp[0]),
                  str, cmdVlanMgmt->cmdData.vlanStatsGet.statsCounter);
    break;

  case DAPI_CMD_GARP_GVRP:
  case DAPI_CMD_GARP_GMRP:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Port %s\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdGarpMgmt->cmdData.portGvrp.getOrSet],
                      cmdGarpMgmt->cmdData.portGvrp.enable==L7_TRUE?"enable":"disable");
    break;

  case DAPI_CMD_GVRP_GMRP_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  System %s%s %s\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdGarpMgmt->cmdData.portGvrp.getOrSet],
                      cmdGarpMgmt->cmdData.gvrpGmrpConfig.gmrp==L7_TRUE?" GMRP":" ",
                      cmdGarpMgmt->cmdData.gvrpGmrpConfig.gvrp==L7_TRUE?"GVRP ":" ",
                      cmdGarpMgmt->cmdData.gvrpGmrpConfig.enable==L7_TRUE?"enable":"disable");
    break;

  case DAPI_CMD_GARP_GROUP_REG_MODIFY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  VLAN:%d  MAC Address: %02X:%02X:%02X:%02X:%02X:%02X ",
                     cmdLagMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdGarpMgmt->cmdData.groupRegModify.getOrSet],
                     cmdGarpMgmt->cmdData.groupRegModify.isStatic==L7_TRUE?"Static":"Dynamic",
                     cmdGarpMgmt->cmdData.groupRegModify.vlanId,
                     MAC_EXPAND(cmdGarpMgmt->cmdData.groupRegModify.grpMacAddr));

    dapiDebugSendMsg("%8s\n   Number of ports: %2d",
                     dapiDebugNameGarpBehave[*cmdGarpMgmt->cmdData.groupRegModify.stateInfo],
                     cmdGarpMgmt->cmdData.groupRegModify.numOfPorts);

    if (cmdGarpMgmt->cmdData.groupRegModify.numOfPorts > 0)
    {
        dapiDebugSendMsg("  portlist: %d.%d.%d\n", USP_EXPAND(&cmdGarpMgmt->cmdData.groupRegModify.ports[0]));
    }
    for (index = 0; index < (int)cmdAddrMgmt->cmdData.macFilterAdd.numOfSrcPortEntries; index++)
    {
      dapiDebugSendMsg("                                   %d.%d.%d\n",
                       USP_EXPAND(&cmdGarpMgmt->cmdData.groupRegModify.ports[index]));
    }
    break;

  case DAPI_CMD_GARP_GROUP_REG_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  VLAN:%d  MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                     cmdLagMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdGarpMgmt->cmdData.groupRegDelete.getOrSet],
                     cmdGarpMgmt->cmdData.groupRegDelete.isStatic==L7_TRUE?"Static":"Dynamic",
                     cmdGarpMgmt->cmdData.groupRegDelete.vlanId,
                     MAC_EXPAND(cmdGarpMgmt->cmdData.groupRegDelete.grpMacAddr));
    break;

  case DAPI_CMD_LOGICAL_INTF_CREATE:
  case DAPI_CMD_LOGICAL_INTF_DELETE:
    /* commentted out smanders stacking mode has changed to type
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  (%d)\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdLifMgmt->cmdData.logicalIntfCreate.getOrSet],
                      dapiDebugNameLif[cmdLifMgmt->cmdData.logicalIntfCreate.mode],
                      cmdLifMgmt->cmdData.logicalIntfCreate.mode);
                      */
    break;

  case DAPI_CMD_LAG_CREATE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  No Parms.\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdLagMgmt->cmdData.lagCreate.getOrSet]);
    break;

  case DAPI_CMD_LAG_PORT_ADD:
  case DAPI_CMD_LAG_PORT_DELETE:
  /* PTin added */
  case DAPI_CMD_INTERNAL_LAG_PORT_ADD:
  case DAPI_CMD_INTERNAL_LAG_PORT_DELETE:
  /* End of PTin added */
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Number of members %d.\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdLagMgmt->cmdData.lagPortDelete.getOrSet],
                      cmdLagMgmt->cmdData.lagPortDelete.numOfMembers);
    for (index=0; index<(int)cmdLagMgmt->cmdData.lagPortDelete.numOfMembers; index++)
    {
      dapiDebugSendMsg("     %d.%d.%d\n", USP_EXPAND(&cmdLagMgmt->cmdData.lagPortDelete.memberSet[index]));
    }

    break;

  case DAPI_CMD_LAG_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  No Parms.\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdLagMgmt->cmdData.lagDelete.getOrSet]);
    break;

  case DAPI_CMD_AD_TRUNK_MODE_SET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdLagMgmt->cmdData.portADTrunk.getOrSet],
                      cmdLagMgmt->cmdData.portADTrunk.enable==L7_TRUE?"Enable":"Disable");
    break;

  case DAPI_CMD_LAG_HASHMODE_SET:
      dapiDebugSendMsg("App: 0x%08lX  (%s)  Port Selection Criteria :%d\n", cmdLagMgmt->appHandle,
                       dapiDebugNameIgnoreSetGet[cmdLagMgmt->cmdData.lagHashMode.getOrSet],
                       cmdLagMgmt->cmdData.lagHashMode.hashMode);
    break;

  case DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD:
  case DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  mask: %d.%d.%d.%d  flags: 0x%02X\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.routeAdd.getOrSet],
                  IP_EXPAND(cmdRtMgmt->cmdData.routeAdd.ipAddr), IP_EXPAND(cmdRtMgmt->cmdData.routeAdd.ipMask),
                  cmdRtMgmt->cmdData.routeAdd.flags);
    dapiDebugSendMsg("                  ECMP  ip[0]: %d.%d.%d.%d  usp[0]: %d.%d.%d  valid[0]: %s\n",
                  IP_EXPAND(cmdRtMgmt->cmdData.routeAdd.route.equalCostRoute[0].ipAddr),
                  USP_EXPAND(&cmdRtMgmt->cmdData.routeAdd.route.equalCostRoute[0].usp),
                  cmdRtMgmt->cmdData.routeAdd.route.equalCostRoute[0].valid == L7_FALSE?"FALSE":"TRUE");
    break;

  case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  mask: %d.%d.%d.%d\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.routeDelete.getOrSet],
                  IP_EXPAND(cmdRtMgmt->cmdData.routeDelete.ipAddr), IP_EXPAND(cmdRtMgmt->cmdData.routeDelete.ipMask));
    break;

  case DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.forwardingConfig.getOrSet],
                  (cmdRtMgmt->cmdData.forwardingConfig.enable==L7_FALSE)?"Disable":"Enable");
    break;

  case DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s  server: %d.%d.%d.%d\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.bootpDhcpConfig.getOrSet],
                  (cmdRtMgmt->cmdData.bootpDhcpConfig.enable==L7_FALSE)?"Disable":"Enable",
                  IP_EXPAND(cmdRtMgmt->cmdData.bootpDhcpConfig.bootpDhcpRelayServer));
    break;

  case DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.icmpRedirectsConfig.getOrSet],
                  (cmdRtMgmt->cmdData.icmpRedirectsConfig.enable==L7_FALSE)?"Disable":"Enable");
    break;

  case DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.mcastforwardConfig.getOrSet],
                  (cmdRtMgmt->cmdData.mcastforwardConfig.enable==L7_FALSE)?"Disable":"Enable");
    break;

  case DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdRtMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtMgmt->cmdData.mcastIgmpConfig.getOrSet],
                  (cmdRtMgmt->cmdData.mcastIgmpConfig.enable==L7_FALSE)?"Disable":"Enable");
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY:
    dapiDebugSendMsg("App: 0x%08lX  \n", cmdRtMgmt->appHandle);
    break;
  case DAPI_CMD_ROUTING_ARP_ENTRY_ADD:
  case DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  flags: 0x%02X\n",
                  cmdArpMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdArpMgmt->cmdData.arpAdd.getOrSet],
                  IP_EXPAND(cmdArpMgmt->cmdData.arpAdd.ipAddr), cmdArpMgmt->cmdData.arpAdd.flags);
    dapiDebugSendMsg("     MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  MAC_EXPAND(cmdArpMgmt->cmdData.arpAdd.macAddr.addr));
    break;

  case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  flags: 0x%02X\n",
                  cmdArpMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdArpMgmt->cmdData.arpDelete.getOrSet],
                  IP_EXPAND(cmdArpMgmt->cmdData.arpDelete.ipAddr), cmdArpMgmt->cmdData.arpDelete.flags);
    dapiDebugSendMsg("     MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  MAC_EXPAND(cmdArpMgmt->cmdData.arpDelete.macAddr.addr));
    break;

  case DAPI_CMD_ROUTING_ARP_ENTRY_QUERY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n",
                  cmdArpMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdArpMgmt->cmdData.arpQuery.getOrSet]);

    break;

  case DAPI_CMD_ROUTING_INTF_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  mask: %d.%d.%d.%d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.rtrIntfAdd.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.ipAddr),
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.ipMask));

    dapiDebugSendMsg("  MAC: %02X:%02X:%02X:%02X:%02X:%02X  vlan: %d  mtu: %ld  Encaps: %s  Bcast: %s\n",
                  MAC_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.macAddr.addr),
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.vlanID,
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.mtu,
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.encapsType==DAPI_ROUTING_INTF_ENCAPS_TYPE_SNAP?"SNAP":"ETHERNET",
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.broadcastCapable==L7_FALSE?"False":"True");
    break;

  case DAPI_CMD_ROUTING_INTF_MODIFY:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  mask: %d.%d.%d.%d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.rtrIntfAdd.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.ipAddr),
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.ipMask));
    dapiDebugSendMsg("  MAC: %02X:%02X:%02X:%02X:%02X:%02X  vlan: %d  mtu: %ld  Encapsulation: %s  Bcast: %s\n",
                  MAC_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfAdd.macAddr.addr),
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.vlanID,
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.mtu,
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.encapsType==DAPI_ROUTING_INTF_ENCAPS_TYPE_SNAP?"SNAP":"ETHERNET",
                  cmdRtIntfMgmt->cmdData.rtrIntfAdd.broadcastCapable==L7_FALSE?"False":"True");
    break;

  case DAPI_CMD_ROUTING_INTF_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d  mask: %d.%d.%d.%d  vlan: %d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.rtrIntfDelete.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfDelete.ipAddr),
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.rtrIntfDelete.ipMask),
                  cmdRtIntfMgmt->cmdData.rtrIntfDelete.vlanID);
    break;

  case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.localMcastAdd.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.localMcastAdd.ipAddr));
    break;

  case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  ip: %d.%d.%d.%d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.localMcastDelete.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.localMcastDelete.ipAddr));
    break;

  case DAPI_CMD_ROUTING_INTF_VRID_ADD:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  VRID: %d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.rtrIntfVRIDAdd.getOrSet],
                  cmdRtIntfMgmt->cmdData.rtrIntfVRIDAdd.vrID);
    break;

  case DAPI_CMD_ROUTING_INTF_VRID_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  VRID: %d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.rtrIntfVRIDDelete.getOrSet],
                  cmdRtIntfMgmt->cmdData.rtrIntfVRIDDelete.vrID);
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.mcastIntfFwdConfig.getOrSet],
                  cmdRtIntfMgmt->cmdData.mcastIntfFwdConfig.enable==L7_TRUE?"enable":"disable");
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_ADD:
  case DAPI_CMD_ROUTING_INTF_MCAST_DELETE:
  case DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET:
  case DAPI_CMD_ROUTING_INTF_MCAST_USE_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Group Addr: %d.%d.%d.%d,  Src IP Addr: %d.%d.%d.%d,\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.mcastAdd.getOrSet],
                  IP_EXPAND(cmdRtIntfMgmt->cmdData.mcastAdd.mcastGroupAddr.addr.ipv4.s_addr), IP_EXPAND(cmdRtIntfMgmt->cmdData.mcastAdd.srcIpAddr.addr.ipv4.s_addr));
    if (DAPI_CMD_ROUTING_INTF_MCAST_ADD == cmd)
    {
      dapiDebugSendMsg("  Src Match: %s, RPF Check: %s, RPF Check Type: %s, Fail Action: %s\n",
                    cmdRtIntfMgmt->cmdData.mcastAdd.matchSrcAddr==L7_TRUE?"True":"False",
                    cmdRtIntfMgmt->cmdData.mcastAdd.rpfCheckEnable==L7_TRUE?"True":"False",
                    cmdRtIntfMgmt->cmdData.mcastAdd.rpfType==DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH?"In I/F":"L3 lookup",
                    cmdRtIntfMgmt->cmdData.mcastAdd.rpfCheckFailAction==DAPI_MCAST_RPF_CHECK_FAIL_ACTION_DROP?"Drop":"2CPU");

      dapiDebugSendMsg("  Outgoing I/F present: %s\n", cmdRtIntfMgmt->cmdData.mcastAdd.outGoingIntfPresent==L7_TRUE?"True":"False");
      if ((L7_TRUE == cmdRtIntfMgmt->cmdData.mcastAdd.outGoingIntfPresent) &&
           (cmdRtIntfMgmt->cmdData.mcastAdd.outUspCount > 0))
      {
        dapiDebugSendMsg("  portlist: %d.%d.%d\n", USP_EXPAND(&cmdRtIntfMgmt->cmdData.mcastAdd.outUspList[0]));

        for (index = 1; index < (int)cmdRtIntfMgmt->cmdData.mcastAdd.outUspCount; index++)
        {
          dapiDebugSendMsg("                                   %d.%d.%d\n",
                           USP_EXPAND(&cmdRtIntfMgmt->cmdData.mcastAdd.outUspList[index]));
        }
      }
    }
    else if (DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET == cmd)
    {
      dapiDebugSendMsg("  Packet Count: %d  (%ssupported)\n", cmdRtIntfMgmt->cmdData.mcastCountQuery.packetCount,    cmdRtIntfMgmt->cmdData.mcastCountQuery.packetCountSupported==L7_TRUE?"":"Not ");
      dapiDebugSendMsg("  Packet Count: %d  (%ssupported)\n", cmdRtIntfMgmt->cmdData.mcastCountQuery.byteCount,      cmdRtIntfMgmt->cmdData.mcastCountQuery.byteCountSupported==L7_TRUE?"":"Not ");
      dapiDebugSendMsg("  Packet Count: %d  (%ssupported)\n", cmdRtIntfMgmt->cmdData.mcastCountQuery.wrongIntfCount, cmdRtIntfMgmt->cmdData.mcastCountQuery.wrongIntfCountSupported==L7_TRUE?"":"Not ");
    }
    else if (DAPI_CMD_ROUTING_INTF_MCAST_USE_GET == cmd)
    {
      dapiDebugSendMsg("  Entry used: %s\n", cmdRtIntfMgmt->cmdData.mcastUseQuery.entryUsed==L7_TRUE?"True":"False");
    }
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD:
  case DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Group Addr: 0x%08X, IP Addr: %d.%d.%d.%d, ",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.mcastPortAdd.getOrSet],
                  cmdRtIntfMgmt->cmdData.mcastPortAdd.mcastGroupAddr.addr.ipv4.s_addr, IP_EXPAND(cmdRtIntfMgmt->cmdData.mcastPortAdd.srcIpAddr.addr.ipv4.s_addr));
    dapiDebugSendMsg("Src Match: %s, VLAN Tag: %s, Port: %d.%d.%d\n",
                      cmdRtIntfMgmt->cmdData.mcastPortAdd.matchSrcAddr==L7_TRUE?"True":"False",
                      cmdRtIntfMgmt->cmdData.mcastPortAdd.vlanTagging==L7_TRUE?"True":"False",
                      USP_EXPAND(&cmdRtIntfMgmt->cmdData.mcastPortAdd.outPortUsp));
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Port %d.%d.%d %s participation, tagging %s\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.mcastVlanPortMemberUpdate.getOrSet],
                  USP_EXPAND(&cmdRtIntfMgmt->cmdData.mcastVlanPortMemberUpdate.outPortUsp),
                  cmdRtIntfMgmt->cmdData.mcastVlanPortMemberUpdate.bIsMember==L7_TRUE?"Fixed":"Other",
                  cmdRtIntfMgmt->cmdData.mcastVlanPortMemberUpdate.vlanTagging==L7_TRUE?"on":"off");
    break;

  case DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  TTL Value: %d\n",
                  cmdRtIntfMgmt->appHandle, dapiDebugNameIgnoreSetGet[cmdRtIntfMgmt->cmdData.ttlMcastVal.getOrSet],
                  cmdRtIntfMgmt->cmdData.ttlMcastVal.ttlVal);
    break;

  case DAPI_CMD_FRAME_SEND:
    sysapiNetMbufGetNextBuffer(cmdTxInfo->cmdData.send.frameHdl, &blockHandle, &frameDataPtr, &frameLength);
    frameLength   =  sysapiNetMbufGetFrameLength(cmdTxInfo->cmdData.send.frameHdl);
    switch (cmdTxInfo->cmdData.send.type)
    {
    case DAPI_FRAME_TYPE_DATA_TO_PORT:
      (void)osapiStrncpy(buffer, "UNICAST TO PORT", sizeof(buffer));
      break;
    case DAPI_FRAME_TYPE_MCAST_DOMAIN:
      (void)osapiStrncpy(buffer, "MULTICAST DOMAIN", sizeof(buffer));
      break;
    case DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT:
      (void)osapiStrncpy(buffer, "NO_FILTER_UNICAST", sizeof(buffer));
      break;
    case DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN:
      (void)osapiStrncpy(buffer, "NO_FILTER_MULTICAST", sizeof(buffer));
      break;
    default:
      (void)osapiStrncpy(buffer, "undefined", sizeof(buffer));
    }
    dapiDebugSendMsg("App: 0x%08lX  length: %d  vlan: %d  pri: %d  type: %s\n",
                  cmdTxInfo->appHandle, frameLength, cmdTxInfo->cmdData.send.vlanID, cmdTxInfo->cmdData.send.priority, buffer);
    if (frameDataPtr != L7_NULLPTR)
    {
    dapiDebugSendMsg("%02X%02X%02X%02X%02X%02X %02X%02X%02X%02X%02X%02X ",
                  MAC_EXPAND(frameDataPtr), MAC_EXPAND(&frameDataPtr[6]));
    dapiDebugSendMsg("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ",
                  MAC_EXPAND(&frameDataPtr[12]), MAC_EXPAND(&frameDataPtr[18]));
    dapiDebugSendMsg("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ...\n",
                  MAC_EXPAND(&frameDataPtr[24]), MAC_EXPAND(&frameDataPtr[30]));
    }
    break;

  case DAPI_CMD_QOS_DIFFSERV_INST_ADD:
  case DAPI_CMD_QOS_DIFFSERV_INST_DELETE:
  case DAPI_CMD_QOS_ACL_ADD:
  case DAPI_CMD_QOS_ACL_DELETE:
  case DAPI_CMD_QOS_SYS_ACL_ADD:
  case DAPI_CMD_QOS_SYS_ACL_DELETE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  %s Rule, size %ld\n", cmdQos->appHandle,
                     dapiDebugNameIgnoreSetGet[cmdQos->cmdData.aclInstAdd.getOrSet],
                     cmdQos->cmdData.aclInstAdd.ifDirection==DAPI_QOS_INTF_DIR_IN?"Ingress":"Egress",
                     cmdQos->cmdData.aclInstAdd.tlvTotalSize);
    pTLV = (L7_tlv_t *)&cmdQos->cmdData.aclInstAdd.pTLV;
    for (index=(L7_int32)cmdQos->cmdData.aclInstAdd.tlvTotalSize; index>0; )
    {
      dapiDebugSendMsg("    Type: 0x%04X  Length: %d  Value: 0x", osapiNtohl(pTLV->type), osapiNtohl(pTLV->length));
      str = (char *)pTLV->valueStart;
      for (jindex=(L7_int32)osapiNtohl(pTLV->length); jindex>0; jindex--)
      {
        dapiDebugSendMsg("%02X", *str);
      }
      index -= osapiNtohl(pTLV->length);
      index -= 4;                 /* account for type and length */
      dapiDebugSendMsg("\n");
    }
    break;

  case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET:
  case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Key: %d, Stat: %d, Value: 0x%08lX:%08lX\n", cmdQos->appHandle,
                     dapiDebugNameIgnoreSetGet[cmdQos->cmdData.aclInstAdd.getOrSet],
                     cmdQos->cmdData.diffServIntfStatInGet.instanceKey,
                     cmdQos->cmdData.diffServIntfStatInGet.statistic,
                     cmdQos->cmdData.diffServIntfStatInGet.value->high,
                     cmdQos->cmdData.diffServIntfStatInGet.value->low);
    break;

  case DAPI_CMD_QOS_ACL_RULE_COUNT_GET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Correlator: 0x%8.8x, Hit Count Ptr: 0x%8.8lx\n",
                     cmdQos->appHandle,
                     dapiDebugNameIgnoreSetGet[cmdQos->cmdData.aclRuleCountGet.getOrSet],
                     cmdQos->cmdData.aclRuleCountGet.correlator,
                     cmdQos->cmdData.aclRuleCountGet.hitCount);
    break;
  case DAPI_CMD_QOS_ACL_RULE_STATUS_SET:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Correlator: 0x%8.8x, Rule Status: %s",
                     cmdQos->appHandle,
                     dapiDebugNameIgnoreSetGet[cmdQos->cmdData.aclRuleStatusSet.getOrSet],
                     cmdQos->cmdData.aclRuleStatusSet.correlator,
                     cmdQos->cmdData.aclRuleStatusSet.status==L7_ACL_RULE_STATUS_ACTIVE?"Active":"Inactive");
  break;
  case DAPI_CMD_L2_DOT1AD_INTF_TYPE:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Port %s\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdDot1adMgmt->cmdData.dot1adIntfType.getOrSet],
                      cmdDot1adMgmt->cmdData.dot1adIntfType.intfType==DOT1AD_INTFERFACE_TYPE_NNI?"nni":"uni");
    break;

  case DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION:
    dapiDebugSendMsg("App: 0x%08lX  (%s)  Port %d\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdDot1adMgmt->cmdData.dot1adtunnelAction.getOrSet],
                      cmdDot1adMgmt->cmdData.dot1adtunnelAction.tunnelAction);
    break;

  case DAPI_CMD_L2_DOT1AD_INTF_STATS:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdDot1adMgmt->cmdData.dot1adtunnelAction.getOrSet]);
    break;

  case DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA:
    dapiDebugSendMsg("App: 0x%08lX  (%s)\n", cmdLagMgmt->appHandle,
                      dapiDebugNameIgnoreSetGet[cmdDot1adMgmt->cmdData.dot1adApplyIntfConfigData.getOrSet]);
    break;
  case DAPI_CMD_INTF_LLPF_CONFIG:
    dapiDebugSendMsg("App: 0x%08lX  (%s)   %s\n",
        cmdSystem->appHandle,
        dapiDebugNameIgnoreSetGet[cmdIntfMgmt->cmdData.llpfConfig.getOrSet],
        cmdIntfMgmt->cmdData.llpfConfig.enable==L7_TRUE?"Enable":"Disable");
    break;

  default:
    dapiDebugSendMsg("  Unrecognized Command.\n");
  }

  return;
}

/****************************************************************************************
*
* @purpose  Process each DAPI callback and display decoded callback info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the callback.
* @param    family  @b{(input)} The feature group
* @param    cmd     @b{(input)} The callback command to execute
* @param    event   @b{(input)}
* @param   *cmdInfo @b{(input)} data for the callback function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCallbackDisp(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *cmdInfo, DAPI_t *dapi_g)
{
  L7_ulong32        message_level = DAPI_DEBUG_NONE;

  if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_TRUE)
  {
    message_level = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[cmd];
  }
  /* slot not operational. Only callback register can be run at this time */
  else if (cmd == DAPI_CMD_CONTROL_CALLBACK_REGISTER)
  {
    message_level = DAPI_DEBUG_MSGLVL_CONTROL;
  }

  if (message_level >= dapiDebugMessageLevel)
  {
    dapiDebugSendMsg("\nDAPI: Callback %4.38s (%d) issued from port %d.%d.%d\n",
                   dapi_g->name->cmd[cmd].name, event, usp->unit, usp->slot, usp->port);
  }

  if (message_level >= dapiDebugDecodeLevel)
  {
    DAPI_DEBUG_CALLBACK_DECODE(cmd, event, cmdInfo, dapi_g);
  }

  return;
}

/****************************************************************************************
*
* @purpose  Process each DAPI callback and display decoded callback info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the callback.
* @param    family  @b{(input)} The feature group
* @param    cmd     @b{(input)} The callback command to execute
* @param    event   @b{(input)}
* @param   *cmdInfo @b{(input)} data for the callback function
* @param    result  @b{(input)} Command return value
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCallbackRspDisp(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event,
                                 void *cmdInfo, L7_RC_t result, DAPI_t *dapi_g)
{
  L7_ulong32        message_level = DAPI_DEBUG_NONE;

  if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_TRUE)
  {
    message_level = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->message_level[cmd];
  }
  /* slot not operational. Only callback register can be run at this time */
  else if (cmd == DAPI_CMD_CONTROL_CALLBACK_REGISTER)
  {
    message_level = DAPI_DEBUG_MSGLVL_CONTROL;
  }

  /* These callbacks are queries */
  if ( ((cmd == DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT) && (event == DAPI_EVENT_ROUTING_ARP_ENTRY_QUERY)) ||
       ((cmd == DAPI_CMD_INTF_UNSOLICITED_EVENT)        && (event == DAPI_EVENT_INTF_DOT1S_STATE_QUERY))    ||
       ((cmd == DAPI_CMD_QVLAN_UNSOLICITED_EVENT)       && (event == DAPI_EVENT_QVLAN_MEMBER_QUERY))        ||
       ((cmd == DAPI_CMD_ADDR_UNSOLICITED_EVENT)        && (event == DAPI_EVENT_ADDR_INTF_MAC_QUERY)) )
  {
    if (message_level >= dapiDebugMessageLevel)
    {
      dapiDebugSendMsg("\nDAPI: Callback Response %4.38s received = %d (%s)\n",
                     dapi_g->name->cmd[cmd].name, result, dapiDebugNameResult[result]);
    }

    if (message_level >= dapiDebugDecodeLevel)
    {
      DAPI_DEBUG_CALLBACK_DECODE(cmd, event, cmdInfo, dapi_g);
    }
  }

  else if (result != L7_SUCCESS)
  {
    if (message_level >= dapiDebugMessageLevel)
    {
      dapiDebugSendMsg("DAPI: Callback Response = %d (%s)\n", result, dapiDebugNameResult[result]);
    }
  }

  return;
}

/****************************************************************************************
*
* @purpose  Display decoded callback information.
*
* @param    cmd     @b{(input)} The callback command to execute
* @param   *cmdInfo @b{(input)} data for the callback function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugCallbackDecode(DAPI_CMD_t cmd, DAPI_EVENT_t event, void *cbInfo, DAPI_t *dapi_g)
{
  DAPI_ROUTING_ARP_CMD_t       *cbArpResolve = (DAPI_ROUTING_ARP_CMD_t *)cbInfo;
  DAPI_FRAME_CMD_t             *cbRxInfo     = (DAPI_FRAME_CMD_t *)cbInfo;
  DAPI_INTF_MGMT_CMD_t         *cbIntfInfo   = (DAPI_INTF_MGMT_CMD_t *)cbInfo;
  DAPI_ADDR_MGMT_CMD_t         *cbAddrInfo   = (DAPI_ADDR_MGMT_CMD_t *)cbInfo;
  DAPI_QVLAN_MGMT_CMD_t        *cbVlanInfo   = (DAPI_QVLAN_MGMT_CMD_t *)cbInfo;
  L7_uint32                     frameLength;
  L7_uchar8                    *frameDataPtr;
  L7_int32                      index;
  L7_BOOL                       cbEventFound = L7_TRUE;

  switch (cmd)
  {
  case DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT:
    if (event == DAPI_EVENT_ROUTING_ARP_ENTRY_QUERY)
    {
      dapiDebugSendMsg("App: 0x%08lX   ip: %d.%d.%d.%d  flags: 0x%02X\n", cbArpResolve->appHandle,
                       IP_EXPAND(cbArpResolve->cmdData.unsolArpResolve.ipAddr),
                       cbArpResolve->cmdData.unsolArpResolve.flags);
    }
    else
    {
      cbEventFound = L7_FALSE;
    }
    break;

  case DAPI_CMD_FRAME_UNSOLICITED_EVENT:
    if (event == DAPI_EVENT_FRAME_RX)
    {
      frameLength   =  sysapiNetMbufGetFrameLength(cbRxInfo->cmdData.receive.frameHdl);
      frameDataPtr  =  sysapiNetMbufGetDataStart(cbRxInfo->cmdData.receive.frameHdl);
      dapiDebugSendMsg("App: 0x%08lX  length: %d  vlan: %d\n", cbRxInfo->appHandle, frameLength,
                       cbRxInfo->cmdData.receive.vlanID);
      dapiDebugSendMsg("%02X%02X%02X%02X%02X%02X %02X%02X%02X%02X%02X%02X ",
                       MAC_EXPAND(frameDataPtr), MAC_EXPAND(&frameDataPtr[6]));
      dapiDebugSendMsg("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ",
                       MAC_EXPAND(&frameDataPtr[12]), MAC_EXPAND(&frameDataPtr[18]));
      dapiDebugSendMsg("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X ...\n",
                       MAC_EXPAND(&frameDataPtr[24]), MAC_EXPAND(&frameDataPtr[30]));
    }
    else
    {
      cbEventFound = L7_FALSE;
    }
    break;

  case DAPI_CMD_INTF_UNSOLICITED_EVENT:
    switch (event)
    {
    case DAPI_EVENT_INTF_CREATE:
      /* commentted out smanders stacking mode has changed to type
      dapiDebugSendMsg("App: 0x%08lX   Create Port   %s - %s.\n",
                    cbIntfInfo->appHandle,
                    dapiDebugNamePortMode[cbIntfInfo->cmdData.unsolPortCreate.mode],
                    dapiDebugNamePortType[cbIntfInfo->cmdData.unsolPortCreate.type]);
                    */
      break;

    case DAPI_EVENT_INTF_DESTROY:
      dapiDebugSendMsg("App: 0x%08lX   Destroy.\n", cbIntfInfo->appHandle);
      break;

    case DAPI_EVENT_INTF_LINK_DOWN:
      dapiDebugSendMsg("App: 0x%08lX   Link Down.\n", cbIntfInfo->appHandle);
      break;

    case DAPI_EVENT_INTF_LINK_UP:
      dapiDebugSendMsg("App: 0x%08lX   Link Up.\n", cbIntfInfo->appHandle);
      break;

    case DAPI_EVENT_INTF_DOT1S_STATE_QUERY:
      dapiDebugSendMsg("App: 0x%08lX   802.1s Query  Instance: %d  Flag: %d\n",
                    cbIntfInfo->appHandle, cbIntfInfo->cmdData.dot1sQuery.instNumber,
                    cbIntfInfo->cmdData.dot1sQuery.canApplyStateChange);
      break;

    default:
      cbEventFound = L7_FALSE;
      break;
    }
    break;

  case DAPI_CMD_QVLAN_UNSOLICITED_EVENT:
    if (event == DAPI_EVENT_QVLAN_MEMBER_QUERY)
    {
      dapiDebugSendMsg("App: 0x%08lX  Number of Vlans: %d\n", cbVlanInfo->appHandle,
                       cbVlanInfo->cmdData.unsolMemberQuery.numOfEntries);
      dapiDebugSendMsg("                        Vlans:");
      for (index = 0; index < (L7_int32)cbVlanInfo->cmdData.unsolMemberQuery.numOfEntries; index++)
      {
        dapiDebugSendMsg(" %4d%s", cbVlanInfo->cmdData.unsolMemberQuery.vlanID[index],
                         cbVlanInfo->cmdData.unsolMemberQuery.lagTagged[index]==L7_TRUE?"*":" ");
      }
      dapiDebugSendMsg("\n   * Tagged VLAN\n");

    }
    else
    {
      cbEventFound = L7_FALSE;
    }
    break;

  case DAPI_CMD_ADDR_UNSOLICITED_EVENT:
    switch (event)
    {
    case DAPI_EVENT_ADDR_LEARNED_ADDRESS:
    case DAPI_EVENT_ADDR_AGED_ADDRESS:
      dapiDebugSendMsg("App: 0x%08lX  Address %s - MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                       cbAddrInfo->appHandle, event==DAPI_EVENT_ADDR_LEARNED_ADDRESS?"Learned":"Aged",
                       MAC_EXPAND(cbAddrInfo->cmdData.unsolLearnedAddress.macAddr.addr));
      dapiDebugSendMsg("  vlan: %d  flags: 0x%X (%s)\n", cbAddrInfo->cmdData.unsolLearnedAddress.vlanID,
                       cbAddrInfo->cmdData.unsolLearnedAddress.flags, dapiDebugNameAddrFlag[cbAddrInfo->cmdData.unsolLearnedAddress.flags]);
      break;

    case DAPI_EVENT_ADDR_INTF_MAC_QUERY:
      dapiDebugSendMsg("App: 0x%08lX  Interface MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
                       cbAddrInfo->appHandle, MAC_EXPAND(cbAddrInfo->cmdData.unsolIntfMacQuery.macAddr.addr));
      break;

    default:
      cbEventFound = L7_FALSE;
      break;
    }
    break;

  default:
    dapiDebugSendMsg("  Unrecognized Cmd.\n");
  }

  if (cbEventFound == L7_FALSE) {
    dapiDebugSendMsg("  Unrecognized Event.\n");
  }

  return;
}

/****************************************************************************************
*
* @purpose  Provide for missing function in strict ANSI. Converts a text string to upper
*           case.
*
* @param   *str     @b{(input)} The string to convert
*
* @returns *str     The converted string
*
* @comments
*
* @end
*
****************************************************************************************/
L7_char8 *dapiStrToUpr(L7_char8 *str)
{
  L7_int32 index;

  for (index=0; index<(L7_int32)strlen(str); index++)
  {
    if ((str[index] >= 'a') && (str[index] <= 'z')) {
      str[index] += ('A' - 'a');
    }
  }
  return(str);
}

#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_NONE)
/****************************************************************************************
*
* @purpose  Debug - Allow for dynamic modification of the per port/command
*           debug flags.
*
* @param   msgLevel     - The message level to set
* @param   unitSlotPort - slot number
* @param   commandName  - port number
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*
****************************************************************************************/
L7_RC_t dapiDebugMsgLvl(L7_ushort16 msgLevel, L7_char8 *unitSlotPort, L7_char8 *commandName)
{
  DAPI_USP_t  usp;
  L7_int32    uspElements, unit, slot, port;
  DAPI_CMD_t  startCmd = 0, endCmd = 0, currCmd;
  L7_int32    indexF, indexC;
  L7_char8    dapiFam[] = "DAPI_FAM";
  L7_char8    dapiCmd[] = "DAPI_CMD";
  const L7_char8 *testStr;
  L7_RC_t retCode = L7_SUCCESS;

  usp.unit = 0;
  usp.slot = 0xFF;
  usp.port = 0xFFFF;
  if ((unitSlotPort == L7_NULLPTR) && (commandName == L7_NULLPTR))
  {
    if (msgLevel <= DAPI_DEBUG_MSGLVL)
    {
      dapi_g->message_level = msgLevel;
    }
    else
    {
      retCode = L7_FAILURE;
    }
    startCmd = 1;    /* terminate for loop immediately */
    endCmd = 0;
  }
  else
  {
    /* set all ports */
    if ((unitSlotPort != L7_NULLPTR) &&
        (dapiStrToUpr(unitSlotPort) != L7_NULLPTR) &&
        (strcmp(unitSlotPort, "ALL") != 0))
    {
      uspElements = sscanf(unitSlotPort, "%d.%d.%d", &unit, &slot, &port);
      switch(uspElements)
      {
      case 3:   /* unit.slot.port specified */
        usp.port = (L7_ushort16)port;
      case 2:  /* unit.slot specified */
        usp.unit = (L7_ushort16)unit;
        usp.slot = (L7_ushort16)slot;
        break;
      default:
        retCode = L7_FAILURE;
      }
      if (usp.unit != 0)
      {
        retCode = L7_FAILURE;
      }
    }

    if ((dapiStrToUpr(commandName) != L7_NULLPTR) && (retCode != L7_FAILURE))
    {
      if (strncmp(commandName, dapiFam, strlen(dapiFam)) == 0)
      {
        for (indexF = 0; indexF < DAPI_NUM_OF_FAMILIES; indexF++)
        {
          testStr = dapi_g->name->family[indexF].name;
          (void)strlen(testStr);
          if (strncmp(commandName, testStr, strlen(testStr)) == 0)
          {
            /* family found */
            for (indexC = 0; indexC < DAPI_NUM_OF_CMDS; indexC++)
            {
              if (indexF == dapi_g->name->cmdToFamilyTable[indexC]) /* found first family command */
              {
                startCmd = (DAPI_CMD_t)indexC;
                break;
              }
            }
            for (; indexC < DAPI_NUM_OF_CMDS; indexC++)
            {
              if (indexF != dapi_g->name->cmdToFamilyTable[indexC]) /* found last family command */
              {
                endCmd = (DAPI_CMD_t)(indexC - 1);
                break;
              }
            }
            break;
          }
        }
      }
      else if ((strncmp(commandName, dapiCmd, strlen(dapiCmd)) == 0) && (retCode != L7_FAILURE))
      {
        for (indexC = 0; indexC < DAPI_NUM_OF_CMDS; indexC++)
        {
          testStr = dapi_g->name->cmd[indexC].name;
          (void)strlen(testStr);
          if (strncmp(commandName, testStr, strlen(testStr)) == 0)
          {
            startCmd = endCmd = (DAPI_CMD_t)indexC;
            break;
          }
        }
      }
      else
      {
        retCode = L7_FAILURE;
      }
    }
    else /* do all commands for the USP specified */
    {
      startCmd = 1;
      endCmd = DAPI_NUM_OF_CMDS - 1;
    }
  }

  if (retCode == L7_FAILURE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n*** Syntax Error ***\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Command Format:\n");
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   devshell \"dapiDebugMsgLvl  lvl  {'USP'|'ALL'  {'CMD'|'CMD_FAMILY'}}\"\n\n");
#else
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   devshell \"dapiDebugMsgLvl  lvl\"\n\n");
#endif
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Where:\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   lvl is the messaging level. Allowable message levels for this build are:\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     %d   No messages\n", DAPI_DEBUG_NONE);
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_ERRORS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     %d   Error messages\n", DAPI_DEBUG_ERRORS);
#endif
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_WARNINGS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     %d   Warning messages\n", DAPI_DEBUG_WARNINGS);
#endif
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     %d   Info messages\n", DAPI_DEBUG_INDICATIONS);
#endif
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_DECODE)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     %d   DAPI parameter decode\n", DAPI_DEBUG_DECODE);
#endif

#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n   USP is unit.slot.port in dotted decimal format. If the port is omitted,\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     all ports in the slot will be acted on (i.e. unit.slot is specified). If\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     ALL is used, all ports in all slots will be acted on.\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n   CMD is either the specific command to act on or the command family to\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "     act on. Omitting this parameter enables all commands on the USP(s).\n\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "If neither the USP nor the CMD are specified this command acts on the DAPI\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "generic message level.\n");
#else
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "This command acts on the DAPI generic message level.\n");
#endif

#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_ERRORS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Specifying the message level is cumulative. Enabling warning messages enables\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "both warning messages and error messages.\n");
#endif

#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Specifying either USP or CMD and at least message level %d enables the indication\n", DAPI_DEBUG_INDICATIONS);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "message. DAPI_CMD_INTF_STATISTICS indication messages may not be enabled as part\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "of a family or when enabling all commands on a particular USP but may be enabled\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "individually.\n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nPermissible families are:\n");
    for (indexF = 0; indexF < DAPI_NUM_OF_FAMILIES; indexF++)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   %s\n", dapi_g->name->family[indexF].name);
    }
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nFor permissible commands, consult dapi_struct.h. Use the command \"NAME\".\n");
#endif
  }
  else /* perform desired action */
  {
    if (usp.slot == (L7_int8) 0xFF)   /* all slots, all ports */
    {
      for (usp.slot = 0; usp.slot < dapi_g->unit[0]->numOfSlots; usp.slot++)
      {
        for (usp.port = 0; usp.port < dapi_g->unit[0]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
        {
          for (currCmd=startCmd; currCmd<=endCmd; currCmd++)
          {
            if (currCmd == DAPI_CMD_INTF_STATISTICS)
            {
              if ((startCmd == endCmd) && (startCmd == currCmd))
              {
                dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
              }
            }
            else
            {
              dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
            }
          }
        }
      }
    }
    else if (usp.port == (L7_short16) 0xFFFF) /* single slot, all ports */
    {
      for (usp.port = 0; usp.port < dapi_g->unit[0]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
      {
        for (currCmd=startCmd; currCmd<=endCmd; currCmd++)
        {
          if (currCmd == DAPI_CMD_INTF_STATISTICS)
          {
            if ((startCmd == endCmd) && (startCmd == currCmd))
            {
              dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
            }
          }
          else
          {
            dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
          }
        }
      }
    }
    else /* single slot, single port */
    {
      for (currCmd=startCmd; currCmd<=endCmd; currCmd++)
      {
        if (currCmd == DAPI_CMD_INTF_STATISTICS)
        {
          if ((startCmd == endCmd) && (startCmd == currCmd))
          {
            dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
          }
        }
        else
        {
          dapi_g->unit[0]->slot[usp.slot]->port[usp.port]->message_level[currCmd] = msgLevel;
        }
      }
    }
    retCode = L7_SUCCESS;
  }
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nThe current DAPI generic message level is %d\n", dapi_g->message_level);
  return retCode;
}
#else
L7_RC_t dapiDebugMsgLvl()
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n\nDAPI debug support is not compiled in!\n\n\n");
  return L7_SUCCESS;
}
#endif
