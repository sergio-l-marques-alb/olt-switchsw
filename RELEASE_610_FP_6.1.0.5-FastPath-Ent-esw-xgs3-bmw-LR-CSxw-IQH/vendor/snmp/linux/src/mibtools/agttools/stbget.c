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


void check_object_instance(FILE *, struct OID_INFO *);

/*
 * This routine prints the function header, or signature (the part of
 * a C function that comes before the open curly brace).
 *
 * The "old style" k_get method always has exactly three parameters for
 * scalar groups.  For tables, the method has a variable number of
 * parameters.  There are four parameters that are always the same, plus
 * there is one extra parameter for each table index.
 *
 * The "new style" k_get method always has exactly five parameters.
 * The first four parameters are the same as the first four parameters
 * of an old-style method for tables.  The fifth parameter is a pointer
 * to the <family>_t structure, which contains all of the index
 * parameters, regardless of how many there are.
 * 
 * To avoid naming conflicts with the old-style methods, the new-style
 * k_get method routines begin with a prefix (initially, "new_").
 */

const char *k_prefix  = "new_";
const char *k_padding = "    ";

/* write_k_get_signature()
 *
 * if new evaluates true, generate new_k_
 */
static void
write_k_get_signature(FILE *fp, struct OID_INFO *ptr, int new)
{
    int                  idx = 0, i = 0;
    struct _index_array *index_array = NULL, *temp_index = NULL;

    /* print the function header */
    if (argsused) {
	fprintf(fp, "/*ARGSUSED*/\n");
    }

    if (new) {
        if (ansi) {
            fprintf(fp, "%s_t *\n%sk_%s_get(int serialNum, ContextInfo *contextInfo,\n", ptr->name, k_prefix, ptr->name);
            for (i = 0; i < (int) strlen(ptr->name); i++) {
                fprintf(fp, " ");
            }
            fprintf(fp, "%s       int nominator, int searchType,\n", k_padding);
            for (i = 0; i < (int) strlen(ptr->name); i++) {
                fprintf(fp, " ");
            }
            fprintf(fp, "%s       %s_t *data)\n", k_padding, ptr->name);
        } else {
            fprintf(fp, "%s_t *\n%sk_%s_get(serialNum, contextInfo, nominator, searchType, data)\n",
                        ptr->name, k_prefix, ptr->name);
            fprintf(fp, "   int serialNum;\n");
            fprintf(fp, "   ContextInfo *contextInfo;\n");
            fprintf(fp, "   int nominator;\n");
            fprintf(fp, "   int searchType;\n");
            fprintf(fp, "   %s_t *data;\n", ptr->name);
        }
    } else {
        idx = get_index_array(ptr->name, &index_array);
        if (ansi) {
            fprintf(fp, "%s_t *\nk_%s_get(int serialNum, ContextInfo *contextInfo,\n", ptr->name, ptr->name);
            for (i = 0; i < (int) strlen(ptr->name); i++) {
                fprintf(fp, " ");
            }
            fprintf(fp, "       int nominator");
            if (idx != 0) {
                fprintf(fp, ",\n");
                for (i = 0; i < (int) strlen(ptr->name); i++) {
                    fprintf(fp, " ");
                }
                fprintf(fp, "       int searchType");
            }
        } else {
            fprintf(fp, "%s_t *\nk_%s_get(serialNum, contextInfo, nominator",
                    ptr->name, ptr->name);
            if (idx != 0) {
                fprintf(fp, ", searchType");
            }
        }
        temp_index = index_array;
        while (temp_index && temp_index->name) {
            if (ansi) {
                fprintf(fp, ",\n");
                for (i = 0; i < (int) strlen(ptr->name); i++) {
                    fprintf(fp, " ");
                }
                fprintf(fp, "       %s %s", temp_index->c_type, temp_index->name);
            } else {
                fprintf(fp, ", %s", temp_index->name);
            }
            temp_index++;
        }
        fprintf(fp, ")\n");
        if (!ansi) {
            fprintf(fp, "   int serialNum;\n");
            fprintf(fp, "   ContextInfo *contextInfo;\n");
            fprintf(fp, "   int nominator;\n");
            if (idx != 0) {
                fprintf(fp, "   int searchType;\n");
            }

            temp_index = index_array;
            while (temp_index && temp_index->name) {
                fprintf(fp, "   %s %s;\n", temp_index->c_type, temp_index->name);
                temp_index++;
            }
        }
    }
}


/*
 * This routine creates a new-style "k_get" method that wraps around
 * a pre-existing old-style "k_get" method, useful for migration purposes.
 */
void
write_k_get_wrapper(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    int                  idx = 0, i = 0, pos = 0, famlen = 0, access_len = 0;
    struct _index_array *index_array = NULL, *temp_index = NULL;
    const char          *padding = "                  ";
    const char          *access = "data->";

    write_k_get_signature(fp, ptr, 1);  /* write a new-style wrapper */
    fprintf(fp, "{\n");

    idx = get_index_array(ptr->name, &index_array);
    if (idx != 0) {
        fprintf(fp, "    if (data == NULL) {\n");
        fprintf(fp, "        return NULL;\n");
        fprintf(fp, "    }\n");
    }
    fprintf(fp, "    return k_%s_get(serialNum, contextInfo, nominator",
            ptr->name);
    if (idx != 0) {
        fprintf(fp, ",\n");
        access_len = strlen(access);
        famlen = strlen(ptr->name);
        for (i = 0; i < famlen; i++) {
            fprintf(fp, " ");
        }
        fprintf(fp, padding);
        fprintf(fp, "searchType");
	pos = famlen + strlen(padding) + strlen("searchType");
        temp_index = index_array;
        while (temp_index && temp_index->name) {
            if ((pos + strlen(temp_index->name) + access_len) > 75) {
                fprintf(fp, ",\n");
                for (i = 0; i < famlen; i++) {
                    fprintf(fp, " ");
                }
                fprintf(fp, "%s%s%s", padding, access, temp_index->name);
                pos = famlen + strlen(padding) +
                      access_len + strlen(temp_index->name);
            } else {
                fprintf(fp, ", %s%s", access, temp_index->name);
                pos += 2;  /* for the comma and space */
                pos += access_len;
                pos += strlen(temp_index->name);
            }
            temp_index++;
        }
    }
    fprintf(fp, ");\n");
    fprintf(fp, "}\n\n");
}


/* this routine writes the "k_get" method routines for the agent */
void
write_k_get_routine(fp, ptr, new)
    FILE           *fp;
    struct OID_INFO *ptr;
    int             new;   /* if new evaluates true, generate new_k_ */
{
    struct OID_INFO *temp_ptr;
    struct _index_array *index_array, *temp_index;
    int             idx;
    int             count;
    int             found;
    char           *status_type = 0;
    char           *struct_name = NULL;

    if (ptr->parent_ptr) {
        struct_name = ptr->parent_ptr->name;
    }
 
 
    write_k_get_signature(fp, ptr, new);
    fprintf(fp, "{\n");

    /* determine if this is a table */
    idx = get_index_array(ptr->name, &index_array);

    /* count the number of indices */
    count = 0;
    if (idx) {
        temp_index = index_array;
        while (temp_index && temp_index->name) {
            count++;
            temp_index++;
        }
    }

    /* is row status being used? */
    if ((row_status_in_family(ptr) != NULL) || search_table || parser) {
	status_type = rowStatusString;
    }

    /* emit the actual code */
    if (status_type != NULL || ((idx == 0) && parser)) {
        if (idx == 0) {
            if (parser) {
                fprintf(fp, "   if ((nominator < 0) ||\n");
		fprintf(fp, "       VALID(nominator, %sData.valid)) {\n", ptr->name);
                fprintf(fp, "       return (&%sData);\n", ptr->name);
                fprintf(fp, "   }\n");
                fprintf(fp, "   else {\n");
                fprintf(fp, "       return NULL;\n");
                fprintf(fp, "   }\n");
            } else {
                fprintf(fp, "   return %sData;\n", ptr->name);
            }
        } else {
	    /* if row status is used in this family, print support routines */
	    fprintf(fp, "    int index;\n");
    	    fprintf(fp, "\n");
	    for (temp_index = index_array, count = 0;
	         temp_index && temp_index->name; temp_index++, count++) {
	        fprintf(fp, "    %s.tip[%d].value.", struct_name, count);
	        print_string(fp, temp_index, INDEX_CAT);
	        if (new) {
	            fprintf(fp, "_val = data->%s;\n", temp_index->name);
	        } else {
	            fprintf(fp, "_val = %s;\n", temp_index->name);
	        }
	    }
	    fprintf(fp, "    if ((index = SearchTable(&%s, searchType)) == -1) {\n", struct_name);
	    fprintf(fp, "        return NULL;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "    return (%s_t *) %s.tp[index];\n", ptr->name,
		struct_name);
	    fprintf(fp, "\n");
	}
    }
    else {
	if ((!table_lookup) || (idx == 0)) {
	    fprintf(fp, "#ifdef NOT_YET\n");
	    fprintf(fp, "   static %s_t %sData;\n", ptr->name, ptr->name);
	    fprintf(fp, "\n");
	    fprintf(fp, "   /*\n");
	    fprintf(fp, "    * put your code to retrieve the information here\n");
	    fprintf(fp, "    */\n");
	    fprintf(fp, "\n");
	}

	if (table_lookup && (idx != 0)) {
	    fprintf(fp, "    Index_t I[%d];\n", count);
	    fprintf(fp, "    int which;\n");
	    fprintf(fp, "\n");
	    count = 0;
	    temp_index = index_array;
	    while (temp_index && temp_index->name) {
		fprintf(fp, "    I[%d].offset = I_%s;\n", count,
			temp_index->name);
		fprintf(fp, "    I[%d].type = T_", count);
		print_string(fp, temp_index, INDEX_CAT);
		fprintf(fp, ";\n");
		fprintf(fp, "    I[%d].value.", count);
		print_string(fp, temp_index, INDEX_CAT);
	        if (new) {
		    fprintf(fp, "_val = data->%s;\n", temp_index->name);
	        } else {
		    fprintf(fp, "_val = %s;\n", temp_index->name);
	        }
		fprintf(fp, "\n");
		count++;
		temp_index++;
	    }
	    fprintf(fp, "    which = TableLookup((anytype_t **)%s, %s_num_used,\n", ptr->name, ptr->name);
	    fprintf(fp, "                   I, %d, searchType);\n", count);
	    fprintf(fp, "\n");
	    fprintf(fp, "    if(which == -1) {\n");
	    fprintf(fp, "        return NULL;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "    return %s[which];\n", ptr->name);
	    fprintf(fp, "\n");
	}
	else {

            /* If the variable is write-only, return NULL */
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
                if ((strcmp(temp_ptr->oid_access, "write-only")) == 0) {
                    fprintf(fp, "   if (nominator == I_%s) {\n", temp_ptr->name);
                    fprintf(fp, "       return NULL;\n");
                    fprintf(fp, "   }\n");
                    fprintf(fp, "\n");
                }
            }

	    for (temp_ptr = ptr->next_family_entry; temp_ptr;
		 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
		fprintf(fp, "   %sData.%s = ;\n", ptr->name, temp_ptr->name);
	    }

	    /*
	     * Add an entry for any indexes which are in another family
	     */

	    /*
	     * get an array containing the indicies of this family (from the
	     * INDEX clause)
	     */
	    get_index_array(ptr->name, &index_array);

	    /*
	     * if this is not a table, then it has no INDEX clause, and
	     * index_array will be NULL.
	     */

	    if (index_array) {
		/* for each index for this table */
		for (temp_index = index_array; temp_index->name; temp_index++) {

		    /*
		     * first, see if this index is in this table.
		     */

		    found = 0;
		    temp_ptr = ptr->next_family_entry;
		    while (temp_ptr) {
			if (strcmp(temp_ptr->name, temp_index->name) == 0)
			    found = 1;
			temp_ptr = temp_ptr->next_family_entry;
		    }

		    /*
		     * if is is not in the structure, then put it in now
		     */
		    if (!found) {
			fprintf(fp, "   %sData.%s = ;\n", ptr->name,
				temp_index->name);
		    }
		}		/* for(temp_index... */
	    }			/* if(index_array) */

	    /*
	     * Add an entry for any indexes which are in another family but
	     * augment this family.
	     */

	    /*
	     * if this group is augmented, print the items which augment the
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
			   	    fprintf(fp, "   %sData.%s = ;\n", ptr->name,
				    	    augments_list->name);
			        }
				augments_list =
				    augments_list->next_family_entry;
			    }
			}
		    }
		}
	    }

	    if (cache) {
		fprintf(fp, "   SET_ALL_VALID(%sData.valid);\n", ptr->name);
	    }
	    fprintf(fp, "   return(&%sData);\n", ptr->name);
	    fprintf(fp, "#else /* NOT_YET */\n");
	    fprintf(fp, "   return(NULL);\n");
	    fprintf(fp, "#endif /* NOT_YET */\n");
	}
    }
    fprintf(fp, "}\n\n");
}

/*----------------------------------------------------------------
 * first print the system independent get method for this family
 *----------------------------------------------------------------*/
void
write_v_get_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    char            buff[80];
    int             idx, i, j;
    int             check, ifdefs;
    struct _index_array *index_array, *temp_index;

    /* get index array */
    idx = get_index_array(ptr->name, &index_array);



    /* print block header */
    fprintf(fp, "/*%s\n", dashes);
    fprintf(fp, " * Retrieve data from the %s family.\n", ptr->name);
    if (!small_v_get) {
        fprintf(fp, " * This is performed in 3 steps:\n");
        fprintf(fp, " *\n");
        fprintf(fp, " *   1) Test the validity of the object instance.\n");
        fprintf(fp, " *   2) Retrieve the data.\n");
        fprintf(fp, " *   3) Build the variable binding (VarBind) that will be returned.\n");
    }
    fprintf(fp, " *%s*/\n", dashes);
    fprintf(fp, "VarBind *\n");
    if (ansi) {
        fprintf(fp, "%s_get(OID *incoming, ObjectInfo *object, int searchType,\n", ptr->name);
        for (i = 0; i < (int) strlen(ptr->name); i++) {
	    fprintf(fp, " ");
        }
        fprintf(fp, "     ContextInfo *contextInfo, int serialNum)\n");
    } else {
        fprintf(fp, "%s_get(incoming, object, searchType, contextInfo, serialNum)\n", ptr->name);
        fprintf(fp, "    OID             *incoming;\n");
        fprintf(fp, "    ObjectInfo      *object;\n");
        fprintf(fp, "    int             searchType;\n");
        fprintf(fp, "    ContextInfo     *contextInfo;\n");
        fprintf(fp, "    int             serialNum;\n");
    }
    fprintf(fp, "{\n");

    /*********************************************************************
     * If -small_v_get is used, just print this small routine and then
     * return.  Otherwise, we have to go on and print the big v_ routines.
     */
    if (small_v_get) {
        if (small_v_uses_global && !v_elimination_data) {
            fprintf(stderr, "postmosy: cannot generate small v_get functions without elimination data\n");
            exit(-1);
        }
        ifdefs = OpenIndexIfdefs(fp, ptr);
        if (ifdefs) {
            fprintf(fp, "    return NULL;\n");
            /* MatchIndexIfdefs(fp, ptr, MII_ELSE); */
            fprintf(fp, "#else /* all indices are supported */\n");
        }

            fprintf(fp, "    return (v_get(incoming, object, searchType, ");
            fprintf(fp, "contextInfo, serialNum,\n");
            if (small_v_uses_global) {
                fprintf(fp, "                  &minv_getinfo[%d]));\n",
                        ptr->getinfo);
            } else {
                fprintf(fp, "                  (SrGetInfoEntry *) &%sGetInfo));\n",
                        ptr->name);
            }
        if (ifdefs) {
            /* MatchIndexIfdefs(fp, ptr, MII_ENDIF); */
            fprintf(fp, "#endif /* all indices are supported */\n");
        }
        /* { */
	fprintf(fp, "}\n");
	fprintf(fp, "\n");
        return;
    }
    /*
     *
     *********************************************************************/

    if (medium_v) {
        if (idx != 0) {
	    fprintf(fp, "    int             instLength = incoming->length - object->oid.length;\n");
        }
    } else
    if (idx != -1) {
	fprintf(fp, "    int             instLength = incoming->length - object->oid.length;\n");
    }
    if (idx == 0) {
        fprintf(fp, "    int             arg = -1;\n");
    }
    else {
	fprintf(fp, "    int             arg = object->nominator;\n");
    }
    if (!offset && !medium_v) {
	fprintf(fp, "    void            *dp = NULL;\n");
    }

    if (!cache) {
	sprintf(buff, "%s_t", ptr->name);
	fprintf(fp, "    %-15s *data;\n", buff);
    }

    if (idx != 0) {
	if (!cache) {
	    if (idx > 0) {
		fprintf(fp, "    SR_UINT32   buffer[%d];\n", idx);
	    }
	    else {
		fprintf(fp, "    SR_UINT32   buffer[MAX_OID_SIZE];\n");
	    }
	    fprintf(fp, "    OID             inst;\n");
	}
	fprintf(fp, "    int             carry;\n");
	if (idx < 0) {
	    fprintf(fp, "    int             i;\n");
	}
        if (!medium_v) {
	temp_index = index_array;
	while (temp_index && temp_index->name) {
	    fprintf(fp, "    %-15s %s", temp_index->c_type, temp_index->name);
	    if ((strncmp(temp_index->c_type, "OctetString", 11) == 0) ||
		(strncmp(temp_index->c_type, "OID", 3) == 0)) {
		fprintf(fp, " = NULL");
	    }
	    fprintf(fp, ";\n");

	    if (idx < 0) {
		fprintf(fp, "    %-15s %s_offset;\n", "int", temp_index->name);
	    }
	    temp_index++;
	}
	} /* medium_v */
	if (idx < 0) {
	    fprintf(fp, "    %-15s index;\n", "int");
	    fprintf(fp, "    %-15s final_index;\n", "int");
	}
        fprintf(fp, "    %-15s ret;\n", "int");
    }
    if (medium_v) {
        fprintf(fp, "    short offset =\n");
        fprintf(fp, "          %sGetInfo.type_table[object->nominator].byteoffset;\n", ptr->name);
        if (idx != 0) {
            fprintf(fp, "    %s_t *alloc_data;\n", ptr->name);
            fprintf(fp, "    SR_INT32 *ipos = NULL;            /* instance index positions */\n");
            fprintf(fp, "    int ipos_size = 0;\n");
        }
    }

    if (late || instance) {
	fprintf(fp, "    int             regInstLen = 0;\n");
    }
    if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
	fprintf(fp, "    int             count;\n");
	fprintf(fp, "    int             which;\n");
    }
    fprintf(fp, "\n");

    if (cache) {
	fprintf(fp, "#ifdef %s_DISABLE_CACHE\n", ptr->name);

	sprintf(buff, "%s_t", ptr->name);
	fprintf(fp, "    %-15s *data = NULL;\n", buff);
	if (idx > 0) {
	    fprintf(fp, "    SR_UINT32   buffer[%d];\n", idx);
	}
	else if (idx < 0) {
	    fprintf(fp, "    SR_UINT32   buffer[MAX_OID_SIZE];\n");
	}
	if (idx != 0) {
	    fprintf(fp, "    OID             inst;\n");
        }

	fprintf(fp, "#else /* %s_DISABLE_CACHE */\n", ptr->name);

	sprintf(buff, "%s_t", ptr->name);
	fprintf(fp, "    static %-15s *data;\n", buff);
	if (idx > 0) {
	    fprintf(fp, "    static SR_UINT32   buffer[%d];\n", idx);
	}
	else if (idx < 0) {
	    fprintf(fp, "    static SR_UINT32   buffer[MAX_OID_SIZE];\n");
	}
	if (idx != 0) {
	    fprintf(fp, "    static OID             inst;\n");
        }
	fprintf(fp, "    static int             last_serialNum;\n");
	fprintf(fp, "    static OID             *last_incoming = NULL;\n");
	if (check_context) {
	    fprintf(fp, "#endif /* %s_DISABLE_CACHE */\n", ptr->name);
	    fprintf(fp, "\n");
	    fprintf(fp, "    /* check whether the agent supports this context */\n");
	    fprintf(fp, "    if(k_%s_context_support(contextInfo) == 0) {\n", ptr->name);
	    fprintf(fp, "        return((VarBind *) NULL);\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "#ifndef %s_DISABLE_CACHE\n", ptr->name);
	}
	fprintf(fp, "    /* if the cache is not valid */\n");
	fprintf(fp, "    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||\n");
	fprintf(fp, "         (CmpOIDInst(incoming, last_incoming, object->oid.length) != 0) ||\n");
	if (idx != 0) {
	    fprintf(fp, "         (!NEXT_ROW(data->valid)) ||\n");
	}
	fprintf(fp, "         (!VALID(object->nominator, data->valid)) ) {\n");

	fprintf(fp, "#endif /* %s_DISABLE_CACHE */\n", ptr->name);
    }

    if (late) {
        fprintf(fp, "    if (object->oidname && (object->oidname[0] == 1)) {\n");
        fprintf(fp, "        regInstLen = (int) object->oidname[1];\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "\n");
    }


    if (medium_v) {
        fprintf(fp, "    /*\n");
        fprintf(fp, "     * Process incoming instance\n");
        fprintf(fp, "     */\n");
        if (idx == 0) {  /* scalar */
            fprintf(fp, "    if ((CheckScalarInstance(incoming, object, searchType)) < 0) {\n");
	    fprintf(fp, "        arg = -1;\n");
	    fprintf(fp, "    } else {\n");
	    fprintf(fp, "        arg = object->nominator;\n");
	    fprintf(fp, "    }\n\n");
        } else {  /* table */
            fprintf(fp, "    if ((alloc_data = malloc (sizeof(%s_t))) == NULL) {\n", ptr->name);
            fprintf(fp, "        return NULL;\n");
            fprintf(fp, "    }\n");
            /* meset to 0 to clear the valid bits */
            fprintf(fp, "    memset(alloc_data, 0, sizeof(%s_t));\n", ptr->name);
            fprintf(fp, "\n");
            fprintf(fp, "    if((ExtractIncomingIndex(incoming, instLength, &%sGetInfo,\n", ptr->name);
            fprintf(fp, "                                searchType, object, alloc_data, &ipos,\n");
            fprintf(fp, "                                &ipos_size, &carry)) < 1) {\n");
            fprintf(fp, "            free(alloc_data);\n");
            fprintf(fp, "            return NULL;\n");
            fprintf(fp, "    }\n");
            fprintf(fp, "\n");
        }
    } else
    check_object_instance(fp, ptr);

    if (compute_nominator || (ptr->num_entries > MAX_COLUMNS)) {
        fprintf(fp, "    /*\n");
        fprintf(fp, "     * Find nominator from last_sid array\n");
        fprintf(fp, "     */\n");
        fprintf(fp, "    if (arg != -1) {\n");
        fprintf(fp, "        count = sizeof(%s_last_sid_array) / sizeof(SR_UINT32);\n", ptr->name);
        fprintf(fp, "        for (which = 0; which < count; which++) {\n");
        fprintf(fp, "            if (%s_last_sid_array[which] ==\n", ptr->name);
        fprintf(fp, "                     object->oid.oid_ptr[object->oid.length-1]) {\n");
        fprintf(fp, "                arg = which;\n");
        fprintf(fp, "                break;\n");
        fprintf(fp, "            }\n");
        fprintf(fp, "        }\n");
        fprintf(fp, "    }\n");
        fprintf(fp, "\n");
    }

    fprintf(fp, "    /*\n");
    fprintf(fp, "     * Retrieve the data from the kernel-specific routine.\n");
    fprintf(fp, "     */\n");


    fprintf(fp, "     if ( ");
    fprintf(fp, "(arg == -1) || ");
    fprintf(fp, "(data = k_%s_get(serialNum, contextInfo, ", ptr->name);
    fprintf(fp, "arg");
    if (idx != 0)
	fprintf(fp, ", searchType");

    temp_index = index_array;
    while (temp_index && temp_index->name) {
	fprintf(fp, ", ");
        if (medium_v) {
	    fprintf(fp, "alloc_data->");
        }
	fprintf(fp, "%s", temp_index->name);
	temp_index++;
    }
    fprintf(fp, ")");
    /* fprintf(fp, ") == (%s_t *) NULL) {\n", ptr->name); */
    fprintf(fp, ") == NULL) {\n");
    fprintf(fp, "        arg = -1;\n");
    fprintf(fp, "     }\n\n");
    if (idx != 0) {
	fprintf(fp, "     else {\n");
	fprintf(fp, "       /*\n");
	fprintf(fp, "        * Build instance information\n");
	fprintf(fp, "        */\n");
	fprintf(fp, "        inst.oid_ptr = buffer;\n");

	if (idx < 0) {
	    fprintf(fp, "        index = 0;\n");
	    check = 0;
	    temp_index = index_array;
	    while (temp_index && temp_index->name) {
		print_x_to_inst(fp, temp_index, &check);
		temp_index++;
		if ((temp_index && temp_index->name) &&
		    !((temp_index + 1) && (temp_index + 1)->name)) {
		    check = 0;
		}
	    }
	    fprintf(fp, "        inst.length = index;\n");
	    fprintf(fp, "     }\n");
	}
	else if (idx > 0) {
	    fprintf(fp, "        inst.length = %d;\n", idx);
	    i = 0;
	    temp_index = index_array;
	    while (temp_index && temp_index->name) {
		if (temp_index->length == 1) {
		    fprintf(fp, "        inst.oid_ptr[%d] = data->%s;\n",
			    i, temp_index->name);
		}
		else if (strcmp(temp_index->asn1_type, "IpAddress") == 0) {
		    for (j = 0; j < 4; j++) {
			fprintf(fp, "        inst.oid_ptr[%d] = ", j + i);
			fprintf(fp, "(data->%s >> %d) & 0xff;\n", temp_index->name, (3 - j) * 8);

		    }
		}
		else if (strcmp(temp_index->asn1_type, "NetworkAddress") == 0) {
		    for (j = 0; j < 5; j++) {
			fprintf(fp, "        inst.oid_ptr[%d] = ", j + i);
			fprintf(fp, "(unsigned long) (data->%s->octet_ptr[%d]);\n", temp_index->name, j);
		    }
		}
		else {
		    printf("stbget: write_v_routine does not know type: %s\n", temp_index->asn1_type);
		    exit(-1);
		}
		i += temp_index->length;
		temp_index++;
	    }
	    fprintf(fp, "     }\n");
	}
    }

    if (medium_v && idx != 0) {
	fprintf(fp, "\n");
	fprintf(fp, "     SrFreeFamily(alloc_data,\n");
	fprintf(fp, "        %sGetInfo.type_table,\n", ptr->name);
	fprintf(fp, "        %sGetInfo.highest_nominator,\n", ptr->name);
	fprintf(fp, "        %sGetInfo.valid_offset,\n", ptr->name);
	fprintf(fp, "        NULL);\n");
	fprintf(fp, "\n");
    } else
    if (idx != 0) {
	fprintf(fp, "\n");
	temp_index = index_array;
	while (temp_index && temp_index->name) {
	    print_string(fp, temp_index, FREE);
	    temp_index++;
	}
	fprintf(fp, "\n");
    }

    if (cache) {
	fprintf(fp, "#ifndef %s_DISABLE_CACHE\n", ptr->name);
	fprintf(fp, "     if (arg == -1) {\n");
	fprintf(fp, "         data = NULL;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "     else {\n");
	fprintf(fp, "         last_serialNum = serialNum;\n");
	fprintf(fp, "         if (last_incoming != NULL) {\n");
	fprintf(fp, "            FreeOID(last_incoming);\n");
	fprintf(fp, "         }\n");
	fprintf(fp, "         last_incoming = CloneOID(incoming);\n");
	fprintf(fp, "     }\n");
        /* { */
	fprintf(fp, "  }\n");
	if (idx == 0) {
	    fprintf(fp, "  else {\n");
	    fprintf(fp, "     arg = object->nominator;\n");
	    fprintf(fp, "  }\n");
	}
	fprintf(fp, "#endif /* %s_DISABLE_CACHE */\n", ptr->name);
	fprintf(fp, "\n");
    }


    fprintf(fp, "     /*\n");
    fprintf(fp, "      * Build the variable binding for the variable that will be returned.\n");
    fprintf(fp, "      */\n");


    if (medium_v) {
        if (idx == 0) { /* scalar */
            fprintf(fp, "    if (data) {\n");
            fprintf(fp, "        return SrBuildReturnVarBind(offset, data, object->oidtype, \n");
            fprintf(fp, "                                object, (OID *) &ZeroOid);\n");
            fprintf(fp, "    }\n");
            fprintf(fp, "    return (VarBind *) NULL;\n");
        } else {  /* table */
            fprintf(fp, "    if (data) {\n");
            fprintf(fp, "        return SrBuildReturnVarBind(offset, data, object->oidtype, \n");
            fprintf(fp, "                                    object, &inst);\n");
            fprintf(fp, "    }\n");
            fprintf(fp, "    return (VarBind *) NULL;\n");
        }
        /* { */
        fprintf(fp, "}\n\n");
        return;
    }

    if (!offset) {
	fprintf(fp, "\n     switch (arg) {\n\n");

	temp_ptr = ptr->next_family_entry;
	while (temp_ptr) {
            if (temp_ptr->valid) {
	        if (strcmp(temp_ptr->oid_access, NOT_ACCESSIBLE_STR) &&
	            strcmp(temp_ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {
    		    fprintf(fp, "#ifdef I_%s\n", temp_ptr->name);
	    	    fprintf(fp, "    case I_%s:\n", temp_ptr->name);
		    fprintf(fp, "       dp = (void *) (%s);\n", get_string(temp_ptr->oid_prim_type, temp_ptr->name, DP));
		    fprintf(fp, "       break;\n");
		    fprintf(fp, "#endif /* I_%s */\n\n", temp_ptr->name);
	        }
	    }
	    temp_ptr = temp_ptr->next_family_entry;
	}

	/*
	 * Add an entry for any indexes which are in another family but
	 * augment this family.
	 */

	/*
	 * if this group is augmented, print the items which augment the
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
			        if (strcmp(augments_list->oid_access, NOT_ACCESSIBLE_STR) &&
			            strcmp(augments_list->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {
			    	    fprintf(fp, "#ifdef I_%s\n",
					    augments_list->name);
				    fprintf(fp, "    case I_%s:\n",
					    augments_list->name);
				    fprintf(fp, "       dp = (void *) (%s);\n",
				        get_string(augments_list->oid_prim_type,
						   augments_list->name, DP));
				    fprintf(fp, "       break;\n");
				    fprintf(fp, "#endif /* I_%s */\n\n",
					    augments_list->name);
			         }
			    }
			    augments_list = augments_list->next_family_entry;
			}
		    }
		}
	    }
	}

	fprintf(fp, "    default:\n");
	fprintf(fp, "       return ((VarBind *) NULL);\n\n");
	fprintf(fp, "    }      /* switch */\n\n");
        if (late) {
	    fprintf(fp, "    object->oid.length -= regInstLen;\n");
        }
	if (idx != 0) {
	    fprintf(fp, "    return (MakeVarBind(object, &inst, dp));\n");
	}
	else {
	    fprintf(fp, "    return (MakeVarBind(object, &ZeroOid, dp));\n");
	}

    }
    else {			/* if (!offset) */
	fprintf(fp, "\n");
	if (idx != 0) {
	    fprintf(fp, "     return (BuildReturnVarBind(arg, data, object, &inst));\n");
	}
	else {
	    fprintf(fp, "     return (BuildReturnVarBind(arg, data, object, &ZeroOid));\n");
	}
    }

    fprintf(fp, "\n}\n");
    fprintf(fp, "\n");
    return;
}


void
check_object_instance(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    int i, j, k;
    int idx;
    int num_indices = 0;
    char *lateString;
    struct _index_array *index_array, *temp_index;

    idx = get_index_array(ptr->name, &index_array);

    if (idx == 0) {
	fprintf(fp, "    /*\n");
	fprintf(fp, "     * Check the object instance.\n     *\n");
	fprintf(fp, "     * An EXACT search requires that the instance be of ");
	fprintf(fp, "length 1 and\n");
	fprintf(fp, "     * the single instance element be 0.\n");
	fprintf(fp, "     *\n");
	fprintf(fp, "     * A NEXT search requires that the requested object does not\n");
	fprintf(fp, "     * lexicographically precede the current object type.\n");
	fprintf(fp, "     */\n\n");
	fprintf(fp, "    switch (searchType) {\n");
	fprintf(fp, "      case EXACT:\n");
	fprintf(fp, "        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {\n");
	fprintf(fp, "          arg = object->nominator;\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "        break;\n\n");
	fprintf(fp, "      case NEXT:\n");
	fprintf(fp, "        if (instLength <= 0) {\n");
	fprintf(fp, "          arg = object->nominator;\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "        break;\n\n");
	fprintf(fp, "    }\n\n");
    }
    else {


	if (idx > 0) {
	    fprintf(fp, "    /*\n");
	    fprintf(fp, "     * Check the object instance.\n     *\n");
	    fprintf(fp, "     * An EXACT search requires that the instance be of ");
	    fprintf(fp, "length %d \n", idx);
	    fprintf(fp, "     *\n");
	    fprintf(fp, "     * A NEXT search requires that the requested object does not\n");
	    fprintf(fp, "     * lexicographically precede the current object type.\n");
	    fprintf(fp, "     */\n\n");

	    fprintf(fp, "    if (searchType == EXACT) {\n");
	    fprintf(fp, "        if (instLength != %d) {\n", idx);
	    fprintf(fp, "          return((VarBind *) NULL);\n");
	    /* fprintf(fp, "          arg = -1;\n"); */
	    fprintf(fp, "        }\n");
	    fprintf(fp, "        carry = 0;\n");
	    fprintf(fp, "    } else {\n");
	    fprintf(fp, "        carry = 1;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}

        if (late || instance) {
	    fprintf(fp, "    if (regInstLen > 0 && searchType == NEXT) {\n");
	    fprintf(fp, "        int cmp = CmpNOID(incoming, &(object->oid), MIN(incoming->length, object->oid.length));\n");
	    fprintf(fp, "        if (cmp < 0) {\n");
	    fprintf(fp, "            carry = 0;\n");
	    fprintf(fp, "            incoming = &(object->oid);\n");
	    fprintf(fp, "         } else if (cmp > 0) {\n");
	    fprintf(fp, "            return (VarBind *) NULL;\n");
	    fprintf(fp, "         }\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
        }

	else if (idx < 0) {
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
	    fprintf(fp, "    if (searchType == EXACT) {\n");
	    fprintf(fp, "       if (final_index != incoming->length) {\n");
	    fprintf(fp, "          return((VarBind *) NULL);\n");
	    /* fprintf(fp, "          arg = -1;\n"); */
	    fprintf(fp, "        }\n");
	    fprintf(fp, "        carry = 0;\n");
	    fprintf(fp, "    } else {\n");
	    fprintf(fp, "        carry = 1;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}

        if (late || instance) {
	    lateString = " - regInstLen";
        } else {
	    lateString = "";
        }
	/*
	 * go to the end of the index array to print the last index first
	 */
	i = idx;
	for (temp_index = index_array, num_indices = 0; temp_index->name; 
             temp_index++, num_indices++);
        j = num_indices;
	while (j--) {
	    fprintf(fp, "    ret = ");
	    fprintf(fp, "InstTo");
	    print_string(fp, &index_array[j], INST_TO_X);
	    if (idx < 0) {
		fprintf(fp, "(incoming, %s_offset%s, &%s, searchType, &carry",
			index_array[j].name, lateString, index_array[j].name);
	    }
	    else {
		i -= index_array[j].length;
		fprintf(fp, "(incoming, %d + object->oid.length%s, &%s, searchType, &carry", i, lateString, index_array[j].name);
	    }
	    if (index_array[j].IndexType > 0) {
		fprintf(fp, ", %d", index_array[j].IndexType);
	    }
	    fprintf(fp, ");\n");

	    fprintf(fp, "    if (ret < 0 ) {\n");
	    /* fprintf(fp, "       return (NULL);\n"); */
	    fprintf(fp, "        arg = -1;\n");

            /* for all but the last index, check to see if InstToXX()
             * returned indication of overflow requiring forward indices
             * to be zero-ed out
             */
            if (j < num_indices -1) { 
	       fprintf(fp, "    } else if (ret == 2) { \n");
	       fprintf(fp, "        /* Index value overflow, zero-out any forward indices */\n");
               k = num_indices - 1;
               while (k > j) {
	          print_string(fp, &index_array[k], FREE);
	          fprintf(fp, "        %s = ", index_array[k].name);
                  print_string(fp, &index_array[k], ZERO_ASSIGNMENT); 
                  k--;
               }
            }
	    fprintf(fp, "    }\n");

	    if (one_based) {
		if (index_array[j].length == 1) {
		    fprintf(fp, "    if (searchType == NEXT)");
		    fprintf(fp, " %s = MAX(1, %s);\n", index_array[j].name, index_array[j].name);
		}
	    }
	    else {
		/*
		 * If a size range is given in the MIB, we never want to call
		 * the k_ routine with a value that is less than the lowest
		 * legal value
		 */
		if (index_array[j].length == 1) {
		    print_index_size_check(fp, index_array[j].name);
		}
	    }
	    fprintf(fp, "\n");
	}
	fprintf(fp, "    if (carry) {\n");
	fprintf(fp, "       arg = -1;\n");
	fprintf(fp, "    }\n");
	fprintf(fp, "\n");
    }
}
