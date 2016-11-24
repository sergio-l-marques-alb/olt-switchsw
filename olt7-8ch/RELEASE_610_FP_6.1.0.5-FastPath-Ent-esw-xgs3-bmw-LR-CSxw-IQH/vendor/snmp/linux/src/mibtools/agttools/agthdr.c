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

#include "sr_conf.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>	/* for unlink() */
#endif	/* HAVE_UNISTD_H */

#include "mibtools.h"
#include "agent.h"

#define ACCESS_READONLY 0x1
#define ACCESS_FAMILY 0x2
static void
emit_oid_access(FILE *fp,
                struct OID_INFO *ptr,
                int flags)
{
    char *name = (flags & ACCESS_FAMILY) ? 
                    ptr->oid_pre_str : ptr->name ;
    int getinfo = 1;
    int testinfo = -1;
    struct OID_INFO *temp_ptr = NULL;
    int found = 0;

    if (no_v_get && no_v_test) {

        /* find the correct element of the SrGetInfoEntry array */
        getinfo = ptr->getinfo;
        testinfo = ptr->testinfo;
        if (getinfo == -1) {
            /* not initialized... use the parent's info */
            getinfo = ptr->getinfo = ptr->parent_ptr->getinfo;
            testinfo = ptr->testinfo = ptr->parent_ptr->testinfo;
        }
        if (getinfo == -1) {
            /* not initialized... see if the parent augments another table */
            if (ptr->parent_ptr->augments != NULL) {
                for (temp_ptr = sorted_root; temp_ptr; 
                     temp_ptr = temp_ptr->next_sorted) {
                    if (strcmp(temp_ptr->name, ptr->parent_ptr->augments)== 0) {
                        found++;
                        break;
                    }
                }
                if (!found) {
                    printf("ERROR: %s aguments unknown table %s\n", ptr->name,
                            ptr->parent_ptr->augments);
                } else {
                    /* use the augmented table's info */
                    getinfo = ptr->getinfo =
                        ptr->parent_ptr->getinfo = temp_ptr->getinfo;
                    testinfo = ptr->testinfo =
                        ptr->parent_ptr->testinfo = temp_ptr->testinfo;
                }
            }
        }

        if (!v_elimination_data) {
            fprintf(stderr, "postmosy: -no_v without elimination data\n");
            exit(-1);
        }
        if (flags & ACCESS_READONLY) {
            if (ptr->parent_ptr->getinfo < 0) {
                printf("WARNING: getinfo is uninitialized\n");
            }
            if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
                if (!silent) {
                    printf("Warning: %s: %s overrides readOnly_test\n",
                           ptr->name, ptr->override_v_test);
                }
                fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
            } else if (ptr->override_v_get != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, readOnly_test)\n",
                    ptr->override_v_get);
            } else if (ptr->override_v_test != NULL) {
                if (!silent) {
                    printf("Warning: %s: %s overrides readOnly_test\n",
                           ptr->name, ptr->override_v_test);
                }
                fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], %s)\n",
                    ptr->parent_ptr->getinfo,
                    ptr->override_v_test);
            } else {
                fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], readOnly_test)\n",
                    ptr->parent_ptr->getinfo);
            }
        } else {
            if (ptr->parent_ptr->getinfo < 0) {
                printf("WARNING: getinfo is uninitialized\n");
            }
            if (ptr->parent_ptr->testinfo < 0) {
                printf("WARNING: testinfo is uninitialized\n");
            }
            if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
            } else if (ptr->override_v_get != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, &minv_testinfo[%d])\n",
                    ptr->override_v_get,
                    ptr->parent_ptr->testinfo);
            } else if (ptr->override_v_test != NULL) {
                fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], %s)\n",
                    ptr->parent_ptr->getinfo,
                    ptr->override_v_test);
            } else {
                fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], &minv_testinfo[%d])\n",
                    ptr->parent_ptr->getinfo,
                    ptr->parent_ptr->testinfo);
            }
        }
    } else if (no_v_get) {
        if (!v_elimination_data) {
            fprintf(stderr, "postmosy: cannot eliminate v_get functions without elimination data\n");
            exit(-1);
        }
        if (ptr->parent_ptr->testinfo < 0) {
            printf("WARNING: testinfo is uninitialized\n");
        }
        if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
            if ((flags & ACCESS_READONLY) && !silent) {
                printf("Warning: %s: %s overrides readOnly_test\n",
                       ptr->name, ptr->override_v_test);
            }
            fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
        } else if (ptr->override_v_get != NULL) {
            fprintf(fp,
                    "    OIDaccess(%s, %s_test)\n",
                    ptr->override_v_get,
                    flags & ACCESS_READONLY ? "readOnly" : name);
        } else if (ptr->override_v_test != NULL) {
            if ((flags & ACCESS_READONLY) && !silent) {
                printf("Warning: %s: %s overrides readOnly_test\n",
                       ptr->name, ptr->override_v_test);
            }
            fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], %s)\n",
                    ptr->parent_ptr->testinfo,
                    ptr->override_v_test);
        } else {
            fprintf(fp,
                    "    OIDaccess(&minv_getinfo[%d], %s_test)\n",
                    ptr->parent_ptr->testinfo,
                    flags & ACCESS_READONLY ? "readOnly" : name);
        }
    } else if (no_v_test) {
        if (!v_elimination_data) {
            fprintf(stderr, "postmosy: cannot eliminate v_test functions without elimination data\n");
            exit(-1);
        }
        if (flags & ACCESS_READONLY) {
            if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
                if (!silent) {
                    printf("Warning: %s: %s overrides readOnly_test\n",
                           ptr->name, ptr->override_v_test);
                }
                fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
            } else if (ptr->override_v_get != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, readOnly_test)\n",
                    ptr->override_v_get);
            } else if (ptr->override_v_test != NULL) {
                if (!silent) {
                    printf("Warning: %s: %s overrides readOnly_test\n",
                           ptr->name, ptr->override_v_test);
                }
                fprintf(fp,
                    "    OIDaccess(%s_get, %s)\n",
                    ptr->oid_pre_str,
                    ptr->override_v_test);
            } else {
                fprintf(fp,
                    "    OIDaccess(%s_get, readOnly_test)\n",
                    ptr->oid_pre_str);
            }
        } else {
            if (ptr->parent_ptr->testinfo < 0) {
                printf("WARNING: testinfo is uninitialized\n");
            }
            if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
            } else if (ptr->override_v_get != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s, &minv_testinfo[%d])\n",
                    ptr->override_v_get,
                    ptr->parent_ptr->testinfo);
            } else if (ptr->override_v_test != NULL) {
                fprintf(fp,
                    "    OIDaccess(%s_get, %s)\n",
                    ptr->oid_pre_str,
                    ptr->override_v_test);
            } else {
                fprintf(fp,
                    "    OIDaccess(%s_get, &minv_testinfo[%d])\n",
                    ptr->oid_pre_str,
                    ptr->parent_ptr->testinfo);
            }
        }
    } else /* neither no_v_get nor no_v_test */ {
        if (ptr->override_v_get != NULL && ptr->override_v_test != NULL) {
            if ((flags & ACCESS_READONLY) && !silent) {
                printf("Warning: %s: %s overrides readOnly_test\n",
                       ptr->name, ptr->override_v_test);
            }
            fprintf(fp,
                    "    OIDaccess(%s, %s)\n",
                    ptr->override_v_get,
                    ptr->override_v_test);
        } else if (ptr->override_v_get != NULL) {
            fprintf(fp,
                    "    OIDaccess(%s, %s_test)\n",
                    ptr->override_v_get,
                    flags & ACCESS_READONLY ? "readOnly" : name);
        } else if (ptr->override_v_test != NULL) {
            if ((flags & ACCESS_READONLY) && !silent) {
                printf("Warning: %s: %s overrides readOnly_test\n",
                       ptr->name, ptr->override_v_test);
            }
            fprintf(fp,
                    "    OIDaccess(%s_get, %s)\n",
                    ptr->oid_pre_str,
                    ptr->override_v_test);
        } else {
            fprintf(fp,
                    "    OIDaccess(%s_get, %s_test)\n",
                    ptr->oid_pre_str,
                    flags & ACCESS_READONLY ? "readOnly" : name);
        }
    }
}


/* write the following output files:
 *    snmptype.h
 *    snmpsupp.h
 *    snmpdef.h
 *    snmppart.h
 *    snmpoid.c
 */
int
output_agent_files(base)
	char           *base;
{
	struct OID_INFO *ptr, *temp_ptr, *row_status_ptr;
	struct OID_INFO *g1, *augments_list;
	OID            *oid_ptr;
	long            i;
	char            buf[80];/* screen width */
	char            buf1[80];	/* screen width */
	char            buf2[256];
	FILE           *fp_defs, *fp_type, *fp_part, *fp_oid,
	               *fp_minv, *fp_minv_get, *fp_minv_test,
	               *fp_supp, *fp_prot, *fp_override;
	int             found, valid_count;
	int             supp_count = 0;
	char            type[128];
	struct _index_array *index_array, *temp_index;
	int             idx;
	static char     get_tmpfilename[] = "minvgtmp.c";
	static char     test_tmpfilename[] = "minvttmp.c";
	char            buff[BUFF_SIZE];
        char           *saved_family_name = NULL;
	extern int      last_SrGetInfoEntry_created;
	extern int      last_SrTestInfoEntry_created;

	/* open the files */
        if (snmpoid_only) {
	    fp_defs = open_file(base, "only");
	    fp_type = fp_defs;
	    fp_prot = fp_defs;
	    fp_part = fp_defs;
	    fp_supp = fp_defs;
        } else {
	    fp_defs = open_file(base, "defs.h");
	    fp_type = open_file(base, "type.h");
            if (separate_type_file) {
	        fp_prot = open_file(base, "prot.h");
            } else {
	        fp_prot = fp_type;
            }
	    fp_part = open_file(base, "part.h");
	    fp_supp = open_file(base, "supp.h");
        }
	fp_oid = open_file(base, "oid.c");
        if (override_config_file != NULL) {
            /* open config file */
            fp_override = fopen(override_config_file, "r");
            if (fp_override == NULL) {
                fprintf(stderr, "postmosy: couldn't open %s for reading\n",
                        override_config_file);
                exit(-1);
            }
            while (fgets(buff, sizeof(buff), fp_override) != NULL) {
                /*
                 * deal with DOS crlf on other OSs
                 */
                if ( (strlen(buff) > 1) &&
                     (buff[strlen(buff) - 2] == '\r')) {
                  buff[strlen(buff) - 2] = '\n';
                }

                /* check size of line read in */
                if (strlen(buff) > (unsigned) (BUFF_SIZE - 1)) {
                    fprintf(stderr, "error in %s. Line too long: %s\n", override_config_file, buff);
                    exit(-1);
                }

                /* read an object name and up to two function names */
                i = sscanf(buff, "%s%s%s", buf, buf1, buf2);

                if (i == EOF) continue;             /* empty line */
                if (i == 0) continue;               /* blank line */
                if (buf[0] == '#') continue;        /* comment line */

                /* examine the first string */
                if ((oid_ptr = MakeOIDFromDot(buf)) == NULL) {
                    if (!silent) {
                        printf("MakeOIDFromDot failed: %s\n"
                               "Ignoring input line from %s:\n"
                               "  \"%s\"\n",
                               buf, override_config_file, buff);
                    }
                    continue;
                }

                /* is the MIB object the only thing on the line? */
                if (i == 1) {
                    if (!silent) {
                        printf("No override functions found for MIB object %s\n"
                               "in file %s (ignored)\n",
                               buf, override_config_file);
                    }
                    continue;
                }

                /* find the object in the sorted list */
                for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
                    if (CmpNOID(oid_ptr, ptr->oid_ptr, oid_ptr->length) == 0) {
                        break;
                    }
                }
                if (ptr == NULL) {
                    /*
                     * this should never happen if the MIB object name
                     * was successfully converted to an OID above
                     */
                    if (!silent) {
                        printf("MIB object %s read from file %s\n"
                               "was not found in the sorted OID_INFO list.  "
                               "Ignoring input line:\n"
                               "  \"%s\"\n",
                               buf, override_config_file, buff);
                    }
                    continue;
                }

                /* examine the second string, if any */
                if (i > 1) {
                    if (buf1[0] == '#') {
                        /* comment */
                        if (!silent) {
                            printf("No override functions found for MIB object %s\n"
                                   "in file %s (ignored)\n",
                                   buf, override_config_file);
                        }
                        continue;
                    }
                    /* assume that the second string is a function name */
                    ptr->override_v_get = (char *) malloc(strlen(buf1)+1);
                    if (ptr->override_v_get == NULL) {
                        printf("Could not allocate memory for override_v_get\n");
                        exit(-1);
                    }
                    strcpy(ptr->override_v_get, buf1);
                }

                /* examine the third string, if any */
                if (i > 2) {
                    if (buf2[0] == '#') continue;  /* comment */
                    /* assume that the third string is a function name */
                    ptr->override_v_test = (char *) malloc(strlen(buf2)+1);
                    if (ptr->override_v_test == NULL) {
                        printf("Could not allocate memory for override_v_test\n");
                        exit(-1);
                    }
                    strcpy(ptr->override_v_test, buf2);
                }
            }
            fclose(fp_override);
        }
        if (no_v_get || no_v_test) {
            /*
             * Write information for the SrGetInfoEntry directly into the
             * <base>minv.c file.
             */
            fp_minv = open_file(base, "minv.c");
    
            /*
             * Write information for the SrGetInfoEntry and SrTestInfoEntry
             * arrays into a temporary file.  After all families have been
             * processed, append the contents of both temporary files to
             * <base>minv.c.
             */
            fp_minv_get = fopen(get_tmpfilename, "w+");
            if (fp_minv_get == NULL) {
                fprintf(stderr, "postmosy: couldn't open temp file for writing\n");
                exit(-1);
            }
            fp_minv_test = fopen(test_tmpfilename, "w+");
            if (fp_minv_test == NULL) {
                fprintf(stderr, "postmosy: couldn't open temp file for writing\n");
                exit(-1);
            }
        }

	/* output copyright notices */
	print_copyright(fp_type);
        if (separate_type_file) {
	    print_copyright(fp_prot);
        }
	print_copyright(fp_part);
	print_copyright(fp_oid);
	if (no_v_get || no_v_test) {
	    print_copyright(fp_minv);
	}
	print_copyright(fp_supp);
	print_copyright(fp_defs);

	if (!no_cvs_keywords) {
            print_cvs_keywords(fp_type, H_FILE);
            if (separate_type_file) {
	        print_cvs_keywords(fp_prot, H_FILE);
            }
            print_cvs_keywords(fp_part, H_FILE);
            print_cvs_keywords(fp_oid, C_FILE);
            print_cvs_keywords(fp_supp, H_FILE);
            print_cvs_keywords(fp_defs, H_FILE);
            if (no_v_get || no_v_test) {
                /*
                 * The <base>minv.c file is normally #include'd by another .c
                 * file, which may already have an rcsid array.  So, we use the
                 * H_FILE parameter to put CVS text inside a comment instead of
                 * creating a [possibly] second rcsid array.
                 */
                print_cvs_keywords(fp_minv, H_FILE);
	    }
	}

	print_support_instructions(fp_supp);

        if (no_v_get || no_v_test) {
            /* add header file include for offsetof() */
	    fprintf(fp_minv, "#include \"sr_conf.h\"\n");
	    fprintf(fp_minv, "\n");
	    fprintf(fp_minv, "#ifdef HAVE_STDDEF_H\n");
	    fprintf(fp_minv, "#include <stddef.h>\n");
	    fprintf(fp_minv, "#endif  /* HAVE_STDDEF_H */\n");
	    fprintf(fp_minv, "\n");
        }

	/* output Automatically Generated by mib compiler note */
	fprintf(fp_supp, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	fprintf(fp_supp, " */\n\n");
	fprintf(fp_type, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	fprintf(fp_type, " */\n\n");
        if (separate_type_file) {
	    fprintf(fp_prot, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	    fprintf(fp_prot, " */\n\n");
        }
	fprintf(fp_part, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	fprintf(fp_part, " */\n\n");
	fprintf(fp_oid, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	fprintf(fp_oid, " */\n\n");
	fprintf(fp_defs, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	fprintf(fp_defs, " */\n\n");
	if (no_v_get || no_v_test) {
	    fprintf(fp_minv, "/*\n * automatically generated by the mib compiler -- do not edit\n");
	    fprintf(fp_minv, " */\n\n");
	}

	print_start_protection(fp_type, base, "type");
        if (separate_type_file) {
	    print_start_protection(fp_prot, base, "prot");
        }
	print_start_protection(fp_part, base, "part");
	print_start_protection(fp_supp, base, "supp");
	print_start_protection(fp_defs, base, "defs");


	/* make a macro for the part file which will make it more legible */
	fprintf(fp_part,
	  "/* only define name strings if we are not compiling LIGHT */\n");
	fprintf(fp_part, "#ifndef LIGHT\n");
        if (export_st) {
            fprintf(fp_part, "#ifndef SR_ST%s_EXTERN\n", base);
            if (intel) {
                fprintf(fp_part, "#define STdeclare(name, value) char FAR name [] = value;\n");
            } else if (use_const) {
                fprintf(fp_part, "#define STdeclare(name, value) const char name [] = value;\n");
            } else {
                fprintf(fp_part, "#define STdeclare(name, value) char name [] = value;\n");
            }
            fprintf(fp_part, "#else\t/* SR_ST%s_EXTERN */\n", base);
            if (intel) {
                fprintf(fp_part, "#define STdeclare(name, value) extern char FAR name [];\n");
            } else if (use_const) {
                fprintf(fp_part, "#define STdeclare(name, value) extern const char name [];\n");
            } else {
                fprintf(fp_part, "#define STdeclare(name, value) extern char name [];\n");
            }
            fprintf(fp_part, "#endif\t/* SR_ST%s_EXTERN */\n", base);
        } else {
            if (intel) {
                fprintf(fp_part, "#define STdeclare(name, value) static char FAR name [] = value;\n");
            } else if (use_const) {
                fprintf(fp_part, "#define STdeclare(name, value) static const char name [] = value;\n");
            } else {
                fprintf(fp_part, "#define STdeclare(name, value) static char name [] = value;\n");
            }
        }
	fprintf(fp_part, "#else\t/* LIGHT */\n");
	fprintf(fp_part, "#define STdeclare(name, value)\n");
	fprintf(fp_part, "#endif\t/* LIGHT */\n");
	fprintf(fp_part, "\n");
        fprintf(fp_part, "/* define the OID components of the OID call table */\n");

        fprintf(fp_prot, "#include \"sr_proto.h\"\n");
	if (userpart) {
            fprintf(fp_type, "#include \"userpart.h\"\n");
	}
	fprintf(fp_type, "\n");
	fprintf(fp_prot, "\n");

#define K_INIT_STRING "extern int k_initialize\n\
    SR_PROTOTYPE((void));\n\n"

#define K_PF_INIT_STRING "extern int k_%s_initialize\n\
    SR_PROTOTYPE((void));\n\n"

#define K_TERM_STRING "extern int k_terminate\n\
    SR_PROTOTYPE((void));\n\n"

#define K_PF_TERM_STRING "extern int k_%s_terminate\n\
    SR_PROTOTYPE((void));\n\n"

	if (per_file_init) {
	    fprintf(fp_prot, K_PF_INIT_STRING, base);
	    fprintf(fp_prot, K_PF_TERM_STRING, base);
	}
	else if (k_init) {
            if (row_status_in_file() == 0) {
	        fprintf(fp_prot, K_INIT_STRING);
                if (k_term) {
	            fprintf(fp_prot, K_TERM_STRING);
                }
            }
	}

#define K_CONFIG_PARSE_STRING "extern int parse_%s_config_file\n\
    SR_PROTOTYPE((const char *dir, int save_junk));\n\n"

#define K_CONFIG_WRITE_STRING "extern int write_%s_config_file\n\
    SR_PROTOTYPE((const char *dir));\n\n"

	if (parser) {
	    fprintf(fp_prot, K_CONFIG_PARSE_STRING, base);
	    fprintf(fp_prot, K_CONFIG_WRITE_STRING, base);
	}

	/* make a bunch of macros to improve legibility of the oid file */
	fprintf(fp_oid, "/* this macro defines the oid value within the structure, including a comma */\n");
        if (use_const) {
	    fprintf(fp_oid, "#define OIDvalue(name, value) { name , (SR_UINT32 *) value },\n");
        } else {
	    fprintf(fp_oid, "#define OIDvalue(name, value) { name , value },\n");
        }
	fprintf(fp_oid,
	"/* this macro includes a name string if LIGHT is not defined */\n");
	fprintf(fp_oid, "#ifndef LIGHT\n");
        if (use_const) {
	    fprintf(fp_oid, "#define OIDincludeST(name) (char *) name ,\n");
        } else {
	    fprintf(fp_oid, "#define OIDincludeST(name) name ,\n");
        }
	fprintf(fp_oid, "#else\t/* LIGHT */\n");
	fprintf(fp_oid, "#ifdef EMANATE\n");
	fprintf(fp_oid, "#define OIDincludeST(name) NULL,\n");
	fprintf(fp_oid, "#else\t/* EMANATE*/\n");
	fprintf(fp_oid, "#define OIDincludeST(name)\n");
	fprintf(fp_oid, "#endif\t/* EMANATE */\n");
	fprintf(fp_oid, "#endif\t/* LIGHT */\n");
	fprintf(fp_oid,
		"/* this macro includes the access routines in the OID structure */\n");
	fprintf(fp_oid, "#ifdef SETS\n");
        fprintf(fp_oid, "#define OIDaccess(getr, testr) (GetMethod)getr, (TestMethod)testr\n");
	fprintf(fp_oid, "#else /* SETS */\n");
	fprintf(fp_oid, "#define OIDaccess(getr, testr) (GetMethod)getr, (TestMethod)readOnly_test\n");
	fprintf(fp_oid, "#endif /* SETS */\n");
	fprintf(fp_oid, "\n");
	fprintf(fp_oid, "/* define the oid call table */\n");

        if (no_v_get || no_v_test) {
            if (v_elimination_data) {
                fprintf(fp_minv_get, "SrGetInfoEntry minv_getinfo[] =\n");
                fprintf(fp_minv_get, "{");  /* } */
                fprintf(fp_minv_test, "SrTestInfoEntry minv_testinfo[] =\n");
                fprintf(fp_minv_test, "{");  /* } */
            } else {
                fprintf(fp_minv, "/* we would like 0 elements in these, but that's not legal ANSI C */\n");
                fprintf(fp_minv, "SrGetInfoEntry minv_getinfo[1];\n");
                fprintf(fp_minv, "SrTestInfoEntry minv_testinfo[1];\n");
                fprintf(fp_minv, "\n");
            }
        }

	/* write out the type file and support file information */
	row_status_ptr = NULL;
	for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if ((ptr->family_root == 1) && (ptr->valid)) {

                        /* see if there are any valid entries in this family */
                        valid_count = 0;
                        temp_ptr = ptr->next_family_entry;
                        while (temp_ptr) {
                                if (temp_ptr->valid) {
                                        valid_count++;
                                        break;
                                }
                                temp_ptr = temp_ptr->next_family_entry;
                        }

			/* include a comment to tell the name of the family */

#ifndef NO_AUGMENTS
			if (!ptr->augments && valid_count) {
				fprintf(fp_type, "\n/*\n * The %s family. \n */\n\n", ptr->name);
                                if (separate_type_file) {
				    fprintf(fp_prot, "\n/*\n * The %s family. \n */\n\n", ptr->name);
                                }
			}
#else				/* NO_AUGMENTS */
			if (valid_count) {
				fprintf(fp_type, "\n/*\n * The %s family. \n */\n\n", ptr->name);
                                if (separate_type_file) {
				    fprintf(fp_prot, "\n/*\n * The %s family. \n */\n\n", ptr->name);
                                }
			}
#endif				/* NO_AUGMENTS */


                        /*
                         * Before supp_count gets reset to zero,
                         * generate the definition for I_<family>_max.
                         * 
                         * To print the #define for the last family,
                         * nearly identical code is repeated after the
                         * end of the loop.
                         */
                        if (v_elimination_data && saved_family_name && supp_count > 0) {
                            sprintf(buf, "#define I_%s_max ", saved_family_name);
                            fprintf(fp_supp, "%-60s%d\n", buf, supp_count-1);
                        }

                        /*
                         * print a blank line to seperate the groups in the
                         * support file
                         */
                        if (saved_family_name) {
                            fprintf(fp_supp, "\n");
                        }

#ifdef APO_LEVEL_1
			if (aggregate && ptr->oid_type && (strcmp(ptr->oid_type, "Aggregate") == 0) && valid_count) {
                            /* Entry and Table */
			    fprintf(fp_supp, "#ifdef APO_LEVEL_1\n");
			    sprintf(buf, "#define I_%s ",ptr->parent_ptr->name);
			    fprintf(fp_supp, "%-59s%d\n", buf, -1);
			    sprintf(buf, "#define I_%s ", ptr->name);
			    fprintf(fp_supp, "%-59s%d\n", buf, -1);
			    fprintf(fp_supp, "#endif /* APO_LEVEL_1 */\n");
                        }
#endif /* APO_LEVEL_1 */
#ifdef APO_LEVEL_2
			if (aggregate && ptr->oid_type && ((strcmp(ptr->oid_type, "Struct") == 0) || (strcmp(ptr->oid_type, "Union") == 0)) && valid_count) {
                            /* Entry and Table */
			    fprintf(fp_supp, "#ifdef APO_LEVEL_2\n");
			    sprintf(buf, "#define I_%s ", ptr->name);
			    fprintf(fp_supp, "%-59s%d\n", buf, -1);
			    fprintf(fp_supp, "#endif /* APO_LEVEL_1 */\n");
                        }
#endif /* APO_LEVEL_2 */

                        /*
                         * Some action begins only after the first
                         * family has been completely processed.
                         * Save a pointer to the current family
                         * for use in the next loop.
                         */
                        saved_family_name = ptr->name;

                        /* move on to the next family */

			temp_ptr = ptr->next_family_entry;
			i = 0;
			if (xxx_type) {
				supp_count = 1;
			} else {
				supp_count = 0;
			}
			while (temp_ptr) {

#ifndef NO_AUGMENTS
				/*
				 * if this is an entry which augments another
				 * table, don't print it here, it will be 
				 * printed with the table which it augments
				 */
				if (!ptr->augments) {
					if (temp_ptr->valid) {
						if (strcmp(temp_ptr->oid_access, NOT_ACCESSIBLE_STR) &&
						    strcmp(temp_ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {
							sprintf(buf, "#define I_%s ", temp_ptr->name);
							fprintf(fp_supp, "%-60s%d\n", buf, supp_count++);
						} else {
                                                        /* This entry is not accessible, thus it will not be
							 * put in the dispatch table. But we will put it in
 							 * support file since it must be in the type file and
 							 * since if it is an index, we will need to set it's
							 * valid bits for sets 
							 */
							sprintf(buf, "#define I_%s ", temp_ptr->name);
							fprintf(fp_supp, "%-60s%d\n", buf, supp_count++);
						}
					}
				}
#else				/* NO_AUGMENTS */
				if (temp_ptr->valid) {
					sprintf(buf, "#define I_%s ", temp_ptr->name);
					fprintf(fp_supp, "%-60s%d\n", buf, supp_count++);
				}
#endif				/* NO_AUGMENTS */

				temp_ptr = temp_ptr->next_family_entry;
			}	/* while (temp_ptr) */

#ifndef NO_AUGMENTS
			/*
			 * if this group is augmented, print the items which
			 * augment the group
			 */
			/* to the support file   */
			if (ptr->augmented) {
				for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
					if (g1->augments) {
						if (strcmp(g1->augments, ptr->name) == 0) {
                                                        /* if augments table
                                                         * is read_write or
                                                         * read_create, then
                                                         * set augmented table
                                                         * to match
                                                         */
                                                        if (g1->group_read_write && !ptr->group_read_write) {
                                                            ptr->group_read_write = 1;
                                                        }
                                                        if (g1->group_read_create && !ptr->group_read_create) {
                                                            ptr->group_read_create = 1;
                                                        }

							augments_list = g1->next_family_entry;
							while (augments_list) {
								if (augments_list->valid) {
									sprintf(buf, "#define I_%s ", augments_list->name);
									fprintf(fp_supp, "%-60s%d\n", buf, supp_count++);
								}
								augments_list = augments_list->next_family_entry;
							}
						}
					}
				}
			}
#if 0
			if (cache) {
				fprintf(fp_supp, "#define %s_ENABLE_CACHE\n", ptr->name);
			}
#endif				/* 0 */

#endif				/* NO_AUGMENTS */

			/*
			 * Add an entry in the support file for the
			 * entries in the INDEX clause if those
			 * entries are not already in the structure
			 * (i.e. if the index is in another family).
			 */
			/* NOTE: The same code is used to add index
			 * entries to the <family>_type.h file. The
			 * values in the #defines in this file MUST
			 * match the order in which they are printed
			 * in the <family>_type.h file.
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
					 * file (i.e. if is in
					 * this table).
					 */

					found = 0;
					temp_ptr = ptr->next_family_entry;
					while (temp_ptr) {
						if (strcmp(temp_ptr->name, temp_index->name) == 0)
							found = 1;
						temp_ptr = temp_ptr->next_family_entry;
					}

					/*
					 * if is is not in the
					 * file, then put it in
					 * now
					 */
					if (!found) {
					    sprintf(buf, "#define I_%sIndex_%s ", ptr->name, temp_index->name);
					    fprintf(fp_supp, "%-60s%d\n", buf, supp_count++);
					}
				}	/* for(temp_index... */
			}	/* if(index_array) */

#ifdef FUTURE_THE_OLD_WAY
			temp_ptr = ptr->next_family_entry;
			fprintf(fp_supp, "#if ");
			fprintf(fp_supp, "(defined I_%s)", temp_ptr->name);
			while (temp_ptr = temp_ptr->next_family_entry) {
				fprintf(fp_supp, " \\\n || (defined I_%s)", temp_ptr->name);
			}
			sprintf(buf2, "%s", ptr->name);
			for (j = 0; j < strlen(buf2); j++) {
				if (islower(buf2[j]))
					buf2[j] = toupper(buf2[j]);
			}
			fprintf(fp_supp, "\n#define %sFAMILY\n", buf2);
			fprintf(fp_supp, "#endif\n");
			fprintf(fp_supp, "\n");
#endif				/* FUTURE_THE_OLD_WAY */

#ifndef NO_AUGMENTS
			/*
			 * if this is an entry which augments another table,
			 * don't print it here, it will be printed with the 
			 * table which it augments
			 */
			if (!ptr->augments) {
#endif				/* NO_AUGMENTS */

				if (valid_count) {
					fprintf(fp_type, "\n");
					fprintf(fp_type, "typedef\n");
					fprintf(fp_type, "struct _%s_t {\n", ptr->name);	/* } */
					if (xxx_type) {
						fprintf(fp_type, "        long            len;\n");
					}
				}
#ifndef NO_AUGMENTS
				if (valid_count) {
					if (xxx_type) {
					   fprintf(fp_part, "#ifdef SNMPPART_DECL\n");
					   fprintf(fp_part, "char %sTypes[] = {\n", ptr->name); /*}*/
					}
					temp_ptr = ptr->next_family_entry;
					while (temp_ptr) {
						if (temp_ptr->valid) {
							find_type(type, temp_ptr->name, temp_ptr->oid_prim_type, temp_ptr->size);
							fprintf(fp_type, "        %s;\n", type);
							if(row_status != 0
							&& strcmp(temp_ptr->oid_type, rowStatusString) == 0) {
							    row_status_ptr = temp_ptr;
							}

							if (xxx_type) {
								map_variable_type_for_ber(temp_ptr->oid_prim_type, buf2);
								fprintf(fp_part, "    %s", buf2);
							}
						}
						temp_ptr = temp_ptr->next_family_entry;
						if (xxx_type && temp_ptr) {
						   fprintf(fp_part, ", \n");
						}
					}
					if (xxx_type) {	/* { */
						fprintf(fp_part, "\n};\n");
						fprintf(fp_part, "#else /* SNMPPART_DECL */\n");
						fprintf(fp_part, "extern char %sTypes[];\n", ptr->name);
						fprintf(fp_part, "#endif /* SNMPPART_DECL */\n");
						fprintf(fp_part, "\n");
					}
					/*
					 * if this group is augmented, print
					 * the items which augment the
					 * group to the type file */
					if (ptr->augmented) {
						for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
							if (g1->augments) {
								if (strcmp(g1->augments, ptr->name) == 0) {
									augments_list = g1->next_family_entry;
									while (augments_list) {
										find_type(type, augments_list->name, augments_list->oid_prim_type, augments_list->size);
										fprintf(fp_type, "        %s;\n", type);
										augments_list = augments_list->next_family_entry;
									}
								}
							}
						}
					}
#else				/* NO_AUGMENTS */
				if (valid_count) {
					fprintf(fp_part, "#define %s_types_init { ");	/* } */
					temp_ptr = ptr->next_family_entry;
					while (temp_ptr) 
                                             if (temp_ptr->valid) {
						find_type(type, temp_ptr->name, temp_ptr->oid_prim_type, temp_ptr->size);
						fprintf(fp_type, "        %s;\n", type);

						map_variable_type_for_ber(temp_ptr->oid_prim_type, buf2);
						fprintf(fp_part, "%s", buf2);
					     }

					     temp_ptr = temp_ptr->next_family_entry;
					     if (temp_ptr) {
					         fprintf(fp_part, ", ");
					     }
					}	/* { */
					fprintf(fp_part, " }\n\n");

					if (userpart) {
						fprintf(fp_type, "#ifdef U_%s\n", ptr->name);
						fprintf(fp_type, "U_%s\n", ptr->name);
						fprintf(fp_type, "#endif /* U_%s */\n", ptr->name);
					}
					/* { */
					fprintf(fp_type, "} %s_t;\n\n", ptr->name);
				}
#endif				/* NO_AUGMENTS */

#ifndef NO_AUGMENTS
				/*
				 * Add an entry in the type structure for the
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
							if (strcmp(temp_ptr->name, temp_index->name) == 0)
								found = 1;
							temp_ptr = temp_ptr->next_family_entry;
						}

						/*
						 * if is is not in the
						 * structure, then put it in
						 * now
						 */
						if (!found) {
							fprintf(fp_type, "        %-15s %s;\n",
								temp_index->c_type, temp_index->name);
						}
					}	/* for(temp_index... */
				}	/* if(index_array) */
				if (row_status_ptr != NULL) {
				    fprintf(fp_type, 
					"        long            %sTimerId;\n", 
					row_status_ptr->oid_type);
				    row_status_ptr = NULL;
				}
				if (userpart) {
					fprintf(fp_type, "#ifdef U_%s\n", ptr->name);
					fprintf(fp_type, "U_%s\n", ptr->name);
					fprintf(fp_type, "#endif /* U_%s */\n", ptr->name);
				}
				if (xxx_type) {
					fprintf(fp_type, "        char            *types;\n");
				}
				fprintf(fp_type, "        char            valid[%d];\n",
					((supp_count + 1 + 7) / 8));

                                if (settable_object_in_family(ptr)) {
                                    fprintf(fp_type, "        char            mod[%d];\n",
                                        ((supp_count + 1 + 7) / 8));
                                }
#ifdef APO_LEVEL_1
                                fprintf(fp_type, "#ifdef APO_LEVEL_1\n");
                                fprintf(fp_type, "        char           hints[%d];\n", ((supp_count + 1 + 7) / 8));
                                fprintf(fp_type, "#endif /* APO_LEVEL_1 */\n");
#endif /* APO_LEVEL_1 */

				/* { */
				fprintf(fp_type, "} %s_t;\n\n", ptr->name);

#if 0
                                if (!compute_nominator) {
                                    if (supp_count > 255) {
                                        printf("ERROR: %s contains more than 255 objects, \nvalues in the support file must be less that 255\n", ptr->name);
                                        printf("use command line option -compute_nominator to handle tables with more than 255 columns\n\n");
                                        exit(1);
                                    }
                                }
#endif /* 0 */

			}	/* if (valid_count) */
#else				/* NO_AUGMENTS */
				temp_ptr = ptr->next_family_entry;
				while (temp_ptr) {
#endif				/* NO_AUGMENTS */

#ifndef NO_AUGMENTS

#ifndef FUTURE_OID_AUGMENTS
					if (ptr->augmented) {
						for (g1 = sorted_root; g1; g1 = g1->next_sorted) {
							if (g1->augments) {
								if (strcmp(g1->augments, ptr->name) == 0) {
									fprintf(fp_type, "#define %s_get %s_get\n",
										g1->name, ptr->name);
#ifdef APO_LEVEL_1
                                                                        if (aggregate) {
			                                                  fprintf(fp_type, "#ifdef APO_LEVEL_1\n");
									  fprintf(fp_type, "#define %s_table_get %s_table_get\n",
										g1->name, ptr->name);
									  fprintf(fp_type, "#define %s_table_test %s_table_test\n",
										g1->name, ptr->name);
			                                                  fprintf(fp_type, "#endif /* APO_LEVEL_1 */\n");
                                                                        }
#endif /* APO_LEVEL_1 */
									fprintf(fp_type, "#define %s_test %s_test\n",
										g1->name, ptr->name);
									fprintf(fp_type, "#define %s_set %s_set\n",
										g1->name, ptr->name);
									fprintf(fp_type, "\n#define %s_t %s_t\n", g1->name,
										ptr->name);
									fprintf(fp_prot, "\n");
								}
							}
						}
#endif				/* FUTURE_OID_AUGMENTS */
					}
					temp_ptr = ptr->next_family_entry;
					while (temp_ptr) {
#endif				/* NO_AUGMENTS */

#define _GET_STRING "extern VarBind * %s_get\n\
    SR_PROTOTYPE((OID *incoming,\n\
                  ObjectInfo *object,\n\
                  int searchType,\n\
                  ContextInfo *contextInfo,\n\
                  int serialNum));\n\n"

#ifdef APO_LEVEL_1

#define _GET_TABLE_STRING "extern VarBind * %s_table_get\n\
    SR_PROTOTYPE((OID *incoming,\n\
                  ObjectInfo *object,\n\
                  int searchType,\n\
                  ContextInfo *contextInfo,\n\
                  int serialNum));\n"

#define _TEST_TABLE_STRING "extern int %s_table_test\n\
    SR_PROTOTYPE((OID *incoming,\n\
                  ObjectInfo *object,\n\
                  ObjectSyntax *value,\n\
                  doList_t *doHead,\n\
                  doList_t *doCur,\n\
                  ContextInfo *contextInfo));\n"

#endif /* APO_LEVEL_1 */

#define _FREE_STRING "extern void %s_free\n\
    SR_PROTOTYPE((%s_t *data));\n\n"

#define _TEST_STRING "extern int %s_test\n\
    SR_PROTOTYPE((OID *incoming,\n\
                  ObjectInfo *object,\n\
                  ObjectSyntax *value,\n\
                  doList_t *doHead,\n\
                  doList_t *doCur,\n\
                  ContextInfo *contextInfo));\n\n"

#define _SET_STRING "extern int %s_set\n\
    SR_PROTOTYPE((doList_t *doHead,\n\
                  doList_t *doCur,\n\
                  ContextInfo *contextInfo));\n\n"

#define _READY_STRING "extern int %s_ready\n\
    SR_PROTOTYPE((doList_t *doHead,\n\
                  doList_t *doCur,\n\
                  ContextInfo *contextInfo));\n\n"

#define K_GET_STRING "extern %s_t *k_%s_get\n\
    SR_PROTOTYPE((int serialNum,\n\
                  ContextInfo *contextInfo,\n\
                  int nominator"

#define NEW_K_GET_STRING "extern %s_t *new_k_%s_get\n\
    SR_PROTOTYPE((int serialNum,\n\
                  ContextInfo *contextInfo,\n\
                  int nominator,\n\
                  int searchType,\n\
                  %s_t *%sData));\n\n"

#define _CLEANUP_STRING "extern int %s_cleanup\n\
    SR_PROTOTYPE((doList_t *trash));\n\n"

						/*
						 * write out the function
						 * declarations
  						 */

                                                /* see if this is a table */
                                                idx = get_index_array(ptr->name, &index_array);

						if (valid_count) {
							if (individual) {
								fprintf(fp_prot, "#if !defined(F_%s)\n", temp_ptr->name);
							}
                                                        fprintf(fp_prot, _GET_STRING, ptr->name);
                                                        if ((no_v_get || no_v_test) && v_elimination_data && IsReadableFamily(ptr)) {
                                                            emit_SrGetInfoEntry_element(fp_minv_get, ptr);
                                                            fprintf(fp_minv, "extern const SnmpType %sTypeTable[];\n", ptr->name);
                                                            fprintf(fp_minv, "extern const SrIndexInfo %sIndexInfo[];\n", ptr->name);
                                                        }
#ifdef APO_LEVEL_1
                                                        if (aggregate && (idx != 0 || isStructOrUnion(ptr->oid_type))) {
			                                    fprintf(fp_prot, "#ifdef APO_LEVEL_1\n");
                                                            if (!isStructOrUnion(ptr->oid_type)) {
                                                                fprintf(fp_prot, _GET_TABLE_STRING, ptr->name);
                                                            }
			                                    fprintf(fp_prot, "#endif /* APO_LEVEL_1 */\n");
			                                    fprintf(fp_prot, "\n");
                                                        }
#endif /* APO_LEVEL_1 */
							if (ptr->group_read_write) {
								fprintf(fp_prot, _TEST_STRING, ptr->name);
#ifdef APO_LEVEL_1
                                                                if (!isStructOrUnion(ptr->oid_type) && idx != 0) {
								    fprintf(fp_prot, "#ifdef APO_LEVEL_1\n");
								    fprintf(fp_prot, _TEST_TABLE_STRING, ptr->name);
								    fprintf(fp_prot, "#endif /* APO_LEVEL_1 */\n");
			                                            fprintf(fp_prot, "\n");
                                                                }
#endif /* APO_LEVEL_1 */
								fprintf(fp_prot, _READY_STRING, ptr->name);
								fprintf(fp_prot, _SET_STRING, ptr->name);
								fprintf(fp_prot, _FREE_STRING, ptr->name, ptr->name);
								/* fprintf(fp_prot, _CLEANUP_STRING, ptr->name);*/ 
								/* 
								 * the cleanup function is static to the v_ file and 
								 * consequently is not put into the type file.  its 
								 * prototype is only in the v_ file 
								 */
                                                            if ((no_v_get || no_v_test) && v_elimination_data) {
                                                                emit_SrTestInfoEntry_element(fp_minv_test, ptr);
                                                            }
							}
                                                        if (new_k_stubs || new_k_wrappers) {
                                                            fprintf(fp_prot, NEW_K_GET_STRING, ptr->name, ptr->name, ptr->name, ptr->name);
                                                        }
                                                        if ((!new_k_stubs) || new_k_wrappers) {
                                                            fprintf(fp_prot, K_GET_STRING, ptr->name, ptr->name);
                                                            if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
                                                                fprintf(fp_prot, ",\n                  int searchType");
                                                            }
                                                            if (var_param_list) {
                                                                fprintf(fp_prot, ",\n                  ...));\n\n");
                                                            } else {
                                                                temp_index = index_array;
                                                                while (temp_index && temp_index->name) {
                                                                    fprintf(fp_prot, ",\n                  %s %s",
                                                                    temp_index->c_type, temp_index->name);
                                                                    temp_index++;
                                                                }
                                                                fprintf(fp_prot, "));\n\n");
                                                            }
                                                        }
#define K_TEST_STRING "extern int k_%s_test\n\
    SR_PROTOTYPE((ObjectInfo *object,\n\
                  ObjectSyntax *value,\n\
                  doList_t *dp,\n\
                  ContextInfo *contextInfo));\n\n"

#define K_SET_STRING "extern int k_%s_set\n\
    SR_PROTOTYPE((%s_t *data,\n\
                  ContextInfo *contextInfo,\n\
                  int function));\n\n"

#define K_READY_STRING "extern int k_%s_ready\n\
    SR_PROTOTYPE((ObjectInfo *object,\n\
                  ObjectSyntax *value,\n\
                  doList_t *doHead,\n\
                  doList_t *dp));\n\n"

#define K_UNDO_STRING "extern int %s_undo\n\
    SR_PROTOTYPE((doList_t *doHead,\n\
                  doList_t *doCur,\n\
                  ContextInfo *contextInfo));\n\n"

#define K_SET_DEFAULTS_STRING "extern int k_%s_set_defaults\n\
    SR_PROTOTYPE((doList_t *dp));\n\n"

#define CLONE_STRING "extern %s_t *Clone_%s\n\
    SR_PROTOTYPE((%s_t *data));\n\n"

							if (ptr->group_read_write) {
                                                                fprintf(fp_prot, K_TEST_STRING, ptr->name);
                                                                fprintf(fp_prot, K_SET_STRING, ptr->name, ptr->name);
                                                                fprintf(fp_prot, K_READY_STRING, ptr->name);
								if (undo) {
                                                                        fprintf(fp_prot, K_UNDO_STRING, ptr->name);
								}
                                                                fprintf(fp_prot, CLONE_STRING, ptr->name, ptr->name, ptr->name);
							}
							if ((ptr->group_read_create) || (ptr->group_read_write)) {
                                                                if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
                                                                    if (xxx_read_create) {    /* xxx_read_create defined if "-read-create" is used */
                                                                        fprintf(fp_prot, "#define %s_READ_CREATE\n",ptr->name);
                                                                    }
                                                                    fprintf(fp_prot, K_SET_DEFAULTS_STRING, ptr->name);
								}
							}
							if (userpart && ptr->group_read_write) {
                                                                fprintf(fp_prot, "extern void k_%sCloneUserpartData\n", ptr->name);
                                                                fprintf(fp_prot, "    SR_PROTOTYPE((%s_t *dst, %s_t *src));\n\n", ptr->name, ptr->name);

                                                                fprintf(fp_prot, "extern void k_%sFreeUserpartData\n", ptr->name);
                                                                fprintf(fp_prot, "    SR_PROTOTYPE((%s_t *data));\n\n", ptr->name);
							}
#define IND_GET_STRING "extern int %s_get\n\
    SR_PROTOTYPE((OID *incoming,\n\
                  ObjectInfo *object,\n\
                  int searchType,\n\
                  ContextInfo *contextInfo,\n\
                  int serialNum));\n\n"

							if (individual) {
								fprintf(fp_prot, "#else\t/* !defined(F_%s) */\n", temp_ptr->name);
                                                                fprintf(fp_prot, IND_GET_STRING, temp_ptr->name);
								if (temp_ptr->read_write) {
                                                                        fprintf(fp_prot, _TEST_STRING, temp_ptr->name);
								}
                                                                fprintf(fp_prot, _SET_STRING, temp_ptr->name);
                                                                fprintf(fp_prot, K_GET_STRING, temp_ptr->name, temp_ptr->name
);
								if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
                                                                        fprintf(fp_prot, ",\n                  int");
								}
								if (var_param_list) {
                                                                        fprintf(fp_prot, ",\n                  ...));\n\n");
								} else {
									fprintf(fp_prot, "%s))\n", get_index_type_from_table(ptr->name));
								}
                                                                fprintf(fp_prot, K_SET_STRING, temp_ptr->name, temp_ptr->name);
								fprintf(fp_prot, "#endif\t/* !defined(F_%s) */\n", temp_ptr->name);
							}	/* end if(individual) */
							fprintf(fp_prot, "\n");
						}	/* if (valid_count) */
                                                if (!individual) {
                                                        temp_ptr = NULL;
                                                } else {
                                                        temp_ptr = temp_ptr->next_family_entry;
                                                }

#ifndef NO_AUGMENTS
					}
				}	/* if (!ptr->augments) */
#else				/* NO_AUGMENTS */
					}
#endif				/* NO_AUGMENTS */

		}	/* if (ptr->family_root...) */
	}


        /*
         * This is nearly identical to some code inside the loop.  It
         * is here to print the #define I_<family>_max line for the
         * last family.
         */
        if (v_elimination_data && saved_family_name && supp_count > 0) {
            sprintf(buf, "#define I_%s_max ", saved_family_name);
            fprintf(fp_supp, "%-60s%d\n", buf, supp_count-1);
        }

        /*
         * print a blank line to seperate the groups in the
         * support file
         */
        fprintf(fp_supp, "\n");

		/*
		 * go through the list of oids and write
		 * information to various files
		 */
		for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
			if (ptr->valid) {

#ifdef APO_LEVEL_1
                            if (aggregate) {
                                /* any valid entries in this family? */
                                valid_count = 0;
                                temp_ptr = ptr->next_family_entry;
                                while (temp_ptr) {
                                    if (temp_ptr->valid) {
                                            valid_count++;
                                            break;
                                    }
                                    temp_ptr = temp_ptr->next_family_entry;
                                }
                                idx = get_index_array(ptr->name, &index_array);
#if 0 
                                if (!ptr->family_root) {
                                    temp_ptr = ptr->next_sorted->next_family_entry;
                                    while (temp_ptr) {
                                        if (temp_ptr->valid) {
                                                valid_count++;
                                                break;
                                        }
                                        temp_ptr = temp_ptr->next_family_entry;
                                    }
                                }
#else /* 0 */
                                if (!ptr->family_root) {
                                    valid_count++;
                                }
#endif /* 0 */
                            }
#endif /* APO_LEVEL_1 */

#ifdef VERBOSE
				if (!silent) {
					printf("%s %s %s %s %s\n", ptr->name, ptr->oid_pre_str, ptr->oid_post_str, ptr->oid_type, ptr->oid_access);
				}
#endif				/* VERBOSE */

#if 0
				fprintf(stdout, "name '%s', oprs = '%s', opos ='%s'\n", ptr->name,
					ptr->oid_pre_str, ptr->oid_post_str);
				fprintf(stdout, "type '%s', access '%s'\n", ptr->oid_type, ptr->oid_access);
#endif

				/*
				 * write the data to the snmpdefs file
				 */
				/*
				 * if there are enumerations,
				 * print them
				 */
				if (ptr->enumer) {
					temp_enum = ptr->enumer;
					fprintf(fp_defs, "/* enumerated values for %s */\n", ptr->name);
					while (temp_enum) {
						sprintf(buf2, "D_%s_%s", ptr->name, temp_enum->cname);
						if (short_names) {
							if (strlen(ptr->name) > (unsigned) 30) {
								printf("name too long for use with -short option: %s\n", ptr->name);
								exit(-1);
							}
							check_defs_size(buf2, temp_enum, ptr->name);
						}
						if (D_long) {
							fprintf(fp_defs, "#define %-30s %dL\n", buf2, temp_enum->val);
						} else {
							fprintf(fp_defs, "#define %-30s %d\n", buf2, temp_enum->val);
						}

						temp_enum = temp_enum->next;
					}
					fprintf(fp_defs, "\n");
				}
				/*
				 * write out the part file
				 * information
				 */
				    fprintf(fp_part, "\n");
				    if (ptr->oid_access == NULL) {
					fprintf(fp_part, "#if defined(N_%s)\n", ptr->name);
					fprintf(fp_part, "#ifdef SNMPPART_DECL\n");
					if (intel) {
					    fprintf(fp_part, "SR_UINT32 FAR ID%s[] = { ", ptr->name); /* } */
					} else if (use_const) {
					    fprintf(fp_part, "const SR_UINT32 ID%s[] = { ", ptr->name); /* } */
					} else {
					    fprintf(fp_part, "SR_UINT32 ID%s[] = { ", ptr->name); /* } */
					}
					oid_ptr = ptr->oid_ptr;
					i = 0;
					while (i < oid_ptr->length) {
#if SIZEOF_INT == 4
						fprintf(fp_part, "%u", oid_ptr->oid_ptr[i]);
#else /* SIZEOF_INT == 4 */
						fprintf(fp_part, "%lu", oid_ptr->oid_ptr[i]);
#endif /* SIZEOF_INT == 4 */
						i++;
						if (i == oid_ptr->length) {
							break;	/* exit the loop */
						} else {
							fprintf(fp_part, ", ");
						}
					}
					/* { */
					fprintf(fp_part, " };\n");
					fprintf(fp_part, "#else\n");
					if (intel) {
					    fprintf(fp_part, "extern SR_UINT32 ID%s[];\n", ptr->name);
					} else if (use_const) {
					    fprintf(fp_part, "extern const SR_UINT32 ID%s[];\n", ptr->name);
					} else {
					    fprintf(fp_part, "extern SR_UINT32 ID%s[];\n", ptr->name);
					}
					fprintf(fp_part, "#endif\t/* SNMPPART_DECL */\n");
					fprintf(fp_part, "#define LN%s %ld\n", ptr->name, i);
					fprintf(fp_part, "#endif\t/* defined(N_%s) */\n", ptr->name);
					fprintf(fp_part, "\n");
#ifdef APO_LEVEL_1
                                    } else if (aggregate && isAggregateType(ptr->oid_type)) {
					fprintf(fp_part, "#ifdef APO_LEVEL_1\n");
					fprintf(fp_part, "#if defined(I_%s)\n", ptr->name);
					fprintf(fp_part, "#ifdef SNMPPART_DECL\n");
					fprintf(fp_part, "STdeclare(ST%s, \"%s\")\n", ptr->name, ptr->name);
					if (intel) {
					    fprintf(fp_part, "SR_UINT32 FAR ID%s[] = { ", ptr->name);	/* } */
					} else if (use_const) {
					    fprintf(fp_part, "const SR_UINT32 ID%s[] = { ", ptr->name);	/* } */
					} else {
					    fprintf(fp_part, "SR_UINT32 ID%s[] = { ", ptr->name);	/* } */
					}
					oid_ptr = ptr->oid_ptr;
					i = 0;
					while (i < oid_ptr->length) {
#if SIZEOF_INT == 4
						fprintf(fp_part, "%u", oid_ptr->oid_ptr[i]);
#else /* SIZEOF_INT == 4 */
						fprintf(fp_part, "%lu", oid_ptr->oid_ptr[i]);
#endif /* SIZEOF_INT == 4 */
						i++;
						if (i == oid_ptr->length) {
							break;	/* exit the loop */
						} else {
							fprintf(fp_part, ", ");
						}
					}
#if 0
                                        /* if it's a row, add a .0 */
                                        if (ptr->family_root == 1) {
					    fprintf(fp_part, ", 0");
                                            i++;
                                        } else { /* it's a table, do nothing */
                                        }
#else /* 0 */
                                        if (idx != 0 || !isStructOrUnion(ptr->oid_type)) {
                                            /* add a .0 unless it's a struct
                                               or union that is not nested */
					    fprintf(fp_part, ", 0");
                                            i++;
                                        }
#endif /* 0 */

					/* { */
					fprintf(fp_part, " };\n");
					fprintf(fp_part, "#else\n");
					if (intel) {
					    fprintf(fp_part, "extern SR_UINT32 FAR ID%s[];\n", ptr->name);
					} else if (use_const) {
					    fprintf(fp_part, "extern const SR_UINT32 ID%s[];\n", ptr->name);
					} else {
					    fprintf(fp_part, "extern SR_UINT32 ID%s[];\n", ptr->name);
					}
					if (export_st) {
					    fprintf(fp_part, "#ifdef SR_ST%s_EXTERN\n", base);
					    fprintf(fp_part, "STdeclare(ST%s, \"%s\")\n", ptr->name, ptr->name);
					    fprintf(fp_part, "#endif\t/* SR_ST%s_EXTERN */\n", base);
					}
					fprintf(fp_part, "#endif\t/* SNMPPART_DECL */\n");
					fprintf(fp_part, "#define LN%s %ld\n", ptr->name, i);
					fprintf(fp_part, "#endif\t/* defined(I_%s) */\n", ptr->name);
					fprintf(fp_part, "#endif /* APO_LEVEL_1 */\n");
					fprintf(fp_part, "\n");

#endif /* APO_LEVEL_1 */
				    } else if (strcmp(ptr->oid_access, NOT_ACCESSIBLE_STR) &&
				               strcmp(ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {
					fprintf(fp_part, "#if defined(I_%s)\n", ptr->name);
					fprintf(fp_part, "#ifdef SNMPPART_DECL\n");
					fprintf(fp_part, "STdeclare(ST%s, \"%s\")\n", ptr->name, ptr->name);
					if (intel) {
					    fprintf(fp_part, "SR_UINT32 FAR ID%s[] = { ", ptr->name);	/* } */
					} else if (use_const) {
					    fprintf(fp_part, "const SR_UINT32 ID%s[] = { ", ptr->name);	/* } */
					} else {
					    fprintf(fp_part, "SR_UINT32 ID%s[] = { ", ptr->name);	/* } */
					}
					oid_ptr = ptr->oid_ptr;
					i = 0;
					while (i < oid_ptr->length) {
#if SIZEOF_INT == 4
						fprintf(fp_part, "%u", oid_ptr->oid_ptr[i]);
#else /* SIZEOF_INT == 4 */
						fprintf(fp_part, "%lu", oid_ptr->oid_ptr[i]);
#endif /* SIZEOF_INT == 4 */
						i++;
						if (i == oid_ptr->length) {
							break;	/* exit the loop */
						} else {
							fprintf(fp_part, ", ");
						}
					}
					/* { */
					fprintf(fp_part, " };\n");
					fprintf(fp_part, "#else\n");
					if (intel) {
					    fprintf(fp_part, "extern SR_UINT32 FAR ID%s[];\n", ptr->name);
					} else if (use_const) {
					    fprintf(fp_part, "extern const SR_UINT32 ID%s[];\n", ptr->name);
					} else {
					    fprintf(fp_part, "extern SR_UINT32 ID%s[];\n", ptr->name);
					}
					if (export_st) {
					    fprintf(fp_part, "#ifdef SR_ST%s_EXTERN\n", base);
					    fprintf(fp_part, "STdeclare(ST%s, \"%s\")\n", ptr->name, ptr->name);
					    fprintf(fp_part, "#endif\t/* SR_ST%s_EXTERN */\n", base);
					}
					fprintf(fp_part, "#endif\t/* SNMPPART_DECL */\n");
					fprintf(fp_part, "#define LN%s %ld\n", ptr->name, i);
					fprintf(fp_part, "#endif\t/* defined(I_%s) */\n", ptr->name);
					fprintf(fp_part, "\n");
				    }

#ifdef APO_LEVEL_1
                                /* if it's a not-accessible aggregate (row or
                                 * table)
                                 */
				if (aggregate && ptr->oid_access != NULL &&
				    (strcmp(ptr->oid_access, 
                                            NOT_ACCESSIBLE_STR) == 0) &&
                                     (isAggregateType(ptr->oid_type)) &&
                                    valid_count) {
                                        if (ptr->family_root == 1) { /* row */
                                            if (no_row_aggs) {
					        fprintf(fp_oid, "#undef I_%s\n",
                                                                 ptr->name);
                                            }
                                        } else { /* table */
                                            if (no_table_aggs) {
					        fprintf(fp_oid, "#undef I_%s\n",
                                                                 ptr->name);
                                            }
                                        }
					fprintf(fp_oid, "#ifdef APO_LEVEL_1\n");
					fprintf(fp_oid, "#if defined(I_%s)\n", ptr->name);
					fprintf(fp_oid, "{\n");	/* } */
					fprintf(fp_oid, "    OIDvalue(LN%s, ID%s)\n", ptr->name, ptr->name);
					fprintf(fp_oid, "    OIDincludeST(ST%s)\n", ptr->name);
                                        /* if it's a row */
                                        if (ptr->family_root == 1) {
                                            if (ptr->group_read_create) {
					        fprintf(fp_oid, "    ROW_TYPE, SR_READ_CREATE,");
                                            } else if (ptr->group_read_write) {
					        fprintf(fp_oid, "    ROW_TYPE, SR_READ_WRITE,");
                                            } else {
					        fprintf(fp_oid, "    ROW_TYPE, SR_READ_ONLY,");
                                            }
                                        } else {  /* it's a table */
                                            OID_INFO_t *row;
                                            static char *fq;
#ifdef APO_LEVEL_2
                                            if (ptr->parent_ptr &&
                                                 isAggregateType
                                                 (ptr->parent_ptr->oid_type)) {
                                                /* tables within tables have
                                                 * and index are are therfore
                                                 * not FULLY_QUALIFIED */
                                                fq = "";
                                            } else {
                                                /* top leve tables are 
                                                 * FULLY_QUALIFIED */
                                                fq = " | FULLY_QUALIFIED";
                                            }
#else /* APO_LEVEL_2 */
                                            fq = " | FULLY_QUALIFIED";
#endif /* APO_LEVEL_2 */
                                            row = ptr->next_sorted;
                                            if (row->group_read_create) {
					        fprintf(fp_oid, "    TABLE_TYPE, SR_READ_CREATE%s,", fq);
                                            } else if (row->group_read_write) {
					        fprintf(fp_oid, "    TABLE_TYPE, SR_READ_WRITE%s,", fq);
                                            } else {
					        fprintf(fp_oid, "    TABLE_TYPE, SR_READ_ONLY%s,", fq);
                                            }
                                        }
					sprintf(buf, "I_%s", ptr->name);
					fprintf(fp_oid, " 0,");

                                        fprintf(fp_oid, " %d,", priority);
					fprintf(fp_oid, "\n");

                                        /* if it's a row */
                                        if (ptr->family_root == 1) {
                                            if (ptr->group_read_write) {
                                                /*
                                                 * %%% DSS: allow -override_v
                                                 * to work here??
                                                 */
                                                fprintf(fp_oid, "    OIDaccess(%s_get, %s_test)\n", ptr->name, ptr->name);
                                            } else {
                                                /*
                                                 * %%% DSS: allow -override_v
                                                 * to work here??
                                                 */
                                                fprintf(fp_oid, "    OIDaccess(%s_get, readOnly_test)\n", ptr->name);
                                            }
                                        } else {  /* it's a table */
                                            if (ptr->next_sorted->group_read_write) {
                                                /*
                                                 * %%% DSS: allow -override_v
                                                 * to work here??
                                                 */
                                                fprintf(fp_oid, "    OIDaccess(%s_table_get, %s_table_test)\n", ptr->next_sorted->name, ptr->next_sorted->name);
                                            } else {
                                                /*
                                                 * %%% DSS: allow -override_v
                                                 * to work here??
                                                 */
                                                fprintf(fp_oid, "    OIDaccess(%s_table_get, readOnly_test)\n", ptr->next_sorted->name);
                                            }
                                        }
					fprintf(fp_oid, "},\n");
					fprintf(fp_oid, "#endif\t/* defined(I_%s) */\n", ptr->name);
					fprintf(fp_oid, "#endif /* APO_LEVEL_1 */\n");
					fprintf(fp_oid, "\n");
                                }
#endif /* APO_LEVEL_1 */

                                /* if it's an accessible leaf object */
				if (ptr->oid_access != NULL &&
				    strcmp(ptr->oid_access, NOT_ACCESSIBLE_STR) &&
				    strcmp(ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {

					/*
					 * write out the oid
					 * file information
					 */
					fprintf(fp_oid, "#if defined(I_%s)\n", ptr->name);
					fprintf(fp_oid, "{\n");	/* } */
					fprintf(fp_oid, "    OIDvalue(LN%s, ID%s)\n", ptr->name, ptr->name);
					fprintf(fp_oid, "    OIDincludeST(ST%s)\n", ptr->name);
					if (strcmp(ptr->oid_access, READ_ONLY_STR) == 0) {
						strcpy(buf1, "SR_READ_ONLY");
					} else if (strcmp(ptr->oid_access, READ_WRITE_STR) == 0) {
						strcpy(buf1, "SR_READ_WRITE");
						/*
						 * write-only
						 * is
						 * provided
						 * for
						 * compatability
						 * with
						 * older
						 * SNMPv1
						 * MIBs
						 * only. It
						 * should not
						 * be used
						 * with newly
						 * defined
						 * MIBs
						 */
					} else if (strcmp(ptr->oid_access, WRITE_ONLY_STR) == 0) {
						strcpy(buf1, "SR_WRITE_ONLY");

					} else if (strcmp(ptr->oid_access, READ_CREATE_STR) == 0) {
						strcpy(buf1, "SR_READ_CREATE");
					} else {

#if 1
						fprintf(stderr, "%s: output_file: unknown access type %s for %s\n",
							prog_name, ptr->oid_access, ptr->name);
						exit(-1);
#endif

						continue;
					}
					map_variable_type_for_ber(ptr->oid_prim_type, buf);
					fprintf(fp_oid, "    %s, %s,", buf, buf1);
                                        if (compute_nominator || (ptr->parent_ptr->num_entries > MAX_COLUMNS)) {
					    fprintf(fp_oid, " %s,", "255");
                                        } else {
					sprintf(buf, "I_%s", ptr->name);
					fprintf(fp_oid, " %s,",
						(strchr(ptr->oid_post_str, '.') == NULL) ? buf : "0");
                                        }

                                        fprintf(fp_oid, " %d,", priority);
					fprintf(fp_oid, "\n");

					if (individual) {
						fprintf(fp_oid, "#if !defined(F_%s)\n", ptr->name);
						if (ptr->read_write) {
							emit_oid_access(fp_oid, ptr, ACCESS_FAMILY);
						} else {
							emit_oid_access(fp_oid, ptr, ACCESS_READONLY | ACCESS_FAMILY);
						}
						
						fprintf(fp_oid, "#else\t/* !defined(F_%s) */\n", ptr->name);
						if (strcmp(ptr->oid_access, READ_ONLY_STR) == 0) {
							emit_oid_access(fp_oid, ptr, 0);
						} else {
							emit_oid_access(fp_oid, ptr, ACCESS_READONLY);
						}

						fprintf(fp_oid, "#endif\t/* !defined(F_%s) */\n", ptr->name);
						/* { */
						fprintf(fp_oid, "}\n");
						fprintf(fp_oid, "#endif\t/* defined(I_%s) */\n", ptr->name);
						fprintf(fp_oid, "\n");
					} else {	/* individual */
						if (ptr->read_write) {
							emit_oid_access(fp_oid, ptr, ACCESS_FAMILY);
						} else {
							emit_oid_access(fp_oid, ptr, ACCESS_FAMILY | ACCESS_READONLY);
						}
						/* { */
						fprintf(fp_oid, "},\n");
						fprintf(fp_oid, "#endif\t/* defined(I_%s) */\n", ptr->name);
						fprintf(fp_oid, "\n");
					}
				}
			}
                }

		fprintf(fp_supp, "#include \"undefs.h\"\n");
		fprintf(fp_supp, "\n");

        if (xxx_type) {
           int count;
           fprintf(fp_part, "#ifdef SNMPPART_DECL\n");
           fprintf(fp_part, "FamilyVADescr_t VA_Families[] = {\n");
	   for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if ((ptr->family_root == 1) && (ptr->valid)) {
                        count = 0;
			temp_ptr = ptr->next_family_entry;
			while (temp_ptr) {
                                count++;
				temp_ptr = temp_ptr->next_family_entry;
                        }
                        fprintf(fp_part, "    { %d, %sTypes },\n", count, ptr->name);
            	}
           }
           fprintf(fp_part, "};\n");
           fprintf(fp_part, "#else /* SNMPPART_DECL */\n");
           fprintf(fp_part, "FamilyVADescr_t VA_Families[];\n");
           fprintf(fp_part, "#endif /* SNMPPART_DECL */\n\n");

#if 0
	   for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if ((ptr->family_root == 1) && (ptr->valid)) {
                        idx = get_index_array(ptr->name, &index_array);
                        if (idx != 0) {
                           fprintf(fp_part, "IDDescr_t ID_%s[] = {\n", ptr->name);
                           temp_index = index_array;
                           while (temp_index && temp_index->name) {
                               map_variable_type_for_ber(temp_index->asn1_type, buf2);
                               fprintf(fp_part, "    { %s, %d, %d },\n", buf2, 0, 0);
                               temp_index++;
                           }
                           fprintf(fp_part, "};\n");
                        }
            	}
            }
#endif /* 0 */
        }

	/*
	 * print #endif for multiple inclusion
	 * protection
	 */
	print_end_protection(fp_type, base, "type");
        if (separate_type_file) {
	    print_end_protection(fp_prot, base, "prot");
        }
	print_end_protection(fp_part, base, "part");
	print_end_protection(fp_supp, base, "supp");
	print_end_protection(fp_defs, base, "defs");


        if (no_v_get || no_v_test) {
            /*
             * finish creating <base>minv.c:
             *    terminate the SrGetInfoEntry initializer
             *    terminate the SrTestInfoEntry initializer
             *    append the contents of the temporary file
             *    instantiate the global pointers to last elements
             */
            if (v_elimination_data) {
                /* terminate the SrGetInfoEntry initializer */
                if (last_SrGetInfoEntry_created < 0) {
                    printf("WARNING: empty SrGetInfoEntry initializer\n");
                }
                /* { */
                fprintf(fp_minv_get, "\n};\n\n");
    
                /* terminate the SrTestInfoEntry initializer */
                if (last_SrTestInfoEntry_created < 0) {
                    printf("WARNING: empty SrTestInfoEntry initializer\n");
                }
                /* { */
                fprintf(fp_minv_test, "\n};\n\n");
    
    #ifndef STUDY_TEMPORARY_FILES
                /* append the contents of the temporary files */
                fprintf(fp_minv, "\n");
                rewind(fp_minv_get);
                while (fgets(buff, sizeof(buff), fp_minv_get) != NULL) {
                    fprintf(fp_minv, "%s", buff);
                }
                rewind(fp_minv_test);
                while (fgets(buff, sizeof(buff), fp_minv_test) != NULL) {
                    fprintf(fp_minv, "%s", buff);
                }
    #endif /* STUDY_TEMPORARY_FILES */
    
                /* instantiate the global pointers to last elements */
                fprintf(fp_minv, "SrGetInfoEntry *minv_getinfo_last = ");
                if (last_SrGetInfoEntry_created < 0) {
                    fprintf(fp_minv, "NULL;\n");
                } else {
                    fprintf(fp_minv, "&minv_getinfo[%d];\n", last_SrGetInfoEntry_created+1);
                }
                fprintf(fp_minv, "SrTestInfoEntry *minv_testinfo_last = ");
                if (v_elimination_data) {
                    if (last_SrTestInfoEntry_created < 0) {
                        fprintf(fp_minv, "NULL;\n");
                    } else {
                        fprintf(fp_minv, "&minv_testinfo[%d];\n", last_SrTestInfoEntry_created+1);
                    }
                } else {
                    /*
                     * There is no SrTestInfoEntry array,
                     * so instantiate a dummy global pointer
                     */
                    fprintf(fp_minv, "NULL;\n");
                }
            } else {
                /*
                 * There are no SrGetInfoEntry or SrTestInfoEntry arrays,
                 * so instantiate dummy global pointers
                 */
                fprintf(fp_minv, "SrGetInfoEntry *minv_getinfo_last = NULL;\n");
                fprintf(fp_minv, "SrTestInfoEntry *minv_testinfo_last = NULL;\n");
            }
            fprintf(fp_minv, "\n");
        }


	/* close all of the files */
        if (snmpoid_only) {
            char *base2;
	    fclose(fp_defs);
	    base2 = (char *)malloc(strlen(base) + 16);
	    if (base2 != NULL) {
	        strcpy(base2, base);
	        strcat(base2, "only");
	        remove(base2);
	        free(base2);
	    }
        } else {
	    fclose(fp_part);
	    fclose(fp_supp);
	    fclose(fp_type);
            if (separate_type_file) {
	        fclose(fp_prot);
            }
	    fclose(fp_defs);
        }
	fclose(fp_oid);
	if (no_v_get || no_v_test) {
	    fclose(fp_minv);
#ifndef STUDY_TEMPORARY_FILES
            fclose(fp_minv_get);
            unlink(get_tmpfilename);
            fclose(fp_minv_test);
            unlink(test_tmpfilename);
#endif /* STUDY_TEMPORARY_FILES */
	}

        if (late) {
	    fp_oid = open_file(base, "oid.c");
	    print_copyright(fp_oid);
	    if (!no_cvs_keywords) {
	      print_cvs_keywords(fp_oid, C_FILE);
	    }
	    fclose(fp_oid);

	    fp_part = open_file(base, "part.h");
	    print_copyright(fp_part);
	    if (!no_cvs_keywords) {
	      print_cvs_keywords(fp_oid, H_FILE);
	    }
	    fclose(fp_part);
        }
        return 1;
}

