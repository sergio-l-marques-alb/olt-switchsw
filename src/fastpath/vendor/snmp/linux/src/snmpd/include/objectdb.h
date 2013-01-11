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

#ifndef SR_OBJECTDB_H
#define SR_OBJECTDB_H

#ifdef  __cplusplus
extern "C" {
#endif


int find_object
    SR_PROTOTYPE((struct _OID *oid,
                  int searchType));

#ifdef SR_SNMPv2_PDU
int IsV2Type
    SR_PROTOTYPE((int type));
#endif	/* SR_SNMPv2_PDU */

int NextObjectClass
    SR_PROTOTYPE((int i,
                  int payload));

int FindObjectClass
    SR_PROTOTYPE((struct _OID *oid,
                  int searchType,
                  int payload));

int MightContainNextOf
     SR_PROTOTYPE((OID *a,
                   OID *b));


struct _VarBind *GetExactObjectInstance
    SR_PROTOTYPE((struct _OID *incoming,
                  struct _OctetString *viewName,
                  struct _ContextInfo *contextInfo,
                  int serialNum,
                  int payload));

struct _VarBind *GetNextObjectInstance
    SR_PROTOTYPE((struct _OID *incoming,
                  struct _OctetString *viewName,
                  struct _ContextInfo *contextInfo,
                  int serialNum,
                  int payload));


#ifdef SR_SNMPv2_PDU
VarBind * exception
    SR_PROTOTYPE((int type,
                  OID *name,
                  int payload));
#endif	/* SR_SNMPv2_PDU */


struct _ObjectInfo *GetOidElem
    SR_PROTOTYPE((int i));

VarBind * i_GetVar
    SR_PROTOTYPE((ContextInfo *contextInfo,
                  int searchType,
                  OID *var));

#ifdef  __cplusplus
}
#endif

#endif	/* SR_OBJECTDB_H */
