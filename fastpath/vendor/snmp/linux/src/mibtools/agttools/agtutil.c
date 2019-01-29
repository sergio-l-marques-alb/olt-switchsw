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

int
isWritable(struct OID_INFO *ptr)
{
    if ( (strcmp(ptr->oid_access, READ_WRITE_STR) == 0) ||
         (strcmp(ptr->oid_access, READ_CREATE_STR) == 0) ) {
        return 1;
    }
    return 0;
}

char           *
get_string(type, name, index)
    char           *type;
    char           *name;
    int             index;
{
    static char     return_string[8][1024];
    char            tmp_str[128];
    char            buff[128];

    tmp_str[0] = '\0';
    strcpy(buff, type);

    if ((strcmp(buff, "INTEGER") == 0) || (strcmp(buff, "Integer32") == 0)) {

	sprintf(return_string[TEST], "value->sl_value;");
        return_string[CLONE][0] = '\0';
	sprintf(return_string[INST], "int");
	sprintf(return_string[DP], "&data->%s", name);
        return_string[FREE][0] = '\0';
	if (index == FREE) {
	    return (return_string[FREE]);
	}
	sprintf(return_string[VALUE], "sl_value");

    }
    else if ((strcmp(buff, "TimeTicks") == 0) ||
	   (strcmp(buff, "Gauge") == 0) || (strcmp(buff, "Counter") == 0) ||
	   (strcmp(buff, "Counter32") == 0) || (strcmp(buff, "Gauge32") == 0) ||
	   (strcmp(buff, "Unsigned32") == 0) ||(strcmp(buff, "UInteger32") == 0)) {

	sprintf(return_string[TEST], "value->ul_value;");
        return_string[CLONE][0] = '\0';
	sprintf(return_string[INST], "uint");
	sprintf(return_string[DP], "&data->%s", name);
        return_string[FREE][0] = '\0';
	if (index == FREE) {
	    return (return_string[FREE]);
	}
	sprintf(return_string[VALUE], "ul_value");

    }
    else if ((strcmp(buff, "IpAddress") == 0)) {
	sprintf(return_string[TEST], "OctetStringToIP(value->os_value);");
        return_string[CLONE][0] = '\0';
	sprintf(return_string[INST], "ip");
	sprintf(return_string[DP],
		"IPToOctetString(data->%s)", name);
        return_string[FREE][0] = '\0';
	if (index == FREE) {
	    return (return_string[FREE]);
	}
	sprintf(return_string[VALUE], "os_value");
    }
    else if ((strcmp(buff, "ObjectID") == 0)) {
	sprintf(return_string[TEST], "\n            CloneOID(value->oid_value);\n");
	sprintf(return_string[CLONE], "CloneOID");
	sprintf(return_string[INST], "x_oid");
	sprintf(return_string[DP],
		"CloneOID(data->%s)", name);
	sprintf(tmp_str, "FreeOID(data->%s);\n", name);
	sprintf(return_string[VALUE], "oid_value");

    }
    else if ((strcmp(buff, "NetworkAddress") == 0)) {

	sprintf(return_string[TEST], "\n         CloneOctetString(value->os_value);\n");
	sprintf(return_string[CLONE], "CloneOctetString");
	sprintf(return_string[INST], "x_octetstring");
	sprintf(return_string[DP], "CloneOctetString(data->%s)", name);
	sprintf(tmp_str, "FreeOctetString(data->%s);\n", name);
	sprintf(return_string[VALUE], "os_value");

    }
    else if (strcmp(buff, "Bits") == 0) {
	sprintf(return_string[TEST], "\n         CloneOctetString(value->os_value);\n");
	sprintf(return_string[CLONE], "CloneOctetString");
	sprintf(return_string[INST], "x_octetstring");
	sprintf(return_string[DP], "CloneOctetString(data->%s)", name);
	sprintf(tmp_str, "FreeOctetString(data->%s);\n", name);
	sprintf(return_string[VALUE], "os_value");

    }
    else if ((strcmp(buff, "OctetString") == 0) ||
	     (strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "Opaque") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	sprintf(return_string[TEST], "\n         CloneOctetString(value->os_value);\n");
	sprintf(return_string[CLONE], "CloneOctetString");
	sprintf(return_string[INST], "x_octetstring");
	sprintf(return_string[DP], "CloneOctetString(data->%s)", name);
	sprintf(tmp_str, "FreeOctetString(data->%s);\n", name);
	sprintf(return_string[VALUE], "os_value");

    }
    else if ((strcmp(buff, "Counter64") == 0)) {
	sprintf(return_string[TEST], "\n         CloneUInt64(value->uint64_value);\n");
	sprintf(return_string[CLONE], "CloneUInt64");
	sprintf(return_string[INST], "int64");
	sprintf(return_string[DP], "CloneUInt64(data->%s)", name);
	sprintf(tmp_str, "FreeUInt64(data->%s);\n", name);
	sprintf(return_string[VALUE], "uint64_value");

    }
    else {
	/* should never happpen */
	fprintf(stderr, "get_string(): unknown type: %s\n", buff);
	exit(-1);
    }

    sprintf(return_string[FREE], "       %s", tmp_str);

    return (return_string[index]);
}

int
print_x_to_inst(fp, index, check)
    FILE           *fp;
    struct _index_array *index;
    int            *check;
{

    char            buff[128];
    int             i;

    strcpy(buff, index->asn1_type);

    if ((strcmp(buff, "INTEGER") == 0) ||
	(strcmp(buff, "Integer32") == 0) ||
	(strcmp(buff, "TimeTicks") == 0) ||
	(strcmp(buff, "Gauge") == 0) ||
	(strcmp(buff, "Counter") == 0) ||
	(strcmp(buff, "Counter32") == 0) ||
	(strcmp(buff, "Gauge32") == 0) ||
	(strcmp(buff, "Unsigned32") == 0) ||
	(strcmp(buff, "UInteger32") == 0)) {

	fprintf(fp, "        inst.oid_ptr[index++] = (unsigned long) data->%s;\n", index->name);
	if (*check) {
	    fprintf(fp, "        if (index > MAX_OID_SIZE) {\n");
	    fprintf(fp, "            arg = -1;\n");
	    fprintf(fp, "            index--;\n");
	    fprintf(fp, "        }\n");
	}
    }
    else if ((strcmp(buff, "IpAddress") == 0)) {
	for (i = 0; i < 4; i++) {
	    fprintf(fp, "        inst.oid_ptr[index++] = ");
	    fprintf(fp, "(data->%s >> %d) & 0xff;\n", index->name, (3-i)*8);
	    if (*check) {
		fprintf(fp, "        if (index > MAX_OID_SIZE) {\n");
		fprintf(fp, "            arg = -1;\n");;
		fprintf(fp, "            index--;\n");
		fprintf(fp, "        }\n");
	    }
	}

    }
    else if ((strcmp(buff, "ObjectID") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "        inst.oid_ptr[index++] = data->%s->length;\n", index->name);
	}
	fprintf(fp, "        for(i = 0; i < data->%s->length; i++) {\n", index->name);
	fprintf(fp, "            inst.oid_ptr[index++] = (unsigned long) data->%s->oid_ptr[i];\n", index->name);
	*check = 1;
	fprintf(fp, "            if (index > MAX_OID_SIZE) {\n");
	fprintf(fp, "                arg = -1;\n");
	fprintf(fp, "                index--;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "        \n");

    }
    else if ((strcmp(buff, "NetworkAddress") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "        inst.oid_ptr[index++] = data->%s->length;\n", index->name);
	}

	fprintf(fp, "        for(i = 0; i < data->%s->length; i++) {\n", index->name);
	fprintf(fp, "            inst.oid_ptr[index++] = (unsigned long) data->%s->octet_ptr[i];\n", index->name);
	*check = 1;
	fprintf(fp, "            if (index > MAX_OID_SIZE) {\n");
	fprintf(fp, "                arg = -1;\n");
	fprintf(fp, "                index--;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "        \n");

    }
    else if ((strcmp(buff, "OctetString") == 0) ||
	     (strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "Bits") == 0) ||
	     (strcmp(buff, "Opaque") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "        inst.oid_ptr[index++] = data->%s->length;\n", index->name);
	}

	fprintf(fp, "        for(i = 0; i < data->%s->length; i++) {\n", index->name);
	fprintf(fp, "            inst.oid_ptr[index++] = (unsigned long) data->%s->octet_ptr[i];\n", index->name);
	*check = 1;
	fprintf(fp, "            if (index > MAX_OID_SIZE) {\n");
	fprintf(fp, "                arg = -1;\n");
	fprintf(fp, "                index--;\n");
	fprintf(fp, "            }\n");
	fprintf(fp, "        }\n");
	fprintf(fp, "        \n");

    }
    else if ((strcmp(buff, "Counter64") == 0)) {
	fprintf(stderr, "Counter64 cannot be used in an index clause\n");
	exit(-1);
    }
    else {
	/* should never happpen */
	fprintf(stderr, "print_x_to_inst(): unknown type: %s\n", buff);
	exit(-1);
    }
    return 1;
}

int
trap_print_x_to_inst(fp, index, number)
    FILE           *fp;
    struct _index_array *index;
    int             number;
{

    char            buff[128];
    int             i;
    char            name[128];

    strcpy(buff, index->asn1_type);

    sprintf(name, "%s_%d", index->name, number);

    if ((strcmp(buff, "INTEGER") == 0) ||
	(strcmp(buff, "Integer32") == 0) ||
	(strcmp(buff, "TimeTicks") == 0) ||
	(strcmp(buff, "Gauge") == 0) ||
	(strcmp(buff, "Counter") == 0) ||
	(strcmp(buff, "Counter32") == 0) ||
	(strcmp(buff, "Gauge32") == 0) ||
	(strcmp(buff, "Unsigned32") == 0) ||
	(strcmp(buff, "UInteger32") == 0)) {

	fprintf(fp, "    inst.oid_ptr[index++] = (unsigned long) %s;\n", name);
    }
    else if ((strcmp(buff, "IpAddress") == 0)) {
	for (i = 0; i < 4; i++) {
	    fprintf(fp, "    inst.oid_ptr[index++] = ((unsigned char *) ");
	    fprintf(fp, "(&%s))[%d];\n", name, i);
	}

    }
    else if ((strcmp(buff, "ObjectID") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "    inst.oid_ptr[index++] = %s->length;\n", name);
	}
	fprintf(fp, "    for(i = 0; i < %s->length; i++) {\n", name);
	fprintf(fp, "        inst.oid_ptr[index++] = (unsigned long) %s->oid_ptr[i];\n", name);
	fprintf(fp, "    }\n");

    }
    else if ((strcmp(buff, "NetworkAddress") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "    inst.oid_ptr[index++] = %s->length;\n", name);
	}

	fprintf(fp, "    for(i = 0; i < %s->length; i++) {\n", name);
	fprintf(fp, "        inst.oid_ptr[index++] = (unsigned long) %s->octet_ptr[i];\n", name);
	fprintf(fp, "    }\n");

    }
    else if ((strcmp(buff, "OctetString") == 0) ||
	     (strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "Bits") == 0) ||
	     (strcmp(buff, "Opaque") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	if ((index->IndexType == SR_VARIABLE) ||
	    (index->IndexType == SR_ASSUME_VARIABLE)) {
	    fprintf(fp, "    inst.oid_ptr[index++] = %s->length;\n", name);
	}

	fprintf(fp, "    for(i = 0; i < %s->length; i++) {\n", name);
	fprintf(fp, "        inst.oid_ptr[index++] = (unsigned long) %s->octet_ptr[i];\n", name);
	fprintf(fp, "    }\n");

    }
    else if ((strcmp(buff, "Counter64") == 0)) {
	fprintf(stderr, "Counter64 cannot be used in an index clause");
	exit(-1);
    }
    else {
	/* should never happpen */
	fprintf(stderr, "trap_print_x_to_inst(): unknown type: %s\n", buff);
	exit(-1);
    }
    return 1;
}

int
print_string(fp, index, which)
    FILE           *fp;
    struct _index_array *index;
    int             which;
{

    char            buff[128];

    strcpy(buff, index->asn1_type);

    if ((strcmp(buff, "INTEGER") == 0) ||
	(strcmp(buff, "Integer32") == 0)) {
	switch (which) {
	  case FREE:
	    break;
	  case INST_TO_X:
	    fprintf(fp, "Int");
	    break;
	  case GET_INDEX:
	    fprintf(fp, "1;\n");
	    break;
	  case INDEX_CAT:
	  case VINDEX_CAT:
	  case FINDEX_CAT:
	  case (VINDEX_CAT | FINDEX_CAT):
	    fprintf(fp, "uint");
	    break;
	  case INDEX_TYPE:
	    fprintf(fp, "INTEGER_TYPE");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "0;\n");
	    break;
	}

    }
    else if ((strcmp(buff, "TimeTicks") == 0) ||
	     (strcmp(buff, "Gauge") == 0) ||
	     (strcmp(buff, "Counter") == 0) ||
	     (strcmp(buff, "Counter32") == 0) ||
	     (strcmp(buff, "Gauge32") == 0) ||
	     (strcmp(buff, "Unsigned32") == 0) ||
	     (strcmp(buff, "UInteger32") == 0)) {
	switch (which) {
	  case FREE:
	    break;
	  case INDEX_CAT:
	  case VINDEX_CAT:
	  case FINDEX_CAT:
	  case (VINDEX_CAT | FINDEX_CAT):
	    fprintf(fp, "uint");
	    break;
	  case INST_TO_X:
	    fprintf(fp, "UInt");
	    break;
	  case GET_INDEX:
	    fprintf(fp, "1;\n");
	    break;
	  case INDEX_TYPE:
	    fprintf(fp, "INTEGER_TYPE");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "0;\n");
	    break;
	}

    }
    else if ((strcmp(buff, "IpAddress") == 0)) {
	switch (which) {
	  case FREE:
	    break;
	  case INST_TO_X:
	    fprintf(fp, "IP");
	    break;
	  case GET_INDEX:
	    fprintf(fp, "4;\n");
	    break;
	  case INDEX_CAT:
	  case VINDEX_CAT:
	  case FINDEX_CAT:
	  case (VINDEX_CAT | FINDEX_CAT):
	    fprintf(fp, "uint");
	    break;
	  case INDEX_TYPE:
	    fprintf(fp, "IP_ADDR_PRIM_TYPE");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "0;\n");
	    break;
	}

    }
    else if ((strcmp(buff, "ObjectID") == 0)) {
	switch (which) {
	  case FREE:
	    fprintf(fp, "        FreeOID(%s);\n", index->name);
	    break;
	  case INST_TO_X:
	    switch (index->IndexType) {
	      case SR_ASSUME_VARIABLE:
		print_assume_var_warning(index->name);
	      case SR_VARIABLE:
		fprintf(fp, "VariableOID");
		break;
	      case SR_IMPLIED:
		fprintf(fp, "ImpliedOID");
		break;
	      default:
		fprintf(fp, "FixedOID");
		break;
	    }
	    break;
	  case GET_INDEX:
	    switch (index->IndexType) {
	      case SR_ASSUME_VARIABLE:
	      case SR_VARIABLE:
		fprintf(fp, "GetVariableIndexLength(incoming, %s_offset);\n",
			index->name);
		break;
	      case SR_IMPLIED:
		fprintf(fp, "GetImpliedIndexLength(incoming, %s_offset);\n",
			index->name);
		break;
	      default:
		fprintf(fp, "%d;\n", index->IndexType);
		break;
	    }
	    break;
	  case (VINDEX_CAT | FINDEX_CAT):
            switch (index->IndexType) {
              case SR_ASSUME_VARIABLE:
              case SR_VARIABLE:
                fprintf(fp, "var_");
                break;
              default:
                if (index->IndexType >= 0) {
                    fprintf(fp, "fixed_");
                }
                break;
            }
	    fprintf(fp, "oid");
	    break;
	  case FINDEX_CAT:
            if (index->IndexType >= 0) {
                fprintf(fp, "fixed_");
            }
	    fprintf(fp, "oid");
	    break;
	  case VINDEX_CAT:
            switch (index->IndexType) {
              case SR_ASSUME_VARIABLE:
              case SR_VARIABLE:
                fprintf(fp, "var_");
                break;
            }
	    fprintf(fp, "oid");
	    break;
	  case INDEX_CAT:
	    fprintf(fp, "oid");
	    break;
	  case INDEX_TYPE:
	    printf("error: -m_ option cannot handle OIDs in the INDEX clause yet\n");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "MakeOID(NULL, 0);\n");
	    break;
	}

    }
    else if (strcmp(buff, "NetworkAddress") == 0) {
	switch (which) {
	  case FREE:
	    fprintf(fp, "        FreeOctetString(%s);\n", index->name);
	    break;
	  case INST_TO_X:
	    fprintf(fp, "NetworkAddress");
	    break;
	  case GET_INDEX:
	    fprintf(fp, "5;\n");
	    break;
	  case INDEX_CAT:
	  case VINDEX_CAT:
	  case FINDEX_CAT:
	  case (VINDEX_CAT | FINDEX_CAT):
	    fprintf(fp, "networkaddress");
	    break;
	  case INDEX_TYPE:
	    fprintf(fp, "OCTET_PRIM_TYPE");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "MakeOctetString(NULL, 5);\n");
	    break;
	}

    }
    else if ((strcmp(buff, "OctetString") == 0) ||
	     (strcmp(buff, "DisplayString") == 0) ||
	     (strcmp(buff, "Bits") == 0) ||
	     (strcmp(buff, "Opaque") == 0) ||
	     (strcmp(buff, "NsapAddress") == 0)) {
	switch (which) {
	  case FREE:
	    fprintf(fp, "        FreeOctetString(%s);\n", index->name);
	    break;
	  case INST_TO_X:
	    switch (index->IndexType) {
	      case SR_ASSUME_VARIABLE:
		print_assume_var_warning(index->name);
	      case SR_VARIABLE:
		fprintf(fp, "VariableOctetString");
		break;
	      case SR_IMPLIED:
		fprintf(fp, "ImpliedOctetString");
		break;
	      default:
		fprintf(fp, "FixedOctetString");
		break;
	    }
	    break;
	  case GET_INDEX:
	    switch (index->IndexType) {
	      case SR_ASSUME_VARIABLE:
	      case SR_VARIABLE:
		fprintf(fp, "GetVariableIndexLength(incoming, %s_offset);\n",
			index->name);
		break;
	      case SR_IMPLIED:
		fprintf(fp, "GetImpliedIndexLength(incoming, %s_offset);\n",
			index->name);
		break;
	      default:
		fprintf(fp, "%d;\n", index->IndexType);
		break;
	    }
	    break;
	  case (VINDEX_CAT | FINDEX_CAT):
            switch (index->IndexType) {
              case SR_ASSUME_VARIABLE:
              case SR_VARIABLE:
                fprintf(fp, "var_");
                break;
              default:
                if (index->IndexType >= 0) {
                    fprintf(fp, "fixed_");
                }
            }
	    fprintf(fp, "octet");
	    break;
	  case FINDEX_CAT:
            if (index->IndexType >= 0) {
                fprintf(fp, "fixed_");
            }
	    fprintf(fp, "octet");
	    break;
	  case VINDEX_CAT:
            switch (index->IndexType) {
              case SR_ASSUME_VARIABLE:
              case SR_VARIABLE:
                fprintf(fp, "var_");
                break;
            }
	    fprintf(fp, "octet");
	    break;
	  case INDEX_CAT:
	    fprintf(fp, "octet");
	    break;
	  case INDEX_TYPE:
	    printf("error: -m_ option cannot handle OCTET STRINGs in the INDEX clause yet\n");
	    break;
	  case ZERO_ASSIGNMENT:
	    switch (index->IndexType) {
	       case SR_ASSUME_VARIABLE:
	       case SR_VARIABLE:
	       case SR_IMPLIED:
	          fprintf(fp, "MakeOctetString(NULL, 0);\n");
	          break;
               default:
                  /* Fixed-length OctetString */
	          fprintf(fp, "MakeOctetString(NULL, %d);\n", index->IndexType);
	          break;
	    }
	}

    }
    else if ((strcmp(buff, "Counter64") == 0)) {
	switch (which) {
	  case FREE:
	    fprintf(fp, "        FreeUInt64(%s);\n", index->name);
	    break;
	  case INST_TO_X:
	    fprintf(fp, "Counter64");
	    break;
	  case GET_INDEX:
	    fprintf(fp, "get_counter64_index_length(incoming, %s_offset\n",
		    index->name);
	    break;
	  case INDEX_CAT:
	  case VINDEX_CAT:
	  case FINDEX_CAT:
	  case (VINDEX_CAT | FINDEX_CAT):
	    fprintf(fp, "uint64");
	    break;
	  case INDEX_TYPE:
	    printf("error: -m_ option cannot handle Counter64s in the INDEX clause yet\n");
	    break;
	  case ZERO_ASSIGNMENT:
	    fprintf(fp, "0;\n");
	    break;
	}

    }
    else {
	/* should never happpen */
	fprintf(stderr, "print_string(): unknown type: %s\n", buff);
	exit(-1);
    }
    return 1;
}


/* In SNMPv2, the index should be not-accessible,
 * but in SNMPv1, an index could be read-write.
 * If the index is read-write, then we must
 * make sure that the value matches the instance.
 */
int
WhosBuriedInGrantsTombCheck(fp, temp_ptr)
    FILE           *fp;
    struct OID_INFO *temp_ptr;
{
    char           *type = temp_ptr->oid_prim_type;

    if ((strcmp(type, "INTEGER") == 0) ||
	(strcmp(type, "Integer32") == 0)) {

	fprintf(fp, "     if (%s != value->sl_value) {\n", temp_ptr->name);
	fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "     \n");

    }
    else if ((strcmp(type, "TimeTicks") == 0) ||
	     (strcmp(type, "Gauge") == 0) ||
	     (strcmp(type, "Counter") == 0) ||
	     (strcmp(type, "Counter32") == 0) ||
	     (strcmp(type, "Gauge32") == 0) ||
	     (strcmp(type, "Unsigned32") == 0) ||
	     (strcmp(type, "UInteger32") == 0)) {

	fprintf(fp, "     if (%s != value->ul_value) {\n", temp_ptr->name);
	fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "     \n");

    }
    else if ((strcmp(type, "ObjectID") == 0)) {
	fprintf(fp, "     if (CmpOID(value->oid_value, ((%s_t *) (dp->data))->%s) != 0) {\n",temp_ptr->parent_ptr->name, temp_ptr->name);
	fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "        \n");

    }
    else if ((strcmp(type, "IpAddress") == 0)) {
	fprintf(fp, "     if (((%s_t *) (dp->data))->%s != OctetStringToIP(value->os_value)) {\n",temp_ptr->parent_ptr->name, temp_ptr->name);
	fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "\n");

    }
    else if ((strcmp(type, "OctetString") == 0) ||
	     (strcmp(type, "DisplayString") == 0) ||
	     (strcmp(type, "Bits") == 0) ||
	     (strcmp(type, "Opaque") == 0) ||
	     (strcmp(type, "NsapAddress") == 0)) {

	fprintf(fp, "     if (CmpOctetStrings(value->os_value, ((%s_t *) (dp->data))->%s) != 0) {\n",temp_ptr->parent_ptr->name, temp_ptr->name);
	fprintf(fp, "        return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "\n");
    }

    return 1;
}


/* In some cases, we can't be sure from the mosy output if an index of
 * type oid or octet string should be fixed length or variable length.
 * In this case, we print a warning.
 */
int
print_assume_var_warning(name)
    char           *name;
{
    /*
     * this routine will get called once when writing the get routine and
     * once for the test routine, so only print the warning for the get
     * routine. Also, since we know contextIdentity is right, we won't print
     * the warning for this special case (this allows the standard MIBs to
     * compile without warning).
     */
    if ((!writing_get_method) || (strcmp(name, "contextIdentity") == 0)) {
	return 0;
    }

    return 1;
}

/* If this is an enumerated integer, make sure the
 * value is one of the enumerations. If it has a
 * size range, make sure the value is within that
 * range.
 */
int
CheckRanges(fp, ptr)
    FILE           *fp;
    struct OID_INFO *ptr;
{
    enumeration_t  *temp_enum;
    size_range_t   *size;
    int             contig = 1;
    int             final_val;

    /* see if this entry is an enumeration */
    if (ptr->enumer != NULL && !strcmp(ptr->oid_prim_type, "INTEGER")) {
	/* see if the values are contiguous */
	for (temp_enum = ptr->enumer; temp_enum->next; temp_enum = temp_enum->next) {
	    if ((temp_enum->val + 1) != temp_enum->next->val) {
		contig = 0;
	    }
	}
	final_val = temp_enum->val;

	if (contig) {
	    fprintf(fp, "     if ((value->sl_value < %d) || (value->sl_value > %d)) {\n", ptr->enumer->val, final_val);
	    fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	    fprintf(fp, "     }\n");
	    fprintf(fp, "\n");

#ifdef NO_LONGER_USED
	    if (storage_type) {
		/* if this is a storageType variable, emit appropriate code */
		if (!strcmp(ptr->oid_type, storageTypeString) ||
		    !strcmp(ptr->oid_type, memoryTypeString)) {
		    fprintf(fp, "     /* restrict storage type values according to the textual\n");
		    fprintf(fp, "      * convention specs */\n");
		    fprintf(fp, "     if (value->sl_value == D_%s_permanent\n", ptr->name);
		    fprintf(fp, "     ||  value->sl_value == D_%s_readOnly) {\n", ptr->name);
		    fprintf(fp, "         /* don't allow readOnly or permanent objects to be created\n");
		    fprintf(fp, "          * dynamically, or to be modified once they exist */\n");
		    fprintf(fp, "         if(dp->undodata == NULL\n");
		    fprintf(fp, "         || ((%s_t *) dp->undodata)->%s\n", ptr->parent_ptr->name, ptr->name);
		    fprintf(fp, "                 != value->sl_value) {\n");
		    fprintf(fp, "             return INCONSISTENT_VALUE_ERROR;\n");
		    fprintf(fp, "         }\n");
		    fprintf(fp, "     } else\n");
		    fprintf(fp, "     if(dp->undodata != NULL\n");
		    fprintf(fp, "     && (((%s_t *) dp->undodata)->%s\n", ptr->parent_ptr->name, ptr->name);
		    fprintf(fp, "             == D_%s_permanent\n", ptr->name);
		    fprintf(fp, "         || ((%s_t *) dp->undodata)->%s\n", ptr->parent_ptr->name, ptr->name);
		    fprintf(fp, "             == D_%s_readOnly)) {\n", ptr->name);
		    fprintf(fp, "         /* don't allow permanent or readOnly objects to be\n");
		    fprintf(fp, "          * converted to volatile storage types */\n");
		    fprintf(fp, "         return INCONSISTENT_VALUE_ERROR;\n");
		    fprintf(fp, "     }\n");
		    fprintf(fp, "\n");
		}
	    }
#endif /* NO_LONGER_USED */
	}
	else {
	    fprintf(fp, "    switch (value->sl_value) {\n");
	    for (temp_enum = ptr->enumer; temp_enum; temp_enum = temp_enum->next) {
		fprintf(fp, "        case %d:\n", temp_enum->val);
	    }
	    fprintf(fp, "            break;\n");
	    fprintf(fp, "        default:\n");
	    fprintf(fp, "            return WRONG_VALUE_ERROR;\n");
	    fprintf(fp, "    }\n");
	    fprintf(fp, "\n");
	}
    }
    if (ptr->enumer != NULL && !strcmp(ptr->oid_prim_type, "Bits")) {
	/* get max enumer value */
        final_val = 0;
	for (temp_enum = ptr->enumer; temp_enum != NULL; temp_enum = temp_enum->next) {
	    if (temp_enum->val > final_val) {
		final_val = temp_enum->val;
	    }
	}
        contig = (0xff << (7 - (final_val % 8))) & 0xff;
        final_val = final_val / 8;
	fprintf(fp, "     if (value->os_value->length != %d) {\n", final_val+1);
	fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	fprintf(fp, "     }\n");
	fprintf(fp, "\n");
        if (contig != 0xff) {
	    fprintf(fp, "    value->os_value->octet_ptr[value->os_value->length-1] &= 0x%x;\n", contig);
        }
    }

    if (ptr->size != NULL) {
	if ((strcmp(ptr->oid_prim_type, "INTEGER") == 0) ||
	    (strcmp(ptr->oid_prim_type, "Integer32") == 0)) {
	    fprintf(fp, "     if ( ");
	    for (size = ptr->size; size; size = size->next) {
		fprintf(fp, "((value->sl_value < %ld) || (value->sl_value > %ld))", size->min, size->max);
		if (size->next) {
		    fprintf(fp, " &&\n");
		    fprintf(fp, "          ");
		}
	    }
	    fprintf(fp, " ) {\n");
	    fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	    fprintf(fp, "     }\n");
	    fprintf(fp, "\n");

	} else if ((strcmp(ptr->oid_prim_type, "OctetString") == 0) ||
	           (strcmp(ptr->oid_prim_type, "Opaque") == 0)) {
	    fprintf(fp, "     if ( ");
	    for (size = ptr->size; size; size = size->next) {
		fprintf(fp, "((value->os_value->length < %ld) || (value->os_value->length > %ld))", size->min, size->max);
		if (size->next) {
		    fprintf(fp, " &&\n");
		    fprintf(fp, "          ");
		}
	    }
	    fprintf(fp, " ) {\n");
	    fprintf(fp, "         return WRONG_LENGTH_ERROR;\n");
	    fprintf(fp, "     }\n");
	    fprintf(fp, "\n");
        } else if ((strcmp(ptr->oid_prim_type, "Gauge") == 0) ||
            (strcmp(ptr->oid_prim_type, "Gauge32") == 0) ||
            (strcmp(ptr->oid_prim_type, "Unsigned32") == 0) ||
            (strcmp(ptr->oid_prim_type, "Counter") == 0) ||
            (strcmp(ptr->oid_prim_type, "Counter32") == 0) ||
            (strcmp(ptr->oid_prim_type, "TimeTicks") == 0)) {
            fprintf(fp, "     if ( ");
            for (size = ptr->size; size; size = size->next) {
                fprintf(fp, "((value->ul_value < %u) || (value->ul_value > %u))", (unsigned) size->min, (unsigned) size->max);
                if (size->next) {
                    fprintf(fp, " &&\n");
                    fprintf(fp, "          ");
                }
            }
            fprintf(fp, " ) {\n");
            fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
            fprintf(fp, "     }\n");
            fprintf(fp, "\n");
        }
    }

    /* is this a rowstatus variable? */
    if (strcmp(ptr->oid_type, rowStatusString) == 0) {
	if (row_status != 0) {
	    /* use extended row status support */
	    fprintf(fp, "     error_status = Check%s(value->sl_value,\n",
		    ptr->oid_type);
	    fprintf(fp, "         %s == NULL ? 0L : %s->%s);\n",
		    ptr->parent_ptr->name, ptr->parent_ptr->name, ptr->name);
	    fprintf(fp, "     if (error_status != 0) return(error_status);\n");
	    fprintf(fp, "\n");
	}
	else {
	    /* use minimal row status support */
	    fprintf(fp, "     if (value->sl_value == D_%s_notReady) {\n",
		    ptr->name);
	    fprintf(fp, "         return WRONG_VALUE_ERROR;\n");
	    fprintf(fp, "     }\n");
	    fprintf(fp, "\n");
            if (ptr->parent_ptr->augments == NULL) {
	      fprintf(fp, "     if (%s == NULL) { \t/* creating a new row */\n",
		      ptr->parent_ptr->name);  /* } */
            } else {
              struct OID_INFO *temp_ptr; 
              int found = 0;
              for (temp_ptr = sorted_root; temp_ptr; 
                   temp_ptr = temp_ptr->next_sorted) {
                  if (strcmp(temp_ptr->name, ptr->parent_ptr->augments) == 0) { 
                      found++;
                      break;
                  }
              }
              if (!found) {
                  printf("ERROR: %s aguments unknown table %s\n", ptr->name,
                          ptr->augments);
                  exit(1);
              }
	      fprintf(fp, "     if (%s == NULL) { \t/* creating a new row */\n",
		      temp_ptr->name);  /* } */
            }
	    fprintf(fp, "        if((value->sl_value == D_%s_notInService) || (value->sl_value == D_%s_active)) {\n",
		    ptr->name, ptr->name);
	    fprintf(fp, "            return INCONSISTENT_VALUE_ERROR;\n");
	    fprintf(fp, "        }\n");
            /* { */
	    fprintf(fp, "     } else { \t\t\t/* modifying an existing row */\n");
	    fprintf(fp, "        if ((value->sl_value == D_%s_createAndGo) || (value->sl_value == D_%s_createAndWait)) {\n",
		    ptr->name, ptr->name);
	    fprintf(fp, "            return INCONSISTENT_VALUE_ERROR;\n");
	    fprintf(fp, "        }\n");
	    fprintf(fp, "     }\n");
	    fprintf(fp, "\n");
	}
    }

    return 1;
}

/* If a size range is given in the MIB, we never want
 * to call the k_ routine with a value that is less than
 * the lowest legal value
 */
int
print_index_size_check(fp, name)
    FILE           *fp;
    char           *name;
{
    struct OID_INFO *ptr;
    if (name == NULL) {
	return -1;
    }

    for (ptr = root; ptr != NULL; ptr = ptr->next) {
	if (strcmp(name, ptr->name) == 0) {

	    /*
	     * First see if there is a size range. Then make sure that there
	     * is only one size range ie, this will not handle INTEGER
	     * (1..3|6..10)
	     */
	    if ((ptr->size) && (ptr->size->next == NULL)) {
		fprintf(fp, "    if (searchType == NEXT) {\n");
		fprintf(fp, "        %s = MAX(%ld, %s);\n", name, ptr->size->min, name);
		fprintf(fp, "    }\n");
	    }

	    return 1;
	}
    }
    return -1;
}


/*
 * The map_variable_type_for_ber() functions maps both NetworkAddress
 * and IpAddress to IP_ADDR_PRIM_TYPE.  This is correct for BER encoding
 * purposes--both are [APPLICATION 0] (RFC 1155).  Consequently, this
 * is the correct type that should be stored in the ObjectInfo
 * structure for MIB objects of type NetworkAddress.
 * 
 * The method routines treat IpAddress and NetworkAddress differently.
 * IpAddress is stored in the family structure as an SR_UINT32.
 * NetworkAddress is stored in the family structure as an OctetString.
 * The reason is because NetworkAddress is an ASN.1 CHOICE where the
 * only choice is IpAddress, so as a table index, you have to store
 * the byte that discriminates the choice (the only discriminator
 * value ever defined for SNMP is `1', meaning IPv4 address).
 *
 * The <family>TypeTable was created for automatically-generated code
 * to handle RowStatus tables.  The type field in this structure
 * identifies how the method routines should deal with the MIB
 * object both as a member of the <family>_t structure and as a
 * table index.  Thus, here the type needs to OCTET_PRIM_TYPE.
 */
int 
map_variable_type_for_family(type, buf)
    char           *type, *buf;
{
    if (strcmp(type, "NetworkAddress") == 0) {
        /* special case */
        strcpy(buf, "OCTET_PRIM_TYPE");
        return 1;
    } else {
        /* otherwise, do the same thing as for BER */
        return map_variable_type_for_ber(type, buf);
    }
}

/*
 * output Makefile fragment
 */
void
output_c_make_file(char *base)
{
    FILE *fp;
 
    fp = open_file("MakeInclude", "");
 
    fprintf(fp, "#\n");
    fprintf(fp, "# Source files\n");
    fprintf(fp, "#\n");
    fprintf(fp, "EXTEND_SRCS = \\\n");
    fprintf(fp, "    v_%s.c k_%s.c\n", base, base);
    fprintf(fp, "\n");
 
    fprintf(fp, "#\n");
    fprintf(fp, "# Object files\n");
    fprintf(fp, "#\n");
    fprintf(fp, "EXTEND_OBJS = \\\n");
    fprintf(fp, "    $(OSOBJDIR)/v_%s.$(OBJEXT) $(OSOBJDIR)/k_%s.$(OBJEXT)\n", base, base);
    fprintf(fp, "\n");
 
    fprintf(fp, "#\n");
    fprintf(fp, "# Compile rules\n");
    fprintf(fp, "#\n");
    fprintf(fp, "$(OSOBJDIR)/k_%s.$(OBJEXT): k_%s.c %stype.h\n", base, base, base);
    fprintf(fp, "\t$(CORULE)\n");
    fprintf(fp, "\n");
    fprintf(fp, "$(OSOBJDIR)/v_%s.$(OBJEXT): v_%s.c %stype.h\n", base, base, base);
    fprintf(fp, "\t$(CORULE)\n");
    fprintf(fp, "\n");

    fclose(fp);
}

