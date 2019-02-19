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

/*   This file defines types and globals which are used in conjunction
 |   with code produced by postmosy when the -minimal_v option is used.
 */

#ifndef MIN_V_H
#define MIN_V_H

#ifdef  __cplusplus
extern "C" {
#endif

/* T_ values 1-5 are found in lookup.h */
#define T_fixed_octet 6
#define T_fixed_oid   7
#define T_networkaddress 8

/*
 * %%% DSS double-check mappings
 */
#define MINV_OCTET_PRIM_VARIABLE T_var_octet    /* was 0x04 */
#define MINV_OCTET_PRIM_FIXED    T_fixed_octet  /* was 0x10 */
#define MINV_OCTET_PRIM_IMPLIED  T_octet        /* was 0x11 */
#define MINV_OID_VARIABLE        T_var_oid      /* was 0x06 */
#define MINV_OID_IMPLIED         T_oid          /* was 0x12 */
#define MINV_OID_FIXED           T_fixed_oid    /* was 0x13 */

typedef void *(*SR_KGET_FPTR)(int, ContextInfo *, int, int, void *);
typedef void (*SR_COPY_FPTR)(void *, void *);
typedef void (*SR_FREE_FPTR)(void *);

typedef struct _srIndexInfo {
    short nominator;  /* SnmpType array Element for this index */
    short type;       /* one of the T_ values */
    short size;       /* size of fixed-length index (-1 otherwise) */
} SrIndexInfo;

typedef struct
{
    SR_KGET_FPTR kfunc;
    SR_FREE_FPTR userpart_free_func;
    int family_size;
    int highest_nominator;
    SnmpType *type_table;     /* points to an array */
    const SrIndexInfo *index_info;  /* points to an array */
    short valid_offset;
    /* %%% DSS add a debug string field? */
} SrGetInfoEntry;

VarBind *v_get(OID *incoming,
	       ObjectInfo *object,
	       int searchType,
	       ContextInfo *contextInfo,
	       int serialNum,
	       const SrGetInfoEntry *getinfo);

#define MINV_NOT_WRITABLE         0
#define MINV_NO_TEST              0
#define MINV_INTEGER_RANGE_TEST   1
#define MINV_UINTEGER_RANGE_TEST  2
#define MINV_INTEGER_ENUM_TEST    3
#define MINV_BITS_TEST            4
#define MINV_LENGTH_TEST          5
#define MINV_CODED_TEST           6

typedef struct RangeTest_s {      /* MINV_INTEGER_RANGE_TEST */
    SR_INT32 minimum;
    SR_INT32 maximum;
} RangeTest_t;

typedef struct URangeTest_s {     /* MINV_UINTEGER_RANGE_TEST */
    SR_UINT32 minimum;
    SR_UINT32 maximum;
} URangeTest_t;

typedef SR_INT32 EnumIntTest_t;   /* MINV_INTEGER_ENUM_TEST */

typedef int BitsTest_t;           /* MINV_BITS_TEST */

typedef struct LengthTest_s {     /* MINV_LENGTH_TEST */
    SR_UINT32 minimum;
    SR_UINT32 maximum;
} LengthTest_t;

typedef struct
{
    const SrGetInfoEntry *getinfo;

    struct sr_member_test {
        char test_kind;
        int test_data_size;
        void *test_data;
        int (*testfunc)(ObjectSyntax *,ObjectSyntax *);/* MINV_CODED_TEST */
    } *member_test;

    int (*kdefaultfunc)(doList_t *dp);
    int (*ktestfunc)(ObjectInfo *, ObjectSyntax *, doList_t *,
                     ContextInfo *);
    int (*kreadyfunc)(ObjectInfo *, ObjectSyntax *, doList_t *,
                      doList_t *);
    int (*kundofunc)(doList_t *, doList_t *, ContextInfo *);
    int (*readyfunc)(doList_t *, doList_t *, ContextInfo *);
    int (*setfunc)(doList_t *, doList_t *, ContextInfo *);
    int (*cleanupfunc)(doList_t *);
    SR_COPY_FPTR userpart_clone_func;
} SrTestInfoEntry;


/*
 * prototypes for functions defined in minv.c 
 */

VarBind *SrBuildReturnVarBind(short offset, void *data,
                              unsigned char family_type,
                              ObjectInfo *object,
                              OID *inst);

int
CheckScalarInstance(OID *incoming,
                    ObjectInfo *object,
                    int searchType);

void
SrZeroValid(char *valid,
            int highest_nominator);

int
SrCopyFamilyContents(void *dst, void *src,
                     const SnmpType *type_table,
                     int highest_nominator,
                     short valid_offset,
                     SR_COPY_FPTR userpart_clone_func,
                     SR_FREE_FPTR userpart_free_func);

void
SrFreeFamilyContents(void *data,
                     const SnmpType *type_table,
                     int highest_nominator,
                     short valid_offset,
                     SR_FREE_FPTR userpart_free_func);

void *
SrCloneFamily(void *data,
              int family_size,
              const SnmpType *type_table,
              int highest_nominator,
              short valid_offset,
              SR_COPY_FPTR userpart_clone_func,
              SR_FREE_FPTR userpart_free_func);

void
SrFreeFamily(void *data,
             const SnmpType *type_table,
             int highest_nominator,
             short valid_offset,
             SR_FREE_FPTR userpart_free_func);

int
ExtractIncomingIndex(OID *incoming,
                     int instLength,
                     const SrGetInfoEntry *info_entry,
                     int searchType,
                     ObjectInfo *new_object,
                     void *alloc_data,
                     SR_INT32 **ipos_update,
                     int *ipos_size_update,
                     int *carry_return);

OID *
ExtractIndexFromData(const SrGetInfoEntry *info_entry, void *data);

/*
 * prototypes for functions defined in minvtest.c 
 */

int
FillInCurrentValues(doList_t *dp, void *data, const SrTestInfoEntry *testinfo);

int
TestAndCopyObject(ObjectInfo *object, ObjectSyntax *value, doList_t *doHead,
           ContextInfo *contextInfo, doList_t *dp, 
           const SrTestInfoEntry *testinfo);

int
SrCleanup(doList_t *trash, const SrTestInfoEntry *testinfo);

int    
CopyIndexFields (void *dst, void *src, const SrTestInfoEntry *testinfo);

int
CompareTableIndices (void *dst, void *src, const SrTestInfoEntry *testinfo);

int
v_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
       doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo,
       const SrTestInfoEntry *testinfo);

int
v_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo,
        const SrTestInfoEntry *testinfo);

int
RowStatus_test(ObjectSyntax *value, ObjectSyntax *oldvalue);

int
DisplayString_test(ObjectSyntax *value, ObjectSyntax *oldvalue);

int
StorageType_test(ObjectSyntax *value, ObjectSyntax *oldvalue);

int
TestAndIncr_test(ObjectSyntax *value, ObjectSyntax *oldvalue);

int
Identity_test(ObjectSyntax *value, ObjectSyntax *oldvalue);

int
GetSetFunction(int set_kind, int state);




#ifdef  __cplusplus
}
#endif

#endif /* MIN_V_H */
