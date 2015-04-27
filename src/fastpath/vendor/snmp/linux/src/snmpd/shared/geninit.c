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

#ifdef SR_GENERATE_CONFIGURATION


#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>


#include <ctype.h>

#include <string.h>

#include <errno.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <netdb.h>


#include <sys/socket.h>

#include <sys/stat.h>

#include <netinet/in.h>

#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "snmpid.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sitedefs.h"
#include "diag.h"
SR_FILENAME

#define USERNAME_BUFSIZ 256
#define AUTHPASS_BUFSIZ 256
#define PRIVPASS_BUFSIZ 256
#define ENTRY_BUFSIZ 512
#define FORMATSTRING_BUFSIZ 512

#include "geninit.h"

#if (defined(SR_GENERATE_OLD_STYLE_CONFIGURATION) || ! ( defined(SR_SNMPv3_ADMIN) ))

#if defined(SR_SNMPv3_PACKET)
static int GenerateUsers(
    ConfigFile *cfgp);

static int
GenerateUsers(
    ConfigFile *cfgp)
{
    char *username = NULL;
    char *authprot = "usmNoAuthProtocol";
#ifndef LIGHT
    char *privprot = "usmNoPrivProtocol";
#else /* LIGHT */
    char *privprot = "1.3.6.1.6.3.10.1.2.1";
#endif /* LIGHT */
#ifndef SR_UNSECURABLE
    char *authpass = NULL;
#ifndef SR_NO_PRIVACY
    char *privpass = NULL;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    char *entry = NULL;
    int status = 0;
    char *end;

    username = (char *)malloc(USERNAME_BUFSIZ);
    if (username == NULL) {
        printf("Error, could not malloc username buffer, aborting.\n");
        goto done;
    }

#ifndef SR_UNSECURABLE
    authpass = (char *)malloc(AUTHPASS_BUFSIZ);
    if (authpass == NULL) {
        printf("Error, could not malloc authpass buffer, aborting.\n");
        goto done;
    }
#ifndef SR_NO_PRIVACY
    privpass = (char *)malloc(PRIVPASS_BUFSIZ);
    if (privpass == NULL) {
        printf("Error, could not malloc privpass buffer, aborting.\n");
        goto done;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    entry = (char *)malloc(ENTRY_BUFSIZ);
    if (entry == NULL) {
        printf("Error, could not malloc entry buffer, aborting.\n");
        goto done;
    }

    printf("Enter User Name: ");
    fgets(username, USERNAME_BUFSIZ, stdin);
    end = strrchr(username,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    if (strlen(username) == 0) {
        printf("Finished entering users.\n");
        goto done;
    }

#ifndef SR_UNSECURABLE
    printf("Enter Authentication Password: ");
    fgets(authpass, AUTHPASS_BUFSIZ, stdin);
    end = strrchr(authpass,'\n');
    if (end != NULL) {
       *end = '\0';
    }
#ifndef SR_NO_PRIVACY
    if (strlen(authpass) != 0) {
        printf("Enter Privacy Password       : ");
        fgets(privpass, PRIVPASS_BUFSIZ, stdin);
        end = strrchr(privpass,'\n');
        if (end != NULL) {
           *end = '\0';
        }
        if (strlen(privpass) == 0) {
            strcpy(privpass, "-");
        } else {
#ifndef LIGHT
            privprot = "usmDESPrivProtocol";
#else /* LIGHT */
            privprot = "1.3.6.1.6.3.10.1.2.2";
#endif /* LIGHT */
        }
    }
    else {
        strcpy(privpass, "-");
    }
#endif /* SR_NO_PRIVACY */
    if (strlen(authpass) == 0) {
        strcpy(authpass, "-");
    } else {
#ifndef LIGHT
        authprot = "usmHMACMD5AuthProtocol";
#else /* LIGHT */
        authprot = "1.3.6.1.6.3.10.1.1.2";
#endif /* LIGHT */
    }
#endif /* SR_UNSECURABLE */

#ifdef SR_SNMPv3_PACKET
    sprintf(entry, "usmUserEntry localSnmpID %s %s %s nonVolatile -"
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            " \"%s\" \"%s\"\n",
            username, authprot, privprot, authpass, privpass);
#else /* SR_NO_PRIVACY */
            " \"%s\"\n", username, authprot, privprot, authpass);
#endif /* SR_NO_PRIVACY */
#else /* SR_UNSECURABLE */
            "\n", username, authprot, privprot);
#endif /* SR_UNSECURABLE */
    sr_fprintf(cfgp, entry);

    sprintf(entry,
            "vacmSecurityToGroupEntry usm %s superUser nonVolatile\n",
            username);
    sr_fprintf(cfgp, entry);
#endif /* SR_SNMPv3_PACKET */

    status = 1;

  done:
    if (username) {
        free(username);
    }
#ifndef SR_UNSECURABLE
    if (authpass) {
        free(authpass);
    }
#ifndef SR_NO_PRIVACY
    if (privpass) {
        free(privpass);
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    if (entry) {
        free(entry);
    }
    return status;
}
#endif	/* defined(SR_SNMPv3_PACKET) */

void
GenerateAgentDefaults(
    char *filename)
{
    ConfigFile *cfgp;
    char *sp;
    char *formatString = NULL;
    char *end;

    FNAME("GenerateAgentDefaults")

    sp = NULL;

    if (OpenConfigFile(filename, FM_READ, &cfgp) == FS_OK) {
        CloseConfigFile(cfgp);
        return;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_WRITE, &cfgp) != FS_OK) {
        DPRINTF((APERROR, "%s: Cannot open configuration.\n", Fname));
        return;
    }

    formatString = (char *)malloc(FORMATSTRING_BUFSIZ);

    if (formatString == NULL) {
        DPRINTF((APERROR, "Could not malloc buffers in GenerateDefaults.\n"));
        goto done;
    }

    DPRINTF((APALWAYS, "Generating factory default configuration values...\n"));

    /* system group */

    /* sysDescr */
    printf("Enter sysDescr: ");
    strcpy(formatString, "sysDescr \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysLocation */
    printf("Enter sysLocation: ");
    strcpy(formatString, "sysLocation \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysContact */
    printf("Enter sysContact: ");
    strcpy(formatString, "sysContact \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysName */
    printf("Enter sysName: ");
    strcpy(formatString, "sysName \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysObjectID is set to 0.0 -Please replace this with your enterprise ID */
    sr_fprintf(cfgp, "sysObjectID 0.0\n");

    /* snmpEnableAuthenTraps */
    sr_fprintf(cfgp, "snmpEnableAuthenTraps 1\n");

#ifdef SR_SNMPv1_ADMIN
    sr_fprintf(cfgp, "community public 0.0.0.0 read 1\n");
    sr_fprintf(cfgp, "community sysadmin 0.0.0.0 write 1\n");

    printf("Enter IP address for trap destination: ");
    strcpy(formatString, "trap public ");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    sr_fprintf(cfgp, formatString);
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMPv3_PACKET
    /* snmpEngineBoots */
    sr_fprintf(cfgp, "snmpEngineBoots 0\n");
#endif /* SR_SNMPv3_PACKET */

#if defined(SR_SNMPv3_PACKET)
    /* Prompt for usernames and passwords */
    printf("Enter User Names and Passwords.  Finish by hitting <return>\n");
    while (GenerateUsers(cfgp));
#endif	/* defined(SR_SNMPv3_PACKET) */

#ifdef SR_SNMPv3_ADMIN
    /* vacmViewTreeFamilyTable */
    sr_fprintf(cfgp,
        "vacmViewTreeFamilyEntry  All 0.0 - included nonVolatile\n"
        "vacmViewTreeFamilyEntry  All 1 - included nonVolatile\n");

    /* vacmAccessTable */
#ifdef SR_SNMPv1_PACKET
    sr_fprintf(cfgp, "vacmAccessEntry  visitor - snmpv1 noAuthNoPriv prefix"
                     " All - All nonVolatile\n"
                     "vacmAccessEntry  superUser - snmpv1 noAuthNoPriv prefix"
                     " All All All nonVolatile\n");
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    sr_fprintf(cfgp, "vacmAccessEntry  visitor - snmpv2c noAuthNoPriv prefix"
                     " All - All nonVolatile\n"
                     "vacmAccessEntry  superUser - snmpv2c noAuthNoPriv prefix"
                     " All All All nonVolatile\n");
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    sr_fprintf(cfgp,
#ifdef SR_UNSECURABLE
                     "vacmAccessEntry  superUser - usm noAuthNoPriv prefix"
                     " All All All nonVolatile\n"
#else /* SR_UNSECURABLE */
                     "vacmAccessEntry  superUser - usm noAuthNoPriv prefix"
                     " All - All nonVolatile\n"
#ifdef SR_NO_PRIVACY
                     "vacmAccessEntry  superUser - usm authNoPriv prefix"
                     " All All All nonVolatile\n"
#else /* SR_NO_PRIVACY */
                     "vacmAccessEntry  superUser - usm authNoPriv prefix"
                     " All - All nonVolatile\n"
                     "vacmAccessEntry  superUser - usm authPriv prefix"
                     " All All All nonVolatile\n"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
              );
#endif /* SR_SNMPv3_PACKET */


    /* snmpTargetAddrEntry table */
    printf("Enter IP address for trap destination: ");
    strcpy(formatString, "snmpTargetAddrEntry stae1 1.3.6.1.6.1.1 ");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, ":162 100 3 mgr1 stpe1 nonVolatile 0.0.0.0:0\n");
    sr_fprintf(cfgp, formatString);
    sp = strstr(formatString, "stae1");
    sp[4] = '2';
    sp = strstr(formatString, "stpe1");
    sp[4] = '2';
    sr_fprintf(cfgp, formatString);

    /* snmpTargetParams table */
    sr_fprintf(cfgp, "snmpTargetParamsEntry  stpe1 0 snmpv1 sysadmin "
                     "noAuthNoPriv nonVolatile\n"
                     "snmpTargetParamsEntry  stpe2 1 snmpv2c sysadmin "
                     "noAuthNoPriv nonVolatile\n");

    /* snmpNotify table */
    sr_fprintf(cfgp, "snmpNotifyEntry sne1 mgr1 1 nonVolatile\n");

    /* vacmSecurityToGroup table */
    sr_fprintf(cfgp, 
            "vacmSecurityToGroupEntry snmpv1 public visitor nonVolatile\n");

    sr_fprintf(cfgp, 
            "vacmSecurityToGroupEntry snmpv2c public visitor nonVolatile\n");

#endif /* SR_SNMPv3_ADMIN */

#ifdef SR_SNMPv1_PACKET_MIB
    /* community table */
#ifdef SR_SNMPv3_ADMIN
    sr_fprintf(cfgp, "snmpCommunityEntry  t0000000 public public localSnmpID "
                     "- - nonVolatile\n");
#endif /* SR_SNMPv3_ADMIN */
#endif /* SR_SNMPv1_PACKET_MIB */


    CloseConfigFile(cfgp);

done:
    if (formatString) {
        free(formatString);
    }
}

#endif	/* (defined(SR_GENERATE_OLD_STYLE_CONFIGURATION) || ! ( defined(SR_SNMPv3_ADMIN) )) */

#if (defined(SR_GENERATE_SNMPV3_CONFIGURATION) && defined(SR_SNMPv3_ADMIN))

#if defined(SR_SNMPv3_PACKET)
static void GenerateUsers(
    ConfigFile *cfgp,
    int secPosture);

#ifndef HOTSPOT
static void
GenerateUsers(
    ConfigFile *cfgp,
    int secPosture)
{
#ifndef LIGHT
    char *authprot = "usmNoAuthProtocol";
    char *privprot = "usmNoPrivProtocol";
#else /* LIGHT */
    char *authprot = "1.3.6.1.6.3.10.1.1.1";
    char *privprot = "1.3.6.1.6.3.10.1.2.1";
#endif /* LIGHT */
#ifndef SR_UNSECURABLE
    char *authpass = NULL;
#ifndef SR_NO_PRIVACY
    char *privpass = NULL;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    char *entry = NULL;
    char *end;

    if (secPosture == 3) {
        return;
    }

#ifndef SR_UNSECURABLE
    authpass = (char *)malloc(AUTHPASS_BUFSIZ);
    if (authpass == NULL) {
        printf("Error, could not malloc authpass buffer, aborting.\n");
        goto done;
    }
#ifndef SR_NO_PRIVACY
    privpass = (char *)malloc(PRIVPASS_BUFSIZ);
    if (privpass == NULL) {
        printf("Error, could not malloc privpass buffer, aborting.\n");
        goto done;
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    entry = (char *)malloc(ENTRY_BUFSIZ);
    if (entry == NULL) {
        printf("Error, could not malloc entry buffer, aborting.\n");
        goto done;
    }

#ifndef SR_UNSECURABLE
    printf("You must enter passwords for the initial user.\n");
    strcpy(authpass, "");
    while (strlen(authpass) == 0) {
        printf("Enter Authentication Password: ");
        fgets(authpass, AUTHPASS_BUFSIZ, stdin);
        end = strrchr(authpass,'\n');
        if (end != NULL) {
           *end = '\0';
        }
    }
#ifndef SR_NO_PRIVACY
    strcpy(privpass, "");
    while (strlen(privpass) == 0) {
        printf("Enter Privacy Password       : ");
        fgets(privpass, PRIVPASS_BUFSIZ, stdin);
        end = strrchr(privpass,'\n');
        if (end != NULL) {
           *end = '\0';
        }
    }
#ifndef LIGHT
    privprot = "usmDESPrivProtocol";
#else /* LIGHT */
    privprot = "1.3.6.1.6.3.10.1.2.2";
#endif /* LIGHT */
#endif /* SR_NO_PRIVACY */
#ifndef LIGHT
    authprot = "usmHMACMD5AuthProtocol";
#else /* LIGHT */
    authprot = "1.3.6.1.6.3.10.1.1.2";
#endif /* LIGHT */
#endif /* SR_UNSECURABLE */

#ifdef SR_SNMPv3_PACKET
    sprintf(entry, "usmUserEntry localSnmpID initial %s %s nonVolatile -"
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
            " \"%s\" \"%s\"\n",
            authprot, privprot, authpass, privpass);
#else /* SR_NO_PRIVACY */
            " \"%s\"\n", authprot, privprot, authpass);
#endif /* SR_NO_PRIVACY */
#else /* SR_UNSECURABLE */
            "\n", authprot, privprot);
#endif /* SR_UNSECURABLE */
    sr_fprintf(cfgp, entry);

    /*
     *   Write the SNMPv3 template users.
     */
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY

    /* --- Auth/Priv goes here --- */
    memset(entry, 0x00, sizeof(ENTRY_BUFSIZ));
    sprintf(entry, 
            "usmUserEntry localSnmpID templateMD5 usmHMACMD5AuthProtocol "
            "usmDESPrivProtocol nonVolatile - \"TMD5ap4cfu\" \"TMD5pp4cfu\" \n");
    sr_fprintf(cfgp, entry);

    memset(entry, 0x00, sizeof(ENTRY_BUFSIZ));
    sprintf(entry, 
            "usmUserEntry localSnmpID templateSHA usmHMACSHAAuthProtocol "
            "usmDESPrivProtocol nonVolatile - \"TSHAiaao\" \"TSHApiapo\" \n");
    sr_fprintf(cfgp, entry);

#else /* SR_NO_PRIVACY */

    /* --- Auth/noPriv goes here --- */
    memset(entry, 0x00, sizeof(ENTRY_BUFSIZ));
    sprintf(entry,
            "usmUserEntry  localSnmpID templateMD5 usmHMACMD5AuthProtocol "
            "usmNoPrivProtocol nonVolatile - \"TMD5ap4cfu\"");
    sr_fprintf(cfgp, entry);

#endif /* SR_NO_PRIVACY */
#else /* SR_UNSECURABLE */
#endif /* SR_UNSECURABLE */

    sr_fprintf(cfgp, 
        "vacmSecurityToGroupEntry usm initial initial nonVolatile\n");
#endif /* SR_SNMPv3_PACKET */

  done:
#ifndef SR_UNSECURABLE
    if (authpass) {
        free(authpass);
    }
#ifndef SR_NO_PRIVACY
    if (privpass) {
        free(privpass);
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    if (entry) {
        free(entry);
    }
}
#else /* HOTSPOT */
static void
GenerateUsers(
    ConfigFile *cfgp,
    int secPosture)
{
    if (secPosture == 3) {
        return;
    }

#ifdef SR_SNMPv3_PACKET
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID initial usmHMACMD5AuthProtocol 1.3.6.1.6.3.10.1.2.2 nonVolatile - initialPass initialPass\n");
    sr_fprintf(cfgp, 
        "vacmSecurityToGroupEntry usm initial initial nonVolatile\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID initial2 usmHMACSHAAuthProtocol 1.3.6.1.6.3.10.1.2.2 nonVolatile - initialPass initialPass\n");
    sr_fprintf(cfgp, 
        "vacmSecurityToGroupEntry usm initial2 initial nonVolatile\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID template1 usmNoAuthProtocol 1.3.6.1.6.3.10.1.2.1 nonVolatile - - -\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID template2 usmHMACMD5AuthProtocol 1.3.6.1.6.3.10.1.2.1 nonVolatile - template2Pass -\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID template3 usmHMACSHAAuthProtocol 1.3.6.1.6.3.10.1.2.1 nonVolatile - template3Pass -\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID template4 usmHMACMD5AuthProtocol 1.3.6.1.6.3.10.1.2.2 nonVolatile - template4Pass template4Pass\n");
    sr_fprintf(cfgp, 
        "usmUserEntry localSnmpID template5 usmHMACSHAAuthProtocol 1.3.6.1.6.3.10.1.2.2 nonVolatile - template5Pass template5Pass\n");
#endif /* SR_SNMPv3_PACKET */
}
#endif /* HOTSPOT */
#endif	/* defined(SR_SNMPv3_PACKET) */

void
GenerateAgentDefaults(
    char *filename)
{
    ConfigFile *cfgp;
    char *formatString = NULL;
    int secPosture = -1;
    char *end;
    FNAME("GenerateAgentDefaults")

    if (OpenConfigFile(filename, FM_READ, &cfgp) == FS_OK) {
        CloseConfigFile(cfgp);
        return;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_WRITE, &cfgp) != FS_OK) {
        DPRINTF((APERROR, "%s: Cannot open configuration.\n", Fname));
        return;
    }

    formatString = (char *)malloc(FORMATSTRING_BUFSIZ);

    if (formatString == NULL) {
        DPRINTF((APERROR, "Could not malloc buffers in GenerateDefaults.\n"));
        goto done;
    }

    DPRINTF((APALWAYS, "Generating factory default configuration values...\n"));

    /* system group */

    /* sysDescr */
    printf("Enter sysDescr: ");
    strcpy(formatString, "sysDescr \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysLocation */
    printf("Enter sysLocation: ");
    strcpy(formatString, "sysLocation \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysContact */
    printf("Enter sysContact: ");
    strcpy(formatString, "sysContact \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysName */
    printf("Enter sysName: ");
    strcpy(formatString, "sysName \"");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, "\"\n");
    sr_fprintf(cfgp, formatString);

    /* sysObjectID is set to 0.0 -Please replace this with your enterprise ID */
    sr_fprintf(cfgp, "sysObjectID 0.0\n");

    /* snmpEnableAuthenTraps */
    sr_fprintf(cfgp, "snmpEnableAuthenTraps 1\n");

#ifdef SR_SNMPv3_PACKET
    /* snmpEngineBoots */
    sr_fprintf(cfgp, "snmpEngineBoots 0\n");
#endif /* SR_SNMPv3_PACKET */

    while ((secPosture < 1) || (secPosture > 3)) {
        printf("The following security postures are available (see RFCs 3414 and 3415):\n"
               "    1) Minimum-secure\n"
               "    2) Semi-secure\n"
               "    3) Very-secure\n"
               "Select a security posture: ");
        fgets(formatString, FORMATSTRING_BUFSIZ, stdin);
        end = strrchr(formatString,'\n');
        if (end != NULL) {
           *end = '\0';
        }
        sscanf(formatString, "%d", &secPosture);
    }

#if defined(SR_SNMPv3_PACKET)
    GenerateUsers(cfgp, secPosture);
#endif	/* defined(SR_SNMPv3_PACKET) */

    /* vacmViewTreeFamilyTable */
    sr_fprintf(cfgp,
        "vacmViewTreeFamilyEntry  internet 1.3.6.1 - included nonVolatile\n");
    if (secPosture == 1) {
        sr_fprintf(cfgp,
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1 - "
                "included nonVolatile\n");
    }
    if (secPosture == 2) {
        sr_fprintf(cfgp,
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1.2.1.1 - "
                "included nonVolatile\n"
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1.2.1.11 - "
                "included nonVolatile\n"
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1.6.3.10.2.1 - "
                "included nonVolatile\n"
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1.6.3.11.2.1 - "
                "included nonVolatile\n"
            "vacmViewTreeFamilyEntry  restricted 1.3.6.1.6.3.15.1.1 - "
                "included nonVolatile\n"
            );
    }

    /* vacmAccessTable */
#ifdef SR_SNMPv1_PACKET
    sr_fprintf(cfgp, "vacmAccessEntry  visitor - snmpv1 noAuthNoPriv prefix"
                     " restricted - restricted nonVolatile\n"
                     "vacmAccessEntry  initial - snmpv1 noAuthNoPriv prefix"
                     " internet internet internet nonVolatile\n");
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    sr_fprintf(cfgp, "vacmAccessEntry  visitor - snmpv2c noAuthNoPriv prefix"
                     " restricted - restricted nonVolatile\n"
                     "vacmAccessEntry  initial - snmpv2c noAuthNoPriv prefix"
                     " internet internet internet nonVolatile\n");
#endif /* SR_SNMPv2c_PACKET */

#ifdef SR_SNMPv3_PACKET
    sr_fprintf(cfgp, "vacmAccessEntry  initial - usm noAuthNoPriv prefix"
                     " restricted - restricted nonVolatile\n"
#ifndef SR_UNSECURABLE
                     "vacmAccessEntry  initial - usm authNoPriv prefix"
                     " internet internet internet nonVolatile\n"
#ifndef SR_NO_PRIVACY
                     "vacmAccessEntry  initial - usm authPriv prefix"
                     " internet internet internet nonVolatile\n"
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
              );
#endif /* SR_SNMPv3_PACKET */


    /* snmpTargetAddrEntry table */
    printf("Enter IP address for trap destination: ");
    strcpy(formatString, "snmpTargetAddrEntry stae1 1.3.6.1.6.1.1 ");
    fgets(formatString + strlen(formatString),
          FORMATSTRING_BUFSIZ - strlen(formatString),
          stdin);
    end = strrchr(formatString,'\n');
    if (end != NULL) {
       *end = '\0';
    }
    strcat(formatString, ":162 100 3 mgr1 stpe1 nonVolatile 0.0.0.0:0\n");
    sr_fprintf(cfgp, formatString);

    /* snmpTargetParams table */

    /* --- Minimum security posture, send an SNMPv1 trap --- */
    if (secPosture == 1) {
        sr_fprintf(cfgp, "snmpTargetParamsEntry  stpe1 0 snmpv1 public "
                         "noAuthNoPriv nonVolatile\n");
    }

    /* --- Semi-secure security posture, send an SNMPv3 authNoPriv trap --- */
    if (secPosture == 2) {
        sr_fprintf(cfgp, "snmpTargetParamsEntry  stpe1 3 usm initial "
                         "authNoPriv nonVolatile\n");
    }


    /* snmpNotify table */
    sr_fprintf(cfgp, "snmpNotifyEntry sne1 mgr1 1 nonVolatile\n");

    /* vacmSecurityToGroup table */
    sr_fprintf(cfgp, 
            "vacmSecurityToGroupEntry snmpv1 public visitor nonVolatile\n");

    sr_fprintf(cfgp, 
            "vacmSecurityToGroupEntry snmpv2c public visitor nonVolatile\n");


#ifdef SR_SNMPv1_PACKET_MIB
    /* snmpCommunity table */

    /* --- Minimum security posture --- */
    if (secPosture == 1) {
        sr_fprintf(cfgp, "snmpCommunityEntry t00000000 public public "
                         "localSnmpID - - nonVolatile\n"
                         "snmpCommunityEntry t00000001 sysadmin initial "
                         "localSnmpID - - nonVolatile\n");
        sr_fprintf(cfgp, 
                "vacmSecurityToGroupEntry snmpv1 initial initial nonVolatile\n");
        sr_fprintf(cfgp, 
                "vacmSecurityToGroupEntry snmpv2c initial initial nonVolatile\n");
    }

    /* --- Semi-secure security posture --- */
    if (secPosture == 2) {
        sr_fprintf(cfgp, "snmpCommunityEntry t00000000 public public "
                         "localSnmpID - - nonVolatile\n"
                         "snmpCommunityEntry t00000001 sysadmin public "
                         "localSnmpID - - nonVolatile\n");
    }

    /* --- Maximum security posture --- */
    if (secPosture == 3) {
    }
#endif /* SR_SNMPv1_PACKET_MIB */


    CloseConfigFile(cfgp);

done:
    if (formatString) {
        free(formatString);
    }
}

#endif	/* (defined(SR_GENERATE_SNMPV3_CONFIGURATION) && defined(SR_SNMPv3_ADMIN)) */

#endif /* SR_GENERATE_CONFIGURATION */

