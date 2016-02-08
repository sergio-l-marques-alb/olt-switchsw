/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmmrt.h
*
* @purpose  MTR table implementation specifices.
*
* @component 
*
* @comments 
*
* @create 01/01/2006
*
* @author  dsatyanarayana
* @end
*
******************************************************************************/
#ifndef _PIMSM_MRT_H_
#define _PIMSM_MRT_H_

#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
/*#include "pimsmmain.h"*/

struct pimsmCB_s;
typedef enum
{
  PIMSM_ENTRY_TYPE_NONE = 0,
  PIMSM_ENTRY_TYPE_S_G,
  PIMSM_ENTRY_TYPE_S_G_RPT,
  PIMSM_ENTRY_TYPE_STAR_G,
  PIMSM_ENTRY_TYPE_STAR_STAR_RP
} pimsmEntryType_t;


typedef struct pimsmLongestMatch_s
{
  pimsmEntryType_t entryType;
  void *mrtNode;
}pimsmLongestMatch_t;


extern L7_RC_t pimsmMRTFind(struct pimsmCB_s *pimsmCb,
                     L7_inet_addr_t *pGrpAddr,
                     L7_inet_addr_t *pSrcAddr,
                     pimsmLongestMatch_t *pMRTNode);

extern void pimsmMRTNodeCleanup(pimsmCB_t * pimsmCb);
#endif /* _PIMSM_MRT_H_ */


