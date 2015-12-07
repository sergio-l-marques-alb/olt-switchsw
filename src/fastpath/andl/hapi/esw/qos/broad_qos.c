/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_qos.c
*
* Purpose: This file contains all the routines for the QOS package
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/
#include <string.h>

#include "broad_qos.h"
#include "broad_voip.h"
#include "broad_iscsi.h"
#include "broad_qos_debug.h"
#include "log.h"

/* Default weights used in non-QoS packages. */
extern L7_uint32 wrr_default_weights[];     /* PTin added: QoS */

/*********************************************************************
*
* @purpose Initialize the QoS package
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
L7_RC_t hapiBroadQosInit(DAPI_t *dapi_g)
{
    L7_RC_t  result;

    hapiBroadQosDebugInit();

    result = hapiBroadQosCommonInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosCommonInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
    result = hapiBroadQosDiffServInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosDiffServInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif

    result = hapiBroadQosAclInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosAclInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }

    result = hapiBroadQosCosInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosCosInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
    result = hapiBroadQosVoipInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosVoipInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
    result = hapiBroadQosIscsiInit(dapi_g);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosIscsiInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif
    return result;
}

/*********************************************************************
*
* @purpose Initialize the QoS package per card
*
* @param   L7_ushort16 unitNum     
* @param   L7_ushort16 slotNum     
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCardInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{
    L7_RC_t                result = L7_SUCCESS;
    DAPI_USP_t             usp;
    BROAD_PORT_t          *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    HAPI_QOS_INTF_DIR_t    direction;
    L7_uint8               cosIndex;

    /* allocate QOS structures for all hapiPorts on this card */
    usp.unit = unitNum;
    usp.slot = slotNum;

    for (usp.port = 0; usp.port < dapi_g->unit[unitNum]->slot[slotNum]->numOfPortsInSlot; usp.port++)
    {
        int i;

        hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

        hapiPortPtr->qos = (void*)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(HAPI_BROAD_QOS_PORT_t));
        if (L7_NULLPTR == hapiPortPtr->qos)
        {
            L7_LOG_ERROR(0);
            return L7_FAILURE;
        }

        memset (hapiPortPtr->qos, 0, sizeof (HAPI_BROAD_QOS_PORT_t));

        qosPortPtr = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;
        qosPortPtr->aclds.policyId[HAPI_QOS_INTF_DIR_IN]        = BROAD_POLICY_INVALID;
        qosPortPtr->aclds.policyId[HAPI_QOS_INTF_DIR_OUT]       = BROAD_POLICY_INVALID;
        qosPortPtr->aclds.policyId2[HAPI_QOS_INTF_DIR_IN]       = BROAD_POLICY_INVALID;
        qosPortPtr->aclds.policyId2[HAPI_QOS_INTF_DIR_OUT]      = BROAD_POLICY_INVALID;

        for (direction = HAPI_QOS_INTF_DIR_IN; direction < HAPI_QOS_INTF_DIR_COUNT; direction++)
        {
          for (i = 0; i < L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
          {
              qosPortPtr->aclds.ruleList[direction][i].inUse    = L7_FALSE;
              qosPortPtr->aclds.ruleList[direction][i].instance = BROAD_POLICY_RULE_INVALID;
              qosPortPtr->aclds.ruleList[direction][i].rule     = BROAD_POLICY_RULE_INVALID;
          }
        }
        
        for (i = 0; i < BROAD_MAX_POLICY_DEPENDS; i++)
            qosPortPtr->aclds.dependList[i] = BROAD_POLICY_INVALID;

        qosPortPtr->cos.trustMode = DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P;

        qosPortPtr->cos.precId = BROAD_POLICY_INVALID;

        hapiPortPtr->voipPolicy = BROAD_POLICY_INVALID;

        /* Ptin modified: QoS */
        for (cosIndex = 0; cosIndex < L7_MAX_CFG_QUEUES_PER_PORT; cosIndex++)
        {
          qosPortPtr->cos.wredExponent[cosIndex] = FD_QOS_COS_QCFG_WRED_DECAY_EXP; 

          qosPortPtr->cos.wrr_weights[cosIndex] = wrr_default_weights[cosIndex];
        }
    }

    return result;
}

/*********************************************************************
*
* @purpose Initialize the QoS package for all ports on a card
*
* @param   L7_ushort16       unitNum
* @param   L7_ushort16       slotNum
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortInit(DAPI_PORT_t *dapiPortPtr)
{
    L7_RC_t    result = L7_SUCCESS;

#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
    result = hapiBroadQosDiffservPortInit(dapiPortPtr);
    if (L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosDiffservPortInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif

    result = hapiBroadQosAclPortInit(dapiPortPtr);
    if(L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosAclPortInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }

    result = hapiBroadQosCosPortInit(dapiPortPtr);
    if(L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosCosPortInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
    result = hapiBroadQosVoipPortInit(dapiPortPtr);
    if(L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosVoipPortInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
    result = hapiBroadQosIscsiPortInit(dapiPortPtr);
    if(L7_SUCCESS != result)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadQosIscsiPortInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
    }
#endif
    return result;
}

/*********************************************************************
*
* @purpose Update the stats from the QoS policies in effect. 
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
L7_RC_t hapiBroadQosStatsUpdate(DAPI_t *dapi_g)
{
    /* Stats are not currently updated using this mechanism. */
    return L7_ERROR;
}

/*********************************************************************
*
* @purpose Notifies the ACL / DiffServ component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t           result
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosPolicyLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    L7_RC_t                 result = L7_SUCCESS;
    BROAD_PORT_t           *hapiPortPtr;
    BROAD_PORT_t           *hapiLagPtr;
    HAPI_BROAD_QOS_PORT_t  *hapiPortQosPtr;
    HAPI_BROAD_QOS_PORT_t  *hapiLagQosPtr;
    HAPI_QOS_INTF_DIR_t     direction;

    hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
    hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);
    hapiPortQosPtr = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;
    hapiLagQosPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiLagPtr->qos;

    for (direction = HAPI_QOS_INTF_DIR_IN; direction < HAPI_QOS_INTF_DIR_COUNT; direction++)
    {
      if (hapiPortQosPtr->aclds.policyId[direction] != hapiLagQosPtr->aclds.policyId[direction])
      {
        /* remove the port policy, if exists */
        if (BROAD_POLICY_INVALID != hapiPortQosPtr->aclds.policyId[direction])
            if (hapiBroadQosRemoveFromIface(hapiPortQosPtr->aclds.policyId[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;

        /* apply the LAG policy, if exists */
        if (BROAD_POLICY_INVALID != hapiLagQosPtr->aclds.policyId[direction])
            if (hapiBroadQosApplyToIface(hapiLagQosPtr->aclds.policyId[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
      }
      if (hapiPortQosPtr->aclds.policyId2[direction] != hapiLagQosPtr->aclds.policyId2[direction])
      {
        /* remove the port policy, if exists */
        if (BROAD_POLICY_INVALID != hapiPortQosPtr->aclds.policyId2[direction])
            if (hapiBroadQosRemoveFromIface(hapiPortQosPtr->aclds.policyId2[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;

        /* apply the LAG policy, if exists */
        if (BROAD_POLICY_INVALID != hapiLagQosPtr->aclds.policyId2[direction])
            if (hapiBroadQosApplyToIface(hapiLagQosPtr->aclds.policyId2[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
      }
    }

    /* update policies dependent upon this LAG */
    if (hapiBroadQosUpdateDepends(lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    return result;
}

/*********************************************************************
*
* @purpose Notifies the ACL / Diffserv component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t           result
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosPolicyLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    L7_RC_t                 result = L7_SUCCESS;
    BROAD_PORT_t           *hapiPortPtr;
    BROAD_PORT_t           *hapiLagPtr;
    HAPI_BROAD_QOS_PORT_t  *hapiPortQosPtr;
    HAPI_BROAD_QOS_PORT_t  *hapiLagQosPtr;
    HAPI_QOS_INTF_DIR_t     direction;

    hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
    hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);
    hapiPortQosPtr = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;
    hapiLagQosPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiLagPtr->qos;

    for (direction = HAPI_QOS_INTF_DIR_IN; direction < HAPI_QOS_INTF_DIR_COUNT; direction++)
    {
      if (hapiPortQosPtr->aclds.policyId[direction] != hapiLagQosPtr->aclds.policyId[direction])
      {
        /* remove the LAG policy, if exists */
        if (BROAD_POLICY_INVALID != hapiLagQosPtr->aclds.policyId[direction])
            if (hapiBroadQosRemoveFromIface(hapiLagQosPtr->aclds.policyId[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
  
        /* apply the port policy, if exists */
        if (BROAD_POLICY_INVALID != hapiPortQosPtr->aclds.policyId[direction])
            if (hapiBroadQosApplyToIface(hapiPortQosPtr->aclds.policyId[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
      }
      if (hapiPortQosPtr->aclds.policyId2[direction] != hapiLagQosPtr->aclds.policyId2[direction])
      {
        /* remove the LAG policy, if exists */
        if (BROAD_POLICY_INVALID != hapiLagQosPtr->aclds.policyId2[direction])
            if (hapiBroadQosRemoveFromIface(hapiLagQosPtr->aclds.policyId2[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
  
        /* apply the port policy, if exists */
        if (BROAD_POLICY_INVALID != hapiPortQosPtr->aclds.policyId2[direction])
            if (hapiBroadQosApplyToIface(hapiPortQosPtr->aclds.policyId2[direction], hapiPortPtr) != L7_SUCCESS)
                result = L7_FAILURE;
      }
    }

    /* update policies dependent upon this LAG */
    if (hapiBroadQosUpdateDepends(lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    return result;
}

/*********************************************************************
*
* @purpose Notifies the QOS component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    L7_RC_t result = L7_SUCCESS;

    hapiBroadQosSemTake(dapi_g);

    /* Handle ACL/Diffserv policies on the port. */
    if (hapiBroadQosPolicyLagAddNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    /* Handle CoS policies on the port. */
    if (hapiBroadQosCosPortLagAddNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
    /* Handle VoIP policies on the port. */
    if (hapiBroadQosVoIPPortLagAddNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;
#endif
    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose Notifies the QOS component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    L7_RC_t result = L7_SUCCESS;

    hapiBroadQosSemTake(dapi_g);

    /* Handle ACL/Diffserv policies on the port. */
    if (hapiBroadQosPolicyLagDeleteNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    /* Handle CoS policies on the port. */
    if (hapiBroadQosCosPortLagDeleteNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

#ifdef L7_QOS_FLEX_PACKAGE_VOIP
    /* Handle VoIP policies on the port. */
    if (hapiBroadQosVoIPPortLagDeleteNotify(portUsp, lagUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;
#endif
    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose Notifies the QOS component of physical port speed changes
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLinkUpNotify(DAPI_USP_t *portUsp, DAPI_t *dapi_g)
{
    L7_RC_t result = L7_SUCCESS;

    hapiBroadQosSemTake(dapi_g);

    /* only CoS component cares about link changes */
    if (hapiBroadQosCosPortLinkUpNotify(portUsp, dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    hapiBroadQosSemGive(dapi_g);

    return result;
}

