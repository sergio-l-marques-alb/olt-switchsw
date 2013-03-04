/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_trace.c
*
* @purpose    Handle tracing for USL
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "trace_api.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "l7_usl_trace.h"
#include "l7_usl_bcm.h"
#include "osapi_support.h"

#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif

static L7_uint32 uslTraceHandle = ~0;
static L7_BOOL   uslTraceMode = L7_TRUE;

static L7_uint32 uslTraceMask = 0x00400000;
static L7_uint32 uslMacSyncTraceMask = 0;

static L7_uint32 uslSemaMask = 0;
static L7_uint32 uslSyncDetailMask = 0x00400000;
static L7_uint32 uslTraceMaxEntries = 0;
static L7_uint32 uslTraceMaxRecordWidth = 0;

/* local prototypes */
void usl_trace_mask_set(L7_uint32 mask);

/*********************************************************************
* @purpose  Initialize the trace utility for USL
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usl_trace_init(L7_uint32 numEntries,L7_uint32 width)
{
  L7_RC_t rc = L7_SUCCESS;

  /* make sure entries make sense */
  if (width < USL_TRACE_ENTRY_SIZE_MAX) width = USL_TRACE_ENTRY_SIZE_MAX;
  if (numEntries == 0) numEntries = USL_TRACE_ENTRY_MAX ;

  if (uslTraceHandle != ~0) (void)usl_trace_fini();

  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(numEntries, width,
                       (L7_uchar8 *)USL_TRACE_REGISTRATION_NAME,
                       &uslTraceHandle) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_E_LOG,"USL: unable to create the trace buffer\n");
    rc = L7_FAILURE;
    uslTraceHandle = ~0;
  }
  else
  {
    usl_trace_mask_set( USL_TRACE_GENERIC | USL_TRACE_SYNC );

    uslTraceMaxEntries = numEntries;
    uslTraceMaxRecordWidth = width;

    /* always start tracing during init */
    usl_trace_mode_apply(L7_TRUE);
  }
  return rc;
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
L7_RC_t usl_trace_fini(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (uslTraceHandle != ~0)
  {
    /* stop tracing at the mask level */
    usl_trace_mask_set(0);

    /* disable the trace util */
    usl_trace_mode_apply(L7_FALSE);

    /* delete the buffer */
    if (traceBlockDelete(uslTraceHandle) != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_E_LOG,"USL: unable to delete the trace buffer\n");
      rc = L7_FAILURE;
    }
    else
    {
      uslTraceHandle = ~0;
      uslTraceMaxEntries = uslTraceMaxRecordWidth = 0;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Create a trace for Broadcom unit attach/detach events
*
* @param    key       {(input)} The MAC address of the attaching/detaching unit
* @param    bcm_unit  {(input)} The bcm_unit of the attaching/detaching unit
* @param    attach    {(input)} L7_TRUE if the trace is an attach, L7 FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_bcom_event(bcm_mac_t key, L7_uint32 bcm_unit, L7_BOOL attach)
{
  L7_ushort16 traceId = 0;

  if (attach)
    traceId = USL_NEW_BCOM_UNIT;
  else
    traceId = USL_DEL_BCOM_UNIT;

  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE))
  {
    TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));

    if (attach != 0)
    {
      TRACE_ENTER_BYTE(key[0]);
      TRACE_ENTER_BYTE(key[1]);
      TRACE_ENTER_BYTE(key[2]);
      TRACE_ENTER_BYTE(key[3]);
      TRACE_ENTER_BYTE(key[4]);
      TRACE_ENTER_BYTE(key[5]);
    } else
    {
      TRACE_ENTER_BYTE(0);
      TRACE_ENTER_BYTE(0);
      TRACE_ENTER_BYTE(0);
      TRACE_ENTER_BYTE(0);
      TRACE_ENTER_BYTE(0);
      TRACE_ENTER_BYTE(0);
    }
    TRACE_ENTER_4BYTE(osapiHtonl(bcm_unit));
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Format the output for a bcom event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_bcom_event_format(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_RC_t rc = L7_SUCCESS;

  /* 6 byte key, 4 byte unit */
  /* KEY */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x ",
                pDataStart[0],pDataStart[1],pDataStart[2],
                pDataStart[3],pDataStart[4],pDataStart[5] );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "bcm_unit %d",osapiNtohl(*((L7_uint32*)&pDataStart[6])));

  return rc;
}

/*********************************************************************
* @purpose  Create a trace for FP unit/slot create/delete events
*
* @param    key       {(input)} The MAC address of the attaching/detaching unit
* @param    fp_unit   {(input)} The fastpath unit of the attaching/detaching unit
* @param    fp_slot   {(input)} The fastpath slot of the attaching/detaching slot
* @param    attach    {(input)} L7_TRUE if the trace is an attach, L7 FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_fp_event(bcm_mac_t key,
                        L7_uint32 fp_unit,
                        L7_uint32 fp_slot,
                        L7_BOOL attach)
{
  L7_uchar8 traceId = 0;

  if (attach)
    traceId = USL_NEW_FP_U_S;
  else
    traceId = USL_DEL_FP_U_S;

  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE))
  {
    TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_BYTE(key[0]);
    TRACE_ENTER_BYTE(key[1]);
    TRACE_ENTER_BYTE(key[2]);
    TRACE_ENTER_BYTE(key[3]);
    TRACE_ENTER_BYTE(key[4]);
    TRACE_ENTER_BYTE(key[5]);
    TRACE_ENTER_BYTE(0); /* Padding for alignment */
    TRACE_ENTER_BYTE(0); /* Padding for alignment */
    TRACE_ENTER_4BYTE(osapiHtonl(fp_unit));
    TRACE_ENTER_4BYTE(osapiHtonl(fp_slot));
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Format the output for a fastpath event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_fp_event_format(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_RC_t rc = L7_SUCCESS;

  /* 6 byte key, 4 byte unit */
  /* KEY */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x ",
                pDataStart[0],pDataStart[1],pDataStart[2],
                pDataStart[3],pDataStart[4],pDataStart[5] );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "fp_u/s %d/",osapiNtohl(*((L7_uint32*)&pDataStart[8])));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d",osapiNtohl(*((L7_uint32*)&pDataStart[12])));

  return rc;
}

/*********************************************************************
* @purpose  Create a trace event for when the database invalidate is invoked
*
* @param    starting    {(input)} L7_TRUE when the invalidate is starting
*                                 L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_invalidate(L7_BOOL starting)
{
  L7_uchar8 begin = 1;

  if (!starting) begin = 0;

  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE))
  {
    TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(USL_DB_INVALIDATE));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_BYTE(begin);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Format the output for a database invalidate
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_invalidate_format(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_RC_t rc = L7_SUCCESS;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s",(pDataStart[0])?" Started":" Finished");

  return rc;
}

/*********************************************************************
* @purpose  Create a trace for FP unit/slot create/delete events
*
* @param    db        {(input)} The database being synched
* @param    progress  {(input)} The point at which the database is during the sync
* @param    reason    {(input)} If the reason is an early exit, the reason
* @param    error     {(input)} The error code if the reason is error
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_sync(USL_DB_ID_t db,L7_uchar8 progress,L7_uchar8 reason,L7_int32 error)
{
  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE))
  {
    if ((db == USL_ANY_DB) ||
        ((db != USL_ANY_DB) && (uslTraceMask & USL_TRACE_SYNC)))
    {
      TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
      TRACE_ENTER_2BYTE(osapiHtons(USL_DB_SYNC));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
      TRACE_ENTER_BYTE(db);
      TRACE_ENTER_BYTE(progress);
      TRACE_ENTER_BYTE(reason);
      TRACE_ENTER_BYTE(0); /* Padding for alignment */
      TRACE_ENTER_4BYTE(osapiHtonl(error));
      TRACE_END();
    }
  }
}


/*********************************************************************
* @purpose  Format the output for a database sync event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_sync_format(L7_ushort16 traceId, L7_uchar8 *pDataStart)
{
  L7_uchar8 *pStr;
  L7_RC_t rc = L7_SUCCESS;

  pStr = usl_db_name_get((USL_DB_ID_t) pDataStart[0]);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-12s:",pStr);

  switch (pDataStart[1])
  {
    case USL_START:
      pStr = (L7_uchar8 *)USL_DB_PROGRESS_START_STRING;
      break;

    case USL_EARLY_EXIT:
      pStr = (L7_uchar8 *)USL_DB_PROGRESS_EARLY_EXIT_STRING ;
      break;

    case USL_FINISHED:
      pStr = (L7_uchar8 *)USL_DB_PROGRESS_FINISHED_STRING   ;
      break;

    default:
      pStr = (L7_uchar8 *)"??????";
      break;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                " %-10s",pStr);

  if (pDataStart[1] == USL_EARLY_EXIT)
  {
    switch (pDataStart[2])
    {
      case USL_NEW_SYNC_ENQUEUED:
        pStr =(L7_uchar8 *) USL_NEW_SYNC_ENQUEUED_STRING;
        break;

      case USL_ERROR:
        pStr =(L7_uchar8 *)USL_ERROR_STRING;
        break;

      default:
        pStr = (L7_uchar8 *)"??????";
        break;
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ", %-8s",pStr);


    if (pDataStart[2] == USL_ERROR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "%d",osapiNtohl(*((L7_int32 *)&pDataStart[4])));
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the trace mask for special tracing
*
* @param    mask    {(input)} A mask of the special semaphore enablement
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_sema_mask_set(L7_uint32 mask)
{
  uslSemaMask = mask;
}

/*********************************************************************
* @purpose  Create a trace for FP unit/slot create/delete events
*
* @param    db        {(input)} The database id
* @param    sema      {(input)} A four byte string (including the NULL terminator)
* @param    task      {(input)} The task the trace is running on
* @param    line      {(input)} The line within the file
* @param    tak       {(input)} 1 for semaTake, 0 for semaGive
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_sema(L7_uchar8 db,L7_uchar8 *sema,L7_int32 task,L7_ulong32 line,L7_BOOL take)
{
  L7_uchar8 take_give;
  take_give = (take)?1:0;


  if ((uslTraceHandle != ~0)
      && (uslTraceMode == L7_TRUE)
      && (uslTraceMask & USL_TRACE_SEMS)
      && (uslSemaMask & (1 << db)))
  {
    TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(USL_DB_SEMA));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(*(L7_uint32 *)sema);
    TRACE_ENTER_4BYTE(osapiHtonl(task));
    TRACE_ENTER_4BYTE(osapiHtonl(line));
    TRACE_ENTER_BYTE(take_give);
    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Format the output for a database sema event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_sema_format(L7_ushort16 traceId,L7_uchar8 *pDataStart)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s ",&pDataStart[0]);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "task=0x%0.8x line=%d %s",
                osapiNtohl(*((L7_uint32*)&pDataStart[4])),
                osapiNtohl(*((L7_uint32*)&pDataStart[8])),
                (pDataStart[12])?"Take":"Give");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a trace for the MACSYNC
*
* @param    event       {(input)} The event being traced
* @param    type        {(input)} The type of message
* @param    size_modid  {(input)} either the modid or number of elements depending
*                                 on the message type
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_macsync(L7_uchar8 event,
                       L7_uint32 type,
                       L7_ulong32 size_modid,
                       L7_uint32 unit)
{
  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE) &&  (uslTraceMask & USL_TRACE_MACSYNC))
  {
    if (uslMacSyncTraceMask & (1 << type))
    {
      TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
      TRACE_ENTER_2BYTE(osapiHtons(USL_MACSYNC));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
      TRACE_ENTER_BYTE(event);
      TRACE_ENTER_BYTE(0); /* Padding for alignment */
      TRACE_ENTER_BYTE(0); /* Padding for alignment */
      TRACE_ENTER_BYTE(0); /* Padding for alignment */
      TRACE_ENTER_4BYTE(osapiHtonl(type));
      TRACE_ENTER_4BYTE(osapiHtonl(size_modid));
      TRACE_ENTER_4BYTE(osapiHtonl(unit));
      TRACE_END();
    }
  }
}

/*********************************************************************
* @purpose  Format the output for a macsync event
*
* @param    traceId     {(input)} The opcode of the event
* @param    pDataStart  {(input)} The byte array of the trace
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
L7_RC_t usl_trace_macsync_format(L7_ushort16 traceId,L7_uchar8 *pDataStart)
{
  L7_uchar8   *pTypeStr;
  L7_uchar8   *pMsgStr;
  L7_uint32   type,data,unit;

  type = data = unit = 0;

  switch (pDataStart[0])
  {
    case USL_MACSYNC_HPC_SEND:
      pTypeStr = USL_MACSYNC_HPC_SEND_STR;
      break;

    case USL_MACSYNC_HPC_RECV:
      pTypeStr = USL_MACSYNC_HPC_RECV_STR;
      break;

    case USL_MACSYNC_RXQ_START:
      pTypeStr = USL_MACSYNC_RXQ_START_STR;
      break;

    case USL_MACSYNC_RXQ_COMP:
      pTypeStr = USL_MACSYNC_RXQ_COMP_STR;
      break;

    default:
      pTypeStr = "????";
      break;
  }

  type=osapiNtohl(*((L7_uint32 *)&pDataStart[4]));
  data = 0;
  switch (type)
  {
    case USL_MAC_ADDR_LEARN:
      pMsgStr = "LEARN : numElem=";
      data=osapiNtohl(*((L7_uint32 *)&pDataStart[8]));
      break;

    case USL_MAC_ADDR_AGE:
      pMsgStr = "AGE : numElem=";
      data=osapiNtohl(*((L7_uint32 *)&pDataStart[8]));
      break;

    case USL_MAC_TABLE_FLUSH:
      pMsgStr = "FLUSH";
      break;

    case USL_MAC_TABLE_UNIT_FLUSH:
      pMsgStr = "UNIT FLUSH : modid=";
      data=osapiNtohl(*((L7_uint32 *)&pDataStart[8]));
      break;

    case USL_MAC_TABLE_FORCE_RESYNC:
      pMsgStr = "RESYNC";
      break;

    case  USL_MAC_TABLE_TRUNK_FLUSH:
      pMsgStr = "TRUNK_FLUSH";
      break;
    case  USL_MAC_TABLE_WLAN_PORT_FLUSH:
      pMsgStr = "WLAN_PORT_FLUSH";
      break;
    case  USL_MAC_TABLE_PORT_FLUSH:
      pMsgStr = "PORT_FLUSH";
      break;
    case  USL_MAC_TABLE_FLUSH_ALL:
      pMsgStr = "FLUSH_ALL";
      break;
    case  USL_MAC_TABLE_SYNC_APPLICATION:
      pMsgStr = "SYNC_APPLICATION";
      break;
    case  USL_MAC_TABLE_VLAN_FLUSH:
      pMsgStr = "VLAN_FLUSH";
      break;
    case  USL_MAC_TABLE_MAC_FLUSH:
      pMsgStr = "MAC_FLUSH";
      break;
    case  USL_SYNC_VLAN_TABLE:
      pMsgStr = "SYNC VLAN";
      break;
    case USL_MAC_TABLE_AGE_TIME:
      pMsgStr = "AGE TIME";
      break;
    case  USL_SYNC_STG_TABLE:
      pMsgStr = "SYNC STG";
      break;
    case  USL_SYNC_TRUNK_TABLE:
      pMsgStr = "SYNC TRUNK";
      break;
    case  USL_SYNC_L2UCAST_STATIC_TABLE:
      pMsgStr = "SYNC L2UCAST STATIC";
      break;
    case  USL_SYNC_L2MC_TABLE:
      pMsgStr = "SYNC L2MC";
      break;
    case  USL_SYNC_L3_INTF_TABLE:
      pMsgStr = "SYNC L3INTF";
      break;
    case  USL_SYNC_L3_TUNNEL_INITIATOR_TABLE:
      pMsgStr = "SYNC L3TNNLINIT";
      break;
    case  USL_SYNC_L3_TUNNEL_TERMINATOR_TABLE:
      pMsgStr = "SYNC L3TNNLTERM";
      break;
    case  USL_SYNC_L3_HOST_TABLE:
      pMsgStr = "SYNC L3IP";
      break;
    case  USL_SYNC_L3_ROUTE_TABLE:
      pMsgStr = "SYNC DEFIP";
      break;
    case  USL_SYNC_IPMC_TABLE:
      pMsgStr = "SYNC IPMC_ROUTE";
      break;
    case  USL_SYNC_IPMC_PORT_TABLE:
      pMsgStr = "SYNC IPMC_PORT";
      break;
    case  USL_IPMC_INUSE_TIMER_POP:
      pMsgStr = "IPMC_INUSE_TIMER";
      break;
    case  USL_IPMC_INUSE:
      pMsgStr = "IPMC_INUSE";
      break;
    case  USL_SYNC_VLANIPSUBNET_TABLE:
      pMsgStr = "SYNC_IPSV";
      break;
   case  USL_SYNC_VLANMAC_TABLE:
      pMsgStr = "SYNC_MACV";
      break;
   case  USL_SYNC_PROTECTED_GROUP_TABLE:
      pMsgStr = "SYNC_PROTECTED_GRP";
      break;
   case  USL_SYNC_WLAN_VLAN_TABLE:
      pMsgStr = "SYNC_WLAN_VLAN";
      break;
   case  USL_SYNC_WLAN_PORT_TABLE:
      pMsgStr = "SYNC_WLAN_PORT";
      break;
    default:
      pMsgStr = "??????";
      break;
  }


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-9s : %-19s",pTypeStr,pMsgStr);

  if (data)
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d " ,data);

  unit = osapiNtohl(*((L7_uint32 *)&pDataStart[12]));

  if (unit)
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "unit=%d",unit);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a generic trace
*
* @param    pString  {(input)} A string to be placed in the trace buffer
*
* @returns  void
*
* @notes    The pString will be truncated at 56 characters the NULL terminator
* @end
*********************************************************************/
void usl_trace_generic(L7_uchar8 *pString)
{
  L7_uint32 i,max;
  L7_uint32 allowed_types;

  if ((uslTraceHandle != ~0) && (uslTraceMode == L7_TRUE) &&  (uslTraceMask & USL_TRACE_GENERIC))
  {
    allowed_types = usl_allowed_log_types_get();\

    if (allowed_types & USL_INFO_LOG)
    {
      printf(pString);
      printf("\n");
    }

    /* generic traces are truncated at 55 characters */
    max = strlen(pString);

    /* make sure strlen is no bigger than the max - header + null term */
    if (max > (uslTraceMaxRecordWidth - (sizeof(uslTracePtHdr_t) + 1)))
      max = (uslTraceMaxRecordWidth - (sizeof(uslTracePtHdr_t) + 1));

    TRACE_BEGIN(uslTraceHandle, L7_DRIVER_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(USL_GENERIC));
    TRACE_ENTER_4BYTE((L7_uint32)osapiTimeMillisecondsGet());

    for (i=0;i < max;i++)
    {
      TRACE_ENTER_BYTE(pString[i]);
    }

    TRACE_ENTER_BYTE(0);

    TRACE_END();
  }
}

/*********************************************************************
* @purpose  Create a generic trace
*
* @param    pString  {(input)} A string to be placed in the trace buffer
*
* @returns  void
*
* @notes    Truncated to the max size of an entry
* @end
*********************************************************************/
L7_RC_t usl_trace_generic_format(L7_ushort16 traceId,L7_uchar8 *pDataStart)
{
  L7_uint32 len;

  len = strlen(pDataStart);

  if (len > (uslTraceMaxRecordWidth - (sizeof(uslTracePtHdr_t) + 1)))
    pDataStart[uslTraceMaxRecordWidth - 1] = L7_NULL;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s",pDataStart);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the trace mask for detailed sync tracing
*
* @param    mask    {(input)} A mask of the Db's
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_sync_detail_mask_set(L7_uint32 mask)
{
  uslSyncDetailMask = mask;
}

/*********************************************************************
* @purpose  Create a trace for FP unit/slot create/delete events
*
* @param    db        {(input)} The database being synched
* @param    progress  {(input)} The point at which the database is during the sync
* @param    reason    {(input)} If the reason is an early exit, the reason
* @param    error     {(input)} The error code if the reason is error
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_sync_detail(USL_DB_ID_t db, L7_uchar8* pString)
{
  if ((uslTraceHandle != ~0)
      && (uslTraceMode == L7_TRUE)
      && (uslTraceMask & USL_TRACE_SYNC_DETAIL)
      && (uslSyncDetailMask & (1 << db)))
  {
    usl_trace_generic(pString);
  }
}

/*********************************************************************
* @purpose  Display the trace log for USL
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    unformat  {(input)} L7_TRUE if the output is to be unformatted.
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_show(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     numEnt;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(uslTraceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate USL trace block for handle "
                  "0x%8.8x\n", uslTraceHandle);
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
          if (((j % USL_TRACE_DISPLAY_CLUSTER) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

      } /* endif unformatted */
      else  /* manually format output */
      {
        uslTraceFmtFunc_t      fmtFunc = L7_NULLPTR; /* id-specific formatter  */
        L7_uchar8             *pIdStr;
        uslTracePtHdr_t        *pHdr;
        L7_BOOL                 printRecord = L7_TRUE;

        /* the first 8 bytes are the same for all Unitmgr trace entries */
        pHdr = (uslTracePtHdr_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case USL_NEW_BCOM_UNIT:
            pIdStr = (L7_uchar8 *)USL_NEW_BCOM_UNIT_STRING;
            fmtFunc = usl_trace_bcom_event_format;
            break;

          case USL_DEL_BCOM_UNIT:
            pIdStr = (L7_uchar8 *)USL_DEL_BCOM_UNIT_STRING;
            fmtFunc = usl_trace_bcom_event_format;
            break;

          case USL_NEW_FP_U_S:
            pIdStr = (L7_uchar8 *)USL_NEW_FP_U_S_STRING;
            fmtFunc = usl_trace_fp_event_format;
            break;

          case USL_DEL_FP_U_S:
            pIdStr = (L7_uchar8 *)USL_DEL_FP_U_S_STRING ;
            fmtFunc = usl_trace_fp_event_format;
            break;

          case USL_DB_INVALIDATE:
            pIdStr = (L7_uchar8 *)USL_DB_INVALIDATE_STRING ;
            fmtFunc = usl_trace_invalidate_format;
            break;

          case USL_DB_SYNC:
            pIdStr = (L7_uchar8 *)USL_DB_SYNC_STRING;
            fmtFunc = usl_trace_sync_format;
            break;

          case USL_DB_SEMA:
            pIdStr = (L7_uchar8 *)USL_DB_SEMA_STRING;
            fmtFunc = usl_trace_sema_format;
            break;

          case USL_MACSYNC:
            pIdStr = (L7_uchar8 *)USL_MACSYNC_STRING;
            fmtFunc = usl_trace_macsync_format;
            break;

          case USL_GENERIC:
            pIdStr = (L7_uchar8 *)USL_GENERIC_STRING;
            fmtFunc = usl_trace_generic_format;
            break;

          default:
            pIdStr = (L7_uchar8 *)"??????";
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
                        (char *)"ts=%8.8lu ",
                        (L7_uint32)(pHdr->timeStamp));

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

  return ;
}

/*********************************************************************
* @purpose  Enable tracing in the USL component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_mode_apply(L7_BOOL enable)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (enable == L7_TRUE)
  {
    rc = traceBlockStart(uslTraceHandle);
  }
  else
  {
    usl_trace_generic("***Stop Trace util***");
    rc = traceBlockStop(uslTraceHandle);
  }

  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "uslTraceModeApply: Unable to apply Unitmgr trace mode (%u), "
            "handle=0x%8.8x\n",enable , uslTraceHandle);
  }
  else
  {
    uslTraceMode = enable;
    if (enable)
      usl_trace_generic("***Start Trace util***");
  }
}

/*********************************************************************
* @purpose  Set the trace mask for special tracing
*
* @param    mask    {(input)} A mask of the special semaphore enablement
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_mask_set(L7_uint32 mask)
{
  uslTraceMask = mask;

  /* if MACSYNC is being traced, set the individual to all traces */
  if ((uslTraceMask & USL_TRACE_MACSYNC) && (!uslMacSyncTraceMask))
    uslMacSyncTraceMask = ~0;
}

/*********************************************************************
* @purpose  Set the trace mask for macsync special tracing
*
* @param    mask    {(input)} A mask specifying the events to trace
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_macsync_mask_set(L7_uint32 mask)
{
  uslMacSyncTraceMask = mask;
}

/*********************************************************************
* @purpose  Print the masks used by the trace utility
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void usl_trace_debug_help()
{

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n************* USL TRACING ***************\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\nCurrent mask is 0x%0.8x\n",
                uslTraceMask);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Macsync mask is 0x%0.8x\n",
                uslMacSyncTraceMask);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Sema Mask = 0x%0.8x\n",uslSemaMask);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Sync Detail Mask = 0x%0.8x\n",uslSyncDetailMask);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Tracing is %s\n",(uslTraceMode)?"Active":"Inactive");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\nThe trace masks are as follows:\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Trace Semaphores", USL_TRACE_SEMS);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Trace Sync Events",USL_TRACE_SYNC);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Trace Detailed Sync Events",USL_TRACE_SYNC_DETAIL);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Trace macsync Events", USL_TRACE_MACSYNC);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","USL Generic trace events",  USL_TRACE_GENERIC);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\nThe macsync trace masks are as follows:\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8lx\n"," ","Macsync Trace learn Event",
                1 << USL_MAC_ADDR_LEARN);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace age Event",
                1 << USL_MAC_ADDR_AGE);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table flush Event",
                1 << USL_MAC_TABLE_FLUSH);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table flush by modid Event",
                1 << USL_MAC_TABLE_UNIT_FLUSH);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table resync Event",
                1 << USL_MAC_TABLE_FORCE_RESYNC);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table HPC Send Event",
                1 << (15+USL_MACSYNC_HPC_SEND));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table HPC Recv Event",
                1 << (15+USL_MACSYNC_HPC_RECV));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table RXQ Start Event",
                1 << (15+USL_MACSYNC_RXQ_START));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s= 0x%0.8x\n"," ","Macsync Trace table RXQ Complete Event",
                1 << (15+USL_MACSYNC_RXQ_COMP));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\nDB identifiers for the Sync detailed or Sema tracing:\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_SYSTEM_DB_ID),
                1<<USL_SYSTEM_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_PORT_DB_ID),
                1<<USL_PORT_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_UCAST_DB_ID),
                1<<USL_L2_UCAST_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_MCAST_DB_ID),
                1<<USL_L2_MCAST_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_VLAN_DB_ID),
                1<<USL_L2_VLAN_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_STG_DB_ID),
                1<<USL_L2_STG_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_TRUNK_DB_ID),
                1<<USL_L2_TRUNK_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_VLAN_IPSUBNET_DB_ID),
                1<<USL_L2_VLAN_IPSUBNET_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_VLAN_MAC_DB_ID),
                1<<USL_L2_VLAN_MAC_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_DVLAN_DB_ID),
                1<<USL_L2_DVLAN_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L2_PROTECTED_GRP_DB_ID),
                1<<USL_L2_PROTECTED_GRP_DB_ID  );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_POLICY_DB_ID),
                1<<USL_POLICY_DB_ID    );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_HOST_DB_ID),
                1<<USL_L3_HOST_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_LPM_DB_ID),
                1<<USL_L3_LPM_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_EGR_NHOP_DB_ID),
                1<<USL_L3_EGR_NHOP_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_MPATH_EGR_NHOP_DB_ID),
                1<<USL_L3_MPATH_EGR_NHOP_DB_ID);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_INTF_DB_ID),
                1<<USL_L3_INTF_DB_ID );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_TUNNEL_INITIATOR_DB_ID),
                1<<USL_L3_TUNNEL_INITIATOR_DB_ID );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_L3_TUNNEL_TERMINATOR_DB_ID),
                1<<USL_L3_TUNNEL_TERMINATOR_DB_ID );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_IPMC_ROUTE_DB_ID) ,
                1<<USL_IPMC_ROUTE_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_IPMC_PORT_DB_ID) ,
                1<<USL_IPMC_PORT_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_WLAN_PORT_DB_ID) ,
                1<<USL_WLAN_PORT_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_WLAN_VLAN_DB_ID) ,
                1<<USL_WLAN_VLAN_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_METRO_VLAN_XLATE_DB_ID) ,
                1<<USL_METRO_VLAN_XLATE_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_METRO_VLAN_XLATE_EGRESS_DB_ID) ,
                1<<USL_METRO_VLAN_XLATE_EGRESS_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%-5s %-45s = 0x%0.8x\n"," ",
                usl_db_name_get(USL_METRO_VLAN_CROSSCONNECT_DB_ID) ,
                1<<USL_METRO_VLAN_CROSSCONNECT_DB_ID   );

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\nusl_trace_mask_set(mask) - masks are as defined above\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "usl_trace_show(count,firstColumnSearchString,unformat)\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "usl_trace_mode_apply(enable) - 1 to enable, 0 to disable tracing\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "usl_trace_macsync_mask_set(mask) - masks are as defined above\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "usl_trace_sema_mask_set(mask) - masks are as defined above\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "usl_trace_sync_detail_mask_set(mask) - masks are as defined above\n");

}


/*********************************************************************
* @purpose  Get the name of a db
*
* @param    dbId     {(input)}
*
* @returns  Pointer to db Name string
*
* @end
*********************************************************************/
L7_uchar8* usl_db_name_get(USL_DB_ID_t dbId)
{
  switch (dbId)
  {
    case USL_SYSTEM_DB_ID:
      return "SYSTEM_DB";

    case USL_PORT_DB_ID:
      return "PORT_DB";

    case USL_L2_UCAST_DB_ID:
      return "L2_UCAST";

    case USL_L2_MCAST_DB_ID:
      return "L2_MCAST";

    case USL_L2_VLAN_DB_ID:
      return "VLAN";

    case USL_L2_STG_DB_ID:
      return "STG";

    case USL_L2_TRUNK_DB_ID:
      return "TRUNK";

    case USL_L2_VLAN_IPSUBNET_DB_ID:
      return "VLAN_IPSUBNET";

    case USL_L2_VLAN_MAC_DB_ID:
      return "VLAN_MAC";

    case USL_L2_DVLAN_DB_ID:
      return "DVLAN";

    case USL_L2_PROTECTED_GRP_DB_ID:
      return "PROTECTED_GRP";

    case USL_POLICY_DB_ID:
      return "POLICY";

    case USL_L3_HOST_DB_ID:
      return "L3_HOST";

    case USL_L3_LPM_DB_ID:
      return "L3_LPM";

    case USL_L3_INTF_DB_ID:
      return "L3_INTF";

    case USL_L3_EGR_NHOP_DB_ID:
      return "L3_EGR_NHOP";

    case USL_L3_MPATH_EGR_NHOP_DB_ID:
      return "L3_MPATH_EGR_NHOP";

    case USL_L3_TUNNEL_INITIATOR_DB_ID:
      return "L3_TUNNEL_INITIATOR";

    case USL_L3_TUNNEL_TERMINATOR_DB_ID:
      return "L3_TUNNEL_TERMINATOR";

    case USL_IPMC_ROUTE_DB_ID:
      return "IPMC_ROUTE";

    case USL_IPMC_PORT_DB_ID:
      return "IPMC_PORT";

    case USL_WLAN_PORT_DB_ID:
      return "WLAN_PORT";

    case USL_WLAN_VLAN_DB_ID:
      return "WLAN_VLAN";

    case USL_METRO_VLAN_XLATE_DB_ID:
      return "VLAN_XLATE";

    case USL_METRO_VLAN_XLATE_EGRESS_DB_ID:
      return "VLAN_XLATE_EGRESS";

    case USL_METRO_VLAN_CROSSCONNECT_DB_ID:
      return "VLAN_CROSSCONNECT";

    default:
      break;
  }

      return "UNKNOWN";

}


/*********************************************************************
* @purpose  Get the name of a db type
*
* @param    dbType     {(input)}
*
* @returns  Pointer to db Name string
*
* @end
*********************************************************************/
L7_uchar8 *usl_db_type_name_get(USL_DB_TYPE_t dbType)
{
  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      return "OPER DB";

    case USL_SHADOW_DB:
      return "SHADOW DB";

    case USL_CURRENT_DB:
     return "CURRENT DB";

    default:
      break;
  }

  return "UNKNOWN";
}

L7_uchar8* usl_db_group_name_get(USL_DB_GROUP_t group)
{
  switch (group)
  {
    case USL_GENERIC_DB_GROUP:
      return "GENERIC_GROUP";

    case USL_L3_DB_GROUP:
      return "L3_GROUP";

    case USL_IPMC_DB_GROUP:
      return "IPMC_GROUP";

    default:
      break;
  }

      return "UNKNOWN";
  }


















