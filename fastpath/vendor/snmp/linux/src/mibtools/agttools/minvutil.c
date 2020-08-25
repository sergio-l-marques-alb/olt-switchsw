/*
 *
 * Copyright (C) 1999-2006 by SNMP Research, Incorporated.
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

#include <stdlib.h>

#include "mibtools.h"
#include "agent.h"

#ifdef P
#undef P
#endif
#define P padding ? padding : ""

int
isIndex(struct OID_INFO *ptr, struct OID_INFO *temp_ptr)
{
    struct _index_array *index_array, *temp_index;

    get_index_array(ptr->name, &index_array);
    for (temp_index = index_array; temp_index && temp_index->name;
                                   temp_index++) {
        if (strcmp(temp_ptr->name, temp_index->name) == 0) {
            return 1;
        }
    }
    return 0;
}

/* generate an initializer for an array element or a standalone variable */
static void
emit_SrGetInfoEntry_initializer(FILE *fp,
                                struct OID_INFO *orig,
                                struct OID_INFO *augments,
                                char *padding)
{
    struct _index_array *index_array;
    struct OID_INFO *ptr = orig;

    get_index_array(ptr->name, &index_array);
    /*
     * if this is not a table, then it has no INDEX clause, and
     * index_array will be NULL.
     */

    fprintf(fp, "%s{\n", P);
    if (small_v_get || no_v_get) {
        fprintf(fp, "%s    (SR_KGET_FPTR) new_k_%s_get,\n", P, ptr->name);
    } else {
        fprintf(fp, "%s    (SR_KGET_FPTR) NULL,\n", P);
    }
    if (userpart && ptr->group_read_write) {
        fprintf(fp, "#ifndef U_%s\n", ptr->name);
        fprintf(fp, "%s    (SR_FREE_FPTR) NULL,\n", P);
        fprintf(fp, "#else /* U_%s */\n", ptr->name);
        fprintf(fp, "%s    (SR_FREE_FPTR) k_%sFreeUserpartData,\n",
                P, ptr->name);
        fprintf(fp, "#endif /* U_%s */\n", ptr->name);
    } else {
        fprintf(fp, "%s    (SR_FREE_FPTR) NULL,\n", P);
    }
    fprintf(fp, "%s    (int) sizeof(%s_t),\n", P, ptr->name);
    fprintf(fp, "%s    I_%s_max,\n", P, ptr->name);
    fprintf(fp, "%s    (SnmpType *) %sTypeTable,\n", P, ptr->name);
    if (index_array) {
        fprintf(fp, "%s    %sIndexInfo,\n", P, ptr->name);
    } else {
        fprintf(fp, "%s    NULL,\n", P);
    }
    fprintf(fp, "%s    (short) offsetof(%s_t, valid)\n", P, ptr->name);
    fprintf(fp, "%s}", P);
}

/* generate SrGetInfoEntry data in a standalone variable */
void
emit_SrGetInfoEntry_standalone(FILE *fp,
                               struct OID_INFO *ptr)
{
    fprintf(fp, "const SrGetInfoEntry %sGetInfo = ", ptr->name);
    emit_SrGetInfoEntry_initializer(fp, ptr, NULL, NULL);
    fprintf(fp, ";\n\n");
}

void
emit_SrGetInfoEntry_augments_standalone(FILE *fp,
                               struct OID_INFO *orig, struct OID_INFO *augment) 
{
    fprintf(fp, "const SrGetInfoEntry %sGetInfo = ", augment->name);
    emit_SrGetInfoEntry_initializer(fp, orig, augment, NULL);
    fprintf(fp, ";\n\n");
}


void
emit_last_sid_array(FILE *fp, struct OID_INFO *ptr)
{
    struct OID_INFO *temp_ptr;

    fprintf(fp, "static const SR_UINT32 %s_last_sid_array[] = {\n", ptr->name);
    temp_ptr = ptr->next_family_entry;
    while (temp_ptr) {
        if (temp_ptr->valid) {
            fprintf(fp, "    %d", 
                temp_ptr->oid_ptr->oid_ptr[temp_ptr->oid_ptr->length - 1]);
            if (temp_ptr->next_family_entry || ptr->augmented) {
                fprintf(fp, ", ");
            } else {
                fprintf(fp, "  ");
            }
            fprintf(fp, "/* %s */\n", temp_ptr->name);
        }
        temp_ptr = temp_ptr->next_family_entry;
    }
    /*
     * if this group is augmented, print descriptors for the items which
     * augment the group
     */
    if (ptr->augmented) {
        struct OID_INFO *augments_list;
        for (temp_ptr = sorted_root; temp_ptr;
             temp_ptr = temp_ptr->next_sorted) {
            if (temp_ptr->augments
                && strcmp(temp_ptr->augments, ptr->name) == 0) {
                augments_list = temp_ptr->next_family_entry;
                while (augments_list) {
                    if (augments_list->valid) {
                        fprintf(fp, "    %d, ", 
                            augments_list->oid_ptr->oid_ptr[augments_list->oid_ptr->length - 1]);
                        fprintf(fp, "/* %s (from %s) */\n", augments_list->name,
                                                           temp_ptr->name);
                    }
                    augments_list = augments_list->next_family_entry;
                }
            }
        }
        fprintf(fp, "    0\n");
    }

    fprintf(fp, "};\n\n");
}

/*
 * output the elements of the sr_member_test array 
 */
void
emit_member_test_element(FILE *fp, struct OID_INFO *ptr, 
                         struct OID_INFO *temp_ptr)
{
    enumeration_t  *temp_enum;
    size_range_t   *size;
    char *test_type;
    char *func_name;
    char test_data_size[512], test_data[512];
    int contig;
    int count;
    struct _index_array *index_array, *temp_index;
    int is_index = 0;

        test_type = "MINV_NO_TEST";
        func_name = "NULL";
        sprintf(test_data_size, "0,");
        sprintf(test_data, "NULL");

        /*
         * Enumerated Integers
         */
        if ((temp_ptr->enumer != NULL) && 
            (strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0)) {
            /* see if the values are contiguous */
            contig = 1;
            count = 1;
            for (temp_enum = temp_ptr->enumer; temp_enum->next; temp_enum = 
                 temp_enum->next) {
                if ((temp_enum->val + 1) != temp_enum->next->val) {
                    contig = 0;
                }
                count++;
            }
            if (contig) {
                test_type = "MINV_INTEGER_RANGE_TEST";
                sprintf(test_data, "%s_range", temp_ptr->name); 
                sprintf(test_data_size, 
                      "\n      sizeof(%s_range)/sizeof(RangeTest_t),"
                      " /* %d */\n     ",
                        temp_ptr->name, count); 
            } else {
                test_type = "MINV_INTEGER_ENUM_TEST";
                sprintf(test_data, "%s_enums", temp_ptr->name); 
                sprintf(test_data_size, 
                      "\n    sizeof(%s_enums)/sizeof(EnumIntTest_t),\n   ", 
                        temp_ptr->name); 
            }
        }
        /*
         * Enumerated BITS
         */
        else if ((temp_ptr->enumer != NULL) && 
                 (strcmp(temp_ptr->oid_prim_type, "Bits") == 0)) {
            sprintf(test_data, "&%s_bits", temp_ptr->name); 
            sprintf(test_data_size, "1,"); 
            test_type = "MINV_BITS_TEST";
        }
        /*
         * Integer or OctetString with size range or length restrictions
         */
        else if (temp_ptr->size != NULL) {

            count = 0;
            for (size = temp_ptr->size; size; size = size->next) {
                count++;
            }

            if ((strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "Integer32") == 0)) {
                test_type = "MINV_INTEGER_RANGE_TEST";
                sprintf(test_data, "%s_range", 
                        temp_ptr->name); 
                sprintf(test_data_size, 
                        "\n      sizeof(%s_range)/sizeof(RangeTest_t),"
                        " /* %d */\n     ",
                        temp_ptr->name, count); 
            } else if ((strcmp(temp_ptr->oid_prim_type, "OctetString") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Opaque") == 0)) {
                test_type = "MINV_LENGTH_TEST";
                sprintf(test_data, "%s_len", 
                        temp_ptr->name); 
                sprintf(test_data_size, 
                        "\n      sizeof(%s_len)/sizeof(LengthTest_t),"
                        " /* %d */\n     ",
                        temp_ptr->name, count); 
            } else if ((strcmp(temp_ptr->oid_prim_type, "Gauge") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "Gauge32") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "Unsigned32") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "Counter") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "Counter32") == 0) ||
                (strcmp(temp_ptr->oid_prim_type, "TimeTicks") == 0)) {
                test_type = "MINV_UINTEGER_RANGE_TEST";
                sprintf(test_data, "%s_range", 
                        temp_ptr->name); 
                sprintf(test_data_size, 
                        "\n      sizeof(%s_range)/sizeof(URangeTest_t),"
                        " /* %d */\n     ",
                        temp_ptr->name, count); 
            }
        }

        /*
         * Textual Conventions
         */
        if (strcmp(temp_ptr->oid_type, "DisplayString") == 0) {
            func_name = "DisplayString_test";
        } else if (strcmp(temp_ptr->oid_type, "RowStatus") == 0) {
            func_name = "RowStatus_test";
        } else if (strcmp(temp_ptr->oid_type, "StorageType") == 0) {
            func_name = "StorageType_test";
        } else if (strcmp(temp_ptr->oid_type, "TestAndIncr") == 0) {
            func_name = "TestAndIncr_test";
        }

        /*
         * see if the variable is an index
         */
        get_index_array(ptr->name, &index_array);
        for (temp_index = index_array; temp_index && temp_index->name;
                                       temp_index++) {
            if (strcmp(temp_ptr->name, temp_index->name) == 0) {
                if (isWritable(temp_ptr)) {
                    func_name = "Identity_test";
                }
                is_index = 1;
                break;
            }
        }

        /*
         * output the structure element
         */
        fprintf(fp, "    /* %s */\n", temp_ptr->name);
        if (isWritable(temp_ptr) || is_index) {
            fprintf(fp, "    { %s, %s %s, %s }", 
                    test_type, test_data_size, test_data, func_name);
        } else {
            fprintf(fp, "    { MINV_NOT_WRITABLE, 0, NULL, NULL }");
        }
}

void 
emit_syntax_restrictions(FILE *fp, struct OID_INFO *ptr, 
                         struct OID_INFO *temp_ptr)
{
    enumeration_t  *temp_enum;
    size_range_t   *size;
    int contig;
    int high, low;
    int count;
    int num_octets;
    int mask;
    int is_index;

        is_index = isIndex(ptr, temp_ptr);

        /*
         * skip objects that are not writable 
         */
        if (!isWritable(temp_ptr) && !is_index) {
            return;
        }

        /*
         * Enumerated Integers
         */
        if ((temp_ptr->enumer != NULL) && 
            (strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0)) {

            /* see if the values are contiguous */
            contig = 1;
            count = 1;
            low = temp_ptr->enumer->val;
            for (temp_enum = temp_ptr->enumer; temp_enum->next; temp_enum = 
                 temp_enum->next) {
                if ((temp_enum->val + 1) != temp_enum->next->val) {
                    contig = 0;
                }
                count++;
            }
            high = temp_enum->val;
            if (!contig) {
                fprintf(fp, "static EnumIntTest_t %s_enums[] = {", temp_ptr->name);
                fprintf(fp, " ");
                for (temp_enum = temp_ptr->enumer; temp_enum; 
                                           temp_enum = temp_enum->next) {
                    fprintf(fp, "%d", temp_enum->val);
                    if (temp_enum->next) {
                        fprintf(fp, ",");
                    }
                    fprintf(fp, " ");
                }
                fprintf(fp, "};\n");
            } else {
                fprintf(fp, "static RangeTest_t   %s_range[] = {",
                        temp_ptr->name);
                fprintf(fp, " { %ld, %ld } ", (long) low, (long) high);
                fprintf(fp, "};\n");
            }
        }
        /*
         * Enumerated BITS
         */
        else if ((temp_ptr->enumer != NULL) && 
                 (strcmp(temp_ptr->oid_prim_type, "Bits") == 0)) {
            /* find highest numbered enumerated bit */
            for (temp_enum = temp_ptr->enumer; temp_enum->next; temp_enum = 
                 temp_enum->next);
            num_octets = (temp_enum->val / 8) + 1;
            mask = (0xff << (7 - (temp_enum->val % 8))) & 0xff;
            fprintf(fp, "static BitsTest_t    %s_bits = %d;\n", 
                    temp_ptr->name, temp_enum->val);
        }
        /*
         * Integer or OctetString with size range or length restrictions
         */
        else if (temp_ptr->size != NULL) {
            if ((strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) ||
                        (strcmp(temp_ptr->oid_prim_type, "Integer32") == 0)) {
                fprintf(fp, "static RangeTest_t   %s_range[] = {",
                        temp_ptr->name);
                for (size = temp_ptr->size; size; size = size->next) {
                    fprintf(fp, " { %ld, %ld }", size->min, 
                                                size->max);
                    if (size->next) {
                        fprintf(fp, ",");
                    }
                }
                fprintf(fp, " };\n");
            } else if ((strcmp(temp_ptr->oid_prim_type, "OctetString") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Opaque") == 0)) {
                fprintf(fp, "static LengthTest_t  %s_len[] = {",
                        temp_ptr->name);
                for (size = temp_ptr->size; size; size = size->next) {
                    fprintf(fp, " { %ld, %ld }", size->min, 
                                                size->max);
                    if (size->next) {
                        fprintf(fp, ", ");
                    }
                }
                fprintf(fp, " };\n");
            } else if ((strcmp(temp_ptr->oid_prim_type, "Gauge") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Gauge32") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Unsigned32") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Counter") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "Counter32") == 0) ||
                       (strcmp(temp_ptr->oid_prim_type, "TimeTicks") == 0)) {
                fprintf(fp, "static URangeTest_t  %s_range[] = {",
                        temp_ptr->name);
                for (size = temp_ptr->size; size; size = size->next) {
                    fprintf(fp, " { %lu, %lu }", size->min, 
                                                size->max);
                    if (size->next) {
                        fprintf(fp, ",");
                    }
                }
                fprintf(fp, " };\n");
            }
        }
}

/*
 * output the sr_member_test array and the arrays which define
 * syntax refinements to size, range, or enumerations
 */
void
emit_SrTestInfoEntry_auxiliary_array(FILE *fp, struct OID_INFO *ptr)
{
    struct OID_INFO *temp_ptr;
    int count;

    /*
     * first, print the test_data for objects which have a refined syntax
     */
    for (temp_ptr = ptr->next_family_entry; temp_ptr;
                    temp_ptr = temp_ptr->next_family_entry) {
        /* if any objects have refined syntax, print a header comment */
        if ( (isWritable(temp_ptr)) && ((temp_ptr->enumer || temp_ptr->size)) ){
            fprintf(fp, "/*\n");
            fprintf(fp, " * Syntax refinements for the %s family\n", ptr->name);
            fprintf(fp, " *\n");
            fprintf(fp, " * For each object in this family in which the syntax clause in the MIB\n");
            fprintf(fp, " * defines a refinement to the size, range, or enumerations, initialize\n");
            fprintf(fp, " * a data structure with these refinements.\n");
            fprintf(fp, " */\n");
            break;
        }
    }

    for (temp_ptr = ptr->next_family_entry; temp_ptr;
                    temp_ptr = temp_ptr->next_family_entry) {
        if (temp_ptr->valid) {
            emit_syntax_restrictions(fp, ptr, temp_ptr);
        }
    }

    /*
     * if this group is augmented, print the items which augment the group
     */
    if (ptr->augmented) {
        struct OID_INFO *augments_list;
        for (temp_ptr = sorted_root; temp_ptr;
             temp_ptr = temp_ptr->next_sorted) {
            if (temp_ptr->augments
                && strcmp(temp_ptr->augments, ptr->name) == 0) {
                augments_list = temp_ptr->next_family_entry;
                while (augments_list) {
                    if (augments_list->valid) {
                        emit_syntax_restrictions(fp, ptr, augments_list);
                    }
                    augments_list = augments_list->next_family_entry;
                }
            }
        }
    }

    /*
     * next, create the test structure with test_kind, sizeof test_data,
     * testdata, and pointer to testfunc
     */
    fprintf(fp, "\n");
    fprintf(fp, "/*\n");
    fprintf(fp, " * Initialize the sr_member_test array with one entry per object in the\n");
    fprintf(fp, " * %s family.\n", ptr->name);
    fprintf(fp, " */\n");
    fprintf(fp, "static struct sr_member_test %s_member_test[] =\n", ptr->name);
    fprintf(fp, "{\n");
    for (temp_ptr = ptr->next_family_entry, count = 0; temp_ptr;
                    temp_ptr = temp_ptr->next_family_entry) {

        if (temp_ptr->valid) {
            if (count != 0) {
                fprintf(fp, ",");
                fprintf(fp, "\n\n");
            }
            count++;
            emit_member_test_element(fp, ptr, temp_ptr);
        }
    }
    /*
     * if this group is augmented, print the items which augment the group
     */
    if (ptr->augmented) {
        struct OID_INFO *augments_list;
        for (temp_ptr = sorted_root; temp_ptr;
             temp_ptr = temp_ptr->next_sorted) {
            if (temp_ptr->augments
                && strcmp(temp_ptr->augments, ptr->name) == 0) {
                augments_list = temp_ptr->next_family_entry;
                while (augments_list) {
                    if (augments_list->valid) {
                        fprintf(fp, ",");
                        fprintf(fp, "\n\n");
                        emit_member_test_element(fp, ptr, augments_list);
                    }
                    augments_list = augments_list->next_family_entry;
                }
            }
        }
    }

    fprintf(fp, "\n");
    fprintf(fp, "};\n");
    fprintf(fp, "\n");
}



/* array element number */
int last_SrGetInfoEntry_created = -1;


/* generate SrGetInfoEntry data in an array element */
void
emit_SrGetInfoEntry_element(FILE *fp,
                            struct OID_INFO *ptr)
{
    static char *comma = "";  /* print no comma before the first element */

    fprintf(fp, "%s\n", comma);
    emit_SrGetInfoEntry_initializer(fp, ptr, NULL, "    ");
    /* DSS augments parameter?? 10/15/2003 ....^  */

    /*
     * Count the number of elements initialized (minv_getinfo_last
     * must point to the last element in the array) and assign the
     * value into the OID_INFO array.
     */
    last_SrGetInfoEntry_created++;
    ptr->getinfo = last_SrGetInfoEntry_created;

    /*
     * Set up to print a comma before the next element
     * (assuming there is a next element)
     */
    comma = ",";
}


/* generate an initializer for an array element or a standalone variable */
static void
emit_SrTestInfoEntry_initializer(FILE *fp, struct OID_INFO *ptr,
                                 char *name, char *padding)
{
    struct _index_array *index_array;
 
    get_index_array(ptr->name, &index_array);

    fprintf(fp, "%s{\n", P);
    if (no_v_get) {
        if (ptr->getinfo < 0) {
            printf("WARNING: no SrGetInfoEntry for %s\n", ptr->name);
            fprintf(fp, "%s    (SrTestInfoEntry *) NULL,\n", P);
        } else {
            fprintf(fp, "%s    &(minv_getinfo[%d]),\n", P, ptr->getinfo);
        }
    } else {
        fprintf(fp, "%s    &%sGetInfo,\n", P, name);
    }
    if (cplusplus) {
        fprintf(fp, "%s    (SrTestInfoEntry::sr_member_test*const) %s_member_test,\n", P, ptr->name);
    } else {
        fprintf(fp, "%s    %s_member_test,\n", P, ptr->name);
    }
    if (index_array) {
        if (ptr->group_read_create == 0) {
            fprintf(fp, "#ifdef %s_READ_CREATE\n", ptr->name);
        }
        fprintf(fp, "%s    k_%s_set_defaults,\n", P, ptr->name);
        if (ptr->group_read_create == 0) {
            fprintf(fp, "#else /* %s_READ_CREATE */\n", ptr->name);
            fprintf(fp, "%s    NULL,\n", P);
            fprintf(fp, "#endif /* %s_READ_CREATE */\n", ptr->name);
        }
    } else {
        fprintf(fp, "%s    NULL,\n", P);
    }
    fprintf(fp, "%s    k_%s_test,\n", P, ptr->name);
    fprintf(fp, "%s    k_%s_ready,\n", P, ptr->name);
    fprintf(fp, "#ifdef SR_%s_UNDO\n", ptr->name);
    fprintf(fp, "%s    %s_undo,\n", P, ptr->name);
    fprintf(fp, "#else /* SR_%s_UNDO */\n", ptr->name);
    fprintf(fp, "%s    NULL,\n", P);
    fprintf(fp, "#endif /* SR_%s_UNDO */\n",ptr->name);
    fprintf(fp, "%s    %s_ready,\n", P, ptr->name);
    fprintf(fp, "%s    %s_set,\n", P, ptr->name);
    fprintf(fp, "%s    %s_cleanup,\n", P, ptr->name);
    if (userpart && ptr->group_read_write) {
        fprintf(fp, "#ifndef U_%s\n", ptr->name);
        fprintf(fp, "%s    (SR_COPY_FPTR) NULL\n", P);
        fprintf(fp, "#else /* U_%s */\n", ptr->name);
        fprintf(fp, "%s    (SR_COPY_FPTR) k_%sCloneUserpartData\n", P, ptr->name);
        fprintf(fp, "#endif /* U_%s */\n", ptr->name);
    } else {
        fprintf(fp, "%s    (SR_COPY_FPTR) NULL\n", P);
    }
    fprintf(fp, "%s}", P);
}

/* generate SrTestInfoEntry data in a standalone variable */
void
emit_SrTestInfoEntry_standalone(FILE *fp,
                                struct OID_INFO *ptr)
{
    fprintf(fp, "/*\n");
    fprintf(fp, " * Initialize SrTestInfoEntry for the %s family.\n", 
            ptr->name);
    fprintf(fp, " */\n");
    fprintf(fp, "const SrTestInfoEntry %sTestInfo = ", ptr->name);
    emit_SrTestInfoEntry_initializer(fp, ptr, ptr->name, NULL);
    fprintf(fp, ";\n\n");
}

void
emit_SrTestInfoEntry_augments_standalone(FILE *fp,
                               struct OID_INFO *orig, struct OID_INFO *augment) 
{
    fprintf(fp, "/*\n");
    fprintf(fp, " * Initialize SrTestInfoEntry for the %s family\n", 
            augment->name); 
    fprintf(fp, " * which augments %s.\n", orig->name); 
    fprintf(fp, " */\n");
    fprintf(fp, "const SrTestInfoEntry %sTestInfo = ", augment->name);
    emit_SrTestInfoEntry_initializer(fp, orig, augment->name, NULL);
    fprintf(fp, ";\n\n");
}


/* array element number */
int last_SrTestInfoEntry_created = -1;


void
emit_SrTestInfoEntry_element(FILE *fp,
                             struct OID_INFO *ptr)
{
    static char *comma = "";  /* print no comma before the first element */

    fprintf(fp, "%s\n", comma);
    emit_SrTestInfoEntry_initializer(fp, ptr, NULL, "    ");
    /* DSS augments parameter?? 10/15/2003 ....^  */

    /*
     * Count the number of elements initialized (minv_testinfo_last
     * must point to the last element in the array) and assign the
     * value into the OID_INFO array.
     */
    last_SrTestInfoEntry_created++;
    ptr->testinfo = last_SrTestInfoEntry_created;

    /*
     * Set up to print a comma before the next element
     * (assuming there is a next element)
     */
    comma = ",";
}


void
write_no_v_get_comment(FILE *fp,
                       struct OID_INFO *ptr)
{
    int idx;
    struct _index_array *index_array;

    /* write a comment noting the absence of the v_ routine */
    fprintf(fp, "/*%s\n", dashes);
    fprintf(fp, " * The %s_get() method routine was not generated", ptr->name);
    if (no_v_get) {
        fprintf(fp, "\n"
                " * (postmosy was launched with the -no_v_get command-line argument).\n");
    } else {
        fprintf(fp, ".\n");
    }
    fprintf(fp, " * A library routine called v_get() performs the same function.\n");
    if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
        /* tabular */
        fprintf(fp, " * This routine uses the data in the SnmpType and SrIndexInfo\n");
        fprintf(fp, " * data structures to correctly process SNMP requests for the\n");
        fprintf(fp, " * %s tabular family.\n", ptr->name);
    } else {
        /* scalar */
        fprintf(fp, " * This routine uses the data in the SnmpType data structure to\n");
        fprintf(fp, " * correctly process SNMP requests for the %s\n", ptr->name);
        fprintf(fp, " * scalar family.\n");
    }
    fprintf(fp, " *%s*/\n\n", dashes);
}

void
write_SrIndexInfo(FILE *fp,
                  struct OID_INFO *ptr)
{
    int idx = 0, found = 0;
    struct _index_array *index_array = NULL, *an_index = NULL;
    struct OID_INFO *temp_ptr = NULL;
    char buf[200];

    /* initialize the buffer */
    strcpy(buf, "");

    if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
        fprintf(fp, "const SrIndexInfo %sIndexInfo[] = {\n", ptr->name); /* } */
        for (an_index = index_array; an_index->name; an_index++) {

            /* see if this index lives in the same table */
            found = 0;
            temp_ptr = ptr->next_family_entry;
            while (temp_ptr) {
                if (strcmp(temp_ptr->name, an_index->name) == 0) {
                    found = 1;
                    break;
                }
                temp_ptr = temp_ptr->next_family_entry;
            }

            /* initialize the nominator field */
            if (found) {
                fprintf(fp, "#ifdef I_%s\n", an_index->name);
                sprintf(buf, "#endif /* I_%s */", an_index->name);
                fprintf(fp, "    { ");  /* } */
                fprintf(fp, "I_%s, ", an_index->name);
            } else {
                /* indices not in the same table have a special macro */
                fprintf(fp, "#ifdef I_%sIndex_%s\n", ptr->name, an_index->name);
                sprintf(buf, "#endif /* I_%sIndex_%s */",
                             ptr->name, an_index->name);
                fprintf(fp, "    { ");  /* } */
                fprintf(fp, "I_%sIndex_%s, ", ptr->name, an_index->name);
            }

            /* initialize the type field */
            fprintf(fp, "T_");
            print_string(fp, an_index, (VINDEX_CAT | FINDEX_CAT));
            fprintf(fp, ", ");

            /* initialize the size field */
            if (an_index->IndexType >= SR_FIXED) {
                fprintf(fp, "%d", an_index->IndexType);
            } else {
                fprintf(fp, "-1");
            } 

            /* end element */
            /* { */
            fprintf(fp, " },\n");

            /* print a matching endif */
            fprintf(fp, "%s\n", buf);
        }
        fprintf(fp, "    { -1, -1, -1 }\n");
        /* { */
        fprintf(fp, "};\n\n");
    }
}

int
OpenIndexIfdefs(FILE *fp,
                struct OID_INFO *ptr)
{
    int idx = 0, found = 0, printed_something = 0;
    struct _index_array *index_array = NULL, *an_index = NULL;
    struct OID_INFO *temp_ptr = NULL;
    char *condition = NULL;

    if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
        condition = "#if";
        for (an_index = index_array; an_index->name; an_index++) {

            /* see if this index lives in the same table */
            found = 0;
            temp_ptr = ptr->next_family_entry;
            while (temp_ptr) {
                if (strcmp(temp_ptr->name, an_index->name) == 0) {
                    found = 1;
                    break;
                }
                temp_ptr = temp_ptr->next_family_entry;
            }

            /* extra logic if this index does not live in the same table */
            if (found) {
                fprintf(fp, "%s !defined(I_%s)", condition, an_index->name);
	        printed_something = 1;
            } else {
                /* indices not in the same table have a special macro */
                fprintf(fp, "%s !defined(I_%sIndex_%s)",
                        condition, ptr->name, an_index->name);
	        printed_something = 1;
            }

            /* update conditional operator */
            condition = " ||";
        }
	if (printed_something) {
            fprintf(fp, "\n");
	}
    }
    return printed_something;
}

int
MatchIndexIfdefs(FILE *fp,
                 struct OID_INFO *ptr,
                 int match_type)
{
    int idx = 0, found = 0, printed_something = 0;
    struct _index_array *index_array = NULL, *an_index = NULL;
    struct OID_INFO *temp_ptr = NULL;
    char *condition = NULL;

    if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
        if (match_type == MII_ELSE) {
            condition = "#else /*";
        } else if (match_type == MII_ENDIF) {
            condition = "#endif /*";
        } else {
            return printed_something;
        }
        for (an_index = index_array; an_index->name; an_index++) {

            /* see if this index lives in the same table */
            found = 0;
            temp_ptr = ptr->next_family_entry;
            while (temp_ptr) {
                if (strcmp(temp_ptr->name, an_index->name) == 0) {
                    found = 1;
                    break;
                }
                temp_ptr = temp_ptr->next_family_entry;
            }

            /* extra logic if this index does not live in the same table */
            if (found) {
                fprintf(fp, "%s I_%s", condition, an_index->name);
	        printed_something = 1;
            } else {
                /* indices not in the same table have a special macro */
                fprintf(fp, "%s I_%sIndex_%s",
                        condition, ptr->name, an_index->name);
	        printed_something = 1;
            }

            /* update conditional operator */
            condition = " ||";
        }
	if (printed_something) {
            fprintf(fp, " */\n");
	}
    }
    return printed_something;
}

int
IsValidFamily(struct OID_INFO *ptr)
{
    struct OID_INFO *temp_ptr;
    int supp_count = 0, valid_count = 0;

    /*
     * These two checks (valid_count and supp_count) correspond
     * to similar checks made in output_agent_files() when writing
     * out the type file and support file information.
     */

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
    if (valid_count == 0) {
        /* no, there are no valid entries in this family */
        return 0;
    }

    /* see if any objects in this family will go into the supp file */
    temp_ptr = ptr->next_family_entry;
    if (xxx_type) {
        supp_count = 1;
    } else {
        supp_count = 0;
    }
    while (temp_ptr) {
        if (temp_ptr->valid) {
            if (!ptr->augments) {
                supp_count++;
            }
        }
        temp_ptr = temp_ptr->next_family_entry;
    }
    if (supp_count == 0) {
        /* no, there are no valid entries in this family */
        return 0;
    }
    return 1;
}

int
IsReadableFamily(struct OID_INFO *ptr)
{
    struct OID_INFO *temp_ptr;

    if (ptr) {
        /* see if there are any readable objects in this family */
        temp_ptr = ptr->next_family_entry;
        while (temp_ptr) {
            if (strcmp(temp_ptr->oid_access, NOT_ACCESSIBLE_STR) &&
                strcmp(temp_ptr->oid_access, ACCESSIBLE_FOR_NOTIFY_STR)) {
                return 1;
            }
            temp_ptr = temp_ptr->next_family_entry;
        }
    }
    return 0;
}

int
HasNetworkAddressInFamily(struct OID_INFO *ptr)
{
    struct OID_INFO *temp_ptr = NULL;

    if (ptr) {
        /* see if there is a NetworkAddress type object in this family */
        temp_ptr = ptr->next_family_entry;
        while (temp_ptr) {
            if (strcmp(temp_ptr->oid_prim_type, "NetworkAddress") == 0) {
                return 1;
            }
            temp_ptr = temp_ptr->next_family_entry;
        }
    }
    return 0;
}

int
HasNetworkAddressInIndex(struct OID_INFO *ptr)
{
    struct _index_array *index_array = NULL, *an_index = NULL;
    int idx = 0;

    if (ptr) {
        if ((idx = get_index_array(ptr->name, &index_array)) != 0) {
            /* see if this table has a NetworkAddress object for an index */
            for (an_index = index_array; an_index->name; an_index++) {
                if (strcmp(an_index->asn1_type, "NetworkAddress") == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

