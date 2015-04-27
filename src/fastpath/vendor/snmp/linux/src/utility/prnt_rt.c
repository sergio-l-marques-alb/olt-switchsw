/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#include "sr_utils.h"
#include "prnt_lib.h"

/*
 * PrintRouteInfo:
 *
 * The former name (pre-snmp12.1.0.0 release) was print_route_info().
 */
void
PrintRouteInfo(vb_list_ptr)
    VarBind        *vb_list_ptr;
{
    VarBind        *vb_ptr;

    struct in_addr  dest_addr, next_addr;
    int             if_num;
    int             metric;
    int             type;
    int             proto;


    /* ipRouteDest */
    vb_ptr = vb_list_ptr;

    dest_addr.s_addr =
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[0]) << 24) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[1]) << 16) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[2]) << 8) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[3]));



    vb_ptr = vb_ptr->next_var;
    if_num = (short) vb_ptr->value.sl_value;

    vb_ptr = vb_ptr->next_var;
    metric = (short) vb_ptr->value.sl_value;

    vb_ptr = vb_ptr->next_var;

    next_addr.s_addr =
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[0]) << 24) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[1]) << 16) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[2]) << 8) +
	(((SR_UINT32) vb_ptr->value.os_value->octet_ptr[3]));




    vb_ptr = vb_ptr->next_var;
    type = (short) vb_ptr->value.sl_value;

    vb_ptr = vb_ptr->next_var;
    proto = (short) vb_ptr->value.sl_value;

    printf("Route to: %s ", (char *)inet_ltoa(htonl(dest_addr.s_addr)));
    printf("via: %s  on if: %d metric: %d type: %d proto: %d\n",
	   (char *)inet_ltoa(htonl(next_addr.s_addr)), if_num, metric, type, proto);



}				/* PrintRouteInfo() */
