/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\policy\std_policy_api.h
*
* @purpose Contains LVL7 standard policy offerings
*
* @component 
*
* @comments 
*
* @create 04/02/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_STD_POLICY_API_H
#define INCLUDE_STD_POLICY_API_H

#include "statsapi.h"

#ifdef STD_POLICY_API_GLOBALS
  #define STD_POLICY_API_EXT
#else
  #define STD_POLICY_API_EXT extern
#endif 

#define STD_POLICY_MUTANT_COUNT 1
#define STD_POLICY_MUTLING_COUNT 1

STD_POLICY_API_EXT mutling_id_t mutlingRxTotalDiscards[STD_POLICY_MUTLING_COUNT];
STD_POLICY_API_EXT mutling_parm_list_t mutantRxTotalDiscards;
STD_POLICY_API_EXT statsParm_entry_t stdPolicyMutants[STD_POLICY_MUTANT_COUNT];

/*********************************************************************
* @purpose  Determine if the interface type is valid in policy
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL policyIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in policy
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL policyIsValidIntf(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Enable or disable flow control for an interface
*
* @param    intf        physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t policyIntfFlowCtrlModeSet(L7_uint32 intf, L7_uint32 mode);


/*********************************************************************
* @purpose  Get flow control for an interface
*
* @param    intf        physical or logical interface to be configured
*                       or MAX_INTERFACE_COUNT to apply configuration
*                       to all interfaces
*
* @param    mode        admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t policyIntfFlowCtrlModeGet(L7_uint32 intf, L7_uint32 *mode);


/*********************************************************************
* @purpose  Sets the Unit's System Flow Control Mode  
*
* @param    mode  System Flow Control Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemFlowControlModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Returns the Unit's System Flow Control Mode 
*
* @param    void  
*
* @returns  mode  System Flow Control Mode (L7_ENABLE or L7_DISABLE)
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemFlowControlModeGet(void);


/*********************************************************************
* @purpose  Sets the Unit's System Broadcast Storm Mode  
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Sets the Unit's System Broadcast Storm Threshold  
*
* @param    threshold  System Broadcast Storm Threshold  
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormThresholdSet(L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);


/*********************************************************************
* @purpose  Sets the Unit's System Multicast Storm Mode  
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Sets the Unit's System Multicast Storm Threshold  
*
* @param    threshold  System Broadcast Storm Threshold  
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormThresholdSet(L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Sets the Unit's System Destination lookup failure Storm Mode  
*
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormModeSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Sets the Unit's System Destination lookup failure Storm Threshold  
*
* @param    threshold  System Broadcast Storm Threshold  
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormThresholdSet(L7_uint32 threshold,
                                                    L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Sets an interface's System Broadcast Storm Mode  
*
* @param    interface  interface to act one  
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
* @purpose  Sets an interface's System Broadcast Storm Threshold  
*
* @param    interface  interface to act on
* @param    threshold  System Broadcast Storm Threshold 
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Sets an interface's Unit's System Multicast Storm Mode  
*
* @param    interface  interface to act one  
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormModeIntfSet(L7_uint32 interface, L7_uint32 mode);


/*********************************************************************
* @purpose  Sets an interface's System Multicast Storm Threshold  
*
* @param    interface  interface to act on
* @param    threshold  System Broadcast Storm Threshold
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormThresholdIntfSet(L7_uint32 interface, L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);

/*********************************************************************
* @purpose  Sets an interace's System Destination lookup failure Storm Mode  
*
* @param    interface  interface to act one  
* @param    mode  System Broadcast Storm Mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormModeIntfSet(L7_uint32 interface,L7_uint32 mode);


/*********************************************************************
* @purpose  Sets an interace's System Destination lookup failure Storm Threshold  
*
* @param    interface  interface to act on
* @param    threshold  System Broadcast Storm Threshold
*
* @param    rate_unit  Threshold rate unit it percent, pps, or kbps  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormThresholdIntfSet(L7_uint32 interface,L7_uint32 threshold,
                                                        L7_RATE_UNIT_t rate_unit);



/*********************************************************************
* @purpose  Returns the Unit's System Broadcast Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormModeGet(void);


/*********************************************************************
* @purpose  Returns the Unit's System Multicast Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormModeGet(void);


/*********************************************************************
* @purpose  Returns the Unit's System Destination lookup failure Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormModeGet(void);


/*********************************************************************
* @purpose  Returns an interface's Broadcast Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemBcastStormModeIntfGet(L7_uint32 interface);


/*********************************************************************
* @purpose  Returns an interface's Multicast Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemMcastStormModeIntfGet(L7_uint32 interface);


/*********************************************************************
* @purpose  Returns an interface's Destination lookup failure Storm Mode 
*
* @param    void  
*
* @returns  mode  System Broadcast Storm Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_uint32 policySystemUcastStormModeIntfGet(L7_uint32 interface);




/*********************************************************************
* @purpose  Returns the Unit's System Broadcast Storm threshold 
*
* @param    *threshold  System Broadcast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemBcastStormThresholdGet(L7_uint32 *threshold,
                                                  L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
* @purpose  Returns the Unit's System Multicast Storm threshold 
*
* @param    *threshold  System Multicast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemMcastStormThresholdGet(L7_uint32 *threshold,
                                                  L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
* @purpose  Returns the Unit's System Destination lookup failure Storm threshold 
*
* @param    *threshold  System Unknown Unicast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemUcastStormThresholdGet(L7_uint32 *threshold,
                                                  L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
* @purpose  Returns an interface's Broadcast Storm threshold 
*
* @param    interface   Interface to get information on
* @param    *threshold  Broadcast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemBcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                      L7_RATE_UNIT_t *rate_unit);


/*********************************************************************
* @purpose  Returns an interface's Multicast Storm threshold 
*
* @param    interface   Interface to get information on
* @param    *threshold  Multicast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemMcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                      L7_RATE_UNIT_t *rate_unit);



/*********************************************************************
* @purpose  Returns an interface's Destination lookup failure Storm threshold 
*
* @param    interface   Interface to get information on
* @param    *threshold  Unknown Unicast Storm Threshold
* @param    *rate_unit  Threshold units in PERCENT or PPS
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t policySystemUcastStormThresholdIntfGet(L7_uint32 interface, L7_uint32 *threshold,
                                                      L7_RATE_UNIT_t *rate_unit);


#endif
