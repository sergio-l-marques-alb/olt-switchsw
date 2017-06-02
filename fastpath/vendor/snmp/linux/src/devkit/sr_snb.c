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

#include <unistd.h>

#include <sys/types.h>

#include <time.h>



#include "sr_time.h"


#include <sys/socket.h>

#include <string.h>

#include <sys/stat.h>

#include <netinet/in.h>


#include <arpa/inet.h>


#include <fcntl.h>


#include <sys/file.h>

#include <sys/ioctl.h>




#include <errno.h>




#include "sr_type.h"
#include "sr_snb.h"
#include "diag.h"
SR_FILENAME




#define SR_ARG_TYPE (char *)





#ifndef SR_ARG_TYPE
#define SR_ARG_TYPE (int)
#endif /* SR_ARG_TYPE */

#ifndef SR_INT_TYPE
#define SR_INT_TYPE int
#endif /* SR_INT_TYPE */

#ifndef SR_NEED_SIZEOF
#define SR_NEED_SIZEOF
#endif /* SR_NEED_SIZEOF */

int
SrSetNonBlocking(int fd)
{
    FNAME("SrSetNonBlocking")
    SR_INT_TYPE arg = 1;

	 if (ioctl(fd, FIONBIO, SR_ARG_TYPE &arg SR_NEED_SIZEOF) < 0)
    {
        DPRINTF((APERROR, "%s: ioctl FIONBIO failed: %s\n", Fname,
           sys_errname(errno)));
        return -1;
    }
    return 0;
}


