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
* print the k_ ready method for this family if it contains writable
* variables
*----------------------------------------------------------------*/
int
write_k_ready_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    char           *destroy_string = "destroy";
    int             i, total_vars=0;

    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }
    if (ansi) {
        fprintf(fp, "int\nk_%s_ready(ObjectInfo *object, ObjectSyntax *value, \n", ptr->name);
        for (i = 0; i < (int) strlen(ptr->name); i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, "         doList_t *doHead, doList_t *dp)\n");
    } else {
        fprintf(fp, "int\nk_%s_ready(object, value, doHead, dp)\n", ptr->name);
        fprintf(fp, "    ObjectInfo     *object;\n");
        fprintf(fp, "    ObjectSyntax   *value;\n");
        fprintf(fp, "    doList_t       *doHead;\n");
        fprintf(fp, "    doList_t       *dp;\n");
    }
    fprintf(fp, "{\n");

    /* count the valid writable variables in this family */
    total_vars = 0;
    for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
        if (!temp_ptr->valid) continue;
        if ((strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) ||
                (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0)) {
            if (strcmp(temp_ptr->oid_type, testAndIncrString) == 0) {
                if (test_and_incr) {
                    /* ignore TestAndIncr variables */
                    continue;
                }
            }
            total_vars++;
        }
    }

    /* function prologue */
    if (row_status_in_family(ptr) == NULL) {
        if (total_vars > 0) {
            fprintf(fp, "#ifdef NOT_YET\n");
            fprintf(fp, "    %s_t *data = (%s_t *) dp->data;\n\n", ptr->name, ptr->name);
        }
    } else {
        fprintf(fp, "    int doListReady = 0;\n");
        fprintf(fp, "    %s_t *data = (%s_t *) dp->data;\n\n", ptr->name, ptr->name);
        if (total_vars > 0) {
            fprintf(fp, "#ifdef NOT_YET\n");
        }
    }

    /* produce common code */
    if (total_vars > 0) {
        fprintf(fp, "    /*\n");
        fprintf(fp, "     * Check the validity of MIB object values here.\n");
        fprintf(fp, "     * The proposed new values are in data.\n");
        fprintf(fp, "     * Perform checks on \"object value relationships\".\n");
        fprintf(fp, "     */\n\n");
        for (temp_ptr = ptr->next_family_entry; temp_ptr;
                                temp_ptr = temp_ptr->next_family_entry) {
            if (!temp_ptr->valid) continue;
            if ((strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) ||
                    (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) ||
                    (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0)) {
                if (strcmp(temp_ptr->oid_type, testAndIncrString) == 0) {
                    if (test_and_incr) {
                        /* ignore TestAndIncr variables */
                        continue;
                    }
                }
                fprintf(fp, "    if (VALID(I_%s, data->valid)) {\n", temp_ptr->name);
                fprintf(fp, "        /*\n");
                fprintf(fp, "         * Examine data->%s in relation to other\n", temp_ptr->name);
                fprintf(fp, "         * MIB objects.  If a relationship is not invalid, set\n");
                fprintf(fp, "         * dp->state to SR_UNKNOWN and return with NO_ERROR.\n");
                fprintf(fp, "         */\n");
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
                                    if (strcmp(augments_list->oid_type, testAndIncrString) == 0) {
                                        if (test_and_incr) {
                                            /* ignore TestAndIncr variables */
                                            augments_list = augments_list->next_family_entry;
                                            continue;
                                        }
                                    }
                                    fprintf(fp, "    if (VALID(I_%s, data->valid)) {\n", augments_list->name);
                                    fprintf(fp, "        /*\n");
                                    fprintf(fp, "         * Examine data->%s in relation to other\n", augments_list->name);
                                    fprintf(fp, "         * MIB objects.  If a relationship is not invalid, set\n");
                                    fprintf(fp, "         * dp->state to SR_UNKNOWN and return with NO_ERROR.\n");
                                    fprintf(fp, "         */\n");
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
        fprintf(fp, "#endif /* NOT_YET */\n");
    }

    if ((temp_ptr = row_status_in_family(ptr)) == NULL) {
	fprintf(fp, "    dp->state = SR_ADD_MODIFY;\n");
    }
    else {
        if (total_vars > 0) {
            fprintf(fp, "\n");
        }
	/* produce row status code */
	fprintf(fp, "    /* no state information needed for destroy */\n");
	fprintf(fp, "    if (data->%s == D_%s_%s) {\n", temp_ptr->name, temp_ptr->name, destroy_string);
	fprintf(fp, "        dp->state = SR_DELETE;\n");
	fprintf(fp, "    } else \n");
	fprintf(fp, "    if (data->%s == RS_UNINITIALIZED) {\n", temp_ptr->name);
	fprintf(fp, "        /* status hasn't been set yet */\n");
	fprintf(fp, "        dp->state = SR_UNKNOWN;\n");
	fprintf(fp, "    } else {\n");
	fprintf(fp, "        /* check row status state machine transitions */\n");
	fprintf(fp, "        if(data->%s == D_%s_createAndGo) {\n",
		temp_ptr->name, temp_ptr->name);
	fprintf(fp, "            data->%s = D_%s_active;\n",
		temp_ptr->name, temp_ptr->name);
	fprintf(fp, "        } \n");
	fprintf(fp, "        doListReady = 1;\n");
	fprintf(fp, "\n");
	fprintf(fp, "        /* if transitioning (or remaining) in the active or notInService states,\n");
	fprintf(fp, "         * make sure that the new values are not inconsistent enough to \n");
	fprintf(fp, "         * make the entry notReady */\n");
	fprintf(fp, "        if(data->%s == D_%s_active\n",
		temp_ptr->name, temp_ptr->name);
	fprintf(fp, "        || data->%s == D_%s_notInService) {\n",
		temp_ptr->name, temp_ptr->name);
	fprintf(fp, "            if(%sReadyToActivate(data) == 0) {\n", ptr->name);
	fprintf(fp, "                /* inconsistent values, don't allow the set operation */\n");
	fprintf(fp, "                doListReady = 0;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "\n");
	fprintf(fp, "        /* did we have all required data? */\n");
	fprintf(fp, "        if(doListReady != 0) {\n");
	fprintf(fp, "            /* yes, update the doList entry */\n");
	fprintf(fp, "            dp->state = SR_ADD_MODIFY;\n");
	fprintf(fp, "        } else {\n");
	fprintf(fp, "            /* no, make sure the set doesn't proceed */\n");
	fprintf(fp, "            dp->state = SR_UNKNOWN;\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "    }\n");
	fprintf(fp, "\n");
    }

    fprintf(fp, "    return NO_ERROR;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "\n");
    return 1;
}
