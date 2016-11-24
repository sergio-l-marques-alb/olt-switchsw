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

#include <malloc.h>

#include <errno.h>

#include <sys/types.h>


#include <string.h>

#include <stdlib.h>

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"



/*  
 *  The following macros can be used to set up system group
 *  information.  The agent attempts to provide a reasonable 
 *  default (from uname()) for sysDescr, sysObjectID, and sysName
 *  if you do not override them here.
 */

#define sysLocationString "sysLocation \"Down on the farm\""
#define sysContactString "sysContact \"SNMP Research, Inc. +1 (865) 573-1434\""
#define sysObjectIDString " "
/* #define sysObjectIDString "sysObjectID 0.0" */
#define sysDescrString  " "
/* #define sysDescrString "sysDescr \"Describe your product here\"" */
#define sysNameString  " "
/* #define sysNameString "sysName \"-\"" */

char *hardioConf[] = {
#if ! ( defined(sysDescrString) )
"sysDescr \""

#ifdef SR_SNMPv1_PACKET
    "SNMPv1"
#define SR_SYS_DESCR_ADD_SLASH
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
#ifdef SR_SYS_DESCR_ADD_SLASH
    "/"
#endif /* SR_SYS_DESCR_ADD_SLASH */
    "SNMPv2c"
#define SR_SYS_DESCR_ADD_SLASH
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
#ifdef SR_SYS_DESCR_ADD_SLASH
    "/"
#endif /* SR_SYS_DESCR_ADD_SLASH */
    "SNMPv3"
#endif /* SR_SNMPv3_PACKET */
    " agent from SNMP Research, Inc.\"",
#endif	/* ! ( defined(sysDescrString) ) */
sysLocationString,
sysContactString,
sysObjectIDString,
sysNameString,
sysDescrString,
"snmpEnableAuthenTraps 1",


#ifdef SR_SNMPv1_ADMIN
"community test1 10.1.2.3 READ 0",
"community test2 0.0.0.0 WRITE 1",
"community test3 0.0.0.0 read 2",
"community test4 10.1.2.4 write 3",
"community public 0.0.0.0 read 4",
"community proxy-letter 0.0.0.0 read 100",
"community proxy-digit 0.0.0.0 read 200",
"community proxy-other 0.0.0.0 read 300",
"community woof 0.0.0.0 write 9",
"trap test2 10.1.2.5",
#ifdef SR_EPIC
"community epic 0.0.0.0 read 10",
"community epicAdmin 0.0.0.0 write 11",
#endif /* SR_EPIC */
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_ADMIN			/* for the rest of hardioConf*/

#ifdef SR_SNMPv1_WRAPPER
#if defined(SR_SNMPv3_ADMIN)
"snmpCommunityEntry  t0000000 public public localSnmpID - anywhereTag nonVolatile",
#if defined(SR_SNMPv3_PACKET)
#if defined(SR_SNMPv1_PACKET)
#endif	/* defined(SR_SNMPv1_PACKET) */
#endif	/* defined(SR_SNMPv3_PACKET) */
#endif	/* defined(SR_SNMPv3_ADMIN) */
#endif /* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv3_PACKET

"snmpEngineBoots 0",

/* usmUser table */

#ifndef LIGHT
#define SR_USM_NO_PRIV_PROTOCOL_DEF       " usmNoPrivProtocol"
#ifndef SR_NO_PRIVACY
#define SR_USM_DES_PRIV_PROTOCOL_DEF      " usmDESPrivProtocol"
#ifdef  SR_AES
#define SR_USM_AES128_PRIV_PROTOCOL_DEF " usmAesCfb128Protocol"
#define SR_USM_AES192_PRIV_PROTOCOL_DEF " usmAESCfb192PrivProtocol"
#define SR_USM_AES256_PRIV_PROTOCOL_DEF " usmAESCfb256PrivProtocol"
#endif /* SR_AES */
#ifdef SR_3DES 
#define SR_USM_3DES_PRIV_PROTOCOL_DEF   " usm3DESPrivProtocol"
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#else /* LIGHT */
#define SR_USM_NO_PRIV_PROTOCOL_DEF       " 1.3.6.1.6.3.10.1.2.1"
#ifndef SR_NO_PRIVACY
#define SR_USM_DES_PRIV_PROTOCOL_DEF      " 1.3.6.1.6.3.10.1.2.2"
#ifdef  SR_AES
#define SR_USM_AES128_PRIV_PROTOCOL_DEF   " 1.3.6.1.4.1.14832.1.2"
#define SR_USM_AES192_PRIV_PROTOCOL_DEF   " 1.3.6.1.4.1.14832.1.3"
#define SR_USM_AES256_PRIV_PROTOCOL_DEF   " 1.3.6.1.4.1.14832.1.4"
#endif /* SR_AES */
#ifdef SR_3DES 
#define SR_USM_3DES_PRIV_PROTOCOL_DEF     " 1.3.6.1.4.1.14832.1.1"
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* LIGHT */

"usmUserEntry localSnmpID public "
    " usmNoAuthProtocol"
    SR_USM_NO_PRIV_PROTOCOL_DEF
    " nonVolatile anywhereTag"
#ifndef SR_UNSECURABLE
    " -"
#ifndef SR_NO_PRIVACY
    " -"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,

"usmUserEntry localSnmpID root "
    " usmHMACMD5AuthProtocol"
#ifndef SR_NO_PRIVACY
    SR_USM_DES_PRIV_PROTOCOL_DEF
#endif /* SR_NO_PRIVACY */
    " nonVolatile anywhereTag"
#ifndef SR_UNSECURABLE
    " authpass"
#ifndef SR_NO_PRIVACY
    " privpass"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,

"usmUserEntry localSnmpID Administrator "
    " usmHMACMD5AuthProtocol"
#ifndef SR_NO_PRIVACY
    SR_USM_DES_PRIV_PROTOCOL_DEF
#endif /* SR_NO_PRIVACY */
    " nonVolatile anywhereTag"
#ifndef SR_UNSECURABLE
    " authpass"
#ifndef SR_NO_PRIVACY
    " privpass"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,

#ifndef SR_UNSECURABLE
#ifdef SR_NO_PRIVACY
"usmUserEntry localSnmpID TemplateMD5 "
    " usmHMACMD5AuthProtocol"
    " nonVolatile nowhere"
    " \"TemplateMD5 Password\""
    ,
#else /* SR_NO_PRIVACY */

"usmUserEntry  localSnmpID TemplateMD5DES "
    " usmHMACMD5AuthProtocol"
    SR_USM_DES_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateMD5DES AuthPassword\"" 
    " \"TemplateMD5DES PrivPassword\""
    ,

"usmUserEntry  localSnmpID TemplateSHADES "
    " usmHMACSHAAuthProtocol"
    SR_USM_DES_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateSHADES AuthPassword\"" 
    " \"TemplateSHADES PrivPassword\""
    ,

#ifdef SR_3DES
"usmUserEntry  localSnmpID TemplateMD53DES "
    " usmHMACMD5AuthProtocol"
    SR_USM_3DES_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateMD53DES AuthPassword\"" 
    " \"TemplateMD53DES PrivPassword\""
    ,

"usmUserEntry  localSnmpID TemplateSHA3DES "
    " usmHMACMD5AuthProtocol"
    SR_USM_DES_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateMD5DES AuthPassword\"" 
    " \"TemplateMD5DES PrivPassword\""
    ,
#endif /* SR_3DES */

#ifdef SR_AES
"usmUserEntry  localSnmpID TemplateMD5AES "
    " usmHMACMD5AuthProtocol"
    SR_USM_AES128_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateMD5AES AuthPassword\"" 
    " \"TemplateMD5AES PrivPassword\""
    ,

"usmUserEntry  localSnmpID TemplateSHAAES "
    " usmHMACSHAAuthProtocol"
    SR_USM_AES128_PRIV_PROTOCOL_DEF
    " nonVolatile nowhere"
    " \"TemplateSHAAES AuthPassword\"" 
    " \"TemplateSHAAES PrivPassword\""
    ,

#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifdef SR_EPIC
"usmUserEntry localSnmpID epic"
#ifndef SR_UNSECURABLE
    " usmHMACMD5AuthProtocol"
#ifndef SR_NO_PRIVACY
    SR_USM_DES_PRIV_PROTOCOL_DEF
#else /* SR_NO_PRIVACY */
    SR_USM_NO_PRIV_PROTOCOL_DEF
#endif /* SR_NO_PRIVACY */
#else /* SR_UNSECURABLE */
    " usmNoAuthProtocol"
    SR_USM_NO_PRIV_PROTOCOL_DEF
#endif /* SR_UNSECURABLE */
    " nonVolatile -"
#ifndef SR_UNSECURABLE
    " \"epicpass1\""
#ifndef SR_NO_PRIVACY
    " \"epicpass2\""
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,

"usmUserEntry localSnmpID epicAdmin"
#ifndef SR_UNSECURABLE
    " usmHMACMD5AuthProtocol"
#ifndef SR_NO_PRIVACY
    SR_USM_DES_PRIV_PROTOCOL_DEF
#else /* SR_NO_PRIVACY */
    SR_USM_NO_PRIV_PROTOCOL_DEF
#endif /* SR_NO_PRIVACY */
#else /* SR_UNSECURABLE */
    " usmNoAuthProtocol"
    SR_USM_NO_PRIV_PROTOCOL_DEF
#endif /* SR_UNSECURABLE */
    " nonVolatile -"
#ifndef SR_UNSECURABLE
    " \"epicAdmin1\""
#ifndef SR_NO_PRIVACY
    " \"epicAdmin2\""
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,
#endif /* SR_EPIC */
#endif /* SR_SNMPv3_PACKET */


/* vacmSecurityToGroup table */

#if defined(SR_SNMPv1_PACKET)
"vacmSecurityToGroupEntry  snmpv1 public public nonVolatile",
#endif	/* defined(SR_SNMPv1_PACKET) */
#if defined(SR_SNMPv2c_PACKET)
"vacmSecurityToGroupEntry  snmpv2c public public nonVolatile",
#endif	/* defined(SR_SNMPv2c_PACKET) */
#if defined(SR_SNMPv3_PACKET)
"vacmSecurityToGroupEntry  usm public public nonVolatile",
"vacmSecurityToGroupEntry  usm root Administrator nonVolatile",
"vacmSecurityToGroupEntry  usm Administrator Administrator nonVolatile",
#endif	/* defined(SR_SNMPv3_PACKET) */
#if defined(SR_EPIC)
"vacmSecurityToGroupEntry  epic epic public nonVolatile",
"vacmSecurityToGroupEntry  epic epicAdmin Administrator nonVolatile",
#endif	/* defined(SR_EPIC) */


/* vacmAccess table */
"vacmAccessEntry  public  - snmpv1 noAuthNoPriv exact ApplicationsView - "
    "ApplicationsView nonVolatile",
"vacmAccessEntry  public  - snmpv2c noAuthNoPriv exact ApplicationsView - "
    "ApplicationsView nonVolatile",
#if defined(SR_SNMPv3_PACKET)
"vacmAccessEntry  public  - usm noAuthNoPriv exact ApplicationsView - "
    "ApplicationsView nonVolatile",
"vacmAccessEntry  Administrator - usm noAuthNoPriv exact restrictedView - "
    "restrictedView nonVolatile",
#ifndef SR_UNSECURABLE
"vacmAccessEntry  Administrator - usm authNoPriv exact All All All nonVolatile",
#ifndef SR_NO_PRIVACY
"vacmAccessEntry  Administrator - usm authPriv exact All All All nonVolatile",
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#endif	/* defined(SR_SNMPv3_PACKET) */
#ifdef SR_EPIC
"vacmAccessEntry  public  - epic noAuthNoPriv exact ApplicationsView - "
    "ApplicationsView nonVolatile",
"vacmAccessEntry  Administrator - epic noAuthNoPriv exact restrictedView - "
    "restrictedView nonVolatile",
#ifndef SR_UNSECURABLE
"vacmAccessEntry  public  - epic authNoPriv exact ApplicationsView "
    "ApplicationsView ApplicationsView nonVolatile",
"vacmAccessEntry  Administrator - epic authNoPriv exact All All All "
    "nonVolatile",
#ifndef SR_NO_PRIVACY
"vacmAccessEntry  public  - epic authPriv exact ApplicationsView "
    "ApplicationsView ApplicationsView nonVolatile",
"vacmAccessEntry  Administrator - epic authPriv exact All All All "
    "nonVolatile",
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#endif /* SR_EPIC */

/* vacmViewTreeFamily table */

#ifdef LIGHT

"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.2.1.1  - included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.4.1.99.12.19 - included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.6.3.1.1.4 - included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.6.3.1.1.5 - included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.4.1.99.12.19.1 - included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  restrictedView 1.3.6.1.2.1.2.2.1.0.2 ff:bf included "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  All 1 0 included nonVolatile ",
"vacmViewTreeFamilyEntry  ApplicationsView 1 0 included nonVolatile ",
"vacmViewTreeFamilyEntry  ApplicationsView 1.3.6.1.6.3.18.1.1 - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView 1.3.6.1.6.3.16.1.4 - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView 1.3.6.1.6.3.16.1.2 - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView 1.3.6.1.6.3.15.1.2.2 - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView 1.3.6.1.6.3.16.1.5.2 - excluded "
    "nonVolatile" ,
#else /* LIGHT */
"vacmViewTreeFamilyEntry  restrictedView system - included nonVolatile ",
"vacmViewTreeFamilyEntry  restrictedView srExamples - included nonVolatile ",
"vacmViewTreeFamilyEntry  restrictedView snmpTrap - included nonVolatile ",
"vacmViewTreeFamilyEntry  restrictedView snmpTraps - included nonVolatile ",
"vacmViewTreeFamilyEntry  restrictedView srBasics - included nonVolatile ",
"vacmViewTreeFamilyEntry  restrictedView ifEntry.0.2 ff:bf included "
    "nonVolatile ",
"vacmViewTreeFamilyEntry  All iso 0 included nonVolatile ",
"vacmViewTreeFamilyEntry  ApplicationsView iso 0 included nonVolatile ",
"vacmViewTreeFamilyEntry  ApplicationsView snmpCommunityTable - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView vacmAccessTable - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView vacmSecurityToGroupTable - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView usmUserTable - excluded "
    "nonVolatile" ,
"vacmViewTreeFamilyEntry  ApplicationsView vacmViewTreeFamilyTable - excluded "
    "nonVolatile" , 
#endif /* LIGHT */
"vacmViewTreeFamilyEntry  HtmlPage 1.3.6.1.4.1.99.12.30 - included "
    "nonVolatile" ,

/* snmpNotify table */
"snmpNotifyEntry  Traps TrapTag trap nonVolatile",
"snmpNotifyEntry  Informs InformTag inform nonVolatile",

/* snmpTargetAddr table */
/* 1.3.6.1.6.1.1 == snmpUDPDomain */
#if defined(SR_SNMPv1_PACKET)
"snmpTargetAddrEntry  localHostV1 1.3.6.1.6.1.1 127.0.0.1:0 100 3 TrapTag "
    "v1ExampleParams nonVolatile 255.255.255.255:0",
#endif	/* defined(SR_SNMPv1_PACKET) */
#if defined(SR_SNMPv2c_PACKET)
"snmpTargetAddrEntry  localHostV2c 1.3.6.1.6.1.1 127.0.0.1:0 100 3 InformTag "
    "v2cExampleParams nonVolatile 255.255.255.255:0",
#endif	/* defined(SR_SNMPv2c_PACKET) */
"snmpTargetAddrEntry  anywhere 1.3.6.1.6.1.1 0.0.0.0:0 0 0 "
    "anywhereTag none nonVolatile 0.0.0.0:0",

/* snmpTargetParams table */
#if defined(SR_SNMPv1_PACKET)
"snmpTargetParamsEntry  v1ExampleParams 0 snmpv1 public noAuthNoPriv "
    "nonVolatile",
#endif	/* defined(SR_SNMPv1_PACKET) */
#if defined(SR_SNMPv2c_PACKET)
"snmpTargetParamsEntry  v2cExampleParams 1 snmpv2c public noAuthNoPriv "
    "nonVolatile",
#endif	/* defined(SR_SNMPv2c_PACKET) */
#if defined(SR_SNMPv3_PACKET)
#ifndef SR_UNSECURABLE
"snmpTargetParamsEntry  v3ExampleParams 3 usm root authNoPriv "
    "nonVolatile",
#else /* SR_UNSECURABLE */
"snmpTargetParamsEntry  v3ExampleParams 3 usm root noAuthNoPriv "
    "nonVolatile",
#endif /* SR_UNSECURABLE */
#endif	/* defined(SR_SNMPv3_PACKET) */

/* snmpNotifyFilterProfile table */
"snmpNotifyFilterProfileEntry v1ExampleParams wellKnownTraps nonVolatile",

/* snmpNotifyFilter table */
/* snmpTraps == 1.3.6.1.6.3.1.1.5 */
"snmpNotifyFilterEntry wellKnownTraps 1.3.6.1.6.3.1.1.5 - included nonVolatile",

/* snmpProxy table */
#if defined(SR_SNMPv3_PACKET)
#if defined(SR_SNMPv1_PACKET)
#endif	/* defined(SR_SNMPv1_PACKET) */
#endif	/* defined(SR_SNMPv3_PACKET) */
#endif /* SR_SNMPv3_ADMIN */
NULL
};


char *mgrConf[] = {

#if defined(SR_SNMPv3_PACKET)
#if defined(SR_SNMPv3_ADMIN)

"snmpEngineBoots 0",

/* usmUser table */

#ifndef SR_USM_NO_PRIV_PROTOCOL_DEF
#define SR_USM_NO_PRIV_PROTOCOL_DEF       " 1.3.6.1.6.3.10.1.2.1"
#ifndef SR_NO_PRIVACY
#define SR_USM_DES_PRIV_PROTOCOL_DEF      " 1.3.6.1.6.3.10.1.2.2"
#endif /* SR_NO_PRIVACY */
#endif /* SR_USM_NO_PRIV_PROTOCOL_DEF */

"usmUserEntry localSnmpID rootNoAuthTemplate "
    " usmNoAuthProtocol"  
    SR_USM_NO_PRIV_PROTOCOL_DEF
    " readOnly -"
#ifndef SR_UNSECURABLE
    " -"
#ifndef SR_NO_PRIVACY
    " -"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    ,

#ifndef SR_UNSECURABLE
"usmUserEntry localSnmpID rootAuthTemplate "
    " usmHMACMD5AuthProtocol"  
    SR_USM_NO_PRIV_PROTOCOL_DEF
    " readOnly -"
    " rootAuthTemplateAuthPassword"
#ifndef SR_NO_PRIVACY
    " -"
#endif /* SR_NO_PRIVACY */
    ,
#endif /* SR_UNSECURABLE */


#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
"usmUserEntry localSnmpID rootAuthTemplate "
    " usmHMACMD5AuthProtocol"  
    SR_USM_DES_PRIV_PROTOCOL_DEF
    " readOnly -"
    " rootAuthTemplateAuthPassword"
    " rootPrivTemplatePrivPassword" 
    ,
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

 
#endif	/* defined(SR_SNMPv3_ADMIN) */
#endif	/* defined(SR_SNMPv3_PACKET) */

NULL
};




char *
sr_fgets(buf, size, cfp)
char *buf;
int size;
ConfigFile *cfp;
{

    if ((strcmp(cfp->filename, "hardioConf")) == 0) { 
        if ( hardioConf[cfp->linecount] == NULL) {
            return NULL;
        } else {
            strncpy(buf, hardioConf[cfp->linecount], size - 1);
            if ( strlen(hardioConf[cfp->linecount]) >= (size_t)(size - 1) ) {
                buf[size - 1] = '\0';
                DPRINTF((APWARN, "sr_fgets: line %d from hardioConf was truncated.\n",
                                  cfp->linecount));
            }
        }
    }
    else if ((strcmp(cfp->filename, "mgrConf")) == 0) {
        if ( mgrConf[cfp->linecount] == NULL) {
            return NULL;
        } else {
            strncpy(buf, mgrConf[cfp->linecount], size - 1);
            if ( strlen(mgrConf[cfp->linecount]) >= (size_t)(size - 1) ) {
                buf[size -1] = '\0';
                DPRINTF((APWARN, "sr_fgets: line %d from mgrConf was truncated.\n", 
                                  cfp->linecount));
            }
        }
    } 
    return buf;
}

int
sr_feof(cfp)
ConfigFile *cfp;
{
    if ((strcmp(cfp->filename, "hardioConf")) == 0) {
        if ( hardioConf[cfp->linecount] == NULL) {
            return -1;
        }
    }
    else if ((strcmp(cfp->filename, "mgrConf")) == 0) {
        if (mgrConf[cfp->linecount] == NULL) {
           return -1;
        }
    }
    return 0;
}

/* 
 * This routine opens a config file and keeps information about it.
 */
int
OpenConfigFile(filename, mode, cfgp)
const char *filename;
int mode;
ConfigFile **cfgp;
{
    int retval =  FS_OK;
    ConfigFile *tmpcfgp;

    tmpcfgp = NULL;

    if ( mode == FM_WRITE ) {
        retval = FS_ERROR;
    }
    else {
        if((tmpcfgp = (ConfigFile *) malloc(sizeof(ConfigFile))) == 0) {
           DPRINTF((APWARN,"OpenConfigFile: couldn't get ConfigFile struct\n"));
           retval = FS_ERROR;
        } else {
           /* save the filename and init the line count */
           if (strstr(filename, "snmpd.cnf")) {
               strcpy(tmpcfgp->filename, "hardioConf");
           }
           else if (strstr(filename, "mgr.cnf")) {
               strcpy(tmpcfgp->filename, "mgrConf");
           }           
           tmpcfgp->linecount = 0;
#ifndef SR_NO_COMMENTS_IN_CONFIG
           tmpcfgp->curCommentEntry = NULL;
#endif /* SR_NO_COMMENTS_IN_CONFIG */
        }
    }

    /* pass back the pointer to the config structure */
    *cfgp = tmpcfgp;

    return(retval);
}

/*
 * This routine closes configuration files and frees their info structs.
 */
void
CloseConfigFile(
    ConfigFile *cfgp)
{
    if(cfgp) {
        free((char *) cfgp);
    }
}

void
write_config_log(
    const char *logfilename,
    const char *logmessage)
{
}

int
sr_fprintf(
    ConfigFile *cfp,
    const char *buf)
{
    return 0;
}
