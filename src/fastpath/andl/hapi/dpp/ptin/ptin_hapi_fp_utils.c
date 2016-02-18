#include "ptin_hapi_fp_utils.h"
#include "broad_group_dnx.h"
#include "logger.h"

#include "bcm/field.h"

/*********************************
 * MACROS FOR MANAGING DATABASE
 *********************************/

/* Check if database index is valid */
#define FP_POLICY_VALID_INDEX(index,db)         ( (index)>=0 && (index)<(db)->database_num_elems )
/* Get database element index from a pointer */
#define FP_POLICY_GET_INDEX_FROM_PTR(ptr,db)    ( ((L7_uint32) ((char *) (ptr)) - (L7_uint32) ((char *) (db)->database_base)) / (L7_uint32) (db)->database_elem_sizeof )
/* Get database pointer from an element index */
#define FP_POLICY_GET_PTR_FROM_INDEX(index,db)  ( (void *) ((char *) (db)->database_base + (index)*(db)->database_elem_sizeof) )
/* Get database base pointer (first element) */
#define FP_POLICY_GET_DATABASE_PTR(db)          ( (void *) (db)->database_base )
/* Get the following element pointer in database */
#define FP_POLICY_GET_PTR_NEXT(ptr,db)          ( (void *) ((char *) (ptr) + (db)->database_elem_sizeof) )
/* Increment database element pointer */
#define FP_POLICY_INC_PTR(ptr,db)               ( ((void *) ((char *) (ptr))) += (db)->database_elem_sizeof)
/* Get the first free element in database */
#define FP_FREEPOLICY_GET_INDEX(db)             ( (db)->database_index_first_free )
/* Check if database has no free elements */
#define FP_FREEPOLICY_HAS_NOVALUE(db)           ( (db)->database_index_first_free<0 )
/* Set free element index in database with a new value */
#define FP_FREEPOLICY_SET_INDEX(index,db)       { (db)->database_index_first_free = (index); }
/* Set no free elements in database */
#define FP_FREEPOLICY_SET_NOVALUE(db)           { (db)->database_index_first_free = -1; }
/* Varify first free element index, and if not valid, correct it to the first element */
#define FP_FREEPOLICY_CORRECT_INDEX(db)         { if (!FP_POLICY_VALID_INDEX((db)->database_index_first_free,db))  (db)->database_index_first_free=0; }
/* Verify if given database index is previous than the first free element, and if so, update the first free element */
#define FP_FREEPOLICY_UPDATE_INDEX(index,db)    { if ((index)<(db)->database_index_first_free)  (db)->database_index_first_free=(index); }


/**
 * Consult Switch available resources
 * 
 * @param resources : struct describing system resources
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_policy_resources_get(st_ptin_policy_resources *resources)
{
  uint8 stage_idx, group_idx;
  bcm_field_group_status_t group_status;
  bcm_error_t error;
  group_table_t *gtable;

  if (resources==L7_NULLPTR)
    return L7_FAILURE;

  memset(resources,0x00,sizeof(st_ptin_policy_resources));

  /* Stage */
  for (stage_idx=0; stage_idx<PTIN_POLICY_MAX_STAGES; stage_idx++)
  {
    /* Groups */
    for (group_idx=0; group_idx<PTIN_POLICY_MAX_GROUPS; group_idx++)
    {
      resources->cap[group_idx][stage_idx].inUse = L7_FALSE;

      /* Get group information */
      gtable = policy_group_count_rules(0,group_idx,stage_idx);
      if (gtable==L7_NULLPTR)
      {
        memset(&resources->cap[group_idx][stage_idx],0x00,sizeof(st_ptin_ffp_rules));
        continue;
      }
      /* Is entry in use */
      if (!(gtable->flags & GROUP_USED))
      {
        PT_LOG_TRACE(LOG_CTX_HAPI,"Group index %u (stage=%d) is not in use.",group_idx,stage_idx);
        memset(&resources->cap[group_idx][stage_idx],0x00,sizeof(st_ptin_ffp_rules));
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_HAPI,"Analysing group index %u / stage=%d: gid=%d (flags=0x%08x)",group_idx,stage_idx,gtable->gid,gtable->flags);

      /* Get group details */
      error = bcm_field_group_status_get(0, gtable->gid, &group_status);
      if (error==BCM_E_NOT_FOUND)
      {
        PT_LOG_TRACE(LOG_CTX_HAPI,"Group %u (stage=%d) does not exist",gtable->gid,stage_idx);
        memset(&resources->cap[group_idx][stage_idx],0x00,sizeof(st_ptin_ffp_rules));
        continue;
      }
      else if (error!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error reading status of group %u, stage=%d (error=%d \"%s\")",gtable->gid,stage_idx,error,bcm_errmsg(error));
        return L7_FAILURE;
      }

      resources->cap[group_idx][stage_idx].inUse = L7_TRUE;
      resources->cap[group_idx][stage_idx].group_id = gtable->gid;

      resources->cap[group_idx][stage_idx].total.rules        = group_status.entries_total;
      resources->cap[group_idx][stage_idx].total.counters     = group_status.counters_total;
      resources->cap[group_idx][stage_idx].total.meters       = group_status.meters_total;
      resources->cap[group_idx][stage_idx].total.slice_width  = group_status.slice_width_physical;

      resources->cap[group_idx][stage_idx].free.rules         = group_status.entries_free;
      resources->cap[group_idx][stage_idx].free.counters      = group_status.counters_free;
      resources->cap[group_idx][stage_idx].free.meters        = group_status.meters_free;
      resources->cap[group_idx][stage_idx].free.slice_width   = group_status.slice_width_physical;

      resources->cap[group_idx][stage_idx].count.rules        = gtable->count_rules;
      resources->cap[group_idx][stage_idx].count.counters     = 0;
      resources->cap[group_idx][stage_idx].count.meters       = 0;
      resources->cap[group_idx][stage_idx].count.slice_width  = group_status.slice_width_physical;
    }
  }

  #if 0
  resources->ffp_available_rules[FPGROUP_START-1] = rules_available;

  resources->vlanXlate_available_entries.ing_stag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_ingress_stag);
  resources->vlanXlate_available_entries.ing_dtag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_ingress_dtag);
  resources->vlanXlate_available_entries.egr_stag = hapi_ptin_get_available_vlanXlate_entries(ptin_vlan_xlate_egress_stag);

  resources->vlans_available.igmp = hapiBroadPtinIgmp_numberOfAvailableVlans();
  resources->vlans_available.dhcp = hapiBroadPtinDhcp_numberOfAvailableVlans();
  resources->vlans_available.bcastLim = hapiBroadPtinBCastLim_numberOfAvailableVlans();

  resources->flowCounters_available_entries = hapiBroadPtinFlowCounters_numberOfAvailableEntries();

//printf("%s(%d) vlanXlate_ing_stag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_ing_stag_available_entries);
//printf("%s(%d) vlanXlate_ing_dtag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_ing_dtag_available_entries);
//printf("%s(%d) vlanXlate_egr_stag_available_entries=%u\r\n",__FILE__,__LINE__,resources->vlanXlate_egr_stag_available_entries);
  #endif

  return L7_SUCCESS;
}


/**********************************
 * FUNCTIONS FOR MANAGING DATABASE
 **********************************/

/**
 * Clear element in database (it will be marked as free)
 * 
 * @param elem : Pointer to element in database
 * @param db : Database descriptor
 */
void ptin_hapi_policy_clear(void *elem, ptin_hapi_database_t *db)
{
  L7_int index;

  /* Validate pointer address */
  if ( !FP_POLICY_VALID_PTR(elem,db) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid policy");
    return;
  }

  /* Calculate index */
  index = FP_POLICY_GET_INDEX_FROM_PTR(elem,db);

  /* We make the assumption that the FP entry was previously removed... this only updates the database */
  db->policy_clear_data(elem);

  /* If index to be freed is before the first_free_index value, update it */
  FP_FREEPOLICY_UPDATE_INDEX(index,db);

  PT_LOG_TRACE(LOG_CTX_HAPI,"Policy cleared");
}


/**
 * Find next used element in database
 * 
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
void *ptin_hapi_policy_next(void *base_ptr, ptin_hapi_database_t *db)
{
  void *ptr;

  /* Run all elements */
  for (ptr = ( base_ptr==L7_NULLPTR ) ? FP_POLICY_GET_DATABASE_PTR(db) : FP_POLICY_GET_PTR_NEXT(base_ptr,db);
       FP_POLICY_VALID_PTR(ptr,db) && !db->policy_inUse(ptr);
       FP_POLICY_INC_PTR(ptr,db) );

  /* Check if all elements were checked without success: not found situation */
  if (!FP_POLICY_VALID_PTR(ptr,db))
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Cell not found");
    return L7_NULLPTR;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Cell found!");

  /* At this point a match has ocurred (index points to that match) */
  return ptr;
}


/**
 * Find element in database with profile data
 * 
 * @param profile : Reference data to search for
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
void *ptin_hapi_policy_find(DAPI_USP_t *usp, void *profile, void *base_ptr, ptin_hapi_database_t *db)
{
  void *ptr;

  /* Run all elements */
  for (ptr = ( base_ptr==L7_NULLPTR ) ? FP_POLICY_GET_DATABASE_PTR(db) : FP_POLICY_GET_PTR_NEXT(base_ptr,db);
       FP_POLICY_VALID_PTR(ptr,db);
       FP_POLICY_INC_PTR(ptr,db) /*ptr = GET_POLICY_PTR_NEXT(ptr)*/ )
  {
    /* Is some comparison parameter differ, skip to the next element */
    if (db->policy_compare(usp,profile,ptr))  break;
  }

  /* Check if all elements were checked without success: not found situation */
  if (!FP_POLICY_VALID_PTR(ptr,db))
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Cell not found");
    return L7_NULLPTR;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Cell found!");

  /* At this point a match has ocurred (index points to that match) */
  return ptr;
}

/**
 * Find element in database with conflict
 * 
 * @param profile : Reference data to search for
 * @param base_ptr : Pointer to first element to look for 
 *                 (L7_NULLPTR to start looking from the
 *                 beginning).
 * @param db : Database descriptor 
 * @param state: ingress or egress 
 * 
 * @return void* : Pointer to the found element (L7_NULLPTR if 
 *         not found or error)
 */
void *ptin_hapi_policy_check_conflicts(DAPI_USP_t *usp, void *profile, void *base_ptr, ptin_hapi_database_t *db, int stage)
{
  void *ptr;

  /* Run all elements */
  for (ptr = ( base_ptr==L7_NULLPTR ) ? FP_POLICY_GET_DATABASE_PTR(db) : FP_POLICY_GET_PTR_NEXT(base_ptr,db);
       FP_POLICY_VALID_PTR(ptr,db);
       FP_POLICY_INC_PTR(ptr,db) /*ptr = GET_POLICY_PTR_NEXT(ptr)*/ )
  {
    /* Is some comparison parameter differ, skip to the next element */
    if (db->policy_check_conflicts(usp, profile, ptr, stage))  break;
  }

  /* Check if all elements were checked without success: not found situation */
  if (!FP_POLICY_VALID_PTR(ptr,db))
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Cell not found");
    return L7_NULLPTR;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Cell found in conflict!");

  /* At this point a match has ocurred (index points to that match) */
  return ptr;
}

/**
 * Find first free element in database
 * 
 * @param db : database descriptor
 * 
 * @return void* : Pointer to first free element (L7_NULLPTR if 
 *         no free elements are available)
 */
void *ptin_hapi_policy_find_free(ptin_hapi_database_t *db)
{
  L7_int index;

  /* No free elements situation */
  if ( FP_FREEPOLICY_HAS_NOVALUE(db) )
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No free elements");
    FP_FREEPOLICY_SET_NOVALUE(db);
    return L7_NULLPTR;
  }

  /* Correct first free element */
  FP_FREEPOLICY_CORRECT_INDEX(db);

  /* Find the first free element */
  for (index=FP_FREEPOLICY_GET_INDEX(db);
       FP_POLICY_VALID_INDEX(index,db) && db->policy_inUse(FP_POLICY_GET_PTR_FROM_INDEX(index,db));
       index++);

  /* Not found situation */
  if ( !FP_POLICY_VALID_INDEX(index,db) )
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No free elements");
    FP_FREEPOLICY_SET_NOVALUE(db);
    return L7_NULLPTR;
  }

  /* Found */
  /* Free Index to be used next time */
  FP_FREEPOLICY_SET_INDEX(index,db);

  PT_LOG_TRACE(LOG_CTX_HAPI,"Found free element");

  return ((void *) FP_POLICY_GET_PTR_FROM_INDEX(index,db));
}

