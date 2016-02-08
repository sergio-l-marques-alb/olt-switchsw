/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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
 * print the undo method for this family if it contains writable
 * variables
 *----------------------------------------------------------------*/
int
write_k_undo_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    struct _index_array *index_array;
    char           *destroy_string = "destroy";
    int i = 0;

    if (undo) {
	/* produce conditional compilation directives */
	fprintf(fp, "\n");
	fprintf(fp, "#ifdef SR_%s_UNDO\n", ptr->name);
	fprintf(fp, "/* add #define SR_%s_UNDO in sitedefs.h to\n", ptr->name);
	fprintf(fp, " * include the undo routine for the %s family.\n", ptr->name);
	fprintf(fp, " */\n");

	/* function header */
	if (argsused) {
	    fprintf(fp, "/*ARGSUSED*/\n");
	}
	fprintf(fp, "int\n");
        if (ansi) {
	    fprintf(fp, "%s_undo(doList_t *doHead, doList_t *doCur,\n", ptr->name);
	    for (i = 0; i < (int) strlen(ptr->name); i++) {
		fprintf(fp, " ");
	    }
            fprintf(fp, "      ContextInfo *contextInfo)\n");
        } else {
	    fprintf(fp, "%s_undo(doHead, doCur, contextInfo)\n", ptr->name);
	    fprintf(fp, "    doList_t       *doHead;\n");
	    fprintf(fp, "    doList_t       *doCur;\n");
	    fprintf(fp, "    ContextInfo    *contextInfo;\n");
        }
	fprintf(fp, "{\n");

	/* is row status used in this family? */
	if ((temp_ptr = row_status_in_family(ptr)) != NULL) {
	    /* row status function prologue */
	    fprintf(fp, "    %s_t *data = (%s_t *) doCur->data;\n", ptr->name, ptr->name);
	    fprintf(fp, "    %s_t *undodata = (%s_t *) doCur->undodata;\n", ptr->name, ptr->name);
	    fprintf(fp, "    %s_t *setdata = NULL;\n", ptr->name);
	    fprintf(fp, "    int function;\n");
	    fprintf(fp, "\n");

	    /* determine the row status undo operation type */
	    fprintf(fp, "    /* we are either trying to undo an add, a delete, or a modify */\n");
	    fprintf(fp, "    if (undodata == NULL) {\n");
	    fprintf(fp, "        /* undoing an add, so delete */\n");
	    fprintf(fp, "        data->%s = D_%s_%s;\n", temp_ptr->name, temp_ptr->name, destroy_string);
	    fprintf(fp, "        setdata = data;\n");
	    fprintf(fp, "        function = SR_DELETE;\n");
	    fprintf(fp, "    } else {\n");
	    fprintf(fp, "        /* undoing a delete or modify, replace the original data */\n");
	    fprintf(fp, "        if(undodata->%s == D_%s_notReady\n",
		    temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        || undodata->%s == D_%s_notInService) {\n",
		    temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "            undodata->%s = D_%s_createAndWait;\n",
		    temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        } else \n");
	    fprintf(fp, "        if(undodata->%s == D_%s_active) {\n",
		    temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "            undodata->%s = D_%s_createAndGo;\n",
		    temp_ptr->name, temp_ptr->name);
	    fprintf(fp, "        }\n");
	    fprintf(fp, "        setdata = undodata;\n");
	    fprintf(fp, "        function = SR_ADD_MODIFY;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");

	    /* perform the undo operation */
	    fprintf(fp, "    /* use the set method for the undo */\n");
	    fprintf(fp, "    if ((setdata != NULL) && k_%s_set(setdata, contextInfo, function) == NO_ERROR) {\n", ptr->name);
	    fprintf(fp, "        return NO_ERROR;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}
	else if ((get_index_array(ptr->name, &index_array) == 0) && (parser)) {
            fprintf(fp, "   if (k_%s_set((%s_t *)doCur->undodata,\n", ptr->name, ptr->name);
            fprintf(fp, "       contextInfo, SR_ADD_MODIFY) == NO_ERROR) {\n");
	    fprintf(fp, "       return NO_ERROR;\n");
	    fprintf(fp, "   }\n");
	}

	/*
	 * emit the standard termination for undo routines -- this is the
	 * same for row-status and non-row-status families
	 */
	fprintf(fp, "   return UNDO_FAILED_ERROR;\n");
	fprintf(fp, "}\n");
	fprintf(fp, "#endif /* SR_%s_UNDO */\n", ptr->name);
	fprintf(fp, "\n");
    }
    return 1;
}
