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


#include <ctype.h>

#include <string.h>

#include <malloc.h>

#include <stdlib.h>


#define WANT_ALL_ERRORS

#include "sr_snmp.h"
#include "oid_lib.h"
#include "diag.h"
#include "tc_time.h"
SR_FILENAME

#include "frmt_lib.h"
#include "prnt_lib.h"


/*
 * SPrintOctetString:
 *
 *   This routine sprint's the contents  of  a  Octet  String's
 *   value  in  hex.   It  is called with a pointer to the Octet-
 *   String construct and the number of bytes to display  on  one
 *   line  (the  variable  'wrap').
 *
 * 11/2/93 DBL
 * Created this function from PrintOctetString
 *
 * This function can be used to sprintf a packet (instead of providing
 * an SPrintPacket function).
 */

void
SPrintOctetString(oct_ptr, wrap, buf)
    const OctetString *oct_ptr;
    int             wrap;
    char           *buf;
{
    SR_INT32        i;
    SR_INT32        pos;
    char            buffer[8];

    strcpy(buf, "");
    for (i = 0, pos = 1; i < oct_ptr->length; i++) {
	if (((i % wrap) == 0) && (oct_ptr->length > 8)) {
	    strcat(buf, "\n");
	}

	snprintf(buffer, sizeof(buffer), "%2.2x ", oct_ptr->octet_ptr[i]);
        strcat(buf, buffer);

	if ((oct_ptr->length >= 8) || ((oct_ptr->length % 4) == 0)) {
	    if ((pos % 2) == 0) {
		strcat(buf, " ");
	    }
	    if ((pos % 4) == 0) {
		strcat(buf, " ");
	    }
	    if ((pos % 8) == 0) {
		strcat(buf, " ");
	    }
	}

	if ((pos % wrap) == 0) {
	    pos = 1;
	}
	else {
	    pos++;
	}
    }
}     /* PrintOctetString() */

#ifdef WANT_SPRINTVARBINDSTUFF
/*
 * SPrintVarBindList:
 *
 *   This routine sprints out the contents of a varbind list in  a
 *   human  readable  form.   
 */ 
int
SPrintVarBindList(const VarBind *vb_ptr, char *buf)
{
    int result;
 
    if (vb_ptr == NULL) {
        return (1);
    }
    while (vb_ptr != NULL) {
        result = SPrintVarBind(vb_ptr, buf);
        buf += strlen(buf);
        sprintf(buf, "\n");
        buf += strlen(buf);
        if (result) {
            return (result);
        }
        vb_ptr = vb_ptr->next_var;
    }
    return(0);
}                               /* end of SPrintVarBindList() */

/*
 * SPrintVarBind: 
 *
 *   This routine sprint's out the contents of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *   The output is in the form "name = value"
 *
 * 11/1/93 DBL
 * Pulled printing code out of PrintVarBindList into this function.
 */

int
SPrintVarBind(vb_ptr, buf)
    const VarBind  *vb_ptr;
    char           *buf;
{
    FNAME("SPrintVarBind")
    char *buffer1 = NULL;
    int rval = 0;

    buffer1 = malloc(sr_pkt_size+256);
    if (buffer1 == NULL) {
        DPRINTF((APERROR, "%s: malloc failed.\n", Fname));
        return -1;
    }
        if (MakeDotFromOID(vb_ptr->name, buf) == -1) {
	    DPRINTF((APTRACE, "%s, vb_ptr->name:\n", Fname));
            free(buffer1);
            buffer1 = NULL;
	    return (-1);
        }
        strcpy(buffer1, buf);
        strcat(buf, " = ");
        rval = SPrintVarBindValue(vb_ptr, &(buf[strlen(buf)]), buffer1);
        free(buffer1);
        buffer1 = NULL;
        return(rval);
}

/*
 * SPrintVarBindValue: 
 *
 *   This routine sprint's out the value of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *
 * 11/3/93 DBL
 * Pulled printing code out of PrintVarBind into this function.
 */


int
SPrintVarBindValue(vb_ptr, buf, name)
    const VarBind  *vb_ptr;
    char           *buf;
    const char     *name;
{
    FNAME("SPrintVarBindValue")
    char           *buffer = NULL;  /* used only for OBJECT_ID_TYPE,
                                       allocated only for OBJECT_ID_TYPE */
    char           *bits;
    OctetString     tmp_os;

    switch (vb_ptr->value.type) {
        case COUNTER_TYPE:		/* handle unsigned integers includes
				 	 * COUNTER_32 */
        case GAUGE_TYPE:		/* includes GAUGE_32 */
	    sprintf(buf, "%lu", (unsigned long)vb_ptr->value.ul_value);
	    break;

        case TIME_TICKS_TYPE:
            /*
             *  Render the TimeTicks in human-friendly form.
             *
             *  The output buffer size is not available, so use 
             *  the minimum size accepted by SPrintTimeTicks().
             */
            SPrintTimeTicks(vb_ptr->value.ul_value, buf, 22);
	    break;

        case INTEGER_TYPE:	/* handle signed integers includes INTEGER_32 */
            if (name) {
	        sprintf(buf, "%s", GetType(name, vb_ptr->value.sl_value));
            } else {
	        sprintf(buf, INT32_FORMAT, vb_ptr->value.sl_value);
            }
	    break;

        case IP_ADDR_PRIM_TYPE:
	    sprintf(buf, "%d.%d.%d.%d",
	           vb_ptr->value.os_value->octet_ptr[0],
	           vb_ptr->value.os_value->octet_ptr[1],
	           vb_ptr->value.os_value->octet_ptr[2],
	           vb_ptr->value.os_value->octet_ptr[3]);
	    break;

        case OBJECT_ID_TYPE:
            buffer = malloc(sr_pkt_size+256);
            if (buffer == NULL) {
                DPRINTF((APERROR, "%s: malloc failed.\n", Fname));
                return (-1);
            }
	    buffer[0] = '\0';
	    if (MakeDotFromOID(vb_ptr->value.oid_value, buffer) == -1) {
	        DPRINTF((APTRACE, "%s: vb_ptr->value.oid_value:\n", Fname));
                free(buffer);
                buffer = NULL;
	        return (-1);
	    }
	    sprintf(buf, "%s", buffer);
            free(buffer);
            buffer = NULL;
	    break;

        case OCTET_PRIM_TYPE:	/* handle quasi-octet strings */
	    if ((bits = GetBitsType(name, vb_ptr->value.os_value)) != NULL) {
		strcpy(buf, bits);
		free(bits);
		break;
	    }

            /*
             *  Attempt to render DateAndTime in human-friendly form.
             *
             *  The output buffer size is not available, so use 
             *  the minimum size accepted by SPrintDateAndTime().
             */
            if (!SPrintDateAndTime(vb_ptr->value.os_value, buf, 32)) {
                break;
            }

	    /*FALLTHROUGH*/

        case OPAQUE_PRIM_TYPE:
            /* 
             * Check for large octet string values. User can look at packet
             * output for full value. This will keep us from writing past the
             * end of our incoming buffer.
             */
            if (vb_ptr->value.os_value->length > 256) {
                DPRINTF((APTRACE, "%s: Length of OctetString value > 256, truncating to 256 for debug output.\n", Fname));
                tmp_os.octet_ptr = vb_ptr->value.os_value->octet_ptr;
                tmp_os.length = 256;
	        if (SPrintAscii(&tmp_os, buf) < 0) {
                    /* cannot print ascii */
	            SPrintOctetString(&tmp_os, 16, buf);
	        }
            }
            else {
	        if (SPrintAscii(vb_ptr->value.os_value, buf) < 0) {
                    /* cannot print ascii */
	            SPrintOctetString(vb_ptr->value.os_value, 16, buf);
	        }
            }
	    break;

        case NULL_TYPE:
	    strcpy(buf, "NULL TYPE/VALUE");
	    break;

        case COUNTER_64_TYPE:
            SPrintCounter64(vb_ptr->value.uint64_value, buf);
	    break;
        case NO_SUCH_OBJECT_EXCEPTION:
	    sprintf(buf, "NO_SUCH_OBJECT_EXCEPTION");
	    break;

        case NO_SUCH_INSTANCE_EXCEPTION:
	    sprintf(buf, "NO_SUCH_INSTANCE_EXCEPTION");
	    break;

        case END_OF_MIB_VIEW_EXCEPTION:
	    sprintf(buf, "END_OF_MIB_VIEW_EXCEPTION");
	    break;

        default:
	    DPRINTF((APTRACE, "%s: Illegal type: 0x%x\n",
                     Fname, vb_ptr->value.type));
	    return (0);

    };				/* end of switch */

    /* do next one now */
    return (0);
}				/* end of SPrintVarBindValue() */
#endif /* WANT_SPRINTVARBINDSTUFF */

/*
 * SPrintAscii:
 *
 *   This routine sprint's out the contents  of  a  Octet  String's
 *   value  as  an ascii string if the value only contains print-
 *   able characters.  It is called with a pointer to the  Octet-
 *   String  construct and checks if the string is printable.  If
 *   it is not it returns a -1 value, otherwise it returns a 1.
 *
 * 11/1/93 DBL
 * Converted PrintAscii to this function.
 */
int
SPrintAscii(os_ptr, buf)
    const OctetString *os_ptr;
    char           *buf;
{
    SR_INT32        i;
    char            buffer[4];

    for (i = 0; i < os_ptr->length; i++) {
	/* ASCII space is not a printable character in MSC */
	if (((!isprint((int) os_ptr->octet_ptr[i])) && 
             (os_ptr->octet_ptr[i] != 0x20) &&
	     (os_ptr->octet_ptr[i] != 0x09) && 
	     (os_ptr->octet_ptr[i] != 0x0a) && 
             (os_ptr->octet_ptr[i] != 0x0d)) && 
             (!((os_ptr->octet_ptr[i] == 0x00) && 
                (i == os_ptr->length - 1)))) {
	    return (-1);
	}
    }

    strcpy(buf, "");
    for (i = 0; i < os_ptr->length; i++) {
	    sprintf(buffer, "%c", os_ptr->octet_ptr[i]);
	    strcat(buf, buffer);
    }

    return (1);
}     /* SPrintAscii() */


/*
 *  SPrintOID:
 *
 *  This routine sprint's out an OID.
 */
int
SPrintOID(const OID *oid, char *buf)
{
    char      *BufPos;
    SR_INT32  i;

    if (buf == NULL) {
        return 0;
    }
  
    if (oid == NULL) {
        sprintf(buf, "%c", '\0');
        return 0; 
    }
 
    BufPos = buf; 
  
    for (i = 0; i < oid->length; i++) {
        sprintf(BufPos, "%lu.%c", (unsigned long) oid->oid_ptr[i], '\0');
        BufPos = buf + strlen(buf);
    }
   
    BufPos--;
    *BufPos = '\0';

    return 0;
}



/*
 *  SPrintCounter64:
 *
 *  This routine sprint's out the contents  of a Counter64 value.
 */
int
SPrintCounter64(ctr64_ptr, buf)
    const UInt64   *ctr64_ptr;
    char           *buf;
{

  #define SR_C64_MAX_NUM 4440000
  unsigned long int nums[7], a;
  int x;
  
   if(ctr64_ptr == NULL)  {
     return (-1);
   }

    /* --- If we're in the Counter32 range, life is easy --- */
    if (ctr64_ptr->big_end == 0) {
        sprintf(buf, "%lu", (unsigned long) ctr64_ptr->little_end);
        return 1;
    }

    /* --- If we've got a real, live Counter64, then calculate and store --- */

     for(x = 0; x < 7; x++)  { 
        nums[x] = 0;
      }

         a = ctr64_ptr->big_end;

         while( a > SR_C64_MAX_NUM) {      /* calculate value of big_end */
           math_loop(SR_C64_MAX_NUM,nums);
           a -= SR_C64_MAX_NUM;
          }

         math_loop(a,nums);
  
         if(nums[4] >= 1000 )  {	/* handle carry over a trillion */
          nums[5] = (nums[4] / 1000);
          nums[4] = nums[4] % 1000;
         }

        if(nums[5] >= 1000 )  {
          nums[6] = nums[5] / 1000;
          nums[5] = nums[5] % 1000;
        }
     
        a = ctr64_ptr->little_end;	/* get value of little_end and add on */
        for(x = 0; x < 4; x++)    {
            nums[x] += a % 1000;
            if(nums[x] >= 1000)   {
              nums[x+1] += nums[x] / 1000;
              nums[x] = nums[x] % 1000;
            }
           a = a / 1000;
         }
 
          if(nums[4] >= 1000)   {		/* handle possible carry */
              nums[5] += nums[4] / 1000;
              nums[4] = nums[4] % 1000;
            }
 
         
          if(nums[6])
            sprintf(buf,"%lu%03lu%03lu%03lu%03lu%03lu%03lu",nums[6],nums[5],nums[4],nums[3],nums[2],nums[1],nums[0]);
          else if (nums[5])
            sprintf(buf,"%lu%03lu%03lu%03lu%03lu%03lu",nums[5],nums[4],nums[3],nums[2],nums[1],nums[0]);
          else if (nums[4])
            sprintf(buf,"%lu%03lu%03lu%03lu%03lu",nums[4],nums[3],nums[2],nums[1],nums[0]);
          else
            sprintf(buf,"%lu%03lu%03lu%03lu",nums[3],nums[2],nums[1],nums[0]);
     

    return 1;
}				/********* end of function SPrintCounter64    *****************/

/*
 * SPrintErrorCode:
 */
void
SPrintErrorCode(SR_INT32 error_index, SR_INT32 error_status, char *buf)
{

    switch ((short) error_status) {
    case TOO_BIG_ERROR:
	sprintf(buf, "Return packet too big.\n");
	break;
    case NO_SUCH_NAME_ERROR:
	sprintf(buf, "No such variable name.  Index:  %d.\n", (int)error_index);
	break;
    case BAD_VALUE_ERROR:
	sprintf(buf, "Bad variable value.  Index:  %d.\n", (int)error_index);
	break;
    case READ_ONLY_ERROR:
	sprintf(buf, "Read only variable:  %d.\n", (int)error_index);
	break;
    case GEN_ERROR:
	sprintf(buf, "General error:  %d.\n", (int)error_index);
	break;
#ifdef SR_SNMPv2_PDU
    case NO_ACCESS_ERROR:
	sprintf(buf, "NO_ACCESS_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_TYPE_ERROR:
	sprintf(buf, "WRONG_TYPE_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_LENGTH_ERROR:
	sprintf(buf, "WRONG_LENGTH_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_ENCODING_ERROR:
	sprintf(buf, "WRONG_ENCODING_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_VALUE_ERROR:
	sprintf(buf, "WRONG_VALUE_ERROR: %d.\n", (int)error_index);
	break;
    case NO_CREATION_ERROR:
	sprintf(buf, "NO_CREATION_ERROR: %d.\n", (int)error_index);
	break;
    case INCONSISTENT_VALUE_ERROR:
	sprintf(buf, "INCONSISTENT_VALUE_ERROR: %d.\n", (int)error_index);
	break;
    case RESOURCE_UNAVAILABLE_ERROR:
	sprintf(buf, "RESOURCE_UNAVAILABLE_ERROR: %d.\n", (int)error_index);
	break;
    case COMMIT_FAILED_ERROR:
	sprintf(buf, "COMMIT_FAILED_ERROR: %d.\n", (int)error_index);
	break;
    case UNDO_FAILED_ERROR:
	sprintf(buf, "UNDO_FAILED_ERROR: %d.\n", (int)error_index);
	break;
    case AUTHORIZATION_ERROR:
	sprintf(buf, "AUTHORIZATION_ERROR: %d.\n", (int)error_index);
	break;
    case NOT_WRITABLE_ERROR:
	sprintf(buf, "NOT_WRITABLE_ERROR: %d.\n", (int)error_index);
	break;
    case INCONSISTENT_NAME_ERROR:
	sprintf(buf, "INCONSISTENT_NAME_ERROR: %d.\n", (int)error_index);
	break;

#endif /* SR_SNMPv2_PDU */
    default:
	sprintf(buf, "Unknown status code:  %d.\n", (int)error_status);
	break;
    };

}     /* SPrintErrorCode() */
