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

#include <malloc.h>

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
#include "tmq.h"
#include "rowstatf.h"
#include "oid_lib.h"
#include "prnt_lib.h"
#include "scan.h"
#include "min_v.h"
#include "diag.h"
SR_FILENAME


extern int GetEmanateVersion(void);
extern int per_varbind_ready;

#define ADAPT_APO_SETS_TO_ROWSTATUS

/*
 * FillInCurrentValues(): copy the current values of this family to
 * dp->data and dp->undodata.
 */
int
FillInCurrentValues(doList_t *dp, void *data, const SrTestInfoEntry *testinfo)
{
    if (data == NULL) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    /* fill in existing values */
    dp->data = SrCloneFamily(data, 
                             testinfo->getinfo->family_size,
                             testinfo->getinfo->type_table,
                             testinfo->getinfo->highest_nominator,
                             testinfo->getinfo->valid_offset,
                             testinfo->userpart_clone_func,
                             testinfo->getinfo->userpart_free_func);
    if (dp->data == NULL) {
        DPRINTF((APWARN, 
              "SrCloneFamily Failed, possible memory allocation failure\n"));
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    dp->undodata = SrCloneFamily(data, 
                                 testinfo->getinfo->family_size,
                                 testinfo->getinfo->type_table,
                                 testinfo->getinfo->highest_nominator,
                                 testinfo->getinfo->valid_offset,
                                 testinfo->userpart_clone_func,
                                 testinfo->getinfo->userpart_free_func);
    if (dp->undodata == NULL) {
        DPRINTF((APWARN, 
              "SrCloneFamily Failed, possible memory allocation failure\n"));
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    return NO_ERROR;
}

/* 
 * TestAndCopyObject()
 *
 * test the object against constraints in the MIB, call the k_ test and
 * ready routines, copy the value into the dp->data structure, and set
 * the valid bits.
 */
int
TestAndCopyObject(ObjectInfo *object, ObjectSyntax *value, doList_t *doHead, 
                  ContextInfo *contextInfo, doList_t *dp, 
                  const SrTestInfoEntry *testinfo)
{
    FNAME("TestAndCopyObject")
    unsigned short offset = 
           testinfo->getinfo->type_table[object->nominator].byteoffset;
    unsigned short valid_offset = testinfo->getinfo->valid_offset;
    /* LVL7 */
    unsigned short mod_offset = valid_offset + (testinfo->getinfo->highest_nominator/8+1);
    /* LVL7 end */
    void *member_ptr = (void *) ((char *)(dp->data) + offset);
    void *undo_member_ptr;
    char *undo_valid_ptr;
    void **tmp, *ptr;
    SR_INT32 *tmp_int;
    SR_UINT32 *tmp_uint;
    SR_UINT32 ipaddr;
    int error_status = NO_ERROR;
    struct sr_member_test *member_test;
    ObjectSyntax *oldvalue, tmpvalue;
    SR_INT32 int32_max, int32_min;
    SR_UINT32 uint32_max, uint32_min;
    int *pnum_bits, num_octets, mask;
    char *valid, *mod; /* LVL7 */
    int ok;
    int i;
    OctetString *tmp_octet = NULL;
 
    if (dp->data == NULL) {
        /* this should never happen */
        DPRINTF((APERROR, "%s: Internal Error\n", Fname));
        return GEN_ERROR;
    }

    /*
     * test the type and status. note that these tests are already
     * done in the master agent for non-aggregate types
     */
    if ((object->oidstatus & SR_ACCESS_MASK) != SR_READ_WRITE
        && (object->oidstatus & SR_ACCESS_MASK) != SR_OLD_READ_CREATE
        && (object->oidstatus & SR_ACCESS_MASK) != SR_READ_CREATE) {
        return NOT_WRITABLE_ERROR;
    }
    if (value->type != object->oidtype) {
        return WRONG_TYPE_ERROR;
    }

    if (object->oidtype != 
                    testinfo->getinfo->type_table[object->nominator].type) {
        DPRINTF((APERROR, "%s: Internal Error\n", Fname));
        return GEN_ERROR;
    }

    if (dp->undodata) {
        undo_member_ptr = (void *) ((char *)(dp->undodata) + offset);
        undo_valid_ptr = ((char *)dp->undodata)+valid_offset;
    } else {
        undo_member_ptr = NULL;
        undo_valid_ptr = NULL;
    }

    member_test = &(testinfo->member_test[object->nominator]);

    switch (object->oidtype) {
        case INTEGER_TYPE:  /* includes INTEGER_32_TYPE */

            if (member_test->test_kind == MINV_INTEGER_RANGE_TEST) {
                ok = 0;
                for (i = 0; i < member_test->test_data_size; i++) {
                    int32_min = 
                     ((RangeTest_t *)(member_test->test_data))[i].minimum;
                    int32_max = 
                     ((RangeTest_t *)(member_test->test_data))[i].maximum;
                    if ( (value->sl_value >= int32_min) &&
                         (value->sl_value <=  int32_max) ) {
                        ok = 1;
                    }
                }
                if (!ok) {
                   return WRONG_VALUE_ERROR;
                }
            }
            if (member_test->test_kind == MINV_INTEGER_ENUM_TEST) {
                int ok = 0;
                for (i = 0; i < member_test->test_data_size; i++) {
                    if (((EnumIntTest_t *) member_test->test_data)[i] == 
                                           value->sl_value) {
                        ok = 1;
                        break;
                    }
                }
                if (!ok) {
                    return WRONG_VALUE_ERROR;
                }
            }
            memcpy ((char *) member_ptr, (char *) &(value->sl_value), 
                    sizeof(SR_INT32));
            break;

        case UNSIGNED32_TYPE:   /* include GAUGE_TYPE and GAUGE_32_TYPE */
        case COUNTER_32_TYPE:   /* includes COUNTER_TYPE */
        case TIME_TICKS_TYPE:
            if (member_test->test_kind == MINV_UINTEGER_RANGE_TEST) {
                ok = 0;
                for (i = 0; i < member_test->test_data_size; i++) {
                    uint32_min = 
                     ((URangeTest_t *)(member_test->test_data))[i].minimum;
                    uint32_max = 
                     ((URangeTest_t *)(member_test->test_data))[i].maximum;
                    if ( (value->sl_value >= uint32_min) &&
                         (value->sl_value <= uint32_max) ) {
                        ok = 1;
                        break;
                    }
                }
                if (!ok) {
                    return WRONG_VALUE_ERROR;
                }
            }

            memcpy ((char *) member_ptr, (char *) &(value->ul_value), 
                    sizeof(SR_UINT32));
            break;

        case IP_ADDR_PRIM_TYPE:
            if (value->os_value->length != 4) {
                return WRONG_LENGTH_ERROR;
            }
            ipaddr = OctetStringToIP(value->os_value);
            memcpy (member_ptr, &ipaddr, sizeof (SR_UINT32));
            break;

        case OCTET_PRIM_TYPE:   /* includes BITS_TYPE */
        case OPAQUE_PRIM_TYPE:
            if (member_test->test_kind == MINV_LENGTH_TEST) {
                ok = 0;
                for (i = 0; i < member_test->test_data_size; i++) {
                    int32_min = 
                     ((LengthTest_t *)(member_test->test_data))[i].minimum;
                    int32_max = 
                     ((LengthTest_t *)(member_test->test_data))[i].maximum;

                    if ( (value->os_value->length >= int32_min) &&
                          (value->os_value->length <= int32_max) ) {
                        ok = 1;
                        break;
                    }
                }
                if (!ok) {
                    return WRONG_LENGTH_ERROR;
                }
            }
            if (member_test->test_kind == MINV_BITS_TEST) {
                pnum_bits = (BitsTest_t *) (member_test->test_data);
                num_octets = (*pnum_bits / 8) + 1;
                if (value->os_value->length != num_octets) {
                    return WRONG_LENGTH_ERROR;
                }
                /* zero out unused bits */
                mask = (0xff << (7 - (*pnum_bits % 8))) & 0xff;
                value->os_value->octet_ptr[value->os_value->length-1] &= 
                                                               mask;
            }

            tmp = (void **) member_ptr;
            if (*tmp != NULL) {
                FreeOctetString((OctetString *) *tmp);
            }
            ptr = (void *) CloneOctetString(value->os_value);
            memcpy (member_ptr, &ptr, sizeof (void *));
            break;

        case OBJECT_ID_TYPE:
            if (member_test->test_kind == MINV_LENGTH_TEST) {
                ok = 0;
                for (i = 0; i < member_test->test_data_size; i++) {
                    int32_min = 
                      ((LengthTest_t *)(member_test->test_data))[i].minimum;
                    int32_max = 
                      ((LengthTest_t *)(member_test->test_data))[i].maximum;

                    if ( (value->oid_value->length >= int32_min) &&
                          (value->oid_value->length <= int32_max) ) {
                        ok = 1;
                    }
                }
                if (!ok) {
                    return WRONG_LENGTH_ERROR;
                }
            }

            tmp = (void **) member_ptr;
            if (*tmp != NULL) {
                FreeOID(*tmp);
            }
            ptr = CloneOID(value->oid_value);
            memcpy (member_ptr, &ptr, sizeof (void *));
            break;

#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
            tmp = (void **) member_ptr;
            if (*tmp != NULL) {
                FreeCounter64(*tmp);
            }
            ptr = CloneUInt64(value->uint64_value);
            memcpy (member_ptr, &ptr, sizeof (void *));
            break;
#endif /* SR_SNMPv2_PDU */

        default:
            /* should never happen */
            DPRINTF((APWARN, "%s: ERROR, unknown type\n", Fname));
            return GEN_ERROR;
    }

    /*
     * if testfunc is not NULL, call the test function specified by
     * member_test->testfunc
     */
    if (member_test->testfunc != NULL) {
        /*
         * first, create an ObjectSyntax structure with the old value, 
         * which is one of the parameters to member_test->testfunc
         */
        tmpvalue.type = object->oidtype;
        if (undo_member_ptr && VALID(object->nominator, undo_valid_ptr)) {
            switch (object->oidtype) {
                case INTEGER_TYPE:  /* includes INTEGER_32_TYPE */
                    tmp_int = (SR_INT32 *) undo_member_ptr;
                    tmpvalue.sl_value = (SR_INT32) *tmp_int;
                    oldvalue = &tmpvalue;
                    break;
                case UNSIGNED32_TYPE:   /* include GAUGE_TYPE, GAUGE_32_TYPE */
                case COUNTER_32_TYPE:   /* includes COUNTER_TYPE */
                case TIME_TICKS_TYPE:
                    tmp_uint = (SR_UINT32 *) undo_member_ptr;
                    tmpvalue.ul_value = (SR_UINT32) *tmp_uint;
                    oldvalue = &tmpvalue;
                    break;
                case IP_ADDR_PRIM_TYPE:
                    tmp_uint = (SR_UINT32 *) undo_member_ptr;
                    tmp_octet = IPToOctetString((SR_UINT32) *tmp_uint);
                    tmpvalue.os_value = tmp_octet;
                    oldvalue = &tmpvalue;
                    break;
                case OCTET_PRIM_TYPE:   /* includes BITS_TYPE */
                case OPAQUE_PRIM_TYPE:
                    tmp = (void **) undo_member_ptr;
                    tmpvalue.os_value = (OctetString *) *tmp;
                    oldvalue = &tmpvalue;
                    break;
                case OBJECT_ID_TYPE:
                    tmp = (void **) undo_member_ptr;
                    tmpvalue.oid_value = (OID *) *tmp;
                    oldvalue = &tmpvalue;
                    break;
                case COUNTER_64_TYPE:
                    tmp = (void **) undo_member_ptr;
                    tmpvalue.uint64_value = (UInt64 *) *tmp;
                    oldvalue = &tmpvalue;
                    break;
                default:
                    /* should never happen */
                    DPRINTF((APWARN, "%s: ERROR, unknown type\n", Fname));
                    return GEN_ERROR;
           }
       } else {
           oldvalue = NULL;
       }
       /*
        * now call the test function 
        */
       error_status = member_test->testfunc(value, oldvalue);
       if (tmp_octet != NULL) {
           FreeOctetString(tmp_octet);
           tmp_octet = NULL;
       }
    }

    /*
     * If the system independent testing failed, return the appropriate
     * error code.
     */
    if (error_status != NO_ERROR) {
        return error_status;
    }

    /* 
     * Do system dependent testing in the k_ test function 
     */
    error_status = testinfo->ktestfunc(object, value, dp, contextInfo);

    /* set the valid bits */
    valid = (void *) ((char *)(dp->data) + valid_offset);
    SET_VALID(object->nominator, valid);

    /* LVL7: Turn on mod bit for set code */
    mod = (void *) ((char *)(dp->data) + mod_offset);
    SET_VALID(object->nominator, mod);
    /* LVL7 end */

    return error_status;
}

/*
 * SrCleanup(): free data and undodata items in the DoList.
 */
int
SrCleanup(doList_t *trash, const SrTestInfoEntry *testinfo)
{
   SrFreeFamily(trash->data, 
                testinfo->getinfo->type_table,
                -1, /* -1 means don't check valid bits */ 
                testinfo->getinfo->valid_offset,
                testinfo->getinfo->userpart_free_func);
 
   SrFreeFamily(trash->undodata, 
                testinfo->getinfo->type_table,
                -1, /* -1 means don't check valid bits */ 
                testinfo->getinfo->valid_offset,
                testinfo->getinfo->userpart_free_func);
   return NO_ERROR;
}

/*
 * CompareTableIndices(): Compare the table index values in the
 * <family>_t structures pointed to by data1 and data2.
 * return 0 if they are the same, non-zero otherwise.
 */
int
CompareTableIndices (void *data1, void *data2, const SrTestInfoEntry *testinfo)
{
    FNAME("CompareTableIndices")
    int i;
    int cmp = 0;
    void *member_ptr1, *member_ptr2;
    short member_offset;
    void **tmp1, **tmp2;
    int *int1, *int2;
    short nominator;
    const SrIndexInfo *index_info = testinfo->getinfo->index_info; 

    if (testinfo->getinfo->index_info == NULL) {
        /*
         * If index_info is NULL, it's a scalar. There are no indexes
         * to compare, so return success.
         */
        return 0;
    }

    for (i = 0; index_info[i].nominator != -1 && cmp == 0; i++) {
        nominator = index_info[i].nominator;
        member_offset = testinfo->getinfo->type_table[nominator].byteoffset;
        if (member_offset == -1) {
            DPRINTF((APWARN, "%s: invalid offset\n", Fname));
            return -1;
        }
        member_ptr1 = ((char *)data1) + member_offset;
        member_ptr2 = ((char *)data2) + member_offset;

        switch (index_info[i].type) {
            case T_uint:
                int1 = member_ptr1;
                int2 = member_ptr2;
                cmp = ((*int1)<(*int2)?-1:((*int1)>(*int2)));
                break;
            case T_octet:
            case T_fixed_octet:
                tmp1 = (void **) member_ptr1;
                tmp2 = (void **) member_ptr2;
                cmp = CmpOctetStrings(*tmp1, *tmp2);
                break;
            case T_oid:
            case T_fixed_oid:
                tmp1 = (void **) member_ptr1;
                tmp2 = (void **) member_ptr2;
                cmp = CmpOID(*tmp1, *tmp2);
                break;
            case T_var_octet:
                tmp1 = (void **) member_ptr1;
                tmp2 = (void **) member_ptr2;
                cmp = CmpOctetStringsWithLen(*tmp1, *tmp2);
                break;
            case T_var_oid:
                tmp1 = (void **) member_ptr1;
                tmp2 = (void **) member_ptr2;
                cmp = CmpOIDWithLen(*tmp1, *tmp2);
                break;
            default:
                DPRINTF((APWARN, "%s: invalid type\n", Fname));
                return -1;
        }
    }
    return cmp;
}

/*
 * CopyIndexFields(): Copy in index fields from the <family>_t structure
 * src to the <family>_t structure dst. Return 0 on success, non-zero on
 * failure.
 */
int
CopyIndexFields (void *dst, void *src, const SrTestInfoEntry *testinfo)
{
    FNAME("CopyIndexFields")
    int i;
    void *src_member_ptr, *dst_member_ptr;
    short member_offset;
    short nominator;
    OctetString  *temp_os_ptr, **temp_os_pp;
    OID *temp_oid_ptr, **temp_oid_pp;

    const SrIndexInfo *index_info = testinfo->getinfo->index_info;
    const SnmpType *type_table = testinfo->getinfo->type_table;
    char *dst_valid_ptr = ((char *)dst) + testinfo->getinfo->valid_offset;

    for (i = 0; index_info[i].nominator != -1; i++) {
        nominator = index_info[i].nominator;
        member_offset = testinfo->getinfo->type_table[nominator].byteoffset;
        if (member_offset == -1) {
            DPRINTF((APWARN, "%s: invalid offset\n", Fname));
            return -1;
        }
        src_member_ptr = ((char *)src) + member_offset;
        dst_member_ptr = ((char *)dst) + member_offset;

        switch (type_table[nominator].type) {
            case COUNTER_32_TYPE: /* includes COUNTER_TYPE */
            case UNSIGNED32_TYPE: /* includes GAUGE_TYPE, GAUGE_32_TYPE */
            case TIME_TICKS_TYPE:
                memcpy((char *)dst_member_ptr,
                       (char *)src_member_ptr, sizeof(SR_UINT32));
                break;
            case INTEGER_32_TYPE: /* includes INTEGER_TYPE */
                memcpy((char *)dst_member_ptr,
                       (char *)src_member_ptr, sizeof(SR_INT32));
                break;
            case IP_ADDR_PRIM_TYPE:
                memcpy((char *)dst_member_ptr,
                       (char *)src_member_ptr, sizeof(SR_UINT32));
                break;
            case OCTET_PRIM_TYPE:
            case OPAQUE_PRIM_TYPE:
                temp_os_pp = (OctetString **) (src_member_ptr);
                temp_os_ptr =
                    CloneOctetString((OctetString *) (*temp_os_pp));
                if (temp_os_ptr == NULL) {
                    return -1;
                }
                memcpy((char *)dst_member_ptr,
                       (char *)(&temp_os_ptr), sizeof(OctetString *));
                break;
            case OBJECT_ID_TYPE:
                temp_oid_pp = (OID **) (src_member_ptr);
                temp_oid_ptr =
                    CloneOID((OID *) (*temp_oid_pp));
                if (temp_oid_ptr == NULL) {
                    return -1;
                }
                memcpy((char *)dst_member_ptr,
                       (char *)(&temp_oid_ptr), sizeof(OID *));
                break;
            default:
                DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname,
                         type_table[nominator].type));
                return -1;
        }
        SET_VALID(nominator, dst_valid_ptr);
    }
    return 0;
}


/*
 * v_test(): system indepented test method.
 * Return NO_ERROR(0) on success or the appropriate error code on failure.
 */
int 
v_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo,
       const SrTestInfoEntry *testinfo)
{
    int i;
    int found;
    int error_status = NO_ERROR;
    int carry = 0;
    doList_t *dp;
    void *cur_data = NULL;
    void *alloc_data;
    static SR_INT32  *ipos = NULL;            /* instance index positions */
    static int ipos_size = 0;
    int instLength = incoming->length - object->oid.length;
    const SrGetInfoEntry *getinfo = testinfo->getinfo;
    char *valid;


    /*
     * allocate memory for the <family>_t structure.
     */
    if ((alloc_data = malloc (getinfo->family_size)) == NULL) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    /* memset to 0 to make sure the valid bits are all initialized to 0 */
    memset(alloc_data, 0, getinfo->family_size);


    /*
     * if index_info is NULL, it's a scalar. otherwise, it's a table
     */
    if (getinfo->index_info == NULL) {
        /*
         * Validate the object instance. For scalars: 
         *   1) It must be of length 1  
         *   2) and the instance must be 0.
         */
        if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
            free(alloc_data);
            return NO_CREATION_ERROR;
        }
    }
    else {
        /*
         * For tables, extract the indexes
         */
        switch(ExtractIncomingIndex(incoming, instLength, getinfo,
                                     EXACT, object, alloc_data, &ipos,
                                     &ipos_size, &carry)) {
            case 0:
            case -1:
                free(alloc_data);
                return NO_CREATION_ERROR;
        }
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == testinfo->setfunc) &&
              (dp->data != NULL) &&
              (CompareTableIndices(dp->data, alloc_data, testinfo) == 0) ) {
            found = 1;
            break; 
        }
    }

    if (!found) {
        cur_data = getinfo->kfunc(-1, contextInfo, -1, EXACT, alloc_data);
        dp = doCur;

        dp->readyMethod = testinfo->readyfunc;
        dp->setMethod = testinfo->setfunc;
        dp->cleanupMethod = testinfo->cleanupfunc; 
        dp->undoMethod = testinfo->kundofunc;
        dp->state = SR_UNKNOWN;
        dp->hintvbs = NULL;
        dp->vbSubPos = 0;

        if (cur_data != NULL
        ) {    /* the row exists */

            /* set the valid bits for the index columns in case the k_ 
             * routine did not. if the k_ routines are required to set
             * the valid bits for the index field, this is not necessary.
             *
             * we could copy the index values from alloc_data if the
             * index valid bits in cur_data are not set. this would allow
             * the k_ get routines to not fill in anything in some cases.
             */
            if (getinfo->index_info) {  /* if its a table */
                for (i = 0; getinfo->index_info[i].nominator != -1; i++) {
                    valid = ((char *)cur_data) + getinfo->valid_offset;
                    SET_VALID(getinfo->index_info[i].nominator, valid);
                }
            }

            /*
             * Copy the values returned from the k_ get routine to 
             * dp->data and dp->undo_data
             */
            error_status = FillInCurrentValues(dp, cur_data, testinfo);
        }
        else {    /* the row does not exists */
            if ( (dp->data = (void *) malloc (getinfo->family_size)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                SrFreeFamily(alloc_data, getinfo->type_table, 
                             getinfo->highest_nominator, 
                             getinfo->valid_offset, 
                             getinfo->userpart_free_func);
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, getinfo->family_size);
                dp->undodata = NULL;

                if (getinfo->index_info != NULL) {
                    /* Fill in reasonable default values for this new entry */
                    CopyIndexFields(dp->data, alloc_data, testinfo);
                    if (testinfo->kdefaultfunc) {
                        error_status = testinfo->kdefaultfunc(dp);
                    }
                }
            }
        }
    }

    SrFreeFamily(alloc_data, getinfo->type_table, getinfo->highest_nominator,
                 getinfo->valid_offset, getinfo->userpart_free_func);

    if (error_status != NO_ERROR) {
        return error_status;
    }

        error_status = TestAndCopyObject(object, value, doHead, contextInfo,
                                      dp, testinfo);

    /*
     * EMANATE version 1 and 2 (pre 16.1) do not have a separate pass for 
     * the ready phase, so call ready from the test method. per_varbind_ready
     * is a global variable that can be used to cause new subagents to behave
     * the old (pre 16.1) way.
     */
    if ((GetEmanateVersion() <= 2) || per_varbind_ready) {
        if (error_status == NO_ERROR) {
            /* call the k_ ready method once for each column in the row */
            error_status = testinfo->kreadyfunc(object, value, doHead, dp);
        }
    } else {
        /* EMANATE version 3 and later (release 16.1 and later) will 
         * call the ready method in a separate pass if dp->state is 
         * SR_PENDING_STATE */
        dp->state = SR_PENDING_STATE;
    }
    return error_status;
}

/*
 * RowStatus_test(): test routine for RowStatus textual convention.
 */
int
RowStatus_test(ObjectSyntax *value, ObjectSyntax *oldvalue)
{
    if ((value->sl_value < 1) || (value->sl_value > 6)) {
        return WRONG_VALUE_ERROR;
    }
    return CheckRowStatus(value->sl_value,
                          oldvalue == NULL ? 0L : oldvalue->sl_value);
}


/*
 * DisplayString_test(): test routine for DisplayString textual convention.
 */
int
DisplayString_test(ObjectSyntax *value, ObjectSyntax *oldvalue)
{
     if (!IsDisplayString(value->os_value)) {
        return WRONG_VALUE_ERROR;
     }
     return NO_ERROR;
}

/*
 * StorageType_test(): test routine for StorageType textual convention.
 */
#define ST_OTHER       1
#define ST_VOLATILE    2
#define ST_NONVOLATILE 3
#define ST_PERMANENT   4
#define ST_READONLY    5

int
StorageType_test(ObjectSyntax *value, ObjectSyntax *oldvalue)
{
    if ((value->sl_value < 1) || (value->sl_value > 5)) {
        return WRONG_VALUE_ERROR;
    }
    if (oldvalue != NULL) {
        /* check for attempts to change 'permanent' to other value */
        if ((oldvalue->sl_value == ST_PERMANENT) &&
            (value->sl_value != ST_PERMANENT)) {
            /* permanent storageType cannot be changed */
            return WRONG_VALUE_ERROR;
        }
        if ((oldvalue->sl_value < ST_PERMANENT) &&
            (value->sl_value >= ST_PERMANENT)) {
            return WRONG_VALUE_ERROR;
        }
    }
    
    return NO_ERROR;
}

/*
 * TestAndIncr_test(): test routine for TestAndIncr textual convention.
 */
int
TestAndIncr_test(ObjectSyntax *value, ObjectSyntax *oldvalue)
{
    if (oldvalue != NULL) {
        if (value->sl_value != oldvalue->sl_value) {
            return INCONSISTENT_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

/*
 * Identity_test(): Make sure the old value and the new value are the same.
 *
 * In SNMPv2, the index should be not-accessible, but
 * in SNMPv1, an index could be read-write. If the
 * index is read-write, then we must make sure that
 * the value matches the instance.
 */
int
Identity_test(ObjectSyntax *value, ObjectSyntax *oldvalue)
{
    if (oldvalue == NULL) {
        return NO_ERROR;
    }
    if (value->type != oldvalue->type) {
        /* should never happen */
        DPRINTF((APERROR, "Identity_test: Internal Error\n"));
        return GEN_ERROR;
    }

    switch (value->type) {
        case INTEGER_TYPE:
            if (value->sl_value != oldvalue->sl_value) {
                return WRONG_VALUE_ERROR;
            }
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            if (value->ul_value != oldvalue->ul_value) {
                return WRONG_VALUE_ERROR;
            }
            break;
        case IP_ADDR_PRIM_TYPE:
            if (CmpOctetStrings(value->os_value, oldvalue->os_value) != 0) {
                return WRONG_VALUE_ERROR;
            }
            break;
        case OCTET_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
            if (CmpOctetStrings(value->os_value, oldvalue->os_value) != 0) {
                return WRONG_VALUE_ERROR;
            }
            break;
        case OBJECT_ID_TYPE:
            if (CmpOID(value->oid_value, oldvalue->oid_value) != 0) {
                return WRONG_VALUE_ERROR;
            }
            break;
#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
            break;
#endif /* SR_SNMPv2_PDU */
        default:
            DPRINTF((APWARN, "Identity_test: unknown type\n"));
            return NO_ERROR;
    }
 
    return NO_ERROR;
}

int
v_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo, 
        const SrTestInfoEntry *testinfo)
{
    int error_status;

    error_status = (testinfo->kreadyfunc) (NULL, NULL, doHead, doCur);


    return error_status;
}

/*
 * GetSetFunction(): called by v_ set routines to get the parameter 'function'
 * that is passed to the k_ routine. The purpose is to map APO set types to
 * the old 'state' field in the doList structure. For non-APO requests, always 
 * return state.
 */
int
GetSetFunction(int set_kind, int state)
{
   return state;
}


