/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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

#include "mibtools.h"
#include "agent.h"

/*
 * write an lcon.c file which contains a call to LocalConnect()
 */

int
write_localconnect_file(base)
    char           *base;
{
    char            buff[100];
    FILE           *fp;

    sprintf(buff, "%slcon", base);
    fp = open_file(buff, ".c");
    print_copyright(fp);

    fprintf(fp, "{\n");

    if (!no_cvs_keywords) {
        print_cvs_keywords(fp, C_FILE);
    }

    fprintf(fp, "    extern int k_%s_initialize\n", base);
    fprintf(fp, "        SR_PROTOTYPE((void)); \n");
    fprintf(fp, "\n");
    fprintf(fp, "    extern ObjectInfo %sOidList[];\n", base);
    fprintf(fp, "    LocalConnect(\"EMANATE\",");
    fprintf(fp, " MakeOIDFromDot(\"0.0\"),");
    fprintf(fp, " \"%s\", %sOidList);\n", base, base);
    fprintf(fp, "    k_%s_initialize();\n", base);
    fprintf(fp, "}\n");
    fprintf(fp, "\n");

    fclose(fp);
    return 1;
}

