/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmd.h
* @purpose     GMRP data base
* @component   GARP
* @comments    none
* @create      03/06/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_GMD_H_
#define INCLUDE_GMD_H_

#include "l7_common.h"

/******************************************************************************
 * GMD : GARP MULTICAST REGISTRATION APPLICATION DATABASE
 ******************************************************************************
 */
typedef struct tagGmd_structure
{
  L7_uchar8     vlanIdmacAddress[2+L7_MAC_ADDR_LEN];    /* attribute Value Mac Address for GMRP */
  void*          next;
} Gmd_structure;
/*typedef L7_uchar8 gmrp_index_t[8];*/
typedef struct
{
  L7_uchar8     vlanIdmacAddress[2+L7_MAC_ADDR_LEN];    /* attribute Value Mac Address for GMRP */
}gmrp_index_t;
typedef struct
{
 gmrp_index_t   *first;
 L7_uint32       numberOfEntries;
}gmrp_gmd_t;
/*********************************************************************
* @purpose  creates DB
*
* @PARAM    L7_uint32         MAX VLANS
* @param    void *            pointer to DB
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  Creates a new instance of gmd, allocating space for up to max_vlans
*         VLAN IDs.
*
*         Returns True if the creation suceeded together with a pointer to the
*         gvd information.
*       
* @end
*********************************************************************/
extern L7_BOOL gmd_create_gmd(void **gmd);

/*********************************************************************
* @purpose  destroy DB
*
* @param    void *            pointer to DB
*
* @returns  None
*
* @notes   Destroys the instance of gvd, releasing previously allocated database and
*          control space.
*       
* @end
*********************************************************************/
extern void gmd_destroy_gmd(void *gmd);

/*********************************************************************
* @purpose  find DB entry 
*
* @param    void *            pointer to DB
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
extern L7_BOOL gmd_find_entry( void *my_gmd, L7_uchar8 *key,
                               L7_uint32 *found_at_index);

/*********************************************************************
* @purpose  create DB entry 
*
* @param    void *            pointer to DB
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
extern L7_BOOL gmd_create_entry(void *my_gmd, L7_uchar8 *key,
                                L7_uint32 *created_at_index);

/*********************************************************************
* @purpose  delete DB entry 
*
* @param    void *            pointer to DB
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL gmd_delete_entry(void *my_gmd,
                                L7_uint32  delete_at_index);

/*********************************************************************
* @purpose  get the 6 byte MAC addr 
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
extern L7_BOOL gmd_get_key(void *my_gmd, L7_uint32 index, L7_uchar8 *key);

#endif /* gmd_h__ */


