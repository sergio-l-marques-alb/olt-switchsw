/* $Id: pb_egr_queuing.c,v 1.12 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_api_egr_queuing.h>

#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_ofp_rates.h>

#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_TC_NDX_MAX                                          (7)
#define SOC_PB_DP_NDX_MAX                                          (3)
#define SOC_PB_MAP_TYPE_NDX_MAX                                    (SOC_PETRA_EGR_NOF_Q_PRIO_MAPPING_TYPES-1)
#define SOC_PB_MAP_PROFILE_NDX_MAX                                 (3)
#define SOC_PB_EGR_Q_PRIORITY_TC_MAX                               (1)
#define SOC_PB_EGR_Q_PRIORITY_DP_MAX                               (3)
#define SOC_PB_EGR_MAX_CAL_SIZE                                    (128)
#define SOC_PB_EGR_MAX_MAL_INDX                                    (127)

#define SOC_PB_EGR_XAUI_RATE                                      (10)
#define SOC_PB_EGR_RXAUI_RATE                                     (10)
#define SOC_PB_EGR_SGMII_RATE                                      (1)
#define SOC_PB_EGR_QSGMII_RATE                                     (1)
#define SOC_PB_EGR_ILKN_RATE                                      (130)
#define SOC_PB_EGR_OTHERS_INDX                                    (64)

#define SOC_PB_EGR_MAXIMAL_NUMBER_OF_MUX_COMPETITORS              (65)

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
  SOC_PB_NIF_TYPE nif_type;
  uint32    mal_indx;
  uint32    rate;
} SOC_PB_NIF_PORT_RATE;

typedef struct
{
  SOC_PB_NIF_PORT_RATE nif_prt_rts[SOC_PB_NIF_NOF_NIFS];
  uint32 nof_nifs;
} SOC_PB_NIF_PORT_RATES;
/* } */

/*************
 * GLOBALS   *
 *************/

/*************
 * FUNCTIONS *
 *************/
/* { */

/* $Id: pb_egr_queuing.c,v 1.12 Broadcom SDK $
 *	Initializes the NIF-Ports calendar to some according to nif port configuration.
 */



/*********************************************************************
* NAME:
*     SOC_PB_NIF_PORT_RATES_clear
* TYPE:
*   PROC
* DATE:
*   Jul 22 2010
* FUNCTION:
*         Initialize Nif Rates struct
* INPUT:
*         SOC_SAND_IN int unit
*
* REMARKS:
*
*
* RETURNS:
*     void.
*********************************************************************/

STATIC void
    SOC_PB_NIF_PORT_RATES_clear(
      SOC_SAND_OUT SOC_PB_NIF_PORT_RATES  *nif_rates
    )
{
  uint32 ind;

  nif_rates->nof_nifs = 0;

  for( ind = 0; ind < SOC_PB_NIF_NOF_NIFS ; ind ++)
  {
    nif_rates->nif_prt_rts[ind].mal_indx = SOC_PB_EGR_MAX_MAL_INDX;
    nif_rates->nif_prt_rts[ind].nif_type = SOC_PB_NIF_TYPE_NONE;
    nif_rates->nif_prt_rts[ind].rate = 0;
  }
}

/*********************************************************************
* NAME:
*     soc_pb_egr_q_nif_cal_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Jul 22 2010
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
*         5. Using the values computed in 4. to initialize SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL address: 0x004a0000
*
*         The calender should admit the constraint derived from the formula BW=(128 * 8)/(ClkFreq * # Clk )
*         => # Clk = (128 * 8)/(ClkFreq * BW) Given that the clock frequency is 3.3 Ns
*         WE get #Clk= (128 * 8) /(3.3 * BW)
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_pb_egr_q_nif_cal_set_unsafe(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,
    nof_lanes,
    mal_id,
    nif_id = 0;
  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA
    nif_port_mux_tbl;
  SOC_PB_NIF_PORT_RATES
    nif_rates ;
  uint32
    nif_base_id,
    nif_last_id,
    ports_rates[SOC_PB_EGR_MAX_CAL_SIZE],
    total_credit_bandwidth = 0,
    calendar_len = 0;
  SOC_PETRA_OFP_RATES_CAL_SCH
    *calendar = NULL;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_INTERFACE_ID
    nif_idx;
  SOC_PB_NIF_ILKN_ID
    prm_ilkn_ndx;
  SOC_PB_NIF_ILKN_INFO
    prm_info_rx,
    prm_info_tx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_QUEUING_INIT);

  SOC_PETRA_ALLOC(calendar, SOC_PETRA_OFP_RATES_CAL_SCH, 1);

  /* 1. Classifying the nif, mal configuration */
  SOC_PB_NIF_PORT_RATES_clear(&nif_rates);

  for ( mal_id = 0; mal_id < SOC_PB_NOF_MAC_LANES ; mal_id ++)
  {
    res = soc_pb_nif_mal_type_get(
                 unit,
                 mal_id,
                 &nif_type
                 );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    
    switch(nif_type)
    {
    case SOC_PB_NIF_TYPE_NONE:
    break;
  
   case SOC_PB_NIF_TYPE_XAUI:
     if (!SOC_PB_NIF_IS_BASE_MAL(mal_id))
     {
       continue;
     }
    
     nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
     nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_XAUI, nif_id);
     nif_rates.nif_prt_rts[nif_id].mal_indx = mal_id;
     nif_rates.nif_prt_rts[nif_id].nif_type = SOC_PB_NIF_TYPE_XAUI;
     nif_rates.nif_prt_rts[nif_id].rate = SOC_PB_EGR_XAUI_RATE;
     nif_rates.nof_nifs++;
    break;
    
   case SOC_PB_NIF_TYPE_SGMII:
      nif_base_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_last_id = SOC_SAND_RNG_LAST(nif_base_id, 2*SOC_PB_SGMII_NIFS_PER_MAL);
      for (nif_id = nif_base_id; nif_id <= nif_last_id; nif_id+=2)
      {
        nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_SGMII, nif_id);

        nif_rates.nif_prt_rts[nif_id].mal_indx = mal_id;

        nif_rates.nif_prt_rts[nif_id].nif_type = SOC_PB_NIF_TYPE_SGMII;

        nif_rates.nif_prt_rts[nif_id].rate = SOC_PB_EGR_SGMII_RATE;
        
        nif_rates.nof_nifs++;
      }
    break;
    
   case SOC_PB_NIF_TYPE_RXAUI:
      nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_RXAUI, nif_id);
      nif_rates.nif_prt_rts[nif_id].mal_indx = mal_id;
      nif_rates.nif_prt_rts[nif_id].nif_type = SOC_PB_NIF_TYPE_RXAUI;
      nif_rates.nif_prt_rts[nif_id].rate = SOC_PB_EGR_RXAUI_RATE;
      nif_rates.nof_nifs++;
    break;

   case SOC_PB_NIF_TYPE_QSGMII:

     nif_base_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
     nif_last_id = SOC_SAND_RNG_LAST(nif_base_id, SOC_PB_QSGMII_NIFS_PER_MAL);
     for (nif_id = nif_base_id; nif_id <= nif_last_id; nif_id++)
     {
       nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_QSGMII, nif_id);
       nif_rates.nif_prt_rts[nif_id].mal_indx = mal_id;
       nif_rates.nif_prt_rts[nif_id].nif_type = SOC_PB_NIF_TYPE_QSGMII;
       nif_rates.nif_prt_rts[nif_id].rate = SOC_PB_EGR_QSGMII_RATE;
       nif_rates.nof_nifs++;
     }
    break;

    case SOC_PB_NIF_TYPE_ILKN:
     if(SOC_PB_NIF_MAL2ILKN(mal_id)!= SOC_PETRA_NIF_ID_NONE)
      {
        nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
        nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_ILKN, nif_id);
        nif_rates.nif_prt_rts[nif_id].mal_indx = mal_id;
        nif_rates.nif_prt_rts[nif_id].nif_type = SOC_PB_NIF_TYPE_ILKN;
        prm_ilkn_ndx = nif_idx;

        res = soc_pb_nif_ilkn_get(
          unit,
          prm_ilkn_ndx,
          &prm_info_rx,
          &prm_info_tx
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        nof_lanes = soc_pb_nif_ilkn_actual_nof_lanes_get(
                      prm_ilkn_ndx,
                      &prm_info_tx
                      );

        nif_rates.nif_prt_rts[nif_id].rate = (SOC_PB_EGR_ILKN_RATE * nof_lanes )/SOC_PB_NIF_ILKN_NOF_LANES_MAX;
        nif_rates.nof_nifs++;
       }
    break;

    default:
      break;
    }
  }

  for (ind = 0; ind < SOC_PB_EGR_MAX_CAL_SIZE ; ind++ )
  {
    ports_rates[ind] = 0;
  }
 
  /* 2. Giving each nif  weight according to its type */
  for (ind = 0; ind < SOC_PB_NIF_NOF_NIFS ; ind++ )
  {
    ports_rates[ind] = nif_rates.nif_prt_rts[ind].rate;
    total_credit_bandwidth += nif_rates.nif_prt_rts[ind].rate;
  }

/* 3. Adding the "others" processes of recycling, deleting as competitor with a given value */
  
  ports_rates[SOC_PB_EGR_MAXIMAL_NUMBER_OF_MUX_COMPETITORS-1] = SOC_PB_EGQ_NIF_OTHERS_DEF_VAL;
  total_credit_bandwidth += SOC_PB_EGQ_NIF_OTHERS_DEF_VAL;
  
/* 4. Computing the optimal calender using "black box" soc_pb_ofp_rates_from_rates_to_calendar function  */

  res = soc_pb_ofp_rates_from_rates_to_calendar(
          unit,
          ports_rates,
          (SOC_PB_EGR_MAXIMAL_NUMBER_OF_MUX_COMPETITORS),
          total_credit_bandwidth,
          SOC_PB_EGR_MAX_CAL_SIZE,
          calendar,
          &calendar_len
        );

/* 5. Using the values computed in 4. to initialize SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL address: 0x004a0000 */
  for (nif_id = 0; nif_id < calendar_len; nif_id++)
  {

    if(calendar->slots[nif_id] == SOC_PB_EGR_OTHERS_INDX)
    {
      nif_port_mux_tbl.fqp_nif_port_mux = SOC_PB_EGQ_NIF_PORT_CAL_BW_GIVE_UPON_REQUEST;
    }
    else
    {
      nif_port_mux_tbl.fqp_nif_port_mux = calendar->slots[nif_id];
    }

    res = soc_pb_egq_fqp_nif_port_mux_tbl_set_unsafe(
            unit,
            nif_id,
            &nif_port_mux_tbl
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
 
exit:
  SOC_PETRA_FREE(calendar);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_queuing_init()",0,0);

}


uint32
  soc_pb_egr_q_nif_cal_set_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_SET_VERIFY);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_nif_cal_set_verify()", 0, 0);
}


uint32
  soc_pb_egr_q_nif_cal_get_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_GET_VERIFY);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_nif_cal_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets egress queue calendar which control the service
 *     times of the nif ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_get_unsafe(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_GET_UNSAFE);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_nif_cal_get_unsafe()", 0, 0);
}

  


/*********************************************************************
* NAME:
*     soc_pb_egr_queuing_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_egr_queuing_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_EGR_DROP_THRESH
    drop_conf;
  SOC_PETRA_EGR_DROP_THRESH
    exact_drop_conf;
  SOC_PETRA_INTERFACE_ID
    if_idx;
  SOC_PETRA_EGR_FC_CHNIF_THRESH
    chnif_conf;
  SOC_PETRA_EGR_FC_CHNIF_THRESH
    exact_chnif_conf;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_QUEUING_INIT);

  regs = soc_petra_regs();
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(&chnif_conf);
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(&exact_chnif_conf);

  /*
   *  Egress Drop Thresholds - unscheduled
   */
  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 0, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 0, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 1, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 1, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 2, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 2, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 3, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 3, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 0, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 0, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 1, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 36864 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 1, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 2, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 34816 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 2, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_egr_unsched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 3, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  drop_conf.buffers_used = 2048;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 32768 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_pb_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 3, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);


  res = soc_petra_egr_sched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] = 32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_sched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_egr_sched_drop_get_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, &drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] = 32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_pkts_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 122880 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_sched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  /*
   *	Set the OC768 scheduling mode as Soc_petra-A (SP for Low-Priority
   *  traffic between Unicast and Multicast)
   */
  fld_val = 0x0;
  SOC_PB_FLD_SET(regs->egq.oc768_unicast_multicast_weight_reg.fast_port_new_unicast_weight, fld_val, 10, exit);
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->egq.oc768_unicast_multicast_weight_reg.fast_port_new_multicast_weight, fld_val, 15, exit);

  for (if_idx = SOC_PB_NIF_ID_RXAUI_FIRST; if_idx <= SOC_PB_NIF_ID_RXAUI_LAST; ++if_idx)
  {
    chnif_conf.words = 1015808 / SOC_SAND_NOF_BITS_IN_UINT32;
    chnif_conf.packets = 7936;
    res = soc_petra_egr_xaui_spaui_fc_set_unsafe(unit, if_idx, &chnif_conf, &exact_chnif_conf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  res = soc_pb_egr_q_nif_cal_set_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_queuing_init()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_UNSCHED_DROP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_wrds),
           thresh->words_consumed,
           &(exact_thresh->words_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_pkts),
           thresh->packets_consumed,
           &(exact_thresh->packets_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx], thresh_reg, 30, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_desc),
           thresh->descriptors_avail,
           &(exact_thresh->descriptors_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_bufs),
           thresh->buffers_avail,
           &(exact_thresh->buffers_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx], thresh_reg, 60, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].total_usc_bufs),
           thresh->buffers_used,
           &(exact_thresh->buffers_used),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type0),
           thresh->queue_words_consumed[0],
           &(exact_thresh->queue_words_consumed[0]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx], thresh_reg, 90, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type1),
           thresh->queue_words_consumed[1],
           &(exact_thresh->queue_words_consumed[1]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type2),
           thresh->queue_words_consumed[2],
           &(exact_thresh->queue_words_consumed[2]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx], thresh_reg, 120, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx].uch_queue_wrds_type3),
           thresh->queue_words_consumed[3],
           &(exact_thresh->queue_words_consumed[3]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx].uch_queue_pkts_type0),
           thresh->queue_pkts_consumed[0],
           &(exact_thresh->queue_pkts_consumed[0]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx], thresh_reg, 130, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx].uch_queue_pkts_type1),
           thresh->queue_pkts_consumed[1],
           &(exact_thresh->queue_pkts_consumed[1]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx].uch_queue_pkts_type2),
           thresh->queue_pkts_consumed[2],
           &(exact_thresh->queue_pkts_consumed[2]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx], thresh_reg, 160, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_6[prio_ndx][dp_ndx].uch_queue_pkts_type3),
           thresh->queue_pkts_consumed[3],
           &(exact_thresh->queue_pkts_consumed[3]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  SOC_PB_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_6[prio_ndx][dp_ndx], thresh_reg, 180, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_unsched_drop_set_unsafe()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_UNSCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PB_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx], thresh_reg, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_wrds),
          &thresh_reg,
          &(thresh->words_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_pkts),
          &thresh_reg,
          &(thresh->packets_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx], thresh_reg, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_desc),
          &thresh_reg,
          &(thresh->descriptors_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_bufs),
          &thresh_reg,
          &(thresh->buffers_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);


  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx], thresh_reg, 90, exit);

   res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].total_usc_bufs),
          &thresh_reg,
          &(thresh->buffers_used)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

 res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type0),
          &thresh_reg,
          &(thresh->queue_words_consumed[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx], thresh_reg, 120, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type1),
          &thresh_reg,
          &(thresh->queue_words_consumed[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);


  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type2),
          &thresh_reg,
          &(thresh->queue_words_consumed[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx], thresh_reg, 120, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx].uch_queue_wrds_type3),
          &thresh_reg,
          &(thresh->queue_words_consumed[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

 res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_4[prio_ndx][dp_ndx].uch_queue_pkts_type0),
          &thresh_reg,
          &(thresh->queue_pkts_consumed[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx], thresh_reg, 160, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx].uch_queue_pkts_type1),
          &thresh_reg,
          &(thresh->queue_pkts_consumed[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);


  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_5[prio_ndx][dp_ndx].uch_queue_pkts_type2),
          &thresh_reg,
          &(thresh->queue_pkts_consumed[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  SOC_PB_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_6[prio_ndx][dp_ndx], thresh_reg, 190, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PB_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_6[prio_ndx][dp_ndx].uch_queue_pkts_type3),
          &thresh_reg,
          &(thresh->queue_pkts_consumed[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_unsched_drop_get_unsafe()",0,0);
}

/*
 *	Internal conversion to the table entry format
 */
STATIC
  uint32
    soc_pb_egr_q_prio_map_entry_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
      SOC_SAND_IN  int                        is_sched_mc,
      SOC_SAND_IN  uint32                     tc_ndx,
      SOC_SAND_IN  uint32                     dp_ndx,
      SOC_SAND_IN  uint32                     map_profile_ndx,
      SOC_SAND_OUT SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY    *entry
    )
{
  uint8
    is_egr_mc = FALSE,
    is_ing_mc = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_MAP_ENTRY_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_clear(entry);

  entry->map_profile = map_profile_ndx;
  entry->dp = dp_ndx;
  entry->tc = tc_ndx;

  switch(map_type_ndx)
  {
  case SOC_PETRA_EGR_UCAST_TO_SCHED:
      if(is_sched_mc) {
          is_ing_mc = TRUE;
          is_egr_mc = FALSE;
      } else {
          is_ing_mc = FALSE;
          is_egr_mc = FALSE;
      }
    break;

  case SOC_PETRA_EGR_MCAST_TO_UNSCHED:
    is_ing_mc = TRUE;
    is_egr_mc = TRUE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

  entry->is_egr_mc = is_egr_mc;
  entry->is_syst_mc = is_ing_mc;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_map_entry_get()", 0, 0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_prio_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY
    entry;
  SOC_PB_EGQ_TC_DP_MAP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority);
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_clear(&entry);

  tbl_data.dp = priority->dp;
  tbl_data.tc = priority->tc;

  res = soc_pb_egr_q_prio_map_entry_get(
          unit,
          map_type_ndx,
          0,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_egq_tc_dp_map_tbl_set_unsafe(
          unit,
          &entry,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Set same value for Sched UC*/
  if(map_type_ndx == SOC_PETRA_EGR_UCAST_TO_SCHED) {
        res = soc_pb_egr_q_prio_map_entry_get(
          unit,
          map_type_ndx,
          1,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          &entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      res = soc_pb_egq_tc_dp_map_tbl_set_unsafe(
              unit,
              &entry,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_set_unsafe()", 0, tc_ndx);
}

uint32
  soc_pb_egr_q_prio_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_PB_TC_NDX_MAX, SOC_PB_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_PB_DP_NDX_MAX, SOC_PB_DP_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(map_type_ndx, SOC_PB_MAP_TYPE_NDX_MAX, SOC_PB_MAP_TYPE_NDX_OUT_OF_RANGE_ERR, 25, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(map_profile_ndx, SOC_PB_MAP_PROFILE_NDX_MAX, SOC_PB_MAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_EGR_Q_PRIORITY, priority, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_set_verify()", 0, tc_ndx);
}

uint32
  soc_pb_egr_q_prio_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tc_ndx, SOC_PB_TC_NDX_MAX, SOC_PB_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(dp_ndx, SOC_PB_DP_NDX_MAX, SOC_PB_DP_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(map_profile_ndx, SOC_PB_MAP_PROFILE_NDX_MAX, SOC_PB_MAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(map_type_ndx, SOC_PB_MAP_TYPE_NDX_MAX, SOC_PB_MAP_TYPE_NDX_OUT_OF_RANGE_ERR, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_get_verify()", 0, tc_ndx);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_prio_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY             *priority
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY
    entry;
  SOC_PB_EGQ_TC_DP_MAP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority);

  SOC_PB_EGR_Q_PRIORITY_clear(priority);
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_clear(&entry);

  res = soc_pb_egr_q_prio_map_entry_get(
          unit,
          map_type_ndx,
          0,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          &entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_egq_tc_dp_map_tbl_get_unsafe(
          unit,
          &entry,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  priority->dp = tbl_data.dp;
  priority->tc = tbl_data.tc;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_get_unsafe()", 0, tc_ndx);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_idx,
    fld_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_SET_UNSAFE);

  regs = soc_petra_regs();

  reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PB_EGQ_TC_DP_MAP_PROFILE_REG_MULT_NOF_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PB_EGQ_TC_DP_MAP_PROFILE_REG_MULT_NOF_FLDS);

  SOC_PB_FLD_SET(regs->egq.tc_dp_map_profile_reg[reg_idx].tc_dp_map_profile[fld_idx], map_profile_id, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_set_unsafe()", ofp_ndx, map_profile_id);
}

uint32
  soc_pb_egr_q_profile_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_SET_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(map_profile_id, SOC_PB_MAP_PROFILE_NDX_MAX, SOC_PB_MAP_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_set_verify()", ofp_ndx, map_profile_id);
}

uint32
  soc_pb_egr_q_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_GET_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_get_verify()", ofp_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *map_profile_id
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    reg_idx,
    fld_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(map_profile_id);

  regs = soc_petra_regs();

  reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PB_EGQ_TC_DP_MAP_PROFILE_REG_MULT_NOF_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PB_EGQ_TC_DP_MAP_PROFILE_REG_MULT_NOF_FLDS);

  SOC_PB_FLD_GET(regs->egq.tc_dp_map_profile_reg[reg_idx].tc_dp_map_profile[fld_idx], fld_val, 10, exit);
  *map_profile_id = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_get_unsafe()", ofp_ndx, 0);
}

uint32
  SOC_PB_EGR_Q_PRIORITY_verify(
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_PB_EGR_Q_PRIORITY_TC_MAX, SOC_PB_EGR_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_PB_EGR_Q_PRIORITY_DP_MAX, SOC_PB_EGR_DP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_EGR_Q_PRIORITY_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
   uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0,
    if_idx;
  uint32
    reg_idx = 0,
    fld_idx = 0;
  SOC_PETRA_REG_ADDR
    *addr = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_CHNIF_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if_idx = SOC_PB_NIF_IS_PB_ID(if_ndx) ? soc_pb_nif2intern_id(if_ndx) : if_ndx;
  if (soc_petra_is_channelized_interface_id(if_idx) == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR, 14, exit);
  }

  regs = soc_petra_regs();

  if (SOC_PB_IS_CPU_IF_ID(if_idx))
  {
    reg_idx = SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_PACKETS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS - 1; /* 7 */
    fld_idx = 0;
  }
  else if (SOC_PB_IS_RCY_IF_ID(if_idx))
  {
    reg_idx = SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_PACKETS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS - 1; /* 7 */
    fld_idx = SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS - 1; /* 1 */
  }
  else if (soc_petra_is_channelized_nif_id(if_idx))
  {
    reg_idx = SOC_PB_NIF2MAL_GLBL_ID(if_idx) / SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS;
    fld_idx = if_idx % SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }
  
  SOC_SAND_ERR_IF_ABOVE_NOF(reg_idx, SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_PACKETS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS, SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 20, exit);

  fld = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_flow_control_threshold_reg[reg_idx].total_cp_npkts_th[fld_idx]);
  addr = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_flow_control_threshold_reg[reg_idx].addr);

  res = soc_petra_read_reg_unsafe(
          unit,
          &(*addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   res = soc_petra_egr_mnt_exp_to_thresh_fld(
           fld,
           &thresh_reg,
           &(thresh->packets)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  fld = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_flow_control_threshold_reg[reg_idx].total_cp_nwrds_th[fld_idx]);
  addr = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_flow_control_threshold_reg[reg_idx].addr);
  res = soc_petra_read_reg_unsafe(
          unit,
          addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           fld,
           &thresh_reg,
           &(thresh->words)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_xaui_spaui_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0,
    if_idx;
  uint32
    reg_idx = 0,
    fld_idx = 0;
  uint32
    thresh_fld = 0;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_CHNIF_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if_idx = SOC_PB_NIF_IS_PB_ID(if_ndx) ? soc_pb_nif2intern_id(if_ndx) : if_ndx;
  if (soc_petra_is_channelized_interface_id(if_idx) == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR, 14, exit);
  }
  
  regs = soc_petra_regs();

  if (SOC_PB_IS_CPU_IF_ID(if_idx))
  {
    reg_idx = SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_WORDS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS - 1; /* 8 */
    fld_idx = 0;
  }
  else if (SOC_PB_IS_RCY_IF_ID(if_idx))
  {
    reg_idx = SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_WORDS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS - 1; /* 8 */
    fld_idx = SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS - 1; /* 1 */
  }
  else if (soc_petra_is_channelized_nif_id(if_idx))
  {
    reg_idx = SOC_PB_NIF2MAL_GLBL_ID(if_idx) / SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS;
    fld_idx = if_idx % SOC_PB_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  if(reg_idx >= SOC_PB_EGQ_CHANNELIZED_PORTS_TOTAL_PACKETS_FLOW_CONTROL_THRESHOLD_REG_MULT_NOF_REGS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  fld = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_flow_control_threshold_reg[reg_idx].total_cp_npkts_th[fld_idx]);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
    fld,
    thresh->packets,
    &(exact_thresh->packets),
    &thresh_reg
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_IMPLICIT_FLD_FROM_REG(*fld, thresh_fld, thresh_reg, 30, exit);

  SOC_PB_IMPLICIT_FLD_SET(*fld, thresh_fld, 40, exit);

  fld = SOC_PB_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_flow_control_threshold_reg[reg_idx].total_cp_nwrds_th[fld_idx]);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
          fld,
          thresh->words,
          &(exact_thresh->words),
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PB_IMPLICIT_FLD_FROM_REG(*fld, thresh_fld, thresh_reg, 60, exit);

  SOC_PB_IMPLICIT_FLD_SET(*fld, thresh_fld, 70, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_xaui_spaui_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;
  uint32
    if_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_CHNIF_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if_idx = SOC_PB_NIF_IS_PB_ID(if_ndx) ? soc_pb_nif2intern_id(if_ndx) : if_ndx;
  if (soc_petra_is_channelized_interface_id(if_idx) == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_egr_xaui_spaui_fc_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

