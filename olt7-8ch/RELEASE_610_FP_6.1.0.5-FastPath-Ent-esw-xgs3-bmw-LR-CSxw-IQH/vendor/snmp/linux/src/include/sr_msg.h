/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#ifndef SR_MSG_H
#define SR_MSG_H

#ifdef  __cplusplus
extern "C" {
#endif

#define SET_MSG_ERR_CODE(m,c) \
    if ((m)->error_code == 0) (m)->error_code = (c)

typedef struct SR_SnmpLcd SnmpLcd;

typedef void (*SrParseSnmpMessageFunction)(
    SnmpMessage *snmp_msg,
    struct SR_SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len);

typedef int (*SrBuildSnmpMessageFunction)(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    struct SR_SnmpLcd *sl_ptr);

typedef SR_INT32 (*SrValidateTransportAddressFunction)(
    void *vta_data,
    TransportInfo *srcTI,
    OctetString *selector);

struct SR_SnmpLcd {
    SR_INT32 num_vers;
    SR_INT32 versions_supported[4];
    void *lcds[4];
    SrParseSnmpMessageFunction parse_functions[4];
    SrBuildSnmpMessageFunction build_functions[4];
    void *vta_data;
    SrValidateTransportAddressFunction vta_function;
};

SnmpMessage *SrParseSnmpMessage(
    SnmpLcd *sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len);

int SrBuildSnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    SnmpLcd *sl_ptr);

void SrFreeSnmpMessage(
    SnmpMessage *snmp_msg);

void *SrGetVersionLcd(
    SnmpLcd *sl_ptr,
    SR_INT32 version);

SnmpMessage *SrCreateV1SnmpMessage(
    OctetString *community,
    int cache);

SnmpMessage *SrCreateV2cSnmpMessage(
    OctetString *community,
    int cache);

SnmpMessage *SrCreateV3SnmpMessage(
    SR_UINT32       securityLevel,
    SR_UINT32       securityModel,
    OctetString    *authSnmpEngineID,
    OctetString    *userName,
    OctetString    *contextSnmpEngineID,
    OctetString    *contextName,
#ifndef SR_UNSECURABLE
    OctetString    *authKey,
    SR_INT32        authProtocol,
    SR_INT32        authLocalized,
#ifndef SR_NO_PRIVACY
    OctetString    *privKey,
    SR_INT32        privProtocol,
    SR_INT32        privLocalized,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    SR_UINT32       cache);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_MSG_H */
