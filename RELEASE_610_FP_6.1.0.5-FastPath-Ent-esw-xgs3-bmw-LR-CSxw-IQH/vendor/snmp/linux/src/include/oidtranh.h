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

/*
 * Provides prototypes for functions for translating between human-friendly 
 * OID descriptors and OIDs.  These functions are intended to be used in code 
 * that interacts with humans.  (HF - "Human Friendly")
 */

#ifndef SR_OIDTRANH_H
#define SR_OIDTRANH_H

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  IsDottedDecimal()
 *
 *  Function:  Tests a character string to see if it is a
 *             dotted-decimal number.
 *
 *  Inputs:    str - the string to test.
 *
 *  Outputs:    0 - the string is NOT a dotted-decimal number string.
 *             -1 - the string IS a dotted-decimal number string.
 *
 *  Notes:     For zero-length strings, this function returns 0.
 */
#define IsDottedDecimal(str) (((strspn(str,"0123456789.")==strlen(str))&&(strlen(str)!=0))?-1:0)


#ifdef SR_MIB_TREE_STRUCTURE
/*
 *  ParseHFString()
 *
 *  Function:  Parses an OID descriptor and instances into "pieces".
 *             A "piece" is either alphanumeric or dotted-decimal.
 *
 *  Inputs:    string - the string to parse.
 *
 *  Outputs:   On failure, return NULL.  On success, returns a token list.
 *
 *  Notes:     The input string is not changed.
 */
char **
ParseHFString(const char *string);
#endif /* SR_MIB_TREE_STRUCTURE */


/*
 *  MakeOIDFromHFDot()
 *
 *  Function:  Attempts to convert a text string into an OID, similar to
 *             MakeOIDFragFromHFDot().  This function checks the converted
 *             OID to verify it is in a valid subtree (subtrees 1, 2, 3) 
 *             from the root.
 *
 *  Inputs:    text_str - the input text string to convert.
 *
 *  Outputs:   On success, returns an OID.  On failure, returns NULL.
 *
 *  Notes:     See the notes for MakeOIDFragFromHFDot().
 */
OID  *
MakeOIDFromHFDot(const char *text_str);


/*
 *  MakeOIDFragFromHFDot()
 *
 *  Function:  Attempts to convert a text string into an OID, similar to
 *             MakeOIDFragFromDot().  This function accepts table object
 *             instances specified in formats that are more human-friendly
 *             than MakeOIDFragFromDot() accepts.  The text string can be
 *             of the forms:
 *
 *             -  a string of dotted-decimal numbers (e.g., 1.3.6.1.2.1.1.1);
 *             -  an OID descriptor (e.g., sysDescr);
 *             -  an OID descriptor with dotted-decimal instance information
 *                (e.g., sysDescr.0);
 *             -  an OID descriptor with instancing information specified
 *                as human-friendly text strings, like:
 *                -  schedDay.NetOp.Calendar_00
 *                -  vacmViewTreeFamilyType.NetObjs.iso
 *                -  usmUserPublic.12.0.0.0.99.0.0.0.161.192.147.142.47.Guest
 *             -  an dotted-decimal OID name with instancing information
 *                specified as human-friendly text strings, like:
 *                -  1.3.6.1.2.1.63.1.2.1.7.NetOp.Calendar_00
 *                -  1.3.6.1.6.3.16.1.5.2.1.4.NetObjs.iso
 *                -  1.3.6.1.6.3.15.1.2.2.1.11.  \
 *                       12.0.0.0.99.0.0.0.161.192.147.142.47.  \
 *                       Guest
 *
 *  Inputs:    text_str - the input text string to convert.
 *
 *  Outputs:   On success, returns an OID.  On failure, returns NULL.
 *
 *  Notes:     This function is intended to be used to convert user input.
 *             It has alot of error-checking and a longer code path than
 *             does MakeOIDFragFromDot().
 *
 *             Unlike MakeOIDFragFromDot(), this function cannot convert
 *             OID strings that include SIDs in hexadecimal or octal.
 *
 *             For converting text strings with human-friendly instance info-
 *             rmation, this function must be able to identify the MIB object,
 *             determine if the MIB object is a table MIB object, and deter-
 *             mine the types of the table indices.  If this function cannot 
 *             determine the types of the table indices, it cannot convert 
 *             the human-friendly instance information into the dotted-deci-
 *             mal forms.
 *
 *             If the input text string specifies a table MIB object with a
 *             table index specified as an OID descriptor (e.g., "iso"), the
 *             OID descriptor must be resolvable through snmpinfo.dat.  If
 *             the OID descriptor cannot be resolved, this function cannot
 *             convert the human-friendly instance information into the dot-
 *             ted-decimal form.
 */
OID  *
MakeOIDFragFromHFDot(const char *text_str);

/*
 *  Constants that specify the behavior of the MakeHFDotFromOID() API.
 */
#define SR_HF_NUMBER_FORM       0x1 /* display OIDs in number form */
#define SR_HF_BRACKETS          0x2 /* display indexes in brakets */
#define SR_HF_VERBOSE_INDEX     0x4 /* display index in the form [name=index] */
#define SR_HF_INDEX_NAME_FORM   0x8 /* display OctetString indexes as txt str */
#define SR_HF_ABBREVIATE_NAME   0x10 /* abbreviate by skipping common prefix */
#define SR_HF_SUPPRESS_LEAF_INDEX  0x20 /* only show index for container */
#define SR_HF_SUPPRESS_ROW_NAME    0x40 /* don't show descriptor for rows */
#define SR_HF_INDEX_AT_EACH_LEVEL  0x80 /* show index at each containment level */
#define SR_HF_PRINT_UNIQUE_INDICES  0x400 /* print unique indices only */

/*
 *  MakeHFDotFromOID()
 *
 *  Function:  Converts an OID into a text string, similar to MakeDotFromOID().
 *             If possible, this function converts table MIB object instances
 *             into human-friendly strings, unlike MakeDotFromOID().
 *
 *  Inputs:    oid      - the OID to convert.
 *             buffer   - points to a character buffer in which to return
 *                        the text string result.
 *             bufsize  - the number of characters in the return buffer.
 *             behavior - the desired behavior
 *
 *  Notes:     This function is intended to be used to render user output.
 *             It has a longer code path than does MakeOIDFragFromDot().
 */
int  
MakeHFDotFromOID(const OID  *oid, 
                 char       *buffer, 
                 int        bufsize, 
                 int        behavior);





#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_OIDTRANH_H */
