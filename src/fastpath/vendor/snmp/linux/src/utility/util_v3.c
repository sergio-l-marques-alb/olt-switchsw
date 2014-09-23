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

/*
 * this file contains code to deal with usec-specific initialization and
 * communication
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <string.h>

#include <ctype.h>

#include <malloc.h>

#include <stddef.h>

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_boots.h"
#include "sr_user.h"
#include "sr_cfg.h"
#ifndef SR_UNSECURABLE
#include "pw2key.h"
#endif /* SR_UNSECURABLE */
#include "sr_utils.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpio.h"
#include "prnt_lib.h"
#include "oid_lib.h"

#include "sr_trans.h"
#include "sr_msg.h"
#include "snmpv3/v3type.h"
#include "snmpv3/v3supp.h"
#include "snmpv3/v3defs.h"
#include "v3_msg.h"


/* globals in snmpio.c */
extern int fd;
extern SR_INT32 packet_len;
extern unsigned char *packet;

extern OctetString *util_local_snmpID;
extern OctetString *defaultContextSnmpID;
extern OctetString *defaultContext;

extern SnmpLcd snmp_lcd;

SnmpV3Lcd util_snmp_v3_lcd;

#include "snmpv3/i_arch.h"
snmpEngine_t util_snmpEngineData;
#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE util_snmpEngineBootsRecord = {
    PARSER_SCALAR,
    "snmpEngineBoots ",
    snmpEngineBootsConverters,
    snmpEngineBootsType,
    NULL,
    &util_snmpEngineData.snmpEngineBoots,
    -1,
    -1,
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */

#include "snmpv3/i_mpd.h"
snmpMPDStats_t util_snmpMPDStatsData;

#ifdef SR_USER_SEC_MODEL
#include "snmpv3/i_usm.h"
usmStats_t util_usmStatsData;
usmUser_t util_usmUserData;
SnmpV2Table util_usmUserTable;
#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE util_usmUserEntryRecord = {
    PARSER_TABLE,
    "usmUserEntry ",
    usmUserEntryConverters,
    usmUserEntryTypeTable,
    &util_usmUserTable,
    NULL,
    offsetof(usmUserEntry_t, usmUserStatus),
    offsetof(usmUserEntry_t, usmUserStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)usmUserEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */
#endif /* SR_USER_SEC_MODEL */

OctetString unknown_snmpID = { NULL, 0 };
OctetString unknown_contextName = { NULL, 0 };
#ifndef SR_UNSECURABLE
OctetString unknown_authKey = { NULL, 0 };
#ifndef SR_NO_PRIVACY
OctetString unknown_privKey = { NULL, 0 };
#ifdef SR_3DES
OctetString unknown_key1 = { NULL, 0 };
OctetString unknown_key2 = { NULL, 0 };
OctetString unknown_key3 = { NULL, 0 };
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
usmUserEntry_t unknown_cluce = {
  &unknown_snmpID,
  NULL,
  NULL,
  NULL,
  (OID *)&usmNoAuthProtocol,
  NULL,
  NULL,
  (OID *)&usmNoPrivProtocol,
  NULL,
  NULL,
  NULL,
  (SR_INT32)0,
  (SR_INT32)0,
  NULL,
  (long)0,
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
  &unknown_authKey,
  &unknown_privKey,
#ifdef SR_3DES
  (SR_INT32)0,
  &unknown_key1,
  &unknown_key2,
  &unknown_key3,
#endif /* SR_3DES */
#else /* SR_NO_PRIVACY */
  &unknown_authKey,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
  "\0\0"
};

OctetString *v3_defaultContext;
#ifndef SR_UNSECURABLE
extern int inform_type;
unsigned short port_num = 21;
#endif /* SR_UNSECURABLE */

#ifdef SR_SNMPv3_PACKET
extern int alt_engineID;
#endif /* SR_SNMPv3_PACKET */





int
InitV3CLU(
    const PARSER_RECORD_TYPE *rt[],
    int *rt_count)
{
    v3_defaultContext = MakeOctetString(NULL, 0);

    util_snmp_v3_lcd.snmpEngineData = &util_snmpEngineData;
    util_snmp_v3_lcd.snmpMPDStatsData = &util_snmpMPDStatsData;
    util_snmp_v3_lcd.usmStatsData = &util_usmStatsData;
    util_snmp_v3_lcd.usmUserData = &util_usmUserData;
    util_snmp_v3_lcd.usmUserTable = &util_usmUserTable;
    util_snmp_v3_lcd.usm_salt =
        ((SR_UINT32)&snmp_lcd) * ((SR_UINT32)GetTimeNow());
    util_snmp_v3_lcd.override_userSecurityName = NULL;
#ifndef SR_UNSECURABLE
    util_snmp_v3_lcd.override_auth_secret = NULL;
#ifndef SR_NO_PRIVACY
    util_snmp_v3_lcd.override_priv_secret = NULL;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    snmp_lcd.versions_supported[snmp_lcd.num_vers] = SR_SNMPv3_VERSION;
    snmp_lcd.lcds[snmp_lcd.num_vers] = (void *)&util_snmp_v3_lcd;
    snmp_lcd.parse_functions[snmp_lcd.num_vers] = SrParseV3SnmpMessage;
    snmp_lcd.build_functions[snmp_lcd.num_vers] = SrBuildV3SnmpMessage;
    snmp_lcd.num_vers++;

    snmp_lcd.vta_data = NULL;
    snmp_lcd.vta_function = NULL;

    i_arch_initialize(util_local_snmpID, &util_snmpEngineData);

    i_mpd_initialize(&util_snmpMPDStatsData);

    i_usm_initialize(&util_usmStatsData,
                     &util_usmUserData,
                     &util_usmUserTable);

#ifdef SR_CONFIG_FP
    rt[(*rt_count)++] = &util_snmpEngineBootsRecord;
    rt[(*rt_count)++] = &util_usmUserEntryRecord;
#endif /* SR_CONFIG_FP */

    return 1;
}

#ifndef SR_UNSECURABLE
int
PromptForV3UserPasswords(snmpID, userName)
    OctetString *snmpID;
    OctetString *userName;
{
    int index;
    usmUserEntry_t *uue;
    SR_INT32 authProtocol = SR_USM_NOAUTH_PROTOCOL;
    SR_INT32 privProtocol = SR_USM_NOPRIV_PROTOCOL;
    char pw1[128];
    char *pw1_env;
#ifndef SR_NO_PRIVACY
    char pw2[128];
    char *pw2_env;
#if (defined(SR_3DES) || defined(SR_AES))
    OctetString *key1 = NULL, *key2 = NULL;
    SR_INT32 remainderLen;
    unsigned char *tmp_ptr;
#endif	/* (defined(SR_3DES) || defined(SR_AES)) */
#endif /* SR_NO_PRIVACY */
    SR_INT32 algorithm = 0;

    strcpy(pw1, "");
#ifndef SR_NO_PRIVACY
    strcpy(pw2, "");
#endif /* SR_NO_PRIVACY */

    pw1_env = getenv("SR_UTIL_AUTH_PASSWORD");
    if (pw1_env != NULL) {
        strcpy(pw1, pw1_env);
#ifndef SR_NO_PRIVACY
        pw2_env = getenv("SR_UTIL_PRIV_PASSWORD");
        if (pw2_env != NULL) {
            strcpy(pw2, pw2_env);
        }
#endif /* SR_NO_PRIVACY */
    } else {
        get_password("Enter Authentication password : ", pw1, sizeof(pw1));
#ifndef SR_NO_PRIVACY
        if (strlen(pw1) > 0) {
            get_password("Enter Privacy password        : ", pw2, sizeof(pw2));
        }
#endif /* SR_NO_PRIVACY */
    }

    if (strlen(pw1) > 0) {
        authProtocol = SR_USM_HMAC_MD5_AUTH_PROTOCOL;
        algorithm = SR_PW2KEY_ALGORITHM_MD5;
        if (strlen(pw1) >= 5) {
            if (strncmp(pw1, "MD5:", 4) == 0) {
                authProtocol = SR_USM_HMAC_MD5_AUTH_PROTOCOL;
                algorithm = SR_PW2KEY_ALGORITHM_MD5;
                memmove(pw1, pw1 + 4, strlen(pw1) - 3);
            }
#ifdef SR_SHA_HASH_ALGORITHM
            else if (strncmp(pw1, "SHA:", 4) == 0) {
                authProtocol = SR_USM_HMAC_SHA_AUTH_PROTOCOL;
                algorithm = SR_PW2KEY_ALGORITHM_SHA;
                memmove(pw1, pw1 + 4, strlen(pw1) - 3);
            }
#endif /* SR_SHA_HASH_ALGORITHM */
        }
    }
#ifndef SR_NO_PRIVACY
    if (strlen(pw2) > 0) {
        privProtocol = SR_USM_DES_PRIV_PROTOCOL;
        if (strlen(pw2) >= 5) {
            if (strncmp(pw2, "DES:", 4) == 0) {
                privProtocol = SR_USM_DES_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 4, strlen(pw2) - 3);
            }
#ifdef SR_3DES
            else if ( (strlen(pw2) >= 6) && (strncmp(pw2, "3DES:", 5) == 0)) {
                privProtocol = SR_USM_3DES_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 5, strlen(pw2) - 4);
            }
#endif /* SR_3DES */
#ifdef SR_AES
            else if ((strlen(pw2) >= 8) && (strncmp(pw2, "AES128:", 7) == 0)) {
                privProtocol = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 7, strlen(pw2) - 6);
            }
            else if ((strlen(pw2) >= 8) && (strncmp(pw2, "AES192:", 7) == 0)) {
                privProtocol = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 7, strlen(pw2) - 6);
            }
            else if ((strlen(pw2) >= 8) && (strncmp(pw2, "AES256:", 7) == 0)) {
                privProtocol = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 7, strlen(pw2) - 6);
            }
            else if (strncmp(pw2, "AES:", 4) == 0) {
                privProtocol = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
                memmove(pw2, pw2 + 4, strlen(pw2) - 3);
            }
#endif /* SR_AES */
        }
    }
#endif /* SR_NO_PRIVACY */

    util_usmUserTable.tip[0].value.octet_val = snmpID;
    util_usmUserTable.tip[1].value.octet_val = userName;
    index = NewTableEntry(&util_usmUserTable);
    if (index == -1) {
        return -1;
    }
    uue = util_usmUserTable.tp[index];
    uue = (usmUserEntry_t *) util_usmUserTable.tp[index];
    uue->usmUserSecurityName = MakeOctetString(NULL, 0);
    uue->usmUserAuthProtocol = SrV3AuthProtocolIntToOID(authProtocol);
    uue->usmUserPrivProtocol = SrV3PrivProtocolIntToOID(privProtocol);
    if (strlen(pw1) == 0) {
        uue->auth_secret = MakeOctetString(NULL, 0);
#ifndef SR_NO_PRIVACY
        uue->priv_secret = MakeOctetString(NULL, 0);
#endif /* SR_NO_PRIVACY */
    } else {
        uue->auth_secret =
            SrTextPasswordToLocalizedKey(algorithm, snmpID, pw1);
#ifndef SR_NO_PRIVACY
        if (strlen(pw2) == 0) {
            uue->priv_secret = MakeOctetString(NULL, 0);
        }
        else {
            switch (privProtocol) {
                case SR_USM_DES_PRIV_PROTOCOL:
                    uue->priv_secret =
                        SrTextPasswordToLocalizedKey(algorithm, snmpID, pw2);
                    break;
#ifdef SR_3DES
                case SR_USM_3DES_PRIV_PROTOCOL:
                    uue->priv_secret = MakeOctetString(NULL, SR_3DES_KEY_LENGTH);
                    if (uue->priv_secret != NULL) {
                        key1 =
                            SrTextPasswordToLocalizedKey(algorithm, snmpID,
                                                         pw2);
                        if (key1 != NULL) {
                            key2 = 
                                SrOctetStringPasswordToLocalizedKey(algorithm, 
                                                                    snmpID, 
                                                                    key1);
                            if (key2 != NULL) {
                                memcpy(uue->priv_secret->octet_ptr,
                                       key1->octet_ptr, key1->length);
                                remainderLen = SR_3DES_KEY_LENGTH - key1->length;
                                tmp_ptr = uue->priv_secret->octet_ptr + key1->length;
                                memcpy(tmp_ptr,
                                       key2->octet_ptr,
                                       remainderLen);
                                FreeOctetString(key1);
                                FreeOctetString(key2);
                            }
                            else {
                                FreeOctetString(key1);
                                FreeOctetString(uue->priv_secret);
                                uue->priv_secret = NULL;
                            }
                        }
                        else {
                            FreeOctetString(uue->priv_secret);
                            uue->priv_secret = NULL;
                        }
                    }
                    break;
#endif /* SR_3DES */
#ifdef SR_AES
                case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
                case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
                case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
                    uue->priv_secret = MakeOctetString(NULL, SR_AES_KEY_LENGTH);
                    if (uue->priv_secret != NULL) {
                        key1 =
                            SrTextPasswordToLocalizedKey(algorithm, snmpID,
                                                         pw2);
                        if (key1 != NULL) {
                            key2 = 
                                SrOctetStringPasswordToLocalizedKey(algorithm, 
                                                                    snmpID, 
                                                                    key1);
                            if (key2 != NULL) {
                                memcpy(uue->priv_secret->octet_ptr,
                                       key1->octet_ptr, key1->length);
                                remainderLen = SR_AES_KEY_LENGTH - key1->length;
                                tmp_ptr = uue->priv_secret->octet_ptr + key1->length;
                                memcpy(tmp_ptr,
                                       key2->octet_ptr,
                                       remainderLen);
                                FreeOctetString(key1);
                                FreeOctetString(key2);
                                /*
                                 * Adjust length for AES128 and AES192
                                 */
                                if (privProtocol == SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
                                    uue->priv_secret->length = 
                                          SR_AES_128_KEY_LENGTH;
                                }
                                if (privProtocol == SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
                                    uue->priv_secret->length = 
                                          SR_AES_192_KEY_LENGTH;
                                }
                            }
                            else {
                                FreeOctetString(key1);
                                FreeOctetString(uue->priv_secret);
                                uue->priv_secret = NULL;
                            }
                        }
                        else {
                            FreeOctetString(uue->priv_secret);
                            uue->priv_secret = NULL;
                        }
                    }
                    break;
#endif /* SR_AES */
                default:
                    break;
            }
        }
#endif /* SR_NO_PRIVACY */
    }
    uue->usmUserStorageType = D_usmUserStorageType_volatile;
    uue->usmUserStatus = D_usmUserStatus_active;

    return index;
}
#else /* SR_UNSECURABLE */
int
CreateV3NoAuthUser(snmpID, userName)
    OctetString *snmpID;
    OctetString *userName;
{
    int index;
    usmUserEntry_t *uue;

    util_usmUserTable.tip[0].value.octet_val = snmpID;
    util_usmUserTable.tip[1].value.octet_val = userName;
    index = NewTableEntry(&util_usmUserTable);
    if (index == -1) {
        return -1;
    }
    uue = (usmUserEntry_t *) util_usmUserTable.tp[index];
    uue->usmUserSecurityName = MakeOctetString(NULL, 0);
    uue->usmUserAuthProtocol = (OID *)&usmNoAuthProtocol;
    uue->usmUserPrivProtocol = (OID *)&usmNoPrivProtocol;
    uue->usmUserStorageType = D_usmUserStorageType_volatile;
    uue->usmUserStatus = D_usmUserStatus_active;

    return index;
}
#endif /* SR_UNSECURABLE */

usmUserEntry_t *v3_cluce;
OctetString *v3_handle = NULL;

SR_UINT32 notInTimeWindow_sids[] =   { 1, 3, 6, 1, 6, 3, 15, 1, 1, 2, 0 };
SR_UINT32 unknownSnmpEngineIDs_sids[] = { 1, 3, 6, 1, 6, 3, 15, 1, 1, 4, 0 };
OID notInTimeWindow_OID = { 11, notInTimeWindow_sids };
OID unknownSnmpEngineIDs_OID = { 11, unknownSnmpEngineIDs_sids };
OID *notInTimeWindow = &notInTimeWindow_OID;
OID *unknownSnmpEngineIDs = &unknownSnmpEngineIDs_OID;
Pdu *v3_fake_pdu;

int
InitializeV3User(input, communication_type)
    char *input;
    int communication_type;
{
    int index;
    OID *oid;
    VarBind *vb;

    /*
     * For now, the v3_handle will be treated as the userName
     */
    v3_handle = MakeOctetStringFromText(input);
    if (v3_handle == NULL) {
        return 0;
    }

    switch (communication_type) {
    case SNMP_COMMUNICATIONS:
        v3_cluce = &unknown_cluce;
        unknown_cluce.usmUserName = v3_handle;
        break;

    case TRAP_SEND_COMMUNICATIONS:
        /* Need to search util_userNameTable for localSnmpID/v3_handle, 
         * if there, use the keys, otherwise, immediately prompt for the keys. 
         */
        util_usmUserTable.tip[0].value.octet_val = util_local_snmpID;
        util_usmUserTable.tip[1].value.octet_val = v3_handle;
        index = SearchTable(&util_usmUserTable, EXACT);
        if (index == -1) {
            DPRINTF((APCONFIG, "No local config info for this user.\n"));
#ifndef SR_UNSECURABLE
            index = PromptForV3UserPasswords(util_local_snmpID, v3_handle);
#else /* SR_UNSECURABLE */
            index = CreateV3NoAuthUser(util_local_snmpID, v3_handle);
#endif /* SR_UNSECURABLE */
            if (index == -1) {
                return 0;
            }
        }
        v3_cluce = (usmUserEntry_t *)util_usmUserTable.tp[index];
        break;

    case TRAP_RCV_COMMUNICATIONS:
        v3_cluce = NULL;
        break;

    case INFORM_COMMUNICATIONS:
        v3_cluce = &unknown_cluce;
        unknown_cluce.usmUserName = v3_handle;
        break;
    } 			/* switch(communications_type) */


    oid = MakeOIDFromDot("1.3.6.1.2.1.1.3");
    if (oid == NULL) { 
        return 0;
    }
    vb = MakeVarBindWithNull(oid, NULL);
    if (vb == NULL) { 
        return 0;
    }
    FreeOID(oid);
    FreeVarBind(vb);
    v3_fake_pdu = MakePdu(GET_NEXT_REQUEST_TYPE, 0L,0L,0L,NULL,NULL,0L,0L,0L);
    if (v3_fake_pdu == NULL) { 
        return 0;
    }

    return 1;
}




Pdu *
PerformV3UserRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    SnmpMessage *snmp_msg = NULL;
    Pdu *in_pdu_ptr = NULL;
    SR_INT32 secLevel;
    int index;
    int use_fake_pdu = 0;
    int i;

    *status = 0;


    /*
     * Attempt to send the pdu and process the response, using the
     * parameters initialized by InitializeV3User(), and possibly updated
     * by previous calls to PerformV3UserRequest().  If a Report PDU is
     * received as the response, these parameters will be updated
     * appropriately, and the request will be re-built and re-sent.
     * This occurs by jumping back to the build_again label.
     */

  build_again:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
    }
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    /* Build the snmp_msg */
    if ( (v3_cluce == &unknown_cluce) && 
         (communication_type != TRAP_SEND_COMMUNICATIONS) ) {
        secLevel = SR_SECURITY_LEVEL_NOAUTH;
        use_fake_pdu = 1;
    } else {
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
        secLevel = SR_SECURITY_LEVEL_PRIV;
        if (v3_cluce->priv_secret == NULL) {
            secLevel = SR_SECURITY_LEVEL_AUTH;
        } else if (v3_cluce->priv_secret->length == 0) {
            secLevel = SR_SECURITY_LEVEL_AUTH;
	}
#else /* !SR_NO_PRIVACY */
        secLevel = SR_SECURITY_LEVEL_AUTH;
#endif /* !SR_NO_PRIVACY */
        if (v3_cluce->auth_secret == NULL) {
            secLevel = SR_SECURITY_LEVEL_NOAUTH;
        } else if (v3_cluce->auth_secret->length == 0) {
            secLevel = SR_SECURITY_LEVEL_NOAUTH;
	}
#else /* !SR_UNSECURABLE */
        secLevel = SR_SECURITY_LEVEL_NOAUTH;
#endif /* !SR_UNSECURABLE */
    }
    snmp_msg = SrCreateV3SnmpMessage(secLevel, 3,
                           (communication_type == TRAP_SEND_COMMUNICATIONS) ?
                               ((out_pdu_ptr->type == INFORM_REQUEST_TYPE) ?
                                     v3_cluce->usmUserEngineID : util_local_snmpID) :
                                     v3_cluce->usmUserEngineID,
                           v3_cluce->usmUserName,
                           (defaultContextSnmpID == NULL) ?
                               v3_cluce->usmUserEngineID : defaultContextSnmpID,
                           (defaultContext == NULL) ?
                               v3_defaultContext : defaultContext,
#ifndef SR_UNSECURABLE
                           v3_cluce->auth_secret,
                           SrV3ProtocolOIDToInt(v3_cluce->usmUserAuthProtocol),
                           1,
#ifndef SR_NO_PRIVACY
                           v3_cluce->priv_secret,
                           SrV3ProtocolOIDToInt(v3_cluce->usmUserPrivProtocol),
                           1,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                           TRUE);
    if (snmp_msg == NULL) {
        *status = SR_ERROR;
        goto done;
    }
    if (use_fake_pdu) {
        v3_fake_pdu->u.normpdu.request_id = out_pdu_ptr->u.normpdu.request_id;
        if (BuildPdu(v3_fake_pdu) == -1) {
          DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
          *status = SR_ERROR;
          goto done;
        }
    }
    if (SrBuildSnmpMessage(snmp_msg,
                           use_fake_pdu ? v3_fake_pdu : out_pdu_ptr,
                           &snmp_lcd)) {
        *status = SR_ERROR;
        goto done;
    }
    use_fake_pdu = 0;

    /* Send the request */
    if (SrSendRequest(fd,
                      snmp_msg->packlet->octet_ptr,
                      snmp_msg->packlet->length) != TRUE) {
        *status = SR_ERROR;
        goto done;
    }

#ifdef SR_SNMPv1_PACKET
    if (out_pdu_ptr->type == TRAP_TYPE) {
        goto done;
    }
    else if (out_pdu_ptr->type == SNMPv2_TRAP_TYPE) { 
#ifdef SR_SNMPv3_PACKET
#ifndef SR_UNSECURABLE
	if ((alt_engineID == TRUE) && (inform_type == SEND_TRAP)) {
            printf("Sent SNMPv3 Trap with altered snmpEngineID : ");
            if (util_local_snmpID->length > 0) {
                printf("%02x", util_local_snmpID->octet_ptr[0]);
            }
            for (i = 1; i < util_local_snmpID->length; i++) {
                printf(":%02x", util_local_snmpID->octet_ptr[i]);
            }
            printf("\n");
        }
#endif /* SR_UNSECURABLE */
#endif /* SR_SNMPv3_PACKET */
        goto done;
    }

#else /* SR_SNMPv1_PACKET */
    if (out_pdu_ptr->type == SNMPv2_TRAP_TYPE) { 
#ifdef SR_SNMPv3_PACKET
#ifndef SR_UNSECURABLE
	if ((alt_engineID == TRUE) && (inform_type == SEND_TRAP)) {
            printf("Sent SNMPv3 Trap with altered snmpEngineID : ");
            if (util_local_snmpID->length > 0) {
                printf("%02x", util_local_snmpID->octet_ptr[0]);
            }
            for (i = 1; i < util_local_snmpID->length; i++) {
                printf(":%02x", util_local_snmpID->octet_ptr[i]);
            }
            printf("\n");
        }
#endif /* SR_UNSECURABLE */
#endif /* SR_SNMPv3_PACKET */
        goto done;
    }
#endif /* SR_SNMPv1_PACKET */

    /* Wait for response */
  wait_again:
    *status = GetResponse();
    if (*status == SR_ERROR) {
        DPRINTF((APERROR, "%s:  receive error.\n", util_name));
        goto done;
    }
    if (*status == SR_TIMEOUT) {
        printf("\nno response.\n");
        goto done;
    }

    SrFreeSnmpMessage(snmp_msg);
    snmp_msg = NULL;

    snmp_msg = SrParseSnmpMessage(&snmp_lcd,
                                  NULL,
                                  NULL,
                                  packet,
                                  (int)packet_len);
    if (snmp_msg == NULL) {
        DPRINTF((APERROR, "%s:  error parsing packet.\n",
                 util_name));
        *status = SR_ERROR;
        goto done;
    }
    switch (snmp_msg->error_code) {
        case ASN_PARSE_ERROR:
            DPRINTF((APERROR, "ASN parse error in incoming message!\n"));
            *status = SR_ERROR;
            goto done;
        case SR_USM_NOT_IN_TIME_WINDOWS:
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
        case SR_USM_WRONG_DIGESTS:
            DPRINTF((APERROR, "Error parsing packet, digest mismatch\n"));
            *status = SR_ERROR;
            goto done;
        case SR_USM_UNKNOWN_USER_NAMES:
            if (!CmpOctetStrings(snmp_msg->u.v3.authSnmpEngineID,
                                 v3_cluce->usmUserEngineID)) {
                DPRINTF((APERROR, "Username %s is unknown to this agent.\n",
                         util_handle));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            util_usmUserTable.tip[0].value.octet_val =
                snmp_msg->u.v3.authSnmpEngineID;
            util_usmUserTable.tip[1].value.octet_val = v3_handle;
            index = SearchTable(&util_usmUserTable, EXACT);
            if (index == -1) {
                DPRINTF((APCONFIG, "No local config info for this agent.\n"));
#ifndef SR_UNSECURABLE
                index =
                    PromptForV3UserPasswords(snmp_msg->u.v3.authSnmpEngineID,
                                             v3_handle);
#else /* SR_UNSECURABLE */
                index =
                    CreateV3NoAuthUser(snmp_msg->u.v3.authSnmpEngineID,
                                     v3_handle);
#endif /* SR_UNSECURABLE */
                if (index == -1) {
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    *status = SR_ERROR;
                    goto done;
                }
            }
            v3_cluce = (usmUserEntry_t *)util_usmUserTable.tp[index];
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
    }

    /* Parse the PDU */
    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        DPRINTF((APERROR, "%s:  error parsing pdu packlet.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    /* Check if this is a report PDU, and process appropriately */
    if (in_pdu_ptr->type == REPORT_TYPE) {

        /* If out-of-lifetime, record the new times and try again */
        if (CmpOID(notInTimeWindow, in_pdu_ptr->var_bind_list->name) == 0) {
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            if (secLevel != SR_SECURITY_LEVEL_PRIV) {
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                if (in_pdu_ptr->u.normpdu.request_id !=
                        out_pdu_ptr->u.normpdu.request_id) {
                    DPRINTF((APALWAYS, "request id mismatch.\n"));
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    goto wait_again;
                }
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            DPRINTF((APTRACE, "REPORT received for notInTimeWindow.\n"));
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
        }

        /* If an unknown user, record the authSnmpID and try again */
        if (CmpOID(unknownSnmpEngineIDs, in_pdu_ptr->var_bind_list->name) == 0) {
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            if (secLevel != SR_SECURITY_LEVEL_PRIV) {
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                if (in_pdu_ptr->u.normpdu.request_id !=
                    out_pdu_ptr->u.normpdu.request_id) {
                    DPRINTF((APALWAYS, "request id mismatch.\n"));
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    goto wait_again;
                }
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            DPRINTF((APTRACE, "REPORT received for unknown engine ID.\n"));
            if (snmp_msg->u.v3.authSnmpEngineID == NULL) {
                DPRINTF((APERROR, "bad authSnmpID in REPORT.\n"));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            if (!CmpOctetStrings(snmp_msg->u.v3.authSnmpEngineID, v3_cluce->usmUserEngineID)) {
                DPRINTF((APERROR, "Username %s is unknown to this agent.\n",
                         util_handle));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            util_usmUserTable.tip[0].value.octet_val = snmp_msg->u.v3.authSnmpEngineID;
            util_usmUserTable.tip[1].value.octet_val = v3_handle;
            index = SearchTable(&util_usmUserTable, EXACT);
            if (index == -1) {
                DPRINTF((APCONFIG, "No local config info for this agent.\n"));
#ifndef SR_UNSECURABLE
                index = PromptForV3UserPasswords(snmp_msg->u.v3.authSnmpEngineID, v3_handle);
#else /* SR_UNSECURABLE */
                index = CreateV3NoAuthUser(snmp_msg->u.v3.authSnmpEngineID, v3_handle);
#endif /* SR_UNSECURABLE */
                if (index == -1) {
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    *status = SR_ERROR;
                    goto done;
                }
            }
            v3_cluce = (usmUserEntry_t *)util_usmUserTable.tp[index];
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
        }

        /* Otherwise we cannot recover */
        DPRINTF((APERROR, "REPORT received, cannot recover:\n"));
        PrintVarBindList(in_pdu_ptr->var_bind_list);
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        *status = SR_ERROR;
        goto done;
    }

    /* check pdu type to verify response type pdu */
   
    switch (in_pdu_ptr->type) {
       case GET_RESPONSE_TYPE:
           break;
       default:
           DPRINTF((APALWAYS, 
                    "%s: error parsing PDU, received unexpected PDU type %d\n",
                    util_name, in_pdu_ptr->type));
            FreePdu(in_pdu_ptr);
            in_pdu_ptr = NULL;
            *status = SR_ERROR;
            goto done;
    }


    /* Make sure request id matches */
    if (in_pdu_ptr->u.normpdu.request_id != out_pdu_ptr->u.normpdu.request_id) {
        DPRINTF((APALWAYS, "request id mismatch.\n"));
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        goto wait_again;
    }

    /* If a response PDU was received, just drop through and return the pdu */

  done:
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    return in_pdu_ptr;
}

/*
 * SetUtilTrapFlag()
 *
 * set the UtilTrapFlag so that this will
 *    1. change util_local_snmpID to use port number 21(trap)
 *       or 22(inform) instead of using snmpTrapPort.
 *    2. change authsnmpEngineTime in trap message to use time of day.
 *
 * return VOID
 */

void SetUtilTrapFlag(int type, unsigned short port)
{
#ifndef SR_UNSECURABLE
   inform_type = type;
   port_num = port;
#endif /* SR_UNSECURABLE */
   return ;
}
