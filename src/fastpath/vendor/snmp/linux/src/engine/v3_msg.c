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



/* needed by netinet/in.h on freebsd */
#include <sys/types.h>

#include <netinet/in.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "diag.h"
SR_FILENAME
#include "snmpv3/v3type.h"
#include "lookup.h"
#include "v2table.h"
#ifdef SR_DEBUG
#include "scan.h"
#endif /* SR_DEBUG */
#include "oid_lib.h"
#include "sr_boots.h"
#include "sr_msg.h"
#include "v3_msg.h"
#include "sr_vta.h"
#include "snmpv3/v3defs.h"	/* for D_usmUserStatus_active definition */
#ifndef SR_UNSECURABLE
#include "sr_auth.h"
#include "pw2key.h"
#ifndef SR_NO_PRIVACY
#include "sr_priv.h"
#ifdef SR_AES
#include "aes.h"
#include "sr_aes.h"
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
static SR_INT32 SrAssign3DESKeys(SnmpMessage *snmp_msg, usmUserEntry_t *uue);
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */

#ifndef SR_UNSECURABLE
#define SEND_TRAP 1
int inform_type = FALSE;
#endif /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE
/* 96 bits = 12 octets */
#define SR_HMAC_96_DIGEST_LENGTH 12
#endif /* SR_UNSECURABLE */

static int SrLookupSnmpEngineBootsAndTime(
    SnmpV3Lcd *snmp_v3_lcd,
    OctetString *snmpEngineID,
    SR_INT32 *snmpEngineBoots,
    SR_INT32 *snmpEngineTime,
    SR_INT32 *lastReceived_snmpEngineTime);

static int
SrLookupSnmpEngineBootsAndTime(
    SnmpV3Lcd *snmp_v3_lcd,
    OctetString *snmpEngineID,
    SR_INT32 *snmpEngineBoots,
    SR_INT32 *snmpEngineTime,
    SR_INT32 *lastReceived_snmpEngineTime)
{

    SR_INT32 curtime;
    *snmpEngineBoots = 0L;
    *snmpEngineTime = 0L;

    if (!CmpOctetStrings(snmpEngineID,
                         snmp_v3_lcd->snmpEngineData->snmpEngineID)) {
        GetTimeNowInSeconds(&curtime, NULL);
        *snmpEngineBoots = snmp_v3_lcd->snmpEngineData->snmpEngineBoots;
        *snmpEngineTime = curtime;
        *lastReceived_snmpEngineTime = curtime;
        return 1;
    } else {
        LookupBootsAndTimeEntry(snmpEngineID,
                                snmpEngineBoots,
                                snmpEngineTime,
                                lastReceived_snmpEngineTime);
        return 0;
    }
}

void
SrParseV3SnmpMessage(
    SnmpMessage *snmp_msg,
    SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len)
{
    FNAME("SrParseV3SnmpMessage")
    short type;
    SR_UINT32 msgFlags;
    OctetString *msgFlags_os = NULL;
    SR_INT32 length;
    SR_INT32 seq_length;
    unsigned char *end_ptr, *new_end_ptr;
    OctetString *authParameters = NULL;
    OctetString *privParameters = NULL;
    unsigned char *digest_ptr;
    SnmpV3Lcd *snmp_v3_lcd = (SnmpV3Lcd *)v_sl_ptr;
    int index;
    usmUserEntry_t *uue, uue_s;
    unsigned char hmac_digest[32];
    SR_INT32 snmpEngineBoots;
    SR_INT32 snmpEngineTime;
    SR_INT32 lastReceived_snmpEngineTime;
#ifndef SR_UNSECURABLE
    OctetString *lk = NULL;
#ifndef SR_NO_PRIVACY
#if (defined(SR_3DES) || defined(SR_AES))
    OctetString *lk2 = NULL;
    SR_INT32 remainderLen, status;
    unsigned char *tmp_ptr;
#endif	/* (defined(SR_3DES) || defined(SR_AES)) */
#ifdef SR_AES
    unsigned char iv[16];
    SR_INT32 engTime;
    SR_INT32 engBoots;
#endif /* SR_AES */
    int decrypted = 0;
    SR_INT32 encrypt_length;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    seq_length = ParseSequence(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: bad sequence tag.\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

/* ought to do some checking on seq_length here, and below */

    snmp_msg->u.v3.msgID = ParseInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: msgID:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    /* if (snmp_msg->u.v3.msgID < 0 || snmp_msg->u.v3.msgID > 2147483647) */
    /* snmp_msg->u.v3.msgID is an unsigned int so it is always >= 0 */
    if (snmp_msg->u.v3.msgID > 2147483647)  {
        DPRINTF((APPACKET, "%s: msgID out of range:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }


    snmp_msg->u.v3.mms = ParseInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: mms:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if (snmp_msg->u.v3.mms < 484 || snmp_msg->u.v3.mms > 2147483647)  {
        DPRINTF((APPACKET, "%s: mms out of range:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }


    msgFlags_os = ParseOctetString(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == INTEGER_TYPE) {
        DPRINTF((APWARN, "%s: old style v3 packet received.\n", Fname));
    }
    if (msgFlags_os == NULL) {
        DPRINTF((APPACKET, "%s: msgFlags\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if (msgFlags_os->length != 1) {
        DPRINTF((APPACKET, "%s: msgFlags, bad length\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        FreeOctetString(msgFlags_os);
        msgFlags_os = NULL;
        goto fail;
    }
    msgFlags = (SR_UINT32)msgFlags_os->octet_ptr[0];
    FreeOctetString(msgFlags_os);
    msgFlags_os = NULL;
    snmp_msg->u.v3.reportableFlag = (msgFlags & 0x4) >> 2;
    switch (msgFlags & 0x03) {
        case SR_SECURITY_LEVEL_BITS_NOAUTH_NOPRIV:
            snmp_msg->u.v3.securityLevel = SR_SECURITY_LEVEL_NOAUTH;
            break;
#ifndef SR_UNSECURABLE
        case SR_SECURITY_LEVEL_BITS_AUTH_NOPRIV:
            snmp_msg->u.v3.securityLevel = SR_SECURITY_LEVEL_AUTH;
            break;
#ifndef SR_NO_PRIVACY
        case SR_SECURITY_LEVEL_BITS_AUTH_PRIV:
            snmp_msg->u.v3.securityLevel = SR_SECURITY_LEVEL_PRIV;
            break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
        default:
            SET_MSG_ERR_CODE(snmp_msg, SR_SNMP_INVALID_MSGS);
    }

    /* extract securityModel */
    snmp_msg->u.v3.securityModel =
        ParseInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: securityModel\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if (snmp_msg->u.v3.securityModel < 1 || snmp_msg->u.v3.securityModel > 2147483647 )  {
        DPRINTF((APPACKET, "%s: securityModel out of range\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if (snmp_msg->u.v3.securityModel != 3) {
        SET_MSG_ERR_CODE(snmp_msg, SR_SNMP_UNKNOWN_SECURITY_MODELS);
    }

/* ought to do some checking on seq_length here, and above */

    /* parse securityParameters tag/length and sequence tag/length */
    if (ParseType(&snmp_msg->wptr, snmp_msg->end_ptr) != OCTET_PRIM_TYPE) {
        DPRINTF((APPACKET, "%s: securityParameters bad tag\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if ((length = ParseLength(&snmp_msg->wptr, snmp_msg->end_ptr)) == -1) {
        DPRINTF((APPACKET, "%s: length error.\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    end_ptr = (unsigned char *)snmp_msg->wptr + length;
    if (snmp_msg->u.v3.securityModel != 3) {
        /* This just skips over the securityParameters, since they are in
         * an unknown format */
        snmp_msg->wptr += length;
        goto done;
    }
    if (length == 0) {
        DPRINTF((APPACKET, "%s: bad securityParameters.\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    seq_length = ParseSequence(&snmp_msg->wptr, end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: bad sequence tag.\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    new_end_ptr = (unsigned char *)snmp_msg->wptr + seq_length;
    if (new_end_ptr < end_ptr) {
        end_ptr = new_end_ptr;
    } else if (new_end_ptr > end_ptr) {
        DPRINTF((APPACKET, "%s: bad securityParameters BER length:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Extract authSnmpEngineID */
    snmp_msg->u.v3.authSnmpEngineID =
        ParseOctetString(&snmp_msg->wptr, end_ptr, &type);
    if (snmp_msg->u.v3.authSnmpEngineID == NULL) {
        DPRINTF((APPACKET, "%s: bad authSnmpEngineID\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Extract authSnmpEngineBoots */
    snmp_msg->u.v3.authSnmpEngineBoots =
        ParseInt(&snmp_msg->wptr, end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: bad authSnmpEngineBoots\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Extract authSnmpEngineTime */
    snmp_msg->u.v3.authSnmpEngineTime =
        ParseInt(&snmp_msg->wptr, end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: bad authSnmpEngineTime\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Extract userName */
    snmp_msg->u.v3.userName = ParseOctetString(&snmp_msg->wptr, end_ptr, &type);
    if (snmp_msg->u.v3.userName == NULL) {
        DPRINTF((APPACKET, "%s: bad userName\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
    if (snmp_msg->u.v3.userName->length > 32) {
        DPRINTF((APPACKET, "%s: userName exceeds maximum size\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Extract digest (this is the authParameters) */
    authParameters = ParseOctetString(&snmp_msg->wptr, end_ptr, &type);
    if (authParameters == NULL) {
        DPRINTF((APPACKET, "%s: bad digest\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }
#ifndef SR_UNSECURABLE
    digest_ptr = (unsigned char *)snmp_msg->wptr - authParameters->length;
#endif /* SR_UNSECURABLE */

    /* Extract priv parameters */
    privParameters = ParseOctetString(&snmp_msg->wptr, end_ptr, &type);
    if (privParameters == NULL) {
        DPRINTF((APPACKET, "%s: bad priv parameters\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* Make sure lengths match up */
    if (snmp_msg->wptr != end_ptr) {
        DPRINTF((APPACKET, "%s: bad BER length in securityParameters\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto fail;
    }

    /* If we got a SR_SNMP_UNKNOWN_SECURITY_MODELS error, we do not need
     * to continue parsing */
    if (snmp_msg->error_code) {
        goto done;
    }

    /* If the reportableFlag is set, the authSnmpEngineID must be equal
     * to the local snmpEngineID */
    if (snmp_msg->u.v3.reportableFlag) {
        if (CmpOctetStrings(snmp_msg->u.v3.authSnmpEngineID,
                            snmp_v3_lcd->snmpEngineData->snmpEngineID)) {
            snmp_msg->error_code = SR_USM_UNKNOWN_ENGINE_IDS;
            FreeOctetString(snmp_msg->u.v3.authSnmpEngineID);
            snmp_msg->u.v3.authSnmpEngineID =
                CloneOctetString(snmp_v3_lcd->snmpEngineData->snmpEngineID);
            goto done;
        }
    }

    if (snmp_v3_lcd->override_userSecurityName) {
        /* Use override information */
        uue = &uue_s;
        uue_s.usmUserName = snmp_v3_lcd->override_userSecurityName;
        uue_s.usmUserSecurityName = snmp_v3_lcd->override_userSecurityName;
#ifndef SR_UNSECURABLE
        uue_s.auth_secret = snmp_v3_lcd->override_auth_secret;
        snmp_msg->u.v3.authProtocol = snmp_v3_lcd->override_auth_protocol;
        snmp_msg->u.v3.authLocalized = snmp_v3_lcd->override_auth_localized;
#ifndef SR_NO_PRIVACY
        uue_s.priv_secret = snmp_v3_lcd->override_priv_secret;
        snmp_msg->u.v3.privProtocol = snmp_v3_lcd->override_priv_protocol;
        snmp_msg->u.v3.privLocalized = snmp_v3_lcd->override_priv_localized;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
        uue_s.usmTargetTag = NULL;
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
        uue_s.key1 = NULL;
        uue_s.key2 = NULL;
        uue_s.key3 = NULL;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
    } else {
        /* Lookup the user in the usmUserTable */
        snmp_v3_lcd->usmUserTable->tip[0].value.octet_val =
            snmp_msg->u.v3.authSnmpEngineID;
        snmp_v3_lcd->usmUserTable->tip[1].value.octet_val =
            snmp_msg->u.v3.userName;
        index = SearchTable(snmp_v3_lcd->usmUserTable, EXACT);

        if (index == -1) {
#ifdef SR_DEBUG
            char *engineID_token = NULL;
            char *username_token = NULL;

            ConvToken_octetString(PARSER_CONVERT_TO_TOKEN,
                                  &engineID_token,
                                  &snmp_msg->u.v3.authSnmpEngineID);
            ConvToken_textOctetString(PARSER_CONVERT_TO_TOKEN,
                                  &username_token,
                                  &snmp_msg->u.v3.userName);
            if (engineID_token != NULL && username_token != NULL) {
                DPRINTF((APPACKET|APCONFIG,
                         "%s: user not configured (unknown user error)\n"
                         "       usmUserName = %s\n"
                         "   usmUserEngineID = %s\n",
                         Fname, username_token, engineID_token)); 
            }
            else {
                DPRINTF((APPACKET|APCONFIG,
                         "%s: user not configured (unknown user error)\n",
                         Fname));
            }    
            if (engineID_token != NULL) {
                free(engineID_token);
            }
            if (username_token != NULL) {
                free(username_token);
            }
#endif /* SR_DEBUG */
            snmp_msg->error_code = SR_USM_UNKNOWN_USER_NAMES;
            goto done;
        }
        uue = (usmUserEntry_t *)snmp_v3_lcd->usmUserTable->tp[index];
        if (uue->usmUserStatus != D_usmUserStatus_active) {
            snmp_msg->error_code = SR_USM_UNKNOWN_USER_NAMES;
            goto done;
        }
#ifndef SR_UNSECURABLE
        snmp_msg->u.v3.authProtocol =
            SrV3ProtocolOIDToInt(uue->usmUserAuthProtocol);
        snmp_msg->u.v3.authLocalized = 1;
#ifndef SR_NO_PRIVACY
        snmp_msg->u.v3.privProtocol =
            SrV3ProtocolOIDToInt(uue->usmUserPrivProtocol);
        snmp_msg->u.v3.privLocalized = 1;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }

    snmp_msg->u.v3.userSecurityName =
        CloneOctetString(uue->usmUserName);
    if (snmp_msg->u.v3.userSecurityName == NULL) {
        DPRINTF((APERROR, "%s: userSecurityName malloc\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        goto done;
    }

#ifndef SR_UNSECURABLE
    /* Make sure keys exist for this security level */
    switch (snmp_msg->u.v3.securityLevel) {
#ifndef SR_NO_PRIVACY
        case SR_SECURITY_LEVEL_PRIV:
            if (snmp_msg->u.v3.privProtocol == SR_USM_NOPRIV_PROTOCOL) {
                snmp_msg->error_code = SR_USM_UNSUPPORTED_SEC_LEVELS;
                goto done;
            }
            if (uue->priv_secret == NULL) {
                snmp_msg->error_code = SR_USM_UNSUPPORTED_SEC_LEVELS;
                goto done;
            }
            snmp_msg->u.v3.privKey = CloneOctetString(uue->priv_secret);
            if (snmp_msg->u.v3.privKey == NULL) {
                DPRINTF((APERROR, "%s: privKey malloc\n", Fname));
                snmp_msg->error_code = ASN_PARSE_ERROR;
                goto done;
            }
#endif /* SR_NO_PRIVACY */
        case SR_SECURITY_LEVEL_AUTH:
            if (snmp_msg->u.v3.authProtocol == SR_USM_NOAUTH_PROTOCOL) {
                snmp_msg->error_code = SR_USM_UNSUPPORTED_SEC_LEVELS;
                goto done;
            }
            if (uue->auth_secret == NULL) {
                snmp_msg->error_code = SR_USM_UNSUPPORTED_SEC_LEVELS;
                goto done;
            }
            snmp_msg->u.v3.authKey = CloneOctetString(uue->auth_secret);
            if (snmp_msg->u.v3.authKey == NULL) {
                DPRINTF((APERROR, "%s: authKey malloc\n", Fname));
                snmp_msg->error_code = ASN_PARSE_ERROR;
                goto done;
            }
    }

    if (SrValidateTransportAddress(sl_ptr->vta_data,
                                   sl_ptr->vta_function,
                                   src_ti,
                                   uue->usmTargetTag) == 0) {
        DPRINTF((APACCESS, "%s: invalid source address\n", Fname));
        snmp_msg->error_code = SR_USM_UNKNOWN_USER_NAMES;
        goto done;
    }

    /* Digest calculation */
    if (snmp_msg->u.v3.securityLevel >= SR_SECURITY_LEVEL_AUTH) {
        /*
         * NOTE: Right now, length of authParameters is always 
         * 12 bytes (96 bits) because we're using either
         * HMAC-MD5-96 or HMAC-SHA-96. If we change this in the
         * future, we'll need to do length check based on
         * snmp_msg->u.v3.authProtocol.
         */
        if (authParameters->length != SR_HMAC_96_DIGEST_LENGTH) {
            DPRINTF((APACCESS,
                    "%s: Length of authParameters is %d bytes, should be 12 bytes\n", Fname, authParameters->length));
            snmp_msg->error_code = SR_USM_WRONG_DIGESTS;
            goto fail;
        }

        memset(digest_ptr, 0, (size_t)authParameters->length);
        if (!snmp_msg->u.v3.authLocalized) {
            lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                               snmp_msg->u.v3.authSnmpEngineID,
                               snmp_msg->u.v3.authKey);
            if (lk == NULL) {
                DPRINTF((APERROR, "%s: localized key malloc\n", Fname));
                snmp_msg->error_code = ASN_PARSE_ERROR;
                goto done;
            }
            FreeOctetString(snmp_msg->u.v3.authKey);
            snmp_msg->u.v3.authKey = lk;
            lk = NULL;
            snmp_msg->u.v3.authLocalized = 1;
        }
        HmacHash(snmp_msg->u.v3.authProtocol,
                 snmp_msg->u.v3.authKey->octet_ptr,
                 hmac_digest,
                 msg,
                 msg_len);
        /* If the authentication parameters don't match, restore
           them in the message, in case we do this twice on a packet. */
        if (memcmp(authParameters->octet_ptr,
                   hmac_digest, (size_t)authParameters->length)) {
            DPRINTF((APACCESS, "%s: digest mismatch\n", Fname));
            snmp_msg->error_code = SR_USM_WRONG_DIGESTS;
            memcpy(digest_ptr, authParameters->octet_ptr, 
                   (size_t)authParameters->length);
            goto done;
        }

        if (SrLookupSnmpEngineBootsAndTime(snmp_v3_lcd,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           &snmpEngineBoots,
                                           &snmpEngineTime,
                                           &lastReceived_snmpEngineTime)) {
            /* case a (local engine is authoritative) */
            if ((snmpEngineBoots == 0x7fffffff) ||
                (snmpEngineBoots != snmp_msg->u.v3.authSnmpEngineBoots) ||
                (snmp_msg->u.v3.authSnmpEngineTime <
                    (snmpEngineTime - SR_USM_LIFETIME_WINDOW)) ||
                (snmp_msg->u.v3.authSnmpEngineTime >
                    (snmpEngineTime + SR_USM_LIFETIME_WINDOW))) {
                DPRINTF((APACCESS, "%s: not in lifetime failure\n", Fname));
                snmp_msg->error_code = SR_USM_NOT_IN_TIME_WINDOWS;
            }
        } else {
            /* case a (local engine is authoritative) */
            /* case b (remote engine is authoritative) */
            /* step 1 */
            if ((snmp_msg->u.v3.authSnmpEngineBoots > snmpEngineBoots) ||
                ((snmp_msg->u.v3.authSnmpEngineBoots == snmpEngineBoots) &&
                 (snmp_msg->u.v3.authSnmpEngineTime >
                     lastReceived_snmpEngineTime))) {
                SR_INT32 curtime;
                GetTimeNowInSeconds(&curtime, NULL);
                SetBootsAndTimeEntry(snmp_msg->u.v3.authSnmpEngineID,
                                     snmp_msg->u.v3.authSnmpEngineBoots,
                                     snmp_msg->u.v3.authSnmpEngineTime -
                                         curtime,
                                     snmp_msg->u.v3.authSnmpEngineTime);
                snmpEngineBoots = snmp_msg->u.v3.authSnmpEngineBoots;
                snmpEngineTime = snmp_msg->u.v3.authSnmpEngineTime;
            }
            /* step 2 */
            if ((snmpEngineBoots == 0x7fffffff) ||
                (snmp_msg->u.v3.authSnmpEngineBoots < snmpEngineBoots) ||
                ((snmp_msg->u.v3.authSnmpEngineBoots == snmpEngineBoots) &&
                 (snmp_msg->u.v3.authSnmpEngineTime <
                     (snmpEngineTime - SR_USM_LIFETIME_WINDOW)))) {
                DPRINTF((APACCESS, "%s: not in lifetime failure\n", Fname));
                snmp_msg->error_code = SR_USM_NOT_IN_TIME_WINDOWS;
            }
        }
    }

#ifndef SR_NO_PRIVACY
    /* Decrypt scoped pdu */
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        switch (snmp_msg->u.v3.privProtocol) {
            case SR_USM_DES_PRIV_PROTOCOL:
#ifdef SR_3DES
            case SR_USM_3DES_PRIV_PROTOCOL:
#endif /* SR_3DES */
#ifdef SR_AES
            case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
#endif /* SR_AES */
                if (privParameters->length != 8) {
                    DPRINTF((APPACKET, "%s: bad priv parameters\n", Fname));
                    SET_MSG_ERR_CODE(snmp_msg, SR_USM_DECRYPTION_ERRORS);
                    goto done;
                }
                if (ParseType(&snmp_msg->wptr,
                              snmp_msg->end_ptr) != OCTET_PRIM_TYPE) {
                    DPRINTF((APPACKET,
                             "%s: encrypted ScopedPDU bad tag", Fname));
                    if (snmp_msg->error_code != 0) {
                        goto done;
                    }
                    snmp_msg->error_code = ASN_PARSE_ERROR;
                    goto fail;
                }
                encrypt_length =
                    ParseLength(&snmp_msg->wptr, snmp_msg->end_ptr);
                if (encrypt_length == -1) {
                    DPRINTF((APPACKET, "%s: length error.\n", Fname));
                    if (snmp_msg->error_code != 0) {
                        goto done;
                    }
                    snmp_msg->error_code = ASN_PARSE_ERROR;
                    goto fail;
                }
                if ((snmp_msg->wptr + encrypt_length) < snmp_msg->end_ptr) {
                    DPRINTF((APPACKET, "%s: ScopedPDU too long.\n", Fname));
                    if (snmp_msg->error_code != 0) {
                        goto done;
                    }
                    snmp_msg->error_code = ASN_PARSE_ERROR;
                    goto fail;
                }
                if ((snmp_msg->wptr + encrypt_length) > snmp_msg->end_ptr) {
                    DPRINTF((APPACKET, "%s: ScopedPDU too short.\n", Fname));
                    if (snmp_msg->error_code != 0) {
                        goto done;
                    }
                    snmp_msg->error_code = ASN_PARSE_ERROR;
                    goto fail;
                }
#ifndef SR_3DES
                if (snmp_msg->u.v3.privProtocol == SR_USM_DES_PRIV_PROTOCOL)
#else /* SR_3DES */
		if (snmp_msg->u.v3.privProtocol == SR_USM_DES_PRIV_PROTOCOL ||
                     snmp_msg->u.v3.privProtocol == SR_USM_3DES_PRIV_PROTOCOL)
#endif /* SR_3DES */
                {
                    if ((snmp_msg->end_ptr - snmp_msg->wptr) % 8) {
                        DPRINTF((APPACKET,
                             "%s: length of pdu not multiple of 8\n", Fname));
                        SET_MSG_ERR_CODE(snmp_msg, SR_USM_DECRYPTION_ERRORS);
                        goto done;
                    }
                }
                if (snmp_msg->u.v3.privProtocol == SR_USM_DES_PRIV_PROTOCOL) {
                    if (!snmp_msg->u.v3.privLocalized) {
                        lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           snmp_msg->u.v3.privKey);
                        if (lk == NULL) {
                            DPRINTF((APERROR, "%s: localized key malloc\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        FreeOctetString(snmp_msg->u.v3.privKey);
                        snmp_msg->u.v3.privKey = lk;
                        lk = NULL;
                        snmp_msg->u.v3.privLocalized = 1;
                    }
                    DesDecrypt(snmp_msg->u.v3.privKey->octet_ptr,
                               privParameters->octet_ptr,
                               (unsigned char *)snmp_msg->wptr,
                               snmp_msg->end_ptr - snmp_msg->wptr);
                    decrypted = 1;
                }
                
#ifdef SR_3DES
                else if (snmp_msg->u.v3.privProtocol == SR_USM_3DES_PRIV_PROTOCOL) {
                    if (!snmp_msg->u.v3.privLocalized) {
                        lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           snmp_msg->u.v3.privKey);
                        if (lk == NULL) {
                            DPRINTF((APERROR,
                                  "%s: localized key malloc\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        lk2 = SrOctetStringPasswordToLocalizedKey(snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           lk);
                        if (lk2 == NULL) {
                            DPRINTF((APERROR,
                                     "%s: localized key2 malloc failure\n",
                                     Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        FreeOctetString(snmp_msg->u.v3.privKey);
                        snmp_msg->u.v3.privKey = NULL;
                        snmp_msg->u.v3.privKey =
                              MakeOctetString(NULL, SR_3DES_KEY_LENGTH);
                        if (snmp_msg->u.v3.privKey == NULL) {
                            DPRINTF((APERROR,
                                  "%s: privKey malloc failure\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        memcpy(snmp_msg->u.v3.privKey->octet_ptr, lk->octet_ptr,
                               lk->length);
                        remainderLen = SR_3DES_KEY_LENGTH - lk->length;
                        tmp_ptr = snmp_msg->u.v3.privKey->octet_ptr + lk->length;
                        memcpy(tmp_ptr, lk2->octet_ptr, remainderLen);
                        if (uue->key1 != NULL) {
                            FreeOctetString(uue->key1);
                            uue->key1 = NULL;
                        }
                        if (uue->key2 != NULL) {
                            FreeOctetString(uue->key2);
                            uue->key2 = NULL;
                        }
                        if (uue->key3 != NULL) {
                            FreeOctetString(uue->key3);
                            uue->key3 = NULL;
                        }
                        FreeOctetString(lk);
                        lk = NULL;
                        FreeOctetString(lk2);
                        lk2 = NULL;
                        snmp_msg->u.v3.privLocalized = 1;

                        status = SrAssign3DESKeys(snmp_msg, uue);
                        if (status == -1) {
                            DPRINTF((APERROR,
                                      "%s: SrAssign3DESKeys failed\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                    }
                    /* Already localized */
                    else {
                        /* 
                         * If all 3 keys are already present, do nothing.
                         * If they are not, free previous values, and copy in
                         * values from privSecret.
                         */
                        if ((uue->key1 == NULL) || (uue->key2 == NULL) ||
                            (uue->key3 == NULL) || (uue->privKeyChangeFlag == TRUE) ||
                            (uue->key1->length != (SR_3DES_EACH_KEY_LENGTH * 2)) ||
                            (uue->key2->length != (SR_3DES_EACH_KEY_LENGTH * 2)) ||
                            (uue->key3->length != (SR_3DES_EACH_KEY_LENGTH * 2))) {
                            if (uue->key1 != NULL) {
                                FreeOctetString(uue->key1);
                                uue->key1 = NULL;
                            }
                            if (uue->key2 != NULL) {
                                FreeOctetString(uue->key2);
                                uue->key2 = NULL;
                            }
                            if (uue->key3 != NULL) {
                                FreeOctetString(uue->key3);
                                uue->key3 = NULL;
                            }
                            status = SrAssign3DESKeys(snmp_msg, uue);
                            if (status == -1) {
                                DPRINTF((APERROR,
                                         "%s: SrAssign3DESKeys failed\n",
                                         Fname));
                                SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                                goto done;
                            }
                            uue->privKeyChangeFlag = FALSE;
                        }
                    }

                    if (CmpOctetStrings(uue->key1, uue->key2) == 0) {
                        DPRINTF((APWARN, 
                             "%s: Warning - 3DES key1 and key2 are equal!\n"));
                    }
                    if (CmpOctetStrings(uue->key2, uue->key3) == 0) {
                        DPRINTF((APWARN, 
                             "%s: Warning - 3DES key2 and key3 are equal!\n"));
                    }
                    if (CmpOctetStrings(uue->key1, uue->key3) == 0) {
                        DPRINTF((APWARN, 
                             "%s: Warning - 3DES key1 and key3 are equal!\n"));
                    }

                    DesEDE_Decrypt(uue->key1->octet_ptr,
                                   uue->key2->octet_ptr,
                                   uue->key3->octet_ptr,
                                   privParameters->octet_ptr,
                                   (unsigned char *)snmp_msg->wptr,
                                   snmp_msg->end_ptr - snmp_msg->wptr);

                    decrypted = 1;
                }
#endif /* SR_3DES */
#ifdef SR_AES
                else if ((snmp_msg->u.v3.privProtocol == 
                                      SR_USM_AES_CFB_128_PRIV_PROTOCOL) ||
		         (snmp_msg->u.v3.privProtocol ==
                                      SR_USM_AES_CFB_192_PRIV_PROTOCOL) ||
		         (snmp_msg->u.v3.privProtocol ==
                                      SR_USM_AES_CFB_256_PRIV_PROTOCOL)) {
                    if (!snmp_msg->u.v3.privLocalized) {
                        /*
                         * password - can be a "phrase" instead of just like
                         *            a UNIX password. 
                         * 
                         *            Example: "This is an AES password"
                         *
                         * P2K = password to localized key function (including
                         *       hashing). The P2K function will return a
                         *       16 byte (128 bits) localized key when using
                         *       MD5 authentication and return a 20 byte
                         *       (160 bits) localized key when using SHA
                         *       authentication.
                         *
                         * localized key (lk) =  P2K(password)
                         *
                         * AES priv key = lk || P2K(lk)
                         *
                         * When using MD5 authentication, the generated AES
                         * privacy key will be 256 bits (32 bytes).
                         *
                         * When using SHA authentication, the generated AES
                         * privacy key will be 320 bits (40 bytes).
                         *
                         * An AES key can either be a 128-bit, a 192-bit, or 
                         * a 256-bit key. Adjust the length of the resulting 
                         * octetstring to the privacy protocol being used. 
                         */

                        /* Get first part of AES key */
                        lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           snmp_msg->u.v3.privKey);
                        if (lk == NULL) {
                            DPRINTF((APERROR,
                                  "%s: localized key malloc\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        /* 
                         * Use output of first key localization, lk, as
                         * input for the second part of the AES key.
                         */
                        lk2 = SrOctetStringPasswordToLocalizedKey(
					   snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           lk);
                        if (lk2 == NULL) {
                            DPRINTF((APERROR,
                                     "%s: localized key2 malloc failure\n",
                                     Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        FreeOctetString(snmp_msg->u.v3.privKey);
                        snmp_msg->u.v3.privKey = NULL;
 
                        /*
                         * Make an octet string for the biggest possible
                         * AES key we can have (SR_AES_KEY_LENGTH).
                         */
                        snmp_msg->u.v3.privKey =
                              MakeOctetString(NULL, SR_AES_KEY_LENGTH);
                        if (snmp_msg->u.v3.privKey == NULL) {
                            DPRINTF((APERROR,
                                  "%s: privKey malloc failure\n", Fname));
                            SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
                            goto done;
                        }
                        /* 
                         * Copy in the first part of the AES key into the
                         * privKey field.
                         */
                        memcpy(snmp_msg->u.v3.privKey->octet_ptr, lk->octet_ptr,
                               lk->length);
                        remainderLen = SR_AES_KEY_LENGTH - lk->length;
                        tmp_ptr = snmp_msg->u.v3.privKey->octet_ptr + lk->length;
                        /*
                         * Copy the second part of the AES privacy key into
                         * place.
                         */
 
                        memcpy(tmp_ptr, lk2->octet_ptr, remainderLen);
                        FreeOctetString(lk);
                        lk = NULL;
                        FreeOctetString(lk2);
                        lk2 = NULL;
                        /*
                         * Adjust key length for AES128 and AES192
                         */
                        if (snmp_msg->u.v3.privProtocol ==
                              SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
                            snmp_msg->u.v3.privKey->length = 
                                  SR_AES_128_KEY_LENGTH;
                        }
                        if (snmp_msg->u.v3.privProtocol ==
                              SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
                            snmp_msg->u.v3.privKey->length = 
                                  SR_AES_192_KEY_LENGTH;
                        }

                        snmp_msg->u.v3.privLocalized = 1;
                    }
                    engTime = htonl(snmp_msg->u.v3.authSnmpEngineTime);
                    engBoots = htonl(snmp_msg->u.v3.authSnmpEngineBoots);
		    memcpy(iv, &engBoots, 4);
		    memcpy(iv+4, &engTime, 4);
		    memcpy(iv+8, privParameters->octet_ptr, 8);
                    AES_cfb_Decrypt(snmp_msg->u.v3.privKey->octet_ptr,
                                      iv, 
				      (unsigned char *)snmp_msg->wptr,
				      snmp_msg->end_ptr - snmp_msg->wptr,
                                      snmp_msg->u.v3.privProtocol);
                    decrypted = 1;
                }
#endif /* SR_AES */
                break;
            default:
                DPRINTF((APWARN, "%s: Unknown privacy protocol!\n", Fname));
                break;
        }
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

  done:
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
    /* 
     * If override was set, a static entry is used.
     * Free the strings we malloced. 
     */
    if (snmp_v3_lcd->override_userSecurityName) {
        if (uue->key1 != NULL) {
            FreeOctetString(uue->key1);
        }
        if (uue->key2 != NULL) {
            FreeOctetString(uue->key2);
        }
        if (uue->key3 != NULL) {
            FreeOctetString(uue->key3);
        }
    } 
#endif /* SR_3DES */
    /* if we needed to decrypt, but couldn't, we cannot continue parsing */
    if (!decrypted &&
        (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV)) {
        goto fail;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /* Extract contextSnmpEngineID and contextName */
    /* check sequence and length */
    seq_length = ParseSequence(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: ParseSequence:\n", Fname));
        SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
        goto fail;
    }
    new_end_ptr = (unsigned char *)snmp_msg->wptr + seq_length;
    if (new_end_ptr < snmp_msg->end_ptr) {
        snmp_msg->end_ptr = new_end_ptr;
    } else if (new_end_ptr > snmp_msg->end_ptr) {
        DPRINTF((APPACKET, "%s: bad scopedPDU BER length:\n", Fname));
        SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
        goto fail;
    }

    /* Extract contextSnmpEngineID */
    snmp_msg->u.v3.contextSnmpEngineID =
        ParseOctetString(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (snmp_msg->u.v3.contextSnmpEngineID == NULL) {
        DPRINTF((APPACKET, "%s: bad contextSnmpEngineID\n", Fname));
        SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
        goto fail;
    }

    /* Extract contextName */
    snmp_msg->u.v3.contextName =
        ParseOctetString(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (snmp_msg->u.v3.contextName == NULL) {
        DPRINTF((APPACKET, "%s: bad contextName\n", Fname));
        SET_MSG_ERR_CODE(snmp_msg, ASN_PARSE_ERROR);
        goto fail;
    }

    /* Determine maximum size of a response pdu that can be sent back */
    snmp_msg->wrapper_size = snmp_msg->wptr - msg;

    /* Response packet may contain significantly larger PDU. In worst case
     * scenario, encoded sequence lengths may be as much as 2 octets larger. 
     * Add maximum of 2 extra octets for each of the following:
     *      msg length, scoped pdu length, pdu length
     */
    snmp_msg->wrapper_size += 6;
 
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    /* If privacy(encryption) is used, add extra padding for encryption
     * encoding. Maximum of 5 additional octets (scoped pdu padding + 1)
     */
    if (snmp_msg->u.v3.privProtocol != SR_USM_NOPRIV_PROTOCOL) {
        snmp_msg->wrapper_size += 5;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    snmp_msg->max_vbl_size = snmp_msg->u.v3.mms -
                             snmp_msg->wrapper_size -
                             SR_PDU_WRAPPER_SIZE;
    /* Copy remaining data (the PDU portion) into snmp_msg->packlet */
    snmp_msg->packlet =
        MakeOctetString2(snmp_msg->wptr,
                        (SR_INT32) (snmp_msg->end_ptr - snmp_msg->wptr));

    if (snmp_msg->packlet == NULL) {
      DPRINTF((APPACKET, "%s: unable to create snmp_msg PDU packlet\n", Fname));
      SET_MSG_ERR_CODE(snmp_msg, RESOURCE_UNAVAILABLE_ERROR);
      goto fail;
    }

  fail:
    if (authParameters != NULL) {
        FreeOctetString(authParameters);
    }
    if (privParameters != NULL) {
        FreeOctetString(privParameters);
    }
#ifndef SR_UNSECURABLE
    if (lk != NULL) {
        FreeOctetString(lk);
    }
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
    if (lk2 != NULL) {
        FreeOctetString(lk2);
    }
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
}

int
SrBuildV3SnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    SnmpLcd *sl_ptr)
{
    FNAME("SrBuildV3SnmpMessage")
    SnmpV3Lcd *snmp_v3_lcd = (SnmpV3Lcd *)v_sl_ptr;
    SR_INT32 scoped_pdu_len, scoped_pdu_lenlen;
    SR_INT32 security_params_len, security_params_lenlen;
    SR_INT32 os_security_params_len, os_security_params_lenlen;
    SR_INT32 header_len, header_lenlen;
    SR_INT32 total_len, total_lenlen;
    SR_INT32 padded_scoped_pdu_len;
#ifndef SR_UNSECURABLE
    int need_keys = 0;
    int index;
    usmUserEntry_t *uue = NULL;
    OctetString *lk;
#ifndef SR_NO_PRIVACY
    SR_INT32 encrypt_len = 0, total_encrypt_len = 0;
    SR_UINT32 salt_val;
    OctetString usm_salt_os;
    unsigned char _usm_salt_os[8];
#if (defined(SR_3DES) || defined(SR_AES))
    OctetString *lk2;
    SR_INT32 remainderLen;
    unsigned char *tmp_ptr;
#endif	/* (defined(SR_3DES) || defined(SR_AES)) */
#ifdef SR_3DES
    SR_INT32 pos;
    OctetString *usm_salt_hash_os = NULL;
    SR_INT32 status, static_uue = FALSE;
    usmUserEntry_t uue_s;
#endif /* SR_3DES */
#ifdef SR_AES
    unsigned char iv[16];
    UInt64 IV_val;
    SR_INT32 engBoots;
    SR_INT32 engTime;
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
    unsigned char *digest_ptr = NULL;
    SR_INT32 lastReceived_snmpEngineTime;
#endif /* SR_UNSECURABLE */
    SR_UINT32 msgFlags;
    OctetString msgFlags_os;
    unsigned char msgFlags_os_buf[1];
    unsigned char *wptr;
    unsigned char *scoped_pdu_ptr;

    /* --- Verify an authSnmpEngineID exists in the message --- */
    if (snmp_msg->u.v3.authSnmpEngineID == NULL) {
        DPRINTF((APPACKET, "%s: no authSnmpEngineID\n", Fname));
        return -1;
    }

    snmp_msg->u.v3.mms = snmp_v3_lcd->snmpEngineData->snmpEngineMaxMessageSize;

    /* calculate msgFlags */
    msgFlags = 0;
    if ((pdu_ptr->type == GET_REQUEST_TYPE) ||
        (pdu_ptr->type == GET_NEXT_REQUEST_TYPE) ||
        (pdu_ptr->type == GET_BULK_REQUEST_TYPE) ||
        (pdu_ptr->type == SET_REQUEST_TYPE) ||
        (pdu_ptr->type == INFORM_REQUEST_TYPE)) {
        msgFlags |= 0x04;
        snmp_msg->u.v3.reportableFlag = 1;
    }
    switch (snmp_msg->u.v3.securityLevel) {
        case SR_SECURITY_LEVEL_NOAUTH:
            msgFlags |= SR_SECURITY_LEVEL_BITS_NOAUTH_NOPRIV;
            break;
#ifndef SR_UNSECURABLE
        case SR_SECURITY_LEVEL_AUTH:
            msgFlags |= SR_SECURITY_LEVEL_BITS_AUTH_NOPRIV;
            if (snmp_msg->u.v3.authKey == NULL) {
                need_keys = 1;
            }
            break;
#ifndef SR_NO_PRIVACY
        case SR_SECURITY_LEVEL_PRIV:
            msgFlags |= SR_SECURITY_LEVEL_BITS_AUTH_PRIV;
            if (snmp_msg->u.v3.privKey == NULL) {
                need_keys = 1;
            }
            break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
        default:
            DPRINTF((APPACKET, "%s: bad securityLevel\n", Fname));
            return -1;
    }
    msgFlags_os.length = 1;
    msgFlags_os.octet_ptr = msgFlags_os_buf;
    msgFlags_os.octet_ptr[0] = (unsigned char)(msgFlags & 0xff);

#ifndef SR_UNSECURABLE
    /* Lookup keys from usmUserTable if necessary */
    if (need_keys) {
#ifdef SR_DEBUG
        char *engineID_token = NULL;
        char *username_token = NULL;

        ConvToken_octetString(PARSER_CONVERT_TO_TOKEN,
                              &engineID_token,
                              &snmp_msg->u.v3.authSnmpEngineID);
        ConvToken_textOctetString(PARSER_CONVERT_TO_TOKEN,
                              &username_token,
                              &snmp_msg->u.v3.userName);
#endif /* SR_DEBUG */
        if (snmp_msg->u.v3.authKey != NULL) {
            FreeOctetString(snmp_msg->u.v3.authKey);
        }
        snmp_msg->u.v3.authKey = NULL;
#ifndef SR_NO_PRIVACY
        if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
            if (snmp_msg->u.v3.privKey != NULL) {
                FreeOctetString(snmp_msg->u.v3.privKey);
            }
            snmp_msg->u.v3.privKey = NULL;
        }
#endif /* SR_NO_PRIVACY */
        snmp_v3_lcd->usmUserTable->tip[0].value.octet_val =
            snmp_msg->u.v3.authSnmpEngineID;
        snmp_v3_lcd->usmUserTable->tip[1].value.octet_val =
            snmp_msg->u.v3.userName;
        index = SearchTable(snmp_v3_lcd->usmUserTable, EXACT);
        if (index == -1) {
#ifdef SR_DEBUG
            if (engineID_token != NULL && username_token != NULL) {
                DPRINTF((APPACKET,
                         "%s: no keys configured\n"
                         "       usmUserName = %s\n"
                         "   usmUserEngineID = %s\n",
                         Fname, username_token, engineID_token)); 
            }
            else {
                DPRINTF((APPACKET,
                         "%s: no keys configured\n",
                         Fname));
            }    
            if (engineID_token != NULL) {
                free(engineID_token);
            }
            if (username_token != NULL) {
                free(username_token);
            }
#endif /* SR_DEBUG */
            return -1;
        }
        uue = (usmUserEntry_t *)snmp_v3_lcd->usmUserTable->tp[index];
        snmp_msg->u.v3.authKey = CloneOctetString(uue->auth_secret);
        snmp_msg->u.v3.authProtocol =
            SrV3ProtocolOIDToInt(uue->usmUserAuthProtocol);
        snmp_msg->u.v3.authLocalized = 1;
        if (snmp_msg->u.v3.authKey == NULL) {
#ifdef SR_DEBUG
            if (engineID_token != NULL && username_token != NULL) {
                DPRINTF((APPACKET,
                         "%s: no authentication key configured\n"
                         "       usmUserName = %s\n"
                         "   usmUserEngineID = %s\n",
                         Fname, username_token, engineID_token)); 
            }
            else {
                DPRINTF((APPACKET,
                         "%s: no authentication key configured\n",
                         Fname));
            }    
            if (engineID_token != NULL) {
                free(engineID_token);
            }
            if (username_token != NULL) {
                free(username_token);
            }
#endif /* SR_DEBUG */
            return -1;
        }
#ifndef SR_NO_PRIVACY
        if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
            snmp_msg->u.v3.privKey = CloneOctetString(uue->priv_secret);
            snmp_msg->u.v3.privProtocol =
                SrV3ProtocolOIDToInt(uue->usmUserPrivProtocol);
            snmp_msg->u.v3.privLocalized = 1;
            if (snmp_msg->u.v3.privKey == NULL) {
#ifdef SR_DEBUG
                if (engineID_token != NULL && username_token != NULL) {
                    DPRINTF((APPACKET,
                             "%s: no privacy key configured\n"
                             "       usmUserName = %s\n"
                             "   usmUserEngineID = %s\n",
                             Fname, username_token, engineID_token)); 
                }
                else {
                    DPRINTF((APPACKET,
                             "%s: no privacy key configured\n",
                             Fname));
                }    
                if (engineID_token != NULL) {
                    free(engineID_token);
                }
                if (username_token != NULL) {
                    free(username_token);
                }
#endif /* SR_DEBUG */
                return -1;
            }
        }
#endif /* SR_NO_PRIVACY */
#ifdef SR_DEBUG
        if (engineID_token != NULL) {
            free(engineID_token);
        }
        if (username_token != NULL) {
            free(username_token);
        }
#endif /* SR_DEBUG */
    }


    /* retrieve boots and time values */
    SrLookupSnmpEngineBootsAndTime(snmp_v3_lcd,
                                   snmp_msg->u.v3.authSnmpEngineID,
                                   &snmp_msg->u.v3.authSnmpEngineBoots,
                                   &snmp_msg->u.v3.authSnmpEngineTime,
                                   &lastReceived_snmpEngineTime);

    
    /*
     * if this is a trap sent from a trapsend utility, 
     *  1. use timeof day or system time as an authSnmpEngineTime 
     *     instead of using a time value returned from 
     *     SrLookupSnmpEngineBootsAndTime() function.
     *  2. authSnmpEngineBoots is always 0.
     */
    if (inform_type == SEND_TRAP) {
        SrTOD tod;
        GetTOD(&tod);
	if (tod.secs == 0) {
	    snmp_msg->u.v3.authSnmpEngineTime = (SR_INT32) (tod.nsecs/100);
        }
        else {
            /* use system clock */
	    snmp_msg->u.v3.authSnmpEngineTime = (SR_INT32) tod.secs;
	}
	snmp_msg->u.v3.authSnmpEngineBoots = 0;
    }
#ifndef SR_NO_PRIVACY
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        switch (snmp_msg->u.v3.privProtocol) {
            case SR_USM_DES_PRIV_PROTOCOL:
#ifdef SR_3DES
            case SR_USM_3DES_PRIV_PROTOCOL:
#endif /* SR_3DES */
                /* initialize salt value */
                usm_salt_os.length = 8;
                usm_salt_os.octet_ptr = _usm_salt_os;
                salt_val = (SR_UINT32)snmp_msg->u.v3.authSnmpEngineBoots;
                _usm_salt_os[0] = (unsigned char)((salt_val & 0xff000000) >> 24);
                _usm_salt_os[1] = (unsigned char)((salt_val & 0xff0000) >> 16);
                _usm_salt_os[2] = (unsigned char)((salt_val & 0xff00) >> 8);
                _usm_salt_os[3] = (unsigned char)(salt_val & 0xff);
                _usm_salt_os[4] = (unsigned char)
                    ((snmp_v3_lcd->usm_salt & 0xff000000) >> 24);
                _usm_salt_os[5] = (unsigned char)
                    ((snmp_v3_lcd->usm_salt & 0xff0000) >> 16);
                _usm_salt_os[6] = (unsigned char)
                    ((snmp_v3_lcd->usm_salt & 0xff00) >> 8);
                _usm_salt_os[7] = (unsigned char)
                    (snmp_v3_lcd->usm_salt & 0xff);
#ifdef SR_3DES
                /*
                 * To achieve effective bit spreading, hash the salt value
                 * using the user's auth protocol.
                 */
                if (snmp_msg->u.v3.privProtocol == SR_USM_3DES_PRIV_PROTOCOL) {
                    usm_salt_hash_os = SrOctetStringPasswordToLocalizedKey(snmp_msg->u.v3.authProtocol,
                                           snmp_msg->u.v3.authSnmpEngineID,
                                           &usm_salt_os);
                    if (usm_salt_hash_os != NULL) {
                        for (pos = 0; pos < 8; pos++) {
                            usm_salt_os.octet_ptr[pos] = usm_salt_hash_os->octet_ptr[pos];
                        }    
                        FreeOctetString(usm_salt_hash_os);
                    }
                }
#endif /* SR_3DES */
                snmp_v3_lcd->usm_salt++;
                break;
#ifdef SR_AES
            case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
                /* initialize salt value */
                usm_salt_os.length = 8;
                usm_salt_os.octet_ptr = _usm_salt_os;
		Get_AES_RandVal(&IV_val);
                _usm_salt_os[0] = 
		     (unsigned char)((IV_val.big_end & 0xff000000) >> 24);
                _usm_salt_os[1] = 
		     (unsigned char)((IV_val.big_end & 0xff0000) >> 16);
                _usm_salt_os[2] = 
		     (unsigned char)((IV_val.big_end & 0xff00) >> 8);
                _usm_salt_os[3] = 
		     (unsigned char)(IV_val.big_end & 0xff);
                _usm_salt_os[4] = (unsigned char)
                    ((IV_val.little_end & 0xff000000) >> 24);
                _usm_salt_os[5] = (unsigned char)
                    ((IV_val.little_end & 0xff0000) >> 16);
                _usm_salt_os[6] = (unsigned char)
                    ((IV_val.little_end & 0xff00) >> 8);
                _usm_salt_os[7] = (unsigned char)
                    (IV_val.little_end & 0xff);

                Increment_AES_RandVal();
                break;
#endif /* SR_AES */
            case SR_USM_NOPRIV_PROTOCOL:
                DPRINTF((APWARN, "%s: secLevel is authPriv "
                         "but privProtocol is noPriv.\n", Fname));
                return -1;
            default: 
                DPRINTF((APWARN, "%s: Unknown privacy protocol.\n", Fname));
                return -1;
        }
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /* length of contextSnmpEngineID + contextName + PDU */
    scoped_pdu_len = FindLenOctetString(snmp_msg->u.v3.contextSnmpEngineID) +
                     FindLenOctetString(snmp_msg->u.v3.contextName) +
                     pdu_ptr->packlet->length;

    /* add sequence tag/length of scoped pdu */
    scoped_pdu_lenlen = DoLenLen(scoped_pdu_len);

    padded_scoped_pdu_len = scoped_pdu_lenlen + scoped_pdu_len + 1;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    /* pad for encryption */
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        switch (snmp_msg->u.v3.privProtocol) {
            case SR_USM_DES_PRIV_PROTOCOL:
#ifdef SR_3DES
            case SR_USM_3DES_PRIV_PROTOCOL:
#endif /* SR_3DES */
                if (padded_scoped_pdu_len % 8) {
                    padded_scoped_pdu_len += 8 - (padded_scoped_pdu_len % 8);
                }
                break;
        }
        encrypt_len = DoLenLen(padded_scoped_pdu_len);
        total_encrypt_len = encrypt_len + 1;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /* calculate length of security parameters */
    security_params_len =
        FindLenOctetString(snmp_msg->u.v3.authSnmpEngineID) +
        FindLenInt(snmp_msg->u.v3.authSnmpEngineBoots) +
        FindLenInt(snmp_msg->u.v3.authSnmpEngineTime) +
        FindLenOctetString(snmp_msg->u.v3.userName);
#ifdef SR_UNSECURABLE
    security_params_len += DoLenLen(0) + 1 + DoLenLen(0) + 1;
#else /* SR_UNSECURABLE */
    if (snmp_msg->u.v3.securityLevel >= SR_SECURITY_LEVEL_AUTH) {
        security_params_len += 12 + DoLenLen(12) + 1;
    } else {
        security_params_len += DoLenLen(0) + 1;
    }
#ifdef SR_NO_PRIVACY
    security_params_len += DoLenLen(0) + 1;
#else /* SR_NO_PRIVACY */
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        switch (snmp_msg->u.v3.privProtocol) {
            case SR_USM_DES_PRIV_PROTOCOL:
#ifdef SR_3DES 
            case SR_USM_3DES_PRIV_PROTOCOL:
#endif /* SR_3DES */
                security_params_len += 8 + DoLenLen(8) + 1;
                break;
#ifdef SR_AES
            case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
                security_params_len += 8 + DoLenLen(8) + 1;
                break;
#endif /* SR_AES */
            default:
                security_params_len += DoLenLen(0) + 1;
                break;
        }
    } else {
        security_params_len += DoLenLen(0) + 1;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /* add sequence tag/length of security parameters */
    security_params_lenlen = DoLenLen(security_params_len);

    /* add octetstring tag/length of security parameters */
    os_security_params_len = security_params_len + security_params_lenlen + 1;
    os_security_params_lenlen = DoLenLen(os_security_params_len);

    /* calculate length of header information */
    header_len = FindLenUInt(snmp_msg->u.v3.msgID) +
                 FindLenUInt(snmp_msg->u.v3.mms) +
                 FindLenOctetString(&msgFlags_os) +
                 FindLenUInt(snmp_msg->u.v3.securityModel);
    header_lenlen = DoLenLen(header_len);

    /* calculate total length of packet */
    total_len = FindLenUInt(snmp_msg->version) +
                header_len + header_lenlen + 1 +
                os_security_params_len + os_security_params_lenlen + 1
                + padded_scoped_pdu_len;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    total_len += total_encrypt_len;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    total_lenlen = DoLenLen(total_len);

    snmp_msg->packlet = MakeOctetString(NULL, total_len + total_lenlen + 1);
    if (snmp_msg->packlet == NULL) {
        DPRINTF((APPACKET, "%s: snmp_msg->packlet\n", Fname));
        return -1;
    }

    wptr = snmp_msg->packlet->octet_ptr;
    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, total_lenlen, total_len);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->version);
    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, header_lenlen, header_len);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->u.v3.msgID);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->u.v3.mms);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, &msgFlags_os);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->u.v3.securityModel);
    *wptr++ = OCTET_PRIM_TYPE;
    AddLen(&wptr, os_security_params_lenlen, os_security_params_len);
    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, security_params_lenlen, security_params_len);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v3.authSnmpEngineID);
    AddInt(&wptr, INTEGER_TYPE, snmp_msg->u.v3.authSnmpEngineBoots);
    AddInt(&wptr, INTEGER_TYPE, snmp_msg->u.v3.authSnmpEngineTime);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v3.userName);
    switch (snmp_msg->u.v3.securityLevel) {
        case SR_SECURITY_LEVEL_NOAUTH:
            *wptr++ = OCTET_PRIM_TYPE;
            *wptr++ = 0;
            *wptr++ = OCTET_PRIM_TYPE;
            *wptr++ = 0;
            break;
#ifndef SR_UNSECURABLE
        case SR_SECURITY_LEVEL_AUTH:
            *wptr++ = OCTET_PRIM_TYPE;
            AddLen(&wptr, DoLenLen(12), 12);
            memset(wptr, 0, 12);
            digest_ptr = wptr;
            wptr += 12;
            *wptr++ = OCTET_PRIM_TYPE;
            *wptr++ = 0;
            break;
#ifndef SR_NO_PRIVACY
        case SR_SECURITY_LEVEL_PRIV:
            *wptr++ = OCTET_PRIM_TYPE;
            AddLen(&wptr, DoLenLen(12), 12);
            memset(wptr, 0, 12);
            digest_ptr = wptr;
            wptr += 12;
            AddOctetString(&wptr, OCTET_PRIM_TYPE, &usm_salt_os);
            break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        *wptr++ = OCTET_PRIM_TYPE;
        AddLen(&wptr, encrypt_len, padded_scoped_pdu_len);
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    scoped_pdu_ptr = wptr;
    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, scoped_pdu_lenlen, scoped_pdu_len);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v3.contextSnmpEngineID);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v3.contextName);
    memcpy(wptr, pdu_ptr->packlet->octet_ptr, (size_t)pdu_ptr->packlet->length);

#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    /* encrypt the scoped pdu */
    if (snmp_msg->u.v3.securityLevel == SR_SECURITY_LEVEL_PRIV) {
        switch (snmp_msg->u.v3.privProtocol) {
            case SR_USM_DES_PRIV_PROTOCOL:
                if (!snmp_msg->u.v3.privLocalized) {
                    lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                       snmp_msg->u.v3.authSnmpEngineID,
                                       snmp_msg->u.v3.privKey);
                    if (lk == NULL) {
                        DPRINTF((APERROR, "%s: localized key malloc\n", Fname));
                        return -1;
                    }
                    FreeOctetString(snmp_msg->u.v3.privKey);
                    snmp_msg->u.v3.privKey = lk;
                    snmp_msg->u.v3.privLocalized = 1;
                }
                DesEncrypt(snmp_msg->u.v3.privKey->octet_ptr, _usm_salt_os,
                           scoped_pdu_ptr, padded_scoped_pdu_len);
                break;
#ifdef SR_3DES
            case SR_USM_3DES_PRIV_PROTOCOL:
                if (!need_keys) {
                    snmp_v3_lcd->usmUserTable->tip[0].value.octet_val =
                        snmp_msg->u.v3.authSnmpEngineID;
                    snmp_v3_lcd->usmUserTable->tip[1].value.octet_val =
                        snmp_msg->u.v3.userName;
                    index = SearchTable(snmp_v3_lcd->usmUserTable, EXACT);
                    if (index == -1) {
                        static_uue = TRUE;
                        uue = &uue_s;
                        uue->key1 = NULL;
                        uue->key2 = NULL;
                        uue->key3 = NULL;
                        uue->privKeyChangeFlag = FALSE;
                    }
                    else {
                        uue = (usmUserEntry_t *)snmp_v3_lcd->usmUserTable->tp[index];
                    }
                }
                if (!snmp_msg->u.v3.privLocalized) {
                    lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                       snmp_msg->u.v3.authSnmpEngineID,
                                       snmp_msg->u.v3.privKey);
                    if (lk == NULL) {
                        DPRINTF((APERROR,
                              "%s: localized key malloc failure\n", Fname));
                        return -1;
                    }
                    lk2 = SrOctetStringPasswordToLocalizedKey(snmp_msg->u.v3.authProtocol,
                                       snmp_msg->u.v3.authSnmpEngineID, lk);
                    if (lk2 == NULL) {
                        DPRINTF((APERROR,
                                 "%s: localized key2 malloc failure\n", Fname));
                        return -1;
                    }
                    FreeOctetString(snmp_msg->u.v3.privKey);
                    snmp_msg->u.v3.privKey = NULL;
                    snmp_msg->u.v3.privKey =
                          MakeOctetString(NULL, SR_3DES_KEY_LENGTH);
                    if (snmp_msg->u.v3.privKey == NULL) {
                        DPRINTF((APERROR,
                              "%s: privKey malloc failure\n", Fname));
                        return -1;
                    }
                    memcpy(snmp_msg->u.v3.privKey->octet_ptr, lk->octet_ptr,
                           lk->length);
                    remainderLen = SR_3DES_KEY_LENGTH - lk->length;
                    tmp_ptr = snmp_msg->u.v3.privKey->octet_ptr + lk->length;
                    memcpy(tmp_ptr, lk2->octet_ptr, remainderLen);
                    if (uue->key1 != NULL) {
                        FreeOctetString(uue->key1);
                        uue->key1 = NULL;
                    }
                    if (uue->key2 != NULL) {
                        FreeOctetString(uue->key2);
                        uue->key2 = NULL;
                    }
                    if (uue->key3 != NULL) {
                        FreeOctetString(uue->key3);
                        uue->key3 = NULL;
                    }
                    FreeOctetString(lk);
                    lk = NULL;
                    FreeOctetString(lk2);
                    lk2 = NULL;
                    snmp_msg->u.v3.privLocalized = 1;

                    status = SrAssign3DESKeys(snmp_msg, uue);
                    if (status == -1) {
                        DPRINTF((APERROR,
                                 "%s: SrAssign3DESKeys failed\n",
                                 Fname));
                        if (static_uue == TRUE) {
                            if (uue->key1 != NULL) {
                                FreeOctetString(uue->key1);
                                uue->key1 = NULL;
                            }
                            if (uue->key2 != NULL) {
                                FreeOctetString(uue->key2);
                                uue->key2 = NULL;
                            }
                            if (uue->key3 != NULL) {
                                FreeOctetString(uue->key3);
                                uue->key3 = NULL;
                            }

                        }
                        uue->privKeyChangeFlag = FALSE;
                        return -1;
                    }
                }
                /* Localized already */
                else {
                    /*
                     * If all 3 keys are already present, do nothing.
                     * If they are not, free previous values, and copy in
                     * values from privSecret.
                     */
                    if ((uue->privKeyChangeFlag == TRUE) ||
                        (uue->key1 == NULL) || (uue->key2 == NULL) ||
                        (uue->key3 == NULL) ||
                        (uue->key1->length != (SR_3DES_EACH_KEY_LENGTH * 2)) ||
                        (uue->key2->length != (SR_3DES_EACH_KEY_LENGTH * 2)) ||
                        (uue->key3->length != (SR_3DES_EACH_KEY_LENGTH * 2))) {
                        if (uue->key1 != NULL) {
                            FreeOctetString(uue->key1);
                            uue->key1 = NULL;
                        }
                        if (uue->key2 != NULL) {
                            FreeOctetString(uue->key2);
                            uue->key2 = NULL;
                        }
                        if (uue->key3 != NULL) {
                            FreeOctetString(uue->key3);
                            uue->key3 = NULL;
                        }

                        status = SrAssign3DESKeys(snmp_msg, uue);
                        if (status == -1) {
                            DPRINTF((APERROR,
                                     "%s: SrAssign3DESKeys failed\n",
                                     Fname));
                            if (static_uue == TRUE) {
                                if (uue->key1 != NULL) {
                                    FreeOctetString(uue->key1);
                                    uue->key1 = NULL;
                                }
                                if (uue->key2 != NULL) {
                                    FreeOctetString(uue->key2);
                                    uue->key2 = NULL;
                                }
                                if (uue->key3 != NULL) {
                                    FreeOctetString(uue->key3);
                                    uue->key3 = NULL;
                                }

                            }
                            uue->privKeyChangeFlag = FALSE;
                            return -1;
                        }
                    }
                }
                if (CmpOctetStrings(uue->key1, uue->key2) == 0) {
                    DPRINTF((APWARN, 
                             "%s: Warning - 3DES key1 and key2 are equal!\n"));
                }
                if (CmpOctetStrings(uue->key2, uue->key3) == 0) {
                    DPRINTF((APWARN, 
                             "%s: Warning - 3DES key2 and key3 are equal!\n"));
                }
                if (CmpOctetStrings(uue->key1, uue->key3) == 0) {
                    DPRINTF((APWARN, 
                             "%s: Warning - 3DES key1 and key3 are equal!\n"));
                }

                DesEDE_Encrypt(uue->key1->octet_ptr,
                               uue->key2->octet_ptr,
                               uue->key3->octet_ptr,
                               _usm_salt_os,
                               scoped_pdu_ptr,
                               padded_scoped_pdu_len);

                if ((static_uue == TRUE) ||
                          (uue->privKeyChangeFlag == TRUE)) {
                    if (uue->key1 != NULL) {
                        FreeOctetString(uue->key1);
                        uue->key1 = NULL;
                    }
                    if (uue->key2 != NULL) {
                        FreeOctetString(uue->key2);
                        uue->key2 = NULL;
                    }
                    if (uue->key3 != NULL) {
                        FreeOctetString(uue->key3);
                        uue->key3 = NULL;
                    }
                    uue->privKeyChangeFlag = FALSE;

                }
                break;
#endif /* SR_3DES */
#ifdef SR_AES
            case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
            case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
                if (!snmp_msg->u.v3.privLocalized) {
                    lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                                       snmp_msg->u.v3.authSnmpEngineID,
                                       snmp_msg->u.v3.privKey);
                    if (lk == NULL) {
                        DPRINTF((APERROR, "%s: localized key malloc\n", Fname));
                        return -1;
                    }
                    lk2 = SrOctetStringPasswordToLocalizedKey(
				       snmp_msg->u.v3.authProtocol,
                                       snmp_msg->u.v3.authSnmpEngineID, lk);
                    if (lk2 == NULL) {
                        DPRINTF((APERROR,
                                 "%s: localized key2 malloc failure\n", Fname));
                        return -1;
                    }
                    FreeOctetString(snmp_msg->u.v3.privKey);
                    snmp_msg->u.v3.privKey = NULL;
                    snmp_msg->u.v3.privKey =
                          MakeOctetString(NULL, SR_AES_KEY_LENGTH);
                    if (snmp_msg->u.v3.privKey == NULL) {
                        DPRINTF((APERROR,
                              "%s: privKey malloc failure\n", Fname));
                        return -1;
                    }
                    memcpy(snmp_msg->u.v3.privKey->octet_ptr, lk->octet_ptr,
                           lk->length);
                    remainderLen = SR_AES_KEY_LENGTH - lk->length;
                    tmp_ptr = snmp_msg->u.v3.privKey->octet_ptr + lk->length;
                    memcpy(tmp_ptr, lk2->octet_ptr, remainderLen);
                    FreeOctetString(lk);
                    lk = NULL;
                    FreeOctetString(lk2);
                    lk2 = NULL;
                    snmp_msg->u.v3.privLocalized = 1;
                    /*
                     * Adjust length for AES128 and AES192
                     */
                    if (snmp_msg->u.v3.privProtocol == 
                          SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
                        snmp_msg->u.v3.privKey->length =
                              SR_AES_128_KEY_LENGTH;
                    }
                    if (snmp_msg->u.v3.privProtocol ==
                          SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
                        snmp_msg->u.v3.privKey->length = 
                              SR_AES_192_KEY_LENGTH;
                    }

                }
                engBoots = htonl(snmp_msg->u.v3.authSnmpEngineBoots);
                engTime = htonl(snmp_msg->u.v3.authSnmpEngineTime);
		memcpy(iv, &engBoots, 4);
		memcpy(iv+4, &engTime, 4);
		memcpy(iv+8, _usm_salt_os, 8);
                AES_cfb_Encrypt(snmp_msg->u.v3.privKey->octet_ptr,
                                iv, 
                                (unsigned char *)scoped_pdu_ptr,
                                padded_scoped_pdu_len,
                                snmp_msg->u.v3.privProtocol);
		break;
#endif /*SR_AES */
            default:
                DPRINTF((APERROR, "%s: Unknown privacy protocol!\n", Fname));
                break;
        }
    }
#endif /* SR_NO_PRIVACY */

    /* calculate digest */
    if (snmp_msg->u.v3.securityLevel >= SR_SECURITY_LEVEL_AUTH) {
        if (!snmp_msg->u.v3.authLocalized) {
            lk = SrLocalizeKey(snmp_msg->u.v3.authProtocol,
                               snmp_msg->u.v3.authSnmpEngineID,
                               snmp_msg->u.v3.authKey);
            if (lk == NULL) {
                DPRINTF((APERROR, "%s: localized key malloc\n", Fname));
                return -1;
            }
            FreeOctetString(snmp_msg->u.v3.authKey);
            snmp_msg->u.v3.authKey = lk;
            snmp_msg->u.v3.authLocalized = 1;
        }
        HmacHash(snmp_msg->u.v3.authProtocol,
                 snmp_msg->u.v3.authKey->octet_ptr,
                 digest_ptr,
                 snmp_msg->packlet->octet_ptr,
                 snmp_msg->packlet->length);
    }
#endif /* SR_UNSECURABLE */

    return 0;
}

/* ----------------------------------------------------------------------
 *
 *  SrV3ProtocolOIDToInt - Map a Protocol Object ID to an integer type.
 *
 *  Arguments:
 *  i	(const OID *) protocol_int
 *	Protocol object ID to map.
 *
 *  o   (SR_INT32) (function value)
 *      Protocol integer index.
 */

SR_INT32
SrV3ProtocolOIDToInt(
    const OID *protocol_oid)
{
    if (CmpOID(protocol_oid, &usmNoAuthProtocol) == 0) {
        return SR_USM_NOAUTH_PROTOCOL;
    }
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
    if (CmpOID(protocol_oid, &usmHMACMD5AuthProtocol) == 0) {
        return SR_USM_HMAC_MD5_AUTH_PROTOCOL;
    }
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
    if (CmpOID(protocol_oid, &usmHMACSHAAuthProtocol) == 0) {
        return SR_USM_HMAC_SHA_AUTH_PROTOCOL;
    }
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

    if (CmpOID(protocol_oid, &usmNoPrivProtocol) == 0) {
        return SR_USM_NOPRIV_PROTOCOL;
    }
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    if (CmpOID(protocol_oid, &usmDESPrivProtocol) == 0) {
        return SR_USM_DES_PRIV_PROTOCOL;
    }
#ifdef SR_3DES
    if (CmpOID(protocol_oid, &usm3DESPrivProtocol) == 0) {
        return SR_USM_3DES_PRIV_PROTOCOL;
    }
#endif /* SR_3DES */
#ifdef SR_AES
    if (CmpOID(protocol_oid, &usmAesCfb128Protocol) == 0) {
        return SR_USM_AES_CFB_128_PRIV_PROTOCOL;
    }
    if (CmpOID(protocol_oid, &usmAESCfb128PrivProtocol) == 0) {
        return SR_USM_AES_CFB_128_PRIV_PROTOCOL;
    }
    if (CmpOID(protocol_oid, &usmAESCfb192PrivProtocol) == 0) {
        return SR_USM_AES_CFB_192_PRIV_PROTOCOL;
    }
    if (CmpOID(protocol_oid, &usmAESCfb256PrivProtocol) == 0) {
        return SR_USM_AES_CFB_256_PRIV_PROTOCOL;
    }
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    return -1;
}

/* ----------------------------------------------------------------------
 *
 *  SrV3AuthProtocolIntToOID - map an authentication protocol
 *  integer type to an object ID.
 *
 *  Arguments:
 *  i	(SR_INT32) protocol_int
 *	Protocol index to map.  Values might be
 *      SR_USM_NOAUTH_PROTOCOL, SR_USM_HMAC_MD5_AUTH_PROTOCOL or 
 *      SR_USM_HMAC_SHA_AUTH_PROTOCOL
 *
 *  o   (OID *) (function value)
 *	Pointer to Object Identifier for the authentication protocol.
 *	Do not free this value; it is statically allocated.
 *	You should clone the return value (using CloneOID()) before
 *	using it in a context where it might be freed.
 *      If the input argument does not match a known or supported
 *	protocol, NULL will be returned.
 */

OID *
SrV3AuthProtocolIntToOID(
    SR_INT32 protocol_int)
{
    switch (protocol_int) {
        case SR_USM_NOAUTH_PROTOCOL:
            return (OID *)&usmNoAuthProtocol;
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
        case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
            return (OID *)&usmHMACMD5AuthProtocol;
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
        case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
            return (OID *)&usmHMACSHAAuthProtocol;
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */
    }
    return (OID *) NULL;
}


/* ----------------------------------------------------------------------
 *
 *  SrV3PrivProtocolIntToOID - map an privacy protocol
 *  integer type to an object ID.
 *
 *  Arguments:
 *  i	(SR_INT32) protocol_int
 *	Protocol index to map.  Values might be
 *      SR_USM_NOPRIV_PROTOCOL
 *      SR_USM_DES_PRIV_PROTOCOL */
#ifdef SR_3DES
/*      SR_USM_3DES_PRIV_PROTOCOL  */
#endif /* SR_3DES */
#ifdef SR_AES
/*      SR_USM_AES_CFB_128_PRIV_PROTOCOL
 *      SR_USM_AES_CFB_192_PRIV_PROTOCOL
 *      SR_USM_AES_CFB_256_PRIV_PROTOCOL. */
#endif /* SR_AES */
/*
 *  o   (OID *) (function value)
 *	Pointer to Object Identifier for the privacy protocol.
 *	Do not free this value; it is statically allocated.
 *	You should clone the return value (using CloneOID()) before
 *	using it in a context where it might be freed.
 *      If the input argument does not match a known or supported
 *	protocol, NULL will be returned.
 */
OID *
SrV3PrivProtocolIntToOID(
    SR_INT32 protocol_int)
{
    switch (protocol_int) {
        case SR_USM_NOPRIV_PROTOCOL:
            return (OID *)&usmNoPrivProtocol;
	    /* break; */
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
        case SR_USM_DES_PRIV_PROTOCOL:
            return (OID *)&usmDESPrivProtocol;
            /* break; */
#ifdef SR_3DES
        case SR_USM_3DES_PRIV_PROTOCOL:
            return (OID *)&usm3DESPrivProtocol;
            /* break; */
#endif /* SR_3DES */
#ifdef SR_AES
        case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
            return (OID *)&usmAesCfb128Protocol;
            /* break; */
        case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
            return (OID *)&usmAESCfb192PrivProtocol;
            /* break; */
        case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
            return (OID *)&usmAESCfb256PrivProtocol;
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }

    return NULL;
}

#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
/* 
 * SrAssign3DESKeys:
 *
 * This routine takes an SNMPv3 message and a usm user entry and separates
 * a 32 byte 3DES key into the 3 keys needed to do 3DES decryption/encryption.
 *
 * Returns:
 *		0 - success
 *	       -1 - failure
 *
 */
static SR_INT32
SrAssign3DESKeys(SnmpMessage *snmp_msg, usmUserEntry_t *uue)
{
    FNAME("SrAssign3DESKeys")
    SR_INT32 status = 0;

    /*
     * Put 3 3DES keys in own memory location for future
     * use.
     */
    uue->key1 =
         MakeOctetString(NULL, SR_3DES_EACH_KEY_LENGTH * 2);
    uue->key2 =
         MakeOctetString(NULL, SR_3DES_EACH_KEY_LENGTH * 2);
    uue->key3 =
         MakeOctetString(NULL, SR_3DES_EACH_KEY_LENGTH * 2);
    if ( (uue->key1 == NULL) || (uue->key2 == NULL) ||
         (uue->key3 == NULL) ) {
        DPRINTF((APERROR,
                  "%s: key1, key2, or key3 malloc failure\n", Fname));
        status = -1;
        goto done;
    }
    memcpy(uue->key1->octet_ptr, snmp_msg->u.v3.privKey->octet_ptr,
           SR_3DES_EACH_KEY_LENGTH);
    memcpy(uue->key1->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
           snmp_msg->u.v3.privKey->octet_ptr + (SR_3DES_EACH_KEY_LENGTH * 3),
           SR_3DES_EACH_KEY_LENGTH);
    memcpy(uue->key2->octet_ptr,
           snmp_msg->u.v3.privKey->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
           SR_3DES_EACH_KEY_LENGTH);
    memcpy(uue->key2->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
           snmp_msg->u.v3.privKey->octet_ptr + (SR_3DES_EACH_KEY_LENGTH * 3),
           SR_3DES_EACH_KEY_LENGTH);
    memcpy(uue->key3->octet_ptr,
           snmp_msg->u.v3.privKey->octet_ptr + (SR_3DES_EACH_KEY_LENGTH * 2),
           SR_3DES_EACH_KEY_LENGTH);
    memcpy(uue->key3->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
           snmp_msg->u.v3.privKey->octet_ptr + (SR_3DES_EACH_KEY_LENGTH * 3),
           SR_3DES_EACH_KEY_LENGTH);

done:
    return status;

}
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
