#ifndef _PTIN_DHCP_DATABASE__H
#define _PTIN_DHCP_DATABASE__H

#include "dtlinclude.h"
#include "ptin_dhcp_database_api.h"


/*********************************************************************
 * Initialize the DHCP database
 * Returns:
 *  L7_SUCCESS or L7_FAILURE
 *********************************************************************/
void ptin_dhcp_database_init(void);


/**
 * Update circuit id for all database entries
 * 
 * @param access_node_id_arg : New access node identifier 
 * @param l2_type_arg : New L2 type 
 * @param slot_field_arg : New slot field 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_dhcp_database_accessNodeId_update(L7_char8 *access_node_id_arg, L7_uchar8 *l2_type_arg, L7_uchar8 *slot_field_arg);

/*********************************************************************
 * Configure an entry of the DHCP database
 * Input:
 *  intIfNum:      interface Id
 *  vlanId:        Outer vlanId
 *  innerVloanId:  Inner vlanId
 *  circuitId:     String with circuitId reference
 *  remoteId:      String with remoteId reference
 * Returns:
 *  L7_SUCCESS or L7_FAILURE
 *********************************************************************/
L7_RC_t ptin_dhcp_database_config(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId, L7_char8 *circuitId, L7_char8 *remoteId);

/*********************************************************************
 * Remove an entry of the DHCP database
 * Input:
 *  intIfNum:      interface Id
 *  vlanId:        Outer vlanId
 *  innerVlanId:   Inner vlanId
 * Returns:
 *  L7_SUCCESS or L7_FAILURE
 *********************************************************************/
L7_RC_t ptin_dhcp_database_remove(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId);

#endif // _PTIN_SNOOP_STATS__H

