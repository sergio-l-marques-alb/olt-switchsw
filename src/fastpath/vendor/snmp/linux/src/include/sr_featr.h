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

/*
 * This file include #defines that will enable and disable features in
 * the source code.  This file used to be called features.h but is now
 * sr_featr.h since some systems have a standard include file called
 * features.h
 */

#ifndef SR_SR_FEATR_H
#define SR_SR_FEATR_H

#ifdef  __cplusplus
extern "C" {
#endif	/* __cplusplus */

/*
 * LIGHT:  remove the Object Descriptors from OID structs for space
 * savings
 */
#ifndef LIGHT
/* #define LIGHT */
#endif	/* LIGHT */

/*
 * SETS:  provide SET request support in method routines
 */
#ifndef SETS
/* #define SETS */
#endif /* SETS */

/*
 * SR_RCSID:  If you want an RCS Id inserted into each object file,
 * define this.
 */
/* #define SR_RCSID */

/*
 * IANA_ENTERPRISE_NUMBER:  This is the enterprise number
 * assigned your company by the Internet Assigned Numbers
 * Authority.  If you do not change this, the SNMP Research,
 * Inc. number will be used (99).  This affects seclib/snmpv2.c
 */
#ifndef IANA_ENTERPRISE_NUMBER
#define IANA_ENTERPRISE_NUMBER 99
#endif	/* IANA_ENTERPRISE_NUMBER */

/*
 * SR_CLEAR_MALLOC:  some mallocs will hand back memory which is already
 * zero'ed out.  In those cases, it is unnecessary to use memset() to
 * zero out the memory.  If you have such a malloc, #undef
 * SR_CLEAR_MALLOC.  Otherwise, leave it defined.
 */
#define SR_CLEAR_MALLOC

/*
 * SR_CLEAR_AUTOS:  if by some wild chance your automatic variables are
 * handed to you zero'ed, then #undef this.  In most cases, you should 
 * leave it alone.
 */
#define SR_CLEAR_AUTOS

/*
 * SR_CLEAR_GLOBALS:  if you don't need to clear global variables with
 * memset, #undef this.
 */
#define SR_CLEAR_GLOBALS

/*
 * SR_NO_COMMENTS_IN_CONFIG:  if you are using our code for parsing
 * configuration files (-DSR_CONFIG_FP is defined) but you do not want
 * to take up space in the agent for the comments that are written out for
 * the entries in the log file, then define this.  The config file will
 * be written out without any comments at all and the strings which
 * constitute those comments will not be compiled into the agent.
 *
 * NOTE WELL:  if you compile ANY of the tree with this on then you must
 * compile all of the tree with this on.  Defining this removes the
 * "comment" field in the PARSER_RECORD_TYPE in scan.h.
 */
#ifndef SR_NO_COMMENTS_IN_CONFIG
/* #define SR_NO_COMMENTS_IN_CONFIG 1 */
#endif	/* SR_NO_COMMENTS_IN_CONFIG */

/*
 * SR_FTL:  if you want to use IEEE MacAddrs, and have them represented as
 * OctetStrings of length 6, turn this on.
 */
#ifndef SR_FTL
/* #define SR_FTL */
#endif	/* SR_FTL */

/*
 * WANT_SPRINTVARBINDSTUFF:  this includes two functions in
 * seclib/frmt_lib.c which wouldn't otherwise be there:
 * SPrintVarBind()
 * SPrintVarBindValue()
 */
#define WANT_SPRINTVARBINDSTUFF




/*
 * SR_MEMCHECK:  if you wish to use the debugger Mem Check from
 * StratosWare Corporation define SR_MEMCHECK to include the file
 * memcheck.h in source code files.
 */
/* #define SR_MEMCHECK */


/*
 * BACKUP_FILE_SUFFIX:  this is the suffix tacked onto the end of
 * filenames when new versions are created by the agent.  If you wish to
 * have something else, redefine this value.
 */

#ifdef EIGHT_PLUS_THREE
#define BACKUP_FILE_SUFFIX ".bak"
#else
#define BACKUP_FILE_SUFFIX "~"
#endif	/* EIGHT_PLUS_THREE */

/* #define SR_MIB_TABLE_HASHING */
/* #define SR_MIB_TABLE_BINARY */

#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SR_FEATR_H */
