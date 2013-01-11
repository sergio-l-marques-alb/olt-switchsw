/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmrp.h
*
* @purpose Contains structs related to all Cand-RP/Static RP/Grp-RP mapping .
*
* @component pimsm
*
* @comments 
*
* @create 04/28/2006
*
* @author Ramu
* @end
*
**********************************************************************/
#ifndef _PIMSM_RP_H_
#define _PIMSM_RP_H_


#include "l3_addrdefs.h"
#include "l7sll_api.h"
#include "l7apptimer_api.h"
#include "radix_api.h"
#include "l7handle_api.h"
#include "pimsmdefs.h"

typedef enum
{
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_NONE = 0,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_CONFIG,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_AUTORP,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_EMBEDDED,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_OTHER,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_FIXED_OVERRIDE,
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_MAX
}PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE;



typedef struct pimsmCandRpGrpAdvList_s
{
  L7_inet_addr_t                      grpAddr;
  L7_uchar8                           grpMaskLen;
}pimsmCandRpGrpAdvList_t;

struct pimsmRpBlock_s;

typedef struct pimsmAddrList_s
{
  L7_sll_member_t                     *next;  /*Link to the next entry */
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE   pimsmOrigin; /*origin Type  - Both ipAddr &origin are index for this */
  L7_inet_addr_t                      pimsmIpAddr; /*RP or Group  address */
  L7_uchar8          pimsmMaskLen;     /* used by group-rp mapping */
  L7_APP_TMR_HNDL_t  pimsmRpGrpExpiryTimer;  /* used by the rp-grp mapping */
  L7_uint32          pimsmAddrListPoolId;  
  L7_BOOL            pimsmOriginFlag;  /* used when originating BSMs,if there is RP with more than one origin */
  void              *pimsmRpGrpNode; /* this is used as pointer to rp details for the grp-rp mapping to get rp details 
                                 this is to point to the bsr when candidate rp advertisement needs to be done */    
  L7_uint32          pimsmRpGrpExpireTimerHandle; /* This is used to keep track of not deleting 
                                               the node when giving this pointer to timeout functions */
  pimsmCB_t     *pimsmCb;  /* pointer to back track*/
}pimsmAddrList_t;


/*This structure is used to store the RP information -it also contains the rp-grp mapping*/
typedef struct pimsmRpGrpNode_s
{
  L7_sll_member_t       *next;/*Link to the next entry */
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE    pimsmOrigin;  /*index on origin & rp addr */  
  L7_inet_addr_t        pimsmRpAddr;/*Rp Address */
  L7_uint32             pimsmRpPriority;/*Priority of the RP */
  L7_ushort16           pimsmRpHoldTime;/*The time in seconds the advertisement is valid */
  L7_uint32             pimsmRpHashMaskLen;/*configured hash mask Len */
  L7_uint32             pimsmHashValue;  /*hash Value */
  L7_sll_t              pimsmGrpList; /*List of group addresses - pimsmAddrList_t */

  /* to have back track have the pool id */
  struct pimsmCB_s      *pimsmCb;  /* pointer to back track for deletion */
}pimsmRpGrpNode_t;


/* structure to hold the radix key type */
typedef struct rpSetTreeKey_s
{
  /* packed len byte + addr */
  L7_uchar8     key[1+ sizeof(L7_inet_addr_t)];
} rpSetTreeKey_t;

typedef struct rpSetRadixData_s
{
  struct l7_radix_node nodes[2]; /* Must be the first element in the structure */
  /* ipaddr (key) and mask must follow */
  rpSetTreeKey_t      pimsmGrpKey;  /* only the signifcant portion is stored */
  rpSetTreeKey_t      pimsmMaskKey;  /* Destination network mask. */      
} rpSetRadixData_t;



/*This structure is used to store the RP set information */
typedef struct pimsmRpSetNode_s
{
  rpSetRadixData_t                    pimsmRpSetRadixKey;  /* contains radix specific implemtation */
  L7_inet_addr_t                      pimsmGrpAddr;    /* Indexed by Group Address and mask len*/
  L7_uchar8                           pimsmGrpMaskLen;  /*mask length */
  L7_sll_t                            pimsmRpList;    /*List of RP addresses - pimsmAddrList_t */
  pimsmRpGrpNode_t                   *pimsmPrefRpGrpNode;    /*pointer to the preferres RP */
  void *next;  /* Need this for radix */
}pimsmRpSetNode_t;

typedef struct pimsmCandRpGrpNode_s
{
  L7_inet_addr_t grpAddr;
  L7_uint32      grpMask;
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin;
}pimsmCandRpGrpNode_t;


typedef struct pimsmCandRpInfo_s
{
  L7_inet_addr_t        pimsmCandRpAddr;/*Rp Address */
  L7_uint32             pimsmCandRpRtrIfNum; /* nterface on which cand-Rp is configured */
  L7_uint32             pimsmCandRpPriority;/*Priority of the RP */
  L7_ushort16           pimsmCandRpHoldTime;/*The time in seconds the advertisement is valid */
  L7_uint32             pimsmCandRpHashMaskLen;/*configured hash mask Len */
  L7_APP_TMR_HNDL_t     pimsmCandRpAdvTimer;   /* C-RP Advertisement Timer */  
  pimsmCandRpGrpNode_t  pimsmCandRpGrpNode[PIMSM_CANDIDATE_RP_GRP_MAX];
}pimsmCandRpInfo_t;

typedef struct pimsmRpBlock_s
{
  L7_sll_t               pimsmRpGrpList;   /* List to RP-GRP mapping - pimsm_grp_rp_node_t */
  radixTree_t            pimsmRpSetTree;        /* List to RP set Grp-Rp mapping - pimsmRpSetNode_t */

  /* some of the house keeping structures to hold the data */
  L7_uchar8              *pimsmRpSetTreeHeap;
  pimsmRpSetNode_t    *pimsmRpSetDataHeap;

  pimsmCandRpInfo_t       pimsmCandRpInfo;
}pimsmRpBlock_t;

/*This structure is an interface structure to the RP component */
typedef struct pimsm_crp_config_info_s
{
  L7_inet_addr_t    pimsmRpAddr;/*RP address */
  L7_inet_addr_t    pimsmGrpAddr;/*Group Address */
  L7_uchar8         pimsmGrpMaskLen;/*Mask Len */
  L7_uint32         pimsmRpPriority;/*proirity of the RP */
  L7_uint32         pimsmRpHashMaskLen;/*configured hash mask Len */
  L7_ushort16       pimsmRpHoldTime;
  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE          pimsmOrigin;/*Static RP/C-RP/info from BSR */
  L7_uint32         pimsmRpRtrIfNum; /* interface on which cand-Rp is configured */
}pimsmCandRpConfigInfo_t;


extern 
L7_RC_t pimsmRpInit(pimsmCB_t  *pimsmCb);

extern
L7_RC_t pimsmRpDeInit(pimsmCB_t  *pimsmCb);

extern
L7_RC_t pimsmRpGrpListGrpNodeFind(pimsmCB_t         *pimsmCb,
                                  pimsmRpGrpNode_t  *pRpGrpNode,
                                  L7_inet_addr_t     grpAddr,
                                  L7_uchar8          grpMaskLen,
                                  PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE  origin,
                                  pimsmAddrList_t  **ppAddrListNode);
extern
L7_RC_t pimsmRpSetTreeGrpNodeFind(pimsmCB_t         *pimsmCb,
                                  L7_inet_addr_t     grpAddr,
                                  L7_inet_addr_t     grpMask,
                                  pimsmRpSetNode_t **ppRpSetNode);

extern
L7_RC_t pimsmRpGrpMappingAdd(pimsmCB_t *pimsmCb,
                             pimsmCandRpConfigInfo_t *pRpConfigInfo);

extern
L7_RC_t pimsmRpGrpMappingDelete(pimsmCB_t  *pimsmCb,
                                pimsmCandRpConfigInfo_t *pRpConfigInfo);

extern
L7_RC_t pimsmRpCandRpAdvEnable(pimsmCB_t *pimsmCb);

extern
L7_RC_t pimsmRpCandidateInfoAdd(pimsmCB_t *pimsmCb,
                                pimsmCandRpConfigInfo_t *pRpConfigInfo);

extern
L7_RC_t pimsmRpCandidateInfoDelete(pimsmCB_t *pimsmCb,
                                   pimsmCandRpConfigInfo_t *pRpConfigInfo);


extern
void pimsmRprGrpMappingExpireTimerSet(pimsmCB_t     *pimsmCb,
                                      L7_inet_addr_t grpAddr,
                                      L7_uchar8      grpMaskLen,
                                      L7_inet_addr_t rpAddr,
                                      PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                                      L7_uint32      timeout);

extern
L7_RC_t pimsmRpGrpMappingUpdate(pimsmCB_t           *pimsmCb,
                                L7_inet_addr_t       pGrpAddr,
                                L7_inet_addr_t       grpMask,
                                L7_inet_addr_t       rpAddr,
                                PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                                L7_uint32            rpPriority,
                                L7_uint32            rpHoldtime);

extern
L7_RC_t pimsmRpGrpMappingPreferredGet(pimsmCB_t         *pimsmCb,
                                      L7_inet_addr_t     grpAddr,
                                      pimsmRpGrpNode_t **ppRpGrpNode);

extern
L7_RC_t pimsmRpAddressGet(pimsmCB_t * pimsmCb, 
                          L7_inet_addr_t *pGrpAddr,
                          L7_inet_addr_t *pRpAddr);

extern
L7_RC_t pimsmRpGrpNodeFind(pimsmCB_t        *pimsmCb,
                           L7_inet_addr_t    grpAddr,
                           L7_inet_addr_t    grpMask,
                           L7_inet_addr_t    rpAddr,
                           PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE   origin,
                           pimsmRpGrpNode_t **ppRpGrpNode);

extern
L7_RC_t pimsmRpSetNodeFind(pimsmCB_t         *pimsmCb,
                           L7_inet_addr_t     grpAddr,
                           L7_inet_addr_t     grpMask,
                           pimsmRpSetNode_t **ppRpSetNode);

extern
L7_RC_t pimsmRpSetNodeNextFind(pimsmCB_t         *pimsmCb,
                               L7_inet_addr_t     grpAddr,
                               L7_inet_addr_t     grpMask,
                               pimsmRpSetNode_t **ppRpSetNode);

extern
L7_RC_t pimsmRpGrpNodeNextFind(pimsmCB_t         *pimsmCb,
                               pimsmRpSetNode_t  *pRpSetNode,
                               L7_inet_addr_t     rpAddr,
                               PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE origin,
                               pimsmRpGrpNode_t **ppRpGrpNode);

extern
void pimsmRpIntfDown(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum);	

extern
void pimsmRpGrpMappingPurge(pimsmCB_t *pimsmCb);


extern L7_BOOL pimsmIAmRP(pimsmCB_t * pimsmCb, 
                          L7_inet_addr_t *pGrpAddr);
#endif /* _PIMSMRP_H_ */

