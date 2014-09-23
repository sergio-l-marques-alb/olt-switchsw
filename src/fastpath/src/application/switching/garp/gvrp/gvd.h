/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gvd.h
* @purpose     GVRP data base
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef INCLUDE_GVD_H_
#define INCLUDE_GVD_H_

#include <l7_common.h>

/******************************************************************************
 * GVD : GARP VLAN DATABASE
 ******************************************************************************
 *
 * The GARP VLAN Database maps VLAN IDs into compact GID indexes and
 * vice versa. It contains VLAN ID to index mappings for all the VLAN IDs
 * dynamically registered (except when there is a database overflow which
 * should be an event that is kept exceedingly rare through appropriate
 * sizing) and for all those for which static controls exist.
 *
 * Taken together with the GID machines for each port (which are identified
 * by the GID indexes provided by GVD), GVD logically provides the Static VLAN
 * Entries and the VLAN Registration Entries of the abstract Filtering Database
 * (P802.1Q, Clause 3.9).
 *
 * Static VLAN Entries are included in this database (and have GID machines
 * defined) on an as needed basis as ports are added to the GVR Application.
 * This example implementation assumes that the necessary information is taken
 * from Static Filtering Entries kept in a Permanent Database outside the
 * example implementation. Static VLAN Entries can also be added, changed, or
 * removed as the running system is managed.
 *
 * VLAN Registration Entries are added and removed by GVRP. Note that a
 * single VLAN ID will only give rise to one entry in this database, and one
 * GID machine per port. That machine provides the functionality for both
 * the Static VLAN Entry and the VLAN Registration Entry.
 */

#define GVRP_KEY_SIZE 2     /* size of Vlan Id is 2 bytes*/
 
typedef struct tagGvd_structure
{
	Vlan_id       attribute_Value; 		/* attribute Value Vlan_Id for GVRP */
/*    L7_uint32     states[L7_MAX_INTERFACE_COUNT] ;*/	/* state of each attribute */
    struct tagGvd_structure *next;
} Gvd_structure;

 
/*********************************************************************
* @purpose  destroy DB
*
* @param    void *            pointer to DB
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  Creates a new instance of gvd, allocating space for up to max_vlans
*         VLAN IDs.
*
*         Returns True if the creation suceeded together with a pointer to the
*         gvd information.
*       
* @end
*********************************************************************/
extern L7_BOOL gvd_create_gvd(void **gvd);

/*********************************************************************
* @purpose  destroy DB
*
* @param    none
*
* @returns  None
*
* @notes   Destroys the instance of gvd, releasing previously allocated database and
*          control space.
*       
* @end
*********************************************************************/
extern void gvd_destroy_gvd(void);

/*********************************************************************
* @purpose  find DB entry 
*
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvd_find_entry(Vlan_id key, L7_uint32 *found_at_index);

/*********************************************************************
* @purpose  create DB entry 
*
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvd_create_entry(Vlan_id key, L7_uint32 *created_at_index);

/*********************************************************************
* @purpose  delete DB entry 
*
* @param    delete_at_index    index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvd_delete_entry(L7_uint32 delete_at_index);

/*********************************************************************
* @purpose  get vlan id 
*
* @param    L7_uint32         index
* @param    Vlan_id           key
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gvd_get_key(L7_uint32 index, Vlan_id *key);

#endif /* gvd_h__ */


