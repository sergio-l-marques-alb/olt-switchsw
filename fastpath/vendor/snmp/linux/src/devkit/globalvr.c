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
#include "sr_snmp.h"

/*
 *  This file is a place to instantiate global variables.  This
 *  made useful by the great seclib split of 2003.
 */

/*
 *  Maximum packet size.  Used all over the place.
 */

SR_UINT32 sr_pkt_size = SR_DEFAULT_MAX_MSG_SIZE;

/*
 *  global OID Definitions
 */

static const SR_UINT32 usmNoAuthProtocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 1, 1
};
const OID usmNoAuthProtocol = {
    OIDSIZE(usmNoAuthProtocol_sids),
    (SR_UINT32 *)usmNoAuthProtocol_sids
};
#ifndef SR_UNSECURABLE
#ifdef SR_MD5_HASH_ALGORITHM
static const SR_UINT32 usmHMACMD5AuthProtocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 1, 2
};
const OID usmHMACMD5AuthProtocol = {
    OIDSIZE(usmHMACMD5AuthProtocol_sids),
    (SR_UINT32 *)usmHMACMD5AuthProtocol_sids
};
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
static const SR_UINT32 usmHMACSHAAuthProtocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 1, 3
};
const OID usmHMACSHAAuthProtocol = {
    OIDSIZE(usmHMACSHAAuthProtocol_sids),
    (SR_UINT32 *)usmHMACSHAAuthProtocol_sids
};
#endif /* SR_SHA_HASH_ALGORITHM */
#endif /* SR_UNSECURABLE */

static const SR_UINT32 usmNoPrivProtocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 2, 1
};
const OID usmNoPrivProtocol = {
    OIDSIZE(usmNoPrivProtocol_sids),
    (SR_UINT32 *)usmNoPrivProtocol_sids
};
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
static const SR_UINT32 usmDESPrivProtocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 2, 2
};
const OID usmDESPrivProtocol = {
    OIDSIZE(usmDESPrivProtocol_sids),
    (SR_UINT32 *)usmDESPrivProtocol_sids
};

#ifdef SR_3DES

static const SR_UINT32 usm3DESPrivProtocol_sids[] = {
    1, 3, 6, 1, 4, 1, 14832, 1, 1
};
const OID usm3DESPrivProtocol = {
    OIDSIZE(usm3DESPrivProtocol_sids),
    (SR_UINT32 *)usm3DESPrivProtocol_sids
};

#endif /* SR_3DES */

#ifdef SR_AES
static const SR_UINT32 usmAesCfb128Protocol_sids[] = {
    1, 3, 6, 1, 6, 3, 10, 1, 2, 4
};
const OID usmAesCfb128Protocol = {
    OIDSIZE(usmAesCfb128Protocol_sids),
    (SR_UINT32 *)usmAesCfb128Protocol_sids
};

static const SR_UINT32 usmAESCfb128PrivProtocol_sids[] = {
    1, 3, 6, 1, 4, 1, 14832, 1, 2
};
const OID usmAESCfb128PrivProtocol = {
    OIDSIZE(usmAESCfb128PrivProtocol_sids),
    (SR_UINT32 *)usmAESCfb128PrivProtocol_sids
};

static const SR_UINT32 usmAESCfb192PrivProtocol_sids[] = {
    1, 3, 6, 1, 4, 1, 14832, 1, 3
};
const OID usmAESCfb192PrivProtocol = {
    OIDSIZE(usmAESCfb192PrivProtocol_sids),
    (SR_UINT32 *)usmAESCfb192PrivProtocol_sids
};

static const SR_UINT32 usmAESCfb256PrivProtocol_sids[] = {
    1, 3, 6, 1, 4, 1, 14832, 1, 4
};
const OID usmAESCfb256PrivProtocol = {
    OIDSIZE(usmAESCfb256PrivProtocol_sids),
    (SR_UINT32 *)usmAESCfb256PrivProtocol_sids
};

#endif /* SR_AES */

#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

/*
 * Prior to release 16, the _ready method on the k_ file was called
 * once for each VarBind in the incoming request. In release 16.1 and
 * later, the _ready routine is called once per DoList entry. Setting
 * the per_varbind_ready variable to 1 causes the _ready proccessing
 * to behave the way it did prior to 16.1 (called once per VarBind).
 */
int per_varbind_ready = 0;

/* <eof> */
