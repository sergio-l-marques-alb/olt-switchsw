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


#include "sr_conf.h"

#include <stdio.h>

#include <sys/types.h>


#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <malloc.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_proto.h"
#include "v3_tag.h"

/*
 * SrTagInTagList
 *
 * This function scans a tag list for a particular tag.  If the tag is
 * present, it returns 1.  Otherwise, it returns 0.
 *
 * This function assumes that both the tag and taglist are well-formed.
 * The result if either is malformed is undefined.
 */
SR_INT32
SrTagInTagList(
    OctetString *tag,
    OctetString *taglist)
{
    unsigned char *t_str, *tl_str, *ch;
    SR_INT32 t_len, tl_len, cl;

    if (tag == NULL) {
        return 0;
    }
    if (taglist == NULL) {
        return 0;
    }
    if (tag->length == 0) {
        return 0;
    }
    if (taglist->length == 0) {
        return 0;
    }

    t_str = tag->octet_ptr;
    t_len = tag->length;

    tl_str = taglist->octet_ptr;
    tl_len = taglist->length;

    while (tl_len > 0) {
        if (t_len > tl_len) {
            return 0;
        }
        cl = 1;
        ch = tl_str;
        while (cl < tl_len) {
            ch++;
            if ((*ch == (unsigned char)0x20) ||
                (*ch == (unsigned char)0x09) ||
                (*ch == (unsigned char)0x0d) ||
                (*ch == (unsigned char)0x0b)) {
                break;
            }
            cl++;
        }
        if (cl == t_len) {
            if (memcmp(t_str, tl_str, (size_t)t_len) == 0) {
                return 1;
            }
        }
        tl_str = ch + 1;
        tl_len = tl_len - cl - 1;
    }
    return 0;
}
