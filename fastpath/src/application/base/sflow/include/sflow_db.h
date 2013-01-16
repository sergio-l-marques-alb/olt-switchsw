/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_db.h
*
* @purpose   SFlow instance datastore access
*
* @component sflow
*
* @comments  Provides APIs to access the sampler, poller instances
*
* @create 23-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_DB_H
#define SFLOW_DB_H
#include "sflow_mib.h"
/******************************************************************************
  sFlow Sampler instances Processing Routines
*******************************************************************************/
L7_RC_t sFlowSamplerInstanceAdd(L7_uint32 dsIndex, L7_uint32 instance, 
                                SFLOW_sampler_mib_t *pSamplerCfg);
L7_RC_t sFlowSamplerInstanceDelete(L7_uint32 dsIndex, L7_uint32 instance);
SFLOW_sampler_t *sFlowSamplerInstanceGet(L7_uint32 dsIndex, L7_uint32 instance,
                                         L7_uint32 flag);

/******************************************************************************
  sFlow Poller instances Processing Routines
*******************************************************************************/
L7_RC_t sFlowPollerInstanceAdd(L7_uint32 dsIndex, L7_uint32 instance,
                               SFLOW_poller_mib_t *pPollerCfg);
L7_RC_t sFlowPollerInstanceDelete(L7_uint32 dsIndex, L7_uint32 instance);
SFLOW_poller_t *sFlowPollerInstanceGet(L7_uint32 dsIndex, L7_uint32 instance,
                                       L7_uint32 flag);

#endif /* SFLOW_DB_H  */
