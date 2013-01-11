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


#include <string.h>


#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "oid_lib.h"
#include "scan.h"
#include "min_v.h"
#include "diag.h"
SR_FILENAME

/*
 * The functions in this file use realloc().  The compat.h
 * file provides a vital fix/workaround for SunOS 4.1.4.
 */
#include "compat.h"


int
CheckScalarInstance(incoming, object, searchType)
    OID *incoming;
    ObjectInfo *object;
    int searchType;
{
    int instLength = incoming->length - object->oid.length;
    /*
    * Check the object instance.
    *
    * An EXACT search requires that the instance be of length 1 and
    * the single instance element be 0.
    *
    * A NEXT search requires that the requested object does not
    * lexicographically precede the current object type.
    */

    switch (searchType) {
     case EXACT:
       if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
           return 1;
       }
       break;

     case NEXT:
       if (instLength <= 0) {
           return 1;
       }
       break;
    }
    return -1;
}

/*
 * SrBuildReturnVarBind() builds a VarBind by taking the desired value from
 * the structure defined in snmptype.h. A pointer to this structure is passed
 * to this function as void *data. The value of interest is determined
 * by the parameter offset. The ObjectInfo structure is used to determine
 * the object and it's type. OID *inst is the instance.
 */
VarBind *
SrBuildReturnVarBind(short offset,
                     void *data,
                     unsigned char family_type,
                     ObjectInfo *object,
                     OID *inst)
{
    void         *dp;
    void         *member_ptr = (void*)((char *)data+offset);
    SR_UINT32    *ipaddr;
    OctetString **temp_os_pp  = NULL;
    OID         **temp_oid_pp = NULL;
#ifdef SR_SNMPv2_PDU
    UInt64      **temp_c64_pp = NULL;
#endif /* SR_SNMPv2_PDU */

    switch (family_type) {
        case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
        case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
        case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
        case TIME_TICKS_TYPE:
            dp = member_ptr;
            break;
        case OCTET_PRIM_TYPE:
            temp_os_pp = (OctetString **) member_ptr;
            dp = CloneOctetString(*temp_os_pp);
            break;
        case OBJECT_ID_TYPE:
            temp_oid_pp = (OID **) member_ptr;
            dp = CloneOID(*temp_oid_pp);
            break;
        case IP_ADDR_PRIM_TYPE:
            ipaddr = member_ptr;
            dp = IPToOctetString (*ipaddr);
            break;
#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
            temp_c64_pp = (UInt64 **) member_ptr;
            dp = CloneUInt64(*temp_c64_pp);
            break;
#endif /* SR_SNMPv2_PDU */
        default:
            return NULL;
    }
    return (MakeVarBind(object, inst, dp));
}



/* 
 *  Valid Bit method routines specifically for use with MINV code:
 *
 *  ** Note that these routines should be used in place of the existing
 *     macros for manipulating the valid bits if you are accessing the
 *     valid bits array through a pointer rather than directly accessing
 *     the array.
 *
 *
 *  Several of the valid bit macros (ie. ZERO_VALID, SET_ALL_VALID, NEXT_ROW)
 *  expect a valid bit array to be passed into the function call and will not
 *  work properly if a pointer to the valid bits is passed instead. 
 *  Because we use offsets to point to the valid bits in the family data
 *  structures in a number of places in our MINV code, we can't use the 
 *  original valid bits macros. The following functions must be used 
 *  instead. 
 *
 * 
 *        SrZeroValid    	(replaces ZERO_VALID)
 * 	  SrSetAllValid  	(replaces SET_ALL_VALID)
 *        SrNextRow       	(replaces NEXT_ROW)
 *        SrClearNextRowBit     (replaces CLEAR_NEXT_ROW_BIT)
 * 
 */ 
 
void
SrZeroValid(char *valid,
            int highest_nominator)
{
    int nominator = 0;

    /* check for bad parameters */
    if (valid == NULL || highest_nominator < 0) {
        return;
    }

    /* clear all valid bits */
    while (nominator <= highest_nominator) {
        CLR_VALID(nominator, valid);
        nominator++;
    }
}

void
SrSetAllValid(char *valid, int highest_nominator) 
{
   int valid_array_size;

   /* check for bad parameters */
   if (valid == NULL || highest_nominator < 0) {
       return;
   }

   valid_array_size = ((highest_nominator + 1) + 1 + 7) / 8;
   memset(valid, 0xff, valid_array_size);
}


short 
SrNextRow(char *valid, int highest_nominator) 
{
   int valid_array_size;

   /* check for bad parameters */
   if (valid == NULL || highest_nominator < 0) {
       return 0;
   }

   valid_array_size = ((highest_nominator + 1) + 1 + 7) / 8;
   return (valid[valid_array_size-1] & (1 << 7));
}


void 
SrClearNextRowBit(char *valid, int highest_nominator) 
{
   int valid_array_size;
   int next_row_bit_position;

   /* check for bad parameters */
   if (valid == NULL || highest_nominator < 0) {
       return;
   }

   valid_array_size = ((highest_nominator + 1) + 1 + 7) / 8;
   next_row_bit_position = (valid_array_size * 8 - 1) / 8;
   valid[next_row_bit_position] &= ~(1 << ((valid_array_size * 8 - 1) % 8));

}





/*
 * SrCopyFamilyContents - copies the contents of one <family>_t structure
 *                        into another
 *
 * Parameters:
 *
 *    dst - The family structure into which data will be copied
 *          (destination).
 *
 *    src - The family structure to be copied (source).
 *
 *    type_type - The SnmpType table for this family as defined in
 *                k_<base>.stb.
 *
 *    highest_nominator - The largest I_ value for this family, as
 *                        defined in <base>supp.h.
 *
 *         (NOTES)
 *         The SnmpType structure may be user-modified.  The number of
 *         array elements may be larger than nominator.  To prevent the
 *         VALID macro from reaching outside of the valid array, the
 *         highest nominator must be known.
 *         
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
 *         For example, the usmUserEntryTypeTable has Userpart fields
 *         for auth_secret and priv_secret (note k_usmUserCloneUserpartData
 *         is an empty function).
#else
 *         For example, the usmUserEntryTypeTable has a Userpart field
 *         for auth_secret (note k_usmUserCloneUserpartData is an empty
 *         function).
#endif
#endif
 *
 *    valid_offset - The position of the valid array within the family
 *                   structure (as provided by the offsetof() macro).
 *
 *    userpart_clone_func - The k_<family>CloneUserpartData() function
 *                          generated in k_<base>.stb when postmosy is
 *                          run with the -userpart argument.  If no
 *                          function was generated, or if U_<family>
 *                          is not defined, set parameter to NULL.
 *
 *    userpart_free_func - The k_<family>FreeUserpartData() function
 *                         generated in k_<base>.stb when postmosy is
 *                         run with the -userpart argument.  If no
 *                         function was generated, or if U_<family>
 *                         is not defined, set parameter to NULL.
 */

/* %%% DSS add a debug string parameter? */

int
SrCopyFamilyContents(void *dst, void *src,
                     const SnmpType *type_table,
                     int highest_nominator,
                     short valid_offset,
                     SR_COPY_FPTR userpart_clone_func,
                     SR_FREE_FPTR userpart_free_func)
{
    FNAME("SrCopyFamilyContents")
    char *src_valid_ptr = ((char *)src)+valid_offset;
    char *dst_valid_ptr = ((char *)dst)+valid_offset;
    int nominator = 0;
    short member_offset = 0;
    void *src_member_ptr = NULL;
    void *dst_member_ptr = NULL;
    int okay_to_copy_member = 0;
    int okay_to_set_valid_bit = 0;
    OctetString  *temp_os_ptr  = NULL, **temp_os_pp  = NULL;
    OID          *temp_oid_ptr = NULL, **temp_oid_pp = NULL;
#ifdef SR_SNMPv2_PDU
    UInt64       *temp_c64_ptr = NULL, **temp_c64_pp = NULL;
#endif /* SR_SNMPv2_PDU */

    /* check for bad parameters */
    if (src == NULL || dst == NULL || type_table == NULL ||
            valid_offset < 0 || highest_nominator < 0) {
        DPRINTF((APWARN, "%s: invalid NULL parameter\n", Fname));
        goto SrCopyFamilyContents_fail;
    }
    if (userpart_clone_func != NULL && userpart_free_func == NULL) {
        DPRINTF((APWARN, "%s: if userpart_clone_func is specified, then\n"
                 "userpart_free_func must also be specified\n", Fname));
        goto SrCopyFamilyContents_fail;
    }

    /* make sure the destination family structure is empty */
    SrFreeFamilyContents(dst, type_table, highest_nominator,
                         valid_offset, (SR_FREE_FPTR) userpart_free_func);

    /*
     * Copy the Userpart data, if a function to do so has been
     * provided.  Do this as a first step so a programming error
     * (buffer overflow, etc.) won't corrupt any of the other
     * family data.
     */
    if (userpart_clone_func) {
        userpart_clone_func(dst, src);
    }

    for (nominator = 0; type_table[nominator].type != -1; nominator++) {
        /*
         * Get a pointer to the member of the family structure with
         * the associated value of nominator.
         */
        member_offset = type_table[nominator].byteoffset;
        if (member_offset < 0) {
            if (type_table[nominator].type == OBJECT_ID_TYPE ||
                type_table[nominator].type == OCTET_PRIM_TYPE ||
                type_table[nominator].type == IP_ADDR_PRIM_TYPE ||
                type_table[nominator].type == OPAQUE_PRIM_TYPE ||
                type_table[nominator].type == COUNTER_64_TYPE) {
                DPRINTF((APWARN,
"%s: the byteoffset field in SnmpType array element %d\n\
is -1; the location of some data to be copied cannot be determined\n",
                         Fname, nominator));
                goto SrCopyFamilyContents_cleanup_and_fail;
            }
            continue;
        }
        src_member_ptr = ((char *)src) + member_offset;
        dst_member_ptr = ((char *)dst) + member_offset;

        /*
         * Determine if an allocated structure should be copied
         * and if the valid bit should be set.
         */
        if (nominator > highest_nominator) {
            /*
             * There are more data fields than valid bits.
             * By default, just copy it.
             */
            /* NULL pointer check */
            if (src_member_ptr != NULL) {
                okay_to_copy_member = 1;
            } else {
                okay_to_copy_member = 0;
            }
            okay_to_set_valid_bit = 0;
        } else if (VALID(nominator, src_valid_ptr)) {
            /* the valid bit for this object is set */
            /* NULL pointer check */
            if (src_member_ptr != NULL) {
                okay_to_copy_member = 1;
                okay_to_set_valid_bit = 1;
            } else {
                okay_to_copy_member = 0;
                okay_to_set_valid_bit = 1;  /* to truly copy the structure */
                DPRINTF((APWARN,
"%s: warning: the valid bit is set for nominator %d,\n\
but the corresponding family member is a NULL pointer\n",
                         Fname));
            }
        } else {
            /* the valid bit for this object is NOT set */
            okay_to_copy_member = 0;
            okay_to_set_valid_bit = 0;
            CLR_VALID(nominator, dst_valid_ptr);
        }

        switch (type_table[nominator].type) {
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
            case IP_ADDR_PRIM_TYPE:
                if (okay_to_copy_member) {
                    memcpy((char *)dst_member_ptr,
                           (char *)src_member_ptr, sizeof(SR_UINT32));
                } else {
                    /* initialize */
                    memset((char *)dst_member_ptr, '\0', sizeof(SR_UINT32));
                }
                break;
            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                if (okay_to_copy_member) {
                    memcpy((char *)dst_member_ptr,
                           (char *)src_member_ptr, sizeof(SR_INT32));
                } else {
                    /* initialize */
                    memset((char *)dst_member_ptr, '\0', sizeof(SR_INT32));
                }
                break;
            case OBJECT_ID_TYPE:
                if (okay_to_copy_member) {
                    temp_oid_pp = (OID **) (src_member_ptr);
                    temp_oid_ptr =
                        CloneOID((OID *) (*temp_oid_pp));
                    if (temp_oid_ptr == NULL) {
                        okay_to_set_valid_bit = 0;
                        memset((char *)dst_member_ptr, '\0', sizeof(OID *));
                        DPRINTF((APWARN,"%s: Value is NULL for object with valid bit set\n",Fname));
                    } else {
                         memcpy((char *)dst_member_ptr,
                                (char *)(&temp_oid_ptr), sizeof(OID *));
                    }
                    temp_oid_ptr = (OID *) NULL;
                    temp_oid_pp = (OID **) NULL;
                } else {
                    /* initialize */
                    memset((char *)dst_member_ptr, '\0', sizeof(OID *));
                }
                break;
            case OCTET_PRIM_TYPE:
            case OPAQUE_PRIM_TYPE:
                if (okay_to_copy_member) {
                    temp_os_pp = (OctetString **) (src_member_ptr);
                    temp_os_ptr =
                        CloneOctetString((OctetString *) (*temp_os_pp));
                    if (temp_os_ptr == NULL) {
                        memset((char *)dst_member_ptr,'\0',sizeof(OctetString *));
                        okay_to_set_valid_bit = 0;
                        DPRINTF((APWARN,"%s: Value is NULL for object with valid bit set\n",Fname));
                    } else {
                        memcpy((char *)dst_member_ptr,
                               (char *)(&temp_os_ptr), sizeof(OctetString *));
                    }
                    temp_os_ptr = (OctetString *) NULL;
                    temp_os_pp = (OctetString **) NULL;
                } else {
                    /* initialize */
                    memset((char *)dst_member_ptr, '\0', sizeof(OctetString *));
                }
                break;
#ifdef SR_SNMPv2_PDU
            case COUNTER_64_TYPE:
                if (okay_to_copy_member) {
                    temp_c64_pp = (UInt64 **) (src_member_ptr);
                    temp_c64_ptr =
                        CloneUInt64((UInt64 *) (*temp_c64_pp));
                    if (temp_c64_ptr == NULL) {
                        goto SrCopyFamilyContents_cleanup_and_fail;
                    }
                    memcpy((char *)dst_member_ptr,
                           (char *)(&temp_c64_ptr), sizeof(UInt64 *));
                    temp_c64_ptr = (UInt64 *) NULL;
                    temp_c64_pp = (UInt64 **) NULL;
                } else {
                    /* initialize */
                    memset((char *)dst_member_ptr, '\0', sizeof(UInt64 *));
                }
                break;
#endif /* SR_SNMPv2_PDU */
            case NULL_TYPE:
                /* nothing to copy? */
                break;
            default:
                DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname,
                         type_table[nominator].type));
                goto SrCopyFamilyContents_cleanup_and_fail;
        }
        if (okay_to_set_valid_bit) {
            SET_VALID(nominator, dst_valid_ptr);
        }
    }

    /* success */
    return 1;

    SrCopyFamilyContents_cleanup_and_fail:

    /* free any memory allocated for the destination family structure */
    SrFreeFamilyContents(dst, type_table, highest_nominator,
                         valid_offset, (SR_FREE_FPTR) userpart_free_func);

    SrCopyFamilyContents_fail:

    return -1;
}


/*
 * SrFreeFamilyContents - frees the contents of a <family>_t structure,
 *                        but not the structure itself
 *
 * Parameters:
 *
 *    data - The family structure.
 *
 *    type_type - The SnmpType table for this family as defined in
 *                k_<base>.stb.
 *
 *    highest_nominator - The largest I_ value for this family, as
 *                        defined in <base>supp.h; to disable the
 *                        checking and clearing of valid bits in
 *                        the structure, set parameter to -1.
 *         (NOTES)
 *         The SnmpType structure may be user-modified.  The number of
 *         array elements may be larger than nominator.  To prevent the
 *         VALID macro from reaching outside of the valid array, the
 *         highest nominator must be known.
 *         
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
 *         For example, the usmUserEntryTypeTable has Userpart fields
 *         for auth_secret and priv_secret (note k_usmUserFreeUserpartData
 *         is an empty function).
#else
 *         For example, the usmUserEntryTypeTable has a Userpart field
 *         for auth_secret (note k_usmUserFreeUserpartData is an empty
 *         function).
#endif
#endif
 *
 *    valid_offset - The position of the valid array within the family
 *                   structure (as provided by the offsetof() macro).
 *
 *    userpart_free_func - The k_<family>FreeUserpartData() function
 *                         generated in k_<base>.stb when postmosy is
 *                         run with the -userpart argument.  If no
 *                         function was generated, or if U_<family>
 *                         is not defined, set parameter to NULL.
 */

/* %%% DSS add a debug string parameter? */

void
SrFreeFamilyContents(void *data,
                     const SnmpType *type_table,
                     int highest_nominator,
                     short valid_offset,
                     SR_FREE_FPTR userpart_free_func)
{
    FNAME("SrFreeFamilyContents")
    char *valid_ptr = ((char *)data)+valid_offset;
    int nominator = 0;
    short member_offset = 0;
    void *member_ptr = NULL;
    int okay_to_free_member = 0;
    int okay_to_clear_member = 0;
    int okay_to_clear_valid_bit = 0;
    OctetString **temp_os_pp  = NULL;
    OID         **temp_oid_pp = NULL;
#ifdef SR_SNMPv2_PDU
    UInt64      **temp_c64_pp = NULL;
#endif /* SR_SNMPv2_PDU */

    /* check for bad parameters */
    if (data == NULL || type_table == NULL) {
        return;
    }

    for (nominator = 0; type_table[nominator].type != -1; nominator++) {
        /*
         * Get a pointer to the member of the family structure with
         * the associated value of nominator.
         */
        member_offset = type_table[nominator].byteoffset;
        if (member_offset < 0) {
#ifdef SR_DEBUG
            if (type_table[nominator].type == OBJECT_ID_TYPE ||
                type_table[nominator].type == OCTET_PRIM_TYPE ||
                type_table[nominator].type == IP_ADDR_PRIM_TYPE ||
                type_table[nominator].type == OPAQUE_PRIM_TYPE ||
                type_table[nominator].type == COUNTER_64_TYPE) {
                DPRINTF((APWARN,
"%s: the byteoffset field in SnmpType array element %d\n\
is -1; the location some potentially allocated memory cannot be\n\
determined; this could result in a memory leak\n",
                         Fname, nominator));
            }
#endif /* SR_DEBUG */
            continue;
        }
        member_ptr = ((char *)data) + member_offset;

        /*
         * Determine if an allocated structure should be freed,
         * if the field in the family structure should be cleared,
         * and if the valid bit should be cleared.
         */
        if (valid_offset < 0) {
            /* the location of the valid bits was not specified */
            okay_to_free_member = 1;
            okay_to_clear_member = 1;
            okay_to_clear_valid_bit = 0;
        } else if (highest_nominator < 0) {
            /* the caller doesn't want any valid bits checked */
            okay_to_free_member = 1;
            okay_to_clear_member = 1;
            okay_to_clear_valid_bit = 0;
        } else if (nominator > highest_nominator) {
            /*
             * There are more data fields than valid bits.
             * 
             * The default setting of okay_to_free_member(1) and
             * okay_to_clear_member(1) (below) means that this function
             * frees the pointer types and sets the member value to
             * NULL/zero.  If there is a userpart free function, it
             * must be prepared to handle the case that some of the
             * user data may have been previously freed. 
             *
             * Implication of the default setting: if a user doesn't
             * want the family members to be freed, he should not put
             * them in the SnmpType array.
             */
            okay_to_free_member = 1;
            okay_to_clear_member = 1;
            okay_to_clear_valid_bit = 0;
        } else if (VALID(nominator, valid_ptr)) {
            /* the valid bit for this object is set */
            okay_to_free_member = 1;
            okay_to_clear_member = 1;
            okay_to_clear_valid_bit = 1;
        } else {
            /* the valid bit for this object is NOT set */
            okay_to_free_member = 0;
            okay_to_clear_member = 1;
            okay_to_clear_valid_bit = 0;  /* bit is already clear */
        }
        /* NULL pointer check */
        if (member_ptr == NULL) {
            okay_to_free_member = 0;
        }

        switch (type_table[nominator].type) {
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
            case IP_ADDR_PRIM_TYPE:
                /* nothing to free */
                if (okay_to_clear_member) {
                    memset((char *)member_ptr, '\0', sizeof(SR_UINT32));
                }
                break;
            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                /* nothing to free */
                if (okay_to_clear_member) {
                    memset((char *)member_ptr, '\0', sizeof(SR_INT32));
                }
                break;
            case OBJECT_ID_TYPE:
                if (okay_to_free_member) {
                    temp_oid_pp = (OID **) member_ptr;
                    FreeOID(*temp_oid_pp);
                    temp_oid_pp = NULL;
                }
                if (okay_to_clear_member) {
                    memset((char *)member_ptr, '\0', sizeof(OID *));
                }
                break;
            case OCTET_PRIM_TYPE:
            case OPAQUE_PRIM_TYPE:
                if (okay_to_free_member) {
                    temp_os_pp = (OctetString **) member_ptr;
                    FreeOctetString(*temp_os_pp);
                    temp_os_pp = NULL;
                }
                if (okay_to_clear_member) {
                    memset((char *)member_ptr, '\0', sizeof(OctetString *));
                }
                break;
#ifdef SR_SNMPv2_PDU
            case COUNTER_64_TYPE:
                if (okay_to_free_member) {
                    temp_c64_pp = (UInt64 **) member_ptr;
                    FreeUInt64(*temp_c64_pp);
                    temp_c64_pp = NULL;
                }
                if (okay_to_clear_member) {
                    memset((char *)member_ptr, '\0', sizeof(UInt64 *));
                }
                break;
#endif /* SR_SNMPv2_PDU */
            case NULL_TYPE:
                /* nothing to free */
                /* nothing to clear? */
                break;
            default:
                DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname,
                         type_table[nominator].type));
                break;
        }
        if (okay_to_clear_valid_bit) {
            CLR_VALID(nominator, valid_ptr);
        }
    }

    /*
     * Free the Userpart data, if a function to do so has been provided.
     */
    if (userpart_free_func) {
        userpart_free_func(data);
    }
}



/* %%% DSS add a debug string parameter? */

void *
SrCloneFamily(void *data,
              int family_size,
              const SnmpType *type_table,
              int highest_nominator,
              short valid_offset,
              SR_COPY_FPTR userpart_clone_func,
              SR_FREE_FPTR userpart_free_func)
{
    FNAME("SrCloneFamily")
    void *new_family_t = NULL;
#ifndef SR_TRUST_FAMILY_SIZE_PARAMETER
    int nominator = 0;
    int highest_element = -1;
    short greatest_offset = valid_offset;       /* valid bit array */
    int size = ((highest_nominator / 8) + 1);   /* size of valid bit array */
    int prevsize = 0;
    int clone_size = 0;
#endif /* SR_TRUST_FAMILY_SIZE_PARAMETER */

    /* check for bad parameters */
    if (data == NULL || family_size <= 0 || type_table == NULL ||
            highest_nominator < 0 || valid_offset < 0) {
        goto SrCloneFamily_fail;
    }
#ifndef SR_TRUST_FAMILY_SIZE_PARAMETER
    for (nominator = 0; type_table[nominator].type != -1; nominator++) {
        if (type_table[nominator].byteoffset >= greatest_offset) {
            if (type_table[nominator].byteoffset == greatest_offset) {
                /*
                 * Two byteoffset fields have the same value?
                 * Probably shouldn't happen, but handle it anyway.
                 * Set size by the largest type seen for this offset.
                 */
                prevsize = size;
            }
            greatest_offset = type_table[nominator].byteoffset;
            switch(type_table[nominator].type) {
                case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
                case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
                case TIME_TICKS_TYPE:
                case IP_ADDR_PRIM_TYPE:
                    size = MAX(prevsize, sizeof(SR_UINT32));
                    break;
                case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                    size = MAX(prevsize, sizeof(SR_INT32));
                    break;
                case OBJECT_ID_TYPE:
                    size = MAX(prevsize, sizeof(OID *));
                    break;
                case OCTET_PRIM_TYPE:
                case OPAQUE_PRIM_TYPE:
                    size = MAX(prevsize, sizeof(OctetString *));
                    break;
#ifdef SR_SNMPv2_PDU
                case COUNTER_64_TYPE:
                    size = MAX(prevsize, sizeof(UInt64 *));
                    break;
#endif /* SR_SNMPv2_PDU */
                case NULL_TYPE:
                    size = MAX(prevsize, 0);
                    break;
                default:
                    DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname,
                             type_table[nominator].type));
                    goto SrCloneFamily_fail;
            }
        }
        /* record that we have walked at least one valid SnmpType element */
        highest_element = nominator;
    }
    if (highest_element < 0 || greatest_offset < 0) {
        goto SrCloneFamily_fail;
    }
    clone_size = greatest_offset + size - 1;
    if (family_size < clone_size) {
        DPRINTF((APWARN,
"%s: the clone operation would overflow the family structure!\n\
   family size (parameter) = %d bytes\n\
   clone size (calculated) = %d bytes\n\
Copy aborted.\n",
                 Fname, family_size, clone_size));
        goto SrCloneFamily_fail;
    }
#endif /* SR_TRUST_FAMILY_SIZE_PARAMETER */

    /* allocate memory for the new <family>_t */
    if ((new_family_t = (void *) malloc(family_size)) == NULL) {
        goto SrCloneFamily_fail;
    }
    memset(new_family_t, 0, family_size);
    SrZeroValid(new_family_t, highest_nominator);

    /* copy source <family>_t to dest <family>_t */
    if (SrCopyFamilyContents(new_family_t, data,
            type_table, highest_nominator, valid_offset,
            userpart_clone_func, userpart_free_func) == 1) {
        /* success */
        return new_family_t;
    }

    SrCloneFamily_fail:
    return (void *) NULL;
}


/*
 * SrFreeFamily - a generic routine that replaces all of the functions
 *                generated by postmosy having the following prototype:
 *
 *                void <family>_free(<family>_t *data);
 *
 * Parameters:
 *
 *    Parameter meanings are the same as SrFreeFamilyContents().
 */

/* %%% DSS add a debug string parameter? */

void
SrFreeFamily(void *data,
             const SnmpType *type_table,
             int highest_nominator,
             short valid_offset,
             SR_FREE_FPTR userpart_free_func)
{
    if (data != NULL) {
        SrFreeFamilyContents(data, type_table, highest_nominator,
                             valid_offset, userpart_free_func);
        free ((char *) data);
    }
}


int
ExtractIncomingIndex(
    OID               *incoming,
    int                instLength,
    const SrGetInfoEntry *info_entry,
    int                searchType,
    ObjectInfo        *new_object,
    void              *alloc_data,
    SR_INT32         **ipos_update,            /* instance index positions */
    int               *ipos_size_update,
    int               *carry_return)
{
    FNAME("ExtractIncomingIndex")
    int                carry = 0;
    int                idx_ilen = 0;
    int                s = 0;                  /* table index significance */
    int                j = 0;
    int                ret;
    int                num_indices = 0; 
    OctetString        *os = NULL;
    OID                *oid = NULL;
    char               *alloc_valid_ptr = NULL;
    
    /* check for bad parameters */
    if (incoming == NULL || info_entry == NULL ||
            new_object == NULL || alloc_data == NULL ||
            ipos_update == NULL || ipos_size_update == NULL) {
        DPRINTF((APERROR, "%s: invalid parameter\n", Fname));
        goto ExtractIncomingIndex_error;
    }

    if (searchType == EXACT) {
        carry = 0;
    } else {
        carry = 1;
    }

    /*
     * Walk forward through the list of indices for this table
     * and record (in the ipos array) the position where each
     * index begins in the incoming OID.
     * 
     * %%% DSS what happens if not all of the indices are
     *     present in the incoming OID?
     * 
     */
    idx_ilen = 0;
    for (s = 0; info_entry->index_info[s].nominator != -1; s++) {
        /*
         * For the index having significance 's', record its
         * position within the incoming OID.  Initially, this
         * is 0 (zero bytes past the end of the object portion).
         * On subsequent iterations, idx_ilen tells the
         * number of bytes past the end of the object portion
         * where the previous index ends.
         * 
         * First, however, make sure that ipos is large enough
         * to old the value.
         */
        if (s >= (*ipos_size_update)) {
            if (((*ipos_update) = realloc((*ipos_update), ((s+1)*sizeof(SR_INT32)))) == NULL) {
                goto ExtractIncomingIndex_error;
            } else {
                (*ipos_size_update) = s+1;
            }
        }
        (*ipos_update)[s]=idx_ilen;

        /*
         * Increase idx_ilen by the size of the index having
         * significance 's'.  Integer and IPAddress privimtive
         * types have a known size.  The size of variable length
         * OctetString and OID indices is part of the incoming OID.
         * The length of fixed length OctetStrings is stored in
         * the index_info array.
         */
        if (info_entry->type_table[
                info_entry->index_info[s].nominator
            ].indextype == -1) {
            DPRINTF((APERROR, "%s: indextype is -1\n", Fname));
        }
        switch (info_entry->type_table[
                    info_entry->index_info[s].nominator
                ].type) {
            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
                idx_ilen++;
                break;
            case IP_ADDR_PRIM_TYPE:
                idx_ilen += 4;
                break;
            case OCTET_PRIM_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OCTET_PRIM_VARIABLE:
                        idx_ilen += GetVariableIndexLength(incoming,
                            (int)(new_object->oid.length + idx_ilen));
                        break;
                    case MINV_OCTET_PRIM_IMPLIED:
                        idx_ilen += GetImpliedIndexLength(incoming,
                            (int)(new_object->oid.length + idx_ilen));
                        break;
                    case MINV_OCTET_PRIM_FIXED:
                    case T_networkaddress:
                        idx_ilen += info_entry->index_info[s].size;
                        break;
                    default:
                        /* This should never happen */
                        goto ExtractIncomingIndex_error;
                }
                break;
            case OBJECT_ID_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OID_VARIABLE:
                        idx_ilen += GetVariableIndexLength(incoming,
                            (int)(new_object->oid.length + idx_ilen));
                        break;
                    case MINV_OID_IMPLIED:
                        idx_ilen += GetImpliedIndexLength(incoming,
                            (int)(new_object->oid.length + idx_ilen));
                        break;
                    case MINV_OID_FIXED:
                        idx_ilen += info_entry->index_info[s].size;
                        break;
                    default:
                        /* This should never happen */
                        goto ExtractIncomingIndex_error;
                }
                break;
            default:
                /* This should not happen */
                goto ExtractIncomingIndex_error;
        }
    }

    num_indices = s;

        if ((searchType == EXACT) && (instLength != idx_ilen)) {
            goto ExtractIncomingIndex_no_data;
        }

    /*
     * Walk backwards through the list of indices for this table
     * and convert instancing subidentifiers in the incoming OID
     * to C data types.  These are stored in the <family>_t
     * structure (in alloc_data).
     * 
     * %%% DSS what happens if not all of the indices are
     *     present in the incoming OID?
     * 
     */
    alloc_valid_ptr = ((char *)alloc_data) + info_entry->valid_offset;
    SrZeroValid(alloc_valid_ptr, info_entry->highest_nominator);
    for (s--; s >= 0; s--) {
        switch (info_entry->type_table[
                    info_entry->index_info[s].nominator
                ].type) {
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
                if ( (InstToUInt(incoming,
                        (int)((*ipos_update)[s] + new_object->oid.length),
                        (SR_UINT32 *) ((char *)alloc_data +
                            info_entry->type_table[
                               info_entry->index_info[s].nominator
                            ].byteoffset),
                        searchType, &carry)) < 0) {
                    goto ExtractIncomingIndex_no_data;
                }
                break;
            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                if ( (InstToInt(incoming,
                        (int)((*ipos_update)[s] + new_object->oid.length),
                        (SR_INT32 *) ((char *)alloc_data +
                            info_entry->type_table[
                               info_entry->index_info[s].nominator
                            ].byteoffset),
                        searchType, &carry)) < 0) {
                    goto ExtractIncomingIndex_no_data;
                }
                break;
            case IP_ADDR_PRIM_TYPE:
                ret = InstToIP(incoming,
                              (int)((*ipos_update)[s] + new_object->oid.length),
                              (SR_UINT32 *) ((char *)alloc_data +
                                       info_entry->type_table[
                                            info_entry->index_info[s].nominator
                                       ].byteoffset),
                              searchType, &carry);
                if (ret < 0 ) {
                    goto ExtractIncomingIndex_no_data;
                } else if (ret == 2) {
                    /* IP Addr value contained invalid octet > 255,
                     * must clear out all forward indices
                     */
                    j = num_indices;
                    for (j--; j > s; j--) {
                         /* zero-out values for each of the forward indices */
                         switch(info_entry->type_table[
                                   info_entry->index_info[j].nominator].type) {

                            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
                            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE */
                            case TIME_TICKS_TYPE:
                            case IP_ADDR_PRIM_TYPE:
                                 /* integer based values set to zero */
                                *(SR_UINT32 *)((char*)alloc_data + 
                                          info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                          ].byteoffset) = 0;
                                break;
                            case OCTET_PRIM_TYPE:
                                switch (info_entry->index_info[s].type) {
                                    case MINV_OCTET_PRIM_VARIABLE: 
                                    case MINV_OCTET_PRIM_IMPLIED:
                                        os = (OctetString *)((char*)alloc_data +
                                                info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                                ].byteoffset);
                                        FreeOctetString(os);

                                        os = MakeOctetString(NULL, 0);
                                        *(OctetString **)((char*)alloc_data + 
                                           info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                           ].byteoffset) = os;
                                        break;
                                    case MINV_OCTET_PRIM_FIXED:
                                        os = (OctetString *)((char*)alloc_data +
                                                info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                                ].byteoffset);
                                        FreeOctetString(os);

                                        os = MakeOctetString(NULL, 
                                                info_entry->index_info[j].size);

                                        *(OctetString **)((char*)alloc_data + 
                                           info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                           ].byteoffset) = os;
                                        break;
                                    case T_networkaddress:
                                        os = (OctetString *)((char*)alloc_data +
                                                info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                                ].byteoffset);
                                        FreeOctetString(os);

                                        os = MakeOctetString(NULL, 5);

                                        *(OctetString **)((char*)alloc_data + 
                                           info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                           ].byteoffset) = os;
                                    default:
                                        /* This should never happen */
                                        goto ExtractIncomingIndex_error;
                                }
                                if (os == NULL) {
                                    goto ExtractIncomingIndex_error;
                                }
                                break;
                            case OBJECT_ID_TYPE:
                                switch (info_entry->index_info[s].type) {
                                    case MINV_OID_VARIABLE:
                                    case MINV_OID_IMPLIED:
                                        oid = (OID *)((char*)alloc_data + 
                                                info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                                ].byteoffset);
                                        FreeOID(oid);

                                        oid = MakeOID(NULL, 0);
                                        *(OID **)((char*)alloc_data + 
                                                info_entry->type_table[
                                             info_entry->index_info[j].nominator
                                                ].byteoffset) = oid;
                                        break;
                                    case MINV_OID_FIXED:
                                      /* there is no "FixedOID()" inst support*/
                                    default:
                                        /* This should never happen */
                                        goto ExtractIncomingIndex_error;
                                }
                                break;

                            default:
                                /* This should not happen */
                                goto ExtractIncomingIndex_error;

                        } /* switch (index type) */ 

                    } /* for each (forward) index */

                } /* ret == 2 */

                break;

            case OCTET_PRIM_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OCTET_PRIM_VARIABLE:
                        if ( (InstToVariableOctetString(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OctetString **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    case MINV_OCTET_PRIM_IMPLIED:
                        if ( (InstToImpliedOctetString(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OctetString **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    case MINV_OCTET_PRIM_FIXED:
                        if ( (InstToFixedOctetString(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OctetString **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry, (int)
                                info_entry->index_info[s].size)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    case T_networkaddress:
                        if ( (InstToNetworkAddress(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OctetString **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    default:
                        /* This should never happen */
                        goto ExtractIncomingIndex_error;
                }
                break;
            case OBJECT_ID_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OID_VARIABLE:
                        if ( (InstToVariableOID(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OID **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    case MINV_OID_IMPLIED:
                        if ( (InstToImpliedOID(incoming,
                                (int)((*ipos_update)[s] + new_object->oid.length),
                                (OID **) ((char *)alloc_data +
                                    info_entry->type_table[
                                       info_entry->index_info[s].nominator
                                    ].byteoffset),
                                searchType, &carry)) < 0 ) {
                            goto ExtractIncomingIndex_no_data;
                        }
                        break;
                    case MINV_OID_FIXED:
                        /* there is no "InstToFixedOID()" function */
                    default:
                        /* This should never happen */
                        goto ExtractIncomingIndex_error;
                }
                break;
            case OPAQUE_PRIM_TYPE:
            case COUNTER_64_TYPE:
            case NULL_TYPE:
            default:
                /* This should never happen */
                goto ExtractIncomingIndex_error;
        }
        SET_VALID(info_entry->index_info[s].nominator, alloc_valid_ptr);
    }

    /* return the value of carry, if requested */
    if (carry_return != NULL) {
        *carry_return = carry;
    }

    /* success */
    return 1;

    ExtractIncomingIndex_no_data:
    return 0;

    ExtractIncomingIndex_error:
    return -1;
}


/* %%% DSS add a debug string field to SrGetInfoEntry? */

VarBind *
v_get(
    OID *incoming,
    ObjectInfo *object,
    int searchType,
    ContextInfo *contextInfo,
    int serialNum,
    const SrGetInfoEntry *info_entry)
{
    static int        instLength = 0;
    static OID        inst = { 0, NULL };
    int               carry = 0;
    int               nominator;
    OID               *newIncoming;

#ifndef DISABLE_CACHE
    static int        last_serialNum = 0;
    static OID       *last_incoming = NULL;
    static const SrGetInfoEntry *last_info = NULL;
    char             *cache_valid_ptr = NULL;
#else /* DISABLE_CACHE */
    VarBind          *return_value = NULL;
#endif /* DISABLE_CACHE */

    /* incoming */
    static SR_INT32  *ipos = NULL;            /* instance index positions */
    static int        ipos_size = 0;

    /* outgoing */
    int               family_size = 0;
    SR_KGET_FPTR      k_get = NULL;
    SR_UINT32         ip_addr = 0;
    int               pos = 0;                /* instance index position */
    static void      *data = NULL;            /* <family>_t return value */
    static void      *alloc_data = NULL;      /* <family>_t passed to k_ */
    static int        alloc_data_size = 0;
    int               idx_olen = 0;
    static SR_UINT32 *alloc_oid_ptr = NULL;
    static int        alloc_oid_ptr_size = 0;

    /* incoming and outgoing */
    short             member_offset = 0;
    ObjectInfo       *new_object = NULL;
    int               s = 0;                  /* table index significance */

    /* check parameters */
    if (incoming == NULL || object == NULL ||
            (searchType != EXACT && searchType != NEXT) || 
            info_entry == NULL) {
        /*
         * The v_get() function should not receive a bad parameter.
         * But it is irresponsible to dereference pointers without
         * checking them first, so we do so here.
         *
         * Branching to v_get_error will free the memory pointed to
         * by the static objects ipos, alloc_data, and alloc_oid_ptr.
         * On any subsequent invocations, the memory will be reallocated.
         *
         * If there is a bad parameter, freeing memory is a reasonable
         * course of action, because it likely means that stack has
         * been corrupted by a user thread and the agent will crash
         * soon anyway.
         *
         * This behavior can be exploited to free allocated memory in an
         * embedded system before the SNMP task terminates.  The v_get()
         * function acts as its own "shutdown" routine if the caller
         * intentionally passes in an invalid parameter.
         */
        goto v_get_error;
    }

    newIncoming = CloneOID(incoming);

    /* initialize */
    instLength = incoming->length - object->oid.length;
    new_object = object;
    k_get = info_entry->kfunc;
    family_size = info_entry->family_size;
    member_offset = info_entry->type_table[object->nominator].byteoffset;


#ifndef DISABLE_CACHE
    if((serialNum == last_serialNum) && (serialNum != -1) && 
            (info_entry == last_info) && data && alloc_data &&
            (CmpOIDInst(newIncoming, last_incoming, object->oid.length) == 0)) {
        cache_valid_ptr = ((char *)data) + info_entry->valid_offset;
        if (SrNextRow(cache_valid_ptr, info_entry->highest_nominator) &&  
            (VALID(object->nominator,((char *) cache_valid_ptr)))) {
            goto data_in_cache;
        }
    }
    /*
     * If there was a previous invocation of v_get(), and if the system
     * dependent method routine returned data in alloc_data, free any
     * constructed objects (OctetString, OID, Counter64, or Opaque)
     * in the allocated <family>_t structure.  If there is any
     * Userpart, it will be freed also.
     */
    if (last_info && (data == alloc_data)) {
        SrFreeFamilyContents(alloc_data,
                             last_info->type_table,
                             last_info->highest_nominator,
                             last_info->valid_offset,
                             (SR_FREE_FPTR) last_info->userpart_free_func);
    }
#endif /* DISABLE_CACHE */

    /*
     * void *realloc(void *ptr, size_t size);
     * 
     * The realloc() function changes the size of the block pointed
     * to  by ptr to size  bytes and returns a pointer to the (pos-
     * sibly moved) block. The contents will be unchanged up to the
     * lesser  of the new and old sizes. If  ptr is NULL, realloc()
     * behaves like malloc() for the specified size. If  size  is 0
     * and  ptr  is  not  a  null pointer, the object pointed to is
     * freed.
     */

    /* make sure allocated storage is large enough for <family>_t */
    if (family_size > alloc_data_size) {
        if ((alloc_data = realloc(alloc_data, family_size)) == NULL) {
            goto v_get_error;
        } else {
            alloc_data_size = family_size;
            memset((char *) alloc_data, '\0', alloc_data_size);
        }
    }

    if (info_entry->index_info == NULL) {
        if (CheckScalarInstance(newIncoming, new_object, searchType) < 0) {
            goto v_get_no_data;
        }
        if ((data = k_get(serialNum, contextInfo, new_object->nominator,
                          searchType, alloc_data)) == NULL) {
            goto v_get_no_data;
        }

#ifndef DISABLE_CACHE
        /* the cache has been updated, reset cache_valid_ptr */
        cache_valid_ptr = ((char *)data) + info_entry->valid_offset;
#endif /* DISABLE_CACHE */


        /* build outgoing instance information */
        inst.oid_ptr=ZeroOid.oid_ptr;
        inst.length=ZeroOid.length;
    } else {


        switch(ExtractIncomingIndex(newIncoming,
                                 instLength,
                                 info_entry,
                                 searchType,
                                 new_object,
                                 alloc_data,
                                 &ipos,
                                 &ipos_size,
                                 &carry)) {
            case 0:
                goto v_get_no_data;
            case -1:
                goto v_get_error;
        }


        /* if the carry is still set, the end of the table has been reached */
        if (carry) {
            SrFreeFamilyContents(alloc_data,
                                 info_entry->type_table,
                                 info_entry->highest_nominator,
                                 info_entry->valid_offset,
                                 info_entry->userpart_free_func);
            goto v_get_no_data;
        }

        /*
         * Retrieve the data from the kernel-specific routine.
         */
        nominator = new_object->nominator;
        if ((data = k_get(serialNum, contextInfo, nominator,
                          searchType, alloc_data)) == NULL) {
            SrFreeFamilyContents(alloc_data,
                                 info_entry->type_table,
                                 info_entry->highest_nominator,
                                 info_entry->valid_offset,
                                 info_entry->userpart_free_func);
            goto v_get_no_data;
        }

#ifndef DISABLE_CACHE
        /* the cache has been updated, reset cache_valid_ptr */
        cache_valid_ptr = ((char *)data) + info_entry->valid_offset;
#endif /* DISABLE_CACHE */

        if (data != alloc_data) {
            /*
             * The k_ routine returned its own <family>_t structure
             * and did not use the allocated one we provided to it.
             * Free the index fields in alloc_data.
             */
            SrFreeFamilyContents(alloc_data,
                                 info_entry->type_table,
                                 info_entry->highest_nominator,
                                 info_entry->valid_offset,
                                 info_entry->userpart_free_func);
#ifndef DISABLE_CACHE
            /*
             * For some applications, it might be reasonable here to:
             *   1. copy the contents of data into alloc_data
             *   2. make the data pointer point to alloc_data
             *   3. update cache_valid_ptr to point into alloc_data
             */
#endif /* DISABLE_CACHE */
        }



        /*
         * Build outgoing instance information.
         */
        inst.oid_ptr = alloc_oid_ptr;
        inst.length = 0;

        /* XXX
           should be able to replace the block of code below with this
           function call, but it has not been well tested yet:
           ExtractIndexFromData(info_entry, data)
        */
        pos = 0;
        for (s = 0; info_entry->index_info[s].nominator != -1; s++) {
            int j;
            OctetString **ostr;
            OID **oid;

            switch (info_entry->type_table[
                        info_entry->index_info[s].nominator
                    ].type) {
                case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
                case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
                case TIME_TICKS_TYPE:
                    idx_olen++;
                    if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                        if ((inst.oid_ptr = alloc_oid_ptr =
                            realloc(alloc_oid_ptr,
                                alloc_oid_ptr_size = 
                                (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                            goto v_get_error;
                        }
                    }
                    inst.oid_ptr[pos++] = *((SR_UINT32 *) ((char *)data +
                        info_entry->type_table[
                           info_entry->index_info[s].nominator
                        ].byteoffset));
                    inst.length = idx_olen;
                    break;

                case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                    idx_olen++;
                    if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                        if ((inst.oid_ptr = alloc_oid_ptr =
                            realloc(alloc_oid_ptr,
                                alloc_oid_ptr_size = 
                                (idx_olen * sizeof(SR_INT32)))) == NULL) {
                            goto v_get_error;
                        }
                    }
                    inst.oid_ptr[pos++] = *((SR_INT32 *) ((char *)data +
                        info_entry->type_table[
                           info_entry->index_info[s].nominator
                        ].byteoffset));
                    inst.length = idx_olen;
                    break;
                case IP_ADDR_PRIM_TYPE:
                    idx_olen += 4;
                    if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                        if ((inst.oid_ptr = alloc_oid_ptr =
                            realloc(alloc_oid_ptr,
                                alloc_oid_ptr_size = 
                                (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                            goto v_get_error;
                        }
                    }
                    ip_addr = *((SR_UINT32 *) ((char *)data +
                        info_entry->type_table[
                           info_entry->index_info[s].nominator
                        ].byteoffset));
                    inst.oid_ptr[pos++] = (ip_addr >> 24) & 0xff;
                    inst.oid_ptr[pos++] = (ip_addr >> 16) & 0xff;
                    inst.oid_ptr[pos++] = (ip_addr >> 8) & 0xff;
                    inst.oid_ptr[pos++] = (ip_addr >> 0) & 0xff;
                    inst.length = idx_olen;
                    break;
                case OCTET_PRIM_TYPE:
                    switch (info_entry->index_info[s].type) {
                        case MINV_OCTET_PRIM_IMPLIED:
                        case MINV_OCTET_PRIM_VARIABLE:
                        case MINV_OCTET_PRIM_FIXED:
                        case T_networkaddress:
                            ostr = (OctetString **)((char *)data +
                                info_entry->type_table[
                                   info_entry->index_info[s].nominator
                                ].byteoffset);
                            if (ostr == NULL) {
                                goto v_get_error;
                            }
                            idx_olen += (*ostr)->length;
                            if (info_entry->index_info[s].type ==
                                    MINV_OCTET_PRIM_VARIABLE) {
                                idx_olen++;
                            }
                            if (info_entry->index_info[s].type ==
                                    T_networkaddress) {
                                idx_olen++;
                            }
                            if (idx_olen > 
                                (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {

                                if ((inst.oid_ptr = alloc_oid_ptr =
                                    realloc(alloc_oid_ptr,
                                        alloc_oid_ptr_size = 
                                        (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                                    goto v_get_error;
                                }
                            }
                            if (info_entry->index_info[s].type ==
                                    MINV_OCTET_PRIM_VARIABLE) {
                                inst.oid_ptr[pos++] = (*ostr)->length;
                            }
                            if (info_entry->index_info[s].type ==
                                    T_networkaddress) {
                                inst.oid_ptr[pos++] = 1;
                            }
                            for (j=0; j < (*ostr)->length; j++) {
                                inst.oid_ptr[pos++] = ((*ostr)->octet_ptr)[j];
                            }
                            inst.length = idx_olen;
                            break;
                        default:
                            /* This should never happen */
                            goto v_get_error;
                    }
                    break;
                case OBJECT_ID_TYPE:
                    switch (info_entry->index_info[s].type) {
                        case MINV_OID_VARIABLE:
                        case MINV_OID_IMPLIED:
                            oid = (OID **)((char *)data +
                                info_entry->type_table[
                                   info_entry->index_info[s].nominator
                                ].byteoffset);
                            if (oid == NULL) {
                                goto v_get_error;
                            }
                            idx_olen += (*oid)->length;
                            if (info_entry->index_info[s].type ==
                                    MINV_OID_VARIABLE) {
                                idx_olen++;
                            }
                            if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                                if ((inst.oid_ptr = alloc_oid_ptr =
                                    realloc(alloc_oid_ptr,
                                        alloc_oid_ptr_size = 
                                        (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                                    goto v_get_error;
                                }
                            }
                            if (info_entry->index_info[s].type ==
                                    MINV_OID_VARIABLE) {
                                inst.oid_ptr[pos++] = (*oid)->length;
                            }
                            for (j=0; j < (*oid)->length; j++) {
                                inst.oid_ptr[pos++] = ((*oid)->oid_ptr)[j];
                            }
                            inst.length = idx_olen;
                            break;
                        default:
                            /* This should never happen */
                            goto v_get_error;
                    }
                    break;
                case OPAQUE_PRIM_TYPE:
                case COUNTER_64_TYPE:
                case NULL_TYPE:
                default:
                    /* This should never happen */
                    goto v_get_error;
            }
        }
    }

    /* successful data retrieval */
#ifndef DISABLE_CACHE
    last_serialNum = serialNum;
    last_info = info_entry;
    if (last_incoming != NULL) {
       FreeOID(last_incoming);
    }
    last_incoming = CloneOID(newIncoming);

    data_in_cache:

    if (newIncoming != NULL) {
        FreeOID(newIncoming);
    }


    return (SrBuildReturnVarBind(member_offset, data,
                 (unsigned char)info_entry->type_table[object->nominator].type,
                 new_object, &inst));

#else /* DISABLE_CACHE */
    return_value =
        SrBuildReturnVarBind(member_offset, data,
                             info_entry->type_table[object->nominator].type,
                             new_object, &inst);
    /*
     * Free any constructed objects (OctetString, OID, Counter64,
     * or Opaque) in the allocated <family>_t structure.  If there
     * is any Userpart, it will be freed also.
     */

    if (data == alloc_data) {
        SrFreeFamilyContents(alloc_data,
                             info_entry->type_table,
                             info_entry->highest_nominator,
                             info_entry->valid_offset,
                             info_entry->userpart_free_func);
    }

    if (newIncoming != NULL {
        FreeOID(newIncoming);
    }
 
    return (return_value);
#endif /* DISABLE_CACHE */

    /*
     * v_get_no_data - return "no data" for a condition that is not
     *                 an error; e.g., end of table has been reached.
     */
    v_get_no_data:
    if (newIncoming != NULL) {
        FreeOID(newIncoming);
    }
#ifndef DISABLE_CACHE
    last_info = NULL;
#endif /* DISABLE_CACHE */
    return (VarBind *) NULL;

    /*
     * v_get_error - a serious error occurred
     *
     *    This could be caused by a run-time problem, such as a
     *    malloc() failure.
     *
     *    This could also be the result of a programming error,
     *    such an an incorrectly constructed ObjectInfo list.
     *
     *    Note: the caller of v_get() may have intentionally passed
     *    in a NULL parameter to exploit the function's behavior of
     *    freeing memory pointed to by static variables.
     */
    v_get_error:
    if (newIncoming != NULL) {
        FreeOID(newIncoming);
    }
#ifndef DISABLE_CACHE
    last_info = NULL;
#endif /* DISABLE_CACHE */
    if (ipos != NULL) {
        free(ipos);
        ipos = NULL;
    }
    ipos_size = 0;
    if (alloc_data != NULL) {
        free(alloc_data);
        alloc_data = NULL;
    }
    alloc_data_size = 0;
    if (alloc_oid_ptr != NULL) {
        free(alloc_oid_ptr);
        alloc_oid_ptr = NULL;
    }
    alloc_oid_ptr_size = 0;
    return (VarBind *) NULL;
}


OID *
ExtractIndexFromData(const SrGetInfoEntry *info_entry, void *data)
{
    int s;
    int idx_olen = 0;
    int pos = 0;                /* instance index position */
    SR_UINT32 ip_addr = 0;
    static SR_UINT32 *alloc_oid_ptr = NULL;
    static int alloc_oid_ptr_size = 0;
    static OID inst = { 0, NULL };

    for (s = 0; info_entry->index_info[s].nominator != -1; s++) {
        int j;
        OctetString **ostr;
        OID **oid;

        switch (info_entry->type_table[
                    info_entry->index_info[s].nominator
                ].type) {
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
                idx_olen++;
                if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                    if ((inst.oid_ptr = alloc_oid_ptr =
                        realloc(alloc_oid_ptr,
                            alloc_oid_ptr_size = 
                            (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                        goto fail;
                    }
                }
                inst.oid_ptr[pos++] = *((SR_UINT32 *) ((char *)data +
                    info_entry->type_table[
                       info_entry->index_info[s].nominator
                    ].byteoffset));
                inst.length = idx_olen;
                break;

            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                idx_olen++;
                if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                    if ((inst.oid_ptr = alloc_oid_ptr =
                        realloc(alloc_oid_ptr,
                            alloc_oid_ptr_size = 
                            (idx_olen * sizeof(SR_INT32)))) == NULL) {
                        goto fail;
                    }
                }
                inst.oid_ptr[pos++] = *((SR_INT32 *) ((char *)data +
                    info_entry->type_table[
                       info_entry->index_info[s].nominator
                    ].byteoffset));
                inst.length = idx_olen;
                break;
            case IP_ADDR_PRIM_TYPE:
                idx_olen += 4;
                if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                    if ((inst.oid_ptr = alloc_oid_ptr =
                        realloc(alloc_oid_ptr,
                            alloc_oid_ptr_size = 
                            (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                        goto fail;
                    }
                }
                ip_addr = *((SR_UINT32 *) ((char *)data +
                    info_entry->type_table[
                       info_entry->index_info[s].nominator
                    ].byteoffset));
                inst.oid_ptr[pos++] = (ip_addr >> 24) & 0xff;
                inst.oid_ptr[pos++] = (ip_addr >> 16) & 0xff;
                inst.oid_ptr[pos++] = (ip_addr >> 8) & 0xff;
                inst.oid_ptr[pos++] = (ip_addr >> 0) & 0xff;
                inst.length = idx_olen;
                break;
            case OCTET_PRIM_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OCTET_PRIM_IMPLIED:
                    case MINV_OCTET_PRIM_VARIABLE:
                    case MINV_OCTET_PRIM_FIXED:
                    case T_networkaddress:
                        ostr = (OctetString **)((char *)data +
                            info_entry->type_table[
                               info_entry->index_info[s].nominator
                            ].byteoffset);
                        if (ostr == NULL) {
                            goto fail;
                        }
                        idx_olen += (*ostr)->length;
                        if (info_entry->index_info[s].type ==
                                MINV_OCTET_PRIM_VARIABLE) {
                            idx_olen++;
                        }
                        if (info_entry->index_info[s].type ==
                                T_networkaddress) {
                            idx_olen++;
                        }
                        if (idx_olen > 
                            (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {

                            if ((inst.oid_ptr = alloc_oid_ptr =
                                realloc(alloc_oid_ptr,
                                    alloc_oid_ptr_size = 
                                    (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                                goto fail;
                            }
                        }
                        if (info_entry->index_info[s].type ==
                                MINV_OCTET_PRIM_VARIABLE) {
                            inst.oid_ptr[pos++] = (*ostr)->length;
                        }
                        if (info_entry->index_info[s].type ==
                                T_networkaddress) {
                            inst.oid_ptr[pos++] = 1;
                        }
                        for (j=0; j < (*ostr)->length; j++) {
                            inst.oid_ptr[pos++] = ((*ostr)->octet_ptr)[j];
                        }
                        inst.length = idx_olen;
                        break;
                    default:
                        /* This should never happen */
                        goto fail;
                }
                break;
            case OBJECT_ID_TYPE:
                switch (info_entry->index_info[s].type) {
                    case MINV_OID_VARIABLE:
                    case MINV_OID_IMPLIED:
                        oid = (OID **)((char *)data +
                            info_entry->type_table[
                               info_entry->index_info[s].nominator
                            ].byteoffset);
                        if (oid == NULL) {
                            goto fail;
                        }
                        idx_olen += (*oid)->length;
                        if (info_entry->index_info[s].type ==
                                MINV_OID_VARIABLE) {
                            idx_olen++;
                        }
                        if (idx_olen > (alloc_oid_ptr_size/(sizeof(SR_UINT32)))) {
                            if ((inst.oid_ptr = alloc_oid_ptr =
                                realloc(alloc_oid_ptr,
                                    alloc_oid_ptr_size = 
                                    (idx_olen * sizeof(SR_UINT32)))) == NULL) {
                                goto fail;
                            }
                        }
                        if (info_entry->index_info[s].type ==
                                MINV_OID_VARIABLE) {
                            inst.oid_ptr[pos++] = (*oid)->length;
                        }
                        for (j=0; j < (*oid)->length; j++) {
                            inst.oid_ptr[pos++] = ((*oid)->oid_ptr)[j];
                        }
                        inst.length = idx_olen;
                        break;
                    default:
                        /* This should never happen */
                        goto fail;
                }
                break;
            case OPAQUE_PRIM_TYPE:
            case COUNTER_64_TYPE:
            case NULL_TYPE:
            default:
                /* This should never happen */
                goto fail;
        }
    }
    return CloneOID(&inst);
fail:
    return NULL;
}
