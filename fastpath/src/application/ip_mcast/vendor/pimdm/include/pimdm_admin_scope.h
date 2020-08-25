/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_admin_scope.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_ADMIN_SCOPE_H
#define _PIMDM_ADMIN_SCOPE_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7sll_api.h"
#include "l3_addrdefs.h"
#include "intf_bitset.h"
#include "pimdm_main.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/*********************************************************************
*               PIM-DM AdminScope Database Node
*********************************************************************/
typedef struct pimdmASBNode_s
{
  L7_sll_member_t    *next;
  L7_inet_addr_t     grpAddr;  /* Group Address */
  L7_inet_addr_t     grpMask;  /* Group Address */
  interface_bitset_t intfMask; /* Interface Mask */
}pimdmASBNode_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmAdminScopeBoundarySet (pimdmCB_t *pimdmCB,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *grpMask,
                            L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmAdminScopeBoundaryReset (pimdmCB_t *pimdmCB,
                              L7_inet_addr_t *grpAddr,
                              L7_inet_addr_t *grpMask,
                              L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmAdminScopeIntfDownUpdate (pimdmCB_t *pimdmCB,
                              L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmAdminScopeIntfBitSetGet (pimdmCB_t *pimdmCB,
                              L7_inet_addr_t *grpAddr,
                              L7_uint32      rtrIfNum,
                              interface_bitset_t *oList);

extern void
pimdmAdminScopeBoundaryInfoShow (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmAdminScopeBoundaryInit (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmAdminScopeBoundaryDeInit (pimdmCB_t *pimdmCB);

#endif  /* _PIMDM_ADMIN_SCOPE_H */
