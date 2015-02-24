#ifndef _PTIN_L2_H
#define _PTIN_L2_H

#include "datatypes.h"
#include "ptin_structs.h"

/**
 * Manage L2 earning events
 * 
 * @param macAddr 
 * @param intIfNum 
 * @param virtual_port 
 * @param vlanId 
 * @param msgsType 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_l2_learn_event(L7_uchar8 *macAddr, L7_uint32 intIfNum, L7_uint32 virtual_port,
                            L7_uint32 vlanId, L7_uchar8 msgsType);

/**
 * Get aging time
 * 
 * @param age : aging time in seconds (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_aging_get(L7_uint32 *age);

/**
 * Set aging time
 * 
 * @param age : aging time in seconds
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_aging_set(L7_uint32 age);


/**
 * Load all MAC entries to a RAM table
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_table_load(void);

/**
 * Get MAC entries loaded with ptin_l2_mac_table_load
 * 
 * @param startId : starting entry id
 * @param numEntries : maximum number of entries to be read 
 *                   (input/output)
 * @param entries : pointer to a list of entries (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_table_get( L7_uint32 startId, L7_uint32 *numEntries, ptin_switch_mac_entry **entries );

/**
 * Flush the MAC table entries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_table_flush( void );

/**
 * Remove a MAC entry
 * 
 * @param entry : entry to be removed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_table_entry_remove( ptin_switch_mac_entry *entry );

/**
 * Add a MAC entry
 * 
 * @param entry : entry to be added
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_l2_mac_table_entry_add( ptin_switch_mac_entry *entry );

#endif /* _PTIN_L2_H */
