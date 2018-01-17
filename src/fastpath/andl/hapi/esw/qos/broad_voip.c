/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: broad_voip.c
*
* Purpose: This file contains all the routines for Auto-voip feature
*
* Component: hapi
*
* Comments:
*
* Created by: Murali Krishna Peddireddy  4/07/07
*
*
* Application will 
*********************************************************************/

#include "bcmx/cosq.h"
#include "bcmx/port.h"

#include "dapi.h"
#include "broad_policy_types.h"
#include "broad_common.h"
#include "broad_qos_common.h"
#include "broad_voip.h"
#include "dapi_struct.h"

typedef struct
{
  BROAD_POLICY_t           voipPolicy;
  BROAD_POLICY_RULE_t      ruleId[2];
  L7_uint32                srcIp;
  L7_uint32                dstIp;
  L7_ushort16              srcPort;
  L7_ushort16              dstPort;
}HAPI_BROAD_VOIP_t;

HAPI_BROAD_VOIP_t   broadVoipTable[L7_VOIP_MAX_CALLS];

static BROAD_POLICY_t   voipProfilePolicy=BROAD_POLICY_INVALID;

static int count = 0;
/*********************************************************************
*
* @function hapiBroadQosVoIPApplyPolicyToIface
*
* @purpose  This function is called whenever a policy is applied 
*           to an interface. If the port is a LAG then all member 
*           ports are updated
*
* @returns  L7_RC_t
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosVoIPApplyPolicyToIface(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_BOOL addDel)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_RC_t       result = L7_SUCCESS;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (BROAD_PORT_IS_LAG(hapiPortPtr))
  {
    int               i;
    DAPI_LAG_ENTRY_t *lagMemberSet;
    BROAD_PORT_t     *lagMemberPtr;

    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

    /* apply VoIP config to each LAG member */
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (L7_TRUE == lagMemberSet[i].inUse)
      {
        lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
        result = hapiBroadPolicyApplyToIface(voipProfilePolicy, lagMemberPtr->bcmx_lport);
      }
    }

    if (result == L7_SUCCESS)
    {
      count++;
    }
  }
  else
  {
    if (addDel == L7_TRUE)
    {
      result = hapiBroadPolicyApplyToIface(voipProfilePolicy, hapiPortPtr->bcmx_lport);
    }
    else
    {
      if (!BROAD_PORT_IS_ACQUIRED(hapiPortPtr))
      {
        result = hapiBroadPolicyApplyToIface(voipProfilePolicy, hapiPortPtr->bcmx_lport);
        if (result == L7_SUCCESS)
        {
          count++;
        }
      }
      else
      {
        count++;
      }
    }

  }

  return result;
}

/*********************************************************************
*
* @function hapiBroadQosVoIPRemovePolicyFromIface
*
* @purpose  This function is called whenever a policy is removed 
*           from an interface. If the port is a LAG then all member 
*           ports are updated
*
* @returns  L7_RC_t
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosVoIPRemovePolicyFromIface(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_BOOL addDel)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_RC_t       result = L7_SUCCESS;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (BROAD_PORT_IS_LAG(hapiPortPtr))
  {
    int               i;
    DAPI_LAG_ENTRY_t *lagMemberSet;
    BROAD_PORT_t     *lagMemberPtr;
    L7_BOOL           found = L7_FALSE;

    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

    /* Remove VoIP config from each LAG member */
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (L7_TRUE == lagMemberSet[i].inUse)
      {
        lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
        result = hapiBroadPolicyRemoveFromIface(voipProfilePolicy, lagMemberPtr->bcmx_lport);

        if (result == L7_SUCCESS)
        {
          found = L7_TRUE;
        }
        else
        {
          break;
        }
      }
    }
    if ((result == L7_SUCCESS) && (found == L7_TRUE))
    {
      count--;
    }
  }
  else
  {
    if (addDel == L7_TRUE)
    {
      result = hapiBroadPolicyRemoveFromIface(voipProfilePolicy, hapiPortPtr->bcmx_lport);
    }
    else
    {
      if (!BROAD_PORT_IS_ACQUIRED(hapiPortPtr))
      {
        result = hapiBroadPolicyRemoveFromIface(voipProfilePolicy, hapiPortPtr->bcmx_lport);
        if (result == L7_SUCCESS)
        {
          count--;
        }
      }
      else
      {
        count--;
      }
    }
  }

  return result;
}

/*********************************************************************
*
* @function cosq_port_bandwidth_set_all
*
* @purpose  Set Cosq bandwidth for all interfaces
*
* @returns  L7_RC_t
*
* @end
*
*********************************************************************/
static L7_RC_t cosq_port_bandwidth_set_all(DAPI_t *dapi_g, bcmx_lport_t port,
                                           L7_uint32 queueId, L7_uint32 minBw,
                                           L7_uint32 maxBw)
{
  DAPI_USP_t    dapiUsp;
  L7_RC_t       result;
  BROAD_PORT_t  *hapiPortPtr;

  for (dapiUsp.unit=0;dapiUsp.unit<dapi_g->system->totalNumOfUnits;dapiUsp.unit++)
  {
   for (dapiUsp.slot=0;dapiUsp.slot<dapi_g->unit[dapiUsp.unit]->numOfSlots;dapiUsp.slot++)
   {
     if ((dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE) &&
         (IS_SLOT_TYPE_PHYSICAL(&dapiUsp, dapi_g)                       == L7_TRUE))
     {
       /* loop through physical ports */
       for (dapiUsp.port=0;dapiUsp.port<dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->numOfPortsInSlot;dapiUsp.port++)
       {
        
         if (isValidUsp (&dapiUsp, dapi_g) != L7_TRUE)
         {
           continue;
         }  
        hapiPortPtr = HAPI_PORT_GET(&dapiUsp, dapi_g);
        result = bcmx_cosq_port_bandwidth_set(port,queueId,minBw,maxBw,0);
        if (result !=BCM_E_NONE)
        {
           return L7_FAILURE;
        }
      }
     }
    }  
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadQosVoipInit
*
* @purpose  AUTO-VOIP init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*
*********************************************************************/
L7_RC_t    hapiBroadQosVoipInit(DAPI_t *dapi_g)
{
  L7_uint32 i;

  for (i=0;i<L7_VOIP_MAX_CALLS;i++)
  {
    broadVoipTable[i].voipPolicy = BROAD_POLICY_INVALID;
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @function hapiBroadQosVoipPortInit
*
* @purpose  Per-port AUTO-VOIP init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoipPortInit(DAPI_PORT_t *dapiPortPtr)
{
  
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_VOIP_SESSION] = (HAPICTLFUNCPTR_t)hapiBroadQosVoipSession;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_VOIP_PROFILE] = (HAPICTLFUNCPTR_t)hapiBroadQosVoipProfile;
  dapiPortPtr->cmdTable[DAPI_CMD_QOS_VOIP_STATS_GET] = (HAPICTLFUNCPTR_t)hapiBroadQosVoipStatsGet;

  return L7_SUCCESS;
}  

/*********************************************************************
*
* @function hapiBroadVoipPolicyFind
*
* @purpose  To get the free index 
*
* @param    L7_uint32  srcIpAddr - source ip address
* @param    L7_uint32  dstIpAddr - destination ip address
* @param    L7_uint32  srcL4Port - source l4 port
* @param    L7_uint32  dstL4Port - destination l4 port
* @param    L7_uint32 *index     - index of entry in the table
* @param    L7_uint32 *voipId    - policy id
*
*
* @returns  L7_RC_t
*
*
* @end
*
*********************************************************************/

L7_RC_t  hapiBroadVoipPolicyFind(L7_uint32  srcIpAddr,
                                 L7_uint32  dstIpAddr,
                                 L7_uint32  srcL4Port,
                                 L7_uint32  dstL4Port,
                                  L7_uint32 *index,
                                  L7_uint32 *voipId)
{
  L7_uint32 i;
  L7_uint32 freeIndex = L7_VOIP_MAX_CALLS;

  for (i=0; i<L7_VOIP_MAX_CALLS;i++)
  {
    if (broadVoipTable[i].voipPolicy != BROAD_POLICY_INVALID)
    {
        if ((broadVoipTable[i].srcIp == srcIpAddr) &&
            (broadVoipTable[i].dstIp == dstIpAddr) &&
            (broadVoipTable[i].srcPort == srcL4Port) &&
            (broadVoipTable[i].dstPort ==  dstL4Port))
        {   
          *index = i;
          *voipId = broadVoipTable[i].voipPolicy;
           return L7_ALREADY_CONFIGURED; 
        }
    }
    else
  {
      if (freeIndex == L7_VOIP_MAX_CALLS)
      {
        freeIndex = i;
      }
    }
  }
  
  if (freeIndex < L7_VOIP_MAX_CALLS)
  {
    *index = freeIndex;
  return L7_SUCCESS;
}

  return L7_FAILURE;
}

/*********************************************************************
*
* @function hapiBroadVoipPolicyUpdate
*
* @purpose  Update hapi VoIP table with call details
*
* @param    DAPI_QOS_CMD_t *voipCmd   - voipSession 
* @param    BROAD_POLICY_t voipId     - policyid
* @param    BROAD_POLICY_RULE_t rule1 - ruleid
* @param    BROAD_POLICY_RULE_t rule2 - ruleid
* @param    L7_uint32 index           - index
*
* @returns  L7_RC_t   
*
* @notes   
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadVoipPolicyUpdate(DAPI_QOS_CMD_t *voipCmd, 
                                  BROAD_POLICY_t voipId,
                                  BROAD_POLICY_RULE_t rule1,
                                  BROAD_POLICY_RULE_t rule2,
                                  L7_uint32 index)
{

  broadVoipTable[index].srcIp = voipCmd->cmdData.voipSession.srcIpAddr;
  broadVoipTable[index].dstIp = voipCmd->cmdData.voipSession.dstIpAddr;
  broadVoipTable[index].srcPort = voipCmd->cmdData.voipSession.srcL4Port;
  broadVoipTable[index].dstPort = voipCmd->cmdData.voipSession.dstL4Port;
  broadVoipTable[index].voipPolicy = voipId;
  broadVoipTable[index].ruleId[0] = rule1;
  broadVoipTable[index].ruleId[1]= rule2;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadVoipPolicyDelete
*
* @purpose  Delete the call from hapi database 
*
* @param    L7_uint32 index 
*
* @returns  L7_RC_t
*
* @notes   
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadVoipPolicyDelete(L7_uint32 index)
{
  broadVoipTable[index].voipPolicy = BROAD_POLICY_INVALID;

  return L7_SUCCESS;
}

/**********************************************************************
* @function  hapiBroadVoipPolicyRuleCounterAdd
*
* @purpose  
* 
* @param    BROAD_POLCIY_t   policy  - policy 
* @param    BROAD_POLICY_RULE_t rule - rule id  
* @param    DAPI_t *dapi_g  
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*
*********************************************************************/
L7_RC_t    hapiBroadVoipPolicyRuleCounterAdd(BROAD_POLICY_RULE_t rule, 
                                             DAPI_t *dapi_g) 
{
  BROAD_PORT_t              *hapiPortPtr;
  DAPI_USP_t                dapiUsp;
  L7_RC_t                   result;

  for (dapiUsp.unit=0;dapiUsp.unit<dapi_g->system->totalNumOfUnits;dapiUsp.unit++)
  {
    for (dapiUsp.slot=0;dapiUsp.slot<dapi_g->unit[dapiUsp.unit]->numOfSlots;dapiUsp.slot++)
    {
      if ((dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE) &&
          (IS_SLOT_TYPE_PHYSICAL(&dapiUsp, dapi_g)                       == L7_TRUE))
      {
        /* loop through physical ports */
        for (dapiUsp.port=0;
             dapiUsp.port<dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->numOfPortsInSlot;
             dapiUsp.port++)
        {
          if (isValidUsp (&dapiUsp, dapi_g) != L7_TRUE)
          {
            continue;
          }
          hapiPortPtr = HAPI_PORT_GET(&dapiUsp, dapi_g);
          result = hapiBroadPolicyRuleCounterAdd(rule, BROAD_COUNT_PACKETS);
          if (result !=L7_SUCCESS)
          {
             continue;
          }
        }
      }
     } /* end of for (dapiUsp.slot ...*/
   }/* end of for (dapiUsp.unit ......*/
   return  L7_SUCCESS;
}

/**********************************************************************
* @function hapiBroadVoipPolicyApply 
*
* @purpose  
*
* @param    BROAD_POLCIY_t policy    - policy 
* @param    DAPI_t *dapi_g 
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*
*********************************************************************/
L7_RC_t    hapiBroadVoipPolicyApply(BROAD_POLICY_t policy, DAPI_t *dapi_g)
{
  BROAD_PORT_t              *hapiPortPtr;
  DAPI_USP_t                dapiUsp;
  L7_RC_t                   result; 

  for (dapiUsp.unit=0;dapiUsp.unit<dapi_g->system->totalNumOfUnits;dapiUsp.unit++)
  {
   for (dapiUsp.slot=0;dapiUsp.slot<dapi_g->unit[dapiUsp.unit]->numOfSlots;dapiUsp.slot++)
   {
    if ((dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE) &&
        (IS_SLOT_TYPE_PHYSICAL(&dapiUsp, dapi_g)                       == L7_TRUE))
    {
      /* loop through physical ports */
      for (dapiUsp.port=0;
           dapiUsp.port<dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->numOfPortsInSlot;
           dapiUsp.port++)
      {
       if (isValidUsp (&dapiUsp, dapi_g) != L7_TRUE)
       {
          continue;
       }
       hapiPortPtr = HAPI_PORT_GET(&dapiUsp, dapi_g);
       result = hapiBroadPolicyApplyToIface(policy, hapiPortPtr->bcmx_lport);
       if (result !=L7_SUCCESS)
       {
         continue;
       }
      }
     }
    } /*for (dapiUsp.slot....*/
  }/* for (dapiUsp.unit ....*/
  return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadQosVoipSession
*
* @purpose  Per-port Per call session Create/Delete
*
* @param    DAPI_USP_t* dapiPortPtr - needs to be a valid usp
* @param    DAPI_CMD_t   cmd        - DAPI_CMD_QOS_VOIP_SESSION
* @param    void         *data      - voipSession
* @param    DAPI_t       *dapi_g    - the driver object
*
*
* @returns  L7_RC_t
*
* @notes    a)install rules to prioritize the RTP data. Each call creates 
*             new policy and it will be installed on all physical ports.
*          
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadQosVoipSession(DAPI_USP_t *usp, 
                                DAPI_CMD_t cmd, 
                                void *data, 
                                DAPI_t *dapi_g)
{
  L7_RC_t                   result = L7_SUCCESS;
  BROAD_POLICY_t            voipId=BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t       rule1,rule2;
  L7_uchar8                 exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                             FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uchar8                 ip_ethtype[]  = {0x08, 0x00};
  L7_uchar8                 udp_proto[]   = {0x11};
  L7_uchar8                 tcp_proto[]   = {0x06};
  DAPI_QOS_CMD_t            *voipCmd= (DAPI_QOS_CMD_t*)data;  
  L7_uchar8                 srcIpAddr[4],dstIpAddr[4],srcL4port[2],dstL4port[2];
  L7_uint32                 index=0,protocol;

  memcpy(srcIpAddr, (L7_uchar8*)&(voipCmd->cmdData.voipSession.srcIpAddr),
                          sizeof(voipCmd->cmdData.voipSession.srcIpAddr));
  memcpy(dstIpAddr, (L7_uchar8*)&(voipCmd->cmdData.voipSession.dstIpAddr),
                          sizeof(voipCmd->cmdData.voipSession.dstIpAddr));
  memcpy(srcL4port, (L7_uchar8*)&(voipCmd->cmdData.voipSession.srcL4Port),
                          sizeof(voipCmd->cmdData.voipSession.srcL4Port));
  memcpy(dstL4port, (L7_uchar8*)&(voipCmd->cmdData.voipSession.dstL4Port),
                          sizeof(voipCmd->cmdData.voipSession.dstL4Port));

  protocol = voipCmd->cmdData.voipSession.protoType;
 

 
  if (voipCmd->cmdData.voipSession.val == L7_TRUE) 
  {
     /* to get the free index */
     result = hapiBroadVoipPolicyFind(voipCmd->cmdData.voipSession.srcIpAddr,
                                      voipCmd->cmdData.voipSession.dstIpAddr,
                                      voipCmd->cmdData.voipSession.srcL4Port,
                                      voipCmd->cmdData.voipSession.dstL4Port,
                                      &index, &voipId);
     if (result == L7_SUCCESS)
     {
       result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
       if (L7_SUCCESS != result)
       {
         hapiBroadPolicyCreateCancel();
         return result;
       }
     }
     else if (result == L7_FAILURE)
     {
       return result;
     }
    
     hapiBroadPolicyRuleAdd(&rule1);
     hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_ETHTYPE,ip_ethtype, exact_match);
     
     if ((protocol == L7_QOS_VOIP_PROTOCOL_SCCP) || (protocol == L7_QOS_VOIP_PROTOCOL_H323))
     {
        hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_PROTO, tcp_proto, exact_match);
     }
     else if (protocol == L7_QOS_VOIP_PROTOCOL_MGCP)
     {
        hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_PROTO, udp_proto, exact_match);
     }
     
     /* we do not add transport protocol type for SIP as it can on both TCP and UDP */
     
     hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_DIP, dstIpAddr, exact_match);
     hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_DPORT,dstL4port, exact_match);
     hapiBroadPolicyRuleActionAdd(rule1,BROAD_ACTION_SET_COSQ,FD_VOIP_COS_QUEUE,0,0);
     hapiBroadVoipPolicyRuleCounterAdd(rule1,dapi_g);  
   
     hapiBroadPolicyRuleAdd(&rule2);
     hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_ETHTYPE,ip_ethtype, exact_match);

     if ((protocol == L7_QOS_VOIP_PROTOCOL_SCCP) || (protocol == L7_QOS_VOIP_PROTOCOL_H323))
     {
        hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_PROTO, tcp_proto, exact_match);
     }
     else if (protocol == L7_QOS_VOIP_PROTOCOL_MGCP)
     {
        hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_PROTO, udp_proto, exact_match);
     }

     /* we do not add transport protocol type for SIP as it can on both TCP and UDP */

     hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_SIP, dstIpAddr, exact_match);
     hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_SPORT, dstL4port,exact_match);
     hapiBroadPolicyRuleActionAdd(rule2,BROAD_ACTION_SET_COSQ, FD_VOIP_COS_QUEUE,0,0);
     hapiBroadVoipPolicyRuleCounterAdd(rule2,dapi_g);

     result = hapiBroadPolicyCommit(&voipId);
     if (result !=L7_SUCCESS)
     {
        (void)hapiBroadPolicyDelete(voipId);
        return result;
     }

     hapiBroadVoipPolicyApply(voipId,dapi_g); 
     result = hapiBroadVoipPolicyUpdate(voipCmd,voipId,rule1,rule2,index);
     if (result !=L7_SUCCESS)
     {
       (void)hapiBroadPolicyDelete(voipId);
       return result;
     }      
     return L7_SUCCESS;
   }
   else if(voipCmd->cmdData.voipSession.val == L7_FALSE)
   {
      if (hapiBroadVoipPolicyFind(voipCmd->cmdData.voipSession.srcIpAddr,
                                  voipCmd->cmdData.voipSession.dstIpAddr,
                                  voipCmd->cmdData.voipSession.srcL4Port,
                                  voipCmd->cmdData.voipSession.dstL4Port,
                                  &index, &voipId) == L7_ALREADY_CONFIGURED)
      {
         if (hapiBroadPolicyDelete(broadVoipTable[index].voipPolicy) != L7_SUCCESS)
             return result;
           hapiBroadVoipPolicyDelete(index);  
         
      }
      else
       return L7_FAILURE;
   }
   return L7_SUCCESS;
}
   
/*********************************************************************
*
* @function hapiBroadVoipControlPacketAdd
*
* @purpose  Add policy to copy VOIP control packets to CPU
*
* @param    L7_uint32 protocol  
* @param    L7_ushort16 port 
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*
*********************************************************************/
       
L7_RC_t hapiBroadVoipControlPacketAdd(L7_uint32 protocol,L7_ushort16 port)
{
  BROAD_POLICY_RULE_t       rule1,rule2;
  L7_uchar8                 exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                             FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uchar8                 ip_ethtype[]  = {0x08, 0x00};
  L7_uchar8                 udp_proto[]   = {0x11};
  L7_uchar8                 tcp_proto[]   = {0x06};
  L7_uchar8                 sport[] = {0x00,0x00};

  memcpy(sport,&port,sizeof(port));

  hapiBroadPolicyRuleAdd(&rule1);
  hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_ETHTYPE,ip_ethtype, exact_match);

  if ((protocol == L7_QOS_VOIP_PROTOCOL_SCCP) || (protocol == L7_QOS_VOIP_PROTOCOL_H323))
  {  
    hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_PROTO, tcp_proto, exact_match);
  }
  else if (protocol == L7_QOS_VOIP_PROTOCOL_MGCP)
  {
    hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_PROTO, udp_proto, exact_match);
  }
  
  /* we do not add transport protocol type for SIP as it can on both TCP and UDP */
  hapiBroadPolicyRuleQualifierAdd(rule1, BROAD_FIELD_SPORT, sport, exact_match);
  hapiBroadPolicyRuleActionAdd(rule1,BROAD_ACTION_COPY_TO_CPU,0,0,0);
  hapiBroadPolicyRuleActionAdd(rule1,BROAD_ACTION_SET_COSQ, FD_VOIP_COS_QUEUE,0,0);


  hapiBroadPolicyRuleAdd(&rule2);
  hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_ETHTYPE,ip_ethtype, exact_match);

  if ((protocol == L7_QOS_VOIP_PROTOCOL_SCCP) || (protocol == L7_QOS_VOIP_PROTOCOL_H323))
  {
    hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_PROTO, tcp_proto, exact_match);
  }
  else if (protocol == L7_QOS_VOIP_PROTOCOL_MGCP)
  {
    hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_PROTO, udp_proto, exact_match);
  }

  /* we do not add transport protocol type for SIP as it can on both TCP and UDP */

  hapiBroadPolicyRuleQualifierAdd(rule2, BROAD_FIELD_DPORT, sport, exact_match);
  hapiBroadPolicyRuleActionAdd(rule2,BROAD_ACTION_COPY_TO_CPU,0,0,0);
  hapiBroadPolicyRuleActionAdd(rule2,BROAD_ACTION_SET_COSQ, FD_VOIP_COS_QUEUE,0,0);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadQosVoipProfile 
*
* @purpose 
*
* @param    DAPI_USP_t *usp - needs to be a valid usp
* @param    DAPI_CMD_t cmd  - DAPI_CMD_QOS_VOIP_PROFILE
* @param    void *data      - voipProfile
* @param    DAPI_t *dapi_g  - the driver object
*
*
* @returns  L7_RC_t
*
* @notes install rules to copy the signaling protocol packets to CPU and 
*        rules are based on L4 port. 
*        The following are the L4 port and signaling protocols
*         SCCP   - 0x07d0
*         SIP    - 0x13C4
*         MGCP   - 0x0AA7
*         H323   - 0x06B8  
* @end
*
*********************************************************************/

L7_RC_t hapiBroadQosVoipProfile(DAPI_USP_t *usp, 
                                DAPI_CMD_t cmd, 
                                void *data, 
                                DAPI_t *dapi_g)
{
  L7_RC_t                   result = L7_SUCCESS;
  BROAD_PORT_t              *hapiPortPtr;
  DAPI_PORT_t               *dapiPortPtr;
  DAPI_QOS_CMD_t            *voipCmd= (DAPI_QOS_CMD_t*)data;
  

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (voipCmd->cmdData.voipProfile.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (voipCmd->cmdData.voipProfile.val == L7_TRUE)
  {  
    if (voipProfilePolicy == BROAD_POLICY_INVALID)
    {
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
      if (L7_SUCCESS != result)
      {
        hapiBroadPolicyCreateCancel();
        return result;
      }
      if (voipCmd->cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_MGCP] == L7_TRUE) 
        hapiBroadVoipControlPacketAdd(L7_QOS_VOIP_PROTOCOL_MGCP, L7_MGCP_L4_PORT);
       
      if (voipCmd->cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_H323] == L7_TRUE)
        hapiBroadVoipControlPacketAdd(L7_QOS_VOIP_PROTOCOL_H323, L7_H323_L4_PORT);
      if (voipCmd->cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SIP] == L7_TRUE)
        hapiBroadVoipControlPacketAdd(L7_QOS_VOIP_PROTOCOL_SIP, L7_SIP_L4_PORT );
      if (voipCmd->cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SCCP]== L7_TRUE)
        hapiBroadVoipControlPacketAdd(L7_QOS_VOIP_PROTOCOL_SCCP, L7_SCCP_L4_PORT);

      result = hapiBroadPolicyCommit(&voipProfilePolicy);
      if (result !=L7_SUCCESS)
        return result;

    }
    result = hapiBroadQosVoIPApplyPolicyToIface(usp, dapi_g, L7_FALSE);
    if (result ==L7_SUCCESS)
    {
      hapiPortPtr->voipPolicy = voipProfilePolicy;
      if ((count == 1) && (voipCmd->cmdData.voipProfile.guarentedBw))
      {
        result = cosq_port_bandwidth_set_all(dapi_g, hapiPortPtr->bcmx_lport,
                                             FD_VOIP_COS_QUEUE,
                                             voipCmd->cmdData.voipProfile.guarentedBw,
                                             0);
      }
      return result;
    }

  }
  else if (voipCmd->cmdData.voipProfile.val == L7_FALSE)
  {
    if (voipProfilePolicy != BROAD_POLICY_INVALID)
    {
      result = hapiBroadQosVoIPRemovePolicyFromIface(usp, dapi_g, L7_FALSE);
      if (result == L7_SUCCESS)
      {
        hapiPortPtr->voipPolicy = BROAD_POLICY_INVALID;
      }
      if (count == 0)
      {
        hapiBroadPolicyDelete(voipProfilePolicy);
        voipProfilePolicy = BROAD_POLICY_INVALID;
        if (voipCmd->cmdData.voipProfile.guarentedBw)
        {
          result = cosq_port_bandwidth_set_all(dapi_g, hapiPortPtr->bcmx_lport,
                                               FD_VOIP_COS_QUEUE,0,0);
        }
      }
    }
  }
  
  return result; 
}
 
/**********************************************************************
* @function hapiBroadQosVoipStatsGet
*
* @purpose  
*
* @param   DAPI_USP_t *usp - needs to be a valid usp
* @param   DAPI_CMD_t cmd  - DAPI_CMD_QOS_VOIP_STATS_GET 
* @param   void *data      - voipStats
* @param   DAPI_t *dapi_g  - the driver object
* 
* @returns  L7_RC_t
*
* @notes   i) accumulate the stats of two rules belongs to each call
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadQosVoipStatsGet(DAPI_USP_t *usp,
                                 DAPI_CMD_t cmd,
                                 void *data,
                                 DAPI_t *dapi_g)
{
  L7_RC_t                   result = L7_SUCCESS;
  BROAD_PORT_t              *hapiPortPtr;
  DAPI_PORT_t               *dapiPortPtr;
  DAPI_QOS_CMD_t            *voipCmd= (DAPI_QOS_CMD_t*)data;
  BROAD_POLICY_STATS_t      stats[2];
  BROAD_POLICY_t            voipId;
  L7_uint32                 index;
  L7_ulong64                temp;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (voipCmd->cmdData.voipStats.getOrSet != DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (hapiBroadVoipPolicyFind( voipCmd->cmdData.voipStats.srcIpAddr,
                               voipCmd->cmdData.voipStats.dstIpAddr,
                               voipCmd->cmdData.voipStats.srcL4Port,
                               voipCmd->cmdData.voipStats.dstL4Port,
                               &index, &voipId) == L7_ALREADY_CONFIGURED)
  {
    result = hapiBroadPolicyStatsGet(voipId, broadVoipTable[index].ruleId[0], &stats[0]);
    if (result == L7_SUCCESS)
    {
      result = hapiBroadPolicyStatsGet(voipId, broadVoipTable[index].ruleId[1], &stats[1]);
      if (result == L7_SUCCESS)
      {
        if (voipCmd->cmdData.voipStats.pHitcount != L7_NULLPTR)
        {
          /* accumulate stats of two rules */
          temp.high = ((stats[0].statMode.counter.count >>32) & 0xfffffff) +
                       ((stats[1].statMode.counter.count >>32) & 0xfffffff);
          temp.low  = ((stats[0].statMode.counter.count) & 0xfffffff) +
                       ((stats[1].statMode.counter.count) & 0xfffffff);
          voipCmd->cmdData.voipStats.pHitcount->high = temp.high;
          voipCmd->cmdData.voipStats.pHitcount->low  = temp.low;
        }
      }
    }
  }
  return result;
}    
/**********************************************************************
* @function hapiBroadVoipDump 
*
* @purpose  
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*
*********************************************************************/
L7_RC_t  hapiBroadVoipCallDump()
{
  L7_uint32  i ;
  for (i=0;i<L7_VOIP_MAX_CALLS;i++)
  {
    printf("\r\n index =%u",i);
    printf("\r\n src IP address=0x%x",broadVoipTable[i].srcIp);
    printf("\r\n dst IP address=0x%x",broadVoipTable[i].dstIp);
    printf("\r\n src L4 port   =0x%x",broadVoipTable[i].srcPort);
    printf("\r\n dst L4 port   =0x%x",broadVoipTable[i].dstPort);
    printf("\r\n policy Id     = %u",broadVoipTable[i].voipPolicy);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Notifies the VoIP component that a physical port has been
*           added to a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments   
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoIPPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  BROAD_PORT_t *hapiPortPtr;
  BROAD_PORT_t *hapiLagPtr;
  L7_RC_t rc = L7_SUCCESS;

  hapiPortPtr = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr  = HAPI_PORT_GET(lagUsp,  dapi_g);

  /* apply LAG policy on the port */

  if (hapiLagPtr->voipPolicy != BROAD_POLICY_INVALID)
  {
    if (hapiPortPtr->voipPolicy == BROAD_POLICY_INVALID)
    {
       rc = hapiBroadQosVoIPApplyPolicyToIface(portUsp, dapi_g, L7_TRUE);
    }
  }
  else
  {
    if (hapiPortPtr->voipPolicy != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadQosVoIPRemovePolicyFromIface(portUsp, dapi_g, L7_TRUE);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Notifies the VoIP component that a physical port has been
*           removed from a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosVoIPPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  BROAD_PORT_t *hapiPortPtr;
  BROAD_PORT_t *hapiLagPtr;
  L7_RC_t rc = L7_SUCCESS;

  hapiPortPtr = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr  = HAPI_PORT_GET(lagUsp,  dapi_g);

  /* apply LAG policy on the port */
  if (hapiLagPtr->voipPolicy != BROAD_POLICY_INVALID)
  {
    if (hapiPortPtr->voipPolicy == BROAD_POLICY_INVALID)
    {
       rc = hapiBroadQosVoIPRemovePolicyFromIface(portUsp, dapi_g, L7_TRUE);
    }
  }
  else
  {
    if (hapiPortPtr->voipPolicy != BROAD_POLICY_INVALID)
    {
      rc = hapiBroadQosVoIPApplyPolicyToIface(portUsp, dapi_g, L7_TRUE);
    }
  }
  return rc;
}

L7_RC_t hapiBroadVoIPPolicyDump()
{
  printf("\r\nvoipProfilePolicy = %x\r\n",voipProfilePolicy);
  printf("\r\nPolicy count = %d\r\n",count);
  return L7_SUCCESS;
}
