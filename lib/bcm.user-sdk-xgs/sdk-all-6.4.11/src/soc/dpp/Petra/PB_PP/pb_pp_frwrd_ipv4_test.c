/* $Id: pb_pp_frwrd_ipv4_test.c,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

/*************
 * INCLUDES  *
 *************/
/* { */
 
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ip_tcam.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4_lpm_mngr.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4_test.h>

#include <sal/appl/sal.h>

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
typedef struct rule_payload_t {
  SOC_SAND_PP_IPV4_SUBNET rule;
  uint32 payload;
} rule_payload_t;

static SOC_PB_PP_IPV4_LPM_MNGR_INFO  *Lpm_mngr = 0;
static rule_payload_t *rule_list=NULL; /*List of rules that have been added.*/
static uint32 rule_count = 0; /*Number of rules in list*/


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */



#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK 0xFFFFFFF
#define SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT 0

#define SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(payload) \
  (((payload) >> SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_SHIFT) & SOC_PB_PP_IPV4_LPM_MNGR_PAT_DATA_FEC_MASK)

STATIC int32 soc_pb_pp_ipv4_test_rule_payload_compare(void *a, void *b) {
  const rule_payload_t *a1 = a;
  const rule_payload_t *b1 = b;

  if(a1->rule.prefix_len != b1->rule.prefix_len) {
    return(a1->rule.prefix_len - b1->rule.prefix_len);
  } else {
    return(a1->rule.ip_address - b1->rule.ip_address);
  }
}


uint32 soc_pb_pp_frwrd_ipv4_test_clear_vrf(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO *reset_info
  )
{
  uint32
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (rule_list)
  {
    sal_free(rule_list);
    rule_list = NULL;
  }

  if (reset_info->vrf == SOC_PB_PP_FRWRD_IP_ALL_VRFS_ID)
  {
    res = soc_pb_pp_frwrd_ipv4_vrf_all_routing_tables_clear(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    goto exit;
  }

  if (reset_info->vrf == 0)
  {
    res = soc_pb_pp_frwrd_ipv4_uc_routing_table_clear(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_pb_pp_frwrd_ipv4_vrf_routing_table_clear(unit,reset_info->vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_reset_vrf()", 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1
uint32 soc_pb_pp_frwrd_ipv4_test_print_mem(
  SOC_SAND_IN int                   unit,
  SOC_SAND_IN  uint32                    bnk_bmp,
  SOC_SAND_IN  uint32                    print_level
  )
{
  uint32
    indx;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PB_PP_FRWRD_IPV4_MEM_STATUS
    mem_status;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
          0,
          &lpm_mngr
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (indx = 1; indx < lpm_mngr.init_info.nof_mems; ++indx)
    {
      if (!(SOC_SAND_BIT(indx) & bnk_bmp))
      {
        continue;
      }
      if (print_level >= 1)
      {
        soc_sand_os_printf("\n\r\n\r bank: %u \n\r",indx);
        res = soc_sand_arr_mem_allocator_print_free_by_order(
            &lpm_mngr.init_info.mem_allocators[indx]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }

      res = soc_pb_pp_frwrd_ipv4_mem_status_get(unit,indx,&mem_status);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      SOC_PB_PP_FRWRD_IPV4_MEM_STATUS_print(&mem_status);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}
#endif


uint32 soc_pb_pp_frwrd_ipv4_test_defrag_mem(
  SOC_SAND_IN int                   unit,
  SOC_SAND_IN  uint32                    bnk_bmp,
  SOC_SAND_IN  uint32                    print_level
  )
{
  uint32
    indx;
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO
    defrag_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
          0,
          &lpm_mngr
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (indx = 1; indx < lpm_mngr.init_info.nof_mems; ++indx)
    {
      if (!(SOC_SAND_BIT(indx) & bnk_bmp))
      {
        continue;
      }
      SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(&defrag_info);
      soc_sand_os_printf("\n\r\n\r Defrage bank: %u \n\r",indx);

      res = soc_pb_pp_frwrd_ipv4_mem_defrage (unit,indx,&defrag_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}


uint32 soc_pb_pp_frwrd_ipv4_test_list_lookup(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT  SOC_PB_PP_FEC_ID                             *fec_id
  )
{
  uint32 best_rule, j;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  for(best_rule=0, j=1; j < rule_count; j++) {
    if((rule_list[j].rule.prefix_len == 0) ||
      ((rule_list[j].rule.ip_address ^ route_key->subnet.ip_address) >> (32 - rule_list[j].rule.prefix_len)) == 0) {
        best_rule = j;
    }
  }
  *fec_id = rule_list[best_rule].payload;


  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_list_lookup()", 0, 0);
}



uint32 soc_pb_pp_frwrd_ipv4_test_lookup(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO *lkup_info
  )
{
  SOC_PB_PP_IPV4_LPM_MNGR_INFO
    lpm_mngr;
  SOC_PB_PP_FEC_ID
    fec_id;
  uint32
    res;
  uint8
    found;
  SOC_PPC_DIAG_RESULT   ret_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* lookup in list */
  res = soc_pb_pp_frwrd_ipv4_test_list_lookup(unit,lkup_info->key.vrf,&(lkup_info->key.key),&fec_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  soc_sand_os_printf("***Lookup vrf:%u %08x/%d in LIST gives payload:  0x%x\n", lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id);

  /* lookup in HW*/
  soc_pb_pp_diag_frwrd_lpm_lkup_get(unit,0,&lkup_info->key,&fec_id, &found, &ret_val);
  soc_sand_os_printf("***Lookup vrf:%u %08x/%d in HW-diag gives payload:  0x%x\n", lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id);

/* lookup in SW-sim*/

  res = soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
        unit,
        &lpm_mngr
      );

  fec_id = soc_pb_pp_ipv4_lpm_mngr_lookup0(Lpm_mngr, unit, lkup_info->key.vrf, lkup_info->key.key.subnet.ip_address);
  soc_sand_os_printf("***Lookup vrf:%u %08x/%d in SW-simulation gives payload:  0x%x\n", lkup_info->key.vrf,lkup_info->key.key.subnet.ip_address, lkup_info->key.key.subnet.prefix_len, fec_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_print_mem_lest()", 0, 0);
}


uint32 soc_pb_pp_frwrd_ipv4_test_vrf(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                    vrf_ndx,
    SOC_SAND_IN  uint32                  nof_iterations
  )
{
  uint32
    res;
  uint32 i;
  SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO key;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO_clear(&key);
  key.key.vrf = vrf_ndx;

      for(i=0; i < rule_count; i++) {
        if(rule_list[i].rule.prefix_len == 0) {
          key.key.key.subnet.ip_address = (uint32)(sal_rand());
        } else {
          key.key.key.subnet.ip_address = rule_list[i].rule.ip_address;
          key.key.key.subnet.ip_address &= ~((1 << (32 - rule_list[i].rule.prefix_len))-1);
          key.key.key.subnet.ip_address |= (uint32)(sal_rand()) & ((1 << (32 - rule_list[i].rule.prefix_len))-1);
        }
        key.key.key.subnet.prefix_len = 32;
        /* if in-cache just remember this lookup */
        
      res = soc_pb_pp_frwrd_ipv4_test_lookup(unit,&key);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_vrf()", 0, 0);
}
#if SOC_PB_PP_DEBUG_IS_LVL1
uint32 soc_pb_pp_frwrd_ipv4_test_run(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PB_PP_FRWRD_IPV4_TEST_INFO *tes_info
  )
{
  SOC_SAND_PP_IPV4_SUBNET key;
  uint8 success;
  uint32
    indx;
    SOC_SAND_PAT_TREE_NODE_KEY node_key;
    SOC_SAND_PAT_TREE_NODE_INFO   lpm_info;
    SOC_SAND_PP_SYSTEM_FEC_ID        sys_fec_id,add_sys_fec_id=0,sys_fec_id2=0;
    SOC_PB_PP_DIAG_IPV4_VPN_KEY
      hw_key;
    uint32 *check_rule_list=NULL,check_rule_idx=0;
    uint32 i,print_level=tes_info->print_level;/* 0 none, 1//print added/remove, 2//print checked*/
    uint32 in_cache = 0;
    uint32 remove_prob=tes_info->remove_prob, till_cache=tes_info->cache_change, test = tes_info->test,run_traffic_sim=tes_info->hw_test;
    SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             route_key;
    SOC_SAND_SUCCESS_FAILURE                    fail_inidcation;
    uint32 nof_routes=tes_info->nof_routes;
    uint32
      nof_adds=0;
    uint32
      nof_removes=0;
    uint32
      res;
    uint8
      hw_ok=TRUE,
      found;
    SOC_PPC_DIAG_RESULT    ret_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

/************************************************************************/
/* Initialization of the LPM instance                                   */
/************************************************************************/
  Lpm_mngr = (SOC_PB_PP_IPV4_LPM_MNGR_INFO*) sal_dpp_alloc(sizeof(SOC_PB_PP_IPV4_LPM_MNGR_INFO),"Lpm_mngr MEM");
  soc_pb_pp_PB_PP_IPV4_LPM_MNGR_INFO_clear(Lpm_mngr);

  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_key);

  if (!rule_list)
  {
    rule_list = (rule_payload_t*)sal_dpp_alloc(sizeof(rule_payload_t)*10000, "rule_list MEM");
    rule_count = 1;
    rule_list[0].rule.ip_address = 0;
    rule_list[0].rule.prefix_len = 0;
    rule_list[0].payload = 0; /*default sys fec*/
  }
  /*check_rule_list = (uint32*)sal_dpp_alloc(sizeof(uint32)*5000);*/
  
  res = soc_pb_pp_sw_db_ipv4_lpm_mngr_get(
          0,
          Lpm_mngr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


    sal_srand(tes_info->seed);
    for(indx = 0; indx < nof_routes && rule_count < 10000 ; indx++) {
      key.ip_address = (uint32)(sal_rand());
      key.prefix_len = (uint8)(uint32)(sal_rand()) % 32 + 1;
      /* Petra b code. Almost not in use. Ignore coverity defects */
      /* coverity[large_shift] */
      key.ip_address &= ~(((uint32) -1) >> key.prefix_len);
      if (indx % 100 == 0)
      {
        soc_sand_os_printf("%d\n\r",indx);
      }
      
      /*now let's see if we can do better*/
      if((uint32)(sal_rand()) % 2 == 0 && rule_count > 0) {
        uint32 selected_rule;
        /*select a rule to make a sub-rule*/
        selected_rule = (uint32)(sal_rand()) % rule_count;
        if(rule_list[selected_rule].rule.prefix_len < 32 && rule_list[selected_rule].rule.prefix_len > 0 && rule_list[selected_rule].rule.prefix_len > 0) {
          key.ip_address = rule_list[selected_rule].rule.ip_address;
          key.ip_address &= ~((1 << (32 - rule_list[selected_rule].rule.prefix_len))-1);
          key.ip_address |= (uint32)(sal_rand()) & ((1 << (32 - rule_list[selected_rule].rule.prefix_len))-1);
          key.prefix_len = (uint8)(uint32)(sal_rand()) % 32 + 1;
        }
      }
      /*sys_fec_id = (uint32)(sal_rand()) & 0xf;*/
      if (tes_info->fec_id == 0xFFFFFFFF)
      {
        add_sys_fec_id = (uint32)(sal_rand()) & 0xff;
      }
      else
      {
        add_sys_fec_id = tes_info->fec_id;
      }

      route_key.subnet.ip_address = key.ip_address;
      route_key.subnet.prefix_len = key.prefix_len;
      if (!in_cache && (till_cache == 0 ||(indx % till_cache == till_cache-1) ))
      {
        if (print_level > 0)
        {
          soc_sand_os_printf("***Enable Caching \n **");
        }
        soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set(0,SOC_PB_PP_FRWRD_IP_ALL_VRFS_ID,SOC_PB_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM);
        in_cache = TRUE;
      }
      else if (in_cache &&  (till_cache!=0 && (indx % till_cache == till_cache-1)))
      {
        if (print_level > 0)
        {
          soc_sand_os_printf("***commit cached \n **");
          soc_sand_os_printf("***Disable Caching \n **");
        }
        /*soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set(0,0,0);*/
        soc_pb_pp_frwrd_ip_routes_cache_commit(0,/*vrf*/SOC_PB_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,0,&fail_inidcation);
        soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set(0,SOC_PB_PP_FRWRD_IP_ALL_VRFS_ID,SOC_PB_PP_FRWRD_IP_CACHE_MODE_NONE);
        in_cache = FALSE;
      }

      if (print_level > 0)
      {
        soc_sand_os_printf("***Adding %08x/%d, 0x%x\n", key.ip_address, key.prefix_len, add_sys_fec_id);
      }
      if (tes_info->add_prob != 0)
      {
        res = soc_pb_pp_frwrd_ipv4_uc_route_add(unit,&route_key,add_sys_fec_id,&fail_inidcation);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        if (fail_inidcation != SOC_SAND_SUCCESS)
        {
          soc_sand_os_printf("***ERROR in ADD***\n\r");
        }
        ++nof_adds;
      }
      
      /*add into the rule_list*/
      if (test && tes_info->add_prob != 0)
      {
        for(i=0; i < rule_count; i++) {
          if(rule_list[i].rule.prefix_len == key.prefix_len &&
             ((key.prefix_len == 0) ||
              ((rule_list[i].rule.ip_address ^ key.ip_address) >> (32 - key.prefix_len)) == 0)) {
            /*rule already exists, overwrite it*/
            rule_list[i].rule.ip_address = key.ip_address;
            rule_list[i].rule.prefix_len = key.prefix_len;
            rule_list[i].payload = add_sys_fec_id;
            break; /*done*/
          }
        }
        if(i >= rule_count) { /*didn't break, adding a new rule, no overwriting*/
          rule_list[rule_count].rule.ip_address = key.ip_address;
          rule_list[rule_count].rule.prefix_len = key.prefix_len;
          rule_list[rule_count].payload = add_sys_fec_id;
          rule_count++;
          /*sort the rule list*/
          soc_sand_os_qsort(rule_list, rule_count, sizeof(*rule_list),
            soc_pb_pp_ipv4_test_rule_payload_compare);
        }
      }
      /*AEZ*/
      /*if(rule_count > 0 && (uint32)(sal_rand()) % 100 < 20) {*/
      if (remove_prob > 0 && tes_info->add_prob == 0 && rule_count == 1)
      {
        soc_sand_os_printf("\n\r Nothin to remove \n\r ");
        goto exit;
      }
      if(rule_count > 0 && (uint32)(sal_rand()) % 101 >= 100 - remove_prob ) {
        uint32 selected_rule;

        selected_rule = (uint32)(sal_rand()) % rule_count;
        if(selected_rule != 0) {
          node_key.val = rule_list[selected_rule].rule.ip_address;
          node_key.prefix = rule_list[selected_rule].rule.prefix_len;
          key.ip_address = node_key.val;
          key.prefix_len = node_key.prefix;
          if (print_level > 0)
          {
            soc_sand_os_printf("***Removing %08x/%d\n", key.ip_address, key.prefix_len);
          }
          /*soc_pb_pp_ipv4_lpm_mngr_prefix_remove(Lpm_mngr, 0, &key, 0, &success);*/
          route_key.subnet.ip_address = key.ip_address;
          route_key.subnet.prefix_len = key.prefix_len;
          res = soc_pb_pp_frwrd_ipv4_uc_route_remove(0,&route_key,&fail_inidcation);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          if (fail_inidcation != SOC_SAND_SUCCESS)
          {
            soc_sand_os_printf("***ERROR in Remove***\n\r");
          }
          ++nof_removes;
          rule_count--;
          rule_list[selected_rule].rule.ip_address = rule_list[rule_count].rule.ip_address;
          rule_list[selected_rule].rule.prefix_len = rule_list[rule_count].rule.prefix_len;
          rule_list[selected_rule].payload = rule_list[rule_count].payload;
          /*sort the rule list*/
          soc_sand_os_qsort(rule_list, rule_count, sizeof(*rule_list), soc_pb_pp_ipv4_test_rule_payload_compare);
        }
      }

      if (in_cache &&  ((indx == nof_routes-1 &&till_cache==0)))
      {
        if (print_level > 0)
        {
          soc_sand_os_printf("***Disable Caching \n **");
        }
        /*soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set(0,0,0);*/
        soc_pb_pp_frwrd_ip_routes_cache_commit(0,/*vrf*/SOC_PB_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,0,&fail_inidcation);
        soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set(0,SOC_PB_PP_FRWRD_IP_ALL_VRFS_ID,SOC_PB_PP_FRWRD_IP_CACHE_MODE_NONE);
        in_cache = FALSE;
      }

      /* do defragment */

      /*if ((tes_info->defragment_rate == 0 ||(indx % tes_info->defragment_rate == tes_info->defragment_rate-1) ))*/
      if (((nof_adds) % tes_info->defragment_rate) >= tes_info->defragment_rate - 1)
      {
        res = soc_pb_pp_frwrd_ipv4_test_print_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = soc_pb_pp_frwrd_ipv4_test_defrag_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        res = soc_pb_pp_frwrd_ipv4_test_print_mem(unit,tes_info->defragment_banks_bmp,tes_info->print_level);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }

      /*Try to look for an interesting key.*/
      if (!test)
      {
        continue;
      }
      
      for(i=0; i < rule_count; i++) {
        uint32 best_rule, j;

        if(rule_list[i].rule.prefix_len == 0) {
          key.ip_address = (uint32)(sal_rand());
        } else {
          key.ip_address = rule_list[i].rule.ip_address;
          key.ip_address &= ~((1 << (32 - rule_list[i].rule.prefix_len))-1);
          key.ip_address |= (uint32)(sal_rand()) & ((1 << (32 - rule_list[i].rule.prefix_len))-1);
        }
        key.prefix_len = 32;
        node_key.val = key.ip_address;
        node_key.prefix = key.prefix_len;

        /* if in-cache just remember this lookup */
        if (in_cache)
        {
          /*check_rule_list[check_rule_idx++] = key.ip_address;*/
          continue;;
        }
        else
        {
          if (/*check_rule_idx >*/ 0)
          {
            key.prefix_len = 32;
            node_key.val = check_rule_list[--check_rule_idx];
            node_key.prefix = key.prefix_len;
            i = 0;
          }
        }
        
        
        for(best_rule=0, j=1; j < rule_count; j++) {
          if((rule_list[j].rule.prefix_len == 0) ||
             ((rule_list[j].rule.ip_address ^ key.ip_address) >> (32 - rule_list[j].rule.prefix_len)) == 0) {
            best_rule = j;
          }
        }
        soc_sand_pat_tree_lpm_get(&Lpm_mngr->init_info.lpms[0], &node_key, 0, &lpm_info, &success);
        sys_fec_id = soc_pb_pp_ipv4_lpm_mngr_lookup0(Lpm_mngr, 0, 0, key.ip_address);
        if (run_traffic_sim)
        {
          SOC_PB_PP_DIAG_IPV4_VPN_KEY_clear(&hw_key);
          hw_key.vrf = 0;
          hw_key.key.subnet.ip_address = key.ip_address;
          hw_key.key.subnet.prefix_len = 32;

          soc_pb_pp_diag_frwrd_lpm_lkup_get(0,0, &hw_key,&sys_fec_id2, &found, &ret_val);
          if (sys_fec_id2 != sys_fec_id)
          {
            hw_ok = FALSE;
          }
        }
        else
        {
          hw_ok = TRUE;
        }
        if (print_level > 1)
        {
          soc_sand_os_printf("***Lookup %08x/%d in list gives payload: %x\n", key.ip_address, key.prefix_len, rule_list[best_rule].payload);
          soc_sand_os_printf("***Lookup %08x/%d in pat gives payload:  %x\n", key.ip_address, key.prefix_len, SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload));
          soc_sand_os_printf("***Lookup %08x/%d in lpm gives payload:  %x\n", key.ip_address, key.prefix_len, sys_fec_id);
        }
        if(rule_list[best_rule].payload != sys_fec_id || SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload) != sys_fec_id || !hw_ok) {
          soc_sand_pat_tree_lpm_get(&Lpm_mngr->init_info.lpms[0], &node_key, 0, &lpm_info, &success);
          soc_sand_os_printf("indx = %d, rule_count = %d\n", indx, rule_count);
          soc_sand_os_printf("***Lookup %08x/%d in list gives payload: %x\n", key.ip_address, key.prefix_len, rule_list[best_rule].payload);
          soc_sand_os_printf("***Lookup %08x/%d in pat gives payload:  %x\n", key.ip_address, key.prefix_len, SOC_PB_PP_IPV4_LPM_MGR_PAT_DATA_TO_FEC_ID(lpm_info.data.payload));
          soc_sand_os_printf("***Lookup %08x/%d in lpm gives payload:  %x\n", key.ip_address, key.prefix_len, sys_fec_id);
          if (!hw_ok)
          {
            soc_sand_os_printf("***Lookup %08x/%d in list gives payload: %x\n", key.ip_address, key.prefix_len, sys_fec_id2);
          }
          for(j=0; j < rule_count; j++) {
            soc_sand_os_printf("Rule: %08x/%d, %x\n", rule_list[j].rule.ip_address, rule_list[j].rule.prefix_len, rule_list[j].payload);
          }
          soc_sand_os_printf("error\n");
          return(1);
        }
      }
      if (print_level > 0)
      {
        soc_sand_os_printf("indx = %d, rule_count = %d\n", indx, rule_count);
      }
      
    }
    if (print_level > 0)
    {
      soc_sand_os_printf("indx = %d, rule_count = %d\n", indx, rule_count);
    }
/*
    if (rule_list)
    {
      sal_free(rule_list);
    }*/
    if (check_rule_list)
    {
      sal_free(check_rule_list);
    }

    if (print_level == 0)
    {
      soc_sand_arr_mem_allocator_print_free(
        &Lpm_mngr->init_info.mem_allocators[5]);
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ipv4_test_run()", 0, 0);
}
#endif


void
  SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO));
  SOC_PB_PP_DIAG_IPV4_VPN_KEY_clear(&(info->key));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_IPV4_TEST_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_IPV4_TEST_INFO));
  info->seed = 50;
  info->nof_routes = 0;
  info->cache_change = 0;
  info->test = 0;
  info->hw_test = 0;
  info->print_level = 0;
  info->remove_prob = 20;
  info->add_prob = 100;
  info->fec_id = 0xFFFFFFFF;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO));
  info->vrf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1
void
  SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_TEST_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("key:");
  SOC_PB_PP_DIAG_IPV4_VPN_KEY_print(&(info->key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_IPV4_TEST_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("seed: %u\n\r",info->seed);
  soc_sand_os_printf("nof_routes: %u\n\r",info->nof_routes);
  soc_sand_os_printf("cache_change: %u\n\r",info->cache_change);
  soc_sand_os_printf("test: %u\n\r",info->test);
  soc_sand_os_printf("hw_test: %u\n\r",info->hw_test);
  soc_sand_os_printf("print_level: %u\n\r",info->print_level);
  soc_sand_os_printf("remove_prob: %u\n\r",info->remove_prob);
  soc_sand_os_printf("add_prob: %u\n\r",info->add_prob);
  soc_sand_os_printf("fec_id: %u\n\r",info->fec_id);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_CLEAR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("vrf: %u\n\r",info->vrf);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

