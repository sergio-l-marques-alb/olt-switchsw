/******************************************************************************
*
* FILE
*
*
* DESCRIPTION
*
*
* NOTE
*
*
******************************************************************************/



/******************************************************************************
*HISTORY OF CHANGES
*******************************************************************************
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    taos    2017-1-3      1.0           initial
*
******************************************************************************/
#ifndef CCU_CFG_H
#define CCU_CFG_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/



/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/


/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif

extern RET_STATUS ccu_cfg_rx_ptp_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ptp_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_frm_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_frm_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_layer_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_layer_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_vlan_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_vlan_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_mac_fw_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_mac_fw_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_port_filter_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_port_filter_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_eth_vlan_set(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_eth_vlan_get(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_l2_eth_type_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_l2_eth_type_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_version_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_version_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_l3_ipudp_type_set(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_l3_ipudp_type_get(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_eth_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_eth_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_eth_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_eth_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_mul_norm_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_mul_norm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_mul_peer_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_mul_peer_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ssm_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ssm_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ssm_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ssm_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_eth_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_eth_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_uni_da_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv4_uni_da_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ipv6_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_udp_port_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_udp_port_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_udp_port_ptp_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_udp_port_ptp_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ccu_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ccu_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ccu_sa_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ccu_sa_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ccu_tag_set(UINT_8 chip_id, UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ccu_tag_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ccu_leth_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ccu_leth_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_rx_ccu_vlan_vid_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_rx_ccu_vlan_vid_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ccu_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ccu_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ccu_sa_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ccu_sa_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_cfg_ptp_version_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_cfg_ptp_version_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_cfg_ts_ctr_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_cfg_ts_ctr_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_cfg_message_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_cfg_message_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ccu_vlan_vid_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ccu_vlan_vid_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ccu_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ccu_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_eth_vlan_set(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_eth_vlan_get(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_l2_eth_type_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_l2_eth_type_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_version_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_version_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_l3_ipudp_type_set(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_l3_ipudp_type_get(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_eth_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_eth_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_eth_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_eth_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_mul_norm_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_mul_norm_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_mul_peer_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_mul_peer_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ssm_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ssm_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ssm_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ssm_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_frm_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_frm_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_layer_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_layer_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_vlan_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_vlan_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_mac_fw_en_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_mac_fw_en_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_port_filter_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_port_filter_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_eth_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_eth_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_uni_da_set(UINT_8 chip_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv4_uni_da_get(UINT_8 chip_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_ipv6_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_udp_port_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_udp_port_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);
extern RET_STATUS ccu_cfg_tx_udp_port_ptp_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter);
extern RET_STATUS ccu_cfg_tx_udp_port_ptp_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter);


#ifdef __cplusplus
}
#endif

#endif



