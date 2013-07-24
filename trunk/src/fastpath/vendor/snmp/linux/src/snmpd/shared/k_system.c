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

#include <string.h>


#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "diag.h"
SR_FILENAME

#include "sitedefs.h"

/* LVL7 */
#include "l7_common.h"
#define SNMP_BUFFER_LEN 256
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_snmp_api.h"
/* LVL7 end */

int ipforwarding_state SR_PROTOTYPE((void));



#define SYSSIZE 255

system_t        systemData;

const SnmpType sysDescrType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysDescrConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

const SnmpType sysObjectIDType[] = {
    { OBJECT_ID_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysObjectIDConverters[] = {
    { ConvToken_ValidOID, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */
const SnmpType sysLocationType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysLocationConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */
const SnmpType sysContactType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysContactConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

const SnmpType sysNameType[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, 0, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER sysNameConverters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
#endif	/* SR_CONFIG_FP */

#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE sysDescrRecord = {
    PARSER_SCALAR, "sysDescr ", sysDescrConverters, sysDescrType,
    NULL, &systemData.sysDescr, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysObjectIDRecord = {
    PARSER_SCALAR, "sysObjectID ", sysObjectIDConverters, sysObjectIDType,
    NULL, &systemData.sysObjectID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysLocationRecord = {
    PARSER_SCALAR, "sysLocation ", sysLocationConverters, sysLocationType,
    NULL, &systemData.sysLocation, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysContactRecord = {
    PARSER_SCALAR, "sysContact ", sysContactConverters, sysContactType,
    NULL, &systemData.sysContact, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE sysNameRecord = {
    PARSER_SCALAR, "sysName ", sysNameConverters, sysNameType,
    NULL, &systemData.sysName, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif	/* SR_CONFIG_FP */


int
k_system_initialize(void)
{
    SR_UINT32 sids[2];

    sids[0] = (SR_UINT32)0;
    sids[1] = (SR_UINT32)0;
    systemData.sysDescr = MakeOctetString(NULL, 0);
#ifndef SYS_OBJECT_ID
    systemData.sysObjectID = MakeOID(sids, 2);
#else /* SYS_OBJECT_ID */
    systemData.sysObjectID = MakeOIDFromDot(SYS_OBJECT_ID);
#endif /* SYS_OBJECT_ID */
    systemData.sysUpTime = (SR_UINT32)0;
    systemData.sysContact = MakeOctetString(NULL, 0);
    systemData.sysName = MakeOctetString(NULL, 0);
    systemData.sysLocation = MakeOctetString(NULL, 0);
    systemData.sysServices = (SR_INT32)72;
#ifdef SR_SYS_OR_TABLE
    systemData.sysORLastChange = (SR_UINT32)0;
#endif /* SR_SYS_OR_TABLE */
    return 1;
}

system_t *
k_system_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    FNAME("k_system_get")
#ifdef OLD_CODE /* LVL7 */
    int             ipforwarding;

    ipforwarding = ipforwarding_state();

    if (ipforwarding == 1) {
	systemData.sysServices |= (1 << (3 - 1));  /* %%% MANIFEST CONSTANT */
    }
    else if (ipforwarding == 0) {
	systemData.sysServices &= ~(1 << (3 - 1));
    }
    else {
	return NULL;
    }

    SET_ALL_VALID(systemData.valid);

    if (systemData.sysObjectID != NULL) {
        /* verify validity of OID value */
        if (systemData.sysObjectID->length < 2) {
           DPRINTF((APTRACE,"%s: sysObjectID has length less than 2\n", Fname));
           if (nominator == I_sysObjectID) {
               return NULL;
           }
           CLR_VALID(I_sysObjectID, systemData.valid);
        } else if ( (systemData.sysObjectID->oid_ptr[0] > 2) ||
                    ((systemData.sysObjectID->oid_ptr[0] < 2) &&
                     (systemData.sysObjectID->oid_ptr[1] >= 40)) ) {
           DPRINTF((APTRACE,
                "%s: sysObjectID contains invalid value for first two subids\n",
                 Fname)); 
           if (nominator == I_sysObjectID) {
               return NULL;
           }
           CLR_VALID(I_sysObjectID, systemData.valid);
        }     
    }

    systemData.sysUpTime = GetTimeNow();
#else /* OLD_CODE LVL7 */
    L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   switch (nominator)
   {
      case I_sysDescr:
         CLR_VALID(I_sysDescr, systemData.valid);
         FreeOctetString(systemData.sysDescr);
         /* clear buffer to ensure trailing \0 */
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if (usmDb1213SysDescrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
             (systemData.sysDescr = MakeOctetStringFromText(snmp_buffer)) != NULL)
         {
            SET_VALID(I_sysDescr, systemData.valid);
         }
         else
         {
            systemData.sysDescr = MakeOctetStringFromText("");
         }
         if (nominator != -1)
            break;

      case I_sysObjectID:
         CLR_VALID(I_sysObjectID, systemData.valid);
         FreeOID(systemData.sysObjectID);
         /* clear buffer to ensure trailing \0 */
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if (usmDb1213SysObjectIDGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS )
         {
            if ( (systemData.sysObjectID = MakeOIDFromDot(snmp_buffer)) != NULL )
               SET_VALID(I_sysObjectID, systemData.valid);
         }
         if (nominator != -1)
            break;

      case I_sysUpTime:
         CLR_VALID(I_sysUpTime, systemData.valid);
         if (usmDb1213SysUpTimeRawGet(USMDB_UNIT_CURRENT, &systemData.sysUpTime) == L7_SUCCESS)
         {
           SET_VALID(I_sysUpTime, systemData.valid);
         }
         if (nominator != -1)
            break;

      case I_sysContact:
         CLR_VALID(I_sysContact, systemData.valid);
         FreeOctetString(systemData.sysContact);
         /* clear buffer to ensure trailing \0 */
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if ( usmDb1213SysContactGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS )
         {
            if ( (systemData.sysContact = MakeOctetStringFromText(snmp_buffer)) != NULL )
               SET_VALID(I_sysContact, systemData.valid);
         }
         else
         {
            systemData.sysContact = MakeOctetStringFromText("");
         }
         if (nominator != -1)
            break;
            
      case I_sysName:
         CLR_VALID(I_sysName, systemData.valid);
         FreeOctetString(systemData.sysName);
         /* clear buffer to ensure trailing \0 */
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if ( usmDb1213SysNameGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS )
         {
            if ( (systemData.sysName = MakeOctetStringFromText(snmp_buffer)) != NULL )
               SET_VALID(I_sysName, systemData.valid);
         }
         else
         {
            systemData.sysName = MakeOctetStringFromText("");
         }
         if (nominator != -1)
            break;

      case I_sysLocation:
         CLR_VALID(I_sysLocation, systemData.valid);
         FreeOctetString(systemData.sysLocation);
         /* clear buffer to ensure trailing \0 */
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if ( usmDb1213SysLocationGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS )
         {
            if ( (systemData.sysLocation = MakeOctetStringFromText(snmp_buffer)) != NULL )
               SET_VALID(I_sysLocation, systemData.valid);
         }
         else
         {
            systemData.sysLocation = MakeOctetStringFromText("");
         }
         if (nominator != -1)
            break;

      case I_sysServices:
         CLR_VALID(I_sysServices, systemData.valid);
         if (usmDb1213SysServicesGet(USMDB_UNIT_CURRENT, &systemData.sysServices) == L7_SUCCESS )
         {
            SET_VALID(I_sysServices, systemData.valid);
         }
         if (nominator != -1)
            break;

      case I_sysORLastChange:
        CLR_VALID(I_sysORLastChange, systemData.valid);
        if (usmDbSnmpSupportedMibTableLastChangeTimeGet(USMDB_UNIT_CURRENT, &systemData.sysORLastChange) == L7_SUCCESS)
        {
          SET_VALID(I_sysORLastChange, systemData.valid);
        }
        if (nominator != -1)
           break;
   }

    if ( nominator >= 0 && !VALID(nominator, systemData.valid) )
      return(NULL);
#endif /* OLD_CODE LVL7 */
    return (&systemData);
}

#ifdef SETS
int
k_system_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
#if ((defined(I_sysContact) || defined(I_sysName)) || defined(I_sysLocation))


    switch (object->nominator) {
#ifdef I_sysContact
    case I_sysContact:
#endif
#ifdef I_sysName
    case I_sysName:
#endif
#ifdef I_sysLocation
    case I_sysLocation:
#endif
	if ( (value->os_value->length > SYSSIZE) || 
             (value->os_value->length < 0) ){
	    return (WRONG_LENGTH_ERROR);
	}
        if (!IsDisplayString(value->os_value)) {
	    return (WRONG_VALUE_ERROR);
	}
	break;
    }
#endif	/* ((defined(I_sysContact) || defined(I_sysName)) || defined(I_sysLocation)) */
    return NO_ERROR;
}

int
k_system_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_system_set(data, contextInfo, function)
   system_t *data;
   ContextInfo *contextInfo;
   int function;
{
    FNAME("k_system_set")

#ifdef OLD_CODE /* LVL7 */
    if (VALID(I_sysName, data->valid)) {
        if (systemData.sysName != NULL) {
            FreeOctetString(systemData.sysName);
        }
        if ((systemData.sysName = CloneOctetString(data->sysName)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysName.\n", Fname));
            return (GEN_ERROR);
        }
    }
    if (VALID(I_sysContact, data->valid)) {
        if (systemData.sysContact != NULL) {
            FreeOctetString(systemData.sysContact);
        }
        if ((systemData.sysContact = 
             CloneOctetString(data->sysContact)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysContact.\n", Fname));
            return (GEN_ERROR);
        }
    }
    if (VALID(I_sysLocation, data->valid)) {
        if (systemData.sysLocation != NULL) {
            FreeOctetString(systemData.sysLocation);
        }
        if ((systemData.sysLocation = 
             CloneOctetString(data->sysLocation)) == NULL) {
            DPRINTF((APERROR, "%s: Could not set sysLocation.\n", Fname));
            return (GEN_ERROR);
        }
    }
    writeConfigFileFlag = TRUE;
#else /* OLD_CODE LVL7 */
    L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   if (VALID(I_sysContact, data->valid))
   {
      /* clear buffer to ensure trailing \0 */
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, data->sysContact->octet_ptr, data->sysContact->length);
      if ( usmDb1213SysContactSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
         return (COMMIT_FAILED_ERROR);
   }
   if (VALID(I_sysName, data->valid)) 
   {
      /* clear buffer to ensure trailing \0 */
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, data->sysName->octet_ptr, data->sysName->length);
      if ( usmDb1213SysNameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
         return (COMMIT_FAILED_ERROR);
   }
   if (VALID(I_sysLocation, data->valid))
   {
      /* clear buffer to ensure trailing \0 */
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, data->sysLocation->octet_ptr, data->sysLocation->length);
      if ( usmDb1213SysLocationSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
         return (COMMIT_FAILED_ERROR);
   }

#endif /* OLD_CODE LVL7*/
   return (NO_ERROR);
}

int
system_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int             state = SR_ADD_MODIFY;
    system_t       *undodata = (system_t *) doCur->undodata;


    if (undodata == NULL) {
        return UNDO_FAILED_ERROR;
    }

    return k_system_set(undodata, contextInfo, state);
}


#endif /* SETS */
