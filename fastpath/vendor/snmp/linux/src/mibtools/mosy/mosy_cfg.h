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

/* config.h - compile time configuration parameters */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/*
 * OPTIONS:-
 *
 * #define PDU_DUMP
 * 		If this is defined, and "dish" is invoke with
 *		dish -pdu foobar
 *		Then a directory "foobar" will be created, and
 *		it will contain logs of all the X500 PDUs sent
 *
 * #define NO_STATS
 * 		If defined, the QUIPU will NOT produce statistical
 *		logs of both the DSA and DUA.
 *
 * #define CHECK_FILE_ATTRIBUTES
 * 		If and EDB entry contains a FILE attribute, check that
 *		the corresponding file exists
 *
 * #define QUIPU_MALLOC
 * 		Use a version of malloc optimised for the memory
 * 		resident Quipu DSA database.
 *
 * #define TURBO_DISK
 *		Store EDB files in gdbm files instead of plain text files.
 *		This makes modifies of entries in large EDB files much
 *		faster.  See the ../../quipu/turbo directory for tools to
 *		help in converting your EDB files.
 *
 * #define TURBO_INDEX
 *		Enable code to build and search database indexes for
 *		selected attributes (see tailor file options optimize_attr,
 *		index_subtree, and index_siblings).
 *		This can cut the search	time for very large databases.
 *
 * #define SOUNDEX_PREFIX
 *		Consider soundex prefixes as matches.  For example, make
 *		"fred" match "frederick".  #defining this option gives
 *		approximate matching behavior the same as in version 6.0.
 *
 * #define STRICT_X500
 *		Enforce X.500 more strictly than "normal".
 *		Useful for conformance testing, but not "real users".
 *
 * #define HAVE_PROTECTED
 *		If defined, enable use of protectedPassword attribute.
 *
 * #define QUIPU_CONSOLE
 *		Undocumented pre-alpha test feature.
 *
 */


#ifndef SR_MOSY_CFG_H
#define SR_MOSY_CFG_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef PC
#define SYSV
#define SYS5
#endif				/* PC */

#ifndef SYSV
#define BSD42			/* Berkeley UNIX */
#define SUNOS4			/* with Sun's enhancements */
#define WRITEV			/* (sort of) real Berkeley UNIX */
#define BSD43			/* 4.3BSD or later */
#else				/* SYSV */
#define VSPRINTF
#endif				/* SYSV */

#ifndef	USE_BUILTIN_OIDS
#define	USE_BUILTIN_OIDS	1
#endif

#define PDU_DUMP
#define QUIPU_MALLOC
#define TURBO_INDEX
#define SOUNDEX_PREFIX
#define HAVE_PROTECTED

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_MOSY_CFG_H */
