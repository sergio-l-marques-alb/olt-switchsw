/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_distiller.h
*
* @purpose    DiffServ component Distiller header file
*
* @component  diffserv
*
* @comments   none
*
* @create     05/03/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_DISTILLER_H
#define INCLUDE_DIFFSERV_DISTILLER_H

#include "l7_common.h"
#include "l7_diffserv_api.h" 
#include "diffserv_prvtmib.h"
#include "tlv_api.h"


/* DiffServ Distiller function prototypes */
L7_RC_t dsDistillerInit(void);
L7_RC_t dsDistillerInitPhase1Process(void);
L7_RC_t dsDistillerInitPhase2Process(void);
L7_RC_t dsDistillerInitPhase3Process(void);
void    dsDistillerFiniPhase1Process(void);
void    dsDistillerFiniPhase2Process(void);
void    dsDistillerFiniPhase3Process(void);
L7_RC_t dsDistillerReset(void);
L7_RC_t dsDistillerAdminModeChange(L7_uint32 mode);
L7_RC_t dsDistillerIntfEventNotify(L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t dsDistillerEvaluateAll(void);
L7_RC_t dsDistillerRowEventNotify(dsmibTableId_t tableId, void *pRow, 
                                  L7_uint32 event);
L7_RC_t dsDistillerInstanceKeyGet(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex, 
                                  L7_uint32 *pKey);
L7_BOOL dsDistillerPolicyIsReady(L7_uint32 policyIndex);
L7_BOOL dsDistillerPolicyInstIsReady(L7_uint32 policyIndex, 
                                     L7_uint32 policyInstIndex);
L7_BOOL dsDistillerPolicyInstIsIssued(L7_uint32 policyIndex, 
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 intIfNum);
L7_RC_t dsDistillerPolicyDirGet(L7_uint32 policyIndex,
                                dsmibServiceIfDir_t *pIfDirection);
L7_RC_t dsDistillerPolicyOLLInsert(L7_uint32 policyIndex, L7_uint32 intIfNum);
L7_RC_t dsDistillerPolicyOLLRemove(L7_uint32 policyIndex, L7_uint32 intIfNum);
L7_RC_t dsDistillerPolicyEvaluate(L7_uint32 policyIndex, L7_uint32 mode,
                                  L7_uint32 evalIntIfNum, L7_BOOL reissueTlv);
L7_RC_t dsDistillerPolicyBuildUp(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                 dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDistillerPolicyTearDown(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                  dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDistillerPolicyTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                dsmibServiceIfDir_t ifDirection, L7_tlvHandle_t tlvHandle);
L7_RC_t dsDistillerCtrlShow(void);
L7_RC_t dsDistillerOLLShow(L7_uint32 policyIndex);
/* ...these are from diffserv_distiller_tlv.c */
L7_RC_t dsDstlTlvInit(void);
void    dsDstlTlvFini(void);
L7_RC_t dsDstlTlvPolicyAdd(L7_uint32 policyIndex, L7_uint32 intIfNum,
                           dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDstlTlvPolicyDel(L7_uint32 policyIndex, L7_uint32 intIfNum,
                           dsmibServiceIfDir_t ifDirection);

/* local definitions */
#define DSDSTL_TLV_INST_KEY_NOT_ISSUED  0       /* 0 is never a valid key     */
#define DSMIB_POLICY_EVAL_ALL_INTF      0       /* non-0 means specific intf  */

/* Distiller service interface AVL node definition */
typedef struct
{
  L7_uint32     policyIndex;                    /* KEY: policy index          */
  L7_uint32     ifIndex;                        /* KEY: service ifIndex       */
} dsDstlIntfNodeKey_t;

typedef struct
{
  /* NOTE:  The search keys MUST be the first items in this structure */
  dsDstlIntfNodeKey_t   key;                    /* AVL search keys            */

  /* node contents */
  L7_uint32     intIfNum;                       /* IIN that matches ifIndex   */
  dsmibServiceIfDir_t ifDirection;              /* copy of pol/intf direction */
  L7_uint32     intfLinkRate;                   /* interface link data rate   */
  L7_uint32     tlvIssued[L7_DIFFSERV_INST_PER_POLICY_LIM+1];

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                  *next;                  /* (RESERVED FOR AVL USE)     */

} dsDstlIntfNode_t;            

/* Distiller control data structure */
typedef struct
{
  L7_BOOL       instIsReady[L7_DIFFSERV_INST_PER_POLICY_LIM+1];
  /* instance->key translation table (split into separate row per policy) */
  L7_uint32     inst2KeyTable[L7_DIFFSERV_INST_PER_POLICY_LIM+1]; 
} dsDstlPolicyInfo_t;

typedef struct
{
  L7_uint32     policyIndex;
  L7_uint32     policyInstIndex;
} dsDstlKey2Inst_t;

typedef struct
{
  avlTree_t           *pOLL;                    /* service intf ordered LL    */
  void *              ollSemId;                 /* OLL mutex semaphore        */
  L7_tlvHandle_t      tlvHandle;                /* TLV block handle           */

  /* key->instance translation table */
  dsDstlKey2Inst_t    key2InstTable[L7_DIFFSERV_POLICY_INST_LIM+1];
                                 
  dsDstlPolicyInfo_t  policy[L7_DIFFSERV_POLICY_LIM+1];  /* policy info       */

} dsDstlCtrl_t;

#endif /* INCLUDE_DIFFSERV_DISTILLER_H */
