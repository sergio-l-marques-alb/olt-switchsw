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
#include "i_notif.h"


static SnmpV2Table *snmpNotifyTable;

void DeletesnmpNotifyEntry
    SR_PROTOTYPE((int index));

static SnmpV2Table *snmpNotifyFilterProfileTable;

void DeletesnmpNotifyFilterProfileEntry
    SR_PROTOTYPE((int index));

static SnmpV2Table *snmpNotifyFilterTable;

void DeletesnmpNotifyFilterEntry
    SR_PROTOTYPE((int index));

/* initialize support for snmpNotifyEntry objects */
int
k_snmpNotifyEntry_initialize(
    SnmpV2Table *snt)
{
    snmpNotifyTable = snt;
    return 1;
}

/* terminate support for snmpNotifyEntry objects */
int
k_snmpNotifyEntry_terminate(void)
{
    snmpNotifyTable = NULL;
    return 1;
}

/* initialize support for snmpNotifyFilterProfileEntry objects */
int
k_snmpNotifyFilterProfileEntry_initialize(
    SnmpV2Table *snfpt)
{
    snmpNotifyFilterProfileTable = snfpt;
    return 1;
}

/* terminate support for snmpNotifyFilterProfileEntry objects */
int
k_snmpNotifyFilterProfileEntry_terminate(void)
{
    snmpNotifyFilterProfileTable = NULL;
    return 1;
}

/* initialize support for snmpNotifyFilterEntry objects */
int
k_snmpNotifyFilterEntry_initialize(
    SnmpV2Table *snft)
{
    snmpNotifyFilterTable = snft;
    return 1;
}

/* terminate support for snmpNotifyFilterEntry objects */
int
k_snmpNotifyFilterEntry_terminate(void)
{
    snmpNotifyFilterTable = NULL;
    return 1;
}

int
k_notif_initialize(
    SnmpV2Table *snt,
    SnmpV2Table *snfpt,
    SnmpV2Table *snft)
{
    int status = 1;

    if (k_snmpNotifyEntry_initialize(snt) != 1) {
        status = -1;
    }
    if (k_snmpNotifyFilterProfileEntry_initialize(snfpt) != 1) {
        status = -1;
    }
    if (k_snmpNotifyFilterEntry_initialize(snft) != 1) {
        status = -1;
    }

    return status;
}

int
k_notif_terminate(void)
{
    int status = 1;

    if (k_snmpNotifyEntry_terminate() != 1) {
        status = -1;
    }
    if (k_snmpNotifyFilterProfileEntry_terminate() != 1) {
        status = -1;
    }
    if (k_snmpNotifyFilterEntry_terminate() != 1) {
        status = -1;
    }

    return status;
}

/* This prototype is required to please some compilers */
int snmpNotifyEntryReadyToActivate
    SR_PROTOTYPE((snmpNotifyEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpNotifyEntryReadyToActivate(snmpNotifyEntry_t *data)
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_snmpNotifyEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyEntryFreeUserpartData (snmpNotifyEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpNotifyEntry */

#ifdef U_snmpNotifyEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyEntryCloneUserpartData (snmpNotifyEntry_t *dst, snmpNotifyEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpNotifyEntry */

snmpNotifyEntry_t *
k_snmpNotifyEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      OctetString * snmpNotifyName)
{
    int index;
    snmpNotifyEntry_t *data;

    snmpNotifyTable->tip[0].value.octet_val = snmpNotifyName;
    while ((index = SearchTable(snmpNotifyTable, searchType)) != -1) {
        data = (snmpNotifyEntry_t *) snmpNotifyTable->tp[index];
        if (nominator < 0 || VALID(nominator, data->valid)) {
            return data;
        } else {
            snmpNotifyTable->tip[0].value.octet_val = data->snmpNotifyName;
            searchType = NEXT_SKIP;
        }
    }
    return NULL;
}

#ifdef SETS
int
IsValidSnmpTagValue(OctetString *stv)
{
    FNAME("IsValidSnmpTagValue")
    int i=0;

    /* check for NULL parameter */
    if (stv == NULL) {
        DPRINTF((APWARN, "%s: NULL parameter\n", Fname));
        return 0;
    }
    /* check for zero-length OctetString */
    if (stv->length == 0) {
        /*
         * From RFC 2573, Page 33:
         *    SYNTAX       OCTET STRING (SIZE (0..255))
         */
        return 1;
    }
    /* check for invalid parameter */
    if ((stv->octet_ptr == NULL) || (stv->length < 0)) {
        DPRINTF((APERROR, "%s: invalid OctetString\n", Fname));
        return 0;
    }
    /* check for an OctetString that is too long */
    if (stv->length > 255) {
        DPRINTF((APTRACE, "%s: OctetString is too long\n", Fname));
        return 0;
    }
    /* check for OctetString contents */
    for (i=0; i<stv->length; i++) {
        switch (stv->octet_ptr[i]) {
            case '\040':  /* 0x20 space in octal */
            case '\011':  /* 0x09 horizontal tab in octal */
            case '\015':  /* 0x0D carriage return in octal */
            case '\012':  /* 0x0A line feed in octal */
            case '\013':  /* 0x0B linefeed (vertical tab) in octal */
                /* the current octet contains a delimiter */
                DPRINTF((APTRACE,
                    "%s: contains a delimiter character\n", Fname));
                return 0;
        }
    }
    /* valid SnmpTagValue */
    return 1;
}


/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void
snmpNotifyTableDeleteCallback (TimeOutDescriptor *tdp)
{
    snmpNotifyEntry_t *data;
 
    /* dummy up an entry to delete */
    data = (snmpNotifyEntry_t *) tdp->UserData2;
    data->snmpNotifyRowStatus = D_snmpNotifyRowStatus_destroy;
    data->RowStatusTimerId = -1;
    
    /* free the timeout descriptor */ 
    free(tdp);
    
    /* call the set method */
    k_snmpNotifyEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the snmpNotifyTable */
void
DeletesnmpNotifyEntry (int index)
{
    FreeEntries(snmpNotifyEntryTypeTable, snmpNotifyTable->tp[index]);
    free(snmpNotifyTable->tp[index]);
    RemoveTableEntry(snmpNotifyTable, index);
}

int
k_snmpNotifyEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{
    snmpNotifyEntry_t
        *proposed_new_state = (snmpNotifyEntry_t *) dp->data;

    if (object->nominator == I_snmpNotifyTag) {
        if (IsValidSnmpTagValue(proposed_new_state->snmpNotifyTag)) {
            return NO_ERROR;
        } else {
            return WRONG_VALUE_ERROR;
        }
    }
    return NO_ERROR;
}

int
k_snmpNotifyEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    snmpNotifyEntry_t *data = (snmpNotifyEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->snmpNotifyRowStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_createAndGo) {
            data->snmpNotifyRowStatus = D_snmpNotifyRowStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_active
        || data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notInService) {
            if(snmpNotifyEntryReadyToActivate(data) == 0) {
                /* inconsistent values, don't allow the set operation */
                doListReady = 0;
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
k_snmpNotifyEntry_set_defaults(doList_t *dp)
{
    snmpNotifyEntry_t *data = (snmpNotifyEntry_t *) (dp->data);

    if ((data->snmpNotifyTag = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpNotifyType = D_snmpNotifyType_trap;
    data->RowStatusTimerId = -1;
    data->snmpNotifyStorageType = D_snmpNotifyStorageType_nonVolatile;

    SET_ALL_VALID(data->valid);
    if (IsValidSnmpTagValue(data->snmpNotifyTag)) {
        CLR_VALID(I_snmpNotifyTag, data->valid);
    }
    return NO_ERROR;
}

int
k_snmpNotifyEntry_set(snmpNotifyEntry_t *data,
                      ContextInfo *contextInfo, int function)
{

    int index;
    snmpNotifyEntry_t *newrow = NULL;

    /* find this entry in the table */
    snmpNotifyTable->tip[0].value.octet_val = data->snmpNotifyName;
    if ((index = SearchTable(snmpNotifyTable, EXACT)) != -1) {
        newrow = (snmpNotifyEntry_t *) snmpNotifyTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->snmpNotifyStorageType;
            DeletesnmpNotifyEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpNotifyTable->tip[0].value.octet_val = data->snmpNotifyName;
        if ((index = NewTableEntry(snmpNotifyTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpNotifyEntry_t *) snmpNotifyTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyTableDeleteCallback)) == -1) {
            DeletesnmpNotifyEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_createAndWait
    || data->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notReady) {
        /* is the entry ready? */
        if(snmpNotifyEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpNotifyRowStatus = D_snmpNotifyRowStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpNotifyRowStatus = D_snmpNotifyRowStatus_notReady;
        }
    }

    if (newrow->snmpNotifyStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->snmpNotifyStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpNotifyRowStatus, snmpNotifyEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpNotifyRowStatus == D_snmpNotifyRowStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notInService || newrow->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpNotifyEntry_UNDO
/* add #define SR_snmpNotifyEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpNotifyEntry family.
 */
int
snmpNotifyEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
    snmpNotifyEntry_t *data = (snmpNotifyEntry_t *) doCur->data;
    snmpNotifyEntry_t *undodata = (snmpNotifyEntry_t *) doCur->undodata;
    snmpNotifyEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpNotifyRowStatus = D_snmpNotifyRowStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notReady
        || undodata->snmpNotifyRowStatus == D_snmpNotifyRowStatus_notInService) {
            undodata->snmpNotifyRowStatus = D_snmpNotifyRowStatus_createAndWait;
        } else 
        if(undodata->snmpNotifyRowStatus == D_snmpNotifyRowStatus_active) {
            undodata->snmpNotifyRowStatus = D_snmpNotifyRowStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpNotifyEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpNotifyEntry_UNDO */

#endif /* SETS */

#if (defined(SR_NOTIFY_FULL_COMPLIANCE) || defined(SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE))
/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
snmpNotifyFilterProfileTableDeleteCallback (TimeOutDescriptor *tdp)
{
    snmpNotifyFilterProfileEntry_t *data;

    /* dummy up an entry to delete */
    data = (snmpNotifyFilterProfileEntry_t *) tdp->UserData2;
    data->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_snmpNotifyFilterProfileEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the snmpNotifyFilterProfileTable */
void
DeletesnmpNotifyFilterProfileEntry (int index)
{
    FreeEntries(snmpNotifyFilterProfileEntryTypeTable, snmpNotifyFilterProfileTable->tp[index]);
    free(snmpNotifyFilterProfileTable->tp[index]);
    RemoveTableEntry(snmpNotifyFilterProfileTable, index);
}

/* This prototype is required to please some compilers */
int snmpNotifyFilterProfileEntryReadyToActivate
    SR_PROTOTYPE((snmpNotifyFilterProfileEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpNotifyFilterProfileEntryReadyToActivate(snmpNotifyFilterProfileEntry_t *data)
{
    /*
     * check valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    if (!VALID(I_snmpNotifyFilterProfileName, data->valid)) {
        return(0);
    }
    if (!VALID(I_snmpNotifyFilterProfileRowStatus, data->valid)) {
        return(0);
    }
    return(1);
}

#ifdef U_snmpNotifyFilterProfileEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyFilterProfileEntryFreeUserpartData (snmpNotifyFilterProfileEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpNotifyFilterProfileEntry */

#ifdef U_snmpNotifyFilterProfileEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyFilterProfileEntryCloneUserpartData (snmpNotifyFilterProfileEntry_t *dst, snmpNotifyFilterProfileEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpNotifyFilterProfileEntry */

snmpNotifyFilterProfileEntry_t *
k_snmpNotifyFilterProfileEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   OctetString * snmpTargetParamsName)
{
    int index;
    snmpNotifyFilterProfileEntry_t *data;

    snmpNotifyFilterProfileTable->tip[0].value.octet_val = snmpTargetParamsName;
    while ((index = 
              SearchTable(snmpNotifyFilterProfileTable, searchType)) != -1) {
        data = (snmpNotifyFilterProfileEntry_t *) 
                         snmpNotifyFilterProfileTable->tp[index];
        if (nominator < 0 || VALID(nominator, data->valid)) {
            return data;
        } else {
            snmpNotifyFilterProfileTable->tip[0].value.octet_val = 
                                                    data->snmpTargetParamsName;
            searchType = NEXT_SKIP;
        }
    }
    return NULL;
}

#ifdef SETS
int
k_snmpNotifyFilterProfileEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_snmpNotifyFilterProfileEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{
    snmpNotifyFilterProfileEntry_t
        *proposed_new_state = (snmpNotifyFilterProfileEntry_t *) dp->data;

    if (VALID(I_snmpNotifyFilterProfileRowStatus, proposed_new_state->valid)) {
        switch(proposed_new_state->snmpNotifyFilterProfileRowStatus) {
            case D_snmpNotifyFilterProfileRowStatus_destroy:
                dp->state = SR_DELETE;
                break;
            case D_snmpNotifyFilterProfileRowStatus_createAndWait:
            case D_snmpNotifyFilterProfileRowStatus_notReady:
                dp->state = SR_ADD_MODIFY;
                break;
            case D_snmpNotifyFilterProfileRowStatus_createAndGo:
            case D_snmpNotifyFilterProfileRowStatus_active:
            case D_snmpNotifyFilterProfileRowStatus_notInService:
                if(snmpNotifyFilterProfileEntryReadyToActivate
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
k_snmpNotifyFilterProfileEntry_set_defaults(doList_t *dp)
{
    snmpNotifyFilterProfileEntry_t *data = (snmpNotifyFilterProfileEntry_t *) (dp->data);

    if ((data->snmpNotifyFilterProfileName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpNotifyFilterProfileStorType = 
        D_snmpNotifyFilterProfileStorType_nonVolatile;
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    /*
     * clear valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    CLR_VALID(I_snmpNotifyFilterProfileName, data->valid);
    CLR_VALID(I_snmpNotifyFilterProfileRowStatus, data->valid);
    return NO_ERROR;
}

int
k_snmpNotifyFilterProfileEntry_set(snmpNotifyFilterProfileEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{

    int index;
    snmpNotifyFilterProfileEntry_t *newrow = NULL;

    /* find this entry in the table */
    snmpNotifyFilterProfileTable->tip[0].value.octet_val = data->snmpTargetParamsName;
    if ((index = SearchTable(snmpNotifyFilterProfileTable, EXACT)) != -1) {
        newrow = (snmpNotifyFilterProfileEntry_t *) snmpNotifyFilterProfileTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->snmpNotifyFilterProfileStorType;
            DeletesnmpNotifyFilterProfileEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpNotifyFilterProfileTable->tip[0].value.octet_val = data->snmpTargetParamsName;
        if ((index = NewTableEntry(snmpNotifyFilterProfileTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpNotifyFilterProfileEntry_t *) snmpNotifyFilterProfileTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyFilterProfileTableDeleteCallback)) == -1) {
            DeletesnmpNotifyFilterProfileEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_createAndWait
    || data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_notReady) {
        /* is the entry ready? */
        if(snmpNotifyFilterProfileEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_notReady;
        }
    }

    /* if we are executing a createAndGo command, transition to active */
    if(data->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_createAndGo) {
         data->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_active;
    }

    if (newrow->snmpNotifyFilterProfileStorType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->snmpNotifyFilterProfileStorType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpNotifyFilterProfileRowStatus, snmpNotifyFilterProfileEntryTypeTable, (void *) newrow, (void *) data);
    memcpy((char *) newrow->valid, (char *) data->valid, sizeof(data->valid));

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_notInService || newrow->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyFilterProfileTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpNotifyFilterProfileEntry_UNDO
/* add #define SR_snmpNotifyFilterProfileEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpNotifyFilterProfileEntry family.
 */
int
snmpNotifyFilterProfileEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
    snmpNotifyFilterProfileEntry_t *data = (snmpNotifyFilterProfileEntry_t *) doCur->data;
    snmpNotifyFilterProfileEntry_t *undodata = (snmpNotifyFilterProfileEntry_t *) doCur->undodata;
    snmpNotifyFilterProfileEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_notReady
        || undodata->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_notInService) {
            undodata->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_createAndWait;
        } else 
        if(undodata->snmpNotifyFilterProfileRowStatus == D_snmpNotifyFilterProfileRowStatus_active) {
            undodata->snmpNotifyFilterProfileRowStatus = D_snmpNotifyFilterProfileRowStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpNotifyFilterProfileEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpNotifyFilterProfileEntry_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
snmpNotifyFilterTableDeleteCallback (TimeOutDescriptor *tdp)
{
    snmpNotifyFilterEntry_t *data;

    /* dummy up an entry to delete */
    data = (snmpNotifyFilterEntry_t *) tdp->UserData2;
    data->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_snmpNotifyFilterEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the snmpNotifyFilterTable */
void
DeletesnmpNotifyFilterEntry (int index)
{
    FreeEntries(snmpNotifyFilterEntryTypeTable, snmpNotifyFilterTable->tp[index]);
    free(snmpNotifyFilterTable->tp[index]);
    RemoveTableEntry(snmpNotifyFilterTable, index);
}

/* This prototype is required to please some compilers */
int snmpNotifyFilterEntryReadyToActivate
    SR_PROTOTYPE((snmpNotifyFilterEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpNotifyFilterEntryReadyToActivate(snmpNotifyFilterEntry_t *data)
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_snmpNotifyFilterEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyFilterEntryFreeUserpartData (snmpNotifyFilterEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpNotifyFilterEntry */

#ifdef U_snmpNotifyFilterEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpNotifyFilterEntryCloneUserpartData (snmpNotifyFilterEntry_t *dst, snmpNotifyFilterEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpNotifyFilterEntry */

snmpNotifyFilterEntry_t *
k_snmpNotifyFilterEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            OctetString * snmpNotifyFilterProfileName,
                            OID * snmpNotifyFilterSubtree)
{
    int index;

    snmpNotifyFilterTable->tip[0].value.octet_val = snmpNotifyFilterProfileName;
    snmpNotifyFilterTable->tip[1].value.oid_val = snmpNotifyFilterSubtree;
    if ((index = SearchTable(snmpNotifyFilterTable, searchType)) == -1) {
        return NULL;
    }

    return (snmpNotifyFilterEntry_t *) snmpNotifyFilterTable->tp[index];

}

#ifdef SETS
int
k_snmpNotifyFilterEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_snmpNotifyFilterEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    snmpNotifyFilterEntry_t *data = (snmpNotifyFilterEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->snmpNotifyFilterRowStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_createAndGo) {
            data->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_active
        || data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notInService) {
            if(snmpNotifyFilterEntryReadyToActivate(data) == 0) {
                /* inconsistent values, don't allow the set operation */
                doListReady = 0;
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
k_snmpNotifyFilterEntry_set_defaults(doList_t *dp)
{
    snmpNotifyFilterEntry_t *data = (snmpNotifyFilterEntry_t *) (dp->data);

    if ((data->snmpNotifyFilterMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->snmpNotifyFilterType = D_snmpNotifyFilterType_included;
    data->snmpNotifyFilterStorageType = 
        D_snmpNotifyFilterStorageType_nonVolatile;
    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snmpNotifyFilterEntry_set(snmpNotifyFilterEntry_t *data,
                            ContextInfo *contextInfo, int function)
{

    int index;
    snmpNotifyFilterEntry_t *newrow = NULL;

    /* find this entry in the table */
    snmpNotifyFilterTable->tip[0].value.octet_val = data->snmpNotifyFilterProfileName;
    snmpNotifyFilterTable->tip[1].value.oid_val = data->snmpNotifyFilterSubtree;
    if ((index = SearchTable(snmpNotifyFilterTable, EXACT)) != -1) {
        newrow = (snmpNotifyFilterEntry_t *) snmpNotifyFilterTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->snmpNotifyFilterStorageType;
            DeletesnmpNotifyFilterEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpNotifyFilterTable->tip[0].value.octet_val = data->snmpNotifyFilterProfileName;
        snmpNotifyFilterTable->tip[1].value.oid_val = data->snmpNotifyFilterSubtree;
        if ((index = NewTableEntry(snmpNotifyFilterTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpNotifyFilterEntry_t *) snmpNotifyFilterTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyFilterTableDeleteCallback)) == -1) {
            DeletesnmpNotifyFilterEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_createAndWait
    || data->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notReady) {
        /* is the entry ready? */
        if(snmpNotifyFilterEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_notReady;
        }
    }

    if (newrow->snmpNotifyFilterStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->snmpNotifyFilterStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpNotifyFilterRowStatus, snmpNotifyFilterEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notInService || newrow->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, snmpNotifyFilterTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpNotifyFilterEntry_UNDO
/* add #define SR_snmpNotifyFilterEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpNotifyFilterEntry family.
 */
int
snmpNotifyFilterEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
    snmpNotifyFilterEntry_t *data = (snmpNotifyFilterEntry_t *) doCur->data;
    snmpNotifyFilterEntry_t *undodata = (snmpNotifyFilterEntry_t *) doCur->undodata;
    snmpNotifyFilterEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notReady
        || undodata->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_notInService) {
            undodata->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_createAndWait;
        } else 
        if(undodata->snmpNotifyFilterRowStatus == D_snmpNotifyFilterRowStatus_active) {
            undodata->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpNotifyFilterEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpNotifyFilterEntry_UNDO */

#endif /* SETS */
#endif	/* (defined(SR_NOTIFY_FULL_COMPLIANCE) || defined(SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE)) */
