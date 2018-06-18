/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr.h
*
* @purpose Hardware and Software System Controller Main Header File
*
* @component cnfgr
*
* @comments none
*
* @create 09/14/2000
*
* @author bmutz
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#include "system_exports.h"


#ifndef INCLUDE_CNFGR_H
#define INCLUDE_CNFGR_H

#define CNFGR_INIT_PHASE_0       0
#define CNFGR_INIT_PHASE_1       1
#define CNFGR_INIT_PHASE_2       2
#define CNFGR_INIT_PHASE_3       3


/*********************************************************************
* @purpose  Check to see if Static MAC Filtering is supported
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL 
cnfgrIsFilteringPresent();

/*********************************************************************
* @purpose  Check to see if IP Map is present in this system configuration.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ipMapPresent();

/*********************************************************************
* @purpose  dot1x Init   
*
* @param    void    
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void cnfgrDot1x(L7_uint32 phase);

/*********************************************************************
* @purpose  double vlan tag Init
*
* @param    void    
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void cnfgrDvlantag();

/*********************************************************************
* @purpose  Check to see if a component is present.
*
* @param    L7_uint32  componentId    
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL cnfgrIsComponentPresent( L7_COMPONENT_IDS_t  componentId);

/*********************************************************************
* @purpose  Check to see if a feature is present.  A feature is a descrete
*           part of a component.
*
* @param    L7_uint32  componentId    
* @param    L7_uint32  featureId    
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL cnfgrIsFeaturePresent( L7_COMPONENT_IDS_t componentId, L7_uint32 featureId);

/*********************************************************************
* @purpose  Determines the base network processing technology for the
*           platform.
*
* @returns  L7_BASE_TECHNOLOGY_t  Base technology type value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    From an application software perspective, many individual
*           chip identifiers (rev levels, etc.) operate identically.
*       
* @end
*********************************************************************/
L7_BASE_TECHNOLOGY_TYPES_t cnfgrBaseTechnologyTypeGet(void);

/*********************************************************************
* @purpose  Determines the base network processing technology subtype
*           for the platform.
*
* @returns  L7_BASE_TECHNOLOGY_SUBTYPE_t  Base technology subtype value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    From an application software perspective, many individual
*           chip identifiers (rev levels, etc.) operate identically.
*           SUBTYPE is used to distinguish between different flavors
*           of a chip family, e.g. features like ECMP that are supported
*           in the 5695, but not the 5690, yet they share the same 
*           BASE_TECHNOLOGY
*       
* @end
*********************************************************************/
L7_BASE_TECHNOLOGY_SUBTYPES_t cnfgrBaseTechnologySubtypeGet(void);

/*********************************************************************
* @purpose  Determines the base network processing technology ID
*           for the platform.
*
* @returns  L7_NPD_ID_t
*
* @notes    This call is required to identify the exact device with in
*           a given technology type and subtype. Finer control for cnfgr
*           features.
*
* @end
*********************************************************************/
L7_NPD_ID_t cnfgrBaseTechnologyNpdTypeIdGet(void);

#endif
