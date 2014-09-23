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

/*
 * write a <base>cfg.c file which contains code and data structures
 * for performing configuration file I/O.
 */

int
write_configio_file(base)
    char           *base;
{
    char                  buff[100];
    FILE                 *fp;
    struct OID_INFO      *ptr, *temp_ptr;
    struct _index_array  *index_array;

    sprintf(buff, "%scfg", base);
    fp = open_file(buff, ".c");
    print_copyright(fp);

    if (!no_cvs_keywords) {
        print_cvs_keywords(fp, C_FILE);
    }

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
    fprintf(fp, "#ifdef HAVE_STDDEF_H\n");
    fprintf(fp, "#include <stddef.h>\n");
    fprintf(fp, "#endif /* HAVE_STDDEF_H */\n");
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"sr_trans.h\"\n");
    fprintf(fp, "#include \"context.h\"\n");
    fprintf(fp, "#include \"method.h\"\n");
    fprintf(fp, "#include \"mibout.h\"\n");
    fprintf(fp, "#include \"lookup.h\"\n");
    fprintf(fp, "#include \"v2table.h\"\n");
    fprintf(fp, "#include \"sr_cfg.h\"\n");
    fprintf(fp, "#include \"scan.h\"\n");
    fprintf(fp, "\n\n");


    /* ------------ */
    /* STEP 1: print data structure externs */
    /* ------------ */

    fprintf(fp, "extern int %sWriteConfigFileFlag;\n", base);
    fprintf(fp, "\n");
    fprintf(fp, "#ifdef SR_CONFIG_FP\n");
    for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	 ptr = ptr->next_sorted) {
        if (get_index_array(ptr->name, &index_array) != 0) {
	    /* tables */
            if (ptr->valid) {
                fprintf(fp, "extern const PARSER_RECORD_TYPE %sRecord;\n",
                        ptr->name);
            }
        } else {
	    /* scalars */
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
                fprintf(fp, "#ifdef I_%s\n", temp_ptr->name);
                fprintf(fp, "extern const PARSER_RECORD_TYPE %sRecord;\n",
                    temp_ptr->name);
                fprintf(fp, "#endif /* I_%s */\n", temp_ptr->name);
            }
        }
    }
    fprintf(fp, "\n");


    /* ------------ */
    /* STEP 2: print array of PARSER_RECORD_TYPE data structures */
    /* ------------ */

    fprintf(fp, "const PARSER_RECORD_TYPE *%s_rt[] = {\n", base);
    for (ptr = sorted_root; (ptr = next_valid_family(ptr)) != NULL;
	 ptr = ptr->next_sorted) {
        if (get_index_array(ptr->name, &index_array) != 0) {
	    /* tables */
            if (ptr->valid) {
                fprintf(fp, "    &%sRecord,\n", ptr->name);
            }
        } else {
	    /* scalars */
            for (temp_ptr = ptr->next_family_entry; temp_ptr;
                 temp_ptr = temp_ptr->next_family_entry) {
                if (!temp_ptr->valid) continue;
                fprintf(fp, "#ifdef I_%s\n", temp_ptr->name);
                fprintf(fp, "    &%sRecord,\n", temp_ptr->name);
                fprintf(fp, "#endif /* I_%s */\n", temp_ptr->name);
            }
        }
    }
    fprintf(fp, "    NULL\n");
    fprintf(fp, "};\n");
    fprintf(fp, "#endif /* SR_CONFIG_FP */\n");
    fprintf(fp, "\n\n");


    /* ------------ */
    /* STEP 3: print function to read the configuration file */
    /* ------------ */

    fprintf(fp, "int\n");
    fprintf(fp, "parse_%s_config_file(const char *dir, int save_junk)\n", base);
    fprintf(fp, "{\n");
    fprintf(fp, "#ifdef SR_CONFIG_FP\n");
    fprintf(fp, "    int len;\n");
    fprintf(fp, "    char configpath[100];\n");
    fprintf(fp, "    char junkpath[100];\n");
    fprintf(fp, "\n");
    fprintf(fp, "    strcpy(configpath, \"\");\n");
    fprintf(fp, "    strcpy(junkpath, \"\");\n");
    fprintf(fp, "    if (dir != NULL) {\n");
    fprintf(fp, "        len = strlen(dir);\n");
    fprintf(fp, "        if (len > 0) {\n");
    fprintf(fp, "            strcat(configpath, dir);\n");
    fprintf(fp, "            strcat(junkpath, dir);\n");
    fprintf(fp, "            if (dir[len-1] != '/') {\n");
    fprintf(fp, "                strcat(configpath, \"/\");\n");
    fprintf(fp, "                strcat(junkpath, \"/\");\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    strcat(configpath, \"%s.cnf\");\n", base);
    fprintf(fp, "    strcat(junkpath, \"%s.jnk\");\n", base);
    fprintf(fp, "    if (save_junk) {\n");
    fprintf(fp, "        return ParseConfigFileWithErrorLog(configpath, junkpath, %s_rt);\n", base);
    fprintf(fp, "    } else {\n");
    fprintf(fp, "        return ParseConfigFile(configpath, %s_rt);\n", base);
    fprintf(fp, "    }\n");
    fprintf(fp, "#else /* SR_CONFIG_FP */\n");
    fprintf(fp, "    return -1;\n");
    fprintf(fp, "#endif /* SR_CONFIG_FP */\n");
    fprintf(fp, "}\n");
    fprintf(fp, "\n\n");


    /* ------------ */
    /* STEP 4: print function to write the configuration file */
    /* ------------ */

    fprintf(fp, "int\n");
    fprintf(fp, "write_%s_config_file(const char *dir)\n", base);
    fprintf(fp, "{\n");
    fprintf(fp, "#ifdef SR_CONFIG_FP\n");
    fprintf(fp, "    int len;\n");
    fprintf(fp, "    char configpath[100];\n");
    fprintf(fp, "\n");
    fprintf(fp, "    strcpy(configpath, \"\");\n");
    fprintf(fp, "    if (dir != NULL) {\n");
    fprintf(fp, "        len = strlen(dir);\n");
    fprintf(fp, "        if (len > 0) {\n");
    fprintf(fp, "            strcat(configpath, dir);\n");
    fprintf(fp, "            if (dir[len-1] != '/') {\n");
    fprintf(fp, "                strcat(configpath, \"/\");\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    strcat(configpath, \"%s.cnf\");\n", base);
    fprintf(fp, "    %sWriteConfigFileFlag = 0;\n", base);
    fprintf(fp, "    return WriteConfigFile(configpath, %s_rt);\n", base);
    fprintf(fp, "#else /* SR_CONFIG_FP */\n");
    fprintf(fp, "    %sWriteConfigFileFlag = 0;\n", base);
    fprintf(fp, "    return -1;\n");
    fprintf(fp, "#endif /* SR_CONFIG_FP */\n");
    fprintf(fp, "}\n");
    fprintf(fp, "\n\n");

    fclose(fp);
    return 1;
}


