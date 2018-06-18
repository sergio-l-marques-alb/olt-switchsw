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

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_utils.h"
#include "sr_cfg.h"
#include "snmpid.h"
#include "sr_trans.h"
#include "snmpv2.h"
#include "sr_msg.h"
#include "v1_msg.h"
#include "diag.h"
SR_FILENAME
#include "oid_lib.h"
#include "oidtranh.h"
#include "tc_time.h"
#include "prnt_lib.h"
#include "sri/sr_kwmatch.h"

SnmpLcd snmp_lcd;

#ifdef SR_MIB_TABL_H
#include "snmp-mib.h"
#endif /* SR_MIB_TABL_H */



#include <sys/types.h>

#include <sys/time.h>

#include <string.h>




#ifndef SR_NO_PRIVACY
#if (defined(SR_AES) && defined(SR_SNMPv3_PACKET))
#include "aes.h"
#include "sr_aes.h"
#endif	/* (defined(SR_AES) && defined(SR_SNMPv3_PACKET)) */
#endif /* SR_NO_PRIVACY */

#include <errno.h>



#ifdef SR_SNMPv3_PACKET
int alt_engineID = FALSE;
#ifdef SR_CONFIG_FP
void updateEngineID(ENGINE_ID *id);
#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv3_PACKET */

/* in snmpio.c */
extern int fd;
extern SR_INT32 packet_len;
extern unsigned char *packet;


/* globals.h */
extern int print_packet_stats;

static void InitUtilFileNames
    SR_PROTOTYPE((void));

int process_command_line_option
    SR_PROTOTYPE((int *argc,
                  char *argv[]));

#ifdef SR_DEBUG
struct {
    char *name;
    SR_INT32 flag;
} LogFlags[] = {
    { "-apnone", 0 },
    { "-apwarn", APWARN },
    { "-aperror", APERROR },
    { "-aptrace", APTRACE },
    { "-apall", APTIMER|APTHREAD|APWARN|APTRACE|APERROR|APCONFIG|APPACKET|APTRAP|APACCESS|APVERBOSE },
    { "-aptimer", APTIMER },
    { "-apthread", APTHREAD },
    { "-apconfig", APCONFIG },
    { "-appacket", APPACKET },
    { "-aptrap", APTRAP },
    { "-apaccess", APACCESS },
    { "-apverbose", APVERBOSE },
    { "-apuser", APUSER },
    { "-apaudit", APAUDIT },
    { "-noapwarn", -(APWARN) },
    { "-noaperror", -(APERROR) },
    { "-noaptrace", -(APTRACE) },
    { "-noaptimer", -(APTIMER) },
    { "-noapthread", -(APTHREAD) },
    { "-noapconfig", -(APCONFIG) },
    { "-noappacket", -(APPACKET) },
    { "-noaptrap", -(APTRAP) },
    { "-noapaccess", -(APACCESS) },
    { "-noapverbose", -(APVERBOSE) },
    { "-noapuser", -(APUSER) },
    { "-noapaudit", -(APAUDIT) }
};

#define NLogFlags (sizeof(LogFlags)/sizeof(*LogFlags))

#endif /* SR_DEBUG */

static void set_output_format(char *format);

#if defined(SR_SNMPv3_PACKET)
OctetString *util_local_snmpID;
OctetString *defaultContextSnmpID = NULL;
OctetString *defaultContext = NULL;
const PARSER_RECORD_TYPE *util_rt[18];
int util_rt_count = 0;

#define SIMPLE_ALG 1
#define COMPLEX_ALG 2
#define MANUAL 3
#define DEFAULT_SNMP_ENGINE_ID_SRC 1

ENGINE_ID engineID, engineID_1;

static const SnmpType TMP_ENGINE_ID_Type[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
static const SnmpType TMP_ENGINE_ID_SRC_Type[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
static const PARSER_CONVERTER TMP_ENGINE_ID_Converters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};
static const PARSER_CONVERTER TMP_ENGINE_ID_SRC_Converters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};

static const PARSER_RECORD_TYPE TMP_ENGINE_ID_Record = {
    PARSER_SCALAR, "snmpEngineID ",
    TMP_ENGINE_ID_Converters, TMP_ENGINE_ID_Type,
    NULL, &engineID.runtime_SNMP_ENGINE_ID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
    };
static const PARSER_RECORD_TYPE TMP_ENGINE_ID_SRC_Record = {
    PARSER_SCALAR, "SNMP_ENGINE_ID_SRC ",
    TMP_ENGINE_ID_SRC_Converters, TMP_ENGINE_ID_SRC_Type,
    NULL, &engineID.runtime_SNMP_ENGINE_ID_SRC, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
    };


static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_Record = {
    PARSER_SCALAR, "snmpEngineID ",
    TMP_ENGINE_ID_Converters, TMP_ENGINE_ID_Type,
    NULL, &engineID_1.runtime_SNMP_ENGINE_ID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
    };
static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_SRC_Record = {
    PARSER_SCALAR, "SNMP_ENGINE_ID_SRC ",
    TMP_ENGINE_ID_SRC_Converters, TMP_ENGINE_ID_SRC_Type,
    NULL, &engineID_1.runtime_SNMP_ENGINE_ID_SRC, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
    };

const PARSER_RECORD_TYPE *engineID_rt[3];
int engineID_rt_count;

#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifndef SR_UNSECURABLE
extern int inform_type;
extern unsigned short port_num;
#endif /* SR_UNSECURABLE */




#ifdef SR_SNMPv2_PDU
/*
 * CheckEndOfMIB: This routine checks to see if every varbind's type
 *                field is equal to END_OF_MIB_VIEW_EXCEPTION.  If
 *                every varbind matches, this routine prints out a
 *                message and returns -1. Otherwise, it returns 0 (not
 *                end of mib). If an error occurs, this routine
 *                returns -2.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_end_of_mib().
 */
int
CheckEndOfMIB(pdu_ptr)
    Pdu            *pdu_ptr;
{
    VarBind        *tmp_vb_ptr;
    int             num_exceptions;
    int             i;

    tmp_vb_ptr = NULL;
    num_exceptions = 0;

    for (i = 0; i < (int) pdu_ptr->num_varbinds; i++) {
	if (tmp_vb_ptr == NULL) {
	    tmp_vb_ptr = pdu_ptr->var_bind_list;
	}
	else {
	    tmp_vb_ptr = tmp_vb_ptr->next_var;
	}

	if (tmp_vb_ptr != NULL) {
	    if (tmp_vb_ptr->value.type != END_OF_MIB_VIEW_EXCEPTION) {
		break;
	    }
	    else if (tmp_vb_ptr->value.type == END_OF_MIB_VIEW_EXCEPTION) {
		num_exceptions++;
	    }

	}
	else {
	    DPRINTF((APWARN, "CheckEndOfMIB: Could not assign tmp_vb_ptr.\n"));
	    return (-2);
	}
    }

    if (num_exceptions == (int) pdu_ptr->num_varbinds) {
	printf("End of MIB.\n");
	return (-1);
    }
    else {
	return (0);
    }

}				/* CheckEndOfMIB() */

#endif				/* SR_SNMPv2_PDU */

/*
 * CreateVarBind:
 *
 *   The former name (pre-snmp12.1.0.0 release) was create_vb().
 */

VarBind        *
CreateVarBind(name, type, value)
    char            name[];
    char            type[];
    char            value[];
{
    OID            *oid_ptr, *oid_ptr1;
    VarBind        *vb_ptr;
    SR_INT32        sl;
    SR_UINT32       ul;
    OctetString    *os_ptr;
    unsigned char   ip_addr_buffer[4];
    unsigned char   oidtype;
    SR_UINT32       temp_ip_addr;

    sl = (SR_INT32)0;
    ul = (SR_UINT32)0;

    oid_ptr = MakeOIDFromDot(name);
    if (oid_ptr == NULL) {
	DPRINTF((APWARN, "Cannot translate variable class: %s\n", name));
	exit(1);
    }

    if (strcmp(type, "-i") == 0) {

	if ((sscanf(value, INT32_HEX_FORMAT, &sl)) != 1) {
	  if ((sscanf(value, INT32_FORMAT, &sl)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	  }
	}

	oidtype = INTEGER_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &sl);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-o") == 0) {
	if ((os_ptr = MakeOctetStringFromHex(value)) == NULL) {
            if ((os_ptr = MakeOctetStringFromText(value)) == NULL) {
                return NULL;
            }
	}
	oidtype = OCTET_PRIM_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) os_ptr);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	os_ptr = NULL;		/* do not free the octetstring, it is part of
				 * the vb. */
	/* end bug fix */
	return (vb_ptr);
    }

    if (strcmp(type, "-D") == 0) {
	os_ptr = MakeOctetString((unsigned char *) value, (SR_INT32) strlen(value));
	if (os_ptr == NULL) {
	    return NULL;
	}
	oidtype = OCTET_PRIM_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) os_ptr);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	os_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-T") == 0) {
        os_ptr = TextToDateAndTime(value);
	if (os_ptr == NULL) {
            printf("\nError: DateAndTime should be in the form: YYYY-Mon-DD,HH:MM:SS.x\n"); 
            printf("       example: 2004-Jan-1,12:30:00.0\n\n");
	    return NULL;
        }
	oidtype = OCTET_PRIM_TYPE; 
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) os_ptr);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	os_ptr = NULL;
	return (vb_ptr);
    }

#if defined(SR_SNMPv2_PDU)
    if (strcmp(type, "-b") == 0) {
	os_ptr = MakeBitsValue(name, value);
	if (os_ptr == NULL) {
	    return NULL;
	}
	oidtype = OCTET_PRIM_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) os_ptr);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	os_ptr = NULL;
	return (vb_ptr);
    }
#endif	/* defined(SR_SNMPv2_PDU) */

    if (strcmp(type, "-d") == 0) {
	if ((oid_ptr1 = MakeOIDFromDot(value)) == NULL) {
	    return NULL;
	}
	oidtype = OBJECT_ID_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) oid_ptr1);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	oid_ptr1 = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-a") == 0) {
	temp_ip_addr = ntohl(inet_addr(value));
	ip_addr_buffer[0] = (unsigned char) ((temp_ip_addr >> 24) & 0xff);
	ip_addr_buffer[1] = (unsigned char) ((temp_ip_addr >> 16) & 0xff);
	ip_addr_buffer[2] = (unsigned char) ((temp_ip_addr >> 8) & 0xff);
	ip_addr_buffer[3] = (unsigned char) (temp_ip_addr & 0xff);

	os_ptr = MakeOctetString(ip_addr_buffer, ((SR_INT32)4));

	oidtype = IP_ADDR_PRIM_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
				      (void *) os_ptr);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	os_ptr = NULL;		/* do not free the octetstring, it is part of
				 * the vb. */
	/* end bug fix */
	return (vb_ptr);
    }

    if (strcmp(type, "-c") == 0) {
	if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	}
	oidtype = COUNTER_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-j") == 0) {
        UInt64 *u64; 
	if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	}
	oidtype = COUNTER_64_TYPE;
        if (!(u64 = (UInt64 *) malloc(sizeof(UInt64)))) {
            DPRINTF((APERROR, "CreateVarBind: malloc failure on UInt64\n"));
            return NULL;
        }
        u64->little_end = ul;
        u64->big_end = 0;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, 
                                      (void *) u64);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-g") == 0) {
	if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	}
	oidtype = GAUGE_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-t") == 0) {

	if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	}

	oidtype = TIME_TICKS_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    if (strcmp(type, "-N") == 0) {

	if ((sscanf(value, INT32_FORMAT, &sl)) != 1) {
	    DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
	    return NULL;
	}

	oidtype = NULL_TYPE;
	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) NULL);
	FreeOID(oid_ptr);
	oid_ptr = NULL;
	return (vb_ptr);
    }

    return (NULL);
}				/* CreateVarBind() */

/*
 * GetVariableType
 *
 * Convert the type constant from mib_oid_table into a type code (like -i or -D)
 *
 * t_codes contains the appropriate code for each type listed in t_values
 */
static char *t_codes[] = {
        "-D",
        "-o",
        "-o",
        "-o",
        "-a",
        "-d",
        "-i",
        "-i",
        "-g",
        "-c",
        "-i",
#ifdef SR_SNMPv2_PDU
        "-c",
        "-g",
        "-j",
        "-i",
        "-b",
#endif /* SR_SNMPv2_PDU */
        "-T",
        (char *)0
};

#ifdef SR_MIB_TREE_H
static short t_values[] = {
        DISPLAY_STRING_TYPE,
        OCTET_PRIM_TYPE,
        PHYS_ADDRESS_TYPE,
        NETWORK_ADDRESS_TYPE,
        IP_ADDR_PRIM_TYPE,
        OBJECT_ID_TYPE,
        TIME_TICKS_TYPE,
        INTEGER_TYPE,
        GAUGE_TYPE,
        COUNTER_TYPE,
        SERVICES_TYPE,
#ifdef SR_SNMPv2_PDU
        COUNTER_32_TYPE,
        GAUGE_32_TYPE,
        COUNTER_64_TYPE,
        INTEGER_32_TYPE,
        BIT_STRING_TYPE,
#endif /* SR_SNMPv2_PDU */
        OCTET_PRIM_TYPE,  /* DateAndTime */
        0
};

int
GetVariableType(ote, type)
    OID_TREE_ELE   *ote;
    char           *type;
{
    int j;
    for (j=0; t_values[j]!=0; j++) {
        if (t_values[j] == ote->type) {
            if (strlen(t_codes[j]) == 0) {
                return -1;
            }
            strcpy(type, t_codes[j]);
            return 1;
        }
    }
    return -1;
}

/*
 * GetEnumFromString:
 *
 * The former name was get_enumerated_int_from_string() (pre-snmp12 release).
 */
int
GetEnumFromString(ote, value, number)
    OID_TREE_ELE   *ote;
    char           *value;
    SR_INT32       *number;
{
    struct enumer  *elist;

    if (ote == NULL) {
        return -1;
    }
    for (elist = ote->enumer; elist; elist = elist->next) {
        if (strcmp(elist->name, value) == 0) {
            *number = elist->number;
            return 1;
        }
    }

    return -1;
}
#endif /* SR_MIB_TREE_H */


/*
 * BuildNextVarBind:
 *
 * BuildNextVarBind takes the command line in the form of either a
 * <name type value> tuple or a <name value> tuple, determines the
 * variable type if necessary and returns a new varbind on success.
 *
 * The parameter argv is a pointer to what was typed on the command
 * line when invoking the manager utility.
 *
 * The parameter in_index is the index into the command line, argv.
 *
 *  The former name (pre-snmp12.1.0.0 release) was build_next_vb().
 */
#ifdef SR_MIB_TREE_H
VarBind        *
BuildNextVarBind(argv, in_index)
    char           *argv[];
    int            *in_index;
{
    SR_INT32        number;
    OID            *oid_ptr;
    char            type[3];
    int             offset;
    VarBind        *vb_ptr;
    int             i;
    OID_TREE_ELE   *ote = NULL;

    i = *in_index;

    if (!isdigit((unsigned char)*argv[i])) {
        ote = AP_GetMIBNodeFromDot(argv[i]);
    }

    if (!argv[i] || !argv[i + 1]) {
        FreeOTE(ote);
	return (NULL);
    }
    if ((*argv[i + 1] == '-') && (!isdigit((unsigned char)argv[i + 1][1]))) {
	if (!argv[i + 2]) {
            FreeOTE(ote);
	    return (NULL);
	}
	offset = 2;
	strncpy(type, argv[i + 1], 2);
	type[2] = '\0';
    }
    else {
	if (ote == NULL) {
	    DPRINTF((APWARN, "do not know type of %s\n", argv[i]));
	    return NULL;
	}
	offset = 1;
	if ((GetVariableType(ote, type)) < 0) {
	    DPRINTF((APWARN, "do not know type of %s\n", argv[i]));
            FreeOTE(ote);
	    return NULL;
	}
    }

    if (((ote != NULL) && strcmp("-i", type) == 0) &&
	(!isdigit((unsigned char)*argv[i + offset])) &&
	(GetEnumFromString(ote, argv[i + offset], &number) > 0)) {


	oid_ptr = MakeOIDFromDot(argv[i]);
	if (oid_ptr == NULL) {
	    DPRINTF((APWARN, "Cannot translate variable class: %s\n", argv[i]));
            FreeOTE(ote);
	    return (NULL);
	}

	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
				      (unsigned char) INTEGER_TYPE,
				      (void *) &number);
        FreeOID(oid_ptr);
	oid_ptr = NULL;

    }
    else {
	vb_ptr = CreateVarBind(argv[i], type, argv[i + offset]);
    }
    if (vb_ptr == NULL) {
	printf("Unable to create request from the given arguments\n\n");
        FreeOTE(ote);
	return (NULL);
    }

    i = i + 1 + offset;
    *in_index = i;
    FreeOTE(ote);
    return (vb_ptr);
}
#endif /* SR_MIB_TREE_H */
#ifdef SR_MIB_TABL_H
VarBind        *
BuildNextVarBind(argv, in_index)
    char           *argv[];
    int            *in_index;
{
    SR_INT32        number;
    OID            *oid_ptr;
    char            type[3];
    VarBind        *vb_ptr;
    int             i;

    i = *in_index;

    if (!argv[i] || !argv[i + 1]) {
	return (NULL);
    }
    if ((*argv[i + 1] == '-') && (!isdigit(argv[i + 1][1]))) {
	if (!argv[i + 2]) {
	    return (NULL);
	}
	strncpy(type, argv[i + 1], 2);
	type[2] = '\0';
    }
    else {
	DPRINTF((APWARN, "must provide type of %s\n", argv[i]));
        return NULL;
    }

    vb_ptr = CreateVarBind(argv[i], type, argv[i + 2]);
    if (vb_ptr == NULL) {
	printf("Unable to create request from the given arguments\n\n");
	return (NULL);
    }

    i = i + 3;
    *in_index = i;
    return (vb_ptr);
}
#endif /* SR_MIB_TABL_H */

/*
 * InitUtilFileNames: initialize the filenames with the directory from
 *            the environment variable SR_MGR_CONF_DIR.  If the variable
 *            is missing or "too long," then the prefix (and hence the
 *            directory) is set to /etc.  "Too long" is the
 *            FNBUFSIZ minus the maximum length of a config file
 *            name.  As of 3/3/93, the longest config file name was 16,
 *            so 17 was arbitrarily chosen.
 *
 * The former name (pre-snmp12.1.0.0 release) was init_util_fnames().
 */

static void
InitUtilFileNames()
{
    char           *prefix;

    if ((prefix = getenv("SR_MGR_CONF_DIR")) == NULL) {
	prefix = SR_MGR_CONF_FILE_PREFIX;
    }

#ifdef SYSV
    if (strlen(prefix) > (unsigned) (FNBUFSIZ - 17)) {
	DPRINTF((APCONFIG, "InitUtilFileNames: filename prefix too long\n defaulting to %s\n", SR_MGR_CONF_FILE_PREFIX));
	prefix = SR_MGR_CONF_FILE_PREFIX;
    }
#else				/* not SYSV */
    if (strlen(prefix) > (unsigned) (FNBUFSIZ - 17)) {
	DPRINTF((APCONFIG, "InitUtilFileNames: filename prefix too long\n defaulting to %s\n", SR_MGR_CONF_FILE_PREFIX));
	prefix = SR_MGR_CONF_FILE_PREFIX;
    }
#endif				/* SYSV */

    if (prefix[strlen(prefix) - 1] == '/') {
	prefix[strlen(prefix) - 1] = '\0';
    }


#ifdef SR_MIB_TREE_H
    sprintf(fn_snmpinfo_dat, "%s/%s", prefix, FN_SNMPINFO_DAT);
#endif /* SR_MIB_TREE_H */

    sprintf(fn_mgr_file, "%s/%s", prefix, FN_MGR_FILE);
}

/*
 * process_command_line_option:
 *
 * This function will process a single command line option.  If a valid option
 * is found, it returns 1.  If there were no valid options, it returns 0.
 */
int
process_command_line_option(argc, argv)
    int *argc;
    char *argv[];
{
#if (defined(SR_SNMPv3_PACKET) || defined(SR_DEBUG))
    int i;
#endif	/* (defined(SR_SNMPv3_PACKET) || defined(SR_DEBUG)) */

    if (*argc <= 1) {
        return 0;
    }

    if (!strcmp(argv[1], "-d")) {
#ifdef SR_DEBUG
        SetLogLevel(APALL);
#endif /* SR_DEBUG */
        if (*argc > 2) {
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
        }
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }

#ifdef SR_DEBUG
    for (i = 0; i < NLogFlags; i++) {
        if (!strcmp(argv[1], LogFlags[i].name)) {
            if (LogFlags[i].flag == 0) {
                SetLogLevel(LogFlags[i].flag);
            }
            else if (LogFlags[i].flag > 0) {
                SetLogLevel(GetLogLevel() | LogFlags[i].flag);
	    }
            else {
                SetLogLevel(GetLogLevel() & ~(-(LogFlags[i].flag)));
            }
            if (*argc > 2) {
                memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
            }
            (*argc)--;
            argv[*argc] = NULL;
            return 1;
        }
    }
#endif /* SR_DEBUG */


#ifdef SR_SNMPv1_PACKET
    if (!strcmp(argv[1], "-v1")) {
        util_version = SR_SNMPv1_VERSION;
        if (*argc > 2) {
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
        }
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    if (!strcmp(argv[1], "-v2c")) {
        util_version = SR_SNMPv2c_VERSION;
        if (*argc > 2) {
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
        }
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    if (!strcmp(argv[1], "-v3")) {
        util_version = SR_SNMPv3_VERSION;
        if (*argc > 2) {
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
        }
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }
    if (util_version == SR_SNMPv3_VERSION) {
        if (!strcmp(argv[1], "-alt_engine_id")) {
            alt_engineID = TRUE;
            if (*argc > 2) {
                memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
            }
            (*argc)--;
            argv[*argc] = NULL;
            return 1;
        }
    }
#endif /* SR_SNMPv3_PACKET */



#if defined(SR_SNMPv3_PACKET)
    switch (util_version) {
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
#endif /* SR_SNMPv3_PACKET */
            if (!strcmp(argv[1], "-ctxid") || !strcmp(argv[1], "-contextid")) {
                if (*argc < 3) {
                    printf("bad context options\n");
                    exit(-1);
                }
                for (i=0; i < (int) strlen(argv[2]); i++) {
                    if (argv[2][i] == ':') {
                        argv[2][i] = ' ';
                    }
                }
                defaultContextSnmpID = MakeOctetStringFromHex(argv[2]);
                if (*argc > 3) {
                    memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
                }
                (*argc)--;
                argv[*argc] = NULL;
                (*argc)--;
                argv[*argc] = NULL;
                return 1;
            }
            if (!strcmp(argv[1], "-ctx") || !strcmp(argv[1], "-context")) {
                if (*argc < 3) {
                    printf("bad context options\n");
                    exit(-1);
                }
                FreeOctetString(defaultContext);
                defaultContext = MakeOctetStringFromText(argv[2]);
                if (*argc > 3) {
                    memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
                }
                (*argc)--;
                argv[*argc] = NULL;
                (*argc)--;
                argv[*argc] = NULL;
                return 1;
            }
    }
#endif	/* defined(SR_SNMPv3_PACKET) */


    if (!strcmp(argv[1], "-format") || !strcmp(argv[1], "-fmt")) {
        if (*argc < 3) {
            printf("bad format option\n");
            exit(-1);
        }
        set_output_format(argv[2]); 
        if (*argc > 3) {
            memcpy(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
        }
        (*argc)--;
        argv[*argc] = NULL;
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }


    if (!strcmp(argv[1], "-t") || !strcmp(argv[1], "-timeout")) {
        if (*argc < 3) {
            printf("bad timeout option\n");
            exit(-1);
        }
        seconds = atoi(argv[2]);
        if (*argc > 3) {
            memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
        }
        (*argc)--;
        argv[*argc] = NULL;
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }


    if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "-retries")) {
        if (*argc < 3) {
            printf("bad retries option\n");
            exit(-1);
        }
        retries = atoi(argv[2]) + 1;
        tretries = atoi(argv[2]) + 1;
        if (*argc > 3) {
            memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
        }
        (*argc)--;
        argv[*argc] = NULL;
        (*argc)--;
        argv[*argc] = NULL;
        return 1;
    }


   if (!strcmp(argv[1], "-pkt_size"))  {
      if( (*argc < 2) || (!isdigit((unsigned char)*argv[2])) )  {
          printf("bad pkt_size option\n");
          exit(-1);
      }

      sr_pkt_size = atoi(argv[2]);
        if (sr_pkt_size < 484)  {
          DPRINTF((APWARN, "max packet size minimum value is 484 bytes. \n"));
          DPRINTF((APWARN, "setting max packet size to 484. \n"));
           sr_pkt_size = 484;
        }
        if (sr_pkt_size > 2147483647)  {
          DPRINTF((APWARN, "max packet size maximum value is 2147483647 bytes. \n"));
          DPRINTF((APWARN, "setting max packet size to 2147483647. \n"));
           sr_pkt_size = 2147483647;
        }

      if (*argc > 3) {
          memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
      }
      (*argc)--;
      argv[*argc] = NULL;
      (*argc)--;
      argv[*argc] = NULL;   
      return 1;
   }			/* end of -pkt_size option  */

    if (!strcmp(argv[1], "-pkt_stats"))  {
         print_packet_stats = 1;
         if (*argc > 2) {
             memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
         }
         (*argc)--;
         argv[*argc] = NULL;
         return 1;
    }

    return 0;
}

#ifdef SR_DEBUG
LogBook *LogBk;

LogBook *
GetLogBook()
{
    return LogBk;
}
#endif /* SR_DEBUG */



/* ----------------------------------------------------------------------
 * InitializeUtility
 *
 */
int
InitializeUtility(type, argc, argv,
                  default_util_dest, default_util_handle, usage_string)
    int type;
    int *argc;
    char *argv[];
    char *default_util_dest;
    char *default_util_handle;
    char *usage_string;
{
#ifdef SR_SNMPv3_PACKET
    FNAME("InitializeUtility")
#endif /* SR_SNMPv3_PACKET */
    char *util_version_env;
    char *format;
#ifdef SR_SNMPv3_PACKET
    int  couldntReadMgrcnf = 0;
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_DEBUG
    if ((LogBk = OpenLog()) == NULL) {
	fprintf(stderr, "warning: OpenLog() failed\n");
	exit(-1);
    }
#endif /* SR_DEBUG */
    snmp_lcd.num_vers = 0;

#ifdef SR_SNMPv1_PACKET
    snmp_lcd.versions_supported[snmp_lcd.num_vers] = SR_SNMPv1_VERSION;
    snmp_lcd.lcds[snmp_lcd.num_vers] = NULL;
    snmp_lcd.parse_functions[snmp_lcd.num_vers] = SrParseV1SnmpMessage;
    snmp_lcd.build_functions[snmp_lcd.num_vers] = SrBuildV1SnmpMessage;
    snmp_lcd.num_vers++;
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    snmp_lcd.versions_supported[snmp_lcd.num_vers] = SR_SNMPv2c_VERSION;
    snmp_lcd.lcds[snmp_lcd.num_vers] = NULL;
    snmp_lcd.parse_functions[snmp_lcd.num_vers] = SrParseV1SnmpMessage;
    snmp_lcd.build_functions[snmp_lcd.num_vers] = SrBuildV1SnmpMessage;
    snmp_lcd.num_vers++;
#endif /* SR_SNMPv2c_PACKET */

    communication_type = type;
    util_name = argv[0];
    util_version = -1;
    seconds = SECS;
    retries = RETRIES;
    tretries = RETRIES;
#if defined(SR_SNMPv3_PACKET)
    defaultContextSnmpID = NULL;
    defaultContext = NULL;
#endif	/* defined(SR_SNMPv3_PACKET) */


#ifdef SR_DEBUG
    SetProgname(argv[0]);
    SetLogLevel(APWARN | APERROR);
#endif                          /* SR_DEBUG */



        util_version_env = getenv("SR_UTIL_SNMP_VERSION");
        if (util_version_env != NULL) {
#ifdef SR_SNMPv1_PACKET
            if (!strcmp(util_version_env, "-v1")) {
                util_version = SR_SNMPv1_VERSION;
            }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
            if (!strcmp(util_version_env, "-v2c")) {
                util_version = SR_SNMPv2c_VERSION;
            }
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
            if (!strcmp(util_version_env, "-v3")) {
                util_version = SR_SNMPv3_VERSION;
            }
#endif /* SR_SNMPv3_PACKET */
        }

    format = getenv("SR_UTIL_OUTPUT_FORMAT");
    if (format) {
        set_output_format(format);
    }

    while (process_command_line_option(argc, argv));

    if (communication_type == TRAP_RCV_COMMUNICATIONS) {
#ifdef SR_SNMPv1_PACKET
        util_version = SR_SNMPv1_VERSION;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        util_version = SR_SNMPv2c_VERSION;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        util_version = SR_SNMPv3_VERSION;
#endif /* SR_SNMPv3_PACKET */
    }

    if (util_version == -1) {
        fprintf(stderr, usage_string, argv[0], argv[0]);
        goto failure;
    }


    util_handle = NULL;
#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        util_handle = getenv("SR_UTIL_COMMUNITY");
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
    if (util_version == SR_SNMPv2c_VERSION) {
        util_handle = getenv("SR_UTIL_COMMUNITY");
    }
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
    if (util_version == SR_SNMPv3_VERSION) {
        util_handle = getenv("SR_UTIL_USERNAME");
    }
#endif /* SR_SNMPv3_PACKET */

    if (util_handle == NULL) {
        if (*argc == 1) {
            util_dest = default_util_dest;
            util_handle = default_util_handle;
        } else if (*argc == 2) {
            util_dest = argv[1];
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
            (*argc)--;
            argv[*argc] = NULL;
            util_handle = default_util_handle;
        } else {
            util_dest = argv[1];
            util_handle = argv[2];
            memmove(argv + 1, argv + 3, sizeof(char *) * ((*argc) - 3));
            (*argc)--;
            argv[*argc] = NULL;
            (*argc)--;
            argv[*argc] = NULL;
        }
    } else {
        if (*argc == 1) {
            util_dest = default_util_dest;
        } else {
            util_dest = argv[1];
            memmove(argv + 1, argv + 2, sizeof(char *) * ((*argc) - 2));
            (*argc)--;
            argv[*argc] = NULL;
        }
    }

    if (util_handle == NULL) {
        fprintf(stderr, usage_string, argv[0], argv[0]);
        goto failure;
    }

    InitUtilFileNames();

#ifdef SR_MIB_TREE_H
    if (NewMIBFromFile(fn_snmpinfo_dat) < 0) {
        printf("Failure in snmpinfo.dat\n");
        goto failure;
    }
#endif /* SR_MIB_TREE_H */

#ifdef SR_MIB_TABL_H
    mib_oid_table = orig_mib_oid_table;
#endif /* SR_MIB_TABL_H */
    switch (communication_type) {
    case SNMP_COMMUNICATIONS:
        if (util_dest == NULL) {
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
            InitializeIO(util_name, util_dest);
        break;

    case TRAP_SEND_COMMUNICATIONS:
        if (util_dest == NULL) {
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
        InitializeTrapIO(util_name, util_dest);
        break;

    case TRAP_RCV_COMMUNICATIONS: 
        if((InitializeTrapRcvIO(util_name)) < 0){
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
        break;

    case INFORM_COMMUNICATIONS:
        if (util_dest == NULL) {
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
        InitializeInformIO(util_name, util_dest);
        break;

    default:
        fprintf(stderr, usage_string, argv[0], argv[0]);
        goto failure;
    }

#if defined(SR_SNMPv3_PACKET)
#ifdef SR_CONFIG_FP
    /* configuration file will be opened twice 
     * get snmpEngineID from a configuration file 
     * SR_ENGINE_ID_SRC : 1  SIMPLE_ALG (algorithm #0)
     *                    2  COMPLEX_ALG (algorithm #1)
     *                    3  MANUAL (manually configured)
     * if
     *  SR_ENGINE_ID_SRC == 1 ->generate snmpEngineID with algorithm#1
     *  SR_ENGINE_ID_SRC == 2 ->generate snmpEngineID with algorithm#2
     *  SR_ENGINE_ID_SRC == 3 ->use manually configured snmpEngineID
     *                          in mgr.cnf
     */

    engineID.runtime_SNMP_ENGINE_ID_SRC = DEFAULT_SNMP_ENGINE_ID_SRC;
    engineID.runtime_SNMP_ENGINE_ID = NULL;

    engineID_rt_count = 0;
    engineID_rt[engineID_rt_count++] = &TMP_ENGINE_ID_Record;
    engineID_rt[engineID_rt_count++] = &TMP_ENGINE_ID_SRC_Record;
    engineID_rt[engineID_rt_count++] = NULL;
    couldntReadMgrcnf = ParseScalerinConfigFile(fn_mgr_file, engineID_rt);
    if (couldntReadMgrcnf) {
        switch (engineID.runtime_SNMP_ENGINE_ID_SRC) {
            case MANUAL:
                if (engineID.runtime_SNMP_ENGINE_ID != NULL) {
                    DPRINTF((APTRACE,
	        	     "%: use manualy assigned snmpEngineID\n", Fname));
                    SrAssignSnmpID(engineID.runtime_SNMP_ENGINE_ID);
                    FreeOctetString(engineID.runtime_SNMP_ENGINE_ID);
                }
                break;
            case COMPLEX_ALG:
                DPRINTF((APWARN,
	            "%s: snmpEngineID algorithm#2 is not supported.\n", Fname));
            case SIMPLE_ALG:
            default:
                break;
        }
    }
#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */

#if defined(SR_SNMPv3_PACKET)
    InitTimeNow();
    util_local_snmpID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);  /* LVL7*/
    if (util_local_snmpID == NULL) {
        printf("Cannot initialize local snmpID.\n");
        goto failure;
    }
#endif	/* defined(SR_SNMPv3_PACKET) */

#if defined(SR_SNMPv3_PACKET)
#ifdef SR_CONFIG_FP
    util_rt[util_rt_count++] = &SNMP_ENGINE_ID_Record;
    util_rt[util_rt_count++] = &SNMP_ENGINE_ID_SRC_Record;
    engineID_1.runtime_SNMP_ENGINE_ID_SRC = DEFAULT_SNMP_ENGINE_ID_SRC;
    engineID_1.runtime_SNMP_ENGINE_ID = NULL;
#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv3_PACKET
#ifndef SR_UNSECURABLE
    if ((inform_type == SEND_INFORM) ||
               ((alt_engineID) && (inform_type))) {
	util_local_snmpID->octet_ptr[6] = 0;
	util_local_snmpID->octet_ptr[7] = (unsigned char)port_num;
    }
#endif /* SR_UNSECURABLE */
    InitV3CLU(util_rt, &util_rt_count);
#endif /* SR_SNMPv3_PACKET */

#if defined(SR_SNMPv3_PACKET)
#ifdef SR_CONFIG_FP
    util_rt[util_rt_count++] = NULL;
    ConvToken_snmpID_localSnmpID = util_local_snmpID;
    if (!couldntReadMgrcnf) {
        ParseConfigFile(fn_mgr_file, util_rt);
    }
    updateEngineID(&engineID_1);
#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv3_PACKET
    if (util_version == SR_SNMPv3_VERSION) {
        if (!InitializeV3User(util_handle, communication_type)) {
            printf("Bad username %s\n", util_handle);
            goto failure;
        }
    }
#endif /* SR_SNMPv3_PACKET */

    global_req_id = MakeReqId();

#ifndef SR_NO_PRIVACY
#if (defined(SR_AES) && defined(SR_SNMPv3_PACKET))
    AesSetup();
#endif	/* (defined(SR_AES) && defined(SR_SNMPv3_PACKET)) */
#endif /* SR_NO_PRIVACY */

    return 0;

  failure:
    return -1;
}

/*
 * PerformCommunityRequest:
 *
 */
Pdu *
PerformCommunityRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    OctetString *community_ptr = NULL;
    SnmpMessage *snmp_msg = NULL;
    Pdu *in_pdu_ptr = NULL;

    *status = 0;

    community_ptr = MakeOctetStringFromText(util_handle);
    if (community_ptr == NULL) {
        DPRINTF((APWARN, "MakeOctetStringFromText failed.\n"));
        *status = SR_ERROR;
        goto done;
    }

#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        snmp_msg = SrCreateV1SnmpMessage(community_ptr, FALSE);
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
    if (util_version == SR_SNMPv2c_VERSION) {
        snmp_msg = SrCreateV2cSnmpMessage(community_ptr, FALSE);
    }
#endif /* SR_SNMPv2c_PACKET */
    community_ptr = NULL;
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "cannot create SnmpMessage.\n"));
        *status = SR_ERROR;
        goto done;
    }

    /* make final request packet */
    if (SrBuildSnmpMessage(snmp_msg, out_pdu_ptr, &snmp_lcd) != 0) {
        DPRINTF((APWARN, "SrBuildSnmpMessage failed.\n"));
        *status = SR_ERROR;
        goto done;
    }

    if (SrSendRequest(fd,
                      snmp_msg->packlet->octet_ptr,
                      snmp_msg->packlet->length) != TRUE) {
        /* could not send */
        *status = SR_ERROR;
        goto done;
    }

    SrFreeSnmpMessage(snmp_msg);
    snmp_msg = NULL;

#ifdef SR_SNMPv1_PACKET
    if (out_pdu_ptr->type == TRAP_TYPE) {
        goto done;
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
    if (out_pdu_ptr->type == SNMPv2_TRAP_TYPE) {
        goto done;
    }
#endif /* SR_SNMPv2_PDU */

  wait_again:
    *status = GetResponse();

    /* exit if error in receive routine */
    if (*status == SR_ERROR) {
        DPRINTF((APALWAYS, "%s:  receive error.\n", util_name));
        goto done;
    }

    if (*status == SR_TIMEOUT) {
        DPRINTF((APTRACE, "%s:  no response.\n", util_name));

        goto done;
    }

    snmp_msg = SrParseSnmpMessage(&snmp_lcd, NULL, NULL, packet, (int)packet_len);
    if (snmp_msg == NULL) {
        DPRINTF((APALWAYS, "%s: error parsing packet.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        DPRINTF((APALWAYS, "%s: error parsing pdu.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    /* check pdu type */    
    if (in_pdu_ptr->type != GET_RESPONSE_TYPE) {
        DPRINTF((APALWAYS, "%s: error parsing PDU, received unexpected PDU type %d\n", util_name, in_pdu_ptr->type));
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        *status = SR_ERROR;
        goto done;  
    }

    /* Make sure request id matches */
    if (in_pdu_ptr->u.normpdu.request_id != out_pdu_ptr->u.normpdu.request_id) {
        DPRINTF((APALWAYS, "request id mismatch.\n"));
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        goto wait_again;
    }

  done:
    if (community_ptr != NULL) {
        FreeOctetString(community_ptr);
        community_ptr = NULL;
    }
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    return in_pdu_ptr;
}


/*
 * PerformRequest:
 *
 */
Pdu *
PerformRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    switch (util_version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            return PerformCommunityRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            return PerformCommunityRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            return PerformV3UserRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv3_PACKET */
    }
    return NULL;
}

#if defined(SR_SNMPv3_PACKET)
#ifndef SR_UNSECURABLE
int
get_password(prompt, pw, maxlen)
    char *prompt;
    char *pw;
    int maxlen;
{
    int pos = 0, ch;

    printf("%s", prompt);

    while (pos < maxlen) {
        ch = getchar();
        if (ch != '\n') {
            pw[pos++] = ch;
        } else {
            break;
        }
    }
    pw[pos] = '\0';
    return pos;
}
#endif /* SR_UNSECURABLE */
#endif	/* defined(SR_SNMPv3_PACKET) */

#if defined(SR_SNMPv3_PACKET)
#ifdef SR_CONFIG_FP
/* 
 * updateEngineID
 *
 * this routine updates the runtime_snmpEngineID stored in the 
 * ENGINE_ID structure with util_local_snmpID.
 *
 * return value : void
 */

void
updateEngineID(ENGINE_ID *id)
{
    int status;

    if (engineID.runtime_SNMP_ENGINE_ID_SRC == 1) {
       if (id->runtime_SNMP_ENGINE_ID != NULL) {
           status = CmpOctetStrings(id->runtime_SNMP_ENGINE_ID,
                                        util_local_snmpID);
           if (status != 0) {
               FreeOctetString(id->runtime_SNMP_ENGINE_ID);
               id->runtime_SNMP_ENGINE_ID =
               CloneOctetString(util_local_snmpID);
           }
        }
        else {
            id->runtime_SNMP_ENGINE_ID =
            CloneOctetString(util_local_snmpID);
        }
    }
}
#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */

/* 
 * set_output_format:  loop through formatting options. Note that partial
 *		       format-option names may be specified as long as the 
 *                     name specified is long enough to uniquely identify
 *                     the complete desired format name.
 *
 *                     *** When adding new formatting options to the list 
 *                     below, use the '[' character to specify the portion of
 *                     the format name that is optional. When doing this, make
 *                     sure that the required portion of the new pattern does 
 *                     not cause any of the previously defined partial format
 *                     options to become non-unique.  Otherwise, the string
 *                     comparison of the previous format options must modified  
 *                     to specify a longer string for the required portion of
 *                     the pattern name.
 */
static void
set_output_format(char *format)
{
    char *item;

    if (format == NULL) {
        return;
    }
    item = strtok(format, ",");
    while (item) {
        if (kw_match(item, "nu[mber")) {
            PrintVarBind_format |= SR_HF_NUMBER_FORM;
        } else if (kw_match(item, "na[me")) {
            PrintVarBind_format &= ~SR_HF_NUMBER_FORM;


        } else if (kw_match(item, "indexna[me")) {
            PrintVarBind_format |= SR_HF_INDEX_NAME_FORM;
            PrintVarBind_format |= SR_HF_BRACKETS;
        } else if (kw_match(item, "indexnu[mber")) {
            PrintVarBind_format &= ~SR_HF_INDEX_NAME_FORM;
            PrintVarBind_format &= ~SR_HF_BRACKETS;

                
        } else {
            printf("invalid format option: %s\n", item);
        }
        item = strtok(NULL, ",");
    }
}






/*
 * PrintResponseStats
 *
 *  Print statistics of response packet:
 *         # bytes in response(s)
 *         # objects returned in response
 *         # total elements in response
 * 
 */
void
PrintResponseStats(int req_type, VarBind *vblist, int packet_count)
{
   VarBind *vb_ptr;
   int num_objs = 0;

   vb_ptr = vblist;
   while(vb_ptr != NULL) {
       num_objs++;
       vb_ptr = vb_ptr->next_var;
   }
   printf("\n\nResponse Statistics:\n");
   if (packet_count > 0) {
       printf("    Number of requests performed:  %d\n", packet_count);
   }
   printf("    VarBind data received:  %d bytes\n", FindLenVarBind(vblist));
   switch(req_type) {
      case GET_REQUEST_TYPE:
          printf("    Total number of elements in VarBind list: %d\n", 
                                               ElementsInVarBindList(vblist));
          break;
      default:
          printf("    Total number of elements in VarBind list: %d\n", 
                                          NonNullElementsInVarBindList(vblist));
          break;
   }
   printf("\n");
}


/* ElementsInVarBindList:  
 * 
 *     Returns the number of objects in the varbind list.
 */
int
ElementsInVarBindList(VarBind *vblist)
{
   VarBind *vb_ptr;
   int num_elements = 0;
 
   vb_ptr = vblist;
 
   while(vb_ptr != NULL) {
        num_elements++;
     vb_ptr = vb_ptr->next_var;
   }
   return num_elements;
}






/* NonNullElementsInVarBindList:  
 * 
 *     Returns the number of non-NULL objects in the varbind list.
 */
int
NonNullElementsInVarBindList(VarBind *vblist)
{
   VarBind *vb_ptr;
   int num_elements = 0;
 

   vb_ptr = vblist;
 
   while(vb_ptr != NULL) {
        if ((vb_ptr->value.type != NULL_TYPE) && 
            (vb_ptr->value.type != END_OF_MIB_VIEW_EXCEPTION)) {
            num_elements++;
        }
     vb_ptr = vb_ptr->next_var;
   }
   return num_elements;
}




