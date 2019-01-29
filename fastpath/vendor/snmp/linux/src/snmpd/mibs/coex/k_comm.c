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
 * -f mib.cnf -o coex -per_file_init -stubs_only -search_table (implies -row_status) 
 * -test_and_incr -parser -row_status -userpart -storage_type 
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "mibout.h"
#include "lookup.h"
#include "v2table.h"
#include "tmq.h"
#include "rowstatf.h"
#include "sr_cfg.h"
#include "scan.h"
#include "mapctx.h"

#include "i_comm.h"

contextMappingList_t contextMappingList_snmpCommunityTable = { NULL, 0, NULL };

static SnmpV2Table *snmpCommunityTable;

void DeletesnmpCommunityEntry
    SR_PROTOTYPE((int index));
extern void snmpCommunityEntry_free
    SR_PROTOTYPE((snmpCommunityEntry_t *data));

/* initialize support for snmpCommunityEntry objects */
int
k_snmpCommunityEntry_initialize(
    char *contextName_text,
    SnmpV2Table *i_snmpCommunityTable)
{
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_snmpCommunityTable,
                              NULL,
                              i_snmpCommunityTable);
        AddContextMappingText(&contextMappingList_snmpCommunityTable,
                              "default",
                              i_snmpCommunityTable);
    } else {
        AddContextMappingText(&contextMappingList_snmpCommunityTable,
                              contextName_text,
                              i_snmpCommunityTable);
    }

    return 1;
}

/* terminate support for snmpCommunityEntry objects */
int
k_snmpCommunityEntry_terminate(void)
{
    /* free allocated memory */
    while(snmpCommunityTable->nitems > 0) {
        DeletesnmpCommunityEntry(0);
    }
    
    if(snmpCommunityTable->tp != NULL){
       free(snmpCommunityTable->tp);
       snmpCommunityTable->tp = NULL;
    }
    return 1;
}

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
snmpCommunityTableDeleteCallback (TimeOutDescriptor *tdp)
{
    cidata_t *ud2 = (cidata_t *)tdp->UserData2;
    snmpCommunityEntry_t *data;

    /* dummy up an entry to delete */
    data = (snmpCommunityEntry_t *) ud2->data;
    data->snmpCommunityStatus = D_snmpCommunityStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_snmpCommunityEntry_set(data, ud2->cip, 0);
    FreeContextInfo(ud2->cip);
    free(ud2); 
}

/* This routine deletes an entry from the snmpCommunityTable */
void
DeletesnmpCommunityEntry (int index)
{
    snmpCommunityEntry_t *data;

    /* get a pointer to the old entry */
    data = (snmpCommunityEntry_t *) snmpCommunityTable->tp[index];

    /* free the old entry and remove it from the table */
    snmpCommunityEntry_free(data);
    RemoveTableEntry(snmpCommunityTable, index);
}

/* This prototype is required to please some compilers */
int snmpCommunityEntryReadyToActivate
    SR_PROTOTYPE((snmpCommunityEntry_t *data));
/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
snmpCommunityEntryReadyToActivate(snmpCommunityEntry_t *data)
{
    /* The snmpCommunityName and snmpCommunitySecurityName must be
     * explicitly set before a row is qualified to become active */
    if ((data->snmpCommunityName != NULL) && 
        (data->snmpCommunityName->length > 0) &&
        (data->snmpCommunitySecurityName != NULL) && 
        (data->snmpCommunitySecurityName->length > 0)) {
        return(1);
    }
    else {
       return(0);
    }
}

#ifdef U_snmpCommunityEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_snmpCommunityEntryFreeUserpartData (snmpCommunityEntry_t *data)
{
    /* nothing to free by default */
}
#endif /* U_snmpCommunityEntry */

#ifdef U_snmpCommunityEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_snmpCommunityEntryCloneUserpartData (snmpCommunityEntry_t *dst, snmpCommunityEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_snmpCommunityEntry */

snmpCommunityEntry_t *
k_snmpCommunityEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         OctetString * snmpCommunityIndex)
{
    int index;
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;
   
    if (!prev_serialNum_initialized) {
        if ((serialNum != prev_serialNum) || (serialNum == -1)) {
            MapContext(&contextMappingList_snmpCommunityTable,
                       contextInfo,
                       (void *)&snmpCommunityTable);
        }
    } else {
        MapContext(&contextMappingList_snmpCommunityTable,
                   contextInfo,
                   (void *)&snmpCommunityTable);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    if (snmpCommunityTable == NULL) {
        return NULL;
    }

    snmpCommunityTable->tip[0].value.octet_val = snmpCommunityIndex;
    if ((index = SearchTable(snmpCommunityTable, searchType)) == -1) {
        return NULL;
    }

    return (snmpCommunityEntry_t *) snmpCommunityTable->tp[index];

}

#ifdef SETS
int
k_snmpCommunityEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{
    MapContext(&contextMappingList_snmpCommunityTable,
               contextInfo,
               (void *)&snmpCommunityTable);
    if (snmpCommunityTable == NULL) {
        return NO_CREATION_ERROR;
    }

    if (object->nominator == I_snmpCommunityContextEngineID) {
        if (value->os_value->length < 5) {
            return WRONG_VALUE_ERROR;
        }
        if (value->os_value->length > 32) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_snmpCommunityEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    int doListReady = 0;
    snmpCommunityEntry_t *data = (snmpCommunityEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->snmpCommunityStatus == D_snmpCommunityStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->snmpCommunityStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->snmpCommunityStatus == D_snmpCommunityStatus_createAndGo) {
            data->snmpCommunityStatus = D_snmpCommunityStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->snmpCommunityStatus == D_snmpCommunityStatus_active
        || data->snmpCommunityStatus == D_snmpCommunityStatus_notInService) {
            if(snmpCommunityEntryReadyToActivate(data) == 0) {
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

extern OctetString *agt_local_snmpID; 

int
k_snmpCommunityEntry_set_defaults(doList_t *dp)
{
    snmpCommunityEntry_t *data = (snmpCommunityEntry_t *) (dp->data);

    if ((data->snmpCommunityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunitySecurityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityContextEngineID = CloneOctetString(agt_local_snmpID)) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityContextName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snmpCommunityTransportTag = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->RowStatusTimerId = -1;


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snmpCommunityEntry_set(snmpCommunityEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

    int index;
    snmpCommunityEntry_t *newrow = NULL;

    MapContext(&contextMappingList_snmpCommunityTable,
               contextInfo,
               (void *)&snmpCommunityTable);
    if (snmpCommunityTable == NULL) {
        return NO_CREATION_ERROR;
    }

    writeConfigFileFlag = TRUE;

    /* find this entry in the table */
    snmpCommunityTable->tip[0].value.octet_val = data->snmpCommunityIndex;
    if ((index = SearchTable(snmpCommunityTable, EXACT)) != -1) {
        newrow = (snmpCommunityEntry_t *) snmpCommunityTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->snmpCommunityStatus == D_snmpCommunityStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            DeletesnmpCommunityEntry(index);
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        snmpCommunityTable->tip[0].value.octet_val = data->snmpCommunityIndex;
        if ((index = NewTableEntry(snmpCommunityTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (snmpCommunityEntry_t *) snmpCommunityTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, snmpCommunityTableDeleteCallback)) == -1) {
            DeletesnmpCommunityEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->snmpCommunityStatus == D_snmpCommunityStatus_createAndWait
    || data->snmpCommunityStatus == D_snmpCommunityStatus_notReady) {
        /* is the entry ready? */
        if(snmpCommunityEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->snmpCommunityStatus = D_snmpCommunityStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->snmpCommunityStatus = D_snmpCommunityStatus_notReady;
        }
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_snmpCommunityStatus, snmpCommunityEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->snmpCommunityStatus == D_snmpCommunityStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->snmpCommunityStatus == D_snmpCommunityStatus_notInService || newrow->snmpCommunityStatus == D_snmpCommunityStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, snmpCommunityTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_snmpCommunityEntry_UNDO
/* add #define SR_snmpCommunityEntry_UNDO in sitedefs.h to
 * include the undo routine for the snmpCommunityEntry family.
 */
int
snmpCommunityEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
    snmpCommunityEntry_t *data = (snmpCommunityEntry_t *) doCur->data;
    snmpCommunityEntry_t *undodata = (snmpCommunityEntry_t *) doCur->undodata;
    snmpCommunityEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->snmpCommunityStatus = D_snmpCommunityStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->snmpCommunityStatus == D_snmpCommunityStatus_notReady
        || undodata->snmpCommunityStatus == D_snmpCommunityStatus_notInService) {
            undodata->snmpCommunityStatus = D_snmpCommunityStatus_createAndWait;
        } else 
        if(undodata->snmpCommunityStatus == D_snmpCommunityStatus_active) {
            undodata->snmpCommunityStatus = D_snmpCommunityStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_snmpCommunityEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_snmpCommunityEntry_UNDO */

#endif /* SETS */

