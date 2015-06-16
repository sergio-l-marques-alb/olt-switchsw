/* $Id: jer_ingress_traffic_mgmt.h,v 1.16 Broadcom SDK $
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


#ifndef __JER_INGRESS_TRAFFIC_MGMT_INCLUDED__
/* { */
#define __JER_INGRESS_TRAFFIC_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h> 
#include <soc/error.h> 
#include <soc/dpp/ARAD/arad_api_ingress_traffic_mgmt.h> 



/*********************************************************************
* NAME:
*     jer_itm_init
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
uint32
  jer_itm_init(
    SOC_SAND_IN  int  unit
  );
/*********************************************************************
*     Set ECN as enabled or disabled for the device
*********************************************************************/
uint32
  jer_itm_enable_ecn_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled /* ECN will be enabled/disabled for non zero/zero values */
  );

/*********************************************************************
*     Return if ECN is enabled for the device
*********************************************************************/
uint32
  jer_itm_enable_ecn_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled /* will return non zero if /ECN is enabled */
  );

/********************************************************************* 
* NAME: 
*     arad_itm_ingress_shape_set_unsafe 
* TYPE: 
*   PROC 
* DATE: 
*   May 12 2008 
* FUNCTION: 
*     The device may have one continues range of queues that 
*     belongs to ingress shaping. And set the Ingress shaping 
*     credit generator. 
* INPUT: 
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access. 
*  SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info - 
*     Ingress shaping configuration. 
* REMARKS: 
*     Base Queue number and add/subtract mode must be set 
*     prior to calling this API. To set base-q configuration, 
*     use ipq_explicit_mapping_mode_info_set API. 
* RETURNS: 
*     OK or ERROR indication. 
*********************************************************************/ 
uint32 
  jer_itm_ingress_shape_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
 
/********************************************************************* 
* NAME: 
*     jer_itm_ingress_shape_get 
* TYPE: 
*   PROC 
* FUNCTION: 
*     Sets ingress shaping configuration. This includes 
*     ingress shaping queues range, and credit generation 
*     configuration. 
* INPUT: 
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access. 
*  SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info - 
*     Ingress shaping configuration. 
* REMARKS: 
*     Base Queue number and add/subtract mode must be set 
*     prior to calling this API. To set base-q configuration, 
*     use ipq_explicit_mapping_mode_info_set API. 
* RETURNS: 
*     OK or ERROR indication. 
*********************************************************************/ 
uint32 
  jer_itm_ingress_shape_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ); 
 
/********************************************************************* 
* NAME: 
*     arad_itm_category_rngs_set 
* TYPE: 
*   PROC 
* DATE: 
*   May 12 2008 
* FUNCTION: 
*     Defines packet queues categories - in contiguous blocks. 
*     IQM queues are divided to 4 categories in contiguous 
*     blocks. Category-4 from 'category-end-3' till the last 
*     queue (32K). 
* INPUT: 
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access. 
*  SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info - 
*     pointer to configuration structure. 
* REMARKS: 
*     None. 
* RETURNS: 
*     OK or ERROR indication. 
*********************************************************************/ 
uint32 
  jer_itm_category_rngs_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info
  );
/********************************************************************* 
* NAME: 
*     arad_itm_category_rngs_get 
* TYPE: 
*   PROC 
* DATE: 
*   May 12 2008 
* FUNCTION: 
*     Defines packet queues categories - in contiguous blocks. 
*     IQM queues are divided to 4 categories in contiguous 
*     blocks. Category-4 from 'category-end-3' till the last 
*     queue (32K). 
* INPUT: 
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access. 
*  SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info - 
*     pointer to configuration structure. 
* REMARKS: 
*     None. 
* RETURNS: 
*   OK or ERROR indication. 
*********************************************************************/ 
uint32 
  jer_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  );
/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );
/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );

uint32
    jer_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info);

uint32
    jer_itm_vsq_wred_set(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO *info, 
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info);
uint32
  jer_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    /*SOC_SAND_IN  uint8               is_ocb_only,*/
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  );
uint32
  jer_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );
uint32
  jer_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  );
uint32
  jer_itm_vsq_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *exact_info
  );
uint32
  jer_itm_vsq_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *info
  );


/**
 * Initialize the Mrps-In-Dp-Mapping and 
 * Drop-Precedence-Mapping. 
 * MrpsInDpMapping: 
 * <dp-meter-cmd,incoming-dp,Ethernet-meter-dp,Ethernet-meter-invalid> 
 * -> <mrps-in-dp,mrps-in-drop> 
 * DropPrecedenceMapping:
 * <dp-meter-cmd,incoming-dp,Ethernet-meter-dp,Meter-processor-dp> 
 * -> <ingress-dp,egress-dp>  
 * The (auto-detected) mode (PP or TM) determines the mapping 
 * (for TM we ignore the meter and the ingress-dp is passed 
 * through). 
 * 
 * @param unit 
 * 
 * @return uint32 
 */
uint32 
  jer_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  );
#include <soc/dpp/SAND/Utils/sand_footer.h> 
/* } __JER_INGRESS_TRAFFIC_MGMT_INCLUDED__*/ 
#endif
