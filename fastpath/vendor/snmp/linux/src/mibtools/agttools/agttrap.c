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
#include "agent.h"

/*
 * Convert an OID string from name form to number form
 */
int
NumberForm(name, buff)
    char           *name;
    char           *buff;
{
    OID            *oid;
    int             i;
    char            temp[16];

    oid = MakeOIDFromDot(name);
    if (oid == NULL) {
	strcpy(buff, name);
	return -1;
    }

    sprintf(buff, "\"");
    for (i = 0; i < oid->length; i++) {
#if SIZEOF_INT == 4
	sprintf(temp, "%d.", oid->oid_ptr[i]);
#else	/*  SIZEOF_INT == 4 */
	sprintf(temp, "%ld.", oid->oid_ptr[i]);
#endif	/*  SIZEOF_INT == 4 */
	strcat(buff, temp);
    }
    buff[strlen(buff) - 1] = '\"';
    buff[strlen(buff)] = '\0';
    return 1;

}

/*
 * write functions to send traps
 */
int
write_traps(base)
    char           *base;
{
    char            buff[100];
    FILE           *fp;
    FILE           *fp_h;
    char           *p, *q;
    char           *enterprise_ptr;
    int             i, j;
    int             count;
    int             idx = 0, total_idx = 0;
    TRAP_INFO_t    *trap_ptr;
    char           *objects[100];
    char           *objects_dotform[100];	/* objects in dotted decimal
						 * form */
    struct OID_INFO *ptr;
    struct _index_array *index_array, *temp_index;

    long            genTrap = 0;
    long            specTrap = 0;
    char            entV2Trap[128];
    int cr_not_printed = 1;

    sprintf(buff, "%strap", base);
    fp = open_file(buff, ".stb");
    fp_h = open_file(buff, ".h");
    print_copyright(fp);
    print_copyright(fp_h);

    if (!no_cvs_keywords) {
      print_cvs_keywords(fp, C_FILE);
    }

    fprintf(fp, "#include \"sr_conf.h\"\n\n");
    fprintf(fp, "#ifdef HAVE_STDIO_H\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#endif /* HAVE_STDIO_H */\n");
    fprintf(fp, "#ifdef HAVE_MEMORY_H\n");
    fprintf(fp, "#include <memory.h>\n");
    fprintf(fp, "#endif /* HAVE_MEMORY_H */\n");
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"diag.h\"\n");
    fprintf(fp, "#include \"sr_trans.h\"\n");
    fprintf(fp, "#include \"context.h\"\n");
    fprintf(fp, "#include \"method.h\"\n");
    fprintf(fp, "#include \"mibout.h\"\n");
    fprintf(fp, "#include \"trap.h\"\n");
    fprintf(fp, "#include \"getvar.h\"\n\n");
    fprintf(fp, "SR_FILENAME\n\n");
    fprintf(fp, "#include \"%s.h\"\n\n", buff);

#define _GETTRAPVAR_PROTOTYPE_STRING "\
static VarBind *%s_GetTrapVar\n\
SR_PROTOTYPE((OID *object,\n\
              OID *inst,\n\
              ContextInfo *contextInfo));\n\n"

#define _GETTRAPVAR_ANSI_FUNC "\
static VarBind *\n\
%s_GetTrapVar(\n\
    OID *object,\n\
    OID *inst,\n\
    ContextInfo *contextInfo)\n"

#define _GETTRAPVAR_KANDR_FUNC "\
static VarBind *\n\
%s_GetTrapVar(object, inst, contextInfo)\n\
    OID *object;\n\
    OID *inst;\n\
    ContextInfo *contextInfo;\n"

#define _GETTRAPVAR_FUNC_BODY "\
{\n\
    OID *var;\n\
    VarBind *vb = NULL;\n\n\
    if ((object == NULL) || (inst == NULL)) {\n\
        return NULL;\n\
    }\n\
    var = CatOID(object, inst);\n\
    FreeOID(object);\n\
    FreeOID(inst);\n\
    if (var == NULL) {\n\
        return NULL;\n\
    }\n\
    vb = i_GetVar(contextInfo, EXACT, var);\n\
    if (vb == NULL) {\n\
        FreeOID(var);\n\
        return NULL;\n\
    }\n\
#ifdef SR_SNMPv2_PDU\n\
    if ((vb->value.type == NO_SUCH_OBJECT_EXCEPTION) ||\n\
        (vb->value.type == NO_SUCH_INSTANCE_EXCEPTION)) {\n\
        FreeOID(var);\n\
        FreeVarBindList(vb);\n\
        return NULL;\n\
    }\n\
#endif /* SR_SNMPv2_PDU */\n\
    FreeOID(var);\n\
    return vb;\n\
}\n\n"

    fprintf(fp, _GETTRAPVAR_PROTOTYPE_STRING, base);

    if (ansi) {
        fprintf(fp, _GETTRAPVAR_ANSI_FUNC, base);
    } else {
        fprintf(fp, _GETTRAPVAR_KANDR_FUNC, base);
    }

    fprintf(fp, _GETTRAPVAR_FUNC_BODY);

    for (trap_ptr = trap_info; trap_ptr; trap_ptr = trap_ptr->next) {
	index_array = NULL;
	total_idx = 0;
	i = 0;
	if ((p = trap_ptr->objects) != NULL) {
	    while (p && *p) {
		while ((isspace((unsigned char)*p)) || (*p == '{') || (*p == '}')) {
		    p++;
		}
		if (!*p || (*p == '\n')) {
		    continue;
		}
		q = p;
		while ((*q) && (*q != ',') && (*q != ' ') && (*q != '}')) {
		    q++;
		}
		*q = '\0';
		objects_dotform[i] = p;
		for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		    if (strcmp(p, ptr->name) == 0) {
			objects_dotform[i] = ptr->oid_fleshed_str;
			break;
		    }
		}
		objects[i++] = p;
		q++;
		p = q;
	    }
	}
	objects[i] = NULL;

	/*
	 * write the send_*_trap functions
	 */
	fprintf(fp, "int\n");
	fprintf(fp, "send_%s_trap(", trap_ptr->name);
	fprintf(fp_h, "int\n");
	fprintf(fp_h, "send_%s_trap(", trap_ptr->name);
	for (i = 0; objects[i]; i++) {
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if (strcmp(objects[i], ptr->name) == 0) {
		    idx = get_index_array(ptr->parent_ptr->name, &index_array);
		    if ((idx == -1) || (total_idx == -1)) {
			total_idx = -1;
		    }
		    else {
			total_idx = total_idx + idx;
		    }
		    break;
		}
	    }
	    if (ptr == NULL) {
		printf("Warning: write_traps() cannot find %s\n", objects[i]);
		printf("send_%s_trap may not be correct\n", trap_ptr->name);
		break;
	    }
	    if (!ansi) {
	      for (temp_index = index_array;
		 temp_index && temp_index->name;
		 temp_index++) {
		fprintf(fp, "%s_%d, ", temp_index->name, i);
	      }
	    }
	}
	if (!ansi) {
	  fprintf(fp, "add_vblist, contextInfo)\n");
	}

	for (i = 0; objects[i]; i++) {
	    index_array = NULL;
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if (strcmp(objects[i], ptr->name) == 0) {
		    idx = get_index_array(ptr->parent_ptr->name, &index_array);
		    break;
		}
	    }
	    if (ptr == NULL) {
		printf("Warning: write_traps() cannot find %s\n", objects[i]);
		printf("send_%s_trap may not be correct\n", trap_ptr->name);
		break;
	    }
	    for (temp_index = index_array;
		 temp_index && temp_index->name;
		 temp_index++) {
		fprintf(fp, "%s    %s %s_%d%s\n",
			(ansi && cr_not_printed) ? "\n" : "",
			temp_index->c_type,
			temp_index->name,
			i,
			(ansi) ? "," : ";");
		fprintf(fp_h, "%s    %s %s_%d%s\n",
			(cr_not_printed) ? "\n" : "",
			temp_index->c_type,
			temp_index->name,
			i,
			(ansi) ? "," : ";");
		cr_not_printed = 0;
	    }
	}
	fprintf(fp, "%s    VarBind *add_vblist%s\n",
		(ansi && cr_not_printed) ? "\n" : "",
		(ansi) ? "," : ";");
	fprintf(fp, "    ContextInfo *contextInfo%s\n",
		(ansi) ? ")" : ";");
	fprintf(fp, "{\n");
	fprintf(fp_h, "%s    VarBind *add_vblist%s\n",
		(cr_not_printed) ? "\n" : "", ",");
	fprintf(fp_h, "    ContextInfo *contextInfo)");
	fprintf(fp_h, ";\n\n");
	cr_not_printed = 1;
	if (total_idx != 0) {
	    fprintf(fp, "    int i;\n");
	    fprintf(fp, "    int index;\n");
	    fprintf(fp, "    OID inst;\n");
	    if (total_idx > 0) {
		fprintf(fp, "    SR_UINT32 buffer[%d];\n", total_idx);
	    }
	    else {
		fprintf(fp, "    SR_UINT32 buffer[MAX_OID_SIZE];\n");
	    }
	}
	for (i = 0; objects[i]; i++) {
	    fprintf(fp, "    OID *%s;\n", objects[i]);
	}
	if (total_idx != 0) {
	    fprintf(fp, "    inst.oid_ptr = buffer;\n");
	    fprintf(fp, "\n");
	}
	for (i = 0; objects[i]; i++) {
	    index_array = NULL;
	    for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		if (strcmp(objects[i], ptr->name) == 0) {
		    idx = get_index_array(ptr->parent_ptr->name, &index_array);
		    break;
		}
	    }
	    if (ptr == NULL) {
		printf("Warning: write_traps() cannot find %s\n", objects[i]);
		printf("send_%s_trap may not be correct\n", trap_ptr->name);
		break;
	    }
	    if (idx != 0) {
		fprintf(fp, "    index = 0;\n");
	    }
	    for (temp_index = index_array; temp_index && temp_index->name; temp_index++) {
		trap_print_x_to_inst(fp, temp_index, i);
	    }
	    if (idx != 0) {
		fprintf(fp, "    inst.length = index;\n");
		fprintf(fp, "    %s = CloneOID(&inst);\n", objects[i]);
		fprintf(fp, "\n");
	    }
	    else {
		fprintf(fp, "    %s = MakeOIDFragFromDot(\"0\");\n", objects[i]);
		fprintf(fp, "\n");
	    }
	}
	fprintf(fp, "    i_send_%s_trap(", trap_ptr->name);
	for (i = 0; objects[i]; i++) {
	    fprintf(fp, "%s, ", objects[i]);
	}
	fprintf(fp, "add_vblist, contextInfo);\n");
	fprintf(fp, "    return 0;\n");
	fprintf(fp, "}\n");
	fprintf(fp, "\n");

	/*
	 * write the i_send_*_trap functions
	 */
	fprintf(fp, "int\n");
	fprintf(fp_h, "int\n");
	fprintf(fp, "i_send_%s_trap(", trap_ptr->name);
	fprintf(fp_h, "i_send_%s_trap(", trap_ptr->name);
	if (!ansi) {
	  for (i = 0; objects[i]; i++) {
	      fprintf(fp, "%s, ", objects[i]);
	  }
	  fprintf(fp, "add_vblist, ");
	  fprintf(fp, "contextInfo)\n");
	}
	for (i = 0; objects[i]; i++) {
	    fprintf(fp, "%s    OID             *%s%s\n",
		    (ansi && cr_not_printed) ? "\n" : "",
		    objects[i],
		    (ansi) ? "," : ";");
	    fprintf(fp_h, "%s    OID             *%s%s\n",
		    (cr_not_printed) ? "\n" : "",
		    objects[i], ",");
	    cr_not_printed = 0;
	}
	fprintf(fp, "%s    VarBind         *add_vblist%s\n",
		(ansi && cr_not_printed) ? "\n" : "",
		(ansi) ? "," : ";");
	fprintf(fp, "    ContextInfo     *contextInfo%s\n",
		(ansi) ? ")" : ";");
	fprintf(fp_h, "%s    VarBind         *add_vblist%s\n",
		(cr_not_printed) ? "\n" : "", ",");
	fprintf(fp_h, "    ContextInfo     *contextInfo)");
	fprintf(fp, "{\n");
	fprintf(fp_h, ";\n\n");
	fprintf(fp, "    OID             *enterprise = NULL;\n");
	fprintf(fp, "    VarBind         *vb = NULL;\n");
	fprintf(fp, "    VarBind         *temp_vb = NULL;\n");
	fprintf(fp, "    \n");
	cr_not_printed = 1;

	for (count = 0; objects[count]; count++);
	if (count) {
	    fprintf(fp, "    if (");
	    for (i = 0; objects[i]; i++) {
		switch (i) {
		  case 0:
		    fprintf(fp, "(%s == NULL)", objects[i]);
		    break;
		  default:
		    fprintf(fp, " ||\n        (%s == NULL)", objects[i]);
		    break;
		}
	    }
	    fprintf(fp, ") {\n");
	    if (count > 1) {
		for (i = 0; objects[i]; i++) {
		    fprintf(fp, "        FreeOID(%s);\n", objects[i]);
		}
	    }
	    fprintf(fp, "        DPRINTF((APTRAP, \"i_send_%s_trap: \"));\n", trap_ptr->name);
	    fprintf(fp, "        DPRINTF((APTRAP, \"At least one incoming OID is NULL\\n\"));\n");
	    fprintf(fp, "        return -1;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}

	/* determine what genTrap and specTrap should be */
	specTrap = 0;
	genTrap = 0;
	if (trap_ptr->number == -1) {
	    /* It is defined as a v2 trap */
	    /*
	     * first see if it is a standard trap so that we can do the right
	     * conversion if we need to send it to a v1 manager
	     */
	    if (strcmp(trap_ptr->name, "coldStart") == 0) {
		genTrap = 0;
	    }
	    else if (strcmp(trap_ptr->name, "warmStart") == 0) {
		genTrap = 1;
	    }
	    else if (strcmp(trap_ptr->name, "linkDown") == 0) {
		genTrap = 2;
	    }
	    else if (strcmp(trap_ptr->name, "linkUp") == 0) {
		genTrap = 3;
	    }
	    else if (strcmp(trap_ptr->name, "authenticationFailure") == 0) {
		genTrap = 4;
	    }
	    else if (strcmp(trap_ptr->name, "egpNeighborLoss") == 0) {
		genTrap = 5;
	    }
	    else {
		/* enterprise specific */
		genTrap = 6;
	    }
	}
	else {
	    /* It is defined as a v1 trap */
	    if (strcmp(trap_ptr->enterprise, "snmp") == 0) {
		/* standard trap */
		genTrap = trap_ptr->number;
	    }
	    else {
		/* enterprise specific */
		genTrap = 6;
		specTrap = trap_ptr->number;
	    }
	}

	/* now build the VarBind list */
	for (i = 0; objects[i]; i++);
	j = i - 1;
	while (--i >= 0) {
	    fprintf(fp, "    /* %s = %s */\n", objects_dotform[i], objects[i]);
	    fprintf(fp, "    temp_vb = ");


	    fprintf(fp, "%s_GetTrapVar(MakeOIDFromDot(\"%s\"),\n", base, objects_dotform[i]);
	    fprintf(fp, "              %s, contextInfo);\n", objects[i]);
	    fprintf(fp, "    if (temp_vb == NULL) {\n");
	    if (i != j) {
		fprintf(fp, "        if (vb != NULL) {\n");
		fprintf(fp, "            FreeVarBindList(vb);\n");
		fprintf(fp, "        }\n");
	    }
	    fprintf(fp, "        DPRINTF((APTRAP, \"i_send_%s_trap: \"));\n", trap_ptr->name);
	    fprintf(fp, "        DPRINTF((APTRAP, \"%s_GetTrapVar() failed\\n\"));\n", base);
	    fprintf(fp, "        return -1;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "    \n");
	    fprintf(fp, "    temp_vb->next_var = vb;\n");
	    fprintf(fp, "    vb = temp_vb;\n");
	    fprintf(fp, "\n");
	}

	/* now find enterprise and entV2Trap */
	if (trap_ptr->number == -1) {
	    /* It is defined as a v2 trap */
	    sprintf(entV2Trap, "%s", trap_ptr->enterprise);
	}
	else {
	    /* It is defined as a v1 trap */
	    if (strcmp(trap_ptr->enterprise, "snmp") == 0) {
		strcpy(entV2Trap, "NULL");
		/* standard trap */
		fprintf(fp, "    if ((enterprise = CloneOID(systemData.sysObjectID)) == NULL) {\n");
		fprintf(fp, "        if (vb != NULL) {\n");
		fprintf(fp, "            FreeVarBindList(vb);\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "        DPRINTF((APTRAP, \"i_send_%s_trap: \"));\n", trap_ptr->name);
		fprintf(fp, "        DPRINTF((APTRAP, \"CloneOID failed for enterprise OID\\n\"));\n");
		fprintf(fp, "        return -1;\n");
		fprintf(fp, "    }\n");
		fprintf(fp, "\n");
	    }
	    else {
		sprintf(entV2Trap, "%s.0.%ld", trap_ptr->enterprise, specTrap);
		/* enterprise specific */
		enterprise_ptr = trap_ptr->enterprise;
		for (ptr = sorted_root; ptr != NULL; ptr = ptr->next_sorted) {
		    if (strcmp(trap_ptr->enterprise, ptr->name) == 0) {
			enterprise_ptr = ptr->oid_fleshed_str;
			break;
		    }
		}
		fprintf(fp, "    /* %s = %s */\n", enterprise_ptr, trap_ptr->enterprise);
		fprintf(fp, "    if ((enterprise = MakeOIDFromDot(\"%s\")) == NULL) {\n", enterprise_ptr);
		fprintf(fp, "        if (vb != NULL) {\n");
		fprintf(fp, "            FreeVarBindList(vb);\n");
		fprintf(fp, "        }\n");
		fprintf(fp, "        DPRINTF((APTRAP, \"i_send_%s_trap: \"));\n", trap_ptr->name);
		fprintf(fp, "        DPRINTF((APTRAP, \"MakeOIDFromDot failed for enterprise OID\\n\"));\n");
		fprintf(fp, "        return -1;\n");
		fprintf(fp, "    }\n");
		fprintf(fp, "\n");
	    }
	}

	NumberForm(entV2Trap, buff);
	if (strcmp(entV2Trap, "NULL") != 0) {
	    fprintf(fp, "    /* %s = %s */\n", buff, entV2Trap);
	}
	fprintf(fp, "    DPRINTF((APTRAP, \"Sending %s Trap\\n\"));\n", trap_ptr->name);
	fprintf(fp, "    do_trap(%ld, %ld, vb, enterprise, %s);\n", genTrap, specTrap, buff);
	fprintf(fp, "    FreeOID(enterprise);\n");
	fprintf(fp, "    return 0;\n");
	fprintf(fp, "}\n");
	fprintf(fp, "\n");

    }
    fclose(fp);
    fclose(fp_h);

    return 1;
}
