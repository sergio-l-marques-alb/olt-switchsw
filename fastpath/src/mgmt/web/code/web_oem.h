/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/base/web_oem.h
 *
 * @purpose Code in support of the various EmWeb html pages
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/12/2000
 *
 * @author tgaunce
 * @end
 *
 **********************************************************************/

#ifndef WEB_OEM_H__
#define WEB_OEM_H__

/*********************************************************************
*
* @purpose Find a match on an NLS string.
*
* @param t id of the string to search for
*
* @returns pointer to NLS string
*
* @returns NULL pointer if no match found
*
* @end
*
*********************************************************************/
L7_char8 *usmWebOEMTokenGet(char * t);

/*********************************************************************
*
* @purpose Find a match on an NLS string.
*
* @param t id of the string to search for
*
* @returns pointer to NLS string
*
* @returns NULL pointer if no match found
*
* @end
*
*********************************************************************/
L7_char8 *usmWebOEMStringGet(L7_int32 stringID);

/*********************************************************************
*
* @purpose Debug util to scan the OEM NLS structure for duplicates.
*
* @end
*
*********************************************************************/
void debugMgmtOEMNLSCheckForDuplicates(void);

/*********************************************************************
*
* @purpose Get the page header part1 (this one has the title)
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_char8 *usmWebPageHeader1stGet(L7_uint32 pageTitleID);

/*********************************************************************
*
* @purpose Get the page header part1 (this one has the help link)
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_char8 *usmWebPageHeader2ndGet(L7_uint32 helpLinkID);

/*********************************************************************
*
* @purpose Get the page footer
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_char8 *usmWebPageFooterGet();

#endif
