/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:       sysnet_api.c
*
* Purpose:    Sysnet API functions
*
* Component:  sysnet
*
* Comments:   none
*
* Date:       11/01/2002
*
* Created by: mfiorito
*
*********************************************************************/
/*************************************************************

*************************************************************/

#include "sysnetinclude.h"
#include "default_cnfgr.h"

void                  *sysnetPduHookSema;
sysnetPduHooks_t      sysnetPduHooks[SYSNET_AF_MAX][L7_SYSNET_PDU_MAX_HOOKS];
sysnetPduRcCounters_t sysnetPduRcCounters[SYSNET_AF_MAX][L7_SYSNET_PDU_MAX_HOOKS];
L7_uint32             sysnetPduAFMap[SYSNET_AF_MAX];
L7_BOOL               sysnetPduHooksInitialized = L7_FALSE;

/*********************************************************************
* @purpose  Initialize the Sysnet PDU Intercept hook structure
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduHooksInit()
{
  if (sysnetPduHooksInitialized == L7_TRUE)
    return L7_SUCCESS;

  sysnetPduHookSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (sysnetPduHookSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Unable to create SYSNET PDU Hook semaphore\n");
    return L7_FAILURE;
  }

  bzero((L7_uchar8 *)sysnetPduHooks, sizeof(sysnetPduHooks));
  bzero((L7_uchar8 *)sysnetPduRcCounters, sizeof(sysnetPduRcCounters));
  /* Fill in AF map with 0xFF since 0 is a valid Address Family */
  memset(sysnetPduAFMap, 0xFF, sizeof(sysnetPduAFMap));
  sysnetPduHooksInitialized = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register for a PDU hook
*
* @param    *sysnetPduIntercept  @b{(input)} Pointer to intercept registration info
*
* @returns  L7_SUCCESS  if registration was successful
* @returns  L7_FAILURE  if 1) invalid AF, hook ID or precedence, or
                        2) AF/hookID/precedence combination already registered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduInterceptRegister(sysnetPduIntercept_t *sysnetPduIntercept)
{
  L7_uint32 af = sysnetPduIntercept->addressFamily;
  L7_uint32 hookId = sysnetPduIntercept->hookId;
  L7_uint32 precedence = sysnetPduIntercept->hookPrecedence;
  L7_uint32 sysnetAFIndex;

  if ( (sysnetPduHooksInitialized == L7_FALSE) && (sysNetPduHooksInit() != L7_SUCCESS) )
    return L7_FAILURE;

  if ( (af >= SYSNET_AF_MAX) ||
       (hookId >= L7_SYSNET_PDU_MAX_HOOKS) ||
       (precedence >= L7_SYSNET_HOOK_PRECEDENCE_LAST) )
    return L7_FAILURE;

  osapiSemaTake(sysnetPduHookSema, L7_WAIT_FOREVER);

  /* Get the index into the hook table for this address family, or the next available */
  if (sysNetPduAFMapGet(af, &sysnetAFIndex) != L7_SUCCESS)
  {
    if (sysNetPduAFMapNextAvailGet(&sysnetAFIndex) != L7_SUCCESS)
    {
      osapiSemaGive(sysnetPduHookSema);
      return L7_FAILURE;
    }
    sysnetPduAFMap[sysnetAFIndex] = af;
  }

  if (sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncList[precedence] != L7_NULLPTR)
  {
    /* af/hook/precedence already registered */
    osapiSemaGive(sysnetPduHookSema);
    return L7_FAILURE;
  }

  strcpy(sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncNameList[precedence],
         sysnetPduIntercept->interceptFuncName);
  sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncList[precedence] = sysnetPduIntercept->interceptFunc;
  sysnetPduHooks[sysnetAFIndex][hookId].numHooks++;

  osapiSemaGive(sysnetPduHookSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-Register a PDU hook
*
* @param    *sysnetPduIntercept  @b{(input)} Pointer to intercept registration info
*
* @returns  L7_SUCCESS  if successful de-registation
* @returns  L7_FAILURE  if 1) hooks uninitialized, 2) invalid AF, hook ID or precendence,
*                          3) or no registation present
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sysNetPduInterceptDeregister(sysnetPduIntercept_t *sysnetPduIntercept)
{
  L7_uint32 af = sysnetPduIntercept->addressFamily;
  L7_uint32 hookId = sysnetPduIntercept->hookId;
  L7_uint32 precedence = sysnetPduIntercept->hookPrecedence;
  L7_uint32 sysnetAFIndex;

  if (sysnetPduHooksInitialized == L7_FALSE)
    return L7_FAILURE;

  if ( (af >= SYSNET_AF_MAX - 1) ||
       (hookId >= L7_SYSNET_PDU_MAX_HOOKS - 1) ||
       (precedence >= L7_SYSNET_HOOK_PRECEDENCE_LAST) )
    return L7_FAILURE;

  osapiSemaTake(sysnetPduHookSema, L7_WAIT_FOREVER);

  if (sysNetPduAFMapGet(af, &sysnetAFIndex) != L7_SUCCESS)
  {
    /* af/hook/precedence not registered */
    osapiSemaGive(sysnetPduHookSema);
    return L7_FAILURE;
  }

  if (sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncList[precedence] == L7_NULLPTR)
  {
    /* af/hook/precedence not registered */
    osapiSemaGive(sysnetPduHookSema);
    return L7_FAILURE;
  }

  sysnetPduHooks[sysnetAFIndex][hookId].numHooks--;
  sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncList[precedence] = L7_NULLPTR;
  bzero(sysnetPduHooks[sysnetAFIndex][hookId].interceptFuncNameList[precedence], SYSNET_MAX_FUNC_NAME);

  osapiSemaGive(sysnetPduHookSema);
  return L7_SUCCESS;
}
