/* $Id: petra_ingress_scheduler.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_ingress_scheduler.h
*
* MODULE PREFIX:  soc_petra_ingress_scheduler
*
* FILE DESCRIPTION:   This file holds the functions which implement the
*                     Soc_petra ingress scheduler. The file contains the
*                     standard unsafe get/set and verify functions,
*                     in addition to their service functions.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PETRA_INGRESS_SCHEDULER_INCLUDED__
/* { */
#define __SOC_PETRA_INGRESS_SCHEDULER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_ingress_scheduler.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_ING_SCH_MAX_DELAY_VAL   0xFFFF
#define SOC_PETRA_ING_SCH_MIN_DELAY_VAL   4

#define SOC_PETRA_ING_SCH_MAX_CAL_VAL     0xFFFF
#define SOC_PETRA_ING_SCH_MIN_CAL_VAL     0
#define SOC_PETRA_ING_SCH_FIRST_CAL_VAL   1024

#define SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS SOC_PETRA_ING_SCH_MESH_LAST

#define SOC_PETRA_ING_SCH_MAX_RATE_MIN 15
#define SOC_PETRA_ING_SCH_MAX_RATE_MAX 120000000

/* } */

/*************
 * MACROS    *
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

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_init
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
  soc_petra_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_mesh_verify
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info -
*     mesh_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_mesh_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info -
*     mesh_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_mesh_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info -
*     mesh_info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_verify
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info -
*     clos_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info -
*     clos_info pointer to configuration structure.
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Sep 30 2007
* FUNCTION:
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info -
*     clos_info pointer to configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_weights_set
* TYPE:
*   PROC
* DATE:
*   Oct 02 2007
* FUNCTION:
*     This procedure writes all the weights in the clos_info structure
*     to the suitable registers fields.
* INPUT:
*     SOC_SAND_IN  int unit - for writing fields.
*     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   clos_info - pointer to the
*           ingress scheduler clos information, where the weights are
*           kept.
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_ingress_scheduler_clos_weights_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
     );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_global_shapers_set
* TYPE:
*   PROC
* DATE:
*   Oct 02 2007
* FUNCTION:
*     This procedure writes the values of the global shapers (fabric and local)
*     in the clos_info structure to the suitable registers fields.
* INPUT:
*     SOC_SAND_IN  int unit - for writing fields.
*     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   clos_info - pointer to the
*           ingress scheduler clos information, where the shapers are
*           kept.
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_global_shapers_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
     SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
    );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_shaper_values_set
* TYPE:
*   PROC
* DATE:
*   Oct 02 2007
* FUNCTION:
*     This procedure writes the values a given shapers (max_burst, max_rate)
*     in the clos_info structure to the suitable registers fields
*     (ShaperMaxCredit, ShaperDelay, ShaperCal). The ShaperDelay & ShaperCal
*     fields are retrieved using an additional function that converts max_rate
*     to the suitable values.
* INPUT:
*     SOC_SAND_IN  int unit - for writing fields.
*     SOC_SAND_IN  SOC_PETRA_ING_SCH_SHAPER shaper - the shaper
*               structure which its parameters are to be written to the registers.
*     SOC_SAND_IN  SOC_PETRA_REG_FIELD max_credit_fld - the field in which to write the
*               max_burst value.
*     SOC_SAND_IN  SOC_PETRA_REG_FIELD delay_fld - the field in which to write the delay
*               value (in 2 clock cycles resolution) derived from the max_rate
*               value.
*     SOC_SAND_IN  SOC_PETRA_REG_FIELD cal_fld - the field in which to write the cal
*               value (in bytes resolution) derived from the max_rate value.
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_shaper_values_set(
       SOC_SAND_IN  int                 unit,
       SOC_SAND_IN  SOC_PETRA_ING_SCH_SHAPER      *shaper,
       SOC_SAND_IN  SOC_PETRA_REG_FIELD           *max_credit_fld,
       SOC_SAND_IN  SOC_PETRA_REG_FIELD           *delay_fld,
       SOC_SAND_IN  SOC_PETRA_REG_FIELD           *cal_fld,
       SOC_SAND_OUT uint32                  *exact_max_rate
      );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_rate_to_delay_cal_form
* TYPE:
*   PROC
* DATE:
*   Oct 02 2007
* FUNCTION:
*     This procedure converts the ingress scheduler shaper max_rate
*     given in kbps to the values of the register fields-
*     1. ShaperDelay: Time interval to add the credit.
*        in two clocks cycles resolution.
*     2. ShaperCal: Credit to add, in bytes resolution.
* INPUT:
*   SOC_SAND_IN  int            unit - for reading device ticks
*   SOC_SAND_IN  uint32             max_rate - the value to be converted.
*   SOC_SAND_OUT uint32*            shaper_delay - pointer for return.
*   SOC_SAND_OUT uint32*            shaper_cal - pointer for return.
* RETURNS:
*   SOC_SAND_INDIRECT:
*     shaper_delay - the ShaperDelay register field.
*     shaper_cal - the ShaperCal register field.
*   OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_ingress_scheduler_rate_to_delay_cal_form(
     SOC_SAND_IN  int            unit,
     SOC_SAND_IN  uint32             max_rate,
     SOC_SAND_OUT uint32*            shaper_delay_2_clocks,
     SOC_SAND_OUT uint32*            shaper_cal,
     SOC_SAND_OUT uint32*            exact_max_rate
     );
/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_shaper_values_get
* TYPE:
*   PROC
* DATE:
*   Oct 09 2007
* FUNCTION:
*     This procedure reads the values of the the shaper (max_burst, max_rate)
*     from the suitable registers fields (ShaperMaxCredit, ShaperDelay, ShaperCal).
*     The max_rate vallue of the Shaper structure is retrieved from the
*     ShaperDelay & ShaperCal fields, using an additional function that
*     converts the suitable values to max_rate.
* INPUT:
*      SOC_SAND_IN   int            unit - for reading fields.
*      SOC_SAND_IN   SOC_PETRA_REG_FIELD      *max_credit_fld - the field in which to
*                                             read the max_burst value.
*      SOC_SAND_IN   SOC_PETRA_REG_FIELD      *delay_fld - the field in which to
*                                             read the delay value.
*      SOC_SAND_IN   SOC_PETRA_REG_FIELD      *cal_fld - the field in which to
*                                             read the cal value.
*      SOC_SAND_OUT  SOC_PETRA_ING_SCH_SHAPER *shaper - the shaper structure which its
*                           parameters are to be received from registers
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_shaper_values_get(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *max_credit_fld,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *delay_fld,
    SOC_SAND_IN   SOC_PETRA_REG_FIELD           *cal_fld,
    SOC_SAND_OUT  SOC_PETRA_ING_SCH_SHAPER      *shaper
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_delay_cal_to_max_rate_form
* TYPE:
*   PROC
* DATE:
*   Oct 09 2007
* FUNCTION:
*     This procedure converts the ingress scheduler shaper register field
*     values:
*     1. ShaperDelay: Time interval to add the credit.
*        in two clocks cycles resolution.
*     2. ShaperCal: Credit to add, in bytes resolution.
*
*     to the max_rate value of the shaper structure in the API (in kbps)
*
* INPUT:
*   SOC_SAND_IN  int         unit - for reading device ticks
*   SOC_SAND_IN  uint32          shaper_delay - the reg field value
*                                           (Time interval to add the credit).
*   SOC_SAND_IN  uint32          shaper_cal - the reg field value
*                                            (Credit to add, in bytes resolution).
*   SOC_SAND_OUT uint32          *max_rate - the value returned (in kbps)
* RETURNS:
*   SOC_SAND_INDIRECT:
*     shaper_delay - the ShaperDelay register field.
*     shaper_cal - the ShaperCal register field.
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_delay_cal_to_max_rate_form(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          shaper_delay,
    SOC_SAND_IN  uint32          shaper_cal,
    SOC_SAND_OUT uint32          *max_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_mesh_reg_flds_db_get
* TYPE:
*   PROC
* DATE:
*   Oct 09 2007
* FUNCTION:
*     This procedure is used for the Mesh topology. This procedure initializes
*     arrays of shaper_max_crdts, shaper_delays and shaper_cals with the
*     appropriate fields. This is done fo0r easier implementation of the set and
*     get functions.
* INPUT:
*     SOC_SAND_IN  int       unit - for reading fields.
*     SOC_PETRA_REG_FIELD          *wfq_weights - an array of the wfq fields (to return).
*     SOC_PETRA_REG_FIELD          *shaper_max_crdts - an array of the maxCredits fields (to return).
*     SOC_PETRA_REG_FIELD          *shaper_delays - an array of the delay fields (to return).
*     SOC_PETRA_REG_FIELD          *shaper_cals - an array of the cal fields (to return).
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_ingress_scheduler_mesh_reg_flds_db_get(
    SOC_SAND_IN  int       unit,
    SOC_PETRA_REG_FIELD          wfq_weights[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_max_crdts[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_delays[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS],
    SOC_PETRA_REG_FIELD          shaper_cals[SOC_PETRA_ING_SCH_NUM_OF_CONTEXTS]
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_weights_get
* TYPE:
*   PROC
* DATE:
*   Oct 09 2007
* FUNCTION:
*     This procedure read all the weights in the suitable registers fields
*     to the clos_info structure.
* INPUT:
*     SOC_SAND_IN  int unit - for reading fields.
*     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   clos_info - pointer to the
*           ingress scheduler clos information, where the weights are
*           kept.
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_weights_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
    );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_clos_global_shapers_get
* TYPE:
*   PROC
* DATE:
*   Oct 09 2007
* FUNCTION:
*     This procedure reads the values of the global shapers (fabric and local)
*     to the clos_info structure from the suitable registers fields.
* INPUT:
*     SOC_SAND_IN  int unit - for reading fields.
*     SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   clos_info - pointer to the
*           ingress scheduler clos information, where the shapers are
*           kept.
*
* OUTPUT:
*   void.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
   soc_petra_ingress_scheduler_clos_global_shapers_get(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_OUT  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
    );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_exact_cal_value
* TYPE:
*   PROC
* DATE:
*   Oct 11 2007
* FUNCTION:
*     This procedure returns a more exact cal value.
*     in order to get an exact cal value
*     calculate:
*        cal_value * max_rate / exact_rate_in_kbits_per_sec;
* INPUT:
*   void.
* RETURNS:
*   SOC_SAND_DIRECT:
*
*
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_exact_cal_value(
    SOC_SAND_IN uint32  cal_value,
    SOC_SAND_IN uint32  max_rate,
    SOC_SAND_IN uint32  exact_rate_in_kbits_per_sec,
    SOC_SAND_OUT uint32 *exact_cal_value_long
  );

/*********************************************************************
* NAME:
*     soc_petra_ingress_scheduler_conversion_test
* TYPE:
*   PROC
* DATE:
*   Oct 12 2007
* FUNCTION:
*     This procedure perform a test that compares the rate values to
*     the exact rate values that are received after the
*     conversion function.
*     The procedure tests 2 main criteria:
*     1. That the exact error percentage from the rate does not exceed limit.
*     2. That the exact is always larger than rate.
* INPUT:
*   SOC_SAND_IN uint8 is_regression - if true less values are checked.
*   SOC_SAND_IN uint8 silent - whether to perform prints (True/False)
* RETURNS:
*   SOC_SAND_DIRECT: uint8 pass - whether the test has passed or not.
*
*
*********************************************************************/

uint8
  soc_petra_ingress_scheduler_conversion_test(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_INGRESS_SCHEDULER_INCLUDED__*/
#endif
