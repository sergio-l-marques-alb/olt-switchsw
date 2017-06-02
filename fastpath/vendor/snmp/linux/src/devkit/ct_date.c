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

#include <string.h>

#include <ctype.h>

#include <malloc.h>

#include <stdlib.h>

#include <unistd.h>


#include <sys/types.h>



#include "sr_snmp.h"
#include "compat.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "tc_time.h"
#include "oid_lib.h"
#include "diag.h"

SR_FILENAME

#define DATE_SIZE 32

SR_INT32
ConvToken_DateAndTime(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = TextToDateAndTime(*token);
            if (*local == NULL) {
                return ConvToken_textOctetString(direction, token, value);
            }
            break;
        case PARSER_CONVERT_TO_TOKEN:
            if (*local == NULL) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            }
            else if ((*local)->length == 0) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            }
            else {
                *token = (char *)malloc(sizeof(char)*DATE_SIZE);
                if (*token == NULL) {
                    return -1;
                }
                if (SPrintDateAndTime(*local, *token, DATE_SIZE) < 0) {
                    free(*token);
                    *token = NULL;
                    return -1;
                }
            }
            break;
        default:
            return -1;
    }
    return 0;
}
