/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_trace.c
*
* @purpose    DiffServ Component trace
*
* @component  DiffServ
*
* @comments   none
*
* @create     10/25/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "l7_diffserv_include.h"
#include "osapi_support.h"


/*********************************************************************
* @purpose  Apply the DiffServ trace mode configuration setting
*
* @param    mode        @b{(input)} L7_DISABLE, L7_ENABLE, L7_TRACE_MODE_ENHANCED
*
* @returns  void
*
* @notes    Do not let a trace mode change failure impact DiffServ
*           operation.  Hence, any error is simply logged.
*
* @end
*********************************************************************/
void dsTraceModeApply(L7_uint32 mode)
{
  L7_RC_t       rc;

  if (mode >= L7_ENABLE)
    rc = traceBlockStart(pDiffServInfo_g->diffServTraceHandle);
  else
    rc = traceBlockStop(pDiffServInfo_g->diffServTraceHandle);

  if (rc != L7_SUCCESS)
  {
    LOG_MSG("dsTraceModeApply: Unable to apply DiffServ trace mode (%u), "
            "handle=0x%8.8x\n", mode, pDiffServInfo_g->diffServTraceHandle);
  }
}

/*********************************************************************
* @purpose  Trace point for MIB row create and delete
*
* @param    traceId       @{(input)} Trace point identifier
* @param    subId         @{(input)} Subcomponent identifier
* @param    tableId       @{(input)} MIB table identifier (per subcomponent)
* @param    index1        @{(input)} First table index
* @param    index2        @{(input)} Second table index
* @param    index3        @{(input)} Third table index
* @param    rowPtr        @{(input)} Table row pointer
* @param    rc            @{(input)} Function exit return code
* @param    activateFlag  @{(input)} Row activation flag
*
* @returns  void
*
* @notes    This function used for both entry and exit of row create/delete.
*
* @end
*********************************************************************/
void dsTracePtRowMgmt(L7_ushort16 traceId,
                      L7_uchar8   subId,
                      L7_uchar8   tableId,
                      L7_ushort16 index1,
                      L7_ushort16 index2,
                      L7_ushort16 index3,
                      L7_ulong32  rowPtr,
                      L7_uchar8   rc,
                      L7_uchar8   activateFlag)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_BYTE(subId);
    TRACE_ENTER_BYTE(tableId);
    TRACE_ENTER_2BYTE(osapiHtons(index1));
    TRACE_ENTER_2BYTE(osapiHtons(index2));
    TRACE_ENTER_2BYTE(osapiHtons(index3));
    TRACE_ENTER_4BYTE(osapiHtonl(rowPtr));
    TRACE_ENTER_BYTE(rc);
    TRACE_ENTER_BYTE(activateFlag);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for MIB object set
*
* @param    traceId       @{(input)} Trace point identifier
* @param    subId         @{(input)} Subcomponent identifier
* @param    tableId       @{(input)} MIB table identifier (per subcomponent)
* @param    index1        @{(input)} First table index
* @param    index2        @{(input)} Second table index
* @param    index3        @{(input)} Third table index
* @param    rowPtr        @{(input)} Table row pointer
* @param    rc            @{(input)} Function exit return code
* @param    oid           @{(input)} Object identifier (OID)
* @param    valLen        @{(input)} Object value length
* @param    pVal          @{(input)} Pointer to object value (up to 8 bytes)
*
* @returns  void
*
* @notes    This function is also used for the object setTest failure case.
*
* @end
*********************************************************************/
void dsTracePtObjectSet(L7_ushort16 traceId,
                        L7_uchar8   subId,
                        L7_uchar8   tableId,
                        L7_ushort16 index1,
                        L7_ushort16 index2,
                        L7_ushort16 index3,
                        L7_ulong32  rowPtr,
                        L7_uchar8   rc,
                        L7_uchar8   oid,
                        L7_uchar8   valLen,
                        L7_uchar8   *pVal)
{
  L7_uint32     i;

  /* adjust value length if greater than maximum allowed for tracing */
  if (valLen > DS_TRACE_OBJ_VAL_LEN_MAX)
    valLen = DS_TRACE_OBJ_VAL_LEN_MAX;
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_BYTE(subId);
    TRACE_ENTER_BYTE(tableId);
    TRACE_ENTER_2BYTE(osapiHtons(index1));
    TRACE_ENTER_2BYTE(osapiHtons(index2));
    TRACE_ENTER_2BYTE(osapiHtons(index3));
    TRACE_ENTER_4BYTE(osapiHtonl(rowPtr));
    TRACE_ENTER_BYTE(rc);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_BYTE(oid);
    TRACE_ENTER_BYTE(valLen);
    for (i = 0; i < DS_TRACE_OBJ_VAL_LEN_MAX; i++)
    {
      TRACE_ENTER_BYTE(*(pVal+i));
    }
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for policy evaluate
*
* @param    traceId       @{(input)} Trace point identifier
* @param    policyIndex   @{(input)} Policy index
* @param    intIfNum      @{(input)} Internal interface number
* @param    ifDirection   @{(input)} Interface direction
* @param    linkState     @{(input)} Link state
* @param    policyIsReady @{(input)} Policy is ready flag
* @param    servRowStatus @{(input)} Service table row status
* @param    adminMode     @{(input)} DiffServ global admin mode
* @param    reissueTLV    @{(input)} Reissue TLV flag
* @param    rc            @{(input)} Function exit return code
* @param    needBuildUp   @{(input)} Need build-up flag
* @param    needTearDown  @{(input)} Need tear-down flag
*
* @returns  void
*
* @notes    This function used for both per-instance and at exit of policy 
*           evaluation.
*
* @end
*********************************************************************/
void dsTracePtPolEval(L7_ushort16 traceId,
                      L7_ushort16 policyIndex,
                      L7_ushort16 intIfNum,
                      L7_uchar8   ifDirection,
                      L7_uchar8   linkState,
                      L7_uchar8   policyIsReady,
                      L7_uchar8   servRowStatus,
                      L7_uchar8   adminMode,
                      L7_uchar8   reissueTlv,
                      L7_uchar8   rc,
                      L7_uchar8   needBuildUp,
                      L7_uchar8   needTearDown)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_2BYTE(osapiHtons(policyIndex));
    TRACE_ENTER_2BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_2BYTE(osapiHtons(intIfNum));
    TRACE_ENTER_BYTE(ifDirection);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_BYTE(linkState);
    TRACE_ENTER_BYTE(policyIsReady);
    TRACE_ENTER_BYTE(servRowStatus);
    TRACE_ENTER_BYTE(adminMode);
    TRACE_ENTER_BYTE(reissueTlv);
    TRACE_ENTER_BYTE(rc);
    TRACE_ENTER_BYTE(needBuildUp);
    TRACE_ENTER_BYTE(needTearDown);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for policy build-up/tear-down
*
* @param    traceId       @{(input)} Trace point identifier
* @param    policyIndex   @{(input)} Policy index
* @param    intIfNum      @{(input)} Internal interface number
* @param    ifDirection   @{(input)} Interface direction
*
* @returns  void
*
* @notes    This function used for entry and exit of policy build-up and
*           tear-down.
*
* @end
*********************************************************************/
void dsTracePtPolUpDn(L7_ushort16 traceId,
                      L7_ushort16 policyIndex,
                      L7_ushort16 intIfNum,
                      L7_uchar8   ifDirection)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_2BYTE(osapiHtons(policyIndex));
    TRACE_ENTER_2BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_2BYTE(osapiHtons(intIfNum));
    TRACE_ENTER_BYTE(ifDirection);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for policy TLV add/delete
*
* @param    traceId         @{(input)} Trace point identifier
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
* @param    intIfNum        @{(input)} Internal interface number
* @param    ifDirection     @{(input)} Interface direction
* @param    instKey         @{(input)} Instance key
* @param    rc              @{(input)} Function exit return code
* @param    combined        @{(input)} "Combined" TLV flag
*
* @returns  void
*
* @notes    This function used for both entry and exit of policy TLV 
*           add/delete.
*
* @end
*********************************************************************/
void dsTracePtPolTlvAddDel(L7_ushort16 traceId,
                           L7_ushort16 policyIndex,
                           L7_ushort16 policyInstIndex,
                           L7_ushort16 intIfNum,
                           L7_uchar8   ifDirection,
                           L7_ulong32  instKey,
                           L7_uchar8   rc,
                           L7_uchar8   combined)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_2BYTE(osapiHtons(policyIndex));
    TRACE_ENTER_2BYTE(osapiHtons(policyInstIndex));
    TRACE_ENTER_2BYTE(osapiHtons(intIfNum));
    TRACE_ENTER_BYTE(ifDirection);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_4BYTE(osapiHtonl(instKey));
    TRACE_ENTER_BYTE(rc);
    TRACE_ENTER_BYTE(combined);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for policy TLV DTL add/delete
*
* @param    traceId         @{(input)} Trace point identifier
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
* @param    intIfNum        @{(input)} Internal interface number
* @param    ifDirection     @{(input)} Interface direction
* @param    tlvHandle       @{(input)} TLV block handle 
* @param    rc              @{(input)} Function exit return code
* @param    tlvType         @{(input)} TLV type identifier
* @param    tlvLen          @{(input)} TLV total length
* @param    instKey         @{(input)} Instance key
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dsTracePtPolTlvDtl(L7_ushort16 traceId,
                        L7_ushort16 policyIndex,
                        L7_ushort16 policyInstIndex,
                        L7_ushort16 intIfNum,
                        L7_uchar8   ifDirection,
                        L7_ulong32  tlvHandle,
                        L7_uchar8   rc,
                        L7_ushort16 tlvType,
                        L7_ushort16 tlvLen,  
                        L7_ulong32  instKey)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_2BYTE(osapiHtons(policyIndex));
    TRACE_ENTER_2BYTE(osapiHtons(policyInstIndex));
    TRACE_ENTER_2BYTE(osapiHtons(intIfNum));
    TRACE_ENTER_BYTE(ifDirection);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_4BYTE(osapiHtonl(tlvHandle));
    TRACE_ENTER_BYTE(rc);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_2BYTE(osapiHtons(tlvType));
    TRACE_ENTER_2BYTE(osapiHtons(tlvLen));
    TRACE_ENTER_2BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_4BYTE(osapiHtonl(instKey));
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Trace point for generic events
*
* @param    traceId       @{(input)} Trace point identifier
* @param    tableId       @{(input)} MIB table identifier
* @param    event         @{(input)} Event code (specific per traceId)
* @param    adminMode     @{(input)} DiffServ administrative mode
* @param    rowPtr        @{(input)} Table row pointer
* @param    intIfNum      @{(input)} Internal interface number
*
* @returns  void
*
* @notes    This function for multiple, unrelated event indications.
*           Not all fields are meaningful for each event, so any
*           interpretation of the trace contents must be based on traceId.
*
* @end
*********************************************************************/
void dsTracePtGenEvent(L7_ushort16 traceId,
                       L7_uchar8   tableId,
                       L7_uchar8   event, 
                       L7_uchar8   adminMode,
                       L7_ulong32  rowPtr,
                       L7_ulong32  intIfNum)
{
  
  if (DS_TRACE_IS_ENABLED() == L7_TRUE)
  {
    TRACE_BEGIN(DS_TRACE_HANDLE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_BYTE(tableId);
    TRACE_ENTER_BYTE(event);
    TRACE_ENTER_BYTE(adminMode);
    TRACE_ENTER_BYTE(DS_TRACE_FIELD_UNUSED);
    TRACE_ENTER_4BYTE(osapiHtonl(rowPtr));
    TRACE_ENTER_4BYTE(osapiHtonl(intIfNum));
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Convert object value data format for trace point
*
* @param    pValue      @{(input)}  Pointer to object value
* @param    valLen      @{(input)}  Length of object value content
* @param    pOutputBuf  @{(output)} Pointer to output buffer
*
* @returns  void
*
* @notes    All trace point value fields are defined in network byte order.
*
* @end
*********************************************************************/
void dsTraceObjectValFormat(void * pValue, size_t valLen, L7_uchar8 *pOutputBuf)
{
  L7_ulong32    val32;
  L7_ushort16   val16;

  /* clear out entire buffer (at least the max amount used by DiffServ trace) */
  memset((char *)pOutputBuf, 0, DS_TRACE_OBJ_VAL_LEN_MAX);

  /* the trace data is always stored in network byte order, so make any
   * necessary conversions here
   */
  if (valLen == sizeof(val32))
  {
    val32 = osapiHtonl(*(L7_ulong32 *)pValue);
    memcpy((char *)pOutputBuf, (char *)&val32, sizeof(val32));
  }

  else if (valLen == sizeof(val16))
  {
    val16 = osapiHtons(*(L7_ushort16 *)pValue);
    memcpy((char *)pOutputBuf, (char *)&val16, sizeof(val16));
  }

  else  /* assume value is defined in terms of bytes */
  {
    /* respect limit of trace point value length definition */
    if (valLen > DS_TRACE_OBJ_VAL_LEN_MAX)
      valLen = DS_TRACE_OBJ_VAL_LEN_MAX;

    memcpy((char *)pOutputBuf, (char *)pValue, valLen);
  }
}

/*********************************************************************
* @purpose  Display the DiffServ component trace data
*
* @param    format      @{(input)} Format trace output (L7_TRUE, L7_FALSE)
* @param    count       @{(input)} Number of entries to display
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A count value of 0 is used to display all entries.
*
* @end
*********************************************************************/
L7_RC_t dsTraceShow(L7_BOOL format, L7_uint32 count)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(DS_TRACE_HANDLE);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate DiffServ trace block for handle "
                  "0x%8.8x\n", DS_TRACE_HANDLE);
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
          if (((j % DS_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- "); 
          }
        } /* endfor j */

      } /* endif unformatted */

      else  /* manually format output */
      {
        dsTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8             *pIdStr;
        dsTracePtHdr_t        *pHdr;

        /* the first 8 bytes are the same for all DiffServ trace entries */
        pHdr = (dsTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
        case DS_TRACE_ID_RSVD_START:
          pIdStr = (L7_uchar8 *)"TrcStart";
          break;
        case DS_TRACE_ID_RSVD_STOP:
          pIdStr = (L7_uchar8 *)"TrcStop";
          break;
        case DS_TRACE_ID_ROW_CREATE_ENTER:
          pIdStr = (L7_uchar8 *)"Create >";
          fmtFunc = dsTraceFmtRowMgmt;
          break;
        case DS_TRACE_ID_ROW_CREATE_EXIT:
          pIdStr = (L7_uchar8 *)"Create <";
          fmtFunc = dsTraceFmtRowMgmt;
          break;
        case DS_TRACE_ID_ROW_DELETE_ENTER:
          pIdStr = (L7_uchar8 *)"Delete >";
          fmtFunc = dsTraceFmtRowMgmt;
          break;
        case DS_TRACE_ID_ROW_DELETE_EXIT:
          pIdStr = (L7_uchar8 *)"Delete <";
          fmtFunc = dsTraceFmtRowMgmt;
          break;
        case DS_TRACE_ID_OBJECT_SETTEST_FAIL:
          pIdStr = (L7_uchar8 *)"SetTestErr";
          fmtFunc = dsTraceFmtObjectSet;
          break;
        case DS_TRACE_ID_OBJECT_SET:
          pIdStr = (L7_uchar8 *)"ObjSet";
          fmtFunc = dsTraceFmtObjectSet;
          break;
        case DS_TRACE_ID_ADMIN_MODE_CHNG:
          pIdStr = (L7_uchar8 *)"AdminChng";
          fmtFunc = dsTraceFmtGenEvent;
          break;
        case DS_TRACE_ID_INTF_EVENT:
          pIdStr = (L7_uchar8 *)"IntfEvent";
          fmtFunc = dsTraceFmtGenEvent;
          break;
        case DS_TRACE_ID_EVAL_ALL:
          pIdStr = (L7_uchar8 *)"EvalAll";
          fmtFunc = dsTraceFmtGenEvent;
          break;
        case DS_TRACE_ID_ROW_EVENT:
          pIdStr = (L7_uchar8 *)"RowEvent";
          fmtFunc = dsTraceFmtGenEvent;
          break;
        case DS_TRACE_ID_POL_EVAL_INST:
          pIdStr = (L7_uchar8 *)"EvalInst";
          fmtFunc = dsTraceFmtPolEval;
          break;
        case DS_TRACE_ID_POL_EVAL_EXIT:
          pIdStr = (L7_uchar8 *)"Eval <";
          fmtFunc = dsTraceFmtPolEval;
          break;
        case DS_TRACE_ID_POL_BUILD_UP:
          pIdStr = (L7_uchar8 *)"PolBldUp";
          fmtFunc = dsTraceFmtPolUpDn;
          break;
        case DS_TRACE_ID_POL_TEAR_DOWN:
          pIdStr = (L7_uchar8 *)"PolTearDn";
          fmtFunc = dsTraceFmtPolUpDn;
          break;
        case DS_TRACE_ID_POL_TLV_ADD_ENTER:
          pIdStr = (L7_uchar8 *)"PolAdd >";
          fmtFunc = dsTraceFmtPolTlvAddDel;
          break;
        case DS_TRACE_ID_POL_TLV_ADD_SEP_EXIT:
          pIdStr = (L7_uchar8 *)"PolSep <";
          fmtFunc = dsTraceFmtPolTlvAddDel;
          break;
        case DS_TRACE_ID_POL_TLV_ADD_COMBO_EXIT:
          pIdStr = (L7_uchar8 *)"PolCombo <";
          fmtFunc = dsTraceFmtPolTlvAddDel;
          break;
        case DS_TRACE_ID_POL_TLV_DEL_ENTER:
          pIdStr = (L7_uchar8 *)"PolDel >";
          fmtFunc = dsTraceFmtPolTlvAddDel;
          break;
        case DS_TRACE_ID_POL_TLV_DEL_EXIT:
          pIdStr = (L7_uchar8 *)"PolDel <";
          fmtFunc = dsTraceFmtPolTlvAddDel;
          break;
        case DS_TRACE_ID_POL_TLV_ADD_DTL:
          pIdStr = (L7_uchar8 *)"PolAddDtl";
          fmtFunc = dsTraceFmtPolTlvDtl;
          break;
        case DS_TRACE_ID_POL_TLV_DEL_DTL:
          pIdStr = (L7_uchar8 *)"PolDelDtl";
          fmtFunc = dsTraceFmtPolTlvDtl;
          break;
        default:
          pIdStr = (L7_uchar8 *)"??????";
          break;
        }
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"%-12s: ", pIdStr);

        /* show the entry timestamp */
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"ts=0x%8.8x ", 
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


/* format string arrays */
static char *dsTraceStrSubId[] = 
{ 
  "----", "Prvt", "Std", "Dstl" 
};

static char *dsTraceStrTableIdPrvt[] = 
{ 
  "----", "Class", "ClsRule", "Policy", "PolInst", "PolAttr", 
  "PerfIn", "PerfOut", "Service" 
};

static char *dsTraceStrTableIdStd[] = 
{ 
  "----", "DataPath", "Clfr", "ClfrElem", "MFClfr", "AuxMFC", "Meter", "TBParm",
  "ColorAwr", "Action", "CosMark", "Cos2Mark", "DscpMark", "PrecMark", "CountAct", 
  "AssignQ", "Redrct", "Mirror", "AlgDrop" 
};

static char *dsTraceStrOidClass[] =
{
  "-----", "Name", "Type", "L3Proto", "AclType", "AclNum", "ST", "RS"
};

static char *dsTraceStrOidClassRule[] =
{
  "-----", "Entry", "Cos", "Cos2", "DipA", "DipM", "Dip6A", "Dip6Plen",  
  "DportS", "DportE", "DmacA", "DmacM", "EtKey", "EtVal", "FlowLbl",
  "Dscp", "Prec", "TosByt", "TosMsk", "ProtNum", "RefIdx", "SipA", "SipM", "Sip6A", "Sip6Plen", 
  "SportS", "SportE", "SmacA", "SmacM", "VidS", "VidE", "Vid2S", "Vid2E", "Excl", "ST", "RS"
};

static char *dsTraceStrOidPolicy[] =
{
  "-----", "Name", "Type", "ST", "RS"
};

static char *dsTraceStrOidPolicyInst[] =
{
  "-----", "ClasIdx", "ST", "RS"
};

static char *dsTraceStrOidPolicyAttr[] =
{
  "-----", 
  "Entry", 
  "AsgnQ"
  "BwCR", "BwCU", 
  "DrpVal",
  "ExpCR", "ExpCU", "ExpCB", 
  "MkCos", "MkCos2", "MkDscp", "MkPrec",
  "SmpCR", "SmpCB", 
  "SmpCAct", "SmpCVal", "SmpNAct", "SmpNVal", 
  "SmpCCMd", "SmpCCVl",
  "SngCR", "SngCB", "SngEB", 
  "SngCAct", "SngCVal", "SngEAct", "SngEVal", "SngNAct", "SngNVal", 
  "SngCCMd", "SngCCVl", "SngCEMd", "SngCEVl",
  "TwoCR", "TwoCB", "TwoPR", "TwoPB", 
  "TwoCAct", "TwoCVal", "TwoEAct", "TwoEVal", "TwoNAct", "TwoNVal",
  "TwoCCMd", "TwoCCVl", "TwoCEMd", "TwoCEVl",
  "RndMin", "RndMax", "RndPrb", "RndSam", "RndExp", 
  "RdrIntf",
  "ShpAvCR", "ShpPkCR", "ShpPkPR", 
  "ST", "RS"
};

static char *dsTraceStrOidService[] =
{
  "-----", "PolIdx", "ST", "RS"
};

static char *dsTraceStrIfDirection[] =
{
  "---", "In", "Out"
};

static char *dsTraceStrRowStatus[] =
{
  "--", "A", "NS", "NR", "CG", "CW", "D"
};

static char *dsTraceStrMode[] =
{
  "Disable", "Enable"
};

/* private MIB tableId-to-format array mapping */
static char **dsTraceMapOidPrvt[] =
{
  L7_NULLPTR,                                   /* invalid table id */
  dsTraceStrOidClass,
  dsTraceStrOidClassRule,
  dsTraceStrOidPolicy,
  dsTraceStrOidPolicyInst,
  dsTraceStrOidPolicyAttr,
  L7_NULLPTR,                                   /* policy perf in */
  L7_NULLPTR,                                   /* policy perf out */
  dsTraceStrOidService
};

#define DS_TRACE_STR_INTF_EVENT_GET(_ev, _pstr) \
  (_pstr) = ((_ev) == L7_ACTIVE) ? "Act" : \
            (((_ev) == L7_INACTIVE) ? "Nact" : \
            ((((_ev) == L7_DISABLE) ? "Disb" : "????")))

#define DS_TRACE_STR_ROW_EVENT_GET(_ev, _pstr) \
  (_pstr) = ((_ev) == L7_ACTIVE) ? "Act" : \
            (((_ev) == L7_INACTIVE) ? "Nact" : \
            ((((_ev) == L7_DISABLE) ? "Disb" : "????")))


/*********************************************************************
* @purpose  Format common MIB table information
*
* @param    pDataStart  @{(input)} Start of MIB table trace info
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dsTraceFmtMibTable(void *pDataStart)
{
  dsTracePtMibTable_t *p = (dsTracePtMibTable_t *)pDataStart;
  char                *pStr;

  /* subcomponent id */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-4s ", dsTraceStrSubId[p->subId]);

  /* table id (interpret relative to subId) */
  if (p->subId == DS_TRACE_SUBID_PRVTMIB)
    pStr = dsTraceStrTableIdPrvt[p->tableId];
  else if (p->subId == DS_TRACE_SUBID_STDMIB)
    pStr = dsTraceStrTableIdStd[p->tableId];
  else
    pStr = dsTraceStrTableIdStd[0];             /* unknown table id */

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-8s ", pStr);

  /* indexes */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"(%u,%u,%u) ", 
                osapiNtohs(p->index1), 
                osapiNtohs(p->index2), 
                osapiNtohs(p->index3));

  /* row ptr */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"pRow=0x%8.8lx ", osapiNtohl(p->rowPtr));
}

/*********************************************************************
* @purpose  Format a Row Mgmt type trace point for trace show
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
L7_RC_t dsTraceFmtRowMgmt(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtRowMgmt_t  *p = (dsTracePtRowMgmt_t *)pDataStart;

  /* use subfunction to display MIB table info */
  dsTraceFmtMibTable(&p->mibTable);

  /* rc */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rc=%1u ", p->rc);

  /* row activation flag */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"act=%1s ", (char *)((p->act == L7_TRUE) ? "Y" : "N"));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format an Object Set type trace point for trace show
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
L7_RC_t dsTraceFmtObjectSet(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtObjectSet_t  *p = (dsTracePtObjectSet_t *)pDataStart;
  L7_uint32             i, imax;
  char                  **pOidTable;

  /* use subfunction to display MIB table info */
  dsTraceFmtMibTable(&p->mibTable);

  /* rc */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rc=%1u ", p->rc);

  /* object id (oid) */
  if (p->mibTable.subId == DS_TRACE_SUBID_PRVTMIB)
  {
    pOidTable = dsTraceMapOidPrvt[p->mibTable.tableId];
    if (pOidTable != L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    (char *)"oid=%-8s ", pOidTable[p->oid]);
    }
  }

  /* object value */
  imax = (L7_uint32)p->valLen;                  /* object value length */
  if (imax > 0)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, (char *)"val=0x");
    for (i = 0; i < imax-1; i++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    (char *)"%2.2x-", p->value[i]);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"%2.2x ", p->value[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format common Policy Evaluation indexes, interface and direction   
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
L7_RC_t dsTraceFmtPolIndex(void *pDataStart)
{
  dsTracePtPolIndex_t   *p = (dsTracePtPolIndex_t *)pDataStart;

  /* indexes, interface and direction */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"pol=(%u,%u) intf=%.2u(%-3s) ", 
                osapiNtohs(p->policyIndex), 
                osapiNtohs(p->policyInstIndex), 
                osapiNtohs(p->intIfNum), 
                dsTraceStrIfDirection[p->ifDirection]);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format a Policy Evaluation type trace point for trace show
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
L7_RC_t dsTraceFmtPolEval(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtPolEval_t    *p = (dsTracePtPolEval_t *)pDataStart;

  /* use subfunction to display policy index info */
  if (dsTraceFmtPolIndex(&p->index) != L7_SUCCESS)
    return L7_FAILURE;

  /* link state */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"link=%1s ",
                (p->linkState == L7_ACTIVE) ? "A" : "I");

  /* policyIsReady flag */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"prdy=%1u ", p->policyIsReady);

  /* service row status */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rstat=%2s ", dsTraceStrRowStatus[p->servRowStatus]);

  /* admin mode */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"mode=%1u ", p->adminMode);

  /* reissueTlv flag */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"reiss=%1u ", p->reissueTlv);

  /* rc */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rc=%1u ", p->rc);

  /* need build-up, need tear-down flags */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"nbu=%1u ntd=%1u ", p->needBuildUp, p->needTearDown);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format a Policy Build-up/Tear-down type trace point for trace show
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
L7_RC_t dsTraceFmtPolUpDn(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtPolUpDn_t    *p = (dsTracePtPolUpDn_t *)pDataStart;

  /* use subfunction to display policy index info */
  if (dsTraceFmtPolIndex(&p->index) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format a Policy TLV Add/Del type trace point for trace show
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
L7_RC_t dsTraceFmtPolTlvAddDel(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtPolTlvAddDel_t *p = (dsTracePtPolTlvAddDel_t *)pDataStart;

  /* use subfunction to display policy index info */
  if (dsTraceFmtPolIndex(&p->index) != L7_SUCCESS)
    return L7_FAILURE;

  /* instance key */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"key=%lu ", osapiNtohl(p->instKey));

  /* rc */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rc=%1u ", p->rc);

  /* "combined" flag */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"combo=%1u ", p->combined);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format a Policy Add/Del type trace point for trace show
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
L7_RC_t dsTraceFmtPolTlvDtl(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtPolTlvDtl_t  *p = (dsTracePtPolTlvDtl_t *)pDataStart;

  /* use subfunction to display policy index info */
  if (dsTraceFmtPolIndex(&p->index) != L7_SUCCESS)
    return L7_FAILURE;

  /* TLV handle */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"hndl=0x%8.8lx ", osapiNtohl(p->tlvHandle));

  /* rc */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"rc=%1u ", p->rc);

  /* TLV id and length */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"tlvType=0x%4.4x tlvLen=0x%4.4x ", 
                osapiNtohs(p->tlvType), osapiNtohs(p->tlvLen));

  /* instance key */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"key=%lu ", osapiNtohl(p->instKey));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Format a Generic Event type trace point for trace show
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
* @notes    Trace point contents are interpreted per the traceId.
*
* @end
*********************************************************************/
L7_RC_t dsTraceFmtGenEvent(L7_ushort16 traceId, void *pDataStart)
{
  dsTracePtGenEvent_t *p = (dsTracePtGenEvent_t *)pDataStart;
  char                *pStr;

  switch (traceId)
  {

  case DS_TRACE_ID_ADMIN_MODE_CHNG:
  case DS_TRACE_ID_EVAL_ALL:

    /* admin mode */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"admin=%-3s ", dsTraceStrMode[p->adminMode]);
    break;


  case DS_TRACE_ID_INTF_EVENT:

    /* interface event */
    DS_TRACE_STR_INTF_EVENT_GET(p->event, pStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"event=%-4s ", pStr);

    /* admin mode */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"admin=%-3s ", dsTraceStrMode[p->adminMode]);

    /* intIfNum */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"intf=%.2lu ", osapiNtohl(p->intIfNum));
    break;


  case DS_TRACE_ID_ROW_EVENT:

    /* table id */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"Prvt %-8s ", dsTraceStrTableIdPrvt[p->tableId]);

    /* row ptr */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"pRow=0x%8.8lx ", osapiNtohl(p->rowPtr));

    /* row event */
    DS_TRACE_STR_ROW_EVENT_GET(p->event, pStr);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"event=%-4s ", pStr);

    /* admin mode */
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  (char *)"admin=%-3s ", dsTraceStrMode[p->adminMode]);

    break;


  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  return L7_SUCCESS;
}

