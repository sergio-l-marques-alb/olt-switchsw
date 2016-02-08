/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_oui_database.h
*
* @purpose    Wireless Data Manager OUI Database USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     10/1/2008
*
* @author     rashmi
*
* @end
*             
*********************************************************************/

#ifndef INCLUDE_USMDB_WDM_OUI_DATABASE_H
#define INCLUDE_USMDB_WDM_OUI_DATABASE_H

#include "l7_common.h"
#include "wireless_comm_structs.h"
/*********************************************************************
* @purpose  Add or update an OUI entry In Local OUI Database.
*
* @param    L7_OUIVal_t ouiVal   @b{(input)}  OUI val of client/AP.
*           L7_char8   *ouiName @b{(input)}  Organizational string for OUI .
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOUIEntryAddUpdate(L7_OUIVal_t ouiVal,L7_char8 *ouiName);


/*********************************************************************
*
* @purpose  Get the OUI Name for the OUI Value.
*
* @param      L7_OUIVal_t ouiVal   @b{(input)} OUI val of client/AP.  
*             L7_char8   *ouiName @b{(output)} pointer to store the oui string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*           This Functions gets only the admin entered OUI entries.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmLocalOUIEntryGet(L7_OUIVal_t ouiVal,L7_char8 *ouiName);

/*********************************************************************
*
* @purpose  Get the Next OUI Entry for the OUI Value.
*
* @param      L7_OUIVal_t ouiVal   @b{(input)} OUI val of client/AP.  
*             L7_char8   *ouiName @b{(output)} pointer to store the oui string.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments:
*          This Function Returns only Admin Entered OUI Entries.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmLocalOUIEntryGetNext(L7_OUIVal_t ouiVal,L7_OUIVal_t *NextouiVal,L7_char8 *ouiName);

/*********************************************************************
*
* @purpose  Delete an OUI Database entry
*
* @param      L7_OUIVal_t ouiVal   @b{(input)} OUI val of client/AP.  
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmOUIEntryDelete(L7_OUIVal_t ouiVal);

#endif /*INCLUDE_USMDB_WDM_OUI_DATABASE_H*/


