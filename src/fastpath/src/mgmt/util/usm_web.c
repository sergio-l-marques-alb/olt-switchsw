/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/emweb/web/base/web.c
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

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_base_common.h"
#include "strlib_base_web.h"
#include "strlib_qos_common.h"
#include "strlib_qos_web.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_nim_api.h"
#include "usm_web.h"
#include "ews_api.h"
#include "ews.h"
#include "ewnet.h"
#include "ew_form.h"
#include "log.h"

#ifndef L7_XWEB_PACKAGE
/**************************************************************************
 * Function: usmWebNumSFPsGet
 *
 * Description:
 *     Return the number of sfps for this platform.
 *
 * Parameters:
 *
 * Return:
 *
 * Notes:
 *************************************************************************/

L7_RC_t usmWebNumSFPsGet(L7_uint32 * numSFPs)
{
  L7_uint32 intIfNum;
  L7_uint32 ifIndex;
  L7_uint32 phyCapability;
  L7_uint32 sfpCount = 0;

  for (intIfNum=1; intIfNum <= L7_MAX_PORT_COUNT; intIfNum++)
  {
    /* Check to see that the interface exists */
    if (usmDbIfIndexGet(0, intIfNum, &ifIndex) == L7_SUCCESS)
    {
      if (usmDbIntfPhyCapabilityGet(intIfNum, &phyCapability) == L7_SUCCESS)
      {
        if (phyCapability & L7_PHY_CAP_PORTSPEED_SFP)
        {
          sfpCount++;
      }
    }
  }
  }
  *numSFPs = sfpCount;

  return L7_SUCCESS;
}

static L7_char8  buf1024[1024];

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
L7_char8 *usmWebPageHeader1stGet(L7_char8 *tokStr)
{
  memset(buf1024, 0, sizeof(buf1024));
  sprintf(buf1024, "%s %s %s %s %s",
      "<table cellpadding=0 cellspacing=0 CLASS=\"tableroundcorners\">\n",
      "<tr><td class=\"pageheaderattributes\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>\n",
      "<td width=100% nowrap class=\"pageheaderattributes\">", tokStr, "</td>\n");

  return buf1024;
}

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
L7_char8 *usmWebPageHeader2ndGet(L7_char8 *tokStr)
{
  memset(buf1024, 0, sizeof(buf1024));
  sprintf(buf1024, "%s %s %s %s %s",
      "<td bgcolor=\"#c81016\" align=right valign=middle><a ", tokStr, " target=\'_blank\'>\n",
      "<img src=\'/base/images/help.gif\' width=68 height=30 border=0 alt=\'Help Pages\'></a></td></tr>\n",
      "<tr><td COLSPAN=3>&nbsp;</td></TR><tr><td colspan=3>\n");

  return buf1024;
}

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
L7_char8 *usmWebPageFooterGet()
{
  memset(buf1024, 0, sizeof(buf1024));
  sprintf(buf1024, "%s %s",
      "</TD></TR><tr><td COLSPAN=3>&nbsp;</td></TR><tr>\n",
      "</tr></table>\n");

  return buf1024;
}
#endif

