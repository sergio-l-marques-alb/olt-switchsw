/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_debug_api.h
*
* @purpose   isdp debug api
*
* @component isdp
*
* @comments
*
* @create    05/03/2008
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_DEBUG_API_H
#define ISDP_DEBUG_API_H

#define ISDP_USER_TRACE(__fmt__, __args__... )                                     \
            LOG_USER_TRACE(L7_ISDP_COMPONENT_ID, __fmt__,##__args__);

/*********************************************************************
*
* @purpose  Set the tracing mode for tx packets
*
* @param    txTraceMode  @b((input)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketTxModeSet(L7_uint32 txTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *txTraceMode @b((output)) Trace mode for tx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketTxModeGet(L7_uint32 *txTraceMode);

/*********************************************************************
*
* @purpose  Set the tracing mode for rx packets
*
* @param    rxTraceMode  @b((input)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketRxModeSet(L7_uint32 rxTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for tx packets
*
* @param    *rxTraceMode @b((output)) Trace mode for rx packets
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTracePacketRxModeGet(L7_uint32 *rxTraceMode);

/*********************************************************************
*
* @purpose  Set the tracing mode for events
*
* @param    eventsTraceMode  @b((input)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTraceEventsModeSet(L7_uint32 eventsTraceMode);

/*********************************************************************
*
* @purpose  Get the tracing mode for events
*
* @param    *eventsTraceMode @b((output)) Trace mode for events
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDebugTraceEventsModeGet(L7_uint32 *eventsTraceMode);

#endif /* ISDP_DEBUG_API_H*/
