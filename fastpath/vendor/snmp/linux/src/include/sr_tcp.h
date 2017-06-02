/*
 *
 * Copyright (C) 1996-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 *  This file defines constants and prototypes for the TCP transport APIs.
 */

#ifndef SR_TCP_H
#define SR_TCP_H

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  Define the default queue length for the ListenTcpTransport() API.
 */
#define SR_TCP_DEFAULT_QLEN 5

/*
 *  Define the default connect timeout in centi-seconds.
 */
#define SR_TO_TCP_CONNECT_TIMEOUT_DEFAULT    200


/*
 *  Define the TransportOption types.
 */

/*
 ****************************************************************************
 *  TransportOptions for OpenTcpTransport().
 ****************************************************************************
 */

/* --- Do not enable the TCP_NODELAY socket option --- */
#define SR_TO_TCP_DELAY            1

/* --- Set the socket to blocking --- */
#define SR_TO_TCP_BLOCKING         2

/* --- Bind within the OpenTcpTransport() API --- */
#define SR_TO_TCP_OPEN_BIND        3

/* --- Connect within the OpenTcpTransport() API --- */
#define SR_TO_TCP_OPEN_CONNECT     4

/* --- Bind/listen within the OpenTcpTransport() API --- */
#define SR_TO_TCP_OPEN_LISTEN      5



/*
 ****************************************************************************
 *  TransportOptions for SendTcpTransport() and ReceiveTcpTransport().
 ****************************************************************************
 */

/* --- For debugging, render data sent or received --- */
#define SR_TO_TCP_RENDER_DATA      6


/*
 ****************************************************************************
 *  TransportOptions for BindTcpTransport().
 ****************************************************************************
 */

/* --- Specify the number of retries for the BindTcpTransport() API --- */
#define SR_TO_TCP_BIND_TRIES         7

/* --- Specify a string containing the port ranges --- */
#define SR_TO_TCP_BIND_PORT_RANGES   8


/*
 ****************************************************************************
 *  TransportOptions for ListenTcpTransport().
 ****************************************************************************
 */

/* --- Specify the listen queue length --- */
#define SR_TO_TCP_LISTEN_QLEN       9


/*
 ****************************************************************************
 *  TransportOptions for ConnectTcpTransport().
 ****************************************************************************
 */

/* --- Specify the connect timeout in centiseconds --- */
#define SR_TO_TCP_CONNECT_TIMEOUT     10





/*
 *  The TransportOptions structure is used to pass additional options to
 *  the OpenTcpTransport API call.  The caller should pass an array of
 *  these structures, with the type field of the last member equal to -1.
 */
typedef struct _TransportOptions {
    SR_INT32   type;
    void       *value;
} TransportOptions;


/*
 *  Public APIs.
 */
int   OpenTcpTransport(
          TransportInfo *ti,
          const TransportOptions *to);

void  CloseTcpTransport(TransportInfo *ti);

int   SendToTcpTransport(
          const char *data, 
          const int  len, 
          const TransportInfo *ti,
          const TransportOptions *to);

int   ReceiveFromTcpTransport(
          char *data, 
          const int  len, 
          TransportInfo *ti,
          const TransportOptions *to);

int   ConnectTcpTransport(
          const TransportInfo *ti,
          const TransportOptions *to);

int   BindTcpTransport(
          TransportInfo *ti,
          const TransportOptions *to);

int   ListenTcpTransport(
          TransportInfo *ti,
          const TransportOptions *to);

int   AcceptTcpTransport(
          const TransportInfo *listen_ti,
          TransportInfo **ti,
          const TransportOptions *to);




/*
 *  Public APIs in t_tcpcmn.c.  These APIs are common regardless of the
 *  underlying implementation of the Open/Close/Send/Receive/Connect/Bind/etc.
 *  APIs.
 */
void CopyTcpTransportInfo(TransportInfo *dst, const TransportInfo *src);

void CopyTcpTransportAddr(TransportInfo *dst, const TransportInfo *src);

int CmpTcpTransportInfo(const TransportInfo *ti1, const TransportInfo *ti2);

int CmpTcpTransportInfoWithMask(
    const TransportInfo *ti1,
    const TransportInfo *ti2,
    const TransportInfo *mask);
 
OctetString *TcpTransportInfoToOctetString(const TransportInfo *ti);

int OctetStringToTcpTransportInfo(TransportInfo *ti, const OctetString *os);

int TMaskOctetStringToTcpTransportInfo(
    TransportInfo *ti, 
    const OctetString *os);

char *FormatTcpTransportString(char *buf, int len, const TransportInfo *ti);

#ifdef  __cplusplus
}
#endif

#endif	/* SR_TCP_H */
