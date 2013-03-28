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

#define        WANT_ALL_ERRORS

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>




#include <malloc.h>

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "sr_user.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"


ContextInfo *
AllocContextInfo(
    SR_UINT32 length)
{
    ContextInfo *cip;

    cip = (ContextInfo *) malloc(sizeof(ContextInfo));
    if (cip == NULL) {
        return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset(cip, 0, sizeof(ContextInfo));
#endif	/* SR_CLEAR_MALLOC */
    cip->CIType = CI_NEW_CONTEXT_INFO;
    cip->u.new_context_info.length = length;

    cip->u.new_context_info.tv = (PointerOrObjectSyntax *)
        malloc(sizeof(PointerOrObjectSyntax) * length);
    if (cip->u.new_context_info.tv == NULL) {
        free(cip);
        return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset(cip->u.new_context_info.tv, 0,
           (int)(sizeof(PointerOrObjectSyntax) * length));
#endif	/* SR_CLEAR_MALLOC */
    return cip;
}

ContextInfo *
CloneContextInfo(
    ContextInfo *cip)
{
    ContextInfo *ncip = NULL;
    NewContextInfo *nci, *nnci;
    int i;

    if (cip == NULL) {
        goto fail;
    }

    if (cip->CIType != CI_NEW_CONTEXT_INFO) {
        goto fail;
    }

    ncip = AllocContextInfo(cip->u.new_context_info.length);
    if (ncip == NULL) {
        goto fail;
    }

    nci = &cip->u.new_context_info;
    nnci = &ncip->u.new_context_info;

    for (i = 0; i < (int) nci->length; i++) {
        if (nci->tv[i].tag == NCI_SSEID_FILTER) {
            nnci->tv[i].value.ptr = NULL;
        } 
        else if (nci->tv[i].tag == NCI_AGG_VIEW_LIMIT) {
            nnci->tv[i].value.ptr =  NULL;
        }
        else {
            switch (nci->tv[i].value.os.type) {
                case INTEGER_TYPE:
                    nnci->tv[i].value.os.sl_value =
                        nci->tv[i].value.os.sl_value;
                    break;
                case COUNTER_TYPE:
                case TIME_TICKS_TYPE:
                case GAUGE_TYPE:
                    nnci->tv[i].value.os.ul_value =
                        nci->tv[i].value.os.ul_value;
                    break;
                case OCTET_PRIM_TYPE:
                    nnci->tv[i].value.os.os_value =
                        CloneOctetString(nci->tv[i].value.os.os_value);
                    if ( (nnci->tv[i].value.os.os_value == NULL) &&
                         (nci->tv[i].value.os.os_value != NULL) ) {
                        goto fail;
                    }
                    break;
                case OBJECT_ID_TYPE:
                    nnci->tv[i].value.os.oid_value =
                        CloneOID(nci->tv[i].value.os.oid_value);
                    if ( (nnci->tv[i].value.os.oid_value == NULL) &&
                         (nci->tv[i].value.os.oid_value != NULL) ) {
                        goto fail;
                    }
                    break;
                default:
                    /* No others possible... */
                    break;
            }
            nnci->tv[i].value.os.type = nci->tv[i].value.os.type;
        }
        nnci->tv[i].tag = nci->tv[i].tag;
    }

    return ncip;

  fail:
    if (ncip != NULL) {
        FreeContextInfo(ncip);
    }
    return NULL;
}

/* this routine frees a context information structure that was allocated
 * with findContextInfo(). */
void
FreeContextInfo(cip)
ContextInfo *cip;
{
    SR_UINT32 i;

    if (cip == NULL) {
        return;
    }

    if (cip->CIType != CI_NEW_CONTEXT_INFO) {
        return;
    }

    for (i = 0; i < cip->u.new_context_info.length; i++) {
            switch (cip->u.new_context_info.tv[i].value.os.type) {
                case OCTET_PRIM_TYPE:
                   FreeOctetString(cip->u.new_context_info.tv[i].value.os.os_value);
                   break;
                case OBJECT_ID_TYPE:
                   FreeOID(cip->u.new_context_info.tv[i].value.os.oid_value);
                   break;
                default:
                   break;
            }
    }
    free(cip->u.new_context_info.tv);
    free(cip);
}

ObjectSyntax *
SearchContextInfo(cip, tag)
    ContextInfo *cip;
    SR_UINT32 tag;
{
    SR_UINT32 i;

    if (cip == NULL) {
        return NULL;
    }

    if (cip->CIType != CI_NEW_CONTEXT_INFO) {
        return NULL;
    }

    for (i = 0; i < cip->u.new_context_info.length; i++) {
        if (cip->u.new_context_info.tv[i].tag == tag) {
            return &(cip->u.new_context_info.tv[i].value.os);
        }
    }

    return NULL;
}

void *
SearchAPOContextInfo(ContextInfo *cip, SR_UINT32 tag)
{
    SR_UINT32 i;

    if (cip == NULL) {
        return NULL;
    }

    if (cip->CIType != CI_NEW_CONTEXT_INFO) {
        return NULL;
    }

    for (i = 0; i < cip->u.new_context_info.length; i++) {
        if (cip->u.new_context_info.tv[i].tag == tag) {
            return (cip->u.new_context_info.tv[i].value.ptr);
        }
    }

    return NULL;
}

/* This routine modifies the request-id value in the NewContextInfo */
/* portion of the ContextInfo structure. The purpose of this        */
/* routine is to allow this value to be set *after* the ContextInfo */
/* structure is allocated and initialized                           */
int
SrAssignRequestIDToContextInfo(cip, val)
   ContextInfo *cip;
   SR_INT32 val;
{     
   SR_UINT32 i;
  
   if (cip == NULL) {
      return 0;
   }  
 
   if (cip->CIType !=  CI_NEW_CONTEXT_INFO ) {
      return 0;
   }  
  
   for (i = 0; i < cip->u.new_context_info.length; i++) {
        if (cip->u.new_context_info.tv[i].tag == NCI_REQUEST_ID) {
           cip->u.new_context_info.tv[i].value.os.sl_value = val;
        }
   }

   return 1;
}

/* SrUpdateContextInfoRecord:
 *     This routine allows the value of a ContextInfo record to be
 *     updated in place 
 *
 */
int
SrUpdateContextInfoRecord(ContextInfo *cip, SR_INT32 tag, void *value)
{
    SR_UINT32 i;

    if (cip == NULL) {
        return 0;
    }
    if (cip->CIType != CI_NEW_CONTEXT_INFO) {
        return 0;
    }
 
    for (i = 0; i < cip->u.new_context_info.length; i++) {
        if (cip->u.new_context_info.tv[i].tag == tag) {
                switch (cip->u.new_context_info.tv[i].value.os.type) {
                    case OCTET_PRIM_TYPE:
                    case IP_ADDR_PRIM_TYPE:
                    case OPAQUE_PRIM_TYPE:
                        if (cip->u.new_context_info.tv[i].value.os.os_value != NULL) {

                            FreeOctetString(cip->u.new_context_info.tv[i].value.os.os_value);
                        }
                        cip->u.new_context_info.tv[i].value.os.os_value = CloneOctetString(value);
                        break;
                    case OBJECT_ID_TYPE:
                        if (cip->u.new_context_info.tv[i].value.os.oid_value != NULL) {
                            FreeOID(cip->u.new_context_info.tv[i].value.os.oid_value);
                        }
                        cip->u.new_context_info.tv[i].value.os.oid_value = CloneOID(value);
                        break;
                    case INTEGER_TYPE:
                        cip->u.new_context_info.tv[i].value.os.sl_value = *((SR_INT32 *) value);
                        break;
                    case COUNTER_TYPE:
                    case GAUGE_TYPE:
                    case TIME_TICKS_TYPE:
                        cip->u.new_context_info.tv[i].value.os.ul_value = *((SR_UINT32 *) value);
                        break;
#ifdef SR_SNMPv2_PDU
                    case COUNTER_64_TYPE:
                        cip->u.new_context_info.tv[i].value.os.uint64_value = CloneUInt64((UInt64 *)value);
                        break;
#endif /* SR_SNMPv2_PDU */
                    default:
                        return (-1);
                }
                break;
        }
    }
    return 0;         
} 
