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
 * -agent -o srAgentInfo 
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "mibout.h"
#include "sr_vrsn.h"
#include "diag.h"
SR_FILENAME

static srAgentInfo_t srAgentInfoData;

/* 
 * Determine which operating system this was built from the 
 * appropriate #define, and put a more meaningful description
 * into SR_OS_FLAVOR
 */
#define SR_OS_FLAVOR "Linux"

/*Default is empty*/
#ifndef SR_OS_FLAVOR
#define SR_OS_FLAVOR ""
#endif /* SR_OS_FLAVOR */

int
k_srAgentInfo_initialize(void)
{
   char bannerstring[sizeof(STARTUP_BANNER_MIBOBJ)],
        osflavor[sizeof(SR_OS_FLAVOR)];

   /*
    * Copy the STARTUP_BANNER_MIBOBJ which is defined in sr_vrsn.h to 
    * bannerstring and remove the newline character
    */
   sprintf(bannerstring, STARTUP_BANNER_MIBOBJ);
   if(strtok(bannerstring, "\n") == NULL) {
	DPRINTF((APTRACE, "Error in strtok\n"));
	goto done;
   }

   sprintf(osflavor, SR_OS_FLAVOR);

   srAgentInfoData.srAgentInfoDescr = MakeOctetStringFromText(bannerstring);
   srAgentInfoData.srAgentInfoOSFlavor = MakeOctetStringFromText(osflavor);
   
done:

   return 1;
}

int
k_srAgentInfo_terminate(void)
{
    return 1;
}

srAgentInfo_t *
k_srAgentInfo_get(int serialNum, ContextInfo *contextInfo,
                  int nominator)
{

   /*
    * put your code to retrieve the information here
    */

   SET_ALL_VALID(srAgentInfoData.valid);
   return(&srAgentInfoData);
}
