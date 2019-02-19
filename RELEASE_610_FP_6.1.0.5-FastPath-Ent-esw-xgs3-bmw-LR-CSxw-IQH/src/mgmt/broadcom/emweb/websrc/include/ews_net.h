/*
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server application interface to network transport layer
 *
 */
#ifndef _EWS_NET_H_
#define _EWS_NET_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"

/******************************************************************************
 *
 * In a typical TCP/IP implementation, the application is responsible for
 * listening to the HTTP TCP port (80) for connection requests.  When a
 * connection request is received, the application accepts the connection
 * on behalf of EmWeb/Server and invoke ewsNetHTTPStart() to inform the
 * EmWeb/Server of the new request.
 *
 * EmWeb/Server assumes that the application maintains data buffers for
 * the reception and transmission of TCP data.  The only requirements that
 * EmWeb/Server imposes on the buffer implementation is as follows:
 *
 *   1. Buffers can be uniquely identified by a buffer descriptor.  No
 *      assumptions are made about the actual structure of the buffer
 *      descriptors or their relationship to data.  For example, a buffer
 *      descriptor could be an index into a table, a pointer to a
 *      structure (either contiguous or seperate from the data represented),
 *      etc.  The application is responsible for defining the appropriate
 *      type for EwaNetBuffer and value for EWA_NET_BUFFER_NULL.
 *
 *   2. Buffers can be chained together.  Given a buffer descriptor,
 *      EmWeb/Server must be able to get or set the "next buffer in the chain"
 *      field.  This is done by ewaNetBufferNextSet() and
 *      ewaNetBufferNextGet().  Note that the buffer chain is terminated when
 *      the next buffer value is EWA_NET_BUFFER_NULL.
 *
 *   3. Given a buffer descriptor, EmWeb/Server can determine the start of
 *      data in the buffer.  Additionally, EmWeb/Server may change the start
 *      of data in the buffer (EmWeb/Server only changes the start of data in
 *      the buffer upward). This is done by ewaNetBufferDataGet() and
 *      ewaNetBufferDataSet().
 *
 *   4. Given a buffer descriptor, EmWeb/Server can determine the size of
 *      contiguous data in the buffer.  Additionally, EmWeb/Server may
 *      change the size of the buffer (EmWeb/Server only changes the
 *      size of the buffer downward).  This is done by ewaNetBufferLengthGet()
 *      and ewaNetBufferLengthSet().
 *
 *   5. EmWeb/Server may allocate a buffer by invoking ewaNetBufferAlloc().
 *      If no buffers are available, this function returns EWA_NET_BUFFER_NULL.
 *      Additionally, EmWeb/Server may release a buffer by invoking
 *      ewaNetBufferFree().
 *
 * As the application receives TCP data on an HTTP connection, it passes
 * this data to the EmWeb/Server by invoking ewsNetHTTPReceive().
 *
 * The EmWeb/Server transmits TCP data on an HTTP connection by invoking
 * ewaNetHTTPSend().  The application may throttle EmWeb/Server by
 * returning EWA_STATUS_OK_YIELD.  This causes the EmWeb/Server to
 * save state and return control to the application.  The application must
 * invoke ewsRun() to give the EmWeb/Server an opportunity to continue
 * processing the request.
 *
 * When the EmWeb/Server completes a request, it invokes ewaNetHTTPEnd().
 *
 * The application may abort a request at any time by invoking
 * ewsNetHTTPAbort().
 *
 *****************************************************************************/

/*
 * ewsNetHTTPStart
 * Start a new HTTP request
 *
 * net_handle   - application-specific handle representing request
 *
 * Returns context for the request, or EWS_CONTEXT_NULL on failure.
 */
extern EwsContext ewsNetHTTPStart ( EwaNetHandle net_handle );

/*
 * ewsNetHTTPAbort
 * Abort a previously started HTTP request.
 *
 * context      - context of request to be aborted
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 *
 * Note that any output queued within the Server context that has not yet
 * been sent to the network application may be lost.  This is the right thing
 * to do in most cases; the network application uses ewsNetHTTPAbort to
 * indicate that communication with the client has been lost.
 *
 * But in cases where the network application wants to "gracefully shut down"
 * a connection and not lose any pending output, it should call ewsNetEOF.
 * That will cause systematic termination of any uncompleted requests and
 * flushing of all unsent output.
 *
 * (Within the Server, if code wishes to terminate a context without losing
 * unsent output, it should first call ewsFlushAll on the context, then
 * ewsNetHTTPAbort.)
 */
extern EwsStatus ewsNetHTTPAbort ( EwsContext context );

/*
 * ewsNetHTTPReceive
 * Receive request data from the network.
 *
 * context      - context of request to which received data applies
 * buffer       - buffer containing received data
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 */
extern EwsStatus ewsNetHTTPReceive ( EwsContext context, EwaNetBuffer buffer );

#ifdef EW_CONFIG_OPTION_SCHED_FC

/*
 * ewsNetFlowControl
 * Mark context for flow control to avoid predicted congestion.  ewsRun() will
 * place the context on the suspended list at the next opportunity and
 * continue by processing additional requests.
 *
 * context      - context of request to be flow controlled
 *
 * Returns EWS_STATUS_OK.
 */
EwsStatus ewsNetFlowControl ( EwsContext context ) ;

/*
 * ewsNetUnFlowControl
 * Resume previously flow controlled context
 *
 * context      - context of request to be flow controlled
 *
 * Returns EWS_STATUS_OK.
 */
EwsStatus ewsNetUnFlowControl ( EwsContext context );

#endif /* EW_CONFIG_OPTION_SCHED_FC */

#ifndef ewaNetHTTPSend
/*
 * ewaNetHTTPSend (Application)
 * This function must be provided by the application to accept data from
 * EmWeb/Server for transmission to a browser in response to a request.
 *
 * net_handle   - application-specific request handle from ewsNetHTTPStart()
 * buffer       - buffer containing data to be transmitted
 *
 * Returns EWA_STATUS_OK on success, EWA_STATUS_OK_YIELD on success and
 * request EmWeb/Server to yield CPU to application, or EWA_STATUS_ERROR on
 * failure.
 */
extern EwaStatus ewaNetHTTPSend
  ( EwaNetHandle net_handle, EwaNetBuffer buffer );
#endif

#ifndef ewaNetHTTPEnd
/*
 * ewaNetHTTPEnd (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to indicate the completion of a request after all response
 * data has been sent.
 *
 * Note that all data previously sent to this connection (via ewaNetHTTPSend,
 * etc.) must be sent over the network, even if the connection has been
 * flow-controlled and the network application has had to buffer the data.
 *
 * net_handle   - application-specific request handle from ewsNetHTTPStart()
 *
 * Returns EWA_STATUS_OK on success, EWA_STATUS_OK_YIELD on success and
 * request EmWeb/Server to yield CPU to application, or EWA_STATUS_ERROR on
 * failure.
 */
extern EwaStatus ewaNetHTTPEnd ( EwaNetHandle net_handle );
#endif

#ifndef ewaNetBufferAlloc
/*
 * ewaNetBufferAlloc (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to request a network buffer to be used for sending data.
 *
 * Returns a buffer, or EWA_NET_BUFFER_NULL on failure. The length of the
 * buffer must be initialized to the number of bytes available for use
 * by EmWeb/Server.
 */
extern EwaNetBuffer ewaNetBufferAlloc ( void );
#endif

#ifndef ewaNetBufferFree
/*
 * ewaNetBufferFree (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to release one or a chain of network buffers (either from
 * ewsNetHTTPReceive() or ewaNetBufferAlloc()).
 *
 * buffer       - buffer(s) to be released
 *
 * No return value
 */
extern void ewaNetBufferFree ( EwaNetBuffer buffer );
#endif

#ifndef ewaNetBufferFreeCheck
/*
 * ewaNetBufferFreeCheck (Application)
 * This function must be provided by the application.  It is used prior to 
 * calling ewaNetBufferFree() ro ensure that the buffer has not already been freed. 
 *
 * buffer       - buffer(s) to be checked
 *
 * Returns TRUE is it is ok to free the buffer, FALSE otherwise
 */
extern boolean ewaNetBufferFreeCheck ( EwaNetBuffer buffer );
#endif

#ifndef ewaNetBufferLengthGet
/*
 * ewaNetBufferLengthGet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to get the length of the data portion of this buffer fragment.
 *
 * buffer       - buffer descriptor
 *
 * Returns length of buffer
 */
uintf ewaNetBufferLengthGet ( EwaNetBuffer buffer );
#endif

#ifndef ewaNetBufferLengthSet
/*
 * ewaNetBufferLengthSet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to set the length of the data portion of this buffer fragment.
 * This function is only used to decrease the original length of a
 * buffer.  EmWeb/Server never increases the length of a buffer.
 *
 * buffer       - buffer descriptor
 * length       - new length value
 *
 * No return value
 */
void ewaNetBufferLengthSet ( EwaNetBuffer buffer, uintf length );
#endif

#ifndef ewaNetBufferDataGet
/*
 * ewaNetBufferDataGet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to get the pointer to the data contained in the buffer.
 *
 * buffer       - buffer descriptor
 *
 * Returns pointer to data in buffer
 */
uint8 * ewaNetBufferDataGet ( EwaNetBuffer buffer );
#endif

#ifndef ewaNetBufferDataSet
/*
 * ewaNetBufferDataSet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to set the pointer to the data contained in the buffer.
 * This function is only used to advance the start of data forward.
 * EmWeb/Server never moves this pointer backward.
 *
 * buffer       - buffer descriptor
 * datap        - new start of data value
 *
 * No return value
 */
void ewaNetBufferDataSet ( EwaNetBuffer buffer, uint8 *datap );
#endif

#ifndef ewaNetBufferNextGet
/*
 * ewaNetBufferNextGet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to get the next buffer descriptor in the buffer chain.
 *
 * buffer       - buffer descriptor
 *
 * Returns next buffer descriptor, or EWA_NET_BUFFER_NULL if end of chain.
 */
EwaNetBuffer ewaNetBufferNextGet ( EwaNetBuffer buffer );
#endif

#ifndef ewaNetBufferNextSet
/*
 * ewaNetBufferNextSet (Application)
 * This function must be provided by the application.  It is invoked by
 * EmWeb/Server to set the next buffer descriptor in the buffer chain.
 *
 * buffer       - buffer descriptor
 * next         - next buffer descriptor to be attached to buffer
 *
 * No return value
 */
void ewaNetBufferNextSet ( EwaNetBuffer buffer, EwaNetBuffer next );
#endif

#ifndef ewaNetLocalHostName
/*
 * ewaNetLocalHostName (Application)
 * This function must be provided by the application.  It is invoked by the
 * EmWeb/Server to build proper redirection (Location:) headers.  This can
 * be either a dotted IP address or a fully qualified hostname.
 */
const char * ewaNetLocalHostName ( EwsContext context );
#endif

#ifndef ewaNetHTTPCleanup
/*
 * ewaNetHTTPCleanup
 * This function must be provided by the application or defined as a
 * empty macro.  It is invoked by EmWeb/Server when a request completes,
 * allowing the application to reset any processing state stored in the
 * network handle.   Note that for HTTP 1.1 persistent connections, this
 * routine may be called several times for the same connection, as one
 * connection can be used for multiple requests.  For the last request
 * on a connection, this routine will be called before ewaNetHTTPEnd is
 * invoked.
 *
 * Returns void.   Note that the request context is undefined during
 * this call and cannot be accessed.
 */
void ewaNetHTTPCleanup( EwaNetHandle handle );
#endif

#ifdef EW_CONFIG_OPTION_SSL_RSA
/*
 * ewsNetSSLStart
 * Start a new HTTP request over SSL
 *
 * net_handle   - application-specific handle representing request
 *
 * Returns context for the request, or EWS_CONTEXT_NULL on failure.
 */
extern EwsContext ewsNetSSLStart ( EwaNetHandle net_handle );
#endif /* EW_CONFIG_OPTION_SSL_RSA */

#ifdef EW_CONFIG_OPTION_UPNP

/*****************************************************************
 * Define networking configuration for use by UPNP.
 * (The user probably does not want to modify this.)
 ***************************************************************** */
#ifndef EWU_MAX_PKT_SIZE
#define EWU_MAX_PKT_SIZE  512  /* max packet size of a UDP datagram */
#endif

/*
 * Network interfaces that are specific to EmWeb/UPnP.
 */

/* The following function (ewsNetEOF)
 * could be available when using only HTTP/TCP, but the Server has no need
 * for them in that mode, so we only define them when using HTTP/U/MU.
 */

/*
 * ewsNetEOF
 * Indicate the end of the connection's input stream without aborting the
 * connection.
 *
 * context      - context of request to end received data
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 */
extern EwsStatus ewsNetEOF ( EwsContext context );

/*
 * EWA_NET_HANDLE_DISCARD
 *
 * A special NetHandle value.  Output sent to this NetHandle is discarded.
 *
 * It is defined in ew_config.h.
 */

#ifndef ewaNetUdpOutStart
/*
 * ewaNetUdpOutStart
 *
 */
extern EwaNetHandle ewaNetUdpOutStart(EwsContext context);
#endif /* ewaNetUdpOutStart */

#ifndef ewaNetUdpSendMsg
/*
 * ewaNetUdpSendMsg
 * Send the netHandle's udp_xmit_buffer out the network over udp.
 *
 *
 */
extern EwaStatus ewaNetUdpSendMsg(EwaNetHandle handle);
#endif /* ewaNetUdpSendMsg */


/*
 * SSDP
 * ====
 */

typedef enum EwuSsdpStatus_e {
  ewuSsdpIPWaiting = 0,
  ewuSsdpNoDevices,
  ewuSsdpNoResources,
  ewuSsdpResourcesWaiting,
  ewuSsdpUPnPEnabledOK,
  ewuSsdpUPnPDisabledOK,
  ewuSsdpUPnPFailed
} EwuSsdpStatus;

/*
 * ewuSsdpDeviceIPAddress()
 *
 *
 * IP Address availability:
 *
 *   IPAddress_valid = FALSE   - IP address was lost
 *                   = TRUE    - IP address was gained
 */
extern void ewuSsdpDeviceIPAddress(boolean IPaddress_valid);

/*
 * ewsNetUdpStart()
 * Start a new SSDP/HTTP request
 *
 * net_handle   - application-specific handle representing request
 *
 * Returns context for the request, or EWS_CONTEXT_NULL on failure.
 */
extern EwsContext ewsNetUdpStart ( EwaNetHandle net_handle );

/*
 * ewsNetSSDPResponseStart()
 * Start a new SSDP/HTTP response
 *
 * net_handle - application-specific handle representing request
 *
 * Returns context for request, or EWS_CONTEXT_NULL on failure.
 */
extern EwsContext ewsNetSSDPResponseStart ( EwaNetHandle handle );

/*
 * Declarations of callbacks to the application.
 */

/* Obtain from the application a globally unique identification of this
 * device.  (Often the ASCII representation of the MAC address of the
 * network interface.)
 */
extern const char *ewaUPnPSystemId( void );

#endif /* EW_CONFIG_OPTION_UPNP */

#endif /* _EWS_NET_H_ */
