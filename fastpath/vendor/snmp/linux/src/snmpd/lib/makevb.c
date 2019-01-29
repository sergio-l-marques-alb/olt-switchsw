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

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>




#include <malloc.h>

#include "sr_snmp.h"

#include "sr_trans.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "diag.h"
SR_FILENAME

/*
 * MakeVarBind:
 *
 * NOTE: This is the "new" version of make_varbind, MakeVarBind. It
 * differs in the parameters passed to it but performs the same
 * task. The "old" version will be maintained for a short time to
 * allow for conversion.
 *
 * This MakeVarBind is used by the agent method routines, while
 * MakeVarBindWithNull and MakeVarBindWithValue are used by the
 * manager routines.
 *
 * The former name (pre-snmp12.1.0.0 release) was MakeVarbind().
 */
VarBind        *
MakeVarBind(object, instance, value)
    const ObjectInfo *object;
    const OID      *instance;
    void           *value;
{
    FNAME("MakeVarBind")

    if (object == NULL) {
        DPRINTF((APWARN, "%s: Null object parameter.\n", Fname));
        return (NULL);
    }

    if (&object->oid == (OID *) NULL) {
        DPRINTF((APWARN, "%s: object->oid is NULL.\n", Fname));
        return (NULL);
    }

    return MakeVarBindWithValue(&object->oid,
                                instance,
                                object->oidtype,
                                value);
}
