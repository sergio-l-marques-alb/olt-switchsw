#include "ptin_qos.h"
#include "ptin_intf.h"
#include "dtl_ptin.h"

/**
 * Configure Shaping
 * 
 * @author mruas (05/01/21)
 * 
 * @param ptin_port 
 * @param tc 
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t
ptin_qos_shaper_set(L7_uint32 ptin_port, L7_int tc,
                    L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size)
{
  L7_uint32 intIfNum;
  ptin_intf_shaper_t entry;
  L7_RC_t rc;

  /* Get intIfNum from ptin_port */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error converting ptin_port %u to intIfNum",  ptin_port);
    return L7_FAILURE;
  }
  
  /* Limit rate max to 100% */
  if(rate_max > 100)
  {
    rate_max = 100;
  }
  /* Rate min must be lower than rate max */
  if (rate_min > rate_max)
  {
    rate_min = rate_max;
  }
  
  //rc = usmDbQosCosQueueIntfShapingRateSet(1, intIfNum, (intfQos->shaping_rate * ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE])/100);
  memset(&entry, 0x00, sizeof(ptin_intf_shaper_t));

  /* Set min, max rate and burst size */
  entry.rate_min = rate_min;
  entry.rate_max = rate_max;
  entry.burst_size = burst_size;
  
  /* Set TC (-1 for all) */
  entry.tc = tc;

  /* Destination queue */
  entry.cosq_dest = PTIN_BRIDGE_VLAN_COSQ_DEST_DEFAULT;

#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
  if (PTIN_PORT_IS_PON_GPON_TYPE(ptin_port))
  {
    entry.cosq_dest = PTIN_BRIDGE_VLAN_COSQ_DEST_WIRED;
  }
  else if (PTIN_PORT_IS_PON_XGSPON_TYPE(ptin_port))
  {
    entry.cosq_dest = PTIN_BRIDGE_VLAN_COSQ_DEST_WIRELESS;
  }
#endif

  PT_LOG_NOTICE(LOG_CTX_INTF, "ptin_port=%u, intIfNum %u, tc=%d dst_queue=%u: min_rate=%u max_rate=%u burst_size=%u",
                ptin_port, intIfNum, entry.tc, entry.cosq_dest, entry.rate_min, entry.rate_max, entry.burst_size);

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_SET, DAPI_CMD_SET, sizeof(ptin_intf_shaper_t), &entry);

  if (rc != L7_SUCCESS)
  {  
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet (rc=%d)", rc);
  }

  return rc;
}


