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

#include <string.h>


/* this routine converts a text string into a hex form suitable for
 * passing into MakeOctetStringFromHex() */
char *
hexify_string(char *str)
{
    int len=0;  /* length of str */
    int s=0;    /* position in str */
    static char buf[1024];
    int b=0;    /* position in buf */
    int pcb=0;  /* flag: previous character was a possible escape-backslash */

    if (str == NULL) {
	return NULL;
    }
    len = strlen(str);
    if (len <= 0) {
	return NULL;
    }
    memset(buf, 0, 1024);

    for (s=0; s<len; s++) {
	switch(str[s]) {
	    case '\\':
		if (pcb) {
		    sprintf(buf+b, "%02X ", '\\');
		    b = strlen(buf);
		}
		pcb = 1;
		break;

	    /*
	     * From RFC 2579, Page 4:
	     * "NUL, LF, CR, BEL, BS, HT, VT and FF have the special
	     * meanings specified in RFC 854"
	     *
	     * If the -defval_c_string command-line argument is used,
	     * then occurrances of "\0", "\n", "\r", "\a", "\b", "\t",
	     * "\v", and "\f" in the DEFVAL clause in the MIB will be
	     * treated as escape sequences, and the appropriate
	     * non-graphic character will be substitued in the hex
	     * string.
	     *
	     * If the -defval_c_string command-line argument is NOT
	     * used, all cases below fall through to the default case.
	     */

	    case '0':				/* NUL, null */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", 0);
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'n':				/* LF, linefeed */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\n');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'r':				/* CR, carriage return */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\r');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'a':				/* BEL, bell */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\a');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'b':				/* BS, backspace */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\b');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 't':				/* HT, horizontal tab */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\t');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'v':				/* VT, vertical tab */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\v');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    case 'f':				/* FF, form feed */
                if (defval_c_string) {
                    if (pcb) {
                        sprintf(buf+b, "%02X ", '\f');
                        b = strlen(buf);
                        pcb = 0;
                    } else {
                        sprintf(buf+b, "%02X ", str[s]);
                        b = strlen(buf);
                    }
                    break;
                }
                /* no break intentional */

	    default:
		if (pcb) {
		    sprintf(buf+b, "%02X ", '\\');
		    b = strlen(buf);
		    pcb = 0;
		}
		sprintf(buf+b, "%02X ", str[s]);
		b = strlen(buf);
		break;
	}
    }
    if (pcb) {
	sprintf(buf+b, "%02X ", '\\');
	b = strlen(buf);
	pcb = 0;
    }
    if (b>0) {
        buf[b-1] = '\0';  /* cover over the last space character */
    }
    return buf;
}


/* this routine produces initialization and termination method routines
 * for the mib objects. */
void
write_k_init_routines(fp, base)
    FILE           *fp;
    char           *base;
{
    struct OID_INFO *ptr;
    struct _index_array *index_array;
    int row_status_flag;
    int idx;
    struct OID_INFO *temp_ptr;
    char *hex;

    char           *struct_name = NULL;

    /* get the base name of the initialization method routine */
    row_status_flag = row_status_in_file();
    if (per_file_init == 0 && row_status_flag == 0) {
	if (k_init == 0) {
	    /* don't write an initialization routine */
	    return;
	}
    }

    /* #include the file given with the -include option */
    if (include_file) {
	fprintf(fp, "#include \"%s\"\n", include_file);
	fprintf(fp, "\n");
    }

    /* print initialization routines */
    if (row_status_flag == 0 && parser == 0) {
	if (per_file_init == 0) {
	    if (k_init) {
		fprintf(fp, "int\n");
		fprintf(fp, "k_initialize(void)\n");
		fprintf(fp, "{\n");
		fprintf(fp, "    return 1;\n");
		fprintf(fp, "}\n");
		fprintf(fp, "\n");
                if (k_term) {
		    fprintf(fp, "int\n");
		    fprintf(fp, "k_terminate(void)\n");
		    fprintf(fp, "{\n");
		    fprintf(fp, "    return 1;\n");
		    fprintf(fp, "}\n");
		    fprintf(fp, "\n");
                }
	    }
	}
	else {
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_initialize(void)\n", base);
	    fprintf(fp, "{\n");
	    fprintf(fp, "    return 1;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_terminate(void)\n", base);
	    fprintf(fp, "{\n");
	    fprintf(fp, "    return 1;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");
	}
    }
    else {
	/* process each valid family of objects */
	for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	     ptr = ptr->next_sorted) {

	    if (!ptr->valid) continue;

            struct_name = ptr->parent_ptr->name;
	    /* get indexing information */
	    idx = get_index_array(ptr->name, &index_array);

	    /* find out if row status is used */
	    if ((row_status_in_family(ptr) != NULL) || ((search_table || parser) && (idx != 0))) {
		row_status_flag = 1;
	    }
	    else {
		row_status_flag = 0;
	    }

	    /* print the standard init header */
	    fprintf(fp, "/* initialize support for %s objects */\n", ptr->name);
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_initialize(void)\n", ptr->name);
	    fprintf(fp, "{\n");

	    /* print row status support? */
	    if (row_status_flag != 0) {
		fprintf(fp, "    if ((%s.tp = (void **) malloc (sizeof(%s_t *))) == NULL) {\n", struct_name, ptr->name);
		fprintf(fp, "        return 0;\n");
		fprintf(fp, "    }\n");
		fprintf(fp, "\n");
	    }

            /* print scalar parser support */
            if (idx == 0 && parser) {
	        fprintf(fp,
"    /*\n"
"     * Initialize scalar objects with reasonable default values.\n"
"     * This must be done prior to reading the configuration file.\n"
"     * Any values found in the configuration file will overwrite\n"
"     * these initial values in RAM.\n"
"     */\n"
"\n"
"    ZERO_VALID(%sData.valid);\n\n", ptr->name);
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                     temp_ptr = temp_ptr->next_family_entry) {
		    if (!temp_ptr->valid) continue;
                    if ((strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Integer32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Unsigned32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Counter") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Counter32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Gauge") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Gauge32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "TimeTicks") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "IpAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "UInteger32") == 0)) {
                        if (temp_ptr->defval) {
                            if (isdigit((unsigned char)temp_ptr->defval[0])) {
                                fprintf(fp, "    %sData.%s = %s; "
                                        "/* DEFVAL */\n",
                                        ptr->name, temp_ptr->name,
                                        temp_ptr->defval);
                            } else if (strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) {
                                if( temp_ptr->enumer != NULL){
                                    remove_hyphens(temp_ptr->defval);
                                    fprintf(fp, "    %sData.%s = D_%s_%s; "
                                            "/* DEFVAL */\n",
                                            ptr->name, temp_ptr->name,
                                            temp_ptr->name, temp_ptr->defval);
                                } else {
                                    fprintf(fp, "    %sData.%s = %s; "
                                            "/* DEFVAL */\n",
                                            ptr->name, temp_ptr->name,
                                            temp_ptr->defval);
                                } 
                            }
                        } else {
                            fprintf(fp, "    %sData.%s = (SR_UINT32) 0;\n",
                                    ptr->name, temp_ptr->name);
                        }
                        fprintf(fp, "    SET_VALID(I_%s, %sData.valid);\n",
                                temp_ptr->name, ptr->name);
                    } else if ((strcmp(temp_ptr->oid_type, "DisplayString") == 0) ||
                        (strcmp(temp_ptr->oid_type, "SnmpAdminString") == 0) ||
                        (strcmp(temp_ptr->oid_type, "SnmpTagValue") == 0) ||
			(strcmp(temp_ptr->oid_prim_type, "PhysAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "OctetString") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Opaque") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "NsapAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "NetworkAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Bits") == 0)) {
                        fprintf(fp, "    %sData.%s = \n        ",
                                ptr->name, temp_ptr->name);
                        if (temp_ptr->defval) {
                            if (strchr(temp_ptr->defval, '\\')) {
                                hex = hexify_string(temp_ptr->defval);
                                if (hex != NULL) {
                                    fprintf(fp,
                                            "MakeOctetStringFromHex(\"%s\");\n"
                                            "        /* DEFVAL = \"%s\" */\n",
                                            hex, temp_ptr->defval);
                                } else {
                                    fprintf(fp, "MakeOctetString(NULL, 0);\n");
                                }
                            } else {
                                fprintf(fp, "MakeOctetStringFromText(\"%s\"); "
                                        "/* DEFVAL */\n",
                                        temp_ptr->defval);
                            }
                        } else {
                            fprintf(fp, "MakeOctetString(NULL, 0);\n");
                        }
                        fprintf(fp, "    if (%sData.%s != NULL) {\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "        SET_VALID(I_%s, %sData.valid);\n",
                                temp_ptr->name, ptr->name);
                        fprintf(fp, "    }\n");
                    } else if (strcmp(temp_ptr->oid_prim_type, "ObjectID") == 0) {
                        int local_found = 0;
                        struct OID_INFO *p;

                        if (temp_ptr->defval) {
                            for (p = sorted_root; p != NULL; p = p->next_sorted) {
                                if (strcmp(p->name, temp_ptr->defval) == 0) {
                                    local_found = 1;
                                    break;
                                }
                            }
                            if (local_found) {
                                fprintf(fp, "    /* %s = %s */\n",
                                        p->name, p->oid_fleshed_str);
                                fprintf(fp, "    %sData.%s = \n        "
                                        "MakeOIDFromDot(\"%s\"); "
                                        "/* DEFVAL */\n",
                                        ptr->name, temp_ptr->name,
                                        p->oid_fleshed_str);
                            } else {
                                fprintf(fp, "    %sData.%s = "
                                        "CloneOID(&nullOID);\n",
                                        ptr->name, temp_ptr->name);
                            }
                        } else {
                            fprintf(fp, "    %sData.%s = "
                                    "CloneOID(&nullOID);\n",
                                    ptr->name, temp_ptr->name);
                        }
                        fprintf(fp, "    if (%sData.%s != NULL) {\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "        SET_VALID(I_%s, %sData.valid);\n",
                                temp_ptr->name, ptr->name);
                        fprintf(fp, "    }\n");
                    } else if (strcmp(temp_ptr->oid_prim_type, "Counter64") == 0) {
                        fprintf(fp, "    %sData.%s = "
                                "MakeCounter64(0);\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "    if (%sData.%s != NULL) {\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "        SET_VALID(I_%s, %sData.valid);\n",
                                temp_ptr->name, ptr->name);
                        fprintf(fp, "    }\n");
                    } else {
                        /* unknown type */
                        fprintf(stderr, "%s: write_k_init_routines: "
                                "unknown type %s\n",
                                prog_name, temp_ptr->oid_prim_type);
                        exit(-1);
                    }
                }
            }

	    /* close the function */
	    fprintf(fp, "    return 1;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");

	    /* print the standard terminate header */
	    fprintf(fp, "/* terminate support for %s objects */\n", ptr->name);
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_terminate(void)\n", ptr->name);
	    fprintf(fp, "{\n");

	    /* print row status support? */
            /* find out if row status is used */
	    if ((row_status_in_family(ptr) != NULL) || ((search_table || parser) && (idx != 0))) {
                row_status_flag = 1;
            }
            else {
                row_status_flag = 0;
            }

	    if (row_status_flag != 0) {
		/* get indexing information */
		get_index_array(ptr->name, &index_array);

		/* print out table shutdown */
		fprintf(fp, "    /* free allocated memory */\n");
		fprintf(fp, "    while(%s.nitems > 0) {\n", struct_name);
		fprintf(fp, "        Delete%s(0);\n",
			ptr->name);
		fprintf(fp, "    }\n");
		fprintf(fp, "    \n");
		fprintf(fp, "    free(%s.tp);\n", struct_name);
	    }

            /* print scalar parser support */
            if (idx == 0 && parser) {
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                     temp_ptr = temp_ptr->next_family_entry) {
                    if ((strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Integer32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Unsigned32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Counter") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Counter32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Gauge") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Gauge32") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "TimeTicks") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "IpAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "UInteger32") == 0)) {
			/* nothing to do just don't take the default case */
                    } else if ((strcmp(temp_ptr->oid_type, "DisplayString") == 0) ||
                        (strcmp(temp_ptr->oid_type, "SnmpAdminString") == 0) ||
                        (strcmp(temp_ptr->oid_type, "SnmpTagValue") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "PhysAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "OctetString") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Opaque") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "NsapAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "NetworkAddress") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Bits") == 0)) {
                        fprintf(fp, "    FreeOctetString(%sData.%s);\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "    %sData.%s = NULL;\n",
                                ptr->name, temp_ptr->name);
                    } else if (strcmp(temp_ptr->oid_prim_type, "ObjectID") == 0) {
                        fprintf(fp, "    FreeOID(%sData.%s);\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "    %sData.%s = NULL;\n",
                                ptr->name, temp_ptr->name);
                    } else if (strcmp(temp_ptr->oid_prim_type, "Counter64") == 0) {
                        fprintf(fp, "    FreeCounter64(%sData.%s);\n",
                                ptr->name, temp_ptr->name);
                        fprintf(fp, "    %sData.%s = NULL;\n",
                                ptr->name, temp_ptr->name);
                    } else {
                        /* unknown type */
                        fprintf(stderr, "%s: write_k_init_routines: "
                                "unknown type %s\n",
                                prog_name, temp_ptr->oid_prim_type);
                        exit(-1);
                    }
                }
            }

	    /* close the function */
	    fprintf(fp, "    return 1;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");
	}

	/* add a call to each function generated above */

	if (per_file_init == 0) {
	    if (k_init) {
		fprintf(fp, "int\n");
		fprintf(fp, "k_initialize(void)\n");
		fprintf(fp, "{\n");
		fprintf(fp, "    int status = 1;\n");
		fprintf(fp, "\n");
		for (ptr = sorted_root;
		     (ptr = next_valid_family(ptr)) != NULL;
		     ptr = ptr->next_sorted) {
		    fprintf(fp, "    if (k_%s_initialize() != 1) {\n",
			    ptr->name);
		    fprintf(fp, "        status = -1;\n");
		    fprintf(fp, "    }\n");
		}
		fprintf(fp, "\n");
		fprintf(fp, "    return status;\n");
		fprintf(fp, "}\n");
		fprintf(fp, "\n");
                if (k_term) {
		    fprintf(fp, "int\n");
		    fprintf(fp, "k_terminate(void)\n");
		    fprintf(fp, "{\n");
		    fprintf(fp, "    int status = 1;\n");
		    fprintf(fp, "\n");
		    for (ptr = sorted_root;
		         (ptr = next_valid_family(ptr)) != NULL;
		         ptr = ptr->next_sorted) {
		        fprintf(fp, "    if (k_%s_terminate() != 1) {\n",
		    	        ptr->name);
		        fprintf(fp, "        status = -1;\n");
		        fprintf(fp, "    }\n");
		    }
		    fprintf(fp, "\n");
		    fprintf(fp, "    return status;\n");
		    fprintf(fp, "}\n");
		    fprintf(fp, "\n");
                }
	    }
	}
	else {
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_initialize(void)\n", base);
	    fprintf(fp, "{\n");
	    fprintf(fp, "    int status = 1;\n");
	    fprintf(fp, "\n");
	    for (ptr = sorted_root;
		 (ptr = next_valid_family(ptr)) != NULL;
		 ptr = ptr->next_sorted) {
		fprintf(fp, "    if (k_%s_initialize() != 1) {\n",
			ptr->name);
		fprintf(fp, "        status = -1;\n");
		fprintf(fp, "    }\n");
	    }
	    fprintf(fp, "\n");
	    fprintf(fp, "    return status;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");
	    fprintf(fp, "int\n");
	    fprintf(fp, "k_%s_terminate(void)\n", base);
	    fprintf(fp, "{\n");
	    fprintf(fp, "    int status = 1;\n");
	    fprintf(fp, "\n");
	    for (ptr = sorted_root;
		 (ptr = next_valid_family(ptr)) != NULL;
		 ptr = ptr->next_sorted) {
		fprintf(fp, "    if (k_%s_terminate() != 1) {\n",
			ptr->name);
		fprintf(fp, "        status = -1;\n");
		fprintf(fp, "    }\n");
	    }
	    fprintf(fp, "\n");
	    fprintf(fp, "    return status;\n");
	    fprintf(fp, "}\n");
	    fprintf(fp, "\n");
	}
    }
}
