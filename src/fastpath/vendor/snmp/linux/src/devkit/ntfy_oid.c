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

#include <sys/types.h>


#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "ntfy_oid.h"

static const SR_UINT32 coldStart_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 1 };
static const OID coldStart_OID = {
    OIDSIZE(coldStart_sids),
    (SR_UINT32 *)coldStart_sids
};
const OID *coldStartTrap = &coldStart_OID;

static const SR_UINT32 warmStart_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 2 };
static const OID warmStart_OID = {
    OIDSIZE(warmStart_sids),
    (SR_UINT32 *)warmStart_sids
};
const OID *warmStartTrap = &warmStart_OID;

static const SR_UINT32 linkDown_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 3 };
static const OID linkDown_OID = {
    OIDSIZE(linkDown_sids),
    (SR_UINT32 *)linkDown_sids
};
const OID *linkDownTrap = &linkDown_OID;

static const SR_UINT32 linkUp_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 4 };
static const OID linkUp_OID = {
    OIDSIZE(linkUp_sids),
    (SR_UINT32 *)linkUp_sids
};
const OID *linkUpTrap = &linkUp_OID;

static const SR_UINT32 authenFailure_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 5 };
static const OID authenFailure_OID = {
    OIDSIZE(authenFailure_sids),
    (SR_UINT32 *)authenFailure_sids
};
const OID *authenFailureTrap = &authenFailure_OID;

static const SR_UINT32 egpNeighborLoss_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 5, 6 };
static const OID egpNeighborLoss_OID = {
    OIDSIZE(egpNeighborLoss_sids),
    (SR_UINT32 *)egpNeighborLoss_sids
};
const OID *egpNeighborLossTrap = &egpNeighborLoss_OID;

static const SR_UINT32 sysUpTime_sids[] =
    { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
const OID sysUpTime_OID = {
    OIDSIZE(sysUpTime_sids),
    (SR_UINT32 *) sysUpTime_sids
};

static const SR_UINT32 snmpTrapOID_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
const OID snmpTrapOID_OID = {
    OIDSIZE(snmpTrapOID_sids),
    (SR_UINT32 *) snmpTrapOID_sids
};

static const SR_UINT32 snmpTrapEnterprise_sids[] =
    { 1, 3, 6, 1, 6, 3, 1, 1, 4, 3, 0 };
const OID snmpTrapEnterprise_OID = {
    OIDSIZE(snmpTrapEnterprise_sids),
    (SR_UINT32 *) snmpTrapEnterprise_sids
};

static SR_UINT32 snmpTrapAddress_sids[] = 
    { 1, 3, 6, 1, 6, 3, 18, 1, 3, 0};
const OID snmpTrapAddress_OID = { 
    OIDSIZE(snmpTrapAddress_sids),
    (SR_UINT32 *)snmpTrapAddress_sids
};

static SR_UINT32 snmpTrapCommunity_sids[] = 
    { 1, 3, 6, 1, 6, 3, 18, 1, 4, 0};
const OID snmpTrapCommunity_OID = { 
    OIDSIZE(snmpTrapCommunity_sids),
    (SR_UINT32 *)snmpTrapCommunity_sids
};

static SR_UINT32 openViewSourceName_sids[] =
    { 1, 3, 6, 1, 4, 1, 11, 2, 17, 2, 2, 0 };
const OID openViewSourceName_OID = {
    OIDSIZE(openViewSourceName_sids),
    (SR_UINT32 *)openViewSourceName_sids
};

