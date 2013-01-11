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
#include "oid_lib.h"
#include "diag.h"

SR_FILENAME


SR_INT32
ConvToken_ValidOID(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
   OID **local = (OID **)value;
 
 
   if (ConvToken_OID(direction, token, value) == -1) {
      return -1;
   }
 
   local = (OID **)value;
 
   if ((direction == PARSER_CONVERT_FROM_TOKEN) && ((*local)->length > 0)) {
       if ( (*local)->length < 2) {
          DPRINTF((APERROR,"OID \"%s\" has length less than 2\n",*token));
          return -1;
       }
       if ( ((*local)->oid_ptr[0] <= 1) &&
            ((*local)->oid_ptr[1] >= 40)) {
          DPRINTF((APERROR,"Invalid OID \"%s\" (if first subid is 0 or 1 second subid must be less than 40)\n",*token));
          return -1;
       }
       if ( (*local)->oid_ptr[0] >= 3) {
          DPRINTF((APERROR,"Invalid OID \"%s\" (first subid must be less than 3)\n",*token));
          return -1;
       }
   }
 
   return 0;
 
}
 

SR_INT32
ConvToken_OID(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OID **local = (OID **)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if(strcmp(*token, "-") == 0) {
                *local = MakeOID(NULL, 0);
                if (*local == NULL) {
                    DPRINTF((APWARN, "Can't make '%s' into an OID\n", *token));
                    return -1;
                }
            } else {
                *local = MakeOIDFromDot(*token);
                if (*local == NULL) {
                    DPRINTF((APWARN, "Can't make '%s' into an OID\n", *token));
                    return -1;
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
                *token = (char *)malloc(128 + ((*local)->length * 12));
                if (*token == NULL) {
                    return -1;
                }
                MakeDotFromOID(*local, *token);
            }
            return 0;
        default:
            return -1;
    }
}
