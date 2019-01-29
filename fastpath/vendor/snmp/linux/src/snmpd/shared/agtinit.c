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

#include <string.h>


#include <ctype.h>

#include <errno.h>

#include <stdlib.h>

#include <malloc.h>

#include <unistd.h>

#include <sys/types.h>

#include <netdb.h>


#include <sys/socket.h>

#include <sys/stat.h>

#include <netinet/in.h>

#include <stddef.h>


#include <sys/utsname.h>


#include "sr_snmp.h"
#include "sr_trans.h"
#include "snmpid.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "comunity.h"
#include "snmpv2.h"
#include "snmptype.h"
#include "sr_msg.h"
#include "v1_msg.h"
#ifdef SR_SNMPv3_PACKET
#include "v3_msg.h"
#endif /* SR_SNMPv3_PACKET */
#include "context.h"
#include "mapctx.h"
#include "snmpd.h"
#include "sitedefs.h"
#include "sr_bdata.h"
#include "sri/gettime.h"

#ifdef SR_SNMPv3_ADMIN
#include "v3_vta.h"
#endif /* SR_SNMPv3_ADMIN */

#include "agtinit.h"
#include "fnames.h"


#include "diag.h"

/* for S_IWGRP and friends on non-posix compliant systems */
#include "compat.h"


SR_FILENAME

#ifdef SR_SNMP_ADMIN_MIB
#ifdef SR_SNMPv3_ADMIN
#define SR_SNMPv3_ADMIN_MIB
#endif /* SR_SNMPv3_ADMIN */
#endif /* SR_SNMP_ADMIN_MIB */

#if (defined(SR_SNMPv3_PACKET_MIB) || defined(SR_SNMPv3_ADMIN_MIB))
#include "snmpv3/v3init.h"
#endif	/* (defined(SR_SNMPv3_PACKET_MIB) || defined(SR_SNMPv3_ADMIN_MIB)) */


#ifdef SR_SNMPv3_PACKET
SnmpV3Lcd agt_snmp_v3_lcd;
#endif /* SR_SNMPv3_PACKET */

#ifndef SR_NO_PRIVACY
#if (defined(SR_SNMPv3_PACKET) && defined(SR_AES))
#include "aes.h"
#include "sr_aes.h"
#endif	/* (defined(SR_SNMPv3_PACKET) && defined(SR_AES)) */
#endif /* SR_NO_PRIVACY */
extern int k_system_initialize(void);
extern int k_apoCap_initialize(void);

extern int k_srAgentInfo_initialize(void);

extern int k_or_initialize(void);


#ifdef SR_TERMINATE_CODE
extern int k_srAgentInfo_terminate(void);

#ifdef SR_SNMPv1_ADMIN
#else

#ifdef SR_SNMPv3_PACKET
#ifdef SR_SNMPv3_PACKET_MIB
extern int k_arch_terminate(void);
extern int k_mpd_terminate(void);

#ifdef SR_USER_SEC_MODEL
extern int k_usm_terminate(void);
#endif /* SR_USER_SEC_MODEL */

#endif /* SR_SNMPv3_PACKET_MIB */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
#ifdef SR_SNMP_ADMIN_MIB
extern int k_arch_terminate(void);
extern int k_ac_terminate(void);
extern int k_ctx_terminate(void);
extern int k_group_terminate(void);
extern int k_view_terminate(void);
extern int k_tgt_terminate(void);
extern int k_notif_terminate(void);
#endif /* SR_SNMP_ADMIN_MIB */
#endif /* SR_SNMPv3_ADMIN */

#ifdef SR_SNMPv3_PACKET
extern int i_arch_terminate(void);
extern int i_mpd_terminate(void);
#ifdef SR_USER_SEC_MODEL
extern int i_usm_terminate(SnmpV2Table *uut);
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
extern int i_arch_terminate(void);
extern int i_ac_terminate(SnmpV2Table *vat);
extern int i_ctx_terminate(SnmpV2Table *vct);
extern int i_group_terminate(SnmpV2Table *vstgt);
extern int i_view_terminate(SnmpV2Table *vvtft);
extern int i_tgt_terminate(SnmpV2Table *stat, SnmpV2Table *stpt);
extern int i_notif_terminate(SnmpV2Table *snt,
                             SnmpV2Table *snfpt,
                             SnmpV2Table *snft);
#endif  /* SR_SNMPv3_ADMIN */

#endif  /* !SR_SNMPv1_ADMIN */

#endif  /* SR_TERMINATE_CODE */


#ifdef SR_SNMPv2_PDU
extern snmpSet_t snmpSetData;
#endif /* SR_SNMPv2_PDU */

#ifndef SR_SNMPv1_ADMIN

#ifdef SR_SNMPv1_WRAPPER

#ifdef SR_SNMPv3_ADMIN

#include "coex/i_comm.h"
SnmpV2Table agt_snmpCommunityTable;

#ifdef SR_SNMPv1_PACKET_MIB
/*
 * This should really be in an include file coex/coexinit.h or some
 * such, but it would _only_ include this, so I'm just putting it
 * directly in this file instead.  The function is in k_comm.c
 */
int k_snmpCommunityEntry_initialize(
    char *contextName_text,
    SnmpV2Table *i_snmpCommunityTable);
#endif	/* SR_SNMPv1_PACKET_MIB */

#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE agt_snmpCommunityEntryRecord = {
    PARSER_TABLE,
    "snmpCommunityEntry ",
    snmpCommunityEntryConverters,
    snmpCommunityEntryTypeTable,
    &agt_snmpCommunityTable,
    NULL,
    offsetof(snmpCommunityEntry_t, snmpCommunityStatus),
    offsetof(snmpCommunityEntry_t, snmpCommunityStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpCommunityEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */

#endif /* SR_SNMPv3_ADMIN */

#endif	/* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv3_PACKET
#include "snmpv3/i_arch.h"
#include "snmpv3/i_mpd.h"
#ifdef SR_USER_SEC_MODEL
#include "snmpv3/i_usm.h"
#endif /* SR_USER_SEC_MODEL */
snmpEngine_t agt_snmpEngineData;
snmpMPDStats_t agt_snmpMPDStatsData;
#ifdef SR_USER_SEC_MODEL
usmStats_t agt_usmStatsData;
usmUser_t agt_usmUserData;
SnmpV2Table agt_usmUserTable;
#endif /* SR_USER_SEC_MODEL */
#ifdef SR_SNMPv3_PACKET_MIB
#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE agt_snmpEngineBootsRecord = {
    PARSER_SCALAR,
    "snmpEngineBoots ",
    snmpEngineBootsConverters,
    snmpEngineBootsType,
    NULL,
    &agt_snmpEngineData.snmpEngineBoots,
    -1,
    -1,
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#ifdef SR_USER_SEC_MODEL
const PARSER_RECORD_TYPE agt_usmUserEntryRecord = {
    PARSER_TABLE,
    "usmUserEntry ",
    usmUserEntryConverters,
    usmUserEntryTypeTable,
    &agt_usmUserTable,
    NULL,
    offsetof(usmUserEntry_t, usmUserStatus),
    offsetof(usmUserEntry_t, usmUserStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)usmUserEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv3_PACKET_MIB */
#endif /* SR_SNMPv3_PACKET */


const PARSER_RECORD_TYPE *engineID_rt[3];
int engineID_rt_count;

/* snmpEngineID related code */
#ifdef SR_SNMPv3_PACKET
#define SIMPLE_ALG 1
#define COMPLEX_ALG 2
#define MANUAL 3

ENGINE_ID engineID;


/*
 * store snmpEngineID into following constant array 
 * by scanning configuration file
 */
static const SnmpType TMP_ENGINE_ID_Type[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
static const SnmpType TMP_ENGINE_ID_SRC_Type[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

/* If k_hardio.c is used don't use following constant */
static const SnmpType SNMP_ENGINE_ID_Type[] = {
    { OCTET_PRIM_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
static const SnmpType SNMP_ENGINE_ID_SRC_Type[] = {
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

extern ENGINE_ID engine_id;

#ifndef SR_NO_COMMENTS_IN_CONFIG
const char snmpEngineIDFormatString[] =
    "# Entry type: snmpEngineID\n"
    "# Format: octetString\n"
    "#   snmpEngineID can be either\n"
    "#      1. automatically generated at agents' start up\n"
    "#   or 2. configured manually\n"
    "#   Note that, if the value of snmpEngineID is changed after\n"
    "#   the plain text passwords are converted to localized keys, then\n"
    "#   the SNMPv3 users will not be able to access the agents using\n"
    "#   their passwords.";

const char snmpEngineIDsrcFormatString[] =
    "# Entry type: SNMP_ENGINE_ID_SRC\n"
    "# Format : integer\n"
    "#              1 (SIMPLE_ALG, algorithm #0)\n"
    "#              2 (COMPLEX_ALG, algorithm #1)\n"
    "#              3 (MANUAL, manually configured)";
#endif  /* SR_NO_COMMENTS_IN_CONFIG */

static const PARSER_CONVERTER SNMP_ENGINE_ID_Converters[] = {
    { ConvToken_textOctetString, NULL },
    { NULL, NULL }
};

static const PARSER_CONVERTER SNMP_ENGINE_ID_SRC_Converters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};

static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_Record = {
    PARSER_SCALAR, "snmpEngineID ",
    SNMP_ENGINE_ID_Converters, SNMP_ENGINE_ID_Type,
    NULL, &engine_id.runtime_SNMP_ENGINE_ID, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *) snmpEngineIDFormatString
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
};

static const PARSER_RECORD_TYPE SNMP_ENGINE_ID_SRC_Record = {
    PARSER_SCALAR, "SNMP_ENGINE_ID_SRC ",
    SNMP_ENGINE_ID_SRC_Converters, SNMP_ENGINE_ID_SRC_Type,
    NULL, &engine_id.runtime_SNMP_ENGINE_ID_SRC, -1, -1, -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *) snmpEngineIDsrcFormatString
#endif  /* SR_NO_COMMENTS_IN_CONFIG */
};


#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN

#include "snmpv3/i_ac.h"
#include "snmpv3/i_ctx.h"
#include "snmpv3/i_group.h"
#include "snmpv3/i_notif.h"
#include "snmpv3/i_tgt.h"
#include "snmpv3/i_view.h"

SnmpV2Table agt_vacmAccessTable;
SnmpV2Table agt_vacmContextTable;
SnmpV2Table agt_vacmSecurityToGroupTable;
SnmpV2Table agt_snmpNotifyTable;
SnmpV2Table agt_snmpNotifyFilterProfileTable;
SnmpV2Table agt_snmpNotifyFilterTable;
snmpTargetObjects_t agt_snmpTargetObjectsData;
SnmpV2Table agt_snmpTargetAddrTable;
SnmpV2Table agt_snmpTargetParamsTable;
vacmMIBViews_t agt_vacmMIBViewsData;
SnmpV2Table agt_vacmViewTreeFamilyTable;

#ifdef SR_SNMP_ADMIN_MIB
#ifdef SR_CONFIG_FP
const PARSER_RECORD_TYPE agt_vacmAccessEntryRecord = {
    PARSER_TABLE | PARSER_FORCE_INDEX_ORDER,
    "vacmAccessEntry ",
    vacmAccessEntryConverters,
    vacmAccessEntryTypeTable,
    &agt_vacmAccessTable,
    NULL,
    offsetof(vacmAccessEntry_t, vacmAccessStatus),
    offsetof(vacmAccessEntry_t, vacmAccessStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)vacmAccessEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};

const PARSER_RECORD_TYPE agt_vacmSecurityToGroupEntryRecord = {
    PARSER_TABLE,
    "vacmSecurityToGroupEntry ",
    vacmSecurityToGroupEntryConverters,
    vacmSecurityToGroupEntryTypeTable,
    &agt_vacmSecurityToGroupTable,
    NULL,
    offsetof(vacmSecurityToGroupEntry_t, vacmSecurityToGroupStatus),
    offsetof(vacmSecurityToGroupEntry_t, vacmSecurityToGroupStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)vacmSecurityToGroupEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};

const PARSER_RECORD_TYPE agt_snmpNotifyEntryRecord = {
    PARSER_TABLE,
    "snmpNotifyEntry ",
    snmpNotifyEntryConverters,
    snmpNotifyEntryTypeTable,
    &agt_snmpNotifyTable,
    NULL,
    offsetof(snmpNotifyEntry_t, snmpNotifyRowStatus),
    offsetof(snmpNotifyEntry_t, snmpNotifyStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpNotifyEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE agt_snmpNotifyFilterProfileEntryRecord = {
    PARSER_TABLE | PARSER_FORCE_INDEX_ORDER,
    "snmpNotifyFilterProfileEntry ",
    snmpNotifyFilterProfileEntryConverters,
    snmpNotifyFilterProfileEntryTypeTable,
    &agt_snmpNotifyFilterProfileTable,
    NULL,
    offsetof(snmpNotifyFilterProfileEntry_t,snmpNotifyFilterProfileRowStatus),
    offsetof(snmpNotifyFilterProfileEntry_t,snmpNotifyFilterProfileStorType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpNotifyFilterProfileEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE agt_snmpNotifyFilterEntryRecord = {
    PARSER_TABLE | PARSER_FORCE_INDEX_ORDER,
    "snmpNotifyFilterEntry ",
    snmpNotifyFilterEntryConverters,
    snmpNotifyFilterEntryTypeTable,
    &agt_snmpNotifyFilterTable,
    NULL,
    offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterRowStatus),
    offsetof(snmpNotifyFilterEntry_t, snmpNotifyFilterStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpNotifyFilterEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};


const PARSER_RECORD_TYPE agt_snmpTargetAddrEntryRecord = {
    PARSER_TABLE,
    "snmpTargetAddrEntry ",
    snmpTargetAddrEntryConverters,
    snmpTargetAddrEntryTypeTable,
    &agt_snmpTargetAddrTable,
    NULL,
    offsetof(snmpTargetAddrEntry_t, snmpTargetAddrRowStatus),
    offsetof(snmpTargetAddrEntry_t, snmpTargetAddrStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpTargetAddrEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
const PARSER_RECORD_TYPE agt_snmpTargetParamsEntryRecord = {
    PARSER_TABLE,
    "snmpTargetParamsEntry ",
    snmpTargetParamsEntryConverters,
    snmpTargetParamsEntryTypeTable,
    &agt_snmpTargetParamsTable,
    NULL,
    offsetof(snmpTargetParamsEntry_t, snmpTargetParamsRowStatus),
    offsetof(snmpTargetParamsEntry_t, snmpTargetParamsStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)snmpTargetParamsEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};

const PARSER_RECORD_TYPE agt_vacmViewTreeFamilyEntryRecord = {
    PARSER_TABLE,
    "vacmViewTreeFamilyEntry ",
    vacmViewTreeFamilyEntryConverters,
    vacmViewTreeFamilyEntryTypeTable,
    &agt_vacmViewTreeFamilyTable,
    NULL,
    offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyStatus),
    offsetof(vacmViewTreeFamilyEntry_t, vacmViewTreeFamilyStorageType),
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *)vacmViewTreeFamilyEntryRecordFormatString
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};

const PARSER_RECORD_TYPE agt_vacmContextEntryRecord = {
    PARSER_TABLE,
    "vacmContextEntry ",
    vacmContextEntryConverters,
    vacmContextEntryTypeTable,
    &agt_vacmContextTable,
    NULL,
    -1,
    -1,
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , (char *) vacmContextEntryRecordFormatString
#endif /* SR_NO_COMMENTS_IN_CONFIG */
};

#endif /* SR_CONFIG_FP */
#endif /* SR_SNMP_ADMIN_MIB */

#endif /* SR_SNMPv3_ADMIN */

#endif /* !SR_SNMPv1_ADMIN */


static void init_globals SR_PROTOTYPE((void));

#ifdef EXTEND_INIT
extern int EXTEND_INIT
    SR_PROTOTYPE((void));
#endif /* EXTEND_INIT */

#ifdef EXTEND_TERM
extern int EXTEND_TERM
    SR_PROTOTYPE((void));
#endif /* EXTEND_TERM */

#if defined(SR_SNMPv3_ADMIN)
OctetString *agt_local_snmpID = NULL;  /* LVL7 */
#endif	/* defined(SR_SNMPv3_ADMIN) */

extern BOOT_DATA *bootData;
void UpdateBoot(BOOT_DATA *b_data);

static void
init_globals()
{
    char            hostname[BUFLEN];
    struct hostent *hp;


    /* Initialize snmpEnableAuthenTraps */

#ifdef SR_CLEAR_GLOBALS
    memset(&snmpData, 0, sizeof(snmpData));
#endif	/* SR_CLEAR_GLOBALS */
    snmpData.snmpEnableAuthenTraps = SNMP_TRAPS_ENABLED;

#ifdef SR_SNMPv2_PDU
    snmpSetData.snmpSetSerialNo = ((SR_INT32)0);
#endif /* SR_SNMPv2_PDU */

    if (gethostname(hostname, sizeof(hostname)) >= 0) {
        hp = gethostbyname(hostname);
        if (hp != NULL) {
            if (systemData.sysName != NULL) {
                FreeOctetString(systemData.sysName);
            }
            systemData.sysName =
                MakeOctetString((unsigned char *) hp->h_name,
                                strlen(hp->h_name));
        }
        else {
            if (systemData.sysName != NULL) {
                FreeOctetString(systemData.sysName);
            }
            systemData.sysName =
                MakeOctetString((unsigned char *) hostname, strlen(hostname));
        }
    }

  {
     struct utsname unamebuf;
     char *descr;

     if (uname(&unamebuf) < 0) {
         DPRINTF((APTRACE, "Can't get uname info, sysDescr not initialized\n"));
     } else {
         descr = (char *) malloc (strlen(unamebuf.sysname) + 
                                  strlen(unamebuf.release) +
                                  strlen(unamebuf.version) +
                                  strlen(unamebuf.machine) +
                                  100);
        if (descr) {

            sprintf(descr, "%s release:%s machine:%s",
                     unamebuf.sysname, unamebuf.release,
                     unamebuf.machine);

            if (systemData.sysDescr != NULL) {  
                FreeOctetString(systemData.sysDescr);
            }
            systemData.sysDescr = MakeOctetStringFromText(descr);
            free(descr);
        }
    }
  }


}

void
/* LVL7 */
snmp_initialize(
/* LVL7 end */
    SnmpLcd *snmp_lcd)
{

#ifndef SR_SNMPv1_ADMIN
#if defined(SR_SNMPv3_PACKET)
#ifdef SR_CONFIG_FP

    engineID_rt_count = 0;
#ifdef SR_SNMPv3_PACKET
    /* configuration file will be opened twice 
     * get snmpEngineID from a configuration file 
     * SR_ENGINE_ID_SRC : 1  SIMPLE_ALG (algorithm #0)
     *                    2  COMPLEX_ALG (algorithm #1)
     *                    3  MANUAL (manually configured)
     * if
     *  SR_ENGINE_ID_SRC == 1 ->generate snmpEngineID with algorithm#0
     *  SR_ENGINE_ID_SRC == 2 ->generate snmpEngineID with algorithm#1
     *  SR_ENGINE_ID_SRC == 3 ->use manually configured snmpEngineID
     *                          in snmpd.cnf
     */

    engineID.runtime_SNMP_ENGINE_ID_SRC = 1;
    engineID.runtime_SNMP_ENGINE_ID = NULL;

    engineID_rt[engineID_rt_count++] = &TMP_ENGINE_ID_Record;
    engineID_rt[engineID_rt_count++] = &TMP_ENGINE_ID_SRC_Record;
#endif /* SR_SNMPv3_PACKET */
    engineID_rt[engineID_rt_count++] = NULL;
    ParseScalerinConfigFile(fn_snmpd_file, engineID_rt);
#ifdef SR_SNMPv3_PACKET
    switch (engineID.runtime_SNMP_ENGINE_ID_SRC) {
	case MANUAL:
	     if (engineID.runtime_SNMP_ENGINE_ID != NULL) {
	         SrAssignSnmpID(engineID.runtime_SNMP_ENGINE_ID);
		 FreeOctetString(engineID.runtime_SNMP_ENGINE_ID);
             }
	     break;
	case COMPLEX_ALG:
	     DPRINTF((APERROR, "does not support COMPLEX_ALG.\n"));
	case SIMPLE_ALG:
	default:
	     break;
    }
#endif /* SR_SNMPv3_PACKET */
#endif /* SR_CONFIG_FP */
#endif	/* defined(SR_SNMPv3_PACKET) */
#endif /* !SR_SNMPv1_ADMIN */ 

#if defined(SR_SNMPv3_ADMIN)
    agt_local_snmpID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);  /* LVL7 */
    if (agt_local_snmpID == NULL) {
	DPRINTF((APERROR, "Could not initialize local snmpID.\n"));
    }
#endif	/* defined(SR_SNMPv3_ADMIN) */



    k_system_initialize();
    k_apoCap_initialize();

    k_srAgentInfo_initialize();

#ifdef SR_SNMPv1_ADMIN

    init_communities();
    init_trapCommunities();

#else /* !SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv1_WRAPPER

#ifdef SR_SNMPv3_ADMIN
    i_snmpCommunityEntry_initialize(&agt_snmpCommunityTable);
#ifdef SR_SNMPv1_PACKET_MIB
    k_snmpCommunityEntry_initialize(NULL, &agt_snmpCommunityTable);
#endif /* SR_SNMPv1_PACKET_MIB */
#endif /* SR_SNMPv3_ADMIN */

#endif /* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv3_PACKET
    i_arch_initialize(agt_local_snmpID, &agt_snmpEngineData);
    i_mpd_initialize(&agt_snmpMPDStatsData);
#ifdef SR_USER_SEC_MODEL
    i_usm_initialize(&agt_usmStatsData,
                     &agt_usmUserData,
                     &agt_usmUserTable);
#endif /* SR_USER_SEC_MODEL */
#ifdef SR_SNMPv3_PACKET_MIB
    k_arch_initialize(NULL, &agt_snmpEngineData);
    k_mpd_initialize(&agt_snmpMPDStatsData);
#ifdef SR_USER_SEC_MODEL
    k_usm_initialize(NULL,
                     &agt_usmStatsData,
                     &agt_usmUserData,
                     &agt_usmUserTable);
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_SNMPv3_PACKET_MIB */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
    i_arch_initialize(agt_local_snmpID, &agt_snmpEngineData);
    i_ac_initialize(&agt_vacmAccessTable);
    i_ctx_initialize(&agt_vacmContextTable);
    i_group_initialize(&agt_vacmSecurityToGroupTable);
    i_view_initialize(&agt_vacmMIBViewsData, &agt_vacmViewTreeFamilyTable);
    i_tgt_initialize(&agt_snmpTargetObjectsData,
                     &agt_snmpTargetAddrTable,
                     &agt_snmpTargetParamsTable);
    i_notif_initialize(&agt_snmpNotifyTable,
                       &agt_snmpNotifyFilterProfileTable,
                       &agt_snmpNotifyFilterTable);
#ifdef SR_SNMP_ADMIN_MIB
    k_arch_initialize(NULL, &agt_snmpEngineData);
    k_ac_initialize(&agt_vacmAccessTable);
    k_ctx_initialize(&agt_vacmContextTable);
    k_group_initialize(&agt_vacmSecurityToGroupTable);
    k_view_initialize(&agt_vacmMIBViewsData, &agt_vacmViewTreeFamilyTable);
    k_tgt_initialize(&agt_snmpTargetObjectsData,
                     &agt_snmpTargetAddrTable,
                     &agt_snmpTargetParamsTable);
    k_notif_initialize(&agt_snmpNotifyTable,
                       &agt_snmpNotifyFilterProfileTable,
                       &agt_snmpNotifyFilterTable);
#endif /* SR_SNMP_ADMIN_MIB */
#endif /* SR_SNMPv3_ADMIN */

#endif /* !SR_SNMPv1_ADMIN */



#ifdef EXTEND_INIT
    if(EXTEND_INIT() == -1) {
	DPRINTF((APWARN,
	"Initialization of agent extensions failed.  Continuing anyway.\n"));
    }
#endif /* EXTEND_INIT */

    init_globals();

    snmp_lcd->num_vers = 0;

#ifdef SR_SNMPv1_PACKET
    snmp_lcd->versions_supported[snmp_lcd->num_vers] = SR_SNMPv1_VERSION;
    snmp_lcd->lcds[snmp_lcd->num_vers] = NULL;
    snmp_lcd->parse_functions[snmp_lcd->num_vers] = SrParseV1SnmpMessage;
    snmp_lcd->build_functions[snmp_lcd->num_vers] = SrBuildV1SnmpMessage;
    snmp_lcd->num_vers++;
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    snmp_lcd->versions_supported[snmp_lcd->num_vers] = SR_SNMPv2c_VERSION;
    snmp_lcd->lcds[snmp_lcd->num_vers] = NULL;
    snmp_lcd->parse_functions[snmp_lcd->num_vers] = SrParseV1SnmpMessage;
    snmp_lcd->build_functions[snmp_lcd->num_vers] = SrBuildV1SnmpMessage;
    snmp_lcd->num_vers++;
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    agt_snmp_v3_lcd.snmpEngineData = &agt_snmpEngineData;
    agt_snmp_v3_lcd.snmpMPDStatsData = &agt_snmpMPDStatsData;
    agt_snmp_v3_lcd.usmStatsData = &agt_usmStatsData;
    agt_snmp_v3_lcd.usmUserData = &agt_usmUserData;
    agt_snmp_v3_lcd.usmUserTable = &agt_usmUserTable;
    agt_snmp_v3_lcd.usm_salt =
        ((SR_UINT32)((unsigned long) snmp_lcd)) * ((SR_UINT32)GetTimeNow());
    agt_snmp_v3_lcd.override_userSecurityName = NULL;
#ifndef SR_UNSECURABLE
    agt_snmp_v3_lcd.override_auth_secret = NULL;
#ifndef SR_NO_PRIVACY
    agt_snmp_v3_lcd.override_priv_secret = NULL;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    snmp_lcd->versions_supported[snmp_lcd->num_vers] = SR_SNMPv3_VERSION;
    snmp_lcd->lcds[snmp_lcd->num_vers] = &agt_snmp_v3_lcd;
    snmp_lcd->parse_functions[snmp_lcd->num_vers] = SrParseV3SnmpMessage;
    snmp_lcd->build_functions[snmp_lcd->num_vers] = SrBuildV3SnmpMessage;
    snmp_lcd->num_vers++;

#ifndef SR_NO_PRIVACY
#if defined(SR_AES)
    AesSetup();
#endif	/* defined(SR_AES) */
#endif /* SR_NO_PRIVACY */

#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv1_ADMIN
    snmp_lcd->vta_data = NULL;
    snmp_lcd->vta_function = NULL;
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_ADMIN
    snmp_lcd->vta_data = (void *)&agt_snmpTargetAddrTable;
    snmp_lcd->vta_function = SrV3ValidateTransportAddress;
#endif /* SR_SNMPv3_ADMIN */
}

#ifdef SR_TERMINATE_CODE
void
terminate(void)
{
    /* We do not have terminate functions for
     * k_system_initialize(), k_srAgentInfo_initialize(),
     * or k_apoCap_initialize().
     */

    /* Free system group objects individually */
     if (systemData.sysName != NULL) {
         FreeOctetString(systemData.sysName);
         systemData.sysName = NULL;
     }
     if (systemData.sysLocation != NULL) {
         FreeOctetString(systemData.sysLocation);
         systemData.sysLocation = NULL;
     }
     if (systemData.sysDescr != NULL) {
         FreeOctetString(systemData.sysDescr);
         systemData.sysDescr = NULL;
     }
     if (systemData.sysContact != NULL) {
         FreeOctetString(systemData.sysContact);
         systemData.sysContact = NULL;
     }
     if (systemData.sysObjectID != NULL) {
         FreeOID(systemData.sysObjectID);
         systemData.sysObjectID = NULL;
     }

#ifdef SR_SNMPv1_ADMIN

    /* We do do not have terminate functions for
     * init_communities() and
     * init_trapCommunities().
     */

#else /* !SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_PACKET
#ifdef SR_SNMPv3_PACKET_MIB
    k_arch_terminate();
    k_mpd_terminate();
#ifdef SR_USER_SEC_MODEL
    k_usm_terminate();
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_SNMPv3_PACKET_MIB */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
#ifdef SR_SNMP_ADMIN_MIB
    k_arch_terminate();
    k_ac_terminate();
    k_ctx_terminate();
    k_group_terminate();
    k_view_terminate();
    k_tgt_terminate();
    k_notif_terminate();
#endif /* SR_SNMP_ADMIN_MIB */
#endif /* SR_SNMPv3_ADMIN */



#ifdef EXTEND_TERM
    if(EXTEND_TERM() == -1) {
	DPRINTF((APWARN,
	"Termination of agent extensions failed.  Continuing anyway.\n"));
    }
#endif /* EXTEND_TERM */




#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv3_ADMIN
    i_snmpCommunityEntry_terminate(&agt_snmpCommunityTable);
#endif /* SR_SNMPv3_ADMIN */
#endif /* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv3_PACKET
    i_arch_terminate();
    i_mpd_terminate();
#ifdef SR_USER_SEC_MODEL
    i_usm_terminate(&agt_usmUserTable);
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
    i_arch_terminate();
    i_ac_terminate(&agt_vacmAccessTable);
    i_ctx_terminate(&agt_vacmContextTable);
    i_group_terminate(&agt_vacmSecurityToGroupTable);
    i_view_terminate(&agt_vacmViewTreeFamilyTable);
    i_tgt_terminate(&agt_snmpTargetAddrTable, &agt_snmpTargetParamsTable);
    i_notif_terminate(&agt_snmpNotifyTable,
                       &agt_snmpNotifyFilterProfileTable,
                       &agt_snmpNotifyFilterTable);
    if (agt_local_snmpID != NULL) {
        FreeOctetString(agt_local_snmpID);
        agt_local_snmpID = NULL; /* LVL7 */
    }     
#endif /* SR_SNMPv3_ADMIN */

#endif /* !SR_SNMPv1_ADMIN */

}
#endif /* SR_TERMINATE_CODE */


#ifdef SR_CONFIG_FP
/* Parser records for scalars */
extern PARSER_RECORD_TYPE sysNameRecord;
extern PARSER_RECORD_TYPE sysDescrRecord;
extern PARSER_RECORD_TYPE sysObjectIDRecord;
extern PARSER_RECORD_TYPE sysLocationRecord;
extern PARSER_RECORD_TYPE sysContactRecord;
extern PARSER_RECORD_TYPE enableAuthenTrapsRecord;
#ifdef SR_SNMPv1_ADMIN
extern PARSER_RECORD_TYPE communitiesRecord;
extern PARSER_RECORD_TYPE trapCommunitiesRecord;
#endif /* SR_SNMPv1_ADMIN */

void
system_rt(
    const PARSER_RECORD_TYPE *rt[],
    int *rt_count)
{
    rt[(*rt_count)++] = &sysDescrRecord;
    rt[(*rt_count)++] = &sysObjectIDRecord;
    rt[(*rt_count)++] = &sysLocationRecord;
    rt[(*rt_count)++] = &sysContactRecord;
    rt[(*rt_count)++] = &sysNameRecord;
    rt[(*rt_count)++] = &enableAuthenTrapsRecord;
}

void
engine_rt(
    const PARSER_RECORD_TYPE *rt[],
    int *rt_count)
{
    /* Initialize rt array for parsing config file */
#ifdef SR_SNMPv1_ADMIN

    rt[(*rt_count)++] = &communitiesRecord;
    rt[(*rt_count)++] = &trapCommunitiesRecord;

#else /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv1_PACKET
#ifdef SR_SNMPv3_ADMIN
    rt[(*rt_count)++] = &agt_snmpCommunityEntryRecord;
#endif /* SR_SNMPv3_ADMIN */
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv3_PACKET
    /*
     * assign bootData so that a bootCounter should be incremented
     * when sysUpTIme wraps.
     */
    bootData = (BOOT_DATA *)malloc(sizeof(BOOT_DATA));
    if (bootData != NULL) {
        bootData->fptr = (funcp) UpdateBoot;
        bootData->rt = (void *)rt;
    }
    else {
	DPRINTF((APERROR, "Can not assign bootData structure\n"));
    }
    rt[(*rt_count)++] = &agt_snmpEngineBootsRecord;
    /* snmpEngineID under SR_SNMPv3_PACKET */
    rt[(*rt_count)++] = &SNMP_ENGINE_ID_Record;
    rt[(*rt_count)++] = &SNMP_ENGINE_ID_SRC_Record;
#ifdef SR_USER_SEC_MODEL
    rt[(*rt_count)++] = &agt_usmUserEntryRecord;
#endif /* SR_USER_SEC_MODEL */
#endif /* SR_SNMPv3_PACKET */

#ifdef SR_SNMPv3_ADMIN
    rt[(*rt_count)++] = &agt_vacmSecurityToGroupEntryRecord;
    rt[(*rt_count)++] = &agt_vacmAccessEntryRecord;
    rt[(*rt_count)++] = &agt_vacmViewTreeFamilyEntryRecord;
    rt[(*rt_count)++] = &agt_snmpNotifyEntryRecord;
    rt[(*rt_count)++] = &agt_snmpTargetAddrEntryRecord;
    rt[(*rt_count)++] = &agt_snmpTargetParamsEntryRecord;
    rt[(*rt_count)++] = &agt_snmpNotifyFilterProfileEntryRecord;
    rt[(*rt_count)++] = &agt_snmpNotifyFilterEntryRecord;
    rt[(*rt_count)++] = &agt_vacmContextEntryRecord;
#endif /* SR_SNMPv3_ADMIN */

#endif /* SR_SNMPv1_ADMIN */
}

#ifndef SR_SNMPv1_ADMIN
#endif /* SR_SNMPv1_ADMIN */
#endif /* SR_CONFIG_FP */

/*
 * This routine obtains snmp agent configuration information and uses it
 * to initialize the 'system' mib variables, traps, and communities.
 */
#define SYSSIZE 255
void
Configure(
    const PARSER_RECORD_TYPE *rt[])
{

#ifdef SR_CONFIG_FP
    struct stat buf;

    if (stat(fn_snmpd_file, &buf) == 0) {
    	if (buf.st_mode & (S_IWGRP|S_IWOTH)) {
	    DPRINTF((APWARN, 
		     "Configuration file %s is writable by group or others.\n",
		     fn_snmpd_file));
	}
	else if (buf.st_mode & (S_IRGRP|S_IROTH)) {
	    DPRINTF((APWARN,
		     "Configuration file %s is readable by group or others.\n",
		     fn_snmpd_file));
	}
    }

#if defined(SR_SNMPv3_ADMIN)
    ConvToken_snmpID_localSnmpID = agt_local_snmpID;
#endif	/* defined(SR_SNMPv3_ADMIN) */
    ParseConfigFileWithErrorLog(fn_snmpd_file, fn_snmpd_junk_file, rt);
#endif /* SR_CONFIG_FP */


#if (defined(SR_SNMP_ADMIN_MIB) && defined(SR_SNMPv3_ADMIN))
/*
 *  Add default context if not already present
 */
(void) i_vacmContextEntry_addDefaultContext(&agt_vacmContextTable);
#endif	/* (defined(SR_SNMP_ADMIN_MIB) && defined(SR_SNMPv3_ADMIN)) */

#ifdef SR_SNMPv3_PACKET
    agt_snmpEngineData.snmpEngineBoots++;
#endif /* SR_SNMPv3_PACKET */
}

#ifdef SR_CONFIG_FP
int
AgentWriteConfigFile(
    const char *filename,
    const PARSER_RECORD_TYPE *rt[])
{
#if defined(SR_SNMPv3_ADMIN)
    ConvToken_snmpID_localSnmpID = agt_local_snmpID;
#endif	/* defined(SR_SNMPv3_ADMIN) */
    return WriteConfigFile(filename, rt);
}
#endif /* SR_CONFIG_FP */

#ifdef SR_SNMPv3_PACKET
#ifdef SR_CONFIG_FP
/* 
 * updateEngineID
 *
 * this routine updates the runtime_snmpEngineID stored in the 
 * ENGINE_ID structure with agt_local_snmpID.
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
					 agt_local_snmpID);
            if (status != 0) {
                FreeOctetString(id->runtime_SNMP_ENGINE_ID);
                id->runtime_SNMP_ENGINE_ID = 
		         CloneOctetString(agt_local_snmpID);
            }
        }
	else {
            id->runtime_SNMP_ENGINE_ID = 
                     CloneOctetString(agt_local_snmpID);
	}
    }
}
#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv3_PACKET */

/*
 * UpdateBoot
 *
 * increment a snmpEngineBoots when a sysUpTime wraps.
 * this function is called from GetTimeNow() in gettime.c
 */
void UpdateBoot(BOOT_DATA *b_data)
{

#ifdef SR_SNMPv3_PACKET
    agt_snmpEngineData.snmpEngineBoots++;

#ifdef SR_CONFIG_FP
    /* update a configuration file */
    AgentWriteConfigFile(fn_snmpd_file, 
			    (const PARSER_RECORD_TYPE **)b_data->rt);
    DPRINTF((APTRACE, "sysUpTime has been wrapped. Agent configuration file updated\n"));
#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv3_PACKET */
}

#include "snmp_util_api.h"
#include "dtlapi.h"

void
SnmpReloadEngineId(OctetString * engineId)
{
  (void)snmpSemaTake();
  if (agt_local_snmpID != NULL)
  {
    FreeOctetString(agt_local_snmpID);
  }
  agt_local_snmpID = CloneOctetString(engineId);

  #ifdef SR_SNMPv3_PACKET
  i_arch_initialize(agt_local_snmpID, &agt_snmpEngineData);
  #endif

  updateEngineID(&engine_id);

  (void)snmpSemaGive();
}
