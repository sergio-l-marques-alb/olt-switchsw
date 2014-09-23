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

#include <malloc.h>


#include <string.h>

#include "sr_type.h"
#include "sr_proto.h"
#include "diag.h"

SR_FILENAME

#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "inst_lib.h"

/*
 * The atTable has been depricated by the ipNetToMediaTable.  Support
 * for the atTable is provided, however, by translating requests to
 * equivalent requests in the ipNetToMediaTable.
 */

atEntry_t *
k_atEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 atIfIndex,
   OctetString * atNetAddress)
{
   static atEntry_t       atEntryData = { 0, NULL, NULL, { '\0' }, { '\0' } };
   ipNetToMediaEntry_t   *temp_data;

    /* clean up the static structure */
    if (atEntryData.atPhysAddress != NULL) {
	FreeOctetString(atEntryData.atPhysAddress);
        atEntryData.atPhysAddress = NULL;  /* LVL7 */
    }
    if (atEntryData.atNetAddress != NULL) {
	FreeOctetString(atEntryData.atNetAddress);
        atEntryData.atNetAddress = NULL;  /* LVL7 */
    }
    ZERO_VALID(atEntryData.valid);

    temp_data = k_ipNetToMediaEntry_get(serialNum, contextInfo, nominator, searchType, atIfIndex, NetworkAddressToIP(atNetAddress));

    if (temp_data == NULL) {
        goto fail;
    } else {
	/* fill the static structure */

#ifdef I_ipNetToMediaIfIndex
	if (VALID(I_ipNetToMediaIfIndex, temp_data->valid)) {
	    /* Copy ipNetToMediaIfIndex to atIfIndex */
	    atEntryData.atIfIndex = temp_data->ipNetToMediaIfIndex;
	    SET_VALID(I_atIfIndex, atEntryData.valid);
	}
#endif /* I_ipNetToMediaIfIndex */

#ifdef I_ipNetToMediaPhysAddress
	if (VALID(I_ipNetToMediaPhysAddress, temp_data->valid)) {
	    /* Copy ipNetToMediaPhysAddress to atPhysAddress */
	    if ((atEntryData.atPhysAddress =
		CloneOctetString(temp_data->ipNetToMediaPhysAddress)) == NULL) {
                goto fail;
            }
	    SET_VALID(I_atPhysAddress, atEntryData.valid);
	}
#endif /* I_ipNetToMediaPhysAddress */

#ifdef I_ipNetToMediaNetAddress
	if (VALID(I_ipNetToMediaNetAddress, temp_data->valid)) {
	    /* Copy ipNetToMediaNetAddress to atNetAddress */
	    if ((atEntryData.atNetAddress =
	       IPToOctetString(temp_data->ipNetToMediaNetAddress)) == NULL) {
                if (VALID(I_atPhysAddress, atEntryData.valid)) {
                    FreeOctetString(atEntryData.atPhysAddress);
                }
                goto fail;
            }
	    SET_VALID(I_atNetAddress, atEntryData.valid);
	}
#endif /* I_ipNetToMediaNetAddress */

	return(&atEntryData);
   }
fail:
    atEntryData.atPhysAddress = NULL;
    atEntryData.atNetAddress = NULL;
    return NULL;
}

#ifdef SETS
int
k_atEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_atEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_atEntry_set_defaults(dp)
    doList_t       *dp;
{
    atEntry_t *data = (atEntry_t *) (dp->data);

    if ((data->atPhysAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}


/*
 * To translate set requests, build the correct structure and call
 * k_ipNetToMediaEntry_set.
 */
int
k_atEntry_set(data, contextInfo, function)
   atEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    static ipNetToMediaEntry_t new_data;

    DPRINTF((APTRACE, "converting atEntry_t to ipNetToMediaEntry_t\n"));

    ZERO_VALID(new_data.valid);

    /*
     * Copy atIfIndex to ipNetToMediaIfIndex
     */
#ifdef I_ipNetToMediaIfIndex
    new_data.ipNetToMediaIfIndex = data->atIfIndex;
    if (VALID(I_atIfIndex, data->valid)) {
	SET_VALID(I_ipNetToMediaIfIndex, new_data.valid);
    }
#endif /* I_ipNetToMediaIfIndex */

    /*
     * The atTable did not contain a field for type, so we have
     * to choose one to fill in--"static" is a reasonable value,
     * but it could have also been D_ipNetToMediaType_dynamic
     */
#ifdef I_ipNetToMediaType
    new_data.ipNetToMediaType = D_ipNetToMediaType_static;
    SET_VALID(I_ipNetToMediaType, new_data.valid);
#endif /* I_ipNetToMediaType */


    /*
     * Copy atPhysAddress to ipNetToMediaPhysAddress
     */
#ifdef I_ipNetToMediaPhysAddress
    new_data.ipNetToMediaPhysAddress = data->atPhysAddress;
    if (VALID(I_atPhysAddress, data->valid)) {
	SET_VALID(I_ipNetToMediaPhysAddress, new_data.valid);
	/*
	 * A row in the atTable was deleted by setting atPhysAddress
	 * to zero.  In the ipNetToMediaTable, a row is deleted by
	 * setting ipNetToMediaType to invalid.  If necessary, change
	 * the default value given to ipNetToMediaType.
	 */
	if (!data->atPhysAddress->length) {
	    new_data.ipNetToMediaType = D_ipNetToMediaType_invalid;
	}
    }
#endif /* I_ipNetToMediaPhysAddress */

    /*
     * Copy atNetAddress to ipNetToMediaNetAddress
     */
#ifdef I_ipNetToMediaNetAddress
    new_data.ipNetToMediaNetAddress = (SR_UINT32) 0;  /* initialize stack */
    if (VALID(I_atNetAddress, data->valid)) {
	new_data.ipNetToMediaNetAddress = OctetStringToIP(data->atNetAddress);
	SET_VALID(I_ipNetToMediaNetAddress, new_data.valid);
    }
#endif /* I_ipNetToMediaNetAddress */

    return k_ipNetToMediaEntry_set(&new_data, contextInfo, function);
}


#ifdef SR_atEntry_UNDO
/* add #define SR_atEntry_UNDO in sitedefs.h to
 * include the undo routine for the atEntry family.
 */
int
atEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_atEntry_UNDO */

#endif /* SETS */
