/* $Id: petra_sw_db.c,v 1.23 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_serdes_low_level_access.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#ifdef PETRA_PP
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#endif
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#ifdef LINK_PB_LIBRARIES
  #ifdef LINK_PB_PP_LIBRARIES
    #include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
  #endif
#endif

#if DUNE_BCM
  #include <soc/dpp/drv.h>
  #include <soc/dpp/Petra/petra_api_ssr.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#ifdef LINK_PB_LIBRARIES
/* the length of key stream
 *    4 * 2: ip addresses (src/dest)
 *    1 * 2: Ip prefix ken (src/dest)
 *    2: VLAN/Port Value
 *    2: VLAN/Port Mask
 *    total: 14 bytes;
 */

/*
 * the length of key stream
 *    16 : ipv6 addresses (dest)
 *    1 :  Ipv6 prefix ken (dest)
 *    2: VLAN/Port Value
 *    2: VLAN/Port Mask
 *    total: 20 bytes;
 */

#endif


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#ifdef LINK_PB_LIBRARIES

#endif

#if DUNE_BCM
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_DIRTY_MARK \
  {uint8 is_init_done;\
  soc_petra_ssr_is_device_init_done(\
    unit,\
    &is_init_done\
  );\
   \
  if (!SOC_UNIT_NUM_VALID(unit)) { \
     return; \
  } \
  if (is_init_done == TRUE && !SOC_WARM_BOOT(unit)){\
  SOC_CONTROL_LOCK(unit);\
  SOC_CONTROL(unit)->scache_dirty = 1;\
  SOC_CONTROL_UNLOCK(unit);}}
#else 
#define BCM_DIRTY_MARK
#endif /*BCM_WARM_BOOT_SUPPORT*/

#else
#define BCM_DIRTY_MARK
#endif /*DUNE_BCM*/

#if DUNE_BCM
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_DIRTY_MARK_RETURNVAL \
  {uint8 is_init_done;\
  soc_petra_ssr_is_device_init_done(\
    unit,\
    &is_init_done\
  );\
   \
  if (!SOC_UNIT_NUM_VALID(unit)) { \
     SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit); \
  } \
  if (is_init_done == TRUE && !SOC_WARM_BOOT(unit)){\
  SOC_CONTROL_LOCK(unit);\
  SOC_CONTROL(unit)->scache_dirty = 1;\
  SOC_CONTROL_UNLOCK(unit);}}
#else 
#define BCM_DIRTY_MARK_RETURNVAL
#endif /*BCM_WARM_BOOT_SUPPORT*/

#else
#define BCM_DIRTY_MARK_RETURNVAL
#endif /*DUNE_BCM*/


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

/* static */
  SOC_PETRA_SW_DB
    Soc_petra_sw_db;

static uint8 Soc_petra_sw_db_initialized = FALSE;


/********************************************************************************************
 * Configuration
 * {
 ********************************************************************************************/


/*
 * } Configuration
 */


/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************************************
 * soc_petra_sw_db_init
 * {
 *********************************************************************************************/

/********************************************************************************************
 * Initialization
 * {
 ********************************************************************************************/
uint32
  soc_petra_sw_db_init(void)
{
  uint32 unit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  if (Soc_petra_sw_db_initialized)
  {
    goto exit;
  }

  soc_petra_srd_brdcst_all_lanes_clear();
  for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit] = NULL;
  }

  Soc_petra_sw_db_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_init()",0,0);
}
/********************************************************************************************
 * Configuration
 *********************************************************************************************/
void
  soc_petra_sw_db_close(void)
{
  Soc_petra_sw_db_initialized = FALSE;
}

/*********************************************************************************************
 * }
 * soc_petra_chip_definitions
 * {
 *********************************************************************************************/


/*********************************************************************************************
 * }
 * soc_petra_egr_ports
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_dev_egr_ports_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_DEV_EGR_PORTS_INITIALIZE);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports), SOC_PETRA_SW_DB_DEV_EGR_PORTS, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_egress_queues_init()",0,0);
}

/*********************************************************************************************
 * }
 * soc_petra_egr_ports
 * {
 *********************************************************************************************/

void
  soc_petra_sw_db_ofp_rates_calcal_length_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 calcal_length
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.calcal_length = calcal_length;
  BCM_DIRTY_MARK;
}

uint32
  soc_petra_sw_db_ofp_rates_calcal_length_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.calcal_length;
}

void
  soc_petra_sw_db_ofp_rates_update_device_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_device
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.update_device = update_device;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_ofp_rates_update_device_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.update_device;
}

void
  soc_petra_sw_db_ofp_rates_update_dev_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_dev_changed
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.update_dev_changed = update_dev_changed;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_ofp_rates_update_dev_changed_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.update_dev_changed;
}

void
  soc_petra_sw_db_ofp_rates_nof_instances_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx,
    SOC_SAND_IN uint32 nof_instances
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.nof_calcal_instances = nof_instances;
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.rcy.nof_calcal_instances = nof_instances;
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.mal[mal_ndx].nof_calcal_instances = nof_instances;
    break;
  default:
    return;
  }
  BCM_DIRTY_MARK;

}

uint32
  soc_petra_sw_db_ofp_rates_nof_instances_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.nof_calcal_instances;
  case SOC_PETRA_MAL_TYPE_RCY:
    return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.rcy.nof_calcal_instances;
  case SOC_PETRA_MAL_TYPE_NIF:
    return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.mal[mal_ndx].nof_calcal_instances;
  default:
    return SOC_SAND_ERR;
  }
}

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  mal_ndx,
    SOC_SAND_IN  uint32                  port_ndx,
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEV_EGR_RATE   *val
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_CPU:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.cpu.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_OLP:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.olp.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    /* *val and the field soc_petra_sw_db_egr_ports.rcy.rates[port_ndx] are both
     *  SOC_PETRA_SW_DB_DEV_EGR_RATE and hence no overrun static
     */
    /* coverity[overrun-buffer-arg:FALSE] */
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.rcy.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.mal[mal_ndx].rates[port_ndx],
      val
    );
    break;
  default:
    return SOC_SAND_ERR;
  }
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_egr_ports_set()", 0, 0);
}

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  mal_ndx,
    SOC_SAND_IN  uint32                  port_ndx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEV_EGR_RATE  *val
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_PETRA_SW_DB_INIT_DEFS;

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_CPU:
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.cpu.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_OLP:
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.olp.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    /* *val and the field soc_petra_sw_db_egr_ports.rcy.rates[port_ndx] are both
     *  SOC_PETRA_SW_DB_DEV_EGR_RATE and hence no overrun static
     */
    /* coverity[overrun-buffer-arg:FALSE] */
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.rcy.rates[port_ndx],
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.mal[mal_ndx].rates[port_ndx],
      val
    );
    break;
  default:
    return SOC_SAND_ERR;
  }
}

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_sch_rate_get(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_OUT  uint32   *val
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  *val = 0;

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    *val += Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.sch_mal_rate;
    *val += Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.sch_mal_rate;
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    *val += Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.rcy.sch_mal_rate;
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    *val += Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.mal[mal_ndx].sch_mal_rate;
    break;
  default:
   return SOC_SAND_ERR;
  }
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_sch_rate_set(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_IN  uint32   *val
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_CPU:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.cpu.sch_mal_rate,
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_OLP:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.olp.sch_mal_rate,
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.rcy.sch_mal_rate,
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      soc_petra_sw_db_egr_ports.mal[mal_ndx].sch_mal_rate,
      val
    );
    break;
  default:
   return SOC_SAND_ERR;
  }
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_egr_ports_mal_sch_rate_set()", 0, 0);
}

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                mal_ndx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEV_EGR_MAL  *val
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;
  uint32
    idx;

  SOC_PETRA_SW_DB_INIT_DEFS;

#ifdef LINK_PB_LIBRARIES
  mal_type = (SOC_PETRA_IS_DEV_PETRA_A) ? soc_petra_mal_type_from_id(mal_ndx) : soc_pb_mal_type_from_id(mal_ndx);
#else
  mal_type = (soc_petra_mal_type_from_id(mal_ndx));
#endif

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_CPU:
    val->nof_calcal_instances = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.nof_calcal_instances;
    val->sch_mal_rate = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.sch_mal_rate;

    for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
    {
      if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].valid)
      {
        val->rates[idx].egq_bursts = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].egq_bursts;
        val->rates[idx].egq_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].egq_rates;
        val->rates[idx].sch_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].sch_rates;
        val->rates[idx].valid = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].valid;
      }
    }
    for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
    {
      if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].valid)
      {
        val->rates[idx].egq_bursts = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].egq_bursts;
        val->rates[idx].egq_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].egq_rates;
        val->rates[idx].sch_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].sch_rates;
        val->rates[idx].valid = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].valid;
      }
    }
    break;
  case SOC_PETRA_MAL_TYPE_OLP:
    val->nof_calcal_instances = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.nof_calcal_instances;
    val->sch_mal_rate = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.sch_mal_rate;

    for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
    {
      if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].valid)
      {
        val->rates[idx].egq_bursts = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].egq_bursts;
        val->rates[idx].egq_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].egq_rates;
        val->rates[idx].sch_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].sch_rates;
      }
      val->rates[idx].valid = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[idx].valid;
    }
    for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
    {
      if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].valid)
      {
        val->rates[idx].egq_bursts = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].egq_bursts;
        val->rates[idx].egq_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].egq_rates;
        val->rates[idx].sch_rates = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].sch_rates;
        val->rates[idx].valid = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.olp.rates[idx].valid;
      }
    }
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.rcy,
      val
    );
    break;
  case SOC_PETRA_MAL_TYPE_NIF:
    SOC_PETRA_SW_DB_FIELD_GET(
      unit,
      soc_petra_sw_db_egr_ports.mal[mal_ndx],
      val
    );
    break;
  default:
   return SOC_SAND_ERR;
  }

  return SOC_SAND_OK;
}

uint32
  soc_petra_sw_db_egr_ports_active_mals_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 nof_mac_lanes,
    SOC_SAND_OUT uint8 *mal_is_active
  )
{
  uint32
    res;
  uint32
    mal_idx,
    ofp_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_EGR_PORTS_ACTIVE_MALS_GET);

  SOC_PETRA_CLEAR(mal_is_active, uint8, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);

  for (mal_idx = 0; mal_idx < nof_mac_lanes; mal_idx++)
  {
    for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ofp_idx++)
    {
      if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.mal[mal_idx].rates[ofp_idx].egq_rates != 0)
      {
        mal_is_active[mal_idx] = TRUE;
        continue;
      }
    }
  }

  for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ofp_idx++)
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.cpu.rates[ofp_idx].egq_rates != 0)
    {
      mal_is_active[SOC_PETRA_OFP_RATES_EGQ_MAL_CPU] = TRUE;
      continue;
    }
  }

  for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ofp_idx++)
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_egr_ports.rcy.rates[ofp_idx].egq_rates != 0)
    {
      mal_is_active[SOC_PETRA_OFP_RATES_EGQ_MAL_RCY] = TRUE;
      continue;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_egr_ports_active_mals_get()",0,0);

}


/*********************************************************************************************
 * }
 * soc_petra_scheduler
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_device_sch_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_DEVICE_SCH_INITIALIZE);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_scheduler), SOC_PETRA_SW_DB_DEVICE_SCH, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_sch_init()",0,0);
}

uint32
  soc_petra_sw_db_device_srd_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;
  uint32
    idx,
    idx1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_DEVICE_SRD_INIT);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes), SOC_PETRA_SW_DB_SERDES, 1);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_irq_disable = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_swap_unhandled = FALSE;

  for (idx = 0; idx < SOC_PETRA_SRD_NOF_QUARTETS; idx++)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.qrtt_max_rate[idx] = SOC_PETRA_SRD_DATA_RATE_NONE;
  }

  for (idx = SOC_PETRA_CONNECTION_DIRECTION_RX; idx <= SOC_PETRA_CONNECTION_DIRECTION_TX; idx++)
  {
    for (idx1 = 0; idx1 < SOC_PETRA_SRD_NOF_LANES; idx1++)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[idx][idx1].enabled = FALSE;
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[idx][idx1].orig_is_swap = FALSE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_srd_init()",0,0);
}


uint32
  soc_petra_sw_db_device_lbg_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    indx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->lbg), SOC_PETRA_SW_DB_LBG, 1);

  for (indx  = 0; indx < SOC_PETRA_NOF_FAP_PORTS; ++indx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->lbg.ports_state[indx].saved_valid = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_lbg_init()",0,0);
}




uint32
  soc_petra_sw_db_device_lag_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->lag), SOC_PETRA_SW_DB_LAGS_INFO, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_lbg_init()",0,0);
}

uint32
  soc_petra_sw_db_cell_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->cell), SOC_PETRA_SW_DB_CELL, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_cell_init()",0,0);
}

uint32
  soc_petra_sw_db_dram_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->dram), SOC_PETRA_SW_DB_DRAM, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_dram_init()",0,0);
}

uint32
  soc_petra_sw_db_qdr_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr), SOC_PETRA_SW_DB_QDR, 1);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.enable = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.size = SOC_PETRA_HW_NOF_QDR_SIZE_MBITS;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.protection_type = SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_qdr_init()",0,0);
}

uint32
  soc_petra_sw_db_mac_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->mac), SOC_PETRA_SW_DB_MAC, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_mac_init()",0,0);
}

#ifdef LINK_PB_LIBRARIES
uint32
  soc_pb_sw_db_synce_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->synce), SOC_PB_SW_DB_SYNCE, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_synce_init()",0,0);
}

uint32
  soc_pb_sw_db_port_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;
  uint32
    tm_ppp_ndx,
    pfg_ndx,
    pp_port_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; pp_port_ndx++)
  {
    SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx]), SOC_PB_SW_DB_PORT, 1);

    for (pfg_ndx = 0; pfg_ndx < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; pfg_ndx++)
    {
      for (tm_ppp_ndx = 0; tm_ppp_ndx < SOC_PB_PMF_NOF_TM_PPPS; tm_ppp_ndx++)
      {
        Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx].pmf_pgm[pfg_ndx][tm_ppp_ndx] = SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX+1;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_port_init()",0,0);
}

uint32
  soc_pb_sw_db_device_lbg_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    indx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg), SOC_PB_SW_DB_LBG, 1);

  for (indx  = 0; indx < SOC_PETRA_NOF_FAP_PORTS; ++indx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.ports_state[indx].saved_valid = FALSE;
  }

  for (indx  = 0; indx < SOC_PB_PORT_NOF_PP_PORTS; ++indx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.profile_state[indx].trap.saved_valid = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_lbg_init()",0,0);
}

uint32
  soc_pb_sw_db_multicast_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    indx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_multicast), SOC_PB_SW_DB_MULTICAST, 1);

  for (indx  = 0; indx < SOC_PETRA_MULT_NOF_MULTICAST_GROUPS; ++indx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_multicast.eg_mult_nof_vlan_bitmaps[indx] = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multicast_init",0,0);
}

void
  soc_petra_sw_db_ftmh_lb_ext_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ftmh_lb_ext_mode = ftmh_lb_ext_mode;
  BCM_DIRTY_MARK;
}

SOC_PB_MGMT_FTMH_LB_EXT_MODE
  soc_petra_sw_db_ftmh_lb_ext_mode_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ftmh_lb_ext_mode;
}

void
  soc_petra_sw_db_add_pph_eep_ext_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 add_pph_eep_ext
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.add_pph_eep_ext = add_pph_eep_ext;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_add_pph_eep_ext_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.add_pph_eep_ext;
}

void
  soc_petra_sw_db_is_petra_rev_a_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petra_rev_a_in_system
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_petra_rev_a_in_system = is_petra_rev_a_in_system;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_petra_rev_a_in_system_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_petra_rev_a_in_system;
}

uint32
  soc_pb_sw_db_tcam_db_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile;
  uint32
    db_id_in_lkp_profile_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db), SOC_PB_SW_DB_TCAM_DB_OLD, 1);

  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile);
  for (lkp_profile.id = 0; lkp_profile.id < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; ++lkp_profile.id)
  {
    for (lkp_profile.cycle_ndx = 0; lkp_profile.cycle_ndx < SOC_PB_TCAM_NOF_CYCLES; ++lkp_profile.cycle_ndx)
    {
      for (db_id_in_lkp_profile_ndx = 0; db_id_in_lkp_profile_ndx < SOC_PB_PMF_LOW_LEVEL_NOF_DBS_PER_LOOKUP_PROFILE_MAX; ++db_id_in_lkp_profile_ndx)
      {
        Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.db_id[lkp_profile.id][lkp_profile.cycle_ndx][db_id_in_lkp_profile_ndx] = 0xFFFFFFFF;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_db_init()",0,0);
}

uint32
  soc_pb_sw_db_tcam_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id,
    tcam_db_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].valid = FALSE;
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].nof_entries_free = 256;
  }

  for (tcam_db_id = 0; tcam_db_id < SOC_PB_TCAM_MAX_NOF_LISTS; ++tcam_db_id)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid = FALSE;
  }

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_init()",0,0);
}

uint32
  soc_pb_sw_db_frwrd_ip_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  SOC_SAND_HASH_TABLE_INIT_INFO
    init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Initialize the route_key -> entry_id hash table
   */
  /* soc_sand_SAND_HASH_TABLE_INFO_clear(unit,hash_tbl); */
  /* init_info                = &hash_tbl->init_info; */
  init_info.prime_handle  = unit;
  init_info.sec_handle    = 0;
  init_info.table_size    = 1024 * 4;
  init_info.table_width   = 1024 * 4;
  init_info.key_size      = 20;
  init_info.data_size     = sizeof(uint32);
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  /* will not work!! */
  res = soc_sand_hash_table_create(
          unit,
          &hash_tbl,
          init_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id = hash_tbl ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sw_db_frwrd_ip_init()", 0, 0);
}

uint32
  soc_pb_sw_db_tcam_mgmt_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    profile_id,
    bank_id,
    cycle,
    prefix,
    access_device;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (profile_id = 0; profile_id < SOC_PB_TCAM_MAX_NOF_LISTS; ++profile_id)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile_id].valid = FALSE;
  }

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    for (cycle = 0; cycle < SOC_PB_TCAM_NOF_ACCESS_CYCLES; ++cycle)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].access_profile[cycle] = SOC_PB_TCAM_MAX_NOF_LISTS;
    }

    for (prefix = 0; prefix < SOC_PB_TCAM_NOF_PREFIXES; ++prefix)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].prefix_db[prefix] = SOC_PB_TCAM_MAX_NOF_LISTS;
    }

    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].nof_dbs = 0;
  }

  for (cycle = 0; cycle < SOC_PB_TCAM_NOF_ACCESS_CYCLES; ++cycle)
  {
    for (access_device = 0; access_device < SOC_PB_TCAM_NOF_ACCESS_DEVICES; ++access_device)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
        tcam_mgmt.access_profile_in_access_device[cycle][access_device] = SOC_PB_TCAM_MAX_NOF_LISTS;
    }
  }

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_init()",0,0);
}

/*
 * SW DB multiset
 */
uint32
  soc_pb_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memcpy(
             data,
             buffer + (offset * len),
             len
             );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_buffer_get_entry()",0,0);
}


uint32
  soc_pb_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  sal_memcpy(
             buffer + (offset * len),
             data,
             len
             );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_buffer_set_entry()",0,0);
}


/*
 * SW DB Egress editor
 */

uint32
  soc_pb_sw_db_eg_editor_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    hw_index_ndx,
    res;
  SOC_SAND_MULTI_SET_PTR
    *multi_set_info ;
  SOC_SAND_MULTI_SET_INIT_INFO
    init_info ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor), SOC_PB_SW_DB_EGR_EDITOR, 1);

  multi_set_info = &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_profiles_multi_set);
  sal_memset(&init_info,0,sizeof(init_info)) ;
  /* soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info); */
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  init_info.member_size = sizeof(uint32);
  init_info.nof_members = SOC_PB_EGR_NOF_TM_PORT_PROFILES;
  init_info.sec_handle = 0;
  init_info.prime_handle = unit;
  /* will not work (petra code changed, wont work but will compile) */
  res = soc_sand_multi_set_create(
    unit,
    multi_set_info,
    init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  multi_set_info = &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_stk_prune_bmps_set);
  /* soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info); */
  sal_memset(&init_info,0,sizeof(init_info)) ;
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  init_info.member_size = SOC_PB_STACK_PRUN_BMP_LEN * sizeof(uint32);
  init_info.nof_members = 2;
  init_info.sec_handle = 0;
  init_info.prime_handle = unit;
  /* will not work (petra code changed, wont work but will compile) */
  res = soc_sand_multi_set_create(
    unit,
    multi_set_info,
    init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (hw_index_ndx = 0; hw_index_ndx < SOC_PB_EGR_NOF_TM_PORT_PROFILES; ++hw_index_ndx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.index_profile[hw_index_ndx] =
      SOC_PB_EGR_NOF_PROG_TM_PORT_PROFILES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_eg_editor_initialize()",0,0);
}

uint32
  soc_pb_sw_db_eg_editor_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
          unit,
          (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_profiles_multi_set)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_destroy(
          unit,
          (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_stk_prune_bmps_set)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_eg_editor_terminate()",0,0);
}



uint32
  soc_pb_sw_db_frwrd_ip_terminate(
                               SOC_SAND_IN  int unit
                               )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_hash_table_destroy(
    unit,
    (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_frwrd_ip_terminate()",0,0);
}

/*
 * Egress ACL
 */
uint32
  soc_pb_sw_db_eg_acl_initialize(
    SOC_SAND_IN  int unit
   )
{
  uint32
    db_id_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (db_id_ndx = 0; db_id_ndx < SOC_PB_EGR_ACL_NOF_DBS; ++db_id_ndx)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_acl.db_type[db_id_ndx] = SOC_PB_EGR_NOF_ACL_DB_TYPES;
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_eg_acl_initialize()",0,0);
}



/*
 * SW DB PMF
 */

uint32
  soc_pb_sw_db_pmf_initialize(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;
  SOC_SAND_MULTI_SET_INIT_INFO
    init_info ;
  SOC_SAND_MULTI_SET_PTR
    *multi_set_info;
  uint32
    pfg_ndx;
  SOC_TMC_PMF_PFG_INFO
    pfg_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf), SOC_PB_SW_DB_PMF, 1);

  multi_set_info = &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.header_profiles_multi_set);
  /* soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info); */
  sal_memset(&init_info,0,sizeof(init_info)) ;
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  init_info.member_size = sizeof(uint32);
  init_info.nof_members = SOC_PB_PMF_NOF_HEADER_PROFILES;
  init_info.sec_handle = 0;
  init_info.prime_handle = unit;
  /* will not work (petra code changed, wont work but will compile) */
  res = soc_sand_multi_set_create(
    unit,
    multi_set_info,
    init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  multi_set_info = &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pp_port_profiles_multi_set);
  /* soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info); */
  sal_memset(&init_info,0,sizeof(init_info)) ;
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  init_info.member_size = sizeof(uint32);
  init_info.nof_members = SOC_PB_PMF_NOF_PP_PORT_PROFILES;
  init_info.sec_handle = 0;
  init_info.prime_handle = unit;
  /* will not work (petra code changed, wont work but will compile) */
  res = soc_sand_multi_set_create(
    unit,
    multi_set_info,
    init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  multi_set_info = &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_profiles_multi_set);
  /* soc_sand_SAND_MULTI_SET_INFO_clear(multi_set_info); */
  sal_memset(&init_info,0,sizeof(init_info)) ;
  init_info.get_entry_fun = soc_pb_sw_db_buffer_get_entry;
  init_info.set_entry_fun = soc_pb_sw_db_buffer_set_entry;
  init_info.max_duplications = SOC_SAND_U32_MAX - 1;
  init_info.member_size = sizeof(uint32);
  init_info.nof_members = SOC_PB_PMF_NOF_PGM_PROFILES;
  init_info.sec_handle = 0;
  init_info.prime_handle = unit;
  /* will not work (petra code changed, wont work but will compile) */
  res = soc_sand_multi_set_create(
    unit,
    multi_set_info,
    init_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   * Always a virtual PMF-Program of type Ethernet to save in the SW DB
   * even if there are no Ethernet PMF-Programs currently
   */
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.is_pmf_pgm_eth[SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX+1] = TRUE;

  SOC_TMC_PMF_PFG_INFO_clear(&pfg_info);
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; pfg_ndx++)
  {
    SOC_PETRA_COPY(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pfg_info[pfg_ndx]), &(pfg_info), SOC_TMC_PMF_PFG_INFO, 1);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_pmf_initialize()",0,0);
}

/*
 * CORE_FREQ_HI_RESOLUTION
 */
uint32
  soc_pb_sw_db_core_freq_hi_res_init(
    SOC_SAND_IN  int unit
   )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.self_freq = SOC_SAND_INTERN_VAL_INVALID_32;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.min_fap_freq_in_system = SOC_SAND_INTERN_VAL_INVALID_32;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_core_freq_hi_res_init()",0,0);
}

uint32
  soc_pb_sw_db_pmf_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;

  res = soc_sand_multi_set_destroy(
          unit,
          (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.header_profiles_multi_set)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_multi_set_destroy(
          unit,
          (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pp_port_profiles_multi_set)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_multi_set_destroy(
          unit,
          (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_profiles_multi_set)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_pmf_terminate()",0,0);
}

/*
 * SW DB Egress mirror
 */
uint32
  soc_pb_sw_db_eg_mirror_initialize(
    SOC_SAND_IN  int unit
   )
{
  uint32
    internal_vid_ndx;
  uint32
    pp_port_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Init each member of eg mirror specifically */
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; ++pp_port_ndx)
  {
    for (internal_vid_ndx = 0; internal_vid_ndx < SOC_PB_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES; ++internal_vid_ndx)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_egr_mirror.eg_mirror_port_vlan[pp_port_ndx][internal_vid_ndx] = FALSE;
    }
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_eg_mirror_initialize()",0,0);
}

/*
 * SW DB multiset
 */
SOC_SAND_MULTI_SET_PTR
  soc_pb_sw_db_multiset_by_type_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* SOC_PB_SW_DB_MULTI_SET */
  )
{
  if (multiset_type >= SOC_PB_NOF_SW_DB_MULTI_SETS)
  {
#ifdef LINK_PB_PP_LIBRARIES
    return soc_pb_pp_sw_db_multiset_by_type_get(unit,multiset_type);
#else
    return 0;
#endif
  }
  else
  {
    switch(multiset_type)
    {
    case SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR:
      return (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_profiles_multi_set);
      break;
    case SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE:
      return (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.header_profiles_multi_set);
      break;
    case SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE:
      return (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pp_port_profiles_multi_set);
      break;
    case SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE:
      return (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_profiles_multi_set);
      break;
    case SOC_PB_SW_DB_MULTI_SET_STK_PRUNE_BMP:
      return (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.egr_editor_stk_prune_bmps_set);
      break;
    default:
      return 0;
    }
  }
}

uint32
  soc_pb_sw_db_multiset_add(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    tmp_val;
  uint8
    add_success;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SW_DB_MULTISET_ADD);

  multi_set = soc_pb_sw_db_multiset_by_type_get(unit,multiset_type);

  tmp_val = val;
  res = soc_sand_multi_set_member_add(
          unit,
          multi_set,
          (SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*)&tmp_val,
          data_indx,
          first_appear,
          &add_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (add_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multiset_add()",0,0);
}

uint32
  soc_pb_sw_db_multiset_remove(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32        val,
    SOC_SAND_OUT  uint32       *data_indx,
    SOC_SAND_OUT  uint8      *last_appear
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    tmp_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SW_DB_MULTISET_REMOVE);

  multi_set = soc_pb_sw_db_multiset_by_type_get(unit,multiset_type);

  tmp_val = (uint32)val;
  res = soc_sand_multi_set_member_remove(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)&tmp_val,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multiset_remove()",0,0);
}

uint32
  soc_pb_sw_db_multiset_lookup(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32               val,
    SOC_SAND_OUT  uint32              *data_indx,
    SOC_SAND_OUT  uint32              *ref_count
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    tmp_val;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SW_DB_MULTISET_LOOKUP);

  multi_set = soc_pb_sw_db_multiset_by_type_get(unit,multiset_type);

  tmp_val = (uint32)val;
  res = soc_sand_multi_set_member_lookup(
          unit,
          multi_set,
          (SOC_SAND_IN SOC_SAND_MULTI_SET_KEY*)&tmp_val,
          data_indx,
          ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multiset_lookup()",0,0);
}

uint32
  soc_pb_sw_db_multiset_remove_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SW_DB_MULTISET_REMOVE_BY_INDEX);

  multi_set = soc_pb_sw_db_multiset_by_type_get(unit,multiset_type);

  res = soc_sand_multi_set_member_remove_by_index(
          unit,
          multi_set,
          data_indx,
          last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multiset_remove_by_index()",0,0);
}

uint32
  soc_pb_sw_db_multiset_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* SOC_PB_SW_DB_MULTI_SET */
  )
{
  SOC_SAND_MULTI_SET_PTR
    multi_set;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SW_DB_MULTISET_CLEAR);

  multi_set = soc_pb_sw_db_multiset_by_type_get(unit,multiset_type);
  res = soc_sand_multi_set_clear(
          unit,
          multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_multiset_clear()",0,0);
}
#endif

uint32
  soc_petra_sw_db_device_init(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN SOC_PETRA_DEV_VER soc_petra_ver
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_DEVICE_INIT);

  SOC_PETRA_ALLOC_ANY_SIZE(Soc_petra_sw_db.soc_petra_device_sw_db[unit], SOC_PETRA_SW_DB_DEVICE, 1);

  res = soc_petra_sw_db_fat_pipe_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sw_db_op_mode_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_sw_db_device_sch_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_sw_db_dev_egr_ports_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_sw_db_multicast_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_sw_db_auto_scheme_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_sw_db_device_srd_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_sw_db_device_lbg_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_sw_db_device_lag_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_sw_db_cell_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_petra_sw_db_dram_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_petra_sw_db_qdr_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  res = soc_petra_sw_db_mac_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

#ifdef LINK_PB_LIBRARIES
  /*
   * The Soc_petra version is not set yet since the
   * SW DB is not yet initialized
   */
  if (soc_petra_ver == SOC_PETRA_DEV_VER_B)
  {
    res = soc_pb_sw_db_synce_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

    res = soc_pb_sw_db_port_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    res = soc_pb_sw_db_tcam_db_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = soc_pb_sw_db_tcam_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = soc_pb_sw_db_tcam_mgmt_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);

    res = soc_pb_sw_db_frwrd_ip_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 185, exit);

    res = soc_pb_sw_db_tcam_mgmt_initialize(unit,TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    res = soc_pb_sw_db_device_lbg_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    res = soc_pb_sw_db_multicast_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    res = soc_pb_sw_db_eg_editor_initialize(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

    res = soc_pb_sw_db_pmf_initialize(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    res = soc_pb_sw_db_eg_acl_initialize(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    res = soc_pb_sw_db_core_freq_hi_res_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_petra_device_init()",unit,0);
}

uint32
  soc_petra_sw_db_device_close(
    SOC_SAND_IN int unit
  )
{
#ifdef LINK_PB_LIBRARIES
  uint32
    res;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_DEVICE_CLOSE);
 
  /* get version information before closing sw db */
#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    soc_pb_sw_db_tcam_terminate(unit);

    res = soc_pb_sw_db_eg_editor_terminate(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_sw_db_pmf_terminate(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_sw_db_frwrd_ip_terminate(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
#endif

  SOC_PETRA_FREE_ANY_SIZE(Soc_petra_sw_db.soc_petra_device_sw_db[unit]);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit] = NULL;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_close()",0,0);
}

void
  soc_petra_sw_db_sch_max_expected_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_IN uint32  rate
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_scheduler.max_expected_port_rate[port_ndx] = rate;
  BCM_DIRTY_MARK;
}

uint32
  soc_petra_sw_db_sch_max_expected_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_scheduler.max_expected_port_rate[port_ndx];
}

/*
 *  Polarity swap handling enable/disable
 */
void
  soc_petra_sw_db_srd_is_swap_unhandled_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8   is_swap_unhandled
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_swap_unhandled = is_swap_unhandled;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_srd_is_swap_unhandled_get(
    SOC_SAND_IN int   unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_swap_unhandled;
}

/*
 *  IRQ enable/disable
 */
void
  soc_petra_sw_db_srd_is_irq_disable_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8   is_irq_disable
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_irq_disable = is_irq_disable;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_srd_is_irq_disable_get(
    SOC_SAND_IN int   unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.is_irq_disable;
}

/*
 *  Quartet Max Rate
 */
void
  soc_petra_sw_db_srd_qrtt_max_rate_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            qrtt_ndx,
    SOC_SAND_IN uint32             rate
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.qrtt_max_rate[qrtt_ndx] = rate;
  BCM_DIRTY_MARK;
}

uint32
  soc_petra_sw_db_srd_qrtt_max_rate_get(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            qrtt_ndx
  )
{
  uint32
    data_rate;

  if (qrtt_ndx >= SOC_PETRA_SRD_NOF_QUARTETS) {
    data_rate = SOC_PETRA_SRD_DATA_RATE_NONE;
  } else {
    data_rate = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.qrtt_max_rate[qrtt_ndx];
  }

  return data_rate;
}

/*
 *  PRBS_23 Swapped Polarity
 */
void
  soc_petra_sw_db_srd_prbs_23_swap_is_enabled_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx,
    SOC_SAND_IN uint8                   is_set
  )
{
  if (
      SOC_SAND_IS_VAL_IN_RANGE(direction, SOC_PETRA_CONNECTION_DIRECTION_RX, SOC_PETRA_CONNECTION_DIRECTION_TX) &&
      (lane_ndx <= (SOC_PETRA_SRD_NOF_LANES-1))
     )
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[direction][lane_ndx].enabled = is_set;
    BCM_DIRTY_MARK;
  }
}

uint8
  soc_petra_sw_db_srd_prbs_23_swap_is_enabled_get(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx
  )
{
  uint8
    is_set = FALSE;

  if (
      SOC_SAND_IS_VAL_IN_RANGE(direction, SOC_PETRA_CONNECTION_DIRECTION_RX, SOC_PETRA_CONNECTION_DIRECTION_TX) &&
      (lane_ndx <= (SOC_PETRA_SRD_NOF_LANES-1))
     )
  {
    is_set = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[direction][lane_ndx].enabled;
  }

  return is_set;
}

void
  soc_petra_sw_db_srd_orig_swap_polarity_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx,
    SOC_SAND_IN uint8                   is_swap_polarity
  )
{
  uint32
    lane_id,
    lane_id_last;
  uint8
    is_brdcst;
  if (
      SOC_SAND_IS_VAL_IN_RANGE(direction, SOC_PETRA_CONNECTION_DIRECTION_RX, SOC_PETRA_CONNECTION_DIRECTION_TX) &&
      (lane_ndx <= (SOC_PETRA_SRD_NOF_LANES-1))
     )
  {
    if (SOC_PETRA_SRD_LANE2INNER_ID(lane_ndx) == 0)
    {
      is_brdcst = soc_petra_srd_brdcst_all_lanes_get(unit);
      lane_id_last = (is_brdcst)?SOC_SAND_RNG_LAST(lane_ndx, SOC_PETRA_SRD_NOF_LANES_PER_QRTT):lane_ndx;
    }
    else
    {
      lane_id_last = lane_ndx;
    }
    for (lane_id = lane_ndx; lane_id <= lane_id_last; lane_id++)
    {
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[direction][lane_id].orig_is_swap = is_swap_polarity;
    }
  }
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_srd_orig_swap_polarity_get(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx
  )
{
  uint8
    is_swap_polarity = FALSE;

  if (
      SOC_SAND_IS_VAL_IN_RANGE(direction, SOC_PETRA_CONNECTION_DIRECTION_RX, SOC_PETRA_CONNECTION_DIRECTION_TX) &&
      (lane_ndx <= (SOC_PETRA_SRD_NOF_LANES-1))
     )
  {
    is_swap_polarity = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->serdes.prbs_23_swap[direction][lane_ndx].orig_is_swap;
  }

  return is_swap_polarity;
}

void
  soc_petra_sw_db_sch_accumulated_grp_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx,
    SOC_SAND_IN uint32  rate
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_scheduler.accumulated_grp_port_rate[grp_ndx] = rate;
  BCM_DIRTY_MARK;
}

uint32
  soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_scheduler.accumulated_grp_port_rate[grp_ndx];
}

/************************************************************************/
/*  LBG                                                                     */
/************************************************************************/
void
  soc_petra_sw_db_lbg_port_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx,
    SOC_SAND_IN uint8       valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->lbg.ports_state[port_indx].saved_valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_lbg_port_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->lbg.ports_state[port_indx].saved_valid;
}

uint32
  soc_petra_sw_db_lbg_port_stat_save(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_IN SOC_PETRA_SW_DB_LBG_PORT  *port_stat
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
 SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    lbg.ports_state[port_indx],
    port_stat
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_port_stat_save()", 0, 0);
}

uint32
  soc_petra_sw_db_lbg_port_stat_load(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_LBG_PORT  *port_stat
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    lbg.ports_state[port_indx],
    port_stat
  );
}

uint32
  soc_petra_sw_db_lbg_packet_size_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      size
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    lbg.packet_size,
    (&size)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_packet_size_set()", 0, 0);
}

uint32
  soc_petra_sw_db_lbg_packet_size_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_OUT uint32                      *size
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    lbg.packet_size,
    size
  );
}
/*
 * LAGs
 */

uint32
  soc_petra_sw_db_lag_in_use_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      lag_ndx,
    SOC_SAND_IN uint8                      use
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    lag.in_use[lag_ndx],
    (&use)
    );
    BCM_DIRTY_MARK_RETURNVAL;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lag_in_use_set()", 0, 0);
}

uint32
  soc_petra_sw_db_lag_in_use_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                       lag_ndx,
    SOC_SAND_OUT uint8                      *use
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    lag.in_use[lag_ndx],
    use
  );
}

/*
 * Cell
 */

uint32
  soc_petra_sw_db_cell_cell_ident_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                      cell_ident
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    cell.current_cell_ident,
    (&cell_ident)
    );
 BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_cell_cell_ident_set()", 0, 0);
}

uint32
  soc_petra_sw_db_cell_cell_ident_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint16                      *cell_ident
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    cell.current_cell_ident,
    cell_ident
  );
}

/*
 * DRAM size
 */

uint32
  soc_petra_sw_db_dram_dram_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dram_size_per_interface_mbyte
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    dram.dram_size_per_interface_mbyte,
    (&dram_size_per_interface_mbyte)
    )
    BCM_DIRTY_MARK_RETURNVAL;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_dram_dram_size_set()", 0, 0);

}

uint32
  soc_petra_sw_db_dram_dram_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dram_size_per_interface_mbyte
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    dram.dram_size_per_interface_mbyte,
    dram_size_per_interface_mbyte
  );
}

/*
 * DLL training sequence register
 */

uint32
  soc_petra_sw_db_dram_dll_periodic_trng_reg_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dll_periodic_trng_reg
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    dram.dll_periodic_trng_reg,
    (&dll_periodic_trng_reg)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_dram_dll_periodic_trng_reg_set()", 0, 0);
}

uint32
  soc_petra_sw_db_dram_dll_periodic_trng_reg_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dll_periodic_trng_reg
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    dram.dll_periodic_trng_reg,
    dll_periodic_trng_reg
  );
}

/*
 * DLL training sequence register - QDR
 */

uint32
  soc_petra_sw_db_qdr_dll_periodic_trng_reg_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dll_periodic_trng_reg
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    qdr.dll_periodic_trng_reg,
    (&dll_periodic_trng_reg)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_qdr_dll_periodic_trng_reg_set()", 0, 0);
}

uint32
  soc_petra_sw_db_qdr_dll_periodic_trng_reg_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dll_periodic_trng_reg
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    qdr.dll_periodic_trng_reg,
    dll_periodic_trng_reg
  );
}

void
  soc_petra_sw_db_qdr_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_SIZE_MBIT     size
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.size = size;
  BCM_DIRTY_MARK;
}

SOC_PETRA_HW_QDR_SIZE_MBIT
  soc_petra_sw_db_qdr_size_get(
    SOC_SAND_IN  int                      unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.size;
}

void
  soc_petra_sw_db_qdr_protection_type_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_PROTECT_TYPE     protection_type
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.protection_type = protection_type;
  BCM_DIRTY_MARK;
}

SOC_PETRA_HW_QDR_PROTECT_TYPE
  soc_petra_sw_db_qdr_protection_type_get(
    SOC_SAND_IN  int                      unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.protection_type;
}

void
  soc_petra_sw_db_qdr_enable_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint8    enable
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.enable = enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_qdr_enable_get(
    SOC_SAND_IN  int                      unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->qdr.enable;
}


/*
 * DRAM type
 */

uint32
  soc_petra_sw_db_dram_type_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE dram_type
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    dram.dram_type,
    (&dram_type)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_dram_type_set()", 0, 0);
}


uint32
  soc_petra_sw_db_dram_type_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE *dram_type
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    dram.dram_type,
    dram_type
  );
}

/*
 * DRAM configuration
 */

uint32
  soc_petra_sw_db_dram_conf_set(
    SOC_SAND_IN int                  unit,
    SOC_SAND_IN SOC_PETRA_HW_DRAM_CONF_PARAMS *dram_conf
  )
{
    SOC_PETRA_SW_DB_INIT_DEFS;
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    dram.dram_conf,
    dram_conf
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_dram_conf_set()", 0, 0);
}

uint32
  soc_petra_sw_db_dram_conf_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PETRA_HW_DRAM_CONF_PARAMS *dram_conf
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    dram.dram_conf,
    dram_conf
  );
}

/* } */

/*********************************************************************************************
 * }
 * soc_petra_multicast
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_multicast_init(
    SOC_SAND_IN int      unit
  )
{
  uint32
    mc_id_i,
    start_of_quarter,
    end_of_quarter,
    backwards_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_MULTICAST_INITIALIZE);

  backwards_ptr = SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.nof_unoccupied = SOC_PETRA_MULT_TABLE_SIZE;

  /* unoccupied list for 1st quarter (32K-1 - (3/4)*32K) */
  start_of_quarter = ((4 * SOC_PETRA_MULT_TABLE_SIZE) / 4) - 1;
  end_of_quarter = ((3 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  for (mc_id_i = start_of_quarter; mc_id_i >= end_of_quarter; mc_id_i--)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)(mc_id_i - 1);
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
    backwards_ptr = mc_id_i;
  }

  /* unoccupied list for 2nd quarter ((2/4)*32K - (1/4)*32K) */
  start_of_quarter = ((2 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  end_of_quarter = ((1 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)start_of_quarter;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
  backwards_ptr = mc_id_i;
  for (mc_id_i = start_of_quarter; mc_id_i > (end_of_quarter + 1); mc_id_i--)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)(mc_id_i - 1);
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
    backwards_ptr = mc_id_i;
  }

  /* unoccupied list for 3rd quarter ((3/4)*32K - (2/4)*32K) */
  start_of_quarter = ((3 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  end_of_quarter = ((2 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)start_of_quarter;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
  backwards_ptr = mc_id_i;
  for (mc_id_i = start_of_quarter -1; mc_id_i >=end_of_quarter ; mc_id_i--)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)(mc_id_i - 1);
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
    backwards_ptr = mc_id_i;
  }
  /* unoccupied list for 4th quarter ((1/4)*32K - 0 */
  start_of_quarter = ((1 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  end_of_quarter = ((0 * SOC_PETRA_MULT_TABLE_SIZE) / 4);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)start_of_quarter;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
  backwards_ptr = mc_id_i;
  for (mc_id_i = start_of_quarter; mc_id_i > end_of_quarter; mc_id_i--)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)(mc_id_i - 1);
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
    backwards_ptr = mc_id_i;
  }

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].next_unoccupied_ptr = (uint16)SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.soc_petra_mult_array[mc_id_i].backwards_ptr = (uint16)backwards_ptr;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.unoccupied_list_head = (uint16)SOC_PETRA_MULT_TABLE_MAX;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_multicast_init()",0,0);
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void
  soc_petra_sw_db_multicast_nof_unoccupied_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 nof_unoccupied
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.nof_unoccupied = nof_unoccupied;
  BCM_DIRTY_MARK;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
uint32
  soc_petra_sw_db_multicast_nof_unoccupied_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_sw_db_multicast.nof_unoccupied;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void
  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id,
    SOC_SAND_IN uint16  next_unoccupied
  )
{
  if (tbl_entry_id < SOC_PETRA_MULT_TABLE_SIZE)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
      soc_petra_sw_db_multicast.soc_petra_mult_array[tbl_entry_id].next_unoccupied_ptr =
      next_unoccupied;
    BCM_DIRTY_MARK;
  }
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
uint16
  soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id
  )
{
  if (tbl_entry_id < SOC_PETRA_MULT_TABLE_SIZE)
  {
    return
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
        soc_petra_sw_db_multicast.soc_petra_mult_array[tbl_entry_id].next_unoccupied_ptr;
  }
  else
  {
    return SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER;
  }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void
  soc_petra_sw_db_multicast_backwards_ptr_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id,
    SOC_SAND_IN uint16  backwards_ptr
  )
{
  if (tbl_entry_id < SOC_PETRA_MULT_TABLE_SIZE)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
      soc_petra_sw_db_multicast.soc_petra_mult_array[tbl_entry_id].backwards_ptr =
      backwards_ptr;
    BCM_DIRTY_MARK;
  }
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
uint16
  soc_petra_sw_db_multicast_backwards_ptr_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id
  )
{
  if (tbl_entry_id < SOC_PETRA_MULT_TABLE_SIZE)
  {
    return
      Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
        soc_petra_sw_db_multicast.soc_petra_mult_array[tbl_entry_id].backwards_ptr;
  }
  else
  {
    return SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER;
  }
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void
  soc_petra_sw_db_multicast_unoccupied_list_head_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint16  new_list_head
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    soc_petra_sw_db_multicast.unoccupied_list_head = new_list_head;
  BCM_DIRTY_MARK;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
uint16
  soc_petra_sw_db_multicast_unoccupied_list_head_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    soc_petra_sw_db_multicast.unoccupied_list_head;
}

/*********************************************************************************************
 * }
 * soc_petra auto scheme
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_auto_scheme_init(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_RET
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_AUTO_SCHEME_INITIALIZE);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_api_auto_queue_flow_mngmnt), SOC_PETRA_SW_DB_AUTO_SCHEME, 1);

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    soc_petra_api_auto_queue_flow_mngmnt.auto_valid = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_auto_scheme_init()",0,0);
}


uint8
  soc_petra_sw_db_auto_scheme_is_initialized(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    soc_petra_api_auto_queue_flow_mngmnt.auto_valid;
}

SOC_SAND_RET
  soc_petra_sw_db_auto_scheme_info_set(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN SOC_PETRA_AQFM_SYSTEM_INFO  *auto_system_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  sal_memcpy(
             &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_api_auto_queue_flow_mngmnt.system_info),
             auto_system_info,
             sizeof(SOC_PETRA_AQFM_SYSTEM_INFO)
             );

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    soc_petra_api_auto_queue_flow_mngmnt.auto_valid = TRUE;
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_auto_scheme_info_set()", 0, 0);
  return ret;
}

SOC_SAND_RET
  soc_petra_sw_db_auto_scheme_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *auto_system_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  sal_memcpy(
             auto_system_info,
             &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_api_auto_queue_flow_mngmnt.system_info),
             sizeof(SOC_PETRA_AQFM_SYSTEM_INFO)
             );

  return ret;
}

SOC_SAND_RET
  soc_petra_sw_db_auto_agg_info_item_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        port_id,
    SOC_SAND_IN  uint32                        agg_level,
    SOC_SAND_IN  uint32                        agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO  *agg_table_item_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  sal_memcpy(
             &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_api_auto_queue_flow_mngmnt.agg_table_info[port_id].item[agg_level][agg_index]),
             agg_table_item_info,
             sizeof(SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO)
             );

  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_auto_agg_info_item_set()", 0, 0);

  return ret;
}

SOC_SAND_RET
  soc_petra_sw_db_auto_agg_info_item_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        port_id,
    SOC_SAND_IN  uint32                        agg_level,
    SOC_SAND_IN  uint32                        agg_index,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO  *agg_table_item_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  sal_memcpy(
             agg_table_item_info,
             &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_petra_api_auto_queue_flow_mngmnt.agg_table_info[port_id].item[agg_level][agg_index]),
             sizeof(SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO)
             );

  return ret;
}


uint32
  soc_petra_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  )
{
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_REVISION_INIT);

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.device_sub_type = SOC_PETRA_REVISION_TYPE_88330;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.pp_enable = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap20_in_system = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap21_in_system = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_a1_or_below_in_system = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fe200_fabric = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.egr_mc_16k_groups_enable = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.tdm_mode = SOC_PETRA_MGMT_TDM_MODE_PACKET;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_variable_cell_size = FALSE;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_packet_segmentation = FALSE;

  for (idx = 0; idx < SOC_PETRA_COMBO_NOF_QRTTS; idx++)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_combo_nif[idx] = FALSE;
  }

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.nif_ref_clk = SOC_PETRA_MGMT_SRD_REF_CLK_125_00;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.fabric_ref_clk = SOC_PETRA_MGMT_SRD_REF_CLK_125_00;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.combo_ref_clk = SOC_PETRA_MGMT_SRD_REF_CLK_125_00;
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.stag_enable = FALSE;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_op_mode_init()",0,0);
}

void
  soc_petra_sw_db_revision_subtype_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_REVISION_SUB_TYPE_ID  subtype_id
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.device_sub_type = subtype_id;
  BCM_DIRTY_MARK;
}

SOC_PETRA_REVISION_SUB_TYPE_ID
  soc_petra_sw_db_revision_subtype_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.device_sub_type;
}

/* Soc_petra device type */
void
  soc_petra_sw_db_ver_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_DEV_VER  ver
  )
{	
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->dev_version.ver = ver;
  BCM_DIRTY_MARK;
}

SOC_PETRA_DEV_VER
  soc_petra_sw_db_ver_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->dev_version.ver;
}

void
  soc_petra_sw_db_pp_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 pp_enable
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.pp_enable = pp_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_pp_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.pp_enable;
}

void
  soc_petra_sw_db_is_fap20_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fap20_in_system
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap20_in_system = is_fap20_in_system;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_fap20_in_system_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap20_in_system;
}

void
soc_petra_sw_db_is_fe1600_in_system_set(
                                   SOC_SAND_IN int unit,
                                   SOC_SAND_IN uint8 is_fe1600_in_system
                                   )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fe1600_in_system = is_fe1600_in_system;
  BCM_DIRTY_MARK;
}

uint8
soc_petra_sw_db_is_fe1600_in_system_get(
                                   SOC_SAND_IN int unit
                                   )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fe1600_in_system;
}


void
  soc_petra_sw_db_is_fap21_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fap21_in_system
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap21_in_system = is_fap21_in_system;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_fap21_in_system_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap21_in_system;
}

uint8
  soc_petra_sw_db_is_fap2x_in_system_get(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_fap2x_in_system;

  is_fap2x_in_system = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap20_in_system;
  is_fap2x_in_system |= Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap21_in_system;

  return is_fap2x_in_system;
}

void
  soc_petra_sw_db_is_a1_or_below_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_a1_or_below_in_system
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_a1_or_below_in_system = is_a1_or_below_in_system;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_a1_or_below_in_system_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_a1_or_below_in_system;
}

/*
 *	Fap 21v is also handled (TRUE also is fap21 and A1 in system)
 */
uint8
  soc_petra_sw_db_is_fap20_and_a1_in_system_get(
    SOC_SAND_IN int unit
  )
{
  uint8
    is_fap20_in_system,
    is_petra_a1_or_below_in_system,
    is_both_in_system;

  is_fap20_in_system = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap20_in_system;
  is_fap20_in_system |= Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fap21_in_system;
  is_petra_a1_or_below_in_system = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_a1_or_below_in_system;
  is_both_in_system = SOC_SAND_NUM2BOOL(is_fap20_in_system && is_petra_a1_or_below_in_system);

  return is_both_in_system;
}

void
  soc_petra_sw_db_is_fe200_fabric_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fe200_fabric
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fe200_fabric = is_fe200_fabric;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_fe200_fabric_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fe200_fabric;
}

void
  soc_petra_sw_db_egr_mc_16k_groups_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 egr_mc_16k_groups_enable
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.egr_mc_16k_groups_enable = egr_mc_16k_groups_enable;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_egr_mc_16k_groups_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.egr_mc_16k_groups_enable;
}

void
  soc_petra_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MGMT_TDM_MODE tdm_mode
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.tdm_mode = tdm_mode;
  BCM_DIRTY_MARK;
}

SOC_PETRA_MGMT_TDM_MODE
  soc_petra_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.tdm_mode;
}

void
  soc_petra_sw_db_is_fabric_variable_cell_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fabric_variable_cell_size
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_variable_cell_size = is_fabric_variable_cell_size;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_fabric_variable_cell_size_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_variable_cell_size;
}

void
  soc_petra_sw_db_is_fabric_packet_segmentation_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fabric_packet_segmentation
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_packet_segmentation = is_fabric_packet_segmentation;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_is_fabric_packet_segmentation_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_fabric_packet_segmentation;
}

void
  soc_petra_sw_db_combo_nif_not_fabric_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT  combo_idx,
    SOC_SAND_IN uint8         is_combo_nif
  )
{
  if (combo_idx < SOC_PETRA_COMBO_NOF_QRTTS)
  {
    Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_combo_nif[combo_idx] = is_combo_nif;
    BCM_DIRTY_MARK;
  }
}

uint8
  soc_petra_sw_db_combo_nif_not_fabric_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT  combo_idx
  )
{
  uint8 is_nif = FALSE;

  if (combo_idx < SOC_PETRA_COMBO_NOF_QRTTS)
  {
    is_nif = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.is_combo_nif[combo_idx];
  }

  return is_nif;
}

void
  soc_petra_sw_db_stag_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 stag_enable
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.stag_enable = stag_enable;
  BCM_DIRTY_MARK;
}

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_nif_get(
    SOC_SAND_IN  int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.nif_ref_clk;
}

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_fabric_get(
    SOC_SAND_IN  int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.fabric_ref_clk;
}

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_combo_get(
    SOC_SAND_IN  int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.ref_clocks_conf.combo_ref_clk;
}

uint8
  soc_petra_sw_db_stag_enable_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->op_mode.stag_enable;
}

/*********************************************************************************************
 * }
 * Fat pipe
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_fat_pipe_init(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_RET
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_FAT_PIPE_INITIALIZE);

  SOC_PETRA_CLEAR(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->fat_pipe), SOC_PETRA_SW_DB_FAT_PIPE, 1);

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->fat_pipe.nof_fap_ports = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sw_db_device_fat_pipe_init()",0,0);
}

SOC_SAND_RET
  soc_petra_sw_db_ref_clocks_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLKS *ref_clocks_conf
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
      unit,
      op_mode.ref_clocks_conf,
      ref_clocks_conf
      );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_ref_clocks_set()", 0, 0);
}

void
  soc_petra_sw_db_fat_pipe_nof_ports_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fat_pipe_nof_ports
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->fat_pipe.nof_fap_ports = fat_pipe_nof_ports;
  BCM_DIRTY_MARK;
}

uint32
  soc_petra_sw_db_fat_pipe_nof_ports_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->fat_pipe.nof_fap_ports;
}

void
  soc_petra_sw_db_interrupt_mask_on_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        val
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->is_interrupt_masked_on = val;
  BCM_DIRTY_MARK;
}

uint8
  soc_petra_sw_db_interrupt_mask_on_get(
    SOC_SAND_IN  int                        unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->is_interrupt_masked_on;
}

/*
 * Fabric MACs
 */
void
  soc_petra_sw_db_is_double_rate_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32   link_ndx,
    SOC_SAND_IN uint8   is_double_rate
  )
{
  if (link_ndx < SOC_DPP_DEFS_GET(unit, nof_fabric_links))
  {
    soc_sand_bitstream_set(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->mac.is_link_double_rate, link_ndx, is_double_rate);
    BCM_DIRTY_MARK;
  }
}

uint8
  soc_petra_sw_db_is_double_rate_get(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32   link_ndx
  )
{
  uint8
    is_double_rate = FALSE;

  if (link_ndx < SOC_DPP_DEFS_GET(unit, nof_fabric_links))
  {
    is_double_rate = SOC_SAND_NUM2BOOL(soc_sand_bitstream_test_bit(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->mac.is_link_double_rate, link_ndx));
  }

  return is_double_rate;
}

#ifdef LINK_PB_LIBRARIES

void
  soc_pb_sw_db_synce_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE                val
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->synce.mode = val;
  BCM_DIRTY_MARK;
}

SOC_PB_NIF_SYNCE_MODE
  soc_pb_sw_db_synce_get(
    SOC_SAND_IN  int                        unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->synce.mode;
}

void
  soc_pb_sw_db_profile_ndx_pmf_pgm_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             tm_ppp_ndx,
    SOC_SAND_IN  uint32             pmf_pgm
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx].pmf_pgm[pfg_ndx][tm_ppp_ndx] = pmf_pgm;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_profile_ndx_pmf_pgm_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             tm_ppp_ndx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx].pmf_pgm[pfg_ndx][tm_ppp_ndx];
}

void
  soc_pb_sw_db_pp_port_header_type_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE header_type
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx].header_type = header_type;
  BCM_DIRTY_MARK;
}

SOC_PETRA_PORT_HEADER_TYPE
  soc_pb_sw_db_pp_port_header_type_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx
    )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->port[pp_port_ndx].header_type;
}

/************************************************************************/
/*  LBG                                                                     */
/************************************************************************/
void
  soc_pb_sw_db_lbg_port_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx,
    SOC_SAND_IN uint8       valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.ports_state[port_indx].saved_valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_lbg_port_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.ports_state[port_indx].saved_valid;
}

uint32
  soc_pb_sw_db_lbg_port_stat_save(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PORT    *port_stat
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    soc_pb_lbg.ports_state[port_indx],
    port_stat
  );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_port_stat_save()", 0, 0);
}

uint32
  soc_pb_sw_db_lbg_port_stat_load(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PORT   *port_stat
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    soc_pb_lbg.ports_state[port_indx],
    port_stat
  );
}

uint32
  soc_pb_sw_db_lbg_profile_info_stat_load(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                            profile_indx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO          *profile_info
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    soc_pb_lbg.profile_state[profile_indx].info,
    profile_info
  );
}

uint32
  soc_pb_sw_db_lbg_profile_info_stat_save(
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              profile_indx,
    SOC_SAND_IN SOC_PB_PORT_PP_PORT_INFO             *profile_info
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    soc_pb_lbg.profile_state[profile_indx].info,
    profile_info
    );
    BCM_DIRTY_MARK_RETURNVAL;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
 exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_pb_sw_db_lbg_profile_info_stat_save()", 0, 0);
}

uint32
  soc_pb_sw_db_lbg_profile_trap_stat_load(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                            profile_indx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PP_PORT_TRAP *profile_trap
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    soc_pb_lbg.profile_state[profile_indx].trap,
    profile_trap
  );
}

uint32
  soc_pb_sw_db_lbg_profile_strength_stat_save(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN uint32                               pp_port_ndx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH         *profile_strength
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    port[pp_port_ndx].strength,
    profile_strength
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_profile_strength_stat_save()", 0, 0);
}

uint32
  soc_pb_sw_db_lbg_profile_strength_stat_load(
    SOC_SAND_IN int                               unit,
    SOC_SAND_IN uint32                                pp_port_ndx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH         *profile_strength
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    port[pp_port_ndx].strength,
    profile_strength
  );
}

uint32
  soc_pb_sw_db_lbg_profile_trap_stat_save(
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              profile_indx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PP_PORT_TRAP    *profile_trap
  )
{

  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    soc_pb_lbg.profile_state[profile_indx].trap,
    profile_trap
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_profile_trap_stat_save()", 0, 0);
}
void
  soc_pb_sw_db_lbg_profile_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        profile_indx,
    SOC_SAND_IN uint8       valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.profile_state[profile_indx].trap.saved_valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_lbg_profile_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        profile_indx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_lbg.profile_state[profile_indx].trap.saved_valid;
}

uint32
  soc_pb_sw_db_lbg_packet_size_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      size
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SW_DB_INIT);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    soc_pb_lbg.packet_size,
    (&size)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_lbg_packet_size_set()", 0, 0);
}

uint32
  soc_pb_sw_db_lbg_packet_size_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_OUT uint32                      *size
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    soc_pb_lbg.packet_size,
    size
  );
}
/*
 *	TCAM
 */

void
  soc_pb_sw_db_tcam_explicit_address_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  explicit_address_set
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.explicit_address_set = explicit_address_set;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_explicit_address_get(
    SOC_SAND_IN  int                        unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.explicit_address_set;
}

void
  soc_pb_sw_db_tcam_explicit_bank_sel_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  explicit_bank_sel_set
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.explicit_bank_sel_set = explicit_bank_sel_set;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_explicit_bank_sel_get(
    SOC_SAND_IN  int                        unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.explicit_bank_sel_set;
}

void
  soc_pb_sw_db_tcam_lkp_profile_db_ids_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE         *lkp_profile,
    SOC_SAND_IN  uint32                  db_id_in_profile_ndx,
    SOC_SAND_IN  uint32                  db_id
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.db_id[lkp_profile->id][lkp_profile->cycle_ndx][db_id_in_profile_ndx]
  = db_id;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_lkp_profile_db_ids_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE         *lkp_profile,
    SOC_SAND_IN  uint32                  db_id_in_profile_ndx
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_db.db_id[lkp_profile->id][lkp_profile->cycle_ndx][db_id_in_profile_ndx];
}

uint32
  soc_pb_sw_db_tcam_info_get(
    SOC_SAND_IN int                              unit,
    SOC_SAND_OUT SOC_PB_SW_DB_TCAM_MGMT_INFO        **tcam_mgmt
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    tcam_info,
    tcam_mgmt
    );
}

uint32
  soc_pb_sw_db_tcam_info_set(
    SOC_SAND_IN int                              unit,
    SOC_SAND_IN SOC_PB_SW_DB_TCAM_MGMT_INFO        *tcam_mgmt
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    tcam_info,
    tcam_mgmt
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_tcam_info_set()", 0, 0);
}

void
  soc_pb_sw_db_tcam_bank_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8 valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_bank_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].valid;
}

void
  soc_pb_sw_db_tcam_bank_entry_size_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint32               bank_id,
    SOC_SAND_IN SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].entry_size = entry_size;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_BANK_ENTRY_SIZE
  soc_pb_sw_db_tcam_bank_entry_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].entry_size;
}

void
  soc_pb_sw_db_tcam_bank_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].entries_used = entries_used;
  BCM_DIRTY_MARK;
}

SOC_SAND_OCC_BM_PTR
  soc_pb_sw_db_tcam_bank_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].entries_used;
}

void
  soc_pb_sw_db_tcam_bank_nof_entries_free_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_entries_free
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].nof_entries_free = nof_entries_free;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_bank_nof_entries_free_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[bank_id].nof_entries_free;
}

void
  soc_pb_sw_db_tcam_db_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8 valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_db_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].valid;
}

void
  soc_pb_sw_db_tcam_db_entry_size_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint32               tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entry_size = entry_size;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_BANK_ENTRY_SIZE
  soc_pb_sw_db_tcam_db_entry_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entry_size;
}

void
  soc_pb_sw_db_tcam_db_prefix_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 prefix_size
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prefix_size = prefix_size;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_db_prefix_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prefix_size;
}

void
  soc_pb_sw_db_tcam_db_priority_mode_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_DB_PRIO_MODE prio_mode
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prio_mode = prio_mode;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_DB_PRIO_MODE
  soc_pb_sw_db_tcam_db_priority_mode_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prio_mode;
}

void
  soc_pb_sw_db_tcam_db_bank_used_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8 is_used
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].bank_used[bank_id] = is_used;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_db_bank_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].bank_used[bank_id];
}

void
  soc_pb_sw_db_tcam_db_prefix_set(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN uint32      tcam_db_id,
    SOC_SAND_IN uint32      bank_id,
    SOC_SAND_IN SOC_PB_TCAM_PREFIX *prefix
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prefix[bank_id] = *prefix;
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_tcam_db_prefix_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      tcam_db_id,
    SOC_SAND_IN  uint32      bank_id,
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX *prefix
  )
{
  *prefix = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].prefix[bank_id];
}

SOC_SAND_SORTED_LIST_PTR
  soc_pb_sw_db_tcam_db_priorities_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].priorities;
}

SOC_SAND_HASH_TABLE_PTR
  soc_pb_sw_db_tcam_db_entry_id_to_location_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entry_id_to_location;
}

void
  soc_pb_sw_db_tcam_db_location_tbl_set(
    SOC_SAND_IN int        unit,
    SOC_SAND_IN uint32        tcam_db_id,
            SOC_PB_TCAM_LOCATION *location_tbl
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].location_tbl =
    (SOC_PB_TCAM_LOCATION *) location_tbl;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_LOCATION *
  soc_pb_sw_db_tcam_db_location_tbl_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].location_tbl;
}

void
  soc_pb_sw_db_tcam_access_profile_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8 valid
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].valid = valid;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_access_profile_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].valid;
}

void
  soc_pb_sw_db_tcam_access_profile_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 tcam_db
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].tcam_db = tcam_db;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_access_profile_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].tcam_db;
}

void
  soc_pb_sw_db_tcam_access_profile_cycle_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            profile,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].cycle = cycle;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_ACCESS_CYCLE
  soc_pb_sw_db_tcam_access_profile_cycle_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].cycle;
}

void
  soc_pb_sw_db_tcam_access_profile_uniform_prefix_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8 uniform_prefix
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].uniform_prefix = uniform_prefix;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_tcam_access_profile_uniform_prefix_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].uniform_prefix;
}

void
  soc_pb_sw_db_tcam_access_profile_min_banks_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 min_banks
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].min_banks = min_banks;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_access_profile_min_banks_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].min_banks;
}

void
  soc_pb_sw_db_tcam_access_profile_callback_set(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint32           profile,
    SOC_SAND_IN SOC_PB_TCAM_MGMT_SIGNAL callback
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].callback = callback;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_MGMT_SIGNAL
  soc_pb_sw_db_tcam_access_profile_callback_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].callback;
}

void
  soc_pb_sw_db_tcam_access_profile_user_data_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32  user_data
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].user_data = user_data;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_access_profile_user_data_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile].user_data;
}

void
  soc_pb_sw_db_tcam_access_profile_access_device_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32             profile_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE device
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile_id].access_device = device;
  BCM_DIRTY_MARK;
}

SOC_PB_TCAM_ACCESS_DEVICE
  soc_pb_sw_db_tcam_access_profile_access_device_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.profiles[profile_id].access_device;
}

void
  soc_pb_sw_db_tcam_managed_bank_access_profile_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            bank_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle,
    SOC_SAND_IN uint32            profile
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].access_profile[cycle] = profile;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_managed_bank_access_profile_get(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            bank_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].access_profile[cycle];
}

void
  soc_pb_sw_db_tcam_managed_bank_prefix_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix,
    SOC_SAND_IN uint32 tcam_db_id
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].prefix_db[prefix] = tcam_db_id;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_managed_bank_prefix_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].prefix_db[prefix];
}

void
  soc_pb_sw_db_tcam_managed_bank_nof_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_dbs
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].nof_dbs = nof_dbs;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_managed_bank_nof_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_mgmt.banks[bank_id].nof_dbs;
}

void
  soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE  cycle,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device,
    SOC_SAND_IN uint32             profile_id
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
    tcam_mgmt.access_profile_in_access_device[cycle][access_device] = profile_id;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE  cycle,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->
           tcam_mgmt.access_profile_in_access_device[cycle][access_device];
}

SOC_SAND_HASH_TABLE_PTR
  soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
    SOC_SAND_IN int unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->frwrd_ip.route_key_to_entry_id;
}

uint32
  soc_pb_hash_table_set_entry(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN uint32         sec_hanlde,
    SOC_SAND_INOUT  uint8      *buffer,
    SOC_SAND_IN  uint32         offset,
    SOC_SAND_IN  uint32         len,
    SOC_SAND_IN uint8          *data
  )
{
  sal_memcpy(
    buffer + (offset * len),
    data,
    len
  );

  return SOC_SAND_OK;
}


uint32
  soc_pb_sw_db_tcam_terminate(
    SOC_SAND_IN  int unit
  )
{
  uint32
    indx,
    tcam_index,
    tcam_bank_indx,
    res;
  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_mgmt;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK;
 
  /* TCAM management {*/

/*soc_pb_sw_db_tcam_mgmt_initialize:*/
  tcam_mgmt = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info;

  for (indx = 0; indx <SOC_PB_TCAM_NOF_BANKS ; ++indx )
  {
     res = soc_sand_occ_bm_destroy(
             unit,
             tcam_mgmt->banks[indx].entries_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[tcam_bank_indx].valid)
    {
      res = soc_sand_occ_bm_destroy(            
            unit,
            Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.bank[tcam_bank_indx].entries_used
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);    
    }    
  }

  /*tcam.tcam_db[tcam_index].entry_id_to_location*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].valid)
    {    
      res = soc_sand_hash_table_destroy(
            unit,
            (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].entry_id_to_location)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    }
  }
  
  /*tcam.tcam_db[tcam_index].priorities*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_sorted_list_destroy(
            unit,
            (SOC_SAND_SORTED_LIST_PTR)(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].priorities)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    }
  }

  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].valid)
    {
      SOC_PETRA_FREE_ANY_SIZE(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_index].location_tbl); 
    }
  }

  SOC_PETRA_FREE_ANY_SIZE(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info);


  /* } TCAM management  */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_terminate()",0,0);
}

uint32
  soc_pb_sw_db_tcam_mgmt_initialize(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint8   create
  )
{
  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_mgmt;
  uint32
    indx;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (create)
  {
    SOC_PETRA_ALLOC_ANY_SIZE( Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info,
                          SOC_PB_SW_DB_TCAM_MGMT_INFO, 1 );
  }
  else
  {
    sal_memset(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info, 0x0, sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO));
  }

  tcam_mgmt = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = SOC_PB_TCAM_BANK_SIZE*2;

  for (indx = 0; indx <SOC_PB_TCAM_NOF_BANKS ; ++indx )
  {
    tcam_mgmt->banks[indx].users[0] = SOC_PB_NOF_TCAM_USERS;
    tcam_mgmt->banks[indx].users[1] = SOC_PB_NOF_TCAM_USERS;

    res = soc_sand_occ_bm_create(
            unit,
            &btmp_init_info,
            &(tcam_mgmt->banks[indx].entries_use)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  btmp_init_info.size = SOC_PB_TCAM_BANK_SIZE*2 * SOC_PB_TCAM_NOF_BANKS;
  btmp_init_info.init_val = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_tcam_mgmt_initialize()",0,0);
}


uint32
  soc_pb_sw_db_device_tcam_user_clear(
    SOC_SAND_IN  int         unit
  )
{
  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_mgmt;
  uint32
    indx,
    indx2,
    indx3;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  tcam_mgmt = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam_info;

  for (indx = 0; indx <SOC_PB_TCAM_NOF_BANKS ; ++indx )
  {
    tcam_mgmt->banks[indx].users[0] = SOC_PB_NOF_TCAM_USERS;
    tcam_mgmt->banks[indx].users[1] = SOC_PB_NOF_TCAM_USERS;

    for (indx2 = 0; indx2 < SOC_PB_TCAM_NOF_CYCLES; ++indx2)
    {
      tcam_mgmt->banks[indx].fp_db[indx2] = SOC_PB_TCAM_MAX_NOF_ACLS;
    }

    res = soc_sand_occ_bm_clear(
            unit,
            tcam_mgmt->banks[indx].entries_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  for (indx = 0; indx < SOC_PB_TCAM_MAX_NOF_LISTS ; ++indx )
  {
    for (indx2 = 0; indx2 < SOC_PB_TCAM_NOF_BANKS; ++indx2)
    {
      tcam_mgmt->uses_info[indx].resources[indx2] = SOC_PB_TCAM_NOF_CYCLES;
      tcam_mgmt->uses_info[indx].prefix[indx2] = 0;
      tcam_mgmt->uses_info[indx].nof_bits[indx2] = 0;
      tcam_mgmt->uses_info[indx].nof_entries[indx2] = 0;
    }
    tcam_mgmt->uses_info[indx].key_src = SOC_PB_NOF_PMF_FEM_INPUT_SRCS;
    for (indx2 = 0; indx2 < SOC_PB_PMF_LOW_LEVEL_NOF_FEMS; ++indx2)
    {
      for (indx3 = 0; indx3 < SOC_PB_TCAM_NOF_CYCLES; ++indx3)
      {
        tcam_mgmt->uses_info[indx].is_in_fem[indx2][indx3] = FALSE;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_sw_db_device_tcam_use_clear()",0,0);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
uint32
  soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 multicast_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_multicast.eg_mult_nof_vlan_bitmaps[multicast_id];
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void
  soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 multicast_id,
    SOC_SAND_IN uint32 nof_vlan_bitmaps
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_multicast.eg_mult_nof_vlan_bitmaps[multicast_id] = nof_vlan_bitmaps;
  BCM_DIRTY_MARK;
}

SOC_PB_EGR_PROG_TM_PORT_PROFILE
  soc_pb_sw_db_eg_editor_index_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index
    )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.index_profile[hw_index];
}

void
  soc_pb_sw_db_eg_editor_index_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN SOC_PB_EGR_PROG_TM_PORT_PROFILE index_profile
    )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_editor.index_profile[hw_index] = index_profile;
  BCM_DIRTY_MARK;
}

SOC_PB_EGR_ACL_DB_TYPE
  soc_pb_sw_db_eg_acl_type_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_acl.db_type[db_id];
}

void
  soc_pb_sw_db_eg_acl_type_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   db_id_ndx,
    SOC_SAND_IN SOC_PB_EGR_ACL_DB_TYPE          type
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_acl.db_type[db_id_ndx] = type;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_pmf_port_index_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pp_port_index_profile[hw_index];
}

void
  soc_pb_sw_db_pmf_port_index_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN uint32                    index_profile
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pp_port_index_profile[hw_index] = index_profile;
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_pgm_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index,
    SOC_SAND_OUT SOC_PB_SW_DB_PMF_PGM_PROFILE *pgm_profile
  )
{
  sal_memcpy(pgm_profile, &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_profile[hw_index]), sizeof(SOC_PB_SW_DB_PMF_PGM_PROFILE));
}

void
  soc_pb_sw_db_pgm_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN SOC_PB_SW_DB_PMF_PGM_PROFILE    *pgm_profile
  )
{
  sal_memcpy(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_profile[hw_index]), pgm_profile, sizeof(SOC_PB_SW_DB_PMF_PGM_PROFILE));
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_pgm_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_IN  SOC_PB_SW_DB_PMF_CE      *ce_instr
  )
{
  sal_memcpy(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_ce[pfg_ndx][key_ndx][instr_ndx]), ce_instr, sizeof(SOC_PB_SW_DB_PMF_CE));
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_pgm_ce_instr_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_OUT SOC_PB_SW_DB_PMF_CE      *ce_instr
  )
{
  sal_memcpy(ce_instr, &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pgm_ce[pfg_ndx][key_ndx][instr_ndx]), sizeof(SOC_PB_SW_DB_PMF_CE));
}

void
  soc_pb_sw_db_is_pgm_eth_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pmf_pgm_ndx,
    SOC_SAND_IN  uint8             is_eth
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.is_pmf_pgm_eth[pmf_pgm_ndx] = is_eth;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_is_pgm_eth_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pmf_pgm_ndx
    )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.is_pmf_pgm_eth[pmf_pgm_ndx];
}


uint32
  soc_pb_sw_db_local_to_sys_port_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tm_port,
    SOC_SAND_IN uint32                     sys_port
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_SW_DB_FIELD_SET(
    unit,
    lag.local_to_sys[tm_port],
    (&sys_port)
    );
  BCM_DIRTY_MARK_RETURNVAL;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
     SOC_SAND_EXIT_AND_SEND_ERROR( " soc_petra_sw_db_local_to_sys_port_set()", 0, 0);
}

void
  soc_pb_sw_db_pfg_info_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  SOC_TMC_PMF_PFG_INFO      *pfg_info
  )
{
  sal_memcpy(&(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pfg_info[pfg_ndx]), pfg_info, sizeof(SOC_TMC_PMF_PFG_INFO));
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_pfg_info_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_TMC_PMF_PFG_INFO      *pfg_info
  )
{
  sal_memcpy(pfg_info, &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.pfg_info[pfg_ndx]), sizeof(SOC_TMC_PMF_PFG_INFO));
}

void
  soc_pb_sw_db_stag_offset_nibble_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             stag_offset_nibble
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.stag_offset_nibble = stag_offset_nibble;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_stag_offset_nibble_get(
    SOC_SAND_IN  int             unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.stag_offset_nibble;
}

void
  soc_pb_sw_db_is_vsq_nif_enabled_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint8             is_vsq_nif_enabled
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.is_vsq_nif_enabled = is_vsq_nif_enabled;
  BCM_DIRTY_MARK;
}

uint8
  soc_pb_sw_db_is_vsq_nif_enabled_get(
    SOC_SAND_IN  int             unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->pmf.is_vsq_nif_enabled;
}

void
  soc_pb_sw_db_egr_acl_sw_db_id_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     egr_acl_ndx,
    SOC_SAND_IN  uint32                      sw_db_id
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_acl.sw_db_id[egr_acl_ndx] = sw_db_id;
  BCM_DIRTY_MARK;
}

void
  soc_pb_sw_db_core_freq_self_freq_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              self_freq
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.self_freq = self_freq;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_core_freq_self_freq_get(
    SOC_SAND_IN  int             unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.self_freq;
}

void
  soc_pb_sw_db_core_freq_min_fap_freq_in_system_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              min_fap_freq_in_system
  )
{
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.min_fap_freq_in_system = min_fap_freq_in_system;
  BCM_DIRTY_MARK;
}

uint32
  soc_pb_sw_db_core_freq_min_fap_freq_in_system_get(
    SOC_SAND_IN  int             unit
  )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->core_freq_hi_res.min_fap_freq_in_system;
}

uint32
  soc_pb_sw_db_egr_acl_sw_db_id_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      egr_acl_ndx
 )
{
  return Soc_petra_sw_db.soc_petra_device_sw_db[unit]->egr_acl.sw_db_id[egr_acl_ndx];
}




uint32
  soc_pb_sw_db_local_to_sys_port_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tm_port,
    SOC_SAND_OUT uint32                     *sys_port
  )
{
  SOC_PETRA_SW_DB_INIT_DEFS;

  SOC_PETRA_SW_DB_FIELD_GET(
    unit,
    lag.local_to_sys[tm_port],
    sys_port
  );
}

/************************************************************************/
/* Outbound mirroring                                                   */
/************************************************************************/
uint32
  soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_IN  uint8             is_exist
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_egr_mirror.eg_mirror_port_vlan[pp_port_ndx][internal_vid_ndx] = is_exist;
  BCM_DIRTY_MARK_RETURNVAL;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_mirror_port_vlan_is_exist_set()",0,0);
}

uint32
  soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_OUT uint8             *is_exist
    )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *is_exist = Soc_petra_sw_db.soc_petra_device_sw_db[unit]->soc_pb_egr_mirror.eg_mirror_port_vlan[pp_port_ndx][internal_vid_ndx];

  SOC_PETRA_DO_NOTHING_AND_EXIT;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_sw_db_mirror_port_vlan_is_exist_get()",0,0);
}


uint32
  soc_pb_ssr_PB_SW_DB_SSR_additional_buff_size_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint32    *buff_byte_size
  )
{
  uint32
    tcam_bank_indx,
    tcam_index;
  uint32
    size,
    res;
  SOC_PETRA_SW_DB_DEVICE
    *dev_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  dev_ptr = Soc_petra_sw_db.soc_petra_device_sw_db[unit];

  *buff_byte_size = sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO); /*for tcam_info field data*/
  
  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    res = soc_sand_occ_bm_get_size_for_save(
            unit,
            dev_ptr->tcam_info->banks[tcam_bank_indx].entries_use,
            &size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    *buff_byte_size += size;
  }
  
  res = soc_sand_hash_table_get_size_for_save(
          unit,
          dev_ptr->frwrd_ip.route_key_to_entry_id,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  *buff_byte_size += size;

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->egr_editor.egr_editor_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  *buff_byte_size += size;

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->egr_editor.egr_editor_stk_prune_bmps_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  *buff_byte_size += size;
  
  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.header_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  *buff_byte_size += size;

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.pp_port_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  *buff_byte_size += size;

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.pgm_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  *buff_byte_size += size;
  
  /*tcam.bank[tcam_bank_indx].entries_used*/
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    if (dev_ptr->tcam.bank[tcam_bank_indx].valid)
    {
      /* Petra code. Almost not in use. Ignore coverity defects */
      /* coverity[returned_value] */
      res = soc_sand_occ_bm_get_size_for_save(
          unit,
          dev_ptr->tcam.bank[tcam_bank_indx].entries_used,
          &size
        );
      *buff_byte_size += size;
    }    
  }

  /*tcam.tcam_db[tcam_index].entry_id_to_location*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_hash_table_get_size_for_save(
            unit,
            dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location,
            &size
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      *buff_byte_size += size; 
    }
  }

  /*tcam.tcam_db[tcam_index].priorities*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_sorted_list_get_size_for_save(
          unit,
          (SOC_SAND_SORTED_LIST_PTR)(dev_ptr->tcam.tcam_db[tcam_index].priorities),
          &size
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      *buff_byte_size += size; 
    }       
  }

  /*tcam.tcam_db[tcam_index].location_tbl*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      uint32 table_size ;

      soc_sand_hash_table_get_table_size(unit,dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location, &table_size) ;
      *buff_byte_size += (sizeof(SOC_PB_TCAM_LOCATION) * table_size);     
    }
  }

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_PB_SW_DB_SSR_additional_buff_size_get", 0, 0);

}


uint32
  soc_pb_ssr_PB_SW_DB_SSR_data_save(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT SOC_PB_SW_DB_DEVICE            *ssr_data,
      SOC_SAND_OUT uint8                      *additional_data,
      SOC_SAND_IN  uint32                      additional_data_size
    )
{
  uint32
    pp_port_ndx,
    tcam_index,
    tcam_bank_indx;
  uint32
    act_size,
    size,
    res;
  SOC_PETRA_SW_DB_DEVICE
    *dev_ptr;
  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info;
  uint8      *additional_data_orig;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  additional_data_orig = additional_data;

  tcam_info = (SOC_PB_SW_DB_TCAM_MGMT_INFO *)additional_data;

  if((additional_data - additional_data_orig + sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO)) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 10, exit);        
  }
  additional_data += sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO);

  dev_ptr = Soc_petra_sw_db.soc_petra_device_sw_db[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 20, exit);
  }

  /* copy whole db field by field */
  ssr_data->soc_petra_ssr.op_mode = dev_ptr->op_mode;
  ssr_data->soc_petra_ssr.dev_version = dev_ptr->dev_version;
  ssr_data->soc_petra_ssr.fat_pipe = dev_ptr->fat_pipe;
  ssr_data->soc_petra_ssr.lag = dev_ptr->lag;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_egr_ports = dev_ptr->soc_petra_sw_db_egr_ports;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_scheduler = dev_ptr->soc_petra_sw_db_scheduler;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_multicast = dev_ptr->soc_petra_sw_db_multicast;
  ssr_data->soc_petra_ssr.soc_petra_api_auto_queue_flow_mngmnt = dev_ptr->soc_petra_api_auto_queue_flow_mngmnt;
  ssr_data->soc_petra_ssr.serdes = dev_ptr->serdes;
  ssr_data->soc_petra_ssr.cell = dev_ptr->cell;
  ssr_data->soc_petra_ssr.lbg = dev_ptr->lbg;
  ssr_data->soc_petra_ssr.dram = dev_ptr->dram;
  ssr_data->soc_petra_ssr.qdr = dev_ptr->qdr;
  ssr_data->soc_petra_ssr.is_interrupt_masked_on = dev_ptr->is_interrupt_masked_on;
  ssr_data->soc_petra_ssr.mac = dev_ptr->mac;
  ssr_data->soc_petra_ssr.synce = dev_ptr->synce;
  ssr_data->soc_petra_ssr.tcam_info = 0x0;

  *tcam_info = *(dev_ptr->tcam_info);
  /* patch zeros in places that contain pointers. these pointers wont be needed in the restoration */
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    sal_memset(&tcam_info->banks[tcam_bank_indx].entries_use, 0x0, sizeof(SOC_SAND_OCC_BM_PTR));
  }

  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    res = soc_sand_occ_bm_get_size_for_save(
            unit,
            dev_ptr->tcam_info->banks[tcam_bank_indx].entries_use,
            &size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 30, exit);        
    }

    res = soc_sand_occ_bm_save(
            unit,
            dev_ptr->tcam_info->banks[tcam_bank_indx].entries_use,
            additional_data,
            size,
            &act_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    additional_data += act_size;
  }

  ssr_data->soc_petra_ssr.tcam_db = dev_ptr->tcam_db;
  
  
  ssr_data->soc_petra_ssr.tcam = dev_ptr->tcam;
  
  /* patch zeros in places that contain pointers. these pointers wont be needed in the restoration */
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    sal_memset(&ssr_data->soc_petra_ssr.tcam.bank[tcam_bank_indx].entries_used, 0x0, sizeof(SOC_SAND_OCC_BM_PTR));
  }

  /*tcam.bank[tcam_bank_indx].entries_used*/
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    if (dev_ptr->tcam.bank[tcam_bank_indx].valid)
    {
      res = soc_sand_occ_bm_get_size_for_save(
              unit,
              dev_ptr->tcam.bank[tcam_bank_indx].entries_used,
              &size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 40, exit);        
      }

      res = soc_sand_occ_bm_save(
              unit,
              dev_ptr->tcam.bank[tcam_bank_indx].entries_used,
              additional_data,
              size,
              &act_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      additional_data += act_size;
    }
  }

  /*tcam.tcam_db[tcam_index].entry_id_to_location*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_hash_table_get_size_for_save(
            unit,
            dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location,
            &size
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 50, exit);        
      }


      res = soc_sand_hash_table_save(
            unit,
            (dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location),
            additional_data,
            size,
            &act_size
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      
      additional_data += act_size;
      sal_memset(&ssr_data->soc_petra_ssr.tcam.tcam_db[tcam_index].entry_id_to_location, 0x0, sizeof(SOC_SAND_HASH_TABLE_INFO));
    }
  }

  /*tcam.tcam_db[tcam_index].priorities*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_sorted_list_get_size_for_save(
            unit,
            (SOC_SAND_SORTED_LIST_PTR)(dev_ptr->tcam.tcam_db[tcam_index].priorities),
            &size
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 60, exit);        
      }

      res = soc_sand_sorted_list_save(
            unit,
            (SOC_SAND_SORTED_LIST_PTR)(dev_ptr->tcam.tcam_db[tcam_index].priorities),
            additional_data,
            size,
            &act_size
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      
      additional_data += act_size;
      sal_memset(&ssr_data->soc_petra_ssr.tcam.tcam_db[tcam_index].priorities, 0x0, sizeof(SOC_SAND_SORTED_LIST_INFO));
    }
  }

  /*tcam.tcam_db[tcam_index].location_tbl*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    uint32 table_size ;
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      soc_sand_hash_table_get_table_size(unit,dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location,&table_size) ;
      size = (sizeof(SOC_PB_TCAM_LOCATION) * table_size);
      if((additional_data - additional_data_orig + size) > additional_data_size) {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 70, exit);        
      }
      
      sal_memcpy(additional_data, ssr_data->soc_petra_ssr.tcam.tcam_db[tcam_index].location_tbl, size);    
      additional_data += size;
    
      /*patch zeroes instead of the pointers*/
      sal_memset(&ssr_data->soc_petra_ssr.tcam.tcam_db[tcam_index].location_tbl, 0x0, sizeof(SOC_PB_TCAM_LOCATION *));   
    }
  }

  ssr_data->soc_petra_ssr.tcam_mgmt = dev_ptr->tcam_mgmt;
    
  /*frwrd_ip*/
  ssr_data->soc_petra_ssr.frwrd_ip = dev_ptr->frwrd_ip;
  res = soc_sand_hash_table_get_size_for_save(
          unit,
          dev_ptr->frwrd_ip.route_key_to_entry_id,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 80, exit);        
  }
  res = soc_sand_hash_table_save(
          unit,
          (dev_ptr->frwrd_ip.route_key_to_entry_id),
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.frwrd_ip.route_key_to_entry_id, 0x0, sizeof(SOC_SAND_HASH_TABLE_INFO));

  ssr_data->soc_petra_ssr.soc_pb_lbg = dev_ptr->soc_pb_lbg;
  ssr_data->soc_petra_ssr.soc_pb_multicast = dev_ptr->soc_pb_multicast;

  /*egr_editor - 2 multiset*/
  ssr_data->soc_petra_ssr.egr_editor = dev_ptr->egr_editor;
  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->egr_editor.egr_editor_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 90, exit);        
  }
  res = soc_sand_multi_set_save(
          unit,
          dev_ptr->egr_editor.egr_editor_profiles_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.egr_editor.egr_editor_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->egr_editor.egr_editor_stk_prune_bmps_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 100, exit);        
  }
  res = soc_sand_multi_set_save(
          unit,
          dev_ptr->egr_editor.egr_editor_stk_prune_bmps_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.egr_editor.egr_editor_stk_prune_bmps_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  ssr_data->soc_petra_ssr.egr_acl = dev_ptr->egr_acl;
  
  ssr_data->soc_petra_ssr.core_freq_hi_res.self_freq = dev_ptr->core_freq_hi_res.self_freq;
  ssr_data->soc_petra_ssr.core_freq_hi_res.min_fap_freq_in_system = dev_ptr->core_freq_hi_res.min_fap_freq_in_system;

  /*pmf  -  3 multi sets*/
  ssr_data->soc_petra_ssr.pmf = dev_ptr->pmf;
  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.header_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 110, exit);        
  }
  res = soc_sand_multi_set_save(
          unit,
          dev_ptr->pmf.header_profiles_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.header_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.pp_port_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 120, exit);        
  }
  res = soc_sand_multi_set_save(
          unit,
          dev_ptr->pmf.pp_port_profiles_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.pp_port_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  res = soc_sand_multi_set_get_size_for_save(
          unit,
          dev_ptr->pmf.pgm_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  if((additional_data - additional_data_orig + size) > additional_data_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 130, exit);        
  }
  res = soc_sand_multi_set_save(
          unit,
          dev_ptr->pmf.pgm_profiles_multi_set,
          additional_data,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  additional_data += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.pgm_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  ssr_data->soc_petra_ssr.soc_pb_egr_mirror = dev_ptr->soc_pb_egr_mirror;
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; pp_port_ndx++)
  {
    ssr_data->soc_petra_ssr.port[pp_port_ndx] = dev_ptr->port[pp_port_ndx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_PB_SW_DB_SSR_data_save", 0, 0);
}

uint32
  soc_pb_ssr_PB_SW_DB_SSR_data_load(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                      *ssr_stream,
    SOC_SAND_IN  uint32                     size_of_pb_ssr,
    SOC_SAND_IN  uint8                      *additional_data
  )
{
  uint32
    pp_port_ndx,
    tcam_index,
    tcam_bank_indx;
  uint32
    res;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_SW_DB_DEVICE
    *dev_ptr;
  const SOC_PB_SW_DB_DEVICE
    *ssr_data;
  const SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_stream);

  tcam_info = (const SOC_PB_SW_DB_TCAM_MGMT_INFO *) additional_data;

  additional_data += sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO);

  ssr_data = (const SOC_PB_SW_DB_DEVICE*)ssr_stream;

  if(Soc_petra_sw_db.soc_petra_device_sw_db[unit] == NULL)
  {
    SOC_PETRA_ALLOC_ANY_SIZE(Soc_petra_sw_db.soc_petra_device_sw_db[unit], SOC_PETRA_SW_DB_DEVICE, 1);
  }

  dev_ptr = Soc_petra_sw_db.soc_petra_device_sw_db[unit];

  /* copy whole db field by field */
  dev_ptr->op_mode = ssr_data->soc_petra_ssr.op_mode;
  dev_ptr->dev_version = ssr_data->soc_petra_ssr.dev_version;
  dev_ptr->fat_pipe = ssr_data->soc_petra_ssr.fat_pipe;
  dev_ptr->lag = ssr_data->soc_petra_ssr.lag;
  dev_ptr->soc_petra_sw_db_egr_ports = ssr_data->soc_petra_ssr.soc_petra_sw_db_egr_ports;
  dev_ptr->soc_petra_sw_db_scheduler = ssr_data->soc_petra_ssr.soc_petra_sw_db_scheduler;
  dev_ptr->soc_petra_sw_db_multicast = ssr_data->soc_petra_ssr.soc_petra_sw_db_multicast;
  dev_ptr->soc_petra_api_auto_queue_flow_mngmnt = ssr_data->soc_petra_ssr.soc_petra_api_auto_queue_flow_mngmnt;
  dev_ptr->serdes = ssr_data->soc_petra_ssr.serdes;
  dev_ptr->cell = ssr_data->soc_petra_ssr.cell;
  dev_ptr->lbg = ssr_data->soc_petra_ssr.lbg;
  dev_ptr->dram = ssr_data->soc_petra_ssr.dram;
  dev_ptr->qdr = ssr_data->soc_petra_ssr.qdr;
  dev_ptr->is_interrupt_masked_on = ssr_data->soc_petra_ssr.is_interrupt_masked_on;
  dev_ptr->mac = ssr_data->soc_petra_ssr.mac;
  dev_ptr->synce = ssr_data->soc_petra_ssr.synce;

  dev_ptr->tcam_info = NULL;
  SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->tcam_info, SOC_PB_SW_DB_TCAM_MGMT_INFO, 1);

  *(dev_ptr->tcam_info) = *tcam_info;
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    res = soc_sand_occ_bm_load(
            unit,
            &additional_data,
            &(dev_ptr->tcam_info->banks[tcam_bank_indx].entries_use)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  dev_ptr->tcam_db = ssr_data->soc_petra_ssr.tcam_db;


  dev_ptr->tcam = ssr_data->soc_petra_ssr.tcam;
  
  
  /*tcam.bank[tcam_bank_indx].entries_used*/
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    if (dev_ptr->tcam.bank[tcam_bank_indx].valid)
    {
      res = soc_sand_occ_bm_load(
            unit,
            &additional_data,
            &(dev_ptr->tcam.bank[tcam_bank_indx].entries_used)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);    
    }    
  }

  /*tcam.tcam_db[tcam_index].entry_id_to_location*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {    
      res = soc_sand_hash_table_load(
            unit,
            &additional_data,
            soc_pb_tcam_db_data_structure_entry_set,
            soc_pb_tcam_db_data_structure_entry_get,
            NULL,
            NULL,
            &dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    }
  }
  
  /*tcam.tcam_db[tcam_index].priorities*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      res = soc_sand_sorted_list_load(
            unit,
            &additional_data,
            soc_pb_tcam_db_data_structure_entry_get,
            soc_pb_tcam_db_data_structure_entry_set,
            (dev_ptr->tcam.tcam_db[tcam_index].prio_mode == SOC_PB_TCAM_DB_PRIO_MODE_BANK) ?  
                    soc_pb_tcam_db_priority_list_cmp_bank : 
                    soc_pb_tcam_db_priority_list_cmp_interlaced,
            (SOC_SAND_SORTED_LIST_PTR *)&(dev_ptr->tcam.tcam_db[tcam_index].priorities)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    }
  }


  /*tcam.tcam_db[tcam_index].location_tbl*/
  for (tcam_index = 0; tcam_index <SOC_PB_TCAM_MAX_NOF_LISTS ; ++tcam_index )
  {
    uint32 table_size ;

    if (dev_ptr->tcam.tcam_db[tcam_index].valid)
    {
      soc_sand_hash_table_get_table_size(unit,dev_ptr->tcam.tcam_db[tcam_index].entry_id_to_location,&table_size) ;
      SOC_PETRA_ALLOC_ANY_SIZE(dev_ptr->tcam.tcam_db[tcam_index].location_tbl, SOC_PB_TCAM_LOCATION, table_size);
      sal_memcpy(dev_ptr->tcam.tcam_db[tcam_index].location_tbl, additional_data, (sizeof(SOC_PB_TCAM_LOCATION) * table_size));
      additional_data += (sizeof(SOC_PB_TCAM_LOCATION) * table_size);  
    }
  }

  dev_ptr->tcam_mgmt = ssr_data->soc_petra_ssr.tcam_mgmt;
  
  dev_ptr->frwrd_ip = ssr_data->soc_petra_ssr.frwrd_ip;
  res = soc_sand_hash_table_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          NULL,
          NULL,
          &dev_ptr->frwrd_ip.route_key_to_entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  dev_ptr->soc_pb_lbg = ssr_data->soc_petra_ssr.soc_pb_lbg;
  dev_ptr->soc_pb_multicast = ssr_data->soc_petra_ssr.soc_pb_multicast;
  
  dev_ptr->egr_editor = ssr_data->soc_petra_ssr.egr_editor;
  res = soc_sand_multi_set_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->egr_editor.egr_editor_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  res = soc_sand_multi_set_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->egr_editor.egr_editor_stk_prune_bmps_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  dev_ptr->egr_acl = ssr_data->soc_petra_ssr.egr_acl;

  dev_ptr->core_freq_hi_res.self_freq = ssr_data->soc_petra_ssr.core_freq_hi_res.self_freq;
  dev_ptr->core_freq_hi_res.min_fap_freq_in_system = ssr_data->soc_petra_ssr.core_freq_hi_res.min_fap_freq_in_system;

  dev_ptr->pmf = ssr_data->soc_petra_ssr.pmf;
  res = soc_sand_multi_set_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->pmf.header_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  res = soc_sand_multi_set_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->pmf.pp_port_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  res = soc_sand_multi_set_load(
          unit,
          &additional_data,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &dev_ptr->pmf.pgm_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  dev_ptr->soc_pb_egr_mirror = ssr_data->soc_petra_ssr.soc_pb_egr_mirror;
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; pp_port_ndx++)
  {
    dev_ptr->port[pp_port_ndx] = ssr_data->soc_petra_ssr.port[pp_port_ndx];
  }

  SOC_SAND_CHECK_FUNC_RESULT(ret, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_PB_SW_DB_SSR_data_load", 0, 0);
}

#endif

uint32
  soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_save(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEVICE_SSR_V01  *ssr_data
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_SW_DB_DEVICE
    *dev_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  dev_ptr = Soc_petra_sw_db.soc_petra_device_sw_db[unit];

  if (dev_ptr == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  sal_memcpy(
             ssr_data,
             dev_ptr,
             sizeof(SOC_PETRA_SW_DB_DEVICE_SSR_V01)
             );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_save", 0, 0);
}

uint32
  soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_load(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                      *ssr_stream
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_SW_DB_DEVICE
    *dev_ptr;
  const SOC_PETRA_SW_DB_DEVICE_SSR_V01
    *ssr_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(ssr_stream);

  ssr_data = (const SOC_PETRA_SW_DB_DEVICE_SSR_V01*)ssr_stream;
  dev_ptr = Soc_petra_sw_db.soc_petra_device_sw_db[unit];

  sal_memcpy(
             dev_ptr,
             ssr_data,
             sizeof(SOC_PETRA_SW_DB_DEVICE_SSR_V01)
             );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_load", 0, 0);
}

#if SOC_PETRA_DEBUG

void
  soc_petra_PETRA_SW_DB_DEV_EGR_RATE_print(
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEV_EGR_RATE   *info
  )
{
  uint8
    zero_rate = SOC_SAND_NUM2BOOL((info->egq_rates == 0) && (info->sch_rates == 0));

  if ((!zero_rate) && (info->valid))
  {
    soc_sand_os_printf("        SCH: %-8u[Kbps], ",info->sch_rates);
    soc_sand_os_printf("EGQ: %-8u[Kbps], ",info->egq_rates);

    if (info->egq_bursts == SOC_PETRA_OFP_RATES_BURST_LIMIT_MAX)
    {
      soc_sand_os_printf("Max Burst: No Limit.\n\r");
    }
    else
    {
      soc_sand_os_printf("Max Burst: %-6u[Byte].\n\r", info->egq_bursts);
    }
  }
}

void
  soc_petra_PETRA_SW_DB_DEV_EGR_MAL_print(
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEV_EGR_MAL   *info
  )
{
  uint32
    ind;

  soc_sand_os_printf("      nof_calcal_instances: %u\n\r", info->nof_calcal_instances);
  soc_sand_os_printf("      sch_mal_rate: %u\n\r", info->sch_mal_rate);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_SW_DB_DEV_EGR_RATE_print(info->rates + ind);
  }
}

void
  soc_petra_PETRA_SW_DB_DEV_EGR_PORTS_print(
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEV_EGR_PORTS   *info
  )
{
  uint32
    ind;

  soc_sand_os_printf("    calcal_length: %u\n\r", info->calcal_length);
  soc_sand_os_printf("    update_device: %u\n\r", info->update_device);
  soc_sand_os_printf("    update_dev_changed: %u\n\r", info->update_dev_changed);
  soc_sand_os_printf("    cpu:\n\r");
  soc_petra_PETRA_SW_DB_DEV_EGR_MAL_print(&(info->cpu));
  soc_sand_os_printf("    rcy:\n\r");
  soc_petra_PETRA_SW_DB_DEV_EGR_MAL_print(&(info->rcy));
#ifdef LINK_PB_LIBRARIES
  for (ind = 0; ind < SOC_SAND_MAX(SOC_PETRA_NOF_MAC_LANES, SOC_PB_NOF_MAC_LANES); ++ind)
#else
  for (ind = 0; ind < SOC_PETRA_NOF_MAC_LANES; ++ind)
#endif
  {
    soc_sand_os_printf("    mal: %u\n\r", ind);
    soc_petra_PETRA_SW_DB_DEV_EGR_MAL_print(info->mal + ind);
  }
}

void
  soc_petra_PETRA_SW_DB_DEVICE_print(
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEVICE   *info
  )
{
  soc_petra_PETRA_SW_DB_DEV_EGR_PORTS_print(&(info->soc_petra_sw_db_egr_ports));
}

void
  soc_petra_sw_db_device_print(
    SOC_SAND_IN  int    unit
  )
{
  soc_sand_os_printf("unit: %u\n\r", unit);
  soc_petra_PETRA_SW_DB_DEVICE_print(Soc_petra_sw_db.soc_petra_device_sw_db[unit]);
}

#endif
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
