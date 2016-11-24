/*********************************************************************
 *                                                                 
 * (C) Copyright Broadcom Corporation 2001-2006
 *
 **********************************************************************
 *
 * Name: cnfgr_sid_components.c
 *
 * Purpose: Configurator definitions for component interface function tables.
 *
 * Component: Configurator (cnfgr)
 *
 * Comments:  This files defines and initializes the function tables used 
 *            for cross component interaction.
 *            
 *
 *
 * Created by: akulkarni 03/14/2007 
 *
 *********************************************************************/

#include "string.h"
#include "l7_common.h"

/* Include the component api header files */
#include "l7_ospf_api.h"
#include "l7_rip_api.h"
#include "l7_vrrp_api.h"
#include "wireless_api.h"
#include "dhcp_snooping_api.h"

/* Define the function table for components */
ospfFuncTable_t ospfFuncTable;
ripFuncTable_t  ripFuncTable;
vrrpFuncTable_t vrrpFuncTable;
wtnnlFuncTable_t wtnnlFuncTable;
centtnnlFuncTable_t centtnnlFuncTable;
dsFuncTable_t    dsFuncTable;

/*********************************************************************
* @purpose  Initialize the function table for each component
*
* @param    None
*
* @returns  L7_SUCCESS   The function tables for the components have been cleared.
*                      
* @returns  L7_ERROR     The initialization has failed. 
*
* @notes    This function should be called only once during system initialization.
*           It clears the function table bindings for all the components and sets it
*           to a default value. The component should then specify the bindings in its
*           phase one of initialization.
*   
* @end
*********************************************************************/

L7_RC_t cnfgrSidComponentFuncTableInit()
{

    /* memset the function pointer table */
    memset(&ospfFuncTable, 0, sizeof(ospfFuncTable_t));
    memset(&ripFuncTable, 0, sizeof(ripFuncTable_t));
    memset(&vrrpFuncTable, 0, sizeof(vrrpFuncTable_t));
    memset(&wtnnlFuncTable, 0, sizeof(wtnnlFuncTable_t));
    memset(&centtnnlFuncTable, 0, sizeof(centtnnlFuncTable_t));
    memset(&dsFuncTable, 0, sizeof(dsFuncTable_t));

    return L7_SUCCESS;
}
