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
 *   Contains constants and prototypes for formatting/printing varbinds.
 */


#include "sr_conf.h"

#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#ifdef SR_MIB_TREE_STRUCTURE
#include "mib_tree.h"
#endif  /* SR_MIB_TREE_STRUCTURE */
#include "oid_lib.h"
#include "diag.h"
#include "frmt_lib.h"
#include "tc_time.h"
#include "oidtranh.h"
#include "fmtvb.h"
SR_FILENAME

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
 *    
 *             This function still needes range checking for Counter64 and
 *             Opaque types.
 */
int
FPrintVarBindValue(
    const VarBind *vb, 
    char          *buffer, 
    const char    *name,
    const int     buflen)
{
    int          i, IsUnprintable, rc;
    char         *str = NULL;
    OctetString  tmp_os;
    /* --- Put the string terminating NULL into output buffer --- */
    buffer[0] = '\0';

    /* --- Handle the different VarBind types --- */
    switch (vb->value.type) {

        /* - Handle unsigned integers including COUNTER_32 and GAUGE_32 - */
        case COUNTER_TYPE:
        case GAUGE_TYPE:
            if (buflen < 11) {
                return -1;
            }
            snprintf(buffer, buflen, "%lu", (unsigned long) vb->value.ul_value);
            break;

        /* --- Render the TimeTicks in human-friendly form --- */
        case TIME_TICKS_TYPE:
            rc = SPrintTimeTicks(vb->value.ul_value, buffer, buflen);
            if (rc == -1) {
                return -1;
            }
            break;

        /* --- Handle signed integers includes INTEGER_32 --- */
        case INTEGER_TYPE:

            if (name) {
                /* --- Check for string enumerations --- */
                str = GetType(name, vb->value.sl_value);
                if (buflen < strlen(str)) {
                    return -1;
                }
                snprintf(buffer, buflen, "%s", str);
            } else {
                /* --- Render the integer value --- */
                if (buflen < 11) {
                    return -1;
                }
                snprintf(buffer, buflen, INT32_FORMAT, vb->value.sl_value);
            }
            break;

        /* --- Handle IP addresses --- */
        case IP_ADDR_PRIM_TYPE:
            if (buflen < 16) {
                return -1;
            }
            snprintf(buffer, buflen, "%d.%d.%d.%d",
                   vb->value.os_value->octet_ptr[0],
                   vb->value.os_value->octet_ptr[1],
                   vb->value.os_value->octet_ptr[2],
                   vb->value.os_value->octet_ptr[3]);
            break;

        /* --- Handle object ID types --- */
        case OBJECT_ID_TYPE:

            /* --- Estimate buffer size required to render the OID --- */
            rc = vb->value.oid_value->length;
            rc = rc * 11 + 64;

            /* --- Return if string buffer is too small --- */
            if (buflen < rc) {
                return -1;
            }

            /* --- Attempt to render the OID --- */
            if (MakeDotFromOID(vb->value.oid_value, buffer) == -1) {
                return (-1);
            }
            break;

        /* --- Handle octet string types --- */
        case OCTET_PRIM_TYPE:

            /* --- Handle quasi-octet strings --- */
            str = GetBitsType(name, vb->value.os_value);
            if (str != NULL) {
                strncpy(buffer, str, strlen(str));
                free(str);
                buffer[strlen(buffer)] = '\0';
                break;
            }

            /* --- Attempt to render DateAndTime in human-friendly form --- */
            if (!SPrintDateAndTime(vb->value.os_value, buffer, buflen)) {
                break;
            }

            /* --- Fall through --- */

        /* --- Handle opaque octets --- */
        case OPAQUE_PRIM_TYPE:

            /* --- Convenience variable --- */
            tmp_os.octet_ptr = vb->value.os_value->octet_ptr;
            tmp_os.length    = vb->value.os_value->length;

            /* --- Determine whether the octets are printable or not --- */
            IsUnprintable = 0;
            for (i = 0; i < tmp_os.length; i++) {
                rc = (int) tmp_os.octet_ptr[i];
                if (!isprint(rc)) {
                    IsUnprintable = 1;
                    break;
                }
            }

            /*
             *  If needed, adjust the length of tmp_os so when rendered, 
             *  tmp_os will fit in the output buffer.  If the octet string 
             *  is unprintable, then assume each octet will be rendered as 
             *  2 hex digits with a trailing space.  This is 3 octets in 
             *  the output string buffer for each octet in the rendered 
             *  octet string.  If the octet string is printable, then assume
             *  each octet will be rendered in one octet in the output buffer.
             *  Subtract 1 character for the NULL string terminator.
             */
            if (IsUnprintable == 1) {
                rc = buflen / 5;  /* Up to 5 bytes per octet */
                if (tmp_os.length > rc) {
                    tmp_os.length = rc - 1;
                }
            }
            else {
                if (tmp_os.length > buflen) {
                    tmp_os.length = buflen - 1;
                }
            }

            /* --- Now render --- */
            if (IsUnprintable == 1) {
                SPrintOctetString(&tmp_os, 16, buffer);
            }
            else {
                SPrintAscii(&tmp_os, buffer);
            }

            buffer[strlen(buffer)+1] = '\0';
            break;

        /* --- Handle NULL types --- */
        case NULL_TYPE:
            if (buflen < 16) {
                return -1;
            }
            strcpy(buffer, "NULL TYPE/VALUE");
            break;

        /* --- Handle Counter64 types --- */
        case COUNTER_64_TYPE:
            if (buflen < 21) {
                return -1;
            }
            SPrintCounter64(vb->value.uint64_value, buffer);
            break;


        /* --- Handle NO_SUCH_OBJECT_EXCEPTIONs --- */
        case NO_SUCH_OBJECT_EXCEPTION:
            if (buflen < 25) {
                return -1;
            }
            snprintf(buffer, buflen, "NO_SUCH_OBJECT_EXCEPTION");
            break;

        /* --- Handle NO_SUCH_INSTANCE_EXCEPTIONs --- */
        case NO_SUCH_INSTANCE_EXCEPTION:
            if (buflen < 27) {
                return -1;
            }
            snprintf(buffer, buflen, "NO_SUCH_INSTANCE_EXCEPTION");
            break;

        /* --- Handle END_OF_MIB_VIEW_EXCEPTIONs --- */
        case END_OF_MIB_VIEW_EXCEPTION:
            if (buflen < 26) {
                return -1;
            }
            snprintf(buffer, buflen, "END_OF_MIB_VIEW_EXCEPTION");
            break;

        /* --- Error --- */
        default:
            return -1;
    }

    /* --- Normal return --- */
    return 0;
}


/*
 *  FormatVarBind()
 *
 *  Function:  This is a generic API for rendering VarBinds.  It can render
 *             them either to stdout or into a character buffer, and it can
 *             render them either with either dotted-decimal or user-friendly
 *             instance information.
 *
 *  Inputs:    vb       - points to the VarBind to render.
 *             behavior - specifies the desired API behavior using constants
 *                        defined in the corresponding header file.
 *             buffer   - points to a pointer to a character string buffer 
 *                        in which to render the VarBind.  If the pointer to
 *                        the character string buffer is NULL, this will mal-
 *                        loc a new buffer and return it.
 *             buflen   - specifies the length of the string buffer.
 *
 *  Outputs:   On success, returns 0.  On failure, it returns -1.
 *             On success, this function also will render the VarBind
 *             either to stdout or to the character return buffer.
 *
 *  Notes:     To render into a memory buffer instead of rendering to stdout, 
 *             the SR_FMTVB_BUFFER behavior flag must be set.  To render into
 *             a memory buffer supplied by the caller, the 'buffer' argument
 *             must be non-NULL (in addition to the SR_FMTVB_BUFFER behavior 
 *             flag being set.)  If no 'buffer' argument is passed (and the
 *             SR_FMTVB_BUFFER behavior flag is set), this function will
 *             allocate a result buffer that must be freed by the caller.
 */
int
FormatVarBind(
    const VarBind *vb, 
    const int behavior, 
    char  **buffer,
    const int buflen)
{
   /*
    *  OID_Descriptor is a character buffer where the human-friendly OID
    *  descriptor is extracted.  It's length is the max length of an OID
    *  descriptor (from RFC 1902, section 3.1) plus 1 for a terminating
    *  NULL character.
    */
   int           allocated, bufsize_needed;
   int           oidsize, pos, rc, valuesize;
   char          OID_Descriptor[65], *result = NULL;
   OID           *oid = NULL;
   ObjectSyntax  *value = NULL;


   /*
    *   Error check the inputs.
    */

   /* --- Must have a VarBind --- */
   if (vb == NULL) {
       return -1;
   }

   /* --- Check if a buffer was specified --- */
   if ((behavior & SR_FMTVB_BUFFER) == SR_FMTVB_BUFFER) {

       /* --- Pointer to the pointer is NULL --- */
       if (buffer == NULL) {
           return -1;
       }

       /* --- If a buffer was passed, check it's length --- */
       if ((*buffer != NULL) && (buflen < 1)) {
           return -1;
       }
   }

   /* --- Convenience variables --- */
   oid     = vb->name;
   value = (ObjectSyntax *) &(vb->value);


   /*
    *   Estimate the size of the character buffer required.
    */

   /* --- Estimate number of chars needed for rendering the OID --- */
   oidsize = oid->length * 11;

   /* --- Estimate number of chars needed for rendering the value --- */
   switch (value->type) {
       case INTEGER_TYPE:
       case COUNTER_TYPE:
       case GAUGE_TYPE:
       case TIME_TICKS_TYPE:
           valuesize = 11;   /* Big enough for 4 billion */
           break;
       case IP_ADDR_PRIM_TYPE:
           valuesize = 16;   /* Big enough for xxx.yyy.zzz.www */
           break;
       case OCTET_PRIM_TYPE:
           valuesize = value->os_value->length;
           break;
       case OBJECT_ID_TYPE:
           valuesize = value->oid_value->length * 11;
           break;
       case COUNTER_64_TYPE:
       default:
           valuesize = 32;   /* Gotta pick some number */
           break;
   }

   /* --- Extra chars for equals, trailing NULL, etc. --- */
   bufsize_needed = oidsize + valuesize + 8;

   /*
    *  This function can return the result either in a formatted memory buffer
    *  or to stdout.  If the SR_FMTVB_BUFFER flag is set, this indicates the
    *  result should be returned in a memory buffer.  If *buffer is NULL, this
    *  function allocates the result buffer; if *buffer is non-NULL, this
    *  function formats into a memory buffer passed in as an argument.
    *
    *  To format into a memory buffer, the buffer must be large enough to hold
    *  the formatted result.  The buffer must be at least large enough to hold 
    *  the object name and at least partial value or string indicating value 
    *  too large to be printed to buffer.
    */
   allocated = 0;
   if (((behavior & SR_FMTVB_BUFFER) == SR_FMTVB_BUFFER)) {

       /* --- Format into a memory buffer --- */
       if (*buffer == NULL) {

           /* --- Allocate a result buffer for return to caller --- */
           result = (char *) malloc(bufsize_needed);
           if (result == NULL) {
               return -1;
           }

           allocated = -1;
       }
       else {
           /* --- Reuse a passed in memory buffer for return to caller --- */
           if (buflen < (oidsize + 40)) {
               return -1;
           }

           bufsize_needed = buflen;
           result = *buffer;
       }
   }
   else {

       /* --- Allocate a result buffer for rendering to stdout --- */
       result = (char *) malloc(bufsize_needed);
       if (result == NULL) {
           return -1;
       }

       allocated = -1;
   }

   /* --- Clear the memory buffer --- */
#ifdef SR_CLEAR_MALLOC
   memset(result, 0, bufsize_needed);
#else /* SR_CLEAR_MALLOC */
   result[0] = '\0';
#endif /* SR_CLEAR_MALLOC */


   /*
    *   Render the VarBind.
    */

   /* --- Render the OID with user-friendly instances if possible --- */
   if ((behavior & SR_FMTVB_NOUF) == SR_FMTVB_NOUF) {
       rc = MakeDotFromOID(oid, result);
   }
   else {
       rc = MakeHFDotFromOID(oid, result, bufsize_needed, 0);
   }

   /* --- Return if the OID rendering failed --- */
   if (rc != 0) {
       if (allocated != 0) {
           free(result);
       }
       return -1;
   }

   /*
    *  Copy the OID descriptor into another buffer.  The OID descriptor
    *  is needed so that SPrintVarBindValue() can map integer values into
    *  their string equivalents.
    */
   strncpy(OID_Descriptor, result, sizeof(OID_Descriptor));

   /* --- Change the first dot into a NULL character --- */
   pos = strcspn(OID_Descriptor, ".");
   OID_Descriptor[pos] = '\0';

   /* --- Append the equals sign --- */
   result[pos]   = ' ';
   result[pos+1] = '=';
   result[pos+2] = ' ';
   result[pos+3] = '\0';

   /*
    *  Print name to the console if the SR_FMTVB_BUFFER flag is not set.
    */
   if ((behavior & SR_FMTVB_BUFFER) != SR_FMTVB_BUFFER) {
       printf("%s\n", result);
   }

   /*
    *  At this point, result has the VarBind descriptor in it.  Call
    *  FPrintVarBindValue() to render the value.  Do some pointer 
    *  funniness so that we don't have to malloc a second buffer.
    */
   pos = strlen(result);
   rc  = FPrintVarBindValue(vb, &(result[pos]), 
                            OID_Descriptor, bufsize_needed-pos);


   /* --- Return if the value rendering failed --- */
   if (rc != 0) {

       if ( ((behavior & SR_FMTVB_BUFFER) == SR_FMTVB_BUFFER) &&
            (*buffer != NULL)) {
           strcpy(*buffer+pos-1, "<value too large to be printed from this routine>");
       } 

       if (allocated != 0) {
           free(result);
       }

       return -1;
   }


   /*
    *  Decide what to do with the result.
    */
   if ((behavior & SR_FMTVB_BUFFER) == SR_FMTVB_BUFFER) {

           /* --- Return the buffer malloc'ed here */
           *buffer = result;
           result  = NULL;
    }
    else {
        printf("%s\n", result+pos);
    }


    /* --- Clean up and return --- */
    if ((result != NULL) && (allocated != 0)) {
        free(result);
    }

    return 0;
}   /* FormatVarBind() */

