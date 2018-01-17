/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_dot1q_util.h
* @purpose     Implementaion of GARP API Functions 
* @component   GARP
* @comments    none
* @create      09/06/2005
* @author      vrynkov
* @author      
* @end
*             
**********************************************************************/

#include "commdefs.h"
#include "datatypes.h"
#include "garpapi.h"

#define GARP_LEAVE_TIMER_MIN_VAL      20
#define GARP_LEAVE_TIMER_MAX_VAL     600

#define GARP_LEAVEALL_TIMER_MIN_VAL  200
#define GARP_LEAVEALL_TIMER_MAX_VAL 6000

#define GARP_JOIN_TIMER_MIN_VAL       10
#define GARP_JOIN_TIMER_MAX_VAL      100

#define GARP_TIMER_MULTIPLICATION_VAL 3

/* garp_dot1q_util.c */  
L7_RC_t garpGlobalModeGet(L7_uint32 *mode, GARPApplication application);
L7_BOOL garpIntfModeGet(L7_uint32 intIfNum, GARPApplication application);
L7_BOOL garpGVRPIntfEnableGet(L7_uint32 intIfNum);
L7_BOOL garpGMRPIntfEnableGet(L7_uint32 intIfNum);
L7_RC_t garpIntfCfgGet(L7_uint32 intIfNum, L7_uint32 *status, 
                       GARPApplication application);
L7_RC_t garpIntfGVRPModeApply(L7_uint32 intIfNum, L7_uint32 status);
L7_RC_t garpIntfGMRPModeApply(L7_uint32 intIfNum, L7_uint32 status);
L7_RC_t garpTimeApply(L7_uint32 intIfNum, L7_uint32 new_time, 
                      GARPCommandNameTable timer);
L7_RC_t garpTimeGet(L7_uint32 intIfNum, L7_uint32 *time, 
                    GARPCommandNameTable timer);
L7_RC_t garpModeApply(L7_uint32 command, GARPApplication application);
L7_RC_t garpIntfAcquiredGet(L7_uint32 intIfNum, L7_uint32 *status);
L7_BOOL garpIntfAttachedGet(L7_uint32 intIfNum);


/* dot1dgarp.c */
L7_uint32 garpDot1sChangeCallBack(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t garpVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);

L7_RC_t garpIntfGVRPModeEnableProcess(L7_uint32 intIfNum, GARPCommand *cmd);
L7_RC_t garpIntfGVRPModeDisableProcess(L7_uint32 intIfNum, GARPCommand *cmd);
L7_RC_t garpIntfGMRPModeProcess(L7_uint32 intIfNum, L7_uint32 status, GARPCommand *cmd);

/* garpapi.c */
GARPStatus GarpBuildCommand(GARPCommand *pCmd,
                            GARPApplication app,
                            GARPCommandNameTable cmd,
                            GARPAttrType attrType,
                            GARPAttrValue pAttrValue, 
                            GARPPort port, L7_uint32 gmrpVID);
