
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_dot3ad_api.h
*
* @purpose SNMP specific value conversion for 802.3ad objects
*
* @component SNMP
*
* @create 06/21/2001
*
* @author Mike Fiorito
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "defaultconfig.h"
#include "sysapi.h"
#include "snmpapi.h"
#include "osapi.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_util_api.h"


/*********************************************************************
* @purpose  Gets the most recent change to the dot3adAggTable,
*           dot3adAggPortListTable or the dot3adAggPortTable and
*           converts the time value to hundredths of a second for
*           use by SNMP.
*
* @param    UnitIndex         the unit for this operation
* @param    *lastChanged      time of last change
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t
snmpDot3adTablesLastChangedGet(L7_uint32 UnitIndex, L7_uint32 *lastChanged)
{
  L7_timespec ts;

  if (usmDbDot3adTablesLastChangedGet(UnitIndex, &ts) == L7_SUCCESS)
  {
    *lastChanged  = ts.seconds;
    *lastChanged += ts.minutes * SNMP_MINUTES;  /* 60 */
    *lastChanged += ts.hours * SNMP_HOURS;      /* 60 * 60 */
    *lastChanged += ts.days * SNMP_DAYS;        /* 60 * 60 * 24 */
    *lastChanged *= SNMP_HUNDRED;               /* Convert to centiseconds */
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Check the validity of an aggregator interface
*
* @param    UnitIndex   the unit for this operation
* @param    extIfNum    external interface number
*
* @returns  L7_SUCCESS, if valid aggregator interface
* @returns  L7_FAILURE, if not valid aggregator interface
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t
snmpDot3adAggEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  return usmDbDot3adAggEntryGet(UnitIndex, intIfNum);
}

/*********************************************************************
* @purpose  Check the validity of an aggregator interface
*
* @param    UnitIndex         the unit for this operation
* @param    extIfNum          external interface number
* @param    *nextExtIfNum     next external interface number
*
* @returns  L7_SUCCESS, if valid aggregator interface
* @returns  L7_FAILURE, if not valid aggregator interface
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t
snmpDot3adAggEntryGetNext(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 tmpIntIfNum = 0;
  L7_uint32 tmpExtIfNum = 0;

  /* iterate through lag interface numbers */
  while (usmDbDot3adAggEntryGetNext(UnitIndex, tmpIntIfNum, &tmpIntIfNum) == L7_SUCCESS)
  {
    /* convert lag internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(tmpIntIfNum, &tmpExtIfNum) == L7_SUCCESS)
    {
      /* if really the next ext interface number */
      if (tmpExtIfNum > extIfNum)
      {
        *nextExtIfNum = tmpExtIfNum;
        return L7_SUCCESS;
      }
    }
  } 

  return L7_FAILURE;
}

L7_RC_t
snmpDot3adAggAggregateOrIndividualGet(L7_uint32 UnitIndex, L7_uint32 lagIntIfNum, L7_int32 *val)
{
  L7_uint32 tempVal;
  L7_RC_t rc;

  rc = usmDbDot3adAggAggregateOrIndividualGet(UnitIndex, lagIntIfNum, &tempVal);

  if (rc == L7_SUCCESS)
  {
    switch (tempVal)
    {
    case L7_TRUE:
      *val = D_dot3adAggAggregateOrIndividual_true;
      break;

    case L7_FALSE:
      *val = D_dot3adAggAggregateOrIndividual_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Check the validity of an physical interface
*
* @param    UnitIndex   the unit for this operation
* @param    extIfNum    external interface number
*
* @returns  L7_SUCCESS, if valid physical interface
* @returns  L7_FAILURE, if not valid physical interface
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t
snmpDot3adAggPortEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
    return usmDbDot3adAggPortIndexGet(UnitIndex, intIfNum);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Check the validity of an physical interface
*
* @param    UnitIndex         the unit for this operation
* @param    extIfNum          external interface number
* @param    *nextExtIfNum     next external interface number
*
* @returns  L7_SUCCESS, if valid physical interface
* @returns  L7_FAILURE, if not valid physical interface
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t
snmpDot3adAggPortEntryGetNext(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 tmpIntIfNum = 0;
  L7_uint32 tmpExtIfNum = 0;

  /* iterate through interface numbers */
  while (usmDbDot3adAggPortIndexGetNext(UnitIndex, tmpIntIfNum, &tmpIntIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(tmpIntIfNum, &tmpExtIfNum) == L7_SUCCESS)
    {
      /* if really the next ext interface number */
      if (tmpExtIfNum > extIfNum)
      {
        *nextExtIfNum = tmpExtIfNum;
        return L7_SUCCESS;
      }
    }
  } 

  return L7_FAILURE;
}

L7_RC_t
snmpDot3adAggPortAggregateOrIndividualGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 tempVal;
  L7_RC_t rc;

  rc = usmDbDot3adAggPortAggregateOrIndividualGet(UnitIndex, intIfNum, &tempVal);

  if (rc == L7_SUCCESS)
  {
    switch (tempVal)
    {
    case L7_TRUE:
      *val = D_dot3adAggPortAggregateOrIndividual_true;
      break;

    case L7_FALSE:
      *val = D_dot3adAggPortAggregateOrIndividual_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
