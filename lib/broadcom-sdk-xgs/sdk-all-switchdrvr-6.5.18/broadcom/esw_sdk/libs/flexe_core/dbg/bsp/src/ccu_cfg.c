

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "global_macro.h"
#include <string.h>
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int ccu_cfg_para_debug = 0;

const CHIP_REG CCU_RX_PTP_EN_REG[]=
{
	{CCU_RX_TAG_MATCH_EN,CCU_CFG_BASE_ADDR,0x1,0,0},
	{CCU_RX_PARSE_EN,CCU_CFG_BASE_ADDR,0x1,1,1},
	{CCU_RX_PTP_L2_UNI_EN,CCU_CFG_BASE_ADDR,0x1,2,2},
	{CCU_RX_CCU_PARSE_EN,CCU_CFG_BASE_ADDR,0x1,4,4},
	{CCU_RX_TC_TYPE_MODE,CCU_CFG_BASE_ADDR,0x1,5,5},
	{CCU_RX_CF_UPDATE_MODE,CCU_CFG_BASE_ADDR,0x1,7,6},
	{CCU_RX_ID_MATCH_EN,CCU_CFG_BASE_ADDR,0x1,8,8},
	{CCU_RX_DROP_ERR_CTRL,CCU_CFG_BASE_ADDR,0x1,10,10},
};
const CHIP_REG CCU_RX_FILT_FW_REG[]=
{
	{CCU_RX_FRM_FW,CCU_CFG_BASE_ADDR,0x2,3,0},
	{CCU_RX_LAYER_FW,CCU_CFG_BASE_ADDR,0x2,6,4},
	{CCU_RX_VLAN_FW,CCU_CFG_BASE_ADDR,0x2,9,7},
	{CCU_RX_MAC_FW,CCU_CFG_BASE_ADDR,0x2,10,10},
	{CCU_RX_PORT_FILTER,CCU_CFG_BASE_ADDR,0x2,18,11},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG,CCU_CFG_BASE_ADDR,0x3,15,0},
	{CCU_RX_ETH_CVLAN_TAG,CCU_CFG_BASE_ADDR,0x3,31,16},
};
const CHIP_REG CCU_RX_L2_ETH_TYPE_REG[]=
{
	{CCU_RX_L2_ETH_TYPE,CCU_CFG_BASE_ADDR,0x4,15,0},
	{CCU_RX_PTP_VERSION,CCU_CFG_BASE_ADDR,0x4,19,16},
	{CCU_RX_IPV4_VERSION,CCU_CFG_BASE_ADDR,0x4,23,20},
	{CCU_RX_IPV6_VERSION,CCU_CFG_BASE_ADDR,0x4,27,24},
};
const CHIP_REG CCU_RX_L3_IP_TYPE_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE,CCU_CFG_BASE_ADDR,0x5,15,0},
	{CCU_RX_L3_IPV6_TYPE,CCU_CFG_BASE_ADDR,0x5,31,16},
};
const CHIP_REG CCU_RX_ETH_MUL_NORM_REG[]=
{
	{CCU_RX_ETH_MUL_NORM_0,CCU_CFG_BASE_ADDR,0x6,31,0},
	{CCU_RX_ETH_MUL_NORM_1,CCU_CFG_BASE_ADDR,0x7,15,0},
};
const CHIP_REG CCU_RX_ETH_MUL_PEER_REG[]=
{
	{CCU_RX_ETH_MUL_PEER_0,CCU_CFG_BASE_ADDR,0x8,31,0},
	{CCU_RX_ETH_MUL_PEER_1,CCU_CFG_BASE_ADDR,0x9,15,0},
};
const CHIP_REG CCU_RX_IPV4_MUL_NORM_REG[]=
{
	{CCU_RX_IPV4_MUL_NORM,CCU_CFG_BASE_ADDR,0xa,31,0},
};
const CHIP_REG CCU_RX_IPV4_MUL_PEER_REG[]=
{
	{CCU_RX_IPV4_MUL_PEER,CCU_CFG_BASE_ADDR,0xb,31,0},
};
const CHIP_REG CCU_RX_IPV6_MUL_NORM_REG[]=
{
	{CCU_RX_IPV6_MUL_NORM_0,CCU_CFG_BASE_ADDR,0xc,31,0},
	{CCU_RX_IPV6_MUL_NORM_1,CCU_CFG_BASE_ADDR,0xd,31,0},
	{CCU_RX_IPV6_MUL_NORM_2,CCU_CFG_BASE_ADDR,0xe,31,0},
	{CCU_RX_IPV6_MUL_NORM_3,CCU_CFG_BASE_ADDR,0xf,31,0},
};
const CHIP_REG CCU_RX_IPV6_MUL_PEER_REG[]=
{
	{CCU_RX_IPV6_MUL_PEER_0,CCU_CFG_BASE_ADDR,0x10,31,0},
	{CCU_RX_IPV6_MUL_PEER_1,CCU_CFG_BASE_ADDR,0x11,31,0},
	{CCU_RX_IPV6_MUL_PEER_2,CCU_CFG_BASE_ADDR,0x12,31,0},
	{CCU_RX_IPV6_MUL_PEER_3,CCU_CFG_BASE_ADDR,0x13,31,0},
};
const CHIP_REG CCU_RX_L2_SSM_TYPE_REG[]=
{
	{CCU_RX_L2_SSM_TYPE,CCU_CFG_BASE_ADDR,0x14,15,0},
	{CCU_RX_L2_SSM_SUBTYPE,CCU_CFG_BASE_ADDR,0x14,23,16},
};
const CHIP_REG CCU_RX_SSM_DA_REG[]=
{
	{CCU_RX_SSM_DA_0,CCU_CFG_BASE_ADDR,0x15,31,0},
	{CCU_RX_SSM_DA_1,CCU_CFG_BASE_ADDR,0x16,15,0},
};
const CHIP_REG CCU_RX_ETH_UNI_DA_REG[]=
{
	{CCU_RX_ETH_UNI_NORM_0,CCU_CFG_BASE_ADDR,0x1a,31,0},
	{CCU_RX_ETH_UNI_NORM_1,CCU_CFG_BASE_ADDR,0x1b,15,0},
};
const CHIP_REG CCU_RX_IPV4_UNI_DA_REG[]=
{
	{CCU_RX_IPV4_UNI_NORM,CCU_CFG_BASE_ADDR,0x1c,31,0},
};
const CHIP_REG CCU_RX_IPV6_UNI_DA_REG[]=
{
	{CCU_RX_IPV6_UNI_NORM_0,CCU_CFG_BASE_ADDR,0x1d,31,0},
	{CCU_RX_IPV6_UNI_NORM_1,CCU_CFG_BASE_ADDR,0x1e,31,0},
	{CCU_RX_IPV6_UNI_NORM_2,CCU_CFG_BASE_ADDR,0x1f,31,0},
	{CCU_RX_IPV6_UNI_NORM_3,CCU_CFG_BASE_ADDR,0x20,31,0},
};
const CHIP_REG CCU_RX_UDP_PORT_NORM_REG[]=
{
	{CCU_RX_UDP_PORT_UNI_NORM,CCU_CFG_BASE_ADDR,0x21,15,0},
	{CCU_RX_UDP_PORT_MUL_NORM,CCU_CFG_BASE_ADDR,0x21,31,16},
};
const CHIP_REG CCU_RX_UDP_PORT_PTP_REG[]=
{
	{CCU_RX_UDP_PORT_UNI_OAM,CCU_CFG_BASE_ADDR,0x22,15,0},
	{CCU_RX_UDP_PORT_EVENT,CCU_CFG_BASE_ADDR,0x22,31,16},
};
const CHIP_REG CCU_RX_DA_REG[]=
{
	{CCU_RX_DA_0,CCU_CFG_BASE_ADDR,0x23,31,0},
	{CCU_RX_DA_1,CCU_CFG_BASE_ADDR,0x24,15,0},
};
const CHIP_REG CCU_RX_SA_REG[]=
{
	{CCU_RX_SA_0,CCU_CFG_BASE_ADDR,0x25,31,0},
	{CCU_RX_SA_1,CCU_CFG_BASE_ADDR,0x26,15,0},
};
const CHIP_REG CCU_RX_VLAN_TAG_REG[]=
{
	{CCU_RX_VLAN_TAG,CCU_CFG_BASE_ADDR,0x27,15,0},
};
const CHIP_REG CCU_RX_LETH_TYPE_REG[]=
{
	{CCU_RX_LETH_TYPE,CCU_CFG_BASE_ADDR,0x28,15,0},
	{CCU_RX_LETH_TYPE2,CCU_CFG_BASE_ADDR,0x28,31,16},
};
const CHIP_REG CCU_RX_VLAN_ID0_REG[]=
{
	{CCU_RX_VLAN_VID0,CCU_CFG_BASE_ADDR,0x29,11,0},
	{CCU_RX_VLAN_VID1,CCU_CFG_BASE_ADDR,0x29,27,16},
};
const CHIP_REG CCU_RX_VLAN_ID1_REG[]=
{
	{CCU_RX_VLAN_VID2,CCU_CFG_BASE_ADDR,0x2a,11,0},
	{CCU_RX_VLAN_VID3,CCU_CFG_BASE_ADDR,0x2a,27,16},
};
const CHIP_REG CCU_RX_VLAN_ID2_REG[]=
{
	{CCU_RX_VLAN_VID4,CCU_CFG_BASE_ADDR,0x2b,11,0},
	{CCU_RX_VLAN_VID5,CCU_CFG_BASE_ADDR,0x2b,27,16},
};
const CHIP_REG CCU_RX_VLAN_ID3_REG[]=
{
	{CCU_RX_VLAN_VID6,CCU_CFG_BASE_ADDR,0x2c,11,0},
	{CCU_RX_VLAN_VID7,CCU_CFG_BASE_ADDR,0x2c,27,16},
};
const CHIP_REG CCU_TX_DA_REG[]=
{
	{CCU_TX_DA_0,CCU_CFG_BASE_ADDR,0x30,31,0},
	{CCU_TX_DA_1,CCU_CFG_BASE_ADDR,0x31,15,0},
};
const CHIP_REG CCU_TX_SA_REG[]=
{
	{CCU_TX_SA_0,CCU_CFG_BASE_ADDR,0x32,31,0},
	{CCU_TX_SA_1,CCU_CFG_BASE_ADDR,0x33,15,0},
};
const CHIP_REG CCU_TX_TYPE_REG[]=
{
	{CCU_TX_VLAN_TAG,CCU_CFG_BASE_ADDR,0x34,15,0},
	{CCU_TX_ETH_TYPE,CCU_CFG_BASE_ADDR,0x34,31,16},
};
const CHIP_REG CCU_TX_CFG_REG[]=
{
	{CCU_TX_CFG_PTP_VERSION,CCU_CFG_BASE_ADDR,0x35,7,0},
	{CCU_TX_CFG_MESSAGE,CCU_CFG_BASE_ADDR,0x35,15,8},
	{CCU_TX_CFG_TS_CTR,CCU_CFG_BASE_ADDR,0x35,31,16},
};
const CHIP_REG CCU_TX_VLAN_ID0_REG[]=
{
	{CCU_TX_VLAN_VID0,CCU_CFG_BASE_ADDR,0x36,15,0},
	{CCU_TX_VLAN_VID1,CCU_CFG_BASE_ADDR,0x36,31,16},
};
const CHIP_REG CCU_TX_VLAN_ID1_REG[]=
{
	{CCU_TX_VLAN_VID2,CCU_CFG_BASE_ADDR,0x37,15,0},
	{CCU_TX_VLAN_VID3,CCU_CFG_BASE_ADDR,0x37,31,16},
};
const CHIP_REG CCU_TX_VLAN_ID2_REG[]=
{
	{CCU_TX_VLAN_VID4,CCU_CFG_BASE_ADDR,0x38,15,0},
	{CCU_TX_VLAN_VID5,CCU_CFG_BASE_ADDR,0x38,31,16},
};
const CHIP_REG CCU_TX_VLAN_ID3_REG[]=
{
	{CCU_TX_VLAN_VID6,CCU_CFG_BASE_ADDR,0x39,15,0},
	{CCU_TX_VLAN_VID7,CCU_CFG_BASE_ADDR,0x39,31,16},
};
const CHIP_REG CCU_TX_EN_REG[]=
{
	{CCU_TX_PTP_L2_UNI_EN,CCU_CFG_BASE_ADDR,0x3a,1,1},
	{CCU_TX_CFG_EN,CCU_CFG_BASE_ADDR,0x3a,2,2},
	{CCU_TX_CRC_RECAL_EN,CCU_CFG_BASE_ADDR,0x3a,3,3},
	{CCU_TX_CFG_DIRECTION,CCU_CFG_BASE_ADDR,0x3a,11,4},
	{CCU_TX_CFG_PORT_ID,CCU_CFG_BASE_ADDR,0x3a,19,12},
	{CCU_TX_TC_TYPE_MODE,CCU_CFG_BASE_ADDR,0x3a,20,20},
	{CCU_TX_CF_UPDATE_MODE,CCU_CFG_BASE_ADDR,0x3a,22,21},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG,CCU_CFG_BASE_ADDR,0x3b,15,0},
	{CCU_TX_ETH_CVLAN_TAG,CCU_CFG_BASE_ADDR,0x3b,31,16},
};
const CHIP_REG CCU_TX_L2_ETH_TYPE_REG[]=
{
	{CCU_TX_L2_ETH_TYPE,CCU_CFG_BASE_ADDR,0x3c,15,0},
	{CCU_TX_PTP_VERSION,CCU_CFG_BASE_ADDR,0x3c,19,16},
	{CCU_TX_IPV4_VERSION,CCU_CFG_BASE_ADDR,0x3c,23,20},
	{CCU_TX_IPV6_VERSION,CCU_CFG_BASE_ADDR,0x3c,27,24},
};
const CHIP_REG CCU_TX_L3_IP_TYPE_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE,CCU_CFG_BASE_ADDR,0x3d,15,0},
	{CCU_TX_L3_IPV6_TYPE,CCU_CFG_BASE_ADDR,0x3d,31,16},
};
const CHIP_REG CCU_TX_ETH_MUL_NORM_REG[]=
{
	{CCU_TX_ETH_MUL_NORM_0,CCU_CFG_BASE_ADDR,0x3e,31,0},
	{CCU_TX_ETH_MUL_NORM_1,CCU_CFG_BASE_ADDR,0x3f,15,0},
};
const CHIP_REG CCU_TX_ETH_MUL_PEER_REG[]=
{
	{CCU_TX_ETH_MUL_PEER_0,CCU_CFG_BASE_ADDR,0x40,31,0},
	{CCU_TX_ETH_MUL_PEER_1,CCU_CFG_BASE_ADDR,0x41,15,0},
};
const CHIP_REG CCU_TX_IPV4_MUL_NORM_REG[]=
{
	{CCU_TX_IPV4_MUL_NORM,CCU_CFG_BASE_ADDR,0x42,31,0},
};
const CHIP_REG CCU_TX_IPV4_MUL_PEER_REG[]=
{
	{CCU_TX_IPV4_MUL_PEER,CCU_CFG_BASE_ADDR,0x43,31,0},
};
const CHIP_REG CCU_TX_IPV6_MUL_NORM_REG[]=
{
	{CCU_TX_IPV6_MUL_NORM_0,CCU_CFG_BASE_ADDR,0x44,31,0},
	{CCU_TX_IPV6_MUL_NORM_1,CCU_CFG_BASE_ADDR,0x45,31,0},
	{CCU_TX_IPV6_MUL_NORM_2,CCU_CFG_BASE_ADDR,0x46,31,0},
	{CCU_TX_IPV6_MUL_NORM_3,CCU_CFG_BASE_ADDR,0x47,31,0},
};
const CHIP_REG CCU_TX_IPV6_MUL_PEER_REG[]=
{
	{CCU_TX_IPV6_MUL_PEER_0,CCU_CFG_BASE_ADDR,0x48,31,0},
	{CCU_TX_IPV6_MUL_PEER_1,CCU_CFG_BASE_ADDR,0x49,31,0},
	{CCU_TX_IPV6_MUL_PEER_2,CCU_CFG_BASE_ADDR,0x4a,31,0},
	{CCU_TX_IPV6_MUL_PEER_3,CCU_CFG_BASE_ADDR,0x4b,31,0},
};
const CHIP_REG CCU_TX_L2_SSM_TYPE_REG[]=
{
	{CCU_TX_L2_SSM_TYPE,CCU_CFG_BASE_ADDR,0x4c,15,0},
	{CCU_TX_L2_SSM_SUBTYPE,CCU_CFG_BASE_ADDR,0x4c,23,16},
};
const CHIP_REG CCU_TX_SSM_DA_REG[]=
{
	{CCU_TX_SSM_DA_0,CCU_CFG_BASE_ADDR,0x4d,31,0},
	{CCU_TX_SSM_DA_1,CCU_CFG_BASE_ADDR,0x4e,15,0},
};
const CHIP_REG CCU_TX_FILT_FW_REG[]=
{
	{CCU_TX_FRM_FW,CCU_CFG_BASE_ADDR,0x4f,3,0},
	{CCU_TX_LAYER_FW,CCU_CFG_BASE_ADDR,0x4f,6,4},
	{CCU_TX_VLAN_FW,CCU_CFG_BASE_ADDR,0x4f,9,7},
	{CCU_TX_MAC_FW,CCU_CFG_BASE_ADDR,0x4f,10,10},
	{CCU_TX_PORT_FILTER,CCU_CFG_BASE_ADDR,0x4f,18,11},
};
const CHIP_REG CCU_TX_ETH_UNI_DA_REG[]=
{
	{CCU_TX_ETH_UNI_NORM_0,CCU_CFG_BASE_ADDR,0x52,31,0},
	{CCU_TX_ETH_UNI_NORM_1,CCU_CFG_BASE_ADDR,0x53,15,0},
};
const CHIP_REG CCU_TX_IPV4_UNI_DA_REG[]=
{
	{CCU_TX_IPV4_UNI_NORM,CCU_CFG_BASE_ADDR,0x54,31,0},
};
const CHIP_REG CCU_TX_IPV6_UNI_DA_REG[]=
{
	{CCU_TX_IPV6_UNI_NORM_0,CCU_CFG_BASE_ADDR,0x55,31,0},
	{CCU_TX_IPV6_UNI_NORM_1,CCU_CFG_BASE_ADDR,0x56,31,0},
	{CCU_TX_IPV6_UNI_NORM_2,CCU_CFG_BASE_ADDR,0x57,31,0},
	{CCU_TX_IPV6_UNI_NORM_3,CCU_CFG_BASE_ADDR,0x58,31,0},
};
const CHIP_REG CCU_TX_UDP_PORT_NORM_REG[]=
{
	{CCU_TX_UDP_PORT_UNI_NORM,CCU_CFG_BASE_ADDR,0x59,15,0},
	{CCU_TX_UDP_PORT_MUL_NORM,CCU_CFG_BASE_ADDR,0x59,31,16},
};
const CHIP_REG CCU_TX_UDP_PORT_PTP_REG[]=
{
	{CCU_TX_UDP_PORT_UNI_OAM,CCU_CFG_BASE_ADDR,0x5a,15,0},
	{CCU_TX_UDP_PORT_EVENT,CCU_CFG_BASE_ADDR,0x5a,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE1_REG[]=
{
	{CCU_RX_L2_ETH_TYPE1,CCU_CFG_BASE_ADDR,0x17,15,0},
	{CCU_TX_L2_ETH_TYPE1,CCU_CFG_BASE_ADDR,0x17,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE2_REG[]=
{
	{CCU_RX_L2_ETH_TYPE2,CCU_CFG_BASE_ADDR,0x18,15,0},
	{CCU_TX_L2_ETH_TYPE2,CCU_CFG_BASE_ADDR,0x18,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE3_REG[]=
{
	{CCU_RX_L2_ETH_TYPE3,CCU_CFG_BASE_ADDR,0x19,15,0},
	{CCU_TX_L2_ETH_TYPE3,CCU_CFG_BASE_ADDR,0x19,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE4_REG[]=
{
	{CCU_RX_L2_ETH_TYPE4,CCU_CFG_BASE_ADDR,0x2d,15,0},
	{CCU_TX_L2_ETH_TYPE4,CCU_CFG_BASE_ADDR,0x2d,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE5_REG[]=
{
	{CCU_RX_L2_ETH_TYPE5,CCU_CFG_BASE_ADDR,0x2e,15,0},
	{CCU_TX_L2_ETH_TYPE5,CCU_CFG_BASE_ADDR,0x2e,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE6_REG[]=
{
	{CCU_RX_L2_ETH_TYPE6,CCU_CFG_BASE_ADDR,0x2f,15,0},
	{CCU_TX_L2_ETH_TYPE6,CCU_CFG_BASE_ADDR,0x2f,31,16},
};
const CHIP_REG CCU_L2_ETH_TYPE7_REG[]=
{
	{CCU_RX_L2_ETH_TYPE7,CCU_CFG_BASE_ADDR,0x69,15,0},
	{CCU_TX_L2_ETH_TYPE7,CCU_CFG_BASE_ADDR,0x69,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG1_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG1,CCU_CFG_BASE_ADDR,0x5b,15,0},
	{CCU_RX_ETH_CVLAN_TAG1,CCU_CFG_BASE_ADDR,0x5b,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG2_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG2,CCU_CFG_BASE_ADDR,0x5c,15,0},
	{CCU_RX_ETH_CVLAN_TAG2,CCU_CFG_BASE_ADDR,0x5c,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG3_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG3,CCU_CFG_BASE_ADDR,0x5d,15,0},
	{CCU_RX_ETH_CVLAN_TAG3,CCU_CFG_BASE_ADDR,0x5d,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG4_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG4,CCU_CFG_BASE_ADDR,0x5e,15,0},
	{CCU_RX_ETH_CVLAN_TAG4,CCU_CFG_BASE_ADDR,0x5e,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG5_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG5,CCU_CFG_BASE_ADDR,0x5f,15,0},
	{CCU_RX_ETH_CVLAN_TAG5,CCU_CFG_BASE_ADDR,0x5f,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG6_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG6,CCU_CFG_BASE_ADDR,0x60,15,0},
	{CCU_RX_ETH_CVLAN_TAG6,CCU_CFG_BASE_ADDR,0x60,31,16},
};
const CHIP_REG CCU_RX_ETH_VLAN_TAG7_REG[]=
{
	{CCU_RX_ETH_VLAN_TAG7,CCU_CFG_BASE_ADDR,0x61,15,0},
	{CCU_RX_ETH_CVLAN_TAG7,CCU_CFG_BASE_ADDR,0x61,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG1_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG1,CCU_CFG_BASE_ADDR,0x62,15,0},
	{CCU_TX_ETH_CVLAN_TAG1,CCU_CFG_BASE_ADDR,0x62,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG2_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG2,CCU_CFG_BASE_ADDR,0x63,15,0},
	{CCU_TX_ETH_CVLAN_TAG2,CCU_CFG_BASE_ADDR,0x63,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG3_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG3,CCU_CFG_BASE_ADDR,0x64,15,0},
	{CCU_TX_ETH_CVLAN_TAG3,CCU_CFG_BASE_ADDR,0x64,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG4_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG4,CCU_CFG_BASE_ADDR,0x65,15,0},
	{CCU_TX_ETH_CVLAN_TAG4,CCU_CFG_BASE_ADDR,0x65,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG5_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG5,CCU_CFG_BASE_ADDR,0x66,15,0},
	{CCU_TX_ETH_CVLAN_TAG5,CCU_CFG_BASE_ADDR,0x66,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG6_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG6,CCU_CFG_BASE_ADDR,0x67,15,0},
	{CCU_TX_ETH_CVLAN_TAG6,CCU_CFG_BASE_ADDR,0x67,31,16},
};
const CHIP_REG CCU_TX_ETH_VLAN_TAG7_REG[]=
{
	{CCU_TX_ETH_VLAN_TAG7,CCU_CFG_BASE_ADDR,0x68,15,0},
	{CCU_TX_ETH_CVLAN_TAG7,CCU_CFG_BASE_ADDR,0x68,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE1_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE1,CCU_CFG_BASE_ADDR,0x6a,15,0},
	{CCU_RX_L3_IPV6_TYPE1,CCU_CFG_BASE_ADDR,0x6a,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE2_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE2,CCU_CFG_BASE_ADDR,0x6b,15,0},
	{CCU_RX_L3_IPV6_TYPE2,CCU_CFG_BASE_ADDR,0x6b,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE3_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE3,CCU_CFG_BASE_ADDR,0x6c,15,0},
	{CCU_RX_L3_IPV6_TYPE3,CCU_CFG_BASE_ADDR,0x6c,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE4_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE4,CCU_CFG_BASE_ADDR,0x6d,15,0},
	{CCU_RX_L3_IPV6_TYPE4,CCU_CFG_BASE_ADDR,0x6d,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE5_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE5,CCU_CFG_BASE_ADDR,0x6e,15,0},
	{CCU_RX_L3_IPV6_TYPE5,CCU_CFG_BASE_ADDR,0x6e,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE6_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE6,CCU_CFG_BASE_ADDR,0x6f,15,0},
	{CCU_RX_L3_IPV6_TYPE6,CCU_CFG_BASE_ADDR,0x6f,31,16},
};
const CHIP_REG CCU_RX_L3_IP_TYPE7_REG[]=
{
	{CCU_RX_L3_IPV4_TYPE7,CCU_CFG_BASE_ADDR,0x70,15,0},
	{CCU_RX_L3_IPV6_TYPE7,CCU_CFG_BASE_ADDR,0x70,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE1_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE1,CCU_CFG_BASE_ADDR,0x71,15,0},
	{CCU_TX_L3_IPV6_TYPE1,CCU_CFG_BASE_ADDR,0x71,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE2_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE2,CCU_CFG_BASE_ADDR,0x72,15,0},
	{CCU_TX_L3_IPV6_TYPE2,CCU_CFG_BASE_ADDR,0x72,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE3_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE3,CCU_CFG_BASE_ADDR,0x73,15,0},
	{CCU_TX_L3_IPV6_TYPE3,CCU_CFG_BASE_ADDR,0x73,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE4_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE4,CCU_CFG_BASE_ADDR,0x74,15,0},
	{CCU_TX_L3_IPV6_TYPE4,CCU_CFG_BASE_ADDR,0x74,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE5_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE5,CCU_CFG_BASE_ADDR,0x75,15,0},
	{CCU_TX_L3_IPV6_TYPE5,CCU_CFG_BASE_ADDR,0x75,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE6_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE6,CCU_CFG_BASE_ADDR,0x76,15,0},
	{CCU_TX_L3_IPV6_TYPE6,CCU_CFG_BASE_ADDR,0x76,31,16},
};
const CHIP_REG CCU_TX_L3_IP_TYPE7_REG[]=
{
	{CCU_TX_L3_IPV4_TYPE7,CCU_CFG_BASE_ADDR,0x77,15,0},
	{CCU_TX_L3_IPV6_TYPE7,CCU_CFG_BASE_ADDR,0x77,31,16},
};

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ptp_en_set
*
* DESCRIPTION
*
*	  ccu ge rx ptp en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CCU_RX_TAG_MATCH_EN 1:CCU_RX_PARSE_EN  2:CCU_RX_PTP_L2_UNI_EN 
*			    3:CCU_RX_CCU_PARSE_EN 4:CCU_RX_TC_TYPE_MODE 5:CCU_RX_CF_UPDATE_MODE 6:CCU_RX_ID_MATCH_EN
*				7:CCU_RX_DROP_ERR_CTRL
**	  parameter: field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ptp_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > CCU_RX_DROP_ERR_CTRL))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_PTP_EN_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_PTP_EN_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_PTP_EN_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_PTP_EN_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ptp_en_get
*
* DESCRIPTION
*
*	  ccu ge rx ptp en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CCU_RX_TAG_MATCH_EN 1:CCU_RX_PARSE_EN  2:CCU_RX_PTP_L2_UNI_EN 
*			    3:CCU_RX_CCU_PARSE_EN 4:CCU_RX_TC_TYPE_MODE 5:CCU_RX_CF_UPDATE_MODE 6:CCU_RX_ID_MATCH_EN
*				7:CCU_RX_DROP_ERR_CTRL
**	  parameter*: pointer to field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ptp_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > CCU_RX_DROP_ERR_CTRL))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_PTP_EN_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_PTP_EN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_PTP_EN_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_PTP_EN_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_PTP_EN_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_frm_fw_set
*
* DESCRIPTION
*
*	  Ccu ge frm fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-ptp normal packet, 1-ptp peer delay packet, 2-SSM packet, 3-ARP packet
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_frm_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 3) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_FRM_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_frm_fw_get
*
* DESCRIPTION
*
*	  Ccu ge frm fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-ptp normal packet, 1-ptp peer delay packet, 2-SSM packet, 3-ARP packet
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_frm_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_FRM_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_FRM_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_FRM_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_layer_fw_set
*
* DESCRIPTION
*
*	  Ccu ge layer fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-802.3 eth packet, 1-UDP/IPV4 packet, 2-UDP/IPV6 packet
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_layer_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_LAYER_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_bit_write(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_layer_fw_get
*
* DESCRIPTION
*
*	  Ccu ge layer fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-802.3 eth packet, 1-UDP/IPV4 packet, 2-UDP/IPV6 packet
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_layer_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_LAYER_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_LAYER_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_bit_read(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_LAYER_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_vlan_fw_set
*
* DESCRIPTION
*
*	  Ccu ge vlan fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-packet without VLAN, 1-packet with VLAN 2-packet with CVLAN
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_vlan_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_VLAN_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_bit_write(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_vlan_fw_get
*
* DESCRIPTION
*
*	  Ccu ge vlan fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-packet without VLAN, 1-packet with VLAN 2-packet with CVLAN
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_vlan_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_VLAN_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_VLAN_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_VLAN_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_mac_fw_en_set
*
* DESCRIPTION
*
*	  Ccu ge mac fw en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_mac_fw_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_MAC_FW_EN_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].start_bit,
        CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].start_bit,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_mac_fw_en_get
*
* DESCRIPTION
*
*	  Ccu ge mac fw en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_mac_fw_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_MAC_FW_EN_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_MAC_FW_EN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].start_bit,
        CCU_RX_FILT_FW_REG[CCU_RX_MAC_FW].start_bit,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_port_filter_set
*
* DESCRIPTION
*
*	  Ccu ge port filter cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0~7 port_id
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_port_filter_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 7) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_PORT_FILTER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_port_filter_get
*
* DESCRIPTION
*
*	  Ccu ge port filter cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0~7 port_id
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_port_filter_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_PORT_FILTER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_PORT_FILTER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].base_addr,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].offset_addr,CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].start_bit+pkt_type,
        CCU_RX_FILT_FW_REG[CCU_RX_PORT_FILTER].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_vlan_set
*
* DESCRIPTION
*
*	  Ccu ge rx eth vlan tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_cvlan_sel:0-vlan 1-cvlan
*	  vlan_vid:0-7,vlan_id0-7
**	  parameter: eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_vlan_set(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_cvlan_sel > 1)  ||
		(vlan_vid > 7)||
		(parameter > 0xffff))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_VLAN_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_cvlan_sel = %d, \
													  \r\n vlan_vid = %d, \
													  \r\n parameter = 0x%x \r\n",   
													  chip_id,
													  vlan_cvlan_sel,
													  vlan_vid,
													  parameter);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_ETH_VLAN_TAG + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG_REG[field_id];	break;
		case 1 : field_id = CCU_RX_ETH_VLAN_TAG1 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG1_REG[field_id];	break;
		case 2 : field_id = CCU_RX_ETH_VLAN_TAG2 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_ETH_VLAN_TAG3 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_ETH_VLAN_TAG4 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_ETH_VLAN_TAG5 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_ETH_VLAN_TAG6 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_ETH_VLAN_TAG7 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_vlan_get
*
* DESCRIPTION
*
*	  Ccu ge rx eth vlan tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_cvlan_sel:0-vlan 1-cvlan
*	  vlan_vid:0-7,vlan_id0-7
**	  parameter*: pointer to eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_vlan_get(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_cvlan_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_VLAN_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_cvlan_sel = %d, \
													  \r\n vlan_vid = %d \r\n",   
													  chip_id,
													  vlan_cvlan_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_VLAN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_ETH_VLAN_TAG + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG_REG[field_id];	break;
		case 1 : field_id = CCU_RX_ETH_VLAN_TAG1 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG1_REG[field_id];	break;
		case 2 : field_id = CCU_RX_ETH_VLAN_TAG2 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_ETH_VLAN_TAG3 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_ETH_VLAN_TAG4 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_ETH_VLAN_TAG5 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_ETH_VLAN_TAG6 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_ETH_VLAN_TAG7 + vlan_cvlan_sel;	p = CCU_RX_ETH_VLAN_TAG7_REG[field_id];	break;
		default: break;
	}

	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_l2_eth_type_set
*
* DESCRIPTION
*
*	  Ccu ge rx l2_eth_type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid:0-7:tx_l2_eth_type0-7
**	  parameter: l2_eth_type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_l2_eth_type_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_L2_ETH_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_vid = %d  \r\n",   
													  chip_id,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_L2_ETH_TYPE;	    p = CCU_RX_L2_ETH_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_RX_L2_ETH_TYPE1;	p = CCU_L2_ETH_TYPE1_REG[field_id];	 break;
		case 2 : field_id = CCU_RX_L2_ETH_TYPE2;	p = CCU_L2_ETH_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_L2_ETH_TYPE3;	p = CCU_L2_ETH_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_L2_ETH_TYPE4;	p = CCU_L2_ETH_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_L2_ETH_TYPE5;	p = CCU_L2_ETH_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_L2_ETH_TYPE6;	p = CCU_L2_ETH_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_L2_ETH_TYPE7;	p = CCU_L2_ETH_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_l2_eth_type_get
*
* DESCRIPTION
*
*	  Ccu ge rx l2_eth_type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid:0-7:tx_l2_eth_type0-7
**	  parameter*: pointer to eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_l2_eth_type_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_L2_ETH_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_vid = %d  \r\n",   
													  chip_id,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_L2_ETH_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_L2_ETH_TYPE;	    p = CCU_RX_L2_ETH_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_RX_L2_ETH_TYPE1;	p = CCU_L2_ETH_TYPE1_REG[field_id];	 break;
		case 2 : field_id = CCU_RX_L2_ETH_TYPE2;	p = CCU_L2_ETH_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_L2_ETH_TYPE3;	p = CCU_L2_ETH_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_L2_ETH_TYPE4;	p = CCU_L2_ETH_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_L2_ETH_TYPE5;	p = CCU_L2_ETH_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_L2_ETH_TYPE6;	p = CCU_L2_ETH_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_L2_ETH_TYPE7;	p = CCU_L2_ETH_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_version_set
*
* DESCRIPTION
*
*	  ccu ge rx version cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:  0:CCU_RX_PTP_VERSION 1:CCU_RX_IPV4_VERSION 2:CCU_RX_IPV6_VERSION
**	  parameter: field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_version_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_VERSION_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if ((field_id + 1) > MAX_INDEX(CCU_RX_L2_ETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_VERSION_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_L2_ETH_TYPE_REG[field_id + 1],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_version_get
*
* DESCRIPTION
*
*	  ccu ge rx version.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CCU_RX_PTP_VERSION 1:CCU_RX_IPV4_VERSION 2:CCU_RX_IPV6_VERSION
**	  parameter*: pointer to field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_version_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_RX_VERSION_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_VERSION_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if ((field_id + 1) > MAX_INDEX(CCU_RX_L2_ETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_VERSION_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_L2_ETH_TYPE_REG[field_id + 1],parameter);
	
	return rv;
}


/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_l3_ipudp_type_set
*
* DESCRIPTION
*
*	  ccu ge rx l3 ipudp type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ipv4_ipv6_sel: 0:L3_IPV4_type 1:L3_IPV6_type
*	  vlan_vid:0-7 vlan_id0-7
**	  parameter: field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_l3_ipudp_type_set(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(ipv4_ipv6_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_L3_IPUDP_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ipv4_ipv6_sel = %d, \
													  \r\n vlan_vid = %d \r\n",   
													  chip_id,
													  ipv4_ipv6_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));	
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_L3_IPV4_TYPE + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_RX_L3_IPV4_TYPE1 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE1_REG[field_id];	break;
		case 2 : field_id = CCU_RX_L3_IPV4_TYPE2 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_L3_IPV4_TYPE3 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_L3_IPV4_TYPE4 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_L3_IPV4_TYPE5 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_L3_IPV4_TYPE6 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_L3_IPV4_TYPE7 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE7_REG[field_id];	break;
		default: break;
	}

	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_l3_ipudp_type_get
*
* DESCRIPTION
*
*	  ccu ge rx l3 ipudp type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ipv4_ipv6_sel: 0:L3_IPV4_type 1:L3_IPV6_type
*	  vlan_vid:0-7 vlan_id0-7
**	  parameter*: pointer to field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_l3_ipudp_type_get(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(ipv4_ipv6_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_L3_IPUDP_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ipv4_ipv6_sel = %d, \
													  \r\n vlan_vid = %d \r\n",    
													  chip_id,
													  ipv4_ipv6_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_L3_IPUDP_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_RX_L3_IPV4_TYPE + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_RX_L3_IPV4_TYPE1 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE1_REG[field_id];	break;
		case 2 : field_id = CCU_RX_L3_IPV4_TYPE2 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_RX_L3_IPV4_TYPE3 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_RX_L3_IPV4_TYPE4 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_RX_L3_IPV4_TYPE5 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_RX_L3_IPV4_TYPE6 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_RX_L3_IPV4_TYPE7 + ipv4_ipv6_sel;	p = CCU_RX_L3_IP_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge rx eth mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_norm_0 0~31 1:Eth_mul_norm_1 32~47
**	  parameter: eth mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_ETH_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge rx eth mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0: Eth_mul_norm_0 0~31 1:Eth_mul_norm_1 32~47
**	  parameter*: pointer to eth mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_NORM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_ETH_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge rx eth mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_peer_0 0~31 1:Eth_mul_peer_1 32~47
**	  parameter: eth mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_PEER_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_ETH_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge rx eth mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_peer_0 0~31 1:Eth_mul_peer_1 32~47
**	  parameter*: pointer to eth mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_MUL_PEER_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_ETH_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: ipv4 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_mul_norm_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_IPV4_MUL_NORM_REG[CCU_RX_IPV4_MUL_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to ipv4 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_mul_norm_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV4_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_IPV4_MUL_NORM_REG[CCU_RX_IPV4_MUL_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: ipv4 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_mul_peer_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_RX_IPV4_MUL_PEER_REG[CCU_RX_IPV4_MUL_PEER],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to ipv4 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_mul_peer_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV4_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_IPV4_MUL_PEER_REG[CCU_RX_IPV4_MUL_PEER],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_norm_0 0~31 1:Ipv6_mul_norm_1 32~63 2:Ipv6_mul_norm_2 64~95 3:Ipv6_mul_norm_3 96~127
**	  parameter: ipv6 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_IPV6_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3:0:Ipv6_mul_norm_0 0~31 1:Ipv6_mul_norm_1 32~63 2:Ipv6_mul_norm_2 64~95 3:Ipv6_mul_norm_3 96~127
**	  parameter*: pointer to ipv6 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_NORM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_IPV6_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_peer_0 0~31 1:Ipv6_mul_peer_1 32~63 2:Ipv6_mul_peer_2 64~95 3:Ipv6_mul_peer_3 96~127
**	  parameter: ipv6 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_PEER_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_IPV6_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_peer_0 0~31 1:Ipv6_mul_peer_1 32~63 2:Ipv6_mul_peer_2 64~95 3:Ipv6_mul_peer_3 96~127
**	  parameter*: pointer to ipv6 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_MUL_PEER_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_IPV6_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ssm_type_set
*
* DESCRIPTION
*
*	  Ccu ge rx ssm type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:L2_ssm_type 1:L2_ssm_subtype
**	  parameter: rx ssm type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ssm_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_SSM_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_L2_SSM_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_TYPE_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_L2_SSM_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ssm_type_get
*
* DESCRIPTION
*
*	  Ccu ge rx ssm type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:L2_ssm_type 1:L2_ssm_subtype
**	  parameter*: pointer to rx ssm type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ssm_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_SSM_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_L2_SSM_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_TYPE_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_L2_SSM_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ssm_da_set
*
* DESCRIPTION
*
*	  Ccu ge rx ssm da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:ssm_da_0 1:ssm_da_1
**	  parameter: rx ssm da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ssm_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_SSM_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_SSM_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_SSM_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ssm_da_get
*
* DESCRIPTION
*
*	  Ccu ge rx ssm da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:ssm_da_0 1:ssm_da_1
**	  parameter*: pointer to rx ssm da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ssm_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_SSM_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_SSM_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_SSM_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_SSM_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge rx eth uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Eth_uni_norm_0~Eth_uni_norm_1
**	  parameter: rx eth uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_UNI_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_ETH_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_eth_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge rx eth uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Eth_uni_norm_0~Eth_uni_norm_1
**	  parameter*: pointer to rx eth uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_eth_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_ETH_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ETH_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_ETH_UNI_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_ETH_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: rx ipv4 uni da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_uni_da_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_RX_IPV4_UNI_DA_REG[CCU_RX_IPV4_UNI_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv4_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv4 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to rx ipv4 uni da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv4_uni_da_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV4_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV4_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_IPV4_UNI_DA_REG[CCU_RX_IPV4_UNI_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Ipv6_uni_norm_0~Ipv6_uni_norm_3
**	  parameter: rx ipv6 uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_UNI_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_IPV6_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ipv6_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge rx ipv6 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Ipv6_uni_norm_0~Ipv6_uni_norm_3
**	  parameter*: pointer to rx ipv6 uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ipv6_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_IPV6_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_IPV6_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_IPV6_UNI_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_IPV6_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_udp_port_norm_set
*
* DESCRIPTION
*
*	  Ccu ge rx udp port norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:Udp_port_uni_norm 1:Udp_port_mul_norm
**	  parameter: rx udp port norm 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_udp_port_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_UDP_PORT_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_UDP_PORT_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_UDP_PORT_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_udp_port_norm_get
*
* DESCRIPTION
*
*	  Ccu ge rx udp port norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_norm 1:Udp_port_mul_norm
**	  parameter*: pointer to rx udp port norm 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_udp_port_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_UDP_PORT_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_UDP_PORT_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_NORM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_UDP_PORT_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_udp_port_ptp_set
*
* DESCRIPTION
*
*	  Ccu ge rx udp port ptp cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_oam 1:Udp_port_event
**	  parameter: rx udp port ptp 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_udp_port_ptp_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_UDP_PORT_PTP_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_UDP_PORT_PTP_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_PTP_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_UDP_PORT_PTP_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_udp_port_ptp_get
*
* DESCRIPTION
*
*	  Ccu ge rx udp port ptp cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_oam 1:Udp_port_event
**	  parameter*: pointer to rx udp port ptp 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_udp_port_ptp_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_UDP_PORT_PTP_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_PTP_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_UDP_PORT_PTP_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_UDP_PORT_PTP_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_UDP_PORT_PTP_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_da_set
*
* DESCRIPTION
*
*	  Ccu ge rx ccu da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1 Rx_ccu_da_0~Rx_ccu_da_1
**	  parameter: rx ccu da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_da_get
*
* DESCRIPTION
*
*	  Ccu ge rx ccu da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 Rx_ccu_da_0~Rx_ccu_da_1
**	  parameter*: pointer to rx ccu da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_sa_set
*
* DESCRIPTION
*
*	  Ccu ge rx ccu sa cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 Rx_ccu_sa_0~Rx_ccu_sa_1
**	  parameter: rx ccu sa
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_sa_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_SA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_SA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_SA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_SA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_sa_get
*
* DESCRIPTION
*
*	  Ccu ge rx ccu sa cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 Rx_ccu_sa_0~Rx_ccu_sa_1
**	  parameter*: pointer to rx ccu sa
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_sa_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_SA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_SA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_SA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_SA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_SA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_tag_set
*
* DESCRIPTION
*
*	  Ccu ge rx ccu tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: rx ccu tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_tag_set(UINT_8 chip_id, UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_TAG_SET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_RX_VLAN_TAG_REG[CCU_RX_VLAN_TAG],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_tag_get
*
* DESCRIPTION
*
*	  Ccu ge rx ccu tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to rx ccu tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_tag_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_TAG_GET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_TAG_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_VLAN_TAG_REG[CCU_RX_VLAN_TAG],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_tag2_set
*
* DESCRIPTION
*
*	  Ccu ge rx ccu tag2 cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 0:CCU_RX_LETH_TYPE 1:CCU_RX_LETH_TYPE2
**	  parameter: rx leth_type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_leth_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || 
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_LETH_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d\r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id> MAX_INDEX(CCU_RX_LETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_LETH_TYPE_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_RX_LETH_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_tag2_get
*
* DESCRIPTION
*
*	  Ccu ge rx ccu tag2 cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*     field_id:0~1 0:CCU_RX_LETH_TYPE 1:CCU_RX_LETH_TYPE2
**	  parameter*: pointer to rx leth_type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_leth_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_RX_CCU_LETH_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d\r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_LETH_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_LETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_RX_CCU_TAG2_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_LETH_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_rx_ccu_vlan_vid_set
*
* DESCRIPTION
*
*	  Ccu ge rx ccu_vlan vid cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid: 0~7
*	  parameter: vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_vlan_vid_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    CHIP_REG p;
    UINT_8 field_id = 0;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (vlan_vid > 7) ||
        (parameter > 0xfff))
    {
        if (ccu_cfg_para_debug)
        {
            printf("[CCU_CFG_RX_CCU_VLAN_VID_SET] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n vlan_vid = %d, \
                    \r\n parameter = %x \r\n",   
                    chip_id,
                    vlan_vid,
                    parameter);
        }
        
        return RET_PARAERR;
    }
    memset(&p, 0, sizeof(CHIP_REG));
    switch(vlan_vid)
    {
        case 0 : field_id = CCU_RX_VLAN_VID0;  p = CCU_RX_VLAN_ID0_REG[field_id];   break;
        case 1 : field_id = CCU_RX_VLAN_VID1;  p = CCU_RX_VLAN_ID0_REG[field_id];   break;
        case 2 : field_id = CCU_RX_VLAN_VID2;  p = CCU_RX_VLAN_ID1_REG[field_id];  break;
        case 3 : field_id = CCU_RX_VLAN_VID3;  p = CCU_RX_VLAN_ID1_REG[field_id];  break;
        case 4 : field_id = CCU_RX_VLAN_VID4;  p = CCU_RX_VLAN_ID2_REG[field_id];  break;
        case 5 : field_id = CCU_RX_VLAN_VID5;  p = CCU_RX_VLAN_ID2_REG[field_id];  break;
        case 6 : field_id = CCU_RX_VLAN_VID6;  p = CCU_RX_VLAN_ID3_REG[field_id];  break;
        case 7 : field_id = CCU_RX_VLAN_VID7;  p = CCU_RX_VLAN_ID3_REG[field_id];  break;
        default: break;
    }
    rv = regp_field_write(chip_id,p,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_ge_rx_ccu_vlan_vid_get
*
* DESCRIPTION
*
*	  Ccu ge rx ccu_vlan vid cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid: 0~7
*	  parameter*: pointer to vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_rx_ccu_vlan_vid_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
    UINT_8 field_id = 0;
		
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_vid > 7))
	{
	    if (ccu_cfg_para_debug)
	    {
		    printf("[CCU_CFG_RX_CCU_VLAN_VID_GET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n vlan_vid = %d  \r\n",   
		 											  chip_id,
		 											  vlan_vid);
	    }
        
	    return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_RX_CCU_VLAN_VID_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
    switch(vlan_vid)
    {
        case 0 : field_id = CCU_RX_VLAN_VID0;  p = CCU_RX_VLAN_ID0_REG[field_id];  break;
        case 1 : field_id = CCU_RX_VLAN_VID1;  p = CCU_RX_VLAN_ID0_REG[field_id];  break;
        case 2 : field_id = CCU_RX_VLAN_VID2;  p = CCU_RX_VLAN_ID1_REG[field_id];  break;
        case 3 : field_id = CCU_RX_VLAN_VID3;  p = CCU_RX_VLAN_ID1_REG[field_id];  break;
        case 4 : field_id = CCU_RX_VLAN_VID4;  p = CCU_RX_VLAN_ID2_REG[field_id];  break;
        case 5 : field_id = CCU_RX_VLAN_VID5;  p = CCU_RX_VLAN_ID2_REG[field_id];  break;
        case 6 : field_id = CCU_RX_VLAN_VID6;  p = CCU_RX_VLAN_ID3_REG[field_id];  break;
        case 7 : field_id = CCU_RX_VLAN_VID7;  p = CCU_RX_VLAN_ID3_REG[field_id];  break;
        default: break;
    }

    rv = regp_field_read(chip_id,p,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_da_set
*
* DESCRIPTION
*
*	  Ccu ge tx ccu da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1 tx_ccu_da_0~tx_ccu_da_1
**	  parameter: tx ccu da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_CCU_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_da_get
*
* DESCRIPTION
*
*	  Ccu ge tx ccu da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 tx_ccu_da_0~tx_ccu_da_1
**	  parameter*: pointer to tx ccu da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_CCU_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_sa_set
*
* DESCRIPTION
*
*	  Ccu ge tx ccu sa cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 tx_ccu_sa_0~tx_ccu_sa_1
**	  parameter: tx ccu sa
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_sa_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_CCU_SA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_SA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_SA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_SA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_sa_get
*
* DESCRIPTION
*
*	  Ccu ge tx ccu sa cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 tx_ccu_sa_0~tx_ccu_sa_1
**	  parameter*: pointer to tx ccu sa
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_sa_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_CCU_SA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_SA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_SA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_CCU_SA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_SA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_arp_type_set
*
* DESCRIPTION
*
*	  Ccu ge tx arp type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:CCU_TX_VLAN_TAG 1:CCU_TX_ETH_TYPE
**	  parameter: tx vlan_tag/eth_type 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_TYPE_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_type_get
*
* DESCRIPTION
*
*	  ccu_cfg_tx_type_get.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:CCU_TX_VLAN_TAG 1:CCU_TX_ETH_TYPE
**	  parameter*: pointer to tx vlan_tag/eth_type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ARP_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ARP_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ARP_TYPE_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_TYPE_REG[field_id],parameter);
	
	return rv;
}
/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_ptp_version_set
*
* DESCRIPTION
*
*	  Ccu ge CCU_TX_CFG_PTP_VERSION.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: tx CCU_TX_CFG_PTP_VERSION 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_ptp_version_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_SET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_PTP_VERSION],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_ptp_version_get
*
* DESCRIPTION
*
*	  CCU_TX_CFG_PTP_VERSION.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to tx CCU_TX_CFG_PTP_VERSION
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_ptp_version_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_PTP_VERSION],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_ts_ctr_set
*
* DESCRIPTION
*
*	  Ccu ge CCU_TX_CFG_PTP_VERSION.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: tx vlan_tag/eth_type 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_ts_ctr_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_SET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_TS_CTR],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_ts_ctr_get
*
* DESCRIPTION
*
*	  CCU_TX_CFG_TS_CTR.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to tx_cfg_ts_ctr
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_ts_ctr_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_TS_CTR],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_message_set
*
* DESCRIPTION
*
*	  Ccu ge CCU_TX_CFG_MESSAGE.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: tx vlan_tag/eth_type 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_message_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_SET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	rv = regp_field_write(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_MESSAGE],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_cfg_message_get
*
* DESCRIPTION
*
*	  CCU_TX_CFG_MESSAGE.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to tx_cfg_ts_ctr
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_cfg_message_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CFG_PTP_VERSION_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_CFG_REG[CCU_TX_CFG_MESSAGE],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_vlan_vid_set
*
* DESCRIPTION
*
*	  Ccu ge tx ccu_vlan vid cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid: 0~7
*	  parameter: vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_vlan_vid_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
    UINT_8 field_id = 0;
    
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_vid > 7) ||
		(parameter > 0xffff))
	{
	    if (ccu_cfg_para_debug)
	    {
		    printf("[CCU_CFG_TX_CCU_VLAN_VID_SET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n vlan_vid = %d, \
		 											  \r\n parameter = %x \r\n",   
		 											  chip_id,
		 											  vlan_vid,
		 											  parameter);
	    }
        
	    return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
   switch(vlan_vid)
    {
        case 0 : field_id = CCU_TX_VLAN_VID0;  p = CCU_TX_VLAN_ID0_REG[field_id];   break;
        case 1 : field_id = CCU_TX_VLAN_VID1;  p = CCU_TX_VLAN_ID0_REG[field_id];   break;
        case 2 : field_id = CCU_TX_VLAN_VID2;  p = CCU_TX_VLAN_ID1_REG[field_id];  break;
        case 3 : field_id = CCU_TX_VLAN_VID3;  p = CCU_TX_VLAN_ID1_REG[field_id];  break;
        case 4 : field_id = CCU_TX_VLAN_VID4;  p = CCU_TX_VLAN_ID2_REG[field_id];  break;
        case 5 : field_id = CCU_TX_VLAN_VID5;  p = CCU_TX_VLAN_ID2_REG[field_id];  break;
        case 6 : field_id = CCU_TX_VLAN_VID6;  p = CCU_TX_VLAN_ID3_REG[field_id];  break;
        case 7 : field_id = CCU_TX_VLAN_VID7;  p = CCU_TX_VLAN_ID3_REG[field_id];  break;
        default: break;
    }

    rv = regp_field_write(chip_id,p,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_ge_tx_ccu_vlan_vid_get
*
* DESCRIPTION
*
*	  Ccu ge tx ccu_vlan vid cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid: 0~7
*	  parameter*: pointer to vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_vlan_vid_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
    UINT_8 field_id = 0;
		
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_vid > 7))
	{
	    if (ccu_cfg_para_debug)
	    {
		    printf("[CCU_CFG_TX_CCU_VLAN_VID_GET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n vlan_vid = %d  \r\n",   
		 											  chip_id,
		 											  vlan_vid);
	    }
        
	    return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CCU_VLAN_VID_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
    memset(&p, 0, sizeof(CHIP_REG));
    switch(vlan_vid)
    {
        case 0 : field_id = CCU_TX_VLAN_VID0;  p = CCU_TX_VLAN_ID0_REG[field_id];  break;
        case 1 : field_id = CCU_TX_VLAN_VID1;  p = CCU_TX_VLAN_ID0_REG[field_id];  break;
        case 2 : field_id = CCU_TX_VLAN_VID2;  p = CCU_TX_VLAN_ID1_REG[field_id];  break;
        case 3 : field_id = CCU_TX_VLAN_VID3;  p = CCU_TX_VLAN_ID1_REG[field_id];  break;
        case 4 : field_id = CCU_TX_VLAN_VID4;  p = CCU_TX_VLAN_ID2_REG[field_id];  break;
        case 5 : field_id = CCU_TX_VLAN_VID5;  p = CCU_TX_VLAN_ID2_REG[field_id];  break;
        case 6 : field_id = CCU_TX_VLAN_VID6;  p = CCU_TX_VLAN_ID3_REG[field_id];  break;
        case 7 : field_id = CCU_TX_VLAN_VID7;  p = CCU_TX_VLAN_ID3_REG[field_id];  break;
        default: break;
    }

    rv = regp_field_read(chip_id,p,parameter);

    return rv;
}


/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ccu_en_set
*
* DESCRIPTION
*
*	  Ccu ge tx ccu_en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CCU_TX_PTP_L2_UNI_EN 1:CCU_TX_CFG_EN  2:CCU_TX_CRC_RECAL_EN
*				3:CCU_TX_CFG_DIRECTION 4:CCU_TX_CFG_PORT_ID 5:CCU_TX_TC_TYPE_MODE
*				6:CCU_TX_CF_UPDATE_MODE
*	  parameter: vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
    
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > CCU_TX_CF_UPDATE_MODE))
	{
	    if (ccu_cfg_para_debug)
	    {
		    printf("[CCU_CFG_TX_CCU_EN_SET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n field_id = %d, \
		 											  \r\n parameter = %x \r\n",   
		 											  chip_id,
		 											  field_id,
		 											  parameter);
	    }
        
	    return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_EN_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CCU_EN_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}

    rv = regp_field_write(chip_id,CCU_TX_EN_REG[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_ge_tx_ccu_en_get
*
* DESCRIPTION
*
*	  Ccu ge tx ccu_en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:CCU_TX_PTP_L2_UNI_EN 1:CCU_TX_CFG_EN 2:CCU_TX_CRC_RECAL_EN
*				3:CCU_TX_CFG_DIRECTION 4:CCU_TX_CFG_PORT_ID 5:CCU_TX_TC_TYPE_MODE
*				6:CCU_TX_CF_UPDATE_MODE
*	  parameter*: pointer to vid value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ccu_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
		
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > CCU_TX_CF_UPDATE_MODE))
	{
	    if (ccu_cfg_para_debug)
	    {
		    printf("[CCU_CFG_TX_CCU_EN_GET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n field_id = %d  \r\n",   
		 											  chip_id,
		 											  field_id);
	    }
        
	    return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CCU_EN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
    
	if (field_id > MAX_INDEX(CCU_TX_EN_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_CCU_EN_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
    
    rv = regp_field_read(chip_id,CCU_TX_EN_REG[field_id],parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_vlan_set
*
* DESCRIPTION
*
*	  Ccu ge tx eth vlan tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_cvlan_sel:0-vlan 1-cvlan
*	  vlan_vid:0-7,vlan_id0-7
**	  parameter: eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_vlan_set(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_cvlan_sel > 1) ||
		(vlan_vid > 7) ||
		(parameter > 0xffff))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_VLAN_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_cvlan_sel = %d, \
													  \r\n vlan_vid = %d, \
													  \r\n parameter = 0x%x \r\n",   
													  chip_id,
													  vlan_cvlan_sel,
													  vlan_vid,
													  parameter);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_ETH_VLAN_TAG + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG_REG[field_id];	break;
		case 1 : field_id = CCU_TX_ETH_VLAN_TAG1 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG1_REG[field_id];	break;
		case 2 : field_id = CCU_TX_ETH_VLAN_TAG2 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_ETH_VLAN_TAG3 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_ETH_VLAN_TAG4 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_ETH_VLAN_TAG5 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_ETH_VLAN_TAG6 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_ETH_VLAN_TAG7 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG7_REG[field_id];	break;
		default: break;
	}

	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_vlan_get
*
* DESCRIPTION
*
*	  Ccu ge tx eth vlan tag cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_cvlan_sel:0-vlan 1-cvlan
*	  vlan_vid:0-7,vlan_id0-7
**	  parameter*: pointer to eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_vlan_get(UINT_8 chip_id,UINT_8 vlan_cvlan_sel,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(vlan_cvlan_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_VLAN_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_cvlan_sel = %d, \
													  \r\n vlan_vid = %d \r\n",   
													  chip_id,
													  vlan_cvlan_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_VLAN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_ETH_VLAN_TAG + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG_REG[field_id];	break;
		case 1 : field_id = CCU_TX_ETH_VLAN_TAG1 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG1_REG[field_id];	break;
		case 2 : field_id = CCU_TX_ETH_VLAN_TAG2 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_ETH_VLAN_TAG3 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_ETH_VLAN_TAG4 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_ETH_VLAN_TAG5 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_ETH_VLAN_TAG6 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_ETH_VLAN_TAG7 + vlan_cvlan_sel;	p = CCU_TX_ETH_VLAN_TAG7_REG[field_id];	break;
		default: break;
	}

	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_l2_eth_type_set
*
* DESCRIPTION
*
*	  Ccu ge tx l2_eth_type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  vlan_vid:0-7:tx_l2_eth_type0-7
**	  parameter: l2_eth_type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_l2_eth_type_set(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;
	
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_L2_ETH_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_vid = %d  \r\n",   
													  chip_id,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_L2_ETH_TYPE;	    p = CCU_TX_L2_ETH_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_TX_L2_ETH_TYPE1;	p = CCU_L2_ETH_TYPE1_REG[field_id];	 break;
		case 2 : field_id = CCU_TX_L2_ETH_TYPE2;	p = CCU_L2_ETH_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_L2_ETH_TYPE3;	p = CCU_L2_ETH_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_L2_ETH_TYPE4;	p = CCU_L2_ETH_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_L2_ETH_TYPE5;	p = CCU_L2_ETH_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_L2_ETH_TYPE6;	p = CCU_L2_ETH_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_L2_ETH_TYPE7;	p = CCU_L2_ETH_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_l2_eth_type_get
*
* DESCRIPTION
*
*	  Ccu ge tx l2_eth_type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to eth vlan tag
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_l2_eth_type_get(UINT_8 chip_id,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_L2_ETH_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n vlan_vid = %d  \r\n",   
													  chip_id,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_L2_ETH_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_L2_ETH_TYPE;  	p = CCU_TX_L2_ETH_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_TX_L2_ETH_TYPE1;	p = CCU_L2_ETH_TYPE1_REG[field_id];	 break;
		case 2 : field_id = CCU_TX_L2_ETH_TYPE2;	p = CCU_L2_ETH_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_L2_ETH_TYPE3;	p = CCU_L2_ETH_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_L2_ETH_TYPE4;	p = CCU_L2_ETH_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_L2_ETH_TYPE5;	p = CCU_L2_ETH_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_L2_ETH_TYPE6;	p = CCU_L2_ETH_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_L2_ETH_TYPE7;	p = CCU_L2_ETH_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_version_set
*
* DESCRIPTION
*
*	  ccu ge tx version cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:  0:CCU_TX_PTP_VERSION 1:CCU_TX_IPV4_VERSION 2:CCU_TX_IPV6_VERSION
**	  parameter: field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_version_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_VERSION_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if ((field_id + 1) > MAX_INDEX(CCU_TX_L2_ETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_VERSION_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_L2_ETH_TYPE_REG[field_id + 1],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_version_get
*
* DESCRIPTION
*
*	  ccu ge tx version.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CCU_TX_PTP_VERSION 1:CCU_TX_IPV4_VERSION 2:CCU_TX_IPV6_VERSION
**	  parameter*: pointer to field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_version_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_VERSION_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_VERSION_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if ((field_id + 1) > MAX_INDEX(CCU_TX_L2_ETH_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_VERSION_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_L2_ETH_TYPE_REG[field_id + 1],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_l3_ipudp_type_set
*
* DESCRIPTION
*
*	  ccu ge tx l3 ipudp type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ipv4_ipv6_sel: 0:L3_IPV4_type 1:L3_IPV6_type
*	  vlan_vid:0-7 vlan_id0-7
**	  parameter: field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_l3_ipudp_type_set(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(ipv4_ipv6_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_L3_IPUDP_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ipv4_ipv6_sel = %d, \
													  \r\n vlan_vid = %d\r\n",   
													  chip_id,
													  ipv4_ipv6_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}
    memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_L3_IPV4_TYPE + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_TX_L3_IPV4_TYPE1 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE1_REG[field_id];	break;
		case 2 : field_id = CCU_TX_L3_IPV4_TYPE2 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_L3_IPV4_TYPE3 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_L3_IPV4_TYPE4 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_L3_IPV4_TYPE5 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_L3_IPV4_TYPE6 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_L3_IPV4_TYPE7 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE7_REG[field_id];	break;
		default: break;
	}
	
	rv = regp_field_write(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_l3_ipudp_type_get
*
* DESCRIPTION
*
*	  ccu ge tx l3 ipudp type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ipv4_ipv6_sel: 0:L3_IPV4_type 1:L3_IPV6_type
*	  vlan_vid:0-7 vlan_id0-7
**	  parameter*: pointer to field value
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_l3_ipudp_type_get(UINT_8 chip_id,UINT_8 ipv4_ipv6_sel,UINT_8 vlan_vid,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(ipv4_ipv6_sel > 1) ||
		(vlan_vid > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_L3_IPUDP_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ipv4_ipv6_sel = %d, \
													  \r\n vlan_vid = %d \r\n",   
													  chip_id,
													  ipv4_ipv6_sel,
													  vlan_vid);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_L3_IPUDP_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
        memset(&p, 0, sizeof(CHIP_REG));
	switch(vlan_vid)
	{
		case 0 : field_id = CCU_TX_L3_IPV4_TYPE + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE_REG[field_id];	break;
		case 1 : field_id = CCU_TX_L3_IPV4_TYPE1 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE1_REG[field_id];	break;
		case 2 : field_id = CCU_TX_L3_IPV4_TYPE2 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE2_REG[field_id];	break;
		case 3 : field_id = CCU_TX_L3_IPV4_TYPE3 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE3_REG[field_id];	break;
		case 4 : field_id = CCU_TX_L3_IPV4_TYPE4 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE4_REG[field_id];	break;
		case 5 : field_id = CCU_TX_L3_IPV4_TYPE5 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE5_REG[field_id];	break;
		case 6 : field_id = CCU_TX_L3_IPV4_TYPE6 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE6_REG[field_id];	break;
		case 7 : field_id = CCU_TX_L3_IPV4_TYPE7 + ipv4_ipv6_sel;	p = CCU_TX_L3_IP_TYPE7_REG[field_id];	break;
		default: break;
	}

	rv = regp_field_read(chip_id,p,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge tx eth mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_norm_0 0~31 1:Eth_mul_norm_1 32~47
**	  parameter: eth mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_ETH_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge tx eth mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0: Eth_mul_norm_0 0~31 1:Eth_mul_norm_1 32~47
**	  parameter*: pointer to eth mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_ETH_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge tx eth mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_peer_0 0~31 1:Eth_mul_peer_1 32~47
**	  parameter: eth mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_PEER_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_ETH_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge tx eth mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0: Eth_mul_peer_0 0~31 1:Eth_mul_peer_1 32~47
**	  parameter*: pointer to eth mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_MUL_PEER_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_ETH_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: ipv4 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_mul_norm_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}


	rv = regp_field_write(chip_id,CCU_TX_IPV4_MUL_NORM_REG[CCU_TX_IPV4_MUL_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to ipv4 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_mul_norm_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV4_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_IPV4_MUL_NORM_REG[CCU_TX_IPV4_MUL_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: ipv4 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_mul_peer_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_IPV4_MUL_PEER_REG[CCU_TX_IPV4_MUL_PEER],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to ipv4 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_mul_peer_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV4_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_IPV4_MUL_PEER_REG[CCU_TX_IPV4_MUL_PEER],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_mul_norm_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_norm_0 0~31 1:Ipv6_mul_norm_1 32~63 2:Ipv6_mul_norm_2 64~95 3:Ipv6_mul_norm_3 96~127
**	  parameter: ipv6 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_mul_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_MUL_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_IPV6_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_mul_norm_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 mul norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3:0:Ipv6_mul_norm_0 0~31 1:Ipv6_mul_norm_1 32~63 2:Ipv6_mul_norm_2 64~95 3:Ipv6_mul_norm_3 96~127
**	  parameter*: pointer to ipv6 mul norm
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_mul_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_MUL_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_MUL_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_NORM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_IPV6_MUL_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_mul_peer_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_peer_0 0~31 1:Ipv6_mul_peer_1 32~63 2:Ipv6_mul_peer_2 64~95 3:Ipv6_mul_peer_3 96~127
**	  parameter: ipv6 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_mul_peer_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_MUL_PEER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_PEER_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_IPV6_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_mul_peer_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 mul peer cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~3:0:Ipv6_mul_peer_0 0~31 1:Ipv6_mul_peer_1 32~63 2:Ipv6_mul_peer_2 64~95 3:Ipv6_mul_peer_3 96~127
**	  parameter*: pointer to ipv6 mul peer
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_mul_peer_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_MUL_PEER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_PEER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_MUL_PEER_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_MUL_PEER_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_IPV6_MUL_PEER_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ssm_type_set
*
* DESCRIPTION
*
*	  Ccu ge tx ssm type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:L2_ssm_type 1:L2_ssm_subtype
**	  parameter: tx ssm type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ssm_type_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_SSM_TYPE_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_L2_SSM_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_TYPE_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_L2_SSM_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ssm_type_get
*
* DESCRIPTION
*
*	  Ccu ge tx ssm type cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:L2_ssm_type 1:L2_ssm_subtype
**	  parameter*: pointer to tx ssm type
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ssm_type_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_SSM_TYPE_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_TYPE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_L2_SSM_TYPE_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_TYPE_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_L2_SSM_TYPE_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ssm_da_set
*
* DESCRIPTION
*
*	  Ccu ge tx ssm da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:ssm_da_0 1:ssm_da_1
**	  parameter: tx ssm da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ssm_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_SSM_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_SSM_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_SSM_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ssm_da_get
*
* DESCRIPTION
*
*	  Ccu ge tx ssm da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:ssm_da_0 1:ssm_da_1
**	  parameter*: pointer to tx ssm da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ssm_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_SSM_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_SSM_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_SSM_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_SSM_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_frm_fw_set
*
* DESCRIPTION
*
*	  Ccu ge frm fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-ptp normal packet, 1-ptp peer delay packet, 2-SSM packet, 3-ARP packet
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_frm_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 3) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_FRM_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_bit_write(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_frm_fw_get
*
* DESCRIPTION
*
*	  Ccu ge frm fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-ptp normal packet, 1-ptp peer delay packet, 2-SSM packet, 3-ARP packet
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_frm_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_FRM_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_FRM_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	
	rv = regp_bit_read(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_FRM_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_layer_fw_set
*
* DESCRIPTION
*
*	  Ccu ge layer fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-802.3 eth packet, 1-UDP/IPV4 packet, 2-UDP/IPV6 packet
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_layer_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_LAYER_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_layer_fw_get
*
* DESCRIPTION
*
*	  Ccu ge layer fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-802.3 eth packet, 1-UDP/IPV4 packet, 2-UDP/IPV6 packet
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_layer_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_LAYER_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_LAYER_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_LAYER_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_vlan_fw_set
*
* DESCRIPTION
*
*	  Ccu ge vlan fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-packet without VLAN, 1-packet with VLAN 2-packet with CVLAN
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_vlan_fw_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_VLAN_FW_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_vlan_fw_get
*
* DESCRIPTION
*
*	  Ccu ge vlan fw cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0-packet without VLAN, 1-packet with VLAN 2-packet with CVLAN
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_vlan_fw_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 2))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_VLAN_FW_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_VLAN_FW_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	
	rv = regp_bit_read(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_VLAN_FW].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_mac_fw_en_set
*
* DESCRIPTION
*
*	  Ccu ge mac fw en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_mac_fw_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_MAC_FW_EN_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	rv = regp_bit_write(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].start_bit,
        CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].start_bit,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_mac_fw_en_get
*
* DESCRIPTION
*
*	  Ccu ge mac fw en cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_mac_fw_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_MAC_FW_EN_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_MAC_FW_EN_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_bit_read(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].start_bit,
        CCU_TX_FILT_FW_REG[CCU_TX_MAC_FW].start_bit,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_port_filter_set
*
* DESCRIPTION
*
*	  Ccu ge port filter cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0~7 port_id
**	  parameter: 0 is not forwarded, 1 is forwarded
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_port_filter_set(UINT_8 chip_id,UINT_8 pkt_type,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 7) ||
		(parameter > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_PORT_FILTER_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d, \
													  \r\n parameter = %d \r\n",   
													  chip_id,
													  pkt_type,
													  parameter);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_bit_write(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_port_filter_get
*
* DESCRIPTION
*
*	  Ccu ge port filter cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  pkt_type: 0~7 port_id
**	  parameter*: pointer to forwarded status
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_port_filter_get(UINT_8 chip_id,UINT_8 pkt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(pkt_type > 7))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CFG_TX_PORT_FILTER_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n pkt_type = %d  \r\n",   
													  chip_id,
													  pkt_type);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CFG_TX_PORT_FILTER_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_bit_read(chip_id,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].base_addr,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].offset_addr,CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].start_bit+pkt_type,
        CCU_TX_FILT_FW_REG[CCU_TX_PORT_FILTER].start_bit+pkt_type,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge tx eth uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1 Eth_uni_norm_0~Eth_uni_norm_1
**	  parameter: tx eth uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_UNI_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_ETH_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_eth_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge tx eth uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1Eth_uni_norm_0~Eth_uni_norm_1
**	  parameter*: pointer to tx eth uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_eth_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_ETH_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ETH_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_ETH_UNI_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_ETH_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter: tx ipv4 uni da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_uni_da_set(UINT_8 chip_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_IPV4_UNI_DA_REG[CCU_TX_IPV4_UNI_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv4_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv4 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to tx ipv4 uni da
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv4_uni_da_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV4_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV4_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	
	rv = regp_field_read(chip_id,CCU_TX_IPV4_UNI_DA_REG[CCU_TX_IPV4_UNI_NORM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_uni_da_set
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Ipv6_uni_norm_0~Ipv6_uni_norm_3
**	  parameter: tx ipv6 uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_uni_da_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_UNI_DA_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_UNI_DA_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_IPV6_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_ipv6_uni_da_get
*
* DESCRIPTION
*
*	  Ccu ge tx ipv6 uni da cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: Ipv6_uni_norm_0~Ipv6_uni_norm_3
**	  parameter*: pointer to tx ipv6 uni da 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_ipv6_uni_da_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 3))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_IPV6_UNI_DA_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_UNI_DA_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_IPV6_UNI_DA_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_IPV6_UNI_DA_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_IPV6_UNI_DA_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_udp_port_norm_set
*
* DESCRIPTION
*
*	  Ccu ge tx udp port norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:Udp_port_uni_norm 1:Udp_port_mul_norm
**	  parameter: tx udp port norm 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_udp_port_norm_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_UDP_PORT_NORM_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_UDP_PORT_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_NORM_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_UDP_PORT_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_udp_port_norm_get
*
* DESCRIPTION
*
*	  Ccu ge tx udp port norm cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_norm 1:Udp_port_mul_norm
**	  parameter*: pointer to tx udp port norm 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_udp_port_norm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_UDP_PORT_NORM_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_NORM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_UDP_PORT_NORM_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_NORM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_UDP_PORT_NORM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_udp_port_ptp_set
*
* DESCRIPTION
*
*	  Ccu ge tx udp port ptp cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_oam 1:Udp_port_event
**	  parameter: tx udp port ptp 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_udp_port_ptp_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_UDP_PORT_PTP_SET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_UDP_PORT_PTP_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_PTP_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_write(chip_id,CCU_TX_UDP_PORT_PTP_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_cfg_tx_udp_port_ptp_get
*
* DESCRIPTION
*
*	  Ccu ge tx udp port ptp cfg.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:Udp_port_uni_oam 1:Udp_port_event
**	  parameter*: pointer to tx udp port ptp 
* RETURNS
*
*	  0: success
*	  1: fail
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 taos       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_cfg_tx_udp_port_ptp_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
	   if (ccu_cfg_para_debug)
	   {
		   printf("[CCU_CGE_TX_UDP_PORT_PTP_GET] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_PTP_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_UDP_PORT_PTP_REG))
	{
		if (ccu_cfg_para_debug)
		{
			printf("[CCU_CGE_TX_UDP_PORT_PTP_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_UDP_PORT_PTP_REG[field_id],parameter);
	
	return rv;
}

