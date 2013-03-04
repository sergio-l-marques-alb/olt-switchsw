/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2003-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal_cnfgr.h
*
* @purpose Contains prototypes to support the configurator API
*
* @component Management Access Control and Administration List
*
* @comments
*
* @create 05/05/2005
*
* @author stamboli
* @end
*
**********************************************************************/

#ifndef INCLUDE_MACAL_CNFGR_H
#define INCLUDE_MACAL_CNFGR_H

#include "l7_cnfgr_api.h"
/*********************************************************************
*
* @purpose  CNFGR System Initialization for Management Access Control and Administration List component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the MACAL comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void macalApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);


#endif /* INCLUDE_MACAL_CNFGR_H */
