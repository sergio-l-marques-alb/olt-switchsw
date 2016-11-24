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

#define        WANT_ALL_ERRORS

#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>

#include <string.h>

#include <sys/types.h>



#include "sr_snmp.h"
#include "sr_trans.h"
#include "sr_msg.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "sr_user.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "method.h"
#include "snmptype.h"
#include "agtctx.h"
#include "diag.h"


SR_FILENAME


extern SnmpV2Table communities;

/*
 * Search for the authorization structure associated with
 * this community (or party) name.
 *
 * error_code is set to -1 if authentication failure traps
 * should be generated.
 *
 * If a proxy context is discovered, the return code is 1.
 */
ContextInfo *
findContextInfo(
    SnmpLcd *snmp_lcd,
    TransportInfo *srcTI,
    TransportInfo *dstTI,
    SnmpMessage *snmp_msg,
    AdminInfo *admin_info,
    SR_UINT32 *limit,
    unsigned int outLen)
{
    FNAME("findContextInfo")
    int i;
    ContextInfo    *cip = NULL;
    COMMUNITY *community = NULL;
    SR_INT32 max_vbl_size_outgoing;


    /* Calculate the maximum varbind list size that we can send */
    max_vbl_size_outgoing = (MIN((SR_INT32)outLen, 
                                 use_large_v1_pdu ?
				     (SR_INT32)sr_pkt_size
				   : (SR_INT32)484)) -
                            snmp_msg->wrapper_size - SR_PDU_WRAPPER_SIZE;

    /* The real maximum varbind list length is the minimum of:
     *    - the maximum varbind list length we can send
     *    - the maximum varbind list length the receiver can receive
     */
    max_vbl_size_outgoing = MIN(snmp_msg->max_vbl_size, max_vbl_size_outgoing);
    if (max_vbl_size_outgoing < 0) {
        *limit = 0;
    } else {
        *limit = (SR_UINT32)max_vbl_size_outgoing;
    }


    cip = AllocContextInfo((SR_UINT32)5);
    if (cip == NULL) {
        return NULL;
    }
    cip->u.new_context_info.tv[0].tag = NCI_SENDER_TRANSPORT;
    cip->u.new_context_info.tv[0].value.os.type = OCTET_PRIM_TYPE;
    cip->u.new_context_info.tv[0].value.os.os_value =
        MakeOctetString((unsigned char *)&(srcTI->t_ipAddr), 4);
    cip->u.new_context_info.tv[1].tag = NCI_SNMP_VERSION;
    cip->u.new_context_info.tv[1].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[1].value.os.sl_value = snmp_msg->version;


    for (i = 0; i < communities.nitems; i++) {
        community = (COMMUNITY *)communities.tp[i];
        if ((snmp_msg->u.v1.community->length ==
             community->community_name->length) &&
            MATCH_IP_ADDR(community->ip_addr, srcTI->t_ipAddr) &&
            (!memcmp((char *) snmp_msg->u.v1.community->octet_ptr,
                     community->community_name->octet_ptr,
                     (int)community->community_name->length))) {
            cip->u.new_context_info.tv[2].tag = NCI_COMMUNITY;
            cip->u.new_context_info.tv[2].value.os.type = OCTET_PRIM_TYPE;
            cip->u.new_context_info.tv[2].value.os.os_value =
                CloneOctetString(community->community_name);
            cip->u.new_context_info.tv[3].tag = NCI_COMMUNITY_PRIVS;
            cip->u.new_context_info.tv[3].value.os.type = INTEGER_TYPE;
            cip->u.new_context_info.tv[3].value.os.sl_value = community->privs;
            admin_info->privs = community->privs;
            
            /* append place-holder for request-id */
            cip->u.new_context_info.tv[4].tag = NCI_REQUEST_ID;
            cip->u.new_context_info.tv[4].value.os.type = INTEGER_TYPE;
            cip->u.new_context_info.tv[4].value.os.sl_value = 0;
            return cip;
        }
    }
    DPRINTF((APACCESS, "%s: No matching community in table.\n", Fname));
    snmp_msg->error_code = SR_SNMP_BAD_COMMUNITY_NAMES;
    FreeContextInfo(cip);
    return NULL;
}                                /* findContextInfo() */

/*
 * LookupViewFromContextInfo
 *
 * This routine should find a viewTreeName given a ContextInfo structure.
 *
 * NOTE: It is not clear how authSnmpID and contextSnmpID values extracted
 *       from the ContextInfo structure or from the communityTable should
 *       be handled.  The correct thing to do may be to check them to
 *       insure that they are equal to the local value of snmpID.
 */
OctetString *
LookupViewFromContextInfo(cip, req_type)
    ContextInfo *cip;
    SR_INT32 req_type;
{
    return NULL;
}
