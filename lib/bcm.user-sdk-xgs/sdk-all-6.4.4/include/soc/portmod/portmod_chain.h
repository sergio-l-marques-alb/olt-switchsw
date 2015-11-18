/*
 *         
 * $Id:$
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
 *     
 *
 */

#ifndef _PORTMOD_CHAIN_H_
#define _PORTMOD_CHAIN_H_
int portmod_port_phychain_core_probe (phymod_core_access_t* core,
                                      const int chain_length);
int portmod_port_phychain_core_identify ( phymod_core_access_t* core,
                                          const int chain_length,
                                          uint32_t core_id,
                                          uint32_t* identified);
int portmod_port_phychain_core_lane_map_get(const phymod_core_access_t* core,
                                            const int chain_length,
                                            phymod_lane_map_t* lane_map);
int portmod_port_phychain_core_reset_set(const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t direction);
int portmod_port_phychain_core_reset_get(const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t* direction );
int portmod_port_phychain_core_firmware_info_get(const phymod_core_access_t* core,
                                                 const int chain_length,
                                                 phymod_core_firmware_info_t* fw_info); 
int portmod_port_phychain_loopback_get(const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_loopback_mode_t loopback,
                                       uint32_t *enable);
int portmod_port_phychain_loopback_set(const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_loopback_mode_t loopback,
                                       uint32_t enable);
int portmod_port_phychain_firmware_core_config_set(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_core_config_t fw_core_config);
int phymod_chain_phy_firmware_core_config_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_firmware_core_config_t *fw_core_config);
int portmod_port_phychain_firmware_lane_config_set(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t fw_lane_config);
int portmod_port_phychain_firmware_lane_config_get(const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t *fw_lane_config);
int portmod_port_phychain_core_pll_sequencer_restart(const phymod_core_access_t* core,
                                                     const int chain_length,
                                                     uint32_t flags,
                                                     phymod_sequencer_operation_t operation);
int phymod_chain_core_wait_event(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_core_event_t event,
                                 uint32_t timeout);
int portmod_port_phychain_rx_restart(const phymod_phy_access_t* phy_access,
                                     const int chain_length);
int portmod_port_phychain_polarity_set(const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       const phymod_polarity_t* polarity);
int portmod_port_phychain_polarity_get(const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_polarity_t* polarity);
int portmod_port_phychain_tx_set(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_tx_t* tx);
int portmod_port_phychain_tx_get(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_tx_t* tx);
int portmod_port_phychain_media_type_tx_get(const phymod_phy_access_t* phy_access,
                                           const int chain_length,
                                           phymod_media_typed_t media,
                                           phymod_tx_t* tx);
int portmod_port_phychain_tx_override_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_tx_override_t* tx_override);
int portmod_port_phychain_tx_override_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_tx_override_t* tx_override);
int portmod_port_phychain_rx_set(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_rx_t* rx);
int portmod_port_phychain_rx_get(const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_rx_t* rx);
int portmod_port_phychain_reset_set(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_reset_t* reset);
int portmod_port_phychain_reset_get(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_reset_t* reset);
int portmod_port_phychain_power_set(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_power_t* power);
int portmod_port_phychain_power_get(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_power_t* power);
int portmod_port_phychain_tx_lane_control_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_tx_lane_control_t tx_control);
int portmod_port_phychain_tx_lane_control_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_tx_lane_control_t *tx_control);
int portmod_port_phychain_rx_lane_control_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_rx_lane_control_t rx_control);
int portmod_port_phychain_rx_lane_control_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_rx_lane_control_t *rx_control);
int portmod_port_phychain_fec_enable_set(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t enable);
int portmod_port_phychain_fec_enable_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *enable);
int portmod_port_phychain_eee_set(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t enable);
int portmod_port_phychain_eee_get(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t *enable);
int portmod_port_phychain_interface_config_set(const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t flags,
                                               const phymod_phy_inf_config_t* config);
int portmod_port_phychain_interface_config_get(const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t flags,
                                  phymod_ref_clk_t ref_clock,
                                  phymod_phy_inf_config_t* config);
int portmod_port_phychain_cl72_set(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t cl72_en);
int portmod_port_phychain_cl72_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *cl72_en);
int portmod_port_phychain_cl72_status_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_cl72_status_t *status);
int portmod_port_phychain_autoneg_ability_set(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              const phymod_autoneg_ability_t* an_ability);
int portmod_port_phychain_autoneg_ability_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_autoneg_ability_t* an_ability);
int portmod_port_phychain_autoneg_remote_ability_get(const phymod_phy_access_t* phy_access,
                                                     const int chain_length,
                                                     phymod_autoneg_ability_t* an_ability);
int portmod_port_phychain_autoneg_set(const phymod_phy_access_t* phy_access,
                                      const int chain_length,
                                      const phymod_autoneg_control_t *an_config);
int portmod_port_phychain_autoneg_get(const phymod_phy_access_t* phy_access,
                                      const int chain_length,
                                      phymod_autoneg_control_t *an_config,
                                      uint32_t * an_done);
int portmod_port_phychain_autoneg_status_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_autoneg_status_t *status);
int portmod_port_phychain_core_init(const phymod_core_access_t* core,
                                    const int chain_length,
                                    const phymod_core_init_config_t* init_config,
                                    const phymod_core_status_t* core_status);
int portmod_port_phychain_pll_multiplier_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *core_vco_pll_multiplier);
int portmod_port_phychain_phy_init(const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   const phymod_phy_init_config_t* init_config);
int portmod_port_phychain_rx_pmd_locked_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *rx_pmd_locked);
int portmod_port_phychain_link_status_get(const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *link_status);
int portmod_port_phychain_pcs_userspeed_set(const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            const phymod_pcs_userspeed_config_t* config);
int portmod_port_phychain_pcs_userspeed_get(const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            phymod_pcs_userspeed_config_t* config);
int portmod_port_phychain_reg_write(const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    uint32_t reg_addr,
                                    uint32_t reg_val);
int portmod_port_phychain_reg_read(const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   uint32_t reg_addr,
                                   uint32_t *reg_val);

int portmod_port_phychain_rx_slicer_position_set(const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 const phymod_slicer_position_t* position);
int portmod_port_phychain_rx_slicer_position_get(const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position);
int portmod_port_phychain_rx_slicer_position_max_get(const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position_min,
                                                 phymod_slicer_position_t* position_max);
int portmod_port_phychain_prbs_config_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          const phymod_prbs_t* prbs);
int portmod_port_phychain_prbs_config_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_t* prbs);
int portmod_port_phychain_prbs_enable_set(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t enable);
int portmod_port_phychain_prbs_enable_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t *enable);
int portmod_port_phychain_prbs_status_get(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_status_t *prbs_status);
int portmod_port_phychain_pattern_config_set(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             const phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_config_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_enable_set(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t enable,
                                             phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_enable_get(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *enable);
int portmod_port_phychain_core_diagnostics_get(const phymod_core_access_t* core,
                                               const int chain_length,
                                               phymod_core_diagnostics_t* diag);
int portmod_port_phychain_phy_diagnostics_get(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_diagnostics_t *diag);
int portmod_port_phychain_pmd_info_dump(const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type);
int portmod_port_phychain_pcs_info_dump(const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type);
int portmod_port_phychain_meas_lowber_eye(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_phy_eyescan_options_t eyescan_options,
                                          uint32_t *buffer);
int portmod_port_phychain_display_lowber_eye(const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_phy_eyescan_options_t eyescan_options,
                                             uint32_t *buffer);
int portmod_port_phychain_pmd_ber_end_cmd(const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint8_t supp_info,
                                          uint32_t timeout_ms);
int portmod_port_phychain_meas_eye_scan_start(const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint8_t direction);
int portmod_port_phychain_meas_eye_scan_done(const phymod_phy_access_t* phy_access,
                                             const int chain_length);
int portmod_port_phychain_read_eye_scan_stripe(const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t *buffer,
                                               uint16_t *status);
int portmod_port_phychain_display_eye_scan_header(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index);
int portmod_port_phychain_display_eye_scan_footer(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index);
int portmod_port_phychain_display_eye_scan_stripe(const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index,
                                                  uint32_t *buffer);
int portmod_port_phychain_eye_scan_debug_info_dump(const phymod_phy_access_t* phy_access,
                                                   const int chain_length);

#endif /*_PORTMOD_CHAIN_H_*/
