/* 
 * $Id: diag_field.c,v 1.42 Broadcom SDK $
 *
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
 *
 * File:        diag.c
 * Purpose:     Device diagnostics commands.
 *
 */



#include <shared/bsl.h>

#include <appl/diag/diag.h>
#include <appl/diag/diag_field.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#endif
#include <sal/appl/sal.h>

#ifdef BCM_DPP_SUPPORT
/******************************************************************** 
 *  Function handler: packet_diag_get (section fp)
 ********************************************************************/
cmd_result_t 
  cmd_ppd_api_fp_packet_diag_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPD_FP_PACKET_DIAG_INFO   
    *prm_info;
  uint32
    soc_sand_dev_id;

  prm_info = sal_alloc(sizeof(SOC_PPD_FP_PACKET_DIAG_INFO), "cmd_ppd_api_fp_packet_diag_get.prm_info");
  if(!prm_info) {
      cli_out("Memory allocation failure\n");
      return CMD_FAIL;
  }
  SOC_PPD_FP_PACKET_DIAG_INFO_clear(prm_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_fp_packet_diag_get(
          soc_sand_dev_id,
          SOC_CORE_DEFAULT,
          prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
      sal_free(prm_info);
      return CMD_FAIL; 
  } 

  SOC_PPD_FP_PACKET_DIAG_INFO_print(prm_info);
  sal_free(prm_info);
  return CMD_OK; 
} 

/******************************************************************** 
 *  Function handler: resource_diag_get (section fp)
 ********************************************************************/

cmd_result_t 
  cmd_ppd_api_fp_resource_diag_get(int unit, args_t* a)  
{   
  uint32 
    soc_sand_dev_id,
    mode,
    ret;   
  SOC_PPD_FP_RESOURCE_DIAG_INFO   
    *prm_info;
  char *arg_mode;

  prm_info = sal_alloc(sizeof(SOC_PPD_FP_RESOURCE_DIAG_INFO), "cmd_ppd_api_fp_resource_diag_get.prm_info");
  if(prm_info == NULL) {
    cli_out("Memory allocation failure\n");
    return CMD_FAIL;
  }
  
  SOC_PPD_FP_RESOURCE_DIAG_INFO_clear(prm_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  arg_mode = ARG_GET(a);
  if (! arg_mode ) {
      mode = 0;
  } else if (! sal_strncasecmp(arg_mode, "1", strlen(arg_mode))) {
      mode = 1;
  } else if (! sal_strncasecmp(arg_mode, "2", strlen(arg_mode))) {
      mode = 2;
  } else if (! sal_strncasecmp(arg_mode, "3", strlen(arg_mode))) {
      mode = 3;
  } else {
      mode = 0;
  } 

  /* Call function */
  ret = soc_ppd_fp_resource_diag_get(
          soc_sand_dev_id,
          mode,
          prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
      sal_free(prm_info); 
      return CMD_FAIL; 
  } 

  SOC_PPD_FP_RESOURCE_DIAG_INFO_print(unit, prm_info);
  sal_free(prm_info);
  
  return CMD_OK; 
}


extern cmd_result_t _bcm_petra_field_test_qualify_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot);
extern cmd_result_t _bcm_petra_field_test_action_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_field_group_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_field_group_destroy(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_entry_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
extern cmd_result_t _bcm_petra_field_test_entry_priority_2(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
extern cmd_result_t _bcm_petra_field_test_shared_bank(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_priority(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_cascaded(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_presel_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_presel_1(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifier_set(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_extraction(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_de_entry_traffic_large(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_field_group_direct_table(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_direct_table_entry_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_full_tcam_diff_prio(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_compress(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_field_group_traffic(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_itmh_parsing_test_pb(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_resend_last_packet(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
extern cmd_result_t _bcm_petra_field_test_user_header(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_watmboot);
STATIC cmd_result_t
cmd_diag_test(int unit, args_t* a)
{
    cmd_result_t res;
    cmd_result_t(*all_tests_func_ingress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_field_group_destroy_with_traffic,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_shared_bank,
        _bcm_petra_field_test_field_group_priority,
        _bcm_petra_field_test_cascaded,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_field_group_presel,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_field_group_direct_extraction,
        _bcm_petra_field_test_de_entry,
        _bcm_petra_field_test_de_entry_traffic,
        _bcm_petra_field_test_de_entry_traffic_large,
        _bcm_petra_field_test_field_group_direct_table,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_diff_prio,
        _bcm_petra_field_test_compress,
        _bcm_petra_field_test_itmh_field_group_traffic,
        _bcm_petra_field_test_itmh_parsing_test,
        _bcm_petra_field_test_itmh_parsing_test_pb,
        _bcm_petra_field_test_user_header
    };

    cmd_result_t(*all_tests_func_egress[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_destroy,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_presel,
        _bcm_petra_field_test_presel_set,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic
    };
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    cmd_result_t(*all_tests_func_external[])(int unit, uint8 stage, uint32 x, uint32 mode, uint8 do_warmboot) = {
        _bcm_petra_field_test_qualify_set,
        _bcm_petra_field_test_action_set,
        _bcm_petra_field_test_field_group,
        _bcm_petra_field_test_field_group_2,
        _bcm_petra_field_test_entry,
        _bcm_petra_field_test_entry_traffic,
        _bcm_petra_field_test_entry_priority,
        _bcm_petra_field_test_entry_priority_2,
        _bcm_petra_field_test_data_qualifiers,
        _bcm_petra_field_test_data_qualifier_set,
        _bcm_petra_field_test_data_qualifiers_entry,
        _bcm_petra_field_test_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic,
        _bcm_petra_field_test_full_tcam,
        _bcm_petra_field_test_full_tcam_diff_prio
    };
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

    uint32 i;
    uint32 x, i_mode;
    uint8 i_stage;
    char *mode;
    char *stage;
    parse_table_t pt;
    char *option;
    uint32 do_warmboot = 0;

    option = ARG_GET(a);
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "x", PQ_INT, (void *) (0),
            &x, NULL);
    parse_table_add(&pt, "Mode", PQ_STRING, (void *) "FAST",
          &mode, NULL);
    parse_table_add(&pt, "Stage", PQ_STRING, (void *) "INGRESS",
          &stage, NULL);
#ifdef BCM_WARM_BOOT_SUPPORT
    parse_table_add(&pt, "Warmboot", PQ_INT, (void *) (0),
          &do_warmboot, NULL);
#endif
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }

    if (ARG_CNT(a) != 0) {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }
#ifndef NO_SAL_APPL
    if(! sal_strncasecmp(mode, "SUPER_FAST", strlen(mode)) ) {
        i_mode = 0;
    } else if(! sal_strncasecmp(mode, "FAST", strlen(mode)) ) {
        i_mode = 1;
    } else if(! sal_strncasecmp(mode, "MEDIUM", strlen(mode)) ) {
        i_mode = 2;
    } else if(! sal_strncasecmp(mode, "SLOW", strlen(mode)) ) {
        i_mode = 3;
    } else if(! sal_strncasecmp(mode, "SCAN", strlen(mode)) ) {
        i_mode = 4;
    } else {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }

    if(! sal_strncasecmp(stage, "INGRESS", strlen(stage)) ) {
        i_stage = 0;
    } else if(! sal_strncasecmp(stage, "EGRESS", strlen(stage)) ) {
        i_stage = 1;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    } else if(! sal_strncasecmp(stage, "EXTERNAL", strlen(stage)) ) {
        i_stage = 2;
#endif /*defined(INCLUDE_KBP) && !defined(BCM_88030) */
    } else {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        return CMD_USAGE;
    }
    if(!option) {
        return CMD_USAGE;
    } else if(! sal_strncasecmp(option, "all", strlen(option)) ){
        if(i_stage == 0) {
            /* ingress */
            for(i=0; i < sizeof(all_tests_func_ingress)/sizeof(all_tests_func_ingress[0]); ++i) {
                res = all_tests_func_ingress[i](unit, i_stage, x, i_mode, do_warmboot);
                if(res != CMD_OK) {
                    return res;
                }
            }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        } else if(i_stage == 2) {
            /* external */
            for(i=0; i < sizeof(all_tests_func_external)/sizeof(all_tests_func_external[0]); ++i) {
                res = all_tests_func_ingress[i](unit, i_stage, x, i_mode, do_warmboot);
                if(res != CMD_OK) {
                    return res;
                }
            }
#endif /*defined(INCLUDE_KBP) && !defined(BCM_88030) */
        } else {
            /* egress */
            for(i=0; i < sizeof(all_tests_func_egress)/sizeof(all_tests_func_egress[0]); ++i) {
                res = all_tests_func_egress[i](unit, i_stage, x, i_mode, do_warmboot);
                if(res != CMD_OK) {
                    return res;
                }
            }
        }            
    } else if(! sal_strncasecmp(option, "qset", strlen(option)) ){
        return _bcm_petra_field_test_qualify_set(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "aset", strlen(option)) ){
        return _bcm_petra_field_test_action_set(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group", strlen(option)) ){
        return _bcm_petra_field_test_field_group(unit, i_stage, x, i_mode, do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    } else if(! sal_strncasecmp(option, "field_group_2", strlen(option)) ){
        return _bcm_petra_field_test_field_group_2(unit, i_stage, x, i_mode, do_warmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    } else if(! sal_strncasecmp(option, "field_group_destroy", strlen(option)) ){
        return _bcm_petra_field_test_field_group_destroy(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group_destroy_traffic", strlen(option)) ){
        return _bcm_petra_field_test_field_group_destroy_with_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group_destroy_traffic_de", strlen(option)) ){
        return _bcm_petra_field_test_field_group_destroy_with_traffic_and_de_fg(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "entry", strlen(option)) ){
        return _bcm_petra_field_test_entry(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "entry_traffic", strlen(option)) ){
        return _bcm_petra_field_test_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "entry_priority", strlen(option)) ){
        return _bcm_petra_field_test_entry_priority(unit, i_stage, x, i_mode, do_warmboot);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    } else if(! sal_strncasecmp(option, "entry_priority_2", strlen(option)) ){
        return _bcm_petra_field_test_entry_priority_2(unit, i_stage, x, i_mode, do_warmboot);
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    } else if(! sal_strncasecmp(option, "shared_bank", strlen(option)) ){
        return _bcm_petra_field_test_shared_bank(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group_priority", strlen(option)) ){
        return _bcm_petra_field_test_field_group_priority(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "cascaded", strlen(option)) ){
        return _bcm_petra_field_test_cascaded(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "presel", strlen(option)) ){
        return _bcm_petra_field_test_presel(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "presel_set", strlen(option)) ){
        return _bcm_petra_field_test_presel_set(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group_presel", strlen(option)) ){
        return _bcm_petra_field_test_field_group_presel(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "field_group_presel_1", strlen(option)) ){
        return _bcm_petra_field_test_field_group_presel_1(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "data_qualifiers", strlen(option)) ){
        return _bcm_petra_field_test_data_qualifiers(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "data_qualifier_set", strlen(option)) ){
        return _bcm_petra_field_test_data_qualifier_set(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "data_qualifiers_entry", strlen(option)) ){
        return _bcm_petra_field_test_data_qualifiers_entry(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "data_qualifiers_entry_traffic", strlen(option)) ){
        return _bcm_petra_field_test_data_qualifiers_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "predefined_data_qualifiers_entry_traffic", strlen(option)) ){
        return _bcm_petra_field_test_predefined_data_qualifiers_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_extraction_field_group", strlen(option)) ){
        return _bcm_petra_field_test_field_group_direct_extraction(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_extraction_entry", strlen(option)) ){
        return _bcm_petra_field_test_de_entry(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_extraction_entry_traffic", strlen(option)) ){
        return _bcm_petra_field_test_de_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_extraction_entry_traffic_large", strlen(option)) ){
        return _bcm_petra_field_test_de_entry_traffic_large(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_table_field_group", strlen(option)) ){
        return _bcm_petra_field_test_field_group_direct_table(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_table_entry", strlen(option)) ){
        return _bcm_petra_field_test_direct_table_entry(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "direct_table_entry_traffic", strlen(option)) ){
        return _bcm_petra_field_test_direct_table_entry_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "full_tcam", strlen(option)) ){
        return _bcm_petra_field_test_full_tcam(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "full_tcam_diff_prio", strlen(option)) ){
        return _bcm_petra_field_test_full_tcam_diff_prio(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "compress", strlen(option)) ){
        return _bcm_petra_field_test_compress(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "itmh_field_group", strlen(option)) ){
        return _bcm_petra_field_test_itmh_field_group(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "itmh_field_group_traffic", strlen(option)) ){
        return _bcm_petra_field_test_itmh_field_group_traffic(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "itmh_parsing_test", strlen(option)) ){
        return _bcm_petra_field_test_itmh_parsing_test(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "itmh_parsing_test_pb", strlen(option)) ){
        return _bcm_petra_field_test_itmh_parsing_test_pb(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "user_header", strlen(option)) ){
        return _bcm_petra_field_test_user_header(unit, i_stage, x, i_mode, do_warmboot);
    } else if(! sal_strncasecmp(option, "resend_last_packet", strlen(option)) ){
        return _bcm_petra_field_test_resend_last_packet(unit, i_stage, x, i_mode, do_warmboot);
    } else {
        return CMD_USAGE;
    }
    return CMD_OK;
#else
     cli_out("Option parsing is not supported when NO_SAL_APPL is defined\n");
     return CMD_USAGE;
#endif
}

cmd_result_t
cmd_dpp_diag_field(int unit, args_t* a)
{
    char      *function;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    } else if (DIAG_FUNC_STR_MATCH(function, "LAST_packet_get", "LAST")) {
        return cmd_ppd_api_fp_packet_diag_get(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "RESource_get", "RES")) {
        return cmd_ppd_api_fp_resource_diag_get(unit, a);
    } else if (! sal_strncasecmp(function, "test", strlen(function))){
        return cmd_diag_test(unit, a);
    } else {
        return CMD_USAGE;
    } 
}

void
print_field_usage(int unit)
{
    char cmd_dpp_diag_field_usage[] =
    "Usage (DIAG field):"
    "\n\tDIAGnotsics field commands\n\t"
    "Usages:\n\t"
    "DIAG field [OPTION] <parameters> ..."
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "OPTION can be:"
    "\nLAST_packet_get - Field ACL results (Key built, TCAM hit and actions done) for the last packet"
    "\n"
    "\nRESource_get <0/1/2> - Diagnostics to reflect which HW resources are used" 
    "\n                       0 - resource usage"
    "\n                       1 - include consistency between SW and HW"
    "\n                       2 - include validation of bank entries"
    "\n"
    "\ntest <test name> (or \"all\") "
    "\n\t\tx - key number."
    "\n\t\tmode - <Fast/Medium/Slow/Scan>. \n"
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    "\n\t\tstage - <Ingress/Egress/External>. \n"
#else
    "\n\t\tstage - <Ingress/Egress>. \n"
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#ifdef BCM_WARM_BOOT_SUPPORT
    "\n\t\twarmboot <0/1>. Do warmboot after field group and entries install. The default is FALSE.\n"
#endif
        ;
#endif   /*COMPILER_STRING_CONST_LIMIT*/

    cli_out(cmd_dpp_diag_field_usage);
}







cmd_result_t
cmd_dpp_diag_dbal(int unit, args_t* a)
{
    char    *function;
    char    *function1;
    int     val = -1, val1;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    } else if (DIAG_FUNC_STR_MATCH(function, "Tables_Info", "ti")) {
        if (arad_pp_dbal_tables_dump(unit, 0)) {
                return CMD_FAIL;
        }
    } else if (DIAG_FUNC_STR_MATCH(function, "Table", "t")) {
        function = ARG_GET(a);
        if (function) {
            val = sal_ctoi(function,0);
            if (arad_pp_dbal_table_info_dump(unit, val)) {
                return CMD_FAIL;
            }
        } else {
            return CMD_USAGE;
        }
    }else if (DIAG_FUNC_STR_MATCH(function, "Last_packet", "lp")) {
        if (arad_pp_dbal_last_packet_dump(unit)) {
            return CMD_FAIL;
        }        
    } else if (DIAG_FUNC_STR_MATCH(function, "Tables_Info_Full", "tif")) {
        if (arad_pp_dbal_tables_dump(unit, 1)) {
            return CMD_FAIL;
        }
    }else if (DIAG_FUNC_STR_MATCH(function, "CE_Information", "cei")) {
        function = ARG_GET(a);
        function1 = ARG_GET(a);
        if (function && function1) {
            val = sal_ctoi(function,0);
            val1 = sal_ctoi(function1,0);
            if (arad_pp_dbal_ce_per_program_dump(unit, val, val1)) {
                return CMD_FAIL;
            }
        }else {
            return CMD_USAGE;
        }
    } else {
        return CMD_USAGE;
    }
        
    return CMD_OK; 
}

void
print_dbal_usage(int unit)
{
    char cmd_dpp_diag_field_usage[] =
    "Usage (DIAG dbal):"
    "\n\tDIAGnotsics dbal commands\n\t"
    "Usages:\n\t"
    "DIAG dbal [OPTION] <parameters> ..."
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else
    "OPTION can be:"
    "\nTables_Info      - Return a brief information about all existing tables."
    "\nTable <ID>       - Returns full information for a specific table ID." 
    "\nTables_Info_Full - Returns full information for all existing tables."
    "\nLast_Packet      - Returns full information for the last packet lookups from the VT, TT and FLP."
    "\nCE_Information <programID> <stage> - Returns information about the copy engines for a specific program ID and stage.\n"
    "                       stage values according to the following: PMF = 0, FLP = 2, SLB = 3, VT = 4, TT = 5\n";
#endif   /*COMPILER_STRING_CONST_LIMIT*/

    cli_out(cmd_dpp_diag_field_usage);
}
#endif /* BCM_DPP_SUPPORT */
