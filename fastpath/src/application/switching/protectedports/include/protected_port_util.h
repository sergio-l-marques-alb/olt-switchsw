/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_util.h
*
* @purpose   protected port utility file
*
* @component protectedPort
*
* @comments 
*
* @create    6/6/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/
#ifndef PROTECTED_PORT_UTIL_H
#define PROTECTED_PORT_UTIL_H

/*-------------------------------------*/
/*  Protected Port INFO STRUCTURES                */
/*-------------------------------------*/

typedef struct protectedPortIntfInfo_s
{
  /* Bit mask indicating the components which have acquired
   * the interface */
  AcquiredMask        acquiredList;
} protectedPortIntfInfo_t;


/* Begin Function Prototypes */

L7_BOOL protectedPortIntfConfigCanApplyToHw(L7_uint32 intIfNum);
L7_BOOL protectedPortIntfIsAttached(L7_uint32 intIfNum);
L7_BOOL protectedPortIntfIsAcquired(L7_uint32 intIfNum);
L7_RC_t protectedPortGroupIntfAddApply(L7_uint32 groupId, L7_uint32 intIfNum);  
L7_RC_t protectedPortGroupIntfDeleteApply(L7_uint32 groupId,L7_uint32 intIfNum); 
L7_BOOL protectedPortIntfIsConfigurable(L7_uint32 groupId, L7_uint32 intIfNum,
                                        protectedPortGroup_t ** pCfg);  
L7_RC_t protectedPortIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent,
                                        NIM_CORRELATOR_t correlator);
void protectedPortStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

void protectedPortIntfMaskChange(L7_INTF_MASK_t *intfMask);
L7_RC_t protectedPortGroupCreateApply(L7_uint32 groupId, L7_INTF_MASK_t intfMask);
L7_RC_t protectedPortGroupDeleteApply(L7_uint32 groupId);

/* protected port debug.h */
       /* Begin Function Prototypes */
void protectedPortBuildTestConfigData(void);                                       
void protectedPortConfigDataTestShow(void);
void protectedPortBuildTestData(protectedPortGroup_t * pCfg, L7_uint32 intIfNum);
L7_RC_t protectedPortCfgDump();
void protectedPortDebugInfoShow ();
void protectedPortMapDebugHelp(void);


/* End Function Prototypes */
#endif /* PROTECTED_PORT_UTIL_H */
