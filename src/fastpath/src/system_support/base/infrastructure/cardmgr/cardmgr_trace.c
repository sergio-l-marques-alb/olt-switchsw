/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_trace.c
*
* @purpose    Trace implementation for CMGR
*
* @component  CMGR
*
* @comments   none
*
* @create     03/08/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include <string.h>

#include "osapi_support.h"
#include "cardmgr_trace.h"

/* Global variables */
static L7_uint32 cmgrTraceHandle = ~0;
static L7_uint32 cmgrTraceMask   = 0;
static L7_BOOL   cmgrTraceMode   = L7_FALSE;

/* Profile variables */
L7_BOOL  cmgrProfileEnable = L7_FALSE;
static cmgrProfileEvent_t  *cmgrProfileData[L7_MAX_UNITS_PER_STACK + 1] = {L7_NULLPTR};

/* Local prototypes */
void cmgrTraceMaskSet(L7_uint32 mask);
void cmgrTraceModeApply(L7_BOOL enable);
void cmgrSysTraceModeApply(L7_uint32 mode);
void cmgrSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart);
void cmgrProfileModeSet(L7_uint32 mode);
void cmgrProfileInfoShow();

/*********************************************************************
* @purpose  Initialize the trace utility for CMGR
*
* @param    num_entries   {(input)}  max entries in the buffer
* @param    width         {(input)}  width in bytes of each entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
void cmgrTraceInit(L7_uint32 num_entries, L7_uint32 width)

{
  /* make sure sizes make sense */
  if (width < CMGR_TRACE_ENTRY_SIZE_MAX) width = CMGR_TRACE_ENTRY_SIZE_MAX;
  if (num_entries == 0) num_entries = CMGR_TRACE_ENTRY_MAX;

  /* if allocated, clean it up */
  if (cmgrTraceHandle != ~0) (void)cmgrTraceFini();

  /* acquire a trace buffer */
  if (traceBlockCreate(num_entries, width,
                       (L7_uchar8 *)CMGR_TRACE_REGISTRATION_NAME,
                       &cmgrTraceHandle) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
            "CMGR: unable to create the trace buffer\n");
    cmgrTraceHandle = ~0;
  }
  else
  {
    compTraceRegistrationInfo_t sysTraceRegInfo;

    cmgrTraceMaskSet(CMGR_TRACE_CARD_EVENTS);
    cmgrTraceModeApply(L7_TRUE);

    sysTraceRegInfo.compTraceHandle = cmgrTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = cmgrSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = cmgrSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_TRUE;
    sysTraceRegisterComponent(L7_CARDMGR_COMPONENT_ID, sysTraceRegInfo);
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
void cmgrTraceFini(void)
{
  compTraceRegistrationInfo_t sysTraceRegInfo;

  if (cmgrTraceHandle != ~0)
  {
    /* stop tracing at the mask level */
    cmgrTraceMaskSet(0);

    /* disable the trace util */
    cmgrTraceModeApply(L7_FALSE);

    /* delete the buffer */
    if (traceBlockDelete(cmgrTraceHandle) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
              "CMGR: unable to delete the trace buffer\n");
    }
    else
    {
      cmgrTraceHandle = ~0;
    }

    sysTraceRegInfo.compTraceHandle = cmgrTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = cmgrSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = cmgrSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_FALSE;
    sysTraceRegisterComponent(L7_CARDMGR_COMPONENT_ID, sysTraceRegInfo);

  }

  return ;
}

/*********************************************************************
* @purpose  Trace a cmpdu event
*
* @param    traceId     {(input)}   Trace identifier
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    fromUnitNum {(input)}   Sender unit number
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrTraceCmpduEvent(L7_ushort16         traceId,
                         L7_cmpdu_packet_t   event,
                         L7_BOOL             start,
                         L7_uint32           fromUnitNum
                        )
{
  if ((cmgrTraceHandle != ~0) &&
      (cmgrTraceMode == L7_TRUE) &&
      (cmgrTraceMask & CMGR_TRACE_CMPDU_EVENTS)
     )
  {
    TRACE_BEGIN(cmgrTraceHandle, L7_CARDMGR_COMPONENT_ID);

    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(event));
    TRACE_ENTER_4BYTE(osapiHtonl(start));
    TRACE_ENTER_4BYTE(osapiHtonl(fromUnitNum));

    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace a card event
*
* @param    traceId     {(input)}   Trace identifier
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    unitNum     {(input)}   Unit number
* @param    slotNum     {(input)}   Slot number
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrTraceCardEvent(L7_ushort16         traceId,
                        L7_cmpdu_packet_t   event,
                        L7_BOOL             start,
                        L7_uint32           unitNum,
                        L7_uint32           slotNum
                       )
{
  if ((cmgrTraceHandle != ~0) &&
      (cmgrTraceMode == L7_TRUE) &&
      (cmgrTraceMask & CMGR_TRACE_CARD_EVENTS)
     )
  {
    TRACE_BEGIN(cmgrTraceHandle, L7_CARDMGR_COMPONENT_ID);

    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(event));
    TRACE_ENTER_4BYTE(osapiHtonl(start));
    TRACE_ENTER_4BYTE(osapiHtonl(unitNum));
    TRACE_ENTER_4BYTE(osapiHtonl(slotNum));

    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Enable tracing in the CMGR component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrTraceModeApply(L7_BOOL enable)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (enable == L7_TRUE)
    rc = traceBlockStart(cmgrTraceHandle);
  else
    rc = traceBlockStop(cmgrTraceHandle);

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
            "cmgrTraceModeApply: Unable to apply CMGR trace mode (%u), "
            "handle=0x%8.8x\n",enable , cmgrTraceHandle);
  }
  else
  {
    cmgrTraceMode = enable;
  }
}

/*********************************************************************
* @purpose  Apply sys tracing mode
*
* @param    mode
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrSysTraceModeApply(L7_uint32 mode)
{
  cmgrTraceModeApply(mode);

  cmgrTraceMaskSet(CMGR_TRACE_CARD_EVENTS);
  if (mode == SYS_TRACE_ALL_EVENTS)
  {
    cmgrTraceMaskSet(CMGR_TRACE_CMPDU_EVENTS);
  }
}
/*********************************************************************
* @purpose  Format the output of a Cmpdu event
*
* @param    traceId     @{(input)} Trace point identifier
* @param    pDataStart  @{(input)} Start of trace point specific info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The pDataStart parm points to the portion of the trace entry
*           following the trace point header.
*
* @end
*********************************************************************/
L7_RC_t cmgrTraceFmtCmpduEvent(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  cmgrTracePtCmpduEvent_t *p = (cmgrTracePtCmpduEvent_t *)pDataStart;
  L7_char8                 tmpStr[100];


  switch (osapiNtohl(p->eventId))
  {
    case L7_CMPDU_STATUS_UPDATE:
      osapiStrncpy((L7_char8 *)tmpStr, "STATUS_UPDATE", sizeof(tmpStr));
      break;
    case L7_CMPDU_STATUS_REQUEST:
      osapiStrncpy((L7_char8 *)tmpStr, "STATUS_REQUEST", sizeof(tmpStr));
      break;
    case L7_CMPDU_REQUEST_ALL_INFO:
      osapiStrncpy((L7_char8 *)tmpStr, "REQUEST_ALL", sizeof(tmpStr));
      break;
    case L7_CMPDU_CARD_REPORT_CARD_PLUGIN:
      osapiStrncpy((L7_char8 *)tmpStr, "REPORT_CARD_PLUGIN", sizeof(tmpStr));
      break;
    case L7_CMPDU_CARD_REPORT_CARD_UNPLUG:
      osapiStrncpy((L7_char8 *)tmpStr, "REPORT_CARD_UNPLUG", sizeof(tmpStr));
      break;
    case L7_CMPDU_CNFGR_REQUEST:
      osapiStrncpy((L7_char8 *)tmpStr, "CNFGR_REQUEST", sizeof(tmpStr));
      break;
    case L7_CMPDU_TIMER_TIMEOUT:
      osapiStrncpy((L7_char8 *)tmpStr, "TIMER_TIMEOUT", sizeof(tmpStr));
      break;
    case L7_CMPDU_CARD_REPORT_CARD_FAILURE:
      osapiStrncpy((L7_char8 *)tmpStr, "REPORT_CARD_FAILURE", sizeof(tmpStr));
      break;
    default:
      osapiStrncpy((L7_char8 *)tmpStr, "UNKNOWN", sizeof(tmpStr));
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"Event=%-22s ", tmpStr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"%-7s ", (p->eventStatus) ? \
                CMGR_EVENT_PROGRESS_START_STRING :\
                CMGR_EVENT_PROGRESS_FINISHED_STRING
               );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"fromUnitNum=%-8d ", osapiNtohl(p->fromUnitNum));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format the output of a Card event
*
* @param    traceId     @{(input)} Trace point identifier
* @param    pDataStart  @{(input)} Start of trace point specific info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The pDataStart parm points to the portion of the trace entry
*           following the trace point header.
*
* @end
*********************************************************************/
L7_RC_t cmgrTraceFmtCardEvent(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  cmgrTracePtCardEvent_t *p = (cmgrTracePtCardEvent_t *)pDataStart;
  L7_char8                tmpStr[100];

  switch (osapiNtohl(p->eventId))
  {
    case L7_CMGR_CARD_INSERT:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_INSERT", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_REMOVE:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_REMOVE", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_SYNC:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_SYNC", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_CREATE:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_CREATE", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_DELETE:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_DELETE", sizeof(tmpStr));
      break;
    case L7_CMGR_LOCAL_UNIT_IS_MANAGER:
      osapiStrncpy((L7_char8 *)tmpStr, "LOCAL_UNIT_MGR", sizeof(tmpStr));
      break;
    case L7_CMGR_LOCAL_UNIT_IS_NOT_MANAGER:
      osapiStrncpy((L7_char8 *)tmpStr, "LOCAL_UNIT_NOT_MGR", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_CREATE_COMPLETE:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_CREATE_COMPL", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_CONFIG_SLOTS:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_CONFIG_SLOTS", sizeof(tmpStr));
      break;
    case L7_CMGR_CARD_CONFIG_PORTS:
      osapiStrncpy((L7_char8 *)tmpStr, "CARD_CONFIG_PORTS", sizeof(tmpStr));
      break;
    case L7_CMGR_UNIT_CONNECTED:
      osapiStrncpy((L7_char8 *)tmpStr, "UNIT_CONNECTED", sizeof(tmpStr));
      break;
    case L7_CMGR_UNIT_DISCONNECTED:
      osapiStrncpy((L7_char8 *)tmpStr, "UNIT_DISCONNECTED", sizeof(tmpStr));
      break;
    case L7_CMGR_STARTUP_CREATE:
      osapiStrncpy((L7_char8 *)tmpStr, "STARTUP_CREATE", sizeof(tmpStr));
      break;
    case L7_CMGR_STARTUP_ACTIVATE:
      osapiStrncpy((L7_char8 *)tmpStr, "STARTUP_ACTIVATE", sizeof(tmpStr));
      break;
    case L7_CMGR_HW_APPLY_EVENT:
      osapiStrncpy((L7_char8 *)tmpStr, "HW_APPLY", sizeof(tmpStr));
      break;

    default:
      osapiStrncpy((L7_char8 *)tmpStr, "UNKNOWN", sizeof(tmpStr));
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"Event=%-23s ", tmpStr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"%-7s ", (p->eventStatus) ? \
                CMGR_EVENT_PROGRESS_START_STRING :\
                CMGR_EVENT_PROGRESS_FINISHED_STRING
               );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"unitNum=%-8d slotNum=%-8d ",
                osapiNtohl(p->unitNum),
                osapiNtohl(p->slotNum)
               );

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Display the Card Manager component trace data
*
* @param    count       @{(input)} Number of entries to display
* @param    format      @{(input)} Format trace output (L7_TRUE, L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A count value of 0 is used to display all entries.
*
* @end
*********************************************************************/
L7_RC_t cmgrTraceShow(L7_uint32 count, L7_BOOL format)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(cmgrTraceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate Card Manager trace block for handle "
                  "0x%8.8x\n", cmgrTraceHandle);
    return L7_FAILURE;
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
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

      if (format == L7_FALSE)
      {
        for (j = 1, jmax = traceData->bytesPerEntry; j <= jmax; j++)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%2.2x ", *pOffset++);
          if (((j % CMGR_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

      } /* endif unformatted */

      else  /* manually format output */
      {
        cmgrTraceFmtFunc_t     fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8             *pIdStr;
        cmgrTracePtHdr_t        *pHdr;

        /* the first 8 bytes are the same for all trace entries */
        pHdr = (cmgrTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case CMGR_TRACE_ID_CMPDU_EVENT:
            pIdStr = (L7_uchar8 *)CMGR_CMPDU_EVENTS_STRING;
            fmtFunc = cmgrTraceFmtCmpduEvent;
            break;

          case CMGR_TRACE_ID_CARD_EVENT:
            pIdStr = (L7_uchar8 *)CMGR_CARD_EVENTS_STRING;
            fmtFunc = cmgrTraceFmtCardEvent;
            break;

          default:
            pIdStr = (L7_uchar8 *)"??????";
            break;
        }
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"%-12s: ", pIdStr);

        /* show the entry timestamp */
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"ts=%4.4d ",
                      (L7_uint32)osapiHtonl(pHdr->timeStamp));

        /* format the rest of the entry per the id value */
        if (fmtFunc != L7_NULLPTR)
        {
          if ((*fmtFunc)(traceId, pOffset) != L7_SUCCESS)
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                          "   *** ERROR (cannot format this entry) ***");
          }
        }
      } /* endelse manually format output */

      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

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

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Card Manager trace format function registered with sysTrace
*
* @param    traceId     @{(input)} Trace point identifier
* @param    timeStamp   @{(input)} Timestamp of the entry
* @param    pDataStart  @{(input)} Start of trace point specific info
*
* @returns  void
*
*
* @end
*********************************************************************/
void cmgrSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart)
{
  cmgrTraceFmtFunc_t     fmtFunc = L7_NULLPTR; /* id-specific formatter  */
  L7_uchar8             *pIdStr;


  /* translate trace code to a display string */
  switch (traceId)
  {
    case CMGR_TRACE_ID_CMPDU_EVENT:
      pIdStr = (L7_uchar8 *)CMGR_CMPDU_EVENTS_STRING;
      fmtFunc = cmgrTraceFmtCmpduEvent;
      break;

    case CMGR_TRACE_ID_CARD_EVENT:
      pIdStr = (L7_uchar8 *)CMGR_CARD_EVENTS_STRING;
      fmtFunc = cmgrTraceFmtCardEvent;
      break;

    default:
      pIdStr = (L7_uchar8 *)"??????";
      break;
   }

   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                 (char *)"%-15s: ", pIdStr);

   /* show the entry timestamp */
   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                 (char *)"ts=%4.4u ",
                 (L7_uint32)osapiHtonl(timeStamp));

   /* format the rest of the entry per the id value */
   if (fmtFunc != L7_NULLPTR)
   {
     if ((*fmtFunc)(traceId, pDataStart) != L7_SUCCESS)
     {
       SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                     "   *** ERROR (cannot format this entry) ***");
     }
   }

   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
}

void cmgrTraceMaskSet(L7_uint32 mask)
{
  cmgrTraceMask = mask;
}


void cmgrTraceConfigPrint(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Tracing is %s\n",(cmgrTraceMode)?"enabled":"disabled");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","Trace Mask",cmgrTraceMask);

}

void cmgrTraceHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - sets what gets traced\n","cmgrTraceMaskSet(mask)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%30s = 0x%0.8x\n","CMGR_TRACE_CMPDU_EVENTS    ",CMGR_TRACE_CMPDU_EVENTS);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%30s = 0x%0.8x\n","CMGR_TRACE_CARD_EVENTS     ",CMGR_TRACE_CARD_EVENTS);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - print CMGR tracing configuration\n","cmgrTraceConfigPrint()" );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - 1 is enabled, 0 is disabled\n","cmgrTraceModeApply(enable)");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Display CMGR trace buffer\n","cmgrTraceShow(numEntries,format)");

}

/*********************************************************************
* @purpose  Clean-up the resources required for profiling CMGR
*
*
* @returns void
*
* @end
*********************************************************************/
void cmgrProfileFini(void)
{
  L7_uint32 i;

  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {

    if (cmgrProfileData[i] != L7_NULLPTR)
    {
      osapiFree(L7_CARDMGR_COMPONENT_ID, cmgrProfileData[i]);
      cmgrProfileData[i] = L7_NULLPTR;
    }
  }
}

/*********************************************************************
* @purpose  Allocate the resources required for profiling CMGR
*
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrProfileInit(void)
{
  L7_uint32 i;
  static L7_BOOL cmgrRegSysProfile = L7_FALSE;
  compProfileRegistrationInfo_t sysProfileRegInfo;

  if (cmgrRegSysProfile == L7_FALSE)
  {
    sysProfileRegInfo.enable = L7_TRUE;
    sysProfileRegInfo.compProfileModeSet = cmgrProfileModeSet;
    sysProfileRegInfo.compProfileInfoShow = cmgrProfileInfoShow;
    sysProfileRegisterComponent(L7_CARDMGR_COMPONENT_ID, sysProfileRegInfo);
    cmgrRegSysProfile = L7_TRUE;
  }

  cmgrProfileFini();
  if (cmgrProfileEnable == L7_TRUE)
  {
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {

      cmgrProfileData[i] = (cmgrProfileEvent_t *) osapiMalloc(L7_CARDMGR_COMPONENT_ID,
                                                              L7_MAX_SLOTS_PER_UNIT *
                                                              sizeof(cmgrProfileEvent_t));
      if (cmgrProfileData[i] == L7_NULLPTR)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      }
    }
  }
}

void cmgrProfileModeSet(L7_BOOL enable)
{
  cmgrProfileEnable = enable;
  if (cmgrProfileEnable == L7_TRUE)
  {
    cmgrProfileInit();
  }
  else
  {
    cmgrProfileFini();
  }
}

void cmgrProfileConfigShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Profiling is %s\n",(cmgrProfileEnable)?"enabled":"disabled");

}

/*********************************************************************
* @purpose  Profile a card event
*
* @param    event       {(input)}   The event being profiled
* @param    start       {(input)}   Indicates start or end of the event
* @param    unitNum     {(input)}   Unit number
* @param    slotNum     {(input)}   Slot number
*
* @returns  void
*
* @end
*********************************************************************/
void cmgrProfileEvent(cmgrProfileEvents_t event,
                      L7_BOOL             start,
                      L7_uint32           unitNum,
                      L7_uint32           slotNum
                     )
{

  if (cmgrProfileEnable != L7_TRUE)
  {
    return;
  }

  switch(event)
  {
    case CMGR_PROFILE_EVENT_CARD_CREATE:
    case CMGR_PROFILE_EVENT_CARD_INSERT:
    case CMGR_PROFILE_EVENT_CARD_ATTACH:
      if (cmgrProfileData[unitNum] != L7_NULLPTR)
      {
        if (start)
        {
          cmgrProfileData[unitNum][slotNum].eventInfo[event].tempTime = osapiTimeMillisecondsGet();
        }
        else
        {
          //cmgrProfileData[unitNum][slotNum].eventInfo[event].totalTime = osapiTimeMillisecondsGet() - cmgrProfileData[unitNum][slotNum].eventInfo[event].tempTime;
          cmgrProfileData[unitNum][slotNum].eventInfo[event].totalTime = osapiTimeMillisecondsGetOffset(cmgrProfileData[unitNum][slotNum].eventInfo[event].tempTime);
        }
      }
      break;
    case CMGR_PROFILE_EVENT_CARD_CONFIG_SLOTS:
    case CMGR_PROFILE_EVENT_CARD_CONFIG_PORTS:
    case CMGR_PROFILE_EVENT_CARD_LOCAL_UNIT_MGR:
    case CMGR_PROFILE_EVENT_STARTUP_CREATE:
    case CMGR_PROFILE_EVENT_STARTUP_ACTIVATE:
      if (cmgrProfileData[0] != L7_NULLPTR)
      {
        if (start)
        {
          cmgrProfileData[0][0].eventInfo[event].tempTime = osapiTimeMillisecondsGet();
        }
        else
        {
          //cmgrProfileData[0][0].eventInfo[event].totalTime = osapiTimeMillisecondsGet() - cmgrProfileData[0][0].eventInfo[event].tempTime;
          cmgrProfileData[0][0].eventInfo[event].totalTime = osapiTimeMillisecondsGetOffset(cmgrProfileData[0][0].eventInfo[event].tempTime);
        }
      }
      break;
    default:
      break;
  }

  return;
}

void cmgrProfileInfoShow(void)
{
  L7_uint32 u,s;

  if (cmgrProfileEnable)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\nUnit/Slot               Card Create time(ms)           DTL Card Insert time(ms)             Card Attach time(ms)  \n"
                 );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "---------               --------------------             --------------------             --------------------\n"
                 );
    for (u = 0; u <= L7_MAX_UNITS_PER_STACK; u++)
    {
      for (s = 0; s < L7_MAX_SLOTS_PER_UNIT; s++)
      {
        if (cmgrProfileData[u] != L7_NULLPTR)
        {
          if (cmgrProfileData[u][s].eventInfo[CMGR_PROFILE_EVENT_CARD_CREATE].totalTime)
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                          "%d/%d                        %-8u                                  %-8u                       %-8u\n",
                           u, s,
                           osapiHtonl(cmgrProfileData[u][s].eventInfo[CMGR_PROFILE_EVENT_CARD_CREATE].totalTime),
                           osapiHtonl(cmgrProfileData[u][s].eventInfo[CMGR_PROFILE_EVENT_CARD_INSERT].totalTime),
                           osapiHtonl(cmgrProfileData[u][s].eventInfo[CMGR_PROFILE_EVENT_CARD_ATTACH].totalTime)
                         );

          }
        }
      }
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\nGlobal config time(ms)             Interface config time(ms)           Local_unit_manager event time(ms)\n"
                 );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "-----------------------            -------------------------          --------------------------------\n");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%-8u                                    %-8u                                   %-8u\n",
                  osapiHtonl(cmgrProfileData[0][0].eventInfo[CMGR_PROFILE_EVENT_CARD_CONFIG_SLOTS].totalTime),
                  osapiHtonl(cmgrProfileData[0][0].eventInfo[CMGR_PROFILE_EVENT_CARD_CONFIG_PORTS].totalTime),
                  osapiHtonl(cmgrProfileData[0][0].eventInfo[CMGR_PROFILE_EVENT_CARD_LOCAL_UNIT_MGR].totalTime)
                 );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\nStartup Create time(ms)          Startup Activate time(ms) \n"
                 );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "-----------------------            ------------------------- \n");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%-8u                                    %-8u \n",
                  osapiHtonl(cmgrProfileData[0][0].eventInfo[CMGR_PROFILE_EVENT_STARTUP_CREATE].totalTime),
                  osapiHtonl(cmgrProfileData[0][0].eventInfo[CMGR_PROFILE_EVENT_STARTUP_ACTIVATE].totalTime)
                 );


  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Cmgr Profiling is not enabled.\n"
                 );

  }

  return;

}


void cmgrProfileHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Enable cmgr events profiling\n","cmgrProfileModeSet(enable)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show cmgr events profiling configuration\n","cmgrProfileConfigShow()");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show profiling Information for cmgr events\n","cmgrProfileInfoShow()");
}

