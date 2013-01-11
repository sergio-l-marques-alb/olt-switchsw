
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_oui_database_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
*
* @component    WDM
*
* @comments     none
*
* @create       09/19/2008
*
* @author       
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_OUI_DATABASE_API_H
#define INCLUDE_WDM_OUI_DATABASE_API_H

#include "datatypes.h"
#include "osapi.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"


/****************************************
*
*  OUI Database
*
*****************************************/

typedef struct OUI_Database_s
{
   L7_OUIVal_t ouiVal;    /* AVL key */
   L7_char8           oui[L7_WDM_OUI_MAX + 1 ] ; /*Default Unknown*/
 }wsOUIDatabase_t;


typedef struct wsOUIDatabaseEntry_s
{
   wsOUIDatabase_t  OUIData;
   void *avlPtr;                            /* Reserved for AVL tree,  must be last field */
}wsOUIDatabaseEntry_t;

#define L7_WDM_DEFAULT_OUI_NAME   "Unknown"

/*********************************************************************
*
* @purpose Delete the local oui Database.
*
* @retruns void
*
* @comments
*
* @end
*
*********************************************************************/
void wdmOUIDBDelete(void);

/*********************************************************************
* @purpose  Initializes the wdm Read Only oui database with all the read only 
*           OUI entries .
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is called only during init
*
* @end
*
*********************************************************************/
L7_RC_t wdmOUIDBInit();
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
L7_RC_t wdmOUIEntryAddUpdate(L7_OUIVal_t ouiVal,L7_char8 *ouiName);


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
*           This function gives the ouiName by searching both readonly entries 
*           and local list(admin entered oui list).Local List is searched first.
* @end
*
*********************************************************************/
L7_RC_t wdmOUIEntryGet(L7_OUIVal_t ouiVal,L7_char8 *ouiName);

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
*           This function gives the ouiName by searching both readonly entries 
*           and local list(admin entered oui list).Local List is searched first.
* @end
*
*********************************************************************/
L7_RC_t wdmCommonOUIEntryGet(L7_OUIVal_t ouiVal,L7_char8 *ouiName);

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
L7_RC_t wdmLocalOUIEntryGet(L7_OUIVal_t ouiVal,L7_char8 *ouiName);

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
* @comments
*          This function looks up in local oui database first and then in the
*          RO Database
*
* @end
*
*********************************************************************/
L7_RC_t wdmOUIEntryGetNext(L7_OUIVal_t ouiVal,L7_OUIVal_t *NextouiVal,L7_char8 *ouiName);

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
L7_RC_t wdmLocalOUIEntryGetNext(L7_OUIVal_t ouiVal,L7_OUIVal_t *NextouiVal,L7_char8 *ouiName);


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
L7_RC_t wdmOUIEntryDelete(L7_OUIVal_t ouiVal);

/*********************************************************************
* @purpose  Get the Number of current entries in Local OUI Database.
*
* @param    L7_uint32 *value     @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmLocalOUIDataBaseNumOfEntriesGet(L7_uint32 *value);

#endif /* INCLUDE_WDM_OUI_DATABASE_API_H*/
