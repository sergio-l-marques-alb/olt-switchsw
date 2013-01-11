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

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "sitedefs.h"
#include "diag.h"
SR_FILENAME

#ifdef SETS

#ifdef OLD_CODE /* LVL7 */
int
k_ifEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    switch (object->nominator) {
#ifdef I_ifAdminStatus
    case I_ifAdminStatus:
#ifndef IF_TESTING_SUPPORTED
        if (value->sl_value == D_ifAdminStatus_testing) {
            return (WRONG_VALUE_ERROR);
        }
#endif
#ifdef IF_ALWAYS_UP
        if (value->sl_value == D_ifAdminStatus_down) {
            return (WRONG_VALUE_ERROR);
        }
#endif /* IF_ALWAYS_UP */
        break;
#endif /* I_ifAdminStatus */


    }

    return NO_ERROR;
}

int
k_ifEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}
#endif /* OLD_CODE LVL7 */
/* ----------------------------------------------------------------------
 *
 *  k_ip_test() - do the usual k_*_test stuff.
 *
 *  Note:  In general, SUPPORT_IP_FORWARDING will not be enabled.
 *  Why, you ask?  Thereby hangs a tale.
 * 
 *  MIB-II does not supply enough functionality to allow a machine to
 *  be configured as a router.  The ipForwarding.0 object was proposed
 *  (in the original MIB) as a way to disable forwarding in cases where
 *  a broadcast storm was caused by the old BSD 255.255.255.255 == 
 *  0.0.0.0 == broadcast address problem, where some machines would
 *  change the 0.0.0.0 to 255.255.255.255 and rebroadcast.  This object
 *  was intended to prevent this behavior, by disabled (but never enabling)
 *  kernel packet forwarding.
 */
int
k_ip_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    switch (object->nominator) {

#ifdef I_ipForwarding
    case I_ipForwarding:

#ifndef SUPPORT_IP_FORWARDING
	if (value->sl_value != D_ipForwarding_not_forwarding) {
	    return WRONG_VALUE_ERROR;
	}
#endif /* SUPPORT_IP_FORWARDING */
	break;
#endif

#ifdef I_ipDefaultTTL
    case I_ipDefaultTTL:
	if (value->sl_value < MIN_IP_TTL || value->sl_value > 255) {
	    return WRONG_VALUE_ERROR;
	}
	break;
#endif

    }				/* switch */

    return NO_ERROR;
}

int
k_ip_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipRouteEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
#ifdef SUPPORT_ROUTE_METRIC_1
    SR_INT32 val;
#endif	/* SUPPORT_ROUTE_METRIC_1 */
#ifdef I_ipRouteNextHop
#endif /* I_ipRouteNextHop */

    switch (object->nominator) {
#ifdef I_ipRouteDest
    case I_ipRouteDest:
	/*
	 * NOTE: This variable must be supported.
	 *
         * For compatibility with MIB-I managers, if the length of ipRouteDest
         * is 0 delete the row. Otherwise, insure the value matches the
         * instance.
         */

        /*
         * MIB-I is obsolete. The linux does not care 
         * about length and value. let ioctl() handle this
         * situation                                       
         *
         */

      break;
#endif /* I_ipRouteDest */

#ifdef I_ipRouteNextHop
    case I_ipRouteNextHop:
        /*
         * For compatibility with MIB-I managers, If this value is 0 or of
         * length 0, it is deleted. Otherwise it must be of length 4.
         */

      /*
         * MIB-I is obsolete. The linux does not care    
       * about length and value. let ioctl() handle this   
       * situation. default gateway is always 0.0.0.0 and  
       * type is direct                                     
       *
       */

      break;
#endif /* I_ipRouteNextHop */

#ifdef I_ipRouteIfIndex
    case I_ipRouteIfIndex:
	/*
	 * Insure that the interface exists.
	 */
	if (k_ifEntry_get(-1, contextInfo, object->nominator, EXACT, value->sl_value) == NULL) {
	    return (INCONSISTENT_VALUE_ERROR);
	}

	break;
#endif

#ifdef I_ipRouteMetric1
    case I_ipRouteMetric1:
#ifndef SUPPORT_ROUTE_METRIC_1
	if (value->sl_value != -1) {
	    return (WRONG_VALUE_ERROR);
	}
#else
	/*
	 * The hop count, ipRouteMetric1, must be positive, zero if a
	 * direct-connection or non-zero otherwise.
	 */
	val = value->sl_value;
	if (val < ((SR_INT32)0) &&
            val != -1 ) {
	    return (WRONG_VALUE_ERROR);
	}

        if ( val == (SR_INT32)-1 ) {
           return NO_ERROR;
        }

	if ((route_type(ip_addr) == D_ipRouteType_direct && val != ((SR_INT32)0)) ||
	    (route_type(ip_addr) != D_ipRouteType_direct && val == ((SR_INT32)0))) {

	    return (INCONSISTENT_VALUE_ERROR);
	}
#endif

	break;
#endif

#ifdef I_ipRouteMetric2
    case I_ipRouteMetric2:

#ifndef SUPPORT_ROUTE_METRIC_2
	if (value->sl_value != -1) {
	    return (WRONG_VALUE_ERROR);
	}
#endif

	break;
#endif

#ifdef I_ipRouteMetric3
    case I_ipRouteMetric3:

#ifndef SUPPORT_ROUTE_METRIC_3
	if (value->sl_value != -1) {
	    return (WRONG_VALUE_ERROR);
	}
#endif
	break;
#endif

#ifdef I_ipRouteMetric4
    case I_ipRouteMetric4:

#ifndef SUPPORT_ROUTE_METRIC_4
	if (value->sl_value != -1) {
	    return (WRONG_VALUE_ERROR);
	}
#endif

	break;
#endif

#ifdef I_ipRouteType
    case I_ipRouteType:
	/*
	 * If "invalid" then delete this row. Otherwise it must be
	 * "indirect."
	 */
	/* 
	 * "direct" is also valid ipRouteType on Linux.
	 * let ioctl handle this.
	 */
	if (value->sl_value == D_ipRouteType_invalid) {
            dp->state = SR_DELETE;
	    return (NO_ERROR);
	}
	break;
#endif

#ifdef I_ipRouteMetric5
    case I_ipRouteMetric5:

#ifndef SUPPORT_ROUTE_METRIC_5
	if (value->sl_value != -1) {
	    return (WRONG_VALUE_ERROR);
	}
#endif

	break;
#endif

    }				/* switch */
    return NO_ERROR;
}

int
k_ipRouteEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_ipRouteEntry_set_defaults(dp)
    doList_t       *dp;
{
#ifdef I_ipRouteMask
    unsigned int    test_byte;

#ifndef SUPPORT_ROUTE_METRIC_1
    SET_VALID(I_ipRouteMetric1, ((ipRouteEntry_t *) (dp->data))->valid);
    ((ipRouteEntry_t *) (dp->data))->ipRouteMetric1 = -1;
#endif

#ifndef SUPPORT_ROUTE_METRIC_2
    SET_VALID(I_ipRouteMetric2, ((ipRouteEntry_t *) (dp->data))->valid);
    ((ipRouteEntry_t *) (dp->data))->ipRouteMetric2 = -1;
#endif

#ifndef SUPPORT_ROUTE_METRIC_3
    SET_VALID(I_ipRouteMetric3, ((ipRouteEntry_t *) (dp->data))->valid);
    ((ipRouteEntry_t *) (dp->data))->ipRouteMetric3 = -1;
#endif

#ifndef SUPPORT_ROUTE_METRIC_4
    SET_VALID(I_ipRouteMetric4, ((ipRouteEntry_t *) (dp->data))->valid);
    ((ipRouteEntry_t *) (dp->data))->ipRouteMetric4 = -1;
#endif

    /* Note: ignores multicast and Class E networks */
    test_byte = (((ipRouteEntry_t *) (dp->data))->ipRouteDest) >> 24;
    ((ipRouteEntry_t *) (dp->data))->ipRouteMask = 0;
    if (test_byte > 0) {
	((ipRouteEntry_t *) (dp->data))->ipRouteMask |= 0xff000000;
    }
    if (test_byte > 127) {
	((ipRouteEntry_t *) (dp->data))->ipRouteMask |= 0x00ff0000;
    }
    if (test_byte > 191) {
	((ipRouteEntry_t *) (dp->data))->ipRouteMask |= 0x0000ff00;
    }

    SET_VALID(I_ipRouteMask, ((ipRouteEntry_t *) (dp->data))->valid);

#ifndef SUPPORT_ROUTE_METRIC_5
    SET_VALID(I_ipRouteMetric5, ((ipRouteEntry_t *) (dp->data))->valid);
    ((ipRouteEntry_t *) (dp->data))->ipRouteMetric5 = -1;
#endif

#ifdef I_ipRouteProto
	SET_VALID(I_ipRouteProto, ((ipRouteEntry_t *) (dp->data))->valid);
	((ipRouteEntry_t *) (dp->data))->ipRouteProto = D_ipRouteProto_netmgmt;
#endif

    return NO_ERROR;
#endif /* I_ipRouteMask */
}

int
k_ipNetToMediaEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    int            error_status = NO_ERROR;

    switch (object->nominator) {

#ifdef I_ipNetToMediaIfIndex
        case I_ipNetToMediaIfIndex:

            break;
#endif                          /* I_ipNetToMediaIfIndex */

#ifdef I_ipNetToMediaPhysAddress
        case I_ipNetToMediaPhysAddress:

            if (value->os_value->length != 6) {
                error_status = INCONSISTENT_VALUE_ERROR;
            }

            break;
#endif                          /* I_ipNetToMediaPhysAddress */

#ifdef I_ipNetToMediaNetAddress
        case I_ipNetToMediaNetAddress:

            break;
#endif                          /* I_ipNetToMediaNetAddress */

#ifdef I_ipNetToMediaType
        case I_ipNetToMediaType:

            break;
#endif                          /* I_ipNetToMediaType */

        default:
            DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in k_ipNetToMediaEntry_test)\n"));
            error_status = GEN_ERROR;

    } /* switch */

    return(error_status);
}

int
k_ipNetToMediaEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{
#ifdef I_ipNetToMediaPhysAddress
    if (VALID(I_ipNetToMediaPhysAddress, ((ipNetToMediaEntry_t *) dp->data)->valid)) {
        dp->state = SR_ADD_MODIFY;
    }
    return NO_ERROR;
#endif /* I_ipNetToMediaPhysAddress */
}

int
k_ipNetToMediaEntry_set_defaults(dp)
    doList_t       *dp;
{

    return NO_ERROR;
}

int
k_tcpConnEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

#ifndef TCP_CONN_STATE_SETTABLE
    /* since tcpConnState can not actually be set on this system,
     * just return a wrong value error
     */
    return WRONG_VALUE_ERROR;

#else /* TCP_CONN_STATE_SETTABLE */
    switch (object->nominator) {
    case I_tcpConnState:
        if (value->sl_value != D_tcpConnState_deleteTCB) {
            return WRONG_VALUE_ERROR;
        }
        if ((value->sl_value < 1) || (value->sl_value > 12)) {
            return WRONG_VALUE_ERROR;
        }
        break;
    }
    return NO_ERROR;
#endif /* TCP_CONN_STATE_SETTABLE */
}

int
k_tcpConnEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

#endif /* SETS */

