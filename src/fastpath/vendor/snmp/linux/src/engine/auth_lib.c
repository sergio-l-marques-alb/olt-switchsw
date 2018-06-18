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

#include <string.h>


#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "snmpv3/v3type.h"
#include "sr_msg.h"
#include "v1_msg.h"


static int auth_lib_snmp_lcd_initialized = 0;
SnmpLcd auth_lib_snmp_lcd;

static void initialize_auth_lib_snmp_lcd(void);

static void
initialize_auth_lib_snmp_lcd()
{
    SR_INT32 i;

    if (auth_lib_snmp_lcd_initialized) {
        return;
    }

    i = 0;

#ifdef SR_SNMPv1_PACKET
    auth_lib_snmp_lcd.versions_supported[i] = SR_SNMPv1_VERSION;
    auth_lib_snmp_lcd.lcds[i] = NULL;
    auth_lib_snmp_lcd.parse_functions[i] = SrParseV1SnmpMessage;
    auth_lib_snmp_lcd.build_functions[i] = SrBuildV1SnmpMessage;
    i++;
#endif /* SR_SNMPv1_PACKET */

#ifdef SR_SNMPv2c_PACKET
    auth_lib_snmp_lcd.versions_supported[i] = SR_SNMPv2c_VERSION;
    auth_lib_snmp_lcd.lcds[i] = NULL;
    auth_lib_snmp_lcd.parse_functions[i] = SrParseV1SnmpMessage;
    auth_lib_snmp_lcd.build_functions[i] = SrBuildV1SnmpMessage;
    i++;
#endif /* SR_SNMPv2c_PACKET */

    auth_lib_snmp_lcd.num_vers = i;
}

/*
 * Authentication is now an ASN.1 entity, so make it here.
 */

/*
 * MakeAuthentication: This routine is used to create a library format
 *                     authentication header data structure for use by
 *                     BuildAuthentication().  This particular
 *                     implementation of the library creates an
 *                     authentication header based on the 'trivial'
 *                     authentication put forth by RFC1155-56-57, which is
 *                     a community octet string ususally based on text.
 *                     The header and the octet string associated with the
 *                     header are free'ed when FreeAuthentication() is
 *                     called with the authentication pointer.
 * 
 * Arguments: community - the community string to be put in the newly
 *                        malloc'ed AuthHeader structure which is returned
 *                        by this routine.
 *
 * Returns: The newly created AuthHeader structure on success, 
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_authentication().
 */
AuthHeader     *
MakeAuthentication(community)
    OctetString    *community;
{
    AuthHeader     *auth_ptr;

    if ((auth_ptr = (AuthHeader *) malloc(sizeof(AuthHeader))) == NULL) {
	DPRINTF((APWARN, "MakeAuthentication, auth_ptr malloc\n"));
	return (NULL);
    }
    /* NULL out the structure */
    SR_CLEAR(auth_ptr);

#ifdef	SR_SNMPv1_PACKET
    auth_ptr->version = SR_SNMPv1_VERSION;
#endif				/* SR_SNMPv1_PACKET */

    auth_ptr->community = community;

    return (auth_ptr);
}				/* end of MakeAuthentication() */

/*
 * BuildAuthentication:
 *    This routine takes the Pdu (pdu_ptr) and the
 *    authentication information (auth_ptr) and builds the
 *    actual SNMP packet in the authentication data
 *    structure's *packlet octet string area.
 *
 *    If this is a SNMPv1 packet, it uses the procedures
 *    described in RFC1155-56-57.  If this is a SNMPv2
 *    packet, it uses the procedures described in
 *    RFC1445-46-47.
 *
 *    The 'packlet' is free'ed when FreeAuthentication()
 *    is called, so it should be copied to a holding area
 *    or the authentication should not be free'ed until the
 *    packet is actually sent. Once this has been done, the
 *    authentication structure (auth_ptr) can be free'ed
 *    with a call to FreeAuthentication().
 *
 * Arguments:
 *    auth_ptr - The packlet field of this structure will contain the
 *               message to be sent out when this routine completes
 *               successfully in the packlet field of the structure.
 *               The packlet field should contain the serialized SNMP
 *               Pdu, which is stored in the incoming parameter,
 *               pdu_ptr, and it should contain the serialized
 *               information for the SNMP wrapper.  Other elements of
 *               auth_ptr are filled in in this routine.
 *
 *    pdu_ptr  - Contains the serialized SNMP packet to be sent.
 *
 * Returns:
 *     0 - on success
 *    -1 - on failure.
 *
 */

int
BuildAuthentication(auth_ptr, pdu_ptr)
    AuthHeader       *auth_ptr;
    Pdu              *pdu_ptr;
{
    FNAME("BuildAuthentication")
    SnmpMessage *snmp_msg = NULL;

    if (auth_ptr == NULL) {
        DPRINTF((APPACKET, "%s: NO AUTH.\n", Fname));
        return -1;
    }
    if (pdu_ptr == NULL) {
        DPRINTF((APPACKET, "%s: NO PDU.\n", Fname));
        return -1;
    }

    initialize_auth_lib_snmp_lcd();

    switch (auth_ptr->version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            snmp_msg = SrCreateV1SnmpMessage(auth_ptr->community, TRUE);
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            snmp_msg = SrCreateV2cSnmpMessage(auth_ptr->community, TRUE);
            break;
#endif /* SR_SNMPv2c_PACKET */
    }

    if (snmp_msg == NULL) {
        return -1;
    }

    if (SrBuildSnmpMessage(snmp_msg, pdu_ptr, &auth_lib_snmp_lcd)) {
        SrFreeSnmpMessage(snmp_msg);
        return -1;
    }

    FreeOctetString(auth_ptr->packlet);
    auth_ptr->packlet = snmp_msg->packlet;
    snmp_msg->packlet = NULL;
    SrFreeSnmpMessage(snmp_msg);
    return 0;
}				/* BuildAuthentication() */

/*
 * FreeAuthentication: If compiled SNMPv1 only, this routine frees all memory
 *                     associated with a TRIVIAL_AUTHENTICATION header data
 *                     structure, including the actual SNMP packet that
 *                     BuildAuthentication() creates and the octet string
 *                     associated with MakeAuthentication(). The Pdu structure
 *                     pointed to by the pdu_ptr passed to MakeAuthentication()
 *                     is NOT touched.
 *
 *                     If compiled with SNMPv2, this routine frees all memory
 *                     associated with a SNMPv2 authentication header data
 *                     structure, including the actual packet that
 *                     BuildAuthentication() creates. The Pdu structure pointed
 *                     to by the pdu_ptr passed to MakeAuthentication() is NOT
 *                     touched.
 *
 * Arguments: auth_ptr - the AuthHeader structure that needs to be freed.
 *
 * Returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was free_authentication().
 */

void
FreeAuthentication(auth_ptr)
    AuthHeader     *auth_ptr;
{
    if (auth_ptr != NULL) {

#ifdef SR_SNMPv1_WRAPPER
	FreeOctetString(auth_ptr->community);
#endif				/* SR_SNMPv1_WRAPPER */

	FreeOctetString(auth_ptr->packlet);
	free((char *) auth_ptr);
    }
}

/*
 * ParseAuthentication:
 *    This routine is used to create a library format
 *    authentication header data structure from an incoming
 *    SNMP packet. If parsing errors occur, a message is
 *    printed if DEBUG is defined, the error_code parameter
 *    is set to the correct error code and the routine
 *    returns  NULL.
 *
 *    It should be noted that the state of the authentication
 *    header created during the building phase after a call
 *    to BuildAuthentication() is nearly identical to the
 *    state of the authentication header after this call on
 *    the parsing side.
 *
 *    If this is a SNMPv1 packet, the procedures for parsing
 *    a packet are described in RFC1155-56-57.  If this is a
 *    SNMPv2 packet, the procedures for parsing a packet are
 *    described in RFC1445-46-47.
 *
 * Arguments:
 *    packet_ptr - Pointer to the incoming serialized message (SNMP packet).
 *    length     - Should be the length of the packet.
 *    error_code - Will contain the specific error that occurred if
 *                 this routine returns NULL.
 *
 * Returns:
 *    On success: A pointer to the filled in AuthHeader structure. All
 *                of the authentication information should be parsed at this
 *                point.  A call to ParsePdu() will be needed to retrieve the
 *                values in the SNMP Pdu.
 *    On failure: Returns NULL.
 */

AuthHeader     *
ParseAuthentication(packet_ptr, length, errorCode)
    const unsigned char *packet_ptr;
    SR_INT32          length;
    int              *errorCode;
{
    FNAME("ParseAuthentication")
    AuthHeader       *auth_ptr = NULL;
    SnmpMessage      *snmp_msg = NULL;

    initialize_auth_lib_snmp_lcd();

    *errorCode = 0;

    auth_ptr = (AuthHeader *) malloc((size_t) sizeof(AuthHeader));
    if (auth_ptr == NULL) {
        DPRINTF((APWARN, "%s: auth_ptr malloc\n", Fname));
        goto fail;
    }
    SR_CLEAR(auth_ptr);

    snmp_msg = SrParseSnmpMessage(&auth_lib_snmp_lcd,
                                  NULL,
                                  NULL,
                                  packet_ptr,
                                  (const int)length);
                                  
    if (snmp_msg == NULL) {
        goto fail;
    }
    auth_ptr->version = snmp_msg->version;
    auth_ptr->packlet = snmp_msg->packlet;
    *errorCode = snmp_msg->error_code;
    switch (auth_ptr->version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            auth_ptr->community = snmp_msg->u.v1.community;
            break;
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            auth_ptr->community = snmp_msg->u.v1.community;
            break;
#endif /* SR_SNMPv2c_PACKET */
    }

    free(snmp_msg);
    return auth_ptr;

  fail:
    FreeAuthentication(auth_ptr);
    NULLIT(auth_ptr);
    if (*errorCode == 0) {
        *errorCode = ASN_PARSE_ERROR;
    }
    return NULL;
}

Pdu *
ParsePdu(auth_ptr)
    const AuthHeader *auth_ptr;
{
    if (auth_ptr == NULL) {
        return NULL;
    }
    return SrParsePdu(auth_ptr->packlet->octet_ptr, auth_ptr->packlet->length);
}
