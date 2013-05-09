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
 
#include "mibtools.h"
#include "mib_tree.h"

static int table_height = 15;
static int scalar_height = 5;

int
output_rtview_file (char *base)
{
    struct OID_INFO *ptr, *temp_ptr;
    FILE *fp;
    struct _index_array *index_array;
    /* int y = 80 - table_height; */
    int y = 70;
 
    fp = open_file(base, ".rtv");
 
    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<rtview>\n");
    fprintf(fp, "        <model file=\"com.sl.gmsjrtview.m_basemodel\"\n");
    fprintf(fp, "                width=\"850\" height=\"600\"\n");
    fprintf(fp, "                bgColor=\"13\"\n");
    fprintf(fp, "                doBevel=\"1\"/>\n");
    fprintf(fp, "\n");

    /*
     * list all groups and then all the families within a group 
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1 && ptr->valid) {
            /* if it has indexes, it's a table, otherwise, it's a scalar */
            get_index_array(ptr->name, &index_array);
            if (index_array) {
                /* table */
                fprintf(fp,"        <object name=\"%s\" class=\"obj_table02\"\n",ptr->parent_ptr->name);
                fprintf(fp, "                y=\"%d.0\" x=\"55.0\"\n", y);
                fprintf(fp, "                objWidth=\"100\"\n");
                fprintf(fp, "                bgColor=\"13\"\n");
                fprintf(fp, "                objHeight=\"%d.0\"\n", table_height);
                fprintf(fp, "                label=\"%s\"\n", ptr->parent_ptr->name);
                fprintf(fp, "                labelTextColor=\"7\"\n");
                fprintf(fp, "                labelTextHeight=\"1.375\"\n");
                fprintf(fp, "                numCharsInColumn=\"6\"\n");
                fprintf(fp, "                autoResizeMode=\"1\"\n");
                fprintf(fp, "                rowLabelMode=\"0\"\n");
                fprintf(fp, "                sortColumnName=\"\"\n");
                fprintf(fp, "                labelTextFont=\"7\"\n");
                fprintf(fp, "                labelTextAlignX=\"2\">\n");
                fprintf(fp, "                <vardef varname=\"tableStringValues\" dsstring=\"gettab $agent 100 ");
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid == 0) {
                        continue;
                    }
                    if (temp_ptr->oid_access && 
                        strcmp(temp_ptr->oid_access, "not-accessible") == 0) {
                        continue;
                    } 
                    fprintf(fp, "%s", temp_ptr->name);
                    if (temp_ptr->next_family_entry) {
                        fprintf(fp, " ");
                    }
                }
                if (y > table_height + 5) {
                    y-=table_height;
                } else if (y > 5) {
                    y -= 5;
                } else {
                }
                fprintf(fp, "\"/>\n");
                fprintf(fp, "        </object>\n");
            } else { 
                /* scalar */

                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid) {
                        fprintf(fp, "        <object name=\"%s\" class=\"obj_label11s\"\n", temp_ptr->name);
                        fprintf(fp, "            y=\"%d.0\" x=\"30.0\"\n", y);
                        fprintf(fp, "            fieldWidth=\"10\"\n");
                        fprintf(fp, "            labelWidth=\"25\"\n");
                        fprintf(fp, "            label=\"%s\"\n", temp_ptr->name);
                        fprintf(fp, "            labelTextFont=\"0.0\"\n");
                        fprintf(fp, "            valueTextColor=\"7\"\n");
                        fprintf(fp, "            valueTextHeight=\"1.25\">\n");
                        fprintf(fp, "            <vardef varname=\"valueString\" dsstring=\"get $agent 30 %s.0\"/>\n", temp_ptr->name);
                        fprintf(fp, "        </object>\n");
                    }
                    if (y > scalar_height + 5) {
                        y-=scalar_height;
                    } else if (y > 5) {
                        y -= 5;
                    } else {
                    }
                }
            }
        }
    }

    fprintf(fp, "\n</rtview>\n");
    fclose(fp);
    return 0;
}

int
output_ciagentmonitor_file(char *base)
{
    struct OID_INFO *ptr, *temp_ptr;
    char *group = "";
    FILE *fp;
    struct _index_array *index_array;
    struct _index_array *temp_index;
    int i, j;
    char prefix[100], name[100];
    int tab = 0, subtab = 0;
 
    fp = open_file(base, ".cnf");
 
    fprintf(fp, "<?xml version=\"1.0\"?>\n");
    fprintf(fp, "<ciagentmonitor>\n");
    fprintf(fp, "\n");

    /*
     * list all groups and then all the families within a group 
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1 && ptr->valid) {
            /* if it has indexes, it's a table, otherwise, it's a scalar */
            get_index_array(ptr->name, &index_array);
            if (index_array) {
                /* table */
                if (strcmp(group, ptr->parent_ptr->parent_ptr->name) != 0) {
                    if (tab) {
                        tab--;
                        fprintf(fp, "    </tab>\n\n");
                    }
                    group = ptr->parent_ptr->parent_ptr->name;
                    fprintf(fp, "    <tab name=\"%s\">\n", group);
                    tab++;
                }
                fprintf(fp,"        <tab name=\"%s\">\n",ptr->parent_ptr->name);
                subtab++;

                fprintf(fp, "            <mibobj columns=\"");
 
                /* see how many prefix characters are the same */
                strcpy(prefix, ptr->next_family_entry->name);
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    for (i=0; prefix[i] && temp_ptr->name[i] == prefix[i]; i++);
                    prefix[i] = '\0';
                }
 
                /* print the name without the prefix and with spaces added
                   in place of caps */
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid == 0) {
                        continue;
                    }
                    for (i = strlen(prefix), j = 0; temp_ptr->name[i]; i++) {
                        if (j > 0 && isupper(temp_ptr->name[i])) {
                            name[j++] = ' ';
                        }
                        name[j++] = temp_ptr->name[i];
                    }
                    name[j] = '\0';
                    fprintf(fp, "%s:%s", temp_ptr->name, name);
                    if (temp_ptr->next_family_entry) {
                        fprintf(fp, ",");
                    }
                }
                /*
                 * Add an entry in the file for the entries in the INDEX
                 * clause if those entries are not already in the file
                 * (i.e. if the index is in another family).
                 */
 
                /* for each index for this table */
                for (temp_index = index_array; temp_index->name; temp_index++) {
                    /*
                     * first, see if this entry is already in the
                     * file (i.e. if is in this table).
                     */
                    int found = 0;
                    for (temp_ptr = ptr->next_family_entry; temp_ptr;
                                temp_ptr = temp_ptr->next_family_entry) {
                        if (temp_ptr->valid) {
                            if (strcmp(temp_ptr->name, temp_index->name) == 0) {
                                found = 1;
                            }
                        } else {
                            continue;
                        }
                    }
 
                    /*
                     * if is is not in the file, then put it in now
                     */
                    if (found) {
                        continue;
                    }
                    fprintf(fp, ",");
                    fprintf(fp, "%s", temp_index->name);
                }    /* for(temp_index... */
                fprintf(fp, "\">%s</mibobj>\n", ptr->parent_ptr->name);
            } else { 
                /* scalar */
                if (tab) {
                    tab--;
                    fprintf(fp, "    </tab>\n\n");
                }
                if (strcmp(group, ptr->name) != 0) {
                    group = ptr->name;
                    fprintf(fp, "    <tab name=\"%s\">\n", group);
                    tab++;
                }
                fprintf(fp, "        <tab name=\"%s\">\n", group);
                subtab++;
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid) {
                        fprintf(fp, "            <mibobj>%s.0</mibobj>'\n",
                                             temp_ptr->name);
                    }
                }
            }
            if (subtab) {
                subtab--;
                fprintf(fp, "        </tab>\n");
            }
        }
    }
    if (tab) {
        tab--;
        fprintf(fp, "    </tab>\n");
    }

    fprintf(fp, "\n</ciagentmonitor>\n");
    fclose(fp);
    return 0;
}

static int count = 1;
static int print_drweb_header(FILE *fp, char *base);
static int one_file = 1;  /* create one big custom page */

int
output_drweb_file(char *base)

{
    struct OID_INFO *ptr, *temp_ptr;
    char *group = "";
    char *file = NULL, *family=NULL;
    FILE *fp, *fppagelist;
    struct _index_array *index_array;
    int i, j;
    char prefix[100], name[100];
    char *a;
 
    fppagelist = open_file("pagelist.", base);
    fp = open_file(base, ".cnf");
    print_drweb_header(fp, base); 
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '<table border=1 cellpadding=10>' \\\n");
    fprintf(fp, "    %s\n\n", base);

    if (one_file) {
        a = "#";
    } else {
        a = "";
        fprintf(fppagelist, "%s\n", base);
    }

    /*
     * first list all the groups in the table column headings
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1 && ptr->valid) {
            /* if it has indexes, it's a table, otherwise, it's a scalar */
            get_index_array(ptr->name, &index_array);
            if (index_array) {
                /* table */
                if (strcmp(group, ptr->parent_ptr->parent_ptr->name) != 0) {
                    group = ptr->parent_ptr->parent_ptr->name;
                    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                    fprintf(fp, "    '<th align=center>%s' \\\n", group);
                    fprintf(fp, "    %s\n\n", base);
                }
            } else { 
                /* scalar */
                if (strcmp(group, ptr->name) != 0) {
                    group = ptr->name;
                    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                    fprintf(fp, "    '<th align=center>%s' \\\n",group);
                    fprintf(fp, "    %s\n\n", base);
                }
            }
        }
    }

    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '<tr>' \\\n");
    fprintf(fp, "    %s\n\n", base);

    /*
     * now list all the families within a group with links to that family
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1 && ptr->valid) {
            /* if it has indexes, it's a table, otherwise, it's a scalar */
            get_index_array(ptr->name, &index_array);
            if (index_array) {
                /* table */
                if (strcmp(group, ptr->parent_ptr->parent_ptr->name) != 0) {
                    group = ptr->parent_ptr->parent_ptr->name;
                    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                    fprintf(fp, "    '<td align=center>' \\\n");
                    fprintf(fp, "    %s\n\n", base);
                }
                fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                fprintf(fp, "    '<a href=%s%s>%s<br></a>' \\\n", a, ptr->parent_ptr->name, ptr->parent_ptr->name);
                fprintf(fp, "    %s\n\n", base);
            } else { 
                /* scalar */
                if (strcmp(group, ptr->name) != 0) {
                    group = ptr->name;
                    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                    fprintf(fp, "    '<td align=center>' \\\n");
                    fprintf(fp, "    %s\n\n", base);
                }
                fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                fprintf(fp, "    '<a href=%s%s> %s<br></a>' \\\n", a, ptr->name, ptr->name);
                fprintf(fp, "    %s\n\n", base);
            }
        }
    }

    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '</table>' \\\n");
    fprintf(fp, "    %s\n\n", base);

    group = "";
    if (!one_file) {
        fclose(fp); fp = NULL; count = 0;
    }

    /*
     * now get all the MIB data from each group
     */
    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
        if (ptr->family_root == 1 && ptr->valid) {

            /* if it has indexes, it's a table, otherwise, it's a scalar */
            get_index_array(ptr->name, &index_array);
            if (index_array) {
                /* table */
                family = ptr->parent_ptr->name;
            } else {
                /* scaler */
                family = ptr->name;
            }

            if (!one_file) {
                file = family;
                fp = open_file(file, ".cnf");
                print_drweb_header(fp, family);
            } else {
                file = base;
            }
            fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
            fprintf(fp, "    '<a name=%s> <h2>%s</h2></a>' \\\n", family, family);
            fprintf(fp, "    %s\n\n", file);
 
            if (!one_file) {
                fprintf(fppagelist, "%s\n", family);
            }

            if (index_array) {
                /* table */
                fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                fprintf(fp, "    '        <mibobj columns=\"");

                /* see how many prefix characters are the same */
                strcpy(prefix, ptr->next_family_entry->name);
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    for (i=0; prefix[i] && temp_ptr->name[i] == prefix[i]; i++);
                    prefix[i] = '\0';
                }

                /* print the name without the prefix and with spaces added
                   in place of caps */
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid == 0) {
                        continue;
                    }
                    for (i = strlen(prefix), j = 0; temp_ptr->name[i]; i++) {
                        if (j > 0 && isupper(temp_ptr->name[i])) {
                            name[j++] = ' ';
                        }
                        name[j++] = temp_ptr->name[i];
                    }
                    name[j] = '\0';
                    fprintf(fp, "%s:%s", temp_ptr->name, name);
                    if (temp_ptr->next_family_entry) {
                        fprintf(fp, ",");
                    }
                }


                fprintf(fp, "\">%s</mibobj>' \\\n", ptr->parent_ptr->name);
                fprintf(fp, "    %s\n\n", file);
            } else { 
                /* scalar */
                for (temp_ptr = ptr->next_family_entry; temp_ptr;
                            temp_ptr = temp_ptr->next_family_entry) {
                    if (temp_ptr->valid) {
                        fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
                        fprintf(fp, "    '        <mibobj>%s.0</mibobj>' \\\n",
                                             temp_ptr->name);
                        fprintf(fp, "    %s\n\n", file);
                    }
                } 
            } 
            fprintf(fp, "\n\n");
            if (!one_file) {
                fclose(fp); fp = NULL; count = 0;
            } 
        }
    }
 
    if (one_file) {
        fclose(fp);
    }
    fclose(fppagelist);
    return 0;
}
int
print_drweb_header(FILE *fp, char *base)
{
    fprintf(fp, "htmlPageEntry %s \"%s\" - - - - 2\n", base, base);
    fprintf(fp, "\n");
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '<html>' \\\n");
    fprintf(fp, "    %s\n\n", base);
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '<head>' \\\n");
    fprintf(fp, "    %s\n\n", base);
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '  <title>%s</title>' \\\n", base);
    fprintf(fp, "    %s\n\n", base);
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '</head>' \\\n");
    fprintf(fp, "    %s\n\n", base);
 
    fprintf(fp, "htmlPageContentEntry %d \\\n", count++);
    fprintf(fp, "    '<body bgcolor=white>' \\\n");
    fprintf(fp, "    %s\n\n", base);

    fprintf(fp, "\n"); 
    return 0;
}
