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
 * Contains constants and prototypes for formatting/printing varbinds.
 */

#ifndef SR_FMTVB_H
#define	SR_FMTVB_H

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  FPrintVarBindValue()
 *
 *  Function:  This routine sprint's out the value of a single varbind
 *             in a human-readable form.
 *
 *  Inputs:    vb       - points to the VarBind to render.
 *             buffer   - points to a pointer to a character string buffer
 *                        in which to render the VarBind.  If the pointer to
 *                        the character string buffer is NULL, this will mal-
 *                        loc a new buffer and return it.
 *             name     - is a character-string representation of the VarBind's
 *                        OID.  This is needed for rendering enumerations.
 *             buflen   - specifies the length of the string buffer.
 *
 *  Outputs:   On success, returns 0.  On failure, it returns -1.
 *             On success, this function also will render the VarBind's value.
 *
 *  Notes:     This function is upwardly-compatible with SPrintVarBindValue().
 *             This function adds a buffer length value for length-checking.
 */
int
FPrintVarBindValue(
    const VarBind *vb,
    char          *buffer,
    const char    *name,
    const int     buflen);


/*
 *  Constants that specify the behavior of the FormatVarBind() API.
 *
 *  SR_FMTVB_DEFAULT  - Render to stdout with user-friendly OID instances.
 *
 *  SR_FMTVB_NOUF     - Render OID instances with dotted-decimal numbers
 *                      instead of user-friendly instances.
 *
 *  SR_FMTVB_BUFFER   - Render to a character buffer instead of to stdout.
 */
#define SR_FMTVB_DEFAULT  0x00
#define SR_FMTVB_NOUF     0x01
#define SR_FMTVB_BUFFER   0x10


/*
 *  FormatVarBind()
 *  
 *  Function:  This is a generic API for rendering VarBinds.  It can render
 *             them either to stdout or into a character buffer, and it can
 *             render them either with either dotted-decimal or user-friendly
 *             instance information.
 *  
 *  Inputs:    vb       - points to the VarBind to render.
 *             behavior - specifies the desired API behavior using the
 *                        constants defined above.
 *             buffer   - points to a pointer to a character string buffer 
 *                        in which to render the VarBind.  If the pointer to
 *                        the character string buffer is NULL, this will mal-
 *                        loc a new buffer and return it.
 *             buflen   - specifies the length of the string buffer.
 *  
 *  Outputs:   On success, returns 0.  On failure, it returns -1.  
 *             On success, this function also will render the VarBind
 *             either to stdout or to the character return buffer.
 */
int
FormatVarBind(
    const VarBind *vb, 
    const int behavior, 
    char  **buffer, 
    const int buflen);


#ifdef  __cplusplus
}
#endif

#endif	/* SR_FMTVB_H */
