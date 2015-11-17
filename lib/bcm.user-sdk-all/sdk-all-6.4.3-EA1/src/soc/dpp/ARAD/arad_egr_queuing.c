#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_egr_queuing.c,v 1.98 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
#include <soc/mem.h>
 * INCLUDES  *
 *************/
/* { */
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/TMC/tmc_api_framework.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_nif.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


#define ARAD_TC_NDX_MAX                                          (7)
#define ARAD_DP_NDX_MAX                                          (3)
#define ARAD_MAP_TYPE_NDX_MAX                                    (ARAD_EGR_NOF_Q_PRIO_MAPPING_TYPES-1)
#define ARAD_MAP_PROFILE_NDX_MAX                                 (7)


/* Invalid base q pair: since assumed ERP port is located at the last PS. */

#define ARAD_EGQ_PS_DEF_PRIORITY                                 (2)
#define ARAD_EGQ_PS_PRIORITY_NOF_BITS                            (2)
#define ARAD_EGQ_SHAPER_MODE_NOF_BITS                            (1)
/* By default, port will be assigned to two priorities (i.e. two q-pairs) */
#define ARAD_EGQ_PORT_DEF_PRIORITY_MODE                          (ARAD_EGR_PORT_TWO_PRIORITIES)
#define ARAD_EGR_PORT_PRIORITY_MAX                               (ARAD_EGR_PORT_EIGHT_PRIORITIES)

#define ARAD_EGQ_PS_MODE_ONE_PRIORITY_VAL                        (0)
#define ARAD_EGQ_PS_MODE_TWO_PRIORITY_VAL                        (1)
#define ARAD_EGQ_PS_MODE_EIGHT_PRIORITY_VAL                      (2)

#define ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(profile,egress_tc) \
  ((profile << 3) + egress_tc)


#define ARAD_EGR_SINGLE_MEMBER_TCG_START (4)
#define ARAD_EGR_SINGLE_MEMBER_TCG_END (7)
#define ARAD_EGQ_NOF_TCG_IN_BITS (3)
#define ARAD_EGQ_TCG_WEIGHT_MIN                (0)
#define ARAD_EGQ_TCG_WEIGHT_MAX                (255)
#define ARAD_EGQ_PORT_ID_INVALID (256)

/*
 *    FQP NIF Port MUX table predefined values
 */

/* Give BW to other interfaces always */


/* FQP value for other */
#define ARAD_EGQ_NIF_OTHERS_DEF_VAL               40

/* PQP value for others */
#define ARAD_EGQ_NIF_PQP_CPU_DEF_VAL              1
#define ARAD_EGQ_NIF_PQP_RCY_DEF_VAL              1
#define ARAD_EGQ_NIF_PQP_OLP_DEF_VAL              1
#define ARAD_EGQ_NIF_PQP_OAM_DEF_VAL              1

#define CGM_MC_INTERFACE_PD_TH_NOF_FIELDS         4
#define CGM_UC_INTERFACE_TH_NOF_FIELDS            8

/* QDCT PD Values are restricted to 12 bits (even though the field is 15 bits) */
#define EGQ_QDCT_PD_MAX_VAL                       4095

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
typedef struct
{
  uint32    rate;
} ARAD_NIF_PORT_RATE;

typedef struct
{
  ARAD_NIF_PORT_RATE nif_prt_rts[ARAD_NIF_NOF_NIFS];
  uint32 nof_nifs;
} ARAD_NIF_PORT_RATES;
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
/*********************************************************************
* NAME:
*     ARAD_NIF_PORT_RATES_clear
* TYPE:
*   PROC
* DATE:
*   Jul 22 2010
* FUNCTION:
*         Initialize Nif Rates struct
* INPUT:
*         SOC_SAND_IN int unit
* REMARKS:
* RETURNS:
*     void.
*********************************************************************/
STATIC void
    ARAD_NIF_PORT_RATES_clear(
      SOC_SAND_OUT ARAD_NIF_PORT_RATES  *nif_rates
    )
{
  uint32 ind;

  nif_rates->nof_nifs = 0;

  for( ind = 0; ind < ARAD_NIF_NOF_NIFS ; ind ++)
  {
    nif_rates->nif_prt_rts[ind].rate = 0;
  }
}

/*********************************************************************
* NAME:
*     arad_egr_q_fqp_scheduer_config
* TYPE:
*   PROC
* DATE:
* FUNCTION:
*         Set fqp scheduler configuration:
*         1. Enable BW requests by others
*         2. Disable BW higher requests on account port with rate of 1009 or higher
* INPUT:
*         SOC_SAND_IN int unit
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  arad_egr_q_fqp_scheduler_config(
    SOC_SAND_IN int unit
  )
{
    soc_pbmp_t port_bm;
    uint32 
        flags,
        is_master,
        if_rate_mbps;
    soc_port_t port;

    SOCDNX_INIT_FUNC_DEFS;
  
    /*Bandwidth normal requests by others*/
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_RCY_REQ_ENf,  0x1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_OAM_REQ_ENf,  0x1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_OLP_REQ_ENf,  0x1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_DEL_REQ_ENf,  0x1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_CPU_REQ_ENf,  0x1));

    /*Bandwidth Higher requests by others*/
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_RCY_HIGHER_ENf,  0x0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_OAM_HIGHER_ENf,  0x0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_OLP_HIGHER_ENf,  0x0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_DEL_HIGHER_ENf,  0x0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, SOC_CORE_ALL, 0, CFG_CPU_HIGHER_ENf,  0x0));

    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /*Disable higher requests on account of 100g ports or higher*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));

        SOC_PBMP_ITER(port_bm, port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

            if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
                /* Statistics interface is not required for Egress */
                continue;
            }
            if (SOC_PORT_IS_ELK_INTERFACE(flags)){
                continue;
            }
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
            if(!is_master) {
                continue;
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));

            if (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS)
            {
                uint32 phys_port_uint32[1],
                higher_request_per_mal[1];
                soc_pbmp_t phys_port;

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phys_port));

                /*soc_port_t to uint32*/
                *phys_port_uint32 = SOC_PBMP_WORD_GET(phys_port, 0);
                /*1 base to 0 base*/
                *phys_port_uint32 = *phys_port_uint32 >> 1;

                /*remove lanes from higer request bitmap*/
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EGQ_HIGHER_REQ_EN_PER_MALr, SOC_CORE_ALL, 0, HIGHER_REQ_EN_PER_MALf, higher_request_per_mal));
                SHR_BITREMOVE_RANGE(higher_request_per_mal, phys_port_uint32, 0, BYTES2BITS(sizeof(uint32)), higher_request_per_mal);
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_HIGHER_REQ_EN_PER_MALr, SOC_CORE_ALL, 0, HIGHER_REQ_EN_PER_MALf,  *higher_request_per_mal));

            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

/*********************************************************************
* NAME:
*   arad_egr_q_static_cal_build_from_rates
* TYPE:
*   PROC
* FUNCTION:
*   generate a calendar by assign reserved slots for each physical port.
*   unused slots filled with ARAD_EGQ_NIF_PORT_CAL_BW_INVALID
* INPUT:
*   SOC_SAND_IN  uint32                   ports_rates[ARAD_NIF_NOF_NIFS] -
*   array of the ports rates in gbits per sec
*   SOC_SAND_OUT SOC_SAND_OUT ARAD_OFP_RATES_CAL_SCH         *calendar-
*   the calendar built by the function
*   
* REMARKS:
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
STATIC uint32
  arad_egr_q_static_cal_build_from_rates(
         SOC_SAND_IN  uint32                   ports_rates[ARAD_NIF_NOF_NIFS],
         SOC_SAND_OUT SOC_SAND_OUT ARAD_OFP_RATES_CAL_SCH         *calendar
    )
{
  /* this array built to keep the maximum distance between CAUI,XLAUI and RXAUI diffrent
     slots in the calendar minimal
     the quad i is in the place arrengment[i] for each quads iteration
   */
  /* [0,4,3,1,5,2,6,7]*/
  static uint32 regular_calendar_arrangement[ARAD_NIF_NOF_NIFS/4] = {0, 3, 5 , 2, 1, 4, 6, 7};
  /* [0,4,1,5,2,3,6,7]*/
  static uint32 ilkn16_and_up_calander_arrengment[ARAD_NIF_NOF_NIFS/4] = {0, 2, 4 , 5, 1, 3, 6, 7};
  uint32 *calendar_config;
   /*we use this for case  to keep the maximum distance minimal*/
  uint32 ktransitions[] = {2,3,1,0};
  uint32 internal_quad_mapping[] = {0,2,1,3};
  uint32 i, j, k;
  uint32 num_of_slots;
  uint32 has_ilkn16_and_up_calander = 0;

  /*reset the calendar*/
  for(i = 0 ; i < ARAD_EGR_MAX_CAL_SIZE ; i++)
  {
       calendar->slots[i] = ARAD_EGQ_NIF_PORT_CAL_BW_INVALID;
  }

  for(i = 0 ; i < ARAD_NIF_NOF_NIFS ; i++)
  {
      if(ports_rates[i] >= 160) {
          has_ilkn16_and_up_calander = 1;
          break;
      }
  }

  calendar_config =  (has_ilkn16_and_up_calander)? ilkn16_and_up_calander_arrengment: regular_calendar_arrangement;

  for(i = 0 ; i < ARAD_NIF_NOF_NIFS ; i++)
  {
    /*we assume each 10 gig needs physical port*/
    num_of_slots = ports_rates[i]/10;
    /*handle low BW ports*/
    num_of_slots *= 4;
    if((num_of_slots == 0) && (ports_rates[i] > 0))
    {
      num_of_slots = ports_rates[i]*10/25;
      if((ports_rates[i]*10) % 25 != 0)
      {
        num_of_slots++;
      }
    }
    j = i;
    k = 0;
    /*calendar slots positioning*/
    while(num_of_slots > 0)
    {
      calendar->slots[calendar_config[j/4]+ internal_quad_mapping[j%4]*8 + ARAD_NIF_NOF_NIFS*k] =  i;
      num_of_slots --;
      k = ktransitions[k];
      if(k == 0)
      {
        /* ILKN > 12 handling*/
        if(i==0 && j>= 11){
          j = (j==11 ? 27 : j-1);
        }
        /*other cases*/
        else if (j < ARAD_NIF_NOF_NIFS - 1) {
          j++;
        }
      }
    }
  }
  return SOC_E_NONE;
}


/*********************************************************************
* NAME:
*     arad_egr_q_nif_cal_set_unsafe
* TYPE:
*   PROC
* DATE:
* FUNCTION:
*         Set the calender of the nif port "processing time" i.e every 2 clk there
*         is a context switch and different nif get "service" or others which include
          delete and recycling.
* INPUT:
*         SOC_SAND_IN int unit
*
* REMARKS:
*         This function consist of four steps:
*         1. Classifying the nif, mal configuration
*         2. Giving each nif  weight according to its type
*         3. Adding the "others" processes of recycling, deleting as competitor with a given value
*         4. Computing the optimal calender using "black box" soc_pb_ofp_rates_from_rates_to_calendar function
*         5. Using the values computed in 4. to initialize ARAD_EGQ_FQP_NIF_PORT_MUX_TBL address:
*
*         The calender should admit the constraint derived from the formula BW=(128 * 8)/(ClkFreq * # Clk )
*         => # Clk = (128 * 8)/(ClkFreq * BW) Given that the clock frequency is 3.3 Ns
*         WE get #Clk= (128 * 8) /(3.3 * BW)
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

int
  arad_egr_q_nif_cal_set_unsafe(
    SOC_SAND_IN int unit
  )
{
    uint32
        res = SOC_SAND_OK;
    uint32
        ind,
        nif_id = 0,
        lanes;
    soc_port_if_t intf;
    ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA
        nif_port_mux_tbl;
    ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA
        pqp_nif_port_mux_tbl;
    ARAD_NIF_PORT_RATES
        nif_rates;
    uint32
        fqp_ports_rates[ARAD_EGR_MAX_CAL_SIZE],
        pqp_ports_rates[ARAD_EGR_MAX_CAL_SIZE],
        fqp_total_credit_bandwidth = 0,
        pqp_total_credit_bandwidth = 0,
        calendar_len = 0;
    uint32 other_pqp_to_fill;
    ARAD_OFP_RATES_CAL_SCH
        *calendar = NULL;
    uint32
        if_rate_mbps;
    uint32
        port,
        is_master,
        flags,
        offset;
    soc_pbmp_t
        port_bm;
    uint32 
        phy_port,
        is_if_0_ilkn_tdm = FALSE,
        is_if_16_ilkn_tdm = FALSE;
    soc_pbmp_t
        ports_bm;
    soc_port_t
        port_i;
    soc_port_if_t 
        nif_type;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
        ilkn_tdm_dedicated_queuing;
  
    SOCDNX_INIT_FUNC_DEFS;

    calendar = (ARAD_OFP_RATES_CAL_SCH*)sal_alloc(sizeof(ARAD_OFP_RATES_CAL_SCH),"calendar egr_q_nif_cal");                  
    if (calendar == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("calendar egr_q_nif_cal allocation failed")));
    }                                                                     
    sal_memset(calendar, 0x0, sizeof(ARAD_OFP_RATES_CAL_SCH));                                      

    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    /* 1. Classifying the nif configuration */
    ARAD_NIF_PORT_RATES_clear(&nif_rates);

    /* Add all valid interfaces */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));

    SOC_PBMP_ITER(port_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
            /* Statistics interface is not required for Egress */
            continue;
        }
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(!is_master) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
        nif_id = phy_port-1;

        if (if_rate_mbps < 1000) {
            nif_rates.nif_prt_rts[nif_id].rate = ARAD_EGR_SGMII_RATE;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &lanes));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &intf));

            if(intf == SOC_PORT_IF_CAUI){
                nif_rates.nif_prt_rts[nif_id].rate = 120;
            } else if(intf == SOC_PORT_IF_TM_INTERNAL_PKT){
                nif_rates.nif_prt_rts[nif_id].rate = 200;
            } else if(if_rate_mbps > 10000* lanes){
                nif_rates.nif_prt_rts[nif_id].rate = 10*lanes;
            } else {
                nif_rates.nif_prt_rts[nif_id].rate = if_rate_mbps / 1000;
            }
        }
        nif_rates.nof_nifs++;
    }

    for (ind = 0; ind < ARAD_EGR_MAX_CAL_SIZE ; ind++ )
    {
        fqp_ports_rates[ind] = 0;
        pqp_ports_rates[ind] = 0;
    }
 
    /* 2. Giving each nif weight according to its type */
    for (ind = 0; ind < ARAD_NIF_NOF_NIFS ; ind++ )
    {
        fqp_ports_rates[ind] = nif_rates.nif_prt_rts[ind].rate;
        fqp_total_credit_bandwidth += nif_rates.nif_prt_rts[ind].rate;

        pqp_ports_rates[ind] = nif_rates.nif_prt_rts[ind].rate;
        pqp_total_credit_bandwidth += nif_rates.nif_prt_rts[ind].rate;
    }

    /* 3. Adding the "others" processes of recycling, deleting as competitor with a given value */  
    /* we use the last interface not used before and we use it for others*/
    fqp_ports_rates[ARAD_EGR_OTHERS_INDX] = ARAD_EGQ_NIF_OTHERS_DEF_VAL;
    fqp_total_credit_bandwidth += ARAD_EGQ_NIF_OTHERS_DEF_VAL;
  
    /* 4. Computing the optimal calender using "black box" arad_ofp_rates_from_rates_to_calendar function  */
    SOCDNX_IF_ERR_EXIT(arad_egr_q_static_cal_build_from_rates(fqp_ports_rates, calendar));
    calendar_len = ARAD_EGR_MAX_CAL_SIZE;

    /*TDM SP MODE CONFIGURATION*/
    if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
        /*For each ilkn_tdm port nif fqp MUX has a special id find which nifs are defined as ilkn*/
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
        SOC_PBMP_ITER(ports_bm, port_i) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &nif_type));
            if(nif_type == SOC_PORT_IF_ILKN){
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port_i, &offset));
                if (0 == offset){
                    is_if_0_ilkn_tdm = TRUE;
                } else {
                    is_if_16_ilkn_tdm = TRUE;
                }
            }
        }
    }

    /* 5. Using the values computed in 4. to initialize ARAD_EGQ_FQP_NIF_PORT_MUX_TBL */
    for (nif_id = 0; nif_id < calendar_len; nif_id++)
    {
        if((calendar->slots[nif_id] == ARAD_EGR_OTHERS_INDX) || (calendar->slots[nif_id] == ARAD_EGQ_NIF_PORT_CAL_BW_INVALID))
        {
            nif_port_mux_tbl.fqp_nif_port_mux = ARAD_EGQ_NIF_PORT_CAL_BW_GIVE_UPON_REQUEST;
        } 
        else 
        {
            /*Special ilkn tdm code { */
            if((0 == calendar->slots[nif_id]) && (is_if_0_ilkn_tdm)){
                nif_port_mux_tbl.fqp_nif_port_mux = 124;
            }
            else if((16 == calendar->slots[nif_id]) && (is_if_16_ilkn_tdm)){
                nif_port_mux_tbl.fqp_nif_port_mux = 123;
            }
            /*Special ilkn tdm code }*/
            else {
                nif_port_mux_tbl.fqp_nif_port_mux = calendar->slots[nif_id];
            }
        }
         
        /* Set FQP table */
        res = arad_egq_fqp_nif_port_mux_tbl_set_unsafe(
            unit,
            nif_id,
            &nif_port_mux_tbl
            );
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* 5.1 Enable requrets for FQP BW by "OTHERS" and Disable Higher request for 100g ports or higher*/
    SOCDNX_IF_ERR_EXIT(arad_egr_q_fqp_scheduler_config(unit));

    /* 6. Adding the "others" processes of recycling, CPU, OLP, OAM */
    pqp_ports_rates[ARAD_EGQ_IFC_CPU] = ARAD_EGQ_NIF_PQP_CPU_DEF_VAL;
    pqp_total_credit_bandwidth += ARAD_EGQ_NIF_PQP_CPU_DEF_VAL;

    pqp_ports_rates[ARAD_EGQ_IFC_OLP] = ARAD_EGQ_NIF_PQP_OLP_DEF_VAL;
    pqp_total_credit_bandwidth += ARAD_EGQ_NIF_PQP_OLP_DEF_VAL;

    pqp_ports_rates[ARAD_EGQ_IFC_RCY] = ARAD_EGQ_NIF_PQP_RCY_DEF_VAL;
    pqp_total_credit_bandwidth += ARAD_EGQ_NIF_PQP_RCY_DEF_VAL;

    pqp_ports_rates[ARAD_EGQ_IFC_OAMP] = ARAD_EGQ_NIF_PQP_OAM_DEF_VAL;
    pqp_total_credit_bandwidth += ARAD_EGQ_NIF_PQP_OAM_DEF_VAL;


    /* 7. Computing the optimal calender using "black box" arad_ofp_rates_from_rates_to_calendar function  */
    SOCDNX_IF_ERR_EXIT(arad_egr_q_static_cal_build_from_rates(pqp_ports_rates, calendar));
    calendar_len = ARAD_EGR_MAX_CAL_SIZE;

    /* 8. Using the values computed in 4. to initialize ARAD_EGQ_PQP_NIF_PORT_MUX_TBL */
    other_pqp_to_fill = ARAD_EGQ_IFC_CPU;
    for (nif_id = 0; nif_id < calendar_len; nif_id++)
    {
        /* Set PQP table */
        if(calendar->slots[nif_id] == ARAD_EGQ_NIF_PORT_CAL_BW_INVALID)
        {
            pqp_nif_port_mux_tbl.pqp_nif_port_mux = other_pqp_to_fill;
            other_pqp_to_fill = (other_pqp_to_fill == ARAD_EGQ_IFC_RCY )? ARAD_EGQ_IFC_CPU : other_pqp_to_fill+1;
        }
        else
        {
            pqp_nif_port_mux_tbl.pqp_nif_port_mux = calendar->slots[nif_id];
        }
        res = arad_egq_pqp_nif_port_mux_tbl_set_unsafe(
            unit,
            nif_id,
            &pqp_nif_port_mux_tbl
            );
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOC_FREE(calendar);
    SOCDNX_FUNC_RETURN;

}

/*********************************************************************
* NAME:
*     arad_egr_queuing_regs_init
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC int
  arad_egr_queuing_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
    uint32
        field_32,
        reg_32;
    uint8
        is_petrab_in_system;
    uint64
        reg_64;
    soc_reg_above_64_val_t
        reg_above_64,
        field_above_64;
    int
        core_id;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    /* Enable sending credits to various interfaces */    
    field_32 = 0x1;
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_CMICMr(unit, SOC_CORE_ALL,  field_32));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_OLPr(unit, SOC_CORE_ALL,  field_32));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_OAMr(unit, SOC_CORE_ALL,  field_32));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_RCYr(unit, SOC_CORE_ALL,  field_32));

    /* Delete FIFO enable */
    COMPILER_64_ZERO(reg_64);
  
    field_32 = 66;
    soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_ALMOSTFULLf, field_32);
    field_32 = SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast_low_priority;
    soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_ALMOSTFULL_1f, field_32);
    field_32 = SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_multicast;
    soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_ALMOSTFULL_2f, field_32);
    field_32 = SOC_DPP_CONFIG(unit)->tm.delete_fifo_almost_full_all;
    soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_ALMOSTFULL_3f, field_32);
    if(SOC_IS_JERICHO(unit)) {
        soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_ALMOSTFULL_4f, field_32);
    }
    field_32 = 42;
    soc_reg64_field32_set(unit,EGQ_DELETE_FIFO_CONFIGURATIONr,&reg_64,DELETE_FIFO_N_RDYf, field_32);

    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_DELETE_FIFO_CONFIGURATIONr(unit, SOC_CORE_ALL, reg_64));

    /* Set ports priority by default to be LOW priority */
    SHR_BITSET_RANGE(field_above_64, 0, ARAD_EGR_NOF_BASE_Q_PAIRS);
    soc_reg_above_64_field_set(unit, EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr, reg_above_64, PORT_PRIORITYf, field_above_64);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr(unit, SOC_CORE_ALL, reg_above_64));

    /* CNTXT FIFO Thresholds */
    reg_32 = 0;
    soc_reg_field_set(unit, EGQ_CONTEXT_FIFO_THRESHOLD_VALUESr, &reg_32, CNTXT_FULL_THf, 42);
    soc_reg_field_set(unit, EGQ_CONTEXT_FIFO_THRESHOLD_VALUESr, &reg_32, CNTXT_ALMOST_FULL_THf, 84);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_CONTEXT_FIFO_THRESHOLD_VALUESr(unit, SOC_CORE_ALL, reg_32));

    is_petrab_in_system = arad_sw_db_is_petrab_in_system_get(unit);

    if (is_petrab_in_system) 
    {
        /* enable 64K for Petra-B compatible */
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_GLOBAL_CONFIGr(unit, core_id, &reg_32));
            soc_reg_field_set(unit, EGQ_GLOBAL_CONFIGr, &reg_32, EGRESS_MC_64K_MODEf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GLOBAL_CONFIGr(unit, core_id, reg_32));

            SOCDNX_IF_ERR_EXIT(READ_EPNI_GLOBAL_CONFIGr(unit, core_id, &reg_32));
            soc_reg_field_set(unit, EPNI_GLOBAL_CONFIGr, &reg_32, EGRESS_MC_64K_MODEf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_GLOBAL_CONFIGr(unit, core_id, reg_32));

            SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_REPLICATION_MCID_MASKr(unit, core_id, &reg_32));
            soc_reg_field_set(unit, EGQ_EGRESS_REPLICATION_MCID_MASKr, &reg_32, MCID_MASKf, 0x3fff);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_REPLICATION_MCID_MASKr(unit, core_id, reg_32));

        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Set scheduled drop threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_unsched_drop_q_pair_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  uint32                drop_precedence,
    SOC_SAND_IN  ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_Q_PAIR_THRESH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);

  if(thresh_info->packet_descriptors > EGQ_QDCT_PD_MAX_VAL)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_EGR_QDCT_PD_VALUE_OUT_OF_RANGE, 5, exit);
  }

  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* Write PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1350, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_MC_PD_TH_DP_0f + drop_precedence, thresh_info->packet_descriptors);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1360, exit, WRITE_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));

  /* Write Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1370, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QQST_TABLEm_field32_set(unit, data, QUEUE_MC_DB_DP_0_THf + drop_precedence, thresh_info->dbuff);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1380, exit, WRITE_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_q_pair_thresh_set_unsafe()",egress_tc,threshold_type); 
}

/* 
 * Get threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_unsched_drop_q_pair_thresh_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  uint32                drop_precedence,
    SOC_SAND_OUT ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_Q_PAIR_THRESH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);
  
  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* READ PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1480, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->packet_descriptors = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_MC_PD_TH_DP_0f + drop_precedence);
  
  /* READ Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1490, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->dbuff = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_MC_DB_DP_0_THf + drop_precedence);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_q_pair_thresh_get_unsafe()",egress_tc,threshold_type); 
}

/* 
 * Set OFP FC threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint32                 egress_tc,
      SOC_SAND_IN  uint32                 threshold_type,
      SOC_SAND_IN  ARAD_EGR_THRESH_INFO      *thresh_info
    )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];   
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_FC_Q_PAIR_THRESH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);

  if(thresh_info->packet_descriptors > EGQ_QDCT_PD_MAX_VAL)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_EGR_QDCT_PD_VALUE_OUT_OF_RANGE, 5, exit);
  }


  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* Write PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1800, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_UC_PD_FC_THf , thresh_info->packet_descriptors);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1810, exit, WRITE_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));

  /* Write Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1820, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QQST_TABLEm_field32_set(unit, data, QUEUE_UC_DB_FC_THf , thresh_info->dbuff);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1830, exit, WRITE_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_q_pair_thresh_set_unsafe()",egress_tc,threshold_type); 
}

/* 
 * Get OFP FC threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_ofp_fc_q_pair_thresh_get_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint32                 egress_tc,
      SOC_SAND_IN  uint32                 threshold_type,
      SOC_SAND_OUT ARAD_EGR_THRESH_INFO      *thresh_info
    )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];   
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_FC_Q_PAIR_THRESH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);
  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* Write PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1930, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->packet_descriptors = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_UC_PD_FC_THf);

  /* READ Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1940, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->dbuff = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_UC_DB_FC_THf);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_q_pair_thresh_get_unsafe()",egress_tc,threshold_type); 
}

/* 
 * Set scheduled drop threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_sched_drop_q_pair_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_Q_PAIR_THRESH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);

  if(thresh_info->packet_descriptors > EGQ_QDCT_PD_MAX_VAL)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_EGR_QDCT_PD_VALUE_OUT_OF_RANGE, 5, exit);
  }

  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* Write PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_UC_PD_DIS_THf , thresh_info->packet_descriptors);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, WRITE_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));

  /* Write Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  soc_EGQ_QQST_TABLEm_field32_set(unit, data, QUEUE_UC_DB_DIS_THf , thresh_info->dbuff);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_q_pair_thresh_set_unsafe()",egress_tc,threshold_type); 
}

/* 
 * Get threshold per q-pair (egress_tc) and threshold type. 
 * Thresholds are of dbuff (256B) and packet descriptors (pd) 
 */
STATIC
  uint32
    arad_egr_sched_drop_q_pair_thresh_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_OUT ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset,
    data[SOC_SAND_MAX(ARAD_EGQ_QDCT_TBL_ENTRY_SIZE,ARAD_EGQ_QQST_TBL_ENTRY_SIZE)];
  uint32 res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_Q_PAIR_THRESH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh_info);

  /* QDCT offset */
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  /* READ PD threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1240, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->packet_descriptors = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_UC_PD_DIS_THf);
  
  /* READ Dbuff threshold */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1250, exit, READ_EGQ_QQST_TABLEm(unit, MEM_BLOCK_ANY, offset, data));
  thresh_info->dbuff = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_UC_DB_DIS_THf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_q_pair_thresh_get_unsafe()",egress_tc,threshold_type); 
}
  
/*********************************************************************
* NAME:
*     arad_egr_queuing_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
int
  arad_egr_queuing_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN          ARAD_EGR_QUEUING_PARTITION_SCHEME    scheme
  )
{
    uint32
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        egq_ppct_tbl_data;
    uint32
        ps_default_priority,
        ps;
    uint64
        data;
    uint32
        data32[2];
    int 
        core_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_egr_queuing_regs_init(unit));

    /* 
     *  Internal mapping between base_q_pair and its according q_pair num.
     *  Must be 1:1 mapping (HW limitation)
     */
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
        for (base_q_pair = 0; base_q_pair < ARAD_EGR_NOF_BASE_Q_PAIRS; base_q_pair++)
        {
            SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_get_unsafe(unit, base_q_pair, core_id, &egq_ppct_tbl_data));
            egq_ppct_tbl_data.base_q_pair_num = base_q_pair;
            egq_ppct_tbl_data.cgm_interface = ARAD_EGQ_IFC_DEF_VAL;
            egq_ppct_tbl_data.cos_map_profile = 0;
            SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_set_unsafe(unit, base_q_pair, core_id, &egq_ppct_tbl_data));
        }
    }

    ps_default_priority = ARAD_EGQ_PS_DEF_PRIORITY;
    data32[0] = 0;
    data32[1] = 0;
    for (ps = 0; ps < ARAD_EGR_NOF_PS; ps++) {
        SHR_BITCOPY_RANGE(data32,ps*ARAD_EGQ_PS_PRIORITY_NOF_BITS,&ps_default_priority,0,ARAD_EGQ_PS_PRIORITY_NOF_BITS);
    }       
    COMPILER_64_SET(data, data32[1], data32[0]);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PS_MODEr(unit, SOC_CORE_ALL, data));
  

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_egr_q_nif_cal_set,(unit)));

    /* Set the Egress shared resource mode */
    SOCDNX_IF_ERR_EXIT(arad_egr_queuing_partition_scheme_set_unsafe(unit, scheme));

    
    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      ARAD_EGR_FC_DEVICE_THRESH dev_conf, exact_dev_conf;
      ARAD_EGR_THRESH_INFO thresh_info;
      ARAD_EGR_QUEUING_DEV_TH device_thresh;
      ARAD_EGR_FC_OFP_THRESH ofp_conf, exact_ofp_conf;
      ARAD_EGR_FC_CHNIF_THRESH chnif_conf, exact_chnif_conf;
      ARAD_EGR_QUEUING_IF_UC_FC if_uc_info;
      int res, drop_precedence, egress_tc, threshold_type;
      int mc_if_profile_ndx, uc_if_profile_ndx;

        /*
       *  Device Thresholds, currently set to maximal value
       */
      arad_ARAD_EGR_FC_DEVICE_THRESH_clear(&dev_conf);
      arad_ARAD_EGR_FC_DEVICE_THRESH_clear(&exact_dev_conf);
      dev_conf.global.buffers = 0x3fff;
      dev_conf.global.descriptors = 0x7fff;
      dev_conf.scheduled.buffers = 0x3fff;
      dev_conf.scheduled.descriptors = 0x7fff;
      dev_conf.unscheduled_pool[0].buffers = 0x3fff;
      dev_conf.unscheduled_pool[1].buffers = 0x3fff;
      dev_conf.unscheduled.descriptors = 0x7fff;
      dev_conf.unscheduled.buffers = 0x3fff;
      dev_conf.unscheduled_pool[0].descriptors = 0x7fff;
      dev_conf.unscheduled_pool[1].descriptors = 0x7fff;
      res = arad_egr_dev_fc_set_unsafe(unit, &dev_conf, &exact_dev_conf);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      /*
       *  OFP Thresholds
       */
      arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);
      thresh_info.dbuff = 0x3ffff;
      thresh_info.packet_descriptors = 0x600;
      for(egress_tc = 0; egress_tc < ARAD_NOF_TRAFFIC_CLASSES; ++egress_tc) {
        for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
          for(drop_precedence = 0; drop_precedence < ARAD_NOF_DROP_PRECEDENCE; ++drop_precedence) {
            res = arad_egr_unsched_drop_q_pair_thresh_set_unsafe(
                    unit,
                    egress_tc,
                    threshold_type,
                    drop_precedence,
                    &thresh_info
                  );
            SOCDNX_SAND_IF_ERR_EXIT(res);
          }
        }
      }

      thresh_info.dbuff = 0x800;
      thresh_info.packet_descriptors = 0x400;
      for(egress_tc = 0; egress_tc < ARAD_NOF_TRAFFIC_CLASSES; ++egress_tc) {
        for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
          res = arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
                  unit,
                  egress_tc,
                  threshold_type,
                  &thresh_info
                );
          SOCDNX_SAND_IF_ERR_EXIT(res);
        }
      }

      thresh_info.dbuff = 0xb00;
      thresh_info.packet_descriptors = 0x600;
      for(egress_tc = 0; egress_tc < ARAD_NOF_TRAFFIC_CLASSES; ++egress_tc) {
        for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
          res = arad_egr_sched_drop_q_pair_thresh_set_unsafe(
                  unit,
                  egress_tc,
                  threshold_type,
                  &thresh_info
                );
          SOCDNX_SAND_IF_ERR_EXIT(res);
        }
      }

      res = arad_egr_queuing_dev_get_unsafe(
              unit,
              &device_thresh
            );
      SOCDNX_SAND_IF_ERR_EXIT(res);

      /* On default, reserved should be empty */
      for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
        for(egress_tc = 0; egress_tc < ARAD_NOF_TRAFFIC_CLASSES; ++egress_tc) {
          device_thresh.thresh_type[threshold_type].reserved[egress_tc].descriptors = 0x000;
        }
      }

      for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
        device_thresh.thresh_type[threshold_type].uc.buffers = 0xa00;
        device_thresh.thresh_type[threshold_type].uc.descriptors = 0x600;
        device_thresh.thresh_type[threshold_type].mc_shared.buffers = 0x3ffff;
        device_thresh.thresh_type[threshold_type].mc_shared.descriptors = 0x600;
      }

      res = arad_egr_queuing_dev_set_unsafe(
              unit,
              &device_thresh
            );
      SOCDNX_SAND_IF_ERR_EXIT(res);

      for(threshold_type = 0; threshold_type < ARAD_NOF_THRESH_TYPES; ++threshold_type) {
        res = arad_egr_ofp_fc_get_unsafe(
                unit,
                ARAD_EGR_Q_PRIO_ALL,
                threshold_type,
                &ofp_conf
              );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        ofp_conf.data_buffers = 0x800;
        ofp_conf.packet_descriptors = 0x400;

        res = arad_egr_ofp_fc_set_unsafe(
                unit,
                ARAD_EGR_Q_PRIO_ALL,
                threshold_type,
                &ofp_conf,
                &exact_ofp_conf
              );
        SOCDNX_SAND_IF_ERR_EXIT(res);
      }

       arad_ARAD_EGR_FC_CHNIF_THRESH_clear(&chnif_conf);
      arad_ARAD_EGR_FC_CHNIF_THRESH_clear(&exact_chnif_conf);

      for (mc_if_profile_ndx = 0; mc_if_profile_ndx < CGM_MC_INTERFACE_PD_TH_NOF_FIELDS; mc_if_profile_ndx++) {
          res = arad_egr_queuing_if_fc_mc_set_unsafe(
                    unit,
                    mc_if_profile_ndx,
                    0x7fff
                  );
          SOCDNX_SAND_IF_ERR_EXIT(res);
      }

      for (uc_if_profile_ndx = 0; uc_if_profile_ndx < CGM_UC_INTERFACE_TH_NOF_FIELDS; uc_if_profile_ndx++) {
          res = arad_egr_queuing_if_fc_uc_get_unsafe(
                    unit,
                    uc_if_profile_ndx,
                    &if_uc_info
                );
          SOCDNX_SAND_IF_ERR_EXIT(res);

          if_uc_info.pd_th = 0x7FFF;
          if_uc_info.size256_th = 0x3FFF;

          res = arad_egr_queuing_if_fc_uc_set_unsafe(
                    unit,
                    uc_if_profile_ndx,
                    &if_uc_info
                  );
          SOCDNX_SAND_IF_ERR_EXIT(res);
      }
    }


exit:
    SOCDNX_FUNC_RETURN;
}


/* Mapping from Queue-pair to OTM-Port and TC */
uint32
  arad_egr_queuing_q_pair_port_tc_find(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                        q_pair,
    SOC_SAND_OUT uint8                        *found,
    SOC_SAND_OUT ARAD_FAP_PORT_ID               *ofp_idx,
    SOC_SAND_OUT uint32                          *tc
  )
{
  uint32
      tc_lcl = 0;
  uint32
    base_q_pair,
    nof_priorities,
    res,
    port_i;
  uint8
    found_lcl = FALSE;
  soc_pbmp_t
    pbmp;
  uint32 
    ofp_ndx = 0;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_Q_PAIR_PORT_TC_FIND);

  /* Find a port where the Queue-pair is inside, and then compute the TC */
  found_lcl = FALSE;

  res = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

  SOC_PBMP_ITER(pbmp, port_i)
  {
      res = soc_port_sw_db_base_q_pair_get(unit, port_i, &base_q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

      res = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

      if ((q_pair >= base_q_pair) && (q_pair < base_q_pair + nof_priorities)) {
          found_lcl = TRUE;
          tc_lcl = q_pair - base_q_pair;

          res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &ofp_ndx, &core);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

          break;
      }
  }

  *found = found_lcl;
  *ofp_idx = ofp_ndx;
  *tc = tc_lcl;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_q_pair_port_tc_find()", q_pair, 0);
}


/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_thresh_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID    tm_port,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;
  ARAD_EGQ_PPCT_TBL_DATA
    ppct_tbl_data;
  ARAD_EGQ_PCT_TBL_DATA
    pct_tbl_data;
  uint32
    base_q_pair,
    nof_pairs,
    curr_q_pair;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_THRESH_TYPE_SET_UNSAFE);
  
  /* Retreive base_q_pair */
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, 0, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, tm_port, 0, &nof_pairs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = arad_egq_ppct_tbl_get_unsafe(
          unit,
          base_q_pair,
          0,
          &ppct_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  ppct_tbl_data.cgm_port_profile = ofp_thresh_type;

  res = arad_egq_ppct_tbl_set_unsafe(
          unit,
          base_q_pair,
          SOC_CORE_ALL,
          &ppct_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit); 
     
  /* PCT is per q_pair */
  for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
  {
    res = arad_egq_pct_tbl_get_unsafe(
          unit,
          curr_q_pair,
          0,
          &pct_tbl_data
        );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    pct_tbl_data.port_profile = ofp_thresh_type;

    res = arad_egq_pct_tbl_set_unsafe(
            unit,
            curr_q_pair,
            0,
            &pct_tbl_data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_thresh_type_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_thresh_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_THRESH_TYPE_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_thresh_type, ARAD_EGR_PORT_NOF_THRESH_TYPES-1,
    ARAD_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_thresh_type_verify()",0,0);
}

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_thresh_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  )
{
  uint32
    res;
  uint32
    base_q_pair;
  ARAD_EGQ_PPCT_TBL_DATA
    tbl_data;
  ARAD_EGR_PORT_THRESH_TYPE  
    thresh_type_val;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_THRESH_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ofp_thresh_type);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  /* Retreive base_q_pair */
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_egq_ppct_tbl_get_unsafe(
          unit,
          base_q_pair,
          0,
          &tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  
  thresh_type_val = tbl_data.cgm_port_profile;

  *ofp_thresh_type = (ARAD_EGR_PORT_THRESH_TYPE)thresh_type_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_thresh_type_get_unsafe()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_sched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  uint32
    egress_tc; 
  ARAD_EGR_PORT_THRESH_TYPE
    thresh_type;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh); 

  /* API is used only for ports with 2 q-pairs only */
  /* For other types of q-pairs (1 or 8), refer to (? - need to be done) */
  egress_tc = prio_ndx;

  for (thresh_type = ARAD_EGR_PORT_THRESH_TYPE_0; thresh_type < ARAD_EGR_PORT_NOF_THRESH_TYPES; thresh_type++)
  {     
     arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);
     thresh_info.dbuff = thresh->queue_words_consumed[thresh_type]; /* queue_words_consumed parameter used for Data buffers */
     thresh_info.packet_descriptors = thresh->queue_pkts_consumed[thresh_type];
     res = arad_egr_sched_drop_q_pair_thresh_set_unsafe(
             unit,
             egress_tc,
             thresh_type,
             &thresh_info
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

     exact_thresh->queue_words_consumed[thresh_type] = thresh_info.dbuff;/* queue_words_consumed parameter used for Data buffers in Arad */
     exact_thresh->queue_pkts_consumed[thresh_type] = thresh_info.packet_descriptors;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_set_unsafe()",prio_ndx,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_sched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);
  if (prio_ndx != ARAD_EGR_Q_PRIO_ALL)
  {
      SOC_SAND_ERR_IF_ABOVE_MAX(
      prio_ndx, ARAD_EGR_NOF_Q_PRIO-1,
      ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
      );
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_verify()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_sched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  uint32
    egress_tc;
  ARAD_EGR_PORT_THRESH_TYPE
    thresh_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh); 

  egress_tc = prio_ndx;

  for (thresh_type = ARAD_EGR_PORT_THRESH_TYPE_0; thresh_type < ARAD_EGR_PORT_NOF_THRESH_TYPES; thresh_type++)
  {     
     arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);

     res = arad_egr_sched_drop_q_pair_thresh_get_unsafe(
             unit,
             egress_tc,
             thresh_type,
             &thresh_info             
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

     thresh->queue_words_consumed[thresh_type] = thresh_info.dbuff;/* queue_words_consumed parameter used for Data buffers in Arad */
     thresh->queue_pkts_consumed[thresh_type] = thresh_info.packet_descriptors;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_get_unsafe()",0,0);
}

uint32
  arad_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO           prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  uint32
    egress_tc;
  ARAD_EGR_PORT_THRESH_TYPE
    thresh_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh); 

  egress_tc = prio_ndx;

  for (thresh_type = ARAD_EGR_PORT_THRESH_TYPE_0; thresh_type < ARAD_EGR_PORT_NOF_THRESH_TYPES; thresh_type++)
  {     
     arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);
     thresh_info.dbuff = thresh->queue_words_consumed[thresh_type];/* queue_words_consumed parameter used for Data buffers in Arad */
     thresh_info.packet_descriptors = thresh->queue_pkts_consumed[thresh_type];
     res = arad_egr_unsched_drop_q_pair_thresh_set_unsafe(
             unit,
             egress_tc,
             thresh_type,
             dp_ndx,
             &thresh_info
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

     exact_thresh->queue_words_consumed[thresh_type] = thresh_info.dbuff;/* queue_words_consumed parameter used for Data buffers in Arad */
     exact_thresh->queue_pkts_consumed[thresh_type] = thresh_info.packet_descriptors;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_set_unsafe()",prio_ndx,dp_ndx);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_unsched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  if (prio_ndx != ARAD_EGR_Q_PRIO_ALL)
  {
      SOC_SAND_ERR_IF_ABOVE_MAX(
      prio_ndx, ARAD_EGR_NOF_Q_PRIO-1,
      ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
      );
  }

  res = arad_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_verify()",0,0);
}

uint32
  arad_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  uint32
    egress_tc;
  ARAD_EGR_PORT_THRESH_TYPE
    thresh_type;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  if (prio_ndx != ARAD_EGR_Q_PRIO_ALL){
      SOC_SAND_ERR_IF_ABOVE_MAX(
      prio_ndx, ARAD_EGR_NOF_Q_PRIO-1,
      ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
      );
  }

  res = arad_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  egress_tc = prio_ndx;

  for (thresh_type = ARAD_EGR_PORT_THRESH_TYPE_0; thresh_type < ARAD_EGR_PORT_NOF_THRESH_TYPES; thresh_type++)
  {     
     arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);

     res = arad_egr_unsched_drop_q_pair_thresh_get_unsafe(
             unit,
             egress_tc,
             thresh_type,
             dp_ndx,
             &thresh_info
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

     thresh->queue_words_consumed[thresh_type] = thresh_info.dbuff;/* queue_words_consumed parameter used for Data buffers in Arad */
     thresh->queue_pkts_consumed[thresh_type] = thresh_info.packet_descriptors;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_get_unsafe()",prio_ndx,dp_ndx);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_dev_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *exact_thresh
  )
{
  soc_reg_above_64_val_t
    data,
    field_val;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DEV_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1590, exit, READ_CGM_CGM_GENERAL_FC_THr(unit, data));
  
  /* Global { */
  /* Global FC DBuff */
  SOC_REG_ABOVE_64_CLEAR(field_val);
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->global.buffers),0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, TOTAL_DB_FC_THf,field_val);
  exact_thresh->global.buffers = thresh->global.buffers;

  /* Global FC PD */
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->global.descriptors),0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, TOTAL_PD_FC_THf,field_val);
  exact_thresh->global.descriptors = thresh->global.descriptors;

  /* Global } */

  /* UC { */
  /* UC FC DBuff */
  SOC_REG_ABOVE_64_CLEAR(field_val);
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->scheduled.buffers),0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, UC_DB_FC_THf, field_val);
  exact_thresh->scheduled.buffers = thresh->scheduled.buffers;

  /* UC FC PD */
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->scheduled.descriptors),0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, UC_PD_FC_THf,field_val);
  exact_thresh->scheduled.descriptors = thresh->scheduled.descriptors;

  /* UC } */

  /* MC { */
  /* MC FC DBuff */
  SOC_REG_ABOVE_64_CLEAR(field_val);
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled.buffers),0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, MC_DB_SP_FC_THf, field_val);
  exact_thresh->unscheduled.buffers = thresh->unscheduled.buffers;

  /* MC FC PD */
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled.descriptors),0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, MC_PD_SP_FC_THf,field_val);
  exact_thresh->unscheduled.descriptors = thresh->unscheduled.descriptors;

  /* CFG MC SP0 FC DBuff */
  SOC_REG_ABOVE_64_CLEAR(field_val);
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled_pool[0].buffers),0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_DB_SP_0_FC_THf, field_val);
  exact_thresh->unscheduled_pool[0].buffers = thresh->unscheduled_pool[0].buffers;

  /* CFG MC SP0 FC PD */
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled_pool[0].descriptors),0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_PD_SP_0_FC_THf,field_val);
  exact_thresh->unscheduled_pool[0].descriptors = thresh->unscheduled_pool[0].descriptors;

  /* CFG MC SP1 FC DBuff */
  SOC_REG_ABOVE_64_CLEAR(field_val);
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled_pool[1].buffers),0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_DB_SP_1_FC_THf, field_val);
  exact_thresh->unscheduled_pool[1].buffers = thresh->unscheduled_pool[1].buffers;

  /* CFG MC SP0 FC PD */
  SHR_BITCOPY_RANGE(field_val,0,&(thresh->unscheduled_pool[1].descriptors),0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_PD_SP_1_FC_THf,field_val);
  exact_thresh->unscheduled_pool[1].descriptors = thresh->unscheduled_pool[1].descriptors;

  /* MC } */

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1600, exit, WRITE_CGM_CGM_GENERAL_FC_THr(unit, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_dev_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_dev_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DEV_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_MAGIC_NUM_VERIFY(thresh);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_dev_fc_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_dev_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *thresh
  )
{
  soc_reg_above_64_val_t
    data,
    field_val;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DEV_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  arad_ARAD_EGR_FC_DEVICE_THRESH_clear(thresh);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1700, exit, READ_CGM_CGM_GENERAL_FC_THr(unit, data));
  /* General { */
  /* General Dbuff */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, TOTAL_DB_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->global.buffers),0,field_val,0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);

  /* General PD */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, TOTAL_PD_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->global.descriptors),0,field_val,0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* General } */

  /* UC { */
  /* UC Dbuff */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, UC_DB_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->scheduled.buffers),0,field_val,0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);

  /* UC PD */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, UC_PD_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->scheduled.descriptors),0,field_val,0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* UC } */

  /* MC { */
  /* MC FC DBuff */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, MC_DB_SP_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled.buffers),0,field_val,0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  
  /* MC FC PD */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, MC_PD_SP_FC_THf,field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled.descriptors),0,field_val,0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* CFG MC SP0 FC DBuff */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_DB_SP_0_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled_pool[0].buffers),0,field_val,0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);

  /* CFG MC SP0 FC PD */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_PD_SP_0_FC_THf,field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled_pool[0].descriptors),0,field_val,0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* CFG MC SP1 FC DBuff */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_DB_SP_1_FC_THf, field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled_pool[1].buffers),0,field_val,0,ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);

  /* CFG MC SP0 FC PD */
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_FC_THr, data, CFG_MC_PD_SP_1_FC_THf,field_val);
  SHR_BITCOPY_RANGE(&(thresh->unscheduled_pool[1].descriptors),0,field_val,0,ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* MC } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_dev_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *exact_thresh
  )
{
  uint32
    res;
  uint32
    egress_tc;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  soc_reg_above_64_val_t
    reg_pd,
    field_pd,
    reg_db,
    field_db,
    mem;
  uint32
    field_32;
        
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  SOC_REG_ABOVE_64_CLEAR(reg_pd);
  SOC_REG_ABOVE_64_CLEAR(field_pd);
  SOC_REG_ABOVE_64_CLEAR(reg_db);
  SOC_REG_ABOVE_64_CLEAR(field_db);
  SOC_REG_ABOVE_64_CLEAR(mem);

  arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);

  if(prio_ndx == ARAD_EGR_Q_PRIO_ALL) {
    /* CGM_CGM_MC_PD_TC_FC_THr */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2040, exit, READ_CGM_CGM_MC_PD_TC_FC_THr(unit, REG_PORT_ANY, reg_pd));
    SHR_BITCOPY_RANGE(field_pd, 0, &thresh->mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

    /* CGM_CGM_MC_DB_TC_FC_THr */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2050, exit, READ_CGM_CGM_MC_DB_TC_FC_THr(unit, reg_db));
    SHR_BITCOPY_RANGE(field_db, 0, &thresh->mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);

    switch(ofp_type_ndx) {
      case 0:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_0f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_0f, field_db);
        break;
      case 1:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_1f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_1f, field_db);
        break;
      case 2:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_2f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_2f, field_db);
        break;
      case 3:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_3f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_3f, field_db);
        break;
      case 4:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_4f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_4f, field_db);
        break;
      case 5:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_5f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_5f, field_db);
        break;
      case 6:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_6f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_6f, field_db);
        break;
      case 7:
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_7f, field_pd);
        soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_7f, field_db);
        break;
      default:
          break;
    }
    if(ofp_type_ndx < 8) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2060, exit, WRITE_CGM_CGM_MC_PD_TC_FC_THr(unit, REG_PORT_ANY, reg_pd));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 2070, exit, WRITE_CGM_CGM_MC_DB_TC_FC_THr(unit, reg_db));
    }

    /* EGQ_PDCT_TABLEm */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2080, exit, READ_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));
    field_32 = thresh->packet_descriptors;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_FC_THf, &field_32);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2090, exit, WRITE_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));

    /* EGQ_PQST_TABLEm */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2100, exit, READ_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));
    field_32 = thresh->data_buffers;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_FC_THf, &field_32);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2110, exit, WRITE_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));
  }
  else
  {
      egress_tc = prio_ndx;

      thresh_info.dbuff = thresh->words;/* queue_words_consumed parameter used for Data buffers in Arad */
      thresh_info.packet_descriptors = thresh->packet_descriptors;
  
      res = arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
               unit,
               egress_tc,
               ofp_type_ndx,
               &thresh_info
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      exact_thresh->words = thresh_info.dbuff;
      exact_thresh->packet_descriptors = thresh_info.packet_descriptors;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_set_unsafe()",prio_ndx,ofp_type_ndx);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  if (prio_ndx != ARAD_EGR_Q_PRIO_ALL){
      SOC_SAND_ERR_IF_ABOVE_MAX(
      prio_ndx, ARAD_EGR_NOF_Q_PRIO-1,
      ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
      );
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_type_ndx, ARAD_EGR_PORT_NOF_THRESH_TYPES-1,
    ARAD_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_verify()",prio_ndx,ofp_type_ndx);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  )
{
  uint32
    res;
  uint32
    egress_tc;
  ARAD_EGR_THRESH_INFO
    thresh_info;
  soc_reg_above_64_val_t
    reg_pd,
    field_pd,
    reg_db,
    field_db,
    mem;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  if (prio_ndx != ARAD_EGR_Q_PRIO_ALL) {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        prio_ndx, ARAD_EGR_NOF_Q_PRIO-1,
        ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
      );
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_type_ndx, ARAD_EGR_PORT_NOF_THRESH_TYPES-1,
    ARAD_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

  arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);

  if(prio_ndx == ARAD_EGR_Q_PRIO_ALL) {
    /* CGM_CGM_MC_PD_TC_FC_THr */
    SOC_REG_ABOVE_64_CLEAR(reg_pd);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2210, exit, READ_CGM_CGM_MC_PD_TC_FC_THr(unit, REG_PORT_ANY, reg_pd));
    
    /* CGM_CGM_MC_DB_TC_FC_THr */
    SOC_REG_ABOVE_64_CLEAR(reg_db);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2220, exit, READ_CGM_CGM_MC_DB_TC_FC_THr(unit, reg_db));

    switch(ofp_type_ndx) {
      case 0:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_0f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_0f, field_db);
        break;
      case 1:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_1f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_1f, field_db);
        break;
      case 2:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_2f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_2f, field_db);
        break;
      case 3:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_3f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_3f, field_db);
        break;
      case 4:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_4f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_4f, field_db);
        break;
      case 5:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_5f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_5f, field_db);
        break;
      case 6:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_6f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_6f, field_db);
        break;
      case 7:
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_TC_FC_THr, reg_pd, CGM_MC_PD_TC_FC_TH_7f, field_pd);
        soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_TC_FC_THr, reg_db, CGM_MC_DB_TC_FC_TH_7f, field_db);
        break;
      default:
          break;
    }
    if(ofp_type_ndx < 7) {
/*
 * COVERITY
 *
 * The variable field_pd is always initiallized when ofp_type_ndx < 7.
 */
/* coverity[uninit_use] */
      SHR_BITCOPY_RANGE(&thresh->mc.descriptors, 0, field_pd, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
      SHR_BITCOPY_RANGE(&thresh->mc.buffers, 0, field_db, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
    }

    /* EGQ_PDCT_TABLEm */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2230, exit, READ_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));
    soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_UC_PD_FC_THf, &thresh->packet_descriptors);
    
    /* EGQ_PQST_TABLEm */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2240, exit, READ_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, ofp_type_ndx, mem));
    soc_EGQ_PQST_TABLEm_field_get(unit, mem, PORT_UC_DB_FC_THf, &thresh->data_buffers);
  }
  else
  {
      egress_tc = prio_ndx;

      res = arad_egr_ofp_fc_q_pair_thresh_get_unsafe(
               unit,
               egress_tc,
               ofp_type_ndx,
               &thresh_info           
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      thresh->words = thresh_info.dbuff;/* queue_words_consumed parameter used for Data buffers in Arad */
      thresh->packet_descriptors = thresh_info.packet_descriptors;
  } 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_get_unsafe()",prio_ndx,ofp_type_ndx);
}

/* 
 * Set WFQ settings for a specificed queue in the OFP.
 */
STATIC
  uint32
    arad_egr_ofp_scheduling_wfq_set_unsafe(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
      SOC_SAND_IN  uint32                cosq,
      SOC_SAND_IN  ARAD_EGR_OFP_SCH_WFQ     *wfq_info
    )
{
  uint32
    res,
    offset;
  ARAD_EGQ_DWM_TBL_DATA
    dwm_tbl_data;
  uint32
    base_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_WFQ_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(wfq_info);

  /* Retreive base_q_pair */
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  /* offset set by base_q_pair + cosq */
  offset = base_q_pair + cosq;

  dwm_tbl_data.uc_or_uc_low_queue_weight = wfq_info->sched_weight;
  dwm_tbl_data.mc_or_mc_low_queue_weight = wfq_info->unsched_weight;

  res = arad_egq_dwm_tbl_set_unsafe(
          unit,
          offset,
          &dwm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_q_prio_map_entry_get()", 0, 0);
}

/* 
 * Set WFQ settings for a specificed queue in the OFP.
 */
STATIC
  uint32
    arad_egr_ofp_scheduling_wfq_get_unsafe(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
      SOC_SAND_IN  uint32                cosq,
      SOC_SAND_OUT ARAD_EGR_OFP_SCH_WFQ     *wfq_info
    )
{
  uint32
    res,
    offset;
  ARAD_EGQ_DWM_TBL_DATA
    dwm_tbl_data;
  uint32
    base_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_WFQ_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(wfq_info);

  ARAD_CLEAR(&dwm_tbl_data, ARAD_EGQ_DWM_TBL_DATA, 1);

  /* Retreive base_q_pair */
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  /* offset set by base_q_pair + cosq */
  offset = base_q_pair + cosq;

  res = arad_egq_dwm_tbl_get_unsafe(
          unit,
          offset,
          &dwm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  wfq_info->sched_weight = dwm_tbl_data.uc_or_uc_low_queue_weight;
  wfq_info->unsched_weight = dwm_tbl_data.mc_or_mc_low_queue_weight;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_q_prio_map_entry_get()", 0, 0);
}


/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_scheduling_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  )
{
  uint32    
    res,
    nif_prio_fld_val;
  soc_reg_above_64_val_t
    data,
    field_val; 
  uint32
    base_q_pair,
    nof_q_pairs,
    q_pair_in_port; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (info->nif_priority)
  {
  case ARAD_EGR_OFP_INTERFACE_PRIO_HIGH:
    nif_prio_fld_val = 0x0;
      break;
  case ARAD_EGR_OFP_INTERFACE_PRIO_LOW:
    nif_prio_fld_val = 0x1;
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 5, exit)
  }

  /* Retreive base_q_pair */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2000, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));

  /*
   * Set the OFP nif-priority. Each bit correspond to a OFP
   */  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2340, exit, READ_EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr(unit, REG_PORT_ANY, data));

  soc_reg_above_64_field_get(unit, EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr, data, PORT_PRIORITYf,field_val);
   
  /* Add the following settings */
  SHR_BITCOPY_RANGE(field_val,base_q_pair,&nif_prio_fld_val,0,1);
   
  /* Set */
  soc_reg_above_64_field_set(unit, EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr, data, PORT_PRIORITYf,field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2350, exit, WRITE_EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr(unit, REG_PORT_ANY, data));
  
  /* Set NIF priority. } */

  /* 
   * Set WFQ weights.
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2355, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_q_pairs));

  for (q_pair_in_port = 0; q_pair_in_port < nof_q_pairs; ++q_pair_in_port)
  {
    res = arad_egr_ofp_scheduling_wfq_set_unsafe(
          unit,
          ofp_ndx,
          q_pair_in_port,
          &(info->ofp_wfq_q_pair[q_pair_in_port])
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }  
  /* Set WFQ weights }*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_scheduling_set_unsafe()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_scheduling_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Verify NIF priority.
   * Set in any case, but ignored by HW unless is mapped to channelized NIF {
   */
  /* ARAD_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST is invalid for Arad */
  SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nif_priority, ARAD_EGR_OFP_CHNIF_NOF_PRIORITIES-2,
      ARAD_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 20, exit
    );

  SOC_SAND_ERR_IF_EQUALS_VALUE(info->nif_priority, ARAD_EGR_OFP_INTERFACE_PRIO_MID, ARAD_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 25,exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq.sched_weight, ARAD_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    ARAD_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq.unsched_weight, ARAD_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    ARAD_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 40, exit
  );
 
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq_high.sched_weight, ARAD_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    ARAD_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 45, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq_high.unsched_weight, ARAD_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    ARAD_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 50, exit
  );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_scheduling_verify()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_ofp_scheduling_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res,
    nif_prio_val = 0;
  soc_reg_above_64_val_t
    data,
    field_val;
  uint32
    base_q_pair,
    nof_q_pairs,
    q_pair_in_port; 




  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field_val);
 
  /* Retreive base_q_pair */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2300, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));

  /* Get NIF priority. Ignored by the HW unless is mapped to channelized NIF { */
  /*
   * Each bit correspond to a OFP
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2450, exit, READ_EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr(unit, REG_PORT_ANY, data));
  soc_reg_above_64_field_get(unit, EGQ_EGRESS_PORT_PRIORITY_CONFIGURATIONr, data, PORT_PRIORITYf,field_val);
  
  SHR_BITCOPY_RANGE(&nif_prio_val,0,field_val,base_q_pair,1);
    
  info->nif_priority = (nif_prio_val == 0) ? ARAD_EGR_OFP_INTERFACE_PRIO_HIGH:ARAD_EGR_OFP_INTERFACE_PRIO_LOW;

  /* Get NIF priority. } */

  /* 
   * Get WFQ weights.
   * Settings should be done via this API, only when the OFP port is a 2 priority mode.
   * The API skip WFQ weights configuration when port is not in 2 priority mode. 
   * Refer to API: () to configure WFQ weights for other modes (1/8 priority mode)
   */
  /* 
   * Get WFQ weights.
   */

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2455, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_q_pairs));

  for (q_pair_in_port = 0; q_pair_in_port < nof_q_pairs; ++q_pair_in_port)
  {
    res = arad_egr_ofp_scheduling_wfq_get_unsafe(
          unit,
          ofp_ndx,
          q_pair_in_port,
          &(info->ofp_wfq_q_pair[q_pair_in_port])
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }  
  /* Get WFQ weights }*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_scheduling_get_unsafe()",0,0);
}   
/*
 *    Internal conversion to the table entry format
 */
STATIC
  int
    arad_egr_q_prio_map_entry_get(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE     map_type_ndx,
      SOC_SAND_IN  uint32                           tc_ndx,
      SOC_SAND_IN  uint32                           dp_ndx,
      SOC_SAND_IN  uint32                           map_profile_ndx,
      SOC_SAND_OUT ARAD_EGQ_TC_DP_MAP_TBL_ENTRY     *entry
    )
{
    uint8 is_egr_mc = FALSE;
    SOCDNX_INIT_FUNC_DEFS;

    ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(entry);

    entry->map_profile = map_profile_ndx;
    entry->dp = dp_ndx;
    entry->tc = tc_ndx;

    switch(map_type_ndx)
    {
        case ARAD_EGR_UCAST_TO_SCHED:
            is_egr_mc = FALSE;
            break;

        case ARAD_EGR_MCAST_TO_UNSCHED:
            is_egr_mc = TRUE;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "ARAD_MAP_TYPE_NDX_OUT_OF_RANGE_ERR")));
    }

    entry->is_egr_mc = is_egr_mc;  

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_q_prio_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE   map_type_ndx,
    SOC_SAND_IN  uint32                         tc_ndx,
    SOC_SAND_IN  uint32                         dp_ndx,
    SOC_SAND_IN  uint32                         map_profile_ndx,
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY            *priority
  )
{
    ARAD_EGQ_TC_DP_MAP_TBL_ENTRY entry;
    ARAD_EGQ_TC_DP_MAP_TBL_DATA tbl_data;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(&entry);

    rv = arad_egr_q_prio_map_entry_get(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          &entry
        );
    SOCDNX_IF_ERR_EXIT(rv);

    tbl_data.dp = priority->dp;
    tbl_data.tc = priority->tc;

    rv = arad_egq_tc_dp_map_tbl_set_unsafe(
          unit,
          core,
          &entry,
          &tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_q_prio_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE   map_type_ndx,
    SOC_SAND_IN  uint32                         tc_ndx,
    SOC_SAND_IN  uint32                         dp_ndx,
    SOC_SAND_IN  uint32                         map_profile_ndx,
    SOC_SAND_OUT ARAD_EGR_Q_PRIORITY            *priority
  )
{
    ARAD_EGQ_TC_DP_MAP_TBL_ENTRY entry;
    ARAD_EGQ_TC_DP_MAP_TBL_DATA tbl_data;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_EGR_Q_PRIORITY_clear(priority);
    ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(&entry);

    rv = arad_egr_q_prio_map_entry_get(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          &entry
        );
    SOCDNX_IF_ERR_EXIT(rv);

    rv = arad_egq_tc_dp_map_tbl_get_unsafe(
          unit,
          core,
          &entry,
          &tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

    priority->dp = tbl_data.dp;
    priority->tc = tbl_data.tc;

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_q_profile_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 map_profile_id
  )
{
    ARAD_EGQ_PPCT_TBL_DATA ppct_tbl_data;
    uint32 base_q_pair;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    /* Retreive base_q_pair */
    SOCDNX_IF_ERR_EXIT( soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core, &base_q_pair));

    rv = arad_egq_ppct_tbl_get_unsafe(
          unit,
          base_q_pair,
          core,
          &ppct_tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

    ppct_tbl_data.cos_map_profile = map_profile_id;

    rv = arad_egq_ppct_tbl_set_unsafe(
          unit,
          base_q_pair,
          core,
          &ppct_tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_q_profile_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT uint32                 *map_profile_id
  )
{
    ARAD_EGQ_PPCT_TBL_DATA tbl_data;  
    uint32 base_q_pair;   
    SOCDNX_INIT_FUNC_DEFS;

    /* Retreive base_q_pair */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core, &base_q_pair));

    SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_get_unsafe(unit, base_q_pair, core, &tbl_data));

    *map_profile_id = tbl_data.cos_map_profile;

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_q_cgm_interface_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 cgm_interface
  )
{
    ARAD_EGQ_PPCT_TBL_DATA ppct_tbl_data;
    uint32 base_q_pair;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    /* Retreive base_q_pair */
    SOCDNX_IF_ERR_EXIT( soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core, &base_q_pair));

    rv = arad_egq_ppct_tbl_get_unsafe(
          unit,
          base_q_pair,
          core,
          &ppct_tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

    ppct_tbl_data.cgm_interface = cgm_interface;

    rv = arad_egq_ppct_tbl_set_unsafe(
          unit,
          base_q_pair,
          core,
          &ppct_tbl_data
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_dsp_pp_to_base_q_pair_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                     *base_q_pair
  )
{

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(base_q_pair);

  SOC_SAND_ERR_IF_ABOVE_NOF(ofp_ndx, ARAD_NOF_FAP_PORTS, ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);
  res = soc_port_sw_db_tm_port_to_base_q_pair_get(
                  unit,
                  ofp_ndx,
                  0,
                  base_q_pair);   
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_to_base_q_pair_get_unsafe()", ofp_ndx, 0);
}

uint32
  arad_egr_dsp_pp_to_base_q_pair_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  uint32                      base_q_pair
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_SET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_ports_fap_port_id_cud_extension_verify(unit, ofp_ndx);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(base_q_pair, ARAD_EGR_BASE_Q_PAIRS_NDX_MAX, ARAD_EGR_BASE_Q_PAIR_NDX_OUT_OF_RANGE_ERR, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_to_base_q_pair_set_verify()", ofp_ndx, base_q_pair);
}

uint32
  arad_egr_dsp_pp_to_base_q_pair_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_to_base_q_pair_get_verify()", ofp_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
  arad_egr_dsp_pp_to_base_q_pair_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     base_q_pair
  )
{
    uint32
        data,
        rv,
        memlock = 0;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA 
        dsp_tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    /* Update HW - EGQ*/
    MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
    memlock = 1;

    rv = arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, tm_port, core_id, &dsp_tbl_data);
    SOCDNX_IF_ERR_EXIT(rv);

    dsp_tbl_data.out_tm_port = base_q_pair;

    rv = arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, tm_port, core_id, &dsp_tbl_data);
    SOCDNX_IF_ERR_EXIT(rv);

    memlock = 0;
    MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);

    /* SCH - same mapping as EGQ */
    SOCDNX_IF_ERR_EXIT(READ_SCH_DSP_2_PORT_MAP_DSPPm(unit, SCH_BLOCK(unit, core_id), tm_port, &data));
    soc_SCH_DSP_2_PORT_MAP_DSPPm_field32_set(unit, &data, DSP_2_PORT_MAP_DSPPf, base_q_pair);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_DSP_2_PORT_MAP_DSPPm(unit, SCH_BLOCK(unit, core_id), tm_port, &data));

exit:
    if(memlock) {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOCDNX_FUNC_RETURN;
}

uint32
  ARAD_EGR_Q_PRIORITY_verify(
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, ARAD_EGR_Q_PRIORITY_TC_MAX, ARAD_EGR_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, ARAD_EGR_Q_PRIORITY_DP_MAX, ARAD_EGR_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_EGR_Q_PRIORITY_verify()",0,0);
}

int
  arad_egr_dsp_pp_priorities_mode_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  ARAD_EGR_PORT_PRIORITY_MODE    priority_mode
  )
{
    uint32 egq_priority_val;   
    uint32
        ps,
        nof_q_pairs,
        curr_q_pair,
        priority_i,
        fld_val,    
        base_q_pair;
    uint64
        data,
        field_val;
    uint32
        data32[2];
    uint32
        data_sch,
        field_sch_val;
    uint32
        pct_data[ARAD_EGQ_PCT_TBL_ENTRY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core_id, &base_q_pair));

    ps = ARAD_BASE_PORT_TC2PS(base_q_pair);

    switch (priority_mode)
    {
        case ARAD_EGR_PORT_ONE_PRIORITY:
            nof_q_pairs = 1;
            egq_priority_val = ARAD_EGQ_PS_MODE_ONE_PRIORITY_VAL;
            break;
        case ARAD_EGR_PORT_TWO_PRIORITIES:
            nof_q_pairs = 2;
            egq_priority_val = ARAD_EGQ_PS_MODE_TWO_PRIORITY_VAL;
            break;
        case ARAD_EGR_PORT_EIGHT_PRIORITIES:
            nof_q_pairs = 8;
            egq_priority_val = ARAD_EGQ_PS_MODE_EIGHT_PRIORITY_VAL;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid priority mode %d"), priority_mode));
    }
  
    /* EGQ */
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PS_MODEr(unit, core_id, &data));
    field_val = soc_reg64_field_get(unit, EGQ_PS_MODEr, data, PS_MODEf);

    data32[0] = COMPILER_64_LO(field_val);
    data32[1] = COMPILER_64_HI(field_val);

    SHR_BITCOPY_RANGE(data32,ps*ARAD_EGQ_PS_PRIORITY_NOF_BITS,&egq_priority_val,0,ARAD_EGQ_PS_PRIORITY_NOF_BITS);

    COMPILER_64_SET(field_val, data32[1], data32[0]);

    soc_reg64_field_set(unit, EGQ_PS_MODEr, &data, PS_MODEf ,field_val);    
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PS_MODEr(unit, core_id, data));

    /* SCH */
    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_1P_PRIORITY_MODE_REGISTERr(unit, core_id, &data_sch));
    field_sch_val = soc_reg_field_get(unit, SCH_PS_1P_PRIORITY_MODE_REGISTERr, data_sch, PS_1P_PRIORITY_MODE_BITMAPf);
    if (priority_mode == ARAD_EGR_PORT_ONE_PRIORITY) {
        field_sch_val |= SOC_SAND_BIT(ps);
    } else {
        field_sch_val &= SOC_SAND_RBIT(ps);
    }
    soc_reg_field_set(unit, SCH_PS_1P_PRIORITY_MODE_REGISTERr, &data_sch, PS_1P_PRIORITY_MODE_BITMAPf, field_sch_val);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PS_1P_PRIORITY_MODE_REGISTERr(unit, core_id, data_sch));

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_2P_PRIORITY_MODE_REGISTERr(unit, core_id, &data_sch));
    field_sch_val = soc_reg_field_get(unit, SCH_PS_2P_PRIORITY_MODE_REGISTERr, data_sch, PS_2P_PRIORITY_MODE_BITMAPf);
    if (priority_mode == ARAD_EGR_PORT_TWO_PRIORITIES) {
        field_sch_val |= SOC_SAND_BIT(ps);
    } else {
        field_sch_val &= SOC_SAND_RBIT(ps);
    }
    soc_reg_field_set(unit, SCH_PS_2P_PRIORITY_MODE_REGISTERr, &data_sch, PS_2P_PRIORITY_MODE_BITMAPf, field_sch_val);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PS_2P_PRIORITY_MODE_REGISTERr(unit, core_id, data_sch));

    /* 
    *  In case of 1/2 P have default TCG Scheme:
    *  For 1P set 1:1 mapping between HR : TCG_NDX
    *  For 2P set HR [i,i+1] to TCG i
    */
    if (priority_mode == ARAD_EGR_PORT_ONE_PRIORITY || priority_mode == ARAD_EGR_PORT_TWO_PRIORITIES) 
    {
        /* Run over all HRs within the port */
        fld_val = 0;
        for (priority_i = 0; priority_i < ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE; ++priority_i) {
            uint32 tcg_ndx = 0;
            if (priority_mode == ARAD_EGR_PORT_ONE_PRIORITY) {
                /* Set tcg static index */    
                tcg_ndx = priority_i;    
            }
            if (priority_mode == ARAD_EGR_PORT_TWO_PRIORITIES) {
                /* Set tcg static index */
                tcg_ndx = (priority_i % 2 == 0) ? priority_i:(priority_i-1) ;    
            }

            SHR_BITCOPY_RANGE(&fld_val, priority_i * ARAD_NOF_TCG_IN_BITS, &tcg_ndx, 0, ARAD_NOF_TCG_IN_BITS);
        }

        SOCDNX_IF_ERR_EXIT(READ_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit, core_id), ps, &data_sch));
        soc_SCH_PORT_SCHEDULER_MAP_PSMm_field32_set(unit, &data_sch, TC_PG_MAPf, fld_val);  
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_PORT_SCHEDULER_MAP_PSMm(unit, SCH_BLOCK(unit, core_id), ps, &data_sch));    
    }

    /* Set COS MAP profile equals Egress TC queue */
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_q_pairs; curr_q_pair++)
    {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PORT_CONFIGURATION_TABLE_PCTm(unit, EGQ_BLOCK(unit, core_id), curr_q_pair, pct_data));
        soc_EGQ_PORT_CONFIGURATION_TABLE_PCTm_field32_set(unit, pct_data, EGRESS_TCf, curr_q_pair-base_q_pair);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PORT_CONFIGURATION_TABLE_PCTm(unit, EGQ_BLOCK(unit, core_id), curr_q_pair, pct_data));
    }

  
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_egr_dsp_pp_priorities_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_PRIORITY_MODE    priority_mode
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_PRIORITIES_MODE_SET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(priority_mode, ARAD_EGR_PORT_PRIORITY_MAX, ARAD_EGR_PRIORITY_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_priorities_mode_set_verify()", ofp_ndx, 0);
}

uint32
  arad_egr_dsp_pp_priorities_mode_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID              ofp_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_priorities_mode_get_verify()", ofp_ndx, 0);
}

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_egr_dsp_pp_to_base_q_pair_set_unsafe" API.
 *     Refer to "arad_egr_dsp_pp_to_base_q_pair_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_egr_dsp_pp_priorities_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE    *priority_mode
  )
{
  uint32
    nof_q_pairs,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority_mode);


  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2455, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_q_pairs));

  switch (nof_q_pairs)
  {
  case 1:
    *priority_mode = ARAD_EGR_PORT_ONE_PRIORITY;
    break;
  case 2:
    *priority_mode = ARAD_EGR_PORT_TWO_PRIORITIES;
    break;
  case 8:
    *priority_mode = ARAD_EGR_PORT_EIGHT_PRIORITIES;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_priorities_mode_get_unsafe()", ofp_ndx, 0);
}

uint32
  arad_egr_dsp_pp_shaper_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGR_PORT_SHAPER_MODE      shaper_mode
  )
{
  uint32
    base_q_pair,
    nof_q_pairs,
    curr_q_pair;
  uint32
    shaper_val;
  uint32 res;
  soc_reg_above_64_val_t
    data,
    field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_SHAPER_MODE_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2000, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2010, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_q_pairs));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2790, exit, READ_EGQ_PACKET_RATE_SHAPER_ENr(unit, REG_PORT_ANY, data));
  soc_reg_above_64_field_get(unit,EGQ_PACKET_RATE_SHAPER_ENr,data,PACKET_RATE_SHAPER_ENf,field_val);

  shaper_val = (shaper_mode == ARAD_EGR_PORT_SHAPER_PACKET_MODE) ? 1:0;

  /* Set all related q_pairs for the given port */
  for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_q_pairs; curr_q_pair++)
  {
    SHR_BITCOPY_RANGE(field_val,curr_q_pair*ARAD_EGQ_SHAPER_MODE_NOF_BITS,&shaper_val,0,ARAD_EGQ_SHAPER_MODE_NOF_BITS);
  } 
  soc_reg_above_64_field_set(unit,EGQ_PACKET_RATE_SHAPER_ENr,data,PACKET_RATE_SHAPER_ENf,field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2800, exit, WRITE_EGQ_PACKET_RATE_SHAPER_ENr(unit, REG_PORT_ANY, data));

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_shaper_mode_get_unsafe()", ofp_ndx, 0);
}

uint32
  arad_egr_dsp_pp_shaper_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_SHAPER_MODE      shaper_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_SHAPER_MODE_SET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_shaper_mode_set_verify()", ofp_ndx, 0);
}

uint32
  arad_egr_dsp_pp_shaper_mode_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_SHAPER_MODE_GET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_shaper_mode_get_verify()", ofp_ndx, 0);
}

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_egr_dsp_pp_shaper_mode_set_unsafe" API.
 *     Refer to "arad_egr_dsp_pp_shaper_mode_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_egr_dsp_pp_shaper_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_SHAPER_MODE     *shaper_mode
  )
{
  uint32
    base_q_pair;
  soc_reg_above_64_val_t
    data,
    field_val;
  uint32
    shaper_val = 0,
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_SHAPER_MODE_GET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field_val);

  SOC_SAND_CHECK_NULL_INPUT(shaper_mode);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2000, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2900, exit, READ_EGQ_PACKET_RATE_SHAPER_ENr(unit, REG_PORT_ANY, data));
  soc_reg_above_64_field_get(unit,EGQ_PACKET_RATE_SHAPER_ENr,data,PACKET_RATE_SHAPER_ENf,field_val);

  SHR_BITCOPY_RANGE(&shaper_val,0,field_val,base_q_pair*ARAD_EGQ_SHAPER_MODE_NOF_BITS,ARAD_EGQ_SHAPER_MODE_NOF_BITS);

  switch (shaper_val)
  {
  case 0:
    *shaper_mode = ARAD_EGR_PORT_SHAPER_DATA_MODE;
    break;
  case 1:
    *shaper_mode = ARAD_EGR_PORT_SHAPER_PACKET_MODE;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_shaper_mode_get_unsafe()", ofp_ndx, 0);
}

uint32    
  arad_egr_queuing_dev_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN          ARAD_EGR_QUEUING_DEV_TH    *info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    field,
    reg,
    mem;
  uint64
    reg_64;
  uint32
    field_32;
  uint8
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_DEV_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  /* CGM_CGM_GENERAL_PD_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1110, exit, READ_CGM_CGM_GENERAL_PD_THr(unit, reg));
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.uc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_PD_THr, reg, UC_PD_THf, field);
  
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_THf, field);
  
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.total.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_PD_THr, reg, TOTAL_PD_THf, field);
  
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool[0].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_SP_0_THf, field);
  
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool[1].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_SP_1_THf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, WRITE_CGM_CGM_GENERAL_PD_THr(unit, reg));

  /* CGM_CGM_GENERAL_DB_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_CGM_CGM_GENERAL_DB_THr(unit, reg));
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.uc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_DB_THr, reg, UC_DB_THf, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_THf, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->global.total.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_DB_THr, reg, TOTAL_DB_THf, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool[0].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_SP_0_THf, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool[1].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_SP_1_THf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1140, exit, WRITE_CGM_CGM_GENERAL_DB_THr(unit, reg));

  /* CGM_CGM_MC_PD_SP_TC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1142, exit, READ_CGM_CGM_MC_PD_SP_TC_THr(unit, reg));
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][0].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_0f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][1].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_1f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][2].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_2f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][3].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_3f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][4].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_4f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][5].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_5f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][6].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_6f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][7].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_7f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][0].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_8f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][1].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_9f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][2].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_10f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][3].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_11f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][4].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_12f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][5].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_13f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][6].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_14f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][7].mc.descriptors, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_15f, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1147, exit, WRITE_CGM_CGM_MC_PD_SP_TC_THr(unit, reg));

  /* CGM_CGM_MC_DB_SP_TC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1150, exit, READ_CGM_CGM_MC_DB_SP_TC_THr(unit, reg));
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][0].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_0f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][1].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_1f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][2].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_2f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][3].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_3f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][4].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_4f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][5].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_5f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][6].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_6f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][7].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_7f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][0].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_8f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][1].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_9f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][2].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_10f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][3].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_11f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][4].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_12f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][5].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_13f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][6].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_14f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][7].mc.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_15f, field);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1160, exit, WRITE_CGM_CGM_MC_DB_SP_TC_THr(unit, reg));

  /* CGM_CGM_MC_RSVD_MAX_VALr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1170, exit, READ_CGM_CGM_MC_RSVD_MAX_VALr(unit, &reg_64));
  soc_reg64_field32_set(unit, CGM_CGM_MC_RSVD_MAX_VALr, &reg_64, CGM_MC_RSVD_PD_SP_0_MAX_VALf, info->pool[0].reserved.descriptors);
  soc_reg64_field32_set(unit, CGM_CGM_MC_RSVD_MAX_VALr, &reg_64, CGM_MC_RSVD_PD_SP_1_MAX_VALf, info->pool[1].reserved.descriptors);
  soc_reg64_field32_set(unit, CGM_CGM_MC_RSVD_MAX_VALr, &reg_64, CGM_MC_RSVD_DB_SP_0_MAX_VALf, info->pool[0].reserved.buffers);
  soc_reg64_field32_set(unit, CGM_CGM_MC_RSVD_MAX_VALr, &reg_64, CGM_MC_RSVD_DB_SP_1_MAX_VALf, info->pool[1].reserved.buffers);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1180, exit, WRITE_CGM_CGM_MC_RSVD_MAX_VALr(unit, reg_64));

  /* CGM_CGM_MC_RSVD_DB_SP_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1190, exit, READ_CGM_CGM_MC_RSVD_DB_SP_THr(unit, reg));
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][0].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_0f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][1].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_1f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][2].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_2f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][3].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_3f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][4].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_4f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][5].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_5f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][6].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_6f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[0][7].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_7f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][0].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_8f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][1].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_9f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][2].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_10f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][3].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_11f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][4].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_12f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][5].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_13f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][6].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_14f, field);
  SOC_REG_ABOVE_64_CLEAR(field);
  SHR_BITCOPY_RANGE(field, 0, &info->pool_tc[1][7].reserved.buffers, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_set(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_15f, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1200, exit, WRITE_CGM_CGM_MC_RSVD_DB_SP_THr(unit, reg));

  /* EGQ_PDCT_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES; ++index) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1210, exit, READ_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    field_32 = info->thresh_type[index].uc.descriptors;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_DIS_THf, &field_32);
    field_32 = info->thresh_type[index].mc_shared.descriptors;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_MC_PD_SHEARD_THf, &field_32);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1220, exit, WRITE_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
  }

  /* EGQ_PQST_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES; ++index) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1230, exit, READ_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    field_32 = info->thresh_type[index].uc.buffers;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_DIS_THf, &field_32);
    field_32 = info->thresh_type[index].mc_shared.buffers;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_MC_DB_SHEARD_THf, &field_32);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1240, exit, WRITE_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
  }

  /* EGQ_QDCT_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES*ARAD_NOF_TRAFFIC_CLASSES; ++index) {
    if(info->thresh_type[index/ARAD_NOF_TRAFFIC_CLASSES].reserved[index%ARAD_NOF_TRAFFIC_CLASSES].descriptors > EGQ_QDCT_PD_MAX_VAL)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_EGR_QDCT_PD_VALUE_OUT_OF_RANGE, 5, exit);
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1250, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    field_32 = info->thresh_type[index/ARAD_NOF_TRAFFIC_CLASSES].reserved[index%ARAD_NOF_TRAFFIC_CLASSES].descriptors;
    soc_EGQ_QDCT_TABLEm_field_set(unit, mem, QUEUE_MC_PD_RSVD_THf, &field_32);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1260, exit, WRITE_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_dev_set_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_dev_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_OUT          ARAD_EGR_QUEUING_DEV_TH    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    field,
    reg,
    mem;
  uint64
    reg_64;
  uint8
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_DEV_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_EGR_QUEUING_DEV_TH_clear(info);

  /* CGM_CGM_GENERAL_PD_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1360, exit, READ_CGM_CGM_GENERAL_PD_THr(unit, reg));
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_PD_THr, reg, UC_PD_THf, field);
  SHR_BITCOPY_RANGE(&info->global.uc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_THf, field);
  SHR_BITCOPY_RANGE(&info->global.mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_PD_THr, reg, TOTAL_PD_THf, field);
  SHR_BITCOPY_RANGE(&info->global.total.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_SP_0_THf, field);
  SHR_BITCOPY_RANGE(&info->pool[0].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_PD_THr, reg, MC_PD_SP_1_THf, field);
  SHR_BITCOPY_RANGE(&info->pool[1].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  
  /* CGM_CGM_GENERAL_DB_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1370, exit, READ_CGM_CGM_GENERAL_DB_THr(unit, reg));
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_DB_THr, reg, UC_DB_THf, field);
  SHR_BITCOPY_RANGE(&info->global.uc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_THf, field);
  SHR_BITCOPY_RANGE(&info->global.mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_DB_THr, reg, TOTAL_DB_THf, field);
  SHR_BITCOPY_RANGE(&info->global.total.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_SP_0_THf, field);
  SHR_BITCOPY_RANGE(&info->pool[0].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_GENERAL_DB_THr, reg, MC_DB_SP_1_THf, field);
  SHR_BITCOPY_RANGE(&info->pool[1].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  
  /* CGM_CGM_MC_PD_SP_TC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1375, exit, READ_CGM_CGM_MC_PD_SP_TC_THr(unit, reg));
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_0f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][0].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_1f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][1].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_2f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][2].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_3f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][3].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_4f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][4].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_5f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][5].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_6f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][6].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_7f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][7].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_8f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][0].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_9f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][1].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_10f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][2].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_11f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][3].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_12f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][4].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_13f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][5].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_14f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][6].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_PD_SP_TC_THr, reg, CGM_MC_PD_SP_TC_TH_15f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][7].mc.descriptors, 0, field, 0, ARAD_EGQ_THRESHOLD_PD_NOF_BITS);

  /* CGM_CGM_MC_DB_SP_TC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1380, exit, READ_CGM_CGM_MC_DB_SP_TC_THr(unit, reg));
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_0f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][0].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_1f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][1].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_2f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][2].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_3f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][3].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_4f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][4].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_5f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][5].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_6f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][6].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_7f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][7].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_8f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][0].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_9f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][1].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_10f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][2].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_11f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][3].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_12f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][4].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_13f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][5].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_14f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][6].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_DB_SP_TC_THr, reg, CGM_MC_DB_SP_TC_TH_15f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][7].mc.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  
  /* CGM_CGM_MC_RSVD_MAX_VALr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1390, exit, READ_CGM_CGM_MC_RSVD_MAX_VALr(unit, &reg_64));
  info->pool[0].reserved.descriptors = soc_reg64_field32_get(unit, CGM_CGM_MC_RSVD_MAX_VALr, reg_64, CGM_MC_RSVD_PD_SP_0_MAX_VALf);
  info->pool[1].reserved.descriptors = soc_reg64_field32_get(unit, CGM_CGM_MC_RSVD_MAX_VALr, reg_64, CGM_MC_RSVD_PD_SP_1_MAX_VALf);
  info->pool[0].reserved.buffers = soc_reg64_field32_get(unit, CGM_CGM_MC_RSVD_MAX_VALr, reg_64, CGM_MC_RSVD_DB_SP_0_MAX_VALf);
  info->pool[1].reserved.buffers = soc_reg64_field32_get(unit, CGM_CGM_MC_RSVD_MAX_VALr, reg_64, CGM_MC_RSVD_DB_SP_1_MAX_VALf);
  
  /* CGM_CGM_MC_RSVD_DB_SP_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1400, exit, READ_CGM_CGM_MC_RSVD_DB_SP_THr(unit, reg));
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_0f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][0].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_1f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][1].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_2f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][2].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_3f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][3].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_4f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][4].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_5f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][5].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_6f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][6].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_7f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[0][7].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_8f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][0].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_9f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][1].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_10f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][2].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_11f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][3].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_12f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][4].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_13f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][5].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_14f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][6].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  soc_reg_above_64_field_get(unit, CGM_CGM_MC_RSVD_DB_SP_THr, reg, CGM_MC_RSVD_DB_SP_TH_15f, field);
  SHR_BITCOPY_RANGE(&info->pool_tc[1][7].reserved.buffers, 0, field, 0, ARAD_EGQ_THRESHOLD_DBUFF_NOF_BITS);
  
  /* EGQ_PDCT_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES; ++index) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1410, exit, READ_EGQ_PDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_UC_PD_DIS_THf, &info->thresh_type[index].uc.descriptors);
    soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_MC_PD_SHEARD_THf, &info->thresh_type[index].mc_shared.descriptors);
  }

  /* EGQ_PQST_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES; ++index) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1420, exit, READ_EGQ_PQST_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    soc_EGQ_PQST_TABLEm_field_get(unit, mem, PORT_UC_DB_DIS_THf, &info->thresh_type[index].uc.buffers);
    soc_EGQ_PQST_TABLEm_field_get(unit, mem, PORT_MC_DB_SHEARD_THf, &info->thresh_type[index].mc_shared.buffers);
  }

  /* EGQ_QDCT_TABLEm */
  for(index = 0; index < ARAD_NOF_THRESH_TYPES*ARAD_NOF_TRAFFIC_CLASSES; ++index) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1430, exit, READ_EGQ_QDCT_TABLEm(unit, MEM_BLOCK_ANY, index, mem));
    soc_EGQ_QDCT_TABLEm_field_get(
                                    unit,
                                    mem,
                                    QUEUE_MC_PD_RSVD_THf,
                                    &info->thresh_type[index/ARAD_NOF_TRAFFIC_CLASSES].reserved[index%ARAD_NOF_TRAFFIC_CLASSES].descriptors
                                  );
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_dev_get_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_mc_cos_map_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32    tc_ndx,
    SOC_SAND_IN    uint32    dp_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_MC_COS_MAP    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
    reg[1],
    field;
  uint8
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_MC_COS_MAP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, ARAD_EGR_Q_PRIORITY_TC_MAX, ARAD_EGR_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, ARAD_EGR_Q_PRIORITY_DP_MAX, ARAD_EGR_DP_OUT_OF_RANGE_ERR, 11, exit);

  /* CGM_CGM_MAP_TC_TO_SPr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1530, exit, READ_CGM_CGM_MAP_TC_TO_SPr(unit, REG_PORT_ANY, reg));
  if(info->pool_id){
    SHR_BITSET(reg, info->tc_group);
  }
  else
  {
    SHR_BITCLR(reg, info->tc_group);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1540, exit, WRITE_CGM_CGM_MAP_TC_TO_SPr(unit, REG_PORT_ANY, *reg));

  /* EGQ_MC_SP_TC_MAPm */
  index = 0;
  index |= (dp_ndx);
  index |= (tc_ndx)<<2;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1550, exit, READ_EGQ_MC_SP_TC_MAPm(unit, MEM_BLOCK_ANY, index, reg));
  field = info->tc_group;
  soc_EGQ_MC_SP_TC_MAPm_field_set(unit, reg, CGM_MC_TCf, &field);
  /* CGM MC SE equals to DP index. CGM_MC_SE is being used later for CGM_DP_ELIGIBLE_TO_USE_RESOURCESr */
  field = dp_ndx;
  soc_EGQ_MC_SP_TC_MAPm_field_set(unit, reg, CGM_MC_SEf, &field);
  field = info->pool_id;
  soc_EGQ_MC_SP_TC_MAPm_field_set(unit, reg, CGM_MC_SPf, &field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1560, exit, WRITE_EGQ_MC_SP_TC_MAPm(unit, MEM_BLOCK_ANY, index, reg));

  /* EGQ_MC_PRIORITY_LOOKUP_TABLEr */
  index = ((tc_ndx)<<2) | dp_ndx;
  *reg = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1570, exit, READ_EGQ_MC_PRIORITY_LOOKUP_TABLEr(unit, REG_PORT_ANY, reg));

  if(!info->pool_id){
    SHR_BITSET(reg, index);
  }
  else
  {
    SHR_BITCLR(reg, index);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1580, exit, WRITE_EGQ_MC_PRIORITY_LOOKUP_TABLEr(unit, REG_PORT_ANY, *reg));

  /* CGM_DP_ELIGIBLE_TO_USE_RESOURCESr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1590, exit, READ_CGM_CGM_DP_ELIGIBLE_TO_USE_RESOURCESr(unit, REG_PORT_ANY, reg));
  index = ((info->pool_id) << 2) | dp_ndx;
  if(info->pool_eligibility){
    SHR_BITSET(reg, index);
  }
  else
  {
    SHR_BITCLR(reg, index);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1600, exit, WRITE_CGM_CGM_DP_ELIGIBLE_TO_USE_RESOURCESr(unit, REG_PORT_ANY, *reg));


  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_mc_cos_map_set_unsafe()", tc_ndx, dp_ndx);
}

uint32    
  arad_egr_queuing_mc_cos_map_get_unsafe(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     uint32    tc_ndx,
    SOC_SAND_IN     uint32    dp_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
    reg[1];
  uint8
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_MC_COS_MAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_EGR_QUEUING_MC_COS_MAP_clear(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, ARAD_EGR_Q_PRIORITY_TC_MAX, ARAD_EGR_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, ARAD_EGR_Q_PRIORITY_DP_MAX, ARAD_EGR_DP_OUT_OF_RANGE_ERR, 11, exit);

  index = 0;
  index |= (dp_ndx);
  index |= (tc_ndx)<<2;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1660, exit, READ_EGQ_MC_SP_TC_MAPm(unit, MEM_BLOCK_ANY, index, reg));
  soc_EGQ_MC_SP_TC_MAPm_field_get(unit, reg, CGM_MC_TCf, &info->tc_group);  
  soc_EGQ_MC_SP_TC_MAPm_field_get(unit, reg, CGM_MC_SPf, &info->pool_id);

  /* CGM_DP_ELIGIBLE_TO_USE_RESOURCESr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1590, exit, READ_CGM_CGM_DP_ELIGIBLE_TO_USE_RESOURCESr(unit, REG_PORT_ANY, reg));
  index = ((info->pool_id) << 2) | dp_ndx;

  info->pool_eligibility = (SHR_BITGET(reg, index)) ? 1:0;
  
  
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_mc_cos_map_get_unsafe()", tc_ndx, dp_ndx);
}

uint32    
  arad_egr_queuing_if_fc_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_IN          ARAD_EGR_QUEUING_IF_FC    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
    if_internal_id;
  soc_reg_above_64_val_t
    reg_above_64;
  ARAD_NIF_TYPE
    nif_type;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
  uint32 
    nof_if_to_be_set = 1;
  uint32
      i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  if_internal_id = arad_nif2intern_id(unit, if_ndx);
  nif_type = arad_nif_id2type(if_ndx);
  ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.tdm_mode;

  if ((nif_type == ARAD_NIF_TYPE_ILKN) && (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)) {
      nof_if_to_be_set = 2;
  }

  if(if_internal_id == ARAD_NIF_ID_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }

  for (i=0; i <  nof_if_to_be_set; ++i,++if_internal_id) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1760, exit, READ_CGM_CGM_MAP_IF_2_THr(unit, REG_PORT_ANY, reg_above_64));
    SHR_BITCOPY_RANGE(reg_above_64, if_internal_id*3, &info->uc_profile, 0, 3);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1770, exit, WRITE_CGM_CGM_MAP_IF_2_THr(unit, REG_PORT_ANY, reg_above_64));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1780, exit, READ_CGM_CGM_MC_INTERFACE_MAP_THr(unit, REG_PORT_ANY, reg_above_64));
    if(if_internal_id*2 > 32) {
    reg_above_64[1] |= (info->mc_pd_profile & 0x3) << (2 * if_internal_id - 32);
    }
    else
    {
    reg_above_64[0] |= (info->mc_pd_profile & 0x3) << (2 * if_internal_id);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1790, exit, WRITE_CGM_CGM_MC_INTERFACE_MAP_THr(unit, REG_PORT_ANY, reg_above_64));
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_set_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_if_fc_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_OUT          ARAD_EGR_QUEUING_IF_FC    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
    if_internal_id;
  soc_reg_above_64_val_t
    reg_above_64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_EGR_QUEUING_IF_FC_clear(info);

  if_internal_id = arad_nif2intern_id(unit, if_ndx);

  if(if_internal_id == ARAD_NIF_ID_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1870, exit, READ_CGM_CGM_MAP_IF_2_THr(unit, REG_PORT_ANY, reg_above_64));
  info->uc_profile = 0;
  SHR_BITCOPY_RANGE(&info->uc_profile, 0, reg_above_64, if_internal_id*3, 3);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1880, exit, READ_CGM_CGM_MC_INTERFACE_MAP_THr(unit, REG_PORT_ANY, reg_above_64));
  info->mc_pd_profile = 0;
  if(if_internal_id*2 > 32) {
    info->mc_pd_profile |= (reg_above_64[1] >> (2*if_internal_id - 32)) & 0x3;
  }
  else
  {
    info->mc_pd_profile |= (reg_above_64[0] >> (2*if_internal_id)) & 0x3;
  }
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_get_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_if_fc_uc_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_IN          ARAD_EGR_QUEUING_IF_UC_FC    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    pd,
    size_256,
    pd_field,
    size_256_field;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_UC_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_REG_ABOVE_64_CLEAR(pd_field);
  SOC_REG_ABOVE_64_CLEAR(size_256_field);

  /* CGM_CGM_UC_PD_INTERFACE_FC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1970, exit, READ_CGM_CGM_UC_PD_INTERFACE_FC_THr(unit, pd));
  SHR_BITCOPY_RANGE(pd_field, 0, &info->pd_th, 0, ARAD_EGQ_PD_INTERFACE_NOF_BITS);

  /* CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1980, exit, READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr(unit, size_256));
  SHR_BITCOPY_RANGE(size_256_field, 0, &info->size256_th, 0, ARAD_EGQ_SIZE_256_INTERFACE_NOF_BITS);

  switch(uc_if_profile_ndx) {
    case 0:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_0f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_0f, size_256_field);
      break;
    case 1:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_1f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_1f, size_256_field);
      break;
    case 2:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_2f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_2f, size_256_field);
      break;
    case 3:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_3f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_3f, size_256_field);
      break;
    case 4:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_4f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_4f, size_256_field);
      break;
    case 5:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_5f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_5f, size_256_field);
      break;
    case 6:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_6f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_6f, size_256_field);
      break;
    case 7:
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_7f, pd_field);
      soc_reg_above_64_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr, size_256, CGM_UC_SIZE_256_INTERFACE_FC_TH_7f, size_256_field);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1990, exit, WRITE_CGM_CGM_UC_PD_INTERFACE_FC_THr(unit, pd));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2000, exit, WRITE_CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr(unit, size_256));

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_uc_set_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_if_fc_uc_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_OUT          ARAD_EGR_QUEUING_IF_UC_FC    *info
  )
{
  uint32
      res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    pd,
    size_256,
    pd_field,
    size_256_field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_UC_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_EGR_QUEUING_IF_UC_FC_clear(info);

  /* CGM_CGM_UC_PD_INTERFACE_FC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2100, exit, READ_CGM_CGM_UC_PD_INTERFACE_FC_THr(unit, pd));

  /* CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2110, exit, READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_THr(unit, size_256));

  switch(uc_if_profile_ndx) {
    case 0:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_0f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_0f, size_256_field);
      break;
    case 1:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_1f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_1f, size_256_field);
      break;
    case 2:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_2f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_2f, size_256_field);
      break;
    case 3:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_3f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_3f, size_256_field);
      break;
    case 4:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_4f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_4f, size_256_field);
      break;
    case 5:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_5f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_5f, size_256_field);
      break;
    case 6:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_6f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_6f, size_256_field);
      break;
    case 7:
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, pd, CGM_UC_PD_INTERFACE_FC_TH_7f, pd_field);
      soc_reg_above_64_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_THr, size_256, CGM_UC_PD_INTERFACE_FC_TH_7f, size_256_field);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }

  SHR_BITCOPY_RANGE(&info->pd_th, 0, pd_field, 0, ARAD_EGQ_PD_INTERFACE_NOF_BITS);
  SHR_BITCOPY_RANGE(&info->size256_th, 0, size_256_field, 0, ARAD_EGQ_SIZE_256_INTERFACE_NOF_BITS);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_uc_get_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_if_fc_mc_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    uint32    mc_if_profile_ndx,
    SOC_SAND_IN uint32    pd_th
  )
{
  uint32
      res = SOC_SAND_OK;
  uint64
    reg;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_MC_SET_UNSAFE);
  /* CGM_CGM_MC_INTERFACE_PD_THr */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2210, exit, READ_CGM_CGM_MC_INTERFACE_PD_THr(unit, REG_PORT_ANY, &reg));
  switch(mc_if_profile_ndx) {
    case 0:
      soc_reg64_field32_set(unit, CGM_CGM_MC_INTERFACE_PD_THr, &reg, CGM_MC_INTERFACE_PD_TH_0f, pd_th);
      break;
    case 1:
      soc_reg64_field32_set(unit, CGM_CGM_MC_INTERFACE_PD_THr, &reg, CGM_MC_INTERFACE_PD_TH_1f, pd_th);
      break;
    case 2:
      soc_reg64_field32_set(unit, CGM_CGM_MC_INTERFACE_PD_THr, &reg, CGM_MC_INTERFACE_PD_TH_2f, pd_th);
      break;
    case 3:
      soc_reg64_field32_set(unit, CGM_CGM_MC_INTERFACE_PD_THr, &reg, CGM_MC_INTERFACE_PD_TH_3f, pd_th);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2220, exit, WRITE_CGM_CGM_MC_INTERFACE_PD_THr(unit, REG_PORT_ANY, reg));

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_mc_set_unsafe()", 0, 0);
}

uint32    
  arad_egr_queuing_if_fc_mc_get_unsafe(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     uint32    mc_if_profile_ndx,
    SOC_SAND_OUT uint32   *pd_th
  )
{
  uint32
      res = SOC_SAND_OK;
  uint64
    reg;
  uint32
    temp_pd_th = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_MC_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(pd_th);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2320, exit, READ_CGM_CGM_MC_INTERFACE_PD_THr(unit, REG_PORT_ANY, &reg));
  switch(mc_if_profile_ndx) {
    case 0:
      temp_pd_th = soc_reg64_field32_get(unit, CGM_CGM_MC_INTERFACE_PD_THr, reg, CGM_MC_INTERFACE_PD_TH_0f);
      break;
    case 1:
      temp_pd_th = soc_reg64_field32_get(unit, CGM_CGM_MC_INTERFACE_PD_THr, reg, CGM_MC_INTERFACE_PD_TH_1f);
      break;
    case 2:
      temp_pd_th = soc_reg64_field32_get(unit, CGM_CGM_MC_INTERFACE_PD_THr, reg, CGM_MC_INTERFACE_PD_TH_2f);
      break;
    case 3:
      temp_pd_th = soc_reg64_field32_get(unit, CGM_CGM_MC_INTERFACE_PD_THr, reg, CGM_MC_INTERFACE_PD_TH_3f);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR,10,exit);
  }
  *pd_th = temp_pd_th;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_fc_mc_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Function description
*       Associate the queue-pair (Port,Priority) to traffic class
*     groups (TCG) attributes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_egr_queuing_ofp_tcg_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
      res = SOC_SAND_OK,
    data,
    field_val;
  uint32
    ps,
    priority_i,
    tcg_i,
    base_q_pair,
    nof_priorities;
  uint8
    is_one_member;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx,0, &nof_priorities));

  if (nof_priorities != ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 10, exit);
  }

  /* If last four tcgs are only a single member TCG. In that case, verify first that the required 
     tcg_ndx is not mapped by other q-pair */
  /* Check each single member TCG that only one priority is mapped to { */
  for (tcg_i = ARAD_EGR_SINGLE_MEMBER_TCG_START; tcg_i <= ARAD_EGR_SINGLE_MEMBER_TCG_END; tcg_i++)
  {
    is_one_member = FALSE;
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i) 
    {
      if (tcg_info->tcg_ndx[priority_i] == tcg_i)
      {
        if (is_one_member)
        {
          /* More than one member set to this tcg */
          SOC_SAND_SET_ERROR_CODE(ARAD_TCG_SINGLE_MEMBER_ERR, 100+tcg_i, exit);
        }
        else
        {
          is_one_member = TRUE;
        }
      }     
    }
  }

  ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
  
  /* Set tcg indexs */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_EGQ_EPS_PRIO_MAPm(unit, MEM_BLOCK_ANY, ps, &data));  
  field_val = soc_EGQ_EPS_PRIO_MAPm_field32_get(unit,&data,EPS_PRIO_MAPf);

  for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
  {
    
    res = soc_sand_bitstream_set_any_field(&(tcg_info->tcg_ndx[priority_i]),priority_i*ARAD_EGQ_NOF_TCG_IN_BITS,ARAD_EGQ_NOF_TCG_IN_BITS,&field_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  
  soc_EGQ_EPS_PRIO_MAPm_field32_set(unit,&data,EPS_PRIO_MAPf,field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_EGQ_EPS_PRIO_MAPm(unit, MEM_BLOCK_ANY, ps, &data));  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_ofp_tcg_set_unsafe()", ofp_ndx, 0);
}

uint32
  arad_egr_queuing_ofp_tcg_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
    res,
    data,
    field_val;
  uint32
    base_q_pair,
    priority_i,
    nof_priorities,
    ps;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_GET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_priorities));

  if (nof_priorities != ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 10, exit);
  }

  ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_EGQ_EPS_PRIO_MAPm(unit, MEM_BLOCK_ANY, ps, &data));  
  field_val = soc_EGQ_EPS_PRIO_MAPm_field32_get(unit,&data,EPS_PRIO_MAPf);

  for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
  {    
    res = soc_sand_bitstream_get_any_field(&field_val,priority_i*ARAD_EGQ_NOF_TCG_IN_BITS,ARAD_EGQ_NOF_TCG_IN_BITS,&(tcg_info->tcg_ndx[priority_i]));
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_ofp_tcg_get_unsafe()", ofp_ndx, 0);
}

uint32
  arad_egr_queuing_ofp_tcg_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
    priority_i,
    nof_priorities;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_SET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_priorities);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  if (nof_priorities != ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 32, exit);
  }

  for (priority_i=0;priority_i < nof_priorities; ++priority_i)
  {
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      tcg_info->tcg_ndx[priority_i], ARAD_TCG_MIN, ARAD_TCG_MAX,
      ARAD_TCG_OUT_OF_RANGE_ERR, 40, exit
      );
  }
  


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_ofp_tcg_set_verify()", ofp_ndx, 0);
}

uint32
  arad_egr_queuing_ofp_tcg_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
      res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_GET_VERIFY);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_ofp_tcg_get_verify()", ofp_ndx, 0);
}

uint32
  arad_egr_queuing_tcg_weight_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          ofp_ndx,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res,
    is_tcg_weight_val,
    place_bit,
    field_val;
  uint32
    ps = 0,
    base_q_pair;    
  soc_reg_above_64_val_t
    data_above_64,
    field_above_64;
  soc_field_t
    field_name;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_SET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(field_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));
  
  /* Set TCG weight valid { */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_WFQ_TCG_DISr(unit, REG_PORT_ANY, data_above_64));
  soc_reg_above_64_field_get(unit, EGQ_WFQ_TCG_DISr, data_above_64, WFQ_TCG_DISf, field_above_64);

  /* Corresponded bit is Base + TCG_NDX (0-255) */
  place_bit = base_q_pair + tcg_ndx;
  is_tcg_weight_val = tcg_weight->tcg_weight_valid ? 0:1;
  if (is_tcg_weight_val)
  {
    res = soc_sand_bitstream_set_bit(field_above_64, place_bit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_sand_bitstream_reset_bit(field_above_64, place_bit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
  soc_reg_above_64_field_set(unit, EGQ_WFQ_TCG_DISr, data_above_64, WFQ_TCG_DISf,field_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EGQ_WFQ_TCG_DISr(unit, REG_PORT_ANY, data_above_64));
  /* Set TCG weight valid } */

  if (tcg_weight->tcg_weight_valid)
  {
    /* Set TCG weight only in case of valid { */
    SOC_REG_ABOVE_64_CLEAR(data_above_64);

    ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, READ_EGQ_DWM_8Pm(unit, MEM_BLOCK_ANY, ps, data_above_64));  

    switch (tcg_ndx)
    {
    case 0:
      field_name = WEIGHT_0f;
      break;
    case 1:
      field_name = WEIGHT_1f;
      break;
    case 2:
      field_name = WEIGHT_2f;
      break;
    case 3:
      field_name = WEIGHT_3f;
      break;
    case 4:
      field_name = WEIGHT_4f;
      break;
    case 5:
      field_name = WEIGHT_5f;
      break;
    case 6:
      field_name = WEIGHT_6f;
      break;
    case 7:
      field_name = WEIGHT_7f;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = tcg_weight->tcg_weight;

    soc_EGQ_DWM_8Pm_field32_set(unit,data_above_64,field_name,field_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EGQ_DWM_8Pm(unit, MEM_BLOCK_ANY, ps, data_above_64));    
    /* Set TCG weight } */
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_tcg_weight_set_unsafe()", ofp_ndx, tcg_ndx);
}


uint32
  arad_egr_queuing_tcg_weight_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          ofp_ndx,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res,    
    place_bit,
    field_val;
  uint32
    ps = 0,
    base_q_pair;    
  soc_reg_above_64_val_t
    data_above_64,
    field_above_64;
  soc_field_t
    field_name;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_GET_UNSAFE);
  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  SOC_REG_ABOVE_64_CLEAR(field_above_64);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair));

  /* Get TCG weight valid { */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_WFQ_TCG_DISr(unit, REG_PORT_ANY, data_above_64));
  soc_reg_above_64_field_get(unit, EGQ_WFQ_TCG_DISr, data_above_64, WFQ_TCG_DISf, field_above_64);

  /* Corresponded bit is Base + TCG_NDX (0-255) */
  place_bit = base_q_pair + tcg_ndx;
  tcg_weight->tcg_weight_valid = !SOC_SAND_NUM2BOOL(soc_sand_bitstream_test_bit(field_above_64,place_bit));
  /* Get TCG weight valid } */

  if (tcg_weight->tcg_weight_valid)
  {
    /* Get TCG weight only in case of valid { */
    SOC_REG_ABOVE_64_CLEAR(data_above_64);

    ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, READ_EGQ_DWM_8Pm(unit, MEM_BLOCK_ANY, ps, data_above_64));  

    switch (tcg_ndx)
    {
    case 0:
      field_name = WEIGHT_0f;
      break;
    case 1:
      field_name = WEIGHT_1f;
      break;
    case 2:
      field_name = WEIGHT_2f;
      break;
    case 3:
      field_name = WEIGHT_3f;
      break;
    case 4:
      field_name = WEIGHT_4f;
      break;
    case 5:
      field_name = WEIGHT_5f;
      break;
    case 6:
      field_name = WEIGHT_6f;
      break;
    case 7:
      field_name = WEIGHT_7f;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TCG_OUT_OF_RANGE_ERR, 50, exit);
    }

    field_val = soc_EGQ_DWM_8Pm_field32_get(unit,data_above_64,field_name);
    tcg_weight->tcg_weight = field_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_tcg_weight_get_unsafe()", ofp_ndx, tcg_ndx);
}

uint32
  arad_egr_queuing_tcg_weight_set_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          ofp_ndx,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_q_pair,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_SET_VERIFY_UNSAFE);
  
  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair);
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (base_q_pair == ARAD_EGQ_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 30, exit)
  }

  /* API functionality only when port is with 8 priorities. */

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

  if (nof_priorities != ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 32, exit);
  }

  /* Verify TCG */
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );

  /* Verify TCG weight */
  if (tcg_weight->tcg_weight_valid)
  {
/*
 * COVERITY
 *
 * ARAD_EGQ_TCG_WEIGHT_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
            tcg_weight->tcg_weight, ARAD_EGQ_TCG_WEIGHT_MIN, ARAD_EGQ_TCG_WEIGHT_MAX, 
            ARAD_SCH_TCG_WEIGHT_OUT_OF_RANGE_ERR, 35, exit
          );
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_tcg_weight_set_verify_unsafe()", ofp_ndx, tcg_ndx);
}

uint32
  arad_egr_queuing_tcg_weight_get_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          ofp_ndx,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res;
  uint32
    base_q_pair,
    nof_priorities;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_GET_VERIFY_UNSAFE);

  res = arad_fap_port_id_verify(unit, ofp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, ofp_ndx, 0, &base_q_pair);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (base_q_pair == ARAD_EGQ_PORT_ID_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_PORT_ID_ERR, 30, exit)
  }

  /* API functionality only when port is with 8 priorities. */
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, ofp_ndx, 0, &nof_priorities); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

  if (nof_priorities != ARAD_TCG_NOF_PRIORITIES_SUPPORT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TCG_NOT_SUPPORTED_ERR, 32, exit);
  }

  /* Verify TCG */
/*
 * COVERITY
 *
 * ARAD_TCG_MIN may be changed to be bigger than 0.
 */
/* coverity[unsigned_compare] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
          tcg_ndx, ARAD_TCG_MIN, ARAD_TCG_MAX, 
          ARAD_TCG_OUT_OF_RANGE_ERR, 35, exit
        );
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_tcg_weight_get_verify_unsafe()", ofp_ndx, tcg_ndx);
}

int    
  arad_egr_queuing_partition_scheme_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    ARAD_EGR_QUEUING_PARTITION_SCHEME    scheme
  )
{
    uint32 set_val;

    SOCDNX_INIT_FUNC_DEFS;

    if(scheme == ARAD_EGR_QUEUING_PARTITION_SCHEME_DISCRETE) {
        set_val = 1;
    } else {
        set_val = 0;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_PD_TH_SP_OR_SHAREDr(unit, SOC_CORE_ALL, set_val));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_DB_TH_SP_OR_SHAREDr(unit, SOC_CORE_ALL, set_val));

exit:
    SOCDNX_FUNC_RETURN;
}

#define READ_REGISTER_NO_PIPE(reg, out_variable) \
  if (READ_##reg(unit, &(out_variable)) != SOC_E_NONE) { \
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to read "#reg" unit %d"), unit)); \
  }


#define READ_REGISTER_ARRAY_NO_PIPE(reg, i, out_variable) \
  if (READ_##reg(unit, i, &(out_variable)) != SOC_E_NONE) { \
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to read "#reg" at index %d unit %d"), i, unit)); \
  }

#define READ_REGISTER_ARRAY(reg, pipe, i, out_variable) \
  if (READ_##reg(unit, pipe, i, &(out_variable)) != SOC_E_NONE) { \
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to read "#reg" at index %d unit %d"), i, unit)); \
  }

#define READ_MEMORY(mem, index1, index2, dma_mem) \
  { \
    int rv = soc_mem_array_read_range(unit, mem, 0, MEM_BLOCK_ANY, index1, index2, dma_mem); \
    if (rv != SOC_E_NONE) { \
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to read "#mem" at indices %d-%d unit %d: %s"), index1, index2, unit, soc_errmsg(rv))); \
    } \
  }

#define CLEAR_MEMORY(mem, index1, index2, dma_mem) \
  { \
    int rv; \
    *dma_mem = 0; \
    rv = arad_fill_partial_table_with_entry(unit, mem, 0, 0, MEM_BLOCK_ANY, index1, index2, (void*)dma_mem); \
    if (rv != SOC_E_NONE) { \
      SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to re-initialize "#mem" at indices %d-%d unit %d: %s"), index1, index2, unit, soc_errmsg(rv))); \
    } \
  }

#define OTM_PORTS_LAST_ARRAY_INDEX   (2*ARAD_EGR_CGM_OTM_PORTS_NUM-1)
#define QUEUES_LAST_ARRAY_INDEX   (2*ARAD_EGR_CGM_OTM_PORTS_NUM-1)

/*
Get Arad congestion statistics.
Output will be returned in each of the first three structure pointers if it is not Null.

If disable_updates is non zero, then maximum statistics update will be disabled during
maximum statistics collection.
This will provide correlated maximum statistics, at the expense of not updating
the maximum statistics during the operation.

If max_stats is not null, the maximum values are reset when they are read.
The next read will contain maximum values from this function call.
*/

int soc_arad_get_congestion_statistics(
  int unit,
  ARAD_EGR_CGM_CONGENSTION_STATS *cur_stats,   /* place current statistics output here */
  ARAD_EGR_CGM_CONGENSTION_STATS *max_stats,   /* place maximum statistics output here */
  ARAD_EGR_CGM_CONGENSTION_COUNTERS *counters, /* place counters output here */
  const int disable_updates /* should the function disable maximum statistics updates when it collects them */
  )
{
  int i;
  int updates_are_disabled = 0;
  uint32 value;
  uint32 *dma_buf = 0;
  uint32 *buf_ptr;
  SOCDNX_INIT_FUNC_DEFS;

  if (cur_stats != NULL || max_stats != NULL) { /* allocate DMA memory if needed */
    int mem_size = 8 *   /* 4 byte words * 2 (UC, MC) * MAX{OTM_PORTS_LAST_ARRAY_INDEX, QUEUES_LAST_ARRAY_INDEX} */
      (OTM_PORTS_LAST_ARRAY_INDEX > QUEUES_LAST_ARRAY_INDEX ? OTM_PORTS_LAST_ARRAY_INDEX : QUEUES_LAST_ARRAY_INDEX);

    dma_buf = soc_cm_salloc(unit, mem_size, "cgm_statistics_mem"); /* allocate DMA memory buffer */
    if (dma_buf == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate dma memory for statistics data")));
    }
  }

  if (cur_stats != NULL) { /* collect current value statistics */

    /* register providing current values */

    /* Current total number of allocated packet descriptors */
    READ_REGISTER_NO_PIPE(CGM_TOTAL_PD_CNTr, value);
    cur_stats->pd = value; /* place the value into the 16 bits integer */
    /* cur_stats->pd= soc_reg_field_get(unit, CGM_TOTAL_PD_CNTr, value, TOTAL_PD_CNTf); */

    /* Current total number of allocated Data Buffers */
    READ_REGISTER_NO_PIPE(CGM_TOTAL_DB_CNTr, value);
    cur_stats->db = value; /* place the value into the 16 bits integer */
    /* cur_stats->db= soc_reg_field_get(unit, CGM_TOTAL_DB_CNTr, value, TOTAL_DB_CNTf); */

    /* Current number of packet descriptors allocated to unicast packets */
    READ_REGISTER_NO_PIPE(CGM_UC_PD_CNTr, value);
    cur_stats->uc_pd = value; /* place the value into the 16 bits integer */
    /* cur_stats->uc_pd= soc_reg_field_get(unit, CGM_UC_PD_CNTr, value, UC_PD_CNTf); */

    /* Current number of packet descriptors allocated to multicast replication packets */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_CNTr, value);
    cur_stats->mc_pd = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_pd= soc_reg_field_get(unit, CGM_MC_PD_CNTr, value, FIELDf); */

    /* Current number of Data Buffers allocated to unicast packets */
    READ_REGISTER_NO_PIPE(CGM_UC_DB_CNTr, value);
    cur_stats->uc_db = value; /* place the value into the 16 bits integer */
    /* cur_stats->uc_db= soc_reg_field_get(unit, CGM_UC_DB_CNTr, value, UC_DB_CNTf); */

    /* Current number of Data Buffers allocated to multicast packets, regardless of number of replications */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_CNTr, value);
    cur_stats->mc_db = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_db= soc_reg_field_get(unit, CGM_MC_DB_CNTr, value, MC_DB_CNTf); */

    /* Current number of packet descriptors allocated to unicast packets destined to this interface. */
    for (i = SOC_REG_NUMELS(unit, CGM_UC_PD_IF_CNTr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_UC_PD_IF_CNTr, i, value);
      cur_stats->uc_pd_if[i] = value; /* place the value into the 16 bits integer */
      /* cur_stats->uc_pd_if[i] = soc_reg_field_get(unit, CGM_UC_PD_IF_CNTr, value, UC_PD_IF_CNT_Nf); */
    }

    /* Current number of packet descriptors allocated to Multicast replication packets destined to this interface */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_PD_IF_CNTr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_PD_IF_CNTr, i, value);
      cur_stats->mc_pd_if[i] = value; /* place the value into the 16 bits integer */
      /* cur_stats->mc_pd_if[i] = soc_reg_field_get(unit, CGM_MC_PD_IF_CNTr, value, MC_PD_IF_CNT_Nf); */
    }

    /* The size of this interface contributed by unicast packets only, where the size is measured in 256B units. */
    for (i = SOC_REG_NUMELS(unit, CGM_UC_SIZE_256_IF_CNTr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_UC_SIZE_256_IF_CNTr, i, value);
      cur_stats->uc_size_256_if[i] = value; /* place the value into the 16 bits integer */
      /* cur_stats->uc_size_256_if[i] = soc_reg_field_get(unit, CGM_UC_SIZE_256_IF_CNTr, value, UC_SIZE_256_IF_CNT_Nf); */
    }

    /* The size of this interface contributed by Multicast replication packets only, where the size is measured in 256B units. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_SIZE_256_IF_CNTr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_SIZE_256_IF_CNTr, i, cur_stats->mc_size_256_if[i]);
      /* cur_stats->mc_size_256_if[i] = soc_reg_field_get(unit, CGM_MC_SIZE_256_IF_CNTr, value, MC_SIZE_256_IF_CNT_Nf); */
    }

    /*
    The memories need to be written to init the maximum value; unlike the registers which auto-initialize.
    This why CLEAR_MEMORY() is used below.
    */

    /* Unicast and Multicast current packet descriptors per OTM-Port */
    READ_MEMORY(EGQ_PDCMm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      cur_stats->uc_pd_port[i] = soc_mem_field32_get(unit, EGQ_PDCMm, buf_ptr, PDCMf); /* 15b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      cur_stats->mc_pd_port[i] = soc_mem_field32_get(unit, EGQ_PDCMm, buf_ptr, PDCMf); /* 15b field */
    }

    /* Unicast and Multicast current Packet descriptors per queue */
    READ_MEMORY(EGQ_QDCMm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      cur_stats->uc_pd_queue[i] = soc_mem_field32_get(unit, EGQ_QDCMm, buf_ptr, QDCMf); /* 15b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      cur_stats->mc_pd_queue[i] = soc_mem_field32_get(unit, EGQ_QDCMm, buf_ptr, QDCMf); /* 15b field */
    }

    /* Unicast and Multicast current data buffers per OTM-Port, for MC the data is size in 256 bytes */
    READ_MEMORY(EGQ_PQSMm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      cur_stats->uc_db_port[i] = soc_mem_field32_get(unit, EGQ_PQSMm, buf_ptr, PQSMf); /* 18b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      cur_stats->mc_db_port[i] = soc_mem_field32_get(unit, EGQ_PQSMm, buf_ptr, PQSMf); /* 18b field */
    }

    /* Unicast and Multicast current data buffers per queue, for MC the data is size in 256 bytes */
    READ_MEMORY(EGQ_QQSMm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      cur_stats->uc_db_queue[i] = soc_mem_field32_get(unit, EGQ_QQSMm, buf_ptr, QQSMf); /* 18b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      cur_stats->mc_db_queue[i] = soc_mem_field32_get(unit, EGQ_QQSMm, buf_ptr, QQSMf); /* 18b field */
    }


    /* Current number of packet descriptors allocated to multicast replication packets bound to Service Pool0 */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_SP_0_CNTr, value);
    cur_stats->mc_pd_sp[0] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_pd_sp[0] = soc_reg_field_get(unit, CGM_MC_PD_SP_0_CNTr, value, MC_PD_SP_0_CNTf); */

    /* Current number of packet descriptors allocated to multicast replication packets bound to Service Pool1 */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_SP_1_CNTr, value);
    cur_stats->mc_pd_sp[1] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_pd_sp[1] = soc_reg_field_get(unit, CGM_MC_PD_SP_1_CNTr, value, MC_PD_SP_1_CNTf); */

    /* Current number of Data Buffers allocated to multicast packets bound to Service Pool0 */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_SP_0_CNTr, value);
    cur_stats->mc_db_sp[0] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_db_sp[0] = soc_reg_field_get(unit, CGM_MC_DB_SP_0_CNTr, value, MC_DB_SP_0_CNTf); */

    /* Current number of Data Buffers allocated to multicast packets bound to Service Pool1 */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_SP_1_CNTr, value);
    cur_stats->mc_db_sp[1] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_db_sp[1] = soc_reg_field_get(unit, CGM_MC_DB_SP_1_CNTr, value, MC_DB_SP_1_CNTf); */


    /* The number of MC-PD'S - Per SP per TC. Indicates the value of Multicast Packet Descriptors Counter ( Per SP per TC). Low 8 counters are for service-pool-0 and high 8 counters are for service-pool-1. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_PD_SP_TC_CNTr); i;) { /* 16 array elements */
      --i;
      READ_REGISTER_ARRAY(CGM_MC_PD_SP_TC_CNTr, REG_PORT_ANY, i, value);
      cur_stats->mc_pd_sp_tc[i] = value; /* place the value into the 16 bits integer */
      /* cur_stats->mc_pd_sp_tc[i] = soc_reg_field_get(unit, CGM_MC_PD_SP_TC_CNTr, value, MC_PD_SP_TC_CNT_Nf); */
    }

    /* The number of MC-DB'S - Per SP per TC. Indicates the value of Multicast Data buffers Counter ( Per SP per TC). Low 8 counters are for service-pool-0 and high 8 counters are for service-pool-1. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_DB_SP_TC_CNTr); i;) { /* 16 array elements */
      --i;
      READ_REGISTER_ARRAY(CGM_MC_DB_SP_TC_CNTr, REG_PORT_ANY, i, value);
      cur_stats->mc_db_sp_tc[i] = value; /* place the value into the 16 bits integer */
      /* cur_stats->mc_db_sp_tc[i] = soc_reg_field_get(unit, CGM_MC_DB_SP_TC_CNTr, value, MC_DB_SP_TC_CNT_Nf); */
    }

    /* Current number of available reserved packet descriptors in Service Pool0. This counter is loaded by the CPU with the maximum number of reserved resource and is decreased for every occupied reserved resource and increased whenever such a resource is reclaimed. */
    READ_REGISTER_NO_PIPE(CGM_MC_RSVD_PD_SP_0_CNTr, value);
    cur_stats->mc_rsvd_pd_sp[0] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_rsvd_pd_sp[0] = soc_reg_field_get(unit, CGM_MC_RSVD_PD_SP_0_CNTr, value, MC_RSVD_PD_SP_0_CNTf); */

    /* Current number of available reserved packet descriptors in Service Pool1. This counter is loaded by the CPU with the maximum number of reserved resource and is decreased for every occupied reserved resource and increased whenever such a resource is reclaimed. */
    READ_REGISTER_NO_PIPE(CGM_MC_RSVD_PD_SP_1_CNTr, value);
    cur_stats->mc_rsvd_pd_sp[1] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_rsvd_pd_sp[1] = soc_reg_field_get(unit, CGM_MC_RSVD_PD_SP_1_CNTr, value, MC_RSVD_PD_SP_1_CNTf); */

    /* Current number of available reserved Data Buffers in Service Pool0. This counter is loaded by the CPU with the maximum number of reserved resource and is decreased for every occupied reserved resource and increased whenever such a resource is reclaimed. */
    READ_REGISTER_NO_PIPE(CGM_MC_RSVD_DB_SP_0_CNTr, value);
    cur_stats->mc_rsvd_db_sp[0] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_rsvd_db_sp[0] = soc_reg_field_get(unit, CGM_MC_RSVD_DB_SP_0_CNTr, value, MC_RSVD_DB_SP_0_CNTf); */

    /* Current number of available reserved Data Buffers in Service Pool1. This counter is loaded by the CPU with the maximum number of reserved resource and is decreased for every occupied reserved resource and increased whenever such a resource is reclaimed. */
    READ_REGISTER_NO_PIPE(CGM_MC_RSVD_DB_SP_1_CNTr, value);
    cur_stats->mc_rsvd_db_sp[1] = value; /* place the value into the 16 bits integer */
    /* cur_stats->mc_rsvd_db_sp[1] = soc_reg_field_get(unit, CGM_MC_RSVD_DB_SP_1_CNTr, value, MC_RSVD_DB_SP_1_CNTf); */

  } /* end of current value collection */


  if (max_stats != NULL) { /* collect maximum value statistics */

    /* disable maximum statistics updated if requested to do so */
    if (disable_updates) {
      if (WRITE_CGM_CGM_MAX_VALUES_DISABLE_UPDATEr(unit, 1)!= SOC_E_NONE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to disable maximum statistics updates")));
      }
      updates_are_disabled = 1; /* we need to later enable updates */
    }


    /* Indicates the maximum value that the Total Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_TOTAL_PD_CNT_MAX_VALUEr, value);
    max_stats->pd = value; /* place the value into the 16 bits integer */
    /* max_stats->pd = soc_reg_field_get(unit, CGM_TOTAL_PD_CNT_MAX_VALUEr, value, TOTAL_PD_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Total Data Buffers Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_TOTAL_DB_CNT_MAX_VALUEr, value);
    max_stats->db = value; /* place the value into the 16 bits integer */
    /* max_stats->db = soc_reg_field_get(unit, CGM_TOTAL_DB_CNT_MAX_VALUEr, value, TOTAL_DB_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Unicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_UC_PD_CNT_MAX_VALUEr, value);
    max_stats->uc_pd = value; /* place the value into the 16 bits integer */
    /* max_stats->uc_pd = soc_reg_field_get(unit, CGM_UC_PD_CNT_MAX_VALUEr, value, UC_PD_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Multicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_CNT_MAX_VALUEr, value);
    max_stats->mc_pd = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_pd = soc_reg_field_get(unit, CGM_MC_PD_CNT_MAX_VALUEr, value, MC_PD_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Unicast Data Buffers Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_UC_DB_CNT_MAX_VALUEr, value);
    max_stats->uc_db = value; /* place the value into the 16 bits integer */
    /* max_stats->uc_db = soc_reg_field_get(unit, CGM_UC_DB_CNT_MAX_VALUEr, value, UC_DB_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Multicast Data Buffers Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_CNT_MAX_VALUEr, value);
    max_stats->mc_db = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_db = soc_reg_field_get(unit, CGM_MC_DB_CNT_MAX_VALUEr, value, MC_DB_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Interface Unicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    for (i = SOC_REG_NUMELS(unit, CGM_UC_PD_IF_CNT_MAX_VALUEr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_UC_PD_IF_CNT_MAX_VALUEr, i, value);
      max_stats->uc_pd_if[i] = value; /* place the value into the 16 bits integer */
      /* max_stats->uc_pd_if[i] = soc_reg_field_get(unit, CGM_UC_PD_IF_CNT_MAX_VALUEr, value, UC_PD_IF_CNT_MAX_VAL_Nf); */
    }

    /* Indicates the maximum value that the Interface Multicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_PD_IF_CNT_MAX_VALUEr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_PD_IF_CNT_MAX_VALUEr, i, value);
      max_stats->mc_pd_if[i] = value; /* place the value into the 16 bits integer */
      /* max_stats->mc_pd_if[i] = soc_reg_field_get(unit, CGM_MC_PD_IF_CNT_MAX_VALUEr, value, MC_PD_IF_CNT_MAX_VAL_Nf); */
    }

    /* Indicates the maximum Interface length, contributed by unicast packets, where length is mesured in units of 256 bytes. */
    for (i = SOC_REG_NUMELS(unit, CGM_UC_SIZE_256_IF_CNT_MAX_VALUEr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_UC_SIZE_256_IF_CNT_MAX_VALUEr, i, value);
      max_stats->uc_size_256_if[i] = value; /* place the value into the 16 bits integer */
      /* max_stats->uc_size_256_if[i] = soc_reg_field_get(unit, CGM_UC_SIZE_256_IF_CNT_MAX_VALUEr, value, UC_SIZE_256_IF_CNT_MAX_VAL_Nf); */
    }

    /* The maximum size of this interface contributed by Multicast replication packets only, where the size is measured in 256B units. */
    /* Indicates the maximum value that the Interface Multicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_SIZE_256_IF_CNT_MAX_VALUEr); i;) { /* 32 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_SIZE_256_IF_CNT_MAX_VALUEr, i, max_stats->mc_size_256_if[i]);
      /* max_stats->mc_size_256_if[i] = soc_reg_field_get(unit, CGM_MC_SIZE_256_IF_CNT_MAX_VALUEr, value, MC_SIZE_256_IF_CNT_MAX_VAL_Nf); */
    }

    /*
    The memories need to be written to init the maximum value; unlike the registers which auto-initialize.
    This why CLEAR_MEMORY() is used below.
    */

    /* Unicast and Multicast maximum packet descriptors per OTM-Port */
    READ_MEMORY(EGQ_PDCMAXm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      max_stats->uc_pd_port[i] = soc_mem_field32_get(unit, EGQ_PDCMAXm, buf_ptr, PDCMAXf); /* 15b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      max_stats->mc_pd_port[i] = soc_mem_field32_get(unit, EGQ_PDCMAXm, buf_ptr, PDCMAXf); /* 15b field */
    }
    CLEAR_MEMORY(EGQ_PDCMAXm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);

    /* Unicast and Multicast maximum Packet descriptors per queue */
    READ_MEMORY(EGQ_QDCMAXm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      max_stats->uc_pd_queue[i] = soc_mem_field32_get(unit, EGQ_QDCMAXm, buf_ptr, QDCMAXf); /* 15b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      max_stats->mc_pd_queue[i] = soc_mem_field32_get(unit, EGQ_QDCMAXm, buf_ptr, QDCMAXf); /* 15b field */
    }
    CLEAR_MEMORY(EGQ_QDCMAXm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);

    /* Unicast and Multicast maximum data buffers per OTM-Port */
    READ_MEMORY(EGQ_PQSMAXm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      max_stats->uc_db_port[i] = soc_mem_field32_get(unit, EGQ_PQSMAXm, buf_ptr, PQSMAXf); /* 18b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_OTM_PORTS_NUM; ++i, ++buf_ptr) {
      max_stats->mc_db_port[i] = soc_mem_field32_get(unit, EGQ_PQSMAXm, buf_ptr, PQSMAXf); /* 18b field */
    }
    CLEAR_MEMORY(EGQ_PQSMAXm, 0, OTM_PORTS_LAST_ARRAY_INDEX, dma_buf);

    /* Unicast and Multicast maximum data buffers per queue */
    READ_MEMORY(EGQ_QQSMAXm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);
    for (i = 0, buf_ptr = dma_buf; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      max_stats->uc_db_queue[i] = soc_mem_field32_get(unit, EGQ_QQSMAXm, buf_ptr, QQSMAXf); /* 18b field */
    }
    for (i = 0; i < ARAD_EGR_CGM_QUEUES_NUM; ++i, ++buf_ptr) {
      max_stats->mc_db_queue[i] = soc_mem_field32_get(unit, EGQ_QQSMAXm, buf_ptr, QQSMAXf); /* 18b field */
    }
    CLEAR_MEMORY(EGQ_QQSMAXm, 0, QUEUES_LAST_ARRAY_INDEX, dma_buf);


    /* Indicates the maximum value that the Service Pool0 Multicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_SP_0_CNT_MAX_VALUEr, value);
    max_stats->mc_pd_sp[0] = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_pd_sp[0] = soc_reg_field_get(unit, CGM_MC_PD_SP_0_CNT_MAX_VALUEr, value, MC_PD_SP_0_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Service Pool1 Multicast Packet Descriptors Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_PD_SP_1_CNT_MAX_VALUEr, value);
    max_stats->mc_pd_sp[1] = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_pd_sp[1] = soc_reg_field_get(unit, CGM_MC_PD_SP_1_CNT_MAX_VALUEr, value, MC_PD_SP_1_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Service Pool0 Multicast Data Buffers Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_SP_0_CNT_MAX_VALUEr, value);
    max_stats->mc_db_sp[0] = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_db_sp[0] = soc_reg_field_get(unit, CGM_MC_DB_SP_0_CNT_MAX_VALUEr, value, MC_DB_SP_0_CNT_MAX_VALf); */

    /* Indicates the maximum value that the Service Pool1 Multicast Data Buffers Counter has reached since the last time it was read by the CPU. */
    READ_REGISTER_NO_PIPE(CGM_MC_DB_SP_1_CNT_MAX_VALUEr, value);
    max_stats->mc_db_sp[1] = value; /* place the value into the 16 bits integer */
    /* max_stats->mc_db_sp[1] = soc_reg_field_get(unit, CGM_MC_DB_SP_1_CNT_MAX_VALUEr, value, MC_DB_SP_1_CNT_MAX_VALf); */


    /* The max number of MC-PD'S - Per SP per TC. Indicates the maximum value of Multicast Packet Descriptors Counter ( Per SP per TC). Low 8 counters are for service-pool-0 and high 8 counters are for service-pool-1. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_PD_SP_TC_CNT_MAX_VALUEr); i;) { /* 16 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_PD_SP_TC_CNT_MAX_VALUEr, i, value);
      max_stats->mc_pd_sp_tc[i] = value; /* place the value into the 16 bits integer */
      /* max_stats->mc_pd_sp_tc[i] = soc_reg_field_get(unit, CGM_MC_PD_SP_TC_CNT_MAX_VALUEr, value, MC_PD_SP_TC_CNT_MAX_VAL_Nf); */
    }

    /* The max number of MC-DB'S - Per SP per TC. Indicates the maximum value of Multicast Data Buffers Counter ( Per SP per TC). Low 8 counters are for service-pool-0 and high 8 counters are for service-pool-1. */
    for (i = SOC_REG_NUMELS(unit, CGM_MC_DB_SP_TC_CNT_MAX_VALUEr); i;) { /* 16 array elements */
      --i;
      READ_REGISTER_ARRAY_NO_PIPE(CGM_MC_DB_SP_TC_CNT_MAX_VALUEr, i, value);
      max_stats->mc_db_sp_tc[i] = value; /* place the value into the 16 bits integer */
      /* max_stats->mc_db_sp_tc[i] = soc_reg_field_get(unit, CGM_MC_DB_SP_TC_CNT_MAX_VALUEr, value, MC_PD_SP_TC_CNT_MAX_VAL_Nf); */
    }

    /* disable maximum statistics updated if requested to do so */
    if (updates_are_disabled) {
      if (WRITE_CGM_CGM_MAX_VALUES_DISABLE_UPDATEr(unit, 0) != SOC_E_NONE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Failed to re-enable maximum statistics updates")));
      }
      updates_are_disabled = 0;
    }

    /* these statistics are not supported for maximum values */
    max_stats->mc_rsvd_pd_sp[0] = max_stats->mc_rsvd_pd_sp[1] =
    max_stats->mc_rsvd_db_sp[0] = max_stats->mc_rsvd_db_sp[1] = 0;

  } /* end of maximujm value collection */

  if (counters != NULL) {
    /* registers providing counter values */

    /* counts the number of packets that were dropped due to lack of unicast packet descriptors */
    READ_REGISTER_NO_PIPE(CGM_CGM_UC_PD_DROPPED_CNT_VALUEr, counters->uc_pd_dropped);
    /* counters->uc_pd_dropped = soc_reg_field_get(unit, CGM_CGM_UC_PD_DROPPED_CNT_VALUEr, value, CGM_UC_PD_DROPPED_CNTf); */

    /* counts the number of packets that were dropped due to lack of multicast packet descriptors */
    READ_REGISTER_NO_PIPE(CGM_CGM_MC_REP_PD_DROPPED_CNT_VALUEr, counters->mc_rep_pd_dropped);
    /* counters->mc_rep_pd_dropped = soc_reg_field_get(unit, CGM_CGM_MC_REP_PD_DROPPED_CNT_VALUEr, value, CGM_MC_REP_PD_DROPPED_CNTf); */

    /* counts the number of packets that were dropped by the RQP due to lack of unicast data buffers */
    READ_REGISTER_NO_PIPE(CGM_CGM_UC_DB_DROPPED_BY_RQP_CNT_VALUEr, counters->uc_db_dropped_by_rqp);
    /* counters->uc_db_dropped_by_rqp = soc_reg_field_get(unit, CGM_CGM_UC_DB_DROPPED_BY_RQP_CNT_VALUEr, value, CGM_UC_DB_DROPPED_BY_RQP_CNTf); */

    /* counts the number of packets that were dropped by the PQP due to lack of unicast data buffers */
    READ_REGISTER_NO_PIPE(CGM_CGM_UC_DB_DROPPED_BY_PQP_CNT_VALUEr, counters->uc_db_dropped_by_pqp);
    /* counters->uc_db_dropped_by_pqp = soc_reg_field_get(unit, CGM_CGM_UC_DB_DROPPED_BY_PQP_CNT_VALUEr, value, CGM_UC_DB_DROPPED_BY_PQP_CNTf); */

    /* Counts the number of packets that were dropped due to lack of multicast data buffers. Note that this counter does not count each replication drop but rather the packet drops before replication, i.e. when a packet with n replication is dropped due to lack of multicast data buffers, the counter is increased by one regardless of number of replications. */
    READ_REGISTER_NO_PIPE(CGM_CGM_MC_DB_DROPPED_CNT_VALUEr, counters->mc_db_dropped);
    /* counters->mc_db_dropped = soc_reg_field_get(unit, CGM_CGM_MC_DB_DROPPED_CNT_VALUEr, value, CGM_MC_DB_DROPPED_CNTf); */

    /* Counts the number of multicast replications that were dropped due to reaching to the maximum port or queue length, where length is mesured in units of 256 bytes. */
    READ_REGISTER_NO_PIPE(CGM_CGM_MC_REP_DB_DROPPED_CNT_VALUEr, counters->mc_rep_db_dropped);
    /* counters->mc_rep_db_dropped = soc_reg_field_get(unit, CGM_CGM_MC_REP_DB_DROPPED_CNT_VALUEr, value, CGM_MC_REP_DB_DROPPED_CNTf); */
  }

exit:

  /* disable maximum statistics updated if requested to do so */
  if (updates_are_disabled) {
    if(WRITE_CGM_CGM_MAX_VALUES_DISABLE_UPDATEr(unit, 0) != SOC_E_NONE) {
       LOG_ERROR(BSL_LS_SOC_EGRESS,
                 (BSL_META_U(unit,
                             "Failed write to register CGM_CGM_MAX_VALUES_DISABLE_UPDATE")));
    }
  }

  if (dma_buf) { /* free DMA memory if it was allocated */
    soc_cm_sfree(unit, dma_buf);
  }
  SOCDNX_FUNC_RETURN;
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

