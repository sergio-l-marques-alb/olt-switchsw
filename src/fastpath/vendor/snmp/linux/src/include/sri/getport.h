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

#ifndef SR_SRI_GETPORT_H
#define SR_SRI_GETPORT_H

#ifdef  __cplusplus
extern "C" {
#endif

int GetSNMPPort
    SR_PROTOTYPE((void));


int GetSNMPTrapPort
    SR_PROTOTYPE((void));

int SetUpEnviron
    SR_PROTOTYPE((int argc,
                  char **argv,
                  char **envp));


/*
 *  GetServerPort()
 *
 *  Function:  Returns the IP port for a server to listen on.
 *
 *  Inputs:    envvar       - the environment variable to check.  If this
 *                            argument is NULL, then no environment variables
 *                            will be queried.  Querying environment variables
 *                            may not be supported on all platforms.
 *             type         - the type of service; must be either
 *                            "udp" or "tcp".
 *             service      - the service name to look for in /etc/services.
 *                            If this argument is NULL, then /etc/services
 *                            will not be checked.
 *             default_port - the default port number to use.
 *
 *  Outputs:   Always returns an integer value.
 *
 *  Notes:     This function returns the port number for a server to listen
 *             on using the following search order:
 *
 *             1)  Attempt to retrieve and convert the environment variable
 *                 specified by the "envvar" argument;
 *
 *             2)  Attempt to retrieve the matching entry from /etc/services
 *                 as specified by the "type" and "service" arguments.
 *
 *             3)  Return the value of the "default_port" argument.
 */
int
GetServerPort(
    char *envvar,
    char *type,
    char *service,
    int  default_port);


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_GETPORT_H */
