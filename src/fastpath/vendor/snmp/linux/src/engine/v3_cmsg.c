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

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include "sr_snmp.h"
#include "sr_trans.h"
#include "diag.h"
SR_FILENAME
#include "snmpv3/v3type.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_msg.h"
#include "v3_msg.h"

SnmpMessage *
SrCreateV3SnmpMessage(
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
    SR_UINT32       cache)
{
    FNAME("SrCreateV3SnmpMessage")
    SnmpMessage *snmp_msg = NULL;
    static SR_UINT32 msgID;

    snmp_msg = (SnmpMessage *)malloc(sizeof(SnmpMessage));
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "%s:  snmp_msg malloc\n", Fname));
        goto fail;
    }
    SR_CLEAR(snmp_msg);

    snmp_msg->version = SR_SNMPv3_VERSION;
    snmp_msg->u.v3.msgID = msgID++;
    snmp_msg->u.v3.securityLevel = securityLevel;
    snmp_msg->u.v3.securityModel = securityModel;
    if (cache) {
        snmp_msg->u.v3.authSnmpEngineID = CloneOctetString(authSnmpEngineID);
        if (snmp_msg->u.v3.authSnmpEngineID == NULL) {
            DPRINTF((APWARN, "%s, authSnmpEngineID malloc\n", Fname));
            goto fail;
        }
        snmp_msg->u.v3.userName = CloneOctetString(userName);
        if (snmp_msg->u.v3.userName == NULL) {
            DPRINTF((APWARN, "%s, userName malloc\n", Fname));
            goto fail;
        }
        snmp_msg->u.v3.contextSnmpEngineID =
            CloneOctetString(contextSnmpEngineID);
        if (snmp_msg->u.v3.contextSnmpEngineID == NULL) {
            DPRINTF((APWARN, "%s, contextSnmpEngineID malloc\n", Fname));
            goto fail;
        }
        if (contextName != NULL) {
            snmp_msg->u.v3.contextName = CloneOctetString(contextName);
        } else {
            snmp_msg->u.v3.contextName = MakeOctetString(NULL, 0);
        }
        if (snmp_msg->u.v3.contextName == NULL) {
            DPRINTF((APWARN, "%s, contextName malloc\n", Fname));
            goto fail;
        }
#ifndef SR_UNSECURABLE
        if (authKey != NULL) {
            snmp_msg->u.v3.authKey = CloneOctetString(authKey);
            if (snmp_msg->u.v3.authKey == NULL) {
                DPRINTF((APWARN, "%s, authKey malloc\n", Fname));
                goto fail;
            }
            snmp_msg->u.v3.authProtocol = authProtocol;
            snmp_msg->u.v3.authLocalized = authLocalized;
        }
#ifndef SR_NO_PRIVACY
        if (privKey != NULL) {
            snmp_msg->u.v3.privKey = CloneOctetString(privKey);
            if (snmp_msg->u.v3.privKey == NULL) {
                DPRINTF((APWARN, "%s, privKey malloc\n", Fname));
                goto fail;
            }
            snmp_msg->u.v3.privProtocol = privProtocol;
            snmp_msg->u.v3.privLocalized = privLocalized;
        }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    } else {
        snmp_msg->u.v3.authSnmpEngineID = authSnmpEngineID;
        snmp_msg->u.v3.userName = userName;
        snmp_msg->u.v3.contextSnmpEngineID = contextSnmpEngineID;
        snmp_msg->u.v3.contextName = contextName;
#ifndef SR_UNSECURABLE
        snmp_msg->u.v3.authKey = authKey;
        snmp_msg->u.v3.authProtocol = authProtocol;
        snmp_msg->u.v3.authLocalized = authLocalized;
#ifndef SR_NO_PRIVACY
        snmp_msg->u.v3.privKey = privKey;
        snmp_msg->u.v3.privProtocol = privProtocol;
        snmp_msg->u.v3.privLocalized = privLocalized;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }
    snmp_msg->cached = cache;
    return snmp_msg;

  fail:
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
    }
    return NULL;
}
