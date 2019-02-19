/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    sys_trace.c
*
* @purpose     File contains defines needed for a trace utility implimentation
*
* @component 
*
* @comments 
*
* @create      7/5/2001
*
* @author      skalyanam
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "log.h"
#include "sysapi.h"
#include "osapi_support.h"

static traceControl_t traceControl;

static L7_uint32 sysTraceMode = SYS_TRACE_GLOBAL_EVENTS;
static compTraceRegistrationInfo_t *sysTraceMgr = L7_NULLPTR;

static L7_uint32 sysProfileMode = SYS_PROFILE_DISABLE;
static compProfileRegistrationInfo_t *sysProfileMgr = L7_NULLPTR;

/*********************************************************************
* @purpose  This function creates and initializes a trace block
*          
* @param 		numOfEntries  @b{(input)}   number of entries in the trace
* @param 		bytesPerEntry @b{(input)}   number of bytes per entry
* @param    *traceName    @b{(input)}   the name for this trace
* @param    traceId       @b{(output)}  the trace ID with which to uniquely identify this trace
*	
* @returns  L7_SUCCESS - on a successful creation of the trace
* @returns  L7_FAILURE - if any operation failed
*
* @notes    bytesPerEntry must be to the power of 2, minimum 8 bytes
*           the trace is initialized. if bytesPerEntry is not a power of 2
*           the closest larger power of two is substituted for bytesPerEntry
*           Header information is a part of the trace block, the head and tail
*           pointers are appropriately  assigned, to the begining and end of 
*           the actual trace data area.
*           Allow two bytes for traceId(to identify a component) and 
*           traceCode(to be used for internal modules of a components).
*           Calculate the bytes Per Entry to include these two bytes. 
*
* @end
*
*********************************************************************/
L7_RC_t traceBlockCreate(L7_uint32 numOfEntries, 
                         L7_uint32 bytesPerEntry, 
                         L7_uchar8 *traceName,
                         L7_uint32 *traceId)
{
  L7_uint32 totalBytes;
  traceData_t *traceData;
  L7_uint32 two = TRACE_BASE_POWER;
  L7_uint32 alignBytes = 0;
  L7_uint32 structureBytes;



  if (bytesPerEntry<TRACE_MIN_ENTRY_SIZE)
  {
    bytesPerEntry = TRACE_MIN_ENTRY_SIZE;
  }
  /*bytesperentry must be a power of 2*/
  while (two < bytesPerEntry)
  {
    two = two*TRACE_BASE_POWER;

  }
  bytesPerEntry = two;

  totalBytes = numOfEntries * bytesPerEntry;

  /*some additional bytes are added to make the header bytes a multiple of bytesPerEntry*/
  /*to ensure the use of malloc aligned and for completeness*/

  structureBytes = (L7_uint32)sizeof(traceData_t);
  alignBytes = structureBytes / bytesPerEntry;/*integer division*/
  alignBytes = (alignBytes + 1) * bytesPerEntry;
  alignBytes = alignBytes - structureBytes;

  /*malloc for the entire trace area header data structure and data area*/
  traceData = (traceData_t *)osapiMalloc(L7_SIM_COMPONENT_ID, (sizeof(traceData_t)+alignBytes+totalBytes));
  if (traceData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /*set all to zeros*/
  memset(traceData,0x00,totalBytes+sizeof(traceData_t)+alignBytes);
  /*initialize traceData_t fields*/
  traceData->bytesPerEntry = bytesPerEntry;
  traceData->numOfEntries = numOfEntries;
  traceData->head = (L7_uchar8 *)traceData + sizeof(traceData_t);
  traceData->tail = traceData->head + totalBytes;
  traceData->next = traceData->head;
  traceData->entryCount = 0;
  traceData->initialized = L7_TRUE;
  traceData->active = L7_FALSE;

  if (traceControl.traceCount == 0)
  { /*this is the first trace block that is getting added*/
    traceControl.traceFirst = traceData;
    traceControl.traceLast = traceData;
    traceData->nextTrace = L7_NULLPTR;
    traceData->prevTrace = L7_NULLPTR;
    traceControl.traceCount++;
    traceData->traceId = traceControl.id;
    traceControl.id++; /*always incremented to maintain a unique id*/
  }
  else
  {
    traceData->prevTrace = traceControl.traceLast;
    traceControl.traceLast->nextTrace = traceData;
    traceControl.traceLast = traceData;
    traceData->nextTrace = L7_NULLPTR;
    traceControl.traceCount++;
    traceData->traceId = traceControl.id;
    traceControl.id++;
  }
  *traceId = traceData->traceId;
  if (strlen((char *)traceName)>=16)
  {
    memcpy(traceData->traceName,traceName,15);
  }
  else
    strcpy((char *)traceData->traceName,(char *)traceName);

  /*create a semaphore here*/
  traceData->semaId = (void *)osapiSemaMCreate(OSAPI_SEM_Q_FIFO|OSAPI_SEM_DELETE_SAFE);
  if (traceData->semaId == L7_NULLPTR)
  {
    osapiFree(L7_SIM_COMPONENT_ID, (void*)traceData);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  locates a trace block in the list of trace blocks
*          
* @param 		traceId @b{(input)}   the trace ID associated with this trace
*	
* @returns  the traceData_t pointer of the found trace block
* @returns  L7_NULLPTR  if the trace was not found
*
* @notes none
*
* @end
*
*********************************************************************/
traceData_t *traceBlockFind(L7_uint32 traceId)
{
  traceData_t *traceData;

  traceData = traceControl.traceFirst;
  if (traceData == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  while (traceData->nextTrace != L7_NULLPTR)
  {
    if (traceData->traceId == traceId)
    {
      return traceData;
    }
    traceData = traceData->nextTrace;
  }
  if (traceData->traceId == traceId)
  {
    return traceData;
  }
  else
    return L7_NULLPTR;

}

/*********************************************************************
* @purpose  removes a trace block from the list and frees memory allocated for it
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns L7_SUCCESS if the trace was successfully removed
* @returns L7_FAILURE if the trace block was not found
*
* @notes    all memory associated with this trace block is freed on 
*           a successful operation
*
* @end
*
*********************************************************************/
L7_RC_t traceBlockDelete(L7_uint32 traceId)
{
  traceData_t *traceData;

  traceData = traceBlockFind(traceId);
  if (traceData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /*if found trace is the first trace*/
  if (traceData->prevTrace == L7_NULLPTR)
  {
    if (traceData->nextTrace == L7_NULLPTR)
    {
      /*first and only trace*/
      traceControl.traceFirst = L7_NULLPTR;
      traceControl.traceLast = L7_NULLPTR;
      traceControl.id = 0; /*set it back to zero only here no where else to maintain unique id*/
    }
    else
    {
      /*there is atleast one more trace*/
      traceData->nextTrace->prevTrace = L7_NULLPTR;
      traceControl.traceFirst =  traceData->nextTrace;
    }
  }
  else if (traceData->nextTrace == L7_NULLPTR)
  { /*found trace is the last trace*/
    traceData->prevTrace->nextTrace = L7_NULLPTR;
    traceControl.traceLast = traceData->prevTrace;

  }
  else
  {
    /*found trace is not first or last*/
    traceData->prevTrace->nextTrace = traceData->nextTrace;
    traceData->nextTrace->prevTrace = traceData->prevTrace;
  }
  if (osapiSemaDelete(traceData->semaId) != L7_SUCCESS)
  {
    LOG_MSG("traceBlockDelete(): Unable to delete trace block semaphore, "
            "id=0x%8.8x\n", (L7_uint32)traceData->semaId);
  }

  /*osapiFree(L7_SIM_COMPONENT_ID, (void *)traceData->head);*/
  osapiFree(L7_SIM_COMPONENT_ID, (void *)traceData);
  traceControl.traceCount--;

  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  activate the trace block
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns  L7_SUCCESS on a successful completion of the begin
* @returns  L7_FAILURE if the trace was not found
*
* @notes    puts a series of 0xB in the trace file for easy identification
*
* @end
*
*********************************************************************/
L7_RC_t traceBlockStart(L7_uint32 traceId)
{
  traceData_t *traceData;

  traceData = traceBlockFind(traceId);                                     
  if (traceData != L7_NULLPTR)
  {
    traceData->active = L7_TRUE;
    /*trace code id for start is 0x01*/
    TRACE_BEGIN(traceId,0x01);
    TRACE_ENTER_2BYTE(TRACE_START_MARKER);
    TRACE_ENTER_4BYTE(0x0);
    TRACE_END();
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  stop tracing into the trace 
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns  L7_SUCCESS if the trace was successfully stoped
* @returns  L7_FAILURE if the trace block ws not found
*
* @notes    puts a string of 0xF for easy identification 
*
* @end
*
*********************************************************************/
L7_RC_t traceBlockStop(L7_uint32 traceId)
{
  traceData_t *traceData;

  traceData = traceBlockFind(traceId);                                     
  if (traceData != L7_NULLPTR)
  {
    /*trace code id for stop is 0x02*/
    TRACE_BEGIN(traceId,0x02);
    TRACE_ENTER_2BYTE(TRACE_END_MARKER);
    TRACE_ENTER_4BYTE(0x0);
    TRACE_END();
    traceData->active = L7_FALSE;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}
/************************************************************************/
/*DEBUG ROUTINE USED VIA DEVSHELL*/
/***********************************************************************/
/*********************************************************************
* @purpose  stop tracing into the trace for all the traces 
*          
* @param 		none
*	
* @returns  none
*
* @notes    puts a string of 0xF for easy identification
*           to be used via devshell 
*
* @end
*
*********************************************************************/
void traceStopAll()
{
  traceData_t * traceData;

  traceData = traceControl.traceFirst;

  while (traceData != L7_NULLPTR)
  {
    (void)traceBlockStop(traceData->traceId);
    traceData = traceData->nextTrace;
  }
}
/*********************************************************************
* @purpose  activate the trace block for all the traces
*          
* @param 		none
*	
* @returns  none
*
* @notes    puts a series of 0xB in the trace file for easy identification
*           to be used via devshell
* @end
*
*********************************************************************/
void traceStartAll()
{
  traceData_t * traceData;

  traceData = traceControl.traceFirst;

  while (traceData != L7_NULLPTR)
  {
    (void)traceBlockStart(traceData->traceId);
    traceData = traceData->nextTrace;
  }
}
/*********************************************************************
* @purpose  dump on screen the trace
*          
* @param 		traceId - the trace id associated with this trace
*	
* @returns  none
*
* @notes    to be used via devshell
*           
* @end
*
*********************************************************************/
void traceDump(L7_uint32 traceId)
{
  traceData_t *traceData;
  L7_uchar8    *byteEntry;
  L7_uchar8    *start;
  L7_uint32  num;
  L7_uint32  i,j;
  traceData = traceBlockFind(traceId);
  if (traceData != L7_NULLPTR)
  {
    if (traceData->entryCount <= traceData->numOfEntries)
    {
      num = (L7_uint32)traceData->entryCount;
      start = traceData->head;
    }
    else
    {  /*trace dump will always begin from the oldest entry to the latest entry*/
      num = traceData->numOfEntries;
      start = traceData->next;
    }


    printf("\n");
    printf( "\t+--------------------------------------------------------+\n" );
    printf( "\t  Trace Name  :  %-16s\n",(char *)traceData->traceName);
    printf( "\t+--------------------------------------------------------+\n" );
    printf( "\t| Head        :  0x%8.8lx      Active      :  %-6u   |\n",
            (L7_ulong32)traceData->head, (L7_ushort16)traceData->active );
    printf( "\t| Tail        :  0x%8.8lx      Entry Size  :  %-6u   |\n",
            (L7_ulong32)traceData->tail, traceData->bytesPerEntry );
    printf( "\t| Next        :  0x%8.8lx      Buffer Size :  %-8lu |\n",
            (L7_ulong32)traceData->next, (L7_ulong32)(traceData->numOfEntries*traceData->bytesPerEntry) );
    printf( "\t| Entry Count :  %-10lu      Max Entries :  %-6u   |\n",
            (L7_ulong32)traceData->entryCount, traceData->numOfEntries );
    printf( "\t+--------------------------------------------------------+\n\n");
    byteEntry = start;
    for (i = 0 ; i<num ;i++)
    {
      for (j = 0 ;j<traceData->bytesPerEntry; j++)
      {
        printf("%02x ",*byteEntry);
        byteEntry++;
        if (byteEntry >= traceData->tail)
        {
          byteEntry = traceData->head;
        }
      }
      printf("\n");
    }

  }
  else
  {
    printf("Trace not found with traceId = %u\n", traceId);

  }
}



#ifdef UNIT_TESTING
void traceKickStart1()
{
  traceData_t *traceData;
  L7_uint32 traceId;

  (void)traceBlockCreate(100,16,(L7_uchar8 *)"thefirstone",&traceId);
  (void)traceBlockStart(traceId);
  traceData = traceBlockFind(traceId);

  TRACE_BEGIN(traceId,0xFF);
  TRACE_ENTER_2BYTE(0xCCCC);
  TRACE_ENTER_4BYTE(0xCCCCCCCC);
  TRACE_ENTER_BYTE(0xCC);
  TRACE_END();

  (void)traceBlockStop(traceId);

  (void)traceBlockDelete(traceId);

}

void traceKickStart2()
{

  traceData_t *traceData;
  L7_uint32 traceId;
  L7_uint32 traceId2;
  L7_uint32 traceId3;
  L7_uint32 dummy;
  (void)traceBlockCreate(100,16,(L7_uchar8 *)"1234567890123456",&traceId);
  (void)traceBlockCreate(200,32,(L7_uchar8 *)"largesttoseetruccation",&traceId2);
  (void)traceBlockCreate(1000,64,(L7_uchar8 *)"dot3adtrace",&traceId3);

  (void)traceBlockStart(traceId);
  (void)traceBlockStart(traceId2);
  (void)traceBlockStart(traceId3);


  traceData = traceBlockFind(traceId);
  if (traceData == L7_NULLPTR)
  {
    /*should not get here*/
    dummy++;
  }
  traceData = traceBlockFind(traceId2);
  if (traceData == L7_NULLPTR)
  {
    /*should not get here*/
    dummy++;
  }
  traceData = traceBlockFind(traceId3);
  if (traceData == L7_NULLPTR)
  {
    /*should not get here*/
    dummy++;
  }
  TRACE_BEGIN(traceId,0xFF);
  TRACE_ENTER_2BYTE(0xCCCC);
  TRACE_ENTER_4BYTE(0xCCCCCCCC);
  TRACE_ENTER_BYTE(0xCC);
  TRACE_END();

  TRACE_BEGIN(traceId2,0xFF);
  TRACE_ENTER_2BYTE(0xDDDD);
  TRACE_ENTER_4BYTE(0xDDDDDDDD);
  TRACE_ENTER_BYTE(0xDD);
  TRACE_END();

  TRACE_BEGIN(traceId3,0xFF);
  TRACE_ENTER_2BYTE(0x9999);
  TRACE_ENTER_4BYTE(0x8888888);
  TRACE_ENTER_BYTE(0x44);
  TRACE_END();

  (void)traceBlockStop(traceId);



  (void)traceBlockStop(traceId2);



  (void)traceBlockStop(traceId3);

  (void)traceBlockDelete(traceId3);
  (void)traceBlockDelete(traceId);
  (void)traceBlockDelete(traceId2);
}

void traceKickStart3()
{

  L7_uint32 traceId,i;

  (void)traceBlockCreate(25,8,(L7_uchar8 *)"trace1",&traceId);
  (void)traceBlockStart(traceId);
  for (i=1;i<24;i++)
  {
    TRACE_BEGIN(traceId,0x77);
    TRACE_ENTER_2BYTE((L7_ushort16)i);
    TRACE_ENTER_4BYTE(i);
    TRACE_END();
  }
  (void)traceBlockStop(traceId);

}

void traceKickStart4()
{

  L7_uint32 traceId,i;

  (void)traceBlockCreate(25,8,(L7_uchar8 *)"trace2",&traceId);
  (void)traceBlockStart(traceId);
  for (i=2;i<30;i++)
  {
    TRACE_BEGIN(traceId,0x77);
    TRACE_ENTER_2BYTE((L7_ushort16)i);
    TRACE_ENTER_4BYTE(i);
    TRACE_END();
  }
  (void)traceBlockStop(traceId);
}
#endif /* UNIT_TESTING */

/*********************************************************************
* @purpose  Initialize the system trace/profile manager
*
* @param    none
*
* @returns  none
*
* @notes    Any FASTPATH component can register it's trace/profile routines
*           with this utility. When sysTraceShow is invoked, the trace of 
*           all the registered components is shown sorted by timestamp.
*
*
* @end
*********************************************************************/
void sysTraceProfileMgrInit()
{
  sysTraceMgr = (compTraceRegistrationInfo_t *) osapiMalloc(L7_SIM_COMPONENT_ID, (L7_LAST_COMPONENT_ID + 1) * sizeof(compTraceRegistrationInfo_t));

  if (sysTraceMgr == L7_NULLPTR) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
    return;
  }

  sysProfileMgr = (compProfileRegistrationInfo_t *) osapiMalloc(L7_SIM_COMPONENT_ID, (L7_LAST_COMPONENT_ID + 1) * sizeof(compProfileRegistrationInfo_t));

  if (sysProfileMgr == L7_NULLPTR) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, " osapiMalloc failed at %s : %d\n", __FUNCTION__, __LINE__);
    return;
  }
}

/*********************************************************************
* @purpose Register the trace utility of a component with sysTrace
*
* @param    none
*
* @returns  none 
*
* @end
*********************************************************************/
void sysTraceRegisterComponent(L7_COMPONENT_IDS_t cid, compTraceRegistrationInfo_t regInfo)
{
  if (cid > L7_FIRST_COMPONENT_ID && cid < L7_LAST_COMPONENT_ID) 
  {
    if (sysTraceMgr != L7_NULLPTR) 
    {
      memcpy(&sysTraceMgr[cid], &regInfo, sizeof(compTraceRegistrationInfo_t));
    }
  }
}

/*********************************************************************
* @purpose Internal helper routine for sysTraceShow
*
* @param   compTraceInfo 
*
* @returns  Next cid whose trace has to be displayed
*
* @end
*********************************************************************/
L7_uint32 sysTraceFindNextCid(compTraceInfo_t *compTraceInfo)
{
  L7_uint32 i;
  L7_uint32 smallestTs = 0xffffffff, cid = L7_LAST_COMPONENT_ID;
  compTracePtHdr_t *pHdr;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++) 
  {
    if (sysTraceMgr[i].enable) 
    {
      if (compTraceInfo[i].valid == L7_TRUE) 
      {
        pHdr = (compTracePtHdr_t *)compTraceInfo[i].pEntry;
        if (pHdr != L7_NULLPTR) 
        {
          if (osapiNtohl(pHdr->timeStamp) < smallestTs) 
          {
            smallestTs = osapiNtohl(pHdr->timeStamp);
            cid = i;
          }
        }
      }
    }
  }
  
  return cid;
}

/*********************************************************************
* @purpose Show the system trace
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
void sysTraceShow()
{
  compTraceInfo_t        compTraceInfo[L7_LAST_COMPONENT_ID];
  L7_uint32              i, traceId, cid;
  traceData_t           *traceData;
  L7_uchar8             *pOffset;
  compTracePtHdr_t      *pHdr;

  memset(compTraceInfo, 0, sizeof(compTraceInfo_t));

  /* Find the trace block for each registered component 
  ** and determine the entry index, total number of entries etc.
  */  
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++) 
  {
    if (sysTraceMgr[i].enable) 
    {
      traceData = traceBlockFind(sysTraceMgr[i].compTraceHandle);
      if (traceData == L7_NULLPTR)
      {
        continue;
      }

      compTraceInfo[i].traceData = traceData;
      compTraceInfo[i].valid = L7_TRUE;
      compTraceInfo[i].numEnt = 0;

      if (traceData->entryCount <= traceData->numOfEntries)
      {
        compTraceInfo[i].pEntry = traceData->head;
        compTraceInfo[i].totalEnt = (L7_uint32)traceData->entryCount;
      }
      else
      {  /*trace dump will always begin from the oldest entry to the latest entry*/
        compTraceInfo[i].pEntry = traceData->next;
        compTraceInfo[i].totalEnt = traceData->numOfEntries;
      }
    }
  }
 
  while (1) 
  {
    /* Find the cid which has the smallest timestamp at the current entry index */
    cid = sysTraceFindNextCid(compTraceInfo);
    if (cid == L7_LAST_COMPONENT_ID) 
    {
      break;
    }

    pOffset = compTraceInfo[cid].pEntry;
    pHdr = (compTracePtHdr_t *)pOffset;
    pOffset += sizeof(*pHdr);

    /* pick up the trace point identifier from the entry contents */
    traceId = osapiNtohs(pHdr->traceId);

    
    if ((traceId != TRACE_START_MARKER) &&
        (traceId != TRACE_END_MARKER))
    {
      /* Display the entry of the cid with  the smallest timestamp at the current entry index */
      sysTraceMgr[cid].compTraceFmtFunc(traceId, osapiNtohl(pHdr->timeStamp), pOffset);
    }
     
    
    compTraceInfo[cid].pEntry += compTraceInfo[cid].traceData->bytesPerEntry;
    if (compTraceInfo[cid].pEntry >= compTraceInfo[cid].traceData->tail)
    {
      compTraceInfo[cid].pEntry = compTraceInfo[cid].traceData->head;
    }
    
    compTraceInfo[cid].numEnt++;
    if (compTraceInfo[cid].numEnt >= compTraceInfo[cid].totalEnt) 
    {
      compTraceInfo[cid].valid = L7_FALSE;
    }
    
  }

  return;
}

void sysTraceModeSet(L7_uint32 mode)
{
  L7_uint32 i;

  sysTraceMode = mode;
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++) 
  {
    if (sysTraceMgr[i].enable) 
    {
      sysTraceMgr[i].compTraceModeSet(mode);
    }
  }

}

void sysTraceConfigPrint()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "System tracing mode is %d\n",sysTraceMode);
}

void sysTraceHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-15s  - Set system trace mode \n", "sysTraceModeSet(mode)"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "      0 - disable system tracing \n"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "      1 - enable tracing of global events \n"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "      2 - enable tracing of all events \n"); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - print system tracing configuration\n","sysTraceConfigPrint()" );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Display System trace buffer\n","sysTraceShow");


}

/*********************************************************************
* @purpose Register the profile utility of a component with sysProfile
*
* @param    none
*
* @returns  none 
*
* @end
*********************************************************************/
void sysProfileRegisterComponent(L7_COMPONENT_IDS_t cid, compProfileRegistrationInfo_t regInfo)
{
  if (cid > L7_FIRST_COMPONENT_ID && cid < L7_LAST_COMPONENT_ID) 
  {
    if (sysProfileMgr != L7_NULLPTR) 
    {
      memcpy(&sysProfileMgr[cid], &regInfo, sizeof(compProfileRegistrationInfo_t));
    }
  }
}

void sysProfileModeSet(L7_uint32 mode)
{
  L7_uint32 i;

  sysProfileMode = mode;
  for (i = 0; i < L7_LAST_COMPONENT_ID; i++) 
  {
    if (sysProfileMgr[i].enable) 
    {
      sysProfileMgr[i].compProfileModeSet(mode);
    }
  }

}

void sysProfileShow()
{
  L7_uint32 i;

  if (sysProfileMode == SYS_PROFILE_DISABLE) 
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "System profiling is disabled\n");
    
    return;
  }

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++) 
  {
    if (sysProfileMgr[i].enable) 
    {
      sysProfileMgr[i].compProfileInfoShow();
    }
  }

}

void sysProfileConfigPrint()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "System profiling mode is %d\n",sysProfileMode);
}

void sysProfileHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-15s  - Set system profile mode \n", "sysProfileModeSet(mode)"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "       0 - disable system profiling \n"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "       1 - enable profiling of global events \n"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "       2 - enable detailed profiling \n"); 

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - print system profiling configuration\n","sysProfileConfigPrint()" );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-30s - Display System profiling info\n","sysProfileShow");


}
