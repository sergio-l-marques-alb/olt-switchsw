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
#include "mib_tree.h"
#ifdef SR_AGENT_TOOLS
#include "agent.h"
#endif	/* SR_AGENT_TOOLS */
char buff[512];

/* this structure lists objects to be included in the "snmp-mib.h" file
 * when LIGHT is defined */
typedef struct _LightOIDs {
    char *name;
    char *oid;
} LightOIDs;

static LightOIDs *lightOIDs;

static int cmp_txt_oid SR_PROTOTYPE((void *, void *));
void CreateLightOIDTable SR_PROTOTYPE((void));
int output_cmib_file SR_PROTOTYPE((char *base));
void output_mgroidlist_file(char *base);
#ifdef CIAGENT_MONITOR
int output_ciagentmonitor_file(char *base);
int output_rtview_file(char *base);
#endif /* CIAGENT_MONITOR */
#ifdef SR_EP_TOOLS
int output_varbind_headers(char *base);
int output_varbind_file(char *base);
#endif /* SR_EP_TOOLS */
#ifdef SR_NAA_TOOLS
int output_dynamic_naa_file(char *base);
int output_static_naa_file(char *base);
#endif /* SR_NAA_TOOLS */

int
output_file(base)
    char           *base;
{
#ifdef SR_NAA_TOOLS
    if (naa_static) {
        output_static_naa_file(base);
    }
    if (naa_dynamic) {
        output_dynamic_naa_file(base);
    }
#else /* SR_NAA_TOOLS */


#ifdef SR_AGENT_TOOLS
#endif /* SR_AGENT_TOOLS */

#ifdef SR_EP_TOOLS
    if (build_varbinds) {
      output_varbind_headers(base);
      output_varbind_file(base); 
    }
    if (snmpmibh) {
      output_mib_file(base);
    }
    if (snmpinfo || inputinfo || remove_subtree) {
        output_xmib_file(base);
    }
#else /* SR_EP_TOOLS */

    if (inputinfo || remove_subtree) {
        output_manager_files(base);
#ifdef CIAGENT_MONITOR
    } else if (ciagentmonitor) {
        output_ciagentmonitor_file(base);
    } else if (rtview) {
        output_rtview_file(base);
#endif /* CIAGENT_MONITOR */
    } else if (mgroidl) {
        output_mgroidlist_file(base);
    } else {
        if (!stubs_only && !snmpoid_only) {
            output_manager_files(base);
        }

#ifdef SR_AGENT_TOOLS
        if (!stubs_only) {
            if (!mgrtool) {
                output_agent_files(base);
            }
        }

        /* if -agent is given on the command line, write method routine stubs */
        if (!mgrtool) {
            if (agent) {
                if (!v_stubs_only) {
                    write_k_agent_files(base);
                }
                write_v_agent_files(base);
            }
            if (traps) {
                write_traps(base);
            }
            if (oidl) {
                write_agent_oidlist_file(base);
            }
            if (localconnect) {
                write_localconnect_file(base);
            }
            if (parser) {
                write_configio_file(base);
            }
            if (make) {
                output_c_make_file(base);
            }
        }
#endif /* SR_AGENT_TOOLS */
    }
#endif /* SR_EP_TOOLS */
#endif /* SR_NAA_TOOLS */
    return 1;
}

/* write the following output files:
 *    snmp-mib.h
 *    snmpinfo.dat
 */
int
output_manager_files(base)
    char           *base;
{
    if (!inputinfo && !remove_subtree) {
#ifdef SR_AGENT_TOOLS
        if (mgrtool || snmpmibh) {
            output_mib_file(base);
        }
#else /* SR_AGENT_TOOLS */
        output_mib_file(base);
#endif /* SR_AGENT_TOOLS */
    }
    if (mibtree) {
	output_cmib_file(base); 
    }
    output_xmib_file(base);

    return 1;
}

/* write the following output files:
 *    snmp-mib.h
 */
int
output_mib_file(base)
    char           *base;
{
    struct OID_INFO *ptr;
    char            buf[128];
    FILE           *fp_mib;
    char            sr_far[8];
    char	   *sr_namestring, *sr_valuestring;
    LightOIDs	   *loidp;
 
    CreateLightOIDTable();

    fp_mib = open_file(base, "-mib.h");

    /* output copyright notices */
    print_copyright(fp_mib);

    if (!no_cvs_keywords) {
      print_cvs_keywords(fp_mib, H_FILE);
    }

    /* output Automatically Generated by mib compiler note */
    fprintf(fp_mib, "/*\n * automatically generated by the mib compiler -- do not edit\n");
    fprintf(fp_mib, " */\n\n");

    print_start_protection(fp_mib, base, "mib");

    fprintf(fp_mib, "#ifdef SR_MIB_TABLE_HASHING\n");
    fprintf(fp_mib, "#define HASHNEXT(x) x, NULL\n");
    fprintf(fp_mib, "#else /* SR_MIB_TABLE_HASHING */\n");
    fprintf(fp_mib, "#define HASHNEXT(x) x\n");
    fprintf(fp_mib, "#endif /* SR_MIB_TABLE_HASHING */\n");
    fprintf(fp_mib, "\n");

    /* the snmp-mib file */
    fprintf(fp_mib, "/* SNMP-MIB translation table.  Subitem on left gets\n");
    fprintf(fp_mib, " * translated into subitem on right.\n");
    fprintf(fp_mib, " */\n");

    if (intel) {
         sr_namestring = "_String";
         sr_valuestring = "_Value";
	 fprintf(fp_mib, "#ifndef LIGHT\n");
	 for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	        if (ptr->valid) {
		    fprintf(fp_mib, "static char FAR %s%s[] = \"%s\",\n", ptr->name,
			    sr_namestring, ptr->name);
		    fprintf(fp_mib, "            FAR %s%s[] = \"%s\";\n", ptr->name,
			    sr_valuestring, ptr->oid_fleshed_str);
	        }
	    }
	    fprintf(fp_mib, "#else /* LIGHT */\n");
	    for(loidp = lightOIDs; loidp->name != 0; loidp++) {
	        fprintf(fp_mib, "static char FAR %s%s[] = \"%s\",\n", 
	    	    loidp->name, sr_namestring, loidp->name);
	        fprintf(fp_mib, "            FAR %s%s[] = \"%s\";\n", 
		    loidp->name, sr_valuestring, loidp->oid);
	    }
	    fprintf(fp_mib, "#endif /* LIGHT */\n");

    } else {
    	    sr_namestring = "";
	    sr_valuestring = "";
    }
    fprintf(fp_mib, "\n");

    fprintf(fp_mib, "#ifndef LIGHT\n");
    if (intel) {
        strcpy(sr_far, "FAR ");
    }
    else {
	sr_far[0] = '\0';
    }
    fprintf(fp_mib, "static struct MIB_OID %sorig_mib_oid_table[] =\n", sr_far);
    fprintf(fp_mib, "{\n");	/* } */

    /*
     * go through the list of oids and write information to files
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (ptr->valid) {

	    /*
	     * write out the data to the snmp-mib file
	     */
	    if (intel) {
	        fprintf(fp_mib, "    %s%s, %s%s,\n", ptr->name, 
			sr_namestring, ptr->name, sr_valuestring);
	    }
	    else {
	        sprintf(buf, "\"%s\", ", ptr->name);

#ifdef DO_NOT_BRACKET_INITIALIZERS
		fprintf(fp_mib, "    %-35sHASHNEXT(\"%s\"),\n", buf, ptr->oid_fleshed_str);
#else				/* DO_NOT_BRACKET_INITIALIZERS */
	    	fprintf(fp_mib, "    { %-35sHASHNEXT(\"%s\") },\n", buf, ptr->oid_fleshed_str);
#endif				/* DO_NOT_BRACKET_INITIALIZERS */
	    }
	}
    }
    /*
     * Now print trailer for snmp-mib.h (just end of structure now)
     */

#ifdef DO_NOT_BRACKET_INITIALIZERS 
    fprintf(fp_mib, "    0, HASHNEXT(0)\n};\n");
#else				/* DO_NOT_BRACKET_INITIALIZERS */ 
    fprintf(fp_mib, "    { (char *) 0, HASHNEXT((char *) 0) }\n};\n");
#endif				/* DO_NOT_BRACKET_INITIALIZERS */

    fprintf(fp_mib, "#else /* LIGHT */\n");
    fprintf(fp_mib, "static struct MIB_OID %sorig_mib_oid_table[] =\n", sr_far);
    fprintf(fp_mib, "{\n");	/* } */
    for(loidp = lightOIDs; loidp->name != 0; loidp++) {
        if(intel) {
#ifdef DO_NOT_BRACKET_INITIALIZERS
	    fprintf(fp_mib, "    %s%s, %s%s,\n",
		    loidp->name, sr_namestring, loidp->name, sr_valuestring);
#else				/* DO_NOT_BRACKET_INITIALIZERS */
	    fprintf(fp_mib, "    { %s%s, %s%s },\n",
		    loidp->name, sr_valuestring, loidp->name, sr_valuestring);
#endif				/* DO_NOT_BRACKET_INITIALIZERS */
	} else {
	    sprintf(buf, "\"%s\", ", loidp->name);
#ifdef DO_NOT_BRACKET_INITIALIZERS
	    fprintf(fp_mib, "    %-35sHASHNEXT(\"%s\"),\n",buf, loidp->oid);
#else				/* DO_NOT_BRACKET_INITIALIZERS */
	    fprintf(fp_mib, "    { %-35sHASHNEXT(\"%s\") },\n", buf, loidp->oid);
#endif				/* DO_NOT_BRACKET_INITIALIZERS */
	}
    }

    /* { */

    /* null terminate the list */
#ifdef DO_NOT_BRACKET_INITIALIZERS
    fprintf(fp_mib, "    0, HASHNEXT(0)\n};\n");
#else				/* DO_NOT_BRACKET_INITIALIZERS */
    fprintf(fp_mib, "    { (char *) 0, HASHNEXT((char *) 0) }\n};\n");
#endif				/* DO_NOT_BRACKET_INITIALIZERS */

    fprintf(fp_mib, "#endif /* LIGHT */\n");
    fprintf(fp_mib, "\n");
    fprintf(fp_mib, "\n");
    fprintf(fp_mib, "\n");
    fprintf(fp_mib, "/* Don't put anything after this #endif */ \n");

    /*
     * print #endif for multiple inclusion protection
     */
    print_end_protection(fp_mib, base, "mib");

    /* close all of the files */
    fclose(fp_mib);
    return 1;
}

/* write the following output files:
 *    snmpinfo.dat
 */
int
output_xmib_file(base)
    char           *base;
{
    struct OID_INFO *ptr;
    char           *temp;
    FILE           *fp_xmib;

    if (mergeinfo || remove_subtree) {
        if ((temp = (char *) getenv("SR_MGR_CONF_DIR")) == NULL) {
            temp = SR_MGR_CONF_FILE_PREFIX;
        }
        fp_xmib = open_file(temp, "/snmpinfo.dat");
    } else {

        /* open the files */
        fp_xmib = open_file(base, "info.dat");
    }

    /* output copyright notices */
    /* print_copyright_2(fp_xmib); */
    print_copyright(fp_xmib);

    fprintf(fp_xmib, "/*\n * automatically generated by the mib compiler -- do not edit\n");
    fprintf(fp_xmib, " */\n\n");
    /*
     * fprintf(fp_xmib,"# \n# automatically generated by the mib compiler --
     * do not edit\n"); fprintf(fp_xmib, "# \n\n");
     */

    /*
     * go through the list of oids and write information to files
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
	if (ptr->valid) {

	    /*
	     * write the data to the extended mib file (snmpinfo.dat)
	     */
	    fprintf(fp_xmib, "%-22s %-30s ", ptr->name, ptr->oid_fleshed_str);


	    if (ptr->oid_type) {
                if (isAggregateType(ptr->oid_type)) {
		    fprintf(fp_xmib, "%-15s ", "Aggregate");
                } 
		else if (ptr->size) {
		    if (ptr->size->next == NULL) {
                        if ( (strcmp(ptr->oid_type, "INTEGER") == 0) ||
                             (strcmp(ptr->oid_type, "Integer32") == 0) ) {
		            sprintf(buff, "%s(%ld..%ld)", ptr->oid_prim_type, 
                                    ptr->size->min, 
                                    ptr->size->max);
                        } else {
		            sprintf(buff, "%s(%lu..%lu)", ptr->oid_prim_type, 
                                    (unsigned long) ptr->size->min, 
                                    (unsigned long) ptr->size->max);
                        }

		        fprintf(fp_xmib, "%-15s ", buff);
                    } else {
		        fprintf(fp_xmib, "%-15s ", ptr->oid_prim_type);
			/* here is where we should handle size ranges of 
			 * the form SIZE(0 | 8..16)
			 */
        	    }
		} else {
		    fprintf(fp_xmib, "%-15s ", ptr->oid_prim_type);
		}
		fprintf(fp_xmib, " ");
	    }
	    else {
		fprintf(fp_xmib, "%-15s ", "nonLeaf");
	    }
	    if (ptr->oid_access) {
		fprintf(fp_xmib, "%s", ptr->oid_access);
	    }
	    fprintf(fp_xmib, "\n");

	    /*
	     * print the INDEX clause 
	     */
	    {
		int idx;
		struct _index_array *index_array, *temp_index;

                idx = get_index_array2(ptr, &index_array);
		if (idx) {
		    fprintf(fp_xmib, "{\n");
		}
                for (temp_index = index_array; temp_index && temp_index->name; temp_index++) {
		    if (temp_index->IndexType == SR_IMPLIED) {
			fprintf(fp_xmib, "IMPLIED %s\n", temp_index->name);
		    } else {
			fprintf(fp_xmib, "        %s\n", temp_index->name);
		    }
                }
		if (idx) {
		    fprintf(fp_xmib, "}\n");
		}
            }

	    /*
	     * if there are enumerations, print them
	     */
	    if (ptr->enumer) {
		temp_enum = ptr->enumer;
		fprintf(fp_xmib, "(\n");
		while (temp_enum) {
		    fprintf(fp_xmib, "   %-3d %s\n", temp_enum->val, temp_enum->name);
		    temp_enum = temp_enum->next;
		}
		fprintf(fp_xmib, ")\n");
	    }
	}
    }
    fclose(fp_xmib);
    return 1;
}

/* 
 * Write the snmpinfo.c file.
 */
int
output_cmib_file(base)
    char *base;
{
    FILE *fp_cmib;
    OID_INFO_t *oid, *oid_tmp;
    enumeration_t *enu;
    index_array_t *index_array, *index;
    int i, hash_value, oid_no, no, oid_no_tmp, 
	len, parent, next, prev, first_descendent;
    short type, access;
    struct xref {
	int enumer, mib_index, hash_next, hash_prev;
	OID_INFO_t *oid;
    } *xref;
    int hash_table[MIB_HASH_TABLE_SIZE];

    fp_cmib = open_file(base, "info.c");

    /* Output copyright and includes */
    print_copyright(fp_cmib);

    if (!no_cvs_keywords) {
      print_cvs_keywords(fp_cmib, C_FILE);
    }

    fputs("\
/*\n\
 * automatically generated by the mib compiler -- do not edit\n\
 */\n\
\n\
#include \"sr_conf.h\"\n\
#ifdef HAVE_STDIO_H\n\
#include <stdio.h>\n\
#endif /* HAVE_STDIO_H */\n\
#include \"sr_snmp.h\"\n\
#include \"lookup.h\"\n\
#include \"v2table.h\"\n\
#include \"scan.h\"\n\
#include \"mib_tree.h\"\n\
\n\
", fp_cmib);

    for (i = 0; i < MIB_HASH_TABLE_SIZE; i++) {
	hash_table[i] = -1;
    }

    /* Count number of nodes and allocate xref array. */
    oid_no = 0;
    for (oid = sorted_root; oid != NULL; oid = oid->next_sorted) {
	if (oid->valid) {
	    oid_no++;
	}
    }
    if ((xref = 
	 (struct xref *) malloc(sizeof(struct xref) * oid_no)) == NULL) {
	printf("output_cmib_file: out of memory\n");
	exit(-1);
    }

    /* Output enumerations */
    fputs("\
/* Enumerations */\n\
static const ENUMER enumerations[] = {\n\
", fp_cmib);
    oid_no = -1;
    no = 0;
    for (oid = sorted_root; oid != NULL; oid = oid->next_sorted) {
	if (!oid->valid) {
	    continue;
	}
	xref[++oid_no].oid = oid;
	/* Build hash table */
	hash_value = CalculateMIBHash(oid->name, -1);
	xref[oid_no].hash_prev = -1;
	if (hash_table[hash_value] == -1) {
	    xref[oid_no].hash_next = -1;
	} else {
	    xref[hash_table[hash_value]].hash_prev = oid_no;
	    xref[oid_no].hash_next = hash_table[hash_value];
	}
	hash_table[hash_value] = oid_no;
	if (!oid->enumer) {
	    xref[oid_no].enumer = -1;
	    continue;
	}
	fprintf(fp_cmib, "    /* %s */\n", oid->name);
	xref[oid_no].enumer = no;
	for (enu = oid->enumer; enu != NULL; enu = enu->next, no++) {
	    fprintf(fp_cmib, "    { \"%s\", %d, ", enu->name, enu->val);
	    if (enu->next != NULL) {
		fprintf(fp_cmib, "(ENUMER *) enumerations + %d", no + 1);
	    } else {
		fputs("NULL", fp_cmib);
	    }
	    fputs(" }", fp_cmib);
	    if (oid->next != NULL) {
		fputs(",", fp_cmib);
	    }
	    fputs("\n", fp_cmib);
	}
    }
    fputs("};\n\n", fp_cmib);

    /* Output MIB indexes */
    fputs("\
/* MIB indexes */\n\
static const MIB_INDEX mib_index[] = {\n\
", fp_cmib);
    oid_no = -1;
    no = 0;
    for (oid = sorted_root; oid != NULL; oid = oid->next_sorted) {
	if (!oid->valid) {
	    continue;
	}
	oid_no++;
	if (get_index_array(oid->name, &index_array) == 0) {
	    xref[oid_no].mib_index = -1;
	    continue;
	}
	fprintf(fp_cmib, "    /* %s */\n", oid->name);
	xref[oid_no].mib_index = no;
	
	for (index = index_array;
	     index != NULL && index->name != NULL;
	     index++, no++) {

	    fprintf(fp_cmib, "    { \"%s\", %d, ", 
		    index->name, index->IndexType == SR_IMPLIED ? 1 : 0);
	    if (index + 1 != NULL && (index + 1)->name != NULL) {
		fprintf(fp_cmib, "(MIB_INDEX *) mib_index + %d", no + 1);
	    } else {
		fputs("NULL", fp_cmib);
	    }
	    fputs(" }", fp_cmib);
	    if (oid->next != NULL) {
		fputs(",", fp_cmib);
	    }
	    fputs("\n", fp_cmib);
	}
    }
    fputs("};\n\n", fp_cmib);

    /* Output MIB Tree nodes */
    fputs("\
/* MIB Tree elements */\n\
static const OID_TREE_ELE root[] = {\n\
",  fp_cmib);
    oid_no = 0;
    for (oid = sorted_root; oid != NULL; oid = oid->next_sorted) {
	if (!oid->valid) {
	    continue;
	}
	if (oid->oid_prim_type != NULL) {
	    type = TypeStringToShort(oid->oid_prim_type);
	} else {
	    type = NON_LEAF_TYPE;
	}
	if (oid->oid_access != NULL) {
	    access = AccessStringToShort(oid->oid_access);
	} else {
	    access = NOT_ACCESSIBLE_ACCESS;
	}
	fprintf(fp_cmib, "    /* %s */\n", oid->name);
	/* sid_value, oid_name, oid_number_str, type, access */
	fprintf(fp_cmib, "\
    {\n\
        %u,\n\
        \"%s\",\n\
        \"%s\",\n\
        0x%hx, 0x%hx,\n\
",
		(unsigned int) oid->oid_ptr->oid_ptr[oid->oid_ptr->length - 1],
		oid->name, oid->oid_fleshed_str, 
		(unsigned short) type, (unsigned short) access);
	/* enumer */
	if (xref[oid_no].enumer >= 0) {
	    fprintf(fp_cmib, "        (ENUMER *) enumerations + %d, ", 
		    xref[oid_no].enumer);
	} else {
	    fputs("        NULL, ", fp_cmib);
	}
	/* mib_index */
	if (xref[oid_no].mib_index >= 0) {
	    fprintf(fp_cmib, "(MIB_INDEX *) mib_index + %d,\n", 
		    xref[oid_no].mib_index);
	} else {
	    fputs("NULL,\n", fp_cmib);
	}
	/* size_lower, size_upper */
	if (oid->size != NULL && type != INTEGER_TYPE) {
	    fprintf(fp_cmib, "        %ld, %ld, ", 
		    oid->size->min, oid->size->max);
	} else {
	    fputs("        0, 0xffffffff, ", fp_cmib);
	}
	/* range_lower, range_upper */
	if (oid->size != NULL && type == INTEGER_TYPE) {
	    fprintf(fp_cmib, "%ld, %ld,\n", 
		    oid->size->min, oid->size->max);
	} else {
	    fputs("-0x7fffffff, 0x7fffffff,\n", fp_cmib);
	}
	/* hash_next */
	if (xref[oid_no].hash_next != -1) {
	    fprintf(fp_cmib, "        (OID_TREE_ELE *) root + %d, ", 
		    xref[oid_no].hash_next);
	} else {
	    fputs("        NULL, ", fp_cmib);	
	}
       	/* hash_prev */
	if (xref[oid_no].hash_prev != -1) {
	    fprintf(fp_cmib, "(OID_TREE_ELE *) root + %d, \n", 
		    xref[oid_no].hash_prev);
	} else {
	    fputs("NULL, \n", fp_cmib);	
	}
	/* parent, next, prev, first_descendent */ 
	parent = next = prev = first_descendent = -1;
	oid_no_tmp = -1;
	for (oid_tmp = sorted_root; 
	     oid_tmp != NULL; 
	     oid_tmp = oid_tmp->next_sorted) {
	    if (!oid_tmp->valid) {
		continue;
	    }
	    if (++oid_no_tmp == oid_no) {
		continue;
	    }
	    len = oid_tmp->oid_ptr->length;
	    if (len == oid->oid_ptr->length) {
		if (CmpNOID(oid->oid_ptr, oid_tmp->oid_ptr, len - 1) == 0) {
		    if (oid_tmp->oid_ptr->oid_ptr[len - 1] <
			oid->oid_ptr->oid_ptr[len - 1]) {
			if (prev == -1 || 
			    oid_tmp->oid_ptr->oid_ptr[len - 1] >
			    xref[prev].oid->oid_ptr->oid_ptr[len - 1]) {
			    prev = oid_no_tmp;
			}
		    } else {
			if (next == -1 || 
			    oid_tmp->oid_ptr->oid_ptr[len - 1] <
			    xref[next].oid->oid_ptr->oid_ptr[len - 1]) {
			    next = oid_no_tmp;
			}
		    }
		}
	    } else if (len == oid->oid_ptr->length - 1) {
		if (CmpNOID(oid->oid_ptr, oid_tmp->oid_ptr, len) == 0) {
		    parent = oid_no_tmp;
		}
	    } else if (len == oid->oid_ptr->length + 1 &&
		       CmpNOID(oid->oid_ptr, oid_tmp->oid_ptr, len - 1) == 0 &&
		       (first_descendent == -1 ||
			oid_tmp->oid_ptr->oid_ptr[len - 1] <
			xref[first_descendent].oid->oid_ptr->oid_ptr[len - 1]))
	    {
		first_descendent = oid_no_tmp;
	    }
	}
	if (parent >= 0) {
	    fprintf(fp_cmib, "        (OID_TREE_ELE *) root + %d, ", parent);
	} else {
	    fputs("        NULL, ", fp_cmib);	
	}
	if (next >= 0) {
	    fprintf(fp_cmib, "(OID_TREE_ELE *) root + %d,\n", next);
	} else {
	    fputs("NULL,\n", fp_cmib);	
	}
	if (prev >= 0) {
	    fprintf(fp_cmib, "        (OID_TREE_ELE *) root + %d, ", prev);
	} else {
	    fputs("        NULL, ", fp_cmib);	
	}
	if (first_descendent >= 0) {
	    fprintf(fp_cmib, "(OID_TREE_ELE *) root + %d\n", first_descendent);
	} else {
	    fputs("NULL\n", fp_cmib);	
	}
	fputs("    }", fp_cmib);
	if (oid->next_sorted != NULL) {
            OID_INFO_t *tmpoidinfo;
            int done = 1;
            for (tmpoidinfo = oid->next_sorted; tmpoidinfo; 
                              tmpoidinfo = tmpoidinfo->next_sorted) {
                if (!tmpoidinfo->valid ) {
                    continue;
                }
                done = 0;
                break;
            }
            /* add a comma, unless this is the last entry in the array */
            if (!done) {
	        fputs(",", fp_cmib);
            }
	}
	fputs("\n", fp_cmib);
	oid_no++;
    }
    fputs("};\n\n", fp_cmib);

    fputs("\
static const SR_UINT32 prefix_sids[] = { 1 };\n\
static const OID prefix = { 1, (SR_UINT32 *) &prefix_sids };\n\
\n\
const MIB_TREE snmpinfo_mib_tree = {\n\
    \"default\",\n\
    (OID *) &prefix,\n\
    NULL,\n\
    NULL,\n\
    (OID_TREE_ELE *) root,\n\
", fp_cmib);

    /* Output hash table */
    fputs("\
    /* MIB Hash Table */\n\
    {\n\
",  fp_cmib);
    for (i = 0; i < MIB_HASH_TABLE_SIZE; i++) {
	if (hash_table[i] != -1) {
	    fprintf(fp_cmib, "        (OID_TREE_ELE *) root + %d", 
		    hash_table[i]);
	} else {
	    fputs("        NULL", fp_cmib);
	}
	if (i < MIB_HASH_TABLE_SIZE - 1) {
	    fputs(",", fp_cmib);
	}
	fputs("\n", fp_cmib);
    }
    fputs("    }\n};\n", fp_cmib);
    free(xref);
    fclose(fp_cmib);
    return 1;
}


int
get_index_array(name, index)
    char           *name;
    struct _index_array *index[];
{
    struct index_table *table;
    int             count, i;
    int             unknown = 0;


    for (table = index_table; table; table = table->next) {
        if (strcmp(name, table->name) == 0) {
            /* printf("index %s\n",table->index); */
            *index = table->index_array;

            count = i = 0;
            while ((*index)[i].name) {
                if ((*index)[i].length > 0) {
                    count += (*index)[i].length;
                }
                else {
                    unknown++;
                }
                i++;
            }

            if (unknown)
                return (-1);
            else
                return (count);
        }
    }
    *index = NULL;
    return 0;
}

/*
 * get_index_array2 is like get_index_array except that it handles
 * the case where the table name is duplicated (duplicate name with
 * different OID)
 */
int
get_index_array2(struct OID_INFO *ptr, struct _index_array *index[])
{
    struct index_table *table;
    int count, i;
    int unknown = 0;
    char *name;
 
    if (ptr->augments) {
        name = ptr->augments;
    } else {
        name = ptr->name;
    }
 
    for (table = index_table; table; table = table->next) {
        if ( (strcmp(name, table->name) == 0) &&
            (table->ptr == ptr) ) {
            /* printf("index %s\n",table->index); */
            *index = table->index_array;
 
            count = i = 0;
            while ((*index)[i].name) {
                if ((*index)[i].length > 0) {
                    count += (*index)[i].length;
                }
                else {
                    unknown++;
                }
                i++;
            }
 
            if (unknown)
                return (-1);
            else
                return (count);
        }
    }
    *index = NULL;
    return 0;
}

/* this is the list of objects.  It must be sorted by OID. */
LightOIDs FixedlightOIDs[] = {
    { "snmpResearch",                    "1.3.6.1.4.1.99",		},
    { 0, 0 },
};

static int
cmp_txt_oid(void *s_1, void *s_2)
{
    LightOIDs *s1 = (LightOIDs *)s_1;
    LightOIDs *s2 = (LightOIDs *)s_2;
    OID *oid1, *oid2;
    int cmp;

    oid1 = MakeOIDFromDot(s1->oid);
    oid2 = MakeOIDFromDot(s2->oid);
    if ((oid1 == NULL) || (oid2 == NULL)) {
	printf("cmp_txt_oid: MakeOIDFromDot failed\n");
	exit(-1);
    }
   cmp = CmpOID(oid1, oid2);
   FreeOID(oid1);
   FreeOID(oid2);

   return cmp;
}

void
CreateLightOIDTable()
{
    FILE *fp;
    int count = 0;
    int found;
    int i;
    char buff[128];
    char *p;
    struct OID_INFO *ptr;

    if (!light_config_file) {
	lightOIDs = FixedlightOIDs;
        return;
    }

    /* open config file to count the entries */
    fp = fopen(light_config_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "postmosy: couldn't open %s for reading\n",
                light_config_file);
        exit(-1);
    }
    /* first, count the entries */
    while (fgets(buff, sizeof(buff), fp) != NULL) {
        count++;
    }
    fclose(fp);
    if ((lightOIDs = (LightOIDs *) malloc (count * sizeof(LightOIDs) + 1)) == NULL) {
        printf("CreateLightOIDTable: out of memory\n");
        exit(-1);
    }

    /* open config file to read the entries */
    fp = fopen(light_config_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "postmosy: couldn't open %s for reading\n",
                light_config_file);
        exit(-1);
    }
    count = 0;
    while (fgets(buff, sizeof(buff), fp) != NULL) {
	i = 0;
        while ( (buff[i] == ' ') || (buff[i] == '\t') ) {
            i++;
        }
        if ((buff[i] == '\n') || (buff[i] == '#')) {
            continue;
        }
 	p = (char *) strchr(buff, '\n');
	if (p) {
	    *p = '\0';
	}
        p = &buff[i];
        found = 0;
        for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
            if (strcmp(ptr->name, p) == 0) {
                lightOIDs[count].name = ptr->name;
                lightOIDs[count].oid = ptr->oid_fleshed_str;
                count++;
		found++;
		continue;
            }
        }
	if (!found) {
            printf("Warning: %s unknown (from file %s), %s will not be in the LIGHT section\n", p, light_config_file, p);
	}
    }

    /* Now, sort the list */
    /* The cast on cmp_txt_oid is there to keep some compilers (such as
     * gcc -ansi -Wall on SunOS) from complaining
     */
    qsort((char *) lightOIDs, count, sizeof(struct _LightOIDs), (int (*)(const void *, const void *)) cmp_txt_oid);

    /* NULL terminate the list */
    lightOIDs[count].name = NULL;
    lightOIDs[count].oid = NULL;

    return;
}

void
output_mgroidlist_file(char *base)
{
    FILE *fp;
    OID_INFO_t *ptr;
    int i;
    char type[64];
 
    fp = open_file(base, "mgroidl.c");
    print_copyright(fp);

    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"%smgroidl.h\"\n", base);
    fprintf(fp, "\n\n");

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (!ptr->valid) {
            continue;
        }
        if (ptr->oid_access == NULL) {
            continue;
        }
        if (isAggregateType(ptr->oid_type)) {
            continue;
        }

        map_variable_type_for_ber(ptr->oid_prim_type, type);
        fprintf(fp, "const SR_UINT32 ID%s[] = { ", ptr->name);

        i = 0;
        while (i < ptr->oid_ptr->length) {
#if SIZEOF_INT == 4
            fprintf(fp, "%u", ptr->oid_ptr->oid_ptr[i]);
#else	/*  SIZEOF_INT == 4 */
            fprintf(fp, "%lu", ptr->oid_ptr->oid_ptr[i]);
#endif	/*  SIZEOF_INT == 4 */
            i++;
            if (i == ptr->oid_ptr->length) {
                break;  /* exit the loop */
            } else {
                fprintf(fp, ", ");
            }
        }
        fprintf(fp, " };\n");
        /* fprintf(fp, "#define LN%s %ld\n", ptr->name, i); */
        fprintf(fp, "const OID oid_%s = { %ld, (SR_UINT32 *) ID%s };\n", 
                     ptr->name, (long int)i, ptr->name);
        /* fprintf(fp, "const VarBind vb_%s = { &oid_%s, %s, NULL };\n", 
                    ptr->name, ptr->name, type); */
        fprintf(fp, "VarBind *vb_%s = NULL;\n", ptr->name);
        fprintf(fp, "\n");
    }

    /* --- Now write the .h file with the externs --- */
 
    fp = open_file(base, "mgroidl.h");
    print_copyright(fp);
  
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "\n\n");
   

    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (!ptr->valid) {
            continue;
        }
        if (ptr->oid_access == NULL) {
            continue;
        }
        if (isAggregateType(ptr->oid_type)) {
            continue;
        }

        map_variable_type_for_ber(ptr->oid_prim_type, type);

        fprintf(fp, "extern const OID oid_%s;\n", ptr->name);
        fprintf(fp, "extern VarBind *vb_%s;\n", ptr->name);
        fprintf(fp, "#define TY%s %s\n", ptr->name, type);

        fprintf(fp, "\n");
    }
}


#ifdef CIAGENT_MONITOR
#include "mgrcnf.c"
#endif /* CIAGENT_MONITOR */
