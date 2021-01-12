#include "ptin_qos.h"
#include "ptin_intf.h"
#include "dtl_ptin.h"

#include "ptin_fieldproc.h"
#include "usmdb_qos_cos_api.h"
#include "usmdb_mib_vlan_api.h"

/* MAX interface rate to limit shaping (percentage value) */
/* Index 1: Effective configuration from manager */
/* Index 2: Max shaper value */
#define PTIN_INTF_SHAPER_MNG_VALUE  0
#define PTIN_INTF_SHAPER_MAX_VALUE  1
#define PTIN_INTF_FEC_VALUE         2
L7_uint32 ptin_intf_shaper_max[PTIN_SYSTEM_N_INTERF][3];
L7_uint32 ptin_burst_size[PTIN_SYSTEM_N_INTERF];

#define MAX_BURST_SIZE 16000

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
static 
L7_RC_t ptin_qos_shaper_set(L7_uint32 ptin_port, L7_int tc,
                            L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size);


/**
 * Initialize QoS module
 * 
 * @author mruas (06/01/21)
 * 
 * @return L7_RC_t 
 */
void ptin_qos_init(void)
{
  int i;

  /* For all interfaces, max rate is 100% */
  for (i = 0; i < PTIN_SYSTEM_N_INTERF; i++)
  {
    ptin_intf_shaper_max[i][PTIN_INTF_SHAPER_MNG_VALUE] = 100;   /* Shaper value from management */
    ptin_intf_shaper_max[i][PTIN_INTF_SHAPER_MAX_VALUE] = 100; /* Max. Shaper value */
    ptin_intf_shaper_max[i][PTIN_INTF_FEC_VALUE]        = 100; /* FEC value value */
    ptin_burst_size[i] = MAX_BURST_SIZE; //default bcm value for port max burst rate
  }
}

/**
 * Apply default QoS configurations to provided interface
 * 
 * @param ptin_port : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_intf_default(L7_uint32 ptin_port)
{
  L7_int          i, j;
  ptin_QoS_intf_t qos_intf_cfg;
  ptin_QoS_cos_t  qos_cos_cfg;
  ptin_QoS_drop_t qos_cos_drop;
  L7_RC_t         rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Define default QoS configuration */
  memset(&qos_intf_cfg,0x00,sizeof(ptin_QoS_intf_t));
  qos_intf_cfg.mask         = PTIN_QOS_INTF_TRUSTMODE_MASK | PTIN_QOS_INTF_PACKETPRIO_MASK;
  qos_intf_cfg.trust_mode   = L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
  
  /* Linear pbit->cos mapping */
  for (i=0; i<8; i++)
  {
    qos_intf_cfg.pktprio.mask[i] = PTIN_QOS_INTF_PACKETPRIO_COS_MASK;
    qos_intf_cfg.pktprio.cos[i]  = i;
  }
  /* Strict scheduler */
  memset(&qos_cos_cfg,0x00,sizeof(ptin_QoS_cos_t));
  qos_cos_cfg.mask           = PTIN_QOS_COS_SCHEDULER_MASK;
  qos_cos_cfg.scheduler_type = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;

  /* Drop management: default is WRED */
  memset(&qos_cos_drop,0x00,sizeof(ptin_QoS_drop_t));
  qos_cos_drop.queue_management_type = 1; /* WRED */
  qos_cos_drop.wred_decayExp = 9;
  qos_cos_drop.mask = PTIN_QOS_COS_QUEUE_MANGM_MASK | PTIN_QOS_COS_WRED_DECAY_EXP_MASK | PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
  for (j = 0; j < 4; j++)
  {
    qos_cos_drop.dp[j].local_mask = PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK |
                                    PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK | PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK | 
                                    PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK;
    qos_cos_drop.dp[j].taildrop_threshold = 100;
    qos_cos_drop.dp[j].wred_min_threshold = 100;
    qos_cos_drop.dp[j].wred_max_threshold = 100;
    qos_cos_drop.dp[j].wred_drop_prob     = 100;
  }

  /* Apply configurations to interface */
  if (ptin_qos_intf_config_set(ptin_port, &qos_intf_cfg)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u: failed QoS initialization of interface", ptin_port);
    rc = L7_FAILURE;
  }
  /* Apply configurations to CoS */
  for (i=0; i<8; i++)
  {
    /* Scheduler configuration */
    if (ptin_qos_cos_config_set (ptin_port, i, &qos_cos_cfg)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u: failed QoS initialization of CoS=%u", ptin_port, i);
      rc = L7_FAILURE;
    }
    /* Drop management configuration */
    if (ptin_qos_drop_config_set(ptin_port, i, &qos_cos_drop) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u: failed QoS initialization of CoS=%u", ptin_port, i);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Apply a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_port
 * @param cos 
 * @param meter 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_cos_policer_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_bw_meter_t *meter)
{
  ptin_bw_profile_t profile;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Null pointer");
    return L7_FAILURE;
  }
  if (cos >= L7_COS_INTF_QUEUE_MAX_COUNT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid COS %u", cos);
    return L7_FAILURE;
  }

  memset(&profile, 0x00, sizeof(profile));

  profile.ptin_port = ptin_port;
  profile.cos       = cos;

  /* Apply policer */
  if ((rc = ptin_bwPolicer_set(&profile, meter, -1)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Remove a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_port 
 * @param cos 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_cos_policer_clear(L7_uint32 ptin_port, L7_uint8 cos)
{
  ptin_bw_profile_t profile;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Null pointer");
    return L7_FAILURE;
  }
  if (cos >= L7_COS_INTF_QUEUE_MAX_COUNT)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid COS %u", cos);
    return L7_FAILURE;
  }

  memset(&profile, 0x00, sizeof(profile));

  profile.ptin_port = ptin_port;
  profile.cos       = cos;

  /* Apply policer */
  if ((rc = ptin_bwPolicer_delete(&profile)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Error applying policer");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Configures interface properties for QoS
 * 
 * @param ptin_port : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_intf_config_set(L7_uint32 ptin_port, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2, cos;
  L7_uint32 intIfNum;
  l7_cosq_set_t queueSet;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || intfQos == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"ptin_port    = %u", ptin_port);
  PT_LOG_TRACE(LOG_CTX_INTF,"Mask         = 0x%02x",intfQos->mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"TrustMode    = %u",intfQos->trust_mode);
  PT_LOG_TRACE(LOG_CTX_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  PT_LOG_TRACE(LOG_CTX_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  PT_LOG_TRACE(LOG_CTX_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.mask   =0x%02x",intfQos->pktprio.mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
            intfQos->pktprio.cos[0],
            intfQos->pktprio.cos[1],
            intfQos->pktprio.cos[2],
            intfQos->pktprio.cos[3],
            intfQos->pktprio.cos[4],
            intfQos->pktprio.cos[5],
            intfQos->pktprio.cos[6],
            intfQos->pktprio.cos[7]);

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u => intIfNum=%u, queueSet %u", ptin_port, intIfNum, queueSet);

  /* Is there any configuration to be applied? */
  if (intfQos->mask==0x00)
  {
    PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
    return L7_SUCCESS;
  }

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  // Get Trust mode
  rc = usmDbQosCosMapTrustModeGet(1, intIfNum, queueSet, &trust_mode);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error with usmDbQosCosMapTrustModeGet (rc=%d)", rc);
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc_global = rc;
  }
  // Validate trust mode
  else if (trust_mode==L7_NULL || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid trust mode (%u)",trust_mode);
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    rc_global = L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "Current trust mode is %u",trust_mode);

  /* Set Trust mode */
  if (intfQos->mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
  {
    // Define trust mode
    rc = usmDbQosCosMapTrustModeSet(1, intIfNum, queueSet, intfQos->trust_mode);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapTrustModeSet (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      // Configuration successfull => change trust mode value
      trust_mode = intfQos->trust_mode;
      PT_LOG_TRACE(LOG_CTX_INTF, "New trust mode is %u",trust_mode);
    }
  }
  /* Bandwidth units */
  if (intfQos->mask & PTIN_QOS_INTF_BANDWIDTHUNIT_MASK)
  {
    /* Do nothing */
    PT_LOG_WARN(LOG_CTX_INTF, "Bandwidth units were not changed");
  }
  /* Shaping rate */
  if (intfQos->mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
  {
    L7_uint32 rate_max_apply, burst_size_apply;

    PT_LOG_NOTICE(LOG_CTX_INTF, "New shaping rate is %u", intfQos->shaping_rate);

    if(intfQos->shaping_rate == 0)
    {
      intfQos->shaping_rate = 100;
    }

    PT_LOG_TRACE(LOG_CTX_INTF, "ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = %u",
                 ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE]);
    PT_LOG_TRACE(LOG_CTX_INTF, "intfQos->shaping_rate = %u",intfQos->shaping_rate);

    //rc = usmDbQosCosQueueIntfShapingRateSet(1, intIfNum, (intfQos->shaping_rate * ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE])/100);

    /* Shaper settings */
    if (intfQos->shaping_rate <= (ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE]))
    {
      rate_max_apply = intfQos->shaping_rate;
    }
    else
    {
      rate_max_apply = ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE];
    }
    burst_size_apply = ptin_burst_size[ptin_port]; 

    PT_LOG_INFO(LOG_CTX_INTF, "Applying shaper to ptin_port %u: rate_max=%u, burst size=u",
                ptin_port, rate_max_apply, burst_size_apply);

    rc = ptin_qos_shaper_set(ptin_port, -1 /*All TC*/, 0 /*Rate_min*/, rate_max_apply, burst_size_apply);

    if (rc == L7_SUCCESS)
    {
      ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE] = intfQos->shaping_rate;
      ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = rate_max_apply;
    }
    else
    {  
      PT_LOG_ERR(LOG_CTX_INTF, "Error with ptin_qos_shaper_set (rc=%d)", rc);
      rc_global = rc;
    }
  }
  /* WRED decay exponent */
  if (intfQos->mask & PTIN_QOS_INTF_WRED_DECAY_EXP_MASK)
  {
    rc = usmDbQosCosQueueWredDecayExponentSet(1, intIfNum, queueSet, intfQos->wred_decay_exponent);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueWredDecayExponentSet (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "New WRED Decay exponent is %u",intfQos->wred_decay_exponent);
    }
  }
  /* Packet priority mask */
  if (intfQos->mask & PTIN_QOS_INTF_PACKETPRIO_MASK &&
      trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
  {
    /* Run all priorities */
    for (prio=0; prio<8; prio++)
    {
      /* If priority mask active, attribute cos */
      if (intfQos->pktprio.mask[prio] == 0)  continue;
      
      // 802.1p trust mode
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        rc = usmDbDot1dTrafficClassSet(1,intIfNum,prio,cos);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbDot1dTrafficClassSet (prio=%u => cos=%u): rc=%d",prio,cos, rc);
          rc_global = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        /* CoS goes from 0 to 7 (0b000 to 0b111) */
        cos = intfQos->pktprio.cos[prio] & 0x07;

        rc = usmDbQosCosMapIpPrecTrafficClassSet(1, intIfNum, queueSet, prio, cos);
        if (rc != L7_SUCCESS)
        { 
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassSet (IPprec=%u => cos=%u): rc=%d",prio,cos, rc); 
          rc_global = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          if ( !((intfQos->pktprio.mask[prio]>>prio2) & 1) )  continue;

          /* Map 64 different priorities (6 bits) to 8 CoS */
          cos = ((intfQos->pktprio.cos[prio])>>(4*prio2)) & 0x07;

          rc = usmDbQosCosMapIpDscpTrafficClassSet(1, intIfNum, queueSet, prio*8+prio2, cos);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassSet (DscpPrio=%u => CoS=%u): rc=%d",prio*8+prio2,cos, rc);
            rc_global = rc;
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc_global = L7_FAILURE;
      }
    }
  }

  if (rc_global==L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully applied to ptin_port=%u", ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS configuration to ptin_port=%u (rc_global=%d)", ptin_port, rc_global);
  }

  return rc_global;
}

/**
 * Read interface properties for QoS
 * 
 * @param ptin_port : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_intf_config_get(L7_uint32 ptin_port, ptin_QoS_intf_t *intfQos)
{
  L7_uint8  prio, prio2;
  L7_uint32 intIfNum, value, cos;
  l7_cosq_set_t queueSet;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF || intfQos == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Clear configurations to be returned */
  memset(intfQos,0x00,sizeof(ptin_QoS_intf_t));

  PT_LOG_TRACE(LOG_CTX_INTF,"ptin_port=%u", ptin_port);

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u => intIfNum=%u, queueSet %u", ptin_port, intIfNum, queueSet);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  // Get Trust mode
  rc = usmDbQosCosMapTrustModeGet(1, intIfNum, queueSet, &value);
  if (rc != L7_SUCCESS)
  {
    trust_mode = L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    PT_LOG_ERR(LOG_CTX_INTF,"Error with usmDbQosCosMapTrustModeGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    trust_mode = value;
    intfQos->trust_mode = (L7_uint8) value;
    intfQos->mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
  }

  /* Get units */
  intfQos->bandwidth_unit = (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_PERCENT) ? 0 : 1;
  intfQos->mask |= PTIN_QOS_INTF_BANDWIDTHUNIT_MASK;

  /* Shaping rate */
  rc = usmDbQosCosQueueIntfShapingRateGet(1, intIfNum, queueSet, &value, L7_NULLPTR);
  if (rc != L7_SUCCESS)
  {  
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    intfQos->shaping_rate = value;
    intfQos->mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
  }

  /* WRED decay exponent */
  rc = usmDbQosCosQueueWredDecayExponentGet(1, intIfNum, queueSet, &value);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueWredDecayExponentGet (rc=%d)", rc);
    rc_global = rc;
  }
  else
  {
    intfQos->wred_decay_exponent = value;
    intfQos->mask |= PTIN_QOS_INTF_WRED_DECAY_EXP_MASK;
  }

  /* Only for non untrusted mode, we have priority map */
  if (trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
  {
    /* Run all priorities */
    for (prio=0; prio<8; prio++)
    {
      // 802.1p trust mode
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        rc = usmDbDot1dTrafficClassGet(1,intIfNum,prio,&cos);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbDot1dTrafficClassGet (prio=%u) (rc=%d)",prio, rc);
          rc_global = rc;
        }
        else
        {
          intfQos->pktprio.mask[prio] = 1;
          intfQos->pktprio.cos[prio]  = cos;
          PT_LOG_TRACE(LOG_CTX_INTF, "Pbit %u => CoS=%u",prio,cos);
        }
      }
      // IP-precedence trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        rc = usmDbQosCosMapIpPrecTrafficClassGet(1, intIfNum, queueSet, prio, &cos);
        if (rc != L7_SUCCESS)
        { 
          PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpPrecTrafficClassGet (IPprec=%u) (rc=%d)",prio, rc);
          rc_global = rc;
        }
        else
        {
          intfQos->pktprio.mask[prio] = 1;
          intfQos->pktprio.cos[prio]  = cos;
          PT_LOG_TRACE(LOG_CTX_INTF, "IPprec %u => CoS=%u",prio,cos);
        }
      }
      // DSCP trust mode
      else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        for (prio2=0; prio2<8; prio2++)
        {
          rc = usmDbQosCosMapIpDscpTrafficClassGet(1, intIfNum, queueSet, prio*8+prio2, &cos);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosMapIpDscpTrafficClassGet (DscpPrio=%u) (rc=%d)",prio*8+prio2, rc);
            rc_global = rc;
            break;
          }
          else
          {
            intfQos->pktprio.mask[prio] |= (L7_uint8) 1 << prio2;
            intfQos->pktprio.cos[prio]  |= ((L7_uint32) cos & 0x0f)<<(prio2*4);
            PT_LOG_TRACE(LOG_CTX_INTF, "DscpPrio %u => CoS=%u",prio*8+prio2,cos);
          }
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Unknown trust mode for prio=%u (%u)",prio,trust_mode);
        rc_global = L7_FAILURE;
      }
    }

    /* Packet priority mask */
    if (intfQos->pktprio.mask)
    {
      intfQos->mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
    }
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"Mask         = 0x%02x",intfQos->mask);
  PT_LOG_TRACE(LOG_CTX_INTF,"TrustMode    = %u",intfQos->trust_mode);
  PT_LOG_TRACE(LOG_CTX_INTF,"BWunits      = %u",intfQos->bandwidth_unit);
  PT_LOG_TRACE(LOG_CTX_INTF,"ShapingRate  = %u",intfQos->shaping_rate);
  PT_LOG_TRACE(LOG_CTX_INTF,"WREDDecayExp = %u",intfQos->wred_decay_exponent);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.mask   ={0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x}",
            intfQos->pktprio.mask[0],intfQos->pktprio.mask[1],intfQos->pktprio.mask[2],intfQos->pktprio.mask[3],intfQos->pktprio.mask[4],intfQos->pktprio.mask[5],intfQos->pktprio.mask[6],intfQos->pktprio.mask[7]);
  PT_LOG_TRACE(LOG_CTX_INTF,"PrioMap.prio[8]={0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x}",
            intfQos->pktprio.cos[0],
            intfQos->pktprio.cos[1],
            intfQos->pktprio.cos[2],
            intfQos->pktprio.cos[3],
            intfQos->pktprio.cos[4],
            intfQos->pktprio.cos[5],
            intfQos->pktprio.cos[6],
            intfQos->pktprio.cos[7]);

  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully read from ptin_port %u", ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error reading QoS configuration from ptin_port %u (rc_global=%d)", ptin_port, rc_global);
  }

  return rc_global;
}


/**
 * Configures a class of service for QoS
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_cos_config_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, i, conf_index;
  l7_cosq_set_t queueSet;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueWeightList_t    schedWeight_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      qosConf == L7_NULLPTR ||
      (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u is intIfNum=%u, queueSet %u", ptin_port, intIfNum, queueSet);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* Get current configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, queueSet, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading scheduler type (rc=%d)", rc);
    return rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, queueSet, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading minimum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, queueSet, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading maximum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Weights list */
  rc = usmDbQosCosQueueWeightListGet(1, intIfNum, queueSet, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading weights list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Is there any configuration to be applied? */
    if (qosConf[conf_index].mask==0x00)
    {
      //PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
      continue;
    }

    /* Scheduler type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_SCHEDULER_MASK)
    {
      schedType_list.schedType[i] = qosConf[conf_index].scheduler_type;
      PT_LOG_TRACE(LOG_CTX_INTF,"Scheduler type in cos=%u, will be updated to %u",i,qosConf[conf_index].scheduler_type);
    }
    /* Minimum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MIN_MASK)
    {
      minBw_list.bandwidth[i] = qosConf[conf_index].min_bandwidth;
      PT_LOG_TRACE(LOG_CTX_INTF,"Minimum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].min_bandwidth);
    }
    /* Maximum bandwidth */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_BW_MAX_MASK)
    {
      maxBw_list.bandwidth[i] = qosConf[conf_index].max_bandwidth;
      PT_LOG_TRACE(LOG_CTX_INTF,"Maximum bandwidth in cos=%u, will be updated to %u",i,qosConf[conf_index].max_bandwidth);
    }
    /* Scheduler type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_WRR_WEIGHT_MASK)
    {
      schedWeight_list.queue_weight[i] = qosConf[conf_index].wrrSched_weight;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRR weight for cos=%u, will be updated to %u",i,qosConf[conf_index].wrrSched_weight);
    }
  }

  /* Apply new configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListSet(1, intIfNum, queueSet, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying scheduler type (rc=%d)", rc);
    rc_global = rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListSet(1, intIfNum, queueSet, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying minimum bandwith (rc=%d)", rc);
    rc_global = rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListSet(1, intIfNum, queueSet, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying maximum bandwith (rc=%d)", rc);
    rc_global = rc;
  }
  /* WRR weights */
  rc = usmDbQosCosQueueWeightListSet(1, intIfNum, queueSet, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error applying WRR weights (rc=%d)", rc);
    rc_global = rc;
  }

  /* Check result */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS configuration successfully applied to ptin_port=%u, cos=%u", ptin_port, cos);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS configuration to ptin_port=%u, cos=%u (rc_global=%d)", ptin_port, cos, rc_global);
  }

  return rc_global;
}

/**
 * Reads a class of service QoS configuration
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_cos_config_get(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_cos_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i;
  l7_cosq_set_t queueSet;
  L7_RC_t   rc = L7_SUCCESS;
  L7_qosCosQueueSchedTypeList_t schedType_list;
  L7_qosCosQueueWeightList_t    schedWeight_list;
  L7_qosCosQueueBwList_t        minBw_list;
  L7_qosCosQueueBwList_t        maxBw_list;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      qosConf == L7_NULLPTR ||
      (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"ptin_port %u, cos=%u", ptin_port, cos);

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u => intIfNum=%u, queueSet %u", ptin_port, intIfNum, queueSet);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Scheduler type */
  rc = usmDbQosCosQueueSchedulerTypeListGet(1, intIfNum, queueSet, &schedType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading scheduler type (rc=%d)", rc);
    return rc;
  }
  /* Minimum bandwidth */
  rc = usmDbQosCosQueueMinBandwidthListGet(1, intIfNum, queueSet, &minBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading minimum bandwith (rc=%d)", rc);
    return rc;
  }
  /* Maximum bandwidth */
  rc = usmDbQosCosQueueMaxBandwidthListGet(1, intIfNum, queueSet, &maxBw_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading maximum bandwith (rc=%d)", rc);
    return rc;
  }
  /* WRR weights */
  rc = usmDbQosCosQueueWeightListGet(1, intIfNum, queueSet, &schedWeight_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading WRR weights (rc=%d)", rc);
    return rc;
  }

  /* Copy returned data to output */
  for (i=0; i<8; i++)
  {
    if ( cos == (L7_uint8)-1 )
    {
      conf_index = i;
    }
    else if (cos==i)
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Clear output structure */
    memset(&qosConf[conf_index],0x00,sizeof(ptin_QoS_cos_t));

    qosConf[conf_index].scheduler_type = (L7_uint8) schedType_list.schedType[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_SCHEDULER_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Scheduler type for cos=%u is %u",i,qosConf[conf_index].scheduler_type);

    qosConf[conf_index].min_bandwidth = minBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MIN_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Minimum bandwith for cos=%u is %u",i,qosConf[conf_index].min_bandwidth);

    qosConf[conf_index].max_bandwidth = maxBw_list.bandwidth[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_BW_MAX_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Maximum bandwith for cos=%u is %u",i,qosConf[conf_index].max_bandwidth);

    qosConf[conf_index].wrrSched_weight = schedWeight_list.queue_weight[i];
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRR_WEIGHT_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"WRR weight for cos=%u is %u",i,qosConf[conf_index].wrrSched_weight);
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully read from ptin_port=%u, cos=%u", ptin_port, cos);

  return L7_SUCCESS;
}

/**
 * Configures a class of service for QoS
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_drop_config_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_drop_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i, j;
  l7_cosq_set_t queueSet;
  L7_RC_t   rc, rc_global = L7_SUCCESS;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosDropParmsList_t      dropParams_list;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      qosConf == L7_NULLPTR ||
      (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"ptin_port %u, cos=%u", ptin_port, cos);

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u => intIfNum=%u, queueSet=%u", ptin_port, intIfNum, queueSet);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1,intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, queueSet, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading mgmtType type (rc=%d)", rc);
    return rc;
  }

  /* Get drop params configurations */
  rc = usmDbQosCosQueueDropParmsListGet(1, intIfNum, queueSet, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading dropParams list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    /* Is there any configuration to be applied? */
    if (qosConf[conf_index].mask==0x00)
    {
      //PT_LOG_WARN(LOG_CTX_INTF, "Empty mask: no configuration to be applied");
      continue;
    }

    /* Drop Management type */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_QUEUE_MANGM_MASK)
    {
      dropParams_list.queue[i].mgmtType = qosConf[conf_index].queue_management_type + 1;
      PT_LOG_TRACE(LOG_CTX_INTF,"Mgmt type in cos=%u, will be updated to %u",i,qosConf[conf_index].queue_management_type);
    }
    /* WRED decay exponent */
    if (qosConf[conf_index].mask & PTIN_QOS_COS_WRED_DECAY_EXP_MASK)
    {
      dropParams_list.queue[i].wred_decayExponent = qosConf[conf_index].wred_decayExp;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED decay Exp. in cos=%u, will be updated to %u",i,qosConf[conf_index].wred_decayExp);
    }

    /* Thresholds */
    if ((qosConf[conf_index].mask & PTIN_QOS_COS_WRED_THRESHOLDS_MASK) || (qosConf[conf_index].mask & PTIN_QOS_COS_TAIL_THRESHOLDS_MASK))
    {
      /* Run all DP levels */
      for (j = 0; j < 4; j++)
      {
        if (qosConf[conf_index].dp[j].local_mask == 0)  continue;

        /* Taildrop threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK)
        {
          dropParams_list.queue[i].tailDropMaxThreshold[j] = qosConf[conf_index].dp[j].taildrop_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"Taildrop threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].taildrop_threshold);
        }
        /* WRED min threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK)
        {
          dropParams_list.queue[i].minThreshold[j] = qosConf[conf_index].dp[j].wred_min_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED min threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_min_threshold);
        }
        /* WRED max threshold */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK)
        {
          dropParams_list.queue[i].wredMaxThreshold[j] = qosConf[conf_index].dp[j].wred_max_threshold;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED max threshold for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_max_threshold);
        }
        /* WRED drop probability */
        if (qosConf[conf_index].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK)
        {
          dropParams_list.queue[i].dropProb[j] = qosConf[conf_index].dp[j].wred_drop_prob;
          PT_LOG_TRACE(LOG_CTX_INTF,"WRED drop probability for cos=%u/dp=%u, will be updated to %u",i,j,qosConf[conf_index].dp[j].wred_drop_prob);
        }
      }
    }
  }

  /* Apply new configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListSet(1, intIfNum, queueSet, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error setting new mgmtType list (rc=%d)", rc);
    rc_global = rc;
  }

  /* Drop params list */
  rc = usmDbQosCosQueueDropParmsListSet(1, intIfNum, queueSet, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error setting new dropParams list (rc=%d)", rc);
    rc_global = rc;
  }

  /* Check result */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully applied to ptin_port=%u, cos=%u", ptin_port, cos);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying QoS drop configuration to ptin_port=%u, cos=%u (rc_global=%d)", ptin_port, cos, rc_global);
  }

  return rc_global;
}

/**
 * Reads a class of service QoS configuration
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_drop_config_get(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_drop_t *qosConf)
{
  L7_uint32 intIfNum, conf_index, i, j;
  l7_cosq_set_t queueSet;
  L7_qosCosQueueMgmtTypeList_t  mgmtType_list;
  L7_qosCosDropParmsList_t      dropParams_list;
  L7_RC_t rc;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      qosConf == L7_NULLPTR ||
      (cos != (L7_uint8)-1 && cos > 7))
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF,"ptin_port %u, cos=%u", ptin_port, cos);

  /* Validate interface */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "ptin_port %u invalid", ptin_port);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "ptin_port %u => intIfNum=%u, queueSet=%u", ptin_port, intIfNum, queueSet);

  /* Check if interface is valid for QoS configuration */
  if (!usmDbQosCosMapIntfIsValid(1, intIfNum))
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Invalid interface for QoS operation (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* Get configurations */
  /* Mgmt type */
  rc = usmDbQosCosQueueMgmtTypeListGet(1, intIfNum, queueSet, &mgmtType_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading mgmtType type (rc=%d)", rc);
    return rc;
  }

  /* Get drop params configurations */
  rc = usmDbQosCosQueueDropParmsListGet(1, intIfNum, queueSet, &dropParams_list);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"Error reading dropParams list (rc=%d)", rc);
    return rc;
  }

  /* Change configurations */
  for (i=0; i<8; i++)
  {
    if (cos == (L7_uint8)-1)
    {
      conf_index = i;
    }
    else if ( cos == i )
    {
      conf_index = 0;
    }
    else
    {
      continue;
    }

    memset(&qosConf[conf_index], 0x00, sizeof(ptin_QoS_drop_t));

    /* Drop Management type */
    qosConf[conf_index].queue_management_type = dropParams_list.queue[i].mgmtType - 1;
    qosConf[conf_index].mask |= PTIN_QOS_COS_QUEUE_MANGM_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"Mgmt type at cos=%u is %u",i,qosConf[conf_index].queue_management_type);

    /* WRED decay exponent */
    qosConf[conf_index].wred_decayExp = dropParams_list.queue[i].wred_decayExponent;
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRED_DECAY_EXP_MASK;
    PT_LOG_TRACE(LOG_CTX_INTF,"WRED decay Exp. at cos=%u is %u",i,qosConf[conf_index].wred_decayExp);

    /* Run all DP levels */
    for (j = 0; j < 4; j++)
    {
      /* Taildrop threshold */
      qosConf[conf_index].dp[j].taildrop_threshold = dropParams_list.queue[i].tailDropMaxThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"Taildrop threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].taildrop_threshold);

      /* WRED min threshold */
      qosConf[conf_index].dp[j].wred_min_threshold = dropParams_list.queue[i].minThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED min threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_min_threshold);

      /* WRED max threshold */
      qosConf[conf_index].dp[j].wred_max_threshold = dropParams_list.queue[i].wredMaxThreshold[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED max threshold for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_max_threshold);

      /* WRED drop probability */
      qosConf[conf_index].dp[j].wred_drop_prob = dropParams_list.queue[i].dropProb[j];
      qosConf[conf_index].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK;
      PT_LOG_TRACE(LOG_CTX_INTF,"WRED drop probability for cos=%u/dp=%u is %u",i,j,qosConf[conf_index].dp[j].wred_drop_prob);
    }
    qosConf[conf_index].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "QoS drop configuration successfully applied to ptin_port=%u, cos=%u", ptin_port, cos);

  return L7_SUCCESS;
}


/**
 * Set the maximum rate for a port
 * 
 * @author mruas (16/08/17)
 * 
 * @param ptin_port
 * @param max_rate : Percentage
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_intf_shaper_set(L7_uint32 ptin_port, L7_uint32 max_rate, L7_uint32 burst_size)
{
  L7_uint32 rate_max_apply;
  L7_RC_t rc;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Limit max rate */
  if (max_rate > 100)
  {
    max_rate = 100;
  }

  /* Limit max rate */
  if (burst_size == 0)
  {
    burst_size = MAX_BURST_SIZE;
  }

#if 0 // this will be done with ptin_hapi_qos_shaper_max_burst_config (broad_ptin.c)
  /* Apply correct shaping rate */
  if (usmDbQosCosQueueIntfShapingRateSet(1, intIfNum, (ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE]*max_rate)/100) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet");
    return L7_FAILURE;
  }
#endif
  
  if (max_rate > ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE])
  {
    rate_max_apply = ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MNG_VALUE];
  }
  else
  {
    rate_max_apply = max_rate;
  }

  PT_LOG_INFO(LOG_CTX_INTF, "Applying shaper to ptin_port %u: rate_max=%u, burst size=u",
              ptin_port, rate_max_apply, burst_size);

  /* Apply shaper */
  rc = ptin_qos_shaper_set(ptin_port, -1 /*All TC*/, 0 /*rate_min*/, rate_max_apply, burst_size);

  if (rc == L7_SUCCESS)
  {
    /* Save max rate for this interface */
    ptin_intf_shaper_max[ptin_port][PTIN_INTF_SHAPER_MAX_VALUE] = rate_max_apply;
    ptin_intf_shaper_max[ptin_port][PTIN_INTF_FEC_VALUE] = max_rate;
    ptin_burst_size[ptin_port] = burst_size;
  }
  else
  {  
    PT_LOG_ERR(LOG_CTX_INTF, "Error with ptin_qos_shaper_set (rc=%d)", rc);
  }

  return rc;
}


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
static 
L7_RC_t ptin_qos_shaper_set(L7_uint32 ptin_port, L7_int tc,
                            L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size)
{
  L7_uint32 intIfNum;
  l7_cosq_set_t queueSet;
  ptin_intf_shaper_t entry;
  L7_RC_t rc;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Get intIfNum from ptin_port */
  if (ptin_intf_port2intIfNum_queueSet(ptin_port, &intIfNum, &queueSet) != L7_SUCCESS)
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
  entry.queueSet = queueSet;

  PT_LOG_NOTICE(LOG_CTX_INTF, "ptin_port=%u/intIfNum %u, tc=%d dst_queue=%u: min_rate=%u max_rate=%u burst_size=%u",
                ptin_port, intIfNum, entry.tc, entry.queueSet, entry.rate_min, entry.rate_max, entry.burst_size);

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_SHAPER_SET, DAPI_CMD_SET, sizeof(ptin_intf_shaper_t), &entry);

  if (rc != L7_SUCCESS)
  {  
    PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateSet (rc=%d)", rc);
  }

  return rc;
}


void ptin_debug_intf_cos_policer_set(L7_uint32 ptin_port, L7_uint8 cos, L7_uint32 cir, L7_uint32 eir, L7_uint32 cbs, L7_uint32 ebs)
{
  ptin_bw_meter_t meter;
  L7_RC_t         rc;

  printf("Configuring policer for ptin_port %u + COS %u...", ptin_port, cos);

  memset(&meter, 0x00, sizeof(meter));
  meter.cir = cir;
  meter.eir = eir;
  meter.cbs = cbs;
  meter.ebs = ebs;

  rc = ptin_qos_cos_policer_set(ptin_port, cos, &meter);

  printf("Result of operation: rc=%d", rc);
}

/**
 * Dump the maximum rate for all interfaces
 * 
 * @author mruas (16/08/17)
 */
void ptin_intf_shaper_max_dump(void)
{
  L7_uint32 port, intIfNum, shaper_rate;
  l7_cosq_set_t queueSet;
  L7_uchar8 queueSet_str[10];
  L7_RC_t rc;

  printf(" ---------------------------------------------------------\r\n");
  printf("| Port | queueSet | Max rate | Mng max | Eff max | BurstS |\r\n");
  for (port = 0; port < PTIN_SYSTEM_N_PORTS; port++)
  {
    if (ptin_intf_port2intIfNum_queueSet(port, &intIfNum, &queueSet) == L7_SUCCESS)
    {
      rc = usmDbQosCosQueueIntfShapingRateGet(1, intIfNum, queueSet, &shaper_rate, L7_NULLPTR);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error with usmDbQosCosQueueIntfShapingRateGet: rc=%d", rc);
        shaper_rate = 0;
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error converting port %u to intIfNum", port);
      shaper_rate = 0;
    }

    /* String associated to the queue */
    switch (queueSet)
    {
      case L7_QOS_QSET_PORT:
        strcpy(queueSet_str,"Port");
        break;
      case L7_QOS_QSET_WIRED:
        strcpy(queueSet_str,"Wired");
        break;
      case L7_QOS_QSET_WIRELESS:
        strcpy(queueSet_str,"Wireless");
        break;
      default:
        strcpy(queueSet_str,"---");
        break;
    }
    
    printf("|  %2u  | %-8s |    %3u   |   %3u   |   %3u   | %6u |\r\n",
           port, queueSet_str,
           ptin_intf_shaper_max[port][PTIN_INTF_SHAPER_MAX_VALUE],
           ptin_intf_shaper_max[port][PTIN_INTF_SHAPER_MNG_VALUE],
           shaper_rate,
           ptin_burst_size[port]);
  }
  printf(" ---------------------------------------------------------\r\n");
}

