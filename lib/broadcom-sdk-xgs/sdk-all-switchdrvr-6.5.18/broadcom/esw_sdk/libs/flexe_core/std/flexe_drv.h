#ifndef _FLEXE_DRV_H
#define _FLEXE_DRV_H

#include <shared/bitop.h>
#include <shared/shrextend/shrextend_error.h>

#define FLEXE_CORE_NOF_PORTS 8
#define FLEXE_CORE_NOF_INSTANCES 8
#define FLEXE_CORE_NOF_INSTANCES_DIV2 4
#define FLEXE_CORE_NOF_LPHYS 8

#define FLEXE_CORE_NOF_TIMESLOTS 80
#define FLEXE_NOF_TIMESLOTS_PER_INSTANCE 20
#define FLEXE_CORE_NOF_SUBCALS 10
#define FLEXE_SAR_NOF_TIMESLOTS 80
#define FLEXE_TINYMAC_NOF_TIMESLOTS 81

#define FLEXE_CORE_NOF_CHANNELS 80
#define FLEXE_CORE_MAX_NOF_CHANNELS (FLEXE_CORE_NOF_CHANNELS+2)

#define FLEXE_INVALID_GROUP_ID -1
#define FLEXE_INVALID_PORT_ID 0xF
#define FLEXE_INVALID_CHANNEL_ID 0x7F
#define FLEXE_INVALID_66SW_CHANNEL_ID 0xFF
#define FLEXE_INVALID_SAR_CHANNEL_ID 0x7F
#define FLEXE_INVALID_TINYMAC_CHANNEL_ID 0x7F
#define FLEXE_INVALID_ILKN_CHANNEL_ID 0x200

#define FLEXE_TINYMAC_OH_CHANNEL_ID 80
#define FLEXE_TINYMAC_1588_CHANNEL_ID 81

#define FLEXE_CORE_NOF_CALENDAR 2
#define FLEXE_66BSWITCH_NOF_SUBSYS 3

#define FLEXE_REG_ACCESS_MAX_RETRIES 100

#define FLEXE_MTU 9600
#define FLEXE_RATEADP_MAX_NOF_TIMESLOTS 16

#define FLEXE_OHIF_NOF_OAM_TYPES 64

#define FLEXE_PHY_SPEED_100G 100000

#define FLEXE_GROUP_IS_VALID(_grp) ((_grp >=0) && (_grp<FLEXE_CORE_NOF_PORTS))

#define FLEXE_PORT_SPEED_TO_RATE(_speed) \
    (_speed/FLEXE_PHY_SPEED_100G == 4)?FLEXE_DRV_PORT_SPEED_400G:(flexe_drv_port_speed_mode_e)(_speed/FLEXE_PHY_SPEED_100G)

#define FLEXE_MAX_LOGICAL_PHY_ID 0xFF

/**
 * \brief enum that represents FlexE env mode\n 
 */
typedef enum
{
    FLEXE_ENV_50G,
    FLEXE_ENV_100G,
    FLEXE_ENV_200G,
    FLEXE_ENV_400G,
    FLEXE_ENV_50G_BYPASS,
    FLEXE_ENV_100G_BYPASS,
    FLEXE_ENV_200G_BYPASS,
    FLEXE_ENV_400G_BYPASS,
    FLEXE_ENV_50G_LOCAL,
    FLEXE_ENV_100G_LOCAL,
    FLEXE_ENV_200G_LOCAL,
    FLEXE_ENV_400G_LOCAL,
    FLEXE_ENV_50G_INSTANCE_LOCAL,
} flexe_env_mode_e;

#define SOC_PORT_FLEXE_RX                (0x1)
#define SOC_PORT_FLEXE_TX                (0x2)

typedef enum
{
    FLEXE_DRV_SERDES_RATE_25G = 0,
    FLEXE_DRV_SERDES_RATE_26G,
    FLEXE_DRV_SERDES_RATE_53G,
    FLEXE_DRV_SERDES_RATE_COUNT,
} flexe_drv_serdes_rate_mode_e;


typedef enum
{
    FLEXE_DRV_PORT_SPEED_50G = 0,
    FLEXE_DRV_PORT_SPEED_100G,
    FLEXE_DRV_PORT_SPEED_200G,
    FLEXE_DRV_PORT_SPEED_400G,
    FLEXE_DRV_PORT_SPEED_COUNT
} flexe_drv_port_speed_mode_e;

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

#define FLEXE_PHY_NOF_INSTANCES(_speed) \
    MAX(_speed/FLEXE_PHY_SPEED_100G, 1)

#define FLEXE_PHY_NOF_LPHYS(_speed) \
    ((_speed)/5000)

/**
 * \brief enum that represents SAR loopback mode\n 
 */
typedef enum
{
    /*
     * External loopback, once a cell is received at the ILKN interface,
     * loops it back immediately.
     */
    FLEXE_DRV_SAR_LOOPBACK_REMOTE,
    /*
     * RX loopback, cells received at the ILKN interface, processed by
     * SAR RX logic, then looped back at the end of RX pipeline.
     */
    FLEXE_DRV_SAR_LOOPBACK_RX,
    /*
     * TX loopback, cells processed by SAR TX logic,
     * then looped back before transmitted out of the ILKN interface.
     */
    FLEXE_DRV_SAR_LOOPBACK_TX,
} flexe_drv_sar_loopback_mode_e;

/**
 * \brief enum that represents SAR cellsize mode\n 
 */
typedef enum
{
    /*
     * 28b mode: SOP - 15 66B blocks; EOP - 13 66B blocks;
     */
    FLEXE_DRV_SAR_CELLSIZE_28B,
    /*
     * 29b mode: SOP - 15 66B blocks; EOP - 14 66B blocks;
     */
    FLEXE_DRV_SAR_CELLSIZE_29B,
} flexe_drv_sar_cellsize_mode_e;

#ifdef BE_HOST
/* incompatible_cast coverity check should be disabled for this MACRO */
#define SHR_BITCOPY_RANGE_U32_TO_U64(_dest, _dest_offset,_src, _src_offset, _num_bits) \
    soc_bitop_range_copy_u32_to_u64_big_endian(_dest, _dest_offset,_src, _src_offset, _num_bits)
/* incompatible_cast coverity check should be disabled for this MACRO */
#define SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        soc_bitop_range_copy_u64_to_u32_big_endian(_dest, _dest_offset,_src, _src_offset, _num_bits)
#else
/* incompatible_cast coverity check should be disabled for this MACRO */
#define SHR_BITCOPY_RANGE_U32_TO_U64(_dest, _dest_offset,_src, _src_offset, _num_bits) \
    SHR_BITCOPY_RANGE((SHR_BITDCL *)_dest, _dest_offset,_src, _src_offset, _num_bits)
/* incompatible_cast coverity check should be disabled for this MACRO */
#define SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        SHR_BITCOPY_RANGE(_dest, _dest_offset,(SHR_BITDCL *)_src, _src_offset, _num_bits)
#endif

#define FLEXE_PHY_SEL_CFG_SET(_cfg, _idx, _val) \
    SHR_BITCOPY_RANGE(_cfg, ((_idx)*4), _val, 0, 4)

#define FLEXE_PHY_SEL_CFG_GET(_cfg, _idx, _val) \
    SHR_BITCOPY_RANGE(_val, 0, _cfg, ((_idx)*4), 4)

#define FLEXE_RX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port) \
    SHR_BITCOPY_RANGE(_map, ((_bcm_port)*4), _flexe_port, 0, 4)

#define FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    SHR_BITCOPY_RANGE(_flexe_port, 0, _map, ((_bcm_port)*4), 4)

#define FLEXE_RX_LF_SEL_SET(_lf_sel, _flexe_port, _bcm_port) \
    SHR_BITCOPY_RANGE(_lf_sel, ((_flexe_port)*3), _bcm_port, 0, 3)

#define FLEXE_RX_LF_SEL_GET(_lf_sel, _flexe_port, _bcm_port) \
        SHR_BITCOPY_RANGE(_bcm_port, 0, _lf_sel, ((_flexe_port)*3), 3)

#define FLEXE_RX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val) \
    SHR_BITCOPY_RANGE(_is_200g, _flexe_port/2, _val, 0, 1)

#define FLEXE_TX_MODE_IS_BYPASS_SET(_is_bypass, _flexe_port, _val) \
    SHR_BITCOPY_RANGE(_is_bypass, _flexe_port, _val, 0, 1)

#define FLEXE_TX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port) \
    FLEXE_RX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port)

#define FLEXE_TX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port)

#define FLEXE_TX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val) \
    FLEXE_RX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val)

#define FLEXE_CPB_EGR_BP_LEVEL_OFFSET 80
#define FLEXE_CPB_EGR_RD_LEVEL_BASE   56
#define FLEXE_CPB_EGR_HIGH_DEPTH_BASE 64
#define FLEXE_CPB_EGR_LOW_DEPTH_BASE  48

#define FSCL_NEXT_BLK_TABLE_DATA(_last, _blk, _next) (((_last) << 14) | ((_blk) << 7) | (_next))

#define FLEXE_DEMUX_REORDER_SRC_INVALID 0xF0
#define FLEXE_DEMUX_NOF_TIMESLOTS_PER_TABLE 8

#define FLEXE_DEMUX_RAM_INVALID_ROW 0xF
#define FLEXE_DEMUX_RDRULE_OFFSET(_channel, _timeslot) ((_channel) * FLEXE_CORE_NOF_TIMESLOTS + _timeslot)

#define FLEXE_DEMUX_GROUP_INFO_SET(_val, _phy, _min_phy) \
    SHR_BITCOPY_RANGE(_val, _phy*4, _min_phy, 0, 4)

#define FLEXE_DEMUX_GROUP_INFO_GET(_val, _phy, _min_phy) \
    SHR_BITCOPY_RANGE(_min_phy, 0, _val, _phy*4, 4)

/* incompatible_cast coverity check should be disabled for this MACRO */
#define FLEXE_DEMUX_REORDER_INFO_SET(_val, _lphy, _src) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_lphy)*8), _src, 0, 8)

/* incompatible_cast coverity check should be disabled for this MACRO */
#define FLEXE_DEMUX_REORDER_INFO_GET(_val, _lphy, _dest) \
    SHR_BITCOPY_RANGE_U64_TO_U32(_dest, 0, _val, ((_lphy)*8), 8)

/* incompatible_cast coverity check should be disabled for this MACRO */
#define FLEXE_DEMUX_GROUP_CFG_SET(_val, _lphy, _min_lphy) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_lphy)*4), _min_lphy, 0, 4)

/* incompatible_cast coverity check should be disabled for this MACRO */
#define FLEXE_DEMUX_GROUP_CFG_GET(_val, _lphy, _min_lphy) \
    SHR_BITCOPY_RANGE_U64_TO_U32(_min_lphy, 0, _val, ((_lphy)*4), 4)

#define FLEXE_DEMUX_CALENDAR_CFG_SET(_val, _timeslot, _chanel) \
    SHR_BITCOPY_RANGE(_val, ((_timeslot)*8), _chanel, 0, 8)

#define FLEXE_DEMUX_CALENDAR_CFG_GET(_val, _timeslot, _chanel) \
    SHR_BITCOPY_RANGE(_chanel, 0, _val, ((_timeslot)*8), 8)

#define FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_SET(_val, _chanel, _lphymap) \
    SHR_BITCOPY_RANGE(_val, ((_chanel)*8), _lphymap, 0, 8)

#define FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_GET(_val, _chanel, _lphymap) \
    SHR_BITCOPY_RANGE(_lphymap, 0, _val, ((_chanel)*8), 8)

#define FLEXE_DEMUX_RDRULE_SET(_val, _subcal, _timeslot) \
    SHR_BITCOPY_RANGE(_val, ((_subcal)*8), _timeslot, 0, 8)

#define FLEXE_DEMUX_RDRULE_GET(_val, _subcal, _timeslot) \
    SHR_BITCOPY_RANGE(_timeslot, 0, _val, ((_subcal)*8), 8)

#define FLEXE_DEMUX_TS_TO_WR_TBL_ID(_timeslot) \
    (((_timeslot)/40)*5 + (_timeslot) % 5)

#define FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(_timeslot) \
    ((((_timeslot) % 40)/5)*10)

#define FLEXE_MAX_COMMON_DIVISOR_10(_x) \
    ((_x % 10 == 0)?10:((_x % 5 == 0)?5:((_x % 2 == 0)?2:1)))

#define FLEXE_MULTI_DESKEW_GRP_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx%4)*8), _sel, 0, 8)

#define FLEXE_MULTI_DESKEW_GRP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx%4)*8), 8)

/**
 * \brief enum that represents FlexE MUX mode\n 
 */
typedef enum
{
    FLEXE_MUX_MODE_50G = 0,
    FLEXE_MUX_MODE_100G = 1,
    FLEXE_MUX_MODE_100G_2ND_PHY = 2,
} flexe_mux_mode_e;

#define FLEXE_MUX_GROUP_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_GROUP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx)*4), 4)

#define FLEXE_MUX_CHANNEL_BELONG_FLEXE_SET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _lphy, 0, 4)

#define FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, (_idx*4), 4)

#define FLEXE_MUX_PCS_SCH_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_PCS_SCH_CFG_GET(_reg_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _reg_val, ((_idx)*4), 4)

#define FLEXE_MUX_PCS_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*8), _ch, 0, 7)

#define FLEXE_MUX_MODE_SET(_val, _idx, _mode) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*2), _mode, 0, 2)

#define FLEXE_MUX_MODE_GET(_val, _idx, _mode) \
    SHR_BITCOPY_RANGE(_mode, 0, _val, ((_idx)*2), 2)

#define FLEXE_MUX_CPHY2_CFG_SET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _lphy, 0, 3)

#define FLEXE_MUX_CPHY2_CFG_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, ((_idx)*4), 3)

#define FLEXE_MUX_SCH_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_MSI_CFG_SET(_val, _idx, _channel) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _channel, 0, 7)

#define FLEXE_MUX_MSI_CFG_GET(_val, _idx, _channel) \
    SHR_BITCOPY_RANGE(_channel, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_TSNUM_CFG_SET(_val, _idx, _tsnum) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _tsnum, 0, 7)

#define FLEXE_MUX_TSNUM_CFG_GET(_val, _idx, _tsnum) \
    SHR_BITCOPY_RANGE(_tsnum, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_CYCLE_CFG_SET(_val, _idx, _cycle) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _cycle, 0, 7)

#define FLEXE_MUX_CYCLE_CFG_GET(_val, _idx, _cycle) \
    SHR_BITCOPY_RANGE(_cycle, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_BLK_CFG_SET(_val, _idx, _blk) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _blk, 0, 4)

#define FLEXE_MUX_BLK_CFG_GET(_val, _idx, _blk) \
    SHR_BITCOPY_RANGE(_blk, 0, _val, (_idx*4), 4)

#define FLEXE_MUX_EN_SEL_CFG(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _sel, 0, 3)

#define FLEXE_MUX_EN_SEL_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, (_idx*4), 3)

#define FLEXE_MUX_INST_SEL_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _sel, 0, 3)

#define FLEXE_MUX_INST_SEL_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, (_idx*4), 3)

/* incompatible_cast coverity check should be disabled for this MACRO */
#define FLEXE_CHANNELIZE_PCS_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_idx)*8), _ch, 0, 7)

#define FLEXE_66SW_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*8), _ch, 0, 7)

/**
 * \brief enum that represents calendar selection (A/B)\n 
 */
typedef enum
{
    /*
     * Calendar A;
     */
    FLEXE_DRV_CALENDAR_SEL_A,
    /*
     * Calendar A;
     */
    FLEXE_DRV_CALENDAR_SEL_B,
    /*
     * Calendar count;
     */
    FLEXE_DRV_CALENDAR_COUNT,
} flexe_drv_calendar_sel_e;

#define FLEXE_OP_A_OR_B(_sel, _a, _b, ...) \
    if (_sel == FLEXE_DRV_CALENDAR_SEL_A) \
    { \
        SHR_IF_ERR_EXIT(_a(__VA_ARGS__)); \
    } \
    else \
    { \
        SHR_IF_ERR_EXIT(_b(__VA_ARGS__)); \
    } \

uint32 soc_flexe_std_bitmap_find_first_set(
    SHR_BITDCL *bitmap,
    int nof_elements);

uint32 soc_flexe_std_bitmap_find_last_set(
    SHR_BITDCL *bitmap,
    int nof_elements);

/**
 * \brief
 *   Converts logical timeslot into hardware timeslot or verse versa
 *
 */
int
soc_flexe_mux_timeslot_convert(
    int timeslot,
    int is_100g);

/**
 * \brief
 *   Update the C bit for the instances in a group
 *
 *
 * [in] unit - unit ID
 * [in] instance_bitmap - FlexE instance bitmap
 * [in] c_bit - 0/1
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_mux_group_c_cfg_set(
    int unit,
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)],
    int c_bit);

/**
 * \brief
 *   Add a channel to rate adapter in MUX
 *
 *
 * [in] unit - unit ID
  *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_mux_rateadp_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Remove a channel from rate adapter in MUX
 *
 *
 * [in] unit - unit ID
  *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_mux_rateadp_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)]);

/**
 * \brief
 *   In big-endian system, copy from uint32 src to uint64 dest.
 *
 *
 * [in/out] u64_dest - uint64 destination
 * [in] dest_offset - destination offset
 * [in] src - uint32 source
 * [in] src_offset - source offset
 * [in] num_bits - number of bits to copy
 *
 * \return
 *
 * \remark
 *
 */
void soc_bitop_range_copy_u32_to_u64_big_endian(
    uint64 *u64_dest,
    int dest_offset,
    uint32 *src,
    int src_offset,
    int num_bits);

/**
 * \brief
 *   In big-endian system, copy from uint64 src to uint32 dest.
 *
 *
 * [in/out] dest - uint32 destination
 * [in] dest_offset - destination offset
 * [in] u64_src - uint64 source
 * [in] src_offset - source offset
 * [in] num_bits - number of bits to copy
 *
 * \return
 *
 * \remark
 *
 */
void soc_bitop_range_copy_u64_to_u32_big_endian(
    uint32 *dest,
    int dest_offset,
    uint64 *u64_src,
    int src_offset,
    int num_bits);

/**
 * \brief
 *   FlexE init
 *
 *
 * [in] unit - unit ID
  *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *    task flexe_core_cfg::EnvRun ();
 */
shr_error_e soc_flexe_std_init(
    int unit);

/**
 * \brief
 *   Add PHY to Flexe Core in non-bypass mode [RX direction].
 *   For bypass mode, call soc_flexe_nd_rx_phy_bypass_set.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] flexe_port - FlexE port that BCM port maps to
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 * [in] serdes_rate - rate of the SerDes
 *                    25G: 0
 *                    26G: 1
 *                    53G: 2
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_rx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate);

/**
 * \brief
 *   Delete PHY from Flexe Core in non-bypass mode [RX direction].
 *   For bypass mode, call soc_flexe_rx_phy_bypass_set with enable=0.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_rx_phy_del(
    int unit,
    int bcm_port,
    int speed);

/**
 * \brief
 *   Add PHY to Flexe Core in non-bypass mode [TX direction].
 *   For bypass mode, call soc_flexe_nd_tx_phy_bypass_set.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] flexe_port - FlexE port that BCM port maps to
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_tx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed);

/**
 * \brief
 *   Delete PHY from Flexe Core in non-bypass mode [TX direction].
 *   For bypass mode, call soc_flexe_nd_tx_phy_bypass_set.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_tx_phy_del(
    int unit,
    int bcm_port,
    int speed);

/**
 * \brief
 *   Add PHY to Flexe Core in bypass mode [RX direction].
 *   For non-bypass mode, call soc_flexe_nd_rx_phy_add.
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID
 * [in] enable - bypass mode enabled or not
 * [in] bypass_channel - channel ID used for this PHY in bypass mode
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_rx_phy_bypass_set(
    int unit,
    int flexe_port,
    int enable,
    int bypass_channel);

/**
 * \brief
 *   Add PHY to Flexe Core in bypass mode [TX direction].
 *   For non-bypass mode, call soc_flexe_nd_tx_phy_add.
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 * [in] enable - bypass mode enabled or not
 * [in] bypass_channel - bypass channel ID for this PHY
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_tx_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel);

/**
 * \brief
 *   Add PHY to Flexe Core in non-bypass mode.
 *   For bypass mode, call soc_flexe_nd_phy_bypass_set.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] flexe_port - FlexE port ID that BCM port maps to
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 * [in] serdes_rate - rate of the SerDes
 *                    25G: 0
 *                    26G: 1
 *                    53G: 2
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate);

/**
 * \brief
 *   Remove PHY from Flexe Core in non-bypass mode.
 *   For bypass mode, call soc_flexe_nd_phy_bypass_set.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_phy_del(
    int unit,
    int bcm_port,
    int speed);

/**
 * \brief
 *   Add PHY to Flexe Core in bypass mode.
 *   For bypass mode, call soc_flexe_nd_phy_add.
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID that BCM port maps to
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 * [in] enable - bypass mode enabled or not
 * [in] bypass_channel - bypass channel ID for this PHY
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel);

/**
 * \brief
 *   SAR subsys init [RX direction].
 *
 * [in] unit - unit ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_rx_init(
    int unit);

/**
 * \brief
 *   SAR subsys init [TX direction].
 *
 * [in] unit - unit ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_tx_init(
    int unit);

/**
 * \brief
 *   Initialize SAR subsys.
 *
 * [in] unit - unit ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_init(
    int unit);

/**
 * \brief
 *   Add SAR channel [RX direction].
 *       This adds a SAR channel, allocates timeslots for it,
 *       and maps it to ILKN channel which has the same channel ID.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Delete SAR channel [RX direction].
 *       This removes a SAR channel, and frees any timeslots allocated for it.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Add SAR channel [TX direction].
 *       This adds a SAR channel, and allocates timeslots for it.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Delete SAR channel [TX direction].
 *       This removes a SAR channel, and frees any timeslots allocated for it.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *              This is the tsmap used when adding this channel.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Add SAR channel.
 *       This adds a SAR channel, and allocates timeslots for it.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Delete SAR channel.
 *       This removes a SAR channel, and frees any timeslot allocated for it.
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 *              This is the tsmap used when adding this channel.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Map SAR channel to ILKN channel (RX direction, ILKN->SAR).
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] ilkn_channel - ILKN channel ID (range: 0~255)
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 */
shr_error_e soc_flexe_std_sar_channel_map_rx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable);

/**
 * \brief
 *   Map SAR channel to ILKN channel (TX direction, SAR->ILKN).
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] ilkn_channel - ILKN channel ID (range: 0~255)
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 */
shr_error_e soc_flexe_std_sar_channel_map_tx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable);

/**
 * \brief
 *   Map SAR channel to ILKN channel.
 *
 * [in] unit - unit ID
 * [in] flags - SOC_PORT_FLEXE_RX or SOC_PORT_FLEXE_TX
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] ilkn_channel - ILKN channel ID (range: 0~255)
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 */
shr_error_e soc_flexe_std_sar_channel_map_set(
    int unit,
    uint32 flags,
    int channel,
    int ilkn_channel,
    int enable);

/**
 * \brief
 *   SAR loopback configuration.
 *   For FLEXE_SAR_LOOPBACK_RX/FLEXE_SAR_LOOPBACK_TX:
 *       Need to setup the SAR channel as well (see also soc_flexe_nd_sar_channel_add).
 *
 * [in] unit - unit ID
 * [in] mode - flexe_drv_sar_loopback_mode_e
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_loopback_set(
    int unit,
    flexe_drv_sar_loopback_mode_e mode,
    int enable);

/**
 * \brief
 *   SAR cellsize mode configuration.
 *
 * [in] unit - unit ID
 * [in] mode - see also flexe_drv_sar_cellsize_mode_e
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_cellsize_mode_set(
    int unit,
    flexe_drv_sar_cellsize_mode_e mode);

/**
 * \brief
 *   FlexE ENV configuration for inf_ch_adp_tx.
 *   This configures various FIFO adjusts for interface chan adapter subsys.
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID
 * [in] phy_rate - rate of the phy
 *                    50G: 0
 *                    100G: 1
 *                    200G: 2
 *                    400G: 3
 * [in] bypass - bypass mode, 0: disabled; 1: enabled
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_env_set(
    int unit,
    int flexe_port,
    int phy_rate,
    int bypass);

/**
 * \brief
 *   Enable/disable calendar for TinyMAC sybsys (bus b).
 *
 * [in] unit - unit ID
 * [in] enable - 0: disabled; 1: enabled
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_tmac_calendar_enable_set(
    int unit,
    int enable);

/**
 * \brief
 *   Add group to FlexE subsys.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphys - LPHYs allocated in order for those ports in the group. Each LPHY is 50G.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_rx_phy_add before adding a phy to a FlexE group
 * \see
 *
 */
shr_error_e soc_flexe_std_group_rx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);

/**
 * \brief
 *   Add group to bus A.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphy_bitmap - bitmap of the lphys allocated for this group. Each bit represents a 50G logical PHY.
 *                    Order is not important when removing so the LPHYs are given as a bitmap.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_rx_phy_del after removing a phy from a FlexE group
 * \see
 *
 */
shr_error_e soc_flexe_std_group_rx_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);

/**
 * \brief
 *   Add group to bus A.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphys - LPHYs allocated in order for those ports in the group. Each LPHY is 50G.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
  *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_tx_phy_add before adding a phy to a FlexE group
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_group_tx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);

/**
 * \brief
 *   Add group to bus A.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphy_bitmap - bitmap of the lphys allocated for this group. Each bit represents a 50G logical PHY.
 *                    Order is not important when removing so the LPHYs are given as a bitmap.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_tx_phy_del after removing a phy from a FlexE group
 * \see
 *
 */
shr_error_e soc_flexe_std_group_tx_remove(
    int unit,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);

/**
 * \brief
 *   Add group to bus A.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphys - LPHYs allocated in order for those ports in the group. Each LPHY is 50G.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_phy_add before adding a phy to a FlexE group
 * \see
 *
 */
shr_error_e soc_flexe_std_group_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);

/**
 * \brief
 *   Add group to bus A.
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port
 * [in] lphy_bitmap - bitmap of the lphys allocated for this group. Each bit represents a 50G logical PHY.
 *                    Order is not important when removing so the LPHYs are given as a bitmap.
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *    Call soc_flexe_nd_phy_del after removing a phy from a FlexE group
 * \see
 *
 */
shr_error_e soc_flexe_std_group_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);

/**
 * \brief
 *   Add channel in FlexE subsys.
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Remove a channel from FlexE subsys.
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *              This is the tsmap used when adding this channel.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Add a channel in FlexE subsys [RX direction].
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] calendar_sel - Which calendar to add this channel
 *                    0: calendar A
 *                    1: calendar B
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Remove a channel from FlexE subsys [RX direction].
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *              This is the tsmap used when adding this channel.
 * [in] calendar_sel - Which calendar to remove this channel from
 *                    0: calendar A
 *                    1: calendar B
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Add channel in FlexE subsys [TX direction].
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] calendar_sel - Which calendar to add this channel
 *                    0: calendar A
 *                    1: calendar B
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Remove a channel from FlexE subsys [TX direction].
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *              This is the tsmap used when adding this channel.
 * [in] calendar_sel - Which calendar to remove this channel from
 *                    0: calendar A
 *                    1: calendar B
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);

/**
 * \brief
 *   Add channel in FlexE subsys in bypass mode.
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] nof_timeslots - Number of timeslots reserved for this (bypass) PHY, in units of 5G.
 * [in] bypass - 0: disable; 1: enable.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_channel_bypass_set(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int enable);

/**
 * \brief
 *   Allocates a channel for TinyMAC.
 *
 * [in] unit - unit ID
 * [in] channel - TinyMAC channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_client_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Frees a channel in TinyMAC.
 *
 * [in] unit - unit ID
 * [in] channel - TinyMAC channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 *              This is the tsmap used when adding this channel.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *    task flexe_core_cfg::DelDutBusBCli(int cliId, int tsNum);
 */
shr_error_e soc_flexe_std_client_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)]);

/**
 * \brief
 *   Add entry to 66b switch table.
 *
 *
 * [in] unit - unit ID
 * [in] src_channel - source channel. This is where the cells come from.
 *              0~79: FlexE channel
 *              80~159: SAR channel
 *              160~241: TinyMAC channel
 * [in] dest_channel - destination channel. This is where the cells routed to.
 *
 * [in] index - index of the destinations to be added. Each cell can be copied to 3 destinations,
 *              indexed as 0/1/2. Among those destinations, two of them can be forwarded to the same sub-system.
 *              E.g., 0/1, 0/2, 1/2 can be forwarded to either FlexE/SAR/TinyMAC sub-system, but not all the 3 of them.
 *
 * [in] enable - 1: add switch entry; 0: remove switch entry.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_66bsw_set(
    int unit,
    int src_channel,
    int dest_channel,
    int index,
    int enable);

/**
 * \brief
 *   FlexE overhead loopback configuration.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID.
 *                    400G: 400G PHY has 4 instances, which are 0/1/2/3
 *                    200G: 200G PHY has 2 instances, which are phy_id/phy_id+1 (0/1, 4/5)
 *                    100G: 100G PHY has 1 instance, which is the phy_id (0/2/4/6)
 *                    100G/50G: 100G PHY has 1 instance, which is the phy_id (0/1/2/3/4/5/6/7)
 *
 * [in] enable - 0: disable; 1: enable;
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_loopback_set(
    int unit,
    int instance,
    int enable);

/**
 * \brief
 *   FlexE overhead source selection configuration.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_config(
    int unit,
    int instance);

/**
 * \brief
 *   Set default configuration for FlexE port.
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - FlexE port ID from FlexEWP(0~7)
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_port_default_config(
    int unit,
    int bcm_port,
    int speed);

/**
 * \brief
 *   Configure logical PHY ID for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] logical_phy_id - logical PHY ID (0~255)
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_logical_phy_id_set(
    int unit,
    int instance,
    int logical_phy_id);

/**
 * \brief
 *   Configure logical PHY ID for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] flags - RX: 1; TX: 2
 * [out] logical_phy_id - logical PHY ID (0~255)
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_logical_phy_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *logical_phy_id);

/**
 * \brief
 *   Configure group ID for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] group_id - FlexE group ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_group_id_set(
    int unit,
    int instance,
    int group_id);

/**
 * \brief
 *   Get group ID for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] flags - RX: 1; TX: 2
 * [out] group_id - FlexE group ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_group_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *group_id);

/**
 * \brief
 *   Set client ID for a timeslot in a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] cal_id - Calendar 0 or 1
 * [in] timeslot - timeslot inside the instance
 * [in] client_id - FlexE client ID that takes this timeslot
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_client_id_set(
    int unit,
    int instance,
    int cal_id,
    int timeslot,
    int client_id);

/**
 * \brief
 *   Set client carried in in FlexE instance overhead.
 *
 *
 * [in] unit - unit ID
 * [in] cal_id - calendar A or B
 * [in] nof_instances - number of instances in instance_array
 * [in] instance_array - FlexE instances array (0~7)
 * [in] nof_slots - maximum numboer of slots in calendar_slots.
 * [out] calendar_slots - calendar slots array which carries client ID.
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_client_id_get(
    int unit,
    int flags,
    int instance,
    int cal_id,
    int timeslot,
    int *client_id);

/**
 * \brief
 *   Set/clear "count" number of consequent logical PHY ID for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] logical_phy_id - logical PHY ID
 * [in] count - number of consequent logical PHY IDs to update (<=4)
 * [in] clr - 0: set thos bits corresponding to logical PHY ID; 1: clear those bits correspding to logical PHY ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_phymap_update(
    int unit,
    int instance,
    int logical_phy_id,
    int count,
    int clr);

/**
 * \brief
 *   Clear all logical PHY IDs for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_phymap_clear(
    int unit,
    int instance);

/**
 * \brief
 *   Set CR bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_cr_bit_set(
    int unit,
    int instance,
    int cal_id);

/**
 * \brief
 *   Get CR bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_cr_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);

/**
 * \brief
 *   Set CA bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_ca_bit_set(
    int unit,
    int instance,
    int cal_id);

/**
 * \brief
 *   Get CA bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [out] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_ca_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);

/**
 * \brief
 *   Set C bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_c_bit_set(
    int unit,
    int instance,
    int cal_id);

/**
 * \brief
 *   Get C bit for a FlexE instance.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID (0~7)
 * [in] flags - 1: RX; 2: TX
 * [out] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_c_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);

/**
 * \brief
 *   Set active calendar for a number of FlexE instances.
 *
 *
 * [in] unit - unit ID
 * [in] instance_bitmap - bitmap of FlexE instances to be set
 * [in] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_active_calendar_set(
    int unit,
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)],
    int cal_id);

/**
 * \brief
 *   Get active calendar for a number of FlexE instances.
 *
 *
 * [in] unit - unit ID
 * [in] instance_bitmap - bitmap of FlexE instances to be set
 * [in] flags - 1: RX; 2: TX
 * [out] cal_id - Calendar ID
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_active_calendar_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);

/**
 * \brief
 *   Get OH alarm status for a FlexE instances.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID
 * [out] alarm_status - alarm status
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_alarm_status_get(
    int unit,
    int instance,
    uint16 *alarm_status);

/**
 * \brief
 *   Get OH calendar mismatch status for a FlexE instances.
 *
 *
 * [in] unit - unit ID
 * [in] instance - FlexE instance ID
 * [out] time_slots - bitmap of the timeslts that are reported with calendar mismatch
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oh_cal_mismatch_ts_get(
    int unit,
    int instance,
    SHR_BITDCL * time_slots);

/**
 * \brief
 *   Get OAM alarm status for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [out] alarms_status - bitmap of alarms reported
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status);

/**
 * \brief
 *   Get OAM alarm status for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] alarm_type - alarm type
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable);

/**
 * \brief
 *   Set OAM BAS period for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [in] base_period - BAS period
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period);

/**
 * \brief
 *   Get OAM BAS period for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [out] base_period - BAS period
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period);

/**
 * \brief
 *   Configure OAM insert enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable);

/**
 * \brief
 *   Get OAM insert enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [out] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable);

/**
 * \brief
 *   Configure OAM bypass enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable);

/**
 * \brief
 *   Get OAM bypass enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [out] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable);

/**
 * \brief
 *   Configure SAR OAM bypass enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable);

/**
 * \brief
 *   Get SAR OAM bypass enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] flags - 1: RX; 2: TX
 * [out] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_sar_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable);

/**
 * \brief
 *   Configure OAM local fault enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable);

/**
 * \brief
 *   Get OAM local fault enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [out] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable);

/**
 * \brief
 *   Configure OAM remote fault enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [in] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable);

/**
 * \brief
 *   Get OAM remote fault enable for a FlexE client.
 *
 *
 * [in] unit - unit ID
 * [in] client_channel - FlexE client ID
 * [out] enable - 0: disable; 1: enable
 *
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e soc_flexe_std_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable);

/**
 * \brief - Configure SD BIP8 alarm block number
 */
shr_error_e soc_flexe_std_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);

/**
 * \brief - Get SD BIP8 alarm block number
 */
shr_error_e soc_flexe_std_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);

/**
 * \brief - Configure SD BIP8 error set threshold
 */
shr_error_e soc_flexe_std_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SD BIP8 error set threshold
 */
shr_error_e soc_flexe_std_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SD BIP8 error clear threshold
 */
shr_error_e soc_flexe_std_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SD BIP8 error clear threshold
 */
shr_error_e soc_flexe_std_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SD BEI alarm block number
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);

/**
 * \brief - Get SD BEI alarm block number
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);

/**
 * \brief - Configure SD BEI error set threshold
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SD BEI error set threshold
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SD BEI error clear threshold
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SD BEI error clear threshold
 */
shr_error_e soc_flexe_std_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SF BIP8 alarm block number
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);

/**
 * \brief - Get SF BIP8 alarm block number
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);

/**
 * \brief - Configure SF BIP8 error set threshold
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SF BIP8 error set threshold
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SF BIP8 error clear threshold
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SF BIP8 error clear threshold
 */
shr_error_e soc_flexe_std_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SF BEI alarm block number
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);

/**
 * \brief - Get SF BEI alarm block number
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);

/**
 * \brief - Configure SF BEI error set threshold
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SF BEI error set threshold
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Configure SF BEI error clear threshold
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);

/**
 * \brief - Get SF BEI error clear threshold
 */
shr_error_e soc_flexe_std_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);

/**
 * \brief - Get OAM BIP8 counter
 */
shr_error_e soc_flexe_std_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val);

/**
 * \brief - Get OAM BEI counter
 */
shr_error_e soc_flexe_std_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val);

/**
 * \brief - Get OAM packet
 */
shr_error_e soc_flexe_std_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val);

/**
 * \brief - Get OAM base packet
 */
shr_error_e soc_flexe_std_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val);

#endif
