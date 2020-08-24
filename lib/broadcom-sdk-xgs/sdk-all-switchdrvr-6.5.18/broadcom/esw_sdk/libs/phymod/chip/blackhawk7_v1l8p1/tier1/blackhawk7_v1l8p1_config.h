/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/***********************************************************************************
 ***********************************************************************************
 *                                                                                 *
 *  Revision    :      *
 *                                                                                 *
 *  Description :  Config functions targeted to IP user                            *
 *                                                                                 *
 ***********************************************************************************
 ***********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/** @file blackhawk7_v1l8p1_config.h
 * Configuration functions provided to IP User
 */

#ifndef BLACKHAWK7_V1L8P1_API_CONFIG_H
#define BLACKHAWK7_V1L8P1_API_CONFIG_H

#include "blackhawk7_v1l8p1_ipconfig.h"
#include "common/srds_api_enum.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"
#include "blackhawk7_v1l8p1_enum.h"
#include "blackhawk7_v1l8p1_types.h"
#include "blackhawk7_v1l8p1_select_defns.h"
#include "blackhawk7_v1l8p1_access.h"
#define GRACEFUL_STOP_TIME 800

#define RELEASE_LOCK_AND_RETURN(__x__) \
    USR_RELEASE_LOCK; \
    return (__x__);

/****************************************************/
/*  CORE Based APIs - Required to be used per Core  */
/****************************************************/
#ifndef SMALL_FOOTPRINT
/* Returns API Version Number */
/** API Version Number.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *api_version API Version Number returned by the API
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_version(srds_access_t *sa__, uint32_t *api_version);
#endif /* SMALL_FOOTPRINT */

/*------------------------------------------------*/
/*  APIs to Read Core Config variables in uC RAM  */
/*------------------------------------------------*/
/** Read value of core_config uC RAM variable.
 *  Note that various API configuration functions can modify core config.
 *  Since the value returned by this can become stale, re-read after modifying core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *struct_val Value to be written into core_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_uc_core_config(srds_access_t *sa__, struct blackhawk7_v1l8p1_uc_core_config_st *struct_val);

/*------------------------------------------------*/
/*  APIs to get physical Tx addr for given lane   */
/*------------------------------------------------*/
/** Read value of physical Tx address for a mapped lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param lane Logical lane address for the lane
 * @param *physical_tx_lane get the physical address of the tx logical lane
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_physical_tx_addr(srds_access_t *sa__, uint8_t lane, uint8_t *physical_tx_lane);

/*------------------------------------------------*/
/*  APIs to get physical Rx addr for given lane   */
/*------------------------------------------------*/
/** Read value of physical Rx address for a mapped lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param lane Logical lane address for the lane
 * @param *physical_rx_lane get the physical address of the rx logical lane
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_physical_rx_addr(srds_access_t *sa__, uint8_t lane, uint8_t *physical_rx_lane);


/*------------------------------------------------*/
/*  APIs to get number of uc cores  */
/*------------------------------------------------*/
/** Get the number of uc cores.
 *  Note that various API configuration functions can modify core config.
 *  Since the value returned by this can become stale, re-read after modifying core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *num_micros Value to be written with number of uc cores
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_micro_num_uc_cores(srds_access_t *sa__, uint8_t *num_micros);

/*-----------------------------------*/
/*  Microcode Load/Verify Functions  */
/*-----------------------------------*/
/** Performs the initial register writes which are required before loading microcode.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param pram indicates if the microcode is loaded through the PRAM interface (1 = yes; 0 = no )
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_load_init(srds_access_t *sa__, uint8_t pram);

/** Writes all or part of the microcode image into Micro.
 * Note: If microcode is being loaded with multiple writes, then ucode_len must be divisble by 4 (32 bit aligned)
 * for all writes, except for the final write, which does not need to be 32 bit aligned.
 * Note: blackhawk7_v1l8p1_ucode_load_init() must be called before the first call to blackhawk7_v1l8p1_ucode_load_write(),
 * and blackhawk7_v1l8p1_ucode_load_close() must be called after the last call to blackhawk7_v1l8p1_ucode_load_write().
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *ucode_image pointer to the Microcode image organized in bytes
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_load_write(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len);

/** Ends Microcode loading state. Call after the last blackhawk7_v1l8p1_ucode_load_write().
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param pram indicates if the microcode is loaded through the PRAM interface (1 = yes; 0 = no )
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_load_close(srds_access_t *sa__, uint8_t pram);

/** Load Microcode into Micro through Register (MDIO) Interface.
 * Once the microcode is loaded, de-assert reset to micro to start executing microcode "wrc_micro_core_rstb(0x1)".
 * \n Note: Micro should be loaded only after issuing a blackhawk7_v1l8p1_uc_reset(1) followed by asserting and de-asserting
 * core_s_reset. Information table should be intialized with blackhawk7_v1l8p1_init_blackhawk7_v1l8p1_info after microcode load.
 * See relevant Programmers guide for more details.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *ucode_image pointer to the Microcode image organized in bytes
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_load(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len);

#ifndef SMALL_FOOTPRINT
/** To verify the Microcode image loaded in the Micro.
 * Read back the microcode from Micro and check against expected microcode image.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *ucode_image pointer to the expeted Microcode image organized in bytes
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_load_verify(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len);

/** To verify the CRC of the microcode loaded in the Micro.
 * Instruct uC to read image and calculate CRC and check against expected CRC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @param expected_crc_value Expected CRC value of the microcode
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_crc_verify(srds_access_t *sa__, uint16_t ucode_len, uint16_t expected_crc_value);

#endif /* SMALL_FOOTPRINT */

/** Load the Micro through the PRAM bus.
 * NOTE: Information table should be intialized with blackhawk7_v1l8p1_init_blackhawk7_v1l8p1_info after microcode load.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_image Microcode Image to be written
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_ucode_pram_load(srds_access_t *sa__, char const * ucode_image, uint32_t ucode_len);

/** Enable or Disable the uC reset.
 * Dummy function to maintain compatibility with BHK16 APIs [calls blackhawk7_v1l8p1_uc_reset(..) internally]
 * Note: Micro should be reset using the API everytime before reloading the microcode
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable uC reset (1 = Enable; 0 = Disable)
 * @param ucode_info struct has information regarding stack size
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_uc_reset_with_info(srds_access_t *sa__, uint8_t enable, ucode_info_t ucode_info);

/** Enable or Disable the uC reset.
 * Note: Micro should be reset using the API everytime before reloading the microcode
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable uC reset (1 = Enable; 0 = Disable)
 * @param ucode_info struct has information regarding stack size
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_uc_reset(srds_access_t *sa__, uint8_t enable, ucode_info_t ucode_info);

/** API to assert Serdes core reset.
 * Issues blackhawk7_v1l8p1_uc_reset followed by blackhawk7_v1l8p1 core reset (core_s_rstb) after selecting com_clk as micro_clk
 * NOTE: This API should be used to issue a core reset instead of directly using the core_s_rstb register 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_info struct has information regarding stack size
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_core_reset(srds_access_t *sa__, ucode_info_t ucode_info);

/** Wait for the uC to become active.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_wait_uc_active(srds_access_t *sa__);

/** Initialize the blackhawk7_v1l8p1_info for the uC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_init_blackhawk7_v1l8p1_info(srds_access_t *sa__);

#ifndef SMALL_FOOTPRINT

/** Set the core_cfg_from_pcs flag in the core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param core_cfg_from_pcs The value to set
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_set_core_config_from_pcs(srds_access_t *sa__, uint8_t core_cfg_from_pcs);

/** Set the osr_2p5_en flag in the core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param osr_2p5_en The value to set
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_set_osr_2p5_en(srds_access_t *sa__, uint8_t osr_2p5_en);

/** Configure the lane address mapping.
 *  Note: Micro and core data path must be in reset when this function is called.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param num_lanes The number of entries in tx_lane_map and rx_lane_map.
 *                  This must match the number of lanes associated with the core.
 * @param *tx_lane_map A num_lanes-sized array of lane indexes to map to the TX lanes.
 *                     Each entry must be from 0 to num_lanes-1, and each entry must be unique.
 * @param *rx_lane_map A num_lanes-sized array of lane indexes to map to the RX lanes.
 *                     Each entry must be from 0 to num_lanes-1, and each entry must be unique.
 *                     If independent TX / RX lane mapping is not enabled for a core, then this must match tx_lane_map.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_map_lanes(srds_access_t *sa__, uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map);

#endif /* SMALL_FOOTPRINT */

/*-----------------*/
/*  Configure PLL  */
/*-----------------*/

/** The pll can be configured by providing all four parameters:  refclk frequency, divider value, VCO output frequency and the PLL options. 
 *  This is the only option in SMALL_FOOTPRINT
 */
#define blackhawk7_v1l8p1_configure_pll_refclk_div_vco blackhawk7_v1l8p1_INTERNAL_configure_pll

#ifndef SMALL_FOOTPRINT
/** PLL Powerdown.
 *
 * Powers down the selected PLL. To power up the PLL, reconfigure the PLL by using
 * one of the "blackhawk7_v1l8p1_configure_pll_" APIs
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_powerdown(srds_access_t *sa__);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on refclk frequency and divider value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param srds_div Divider value (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_div(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           enum blackhawk7_v1l8p1_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on refclk frequency and VCO output frequency.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_vco(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_div_vco(srds_access_t *sa__,
                                        enum blackhawk7_v1l8p1_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on input refclk frequency and divider value.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 2  (enumerated)
 * @param srds_div Divider value based on input reference clock frequency (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_div_div2refclk(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           enum blackhawk7_v1l8p1_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on input refclk frequency and VCO output frequency.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 2 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_vco_div2refclk(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value based on input refclk frequency before refclk divide by 2 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_div_vco_div2refclk(srds_access_t *sa__,
                                        enum blackhawk7_v1l8p1_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);
/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on input refclk frequency and divider value.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 4  (enumerated)
 * @param srds_div Divider value based on input reference clock frequency (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_div_div4refclk(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           enum blackhawk7_v1l8p1_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on input refclk frequency and VCO output frequency.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 4 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_refclk_vco_div4refclk(srds_access_t *sa__,
                                           enum blackhawk7_v1l8p1_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value based on input refclk frequency before refclk divide by 4 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_configure_pll_div_vco_div4refclk(srds_access_t *sa__,
                                        enum blackhawk7_v1l8p1_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);


/** Get the VCO frequency in kHz, based on the reference clock frequency and divider value
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk_freq_hz Reference clock frequency, in Hz.
 * @param srds_div Divider value, in the same encoding as enum #blackhawk7_v1l8p1_pll_div_enum.
 * @param *vco_freq_khz VCO output frequency, in kHz, obtained based on reference clock frequency and dic value
 * @param pll_option Select PLL configuration option from enum #blackhawk7_v1l8p1_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_vco_from_refclk_div(srds_access_t *sa__, uint32_t refclk_freq_hz, enum blackhawk7_v1l8p1_pll_div_enum srds_div, uint32_t *vco_freq_khz, enum blackhawk7_v1l8p1_pll_option_enum pll_option);
#endif /* SMALL_FOOTPRINT */

/**************************************************/
/* LANE Based APIs - Required to be used per Lane */
/**************************************************/

/*------------------------------------------------------------*/
/*  APIs to Write Lane Config and User variables into uC RAM  */
/*------------------------------------------------------------*/
/** Write to lane_config uC RAM variable.
 * Note: This API should be used only during configuration under dp_reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param struct_val Value to be written into lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_set_uc_lane_cfg(srds_access_t *sa__, struct blackhawk7_v1l8p1_uc_lane_config_st struct_val);

#ifndef SMALL_FOOTPRINT
/*-----------------------------------------------------------*/
/*  APIs to Read Lane Config and User variables from uC RAM  */
/*-----------------------------------------------------------*/
/** Read value of lane_config uC RAM variable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *struct_val Value read from lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_get_uc_lane_cfg(srds_access_t *sa__, struct blackhawk7_v1l8p1_uc_lane_config_st *struct_val);
#endif /* SMALL_FOOTPRINT */


/*--------------------------------------------*/
/*  APIs to Enable or Disable datapath reset  */
/*--------------------------------------------*/

/** Enable or Disable TX datapath reset.
 * Asserts handshake signals upon disable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_tx_dp_reset(srds_access_t *sa__, uint8_t enable);

/** Enable or Disable RX datapath reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable RX datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_rx_dp_reset(srds_access_t *sa__, uint8_t enable);

/** Enable or Disable Core datapath reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable Core datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_core_dp_reset(srds_access_t *sa__, uint8_t enable);


/*---------------------------*/
/*  Blackhawk TX Analog APIs  */
/*---------------------------*/
#ifndef SMALL_FOOTPRINT
/** Write TX AFE parameters.
 * NOTE: This API should be used only after lane_dp_reset is released. 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param selects the parameter to write based on #blackhawk7_v1l8p1_tx_afe_settings_enum
 * @param val is the signed input value to the parameter
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_write_tx_afe(srds_access_t *sa__, enum blackhawk7_v1l8p1_tx_afe_settings_enum param, int16_t val);
#endif /* SMALL_FOOTPRINT */

/** Read TX AFE parameters.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param selects the parameter to read based on #blackhawk7_v1l8p1_tx_afe_settings_enum
 * @param *val is the returned signed value of the parameter
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_read_tx_afe(srds_access_t *sa__, enum blackhawk7_v1l8p1_tx_afe_settings_enum param, int16_t *val);

/** Validates Blackhawk TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable_taps Enable TXFIR TAPs based on #blackhawk7_v1l8p1_txfir_tap_enable_enum
 * @param pre2    TXFIR pre2  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param pre1    TXFIR pre1  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param main    TXFIR main  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post1   TXFIR post  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post2   TXFIR post2 tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post3   TXFIR post3 tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_validate_txfir_cfg(srds_access_t *sa__, enum blackhawk7_v1l8p1_txfir_tap_enable_enum enable_taps, int16_t pre2, int16_t pre1, int16_t main, int16_t post1, int16_t post2, int16_t post3);

/** Writes Serdes TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid
 * NOTE: Dynamically switching between enable_6tap and !enable_6tap will cause a 1UI slip in TX 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable_taps Enable TXFIR TAPs based on #blackhawk7_v1l8p1_txfir_tap_enable_enum
 * @param pre2    TXFIR pre2  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param pre1    TXFIR pre1  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param main    TXFIR main  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post1   TXFIR post  tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post2   TXFIR post2 tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @param post3   TXFIR post3 tap value (-168 to +168 when in PAM4 mode, otherwise -127 to 127)
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_apply_txfir_cfg(srds_access_t *sa__, enum blackhawk7_v1l8p1_txfir_tap_enable_enum enable_taps, int16_t pre2, int16_t pre1, int16_t main, int16_t post1, int16_t post2, int16_t post3);


/*----------------*/
/*   PMD_RX_LOCK  */
/*----------------*/

/** PMD rx lock status of current lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *pmd_rx_lock PMD_RX_LOCK status of current lane returned by API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_rx_lock);

/*--------------------------------*/
/*  Serdes TX disable/RX Restart  */
/*--------------------------------*/
/** TX Disable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX disable (1 = TX Disable asserted; 0 = TX Disable removed)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_tx_disable(srds_access_t *sa__, uint8_t enable);

/** Enable/disable Restart RX and hold.
 * (Reset DSC state machine into RESTART State and hold it till disabled)
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable Restart RX and hold (1 = RX restart and hold; 0 = Release hold in restart state)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_rx_restart(srds_access_t *sa__, uint8_t enable);


#ifndef SMALL_FOOTPRINT
/*-----------------------------*/
/*  Stop/Resume RX Adaptation  */
/*-----------------------------*/
/** Stop RX Adaptation on a Lane. Control is returned only after attempting to stop adaptation.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable RX Adaptation stop (1 = Stop RX Adaptation on lane; 0 = Resume RX Adaptation on lane)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_stop_rx_adaptation(srds_access_t *sa__, uint8_t enable);

/** Request to stop RX Adaptation on a Lane.
 * Control will be returned immediately before adaptaion is completely stopped.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * To resume RX adaptation, use the blackhawk7_v1l8p1_stop_rx_adaptation() API.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_request_stop_rx_adaptation(srds_access_t *sa__);

/*------------------------------------*/
/*  Read/Write all RX AFE parameters  */
/*------------------------------------*/

/** Write to RX AFE settings.
 * Note: RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be modified
 * @param val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_write_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t val);

/** Read from RX AFE settings.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be read
 * @param *val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_read_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t *val);
#endif /* SMALL_FOOTPRINT */

/*-----------------------------*/
/*  TX_PI Fixed Frequency Mode */
/*-----------------------------*/

/** TX_PI Fixed Frequency Mode.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX_PI (1 = Enable; 0 = Disable)
 * @param freq_override_val Fixed Frequency Override value (freq_override_val = desired_ppm*8192/781.25; Range: -8192 to + 8192);
 * @return Error Code generated by invalid TX_PI settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_tx_pi_freq_override(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val);

#ifndef SMALL_FOOTPRINT
/** This function allows user to align TX clocks in selected lanes.
 * The current lane is expected to be the master lane. 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable - 0: enable tx_clock_align for selected lanes with current lane as master; 
 *                 1: disable tx_clock_align for selected lanes including master lane
 * @param lane_mask: bit mask for selecting lanes (0x00001111 - lane 0-3 are selected)
 * NOTE: The current lane should be selected as part of the lane_mask; else API errors out.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_tx_clock_align_enable(srds_access_t *sa__, uint8_t enable, uint8_t lane_mask);
#endif /* SMALL_FOOTPRINT */

/*--------------------------------------------*/
/*  Loopback and Ultra-Low Latency Functions  */
/*--------------------------------------------*/

/** Locks TX_PI to Loop timing.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable TX_PI lock to loop timing (1 = Enable Lock; 0 = Disable Lock)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_loop_timing(srds_access_t *sa__, uint8_t enable);

/** Enable/Disable Remote Loopback.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable Remote Loopback (1 = Enable rmt_lpbk; 0 = Disable rmt_lpbk)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t blackhawk7_v1l8p1_rmt_lpbk(srds_access_t *sa__, uint8_t enable);

#endif

#ifdef __cplusplus
}
#endif
