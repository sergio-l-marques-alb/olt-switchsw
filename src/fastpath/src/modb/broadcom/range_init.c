/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename init.c
 *
 * @purpose
 *
 * @component object handlers
 *
 * @comments
 *
 * @create  30 May 2007
 *
 * @author  Rama Sasthri, Kristipati
 *          Madan Mohan Goud, Kanala
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "xlib.h"
#include "commdefs.h"
#include "nimapi.h"
#include "usmdb_util_api.h"
#include "filter_exports.h"

#include "fpobj_util.h"

typedef struct myUsp
{
  int unit, slot, port;
} myUSP_t;

static xLibRC_t validateUsp(myUSP_t first,myUSP_t second)
{
  if(second.port == 0)
    return XLIBRC_SUCCESS;

  if(first.unit > L7_MAX_UNITS_PER_STACK || second.unit > L7_MAX_UNITS_PER_STACK)
    return XLIBRC_FAILURE;

  if(first.unit > second.unit)
    return XLIBRC_FAILURE;

  if(first.slot > L7_MAX_SLOTS_PER_UNIT || second.slot > L7_MAX_SLOTS_PER_UNIT)
    return XLIBRC_FAILURE;

  if(first.unit == second.unit && first.slot > second.slot)
    return XLIBRC_FAILURE;

  if(first.unit == second.unit && first.slot == second.slot && first.port > second.port)
    return XLIBRC_FAILURE;

  return XLIBRC_SUCCESS;

}


static xLibU32_t compareUsp (nimUSP_t first, nimUSP_t second)
{

  if ((first.unit == second.unit) && (first.slot == second.slot))
  {
    return second.port - first.port;
  }
  else
  {
    /* this is little bit complex
       if first.unit is lesser than second.unit by 1 then check whether the s/p of first is 1 less than s/p of second
     */
    if(second.unit - first.unit == 1)
    {
      if(first.slot == L7_MAX_SLOTS_PER_UNIT)
      {
        if(first.port == L7_MAX_PORTS_PER_SLOT && second.port == 1)
        {
          return 1;
        }
      }
    }  
    else 
    {
      return 2;
    }
  }
  return 2;/*some number other than 1*/
}

/* Encoding routine for user type usp_t */
static xLibRC_t encode_uspRange_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  nimUSP_t uspList[L7_FILTER_MAX_INTF];
  L7_INTF_MASK_t intfMask;
  xLibU32_t numPorts = 0;
  xLibU32_t i, j;
  xLibU32_t hifen = 0;;
  xLibU32_t comma = 0;;
  xLibU8_t resultBuf[1024];
  nimUSP_t uspOne, uspTwo;
  xLibStr256_t USP;
  xLibStr256_t USP_temp1;
  xLibStr256_t USP_temp2;
  xLibU32_t spCount = 0;

  memset (&intfMask, 0x0, sizeof (intfMask));
  memcpy (&intfMask.value, inBuf, inLen);
  memset (resultBuf, 0x0, 1024);
  memset (USP, 0x0, sizeof (USP));
  memset (USP_temp1, 0x0, sizeof (USP_temp1));
  memset (USP_temp2, 0x0, sizeof (USP_temp2));
  memset (uspList, 0x0, sizeof (uspList));
  memset (outBuf, 0x0, *outSizeLen);

  if (usmDbConvertMaskToList (&intfMask, intfList, &numPorts) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (numPorts == 0)
  {
    memset (outBuf, 0x0, *outSizeLen);
    memcpy (outBuf, inBuf, inLen);
    *outSizeLen = inLen;
    return XLIBRC_SUCCESS;
  }

  for (spCount = 0, i = 1; i <= numPorts; i++)
  {
    if (nimGetUnitSlotPort (intfList[i], &uspList[i-1]) != L7_SUCCESS)
    {
      break;
    }
  }
  if (i > 1)
  {
    spCount = i - 1;
  }
  i = 0;
  while (i < spCount)
  {
    uspOne = uspTwo = uspList[i];
    j = i + 1;
    hifen = comma = 0;
    while (j < spCount)
    {
      if (compareUsp (uspTwo, uspList[j]) == 1)
      {
        uspTwo = uspList[j];
        j++;
      }
      else
      {
        break;
      }
    }
    if ((j - i > 1))            /*There are consecutive s/p i.e hifen involved */
    {
      hifen = 1;
      i = j;
    }
    else if ((j - i == 1) && (j < spCount))     /*no hifen i.e consecutive interfaces not involved */
    {
      comma = 1;
      uspTwo = uspList[j];
      i = j;
    }
    else if (j >= spCount)      /*Last s/p in the list */
    {
      memset (USP, 0x0, sizeof (USP));
      fpObjPrintUSP (USP, sizeof(USP), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
      break;
    }

    if (hifen == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      fpObjPrintUSP (USP_temp2, sizeof(USP_temp2), (xLibU32_t) uspTwo.unit, (xLibU32_t) uspTwo.slot, uspTwo.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s-%s",USP_temp1, USP_temp2);
    }
    else if (comma == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s",USP_temp1);

    }
    (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
    if (i < spCount)
    {
     (void) osapiStrncat (resultBuf, ",", sizeof(resultBuf)-strlen (resultBuf)-1);
    }
    else
    {
      break;
    }

  }                             /* end for intfList */

  /*strncat(resultBuf,USP,strlen(USP)); */
  (void) osapiStrncat (resultBuf, "\0", sizeof(resultBuf)-strlen (resultBuf));
  (void)osapiStrncpy(outBuf, resultBuf, strlen (resultBuf));
  *outSizeLen = strlen (resultBuf);
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type usp_t */
static xLibRC_t encode_uspRangeFP_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  nimUSP_t uspList[L7_FILTER_MAX_INTF];
  L7_INTF_MASK_t intfMask;
  xLibU32_t numPorts = 0;
  xLibU32_t i, j;
  xLibU32_t hifen = 0;;
  xLibU32_t comma = 0;;
  xLibU8_t resultBuf[1024];
  nimUSP_t uspOne, uspTwo;
  xLibStr256_t USP;
  xLibStr256_t USP_temp1;
  xLibStr256_t USP_temp2;
  xLibU32_t spCount = 0;

  memset (&intfMask, 0x0, sizeof (intfMask));
  memcpy (&intfMask.value, inBuf, inLen);
  memset (resultBuf, 0x0, 1024);
  memset (USP, 0x0, sizeof (USP));
  memset (USP_temp1, 0x0, sizeof (USP_temp1));
  memset (USP_temp2, 0x0, sizeof (USP_temp2));
  memset (uspList, 0x0, sizeof (uspList));
  memset (outBuf, 0x0, *outSizeLen);

  if (usmDbConvertMaskToList (&intfMask, intfList, &numPorts) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (numPorts == 0)
  {
    memset (outBuf, 0x0, *outSizeLen);
    memcpy (outBuf, inBuf, inLen);
    *outSizeLen = inLen;
    return XLIBRC_SUCCESS;
  }

  for (spCount = 0, i = 1; i <= numPorts; i++)
  {
    if (nimGetUnitSlotPort (intfList[i], &uspList[i-1]) != L7_SUCCESS)
    {
      break;
    }
  }
  if (i > 1)
  {
    spCount = i - 1;
  }
  i = 0;
  while (i < spCount)
  {
    uspOne = uspTwo = uspList[i];
    j = i + 1;
    hifen = comma = 0;
    while (j < spCount)
    {
      if (compareUsp (uspTwo, uspList[j]) == 1)
      {
        uspTwo = uspList[j];
        j++;
      }
      else
      {
        break;
      }
    }
    if ((j - i > 1))            /*There are consecutive s/p i.e hifen involved */
    {
      hifen = 1;
      i = j;
    }
    else if ((j - i == 1) && (j < spCount))     /*no hifen i.e consecutive interfaces not involved */
    {
      comma = 1;
      uspTwo = uspList[j];
      i = j + 1;
    }
    else if (j >= spCount)      /*Last s/p in the list */
    {
      memset (USP, 0x0, sizeof (USP));
      fpObjPrintUSP (USP, sizeof(USP), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);

      (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
      break;
    }

    if (hifen == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      fpObjPrintUSP (USP_temp2, sizeof(USP_temp2), (xLibU32_t) uspTwo.unit, (xLibU32_t) uspTwo.slot, uspTwo.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s-%s",USP_temp1, USP_temp2);
    }
    else if (comma == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      fpObjPrintUSP (USP_temp2, sizeof(USP_temp2), (xLibU32_t) uspTwo.unit, (xLibU32_t) uspTwo.slot, uspTwo.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s,%s",USP_temp1, USP_temp2);
    }
    (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
    if (i < spCount)
    {
     (void) osapiStrncat (resultBuf, ",", sizeof(resultBuf)-strlen (resultBuf)-1);
    }
    else
    {
      break;
    }
  }                             /* end for intfList */

  /*strncat(resultBuf,USP,strlen(USP)); */
  (void) osapiStrncat (resultBuf, "\0", sizeof(resultBuf)-strlen (resultBuf));
  (void)osapiStrncpy(outBuf, resultBuf, strlen (resultBuf));
  *outSizeLen = strlen (resultBuf);
  return XLIBRC_SUCCESS;
}


static xLibRC_t encode_uspNoneRange_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  xLibU32_t intfList[L7_FILTER_MAX_INTF];
  nimUSP_t uspList[L7_FILTER_MAX_INTF];
  L7_INTF_MASK_t intfMask;
  xLibU32_t numPorts = 0;
  xLibU32_t i, j;
  xLibU32_t hifen = 0;;
  xLibU32_t comma = 0;;
  xLibU8_t resultBuf[1024];
  nimUSP_t uspOne, uspTwo;
  xLibStr256_t USP;
  xLibStr256_t USP_temp1;
  xLibStr256_t USP_temp2;
  xLibU32_t spCount = 0;

  memset (&intfMask, 0x0, sizeof (intfMask));
  memcpy (&intfMask.value, inBuf, inLen);
  memset (resultBuf, 0x0, 1024);
  memset (USP, 0x0, sizeof (USP));
  memset (USP_temp1, 0x0, sizeof (USP_temp1));
  memset (USP_temp2, 0x0, sizeof (USP_temp2));
  memset (uspList, 0x0, sizeof (uspList));
  memset (outBuf, 0x0, *outSizeLen);

  if (usmDbConvertMaskToList (&intfMask, intfList, &numPorts) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (numPorts == 0)
  {
    memset (outBuf, 0x0, *outSizeLen);
    (void)osapiStrncpy(outBuf, "None", 4);
    outBuf[5] ='\0';
    *outSizeLen = strlen (outBuf);
    return XLIBRC_SUCCESS;
  }

  for (spCount = 0, i = 1; i <= numPorts; i++)
  {
    if (nimGetUnitSlotPort (intfList[i], &uspList[i-1]) != L7_SUCCESS)
    {
      break;
    }
  }
  if (i > 1)
  {
    spCount = i - 1;
  }
  i = 0;
  while (i < spCount)
  {
    uspOne = uspTwo = uspList[i];
    j = i + 1;
    hifen = comma = 0;
    while (j < spCount)
    {
      if (compareUsp (uspTwo, uspList[j]) == 1)
      {
        uspTwo = uspList[j];
        j++;
      }
      else
      {
        break;
      }
    }
    if ((j - i > 1))            /*There are consecutive s/p i.e hifen involved */
    {
      hifen = 1;
      i = j;
    }
    else if ((j - i == 1) && (j < spCount))     /*no hifen i.e consecutive interfaces not involved */
    {
      comma = 1;
      uspTwo = uspList[j];
      i = j + 1;
    }
    else if (j >= spCount)      /*Last s/p in the list */
    {
      memset (USP, 0x0, sizeof (USP));
      fpObjPrintUSP (USP, sizeof(USP), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
      break;
    }

    if (hifen == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      fpObjPrintUSP (USP_temp2, sizeof(USP_temp2), (xLibU32_t) uspTwo.unit, (xLibU32_t) uspTwo.slot, uspTwo.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s-%s",USP_temp1, USP_temp2);
    }
    else if (comma == 1)
    {
      memset (USP, 0x0, sizeof (USP));
      memset (USP_temp1, 0x0, sizeof (USP_temp1));
      memset (USP_temp2, 0x0, sizeof (USP_temp2));
      fpObjPrintUSP (USP_temp1, sizeof(USP_temp1), (xLibU32_t) uspOne.unit, (xLibU32_t) uspOne.slot,
          (xLibU32_t) uspOne.port);
      fpObjPrintUSP (USP_temp2, sizeof(USP_temp2), (xLibU32_t) uspTwo.unit, (xLibU32_t) uspTwo.slot, uspTwo.port);
      (void) osapiSnprintf(USP, sizeof(USP), "%s,%s",USP_temp1, USP_temp2);
    }
    (void) osapiStrncat (resultBuf, USP, sizeof(resultBuf)-strlen (resultBuf)-1);
    if (i < spCount)
    {
     (void) osapiStrncat (resultBuf, ",", sizeof(resultBuf)-strlen (resultBuf)-1);
    }
    else
    {
      break;
    }

  }                             /* end for intfList */

  /*strncat(resultBuf,USP,strlen(USP)); */
  (void) osapiStrncat (resultBuf, "\0", sizeof(resultBuf)-strlen (resultBuf));
  (void)osapiStrncpy(outBuf, resultBuf, strlen (resultBuf));
  *outSizeLen = strlen (resultBuf);
  return XLIBRC_SUCCESS;
}


static xLibRC_t fillMask (L7_INTF_MASK_t * outptr, myUSP_t first, myUSP_t second)
{
  xLibU32_t tempUnit = 1;
  xLibU32_t tempSlot;
  xLibU32_t tempPort;

  xLibU32_t firstIntfNum, tempIntfNum;

  if(validateUsp(first,second) != XLIBRC_SUCCESS)
    return XLIBRC_FAILURE;

  if (usmDbIntIfNumFromUSPGet (first.unit, first.slot, first.port, &firstIntfNum) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (outptr != NULL)
  {
    if (second.port == 0)
    {
      L7_INTF_SETMASKBIT (*outptr, firstIntfNum);
    }
    else
    {
      tempUnit = first.unit;
      tempSlot = first.slot;
      tempPort = first.port;

      while (tempUnit <= second.unit )
      {
        if(tempSlot <= L7_MAX_SLOTS_PER_UNIT)
        {
          if(tempUnit == second.unit && tempSlot == second.slot)
          {
            while(tempPort <= second.port)
            {
              if (usmDbIntIfNumFromUSPGet (tempUnit, tempSlot, tempPort , &tempIntfNum) == L7_SUCCESS)
              {
                L7_INTF_SETMASKBIT (*outptr, tempIntfNum);
              }

              tempPort++;
            }
            return XLIBRC_SUCCESS;
          }
          else
          {
            while(tempPort <= L7_MAX_PORTS_PER_SLOT)
            {
              if (usmDbIntIfNumFromUSPGet (tempUnit, tempSlot, tempPort , &tempIntfNum) ==L7_SUCCESS)
              {
                L7_INTF_SETMASKBIT (*outptr, tempIntfNum);
              }

              tempPort++;
            }
          }
          tempSlot++;
          tempPort = 1;
        }
        else
        {
          tempPort = 1;
          tempSlot = 0;
          tempUnit++;
        }

      }
    }
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t parseHifen (char *comma, L7_INTF_MASK_t * outptr)
{
  char *first = NULL, *second = NULL;
  myUSP_t fUsp, sUsp;
  char *tfirst = NULL, *tsecond = NULL;

  fUsp.unit = fUsp.slot = fUsp.port = sUsp.unit = sUsp.slot = sUsp.port = 0;
  first = strtok_r (comma, "-", &second);
  if (first != NULL)
  {
    tfirst = osapiStrDup (first);
    if (second != NULL)
    {
      tsecond = osapiStrDup (second);
    }
  }

  if (tfirst != NULL)
  {
    fpObjScanUSP (tfirst, &fUsp.unit, &fUsp.slot, &fUsp.port);
    osapiFree(L7_OSAPI_COMPONENT_ID, tfirst);
  }

  if (tsecond != NULL && strcmp (tsecond, "") != 0)
  {
    fpObjScanUSP (tsecond, &sUsp.unit, &sUsp.slot, &sUsp.port);
    osapiFree(L7_OSAPI_COMPONENT_ID, tsecond);
#if 0
    if (sUsp.port < fUsp.port)
    {
      return XLIBRC_FAILURE;
    }
#endif
  }
  return fillMask (outptr, fUsp, sUsp);
}

static xLibRC_t parseHifenFP (char *comma, L7_INTF_MASK_t * outptr)
{
  char *first = NULL, *second = NULL;
  myUSP_t fUsp, sUsp;
  char *tfirst = NULL, *tsecond = NULL;

  fUsp.unit = fUsp.slot = fUsp.port = sUsp.unit = sUsp.slot = sUsp.port = 0;
  first = strtok_r (comma, "-", &second);
  if (first != NULL)
  {
    tfirst = osapiStrDup (first);
    if (second != NULL)
    {
      tsecond = osapiStrDup (second);
    }
  }

  if (tfirst != NULL)
  {
    fpObjScanUSP (tfirst, &fUsp.unit, &fUsp.slot, &fUsp.port);

    osapiFree(L7_OSAPI_COMPONENT_ID, tfirst);
  }

  if (tsecond != NULL && strcmp (tsecond, "") != 0)
  {
    fpObjScanUSP (tsecond, &sUsp.unit, &sUsp.slot, &sUsp.port);
    osapiFree(L7_OSAPI_COMPONENT_ID, tsecond);
#if 0
    if (sUsp.port < fUsp.port)
    {
      return XLIBRC_FAILURE;
    }
#endif
  }
  return fillMask (outptr, fUsp, sUsp);
}


static xLibRC_t getIntfMask (char *inptr, L7_INTF_MASK_t * outptr)
{
  char *comma, *savePtr;

  if (inptr == NULL)
  {
    return XLIBRC_FAILURE;
  }

  comma = strtok_r (inptr, ",", &savePtr);
  if (comma != NULL)
  {
    while (comma != NULL)
    {
      if (parseHifen (comma, outptr) == XLIBRC_FAILURE)
      {
        return XLIBRC_FAILURE;
      }
      comma = strtok_r (NULL, ",", &savePtr);
    }
  }
  else
  {
    return parseHifen (inptr, outptr);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t getFPIntfMask (char *inptr, L7_INTF_MASK_t * outptr)
{
  char *comma, *savePtr;

  if (inptr == NULL)
  {
    return XLIBRC_FAILURE;
  }

  comma = strtok_r (inptr, ",", &savePtr);
  if (comma != NULL)
  {
    while (comma != NULL)
    {
      if (parseHifenFP (comma, outptr) == XLIBRC_FAILURE)
      {
        return XLIBRC_FAILURE;
      }
      comma = strtok_r (NULL, ",", &savePtr);
    }
  }
  else
  {
    return parseHifenFP (inptr, outptr);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_uspRange_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_INTF_MASK_t intfMask;

  memset (&intfMask, 0x0, sizeof (L7_INTF_MASK_t));
  memset (outBuf, 0x0, *outSizeLen);
  
  *outSizeLen = 0;
  if (XLIBRC_SUCCESS == getIntfMask (inBuf, &intfMask))
  {
    memcpy (outBuf, &intfMask, sizeof (intfMask));
    *outSizeLen = sizeof (intfMask);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_uspRangeFP_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_INTF_MASK_t intfMask;

  memset (&intfMask, 0x0, sizeof (L7_INTF_MASK_t));
  memset (outBuf, 0x0, *outSizeLen);

  *outSizeLen = 0;
  if (XLIBRC_SUCCESS == getFPIntfMask (inBuf, &intfMask))
  {
    memcpy (outBuf, &intfMask, sizeof (intfMask));
    *outSizeLen = sizeof (intfMask);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_uspNoneRange_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  char inbuf[256];
  L7_INTF_MASK_t intfMask;

  memset (inbuf, 0x0, 256);
  memset (&intfMask, 0x0, sizeof (L7_INTF_MASK_t));
  memset (outBuf, 0x0, *outSizeLen);
  memcpy (inbuf, inBuf, strlen (inBuf));
  *outSizeLen = 0;
  if (XLIBRC_SUCCESS == getIntfMask (inbuf, &intfMask))
  {
    memcpy (outBuf, &intfMask, sizeof (intfMask));
    *outSizeLen = sizeof (intfMask);
  }
  return XLIBRC_SUCCESS;
}

/* inBuf contains vlan mask.convert it to string and pass it to outBuf*/

static int getNextVlanFromMask (L7_VLAN_MASK_t* vlanMask, int vid)
{
  int i = 0;
  for (i = vid + 1; i <= L7_VLAN_MAX_MASK_BIT; i++)
  {
    if (L7_VLAN_ISMASKBITSET ((*vlanMask), i))
    {
      return i;
    }
  }
  return -1;
}

static xLibRC_t printVidRange (int first, int last, char *temp, int len)
{

  if (temp == NULL)
  {
    return XLIBRC_FAILURE;
  }

  memset (temp, 0x0, len);
  if (first < last)
  {
    sprintf (temp, "%d-%d,", first, last);
  }
  else
  {
    sprintf (temp, "%d,", first);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_vlanRange_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_VLAN_MASK_t vlanMask;
  int firstvid, nextvid, tempvid;
  /*int count = 0; */
  char resultBuf[4096];
  char tempBuf[256];

  memset (resultBuf, 0x0, sizeof (resultBuf));
  memset (&vlanMask, 0x0, sizeof (vlanMask));
  memcpy (&vlanMask, inBuf, inLen);
  /* count = countNumOfVlans(vlanMask); */

  firstvid = getNextVlanFromMask (&vlanMask, 0);
  if (firstvid == -1)
  {
    return XLIBRC_FAILURE;
  }

  do
  {
    tempvid = firstvid;
    nextvid = getNextVlanFromMask (&vlanMask, firstvid);
    if (nextvid == -1)
    {
      printVidRange (firstvid, tempvid, tempBuf, sizeof (tempBuf));
      (void) osapiStrncat (resultBuf, tempBuf, sizeof(resultBuf)-strlen (resultBuf)-1);
      break;
    }
    while (nextvid - tempvid == 1)
    {
      tempvid = nextvid;
      nextvid = getNextVlanFromMask (&vlanMask, tempvid);
    }

    printVidRange (firstvid, tempvid, tempBuf, sizeof (tempBuf));
    (void) osapiStrncat (resultBuf, tempBuf, sizeof(resultBuf)-strlen (resultBuf)-1);
    firstvid = nextvid;
    if (nextvid == -1)
    {
      break;
    }

  }
  while (1);
  resultBuf[strlen (resultBuf) - 1] = '\0';
  memset (outBuf, 0x0, *outSizeLen);
  memcpy (outBuf, resultBuf, strlen (resultBuf));
  *outSizeLen = strlen (resultBuf);
  return XLIBRC_SUCCESS;
}

/*inBuf contains string convert to vlan mask*/
static xLibRC_t fillVlanMask (L7_VLAN_MASK_t * outptr, int first, int second)
{
  int i = 0;

  if (outptr != NULL)
  {
    if (second == 0)
    {
      L7_VLAN_SETMASKBIT (*outptr, first);
    }
    else
    {
      i = first;
      while (i <= second)
      {
        L7_VLAN_SETMASKBIT (*outptr, i);
        i++;
      }
    }
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t parseVlanHifen (char *comma, L7_VLAN_MASK_t * outptr)
{
  char *first = NULL, *second = NULL;
  int fVlan, sVlan;
  char *tfirst = NULL, *tsecond = NULL;

  fVlan = sVlan = 0;
  first = strtok_r (comma, "-", &second);
  if (first != NULL)
  {
    tfirst = osapiStrDup (first);
    if (second != NULL)
    {
      tsecond = osapiStrDup (second);
    }
  }

  if (tfirst != NULL)
  {
    sscanf (tfirst, "%d", &fVlan);
    osapiFree(L7_OSAPI_COMPONENT_ID, tfirst);
  }
  if (tsecond != NULL && strcmp (tsecond, "") != 0)
  {
    sscanf (tsecond, "%d", &sVlan);
    osapiFree(L7_OSAPI_COMPONENT_ID, tsecond);
    if (sVlan < fVlan)
    {
      return XLIBRC_FAILURE;
    }
  }
  return fillVlanMask (outptr, fVlan, sVlan);
}

static xLibRC_t getVlanMask (char *inptr, L7_VLAN_MASK_t * outptr)
{
  char *comma, *savePtr;

  if (inptr == NULL)
  {
    return XLIBRC_FAILURE;
  }

  comma = strtok_r (inptr, ",", &savePtr);
  if (comma != NULL)
  {
    while (comma != NULL)
    {
      if (parseVlanHifen (comma, outptr) == XLIBRC_FAILURE)
      {
        return XLIBRC_FAILURE;
      }
      comma = strtok_r (NULL, ",", &savePtr);
    }
  }
  else
  {
    return parseVlanHifen (inptr, outptr);
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t validate_string (xLibS8_t * inBuf)
{
  if (inBuf == NULL)
  {
    return XLIBRC_FAILURE;
  }
  if (strlen (inBuf) < 1)
  {
    return XLIBRC_FAILURE;
  }
  if (strcmp (inBuf, "") == 0)
  {
    return XLIBRC_FAILURE;
  }
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_vlanRange_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  char inbuf[4096];
  L7_VLAN_MASK_t vlanMask;

  memset (inbuf, 0x0, 4096);
  memset (&vlanMask, 0x0, sizeof (L7_VLAN_MASK_t));
  memset (outBuf, 0x0, *outSizeLen);
  if (validate_string (inBuf) == XLIBRC_FAILURE)
  {
    return XLIBRC_FAILURE;
  }

  if(strlen(inBuf) > sizeof(inbuf))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (inbuf, inBuf, strlen (inBuf));
  if (XLIBRC_SUCCESS == getVlanMask (inbuf, &vlanMask))
  {
    memcpy (outBuf, &vlanMask, sizeof (vlanMask));
    *outSizeLen = sizeof (vlanMask);
  }
  return XLIBRC_SUCCESS;
}

static L7_RC_t dayOfWeekGet(const char *dayName, L7_uint32 *dayNumber)
{
  L7_uint32     i;

  const L7_uchar8 *daysOfWeek[] = {
    "",
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

  for(i=1; i<=7; i++)
  {
    if(strcmp((L7_char8 *)dayName, (L7_char8 *)daysOfWeek[i]) == 0)
    {
      *dayNumber = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

void MaskToDaysOfWeek( L7_uchar8 daysOfTheWeek, L7_uchar8 *buf)
{
  if ( daysOfTheWeek & (1 << L7_DAY_SUN ))
  {
    strcat(buf,"Sunday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_MON ))
  {
    strcat(buf,"Monday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_TUE ))
  {
    strcat(buf,"Tuesday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_WED ))
  {
    strcat(buf,"Wednesday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_THU ))
  {
    strcat(buf,"Thursday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_FRI ))
  {
    strcat(buf,"Friday,");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_SAT ))
  {
    strcat(buf,"Saturday,");
  }

}

static xLibRC_t encode_dayList_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_uchar8 dayMask;
  char resultBuf[256];
  xLibU32_t val;

  memset (resultBuf, 0x0, sizeof (resultBuf));
  memset (&dayMask, 0x0, sizeof (dayMask));
  memcpy(&val, inBuf, inLen);
  dayMask = (L7_uchar8)val;
 
  MaskToDaysOfWeek(dayMask, resultBuf);

  resultBuf[(strlen(resultBuf))-1 ] = '\0';
  memset (outBuf, 0x0, *outSizeLen);
  memcpy (outBuf, resultBuf, strlen (resultBuf));
  *outSizeLen = strlen (resultBuf);
  return XLIBRC_SUCCESS;
}

static xLibRC_t getDayMask( char *inPtr, unsigned char *dayMask)
{
  L7_uint32 day;
  char *comma, *savePtr;
  comma = strtok_r(inPtr, ",", &savePtr);
  while(comma != NULL)
  {
    if (dayOfWeekGet(comma, &day) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    *dayMask |= 1 << day;
    comma = strtok_r('\0', ",", &savePtr);
  }

  return XLIBRC_SUCCESS;

}


static xLibRC_t decode_dayList_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  char inbuf[256];
  L7_uchar8 dayMask = 0;
  xLibU32_t val;

  memset (inbuf, 0x0, 256);
  memset (outBuf, 0x0, *outSizeLen);

  if(strlen(inBuf) > sizeof(inbuf))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (inbuf, inBuf, strlen (inBuf));
 
  if (strcmp(inbuf, "Daily") == 0)
  {
    dayMask = ((1 << L7_DAY_MON) | (1 << L7_DAY_TUE) | (1 << L7_DAY_WED) | \
                   (1 << L7_DAY_THU) | (1 << L7_DAY_FRI) | (1 << L7_DAY_SAT) | \
                   (1 << L7_DAY_SUN));
  }
  else if (strcmp(inbuf, "Weekend") == 0)
  {
    dayMask = ((1 << L7_DAY_SAT) | (1 << L7_DAY_SUN));
  }
  else if (strcmp(inbuf, "Weekdays") == 0)
  {
    dayMask = ((1 << L7_DAY_MON) | (1 << L7_DAY_TUE) | (1 << L7_DAY_WED) | \
                   (1 << L7_DAY_THU) | (1 << L7_DAY_FRI));

  }
  else
  {
    if (getDayMask (inbuf, &dayMask) != XLIBRC_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
  }
  val = dayMask;
  memcpy (outBuf, &val, sizeof (val));
  *outSizeLen = sizeof (val);
  return XLIBRC_SUCCESS;
}

void fpObjRangeHandlersInit (void)
{
  /* Registration for uspRange_t type */
  xLibDataMapRegister ("uspRange_t", encode_uspRange_t, decode_uspRange_t);
  xLibDataMapRegister ("uspRangeFP_t", encode_uspRangeFP_t, decode_uspRangeFP_t);
  xLibDataMapRegister ("vlanRange_t", encode_vlanRange_t, decode_vlanRange_t);
  xLibDataMapRegister ("uspNoneRange_t", encode_uspNoneRange_t, decode_uspNoneRange_t);
  xLibDataMapRegister ("dayList_t", encode_dayList_t, decode_dayList_t);
}
