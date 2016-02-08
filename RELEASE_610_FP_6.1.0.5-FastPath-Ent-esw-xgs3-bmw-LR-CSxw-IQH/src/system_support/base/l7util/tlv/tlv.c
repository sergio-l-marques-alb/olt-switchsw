/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   tlv.c
*
* @purpose    Generic API functions for producing a TLV structure
*
* @component  tlv
*
* @comments   none
*
* @create     07/05/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include <string.h>
#include <stdio.h>
#include "l7_common.h"                          /* needed by sysapi.h */
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "trace_api.h"
#include "log.h"
#include "tlv.h"
#include "l7_resources.h"


/* fle globals */
static L7_tlvCtrl_t *   TlvListAnchor = L7_NULLPTR;   /* TLV list anchor     */
static void *           TlvListSemId = L7_NULLPTR;    /* TLV list semaphore  */
static L7_BOOL          TlvFirstRegister = L7_FALSE;  /* first registration  */
static L7_tlvCtrl_t *   TlvInUse[L7_TLV_HANDLE_MAX+1];  /*handle in-use array*/

/* local function prototypes */
static L7_RC_t tlvCtrlPtrGet(L7_tlvHandle_t tlvHandle, 
                             L7_tlvCtrl_t **ppTlvCtrl);
static L7_RC_t tlvCtrlPtrActiveGet(L7_tlvHandle_t tlvHandle, 
                                   L7_tlvCtrl_t **ppTlvCtrl);
static void tlvCtrlBlockInit(L7_tlvCtrl_t *pTlvCtrl);
static L7_RC_t tlvCtrlEnq(L7_tlvCtrl_t **pList, L7_tlvCtrl_t *pElem);
static L7_RC_t tlvCtrlDeq(L7_tlvCtrl_t **pList, L7_tlvCtrl_t *pElem);
static L7_RC_t tlvWalk(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                       L7_tlvUserParseFunc_t userParseFunc,
                       L7_uint32 *pEntrySize);


/*********************************************************************
* @purpose  Allocates resources for construction of a new TLV block
*
* @param    tlvSizeMax  @{(input)}  Maximum size of TLV block needed
* @param    appId       @{(input)}  Application identifier
* @param    appName     @{(input)}  Application name string
* @param    pTlvHandle  @{(output)} Pointer to TLV handle output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not enough resources for a new TLV
* @returns  L7_FAILURE  
*
* @notes    The tlvSizeMax parm is used as an upper limit of the TLV
*           block size required by the application.  It must be specified
*           as a value from 4 to 65536.
*
* @notes    The appId parm is primarily used for debugging.  It is 
*           recommended that a component ID (or something similar)
*           be used as a means of identifying which application owns
*           a particular TLV block resource.  A value of zero is permitted.
*       
* @end
*********************************************************************/
L7_RC_t tlvRegister(L7_uint32 tlvSizeMax, L7_uint32 appId, L7_uchar8 *pAppName,
                    L7_tlvHandle_t *pTlvHandle)
{
  L7_uint32       i;
  L7_tlvHandle_t  tlvHandle;
  L7_uint32       dataAreaSize, allocSize;
  L7_tlvCtrl_t    *pTlvCtrl;
  L7_uchar8       traceNameStr[L7_TLV_TRACE_NAME_LEN+1];
  L7_uint32       traceId;

  if (pTlvHandle == L7_NULLPTR)
    return L7_FAILURE;
  if (L7_TLV_BLK_SIZE_IN_RANGE(tlvSizeMax) != L7_TRUE)
    return L7_FAILURE;

  *pTlvHandle = L7_TLV_HANDLE_INVALID;          /* init to invalid value */

  /* one-time init (upon first application registration) */
  if (TlvFirstRegister == L7_FALSE)
  {
    /* create a semaphore to control mutual exclusion access to the linked
     * list of TLV blocks
     *
     * NOTE: only attempt semaphore creation once
     */
    TlvListSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (TlvListSemId == L7_NULLPTR)
    {
	  L7_LOG(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "tlvRegister: Unable to create TLV "
						"block list semaphore. System resource issue within TLV utility.\n");
      return L7_FAILURE;
    }

    /* initialize TLV handle in-use array */
    for (i = 0; i <= L7_TLV_HANDLE_MAX; i++)
      TlvInUse[i] = L7_NULLPTR;

    TlvFirstRegister = L7_TRUE;
  }

  /* make sure TLV block semaphore ID exists before allowing any registration
   * to proceed
   *
   * NOTE: by checking this here, each API does not need to check for it too,
   *       since a TLV handle will only be issued if the utility is operational
   */
  if (TlvListSemId == L7_NULLPTR)
    return L7_FAILURE;

  /* find first available handle index that is not currently in use
   * NOTE: don't use array element 0 
   */
  tlvHandle = L7_TLV_HANDLE_INVALID;
  for (i = 1; i <= L7_TLV_HANDLE_MAX; i++)
  {
    if (TlvInUse[i] == L7_NULLPTR)
    {
      tlvHandle = i;
      break;
    }
  }
  if ((i > L7_TLV_HANDLE_MAX) || (tlvHandle == L7_TLV_HANDLE_INVALID))
  {
	L7_LOG(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID, "tlvRegister: All TLV blocks currently "
					"in use. Maximum number of supported TLV registrants are already in use.\n");
    return L7_ERROR;
  }

  /* round up requested block size to 4-byte boundary */
  dataAreaSize = (tlvSizeMax + L7_TLV_AREA_ROUNDUP_MASK) & 
                   ~L7_TLV_AREA_ROUNDUP_MASK;

  /* allocate a buffer for the TLV block and initialize it to 0
   * NOTE: relying on OSAPI to zero entire block content
   */
  allocSize = L7_TLV_BLK_ALLOC_SIZE(dataAreaSize);
  if ((pTlvCtrl = (L7_tlvCtrl_t *)osapiMalloc(L7_SIM_COMPONENT_ID, allocSize)) == L7_NULLPTR)
    return L7_ERROR;

  /* chain new TLV block into linked list */
  if (tlvCtrlEnq(&TlvListAnchor, pTlvCtrl) != L7_SUCCESS)
  {
    osapiFree(L7_SIM_COMPONENT_ID, pTlvCtrl);
    return L7_FAILURE;
  }

  /* assign this TLV block to its handle */
  TlvInUse[i] = pTlvCtrl;
  pTlvCtrl->tlvHandle = i;

  /* initialize registration control fields */
  pTlvCtrl->allocSize = allocSize;
  pTlvCtrl->appId = appId;                      /* owner application ID */
  memcpy(pTlvCtrl->appName, pAppName, L7_TLV_APP_NAME_MAX);
  pTlvCtrl->appName[L7_TLV_APP_NAME_MAX] = (L7_uchar8)'\0';

  /* set max offset value based on requested (not allocated) size; 
   * this field is only updated during registration
   */
  pTlvCtrl->maxOffset = L7_TLV_BLK_ALLOC_SIZE(tlvSizeMax);  /* only set here */

  /* initialize TLV block operational fields */
  tlvCtrlBlockInit(pTlvCtrl);

  /* init size stats */
  pTlvCtrl->sizeLo = 0;
  pTlvCtrl->sizeHi = 0;

  /* acquire a trace buffer */
  sprintf((char *)traceNameStr, "TLV%8.8x", (L7_uint32)pTlvCtrl);
  if (traceBlockCreate(L7_TLV_TRACE_ENTRY_MAX, L7_TLV_TRACE_ENTRY_SIZE_MAX,
                       traceNameStr, &traceId) != L7_SUCCESS)
  {
	L7_LOGF(L7_LOG_SEVERITY_CRITICAL , L7_SIM_COMPONENT_ID, "tlvRegister: Unable to obtain trace buffer "
							"for App ID %u, TLV handle 0x%8.8x. FASTPATH trace component is out of trace "
							"buffers.\n", appId, (L7_uint32)pTlvCtrl);
    traceId = L7_TLV_TRACE_ID_INVALID;
  }
  else
  {
    if (traceBlockStart(traceId) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR , L7_SIM_COMPONENT_ID, "tlvRegister: Unable to start tracing for App "
			  "ID %u, TLV handle 0x%8.8x. Failed to start TLV tracing, most likely due to an invalid trace "
			  "buffer identifier.\n", appId, (L7_uint32)pTlvCtrl);
    }
  }
  pTlvCtrl->traceId = traceId;

  /* output TLV handle */
  *pTlvHandle = tlvHandle;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroys the specified TLV block and deallocates any dynamic
*           memory used during ts construction
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Upon successful completion, the tlvHandle value no longer
*           represents this TLV block.
*
* @notes    This function can be issued at any time regardless of the 
*           state of the TLV block.
*       
* @end
*********************************************************************/
L7_RC_t tlvUnregister(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* release the trace buffer */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    if (traceBlockDelete(pTlvCtrl->traceId) != L7_SUCCESS)
      return L7_FAILURE;
    pTlvCtrl->traceId = L7_TLV_TRACE_ID_INVALID;
  }

  /* remove TLV block from linked list */
  if (tlvCtrlDeq(&TlvListAnchor, pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* mark handle available */
  TlvInUse[tlvHandle] = L7_NULLPTR;

  /* de-allocate TLV block buffer */
  osapiFree(L7_SIM_COMPONENT_ID, pTlvCtrl);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Activates specified TLV block for construction
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block currently in use
* @returns  L7_FAILURE  
*
* @notes    Once this command completes successfully, a tlvDelete() is needed
*           before this TLV block can be reused for a subsequent TLV creation.
*           The L7_ERROR return indicates the TLV block is currently in use.
*
* @end
*********************************************************************/
L7_RC_t tlvCreate(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_CREATE);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE(0);
    TRACE_ENTER_2BYTE(0);
    TRACE_END();
  }

  /* make sure TLV block not currently active */
  if (L7_TLV_IS_ACTIVE(pTlvCtrl) == L7_TRUE)
    return L7_ERROR;

  /* clear out the old TLV data
   * NOTE: this is done at creation rather than deletion to aid in debugging
   */
  memset(L7_TLV_BLK_DATA_START(pTlvCtrl), 0, L7_TLV_BLK_DATA_LENGTH(pTlvCtrl));

  /* re-initialize the variable portion of the TLV control info */
  tlvCtrlBlockInit(pTlvCtrl);

  /* mark the TLV block active */
  pTlvCtrl->active = L7_YES;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicates construction of entire specified TLV block is complete
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block still open
* @returns  L7_FAILURE  
*
* @notes    Once this command completes successfully, the specified TLV block
*           contents can no longer be updated.  Use the tlvQuery() function to
*           obtain starting location and length of this completed TLV block.
*
* @notes    The L7_ERROR return is typically due to one or more tlvOpen() 
*           functions not having an associated tlvClose() issued prior to
*           the tlvComplete().
*
* @notes    Regardless of the return value from this function, tlvDelete()
*           must still be called to dispose of any TLV that has been created. 
*       
* @end
*********************************************************************/
L7_RC_t tlvComplete(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_ushort16   tlvType, tlvLen;
  L7_uint32     totalLen;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* extract TLV type and length */
  totalLen = L7_TLV_BLK_DATA_LENGTH(pTlvCtrl);
  tlvType = osapiNtohs(*(L7_ushort16 *)L7_TLV_BLK_DATA_START(pTlvCtrl));
  tlvLen = (L7_ushort16)(totalLen - L7_TLV_ENTRY_SIZE());

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_COMPLETE);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE(tlvType);
    TRACE_ENTER_2BYTE(tlvLen);
    TRACE_END();
  }

  /* make sure no TLV entries are still open */
  if (pTlvCtrl->numOpen > 0)
    return L7_ERROR;

  pTlvCtrl->complete = L7_YES;

  /* update size stats */
  if ((pTlvCtrl->sizeLo == 0) || (pTlvCtrl->sizeLo > totalLen))
    pTlvCtrl->sizeLo = totalLen;
  if (pTlvCtrl->sizeHi < totalLen)
    pTlvCtrl->sizeHi = totalLen;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deactivates specified TLV block, preventing further use
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block was not created
* @returns  L7_FAILURE  
*
* @notes    Upon successful completion, the TLV block is no longer 
*           considered to exist and should not be accessed with the
*           TLV pointer previously provided by tlvQuery().
*
* @notes    This function can be called any time following a tlvCreate()
*           and does not require a tlvComplete() to be issued first.  All
*           outstanding tlvOpen() operations are considered null-and-void.
*
* @notes    The actual contents of the TLV block control and data areas
*           are left intact until a subsequent tlvCreate() is issued to
*           help facilitate debugging.  This content is still considered
*           invalid from an operational perspective, however.
*
* @end
*********************************************************************/
L7_RC_t tlvDelete(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle (going to check for active separately) */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_DELETE);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE(0);
    TRACE_ENTER_2BYTE(0);
    TRACE_END();
  }

  /* make sure TLV block is currently active */
  if (L7_TLV_IS_ACTIVE(pTlvCtrl) != L7_TRUE)
    return L7_ERROR;

  /* mark the TLV block inactive */
  pTlvCtrl->active = L7_NO;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Provides location and total length of completed TLV block
*
* @param    tlvHandle   @{(input)}  TLV block handle
* @param    ppTlv       @{(output)} Pointer to TLV buffer pointer output location
* @param    pTlvLen     @{(output)} Pointer to TLV length output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Specified TLV block has not been completed
* @returns  L7_FAILURE  
*
* @notes    This function is only valid following a successful tlvComplete().
*
* @notes    The output TLV length value represents the total number of bytes
*           occupied by the TLV block in the output buffer, including the
*           type and length fields of the first TLV within it.
*       
* @end
*********************************************************************/
L7_RC_t tlvQuery(L7_tlvHandle_t tlvHandle, L7_tlv_t **ppTlv, 
                 L7_uint32 *pTlvLen)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* this request not valid until after TLV block is complete */
  if (L7_TLV_IS_COMPLETE(pTlvCtrl) == L7_FALSE)
    return L7_ERROR;

  /* output requested information */
  *ppTlv = (L7_tlv_t *)L7_TLV_BLK_DATA_START(pTlvCtrl);
  *pTlvLen = L7_TLV_BLK_DATA_LENGTH(pTlvCtrl);

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_QUERY);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE((*ppTlv)->type);
    TRACE_ENTER_2BYTE((L7_ushort16)*pTlvLen);
    TRACE_END();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Appends a fixed-sized TLV entry to the specified TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
* @param    length      @{(input)} Number of bytes in pValue buffer
* @param    pValue      @{(input)} Pointer to TLV entry data character buffer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    All data passed through pValue is treated as a character array
*           and must be in big-endian format (per the TLV definition).
*
* @notes    If no additional data is defined for the TLV entry, the length
*           value must be specified as 0 (pValue is ignored in this case).
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there
*           is no more room in the TLV block for this TLV entry.
*
* @end
*********************************************************************/
L7_RC_t tlvWrite(L7_tlvHandle_t tlvHandle, L7_uint32 type, L7_uint32 length,
                 L7_uchar8 *pValue)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_tlvHdr_t   *pTlvHdr;
  L7_uint32     totalLength;
  L7_uint32     i;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* check data inputs */
  if (type > (L7_uint32)L7_TLV_TYPE_VAL_MAX)
    return L7_FAILURE;
  if (length > (L7_uint32)L7_TLV_LENGTH_VAL_MAX)
    return L7_FAILURE;
  if ((length != 0) && (pValue == L7_NULLPTR))
    return L7_FAILURE;

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_WRITE);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE((L7_ushort16)type);
    TRACE_ENTER_2BYTE((L7_ushort16)length);
    TRACE_END();
  }

  /* this request only valid before TLV block is complete */
  if (L7_TLV_IS_COMPLETE(pTlvCtrl) == L7_TRUE)
    return L7_ERROR;

  /* calculate total length of this TLV entry */
  totalLength = L7_TLV_ENTRY_SIZE() + length;

  /* indicate error if not enough room remaining in TLV block */
  if (L7_TLV_BLK_HAS_ROOM(pTlvCtrl, totalLength) != L7_TRUE)
      return L7_ERROR;

  /* set the TLV entry type and length fields
   *
   * NOTE:  Must store into TLV in network byte order (big endian)!
   */
  pTlvHdr = (L7_tlvHdr_t *)L7_TLV_NEXT_WRITE_PTR(pTlvCtrl);
  pTlvHdr->type = osapiHtons((L7_ushort16)type);
  pTlvHdr->length = osapiHtons((L7_ushort16)length);

  /* copy the caller's TLV value data, if any
   *
   * NOTE:  Must store into TLV in network byte order (big endian)!
   */
  if (length != 0)
  {
    L7_uchar8 *pDst = L7_TLV_ENTRY_DATA_START(pTlvHdr);
    memcpy(pDst, pValue, length);
  }

  /* update TLV block controls */
  pTlvCtrl->nextOffset += totalLength;

  /* update length of all open TLVs since this TLV entry is nested within them
   *
   * NOTE: Since the total length is used here, tlvOpen() must not add a 
   *       newly opened TLV to the openOffset[] list until AFTER this call is 
   *       made.
   *
   * NOTE: By definition, if the first TLV entry in the TLV block is 
   *       variable-sized (i.e., it contains subordinate TLVs), it is
   *       the first open TLV and its length field is maintained just like
   *       any other open TLV.
   */
  for (i = 0; i < pTlvCtrl->numOpen; i++)
  {
    L7_tlvHdr_t *pOpenHdr;
    L7_uint32   oldLength;

    if (pTlvCtrl->openOffset[i] == L7_TLV_OPEN_STK_UNUSED)
      break;
    pOpenHdr = (L7_tlvHdr_t *)L7_TLV_OPEN_HDR_START(pTlvCtrl, i);
    oldLength = osapiNtohs((L7_ushort16)pOpenHdr->length);
    pOpenHdr->length = osapiHtons((L7_ushort16)(oldLength + totalLength));
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Starts appending a variable-sized TLV entry to the specified 
*           TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
* @param    length      @{(input)} Number of bytes in pValue buffer
* @param    pValue      @{(input)} Pointer to TLV entry data character buffer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    All data passed through pValue is treated as a character array
*           and must be in big-endian format (per the TLV definition).
*
* @notes    A variable-sized TLV entry may contain some natively-defined
*           data (per length and pValue parms) and often contains additional
*           fixed- and/or variable-sized TLVs within its value field.
*
* @notes    If no native data is defined for the TLV entry, the length
*           value must be specified as 0 (pValue is ignored in this case);
*           this has no bearing on whether any nested TLVs are defined 
*           within this TLV entry.
*
* @notes    Each tlvOpen() must be paired with a corresponding tlvClose().
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there are
*           too many TLVs open within the TLV block simultaneously.
*       
* @end
*********************************************************************/
L7_RC_t tlvOpen(L7_tlvHandle_t tlvHandle, L7_uint32 type, L7_uint32 length,
                L7_uchar8 *pValue)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_uint32     openOffset;
  L7_RC_t       rc;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_OPEN);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE((L7_ushort16)type);
    TRACE_ENTER_2BYTE((L7_ushort16)pTlvCtrl->nextOffset);
    TRACE_END();
  }

  /* this request only valid before TLV block is complete */
  if (L7_TLV_IS_COMPLETE(pTlvCtrl) == L7_TRUE)
    return L7_ERROR;

  /* make sure not already at max nesting level */
  if (pTlvCtrl->numOpen >= L7_TLV_OPEN_STK_MAX)
    return L7_ERROR;

  /* save starting offset of where this TLV entry will be written */
  openOffset = pTlvCtrl->nextOffset;

  /* re-use the TLV write function to set up this TLV entry's info
   * (let it perform parm validity checking)
   *
   * NOTE:  This must be called BEFORE creating the new open nest level.
   */
  if ((rc = tlvWrite(tlvHandle, type, length, pValue)) != L7_SUCCESS)
    return rc;

  /* create the nest level for this open TLV */
  pTlvCtrl->openOffset[pTlvCtrl->numOpen++] = openOffset;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Finishes the definition of a currently-open TLV entry within
*           the specified TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    Each tlvClose() must be paired with its corresponding tlvOpen().
*           Once a TLV entry is successfully closed, its contents can no 
*           longer be modified (does not terminate modification to the overall
*           TLV block, however -- see tlvComplete() function)
*
* @notes    The type parameter must be the same as was used for the
*           corresponding tlvOpen(), but is used more for error checking since
*           it is not always guaranteed to uniquely identify which of the 
*           currently open TLV entries is being closed.
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there are
*           no TLVs open within the TLV block.
*       
* @end
*********************************************************************/
L7_RC_t tlvClose(L7_tlvHandle_t tlvHandle, L7_uint32 type)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_tlvHdr_t   *pOpenHdr;
  L7_uint32     nestLvl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the TLV header info for this open nest level */
  nestLvl = pTlvCtrl->numOpen - 1;
  pOpenHdr = (L7_tlvHdr_t *)L7_TLV_OPEN_HDR_START(pTlvCtrl, nestLvl);

  /* TRACE_POINT */
  if (pTlvCtrl->traceId != L7_TLV_TRACE_ID_INVALID)
  {
    TRACE_BEGIN(pTlvCtrl->traceId, L7_TLV_TRACE_CODE_CLOSE);
    TRACE_ENTER_2BYTE((L7_ushort16)osapiTimeMillisecondsGet());
    TRACE_ENTER_4BYTE((L7_uint32)tlvHandle);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->active);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->complete);
    TRACE_ENTER_BYTE((L7_uchar8)pTlvCtrl->numOpen);
    TRACE_ENTER_BYTE(0);
    TRACE_ENTER_2BYTE((L7_ushort16)type);
    TRACE_ENTER_2BYTE(osapiNtohs(pOpenHdr->length));
    TRACE_END();
  }

  /* this request not valid once TLV block is complete */
  if (L7_TLV_IS_COMPLETE(pTlvCtrl) == L7_TRUE)
    return L7_ERROR;

  /* make sure at least one TLV is open */
  if (pTlvCtrl->numOpen == 0)
    return L7_ERROR;

  /* the TLV open stack is LIFO, so close the most recently opened TLV
   *
   * as a crude sanity check, make sure the Type field of the open TLV
   * matches the one specified in the close -- there is no requirement 
   * that only one TLV of a given Type be opened at any one time, however
   *
   * NOTE:  The TLV length field was updated each time information was
   *        written within this TLV nest level, so no need to update it here.
   */
  if ((L7_uint32)osapiNtohs(pOpenHdr->type) != type)
    return L7_FAILURE;

  pTlvCtrl->numOpen--;
  pTlvCtrl->openOffset[nestLvl] = L7_TLV_OPEN_STK_UNUSED;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if the specified TLV block handle is valid
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_TRUE     TLV handle is registered
* @returns  L7_FALSE    TLV handle not registered
*
* @notes    This function indicates that a TLV block corresponding
*           to the specified handle is registered.  It does not imply
*           anything about the current state or contents of the TLV block.
*
* @end
*********************************************************************/
L7_BOOL tlvHandleIsValid(L7_tlvHandle_t tlvHandle)
{
  if (tlvHandle == L7_TLV_HANDLE_INVALID)
    return L7_FALSE;

  return (TlvInUse[tlvHandle] != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if the specified TLV handle represents an active
*           TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_TRUE     TLV handle is valid
* @returns  L7_FALSE    TLV handle not valid
*
* @notes    This function indicates that a TLV block corresponding
*           to the specified handle is both registered and created.  It 
*           does not imply anything about the completion state or contents
*           of the TLV block.
*
* @end
*********************************************************************/
L7_BOOL tlvBlockIsActive(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl = L7_NULLPTR;

  (void)tlvCtrlPtrActiveGet(tlvHandle, &pTlvCtrl);  /* check ptr instead of rc*/
  return (pTlvCtrl != L7_NULLPTR) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
* @purpose  Parse the TLV contents
*
* @param    pTlv          @{(input)} Pointer to TLV start location
* @param    userParseFunc @{(input)} User parser function
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Error reported by user parser function
* @returns  L7_FAILURE
*
* @notes    Does not interpret the TLV contents, but rather navigates the
*           TLV, including any nested TLVs.  A user function is called
*           to do any TLV type-specific interpretation and processing (see
*           typedef for function prototype).
*
* @notes    Assumes TLV was properly built using the TLV utility APIs.
*
* @end
*********************************************************************/
L7_RC_t tlvParse(L7_tlv_t *pTlv, L7_tlvUserParseFunc_t userParseFunc)
{
  L7_ushort16   tlvType, tlvLength;
  L7_uint32     totalSize, entrySize;
  L7_RC_t       rc;

  /* check inputs */
  if (pTlv == L7_NULLPTR)
    return L7_FAILURE;
  if (userParseFunc == L7_NULLPTR)
    return L7_FAILURE;

  /* extract type and length of primary (i.e., first) TLV */
  tlvType = osapiNtohs(pTlv->type);
  tlvLength = osapiNtohs(pTlv->length);

  /* calculate the total TLV size (to compare against derived size later) */
  totalSize = L7_TLV_HEADER_SIZE + tlvLength;

  /* walk through the TLV block entry-by-entry (USES RECURSION!)
   * NOTE: always start at nest level 0
   */
  entrySize = 0;
  if ((rc = tlvWalk(pTlv, 0, userParseFunc, &entrySize))
      != L7_SUCCESS)
  {
	L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "tlvParse: Error processing contents of TLV"
			" 0x%4.4x at address 0x%8.8x, rc=%u. Indicates a malformed type-length-value data block "
			"presented by the caller of the TLV utility.\n", tlvType, (L7_uint32)pTlv, rc);
    return rc;
  }

  if (totalSize != entrySize)
  {
	L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID, "tlvParse: TLV size mismatch: expected=%u, "
                    "derived=%u. Indicates a malformed type-length-value data block presented by the "
					"caller of the TLV utility.\n", totalSize, entrySize);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start tracing for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Trace is automatically started when each TLV block is registered.
*
* @end
*********************************************************************/
L7_RC_t tlvTraceStart(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  return traceBlockStart(pTlvCtrl->traceId);
}

/*********************************************************************
* @purpose  Stop tracing for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t tlvTraceStop(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  return traceBlockStop(pTlvCtrl->traceId);
}

/*********************************************************************
* @purpose  Display the trace data for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
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
L7_RC_t tlvTraceShow(L7_tlvHandle_t tlvHandle, L7_BOOL format, L7_uint32 count)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j;
  L7_uchar8     traceCode, *pCodeStr;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  traceData = traceBlockFind(pTlvCtrl->traceId);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate trace block for trace ID 0x%8.8x\n",
                  (L7_uint32)pTlvCtrl->traceId);
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
        for (j = 1; j <= traceData->bytesPerEntry; j++)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                        "%2.2x ", *pOffset++);
          if (j == (traceData->bytesPerEntry / L7_TLV_TRACE_DISPLAY_SECTIONS))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- "); 
          }
        } /* endfor j */

      } /* endif unformatted */

      else  /* manually format output */
      {
        /* the first 12 bytes are the same for all TLV trace entries */
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Id=0x%2.2x ", *pOffset++);
        traceCode = *pOffset;
        /* translate trace code to string */
        switch (traceCode)
        {
        case L7_TLV_TRACE_CODE_CREATE:
          pCodeStr = (L7_uchar8 *)"Create";
          break;
        case L7_TLV_TRACE_CODE_COMPLETE:
          pCodeStr = (L7_uchar8 *)"Cmplt ";
          break;
        case L7_TLV_TRACE_CODE_DELETE:
          pCodeStr = (L7_uchar8 *)"Delete";
          break;
        case L7_TLV_TRACE_CODE_QUERY:
          pCodeStr = (L7_uchar8 *)"Query ";
          break;
        case L7_TLV_TRACE_CODE_WRITE:
          pCodeStr = (L7_uchar8 *)"Write ";
          break; 
        case L7_TLV_TRACE_CODE_OPEN:
          pCodeStr = (L7_uchar8 *)"Open  ";
          break;
        case L7_TLV_TRACE_CODE_CLOSE:
          pCodeStr = (L7_uchar8 *)"Close ";
          break;
        default:
          pCodeStr = (L7_uchar8 *)"??????";
          break;
        }
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"Code=%-6s ", pCodeStr);
        pOffset++;
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Tstmp=%5.5u ", *(L7_ushort16 *)pOffset);
        pOffset += sizeof(L7_ushort16);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Hndl=%u ", *(L7_uint32 *)pOffset);
        pOffset += sizeof(L7_uint32);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Act=%u ", *pOffset++);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Cmp=%u ", *pOffset++);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Opn=%u ", *pOffset++);
        pOffset++;                                /* skip unused byte */

        /* certain TLV trace points contain up to 4 bytes of additional info */
        switch (traceCode)
        {
        case L7_TLV_TRACE_CODE_COMPLETE:
        case L7_TLV_TRACE_CODE_WRITE:
        case L7_TLV_TRACE_CODE_CLOSE:
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Type=0x%4.4x ", *(L7_ushort16 *)pOffset);
          pOffset += sizeof(L7_ushort16);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Len=0x%4.4x ", *(L7_ushort16 *)pOffset);
          break;

        case L7_TLV_TRACE_CODE_QUERY:
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Type=0x%4.4x ", *(L7_ushort16 *)pOffset);
          pOffset += sizeof(L7_ushort16);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "TotLen=0x%4.4x ", *(L7_ushort16 *)pOffset);
          break;

        case L7_TLV_TRACE_CODE_OPEN:
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "Type=0x%4.4x ", *(L7_ushort16 *)pOffset);
          pOffset += sizeof(L7_ushort16);
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "NxtOfst=0x%4.4x ", *(L7_ushort16 *)pOffset);
          break;

        default:
          break;
        } /* endswitch */

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


/*====================== Utility Internal Helper Functions ===================*/

/*********************************************************************
* @purpose  Get the TLV block control from the specified TLV handle
*           regardless of whether TLV is active or not
*
* @param    tlvHandle   @{(input)}  TLV block handle
* @param    ppTlvCtrl   @{(output)} Pointer to TLV block pointer
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Internal function only.
*
* @notes    Only use this function if the TLV active state is immaterial.
*           If the TLV must be in the active state, use the 
*           tlvCtrlPtrActiveGet() function instead.
*       
* @end
*********************************************************************/
static L7_RC_t tlvCtrlPtrGet(L7_tlvHandle_t tlvHandle, 
                             L7_tlvCtrl_t **ppTlvCtrl)
{
  if (ppTlvCtrl == L7_NULLPTR)
    return L7_FAILURE;

  if (tlvHandleIsValid(tlvHandle) != L7_TRUE)
  {
    *ppTlvCtrl = L7_NULLPTR;
    return L7_FAILURE;
  }

  /* use the handle to index the in-use array for the TLV control ptr */
  *ppTlvCtrl = TlvInUse[tlvHandle];
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the TLV block control from the specified TLV handle,
*           but only if the TLV is currently active
*
* @param    tlvHandle   @{(input)}  TLV block handle
* @param    ppTlvCtrl   @{(output)} Pointer to TLV block pointer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Handle is valid, but TLV not active
* @returns  L7_FAILURE  
*
* @notes    Internal function only.
*
* @notes    Use this function if the TLV is required to be active, such
*           as when working with the TLV block contents.  An active TLV
*           block is one for which tlvCreate() has successfully occurred.
*           Use tlvCtrlPtrGet() instead if the active state is immaterial. 
*       
* @end
*********************************************************************/
static L7_RC_t tlvCtrlPtrActiveGet(L7_tlvHandle_t tlvHandle, 
                                   L7_tlvCtrl_t **ppTlvCtrl)
{
  /* rely on checking within common conversion function */
  if (tlvCtrlPtrGet(tlvHandle, ppTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;

  if (L7_TLV_IS_ACTIVE(*ppTlvCtrl) != L7_TRUE)
  {
    *ppTlvCtrl = L7_NULLPTR;
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initializes operational fields of TLV block control structure
*
* @param    pTlvCtrl    @{(input)} TLV block pointer
*
* @returns  void  
*
* @notes    Internal function only.
*
* @notes    Must be called each time tlvCreate() is used.
*       
* @end
*********************************************************************/
static void tlvCtrlBlockInit(L7_tlvCtrl_t *pTlvCtrl)
{
  L7_uint32     i;

  if (pTlvCtrl == L7_NULLPTR)
  {
    LOG_MSG("tlvCtrlBlockInit: TLV control pointer parm is null.\n");
    return;
  }

  /* initialize non-constant block control fields
   * NOTE: maxOffset contains original alloc size so it is left alone here
   */
  pTlvCtrl->active = L7_NO;
  pTlvCtrl->complete = L7_NO;
  pTlvCtrl->nextOffset = (L7_uint32)L7_TLV_CTRL_SIZE();
  pTlvCtrl->numOpen = 0;
  for (i = 0; i < L7_TLV_OPEN_STK_MAX; i++)
    pTlvCtrl->openOffset[i] = L7_TLV_OPEN_STK_UNUSED;
}

/*********************************************************************
* @purpose  Enqueue a TLV block to the linked list
*
* @param    pList       @{(input)} Pointer to TLV linked list anchor
* @param    pElem       @{(input)} TLV block element pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Internal function only.
*
* @notes    Enqueues the element at the front of the list, since it is
*           assumes that dequeues are very infrequent.
*       
* @end
*********************************************************************/
static L7_RC_t tlvCtrlEnq(L7_tlvCtrl_t **pList, L7_tlvCtrl_t *pElem)
{
  /* check inputs */
  if (pList == L7_NULLPTR)
    return L7_FAILURE;
  if (pElem == L7_NULLPTR)
    return L7_FAILURE;

  pElem->pChain = *pList;
  *pList = pElem;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Dequeue a TLV block from the linked list
*
* @param    pList       @{(input)} Pointer to TLV linked list anchor
* @param    pElem       @{(input)} TLV block element pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Element not found
* @returns  L7_FAILURE
*
* @notes    Internal function only.
*
* @end
*********************************************************************/
static L7_RC_t tlvCtrlDeq(L7_tlvCtrl_t **pList, L7_tlvCtrl_t *pElem)
{
  L7_tlvCtrl_t  *p;

  /* check inputs */
  if (pList == L7_NULLPTR)
    return L7_FAILURE;
  if (pElem == L7_NULLPTR)
    return L7_FAILURE;

  /* handle case where elem is first in the list */
  if (*pList == pElem)
  {
    *pList = pElem->pChain;
    return L7_SUCCESS;
  }

  /* search list for elem */
  p = *pList;
  while (p != L7_NULLPTR)
  {
    if (p->pChain == pElem)
    {
      p->pChain = pElem->pChain;
      return L7_SUCCESS;
    }
    p = p->pChain;
  }

  return L7_ERROR;
}

/*********************************************************************
* @purpose  Walk the TLV entry-by-entry
*
* @param    pTlv          @{(input)}  TLV entry start location
* @param    nestLvl       @{(input)}  Nesting level of this TLV (0=top level)
* @param    userParseFunc @{(input)}  User parser function
* @param    pEntrySize    @{(output)} Pointer to TLV entry size output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Error reported by user parser function
* @returns  L7_FAILURE
*
* @notes    Internal function only.
*
* @notes    USES RECURSION to process nested TLVs within the value field 
*           of the specified TLV.
*
* @notes    The value output via the pEntrySize parameter must be the total
*           size of the TLV, which includes its type and length fields.
*
* @end
*********************************************************************/
static L7_RC_t tlvWalk(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                       L7_tlvUserParseFunc_t userParseFunc,
                       L7_uint32 *pEntrySize)
{
  L7_RC_t       rc;
  L7_uint32     size;                           /* working entry size value */
  L7_int32      remainLen;                      /* NOTE: must be SIGNED int */

  /* derive a remaining length for this TLV, including type, length fields */
  remainLen = L7_TLV_HEADER_SIZE + osapiNtohs(pTlv->length);

  *pEntrySize = 0;

  /* invoke user parser function to process native contents of TLV entry
   * (e.g., to display TLV header info plus native data, if any)
   */
  size = 0;
  rc = (*userParseFunc)(pTlv, nestLvl, &size);
  *pEntrySize += size;
  if ((rc != L7_SUCCESS) || (size < L7_TLV_HEADER_SIZE))
    return L7_ERROR;

  /* reduce remaining length by TLV header and native data amount */
  remainLen -= size;

  /* a positive remainLen means there must be nested TLV info, so continue
   * processing until it is consumed
   */
  while (remainLen > 0)
  {
    /* advance pointer to next TLV */
    pTlv = (L7_tlv_t *)((L7_uchar8 *)pTlv + size);
    size = 0;

    /* RECURSIVELY INVOKE OWN FUNCTION to process next TLV (w/new nest level) */
    rc = tlvWalk(pTlv, nestLvl+1, userParseFunc, &size);
    *pEntrySize += size;
    if (rc != L7_SUCCESS)
      return rc;

    remainLen -= size;
  }

  return L7_SUCCESS;
}


/*============================== Display Functions ===========================*/

/*********************************************************************
* @purpose  Display the TLV block control structure information
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t tlvCtrlShow(L7_tlvHandle_t tlvHandle)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_uint32     dataAreaSize, dataAreaUsed;
  L7_tlvHdr_t   *pHdr;
  L7_ushort16   valShort;
  L7_uint32     i;
  L7_uint32     j, jmax;
  L7_uchar8     *pData;

  /* get TLV control ptr from handle */
  if (tlvCtrlPtrGet(tlvHandle, &pTlvCtrl) != L7_SUCCESS)
    return L7_FAILURE;
  
  dataAreaSize = L7_TLV_BLK_DATA_MAX(pTlvCtrl);
  dataAreaUsed = L7_TLV_BLK_DATA_LENGTH(pTlvCtrl);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block handle          : %u\n", 
                (L7_uint32)pTlvCtrl->tlvHandle);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block location        : 0x%8.8x\n", (L7_uint32)pTlvCtrl);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block alloc size      : 0x%8.8x (%u)\n", 
                pTlvCtrl->allocSize, pTlvCtrl->allocSize);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block data area size  : 0x%8.8x (%u)\n", 
                dataAreaSize, dataAreaSize);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block data area used  : 0x%8.8x (%u)\n", 
                dataAreaUsed, dataAreaUsed);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block chain ptr       : 0x%8.8x\n",
                (L7_uint32)pTlvCtrl->pChain);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block application ID  : %u\n", pTlvCtrl->appId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block application name: %s\n", (char *)pTlvCtrl->appName);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV block trace ID        : 0x%8.8x\n", pTlvCtrl->traceId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV active                : %s\n", 
                (pTlvCtrl->active == L7_YES) ? "Yes" : "No");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV complete              : %s\n", 
                (pTlvCtrl->complete == L7_YES) ? "Yes" : "No");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Next data write offset    : 0x%4.4x (%u)\n", 
                pTlvCtrl->nextOffset, pTlvCtrl->nextOffset);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Max TLV data offset       : 0x%4.4x (%u)\n", 
                pTlvCtrl->maxOffset, pTlvCtrl->maxOffset);

  pHdr = (L7_tlvHdr_t *)L7_TLV_BLK_DATA_START(pTlvCtrl);
  valShort = osapiNtohs(pHdr->type);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Primary TLV type          : 0x%4.4x (%u)\n", 
                valShort, valShort);
  valShort = osapiNtohs(pHdr->length);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Primary TLV length        : 0x%4.4x (%u)\n", 
                valShort, valShort);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Number of open TLVs       : %u\n", pTlvCtrl->numOpen);
  /* display info about each open TLV nest level */
  for (i = 0; i < pTlvCtrl->numOpen; i++)
  {
    pHdr = (L7_tlvHdr_t *)L7_TLV_OPEN_HDR_START(pTlvCtrl, i);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
              "  [%2u] offset 0x%4.4x (%5u) -- ", 
              i, pTlvCtrl->openOffset[i], pTlvCtrl->openOffset[i]);
    pData = (L7_uchar8 *)pHdr;
    jmax = L7_TLV_ENTRY_SIZE() + min(8, pHdr->length);
    for (j = 0; j < jmax; j++)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%2.2x ", *pData++);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV size low-water mark   : %u\n", pTlvCtrl->sizeLo);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "TLV size high-water mark  : %u\n", pTlvCtrl->sizeHi);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "TLV Data (up to 64 bytes) :\n");

  /* display the first N bytes of the TLV data area */
  pData = L7_TLV_BLK_DATA_START(pTlvCtrl);
  jmax = min(dataAreaUsed, L7_TLV_SHOW_DATA_MAX);
  for (j = 1; j <= jmax; j++, pData++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%2.2x ", *pData);
    if ((j % L7_TLV_SHOW_DATA_ROW) == 0)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
    }
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display a summary list of registered TLV blocks          
*
* @param    void
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
void tlvBlockSummaryShow(void)
{
  L7_tlvCtrl_t  *pTlvCtrl;
  L7_uint32     i;

  pTlvCtrl = TlvListAnchor;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

  if (pTlvCtrl == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "  TLV block list is empty\n");
    return;
  }

  /* print output header */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Handle BlockAddr   AppID   Active  MaxSize  ");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "SizeLo  SizeHi AppName        \n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "------ ---------  -------  ------  -------  ");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "------  ------ ---------------\n");
  /* walk the TLV in-use array */
  for (i = 1; i <= L7_TLV_HANDLE_MAX; i++)
  {
    pTlvCtrl = TlvInUse[i];
    if (pTlvCtrl != L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    " %4u   %8.8x   %5u     %-3s  %7u    %5u   %5u %-15s\n",
                    pTlvCtrl->tlvHandle,
                    (L7_uint32)pTlvCtrl,
                    pTlvCtrl->appId,
                    (pTlvCtrl->active == L7_YES) ? "Yes" : "No",
                    L7_TLV_BLK_DATA_MAX(pTlvCtrl),
                    pTlvCtrl->sizeLo,
                    pTlvCtrl->sizeHi,
                    (char *)pTlvCtrl->appName);
    }
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
}

