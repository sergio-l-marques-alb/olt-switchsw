/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_trace.c
*
* @purpose    Trace implementation for CNFGR
*
* @component  CNFGR
*
* @comments   none
*
* @create     02/25/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include <string.h>

#include "osapi_support.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "cnfgr_trace.h"

/* Global variables */
static L7_uint32                cnfgrTraceHandle = ~0;
static L7_uint32                cnfgrTraceMask   = 0;
static L7_BOOL                  cnfgrTraceMode   = L7_FALSE;
static CNFGR_CTLR_EVNT_t        currTraceCnfgrEvent = CNFGR_EVNT_FIRST;
static L7_CNFGR_RQST_t          currTraceCnfgrReqEvent = L7_CNFGR_RQST_FIRST;

/* Profiling variables */
static cnfgrProfileCompInfo_t *cnfgrProfileEventCompInfo = L7_NULLPTR;
static cnfgrProfileEvent_t    *cnfgrProfileEventInfo = L7_NULLPTR;
static L7_BOOL                 cnfgrProfileEnable = L7_FALSE;
static CNFGR_CTLR_EVNT_t       currProfileCnfgrEvent = CNFGR_EVNT_FIRST;
static L7_CNFGR_RQST_t         currProfileCnfgrReqEvent = L7_CNFGR_RQST_FIRST;

void cnfgrTraceModeApply(L7_BOOL enable);
void cnfgrTraceMaskSet(L7_uint32 mask);
void cnfgrSysTraceModeApply(L7_uint32 enable);
void cnfgrSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart);

void cnfgrProfileModeSet(L7_uint32 mode);
void cnfgrProfileInfoShow();

/*********************************************************************
* @purpose  Initialize the trace utility for CNFGR
*
* @param    num_entries   {(input)}  max entries in the buffer 
* @param    width         {(input)}  width in bytes of each entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
void cnfgrTraceInit(L7_uint32 num_entries, L7_uint32 width)

{
  /* make sure sizes make sense */
  if (width < CNFGR_TRACE_ENTRY_SIZE_MAX) width = CNFGR_TRACE_ENTRY_SIZE_MAX;
  if (num_entries == 0) num_entries = CNFGR_TRACE_ENTRY_MAX;         

  /* if allocated, clean it up */
  if (cnfgrTraceHandle != ~0) (void)cnfgrTraceFini();

  /* acquire a trace buffer */
  if (traceBlockCreate(num_entries, width,
                       (L7_uchar8 *)CNFGR_TRACE_REGISTRATION_NAME, 
                       &cnfgrTraceHandle) != L7_SUCCESS)
  {
    LOG_MSG("CNFGR: unable to create the trace buffer\n");
    cnfgrTraceHandle = ~0;
  }
  else
  {
    compTraceRegistrationInfo_t sysTraceRegInfo;

    cnfgrTraceModeApply(L7_TRUE);
    cnfgrTraceMaskSet( CNFGR_TRACE_EVENT );

    sysTraceRegInfo.compTraceHandle = cnfgrTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = cnfgrSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = cnfgrSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_TRUE;
    sysTraceRegisterComponent(L7_CNFGR_COMPONENT_ID, sysTraceRegInfo);
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
void cnfgrTraceFini(void)
{
  compTraceRegistrationInfo_t sysTraceRegInfo;

  if (cnfgrTraceHandle != ~0)
  {
    cnfgrTraceMaskSet(0);

    /* disable the trace util */
    cnfgrTraceModeApply(L7_FALSE);

    /* delete the buffer */
    if (traceBlockDelete(cnfgrTraceHandle) != L7_SUCCESS)
    {
      LOG_MSG("CNFGR: unable to delete the trace buffer\n");
    }
    else
    {
      cnfgrTraceHandle = ~0;
    }

    sysTraceRegInfo.compTraceHandle = cnfgrTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = cnfgrSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = cnfgrSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_FALSE;
    sysTraceRegisterComponent(L7_CNFGR_COMPONENT_ID, sysTraceRegInfo);

  }
  return ;
}

/*********************************************************************
* @purpose  Trace a cnfgr event
*
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrTraceEvent(CNFGR_CTLR_EVNT_t   event,
                     L7_BOOL             start
                    )
{
  if ((cnfgrTraceHandle != ~0) && 
      (cnfgrTraceMode == L7_TRUE) &&
      (cnfgrTraceMask & CNFGR_TRACE_EVENT)
     )
  {
    if (start == L7_TRUE) 
    {
      currTraceCnfgrEvent = event;
    }

    TRACE_BEGIN(cnfgrTraceHandle, L7_CNFGR_COMPONENT_ID);

    TRACE_ENTER_2BYTE((start)?osapiHtons(CNFGR_EVENT_START):osapiHtons(CNFGR_EVENT_FINI));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(currTraceCnfgrEvent));
    TRACE_ENTER_4BYTE(osapiHtonl(0));
    TRACE_ENTER_BYTE(L7_CNFGR_COMPONENT_ID);

    TRACE_END();

    if (start == L7_FALSE) 
    {
      currTraceCnfgrEvent = CNFGR_EVNT_FIRST;
    }
  }
}

/*********************************************************************
* @purpose  Trace a cnfgr event issued to a component
*
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    compId      {(input)}   Indicates the compId acting on the event
* @param    correlator  {(input)}   Indicates the correlator for the component
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrTraceEventPerComp(L7_CNFGR_RQST_t     event,
                            L7_BOOL             start,
                            L7_uchar8           compId,
                            L7_uint32           correlator
                           )
{
  if ((cnfgrTraceHandle != ~0) && 
      (cnfgrTraceMode == L7_TRUE) &&
      (cnfgrTraceMask & CNFGR_TRACE_EVENT_PER_COMPONENT)
     )
  {
    if (start == L7_TRUE) 
    {
      currTraceCnfgrReqEvent = event;
    }

    TRACE_BEGIN(cnfgrTraceHandle, L7_CNFGR_COMPONENT_ID);

    TRACE_ENTER_2BYTE((start)?osapiHtons(CNFGR_EVENT_PER_COMP_START):osapiHtons(CNFGR_EVENT_PER_COMP_FINI));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(currTraceCnfgrReqEvent));
    TRACE_ENTER_4BYTE(osapiHtonl(correlator));
    TRACE_ENTER_BYTE(compId);

    TRACE_END();

  }
}

void cnfgrTraceMaskSet(L7_uint32 mask)
{
  cnfgrTraceMask = mask;
}

/*********************************************************************
* @purpose  Enable tracing in the CNFGR component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrTraceModeApply(L7_BOOL enable)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (enable == L7_TRUE)
    rc = traceBlockStart(cnfgrTraceHandle);
  else
    rc = traceBlockStop(cnfgrTraceHandle);

  if (rc != L7_SUCCESS)
  {
    LOG_MSG("cnfgrTraceModeApply: Unable to apply CNFGR trace mode (%u), "
            "handle=0x%8.8x\n",enable , cnfgrTraceHandle);
  }
  else
  {
    cnfgrTraceMode = enable;
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
void cnfgrSysTraceModeApply(L7_uint32 mode)
{
  cnfgrTraceModeApply(mode);

  cnfgrTraceMaskSet(CNFGR_TRACE_EVENT);
  if (mode == SYS_TRACE_ALL_EVENTS)
  {
    cnfgrTraceMaskSet(CNFGR_TRACE_EVENT_PER_COMPONENT);
  }
}
/*********************************************************************
* @purpose  Format the output of a Cnfgr event 
*
* @param    traceId     @{(input)} Trace Id
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
L7_RC_t cnfgrTraceEventFormat(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  cnfgrTracePt_t *p = (cnfgrTracePt_t *)pDataStart;
  L7_char8        tmpStr[L7_COMPONENT_NAME_MAX_LEN];
  L7_uchar8      *str;
 

  switch (osapiNtohl(p->eventId))
  {
    case CNFGR_EVNT_I_START:
      osapiStrncpy((L7_char8 *)tmpStr, "P1", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_I_PHASE1_CMPLT:
      osapiStrncpy((L7_char8 *)tmpStr, "P2", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_I_PHASE2_CMPLT:
      osapiStrncpy((L7_char8 *)tmpStr, "WMU", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_I_MGMT_UNIT:
      osapiStrncpy((L7_char8 *)tmpStr, "P3", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_I_UNCONFIGURE_CMPLT:
      osapiStrncpy((L7_char8 *)tmpStr, "U_CMPLT", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_E_START:
      osapiStrncpy((L7_char8 *)tmpStr, "E", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_U_START:
      osapiStrncpy((L7_char8 *)tmpStr, "U_P1", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_U_PHASE1_CMPLT:
      osapiStrncpy((L7_char8 *)tmpStr, "U_P2", sizeof(tmpStr));
      break;
    case CNFGR_EVNT_FIRST:
      osapiStrncpy((L7_char8 *)tmpStr, "EVNT_FIRST", sizeof(tmpStr));
      break;
    default:
      osapiStrncpy((L7_char8 *)tmpStr, "UNKNOWN", sizeof(tmpStr));
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"Event=%-22s ", tmpStr);

  if (traceId == CNFGR_EVENT_START)
    str = CNFGR_EVENT_PROGRESS_START_STRING;
  else
    str = CNFGR_EVENT_PROGRESS_FINISHED_STRING;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_uchar8 *)"%-7s ", str
               );
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format the output of a Cnfgr event issued to a component
*
* @param    traceId     @{(input)} Trace Id
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
L7_RC_t cnfgrTraceEventCompFormat(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  cnfgrTracePt_t *p = (cnfgrTracePt_t *)pDataStart;
  L7_char8        tmpStr[L7_COMPONENT_NAME_MAX_LEN];
  L7_uchar8      *str;
 

  switch (osapiNtohl(p->eventId))
  {
    case L7_CNFGR_RQST_I_PHASE1_START:
      osapiStrncpy((L7_char8 *)tmpStr, "P1_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_I_PHASE2_START:
      osapiStrncpy((L7_char8 *)tmpStr, "P2_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_I_PHASE3_START:
      osapiStrncpy((L7_char8 *)tmpStr, "P3_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
      osapiStrncpy((L7_char8 *)tmpStr, "WMU", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_E_START:
      osapiStrncpy((L7_char8 *)tmpStr, "E_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_U_PHASE1_START:
      osapiStrncpy((L7_char8 *)tmpStr, "U_P1_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_U_PHASE2_START:
      osapiStrncpy((L7_char8 *)tmpStr, "U_P2_START", sizeof(tmpStr));
      break;
    case L7_CNFGR_RQST_FIRST:
      osapiStrncpy((L7_char8 *)tmpStr, "REQ_FINISH", sizeof(tmpStr));
      break;
    default:
      osapiStrncpy((L7_char8 *)tmpStr, "UNKNOWN", sizeof(tmpStr));
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_char8 *)"Event=%-22s ", tmpStr);

  if (traceId == CNFGR_EVENT_PER_COMP_START)
    str = CNFGR_EVENT_PROGRESS_START_STRING;
  else
    str = CNFGR_EVENT_PROGRESS_FINISHED_STRING;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (L7_uchar8 *)"%-7s ", str
               );
  if (traceId == CNFGR_EVENT_PER_COMP_START)
  {
    memset(tmpStr, 0, sizeof(tmpStr));
    cnfgrApiComponentNameGet(p->targetCompId, tmpStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (L7_char8 *)"compId=%-8d(%-20s) correlator = %-8d ", osapiHtonl(p->targetCompId), 
                  tmpStr, osapiHtonl(p->correlator)
                 );
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (L7_char8 *)"correlator = %-8d ", osapiHtonl(p->correlator)
                 );
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the Cnfgr component trace data
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
L7_RC_t cnfgrTraceShow(L7_uint32 count, L7_BOOL format)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax, traceId;

  traceData = traceBlockFind(cnfgrTraceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate Card Manager trace block for handle "
                  "0x%8.8x\n", cnfgrTraceHandle);
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
          if (((j % CNFGR_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

      } /* endif unformatted */

      else  /* manually format output */
      {
        cnfgrTracePtHdr_t       *pHdr;
        cnfgrTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8               *pIdStr;

        /* the first 8 bytes are the same for all trace entries */
        pHdr = (cnfgrTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case CNFGR_EVENT_START:
          case CNFGR_EVENT_FINI:
            pIdStr = (L7_uchar8 *)CNFGR_EVENT_STRING;
            fmtFunc = cnfgrTraceEventFormat;
            break;
          case CNFGR_EVENT_PER_COMP_START:
          case CNFGR_EVENT_PER_COMP_FINI:
            pIdStr = (L7_uchar8 *)CNFGR_EVENT_PER_COMP_STRING;
            fmtFunc = cnfgrTraceEventCompFormat;
            break;
          default:
            pIdStr = (L7_uchar8 *)"######";
            break;
        }

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"%-15s: ", pIdStr);
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
* @purpose  Cnfgr trace format function registered with sysTrace
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
void cnfgrSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart)
{
  cnfgrTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
  L7_uchar8               *pIdStr;

  /* translate trace code to a display string */
  switch (traceId)
  {
    case CNFGR_EVENT_START:
    case CNFGR_EVENT_FINI:
      pIdStr = (L7_uchar8 *)CNFGR_EVENT_STRING;
      fmtFunc = cnfgrTraceEventFormat;
      break;
    case CNFGR_EVENT_PER_COMP_START:
    case CNFGR_EVENT_PER_COMP_FINI:
      pIdStr = (L7_uchar8 *)CNFGR_EVENT_PER_COMP_STRING;
      fmtFunc = cnfgrTraceEventCompFormat;
      break;
    default:
      pIdStr = (L7_uchar8 *)"######";
      break;
  }


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-15s: ", pIdStr);
  /* show the entry timestamp */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"ts=%4.4d ",
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


void cnfgrTraceConfigPrint(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Tracing is %s\n",(cnfgrTraceMode)?"enabled":"disabled");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Trace mask is %s\n", cnfgrTraceMask);

}

void cnfgrTraceHelp(void)
{

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - print CNFGR tracing configuration\n","cnfgrTraceConfigPrint()" );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - 1 is enabled, 0 is disabled\n","cnfgrTraceModeApply(enable)");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Display CNFGR trace buffer\n","cnfgrTraceShow(numEntries,format)");

}

/*********************************************************************
* @purpose  Clean-up the resources required for profiling CNFGR
*
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileFini()
{
  if (cnfgrProfileEventInfo != L7_NULLPTR) 
  {
    osapiFree(L7_CNFGR_COMPONENT_ID, cnfgrProfileEventInfo);
    cnfgrProfileEventInfo = L7_NULLPTR;
  }

  if (cnfgrProfileEventCompInfo != L7_NULLPTR) 
  {
    osapiFree(L7_CNFGR_COMPONENT_ID, cnfgrProfileEventCompInfo);
    cnfgrProfileEventCompInfo = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  Allocate the resources required for profiling CNFGR
*
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileInit()
{
  static L7_BOOL cnfgrRegSysProfile = L7_FALSE;
  compProfileRegistrationInfo_t sysProfileRegInfo;

  if (cnfgrRegSysProfile == L7_FALSE) 
  {
    sysProfileRegInfo.enable = L7_TRUE;
    sysProfileRegInfo.compProfileModeSet = cnfgrProfileModeSet;
    sysProfileRegInfo.compProfileInfoShow = cnfgrProfileInfoShow;
    sysProfileRegisterComponent(L7_CNFGR_COMPONENT_ID, sysProfileRegInfo);
    cnfgrRegSysProfile = L7_TRUE;
  }

  cnfgrProfileFini();
  
  if (cnfgrProfileEnable == L7_TRUE) 
  {
    cnfgrProfileEventInfo = (cnfgrProfileEvent_t *) osapiMalloc(L7_CNFGR_COMPONENT_ID, sizeof(cnfgrProfileEvent_t));
    if (cnfgrProfileEventInfo == L7_NULLPTR) 
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }

    cnfgrProfileEventCompInfo = (cnfgrProfileCompInfo_t *) osapiMalloc(L7_CNFGR_COMPONENT_ID, (L7_LAST_COMPONENT_ID + 1) * sizeof(cnfgrProfileCompInfo_t));
    
    if (cnfgrProfileEventCompInfo == L7_NULLPTR) 
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }
  }
}

/*********************************************************************
* @purpose  Start profiling of a cnfgr event
*
* @param    event       {(input)}   The cnfgr event being issued
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileEventStart(CNFGR_CTLR_EVNT_t   event)
{
  if (cnfgrProfileEnable == L7_TRUE) 
  {
    currProfileCnfgrEvent = event;
    cnfgrProfileEventInfo->eventInfo[event].tempTime = osapiTimeMillisecondsGet(); 
  }
}

/*********************************************************************
* @purpose  Stop profiling of the current cnfgr event
*
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileEventStop(void)
{
  if (cnfgrProfileEnable == L7_TRUE) 
  {
    //cnfgrProfileEventInfo->eventInfo[currProfileCnfgrEvent].totalTime = osapiTimeMillisecondsGet() - cnfgrProfileEventInfo->eventInfo[currProfileCnfgrEvent].tempTime; 
    cnfgrProfileEventInfo->eventInfo[currProfileCnfgrEvent].totalTime = osapiTimeMillisecondsGetOffset(cnfgrProfileEventInfo->eventInfo[currProfileCnfgrEvent].tempTime);
    currProfileCnfgrEvent = CNFGR_EVNT_FIRST;
  }
}

/*********************************************************************
* @purpose  Start profiling of current cnfgr event on a per component basis
*
* @param    event            {(input)}   The event being traced
* @param    compId           {(input)}   The compId being traced
* @param    correlator       {(input)}   The correlator of the component being traced
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileEventCompStart(L7_CNFGR_RQST_t     event,
                                L7_uchar8           compId,
                                L7_uint32           correlator
                               )
{
  if (cnfgrProfileEnable == L7_TRUE) 
  {
    currProfileCnfgrReqEvent = event - L7_CNFGR_RQST_FIRST;
    cnfgrProfileEventCompInfo[compId].correlator = correlator;
    cnfgrProfileEventCompInfo[compId].eventInfo[currProfileCnfgrReqEvent].tempTime = osapiTimeMillisecondsGet();
  }
}

/*********************************************************************
* @purpose  Stop profiling of current cnfgr event on a per component basis
*
* @param    correlator       {(input)}   The correlator of the component being traced
*
* @returns  void
*
* @end
*********************************************************************/
void cnfgrProfileEventCompStop(L7_uint32 correlator)
{
  L7_uint32 i;

  if (cnfgrProfileEnable == L7_TRUE) 
  {
    for (i = L7_FIRST_COMPONENT_ID; i < L7_LAST_COMPONENT_ID; i++) 
    {
      if (cnfgrProfileEventCompInfo[i].correlator == correlator) 
      {
        //cnfgrProfileEventCompInfo[i].eventInfo[currProfileCnfgrReqEvent].totalTime = osapiTimeMillisecondsGet() - cnfgrProfileEventCompInfo[i].eventInfo[currProfileCnfgrReqEvent].tempTime;
        cnfgrProfileEventCompInfo[i].eventInfo[currProfileCnfgrReqEvent].totalTime = osapiTimeMillisecondsGetOffset(cnfgrProfileEventCompInfo[i].eventInfo[currProfileCnfgrReqEvent].tempTime);
        cnfgrProfileEventCompInfo[i].correlator = 0;
        break;
      }
    }
   
  }
}

void cnfgrProfileModeSet(L7_uint32 enable)
{
  cnfgrProfileEnable = enable;
  if (cnfgrProfileEnable == L7_TRUE) 
  {
    cnfgrProfileInit();
  }
  else
  {
    cnfgrProfileFini();
  }
}

void cnfgrProfileConfigShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Profiling is %s\n",(cnfgrProfileEnable)?"enabled":"disabled");
  
}

void cnfgrProfileInfoShow()
{
  L7_uint32 i;
  L7_char8  tmpStr[L7_COMPONENT_NAME_MAX_LEN];

  if (cnfgrProfileEnable) 
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\nTotal time spent in configurator states\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  " P1 (ms)       P2 (ms)     P3 (ms)    WMU (ms)       E (ms)       U_P1 (ms)    U_P2 (ms) \n"
                 );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "-------       -------      ------     -------       -------      --------     -------- \n");

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%-8d       %-8d    %-8d     %-8d     %-8d    %-8d     %-8d\n",
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_I_START].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_I_PHASE1_CMPLT].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_I_MGMT_UNIT].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_I_PHASE2_CMPLT].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_E_START].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_U_START].totalTime,
                  cnfgrProfileEventInfo->eventInfo[CNFGR_EVNT_U_PHASE1_CMPLT].totalTime
                 );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nTotal time spent in configurator states per component\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "       CompId                             P1 (ms)          P2 (ms)        P3 (ms)        WMU (ms)        E (ms)        U_P1 (ms)      U_P2 (ms) \n"
                 );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "-----------------------                  -------           ------         ------         -------        -------       --------       -------\n");
    for (i = L7_FIRST_COMPONENT_ID; i < L7_LAST_COMPONENT_ID; i++) 
    {
      memset(tmpStr, 0, sizeof(tmpStr));
      cnfgrApiComponentNameGet(i, tmpStr);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "%-2d-%-25s                   %-8d      %-8d      %-8d        %-8d        %-8d      %-8d        %-8d\n",
                    i, tmpStr,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_I_PHASE1_START - L7_CNFGR_RQST_FIRST].totalTime,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_I_PHASE2_START - L7_CNFGR_RQST_FIRST].totalTime,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_I_PHASE3_START - L7_CNFGR_RQST_FIRST].totalTime,

                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_I_WAIT_MGMT_UNIT - L7_CNFGR_RQST_FIRST].totalTime,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_E_START - L7_CNFGR_RQST_FIRST].totalTime,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_U_PHASE1_START - L7_CNFGR_RQST_FIRST].totalTime,
                    cnfgrProfileEventCompInfo[i].eventInfo[L7_CNFGR_RQST_U_PHASE2_START - L7_CNFGR_RQST_FIRST].totalTime
                   );
    }
  
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Cnfgr Profiling is not enabled.\n"
                 );
    
  }
   
}
void cnfgrProfileHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Enable cnfgr events profiling\n","cnfgrProfileModeSet(enable)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show cnfgr events profiling configuration\n","cnfgrProfileConfigShow()");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show profiling Information for cnfgr events\n","cnfgrProfileInfoShow()");
}
