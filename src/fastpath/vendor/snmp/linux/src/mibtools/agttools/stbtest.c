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
 * print the k_ test method for this family if it contains writable
 * variables
 *----------------------------------------------------------------*/
void
write_k_test_routine(fp, ptr)
FILE *fp;
struct OID_INFO *ptr;
{
    int i, total_vars=0, TestAndIncr_vars=0;
    struct OID_INFO *temp_ptr;
    char *type, *value;

    /* typically, this is a small function */
    if (argsused) {
        fprintf(fp, "/*ARGSUSED*/\n");
    }
    if (ansi) {
        fprintf(fp, "int\nk_%s_test(ObjectInfo *object, ObjectSyntax *value,\n", ptr->name);
        for (i = 0; i < (int) strlen(ptr->name); i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "        doList_t *dp, ContextInfo *contextInfo)\n");
    } else {
        fprintf(fp, "int\nk_%s_test(object, value, dp, contextInfo)\n", ptr->name);
        fprintf(fp, "    ObjectInfo     *object;\n");
        fprintf(fp, "    ObjectSyntax   *value;\n");
        fprintf(fp, "    doList_t       *dp;\n");
        fprintf(fp, "    ContextInfo    *contextInfo;\n");
    }
    fprintf(fp, "{\n");

    /* count the valid writable variables in this family */
    total_vars = 0;
    TestAndIncr_vars = 0;
    for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
        if (!temp_ptr->valid) continue;
        if ((strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0)) {
            total_vars++;
            if (strcmp(temp_ptr->oid_type, testAndIncrString) == 0) {
                TestAndIncr_vars++;
            }
        }
    }

    if (total_vars > 0) {
        if (!test_and_incr || TestAndIncr_vars == 0) {
            fprintf(fp, "#ifdef NOT_YET\n");
        }
        fprintf(fp, "    %s_t *data = (%s_t *) dp->data;\n", ptr->name, ptr->name);
        if (test_and_incr && TestAndIncr_vars > 0) {
            fprintf(fp, "    %s_t *undodata = (%s_t *) dp->undodata;\n", ptr->name, ptr->name);
        }
        fprintf(fp, "\n");
        fprintf(fp, "    /*\n");
        fprintf(fp, "     * Check the validity of MIB object values here.\n");
        fprintf(fp, "     * Perform checks on \"object values in isolation\".\n");
        fprintf(fp, "     */\n\n");

        i = 0;
        for (temp_ptr = ptr->next_family_entry; temp_ptr;
                                temp_ptr = temp_ptr->next_family_entry) {
            if (!temp_ptr->valid) continue;
            if ((strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) ||
                    (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) ||
                    (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0)) {
                i++;
                fprintf(fp, "    %sif (object->nominator == I_%s) {\n", i > 1 ? "else " : "", temp_ptr->name);
                if (test_and_incr &&
                        strcmp(temp_ptr->oid_type, testAndIncrString) == 0) {
                    fprintf(fp, "        if (undodata != NULL) {\n");
                    fprintf(fp, "            /* TestAndIncr variable...new value must equal old value */\n");
                    fprintf(fp, "            if (value->sl_value != undodata->%s) {\n", temp_ptr->name);
                    fprintf(fp, "                return INCONSISTENT_VALUE_ERROR;\n");
                    fprintf(fp, "            }\n");
                    fprintf(fp, "        }\n");
                    fprintf(fp, "        SET_VALID(I_%s, data->mod);\n", temp_ptr->name);
                } else {
                    type = temp_ptr->oid_prim_type;
                    if ((strcmp(type, "INTEGER") == 0) ||
                        (strcmp(type, "Integer32") == 0)) {
                        value = "value->sl_value";
                    }
                    else if ((strcmp(type, "TimeTicks") == 0) ||
                             (strcmp(type, "Gauge") == 0) ||
                             (strcmp(type, "Counter") == 0) ||
                             (strcmp(type, "Counter32") == 0) ||
                             (strcmp(type, "Gauge32") == 0) ||
                             (strcmp(type, "Unsigned32") == 0) ||
                             (strcmp(type, "UInteger32") == 0)) {
                        value = "value->ul_value";
                    }
                    else if ((strcmp(type, "ObjectID") == 0)) {
                        value = "value->oid_value";
                    }
                    else if ((strcmp(type, "OctetString") == 0) ||
                             (strcmp(type, "DisplayString") == 0) ||
                             (strcmp(type, "IpAddress") == 0) ||
                             (strcmp(type, "Bits") == 0) ||
                             (strcmp(type, "Opaque") == 0) ||
                             (strcmp(type, "NsapAddress") == 0)) {
                        value = "value->os_value";
                    } else {
                        value = "value";
                    }
                    fprintf(fp, "        /*\n");
                    fprintf(fp, "         * Examine %s.  In case of error,\n", value);
                    fprintf(fp, "         * return from the function here with an error code.\n");
                    fprintf(fp, "         */\n");
                    fprintf(fp, "        SET_VALID(I_%s, data->mod);\n", temp_ptr->name);
                }
                fprintf(fp, "    }\n");
            }
        }
	if (ptr->augmented) {
	    struct OID_INFO *g1, *augments_list;

	    for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
		if (g1->augments) {
		    if (strcmp(g1->augments, ptr->name) == 0) {
			augments_list = g1->next_family_entry;
			while (augments_list) {
			    if (augments_list->valid) {
                                if ((strcmp(augments_list->oid_access, READ_WRITE_STR) == 0) ||
                                        (strcmp(augments_list->oid_access, WRITE_ONLY_STR) == 0) ||
                                        (strcmp(augments_list->oid_access, READ_CREATE_STR) == 0)) {
                                    i++;
                                    fprintf(fp, "    %sif (object->nominator == I_%s) {\n", i > 1 ? "else " : "", augments_list->name);
                                    if (test_and_incr &&
                                            strcmp(augments_list->oid_type, testAndIncrString) == 0) {
                                        fprintf(fp, "        if (undodata != NULL) {\n");
                                        fprintf(fp, "            /* TestAndIncr variable...new value must equal old value */\n");
                                        fprintf(fp, "            if (value->sl_value != undodata->%s) {\n", augments_list->name);
                                        fprintf(fp, "                return INCONSISTENT_VALUE_ERROR;\n");
                                        fprintf(fp, "            }\n");
                                        fprintf(fp, "        }\n");
                                        fprintf(fp, "        SET_VALID(I_%s, data->mod);\n", augments_list->name);
                                    } else {
                                        type = augments_list->oid_prim_type;
                                        if ((strcmp(type, "INTEGER") == 0) ||
                                            (strcmp(type, "Integer32") == 0)) {
                                            value = "value->sl_value";
                                        }
                                        else if ((strcmp(type, "TimeTicks") == 0) ||
                                                 (strcmp(type, "Gauge") == 0) ||
                                                 (strcmp(type, "Counter") == 0) ||
                                                 (strcmp(type, "Counter32") == 0) ||
                                                 (strcmp(type, "Gauge32") == 0) ||
                                                 (strcmp(type, "Unsigned32") == 0) ||
                                                 (strcmp(type, "UInteger32") == 0)) {
                                            value = "value->ul_value";
                                        }
                                        else if ((strcmp(type, "ObjectID") == 0)) {
                                            value = "value->oid_value";
                                        }
                                        else if ((strcmp(type, "OctetString") == 0) ||
                                                 (strcmp(type, "DisplayString") == 0) ||
                                                 (strcmp(type, "IpAddress") == 0) ||
                                                 (strcmp(type, "Bits") == 0) ||
                                                 (strcmp(type, "Opaque") == 0) ||
                                                 (strcmp(type, "NsapAddress") == 0)) {
                                            value = "value->os_value";
                                        } else {
                                            value = "value";
                                        }
                                        fprintf(fp, "        /*\n");
                                        fprintf(fp, "         * Examine %s.  In case of error,\n", value);
                                        fprintf(fp, "         * return from the function here with an error code.\n");
                                        fprintf(fp, "         */\n");
                                        fprintf(fp, "        SET_VALID(I_%s, data->mod);\n", augments_list->name);
                                    }
                                    fprintf(fp, "    }\n");
                                }
			    }
			    augments_list =
				augments_list->next_family_entry;
			}
		    }
		}
	    }
	}
        if (!test_and_incr || TestAndIncr_vars == 0) {
            fprintf(fp, "#endif /* NOT_YET */\n");
        }
    }
    fprintf(fp, "    return NO_ERROR;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "\n");
}

/*----------------------------------------------------------------
 * Now print the clone routine
 *---------------------------------------------------------------*/
void
write_v_clone_routine(fp, ptr)
FILE *fp;
struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    struct OID_INFO *g1, *augments_list;
    int             idx;
    char           *temp;
    int i;
    int docheck=0;
    struct _index_array *index_array, *temp_index;

    idx = get_index_array(ptr->name, &index_array);

    if ((idx == 0) && !search_table) {
        /* We only need the clone routine for tables 
         * unless we are doing undos
         */
        fprintf(fp, "#ifdef SR_%s_UNDO\n", ptr->name);
    }

    fprintf(fp, "/*%s-\n", dashes);
    fprintf(fp, " * clone the %s family\n", ptr->name);
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "%s_t *\n", ptr->name);
    if (ansi) {
	fprintf(fp, "Clone_%s(%s_t *%s)\n", ptr->name, ptr->name, ptr->name);
    } else {
	fprintf(fp, "Clone_%s(%s)\n", ptr->name, ptr->name);
	fprintf(fp, "    %s_t *%s;\n", ptr->name, ptr->name);
    }
    fprintf(fp, "{\n");
 
    if (small_v_test) {
       fprintf(fp, "    /* Clone function is not used by auto-generated */\n");
       fprintf(fp, "    /* code, but may be used by user code */\n");
       fprintf(fp, "    return (%s_t *)SrCloneFamily(%s,\n", ptr->name, ptr->name);
       fprintf(fp, "                         %sGetInfo.family_size,\n", 
               ptr->name);
       fprintf(fp, "                         %sGetInfo.type_table,\n", 
               ptr->name);
       fprintf(fp, "                         %sGetInfo.highest_nominator,\n", 
               ptr->name);
       fprintf(fp, "                         %sGetInfo.valid_offset,\n", 
               ptr->name);
       fprintf(fp, "                         %sTestInfo.userpart_clone_func,\n",
               ptr->name);
       fprintf(fp, "                         %sGetInfo.userpart_free_func", 
               ptr->name);
       fprintf(fp, ");\n");
       fprintf(fp, "}\n");
       fprintf(fp, "\n");

       if ((idx == 0) & !search_table) {
           fprintf(fp, "#endif /* defined(SR_%s_UNDO) */\n", ptr->name);
       }
       return;
    }

    fprintf(fp, "    %s_t *data;\n", ptr->name);
    fprintf(fp, "\n");
    fprintf(fp, "    if (%s == NULL) {\n", ptr->name);
    fprintf(fp, "        return NULL;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "\n");
    fprintf(fp, "    if ((data = (%s_t *) malloc (sizeof(%s_t))) == NULL) {\n", ptr->name, ptr->name);
    fprintf(fp, "        return NULL;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    memcpy((char *) (data), (char *) (%s), sizeof(%s_t));\n", ptr->name, ptr->name);
    fprintf(fp, "\n");
    for (temp_ptr = ptr->next_family_entry; temp_ptr; temp_ptr = temp_ptr->next_family_entry) {
        if (temp_ptr->valid) {
	    temp = get_string(temp_ptr->oid_prim_type, temp_ptr->name, CLONE);
    	    if (temp[0]) {
	        docheck++;
	        fprintf(fp, "    data->%s = %s(%s->%s);\n", temp_ptr->name, temp, ptr->name, temp_ptr->name);
	    }
	}
    }


    /*
     * Clone fields where the object is defined in a different table
     * but is in this structure since it is part of the index for this
     * table.
     */

    /* get an array containing the indicies of this family */
    get_index_array(ptr->name, &index_array);

    /*
     * if this is not a table, then it has no INDEX clause, and 
     * index_array will be NULL.
     */
    if (index_array) {
        int found;

        /* for each index for this table */
	for (temp_index = index_array; temp_index->name; temp_index++) {

	    /*
	    * first, see if this entry is already in the
	    * structure (i.e. if is in this table).
	    */

            found = 0;
	    temp_ptr = ptr->next_family_entry;
	        while (temp_ptr) {
		    if (strcmp(temp_ptr->name, temp_index->name) == 0) {
			found = 1;
                        break;
		    }
	            temp_ptr = temp_ptr->next_family_entry;
            }

            /* if it has not yet been cloned, then do it now */
            if (!found) {
                for (temp_ptr = sorted_root; temp_ptr; temp_ptr=temp_ptr->next_sorted) {
                    if (strcmp(temp_index->name, temp_ptr->name) == 0) {
	                temp = get_string(temp_index->asn1_type, temp_index->name, CLONE);
	                if (temp[0]) {
                            fprintf(fp, "\n");
                            fprintf(fp, "    /* Clone index object which is defined in another table */\n");
	                    fprintf(fp, "    data->%s = %s(%s->%s);\n", temp_index->name, temp, ptr->name, temp_index->name);
	                }
                        break;
                    }
                }
            }
        }	/* for(temp_index... */
    }	/* if(index_array) */


    /*
     * if this group is augmented, print the items which
     * augment the group
     */
    if (ptr->augmented) {
        fprintf(fp, "\n");
        fprintf(fp, "    /* Clone objects for tables which AUGMENT this table */\n");
	for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
	    if (g1->augments) {
		if (strcmp(g1->augments, ptr->name) == 0) {
		    augments_list = g1->next_family_entry;
		    while (augments_list) {
		        if (augments_list->valid) {
	                    temp = get_string(augments_list->oid_prim_type, augments_list->name, CLONE);
	                    if (temp[0]) {
	                        fprintf(fp, "    data->%s = %s(%s->%s);\n", augments_list->name, temp, ptr->name, augments_list->name);
	                    }
		        }
                        augments_list = augments_list->next_family_entry;
		    }
	    	}
            }
	}
    }

    if (userpart) {
        fprintf(fp, "#ifdef U_%s\n", ptr->name);
        fprintf(fp, "    k_%sCloneUserpartData(data, %s);\n", ptr->name, ptr->name);
        fprintf(fp, "#endif /* U_%s */\n", ptr->name);
    }
    fprintf(fp, "\n");

    if (docheck) {
        i = 0;
        fprintf(fp, "    if (");
        for (temp_ptr = ptr->next_family_entry; temp_ptr; temp_ptr = temp_ptr->next_family_entry) {
            if (temp_ptr->valid) {
	        temp = get_string(temp_ptr->oid_prim_type, temp_ptr->name, CLONE);
	        if (temp[0]) {
                    if (i++ != 0) {
	                fprintf(fp, "            || ");
                    }
	            fprintf(fp, "(data->%s == NULL)\n", temp_ptr->name);
    	        }
    	    }
        }
        fprintf(fp, "        ) {\n");
        fprintf(fp, "        %s_free(data);\n", ptr->name);
        fprintf(fp, "        return NULL;\n");
        fprintf(fp, "    }\n");
    }

    fprintf(fp, "    return data;\n");
    fprintf(fp, "}\n");
    if ((idx == 0) & !search_table) {
        fprintf(fp, "#endif /* defined(SR_%s_UNDO) */\n", ptr->name);
    }
    fprintf(fp, "\n");
}

/* this routine writes a switch statement in the v_test() method routine */
int
print_test_switch(fp, ptr, temp_ptr, temp_index, index_array, storage_type_ptr)
    FILE *fp;
    struct OID_INFO *ptr, *temp_ptr, *storage_type_ptr;
    struct _index_array *index_array, *temp_index;
{
    if (temp_ptr->read_write) {
	fprintf(fp, "#ifdef I_%s\n", temp_ptr->name);
	fprintf(fp, "   case I_%s:\n\n", temp_ptr->name);
	if (!offset) {
	    if (strcmp(temp_ptr->oid_prim_type, "IpAddress") == 0) {
		fprintf(fp, "     if (value->os_value->length != 4) {\n");
		fprintf(fp, "         return WRONG_LENGTH_ERROR;\n");
		fprintf(fp, "     }\n");
		fprintf(fp, "\n");
	    }
	}

	/*
	 * In SNMPv2, the index should be not-accessible, but
	 * in SNMPv1, an index could be read-write. If the
	 * index is read-write, then we must make sure that
	 * the value matches the instance.
	 */
	/* first, see if this variable is an index */
	temp_index = index_array;
	while (temp_index && temp_index->name) {
	    if (strcmp(temp_ptr->name, temp_index->name) == 0) {
		/* if it is an index, do the check */
		WhosBuriedInGrantsTombCheck(fp, temp_ptr);
	    }
	    temp_index++;
	}

	/*
	 * If this is an enumerated integer, make sure the
	 * value is one of the enumerations. If it has a size
	 * range, make sure the value is within that range.
	 */
	CheckRanges(fp, temp_ptr);

        /* If this is a DisplayString, call IsDisplayString() */
        if (strcmp(temp_ptr->oid_type, "DisplayString") == 0) { 
	    fprintf(fp, "     if (!IsDisplayString(value->os_value)) {\n");
            fprintf(fp, "        return WRONG_VALUE_ERROR;\n");
            fprintf(fp, "     }\n");
            fprintf(fp, "\n");
        }

        if (storage_type && storage_type_ptr) {
	    if (strcmp(storage_type_ptr->name, temp_ptr->name) != 0) {
	        /* check for attempts to delete a "permanent" rowstatus
	         * entry */
	        if(strcmp(temp_ptr->oid_type, rowStatusString) == 0) {
    		    fprintf(fp, "     /* check for attempts to delete 'permanent' rows */\n");
		    fprintf(fp, "     if(%s != NULL\n", ptr->name);
		    fprintf(fp, "     && %s->%s == D_%s_permanent\n",
			    ptr->name, storage_type_ptr->name, 
			    storage_type_ptr->name);
		    fprintf(fp, "     && value->sl_value == D_%s_destroy) {\n",
		    	    temp_ptr->name);
		    fprintf(fp, "         /* permanent rows can be changed but not deleted */\n");
		    fprintf(fp, "         return INCONSISTENT_VALUE_ERROR;\n");
		    fprintf(fp, "     }\n");
		    fprintf(fp, "\n");
	        }

    	    } else {
		    fprintf(fp, "     if (%s != NULL) {\n", ptr->name);
    		    fprintf(fp, "         /* check for attempts to change 'permanent' to other value */\n");
		    fprintf(fp, "         if ((%s->%s == D_%s_permanent) &&\n",
			    ptr->name, storage_type_ptr->name, 
			    storage_type_ptr->name);
		    fprintf(fp, "             (value->sl_value != D_%s_permanent)) {\n",
			    storage_type_ptr->name);
		    fprintf(fp, "             /* permanent storageType cannot be changed */\n");
		    fprintf(fp, "             return WRONG_VALUE_ERROR;\n");
		    fprintf(fp, "         }\n");
    		    fprintf(fp, "         /* check for attempts to change 'permanent' to other value */\n");
		    fprintf(fp, "         if ((%s->%s < D_%s_permanent) &&\n",
			    ptr->name, storage_type_ptr->name, 
			    storage_type_ptr->name);
		    fprintf(fp, "             (value->sl_value >= D_%s_permanent)) {\n",
			    storage_type_ptr->name);
		    fprintf(fp, "             /* permanent storageType cannot be changed */\n");
		    fprintf(fp, "             return WRONG_VALUE_ERROR;\n");
		    fprintf(fp, "         }\n");
		    fprintf(fp, "     }\n");
    	    }
    	}

	if (!offset) {
	    if (is_oid(temp_ptr->oid_prim_type)) {
		fprintf(fp, "     if (((%s_t *) (dp->data))->%s != NULL) {\n", ptr->name, temp_ptr->name);
		fprintf(fp, "        FreeOID(((%s_t *) (dp->data))->%s);\n", ptr->name, temp_ptr->name);
		fprintf(fp, "     }\n\n");
	    }
	    else if (is_octetstring(temp_ptr->oid_prim_type)) {
		fprintf(fp, "     if (((%s_t *) (dp->data))->%s != NULL) {\n", ptr->name, temp_ptr->name);
		fprintf(fp, "        FreeOctetString(((%s_t *) (dp->data))->%s);\n", ptr->name, temp_ptr->name);
		fprintf(fp, "     }\n\n");
	    }
	}
	if (!offset) {
	    fprintf(fp, "     ((%s_t *) (dp->data))->%s = %s\n", ptr->name, temp_ptr->name, get_string(temp_ptr->oid_prim_type, temp_ptr->name, TEST));
	}
	fprintf(fp, "     break;\n");
	fprintf(fp, "#endif /* I_%s */\n\n", temp_ptr->name);
    }
    temp_ptr = temp_ptr->next_family_entry;
    return 1;
}

/*----------------------------------------------------------------
 * Now print the test routine
 *---------------------------------------------------------------*/
void
write_v_test_routine(fp, ptr)
FILE *fp;
struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr = NULL, *storage_type_ptr = NULL;
    int             idx, i;
    char           *temp;
    struct _index_array *index_array, *temp_index;
    int found;
    char *lateString;
    extern const char *k_prefix;

    /* get indexing information */
    idx = get_index_array(ptr->name, &index_array);

    if (storage_type) {
        /* is the storageType textual convention used in this family? */
        storage_type_ptr = storage_type_in_family(ptr);
        if(idx == 0 && storage_type_ptr != NULL) {
    	    fprintf(stderr, 
	        "Warning -- StorageType object (%s) in non-tabular family %s\n",
	        storage_type_ptr->name, ptr->name);
        }
    }

    /* emit the routine header */
    fprintf(fp, "/*%s\n", dashes);
    fprintf(fp, " * Determine if this SET request is valid. If so, add it to the do-list.\n");
    if (!v_elimination_data) {
    fprintf(fp, " * This operation is performed in 4 steps:\n");
    fprintf(fp, " *\n");
    fprintf(fp, " *   1) Validate the object instance.\n");
    fprintf(fp, " *   2) Locate a \"related\" do-list element, or create a new do-list\n");
    fprintf(fp, " *      element.\n");
    fprintf(fp, " *   3) Add the SET request to the selected do-list element.\n");
    fprintf(fp, " *   4) Finally, determine if the do-list element (all \"related\" set\n");
    fprintf(fp, " *      requests) is completely valid.\n");

    }
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "int \n");
    fprintf(fp, "%s_test", ptr->name);
    if (ansi) {
        fprintf(fp, "(OID *incoming, ObjectInfo *object, ObjectSyntax *value,\n");
        for (i = 0; i < (int) strlen(ptr->name); i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "      doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)\n");
    } else {
        fprintf(fp, "(incoming, object, value, doHead,");
        fprintf(fp, " doCur, contextInfo)\n");
        fprintf(fp, "    OID            *incoming;\n");
        fprintf(fp, "    ObjectInfo     *object;\n");
        fprintf(fp, "    ObjectSyntax   *value;\n");
        fprintf(fp, "    doList_t       *doHead;\n");
        fprintf(fp, "    doList_t       *doCur;\n");
        fprintf(fp, "    ContextInfo    *contextInfo;\n");
    }
    fprintf(fp, "{\n");

    if (small_v_test) {
	fprintf(fp, "    return v_test(incoming, object, value, doHead, doCur, contextInfo,\n");
	fprintf(fp, "                  &%sTestInfo);\n", ptr->name);
	fprintf(fp, "}\n\n");
        return;
    }

    if (medium_v) {
	fprintf(fp, "    doList_t *dp;\n");
	fprintf(fp, "    int error_status;\n");
	fprintf(fp, "    %s_t *cur_data;\n", ptr->name);
	fprintf(fp, "    %s_t *alloc_data;\n", ptr->name);
	fprintf(fp, "\n");
	fprintf(fp, "    if ((error_status = GetFamilyIndexValues(incoming, object,\n");
	fprintf(fp, "                            (void **) &alloc_data,\n");
	fprintf(fp, "                            &%sTestInfo)) != NO_ERROR) {\n", ptr->name);
	fprintf(fp, "        return error_status;\n");
	fprintf(fp, "    }\n");
	fprintf(fp, "\n");
	fprintf(fp, "    if ((dp = CheckDoList(doHead, (void *) alloc_data,\n");
	fprintf(fp, "                          &%sTestInfo)) == NULL) {\n", ptr->name);
	fprintf(fp, "        /* This set request is not in the do-list. Create a new DoList Entry */\n");
	fprintf(fp, "        cur_data = k_%s_get(-1, contextInfo, -1", ptr->name);
        if (idx != 0) {
	    fprintf(fp, ", NEXT");
	    for (temp_index = index_array; temp_index->name; temp_index++) {
	        fprintf(fp, ",\n");
	        fprintf(fp, "                                         ");
	        fprintf(fp, "alloc_data->%s", temp_index->name);
            }
        }
	fprintf(fp, ");\n");
	fprintf(fp, "\n");
	fprintf(fp, "        error_status = UpdateDoList(doCur, (void *) cur_data,\n");
	fprintf(fp, "                          (void *) alloc_data,\n");
	fprintf(fp, "                          &%sTestInfo);\n", ptr->name);
	fprintf(fp, "        dp = doCur;\n");
	fprintf(fp, "    }\n");
	fprintf(fp, "\n");
	fprintf(fp, "    return TestAndCleanup(error_status, (void *) alloc_data, object, value,\n");
	fprintf(fp, "                          doHead, contextInfo, dp,\n");
	fprintf(fp, "                          &%sTestInfo);\n", ptr->name);
	fprintf(fp, "}\n\n");
        return;
    }

    if (idx != -1) {
	fprintf(fp, "    int            instLength = incoming->length - object->oid.length;\n");
    }
    fprintf(fp, "    doList_t       *dp = NULL;\n");
    fprintf(fp, "    int            found;\n");
    fprintf(fp, "    int            error_status = NO_ERROR;\n");
    /* fprintf(fp, "    long           index;\n"); */
    /* fprintf(fp, "    int            searchType = EXACT;\n"); */
    if (idx != 0) {
	fprintf(fp, "    int            carry = 0;\n");
    }
    fprintf(fp, "    %s_t     *%s;\n", ptr->name, ptr->name);
    temp_index = index_array;
    while (temp_index && temp_index->name) {
	fprintf(fp, "    %-14s %s", temp_index->c_type, temp_index->name);
	if ((strncmp(temp_index->c_type, "OctetString", 11) == 0) ||
	    (strncmp(temp_index->c_type, "OID", 3) == 0)) {
	    fprintf(fp, " = NULL");
	}
	fprintf(fp, ";\n");
	if (idx < 0) {
	    fprintf(fp, "    %-14s %s_offset;\n", "int", temp_index->name);
	}
	temp_index++;
    }
    if (idx < 0) {
	fprintf(fp, "    %-14s final_index;\n", "int");
    }
    fprintf(fp, "\n");

    if (late || instance) {
        fprintf(fp, "    int regInstLen = 0;\n");
        fprintf(fp, "\n");
    }
    if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
        fprintf(fp, "    int which;\n");
        fprintf(fp, "    int count;\n");
        fprintf(fp, "    int arg = -1;\n");
        fprintf(fp, "\n");
    }

    if (new_k_stubs) {
        fprintf(fp, "    %s_t      %s_temp;\n", ptr->name, ptr->name);
        fprintf(fp, "\n");

/*
 * At this point, generation of variable declarations ends and
 * executable code begins.
 */

        fprintf(fp, "    memset((char *) &%s_temp, '\\0', sizeof(%s_t));\n",
                ptr->name, ptr->name);
        fprintf(fp, "    ZERO_VALID(%s_temp.valid);\n\n", ptr->name);
    }

    if (late) {
        fprintf(fp, "    if (object->oidname && (object->oidname[0] == 1)) {\n");
        fprintf(fp, "        regInstLen = (int) object->oidname[1];\n    ");
    }
    if (instance) {
        fprintf(fp, "    instLength = incoming->length - object->oid.length + regInstLen;\n");
    }
    if (late) {
        fprintf(fp, "    }\n");
    }
    if (late || instance) {
        fprintf(fp, "\n");
    }

    if (check_context) {
        fprintf(fp, "    /* check whether the agent supports this context */\n");
        fprintf(fp, "    if(k_%s_context_support(contextInfo) == 0) {\n", ptr->name);
        fprintf(fp, "        return(NO_CONTEXT_SUPPORT);\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "\n");
    }

    if (idx != 0) {
	fprintf(fp, "    /*\n");
	fprintf(fp, "     * Validate the object instance\n");
	fprintf(fp, "     *\n");
	fprintf(fp, "     */\n");

	if (idx < 0) {
	    temp_index = index_array;
	    if (temp_index && temp_index->name) {
		fprintf(fp, "    %s_offset = object->oid.length;\n", temp_index->name);
		temp_index++;
	    }
	    while (temp_index && temp_index->name) {
		fprintf(fp, "    %s_offset = %s_offset + ",
		      temp_index->name, (temp_index - 1)->name);
		if ((temp_index - 1)->length > 0) {
		    fprintf(fp, "%d;\n", (temp_index - 1)->length);
		}
		else {
		    print_string(fp, (temp_index - 1), GET_INDEX);
		}

		temp_index++;
	    }
	    fprintf(fp, "    final_index = %s_offset + ", (temp_index - 1)->name);
	    if ((temp_index - 1)->length > 0) {
		fprintf(fp, "%d;\n", (temp_index - 1)->length);
	    }
	    else {
		print_string(fp, (temp_index - 1), GET_INDEX);
	    }
	    fprintf(fp, "\n");
	    fprintf(fp, "    if (final_index != incoming->length) {\n");
	    fprintf(fp, "          return(NO_CREATION_ERROR);\n");
	    fprintf(fp, "    }\n");
	}
	else if (idx > 0) {
	    fprintf(fp, "    if (instLength != %d) {\n", idx);
	    fprintf(fp, "        return NO_CREATION_ERROR;\n");
	    /* fprintf(fp, "          arg = -1;\n"); */
	    fprintf(fp, "    }\n");
	}

	fprintf(fp, "\n");
        if (late || instance) {
            lateString = " - regInstLen";
        } else {
            lateString = "";
        }
	i = 0;
	for (temp_index = index_array; temp_index->name; temp_index++) {
	    fprintf(fp, "    if ( (");
	    fprintf(fp, "InstTo");
	    print_string(fp, temp_index, INST_TO_X);
	    if (idx < 0) {
		fprintf(fp, "(incoming, %s_offset%s, &%s, EXACT, &carry", temp_index->name, lateString, temp_index->name);
	    }
	    else {
		fprintf(fp, "(incoming, %d + object->oid.length%s, &%s, EXACT, &carry", i, lateString, temp_index->name);
		i += temp_index->length;

	    }
	    if (temp_index->IndexType > 0) {
                if (strcmp(temp_index->asn1_type, "NetworkAddress") != 0) {
                    fprintf(fp, ", %d", temp_index->IndexType);
                }
	    }
	    fprintf(fp, ")");

	    fprintf(fp, ") < 0 ) {\n");
	    fprintf(fp, "        error_status = NO_CREATION_ERROR;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}
	fprintf(fp, "    if (error_status != NO_ERROR) {\n");
	for (temp_index = index_array; 
		    temp_index && temp_index->name; 
		    temp_index++) {
	    print_string(fp, temp_index, FREE);
	}
	fprintf(fp, "        return error_status;\n");
	fprintf(fp, "    }\n");
    }
    else if (idx == 0) {
	fprintf(fp, "    /*\n");
	fprintf(fp, "     * Validate the object instance: 1) It must be of ");
	fprintf(fp, "length 1  2) and the\n");
	fprintf(fp, "     * instance must be 0.\n");
	fprintf(fp, "     */\n");
	fprintf(fp, "    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {\n");
	fprintf(fp, "        return (NO_CREATION_ERROR); \n");
	fprintf(fp, "    }\n");
    }
    fprintf(fp, "\n");

    fprintf(fp, "    /*\n");
    fprintf(fp, "     * Determine the element of the do-list which should contain this SET\n");
    fprintf(fp, "     * request. This is decided by looking for a previous do-list element for\n");
    if (idx != 0) {
	fprintf(fp, "     * the same row.\n");
    }
    else {
	fprintf(fp, "     * the same family.\n");
    }
    fprintf(fp, "     */\n");

    fprintf(fp, "    found = 0;\n\n");
    fprintf(fp, "    for (dp = doHead; dp != NULL; dp = dp->next) { \n");
    fprintf(fp, "         if ( (dp->setMethod == %s_set) &&\n", ptr->name);
    fprintf(fp, "            (((%s_t *) (dp->data)) != NULL) ", ptr->name);
    if (idx != 0) {
	for (temp_index = index_array; temp_index->name; temp_index++) {
	    if (is_oid(temp_index->asn1_type)) {
		fprintf(fp, "&&\n            (CmpOID(((%s_t *) (dp->data))->%s, %s) == 0) ", ptr->name, temp_index->name, temp_index->name);
	    } else if (is_octetstring(temp_index->asn1_type)) {
		fprintf(fp, "&&\n            (CmpOctetStrings(((%s_t *) (dp->data))->%s, %s) == 0) ", ptr->name, temp_index->name, temp_index->name);
	    } else {
		fprintf(fp, "&&\n            (((%s_t *) (dp->data))->%s == %s) ", ptr->name, temp_index->name, temp_index->name);
	    }
	}
    }
    fprintf(fp, ") {\n\n");
    fprintf(fp, "            found = 1;\n");
    fprintf(fp, "            break; \n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "\n");


    /*
     * If the family/instance is NOT found in the do-list,
     * call the k_<family>_get() method routine to build data
     * and undodata.
     * 
     * If the family/instance is found in the do-list, set
     * the temporary family pointer to undodata, which is a
     * snapshot of the current state--same as you would get
     * by calling k_<family>_get() again.
     */

    fprintf(fp, "    if (!found) {\n");

    if (idx != 0) {
        if (new_k_stubs) {
            fprintf(fp, "        /*\n");
            fprintf(fp, "         * For the new-style k_get interface, the index parameters are\n");
            fprintf(fp, "         * passed in as fields of a %s_t structure.\n", ptr->name);
            fprintf(fp, "         */\n");
            temp_index = index_array;
            while (temp_index && temp_index->name) {
                fprintf(fp, "        %s_temp.%s = %s;\n",
                        ptr->name, temp_index->name, temp_index->name);
                fprintf(fp, "        SET_VALID(I_%s, %s_temp.valid);\n",
                        temp_index->name, ptr->name);
                temp_index++;
            }
            fprintf(fp, "\n");
            fprintf(fp, "        %s = %sk_%s_get(-1, contextInfo, -1",
                    ptr->name, k_prefix, ptr->name);
        } else {
            fprintf(fp, "        %s = k_%s_get(-1, contextInfo, -1",
                    ptr->name, ptr->name);
        }
	fprintf(fp, ", EXACT");
        if (new_k_stubs) {
            fprintf(fp, ", &%s_temp", ptr->name);
        } else {
            temp_index = index_array;
            while (temp_index && temp_index->name) {
                fprintf(fp, ", %s", temp_index->name);
                temp_index++;
            }
        }
	fprintf(fp, ");\n");
        if (storage_type && storage_type_ptr) {
            fprintf(fp, "        if (%s != NULL) {\n", ptr->name);
            fprintf(fp, "            if (%s->%s == D_%s_readOnly) {\n",
                    ptr->name, storage_type_ptr->name, storage_type_ptr->name);
	    for (temp_index = index_array; 
		        temp_index && temp_index->name; 
		        temp_index++) {
	        print_string(fp, temp_index, FREE); }
            fprintf(fp, "                return NOT_WRITABLE_ERROR;\n");
            fprintf(fp, "            }\n");
            fprintf(fp, "        }\n\n");
        }
    } else if (idx == 0) {
        if (new_k_stubs) {
            fprintf(fp, "        %s = %sk_%s_get(-1, contextInfo, -1, EXACT, &%s_temp);\n",
                    ptr->name, k_prefix, ptr->name, ptr->name);
        } else {
            fprintf(fp, "        %s = k_%s_get(-1, contextInfo, -1);\n",
                    ptr->name, ptr->name);
        }
    }
    fprintf(fp, "        dp = doCur;\n");

    if (idx == 0) {
        if (!search_table) {
	    fprintf(fp, "        if ( (dp->data = (void *) malloc (sizeof(%s_t))) == NULL) { \n", ptr->name);
        } else {
	    fprintf(fp, "        if ((dp->data = (void *) Clone_%s(%s)) == NULL) {\n", ptr->name, ptr->name );
	}
	fprintf(fp, "            DPRINTF((APWARN, \"Cannot allocate memory\\n\"));\n");
	fprintf(fp, "            return RESOURCE_UNAVAILABLE_ERROR;\n");
	fprintf(fp, "        }\n");
	if (!search_table) {
	    fprintf(fp, "#ifdef SR_CLEAR_MALLOC\n");
	    fprintf(fp, "        memset(dp->data, 0, sizeof(%s_t));\n", ptr->name);
	    fprintf(fp, "#endif	/* SR_CLEAR_MALLOC */\n");
	}

        fprintf(fp, "#ifdef SR_%s_UNDO\n", ptr->name);
	fprintf(fp, "        if ((dp->undodata = (void *) Clone_%s(%s)) == NULL) {\n", ptr->name, ptr->name );
	fprintf(fp, "            DPRINTF((APWARN, \"Cannot allocate memory\\n\"));\n");
	fprintf(fp, "            error_status = RESOURCE_UNAVAILABLE_ERROR;\n");
	fprintf(fp, "        }\n");
        fprintf(fp, "#endif /* defined(SR_%s_UNDO) */\n", ptr->name);
    }

    fprintf(fp, "\n");
    fprintf(fp, "        dp->setMethod = %s_set;\n", ptr->name);
    fprintf(fp, "        dp->cleanupMethod = %s_cleanup;\n", ptr->name);
    if (undo) {
	fprintf(fp, "#ifdef SR_%s_UNDO\n", ptr->name);
	fprintf(fp, "        dp->undoMethod = %s_undo;\n", ptr->name);
	fprintf(fp, "#else /* SR_%s_UNDO */\n", ptr->name);
	fprintf(fp, "        dp->undoMethod = NULL;\n");
	fprintf(fp, "#endif /* SR_%s_UNDO */\n", ptr->name);
    }
    else {
	fprintf(fp, "        dp->undoMethod = NULL;\n");
    }
    fprintf(fp, "        dp->state = SR_UNKNOWN;\n");
    fprintf(fp, "\n");

    if (idx) {
	fprintf(fp, "        if (%s != NULL) {\n", ptr->name);
	fprintf(fp, "            /* fill in existing values */\n");
	fprintf(fp, "            if ((dp->data = (void *) Clone_%s(%s)) == NULL) {\n", ptr->name, ptr->name );
	fprintf(fp, "                DPRINTF((APWARN, \"Cannot allocate memory\\n\"));\n");
	fprintf(fp, "                error_status = RESOURCE_UNAVAILABLE_ERROR;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "            if ((dp->undodata = (void *) Clone_%s(%s)) == NULL) {\n", ptr->name, ptr->name );
	fprintf(fp, "                DPRINTF((APWARN, \"Cannot allocate memory\\n\"));\n");
	fprintf(fp, "                error_status = RESOURCE_UNAVAILABLE_ERROR;\n");
	fprintf(fp, "            }\n");


	fprintf(fp, "        }\n");
	fprintf(fp, "        else {\n");
	if (ptr->group_read_create == 0) {
	    fprintf(fp, "\n#ifdef %s_READ_CREATE \n", ptr->name);
	}

	fprintf(fp, "            if ( (dp->data = (void *) malloc (sizeof(%s_t))) == NULL) { \n", ptr->name);
	fprintf(fp, "                DPRINTF((APWARN, \"Cannot allocate memory\\n\"));\n");
	fprintf(fp, "                error_status = RESOURCE_UNAVAILABLE_ERROR;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "            else {\n");
	fprintf(fp, "#ifdef SR_CLEAR_MALLOC\n");
	fprintf(fp, "                memset(dp->data, 0, sizeof(%s_t));\n", ptr->name);
	fprintf(fp, "#endif /* SR_CLEAR_MALLOC */\n");
	fprintf(fp, "                dp->undodata = NULL;\n");
	fprintf(fp, "\n");

	fprintf(fp, "                /* Fill in reasonable default values for this new entry */\n");
	temp_index = index_array;
	while (temp_index && temp_index->name) {
	    temp = get_string(temp_index->asn1_type, temp_index->name, CLONE);
	    fprintf(fp, "                ((%s_t *) (dp->data))->%s = %s(%s);\n", ptr->name, temp_index->name, temp, temp_index->name);

	    /*
	     * first, see if this index was defined in this table.
	     */

	    found = 0;
	    temp_ptr = ptr->next_family_entry;
	    while (temp_ptr) {
		if (strcmp(temp_ptr->name, temp_index->name) == 0) {
			found = 1;
		}
		temp_ptr = temp_ptr->next_family_entry;
	    }

	    /*
	     * if it is defined in this table, print it here
    	     */
	    if (found) {
	        fprintf(fp, "                SET_VALID(I_%s, ((%s_t *) (dp->data))->valid);\n", temp_index->name, ptr->name);
	    /*
	     * Otherwise, add <TableName>Index after the I_ 
    	     */
    	    } else {
	        fprintf(fp, "                SET_VALID(I_%sIndex_%s, ((%s_t *) (dp->data))->valid);\n", ptr->name, temp_index->name, ptr->name);
 	    }

	    fprintf(fp, "\n");
	    temp_index++;
	}

	fprintf(fp, "                error_status = k_%s_set_defaults(dp);\n", ptr->name);
	fprintf(fp, "            }\n");
	if (ptr->group_read_create == 0) {
	    fprintf(fp, "#else /* %s_READ_CREATE */\n", ptr->name);
	    fprintf(fp, "            error_status = NO_CREATION_ERROR;\n");
	    fprintf(fp, "#endif /* %s_READ_CREATE */\n", ptr->name);
	}

	fprintf(fp, "        }\n");
    }
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        %s = (%s_t *) dp->undodata;\n", ptr->name, ptr->name);
    fprintf(fp, "    }\n");
    fprintf(fp, "\n");
    for (temp_index = index_array; 
	    temp_index && temp_index->name;
	    temp_index++) {
	print_string(fp, temp_index, FREE);
    }
    fprintf(fp, "    if (error_status != NO_ERROR) {\n");

    fprintf(fp, "        return error_status;\n");
    fprintf(fp, "    }\n");

    fprintf(fp, "\n");
    if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
        fprintf(fp, "    /*\n");
        fprintf(fp, "     * Find nominator from last_sid array\n");
        fprintf(fp, "     */\n");
        fprintf(fp, "    count = sizeof(%s_last_sid_array) / sizeof(SR_UINT32);\n", ptr->name);
        fprintf(fp, "    for (which = 0; which < count; which++) {\n");
        fprintf(fp, "        if (%s_last_sid_array[which] ==\n", ptr->name);
        fprintf(fp, "                 object->oid.oid_ptr[object->oid.length-1]) {\n");
        fprintf(fp, "            arg = which;\n");
        fprintf(fp, "            break;\n");
        fprintf(fp, "        }\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "    if (arg != which) {\n");
        fprintf(fp, "        return GEN_ERROR;\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "\n");
        fprintf(fp, "    switch (arg) {\n\n");
    } else {
        fprintf(fp, "    switch (object->nominator) {\n\n");
    }

    /* write a test case for each entry */
    for (temp_ptr = ptr->next_family_entry; temp_ptr; 
		    temp_ptr = temp_ptr->next_family_entry) {
        if (temp_ptr->valid) {
	    print_test_switch(fp, ptr, temp_ptr, temp_index, index_array,
	  	              storage_type_ptr);
        }
    }

   /*
    * Add an entry for any indexes which are in another
    * family but augment this family.
    */

    /*
     * if this group is augmented, print
     * the items which augment the
     * group to the type file
     */
    if (ptr->augmented) {
	struct OID_INFO *g1, *augments_list;

	for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
	    if (g1->augments) {
		if (strcmp(g1->augments, ptr->name) == 0) {
		    augments_list = g1->next_family_entry;
		    while (augments_list) {
                         if (augments_list->valid) {
			     print_test_switch(fp, ptr, augments_list, 
                                     temp_index, index_array, storage_type_ptr);
                         }
			 augments_list = augments_list->next_family_entry;
		    }
		}
	    }
	}
    }

    fprintf(fp, "   default:\n");
    fprintf(fp, "       DPRINTF((APALWAYS, \"Internal error (invalid nominator ");
    fprintf(fp, "in %s_test)\\n\"));\n", ptr->name);

    fprintf(fp, "       return GEN_ERROR;\n\n");

    fprintf(fp, "   }        /* switch */\n");

    fprintf(fp, "\n");
    fprintf(fp, "   /* Do system dependent testing in k_%s_test */\n", ptr->name);
    fprintf(fp, "   error_status = k_%s_test(object, value, dp, contextInfo);\n", ptr->name);
    fprintf(fp, "\n");
    if (offset) {
	fprintf(fp, "   if (error_status == NO_ERROR) {\n");
	fprintf(fp, "      error_status = AssignValue(object, dp->data, value);\n");
	fprintf(fp, "   }\n");
	fprintf(fp, "\n");
    }
    fprintf(fp, "   if (error_status == NO_ERROR) {\n");
    if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
        fprintf(fp, "        SET_VALID(arg, ((%s_t *) (dp->data))->valid);\n", ptr->name);
    } else {
        fprintf(fp, "        SET_VALID(object->nominator, ((%s_t *) (dp->data))->valid);\n", ptr->name);
    }
    fprintf(fp, "        error_status = k_%s_ready(object, value, doHead, dp);\n", ptr->name);
    fprintf(fp, "   }\n");
    fprintf(fp, "\n");
    fprintf(fp, "   return (error_status);\n");
    fprintf(fp, "}\n\n");
}

/*----------------------------------------------------------------
 * print the ready routine
 *---------------------------------------------------------------*/
void
write_v_ready_routine(fp, ptr)
FILE *fp;
struct OID_INFO *ptr;
{
    int             idx; 
    struct _index_array *index_array;

    /* get indexing information */
    idx = get_index_array(ptr->name, &index_array);

    /* emit the routine header */
    fprintf(fp, "/*%s\n", dashes);
    fprintf(fp, " * Determine if entries in this SET request are consistent\n");
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "int \n");
    fprintf(fp, "%s_ready", ptr->name);
    fprintf(fp, "(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)\n");
    fprintf(fp, "{\n");

    fprintf(fp, "    return v_ready(doHead, doCur, contextInfo,\n");
    fprintf(fp, "                  &%sTestInfo);\n", ptr->name);
    fprintf(fp, "}\n\n");
    return;
}

