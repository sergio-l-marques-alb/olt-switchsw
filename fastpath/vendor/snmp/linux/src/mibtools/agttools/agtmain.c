/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#define	SR_AGENT_MAIN

#include "mibtools.h"
#include "agent.h"

/* global data used by other modules */
char            storageTypeString[] = "StorageType";
char            memoryTypeString[] = "MemoryType";
char            testAndIncrString[] = "TestAndIncr";
char            rowStatusString[] = "RowStatus";
char            dashes[] =
"---------------------------------------------------------------------";
int             writing_get_method = 0;

/*
 * print stub k_ method routines
 */
int
write_k_agent_files(base)
    char           *base;
{
    struct OID_INFO *ptr;
    char            buff[80];
    FILE           *fp;

    /* open the stub file */
    sprintf(buff, "k_%s", base);
    fp = open_file(buff, ".stb");

    /* write file headers */
    write_k_stb_headers(fp, base);

    /* write initialization and termination routines */
    write_k_init_routines(fp, base);

    /* start looking for valid families of objects to emit code for */
    for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	 ptr = ptr->next_sorted) {
	/* write a routine to make sure that the requests' context is ok */
	write_k_context_routine(fp, ptr);

	/* write row status support routines if necessary */
	write_row_status_routines(fp, ptr);

	/* handle userpart routines */
        if (userpart && (ptr->group_read_write || search_table)) {
	    /* write routines to free userpart data */
	    write_k_free_userpart_routine(fp, ptr);
	    write_k_clone_userpart_routine(fp, ptr);
	}

	/* write out get method routines */
	write_k_get_routine(fp, ptr, new_k_stubs);

	/* write set-method code if this is a writeable group */
	if (ptr->group_read_write) {
	    fprintf(fp, "#ifdef SETS\n");
	    write_k_test_routine(fp, ptr);
	    write_k_ready_routine(fp, ptr);
	    write_k_set_defaults_routine(fp, ptr);
	    write_k_set_routine(fp, ptr, base);
	    write_k_undo_routine(fp, ptr);
	    fprintf(fp, "#endif /* SETS */\n\n");
	}
    }

    /* close the output file */
    fclose(fp);
    return 1;
}

/*
 * print stub k_ method routines
 */
int
write_v_agent_files(base)
    char           *base;
{
    struct OID_INFO *ptr;
    struct OID_INFO *temp_ptr;
    char             buff[80];
    FILE            *fp;

    sprintf(buff, "v_%s", base);
    fp = open_file(buff, ".stb");

    /* write headers for the file */
    write_v_stb_headers(fp);

    /* start looking for valid families of objects to emit code for */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {

        if ((ptr->family_root == 0) || (!ptr->valid)) {
            continue;
        }

        if (ptr->augments) {
            continue;
        }
  
        if (!IsValidFamily(ptr)) {
            continue;
        }
        if (!IsReadableFamily(ptr)) {
            continue;
        }

        if (no_v_get && !small_v_get) {
            /* write a comment noting the absence of the v_ routine */
            write_no_v_get_comment(fp, ptr);
        }

        if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
            emit_last_sid_array(fp, ptr);
        }

        if (v_elimination_data) {
            /*
             * The following if-statement also appears in the function
             * write_k_stb_headers().  If you change the conditional logic
             * here, change it in the other function also.
             */
            /* generate the TypeTable data structure */
            if ((row_status_in_family(ptr) != NULL) || search_table || parser) {
                /*
                 * The <family>TypeTable for tabular families will be
                 * added to k_<base>.stb somewhere else.  Here, we
                 * should add the <family>TypeTable to v_<base>.stb
                 * only for scalar families.
                 */
                write_scalar_family_TypeTable(fp, ptr);
                write_tabular_family_TypeTable_comment(fp, ptr, base);
            } else {
                /*
                 * No <family>TypeTable arrays are being generated
                 * elsewhere.  So for any family (scalar or tabular),
                 * we add the data structure to the v_<base>.stb file.
                 */
                write_any_family_TypeTable(fp, ptr);
            }

            /* generate the SrIndexInfo array */
            write_SrIndexInfo(fp, ptr);
        }

        if (v_elimination_data && !small_v_uses_global) {
            emit_SrGetInfoEntry_standalone(fp, ptr);
            if (ptr->augmented) { 
                for (temp_ptr = sorted_root; temp_ptr;
                temp_ptr = temp_ptr->next_sorted) {
                    if (temp_ptr->augments
                        && strcmp(temp_ptr->augments, ptr->name) == 0) {
                        emit_SrGetInfoEntry_augments_standalone(fp, ptr, 
                                                                temp_ptr);
                    }
                }
            }
        }

        if (!no_v_get) {
	    /* emit a get method routine */
	    writing_get_method = 1;
	    write_v_get_routine(fp, ptr);
	    writing_get_method = 0;
        }

        if (new_k_wrappers) {
            /* generate wrappers for old k_get routines */
            write_k_get_wrapper(fp, ptr);
        }


	/* emit set-method code if the family is read-write */
	if (ptr->group_read_write) {
	    fprintf(fp, "#ifdef SETS \n\n");
	    write_v_cleanup_prototype(fp, ptr);
            if (v_elimination_data) {
                emit_SrTestInfoEntry_auxiliary_array(fp, ptr);
                emit_SrTestInfoEntry_standalone(fp, ptr);
                if (ptr->augmented) {
                    for (temp_ptr = sorted_root; temp_ptr;
                    temp_ptr = temp_ptr->next_sorted) {
                        if (temp_ptr->augments
                            && strcmp(temp_ptr->augments, ptr->name) == 0) {
                            emit_SrTestInfoEntry_augments_standalone(fp, ptr, 
                                                                     temp_ptr);
                        }
                    }
                }

            }
	    write_v_free_routine(fp, ptr);
	    write_v_cleanup_routine(fp, ptr);
	    write_v_clone_routine(fp, ptr);
	    write_v_test_routine(fp, ptr);
            if (v_elimination_data) {
	        write_v_ready_routine(fp, ptr);
            }
	    write_v_set_routine(fp, ptr);
	    fprintf(fp, "#endif /* SETS */\n\n\n");
	} else {
            if (search_table) {
	        write_v_free_routine(fp, ptr);
            }
        }
    }

    /* close the output file */
    fclose(fp);
    return 1;
}
