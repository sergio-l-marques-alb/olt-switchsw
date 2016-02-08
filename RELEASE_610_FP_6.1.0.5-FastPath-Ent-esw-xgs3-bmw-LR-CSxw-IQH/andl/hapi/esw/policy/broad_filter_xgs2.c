/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_filter_xgs2.c
*
* This file implements XGS II specific policy code for the local unit.
*
* @component hapi
*
* @create    4/18/2006
*
* @author    sdoke
*
* @end
*
**********************************************************************/

#include "broad_filter_xgs2.h"
#include "bcm/filter.h"
#include "bcm/meter.h"
#include "bcmx/bcmx_int.h"
#include "ibde.h"

#define FFP_DEBUG_PRINT if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW) printf

/* Filter Action Map */
/* This table maps the BROAD_ACTION_t enum to BCM API types. Keep in mind that the same
 * params (0/1/2) are passed to all actions, so they must have compatible params or be
 * specified in separate actions.
 */

#define FILTER_ACTION_INVALID bcmActionInvalid
#define FILTER_ACTION_NONE   (FILTER_ACTION_INVALID+1)

#define FILTER_ACTIONS_PER_MAP_ENTRY     4

typedef struct
{
    bcm_filter_action_t green[FILTER_ACTIONS_PER_MAP_ENTRY];
    bcm_filter_action_t yellow[FILTER_ACTIONS_PER_MAP_ENTRY];
    bcm_filter_action_t red[FILTER_ACTIONS_PER_MAP_ENTRY];
}
filter_action_map_entry_t;

static filter_action_map_entry_t filter_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmActionDoNotSwitch,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmActionDoNotSwitch,    FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE},
        { bcmActionDoNotSwitch,    FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE},
        { bcmActionDoNotSwitch,    FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE}

    },
    /* PERMIT - default behavior */
    {
        { bcmActionDoSwitch,       FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE},
        { bcmActionDoSwitch,       FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE},
        { bcmActionDoSwitch,       FILTER_ACTION_NONE, FILTER_ACTION_NONE, FILTER_ACTION_NONE}
    },
    /* REDIRECT */
    {
        { bcmActionSetPortAll,     FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* MIRROR */
    {
        { bcmActionCopyToMirror,   FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { bcmActionCopyToCpu,      bcmActionDoNotSwitch,  FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* COPY_TO_CPU */
    {
        { bcmActionCopyToCpu,      FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_COSQ */
    {
        { bcmActionSetPrio,        FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { bcmActionInsDiffServ,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_TOS */
    {
        { bcmActionInsTOSP,        FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_USERPRIO */
    {
        { bcmActionInsPrio,      FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_DROPPREC */
    {
        { bcmActionDropPrecedence, FILTER_ACTION_NONE,    FILTER_ACTION_NONE,    FILTER_ACTION_NONE},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID,   FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* ADD_OUTER_VID */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_REASON */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* PTin added: SET_COSQ_AS_PACKPRIO */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
    /* PTin added: REDIRECT_TRUNK */
    {
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID},
        { FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID, FILTER_ACTION_INVALID}
    },
};

/* Mapping from BROAD_ENTRY_t to bcm_filterid_t */
#define BROAD_ENTRY_TO_BCM_FILTER(entry)   ((bcm_filterid_t)entry)
#define BCM_FILTER_TO_BROAD_ENTRY(fid)     ((BROAD_ENTRY_t)fid)

/* Filter Management Functions */

static int _policy_filter_add_std_field(int                   unit, 
                                        bcm_filterid_t        fid, 
                                        BROAD_POLICY_FIELD_t  field, 
                                        char                 *value, 
                                        char                 *mask, 
                                        uint32               *flags)
{
    int rv;

    switch (field)
    {
    case BROAD_FIELD_MACDA:
        FFP_DEBUG_PRINT("Adding MACDA policy\r\n"); 
        rv = bcm_filter_qualify_data32(unit, fid, 0, (*((uint32 *)(value+0)) & *((uint32 *)(mask+0))),
                                       *((uint32 *)(mask+0)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        rv = bcm_filter_qualify_data16(unit, fid, 4, (*((uint16 *)(value+4)) & *((uint16 *)(mask+4))),
                                       *((uint16 *)(mask+4)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_MACSA:
        FFP_DEBUG_PRINT("Adding MACSA policy\r\n"); 
        rv = bcm_filter_qualify_data32(unit, fid, 6, (*((uint32 *)(value+0)) & *((uint32 *)(mask+0))),
                                       *((uint32 *)(mask+0)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        rv = bcm_filter_qualify_data16(unit, fid, 10, (*((uint16 *)(value+4)) & *((uint16 *)(mask+4))),
                                       *((uint16 *)(mask+4)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
   
    case BROAD_FIELD_VLAN_FORMAT:
        if (*value & BROAD_VLAN_FORMAT_STAG)
        {
           *flags |= BCM_FILTER_PKTFMT_TAG;  
        }
        else if (*value & BROAD_VLAN_FORMAT_UNTAG) 
        {
           *flags |= BCM_FILTER_PKTFMT_UNTAG;  
        }

        rv = BCM_E_NONE;
        break;

    case BROAD_FIELD_IP_TYPE:
        if (*value & BROAD_IP_TYPE_IPV4) 
        {
           *flags |= BCM_FILTER_PKTFMT_IPV4;  
        }
        else if (*value & BROAD_IP_TYPE_IPV6) 
        {
           *flags |= BCM_FILTER_PKTFMT_IPV6;  
        }
        else if (*value & BROAD_IP_TYPE_NONIP) 
        {
           *flags |= BCM_FILTER_PKTFMT_NONIP;  
        }
        rv = BCM_E_NONE;
        break;

    case BROAD_FIELD_ETHTYPE:

        rv = bcm_filter_qualify_data16(unit, fid, 16, *((uint16 *)(value+0)), 0xFFFF);
        FFP_DEBUG_PRINT("Adding ETHTYPE policy - returned %d\r\n", rv); 
        if (BCM_E_PARAM == rv)
        {
            /* ETHTYPE, update the packet format flags. */
            switch (*((uint16 *)value+0))
            {
            case 0x0800:
                *flags |= BCM_FILTER_PKTFMT_IPV4;
                rv = BCM_E_NONE;
                break;
            case 0x86DD:
                *flags |= BCM_FILTER_PKTFMT_IPV6;
                rv = BCM_E_NONE;
                break;

            default:
                /* not specified -- let caller decide */
                rv = BCM_E_PARAM;
                break;
            }
        }
        break;
    case BROAD_FIELD_OVID:
        FFP_DEBUG_PRINT("Adding OVID/OCOS policy\r\n"); 
        FFP_DEBUG_PRINT("4 - bcm_filter_qualify_data8 fid = %d\r\n", fid); 
        rv = bcm_filter_qualify_data16(unit, fid, 14, *((uint16 *)(value+0)), 
                                       *((uint16 *)(mask+0)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }

        /* HACK ALERT!!. There is no clean way of determining whether tagged pkts are to be matched.
         * For dot1p policies, pkt format must be set to tagged, else untagged packets get hit too.
         */
        if (*((uint16 *)(mask+0)) & 0xe000)
        {
          *flags |= BCM_FILTER_PKTFMT_TAG;
        }
        break;
    case BROAD_FIELD_IVID:
        FFP_DEBUG_PRINT("Adding IVID/ICOS policy\r\n"); 
        rv = bcm_filter_qualify_data16(unit, fid, 18, (*((uint16 *)(value+0)) & *((uint16 *)(mask+0))), 
                                       *((uint16 *)(mask+0)));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        *flags |= BCM_FILTER_PKTFMT_INNER_TAG;
        break;
    case BROAD_FIELD_DSCP:
        FFP_DEBUG_PRINT("Adding DSCP policy\r\n"); 
        rv = bcm_filter_qualify_data8(unit, fid, 19, ((*(uint8 *)value) & *(uint8 *)mask), 
                                      *((uint8 *)mask));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_PROTO:
        FFP_DEBUG_PRINT("Adding PROTO policy\r\n"); 
        rv = bcm_filter_qualify_data8(unit, fid, 27, ((*(uint8 *)value) & *(uint8 *)mask), 
                                      *((uint8 *)mask));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_SIP:
        FFP_DEBUG_PRINT("Adding SIP policy\r\n"); 
        rv = bcm_filter_qualify_data32(unit, fid, 30, 
                                       (*((uint32 *)value) & *((uint32 *)mask)), 
                                       *((uint32 *)mask));

        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_DIP:
        FFP_DEBUG_PRINT("Adding DIP policy\r\n"); 
        rv = bcm_filter_qualify_data32(unit, fid, 34,
                                       (*((uint32 *)value) & *((uint32 *)mask)), 
                                       *((uint32 *)mask)); 

        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_SPORT:
        FFP_DEBUG_PRINT("Adding SPORT policy\r\n"); 
        rv = bcm_filter_qualify_data16(unit, fid, 38, (*((uint16 *)value) & *((uint16 *)mask)),
                                       *((uint16 *)mask));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    case BROAD_FIELD_DPORT:
        FFP_DEBUG_PRINT("Adding DPORT policy\r\n"); 
        rv = bcm_filter_qualify_data16(unit, fid, 40, (*((uint16 *)value) & *((uint16 *)mask)), 
                                       *((uint16 *)mask));
        if (BCM_E_NONE != rv)
        {
          LOG_ERROR(rv);
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

static int _policy_filter_add_field(int                   unit, 
                                    bcm_filterid_t        fid, 
                                    BROAD_POLICY_FIELD_t  field, 
                                    char                 *value, 
                                    char                 *mask, 
                                    uint32               *flags)
{
    int  rv;

    /* User defined fields not supported by xgs2 */

    rv = _policy_filter_add_std_field(unit, fid, field, value, mask, flags);

    return rv;
}
static void _policy_filter_get_action_parms(BROAD_ACTION_ENTRY_t       *actionPtr,
                                            BROAD_POLICY_ACTION_t       action, 
                                            BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                            L7_uint32                  *param0,
                                            L7_uint32                  *param1)
{
  *param0 = 0;
  *param1 = 0;

  switch (action)
  {
  case BROAD_ACTION_REDIRECT:
  case BROAD_ACTION_MIRROR:
    *param0 = actionPtr->parms.modid;
    *param1 = actionPtr->parms.modport;
    break;

  /* PTin added */
  case BROAD_ACTION_REDIRECT_TRUNK:
    *param0 = actionPtr->parms.trunkid;
    break;

  case BROAD_ACTION_SET_COSQ:
    *param0 = actionPtr->parms.set_cosq[action_scope];
    break;

  case BROAD_ACTION_SET_DSCP:
    *param0 = actionPtr->parms.set_dscp[action_scope];
    break;

  case BROAD_ACTION_SET_TOS:
    *param0 = actionPtr->parms.set_tos[action_scope];
    break;

  case BROAD_ACTION_SET_USERPRIO:
    *param0 = actionPtr->parms.set_userprio[action_scope];
    break;

  case BROAD_ACTION_SET_DROPPREC:
    *param0 = actionPtr->parms.set_dropprec.conforming;
    break;

  // PTin added
  case BROAD_ACTION_SET_REASON_CODE:
    *param0 = actionPtr->parms.set_reason;
    break;

  case BROAD_ACTION_SET_COSQ_AS_PKTPRIO: /* PTin added */
  default:
    break;
  }
}
static int _policy_filter_add_actions(int                        unit, 
                                      bcm_filterid_t             fid, 
                                      BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
  BROAD_ACTION_ENTRY_t       *actPtr;
  BROAD_POLICY_ACTION_t       action;
  BROAD_POLICY_ACTION_SCOPE_t action_scope;
  int                         rv = BCM_E_NONE;
  int                         i;
  bcm_filter_action_t         bcm_action;
  int                         param0, param1;

  actPtr = &rulePtr->actionInfo;

  /* map BROAD_ACTION_t to corresponding BCM action(s) */
  for (action = 0; action < BROAD_ACTION_LAST; action++)
  {
    for (i = 0; i < FILTER_ACTIONS_PER_MAP_ENTRY; i++)
    {
      for (action_scope = BROAD_POLICY_ACTION_CONFORMING; action_scope < BROAD_POLICY_ACTION_LAST; action_scope++)
      {
        if ((action_scope == BROAD_POLICY_ACTION_EXCEEDING) && BROAD_EXCEEDING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = filter_action_map[action].yellow[i];
        }
        else if ((action_scope == BROAD_POLICY_ACTION_NONCONFORMING) && BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = filter_action_map[action].red[i];
        }
        else if ((action_scope == BROAD_POLICY_ACTION_CONFORMING) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = filter_action_map[action].green[i];
        }
        else
        {
          /* Action is not specified. */
          continue;
        }
  
        /* handle case of redirect to invalid port, e.g. LAG with no members */
        if ((bcmActionSetPortAll == bcm_action) && (BCMX_LPORT_INVALID == actPtr->parms.modid))
        {
          bcm_action = bcmActionDoNotSwitch;
          printf ("_policy_filter_add_action: HANDLE INVALID LPORT\n");
          continue;
        }
  
        if (FILTER_ACTION_INVALID == bcm_action)
        {
          return BCM_E_CONFIG;
        }
        else if (FILTER_ACTION_NONE != bcm_action)
        {
          _policy_filter_get_action_parms(actPtr, action, action_scope, &param0, &param1);
          if (BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(actPtr, action))
          {
            FFP_DEBUG_PRINT("6 - bcm_filter_action_out_profile fid = %d, action = %d, param %d\r\n",
                            fid, bcm_action, param0);
            /* set out-of-profile packet actions */
            rv = bcm_filter_action_out_profile(unit, fid, bcm_action, 
                                               param0, rulePtr->u.meter.meterInfo.ffpmtr_id); 
            if (BCM_E_NONE != rv)
              return rv;
          }
          else
          {
            FFP_DEBUG_PRINT("set action fid = %d, action = %d param %d\r\n", fid, bcm_action, param0);
            if (bcmActionSetPortAll == bcm_action)
            {
              rv = bcm_filter_action_match(unit, fid, bcm_action, param1); 
              if (rv == BCM_E_NONE)
              {
                rv = bcm_filter_action_match(unit, fid, bcmActionSetModule, param0);
                if (rv == BCM_E_NONE)
                {
                  /* Set the untag bit for redirect policies, else redirect pkts will be tagged */
                  rv = bcm_filter_action_match(unit, fid, bcmActionSetPortUntag, 1);
                }
              }
            }
            /* set remaining actions */
            else
            {
              rv = bcm_filter_action_match(unit, fid, bcm_action, param0);
            }
  
            if (BCM_E_NONE != rv)
              return rv;
          }
        }
      }
    }
  }

  return rv;
}


static int _policy_add_meter(int unit, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                  rv = BCM_E_NONE;
    uint64               zero64;
    BROAD_METER_ENTRY_t *meterPtr;

    meterPtr = &rulePtr->u.meter.meterInfo;

    FFP_DEBUG_PRINT("1 - bcm_meter_create unit = %d, port = %d\r\n", unit, meterPtr->port);
    /* create the meter and the out-of-profile counter */
    rv = bcm_meter_create(unit, meterPtr->port, &meterPtr->ffpmtr_id);
    if (BCM_E_NONE != rv)
        return rv;

    FFP_DEBUG_PRINT("2 - bcm_meter_set ffpmtr_id = %d, cir = %d\r\n", meterPtr->ffpmtr_id, meterPtr->cir);
    /* all meters have cir/cbs */
    rv = bcm_meter_set(unit, meterPtr->port, meterPtr->ffpmtr_id, meterPtr->cir, meterPtr->cbs);
    if (BCM_E_NONE != rv)
        return rv;

    /* zero values prior to first use */
    COMPILER_64_ZERO(zero64);
    (void)bcm_ffppacketcounter_set(unit, meterPtr->port, meterPtr->ffpmtr_id, zero64);

    return rv;
}

static int _policy_add_counter(int unit, bcm_filterid_t fid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                    rv = BCM_E_NONE;
    uint64                 zero64;
    BROAD_COUNTER_ENTRY_t *counterPtr;

    counterPtr = &rulePtr->u.counter.counterInfo;

    FFP_DEBUG_PRINT("Adding Counter - unit = %d, port = %d\r\n", unit, counterPtr->port);
    /* xgs2 only supports per-packet counting */
    if (BROAD_COUNT_PACKETS != counterPtr->mode)
      return BCM_E_UNAVAIL;

    /* add counter to support stats */
    rv = bcm_ffpcounter_create(unit, counterPtr->port, &counterPtr->ffpcntr_id);
    if (BCM_E_NONE != rv)
        return rv;

    FFP_DEBUG_PRINT("bcm_ffpcounter_create: Returned ID %d \r\n", counterPtr->ffpcntr_id);

    /* zero values prior to first use */
    COMPILER_64_ZERO(zero64);
    (void)bcm_ffpcounter_set(unit, counterPtr->port, counterPtr->ffpcntr_id, zero64);

    rv = bcm_filter_action_match(unit, fid, bcmActionIncrCounter, counterPtr->ffpcntr_id);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

int policy_filter_add_rule(int                        unit, 
                           BROAD_POLICY_RULE_ENTRY_t *rulePtr, 
                           BROAD_ENTRY_t             *entry,
                           int                       prio)                           
{
    BROAD_POLICY_FIELD_t       f;
    bcm_filterid_t             fid;
    int                        rv = BCM_E_NONE;
    uint32                     pktFlags;
    pbmp_t                     ipbm;

    rv = bcm_filter_create(unit, &fid);
    if (BCM_E_NONE != rv)
        return rv;

    FFP_DEBUG_PRINT("3 - bcm_filter_create unit = %d, fid = %d\r\n", unit, fid);

    *entry = BCM_FILTER_TO_BROAD_ENTRY(fid);

    /* Specify which packet formats are of interest. */
    pktFlags = 0x0000;

    /* Add all the fields */
    for (f = 0; f < BROAD_FIELD_LAST; f++)
    {
        if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, f) == BROAD_FIELD_SPECIFIED)
        {

          /* Qualify on all specified fields.
           */
          rv = _policy_filter_add_field(unit, fid, f, 
                                        hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, f), 
                                        hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, f), 
                                        &pktFlags);
        
          if (BCM_E_NONE != rv)
          {
            return rv;
          }
        }
    }

    /* add meters or counters, if any, but not both */
    if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        FFP_DEBUG_PRINT("- adding a meter\n");

        rv = _policy_add_meter(unit, rulePtr);
        if (BCM_E_NONE != rv)
            return rv;
    }
    else if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        FFP_DEBUG_PRINT("- adding a counter\n");

        rv = _policy_add_counter(unit, fid, rulePtr);
        if (BCM_E_NONE != rv)
            return rv;
    }
    /* Qualify on PacketFormat to handle untagged, tagged, ipv4/6, etc. */
    if (pktFlags)
    {
      rv = bcm_filter_qualify_format(unit, fid, (bcm_filter_format_t)pktFlags);
      if (BCM_E_NONE !=rv)
        return rv;
    }
 
    /* Add all the actions */
    rv = _policy_filter_add_actions(unit, fid, rulePtr);
    if (BCM_E_NONE != rv)
        return rv;

    FFP_DEBUG_PRINT("8 - bcm_filter_qualify_priority prio = %d fid = %d\r\n", prio, fid);
    rv = bcm_filter_qualify_priority(unit, fid, prio);
    if (BCM_E_NONE != rv)
        return rv;

    /* Got to qualify on ingress. Else, multiple Dot1p policies will conflict */
    BCM_PBMP_CLEAR(ipbm);
    rv = bcm_filter_qualify_ingress(unit, fid, ipbm);   
    if (BCM_E_NONE != rv)
        return rv;
    
    FFP_DEBUG_PRINT("8 - bcm_filter_install fid = %d\r\n", fid);
    rv = bcm_filter_install(unit, fid);
    if (BCM_E_NONE != rv)
    {
      FFP_DEBUG_PRINT("bcm_filter_install returned error = %s\r\n", bcm_errmsg(rv));
      return rv;
    }

    return rv;
}

int policy_filter_set_ipbm(int           unit, 
                           BROAD_ENTRY_t entry,
                           bcm_pbmp_t    ipbm,
                           bcm_pbmp_t    ipbmMask)
{
    int            rv;
    bcm_filterid_t fid;

    fid = BROAD_ENTRY_TO_BCM_FILTER(entry);

    /* Reinstall only works for actions, so remove and install the filter explicitly. */
    rv = bcm_filter_remove(unit, fid);
    if (BCM_E_NONE != rv)
        return rv;

    BCM_PBMP_AND(ipbm, ipbmMask);

    FFP_DEBUG_PRINT("5 - bcm_filter_qualify_ingress fid = %d\r\n", fid);

    rv = bcm_filter_qualify_ingress(unit, fid, ipbm);
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_filter_install(unit, fid);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

int policy_filter_delete(int           unit, 
                         BROAD_ENTRY_t entry)
{
    int rv;
    bcm_filterid_t fid;

    fid = BROAD_ENTRY_TO_BCM_FILTER(entry);

    FFP_DEBUG_PRINT("5 - bcm_filter_remove fid = %d\r\n", fid);

    rv = bcm_filter_remove(unit, fid);
    if (BCM_E_NONE != rv)
    {
       FFP_DEBUG_PRINT("bcm_filter_remove returned error = %s\r\n", bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_filter_destroy(unit, fid);
    if (BCM_E_NONE != rv)
        return rv;
    
    return rv;
}

int policy_filter_delete_rule(int   unit, 
                              BROAD_FFP_RULE_INFO_t *ruleInfo)   
                              
{
    int rv;

    if (ruleInfo->meter_flags & BROAD_METER_SPECIFIED)
    {
      rv = bcm_meter_delete(unit, ruleInfo->mtr_port, ruleInfo->ffpmtr_id);
    }
    else if (ruleInfo->cntr_flags & BROAD_COUNTER_SPECIFIED )
    {
      rv = bcm_ffpcounter_delete(unit, ruleInfo->cntr_port, ruleInfo->ffpcntr_id);
    }

    rv = policy_filter_delete(unit, ruleInfo->entry);

    return rv;
}

int policy_filter_get_stats(int                        unit, 
                            BROAD_FFP_RULE_INFO_t     *ruleInfo,
                            uint64                    *val1,
                            uint64                    *val2)
{
    int rv;
    uint64 out_of_profile;
    uint64 in_flow;

    COMPILER_64_ZERO(out_of_profile);
    COMPILER_64_ZERO(in_flow);
    COMPILER_64_ZERO(*val1);
    COMPILER_64_ZERO(*val2);

    if (ruleInfo->meter_flags & BROAD_METER_SPECIFIED)
    {
       rv = bcm_ffppacketcounter_get(unit, 
                                     ruleInfo->mtr_port,
                                     ruleInfo->ffpmtr_id, &out_of_profile);

       FFP_DEBUG_PRINT("bcm_ffppacketcounter_get returned %d\r\n", rv);

       if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
          return rv;
    }

    if (ruleInfo->cntr_flags & BROAD_COUNTER_SPECIFIED )
    {
       rv = bcm_ffpcounter_get(unit, 
                               ruleInfo->cntr_port,
                               ruleInfo->ffpcntr_id, &in_flow);

       FFP_DEBUG_PRINT("bcm_ffpcounter_get returned %d\r\n", rv);

       if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
          return rv;
    }

    *val1 = out_of_profile;
    *val2 = in_flow - out_of_profile; /* Calculate in-profile packet count */

    return BCM_E_NONE;
}
