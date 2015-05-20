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

#include <stdlib.h>

#include <stddef.h>


#include <sys/types.h>


#include <malloc.h>

#include <netdb.h>



#include <netinet/in.h>

#include <string.h>

#include "logger.h"

#include "sr_snmp.h"
#include "compat.h"
#include "diag.h"
SR_FILENAME



#define SR_SNMP_ENV_NAME "SR_SNMP_TEST_PORT"
#define SR_TRAP_ENV_NAME "SR_TRAP_TEST_PORT"

#define SR_SNMP_SVC_NAME "snmp"
#define SR_TRAP_SVC_NAME "snmp-trap"

#define SNMP_PORT 1161          /* PTin modified: port connections (161) */
#define SNMP_TRAP_PORT 1162     /* PTin modified: port connections (162) */


/*
 * GetSNMPPort()
 *
 * If the environment variable SR_SNMP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP requests. Otherwise, if the
 * function getservbyname(SR_SNMP_SVC_NAME, "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise use the
 * #define SNMP_PORT if it exists, otherwise, use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPPort()
{
  /* PTin removed: SNMP */
  #if 0
    FNAME("GetSNMPPort")
    static int      snmp_port;
    struct servent *snmp_serv;
    static int      called = 0;
    char           *temp;

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    snmp_port = atoi(temp);
	    DPRINTF((APTRACE, "%s: snmp port is %d from getenv(\"%s\")\n",
		     Fname, snmp_port, SR_SNMP_ENV_NAME));
	}
	else if ((snmp_serv =
		  getservbyname(SR_SNMP_SVC_NAME, "udp")) != NULL) {
	    snmp_port = ntohs(snmp_serv->s_port);
	    DPRINTF((APTRACE,
		     "%s: snmp port is %d from %s in /etc/services\n",
		     Fname, snmp_port, SR_SNMP_SVC_NAME));
	}
	else {
	    DPRINTF((APTRACE, "%s: no %s entry in /etc/services\n", Fname,
		     SR_SNMP_SVC_NAME));

#ifdef SNMP_PORT
	    snmp_port = SNMP_PORT;
	    DPRINTF((APTRACE, "snmp port is %d from #define SNMP_PORT\n",
		     snmp_port));
#else
	    snmp_port = 161;
	    DPRINTF((APWARN, "snmp port is %d from constant\n", snmp_port));
#endif

	}
    }

    return snmp_port;
  #endif
  LOG_PT_WARN(LOG_CTX_MISC, "Returning SNMP_PORT=%u",SNMP_PORT);
  return SNMP_PORT;
}


/*
 * GetSNMPTrapPort()
 *
 * If the environment variable SR_TRAP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP traps. Else, if the environment
 * variable SR_SNMP_ENV_NAME exits, use it's value + 1. Otherwise, if the
 * function getservbyname("snmp-trap", "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise if the
 * #define SNMP_TRAP_PORT exits, use it's value, otherwise use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPTrapPort()
{
  /* PTin removed: SNMP */
  #if 0
    FNAME("GetSNMPTrapPort")
    static int      trap_port;
    static int      called = 0;
    char           *temp;
    struct servent *snmp_trap_serv;

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_TRAP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp);
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\")\n",
		     Fname, trap_port, SR_TRAP_ENV_NAME));
	}
	else if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp) + 1;
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\") + 1\n",
		     Fname, trap_port, SR_SNMP_ENV_NAME));
	}
	else if ((snmp_trap_serv =
		  getservbyname(SR_TRAP_SVC_NAME, "udp")) != NULL) {
	    trap_port = ntohs(snmp_trap_serv->s_port);
	    DPRINTF((APTRAP, "%s: trap port %d from %s in /etc/services\n",
		     Fname, trap_port, SR_TRAP_SVC_NAME));
	}
	else {
	    DPRINTF((APTRAP, "%s: no %s entry in /etc/services; ", Fname,
		     SR_TRAP_SVC_NAME));

#ifdef SNMP_TRAP_PORT
	    trap_port = SNMP_TRAP_PORT;
	    DPRINTF((APTRAP, "trap port is %d from #define SNMP_TRAP_PORT\n",
		     trap_port));
#else				/* SNMP_TRAP_PORT */
	    trap_port = 162;
	    DPRINTF((APWARN, "trap port is %d from constant\n", trap_port));
#endif				/* SNMP_TRAP_PORT */

	}
    }

    return trap_port;
  #endif
  LOG_PT_WARN(LOG_CTX_MISC, "Returning SNMP_TRAP_PORT=%u",SNMP_TRAP_PORT);
  return SNMP_TRAP_PORT;
}

#ifdef WANT_SETUPENVIRON

extern char   **environ;

int SetUpEnviron
    SR_PROTOTYPE((int argc, 
		  char **argv,
		  char **envp));

/* SetUpEnviron() scribbles on argv, so call it after all arg processing */
/* It puts the port number in argv[0] so it will show up in ps */

int
SetUpEnviron(argc, argv, envp)
    int             argc;
    char          **argv;
    char          **envp;
{
    int             s_port = GetSNMPPort();
    int             i;
    int             totavail = 0;
    char          **dupenviron = NULL;
    char            buf[256];

    if (envp < argv) return 0;

    if (argc > 0) {
	totavail = (argv[argc - 1] - argv[0]) + strlen(argv[argc - 1]) - 2;
	if (envp != NULL) {
	    /* save the user environment so we can write over it */
	    for (i = 0; envp[i] != NULL; i++) {
	    }
	    if ((dupenviron = (char **) malloc((i + 1) * sizeof(char *))) == NULL) {
		DPRINTF((APERROR, "can't duplicate environment\n"));
		goto fail;
	    }
#ifdef SR_CLEAR_MALLOC
	    memset(dupenviron, 0, (i + 1) * sizeof(char *));
#endif	/* SR_CLEAR_MALLOC */
	    for (i = 0; envp[i] != NULL; i++) {
		if ((dupenviron[i] = strdup(envp[i])) == NULL) {
		    DPRINTF((APERROR, "can't duplicate environment string\n"));
		    goto fail;
		}
	    }
	    totavail = (envp[i - 1] - argv[0]) + strlen(envp[i - 1]) - 2;
	    environ = dupenviron;
	}

#ifdef SR_CLEAR_AUTOS
	memset(buf, 0, sizeof(buf));
#endif	/* SR_CLEAR_AUTOS */
	snprintf(buf, sizeof(buf), "%s (%d)", argv[0], s_port);

	if (strlen(buf) < (unsigned)totavail) {
	    strcpy(argv[0], buf);
	    memset(argv[0] + strlen(buf) + 1, ' ', totavail - 1 - strlen(buf));
	}
    }

    return 0;
fail:
    if (dupenviron != NULL) {
	for (i = 0; dupenviron[i] != NULL; i++) {
	    free(dupenviron[i]);
	}
	free(dupenviron);
    }
    return -1;
}
#endif	/* WANT_SETUPENVIRON */


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
    int  default_port)
{
    struct servent *servent;
    char   *ev;

    /* --- Check for an environment variable --- */
    if (envvar != NULL) {
        ev = getenv(envvar);
        if (ev != NULL) {
            if (atoi(ev) != 0) {
                return atoi(ev);
            }
        }
    }

    /* --- Check for an entry in /etc/services --- */
    if ((type != NULL) && (service != NULL) &&
        ((strcmp(type, "udp") == 0) || (strcmp(type, "tcp") == 0))) {
        servent = getservbyname(service, type);
        if (servent != NULL) {
            return (int) servent->s_port;
        }
    }

    /* --- Return the default port --- */
    return default_port;
}   /* GetServerPort() */

