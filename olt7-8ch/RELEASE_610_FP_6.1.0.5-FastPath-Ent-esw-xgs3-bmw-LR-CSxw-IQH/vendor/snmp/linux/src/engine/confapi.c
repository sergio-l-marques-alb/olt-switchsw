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
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>

#include <stdlib.h>

#include <string.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "v2clssc.h"
#include "lookup.h"
#include "v2table.h"            /* for SnmpV2Table */
#include "pw2key.h"             /* for SrTextPasswordToLocalizedKey */
#include "snmpid.h"             /* for SrGetSnmpID  */
#include "diag.h"
#include "snmpv3/v3type.h"      /* for snmpTargetAddrEntry_t */
#include "scan.h"
#include "sr_ip.h"		/* for ConvToken_IP_TAddr */
#include "snmpv3/i_usm.h"       /* for usmUserEntryTypeTable */
#include "snmpv3/v3defs.h"      /* for usmUserTable constant macros */
#include "snmpv3/i_tgt.h"       /* for snmpTargetParamsEntryTypeTable */
#include "snmpv3/i_notif.h"     /* for snmpNotifyEntryTypeTable */
#include "snmpv3/i_ctx.h"       /* for vacmContextTable */
#include "coex/i_comm.h"        /* for snmpCommunityEntryTypeTable */
#include "coex/coextype.h"      /* for snmpCommunityEntry_t definition */
#include "coex/coexdefs.h"      /* for snmpCommunityTable constant macros */
SR_FILENAME

extern SnmpV2Table agt_vacmAccessTable;
extern SnmpV2Table agt_vacmSecurityToGroupTable;
extern SnmpV2Table agt_vacmViewTreeFamilyTable;
extern SnmpV2Table agt_usmUserTable;
extern SnmpV2Table agt_snmpTargetAddrTable;
extern SnmpV2Table agt_snmpCommunityTable;
extern SnmpV2Table agt_snmpTargetParamsTable;
extern SnmpV2Table agt_snmpNotifyTable;
extern SnmpV2Table agt_snmpNotifyFilterProfileTable;
extern SnmpV2Table agt_snmpNotifyFilterTable;
extern SnmpV2Table agt_vacmContextTable;
extern int writeConfigFileFlag;

extern void DeletevacmSecurityToGroupEntry(int index);
extern void DeletesnmpTargetAddrEntry(int index);
extern void DeleteusmUserEntry(int index);
extern void DeletevacmViewTreeFamilyEntry(int index);
extern void DeletevacmAccessEntry(int index);


/* This function will access the snmpTargetAddr table. value of FLAG decides what to do
 * 1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 * snmpTargetAddr table is indexed by name only.
 * Upon success 0 is returned, -1 is returned for any error.
 */

int 
ManageTargetAddrEntry(
    const char *Name,
    const char *TDomain,
    char *TAddr,
    int timeout,
    int retries,
    const char *TagList,
    const char *Params,
    int storage,
    char *TMask,
    int MMS,
    SR_UINT32 FLAG)
{
    int index, status = 0;
    snmpTargetAddrEntry_t *ste = NULL;
    OctetString *addr = NULL;
    FNAME("ManageTargetAddrEntry")

   /* error checking  */

   if (Name == NULL || TDomain == NULL || TAddr == NULL)  {
     DPRINTF((APERROR,"%s: Either Name, TDomain or TAddr is NULL\n",Fname));
     return -1;
   }

   if ( (strlen(Name) < 1) || (strlen(Name) > 32)  ){
     DPRINTF((APERROR,"%s: Received bad length for Name\n",Fname));
     return -1;
   }

   if ( (strlen(TAddr) < 1) || (strlen(TAddr) > 255)  ){
     DPRINTF((APERROR,"%s: Received bad length for TAddr\n",Fname));
     return -1;   
   }

   if ( (TagList != NULL) && (strlen(TagList) < 0 || strlen(TagList) > 255) ){
     DPRINTF((APERROR,"%s: Received bad length for TagList\n",Fname));
     return -1;
   }

   if ( (Params != NULL) && (strlen(Params) < 1 || strlen(Params) > 32) ){
     DPRINTF((APERROR,"%s: Received bad length for Params\n",Fname));
     return -1;
   }

   if ( (TMask != NULL) && (strlen(TMask) < 0 || strlen(TMask) > 255) ){
     DPRINTF((APERROR,"%s: Received bad length for TMask\n",Fname));
     return -1;
   }

   if (FLAG < 1 || FLAG > 3 )  {
     DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
     return -1;
   }

  if (storage < 1 || storage > 5 )  {
     DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
     return -1;
  }

  if (timeout < 0 || timeout > 2147483647) {
     DPRINTF((APERROR,"%s: Received bad value for timeout\n",Fname));
     return -1;
  }

  if ( retries < 0 || retries > 255) {
     DPRINTF((APERROR,"%s: Received bad value for RetryCount\n",Fname));
     return -1;
  }

  if (MMS < 484 || MMS > 65535)  {
     DPRINTF((APERROR,"%s: Received bad value for MMS (max message size)\n",Fname));
     return -1; 
  }

   /* set up for original tree search */

    agt_snmpTargetAddrTable.tip[0].value.octet_val = MakeOctetStringFromText(Name);
    if (agt_snmpTargetAddrTable.tip[0].value.octet_val == NULL)  {
      return -1;
    }

    index = SearchTable(&agt_snmpTargetAddrTable, EXACT);

   if (FLAG == 1  || FLAG == 2)  {

       if (FLAG == 1)  {
           if (index != -1) {
               DPRINTF((APERROR,
                 "%s: Cannot create row in snmpTargetAddrTable, row already exists\n",Fname));
               goto targetfail;
           }

           index = NewTableEntry(&agt_snmpTargetAddrTable);
           if (index == -1) {
               DPRINTF((APERROR,
                   "%s: Cannot create new row in snmpTargetAddrTable\n",Fname));
               goto targetfail;
           }
       } else {		/* FLAG = 2  */
           if (index == -1) {
               DPRINTF((APERROR,"%s: No such row to modify in snmpTargetAddrTable\n",Fname));
               goto targetfail;
           }
       } 

       ste = (snmpTargetAddrEntry_t *)agt_snmpTargetAddrTable.tp[index];

       if(ste->snmpTargetAddrTDomain != NULL) {
       FreeOID(ste->snmpTargetAddrTDomain);
       }
       if ( (strcmp(TDomain,"-") == 0) || (strcmp(TDomain,"NULL") == 0) ) {
          ste->snmpTargetAddrTDomain = MakeOIDFromDot("0.0");
       } else {
          ste->snmpTargetAddrTDomain = MakeOIDFromDot(TDomain);
       }

       if (ste->snmpTargetAddrTAddress != NULL) {
       FreeOctetString(ste->snmpTargetAddrTAddress);
       }
       if ( (strcmp(TAddr,"-") == 0) || (strcmp(TAddr,"NULL") == 0) ) {
          ste->snmpTargetAddrTAddress = MakeOctetString(NULL,0);
       } else {
/* LVL7 */
	  if(strcmp(TDomain, "transportDomainUdpIpv6") == 0)
              status = ConvToken_IP6_TAddr(PARSER_CONVERT_FROM_TOKEN, &TAddr, &addr);
	  else
/* LVL7 */
          status = ConvToken_IP_TAddr(PARSER_CONVERT_FROM_TOKEN, &TAddr, &addr);
          if (status == -1) {
             DPRINTF((APERROR,"%s: ConvToken_IP_TAddr failed\n"));
             goto targetfail;
          }  else {
             ste->snmpTargetAddrTAddress = CloneOctetString(addr); 
             FreeOctetString(addr);
             addr = NULL;
          }
       }			/* end of else, non null TAddr  */

       if (ste->snmpTargetAddrTagList != NULL) {
        FreeOctetString(ste->snmpTargetAddrTagList);
       }
       if ( (TagList == NULL) ||(strcmp(TagList,"-") == 0)
                       || (strcmp(TagList,"NULL") == 0) ) {
             ste->snmpTargetAddrTagList = MakeOctetString(NULL,0);
       } else {
           ste->snmpTargetAddrTagList = MakeOctetStringFromText(TagList);
       }


       if (ste->snmpTargetAddrParams != NULL) {
        FreeOctetString(ste->snmpTargetAddrParams);
       }
       if ( (Params == NULL) ||(strcmp(Params,"-") == 0)
                        || (strcmp(Params,"NULL") == 0) ) {
             ste->snmpTargetAddrParams = MakeOctetString(NULL,0);
       } else {
           ste->snmpTargetAddrParams = MakeOctetStringFromText(Params);
       }

       if (ste->snmpTargetAddrTMask != NULL) {
        FreeOctetString(ste->snmpTargetAddrTMask);
       }
       if ( (TMask == NULL) || (strcmp(TMask,"-") == 0)
                          || (strcmp(TMask,"NULL") == 0) ) {
            ste->snmpTargetAddrTMask = MakeOctetString(NULL,0);
       } else {
/* LVL7 */
	    if(strcmp(TDomain, "transportDomainUdpIpv6") == 0)
               status = ConvToken_IP6_TAddr(PARSER_CONVERT_FROM_TOKEN, &TMask, &addr);
	    else
/* LVL7 */
            status = ConvToken_IP_TAddr(PARSER_CONVERT_FROM_TOKEN, &TMask, &addr);
            if (status == -1) {
                DPRINTF((APERROR,"%s: ConvToken_IP_TAddr failed\n",Fname));
                goto targetfail;
            }  else {
                ste->snmpTargetAddrTMask = CloneOctetString(addr); 
                FreeOctetString(addr);
                addr = NULL;
            }
       }			/* end else, TMask not NULL */

       memset(ste->valid, 0xff, sizeof(ste->valid));

      /*   if any of the items that needed to be malloced failed, go to targetfail */
       if ((ste->snmpTargetAddrTDomain == NULL)   || (ste->snmpTargetAddrTAddress == NULL)
          || (ste->snmpTargetAddrTagList == NULL) || (ste->snmpTargetAddrParams == NULL)
                                                  || (ste->snmpTargetAddrTMask == NULL) )  {
             goto targetfail;
       } 

       ste->snmpTargetAddrTimeout = timeout;
       ste->snmpTargetAddrRetryCount = retries;
       ste->snmpTargetAddrMMS = MMS;
       ste->snmpTargetAddrStorageType = storage;
       ste->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_active;
       goto targetdone;
   }				/* end of if FLAG = 1 or 2 */

   else {			/* FLAG == 3  */   
       if (index == -1) {
           DPRINTF((APERROR,"%s: No such row to delete in snmpTargetAddrTable\n",Fname));
           goto targetfail;
       }    
       DeletesnmpTargetAddrEntry(index);
       goto targetdone;
   }                             /* end of if FLAG == 3 */

 targetfail:		/* free malloced items */
         FreeOctetString(agt_snmpTargetAddrTable.tip[0].value.octet_val);
         if (addr != NULL) {
            FreeOctetString(addr);
         }
         if (ste != NULL)  {		/* if row created, but not finished */
            DeletesnmpTargetAddrEntry(index);
         }
         return -1;

 targetdone:		/* free malloced items and force config file rewrite */
       FreeOctetString(agt_snmpTargetAddrTable.tip[0].value.octet_val);
       writeConfigFileFlag = TRUE;
       return 0;

}				/* end of function ManageTargetAddrEntry */

/* This function will access the usmUserTable table. value of FLAG decides what to do
 * 1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 * usmUserTable table is indexed by snmpEngineID then Name.
 * Upon success 0 is returned, -1 is returned for any error.
 */

int
ManageUserEntry(
    OctetString *snmpEngineID,
    const char *Name,
    const char *authprot,
    const char *privprot,
    int storage,
    const char *target,
#ifndef SR_UNSECURABLE
    const char *authpass,
#endif  /*  SR_UNSECURABLE  */
#ifndef SR_NO_PRIVACY
    const char *privpass,
#endif  /* SR_NO_PRIVACY */
    SR_UINT32 FLAG)
{
    int index, free_flag = 0;
    usmUserEntry_t *uue = NULL;
    SR_INT32 auth_algorithm = 0;
#if (defined(SR_3DES) || defined(SR_AES))
    OctetString *key1 = NULL, *key2 = NULL;
    SR_INT32 remainderLen;
    unsigned char *tmp_ptr;
    int privProtocol;
#endif	/* (defined(SR_3DES) || defined(SR_AES)) */
    FNAME("ManageUserEntry")
 
   /* error check incoming args */

     if (FLAG == 3)  {		/* if deleting user, only need Name, all other args may be NULL */
       if (Name == NULL)  {
          DPRINTF((APERROR,"%s: Name is NULL\n",Fname));
          return -1;
       }
     }  else {			/* FLAG == 1 or 2  */
      if (Name == NULL || authprot == NULL || privprot == NULL)  {
        DPRINTF((APERROR,"%s: One of Name, authprot or privport is NULL\n",Fname));
        return -1;
      }
     }

    if ( (target != NULL) && (strlen(target) < 0 || strlen(target) > 255 ) )  {
        DPRINTF((APERROR,"%s: Received bad length for target\n",Fname));
        return -1;
    }

    if (FLAG < 1 || FLAG > 3 )  {
        DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
        return -1;
    }

    if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
        return -1;
    }

  /* build the search structure */

    if ( (snmpEngineID == NULL) || (strcmp((char*)snmpEngineID->octet_ptr,"localSnmpID") == 0) 
                                || (strcmp((char *)snmpEngineID->octet_ptr,"NULL") == 0)
                                || (strcmp((char *)snmpEngineID->octet_ptr,"-") == 0)  )   {
       snmpEngineID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);  /* LVL7*/           /* get default agt ID */
       if (snmpEngineID == NULL)  {	                   /* error with SrGetSnmpID */
          DPRINTF((APERROR,"%s: Cannot get local snmpdID\n",Fname));
          return -1;
       }
       free_flag = 1;		/* so I know to free the malloced snmpEngineID */
    }

       agt_usmUserTable.tip[0].value.octet_val = CloneOctetString(snmpEngineID);
       if (agt_usmUserTable.tip[0].value.octet_val == NULL)  {
           if (free_flag == 1)  {
              FreeOctetString(snmpEngineID);
           }
           return -1;
       }
       agt_usmUserTable.tip[1].value.octet_val = MakeOctetStringFromText(Name);
       if (agt_usmUserTable.tip[1].value.octet_val == NULL)  {
           FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
           if (free_flag == 1)  {
               FreeOctetString(snmpEngineID);
           }
           return -1;
       }

    index = SearchTable(&agt_usmUserTable, EXACT);

    if ( FLAG == 1 || FLAG == 2 )   {

        if (FLAG == 1)  {
            if (index != -1) {
                DPRINTF((APERROR,
                  "%s: Cannot create row in usmUserTable, row already exists\n",Fname));
                goto userfail;
            }

            index = NewTableEntry(&agt_usmUserTable);
                if (index == -1) {
                    DPRINTF((APERROR,
                        "%s: Cannot create new row in usmUserTable\n",Fname));
                    goto userfail;
                }
         } else {		/* FLAG == 2  */
             if (index == -1) {
                DPRINTF((APERROR,"%s: No such row to modify in usmUserTable\n",Fname));
                goto userfail;
             }
         }

        uue = (usmUserEntry_t *)agt_usmUserTable.tp[index];

        if (uue->usmUserSecurityName != NULL) {
         FreeOctetString(uue->usmUserSecurityName);
        }
        uue->usmUserSecurityName = MakeOctetStringFromText(Name);

        if (uue->usmUserCloneFrom != NULL) {
         FreeOID(uue->usmUserCloneFrom);
        } 
        uue->usmUserCloneFrom = MakeOIDFromDot("0.0");

        if (uue->usmUserAuthKeyChange != NULL) {
         FreeOctetString(uue->usmUserAuthKeyChange);
        } 
        uue->usmUserAuthKeyChange = MakeOctetString(NULL, 0);

        if (uue->usmUserOwnAuthKeyChange != NULL) {
         FreeOctetString(uue->usmUserOwnAuthKeyChange);
        }
        uue->usmUserOwnAuthKeyChange = MakeOctetString(NULL, 0);

        if (uue->usmUserPrivKeyChange != NULL) {
         FreeOctetString(uue->usmUserPrivKeyChange);
        } 
        uue->usmUserPrivKeyChange = MakeOctetString(NULL, 0);

        if (uue->usmUserOwnPrivKeyChange != NULL) { 
         FreeOctetString(uue->usmUserOwnPrivKeyChange);
        } 
        uue->usmUserOwnPrivKeyChange = MakeOctetString(NULL, 0);

        if (uue->usmUserPublic != NULL) {
         FreeOctetString(uue->usmUserPublic);
        }    
        uue->usmUserPublic = MakeOctetString(NULL, 0);

        if (uue->usmTargetTag != NULL) {
         FreeOctetString(uue->usmTargetTag);
        }
        if(target == NULL || (strcmp(target, "-") == 0) || (strcmp(target, "NULL") == 0) ){
             uue->usmTargetTag = MakeOctetString(NULL,0);
        } else {
             uue->usmTargetTag = MakeOctetStringFromText(target);
        }
        memset(uue->valid, 0xff, sizeof(uue->valid));

        /* check all malloced items, if one failed, go to userfail  */
        if ( (uue->usmUserEngineID == NULL) || (uue->usmUserName == NULL)
             || (uue->usmUserSecurityName == NULL) || (uue->usmUserCloneFrom == NULL)
             || (uue->usmUserAuthKeyChange == NULL) || (uue->usmUserOwnAuthKeyChange == NULL)
             || (uue->usmUserPrivKeyChange == NULL) || (uue->usmUserOwnPrivKeyChange == NULL)
             || (uue->usmUserPublic == NULL) || (uue->usmTargetTag == NULL) ) {
                     goto userfail;
        }

        uue->usmUserStorageType = storage;
        uue->usmUserStatus = D_usmUserStatus_active;

        if (uue->usmUserAuthProtocol != NULL) {
          FreeOID(uue->usmUserAuthProtocol);
        }

        if(strcmp(authprot,"usmNoAuthProtocol") == 0)  {
            auth_algorithm = 0;
            uue->usmUserAuthProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.1.1");
#ifndef SR_UNSECURABLE
            if (uue->auth_secret != NULL) {
                FreeOctetString(uue->auth_secret);
            }
            uue->auth_secret = MakeOctetString(NULL, 0);
#endif       /* SR_UNSECURABLE */
        } 
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
        else if (strcmp(authprot,"usmHMACMD5AuthProtocol") == 0)  {
           auth_algorithm = SR_PW2KEY_ALGORITHM_MD5;
           if (authpass == NULL)  {
              DPRINTF((APERROR,"%s: Received NULL for authority password\n",Fname));
              goto userfail;
           }
           uue->usmUserAuthProtocol =  MakeOIDFromDot("1.3.6.1.6.3.10.1.1.2");
           if (uue->auth_secret != NULL) {
               FreeOctetString(uue->auth_secret);
           }
           uue->auth_secret = SrTextPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_MD5,
                                               uue->usmUserEngineID,authpass);
        }
#endif                /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
        else if (strcmp(authprot,"usmHMACSHAAuthProtocol") == 0)  {
            auth_algorithm = SR_PW2KEY_ALGORITHM_SHA;
            if (authpass == NULL)  {
                  DPRINTF((APERROR,"%s: Received NULL for authority password\n",Fname));
                  goto userfail;
            }
            uue->usmUserAuthProtocol =  MakeOIDFromDot("1.3.6.1.6.3.10.1.1.3");
            if (uue->auth_secret != NULL) {
                FreeOctetString(uue->auth_secret);
            }
            uue->auth_secret = SrTextPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_SHA,
                                               uue->usmUserEngineID,authpass);
        }
#endif   /* SR_SHA_HASH_ALGORITHM */
#endif  /* SR_UNSECURABLE */
        else {
             DPRINTF((APERROR,"%s: Received bad value for authprot\n",Fname));
             goto userfail;
        }
        if (uue->usmUserAuthProtocol == NULL)  {
             goto userfail;
        }

        if (uue->usmUserPrivProtocol != NULL) {
            FreeOID(uue->usmUserPrivProtocol);
        }

        if(strcmp(privprot,"usmNoPrivProtocol") == 0)  {
            uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.2.1");
#ifndef SR_NO_PRIVACY
         if (uue->priv_secret != NULL) {
            FreeOctetString(uue->priv_secret);
           }
            uue->priv_secret = MakeOctetString(NULL, 0);
#endif /* SR_NO_PRIVACY */
        }
#ifndef SR_NO_PRIVACY
        else if (strcmp(privprot,"usmDESPrivProtocol") == 0) {
            if (privpass == NULL)  {
                DPRINTF((APERROR,"%s: Received NULL for privacy password\n",Fname));
                goto userfail;
            }
            uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.2.2");
            if (uue->priv_secret != NULL) {
               FreeOctetString(uue->priv_secret);
            }
            if (auth_algorithm == 0) {   /* usmNoAuthProtocol */
                DPRINTF((APERROR,"%s: usmNoAuthProtocol not valid with privacy\n", Fname));
                goto userfail;
            }
            else {    /* MD5 or SHA authentication algorithm */
               uue->priv_secret =  SrTextPasswordToLocalizedKey(auth_algorithm,
                                                uue->usmUserEngineID,privpass);
            }
        }
#ifdef SR_3DES
        else if (strcmp(privprot,"usm3DESPrivProtocol") == 0) {
            if (privpass == NULL)  {
                DPRINTF((APERROR,"%s: Received NULL for privacy password\n",Fname));
                goto userfail;
            }
            uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.4.1.14832.1.1");

            if (auth_algorithm == 0) {   /* usmNoAuthProtocol */
                DPRINTF((APERROR,"%s: usmNoAuthProtocol not valid with privacy\n", Fname));
                goto userfail;
            }
            if (uue->priv_secret != NULL) {
               FreeOctetString(uue->priv_secret);
            }
            uue->priv_secret = MakeOctetString(NULL, SR_3DES_KEY_LENGTH);
            if (uue->priv_secret != NULL) {
                key1 = SrTextPasswordToLocalizedKey(auth_algorithm, 
                                              uue->usmUserEngineID, privpass);
                if (key1 != NULL) {
                    key2 = SrOctetStringPasswordToLocalizedKey(auth_algorithm,
                                                           uue->usmUserEngineID,
                                                           key1);
                    if (key2 != NULL) {
                        memcpy(uue->priv_secret->octet_ptr,
                               key1->octet_ptr, key1->length);
                        remainderLen = SR_3DES_KEY_LENGTH - key1->length;
                        tmp_ptr = uue->priv_secret->octet_ptr + key1->length;
                        memcpy(tmp_ptr, key2->octet_ptr, remainderLen);
                        FreeOctetString(key1);
                        FreeOctetString(key2);
                    } else {
                        FreeOctetString(key1);
                        FreeOctetString(uue->priv_secret);
                        uue->priv_secret = NULL;
                        DPRINTF((APERROR,"%s: failed to create key\n", Fname));
                        goto userfail;
                    }
                } else {
                    FreeOctetString(uue->priv_secret);
                    uue->priv_secret = NULL;
                    DPRINTF((APERROR,"%s: failed to create key\n", Fname));
                    goto userfail;
                }
            } else {
                DPRINTF((APERROR,"%s: malloc failed\n",Fname));
                goto userfail;
            } 
        }  
#endif /* SR_3DES */
#ifdef SR_AES
        else if ((strcmp(privprot,"usmAesCfb128Protocol") == 0) ||
                 (strcmp(privprot,"usmAESCfb128PrivProtocol") == 0) ||
                 (strcmp(privprot,"usmAESCfb192PrivProtocol") == 0) ||
                 (strcmp(privprot,"usmAESCfb256PrivProtocol") == 0)) {

            if (privpass == NULL)  {
                DPRINTF((APERROR,"%s: Received NULL for privacy password\n",Fname));
                goto userfail;
            }

            if (auth_algorithm == 0) {   /* usmNoAuthProtocol */
                DPRINTF((APERROR,"%s: usmNoAuthProtocol not valid with privacy\n", Fname));
                goto userfail;
            }

            if ((strcmp(privprot, "usmAesCfb128Protocol") == 0) ||
                (strcmp(privprot,"usmAESCfb128PrivProtocol") == 0)) {
               /* protocol defined in snmpUsmAesMIB, rfc3826 
                * Note that esoConsortiumMIB object will be converted to
                * snmpUsmAesMIB object
                */
               uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.2.4");
               privProtocol = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
            } else if (strcmp(privprot,"usmAESCfb192PrivProtocol") == 0) {
               /* protocol defined in esoConsortiumMIB */
               uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.4.1.14832.1.3");
               privProtocol = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
            } else {   /* usmAESCfb256PrivProtocol */
               /* protocol defined in esoConsortiumMIB */
               uue->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.4.1.14832.1.4");
               privProtocol = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
            }
 
            if (uue->priv_secret != NULL) {
               FreeOctetString(uue->priv_secret);
            }
            uue->priv_secret = MakeOctetString(NULL, SR_AES_KEY_LENGTH);
            if (uue->priv_secret != NULL) {
                key1 = SrTextPasswordToLocalizedKey(auth_algorithm, 
                                              uue->usmUserEngineID, privpass);
                if (key1 != NULL) {
                    key2 = SrOctetStringPasswordToLocalizedKey(auth_algorithm,
                                                          uue->usmUserEngineID,
                                                          key1);
                    if (key2 != NULL) {
                        memcpy(uue->priv_secret->octet_ptr,
                               key1->octet_ptr, key1->length);
                        remainderLen = SR_AES_KEY_LENGTH - key1->length;
                        tmp_ptr = uue->priv_secret->octet_ptr + key1->length;
                        memcpy(tmp_ptr, key2->octet_ptr, remainderLen);
                        FreeOctetString(key1);
                        FreeOctetString(key2);
              
                        /*
                         * Adjust length for AES128 and AES192
                         */
                        if (privProtocol == SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
                            uue->priv_secret->length = SR_AES_128_KEY_LENGTH;

                        } 
                        if (privProtocol == SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
                            uue->priv_secret->length = SR_AES_192_KEY_LENGTH;
                        }
                    } else {
                        FreeOctetString(key1);
                        FreeOctetString(uue->priv_secret);
                        uue->priv_secret = NULL;
                        DPRINTF((APERROR,"%s: failed to create key\n", Fname));
                        goto userfail;
                    }
                } else {
                    FreeOctetString(uue->priv_secret);
                    uue->priv_secret = NULL;
                    DPRINTF((APERROR,"%s: failed to create key\n", Fname));
                    goto userfail;
                }
            } else {
                DPRINTF((APERROR,"%s: malloc failed\n", Fname));
                goto userfail;
            }
        }  
#endif /* SR_AES */
#endif /* SR_NO_PRIVACY */
        else {
            DPRINTF((APERROR,"%s: Received bad value for privprot\n",Fname));
            goto userfail;
        }
        if (uue->usmUserPrivProtocol == NULL)  {
            goto userfail; 
        }

        goto userdone;
    }			/* end of if FLAG = 1 or 2 */
    else  {			/* FLAG == 3 */

        if (index == -1) {
            DPRINTF((APERROR,"%s: No such row to delete in usmUserTable\n",Fname));
            goto userfail;
         }

         FreeEntries(usmUserEntryTypeTable, agt_usmUserTable.tp[index]);
         free(agt_usmUserTable.tp[index]);
         RemoveTableEntry(&agt_usmUserTable, index);
         goto userdone;
    }				/* end of if FLAG == 3 */

 userfail:		/* free malloced items */
        FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
        FreeOctetString(agt_usmUserTable.tip[1].value.octet_val);
        if (free_flag == 1)  {			/* if I malloced this value */
           FreeOctetString(snmpEngineID);
        }
        if (uue != NULL)  {			/* if table entry added, but error occured */
           FreeEntries(usmUserEntryTypeTable, agt_usmUserTable.tp[index]);
           free(agt_usmUserTable.tp[index]);
           RemoveTableEntry(&agt_usmUserTable, index);
        }
        return -1;

 userdone:		/* free malloced items and force config file rewrite */
       FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
       FreeOctetString(agt_usmUserTable.tip[1].value.octet_val);
       if (free_flag == 1)  {                  /* if I malloced this value */
           FreeOctetString(snmpEngineID);
       }
       writeConfigFileFlag = TRUE;
       return 0;

}		/* end of function ManageUserEntry */

/* This function will access the vacmViewTreeFamilyTable table. value of FLAG decides what to do
 * 1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 * vacmViewTreeFamilyTable table is indexed by viewName then viewTree.
 * Upon success 0 is returned, -1 is returned for any error.
 */

int
ManageViewTreeFamilyEntry(
    const char *viewName,
    const char *viewTree,
    const char *viewMask,
    int type,
    int storage,
    SR_UINT32 FLAG)
{
    int index;
    vacmViewTreeFamilyEntry_t *vte = NULL;
    FNAME("ManageViewTreeFamilyEntry")

   /* error checking  */

    if (viewName == NULL || viewTree == NULL)  {
       DPRINTF((APERROR,"%s: Either viewName or viewTree is NULL\n",Fname));
       return -1;
    }

    if (strlen(viewName) < 1 || strlen(viewName) > 32)  {
       DPRINTF((APERROR,"%s: Received bad length for viewName\n",Fname));
       return -1;
    }      

    if ( (viewMask != NULL) && (strlen(viewMask) < 0 || strlen(viewMask) > 16) )  {
       DPRINTF((APERROR,"%s: Received bad length for viewMask\n",Fname));
       return -1;
    }

    if (FLAG < 1 || FLAG > 3 )  {
       DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
       return -1;
    }

    if (storage < 1 || storage > 5 )  {
       DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
       return -1;
    }

    if (type < 1 || type > 2 )  {		/* 1 - include, 2 - exclude */
       DPRINTF((APERROR,"%s: Received bad value for type\n",Fname));
       return -1;
    }

   /* set up for original tree search */

    agt_vacmViewTreeFamilyTable.tip[0].value.octet_val = MakeOctetStringFromText(viewName);
    if (agt_vacmViewTreeFamilyTable.tip[0].value.octet_val == NULL)  {
        return -1;
    }     
    agt_vacmViewTreeFamilyTable.tip[1].value.oid_val = MakeOIDFromDot(viewTree);
    if (agt_vacmViewTreeFamilyTable.tip[1].value.oid_val == NULL)  {
        FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
        return -1;
    }

    index = SearchTable(&agt_vacmViewTreeFamilyTable, EXACT);

    if ( FLAG == 1 || FLAG == 2 )  {

       if (FLAG == 1 )  {
          if (index != -1) {
             DPRINTF((APERROR,
             "%s: Cannot create row in vacmViewTreeFamilyTable, row already exists\n",Fname));
             goto viewtreefail;
          }

          index = NewTableEntry(&agt_vacmViewTreeFamilyTable);
          if (index == -1) {
               DPRINTF((APERROR,
                 "%s: Cannot create new row in vacmViewTreeFamilyTable\n",Fname));
               goto viewtreefail;
          }
       }  else {		/* FLAG == 2  */
             if (index == -1) {
                 DPRINTF((APERROR,"%s: No such row to modify in vacmViewTreeFamilyTable\n",Fname));
                 goto viewtreefail;
             }
       }

     vte = (vacmViewTreeFamilyEntry_t *)agt_vacmViewTreeFamilyTable.tp[index];

     if (vte->vacmViewTreeFamilyMask != NULL) {
       FreeOctetString(vte->vacmViewTreeFamilyMask);
     }
     if ( (viewMask == NULL) || (strcmp(viewMask,"-") == 0)
                             || (strcmp(viewMask,"NULL") == 0) ) {
         vte->vacmViewTreeFamilyMask = MakeOctetString(NULL, 0);
     } else {
         vte->vacmViewTreeFamilyMask = MakeOctetStringFromHex(viewMask);
     }
     if (vte->vacmViewTreeFamilyMask == NULL)  {
          goto viewtreefail;
     }

     vte->vacmViewTreeFamilyType = type;
     vte->vacmViewTreeFamilyStorageType = storage;
     vte->vacmViewTreeFamilyStatus = D_vacmViewTreeFamilyStatus_active;
     memset(vte->valid, 0xff, sizeof(vte->valid));
     goto viewtreedone;
   }				/* end of if FLAG = 1 or 2 */
   else {			/* FLAG == 3  */
    
      if (index == -1) {
         DPRINTF((APERROR,
              "%s: No such row to delete in vacmViewTreeFamilyTable\n",Fname));
         goto viewtreefail;
      }    
      DeletevacmViewTreeFamilyEntry(index);
      goto viewtreedone;
   }                             /* end of if FLAG == 3 */

 viewtreefail:		/* free malloced items */
         FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
         FreeOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);
         if (vte != NULL)  {               /* if table entry created, but error occured */
            DeletevacmViewTreeFamilyEntry(index);
         }
         return -1;

 viewtreedone:		/* free malloced itesm and force config file rewrite */
       FreeOctetString(agt_vacmViewTreeFamilyTable.tip[0].value.octet_val);
       FreeOID(agt_vacmViewTreeFamilyTable.tip[1].value.oid_val);
       writeConfigFileFlag = TRUE;
       return 0;

}				/* end of function ManageViewTreeFamilyEntry */

/* 
 * This function will access the vacmSecurityToGroupTable table. 
 * value of FLAG decides what to do
 * 1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 * vacmSecurityToGroupTable table is indexed by SecurityModel then SecurityName.
 * Upon success 0 is returned, -1 is returned for any error.
 */

int
ManageSecurityToGroupEntry(
    const char *SecurityModel,
    const char *SecurityName,
    const char *GroupName,
    int storage,
    SR_UINT32 FLAG)
{

    int index;
    vacmSecurityToGroupEntry_t *vstge = NULL;
    FNAME("ManageSecurityToGroupEntry")

   /* error check incoming args */

    if (SecurityModel == NULL || SecurityName == NULL || GroupName == NULL)  {
        DPRINTF((APERROR,"%s: SecurityModel, SecurityName or GroupName is NULL\n",Fname));
        return -1;
    }

    if (strlen(SecurityName) < 1 || strlen(SecurityName) > 32)  {
        DPRINTF((APERROR,"%s: Received bad length for SecurityName\n",Fname));
        return -1;
    } 

    if (strlen(GroupName) < 1 || strlen(GroupName) > 32)  {
        DPRINTF((APERROR,"%s: Received bad length for GroupName\n",Fname));
        return -1;
    }

    if (FLAG < 1 || FLAG > 3 )  {
        DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
        return -1;
    }

    if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
        return -1;
    }

  /* build the search struct */

    if (strcmp(SecurityModel,"snmpv1") == 0 )  {
        agt_vacmSecurityToGroupTable.tip[0].value.uint_val = 1;
    }
    else if (strcmp(SecurityModel,"snmpv2c") == 0 )  {
        agt_vacmSecurityToGroupTable.tip[0].value.uint_val = 2;
    }
    else if (strcmp(SecurityModel,"usm") == 0 )  {
         agt_vacmSecurityToGroupTable.tip[0].value.uint_val = 3;
    }
    else  {
         DPRINTF((APERROR,"%s: Received bad value for SecruityModel \n",Fname));
         return -1;
    }

    agt_vacmSecurityToGroupTable.tip[1].value.octet_val =
        MakeOctetStringFromText(SecurityName);
    if (agt_vacmSecurityToGroupTable.tip[1].value.octet_val == NULL)  {
        return -1;
    }

    index = SearchTable(&agt_vacmSecurityToGroupTable, EXACT);

    if  ( FLAG == 1 || FLAG == 2 )   {

       if (FLAG == 1 )  {
           if (index != -1) {
              DPRINTF((APERROR,
                "%s: Cannot create row in vacmSecurityToGroupTable, row already exists\n",Fname));
              goto sectogroupfail;
           }

           index = NewTableEntry(&agt_vacmSecurityToGroupTable);
           if (index == -1) {
               DPRINTF((APERROR,"Could not create vacmSecurityToGroupEntry for %s\n",
                            SecurityName));
               goto sectogroupfail;
           } 
       }  else {  /* FLAG == 2 */
             if (index == -1) {
                 DPRINTF((APERROR,"%s: No such row to modify in vacmSecurityToGroupTable\n",Fname));
                 goto sectogroupfail;
             }
       }

       vstge = (vacmSecurityToGroupEntry_t *)agt_vacmSecurityToGroupTable.tp[index];

       if (vstge->vacmGroupName != NULL) {
       FreeOctetString(vstge->vacmGroupName);
       }
       vstge->vacmGroupName = MakeOctetStringFromText(GroupName);
       if (vstge->vacmGroupName == NULL)  {
           goto sectogroupfail;
       }

       vstge->vacmSecurityToGroupStorageType = storage;
       vstge->vacmSecurityToGroupStatus = 1;
       memset(vstge->valid, 0xff, sizeof(vstge->valid));
       goto sectogroupdone;
    }			/* end of if FLAG = 1 or 2 */
    else  {			/* FLAG == 3 */

        if (index == -1) {
            DPRINTF((APERROR,"%s: No such row to delete in vacmSecurityToGroupTable\n",Fname));
            goto sectogroupfail;
        }

        DeletevacmSecurityToGroupEntry(index);
        goto sectogroupdone;
    }    		/* end of else FLAG == 3  */
        
 sectogroupfail:		/* free malloced items */
       FreeOctetString(agt_vacmSecurityToGroupTable.tip[1].value.octet_val);
       if (vstge != NULL)  {		/* table entry created, but error occured */
          DeletevacmSecurityToGroupEntry(index);
       }
       return -1;

 sectogroupdone:		/* free malloced items and force config file rewrite */
        FreeOctetString(agt_vacmSecurityToGroupTable.tip[1].value.octet_val);
        writeConfigFileFlag = TRUE;
        return 0;

}		/* end of function ManageSecurityToGroup */


/* This function will access the vacmAccessTable table. value of FLAG decides what to do
 * 1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 * vacmAccessTable table is indexed by groupName then ctxName then
 * securityModel then securityLevel.
 * Upon success 0 is returned, -1 is returned for any error.
 */

int
ManageAccessEntry(
    const char *groupName,
    const char *ctxName,
    const char *securityModel,
    const char *securityLevel,
    const char *readView,
    const char *writeView,
    const char *notifyView,
    int storage,
    SR_UINT32 FLAG)
{
    int index = 0;
    vacmAccessEntry_t *ace = NULL;
    FNAME("ManageAccessEntry")

   /* error check incoming args */

    if (groupName == NULL || securityLevel == NULL || securityModel == NULL)  {
        DPRINTF((APERROR,"%s: groupName, securityModel or securityLevel is NULL\n",Fname));
        return -1;
    }

    if (strlen(groupName) < 1 || strlen(groupName) > 32 ) {
        DPRINTF((APERROR,"%s: Bad length for groupName\n",Fname));
        return -1;
    }

    if ( (readView != NULL) && (strlen(readView) < 0 || strlen(readView) > 32) )  {
        DPRINTF((APERROR,"%s: Bad length for readView\n",Fname));
        return -1;
    }

    if ( (writeView != NULL) && (strlen(writeView) < 0 || strlen(writeView) > 32) )  {
        DPRINTF((APERROR,"%s: Bad length for writeView\n",Fname));
        return -1;
    }

    if ( (notifyView != NULL) && (strlen(notifyView) < 0 || strlen(notifyView) > 32) )  {
        DPRINTF((APERROR,"%s: Bad length for notifyView\n",Fname));
        return -1;
    }

    if (FLAG < 1 || FLAG > 3 )  {
        DPRINTF((APERROR,"%s: Bad value for FLAG\n",Fname));
        return -1;
    }

    if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Bad value for storage\n",Fname));
        return -1;
    }

   /* build the search struct */

    agt_vacmAccessTable.tip[0].value.octet_val = MakeOctetStringFromText(groupName);
    if (agt_vacmAccessTable.tip[0].value.octet_val == NULL)  {
        return -1;
    }

    if ((ctxName == NULL) || (strcmp(ctxName,"-")==0) || (strcmp(ctxName,"NULL")==0) )  {
        agt_vacmAccessTable.tip[1].value.octet_val = MakeOctetString(NULL,0);
    }  else { 
        agt_vacmAccessTable.tip[1].value.octet_val = MakeOctetStringFromText(ctxName);
    }
    if (agt_vacmAccessTable.tip[1].value.octet_val == NULL)   {
        FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
        return -1;
    }

    if (strcmp(securityModel,"snmpv1") == 0 )  {
     agt_vacmAccessTable.tip[2].value.uint_val = 1;
    }
    else if (strcmp(securityModel,"snmpv2c") == 0 )  {
     agt_vacmAccessTable.tip[2].value.uint_val = 2;
    }
    else if (strcmp(securityModel,"usm") == 0 )  {
      agt_vacmAccessTable.tip[2].value.uint_val = 3;
    }
    else  {
       DPRINTF((APERROR,"%s: Received bad value for SecruityModel\n",Fname));
       goto accessfail;
    }

    if (strcmp(securityLevel,"noAuthNoPriv") == 0 )  {
      agt_vacmAccessTable.tip[3].value.uint_val = 1;
    }
#ifndef SR_UNSECURABLE
    else if (strcmp(securityLevel,"authNoPriv") == 0 )  {
      agt_vacmAccessTable.tip[3].value.uint_val = 2;
    }
#ifndef SR_NO_PRIVACY
    else if (strcmp(securityLevel,"authPriv") == 0 )  {
      agt_vacmAccessTable.tip[3].value.uint_val = 3;
    }
#endif  /* SR_NO_PRIVACY */
#endif  /* SR_UNSECURABLE  */
    else  {
       DPRINTF((APERROR,"%s: Received bad value for SecurityLevel\n",Fname));
       goto accessfail;
    }

    index = SearchTable(&agt_vacmAccessTable, EXACT);

    if ( FLAG == 1 || FLAG == 2 )  {

        if (FLAG == 1 )  {
            if (index != -1) {
                DPRINTF((APERROR,
                  "%s: Cannot create row in vacmAccessTable, row already exists\n",Fname));
                goto accessfail;
            }

            index = NewTableEntry(&agt_vacmAccessTable);
                if (index == -1) {
                  DPRINTF((APERROR,"%s:  Couldn't create new row in vacmAccessTable\n",Fname));
                  goto accessfail;
                }
        }  else {		/* FLAG == 2 */
                if (index == -1) {
                    DPRINTF((APERROR,"%s: No such row to modify in vacmAccessTable\n",Fname));
                    goto accessfail;
                }
        }

        ace = (vacmAccessEntry_t *)agt_vacmAccessTable.tp[index];

      if (ace->vacmAccessReadViewName != NULL) {
         FreeOctetString(ace->vacmAccessReadViewName);
        }
        if ( (readView == NULL) || (strcmp(readView,"-") == 0) 
                           || (strcmp(readView,"NULL") == 0) )    {
            ace->vacmAccessReadViewName = MakeOctetString(NULL, 0);
        } else {
            ace->vacmAccessReadViewName = MakeOctetStringFromText(readView);
        }
        if (ace->vacmAccessReadViewName == NULL)  {
            goto accessfail;
        }

        if (ace->vacmAccessWriteViewName != NULL) {
          FreeOctetString(ace->vacmAccessWriteViewName);
        } 
        if ( (writeView == NULL) || (strcmp(writeView,"-") == 0) 
                            || (strcmp(writeView,"NULL") == 0) )    {
            ace->vacmAccessWriteViewName = MakeOctetString(NULL, 0);
        } else {
            ace->vacmAccessWriteViewName = MakeOctetStringFromText(writeView);
        }
        if (ace->vacmAccessWriteViewName == NULL)  {
            goto accessfail;
        }

      if (ace->vacmAccessNotifyViewName != NULL) {
         FreeOctetString(ace->vacmAccessNotifyViewName);
        }
        if ( (notifyView == NULL) || (strcmp(notifyView,"-") == 0) 
                              || (strcmp(notifyView,"NULL") == 0)  ) {
            ace->vacmAccessNotifyViewName = MakeOctetString(NULL, 0);
        } else {
            ace->vacmAccessNotifyViewName = MakeOctetStringFromText(notifyView);
        }
        if (ace->vacmAccessNotifyViewName == NULL)  {
            goto accessfail;
        }

        if( (ctxName == NULL) || (strcmp(ctxName,"-") == 0)
                          || (strcmp(ctxName,"NULL") == 0)  ) {
             ace->vacmAccessContextMatch = 1;		/* exact match */
        } else  {
             ace->vacmAccessContextMatch = 2;		/* prefex match */
        }

        ace->vacmAccessStorageType = storage;
        ace->vacmAccessStatus = D_vacmAccessStatus_active;
        memset(ace->valid, 0xff, sizeof(ace->valid));
        goto accessdone;
    }			/* end of if FLAG = 1 or 2 */
    else  {			/* FLAG == 3 */
        if (index == -1) {
            DPRINTF((APERROR,"%s: No such row to delete in vacmAccessTable\n",Fname));
            goto accessfail;
        }

        DeletevacmAccessEntry(index);       
        goto accessdone;
    }				/* end of if FLAG == 3 */

 accessfail:		/* free malloced items */
       FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
       FreeOctetString(agt_vacmAccessTable.tip[1].value.octet_val);
       if (ace != NULL)  {    		/* table entry created, but error occured */
          DeletevacmAccessEntry(index);
       }
       return -1;

 accessdone:		/* free malloced items and force config file rewrite */
        FreeOctetString(agt_vacmAccessTable.tip[0].value.octet_val);
        FreeOctetString(agt_vacmAccessTable.tip[1].value.octet_val);
        writeConfigFileFlag = TRUE;
        return 0;

}		/* end of function ManageAccessEntry */

/*
 * ManageCommunityEntry() :
 *
 *  This function will access the snmpCommunityTable. The value of
 *  the FLAG parameter that you pass into this routine determines
 *  what operation will be performed :
 *  1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 *  
 *  The snmpCommunityTable is indexed by the snmpCommunityIndex which
 *  can be any unique string for each entry in the snmpCommunityTable.
 *  The snmpCommunityIndex does not have any inherent meaning other than
 *  to act as a unique identifier for each row in the table.
 *
 *  The contextEngineID, contextName, and target values that you pass
 *  into this routine can either be NULL or can specify a valid value.
 *  If NULL is used, then the contextEngineID used will be "localSnmpID"
 *  and the default snmpCommunityContextName and snmpCommunityTransportTag
 *  will be used.
 * 
 *  Upon success 0 is returned, -1 is returned for any error.
 */      
int 
ManageCommunityEntry(
   const char        *communityIndex,       /* snmpCommunityIndex */
   const char        *communityName,        /* snmpCommunityName */
   const char        *securityName,         /* snmpCommunitySecurityName */
   OctetString       *contextEngineID,      /* snmpCommunityContextEngineID */
   const char        *contextName,          /* snmpCommunityContextName */
   const char        *target,               /* snmpCommunityTransportTag */
   int         storage,               /* snmpCommunityStorage */
   int         status,
   SR_UINT32   FLAG)
{ 
  
   int index; 
   snmpCommunityEntry_t *sce = NULL;
  
   FNAME("ManageCommunityEntry")
 
   if (FLAG == 3) {
     /* when deleting a user, only require community index */
     if (communityIndex == NULL) { 
        DPRINTF((APERROR,"%s: Community Index is NULL\n", Fname));
        return -1;
     }
   }
   else {
     if (communityIndex == NULL || communityName == NULL ||
                                                  securityName == NULL) {
       DPRINTF((APERROR,
           "%s: Community index, name or security name is NULL\n",Fname));
       return -1;
     }
     if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
        return -1;
     }
   }

   if (FLAG < 1 || FLAG > 3) {
	DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
        return -1;      
    }
 
    
   /* build the search structure */
   agt_snmpCommunityTable.tip[0].value.octet_val =
                MakeOctetStringFromText(communityIndex);
   if (agt_snmpCommunityTable.tip[0].value.octet_val == NULL) {
     return -1;
   }
 
   index = SearchTable(&agt_snmpCommunityTable, EXACT);
 
   if (FLAG == 1 || FLAG == 2) {
      if (FLAG == 1) {
         if(index != -1) {   
           DPRINTF((APERROR,
                "%s: Cannot create row, row already exists!\n", Fname));
           goto commfail;
         }
 
         /* create new entry in snmpCommunityTable */
         index = NewTableEntry(&agt_snmpCommunityTable);
         if (index == -1) {
            DPRINTF((APERROR, 
                 "%s: Cannot create new row in snmpCommunityTable\n", Fname));
            goto commfail;
         }
 
     } else {     /* FLAG == 2 */
        if (index == -1) {
           DPRINTF((APERROR,
                "%s: Cannot modify row, no such row in table\n", Fname));
           goto commfail;
        }      
     }
   
     /* set up pointer to this row in the table */
     sce = (snmpCommunityEntry_t *)agt_snmpCommunityTable.tp[index];
   
     /* copy values passed to this routine into your snmpCommunityTable */
  
     if (sce->snmpCommunityName != NULL) {
       FreeOctetString(sce->snmpCommunityName);
     }
     sce->snmpCommunityName = MakeOctetStringFromText(communityName);
 
     if (sce->snmpCommunitySecurityName != NULL) {
       FreeOctetString(sce->snmpCommunitySecurityName); 
     }
     sce->snmpCommunitySecurityName = MakeOctetStringFromText(securityName);
        
     if (sce->snmpCommunityContextEngineID != NULL) {
       FreeOctetString(sce->snmpCommunityContextEngineID);
     }
     if (contextEngineID == NULL ||
          (strncmp("localSnmpID",(char*)contextEngineID->octet_ptr,
              MIN(contextEngineID->length, strlen("localSnmpID")))==0) ||
          (strncmp("-",(char*)contextEngineID->octet_ptr, 
              MIN(contextEngineID->length, strlen("-")))==0)) {
       /* assign default local snmpEngineID */
       sce->snmpCommunityContextEngineID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);
     }
     else {
       sce->snmpCommunityContextEngineID = CloneOctetString(contextEngineID);
     }  
   
     if (sce->snmpCommunityContextName != NULL) {
       FreeOctetString(sce->snmpCommunityContextName);
     }
     if (contextName == NULL || (strcmp("-",contextName)==0)) {
       sce->snmpCommunityContextName = MakeOctetStringFromText("");
     }
     else {
       sce->snmpCommunityContextName = MakeOctetStringFromText(contextName);
     }
 
     if (sce->snmpCommunityTransportTag != NULL) {
       FreeOctetString(sce->snmpCommunityTransportTag);
     }   
     if (target == NULL || (strcmp(target,"-")==0)) {
       sce->snmpCommunityTransportTag = MakeOctetStringFromText("");
     }     
     else {
       sce->snmpCommunityTransportTag = MakeOctetStringFromText(target);
     }   
         
     memset(sce->valid, 0xff, sizeof(sce->valid));
            
     /* check to make sure memory allocations above all succeeded */
     if ( (sce->snmpCommunityIndex == NULL) || (sce->snmpCommunityName == NULL)
        || (sce->snmpCommunitySecurityName == NULL)
        || (sce->snmpCommunityContextEngineID  == NULL)
        || (sce->snmpCommunityContextName == NULL)
        || (sce->snmpCommunityTransportTag == NULL)  ) {
           goto commfail;
     }          
           
     sce->snmpCommunityStorageType = storage;
     /* LVL7 */
     if (status == TRUE)
     {
     sce->snmpCommunityStatus = D_snmpCommunityStatus_active;
     }
     else
     {
       sce->snmpCommunityStatus = D_snmpCommunityStatus_notInService;
     }
     /* LVL7 end */
   
     goto commdone;
     
   
  }            /* end of  if (FLAG == 1 || FLAG == 2) */
  else {       /* Begin FLAG == 3 */
     
      if (index == -1) {
        DPRINTF((APERROR,
            "%s: This row does not exist, cannot delete this row\n", Fname));
         goto commfail;
      } 
       
      /* if row does exist, then delete it */
      FreeEntries(snmpCommunityEntryTypeTable, agt_snmpCommunityTable.tp[index])
;       
      free(agt_snmpCommunityTable.tp[index]); 
      RemoveTableEntry(&agt_snmpCommunityTable, index);
      goto commdone;
     
  }       
              
  
commfail :              /* free malloced items */
    FreeOctetString(agt_snmpCommunityTable.tip[0].value.octet_val);
    if (sce != NULL) {
       FreeEntries(snmpCommunityEntryTypeTable, agt_snmpCommunityTable.tp[index]
);   
       free(agt_snmpCommunityTable.tp[index]);
       RemoveTableEntry(&agt_snmpCommunityTable, index);
    }
    return -1;
       
commdone :              /* free malloced items and flag config file rewrite*/
    FreeOctetString(agt_snmpCommunityTable.tip[0].value.octet_val);
    writeConfigFileFlag = TRUE;
    return 0;
     
}      
 




/*
 * ManageTargetParamsEntry() :
 *
 *  This function will access the snmpTargetParamsTable. The value of
 *  the FLAG parameter that you pass into this routine determines
 *  what operation will be performed :
 *  1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 *  
 *  The snmpTargetParamsTable is indexed by the snmpTargetParamsIndex which
 *  can be any unique string for each entry in the snmpTargetParamsTable.
 *  The snmpTargetParamsIndex does not have any inherent meaning other than
 *  to act as a unique identifier for each row in the table.
 *
 *  The snmpTargetParamsMPModel (mpModel) must be given a valid value of 
 *       0 (snmpv1)
 *       1 (snmpv2c)
 *       3 (snmpv3)
 *
 *  The snmpTargetParamsSecurityModel (securityModel) must be given a valid
 *  value of one of the following security models :
 *       snmpv1
 *       snmpv2c
 *       usm
 *  
 *  The snmpTargetParamsSecurityLevel (securityLevel) must be given a valid
 *  value of one of the following security levels :
 *    	noAuthNoPriv
 *  	authNoPriv
 * 	authPriv
 *
 *  Upon success 0 is returned, -1 is returned for any error.
 */      
int       
ManageTargetParamsEntry(const char *name,	      /* snmpTargetParamsName */
			int   mpModel,             /* snmpTargetParamsMPModel */
			const char *securityModel, /* snmpTargetParamsSecurityModel*/
			const char *securityName,   /* snmpTargetParamsSecurityName */
			const char *securityLevel,  /* snmpTargetParamsSecurityLevel*/
			int  storage,	      /* snmpTargetParamsStorageType */
			SR_UINT32 FLAG) 
{
 
    int index;
    snmpTargetParamsEntry_t *stp = NULL;

    FNAME("ManageTargetParamsEntry")
    
    /* check incoming parameters */
   
    if (FLAG == 3)  {    /* if deleting entry, only care about name arg */
      if (name == NULL) {
	DPRINTF((APERROR,"%s: name is NULL\n", Fname));
        return -1;
      }
    }
    else {    /* FLAG == 1 or 2 */
      if (name == NULL || securityModel == NULL || securityName == NULL || 
		securityLevel == NULL) {

	DPRINTF((APERROR, "%s: Check parameters, one of the fields is NULL", 
			Fname));
	return -1;
      }
      if (mpModel <0 || mpModel > 3) {
        DPRINTF((APERROR,"%s: Received bad value for mpModel\n", Fname));
        return -1; 
      }
      if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Received bad value for storage\n", Fname));
        return -1;
      }
    }

    if (FLAG < 1 || FLAG > 3 )  {
        DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
        return -1;      
    }



    /* build the search structure */


   agt_snmpTargetParamsTable.tip[0].value.octet_val = 
		MakeOctetStringFromText(name);
   if (agt_snmpTargetParamsTable.tip[0].value.octet_val == NULL) {
     DPRINTF((APERROR,"%s: Unable to create OctetString for name %s\n", Fname,
	name));
     return -1;
   }  

   index = SearchTable(&agt_snmpTargetParamsTable, EXACT);

   if (FLAG == 1 || FLAG == 2) {     /* adding or modifying a row */
     
      if (FLAG ==1) {
	if (index != -1) {
          DPRINTF((APERROR, 
		"%s: Cannot create row, row already exists\n", Fname));  
	  goto targetfail;
        }
	
	index = NewTableEntry(&agt_snmpTargetParamsTable);
	if (index == -1) {
	  DPRINTF((APERROR,
		"%s: Cannot create new row in snmpTargetParamsTable\n", Fname));
          goto targetfail;
        }
     }
     else {   /* FLAG == 2 */
        if (index == -1) {
	  DPRINTF((APERROR,
	 	"%s: No such row to modify in snmpTargetParamsTable\n", Fname));
          goto targetfail;
        }
     }

     /* fill in values for this row in snmpTargetParamsTable */
     stp = (snmpTargetParamsEntry_t *)agt_snmpTargetParamsTable.tp[index];

     stp->snmpTargetParamsMPModel = mpModel;
  
     if (strcmp(securityModel, "snmpv1") == 0) {
	stp->snmpTargetParamsSecurityModel = SR_SECURITY_MODEL_V1;
     }
     else if (strcmp(securityModel, "snmpv2c") == 0) {
	stp->snmpTargetParamsSecurityModel = SR_SECURITY_MODEL_V2C;
     }
     else if (strcmp(securityModel, "usm") == 0) {
	stp->snmpTargetParamsSecurityModel = SR_SECURITY_MODEL_USM;
     }
     else {
       DPRINTF((APERROR,"%s: Received bad value for securityModel\n", Fname));
       goto targetfail;
     }

     if (stp->snmpTargetParamsSecurityName != NULL) {
        FreeOctetString(stp->snmpTargetParamsSecurityName);
     }
     stp->snmpTargetParamsSecurityName = 
			MakeOctetStringFromText(securityName);
	
     if (strcmp(securityLevel, "noAuthNoPriv") == 0) {
        stp->snmpTargetParamsSecurityLevel = 
		D_vacmAccessSecurityLevel_noAuthNoPriv;
     }
     else if (strcmp(securityLevel, "authNoPriv") == 0) {
        stp->snmpTargetParamsSecurityLevel = 
		D_vacmAccessSecurityLevel_authNoPriv;
     }
     else if ( strcmp(securityLevel, "authPriv") == 0) {
	stp->snmpTargetParamsSecurityLevel = 
		D_vacmAccessSecurityLevel_authPriv;
     }
     else {
        DPRINTF((APERROR,"%s: Received bad value for securityLevel\n", Fname));
        return -1;
     }
     

     stp->snmpTargetParamsStorageType = storage;

     memset(stp->valid, 0xff, sizeof(stp->valid));

     /* check to make sure memory allocation succeeded */
     if ( (stp->snmpTargetParamsSecurityModel == 0) ||
          (stp->snmpTargetParamsSecurityName == NULL) ||
          (stp->snmpTargetParamsSecurityLevel == 0)  ) {
	goto targetfail;
     }

     stp->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_active;
 
     goto targetdone;

   }     		/* end of if (FLAG == 1 || FLAG == 2) */
   else  { 		/* Begin FLAG == 3 */

     if (index == -1) {
       DPRINTF((APERROR,"%s: This row does not exist, cannot delete\n", Fname));
       goto targetfail;
     }
     
     /* Delete row */
     FreeEntries(snmpTargetParamsEntryTypeTable, 
			agt_snmpTargetParamsTable.tp[index]); 
     free(agt_snmpTargetParamsTable.tp[index]);
     RemoveTableEntry(&agt_snmpTargetParamsTable, index);
     goto targetdone;
  }

  
targetfail :
     /* free malloced items */
     if (stp != NULL) {
       FreeEntries(snmpTargetParamsEntryTypeTable, 
                        agt_snmpTargetParamsTable.tp[index]);
       free(agt_snmpTargetParamsTable.tp[index]);
       RemoveTableEntry(&agt_snmpTargetParamsTable, index);
     }
     FreeOctetString(agt_snmpTargetParamsTable.tip[0].value.octet_val);
     return -1;


targetdone:
     /* free malloced items and flag config file rewrite */  
     FreeOctetString(agt_snmpTargetParamsTable.tip[0].value.octet_val);
     writeConfigFileFlag = TRUE;
     return 0; 
}

/* 
 * ManageNotifyEntry() :
 * 
 *  This function will access the snmpNotifyTable. The value of
 *  the FLAG parameter that you pass into this routine determines
 *  what operation will be performed :
 *  1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 *  
 *  The snmpNotifyTable is indexed by the snmpNotifyName which can
 *  be any unique string associated with each snmpNotifyEntry.
 *  The snmpNotifyName does not have any inherent meaning other than
 *  to act as a unique identifier for each row in the table.
 *  
 *  The snmpNotifyTag is used to select entries in the snmpTargetAddrTable
 *  to be used when generating notifications. Any active entry in the
 *  snmpTargetAddrTable whose snmpTargetAddrTagList object contains the
 *  value of this snmpNotifyTag will be selected as a notification target.
 *
 *  The snmpNotifyType value specifies the type of notification (trap or
 *  inform) that should be sent to corresponding targets in the
 *  snmpTargetAddrTable whose snmpTargetAddrTagList object includes the
 *  value of this snmpNotifyEntry's snmpNotifyTag. The values of
 *  snmpNotifyType should be:
 * 
 *       1 - trap       2 - inform
 * 
 *  Upon success, this routine will return 0, -1 is returned for any error  
 *   
 */ 
int
ManageNotifyEntry(
   const char  *notifyName,        /* snmpNotifyName */
   const char  *notifyTag,         /* snmpNotifyTag  */
   int         notifyType,         /* snmpNotifyType */
   int         storage,            /* snmpNotifyRowStatus */
   SR_UINT32   FLAG) 
{    
                        
   int index;
   snmpNotifyEntry_t *sne = NULL;
     
   FNAME("ManageNotifyEntry")
 
   if (FLAG == 3) {
      /* when deleting an entry, only require name index */
      if (notifyName == NULL) {
         DPRINTF((APERROR, "%s: notify name is NULL\n", Fname));
         return -1;
      }                 
   }   
   else {
      if (notifyName == NULL || notifyTag == NULL) {
         DPRINTF((APERROR, "%s: notify name or notify tag is NULL\n", Fname));
         return -1;
      }
      if (notifyType < 1 || notifyType > 2) {
        DPRINTF((APERROR,"%s: Received bad value for notify type\n",Fname));
        return -1;
      }
      if (storage < 1 || storage > 5) {
        DPRINTF((APERROR,"%s: Received bad value for storage\n",Fname));
        return -1;
      }
 
   }
 
   if (FLAG < 1 || FLAG > 3) {
       DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
       return -1;
   }
 
   /* build the search structure */
   agt_snmpNotifyTable.tip[0].value.octet_val =
               MakeOctetStringFromText(notifyName);
   if (agt_snmpNotifyTable.tip[0].value.octet_val == NULL) {
     return -1; 
   }
 
   index = SearchTable(&agt_snmpNotifyTable, EXACT);
 
   if (FLAG == 1 || FLAG == 2) {
      if (FLAG == 1) {
         if(index != -1) {
           DPRINTF((APERROR,
                "%s: Cannot create row, row already exists!\n", Fname));
           goto notifyfail;
         }
   
         /* create new entry in snmpNotifyTable */
         index = NewTableEntry(&agt_snmpNotifyTable);
         if (index == -1) {
            DPRINTF((APERROR,
                 "%s: Cannot create new row in snmpCommunityTable\n", Fname));
            goto notifyfail;
         }
   
       } else {     /* FLAG == 2 */
         if (index == -1) {
            DPRINTF((APERROR,      
                 "%s: Cannot modify row, no such row in table\n", Fname));
            goto notifyfail;
         }              
       }
   
       /* set up pointer to this row in the table */
       sne = (snmpNotifyEntry_t *)agt_snmpNotifyTable.tp[index];
 
       /* copy values passed to this routine into your snmpNotifyTable */
      
       if (sne->snmpNotifyTag != NULL) {
          FreeOctetString(sne->snmpNotifyTag); 
       } 
       sne->snmpNotifyTag = MakeOctetStringFromText(notifyTag);
   
       sne->snmpNotifyType = notifyType;
      
       sne->snmpNotifyStorageType = storage;
         
       sne->snmpNotifyRowStatus = D_snmpNotifyRowStatus_active;
 
       memset(sne->valid, 0xff, sizeof(sne->valid)); 
        
       /* check to make sure memory allocation above succeeded */
       if ( (sne->snmpNotifyTag == NULL) ) {
          goto notifyfail;
       }
 
       goto notifydone;
   
   }            /* end of  if (FLAG == 1 || FLAG == 2) */
   else {       /* Begin FLAG == 3 */
       
      if (index == -1) {
        DPRINTF((APERROR,
            "%s: This row does not exist, cannot delete this row\n", Fname));
         goto notifyfail;
      }
               
      /* if row does exist, then delete it */
      FreeEntries(snmpNotifyEntryTypeTable, agt_snmpNotifyTable.tp[index]);
      free(agt_snmpNotifyTable.tp[index]);
      RemoveTableEntry(&agt_snmpNotifyTable, index);
      goto notifydone;
 
   }
      
         
notifyfail:            /* free malloced items */
   FreeOctetString(agt_snmpNotifyTable.tip[0].value.octet_val); 
   if (sne != NULL) {
      FreeEntries(snmpNotifyEntryTypeTable, agt_snmpNotifyTable.tp[index]);
      free(agt_snmpNotifyTable.tp[index]);
      RemoveTableEntry(&agt_snmpNotifyTable, index);
   }     
   return -1;
     
                 
notifydone: 
   FreeOctetString(agt_snmpNotifyTable.tip[0].value.octet_val);
   writeConfigFileFlag = TRUE;
   return 0;
         
}



/* ManageContextEntry():
 *
 * This function will access the vacmContextTable.  Note that the
 * vacmContextTable is not an SNMP read-write table, so this table
 * can be modified internally by the agent, but cannot be configured
 * remotely via SNMP.  
 *
 * This routine provides the ability to add/remove entries to this 
 * table, but the entries in this table are not modifiable.
 *
 * The value of FLAG decides what to do:
 * 1 - Add new entry   3 - Delete an entry
 * vacmContextTable is indexed by ContextName.
 * Upon success 0 is returned, -1 is returned for any error.
 *
 * The vacmContextName value is provided by the ContextName and specifies
 * the context to add to the vacmContextTable. This name is limited to an
 * ASCII-displayable string of no more than 32 characters in length.
 */
int
ManageContextEntry(
    const char *ContextName,
    SR_UINT32 FLAG)
{
 
    int index;
    vacmContextEntry_t *vce = NULL;
    FNAME("ManageContextEntry")
 
    /* error check incoming args */
 
    if (ContextName == NULL) {
        DPRINTF((APERROR,"%s: ContextName is NULL\n",Fname));
        return -1;
    }
 
    if (strlen(ContextName) < 1 || strlen(ContextName) > 32)  {
        DPRINTF((APERROR,"%s: Received bad length for ContextName\n",Fname));
        return -1;
    }
 
    if (FLAG != 1 && FLAG != 3 )  {
        DPRINTF((APERROR,"%s: Received bad value for FLAG\n",Fname));
        return -1;
    }
 
 
  /* build the search struct */
 
    agt_vacmContextTable.tip[0].value.octet_val =
        MakeOctetStringFromText(ContextName);
    if (agt_vacmContextTable.tip[0].value.octet_val == NULL)  {
        return -1;
    }
 
    index = SearchTable(&agt_vacmContextTable, EXACT);
 
    if  ( FLAG == 1 ) {
 
       if (index != -1) {
           DPRINTF((APERROR,
              "%s: Cannot create row in vacmContextTable, row already exists\n",Fname));
           goto contextfail;
       }
 
       index = NewTableEntry(&agt_vacmContextTable);
       if (index == -1) {
           DPRINTF((APERROR,"Could not create vacmContextTable for %s\n",
                        ContextName));
           goto contextfail;
       }
 
       vce = (vacmContextEntry_t *)agt_vacmContextTable.tp[index];
       memset(vce->valid, 0xff, sizeof(vce->valid));

       goto contextdone;
    }                   /* end of if FLAG = 1 */
    else  {                     /* FLAG == 3 */
 
        if (index == -1) {
            DPRINTF((APERROR,"%s: No such row to delete in vacmContextTable\n",Fname));
            goto contextfail;
        }
 
        FreeEntries(vacmContextEntryTypeTable, agt_vacmContextTable.tp[index]);
        free(agt_vacmContextTable.tp[index]);
        RemoveTableEntry(&agt_vacmContextTable, index);
 
        goto contextdone;
    }                   /* end of else FLAG == 3  */
 
 contextfail:           /* free malloced items */
       FreeOctetString(agt_vacmContextTable.tip[0].value.octet_val);
       if (vce != NULL)  {       /* table entry created, but error occured */
           FreeEntries(vacmContextEntryTypeTable, agt_vacmContextTable.tp[index]);
           free(agt_vacmContextTable.tp[index]);
           RemoveTableEntry(&agt_vacmContextTable, index);
       }
       return -1;
 
 contextdone:           /* free malloced items and force config file rewrite */
        FreeOctetString(agt_vacmContextTable.tip[0].value.octet_val);
        writeConfigFileFlag = TRUE;
        return 0;
 
}               /* end of function ManageContextEntry */



/*
 * ManageNotifyFilterProfileEntry()
 *
 *  This function will access the snmpNotifyFilterProfileTable. The value of
 *  the FLAG parameter that you pass into this routine determines
 *  what operation will be performed :
 *  1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 *
 *  The snmpNotifyFilterProfileTable is indexed by the snmpTargetParamsName
 *  in the snmpTargetParamsTable associated with each 
 *  snmpNotifyFilterProfileEntry.
 *  The parameter paramName = snmpTargetParamsName
 *  
 *  The snmpNotifyFilterProfileName is the name of teh profile to be used
 *  when generating notifications using the corresposnding enrty in the 
 *  snmpTargetAddrTable.
 *  The parameter profileName = snmpNotifyFilterProfileName
 *
 *  The parameter storage = snmpNotifyFilterProfileStorType
 *             1 - other
 *             2 - volatile
 *             3 - nonVolatile
 *             4 - permanent
 *             5 - readOnly
 *
 *  Upon success, this routine will return 0, -1 is returned for any error.
 */

int
ManageNotifyFilterProfileEntry(const char *paramName,
                               const char *profileName,
                               int storage,
                               SR_UINT32 FLAG)
{
    int index, index1;
    snmpNotifyFilterProfileEntry_t *snfpe = NULL;

    FNAME("ManageNotifyFilterProfileEntry")

    /* check incoming parameters */

    if (FLAG == 3)  {    /* if deleting entry, only care about name arg */
	if (paramName == NULL) {
	    DPRINTF((APERROR,"%s: param_name is NULL\n", Fname));
            return -1;
        }
    }
    else { /* FLAG == 1 or 2 */
        if ((paramName == NULL) || (profileName == NULL)) {
            DPRINTF((APERROR, 
                "%s: Check parameters, one of the fields is NULL", Fname));
            return -1;
        }
        if (strlen(profileName) < 1 || strlen(profileName) > 32) {
            DPRINTF((APERROR,"%s: Received bad length for profileName\n",
                                                                 Fname));
            return -1;
        }
    }
    if (storage < 1 || storage > 5) {
	DPRINTF((APERROR,"%s: Received bad value for storage\n", Fname));
        return -1;
    }
    if (FLAG < 1 || FLAG > 3 )  {
	DPRINTF((APERROR,"%s: Received bad value for FLAG\n", Fname));
        return -1;
    }

    /* build the search structure */
    agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val =
                           MakeOctetStringFromText(paramName);
    if (agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val == NULL) {
        DPRINTF((APERROR, "%s: Unable to create OctetString for name %s\n",
                                             Fname, paramName));
        return -1;
    }

    index = SearchTable(&agt_snmpNotifyFilterProfileTable, EXACT);

    if (FLAG == 1 || FLAG == 2) {     /* adding or modifying a row */
	if (FLAG == 1) {
            if (index != -1) {
                DPRINTF((APERROR,
                    "%s: Cannot create row, row already exists\n", Fname));
                goto targetfail;
            }
            /*
             * check snmpTargetParamsName in the snmpTargetParamsEntry.
             */
	    agt_snmpTargetParamsTable.tip[0].value.octet_val =
                                   MakeOctetStringFromText(paramName);
            if (agt_snmpTargetParamsTable.tip[0].value.octet_val == NULL) {
                DPRINTF((APERROR, 
                        "%s: Unable to create OctetString for name %s\n",
                                                        Fname, paramName));
                goto targetfail;
            }
            index1 = SearchTable(&agt_snmpTargetParamsTable, EXACT);
            FreeOctetString(
                   agt_snmpTargetParamsTable.tip[0].value.octet_val);
            if (index1 == -1) {
                DPRINTF((APERROR,
                    "%s: No ParamsName in the snmpTargetParamsEntry\n",
			                                           Fname));
                goto targetfail;
            }
            index = NewTableEntry(&agt_snmpNotifyFilterProfileTable);
            if (index == -1) {
                DPRINTF((APERROR,
                 "%s: Cannot create new row in snmpNotifyFilterProfileTable\n",
                                                                  Fname));
                goto targetfail;
            }
        }
        else {    /* FLAG == 2 */
            if (index == -1) {
                DPRINTF((APERROR,
                    "%s: No such row to modify in snmpTargetParamsTable\n", 
                                                                     Fname));
                goto targetfail;
            }
        }

        /* fill in values for this row in snmpNotifyFilterProfileTable */
        snfpe = (snmpNotifyFilterProfileEntry_t *)
                                  agt_snmpNotifyFilterProfileTable.tp[index];

        if (snfpe->snmpNotifyFilterProfileName != NULL) {
            FreeOctetString(snfpe->snmpNotifyFilterProfileName);
        }
        snfpe->snmpNotifyFilterProfileName = 
                                MakeOctetStringFromText(profileName);
        snfpe->snmpNotifyFilterProfileStorType = storage;

        snfpe->snmpNotifyFilterProfileRowStatus = 
                                D_snmpNotifyFilterProfileRowStatus_active;
        memset(snfpe->valid, 0xff, sizeof(snfpe->valid));

        /* check to make sure memory allocation above succeeded */
        if (snfpe->snmpNotifyFilterProfileName == NULL) {
            goto targetfail;
        }
        goto targetdone;
    }
    else { /* Begin FLAG == 3 */
        if (index == -1) {
            DPRINTF((APERROR,
                    "%s: This row does not exist, cannot delete\n", Fname));
            goto targetfail;
        }

        /* Delete row */
        FreeEntries(snmpNotifyFilterProfileEntryTypeTable,
                                 agt_snmpNotifyFilterProfileTable.tp[index]);
        free(agt_snmpNotifyFilterProfileTable.tp[index]);
        RemoveTableEntry(&agt_snmpNotifyFilterProfileTable, index);
        goto targetdone;
    }

  targetfail :

    /* free malloced items */
    if (snfpe != NULL) {
        FreeEntries(snmpNotifyFilterProfileEntryTypeTable,
                                 agt_snmpNotifyFilterProfileTable.tp[index]);
        free(agt_snmpNotifyFilterProfileTable.tp[index]);
        RemoveTableEntry(&agt_snmpNotifyFilterProfileTable, index);
    }
    FreeOctetString(agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val);
    return -1;

  targetdone :

    /* free malloced items and flag config file rewrite */
    FreeOctetString(agt_snmpNotifyFilterProfileTable.tip[0].value.octet_val);
    writeConfigFileFlag = TRUE;
    return 0;
}


/*
 * ManageNotifyFilterEntry()
 *
 *  This function will access the snmpNotifyFilterTable. The value of
 *  the FLAG parameter that you pass into this routine determines
 *  what operation will be performed :
 *  1 - Add new entry      2 - modify a current entry    3 - Delete an entry
 *
 *  The snmpNotifyFilterTable is indexed by the snmpNotifyFilterProfileName
 *  in the snmpNotifyFilterProfileEntry and the snmpNotifyFilterSubtree
 *  associated with each snmpNotifyFilterProfileEntry.
 *  The parameter profileName = snmpNotifyFilterProfileName
 *                filterSubtree = snmpNotifyFilterSubtree
 *  
 *  The snmpNotifyFilterMask defines a family of subtrees which are included
 *  in or excluded from the filter profile.
 *  The parameter filterMask = snmpNotifyFilterMask
 *
 *  The parameter type = snmpNotifyFilterType
 *             1 - included, 2 - excluded
 *
 *  The parameter storage = snmpNotifyFilterStorageType
 *             1 - other
 *             2 - volatile
 *             3 - nonVolatile
 *             4 - permanent
 *             5 - readOnly
 *
 *  Upon success, this routine will return 0, -1 is returned for any error.
 */

int
ManageNotifyFilterEntry(const char *profileName,
			const char *filterSubtree,
                        const char *filterMask,
                        int type,
                        int storage,
                        SR_UINT32 FLAG)
{
    int index;
    snmpNotifyFilterEntry_t *nfe = NULL;
    FNAME("ManageNotifyFilterEntry")

    /* error checking  */

    if ((profileName == NULL) || 
	(filterSubtree == NULL)) {
        DPRINTF((APERROR,
             "%s: Either profileName or profileSubtree is NULL\n", Fname));
        return -1;
    }
    if ((strlen(profileName) < 1) || (strlen(profileName) > 32)) {
	DPRINTF((APERROR,"%s: Received bad length for profileName\n",
                                                              Fname));
        return -1;
    }
    if ((filterMask != NULL) && 
	    ((strlen(filterMask) < 0) || (strlen(filterMask) > 16))) {
        DPRINTF((APERROR,"%s: Received bad length for filterMask\n", Fname));
        return -1;
    }

    if (storage < 1 || storage > 5) {
	DPRINTF((APERROR,"%s: Received bad value for storage\n", Fname));
        return -1;
    }
    if (FLAG < 1 || FLAG > 3 )  {
	DPRINTF((APERROR,"%s: Received bad value for FLAG\n", Fname));
        return -1;
    }

    if (type < 1 || type > 2 )  {  /* 1 - include, 2 - exclude */
        DPRINTF((APERROR,"%s: Received bad value for type\n", Fname));
        return -1;
    }

    /* build the search structure */
    agt_snmpNotifyFilterTable.tip[0].value.octet_val =
                           MakeOctetStringFromText(profileName);
    if (agt_snmpNotifyFilterTable.tip[0].value.octet_val == NULL) {
        DPRINTF((APERROR, "%s: Unable to create OctetString for name %s\n",
                                             Fname, profileName));
        return -1;
    }
    agt_snmpNotifyFilterTable.tip[1].value.oid_val =
                           MakeOIDFromDot(filterSubtree);
    if (agt_snmpNotifyFilterTable.tip[1].value.oid_val == NULL) {
        DPRINTF((APERROR, "%s: Unable to create OID for subtree %s\n",
                                             Fname, filterSubtree));
        return -1;
    }

    index = SearchTable(&agt_snmpNotifyFilterTable, EXACT);

    if (FLAG == 1 || FLAG == 2) {     /* adding or modifying a row */
	if (FLAG == 1) {
            if (index != -1) {
                DPRINTF((APERROR,
                    "%s: Cannot create row, row already exists\n", Fname));
                goto targetfail;
            }
            index = NewTableEntry(&agt_snmpNotifyFilterTable);
            if (index == -1) {
                DPRINTF((APERROR,
                 "%s: Cannot create new row in snmpNotifyFilterTable\n",
                                                                  Fname));
                goto targetfail;
            }
        }
        else {    /* FLAG == 2 */
            if (index == -1) {
                DPRINTF((APERROR,
                "%s: No such row to modify in snmpNotifyFilterTable\n", 
                                                                     Fname));
                goto targetfail;
            }
        }
        
        nfe = (snmpNotifyFilterEntry_t *)agt_snmpNotifyFilterTable.tp[index];

        if (nfe->snmpNotifyFilterMask != NULL) {
            FreeOctetString(nfe->snmpNotifyFilterMask);
        }
        if ((filterMask == NULL) || 
            (strcmp(filterMask,"-") == 0) ||
            (strcmp(filterMask,"NULL") == 0)) {
            nfe->snmpNotifyFilterMask = MakeOctetString(NULL, 0);
        }
        else {
            nfe->snmpNotifyFilterMask = MakeOctetStringFromHex(filterMask);
        }
        if (nfe->snmpNotifyFilterMask == NULL) {
            goto targetfail;
        }
	nfe->snmpNotifyFilterType = type;
        nfe->snmpNotifyFilterStorageType = storage;
        nfe->snmpNotifyFilterRowStatus = D_snmpNotifyFilterRowStatus_active;
        memset(nfe->valid, 0xff, sizeof(nfe->valid));
        goto targetdone;
    }
    else { /* Begin FLAG == 3 */
        if (index == -1) {
            DPRINTF((APERROR,
                    "%s: This row does not exist, cannot delete\n", Fname));
            goto targetfail;
        }

        /* Delete row */
        FreeEntries(snmpNotifyFilterEntryTypeTable,
                                 agt_snmpNotifyFilterTable.tp[index]);
        free(agt_snmpNotifyFilterTable.tp[index]);
        RemoveTableEntry(&agt_snmpNotifyFilterTable, index);
        goto targetdone;
    }

  targetfail:

    /* free malloced items */
    FreeOctetString(agt_snmpNotifyFilterTable.tip[0].value.octet_val);
    FreeOID(agt_snmpNotifyFilterTable.tip[1].value.oid_val);
    if (nfe != NULL) {
        FreeEntries(snmpNotifyFilterEntryTypeTable,
                                 agt_snmpNotifyFilterTable.tp[index]);
        free(agt_snmpNotifyFilterTable.tp[index]);
    }
    return -1;

 targetdone:

    /* free malloced items and flag config file rewrite */
    FreeOctetString(agt_snmpNotifyFilterTable.tip[0].value.octet_val);
    FreeOID(agt_snmpNotifyFilterTable.tip[1].value.oid_val);
    writeConfigFileFlag = TRUE;
    return 0;
}
