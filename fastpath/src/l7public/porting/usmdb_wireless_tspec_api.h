/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
* @filename   usmdb_wireless_tspec_api.h
*
* @purpose    Wireless TSPEC USMDB API functions
*
* @component  Wireless
*
* @comments   none
*
* @create     01/07/2009
*
* @author     gpaussa
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_USMDB_WIRELESS_TSPEC_API_H
#define INCLUDE_USMDB_WIRELESS_TSPEC_API_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Verifies whether a TSPEC access category index is supported or not.
*
* @param    L7_uint32    acindex        @b{(input)}  Access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessTspecSupportedACIndexGet(L7_uint32 acindex);

/*********************************************************************
* @purpose  Gets the first supported TSPEC access category index.
*
* @param    L7_uint32    *acindex       @b{(input)}  First access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @comments In this case, the first supported access category is voice,
*           which can be used as input to a subsequent 'NextGet' API call.
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessTspecSupportedACIndexFirstGet(L7_uint32 *acindex);

/*********************************************************************
* @purpose  Gets the last supported TSPEC access category index.
*
* @param    L7_uint32    *acindex       @b{(input)}  Last access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @comments In this case, the last supported access category is voice,
*           which can be used as input to a subsequent 'NextGet' API call.
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessTspecSupportedACIndexLastGet(L7_uint32 *acindex);

/*********************************************************************
* @purpose  Gets the next supported TSPEC access category index.
*
* @param    L7_uint32    acindex        @b{(input)}  Access category index
* @param    L7_uint32    *nextAcindex   @b{(output)} Next access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments For TSPEC access categories, the "next" value is perceived
*           as the next supported value from the following order:
*           voice, video, best-effort, background.  These are iterated
*           in decreasing numerical order based on their corresponding
*           index value.
*
* @comments Per the list above, only the SUPPORTED access categories are 
*           considered here, meaning that the access category can be 
*           configured for mandatory admission control.
*
* @end
*********************************************************************/
L7_RC_t usmDbWirelessTspecSupportedACIndexNextGet(L7_uint32 acindex,
                                                  L7_uint32 *nextAcindex);

#endif /* INCLUDE_USMDB_WIRELESS_TSPEC_API_H */
