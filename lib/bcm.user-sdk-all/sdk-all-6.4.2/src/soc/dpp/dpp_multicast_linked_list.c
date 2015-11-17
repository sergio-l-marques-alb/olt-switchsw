/* $Id: dpp_multicast_ingress.c,v  $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST

#include <soc/dpp/multicast_imp.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>


#include <soc/mcm/memregs.h>

#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/mbcm.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * Initialization of the Arad blocks configured in this module.
 * Called as part of the initialization sequence.
 */
uint32 dpp_mc_init(
    SOC_SAND_IN  int                 unit
)
{
#ifdef _ARAD_MC_TEST_UNNEEDED_FABRIC_CODE_0
  uint32 reg_idx, fld_idx;
  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS links;
#endif
  uint32 mc_tc, idx;
  SOC_SAND_U32_RANGE fmc_q_range;
  SOCDNX_INIT_FUNC_DEFS;

  /*
   *  Traffic Class to multicast traffic class mapping - set MULT-TC = TC/2
   */
  for (idx = 0; idx < ARAD_NOF_TRAFFIC_CLASSES; idx ++)
  {
    mc_tc = idx/2;
    SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_fabric_traffic_class_to_multicast_cls_map_set, (unit, idx, mc_tc)));
  }

#ifdef _ARAD_MC_TEST_UNNEEDED_FABRIC_CODE_0
  /*
   *  Enable auto-detect of active fabric links, with no mask.
   *  For this purpose - set all links as "active", and then enable auto-refresh
   */
  for (idx = 0; idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); idx++)
  {
    reg_idx = ARAD_REG_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = ARAD_FLD_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    SOC_SAND_SET_BIT(links.bitmap[reg_idx], 0x1, fld_idx);
  }

  SOCDNX_SAND_IF_ERR_EXIT(arad_mult_fabric_active_links_set_unsafe(unit, &links, TRUE));
#endif

#ifdef _ARAD_MC_TEST_DOUBLE_INITIALIZATION_0
  {
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
      eg_vlan_rep;
    eg_vlan_rep.mc_id_low  = 0;
    eg_vlan_rep.mc_id_high = SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX - 1;
    SOCDNX_IF_ERR_EXIT(dpp_mult_eg_bitmap_group_range_set(unit, &eg_vlan_rep));
  }
#endif
  
    soc_sand_SAND_U32_RANGE_clear(&fmc_q_range);
    fmc_q_range.start = 0;
    fmc_q_range.end = 3;
    SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_set,(unit, &fmc_q_range)));

exit:
  SOCDNX_FUNC_RETURN;
}



/*
 * This function checks if the multicast group is open (possibly empty).
 * returns TRUE if the group is open (start of a group), otherwise FALSE.
 */
uint8 dpp_mult_does_group_exist(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  mcid,       /* MC ID of the group */
    SOC_SAND_IN  int     is_egress   /* is the MC group an egress group */
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
  if (mcid >= nof_mc_ids) {
    LOG_ERROR(BSL_LS_SOC_MULTICAST, (BSL_META_U(unit, "multicast ID %u is not under the number of multicast IDs: %u\n"), mcid, nof_mc_ids));
    return FALSE;
  }
  if (!is_egress) { /* ingress group */
    if (DPP_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) {
      return FALSE;
    }
    DPP_MC_ASSERT(mcid == DPP_MCDS_GET_PREV_ENTRY(mcds, mcid)); /* verify that mcid is a start of a linked list */
    return TRUE;
  } else { /* egress group */
    return dpp_egress_group_open_get(unit, mcid);
  }
}


/*
 * This function checks if the multicast group is open (possibly empty).
 * returns TRUE if the group is open (start of a group), otherwise FALSE.
 */
uint32 dpp_mult_does_group_exist_ext( 
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_MULT_ID mcid,      /* MC ID of the group */
    SOC_SAND_IN  int             is_egress, /* is the MC group an egress group */
    SOC_SAND_OUT uint8           *is_open   /* returns FALSE if not open */
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
  SOCDNX_INIT_FUNC_DEFS;

  if (mcid >= nof_mc_ids) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("multicast ID %u is not under the number of multicast IDs: %u"), mcid, nof_mc_ids));
  }
  if (!is_egress) { /* ingress group */
    if (DPP_MCDS_GET_TYPE(mcds, mcid) != DPP_MCDS_TYPE_VALUE_INGRESS_START) {
      *is_open = FALSE;
    } else {
      DPP_MC_ASSERT(mcid == DPP_MCDS_GET_PREV_ENTRY(mcds, mcid)); /* verify that mcid is a start of a linked list */
      *is_open = TRUE;
    }
  } else { /* egress group */
    *is_open = dpp_egress_group_open_get(unit, mcid);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Relocate the given used entries, not disturbing multicast traffic to
 * the group containing the entries.
 * SWDB and hardware are updated accordingly.
 * If relocation_block_size is 0, then the function calculates
 * relocation_block_start and relocation_block_size by itself to suit mcdb_index.
 * After successful relocation, the block that was relocated is freed, except for entry mcdb_index.
 *
 * This function may overwrite the mc group replications stored in the mcds.
 */

uint32
  dpp_mcdb_relocate_entries(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        mcdb_index,             /* table index needed for the start of a group */
    SOC_SAND_IN  uint32                        relocation_block_start, /* the start of the relocation block */
    SOC_SAND_IN  dpp_free_entries_block_size_t relocation_block_size,  /* the size of the relocation block */
    SOC_SAND_OUT SOC_TMC_ERROR                 *out_err                /* return possible errors that the caller may want to ignore: insufficient memory */
)
{
  uint32 start = relocation_block_start, found_block_start, prev_entry;
  dpp_free_entries_block_size_t size = relocation_block_size, found_block_size;
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 entry_type = DPP_MCDS_GET_TYPE(mcds, mcdb_index);
  dpp_free_entries_blocks_region_t *region = 0;
  int free_alloced_block = 0;
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(out_err);

  if (!size) { /* need to calculate the relocated entries */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_get_relocation_block(mcds, mcdb_index, &start, &size));
    if (!size) {
      DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_GET_TYPE(mcds, mcdb_index)));
      SOC_EXIT; /* a relocation is not needed */
    }
  }

  /* entry must be used, and if a block (size>1) is relocated, this must be non tdm egress */
  DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(entry_type) && (size == 1 || entry_type == DPP_MCDS_TYPE_VALUE_EGRESS));

  SOCDNX_IF_ERR_EXIT( /* get free entries */
    dpp_mcds_get_free_entries_block(mcds, DPP_MCDS_GET_FREE_BLOCKS_PREFER_SIZE | DPP_MCDS_GET_FREE_BLOCKS_DONT_FAIL,
      size, size, &found_block_start, &found_block_size));
  if (!found_block_size) {
    *out_err = _SHR_E_FULL;
    SOC_EXIT;
  }
  DPP_MC_ASSERT(found_block_size <= size);
  prev_entry = DPP_MCDS_GET_PREV_ENTRY(mcds, start);

  free_alloced_block = 1;
  if (found_block_size == size) { /* can relocate all the block to the allocated block */
    dpp_free_entries_block_size_t s;
    uint32 after_block; /* the index if the entry after the block (pointed to by the last block entry) */

    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, start + size - 1, entry_type, &after_block));

    for (s = 0; s < size; ++s) {
      SOCDNX_IF_ERR_EXIT( /* copy entry to new location and write to hardware */
        dpp_mcdb_copy_write(unit, start + s, found_block_start + s));
      DPP_MCDS_SET_PREV_ENTRY(mcds, found_block_start + s, s ? found_block_start + (s-1) : prev_entry);
    }
    SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( /* point to the new block instead of to the old one */
      mcds, unit, prev_entry, entry_type, found_block_start));
    free_alloced_block = 0;
    if (after_block != DPP_MC_EGRESS_LINK_PTR_END && after_block != MCDS_INGRESS_LINK_END(mcds)) { /* link the entry after the relocated block back to the new block */
      DPP_MCDS_SET_PREV_ENTRY(mcds, after_block, found_block_start + size - 1);
    }
  } else { /* can not relocate as one block, is non-tdm egress */
    uint32 list_next; /* the next entry in the list after the block to be relocated */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_copy_replications_from_egress_block( /* set mcdb with the block's entries */
      unit, 1, start, size, &list_next));
    free_alloced_block = 0;

    SOCDNX_IF_ERR_EXIT(dpp_mcds_set_egress_linked_list( /* replace block by a linked list */
      unit, FALSE, prev_entry, list_next, found_block_start, found_block_size, &found_block_start, out_err));
    if (*out_err) {
      SOC_EXIT;
    }
  }

  /* after reallocating, need to free the old block except for the entry which will start the linked list of the new group */
  if (mcdb_index > start) { /* free entries in relocated block before the entry to be used as a start of a MC group */
    if (!region) {
      region = dpp_mcds_get_region(mcds, mcdb_index);
    }
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, start, mcdb_index - 1, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
  }
  if (mcdb_index + 1 < start + size) { /* free entries in relocated block before the entry to be used as a start of a MC group */
    if (!region) {
      region = dpp_mcds_get_region(mcds, mcdb_index);
    }
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, mcdb_index + 1, start + (size - 1), region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
  }
  *out_err = _SHR_E_NONE;

exit:
  if (free_alloced_block) {
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, 
      found_block_start, found_block_start + (found_block_size - 1), dpp_mcds_get_region(mcds, found_block_start), McdsFreeBuildBlocksAdd_AllMustBeUsed));
  }
  SOCDNX_FUNC_RETURN;
}


/*
 * Free a linked list that is not used any more, updating mcds and hardware.
 * Stop at the given entry, and do not free it.
 * The given linked list must not include the first entry of the group.
 * All entries in the linked list should be of the given type.
 */

uint32
  dpp_mcdb_free_linked_list_till_my_end(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    first_index,  /* table index of the first entry in the linked list to free */
    SOC_SAND_IN  uint32    entries_type, /* the type of the entries in the list */
    SOC_SAND_IN  uint32    end_index     /* the index of the end of the list to be freed */
)
{
  uint32 cur_index;
  uint32 block_start = 0, block_end = 0; /* will mark a block of consecutive entries in the linked list to free together */
  uint32 range_start = 0, range_end = 0; /* will mark the consecutive range of the region in which the block can grow */
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  dpp_free_entries_blocks_region_t *region = 0;
  SOCDNX_INIT_FUNC_DEFS;

  for (cur_index = first_index; cur_index != end_index; ) { /* loop over the linked list */
    DPP_MC_ASSERT(cur_index > 0 && cur_index < MCDS_INGRESS_LINK_END(mcds));

    if (block_end) { /* we are in a block, try enlarging it with the entry */
      if (cur_index == block_end + 1 && cur_index <= range_end) { /* Can this entry join the block at its end? */
        block_end = cur_index;
      } else if (cur_index + 1 == block_start && cur_index >= range_start) { /* Can this entry join the block at its start? */
        block_start = cur_index;
      } else { /* can't enlarge block, add block to free list and start a new one */
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
        region = dpp_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); /* get range in which we can accumulate a block */
        block_start = block_end = cur_index;
      }
    } else { /* we are at at the first entry */
      region = dpp_mcds_get_region_and_consec_range(mcds, cur_index, &range_start, &range_end); /* get range in which we can accumulate a block */
      block_start = block_end = cur_index;
    }
    SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds, unit, cur_index, entries_type, &cur_index));
  }
  if (block_end) { /* write the last discovered consecutive */
    SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks(unit, mcds, block_start, block_end, region, McdsFreeBuildBlocksAdd_AllMustBeUsed));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * Free a linked list that is not used any more, updating mcds and hardware.
 * The given linked list must not include the first entry of the group.
 * All entries in the linked list should be of the given type.
 */

uint32
  dpp_mcdb_free_linked_list(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  first_index, /* table index of the first entry in the linked list to free */
    SOC_SAND_IN uint32  entries_type /* the type of the entries in the list */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  return dpp_mcdb_free_linked_list_till_my_end(unit, first_index, entries_type, 
    (DPP_MCDS_TYPE_IS_INGRESS(entries_type) ? MCDS_INGRESS_LINK_END(mcds) : DPP_MC_EGRESS_LINK_PTR_END));
}


/*
 * If the given egress linked list entry contains the given replication,
 * then remove the replication from the linked list. If it is the only
 * replication in the linked list, the entry is removed from the group.
 * Does not support the TDM format.
 */
uint32 dpp_mcds_remove_replications_from_egress_group(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  entry_index, /* table index of the first entry in the group */
    SOC_SAND_IN uint32  dest,        /* The destination of the replication to be removed */
    SOC_SAND_IN uint32  cud          /* The CUD of the replication to be removed  group */
)
{
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    arad_mcdb_entry_t *mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, entry_index);
    uint32 format = 0, next_index = entry_index, cur_index = 0, prev_index = entry_index, bitmap = -1;
    uint32 freed_index = entry_index; /* entry to be freed, the entry_index value means: can not free the entry */
    int is_outlif_only_replication = 0, is_outlif_cud_replication = 0, is_bitmap_cud_replication = 0;
    int remove_entry = 0, found = 0;
    uint16 entries_left = DPP_MULT_MAX_EGRESS_REPLICATIONS; /* If we loop over more iterations this this it is certainly an error */
    SOCDNX_INIT_FUNC_DEFS;
    DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS_START == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); /* type should exactly match entry */

    /* process the destination to search for and delete */
    if (dest == _SHR_GPORT_INVALID) { /*outlif only, or port+outlif replication */
        is_outlif_only_replication = 1;
        if (cud == DPP_MC_EGR_OUTLIF_DISABLED) { /* Invalid value, used to mark no replication in hardware */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    } else if (dest & ARAD_MC_EGR_IS_BITMAP_BIT) { /* bitmap + outlif replication */
        bitmap = dest & ~ARAD_MC_EGR_IS_BITMAP_BIT;
        is_bitmap_cud_replication = 1;
        if (bitmap == DPP_MC_EGR_OUTLIF_DISABLED) { /* Invalid value, used to mark no replication in hardware */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    } else {
        is_outlif_cud_replication = 1;
        if (dest == DPP_MULT_EGRESS_PORT_INVALID) { /* Invalid value, used to mark no replication in hardware */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid replication to delete")));
        }
    }

    for (; entries_left; --entries_left) {
        cur_index = next_index;

        switch (format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, ENTRY_FORMATf)) {
          /* select memory format based on the format type */
          case 0:
            next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, LINK_PTRf);
            if (is_outlif_cud_replication && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, OUTLIF_1f) == cud) {
                uint32 port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Af);
                if (dest == port) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Af, DPP_MULT_EGRESS_PORT_INVALID); /* remove replication */
                    if (soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf) == ARAD_MULT_EGRESS_SMALL_PORT_INVALID) {
                        remove_entry = 1;
                    }
                } else if (dest < ARAD_MULT_EGRESS_SMALL_PORT_INVALID && dest == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf)) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_entry, PP_DSP_1Bf, ARAD_MULT_EGRESS_SMALL_PORT_INVALID); /* remove replication */
                    if (port == DPP_MULT_EGRESS_PORT_INVALID) {
                        remove_entry = 1;
                    }
                }
            }
            break;

          case 1:
            next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, LINK_PTRf);
            if (is_bitmap_cud_replication && bitmap == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, BMP_PTRf) &&
                cud == soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, OUTLIF_1f)) {
                soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_entry, BMP_PTRf, DPP_MC_EGR_BITMAP_DISABLED); /* remove replication */
                found = remove_entry = 1;
            }
            break;

            case 2:
            case 3:
              next_index = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, LINK_PTRf);
              if (is_outlif_only_replication) {
                  uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f);
                  uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_2f);
                  if (outlif1 == cud) {
                      found = 1;
                      soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); /* remove replication */
                      if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                          remove_entry = 1;
                      }
                  } else if (outlif2 == cud) {
                      found = 1;
                      soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); /* remove replication */
                      if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                          remove_entry = 1;
                      }
                  }
              }
              break;

          case 4:
          case 5:
            if (is_outlif_cud_replication) {
                uint32 port1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f);
                uint32 port2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f);
                if (port1 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_1f) == cud) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_1f, DPP_MULT_EGRESS_PORT_INVALID); /* remove replication */
                    if (port2 == DPP_MULT_EGRESS_PORT_INVALID) { /* replication disabled for this entry part */
                        remove_entry = 1;
                    }
                } else if (port2 == dest && soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, OUTLIF_2f) == cud) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_entry, PP_DSP_2f, DPP_MULT_EGRESS_PORT_INVALID); /* remove replication */
                    if (port1 == DPP_MULT_EGRESS_PORT_INVALID) { /* replication disabled for this entry part */
                        remove_entry = 1;
                    }
                }
            }
            break;

          default: /* formats 6,7 */
            if (is_outlif_only_replication) {
                uint32 outlif1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f);
                uint32 outlif2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f);
                uint32 outlif3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f);
                if (outlif1 == cud) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_1f, DPP_MC_EGR_OUTLIF_DISABLED); /* remove replication */
                    if (outlif2 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { /* This is the only replication in the entry */
                        remove_entry = 1;
                    }
                } else if (outlif2 == cud) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_2f, DPP_MC_EGR_OUTLIF_DISABLED); /* remove replication */
                    if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif3 == DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                        remove_entry = 1;
                    }
                } else if (outlif3 == cud) {
                    found = 1;
                    soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_entry, OUTLIF_3f, DPP_MC_EGR_OUTLIF_DISABLED); /* remove replication */
                    if (outlif1 == DPP_MC_EGR_OUTLIF_DISABLED && outlif2 == DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                        remove_entry = 1;
                    }
                }
            }
        }

        /* get the next entry */
        if (format >= 4) {
            if (format & 1) {
                next_index = cur_index + 1;
                DPP_MC_ASSERT(next_index < MCDS_INGRESS_LINK_END(mcds));
            } else {
                next_index = DPP_MC_EGRESS_LINK_PTR_END;
            }
        }
        if (found) {
            break;
        }

        if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { /* The replication was not found till the end of the group */
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("replication not found in group")));
        }
        prev_index = cur_index;
        mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
        DPP_MC_ASSERT(DPP_MCDS_TYPE_VALUE_EGRESS == DPP_MCDS_ENTRY_GET_TYPE(mcdb_entry)); /* type should exactly match entry */
    }

    if (!found) {
        DPP_MC_ASSERT(0);
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("The replication was not found till the legal size of a group")));
    }

    if (remove_entry) { /* If we want to remove the entry since no replication was left in it */
        int try_to_copy_the_next_entry = 0;
        if (cur_index == entry_index) { /* the first entry in the group needs to be removed */
            DPP_MC_ASSERT(prev_index == entry_index);
            if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { /* this is the only entry in the group, just update it */
                mcdb_entry->word0 = mcds->free_value[0];
                mcdb_entry->word1 &= ~mcds->msb_word_mask;
                mcdb_entry->word1 |= mcds->free_value[1];
            } else  { /* The removed entry points to a second entry */
                try_to_copy_the_next_entry = 1;
            }
        } else { /* the replication entry is not the start of the group, it will be removed */
            arad_mcdb_entry_t *prev_entry = MCDS_GET_MCDB_ENTRY(mcds, prev_index);
            uint32 prev_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, prev_entry, ENTRY_FORMATf);
            DPP_MC_ASSERT(prev_index != cur_index);
            if (prev_format < 4) {
                /* We will remove the entry by having the previous entry point to the next entry */
                SOCDNX_IF_ERR_EXIT(MCDS_SET_NEXT_POINTER( /* set next pointer and write to hardware the first group entry */
                  mcds, unit, prev_index, DPP_MCDS_TYPE_VALUE_EGRESS, next_index));
                freed_index = cur_index;
            } else if (next_index == DPP_MC_EGRESS_LINK_PTR_END) { /* We can remove the last entry even if there is no pointer to it */
                DPP_MC_ASSERT(prev_format & 1);
                soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, prev_entry, ENTRY_FORMATf, prev_format & ~1); /* mark previous entry as end of group */
                SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, prev_index));
                freed_index = cur_index;
            } else {
                try_to_copy_the_next_entry = 1;
            }
        }

        if (try_to_copy_the_next_entry) { /* If the next entry is the end of the group or has a pointer, we can copy it on top of this entry */
            arad_mcdb_entry_t *next_entry = MCDS_GET_MCDB_ENTRY(mcds, next_index);
            uint32 next_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, next_entry, ENTRY_FORMATf);
            DPP_MC_ASSERT(DPP_MCDS_ENTRY_GET_TYPE(next_entry) == DPP_MCDS_TYPE_VALUE_EGRESS);
            if (next_format < 4 || !(next_format & 1)) { /* If the next entry has a pointer or is the last entry in the group */
                mcdb_entry->word0 = next_entry->word0;
                mcdb_entry->word1 &= ~mcds->msb_word_mask;
                mcdb_entry->word1 |= (next_entry->word1 & mcds->msb_word_mask);
                freed_index = next_index;
                SOCDNX_IF_ERR_EXIT(MCDS_GET_NEXT_POINTER(mcds,unit, next_index, DPP_MCDS_TYPE_VALUE_EGRESS, &next_index)); /* get entry to update the back_pointer in */
            }
        }
    }

    if (freed_index != cur_index || !remove_entry) { /* if not freeing the current entry, it needs to be updated in hardware */
        SOCDNX_IF_ERR_EXIT(dpp_mcds_write_entry(unit, cur_index)); /* write group start entry to hardware */
    }

    if (freed_index != entry_index) { /* if an entry was removed, free it */
        if (next_index != DPP_MC_EGRESS_LINK_PTR_END) {
            DPP_MC_ASSERT(DPP_MCDS_GET_TYPE(mcds, next_index) == DPP_MCDS_TYPE_VALUE_EGRESS);
            DPP_MCDS_SET_PREV_ENTRY(mcds, next_index, prev_index); /* make entry next_index point back to prev_index */
        }
        SOCDNX_IF_ERR_EXIT(dpp_mcds_build_free_blocks( /* free the entry freed_index */
          unit, mcds, freed_index, freed_index, dpp_mcds_get_region(mcds, freed_index), McdsFreeBuildBlocksAdd_AllMustBeUsed));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


/*
 * Return the contents of a multicast linked list group of a given type.
 * The contents is returned in the mcds buffer.
 * if the group size is bigger than the specified max_size, only max_size entries are returned,
 * it will not be an error, and the actual size of the group is returned.
 * The group must be open.
 */

uint32
  dpp_mcds_get_group(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   group_id,   /* the mcid of the group */
    SOC_SAND_IN  uint32   group_type, /* the type of the group (of the entries in the list) */
    SOC_SAND_IN  uint16   max_size,   /* the maximum number of members to return from the group */
    SOC_SAND_OUT uint16   *group_size /* the returned actual group size */
)
{
  dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  uint32 entry_type = DPP_MCDS_TYPE_GET_NONE_START(group_type); /* type for the non first entry */
  const uint16 max_group_size = DPP_MCDS_TYPE_IS_INGRESS(group_type) ? DPP_MULT_MAX_INGRESS_REPLICATIONS : DPP_MULT_MAX_EGRESS_REPLICATIONS;
  uint32 end_index = DPP_MCDS_TYPE_IS_INGRESS(group_type) ? MCDS_INGRESS_LINK_END(mcds) : DPP_MC_EGRESS_LINK_PTR_END;
  uint32 cur_index = group_id;
  uint16 entries_remaining = max_size > max_group_size ? max_group_size : max_size; /* max replications to return */

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(group_size);
  DPP_MC_ASSERT(DPP_MCDS_TYPE_IS_USED(group_type));
  if (DPP_MCDS_TYPE_IS_EGRESS(group_type)) { /* calculate index of first group entry */
    cur_index += ARAD_MULT_NOF_MULTICAST_GROUPS; 
  }
  *group_size = 0;

  dpp_mcds_clear_replications(mcds, group_type); /* reset the returned replications to none */
  /* get replications from the first entry */
  SOCDNX_IF_ERR_EXIT(mcds->get_replications_from_entry(unit, cur_index,
    DPP_MCDS_TYPE_GET_START(group_type), &entries_remaining, group_size, &cur_index));

  /* get replications from the rest of the entries */
  while (cur_index != end_index) {
    DPP_MC_ASSERT(cur_index > 0 && cur_index < MCDS_INGRESS_LINK_END(mcds));
    SOCDNX_IF_ERR_EXIT(mcds->get_replications_from_entry(unit, cur_index, entry_type, &entries_remaining, group_size, &cur_index));
    if (*group_size > max_group_size) { /* group is somehow bigger than allowed - internal error */
      DPP_MC_ASSERT(0);
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("group is too big")));
    }
  }

exit:
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* Initialize MC replication database
* The initialization accesses the replication table as if it was an
* Ingress replication, for all entries (including Egress MC)
**********************************************************************/
uint32 dpp_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int unit
)
{
  const dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
  SOCDNX_INIT_FUNC_DEFS;

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM) /* do not init MCDB hardware in simulation, later instead of reading it into the mcds, use the init values */
#endif
  {
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IRR_MCDBm, MEM_BLOCK_ANY, mcds->free_value));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

