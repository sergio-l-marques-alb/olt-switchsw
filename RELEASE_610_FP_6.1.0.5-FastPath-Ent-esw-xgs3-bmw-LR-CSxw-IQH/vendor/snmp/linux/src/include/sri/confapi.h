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

#ifndef SR_SRI_NEWFILES_H
#define SR_SRI_NEWFILES_H

#ifdef  __cplusplus
extern "C" {
#endif

int ManageTargetAddrEntry
                 (char *Name, 
                  char *TDomain,
                  char *TAddr,
                  int timeout,
                  int retries,
                  char *TagList,
                  char *Params,
                  int storage,
                  char *TMask,
                  int MMS,
                  SR_UINT32 FLAG);

int ManageUserEntry
                 (OctetString *snmpEngineID,
                  char *Name,
                  char *authprot,
                  char *privprot,
                  int storage,
                  char *target,
#ifndef SR_UNSECURABLE
                  char *authpass,
#endif  /*  SR_UNSECURABLE  */
#ifndef SR_NO_PRIVACY
                  char *privpass,
#endif  /* SR_NO_PRIVACY */
                  SR_UINT32 FLAG);

int ManageViewTreeFamilyEntry
                 (char *viewName,
                  char *viewTree,
                  char *viewMask,
                  int type,
                  int storage,
                  SR_UINT32 FLAG);

int ManageSecurityToGroupEntry
                 (char *SecurityModel,
                  char *SecurityName,
                  char *GroupName,
                  int storage,
                  SR_UINT32 FLAG);

int ManageAccessEntry
    SR_PROTOTYPE((char *groupName,
                  char *ctxName,
                  char *securityModel,
                  char *securityLevel,
                  char *readView,
                  char *writeView,
                  char *notifyView,
                  int storage,
                  SR_UINT32 FLAG));


int
ManageCommunityEntry
    SR_PROTOTYPE((const char  *communityIndex,
                  const char *communityName,
                  const char *securityName,
                  OctetString *contextEngineID,
                  const char *contextName,
                  const char *target,
                  int        storage,
                  SR_UINT32  FLAG));

int
ManageTargetParamsEntry
    SR_PROTOTYPE((const char *name,
                  int        mpModel,
                  const char *securityModel,
                  const char *securityName,
                  const char *securityLevel,
                  int        storage,
                  SR_UINT32  FLAG));

int
ManageNotifyEntry
    SR_PROTOTYPE((const char *notifyName,
                  const char *notifyTag,
                  int        notifyType,
                  int        storage,
                  SR_UINT32  FLAG));
int
ManageContextEntry
    SR_PROTOTYPE((const char *ContextName,
     		  SR_UINT32 FLAG));

int
ManageNotifyFilterProfileEntry
    SR_PROTOTYPE((const char *paramName,
                  const char *profileName,
                  int        storage,
                  SR_UINT32  FLAG));

int
ManageNotifyFilterEntry
    SR_PROTOTYPE((const char *profileName,
                  const char *filterSubtree,
                  const char *filterMask,
                  int        type,
                  int        storage,
                  SR_UINT32  FLAG));


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_NEWFILES_H */
