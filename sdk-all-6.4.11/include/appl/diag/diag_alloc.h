/*
 * $Id: diag_alloc.h,v 1.3 Broadcom SDK $
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
 *
 * File:        diag_alloc.h
 * Purpose:     
 */

#ifndef   _DIAG_ALLOC_H_
#define   _DIAG_ALLOC_H_

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/gport_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#endif

/* check if the gport is supported */
#define diag_alloc_gport_type_is_supported(gport_id) (BCM_GPORT_IS_VLAN_PORT(gport_id) || BCM_GPORT_IS_MPLS_PORT(gport_id))


/* *******************************************************************************************************
                                 enums and structs for diag alloc
   ******************************************************************************************************* */

/* parameters of diag alloc  */
typedef struct
{
    int from; /* optional parameter: start range, if unused -1  */
    int to; /* optional parameter: end range, if unused -1 */
    int info;  /* optional parameter: get more info from the current id */
    int direct;    /*optional parameter: get directly from hardware */
} diag_alloc_system_table_parameters;

typedef struct
{
   const char     *poolDescription;
   const char     *poolName;
   const char     *shortPoolName;
   int            pool_id;
   cmd_result_t  (*info_print_cb) (int,int); /* define callback for print */
   cmd_result_t  (*cmd_diag_alloc_hw_block) (int, diag_alloc_system_table_parameters *); /* define callback for HW info print */
   
} DIAG_ALLOC_POOL_DEF;

/* exist resource */
typedef enum diag_alloc_gport_resource_exist_e {
    diag_alloc_gport_resource_not_exist = 1,  /* SHOULD BE FIRST */
    diag_alloc_gport_resource_exist, 
    diag_alloc_gport_resource_remote
} diag_alloc_gport_resource_exist_t;

/* gport type supported in diag alloc gport */
typedef enum diag_alloc_gport_info_gport_type_e {
    diag_alloc_gport_info_gport_type_vlan = 1, /* SHOULD BE FIRST */
    diag_alloc_gport_info_gport_type_mpls 
} diag_alloc_gport_info_gport_type_t;


/* encoding */
typedef enum diag_alloc_gport_info_encoding_e {
    diag_alloc_gport_info_encoding_no_protection = 1, /*SHOULD BE FIRST */
    diag_alloc_gport_info_encoding_one_plus_one_protection, 
    diag_alloc_gport_info_encoding_one_one_protection
} diag_alloc_gport_info_encoding_t ;


/* params of the diag pp gport call */
typedef struct
{
  /* 
   * gport id
   */
   uint32 gport_id;
  /* 
   * indicate if the user wants general information on inLif,outLif,FEC MC-ID. 
   */
  uint32 resource;
  /* 
   * indicate if the user wants information on FEC db
   */
  uint32 forwarding_database;
  /* 
   * indicate if the user wants information on LIF database
   */
  uint32 lif_database;
} DIAG_ALLOC_GPORT_PARAMS;

#ifdef BCM_DPP_SUPPORT
/* diag pp gport infos */
typedef struct
{
    /* 
     * Gport type: VLAN, MPLS, ...
     */
    diag_alloc_gport_info_gport_type_t gport_type;
    /* 
     * Gport id
     */
    uint32 gport_id;
    /* 
     * GlobalInLif
     */
    int global_lif_id;

    /* 
     * inLif
     */
    int lif_id;

    /* 
     * outLif
     */
    int out_lif_id;

    /*
     * indicate if lif are remote
     */
    int is_remote;

    /* 
     * fec 
     */
    int fec_id;

    /* 
     * Forwarding info 
     */
    _BCM_GPORT_PHY_PORT_INFO *forwarding_info_data;

    /* 
     * multicast id
     */
    int multicast_id; 

    /* 
     * is the multicast is primary or secondary
     */
    int multicast_is_primary; 

    /* 
     * Encoding: 1+1 protection, No protection or 1:1 protection
     */
    diag_alloc_gport_info_encoding_t encoding;
} DIAG_ALLOC_GPORT_INFO;

 


/* Functions diag alloc */
cmd_result_t
cmd_dpp_diag_alloc(int unit, args_t* a); 

void
print_alloc_usage(int unit);

cmd_result_t
cmd_dpp_diag_tmplt(int unit, args_t* a); 

void
print_tmplt_usage(int unit);


/* Utils function for diag pp gport:*/
/* clear gport_param */
void diag_alloc_gport_param_init(DIAG_ALLOC_GPORT_PARAMS* gport_params);
/* clear gport_info */
void diag_alloc_gport_info_init(DIAG_ALLOC_GPORT_INFO* gport_info);


cmd_result_t 
diag_alloc_gport_info_get(int unit, 
                          DIAG_ALLOC_GPORT_PARAMS* gport_params, 
                          DIAG_ALLOC_GPORT_INFO* gport_info);

void 
diag_alloc_gport_info_print(int unit, 
                            DIAG_ALLOC_GPORT_PARAMS* gport_params,
                            DIAG_ALLOC_GPORT_INFO* gport_info);

int 
cmd_diag_alloc_hw_fec_get_block_and_print(int unit, 
                                          SOC_PPD_FRWRD_FEC_MATCH_RULE *prm_rule, 
                                          SOC_SAND_TABLE_BLOCK_RANGE   *prm_block_range, 
                                          int                          block_size);

int 
cmd_diag_alloc_hw_lif_table_get_block_and_print(int unit, 
                                          SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE *prm_rule, 
                                          SOC_SAND_TABLE_BLOCK_RANGE   *prm_block_range, 
                                          int                          block_size);
cmd_result_t 
dpp_diag_alloc_cosq_sched_print(int unit,
                                int core,
                                int flow_id,
                                int flow_count,
                                char* allocated_resource);
#endif /* BCM_DPP_SUPPORT */
#endif /* _DIAG_ALLOC_H_ */

