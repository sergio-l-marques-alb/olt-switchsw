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
#include "oid_lib.h"
#include "diag.h"

SR_FILENAME

SR_INT32
ConvToken_textOctetString(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    int i, j, printable, needquotes;
    int len;
    char *tmp;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "-") == 0) {
                *local = MakeOctetString(NULL, 0);
            } else {
                len = strlen(*token);
                if ((len % 3) == 2) {
                    for (i = 2; i < len; i += 3) {
                        if ((*token)[i] != ':') {
                            break;
                        }
                    }
                    if (i >= len) {
                        *local = MakeOctetStringFromHex(*token);
                        if (*local == NULL)  {
                            *local = MakeOctetStringFromText(*token);
                        }
                    } else {
                        *local = MakeOctetStringFromText(*token);
                    }
                } else {
                    *local = MakeOctetStringFromText(*token);
                }
                if (*local == NULL) {
                    /* print a warning and assign a fake string */
                    DPRINTF((APWARN, "Can't make '%s' into an OctetString\n",
                             *token));
                    *local = MakeOctetString(NULL, 0);
                }
            }
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            if (*local == NULL) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else if ((*local)->length == 0) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else {
                printable = 0;
                for (i = 0, j = 0; i < (*local)->length; i++, j++) {
                    if ((j % 3) < 2) {
                        if ((((*local)->octet_ptr[i] < '0') ||
                             ((*local)->octet_ptr[i] > '9')) &&
                            (((*local)->octet_ptr[i] < 'a') ||
                             ((*local)->octet_ptr[i] > 'f')) &&
                            (((*local)->octet_ptr[i] < 'A') ||
                             ((*local)->octet_ptr[i] > 'F'))) {
                            printable = 1;
                            break;
                        }
                    } else {
                        if ((*local)->octet_ptr[i] != ':') {
                            printable = 1;
                            break;
                        }
                    }
                }
                if (printable) {
                    for (i = 0; i < (*local)->length; i++) {
                        if (!isprint((unsigned char)(*local)->octet_ptr[i])) {
                            printable = 0;
                            if ((char)(*local)->octet_ptr[i] == '\n') {
                                printable = 1;
                                continue;
                            }
                            if ((char)(*local)->octet_ptr[i] == '\r') {
                                printable = 1;
                                continue;
                            }
                            if ((char)(*local)->octet_ptr[i] == '\t') {
                                printable = 1;
                                continue;
                            }
                            break;
                        }
                    }
                }
                if (printable) {
                    *token = (char *)malloc((*local)->length * 2 + 3);
                    if (*token == NULL) {
                        return -1;
                    }
                    needquotes = 0;
                    for (i = 0; i < (*local)->length; i++) {
                        if ((*local)->octet_ptr[i] == ' ' ||
			    (*local)->octet_ptr[i] == '\\' ||
			    (*local)->octet_ptr[i] == '\n' ||
			    (*local)->octet_ptr[i] == '\r' ||
			    (*local)->octet_ptr[i] == '\t') {
                            needquotes = 1;
                        }
                    }
                    if (needquotes) {
                        memcpy((*token)+1,(*local)->octet_ptr,
                               (int)(*local)->length);
                        (*token)[0] = '\"';
                        for (i=0,j=1; i<(int)(*local)->length; i++,j++) {
                            if ((*local)->octet_ptr[i] == '"') {
                                (*token)[j++] = '\\';
                                (*token)[j] = '"';
                            } else if ((*local)->octet_ptr[i] == '\\') {
                                (*token)[j++] = '\\';
                                (*token)[j] = '\\';
                            } else if ((*local)->octet_ptr[i] == '\n') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 'n';
                            } else if ((*local)->octet_ptr[i] == '\r') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 'r';
                            } else if ((*local)->octet_ptr[i] == '\t') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 't';
                            } else {
                                (*token)[j] = (*local)->octet_ptr[i];
                            }
                        }
                        (*token)[j++] = '\"';
                        (*token)[j] = '\0';
                    } else {
                        memcpy((*token), (*local)->octet_ptr, 
                               (int)(*local)->length);
                        (*token)[(*local)->length] = '\0';
                    }
                } else {
                    *token = (char *)malloc((*local)->length * 3 + 3);
                    if (*token == NULL) {
                        return -1;
                    }
                    tmp = *token;
                    for (i = 0; i < (*local)->length; i++) {
                        sprintf(tmp, "%02x:", (*local)->octet_ptr[i]);
                        tmp += 3;
                    }
                    tmp--;
                    tmp[0] = '\0';
                }
            }
            return 0;
        default:
            return -1;
    }
}
