/* $Id: arad_kbp.c,v 1.50 Broadcom SDK $
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

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ilm.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/dpp_wb_engine.h>

#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
#include <soc/dpp/SAND/Management/sand_low_level.h>
#endif 

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define arad_kbp_alloc(s,x) sal_alloc((s*x),__FILE__)
#define arad_kbp_free(s) sal_free(s)

/* KBP mdio */
#define ARAD_KBP_MDIO_CLAUSE 45

/* KBP ROP test */
#define ARAD_KBP_ROP_TEST_REG_ADDR       0x102 
#define ARAD_KBP_ROP_TEST_DATA          {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x35}

/*
 *    ELK CPU/ROP Defines
 */






/* 
 *  Arad NLM Application
 */ 

 /* Number of devices */

/* 
 * ARAD frwarding NLM Application Defines
 */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */


void alg_kbp_callback(void *handle, struct kbp_db *db,struct kbp_entry *entry, int32_t old_index, int32_t new_index);


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct kbp_init_user_data_s{
    int device;
    uint32 kbp_mdio_id;
}kbp_init_user_data_t;

typedef struct kbp_warmboot_s{
    FILE *kbp_file_fp;
    kbp_device_issu_read_fn  kbp_file_read;
    kbp_device_issu_write_fn kbp_file_write;
}kbp_warmboot_t;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
static
    ARAD_KBP_TABLE_CONFIG
        Arad_kbp_table_config_info_static[ARAD_KBP_FRWRD_IP_NOF_TABLES] = {

            /* TABLE 0 (IPv4 + RPF): 80b, <4'b 0s> <12b VRF> <32b DIP> <32b don't care> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 1 (IPv4 + RPF): 80b, <4'b 0s> <12b VRF> <32b SIP> <32b don't care> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            }, 

            /* TABLE 2 (IPv4 Multicast + RPF): 80b, <12b IN-RIF> <32b SIP> <28b DIP> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 3 (IPv6 Unicast) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_160,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                128, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },
            
            /* TABLE 4 (IPv6 Unicast RPF) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_160,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                128, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 5 (IPv6 Multicast) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_320,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 6 (MPLS) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_LSR, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 7 (TRILL UC) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 8 (TRILL MC) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 9 (DUMMY-0) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 10 (DUMMY-1) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 11 (DUMMY-2) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 12 (DUMMY-3) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_3, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },
        };  

static 
    ARAD_KBP_LTR_CONFIG
        Arad_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0: IPv4 UC (without RPF) */
            {
                FALSE, /* valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}                
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 1: IPv4 UC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF, /* ltr_id */
                0x3, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 2: IPv4 MC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, /* ltr_id */
                0x3, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {4, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 3: IPv6 UC (without RPF) */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },

            /* DB 4: IPv6 UC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF, /* ltr_id */
                0x3, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 5: IPv6 MC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF, /* ltr_id */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                0x3, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                    {3, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                    {2, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}}
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {4, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 6: MPLS */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, /* opcode */
                ARAD_KBP_FRWRD_LTR_LSR, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 7: TRILL UC */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_TRILL_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 8: TRILL MC */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, /* opcode */
                ARAD_KBP_FRWRD_LTR_TRILL_MC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/ 
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 
        };

static 
    arad_kbp_lut_data_t
        Arad_kbp_gtm_lut_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV4_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 1 : IPv4 UC+RPF PROGRAM */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 2 : IPv4 MC PROGRAM */
            {
                12, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 3 : IPv6 UC PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                34, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                18, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV6_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 4 : IPv6 UC+RPF PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                34, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                18, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 5 : IPv6 MC+RPF PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                35, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                19, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 6 : MPLS PROGRAM */
            {
                6, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_LSR), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 7 : TRILL UC PROGRAM */
            {
                2, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_TRILL_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 8 : TRILL MC PROGRAM */
            {
                4, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_TRILL_MC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

        };

static 
    arad_kbp_frwd_ltr_db_t
        Arad_kbp_gtm_ltr_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 1 : IPv4 UC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_RPF, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 2 : IPv4 MC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 3 : IPv6 UC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 4 : IPv6 UC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 5 : IPv6 MC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, 
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
            },

            /* DB 6 : MPLS */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 7 : TRILL UC */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 8 : TRILL MC */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

        };

static
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO
        Arad_kbp_gtm_opcode_config_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 1 : IPv4 UC + RPF PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 2 (Second Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },


            /* DB 2 : IPv4 MC + RPF PROGRAM */
            {
                (12 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 2 (Second Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 3 : IPv6 UC PROGRAM */
            {
                (34 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 4 : IPv6 UC + RPF PROGRAM */
            {
                (34 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 2 (Second Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 5 : IPv6 MC + RPF PROGRAM */
            {
                (35 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 2 (Second Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 6 : MPLS PROGRAM */
            {
                (6 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 7 : TRILL UC PROGRAM */
            {
                (2 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 8 : TRILL MC PROGRAM */
            {
                (4 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

        };

#ifdef BCM_88660_A0
static
    ARAD_KBP_TABLE_CONFIG
        Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_IP_NOF_TABLES] = {

            /* TABLE 0 (IPv4 + RPF): 80b, <4'b 0s> <12b VRF> <32b DIP> <32b don't care> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 1 (IPv4 + RPF): 80b, <4'b 0s> <12b VRF> <32b SIP> <32b don't care> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0 /* Clone table ID */
            }, 

            /* TABLE 2 (IPv4 Multicast + RPF): 80b, <12b IN-RIF> <32b SIP> <28b DIP> */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 3 (IPv6 Unicast) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_160,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                128, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },
            
            /* TABLE 4 (IPv6 Unicast RPF) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_160,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                128, /* Min Priority */
				ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0 /* Clone table ID */
            },

            /* TABLE 5 (IPv6 Multicast) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_320,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 6 (MPLS) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_LSR, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 7 (TRILL UC) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 8 (TRILL MC) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                0, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 9 (DUMMY-0) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_0, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_64B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 10 (DUMMY-1) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 11 (DUMMY-2) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 12 (DUMMY-3) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_3, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_32B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                -1, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 13 (IP LSR SHARED table) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_128B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 14 (IP LSR SHARED for IP + RPF) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_128B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED /* Clone table ID */
            },

            /* TABLE 15 (IP LSR SHARED for LSR) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,   /* Table Width */
                NLM_TBL_ADLEN_128B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED /* Clone table ID */
            },

            /* TABLE 16 (IPv6 extended table) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_160,   /* Table Width */
                NLM_TBL_ADLEN_128B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },

            /* TABLE 17 (p2p extended table) */ 
            {   
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P, /* Table ID */ 
                0, /* Table Size */
                NLM_TBL_WIDTH_80,    /* Table Width */
                NLM_TBL_ADLEN_128B,  /* Associated Data Width */ 
                0,  /* Group ID Start */
                20, /* Group ID End */
                NLMDEV_BANK_0, /* Bank Number */
                32, /* Min Priority */
				ARAD_KBP_FRWRD_IP_NOF_TABLES /* Clone table ID */
            },
        };  

static 
    ARAD_KBP_LTR_CONFIG
        Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0: IPv4 UC (without RPF) */
            {
                FALSE, /* valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}                
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 1: IPv4 UC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF, /* ltr_id */
                0x5, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmTRUE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {0},
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 2: IPv4 MC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, /* ltr_id */
                0x5, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmTRUE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {0},
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {4, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 3: IPv6 UC (without RPF) */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },
            /* DB 4: IPv6 UC (with RPF in 2 Pass - for Pass 1 SIP) */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS, /* ltr_id */
                0x0, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                    {2, {{"VRF", 2}, {"SIP", 16}}},
                    {2, {{"VRF", 2}, {"SIP", 16}}}                    
                },
                /* master key */
                {2, {{"VRF", 2}, {"SIP", 16}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },
            /* DB 5: IPv6 UC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF, /* ltr_id */
                0x5, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmTRUE, /* is_cmp3_search */
                { /* ltr searches */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {0},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {0},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 6: IPv6 MC with RPF */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF, /* ltr_id */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                0x5, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NlmTRUE, /* is_cmp3_search */
                { /* ltr searches */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                    {3, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {0},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                    {2, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}}
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                },
                /* master key */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                {4, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}},
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"MC-GROUP", 15, KBP_KEY_FIELD_PREFIX}}},
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 7: MPLS */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, /* opcode */
                ARAD_KBP_FRWRD_LTR_LSR, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 8: TRILL UC */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, /* opcode */
                ARAD_KBP_FRWRD_LTR_TRILL_UC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }, 

            /* DB 9: TRILL MC */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, /* opcode */
                ARAD_KBP_FRWRD_LTR_TRILL_MC, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/ 
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },

            /* DB 10: Shared IPv4 LSR primary (not used for any search) */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_NONE, /* opcode */                
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },  

            /* DB 11: Shared IPv4 LSR Shared for IP */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP, /* opcode */
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"HOLE", 3, KBP_KEY_FIELD_HOLE},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },  

            /* DB 12: IPv4 LSR Shared for IP with RPF*/
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP_WITH_RPF, /* opcode */
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF, /* ltr_id */
                0x3, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"HOLE", 3, KBP_KEY_FIELD_HOLE},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {0},
                {0}
                },
                /* master key */
                {3, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },  

            /* DB 13: Shared IPv4 LSR Shared for LSR */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_LSR, /* opcode */
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX}, {"HOLE", 2, KBP_KEY_FIELD_HOLE}, {"HOLE", 4, KBP_KEY_FIELD_HOLE}}}
                },
                /* master key */
                {1, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },
            
            /* DB 14: extended IPv6 */
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_IPV6, /* opcode */
                ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {2, {{"VRF", 2, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            },

            /* DB 15: extended p2p Ronen*/
            {
                FALSE, /* Valid */
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P, /* opcode */
                ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P, /* ltr_id */
                0x1, /* parallel_srches_bmp */
                {ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P, ARAD_KBP_FRWRD_TBL_ID_DUMMY_1, ARAD_KBP_FRWRD_TBL_ID_DUMMY_2, ARAD_KBP_FRWRD_TBL_ID_DUMMY_3}, /* tbl_id's*/
                {ARAD_KBP_DB_TYPE_FORWARDING, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES, ARAD_KBP_NOF_DB_TYPES}, /* Search Types*/ 
                NlmFALSE, /* is_cmp3_search */
                { /* ltr searches */
                {1, {{"IN-LIF-ID", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                /* master key */
                {1, {{"IN-LIF-ID", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, /* ARAD_KBP_INSTRUCTION */
                NULL, /* ARAD_KBP_KEY */
            }
        };

static 
    arad_kbp_lut_data_t
        Arad_plus_kbp_gtm_lut_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV4_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 1 : IPv4 UC+RPF PROGRAM */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 2 : IPv4 MC PROGRAM */
            {
                12, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 3 : IPv6 UC PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                34, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                18, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV6_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 


            /* DB 4 : IPv6 UC (with RPF in 2 Pass - for Pass 1 SIP) PROGRAM */
            {
                18, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 
            /* DB 5 : IPv6 UC+RPF PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                34, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                18, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 6 : IPv6 MC+RPF PROGRAM */
            {
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                35, /* Rec Size - bytes */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                19, /* Rec Size - bytes */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                ARAD_KBP_LUT_AD_TRANSFER_2B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result2_idx_or_ad */
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 7 : MPLS PROGRAM */
            {
                6, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_LSR), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 8 : TRILL UC PROGRAM */
            {
                2, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_TRILL_UC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 9 : TRILL MC PROGRAM */
            {
                4, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_TRILL_MC), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_6B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } ,

            /* DB 10 : IPv4 LSR Shared */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 11 : IPv4 LSR Shared for IP */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } , 

            /* DB 12 : IPv4 LSR Shared for IP RPF */
            {
                10, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_4B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } ,

            /* DB 13 : IPv4 MPLS Shared for LSR */
            {
                4, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } ,

            /* DB 14 : IPv6 extended */
            {
                18, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } ,

                        /* DB 15 : p2p extended */
            {
                2, /* Rec Size - bytes */
                ARAD_KBP_LUT_REC_TYPE_REQUEST, /* Rec Type */
                0x1, /* Rec Valid */
                ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE, /* Mode */
                ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA, /* Key Config */
                0x0, /* lut_key_data */
                ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P), /* Instruction - LTR */
                0x0, /* key_w_cpd_gt_80 */
                0x0, /* copy_data_cfg */
                ARAD_KBP_LUT_AD_TRANSFER_11B, /* result0_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_AD_ONLY, /* result0_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result1_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result1_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result2_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result2_idx_or_ad */
                ARAD_KBP_LUT_AD_TRANSFER_1B, /* result3_idx_ad_cfg */
                ARAD_KBP_LUT_TRANSFER_INDX_ONLY, /* result3_idx_or_ad */
                0x0, /* result4_idx_ad_cfg */
                0x0, /* result4_idx_or_ad */
                0x0, /* result5_idx_ad_cfg */
                0x0  /* result5_idx_or_ad */
            } ,
        };

static 
    arad_kbp_frwd_ltr_db_t
        Arad_plus_kbp_gtm_ltr_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 1 : IPv4 UC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_RPF, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 2 : IPv4 MC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 3 : IPv6 UC PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 4 : IPv6 IPv6 UC (with RPF in 2 Pass - for Pass 1 SIP) PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_4B) 
            },

            /* DB 5 : IPv6 UC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
            },

            /* DB 6 : IPv6 MC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, 
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, ARAD_KBP_LUT_AD_TRANSFER_2B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B + ARAD_KBP_LUT_AD_TRANSFER_2B) 
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
            },

            /* DB 7 : MPLS */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 8 : TRILL UC */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 9 : TRILL MC */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_6B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_64B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_6B) 
            },

            /* DB 10 : shared PRIMARY */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_NONE, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B) 
            },

            /* DB 11 : shared IP */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B) 
            },

            /* DB 12 : shared IPv4 UC + RPF PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP_WITH_RPF, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, ARAD_KBP_LUT_AD_TRANSFER_4B, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_INDEX_AND_32B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B + ARAD_KBP_LUT_AD_TRANSFER_4B) 
            },

            /* DB 13 : shared LSR PROGRAM */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_LSR, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B) 
            },

            /* DB 14 : IPv6 extended */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_IPV6, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B) 
            },

            /* DB 15 : p2p extended */
            {
                /* Opcode */
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P, 
                /* Result Data Length for each of 4 lookups */
                {ARAD_KBP_LUT_AD_TRANSFER_11B, 0x0, 0x0, 0x0},
                /* Result Data Format */
                {NLM_ARAD_INDEX_AND_128B_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD, NLM_ARAD_NO_INDEX_NO_AD},
                /* Result Total Length */
                (1 + ARAD_KBP_LUT_AD_TRANSFER_11B) 
            },

        };

static
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO
        Arad_plus_kbp_gtm_opcode_config_info_static[ARAD_KBP_FRWRD_DB_NOF_TYPES] = {

            /* DB 0 : IPv4 UC PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 1 : IPv4 UC + RPF PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 4 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 4 + (Second result) + 2 (Third Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },


            /* DB 2 : IPv4 MC + RPF PROGRAM */
            {
                (12 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 4 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 4 + (Second result) + 2 (Third Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 3 : IPv6 UC PROGRAM */
            {
                (34 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 4 : IPv6 IPv6 UC (with RPF in 2 Pass - for Pass 1 SIP) PROGRAM */
            {
                (18 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 4 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 4 + (Second result) + 2 (Third Result) - 1 (minus 1) */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 5 : IPv6 UC + RPF PROGRAM */
            {
                (34 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 4 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 4 + (Second result) + 2 (Third Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 6 : IPv6 MC + RPF PROGRAM */
            {
                (35 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 + 4 + 2 - 1), /* RX Data Size (1 (action) + 6 (First Result) + 4 + (Second result) + 2 (Third Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 7 : MPLS PROGRAM */
            {
                (6 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 8 : TRILL UC PROGRAM */
            {
                (2 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 9 : TRILL MC PROGRAM */
            {
                (4 - 1), /* TX Data Size (18 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 6 - 1), /* RX Data Size (1 (action) + 6 (First Result) + - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 10 : shared  */
            {
                (0), /* TX Data Size (10 bytes, minus 1) */
                0, /* TX Data Type (REQUEST) */
                (0), /* RX Data Size (1 (action) + 11 (First Result) - 1 (minus 1)  */
                0 /* RX Data Type (REPLY) */
            },

            /* DB 11 : shared IPv4 UC PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 11 - 1), /* RX Data Size (1 (action) + 11 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 12 : shared IPv4 UC + RPF PROGRAM */
            {
                (10 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 11 + 4 - 1), /* RX Data Size (1 (action) + 11 (First Result) + 4 + (Second result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 13 : shared LSR PROGRAM */
            {
                (3 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 11 - 1), 
                3 /* RX Data Type (REPLY) */
            },

            /* DB 14 : extended IPv6 PROGRAM */
            {
                (18 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 11 - 1), /* RX Data Size (1 (action) + 11 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

            /* DB 15 : extended IPv6 PROGRAM */
            {
                (2 - 1), /* TX Data Size (10 bytes, minus 1) */
                1, /* TX Data Type (REQUEST) */
                (1 + 11 - 1), /* RX Data Size (1 (action) + 11 (First Result) - 1 (minus 1)  */
                3 /* RX Data Type (REPLY) */
            },

        };
#endif /* BCM_88660_A0 */

/* mapping DB type to LTR id */
static
    ARAD_KBP_FRWRD_IP_LTR
        Arad_kbp_db_type_to_ltr[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS] = {
            ARAD_KBP_FRWRD_LTR_IPV4_UC,
            ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF,
            ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, 
            ARAD_KBP_FRWRD_LTR_IPV6_UC,
            ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS,
			ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF,
            ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF,
            ARAD_KBP_FRWRD_LTR_LSR, 
            ARAD_KBP_FRWRD_LTR_TRILL_UC,
            ARAD_KBP_FRWRD_LTR_TRILL_MC,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF,
            ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR,
            ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6,
            ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P
        };

static kbp_warmboot_t kbp_warmboot_data[SOC_SAND_MAX_DEVICE];

static
    ARAD_KBP_TABLE_CONFIG
        Arad_kbp_table_config_info[SOC_SAND_MAX_DEVICE][ARAD_KBP_MAX_NUM_OF_TABLES];
static 
    ARAD_KBP_LTR_CONFIG
        Arad_kbp_ltr_config[SOC_SAND_MAX_DEVICE][ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
static 
    arad_kbp_lut_data_t
        Arad_kbp_gtm_lut_info[SOC_SAND_MAX_DEVICE][ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
static 
    arad_kbp_frwd_ltr_db_t
        Arad_kbp_gtm_ltr_info[SOC_SAND_MAX_DEVICE][ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
static
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO
        Arad_kbp_gtm_opcode_config_info[SOC_SAND_MAX_DEVICE][ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];

static 
    genericTblMgrAradAppData 
        *AradAppData[SOC_SAND_MAX_DEVICE];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/* 
 *  Utility functions
 */


/* 
 *  Test functions
 */

/* This function takes unsigned integer, puts each byte into array pointed by 'data' */
STATIC void WriteValueToBitVector4( uint8 *data, uint32 value )
{
      data[ 0 ] = ( uint8 )( 0xFF & ( value >> 24 ) );
      data[ 1 ] = ( uint8 )( 0xFF & ( value >> 16 ) );
      data[ 2 ] = ( uint8 )( 0xFF & ( value >> 8 ) );
      data[ 3 ] = ( uint8 )( 0xFF & ( value >> 0 ) );
}

void FillRecordPatternFor_80B0_Table(uint8 *data_p, uint8 *mask_p, uint32 iter)
{
    /* 0.0.32b <source/dest_IP>.X.X.X.X */

    sal_memset( data_p, 0, 10 );
    sal_memset( mask_p, 0, 10 );

    /* first 2B are 0, second 4B are source/dest IP, remaining 4B don't cares. MS 48b valid,
     * so mask the LS 32b
     */
    WriteValueToBitVector4(&data_p[2], iter);

    /* Mask off last 4 bytes */
    sal_memset(&mask_p[6], 0xFF, 4 );

    return;
}

/* This function fills the internal data strcture for tables with records */
STATIC void CreateRecordsInTables_nosmt(genericTblMgrAradAppData *refAppData_p,
    uint32 num_entries,
    uint32 record_base_tbl[4],
    uint32 ad_val_tbl[4] )
{
    tableInfo *tblInfo_p;
    tableRecordInfo *tblRecordInfo_p;
    uint32 alloc_size;
    uint8  iter_tbl, tblWidth_inBytes;
    uint8  *rec_data, *rec_mask;
    uint16  start, end, iter_group, iter_priority;
    uint32 loop = 0, start_value = 0, advlaue = 0;

    /* First allocate memory for storing records within the data structures.
     * Each table structure has start groupId and end groupId. Records of
     * all groupIds including the start-end groupId are added. With each
     * groupId, priorities of the records would range from [0 TO groupId-1].
     */
     for( iter_tbl = 0; iter_tbl < ARAD_KBP_FRWRD_IP_NOF_TABLES; iter_tbl++ )
     {
         if (NULL == refAppData_p->g_gtmInfo[iter_tbl].tblInfo.db_p) {
            /* This table is not initialized, continue to next table */
            continue;
        }

        tblInfo_p = &refAppData_p->g_gtmInfo[iter_tbl].tblInfo;

        start = tblInfo_p->groupId_start;
        end = tblInfo_p->groupId_end;

        tblInfo_p->tbl_size = num_entries;
        alloc_size = tblInfo_p->tbl_size;
        tblInfo_p->tblRecordInfo_p = arad_kbp_alloc(alloc_size, sizeof( tableRecordInfo ) );

        tblInfo_p->max_recCount    = alloc_size;
         tblWidth_inBytes = (uint8)(tblInfo_p->tbl_width / 8);

        rec_data = arad_kbp_alloc(1, tblWidth_inBytes );
        rec_mask = arad_kbp_alloc(1, tblWidth_inBytes );

        start_value = record_base_tbl[iter_tbl]; /* dest  Ip: table-0 */
        advlaue = ad_val_tbl[iter_tbl];      /* 64b  ad:         */

        /* store the records */
        iter_group = start;
        iter_priority = 0;
        tblRecordInfo_p = tblInfo_p->tblRecordInfo_p;

        for(loop = 0; loop < tblInfo_p->tbl_size; loop++)
        {
            tblRecordInfo_p->groupId  = iter_group;
            tblRecordInfo_p->priority = iter_priority;

            tblRecordInfo_p->record.m_data = arad_kbp_alloc(1, tblWidth_inBytes );
            tblRecordInfo_p->record.m_mask = arad_kbp_alloc(1, tblWidth_inBytes );
            tblRecordInfo_p->record.m_len  = tblInfo_p->tbl_width;

            /* Generate the ACL record here */
            FillRecordPatternFor_80B0_Table(rec_data, rec_mask, (start_value + loop));

            /* associated data : msb32b */
            WriteValueToBitVector4(&tblRecordInfo_p->assoData[0], (advlaue + loop));

            if( tblInfo_p->tbl_id  == 0) /* lsb 32b associated data */
                WriteValueToBitVector4(&tblRecordInfo_p->assoData[4], (advlaue + loop));

            sal_memcpy( tblRecordInfo_p->record.m_data, rec_data, tblWidth_inBytes );
            sal_memcpy( tblRecordInfo_p->record.m_mask, rec_mask, tblWidth_inBytes );

            tblRecordInfo_p++;

#ifdef WITH_INDEX_SHUFFLE
            iter_priority++;

            /* loop back priority */
            if( (iter_priority % end) == 0)
            {
                iter_priority = start;
                iter_group++;
            }

            /* loop back group */
            if( iter_group && ((iter_group % end) == 0))
            {
                iter_group = start;
            }
#else
            iter_group++;
            iter_priority++;

            /* loop back priority */
            if( (iter_priority % end) == 0)
            {
                iter_priority = start;
            }

            /* loop back group */
            if( (iter_group % end) == 0)
            {
                iter_group = start;
            }
#endif
        }

        /* Free the memory allocated for temp data and mask. A fresh memory will be
         * based on the table width
         */
        arad_kbp_free(rec_data );
        arad_kbp_free(rec_mask );

     } /* end of table's loop */
}


/* Adds records into various tables */
STATIC int AddRecordsIntoTables(
                genericTblMgrAradAppData *refAppData_p,
                uint8 instance
                )
{
    tableInfo *tblInfo_p;
    tableRecordInfo    *tblRecordInfo_p;
    uint32 iter_rec, num_recs;
    struct kbp_ad_db *ad_db_p;
    struct kbp_entry *dummy_entry;

    tblInfo_p = &refAppData_p->g_gtmInfo[instance].tblInfo;
    tblRecordInfo_p = tblInfo_p->tblRecordInfo_p;
    num_recs  = tblInfo_p->max_recCount;

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META("\n Adding records into table = %d\n"),instance));
    for(iter_rec = 0; iter_rec < num_recs; iter_rec++ )
    {
        struct kbp_ad *ad_entry;

        /* Add record now */
        KBP_TRY(kbp_db_add_ace(tblInfo_p->db_p, tblRecordInfo_p->record.m_data, tblRecordInfo_p->record.m_mask,
                             tblRecordInfo_p->priority, &dummy_entry));

        /* Add the AD */
        if(tblInfo_p->tbl_assoWidth){
        ad_db_p = refAppData_p->g_gtmInfo[instance].tblInfo.ad_db_p;
        KBP_TRY(kbp_ad_db_add_entry(ad_db_p,tblRecordInfo_p->assoData,&ad_entry));
        KBP_TRY(kbp_entry_add_ad(tblInfo_p->db_p,dummy_entry,ad_entry));
        }

        /* install the entry */
#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
soc_sand_ll_timer_set("AddRecordsIntoTables", 2);
#endif 
        KBP_TRY(kbp_db_install(tblInfo_p->db_p));
#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
soc_sand_ll_timer_stop(2);
#endif 


        /* Advance the record pointer */
        tblRecordInfo_p++;
        tblInfo_p->rec_count++;

        if( (tblInfo_p->rec_count % 500) == 0 ) {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META("\n\t    - %u records added to table \n"),tblInfo_p->rec_count));
        }

    } /* end of for loop */

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META("\t   Total number of records added [%u] to table\n"), tblInfo_p->rec_count));

    return SOC_SAND_OK;
}

#define SRCH_ENABLE

#ifdef SRCH_ENABLE
/* 2x parallel search of SrcIP(table-0) and DstIP(table-1) tables using LTR 0 */
STATIC int Perform_LTR_Searches(int unit,
                                genericTblMgrAradAppData *refAppData_p,
                                uint32 nof_searches,
                                uint32 tbl_id,
                                uint32 db_type,
                                uint8 ltr_num)
{
    tableInfo   *tblInfo_p[4];
    tableRecordInfo   *tblRecordInfo_p[4];
    alg_kbp_rec *tblRecord_p[4];
    uint32 iter, numRecs;
    uint8  i=0;
    int32_t index1 = -1,priority1 = -1,index2 = -1,priority2 = -1;
    tableRecordInfo *tblRecHoldInfo_p[4]; /* reference for random prefixes */
    struct kbp_entry *entry_in_db1 = NULL,*entry_in_db2 = NULL;
    uint8_t db_key[80] = {0,};
    arad_kbp_rslt exp_result;

    /* Device Manager declarations */
    struct kbp_search_result cmp_rslt;
    ARAD_KBP_LTR_CONFIG *ltr_config_info = NULL;
    uint8_t mkey[80] = { 0, };

    sal_memset(tblInfo_p, 0x0, sizeof(tableInfo*) * 4);
    sal_memset(tblRecordInfo_p, 0x0, sizeof(tableRecordInfo*) * 4);
    sal_memset(tblRecord_p, 0x0, sizeof(alg_kbp_rec*) * 4);
    sal_memset(tblRecHoldInfo_p, 0x0, sizeof(tableRecordInfo*) * 4);

    ltr_config_info = &(Arad_kbp_ltr_config[unit][db_type]);

    for (i=0; i < nof_searches; i++) {

        tblInfo_p[i] = &refAppData_p->g_gtmInfo[tbl_id+i].tblInfo;

        /* expected search results */
        tblRecHoldInfo_p[i] = tblRecordInfo_p[i] = tblInfo_p[i]->tblRecordInfo_p;
    }

    numRecs = tblInfo_p[0]->max_recCount; /* all records count is same */

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n\tPerforming LTR#%d searches\n"), ltr_num));

    iter = 0;

    /* MasterKey[79:0] is constructed from table0/1 records */
    for( iter = 0; iter < numRecs; iter++ )
    {
        /* NlmCmFile__printf("\n\tStart     - %u keys are searched ", iter); */
        sal_memset(&cmp_rslt,0,sizeof(struct kbp_search_result));
        sal_memset(&exp_result,0,sizeof(arad_kbp_rslt));
        sal_memset(mkey,0,80);

        /* Expected results */
        for(i = 0; i < nof_searches; i++)
        {
            exp_result.m_resultValid[i] = 1;
            exp_result.m_hitOrMiss[i]   = 1;
            exp_result.m_hitDevId[i]    = 0;

            exp_result.m_hitIndex[i]    = 0x0; 
        }

        /* construct the master key 79:0  <16B_0's><32b_sourceIP><32b_destinationIP> */
        switch (ltr_num) {
        case ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF:

            exp_result.m_respType[0] = 0x2;
            exp_result.m_respType[1] = 0x1;

            /* using 64bit AD XPT format, but KBP LUT configuratoin is only for 48 bit. then copy 48bit from bit 16 */
            sal_memcpy(&exp_result.m_AssocData[0][2], &tblRecordInfo_p[0]->assoData[2], 6);

            /* using 32bit AD XPT format, but KBP LUT configuratoin is only for 16 bit. then copy 16bit from bit 16 */
            sal_memcpy(&exp_result.m_AssocData[1][2], &tblRecordInfo_p[1]->assoData[2], 2);

            tblRecord_p[0] = &(tblRecordInfo_p[0]->record);
            sal_memcpy(&mkey[0],tblRecord_p[0]->m_data, 10);

            tblRecord_p[1] = &(tblRecordInfo_p[1]->record);
            sal_memcpy(&mkey[6],tblRecord_p[1]->m_data, 4);
            break;

        case ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF:

            exp_result.m_respType[0] = 0x1;

            /* using 32bit AD XPT format, but KBP LUT configuratoin is only for 16 bit. then copy 16bit from bit 16 */
            sal_memcpy(&exp_result.m_AssocData[0][1], &tblRecordInfo_p[0]->assoData[1], 3);

            tblRecord_p[0] = &(tblRecordInfo_p[0]->record);
            sal_memcpy(&mkey[0],tblRecord_p[0]->m_data, 9);
            break;

        default:
            break;
        }

        /* Search the S/W*/
         {
             i = 0;
             sal_memcpy(db_key,tblRecord_p[i]->m_data,10);
             KBP_TRY(kbp_db_search(refAppData_p->g_gtmInfo[i].tblInfo.db_p,db_key, &entry_in_db1, &index1, &priority1));

             LOG_INFO(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "\n\t- Search result for db = %d,index = %x\n"), i,index1));

             ++i;
             sal_memset(db_key,0,10);
             sal_memcpy(db_key,tblRecord_p[i]->m_data,10);
             KBP_TRY(kbp_db_search(refAppData_p->g_gtmInfo[i].tblInfo.db_p,db_key, &entry_in_db2, &index2, &priority2));

             LOG_INFO(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "\n\t- Search result for db = %d,index = %x\n"), i,index2));
         }

        /*fire searches */
        KBP_STRY(kbp_instruction_search(ltr_config_info->inst_p, mkey, 0, &cmp_rslt));

        for(i = 0; i < nof_searches; i++) {
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Hit/miss of search = %d is %x, index  = %x\n"),i,cmp_rslt.hit_or_miss[i],cmp_rslt.hit_index[i]));
        }

        for(i = 0; i<nof_searches;i++)
        {
            uint16_t j = 0;
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Actual AD for Search = %d\n"),i));
            for(j = 0;j<16;j++)
            {
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "%d_"),cmp_rslt.assoc_data[i][j]));
            }
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n")));
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n Expected AD for Search = %d\n"),i));
            for(j = 0;j<16;j++)
            {
                LOG_INFO(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "%d_"),exp_result.m_AssocData[i][j]));
            }
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n")));
        }

        if( iter && ((iter % 500) == 0 ) )
            LOG_INFO(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "\n\t     - %u keys are searched "), iter));


        /* NlmCmFile__printf("\n\tEnd     - %u keys are searched ", iter); */
    }

    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n\t   Total number of keys searched [%u]\n"), iter));
    return SOC_SAND_OK;
}
#endif

uint32 arad_kbp_test_ip4_rpf_NlmGenericTableManager(
    int unit,
    uint32 num_entries,
    uint32 record_base_tbl[4],
    uint32 ad_val_tbl[4]
    )
{
    uint8  iter_tbl;
    genericTblMgrAradAppData *refAppData_p = AradAppData[unit];

    /* Create records to be inserted into various tables. */
    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "\n Creating the records to add to table\n")));
    CreateRecordsInTables_nosmt(
        refAppData_p,
        num_entries,
        record_base_tbl,
        ad_val_tbl);

    /* Add records to tables now */
    for( iter_tbl = 0; iter_tbl < ARAD_KBP_FRWRD_IP_NOF_TABLES; iter_tbl++ )
    {
        /* Verify this table is initialized before adding records */
        if (NULL != refAppData_p->g_gtmInfo[iter_tbl].tblInfo.db_p) {
           AddRecordsIntoTables( refAppData_p,iter_tbl);
        }
    }

    /* perform searches */
#ifdef  SRCH_ENABLE
    if (NULL != refAppData_p->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0].tblInfo.db_p)
    {
        Perform_LTR_Searches(
           unit,
           refAppData_p,
           2,
           ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0,
           ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF ,
           ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF
        );
    }

    if (NULL != refAppData_p->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].tblInfo.db_p)
    {
        Perform_LTR_Searches(
           unit,
           refAppData_p, 
           1, 
           ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, 
           ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF,
           ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF
        );
    }
#endif
    return 0x0;
}

/* Test ROP packet write and read */
int arad_kbp_init_rop_test(int unit){
    uint32
       res,  
       addr = ARAD_KBP_ROP_TEST_REG_ADDR, nbo_addr;
    uint8 data[10] = ARAD_KBP_ROP_TEST_DATA;
    arad_kbp_rop_write_t wr_data;
    arad_kbp_rop_read_t rd_data;

    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));
    sal_memset(wr_data.mask, 0x0, NLM_DATA_WIDTH_BYTES);
    sal_memset(wr_data.addr_short, 0x0, NLMDEV_REG_ADDR_LEN_IN_BYTES);
    wr_data.writeMode = NLM_ARAD_WRITE_MODE_DATABASE_DM;
    
    /* Set wr_data */
    ARAD_KBP_ROP_REVERSE_DATA(data, wr_data.data, 10);
    
    /* Set rd_data */
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
    rd_data.dataType = NLM_ARAD_READ_MODE_DATA_X;
    
    /* ROP write and ROP read */
    res = arad_kbp_rop_write (unit, &wr_data);
    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_write failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }

    res = arad_kbp_rop_write (unit, &wr_data);
    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_write failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }

    res = arad_kbp_rop_read (unit, &rd_data);
    if (res!=0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_read failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }
    /* Comparing Read data with Written data */
    res = sal_memcmp(rd_data.data+1 ,wr_data.data, 10);
    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop write-read failed. read data does no match expected data\n"), 
                              FUNCTION_NAME()));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/* 
 *  KBP db (SW) management functions
 */
void 
    arad_kbp_sw_init(
       int unit
    )
{
    uint32
        tbl_idx,
        res_idx,
        db_type,
        table_id,
        table_size;    
    uint8 
        valid_ltr;

    /* Initialize DB tables with static configuration */
#ifdef BCM_88660_A0    
    if (SOC_IS_ARADPLUS(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
        if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {/* in this case the RPF table is primary table. */
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1].clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;
        }
        if (ARAD_KBP_ENABLE_IPV6_EXTENDED){/* in this case the RPF table is primary table. */
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1].clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;            
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC].parallel_srches_bmp = 0;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF].parallel_srches_bmp = 0;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF_2PASS].parallel_srches_bmp = 0x3;
        }
        sal_memcpy(&Arad_kbp_table_config_info[unit][0], &Arad_plus_kbp_table_config_info_static[0], sizeof(ARAD_KBP_TABLE_CONFIG)*ARAD_KBP_FRWRD_IP_NOF_TABLES);
        sal_memcpy(&Arad_kbp_ltr_config[unit][0], &Arad_plus_kbp_ltr_config_static[0], sizeof(ARAD_KBP_LTR_CONFIG)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_lut_info[unit][0], &Arad_plus_kbp_gtm_lut_info_static[0], sizeof(arad_kbp_lut_data_t)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_ltr_info[unit][0], &Arad_plus_kbp_gtm_ltr_info_static[0], sizeof(arad_kbp_frwd_ltr_db_t)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_opcode_config_info[unit][0], &Arad_plus_kbp_gtm_opcode_config_info_static[0], sizeof(ARAD_KBP_GTM_OPCODE_CONFIG_INFO)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
    }
    else
#endif /* BCM_88660 */
    {
        sal_memcpy(&Arad_kbp_table_config_info[unit][0], &Arad_kbp_table_config_info_static[0], sizeof(ARAD_KBP_TABLE_CONFIG)*ARAD_KBP_FRWRD_IP_NOF_TABLES);
        sal_memcpy(&Arad_kbp_ltr_config[unit][0], &Arad_kbp_ltr_config_static[0], sizeof(ARAD_KBP_LTR_CONFIG)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_lut_info[unit][0], &Arad_kbp_gtm_lut_info_static[0], sizeof(arad_kbp_lut_data_t)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_ltr_info[unit][0], &Arad_kbp_gtm_ltr_info_static[0], sizeof(arad_kbp_frwd_ltr_db_t)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
        sal_memcpy(&Arad_kbp_gtm_opcode_config_info[unit][0], &Arad_kbp_gtm_opcode_config_info_static[0], sizeof(ARAD_KBP_GTM_OPCODE_CONFIG_INFO)*ARAD_KBP_FRWRD_DB_NOF_TYPES);
    }

    /* First set table sizes according to configuration */
    for ( tbl_idx = 0; 
          tbl_idx < ARAD_KBP_FRWRD_IP_NOF_TABLES; 
          tbl_idx++ ) 
    {
        table_size = SOC_DPP_CONFIG(unit)->arad->init.elk.fwd_table_size[tbl_idx];
        if (table_size > 0x0) 
        {
            Arad_kbp_table_config_info[unit][tbl_idx].valid = TRUE;
            Arad_kbp_table_config_info[unit][tbl_idx].tbl_size = table_size;
#ifdef ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF
#else /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */
            /* Disable the IPv6 RPF table */
            if(tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1 && (!ARAD_KBP_ENABLE_IPV6_EXTENDED)) {
                Arad_kbp_table_config_info[unit][tbl_idx].tbl_size = 0;
            }
#endif /* ARAD_KBP_IPV6_INSERT_SIP_IN_MASTER_KEY_FOR_VRF */

            /* Disable the IPv4 RPF table for some combination of SOC property */
            if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1) {
                if (soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0) 
                    && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0) == 0)) {
                    Arad_kbp_table_config_info[unit][tbl_idx].tbl_size = 0;
                }
            }
        }   
    }

    for ( tbl_idx = ARAD_KBP_ACL_TABLE_ID_OFFSET; 
          tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; 
          tbl_idx++) 
    {
        /* Clear Tables (KBP-DBs) */
        ARAD_KBP_TABLE_CONFIG_clear(&Arad_kbp_table_config_info[unit][tbl_idx]);
    }

    /* Clear all LTRs */
    for ( db_type = ARAD_KBP_FRWRD_DB_ACL_OFFSET; 
          db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; 
          db_type++) 
    {
        /* Clear LTR (KBP-instruction), LUT and ROP configurations */
        sal_memset(&Arad_kbp_gtm_lut_info[unit][db_type], 0x0, sizeof(arad_kbp_lut_data_t));
        arad_kbp_frwd_ltr_db_clear(&Arad_kbp_gtm_ltr_info[unit][db_type]);
        ARAD_KBP_LTR_CONFIG_clear(&Arad_kbp_ltr_config[unit][db_type]);
        ARAD_KBP_GTM_OPCODE_CONFIG_INFO_clear(&Arad_kbp_gtm_opcode_config_info[unit][db_type]);
    }

    /* Now verify all tables used by each LTR are valid */
    for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) 
    {
        /* Verify LTR has tables configured */
        valid_ltr = (Arad_kbp_ltr_config[unit][db_type].parallel_srches_bmp > 0) ? TRUE : FALSE;

        for ( tbl_idx = 0; tbl_idx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; tbl_idx++) 
        {
            if (SHR_BITGET(&Arad_kbp_ltr_config[unit][db_type].parallel_srches_bmp, tbl_idx)) {
                /* If one of the tables is not valid, then LTR is not valid */
                table_id = Arad_kbp_ltr_config[unit][db_type].tbl_id[tbl_idx];
                valid_ltr &= Arad_kbp_table_config_info[unit][table_id].valid;
            }
        }

        Arad_kbp_ltr_config[unit][db_type].valid = valid_ltr;
    }

    sal_memset(arad_kbp_frwd_ltr_db, 0x0, sizeof(arad_kbp_frwd_ltr_db_t) * ARAD_KBP_ROP_LTR_NUM_MAX);

    for ( db_type = 0; 
          db_type < ARAD_KBP_ROP_LTR_NUM_MAX; 
          db_type++) 
    {
        /* Clear LTR (KBP-instruction), LUT and ROP configurations */
        for ( res_idx = 0; 
              res_idx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; 
              res_idx++) 
        {
            arad_kbp_frwd_ltr_db[db_type].res_format[res_idx] = NLM_ARAD_NO_INDEX_NO_AD;
        }
    }
    

}

STATIC
uint32 
    arad_kbp_db_create_verify(
       SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG   *sw_table
   )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    SOC_SAND_CHECK_NULL_INPUT(sw_table);

    /* Verify table attributes */
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->bankNum, NLMDEV_BANK_0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->group_id_start, 0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(sw_table->group_id_end, 0, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 30, exit);

    /* Table width must be one of the following */
    if(sw_table->tbl_width != NLM_TBL_WIDTH_80 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_160 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_320 &&
       sw_table->tbl_width != NLM_TBL_WIDTH_640 )
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 40, exit);
    }

    /* Associated width must be one of the following */
    if(sw_table->tbl_asso_width != NLM_TBL_ADLEN_ZERO &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_32B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_64B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_128B &&
       sw_table->tbl_asso_width != NLM_TBL_ADLEN_256B )
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 50, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_create_verify()", 0, 0);
}

STATIC
uint32 
    arad_kbp_db_create(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG       *sw_table,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   )
{
    uint32 
        res = SOC_SAND_OK;

    uint8 
        check_only;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(sw_table);

    check_only = (flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) ? TRUE : FALSE;
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    
    /* Verify table attributes */
    res = arad_kbp_db_create_verify(sw_table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Search for the next valid table */
    if(Arad_kbp_table_config_info[unit][table_id].valid == FALSE)
    {
        if (!check_only) 
        {
            sal_memcpy(&Arad_kbp_table_config_info[unit][table_id], 
                       sw_table, 
                       sizeof(ARAD_KBP_TABLE_CONFIG));

            Arad_kbp_table_config_info[unit][table_id].valid = TRUE;
            Arad_kbp_table_config_info[unit][table_id].tbl_id = table_id;
        }
        *success = SOC_SAND_SUCCESS;
    }
    else
    {
        /* Table already configured */
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 30, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_create()", 0, 0);
}

STATIC
uint32 
    arad_kbp_ltr_lookup_add_verify(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  ARAD_KBP_DB_TYPE            db_type,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH  *search_format
   )
{
    uint32 
        field_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_OUT_OF_RANGE(ltr_id, ARAD_KBP_LTR_ID_FIRST, ARAD_KBP_MAX_ACL_LTR_ID - 1, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);    
    SOC_SAND_ERR_IF_OUT_OF_RANGE(table_id, ARAD_KBP_ACL_TABLE_ID_OFFSET, ARAD_KBP_MAX_NUM_OF_TABLES - 1, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit); 
    /* Verify the table is valid (already created) iff not check only */
    if (((flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) && (Arad_kbp_table_config_info[unit][table_id].valid))
        || ((!(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY)) && (!Arad_kbp_table_config_info[unit][table_id].valid)))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DB_DOESNT_EXIST_ERR, 30, exit);
    }

    SOC_SAND_ERR_IF_ABOVE_NOF(db_type, ARAD_KBP_NOF_DB_TYPES, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 40, exit);    
    SOC_SAND_ERR_IF_ABOVE_MAX(search_format->nof_key_segments, ARAD_KBP_MAX_NOF_KEY_SEGMENTS, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 50, exit);

    /* Verify total number of segments in Master key will not exceed max */
    SOC_SAND_ERR_IF_ABOVE_MAX((search_format->nof_key_segments + Arad_kbp_ltr_config[unit][db_type].master_key_fields.nof_key_segments), 
                              ARAD_KBP_MAX_NOF_KEY_SEGMENTS, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 60, exit);

    /* Verify search format */
    for (field_idx = 0; field_idx < search_format->nof_key_segments; field_idx++) 
    {
        SOC_SAND_ERR_IF_ABOVE_MAX(search_format->key_segment[field_idx].nof_bytes, ARAD_KBP_MAX_SEGMENT_LENGTH_BYTES, 
                                  ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 70, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_lookup_add_verify()", 0, 0);
}

STATIC
uint32 
    arad_kbp_ltr_lookup_add(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  ARAD_KBP_DB_TYPE            db_type,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH  *search_format,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   )
{
    uint32 
        ltr_idx,
        nof_segments,
        res = SOC_SAND_OK;
    uint8
        check_only;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(success);

    check_only = (flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY) ? TRUE : FALSE;
    *success = SOC_SAND_FAILURE_INTERNAL_ERR;

    res = arad_kbp_ltr_lookup_add_verify(
            unit,
            ltr_id, 
            flags,
            table_id, 
            db_type, 
            search_format
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* The LTR ID is correlated with the LTR location index */
    if (ltr_id < ARAD_KBP_ACL_LTR_ID_OFFSET) {
        for(ltr_idx = ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC; ltr_idx < ARAD_KBP_FRWRD_DB_NOF_TYPES; ltr_idx++) {
            if (Arad_kbp_db_type_to_ltr[ltr_idx] == ltr_id) {
                break;
            }
        }
    }
    else {
        ltr_idx = ltr_id - ARAD_KBP_ACL_LTR_ID_OFFSET + ARAD_KBP_FRWRD_DB_NOF_TYPES; 
    }

    /* Verify that there is an available lookup/search in LTR.
     * (Each LTR supports up to 4 parallel DB lookups).
     */
    if (SHR_BITGET(&Arad_kbp_ltr_config[unit][ltr_idx].parallel_srches_bmp, search_id)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
    }

    /* Verify that no other ACL Database is configured for this LTR.
     * (Currently only one ACL per LTR is supported, in addition to 
     * Forwarding tables).
     */
    if (Arad_kbp_ltr_config[unit][ltr_idx].search_type[search_id] == ARAD_KBP_DB_TYPE_ACL) {
        SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_DATABASE_ALREADY_EXISTS_ERR, 30, exit);
    }

    if (!check_only) 
    {
        /* In case verification passed and configuration is allowed, then add new
         * db lookup to this LTR, when the search index is the next one available.
         */
        SHR_BITSET(&Arad_kbp_ltr_config[unit][ltr_idx].parallel_srches_bmp, search_id);
        Arad_kbp_ltr_config[unit][ltr_idx].tbl_id[search_id] = table_id;
        Arad_kbp_ltr_config[unit][ltr_idx].search_type[search_id] = db_type;

        /* Search Format was already verified */
        sal_memcpy(&Arad_kbp_ltr_config[unit][ltr_idx].ltr_search[search_id],
                   search_format, 
                   sizeof(ARAD_KBP_LTR_SINGLE_SEARCH));

        /* New search fields need to be added to master key as well */
        nof_segments = Arad_kbp_ltr_config[unit][ltr_idx].master_key_fields.nof_key_segments;

        sal_memcpy(&Arad_kbp_ltr_config[unit][ltr_idx].master_key_fields.key_segment[nof_segments],
                   &search_format->key_segment[0], 
                   sizeof(ARAD_KBP_KEY_SEGMENT) * search_format->nof_key_segments);
       
        Arad_kbp_ltr_config[unit][ltr_idx].master_key_fields.nof_key_segments += 
            search_format->nof_key_segments;

        /* Validate LTR (instruction) */
        Arad_kbp_ltr_config[unit][ltr_idx].valid = TRUE;
        Arad_kbp_ltr_config[unit][ltr_idx].opcode = opcode;
        Arad_kbp_ltr_config[unit][ltr_idx].ltr_id = ltr_id;
    }

    *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_lookup_add()", 0, 0);
}

STATIC
uint32 
    arad_kbp_opcode_to_db_type(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_OUT uint32                      *db_type,
       SOC_SAND_OUT uint32                      *ltr_id,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   )
{
    uint32 
        db_type_idx,
        found_idx = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(ltr_id);
    SOC_SAND_CHECK_NULL_INPUT(success);

    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;

    /* Check if opcode has tables already */
    for (db_type_idx = 0; (db_type_idx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) && (SOC_SAND_SUCCESS != *success); db_type_idx++) 
    {
        if (Arad_kbp_ltr_config[unit][db_type_idx].valid) 
        {
            if (Arad_kbp_ltr_config[unit][db_type_idx].opcode == opcode) 
            {
                found_idx = db_type_idx;
                *success = SOC_SAND_SUCCESS;
                break;
            }
        }

    }
    
    /* If no LTR was matched, find an unused LTR for the given opcode */
    if (*success != SOC_SAND_SUCCESS) 
    {
        for (db_type_idx = ARAD_KBP_FRWRD_DB_NOF_TYPES; (db_type_idx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) && (SOC_SAND_SUCCESS != *success); db_type_idx++) 
        {
            if(Arad_kbp_ltr_config[unit][db_type_idx].valid == FALSE)
            {
                found_idx = db_type_idx;
                *success = SOC_SAND_SUCCESS;
            }
        }
    }
   
    /* If a matching or available LTR was found, update return values */
    if (*success == SOC_SAND_SUCCESS) 
    {
        if (found_idx < ARAD_KBP_FRWRD_DB_NOF_TYPES) {
            *ltr_id = Arad_kbp_db_type_to_ltr[found_idx];
        }
        else {
            *ltr_id = ARAD_KBP_ACL_LTR_ID_OFFSET + found_idx - ARAD_KBP_FRWRD_DB_NOF_TYPES;
        }

        *db_type = found_idx;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_opcode_to_db_type()", 0, 0);
}

STATIC
uint32 
    arad_kbp_add_ltr_to_opcode_verify(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bits,
       SOC_SAND_IN  uint32                      search_id
   )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_NOF(db_type, ARAD_KBP_MAX_NUM_OF_FRWRD_DBS, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);    
    SOC_SAND_ERR_IF_ABOVE_NOF(opcode, SOC_DPP_DEFS_GET(unit, nof_flp_programs), ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);    
    SOC_SAND_ERR_IF_OUT_OF_RANGE(ltr_id, ARAD_KBP_LTR_ID_FIRST, ARAD_KBP_MAX_ACL_LTR_ID - 1, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 30, exit);    
    SOC_SAND_ERR_IF_ABOVE_NOF(key_size_in_bits, ARAD_KBP_MASTER_KEY_MAX_LENGTH, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 40, exit);
    SOC_SAND_ERR_IF_ABOVE_NOF(search_id, ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_to_opcode_verify()", 0, 0);
}


STATIC
uint32 
    arad_kbp_add_ltr_search_to_opcode(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bytes,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   )
{
    uint32
        result_idx;

    arad_kbp_lut_data_t 
        *lut_info;
    arad_kbp_frwd_ltr_db_t
        *ltr_db_info;
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO 
        *opcode_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(success);

    /* Configure LUT:
     * Includes encoding of LTR, record size and type, result size and type.
     */
    lut_info = &Arad_kbp_gtm_lut_info[unit][db_type];

    lut_info->rec_size += key_size_in_bytes;
    lut_info->rec_type = ARAD_KBP_LUT_REC_TYPE_REQUEST;
    lut_info->rec_is_valid = 1;
    lut_info->mode = ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE;
    lut_info->key_config = ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA;
    if (Arad_kbp_ltr_config[unit][db_type].is_cmp3_search) {
        lut_info->instr = ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ltr_id);
    }
    else {
        lut_info->instr = ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ltr_id);
    }

    ltr_db_info = &Arad_kbp_gtm_ltr_info[unit][db_type];
    ltr_db_info->opcode = opcode;

    switch (search_id) {
        case 0:
            lut_info->result0_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_6B;
            lut_info->result0_idx_or_ad = ARAD_KBP_LUT_TRANSFER_AD_ONLY;

            ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_6B;
            ltr_db_info->res_format[search_id] = NLM_ARAD_INDEX_AND_64B_AD;

            break;
        case 1:
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)
				       && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
                lut_info->result1_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_4B;
                ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_4B;
            }
            else 
#endif /* BCM_88660_A0 */
            {
                lut_info->result1_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_2B;
                ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_2B;
            }
            lut_info->result1_idx_or_ad = ARAD_KBP_LUT_TRANSFER_AD_ONLY;    
            ltr_db_info->res_format[search_id] = NLM_ARAD_INDEX_AND_32B_AD;

        break;
        case 2:
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)
				       && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
                lut_info->result2_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_2B;
                ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_2B;
            }
            else 
#endif /* BCM_88660_A0 */
            {
                lut_info->result2_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_4B;
                ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_4B;
            }
            lut_info->result2_idx_or_ad = ARAD_KBP_LUT_TRANSFER_AD_ONLY;
            ltr_db_info->res_format[search_id] = NLM_ARAD_INDEX_AND_32B_AD;

            break;
        case 3:
            lut_info->result3_idx_ad_cfg = ARAD_KBP_LUT_AD_TRANSFER_3B;
            lut_info->result3_idx_or_ad = ARAD_KBP_LUT_TRANSFER_AD_ONLY;

            ltr_db_info->res_data_len[search_id] = ARAD_KBP_LUT_AD_TRANSFER_3B;
            ltr_db_info->res_format[search_id] = NLM_ARAD_INDEX_AND_32B_AD;

            break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
        }

    /* Configure LTR Database Info:
     * Includes opcode, result encoding and length.
     */
    ltr_db_info->res_total_data_len = 1;
    for (result_idx = 0;
         result_idx < NLM_ARAD_MAX_NUM_RESULTS_PER_INST; 
         result_idx++ ) 
    {
            if (ltr_db_info->res_data_len[result_idx] != NLM_ARAD_NO_INDEX_NO_AD) 
            {
                ltr_db_info->res_total_data_len += ltr_db_info->res_data_len[result_idx];
            }
        }

    /* Configure opcode Info:
     * Includes ROP TX and RX definitions per opcode.
     */
    opcode_info = &Arad_kbp_gtm_opcode_config_info[unit][db_type];

    opcode_info->tx_data_size = lut_info->rec_size - 1; /* subtract 1 from full size */
    opcode_info->tx_data_type = 1; /* REQUEST */
    /* Add (6,2,4,3) bytes to total result according to result index */
    opcode_info->rx_data_size += (search_id == 0) ? 6 : ((search_id == 1) ? 2 : ((search_id == 2) ? 4 : 3)); 
    opcode_info->rx_data_type = 3; /* REPLY */
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_search_to_opcode()", 0, 0);
}

STATIC
uint32 
    arad_kbp_add_ltr_to_opcode(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_IN  uint32                      ltr_id,
       SOC_SAND_IN  uint32                      key_size_in_bytes,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH  *search_format,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   )
{
    uint32
        srch_idx,
        ltr_key_size_in_bytes,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* First Verify */
    res = arad_kbp_add_ltr_to_opcode_verify(
            unit,
            db_type,
            opcode,
            ltr_id,
            (key_size_in_bytes * 8),
            search_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(!(flags & ARAD_KBP_TABLE_ALLOC_CHECK_ONLY))
    {
        for(srch_idx = 0; srch_idx <= search_id; srch_idx++)
        {
            if ((srch_idx != search_id) && 
                SHR_BITGET(&Arad_kbp_ltr_config[unit][db_type].parallel_srches_bmp, srch_idx)) {
                continue;
            }

            ltr_key_size_in_bytes = (srch_idx == search_id) ? key_size_in_bytes : 0;

            /* Add LTR to Opcode */
            res = arad_kbp_add_ltr_search_to_opcode(
                    unit,
                    db_type,
                    opcode,
                    ltr_id,
                    ltr_key_size_in_bytes,
                    srch_idx,
                    success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            if (srch_idx != search_id)
            {
                /* After setting some LTR search index, make sure to
                 * configure bitmap even if it's DUMMY table, so that 
                 * if will be configured in table_init and search_init 
                 *  
                 * Define the Key segment to be a dummy field after the master key 
                 */
                Arad_kbp_table_config_info[unit][ARAD_KBP_TABLE_INDX_TO_DUMMY_TABLE_ID(srch_idx)].valid = TRUE;
                
                SHR_BITSET(&Arad_kbp_ltr_config[unit][db_type].parallel_srches_bmp, srch_idx);
                Arad_kbp_ltr_config[unit][db_type].search_type[srch_idx] = ARAD_KBP_DB_TYPE_FORWARDING;
                Arad_kbp_ltr_config[unit][db_type].ltr_search[srch_idx].nof_key_segments = 1;
                Arad_kbp_ltr_config[unit][db_type].ltr_search[srch_idx].key_segment[0] = search_format->key_segment[0];
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_ltr_to_opcode()", 0, 0);
}

/* 
 * For ACL, the key-size gives the table size.
 * Then, all the computations are done according to the table 
 * size, assuming the whole table size is the logical key 
 */
uint32 
    arad_kbp_add_acl(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  uint32                      pgm_bmp_used,
       SOC_SAND_IN  uint32                      pgm_ndx_min,
       SOC_SAND_IN  uint32                      pgm_ndx_max,
       SOC_SAND_IN  uint32                      key_size_in_bytes,   
       SOC_SAND_IN  uint32                      flags, 
       SOC_SAND_IN  uint32                      min_priority,   
       SOC_SAND_OUT uint32                      *pgm_bmp_new,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    )
{
    uint32 
        res,
        db_type,
        pgm_ndx,
        ltr_id,
        segment_idx;
    ARAD_KBP_TABLE_CONFIG 
        kbp_table_config;
    ARAD_KBP_LTR_SINGLE_SEARCH 
        search_format;
    ARAD_KBP_FRWRD_IP_OPCODE
        opcode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Initialize to invalid Table ID */
    kbp_table_config.tbl_id = ARAD_KBP_MAX_NUM_OF_TABLES; 

    /* Set table width according to key-calc result */
    if (key_size_in_bytes <= 10) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_80; 
    } 
    else if (key_size_in_bytes <= 20) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_160; 
    } 
    else if (key_size_in_bytes <= 40) {
        kbp_table_config.tbl_width = NLM_TBL_WIDTH_320; 
    } 
    else {
        /* Undefined table width */
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3, exit);
    }

    kbp_table_config.tbl_asso_width = NLM_TBL_ADLEN_32B; 
    kbp_table_config.bankNum = NLMDEV_BANK_0;
    kbp_table_config.group_id_start = 0;
    kbp_table_config.group_id_end = 0;
    kbp_table_config.min_priority = min_priority;
    kbp_table_config.clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;

    /* Configurable table size acocrding to SOC property */
    switch (kbp_table_config.tbl_width) {
    case NLM_TBL_WIDTH_80:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL80_TABLE_SIZE, (64*1024)); 
        break;
    case NLM_TBL_WIDTH_160:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL160_TABLE_SIZE, (64*1024)); 
        break;
    case NLM_TBL_WIDTH_320:
        kbp_table_config.tbl_size = soc_property_get(unit, spn_EXT_ACL320_TABLE_SIZE, (64*1024)); 
        break;
    default:
        /* Undefined table width */
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }

    /* Initialize search format */

    /* The key segments will be in 80 bit chunks.
     * Currently there is no reason to break the ACL to its qualifiers 
     * since ACL in ELK uses only key C and only one ACL is currently 
     * supported (lookup key can be taken as a whole instead of separate 
     * pieces of the master key).
     * The reason to break down to segments is the max key field size 
     * in ELK is 16 bytes.  
     */
    memset(&search_format, 0, sizeof(search_format));
    search_format.nof_key_segments = (key_size_in_bytes + 9) / 10;

    for (segment_idx = 0; segment_idx < search_format.nof_key_segments; segment_idx++) 
    {
      /* the last segment should be the remaining
       * of the bytes, else a full 10 byte segment 
       * For the last segment the nof_bytes should be between 1B and 10B.
       */
      search_format.key_segment[segment_idx].nof_bytes = 
        (segment_idx == (search_format.nof_key_segments-1)) ? (((key_size_in_bytes - 1) % 10) + 1) : 10;

      sal_sprintf(search_format.key_segment[segment_idx].name, "ACL%d-s%d", table_id, segment_idx);
      search_format.key_segment[segment_idx].type = KBP_KEY_FIELD_TERNARY;
    }

    /* KBP config SW is configured in two phases:
     * Phase 1 -> check physibility 
     * Phase 2 -> allocate resources (databases and instructions) 
     *  
     * Actual configuration of KBP device is done at a later time 
     * according to this configation.
     */

    res = arad_kbp_db_create(
              unit,
              table_id,
              &kbp_table_config, 
              flags,
              success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

    if(SOC_SAND_SUCCESS != *success) {
        /* Break configuration loop */
        ARAD_DO_NOTHING_AND_EXIT;
    }

    for (pgm_ndx = pgm_ndx_min; pgm_ndx < pgm_ndx_max; ++pgm_ndx)
    {
        if (SOC_SAND_GET_BIT(pgm_bmp_used, pgm_ndx) != 0x1){
            /* PMF Program not used */
            continue;
        }

        /* Get DB type and LTR ID from program or allocate
         * new program if necessary
         */
        opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(pgm_ndx);
        res = arad_kbp_opcode_to_db_type(
                  unit,
                  opcode,
                  &db_type,
                  &ltr_id,
                  success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        /* Add table to KBP instruction (LTR search) */
        res = arad_kbp_ltr_lookup_add(
                   unit,
                   search_id,
                   opcode,
                   ltr_id,
                   flags,
                   table_id,
                   ARAD_KBP_DB_TYPE_ACL, /* database type */
                   &search_format,
                   success
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        /* Add LTR to Opcode */
        res = arad_kbp_add_ltr_to_opcode(
                unit,
                db_type,
                flags,
                opcode,
                ltr_id,
                key_size_in_bytes,
                search_id,
                &search_format,
                success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        if(SOC_SAND_SUCCESS != *success) {
            ARAD_DO_NOTHING_AND_EXIT;
        }

        /* update the program if configuration was successfull */
        SHR_BITSET(pgm_bmp_new, pgm_ndx);
    }
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_add_acl()", 0, 0);
}

STATIC
  void 
    arad_kbp_sw_config_print(
       SOC_SAND_IN  int  unit
    )
{
    uint32 
        idx;

    if(NULL == AradAppData[unit])
    {
        LOG_CLI((BSL_META_U(unit,
                            "\n***** KBP Device is not yet installed with the following configuration!\n\n")));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nTable Configuration\n\n")));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s"),"Tbl-ID"));
    LOG_CLI((BSL_META_U(unit,
                        "%-15s"),"Tbl-Name"));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s"),"Size"));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s"),"Width"));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s"),"AD Width"));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s"),"Num ent."));
    LOG_CLI((BSL_META_U(unit,
                        "%-10s\n"),"~Capacity"));
    LOG_CLI((BSL_META_U(unit,
                        "-------------------------------------------------------\n")));

    for ( idx = 0; 
          idx < ARAD_KBP_MAX_NUM_OF_TABLES; 
          idx++) 
    {
        if (Arad_kbp_table_config_info[unit][idx].valid) 
        {
            ARAD_KBP_TABLE_CONFIG_print(unit, &Arad_kbp_table_config_info[unit][idx]);
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\nApplication Configuration\n")));

    for ( idx = 0; 
          idx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; 
          idx++) 
    {
        if (Arad_kbp_ltr_config[unit][idx].valid) 
        {
            LOG_CLI((BSL_META_U(unit,
                                "----------------------------------------------\n")));
            LOG_CLI((BSL_META_U(unit,
                                "LTR Index: %d\n"), idx));
            ARAD_KBP_LTR_CONFIG_print(&Arad_kbp_ltr_config[unit][idx]);
            arad_kbp_frwd_ltr_db_print(&Arad_kbp_gtm_ltr_info[unit][idx]);
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }
}

STATIC
  void
    arad_kbp_device_print(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  char*   file_name
    )
{
    FILE *kbp_file;

    if (AradAppData[unit]->device_p == NULL ||
        file_name == NULL) 
    {
        return;
    }

    kbp_file = fopen(file_name,"w");

    if (kbp_file != NULL) {
        kbp_device_print_html(AradAppData[unit]->device_p, kbp_file);
        fclose(kbp_file);
    }
}

void 
    arad_kbp_sw_print(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  print_level,
       SOC_SAND_IN  char*   file_name
    )
{
    switch(print_level) 
    {
    case 0:
        arad_kbp_sw_config_print(unit);
        break;
    case 1: 
        arad_kbp_device_print(unit, file_name);
        break;
    default:
        break;
    }
}

/* 
 *  Table Get functions
 */
uint32 arad_kbp_alg_kbp_db_get(
   int unit,
   uint32 frwrd_tbl_id,
   struct kbp_db **db_p)
{
    uint32 res = SOC_SAND_OK;
    *db_p = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo.db_p;
    return res;
}

uint32 arad_kbp_alg_kbp_ad_db_get(
   int unit,
   uint32 frwrd_tbl_id,
   struct kbp_ad_db **ad_db_p)
{
    uint32 
        res = SOC_SAND_OK;
    *ad_db_p = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo.ad_db_p;
    return res;
}

uint32 
    arad_kbp_alg_kbp_get_inst_pointer(
        SOC_SAND_IN  int                     unit,
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_LTR      ltr_id,
        SOC_SAND_OUT struct  kbp_instruction    **inst_p
    )
{
    uint32 db_type;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Verify LTR ID has a valid value */
    SOC_SAND_ERR_IF_OUT_OF_RANGE(ltr_id, ARAD_KBP_LTR_ID_FIRST, ARAD_KBP_MAX_ACL_LTR_ID - 1, ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);    

    db_type = ltr_id - ARAD_KBP_LTR_ID_FIRST;

    if((Arad_kbp_ltr_config[unit][db_type].valid) &&
       (Arad_kbp_ltr_config[unit][db_type].ltr_id == ltr_id))
    {
        *inst_p = Arad_kbp_ltr_config[unit][db_type].inst_p;
    }
    else
    {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_alg_kbp_get_inst_pointer()", ltr_id, 0);
}

uint32 
    arad_kbp_gtm_table_info_get(
       int unit,
       uint32 frwrd_tbl_id,
       tableInfo *tblInfo_p
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(tblInfo_p);

    if (AradAppData[unit] == NULL) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    else {
        sal_memcpy(tblInfo_p, 
                   &(AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo), 
                   sizeof(tableInfo));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_gtm_table_info_get()", frwrd_tbl_id, 0);
}


uint32 
    arad_kbp_get_device_pointer(
        SOC_SAND_IN  int                unit,
        SOC_SAND_OUT struct kbp_device  **device_p
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(device_p);

    if(AradAppData[unit]->device_p != NULL) {
        *device_p = AradAppData[unit]->device_p;
    }
    else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_get_device_pointer()", 0, 0);
}

uint32 arad_kbp_gtm_ltr_num_get(
   int unit,
   uint32 frwrd_tbl_id,
   uint8 *ltr_num)
{
    uint32 res = SOC_SAND_OK;
    
    *ltr_num = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].ltr_num;

    return res;
}


STATIC
int32_t arad_kbp_callback_mdio_read(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value){
    uint32 kbp_reg_addr;
    kbp_init_user_data_t *user_data = handle;
    int rv;
    
    if(handle == NULL){
        return KBP_INVALID_ARGUMENT;
    }
    kbp_reg_addr = reg + (dev << 16);
    rv = soc_dcmn_miim_read(user_data->device, ARAD_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id, kbp_reg_addr, value);
    if(SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META("%s(): soc_dcmn_miim_read() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;
}

STATIC
int32_t arad_kbp_callback_mdio_write(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value){
    uint32 kbp_reg_addr;
    kbp_init_user_data_t *user_data = handle;
    int rv;
    
    if(handle == NULL){
        return KBP_INVALID_ARGUMENT;
    }
    kbp_reg_addr = reg + (dev << 16);
    rv = soc_dcmn_miim_write(user_data->device, ARAD_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id, kbp_reg_addr, value);
    if(SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META("%s(): soc_dcmn_miim_read() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;   
}


STATIC
int32_t arad_kbp_usleep_callback(void *handle, uint32_t usec){
    sal_usleep(usec);
    return KBP_OK;
}

/*
 *    Initialization functions functions
 */
uint32 arad_kbp_init_kbp_ilkn_port_set(
    int unit,
    uint32 kbp_mdio_id,
    int ilkn_num_lanes, 
    int ilkn_rate, 
    int ilkn_meta_data,
    uint32 kbp_ilkn_rev,
    kbp_reset_f kbp_reset)
{
    int rc;
    struct kbp_device_config kbp_config = KBP_DEVICE_CONFIG_DEFAULT;
    kbp_init_user_data_t handle;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* KBP mdio ID format (kbp_mdio_id):
     * bits [5:6,8:9] - bus ID. Arad has 8 external buses (0-7).
     * bit [7] - Internal select. Set to 0 for external phy access.
     * bits [0:4] - phy/kbp id 
     *  
     * Adress format (according to kbp register spec): 
     * Mdio address =  [15:0] 
     * Mdio Device ID = [23:16] 
     */

   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Start. kbp_mdio_id=0x%x, kbp_ilkn_rev=%d.\n"), FUNCTION_NAME(), kbp_mdio_id, kbp_ilkn_rev));

    switch(ilkn_rate){
    case 6250:
        kbp_config.speed = KBP_INIT_LANE_6_25;
        break;
    case 10312:
        kbp_config.speed = KBP_INIT_LANE_10_3;
        break;
    case 12500:
        kbp_config.speed = KBP_INIT_LANE_12_5;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): Error, unsupported ilkn_rate=%d.\n"), FUNCTION_NAME(), ilkn_rate));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 70, exit);  
    }
    if((ilkn_num_lanes == 4) || (ilkn_num_lanes == 8) || (ilkn_num_lanes == 12)){
        kbp_config.port_map[0].num_lanes = ilkn_num_lanes;
    }
    else{
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): Unsupported ilkn_num_lanes=%d.\n"), FUNCTION_NAME(), ilkn_num_lanes));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 80, exit);
    }
    kbp_config.burst = KBP_INIT_BURST_SHORT_16_BYTES;
    /*metframe length set*/
    if (ilkn_meta_data == 64 || ilkn_meta_data == 2048) {
        kbp_config.meta_frame_len = ilkn_meta_data;
    }
    else{
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): Unsupported ilkn_meta_data=%d.\n"), FUNCTION_NAME(), ilkn_meta_data));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 90, exit);
    }
    handle.device = unit;
    handle.kbp_mdio_id = kbp_mdio_id;
    kbp_config.handle = &handle;

    /*callbacks set*/
    kbp_config.mdio_read = arad_kbp_callback_mdio_read;
    kbp_config.mdio_write = arad_kbp_callback_mdio_write;
    kbp_config.assert_kbp_resets = kbp_reset;
    kbp_config.usleep = arad_kbp_usleep_callback;
    kbp_config.pre_enable_link_callback = NULL;
    kbp_config.reverse_lanes = kbp_ilkn_rev;

    /*config the kbp*/
    rc = kbp_device_interface_init(KBP_DEVICE_12K, KBP_DEVICE_DEFAULT, &kbp_config);
    if(ARAD_KBP_TO_SOC_RESULT(rc) != SOC_SAND_OK){
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): configuring elk device. %s\n"), FUNCTION_NAME(), kbp_get_status_string(rc)));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 100, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_kbp_ilkn_port_set()", ilkn_num_lanes, ilkn_rate);
}


uint32 arad_kbp_init_egw_config_set(
    int unit,
    int ilkn_num_lanes, 
    int ilkn_rate)
{
    uint32
        res,
        reg_val = 0x0,
        fld_val = 0x0,
        ilkn_total_mbits_rate,
        ilkn_total_burst_rate,
        core_clk_ticks,
        spr_dly_mega,
        spr_dly_fld,
        spr_dly_fraction_fld;
    uint64
        reg64_val = 0x0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Minimum/Maximum packet size in B, up to which packet not closed */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_PACKET_SIZEr(unit, REG_PORT_ANY, &reg_val));
    fld_val = 64;
    soc_reg_field_set(unit, IHB_PACKET_SIZEr, &reg_val, MIN_PKT_SIZEf, fld_val);
    fld_val = 256;
    soc_reg_field_set(unit, IHB_PACKET_SIZEr, &reg_val, MAX_PKT_SIZEf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_PACKET_SIZEr(unit, REG_PORT_ANY, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_TRANSMIT_CFGSr(unit, REG_PORT_ANY, &reg64_val));
    /* EGW will not transmit FLP records if FIFO exceed this threshold */
    fld_val = 0x190;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, TRANSMITING_THRESHOLDf, fld_val);
    /* congestion identified when FIFO exceed this threshold (enables ROP style packing and/or MaxPktSize reaching) */
    fld_val = 0x14;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, PACKING_THRESHOLDf, fld_val);
    /* Enable ROP style packing */
    fld_val = 0x1;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, PACKING_ENABLEf, fld_val);
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)){
        /* set CPU priority over traffic lookups */
        fld_val = 0x1;
        soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, CPU_RECORD_PRIOf, fld_val);
    }
#endif /* BCM_88660_A0 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_TRANSMIT_CFGSr(unit, REG_PORT_ANY, reg64_val));

    /* Enables Attaching application prefix in packet transmition/reciving */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_APP_PREFIX_CONTROLr(unit, REG_PORT_ANY, &reg_val));
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_APP_PREFIX_CONTROLr, &reg_val, TX_APP_PREFIX_ENABLEf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_APP_PREFIX_CONTROLr, &reg_val, RX_APP_PREFIX_ENABLEf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_APP_PREFIX_CONTROLr(unit, REG_PORT_ANY, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_PHY_CHANNEL_CFGSr(unit, REG_PORT_ANY, &reg_val));
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_PHY_CHANNEL_CFGSr, &reg_val, TX_CHANNEL_NUMf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_PHY_CHANNEL_CFGSr, &reg_val, RX_CHANNEL_NUMf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_PHY_CHANNEL_CFGSr(unit, REG_PORT_ANY, reg_val));

    /* 
     * Shaper configuration
     */
    /* ilkn_num_lanes = 12; */
    ilkn_total_mbits_rate = ilkn_num_lanes * ilkn_rate;
    ilkn_total_burst_rate = ilkn_total_mbits_rate / (32 * 8);
    core_clk_ticks = arad_chip_ticks_per_sec_get(unit);
    spr_dly_mega = core_clk_ticks / ilkn_total_burst_rate;

    spr_dly_fld = spr_dly_mega / 1000000; 
    /* Need to find the fraction = ((64 * 1024 * dly_mega) / 1000000).
     * since the fraction is = x/64K. 64/1000000 = 1/15625 
     */ 
    spr_dly_fraction_fld = ((spr_dly_mega % 1000000) * 1024) / 15625;
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Shaper configuration ilkn_total_mbits_rate=%d, ilkn_total_burst_rate=%d, core_clk_ticks=%d\n"), 
                           FUNCTION_NAME(), ilkn_total_mbits_rate, ilkn_total_burst_rate, core_clk_ticks)); 
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Shaper configuration spr_dly_fld=%d, spr_dly_fraction_fld=%d\n"), 
                           FUNCTION_NAME(), spr_dly_fld, spr_dly_fraction_fld));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_SPR_DLY_CFGSr(unit, REG_PORT_ANY, &reg64_val));
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_DLYf, spr_dly_fld);
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_DLY_FRACTIONf, spr_dly_fraction_fld);
    fld_val = 0x1ff;
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_MAX_BURSTf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_SPR_DLY_CFGSr(unit, REG_PORT_ANY, reg64_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_SPR_CFGSr(unit, REG_PORT_ANY, &reg_val));
    fld_val = 0x3;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, WORD_ALIGNMENTf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, PACKET_GAPf, fld_val);
    fld_val = 0x8;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, PACKET_OVERHEADf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_SPR_CFGSr(unit, REG_PORT_ANY, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_INTERLAKEN_CFGSr(unit, REG_PORT_ANY, &reg_val));
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MODEf, fld_val);
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MIN_BURSTf, fld_val);
    fld_val = 0x7;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MAX_BURSTf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_INTERLAKEN_CFGSr(unit, REG_PORT_ANY, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_GENERAL_CFGSr(unit, REG_PORT_ANY, &reg_val));
    /* ***** Enables lookups to ELK ***** */
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, ENABLE_ELK_LOOKUPf, fld_val);
    fld_val = 0x0;
    /* PB leftover should be 0x0*/
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, NIF_TX_INIT_CREDITSf, fld_val);
    /* Error recovery time of ROP packet transmission */
    fld_val = 0x3E8; 
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, TIMEOUT_DLYf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_GENERAL_CFGSr(unit, REG_PORT_ANY, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr(unit, 0, &reg64_val));
    /* ***** Enables lookups to ELK ***** */
    fld_val = 0x1;
    soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg64_val, ELK_ENABLEf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg64_val));

    /* ***** Enables ELK on ilkn port 10/16 ***** */
    fld_val = 0x1; /*ILKN port 16 val*/
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)){
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_ELK_CFG_ON_ILKN_PORT_10r(unit, &reg_val));
        if(SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode){
            soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10f, 1);
            fld_val = 0; /*change the value of ELK on ILKN port 16*/
            switch(ilkn_num_lanes){
            case 4:
                soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_FOUR_LANESf, 1);
                break;
            case 8:
                soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_FOUR_LANESf, 1);
                soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_EIGHT_LANESf, 1);
                break;
            default:
                 LOG_ERROR(BSL_LS_SOC_TCAM,
                           (BSL_META_U(unit,
                                       "Error in %s(): External interface mode unsupported ilkn_num_lanes=%d. (supported num of lanes 4/8)\n"), FUNCTION_NAME(), ilkn_num_lanes));
                 SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 11, exit);
            }
        }
        else{
            soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10f, 0);
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_NBI_ELK_CFG_ON_ILKN_PORT_10r(unit, reg_val));
    }
#endif 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_ELK_CFGr(unit, &reg_val));
    
    soc_reg_field_set(unit, NBI_ELK_CFGr, &reg_val, ELK_ENABLE_ON_ILKN_PORT_16f, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_NBI_ELK_CFGr(unit, reg_val));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_egw_config_set()", ilkn_num_lanes, ilkn_rate);
}

uint32 arad_kbp_init_egw_default_opcode_set(
    int unit)
{
    uint32
        res,
        mem_filed = 0x0;
    uint32
        table_entry[1];
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Configure in the EGW the used OPCODES */
    /* LUT_WR. Opcode ARAD_KBP_CPU_WR_LUT_OPCODE = 255 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    /* Opcode 255 is 10B data (80 bit) */
    mem_filed = 9;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 255 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));

    /* LUT_RD. Opcode ARAD_KBP_CPU_RD_LUT_OPCODE = 254 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    /* Opcode 254 is 1B data (8 bit) */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 254 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    /* Reply is 10B */
    mem_filed = 9;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));

    /* PIOWR. Opcode ARAD_KBP_CPU_PIOWR_OPCODE = 253 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    /* Opcode 253 is 24B data */
    mem_filed = 23;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 253 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));

    /* PIORD-X. Opcode ARAD_KBP_CPU_PIORDX_OPCODE = 252 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    /* Opcode 252 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 252 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));

    /* PIORD-Y. Opcode ARAD_KBP_CPU_PIORDY_OPCODE = 251 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Opcode 251 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 251 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
  
    /* PIORD-Y. Opcode ARAD_KBP_CPU_CTX_BUFF_WRITE_OPCODE = 251 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Opcode 251 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 251 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));

    /* Block Copy. Opcode ARAD_KBP_CPU_BLK_COPY_OPCODE = 249 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    /* Opcode 249 is 8B data */
    mem_filed = 7;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 249 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));

   /* Block move. Opcode ARAD_KBP_CPU_BLK_MOVE_OPCODE = 248 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    /* Opcode 248 is 8B data */
    mem_filed = 7;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 248 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));

   /* Block clear. Opcode ARAD_KBP_CPU_BLK_CLR_OPCODE = 247 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    /* Opcode 247 is 5B data */
    mem_filed = 4;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 247 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));

   /* Block invalidate. Opcode ARAD_KBP_CPU_BLK_CLR_OPCODE = 246 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    /* Opcode 246 is 5B data */
    mem_filed = 4;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 125, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 135, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
        
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_egw_default_opcode_set()", 0x0, 0x0);
}


uint32 arad_kbp_ilkn_interface_param_get( 
    int         unit,
    soc_port_t *ilkn_port,
    uint32     *ilkn_num_lanes,
    int        *ilkn_rate,
    uint32     *ilkn_metaframe)
{
    uint32
        res,
        offset;
    soc_error_t
        rv;
    soc_port_t
        port;
    soc_pbmp_t 
        ports_bm;
    soc_port_if_t 
        interface_type;
    ARAD_PORTS_ILKN_CONFIG 
        *ilkn_config;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Find ILKN1 port number */
    *ilkn_port = -1;
    rv = soc_port_sw_db_valid_ports_get(unit, 0x0, &ports_bm);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }
    SOC_PBMP_ITER(ports_bm, port){
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (SOC_PORT_IF_ILKN == interface_type) {
            rv = soc_port_sw_db_protocol_offset_get(unit, port, &offset);
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
            if(SOC_FAILURE(rv)) {
                SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 4, exit);
            }
            if (0x1 == offset) {
                *ilkn_port = port;
                break;
            }
        }
    }
    if (*ilkn_port == -1) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): No ILKN1 port found.\n"), FUNCTION_NAME()));    
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 5, exit);
    }

    /* get ILKN configuration */
    rv = soc_port_sw_db_num_lanes_get(unit, *ilkn_port, ilkn_num_lanes);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 10, exit);
    }
    rv = soc_port_sw_db_speed_get(unit, *ilkn_port, ilkn_rate);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 20, exit);
    }
    rv = soc_port_sw_db_protocol_offset_get(unit, port, &offset);
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 30, exit);
    }
    *ilkn_metaframe = ilkn_config->metaframe_sync_period;
    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): ilkn_port=%d, ilkn_num_lanes=%d, ilkn_rate=%d, ilkn_metaframe=%d\n"), 
                            FUNCTION_NAME(), *ilkn_port, *ilkn_num_lanes, *ilkn_rate, *ilkn_metaframe));    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_ilkn_interface_param_get()", *ilkn_num_lanes, *ilkn_rate);
}

uint32 arad_kbp_init_kbp_interface( 
    int unit,
    uint32 kbp_mdio_id,
    uint32 kbp_ilkn_rev,
    kbp_reset_f kbp_reset)
{
    uint32
        res,
        ilkn_num_lanes,
        ilkn_metaframe;
    soc_port_t
        ilkn_port;
    int
        ilkn_rate;
   
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_DPP_CONFIG(unit)->arad->init.elk.enable == 0x0) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): ELK disabled (ext_tcam_dev_type might be NONE)!!!\n"), FUNCTION_NAME()));    
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    if(kbp_reset == NULL){
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): Invalid reset function\n"), FUNCTION_NAME()));    
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    res = arad_kbp_ilkn_interface_param_get(unit, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): kbp_mdio_id=0x%x, ilkn_port=%d, ilkn_num_lanes=%d, ilkn_rate=%d, ilkn_metaframe=%d\n"), 
                            FUNCTION_NAME(), kbp_mdio_id, ilkn_port, ilkn_num_lanes, ilkn_rate, ilkn_metaframe));    
    
    /* KBP ILKN config needed */
    /* using the ElkIlknRev mode from soc properties if exists */
    kbp_ilkn_rev = soc_property_get(unit, spn_EXT_ILKN_REVERSE, kbp_ilkn_rev);
    res = arad_kbp_init_kbp_ilkn_port_set(unit, kbp_mdio_id, ilkn_num_lanes, ilkn_rate, ilkn_metaframe, kbp_ilkn_rev, kbp_reset);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_kbp_interface()", ilkn_num_lanes, ilkn_rate);
}

uint32 arad_kbp_blk_lut_set(int unit)
{
    uint32
        res = SOC_SAND_OK;
    arad_kbp_lut_data_t 
        lut_data;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(&lut_data, 0, sizeof(lut_data));

    /* Block copy instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_COPY_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_COPY_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, ARAD_KBP_CPU_BLK_COPY_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Block move instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_MOVE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_MOVE_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, ARAD_KBP_CPU_BLK_MOVE_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Block clear instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_CLR_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_CLR_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, ARAD_KBP_CPU_BLK_CLR_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Block invalidate instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_EV_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_EV_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, ARAD_KBP_CPU_BLK_EV_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_blk_lut_set()", 0, 0);
}

uint32 arad_kbp_init_arad_interface( 
    int unit)
{
    uint32
        res,
        ilkn_num_lanes,
        ilkn_metaframe;
    soc_port_t
        ilkn_port;
    int
        ilkn_rate;
    uint64
        reg_val;
   
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_kbp_ilkn_interface_param_get(unit, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): ilkn_port=%d, ilkn_num_lanes=%d, ilkn_rate=%d, ilkn_metaframe=%d\n"), FUNCTION_NAME(), ilkn_port, ilkn_num_lanes, ilkn_rate, ilkn_metaframe));    
       
    /*mark ELK flag in SW DB*/ 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_port_sw_db_flag_add(unit, ilkn_port, SOC_PORT_FLAGS_ELK)); 

     
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_NBI_TX_1_ILKN_CONTROLr(unit, &reg_val));
    soc_reg64_field_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val, TX_1_BURSTSHORTf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_NBI_TX_1_ILKN_CONTROLr(unit, reg_val));

   /* EGW config needed, might be according to ilkn configuration */
    res = arad_kbp_init_egw_config_set(unit, ilkn_num_lanes, ilkn_rate);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = arad_kbp_init_egw_default_opcode_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_arad_interface()", ilkn_num_lanes, ilkn_rate);
}

/* This function performs Device Manager related Inits */
STATIC int arad_kbp_init_nlm_dev_mgr( 
   int unit)
{
    uint32 
        flags;
    kbp_warmboot_t *warmboot_data;
    int32 res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    warmboot_data = &kbp_warmboot_data[unit];

    flags = KBP_DEVICE_DEFAULT | KBP_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit)) {
        flags |= KBP_DEVICE_SKIP_INIT;
    }
    KBP_TRY(kbp_device_init(AradAppData[unit]->dalloc_p, 
                            KBP_DEVICE_12K, 
                            flags, 
                            AradAppData[unit]->alg_kbp_xpt_p, 
                            NULL,
                            &AradAppData[unit]->device_p));

    if (SOC_WARM_BOOT(unit)) {
       res = kbp_device_restore_state(AradAppData[unit]->device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp);
       if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): failed with error: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
       }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_nlm_dev_mgr()", 0, 0);
}

uint32 
    arad_kbp_init_nlm_app_set(
       SOC_SAND_IN int unit
    )
{
    uint32
        res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Currently Device Manager is flushing out each request immediately. */
    AradAppData[unit]->request_queue_len = 1;
    AradAppData[unit]->result_queue_len  = 1;

    /* Create the default allocator */
    KBP_TRY(default_allocator_create(&AradAppData[unit]->dalloc_p));

    AradAppData[unit]->alloc_p = NlmCmAllocator__ctor(&AradAppData[unit]->alloc_bdy);
    if(!AradAppData[unit]->alloc_p){
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): Allocator creation Failed!\n"), FUNCTION_NAME()));
        exit(0);
    }

    /* Create transport interface */
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Create transport interface.\n"), FUNCTION_NAME()));

    AradAppData[unit]->alg_kbp_xpt_p = arad_kbp_xpt_init(
        unit,
        AradAppData[unit]->alloc_p,            /* General purpose memory allocator */
        AradAppData[unit]->request_queue_len,  /* Max request count */
        AradAppData[unit]->result_queue_len,   /* Max result count*/
        AradAppData[unit]->channel_id);
    if (AradAppData[unit]->alg_kbp_xpt_p == NULL) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): arad_kbp_xpt_init() failed!\n"), FUNCTION_NAME()));
        exit(0);
    }

    /* Initialize Device Manager now */
    res = arad_kbp_init_nlm_dev_mgr(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_nlm_app_set()", 
                                  AradAppData[unit]->request_queue_len, 
                                  AradAppData[unit]->result_queue_len);
}

STATIC 
void
    arad_kbp_deinit_nlm_app_set(
       SOC_SAND_IN int unit
    )
{
   /* destroy kbp_dm , was created in arad_kbp_deinit_nlm_dev_mgr() */
    kbp_device_destroy(AradAppData[unit]->device_p);
    arad_kbp_xpt_destroy(AradAppData[unit]->alg_kbp_xpt_p);
}

STATIC
    int arad_kbp_device_lock_config(
        SOC_SAND_IN  int unit
    )
{
    uint32 res = SOC_SAND_OK;
    KBP_TRY(kbp_device_lock(AradAppData[unit]->device_p));
    return res;
}

STATIC
  uint32 
    arad_kbp_key_init(
        SOC_SAND_IN int                      unit,
        SOC_SAND_IN ARAD_KBP_FRWRD_IP_DB_TYPE   gtm_db_type
    )
{
    uint32
        srch_ndx,
        table_idx,
        key_ndx,
        table_key_offset;

    uint32_t res;

    ARAD_KBP_LTR_CONFIG
        *ltr_config_info = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ltr_config_info = &(Arad_kbp_ltr_config[unit][gtm_db_type]);

    if (!SOC_WARM_BOOT(unit)) {
        /*Master key Creation*/
        KBP_TRY(kbp_key_init(AradAppData[unit]->device_p, &ltr_config_info->master_key));

        for(srch_ndx = 0; srch_ndx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; srch_ndx++)
        {
            if (SHR_BITGET(&ltr_config_info->parallel_srches_bmp, srch_ndx)) {
                table_idx = ltr_config_info->tbl_id[srch_ndx];

                for(key_ndx = 0;
                     key_ndx < ltr_config_info->ltr_search[srch_ndx].nof_key_segments;
                     key_ndx++)
                {
                    /*Create a unique field name*/
                    if(ltr_config_info->ltr_search[srch_ndx].key_segment[key_ndx].nof_bytes){

                        if(!(AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db)){
                            int type = ltr_config_info->ltr_search[srch_ndx].key_segment[key_ndx].type == KBP_KEY_FIELD_HOLE ? KBP_KEY_FIELD_HOLE : KBP_KEY_FIELD_PREFIX;
                            res = kbp_key_add_field(
                                    AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.key,
                                    ltr_config_info->ltr_search[srch_ndx].key_segment[key_ndx].name,
                                    (ltr_config_info->ltr_search[srch_ndx].key_segment[key_ndx].nof_bytes * 8),
                                    type
                                  );

                            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                                if(res != KBP_DUPLICATE_KEY_FIELD) {
                                   LOG_ERROR(BSL_LS_SOC_TCAM,
                                             (BSL_META_U(unit,
                                                         "Error in %s(): DB Key : kbp_key_add_field with failed: %s!\n"), 
                                                         FUNCTION_NAME(),
                                              kbp_get_status_string(res)));

                                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                                }
                            }
                        }
                    }
                }

                if(!(AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db)){
                    res = kbp_db_set_key(
                            AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.db_p,
                            AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.key
                          );
                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                       LOG_ERROR(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "Error in %s(): DB Key : kbp_db_set_key with failed: %s, srch_ndx = %d table_idx = %d gtm_db_type = %d!\n"), 
                                             FUNCTION_NAME(),
                                  kbp_get_status_string(res), srch_ndx, table_idx, gtm_db_type));

                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
                    }

                    AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db = 1;
                    AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.nof_bytes = ltr_config_info->ltr_search[srch_ndx].key_segment[0].nof_bytes;
                    sal_memcpy(
                        AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.name, 
                        ltr_config_info->ltr_search[srch_ndx].key_segment[0].name, 
                       sizeof(ltr_config_info->ltr_search[srch_ndx].key_segment[0].name));
                }
            }
        }

        table_key_offset = 0;
        for(key_ndx = 0;
             key_ndx < ltr_config_info->master_key_fields.nof_key_segments;
             key_ndx++)
        {
            /*Create a unique field name*/
            if(ltr_config_info->master_key_fields.key_segment[key_ndx].nof_bytes){

                res = kbp_key_add_field(
                        ltr_config_info->master_key, 
                        ltr_config_info->master_key_fields.key_segment[key_ndx].name,
                        (ltr_config_info->master_key_fields.key_segment[key_ndx].nof_bytes*8), 
                        KBP_KEY_FIELD_PREFIX
                      );

                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    if(res != KBP_DUPLICATE_KEY_FIELD) {
                       LOG_ERROR(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "Error in %s(): DB Master Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                             FUNCTION_NAME(),
                                  res,
                                  kbp_get_status_string(res)));

                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                    }
                }

                table_key_offset += ltr_config_info->master_key_fields.key_segment[key_ndx].nof_bytes;
            }
        }

        if ((table_key_offset % 10) != 0) 
        {
            table_key_offset = (10 - (table_key_offset % 10));
            res = kbp_key_add_field(
                    ltr_config_info->master_key, 
                    "DUMMY",
                    table_key_offset*8, 
                    KBP_KEY_FIELD_PREFIX
                  );

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                if(res != KBP_DUPLICATE_KEY_FIELD) {
                   LOG_ERROR(BSL_LS_SOC_TCAM,
                             (BSL_META_U(unit,
                                         "Error in %s(): DB Master Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                         FUNCTION_NAME(),
                              res,
                              kbp_get_status_string(res)));

                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                }
            }
        }

        for(srch_ndx = 0; srch_ndx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; srch_ndx++)
        {
            if (SHR_BITGET(&ltr_config_info->parallel_srches_bmp, srch_ndx)) {
                table_idx = ltr_config_info->tbl_id[srch_ndx];
                if ((table_idx >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_0) && (table_idx <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_3)) {
                    /* Add the dummy fields of the others */
                    res = kbp_key_add_field(
                            ltr_config_info->master_key, 
                            AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.name,
                            AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.nof_bytes*8, 
                            KBP_KEY_FIELD_PREFIX
                          );

                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        if(res != KBP_DUPLICATE_KEY_FIELD) {
                           LOG_ERROR(BSL_LS_SOC_TCAM,
                                     (BSL_META_U(unit,
                                                 "Error in %s(): DB Master Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                                 FUNCTION_NAME(),
                                      res,
                                      kbp_get_status_string(res)));

                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 33, exit);
                        }
                    }
                }
            }
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_key_init()", gtm_db_type, 0x0);
}

STATIC
  uint32 
    arad_kbp_search_init(
        SOC_SAND_IN int                      unit,
        SOC_SAND_IN ARAD_KBP_FRWRD_IP_DB_TYPE   gtm_db_type
    )
{
    uint32
        srch_ndx, res;

    ARAD_KBP_LTR_CONFIG
        *ltr_config_info = NULL;

    ARAD_KBP_LTR_HANDLES
        ltr_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ltr_config_info = &(Arad_kbp_ltr_config[unit][gtm_db_type]);

    if (!SOC_WARM_BOOT(unit)) {
        /* create a new instruction */
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): DB Type [%d] : create a new instruction, LTR ID %d.\n"),
                                FUNCTION_NAME(),
                     gtm_db_type,
                     ltr_config_info->ltr_id));

        KBP_TRY(kbp_instruction_init(
                    AradAppData[unit]->device_p, 
                    ltr_config_info->ltr_id,
                    ltr_config_info->ltr_id, 
                    &ltr_config_info->inst_p)
                );

		/* save instruction handle */
        ltr_handles_info.inst_p = ltr_config_info->inst_p;
        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_LTR_INFO, 
                                        &ltr_handles_info, 
                                        gtm_db_type);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        KBP_TRY(kbp_instruction_set_key(ltr_config_info->inst_p, ltr_config_info->master_key));

        for(srch_ndx = 0; srch_ndx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; srch_ndx++){
            if (SHR_BITGET(&ltr_config_info->parallel_srches_bmp, srch_ndx)) {
                res = kbp_instruction_add_db(ltr_config_info->inst_p,
                                                AradAppData[unit]->g_gtmInfo[ltr_config_info->tbl_id[srch_ndx]].tblInfo.db_p,
                                                srch_ndx);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                   LOG_ERROR(BSL_LS_SOC_TCAM,
                             (BSL_META_U(unit,
                                         "Error in %s(): kbp_instruction_add_db failed with : %s, db_type = %d srch_ndx = %d!\n"), 
                                         FUNCTION_NAME(),
                              kbp_get_status_string(res), gtm_db_type, srch_ndx));

                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                }
            }
        }

        res = kbp_instruction_install(ltr_config_info->inst_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_instruction_install failed with : %s, gtm_db_type = %d!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), gtm_db_type));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }

    }
    else {
        /* refresh the instruction handle */
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_LTR_INFO, 
                                       &ltr_handles_info, 
                                       gtm_db_type);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        KBP_TRY(kbp_instruction_refresh_handle(AradAppData[unit]->device_p, ltr_handles_info.inst_p, &ltr_handles_info.inst_p));

        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_LTR_INFO, 
                                        &ltr_handles_info, 
                                        gtm_db_type);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_search_init()", gtm_db_type, 0x0);
}

/* index change callback function */
void alg_kbp_callback(void *handle,
                      struct kbp_db *db,
                      struct kbp_entry *entry,
                      int32_t old_index,
                      int32_t new_index){
    (void)handle;
    (void)db;
    (void)entry;
    (void)old_index;
    (void)new_index;

    /*LOG_INFO(BSL_LS_SOC_TCAM,
               (BSL_META("\n Index change callback called: oldIndex  = %x,New Index = %x\n"),old_index,new_index));*/
    return;
}

/* Get table size in bytes */
uint32
  arad_kbp_table_size_get(
    SOC_SAND_IN int                   unit, 
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                  *table_size_in_bytes,
    SOC_SAND_OUT uint32                  *table_payload_in_bytes
  )
{
    uint32
        fp_db_id,
        res,
        pgm_idx,
        pgm_bmp_remain,
        pgm_bmp_used,
        db_type,
        ltr_id,
        tbl_width_in_bits = 0,
        tbl_payload_in_bits = 0;
    ARAD_PP_FP_DATABASE_INFO                    
        fp_database_info;
    uint8
        success_uint8,
        opcode,
        is_default_db,
        is_valid;
    ARAD_FP_DATABASE_STAGE   
        stage = ARAD_FP_DATABASE_STAGE_INGRESS_FLP;
    SOC_SAND_SUCCESS_FAILURE    
        success;
    ARAD_KBP_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Check if table was already created */
    if(table_id >= ARAD_KBP_MAX_NUM_OF_TABLES) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    if(table_id >= ARAD_KBP_ACL_TABLE_ID_OFFSET) {
        /* 
         * ACL case 
         * Retrieve the table size by: 
         * 1. Get the used FLP program bitmap and the lookup-id 
         * 2. Get the KBP table id, and its size
         */
        fp_db_id = table_id - ARAD_KBP_ACL_TABLE_ID_OFFSET;
        ARAD_PP_FP_DATABASE_INFO_clear(&fp_database_info);
        res = arad_pp_fp_database_get_unsafe(unit, fp_db_id, &fp_database_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        is_default_db = (fp_database_info.supported_pfgs_arad[0] || fp_database_info.supported_pfgs_arad[1])? FALSE : TRUE;
        res = arad_pmf_psl_pmf_pgms_get(
                  unit,
                  stage,
                  fp_database_info.supported_pfgs_arad,
                  is_default_db, /* Relevant for all lines */
                  FALSE /* is_for_tm */,
                  &pgm_bmp_used,
                  &pgm_bmp_remain,
                  NULL /* presel_bmp_update */,
                  &success_uint8);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        /* Get a FLP program used */
        for(pgm_idx = 0; pgm_idx < ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; pgm_idx++)
        {
            if (SHR_BITGET(&pgm_bmp_used, pgm_idx)) {
                /* Assumption at least one program */
                break;
            }
        }

        /* Get the table id */
        opcode = ARAD_KBP_FLP_PROG_TO_OPCODE(pgm_idx);
        res = arad_kbp_opcode_to_db_type(
                  unit,
                  opcode,
                  &db_type,
                  &ltr_id,
                  &success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                       &db_handles_info, 
                                       table_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        is_valid = db_handles_info.is_valid;
        if (!is_valid) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }
 	    tbl_width_in_bits = db_handles_info.table_width;
 	 	tbl_payload_in_bits = db_handles_info.table_asso_width;
       
    }
    else {
        tbl_width_in_bits = Arad_kbp_table_config_info[unit][table_id].tbl_width;
        tbl_payload_in_bits = Arad_kbp_table_config_info[unit][table_id].tbl_asso_width;
    }
    *table_size_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_width_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); /* Key size in bytes */
    *table_payload_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_payload_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); /* Payload size in bytes */

    ARAD_DO_NOTHING_AND_EXIT;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_size_get()", table_id, 0x0);
}

uint32
  arad_kbp_static_table_size_get(
    SOC_SAND_IN int                   unit, 
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                  *table_size_in_bytes,
    SOC_SAND_OUT uint32                  *table_payload_in_bytes
  )
{
    uint32
        tbl_width_in_bits = 0,
        tbl_payload_in_bits = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Check if table was already created */
    if(table_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    tbl_width_in_bits = Arad_kbp_table_config_info_static[table_id].tbl_width;
    tbl_payload_in_bits = Arad_kbp_table_config_info_static[table_id].tbl_asso_width;
    
    *table_size_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_width_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); /* Key size in bytes */
    *table_payload_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_payload_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); /* Payload size in bytes */

    ARAD_DO_NOTHING_AND_EXIT;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_static_table_size_get()", table_id, 0x0);
}

/* Get LTR-ID per table id */
uint32
  arad_kbp_table_ltr_id_get(
    SOC_SAND_IN int                   unit, 
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT ARAD_KBP_FRWRD_IP_LTR   *ltr_id
  )
{
    ARAD_KBP_FRWRD_IP_DB_TYPE 
        gtm_db_type_ndx;
    uint32
        srch_ndx;
    uint8
        found;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    found = FALSE;
    for(gtm_db_type_ndx = 0; (gtm_db_type_ndx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) && (!found); gtm_db_type_ndx++) 
    {
        for(srch_ndx = 0; (srch_ndx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES) && (!found); srch_ndx++) 
        {
            if (SHR_BITGET(&Arad_kbp_ltr_config[unit][gtm_db_type_ndx].parallel_srches_bmp, srch_ndx)) {
                if (Arad_kbp_ltr_config[unit][gtm_db_type_ndx].tbl_id[srch_ndx] == table_id) {
                    found = TRUE;
                    *ltr_id = Arad_kbp_ltr_config[unit][gtm_db_type_ndx].ltr_id;
                }
            }
        }
    }

    /* Assumption always found */
    if (!found) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    ARAD_DO_NOTHING_AND_EXIT;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_ltr_id_get()", table_id, 0x0);
}

/* This function performs Generic Table Manager related Inits */
STATIC
  uint32 
    arad_kbp_table_init(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 table_size,
        SOC_SAND_IN  uint32 table_id
    )
{
    uint32 
        res = SOC_SAND_OK;

    ARAD_KBP_TABLE_CONFIG
        *table_config_info = NULL;
    uint32
        db_type = KBP_DB_LPM;
    ARAD_KBP_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(NULL != AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /* Check if table was already created */
    table_config_info = &(Arad_kbp_table_config_info[unit][table_id]);

    /* Create tables according to GTM database type */
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id        = table_config_info->tbl_id;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width     = table_config_info->tbl_width;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size      = table_size;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth = table_config_info->tbl_asso_width;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.groupId_start = table_config_info->group_id_start;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.groupId_end   = table_config_info->group_id_end;

    if (!SOC_WARM_BOOT(unit)) {
        /* Call the CreateTable API to create each of the tables */
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): Table [%d] : create DB with width=%d, table-size=%d.\n"),
                               FUNCTION_NAME(),
                    table_config_info->tbl_id,
                    table_config_info->tbl_width,
                    table_config_info->tbl_size));

        if (table_config_info->clone_of_tbl_id == ARAD_KBP_FRWRD_IP_NOF_TABLES) {
            /* Not a clone. Create the DB */
            if (table_config_info->tbl_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES || ((table_config_info->tbl_id >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_0) && (table_config_info->tbl_id <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_3))) {
                db_type = KBP_DB_ACL; 
            }
            res = kbp_db_init(
                    AradAppData[unit]->device_p, 
                    db_type, 
                    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id,
                    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size, 
                    &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_init failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            /* Associate DB with AD table */
            if ((AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth != NLM_TBL_ADLEN_ZERO) && 
                (AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size > 0)) {
                LOG_VERBOSE(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                        "%s(): Table [%d] : create AD DB with asso-data=%d.\n"),
                                        FUNCTION_NAME(),
                             table_id,
                             ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth)));

                KBP_TRY(kbp_ad_db_init(
                            AradAppData[unit]->device_p, 
                            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id,
                            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size, 
                            ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth),
                            &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p)
                        );

                KBP_TRY(kbp_db_set_ad(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                      AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p));

				/* save DB handles */
                db_handles_info.db_p    = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p;
                db_handles_info.ad_db_p = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p;
                db_handles_info.is_valid = TRUE;
                db_handles_info.table_size = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size;
                db_handles_info.table_id = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id;
                db_handles_info.table_width = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width;
                db_handles_info.table_asso_width = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth;
                res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                                &db_handles_info, 
                                                table_id);

                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            }

           LOG_VERBOSE(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "%s(): Table [%d] : Create new key for each database.\n"),
                                   FUNCTION_NAME(),
                        table_config_info->tbl_id));
        }
        else {
            KBP_TRY(kbp_db_clone(AradAppData[unit]->g_gtmInfo[table_config_info->clone_of_tbl_id].tblInfo.db_p,
                         AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id,
                         &(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p))
                    );

			/* save Db handle */
            db_handles_info.db_p    = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p;
            res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                            &db_handles_info, 
                                            table_id);

            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        }
        KBP_TRY(kbp_key_init(AradAppData[unit]->device_p, 
                             &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.key));

       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): Table [%d] : Setting ACL database to massively parallel mode.\n"),
                               FUNCTION_NAME(), table_config_info->tbl_id));

        KBP_TRY(kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, 
                                    KBP_PROP_ALGORITHMIC, 0));

        if (table_config_info->min_priority >= 0) {
            KBP_TRY(kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, 
                                        KBP_PROP_MIN_PRIORITY, table_config_info->min_priority));
        }

        /* Set UPD level. Enables all HW DBA resources. */
        KBP_TRY(kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, 
                                109, 2));

        ARAD_DO_NOTHING_AND_EXIT;
    }
    else {
        /* refresh the DB handles */
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                       &db_handles_info, 
                                       table_id);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        KBP_TRY(kbp_db_refresh_handle(AradAppData[unit]->device_p, db_handles_info.db_p, &db_handles_info.db_p));
        KBP_TRY(kbp_ad_db_refresh_handle(AradAppData[unit]->device_p, db_handles_info.ad_db_p, &db_handles_info.ad_db_p));

        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p = db_handles_info.db_p;
        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p = db_handles_info.ad_db_p;
        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size = db_handles_info.table_size;
        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id = db_handles_info.table_id; 
        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width = db_handles_info.table_width;
        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth = db_handles_info.table_asso_width;

        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                        &db_handles_info, 
                                        table_id);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    }
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_init()", table_id, table_size);
}


uint32 arad_kbp_frwrd_opcode_set(
    int unit,
    uint8 opcode,
    uint32 tx_data_size, 
    uint32 tx_data_type, 
    uint32 rx_data_size, 
    uint32 rx_data_type)
{
    uint32
        res;
    uint32
        table_entry[1];
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Configure in the EGW OPCODES */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, opcode, table_entry));
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, tx_data_size);
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, tx_data_type);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, opcode, table_entry));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, opcode, table_entry));
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, rx_data_size);
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, rx_data_type);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, opcode, table_entry));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_frwrd_opcode_set()", opcode, tx_data_size);
}

uint32 arad_kbp_frwrd_ip_rpf_lut_set(
    int unit,
    uint8 opcode,
    uint32 ltr,
    uint32 gtm_table_id)
{
    uint32
        res = SOC_SAND_OK;
    arad_kbp_lut_data_t 
        *lut_data;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    lut_data = &(Arad_kbp_gtm_lut_info[unit][gtm_table_id]);

    /* Setting LTR to opcode mapping DB
     * res_format - rounding up AD size.
     */
    
    sal_memcpy(
       &(arad_kbp_frwd_ltr_db[ltr]), 
       &(Arad_kbp_gtm_ltr_info[unit][gtm_table_id]), 
       sizeof(arad_kbp_frwd_ltr_db_t)
    );
    
    if (!SOC_WARM_BOOT(unit)) {
        res = arad_kbp_lut_write(unit, opcode, lut_data, NULL);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_frwrd_ip_rpf_lut_set()", opcode, ltr);
}

uint32 arad_kbp_frwrd_ip_opcode_set(
    int unit,
    uint8 opcode,
    uint32 gtm_table_id)
{
    uint32
        res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_WARM_BOOT(unit)) {
        /* Setting Arad opcode */
        res = arad_kbp_frwrd_opcode_set(
            unit, 
            opcode,
            Arad_kbp_gtm_opcode_config_info[unit][gtm_table_id].tx_data_size,
            Arad_kbp_gtm_opcode_config_info[unit][gtm_table_id].tx_data_type,
            Arad_kbp_gtm_opcode_config_info[unit][gtm_table_id].rx_data_size,
            Arad_kbp_gtm_opcode_config_info[unit][gtm_table_id].rx_data_type
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_frwrd_ip4_rpf_opcode_set()", opcode, gtm_table_id);
}


uint32 arad_kbp_init_db_set(
    int unit,
    ARAD_INIT_ELK *elk)
{
    uint32
        tbl_idx,
        db_type,
        res,
        tbl_id,
        tbl_size;

    uint8 is_valid;

    ARAD_KBP_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Configure DBs */
    for (tbl_idx = 0; tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; tbl_idx++) 
    {
        if (SOC_WARM_BOOT(unit)) {
            res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                                           &db_handles_info, 
                                           tbl_idx);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            is_valid = db_handles_info.is_valid;
            tbl_id = db_handles_info.table_id;
            tbl_size = db_handles_info.table_size;
        } 
        else {
            is_valid = Arad_kbp_table_config_info[unit][tbl_idx].valid; 
            tbl_id = Arad_kbp_table_config_info[unit][tbl_idx].tbl_id;
            tbl_size = Arad_kbp_table_config_info[unit][tbl_idx].tbl_size;
        }

        if (is_valid) 
        {
            res = arad_kbp_table_init(
                    unit, 
                    tbl_size, 
                    tbl_id
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }

    /* Configure Tables keys in KBP */
    for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) 
    {
        if (Arad_kbp_ltr_config[unit][db_type].valid) 
        {
            /* Configure Search */
            res = arad_kbp_key_init(
                    unit, 
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
        }
    }

    /* Configure Tables and LTRs in KBP */
    for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) 
    {
        if (Arad_kbp_ltr_config[unit][db_type].valid) 
        {
           LOG_VERBOSE(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "%s(): Configure DB Type %s, LTR=%d(0x%x), Opcode = %d\n"), 
                                   FUNCTION_NAME(),
                        ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(db_type),
                        Arad_kbp_ltr_config[unit][db_type].ltr_id,
                        Arad_kbp_ltr_config[unit][db_type].ltr_id,
                        Arad_kbp_ltr_config[unit][db_type].opcode));

            /* Configure Search */
            res = arad_kbp_search_init(
                    unit, 
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

            /* Configure KBP Opcode/LUT table */
            res = arad_kbp_frwrd_ip_rpf_lut_set(
                    unit, 
                    Arad_kbp_ltr_config[unit][db_type].opcode,
                    Arad_kbp_ltr_config[unit][db_type].ltr_id,
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            /* Configure Arad Opcode/LUT table */
            res = arad_kbp_frwrd_ip_opcode_set(
                    unit, 
                    Arad_kbp_ltr_config[unit][db_type].opcode,
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        }
    }

    res = arad_kbp_device_lock_config(unit);
    if(res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "Error in %s(): %s!\n"), 
                         FUNCTION_NAME(),
              kbp_get_status_string(res)));
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_db_set()", 0, 0);
}

uint32 arad_kbp_db_commit(
   int unit)
{
    struct kbp_db 
        *db_p = NULL;
    uint32
        tbl_idx,
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* commit DBs */
    for (tbl_idx = 0; tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; tbl_idx++) 
    {
        if (Arad_kbp_table_config_info[unit][tbl_idx].valid) 
        {
            res = arad_kbp_alg_kbp_db_get(unit, tbl_idx, &db_p);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
soc_sand_ll_timer_set("arad_kbp_db_commit", 3);
#endif 
            res = kbp_db_install(db_p);
#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
soc_sand_ll_timer_stop(3);
#endif 
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_db_commit()", 0, 0);
}

STATIC
uint32 arad_kbp_deinit_frwd_db_set(
    int unit)
{
    uint32
        res,
        db_idx;

    ARAD_KBP_DB_HANDLES
        db_handles_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Set all configuration table to 0 */
    sal_memset(&Arad_kbp_table_config_info[unit][0], 0x0, sizeof(ARAD_KBP_TABLE_CONFIG)*ARAD_KBP_MAX_NUM_OF_TABLES);
    sal_memset(&Arad_kbp_ltr_config[unit][0], 0x0, sizeof(ARAD_KBP_LTR_CONFIG)*ARAD_KBP_MAX_NUM_OF_FRWRD_DBS);
    sal_memset(&Arad_kbp_gtm_lut_info[unit][0], 0x0, sizeof(arad_kbp_lut_data_t)*ARAD_KBP_MAX_NUM_OF_FRWRD_DBS);
    sal_memset(&Arad_kbp_gtm_ltr_info[unit][0], 0x0, sizeof(arad_kbp_frwd_ltr_db_t)*ARAD_KBP_MAX_NUM_OF_FRWRD_DBS);
    sal_memset(&Arad_kbp_gtm_opcode_config_info[unit][0], 0x0, sizeof(ARAD_KBP_GTM_OPCODE_CONFIG_INFO)*ARAD_KBP_MAX_NUM_OF_FRWRD_DBS);

    arad_kbp_sw_init(unit);

    for(db_idx = 0; db_idx < ARAD_KBP_MAX_NUM_OF_TABLES; db_idx++) 
    {
        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_KBP_DB_INFO, 
                &db_handles_info, 
                db_idx);

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }
 
exit:   
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in  arad_kbp_deinit_frwd_db_set()", 0, 0);
}

/* 
 * Arad KBP application init function. 
 * Called from PP Mgmt. 
 */
uint32 arad_kbp_init_app(
    int unit,
    ARAD_INIT_ELK *elk_ptr)
{
    uint32
        res;
    ARAD_INIT_ELK *elk;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (elk_ptr == NULL) {
        elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    } else {
        elk = elk_ptr;
    }

    if (elk->enable == 0x0) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): ELK disabled (ext_tcam_dev_type might be NONE)!!!\n"), FUNCTION_NAME()));    
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    /* ROP Write-Read Test */
    res = arad_kbp_init_rop_test(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    AradAppData[unit] = NULL;
    ARAD_ALLOC_ANY_SIZE(AradAppData[unit], genericTblMgrAradAppData, 1,"AradAppData[unit]");
    sal_memset(AradAppData[unit], 0x0, sizeof(genericTblMgrAradAppData));

    /* NLM Application init */
    res = arad_kbp_init_nlm_app_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Create KBP DB, config KBP searches, config KBP LUT, config Arad opcode */
    res = arad_kbp_init_db_set(unit, elk);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (!SOC_WARM_BOOT(unit)) {
        res = arad_kbp_blk_lut_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_app()", elk->enable, elk->tcam_dev_type);
}

uint32 
    arad_kbp_deinit_app(
        int unit
    )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(NULL == AradAppData[unit])
    {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp device not initialized.\n"), FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = arad_kbp_deinit_frwd_db_set(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    arad_kbp_deinit_nlm_app_set(unit);

    ARAD_FREE(AradAppData[unit]);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_deinit_app()", unit, 0x0);
}

void
  arad_kbp_frwd_ltr_db_clear(
    SOC_SAND_OUT arad_kbp_frwd_ltr_db_t *info
  )
{
    uint32 
        res_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(arad_kbp_frwd_ltr_db_t));
    for( res_idx = 0; 
         res_idx < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; 
         res_idx++ )
    {
        info->res_format[res_idx] = NLM_ARAD_NO_INDEX_NO_AD;
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_clear(
    SOC_SAND_OUT ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_GTM_OPCODE_CONFIG_INFO));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *info
  )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_LTR_CONFIG));
    info->valid = FALSE;
    
    for (index = 0; index < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; index++) 
    {
        info->tbl_id[index] = ARAD_KBP_TABLE_INDX_TO_DUMMY_TABLE_ID(index);
        info->search_type[index] = ARAD_KBP_NOF_DB_TYPES;
        ARAD_KBP_LTR_SINGLE_SEARCH_clear(info->ltr_search);
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_SINGLE_SEARCH_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_SINGLE_SEARCH *info
  )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_LTR_SINGLE_SEARCH));

    for (index = 0; index < ARAD_KBP_MAX_NOF_KEY_SEGMENTS; index++) 
    {
        ARAD_KBP_KEY_SEGMENT_clear(&info->key_segment[index]);
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_TABLE_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_TABLE_CONFIG *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_TABLE_CONFIG));
    info->valid = FALSE;
    info->tbl_width = NLM_TBL_WIDTH_END;
    info->tbl_asso_width = NLM_TBL_ADLEN_END;
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_KEY_SEGMENT_clear(
    SOC_SAND_OUT ARAD_KBP_KEY_SEGMENT *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(ARAD_KBP_KEY_SEGMENT));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  NlmAradCompareResponseFormat_to_string(
    SOC_SAND_IN  NlmAradCompareResponseFormat enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case NLM_ARAD_ONLY_INDEX_NO_AD:
    str = "Index-No-AD";
  break;
  case NLM_ARAD_INDEX_AND_32B_AD:
    str = "Index,32bAD";
  break;
  case NLM_ARAD_INDEX_AND_64B_AD:
    str = "Index,64bAD";
  break;
  case NLM_ARAD_INDEX_AND_128B_AD:
    str = "Index,128bAD";
  break;
  case NLM_ARAD_NO_INDEX_NO_AD:
    str = "No-Index-No-AD";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC:
    str = "IPv4 Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF:
    str = "IPv4 Unicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF:
    str = "IPv4 Multicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC:
    str = "IPv6 Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF:
    str = "IPv6 Unicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IPV6_MC_RPF:
    str = "IPv6 Multicast RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_LSR:
    str = "MPLS";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_TRILL_UC:
    str = "TRILL Unicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_TRILL_MC:
    str = "TRILL Multicast";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP:
    str = "IP LSR SHARED FOR IP";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP_WITH_RPF:
    str = "IP LSR SHARED FOR IP WITH RPF";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_LSR:
    str = "IP LSR SHARED FOR LSR";
  break;
  case ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_IPv6:
    str = "EXTENDED IPv6";
  break;

  case ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_P2P:
    str = "EXTENDED P2P";
  break;

  default:
    str = "ACL";
  }
  return str;
}

const char*
  ARAD_KBP_FRWRD_IP_TBL_ID_to_string(
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0:
    str = "IPv4 UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1:
    str = "IPv4 RPF";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV4_MC:
    str = "IPv4 MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0:
    str = "IPv6 UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1:
    str = "IPv6 RPF";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_IPV6_MC:
    str = "IPv6 MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR:
    str = "MPLS";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_TRILL_UC:
    str = "TRILL UC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_TRILL_MC:
    str = "TRILL MC";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_0:
    str = "DUMMY 0";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_1:
    str = "DUMMY 1";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_2:
    str = "DUMMY 2";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_DUMMY_3:
    str = "DUMMY 3";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED:
    str = "LSR IP SHARED";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP:
    str = "LSR IP SHARED FOR IP";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR:
    str = "LSR IP SHARED FOR LSR";
  break;
  case ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6:
    str = "EXTENDED IPV6";
  break;
  default:
    str = "ACL";
  }
  return str;
}

const char*
  ARAD_KBP_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_KBP_DB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case ARAD_KBP_DB_TYPE_ACL:
    str = "ACL";
  break;
  case ARAD_KBP_DB_TYPE_FORWARDING:
    str = "Forwarding";
  break;
  default:
    str = "Unknown";
  }
  return str;
}

/* Print Definitions */
void
  arad_kbp_frwd_ltr_db_print(
    SOC_SAND_IN  arad_kbp_frwd_ltr_db_t *info
  )
{
    uint32
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    for (index = 0; index < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; index++) 
    {
        if ((info->res_format[index] != NLM_ARAD_NO_INDEX_NO_AD) &&
            (info->res_format[index] != NLM_ARAD_ONLY_INDEX_NO_AD)) {
            LOG_CLI((BSL_META_U(unit,
                                "  Result %d: length=%d, format=<%s>\n"), 
                     index, 
                     info->res_data_len[index], 
                     NlmAradCompareResponseFormat_to_string(info->res_format[index])));
        }
    }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_print(
    SOC_SAND_IN  ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "  TX Data Size: %d\n"), info->tx_data_size));
    LOG_CLI((BSL_META_U(unit,
                        "  TX Data Type: %d\n"), info->tx_data_type));
    LOG_CLI((BSL_META_U(unit,
                        "  RX Data Size: %d\n"), info->rx_data_size));
    LOG_CLI((BSL_META_U(unit,
                        "  RX Data Type: %d\n"), info->rx_data_type));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_CONFIG_print(
    SOC_SAND_IN  ARAD_KBP_LTR_CONFIG *info
  )
{
    uint32
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    if (info->valid) 
    {
        LOG_CLI((BSL_META_U(unit,
                            "  Opcode: %d, LTR ID: %d, Parallel Searches bitmap: %d\n"), 
                 info->opcode, 
                 info->ltr_id,
                 info->parallel_srches_bmp));

        LOG_CLI((BSL_META_U(unit,
                            "  Master Key: ")));
        for (index = 0; index < info->master_key_fields.nof_key_segments; index++) 
        {
            ARAD_KBP_KEY_SEGMENT_print(&info->master_key_fields.key_segment[index]);
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        
        for (index = 0; index < ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES; index++) 
        {
            if (SHR_BITGET(&info->parallel_srches_bmp, index)) {
                LOG_CLI((BSL_META_U(unit,
                                    "  SEARCH %d: table %d, type %s, "), index, info->tbl_id[index], 
                         ARAD_KBP_DB_TYPE_to_string(info->search_type[index])));
                ARAD_KBP_LTR_SINGLE_SEARCH_print(&info->ltr_search[index]);
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
        }
    }

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_LTR_SINGLE_SEARCH_print(
    SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH *info
    )
{
    uint32 
        index;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "Segments(name,bytes): ")));
    for (index = 0; index < info->nof_key_segments; index++) 
    {
        ARAD_KBP_KEY_SEGMENT_print(&info->key_segment[index]);
    }
    
exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_KBP_TABLE_CONFIG_print(
      SOC_SAND_IN  int  unit,
      SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG *info
    )
{
    struct kbp_db_stats stats;
    uint32 res;

    if (info->valid) 
    {
        LOG_CLI((BSL_META_U(unit,
                            " %-9d"), info->tbl_id));
        LOG_CLI((BSL_META_U(unit,
                            "%-15s"), ARAD_KBP_FRWRD_IP_TBL_ID_to_string(info->tbl_id)));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->tbl_size));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), info->tbl_width));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(info->tbl_asso_width)));
        /* 
        LOG_CLI((BSL_META_U(unit,
                            "%-10d\n"), info->group_id_start)); 
        LOG_CLI((BSL_META_U(unit,
                            "%-10d\n"), info->group_id_end));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d\n"), info->bankNum));
        */ 
        res = kbp_db_stats(AradAppData[unit]->g_gtmInfo[info->tbl_id].tblInfo.db_p, &stats);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): DB: kbp_db_stats with failed: %s!\n"), 
                                  FUNCTION_NAME(),
                       kbp_get_status_string(res)));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-10d"), stats.num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "%-10d\n"), stats.capacity_estimate));
    }
}

void
  ARAD_KBP_KEY_SEGMENT_print(
    SOC_SAND_IN  ARAD_KBP_KEY_SEGMENT *info
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "<%s,%d> "), info->name, info->nof_bytes));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
    arad_kbp_ltr_config_get(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 flp_program,
        SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *config
    )
{
    uint32 prog_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(config);

    for(prog_id = 0; prog_id < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; ++prog_id) {
        if(Arad_kbp_ltr_config[unit][prog_id].opcode == flp_program && Arad_kbp_ltr_config[unit][prog_id].valid) {
            *config = Arad_kbp_ltr_config[unit][prog_id];
            ARAD_DO_NOTHING_AND_EXIT;
        }
    }

    /* no configuration found */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_config_get()", 0, 0);
}

int
 arad_kbp_sync(int unit)
{
    kbp_warmboot_t *warmboot_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    warmboot_data = &kbp_warmboot_data[unit];

    KBP_TRY(kbp_device_save_state(AradAppData[unit]->device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));
   
    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

void 
  arad_kbp_warmboot_register(int unit,
                             FILE *file_fp,
                             kbp_device_issu_read_fn read_fn, 
                             kbp_device_issu_write_fn write_fn)
{
    kbp_warmboot_data[unit].kbp_file_fp = file_fp;
    kbp_warmboot_data[unit].kbp_file_read = read_fn;
    kbp_warmboot_data[unit].kbp_file_write = write_fn;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* #if defined(BCM_88650_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */

