/*
 *
 * Copyright (C) 2001-2006 by SNMP Research, Incorporated.
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


#include <stdlib.h>


#include "sr_snmp.h"
#include "sr_utils.h"
#include "pw2key.h"


FILE *fp;

void write_user(
    char *name,
    char *group,
    char *storageType
#ifndef SR_UNSECURABLE
    ,char *authpass
    ,int  authprotflag
#ifndef SR_NO_PRIVACY
    ,char *privpass
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    );

void write_view(
    char *name,
    char *subtree,
    int status);

void write_access(
    SR_INT32 version,
    SR_INT32 securityLevel,
    char *group,
    char *context,
    char *readView,
    char *writeView,
    char *notifyView);

void set_group_configuration(void);
void add_default_configuration(void);
void add_template_configuration(void);
void add_custom_configuration(void);
void add_dr_web_configuration(void);
void add_notify_configuration(void);

int main(int argc, char *argv[]);

void
write_user(
    char *name,
    char *group,
    char *storageType
#ifndef SR_UNSECURABLE
    ,char *authpass
    ,int  authprotflag
#ifndef SR_NO_PRIVACY
    ,char *privpass
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    )
{
#ifndef SR_UNSECURABLE
    char *ap = "-";
#ifndef SR_NO_PRIVACY
    char *pp = "-";
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#ifdef SR_SNMPv3_PACKET
    char *authprot = "usmNoAuthProtocol";
    char *privprot = "usmNoPrivProtocol";
#endif /* SR_SNMPv3_PACKET */

#ifndef SR_UNSECURABLE
      if (strlen(authpass) != 0) {
        ap = authpass;
#ifdef SR_SNMPv3_PACKET
        authprot = "usmHMACMD5AuthProtocol";
#ifdef SR_SHA_HASH_ALGORITHM
        if (authprotflag == SR_PW2KEY_ALGORITHM_SHA) {
            authprot = "usmHMACSHAAuthProtocol";
        }
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_SNMPv3_PACKET */
    }

#ifndef SR_NO_PRIVACY
    if (strlen(privpass) != 0) {
        pp = privpass;
#ifdef SR_SNMPv3_PACKET
        privprot = "usmDESPrivProtocol";
#endif /* SR_SNMPv3_PACKET */
    }
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifdef SR_SNMPv3_PACKET
    fprintf(fp, "usmUserEntry localSnmpID %s %s %s %s -"
#ifndef SR_UNSECURABLE
                " \"%s\""
#ifndef SR_NO_PRIVACY
                " \"%s\""
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
                "\n", name, authprot, privprot, storageType
#ifndef SR_UNSECURABLE
                ,ap
#ifndef SR_NO_PRIVACY
                ,pp
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
            );

    if (group != NULL) {
        fprintf(fp, "vacmSecurityToGroupEntry usm %s %s %s\n",
                name, group, storageType);
    }
#endif /* SR_SNMPv3_PACKET */
}

void
write_view(
    char *name,
    char *subtree,
    int status)
{

    if(status == INCLUDED){
       fprintf(fp, "vacmViewTreeFamilyEntry %s %s - included nonVolatile\n",
                name, subtree);
    } 
    else {
       fprintf(fp, "vacmViewTreeFamilyEntry %s %s - excluded nonVolatile\n",
                name, subtree);
    }
}

void
write_access(
    SR_INT32 version,
    SR_INT32 securityLevel,
    char *group,
    char *context,
    char *readView,
    char *writeView,
    char *notifyView)
{
    char *secModel = NULL;
    char *secLevel = NULL;

    switch (version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            secModel = "snmpv1";
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            secModel = "snmpv2c";
            break;
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            secModel = "usm";
            break;
#endif /* SR_SNMPv3_PACKET */
    }

    switch (securityLevel) {
        case SR_SECURITY_LEVEL_NOAUTH:
            secLevel = "noAuthNoPriv";
            break;
#ifndef SR_UNSECURABLE
        case SR_SECURITY_LEVEL_AUTH:
            secLevel = "authNoPriv";
            break;
#ifndef SR_NO_PRIVACY
        case SR_SECURITY_LEVEL_PRIV:
            secLevel = "authPriv";
            break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
    }

    if (strlen(context) == 0) {
        context = "-";
    }

    if (strlen(writeView) == 0) {
        writeView = "-";
    }
    if (strlen(readView) == 0) {
        readView = "-";
    }
    if (strlen(notifyView) == 0) {
        notifyView = "-";
    }

    fprintf(fp, "vacmAccessEntry %s %s %s %s prefix %s %s %s nonVolatile\n",
            group, context, secModel, secLevel,
            readView, writeView, notifyView);
}

/*
 *  add_default_configuration writes out the community and usmUserEntries
 *  that are standard in our default snmpd.cnf file.
 */


void
add_default_configuration(void)
{
    char emptybuf[2];
#ifndef SR_UNSECURABLE
    char buf1[64]; 
    char buf2[64];
#ifndef SR_NO_PRIVACY
    char buf3[64];
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

   strcpy(emptybuf, "");
#ifndef SR_UNSECURABLE
   strcpy(buf1, "password for AnneXPert");
   strcpy(buf2, "abracadabra");
#ifndef SR_NO_PRIVACY
   strcpy(buf3, "alakazam");
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

    /*
     * Write AGENT config file
     */

    fprintf(fp, "\n# Default Entries\n");
    write_user("Guest", "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               , emptybuf
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               , emptybuf 
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );

    write_user("AnneXPert", "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               ,buf1
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               ,emptybuf
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );


    write_user("CharlieDChief", "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               ,buf2
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               ,buf3
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );

    /* Write community entry */
    fprintf(fp, "snmpCommunityEntry t0000 public public localSnmpID - - nonVolatile\n");

    /* Write Security to Group Entry */
    fprintf(fp, "vacmSecurityToGroupEntry snmpv1 public Anyone nonVolatile\n");
    fprintf(fp, "vacmSecurityToGroupEntry snmpv2c public Anyone nonVolatile\n");

    return;

}

/*
 *  add_template_configuration writes template usmUserEntries
 *  that are required for remote v3 configuration of additional users. 
 */
 
 
void
add_template_configuration(void)
{   
    char emptybuf[2];
#ifndef SR_UNSECURABLE
    char md5buf1[64];
    char md5buf2[64];
#ifdef SR_SHA_HASH_ALGORITHM
    char shabuf1[64];
    char shabuf2[64];
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

   strcpy(emptybuf, "");
#ifndef SR_UNSECURABLE
   strcpy(md5buf1, "TMD5ap4cfu");
#ifndef SR_NO_PRIVACY
   strcpy(md5buf2, "TMD5pp4cfu");
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifdef SR_SHA_HASH_ALGORITHM
#ifndef SR_UNSECURABLE
   strcpy(shabuf1, "TSHAiaao");
#ifndef SR_NO_PRIVACY
   strcpy(shabuf2, "TSHApiapo");
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
#endif /* SR_SHA_HASH_ALGORITHM */
    
    fprintf(fp, "\n\n");
    fprintf(fp, "# Template Entries\n\n");
    write_user("templateMD5", "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               ,md5buf1
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               ,md5buf2
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );

#ifdef SR_SHA_HASH_ALGORITHM
    fprintf(fp, "\n");
    write_user("templateSHA", "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               ,shabuf1
               ,SR_PW2KEY_ALGORITHM_SHA
#ifndef SR_NO_PRIVACY
               ,shabuf2
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );
#endif /* SR_SHA_HASH_ALGORITHM */

     return;
}




void
set_group_configuration(void)
{

    /* The following views and group access entries can be used
     * if either default or custom configuration is used
     */

    /* Write viewTree entries */

    write_view("All", "1.3.6.1", INCLUDED);
    write_view("SystemObjects", "1.3.6.1.2.1.1", INCLUDED);
    write_view("CfgProt", "1.3.6.1", INCLUDED);
    write_view("CfgProt", "snmpCommunityTable", EXCLUDED);
    write_view("CfgProt", "vacmAccessTable", EXCLUDED);
    write_view("CfgProt", "vacmSecurityToGroupTable", EXCLUDED);
    write_view("CfgProt", "usmUserTable", EXCLUDED);
    write_view("CfgProt", "vacmViewTreeFamilyTable", EXCLUDED);
    write_view("internet", "1.3.6.1", INCLUDED);
    write_view("restricted", "1.3.6.1.2.1.1", INCLUDED);
    write_view("restricted", "1.3.6.1.2.1.11", INCLUDED);
    write_view("restricted", "1.3.6.1.6.3.10.2.1", INCLUDED);
    write_view("restricted", "1.3.6.1.6.3.11.2.1", INCLUDED);
    write_view("restricted", "1.3.6.1.6.3.15.1.1", INCLUDED);
 
    /* Write ac entries */

    write_access(SR_SNMPv1_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "Anyone", "", "CfgProt", "", "CfgProt");
    write_access(SR_SNMPv2c_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "Anyone", "", "CfgProt", "", "CfgProt");

#ifndef SR_UNSECURABLE
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "initial", "", "restricted", "", "restricted");
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "initial", "mgr", "restricted", "", "restricted");
#else  /* SR_UNSECURABLE */
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "initial", "", "internet", "internet", "internet");
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_NOAUTH,
                 "initial", "mgr", "internet", "internet", "internet");
#endif /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_AUTH,
                 "initial", "", "internet", "internet", "internet");
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_AUTH,
                 "initial", "mgr", "internet", "internet", "internet");
#ifndef SR_NO_PRIVACY
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_PRIV,
                 "initial", "", "internet", "internet", "internet");
    write_access(SR_SNMPv3_VERSION, SR_SECURITY_LEVEL_PRIV,
                 "initial", "mgr", "internet", "internet", "internet");
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

     return;

}





void 
add_notify_configuration(void)
{
  char answer[256];
  char resp;
  static int index=1;
  int i=0, check=0;
  char ipaddr[16];

  memset(ipaddr, '\0' , 16);

  fprintf(fp, "\n# Notification Entries\n\n");
  fprintf(fp, "snmpNotifyEntry  31 Console trap nonVolatile\n");

    /* add snmpTargetAddrEntries commented out so user can assign own*/


    /* add snmpTargetParamsEntries */

   fprintf(fp, "snmpTargetParamsEntry  v1ExampleParams 0 snmpv1 public noAuthNoPriv \\\n");
   fprintf(fp, "nonVolatile\n");
   fprintf(fp, "snmpTargetParamsEntry  v2cExampleParams 1 snmpv2c public noAuthNoPriv \\\n");
   fprintf(fp, "nonVolatile\n");


  do{
       printf("     1) Add a v1 trap entry\n");
       printf("     2) Add a v2c trap entry\n");
       printf("     3) Exit notification configuration\n\n");
       printf("Select Action: "); 
       fflush(stdout);

       memset(&answer, 0, sizeof(answer));
       fgets(answer, sizeof(answer), stdin);
       resp = answer[0];
       fseek(stdin, 0, SEEK_END);

       switch(resp){
         case '1':
            printf("Enter IP Address: ");
            fflush(stdout);
            memset(&answer, 0, sizeof(answer));
            fgets(answer, sizeof(answer), stdin);
            for(i = 0; i < 16; i++){
                if(answer[i] == '\n') {
                   ipaddr[i] = '\0';
                } else {
                   ipaddr[i] = answer[i];
                }
            }
            fseek(stdin, 0, SEEK_END);
            check = inet_addr(ipaddr);
            if(check == -1){
                 printf("\nWARNING:  IP address provided is formatted incorrectly\n");
                 check = 0;
                 break;
            } 
            fprintf(fp, "snmpTargetAddrEntry %d snmpUDPDomain %s:0.0 100 3 Console \\\n", index, ipaddr);
            fprintf(fp, "v1ExampleParams nonVolatile 255.255.255.255:0.0\n"); 
            fflush(stdin);
            index++;
            break;

         case '2':
            printf("Enter IP Address: ");
            fflush(stdout);
            memset(&answer, 0, sizeof(answer));
            fgets(answer, sizeof(answer), stdin);
            for(i = 0; i < 16; i++){
               if(answer[i] == '\n') {
                  ipaddr[i] = '\0';
               } else {
                  ipaddr[i] = answer[i];
               }
             }
            fseek(stdin, 0, SEEK_END);
            check = inet_addr(ipaddr);
            if(check == -1){
                 printf("\nWARNING: IP address provided is formatted incorrectly\n");
                 check = 0;
                 break;
            } 
            fprintf(fp, "snmpTargetAddrEntry %d snmpUDPDomain %s:0.0 100 3 Console \\\n", index, ipaddr);
            fprintf(fp, "v2cExampleParams nonVolatile 255.255.255.255:0.0\n"); 
            index++;
            break;

         case '3':
            return;         /* exit notification configuration */

         default:
            printf("Invalid selection\n");
     }

      printf("\n");

    } while(1);
}


void
add_custom_configuration(void)
{
    char community[256], dummy[256];
    static int index = 1;
    char buf1[256];
    char buf2[256];
    char resp;
    char emptybuf[2];
    char *ptr;

    memset(community, '\0', sizeof(community));
    memset(buf1, '\0', sizeof(buf1));
    memset(buf2, '\0', sizeof(buf2));
    strcpy(emptybuf, "");


    /* Write access group for v1 community with set privileges entries */
    /* This adds a v1/v2c group that can perform sets                  */

    fprintf(fp, "\n# Custom Configuration\n\n");

    /* access for the group Anyone already added in group configurations */

    write_access(SR_SNMPv1_VERSION, SR_SECURITY_LEVEL_NOAUTH, "SetAnyone", "", "CfgProt", "CfgProt", "CfgProt");
    write_access(SR_SNMPv2c_VERSION, SR_SECURITY_LEVEL_NOAUTH, "SetAnyone", "", "CfgProt", "CfgProt", "CfgProt");


  do{
       fflush(stdin);
       printf("     1) Add v1 community with read only permissions\n");
       printf("     2) Add v1 community with read/write permissions\n");
       printf("     3) Add v2c community with read only permissions\n");
       printf("     4) Add v2c community with read/write permissions\n");
       printf("     5) Add v3 user with minimal security (no auth, no priv)\n");
#ifndef SR_UNSECURABLE
       printf("     6) Add v3 user with moderate security (auth, no priv)\n");
#ifndef SR_NO_PRIVACY
       printf("     7) Add v3 user with maximum security (auth, priv)\n");
       printf("     8) Exit custom user configuration\n\n");
#else /* SR_NO_PRIVACY */
       printf("     7) Exit custom user configuration\n\n");
#endif /* SR_NO_PRIVACY */
#else /* !SR_UNSECURABLE */
       printf("     6) Exit custom user configuration\n\n");
#endif /* !SR_UNSECURABLE */

       printf("Select Action: "); 
       fflush(stdout);

       memset(&dummy, 0, sizeof(dummy));
       fgets(dummy, sizeof(dummy), stdin);
       resp = dummy[0];
       fseek(stdin, 0, SEEK_END);

       memset(community, '\0', sizeof(community));

       switch(resp){
          case '1':
             printf("Enter community string:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if( strlen(community) == 0){
                printf("WARNING: You must enter a valid string\n");
                break;
             } 
             else {
               fprintf(fp, "snmpCommunityEntry t00%d %s %s localSnmpID - - nonVolatile\n", index, community, community);   
               fprintf(fp, "vacmSecurityToGroupEntry snmpv1 %s Anyone nonVolatile\n", community); 
             index++;
             }
             break;

          case '2':
             printf("Enter community string:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                fprintf(fp, "snmpCommunityEntry t00%d %s %s localSnmpID - - nonVolatile\n", index, community, community);   
                fprintf(fp, "vacmSecurityToGroupEntry snmpv1 %s SetAnyone nonVolatile\n", community); 
                index++;
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }
             break;

          case '3':
             printf("WARNING:  Do not use already configured community string\n\n");
             printf("Enter community string:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                fprintf(fp, "snmpCommunityEntry t00%d %s %s localSnmpID - - nonVolatile\n", index, community, community);   
                fprintf(fp, "vacmSecurityToGroupEntry snmpv2c %s Anyone nonVolatile\n", community); 
                index++;
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }

             break;

          case '4':
             printf("WARNING:  Do not use already configured community string\n\n");
             printf("Enter community string:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                fprintf(fp, "snmpCommunityEntry t00%d %s %s localSnmpID - - nonVolatile\n", index, community, community);   
                fprintf(fp, "vacmSecurityToGroupEntry snmpv2c %s SetAnyone nonVolatile\n", community); 
                index++;
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }
             break;

          case '5':
             printf("\nNOTE: This v3 user will have a semi-secure access view. \n");
             printf("      See rfc2574 for details on this posture.\n\n");
             printf("Enter User Name:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                write_user(community, "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               , emptybuf
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               , emptybuf
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }
             break;

#ifndef SR_UNSECURABLE
          case '6':
             printf("\nNOTE: This v3 user will minimum-secure view which allows\n");
             printf("      read and write access to the mib subtree under internet. \n");
             printf("      See rfc2574 for additional information on this posture.\n\n");
             printf("Enter User Name:  ");
             fflush(stdout);
             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
               ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                printf("Enter MD5 Authentication Password:  ");
                fflush(stdout);
                memset(&dummy, 0, sizeof(dummy));
                fgets(dummy, sizeof(dummy), stdin);
                ptr = strstr(dummy, "\n");
                if(ptr != NULL) {
                   ptr[0] = '\0';
                }
                ptr = strstr(dummy, " ");
                if(ptr != NULL) {
                   ptr[0] = '\0';
                }
               memcpy(&buf1, &dummy, sizeof(buf1));
               fseek(stdin, 0, SEEK_END);

               if(strlen(buf1)){
                
                    write_user(community, "initial", "nonVolatile"
               ,buf1
#ifndef SR_UNSECURABLE
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY 
               ,emptybuf
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );
               } 
               else {
                 printf("WARNING: This user requires an authentication password\n");
                 break;
               }
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }
             break;

#ifndef SR_NO_PRIVACY 
          case '7':
             printf("\nNOTE: This v3 user will minimum-secure view which\n");
             printf("      allows read and write access to the mib subtree \n");
             printf("      under internet.  See rfc2574 for additional\n"); 
             printf("      information on this posture.\n\n");
             printf("Enter User Name:  ");
             fflush(stdout);

             memset(&dummy, 0, sizeof(dummy));
             fgets(dummy, sizeof(dummy), stdin);
             ptr = strstr(dummy, "\n");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             ptr = strstr(dummy, " ");
             if(ptr != NULL) {
                ptr[0] = '\0';
             }
             memcpy(&community, &dummy, sizeof(community));
             fseek(stdin, 0, SEEK_END);

             if(strlen(community)){
                printf("Enter MD5 Authentication Password:  ");
                fflush(stdout);

                memset(&dummy, 0, sizeof(dummy));
                fgets(dummy, sizeof(dummy), stdin);
                ptr = strstr(dummy, "\n");
                if(ptr != NULL) {
                   ptr[0] = '\0';
                }
                ptr = strstr(dummy, " ");
                if(ptr != NULL) {
                   ptr[0] = '\0';
                }
                memcpy(&buf1, &dummy, sizeof(buf1));
                fseek(stdin, 0, SEEK_END);

                printf("Enter DES Privacy Password:  ");
                fflush(stdout);
                memset(&dummy, 0, sizeof(dummy));
                fgets(dummy, sizeof(dummy), stdin);
                ptr = strstr(dummy, "\n");
                if(ptr != NULL) {
                   ptr[0] = '\0';
                }
                ptr = strstr(dummy, " ");
                if(ptr != NULL) {
                  ptr[0] = '\0';
                }
                memcpy(&buf2, &dummy, sizeof(buf2));
                fseek(stdin, 0, SEEK_END);

                if( strlen(buf1) && strlen(buf2) ){
                    write_user(community, "initial", "nonVolatile"
#ifndef SR_UNSECURABLE
               ,buf1
               ,SR_PW2KEY_ALGORITHM_MD5
#ifndef SR_NO_PRIVACY
               ,buf2
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */
               );
                 } 
                 else {
                      printf("WARNING: Password error\n");
                      break;
                 }
             } 
             else {
                printf("WARNING: You must enter a valid string\n");
             }
             break;
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

#ifdef SR_UNSECURABLE
          case '6':
             return;   /* exit custom configuration */
#endif /* SR_UNSECURABLE */

#ifdef SR_NO_PRIVACY
#ifndef SR_UNSECURABLE
          case '7':
             return;   /* exit custom configuration */
#endif /* SR_UNSECURABLE */
#endif /* SR_NO_PRIVACY */

          case '8':
             return;   /* exit custom configuration */

          default:
             printf("Invalid selection\n");
             break;
        }

        printf("\n");

      } while(1);

}

FILE *fp;

int
main(int argc, char *argv[])
{

    char resp, dummy[256];

    printf("Writing snmpd.cnf . . .\n");
    fflush(stdout);
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    fp = fopen("snmpd.cnf.domestic", "w");
#else   /* SR_NO_PRIVACY */
    fp = fopen("snmpd.cnf.md5nodes", "w");
#endif  /* SR_NO_PRIVACY */
#else   /* SR_UNSECURABLE */
    fp = fopen("snmpd.cnf.export", "w");
#endif  /* SR_UNSECURABLE */


    set_group_configuration();
    add_template_configuration();

    printf("\nUse the default snmpd.cnf configuration: (Y or N)?  ");
    fflush(stdout);
     memset(&dummy, 0, sizeof(dummy));
     fgets(dummy, sizeof(dummy), stdin);
     resp = dummy[0];
     fseek(stdin, 0, SEEK_END);
  
    if( resp == 'Y' || resp == 'y'){
       add_default_configuration();
    }  
    else {

    printf("\nAn entry for the v1/v2c snmpCommunity public is being\n");
    printf("added for trap configuration\n");

    /* Write community entry */
    fprintf(fp, "snmpCommunityEntry trap1 public public localSnmpID - - nonVolatile\n");

    /* Add SecuritytoGroup Entry for public */
    fprintf(fp, "vacmSecurityToGroupEntry snmpv1 public Anyone nonVolatile\n");
    fprintf(fp, "vacmSecurityToGroupEntry snmpv2c public Anyone nonVolatile\n");

    }

    printf("\nTRAP CONFIGURATION\n\n");
    add_notify_configuration();


    printf("\n\nCUSTOM USER CONFIGURATION\n\n");
    printf("Add custom user configuration: (Y or N)?  ");
    fflush(stdout);
 
     memset(&dummy, 0, sizeof(dummy));
     fgets(dummy, sizeof(dummy), stdin);
     resp = dummy[0];
     fseek(stdin, 0, SEEK_END);
  
    if( resp == 'Y' || resp == 'y'){
       add_custom_configuration();
    }

    printf("\n***** Configuration complete *****\n\n");

    fclose(fp);

    return 1;
}
