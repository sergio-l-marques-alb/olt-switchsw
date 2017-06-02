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
#include "rowstat.h"

/*----------------------------------------------------------------
 * print the k_ set_defaults method for this family if it is a table
 * and contains writable variables
 *----------------------------------------------------------------*/
int
write_k_set_defaults_routine(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    struct OID_INFO *p, *temp_ptr;
    struct _index_array *index_array, *temp_index;
    int             idx;
    int             found;
    char           *status_type;
    char           *hex;
    char           *rs_sub_value = NULL;
    char           *rs_comment = "\
    /*\n\
     * The following initial value is not the actual value\n\
     * that appears in the DEFVAL clause of the MIB document.\n\
     * The actual value was: %s\n\
     */\n";

    /* do we support row status? */
    if (row_status != 0) {
	/* yes, look for rowstatus objects */
	status_type = rowStatusString;
    }
    else {
	/* no, use null */
	status_type = NULL;
    }

    /*
     * get an array containing the indicies of this family (from the INDEX
     * clause)
     */
    idx = get_index_array(ptr->name, &index_array);
    if (idx != 0) {
        if (ansi) {
	    fprintf(fp, "int\nk_%s_set_defaults(doList_t *dp)\n", ptr->name);
        } else {
	    fprintf(fp, "int\nk_%s_set_defaults(dp)\n", ptr->name);
	    fprintf(fp, "    doList_t       *dp;\n");
        }
	fprintf(fp, "{\n");

	fprintf(fp, "    %s_t *data = (%s_t *) (dp->data);\n", ptr->name, ptr->name);
	fprintf(fp, "\n");
	for (temp_ptr = ptr->next_family_entry; temp_ptr; temp_ptr = temp_ptr->next_family_entry) {
            if (temp_ptr->valid == 0) {
                continue;
            }

	    /*
	     * if this is not a table, then it has no INDEX clause, and
	     * index_array will be NULL.
	     */
	    found = 0;
	    if (index_array) {
		/* for each index for this table */
		for (temp_index = index_array;
		     temp_index->name && found == 0; temp_index++) {
		    if (strcmp(temp_ptr->name, temp_index->name)
			== 0) {
			found = 1;
		    }
		}		/* for(temp_index... */
	    }			/* if(index_array) */

	    /* don't try to set the index value to a default */
	    if (found != 0)
		continue;

	    if (is_octetstring(temp_ptr->oid_prim_type)) {
		if (temp_ptr->defval) {
                    if (strchr(temp_ptr->defval, '\\')) {
                        hex = hexify_string(temp_ptr->defval);
                        if (hex != NULL) {
                            fprintf(fp, "    if ((data->%s = MakeOctetStringFromHex(\"%s\")) == 0) {\n", temp_ptr->name, hex);
                            /* } */
		            fprintf(fp, "        /* DEFVAL = \"%s\" */\n", temp_ptr->defval);
                        } else {
                            fprintf(fp, "    if ((data->%s = MakeOctetStringFromText(\"\")) == 0) {\n", temp_ptr->name);
                            /* } */
                        }
                    } else {
                        fprintf(fp, "    if ((data->%s = MakeOctetStringFromText(\"%s\")) == 0) {\n", temp_ptr->name, temp_ptr->defval);
                        /* } */
                    }
		} else {
		    fprintf(fp, "    if ((data->%s = MakeOctetStringFromText(\"\")) == 0) {\n", temp_ptr->name);
		}
		fprintf(fp, "        return RESOURCE_UNAVAILABLE_ERROR;\n");
		fprintf(fp, "    }\n");
	    } else if (is_oid(temp_ptr->oid_prim_type)) {
                found = 0;
		if (temp_ptr->defval) {
                    for (p = sorted_root; p != NULL; p = p->next_sorted) {
                        if (strcmp(p->name, temp_ptr->defval) == 0) {
			    found = 1;
			    break;
                        }
                    }
		    if (found) {
		        fprintf(fp, "    /* %s = %s */\n", p->name, p->oid_fleshed_str);
		        fprintf(fp, "    if ((data->%s = MakeOIDFromDot(\"%s\")) == 0) {\n", temp_ptr->name, p->oid_fleshed_str);
		        /* } */
		    } else {
		        fprintf(fp, "    if ((data->%s = MakeOIDFromDot(\"0.0\")) == 0) {\n", temp_ptr->name);
		        /* } */
		    }
                } else {
		    fprintf(fp, "    if ((data->%s = MakeOIDFromDot(\"0.0\")) == 0) {\n", temp_ptr->name);
		    /* } */
                }
		fprintf(fp, "        return RESOURCE_UNAVAILABLE_ERROR;\n");
		/* { */
		fprintf(fp, "    }\n");
	    } else if (is_bits(temp_ptr->oid_prim_type)) {
		if (temp_ptr->defval) {
		    fprintf(fp, "    if ((data->%s = MakeOctetStringFromHex(\"%s\") == 0) {\n", temp_ptr->name, temp_ptr->defval);
		    /* } */
                } else {
		    fprintf(fp, "    if ((data->%s = MakeOctetStringFromHex(\"0x00\") == 0) {\n", temp_ptr->name);
                }
		fprintf(fp, "        return RESOURCE_UNAVAILABLE_ERROR;\n");
		fprintf(fp, "    }\n");
	    } else {
		if (temp_ptr->defval) {
           if ( isdigit((unsigned char)temp_ptr->defval[0]) ||
                ( (temp_ptr->defval[0] == '-') &&  
                  isdigit((unsigned char)temp_ptr->defval[1])) ) {
                        rs_sub_value = substitute_rsval(temp_ptr);
                        if (rs_sub_value != NULL) {
                            fprintf(fp, rs_comment, temp_ptr->defval);
                            fprintf(fp, "    data->%s = %s;\n",
                                    temp_ptr->name, rs_sub_value);
                        } else {
                            fprintf(fp, "    data->%s = %s;\n",
                                    temp_ptr->name, temp_ptr->defval);
                        }
		    } else if (strcmp(temp_ptr->oid_prim_type, "INTEGER") == 0) {
                        if( temp_ptr->enumer != NULL){
                            rs_sub_value = substitute_rsval(temp_ptr);
                            remove_hyphens(temp_ptr->defval);
                            if (rs_sub_value != NULL) {
                                fprintf(fp, rs_comment, temp_ptr->defval);
                                fprintf(fp, "    data->%s = %s;\n",
                                        temp_ptr->name, rs_sub_value);
                            } else {
		                fprintf(fp, "    data->%s = D_%s_%s;\n",
                                        temp_ptr->name, temp_ptr->name,
                                        temp_ptr->defval);
                            }
                        } else {
                            rs_sub_value = substitute_rsval(temp_ptr);
                            if (rs_sub_value != NULL) {
                                fprintf(fp, rs_comment, temp_ptr->defval);
                                fprintf(fp, "    data->%s = %s;\n",
                                        temp_ptr->name, rs_sub_value);
                            } else {
                                fprintf(fp, "    data->%s = %s;\n",
                                        temp_ptr->name, temp_ptr->defval);
                            }
                        } 
		    }
		} else if (is_counter64(temp_ptr->oid_prim_type)) {
                   /* defval illegal for Counter64 type */
		        fprintf(fp, "    data->%s = (UInt64 *) MakeCounter64(0);\n\n", temp_ptr->name);
                } else if (is_counter(temp_ptr->oid_prim_type)) {
                   /*  Counter 32 - uint32  */
                   fprintf(fp, "    data->%s = (SR_UINT32) 0;\n", temp_ptr->name);
		}
	    }
	    if (status_type != NULL
		&& strcmp(temp_ptr->oid_type, status_type) == 0) {
		fprintf(fp, "    data->%sTimerId = -1;\n", status_type);
		fprintf(fp, "\n");
	    }
	}
	fprintf(fp, "\n");
	fprintf(fp, "    SET_ALL_VALID(data->valid);\n");
	fprintf(fp, "    return NO_ERROR;\n");
	fprintf(fp, "}\n");
	fprintf(fp, "\n");
    }
    return 1;
}


/*----------------------------------------------------------------
 * 
 * return a string containing the D_ macro (or RS_ macro)
 * for a substitute RowStatus value obtained as follows...
 * 
 *    DEFVAL { active } or
 *    DEFVAL { 1 }
 *    becomes "D_<object>_createAndGo" if the object has enumerations
 *    or "RS_CREATE_AND_GO" if the object does not have enumerations
 *
 *    DEFVAL { notInService } or
 *    DEFVAL { 2 } or
 *    DEFVAL { notReady } or
 *    DEFVAL { 3 }
 *    becomes "D_<object>_createAndWait" if the object has enumerations
 *    or "RS_CREATE_AND_WAIT" if the object does not have enumerations
 * 
 *    DEFVAL { <bogus_numeric_value> } or
 *    DEFVAL { <bogus_enumeration> }
 *    becomes "RS_UNINITIALIZED"
 * 
 *----------------------------------------------------------------*/

char *
substitute_rsval(struct OID_INFO *p)
{
    int               c = 0, origval = 0;
    enumeration_t    *temp_enum = NULL;
    static char       return_buffer[200];
    static char      *rsEnum[] = {
        NULL,
        "active",
        "notInService",
        "notReady",
        "createAndGo",
        "createAndWait",
        "destroy"
    };
    char             *enum_warning_fmt =
        "Warning: substitue_rsval(): enumeration list "
        "for %s is not valid for RowStatus: %s\n";

    /* check if substitution is requested */
    if (fix_rowstatus_defval == 0) return NULL;

    /* check for bad parameter */
    if (p == NULL) return NULL;

    /* check to make sure this is a RowStatus type object */
    if (strcmp(p->oid_type, rowStatusString) != 0) return NULL;

    /* if present, sanity check the object's enumerations */
    if (p->enumer) {
        c = 1;
        for (temp_enum = p->enumer;
             temp_enum != NULL; temp_enum = temp_enum->next) {
            if (c>6) {
                printf(enum_warning_fmt, p->name, "too many enumerations");
                return NULL;
            }
            if (temp_enum->val != c) {
                printf(enum_warning_fmt, p->name, "value mismatch");
                return NULL;
            }
            if (strcmp(temp_enum->name, rsEnum[c]) != 0) {
                printf(enum_warning_fmt, p->name, "name mismatch");
                return NULL;
            }
            c++;
        }
    } else {
        printf("Warning: substitue_rsval(): no enumeration list "
               "for RowStatus object %s, any substitution will "
               "use a generic RS_ macro\n", p->name);
    }
    if (c <= 6) {
        printf("Warning: substitue_rsval(): incomplete enumeration "
               "list for RowStatus object %s, a substitution may "
               "use a generic RS_ macro\n", p->name);
    }

    /* convert DEFVAL to numeric */
    if (isdigit((unsigned char)p->defval[0])) {
        origval = atoi(p->defval);
    } else if (strcmp(p->defval, rsEnum[RS_ACTIVE]) == 0) {
        origval = RS_ACTIVE;
    } else if (strcmp(p->defval, rsEnum[RS_NOT_IN_SERVICE]) == 0) {
        origval = RS_NOT_IN_SERVICE;
    } else if (strcmp(p->defval, rsEnum[RS_NOT_READY]) == 0) {
        origval = RS_NOT_READY;
    } else if (strcmp(p->defval, rsEnum[RS_CREATE_AND_GO]) == 0) {
        origval = RS_CREATE_AND_GO;
    } else if (strcmp(p->defval, rsEnum[RS_CREATE_AND_WAIT]) == 0) {
        origval = RS_CREATE_AND_WAIT;
    } else if (strcmp(p->defval, rsEnum[RS_DESTROY]) == 0) {
        origval = RS_DESTROY;
    } else {
        printf("Warning: substitue_rsval(): unknown text value (%s) "
               "in DEFVAL clause for RowStatus object (%s)\n",
               p->defval, p->name);
        /*
         * To avoid generating a bogus macro in the form
         * of D_<object>_<nonEnumerationString>, substitute
         * the "uninitialized" value.
         */
        sprintf(return_buffer, "RS_UNINITIALIZED");
        return (return_buffer);
    }

    switch(origval) {
        case RS_ACTIVE:
            if (p->enumer && c > RS_CREATE_AND_GO) {
                sprintf(return_buffer, "D_%s_%s",
                        p->name, rsEnum[RS_CREATE_AND_GO]);
            } else {
                sprintf(return_buffer, "RS_CREATE_AND_GO");
            }
            return (return_buffer);

        case RS_NOT_IN_SERVICE:
            break;
        case RS_NOT_READY:
            if (p->enumer && c > RS_CREATE_AND_WAIT) {
                sprintf(return_buffer, "D_%s_%s",
                        p->name, rsEnum[RS_CREATE_AND_WAIT]);
            } else {
                sprintf(return_buffer, "RS_CREATE_AND_WAIT");
            }
            return (return_buffer);

        case RS_CREATE_AND_GO:
            break;
        case RS_CREATE_AND_WAIT:
            break;
        case RS_DESTROY:
            /* no need for substitution */
            return NULL;

        default:
            printf("Warning: substitue_rsval(): unknown numeric value (%s) "
                   "in DEFVAL clause for RowStatus object (%s)\n",
                   p->defval, p->name);
            /*
             * To avoid generating code containing a bogus initial
             * value, substitute the "uninitialized" value.
             */
            sprintf(return_buffer, "RS_UNINITIALIZED");
            return (return_buffer);
    }

    return NULL;
}

