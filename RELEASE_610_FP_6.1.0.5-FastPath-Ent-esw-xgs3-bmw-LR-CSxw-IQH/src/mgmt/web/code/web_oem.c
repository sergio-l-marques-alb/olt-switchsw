/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src\mgmt\web\web.c
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "l7_common.h"
#include "web.h"
#include "ews_api.h"
#include "ewnet.h"
#include "log.h"

#if 0
L7_char8  *usmWebOEMTokenGet( L7_char8 *t );
L7_char8  *usmWebOEMStringGet( L7_int32 stringID );

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
L7_char8 *usmWebOEMTokenGet(char *t)
{
  L7_int32  stringID;

  stringID = atoi(t);

  return (usmWebOEMStringGet(stringID));

}

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
L7_char8 *usmWebOEMStringGet(L7_int32 stringID)
{
  L7_uint32 top, bot, mid;

  top = usmOEMNLSLength-1;
  bot = 0;

  while (top >= bot)
  {
    mid = (top + bot) / 2;
    if (usmOEMEnglish[mid].id == stringID)
      break;
    else if (usmOEMEnglish[mid].id < stringID)
    {
      bot = mid+1;
    }
    else
    {
      top = mid-1;
      if (mid == 0)
      {
        printf("\n ERROR : NLS String not found! ID = %d\n", stringID);
        return("++ERROR++");
      }
    }
  }

  if (top >= bot)
    return(usmOEMEnglish[mid].string);
  else
  {
    printf("\n ERROR : NLS String not found! ID = %d\n", stringID);
    return("++ERROR++");
  }

}

/*********************************************************************
 *
 * @purpose Debug util to scan the OEM NLS structure for duplicates.
 *
 * @end
 *
 *********************************************************************/
void debugMgmtOEMNLSCheckForDuplicates(void)
{
  L7_uint32 checkID = 0;
  L7_uint32 i = 0;
  L7_uint32 numMatches = 0;

  for (checkID=0; checkID < usmOEMNLSLength; checkID++)
  {
    numMatches = 0;
    for (i=0; i < usmOEMNLSLength; i++)
    {
      if (usmOEMEnglish[i].id == usmOEMEnglish[checkID].id)
        numMatches++;
    }
    if (numMatches > 1)
      printf("Found %d matches for NLS string id %d\n", numMatches, usmOEMEnglish[checkID].id);
  }
}
#endif

