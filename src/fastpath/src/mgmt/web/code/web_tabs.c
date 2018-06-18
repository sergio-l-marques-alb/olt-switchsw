/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\mgmt\web\base\web_tabs.c
*
* @purpose Code in support of the tabbed navigation
*
* @component unitmgr
*
* @comments tba
*
* @create 09-Mar-2006
*
* @author tgaunce
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "l7_common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "web_tabs.h"
#include "osapi.h"



/* Markup should look somethign like this : 

  <td id="tabl0" background="images/tableft.gif" bgcolor="#50c0e0">
      <img src="images/dot.gif" width=8 height=24 border=0></td>

  <td id=tabm0 background="images/middle.gif" bgcolor="#50c0e0">
  <a onclick=tabClick(0,3) id="tablink" href="profile_summ.html" target=istatus> Summary </a></td>

  <td id="tabr0" background="images/tabright.gif" bgcolor="#50c0e0">
      <img src="images/dot.gif" width=7 height=24 border=0></td>
*/

void usmWebTabGen(L7_char8   *name, 
                  L7_char8   *link, 
                  L7_char8   *targetFrame,
                  L7_uint32   tabIndex,
                  L7_BOOL     tabSel,
                  L7_uint32   numTabs, 
                  L7_char8   *buf, 
                  L7_uint32   buflen)
{
  L7_char8  tabcolor[16];
  L7_char8  textcolor[16];
  L7_char8  tbuf[256];
/*
  printf("\n Building tab for tab index %d", tabIndex); 
  printf("\n   Name = %s", name); 
  printf("\n   Link = %s", link); 
  printf("\n   targetFrame = %s", targetFrame); 
  printf("\n   tabIndex = %d", tabIndex);
  if (tabSel == L7_TRUE)
    printf("\nSelected"); 
  else
    printf("\nNOT Selected"); 
  printf("\n   numTabs = %d", numTabs); 
  printf("\n   buflen = %d\n", buflen); 
*/
  /*   Need to get the selected tab from the query string.  If null query 
       string, select the first tab.
  */

  if (tabIndex > TAB_MAX_COUNT)
  {
    osapiStrncpy(buf, "<TD>ERROR_MAX16</TD>", buflen-1);
    buf[buflen-1] = '\0';
    return;
  }

  if (tabSel == L7_TRUE)
  {
    osapiStrncpy(tabcolor, TAB_COLOR_SELECTED, sizeof(tabcolor)-1);
    tabcolor[sizeof(tabcolor)-1] = '\0';
    osapiStrncpy(textcolor, TAB_TEXT_COLOR_SELECTED, sizeof(textcolor)-1);
    textcolor[sizeof(textcolor)-1] = '\0';
  }
  else
  {
    osapiStrncpy(tabcolor, TAB_COLOR_UNSELECTED, sizeof(tabcolor)-1);
    tabcolor[sizeof(tabcolor)-1] = '\0';
    osapiStrncpy(textcolor, TAB_TEXT_COLOR_UNSELECTED, sizeof(textcolor)-1);
    textcolor[sizeof(textcolor)-1] = '\0';
  }

  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td id=\"tabl%d\" background=\"%s\" bgcolor=\"%s\"><img src=\"%s\" width=%d height=%d border=0></TD>",
                              tabIndex, TAB_IMG_LEFT, tabcolor, TAB_IMG_DOT, TAB_IMG_LEFT_WIDTH, TAB_IMG_HEIGHT); 
  osapiStrncpy(buf, tbuf, buflen-1);
  buf[buflen-1] = '\0';

  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td nowrap id=\"tabm%d\"", tabIndex);
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
  if (TAB_IMAGE_MID_IS_USED == L7_TRUE)
  {
    osapiSnprintf(tbuf, sizeof(tbuf), "background=\"%s\"", TAB_IMG_MID);
    osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
  }
  osapiSnprintf(tbuf, sizeof(tbuf), "bgcolor=\"%s\"><a style=\"color: %s\" id=\"tablink%d\" href=\"%s\" target=\"%s\"> %s </a></td>",
                              tabcolor, textcolor, tabIndex, link, targetFrame, name); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));


  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td id=\"tabr%d\" background=\"%s\" bgcolor=\"%s\"><img src=\"%s\" width=%d height=%d border=0></TD>\n",
                              tabIndex, TAB_IMG_RIGHT, tabcolor, TAB_IMG_DOT, TAB_IMG_RIGHT_WIDTH, TAB_IMG_HEIGHT); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
}


void usmWebTabGenRow2(L7_char8   *name, 
                      L7_char8   *link, 
                      L7_char8   *targetFrame,
                      L7_uint32   tabIndex,
                      L7_BOOL     tabSel,
                      L7_uint32   numTabs, 
                      L7_char8   *buf, 
                      L7_uint32   buflen)
{
  L7_char8  tabcolor[16];
  L7_char8  textcolor[16];
  L7_char8  tbuf[512];

  if (tabIndex > TAB_MAX_COUNT)
  {
    osapiStrncpy(buf, "<TD>ERROR_MAX16</TD>", buflen-1);
    buf[buflen-1] = '\0';
    return;
  }

  if (tabSel == L7_TRUE)
  {
    osapiStrncpy(tabcolor, TAB_COLOR_SELECTED, sizeof(tabcolor)-1);
    tabcolor[sizeof(tabcolor)-1] = '\0';
    osapiStrncpy(textcolor, TAB_TEXT_COLOR_SELECTED, sizeof(textcolor)-1);
    textcolor[sizeof(textcolor)-1] = '\0';
  }
  else
  {
    osapiStrncpy(tabcolor, TAB_COLOR_UNSELECTED, sizeof(tabcolor)-1);
    tabcolor[sizeof(tabcolor)-1] = '\0';
    osapiStrncpy(textcolor, TAB_TEXT_COLOR_UNSELECTED, sizeof(textcolor)-1);
    textcolor[sizeof(textcolor)-1] = '\0';
  }

  /*

<td id="tabl0" bgcolor="#c81016"><img src="spacer_red.gif" border="0" height="18" width="1"></td>
<td id="tabm0" bgcolor="#c81016" nowrap="nowrap" align="center"><a style="color: rgb(255, 255, 255);" id="tab2link0" href="http://10.254.24.29/wireless/approfile/profile_management_cfg.html?profileID=2" target="basefrm"> Global </a></td>
<td id="tabr0" bgcolor="#c81016" align="right"><img src="spacer_dark.gif" border="0" height="18" width="1"></td>

  */

  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td nowrap id=\"tabl%d\" bgcolor=\"%s\">&nbsp;</TD>", tabIndex, tabcolor); 
  osapiStrncpy(buf, tbuf, buflen-1);
  buf[buflen-1] = '\0';

  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td nowrap id=\"tabm%d\"", tabIndex);
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));

  osapiSnprintf(tbuf, sizeof(tbuf), "bgcolor=\"%s\"  align=\"center\"><a style=\"color: %s\" id=\"tab2link%d\" href=\"%s\" target=\"%s\"> %s </a></td>",
                              tabcolor, textcolor, tabIndex, link, targetFrame, name); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));


  osapiSnprintf(tbuf, sizeof(tbuf), "\n<td nowrap id=\"tabr%d\" bgcolor=\"%s\" align=right><img src=\"%s\" width=%d height=%d border=0></TD>\n",
                              tabIndex, tabcolor, TAB_IMG_ROW2, TAB_IMG_ROW2_WIDTH, TAB_IMG_ROW2_HEIGHT); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
}



void usmWebTabWrapperTop(L7_char8 *buf, L7_uint32 buflen)
{
  osapiSnprintf(buf, buflen, "\n<table width=\"100%%\" border=0 cellpadding=0 cellspacing=0 bgcolor=\"%s\">\n", TAB_COLOR_BACKGROUND); 
  osapiStrncat(buf, "<tr><td><img src=\"/images/spacer_h4.gif\" width=1 height=4 border=0></td></tr>\n", (buflen-1-strlen(buf)));
  osapiStrncat(buf, "</table>\n", (buflen-1-strlen(buf)));
  osapiStrncat(buf, "<table cellpadding=0 cellspacing=0 border=0 bgcolor=\"#ffffff\"><tr>\n", (buflen-1-strlen(buf)));
}


void usmWebTabWrapperMiddle(L7_char8 *buf, L7_uint32 buflen)
{
  L7_char8  tbuf[256];

  osapiStrncpy(buf, "\n</tr>\n</table>\n", buflen-1);
  buf[buflen-1] = '\0';
  osapiSnprintf(tbuf, sizeof(tbuf), "\n<table bgcolor=\"%s\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%%\">",
               TAB_ROW2_SEPARATOR_COLOR); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
  osapiSnprintf(tbuf, sizeof(tbuf), "\n<tr><td bgcolor=\"%s\"><img src=\"%s\" border=\"0\" height=\"1\" width=\"1\"></td></tr>",
               TAB_ROW2_SEPARATOR_COLOR, TAB_IMG_ROW2_LINE); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
  osapiStrncat(buf, "</table>", (buflen-1-strlen(buf)));
  osapiSnprintf(tbuf, sizeof(tbuf), "\n<table cellpadding=0 cellspacing=0 border=0 bgcolor=\"%s\" width=\"100%%\"><tr>\n",
               TAB_COLOR_BACKGROUND); 
  osapiStrncat(buf, tbuf, (buflen-1-strlen(buf)));
}


void usmWebTabWrapperBottom(L7_char8 *buf, L7_uint32 buflen)
{
  osapiStrncpy(buf, "\n</tr>\n</table>\n", buflen-1);
  buf[buflen-1] = '\0';
}

char *substring(size_t start, size_t stop, const char *src, char *dst, size_t size)
{
   int count = stop - start;
   if ( count >= --size )
   {
      count = size;
   }
   sprintf(dst, "%.*s", count, src + start);
   return dst;
}


