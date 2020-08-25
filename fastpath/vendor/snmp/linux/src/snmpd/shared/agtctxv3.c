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
#include "sr_vta.h"
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
#include "snmpv2d.h"
#include "agtctx.h"
#include "diag.h"
SR_FILENAME


#include "v3_msg.h"
extern SnmpV2Table agt_vacmContextTable;
extern SnmpV2Table agt_vacmSecurityToGroupTable;
extern SnmpV2Table agt_vacmAccessTable;

#include "snmpv3/v3defs.h"

#ifdef SR_SNMPv3_PACKET
extern SnmpV2Table agt_usmUserTable;
#endif /* SR_SNMPv3_PACKET */


#ifdef SR_SNMPv1_WRAPPER
extern SnmpV2Table agt_snmpCommunityTable;
static snmpCommunityEntry_t *LookupCommunityInfo(
    OctetString *community,
    int *index);
#endif /* SR_SNMPv1_WRAPPER */

static const OctetString default_context = { NULL, 0 };
static const unsigned char v2s_default_context_chars[] = "default";
static const OctetString v2s_default_context = {
    (unsigned char *)v2s_default_context_chars,
    7
};

extern OctetString *agt_local_snmpID;

/*
 * Search for the authorization structure associated with
 * this community (or SNMPv3 user) name.
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
    ContextInfo    *cip = NULL;
    int index;
    vacmContextEntry_t *vce = NULL;
    vacmSecurityToGroupEntry_t *vstge;
#ifdef SR_SNMPv1_WRAPPER
    snmpCommunityEntry_t *sce = NULL;
    SR_INT32 mms;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
    SnmpV3Lcd *snmp_v3_lcd =
        (SnmpV3Lcd *)SrGetVersionLcd(snmp_lcd, SR_SNMPv3_VERSION);
#endif /* SR_SNMPv3_PACKET */
    int tn = 0;
    int slots;
    SR_INT32 max_vbl_size_outgoing, local_limit;


    CopyTransportInfo(&admin_info->srcTI, srcTI);
 
    /* Calculate the maximum varbind list size that we can send */
    local_limit = outLen - snmp_msg->wrapper_size - SR_PDU_WRAPPER_SIZE;

    switch (snmp_msg->version) {
#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
            index = 0;
            while (1) {
                sce = LookupCommunityInfo(snmp_msg->u.v1.community, &index);
                if (sce == NULL) {
                    DPRINTF((APACCESS,
                           "%s: Authentication failure, bad community string\n",
                           Fname));
                    snmp_msg->error_code = SR_SNMP_BAD_COMMUNITY_NAMES;
                    goto failure;
                }
/* NOTE: if we add a runtime default mms value for packet sources,
 * this function should return that value if the transport tag is empty */
                mms =
                    SrValidateTransportAddress(snmp_lcd->vta_data,
                                               snmp_lcd->vta_function, srcTI,
                                               sce->snmpCommunityTransportTag);
                if (mms != 0) {
                    break;
                }
            }
            snmp_msg->max_vbl_size = mms - snmp_msg->wrapper_size -
                                     SR_PDU_WRAPPER_SIZE;
#ifdef SR_SNMPv1_PACKET
            if (snmp_msg->version == SR_SNMPv1_VERSION) {
                admin_info->securityModel = SR_SECURITY_MODEL_V1;
            }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
            if (snmp_msg->version == SR_SNMPv2c_VERSION) {
                admin_info->securityModel = SR_SECURITY_MODEL_V2C;
            }
#endif /* SR_SNMPv2c_PACKET */
            admin_info->securityLevel = SR_SECURITY_LEVEL_NOAUTH;
            admin_info->authSnmpEngineID = agt_local_snmpID;
            admin_info->securityName = sce->snmpCommunitySecurityName;
            admin_info->contextSnmpEngineID = sce->snmpCommunityContextEngineID;
            admin_info->contextName = sce->snmpCommunityContextName;
            admin_info->groupName = NULL;
            break;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            admin_info->securityModel = snmp_msg->u.v3.securityModel;
            admin_info->securityLevel = snmp_msg->u.v3.securityLevel;
            admin_info->authSnmpEngineID = snmp_msg->u.v3.authSnmpEngineID;
            admin_info->securityName = snmp_msg->u.v3.userSecurityName;
            admin_info->contextSnmpEngineID =snmp_msg->u.v3.contextSnmpEngineID;
            admin_info->contextName = snmp_msg->u.v3.contextName;
            admin_info->groupName = NULL;
            break;
#endif /* SR_SNMPv3_PACKET */
        default:
            goto failure;
    }

    /* The real maximum varbind list length is the minimum of:
     *    - the maximum varbind list length we can send
     *    - the maximum varbind list length the receiver can receive
     */
    max_vbl_size_outgoing = MIN(snmp_msg->max_vbl_size, local_limit);
    if (max_vbl_size_outgoing < 0) {
        *limit = 0;
    } else {
        *limit = (SR_UINT32)max_vbl_size_outgoing;
    }

    /* Check if this is a local context */
    if (CmpOctetStrings(snmp_v3_lcd->snmpEngineData->snmpEngineID,
                        admin_info->contextSnmpEngineID)) {
        snmp_msg->error_code = SR_SNMP_UNKNOWN_PDU_HANDLERS;
        goto failure;
    }

    /* Check if the local context exists */

     agt_vacmContextTable.tip[0].value.octet_val = admin_info->contextName;
     index = SearchTable(&agt_vacmContextTable, EXACT);
     if (index == -1) {
         snmp_msg->error_code = UNKNOWN_CONTEXT_ERROR;
         goto failure;
     }
     vce = (vacmContextEntry_t *)agt_vacmContextTable.tp[index];




    /* Determine the group name */
    agt_vacmSecurityToGroupTable.tip[0].value.uint_val =
        admin_info->securityModel;
    agt_vacmSecurityToGroupTable.tip[1].value.octet_val =
        admin_info->securityName;
    index = SearchTable(&agt_vacmSecurityToGroupTable, EXACT);
    if (index != -1) {
        vstge = (vacmSecurityToGroupEntry_t *)
            agt_vacmSecurityToGroupTable.tp[index];
        if (vstge->vacmSecurityToGroupStatus !=
                D_vacmSecurityToGroupStatus_active) {
            snmp_msg->error_code = SR_NO_GROUP_ERROR;
            goto failure;
        }
        admin_info->groupName = vstge->vacmGroupName;
    } else {
          snmp_msg->error_code = SR_NO_GROUP_ERROR;
          goto failure;
    }
    
    slots = 17;

    switch (snmp_msg->version) {
#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
#endif /* SR_SNMPv2c_PACKET */
            cip = AllocContextInfo((SR_UINT32)(slots + 1));
            if (cip == NULL) {
                goto failure;
            }
            cip->u.new_context_info.tv[tn].tag = NCI_COMMUNITY;
            cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
            cip->u.new_context_info.tv[tn].value.os.os_value =
                CloneOctetString(snmp_msg->u.v1.community);
            tn++;
            break;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            cip = AllocContextInfo((SR_UINT32)slots);
            if (cip == NULL) {
                goto failure;
            }
            break;
#endif /* SR_SNMPv3_PACKET */
        default:
            goto failure;
    }

    cip->u.new_context_info.tv[tn].tag = NCI_SECURITY_MODEL;
    cip->u.new_context_info.tv[tn].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[tn].value.os.sl_value =
        admin_info->securityModel;
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_SECURITY_LEVEL;
    cip->u.new_context_info.tv[tn].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[tn].value.os.sl_value =
        admin_info->securityLevel;
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_SENDER_TRANSPORT;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    switch (srcTI->type) {
	default:
            cip->u.new_context_info.tv[tn].value.os.os_value =
            MakeOctetString((unsigned char *)&srcTI->t_ipAddr, 4);
            tn++;
            break;
    }

    cip->u.new_context_info.tv[tn].tag = NCI_RECEIVER_TRANSPORT;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    switch (dstTI->type) {
        default:
            cip->u.new_context_info.tv[tn].value.os.os_value =
                  MakeOctetString((unsigned char *)&dstTI->t_ipAddr, 4);
            tn++;
            break;
    }

    cip->u.new_context_info.tv[tn].tag = NCI_MESSAGE_PROCESSING_MODEL;
    cip->u.new_context_info.tv[tn].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[tn].value.os.sl_value = snmp_msg->version;
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_AUTH_SNMP_ENGINE_ID;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    cip->u.new_context_info.tv[tn].value.os.os_value =
        CloneOctetString(admin_info->authSnmpEngineID);
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_SECURITY_NAME;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    cip->u.new_context_info.tv[tn].value.os.os_value =
        CloneOctetString(admin_info->securityName);
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_CONTEXT_SNMP_ENGINE_ID;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    cip->u.new_context_info.tv[tn].value.os.os_value =
        CloneOctetString(admin_info->contextSnmpEngineID);
    tn++;

    cip->u.new_context_info.tv[tn].tag = NCI_CONTEXT_NAME;
    cip->u.new_context_info.tv[tn].value.os.type = OCTET_PRIM_TYPE;
    cip->u.new_context_info.tv[tn].value.os.os_value =
        CloneOctetString(admin_info->contextName);
    tn++;
    
    /* add a place-holder for request-id */
    cip->u.new_context_info.tv[tn].tag = NCI_REQUEST_ID;
    cip->u.new_context_info.tv[tn].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[tn].value.os.sl_value = 0;
    tn++;

    /* add a place-holder for MMS remaining */
    cip->u.new_context_info.tv[tn].tag = NCI_MMS_REMAINING;
    cip->u.new_context_info.tv[tn].value.os.type = INTEGER_TYPE;
    cip->u.new_context_info.tv[tn].value.os.sl_value = *limit;
    tn++;



    return cip;

  failure:
    /* Just to be safe, make sure *limit is really set */
    max_vbl_size_outgoing = MIN(snmp_msg->max_vbl_size, local_limit);
    if (max_vbl_size_outgoing < 0) {
        *limit = 0;
    } else {
        *limit = (SR_UINT32)max_vbl_size_outgoing;
    }
    if (cip) {
        FreeContextInfo(cip);
    }
    return NULL;
}                                /* findContextInfo() */

/*
 * LookupViewFromContextInfo
 *
 * This routine should find a viewTreeName given a ContextInfo structure.
 *
 * NOTE: For a packet which was received and processing normally, the
 *       authSnmpID and contextSnmpID values must be the local values,
 *       and so they need not be checked.  If the ContextInfo structure
 *       was faked by an agent/subagent, the values are not meaningful,
 *       and so they can be ignored.
 */
OctetString *
LookupViewFromContextInfo(cip, req_type)
    ContextInfo *cip;
    SR_INT32 req_type;
{
    SR_INT32 mpm;
    ObjectSyntax *os;
    SR_INT32 securityModel;
    SR_INT32 securityLevel;
    OctetString *authSnmpEngineID, *securityName;
    OctetString *contextSnmpEngineID, *contextName;
    OctetString *groupName = NULL;
#ifdef SR_SNMPv1_WRAPPER
    snmpCommunityEntry_t *sce;
#endif	/* SR_SNMPv1_WRAPPER */
    vacmSecurityToGroupEntry_t *vstge;
    vacmAccessEntry_t *vae;
    int index;

    os = SearchContextInfo(cip, NCI_MESSAGE_PROCESSING_MODEL);
    if (os == NULL) {
        return NULL;
    }
    mpm = os->sl_value;

    os = SearchContextInfo(cip, NCI_SECURITY_MODEL);
    if (os == NULL) {
        return NULL;
    }
    securityModel = os->sl_value;

    os = SearchContextInfo(cip, NCI_SECURITY_LEVEL);
    if (os == NULL) {
        return NULL;
    }
    securityLevel = os->sl_value;

    os = SearchContextInfo(cip, NCI_AUTH_SNMP_ENGINE_ID);
    if (os == NULL) {
        authSnmpEngineID = agt_local_snmpID;
    } else {
        authSnmpEngineID = os->os_value;
    }

    os = SearchContextInfo(cip, NCI_SECURITY_NAME);
    if (os == NULL) {
        return NULL;
    }
    securityName = os->os_value;

    os = SearchContextInfo(cip, NCI_CONTEXT_SNMP_ENGINE_ID);
    if (os == NULL) {
        contextSnmpEngineID = agt_local_snmpID;
    } else {
        contextSnmpEngineID = os->os_value;
    }

    os = SearchContextInfo(cip, NCI_CONTEXT_NAME);
    if (os == NULL) {
        return NULL;
    }
    contextName = os->os_value;

    switch (mpm) {

#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_MESSAGE_PROCESSING_MODEL:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_MESSAGE_PROCESSING_MODEL:
#endif /* SR_SNMPv2c_PACKET */
            /* should make sure an appropriate entry really exists in
             * the snmpCommunityTable */
            for (index = 0; index < agt_snmpCommunityTable.nitems; index++) {
                sce = (snmpCommunityEntry_t *)agt_snmpCommunityTable.tp[index];
                if (sce->snmpCommunityStatus != RS_ACTIVE) {
                   continue;
                }
                if (CmpOctetStrings(securityName,
                                    sce->snmpCommunitySecurityName)) {
                    continue;
                }
                if (CmpOctetStrings(contextSnmpEngineID,
                                    sce->snmpCommunityContextEngineID)) {
                    continue;
                }
                if (CmpOctetStrings(contextName,
                                    sce->snmpCommunityContextName)) {
                    continue;
                }
                break;
            }
            if (index == agt_snmpCommunityTable.nitems) {
                return NULL;
            }
            break;
#endif /* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_MESSAGE_PROCESSING_MODEL:
            if (securityModel == SR_SECURITY_MODEL_USM) {
                /*
                 *  The usmUserTable needs to be consulted only for the USM
                 *  security model.  For example, if the security model is
                 *  CLI, XML, or EPIC, then there is no need to check the
                 *  usmUserTable for that security name.
                 */
                agt_usmUserTable.tip[0].value.octet_val = authSnmpEngineID;
                agt_usmUserTable.tip[1].value.octet_val = securityName;
                if (SearchTable(&agt_usmUserTable, EXACT) == -1) {
                    return NULL;
                }
            }
            break;
#endif /* SR_SNMPv3_PACKET */


        default:
            return NULL;
    }

    /* Determine the group name */
    agt_vacmSecurityToGroupTable.tip[0].value.uint_val = securityModel;
    agt_vacmSecurityToGroupTable.tip[1].value.octet_val = securityName;
    index = SearchTable(&agt_vacmSecurityToGroupTable, EXACT);
    if (index != -1) {
        vstge = (vacmSecurityToGroupEntry_t *)
            agt_vacmSecurityToGroupTable.tp[index];
        if (vstge->vacmSecurityToGroupStatus !=
                D_vacmSecurityToGroupStatus_active) {
            return NULL;
        }
        groupName = vstge->vacmGroupName;
    }
    if (groupName == NULL) {
        return NULL;
    }

    vae = LookupVacmAccessEntryWithMask(groupName,
                                        contextName,
                                        securityModel,
                                        securityLevel);
    if (vae == NULL) {
        return NULL;
    }
    switch (req_type) {
        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:
        case GET_BULK_REQUEST_TYPE:
            return vae->vacmAccessReadViewName;

        case SET_REQUEST_TYPE:
            return vae->vacmAccessWriteViewName;

        case TRAP_TYPE:
        case SNMPv2_TRAP_TYPE:
            return vae->vacmAccessNotifyViewName;
    }
    return NULL;
}

vacmAccessEntry_t *
LookupVacmAccessEntryWithMask(
    OctetString *groupName,
    OctetString *contextName,
    SR_INT32 securityModel,
    SR_INT32 securityLevel)
{
    vacmAccessEntry_t *vae, *s_vae = NULL;
    int index, got_exact = 0;
    OctetString eos = {NULL, 0};
    OctetString cn;
    SR_INT32 save_len;

    if (contextName == NULL) {
        cn.octet_ptr = NULL;
        cn.length = 0;
    } else {
        cn.octet_ptr = contextName->octet_ptr;
        cn.length = contextName->length;
    }

    if (groupName == NULL) {
         return NULL;
    }

    agt_vacmAccessTable.tip[0].value.octet_val = groupName;
    agt_vacmAccessTable.tip[1].value.octet_val = &eos;
    agt_vacmAccessTable.tip[2].value.uint_val = 0;
    agt_vacmAccessTable.tip[3].value.uint_val = 0;
    index = SearchTable(&agt_vacmAccessTable, NEXT);
    if (index == -1) {
        return NULL;
    }
    for (; index < agt_vacmAccessTable.nitems; index++) {
        vae = (vacmAccessEntry_t *)agt_vacmAccessTable.tp[index];
        if (vae->vacmAccessStatus != D_vacmAccessStatus_active) {
            continue;
        }
        if (CmpOctetStrings(groupName, vae->vacmGroupName)) {
            break;
        }
        if (vae->vacmAccessSecurityModel != securityModel) {
            continue;
        }
        if (vae->vacmAccessSecurityLevel > securityLevel) {
            continue;
        }
        if (vae->vacmAccessContextMatch == 1) {
            if (CmpOctetStrings(vae->vacmAccessContextPrefix, &cn)) {
                continue;
            }
            if (s_vae != NULL) {
                if (s_vae->vacmAccessSecurityLevel >
                        vae->vacmAccessSecurityLevel) {
                    continue;
                }
            }
            s_vae = vae;
            got_exact = 1;
        } else {
            if (got_exact) {
                continue;
            }
            if (vae->vacmAccessContextPrefix->length > cn.length) {
                continue;
            }
            save_len = cn.length;
            cn.length = vae->vacmAccessContextPrefix->length;
            if (CmpOctetStrings(vae->vacmAccessContextPrefix, &cn)) {
                cn.length = save_len;
                continue;
            }
            cn.length = save_len;
            if (s_vae != NULL) {
                if (s_vae->vacmAccessContextPrefix->length >
                        vae->vacmAccessContextPrefix->length) {
                    continue;
                }
                if (s_vae->vacmAccessContextPrefix->length ==
                        vae->vacmAccessContextPrefix->length) {
                    if (s_vae->vacmAccessSecurityLevel >
                            vae->vacmAccessSecurityLevel) {
                        continue;
                    }
                }
            }
            s_vae = vae;
        }
    }

    return s_vae;
}

#ifdef SR_SNMPv1_WRAPPER
extern SnmpV2Table agt_snmpCommunityTable;

static snmpCommunityEntry_t *
LookupCommunityInfo(
    OctetString *community,
    int *index)
{
    snmpCommunityEntry_t *sce;

    if (community == NULL) {
        return NULL;
    }

    for (; (*index) < agt_snmpCommunityTable.nitems; (*index)++) {
        sce = (snmpCommunityEntry_t *)agt_snmpCommunityTable.tp[(*index)];
        if (sce->snmpCommunityStatus != RS_ACTIVE) {
            continue;
        }
        if (CmpOctetStrings(community, sce->snmpCommunityName) == 0) {
            (*index)++;
            return sce;
        }
    }
    return NULL;
}
#endif /* SR_SNMPv1_WRAPPER */
