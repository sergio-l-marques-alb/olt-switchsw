#ifndef _PTIN_DHCP_DATABASE_API__H
#define _PTIN_DHCP_DATABASE_API__H

#include "datatypes.h"

/*********************************************************************
 * Read an entry of the DHCP database
 * Input:
 *  intIfNum:      interface Id
 *  vlanId:        Outer vlanId
 *  innerVloanId:  Inner vlanId
 * Output:
 *  circuitId:     String with circuitId reference
 *  remoteId:      String with remoteId reference
 * Returns:
 *  L7_SUCCESS or L7_FAILURE
 *********************************************************************/
L7_RC_t ptin_dhcp_database_get(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId, L7_char8 *circuitId, L7_char8 *remoteId);

/**
 * Extract the PON interface from the circuit Id. 
 * 
 * @author mruas (11/13/2012)
 * 
 * @param circuitId : circuit Id string
 * @param len : maximum length
 * @param dstPort : destination port to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_dhcp_database_dstPort_get(L7_uchar8 *circuitId, L7_int len, L7_int *dstPort);

#endif
