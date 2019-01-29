/*
 *
 * Copyright (C) 1996-2002 by SNMP Research, Incorporated.
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

#ifndef SR_SR_IPX_H
#define SR_SR_IPX_H

#ifdef  __cplusplus
extern "C" {
#endif

extern void CopyIpxTransportInfo(TransportInfo *dst, const TransportInfo *src);

extern void CopyIpxTransportAddr(TransportInfo *dst, const TransportInfo *src);

extern int CmpIpxTransportInfo(
    const TransportInfo *ti1,
    const TransportInfo *ti2);

extern int CmpIpxTransportInfoWithMask(
    const TransportInfo *ti1,
    const TransportInfo *ti2,
    const TransportInfo *mask);
 
extern OctetString *IpxTransportInfoToOctetString(const TransportInfo *ti);

extern int OctetStringToIpxTransportInfo(
    TransportInfo *ti,
    const OctetString *os);

extern int TMaskOctetStringToIpxTransportInfo(
    TransportInfo *ti,
    const OctetString *os);

extern int SendToIpxTransport(
    const char *data,
    int len,
    const TransportInfo *ti);

extern int ReceiveFromIpxTransport(char *data, int len, TransportInfo *ti);

extern char *FormatIpxTransportString(
    char *buf,
    int len,
    const TransportInfo *ti);

int OpenIpxTransport(TransportInfo *ti);

int BindIpxTransport(TransportInfo *ti, int tries);

void CloseIpxTransport(TransportInfo *ti);

SR_INT32 ConvToken_IPX_TAddr(SR_INT32 direction, char **token, void *value);

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_IPX_H */
