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
 */


/*
 * Arguments used to create this file:
 * -apo -f mibcompflags -userpart -v_stubs_only 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 ip_last_sid_array[] = {
    1, /* ipForwarding */
    2, /* ipDefaultTTL */
    3, /* ipInReceives */
    4, /* ipInHdrErrors */
    5, /* ipInAddrErrors */
    6, /* ipForwDatagrams */
    7, /* ipInUnknownProtos */
    8, /* ipInDiscards */
    9, /* ipInDelivers */
    10, /* ipOutRequests */
    11, /* ipOutDiscards */
    12, /* ipOutNoRoutes */
    13, /* ipReasmTimeout */
    14, /* ipReasmReqds */
    15, /* ipReasmOKs */
    16, /* ipReasmFails */
    17, /* ipFragOKs */
    18, /* ipFragFails */
    19, /* ipFragCreates */
    23  /* ipRoutingDiscards */
};

const SnmpType ipTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ip_t, ipForwarding), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ip_t, ipDefaultTTL), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInReceives), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInHdrErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInAddrErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipForwDatagrams), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInUnknownProtos), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInDiscards), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipInDelivers), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipOutRequests), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipOutDiscards), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipOutNoRoutes), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipReasmTimeout), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipReasmReqds), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipReasmOKs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipReasmFails), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipFragOKs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipFragFails), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipFragCreates), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ip_t, ipRoutingDiscards), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry ipGetInfo = {
    (SR_KGET_FPTR) new_k_ip_get,
#ifndef U_ip
    (SR_FREE_FPTR) NULL,
#else /* U_ip */
    (SR_FREE_FPTR) k_ipFreeUserpartData,
#endif /* U_ip */
    (int) sizeof(ip_t),
    I_ip_max,
    (SnmpType *) ipTypeTable,
    NULL,
    (short) offsetof(ip_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the ip family.
 *---------------------------------------------------------------------*/
VarBind *
ip_get(OID *incoming, ObjectInfo *object, int searchType,
       ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &ipGetInfo));
}

ip_t *
new_k_ip_get(int serialNum, ContextInfo *contextInfo,
             int nominator, int searchType,
             ip_t *data)
{
    return k_ip_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int ip_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the ip family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   ipForwarding_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * ip family.
 */
static struct sr_member_test ip_member_test[] =
{
    /* ipForwarding */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(ipForwarding_range)/sizeof(RangeTest_t), /* 2 */
      ipForwarding_range, NULL },

    /* ipDefaultTTL */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipInReceives */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipInHdrErrors */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipInAddrErrors */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipForwDatagrams */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipInUnknownProtos */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipInDiscards */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipInDelivers */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipOutRequests */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipOutDiscards */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipOutNoRoutes */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipReasmTimeout */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipReasmReqds */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipReasmOKs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipReasmFails */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipFragOKs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipFragFails */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipFragCreates */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipRoutingDiscards */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the ip family.
 */
const SrTestInfoEntry ipTestInfo = {
    &ipGetInfo,
    ip_member_test,
    NULL,
    k_ip_test,
    k_ip_ready,
#ifdef SR_ip_UNDO
    ip_undo,
#else /* SR_ip_UNDO */
    NULL,
#endif /* SR_ip_UNDO */
    ip_ready,
    ip_set,
    ip_cleanup,
#ifndef U_ip
    (SR_COPY_FPTR) NULL
#else /* U_ip */
    (SR_COPY_FPTR) k_ipCloneUserpartData
#endif /* U_ip */
};

/*----------------------------------------------------------------------
 * cleanup after ip set/undo
 *---------------------------------------------------------------------*/
static int
ip_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &ipTestInfo);
}

#ifdef SR_ip_UNDO
/*----------------------------------------------------------------------
 * clone the ip family
 *---------------------------------------------------------------------*/
ip_t *
Clone_ip(ip_t *ip)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (ip_t *)SrCloneFamily(ip,
                         ipGetInfo.family_size,
                         ipGetInfo.type_table,
                         ipGetInfo.highest_nominator,
                         ipGetInfo.valid_offset,
                         ipTestInfo.userpart_clone_func,
                         ipGetInfo.userpart_free_func);
}

#endif /* defined(SR_ip_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
ip_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
        doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &ipTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
ip_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &ipTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ip_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_ip_set((ip_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 ipAddrEntry_last_sid_array[] = {
    1, /* ipAdEntAddr */
    2, /* ipAdEntIfIndex */
    3, /* ipAdEntNetMask */
    4, /* ipAdEntBcastAddr */
    5  /* ipAdEntReasmMaxSize */
};

const SnmpType ipAddrEntryTypeTable[] = {
    { IP_ADDR_PRIM_TYPE, SR_READ_ONLY, offsetof(ipAddrEntry_t, ipAdEntAddr), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ipAddrEntry_t, ipAdEntIfIndex), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_ONLY, offsetof(ipAddrEntry_t, ipAdEntNetMask), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ipAddrEntry_t, ipAdEntBcastAddr), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ipAddrEntry_t, ipAdEntReasmMaxSize), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo ipAddrEntryIndexInfo[] = {
#ifdef I_ipAdEntAddr
    { I_ipAdEntAddr, T_uint, -1 },
#endif /* I_ipAdEntAddr */
    { -1, -1, -1 }
};

const SrGetInfoEntry ipAddrEntryGetInfo = {
    (SR_KGET_FPTR) new_k_ipAddrEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(ipAddrEntry_t),
    I_ipAddrEntry_max,
    (SnmpType *) ipAddrEntryTypeTable,
    ipAddrEntryIndexInfo,
    (short) offsetof(ipAddrEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the ipAddrEntry family.
 *---------------------------------------------------------------------*/
VarBind *
ipAddrEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_ipAdEntAddr) )
    return NULL;
#else	/* ! ( defined(I_ipAdEntAddr) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &ipAddrEntryGetInfo));
#endif	/* ! ( defined(I_ipAdEntAddr) ) */
}




ipAddrEntry_t *
new_k_ipAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator, int searchType,
                      ipAddrEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_ipAddrEntry_get(serialNum, contextInfo, nominator,
                             searchType, data->ipAdEntAddr);
}


static const SR_UINT32 ipRouteEntry_last_sid_array[] = {
    1, /* ipRouteDest */
    2, /* ipRouteIfIndex */
    3, /* ipRouteMetric1 */
    4, /* ipRouteMetric2 */
    5, /* ipRouteMetric3 */
    6, /* ipRouteMetric4 */
    7, /* ipRouteNextHop */
    8, /* ipRouteType */
    9, /* ipRouteProto */
    10, /* ipRouteAge */
    11, /* ipRouteMask */
    12, /* ipRouteMetric5 */
    13  /* ipRouteInfo */
};

const SnmpType ipRouteEntryTypeTable[] = {
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteDest), 0 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteIfIndex), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMetric1), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMetric2), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMetric3), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMetric4), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteNextHop), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteType), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ipRouteEntry_t, ipRouteProto), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteAge), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMask), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipRouteEntry_t, ipRouteMetric5), -1 },
    { OBJECT_ID_TYPE, SR_READ_ONLY, offsetof(ipRouteEntry_t, ipRouteInfo), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo ipRouteEntryIndexInfo[] = {
#ifdef I_ipRouteDest
    { I_ipRouteDest, T_uint, -1 },
#endif /* I_ipRouteDest */
    { -1, -1, -1 }
};

const SrGetInfoEntry ipRouteEntryGetInfo = {
    (SR_KGET_FPTR) new_k_ipRouteEntry_get,
#ifndef U_ipRouteEntry
    (SR_FREE_FPTR) NULL,
#else /* U_ipRouteEntry */
    (SR_FREE_FPTR) k_ipRouteEntryFreeUserpartData,
#endif /* U_ipRouteEntry */
    (int) sizeof(ipRouteEntry_t),
    I_ipRouteEntry_max,
    (SnmpType *) ipRouteEntryTypeTable,
    ipRouteEntryIndexInfo,
    (short) offsetof(ipRouteEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the ipRouteEntry family.
 *---------------------------------------------------------------------*/
VarBind *
ipRouteEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_ipRouteDest) )
    return NULL;
#else	/* ! ( defined(I_ipRouteDest) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &ipRouteEntryGetInfo));
#endif	/* ! ( defined(I_ipRouteDest) ) */
}

ipRouteEntry_t *
new_k_ipRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       ipRouteEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_ipRouteEntry_get(serialNum, contextInfo, nominator,
                              searchType, data->ipRouteDest);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int ipRouteEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the ipRouteEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   ipRouteType_range[] = { { 1, 4 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * ipRouteEntry family.
 */
static struct sr_member_test ipRouteEntry_member_test[] =
{
    /* ipRouteDest */
    { MINV_NO_TEST, 0, NULL, Identity_test },

    /* ipRouteIfIndex */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMetric1 */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMetric2 */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMetric3 */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMetric4 */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteNextHop */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(ipRouteType_range)/sizeof(RangeTest_t), /* 4 */
      ipRouteType_range, NULL },

    /* ipRouteProto */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ipRouteAge */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMask */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteMetric5 */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipRouteInfo */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the ipRouteEntry family.
 */
const SrTestInfoEntry ipRouteEntryTestInfo = {
    &ipRouteEntryGetInfo,
    ipRouteEntry_member_test,
#ifdef ipRouteEntry_READ_CREATE
    k_ipRouteEntry_set_defaults,
#else /* ipRouteEntry_READ_CREATE */
    NULL,
#endif /* ipRouteEntry_READ_CREATE */
    k_ipRouteEntry_test,
    k_ipRouteEntry_ready,
#ifdef SR_ipRouteEntry_UNDO
    ipRouteEntry_undo,
#else /* SR_ipRouteEntry_UNDO */
    NULL,
#endif /* SR_ipRouteEntry_UNDO */
    ipRouteEntry_ready,
    ipRouteEntry_set,
    ipRouteEntry_cleanup,
#ifndef U_ipRouteEntry
    (SR_COPY_FPTR) NULL
#else /* U_ipRouteEntry */
    (SR_COPY_FPTR) k_ipRouteEntryCloneUserpartData
#endif /* U_ipRouteEntry */
};

/*----------------------------------------------------------------------
 * Free the ipRouteEntry data object.
 *---------------------------------------------------------------------*/
void
ipRouteEntry_free(ipRouteEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 ipRouteEntryGetInfo.type_table,
                 ipRouteEntryGetInfo.highest_nominator,
                 ipRouteEntryGetInfo.valid_offset,
                 ipRouteEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after ipRouteEntry set/undo
 *---------------------------------------------------------------------*/
static int
ipRouteEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &ipRouteEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the ipRouteEntry family
 *---------------------------------------------------------------------*/
ipRouteEntry_t *
Clone_ipRouteEntry(ipRouteEntry_t *ipRouteEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (ipRouteEntry_t *)SrCloneFamily(ipRouteEntry,
                         ipRouteEntryGetInfo.family_size,
                         ipRouteEntryGetInfo.type_table,
                         ipRouteEntryGetInfo.highest_nominator,
                         ipRouteEntryGetInfo.valid_offset,
                         ipRouteEntryTestInfo.userpart_clone_func,
                         ipRouteEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
ipRouteEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &ipRouteEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
ipRouteEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &ipRouteEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ipRouteEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_ipRouteEntry_set((ipRouteEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


static const SR_UINT32 ipNetToMediaEntry_last_sid_array[] = {
    1, /* ipNetToMediaIfIndex */
    2, /* ipNetToMediaPhysAddress */
    3, /* ipNetToMediaNetAddress */
    4  /* ipNetToMediaType */
};

const SnmpType ipNetToMediaEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipNetToMediaEntry_t, ipNetToMediaIfIndex), 0 },
    { OCTET_PRIM_TYPE, SR_READ_WRITE, offsetof(ipNetToMediaEntry_t, ipNetToMediaPhysAddress), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_WRITE, offsetof(ipNetToMediaEntry_t, ipNetToMediaNetAddress), 1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ipNetToMediaEntry_t, ipNetToMediaType), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo ipNetToMediaEntryIndexInfo[] = {
#ifdef I_ipNetToMediaIfIndex
    { I_ipNetToMediaIfIndex, T_uint, -1 },
#endif /* I_ipNetToMediaIfIndex */
#ifdef I_ipNetToMediaNetAddress
    { I_ipNetToMediaNetAddress, T_uint, -1 },
#endif /* I_ipNetToMediaNetAddress */
    { -1, -1, -1 }
};

const SrGetInfoEntry ipNetToMediaEntryGetInfo = {
    (SR_KGET_FPTR) new_k_ipNetToMediaEntry_get,
#ifndef U_ipNetToMediaEntry
    (SR_FREE_FPTR) NULL,
#else /* U_ipNetToMediaEntry */
    (SR_FREE_FPTR) k_ipNetToMediaEntryFreeUserpartData,
#endif /* U_ipNetToMediaEntry */
    (int) sizeof(ipNetToMediaEntry_t),
    I_ipNetToMediaEntry_max,
    (SnmpType *) ipNetToMediaEntryTypeTable,
    ipNetToMediaEntryIndexInfo,
    (short) offsetof(ipNetToMediaEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the ipNetToMediaEntry family.
 *---------------------------------------------------------------------*/
VarBind *
ipNetToMediaEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                      ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_ipNetToMediaIfIndex) ) || ! ( defined(I_ipNetToMediaNetAddress) ))
    return NULL;
#else	/* (! ( defined(I_ipNetToMediaIfIndex) ) || ! ( defined(I_ipNetToMediaNetAddress) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &ipNetToMediaEntryGetInfo));
#endif	/* (! ( defined(I_ipNetToMediaIfIndex) ) || ! ( defined(I_ipNetToMediaNetAddress) )) */
}

ipNetToMediaEntry_t *
new_k_ipNetToMediaEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator, int searchType,
                            ipNetToMediaEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_ipNetToMediaEntry_get(serialNum, contextInfo, nominator,
                                   searchType, data->ipNetToMediaIfIndex,
                                   data->ipNetToMediaNetAddress);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int ipNetToMediaEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the ipNetToMediaEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   ipNetToMediaType_range[] = { { 1, 4 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * ipNetToMediaEntry family.
 */
static struct sr_member_test ipNetToMediaEntry_member_test[] =
{
    /* ipNetToMediaIfIndex */
    { MINV_NO_TEST, 0, NULL, Identity_test },

    /* ipNetToMediaPhysAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ipNetToMediaNetAddress */
    { MINV_NO_TEST, 0, NULL, Identity_test },

    /* ipNetToMediaType */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(ipNetToMediaType_range)/sizeof(RangeTest_t), /* 4 */
      ipNetToMediaType_range, NULL }
};


/*
 * Initialize SrTestInfoEntry for the ipNetToMediaEntry family.
 */
const SrTestInfoEntry ipNetToMediaEntryTestInfo = {
    &ipNetToMediaEntryGetInfo,
    ipNetToMediaEntry_member_test,
#ifdef ipNetToMediaEntry_READ_CREATE
    k_ipNetToMediaEntry_set_defaults,
#else /* ipNetToMediaEntry_READ_CREATE */
    NULL,
#endif /* ipNetToMediaEntry_READ_CREATE */
    k_ipNetToMediaEntry_test,
    k_ipNetToMediaEntry_ready,
#ifdef SR_ipNetToMediaEntry_UNDO
    ipNetToMediaEntry_undo,
#else /* SR_ipNetToMediaEntry_UNDO */
    NULL,
#endif /* SR_ipNetToMediaEntry_UNDO */
    ipNetToMediaEntry_ready,
    ipNetToMediaEntry_set,
    ipNetToMediaEntry_cleanup,
#ifndef U_ipNetToMediaEntry
    (SR_COPY_FPTR) NULL
#else /* U_ipNetToMediaEntry */
    (SR_COPY_FPTR) k_ipNetToMediaEntryCloneUserpartData
#endif /* U_ipNetToMediaEntry */
};

/*----------------------------------------------------------------------
 * Free the ipNetToMediaEntry data object.
 *---------------------------------------------------------------------*/
void
ipNetToMediaEntry_free(ipNetToMediaEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 ipNetToMediaEntryGetInfo.type_table,
                 ipNetToMediaEntryGetInfo.highest_nominator,
                 ipNetToMediaEntryGetInfo.valid_offset,
                 ipNetToMediaEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after ipNetToMediaEntry set/undo
 *---------------------------------------------------------------------*/
static int
ipNetToMediaEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &ipNetToMediaEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the ipNetToMediaEntry family
 *---------------------------------------------------------------------*/
ipNetToMediaEntry_t *
Clone_ipNetToMediaEntry(ipNetToMediaEntry_t *ipNetToMediaEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (ipNetToMediaEntry_t *)SrCloneFamily(ipNetToMediaEntry,
                         ipNetToMediaEntryGetInfo.family_size,
                         ipNetToMediaEntryGetInfo.type_table,
                         ipNetToMediaEntryGetInfo.highest_nominator,
                         ipNetToMediaEntryGetInfo.valid_offset,
                         ipNetToMediaEntryTestInfo.userpart_clone_func,
                         ipNetToMediaEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
ipNetToMediaEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &ipNetToMediaEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
ipNetToMediaEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &ipNetToMediaEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ipNetToMediaEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_ipNetToMediaEntry_set((ipNetToMediaEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


