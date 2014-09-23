/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi.c
*
* @purpose   This file contains the entry points to DAPI
*
* @component dapi
*
* @comments
*
* @create    11/1/2004
*
* @author    mbaucom
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "dapi_trace.h"
#include "osapi_support.h"
#include "trace_api.h"
#include "dapi_debug.h"

#define DAPI_TRACE_DISPLAY_CLUSTER  8

/* Module variables */
static L7_uint32 dapiTraceHandle = ~0;
static L7_BOOL   dapiTraceMode = L7_FALSE;
static L7_uint32 dapiTraceMask = 0;
static L7_uint32 dapiTraceMaxEntries = 0;
static L7_uint32 dapiTraceMaxRecordWidth = 0;
static L7_uint32 dapiTraceIntIf = 0;
static DAPI_CMD_t dapiTraceCmdIgnore = 0;
static L7_uint32  dapiTraceOneShot = 0;
static L7_uint32  dapiTraceFrameType = 0;

/* Local datatypes */
typedef L7_RC_t (*dapiTraceFmtFunc_t)(L7_uchar8 *pDataStart);

typedef struct
{
  L7_uchar8   handle;  /* 00   : trace buffer handle */
  L7_uchar8   compId;  /* 01   : component id        */
  L7_ushort16 traceId;
  L7_uint32   time;
  L7_uint32   task;
} dapiTracePtHdr_t;

/* Dapi profiling variables */
dapiCmdProfileInfo_t *dapiCmdProfileInfo = L7_NULLPTR;
L7_BOOL               dapiProfileEnable = L7_FALSE;

/* local prototypes */
static L7_RC_t  dapiTraceFrameFormat(L7_uchar8 *pDataStart);
static L7_RC_t  dapiTraceDapiCallbackFormat(L7_uchar8 *pDataStart);
static L7_RC_t  dapiTraceGenericFormat(L7_uchar8 *pDataStart);
static L7_RC_t  dapiTraceDapiCtlFormat(L7_uchar8 *pDataStart);
L7_uint32 dapiTraceFrameTypeSet(L7_uint32 type);

void dapiProfileModeSet(L7_uint32 mode);
void dapiProfileInfoShow();


/*********************************************************************
* @purpose  Initialize the trace utility for dapi
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    dapiTraceModeApply(L7_TRUE) must be called in order to trace
* @end
*********************************************************************/
void dapiTraceInit(L7_uint32 numEntries,L7_uint32 width)
{
  /* make sure the sizes make sense */
  if (numEntries == 0) numEntries = DAPI_TRACE_ENTRY_MAX;
  if (width < DAPI_TRACE_ENTRY_SIZE_MAX) width = DAPI_TRACE_ENTRY_SIZE_MAX;

  /* clean up if the handle is already allocated */
  if (dapiTraceHandle != ~0) (void)dapiTraceFini();

  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(numEntries, width,
                       (L7_uchar8 *)"DAPI",
                       &dapiTraceHandle) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "DAPI: unable to create the trace buffer\n");
    dapiTraceHandle = ~0;
  }
  else
  {
    dapiTraceMaskSet(DAPI_TRACE_GENERIC_MASK | DAPI_TRACE_STACK_EVENT_MASK);
    dapiTraceIgnoreCmdSet(DAPI_CMD_INTF_STATISTICS);
    dapiTraceMaxEntries = numEntries;
    dapiTraceMaxRecordWidth = width;

    /* always disable tracing during init */
    dapiTraceModeApply(L7_TRUE);
  }

  return ;
}

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dapiTraceFini(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (dapiTraceHandle != ~0)
  {
    /* disable at the mask level */
    dapiTraceMaskSet(0);

    /* disable at the trace util */
    dapiTraceModeApply(L7_FALSE);

    if (traceBlockDelete(dapiTraceHandle) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "DAPI: unable to delete the trace buffer\n");
      rc = L7_FAILURE;
    }
    else
    {
      dapiTraceHandle = ~0;
      dapiTraceMaxEntries = dapiTraceMaxRecordWidth = 0;
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Trace a dapiCtl call
*
* @param    cmd     @b{(input)}     The command being traced
* @param    pUsp    @b{(input)}     The port being acted upon
* @param    enter   @b{(input)}     L7_TRUE if trace is prior to invocation of dapiCtl
*                               L7_FALSE if the trace is after the invocation
* @param    rc      @b{(input)}     The return code of the dapiCtl call
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceDapiCtl(DAPI_CMD_t cmd, DAPI_USP_t *pUsp, L7_BOOL enter,L7_RC_t rc)
{
  L7_ushort16 traceId = DAPI_TRACE_DAPI_CTL;
  L7_uint32   uspInt = USP_COMPRESS(pUsp);


  if ((dapiTraceHandle != ~0) &&
      (dapiTraceMode == L7_TRUE) &&
      (dapiTraceMask & DAPI_TRACE_DAPI_CTL_MASK))
  {
    if (dapiTraceCmdIgnore == cmd) return;
    if ((dapiTraceIntIf > 0) && (dapiTraceIntIf != uspInt)) return;

    TRACE_BEGIN(dapiTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)cmd));
    TRACE_ENTER_4BYTE(osapiHtonl(uspInt));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)rc));
    TRACE_ENTER_BYTE(enter);
    TRACE_END();

    if (dapiTraceOneShot > 0)
    {
      dapiTraceOneShot++;
      if (dapiTraceOneShot >= dapiTraceMaxEntries)
      {
        dapiTraceModeApply(L7_FALSE);
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Trace a preformated string
*
* @param    str     @b{(input)}     a string to be placed directly in the trace
*
* @returns  void
*
* @notes    The string will be truncated to the trace width
* @end
*********************************************************************/
void dapiTraceGeneric(L7_char8* format,...)
{
  L7_ushort16 traceId = DAPI_TRACE_GENERIC;
  L7_uint32 max;
  L7_char8  buf[256];
  va_list ap;

  memset(&ap, 0, sizeof(ap));

  va_start(ap, format);
  (void)vsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  if ((dapiTraceHandle != ~0) &&
      (dapiTraceMode == L7_TRUE) &&
      (dapiTraceMask & DAPI_TRACE_GENERIC_MASK))
  {
    TRACE_BEGIN(dapiTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));

    /* generic traces are truncated at 55 characters */
    max = strlen(buf);

    /* make sure strlen is no bigger than the max - header + null term */
    if (max > (dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + 1)))
      max = (dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + 1));

    TRACE_ENTER_BLOCK(buf,max);
    TRACE_ENTER_BYTE(0);

    TRACE_END();

    if (dapiTraceOneShot > 0)
    {
      dapiTraceOneShot++;
      if (dapiTraceOneShot >= dapiTraceMaxEntries)
      {
        dapiTraceModeApply(L7_FALSE);
      }
    }

  }

  return;
}


/*********************************************************************
* @purpose  Trace a preformated string of stack events
*
* @param    str     @b{(input)}     a string to be placed directly in the trace
*
* @returns  void
*
* @notes    The string will be truncated to the trace width
* @end
*********************************************************************/
void dapiTraceStackEvent(L7_char8* format,...)
{
  L7_ushort16 traceId = DAPI_TRACE_STACK_EVENT;
  L7_uint32 max;
  L7_char8  buf[256];
  va_list ap;

  memset(&ap, 0, sizeof(ap));

  va_start(ap, format);
  (void)vsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  if ((dapiTraceHandle != ~0) &&
      (dapiTraceMode == L7_TRUE) &&
      (dapiTraceMask & DAPI_TRACE_STACK_EVENT_MASK))
  {
    TRACE_BEGIN(dapiTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));

    /* generic traces are truncated at 55 characters */
    max = strlen(buf);

    /* make sure strlen is no bigger than the max - header + null term */
    if (max > (dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + 1)))
      max = (dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + 1));

    TRACE_ENTER_BLOCK(buf,max);
    TRACE_ENTER_BYTE(0);

    TRACE_END();

    if (dapiTraceOneShot > 0)
    {
      dapiTraceOneShot++;
      if (dapiTraceOneShot >= dapiTraceMaxEntries)
      {
        dapiTraceModeApply(L7_FALSE);
      }
    }

  }

  return;
}


/*********************************************************************
* @purpose  Trace a Callback
*
* @param    event   @b{(input)} The callback event being traced
* @param    pUsp    @b{(input)} The port the callback is with reference to
* @param    enter   @b{(input)}     L7_TRUE if trace is prior to invocation of dapiCtl
*                               L7_FALSE if the trace is after the invocation
* @param    rc      @b{(input)}     The return code of the dapiCtl call
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceDapiCallback(DAPI_EVENT_t event, DAPI_USP_t *pUsp, L7_BOOL enter,L7_RC_t rc)
{
  L7_ushort16 traceId = DAPI_TRACE_DAPI_CB;
  L7_uint32   uspInt = USP_COMPRESS(pUsp);

  if ((dapiTraceHandle != ~0) &&
      (dapiTraceMode == L7_TRUE) &&
      (dapiTraceMask & DAPI_TRACE_DAPI_CB_MASK))
  {
    TRACE_BEGIN(dapiTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)event));
    TRACE_ENTER_4BYTE(osapiHtonl(uspInt));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)rc));
    TRACE_ENTER_BYTE(enter);
    TRACE_END();

    if (dapiTraceOneShot > 0)
    {
      dapiTraceOneShot++;
      if (dapiTraceOneShot >= dapiTraceMaxEntries)
      {
        dapiTraceModeApply(L7_FALSE);
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Trace a frame
*
* @param    pUsp    {(input)} The port receiving the packet
* @param    pkt     {(input)} The packet being traced
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceFrame(DAPI_USP_t *pUsp, L7_uchar8 *pkt,L7_uchar8 drop,L7_uint32 cookie)
{
  L7_ushort16 traceId = DAPI_TRACE_FRAME;
  L7_uint32   uspInt = USP_COMPRESS(pUsp);
  L7_uint32   numBytes;

  if ((dapiTraceHandle != ~0) &&
      (dapiTraceMode == L7_TRUE) &&
      (dapiTraceMask & DAPI_TRACE_FRAME_MASK))
  {
    if (((L7_uint32)(1 << drop)) & ~dapiTraceFrameType) return;
    if ((dapiTraceIntIf > 0) && (dapiTraceIntIf != uspInt)) return;

    TRACE_BEGIN(dapiTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));
    TRACE_ENTER_4BYTE(osapiHtonl(uspInt));
    TRACE_ENTER_BYTE(drop);
    TRACE_ENTER_4BYTE(osapiHtonl(cookie));

    /* Get the number of bytes to write for the packet */
    numBytes = dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + sizeof(uspInt) + sizeof(drop) + sizeof(cookie));
    TRACE_ENTER_BLOCK(pkt ,numBytes);

    TRACE_END();

    if (dapiTraceOneShot > 0)
    {
      dapiTraceOneShot++;
      if (dapiTraceOneShot >= dapiTraceMaxEntries)
      {
        dapiTraceModeApply(L7_FALSE);
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Show the trace buffer
*
* @param    count       @b{(input)} The number of events to show
* @param    db          @b{(input)} A 3 char string id to search on
* @param    unformat    @b{(input)} L7_TRUE  - for unformatted output
* @param                            L7_FALSE - for formatted output
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
void dapiTraceShow(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(dapiTraceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate DAPI trace block for handle "
                  "0x%8.8x\n", dapiTraceHandle);
    return ;
  }

  if (traceData->entryCount <= traceData->numOfEntries)
  {
    pEntry = traceData->head;
    numEnt = (L7_uint32)traceData->entryCount;
    entIndex = 0;
  }
  else
  {  /*trace dump will always begin from the oldest entry to the latest entry*/
    pEntry = traceData->next;
    numEnt = traceData->numOfEntries;
    entIndex = traceData->entryCount % traceData->numOfEntries;
  }

  /* a count of 0 means 'display all' entries */
  if (count == 0)
    count = numEnt;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");


  for (i = 0; i < numEnt; i++)
  {
    pOffset = pEntry;

    /* only display the last N entries, indicated by the count parm */
    if (count >= (numEnt-i))
    {
      if (unformat == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

        for (j = 1, jmax = traceData->bytesPerEntry; j <= jmax; j++)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%2.2x ", *pOffset++);
          if (((j % DAPI_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

      } /* endif unformatted */
      else  /* manually format output */
      {
        dapiTraceFmtFunc_t     fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8             *pIdStr;
        dapiTracePtHdr_t        *pHdr;
        L7_BOOL                 printRecord = L7_TRUE;

        /* the first x bytes are the same for all dapi trace entries */
        pHdr = (dapiTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case DAPI_TRACE_DAPI_CTL:
            pIdStr = (L7_uchar8 *)"CTL";
            fmtFunc = dapiTraceDapiCtlFormat;
            break;

          case DAPI_TRACE_GENERIC:
            pIdStr = (L7_uchar8 *)"GENERIC";
            fmtFunc = dapiTraceGenericFormat;
            break;

          case DAPI_TRACE_STACK_EVENT:
            pIdStr = (L7_uchar8 *)"STACK_EVENT";
            fmtFunc = dapiTraceGenericFormat;
            break;

          case DAPI_TRACE_DAPI_CB:
            pIdStr = (L7_uchar8 *)"CB";
            fmtFunc = dapiTraceDapiCallbackFormat;
            break;

          case DAPI_TRACE_FRAME:
            pIdStr = (L7_uchar8 *)"FRAME";
            fmtFunc = dapiTraceFrameFormat;
            break;

          default:
            pIdStr = (L7_uchar8 *)"???????";
            break;
        }

        if (db != L7_NULL)
        {
          if (!strstr(pIdStr,db))
            printRecord = L7_FALSE;
        }

        if (printRecord)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"%-15s: ", pIdStr);

          /* show the entry timestamp */
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"ts=%8.8u ",
                        (L7_uint32)osapiHtonl(pHdr->time));

          /* show the task */
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"tid=0x%0.8x ",
                        (L7_uint32)osapiHtonl(pHdr->task));

          /* format the rest of the entry per the id value */
          if (fmtFunc != L7_NULLPTR)
          {
            if ((*fmtFunc)(pOffset) != L7_SUCCESS)
            {
              SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                            "   *** ERROR (cannot format this entry) ***");
            }
          }
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
        }

      } /* endelse manually format output */

    } /* endif count >= (numEnt-i) */

    /* watch for trace wrap */
    entIndex++;
    pEntry += traceData->bytesPerEntry;
    if (pEntry >= traceData->tail)
    {
      pEntry = traceData->head;
      entIndex = 0;
    }

  } /* endfor i */

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

  return ;
}

/*********************************************************************
* @purpose  Get the string which identifies a command
*
* @param    cmd     @b{(input)} The command
*
* @returns  void
*
* @end
*********************************************************************/
L7_char8 *dapiCommandToStringGet(DAPI_CMD_t cmd)
{
  L7_char8 *str;

  switch (cmd)
  {
    case DAPI_CMD_UNDEFINED:
      str = "DAPI_CMD_UNDEFINED";
      break;
    case DAPI_CMD_CONTROL_CALLBACK_REGISTER:
      str = "DAPI_CMD_CONTROL_CALLBACK_REGISTER";
      break;
    case DAPI_CMD_CONTROL_DRIVER_START:
      str = "DAPI_CMD_CONTROL_DRIVER_START";
      break;
    case DAPI_CMD_CONTROL_HW_APPLY_NOTIFY:
      str = "DAPI_CMD_CONTROL_HW_APPLY_NOTIFY";
      break;
    case DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY:
      str = "DAPI_CMD_CONTROL_UNIT_STATUS_NOTIFY";
      break;
    case DAPI_CMD_CARD_INSERT:
      str = "DAPI_CMD_CARD_INSERT";
      break;
    case DAPI_CMD_CARD_REMOVE:
      str = "DAPI_CMD_CARD_REMOVE";
      break;
    case DAPI_CMD_CARD_PLUGIN:
      str = "DAPI_CMD_CARD_PLUGIN";
      break;
    case DAPI_CMD_CARD_UNPLUG:
      str = "DAPI_CMD_CARD_UNPLUG";
      break;
    case DAPI_CMD_SYSTEM_MIRRORING:
      str = "DAPI_CMD_SYSTEM_MIRRORING";
      break;
    case DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS:
      str = "DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS";
      break;
    case DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS:
      str = "DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS";
      break;
    case DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET:
      str = "DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET";
      break;
    case DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET:
      str = "DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET";
      break;
    case DAPI_CMD_INTF_DOT1S_BPDU_FILTERING:
      str = "DAPI_CMD_INTF_DOT1S_BPDU_FILTERING";
      break;
    case DAPI_CMD_INTF_DOT1S_BPDU_GUARD:
      str = "DAPI_CMD_INTF_DOT1S_BPDU_GUARD";
      break;
    case DAPI_CMD_INTF_DOT1S_BPDU_FLOOD:
      str = "DAPI_CMD_INTF_DOT1S_BPDU_FLOOD";
      break;
    case DAPI_CMD_SYSTEM_FLOW_CONTROL:
      str = "DAPI_CMD_SYSTEM_FLOW_CONTROL";
      break;
    case DAPI_CMD_SYSTEM_SNOOP_CONFIG:
      str = "DAPI_CMD_SYSTEM_SNOOP_CONFIG";
      break;
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE:
      str = "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_CREATE";
      break;
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE:
      str = "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_DELETE";
      break;
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD:
      str = "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_ADD";
      break;
    case DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE:
      str = "DAPI_CMD_SYSTEM_DOT1S_INSTANCE_VLAN_REMOVE";
      break;
    case DAPI_CMD_SYSTEM_DOT1X_CONFIG:
      str = "DAPI_CMD_SYSTEM_DOT1X_CONFIG";
      break;
    case DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG:
      str = "DAPI_CMD_SYSTEM_DOSCONTROL_CONFIG";
      break;
    case DAPI_CMD_INTF_DOSCONTROL_CONFIG:
      str = "DAPI_CMD_INTF_DOSCONTROL_CONFIG";
      break;
    case DAPI_CMD_FRAME_UNSOLICITED_EVENT:
      str = "DAPI_CMD_FRAME_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_FRAME_SEND:
      str = "DAPI_CMD_FRAME_SEND";
      break;
    case DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY:
      str = "DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY";
      break;
    case DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED:
      str = "DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED";
      break;
    case DAPI_CMD_INTF_SAMPLE_RATE:
      str = "DAPI_CMD_INTF_SAMPLE_RATE";
      break;
    case DAPI_CMD_INTF_UNSOLICITED_EVENT:
      str = "DAPI_CMD_INTF_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_INTF_STP_STATE:
      str = "DAPI_CMD_INTF_STP_STATE";
      break;
    case DAPI_CMD_INTF_SPEED_CONFIG:
      str = "DAPI_CMD_INTF_SPEED_CONFIG";
      break;
    case DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG:
      str = "DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG";
      break;
    case DAPI_CMD_INTF_LOOPBACK_CONFIG:
      str = "DAPI_CMD_INTF_LOOPBACK_CONFIG";
      break;
    case DAPI_CMD_INTF_ISOLATE_PHY_CONFIG:
      str = "DAPI_CMD_INTF_ISOLATE_PHY_CONFIG";
      break;
    case DAPI_CMD_INTF_STATISTICS:
      str = "DAPI_CMD_INTF_STATISTICS";
      break;
    case DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG:
      str = "DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG";
      break;
    case DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG:
      str = "DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG";
      break;
    case DAPI_CMD_INTF_IPSG_CONFIG:
      str = "DAPI_CMD_IPSG_INTF_CONFIG";
      break;
    case DAPI_CMD_INTF_IPSG_STATS_GET:
      str = "DAPI_CMD_IPSG_INTF_STATS_GET";
      break;
    case DAPI_CMD_INTF_IPSG_CLIENT_ADD:
      str = "DAPI_CMD_IPSG_CLIENT_ADD";
      break;
    case DAPI_CMD_INTF_IPSG_CLIENT_DELETE:
      str = "DAPI_CMD_IPSG_CLIENT_DELETE";
      break;
    case DAPI_CMD_INTF_DOT3AH_CONFIG_ADD:
      str = "DAPI_CMD_DOT3AH_CONFIG_ADD";
      break;
    case DAPI_CMD_INTF_DOT3AH_CONFIG_REMOVE:
      str = "DAPI_CMD_DOT3AH_CONFIG_REMOVE";
      break;
    case DAPI_CMD_INTF_DOT3AH_REM_LB_CONFIG:
      str = "DAPI_CMD_DOT3AH_REM_LB_CONFIG";
      break;
    case DAPI_CMD_INTF_DOT3AH_REM_LB_ADD:
      str = "DAPI_CMD_DOT3AH_REM_LB_ADD";
      break;
    case DAPI_CMD_INTF_DOT3AH_REM_LB_ADD_UNI:
      str = "DAPI_CMD_DOT3AH_REM_LB_ADD_UNI";
      break;
    case DAPI_CMD_INTF_DOT3AH_REM_LB_DELETE:
      str = "DAPI_CMD_DOT3AH_REM_LB_DELETE";
      break;
    case DAPI_CMD_INTF_FLOW_CONTROL:
      str = "DAPI_CMD_INTF_FLOW_CONTROL";
      break;
    case DAPI_CMD_INTF_CABLE_STATUS_GET:
      str = "DAPI_CMD_INTF_CABLE_STATUS_GET";
      break;
    case DAPI_CMD_INTF_FIBER_DIAG_GET:
      str = "DAPI_CMD_INTF_FIBER_DIAG_GET";
      break;
    case DAPI_CMD_INTF_DOT1S_STATE:
      str = "DAPI_CMD_INTF_DOT1S_STATE";
      break;
    case DAPI_CMD_INTF_DOT1X_STATUS:
      str = "DAPI_CMD_INTF_DOT1X_STATUS";
      break;
    case DAPI_CMD_INTF_DOT1X_CLIENT_ADD:
      str = "DAPI_CMD_INTF_DOT1X_CLIENT_ADD";
      break;
    case DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE:
      str = "DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE";
      break;
  case DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT:
      str="DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT";
      break;
    case DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK:
      str="DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK";
      break;
    case DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK:
      str="DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK";
      break;
    case DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG:
      str = "DAPI_CMD_INTF_DOUBLEVLAN_TAG_CONFIG";
      break;
    case DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG:
      str = "DAPI_CMD_INTF_MULTI_TPID_DOUBLEVLAN_TAG_CONFIG";
      break;
    case DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG:
      str = "DAPI_CMD_DEFAULT_TPID_DOUBLEVLAN_TAG_CONFIG";
      break;
    case DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG:
      str = "DAPI_CMD_INTF_MAX_FRAME_SIZE_CONFIG";
      break;
    case DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG:
      str = "DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG";
      break;
    case DAPI_CMD_ADDR_UNSOLICITED_EVENT:
      str = "DAPI_CMD_ADDR_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD:
      str = "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_ADD";
      break;
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY:
      str = "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_MODIFY";
      break;
    case DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE:
      str = "DAPI_CMD_ADDR_MAC_ADDRESS_ENTRY_DELETE";
      break;
    case DAPI_CMD_ADDR_MAC_FILTER_ADD:
      str = "DAPI_CMD_ADDR_MAC_FILTER_ADD";
      break;
    case DAPI_CMD_ADDR_MAC_FILTER_DELETE:
      str = "DAPI_CMD_ADDR_MAC_FILTER_DELETE";
      break;
    case DAPI_CMD_ADDR_AGING_TIME:
      str = "DAPI_CMD_ADDR_AGING_TIME";
      break;
    case DAPI_CMD_ADDR_FLUSH:
      str = "DAPI_CMD_ADDR_FLUSH";
      break;

    case DAPI_CMD_ADDR_FLUSH_VLAN:
      str = "DAPI_CMD_ADDR_FLUSH_VLAN";
      break;

    case DAPI_CMD_ADDR_FLUSH_MAC:
      str = "DAPI_CMD_ADDR_FLUSH_MAC";
      break;

    case DAPI_CMD_QVLAN_UNSOLICITED_EVENT:
      str = "DAPI_CMD_QVLAN_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_QVLAN_VLAN_CREATE:
      str = "DAPI_CMD_QVLAN_VLAN_CREATE";
      break;
    case DAPI_CMD_QVLAN_VLAN_LIST_CREATE:
      str = "DAPI_CMD_QVLAN_VLAN_LIST_CREATE";
      break;
    case DAPI_CMD_QVLAN_VLAN_LIST_PURGE:
      str = "DAPI_CMD_QVLAN_VLAN_LIST_PURGE";
      break;
    case DAPI_CMD_QVLAN_VLAN_PORT_ADD:
      str = "DAPI_CMD_QVLAN_VLAN_PORT_ADD";
      break;
    case DAPI_CMD_QVLAN_VLAN_PORT_DELETE:
      str = "DAPI_CMD_QVLAN_VLAN_PORT_DELETE";
      break;
    case DAPI_CMD_QVLAN_VLAN_PURGE:
      str = "DAPI_CMD_QVLAN_VLAN_PURGE";
      break;
    case DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET:
    str = "DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET";
    break;
    case DAPI_CMD_QVLAN_PORT_CONFIG:
      str = "DAPI_CMD_QVLAN_PORT_CONFIG";
      break;
    case DAPI_CMD_QVLAN_PBVLAN_CONFIG:
      str = "DAPI_CMD_QVLAN_PBVLAN_CONFIG";
      break;
    case DAPI_CMD_QVLAN_PORT_PRIORITY:
      str = "DAPI_CMD_QVLAN_PORT_PRIORITY";
      break;
    case DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP:
      str = "DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP";
      break;
    case DAPI_CMD_QVLAN_VLAN_STATS_GET:
      str = "DAPI_CMD_QVLAN_VLAN_STATS_GET";
      break;
    case DAPI_CMD_GARP_GVRP:
      str = "DAPI_CMD_GARP_GVRP";
      break;
    case DAPI_CMD_GARP_GMRP:
      str = "DAPI_CMD_GARP_GMRP";
      break;
    case DAPI_CMD_GVRP_GMRP_CONFIG:
      str = "DAPI_CMD_GVRP_GMRP_CONFIG";
      break;
    case DAPI_CMD_GARP_GROUP_REG_MODIFY:
      str = "DAPI_CMD_GARP_GROUP_REG_MODIFY";
      break;
    case DAPI_CMD_GARP_GROUP_REG_DELETE:
      str = "DAPI_CMD_GARP_GROUP_REG_DELETE";
      break;
    case DAPI_CMD_LOGICAL_INTF_CREATE:
      str = "DAPI_CMD_LOGICAL_INTF_CREATE";
      break;
    case DAPI_CMD_LOGICAL_INTF_DELETE:
      str = "DAPI_CMD_LOGICAL_INTF_DELETE";
      break;
    case DAPI_CMD_LAG_CREATE:
      str = "DAPI_CMD_LAG_CREATE";
      break;
    case DAPI_CMD_LAG_PORT_ADD:
      str = "DAPI_CMD_LAG_PORT_ADD";
      break;
    case DAPI_CMD_LAG_PORT_DELETE:
      str = "DAPI_CMD_LAG_PORT_DELETE";
      break;
    /* PTin added */
    case DAPI_CMD_INTERNAL_LAG_PORT_ADD:
      str = "DAPI_CMD_INTERNAL_LAG_PORT_ADD";
      break;
    case DAPI_CMD_INTERNAL_LAG_PORT_DELETE:
      str = "DAPI_CMD_INTERNAL_LAG_PORT_DELETE";
      break;
    /* End of PTin added */
    case DAPI_CMD_LAG_DELETE:
      str = "DAPI_CMD_LAG_DELETE";
      break;
    case DAPI_CMD_AD_TRUNK_MODE_SET:
      str = "DAPI_CMD_AD_TRUNK_MODE_SET";
      break;
    case DAPI_CMD_LAG_HASHMODE_SET:
      str = "DAPI_CMD_LAG_HASHMODE_SET";
      break;
    case DAPI_CMD_LAGS_SYNCHRONIZE:
      str = "DAPI_CMD_LAGS_SYNCHRONIZE";
      break;
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD:
      str = "DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD";
      break;
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY:
      str = "DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY";
      break;
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE:
      str = "DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE";
      break;
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL:
      str = "DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL";
      break;
    case DAPI_CMD_IPV6_ROUTE_ENTRY_ADD:
      str = "DAPI_CMD_IPV6_ROUTE_ENTRY_ADD";
      break;
    case DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY:
      str = "DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY";
      break;
    case DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE:
      str = "DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE";
      break;
    case DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG:
      str = "DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG";
      break;
    case DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG:
      str = "DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG";
      break;
    case DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG:
      str = "DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG";
      break;
    case DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG:
      str = "DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG";
      break;
    case DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG:
      str = "DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG";
      break;
    case DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG:
      str = "DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_SNOOP_NOTIFY";
      break;
    case DAPI_CMD_ROUTING_ARP_ENTRY_ADD:
      str = "DAPI_CMD_ROUTING_ARP_ENTRY_ADD";
      break;
    case DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY:
      str = "DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY";
      break;
    case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE:
      str = "DAPI_CMD_ROUTING_ARP_ENTRY_DELETE";
      break;
    case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL:
      str = "DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL";
      break;
    case DAPI_CMD_ROUTING_ARP_ENTRY_QUERY:
      str = "DAPI_CMD_ROUTING_ARP_ENTRY_QUERY";
      break;
    case DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT:
      str = "DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_IPV6_NEIGH_ENTRY_ADD:
      str = "DAPI_CMD_IPV6_NEIGH_ENTRY_ADD";
      break;
    case DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY:
      str = "DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY";
      break;
    case DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE:
      str = "DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE";
      break;
    case DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY:
      str = "DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY";
      break;
    case DAPI_CMD_ROUTING_INTF_ADD:
      str = "DAPI_CMD_ROUTING_INTF_ADD";
      break;
    case DAPI_CMD_ROUTING_INTF_MODIFY:
      str = "DAPI_CMD_ROUTING_INTF_MODIFY";
      break;
    case DAPI_CMD_ROUTING_INTF_DELETE:
      str = "DAPI_CMD_ROUTING_INTF_DELETE";
      break;
    case DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET:
      str = "DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET";
      break;
    case DAPI_CMD_IPV6_INTF_ADDR_ADD:
      str = "DAPI_CMD_IPV6_INTF_ADDR_ADD";
      break;
    case DAPI_CMD_IPV6_INTF_ADDR_DELETE:
      str = "DAPI_CMD_IPV6_INTF_ADDR_DELETE";
      break;
    case DAPI_CMD_IPV6_INTF_STATS_GET:
      str = "DAPI_CMD_IPV6_INTF_STATS_GET";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_FWD_CONFIG";
      break;
    case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD:
      str = "DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD";
      break;
    case DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE:
      str = "DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE";
      break;
    case DAPI_CMD_ROUTING_INTF_VRID_ADD:
      str = "DAPI_CMD_ROUTING_INTF_VRID_ADD";
      break;
    case DAPI_CMD_ROUTING_INTF_VRID_DELETE:
      str = "DAPI_CMD_ROUTING_INTF_VRID_DELETE";
      break;
    case DAPI_CMD_TUNNEL_CREATE:
      str = "DAPI_CMD_TUNNEL_CREATE";
      break;
    case DAPI_CMD_TUNNEL_DELETE:
      str = "DAPI_CMD_TUNNEL_DELETE";
      break;
    case DAPI_CMD_TUNNEL_NEXT_HOP_SET:
      str = "DAPI_CMD_TUNNEL_NEXT_HOP_SET";
      break;
    case DAPI_CMD_L3_TUNNEL_ADD:
      str = "DAPI_CMD_L3_TUNNEL_ADD";
      break;
    case DAPI_CMD_L3_TUNNEL_DELETE:
      str = "DAPI_CMD_L3_TUNNEL_DELETE";
      break;
    case DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD:
      str = "DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD";
      break;
    case DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE:
      str = "DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_ADD:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_ADD";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_DELETE:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_DELETE";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_COUNTERS_GET";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_USE_GET:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_USE_GET";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_PORT_DELETE";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_VLAN_PORT_MEMBER_UPDATE";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET";
      break;
    case DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT:
      str = "DAPI_CMD_ROUTING_INTF_MCAST_UNSOLICITED_EVENT";
      break;
    case DAPI_CMD_QOS_DIFFSERV_INST_ADD:
      str = "DAPI_CMD_QOS_DIFFSERV_INST_ADD";
      break;
    case DAPI_CMD_QOS_DIFFSERV_INST_DELETE:
      str = "DAPI_CMD_QOS_DIFFSERV_INST_DELETE";
      break;
    case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET:
      str = "DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET";
      break;
    case DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET:
      str = "DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET";
      break;
    case DAPI_CMD_QOS_SYS_ACL_ADD:
      str = DAPI_CMD_QOS_SYS_ACL_ADD_NAME;
      break;
    case DAPI_CMD_QOS_SYS_ACL_DELETE:
      str = DAPI_CMD_QOS_SYS_ACL_DELETE_NAME;
      break;
    case DAPI_CMD_QOS_ACL_ADD:
      str = "DAPI_CMD_QOS_ACL_ADD";
      break;
    case DAPI_CMD_QOS_ACL_DELETE:
      str = "DAPI_CMD_QOS_ACL_DELETE";
      break;
    case DAPI_CMD_QOS_ACL_RULE_COUNT_GET:
      str = "DAPI_CMD_QOS_ACL_RULE_COUNT_GET";
      break;
    case DAPI_CMD_QOS_ACL_RULE_STATUS_SET:
      str = "DAPI_CMD_QOS_ACL_RULE_STATUS_SET";
      break;
    case DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP:
      str = "DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP";
      break;
    case DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP:
      str = "DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP";
      break;
    case DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG:
      str = "DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG";
      break;
    case DAPI_CMD_QOS_COS_INTF_CONFIG:
      str = "DAPI_CMD_QOS_COS_INTF_CONFIG";
      break;
    case DAPI_CMD_QOS_COS_INTF_STATUS:
      str = "DAPI_CMD_QOS_COS_INTF_STATUS";
      break;
    case DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG:
      str = "DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG";
      break;
    case DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG:
      str = "DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG";
      break;
    case DAPI_CMD_QOS_VOIP_SESSION:
      str = "DAPI_CMD_QOS_VOIP_SESSION";
      break;
    case DAPI_CMD_QOS_VOIP_PROFILE:
      str = "DAPI_CMD_QOS_VOIP_PROFILE";
      break;
    case DAPI_CMD_QOS_VOIP_STATS_GET:
      str = "DAPI_CMD_QOS_VOIP_STATS_GET";
      break;
    case DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD:
      str = "DAPI_CMD_QOS_ISCSI_TARGET_PORT_ADD";
      break;
    case DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE:
      str = "DAPI_CMD_QOS_ISCSI_TARGET_PORT_DELETE";
      break;
    case DAPI_CMD_QOS_ISCSI_CONNECTION_ADD:
      str = "DAPI_CMD_QOS_ISCSI_CONNECTION_ADD";
      break;
    case DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE:
      str = "DAPI_CMD_QOS_ISCSI_CONNECTION_REMOVE";
      break;
    case DAPI_CMD_QOS_ISCSI_COUNTER_GET:
      str = "DAPI_CMD_QOS_ISCSI_COUNTER_GET";
      break;
    case DAPI_CMD_L2_TUNNEL_CREATE:
      str = "DAPI_CMD_L2_TUNNEL_CREATE";
      break;
    case DAPI_CMD_L2_TUNNEL_MODIFY:
      str = "DAPI_CMD_L2_TUNNEL_MODIFY";
      break;
    case DAPI_CMD_L2_TUNNEL_DELETE:
      str = "DAPI_CMD_L2_TUNNEL_DELETE";
      break;
    case DAPI_CMD_L2_TUNNEL_VLAN_SET:
      str = "DAPI_CMD_L2_TUNNEL_VLAN_SET";
      break;
    case DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET:
      str = "DAPI_CMD_L2_TUNNEL_UDP_PORTS_SET";
      break;
    case DAPI_CMD_DEBUG_SHELL:
      str = "DAPI_CMD_DEBUG_SHELL";
          break;
    case DAPI_CMD_VOICE_VLAN_DEVICE_ADD:
      str = "DAPI_CMD_VOICE_VLAN_DEVICE_ADD";
      break;
    case DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE:
      str = "DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE";
      break;
    case DAPI_CMD_VOICE_VLAN_COS_OVERRIDE:
      str = "DAPI_CMD_VOICE_VLAN_COS_OVERRIDE";
      break;
    case DAPI_CMD_VOICE_VLAN_AUTH:
      str = "DAPI_CMD_VOICE_VLAN_AUTH";
      break;
    case DAPI_CMD_L2_DOT1AD_INTF_TYPE:
      str = "DAPI_CMD_L2_DOT1AD_INTF_TYPE";
      break;
    case DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION:
      str = "DAPI_CMD_L2_DOT1AD_TUNNEL_ACTION";
      break;
    case DAPI_CMD_L2_DOT1AD_INTF_STATS:
      str = "DAPI_CMD_L2_DOT1AD_INTF_STATS";
      break;
    case DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA:
      str = "DAPI_CMD_L2_DOT1AD_APPLY_INTF_CONFIG_DATA";
      break;
    case DAPI_CMD_DOT1AD_ADD:
      str = "DAPI_CMD_DOT1AD_ADD";
      break;
    case DAPI_CMD_DOT1AD_DELETE:
      str = "DAPI_CMD_DOT1AD_DELETE";
      break;
    case DAPI_CMD_DOT1AD_RULE_COUNT_GET:
      str = "DAPI_CMD_DOT1AD_RULE_COUNT_GET";
      break;

    case DAPI_NUM_OF_CMDS:
      str = "DAPI_NUM_OF_CMDS";
      break;
    case DAPI_CMD_IPSUBNET_VLAN_CREATE:
      str = "DAPI_CMD_IPSUBNET_VLAN_CREATE";
      break;
    case DAPI_CMD_IPSUBNET_VLAN_DELETE:
      str = "DAPI_CMD_IPSUBNET_VLAN_DELETE";
      break;
    case DAPI_CMD_MAC_VLAN_CREATE:
      str = "DAPI_CMD_MAC_VLAN_CREATE";
      break;
    case DAPI_CMD_MAC_VLAN_DELETE:
      str = "DAPI_CMD_MAC_VLAN_DELETE";
      break;

    case DAPI_CMD_ISDP_INTF_STATUS_SET:
      str = "DAPI_CMD_ISDP_INTF_STATUS_SET";
      break;
    case DAPI_CMD_INTF_LLPF_CONFIG:
      str = "DAPI_CMD_INTF_LLPF_CONFIG";
      break;
    default:
      str = "UNKNOWN";
      break;
  }

  return str;
}

/*********************************************************************
* @purpose  Enable the trace
*
* @param    enable      @b{(input)} L7_TRUE to enable the trace
*                                   L7_FALSE to disable the trace
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceModeApply(L7_BOOL enable)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (enable == L7_TRUE)
    rc = traceBlockStart(dapiTraceHandle);
  else
    rc = traceBlockStop(dapiTraceHandle);

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "dapiTraceModeApply: Unable to apply dapi trace mode (%u) handle=0x%8.8x\n",
            enable , dapiTraceHandle);
  }
  else
  {
    dapiTraceMode = enable;
  }
}

/*********************************************************************
* @purpose  Enable trace for a specific interface only
*
* @param    intIf   @b{(input)} The compressed usp
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceIntIfSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  DAPI_USP_t usp = {unit,slot,port};

  dapiTraceIntIf = USP_COMPRESS(&usp);
}

/*********************************************************************
* @purpose  Disable tracing for a specific dapi Command
*
* @param    cmd     @b{(input)} The command being ignored
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceIgnoreCmdSet(DAPI_CMD_t cmd)
{
  dapiTraceCmdIgnore = cmd;
}

/*********************************************************************
* @purpose  Only let the trace buffer fill once
*
* @param    enable      @b{(input)} L7_TRUE/L7_FALSE to enable/disable oneshot
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceOneShotSet(L7_BOOL enable)
{
  if (enable)
  {
    dapiTraceOneShot = 1;

    if (!dapiTraceMode)
    dapiTraceModeApply(L7_TRUE);
  }
  else
  {
    dapiTraceOneShot = 0;
    dapiTraceModeApply(L7_TRUE);
  }
}

/*********************************************************************
* @purpose  Set the trace mask of what to trace
*
* @param    mask        @b{(input)} The mask
*
* @returns  void
*
* @end
*********************************************************************/
L7_uint32 dapiTraceMaskSet(L7_uint32 mask)
{
  dapiTraceMask = mask;
  /* if we are setting it to FRAME, set all frame types */
  if ((mask & DAPI_TRACE_FRAME_MASK) && (dapiTraceFrameType == 0))
      dapiTraceFrameTypeSet((1 << DAPI_TRACE_FRAME_DROP) |
                            (1 << DAPI_TRACE_FRAME_FWD)  |
                            (1 << DAPI_TRACE_FRAME_SEND));

  return mask;
}

/*********************************************************************
* @purpose  Format the output for a dapiCtl event
*
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
static L7_RC_t dapiTraceDapiCtlFormat(L7_uchar8 *pDataStart)
{
  DAPI_CMD_t cmd;
  L7_uint32  uspInt;
  DAPI_USP_t usp;
  L7_RC_t    traceRc;

  cmd = (DAPI_CMD_t)osapiNtohl(*(L7_ulong32*)&pDataStart[0]);
  uspInt = (L7_uint32)osapiNtohl(*(L7_ulong32*)&pDataStart[4]);
  USP_DECOMPRESS(uspInt,&usp);
  traceRc = (L7_RC_t)osapiNtohl(*(L7_ulong32*)&pDataStart[8]);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s - %d/%d/%d - %s %d",dapiCommandToStringGet(cmd),
                usp.unit,usp.slot,usp.port,(pDataStart[12])?"enter":"exit",traceRc);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format the output of trace generic
*
* @param    pDataStart  @b{(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
static L7_RC_t dapiTraceGenericFormat(L7_uchar8 *pDataStart)
{
  L7_uint32 len;

  len = strlen(pDataStart);

  if (len > (dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + 1)))
    pDataStart[dapiTraceMaxRecordWidth - 1] = L7_NULL;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s",pDataStart);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Format the output for a dapiCallback event
*
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
static L7_RC_t dapiTraceDapiCallbackFormat(L7_uchar8 *pDataStart)
{
  DAPI_EVENT_t cmd;
  L7_uint32  uspInt;
  L7_RC_t    traceRc;
  L7_uchar8 eventStr[16];
  DAPI_USP_t usp;

  cmd = (DAPI_CMD_t)osapiNtohl(*(L7_ulong32*)&pDataStart[0]);
  uspInt = (L7_uint32)osapiNtohl(*(L7_ulong32*)&pDataStart[4]);
  USP_DECOMPRESS(uspInt,&usp);
  traceRc = (L7_RC_t)osapiNtohl(*(L7_ulong32*)&pDataStart[8]);

  switch (cmd)
  {
    case  DAPI_EVENT_FRAME_RX:
      osapiStrncpy(eventStr, "Recv", sizeof(eventStr));
      break;
    case  DAPI_EVENT_INTF_LINK_DOWN:
      osapiStrncpy(eventStr,"DOWN", sizeof(eventStr));
      break;
    case DAPI_EVENT_INTF_LINK_UP:
      osapiStrncpy(eventStr,"UP", sizeof(eventStr));
      break;
    case DAPI_EVENT_ADDR_LEARNED_ADDRESS:
      osapiStrncpy(eventStr,"Addr LEARN", sizeof(eventStr));
      break;
    case DAPI_EVENT_ADDR_AGED_ADDRESS:
      osapiStrncpy(eventStr,"Addr AGED", sizeof(eventStr));
      break;
    default:
      osapiSnprintf(eventStr, sizeof(eventStr),"Evt(%d)",cmd);
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "event %s - %d/%d/%d - %s %d",eventStr,
                usp.unit,usp.slot,usp.port,(pDataStart[12])?"enter":"exit",traceRc);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format the output for a frame event
*
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
static L7_RC_t dapiTraceFrameFormat(L7_uchar8 *pDataStart)
{
  L7_uint32  uspInt,numBytes,offSet;
  DAPI_USP_t    usp;
  L7_uchar8 drop;
  L7_uint32 cookie;
  L7_ulong32 tmp;

  memcpy(&tmp,&pDataStart[0],sizeof(tmp));
  uspInt = (L7_uint32)osapiNtohl(tmp);
  USP_DECOMPRESS(uspInt,&usp);
  pDataStart += sizeof(uspInt);
  drop = *pDataStart;
  pDataStart += sizeof(drop);
  memcpy(&tmp,pDataStart,sizeof(tmp));
  cookie = (L7_uint32)osapiNtohl(tmp);

  pDataStart += sizeof(cookie);

  numBytes = dapiTraceMaxRecordWidth - (sizeof(dapiTracePtHdr_t) + sizeof(uspInt) + sizeof(drop) + sizeof(cookie));

  if (drop == DAPI_TRACE_FRAME_SEND)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d/%d/%d, %s, %0.8x, ",usp.unit,usp.slot,usp.port,"S",cookie);
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d/%d/%d, %s, %0.8x, ",usp.unit,usp.slot,usp.port,
                  (drop==DAPI_TRACE_FRAME_DROP)?"D":"F",cookie);
  }

  offSet = 0;

  if (numBytes > 18)
  {
    /* DA */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x ",
                  pDataStart[0],pDataStart[1],
                  pDataStart[2],pDataStart[3],
                  pDataStart[4],pDataStart[5]);

    /* SA */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x ",
                  pDataStart[6],pDataStart[7],
                  pDataStart[8],pDataStart[9],
                  pDataStart[10],pDataStart[11]);

    /* VPI and VID */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%0.2x%0.2x %0.2x%0.2x ",
                  pDataStart[12],pDataStart[13],
                  pDataStart[14],pDataStart[15]);

    /* ETYPE */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%0.2x%0.2x ",
                  pDataStart[16],pDataStart[17]);

    offSet = 18;
  }

  for (; offSet < numBytes; offSet++)
  {
        if ((offSet > 0) && (!(offSet % 8)))
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, " ");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
            "%0.2x",pDataStart[offSet]);
  }

  return L7_SUCCESS;
}

L7_uint32 dapiTraceFrameTypeSet(L7_uint32 type)
{
  L7_uint32 old = dapiTraceFrameType;
  dapiTraceFrameType = type;
  return old;
}


void dapiTraceHelp()
{
  DAPI_USP_t usp;
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dapiTraceMaskSet(mask)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dapiTraceIntIfSet(unit,slot,port)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dapiTraceOneShotSet(enable)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Interface is in the form      0x0000uspp\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DAPI_TRACE_DAPI_CTL_MASK      %0.8x\n",DAPI_TRACE_DAPI_CTL_MASK);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DAPI_TRACE_GENERIC_MASK       %0.8x\n",DAPI_TRACE_GENERIC_MASK);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DAPI_TRACE_DAPI_CB_MASK       %0.8x\n",DAPI_TRACE_DAPI_CB_MASK);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DAPI_TRACE_FRAME_MASK         %0.8x\n",DAPI_TRACE_FRAME_MASK);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DAPI_TRACE_STACK_EVENT_MASK   %0.8x\n",DAPI_TRACE_STACK_EVENT_MASK);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "******************************\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dapiTraceFrameTypeSet(mask)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Receive: DROPPED %0.8x\n",1 << DAPI_TRACE_FRAME_DROP);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Receive: To App  %0.8x\n",1 << DAPI_TRACE_FRAME_FWD);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Send             %0.8x\n",1 << DAPI_TRACE_FRAME_SEND);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "******************************\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Dapi Trace Mask   = %0.8x\n",dapiTraceMask);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Dapi Trace FRAMES = %0.8x\n",dapiTraceFrameType);

  USP_DECOMPRESS(dapiTraceIntIf,&usp);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Int      = %d/%d/%d\n",usp.unit,usp.slot,usp.port);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "oneShot  = %d\n",dapiTraceOneShot);

}

/*********************************************************************
* @purpose  Clean-up the resources required for profiling Dapi
*
*
* @returns  void
*
* @end
*********************************************************************/
void dapiProfileFini(void)
{
  if (dapiCmdProfileInfo != L7_NULLPTR)
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, dapiCmdProfileInfo);
    dapiCmdProfileInfo = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  Allocate the resources required for profiling Dapi
*
*
* @returns  void
*
* @end
*********************************************************************/
void dapiProfileInit(void)
{
  static L7_BOOL dapiRegSysProfile = L7_FALSE;
  compProfileRegistrationInfo_t sysProfileRegInfo;

  if (dapiRegSysProfile == L7_FALSE)
  {
    sysProfileRegInfo.enable = L7_TRUE;
    sysProfileRegInfo.compProfileModeSet = dapiProfileModeSet;
    sysProfileRegInfo.compProfileInfoShow = dapiProfileInfoShow;
    sysProfileRegisterComponent(L7_DRIVER_COMPONENT_ID, sysProfileRegInfo);
    dapiRegSysProfile = L7_TRUE;
  }

  if (dapiProfileEnable)
  {
    dapiCmdProfileInfo = (dapiCmdProfileInfo_t *) osapiMalloc(L7_DRIVER_COMPONENT_ID, (DAPI_NUM_OF_CMDS + 1) * sizeof(dapiCmdProfileInfo_t));

    if (dapiCmdProfileInfo == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }
  }
}

void dapiProfileModeSet(L7_uint32 enable)
{
  dapiProfileEnable = enable;
  if (dapiProfileEnable == L7_TRUE)
  {
    dapiProfileInit();
  }
  else
  {
    dapiProfileFini();
  }
}

void dapiProfileConfigShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Profiling is %s\n",(dapiProfileEnable)?"enabled":"disabled");

}
/*********************************************************************
* @purpose  Start profiling of a dapi cmd
*
* @param    dapiCmd    {(input)}   The dapi command being profiled
* @param    start      {(input)}   Indicates start or end of the cmd
*
* @returns  void
*
* @end
*********************************************************************/
void dapiCmdProfile(DAPI_CMD_t dapiCmd, L7_BOOL start)
{
  L7_uint32 currTime;

  if (dapiProfileEnable)
  {
    if (start == L7_TRUE)
    {
      dapiCmdProfileInfo[dapiCmd].numInvocations++;
      dapiCmdProfileInfo[dapiCmd].startTime = osapiTimeMillisecondsGet();
    }
    else
    {
      currTime = osapiTimeMillisecondsGet() - dapiCmdProfileInfo[dapiCmd].startTime;
      dapiCmdProfileInfo[dapiCmd].totalTime += currTime;
      if (currTime > dapiCmdProfileInfo[dapiCmd].maxTime)
      {
        dapiCmdProfileInfo[dapiCmd].maxTime = currTime;
      }
    }
  }
}

void dapiProfileInfoShow(void)
{
  L7_uint32 i;


  if (dapiProfileEnable)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n             Dapi Cmd                                       Total time (ms)      Number of invocations      Maximum time (ms)  \n"
                 );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "----------------------------------------                    --------------       --------------------      ------------------\n");

    for (i = 0; i < DAPI_NUM_OF_CMDS; i++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "%-2d - %-55s          %-8d          %-8d                %-8d\n",
                    i, dapiCommandToStringGet(i),
                    dapiCmdProfileInfo[i].totalTime,
                    dapiCmdProfileInfo[i].numInvocations,
                    dapiCmdProfileInfo[i].maxTime
                   );
    }
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Dapi cmd profiling is not enabled\n");

  }
}
void dapiProfileHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Enable dapi cmds profiling\n","dapiProfileModeSet(enable)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show dapi events profiling configuration\n","dapiProfileConfigShow()");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show profiling Information for cnfgr events\n","dapiProfileInfoShow()");
}
