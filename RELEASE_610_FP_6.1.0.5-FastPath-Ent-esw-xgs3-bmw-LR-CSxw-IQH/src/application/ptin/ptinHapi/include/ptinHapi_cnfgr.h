/*
 * ptinHapi_cnfgr.h
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 */

#ifndef PTINHAPI_CNFGR_H_
#define PTINHAPI_CNFGR_H_

#include "dtl_ptin.h"

extern L7_int    linkStatus[L7_SYSTEM_N_INTERF];
extern L7_int    lagActiveMembers[L7_SYSTEM_N_PORTS];

#define PTIN_L2_QUEUE "PTIN_L2-Q "

typedef struct ptinL2Msg
{
  L7_uchar8 msgsType;                    /* add or del              */
  L7_uchar8 entryType;                   /* fdb_entry_type_t        */
  L7_uchar8 mac_addr[L7_MAC_ADDR_LEN];   /* mac address             */
  L7_ushort16 intIfNum;                  /* port interface number   */
  L7_ushort16 vlanId;                    /* Vlan ID                 */
} ptinL2Msg_t;

extern void *ptin_module_l2_sem;

extern void *ptinL2Queue;

L7_RC_t ptinHapiCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinHapiCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinHapiCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason );


void    ptinHapiCnfgrFiniPhase1Process();
void    ptinHapiCnfgrFiniPhase2Process();
void    ptinHapiCnfgrFiniPhase3Process();
L7_RC_t ptinHapiCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t ptinHapiCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t ptinHapiCfgDump();

#endif /* PTINHAPI_CNFGR_H_ */
