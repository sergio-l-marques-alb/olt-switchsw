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
#include "i_view.h"


/* global data describing the vacmMIBViews family */
static vacmMIBViews_t *vacmMIBViewsData;

static SnmpV2Table *vacmViewTreeFamilyTable;

void DeletevacmViewTreeFamilyEntry
    SR_PROTOTYPE((int index));

/* initialize support for vacmMIBViews objects */
int
k_vacmMIBViews_initialize(
    vacmMIBViews_t *vmv)
{
    vacmMIBViewsData = vmv;
    return 1;
}

/* terminate support for vacmMIBViews objects */
int
k_vacmMIBViews_terminate(void)
{
    vacmMIBViewsData = NULL;
    return 1;
}

/* initialize support for vacmViewTreeFamilyEntry objects */
int
k_vacmViewTreeFamilyEntry_initialize(
    SnmpV2Table *vvtft)
{
    vacmViewTreeFamilyTable = vvtft;
    return 1;
}

/* terminate support for vacmViewTreeFamilyEntry objects */
int
k_vacmViewTreeFamilyEntry_terminate(void)
{
    vacmViewTreeFamilyTable = NULL;
    return 1;
}

int
k_view_initialize(
    vacmMIBViews_t *vmv,
    SnmpV2Table *vvtft)
{
    int status = 1;

    if (k_vacmMIBViews_initialize(vmv) != 1) {
        status = -1;
    }
    if (k_vacmViewTreeFamilyEntry_initialize(vvtft) != 1) {
        status = -1;
    }

    return status;
}

int
k_view_terminate(void)
{
    int status = 1;

    if (k_vacmMIBViews_terminate() != 1) {
        status = -1;
    }
    if (k_vacmViewTreeFamilyEntry_terminate() != 1) {
        status = -1;
    }

    return status;
}

#ifdef U_vacmMIBViews
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_vacmMIBViewsFreeUserpartData (vacmMIBViews_t *data)
{
    /* nothing to free by default */
}
#endif /* U_vacmMIBViews */

#ifdef U_vacmMIBViews
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_vacmMIBViewsCloneUserpartData (vacmMIBViews_t *dst, vacmMIBViews_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_vacmMIBViews */

vacmMIBViews_t *
k_vacmMIBViews_get(int serialNum, ContextInfo *contextInfo,
                   int nominator)
{
   return vacmMIBViewsData;
}

#ifdef SETS
int
k_vacmMIBViews_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{
    if ( object->nominator == I_vacmViewSpinLock ) {
       if ( value->sl_value != vacmMIBViewsData->vacmViewSpinLock ) {
            return INCONSISTENT_VALUE_ERROR;
       }
    }
    return NO_ERROR;
}

int
k_vacmMIBViews_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_vacmMIBViews_set(vacmMIBViews_t *data,
                   ContextInfo *contextInfo, int function)
{

   if (data->vacmViewSpinLock != 2147483647) {
       vacmMIBViewsData->vacmViewSpinLock = ++data->vacmViewSpinLock;
   }
   else {
       vacmMIBViewsData->vacmViewSpinLock = 0;
   }
   return NO_ERROR;
}

#ifdef SR_vacmMIBViews_UNDO
/* add #define SR_vacmMIBViews_UNDO in sitedefs.h to
 * include the undo routine for the vacmMIBViews family.
 */
int
vacmMIBViews_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_vacmMIBViews_UNDO */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
vacmViewTreeFamilyTableDeleteCallback (TimeOutDescriptor *tdp)
{
    vacmViewTreeFamilyEntry_t *data;

    /* dummy up an entry to delete */
    data = (vacmViewTreeFamilyEntry_t *) tdp->UserData2;
    data->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_vacmViewTreeFamilyEntry_set(data, (ContextInfo *) NULL, 0);
}

#endif /* SETS */

/* This routine deletes an entry from the vacmViewTreeFamilyTable */
void
DeletevacmViewTreeFamilyEntry (int index)
{
    FreeEntries(vacmViewTreeFamilyEntryTypeTable, vacmViewTreeFamilyTable->tp[index]);
    free(vacmViewTreeFamilyTable->tp[index]);
    RemoveTableEntry(vacmViewTreeFamilyTable, index);
}

/* This prototype is required to please some compilers */
int vacmViewTreeFamilyEntryReadyToActivate
    SR_PROTOTYPE((vacmViewTreeFamilyEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
vacmViewTreeFamilyEntryReadyToActivate(vacmViewTreeFamilyEntry_t *data)
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_vacmViewTreeFamilyEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_vacmViewTreeFamilyEntryFreeUserpartData (vacmViewTreeFamilyEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_vacmViewTreeFamilyEntry */

#ifdef U_vacmViewTreeFamilyEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_vacmViewTreeFamilyEntryCloneUserpartData (vacmViewTreeFamilyEntry_t *dst, vacmViewTreeFamilyEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_vacmViewTreeFamilyEntry */

vacmViewTreeFamilyEntry_t *
k_vacmViewTreeFamilyEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              OctetString * vacmViewTreeFamilyViewName,
                              OID * vacmViewTreeFamilySubtree)
{
    int index;

    vacmViewTreeFamilyTable->tip[0].value.octet_val = vacmViewTreeFamilyViewName;
    vacmViewTreeFamilyTable->tip[1].value.oid_val = vacmViewTreeFamilySubtree;
    if ((index = SearchTable(vacmViewTreeFamilyTable, searchType)) == -1) {
        return NULL;
    }

    return (vacmViewTreeFamilyEntry_t *) vacmViewTreeFamilyTable->tp[index];

}

#ifdef SETS
int
k_vacmViewTreeFamilyEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{
    vacmViewTreeFamilyEntry_t *data = NULL; 

    data = dp->data;

    if ( data->vacmViewTreeFamilyViewName->length < 1 ||
         data->vacmViewTreeFamilyViewName->length > 32 ) { 
        return NO_CREATION_ERROR;
    }

    return NO_ERROR;
}

int
k_vacmViewTreeFamilyEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    vacmViewTreeFamilyEntry_t *data = (vacmViewTreeFamilyEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->vacmViewTreeFamilyStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_createAndGo) {
            data->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_active
        || data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notInService) {
            if(vacmViewTreeFamilyEntryReadyToActivate(data) == 0) {
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
k_vacmViewTreeFamilyEntry_set_defaults(doList_t *dp)
{
    vacmViewTreeFamilyEntry_t *data = (vacmViewTreeFamilyEntry_t *) (dp->data);

    if ((data->vacmViewTreeFamilyMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->vacmViewTreeFamilyType = D_vacmViewTreeFamilyType_included;
    data->vacmViewTreeFamilyStorageType = D_vacmViewTreeFamilyStorageType_nonVolatile;
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_vacmViewTreeFamilyEntry_set(vacmViewTreeFamilyEntry_t *data,
                              ContextInfo *contextInfo, int function)
{

    int index;
    vacmViewTreeFamilyEntry_t *newrow = NULL;

    /* find this entry in the table */
    vacmViewTreeFamilyTable->tip[0].value.octet_val = data->vacmViewTreeFamilyViewName;
    vacmViewTreeFamilyTable->tip[1].value.oid_val = data->vacmViewTreeFamilySubtree;
    if ((index = SearchTable(vacmViewTreeFamilyTable, EXACT)) != -1) {
        newrow = (vacmViewTreeFamilyEntry_t *) vacmViewTreeFamilyTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->vacmViewTreeFamilyStorageType;
            DeletevacmViewTreeFamilyEntry(index);
            if (flag >= SR_NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        vacmViewTreeFamilyTable->tip[0].value.octet_val = data->vacmViewTreeFamilyViewName;
        vacmViewTreeFamilyTable->tip[1].value.oid_val = data->vacmViewTreeFamilySubtree;
        if ((index = NewTableEntry(vacmViewTreeFamilyTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (vacmViewTreeFamilyEntry_t *) vacmViewTreeFamilyTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmViewTreeFamilyTableDeleteCallback)) == -1) {
            DeletevacmViewTreeFamilyEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_createAndWait
    || data->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notReady) {
        /* is the entry ready? */
        if(vacmViewTreeFamilyEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_notReady;
        }
    }

    if (newrow->vacmViewTreeFamilyStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->vacmViewTreeFamilyStorageType >= SR_NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_vacmViewTreeFamilyStatus, vacmViewTreeFamilyEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notInService || newrow->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, vacmViewTreeFamilyTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_vacmViewTreeFamilyEntry_UNDO
/* add #define SR_vacmViewTreeFamilyEntry_UNDO in sitedefs.h to
 * include the undo routine for the vacmViewTreeFamilyEntry family.
 */
int
vacmViewTreeFamilyEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
    vacmViewTreeFamilyEntry_t *data = (vacmViewTreeFamilyEntry_t *) doCur->data;
    vacmViewTreeFamilyEntry_t *undodata = (vacmViewTreeFamilyEntry_t *) doCur->undodata;
    vacmViewTreeFamilyEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notReady
        || undodata->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_notInService) {
            undodata->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_createAndWait;
        } else 
        if(undodata->vacmViewTreeFamilyStatus == D_vacmViewTreeFamilyStatus_active) {
            undodata->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_vacmViewTreeFamilyEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_vacmViewTreeFamilyEntry_UNDO */

#endif /* SETS */
