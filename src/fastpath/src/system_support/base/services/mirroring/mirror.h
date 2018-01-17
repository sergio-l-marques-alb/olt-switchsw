/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror.h
* @purpose     Port mirroring API prototypes
* @component   mirroring
* @comments    none
* @create      11/21/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

#ifndef MIRROR_H
#define MIRROR_H
#include "nimapi.h"
#include "mirror_cfg.h"
#include "mirror_api.h"
#define MIRROR_DEBUG(msg) SYSAPI_PRINTF(0,"\n%s",msg)

L7_RC_t mirrorSave(void);

L7_BOOL mirrorHasDataChanged(void);

void mirrorResetDataChanged(void);

L7_RC_t mirrorApplyConfigData(void);

L7_RC_t mirrorIntfCreate(L7_uint32 intIfnum);

L7_RC_t mirrorIntfApplyConfigData(L7_uint32 intIfnum);

L7_RC_t mirrorIntfDetach(L7_uint32 intIfnum);

L7_RC_t mirrorIntfDelete(L7_uint32 intIfNum);

L7_RC_t mirrorIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_CORRELATOR_t correlator);

void mirrorStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

void mirrorBuildDefaultConfigData(L7_uint32 ver);

void mirrorNimEventIntfNotify(L7_uint32 intIfDest,
                              L7_PORT_EVENTS_t eventType);

L7_RC_t mirrorIsValidSrcIntf(L7_uint32 intIfNum,L7_BOOL *valid);

L7_RC_t mirrorIsValidDestIntf(L7_uint32 intIfNum, L7_BOOL *valid);

L7_BOOL mirrorIsValidSession(L7_uint32 sessionNum);
 
L7_BOOL mirrorDestEmpty(L7_uint32 sessionNum);

L7_RC_t mirrorSourcePortDirectionSet(L7_uint32 sessionNum,L7_uint32 intIfNum, L7_MIRROR_DIRECTION_t probeType);

L7_uint32 mirrorConfigTraceModeGet(void);

L7_RC_t mirrorSourcePortSet(L7_uint32 sessionNum, L7_uint32 intIfNum,L7_uint32 mode, L7_MIRROR_DIRECTION_t probeType);

#define MIRROR_TRACE(format,args...)                      \
{                                                           \
   if ( mirrorConfigTraceModeGet() == L7_ENABLE)                   \
   {                                                        \
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,format,##args);    \
   }                                                        \
}


#endif  /* MIRROR_H */


