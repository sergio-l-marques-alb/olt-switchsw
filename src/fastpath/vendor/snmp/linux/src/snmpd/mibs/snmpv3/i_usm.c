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
 */


/*
 * Arguments used to create this file:
 * -f mib.cnf -o v3 -per_file_init -stubs_only -search_table (implies -row_status) 
 * -test_and_incr -parser -row_status -userpart -storage_type 
 */


/*
 * File Description:
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "diag.h"
SR_FILENAME
#include "v3type.h"

Index_t usmUserEntryIndex[] = {
    { offsetof(usmUserEntry_t, usmUserEngineID),  T_var_octet},
    { offsetof(usmUserEntry_t, usmUserName),  T_var_octet}
};

const SnmpType usmUserEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(usmUserEntry_t, usmUserEngineID), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(usmUserEntry_t, usmUserName), 1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY,
      offsetof(usmUserEntry_t, usmUserSecurityName), -1 },
    { OBJECT_ID_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserCloneFrom), -1 },
    { OBJECT_ID_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserAuthProtocol), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserAuthKeyChange), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserOwnAuthKeyChange), -1 },
    { OBJECT_ID_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserPrivProtocol), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserPrivKeyChange), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserOwnPrivKeyChange), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserPublic), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserStorageType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmUserStatus), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, usmTargetTag), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(usmUserEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, auth_secret), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, priv_secret), -1 },
#ifdef SR_3DES
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, privKeyChangeFlag), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, key1), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, key2), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, key3), -1 },
#endif /* SR_3DES */
#else /* SR_NO_PRIVACY */
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(usmUserEntry_t, auth_secret), -1 },
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER usmUserEntryConverters[] = {
    { ConvToken_snmpID, NULL },    /* usmUserEngineID */
    { ConvToken_textOctetString, NULL },    /* usmUserName */
    { ConvToken_textOctetString, "-" },    /* usmUserSecurityName */
    { ConvToken_OID, "0.0" },    /* usmUserCloneFrom */
    { ConvToken_authProtocol, NULL },    /* usmUserAuthProtocol */
    { ConvToken_textOctetString, "-" },    /* usmUserAuthKeyChange */
    { ConvToken_textOctetString, "-" },    /* usmUserOwnAuthKeyChange */
    { ConvToken_privProtocol, NULL },    /* usmUserPrivProtocol */
    { ConvToken_textOctetString, "-" },    /* usmUserPrivKeyChange */
    { ConvToken_textOctetString, "-" },    /* usmUserOwnPrivKeyChange */
    { ConvToken_textOctetString, "-" },    /* usmUserPublic */
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },    /* usmUserStatus */
    { ConvToken_textOctetString, NULL },    /* usmTargetTag */
    { ConvToken_integer, "-1" },
    /* Additional entries for user defines go here */
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    { ConvToken_localizedKey, NULL },
#if (! ( defined(SR_3DES) ) && ! ( defined(SR_AES) ))
    { ConvToken_localizedDesKey, NULL },
#else	/* (! ( defined(SR_3DES) ) && ! ( defined(SR_AES) )) */
    { ConvToken_localizedPrivKey, NULL },
#endif	/* (! ( defined(SR_3DES) ) && ! ( defined(SR_AES) )) */
#ifdef SR_3DES
    { ConvToken_integer, "0" },            /* privKeyChangeFlag */
    { ConvToken_textOctetString, "-" },    /* key1 */
    { ConvToken_textOctetString, "-" },    /* key2 */
    { ConvToken_textOctetString, "-" },    /* key3 */
#endif /* SR_3DES */
#else /* SR_NO_PRIVACY */
    { ConvToken_localizedKey, NULL },
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    { NULL, NULL }
};
#ifndef SR_NO_COMMENTS_IN_CONFIG
const char usmUserEntryRecordFormatString[] =
    "#Entry type: usmUserEntry\n"
    "#Format:  usmUserEngineID  (octetString)\n"
    "#         usmUserName  (text)\n"
    "#         usmUserAuthProtocol  (OID)\n"
    "#         usmUserPrivProtocol  (OID)\n"
    "#         usmUserStorageType  (nonVolatile, permanent, readOnly)\n"
    "#         usmTargetTag  (text)"
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    "\n"
    "#         AuthKey  (octetString)\n"
    "#         PrivKey  (octetString)"
#else /* SR_NO_PRIVACY */
    "\n"
    "#         AuthKey  (octetString)"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ;
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
#endif /* SR_CONFIG_FP */

/* initialize support for usmStats objects */
int
i_usmStats_initialize(
    usmStats_t *us)
{
    us->usmStatsUnsupportedSecLevels = 0;
    us->usmStatsNotInTimeWindows = 0;
    us->usmStatsUnknownUserNames = 0;
    us->usmStatsUnknownEngineIDs = 0;
    us->usmStatsWrongDigests = 0;
    us->usmStatsDecryptionErrors = 0;
    return 1;
}

/* terminate support for usmStats objects */
int
i_usmStats_terminate(void)
{
    return 1;
}

/* initialize support for usmUser objects */
int
i_usmUser_initialize(
    usmUser_t *uu)
{
    uu->usmUserSpinLock = GetTimeNow() * 123;
    return 1;
}

/* terminate support for usmUser objects */
int
i_usmUser_terminate(void)
{
    return 1;
}

/* initialize support for usmUserEntry objects */
int
i_usmUserEntry_initialize(
    SnmpV2Table *uut)
{
    uut->tp = (void **) malloc (sizeof(usmUserEntry_t *));
    if (uut->tp == NULL) {
        return 0;
    }
    uut->tip = usmUserEntryIndex;
    uut->nitems = 0;
    uut->nindices = sizeof(usmUserEntryIndex) / sizeof(Index_t);
    uut->rowsize = sizeof(usmUserEntry_t);

    return 1;
}

/* terminate support for usmUserEntry objects */
int
i_usmUserEntry_terminate(
    SnmpV2Table *uut)
{
    /* free allocated memory */
    while(uut->nitems > 0) {
        FreeEntries(usmUserEntryTypeTable, uut->tp[0]);
        free(uut->tp[0]);
        RemoveTableEntry(uut, 0);
    }

    if (uut->tp != NULL){
      free(uut->tp);
      uut->tp = NULL;
    }
    return 1;
}

int
i_usm_initialize(
    usmStats_t *us,
    usmUser_t *uu,
    SnmpV2Table *uut)
{
    int status = 1;

    if (i_usmStats_initialize(us) != 1) {
        status = -1;
    }
    if (i_usmUser_initialize(uu) != 1) {
        status = -1;
    }
    if (i_usmUserEntry_initialize(uut) != 1) {
        status = -1;
    }

    return status;
}

int
i_usm_terminate(
    SnmpV2Table *uut)
{
    int status = 1;

    if (i_usmUserEntry_terminate(uut) != 1) {
        status = -1;
    }

    return status;
}
