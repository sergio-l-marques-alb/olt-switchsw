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

#ifndef SR_PW2KEY_H
#define	SR_PW2KEY_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SR_UNSECURABLE

#define SR_PW2KEY_ALGORITHM_MD5 1
#ifdef SR_SHA_HASH_ALGORITHM
#define SR_PW2KEY_ALGORITHM_SHA 2
#endif /* SR_SHA_HASH_ALGORITHM */

OctetString *SrOctetStringPasswordToKey(
    SR_INT32 algorithm,
    const OctetString *password);

OctetString *SrTextPasswordToKey(
    SR_INT32 algorithm,
    const char *password);

OctetString *SrOctetStringPasswordToLocalizedKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const OctetString *password);

OctetString *SrTextPasswordToLocalizedKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const char *password);

OctetString *SrLocalizeKey(
    SR_INT32 algorithm,
    const OctetString *snmpEngineID,
    const OctetString *key);

#ifndef SR_NO_PRIVACY
OctetString *
SrTextPrivPWToLocalizedKey(SR_INT32 algorithm,
                           SR_INT32 priv_protocol,
                           const OctetString *snmpEngineID,
                           const char *password);

OctetString *
SrOctetStringPrivPWToLocalizedKey(SR_INT32 algorithm,
                                  SR_INT32 priv_protocol,
                                  const OctetString *snmpEngineID,
                                  const OctetString *password,
                                  SR_INT32 hashed);
#endif /* SR_NO_PRIVACY */

#endif /* SR_UNSECURABLE */

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_PW2KEY_H */
