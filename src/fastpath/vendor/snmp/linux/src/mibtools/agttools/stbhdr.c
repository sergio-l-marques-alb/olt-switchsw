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

void
print_converter_entry(fp, temp_ptr)
    FILE           *fp;
    struct OID_INFO *temp_ptr;
{
    char            temp[128];

    strcpy(temp, temp_ptr->oid_prim_type);

    /*
     * try to figure out what variable type to use
     */

    /* first, check some common textual conventions */
    if (strcmp(temp_ptr->oid_type, "DisplayString") == 0) {
        fprintf(fp, "    { ConvToken_textOctetString, NULL },    /* %s */\n", temp_ptr->name);
        return;
    }
    if (strcmp(temp_ptr->oid_type, "OwnerString") == 0) {
        fprintf(fp, "    { ConvToken_textOctetString, NULL },    /* %s */\n", temp_ptr->name);
        return;
    }
    if (strcmp(temp_ptr->oid_type, "SnmpAdminString") == 0) {
        fprintf(fp, "    { ConvToken_textOctetString, NULL },    /* %s */\n", temp_ptr->name);
        return;
    }
    if (strcmp(temp_ptr->oid_type, "SnmpTagValue") == 0) {
        fprintf(fp, "    { ConvToken_textOctetString, NULL },    /* %s */\n", temp_ptr->name);
        return;
    }

    /* if it is not one of the above textual conventions, then check types */
    if ((strcmp(temp, "INTEGER") == 0) ||
       ( strcmp(temp, "Integer32") == 0)) {
        fprintf(fp, "    { ConvToken_integer, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "PhysAddress") == 0 ||
               strcmp(temp, "Bits") == 0 ||
               strcmp(temp, "Opaque") == 0 ||
	       strcmp(temp, "OctetString") == 0) {
        fprintf(fp, "    { ConvToken_octetString, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "ObjectID") == 0) {
        fprintf(fp, "    { ConvToken_OID, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "IpAddress") == 0) {
        fprintf(fp, "    { ConvToken_unsigned, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "NetworkAddress") == 0) {
        fprintf(fp, "    { ConvToken_octetString, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "Counter") == 0 ||
               strcmp(temp, "Gauge") == 0 ||
               strcmp(temp, "TimeTicks") == 0 ||
               strcmp(temp, "UInteger32") == 0) {
        fprintf(fp, "    { ConvToken_unsigned, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "Counter32") == 0 ||
               strcmp(temp, "Gauge32") == 0 ||
               strcmp(temp, "Unsigned32") == 0 ||
               strcmp(temp, "UInteger32") == 0) {
        fprintf(fp, "    { ConvToken_unsigned, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "NsapAddress") == 0) {
        fprintf(fp, "    { ConvToken_textOctetString, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "Counter64") == 0) {
        fprintf(fp, "    { ConvToken_counter64, NULL },    /* %s */\n", temp_ptr->name);
    } else if (strcmp(temp, "Integer32") == 0) {
        fprintf(fp, "    { ConvToken_integer, NULL },    /* %s */\n", temp_ptr->name);
    } else {
	/* unknown type */
	fprintf(stderr, "%s: print_converter_entry: unknown type %s\n",
		prog_name, temp);
	exit(-1);
    }
}

void
print_comments_entry(fp, temp_ptr)
    FILE           *fp;
    struct OID_INFO *temp_ptr;
{
    char            temp[128];

    strcpy(temp, temp_ptr->oid_prim_type);

    /*
     * try to figure out what variable type to use
     */

    /* first, check some common textual conventions */
    if (strcmp(temp_ptr->oid_type, "DisplayString") == 0) {
        fprintf(fp, "text");
        return;
    }
    if (strcmp(temp_ptr->oid_type, "OwnerString") == 0) {
        fprintf(fp, "text");
        return;
    }
    if (strcmp(temp_ptr->oid_type, "SnmpAdminString") == 0) {
        fprintf(fp, "text");
        return;
    }
    if (strcmp(temp_ptr->oid_type, "SnmpTagValue") == 0) {
        fprintf(fp, "text");
        return;
    }

    /* if it is not one of the above textual conventions, then check types */
    if ((strcmp(temp, "INTEGER") == 0) ||
       ( strcmp(temp, "Integer32") == 0)) {
        fprintf(fp, "integer");
    } else if (strcmp(temp, "PhysAddress") == 0 ||
	       strcmp(temp, "Opaque") == 0 ||
	       strcmp(temp, "OctetString") == 0) {
        fprintf(fp, "octetString");
    } else if (strcmp(temp, "ObjectID") == 0) {
        fprintf(fp, "ObjectID");
    } else if (strcmp(temp, "IpAddress") == 0) {
        fprintf(fp, "ipAddress");
    } else if (strcmp(temp, "NetworkAddress") == 0) {
        fprintf(fp, "networkAddress");
    } else if (strcmp(temp, "Counter") == 0 ||
               strcmp(temp, "Gauge") == 0 ||
               strcmp(temp, "TimeTicks") == 0 ||
               strcmp(temp, "UInteger32") == 0) {
        fprintf(fp, "unsigned");
    } else if (strcmp(temp, "Counter32") == 0 ||
               strcmp(temp, "Gauge32") == 0 ||
               strcmp(temp, "Unsigned32") == 0 ||
               strcmp(temp, "UInteger32") == 0) {
        fprintf(fp, "unsigned");
    } else if (strcmp(temp, "NsapAddress") == 0) {
        fprintf(fp, "nsapAddress");
    } else if (strcmp(temp, "Counter64") == 0) {
        fprintf(fp, "counter64");
    } else if (strcmp(temp, "Integer32") == 0) {
        fprintf(fp, "integer");
    } else if (strcmp(temp, "Bits") == 0) {
        fprintf(fp, "bits");
    } else {
	/* unknown type */
	fprintf(stderr, "%s: print_comments_entry: unknown type %s\n",
		prog_name, temp);
	exit(-1);
    }
}

void
print_type_record(fp, temp_ptr, index_array, parent, local)
    FILE           *fp;
    struct OID_INFO *temp_ptr;
    struct _index_array *index_array;
    char *parent;
    int  local;
{
    struct _index_array *temp_index;
    char            buff[64];
    int             count;

    map_variable_type_for_family(temp_ptr->oid_prim_type, buff);
    fprintf(fp, "    { %s, ", buff);

    if (!local) {   /* external index not accessible */
	strcpy(buff, "SR_NOT_ACCESSIBLE");
    }
    else if (strcmp(temp_ptr->oid_access, READ_ONLY_STR) == 0) {
	strcpy(buff, "SR_READ_ONLY");
    }
    else if (strcmp(temp_ptr->oid_access, READ_WRITE_STR) == 0) {
	strcpy(buff, "SR_READ_WRITE");
    }
    else if (strcmp(temp_ptr->oid_access, WRITE_ONLY_STR) == 0) {
	strcpy(buff, "SR_WRITE_ONLY");
    }
    else if (strcmp(temp_ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR) == 0) {
	strcpy(buff, "SR_ACCESSIBLE_FOR_NOTIFY");
    }
    else if (strcmp(temp_ptr->oid_access, NOT_ACCESSIBLE_STR) == 0) {
	strcpy(buff, "SR_NOT_ACCESSIBLE");

    }
    else if (strcmp(temp_ptr->oid_access, READ_CREATE_STR) == 0) {
	strcpy(buff, "SR_READ_CREATE");
    }
    else {
	fprintf(stderr,
		"stbhdr: output_file: unknown access type %s for %s\n",
		temp_ptr->oid_access, temp_ptr->name);
	exit(-1);

    }
    fprintf(fp, "%s, ", buff);

    fprintf(fp, "offsetof(%s_t, %s), ", parent, temp_ptr->name);

    /* if it's an index, print its location in idx */
    for (temp_index = index_array, count = 0; temp_index && temp_index->name; temp_index++, count++) {
	if (strcmp(temp_index->name, temp_ptr->name) == 0) {
	    break;
	}
    }
    if (temp_index == NULL || temp_index->name == NULL) {
	count = (-1);
    }
    fprintf(fp, "%d },\n", count);
}


/* this routine prints parser data structures for one scalar object */
void
print_scalar_parser_code(FILE *fp, struct OID_INFO *ptr)
{
    char            buff[64];

    if (!(ptr->valid)) return;

    /* ------------ */
    /* STEP 1: create a TypeTable entry */
    /* ------------ */

    if (!type_table) {
	fprintf(fp, "/*\n");
        fprintf(fp, " * The %sTypeTable array would have been generated\n", ptr->name);
        fprintf(fp, " * here, but the -no_type_table argument was specified.\n");
        fprintf(fp, " */\n");
        fprintf(fp, "extern const SnmpType %sTypeTable[];\n\n", ptr->name);
        return;
    } else {
        fprintf(fp, "const SnmpType %sTypeTable[] = {\n", ptr->name);
        map_variable_type_for_family(ptr->oid_prim_type, buff);
        fprintf(fp, "    { %s, ", buff);
        if (strcmp(ptr->oid_access, READ_ONLY_STR) == 0) {
            strcpy(buff, "SR_READ_ONLY");
        }
        else if (strcmp(ptr->oid_access, READ_WRITE_STR) == 0) {
            strcpy(buff, "SR_READ_WRITE");
        }
        else if (strcmp(ptr->oid_access, WRITE_ONLY_STR) == 0) {
            strcpy(buff, "SR_WRITE_ONLY");
        }
        else if (strcmp(ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR) == 0) {
            strcpy(buff, "SR_ACCESSIBLE_FOR_NOTIFY");
        }
        else if (strcmp(ptr->oid_access, NOT_ACCESSIBLE_STR) == 0) {
            strcpy(buff, "SR_NOT_ACCESSIBLE");

        }
        else if (strcmp(ptr->oid_access, READ_CREATE_STR) == 0) {
            strcpy(buff, "SR_READ_CREATE");
        }
        else {
            fprintf(stderr,
                    "stbhdr: output_file: unknown access type %s for %s\n",
                    ptr->oid_access, ptr->name);
            exit(-1);
        }
        fprintf(fp, "%s, ", buff);
        fprintf(fp, "0, ");         /* prints offsetof() in table code */
        fprintf(fp, "-1 },\n");     /* prints idx position in table code */
        fprintf(fp, "    { -1, -1, (unsigned short) -1, -1 }\n");
        fprintf(fp, "};\n\n");
    }


    /* ------------ */
    /* STEP 2: create a PARSER_CONVERT array */
    /* ------------ */

    fprintf(fp, "#ifdef SR_CONFIG_FP\n");
    fprintf(fp, "#ifdef I_%s\n", ptr->name);
    fprintf(fp, "const PARSER_CONVERTER %sConverter[] = {\n", ptr->name);
    print_converter_entry(fp, ptr);
    fprintf(fp, "    { NULL, NULL }\n};\n");


    /* ------------ */
    /* STEP 3: create a PARSER_RECORD_TYPE structure */
    /* ------------ */

    fprintf(fp, "const PARSER_RECORD_TYPE %sRecord = {\n", ptr->name);
    fprintf(fp, "    PARSER_SCALAR,\n");
    fprintf(fp, "    \"%s \",\n", ptr->name);
    fprintf(fp, "    %sConverter,\n", ptr->name);
    fprintf(fp, "    %sTypeTable,\n", ptr->name);
    fprintf(fp, "    NULL,\n"); /* pointer to table, NULL for scalars */
    fprintf(fp, "    &%sData.%s,\n", ptr->parent_ptr->name, ptr->name);
    fprintf(fp, "    -1,\n");
    fprintf(fp, "    -1,\n");
    fprintf(fp, "    -1\n");
    fprintf(fp, "#ifndef SR_NO_COMMENTS_IN_CONFIG\n");
    if (comments) {
        fprintf(fp, ",\"# Entry type: %s", ptr->name);
        fprintf(fp, "\\\n\\n# Entry format:");
        fprintf(fp, "\\\n\\n#        %-30s (", ptr->name);
        print_comments_entry(fp, ptr);
        fprintf(fp, ")");
        fprintf(fp, "\"\n");
    } else {
        fprintf(fp, "    , NULL\n");
    }
    fprintf(fp, "#endif /* SR_NO_COMMENTS_IN_CONFIG */\n");
    fprintf(fp, "};\n");
    fprintf(fp, "#endif /* I_%s */\n", ptr->name);
    fprintf(fp, "#endif /* SR_CONFIG_FP */\n\n");
}


/*
 * The following function generates TypeTable structures for entire
 * scalar families, as needed by the v_ elimination code.
 */
void
write_scalar_family_TypeTable(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    struct _index_array *index_array;

    if (!(ptr->valid)) return;

    /* stop now if this family is tabular */
    if (get_index_array(ptr->name, &index_array) != 0) {
        return;
    }

    if (!type_table) {
	fprintf(fp, "/*\n");
        fprintf(fp, " * The %sTypeTable array would have been generated\n", ptr->name);
        fprintf(fp, " * here, but the -no_type_table argument was specified.\n");
        fprintf(fp, " */\n");
        fprintf(fp, "extern const SnmpType %sTypeTable[];\n\n", ptr->name);
        return;
    }

    fprintf(fp, "const SnmpType %sTypeTable[] = {\n", ptr->name);

    /* print a descriptor for every element in the table */
    for (temp_ptr = ptr->next_family_entry; temp_ptr;
         temp_ptr = temp_ptr->next_family_entry) {
        if (temp_ptr->valid) {
            print_type_record(fp, temp_ptr, NULL, ptr->name, 1);
	}
    }
    if (userpart && parser) {
        fprintf(fp, "    /* Additional entries for user defines go here */\n");
    }
    fprintf(fp, "    { -1, -1, (unsigned short) -1, -1 }\n");
    fprintf(fp, "};\n\n");
}


void
write_tabular_family_TypeTable(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    struct OID_INFO *augments_list;
    struct _index_array *index_array, *temp_index;
    int found;

    if (!(ptr->valid)) return;

    /* stop now if this family is scalar */
    if (get_index_array(ptr->name, &index_array) == 0) {
        return;
    }

    if (!type_table) {
	fprintf(fp, "/*\n");
        fprintf(fp, " * The %sTypeTable array would have been generated\n", ptr->name);
        fprintf(fp, " * here, but the -no_type_table argument was specified.\n");
        fprintf(fp, " */\n");
        fprintf(fp, "extern const SnmpType %sTypeTable[];\n\n", ptr->name);
        return;
    }

    /*
     * The code below was once a part of print_family_descriptor().
     * It was moved here, unchanged except for indendation.
     */

    if (index_array) {
        fprintf(fp, "const SnmpType %sTypeTable[] = {\n", ptr->name); /* } */

        /* print a descriptor for every element in the table */
        for (temp_ptr = ptr->next_family_entry; temp_ptr;
             temp_ptr = temp_ptr->next_family_entry) {
            if (temp_ptr->valid) {
                print_type_record(fp, temp_ptr, index_array, ptr->name, 1);
	    }
        }
        /*
         * Add an entry in this table for the
         * entries in the INDEX clause if those
         * entries are not already in the structure
         * (i.e. if the index is in another family).
         */

        /* for each index for this table */
        for (temp_index = index_array; temp_index->name; temp_index++) {

            /*
             * first, see if this entry
             * is already in the
             * structure (i.e. if is in
             * this table).
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
        }
     }

    /*
     * if this group is augmented, print descriptors for the items which
     * augment the group
     */
    if (ptr->augmented) {
            for (temp_ptr = sorted_root; temp_ptr;
             temp_ptr = temp_ptr->next_sorted) {
            if (temp_ptr->augments
                && strcmp(temp_ptr->augments, ptr->name) == 0) {
                augments_list = temp_ptr->next_family_entry;
                while (augments_list) {
                    if (augments_list->valid) {
                        print_type_record(fp, augments_list, index_array, ptr->name, 1);
                    }
                    augments_list = augments_list->next_family_entry;
                }
            }
        }
    }

    if (index_array) {
        /* for each index for this table */
        for (temp_index = index_array; temp_index->name; temp_index++) {
            /*
             * if is is not in the
             * structure, then put it in
             * now
             */
            if (!found) {
                for (temp_ptr = sorted_root; temp_ptr; temp_ptr=temp_ptr->next_sorted) {
                    if (strcmp(temp_index->name, temp_ptr->name) == 0) {
                        print_type_record(fp, temp_ptr, index_array, ptr->name,0);
                        break;
                    }
                }
            }
        }        /* for(temp_index... */
    }        /* if(index_array) */

    if (row_status && parser && row_status_in_family(ptr)) {
        fprintf(fp, "    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(%s_t, RowStatusTimerId), -1 },\n", ptr->name);
    }

    if (userpart && parser) {
        fprintf(fp, "    /* Additional entries for user defines go here */\n");
    }

    fprintf(fp, "    { -1, -1, (unsigned short) -1, -1 }\n");

    /* { */
    fprintf(fp, "};\n\n");
}


void
write_any_family_TypeTable(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    /*
     * Both of the functions below check to make sure they are the
     * correct one to be called.  The write_scalar_family_TypeTable()
     * function to return immediately if you pass in a tabular family.
     * The write_tabular_family_TypeTable() will return immediately
     * if you pass in a scalar family.  So we just call both functions
     * here, and the correct <family>TypeTable array will be generated.
     */
    write_scalar_family_TypeTable(fp, ptr);
    write_tabular_family_TypeTable(fp, ptr);
}


void
write_tabular_family_TypeTable_comment(fp, ptr, base)
    FILE           *fp;
    struct OID_INFO *ptr;
    char           *base;
{
    struct _index_array *index_array;

    /* stop now if this family is scalar */
    if (get_index_array(ptr->name, &index_array) == 0) {
        return;
    }

    if (base == NULL) {
        base = "<base>";
    }
    fprintf(fp, "/*\n");
    fprintf(fp, " * The %sTypeTable array should be located in the\n", ptr->name);
    fprintf(fp, " * k_%s.stb file, because one of the following command-line\n", base);
    fprintf(fp, " * arguments was used: -row_status, -search_table, or -parser.\n");
    fprintf(fp, " */\n");
    fprintf(fp, "extern const SnmpType %sTypeTable[];\n\n", ptr->name);
}


/* this routine prints a data structure describing a table entry */
/* (also called for scalar groups if the -parser option is used) */
void
print_family_descriptor(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *temp_ptr;
    struct OID_INFO *augments_list;

    struct _index_array *index_array, *temp_index;
    int             count;
    char           *table_type = "SnmpV2Table";
    int idx;
    int found;

    char *struct_name = ptr->parent_ptr->name;
 

    idx = get_index_array(ptr->name, &index_array);

    /* generate an index table structure */
    for (temp_index = index_array, count = 0;
         temp_index && temp_index->name; temp_index++, count++) {
	 /*
	  * The purpose of this loop is apperantly to
	  * increment the counter 'count'. -dss
	  */
	 ;
    }

    if (idx == 0) {
        fprintf(fp, "/* global data describing the %s family */\n",
    	        ptr->name);

        if (parser) {
            fprintf(fp, "%s_t %sData;\n\n", ptr->name, ptr->name);
            /* print a parser code for every scalar in the family */
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (temp_ptr->valid) {
                    print_scalar_parser_code(fp, temp_ptr);
                }
            }
            if (userpart) {
                fprintf(fp, "#ifdef U_%s\n", ptr->name);
                fprintf(fp, "    /*\n");
                fprintf(fp, "     * Additional SnmpType, PARSER_CONVERTER, and\n");
                fprintf(fp, "     * PARSER_RECORD_TYPE structure definitions for\n");
                fprintf(fp, "     * U_%s go here.\n", ptr->name);
                fprintf(fp, "     */\n");
                fprintf(fp, "#endif /* U_%s */\n", ptr->name);
            }
        } else {
            fprintf(fp, "%s_t *%sData;\n", ptr->name, ptr->name);
        }
        fprintf(fp, "\n");
    } else if ((row_status_in_family(ptr) != NULL) || search_table || parser) {
        fprintf(fp, "/* global data describing the %s entries */\n",
    	        ptr->parent_ptr->name);

        fprintf(fp, "Index_t %sIndex[] = {\n", ptr->name);
        for (temp_index = index_array, count = 0;
                  (temp_index && temp_index->name); temp_index++, count++) {
            fprintf(fp, "    { ");
            fprintf(fp, "offsetof(%s_t, %s), ", ptr->name, temp_index->name);
            fprintf(fp, " T_");
            print_string(fp, temp_index, VINDEX_CAT);
            fprintf(fp, "}");
            if ((temp_index + 1)->name) {
                fprintf(fp, ",");
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "};\n");
        fprintf(fp, "\n");

        fprintf(fp, "%s %s = {\n", table_type, struct_name);
        fprintf(fp, "    NULL,\n");
        fprintf(fp, "    %sIndex,\n", ptr->name);
        fprintf(fp, "    0,\n");
        fprintf(fp, "    %d,\n", count);
        fprintf(fp, "    sizeof(%s_t)\n", ptr->name);
        fprintf(fp, "};\n");
        fprintf(fp, "\n");

        write_tabular_family_TypeTable(fp, ptr);

        if (parser) {
            /*
             * Generate format string.
             */
            fprintf(fp, "#ifdef SR_CONFIG_FP\n");
            fprintf(fp, "const PARSER_CONVERTER %sConverters[] = {\n", 
		    ptr->name);
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
	         temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
	        print_converter_entry(fp, temp_ptr);
            }
        /*
         * Add an entry in this table for the
         * entries in the INDEX clause if those
         * entries are not already in the structure
         * (i.e. if the index is in another family).
         */

        /*
        * get an array containing the indicies of
        * this family (from the INDEX clause)
        */
        get_index_array(ptr->name, &index_array);

        /*
         * if this is not a table, then it has no
         * INDEX clause, and index_array will be
         * NULL.
         */
        if (index_array) {
		/* for each index for this table */
		for (temp_index = index_array; temp_index->name; temp_index++) {

			/*
			 * first, see if this entry
			 * is already in the
			 * structure (i.e. if is in
			 * this table).
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

			/*
			 * if is is not in the
			 * structure, then put it in
			 * now
			 */
			if (!found) {
			    for (temp_ptr = sorted_root; temp_ptr; temp_ptr=temp_ptr->next_sorted) {
			        if (strcmp(temp_index->name, temp_ptr->name) == 0) {
			            print_converter_entry(fp, temp_ptr);
				    break;
                                }
			    }
			}
		}	/* for(temp_index... */
        }	/* if(index_array) */

            /*
             * if this group is augmented, generate a converter
             * for the items which augment the group
             */
            if (ptr->augmented) {
                    for (temp_ptr = sorted_root; temp_ptr;
                     temp_ptr = temp_ptr->next_sorted) {
                    if (temp_ptr->augments
                        && strcmp(temp_ptr->augments, ptr->name) == 0) {
                        augments_list = temp_ptr->next_family_entry;
                        while (augments_list) {
                            if (augments_list->valid &&
                                strcmp(augments_list->oid_type, "Aggregate") != 0) {
                                print_converter_entry(fp, augments_list);
                            }
                            augments_list = augments_list->next_family_entry;
                        }
                    }
                }
            }

            if (row_status && row_status_in_family(ptr)) {
                fprintf(fp, "    { ConvToken_integer, \"-1\" },\n");
            }
            if (userpart && parser) {
                fprintf(fp, "    /* Additional entries for user defines go here */\n");
            }
            fprintf(fp, "    { NULL, NULL }\n};\n");

            /*
             * Generate parser record.
             */
            fprintf(fp, "const PARSER_RECORD_TYPE %sRecord = {\n", ptr->name);
            fprintf(fp, "    PARSER_TABLE,\n");
            fprintf(fp, "    \"%s \",\n", ptr->name);
            fprintf(fp, "    %sConverters,\n", ptr->name);
            fprintf(fp, "    %sTypeTable,\n", ptr->name);
            fprintf(fp, "    &%s,\n", struct_name);
            fprintf(fp, "    NULL,\n");
            fprintf(fp, "    -1,\n");
            fprintf(fp, "    -1,\n");
            fprintf(fp, "    -1\n");
            fprintf(fp, "#ifndef SR_NO_COMMENTS_IN_CONFIG\n");
            if (comments) {
                fprintf(fp, ",\"# Entry type: %s", ptr->name);
                fprintf(fp, "\\n\\\n# Entry format:");
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                                temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid) {
                        fprintf(fp, "\\n\\\n#        %-30s (", temp_ptr->name);
                        print_comments_entry(fp, temp_ptr);
                        fprintf(fp, ")");
                    }
                }
                /*
                 * Add an entry in this table for the
                 * entries in the INDEX clause if those
                 * entries are not already in the structure
                 * (i.e. if the index is in another family).
                 */
        
                /*
                * get an array containing the indicies of
                * this family (from the INDEX clause)
                */
                get_index_array(ptr->name, &index_array);

                /*
                 * if this is not a table, then it has no
                 * INDEX clause, and index_array will be
                 * NULL.
                 */
                if (index_array) {
        		/* for each index for this table */
        		for (temp_index = index_array; temp_index->name; 
                                                       temp_index++) {
        			/*
        			 * first, see if this entry
        			 * is already in the
        			 * structure (i.e. if is in
        			 * this table).
        			 */
        
        			found = 0;
        			temp_ptr = ptr->next_family_entry;
        			while (temp_ptr) {
        				if (strcmp(temp_ptr->name, 
                                                   temp_index->name) == 0) {
        					found = 1;
                                                break;
        				}
        				temp_ptr = temp_ptr->next_family_entry;
        			}
        
        			/*
        			 * if is is not in the
        			 * structure, then put it in
        			 * now
        			 */
        			if (!found) {
        			    for (temp_ptr = sorted_root; temp_ptr; 
                                             temp_ptr=temp_ptr->next_sorted) {
        			        if (strcmp(temp_index->name,  
                                                       temp_ptr->name) == 0) {
                                            fprintf(fp, "\\n\\\n#        %-30s (", temp_ptr->name);
                                            print_comments_entry(fp, temp_ptr);
                                            fprintf(fp, ")");
        				    break;
                                        }
        			    }
        			}
        		}	/* for(temp_index... */
                }	/* if(index_array) */

                fprintf(fp, "\"\n");
            } else {
                fprintf(fp, "    , NULL\n");
            }
            fprintf(fp, "#endif /* SR_NO_COMMENTS_IN_CONFIG */\n");
            fprintf(fp, "};\n");
            fprintf(fp, "#endif /* SR_CONFIG_FP */\n");
        }

        fprintf(fp, "\n");
    }
}

/* write header comments and code for the k_ stub file */
void
write_k_stb_headers(fp, base)
    FILE           *fp;
    char           *base;
{
    struct OID_INFO *ptr;
    struct _index_array *index_array;
    int idx;

    /* print the copyright header */
    print_copyright(fp);

    if (!no_cvs_keywords) {
      print_cvs_keywords(fp, C_FILE);
    }

    /* fprintf(fp, "\n\n"); */
    fprintf(fp, "#include \"sr_conf.h\"\n\n");
    fprintf(fp, "#ifdef HAVE_STDIO_H\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#endif /* HAVE_STDIO_H */\n");
    fprintf(fp, "#ifdef HAVE_STDLIB_H\n");
    fprintf(fp, "#include <stdlib.h>\n");
    fprintf(fp, "#endif /* HAVE_STDLIB_H */\n");
    fprintf(fp, "#ifdef HAVE_STRING_H\n");
    fprintf(fp, "#include <string.h>\n");
    fprintf(fp, "#endif /* HAVE_STRING_H */\n");
    fprintf(fp, "#ifdef HAVE_MEMORY_H\n");
    fprintf(fp, "#include <memory.h>\n");
    fprintf(fp, "#endif /* HAVE_MEMORY_H */\n");
    fprintf(fp, "#ifdef HAVE_STDDEF_H\n");
    fprintf(fp, "#include <stddef.h>\n");
    fprintf(fp, "#endif /* HAVE_STDDEF_H */\n");
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"sr_trans.h\"\n");
    fprintf(fp, "#include \"context.h\"\n");
    fprintf(fp, "#include \"method.h\"\n");
    fprintf(fp, "#include \"makevb.h\"\n");

    /* if we're using row status, include an appropriate header file */
    if (row_status_in_file() != 0) {
	fprintf(fp, "#include \"lookup.h\"\n");
	fprintf(fp, "#include \"v2table.h\"\n");
	fprintf(fp, "#include \"tmq.h\"\n");
	fprintf(fp, "#include \"rowstatf.h\"\n");
    } else if (parser || search_table || v_elimination_data) {
	fprintf(fp, "#include \"lookup.h\"\n");
	fprintf(fp, "#include \"v2table.h\"\n");
    }

    if (parser) {
	fprintf(fp, "#include \"sr_cfg.h\"\n");
	fprintf(fp, "#include \"scan.h\"\n");
    }

    if (v_elimination_data) {
	fprintf(fp, "#include \"min_v.h\"\n");
    }

    fprintf(fp, "#include \"mibout.h\"\n");

    fprintf(fp, "\n");
    if (table_lookup) {
	fprintf(fp, "#include \"lookup.h\"\n\n");
    }
    fprintf(fp, "\n");

    if (parser) {
        /* create a flag to indicate when changes should be recorded */
        fprintf(fp, "int %sWriteConfigFileFlag = 0;\n\n", base);
        /* print extern reference to support scalar initialization */
        fprintf(fp, "extern const OID nullOID;\n\n");
    }

    /* print header data structures if necessary */
    for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	 ptr = ptr->next_sorted) {

        idx = get_index_array(ptr->name, &index_array);

        /*
         * The following if-statement also appears in the function
         * write_v_agent_files().  If you change the conditional logic
         * here, change it in the other function also.
         */
	/* if this family uses row status print a header structure */
	if ((row_status_in_family(ptr) != NULL) || search_table || parser) {
	    print_family_descriptor(fp, ptr);
            if (idx != 0) {
	        fprintf(fp, "void Delete%s\n", ptr->name);
	        fprintf(fp, "    SR_PROTOTYPE((int index));\n");
	        fprintf(fp, "extern void %s_free\n", ptr->name);
                fprintf(fp, "    SR_PROTOTYPE((%s_t *data));\n", ptr->name);
	        fprintf(fp, "\n");
            }
	}
	else if (table_lookup && (idx != 0)) {
	    fprintf(fp, "/* keep track of %s entries in use */\n", ptr->name);
	    fprintf(fp, "%s_t **%s;\n", ptr->name, ptr->name);
	    fprintf(fp, "int %s_num_used = 0;\n", ptr->name);
	    fprintf(fp, "\n");
	}
    }
}

/* write header comments and code for the v_ stub file */
void
write_v_stb_headers(fp)
    FILE           *fp;
{
    struct OID_INFO *ptr;
    int idx;
    struct _index_array *index_array;

    print_copyright(fp);

    if (!no_cvs_keywords) {
      print_cvs_keywords(fp, C_FILE);
    }

    fprintf(fp, "#include \"sr_conf.h\"\n\n");
    fprintf(fp, "#ifdef HAVE_STDIO_H\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#endif /* HAVE_STDIO_H */\n\n");
    fprintf(fp, "#ifdef HAVE_STDLIB_H\n");
    fprintf(fp, "#include <stdlib.h>\n");
    fprintf(fp, "#endif\t/* HAVE_STDLIB_H */\n\n");
    fprintf(fp, "#ifdef HAVE_MALLOC_H\n");
    fprintf(fp, "#include <malloc.h>\n");
    fprintf(fp, "#endif\t/* HAVE_MALLOC_H */\n\n");
    fprintf(fp, "#ifdef HAVE_STRING_H\n");
    fprintf(fp, "#include <string.h>\n");
    fprintf(fp, "#endif\t/* HAVE_STRING_H */\n\n");
    fprintf(fp, "#ifdef HAVE_MEMORY_H\n");
    fprintf(fp, "#include <memory.h>\n");
    fprintf(fp, "#endif\t/* HAVE_MEMORY_H */\n\n");
    fprintf(fp, "#ifdef HAVE_STDDEF_H\n");
    fprintf(fp, "#include <stddef.h>\n");
    fprintf(fp, "#endif /* HAVE_STDDEF_H */\n");
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"sr_trans.h\"\n");
    fprintf(fp, "#include \"context.h\"\n");

    fprintf(fp, "#include \"method.h\"\n");
    fprintf(fp, "#include \"makevb.h\"\n");
    fprintf(fp, "#include \"inst_lib.h\"\n");

    fprintf(fp, "#include \"oid_lib.h\"\n");

    if (row_status_in_file() != 0) {
        fprintf(fp, "#include \"lookup.h\"\n");
	fprintf(fp, "#include \"v2table.h\"\n");
	fprintf(fp, "#include \"tmq.h\"\n");
	fprintf(fp, "#include \"rowstatf.h\"\n");
    } else if (parser || search_table || v_elimination_data) {
	fprintf(fp, "#include \"lookup.h\"\n");
	fprintf(fp, "#include \"v2table.h\"\n");
    }

    fprintf(fp, "#include \"diag.h\"\n");
    fprintf(fp, "SR_FILENAME\n");

    if (v_elimination_data) {
	fprintf(fp, "#include \"min_v.h\"\n");
    }
    fprintf(fp, "#include \"mibout.h\"\n\n");

    /* if read_create option was specified */
    if (xxx_read_create) {
      for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
           ptr = ptr->next_sorted) {
         idx = get_index_array(ptr->name, &index_array);
   
         /* if family is a table and doesn't have read_create access */ 
         if (idx  &&  ptr->group_read_create == 0) {
            fprintf(fp, "#define %s_READ_CREATE\n", ptr->name);
         }
      }
      fprintf(fp, "\n");
   }

}

