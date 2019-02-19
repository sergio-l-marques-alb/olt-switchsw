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
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "diag.h"
SR_FILENAME
#include "mibout.h"
#include "global.h"
#include "sr_msg.h"
#include "v3_msg.h"
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
#include "md5.h"
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
#include "sha.h"
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */
#include "tmq.h"
#include "rowstatf.h"
#include "i_usm.h"
#include "mapctx.h"
#include "oid_lib.h"	/* for CmpOID */

#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
SR_INT32
srCheck3DESKeys(
     OctetString *privKey,
     OctetString *keyChange,
     SR_INT32 authProt);
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */

void UpdateUsmSecret(
     SR_INT32 hashAlgorithm,
     OctetString *change,
     OctetString *secret);

#ifdef SR_MD5_HASH_ALGORITHM
void MD5UpdateUsmSecret(
     OctetString *change,
     OctetString *secret);
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
void SHAUpdateUsmSecret(
     OctetString *change,
     OctetString *secret);
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

#define MIN_CLONEFROM_LENGTH 14

contextMappingList_t contextMappingList_usmStatsData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_usmUserData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_usmUserTable = { NULL, 0, NULL };

/* global data describing the usmStats family */
usmStats_t *usmStatsData;

/* global data describing the usmUser family */
usmUser_t *usmUserData;

/* global data describing the usmUserTable entries */
SnmpV2Table *usmUserTable;

void DeleteusmUserEntry
    SR_PROTOTYPE((int index));

/* initialize support for usmStats objects */
int
k_usmStats_initialize(
    char *contextName_text,
    usmStats_t *us)
{
    AddContextMappingText(&contextMappingList_usmStatsData,
                          contextName_text,
                          (void *)us);
    return 1;
}

/* terminate support for usmStats objects */
int
k_usmStats_terminate(void)
{
    return 1;
}

/* initialize support for usmUser objects */
int
k_usmUser_initialize(
    char *contextName_text,
    usmUser_t *uu)
{
    AddContextMappingText(&contextMappingList_usmUserData,
                          contextName_text,
                          (void *)uu);
    return 1;
}

/* terminate support for usmUser objects */
int
k_usmUser_terminate(void)
{
    return 1;
}

/* initialize support for usmUserEntry objects */
int
k_usmUserEntry_initialize(
    char *contextName_text,
    SnmpV2Table *uut)
{
    AddContextMappingText(&contextMappingList_usmUserTable,
                          contextName_text,
                          (void *)uut);

    return 1;
}

/* terminate support for usmUserEntry objects */
int
k_usmUserEntry_terminate(
    SnmpV2Table *uut)
{
    /* free allocated memory */
    usmUserTable = uut;
    while(uut->nitems > 0) {
        DeleteusmUserEntry(0);
    }
    
    free(uut->tp);
    return 1;
}

int
k_usm_initialize(
    char *contextName_text,
    usmStats_t *us,
    usmUser_t *uu,
    SnmpV2Table *uut)
{
    int status = 1;

    if (k_usmStats_initialize(contextName_text, us) != 1) {
        status = -1;
    }
    if (k_usmUser_initialize(contextName_text, uu) != 1) {
        status = -1;
    }
    if (k_usmUserEntry_initialize(contextName_text, uut) != 1) {
        status = -1;
    }

    return status;
}

int
k_usm_terminate(void)
{
    int status = 1;


    return status;
}

#ifdef U_usmStats
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usmStatsFreeUserpartData (usmStats_t *data)
{
    /* nothing to free by default */
}
#endif /* U_usmStats */

#ifdef U_usmStats
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usmStatsCloneUserpartData (usmStats_t *dst, usmStats_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_usmStats */

usmStats_t *
k_usmStats_get(int serialNum, ContextInfo *contextInfo,
               int nominator)
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if ((serialNum != prev_serialNum) || (serialNum == -1)) {
            MapContext(&contextMappingList_usmStatsData,
                       contextInfo,
                       (void *)&usmStatsData);
        }
    } else {
        MapContext(&contextMappingList_usmStatsData,
                   contextInfo,
                   (void *)&usmStatsData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;
    
    return usmStatsData;
}

#ifdef U_usmUser
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usmUserFreeUserpartData (usmUser_t *data)
{
    /* nothing to free by default */
}
#endif /* U_usmUser */

#ifdef U_usmUser
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usmUserCloneUserpartData (usmUser_t *dst, usmUser_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_usmUser */

usmUser_t *
k_usmUser_get(int serialNum, ContextInfo *contextInfo,
              int nominator)
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if ((serialNum != prev_serialNum) || (serialNum == -1)) {
            MapContext(&contextMappingList_usmUserData,
                       contextInfo,
                       (void *)&usmUserData);
        }
    } else {
        MapContext(&contextMappingList_usmUserData,
                   contextInfo,
                   (void *)&usmUserData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    return usmUserData;
}

#ifdef SETS
int
k_usmUser_test(ObjectInfo *object, ObjectSyntax *value,
               doList_t *dp, ContextInfo *contextInfo)
{
    MapContext(&contextMappingList_usmUserData,
               contextInfo,
               (void *)&usmUserData);
    if (usmUserData == NULL) {
        return NO_CREATION_ERROR;
    }
    if (usmUserData->usmUserSpinLock != value->sl_value) {
       return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_usmUser_ready(ObjectInfo *object, ObjectSyntax *value, 
                doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_usmUser_set(usmUser_t *data,
              ContextInfo *contextInfo, int function)
{
    MapContext(&contextMappingList_usmUserData,
               contextInfo,
               (void *)&usmUserData);
    if (usmUserData == NULL) {
        return NO_CREATION_ERROR;
    }

   usmUserData->usmUserSpinLock = ++data->usmUserSpinLock;
   return NO_ERROR;
}

#ifdef SR_usmUser_UNDO
/* add #define SR_usmUser_UNDO in sitedefs.h to
 * include the undo routine for the usmUser family.
 */
int
usmUser_undo(doList_t *doHead, doList_t *doCur,
             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_usmUser_UNDO */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
usmUserTableDeleteCallback (TimeOutDescriptor *tdp)
{
    cidata_t *ud2 = (cidata_t *)tdp->UserData2;
    usmUserEntry_t *data;

    /* dummy up an entry to delete */
    data = (usmUserEntry_t *) ud2->data;
    data->usmUserStatus = D_usmUserStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_usmUserEntry_set(data, ud2->cip, 0);
    FreeContextInfo(ud2->cip);
    free(ud2);
}

#endif /* SETS */

/* This routine deletes an entry from the usmUserTable */
void
DeleteusmUserEntry (int index)
{
    FreeEntries(usmUserEntryTypeTable, usmUserTable->tp[index]);
    free(usmUserTable->tp[index]);
    RemoveTableEntry(usmUserTable, index);
}

/* This prototype is required to please some compilers */
int usmUserEntryReadyToActivate
    SR_PROTOTYPE((usmUserEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
usmUserEntryReadyToActivate(usmUserEntry_t *data)
{
    if (!VALID(I_usmUserCloneFrom, data->valid)) {
        return 0;
    }

    if (CmpOID(data->usmUserAuthProtocol, &usmNoAuthProtocol)) {
        if (!VALID(I_usmUserAuthKeyChange, data->valid) &&
            !VALID(I_usmUserOwnAuthKeyChange, data->valid)) {
            return 0;
        }
    }

    if (CmpOID(data->usmUserPrivProtocol, &usmNoPrivProtocol)) {
        if (!VALID(I_usmUserPrivKeyChange, data->valid) &&
            !VALID(I_usmUserOwnPrivKeyChange, data->valid)) {
            return 0;
        }
    }

    if (CmpOID(data->usmUserAuthProtocol, &usmNoAuthProtocol) == 0)  {
        if (CmpOID(data->usmUserPrivProtocol, &usmNoPrivProtocol) != 0)  {
            return 0;
        }
    }

    return(1);
}

#ifdef U_usmUserEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usmUserEntryFreeUserpartData (usmUserEntry_t *data)
{
#ifndef SR_UNSECURABLE
    if (data->auth_secret != NULL) {
        FreeOctetString(data->auth_secret);
    }
#ifndef SR_NO_PRIVACY
    if (data->priv_secret != NULL) {
        FreeOctetString(data->priv_secret);
    }
#ifdef SR_3DES
    if (data->key1 != NULL) {
        FreeOctetString(data->key1);
    }
    if (data->key2 != NULL) {
        FreeOctetString(data->key2);
    }
    if (data->key3 != NULL) {
        FreeOctetString(data->key3);
    }
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
}
#endif /* U_usmUserEntry */

#ifdef U_usmUserEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usmUserEntryCloneUserpartData (usmUserEntry_t *dst, usmUserEntry_t *src)
{
#ifndef SR_UNSECURABLE
    /* 
     * Do not clone auth_secret here. The auth_secret field is cloned in
     * the SrCopyFamilyContents() function in minv.c when it goes through every
     * element in the usmUserEntryTypeTable and clones it there. Otherwise,
     * we end up cloning the auth_secret twice and not freeing the value from
     * the first clone.
     */
#ifndef SR_NO_PRIVACY
    /* 
     * Do not clone priv_secret here. The priv_secret field is cloned in
     * the SrCopyFamilyContents() function in minv.c when it goes through every
     * element in the usmUserEntryTypeTable and clones it there. Otherwise,
     * we end up cloning the priv_secret twice and not freeing the value from
     * the first clone.
     */
#ifdef SR_3DES
    dst->privKeyChangeFlag = src->privKeyChangeFlag;
    dst->key1 = NULL;
    dst->key2 = NULL;
    dst->key3 = NULL;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
}
#endif /* U_usmUserEntry */

usmUserEntry_t *
k_usmUserEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   OctetString * usmUserEngineID,
                   OctetString * usmUserName)
{
    int index;
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;
    usmUserEntry_t *uue;

    if (!prev_serialNum_initialized) {
        if ((serialNum != prev_serialNum) || (serialNum == -1)) {
            MapContext(&contextMappingList_usmUserTable,
                       contextInfo,
                       (void *)&usmUserTable);
        }
    } else {
        MapContext(&contextMappingList_usmUserTable,
                   contextInfo,
                   (void *)&usmUserTable);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    if (usmUserTable == NULL) {
        return NULL;
    }

    usmUserTable->tip[0].value.octet_val = usmUserEngineID;
    usmUserTable->tip[1].value.octet_val = usmUserName;
    if ((index = SearchTable(usmUserTable, searchType)) == -1) {
        return NULL;
    }

    uue = (usmUserEntry_t *)usmUserTable->tp[index];
    if ( uue->usmUserSecurityName == NULL || 
         uue->usmUserSecurityName->length == 0) {

        if (uue->usmUserSecurityName != NULL) {
            FreeOctetString(uue->usmUserSecurityName);
        }
        uue->usmUserSecurityName = CloneOctetString(uue->usmUserName);
    }

    /* --- Insure the VALID bits are set for the key change objects --- */
    if (uue->usmUserAuthKeyChange != NULL) {
        if (!VALID(I_usmUserAuthKeyChange, uue->valid)) {
            SET_VALID(I_usmUserAuthKeyChange, uue->valid);
        }
    }
    if (uue->usmUserOwnAuthKeyChange != NULL) {
        if (!VALID(I_usmUserOwnAuthKeyChange, uue->valid)) {
            SET_VALID(I_usmUserOwnAuthKeyChange, uue->valid);
        }
    }
    if (uue->usmUserPrivKeyChange != NULL) {
        if (!VALID(I_usmUserPrivKeyChange, uue->valid)) {
            SET_VALID(I_usmUserPrivKeyChange, uue->valid);
        }
    }
    if (uue->usmUserOwnPrivKeyChange != NULL) {
        if (!VALID(I_usmUserOwnPrivKeyChange, uue->valid)) {
            SET_VALID(I_usmUserOwnPrivKeyChange, uue->valid);
        }
    }

    return ((usmUserEntry_t *)uue);
}   /* k_usmUserEntry_get() */

#ifdef SETS
int
k_usmUserEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{
    usmUserEntry_t *data, *usmUserCloneFrom;
    SR_INT32 snmpEngineID_len;
    SR_INT32 usmUserName_len;
    SR_UINT32 *snmpEngineID_ptr;
    SR_UINT32 *usmUserName_ptr;
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
    SR_INT32 authProt;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
    int index;
    int i;
    ObjectSyntax *os;

    MapContext(&contextMappingList_usmUserTable,
               contextInfo,
               (void *)&usmUserTable);
    if (usmUserTable == NULL) {
        return NO_CREATION_ERROR;
    }

    data = (usmUserEntry_t *)dp->data;

    if ((object->nominator == I_usmUserOwnAuthKeyChange) ||
        (object->nominator == I_usmUserOwnPrivKeyChange)) {
        /* Make sure that the username used in the request matches the
         * username to be changed */
        os = SearchContextInfo(contextInfo, NCI_SECURITY_MODEL);
        if (os == NULL) {
            return NO_ACCESS_ERROR;
        }
        if (os->sl_value != SR_SECURITY_MODEL_USM) {
            return NO_ACCESS_ERROR;
        }
        os = SearchContextInfo(contextInfo, NCI_SECURITY_NAME);
        if (os == NULL) {
            return NO_ACCESS_ERROR;
        }
        if (CmpOctetStrings(os->os_value, data->usmUserName)) {
            return NO_ACCESS_ERROR;
        }
    }

#ifndef SR_UNSECURABLE
    if (object->nominator == I_usmUserAuthProtocol) {
       if (CmpOID(value->u.oidval, &usmNoAuthProtocol)) {
#ifdef SR_MD5_HASH_ALGORITHM
           if (CmpOID(value->u.oidval, &usmHMACMD5AuthProtocol)) {
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
              if (CmpOID(value->u.oidval, &usmHMACSHAAuthProtocol)) {
#endif /* SR_SHA_HASH_ALGORITHM */
                   return WRONG_VALUE_ERROR;
#ifdef SR_SHA_HASH_ALGORITHM
              }
#endif /* SR_SHA_HASH_ALGORITHM */
#ifdef SR_MD5_HASH_ALGORITHM
          } 
#endif /* SR_MD5_HASH_ALGORITHM */
       }
     }
#endif /* SR_UNSECURABLE */


#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    if (object->nominator == I_usmUserPrivProtocol) {
       if ( CmpOID(value->u.oidval, &usmNoPrivProtocol) &&
          ( CmpOID(value->u.oidval, &usmDESPrivProtocol))) {
#ifdef SR_3DES
           if (CmpOID(value->u.oidval, &usm3DESPrivProtocol)) {
#endif /* SR_3DES */
#ifdef SR_AES
              if ( (CmpOID(value->u.oidval, &usmAesCfb128Protocol))
                   && (CmpOID(value->u.oidval, &usmAESCfb128PrivProtocol))
                   && (CmpOID(value->u.oidval, &usmAESCfb192PrivProtocol))
                   && (CmpOID(value->u.oidval, &usmAESCfb256PrivProtocol))) {
#endif /* SR_AES */
                    return WRONG_VALUE_ERROR;
#ifdef SR_AES
              }
#endif /* SR_AES */
#ifdef SR_3DES
           }
#endif /* SR_3DES */
       }
   }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE

    /* Make sure key change values are proper length */
    if ((object->nominator == I_usmUserAuthKeyChange) &&
        (VALID(I_usmUserCloneFrom, data->valid))) {
        if ((data->auth_secret->length * 2) != value->os_value->length) {
            return INCONSISTENT_VALUE_ERROR;
        }
    }
    if ((object->nominator == I_usmUserOwnAuthKeyChange) &&
        (VALID(I_usmUserCloneFrom, data->valid))) {
        if ((data->auth_secret->length * 2) != value->os_value->length) {
            return INCONSISTENT_VALUE_ERROR;
        }
    }
    /* can only change usmUserAuthProtocol to usmNoAuthProtocol */
    if (object->nominator == I_usmUserAuthProtocol)   {
        if(CmpOID(value->u.oidval, &usmNoAuthProtocol) != 0)  {
             return INCONSISTENT_VALUE_ERROR;
        }
    }

#ifndef SR_NO_PRIVACY

    /* can only change usmUserPrivProtocol to usmNoPrivProtocol */
    if (object->nominator == I_usmUserPrivProtocol)   {
        if(CmpOID(value->u.oidval, &usmNoPrivProtocol) != 0)  {
             return INCONSISTENT_VALUE_ERROR;
        }
    }

    if ((object->nominator == I_usmUserPrivKeyChange) &&
        (VALID(I_usmUserCloneFrom, data->valid))) {
        if ((data->priv_secret->length * 2) != value->os_value->length) {
            return INCONSISTENT_VALUE_ERROR;
        }
#ifdef SR_3DES
        /* Check to see if 3 3DES keys are different */
        if (CmpOID(data->usmUserPrivProtocol, &usm3DESPrivProtocol) == 0) {
            authProt = SrV3ProtocolOIDToInt(data->usmUserAuthProtocol);
            if (srCheck3DESKeys(data->priv_secret, value->os_value, 
                                authProt) != 0) {
                return INCONSISTENT_VALUE_ERROR;
            }
        }
#endif /* SR_3DES */
    }
    if ((object->nominator == I_usmUserOwnPrivKeyChange) &&
        (VALID(I_usmUserCloneFrom, data->valid))) {
        if ((data->priv_secret->length * 2) != value->os_value->length) {
            return INCONSISTENT_VALUE_ERROR;
        }
#ifdef SR_3DES
        /* Check to see if 3 3DES keys are different */
        if (CmpOID(data->usmUserPrivProtocol, &usm3DESPrivProtocol) == 0) {
            authProt = SrV3ProtocolOIDToInt(data->usmUserAuthProtocol);
            if (srCheck3DESKeys(data->priv_secret, value->os_value,
                                authProt) != 0) {
                return INCONSISTENT_VALUE_ERROR;
            }
        }
#endif /* SR_3DES */
    }

#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    if (object->nominator == I_usmUserCloneFrom) {
        if (VALID(I_usmUserCloneFrom, data->valid)) {
            FreeOID(data->usmUserCloneFrom);
            data->usmUserCloneFrom = MakeOIDFromDot("0.0");
            if (data->usmUserCloneFrom == NULL) {
                return RESOURCE_UNAVAILABLE_ERROR;
            }
            return NO_ERROR;
        }
        if (value->oid_value->length < MIN_CLONEFROM_LENGTH) {
            return WRONG_VALUE_ERROR;
        }
        /* Must do the clone, so make sure the clonee exists */
        snmpEngineID_len = value->oid_value->oid_ptr[12];
        snmpEngineID_ptr = &(value->oid_value->oid_ptr[13]);
        usmUserName_len = value->oid_value->length - snmpEngineID_len - 14;
        usmUserName_ptr = &(value->oid_value->oid_ptr[14 + snmpEngineID_len]);
        usmUserTable->tip[0].value.octet_val =
            MakeOctetString(NULL, snmpEngineID_len);
        if (usmUserTable->tip[0].value.octet_val == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        usmUserTable->tip[1].value.octet_val =
            MakeOctetString(NULL, usmUserName_len);
        if (usmUserTable->tip[1].value.octet_val == NULL) {
            FreeOctetString(usmUserTable->tip[0].value.octet_val);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        for (i = 0; i < snmpEngineID_len; i++) {
            usmUserTable->tip[0].value.octet_val->octet_ptr[i] =
                (unsigned char)snmpEngineID_ptr[i];
        }
        for (i = 0; i < usmUserName_len; i++) {
            usmUserTable->tip[1].value.octet_val->octet_ptr[i] =
                (unsigned char)usmUserName_ptr[i];
        }
        index = SearchTable(usmUserTable, EXACT);
        FreeOctetString(usmUserTable->tip[0].value.octet_val);
        FreeOctetString(usmUserTable->tip[1].value.octet_val);
        if (index == -1) {
            return INCONSISTENT_VALUE_ERROR;
        }
        usmUserCloneFrom = (usmUserEntry_t *)usmUserTable->tp[index];
        if ((usmUserCloneFrom->usmUserStatus != RS_ACTIVE) &&
            (usmUserCloneFrom->usmUserStatus != RS_NOT_IN_SERVICE)) {
            return INCONSISTENT_NAME_ERROR;
        }
#ifndef SR_UNSECURABLE
        /* Clone the secrets and protocols now */
        if (data->usmUserAuthProtocol != NULL) {
            FreeOID(data->usmUserAuthProtocol);
        }
        data->usmUserAuthProtocol =
            CloneOID(usmUserCloneFrom->usmUserAuthProtocol);
        if (data->usmUserAuthProtocol == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        if (data->auth_secret != NULL) {
            FreeOctetString(data->auth_secret);
        }
        data->auth_secret = CloneOctetString(usmUserCloneFrom->auth_secret);
        if (data->auth_secret == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifndef SR_NO_PRIVACY
        if (data->usmUserPrivProtocol != NULL) {
            FreeOID(data->usmUserPrivProtocol);
        }
        data->usmUserPrivProtocol =
            CloneOID(usmUserCloneFrom->usmUserPrivProtocol);
        if (data->usmUserPrivProtocol == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        if (data->priv_secret != NULL) {
            FreeOctetString(data->priv_secret);
        }
        data->priv_secret = CloneOctetString(usmUserCloneFrom->priv_secret);
        if (data->priv_secret == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE
        /* Make sure key change values are proper length */
        if (VALID(I_usmUserAuthKeyChange, data->valid)) {
            if ((data->auth_secret->length * 2) !=
                     data->usmUserAuthKeyChange->length) {
                return INCONSISTENT_VALUE_ERROR;
            }
        }
        if (VALID(I_usmUserOwnAuthKeyChange, data->valid)) {
            if ((data->auth_secret->length * 2) !=
                     data->usmUserOwnAuthKeyChange->length) {
                return INCONSISTENT_VALUE_ERROR;
            }
        }
#ifndef SR_NO_PRIVACY
        if (VALID(I_usmUserPrivKeyChange, data->valid)) {
            if ((data->priv_secret->length * 2) !=
                     data->usmUserPrivKeyChange->length) {
                return INCONSISTENT_VALUE_ERROR;
            }
#ifdef SR_3DES
            /* Check to see if 3 3DES keys are different */
            if (CmpOID(data->usmUserPrivProtocol, &usm3DESPrivProtocol) == 0) {
                authProt = SrV3ProtocolOIDToInt(data->usmUserAuthProtocol);
                if (srCheck3DESKeys(data->priv_secret, 
                                    data->usmUserPrivKeyChange,
                                    authProt) != 0) {
                    return INCONSISTENT_VALUE_ERROR;
                }
            }
#endif /* SR_3DES */
        }
        if (VALID(I_usmUserOwnPrivKeyChange, data->valid)) {
            if ((data->priv_secret->length * 2) !=
                     data->usmUserOwnPrivKeyChange->length) {
                return INCONSISTENT_VALUE_ERROR;
            }
#ifdef SR_3DES
            /* Check to see if 3 3DES keys are different */
            if (CmpOID(data->usmUserPrivProtocol, &usm3DESPrivProtocol) == 0) {
                authProt = SrV3ProtocolOIDToInt(data->usmUserAuthProtocol);
                if (srCheck3DESKeys(data->priv_secret, 
                                    data->usmUserOwnPrivKeyChange,
                                    authProt) != 0) {
                    return INCONSISTENT_VALUE_ERROR;
                }
            }
#endif /* SR_3DES */
        }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
        FreeOID(data->usmUserCloneFrom);
        data->usmUserCloneFrom = MakeOIDFromDot("0.0");
        if (data->usmUserCloneFrom == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }
    return NO_ERROR;
}

int
k_usmUserEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    usmUserEntry_t *data = (usmUserEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->usmUserStatus == D_usmUserStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->usmUserStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->usmUserStatus == D_usmUserStatus_createAndGo) {
            data->usmUserStatus = D_usmUserStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->usmUserStatus == D_usmUserStatus_active
        || data->usmUserStatus == D_usmUserStatus_notInService) {
            if(usmUserEntryReadyToActivate(data) == 0) {
                /* inconsistent values, don't allow the set operation */
                doListReady = 0;
            }
        } else if (data->usmUserStatus == D_usmUserStatus_createAndWait ||
                   data->usmUserStatus == D_usmUserStatus_notReady) {

            /* Check to make sure that cloneFrom has already been set if
             * authKeyChange or privKeyChange is being set
             */
            if ( VALID(I_usmUserAuthKeyChange, data->valid) ||
                 VALID(I_usmUserPrivKeyChange, data->valid) ||
                 VALID(I_usmUserOwnAuthKeyChange, data->valid) ||
                 VALID(I_usmUserOwnPrivKeyChange, data->valid) ) {

                if (!VALID(I_usmUserCloneFrom, data->valid)) {
                    /* Can't set KeyChange without a cloneFrom user */
                    if(dp->state == SR_UNUSED || dp->state == SR_PENDING_STATE){
                        dp->state = INCONSISTENT_NAME_STATE;
                    }
                    return NO_ERROR;
                }
            }
        }

        /* did we have all required data? */
        if(doListReady != 0) {
            /* yes, update the doList entry */
            dp->state = SR_ADD_MODIFY;
        } else {
            /* no, make sure the set doesn't proceed */
            dp->state = SR_UNKNOWN;
        }
    }

    return NO_ERROR;
}

int
k_usmUserEntry_set_defaults(doList_t *dp)
{
    usmUserEntry_t *data = (usmUserEntry_t *) (dp->data);

    if ((data->usmUserSecurityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserCloneFrom = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    /* usmNoAuthProtocol = 1.3.6.1.6.3.10.1.1.1 */
    /* usmHMACMD5AuthProtocol = 1.3.6.1.6.3.10.1.1.2 */
    if ((data->usmUserAuthProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.1.1")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserAuthKeyChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserOwnAuthKeyChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    /* usmNoPrivProtocol = 1.3.6.1.6.3.10.1.2.1 */
    if ((data->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.2.1")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserPrivKeyChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserOwnPrivKeyChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->usmUserPublic = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->usmUserStorageType = D_usmUserStorageType_nonVolatile;
    if ((data->usmTargetTag = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->RowStatusTimerId = -1;

#ifndef SR_UNSECURABLE
    data->auth_secret = NULL;
#ifndef SR_NO_PRIVACY
    data->priv_secret = NULL;
#ifdef SR_3DES
    data->privKeyChangeFlag = FALSE;
    data->key1 = NULL;
    data->key2 = NULL;
    data->key3 = NULL;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    SET_ALL_VALID(data->valid);
    CLR_VALID(I_usmUserAuthKeyChange, data->valid);
    CLR_VALID(I_usmUserOwnAuthKeyChange, data->valid);
    CLR_VALID(I_usmUserPrivKeyChange, data->valid);
    CLR_VALID(I_usmUserOwnPrivKeyChange, data->valid);
    CLR_VALID(I_usmUserCloneFrom, data->valid);

    return NO_ERROR;
}

int
k_usmUserEntry_set(usmUserEntry_t *data,
                   ContextInfo *contextInfo, int function)
{

    int index, i;
    usmUserEntry_t *newrow = NULL;
#ifndef SR_UNSECURABLE
    SR_INT32 authProt;
#ifndef SR_NO_PRIVACY
    SR_INT32 privProt;
#ifdef SR_3DES
    unsigned char *key1, *key2, *key3;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    MapContext(&contextMappingList_usmUserTable,
               contextInfo,
               (void *)&usmUserTable);
    if (usmUserTable == NULL) {
        return NO_CREATION_ERROR;
    }

    /* find this entry in the table */
    usmUserTable->tip[0].value.octet_val = data->usmUserEngineID;
    usmUserTable->tip[1].value.octet_val = data->usmUserName;
    if ((index = SearchTable(usmUserTable, EXACT)) != -1) {
        newrow = (usmUserEntry_t *) usmUserTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->usmUserStatus == D_usmUserStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->usmUserStorageType;
            DeleteusmUserEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        usmUserTable->tip[0].value.octet_val = data->usmUserEngineID;
        usmUserTable->tip[1].value.octet_val = data->usmUserName;
        if ((index = NewTableEntry(usmUserTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (usmUserEntry_t *) usmUserTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, usmUserTableDeleteCallback)) == -1) {
            DeleteusmUserEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }


    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->usmUserStatus == D_usmUserStatus_createAndWait
    || data->usmUserStatus == D_usmUserStatus_notReady) {
        /* is the entry ready? */
        if(usmUserEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->usmUserStatus = D_usmUserStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->usmUserStatus = D_usmUserStatus_notReady;
        }
    }

    if (data->usmUserStatus != D_usmUserStatus_notReady)  {
#ifndef SR_UNSECURABLE
    /* Update authentication key */
    authProt = SrV3ProtocolOIDToInt(data->usmUserAuthProtocol);
    if (VALID(I_usmUserAuthKeyChange, data->valid)) {
        UpdateUsmSecret(authProt,
                        data->usmUserAuthKeyChange,
                        data->auth_secret);
        FreeOctetString(data->usmUserAuthKeyChange);
        data->usmUserAuthKeyChange = MakeOctetStringFromText("");
    }
    if (VALID(I_usmUserOwnAuthKeyChange, data->valid)) {
        UpdateUsmSecret(authProt,
                        data->usmUserOwnAuthKeyChange,
                        data->auth_secret);
        FreeOctetString(data->usmUserOwnAuthKeyChange);
        data->usmUserOwnAuthKeyChange = MakeOctetStringFromText("");
    }

#ifndef SR_NO_PRIVACY
    /* Update privacy key */
    privProt = SrV3ProtocolOIDToInt(data->usmUserPrivProtocol);
    if (VALID(I_usmUserPrivKeyChange, data->valid)) {
        UpdateUsmSecret(authProt,
                        data->usmUserPrivKeyChange,
                        data->priv_secret);
        FreeOctetString(data->usmUserPrivKeyChange);
        data->usmUserPrivKeyChange = MakeOctetStringFromText("");
#ifdef SR_3DES
        /*
         * If key has changed, free and/or null out 3DES keys and let
         * build/parse routine update the values.
         */
        if (privProt == SR_USM_3DES_PRIV_PROTOCOL) {
            if (data->key1 != NULL) {
                FreeOctetString(data->key1);
                data->key1 = NULL;
            }
            if (data->key2 != NULL) {
                FreeOctetString(data->key2);
                data->key2 = NULL;
            }
            if (data->key3 != NULL) {
                FreeOctetString(data->key3);
                data->key3 = NULL;
            }
            /* Check to see if 3 3DES keys are different */
            if (data->priv_secret != NULL) {
                key1 = data->priv_secret->octet_ptr;
                key2 = data->priv_secret->octet_ptr + SR_3DES_EACH_KEY_LENGTH;
                key3 = 
                    data->priv_secret->octet_ptr + (SR_3DES_EACH_KEY_LENGTH*2);
                if (memcmp(key1, key2, SR_3DES_EACH_KEY_LENGTH) == 0) {
                    DPRINTF((APWARN, "k_usmUserEntry_set: 3DES key1 and key2 are equal!\n"));
                }
                if (memcmp(key2, key3, SR_3DES_EACH_KEY_LENGTH) == 0) {
                    DPRINTF((APWARN, "k_usmUserEntry_set: 3DES key2 and key3 are equal!\n"));
                }
                if (memcmp(key1, key3, SR_3DES_EACH_KEY_LENGTH) == 0) {
                    DPRINTF((APWARN, "k_usmUserEntry_set: 3DES key1 and key3 are equal!\n"));
                }
            }
 
        }
#endif /* SR_3DES */
    }
    if (VALID(I_usmUserOwnPrivKeyChange, data->valid)) {
        UpdateUsmSecret(authProt,
                        data->usmUserOwnPrivKeyChange,
                        data->priv_secret);
        FreeOctetString(data->usmUserOwnPrivKeyChange);
        data->usmUserOwnPrivKeyChange = MakeOctetStringFromText("");
#ifdef SR_3DES
        /*
         * If key has changed, free and/or null out 3DES keys and let
         * build/parse routine update the values.
         */
        if (privProt == SR_USM_3DES_PRIV_PROTOCOL) {
            if (data->key1 != NULL) {
                FreeOctetString(data->key1);
                data->key1 = NULL;
            }
            if (data->key2 != NULL) {
                FreeOctetString(data->key2);
                data->key2 = NULL;
            }
            if (data->key3 != NULL) {
                FreeOctetString(data->key3);
                data->key3 = NULL;
            }
            data->privKeyChangeFlag = TRUE;
        }
#endif /* SR_3DES */
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }  /* end of if status != notReady */

    if (newrow->usmUserStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->usmUserStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_usmTargetTag, usmUserEntryTypeTable, (void *) newrow, (void *) data);
#ifdef SR_3DES
    newrow->privKeyChangeFlag = data->privKeyChangeFlag;
#endif /* SR_3DES */
    /* SET_ALL_VALID(newrow->valid); */

    for (i = 0; i < 17 ; i++) {
        if (VALID(i, data->valid)) {
            SET_VALID(i, newrow->valid);
        }
    }

#ifndef SR_UNSECURABLE
    if (newrow->auth_secret != NULL) {
        FreeOctetString(newrow->auth_secret);
    }
    newrow->auth_secret = data->auth_secret;
    data->auth_secret = NULL;
#ifndef SR_NO_PRIVACY
    if (newrow->priv_secret != NULL) {
        FreeOctetString(newrow->priv_secret);
    }
    newrow->priv_secret = data->priv_secret;
    data->priv_secret = NULL;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->usmUserStatus == D_usmUserStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->usmUserStatus == D_usmUserStatus_notInService || newrow->usmUserStatus == D_usmUserStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, usmUserTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_usmUserEntry_UNDO
/* add #define SR_usmUserEntry_UNDO in sitedefs.h to
 * include the undo routine for the usmUserEntry family.
 */
int
usmUserEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
    usmUserEntry_t *data = (usmUserEntry_t *) doCur->data;
    usmUserEntry_t *undodata = (usmUserEntry_t *) doCur->undodata;
    usmUserEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->usmUserStatus = D_usmUserStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->usmUserStatus == D_usmUserStatus_notReady
        || undodata->usmUserStatus == D_usmUserStatus_notInService) {
            undodata->usmUserStatus = D_usmUserStatus_createAndWait;
        } else 
        if(undodata->usmUserStatus == D_usmUserStatus_active) {
            undodata->usmUserStatus = D_usmUserStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_usmUserEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_usmUserEntry_UNDO */

#endif /* SETS */

#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
#ifdef SR_3DES
/* 
 * srCheck3DESKeys:
 * 
 * 	This routine computes the new priv key value given the old priv key
 * value and the key change value. The routine then checks to make sure that
 * all three 3DES keys that are generated are not equal to each other.
 * In other words, key1 != key2, key2 != key3, and key1 != key3.
 *
 * Returns:
 *		0 - success
 *	       -1 - failure
 *
 */
SR_INT32
srCheck3DESKeys(OctetString *privKey, OctetString *keyChange, SR_INT32 authProt)
{

    unsigned char *key1, *key2, *key3;
    OctetString *newPrivKey = NULL;
    SR_INT32 returnValue = -1;

    if ( (privKey == NULL) || (keyChange == NULL) ){
        goto done;
    }

    newPrivKey = CloneOctetString(privKey);
    if (newPrivKey == NULL) {
        goto done;
    }
    UpdateUsmSecret(authProt, keyChange, newPrivKey);
    key1 = newPrivKey->octet_ptr;
    key2 = newPrivKey->octet_ptr + SR_3DES_EACH_KEY_LENGTH;
    key3 = newPrivKey->octet_ptr + (SR_3DES_EACH_KEY_LENGTH * 2);
    if (memcmp(key1, key2, SR_3DES_EACH_KEY_LENGTH) == 0) {
        DPRINTF((APWARN, "srCheck3DESKeys: 3DES key1 and key2 are equal!\n"));
        goto done;
    }
    if (memcmp(key2, key3, SR_3DES_EACH_KEY_LENGTH) == 0) {
        DPRINTF((APWARN, "srCheck3DESKeys: 3DES key2 and key3 are equal!\n"));
        goto done;
    }
    if (memcmp(key1, key3, SR_3DES_EACH_KEY_LENGTH) == 0) {
        DPRINTF((APWARN, "srCheck3DESKeys: 3DES key1 and key3 are equal!\n"));
        goto done;
    }

    returnValue = 0;

done:   
    if (newPrivKey != NULL) {
        FreeOctetString(newPrivKey);
    }
    return returnValue; 
}
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */

void
UpdateUsmSecret(
    SR_INT32 hashAlgorithm,
    OctetString *change,
    OctetString *secret)
{
    switch (hashAlgorithm) {
#ifdef SR_MD5_HASH_ALGORITHM
        case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
            MD5UpdateUsmSecret(change, secret);
            break;
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
        case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
            SHAUpdateUsmSecret(change, secret);
            break;
#endif /* SR_SHA_HASH_ALGORITHM */
    }
}

#ifdef SR_MD5_HASH_ALGORITHM
void
MD5UpdateUsmSecret(
    OctetString *change,
    OctetString *secret)
{
    unsigned char temp[64], new[64], rval[64];
    SR_INT32 temp_len, rval_len;
    MD5_CTX MD;
    int i, j, iter;

    if (change->length == 0) {
        return;
    }

    rval_len = secret->length;
    memcpy(rval, change->octet_ptr, (size_t)secret->length);

    iter = (change->length - secret->length - 1) / MD5_DIGEST_SIZE;

    temp_len = secret->length;
    memcpy(temp, secret->octet_ptr, (size_t)temp_len);

    for (i = 0; i < iter; i++) {
        memcpy(temp + temp_len, rval, (size_t)rval_len);
        SrMD5Init(&MD);
        SrMD5Update(&MD, temp, (unsigned int)(temp_len + rval_len));
        SrMD5Final(temp, &MD);
        for (j = 0; j < MD5_DIGEST_SIZE; j++) {
            new[i*MD5_DIGEST_SIZE+j] =
                temp[j] ^ change->octet_ptr[rval_len+i*MD5_DIGEST_SIZE+j];
        }
        temp_len = MD5_DIGEST_SIZE;
    }
    memcpy(temp + temp_len, rval, (size_t)rval_len);
    SrMD5Init(&MD);
    SrMD5Update(&MD, temp, (unsigned int)(temp_len + rval_len));
    SrMD5Final(temp, &MD);
    for (j = 0; j < change->length - rval_len - (i * MD5_DIGEST_SIZE); j++) {
        new[i*MD5_DIGEST_SIZE+j] =
            temp[j] ^ change->octet_ptr[rval_len+i*MD5_DIGEST_SIZE+j];
    }

    memcpy(secret->octet_ptr, new, secret->length);
}
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
void
SHAUpdateUsmSecret(
    OctetString *change,
    OctetString *secret)
{
    unsigned char temp[64], new[64], rval[64];
    SR_INT32 temp_len, rval_len;
    SHA_CTX MD;
    int i, j, iter;

    if (change->length == 0) {
        return;
    }

    rval_len = secret->length;
    memcpy(rval, change->octet_ptr, secret->length);

    iter = (change->length - secret->length - 1) / SHA_DIGEST_SIZE;

    temp_len = secret->length;
    memcpy(temp, secret->octet_ptr, temp_len);

    for (i = 0; i < iter; i++) {
        memcpy(temp + temp_len, rval, rval_len);
        SHAInit(&MD);
        SHAUpdate(&MD, temp, (unsigned int)(temp_len + rval_len));
        SHAFinal(temp, &MD);
        for (j = 0; j < SHA_DIGEST_SIZE; j++) {
            new[i*SHA_DIGEST_SIZE+j] =
                temp[j] ^ change->octet_ptr[rval_len+i*SHA_DIGEST_SIZE+j];
        }
        temp_len = SHA_DIGEST_SIZE;
    }
    memcpy(temp + temp_len, rval, rval_len);
    SHAInit(&MD);
    SHAUpdate(&MD, temp, (unsigned int)(temp_len + rval_len));
    SHAFinal(temp, &MD);
    for (j = 0; j < change->length - rval_len - (i * SHA_DIGEST_SIZE); j++) {
        new[i*SHA_DIGEST_SIZE+j] =
            temp[j] ^ change->octet_ptr[rval_len+i*SHA_DIGEST_SIZE+j];
    }

    memcpy(secret->octet_ptr, new, secret->length);
}
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */
