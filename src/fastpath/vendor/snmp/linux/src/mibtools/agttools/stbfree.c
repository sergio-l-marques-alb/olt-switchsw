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

/* write a routine to free userpart data */
void
write_k_free_userpart_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    fprintf(fp, "#ifdef U_%s\n", ptr->name);
    fprintf(fp, "/* This routine can be used to free data which\n");
    fprintf(fp, " * is defined in the userpart part of the structure */\n");
    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }
    fprintf(fp, "void\n");
    if (ansi) {
        fprintf(fp, "k_%sFreeUserpartData (%s_t *data)\n", ptr->name, ptr->name);
    } else {
        fprintf(fp, "k_%sFreeUserpartData (data)\n", ptr->name);
        fprintf(fp, "    %s_t *data;\n", ptr->name);
    }
    fprintf(fp, "{\n");
    fprintf(fp, "    /* nothing to free by default */\n");
    fprintf(fp, "}\n");
    fprintf(fp, "#endif /* U_%s */\n", ptr->name);
    fprintf(fp, "\n");
}

/* write the k_ routine to clone userpart */
void
write_k_clone_userpart_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    fprintf(fp, "#ifdef U_%s\n", ptr->name);
    fprintf(fp, "/* This routine can be used to clone data which\n");
    fprintf(fp, " * is defined in the userpart part of the structure */\n");
    if (argsused) {
        fprintf(fp, "/*ARGSUSED*/\n");
    }
    fprintf(fp, "void\n");
    if (ansi) {
        fprintf(fp, "k_%sCloneUserpartData (%s_t *dst, %s_t *src)\n", 
		ptr->name, ptr->name, ptr->name);
    } else {
        fprintf(fp, "k_%sCloneUserpartData (dst, src)\n", ptr->name);
        fprintf(fp, "    %s_t *dst, *src;\n", ptr->name);
    }
    fprintf(fp, "{\n");
    fprintf(fp, "    /* nothing to clone by default */\n");
    fprintf(fp, "}\n");
    fprintf(fp, "#endif /* U_%s */\n", ptr->name);
    fprintf(fp, "\n");
}

/*----------------------------------------------------------------
 * first print the free routine
 *---------------------------------------------------------------*/
void
write_v_free_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    char            buff[80];

    struct OID_INFO *g1, *augments_list; 
    struct _index_array *index_array, *temp_index;

    /* get an array containing the indicies of this family */
    get_index_array(ptr->name, &index_array);

    if (small_v_test) {
        if (!index_array && !search_table) {
            /* don't need free routine for scalars with small_v_test */
            return;
        }
    }

    fprintf(fp, "/*%s-\n", dashes);
    fprintf(fp, " * Free the %s data object.\n", ptr->name);
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "void\n");
    if (ansi) {
        fprintf(fp, "%s_free(%s_t *data)\n", ptr->name, ptr->name);
    } else {
        fprintf(fp, "%s_free(data)\n", ptr->name);
        /* fprintf(fp, "    %s_t       *data;\n"); */
        sprintf(buff, "%s_t", ptr->name);
        fprintf(fp, "   %-15s *data;\n", buff);
    }
    fprintf(fp, "{\n");
    if (small_v_test) {
        fprintf(fp, "    /* free function is only needed by old k_ routines with -row_status */\n");
        fprintf(fp, "    SrFreeFamily(data,\n");
        fprintf(fp, "                 %sGetInfo.type_table,\n", ptr->name);
        fprintf(fp, "                 %sGetInfo.highest_nominator,\n", 
                ptr->name);
        fprintf(fp, "                 %sGetInfo.valid_offset,\n", ptr->name);
        fprintf(fp, "                 %sGetInfo.userpart_free_func);\n", 
                ptr->name);
        fprintf(fp, "}\n\n");

        return;
    }
    fprintf(fp, "   if (data != NULL) {\n");
    if (userpart != 0) {
	fprintf(fp, "#ifdef U_%s\n", ptr->name);
	fprintf(fp, "       /* free user-defined data */\n");
	fprintf(fp, "       k_%sFreeUserpartData (data);\n", ptr->name);
	fprintf(fp, "#endif /* U_%s */\n", ptr->name);
	fprintf(fp, "\n");
    }

    temp_ptr = ptr->next_family_entry;
    while (temp_ptr) {
        if (temp_ptr->valid) {
	    /* if (temp_ptr->read_write) { */
    	    fprintf(fp, "%s", get_string(temp_ptr->oid_prim_type,
	    			         temp_ptr->name, FREE));
	    /* } */
        }
	temp_ptr = temp_ptr->next_family_entry;
    }


    /*
     * Free fields where the object is defined in a different table
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
	                fprintf(fp, "%s", get_string(temp_index->asn1_type, temp_ptr->name, FREE));
                        break;
                    }
                }
            }
        }	/* for(temp_index... */
    }	/* if(index_array) */


#ifndef NO_AUGMENTS
    /*
     * if this group is augmented, print the items which
     * augment the group
     */
    if (ptr->augmented) {
        fprintf(fp, "\n");
	for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
	    if (g1->augments) {
		if (strcmp(g1->augments, ptr->name) == 0) {
		    augments_list = g1->next_family_entry;
		    while (augments_list) {
		        if (augments_list->valid) {
	                    fprintf(fp, "%s", get_string(augments_list->oid_prim_type, augments_list->name, FREE));
		        }
                        augments_list = augments_list->next_family_entry;
		    }
	    	}
            }
	}
    }
#endif /* NO_AUGMENTS */


    fprintf(fp, "\n");
    fprintf(fp, "       free ((char *) data);\n");

    fprintf(fp, "   }\n");
    /* fprintf(fp, "\n"); */
    fprintf(fp, "}\n\n");
}

#define _CLEANUP_STRING "static int %s_cleanup\n\
    SR_PROTOTYPE((doList_t *trash));\n"

/*----------------------------------------------------------------
 * Now print the cleanup routine
 *---------------------------------------------------------------*/
void
write_v_cleanup_prototype(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
     fprintf(fp, "#ifdef __cplusplus\n");
     fprintf(fp, "extern \"C\" {\n");
     fprintf(fp, "#endif\n");
     fprintf(fp, _CLEANUP_STRING, ptr->name);
     fprintf(fp, "#ifdef __cplusplus\n");
     fprintf(fp, "}\n");
     fprintf(fp, "#endif\n\n");

}

void
write_v_cleanup_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    fprintf(fp, "/*%s-\n", dashes);
    fprintf(fp, " * cleanup after %s set/undo\n", ptr->name);
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "static int\n");
    if (ansi) {
        fprintf(fp, "%s_cleanup(doList_t *trash)\n", ptr->name);
    } else {
        fprintf(fp, "%s_cleanup(trash)\n", ptr->name);
        fprintf(fp, "   doList_t *trash;\n");
    }
    fprintf(fp, "{\n");
    if (small_v_test) {
       fprintf(fp, "    return SrCleanup(trash, &%sTestInfo);\n", ptr->name);   
    } else {
       fprintf(fp, "   %s_free((%s_t *) trash->data);\n", ptr->name, ptr->name);
       fprintf(fp, "   %s_free((%s_t *) trash->undodata);\n", ptr->name, ptr->name);
        fprintf(fp, "   return NO_ERROR;\n");
    }
    fprintf(fp, "}\n");
    fprintf(fp, "\n");
}
