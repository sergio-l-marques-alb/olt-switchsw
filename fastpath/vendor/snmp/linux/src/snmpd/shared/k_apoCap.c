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
 * -agent -search_table (implies -row_status) -o apoCap 
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "min_v.h"
#include "mibout.h"
#include "compat.h"


/* global data describing the apoCapabilitiesObjects family */
static apoCapabilitiesObjects_t apoCapabilitiesObjectsData;

/* global data describing the apoAdditionalSupportedSecurityModelsTable entries */
Index_t apoAdditionalSupportedSecurityModelsEntryIndex[] = {
    { offsetof(apoAdditionalSupportedSecurityModelsEntry_t, apoAdditionalSupportedSecurityModels),  T_uint}
};

SnmpV2Table apoAdditionalSupportedSecurityModelsTable = {
    NULL,
    apoAdditionalSupportedSecurityModelsEntryIndex,
    0,
    1,
    sizeof(apoAdditionalSupportedSecurityModelsEntry_t)
};

const SnmpType apoAdditionalSupportedSecurityModelsEntryTypeTable[] = {
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(apoAdditionalSupportedSecurityModelsEntry_t, apoAdditionalSupportedSecurityModels), 0 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(apoAdditionalSupportedSecurityModelsEntry_t, apoAdditionalSupportedSecurityModelsDescr), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeleteapoAdditionalSupportedSecurityModelsEntry
    SR_PROTOTYPE((int index));
extern void apoAdditionalSupportedSecurityModelsEntry_free
    SR_PROTOTYPE((apoAdditionalSupportedSecurityModelsEntry_t *data));

/* global data describing the apoAdditionalSupportedAuthProtocolsTable entries */
Index_t apoAdditionalSupportedAuthProtocolsEntryIndex[] = {
    { offsetof(apoAdditionalSupportedAuthProtocolsEntry_t, apoAdditionalSupportedAuthProtocols),  T_var_oid}
};

SnmpV2Table apoAdditionalSupportedAuthProtocolsTable = {
    NULL,
    apoAdditionalSupportedAuthProtocolsEntryIndex,
    0,
    1,
    sizeof(apoAdditionalSupportedAuthProtocolsEntry_t)
};

const SnmpType apoAdditionalSupportedAuthProtocolsEntryTypeTable[] = {
    { OBJECT_ID_TYPE, SR_NOT_ACCESSIBLE, offsetof(apoAdditionalSupportedAuthProtocolsEntry_t, apoAdditionalSupportedAuthProtocols), 0 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(apoAdditionalSupportedAuthProtocolsEntry_t, apoAdditionalSupportedAuthProtocolsDescr), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeleteapoAdditionalSupportedAuthProtocolsEntry
    SR_PROTOTYPE((int index));
extern void apoAdditionalSupportedAuthProtocolsEntry_free
    SR_PROTOTYPE((apoAdditionalSupportedAuthProtocolsEntry_t *data));

/* global data describing the apoAdditionalSupportedPrivProtocolsTable entries */
Index_t apoAdditionalSupportedPrivProtocolsEntryIndex[] = {
    { offsetof(apoAdditionalSupportedPrivProtocolsEntry_t, apoAdditionalSupportedPrivProtocols),  T_var_oid}
};

SnmpV2Table apoAdditionalSupportedPrivProtocolsTable = {
    NULL,
    apoAdditionalSupportedPrivProtocolsEntryIndex,
    0,
    1,
    sizeof(apoAdditionalSupportedPrivProtocolsEntry_t)
};

const SnmpType apoAdditionalSupportedPrivProtocolsEntryTypeTable[] = {
    { OBJECT_ID_TYPE, SR_NOT_ACCESSIBLE, offsetof(apoAdditionalSupportedPrivProtocolsEntry_t, apoAdditionalSupportedPrivProtocols), 0 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE, offsetof(apoAdditionalSupportedPrivProtocolsEntry_t, apoAdditionalSupportedPrivProtocolsDescr), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};


void DeleteapoAdditionalSupportedPrivProtocolsEntry
    SR_PROTOTYPE((int index));
extern void apoAdditionalSupportedPrivProtocolsEntry_free
    SR_PROTOTYPE((apoAdditionalSupportedPrivProtocolsEntry_t *data));

/*BAB*/
char *conv_to_hex
    SR_PROTOTYPE((char *str));

char *check_supp_pdu_classes
    SR_PROTOTYPE((void));

char *check_supp_snmp_versions
    SR_PROTOTYPE((void));

char *check_supp_sec_models
    SR_PROTOTYPE((void));

char *check_supp_auth_prot
    SR_PROTOTYPE((void));

char *check_supp_priv_prot
    SR_PROTOTYPE((void));

/*
 *  Macro for testing bits in a bit string.
 */
#define IS_BIT_SET(oct, num)  \
     ((((oct)->octet_ptr)[(num)/8] & (0x80 >> ((num) % 8))) ? 1 : 0)

int
k_apoCap_initialize(void)
{
    char *bit_str=NULL;

    bit_str = check_supp_pdu_classes();
    if (bit_str != NULL)
    {
      apoCapabilitiesObjectsData.apoSupportedPduClasses = MakeOctetStringFromHex(conv_to_hex(bit_str));
    }

    bit_str = check_supp_snmp_versions();
    if (bit_str != NULL)
    {
      apoCapabilitiesObjectsData.apoSupportedSnmpVersions = MakeOctetStringFromHex(conv_to_hex(bit_str));
    }

    bit_str = check_supp_sec_models();
    if (bit_str != NULL)
    {
      apoCapabilitiesObjectsData.apoSupportedSecurityModels = MakeOctetStringFromHex(conv_to_hex(bit_str));
    }

    bit_str = check_supp_auth_prot();
    /* Check if NULL for v1/v2c agents */
    if(bit_str) {
      apoCapabilitiesObjectsData.apoSupportedAuthProtocols = 
		MakeOctetStringFromHex(conv_to_hex(bit_str));
    }
    else {
  	apoCapabilitiesObjectsData.apoSupportedAuthProtocols =
                MakeOctetStringFromText("");
    }

    bit_str = check_supp_priv_prot();
    /* Check if NULL for v1/v2c agents */
    if(bit_str) {
      apoCapabilitiesObjectsData.apoSupportedPrivProtocols =
                MakeOctetStringFromHex(conv_to_hex(bit_str));
    }
    else {
        apoCapabilitiesObjectsData.apoSupportedPrivProtocols =
                MakeOctetStringFromText("");
    }

    return 1;
}

int
k_apoCap_terminate(void)
{
    return 1;
}

apoCapabilitiesObjects_t *
k_apoCapabilitiesObjects_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
   /*
    * put your code to retrieve the information here
    */

   SET_ALL_VALID(apoCapabilitiesObjectsData.valid);
   return(&apoCapabilitiesObjectsData);
}

/* This routine deletes an entry from the apoAdditionalSupportedSecurityModelsTable */
void
DeleteapoAdditionalSupportedSecurityModelsEntry (int index)
{
    apoAdditionalSupportedSecurityModelsEntry_t *data;

    /* get a pointer to the old entry */
    data = (apoAdditionalSupportedSecurityModelsEntry_t *) apoAdditionalSupportedSecurityModelsTable.tp[index];

    /* free the old entry and remove it from the table */
    apoAdditionalSupportedSecurityModelsEntry_free(data);
    RemoveTableEntry(&apoAdditionalSupportedSecurityModelsTable, index);
}

apoAdditionalSupportedSecurityModelsEntry_t *
k_apoAdditionalSupportedSecurityModelsEntry_get(int serialNum, ContextInfo *contextInfo,
                                                int nominator,
                                                int searchType,
                                                SR_INT32 apoAdditionalSupportedSecurityModels)
{
    int index;

    apoAdditionalSupportedSecurityModelsTable.tip[0].value.uint_val = apoAdditionalSupportedSecurityModels;
    if ((index = SearchTable(&apoAdditionalSupportedSecurityModelsTable, searchType)) == -1) {
        return NULL;
    }

    return (apoAdditionalSupportedSecurityModelsEntry_t *) apoAdditionalSupportedSecurityModelsTable.tp[index];

}

/* This routine deletes an entry from the apoAdditionalSupportedAuthProtocolsTable */
void
DeleteapoAdditionalSupportedAuthProtocolsEntry (int index)
{
    apoAdditionalSupportedAuthProtocolsEntry_t *data;

    /* get a pointer to the old entry */
    data = (apoAdditionalSupportedAuthProtocolsEntry_t *) apoAdditionalSupportedAuthProtocolsTable.tp[index];

    /* free the old entry and remove it from the table */
    apoAdditionalSupportedAuthProtocolsEntry_free(data);
    RemoveTableEntry(&apoAdditionalSupportedAuthProtocolsTable, index);
}

apoAdditionalSupportedAuthProtocolsEntry_t *
k_apoAdditionalSupportedAuthProtocolsEntry_get(int serialNum, ContextInfo *contextInfo,
                                               int nominator,
                                               int searchType,
                                               OID * apoAdditionalSupportedAuthProtocols)
{
    int index;

    apoAdditionalSupportedAuthProtocolsTable.tip[0].value.oid_val = apoAdditionalSupportedAuthProtocols;
    if ((index = SearchTable(&apoAdditionalSupportedAuthProtocolsTable, searchType)) == -1) {
        return NULL;
    }

    return (apoAdditionalSupportedAuthProtocolsEntry_t *) apoAdditionalSupportedAuthProtocolsTable.tp[index];

}

/* This routine deletes an entry from the apoAdditionalSupportedPrivProtocolsTable */
void
DeleteapoAdditionalSupportedPrivProtocolsEntry (int index)
{
    apoAdditionalSupportedPrivProtocolsEntry_t *data;

    /* get a pointer to the old entry */
    data = (apoAdditionalSupportedPrivProtocolsEntry_t *) apoAdditionalSupportedPrivProtocolsTable.tp[index];

    /* free the old entry and remove it from the table */
    apoAdditionalSupportedPrivProtocolsEntry_free(data);
    RemoveTableEntry(&apoAdditionalSupportedPrivProtocolsTable, index);
}

apoAdditionalSupportedPrivProtocolsEntry_t *
k_apoAdditionalSupportedPrivProtocolsEntry_get(int serialNum, ContextInfo *contextInfo,
                                               int nominator,
                                               int searchType,
                                               OID * apoAdditionalSupportedPrivProtocols)
{
    int index;

    apoAdditionalSupportedPrivProtocolsTable.tip[0].value.oid_val = apoAdditionalSupportedPrivProtocols;
    if ((index = SearchTable(&apoAdditionalSupportedPrivProtocolsTable, searchType)) == -1) {
        return NULL;
    }

    return (apoAdditionalSupportedPrivProtocolsEntry_t *) apoAdditionalSupportedPrivProtocolsTable.tp[index];

}

#ifdef SETS
int
k_apoAdditionalSupportedPrivProtocolsEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                                doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    apoAdditionalSupportedPrivProtocolsEntry_t *data = (apoAdditionalSupportedPrivProtocolsEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_apoAdditionalSupportedPrivProtocolsDescr) {
        /*
         * Examine value->os_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_apoAdditionalSupportedPrivProtocolsDescr, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_apoAdditionalSupportedPrivProtocolsEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                                 doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    apoAdditionalSupportedPrivProtocolsEntry_t *data = (apoAdditionalSupportedPrivProtocolsEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_apoAdditionalSupportedPrivProtocolsDescr, data->valid)) {
        /*
         * Examine data->apoAdditionalSupportedPrivProtocolsDescr in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_apoAdditionalSupportedPrivProtocolsEntry_set_defaults(doList_t *dp)
{
    apoAdditionalSupportedPrivProtocolsEntry_t *data = (apoAdditionalSupportedPrivProtocolsEntry_t *) (dp->data);

    if ((data->apoAdditionalSupportedPrivProtocolsDescr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_apoAdditionalSupportedPrivProtocolsEntry_set(apoAdditionalSupportedPrivProtocolsEntry_t *data,
                                               ContextInfo *contextInfo, int function)
{

    int index;
    apoAdditionalSupportedPrivProtocolsEntry_t *newrow = NULL;

    /* find this entry in the table */
    apoAdditionalSupportedPrivProtocolsTable.tip[0].value.oid_val = data->apoAdditionalSupportedPrivProtocols;
    if ((index = SearchTable(&apoAdditionalSupportedPrivProtocolsTable, EXACT)) != -1) {
        newrow = (apoAdditionalSupportedPrivProtocolsEntry_t *) apoAdditionalSupportedPrivProtocolsTable.tp[index];
    }
    else {
        return COMMIT_FAILED_ERROR;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_apoAdditionalSupportedPrivProtocols, apoAdditionalSupportedPrivProtocolsEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);


    return NO_ERROR;

}

#ifdef SR_apoAdditionalSupportedPrivProtocolsEntry_UNDO
/* add #define SR_apoAdditionalSupportedPrivProtocolsEntry_UNDO in sitedefs.h to
 * include the undo routine for the apoAdditionalSupportedPrivProtocolsEntry family.
 */
int
apoAdditionalSupportedPrivProtocolsEntry_undo(doList_t *doHead, doList_t *doCur,
                                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_apoAdditionalSupportedPrivProtocolsEntry_UNDO */

#endif /* SETS */

char *
conv_to_hex(char *str) 
{
  if(strcmp("0001", str) == 0) {	
	return("10");
  }
  else if(strcmp("0010", str) == 0) {	
	return "20";
  }
  else if(strcmp("0011", str) == 0) {	
	return "30";
  }
  else if(strcmp("0100", str) == 0) {	
	return "40";
  }
  else if(strcmp("0101", str) == 0) {	
	return "50";
  }
  else if(strcmp("0110", str) == 0) {	
	return "60";
  }
  else if(strcmp("0111", str) == 0) {	
	return "70";
  }
  else if(strcmp("1000", str) == 0) {	
	return "80";
  }
  else if(strcmp("1001", str) == 0) {	
	return "90";
  }
  else if(strcmp("1010", str) == 0) {	
	return "A0";
  }
  else if(strcmp("1011", str) == 0) {	
	return "B0";
  }
  else if(strcmp("1100", str) == 0) {	
	return "C0";
  }
  else if(strcmp("1101", str) == 0) {	
	return "D0";
  }
  else if(strcmp("1110", str) == 0) {	
	return "E0";
  }
  else if(strcmp("1111", str) == 0) {	
	return "F0";
  }

  /*else*/
  return NULL;
}

char *
check_supp_pdu_classes(void)
{

 static char bit_string[4];
 short change_flag=0;

 memcpy(bit_string, "0000", sizeof(bit_string));

#ifdef SR_SNMPv1_PACKET
  bit_string[0] = '1';
  change_flag = 1;
#endif /*SNMPv1*/

#ifdef SR_SNMPv2_PDU
  bit_string[1] = '1';
  change_flag = 1;
#endif /*SNMPv2*/



  if(change_flag) {
    return(bit_string);
  }
  else {
    return NULL;
  }
}

char *
check_supp_snmp_versions(void)
{

  static char bit_string[4];
  short change_flag=0;

  memcpy(bit_string, "0000", sizeof(bit_string));

#ifdef SR_SNMPv1_PACKET
  bit_string[0] = '1';
  change_flag = 1;
#endif /*SNMPv1*/

#ifdef SR_SNMPv2c_PACKET
  bit_string[1] = '1';
  change_flag = 1;
#endif /*SNMPv2*/

#ifdef SR_SNMPv3_PACKET
  bit_string[2] = '1';
  change_flag = 1;
#endif /*SR_SNMPv3_PACKET*/

  if(change_flag) {
    return(bit_string);
  }
  else {
    return NULL;
  }
}

char *
check_supp_sec_models(void)
{

  static char bit_string[4];
  short change_flag=0;

  memcpy(bit_string, "0000", sizeof(bit_string));

  /*Leave the "other" bit (bit 0) blank for now*/

#ifdef SR_SNMPv1_PACKET
  bit_string[1] = '1';
  change_flag = 1;
#endif /*SNMPv1*/

#ifdef SR_SNMPv2c_PACKET
  bit_string[2] = '1';
  change_flag = 1;
#endif /*SNMPv2*/

#ifdef SR_USER_SEC_MODEL
  bit_string[3] = '1';
  change_flag = 1;
#endif /*SR_SNMPv3_PACKET*/

  if(change_flag) {
    return(bit_string);
  }
  else {
    return NULL;
  }
}

char *
check_supp_auth_prot(void)
{

  static char bit_string[4];
  short change_flag=0;

  memcpy(bit_string, "0000", sizeof(bit_string));

  /*Leave the "other" bit (bit 0) blank for now*/

#ifdef SR_MD5_HASH_ALGORITHM
  bit_string[1] = '1';
  change_flag = 1;
#endif /*SR_MD5_HASH_ALGORITHM*/

#ifdef SR_SHA_HASH_ALGORITHM
  bit_string[2] = '1';
  change_flag = 1;
#endif /*SR_SHA_HASH_ALGORITHM*/

  if(change_flag) {
    return(bit_string);
  }
  else {
    return NULL;
  }
}

char *
check_supp_priv_prot(void)
{

  static char bit_string[4];
  short change_flag=0;

  memcpy(bit_string, "0000", sizeof(bit_string));

  /*Leave the "other" bit (bit 0) blank for now*/

#ifdef SR_SNMPv3_PACKET
#ifdef SR_USER_SEC_MODEL /* single DES */
  bit_string[1] = '1';
  change_flag = 1;
#endif /*SR_USER_SEC_MODEL*/

#ifdef SR_3DES
  bit_string[2] = '1';
  change_flag = 1;
#endif /*SR_3DES*/

#ifdef SR_AES
  bit_string[3] = '1';
  change_flag = 1;
#endif /*SR_AES*/
#endif /* SR_SNMPv3_PACKET */

  if(change_flag) {
    return(bit_string);
  }
  else {
    return NULL;
  }
}
