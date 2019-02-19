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
#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
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
#include "i_ac.h"


static SnmpV2Table *vacmAccessTable;

void DeletevacmAccessEntry
    SR_PROTOTYPE((int index));

/* initialize support for vacmAccessEntry objects */
int
k_vacmAccessEntry_initialize(
    SnmpV2Table *vat)
{
    vacmAccessTable = vat;
    return 1;
}

/* terminate support for vacmAccessEntry objects */
int
k_vacmAccessEntry_terminate(void)
{
    vacmAccessTable = NULL;
    return 1;
}

int
k_ac_initialize(
    SnmpV2Table *vat)
{
    int status = 1;

    if (k_vacmAccessEntry_initialize(vat) != 1) {
        status = -1;
    }

    return status;
}

int
k_ac_terminate(void)
{
    int status = 1;

    if (k_vacmAccessEntry_terminate() != 1) {
        status = -1;
    }

    return status;
}

/* This routine deletes an entry from the vacmAccessTable */
void
DeletevacmAccessEntry (int index)
{
    FreeEntries(vacmAccessEntryTypeTable, vacmAccessTable->tp[index]);
    free(vacmAccessTable->tp[index]);
    RemoveTableEntry(vacmAccessTable, index);
}

/* This prototype is required to please some compilers */
int vacmAccessEntryReadyToActivate
    SR_PROTOTYPE((vacmAccessEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
vacmAccessEntryReadyToActivate(vacmAccessEntry_t *data)
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_vacmAccessEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_vacmAccessEntryFreeUserpartData (vacmAccessEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_vacmAccessEntry */

#ifdef U_vacmAccessEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_vacmAccessEntryCloneUserpartData (vacmAccessEntry_t *dst, vacmAccessEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_vacmAccessEntry */

vacmAccessEntry_t *
k_vacmAccessEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      OctetString * vacmGroupName,
                      OctetString * vacmAccessContextPrefix,
                      SR_INT32 vacmAccessSecurityModel,
                      SR_INT32 vacmAccessSecurityLevel)
{
    int index;

    vacmAccessTable->tip[0].value.octet_val = vacmGroupName;
    vacmAccessTable->tip[1].value.octet_val = vacmAccessContextPrefix;
    vacmAccessTable->tip[2].value.uint_val = vacmAccessSecurityModel;
    vacmAccessTable->tip[3].value.uint_val = vacmAccessSecurityLevel;
    if ((index = SearchTable(vacmAccessTable, searchType)) == -1) {
        return NULL;
    }

    return (vacmAccessEntry_t *) vacmAccessTable->tp[index];

}

#ifdef SETS

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void     
vacmAccessTableDeleteCallback (TimeOutDescriptor *tdp)
{
    vacmAccessEntry_t *data;
            
    /* dummy up an entry to delete */
    data = (vacmAccessEntry_t *) tdp->UserData2;
    data->vacmAccessStatus = D_vacmAccessStatus_destroy;
    data->RowStatusTimerId = -1;
 
    /* free the timeout descriptor */
    free(tdp); 
    
    /* call the set method */
    k_vacmAccessEntry_set(data, (ContextInfo *) NULL, 0);
}

int
k_vacmAccessEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{
    vacmAccessEntry_t *data = NULL;

    data = (vacmAccessEntry_t *)dp->data;

    if ( data->vacmAccessContextPrefix->length < 0 ||
         data->vacmAccessContextPrefix->length > 32 ) {
        return NO_CREATION_ERROR;
    }

    if ( data->vacmAccessSecurityModel == 0 ) {
        return NO_CREATION_ERROR;
    }

    if ( data->vacmAccessSecurityLevel < 1 || data->vacmAccessSecurityLevel > 3 ) {
        return NO_CREATION_ERROR;
    }

    if ( data->vacmAccessContextMatch < 1 || data->vacmAccessContextMatch > 2 ) {
       return NO_CREATION_ERROR;
    }

    return NO_ERROR;
}

int
k_vacmAccessEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    vacmAccessEntry_t *data = (vacmAccessEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->vacmAccessStatus == D_vacmAccessStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->vacmAccessStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->vacmAccessStatus == D_vacmAccessStatus_createAndGo) {
            data->vacmAccessStatus = D_vacmAccessStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->vacmAccessStatus == D_vacmAccessStatus_active
        || data->vacmAccessStatus == D_vacmAccessStatus_notInService) {
            if(vacmAccessEntryReadyToActivate(data) == 0) {
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
k_vacmAccessEntry_set_defaults(doList_t *dp)
{
    vacmAccessEntry_t *data = (vacmAccessEntry_t *) (dp->data);

    if ((data->vacmAccessReadViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->vacmAccessWriteViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->vacmAccessNotifyViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->vacmAccessContextMatch = D_vacmAccessContextMatch_exact;
    data->vacmAccessStorageType = D_vacmAccessStorageType_nonVolatile;
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_vacmAccessEntry_set(vacmAccessEntry_t *data,
                      ContextInfo *contextInfo, int function)
{

    int index;
    vacmAccessEntry_t *newrow = NULL;

    /* find this entry in the table */
    vacmAccessTable->tip[0].value.octet_val = data->vacmGroupName;
    vacmAccessTable->tip[1].value.octet_val = data->vacmAccessContextPrefix;
    vacmAccessTable->tip[2].value.uint_val = data->vacmAccessSecurityModel;
    vacmAccessTable->tip[3].value.uint_val = data->vacmAccessSecurityLevel;
    if ((index = SearchTable(vacmAccessTable, EXACT)) != -1) {
        newrow = (vacmAccessEntry_t *) vacmAccessTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->vacmAccessStatus == D_vacmAccessStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->vacmAccessStorageType;
            DeletevacmAccessEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        vacmAccessTable->tip[0].value.octet_val = data->vacmGroupName;
        vacmAccessTable->tip[1].value.octet_val = data->vacmAccessContextPrefix;
        vacmAccessTable->tip[2].value.uint_val = data->vacmAccessSecurityModel;
        vacmAccessTable->tip[3].value.uint_val = data->vacmAccessSecurityLevel;
        if ((index = NewTableEntry(vacmAccessTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (vacmAccessEntry_t *) vacmAccessTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmAccessTableDeleteCallback)) == -1) {
            DeletevacmAccessEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->vacmAccessStatus == D_vacmAccessStatus_createAndWait
    || data->vacmAccessStatus == D_vacmAccessStatus_notReady) {
        /* is the entry ready? */
        if(vacmAccessEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->vacmAccessStatus = D_vacmAccessStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->vacmAccessStatus = D_vacmAccessStatus_notReady;
        }
    }

    if (newrow->vacmAccessStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->vacmAccessStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_vacmAccessStatus, vacmAccessEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->vacmAccessStatus == D_vacmAccessStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->vacmAccessStatus == D_vacmAccessStatus_notInService || newrow->vacmAccessStatus == D_vacmAccessStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmAccessTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_vacmAccessEntry_UNDO
/* add #define SR_vacmAccessEntry_UNDO in sitedefs.h to
 * include the undo routine for the vacmAccessEntry family.
 */
int
vacmAccessEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
    vacmAccessEntry_t *data = (vacmAccessEntry_t *) doCur->data;
    vacmAccessEntry_t *undodata = (vacmAccessEntry_t *) doCur->undodata;
    vacmAccessEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->vacmAccessStatus = D_vacmAccessStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->vacmAccessStatus == D_vacmAccessStatus_notReady
        || undodata->vacmAccessStatus == D_vacmAccessStatus_notInService) {
            undodata->vacmAccessStatus = D_vacmAccessStatus_createAndWait;
        } else 
        if(undodata->vacmAccessStatus == D_vacmAccessStatus_active) {
            undodata->vacmAccessStatus = D_vacmAccessStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_vacmAccessEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_vacmAccessEntry_UNDO */

#endif /* SETS */
