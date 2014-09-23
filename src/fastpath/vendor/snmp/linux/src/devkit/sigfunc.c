/*
 *
 * Copyright (C) 1996-2006 by SNMP Research, Incorporated.
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


/*
 * SetSigFunction was added due to the fact that older version of SYSVR4
 * implement signals differently than SunOS and later SYSVR4 versions.
 * Specifically, we need to call sigaction() rather than signal, as in
 * older SYSVR4 signal() was unreliable. In calling SetSigFuncion(), code
 * running in SYSVR4 environments will use sigaction(), while non-SYSVR4
 * environments will continue to use signal(). For an excellent 
 * explanation of signals, see "Advanced Programming in the UNIX 
 * Environment" by Stevens, chapter 10.7. 
 *
 */

#include "sr_conf.h"


#include <stdio.h>

#include <string.h>

#include <signal.h>

#include "sr_snmp.h"

SigFunc
SetSigFunction(sig, disp)
int sig;
SigFunc disp;
{
#if defined(SYSV)
   struct sigaction act, oact;

   memset(&act, 0x00, sizeof(struct sigaction));
   memset(&oact, 0x00, sizeof(struct sigaction));

   act.sa_handler = disp;
   sigemptyset(&act.sa_mask);
   act.sa_flags=0;
#ifdef SA_INTERRUPT
   act.sa_flags |= SA_INTERRUPT;   /* like sunos */
#endif  /* SA_INTERRUPT */
   if (sigaction(sig, &act, &oact)<0) {
     return(SIG_ERR);
   }
   return(oact.sa_handler);
#else	/* defined(SYSV) */
   return(signal(sig, disp));
#endif	/* defined(SYSV) */

}

/* GetSigFunction takes a signal, and returns a pointer to the function
 * that is registered to handle that signal.
 *
 */

SigFunc
GetSigFunction(int sig)
{
   SigFunc old_func;

   old_func = SetSigFunction(sig, (SigFunc)1);
   SetSigFunction(sig, old_func);

   return old_func;
}
