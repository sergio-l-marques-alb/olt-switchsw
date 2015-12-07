/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename  broad_group_shuffle.c
*
* @component hapi
*
* @create    1/27/2010
*
* @author    colinw
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "commdefs.h"
#include "sal/types.h"
#include "broad_group_shuffle.h"
#include "broad_group_sqset.h"
#include "broad_policy_types.h"
#include "broad_group_xgs3.h"
#include "l7_usl_policy_db.h"

/* PTin added: includes */
#include "logger.h"

/* Group Definitions */

#define GROUP_NONE               0
#define GROUP_USED               1
#define GROUP_EFP_ON_IFP         2

typedef struct
{
  unsigned int        flags;
  int                 baseGroup;
  BROAD_POLICY_TYPE_t type;
  int                 sqset;  /* super qset this group was created from */
  int                 rulesUsed;
  int                 countersUsed;
  int                 metersUsed;
} model_group_table_t;

typedef struct
{
  BROAD_GROUP_t                  group;
  bcm_pbmp_t                     pbm;
  sqsetWidth_t                   minQsetWidth;
} model_policy_table_t;

typedef struct shuffle_counter_info_s
{
  BROAD_POLICY_t                 policy;
  int                            ruleId;
  uint64                         val1;
  uint64                         val2;
  struct shuffle_counter_info_s *next;
} shuffle_counter_info_t;

static model_group_table_t    *model_group_table         = L7_NULL;
static int                     model_group_table_size    = 0;
static model_policy_table_t   *model_policy_table        = L7_NULL;
static shuffle_counter_info_t *counter_info              = L7_NULL;

static int _policy_model_group_table_init(int unit, BROAD_POLICY_STAGE_t policyStage)
{
  int i;
  int new_table_size;

  if (model_policy_table == L7_NULL)
  {
    model_policy_table = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES * sizeof(*model_policy_table));
    if (model_policy_table == L7_NULL)
    {
      return BCM_E_MEMORY;
    }
  }
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    model_policy_table[i].group        = BROAD_GROUP_INVALID;
    model_policy_table[i].minQsetWidth = sqsetWidthSingle;
    BCM_PBMP_CLEAR(model_policy_table[i].pbm);
  }

  new_table_size = _policy_group_total_slices(unit, policyStage);

  /* If the table size is changing and we previously allocated a table, free it now. */
  if ((new_table_size != model_group_table_size) && (model_group_table != L7_NULL))
  {
    osapiFree(L7_DRIVER_COMPONENT_ID, model_group_table);
    model_group_table = L7_NULL;
  }

  model_group_table_size = new_table_size;
  if (model_group_table == L7_NULL)
  {
    model_group_table = osapiMalloc(L7_DRIVER_COMPONENT_ID, model_group_table_size * sizeof(model_group_table_t));
    if (model_group_table == L7_NULL)
    {
      return BCM_E_MEMORY;
    }
  }

  for (i = 0; i < model_group_table_size; i++)
  {
    memset(&model_group_table[i], 0, sizeof(model_group_table_t));
    model_group_table[i].flags = GROUP_NONE;
  }

  return BCM_E_NONE;
}

static int _policy_model_group_find_first(int                  unit, 
                                          BROAD_POLICY_STAGE_t policyStage,
                                          BROAD_POLICY_TYPE_t  type, 
                                          BROAD_GROUP_t       *group)
{
  group_alloc_block_t  block, used_block;
  group_alloc_dir_t    dir, used_dir;
  model_group_table_t *groupPtr;
  super_qset_entry_t   sqsetInfo;

  _policy_group_alloc_type(type, &block, &dir);

  if (ALLOC_HIGH_TO_LOW == dir)
  {
    *group =_policy_group_block_high_prio_get(unit, policyStage, block);
  }
  else
  {
    *group =_policy_group_block_low_prio_get(unit, policyStage, block);
  }

  groupPtr = &model_group_table[*group];
  if (groupPtr->flags & GROUP_USED)
  {
    /* Set group to the first slice of the group. */
    _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
    *group -= *group % sqsetInfo.status.slice_width_physical;
    groupPtr = &model_group_table[*group];

    /* make sure that the block that this group belongs to matches
       the block requested */
    if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
    {
      return BCM_E_FAIL;
    }
    _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
    if (block != used_block)
    {
      return BCM_E_FAIL;
    }
  }

  return BCM_E_NONE;
}

static int _policy_model_group_decrement(int                   unit,
                                         BROAD_POLICY_STAGE_t  policyStage,
                                         BROAD_GROUP_t        *group, 
                                         group_alloc_block_t   block, 
                                         BROAD_POLICY_TYPE_t   type)
{
  group_alloc_block_t  used_block;
  group_alloc_dir_t    used_dir;
  model_group_table_t *groupPtr;
  super_qset_entry_t   sqsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the first slice. */
  if (*group < model_group_table_size)
  {
    groupPtr = &model_group_table[*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group -= *group % sqsetInfo.status.slice_width_physical;
    }
  }

  if ((*group - 1) >= _policy_group_block_low_prio_get(unit, policyStage, block))
  {
    *group = *group - 1;

    groupPtr = &model_group_table[*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group -= *group % sqsetInfo.status.slice_width_physical;
      groupPtr = &model_group_table[*group];

      /* make sure that the block that this group belongs to matches
         the block requested */
      if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
      {
        return BCM_E_FAIL;
      }
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
    }

    return BCM_E_NONE;
  }

  return BCM_E_FAIL;
}

static int _policy_model_group_increment(int                   unit, 
                                         BROAD_POLICY_STAGE_t  policyStage,
                                         BROAD_GROUP_t        *group, 
                                         group_alloc_block_t   block, 
                                         BROAD_POLICY_TYPE_t   type)
{
  group_alloc_block_t  used_block;
  group_alloc_dir_t    used_dir;
  model_group_table_t *groupPtr;
  super_qset_entry_t   sqsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the last slice. */
  if (*group < model_group_table_size)
  {
    groupPtr = &model_group_table[*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the last slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group += sqsetInfo.status.slice_width_physical - 1;
    }
  }

  while ((*group + 1) <= _policy_group_block_high_prio_get(unit, policyStage, block))
  {
    *group = *group + 1;

    groupPtr = &model_group_table[*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* make sure that the block that this group belongs to matches
         the block requested */
      if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
      {
        return BCM_E_FAIL;
      }
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
    }

    return BCM_E_NONE;
  }

  return BCM_E_FAIL;
}

static int _policy_model_group_find_next(int                  unit, 
                                         policy_map_table_t   *policyPtr,
                                         BROAD_GROUP_t        *group)
{
  group_alloc_block_t  block;
  group_alloc_dir_t    dir;
  int                  retval = BCM_E_FAIL;

  _policy_group_alloc_type(policyPtr->policyType, &block, &dir);

  /* Do not allow the high and low groups to co-mingle. */
  if (ALLOC_HIGH_TO_LOW == dir)
  {
    retval = _policy_model_group_decrement(unit, policyPtr->policyStage, group, block, policyPtr->policyType);
  }
  else
  {
    retval = _policy_model_group_increment(unit, policyPtr->policyStage, group, block, policyPtr->policyType);
  }

  return retval;
}
static L7_BOOL _policy_modeled_group_is_available(int group, int physical_slice_width_for_group)
{
  int     i;
  L7_BOOL rv = L7_TRUE;

  if ((group + physical_slice_width_for_group) > model_group_table_size)
  {
    return L7_FALSE;
  }

  for (i = group; i < (group + physical_slice_width_for_group); i++)
  {
    if (model_group_table[i].flags & GROUP_USED)
    {
      rv = L7_FALSE;
      break;
    }
  }

  return rv;
}

static int _policy_model_group_resource_check(int                  unit,
                                              policy_map_table_t  *policyPtr,
                                              int                  group)
{
  int                  rv                = BCM_E_NONE;
  int                  rulesAvailable    = 0;
  int                  countersAvailable = 0;
  int                  metersAvailable   = 0;
  int                  i;
  int                  sqset;
  super_qset_entry_t   policySqsetInfo, sqsetInfo;
  model_group_table_t *tempGroupPtr;

  /* Calculate available resources based on available slices and nature of sqset. */

  if (_policy_minimal_sqset_get(unit, policyPtr->policyType, &policyPtr->resourceReq, &sqset) != BCM_E_NONE)
  {
    /* This shouldn't occur as we've already found some matching qset before. */
    return BCM_E_FAIL;
  }
  _policy_sqset_get(unit, sqset,           &policySqsetInfo);

  tempGroupPtr = &model_group_table[group];
  _policy_sqset_get(unit, tempGroupPtr->sqset, &sqsetInfo);

  for (i = 0; i < model_group_table_size; i += sqsetInfo.status.slice_width_physical)
  {
    tempGroupPtr = &model_group_table[i];
    if ((tempGroupPtr->flags & GROUP_USED) && (tempGroupPtr->baseGroup == group))
    {
      rulesAvailable    += (sqsetInfo.status.natural_depth - tempGroupPtr->rulesUsed);
      countersAvailable += (sqsetInfo.status.natural_depth - tempGroupPtr->countersUsed);
      metersAvailable   += (sqsetInfo.status.natural_depth - tempGroupPtr->metersUsed);
    }
    else if (_policy_modeled_group_is_available(i, sqsetInfo.status.slice_width_physical))
    {
      /* If virtual slice grouping is supported, then the number of total 
         entries will be greater than the natural_depth of the slice group. 
         In this case, add up the available resources for the unused groups. 
         Note however, that if the group being checked is wider than 
         the policy requires, we don't want to waste slices to expand this group.
         Instead we will try to allocate a more narrow group later on. */
      if ((sqsetInfo.status.entries_total > sqsetInfo.status.natural_depth) &&
          (policySqsetInfo.sqsetWidth == sqsetInfo.sqsetWidth))
      {
        rulesAvailable    += sqsetInfo.status.natural_depth;
        countersAvailable += sqsetInfo.status.natural_depth;
        metersAvailable   += sqsetInfo.status.natural_depth;
      }
    }
  }

  if ((rulesAvailable    >= policyPtr->resourceReq.ruleCount) &&
      (countersAvailable >= policyPtr->resourceReq.counterCount) &&
      (metersAvailable   >= policyPtr->resourceReq.meterCount))
  {
    rv = BCM_E_NONE;
  }
  else
  {
    rv = BCM_E_RESOURCE;
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
  {
    sysapiPrintf("  MODEL: Entries free/needed %d/%d, counters free/needed %d/%d, meters free/needed %d/%d\n",
           rulesAvailable,    policyPtr->resourceReq.ruleCount, 
           countersAvailable, policyPtr->resourceReq.counterCount, 
           metersAvailable,   policyPtr->resourceReq.meterCount);
  }

  return rv;
}

void _policy_model_policy_store_in_groups(int unit, policy_map_table_t *policyPtr, int sqset, BROAD_GROUP_t baseGroup)
{
  int                  i;
  int                  tempGroup;
  int                  rulesRemaining, countersRemaining, metersRemaining;
  int                  rulesAvailable, countersAvailable, metersAvailable;
  int                  rulesAdded = 0, countersAdded = 0, metersAdded = 0;
  model_group_table_t *groupPtr;
  super_qset_entry_t   sqsetInfo;
  group_alloc_block_t  block;
  group_alloc_dir_t    dir;

  _policy_group_alloc_type(policyPtr->policyType, &block, &dir);

  rulesRemaining    = policyPtr->resourceReq.ruleCount;
  countersRemaining = policyPtr->resourceReq.counterCount;
  metersRemaining   = policyPtr->resourceReq.meterCount;

  _policy_sqset_get(unit, sqset, &sqsetInfo);

  for (i = baseGroup; i < (baseGroup + sqsetInfo.status.slice_width_physical); i++)
  {
    groupPtr = &model_group_table[i];

    groupPtr->flags = GROUP_USED;
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      groupPtr->flags |= GROUP_EFP_ON_IFP;
    }
    groupPtr->type      = policyPtr->policyType;
    groupPtr->baseGroup = baseGroup;
    groupPtr->sqset     = sqset;

    rulesAvailable      = sqsetInfo.status.natural_depth - groupPtr->rulesUsed;
    countersAvailable   = sqsetInfo.status.natural_depth - groupPtr->countersUsed;
    metersAvailable     = sqsetInfo.status.natural_depth - groupPtr->metersUsed;

    rulesAdded          = min(rulesRemaining,    rulesAvailable);
    countersAdded       = min(countersRemaining, countersAvailable);
    metersAdded         = min(metersRemaining,   metersAvailable);

    groupPtr->rulesUsed    += rulesAdded;
    groupPtr->countersUsed += countersAdded;
    groupPtr->metersUsed   += metersAdded;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    {
      if (i % sqsetInfo.status.slice_width_physical == 0)
      {
        sysapiPrintf("- MODEL: storing %d rules, %d counters, %d meters in group %d\n", rulesAdded, countersAdded, metersAdded, i);
      }
    }
  }

  rulesRemaining    -= rulesAdded;
  countersRemaining -= countersAdded;
  metersRemaining   -= metersAdded;

  if (dir == ALLOC_HIGH_TO_LOW)
  {
    tempGroup = model_group_table_size - 1;
  }
  else
  {
    tempGroup = 0;
  }
  while (rulesRemaining || countersRemaining || metersRemaining)
  {
    /* Find free groups to accommodate the rest of the policy resource requirements. */
    /* Enforce slice boundary */
    if ((tempGroup % sqsetInfo.status.slice_width_physical) == 0)
    {
      groupPtr = &model_group_table[tempGroup];
      if (((groupPtr->flags & GROUP_USED) && (groupPtr->baseGroup == baseGroup)) ||
          _policy_modeled_group_is_available(tempGroup, sqsetInfo.status.slice_width_physical))
      {
        for (i = tempGroup; i < (tempGroup + sqsetInfo.status.slice_width_physical); i++)
        {
          groupPtr = &model_group_table[i];

          groupPtr->flags = GROUP_USED;
          if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
          {
            groupPtr->flags |= GROUP_EFP_ON_IFP;
          }
          groupPtr->type      = policyPtr->policyType;
          groupPtr->baseGroup = baseGroup;
          groupPtr->sqset     = sqset;

          rulesAvailable      = sqsetInfo.status.natural_depth - groupPtr->rulesUsed;
          countersAvailable   = sqsetInfo.status.natural_depth - groupPtr->countersUsed;
          metersAvailable     = sqsetInfo.status.natural_depth - groupPtr->metersUsed;

          rulesAdded          = min(rulesRemaining,    rulesAvailable);
          countersAdded       = min(countersRemaining, countersAvailable);
          metersAdded         = min(metersRemaining,   metersAvailable);

          groupPtr->rulesUsed    += rulesAdded;
          groupPtr->countersUsed += countersAdded;
          groupPtr->metersUsed   += metersAdded;

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          {
            if (i % sqsetInfo.status.slice_width_physical == 0)
            {
              sysapiPrintf("- MODEL: storing %d rules, %d counters, %d meters in group %d\n", rulesAdded, countersAdded, metersAdded, i);
            }
          }
        }

        rulesRemaining    -= rulesAdded;
        countersRemaining -= countersAdded;
        metersRemaining   -= metersAdded;
      }
    }

    if (dir == ALLOC_HIGH_TO_LOW)
    {
      tempGroup--;
    }
    else
    {
      tempGroup++;
    }

    if ((tempGroup < 0) || (tempGroup >= model_group_table_size))
    {
      break;
    }
  }
}

static int _policy_model_group_find_group(int                   unit,
                                          BROAD_POLICY_t        policy,
                                          policy_map_table_t   *policyPtr,
                                          BROAD_GROUP_t        *group)
{
  int                  rv;
  sqsetWidth_t         qsetWidth;
  int                  groupEfpUsingIfp, policyEfpUsingIfp;
  super_qset_entry_t   groupSqsetInfo, policySqsetInfo;
  group_alloc_block_t  block;
  group_alloc_dir_t    dir;
  int                  minSqset;

  _policy_group_alloc_type(policyPtr->policyType, &block, &dir);

  /* Find an existing group that can satisfy the policy requirements. */
  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    rv = _policy_model_group_find_first(unit, policyPtr->policyStage, policyPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
      bcm_field_qset_t         qset;
      model_group_table_t     *groupPtr;

      groupPtr = &model_group_table[*group];

      if (groupPtr->flags & GROUP_USED)
      {
        _policy_sqset_get(unit, groupPtr->sqset, &groupSqsetInfo);
        if (groupSqsetInfo.sqsetWidth == qsetWidth)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- MODEL: examining model group %d, qsetWidth %d\n", *group, qsetWidth);

          /* Insure the group has a suitable qset and enough free entries, counters, et al. */
          BCM_FIELD_QSET_INIT(qset);
          _policy_set_union(groupSqsetInfo.qsetAgg, &qset);
          _policy_set_union(groupSqsetInfo.qsetUdf, &qset);

          rv = _policy_set_subset(policyPtr->resourceReq.qsetAgg, 
                                  policyPtr->resourceReq.customQset, 
                                  qset, 
                                  groupSqsetInfo.customQset);
          if (BCM_E_NONE == rv)
          {
            rv = _policy_model_group_resource_check(unit, policyPtr, *group);
            if (rv == BCM_E_NONE)
            {
              /* ensure that policies using IFP for EFP only go in groups using IFP for EFP */
              groupEfpUsingIfp  = (groupPtr->flags & GROUP_EFP_ON_IFP)      ? L7_TRUE : L7_FALSE;
              policyEfpUsingIfp = (policyPtr->flags & GROUP_MAP_EFP_ON_IFP) ? L7_TRUE : L7_FALSE;

              if (groupEfpUsingIfp == policyEfpUsingIfp)
              {
                /* reuse existing group */
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                  sysapiPrintf("- MODEL: reuse group %d\n", *group);

                /* Store the policy to group mapping for use later in case
                   we need to rewrite the policies. */
                if (_policy_minimal_sqset_get(unit, policyPtr->policyType, &policyPtr->resourceReq, &minSqset) == BCM_E_NONE)
                {
                  _policy_sqset_get(unit, minSqset, &policySqsetInfo);
                  model_policy_table[policy].group        = *group;
                  model_policy_table[policy].minQsetWidth = policySqsetInfo.sqsetWidth;
                  BCM_PBMP_ASSIGN(model_policy_table[policy].pbm, policyPtr->pbm);
  
                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                    sysapiPrintf("- MODEL: storing policy %d in groups\n", policy);
  
                  _policy_model_policy_store_in_groups(unit, policyPtr, groupPtr->sqset, *group);
  
                  return BCM_E_NONE;
                }
              }
            }
          }
        }
      }

      rv = _policy_model_group_find_next(unit, policyPtr, group);
    }
  }

  return rv;
}

static int _policy_model_group_alloc_group(int                   unit,
                                           BROAD_POLICY_t        policy,
                                           policy_map_table_t   *policyPtr,
                                           BROAD_GROUP_t        *group)
{
  int                    rv = BCM_E_FAIL;
  int                    sqset;
  int                    i;
  model_group_table_t   *groupPtr;
  sqsetWidth_t           qsetWidth;
  super_qset_entry_t     groupSqsetInfo, policySqsetInfo;
  group_alloc_block_t    block;
  group_alloc_dir_t      dir;
  int                    minSqset;

  _policy_group_alloc_type(policyPtr->policyType, &block, &dir);

  /* make sure that only one group can do EFP on IFP */
  if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
  {
    for (i = 0; i < model_group_table_size; i++)
    {
      if ((model_group_table[i].flags & (GROUP_USED | GROUP_EFP_ON_IFP)) == (GROUP_USED | GROUP_EFP_ON_IFP))
      {
        return BCM_E_FAIL;
      }
    }
  }

  debug_print_qset(&policyPtr->resourceReq.qsetAgg);

  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    /* create all new groups based upon a super qset */
    rv = _policy_super_qset_find_match(unit, 
                                       policyPtr->policyType, 
                                       qsetWidth, 
                                       policyPtr->resourceReq.qsetAgg, 
                                       policyPtr->resourceReq.customQset, 
                                       &sqset);
    if (BCM_E_NONE != rv)
    {
      continue;
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- MODEL: using super qset %d\n", sqset);

    _policy_sqset_get(unit, sqset, &groupSqsetInfo);

    /* Try to find a group priority that we can use to create this group. */
    rv = _policy_model_group_find_first(unit, policyPtr->policyStage, policyPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
      /* Enforce physical boundary conditions. */
      if (*group % groupSqsetInfo.status.slice_width_physical == 0)
      {
        if (_policy_modeled_group_is_available(*group, groupSqsetInfo.status.slice_width_physical))
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- MODEL: examining model group %d, qsetWidth %d\n", *group, qsetWidth);

          groupPtr = &model_group_table[*group];

          groupPtr->flags = GROUP_USED;
          if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
          {
            groupPtr->flags |= GROUP_EFP_ON_IFP;
          }
          groupPtr->type         = policyPtr->policyType;
          groupPtr->baseGroup    = *group;
          groupPtr->sqset        = sqset;
          groupPtr->rulesUsed    = 0;
          groupPtr->countersUsed = 0;
          groupPtr->metersUsed   = 0;

          rv = _policy_model_group_resource_check(unit, policyPtr, *group);
          if (rv == BCM_E_NONE)
          {
            /* This policy will fit... update the group resources. */
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("- MODEL: storing policy %d in groups\n", policy);

            if (_policy_minimal_sqset_get(unit, policyPtr->policyType, &policyPtr->resourceReq, &minSqset) == BCM_E_NONE)
            {
              _policy_sqset_get(unit, minSqset, &policySqsetInfo);

              _policy_model_policy_store_in_groups(unit, policyPtr, sqset, *group);

              /* Store the policy to group mapping for use later in case
                 we need to rewrite the policies. */
              model_policy_table[policy].group        = *group;
              model_policy_table[policy].minQsetWidth = policySqsetInfo.sqsetWidth;
              BCM_PBMP_ASSIGN(model_policy_table[policy].pbm, policyPtr->pbm);

              break;
            }
          }
          else
          {
            /* The policy doesn't fit... clean up. */
            groupPtr->flags = GROUP_NONE;
          }
        }
      }

      rv = _policy_model_group_find_next(unit, policyPtr, group);
    }

    if (rv == BCM_E_NONE)
    {
      break;
    }
  }

  return rv;
}

static int _policy_model_group_create(int                   unit, 
                                      BROAD_POLICY_t        policy,
                                      policy_map_table_t   *policyPtr)
{
  int                  rv;
  L7_BOOL              reworkQset;
  bcm_field_qualify_t  reworkQualifier = bcmFieldQualifyIpType;
  BROAD_POLICY_STAGE_t policyStage;
  BROAD_GROUP_t        group;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- MODEL: creating policy %d\n", policy);

  policyStage = policyPtr->policyStage;

  rv = _policy_model_group_find_group(unit, policy, policyPtr, &group);

  if (BCM_E_NONE != rv)
  {
    /* see if there is an alternative qset already created */
    reworkQset = L7_FALSE;
    if ((policyPtr->resourceReq.requiresEtype == 0)  && 
        BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIpType))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIpType;
    }
    else if ((policyPtr->resourceReq.requiresEtype == 0)  && 
             BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIp4))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIp4;
    }
    else if ((policyPtr->resourceReq.requiresEtype == 0)  && 
             BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIp6))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIp6;
    }

    if (reworkQset == L7_TRUE)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- MODEL: couldn't find group, reworking qset %d\n", policy);

      BCM_FIELD_QSET_REMOVE(policyPtr->resourceReq.qsetAgg, reworkQualifier);
      BCM_FIELD_QSET_ADD(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyEtherType);
      rv = _policy_model_group_find_group(unit, policy, policyPtr, &group);

      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("- MODEL: couldn't find group, restoring qset %d\n", policy);

        BCM_FIELD_QSET_ADD(policyPtr->resourceReq.qsetAgg, reworkQualifier);
        BCM_FIELD_QSET_REMOVE(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyEtherType);
      }
    }
  }

  if (BCM_E_NONE != rv)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- MODEL: allocate new group\n");

    rv = _policy_model_group_alloc_group(unit, policy, policyPtr, &group);

    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- MODEL: allocate new group FAILED\n");

      reworkQset = L7_FALSE;
      if ((policyPtr->resourceReq.requiresEtype == 0)  && 
          BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIpType))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIpType;
      }
      else if ((policyPtr->resourceReq.requiresEtype == 0)  && 
               BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIp4))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp4;
      }
      else if ((policyPtr->resourceReq.requiresEtype == 0)  && 
               BCM_FIELD_QSET_TEST(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyIp6))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp6;
      }

      if (reworkQset == L7_TRUE)
      {
        /*
         * attempt to rework the qset:
         * ethertype was not required due to an ethertype of 0800 or 86dd,
         * but let's try etherType in case there exists a group with this setting
         * remove the bcmFieldQualifyIpType from the qset and search again
         */
        BCM_FIELD_QSET_REMOVE(policyPtr->resourceReq.qsetAgg, reworkQualifier);
        BCM_FIELD_QSET_ADD(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyEtherType);

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("- MODEL: allocate new group\n");

        rv = _policy_model_group_alloc_group(unit, policy, policyPtr, &group);

        if (BCM_E_NONE != rv)
        {
          BCM_FIELD_QSET_ADD(policyPtr->resourceReq.qsetAgg, reworkQualifier);
          BCM_FIELD_QSET_REMOVE(policyPtr->resourceReq.qsetAgg, bcmFieldQualifyEtherType);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- MODEL: allocate new group FAILED\n");
        }
      }
    }
  }

  if (rv == BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- MODEL: created group\n");
  }

  return rv;
}

static int _policy_group_rewrite_check(int unit, BROAD_POLICY_ENTRY_t *policyData)
{
  int                rv;
  sqsetWidth_t       qsetWidth;
  L7_uint32          policy;
  L7_uint32          minSqset;
  super_qset_entry_t minSqsetInfo;
  policy_map_table_t policyInfo;

  rv = _policy_model_group_table_init(unit, policyData->policyStage);
  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- checking full rewrite of policies\n");

  /* The goal here is to model what the groups would look like if 
     all the policies were written in order of the widest to narrowest.
     This approach results in the most densely and efficiently packed groups and 
     provides the maximum potential freeable slices. */
  for (qsetWidth = sqsetWidthLast - 1; ((qsetWidth >= sqsetWidthFirst) && (qsetWidth < sqsetWidthLast)); qsetWidth--)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- MODEL: checking policies of width %d\n", qsetWidth);

    /* Find all the policies that use this width and write them to the model. */
    for (policy = 0; policy < BROAD_MAX_POLICIES; policy++)
    {
      if (_policy_map_get_info(unit, policy, &policyInfo) != BCM_E_NONE)
      {
        /* This policy doesn't exist on this BCM unit. */
        continue;
      }

      /* Only need to be concerned w/ the stage of the new policy. */
      if (policyInfo.policyStage != policyData->policyStage)
      {
        /* We're not concerned about this policy since it's in
           a different stage. */
        continue;
      }

      rv = _policy_minimal_sqset_get(unit, policyInfo.policyType, &policyInfo.resourceReq, &minSqset);
      if (rv != BCM_E_NONE)
      {
        /* There is no sqset to accommodate this policy, therefore even a 
           policy shuffle won't help fit the new policy. */
        break;
      }

      _policy_sqset_get(unit, minSqset, &minSqsetInfo);

      if (minSqsetInfo.sqsetWidth != qsetWidth)
      {
        /* We're not concerned about this policy during this iteration. */
        continue;
      }

      rv = _policy_model_group_create(unit, policy, &policyInfo);
      if (rv != BCM_E_NONE)
      {
        /* Not all policies can fit, therefore even a policy shuffle
           won't help fit the new policy. */
        break;
      }
    } /* for policy */

    if (rv != BCM_E_NONE)
    {
      /* Not all policies can fit, therefore even a policy shuffle
         won't help fit the new policy. */
      break;
    }
  } /* for qsetWidth */

  return rv;
}

static int _policy_policy_destroy_with_counter_backup(int unit, BROAD_POLICY_t policy)
{
  int                     rv = BCM_E_NONE;
  int                     ruleId;
  uint64                  val1;
  uint64                  val2;
  shuffle_counter_info_t *counterPtr;
  policy_map_table_t      policyInfo;

  if (_policy_map_get_info(unit, policy, &policyInfo) == BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- SHUFFLE: destroying policy %d with counter backup\n", policy);

    if (policyInfo.resourceReq.counterCount > 0)
    {
      /* Store counters. */
      for (ruleId = 0; ruleId < policyInfo.entryCount; ruleId++)
      {
        COMPILER_64_ZERO(val1);
        COMPILER_64_ZERO(val2);

        rv = BCM_E_EMPTY;

        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
          PT_LOG_WARN(LOG_CTX_HAPI,"Shuffle counters not supported!");
          rv = BCM_E_UNAVAIL;
        #else
          if (policyInfo.policyStage == BROAD_POLICY_STAGE_EGRESS)
          {
            rv = bcm_field_counter_get(unit, policyInfo.entry[ruleId], 0, &val2);
          }
          else
          {
            rv = bcm_field_counter_get(unit, policyInfo.entry[ruleId], 0, &val1);
            if (rv == BCM_E_NONE)
            {
              rv = bcm_field_counter_get(unit, policyInfo.entry[ruleId], 1, &val2);
            }
          }
        #endif

        if (rv == BCM_E_NONE)
        {
          counterPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*counterPtr));
          if (counterPtr != L7_NULL)
          {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("- SHUFFLE: storing counters for policy %d, rule %d\n", policy, ruleId);

            counterPtr->policy = policy;
            counterPtr->ruleId = ruleId;
            counterPtr->val1   = val1;
            counterPtr->val2   = val2;
            counterPtr->next   = counter_info;
            counter_info       = counterPtr;
          }
        }
      }
    }

    /* Destroy the policy. */
    (void)l7_bcm_policy_destroy(unit, policy);
  }

  return L7_SUCCESS;
}

static int _policy_group_destroy_with_counter_backup(int unit, BROAD_GROUP_t group)
{
  int                     rv = BCM_E_NONE;
  BROAD_POLICY_t          policy;
  policy_map_table_t      policyInfo;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- SHUFFLE: destroying group %d\n", group);

  /* Iterate through all policies and store away any counters, then delete them.
     The group itself will be destroyed as a result of destroying the last policy
     that is in that group. */

  for (policy = 0; policy < BROAD_MAX_POLICIES; policy++)
  {
    rv = _policy_map_get_info(unit, policy, &policyInfo);
    if (rv == BCM_E_NONE)
    {
      if (policyInfo.group == group)
      {
        rv = _policy_policy_destroy_with_counter_backup(unit, policy);
      }
    }
  }

  return BCM_E_NONE;
}

static int _policy_counters_restore(int unit)
{
  int                     rv = BCM_E_NONE;
  shuffle_counter_info_t *counterPtr;
  shuffle_counter_info_t *prevCounterPtr;
  policy_map_table_t      policyInfo;

  /* Restore all counter values that were previously saved. */
  counterPtr = counter_info;

  while (counterPtr != L7_NULL)
  {
    if (_policy_map_get_info(unit, counterPtr->policy, &policyInfo) == BCM_E_NONE)
    {
      if (counterPtr->ruleId < policyInfo.entryCount)
      {
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
          PT_LOG_WARN(LOG_CTX_HAPI,"Shuffle counters not supported!");
          rv = BCM_E_UNAVAIL;
        #else
          if (policyInfo.policyStage == BROAD_POLICY_STAGE_EGRESS)
          {
            rv = bcm_field_counter_set(unit, policyInfo.entry[counterPtr->ruleId], 0, counterPtr->val2);
            if (rv != BCM_E_NONE)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                sysapiPrintf("- bcm_field_counter_set() returned %d\n", rv);
            }
          }
          else
          {
            rv = bcm_field_counter_set(unit, policyInfo.entry[counterPtr->ruleId], 0, counterPtr->val1);
            if (rv != BCM_E_NONE)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                sysapiPrintf("- bcm_field_counter_set() returned %d\n", rv);
            }
    
            rv = bcm_field_counter_set(unit, policyInfo.entry[counterPtr->ruleId], 1, counterPtr->val2);
            if (rv != BCM_E_NONE)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                sysapiPrintf("- bcm_field_counter_set() returned %d\n", rv);
            }
          }
        #endif
      }
      else
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- bad ruleId %d in policy %d, entryCount %d\n", counterPtr->ruleId, counterPtr->policy, policyInfo.entryCount);
      }
    }

    prevCounterPtr = counterPtr;
    counterPtr = counterPtr->next;
    osapiFree(L7_DRIVER_COMPONENT_ID, prevCounterPtr);
  }

  counter_info = L7_NULL;

  return rv;
}

static int _policy_group_rewrite(int unit, BROAD_POLICY_t newPolicyId, BROAD_POLICY_ENTRY_t *policyData)
{
  int                   rv = BCM_E_NONE;
  model_group_table_t  *modelGroupPtr;
  group_table_t         groupInfo;
  BROAD_GROUP_t         group;
  L7_BOOL               destroyGroup;
  super_qset_entry_t    sqsetInfo;
  BROAD_POLICY_t        policy;
  policy_map_table_t    policyMapInfo;
  BROAD_POLICY_ENTRY_t  policyInfo;
  BROAD_POLICY_ENTRY_t *policyInfoPtr;
  sqsetWidth_t          qsetWidth;
  bcm_pbmp_t            pbmp;
  int                   bcmPort;

  /* The rewrite consists of two stages. The first stage is to delete policies and groups
     in order to make room for the rewrite. The second stage is to create groups and 
     rewrite the policies in their new groups (or leave them in their old groups).
     Policies that are moved have their counter values stored and restored after
     they've been rewritten. */

  /* Iterate through each configured group and compare it to the 
     modeled groups. If the sqset or type has changed, or the group doesn't
     exist in the model, then destroy the group. */
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- SHUFFLE: destroying groups that differ from model\n");

  for (group = 0; group < model_group_table_size; group++)
  {
    modelGroupPtr = &model_group_table[group];
    _policy_group_info_get(unit, policyData->policyStage, group, &groupInfo);

    destroyGroup = L7_FALSE;

    if ((modelGroupPtr->flags == GROUP_NONE) && (groupInfo.flags & GROUP_USED))
    {
      destroyGroup = L7_TRUE;
    }
    else if ((modelGroupPtr->flags & GROUP_USED) && (groupInfo.flags & GROUP_USED))
    {
      if ((modelGroupPtr->sqset != groupInfo.sqset) || (modelGroupPtr->type != groupInfo.type))
      {
        destroyGroup = L7_TRUE;
      }
    }

    if (destroyGroup == L7_TRUE)
    {
      /* Destroy this group and all the policies it contains. Be sure that any policy 
         that has counters has the counter values preserved so that they are restored
         when the policy rules are reinstalled. */
      rv = _policy_group_destroy_with_counter_backup(unit, group);
      if (rv != BCM_E_NONE)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("- SHUFFLE: _policy_group_destroy_with_counter_backup returned %d\n", rv);
      }
    }

    if (groupInfo.flags & GROUP_USED)
    {
      /* Skip to the last slice in the group. */
      _policy_sqset_get(unit, groupInfo.sqset, &sqsetInfo);
      group += sqsetInfo.status.slice_width_physical - 1;
    }
  }

  /* Iterate through all the policies and if the modeled group differs from the actual group,
     then destroy the policy (as it is being moved), but preserve the counters to be restored later. */
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- SHUFFLE: destroying policies that have moved in the model\n");

  for (policy = 0; policy < BROAD_MAX_POLICIES; policy++)
  {
    if (_policy_map_get_info(unit, policy, &policyMapInfo) == BCM_E_NONE)
    {
      if (policyMapInfo.policyStage == policyData->policyStage)
      {
        if ((policy != newPolicyId) && (policyMapInfo.group != model_policy_table[policy].group))
        {
          rv = _policy_policy_destroy_with_counter_backup(unit, policy);
          if (rv != BCM_E_NONE)
          {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("- SHUFFLE: _policy_policy_destroy_with_counter_backup returned %d\n", rv);
          }
        }
      }
    }
  }

  /* Run compression API on remaining groups. This will close up any rule gaps and possibly free slices. */
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- SHUFFLE: running compression on remaining groups\n");

  for (group = 0; group < model_group_table_size; group++)
  {
    _policy_group_info_get(unit, policyData->policyStage, group, &groupInfo);
    if (groupInfo.flags & GROUP_USED)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- SHUFFLE: compressing group %d (GID %d)\n", group, groupInfo.gid);

      rv = bcm_field_group_compress(unit, groupInfo.gid);
      if (rv != BCM_E_NONE)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- bcm_field_group_compress() returned %d\n", rv);
      }

      /* Skip to the last slice in the group. */
      _policy_sqset_get(unit, groupInfo.sqset, &sqsetInfo);
      group += sqsetInfo.status.slice_width_physical - 1;
    }
  }

  /* Iterate through all modeled policies, widest to narrowest. If the policy is in the model but
     not in the HW, create the policy. */
  for (qsetWidth = sqsetWidthLast - 1; ((qsetWidth < sqsetWidthLast) && (qsetWidth >= sqsetWidthFirst)); qsetWidth--)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- SHUFFLE: Install policies of width %d\n", qsetWidth);

    for (policy = 0; policy < BROAD_MAX_POLICIES; policy++)
    {
      group = model_policy_table[policy].group;
      if (group != BROAD_GROUP_INVALID)
      {
        if (model_policy_table[policy].minQsetWidth == qsetWidth)
        {
          if ((_policy_map_get_info(unit, policy, &policyMapInfo) == BCM_E_NOT_FOUND) ||
              (policy == newPolicyId))
          {
            /* Restore the policy. */
            if (policy == newPolicyId)
            {
              /* If this is the newly added policy then it's not in the USL DB yet, 
                 so use the policy info passed to us. */
              policyInfoPtr = policyData;
              rv = BCM_E_NONE;
            }
            else
            {
              /* Otherwise obtain the policy info from the USL DB. */
              rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, &policyInfo);
              policyInfoPtr = &policyInfo;
            }
            if (rv == BCM_E_NONE)
            {
              rv = l7_bcm_policy_create(unit, policy, policyInfoPtr, L7_FALSE);
              if (rv == BCM_E_NONE)
              {
                /* Restore port bitmap for policy. */
                if (_policy_map_get_info(unit, policy, &policyMapInfo) == BCM_E_NONE)
                {
                  /* The pbmp for the policy will be the default since it was just
                     created. Figure out which bits in the modeled pbmp are
                     different from the default, and make the appropriate
                     updates. */
                  BCM_PBMP_ASSIGN(pbmp, model_policy_table[policy].pbm);
                  BCM_PBMP_XOR(pbmp, policyMapInfo.pbm);
                  BCM_PBMP_ITER(pbmp, bcmPort)
                  {
                    if (BCM_PBMP_MEMBER(model_policy_table[policy].pbm, bcmPort))
                    {
                      rv = l7_bcm_policy_apply(unit, policy, bcmPort);
                      if (rv != BCM_E_NONE)
                      {
                        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                          sysapiPrintf("- SHUFFLE: l7_bcm_policy_apply() returned %d for policy %d, bcmPort %d\n", rv, policy, bcmPort);
                        break;
                      }
                    }
                    else
                    {
                      rv = l7_bcm_policy_remove(unit, policy, bcmPort);
                      if (rv != BCM_E_NONE)
                      {
                        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                          sysapiPrintf("- SHUFFLE: l7_bcm_policy_remove() returned %d for policy %d, bcmPort %d\n", rv, policy, bcmPort);
                        break;
                      }
                    }
                  }
                }
              }
              else
              {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                  sysapiPrintf("- SHUFFLE: l7_bcm_policy_create() returned %d for policy %d\n", rv, policy);
              }

              if (policy != newPolicyId)
              {
                /* Free any rules allocated by usl_db_policy_info_get(). */
                hapiBroadPolicyRulesPurge(&policyInfo);
              }
            }
          }
        }
      }
    }
  }

  /* Restore counter values that were backed up. */
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- SHUFFLE: Restoring counters\n");

  rv = _policy_counters_restore(unit);

  return rv;
}

int policy_group_shuffle(int unit, BROAD_POLICY_t newPolicyId, BROAD_POLICY_ENTRY_t *policyData)
{
  int       rv = BCM_E_NONE;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- shuffle policies\n");

  /* Check if the policy will fit if we rewrite all policies, widest to narrowest. */
  rv = _policy_group_rewrite_check(unit, policyData);
  if (rv == BCM_E_NONE)
  {
    rv = _policy_group_rewrite(unit, newPolicyId, policyData);
  }

  if (rv == BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- shuffle successful\n");
  }
  else
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- can't rewrite policies to fit new policy\n");
  }

  return rv;
}

