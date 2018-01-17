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

#include "mibtools.h"
#include "agent.h"

/*----------------------------------------------------------------
 * print the k_ set method for this family if it contains writable
 * variables
 *----------------------------------------------------------------*/
void
write_k_set_routine(fp, ptr, base)
    FILE           *fp;
    struct OID_INFO *ptr;
    char           *base;
{
    struct OID_INFO *temp_ptr;
    struct _index_array *index_array, *temp_index;
    int             count;
    char           *status_type = rowStatusString;
    char           *destroy_string = "destroy";
    char           *active_string = "active";
    int             idx;
    int i = 0;

    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }
    if (ansi) {
        fprintf(fp, "int\nk_%s_set(%s_t *data,\n", ptr->name, ptr->name);
        for (i = 0; i < (int) strlen(ptr->name); i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "       ContextInfo *contextInfo, int function)\n");
    } else {
        fprintf(fp, "int\nk_%s_set(data, contextInfo, function)\n", ptr->name);
        fprintf(fp, "   %s_t *data;\n   ContextInfo *contextInfo;\n   int function;\n", ptr->name);
    }
    fprintf(fp, "{\n\n");


    /* get indexing information */
    idx = get_index_array(ptr->name, &index_array);

    if (((temp_ptr = row_status_in_family(ptr)) == NULL) 
                 && ((!search_table && !parser) || idx == 0)) {
	if ((idx == 0) && (parser)) {
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
                if (strcmp(temp_ptr->oid_access, READ_WRITE_STR) != 0) {
                    continue;
                }
                fprintf(fp, "    if (VALID(I_%s, data->mod)) {\n", temp_ptr->name);
                if (test_and_incr &&
                    (strcmp(temp_ptr->oid_type, testAndIncrString) == 0)) {
		    fprintf(fp, "        if (data->%s != 2147483647) {\n", temp_ptr->name);
                    fprintf(fp, "            %sData.%s = \n", ptr->name, temp_ptr->name);
		    fprintf(fp, "                ++data->%s;\n", temp_ptr->name);
		    fprintf(fp, "        } else {\n");
                    fprintf(fp, "            %sData.%s = 0;\n", ptr->name, temp_ptr->name);
		    fprintf(fp, "        }\n");
                } else if ((strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Integer32") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Unsigned32") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "TimeTicks") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "IpAddress") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "UInteger32") == 0)) {
                    fprintf(fp, "        %sData.%s = \n", ptr->name, temp_ptr->name);
		    fprintf(fp, "            data->%s;\n", temp_ptr->name);
                } else if ((strcmp(temp_ptr->oid_prim_type, "Counter") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Counter32") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Gauge32") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Gauge") == 0)) {
		    /* Counter and Gauge types can not be set */
                } else if ((strcmp(temp_ptr->oid_type, "DisplayString") == 0) ||
                    (strcmp(temp_ptr->oid_type, "SnmpAdminString") == 0) ||
                    (strcmp(temp_ptr->oid_type, "SnmpTagValue") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "PhysAddress") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "OctetString") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "NsapAddress") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "NetworkAddress") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Opaque") == 0) ||
                    (strcmp(temp_ptr->oid_prim_type, "Bits") == 0)) {
                    fprintf(fp, "        if (%sData.%s != NULL) {\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            FreeOctetString(%sData.%s);\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "        }\n");
                    fprintf(fp, "        %sData.%s = \n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            CloneOctetString(data->%s);\n", temp_ptr->name);

                    fprintf(fp, "        if (%sData.%s != NULL) {\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            SET_VALID(I_%s, %sData.valid);\n", temp_ptr->name, ptr->name);
                    fprintf(fp, "        } else {\n");
                    fprintf(fp, "            CLR_VALID(I_%s, %sData.valid);\n", temp_ptr->name, ptr->name);
                    fprintf(fp, "        }\n");
                } else if (strcmp(temp_ptr->oid_prim_type, "ObjectID") == 0) {
                    fprintf(fp, "        if (%sData.%s != NULL) {\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            FreeOID(%sData.%s);\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "        }\n");
                    fprintf(fp, "        %sData.%s = \n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            CloneOID(data->%s);\n", temp_ptr->name);

                    fprintf(fp, "        if (%sData.%s != NULL) {\n", ptr->name, temp_ptr->name);
                    fprintf(fp, "            SET_VALID(I_%s, %sData.valid);\n", temp_ptr->name, ptr->name);
                    fprintf(fp, "        } else {\n");
                    fprintf(fp, "            CLR_VALID(I_%s, %sData.valid);\n", temp_ptr->name, ptr->name);
                    fprintf(fp, "        }\n");
                } else if (strcmp(temp_ptr->oid_prim_type, "Counter64") == 0) {
		    /* Counter64 types can not be set */
                } else {
                    /* unknown type */
                    fprintf(stderr, "%s: write_k_init_routines: "
                            "unknown type %s\n",
                            prog_name, temp_ptr->oid_prim_type);
                    exit(-1);
                }
                fprintf(fp, "    }\n");
            }
	    fprintf(fp, "\n");
	    fprintf(fp, "    %sWriteConfigFileFlag = 1;\n", base);
	    fprintf(fp, "    return NO_ERROR;\n");
	} else if ((idx == 0) && (search_table)) {
	    fprintf(fp, "   %s_free(%sData);\n", ptr->name, ptr->name);
	    fprintf(fp, "   %sData = Clone_%s(data);\n", ptr->name, ptr->name);
	    fprintf(fp, "   return NO_ERROR;\n");
	} else if (test_and_incr) {
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
                if (strcmp(temp_ptr->oid_access, READ_WRITE_STR) != 0) {
                    continue;
                }
                if (strcmp(temp_ptr->oid_type, testAndIncrString) == 0) {
                    fprintf(fp, "#ifdef NOT_YET\n");
                    fprintf(fp, "    if (VALID(I_%s, data->mod)) {\n", temp_ptr->name);
		    fprintf(fp, "        if (data->%s != 2147483647) {\n", temp_ptr->name);
		    fprintf(fp, "            /* store ++data->%s */\n", temp_ptr->name);
		    fprintf(fp, "        } else {\n");
		    fprintf(fp, "            /* store zero (value wrapped) */\n");
		    fprintf(fp, "        }\n");
                    fprintf(fp, "    }\n");
                    fprintf(fp, "#endif /* NOT_YET */\n");
                }
            }
	    fprintf(fp, "   return COMMIT_FAILED_ERROR;\n");
	} else {
	    /* no special support */
	    fprintf(fp, "   return COMMIT_FAILED_ERROR;\n");
	}
    }
    else {
	/* function prologue */
	fprintf(fp, "    int index;\n");
	fprintf(fp, "    %s_t *newrow = NULL;\n", ptr->name);
	fprintf(fp, "\n");

	/* search for an existing entry */
	fprintf(fp, "    /* find this entry in the table */\n");
	for (temp_index = index_array, count = 0;
	     temp_index && temp_index->name; temp_index++, count++) {
	    fprintf(fp, "    %s.tip[%d].value.", ptr->parent_ptr->name, count);
	    print_string(fp, temp_index, INDEX_CAT);
	    fprintf(fp, "_val = data->%s;\n", temp_index->name);
	}
	fprintf(fp, "    if ((index = SearchTable(&%s, EXACT)) != -1) {\n",
		ptr->parent_ptr->name);
	fprintf(fp, "        newrow = (%s_t *) %s.tp[index];\n", ptr->name,
		ptr->parent_ptr->name);
	fprintf(fp, "    }\n");

        if (temp_ptr) {
	    fprintf(fp, "\n");

	    /* decide whether to destroy, create, or modify the entry */
	    fprintf(fp, "    /* perform the table entry operation on it */\n");
	    fprintf(fp, "    if (data->%s == D_%s_%s) {\n", temp_ptr->name,
	    	temp_ptr->name, destroy_string);
	    fprintf(fp, "        if (data->%sTimerId != -1) {\n",
		status_type);
	    fprintf(fp, "            Cancel%sTimeout(data->%sTimerId);\n",
		status_type, status_type);
	    fprintf(fp, "        }\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "        if (index == -1) {\n");
	    fprintf(fp, "            return NO_ERROR;\n");
	    fprintf(fp, "        } else {\n");
	    fprintf(fp, "            /* delete the table entry */\n");
	    fprintf(fp, "            Delete%s(index);\n", ptr->name);
	    if (parser) {
		fprintf(fp, "            %sWriteConfigFileFlag = 1;\n", base);
	    }
	    fprintf(fp, "            return NO_ERROR;\n");
	    fprintf(fp, "        }\n");
	    fprintf(fp, "    } else if (index == -1) {\n");
    
	    /* create the entry */
	    fprintf(fp, "        /* add the entry */\n");
	    for (temp_index = index_array, count = 0; temp_index && temp_index->name; temp_index++, count++) {
	        fprintf(fp, "        %s.tip[%d].value.", ptr->parent_ptr->name, count);
	        print_string(fp, temp_index, INDEX_CAT);
	        fprintf(fp, "_val = data->%s;\n", temp_index->name);
	    }
	    fprintf(fp, "        if ((index = NewTableEntry(&%s)) == -1) {\n",
	    	ptr->parent_ptr->name);
	    fprintf(fp, "            return RESOURCE_UNAVAILABLE_ERROR;\n");
	    fprintf(fp, "        }\n");
	    fprintf(fp, "        newrow = (%s_t *) %s.tp[index];\n", ptr->name,
	    	ptr->parent_ptr->name);
	    fprintf(fp, "\n");
	    fprintf(fp, "        /* set a timeout */\n");
            if (old_row_timer == 1) {
	        fprintf(fp, "        if ((newrow->%sTimerId = Set%sTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, %sDeleteCallback)) == -1) {\n", status_type, status_type, ptr->parent_ptr->name);
            }
            else {
	        fprintf(fp, "        if ((newrow->%sTimerId = SrSet%sTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, %sDeleteCallback)) == -1) {\n", status_type, status_type, ptr->parent_ptr->name);
            }
	    fprintf(fp, "            Delete%s(index);\n", ptr->name);
	    fprintf(fp, "            return RESOURCE_UNAVAILABLE_ERROR;\n");
	    fprintf(fp, "        }\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");

	    /* modify the entry */
	    fprintf(fp, "    /* if we are executing a createAndWait command, determine whether\n");
	    fprintf(fp, "     * to transition to notReady or notInService.  If we are notReady,\n");
	    fprintf(fp, "     * transition to notInService whenever enough information becomes\n");
	    fprintf(fp, "     * available to the agent. */\n");
	    fprintf(fp, "    if(data->%s == D_%s_createAndWait\n",
	    	temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "    || data->%s == D_%s_notReady) {\n",
	    	temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        /* is the entry ready? */\n");
	    fprintf(fp, "        if(%sReadyToActivate(data) != 0) {\n", ptr->name);
	    fprintf(fp, "            /* yes, it can go to notInService */\n");
	    fprintf(fp, "            data->%s = D_%s_notInService;\n", temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        } else {\n");
	    fprintf(fp, "            /* no, it isn't ready yet */\n");
	    fprintf(fp, "            data->%s = D_%s_notReady;\n", temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        }\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
    
	    /* update values */
	    fprintf(fp, "    /* copy values from the scratch copy to live data */\n");
	    fprintf(fp, "    TransferEntries(I_%s, %sTypeTable, (void *) newrow, (void *) data);\n", last_object_in_family(ptr), ptr->name);
	    fprintf(fp, "    SET_ALL_VALID(newrow->valid);\n");
	    fprintf(fp, "\n");
	} else {
	    fprintf(fp, "    else {\n");
	    if (ptr->group_read_create || xxx_read_create) {
	        fprintf(fp, "        /* create new row */\n");
                fprintf(fp, "        if ((index = NewTableEntry(&%s)) == -1) {\n", ptr->parent_ptr->name);
	        fprintf(fp, "            return COMMIT_FAILED_ERROR;\n");
                fprintf(fp, "        }\n");
                fprintf(fp, "        newrow = (%s_t *) %s.tp[index];\n", ptr->name, ptr->parent_ptr->name);
            } else {
	        fprintf(fp, "        return COMMIT_FAILED_ERROR;\n");
            }
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");

	    /* update values */
	    fprintf(fp, "    /* copy values from the scratch copy to live data */\n");
	    fprintf(fp, "    TransferEntries(I_%s, %sTypeTable, (void *) newrow, (void *) data);\n", last_object_in_family(ptr), ptr->name);
	    fprintf(fp, "    SET_ALL_VALID(newrow->valid);\n");
	    fprintf(fp, "\n");
	}

	if (temp_ptr) {
	    /* handle timeouts */
       	    fprintf(fp, "    /* if the new row entry is now valid, cancel the creation timeout */\n");
	    fprintf(fp, "    if (newrow->%s == D_%s_%s && newrow->%sTimerId != -1) {\n",
		temp_ptr->name, temp_ptr->name, active_string, status_type);
	    fprintf(fp, "        Cancel%sTimeout(newrow->%sTimerId);\n", status_type,
		status_type);
	    fprintf(fp, "        newrow->%sTimerId = -1;\n", status_type);
	    fprintf(fp, "    } else\n");
	    fprintf(fp, "    if ((newrow->%s == D_%s_notInService || newrow->%s == D_%s_notReady)\n",
		temp_ptr->name, temp_ptr->name, temp_ptr->name,
		temp_ptr->name);
	    fprintf(fp, "    && newrow->%sTimerId == -1) {\n",
		status_type);
	    fprintf(fp, "        /* we are transitioning to an inactive state.  Set a timer to delete\n");
	    fprintf(fp, "         * the row after an appropriate interval */\n");
            if (old_row_timer == 1) {
	        fprintf(fp, "        newrow->%sTimerId = Set%sTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, %sDeleteCallback);\n",
	    	                status_type, status_type,
	    	                               ptr->parent_ptr->name);
	        fprintf(fp, "    }\n");
            }
            else {
	        fprintf(fp, "        newrow->%sTimerId = SrSet%sTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, %sDeleteCallback);\n",
	    	                status_type, status_type,
	    	                               ptr->parent_ptr->name);
	        fprintf(fp, "    }\n");
            }
	}

	/* close the function */
	fprintf(fp, "\n");
	if (parser) {
	    fprintf(fp, "    %sWriteConfigFileFlag = 1;\n", base);
	}
	fprintf(fp, "    return NO_ERROR;\n");
	fprintf(fp, "\n");
    }
    fprintf(fp, "}\n");
}

/*----------------------------------------------------------------
 * Now print the set routine
 *---------------------------------------------------------------*/
void
write_v_set_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    fprintf(fp, "/*%s\n", dashes);
    fprintf(fp, " * Perform the kernel-specific set function for this group of\n");
    fprintf(fp, " * related objects.\n");
    fprintf(fp, " *%s*/\n", dashes);

    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }
    fprintf(fp, "int \n");
    fprintf(fp, "%s_set", ptr->name);
    if (ansi) {
        fprintf(fp, "(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)\n");
    } else {
        fprintf(fp, "(doHead, doCur, contextInfo)\n");
        fprintf(fp, "    doList_t       *doHead;\n");
        fprintf(fp, "    doList_t       *doCur;\n");
        fprintf(fp, "    ContextInfo    *contextInfo;\n");
    }
    fprintf(fp, "{\n");
    fprintf(fp, "  return (k_%s_set((%s_t *) ", ptr->name, ptr->name);
    fprintf(fp, "(doCur->data),\n");
        fprintf(fp, "            contextInfo, doCur->state));");
    fprintf(fp, "\n}\n\n");
}
