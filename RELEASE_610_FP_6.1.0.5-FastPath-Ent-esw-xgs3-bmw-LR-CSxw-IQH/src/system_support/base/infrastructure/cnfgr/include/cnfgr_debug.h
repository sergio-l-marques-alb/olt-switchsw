/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: cnfgr_debug.h
*
* Purpose: Configurator component debug header file
*
* Component: Configurator (cnfgr)
*
* Comments:
*
* Created by: msmith 07/01/2003 
*
*********************************************************************/
#ifndef INCLUDE_CNFGR_DBG_H
#define INCLUDE_CNFGR_DBG_H


/*********************************************************************
* @purpose  This function initializes Debug module. The use of this  
*           function is mandatory. This function is a Debug interface.
*
* @param    None
*
* @returns  L7_SUCCESS - Always
*
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
L7_RC_t cnfgrDebugInitialize(void);

/*********************************************************************
* @purpose  This function terminates the Debug module. The use of this  
*           function is optional. This function is a Debug interface.
*
* @param    None
*
* @returns  None.          
*
* @notes    None                                              
*
*       
* @end
*********************************************************************/
void cnfgrDebugFini(void);

/*********************************************************************
* @purpose  This function dumps information about a component.       
*
* @param    cid - @a{(input)} Component id 
*
* @returns  nothing
*
* @notes    none
*       
* @end
*********************************************************************/
void cnfgrDebugCnfgrDataShow(L7_COMPONENT_IDS_t cid);

/*********************************************************************
* @purpose  This function dumps information about all components.
*
* @param    nothing
*
* @returns  nothing
*
* @notes    none
*       
* @end
*********************************************************************/
void cnfgrDebugCnfgrDataShowAll();

#endif /* INCLUDE_CNFGR_DBG_H */
