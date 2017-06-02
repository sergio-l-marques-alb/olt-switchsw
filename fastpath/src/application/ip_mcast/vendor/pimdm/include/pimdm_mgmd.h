/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_mgmd.h
*
* @purpose    MGMD Module interactions to handle Local Memberships
*
* @component  PIM-DM
*
* @comments
*
* @create     07/12/2006
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_MGMD_H
#define _PIMDM_MGMD_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7_mgmd_api.h"
#include "pimdm_main.h"
#include "pimdm_mrt.h"

/*******************************************************************************
**                       General Definitions                                  **
*******************************************************************************/
/* None */

/*******************************************************************************
**                    Function Prototype Declarations                         **
*******************************************************************************/
/* Notes on pimdmLocalMemb_t :
   -------------------------
   1. Each <S,G> and <*,G> in the system is represented by one entry of this kind.
   2. For ease of searches based on groups, the entries are sorted first on G then on S.
   3. A <*,G> is represented as <G,0>. Since 0 is an invalid value for S, with this
      representation, <*,G> and <S,G> entries can easily co-exist on the same storage
      without any clashes.
   4. Each bit in the incl[] bitmap represents a router interface for which the
      associated <S,G> or <*,G> is in include mode.
   5. Each bit in the excl[] bitmap represents a router interface for which the
      associated <S,G> or <*,G> is in exclude mode. */
typedef struct pimdmLocalMemb_s
{
  L7_inet_addr_t group;
  L7_inet_addr_t source;
  interface_bitset_t incl;
  interface_bitset_t excl;
  void *avlData; /* AVL TREE requires this as last */
} pimdmLocalMemb_t;

extern L7_RC_t
pimdmMgmdInit (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmMgmdDeInit (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmMgmdMsgProcess (pimdmCB_t       *pimdmCB,
                     L7_uint32       mgmdEventType,
                     mgmdMrpEventInfo_t *mgmdUpdateEntry);
extern L7_BOOL
pimdmMgmdStarGInclCompute(pimdmCB_t      *pimdmCB,
                          L7_inet_addr_t *pGroup,
                          interface_bitset_t *pBitmap);
extern L7_BOOL
pimdmMgmdSGInclCompute(pimdmCB_t      *pimdmCB,
                       L7_inet_addr_t *pSource,
                       L7_inet_addr_t *pGroup,
                       interface_bitset_t *pBitmap);
extern L7_BOOL
pimdmMgmdSGExclCompute(pimdmCB_t      *pimdmCB,
                       L7_inet_addr_t *pSource,
                       L7_inet_addr_t *pGroup,
                       interface_bitset_t *pBitmap);
extern void
pimdmMgmdShow(pimdmCB_t *pimdmCB,L7_int32 count);

extern pimdmLocalMemb_t*
pimdmMgmdEntryNextGet (pimdmCB_t *pimdmCB,
                      L7_inet_addr_t *srcAddr,
                      L7_inet_addr_t *grpAddr);
extern L7_BOOL
pimdmMgmdMembershipSGInclDelete(pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pGroup,
                                L7_inet_addr_t *pSource,
                                L7_uint32      rtrIfNum);
extern L7_BOOL
pimdmMgmdMembershipSGExclDelete(pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pGroup,
                                L7_inet_addr_t *pSource,
                                L7_uint32      rtrIfNum);
extern L7_BOOL
pimdmMgmdMembershipStarGDelete(pimdmCB_t      *pimdmCB,
                                L7_inet_addr_t *pGroup,
                                L7_uint32      rtrIfNum);

extern L7_RC_t
pimdmMgmdIntfGroupInfoGet (pimdmCB_t *pimdmCB,
                           L7_uint32 rtrIfNum);

extern L7_RC_t
pimdmMgmdIntfDownUpdate (pimdmCB_t *pimdmCB,
                         L7_uint32 rtrIfNum);

#endif /* _PIMDM_MGMD_H */
