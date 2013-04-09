/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:      broad_qos_common.h
*
* @purpose   This file contains the prototypes for QOS
*
* Component: hapi
*
* Comments:
*
* Created by: robp 3/18/2004
*
*********************************************************************/

#ifndef INCLUDE_BROAD_QOS_COMMON_H
#define INCLUDE_BROAD_QOS_COMMON_H

#include "broad_common.h"
#include "broad_policy.h"
#include "broad_tlv.h" 

typedef struct
{
    void      *semaphore;
    L7_BOOL   dscpMapDirty;
    L7_uchar8 dscpMap[L7_QOS_COS_MAP_NUM_IPDSCP]; /* Global dscp mapping table */
}
HAPI_BROAD_QOS_t;

typedef enum
{
  HAPI_QOS_INTF_DIR_IN,
  HAPI_QOS_INTF_DIR_OUT,
  HAPI_QOS_INTF_DIR_COUNT

} HAPI_QOS_INTF_DIR_t;

/* Convert IP Prec to DSCP/TOS values. */
#define BROAD_IPPREC_TO_DSCP(p) ((p << 3) & 0xFF)
#define BROAD_IPPREC_TO_TOS(p)  ((p << 5) & 0xFF)

/* Number of dependent policies per port/LAG. For performance reasons this
 * list is kept small.
 */
#define BROAD_MAX_POLICY_DEPENDS 5

/* Each DiffServ instance can consume multiple rules, for cases such as
 * EthType1/2 and color-aware policing. Given support for simple-policing
 * the factor is 2 EthTypes * 2 ColorAware rules = 4.
 */
#define BROAD_MAX_RULES_PER_DIFFSERV_POLICY   (L7_DIFFSERV_INST_PER_POLICY_LIM*4)

typedef struct
{
    BROAD_POLICY_t policyId[HAPI_QOS_INTF_DIR_COUNT];       /* Indicates the port policy id. */
    BROAD_POLICY_t policyId2[HAPI_QOS_INTF_DIR_COUNT];      /* Indicates the second port policy id,
                                                               if applicable. The ACL application allows
                                                               IPv6 ACLs to be mixed with IPv4 ACLs.
                                                               When this occurs, the HAPI ACL code
                                                               needs to split the combined ACL
                                                               into two separate policies: one for the 
                                                               IPv4/MAC policy and one for the IPv6
                                                               policy. Regarding enforcement of
                                                               mutual exclusion between ACL and Diffserv,
                                                               it is sufficient to just check if 
                                                               'policyId' is a valid policy ID, as
                                                               ACL will always set this first, and only 
                                                               use 'policyId2' if necessary. */
    
    /* DiffServ instance to rule mapping table used for stats. */
    BROAD_POLICY_RULE_LIST_t ruleList[HAPI_QOS_INTF_DIR_COUNT][L7_DIFFSERV_INST_PER_POLICY_LIM];

    /* policies that are dependent on this port, e.g. redirect to LAG */
    BROAD_POLICY_t dependList[BROAD_MAX_POLICY_DEPENDS];
}
HAPI_BROAD_ACLDS_PORT_t;

typedef struct
{
    L7_uchar8 wredMinThresh[L7_MAX_CFG_DROP_PREC_LEVELS+1];
    L7_uchar8 wredMaxThresh[L7_MAX_CFG_DROP_PREC_LEVELS+1];
    L7_uchar8 wredDropProb[L7_MAX_CFG_DROP_PREC_LEVELS+1];
    L7_uchar8 taildropThresh[L7_MAX_CFG_DROP_PREC_LEVELS+1];
}
HAPI_BROAD_COS_COLOR_PORT_t;

typedef struct
{
    L7_uint32 trustMode;         /* interface trust mode */

    /* L2 mappings are supported in all builds so they are stored
     * in BROAD_PORT_t. L3 specific mappings are supported only in
     * QoS packages, so they are stored separately (below).
     */
    BROAD_POLICY_t precId;
    BROAD_POLICY_t precDefaultPolicyId;

    L7_BOOL dscpMapDirty;

    L7_uchar8 precMap[L7_QOS_COS_MAP_NUM_IPPREC];
    L7_uchar8 dscpMap[L7_QOS_COS_MAP_NUM_IPDSCP]; /* per port DSCP table */
    L7_uchar8 defaultCos;     /* default cos for L2 frames */

    /* per-port configuration */
    L7_BOOL                         intfShapingSpec;
    L7_uint32                       intfShaping;

    /* per-queue configuration */
    L7_BOOL                         queueConfigSpec;
    DAPI_QOS_COS_QUEUE_MGMT_TYPE_t  dropType[BCM_COS_COUNT];
    DAPI_QOS_COS_QUEUE_SCHED_TYPE_t schedType[BCM_COS_COUNT];
    L7_uint32                       minBw[BCM_COS_COUNT];     /* PTin modified: QoS: L7_uchar8 to L7_uint32 */
    L7_uint32                       maxBw[BCM_COS_COUNT];     /* PTin modified: QoS: L7_uchar8 to L7_uint32 */
#ifdef L7_COS_PACKAGE
    L7_uchar8                       wredExponent;
    HAPI_BROAD_COS_COLOR_PORT_t     perColorParams[BCM_COS_COUNT];
#endif
}
HAPI_BROAD_COS_PORT_t;

typedef struct
{
    HAPI_BROAD_ACLDS_PORT_t aclds;
    HAPI_BROAD_COS_PORT_t   cos;
}
HAPI_BROAD_QOS_PORT_t;

/* QOS will send down LAG config when the LAG is created, but has no
 * members. This check allows us to defer LAG config until it has at
 * least one member, that is, the LAG actually exists in hardware.
 */
#define BROAD_PORT_IS_LAG(portPtr)      ((portPtr)->port_is_lag == L7_TRUE)
#define BROAD_PORT_IS_ACQUIRED(portPtr) ((portPtr)->hapiModeparm.physical.isMemberOfLag)

/*********************************************************************
*
* @purpose Initialize the QOS Common package
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCommonInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Take semaphore to protect QOS resources
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosSemTake(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Give semaphore to protect QOS resources
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t           result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosSemGive(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Determines the bandwidth of an interface
*
* @param   BROAD_PORT_t     *hapiPortPtr
* @param   L7_uint32        *portSpeed (output)
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadQosIntfSpeedGet(BROAD_PORT_t *hapiPortPtr, L7_uint32 *portSpeed);

/*********************************************************************
*
* @purpose  Apply QOS attributes to a physical port
*
* @param    BROAD_POLICY_t    policy
* @param    BROAD_PORT_t     *hapiPortPtr
*
* @returns  L7_RC_t
*
* @notes    For internal use only.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosApplyToIface(BROAD_POLICY_t policy, BROAD_PORT_t *hapiPortPtr);

/*********************************************************************
*
* @purpose  Remove QOS attributes from a physical port
*
* @param    BROAD_POLICY_t    policy
* @param    BROAD_PORT_t     *hapiPortPtr
*
* @returns  L7_RC_t
*
* @notes    For internal use only.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosRemoveFromIface(BROAD_POLICY_t policy, BROAD_PORT_t *hapiPortPtr);

/*********************************************************************
*
* @purpose  Apply QOS attributes to a port or LAG interface
*
* @param    BROAD_POLICY_t    policy
* @param    DAPI_USP_t       *usp
* @param    DAPI_t           *dapi_g
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosApplyPolicy(BROAD_POLICY_t policy, DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Remove Qos attributes from a port or LAG interface
*
* @param    BROAD_POLICY_t    policy
* @param    DAPI_USP_t       *usp
* @param    DAPI_t           *dapi_g
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosRemovePolicy(BROAD_POLICY_t policy, DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Add dependency between the policy and the port/LAG.
*
* @param    BROAD_POLICY_t    policy - dependent policy
* @param    DAPI_USP_t       *usp    - subject port/LAG
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosAddDepends(BROAD_POLICY_t policy, DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Delete all dependencies this policy has on any ports.
*
* @param    BROAD_POLICY_t    policy - dependent policy
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosDelDependsAll(BROAD_POLICY_t policy, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose  Update dependent policies as result of subject port change.
*
* @param    DAPI_USP_t       *usp    - subject port/LAG
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosUpdateDepends(DAPI_USP_t *usp, DAPI_t *dapi_g);

#endif
