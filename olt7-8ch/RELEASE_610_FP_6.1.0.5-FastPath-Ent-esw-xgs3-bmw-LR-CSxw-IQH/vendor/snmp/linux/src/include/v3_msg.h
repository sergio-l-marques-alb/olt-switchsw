/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
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

#ifndef SR_V3_MSG_H
#define SR_V3_MSG_H

#ifdef  __cplusplus
extern "C" {
#endif

extern const OID usmNoAuthProtocol;
#ifndef SR_UNSECURABLE
extern const OID usmHMACMD5AuthProtocol;
#ifdef SR_SHA_HASH_ALGORITHM
extern const OID usmHMACSHAAuthProtocol;
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

extern const OID usmNoPrivProtocol;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
extern const OID usmDESPrivProtocol;
#ifdef SR_3DES
extern const OID usm3DESPrivProtocol;
#endif /* SR_3DES */
#ifdef SR_AES
extern const OID usmAesCfb128Protocol;
extern const OID usmAESCfb128PrivProtocol;
extern const OID usmAESCfb192PrivProtocol;
extern const OID usmAESCfb256PrivProtocol;
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */


typedef struct {
    snmpEngine_t *snmpEngineData;
    snmpMPDStats_t *snmpMPDStatsData;
    usmStats_t *usmStatsData;
    usmUser_t *usmUserData;
    SnmpV2Table *usmUserTable;
    SR_UINT32 usm_salt;
    OctetString *override_userSecurityName;
#ifndef SR_UNSECURABLE
    OctetString *override_auth_secret;
    SR_INT32 override_auth_protocol;
    SR_INT32 override_auth_localized;
#ifndef SR_NO_PRIVACY
    OctetString *override_priv_secret;
    SR_INT32 override_priv_protocol;
    SR_INT32 override_priv_localized;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
} SnmpV3Lcd;

void SrParseV3SnmpMessage(
    SnmpMessage *snmp_msg,
    SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len);

int SrBuildV3SnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    SnmpLcd *sl_ptr);

SR_INT32 SrV3ProtocolOIDToInt(
    const OID *protocol_oid);

OID *SrV3AuthProtocolIntToOID(
    SR_INT32 protocol_int);

OID *SrV3PrivProtocolIntToOID(
    SR_INT32 protocol_int);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_V3_MSG_H */
