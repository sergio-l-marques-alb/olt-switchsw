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

#include "mibtools.h"
#include "agent.h"

/*
 * write an oidl.c file which contains a standalone list of objects
 */

int
write_agent_oidlist_file(base)
    char           *base;
{
    char            buff[100];
    FILE           *fp;

    sprintf(buff, "%soidl", base);
    fp = open_file(buff, ".c");
    print_copyright(fp);

    if (!no_cvs_keywords) {
        print_cvs_keywords(fp, C_FILE);
    }

    fprintf(fp, "#include \"sr_conf.h\"\n");
    fprintf(fp, "\n");
    fprintf(fp, "#ifdef HAVE_STDIO_H\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#endif /* HAVE_STDIO_H */\n");
    fprintf(fp, "\n");
    fprintf(fp, "#ifdef HAVE_SYS_TYPES_H\n");
    fprintf(fp, "#include <sys/types.h>\n");
    fprintf(fp, "#endif /* HAVE_SYS_TYPES_H */\n");
    fprintf(fp, "\n");
    fprintf(fp, "#include \"sr_snmp.h\"\n");
    fprintf(fp, "#include \"sr_trans.h\"\n");
    fprintf(fp, "#include \"context.h\"\n");
    fprintf(fp, "#include \"method.h\"\n");
    fprintf(fp, "#include \"%sdefs.h\"\n", base);
    fprintf(fp, "#include \"%ssupp.h\"\n", base);
    fprintf(fp, "#ifndef SNMPPART_DECL\n");
    fprintf(fp, "#define SNMPPART_DECL\n");
    fprintf(fp, "#endif /* SNMPPART_DECL */\n");
    fprintf(fp, "#include \"%spart.h\"\n", base);
    fprintf(fp, "#undef  SNMPPART_DECL\n");
    fprintf(fp, "#include \"%stype.h\"\n", base);

    if (separate_type_file) {
        fprintf(fp, "#include \"%sprot.h\"\n", base);
    }

    fprintf(fp, "\n");
    fprintf(fp, "/* the objects internal to the agent */\n");
    fprintf(fp, "ObjectInfo      %sOidList[] = {\n", base);
    fprintf(fp, "\n");
    fprintf(fp, "#include \"%soid.c\"\n", base);
    fprintf(fp, "\n");
    fprintf(fp, "    {{0, NULL},  /* The special \"null\"-terminator. */\n");
    fprintf(fp, "#ifndef LIGHT\n");
    fprintf(fp, "    NULL,\n");
    fprintf(fp, "#endif /* LIGHT */\n");
    fprintf(fp, "    0, 0, 0, 0,\n");
    fprintf(fp, "    NULL, NULL}\n");
    fprintf(fp, "};\n");
    fprintf(fp, "\n");

    if (no_v_get || no_v_test) {
        fprintf(fp, "#include \"%sminv.c\"\n", base);
        fprintf(fp, "\n");
    }

    fclose(fp);
    return 1;
}

