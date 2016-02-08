/*
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
#include "tmq.h"
#include "rowstatf.h"
#include "i_tgt.h"
#include "tdomain.h"
#include "oid_lib.h"


/* global data describing the snmpTargetObjects family */
static snmpTargetObjects_t *snmpTargetObjectsData;

static SnmpV2Table *snmpTargetAddrTable;

void DeletesnmpTargetAddrEntry
    SR_PROTOTYPE((int index));

static SnmpV2Table *snmpTargetParamsTable;

void DeletesnmpTargetParamsEntry
    SR_PROTOTYPE((int index));

/* initialize support for snmpTargetObjects objects */
int
k_snmpTargetObjects_initialize(
    snmpTargetObjects_t *sto)
{
    snmpTargetObjectsData = sto;
    return 1;
}

/* terminate support for snmpTargetObjects objects */
int
k_snmpTargetObjects_terminate(void)
{
    snmpTargetObjectsData = NULL;
    return 1;
}

/* initialize support for snmpTargetAddrEntry objects */
int
k_snmpTargetAddrEntry_initialize(
    SnmpV2Table *stat)
{
    snmpTargetAddrTable = stat;
    return 1;
}

/* terminate support for snmpTargetAddrEntry objects */
int
k_snmpTargetAddrEntry_terminate(void)
{
    snmpTargetAddrTable = NULL;
    return 1;
}

/* initialize support for snmpTargetParamsEntry objects */
int
k_snmpTargetParamsEntry_initialize(
    SnmpV2Table *stpt)
{
    snmpTargetParamsTable = stpt;
    return 1;
}

/* terminate support for snmpTargetParamsEntry objects */
int
k_snmpTargetParamsEntry_terminate(void)
{
    snmpTargetParamsTable = NULL;
    return 1;
}

int
k_tgt_initialize(
    snmpTargetObjects_t *sto,
    SnmpV2Table *stat,
    SnmpV2Table *stpt)
{
    int status = 1;

    if (k_snmpTargetObjects_initialize(sto) != 1) {
        status = -1;
    }
    if (k_snmpTargetAddrEntry_initialize(stat) != 1) {
        status = -1;
    }
    if (k_snmpTargetParamsEntry_initialize(stpt) != 1) {
        status = -1;
    }

    return status;
}

int
k_tgt_terminate(void)
{
    int status = 1;

    if (k_snmpTargetObjects_terminate() != 1) {
        status = -1;
    }
    if (k_snmpTargetAddrEntry_terminate() != 1) {
        status = -1;
    }
    if (k_snmpTargetParamsEntry_terminate() != 1) {
        status = -1;
    }

    return status;
}

#ifdef U_snmpTargetObjects
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetObjectsFreeUserpartData (snmpTargetObjects_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpTargetObjects */

#ifdef U_snmpTargetObjects
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetObjectsCloneUserpartData (snmpTargetObjects_t *dst, snmpTargetObjects_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpTargetObjects */

snmpTargetObjects_t *
k_snmpTargetObjects_get(int serialNum, ContextInfo *contextInfo,
                        int nominator)
{
   return snmpTargetObjectsData;
}

#ifdef SETS
int
k_snmpTargetObjects_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{
    if (object->nominator == I_snmpTargetSpinLock) {
        if ( value->sl_value != snmpTargetObjectsData->snmpTargetSpinLock) {
            return INCONSISTENT_VALUE_ERROR;
        }
    }
    return NO_ERROR;
}

int
k_snmpTargetObjects_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_snmpTargetObjects_set(snmpTargetObjects_t *data,
                        ContextInfo *contextInfo, int function)
{
   if (data->snmpTargetSpinLock != 2147483647) {
       snmpTargetObjectsData->snmpTargetSpinLock = ++data->snmpTargetSpinLock;
   }
   else {
       snmpTargetObjectsData->snmpTargetSpinLock = 0;
   }
   return NO_ERROR;
}

#ifdef SR_snmpTargetObjects_UNDO
/* add #define SR_snmpTargetObjects_UNDO in sitedefs.h to
 * include the undo routine for the snmpTargetObjects family.
 */
int
snmpTargetObjects_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpTargetObjects_UNDO */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
snmpTargetAddrTableDeleteCallback (TimeOutDescriptor *tdp)
{
    snmpTargetAddrEntry_t *data;

    /* dummy up an entry to delete */
    data = (snmpTargetAddrEntry_t *) tdp->UserData2;
    data->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_snmpTargetAddrEntry_set(data, (ContextInfo *) NULL, 0);
}

#endif /* SETS */

/* This routine deletes an entry from the snmpTargetAddrTable */
void
DeletesnmpTargetAddrEntry (int index)
{
    FreeEntries(snmpTargetAddrEntryTypeTable, snmpTargetAddrTable->tp[index]);
    free(snmpTargetAddrTable->tp[index]);
    RemoveTableEntry(snmpTargetAddrTable, index);
}

/* This prototype is required to please some compilers */
int snmpTargetAddrEntryReadyToActivate
    SR_PROTOTYPE((snmpTargetAddrEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpTargetAddrEntryReadyToActivate(snmpTargetAddrEntry_t *data)
{
    /*
     * check valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    if (!VALID(I_snmpTargetAddrTDomain, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetAddrTAddress, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetAddrParams, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetAddrRowStatus, data->valid)) {
        return(0);
    }
    return(1);
}

#ifdef U_snmpTargetAddrEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetAddrEntryFreeUserpartData (snmpTargetAddrEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpTargetAddrEntry */

#ifdef U_snmpTargetAddrEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetAddrEntryCloneUserpartData (snmpTargetAddrEntry_t *dst, snmpTargetAddrEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpTargetAddrEntry */

snmpTargetAddrEntry_t *
k_snmpTargetAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          OctetString * snmpTargetAddrName)
{
    int index;
    snmpTargetAddrEntry_t *stae;

    snmpTargetAddrTable->tip[0].value.octet_val = snmpTargetAddrName;
    while ((index = SearchTable(snmpTargetAddrTable, searchType)) != -1) {
/*
 *  The following special code is required by the following 
 *  circumstances:
 *
 *  .  The snmpTargetAddrParams entry makes little sense when
 *     the snmpTargetAddrEntry is only being used by a usmTargetTags
 *     entry, and
 *  .  the value for the snmpTargetAddrParams specified in a config
 *     file is - (empty string).  Historically, we allow this.
 */

        stae = (snmpTargetAddrEntry_t *) snmpTargetAddrTable->tp[index];
        if ( (stae->snmpTargetAddrParams != NULL) &&
             (stae->snmpTargetAddrParams->length == 0) ) {
            if (nominator == I_snmpTargetAddrParams) {
                if (searchType == EXACT) {
                    return NULL;
                }
                else {
                    /* Skip this entry, find next entry with valid value for
                     * snmpTargetAddrParams object 
                     */
                    snmpTargetAddrTable->tip[0].value.octet_val = 
						   stae->snmpTargetAddrName;
                    while((index = 
			   SearchTable(snmpTargetAddrTable, NEXT_SKIP)) != -1){
                        stae = (snmpTargetAddrEntry_t *) 
					snmpTargetAddrTable->tp[index];
                        if (stae->snmpTargetAddrParams->length != 0) {
                            break; 
                        }
                        snmpTargetAddrTable->tip[0].value.octet_val = 
						    stae->snmpTargetAddrName;
                    }
                    if (index == -1) { 
                        return NULL;
                    }
                }
            }
            else {
                CLR_VALID(I_snmpTargetAddrParams, stae->valid);
            }
        }

        if (nominator < 0 || VALID(nominator, stae->valid)) {
            return stae;
        } else {
	    snmpTargetAddrTable->tip[0].value.octet_val = 
					     stae->snmpTargetAddrName;
            searchType = NEXT_SKIP;
        }
    }
    return NULL;
}

#ifdef SETS
int
IsValidSnmpTagList(OctetString *stl)
{
    FNAME("IsValidSnmpTagList")
    int i=0, delimiter=0;

    /* check for NULL parameter */
    if (stl == NULL) {
        DPRINTF((APWARN, "%s: NULL parameter\n", Fname));
        return 0;
    }
    /* check for zero-length OctetString */
    if (stl->length == 0) {
        /*
         * From RFC 2573, Page 34:
         *    Note that although a tag value may not have a length of
         *    zero, an empty string is still valid.  This indicates
         *    an empty list (i.e. there are no tag values in the list).
         */
        return 1;
    }
    /* check for invalid parameter */
    if ((stl->octet_ptr == NULL) || (stl->length < 0)) {
        DPRINTF((APERROR, "%s: invalid OctetString\n", Fname));
        return 0;
    }
    /* check for an OctetString that is too long */
    if (stl->length > 255) {
        DPRINTF((APTRACE, "%s: OctetString is too long\n", Fname));
        return 0;
    }
    /* check for OctetString contents */
    for (i=0; i<stl->length; i++) {
        switch (stl->octet_ptr[i]) {
            case '\040':  /* 0x20 space in octal */
            case '\011':  /* 0x09 horizontal tab in octal */
            case '\015':  /* 0x0D carriage return in octal */
            case '\012':  /* 0x0A line feed in octal */
            case '\013':  /* 0x0B linefeed (vertical tab) in octal */
                if (i==0) {
                    /* the first octet contains a delimiter */
                    DPRINTF((APTRACE,
                        "%s: leading delimiter character\n", Fname));
                    return 0;
                }
                if (delimiter) {
                    /*
                     * The previous octet contains a delimiter.
                     * There are two delimiters in a row.
                     */
                    DPRINTF((APTRACE,
                        "%s: multiple adjacent delimiter characters\n",
                        Fname));
                    return 0;
                }
                /* the current octet contains a delimiter */
                delimiter = 1;
                break;
            default:
                /* the current octet does not contain a delimiter */
                delimiter = 0;
                break;
        }
    }
    if (delimiter) {
        /* the final octet contains a delimiter */
        DPRINTF((APTRACE, "%s: trailing delimiter character\n", Fname));
        return 0;
    }
    /* valid SnmpTagList */
    return 1;
}

int
k_snmpTargetAddrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{
    snmpTargetAddrEntry_t
        *proposed_new_state = (snmpTargetAddrEntry_t *) dp->data;

    if (object->nominator == I_snmpTargetAddrTagList) {
        if (IsValidSnmpTagList(proposed_new_state->snmpTargetAddrTagList)) {
            return NO_ERROR;
        } else {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_snmpTargetAddrTAddress) {
       if ((proposed_new_state->snmpTargetAddrTDomain != NULL ) &&
           (CmpOID(proposed_new_state->snmpTargetAddrTDomain, snmpUDPDomainOID) == 0)) {
              if ( proposed_new_state->snmpTargetAddrTAddress->length != 6) {
                 return WRONG_VALUE_ERROR;
              }
       }
    }

    return NO_ERROR;
}

int
k_snmpTargetAddrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{
    snmpTargetAddrEntry_t
        *proposed_new_state = (snmpTargetAddrEntry_t *) dp->data;

/*
 *  ready test must past both constraints so initial state
 *  of SR_UNKNOWN appropriate
 */
    dp->state = SR_UNKNOWN;


    if( proposed_new_state->snmpTargetAddrRowStatus ==
        D_snmpTargetAddrRowStatus_active && (
        VALID(I_snmpTargetAddrTDomain, proposed_new_state->valid) ||
        VALID(I_snmpTargetAddrTAddress, proposed_new_state->valid) ) )
    {
      /*
       *  From MIB:
       *  The following objects may not be modified while the
       *  value of this object is active(1):
       *    - snmpTargetAddrTDomain
       *    - snmpTargetAddrTAddress
       *  An attempt to set these objects while the value of
       *  snmpTargetAddrRowStatus is active(1) will result in
       *  an inconsistentValue error.
       */
       dp->state = SR_UNKNOWN;
    }
    else
    {
       if (VALID(I_snmpTargetAddrRowStatus, proposed_new_state->valid)) 
       {
           switch(proposed_new_state->snmpTargetAddrRowStatus) {
               case D_snmpTargetAddrRowStatus_destroy:
                   dp->state = SR_DELETE;
                   break;
               case D_snmpTargetAddrRowStatus_createAndWait:
               case D_snmpTargetAddrRowStatus_notReady:
                   dp->state = SR_ADD_MODIFY;
                   break;
               case D_snmpTargetAddrRowStatus_createAndGo:
               case D_snmpTargetAddrRowStatus_active:
               case D_snmpTargetAddrRowStatus_notInService:
                   if(snmpTargetAddrEntryReadyToActivate
                                            (proposed_new_state) == 1) {
                       dp->state = SR_ADD_MODIFY;
                   } else {
                       dp->state = SR_UNKNOWN;
                   }
                   break;
               default:
                   dp->state = SR_UNKNOWN;
                   break;
           }
        }

      /*
       *  Target address and mask must have same length, or target mask
       *  length must be zero.
       */
       if (VALID(I_snmpTargetAddrTMask, proposed_new_state->valid) &&
                  VALID(I_snmpTargetAddrTAddress, proposed_new_state->valid)) 
       {
          if ((proposed_new_state->snmpTargetAddrTMask->length != 0) &&
              (proposed_new_state->snmpTargetAddrTMask->length !=
               proposed_new_state->snmpTargetAddrTAddress->length))
          {
               dp->state = SR_UNKNOWN;
          }
       }
    }

    return NO_ERROR;
}

int
k_snmpTargetAddrEntry_set_defaults(doList_t *dp)
{
    snmpTargetAddrEntry_t *data = (snmpTargetAddrEntry_t *) (dp->data);

    if ((data->snmpTargetAddrTDomain = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpTargetAddrTAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpTargetAddrTimeout = 1500;    /* TimeInterval / 1/100 seconds */
    data->snmpTargetAddrRetryCount = 3;
    if ((data->snmpTargetAddrTagList = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpTargetAddrParams = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpTargetAddrStorageType = D_snmpTargetAddrStorageType_nonVolatile;
    if ((data->snmpTargetAddrTMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpTargetAddrMMS = 2048;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);
    /*
     * clear valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    CLR_VALID(I_snmpTargetAddrTDomain, data->valid);
    CLR_VALID(I_snmpTargetAddrTAddress, data->valid);
    CLR_VALID(I_snmpTargetAddrParams, data->valid);
    CLR_VALID(I_snmpTargetAddrRowStatus, data->valid);

    if (!IsValidSnmpTagList(data->snmpTargetAddrTagList)) {
        CLR_VALID(I_snmpTargetAddrTagList, data->valid);
    }
    return NO_ERROR;
}

int
k_snmpTargetAddrEntry_set(snmpTargetAddrEntry_t *data,
                          ContextInfo *contextInfo, int function)
{

    int index;
    snmpTargetAddrEntry_t *newrow = NULL;

    /* find this entry in the table */
    snmpTargetAddrTable->tip[0].value.octet_val = data->snmpTargetAddrName;
    if ((index = SearchTable(snmpTargetAddrTable, EXACT)) != -1) {
        newrow = (snmpTargetAddrEntry_t *) snmpTargetAddrTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->snmpTargetAddrStorageType;
            DeletesnmpTargetAddrEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpTargetAddrTable->tip[0].value.octet_val = data->snmpTargetAddrName;
        if ((index = NewTableEntry(snmpTargetAddrTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpTargetAddrEntry_t *) snmpTargetAddrTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpTargetAddrTableDeleteCallback)) == -1) {
            DeletesnmpTargetAddrEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_createAndWait
    || data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_notReady) {
        /* is the entry ready? */
        if(snmpTargetAddrEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_notReady;
        }
    }

    /* if we are executing a createAndGo command, transition to active */
    if(data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_createAndGo) {
         data->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_active;
    }

    if (newrow->snmpTargetAddrStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->snmpTargetAddrStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpTargetAddrMMS, snmpTargetAddrEntryTypeTable, (void *) newrow, (void *) data);
    memcpy((char *) newrow->valid, (char *) data->valid, sizeof(data->valid));

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_notInService || newrow->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpTargetAddrTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpTargetAddrEntry_UNDO
/* add #define SR_snmpTargetAddrEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpTargetAddrEntry family.
 */
int
snmpTargetAddrEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
    snmpTargetAddrEntry_t *data = (snmpTargetAddrEntry_t *) doCur->data;
    snmpTargetAddrEntry_t *undodata = (snmpTargetAddrEntry_t *) doCur->undodata;
    snmpTargetAddrEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_notReady
        || undodata->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_notInService) {
            undodata->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_createAndWait;
        } else 
        if(undodata->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_active) {
            undodata->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpTargetAddrEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpTargetAddrEntry_UNDO */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
snmpTargetParamsTableDeleteCallback (TimeOutDescriptor *tdp)
{
    snmpTargetParamsEntry_t *data;

    /* dummy up an entry to delete */
    data = (snmpTargetParamsEntry_t *) tdp->UserData2;
    data->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_snmpTargetParamsEntry_set(data, (ContextInfo *) NULL, 0);
}

#endif /* SETS */

/* This routine deletes an entry from the snmpTargetParamsTable */
void
DeletesnmpTargetParamsEntry (int index)
{
    FreeEntries(snmpTargetParamsEntryTypeTable, snmpTargetParamsTable->tp[index]);
    free(snmpTargetParamsTable->tp[index]);
    RemoveTableEntry(snmpTargetParamsTable, index);
}

/* This prototype is required to please some compilers */
int snmpTargetParamsEntryReadyToActivate
    SR_PROTOTYPE((snmpTargetParamsEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpTargetParamsEntryReadyToActivate(snmpTargetParamsEntry_t *data)
{
    /*
     * check valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    if (!VALID(I_snmpTargetParamsMPModel, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetParamsSecurityModel, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetParamsSecurityName, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetParamsSecurityLevel, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpTargetParamsRowStatus, data->valid)) {
        return(0);
    }
    return(1);
}

#ifdef U_snmpTargetParamsEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetParamsEntryFreeUserpartData (snmpTargetParamsEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpTargetParamsEntry */

#ifdef U_snmpTargetParamsEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpTargetParamsEntryCloneUserpartData (snmpTargetParamsEntry_t *dst, snmpTargetParamsEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpTargetParamsEntry */

snmpTargetParamsEntry_t *
k_snmpTargetParamsEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            OctetString * snmpTargetParamsName)
{
    int index;
    snmpTargetParamsEntry_t *data;
    snmpTargetParamsTable->tip[0].value.octet_val = snmpTargetParamsName;
    while ((index = SearchTable(snmpTargetParamsTable, searchType)) != -1) {
        data = (snmpTargetParamsEntry_t *) snmpTargetParamsTable->tp[index];
        if (nominator < 0 || VALID(nominator, data->valid)) {
            return data;
        } else {
	    snmpTargetParamsTable->tip[0].value.octet_val = 
                                           data->snmpTargetParamsName;
            searchType = NEXT_SKIP;
        }
    }
    return NULL;
}

#ifdef SETS
int
k_snmpTargetParamsEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_snmpTargetParamsEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{
    snmpTargetParamsEntry_t
        *proposed_new_state = (snmpTargetParamsEntry_t *) dp->data;

    if (VALID(I_snmpTargetParamsRowStatus, proposed_new_state->valid)) {
        switch(proposed_new_state->snmpTargetParamsRowStatus) {
            case D_snmpTargetParamsRowStatus_destroy:
                dp->state = SR_DELETE;
                break;
            case D_snmpTargetParamsRowStatus_createAndWait:
            case D_snmpTargetParamsRowStatus_notReady:
                dp->state = SR_ADD_MODIFY;
                break;
            case D_snmpTargetParamsRowStatus_createAndGo:
            case D_snmpTargetParamsRowStatus_active:
            case D_snmpTargetParamsRowStatus_notInService:
                if(snmpTargetParamsEntryReadyToActivate
                                            (proposed_new_state) == 1) {
                    dp->state = SR_ADD_MODIFY;
                } else {
                    dp->state = SR_UNKNOWN;
                }
                break;
            default:
                dp->state = SR_UNKNOWN;
        }
    } else {
        dp->state = SR_UNKNOWN;
    }
    return NO_ERROR;
}

int
k_snmpTargetParamsEntry_set_defaults(doList_t *dp)
{
    snmpTargetParamsEntry_t *data = (snmpTargetParamsEntry_t *) (dp->data);

    if ((data->snmpTargetParamsSecurityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpTargetParamsStorageType = 
        D_snmpTargetParamsStorageType_nonVolatile;
    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);
    /*
     * clear valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    CLR_VALID(I_snmpTargetParamsMPModel, data->valid);
    CLR_VALID(I_snmpTargetParamsSecurityModel, data->valid);
    CLR_VALID(I_snmpTargetParamsSecurityName, data->valid);
    CLR_VALID(I_snmpTargetParamsSecurityLevel, data->valid);
    CLR_VALID(I_snmpTargetParamsRowStatus, data->valid);
    return NO_ERROR;
}

int
k_snmpTargetParamsEntry_set(snmpTargetParamsEntry_t *data,
                            ContextInfo *contextInfo, int function)
{

    int index;
    snmpTargetParamsEntry_t *newrow = NULL;

    /* find this entry in the table */
    snmpTargetParamsTable->tip[0].value.octet_val = data->snmpTargetParamsName;
    if ((index = SearchTable(snmpTargetParamsTable, EXACT)) != -1) {
        newrow = (snmpTargetParamsEntry_t *) snmpTargetParamsTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->snmpTargetParamsStorageType;
            DeletesnmpTargetParamsEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpTargetParamsTable->tip[0].value.octet_val = data->snmpTargetParamsName;
        if ((index = NewTableEntry(snmpTargetParamsTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpTargetParamsEntry_t *) snmpTargetParamsTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpTargetParamsTableDeleteCallback)) == -1) {
            DeletesnmpTargetParamsEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_createAndWait
    || data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_notReady) {
        /* is the entry ready? */
        if(snmpTargetParamsEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_notReady;
        }
    }

    /* if we are executing a createAndGo command, transition to active */
    if(data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_createAndGo) {
         data->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_active;
    }

    if (newrow->snmpTargetParamsStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->snmpTargetParamsStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpTargetParamsRowStatus, snmpTargetParamsEntryTypeTable, (void *) newrow, (void *) data);
    memcpy((char *) newrow->valid, (char *) data->valid, sizeof(data->valid));

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_notInService || newrow->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpTargetParamsTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpTargetParamsEntry_UNDO
/* add #define SR_snmpTargetParamsEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpTargetParamsEntry family.
 */
int
snmpTargetParamsEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
    snmpTargetParamsEntry_t *data = (snmpTargetParamsEntry_t *) doCur->data;
    snmpTargetParamsEntry_t *undodata = (snmpTargetParamsEntry_t *) doCur->undodata;
    snmpTargetParamsEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_notReady
        || undodata->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_notInService) {
            undodata->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_createAndWait;
        } else 
        if(undodata->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_active) {
            undodata->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpTargetParamsEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpTargetParamsEntry_UNDO */

#endif /* SETS */
