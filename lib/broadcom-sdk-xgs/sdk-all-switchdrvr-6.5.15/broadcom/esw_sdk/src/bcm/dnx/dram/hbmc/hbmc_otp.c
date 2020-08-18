/** \file src/bcm/dnx/dram/hbmc/hbmc_otp.c
 *
 *
 *  This file contains implimentation of functions for HBMC OTP
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */

#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_cb.h>
#include <soc/shmoo_hbm16.h>
#include "hbmc_dbal_access.h"
#include "hbmc_monitor.h"
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/drv.h>
#include <soc/dnxc/dnxc_ha.h>

/*
 * }
 */

/*
 * defines
 * {
 */

/*
 * }
 */

 /**
  * \brief - read from otp, hbm16 tune configuration. 
  *
  * \param [in] unit - unit number
  * \param [in] hbm_ndx - hbm index
  * \param [out] shmoo_config_param - pointer to array which function fill all the restored data from the file.
  *
  *
  * \return
  *   shr_error_e
  *
  * \remark
  *   * None
  * \see
  *   * None
  */
 soc_error_t
 dnx_hbmc_otp_shmoo_hbm16_restore_from_otp(
     int unit,
     int hbm_ndx,
     hbmc_shmoo_config_param_t * shmoo_config_param)
 {
     const hbmc_shmoo_channel_config_param_t *channel_metadata = soc_hbm16_shmoo_channel_config_param_metadata_get();
     const hbmc_shmoo_midstack_config_param_t *midstack_metadata = soc_hbm16_shmoo_midstack_config_param_metadata_get();
     uint32 otp_restore_version;

     SHR_FUNC_INIT_VARS(unit);
 
     otp_restore_version = dnx_data_dram.general_info.otp_restore_version_get(unit);
 
     /** T.B.D - read bit string from OTP (all relevant lines) and fill prog_data */
     /** each otp row capture in 3 fields of prog_data: prog_data[0] = bits:0..31, prog_data[1] = bits:32..63, prog_data[2] = bits:64..71 */
 
     
     
     switch(otp_restore_version)
     {
         case 1:
             otp_restore_version = HBMC_SHMOO_OTP_VER_1;
             break;
         default:
             SHR_ERR_EXIT(_SHR_E_INTERNAL, 
                 "invalid otp restore version read from dnx_data. version=%d\n", otp_restore_version);
     }
 
                 
     for (int channel_ndx = 0; channel_ndx < dnx_data_dram.hbm.nof_channels_get(unit); channel_ndx++)
     {
         for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_ndx++)
         {
             /** if the parameter level (=version) belong to the given version, restore it  */
             if (channel_metadata[metadata_ndx].level == otp_restore_version)
             {
                /* T.B.D - restore field data */
                 shmoo_config_param->channel_data[channel_ndx][metadata_ndx].valid = TRUE;
             }
         }
     }
 
     for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_ndx++)
     {
         /** if the parameter level (=version) belong to the given version, restore it  */
         if (midstack_metadata[metadata_ndx].level == otp_restore_version)
         {
                /* T.B.D - restore field data */         
             shmoo_config_param->midstack_data[metadata_ndx].valid = TRUE;
         }
     }
 
 exit:
     SHR_FUNC_EXIT;
 }

