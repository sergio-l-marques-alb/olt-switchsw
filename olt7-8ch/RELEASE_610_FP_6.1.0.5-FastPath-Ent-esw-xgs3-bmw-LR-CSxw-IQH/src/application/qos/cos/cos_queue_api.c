/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_queue_api.c
*
* @purpose    COS component queue configuration API implementation
*
* @component  cos
*
* @comments   none
*
* @create     03/12/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cos_api.h"

#include "cos_util.h"
#include "cos_config.h"
#include "log.h"

/* local function prototypes */
static void cosQueueThreshStringFormat(const L7_uchar8 val[], L7_uchar8 *pStr);


/*********************************************************************
* @purpose  Verify specified queue config interface index exists
*
* @param    intIfNum    @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfIndexGet(L7_uint32 intIfNum)
{
  return cosIntfIndexGet(intIfNum);
}

/*********************************************************************
* @purpose  Determine next sequential queue config interface index
*
* @param    intIfNum    @b{(input)}  Internal interface number to begin search
* @param    *pNext      @b{(output)} Ptr to next internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfIndexGetNext(L7_uint32 intIfNum, L7_uint32 *pNext)
{
  return cosIntfIndexGetNext(intIfNum, pNext);
}

/*********************************************************************
* @purpose  Determine if the specified interface is valid for COS queue
*           config
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
L7_BOOL cosQueueIntfIsValid(L7_uint32 intIfNum)
{
  /* treat global mode L7_ALL_INTERFACES as valid value */
  if (intIfNum != L7_ALL_INTERFACES)
  {
    if (cosQueueIntfIndexGet(intIfNum) != L7_SUCCESS)
      return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Verify specified queue ID index exists
*
* @param    queueId     @b{(input)}  Queue id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIdIndexGet(L7_uint32 queueId)
{
  if ((queueId >= L7_QOS_COS_QUEUE_ID_MIN) && 
      (queueId <= L7_QOS_COS_QUEUE_ID_MAX))
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential queue ID index
*
* @param    queueId     @b{(input)}  Queue id
* @param    *pNext      @b{(output)} Ptr to next queue ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIdIndexGetNext(L7_uint32 queueId, L7_uint32 *pNext)
{
  if (queueId >= L7_QOS_COS_QUEUE_ID_MAX)
    return L7_FAILURE;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  if (queueId < L7_QOS_COS_QUEUE_ID_MIN)
    *pNext = L7_QOS_COS_QUEUE_ID_MIN;
  else
    *pNext = queueId + 1;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Verify specified queue drop precedence level index exists
*
* @param    dropPrec    @b{(input)}  Drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropPrecIndexGet(L7_uint32 dropPrec)
{
  /* Allow max+1 for non-TCP traffic */
  if ((dropPrec >= L7_QOS_COS_DROP_PREC_LEVEL_MIN) && 
      (dropPrec <= (L7_QOS_COS_DROP_PREC_LEVEL_MAX+1)))
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential queue drop precedence level index
*
* @param    dropPrec    @b{(input)}  Drop precedence level
* @param    *pNext      @b{(output)} Ptr to next drop precedence level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropPrecIndexGetNext(L7_uint32 dropPrec, L7_uint32 *pNext)
{
  /* Allow max+1 for non-TCP traffic */
  if (dropPrec >= (L7_QOS_COS_DROP_PREC_LEVEL_MAX+1))
    return L7_FAILURE;

  if (pNext == L7_NULLPTR)
    return L7_FAILURE;

  if (dropPrec < L7_QOS_COS_DROP_PREC_LEVEL_MIN)
    *pNext = L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  else
    *pNext = dropPrec + 1;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Restore default settings for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDefaultsRestore(L7_uint32 intIfNum)
{
  L7_cosCfgParms_t  *pCfg;
  L7_cosQueueCfg_t  queueCfgOrig[L7_COS_INTF_QUEUE_MAX_COUNT];

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* save original queue config to check if data changed due to restore */
  memcpy(queueCfgOrig, pCfg->queue, sizeof(queueCfgOrig));

  cosDefaultQueueConfigAllBuild(pCfg->queue);

  if (cosConfigIntfQueueCfgDataApply(intIfNum, pCfg) != L7_SUCCESS)
  {
    LOG_MSG("Unable to apply COS queue defaults on intf %u\n", intIfNum);
  }

  if (memcmp(queueCfgOrig, pCfg->queue, sizeof(queueCfgOrig)) != 0)
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Restore default settings for all queues globally on all interfaces
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cosQueueDefaultsGlobalRestore(void)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_cosQueueCfg_t  queueCfgOrig[L7_COS_INTF_QUEUE_MAX_COUNT];
  L7_uint32         intIfNum;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* restore defaults for each configurable interface
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueDefaultsRestore(intIfNum) != L7_SUCCESS)
      {
        LOG_MSG("Unable to restore global COS queue defaults on intf %u\n", 
                intIfNum);
      }
    }
  }

  /* save original queue config to check if data changed due to restore */
  memcpy(queueCfgOrig, pCfgGlob->queue, sizeof(queueCfgOrig));

  /* set the global queue config to its defaults */
  cosDefaultQueueConfigAllBuild(pCfgGlob->queue);

  if (memcmp(queueCfgOrig, pCfgGlob->queue, sizeof(queueCfgOrig)) != 0)
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the COS egress shaping rate for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to intf shaping rate output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateGet(L7_uint32 intIfNum, L7_uint32 *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;
  
  *pVal = pCfg->intf.intfShapingRate;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the COS egress shaping rate for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Intf shaping rate value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfg;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  /* a rate of 0 is used to turn off rate limiting and is acceptible regardless of units */
  if (0 != val)
  {
    if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      /* check proposed value */
      if ((val < L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN) ||
          (val > L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX))
        return L7_FAILURE;
  
    }
    else
    {
      /* check proposed value */
      if ((val < L7_QOS_COS_INTF_SHAPING_RATE_MIN) ||
          (val > L7_QOS_COS_INTF_SHAPING_RATE_MAX))
        return L7_FAILURE;
  
     /* check percentage as an integral step size amount between min, max */
     if ((val != L7_QOS_COS_INTF_SHAPING_RATE_MIN) &&
         (val != L7_QOS_COS_INTF_SHAPING_RATE_MAX) &&
         ((val % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
       return L7_FAILURE;
    }
  }

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->intf.intfShapingRate == val)
    return L7_SUCCESS;

  /* apply the new shaping rate along with the other intf config parms */
  (void)cosQueueIntfConfigApply(intIfNum, val, pCfg->intf.queueMgmtTypePerIntf,
                                pCfg->intf.wredDecayExponent); /* rc ignored here */

  pCfg->intf.intfShapingRate = val;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the COS interface parameters for this interface
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    intfShapingRate @b{(input)}  Interface shaping rate in kbps
* @param    intfShapingBurstSize @b{(input)}  Interface shaping burst size in kbits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingStatusGet(L7_uint32 intIfNum, 
                              L7_uint32 *intfShapingRate,
                              L7_uint32 *intfShapingBurstSize)
{
  if ((intIfNum == L7_ALL_INTERFACES) || 
      (intfShapingRate == L7_NULLPTR) || 
      (intfShapingRate == L7_NULLPTR))
    return L7_FAILURE;

  return(cosIntfShapingStatusGet(intIfNum, intfShapingRate, intfShapingBurstSize));
}

/*************************************************************************
* @purpose  Set the COS egress shaping rate globally for all interfaces
*
* @param    val         @b{(input)}  Intf shaping rate value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueIntfShapingRateGlobalSet(L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum;

  /* a rate of 0 is used to turn off rate limiting and is acceptible regardless of units */
  if (0 != val)
  {
    if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      /* check proposed value */
      if ((val < L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN) ||
          (val > L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX))
        return L7_FAILURE;
    }
    else
    {
      /* check proposed value */
      if ((val < L7_QOS_COS_INTF_SHAPING_RATE_MIN) ||
          (val > L7_QOS_COS_INTF_SHAPING_RATE_MAX))
        return L7_FAILURE;
  
     /* check percentage as an integral step size amount between min, max */
     if ((val != L7_QOS_COS_INTF_SHAPING_RATE_MIN) &&
         (val != L7_QOS_COS_INTF_SHAPING_RATE_MAX) &&
         ((val % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
       return L7_FAILURE;
    }
  }

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueIntfShapingRateSet(intIfNum, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global COS shaping rate (%u) on intf %u\n",
                val, intIfNum);
      }
    }
  }

  if (pCfgGlob->intf.intfShapingRate != val)
  {
    pCfgGlob->intf.intfShapingRate = val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the COS queue management type for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to mgmt type output value    N... return FAILURE from else?\n");
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfGet(L7_uint32 intIfNum, 
                                   L7_QOS_COS_QUEUE_MGMT_TYPE_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *pVal = (L7_QOS_COS_QUEUE_MGMT_TYPE_t)pCfg->intf.queueMgmtTypePerIntf;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the COS queue management type for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    val         @b{(input)}  Queue mgmt type value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfSet(L7_uint32 intIfNum, 
                                   L7_QOS_COS_QUEUE_MGMT_TYPE_t val)
{
  L7_RC_t                   rc;
  L7_cosCfgParms_t          *pCfg;
  L7_uint32                 i;
  L7_qosCosDropParmsList_t  dropParms;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (val == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED) 
  {
      return L7_SUCCESS;
  }
  /* check proposed value */
  if ((val != L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) &&
      (val != L7_QOS_COS_QUEUE_MGMT_TYPE_WRED))
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->intf.queueMgmtTypePerIntf == (L7_uchar8)val)
    return L7_SUCCESS;

  /* apply per-queue tail drop or WRED parms for all queues
   *
   * NOTE:  Applying the interface config will activate the queue mgmt mode
   *        in the device, so apply these parms first.
   */
  rc = cosQueueDropParmsListGet(intIfNum, &dropParms);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("Unable to obtain COS drop parms for intf %u\n", intIfNum);
    return L7_FAILURE;
  }

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    dropParms.queue[i].mgmtType = val;
  } /* endfor i */
  rc = cosQueueDropParmsListSet(intIfNum, &dropParms);

  /* apply the new mgmt type along with the other intf config parms
   *
   * NOTE:  The rc value is used as a flag for calling this apply function,
   *        but does not indicate the overall function exit value.
   */
  if (rc == L7_SUCCESS)
  {
    (void)cosQueueIntfConfigApply(intIfNum, pCfg->intf.intfShapingRate,
                                  val, pCfg->intf.wredDecayExponent); /* rc ignored here */
  }

  pCfg->intf.queueMgmtTypePerIntf = (L7_uchar8)val;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the COS queue management type globally for all interfaces
*
* @param    val         @b{(input)}  Queue mgmt type value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is for the per-interface config object, which is only
*           valid when the device does not support per-queue config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypePerIntfGlobalSet(L7_QOS_COS_QUEUE_MGMT_TYPE_t val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum;

  /* check proposed value */
  if ((val != L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) &&
      (val != L7_QOS_COS_QUEUE_MGMT_TYPE_WRED))
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueMgmtTypePerIntfSet(intIfNum, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global COS queue mgmt type \'%s\' on intf %u\n",
                cosQueueMgmtTypeStr[val], intIfNum);
      }
    }
  }

  if (pCfgGlob->intf.queueMgmtTypePerIntf != (L7_uchar8)val)
  {
    pCfgGlob->intf.queueMgmtTypePerIntf = (L7_uchar8)val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the WRED decay exponent for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to decay exponent output value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentGet(L7_uint32 intIfNum, L7_uint32 *pVal)
{
  L7_cosCfgParms_t  *pCfg;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *pVal = pCfg->intf.wredDecayExponent;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the WRED decay exponent for this interface
*
* @param    intIfNum    @b{(input)} Internal interface number     
* @param    val         @b{(input)} Decay exponent value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfg;

  /* NOTE:  Cannot check system-only feature id here, since this 'set' 
   *        function is commonly used by 'globalSet'.  The only place
   *        to do this feature checking is in USMDB.
   */

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  /* check proposed value */
  if ((val < L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN) ||
      (val > L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX))
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* this is a no-op if currently configured value is same as new value */
  if (pCfg->intf.wredDecayExponent == (L7_uchar8)val)
    return L7_SUCCESS;

  /* apply the new mgmt type along with the other intf config parms */
  (void)cosQueueIntfConfigApply(intIfNum, pCfg->intf.intfShapingRate,
                                pCfg->intf.queueMgmtTypePerIntf, val);  /* rc ignored here */

  pCfg->intf.wredDecayExponent = (L7_uchar8)val;
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the WRED decay exponent globally for all interfaces
*
* @param    val         @b{(input)} Decay exponent value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cosQueueWredDecayExponentGlobalSet(L7_uint32 val)
{
  L7_cosCfgParms_t  *pCfgGlob, *pCfgIntf;
  L7_uint32         intIfNum;

  /* check proposed value */
  if ((val < L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN) ||
      (val > L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX))
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueWredDecayExponentSet(intIfNum, val) != L7_SUCCESS)
      {
        LOG_MSG("Unable to set global COS WRED decay exponent (%u) on intf %u\n",
                val, intIfNum);
      }

      /* NOTE:  Not checking L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID
       *        here, as this feature ID is more appropriate for the UI than
       *        HAPI.  Since the WRED decay exponent value is part of the 
       *        overall interface config, whose other fields may be 
       *        configurable on a per-interface basis, it does not make sense
       *        to special case this here in an attempt to stop after the
       *        first interface is applied.
       */
    }
  }

  if (pCfgGlob->intf.wredDecayExponent != (L7_uchar8)val)
  {
    pCfgGlob->intf.wredDecayExponent = (L7_uchar8)val;
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the minimum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to min bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthListGet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uint32         i;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pVal->bandwidth[i] = pCfg->queue[i].minBwPercent;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the minimum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to min bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthListSet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal)
{
  L7_cosCfgParms_t              *pCfg;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i, totalBw;
  L7_BOOL                       minChanged;
  L7_cosQueueSchedParms_t       qParms;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  totalBw = 0;
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->bandwidth[i] < L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) ||
        (pVal->bandwidth[i] > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX))
      return L7_FAILURE;

    /* check percentage as an integral step size amount between min, max */
    if ((pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) &&
        (pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX) &&
        ((pVal->bandwidth[i] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
      return L7_FAILURE;

    totalBw += pVal->bandwidth[i];
  }

  /* sum of individual min bandwidth values cannot exceed defined maximum */
  if (totalBw > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* need to do the following:
   *   1. create local copy of 'list' values from queue config arrays
   *   2. flag any change in currently configured values
   *   3. enforce rule:  setting new min > nonzero max forces max to same value
   */
  minChanged = L7_FALSE;
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfg->queue[i];

    qParms.minBwList.bandwidth[i] = pQ->minBwPercent;
    qParms.maxBwList.bandwidth[i] = pQ->maxBwPercent;
    qParms.schedTypeList.schedType[i] = pQ->schedulerType;

    if (qParms.minBwList.bandwidth[i] != pVal->bandwidth[i])
    {
      qParms.minBwList.bandwidth[i] = pVal->bandwidth[i];
      minChanged = L7_TRUE;

      /* adjust non-zero max bw upwards if now less than new min bw */
      if (qParms.maxBwList.bandwidth[i] != 0)
      {
        if (qParms.maxBwList.bandwidth[i] < qParms.minBwList.bandwidth[i])
          qParms.maxBwList.bandwidth[i] = qParms.minBwList.bandwidth[i];
      }
    }
  } /* endfor i */

  if (minChanged == L7_FALSE)
    return L7_SUCCESS;

  /* apply the new minBw values along with the other queue config parms */
  (void)cosQueueSchedConfigApply(intIfNum, &qParms);  /* rc ignored here */

  /* update the COS queue config from the temporary local list info */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfg->queue[i];
    pQ->minBwPercent = /*(L7_uchar8)*/ qParms.minBwList.bandwidth[i];   /* PTin modified */
    pQ->maxBwPercent = /*(L7_uchar8)*/ qParms.maxBwList.bandwidth[i];   /* PTin modified */
  }
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the minimum bandwidth list for all queues globally on all
*           interfaces
*
* @param    *pVal        @b{(input)}  Ptr to min bandwidth list    
* @param    *pListMask   @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMinBandwidthGlobalListSet(L7_qosCosQueueBwList_t *pVal, 
                                          L7_qosCosQueueListMask_t *pListMask)
{
  L7_cosCfgParms_t        *pCfgGlob, *pCfgIntf;
  L7_cosQueueCfg_t        *pQ;
  L7_uint32               i, totalBw, intIfNum;
  L7_qosCosQueueBwList_t  minBwIntf;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;
  if (pListMask == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  totalBw = 0;
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->bandwidth[i] < L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) ||
        (pVal->bandwidth[i] > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX))
      return L7_FAILURE;

    /* check percentage as an integral step size amount between min, max */
    if ((pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) &&
        (pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX) &&
        ((pVal->bandwidth[i] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
      return L7_FAILURE;

    totalBw += pVal->bandwidth[i];
  }
  /* sum of individual min bandwidth values cannot exceed defined upper limit */
  if (totalBw > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueMinBandwidthListGet(intIfNum, &minBwIntf) == L7_SUCCESS)
      {
        for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        {
          /* update interface with global parms indicated as being set */
          if (pListMask->setMask[i] == L7_TRUE)
            minBwIntf.bandwidth[i] = pVal->bandwidth[i];
        }

        if (cosQueueMinBandwidthListSet(intIfNum, &minBwIntf) != L7_SUCCESS)
        {
          LOG_MSG("Unable to set global COS minimum bandwidth config on intf %u\n",
                  intIfNum);
        }
      }
      else 
      {
        LOG_MSG("Unable to get COS minimum bandwidth config from intf %u\n",
                intIfNum);
      }
    }
  }

  /* look for changes to global config values */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfgGlob->queue[i];
    if (pQ->minBwPercent != /*(L7_uchar8)*/ pVal->bandwidth[i])   /* PTin modified: COS */
    {
      pQ->minBwPercent = /*(L7_uchar8)*/ pVal->bandwidth[i];      /* PTin modified: COS */
      pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      /* if new minBw > nonzero maxBw, force maxBw to same value */
      if (pQ->maxBwPercent != 0)
      {
        if (pQ->maxBwPercent < pQ->minBwPercent)
          pQ->maxBwPercent = pQ->minBwPercent;
      }
    }
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the maximum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to max bandwidth output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthListGet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uint32         i;

  if (pVal == L7_NULLPTR) {
    return L7_FAILURE;
  }

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS) {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pVal->bandwidth[i] = pCfg->queue[i].maxBwPercent;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the maximum bandwidth list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to max bandwidth list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthListSet(L7_uint32 intIfNum,
                                    L7_qosCosQueueBwList_t *pVal)
{
  L7_cosCfgParms_t              *pCfg;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i;
  L7_BOOL                       maxChanged;
  L7_cosQueueSchedParms_t       qParms;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->bandwidth[i] < L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN) ||
        (pVal->bandwidth[i] > L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX))
      return L7_FAILURE;

    /* check percentage as an integral step size amount between min, max */
    if ((pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN) &&
        (pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX) &&
        ((pVal->bandwidth[i] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
      return L7_FAILURE;

    /* any nonzero max must not be less than existing min */
    if (pVal->bandwidth[i] != 0)
      if (pVal->bandwidth[i] < pCfg->queue[i].minBwPercent)
        return L7_FAILURE;
  }

  /* need to do the following:
   *   1. create local copy of 'list' values from queue config arrays
   *   2. flag any change in currently configured values
   */
  maxChanged = L7_FALSE;
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfg->queue[i];

    qParms.minBwList.bandwidth[i] = pQ->minBwPercent;
    qParms.maxBwList.bandwidth[i] = pQ->maxBwPercent;
    qParms.schedTypeList.schedType[i] = pQ->schedulerType;

    if (qParms.maxBwList.bandwidth[i] != pVal->bandwidth[i])
    {
      qParms.maxBwList.bandwidth[i] = pVal->bandwidth[i];
      maxChanged = L7_TRUE;
    }
  }

  if (maxChanged == L7_FALSE)
    return L7_SUCCESS;

  /* apply the new maxBw values along with the other queue config parms */
  (void)cosQueueSchedConfigApply(intIfNum, &qParms);  /* rc ignored here */

  /* update the COS queue config from the temporary local list info */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pCfg->queue[i].maxBwPercent = /*(L7_uchar8)*/ qParms.maxBwList.bandwidth[i];  /* PTin modified */

  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the maximum bandwidth list for all queues globally on all
*           interfaces
*
* @param    *pVal       @b{(input)}  Ptr to max bandwidth list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMaxBandwidthGlobalListSet(L7_qosCosQueueBwList_t *pVal,
                                          L7_qosCosQueueListMask_t *pListMask)
{
  L7_cosCfgParms_t        *pCfgGlob, *pCfgIntf;
  L7_cosQueueCfg_t        *pQ;
  L7_uint32               i, intIfNum;
  L7_uint32               maxBwNew;
  L7_qosCosQueueBwList_t  maxBwIntf;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;
  if (pListMask == L7_NULLPTR)
    return L7_FAILURE;

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->bandwidth[i] < L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN) ||
        (pVal->bandwidth[i] > L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX))
      return L7_FAILURE;

    /* check percentage as an integral step size amount between min, max */
    if ((pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN) &&
        (pVal->bandwidth[i] != L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX) &&
        ((pVal->bandwidth[i] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
      return L7_FAILURE;

    /* pre-check global and all intfs for new nonzero max < existing min
     * since this is an invalid config
     *
     * NOTE: Skip this checking for list values not being set, since it
     *       could cause an error condition to be detected incorrectly.
     */
    if (pListMask->setMask[i] != L7_TRUE)
      continue;

    maxBwNew = pVal->bandwidth[i];

    if (maxBwNew == 0)
      continue;

    if (maxBwNew < pCfgGlob->queue[i].minBwPercent)
      return L7_FAILURE;

    intIfNum = 0;
    while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
    {
      /* only work with configurable interfaces */
      if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
      {
        if (maxBwNew < pCfgIntf->queue[i].minBwPercent)
          return L7_FAILURE;
      }
    } /* endwhile */
  } /* endfor i */

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueMaxBandwidthListGet(intIfNum, &maxBwIntf) == L7_SUCCESS)
      {
        for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        {
          /* update interface with global parms indicated as being set */
          if (pListMask->setMask[i] == L7_TRUE)
            maxBwIntf.bandwidth[i] = pVal->bandwidth[i];
        }

        if (cosQueueMaxBandwidthListSet(intIfNum, &maxBwIntf) != L7_SUCCESS)
        {
          LOG_MSG("Unable to set global COS maximum bandwidth config on intf %u\n",
                  intIfNum);
        }
      }
      else 
      {
        LOG_MSG("Unable to get COS maximum bandwidth config from intf %u\n",
                intIfNum);
      }
    }
  }

  /* look for changes to global config values
   *
   * NOTE:  The max >= min requirement was tested above 
   */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfgGlob->queue[i];
    if (pQ->maxBwPercent != /*(L7_uchar8)*/ pVal->bandwidth[i])   /* PTin modified: COS */
    {
      pQ->maxBwPercent = /*(L7_uchar8)*/ pVal->bandwidth[i];      /* PTin modified: COS */
      pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
    }
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the scheduler type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to scheduler type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeListGet(L7_uint32 intIfNum,
                                     L7_qosCosQueueSchedTypeList_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uint32         i;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pVal->schedType[i] = pCfg->queue[i].schedulerType;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the scheduler type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to scheduler type list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeListSet(L7_uint32 intIfNum,
                                     L7_qosCosQueueSchedTypeList_t *pVal)
{
  L7_cosCfgParms_t              *pCfg;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i;
  L7_BOOL                       valChanged;
  L7_cosQueueSchedParms_t       qParms;
  L7_RC_t                       rc;


  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->schedType[i] != L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT) &&
        (pVal->schedType[i] != L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED))
      return L7_FAILURE;
  }

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* need to do the following:
   *   1. create local copy of 'list' values from queue config arrays
   *   2. flag any change in currently configured values
   */
  valChanged = L7_FALSE;
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfg->queue[i];

    qParms.minBwList.bandwidth[i] = pQ->minBwPercent;
    qParms.maxBwList.bandwidth[i] = pQ->maxBwPercent;
    qParms.schedTypeList.schedType[i] = pQ->schedulerType;

    if (qParms.schedTypeList.schedType[i] != pVal->schedType[i])
    {
      qParms.schedTypeList.schedType[i] = pVal->schedType[i];
      valChanged = L7_TRUE;
    }
  }

  if (valChanged == L7_FALSE)
    return L7_SUCCESS;

  /* apply the new schedType values along with the other queue config parms */
  rc = cosQueueSchedConfigApply(intIfNum, &qParms);  /* rc ignored here */
#if defined(FEAT_METRO_CPE_V1_0)
  if (rc == L7_FAILURE)
  {
    return rc;
  }
#endif

  /* update the COS queue config from the temporary local list info */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pCfg->queue[i].schedulerType = (L7_uchar8)qParms.schedTypeList.schedType[i];

  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the scheduler type list for all queues globally on all
*           interfaces
*
* @param    *pVal       @b{(input)}  Ptr to scheduler type list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueSchedulerTypeGlobalListSet(L7_qosCosQueueSchedTypeList_t *pVal,
                                           L7_qosCosQueueListMask_t *pListMask)
{
  L7_cosCfgParms_t              *pCfgGlob, *pCfgIntf;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i, intIfNum;
  L7_qosCosQueueSchedTypeList_t schedTypeIntf;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;
  if (pListMask == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->schedType[i] != L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT) &&
        (pVal->schedType[i] != L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED))
      return L7_FAILURE;
  }

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueSchedulerTypeListGet(intIfNum, &schedTypeIntf) == L7_SUCCESS)
      {
        for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        {
          /* update interface with global parms indicated as being set */
          if (pListMask->setMask[i] == L7_TRUE)
            schedTypeIntf.schedType[i] = pVal->schedType[i];
        }

        if (cosQueueSchedulerTypeListSet(intIfNum, &schedTypeIntf) != L7_SUCCESS)
        {
          LOG_MSG("Unable to set global COS scheduler type config on intf %u\n",
                  intIfNum);
          #if defined(FEAT_METRO_CPE_V1_0)
           return L7_FAILURE; 
          #endif
        }
      }
      else 
      {
        LOG_MSG("Unable to get COS scheduler type config from intf %u\n",
                intIfNum);
      }
    }
  }

  /* look for changes to global config values */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfgGlob->queue[i];
    if (pQ->schedulerType != (L7_uchar8)pVal->schedType[i])
    {
      pQ->schedulerType = (L7_uchar8)pVal->schedType[i];
      pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
    }
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the queue management type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to queue mgmt type output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeListGet(L7_uint32 intIfNum,
                                L7_qosCosQueueMgmtTypeList_t *pVal)
{
  L7_cosCfgParms_t  *pCfg;
  L7_uint32         i;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    pVal->mgmtType[i] = pCfg->queue[i].queueMgmtType;

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the queue management type list for all queues on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to queue mgmt type list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeListSet(L7_uint32 intIfNum,
                                L7_qosCosQueueMgmtTypeList_t *pVal)
{
  L7_RC_t                       rc;
  L7_cosCfgParms_t              *pCfg;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i;
  L7_qosCosDropParmsList_t      dropParms;
  L7_BOOL                       something_differed = L7_FALSE;

  if (intIfNum == L7_ALL_INTERFACES)
    return L7_FAILURE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if ((pVal->mgmtType[i] != L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) &&
        (pVal->mgmtType[i] != L7_QOS_COS_QUEUE_MGMT_TYPE_WRED) && 
        (pVal->mgmtType[i] != L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED))
      return L7_FAILURE;
  }

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* need to do the following:
   *   1. apply tail drop or WRED parms to activate queue management mode
   *   2. update configuration with changed values
   */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfg->queue[i];

    /* skip processing if value has not changed */
    if (pVal->mgmtType[i] == pQ->queueMgmtType)
      continue;
    if (pVal->mgmtType[i] == L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED) 
      continue;

    /* update the COS queue config from the temporary local list info */
    pQ->queueMgmtType = (L7_uchar8)pVal->mgmtType[i];
    pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
    something_differed = L7_TRUE;
  } /* endfor i */
  if (something_differed == L7_TRUE) 
  {
      rc = cosQueueDropParmsListGet(intIfNum, &dropParms);
      if (rc == L7_SUCCESS) 
      {
          cosQueueDropParmsApply(intIfNum, &dropParms);
      }
      else
      {
        LOG_MSG("Unable to obtain COS drop parms for intf %u\n", intIfNum);
      }
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the queue management type list for all queues globally on 
*           all interfaces
*
* @param    *pVal       @b{(input)}  Ptr to queue mgmt type list    
* @param    *pListMask  @b{(input)}  Mask indicating which list items changed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is for the per-queue config object, which is only
*           valid when the device does not support per-interface config.
*
* @comments Only updates global values for list items whose corresponding
*           *pListMask value is set to L7_TRUE.  All list values are 
*           considered valid for error checking, etc.
*
* @end
*********************************************************************/
L7_RC_t cosQueueMgmtTypeGlobalListSet(L7_qosCosQueueMgmtTypeList_t *pVal,
                                      L7_qosCosQueueListMask_t *pListMask)
{
  L7_cosCfgParms_t              *pCfgGlob, *pCfgIntf;
  L7_cosQueueCfg_t              *pQ;
  L7_uint32                     i, intIfNum;
  L7_qosCosQueueMgmtTypeList_t  mgmtTypeIntf;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;
  if (pListMask == L7_NULLPTR)
    return L7_FAILURE;

  /* check proposed value list */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
      if (pListMask->setMask[i] == L7_TRUE) 
      {
          if ((pVal->mgmtType[i] != L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) &&
            (pVal->mgmtType[i] != L7_QOS_COS_QUEUE_MGMT_TYPE_WRED))
          return L7_FAILURE;
      }
  }

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
      if (cosQueueMgmtTypeListGet(intIfNum, &mgmtTypeIntf) == L7_SUCCESS)
      {
        for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        {
          /* update interface with global parms indicated as being set */
          if (pListMask->setMask[i] == L7_TRUE)
            mgmtTypeIntf.mgmtType[i] = pVal->mgmtType[i];
        }

        if (cosQueueMgmtTypeListSet(intIfNum, &mgmtTypeIntf) != L7_SUCCESS)
        {
          LOG_MSG("Unable to set global COS queue mgmt type on intf %u\n",
                  intIfNum);
        }
      }
      else 
      {
        LOG_MSG("Unable to get COS queue mgmt type config from intf %u\n",
                intIfNum);
      }
    }
  }

  /* look for changes to global config values */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    pQ = &pCfgGlob->queue[i];
    if (pQ->queueMgmtType != (L7_uchar8)pVal->mgmtType[i])
    {
      pQ->queueMgmtType = (L7_uchar8)pVal->mgmtType[i];
      pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
    }
  }

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the queue WRED / taildrop config parms list for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(output)} Ptr to drop parms output list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsListGet(L7_uint32 intIfNum,
                                 L7_qosCosDropParmsList_t *pVal)
{
  L7_BOOL             maxThreshSupp = L7_TRUE;
  L7_cosCfgParms_t    *pCfg;
  L7_cosDropPrecCfg_t *pDP;
  L7_uint32           queueIndex, precIndex;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) != L7_TRUE)
    maxThreshSupp = L7_FALSE;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  for (queueIndex=0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
  {
      pVal->queue[queueIndex].mgmtType = pCfg->queue[queueIndex].queueMgmtType;
      for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
      {
        pDP = &pCfg->queue[queueIndex].dropPrec[precIndex];
        pVal->queue[queueIndex].minThreshold[precIndex]  = pDP->wredMinThresh;
        pVal->queue[queueIndex].wredMaxThreshold[precIndex]  = 
            (maxThreshSupp == L7_TRUE) ? pDP->wredMaxThresh : 0;
        pVal->queue[queueIndex].tailDropMaxThreshold[precIndex] = pDP->tdropThresh;
        pVal->queue[queueIndex].dropProb[precIndex] = pDP->wredDropProb;
      }
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Set the queue WRED / taildrop config parms list for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    *pVal       @b{(input)}  Ptr to drop parms list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only handles individual intIfNum values, not L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsListSet(L7_uint32 intIfNum, 
                                 L7_qosCosDropParmsList_t *pVal)
{
  L7_BOOL                   maxThreshSupp = L7_TRUE;
  L7_cosCfgParms_t          *pCfg;
  L7_uint32                 queueIndex, precIndex;
  L7_qosCosDropParmsList_t  dropParms;
  L7_RC_t                   rc;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) != L7_TRUE)
    maxThreshSupp = L7_FALSE;

  if (pVal == L7_NULLPTR)
        return L7_FAILURE;

  if (cosQueueDropParmsValidate(pVal) != L7_SUCCESS)
      return L7_FAILURE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
      return L7_FAILURE;

  /* this is a no-op if currently configured values are same as new values */
  if (cosQueueDropParmsDiffer(pVal, pCfg) == L7_FALSE)
      return L7_SUCCESS;

  /* copy incoming parm list locally and set the maxThreshold values
   * to 0 if maxThresh feature not supported
   */
  memcpy(&dropParms, pVal, sizeof(dropParms));
  if (maxThreshSupp == L7_FALSE)
  {
      for(queueIndex=0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
      {
          for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++) 
          {
              dropParms.queue[queueIndex].wredMaxThreshold[precIndex] = 0;
          }
      }
  }

  /* apply the new drop parms for this queue (use local copy here!)
   */
  rc = cosQueueDropParmsApply(intIfNum, &dropParms);

  cosQueueDropConfigUpdate(pVal, pCfg);
  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return rc;
}

/*************************************************************************
* @purpose  Set the queue WRED / taildrop config parms list globally for all interfaces
*
* @param    *pVal       @b{(input)}  Ptr to drop parms list    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropParmsGlobalListSet(L7_qosCosDropParmsList_t *pVal)                                       
{
  L7_cosCfgParms_t          *pCfgGlob, *pCfgIntf;
  L7_uint32                 intIfNum;

  if (pVal == L7_NULLPTR)
    return L7_FAILURE;
  
  if (cosQueueDropParmsValidate(pVal) != L7_SUCCESS) 
  {
      return L7_FAILURE;
  }

  /* make sure global config can be referenced */
  if (cosCfgPtrFind(L7_ALL_INTERFACES, &pCfgGlob) != L7_SUCCESS)
    return L7_FAILURE;

  /* update each configurable interface with this global value
   *
   * NOTE:  Favoring simplicity over robustness here.  Not worrying
   *        about trying to undo a partially-applied config change
   *        when a failure occurs on an individual interface.
   */
  intIfNum = 0;
  while (cosQueueIntfIndexGetNext(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* only work with configurable interfaces when changing global config */
    if (cosCfgPtrFind(intIfNum, &pCfgIntf) == L7_SUCCESS)
    {
        cosQueueDropParmsApply(intIfNum, pVal);
	if (cosQueueDropParmsDiffer(pVal, pCfgIntf) != L7_FALSE)
	{
	  cosQueueDropConfigUpdate(pVal, pCfgIntf);
	  pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
	}
    }
    else 
    {
        LOG_MSG("Unable to get COS taildrop/WRED config from intf %u\n", 
                intIfNum);
    }
  }
 
  if (cosQueueDropParmsDiffer(pVal, pCfgGlob) != L7_FALSE) 
  {
      cosQueueDropConfigUpdate(pVal, pCfgGlob);
      pCosCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Restore default settings of all WRED / taildrop 
*           config parms on this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue ID to de-configure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_RC_t cosQueueDropDefaultsRestore(L7_uint32 intIfNum, L7_uint32 queueId)
{
  L7_RC_t                     rc;
  L7_cosCfgParms_t            *pCfg;
  L7_cosQueueCfg_t            queueCfgDefaults;
  L7_qosCosDropParmsList_t    dropParmsList;
  L7_uint32                   queueIndex, precIndex;
  L7_QOS_COS_QUEUE_MGMT_TYPE_t curMgmtType;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* Get current mgmt type so we don't inadvertently change it */
  rc = cosQueueDropParmsListGet(intIfNum, &dropParmsList);
 
  if (rc != L7_SUCCESS)
    return rc;

  curMgmtType = dropParmsList.queue[queueId].mgmtType;

  /* build defaults for target queue in a local structure */
  cosDefaultQueueConfigBuild(&queueCfgDefaults, queueId);
  /* create a parms list that is used for the 'set' function */
  memset(&dropParmsList, 0, sizeof(dropParmsList));

  for (queueIndex = 0; queueIndex < L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
  {
      if (queueIndex != queueId) {
          dropParmsList.queue[queueIndex].mgmtType = L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED;
          continue;
      }
      dropParmsList.queue[queueIndex].mgmtType = curMgmtType;
      for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
      {
          dropParmsList.queue[queueIndex].tailDropMaxThreshold[precIndex] = queueCfgDefaults.dropPrec[precIndex].tdropThresh;
          dropParmsList.queue[queueIndex].wredMaxThreshold[precIndex] = queueCfgDefaults.dropPrec[precIndex].wredMaxThresh;
          dropParmsList.queue[queueIndex].minThreshold[precIndex]  = queueCfgDefaults.dropPrec[precIndex].wredMinThresh;
          dropParmsList.queue[queueIndex].dropProb[precIndex]  = queueCfgDefaults.dropPrec[precIndex].wredDropProb;
      }
  }
  /* set the queue config WRED parameter defaults */
  if (intIfNum == L7_ALL_INTERFACES)
    rc = cosQueueDropParmsGlobalListSet(&dropParmsList);
  else
    rc = cosQueueDropParmsListSet(intIfNum, &dropParmsList);
 
  if (rc != L7_SUCCESS)
  {
    COS_PRT(COS_MSGLVL_HI, "\nFailed to restore default COS taildrop/WRED parms "
            "on intf %u, queue %u\n", intIfNum, queueId);
  }

  return rc;
}

/*************************************************************************
* @purpose  Get the number of configurable queues per port
*
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueNumQueuesPerPortGet(L7_uint32 *pVal)
{
  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  *pVal = (L7_uint32)L7_MAX_CFG_QUEUES_PER_PORT;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the number of drop precedence levels supported per queue
*
* @param    *pVal       @b{(output)} Ptr to output value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cosQueueNumDropPrecLevelsGet(L7_uint32 *pVal)
{
  if (pVal == L7_NULLPTR)
    return L7_FAILURE;

  *pVal = (L7_uint32)L7_MAX_CFG_DROP_PREC_LEVELS;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Check if specified queue mgmt type is active for the queue
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    queueId     @b{(input)}  Queue id
* @param    qMgmtType   @b{(input)}  Desired queue mgmt type value  
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Internal helper function, not part of COS queue config API.
*
* @comments An intIfNum of L7_ALL_INTERFACES denotes global config operation.
*
* @end
*********************************************************************/
L7_BOOL cosQueueMgmtTypeIsActive(L7_uint32 intIfNum, L7_uint32 queueId,
                                 L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtType)
{
  L7_cosCfgParms_t              *pCfg;
  L7_QOS_COS_QUEUE_MGMT_TYPE_t  mgmtType;

  if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) ||
      (queueId > L7_QOS_COS_QUEUE_ID_MAX))
    return L7_FALSE;

  if (cosCfgPtrFind(intIfNum, &pCfg) != L7_SUCCESS)
    return L7_FALSE;

  /* determine which config field to check based on feature support */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, 
                            L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
  {
    /* use per-intf config parm */
    mgmtType = (L7_QOS_COS_QUEUE_MGMT_TYPE_t)pCfg->intf.queueMgmtTypePerIntf;
  }
  else
  {
    /* use per-queue config parm */
    mgmtType = (L7_QOS_COS_QUEUE_MGMT_TYPE_t)pCfg->queue[queueId].queueMgmtType;
  }

  return (mgmtType == qMgmtType) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
* @purpose  Display the current COS Queue Configuration contents
*
* @param    intIfNum    @b{(input)}  Internal interface number     
*
* @returns  void
*
* @comments Engineering debug function.
*
* @comments An intIfNum of 0 or L7_ALL_INTERFACES causes global tables 
*           to be displayed.
*
* @end
*********************************************************************/
void cosQueueConfigShow(L7_uint32 intIfNum)
{
  L7_cosIntfCfg_t       *pI;
  L7_cosQueueCfg_t      *pQ;
  L7_uint32             msgLvlReqd;
  L7_cosCfgIntfParms_t  *pCfgIntf;
  L7_uint32             i, j;
  L7_uchar8             tmpVal[L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uchar8             tmpStr[(L7_MAX_CFG_DROP_PREC_LEVELS*3)+1];
  char                  *fmtStr;

  memset(tmpVal, 0, sizeof(tmpVal));
  memset(tmpStr, 0, sizeof(tmpStr));

  /* specify required COS message level to display this output */
  msgLvlReqd = COS_MSGLVL_ON;

  if (intIfNum == 0)
    intIfNum = L7_ALL_INTERFACES;

  if (intIfNum == L7_ALL_INTERFACES)
  {
    pI = &pCosCfgData_g->cosGlobal.cfg.intf;
    pQ = pCosCfgData_g->cosGlobal.cfg.queue;
  }
  else
  {
    if (cosIntfIsConfigurable(intIfNum, &pCfgIntf) == L7_TRUE)
    {
      pI = &pCfgIntf->cfg.intf;
      pQ = pCfgIntf->cfg.queue;
    }
    else
    {
      /* configuration does not exist for this intIfNum */
      return;
    }
  }

  /* display interface-level parms */
  COS_PRT(msgLvlReqd, "  Intf shaping rate       :  %u\n", pI->intfShapingRate);
  COS_PRT(msgLvlReqd, "  Queue mgmt type per-intf:  %u\n", pI->queueMgmtTypePerIntf);
  COS_PRT(msgLvlReqd, "  WRED decay exponent     :  %u\n", pI->wredDecayExponent);
  COS_PRT(msgLvlReqd, "\n");

  /* display per-queue parms */
  COS_PRT(msgLvlReqd, "  queue min max sched mgmt   tail     wred     wred     wred   \n");
  COS_PRT(msgLvlReqd, "   id   bw  bw  type  type  thresh   minThr   maxThr  dropProb \n");
  COS_PRT(msgLvlReqd, "  ----- --- --- ----- ---- -------- -------- -------- -------- \n");
    /* sample output:      0    25 100  wrr  tail 16/14/12  8/ 6/ 4 16/14/12 10/10/10 */

  /* derive a customized format string for the drop precedence parms in order
   * to center the output in the column
   */
  if (L7_MAX_CFG_DROP_PREC_LEVELS == 1)
    fmtStr = "   %-2s    ";
  else if (L7_MAX_CFG_DROP_PREC_LEVELS == 2)
    fmtStr = " %-5s   ";
  else
    fmtStr = "%-8s ";

  /* only display supported number of queues for the platform, not the 
   * maximum number of queues defined for the config array
   */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++, pQ++)
  {
    COS_PRT(msgLvlReqd, "    %1u   %3u %3u  %-3.3s  %-4.4s ", 
            pQ->queueId, pQ->minBwPercent, pQ->maxBwPercent, 
            cosQueueSchedTypeStr[pQ->schedulerType],
            cosQueueMgmtTypeStr[pQ->queueMgmtType]);

    /* pull together the drop precedence level values for each parameter
     * and format into a display string
     */
    for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS; j++)
      tmpVal[j] = pQ->dropPrec[j].tdropThresh;
    cosQueueThreshStringFormat(tmpVal, tmpStr);
    COS_PRT(msgLvlReqd, fmtStr, tmpStr); 

    for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
      tmpVal[j] = pQ->dropPrec[j].wredMinThresh;
    cosQueueThreshStringFormat(tmpVal, tmpStr);
    COS_PRT(msgLvlReqd, fmtStr, tmpStr); 

    for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
      tmpVal[j] = pQ->dropPrec[j].wredMaxThresh;
    cosQueueThreshStringFormat(tmpVal, tmpStr);
    COS_PRT(msgLvlReqd, fmtStr, tmpStr); 

    for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
      tmpVal[j] = pQ->dropPrec[j].wredDropProb;
    cosQueueThreshStringFormat(tmpVal, tmpStr);
    COS_PRT(msgLvlReqd, fmtStr, tmpStr); 

    COS_PRT(msgLvlReqd, "\n");          /* ends queue row */

  } /* endfor i */

  COS_PRT(msgLvlReqd, "\n");
}

/*********************************************************************
* @purpose  Format a threshold display string based on number of 
*           drop precedence levels
*
* @param    val[]       @b{(input)}  Array of values to format
* @param    *pStr       @b{(output)} Ptr to string output location     
*
* @returns  void
*
* @comments Private helper function, only to be used by cosQueueConfigShow().
*
* @comments val[] is assumed to be an array[L7_MAX_CFG_DROP_PREC_LEVELS]
*           while pStr is assumed to be a buffer of at least 
*           ((L7_MAX_CFG_DROP_PREC_LEVELS*3)+1) bytes to hold the formatted
*           output threshold information in the form "xx/xx/xx".  Each
*           value to be displayed is assumed to require two digits.
*
* @end
*********************************************************************/
static void cosQueueThreshStringFormat(const L7_uchar8 val[], L7_uchar8 *pStr)
{
  char          *p = (char *)pStr;
  L7_uint32     j;

  *p = (char)L7_EOS;                    /* just a precaution */

  for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS; j++)
  {
    if (j > 0)
      sprintf(p++, "/");                /* value separator */

    sprintf(p, "%2u", val[j]);
    p += 2;                             /* incr must match preceding fmt width*/

  } /* endfor j */
}

