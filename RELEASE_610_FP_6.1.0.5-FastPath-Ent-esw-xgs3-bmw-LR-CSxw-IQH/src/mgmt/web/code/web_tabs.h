/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\mgmt\web\web.h
*
* @purpose Support of the EmWeb code 
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

#ifndef WEB_TABS_H__
#define WEB_TABS_H__

#define TAB_IMG_LEFT             "/images/tableft.gif"
#define TAB_IMG_MID              "/images/tabmid.gif"
#define TAB_IMAGE_MID_IS_USED    L7_FALSE        
#define TAB_IMG_RIGHT            "/images/tabright.gif"
#define TAB_IMG_DOT              "/images/dot.gif"
#define TAB_IMG_ROW2             "/images/tab_spacer_row2.gif"
#define TAB_IMG_ROW2_LINE        "/images/tab_line_row2.gif"

#define TAB_IMG_HEIGHT       24
#define TAB_IMG_LEFT_WIDTH   8
#define TAB_IMG_RIGHT_WIDTH  7
#define TAB_IMG_ROW2_WIDTH   1
#define TAB_IMG_ROW2_HEIGHT  18

#define TAB_ROWS_MAX              2
#define USMWEB_APPINFO_TAB_ROW1   0
#define USMWEB_APPINFO_TAB_ROW2   1
#define TAB_MAX_COUNT            16

#define TAB_COLOR_SELECTED         "#b70024"
#define TAB_COLOR_UNSELECTED       "#c0c0c0"
#define TAB_COLOR_BACKGROUND       "#ffffff"
#define TAB_ROW2_SEPARATOR_COLOR   "#909090"

#define TAB_TEXT_COLOR_SELECTED    "#ffffff"
#define TAB_TEXT_COLOR_UNSELECTED  "#000000"

extern void usmWebTabGen(L7_char8 *name, 
                         L7_char8 *link,
                         L7_char8 *targetFrame,
                         L7_uint32 tabIndex, 
                         L7_uint32 tabSel, 
                         L7_uint32 numTabs, 
                         L7_char8 *buf, 
                         L7_uint32 buflen);

extern void usmWebTabGenRow2(L7_char8   *name, 
                             L7_char8   *link, 
                             L7_char8   *targetFrame,
                             L7_uint32   tabIndex,
                             L7_BOOL     tabSel,
                             L7_uint32   numTabs, 
                             L7_char8   *buf, 
                             L7_uint32   buflen);

extern void usmWebTabWrapperTop(L7_char8 *buf, L7_uint32 buflen);

extern void usmWebTabWrapperBottom(L7_char8 *buf, L7_uint32 buflen);

extern void usmWebTabWrapperMiddle(L7_char8 *buf, L7_uint32 buflen);


#endif
