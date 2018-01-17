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

/* this routine writes a timer-based row deletion callback */
void
write_rs_delete_callback(fp, ptr, row_status_ptr)
    FILE           *fp;
    struct OID_INFO *ptr, *row_status_ptr;
{
    char           *status_type = rowStatusString;
    char           *destroy_string = "destroy";
    char           *struct_name = ptr->parent_ptr->name;


    /* generate a routine to delete rows based on timeouts */
    fprintf(fp, "/* This routine is called by the timeout code to \n");
    fprintf(fp, " * delete a pending creation of a Table entry */ \n");
    fprintf(fp, "void\n");
    if (old_row_timer == 1) {
        /* use old API */
        if (ansi) {
            fprintf(fp, "%sDeleteCallback (TimeOutDescriptor *tdp)\n",struct_name);
        } else {
            fprintf(fp, "%sDeleteCallback (tdp)\n", struct_name);
            fprintf(fp, "    TimeOutDescriptor *tdp;\n");
        }
        fprintf(fp, "{\n");
        fprintf(fp, "    %s_t *data;\n", ptr->name);
        fprintf(fp, "\n");
        fprintf(fp, "    /* dummy up an entry to delete */\n");
        fprintf(fp, "    data = (%s_t *) tdp->UserData2;\n", ptr->name);
        fprintf(fp, "    data->%s = D_%s_%s;\n", row_status_ptr->name,
                                       row_status_ptr->name, destroy_string);
        fprintf(fp, "    data->%sTimerId = -1;\n", status_type);
        fprintf(fp, "\n");
        fprintf(fp, "    /* free the timeout descriptor */\n");
        fprintf(fp, "    free(tdp);\n");
        fprintf(fp, "    /* call the set method */\n");
        fprintf(fp, "    k_%s_set(data, (ContextInfo *) NULL, 0);\n",
                                                                  ptr->name);
        fprintf(fp, "}\n");
        fprintf(fp, "\n");
    }
    else {
        /* use new API */
        if (ansi) {
            fprintf(fp, "%sDeleteCallback (SR_UINT32 timeout_type,\n", struct_name);
            fprintf(fp, "                  int timer_id,\n");
            fprintf(fp, "                  SR_INT32 userdata1,\n");
            fprintf(fp, "                  void *userdata2)\n");
        } else {
            fprintf(fp, "%sDeleteCallback (timeout_type, timer_id, userdata1, userdata2)\n", struct_name);
            fprintf(fp, "    SR_UINT32 timeout_type;\n");
            fprintf(fp, "    int timer_id;\n");
            fprintf(fp, "    SR_INT32 userdata1;\n");
            fprintf(fp, "    void *userdata2;\n");
        }
        fprintf(fp, "{\n");
        fprintf(fp, "    %s_t *data;\n", ptr->name);
        fprintf(fp, "\n");
        fprintf(fp, "    /* dummy up an entry to delete */\n");
        fprintf(fp, "    data = (%s_t *) userdata2;\n", ptr->name);
        fprintf(fp, "    data->%s = D_%s_%s;\n", row_status_ptr->name,
                                       row_status_ptr->name, destroy_string);
        fprintf(fp, "    data->%sTimerId = -1;\n", status_type);
        fprintf(fp, "\n");
        fprintf(fp, "    /* call the set method */\n");
        fprintf(fp, "    k_%s_set(data, (ContextInfo *) NULL, 0);\n",
                                                                  ptr->name);
        fprintf(fp, "}\n");
        fprintf(fp, "\n");
    }
}

/* this routine writes a table entry deletion routine */
void
write_rs_delete_entry_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    char           *struct_name = ptr->parent_ptr->name;


    /* generate a routine to delete an entry from a table */
    fprintf(fp, "/* This routine deletes an entry from the %s */\n",
	    struct_name);
    fprintf(fp, "void\n");
    if (ansi) {
        fprintf(fp, "Delete%s (int index)\n", ptr->name);
    } else {
        fprintf(fp, "Delete%s (index)\n", ptr->name);
        fprintf(fp, "    int index;\n");
    }
    fprintf(fp, "{\n");
    fprintf(fp, "    %s_t *data;\n", ptr->name);
    fprintf(fp, "\n");
    fprintf(fp, "    /* get a pointer to the old entry */\n");
    fprintf(fp, "    data = (%s_t *) %s.tp[index];\n", ptr->name,
	    struct_name);
    fprintf(fp, "\n");
    fprintf(fp, "    /* free the old entry and remove it from the table */\n");
    fprintf(fp, "    %s_free(data);\n", ptr->name);
    fprintf(fp, "    RemoveTableEntry(&%s, index);\n",
	    struct_name);
    fprintf(fp, "}\n");
    fprintf(fp, "\n");
}

/* this routine write out an activation consistency check routine */
void
write_rs_ready_to_activate_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    /*
     * generate a routine which will be used to monitor the consistency of
     * data in the row
     */
    fprintf(fp, "/* This prototype is required to please some compilers */\n");
    fprintf(fp, "int %sReadyToActivate\n", ptr->name);
    fprintf(fp, "    SR_PROTOTYPE((%s_t *data));\n", ptr->name);
    fprintf(fp, "/* This routine returns nonzero if the table entry contains sufficient\n");
    fprintf(fp, " * information to be in the active (or notInService) state. */\n");
    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }
    if (ansi) {
        fprintf(fp, "int\n%sReadyToActivate(%s_t *data)\n", ptr->name, ptr->name);
    } else {
        fprintf(fp, "int\n%sReadyToActivate(data)\n", ptr->name);
        fprintf(fp, "%s_t *data;\n", ptr->name);
    }
    fprintf(fp, "{\n");
    fprintf(fp, "    /* by default, assume sufficient information */\n");
    fprintf(fp, "    return(1);\n");
    fprintf(fp, "}\n\n");
}

/* this routine write out rowStatus support routines */
void
write_row_status_routines(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *row_status_ptr;
    struct _index_array *index_array;
    int             idx;

    /* determine if this is a table */
    idx = get_index_array(ptr->name, &index_array);

    /* is row status used in this family? */
    if ((row_status_ptr = row_status_in_family(ptr)) != NULL) {
        if (ptr->group_read_write) {
	    /* generate a routine to delete rows based on timeouts */
	    write_rs_delete_callback(fp, ptr, row_status_ptr);
        }

	/* generate a routine to delete an entry from a table */
	write_rs_delete_entry_routine(fp, ptr);

	/*
	 * generate a routine which will be used to monitor the consistency
	 * of data in the row
	 */
	write_rs_ready_to_activate_routine(fp, ptr);
    } else if ((search_table || parser) && (idx != 0)) {
	/* generate a routine to delete an entry from a table */
	write_rs_delete_entry_routine(fp, ptr);
    }
}
