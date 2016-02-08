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
 */


/*
 * Arguments used to create this file:
 * -apo -c apoCap apoCapabilitiesMIB -userpart -v_stubs_only -search_table (implies -row_status) 
 * 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 apoCapabilitiesObjects_last_sid_array[] = {
    1, /* apoSupportedPduClasses */
    2, /* apoSupportedSnmpVersions */
    3, /* apoSupportedSecurityModels */
    5, /* apoSupportedAuthProtocols */
    7  /* apoSupportedPrivProtocols */
};

const SnmpType apoCapabilitiesObjectsTypeTable[] = {
    { BITS_TYPE, SR_READ_ONLY, offsetof(apoCapabilitiesObjects_t, apoSupportedPduClasses), -1 },
    { BITS_TYPE, SR_READ_ONLY, offsetof(apoCapabilitiesObjects_t, apoSupportedSnmpVersions), -1 },
    { BITS_TYPE, SR_READ_ONLY, offsetof(apoCapabilitiesObjects_t, apoSupportedSecurityModels), -1 },
    { BITS_TYPE, SR_READ_ONLY, offsetof(apoCapabilitiesObjects_t, apoSupportedAuthProtocols), -1 },
    { BITS_TYPE, SR_READ_ONLY, offsetof(apoCapabilitiesObjects_t, apoSupportedPrivProtocols), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry apoCapabilitiesObjectsGetInfo = {
    (SR_KGET_FPTR) new_k_apoCapabilitiesObjects_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(apoCapabilitiesObjects_t),
    I_apoCapabilitiesObjects_max,
    (SnmpType *) apoCapabilitiesObjectsTypeTable,
    NULL,
    (short) offsetof(apoCapabilitiesObjects_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the apoCapabilitiesObjects family.
 *---------------------------------------------------------------------*/
VarBind *
apoCapabilitiesObjects_get(OID *incoming, ObjectInfo *object, int searchType,
                           ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &apoCapabilitiesObjectsGetInfo));
}

apoCapabilitiesObjects_t *
new_k_apoCapabilitiesObjects_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator, int searchType,
                                 apoCapabilitiesObjects_t *data)
{
    return k_apoCapabilitiesObjects_get(serialNum, contextInfo, nominator);
}

/*----------------------------------------------------------------------
 * Free the apoCapabilitiesObjects data object.
 *---------------------------------------------------------------------*/
void
apoCapabilitiesObjects_free(apoCapabilitiesObjects_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 apoCapabilitiesObjectsGetInfo.type_table,
                 apoCapabilitiesObjectsGetInfo.highest_nominator,
                 apoCapabilitiesObjectsGetInfo.valid_offset,
                 apoCapabilitiesObjectsGetInfo.userpart_free_func);
}

static const SR_UINT32 apoAdditionalSupportedSecurityModelsEntry_last_sid_array[] = {
    1, /* apoAdditionalSupportedSecurityModels */
    2  /* apoAdditionalSupportedSecurityModelsDescr */
};

/*
 * The apoAdditionalSupportedSecurityModelsEntryTypeTable array should be located in the
 * k_apoCap.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType apoAdditionalSupportedSecurityModelsEntryTypeTable[];

const SrIndexInfo apoAdditionalSupportedSecurityModelsEntryIndexInfo[] = {
#ifdef I_apoAdditionalSupportedSecurityModels
    { I_apoAdditionalSupportedSecurityModels, T_uint, -1 },
#endif /* I_apoAdditionalSupportedSecurityModels */
    { -1, -1, -1 }
};

const SrGetInfoEntry apoAdditionalSupportedSecurityModelsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_apoAdditionalSupportedSecurityModelsEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(apoAdditionalSupportedSecurityModelsEntry_t),
    I_apoAdditionalSupportedSecurityModelsEntry_max,
    (SnmpType *) apoAdditionalSupportedSecurityModelsEntryTypeTable,
    apoAdditionalSupportedSecurityModelsEntryIndexInfo,
    (short) offsetof(apoAdditionalSupportedSecurityModelsEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the apoAdditionalSupportedSecurityModelsEntry family.
 *---------------------------------------------------------------------*/
VarBind *
apoAdditionalSupportedSecurityModelsEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                                              ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_apoAdditionalSupportedSecurityModels) )
    return NULL;
#else	/* ! ( defined(I_apoAdditionalSupportedSecurityModels) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &apoAdditionalSupportedSecurityModelsEntryGetInfo));
#endif	/* ! ( defined(I_apoAdditionalSupportedSecurityModels) ) */
}

apoAdditionalSupportedSecurityModelsEntry_t *
new_k_apoAdditionalSupportedSecurityModelsEntry_get(int serialNum, ContextInfo *contextInfo,
                                                    int nominator, int searchType,
                                                    apoAdditionalSupportedSecurityModelsEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_apoAdditionalSupportedSecurityModelsEntry_get(serialNum, contextInfo, nominator,
                                                           searchType,
                                                           data->apoAdditionalSupportedSecurityModels);
}


/*----------------------------------------------------------------------
 * Free the apoAdditionalSupportedSecurityModelsEntry data object.
 *---------------------------------------------------------------------*/
void
apoAdditionalSupportedSecurityModelsEntry_free(apoAdditionalSupportedSecurityModelsEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 apoAdditionalSupportedSecurityModelsEntryGetInfo.type_table,
                 apoAdditionalSupportedSecurityModelsEntryGetInfo.highest_nominator,
                 apoAdditionalSupportedSecurityModelsEntryGetInfo.valid_offset,
                 apoAdditionalSupportedSecurityModelsEntryGetInfo.userpart_free_func);
}

static const SR_UINT32 apoAdditionalSupportedAuthProtocolsEntry_last_sid_array[] = {
    1, /* apoAdditionalSupportedAuthProtocols */
    2  /* apoAdditionalSupportedAuthProtocolsDescr */
};

/*
 * The apoAdditionalSupportedAuthProtocolsEntryTypeTable array should be located in the
 * k_apoCap.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType apoAdditionalSupportedAuthProtocolsEntryTypeTable[];

const SrIndexInfo apoAdditionalSupportedAuthProtocolsEntryIndexInfo[] = {
#ifdef I_apoAdditionalSupportedAuthProtocols
    { I_apoAdditionalSupportedAuthProtocols, T_var_oid, -1 },
#endif /* I_apoAdditionalSupportedAuthProtocols */
    { -1, -1, -1 }
};

const SrGetInfoEntry apoAdditionalSupportedAuthProtocolsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_apoAdditionalSupportedAuthProtocolsEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(apoAdditionalSupportedAuthProtocolsEntry_t),
    I_apoAdditionalSupportedAuthProtocolsEntry_max,
    (SnmpType *) apoAdditionalSupportedAuthProtocolsEntryTypeTable,
    apoAdditionalSupportedAuthProtocolsEntryIndexInfo,
    (short) offsetof(apoAdditionalSupportedAuthProtocolsEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the apoAdditionalSupportedAuthProtocolsEntry family.
 *---------------------------------------------------------------------*/
VarBind *
apoAdditionalSupportedAuthProtocolsEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                                             ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_apoAdditionalSupportedAuthProtocols) )
    return NULL;
#else	/* ! ( defined(I_apoAdditionalSupportedAuthProtocols) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &apoAdditionalSupportedAuthProtocolsEntryGetInfo));
#endif	/* ! ( defined(I_apoAdditionalSupportedAuthProtocols) ) */
}

apoAdditionalSupportedAuthProtocolsEntry_t *
new_k_apoAdditionalSupportedAuthProtocolsEntry_get(int serialNum, ContextInfo *contextInfo,
                                                   int nominator, int searchType,
                                                   apoAdditionalSupportedAuthProtocolsEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_apoAdditionalSupportedAuthProtocolsEntry_get(serialNum, contextInfo, nominator,
                                                          searchType,
                                                          data->apoAdditionalSupportedAuthProtocols);
}


/*----------------------------------------------------------------------
 * Free the apoAdditionalSupportedAuthProtocolsEntry data object.
 *---------------------------------------------------------------------*/
void
apoAdditionalSupportedAuthProtocolsEntry_free(apoAdditionalSupportedAuthProtocolsEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 apoAdditionalSupportedAuthProtocolsEntryGetInfo.type_table,
                 apoAdditionalSupportedAuthProtocolsEntryGetInfo.highest_nominator,
                 apoAdditionalSupportedAuthProtocolsEntryGetInfo.valid_offset,
                 apoAdditionalSupportedAuthProtocolsEntryGetInfo.userpart_free_func);
}

static const SR_UINT32 apoAdditionalSupportedPrivProtocolsEntry_last_sid_array[] = {
    1, /* apoAdditionalSupportedPrivProtocols */
    2  /* apoAdditionalSupportedPrivProtocolsDescr */
};

/*
 * The apoAdditionalSupportedPrivProtocolsEntryTypeTable array should be located in the
 * k_apoCap.stb file, because one of the following command-line
 * arguments was used: -row_status, -search_table, or -parser.
 */
extern const SnmpType apoAdditionalSupportedPrivProtocolsEntryTypeTable[];

const SrIndexInfo apoAdditionalSupportedPrivProtocolsEntryIndexInfo[] = {
#ifdef I_apoAdditionalSupportedPrivProtocols
    { I_apoAdditionalSupportedPrivProtocols, T_var_oid, -1 },
#endif /* I_apoAdditionalSupportedPrivProtocols */
    { -1, -1, -1 }
};

const SrGetInfoEntry apoAdditionalSupportedPrivProtocolsEntryGetInfo = {
    (SR_KGET_FPTR) new_k_apoAdditionalSupportedPrivProtocolsEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(apoAdditionalSupportedPrivProtocolsEntry_t),
    I_apoAdditionalSupportedPrivProtocolsEntry_max,
    (SnmpType *) apoAdditionalSupportedPrivProtocolsEntryTypeTable,
    apoAdditionalSupportedPrivProtocolsEntryIndexInfo,
    (short) offsetof(apoAdditionalSupportedPrivProtocolsEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the apoAdditionalSupportedPrivProtocolsEntry family.
 *---------------------------------------------------------------------*/
VarBind *
apoAdditionalSupportedPrivProtocolsEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                                             ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_apoAdditionalSupportedPrivProtocols) )
    return NULL;
#else	/* ! ( defined(I_apoAdditionalSupportedPrivProtocols) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &apoAdditionalSupportedPrivProtocolsEntryGetInfo));
#endif	/* ! ( defined(I_apoAdditionalSupportedPrivProtocols) ) */
}

apoAdditionalSupportedPrivProtocolsEntry_t *
new_k_apoAdditionalSupportedPrivProtocolsEntry_get(int serialNum, ContextInfo *contextInfo,
                                                   int nominator, int searchType,
                                                   apoAdditionalSupportedPrivProtocolsEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_apoAdditionalSupportedPrivProtocolsEntry_get(serialNum, contextInfo, nominator,
                                                          searchType,
                                                          data->apoAdditionalSupportedPrivProtocols);
}


/*----------------------------------------------------------------------
 * Free the apoAdditionalSupportedPrivProtocolsEntry data object.
 *---------------------------------------------------------------------*/
void
apoAdditionalSupportedPrivProtocolsEntry_free(apoAdditionalSupportedPrivProtocolsEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 apoAdditionalSupportedPrivProtocolsEntryGetInfo.type_table,
                 apoAdditionalSupportedPrivProtocolsEntryGetInfo.highest_nominator,
                 apoAdditionalSupportedPrivProtocolsEntryGetInfo.valid_offset,
                 apoAdditionalSupportedPrivProtocolsEntryGetInfo.userpart_free_func);
}

