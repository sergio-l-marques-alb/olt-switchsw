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
#include "tmq.h"
#include "rowstatf.h"
#include "i_group.h"


static SnmpV2Table *vacmSecurityToGroupTable;

void DeletevacmSecurityToGroupEntry
    SR_PROTOTYPE((int index));

/* initialize support for vacmSecurityToGroupEntry objects */
int
k_vacmSecurityToGroupEntry_initialize(
    SnmpV2Table *vstgt)
{
    vacmSecurityToGroupTable = vstgt;
    return 1;
}

/* terminate support for vacmSecurityToGroupEntry objects */
int
k_vacmSecurityToGroupEntry_terminate(void)
{
    vacmSecurityToGroupTable = NULL;
    return 1;
}

int
k_group_initialize(
    SnmpV2Table *vstgt)
{
    int status = 1;

    if (k_vacmSecurityToGroupEntry_initialize(vstgt) != 1) {
        status = -1;
    }

    return status;
}

int
k_group_terminate(void)
{
    int status = 1;

    if (k_vacmSecurityToGroupEntry_terminate() != 1) {
        status = -1;
    }

    return status;
}

/* This routine deletes an entry from the vacmSecurityToGroupTable */
void
DeletevacmSecurityToGroupEntry (int index)
{
    FreeEntries(vacmSecurityToGroupEntryTypeTable, vacmSecurityToGroupTable->tp[index]);
    free(vacmSecurityToGroupTable->tp[index]);
    RemoveTableEntry(vacmSecurityToGroupTable, index);
}

/* This prototype is required to please some compilers */
int vacmSecurityToGroupEntryReadyToActivate
    SR_PROTOTYPE((vacmSecurityToGroupEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
vacmSecurityToGroupEntryReadyToActivate(vacmSecurityToGroupEntry_t *data)
{
    /*
     * check valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    if (!VALID(I_vacmGroupName, data->valid)) {
        return(0);
    }
    if (!VALID(I_vacmSecurityToGroupStatus, data->valid)) {
        return(0);
    }
    return(1);
}

#ifdef U_vacmSecurityToGroupEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_vacmSecurityToGroupEntryFreeUserpartData (vacmSecurityToGroupEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_vacmSecurityToGroupEntry */

#ifdef U_vacmSecurityToGroupEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_vacmSecurityToGroupEntryCloneUserpartData (vacmSecurityToGroupEntry_t *dst, vacmSecurityToGroupEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_vacmSecurityToGroupEntry */

vacmSecurityToGroupEntry_t *
k_vacmSecurityToGroupEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 vacmSecurityModel,
                               OctetString * vacmSecurityName)
{
    int index;
    vacmSecurityToGroupEntry_t *data;

    vacmSecurityToGroupTable->tip[0].value.uint_val = vacmSecurityModel;
    vacmSecurityToGroupTable->tip[1].value.octet_val = vacmSecurityName;
    while ((index = SearchTable(vacmSecurityToGroupTable, searchType)) != -1) {
        data = (vacmSecurityToGroupEntry_t *) 
			vacmSecurityToGroupTable->tp[index];
        if (nominator < 0 || VALID(nominator, data->valid)) {
            return data;
        } else {
            vacmSecurityToGroupTable->tip[0].value.uint_val = 
						     data->vacmSecurityModel;
            vacmSecurityToGroupTable->tip[1].value.octet_val = 
						     data->vacmSecurityName;
	    searchType = NEXT_SKIP;
        }
    } 
    return NULL;
}

#ifdef SETS

/* This routine is called by the timeout code to
 * delete a pending creation of a Table entry */
void     
vacmSecurityToGroupTableDeleteCallback (TimeOutDescriptor *tdp)
{
    vacmSecurityToGroupEntry_t *data;
            
    /* dummy up an entry to delete */
    data = (vacmSecurityToGroupEntry_t *) tdp->UserData2;
    data->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_destroy;
    data->RowStatusTimerId = -1;
 
    /* free the timeout descriptor */
    free(tdp); 
    
    /* call the set method */
    k_vacmSecurityToGroupEntry_set(data, (ContextInfo *) NULL, 0);
}

int
k_vacmSecurityToGroupEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    vacmSecurityToGroupEntry_t *data = NULL;

    data = (vacmSecurityToGroupEntry_t *)dp->data;

    if ( data->vacmSecurityModel < 1 || data->vacmSecurityModel > 2147483647 ) {
       return NO_CREATION_ERROR;
    }

    if ( data->vacmSecurityName->length < 1 || data->vacmSecurityName->length > 32 ) {
       return NO_CREATION_ERROR;
    }

    return NO_ERROR;
}

int
k_vacmSecurityToGroupEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{
    vacmSecurityToGroupEntry_t
        *proposed_new_state = (vacmSecurityToGroupEntry_t *) dp->data;

    if (VALID(I_vacmSecurityToGroupStatus, proposed_new_state->valid)) {
        switch(proposed_new_state->vacmSecurityToGroupStatus) {
            case D_vacmSecurityToGroupStatus_destroy:
                dp->state = SR_DELETE;
                break;
            case D_vacmSecurityToGroupStatus_createAndWait:
            case D_vacmSecurityToGroupStatus_notReady:
                dp->state = SR_ADD_MODIFY;
                break;
            case D_vacmSecurityToGroupStatus_createAndGo:
            case D_vacmSecurityToGroupStatus_active:
            case D_vacmSecurityToGroupStatus_notInService:
                if(vacmSecurityToGroupEntryReadyToActivate
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
k_vacmSecurityToGroupEntry_set_defaults(doList_t *dp)
{
    vacmSecurityToGroupEntry_t *data = (vacmSecurityToGroupEntry_t *) (dp->data);

    if ((data->vacmGroupName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->vacmSecurityToGroupStorageType = D_vacmSecurityToGroupStorageType_nonVolatile;
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    /*
     * clear valid bits for non-index objects that do not have
     * a DEFVAL clause
     */
    CLR_VALID(I_vacmGroupName, data->valid);
    CLR_VALID(I_vacmSecurityToGroupStatus, data->valid);
    return NO_ERROR;
}

int
k_vacmSecurityToGroupEntry_set(vacmSecurityToGroupEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

    int index;
    vacmSecurityToGroupEntry_t *newrow = NULL;

    /* find this entry in the table */
    vacmSecurityToGroupTable->tip[0].value.uint_val = data->vacmSecurityModel;
    vacmSecurityToGroupTable->tip[1].value.octet_val = data->vacmSecurityName;
    if ((index = SearchTable(vacmSecurityToGroupTable, EXACT)) != -1) {
        newrow = (vacmSecurityToGroupEntry_t *) vacmSecurityToGroupTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->vacmSecurityToGroupStorageType;
            DeletevacmSecurityToGroupEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        vacmSecurityToGroupTable->tip[0].value.uint_val = data->vacmSecurityModel;
        vacmSecurityToGroupTable->tip[1].value.octet_val = data->vacmSecurityName;
        if ((index = NewTableEntry(vacmSecurityToGroupTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (vacmSecurityToGroupEntry_t *) vacmSecurityToGroupTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmSecurityToGroupTableDeleteCallback)) == -1) {
            DeletevacmSecurityToGroupEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_createAndWait
    || data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_notReady) {
        /* is the entry ready? */
        if(vacmSecurityToGroupEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_notReady;
        }
    }

    /* if we are executing a createAndGo command, transition to active */
    if(data->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_createAndGo) {
         data->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_active;
    }

    if (newrow->vacmSecurityToGroupStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->vacmSecurityToGroupStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_vacmSecurityToGroupStatus, vacmSecurityToGroupEntryTypeTable, (void *) newrow, (void *) data);
    memcpy((char *) newrow->valid, (char *) data->valid, sizeof(data->valid));

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_notInService || newrow->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmSecurityToGroupTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_vacmSecurityToGroupEntry_UNDO
/* add #define SR_vacmSecurityToGroupEntry_UNDO in sitedefs.h to
 * include the undo routine for the vacmSecurityToGroupEntry family.
 */
int
vacmSecurityToGroupEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
    vacmSecurityToGroupEntry_t *data = (vacmSecurityToGroupEntry_t *) doCur->data;
    vacmSecurityToGroupEntry_t *undodata = (vacmSecurityToGroupEntry_t *) doCur->undodata;
    vacmSecurityToGroupEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_notReady
        || undodata->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_notInService) {
            undodata->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_createAndWait;
        } else 
        if(undodata->vacmSecurityToGroupStatus == D_vacmSecurityToGroupStatus_active) {
            undodata->vacmSecurityToGroupStatus = D_vacmSecurityToGroupStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_vacmSecurityToGroupEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_vacmSecurityToGroupEntry_UNDO */

#endif /* SETS */
