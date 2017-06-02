/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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
 */


#ifndef SR_V3INIT_H
#define SR_V3INIT_H


#ifdef __cplusplus
extern "C" {
#endif

int k_vacmAccessEntry_initialize(
    SnmpV2Table *vat);

int k_ac_initialize(
    SnmpV2Table *vat);

int k_snmpEngine_initialize(
    char *contextName_text,
    snmpEngine_t *se);

int k_arch_initialize(
    char *contextName_text,
    snmpEngine_t *se);

int k_vacmContextEntry_initialize(
    SnmpV2Table *vct);

int k_ctx_initialize(
    SnmpV2Table *vct);

int k_vacmSecurityToGroupEntry_initialize(
    SnmpV2Table *vstgt);

int k_group_initialize(
    SnmpV2Table *vstgt);

int k_snmpMPDStats_initialize(
    snmpMPDStats_t *sms);

int k_mpd_initialize(
    snmpMPDStats_t *sms);

int k_snmpNotifyEntry_initialize(
    SnmpV2Table *snt);

int k_snmpNotifyFilterProfileEntry_initialize(
    SnmpV2Table *snfpt);

int k_snmpNotifyFilterEntry_initialize(
    SnmpV2Table *snft);

int k_notif_initialize(
    SnmpV2Table *snt,
    SnmpV2Table *snfpt,
    SnmpV2Table *snft);

int k_snmpProxyEntry_initialize(
    SnmpV2Table *spt);

int k_proxy_initialize(
    SnmpV2Table *spt);

int k_snmpTargetObjects_initialize(
    snmpTargetObjects_t *sto);

int k_snmpTargetAddrEntry_initialize(
    SnmpV2Table *stat);

int k_snmpTargetParamsEntry_initialize(
    SnmpV2Table *stpt);

int k_tgt_initialize(
    snmpTargetObjects_t *sto,
    SnmpV2Table *stat,
    SnmpV2Table *stpt);

int k_usmStats_initialize(
    char *contextName_text,
    usmStats_t *us);

int k_usmUser_initialize(
    char *contextName_text,
    usmUser_t *uu);

int k_usmUserEntry_initialize(
    char *contextName_text,
    SnmpV2Table *uut);

int k_usm_initialize(
    char *contextName_text,
    usmStats_t *us,
    usmUser_t *uu,
    SnmpV2Table *uut);

int k_vacmMIBViews_initialize(
    vacmMIBViews_t *vmv);

int k_vacmViewTreeFamilyEntry_initialize(
    SnmpV2Table *vvtft);

int k_view_initialize(
    vacmMIBViews_t *vmv,
    SnmpV2Table *vvtft);

#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_V3INIT_H */
