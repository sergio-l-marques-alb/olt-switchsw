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

#include <sys/stat.h>

#include <fcntl.h>


#include "sr_snmp.h"
#include "compat.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "oid_lib.h"
#include "diag.h"
#include "ct_old.h"
SR_FILENAME


#ifdef SR_CONFIG_FP
SR_INT32
ConvToken_storageType(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_INT32 *local = (SR_INT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "other") == 0) {
                *local = SR_OTHER;
                return 0;
            }
            if (strcmp(*token, "volatile") == 0) {
                *local = SR_VOLATILE;
                return 0;
            }
            if (strcmp(*token, "nonVolatile") == 0) {
                *local = SR_NONVOLATILE;
                return 0;
            }
            if (strcmp(*token, "readOnly") == 0) {
                *local = SR_READONLY;
                return 0;
            }
            if (strcmp(*token, "permanent") == 0) {
                *local = SR_PERMANENT;
                return 0;
            }
            DPRINTF((APWARN, "Unknown storage type '%s'\n", *token));
            return -1;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(16);
            if (*token == NULL) {
                return -1;
            }
            switch (*local) {
                case SR_OTHER:
                    strcpy(*token, "other");
                    break;
                case SR_VOLATILE:
                    strcpy(*token, "volatile");
                    break;
                case SR_NONVOLATILE:
                    strcpy(*token, "nonVolatile");
                    break;
                case SR_READONLY:
                    strcpy(*token, "readOnly");
                    break;
                case SR_PERMANENT:
                    strcpy(*token, "permanent");
                    break;
            }
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_viewType(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_INT32 *local = (SR_INT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "included") == 0) {
                *local = INCLUDED;
                return 0;
            }
            if (strcmp(*token, "excluded") == 0) {
                *local = EXCLUDED;
                return 0;
            }
            DPRINTF((APWARN, "Unknown view type '%s'\n", *token));
            return -1;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(16);
            if (*token == NULL) {
                return -1;
            }
            switch (*local) {
                case INCLUDED:
                    strcpy(*token, "included");
                    break;
                case EXCLUDED:
                    strcpy(*token, "excluded");
                    break;
            }
            return 0;
        default:
            return -1;
    }
}

#endif /* SR_CONFIG_FP */
