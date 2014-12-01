/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_trace.c
*
* @purpose    Handle tracing for NIM
*
* @component  NIM
*
* @comments   none
*
* @create     03/25/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "trace_api.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "osapi_support.h"
#include "nim_trace.h"
#include "nim_util.h"
#include "nimapi.h"
#include "cnfgr_sid.h"
#include "comm_mask.h"

static L7_uint32 nimTraceHandle = ~0;
static L7_BOOL   nimTraceMode = L7_FALSE;
static L7_INTF_MASK_t nimTraceIntfMask;
static L7_uint32 nimTraceEventsCtr = 0;
static L7_uint32 nimTraceMask = 0;

/*
** Global variables for used for profiling interface events
*/

static L7_BOOL                   nimProfileEnable = L7_FALSE;
static L7_uint32                 nimProfileMask = NIM_PROFILE_PORT_EVENT_PER_COMP;

/*
** Reports total time spent per port event in the system
*/
static nimProfileEvent_t         *nimProfileEventInfo = L7_NULLPTR;

/*
** Reports total time spent per port event per interface in the system
*/
static nimProfileEventIntfInfo_t *nimProfileIntfEventInfo = L7_NULLPTR;

/*
** Reports total time spent per port event per component in the system
*/
static nimProfileEvent_t         *nimProfileCompEventInfo = L7_NULLPTR;

/*
** Reports total time spent per port event per interface per component in the system
*/
static nimProfileEventIntfInfo_t *nimProfileCompIntfEventInfo = L7_NULLPTR;

/* local prototypes */
void nimTraceMaskSet(L7_uint32 mask);
void nimTraceModeApply(L7_uint32 enable);
void nimTraceIntIfNumSet(L7_uint32 intIfNum);
void nimTraceIntIfNumClear(void);
L7_char8 *nimDebugCompStringGet(L7_COMPONENT_IDS_t cid);
void nimSysTraceModeApply(L7_uint32 enable);
void  nimSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart);
void nimSysProfileModeSet(L7_uint32 mode);
void nimSysProfileInfoShow();

/*********************************************************************
* @purpose  Initialize the trace utility for NIM
*
* @param    num_entries   {(input)}  max entries in the buffer
* @param    width         {(input)}  width in bytes of each entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
void nimTraceInit(L7_uint32 num_entries, L7_uint32 width)

{
  /* make sure sizes make sense */
  if (width < NIM_TRACE_ENTRY_SIZE_MAX) width = NIM_TRACE_ENTRY_SIZE_MAX;
  if (num_entries == 0) num_entries = NIM_TRACE_ENTRY_MAX;

  /* if allocated, clean it up */
  if (nimTraceHandle != ~0) (void)nimTraceFini();

  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(num_entries, width,
                       (L7_uchar8 *)NIM_TRACE_REGISTRATION_NAME,
                       &nimTraceHandle) != L7_SUCCESS)
  {
    NIM_LOG_MSG("NIM: unable to create the trace buffer\n");
    nimTraceHandle = ~0;
  }
  else
  {
    compTraceRegistrationInfo_t sysTraceRegInfo;

    nimTraceMaskSet( NIM_TRACE_EVENT_ERROR );

    /* always start tracing during init */
    nimTraceModeApply(L7_TRUE);
    nimTraceIntIfNumClear();
    nimTraceEventsCtr = 0;


    sysTraceRegInfo.compTraceHandle = nimTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = nimSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = nimSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_TRUE;
    sysTraceRegisterComponent(L7_NIM_COMPONENT_ID, sysTraceRegInfo);

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
void nimTraceFini(void)
{
  if (nimTraceHandle != ~0)
  {
    compTraceRegistrationInfo_t sysTraceRegInfo;

    sysTraceRegInfo.compTraceHandle = nimTraceHandle;
    sysTraceRegInfo.compTraceFmtFunc = nimSysTraceFmtFunc;
    sysTraceRegInfo.compTraceModeSet = nimSysTraceModeApply;
    sysTraceRegInfo.enable         = L7_FALSE;
    sysTraceRegisterComponent(L7_NIM_COMPONENT_ID, sysTraceRegInfo);

    /* stop tracing at the mask level */
    nimTraceMaskSet(0);

    /* disable the trace util */
    nimTraceModeApply(L7_FALSE);

    /* delete the buffer */
    if (traceBlockDelete(nimTraceHandle) != L7_SUCCESS)
    {
      NIM_LOG_MSG("NIM: unable to delete the trace buffer\n");
    }
    else
    {
      nimTraceHandle = ~0;
    }

  }
  return ;
}

/*********************************************************************
* @purpose  Trace a port event
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    start       {(input)}   L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void nimTracePortEvent(L7_COMPONENT_IDS_t component,
                       L7_PORT_EVENTS_t   event,
                       L7_uint32          intIfNum,
                       L7_BOOL            start,
                       NIM_HANDLE_t       handle)
{
  L7_uint32 info;
  L7_int32 taskId;

  /* increments the counter for another event */
  if (start)
    nimTraceEventsCtr++;

  if ((nimTraceHandle != ~0) &&
      (nimTraceMode == L7_TRUE) &&
      (nimTraceMask & NIM_TRACE_PORT_EVENTS) &&
      (start || (nimTraceMask & NIM_TRACE_FINI)))
  {
    L7_BOOL specificInterface = L7_FALSE;
    L7_INTF_NONZEROMASK(nimTraceIntfMask, specificInterface);
    if (!specificInterface || L7_INTF_ISMASKBITSET(nimTraceIntfMask, intIfNum))
    {
      taskId = osapiTaskIdSelf();

      info = ((component & 0xff) << 24) | ((event & 0xff) << 16) | (intIfNum & 0xffff);

      TRACE_BEGIN(nimTraceHandle, L7_NIM_COMPONENT_ID);
      TRACE_ENTER_2BYTE((start)?osapiHtons(NIM_PORT_EVENTS_START):osapiHtons(NIM_PORT_EVENTS_FINI));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)taskId));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)info));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)handle));


      TRACE_END();
    }
  }
}

/*********************************************************************
* @purpose  Trace a port event on a per component basis
*
* @param    component   {(input)} The component being traced
* @param    event       {(input)} The L7 event being issued
* @param    intIfNum    {(input)} The internal interface for the event
* @param    start       {(input)}  L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void nimTracePortEventComp(L7_COMPONENT_IDS_t component,
                           L7_PORT_EVENTS_t   event,
                           L7_uint32          intIfNum,
                           L7_BOOL            start,
                           NIM_HANDLE_t       handle)
{
  L7_uint32 info;
  L7_int32 taskId;

  /* increments the counter for another event */
  if (start)
    nimTraceEventsCtr++;

  if ((nimTraceHandle != ~0) &&
      (nimTraceMode == L7_TRUE) &&
      (nimTraceMask & NIM_TRACE_PORT_EVENT_PER_COMPONENT) &&
      (start || (nimTraceMask & NIM_TRACE_FINI)))
  {
    L7_BOOL specificInterface = L7_FALSE;
    L7_INTF_NONZEROMASK(nimTraceIntfMask, specificInterface);
    if (!specificInterface || L7_INTF_ISMASKBITSET(nimTraceIntfMask, intIfNum))
    {
      taskId = osapiTaskIdSelf();

      info = ((component & 0xff) << 24) | ((event & 0xff) << 16) | (intIfNum & 0xffff);

      TRACE_BEGIN(nimTraceHandle, L7_NIM_COMPONENT_ID);
      TRACE_ENTER_2BYTE((start)?osapiHtons(NIM_PORT_EVENTS_PER_COMP_START):osapiHtons(NIM_PORT_EVENTS_PER_COMP_FINI));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)taskId));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)info));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)handle));


      TRACE_END();
    }
  }
}

/*********************************************************************
* @purpose  Format the output for a port event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t nimTracePortEventFormat(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 info;
  L7_uchar8 *str;
  L7_uchar8 compName[L7_COMPONENT_NAME_MAX_LEN];

  memset(compName, 0, sizeof(compName));
  osapiStrncpy(compName, "NA", sizeof(compName));

  /* 6 byte key, 4 byte unit */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Task %x, ",osapiNtohl(*((L7_uint32*)&pDataStart[0])));

  info = osapiNtohl(*((L7_uint32*)&pDataStart[4]));

  if ((rc = cnfgrApiComponentNameGet((L7_COMPONENT_IDS_t)((info >> 24) & 0xff), compName)) != L7_SUCCESS)
  {
    osapiStrncpySafe(compName, "Unknown", 8);
  }


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s, ", compName);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s, ",nimGetIntfEvent((L7_PORT_EVENTS_t)((info >> 16) & 0xff)));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "If %d, ",(L7_uint32)(info & 0xffff));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Id %d, ",osapiNtohl(*((L7_uint32*)&pDataStart[8])));

  if ((traceId == NIM_PORT_EVENTS_START) ||
      (traceId == NIM_PORT_EVENTS_PER_COMP_START))
    str = NIM_EVENT_PROGRESS_START_STRING;
  else
    str = NIM_EVENT_PROGRESS_FINISHED_STRING;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s ",str);

  return rc;
}

/*********************************************************************
* @purpose  Trace a event error
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    nimErr      {(input)}   an identifier for the type of error
*
* @returns  void
*
* @end
*********************************************************************/
void nimTraceEventError(L7_COMPONENT_IDS_t component,
                        L7_PORT_EVENTS_t   event,
                        L7_uint32          intIfNum,
                        nimTraceErr_e      nimErr
                       )
{
  L7_uint32 info = 0;

  if ((nimTraceHandle != ~0) &&
      (nimTraceMode == L7_TRUE) &&
      (nimTraceMask & NIM_TRACE_EVENT_ERROR))
  {
    L7_BOOL specificInterface = L7_FALSE;
    L7_INTF_NONZEROMASK(nimTraceIntfMask, specificInterface);
    if (!specificInterface || L7_INTF_ISMASKBITSET(nimTraceIntfMask, intIfNum))
    {
      info = ((component & 0xff) << 24) | ((event & 0xff) << 16) | (intIfNum & 0xffff);

      TRACE_BEGIN(nimTraceHandle, L7_NIM_COMPONENT_ID);
      TRACE_ENTER_2BYTE(osapiHtons(NIM_ERR));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
      TRACE_ENTER_4BYTE(osapiHtonl(nimErr));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)info));

      TRACE_END();
    }
  }
}

/*********************************************************************
* @purpose  Format the output for a port event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t nimTraceEventErrorFormat(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 info;
  nimTraceErr_e nimErr;

  nimErr = (nimTraceErr_e)osapiNtohl(*((L7_uint32*)&pDataStart[0]));

  switch (nimErr)
  {
    case NIM_ERR_EVENT_INTF_DNE:
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Event on non-existent intf, ");
      break;
    case NIM_ERR_EVENT_OUT_OF_ORDER:
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Event out of order, ");
      break;
    case NIM_ERR_EVENT_WRONG_PHASE:
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Event in wrong phase, ");
      break;
    case NIM_ERR_EVENT_INVALID_USP:
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Event with invalid usp, ");
      break;
    default:
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "@@@@@@, ");
      break;
  }

  info = osapiNtohl(*((L7_uint32*)&pDataStart[4]));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Comp %d, ",(L7_uint32)((info >> 24) & 0xff));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Event %d, ",(L7_uint32)((info >> 16) & 0xff));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Intf %d, ",(L7_uint32)(info & 0xffff));

  return rc;
}

/*********************************************************************
* @purpose  Display the trace log for NIM
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    unformat  {(input)} L7_TRUE if the output is to be unformatted.
*
* @returns  void
*
* @end
*********************************************************************/
void nimTraceShow(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  if (nimTraceHandle == ~0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate NIM trace block for handle "
                  "0x%8.8x\n", nimTraceHandle);
    return;
  }

  traceData = traceBlockFind(nimTraceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate NIM trace block for handle "
                  "0x%8.8x\n", nimTraceHandle);
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
          if (((j % NIM_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

      } /* endif unformatted */
      else  /* manually format output */
      {
        nimTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8             *pIdStr;
        nimTracePtHdr_t        *pHdr;
        L7_BOOL                 printRecord = L7_TRUE;

        /* the first 8 bytes are the same for all Unitmgr trace entries */
        pHdr = (nimTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case NIM_PORT_EVENTS_START:
          case NIM_PORT_EVENTS_FINI:
            pIdStr = (L7_uchar8 *)NIM_PORT_EVENTS_STRING;
            fmtFunc = nimTracePortEventFormat;
            break;
          case NIM_PORT_EVENTS_PER_COMP_START:
          case NIM_PORT_EVENTS_PER_COMP_FINI:
            pIdStr = (L7_uchar8 *)NIM_PORT_EVENT_PER_COMP_STRING;
            fmtFunc = nimTracePortEventFormat;
            break;
          case NIM_ERR:
            pIdStr  = (L7_uchar8 *)NIM_ERR_STRING;
            fmtFunc = nimTraceEventErrorFormat;
            break;
          default:
            pIdStr = (L7_uchar8 *)"@@@@@@";
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

  return;
}

/*********************************************************************
* @purpose  NIM trace format function registered with sysTrace
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
void nimSysTraceFmtFunc(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart)
{
  nimTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
  L7_uchar8             *pIdStr;

  /* translate trace code to a display string */
  switch (traceId)
  {
    case NIM_PORT_EVENTS_START:
    case NIM_PORT_EVENTS_FINI:
      pIdStr = (L7_uchar8 *)NIM_PORT_EVENTS_STRING;
      fmtFunc = nimTracePortEventFormat;
      break;
    case NIM_PORT_EVENTS_PER_COMP_START:
    case NIM_PORT_EVENTS_PER_COMP_FINI:
      pIdStr = (L7_uchar8 *)NIM_PORT_EVENT_PER_COMP_STRING;
      fmtFunc = nimTracePortEventFormat;
      break;
    case NIM_ERR:
      pIdStr  = (L7_uchar8 *)NIM_ERR_STRING;
      fmtFunc = nimTraceEventErrorFormat;
      break;
    default:
      pIdStr = (L7_uchar8 *)"@@@@@@";
      break;
  }


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-15s: ", pIdStr);

  /* show the entry timestamp */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"ts=%8.8u ",
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

void nimTraceMaskSet(L7_uint32 mask)
{
  nimTraceMask = mask;
}

/*********************************************************************
* @purpose  Enable tracing in the NIM component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
void nimTraceModeApply(L7_BOOL enable)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (enable == L7_TRUE)
    rc = traceBlockStart(nimTraceHandle);
  else
    rc = traceBlockStop(nimTraceHandle);

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "nimTraceModeApply: Unable to apply NIM trace mode (%u), "
            "handle=0x%8.8x\n",enable , nimTraceHandle);
  }
  else
  {
    nimTraceMode = enable;
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
void nimSysTraceModeApply(L7_uint32 mode)
{
  nimTraceModeApply(mode);

  nimTraceMaskSet(NIM_TRACE_EVENT_ERROR);
  if (mode == SYS_TRACE_ALL_EVENTS)
  {
    nimTraceMaskSet(NIM_TRACE_PORT_EVENTS | NIM_TRACE_PORT_EVENT_PER_COMPONENT | NIM_TRACE_EVENT_ERROR);
  }
}

void nimTraceIntIfNumSet(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(nimTraceIntfMask, intIfNum);
}

void nimTraceIntIfNumClear(void)
{
  memset(&nimTraceIntfMask, 0, sizeof(nimTraceIntfMask));
}

void nimTraceStatsPrint(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Tracing is %s\n",(nimTraceMode)?"enabled":"disabled");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","Trace Mask",nimTraceMask);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = %d\n","Number of events", nimTraceEventsCtr);
}

void nimTraceHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - sets what gets traced\n","nimTraceMaskSet(mask)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_TRACE_PORT_EVENTS    ",NIM_TRACE_PORT_EVENTS);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_TRACE_PORT_EVENTS_PER_COMPONENT    ",NIM_TRACE_PORT_EVENT_PER_COMPONENT);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_TRACE_EVENT_ERROR    ",NIM_TRACE_EVENT_ERROR);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_TRACE_FINI    ",NIM_TRACE_FINI);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - print stats from NIM\n","nimTraceStatsPrint()" );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - 1 is enabled, 0 is disabled\n","nimTraceModeApply(enable)");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Trace a specific interface\n","nimTraceIntIfNumSet()");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Trace all interfaces\n","nimTraceIntIfNumClear()");

  nimTraceStatsPrint();
}


L7_char8 *nimDebugCompStringGet(L7_COMPONENT_IDS_t cid)
{
  L7_char8 *component_name = L7_NULLPTR;
  component_name = cnfgrSidComponentNameGet(cid);
  if (component_name == L7_NULLPTR)
  {
      return("NA");
  }
  return component_name;
}

/*********************************************************************
* @purpose  Clean-up the resources required for profiling NIM
*
*
* @returns void
*
* @end
*********************************************************************/
void nimProfileFini(void)
{
  if (nimProfileEventInfo != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimProfileEventInfo);
    nimProfileEventInfo = L7_NULLPTR;

  }
  if (nimProfileIntfEventInfo != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimProfileIntfEventInfo);
    nimProfileIntfEventInfo = L7_NULLPTR;
  }
  if (nimProfileCompEventInfo != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimProfileCompEventInfo);
    nimProfileCompEventInfo = L7_NULLPTR;
  }
  if (nimProfileCompIntfEventInfo != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimProfileCompIntfEventInfo);
    nimProfileCompIntfEventInfo = L7_NULLPTR;
  }
  return;
}

/*********************************************************************
* @purpose  Allocate the resources required for profiling NIM
*
*
* @returns  void
*
* @end
*********************************************************************/
void nimProfileInit(void)
{
  static L7_BOOL nimRegSysProfile = L7_FALSE;
  compProfileRegistrationInfo_t sysProfileRegInfo;

  if (nimRegSysProfile == L7_FALSE)
  {
    sysProfileRegInfo.enable = L7_TRUE;
    sysProfileRegInfo.compProfileModeSet = nimSysProfileModeSet;
    sysProfileRegInfo.compProfileInfoShow = nimSysProfileInfoShow;
    sysProfileRegisterComponent(L7_NIM_COMPONENT_ID, sysProfileRegInfo);
    nimRegSysProfile = L7_TRUE;
  }

  nimProfileFini();
  if (nimProfileEnable == L7_TRUE)
  {
    nimProfileEventInfo = (nimProfileEvent_t *) osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimProfileEvent_t));
    if (nimProfileEventInfo == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }

    nimProfileIntfEventInfo = (nimProfileEventIntfInfo_t *) osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimProfileEventIntfInfo_t));
    if (nimProfileIntfEventInfo == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }

    nimProfileCompEventInfo = (nimProfileEvent_t *) osapiMalloc(L7_NIM_COMPONENT_ID, (L7_LAST_COMPONENT_ID)* sizeof(nimProfileEvent_t));
    if (nimProfileCompEventInfo == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
      return;
    }

    if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP_INTF)
    {
      nimProfileCompIntfEventInfo = (nimProfileEventIntfInfo_t *) osapiMalloc(L7_NIM_COMPONENT_ID, (L7_LAST_COMPONENT_ID) * sizeof(nimProfileEventIntfInfo_t));
      if (nimProfileCompIntfEventInfo == L7_NULLPTR)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
        return;
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Profile a port event
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    start       {(input)}   L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void nimProfilePortEvent(L7_COMPONENT_IDS_t component,
                         L7_PORT_EVENTS_t   event,
                         L7_uint32          intIfNum,
                         L7_BOOL            start
                        )
{
  if (nimProfileEnable == L7_TRUE)
  {
    if (start == L7_TRUE)
    {
      nimProfileEventInfo->eventInfo[event].tempTime = osapiTimeMillisecondsGet();
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_INTF)
      {
        nimProfileIntfEventInfo->intfInfo[intIfNum].eventInfo[event].tempTime = osapiTimeMillisecondsGet();
      }
    }
    else
    {
      //nimProfileEventInfo->eventInfo[event].totalTime += osapiTimeMillisecondsGet() - nimProfileEventInfo->eventInfo[event].tempTime;
      nimProfileEventInfo->eventInfo[event].totalTime += osapiTimeMillisecondsGetOffset(nimProfileEventInfo->eventInfo[event].tempTime);
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_INTF)
      {
        //nimProfileIntfEventInfo->intfInfo[intIfNum].eventInfo[event].totalTime = osapiTimeMillisecondsGet() - nimProfileIntfEventInfo->intfInfo[intIfNum].eventInfo[event].tempTime;
        nimProfileIntfEventInfo->intfInfo[intIfNum].eventInfo[event].totalTime = osapiTimeMillisecondsGetOffset(nimProfileIntfEventInfo->intfInfo[intIfNum].eventInfo[event].tempTime);
      }
    }
  }
}

/*********************************************************************
* @purpose  Profile a port event on a per component basis
*
* @param    component   {(input)}   The component being traced
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    start       {(input)}   L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void nimProfilePortEventComp(L7_COMPONENT_IDS_t component,
                             L7_PORT_EVENTS_t   event,
                             L7_uint32          intIfNum,
                             L7_BOOL            start
                            )
{
  if (nimProfileEnable == L7_TRUE)
  {
    if (start == L7_TRUE)
    {
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP)
      {
        nimProfileCompEventInfo[component].eventInfo[event].tempTime = osapiTimeMillisecondsGet();
      }
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP_INTF)
      {
        nimProfileCompIntfEventInfo[component].intfInfo[intIfNum].eventInfo[event].tempTime = osapiTimeMillisecondsGet();
      }
    }
    else
    {
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP)
      {
        //nimProfileCompEventInfo[component].eventInfo[event].totalTime += osapiTimeMillisecondsGet() - nimProfileCompEventInfo[component].eventInfo[event].tempTime;
        nimProfileCompEventInfo[component].eventInfo[event].totalTime += osapiTimeMillisecondsGetOffset(nimProfileCompEventInfo[component].eventInfo[event].tempTime);
      }
      if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP_INTF)
      {
        //nimProfileCompIntfEventInfo[component].intfInfo[intIfNum].eventInfo[event].totalTime = osapiTimeMillisecondsGet() - nimProfileCompIntfEventInfo[component].intfInfo[intIfNum].eventInfo[event].tempTime;
        nimProfileCompIntfEventInfo[component].intfInfo[intIfNum].eventInfo[event].totalTime = osapiTimeMillisecondsGetOffset(nimProfileCompIntfEventInfo[component].intfInfo[intIfNum].eventInfo[event].tempTime);
      }
    }
  }
}

void nimProfileModeSet(L7_BOOL enable)
{
  nimProfileEnable = enable;
  if (nimProfileEnable == L7_TRUE)
  {
    nimProfileInit();
  }
  else
  {
    nimProfileFini();
  }
}

void nimProfileMaskSet(L7_uint32 mask)
{
  nimProfileMask = mask;
}

void nimProfileConfigShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Profiling is %s\n",(nimProfileEnable)?"enabled":"disabled");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","Profile Mask",nimProfileMask);
}

void nimProfileInfoShowEventDetail(L7_uint32 event, L7_BOOL compDetail, L7_BOOL intfDetail)
{
  L7_uint32 i;
  L7_RC_t   rc;
  nimUSP_t  nimUsp;
  L7_BOOL   noCompPrint = L7_TRUE, noIntfPrint = L7_TRUE;

  if (compDetail == L7_TRUE)
  {

    if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_COMP)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "\nProcessing time for event %d(%s) per component:\n", event,
                    nimGetIntfEvent(event)
                   );
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Component                        Processing time(msec)  \n"
                   );
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "-------------                    ---------------------  \n");
      for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
      {
        if (nimProfileCompEventInfo[i].eventInfo[event].totalTime)
        {
          L7_uchar8 compName[L7_COMPONENT_NAME_MAX_LEN];

          noCompPrint = L7_FALSE;
          memset(compName, 0, sizeof(compName));
          osapiStrncpy(compName, "NA", sizeof(compName));
          if ((rc = cnfgrApiComponentNameGet(i, compName)) != L7_SUCCESS)
          {
            osapiStrncpySafe(compName, "Unknown", 8);
          }

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%d-%-20s             %-8u \n",
                        i,
                        compName,
                        osapiHtonl(nimProfileCompEventInfo[i].eventInfo[event].totalTime));
        }
      }

      if (noCompPrint == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "*** All components took 0 ms for this event ***\n"
                     );
      }
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Profiling of port events per component is not enabled.\n"
                   );
    }
  }

  if (intfDetail == L7_TRUE)
  {
    if (nimProfileMask & NIM_PROFILE_PORT_EVENT_PER_INTF)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "\nProcessing time for event %d(%s) per Interface:\n", event,
                    nimGetIntfEvent(event)
                   );
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "USP              Processing time(msec)  \n"
                   );
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "-------          --------------------   \n"
                   );
      for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
      {
        rc = nimGetUnitSlotPort(i, &nimUsp);
        if (rc != L7_SUCCESS)
        {
          continue;
        }

        if (nimProfileIntfEventInfo->intfInfo[i].eventInfo[event].totalTime)
        {
          noIntfPrint = L7_FALSE;
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%d/%d/%d          %-8u \n",
                        nimUsp.unit, nimUsp.slot, nimUsp.port,
                        osapiHtonl(nimProfileIntfEventInfo->intfInfo[i].eventInfo[event].totalTime));
        }
      }

      if (noIntfPrint == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "*** All interfaces took 0 ms for this event ***\n"
                     );
      }
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Profiling of port events per interface is not enabled.\n"
                   );
    }
  }

  return;
}

void nimProfileInfoShowEvent(L7_uint32 event, L7_BOOL compDetail, L7_BOOL intfDetail)
{
  L7_uint32 i;

  if (nimProfileEnable == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Nim profiling is disabled\n");
    return;
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "\n  Event                       Processing time(msec)      \n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "--------------                  ---------------------      \n");
  if (event != 0)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "%2d-%-20s             %8u \n",
                  event,
                  nimGetIntfEvent(event),
                  osapiHtonl(nimProfileEventInfo->eventInfo[event].totalTime)
                 );
    nimProfileInfoShowEventDetail(event, compDetail, intfDetail);
  }
  else
  {
    for (i = 0; i < L7_LAST_PORT_EVENT; i++)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%2d-%-20s             %8u \n",
                    i,
                    nimGetIntfEvent(i),
                    osapiHtonl(nimProfileEventInfo->eventInfo[i].totalTime)
                   );

    }
    for (i = 0; i < L7_LAST_PORT_EVENT; i++)
    {
      nimProfileInfoShowEventDetail(i, compDetail, intfDetail);
    }
  }

  return;
}

void nimProfileHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Enable port events profiling\n","nimProfileModeSet(enable)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Configure port events profile mask\n","nimProfileMaskSet(mask)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_PROFILE_PORT_EVENT_PER_INTF    ",NIM_PROFILE_PORT_EVENT_PER_INTF);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_PROFILE_PORT_EVENT_PER_COMP    ",NIM_PROFILE_PORT_EVENT_PER_COMP);
#if 0
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s = 0x%0.8x\n","NIM_PROFILE_PORT_EVENT_PER_COMP_INTF    ",NIM_PROFILE_PORT_EVENT_PER_COMP_INTF);
#endif
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show port events profiling configuration\n","nimProfileConfigShow()");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Show profiling Information for a particular port event\n","nimProfileInfoShowEvent(event,compDetail,intfDetail)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                " - If event is 0 then information for all the events is displayed\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                " - If compDetail is 1 then processing time of the event per component is also displayed\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                " - If intfDetail is 1 then processing time of the event per interface is also displayed\n");

  return;
}

void nimSysProfileModeSet(L7_uint32 mode)
{
  nimProfileModeSet(mode);
  nimProfileMaskSet(NIM_PROFILE_PORT_EVENT_PER_COMP);
  if (mode == SYS_PROFILE_ALL_EVENTS)
  {
    nimProfileMaskSet(NIM_PROFILE_PORT_EVENT_PER_COMP | NIM_PROFILE_PORT_EVENT_PER_INTF);
  }
}

void nimSysProfileInfoShow()
{
  nimProfileInfoShowEvent(0,1,1);
}
