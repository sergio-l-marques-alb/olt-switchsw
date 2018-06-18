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

#include <errno.h>

#include <sys/types.h>





#include <string.h>

#include <malloc.h>


#include <netinet/in.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "sr_msg.h"

#include "diag.h"
SR_FILENAME

#include "snmpv3/v3type.h"
#include "v3_msg.h"


#ifndef SR_UNSECURABLE
#include "pw2key.h"
#endif /* SR_UNSECURABLE */

#include "sr_ip.h"    /* for GetLocalIPAddress() */



#ifdef SR_CONFIG_FP

static const ParserIntegerTranslation contextMatch_Translations[] = {
    { "exact", 1 },
    { "prefix", 2 },
    { "1", 1 },
    { "2", 2 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable contextMatch_TranslationTable = {
    INTEGER_TYPE,
    "contextMatch",
    16,
    contextMatch_Translations
};

static const ParserIntegerTranslation securityLevel_Translations[] = {
    { "noAuthNoPriv", SR_SECURITY_LEVEL_NOAUTH },
    { "1", SR_SECURITY_LEVEL_NOAUTH },
#ifndef SR_UNSECURABLE
    { "authNoPriv", SR_SECURITY_LEVEL_AUTH },
    { "2", SR_SECURITY_LEVEL_AUTH },
#ifndef SR_NO_PRIVACY
    { "authPriv", SR_SECURITY_LEVEL_PRIV },
    { "3", SR_SECURITY_LEVEL_PRIV },
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    { NULL, 0 }
};
const ParserIntegerTranslationTable securityLevel_TranslationTable = {
    INTEGER_TYPE,
    "securityLevel",
    16,
    securityLevel_Translations
};

static const ParserIntegerTranslation securityModel_Translations[] = {
    { "snmpv1", SR_SECURITY_MODEL_V1 },
    { "1", SR_SECURITY_MODEL_V1 },
    { "snmpv2c", SR_SECURITY_MODEL_V2C },
    { "2", SR_SECURITY_MODEL_V2C },
    { "usm", SR_SECURITY_MODEL_USM },
    { "3", SR_SECURITY_MODEL_USM },
#ifdef SR_EPIC
    { "epic", SR_SECURITY_MODEL_EPIC },
    { "25346", SR_SECURITY_MODEL_EPIC },
    { "cli", SR_SECURITY_MODEL_CLI },
    { "25348", SR_SECURITY_MODEL_CLI },
    { "xml", SR_SECURITY_MODEL_XML },
    { "25347", SR_SECURITY_MODEL_XML },
#endif /* SR_EPIC */
#ifdef SR_EMWEB_EPIC
    { "emweb", SR_SECURITY_MODEL_EMWEB_EPIC },
    { "973057", SR_SECURITY_MODEL_EMWEB_EPIC },
    { "emwebepic", SR_SECURITY_MODEL_EMWEB_EPIC },
#endif /* SR_EMWEB_EPIC */
    { NULL, 0 }
};
const ParserIntegerTranslationTable securityModel_TranslationTable = {
    INTEGER_TYPE,
    "securityModel",
    16,
    securityModel_Translations
};


/*
 * The following ConvToken routines are used by the parser code to convert
 * data types specific to SNMPv3.
 */

#ifndef SR_UNSECURABLE
extern OctetString *ConvToken_snmpID_prevSnmpID;
static SR_INT32 prev_localization_algorithm;
#endif /* SR_UNSECURABLE */

#ifndef SR_NO_PRIVACY
static SR_INT32 cur_PrivType = -1;
#endif /* SR_NO_PRIVACY */

SR_INT32
ConvToken_authProtocol(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OID **local = (OID **)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = NULL;
            if (strcmp(*token, "usmNoAuthProtocol") == 0) {
                *local = CloneOID(&usmNoAuthProtocol);
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.1.1") == 0) {
                *local = CloneOID(&usmNoAuthProtocol);
            }
#endif	/* LIGHT */
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
            else if (strcmp(*token, "usmHMACMD5AuthProtocol") == 0) {
                *local = CloneOID(&usmHMACMD5AuthProtocol);
                prev_localization_algorithm = SR_PW2KEY_ALGORITHM_MD5;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.1.2") == 0) {
                *local = CloneOID(&usmHMACMD5AuthProtocol);
                prev_localization_algorithm = SR_PW2KEY_ALGORITHM_MD5;
            }
#endif	/* LIGHT */
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
            else if (strcmp(*token, "usmHMACSHAAuthProtocol") == 0) {
                *local = CloneOID(&usmHMACSHAAuthProtocol);
                prev_localization_algorithm = SR_PW2KEY_ALGORITHM_SHA;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.1.3") == 0) {
                *local = CloneOID(&usmHMACSHAAuthProtocol);
                prev_localization_algorithm = SR_PW2KEY_ALGORITHM_SHA;
            }
#endif	/* LIGHT */
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */
            if (*local == NULL) {
                return -1;
            } else {
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if (CmpOID(*local, &usmNoAuthProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmNoAuthProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.1.1");
#endif	/* LIGHT */
                return 0;
            }
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
            if (CmpOID(*local, &usmHMACMD5AuthProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmHMACMD5AuthProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.1.2");
#endif	/* LIGHT */
                return 0;
            }
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
            if (CmpOID(*local, &usmHMACSHAAuthProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmHMACSHAAuthProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.1.3");
#endif	/* LIGHT */
                return 0;
            }
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */
            free(*token);
            *token = NULL;
            return -1;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_privProtocol(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OID **local = (OID **)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = NULL;
            if (strcmp(*token, "usmNoPrivProtocol") == 0) {
                *local = CloneOID(&usmNoPrivProtocol);
#ifndef SR_NO_PRIVACY
                cur_PrivType = SR_USM_NOPRIV_PROTOCOL;
#endif /* SR_NO_PRIVACY */
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.2.1") == 0) {
                *local = CloneOID(&usmNoPrivProtocol);
#ifndef SR_NO_PRIVACY
                cur_PrivType = SR_USM_NOPRIV_PROTOCOL;
#endif /* SR_NO_PRIVACY */
            }
#endif	/* LIGHT */
#ifndef SR_NO_PRIVACY
            else if (strcmp(*token, "usmDESPrivProtocol") == 0) {
                *local = CloneOID(&usmDESPrivProtocol);
                cur_PrivType = SR_USM_DES_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.2.2") == 0) {
                *local = CloneOID(&usmDESPrivProtocol);
                cur_PrivType = SR_USM_DES_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
#ifdef SR_3DES
            else if (strcmp(*token, "usm3DESPrivProtocol") == 0) {
                *local = CloneOID(&usm3DESPrivProtocol);
                cur_PrivType = SR_USM_3DES_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.4.1.14832.1.1") == 0) {
                *local = CloneOID(&usm3DESPrivProtocol);
                cur_PrivType = SR_USM_3DES_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
#endif /* SR_3DES */
#ifdef SR_AES
            else if (strcmp(*token, "usmAesCfb128Protocol") == 0) {
                *local = CloneOID(&usmAesCfb128Protocol);
                cur_PrivType = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.6.3.10.1.2.4") == 0) {
                *local = CloneOID(&usmAesCfb128Protocol);
                cur_PrivType = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
            else if (strcmp(*token, "usmAESCfb128PrivProtocol") == 0) {
                *local = CloneOID(&usmAesCfb128Protocol);
                cur_PrivType = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.4.1.14832.1.2") == 0) {
                *local = CloneOID(&usmAesCfb128Protocol);
                cur_PrivType = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
            else if (strcmp(*token, "usmAESCfb192PrivProtocol") == 0) {
                *local = CloneOID(&usmAESCfb192PrivProtocol);
                cur_PrivType = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.4.1.14832.1.3") == 0) {
                *local = CloneOID(&usmAESCfb192PrivProtocol);
                cur_PrivType = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
            else if (strcmp(*token, "usmAESCfb256PrivProtocol") == 0) {
                *local = CloneOID(&usmAESCfb256PrivProtocol);
                cur_PrivType = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
            }
#ifdef LIGHT
            else if (strcmp(*token, "1.3.6.1.4.1.14832.1.4") == 0) {
                *local = CloneOID(&usmAESCfb256PrivProtocol);
                cur_PrivType = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
            }
#endif	/* LIGHT */
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
            if (*local == NULL) {
                return -1;
            } else {
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if (CmpOID(*local, &usmNoPrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmNoPrivProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.2.1");
#endif	/* LIGHT */
                return 0;
            }
#ifndef SR_NO_PRIVACY
            if (CmpOID(*local, &usmDESPrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmDESPrivProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.2.2");
#endif	/* LIGHT */
                return 0;
            }
#ifdef SR_3DES
            if (CmpOID(*local, &usm3DESPrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usm3DESPrivProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.4.1.14832.1.1");
#endif	/* LIGHT */
                return 0;
            }
#endif /* SR_3DES */
#ifdef SR_AES
            if (CmpOID(*local, &usmAesCfb128Protocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmAesCfb128Protocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.2.4");
#endif	/* LIGHT */
                return 0;
            }
            if (CmpOID(*local, &usmAESCfb128PrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmAesCfb128Protocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.6.3.10.1.2.4");
#endif	/* LIGHT */
                return 0;
            }
            if (CmpOID(*local, &usmAESCfb192PrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmAESCfb192PrivProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.4.1.14832.1.3");
#endif	/* LIGHT */
                return 0;
            }
            if (CmpOID(*local, &usmAESCfb256PrivProtocol) == 0) {
#ifndef LIGHT
                strcpy(*token, "usmAESCfb256PrivProtocol");
#else	/* LIGHT */
                strcpy(*token, "1.3.6.1.4.1.14832.1.4");
#endif	/* LIGHT */
                return 0;
            }
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
            free(*token);
            *token = NULL;
            return -1;
        default:
            return -1;
    }
}


#ifndef SR_UNSECURABLE
SR_INT32
ConvToken_localizedKey(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 pos, len;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            len = (SR_INT32)strlen(*token);
            for (pos = 0; pos < len; pos++) {
                if (((*token)[pos] == ':') ||
                    ((*token)[pos] == '-')) {
                    return ConvToken_octetString(direction, token, value);
                 }
            }
            DPRINTF((APCONFIG, "converting password to localized key . . .\n"));
            *local =
                SrTextPasswordToLocalizedKey(prev_localization_algorithm,
                                             ConvToken_snmpID_prevSnmpID,
                                             (*token));
            if (*local == NULL) {
                return -1;
            } else {
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            return ConvToken_octetString(direction, token, value);
        default:
            return -1;
    }
}

#ifndef SR_NO_PRIVACY
SR_INT32
ConvToken_localizedDesKey(SR_INT32 direction, char **token, void *value)
{
    OctetString **local = (OctetString **)value;
    SR_INT32 pos, len;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            len = (SR_INT32)strlen(*token);
            for (pos = 0; pos < len; pos++) {
                if (((*token)[pos] == ':') ||
                    ((*token)[pos] == '-')) {
                    return ConvToken_octetString(direction, token, value);
                 }
            }
            DPRINTF((APCONFIG, "converting password to localized key . . .\n"));
            *local =
                SrTextPasswordToLocalizedKey(prev_localization_algorithm,
                                             ConvToken_snmpID_prevSnmpID,
                                             (*token));
            if (*local == NULL) {
                return -1;
            } else {
                (*local)->length = 16;
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            return ConvToken_octetString(direction, token, value);
        default:
            return -1;
    }
}

SR_INT32
ConvToken_localizedPrivKey(SR_INT32 direction, char **token, void *value)
{
    OctetString **local = (OctetString **)value;
#if (defined(SR_3DES) || defined(SR_AES))
    FNAME("ConvToken_localizedPrivKey")
    OctetString *key1, *key2;
    unsigned char *tmp_ptr;
    SR_INT32  remainderLen;
#endif	/* (defined(SR_3DES) || defined(SR_AES)) */
    SR_INT32 pos, len, status; 

    status = -1;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            len = (SR_INT32)strlen(*token);
            for (pos = 0; pos < len; pos++) {
                if (((*token)[pos] == ':') ||
                    ((*token)[pos] == '-')) {
                    return ConvToken_octetString(direction, token, value);
                 }
            }
            DPRINTF((APCONFIG, "converting password to localized key . . .\n"));
            /*
             * Currently using global, cur_PrivType, to determine which
             * privacy protocol we are using. This is set for this row
             * when ConvToken_PrivProtocol is called.
             */
            switch (cur_PrivType) {
                case SR_USM_NOPRIV_PROTOCOL:
                case SR_USM_DES_PRIV_PROTOCOL:
                    *local =
                       SrTextPasswordToLocalizedKey(prev_localization_algorithm,
                                                    ConvToken_snmpID_prevSnmpID,
                                                    (*token));
                    if (*local == NULL) {
                        status = -1;
                    } else {
                        (*local)->length = 16;
                        status = 0;
                    }
                    break;
#ifdef SR_3DES
                case SR_USM_3DES_PRIV_PROTOCOL:
                    *local = MakeOctetString(NULL, SR_3DES_KEY_LENGTH);
                    if (*local == NULL) {
                        return -1;
                    }
                    key1 =
                       SrTextPasswordToLocalizedKey(prev_localization_algorithm,
                                                    ConvToken_snmpID_prevSnmpID,
                                                    (*token));
                    if (key1 == NULL) {
                        FreeOctetString(*local);
                        status = -1;
                    }
                    else {
                        key2 =
                           SrOctetStringPasswordToLocalizedKey(prev_localization_algorithm,
                                                        ConvToken_snmpID_prevSnmpID,
                                                        key1);
                        if (key2 == NULL) {
                            FreeOctetString(*local);
                            FreeOctetString(key1);
                            status = -1;
                        }
                        else {
                            if (memcmp(key1->octet_ptr,
                                       key1->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
                                       SR_3DES_EACH_KEY_LENGTH) == 0) {
                                DPRINTF((APWARN, "%s: 3DES key1 and key2 are equal!\n", Fname));
                            }
                            if (memcmp(key1->octet_ptr + SR_3DES_EACH_KEY_LENGTH,
                                       key2->octet_ptr, 
                                       SR_3DES_EACH_KEY_LENGTH) == 0) {
                                DPRINTF((APWARN, "%s: 3DES key2 and key3 are equal!\n", Fname));
                            }
                            if (memcmp(key1->octet_ptr,
                                       key2->octet_ptr, 
                                       SR_3DES_EACH_KEY_LENGTH) == 0) {
                                DPRINTF((APWARN, "%s: 3DES key1 and key3 are equal!\n", Fname));
                            }
                            memcpy((*local)->octet_ptr, key1->octet_ptr,
                                   key1->length);
                            remainderLen = SR_3DES_KEY_LENGTH - key1->length;
                            tmp_ptr = (*local)->octet_ptr + key1->length;
                            memcpy(tmp_ptr, key2->octet_ptr, remainderLen);
                            free(key1);
                            free(key2);
                            status = 0;
                        }
                    }
                    break;
#endif /* SR_3DES */
#ifdef SR_AES
                case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
                case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
                case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
                    *local = MakeOctetString(NULL, SR_AES_KEY_LENGTH);
                    if (*local == NULL) {
                        return -1;
                    }
                    key1 =
                       SrTextPasswordToLocalizedKey(prev_localization_algorithm,
                                                    ConvToken_snmpID_prevSnmpID,
                                                    (*token));
                    if (key1 == NULL) {
                        FreeOctetString(*local);
                        status = -1;
                    }
                    else {
                        key2 =
                           SrOctetStringPasswordToLocalizedKey(
					     prev_localization_algorithm,
                                             ConvToken_snmpID_prevSnmpID,
                                             key1);
                        if (key2 == NULL) {
                            FreeOctetString(*local);
                            FreeOctetString(key1);
                            status = -1;
                        }
                        else {
                            memcpy((*local)->octet_ptr, key1->octet_ptr,
                                   key1->length);
                            remainderLen = SR_AES_KEY_LENGTH - key1->length;
                            tmp_ptr = (*local)->octet_ptr + key1->length;
                            memcpy(tmp_ptr, key2->octet_ptr, remainderLen);
                            free(key1);
                            free(key2);

                            /*
                             * Adjust length for AES128 and AES192
                             */
                            if (cur_PrivType ==
                                  SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
                                (*local)->length = SR_AES_128_KEY_LENGTH;
                            }
                            if (cur_PrivType ==
                                  SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
                                (*local)->length = SR_AES_192_KEY_LENGTH;
                            }

                            status = 0;
                        }
                    }
                    break;
#endif /* SR_AES */
                default:
                    status = -1;
                    break;
             
            }
            break;
        case PARSER_CONVERT_TO_TOKEN:
            status = ConvToken_octetString(direction, token, value);
            break;
        default:
            status = -1;
    }
 
    return status;
}
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#endif /* SR_CONFIG_FP */
