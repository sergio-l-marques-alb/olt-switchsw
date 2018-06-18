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
#ifndef L7_USL_TRACE_H
#define L7_USL_TRACE_H

#include "l7_common.h"
#include "l7_usl_common.h"
#include "trace_api.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "bcmx/l2.h"

#define USL_TRACE_ENTRY_MAX         1024
#define USL_TRACE_ENTRY_SIZE_MAX    128
#define USL_TRACE_REGISTRATION_NAME "USL"
#define USL_TRACE_DISPLAY_CLUSTER   8

/*
 * Bit definitions for the trace mask 
 */
#define USL_TRACE_SEMS         ((L7_uint32)(1 << 0))
#define USL_TRACE_SYNC         ((L7_uint32)(1 << 1))
#define USL_TRACE_SYNC_DETAIL  ((L7_uint32)(1 << 2))
#define USL_TRACE_MACSYNC      ((L7_uint32)(1 << 3))
#define USL_TRACE_GENERIC      ((L7_uint32)(1 << 4))

/* TRACE Events */
#define USL_NEW_BCOM_UNIT ((L7_ushort16)0x1)
#define USL_NEW_BCOM_UNIT_STRING "bcmUnit attach>"
#define USL_DEL_BCOM_UNIT ((L7_ushort16)0x2)
#define USL_DEL_BCOM_UNIT_STRING "bcmUnit detach>"
#define USL_NEW_FP_U_S    ((L7_ushort16)0x3)
#define USL_NEW_FP_U_S_STRING    "FP(U/S) attach>"
#define USL_DEL_FP_U_S    ((L7_ushort16)0x4)
#define USL_DEL_FP_U_S_STRING    "FP(U/S) detach>"
#define USL_DB_INVALIDATE ((L7_ushort16)0x5)
#define USL_DB_INVALIDATE_STRING "DB Invalidate>" 
#define USL_DB_SYNC       ((L7_ushort16)0x6)
#define USL_DB_SYNC_STRING       "SYNC>"
#define USL_DB_SEMA       ((L7_ushort16)0x7)
#define USL_DB_SEMA_STRING       "SEMA>"      
#define USL_ERR           ((L7_ushort16)0x8) 
#define USL_ERR_STRING           "ERR>"      
#define USL_MACSYNC       ((L7_ushort16)0x9) 
#define USL_MACSYNC_STRING       "MACSYNC>"      
#define USL_GENERIC       ((L7_ushort16)0xa) 
#define USL_GENERIC_STRING       "INFO>"      


/* 
 * TRACE DB Identifiers 
 */



#define   USL_ANY_DB         ((L7_uchar8)0xFF)
#define USL_ANY_DB_NAME        "USL"

/* 
 * Progress on a Sync 
 */
#define USL_START       ((L7_uchar8)0x1)
#define USL_DB_PROGRESS_START_STRING      "Start"

#define USL_EARLY_EXIT  ((L7_uchar8)0x2)
#define USL_DB_PROGRESS_EARLY_EXIT_STRING "Exit "

#define USL_FINISHED    ((L7_uchar8)0x3)
#define USL_DB_PROGRESS_FINISHED_STRING   "Comp "

/* 
 * Reason for sync exit 
 */
#define USL_NEW_SYNC_ENQUEUED ((L7_uchar8)0x1)
#define USL_NEW_SYNC_ENQUEUED_STRING "New Sync"

#define USL_ERROR             ((L7_uchar8)0x2)
#define USL_ERROR_STRING             "Error = "

#define USL_NONE              ((L7_uchar8)0x3)
#define USL_NONE_STRING              " "

/*
 * Trace message events/strings for MACSYNC 
 */
#define USL_MACSYNC_HPC_SEND  ((L7_uchar8)0x1)
#define USL_MACSYNC_HPC_SEND_STR  "HPC_SEND"
#define USL_MACSYNC_HPC_RECV  ((L7_uchar8)0x2)
#define USL_MACSYNC_HPC_RECV_STR  "HPC_RECV"
#define USL_MACSYNC_RXQ_START ((L7_uchar8)0x3)
#define USL_MACSYNC_RXQ_START_STR "RXQ_START"
#define USL_MACSYNC_RXQ_COMP  ((L7_uchar8)0x4)
#define USL_MACSYNC_RXQ_COMP_STR  "RXQ_COMP"

/* header used by all trace points
 * NOTE: The handle and component ID fields are part of trace API function call
 */
typedef struct
{
  L7_uchar8         handle;                     /* 00   : trace buffer handle */
  L7_uchar8         compId;                     /* 01   : component id        */
  L7_ushort16       traceId;                    /* 02-03: trace id code       */
  L7_ulong32        timeStamp;                  /* 04-07: entry timestamp     */
} uslTracePtHdr_t;

/* prototype of trace id formatting functions */
typedef L7_RC_t (*uslTraceFmtFunc_t)(L7_ushort16 traceId, L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Initialize the trace utility for USL
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t usl_trace_init(L7_uint32 numEntries,L7_uint32 width);

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t usl_trace_fini(void);

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
extern void usl_trace_bcom_event(bcm_mac_t key, L7_uint32 bcm_unit, L7_BOOL attach);

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
extern L7_RC_t usl_trace_bcom_event_format(L7_ushort16 traceId, L7_uchar8 *pDataStart);

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
extern void usl_trace_fp_event(bcm_mac_t key,
                               L7_uint32 fp_unit,
                               L7_uint32 fp_slot,
                               L7_BOOL attach);

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
extern L7_RC_t usl_trace_fp_event_format(L7_ushort16 traceId, L7_uchar8 *pDataStart);

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
extern void usl_trace_invalidate(L7_BOOL starting);

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
extern L7_RC_t usl_trace_invalidate_format(L7_ushort16 traceId, L7_uchar8 *pDataStart);

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
void usl_trace_sync(USL_DB_ID_t db,L7_uchar8 progress,L7_uchar8 reason,L7_int32 error);


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
void usl_trace_sync_detail(USL_DB_ID_t db, L7_uchar8* pString);

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
extern L7_RC_t usl_trace_sync_format(L7_ushort16 traceId, L7_uchar8 *pDataStart);

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
extern void usl_trace_generic(L7_uchar8 *pString);

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
extern L7_RC_t usl_trace_generic_format(L7_ushort16 traceId,L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Display the trace log for USL
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    db        {(input)} A search string on the first column of trace
* @param    unformat  {(input)} L7_TRUE if the output is to be unformatted.  
*
* @returns  void
*
* @end
*********************************************************************/
extern void usl_trace_show(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat);

/*********************************************************************
* @purpose  Enable tracing in the USL component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
extern void usl_trace_mode_apply(L7_BOOL enable);

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
extern void usl_trace_sema(L7_uchar8 db,L7_uchar8 *sema,L7_int32 task,L7_ulong32 line,L7_BOOL take);

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
extern L7_RC_t usl_trace_sema_format(L7_ushort16 traceId,L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Create a trace for the MACSYNC
*
* @param    event       {(input)} The event being traced
* @param    type        {(input)} The type of message
* @param    size_modid  {(input)} either the modid or number of elements depending
*                                 on the message type
* @param    unit        {(input)} The unit for the message (if it is a HPC event)
*
* @returns  void
*
* @end
*********************************************************************/
extern void usl_trace_macsync(L7_uchar8 event,
                       L7_uint32 type,
                       L7_ulong32 size_modid,
                       L7_uint32 unit);

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
extern L7_RC_t usl_trace_macsync_format(L7_ushort16 traceId,L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Get the name of a db
*
* @param    dbId     {(input)} 
*
* @returns  Pointer to db Name string
*
* @end
*********************************************************************/
extern L7_uchar8* usl_db_name_get(USL_DB_ID_t dbId);


/*********************************************************************
* @purpose  Get the name of a db type
*
* @param    dbType     {(input)} 
*
* @returns  Pointer to db Name string
*
* @end
*********************************************************************/
extern L7_uchar8 *usl_db_type_name_get(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Get the name of a db Group
*
* @param    Group     {(input)} 
*
* @returns  Pointer to db Group Name string
*
* @end
*********************************************************************/
extern L7_uchar8* usl_db_group_name_get(USL_DB_GROUP_t group);

#endif
