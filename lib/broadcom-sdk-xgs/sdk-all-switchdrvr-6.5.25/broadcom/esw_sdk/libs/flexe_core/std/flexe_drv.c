/** \file flexe_drv.c
 *
 * Functions for FlexE core low level drivers
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLEXE

/*************
 * INCLUDES  *
 *************/
/* { */

#include <sal/core/thread.h>

#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <flexe_drv.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>

#include <bcm_int/dnx/port/port_flexe_reg_access.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 * Global struct to store FlexE core port info
 */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

void
soc_bitop_range_copy_u32_to_u64_big_endian(
    uint64 *u64_dest,
    int dest_offset,
    uint32 *src,
    int src_offset,
    int num_bits)
{
    uint32 buf[2];

    buf[0] = COMPILER_64_LO(*u64_dest);
    buf[1] = COMPILER_64_HI(*u64_dest);

    SHR_BITCOPY_RANGE(buf, dest_offset, src, src_offset, num_bits);

    COMPILER_64_SET(*u64_dest, buf[1], buf[0]);
}

void
soc_bitop_range_copy_u64_to_u32_big_endian(
    uint32 *dest,
    int dest_offset,
    uint64 *u64_src,
    int src_offset,
    int num_bits)
{
    uint32 buf[2];

    buf[0] = COMPILER_64_LO(*u64_src);
    buf[1] = COMPILER_64_HI(*u64_src);

    SHR_BITCOPY_RANGE(dest, dest_offset, buf, src_offset, num_bits);
}

uint32
soc_flexe_std_minimum_lphy_calculate(
    uint8 lphys[FLEXE_CORE_NOF_LPHYS])
{
    uint32 min_lphy = lphys[0];
    int ii;

    for (ii=1; ii<FLEXE_CORE_NOF_LPHYS; ++ii)
    {
        if (lphys[ii] < min_lphy)
        {
            min_lphy = lphys[ii];
        }
    }

    return min_lphy;
}

uint32
soc_flexe_std_bitmap_find_first_set(
    SHR_BITDCL *bitmap,
    int nof_elements)
{
    int ii;

    SHR_BIT_ITER(bitmap, nof_elements, ii)
    {
        break;
    }


    return ii;
}

uint32
soc_flexe_std_bitmap_find_last_set(
SHR_BITDCL *bitmap,
int nof_elements)
{
    int ii;
    int rv = nof_elements;

    SHR_BIT_ITER(bitmap, nof_elements, ii)
    {
        rv= ii;
    }


    return rv;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_soft_reset(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Pull down reset
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_RST_GLB_LOGIC_Nr(unit, 0));

    sal_usleep(1);

    /*
     * Pull up reset
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_RST_GLB_LOGIC_Nr(unit, 1));

    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure environment for FlexE port
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - This is the FlexE port mapped from BCM port.
 * [in] mode - TBD
 * [in] enable - 0: port is disabled; 1: port is enabled
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
STATIC shr_error_e
_soc_flexe_env_cfg(
    int unit,
    int flexe_port,
    flexe_env_mode_e mode,
    int enable)
{
    uint32 reg_val = 0;
    uint32 cfg_period = 0;
    uint32 multiplier = 0;
    uint32 nom_cnt = 0;
    uint32 local_m = 0;
    uint32 m_cfg = 0;
    uint32 base_m_cfg = 0;
    uint32 is_local = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, FLEXE_ENV_Qf, 32);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, ADJ_01_ENf, 0);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, FIFO_ADJ_ENf, 1);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, FIFO_ADJ_PERIODf, 0);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, FIFO_ADJ_MAX_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_3r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_3r, &reg_val, ADJ_IN_CHECK_VALUEf, 100);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_3r, &reg_val, ADJ_IN_CORRECT_ENf, 1);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_3r, &reg_val, LOCAL_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_3r(unit, flexe_port, reg_val));

        /*
         * Set rate ADJ according to mode
         */
        switch (mode)
        {
            case FLEXE_ENV_50G:
                cfg_period = 10767;
                multiplier = 1;
                nom_cnt = 2028;
                local_m = 499;
                m_cfg = 0xC7FCE000;
                base_m_cfg = 0xCE3B5F42;
                is_local = 0;
                break;
            case FLEXE_ENV_100G:
                cfg_period = 10767;
                multiplier = 0;
                nom_cnt = 2028;
                local_m = 499;
                m_cfg = 0xCCFD4000;
                base_m_cfg = 0xCCFD8B2F;
                is_local = 0;
                break;
            case FLEXE_ENV_200G:
                cfg_period = 10767;
                multiplier = 0;
                nom_cnt = 2028;
                local_m = 499;
                m_cfg = 0x63FE7000;
                base_m_cfg = 0x6A3CEF42;
                is_local = 0;
                break;
            case FLEXE_ENV_400G:
                cfg_period = 10767;
                multiplier = 0;
                nom_cnt = 2028;
                local_m = 499;
                m_cfg = 0x31FF3800;
                base_m_cfg = 0x383DB742;
                is_local = 0;
                break;
            case FLEXE_ENV_50G_BYPASS:
                cfg_period = 10097;
                multiplier = 1;
                nom_cnt = 1900;
                local_m = 531;
                m_cfg = 0xCCCCCCCC;
                base_m_cfg = 0xCCCCCCCD;
                is_local = 0;
                break;
            case FLEXE_ENV_100G_BYPASS:
                cfg_period = 5014;
                multiplier = 1;
                nom_cnt = 1887;
                local_m = 521;
                m_cfg = 0xCCCCCCCC;
                base_m_cfg = 0xCCCCCCCD;
                is_local = 0;
                break;
            case FLEXE_ENV_200G_BYPASS:
                cfg_period = 2511;
                multiplier = 1;
                nom_cnt = 1890;
                local_m = 556;
                m_cfg = 0xCCCCCCCC;
                base_m_cfg = 0xCCCCCCCD;
                is_local = 0;
                break;
            case FLEXE_ENV_400G_BYPASS:
                cfg_period = 2511;
                multiplier = 0;
                nom_cnt = 1890;
                local_m = 556;
                m_cfg = 0xCCCCCCCC;
                base_m_cfg = 0xCCCCCCCD;
                is_local = 0;
                break;
            case FLEXE_ENV_50G_LOCAL:
                cfg_period = 10097;
                multiplier = 0;
                nom_cnt = 1900;
                local_m = 531;
                m_cfg = 0x302F55CD;
                base_m_cfg = 0x3030AACD;
                is_local = 1;
                break;
            case FLEXE_ENV_100G_LOCAL:
                cfg_period = 5014;
                multiplier = 0;
                nom_cnt = 1887;
                local_m = 521;
                m_cfg = 0x605EBE7F;
                base_m_cfg = 0x605F147F;
                is_local = 1;
                break;
            case FLEXE_ENV_200G_LOCAL:
                cfg_period = 2511;
                multiplier = 0;
                nom_cnt = 1890;
                local_m = 556;
                m_cfg = 0xC0BE4731;
                base_m_cfg = 0xC0BE5D71;
                is_local = 1;
                break;
            case FLEXE_ENV_400G_LOCAL:
                cfg_period = 2511;
                multiplier = 0;
                nom_cnt = 1890;
                local_m = 556;
                m_cfg = 0xC0BE4731;
                base_m_cfg = 0xC0BE5D71;
                is_local = 1;
                break;
            case FLEXE_ENV_50G_INSTANCE_LOCAL:
                cfg_period = 10767;
                multiplier = 0;
                nom_cnt = 2028;
                local_m = 499;
                m_cfg = 0x303A3F14;
                base_m_cfg = 0x303DA985;
                is_local = 1;
                break;
            default:
                break;;
        }

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_1r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_1r, &reg_val, CFG_PERIODf, cfg_period);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_1r, &reg_val, MULTIPLIERf, multiplier);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_1r, &reg_val, NOM_CNTf, nom_cnt);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_1r(unit, flexe_port, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_3r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_3r, &reg_val, LOCAL_Mf, local_m);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_3r(unit, flexe_port, reg_val));

        SHR_IF_ERR_EXIT(WRITE_FSCL_M_CFGr(unit, flexe_port, m_cfg));
        SHR_IF_ERR_EXIT(WRITE_FSCL_BASE_M_CFGr(unit, flexe_port, base_m_cfg));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, IS_LOCALf, is_local);
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, ENV_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, reg_val));
    }
    else
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, ENV_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_2r(unit, flexe_port, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Init FlexE IF adapter (bus A) [RX direction]
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
STATIC shr_error_e
_soc_flexe_interface_rx_init(
    int unit)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Maps all BCM ports to invalid FlexE port (0xF).
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_RX_MAP_TABLEr(unit, 0xFFFFFFFF));

    SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_400Gf, 0);
    soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

    /*
     * LF_SEL (8x3bits):
     *     0 - 0
     *     1 - 1
     *     ...
     *     7 - 7
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, 0xfac688));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add a PHY in FlexE IF adapter (bus A) [RX direction]
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] flexe_port - FlexE port that bcm_port maps to
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
STATIC shr_error_e
_soc_flexe_interface_rx_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    uint32 ts_delta = 0;
    uint32 is_200g = 0;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);
    /*
     * Maps BCM port to FlexE port (FlexE port = BCM port) .
     */
    SHR_IF_ERR_EXIT(soc_flexe_std_rx_channel_map_table_set(unit, bcm_port, flexe_port));
    switch (rate)
    {
        case FLEXE_DRV_PORT_SPEED_50G:
            /*
             * Clear mode 200G
             */
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            is_200g = soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_200Gf);
            val = 0;
            FLEXE_RX_MODE_IS_200G_SET(&is_200g, flexe_port, &val);
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

            /*
             * RX LF select (after mapping)
             */
            SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &reg_val));
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port, (SHR_BITDCL *) & bcm_port);
            SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, reg_val));

            switch (serdes_rate)
            {
                case FLEXE_DRV_SERDES_RATE_25G:
                    ts_delta = 20;
                    break;
                case FLEXE_DRV_SERDES_RATE_26G:
                    ts_delta = 19;
                    break;
                case FLEXE_DRV_SERDES_RATE_53G:
                    ts_delta = 19;
                    break;
                default:
                    break;
            }

            /*
             * TS delta configuration. Index is FlexE port after mapping.
             */
            reg_val = ts_delta;
            reg_val <<= 16;
            reg_val |= ts_delta;
            SHR_IF_ERR_EXIT(WRITE_FSCL_TS_DELTAr(unit, flexe_port, reg_val));
            break;
        case FLEXE_DRV_PORT_SPEED_100G:
            /*
             * Clear mode 200G
             */
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            is_200g = soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_200Gf);
            val = 0;
            FLEXE_RX_MODE_IS_200G_SET(&is_200g, flexe_port, &val);
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

            /*
             * RX LF select (after mapping)
             */
            SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &reg_val));
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port, (SHR_BITDCL *) & bcm_port);
            SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, reg_val));

            /*
             * TS delta configuration. Index is FlexE port after mapping.
             */
            ts_delta = 10;
            reg_val = ts_delta;
            reg_val <<= 16;
            reg_val |= ts_delta;
            SHR_IF_ERR_EXIT(WRITE_FSCL_TS_DELTAr(unit, flexe_port, reg_val));
            break;
        case FLEXE_DRV_PORT_SPEED_200G:
            /*
             * Mode is 200G (after mapping).
             */
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            is_200g = soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_200Gf);
            val = 1;
            FLEXE_RX_MODE_IS_200G_SET(&is_200g, flexe_port, &val);
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

            /*
             * RX LF select (after mapping)
             */
            SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &reg_val));
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port, (SHR_BITDCL *) & bcm_port);
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port + 1, (SHR_BITDCL *) & bcm_port);
            SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, reg_val));

            /*
             * TS delta configuration. Index is FlexE port after mapping.
             */
            ts_delta = 5;
            reg_val = ts_delta;
            reg_val <<= 16;
            reg_val |= ts_delta;
            SHR_IF_ERR_EXIT(WRITE_FSCL_TS_DELTAr(unit, flexe_port, reg_val));

            /*
             * REMARKS: Asymmetric mode * When to enable asymmetric mode? * Registers:
             * FSCL_MODE_CFGr.IS_ASYM/IS_PORT6/ASYM_INST_SEL 
             */
            break;
        case FLEXE_DRV_PORT_SPEED_400G:
            /*
             * Clear mode 200G
             */
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            is_200g = soc_reg_field_get(unit, FSCL_MODE_CFGr, reg_val, IS_200Gf);
            val = 0;
            FLEXE_RX_MODE_IS_200G_SET(&is_200g, flexe_port, &val);
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

            /*
             * Mode is 200G (after mapping).
             */
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_400Gf, 1);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));

            /*
             * RX LF select (after mapping)
             */
            SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &reg_val));
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port, (SHR_BITDCL *) & bcm_port);
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port + 1, (SHR_BITDCL *) & bcm_port);
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port + 2, (SHR_BITDCL *) & bcm_port);
            FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port + 3, (SHR_BITDCL *) & bcm_port);
            SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, reg_val));

            /*
             * TS delta configuration. Index is FlexE port after mapping.
             */
            ts_delta = 2;
            reg_val = ts_delta;
            reg_val <<= 16;
            reg_val |= ts_delta;
            SHR_IF_ERR_EXIT(WRITE_FSCL_TS_DELTAr(unit, flexe_port, reg_val));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Remove a PHY from FlexE IF adapter (bus A) [RX direction]
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
STATIC shr_error_e
_soc_flexe_interface_rx_del(
    int unit,
    int bcm_port,
    int speed)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    uint32 flexe_port = 0;
    uint32 val;
    int nof_instances;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_RX_MAP_TABLEr(unit, &reg_val));

    /*
     * Save FlexE port BCM port mapped to
     */
    FLEXE_RX_MAP_TABLE_GET(&reg_val, bcm_port, &flexe_port);
    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);

    switch (rate)
    {
        case FLEXE_DRV_PORT_SPEED_400G:
            SHR_IF_ERR_EXIT(READ_FSCL_MODE_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_400Gf, 0);
            SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_CFGr(unit, reg_val));
            break;
        default:
            break;
    }

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    SHR_IF_ERR_EXIT(READ_FSCL_LF_SELr(unit, &reg_val));

    for (index = 0; index < nof_instances; ++index)
    {
        val = flexe_port + index;

        /*
         * Restore RX LF selection
         */
        FLEXE_RX_LF_SEL_SET(&reg_val, flexe_port + index, (SHR_BITDCL *) & val);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_LF_SELr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init FlexE IF adapter (bus A) [TX direction]
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
STATIC shr_error_e
_soc_flexe_interface_tx_init(
    int unit)
{
    uint32 reg_val = 0;
    uint32 data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MAP_TABLEr(unit, 0xFFFFFFFF));

    SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_400Gf, 0);
    soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_200Gf, 0);
    soc_reg_field_set(unit, FSCL_MODE_CFGr, &reg_val, IS_ASYMf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

    data = FLEXE_INVALID_PORT_ID;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_MAP_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS-1, &data));

    SHR_IF_ERR_EXIT(WRITE_FSCL_MAP_ENr(unit, 1));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_IS_400Gf, 0);
    soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_400G_CHf, 0x7F);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXTERNAL_CFGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add a PHY in FlexE IF adapter (bus A) [TX direction]
 *
 *
 * [in] unit - unit ID
 * [in] bcm_port - BCM port ID from NB
 *                    400G: 0
 *                    200G: 0/4
 *                    100G: 0/2/4/6
 *                    50G: 0/1/2/3/4/5/6/7
 * [in] flexe_port - FlexE port that bcm_port maps to
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
STATIC shr_error_e
_soc_flexe_interface_tx_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    uint32 is_200g = 0;
    uint32 start_level = 0;
    uint32 high_level = 0;
    uint32 low_level = 0;
    uint32 up_level = 0;
    uint32 down_level = 0;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);
    /*
     * Map BCM port to local port
     */
    SHR_IF_ERR_EXIT(soc_flexe_std_tx_channel_map_table_set(unit, bcm_port, flexe_port));
    /*
     * Set FIFO level accoring to rate
     */
    switch (rate)
    {
        case FLEXE_DRV_PORT_SPEED_50G:
            start_level = 12;
            high_level = 16;
            low_level = 8;
            up_level = 21;
            down_level = 3;
            break;
        case FLEXE_DRV_PORT_SPEED_100G:
            start_level = 14;
            high_level = 18;
            low_level = 10;
            up_level = 25;
            down_level = 3;
            break;
        case FLEXE_DRV_PORT_SPEED_200G:
            /*
             * Mark 200G mode
             */
            SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));
            is_200g = soc_reg_field_get(unit, FSCL_TX_MODE_CFGr, reg_val, IS_200Gf);
            val = 1;
            FLEXE_TX_MODE_IS_200G_SET(&is_200g, flexe_port, &val);
            soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

            start_level = 22;
            high_level = 29;
            low_level = 15;
            up_level = 41;
            down_level = 3;
            break;
        case FLEXE_DRV_PORT_SPEED_400G:
            /*
             * Mark 400G mode
             */
            SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_400Gf, 1);
            SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

            start_level = 38;
            high_level = 50;
            low_level = 26;
            break;
        default:
            break;
    }

    /*
     * Port FIFO cfg1 for FlexE port.
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, START_LEVELf, start_level);
    soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, HIGH_LEVELf, high_level);
    soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, LOW_LEVELf, low_level);
    SHR_IF_ERR_EXIT(WRITE_FSCL_PORT_FIFO_CFG_1r(unit, flexe_port, reg_val));

    if (rate != FLEXE_DRV_PORT_SPEED_400G)
    {
        /*
         * Port FIFO cfg2 for FlexE port (not used in case of 400G).
         */
        reg_val = 0;
        soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_2r, &reg_val, PROTECT_UP_LEVELf, up_level);
        soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_2r, &reg_val, PROTECT_DOWN_LEVELf, down_level);
        SHR_IF_ERR_EXIT(WRITE_FSCL_PORT_FIFO_CFG_2r(unit, flexe_port, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   FlexE SAR subsys init [TX direction]
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
STATIC shr_error_e
_soc_flexe_interface_tx_del(
    int unit,
    int bcm_port,
    int speed)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    uint32 is_200g = 0;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));

    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);

    /*
     * Restore 200G/400G configurations
     */
    switch (rate)
    {
        case FLEXE_DRV_PORT_SPEED_200G:
            is_200g = soc_reg_field_get(unit, FSCL_TX_MODE_CFGr, reg_val, IS_200Gf);
            val = 0;
            FLEXE_TX_MODE_IS_200G_SET(&is_200g, bcm_port, &val);
            soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_200Gf, is_200g);
            break;
        case FLEXE_DRV_PORT_SPEED_400G:
            soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_400Gf, 0);
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enables/clears bypass for a PHY in FlexE IF adapter (bus A) [TX direction]
 *   RX direction not relevant.
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
STATIC shr_error_e
_soc_flexe_interface_tx_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    uint32 is_bypass = 0;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);

    if (!enable)
    {
        /*
         * Map channel to invalid port
         */
        val = FLEXE_INVALID_PORT_ID;
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_MAP_RAMm, 0, FSCL_BLOCK(unit), bypass_channel, &val));

        /*
         * Clear bypass bit
         */
        SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));
        is_bypass = soc_reg_field_get(unit, FSCL_TX_MODE_CFGr, reg_val, IS_BYPASSf);
        SHR_BITCLR(&is_bypass, flexe_port);
        soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_BYPASSf, is_bypass);
        SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

        if (rate == FLEXE_DRV_PORT_SPEED_400G)
        {
            /*
             * Restore configurations in case of 400G
             */
            reg_val = 0;
            soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_IS_400Gf, 0);
            soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_400G_CHf, 0x7F);
            SHR_IF_ERR_EXIT(WRITE_FSCL_EXTERNAL_CFGr(unit, reg_val));
        }
    }
    else
    {
        /*
         * Set bypass bit
         */
        SHR_IF_ERR_EXIT(READ_FSCL_TX_MODE_CFGr(unit, &reg_val));
        is_bypass = soc_reg_field_get(unit, FSCL_TX_MODE_CFGr, reg_val, IS_BYPASSf);

        /* coverity[ptr_arith:FALSE] */
        SHR_BITSET(&is_bypass, flexe_port);
        soc_reg_field_set(unit, FSCL_TX_MODE_CFGr, &reg_val, IS_BYPASSf, is_bypass);
        SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MODE_CFGr(unit, reg_val));

        /*
         * Map channel to FlexE port
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_MAP_RAMm, 0, FSCL_BLOCK(unit), bypass_channel, &flexe_port));

        if (rate == FLEXE_DRV_PORT_SPEED_400G)
        {
            /*
             * Configure 400G
             */
            reg_val = 0;
            soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_IS_400Gf, 1);
            soc_reg_field_set(unit, FSCL_EXTERNAL_CFGr, &reg_val, RA_400G_CHf, bypass_channel);
            SHR_IF_ERR_EXIT(WRITE_FSCL_EXTERNAL_CFGr(unit, reg_val));
        }

        /*
         * Set port FIFO level
         */
        reg_val = 0;
        soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, START_LEVELf, 12);
        soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, HIGH_LEVELf, 16);
        soc_reg_field_set(unit, FSCL_PORT_FIFO_CFG_1r, &reg_val, LOW_LEVELf, 8);
        SHR_IF_ERR_EXIT(WRITE_FSCL_PORT_FIFO_CFG_1r(unit, flexe_port, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   FlexE SAR subsys init [RX direction]
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
STATIC shr_error_e
_soc_flexe_sar_rx_init(
    int unit)
{
    uint32 rx_joint_init_done = 0;
    uint32 cpb2sar_fifo_adj_init_done = 0;
    uint32 sar_fifo_adj_init_done = 0;
    uint32 cpb2sar_decimate_init_done = 0;
    uint32 reg_val;
    uint32 nof_waits = 0;
    uint32 data;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_TABLE_SWITCHr, &reg_val, CPB_2_SAR_FIFO_ADJ_INITf, 1);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_TABLE_SWITCHr, &reg_val, SAR_FIFO_ADJ_INITf, 1);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_TABLE_SWITCHr, &reg_val, CPB_2_SAR_DECIMATE_INITf, 1);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_TABLE_SWITCHr(unit, reg_val));

    /*
     * Poll for init done
     */
    while (nof_waits++ < FLEXE_REG_ACCESS_MAX_RETRIES)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));

        rx_joint_init_done = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, RX_JOINT_INIT_DONEf);
        cpb2sar_fifo_adj_init_done =
            soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_FIFO_ADJ_INIT_DONEf);
        sar_fifo_adj_init_done = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, SAR_FIFO_ADJ_INIT_DONEf);
        cpb2sar_decimate_init_done =
            soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_DECIMATE_INIT_DONEf);

        if (rx_joint_init_done && cpb2sar_fifo_adj_init_done && sar_fifo_adj_init_done && cpb2sar_decimate_init_done)
        {
            break;
        }

        sal_usleep(1);
    }

    if (!(rx_joint_init_done && cpb2sar_fifo_adj_init_done && sar_fifo_adj_init_done && cpb2sar_decimate_init_done))
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "failed polling init done bits 0x%x\n", reg_val);
    }

    data = FLEXE_INVALID_SAR_CHANNEL_ID;
    for (index = 0; index < FLEXE_CORE_NOF_CALENDAR * FLEXE_SAR_NOF_TIMESLOTS; ++index)
    {
        /*
         * Map timeslots to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_CPB_2_SAR_RAMm, 0, FSCL_BLOCK(unit), index, &data));
    }

    /*
     * Enable calendar
     */
    SHR_IF_ERR_EXIT(READ_FSCL_CPB_2_SAR_CFG_2r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_2r, &reg_val, CPB_2_SAR_CALENDAR_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_2r(unit, reg_val));

    /*
     * read-clear alarms & statistics
     */
    for (index = 0; index < FLEXE_SAR_NOF_TIMESLOTS; ++index)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CH_ALMr(unit, index, &reg_val));
    }

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_GLB_ALMr(unit, &reg_val));
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_PKT_CNTr(unit, &reg_val));
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_PKT_CNT_256r(unit, &reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_GLB_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_RX_GLB_CFGr, &reg_val, RX_LOOPBACKf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_GLB_CFGr, &reg_val, B_66_SIZE_29Bf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_GLB_CFGr, &reg_val, RX_PKT_CNT_PROBf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_GLB_CFGr(unit, reg_val));

    /*
     * Mark channel as not enabled, map channel to invalid ILKN channel
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, MON_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, CPB_2_SAR_CH_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, BRCM_FC_CH_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_CHAN_MAPf, FLEXE_INVALID_ILKN_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_HIGH_CFGf, 13);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_LOW_CFGf, 3);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_MID_CFGf, 7);

    for (index = 0; index < FLEXE_SAR_NOF_TIMESLOTS; index++)
    {
        SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, index, reg_val));
    }

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_1r, &reg_val, CPB_2_SAR_CFG_PERIODf, 2507);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_1r, &reg_val, CPB_2_SAR_NOM_CNTf, 1887);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_1r, &reg_val, CPB_2_SAR_FIFO_ADJ_ENf, 1);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_1r, &reg_val, SAR_FIFO_ADJ_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_1r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_CPB_2_SAR_CFG_2r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_2r, &reg_val, CPB_2_SAR_Qf, 32);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_2r, &reg_val, CPB_2_SAR_ADJ_01_ENf, 0);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_2r, &reg_val, ADJ_IN_CHECK_VALUEf, 100);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_2r, &reg_val, ADJ_IN_CORRECT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_2r(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_3r, &reg_val, LOCAL_ENf, 1);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_3r, &reg_val, LOCAL_Mf, 426);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_3r, &reg_val, DELAY_CYCLESf, 20);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_3r(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_4r, &reg_val, CPB_2_SAR_FIFO_ADJ_PERIODf, 32767);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_4r, &reg_val, SAR_FIFO_ADJ_PERIODf, 32767);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_4r(unit, reg_val));

    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2SAR_M_2NDr(unit, 0xFEBC44FF));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2SAR_BASE_M_2NDr(unit, 0xFFF8D4FF));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2SAR_M_1STr(unit, 0xAB77B8B0));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2SAR_BASE_M_1STr(unit, 0xAB77C2B0));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_311Mr, &reg_val, CPB_2_SAR_MULTIPLIERf, 3);
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_CFG_311Mr, &reg_val, CPB_2SAR_BYPASS_M_N_2NDf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_CFG_311Mr(unit, reg_val));

    data = 8;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_AEMPTY_DEPTHm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   FlexE SAR subsys init [TX direction]
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
STATIC shr_error_e
_soc_flexe_sar_tx_init(
    int unit)
{
    uint32 reg_val = 0;
    uint32 data;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    data = FLEXE_INVALID_SAR_CHANNEL_ID;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RA_2_SAR_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CALENDAR * FLEXE_SAR_NOF_TIMESLOTS-1, &data));

    /*
     * Enable calendar
     */
    SHR_IF_ERR_EXIT(READ_FSCL_RA_2_SAR_CFG_2r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_2r, &reg_val, RA_2_SAR_CALENDAR_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_CFG_2r(unit, reg_val));

    /*
     * read-clear alarms & statistics
     */
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_PKT_CNTr(unit, &reg_val));
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_PKT_CNT_256r(unit, &reg_val));
    SHR_IF_ERR_EXIT(READ_FSCL_RA_2_SAR_ENV_ALMr(unit, &reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_GLB_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, TX_LOOPBACKf, 0);
    soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, B_66_SIZE_29Bf, 0);
    soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, TX_CNT_PROBf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_GLB_CFGr(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_1r, &reg_val, RA_2_SAR_CFG_PERIODf, 2507);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_1r, &reg_val, RA_2_SAR_NOM_CNTf, 1887);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_CFG_1r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_RA_2_SAR_CFG_2r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_2r, &reg_val, RA_2_SAR_Qf, 32);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_2r, &reg_val, RA_2_SAR_ADJ_01_ENf, 0);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_2r, &reg_val, ADJ_IN_CHECK_VALUEf, 100);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_2r, &reg_val, ADJ_IN_CORRECT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_CFG_2r(unit, reg_val));

    /*
     * map channel to invalid ILKN channel
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, TX_CHAN_MAPf, FLEXE_INVALID_ILKN_CHANNEL_ID);
    for (index = 0; index < FLEXE_SAR_NOF_TIMESLOTS; index++)
    {
        SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_CTRL_CFGr(unit, index, reg_val));
    }

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_3r, &reg_val, LOCAL_ENf, 1);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_3r, &reg_val, LOCAL_Mf, 426);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_CFG_3r(unit, reg_val));

    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2SAR_M_1STr(unit, 0xAB77B8B0));
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2SAR_BASE_M_1STr(unit, 0xAB77C2B0));
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2SAR_M_2NDr(unit, 0xFEBC44FF));
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2SAR_BASE_M_2NDr(unit, 0xFFF8D4FF));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_311Mr, &reg_val, RA_2_SAR_MULTIPLIERf, 3);
    soc_reg_field_set(unit, FSCL_RA_2_SAR_CFG_311Mr, &reg_val, RA_2SAR_BYPASS_M_N_2NDf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_CFG_311Mr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add channel to SAR [RX direction]
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 * [in] ilkn_channel - The ILKN channel ID that this channel maps to (range: 0~511)
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
STATIC shr_error_e
_soc_flexe_sar_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 calendar_sel = 0;
    uint32 hw_busy;
    uint32 joint_init_done;
    uint32 nof_timeslots = 0;
    uint32 fifo_high_cfg = 0;
    uint32 fifo_mid_cfg = 0;
    uint32 fifo_low_cfg = 0;
    uint32 depth;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_CURRENT_TABLEf);

    /*
     * Configure standby calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        ++nof_timeslots;

        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_CPB_2_SAR_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &channel));
    }

    /*
     * switch the calendar (standby->active)
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_TABLE_SWITCHr, &reg_val, CPB_2_SAR_TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_TABLE_SWITCHr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_CPB_2_SAR_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &channel));
    }

    /*
     * Map channel to default ILKN channel (ILKN channel ID == SAR channel ID)
     */
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, CPB_2_SAR_CH_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, channel, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    joint_init_done = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, RX_JOINT_INIT_DONEf);

    /*
     * Poll for joint init done
     */
    while (!joint_init_done && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
        joint_init_done = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, RX_JOINT_INIT_DONEf);
    }

    if (!joint_init_done)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "joint init not done %d\n", reg_val);
    }

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, channel, &reg_val));

    /*
     * configure FIFO according to bandwidth
     */
    if (nof_timeslots == 1)
    {
        fifo_high_cfg = 5;
        fifo_mid_cfg = 2;
        fifo_low_cfg = 1;
    }
    else if (nof_timeslots == 2)
    {
        fifo_high_cfg = 6;
        fifo_mid_cfg = 3;
        fifo_low_cfg = 2;
    }
    else if ((nof_timeslots > 2) && (nof_timeslots <= 5))
    {
        fifo_high_cfg = 7;
        fifo_mid_cfg = 4;
        fifo_low_cfg = 2;
    }
    else if ((nof_timeslots > 5) && (nof_timeslots <= 10))
    {
        fifo_high_cfg = 8;
        fifo_mid_cfg = 5;
        fifo_low_cfg = 2;
    }
    else if ((nof_timeslots > 10) && (nof_timeslots <= 20))
    {
        fifo_high_cfg = 9;
        fifo_mid_cfg = 6;
        fifo_low_cfg = 3;
    }
    else if (nof_timeslots > 20)
    {
        fifo_high_cfg = 11;
        fifo_mid_cfg = 7;
        fifo_low_cfg = 3;
    }

    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_HIGH_CFGf, fifo_high_cfg);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_MID_CFGf, fifo_mid_cfg);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_LOW_CFGf, fifo_low_cfg);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, MON_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, channel, reg_val));

    depth = MIN(nof_timeslots, 8);
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_AEMPTY_DEPTHm, 0, FSCL_BLOCK(unit), channel, &depth));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove a channel from SAR [RX direction]
 *
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
STATIC shr_error_e
_soc_flexe_sar_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 calendar_sel = 0;
    uint32 hw_busy;
    uint32 data;
    uint32 invalid_ch_id = FLEXE_INVALID_SAR_CHANNEL_ID;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Restore channel map and FIFO level
     */
    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, MON_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, CPB_2_SAR_CH_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_HIGH_CFGf, 13);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_MID_CFGf, 7);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_LOW_CFGf, 3);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, channel, reg_val));

    data = 8;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_AEMPTY_DEPTHm, 0, FSCL_BLOCK(unit), channel, &data));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_CURRENT_TABLEf);

    /*
     * Map timeslot to invalid channel
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_CPB_2_SAR_RAMm,
                                      0,
                                      FSCL_BLOCK(unit), (index << 1) | calendar_sel, &invalid_ch_id));
    }

    /*
     * switch the calendar (standby->active)
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CPB_2_SAR_TABLE_SWITCHr, &reg_val, CPB_2_SAR_TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CPB_2_SAR_TABLE_SWITCHr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_SAR_RX_STATEr, reg_val, CPB_2_SAR_BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_CPB_2_SAR_RAMm,
                                      0,
                                      FSCL_BLOCK(unit), (index << 1) | calendar_sel, &invalid_ch_id));
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Add channel to SAR [TX direction]
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] tsmap - timeslot bitmap (maximum 80 bits). Each enabled bit represents a timeslot of 5G.
 * [in] ilkn_channel - The ILKN channel ID that this channel maps to (range: 0~511)
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
STATIC shr_error_e
_soc_flexe_sar_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 calendar_sel = 0;
    uint32 hw_busy;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_CURRENT_TABLEf);

    /*
     * Configure standby calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_RA_2_SAR_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &channel));
    }

    /*
     * switch the calendar (standby->active)
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_RA_2_SAR_TABLE_SWITCHr, &reg_val, RA_2_SAR_TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_TABLE_SWITCHr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_RA_2_SAR_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &channel));
    }

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_CTRL_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, RA_2_SAR_CH_ENf, 1);
    soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, SEG_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_CTRL_CFGr(unit, channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *   Remove a channel from SAR [RX direction]
 *
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
STATIC shr_error_e
_soc_flexe_sar_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 calendar_sel = 0;
    uint32 hw_busy;
    uint32 invalid_ch_id = FLEXE_INVALID_SAR_CHANNEL_ID;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_CTRL_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, RA_2_SAR_CH_ENf, 0);
    soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, SEG_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_CTRL_CFGr(unit, channel, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_CURRENT_TABLEf);

    /*
     * Configure standby calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_RA_2_SAR_RAMm,
                                      0,
                                      FSCL_BLOCK(unit), (index << 1) | calendar_sel, &invalid_ch_id));
    }

    /*
     * switch the calendar (standby->active)
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_RA_2_SAR_TABLE_SWITCHr, &reg_val, RA_2_SAR_TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RA_2_SAR_TABLE_SWITCHr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_SAR_TX_STATEr, reg_val, RA_2_SAR_BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                      FSCL_RA_2_SAR_RAMm,
                                      0,
                                      FSCL_BLOCK(unit), (index << 1) | calendar_sel, &invalid_ch_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure CPB FIFO depth for channel.
 *   CPB is a receiver jitter buffer in the direction of ILKN -> SAR.
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] ts_num - number of timeslots. Each timeslot is 5G.
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
STATIC shr_error_e
_soc_flexe_cpb_fifo_depth_set(
    int unit,
    int channel,
    uint32 ts_num)
{
    uint32 full_depth_cfg = 0;
    uint32 bp_level_cfg = 0;
    uint32 rd_level_cfg = 0;
    uint32 high_depth_cfg = 0;
    uint32 low_depth_cfg = 0;
    uint32 bp_low_level_cfg = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Calculate FIFO level according to rate (number of timeslots)
     */
    full_depth_cfg = 128 * ts_num;
    bp_level_cfg = FLEXE_CPB_EGR_BP_LEVEL_OFFSET * ts_num;
    bp_low_level_cfg = FLEXE_CPB_EGR_BP_LEVEL_OFFSET * ts_num - 4;
    rd_level_cfg = FLEXE_CPB_EGR_RD_LEVEL_BASE * ts_num;
    high_depth_cfg = FLEXE_CPB_EGR_HIGH_DEPTH_BASE * ts_num;
    low_depth_cfg = FLEXE_CPB_EGR_LOW_DEPTH_BASE * ts_num;

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_FULL_DEPTH_EGRESSm, 0, FSCL_BLOCK(unit), channel, &full_depth_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_BP_LEVEL_EGRESSm, 0, FSCL_BLOCK(unit), channel, &bp_level_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_BP_LOW_EGRESSm, 0, FSCL_BLOCK(unit), channel, &bp_low_level_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_RD_LEVEL_EGRESSm, 0, FSCL_BLOCK(unit), channel, &rd_level_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_HIGH_DEPTH_EGm, 0, FSCL_BLOCK(unit), channel, &high_depth_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_LOW_DEPTH_EGm, 0, FSCL_BLOCK(unit), channel, &low_depth_cfg));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure CPB FIFO depth for channel, optimized for performance.
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
 * [in] ts_num - number of timeslots. Each timeslot is 5G.
 * [in] mode - used to calculate the FIFO depth for channel
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
shr_error_e
_soc_flexe_cpb_fifo_depth_set_for_performance(
    int unit,
    int channel,
    uint32 ts_num,
    uint32 mode)
{
    uint32 full_depth_cfg = 0;
    uint32 rd_level_cfg = 0;
    uint32 high_depth_cfg = 0;
    uint32 low_depth_cfg = 0;

    SHR_FUNC_INIT_VARS(unit);

    mode = MIN(mode, 10);

    /*
     * Calculate FIFO level according to rate (number of timeslots)
     */
    full_depth_cfg = 128 * ts_num;
    rd_level_cfg = 5.5 * mode * ts_num + MIN(ts_num, 8);

    if ((mode * ts_num) % 2 != 0)
    {
        rd_level_cfg++;
    }

    high_depth_cfg = rd_level_cfg + 5 * ts_num;
    low_depth_cfg = rd_level_cfg - 5 * ts_num;

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_FULL_DEPTH_EGRESSm, 0, FSCL_BLOCK(unit), channel, &full_depth_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_RD_LEVEL_EGRESSm, 0, FSCL_BLOCK(unit), channel, &rd_level_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_HIGH_DEPTH_EGm, 0, FSCL_BLOCK(unit), channel, &high_depth_cfg));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_LOW_DEPTH_EGm, 0, FSCL_BLOCK(unit), channel, &low_depth_cfg));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init CPB subsys.
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
STATIC shr_error_e
_soc_flexe_cpb_init(
    int unit)
{
    soc_reg_above_64_val_t data;
    uint32 invalid_start_blk = FLEXE_INVALID_CHANNEL_ID;
    uint32 invalid_next_blk = 0x4000 | FLEXE_INVALID_CHANNEL_ID;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(data);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MON_EN_EGr(unit, data));

    /*
     * Map timeslot to invalid channel
     */
    for (index = 0; index < FLEXE_SAR_NOF_TIMESLOTS; ++index)
    {
        _soc_flexe_cpb_fifo_depth_set(unit, index, 1);
    }

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_START_BLK_TBm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &invalid_start_blk));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_NEXT_BLK_TBm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &invalid_next_blk));

    SHR_IF_ERR_EXIT(WRITE_FSCL_CLR_EGr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add a channel to CPB
 *
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
STATIC shr_error_e
_soc_flexe_cpb_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    soc_reg_above_64_val_t data;
    uint32 reg_val;
    uint32 next_blk;
    int start_blk = -1;
    int prev_blk = -1;
    int blk_id = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * start_blk = first timeslot in tsmap, blk_id=0
     * start_blk points to next timeslot in tsmap, blk_id=1
     * ...
     * last timeslot in tsmap sets the last entry indication (bit[14] = 1)
     */
    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        if (start_blk < 0)
        {
            /*
             * blk_id = 0
             */
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_START_BLK_TBm, 0, FSCL_BLOCK(unit), channel, &index));

            start_blk = index;
        }
        else
        {
            /*
             * blk_id = 1~n-1
             */
            next_blk = FSCL_NEXT_BLK_TABLE_DATA(0, blk_id, index);
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_NEXT_BLK_TBm, 0, FSCL_BLOCK(unit), prev_blk, &next_blk));
        }

        prev_blk = index;
        ++blk_id;
    }

    /*
     * empty timeslot bitmap
     */
    if (start_blk < 0)
    {
        SHR_ERR_EXIT(SOC_E_PARAM, "Empty timeslot bitmap\n");
    }

    /*
     * Point last block to start block, mark last entry indication
     */
    next_blk = FSCL_NEXT_BLK_TABLE_DATA(1, 0, start_blk);
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_NEXT_BLK_TBm, 0, FSCL_BLOCK(unit), prev_blk, &next_blk));

    _soc_flexe_cpb_fifo_depth_set_for_performance(unit, channel, blk_id, 2);

    sal_usleep(1);

    /*
     * Enable channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_MON_EN_EGr(unit, data));
    SHR_BITSET(data, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MON_EN_EGr(unit, data));

    /*
     * Clear statistics for channel
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_CH_EGr(unit, channel));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_STA_CLR_EGr, &reg_val, STA_CLR_EGf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_EGr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove a channel from CPB
 *
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
STATIC shr_error_e
_soc_flexe_cpb_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    soc_reg_above_64_val_t data;
    uint32 invalid_start_blk = FLEXE_INVALID_CHANNEL_ID;
    uint32 invalid_next_blk = FSCL_NEXT_BLK_TABLE_DATA(1, 0, FLEXE_INVALID_CHANNEL_ID);
    uint32 reg_val;
    int blk_id = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Disable channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_MON_EN_EGr(unit, data));
    SHR_BITCLR(data, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MON_EN_EGr(unit, data));

    /*
     * Mark start blk as invalid
     */
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_START_BLK_TBm, 0, FSCL_BLOCK(unit), channel, &invalid_start_blk));

    _soc_flexe_cpb_fifo_depth_set_for_performance(unit, channel, 1, 2);

    /*
     * Restore next blk configuration for all timeslots
     */
    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_NEXT_BLK_TBm, 0, FSCL_BLOCK(unit), index, &invalid_next_blk));

        ++blk_id;
    }

    /*
     * Clear statistics for channel
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_CH_EGr(unit, channel));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_STA_CLR_EGr, &reg_val, STA_CLR_EGf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_EGr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear statistics in CPB
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
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
shr_error_e
_soc_flexe_cpb_statistic_clear(
    int unit,
    int channel)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * clear all 
     */
    if (0xFF == channel)
    {
        /*
         * Clear statistics for all channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_CLR_EGr(unit, 1));
    }
    else
    {
        /*
         * Clear statistics for single channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_CH_EGr(unit, channel));

        reg_val = 0;
        soc_reg_field_set(unit, FSCL_STA_CLR_EGr, &reg_val, STA_CLR_EGf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_STA_CLR_EGr(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init MCMAC in SAR
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
STATIC shr_error_e
_soc_flexe_mcmac_init(
    int unit)
{
    soc_reg_above_64_val_t data;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Disable statistics
     */
    SOC_REG_ABOVE_64_CLEAR(data);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STAT_ENABLEr(unit, data));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_WORK_MODEr(unit, data));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_ENABLEr(unit, data));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STAT_ENABLEr(unit, data));

    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_TS_RAM_INITr(unit, 1));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_ENV_RAM_INITr(unit, 1));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_MACRX_CFGr, &reg_val, WATCHDOG_TIMEf, 0x12);
    soc_reg_field_set(unit, FSCL_MACRX_CFGr, &reg_val, WATCHDOG_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_CFGr(unit, reg_val));

    /*
     * average 8B IPG
     */
    SHR_IF_ERR_EXIT(READ_FSCL_MACTX_AVG_IPGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_MACTX_AVG_IPGr, &reg_val, AVG_IPGf, 2);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_AVG_IPGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add statistics for channel in MCMAC [TX direction]
 *
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
STATIC shr_error_e
_soc_flexe_mcmac_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    soc_reg_above_64_val_t data;
    uint32 mtu;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    mtu = FLEXE_MTU;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_MACTX_MAX_PKTm, 0, FSCL_BLOCK(unit), channel, &mtu));

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslots to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_MACTX_TS_CONFIGm, 0, FSCL_BLOCK(unit), index, &channel));
    }

    SOC_REG_ABOVE_64_CLEAR(data);

    /*
     * Clear statistics for channel
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STATISTIC_CLR_CHANr(unit, channel));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STATISTIC_CLRr(unit, 1));

    SOC_REG_ABOVE_64_CLEAR(data);

    /*
     * Enable statistics for channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_MACTX_STAT_ENABLEr(unit, data));
    SHR_BITSET(data, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STAT_ENABLEr(unit, data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove statistics for channel in MCMAC [TX direction]
 *
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
STATIC shr_error_e
_soc_flexe_mcmac_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    soc_reg_above_64_val_t data;
    uint32 invalid_sar_ch = FLEXE_INVALID_SAR_CHANNEL_ID;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslots to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_MACTX_TS_CONFIGm, 0, FSCL_BLOCK(unit), index, &invalid_sar_ch));
    }

    SOC_REG_ABOVE_64_CLEAR(data);

    /*
     * Disable statistics for channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_MACTX_STAT_ENABLEr(unit, data));
    SHR_BITCLR(data, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STAT_ENABLEr(unit, data));

    SOC_REG_ABOVE_64_CLEAR(data);

    /*
     * Clear statistics for channel
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STATISTIC_CLR_CHANr(unit, channel));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STATISTIC_CLRr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add statistics for channel in MCMAC [RX direction]
 *
 *
 * [in] unit - unit ID
 * [in] channel - SAR channel ID (range: 0~79)
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
STATIC shr_error_e
_soc_flexe_mcmac_channel_rx_set(
    int unit,
    int channel,
    int enable)
{
    soc_reg_above_64_val_t data;
    uint32 mtu = FLEXE_MTU;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_MACRX_MAX_PKTm, 0, FSCL_BLOCK(unit), channel, &mtu));

    SOC_REG_ABOVE_64_CLEAR(data);

    SHR_IF_ERR_EXIT(READ_FSCL_MACRX_ENABLEr(unit, data));
    SHR_BITCLR(data, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_ENABLEr(unit, data));

    SOC_REG_ABOVE_64_CLEAR(data);

    if (enable)
    {
        /*
         * Clear statistics for channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STATISTIC_CLEAR_CHANr(unit, channel));
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STATISTIC_CLEARr(unit, 1));

        /*
         * Enable statistics for channel
         */
        SHR_IF_ERR_EXIT(READ_FSCL_MACRX_STAT_ENABLEr(unit, data));
        SHR_BITSET(data, channel);
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STAT_ENABLEr(unit, data));
    }
    else
    {
        /*
         * Disable statistics for channel
         */
        SHR_IF_ERR_EXIT(READ_FSCL_MACRX_STAT_ENABLEr(unit, data));
        SHR_BITCLR(data, channel);
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STAT_ENABLEr(unit, data));

        /*
         * Clear statistics for channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STATISTIC_CLEAR_CHANr(unit, channel));
        SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STATISTIC_CLEARr(unit, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init MAC rate adapter, this is the rate adapter in SAR
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
STATIC shr_error_e
_soc_flexe_mac_rateadp_init(
    int unit)
{
    uint64 mac_adj_cfg;
    uint32 blk_num_cfg = 0;
    uint32 map_reg = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &blk_num_cfg, BLK_NUMf, 1);

    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &map_reg, MAP_SEQf, 0xF);
    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &map_reg, MAP_CHIDf, FLEXE_INVALID_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &map_reg, MAP_ENf, 0);

    COMPILER_64_ZERO(mac_adj_cfg);
    SHR_IF_ERR_EXIT(READ_FSCL_MAC_ADJ_CFGr(unit, 0, &mac_adj_cfg));
    soc_reg64_field32_set(unit, FSCL_MAC_ADJ_CFGr, &mac_adj_cfg, I_PLL_COEFFf, 0);
    soc_reg64_field32_set(unit, FSCL_MAC_ADJ_CFGr, &mac_adj_cfg, I_CFG_FINISHf, 1);

    for (index = 0; index < FLEXE_CORE_NOF_CHANNELS; ++index)
    {
        /*
         * Map block FIFO to invalid channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_BLK_NUM_CFGr(unit, index, blk_num_cfg));
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_MAP_REGr(unit, index, map_reg));
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_ADJ_CFGr(unit, index, mac_adj_cfg));
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_IND_SELr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add channel to MAC rate adapter
 *
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
STATIC shr_error_e
_soc_flexe_mac_rateadp_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 algo_sel = 0;
    uint32 num_ts = 0;
    int index;
    uint32 fifo_level[2][80] = {
        {
         0x0d0b0702, 0x19140c03, 0x22180e04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482808, 0x6c583109,
         0x7864370a, 0x8c78440f, 0x9c884c0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         },
        {
         0x0d0b0702, 0x19140c03, 0x22180f04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482908, 0x6c583109,
         0x7864370a, 0x8c78410a, 0x9c88490a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         }
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        ++num_ts;
    }

    SHR_IF_ERR_EXIT(READ_FSCL_MAC_IND_SELr(unit, &reg_val));
    algo_sel = soc_reg_field_get(unit, FSCL_MAC_IND_SELr, reg_val, IND_SELf);

    /*
     * Calculate FIFO level according to number of timeslots
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_FIFO_LEVEL_CFGr(unit, channel, fifo_level[algo_sel & 0x1][num_ts - 1]));

    SHR_IF_ERR_EXIT(READ_FSCL_MAC_BLK_NUM_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 0);
    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &reg_val, BLK_NUMf, MIN(num_ts, FLEXE_RATEADP_MAX_NOF_TIMESLOTS));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_BLK_NUM_CFGr(unit, channel, reg_val));

    num_ts = 0;

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        reg_val = 0;
        soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &reg_val, MAP_SEQf, num_ts);
        soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &reg_val, MAP_CHIDf, channel);
        soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &reg_val, MAP_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_MAP_REGr(unit, index, reg_val));

        /*
         * Up to 16 FIFO blocks
         */
        if (++num_ts >= FLEXE_RATEADP_MAX_NOF_TIMESLOTS)
        {
            break;
        }
    }

    SHR_IF_ERR_EXIT(READ_FSCL_MAC_BLK_NUM_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_BLK_NUM_CFGr(unit, channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove a channel from MAC rate adapter
 *
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
STATIC shr_error_e
_soc_flexe_mac_rateadp_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 mac_map_reg = 0;
    uint32 num_ts = 0;
    uint32 algo_sel = 0;
    int index;
    uint32 fifo_level[2][80] = {
        {
         0x0d0b0702, 0x19140c03, 0x22180e04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482808, 0x6c583109,
         0x7864370a, 0x8c78440f, 0x9c884c0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         },
        {
         0x0d0b0702, 0x19140c03, 0x22180f04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482908, 0x6c583109,
         0x7864370a, 0x8c78410a, 0x9c88490a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         }
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_MAC_IND_SELr(unit, &reg_val));
    algo_sel = soc_reg_field_get(unit, FSCL_MAC_IND_SELr, reg_val, IND_SELf);

    /*
     * Restore FIFO level configuration
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_FIFO_LEVEL_CFGr(unit, channel, fifo_level[algo_sel & 0x1][0]));

    SHR_IF_ERR_EXIT(READ_FSCL_MAC_BLK_NUM_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 0);
    soc_reg_field_set(unit, FSCL_MAC_BLK_NUM_CFGr, &reg_val, BLK_NUMf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_BLK_NUM_CFGr(unit, channel, reg_val));

    num_ts = 0;

    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &mac_map_reg, MAP_SEQf, 0xF);
    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &mac_map_reg, MAP_CHIDf, FLEXE_INVALID_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_MAC_MAP_REGr, &mac_map_reg, MAP_ENf, 0);

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Restore map_reg
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAC_MAP_REGr(unit, index, mac_map_reg));

        if (++num_ts >= FLEXE_RATEADP_MAX_NOF_TIMESLOTS)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init FlexE MUX
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
STATIC shr_error_e
_soc_flexe_demux_init(
    int unit)
{
    uint32 reorder_src_invalid = FLEXE_DEMUX_REORDER_SRC_INVALID;
    uint32 row = FLEXE_DEMUX_RAM_INVALID_ROW;
    uint32 lphy_invalid = FLEXE_INVALID_PORT_ID;
    soc_reg_above_64_val_t rd_rule;
    uint64 group_cfg;
    uint64 lphy_reorder;
    uint32 chproperty = 0xFFFFF;
    int lphy;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Mark as invalid
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXEGROUP_INFOr(unit, 0xFFFFFFFF));

    SOC_REG_ABOVE_64_CLEAR(rd_rule);
    SOC_REG_ABOVE_64_CREATE_MASK(rd_rule, 80, 0);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_WCFGRAMm, 0, FLEXE_CORE_NOF_SUBCALS-1, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &row));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_WCFGRAM_Bm, 0, FLEXE_CORE_NOF_SUBCALS-1, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &row));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RDRULEm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS * FLEXE_CORE_NOF_TIMESLOTS -1, &rd_rule));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RDRULE_Bm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS * FLEXE_CORE_NOF_TIMESLOTS -1, &rd_rule));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CHPROPERTYm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS -1, &chproperty));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CHPROPERTY_Bm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS -1, &chproperty));

    SHR_IF_ERR_EXIT(WRITE_FSCL_AUTOSET_ENr(unit, 0x40000001));

    COMPILER_64_SET(group_cfg, 0, 0);
    COMPILER_64_SET(lphy_reorder, 0, 0);
    for (lphy = 0; lphy < FLEXE_CORE_NOF_LPHYS; ++lphy)
    {
        /* coverity[incompatible_cast:FALSE] */
        FLEXE_DEMUX_GROUP_CFG_SET(&group_cfg, lphy, &lphy_invalid);

        /* coverity[incompatible_cast:FALSE] */
        FLEXE_DEMUX_REORDER_INFO_SET(&lphy_reorder, lphy, &reorder_src_invalid);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_GROUP_CFGr(unit, group_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_REORDERr(unit, lphy_reorder));

    SHR_IF_ERR_EXIT(WRITE_FSCL_ENABLE_ABr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add PHY to FlexE DEMUX
 *
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port.
 * [in] lphy_bitmap - bitmap of the lphys allocated for this group. Each bit represents a 50G logical PHY.
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
STATIC shr_error_e
_soc_flexe_demux_phy_add(
    int unit,
    int group,
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed)
{
    soc_reg_t grp_cfg_regs[] = { FSCL_FLEXE_INSTANCE_GRP_CFG_0r, FSCL_FLEXE_INSTANCE_GRP_CFG_1r };
    SHR_BITDCL grp_instances[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    uint32 min_instance_cur = FLEXE_INVALID_PORT_ID;
    uint32 min_instance = FLEXE_INVALID_PORT_ID;
    uint32 min_lphy = FLEXE_INVALID_PORT_ID;
    uint32 min_lphy_sel;
    uint32 flexegroup_info = 0;
    uint32 mode_info = 0;
    uint32 reorder_sel;
    uint64 reorder_info;
    uint64 group_cfg;
    uint32 grp_cfg = 0;
    uint32 lphy = 0;
    int instance;
    int nof_lphys;
    int is_100g;
    int index, ii, lphy_index;

    SHR_FUNC_INIT_VARS(unit);

    is_100g = (speed >= FLEXE_PHY_SPEED_100G);
    nof_lphys = is_100g ? 2 : 1;

    COMPILER_64_ZERO(reorder_info);
    COMPILER_64_ZERO(group_cfg);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXEGROUP_INFOr(unit, &flexegroup_info));
    SHR_IF_ERR_EXIT(READ_FSCL_MODE_INFOr(unit, &mode_info));
    SHR_IF_ERR_EXIT(READ_FSCL_REORDERr(unit, &reorder_info));
    SHR_IF_ERR_EXIT(READ_FSCL_GROUP_CFGr(unit, &group_cfg));

    /*
     * Find all existing instances in group
     */
    SHR_BITCLR_RANGE(grp_instances, 0, FLEXE_CORE_NOF_INSTANCES);

    if (FLEXE_GROUP_IS_VALID(group))
    {
        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, &grp_cfg));
        FLEXE_MULTI_DESKEW_GRP_CFG_GET(&grp_cfg, group, grp_instances);
    }

    min_lphy = soc_flexe_std_minimum_lphy_calculate(lphys);

    /*
     * Update min LPHY for existing LPHYs if necessary
     */
    SHR_BIT_ITER(grp_instances, FLEXE_CORE_NOF_INSTANCES, instance)
    {
        /*
         * Find the minimum LPHY for group
         */
        min_lphy_sel = FLEXE_INVALID_PORT_ID;

        for (lphy =0; lphy <FLEXE_CORE_NOF_LPHYS; ++lphy )
        {
            reorder_sel = 0;

            /* coverity[incompatible_cast:FALSE] */
            FLEXE_DEMUX_REORDER_INFO_GET(&reorder_info, lphy, &reorder_sel);

            if (reorder_sel == instance)
            {
                /* coverity[incompatible_cast:FALSE] */
                FLEXE_DEMUX_GROUP_CFG_GET(&group_cfg, lphy, &min_lphy_sel);
                break;
            }
        }

        /*
         * min LPHYs changed for existing PHYs
         */
        if ((min_lphy_sel > min_lphy) && (min_lphy_sel != FLEXE_INVALID_PORT_ID))
        {
            lphy = 0;

            for (ii = 0; ii < FLEXE_CORE_NOF_LPHYS; ++ii)
            {
                /* coverity[incompatible_cast:FALSE] */
                FLEXE_DEMUX_GROUP_CFG_GET(&group_cfg, ii, &lphy);

                /*
                 * Update all existing LPHYs in the group to point to the newly calculated min LPHY
                 */
                if (lphy == min_lphy_sel)
                {
                    /* coverity[incompatible_cast:FALSE] */
                    FLEXE_DEMUX_GROUP_CFG_SET(&group_cfg, ii, &min_lphy);
                }
            }
        }
        else
        {
            /*
             * min LPHY does not change for existing PHYs
             * current minimum LPHY will be used for the newly added LPHYs
             */
            min_lphy = MIN(min_lphy, min_lphy_sel);
        }

        break;
    }

    min_instance = soc_flexe_std_bitmap_find_first_set(instance_bitmap, FLEXE_CORE_NOF_INSTANCES);
    min_instance_cur = soc_flexe_std_bitmap_find_first_set(grp_instances, FLEXE_CORE_NOF_INSTANCES);

    if (min_instance < min_instance_cur)
    {
        /*
         * Update min instance for existing instances if necessary
         */
        SHR_BIT_ITER(grp_instances, FLEXE_CORE_NOF_INSTANCES, instance)
        {
            /*
             * All instances in one group are synced to the minimum instance in the group
             */
            FLEXE_DEMUX_GROUP_INFO_SET(&flexegroup_info, instance, (SHR_BITDCL *) & min_instance);

        }
    }
    else
    {
        min_instance = min_instance_cur;
    }


    lphy_index = 0;

    /*
     * Iterate through all instances
     */
    SHR_BIT_ITER(instance_bitmap, FLEXE_CORE_NOF_INSTANCES, instance)
    {
        /*
         * All instances in one group are synced to the minimum instance in the group
         */
        FLEXE_DEMUX_GROUP_INFO_SET(&flexegroup_info, instance, (SHR_BITDCL *) & min_instance);
        /* coverity[ptr_arith:FALSE] */
        is_100g ? SHR_BITSET(&mode_info, instance) : SHR_BITCLR(&mode_info, instance);

        /*
         * Before reorder, each 100G instance splits into two LPHYs, x/x+8, respectively.
         * After reorder, each 100G instance takes two LPHYs from lphys.
         * Each LPHY is 50G, a PHY might take more than 2 LPHYs. E.g., a 200G PHY takes 4 LPHYs (2 instances).
         */
        reorder_sel = instance;

        for (index = 0; index < nof_lphys; ++index)
        {
            if (index == 1)
            {
                /*
                 * 2nd LPHY of a 100G instance is x+8 before reorder
                 */
                reorder_sel += 8;
            }

            /*
             * lphy selects either the 1st LPHY (=instance ID) or the 2nd LPHY (=instance ID + 8)
             */
            /* coverity[incompatible_cast:FALSE] */
            FLEXE_DEMUX_REORDER_INFO_SET(&reorder_info, lphys[lphy_index], &reorder_sel);

            /*
             * All LPHYs aligned to the minimum lphy in the group
             */
            /* coverity[incompatible_cast:FALSE] */
            FLEXE_DEMUX_GROUP_CFG_SET(&group_cfg, lphys[lphy_index], &min_lphy);

            ++lphy_index;
        }
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXEGROUP_INFOr(unit, flexegroup_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_INFOr(unit, mode_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_REORDERr(unit, reorder_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_GROUP_CFGr(unit, group_cfg));

    /*
     * FlexE instance group deskew configuration
     */
    if (FLEXE_GROUP_IS_VALID(group))
    {
        uint32 grp_cfg_tmp = 0;

        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE(&grp_cfg_tmp, (group % 4) * 8, instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

        /* coverity[ptr_arith:FALSE] */
        SHR_BITOR_RANGE(&grp_cfg, &grp_cfg_tmp, (group % 4) * 8, FLEXE_CORE_NOF_INSTANCES, &grp_cfg);
        SHR_IF_ERR_EXIT(soc_custom_reg32_set(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, grp_cfg));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove PHY from FlexE DEMUX
 *
 *
 * [in] unit - unit ID
 * [in] group - FlexE group ID (0~7)
 * [in] phy_bitmap - bitmap of the phys belong to this group. Each bit represents a FlexE port.
 * [in] lphy_bitmap - bitmap of the lphys allocated for this group. Each bit represents a 50G logical PHY.
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
STATIC shr_error_e
_soc_flexe_demux_phy_remove(
    int unit,
    int group,
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed)
{
    soc_reg_t grp_cfg_regs[] = { FSCL_FLEXE_INSTANCE_GRP_CFG_0r, FSCL_FLEXE_INSTANCE_GRP_CFG_1r };
    SHR_BITDCL grp_instances[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    uint32 reorder_src_invalid = FLEXE_DEMUX_REORDER_SRC_INVALID;
    uint32 invalid_instance = FLEXE_INVALID_PORT_ID;
    uint32 min_instance = FLEXE_INVALID_PORT_ID;
    uint32 flexegroup_info = 0;
    uint32 mode_info = 0;
    uint32 grp_cfg = 0;
    uint64 reorder_info;
    uint64 group_cfg;
    uint32 min_lphy_sel;
    uint32 min_lphy;
    int instance;
    int lphy;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reorder_info);
    COMPILER_64_ZERO(group_cfg);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXEGROUP_INFOr(unit, &flexegroup_info));
    SHR_IF_ERR_EXIT(READ_FSCL_MODE_INFOr(unit, &mode_info));
    SHR_IF_ERR_EXIT(READ_FSCL_REORDERr(unit, &reorder_info));
    SHR_IF_ERR_EXIT(READ_FSCL_GROUP_CFGr(unit, &group_cfg));

    /*
     * Find all existing instances in group
     */
    SHR_BITCLR_RANGE(grp_instances, 0, FLEXE_CORE_NOF_INSTANCES);

    if (FLEXE_GROUP_IS_VALID(group))
    {
        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, &grp_cfg));
        FLEXE_MULTI_DESKEW_GRP_CFG_GET(&grp_cfg, group, grp_instances);
    }

    min_instance = soc_flexe_std_bitmap_find_first_set(grp_instances, FLEXE_CORE_NOF_INSTANCES);

    SHR_BITREMOVE_RANGE(grp_instances, instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES, grp_instances);

    /*
     * If minimum instance is being removed, update minimum instance for remaining instances
     */
    if (SHR_IS_BITSET(instance_bitmap, min_instance))
    {
        min_instance = soc_flexe_std_bitmap_find_first_set(grp_instances, FLEXE_CORE_NOF_INSTANCES);

        SHR_BIT_ITER(grp_instances, FLEXE_CORE_NOF_INSTANCES, instance)
        {
            FLEXE_DEMUX_GROUP_INFO_SET(&flexegroup_info, instance, &min_instance);
        }
    }

    SHR_BIT_ITER(instance_bitmap, FLEXE_CORE_NOF_INSTANCES, instance)
    {
        /*
         * Mark as invalid
         */
        FLEXE_DEMUX_GROUP_INFO_SET(&flexegroup_info, instance, &invalid_instance);

        /*
         * Restore mode info
         */
        /* coverity[ptr_arith:FALSE] */
        SHR_BITCLR(&mode_info, instance);
    }

    min_lphy = soc_flexe_std_bitmap_find_first_set(lphy_bitmap, FLEXE_CORE_NOF_LPHYS);

    /*
     * Update min LPHY for existing LPHYs if necessary
     */
    SHR_BIT_ITER(grp_instances, FLEXE_CORE_NOF_INSTANCES, instance)
    {
        /*
         * Find the minimum LPHY for group
         */
        min_lphy_sel = 0;

        /* coverity[incompatible_cast:FALSE] */
        FLEXE_DEMUX_GROUP_CFG_GET(&group_cfg, instance, &min_lphy_sel);

        /*
         * min LPHY is being removed
         */
        if (min_lphy_sel == min_lphy)
        {
            min_lphy_sel = FLEXE_INVALID_PORT_ID;

            for (lphy=0; lphy < FLEXE_CORE_NOF_LPHYS; ++lphy)
            {
                uint32 reorder_sel = 0;

                /* coverity[incompatible_cast:FALSE] */
                FLEXE_DEMUX_REORDER_INFO_GET(&reorder_info, lphy, &reorder_sel);

                if (SHR_BITGET(grp_instances, reorder_sel & (FLEXE_CORE_NOF_INSTANCES-1)))
                {
                    if (min_lphy_sel > lphy)
                    {
                        /*
                         * Find the new candidate for min LPHY.
                         * This is the minimum LPHY which selects LPHYs from instances belong to this group.
                         */
                        min_lphy_sel = lphy;
                    }

                    /* coverity[incompatible_cast:FALSE] */
                    FLEXE_DEMUX_GROUP_CFG_SET(&group_cfg, lphy, &min_lphy_sel);
                }
            }
        }

        break;
    }

    SHR_BIT_ITER(lphy_bitmap, FLEXE_CORE_NOF_LPHYS, lphy)
    {
        /*
         * Mark as invalid
         */
        /* coverity[incompatible_cast:FALSE] */
        FLEXE_DEMUX_REORDER_INFO_SET(&reorder_info, lphy, &reorder_src_invalid);

        /* coverity[incompatible_cast:FALSE] */
        FLEXE_DEMUX_GROUP_CFG_SET(&group_cfg, lphy, &invalid_instance);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXEGROUP_INFOr(unit, flexegroup_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_MODE_INFOr(unit, mode_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_REORDERr(unit, reorder_info));
    SHR_IF_ERR_EXIT(WRITE_FSCL_GROUP_CFGr(unit, group_cfg));

    /*
     * Remove instances from FlexE deskew group
     */
    if (FLEXE_GROUP_IS_VALID(group))
    {
        uint32 grp_cfg_temp = 0;

        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, &grp_cfg));

        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE(&grp_cfg_temp, (group % 4) * FLEXE_CORE_NOF_INSTANCES, instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

        /* coverity[callee_ptr_arith:FALSE] */
        SHR_BITREMOVE_RANGE(&grp_cfg, &grp_cfg_temp, (group % 4) * FLEXE_CORE_NOF_INSTANCES, FLEXE_CORE_NOF_INSTANCES, &grp_cfg);

        SHR_IF_ERR_EXIT(soc_custom_reg32_set(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, grp_cfg));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add a channel to FlexE DEMUX
 *
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] calendar_sel - Which calendar to add this channel to
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
STATIC shr_error_e
_soc_flexe_demux_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel)
{
    soc_mem_t wcfgram_mem, rdrule_mem, chproperty_mem;
    soc_reg_above_64_val_t calendar_cfg;
    soc_reg_above_64_val_t own_tsgroup;
    soc_reg_above_64_val_t rdrule;
    soc_reg_above_64_val_t read_ts;
    uint32 channel_own_tsgroup = 0;
    uint32 chproperty = 0;
    uint32 timeslots[FLEXE_CORE_NOF_TIMESLOTS] = { 0 };
    uint32 rownum;
    uint32 channel_1_based = channel + 1;
    uint32 timeslot_plus_1;
    int blk_cnt;
    int cycle_cnt;
    int blk;
    int cycle;
    int tsnum = 0;
    int timeslot;
    int subcal;
    int col;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(calendar_cfg);
    SOC_REG_ABOVE_64_CLEAR(own_tsgroup);
    SOC_REG_ABOVE_64_CLEAR(read_ts);

    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CALENDAR_CFGr, READ_FSCL_CALENDAR_CFG_Br, unit, calendar_cfg);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CHANNEL_OWN_TSGROUPr, READ_FSCL_CHANNEL_OWN_TSGROUP_Br, unit, own_tsgroup);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_READY_TSr, READ_FSCL_READY_TS_Br, unit, read_ts);

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * Configure calendar, map timeslot to channel
         */
        FLEXE_DEMUX_CALENDAR_CFG_SET(calendar_cfg, timeslot, &channel_1_based);

        /*
         * Channel own timeslot group, each group is 50G
         */
        /* coverity[ptr_arith:FALSE] */
        SHR_BITSET(&channel_own_tsgroup, timeslot / 10);

        /*
         * Timeslot enable
         */
        SHR_BITSET(read_ts, timeslot);

        timeslots[tsnum] = timeslot;
        ++tsnum;
    }

    FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_SET(own_tsgroup, channel, &channel_own_tsgroup);

    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CALENDAR_CFGr, WRITE_FSCL_CALENDAR_CFG_Br, unit, calendar_cfg);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CHANNEL_OWN_TSGROUPr, WRITE_FSCL_CHANNEL_OWN_TSGROUP_Br, unit,
                    own_tsgroup);

    blk_cnt = FLEXE_MAX_COMMON_DIVISOR_10(tsnum);
    cycle_cnt = tsnum / blk_cnt;

    timeslot = 0;
    subcal = 0;
    wcfgram_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_WCFGRAMm : FSCL_WCFGRAM_Bm;
    rdrule_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_RDRULEm : FSCL_RDRULE_Bm;
    chproperty_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_CHPROPERTYm : FSCL_CHPROPERTY_Bm;

    for (blk = 0; blk < blk_cnt; ++blk)
    {
        for (cycle = 0; cycle < cycle_cnt; ++cycle)
        {
            SOC_REG_ABOVE_64_CLEAR(rdrule);

            for (col = 0; col < FLEXE_CORE_NOF_SUBCALS; ++col)
            {
                rownum = (blk + col) % FLEXE_CORE_NOF_SUBCALS;
                rownum += 1;

                SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                           wcfgram_mem,
                                                           FLEXE_DEMUX_TS_TO_WR_TBL_ID(timeslots[timeslot % tsnum]),
                                                           FSCL_BLOCK(unit),
                                                           FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(timeslots
                                                                                               [timeslot % tsnum]) +
                                                           subcal, &rownum));

                timeslot_plus_1 = timeslots[timeslot % tsnum] + 1;
                FLEXE_DEMUX_RDRULE_SET(rdrule, rownum - 1, &timeslot_plus_1);

                ++timeslot;

                if ((timeslot % tsnum) == 0)
                {
                    ++subcal;
                }
            }

            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       rdrule_mem,
                                                       0,
                                                       FSCL_BLOCK(unit),
                                                       channel * FLEXE_CORE_NOF_TIMESLOTS + blk * cycle_cnt + cycle,
                                                       rdrule));
        }
    }

    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, TSNUMf, tsnum);
    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, BLKf, blk_cnt - 1);;
    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, CYCLEf, cycle_cnt - 1);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, chproperty_mem, 0, FSCL_BLOCK(unit), channel, &chproperty));

    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_READY_TSr, WRITE_FSCL_READY_TS_Br, unit, read_ts);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove a channel from FlexE DEMUX
 *
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] tsmap - timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] calendar_sel - Which calendar to add this channel to
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
STATIC shr_error_e
_soc_flexe_demux_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel)
{
    SHR_BITDCL channel_map[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    soc_reg_above_64_val_t calendar_cfg;
    soc_reg_above_64_val_t own_tsgroup;
    soc_reg_above_64_val_t rdrule;
    soc_reg_above_64_val_t read_ts;
    soc_mem_t wcfgram_mem, rdrule_mem, chproperty_mem;
    uint32 chproperty = 0;
    uint8 timeslots[FLEXE_CORE_NOF_TIMESLOTS] = { 0 };
    uint32 rownum;
    uint32 channel_own_ts_group = 0;
    uint32 zero_channel_id = 0;
    int tsnum = 0;
    int timeslot;
    int subcal;

    SHR_FUNC_INIT_VARS(unit);

    if ((channel >= 0) && (channel < FLEXE_CORE_NOF_CHANNELS))
    {
        SHR_BITCLR_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
        SHR_BITSET(channel_map, channel);
    }
    else
    {
        /*
         * Delete all
         */
        SHR_BITSET_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
    }

    SOC_REG_ABOVE_64_CLEAR(calendar_cfg);
    SOC_REG_ABOVE_64_CLEAR(own_tsgroup);
    SOC_REG_ABOVE_64_CLEAR(read_ts);

    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CALENDAR_CFGr, READ_FSCL_CALENDAR_CFG_Br, unit, calendar_cfg);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CHANNEL_OWN_TSGROUPr, READ_FSCL_CHANNEL_OWN_TSGROUP_Br, unit, own_tsgroup);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_READY_TSr, READ_FSCL_READY_TS_Br, unit, read_ts);

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * Restore calendar
         */
        FLEXE_DEMUX_CALENDAR_CFG_SET(calendar_cfg, timeslot, &zero_channel_id);

        timeslots[tsnum] = timeslot;
        ++tsnum;
    }

    SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
    {
        FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_SET(own_tsgroup, channel, &channel_own_ts_group);
    }

    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CALENDAR_CFGr, WRITE_FSCL_CALENDAR_CFG_Br, unit, calendar_cfg);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CHANNEL_OWN_TSGROUPr, WRITE_FSCL_CHANNEL_OWN_TSGROUP_Br, unit,
                    own_tsgroup);

    wcfgram_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_WCFGRAMm : FSCL_WCFGRAM_Bm;
    rdrule_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_RDRULEm : FSCL_RDRULE_Bm;
    chproperty_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_CHPROPERTYm : FSCL_CHPROPERTY_Bm;

    SOC_REG_ABOVE_64_CREATE_MASK(rdrule, FLEXE_CORE_NOF_TIMESLOTS, 0);
    rownum = FLEXE_DEMUX_RAM_INVALID_ROW;

    for (timeslot = 0; timeslot < tsnum; ++timeslot)
    {
        for (subcal = 0; subcal < FLEXE_CORE_NOF_SUBCALS; ++subcal)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       wcfgram_mem,
                                                       FLEXE_DEMUX_TS_TO_WR_TBL_ID(timeslots[timeslot]),
                                                       FSCL_BLOCK(unit),
                                                       FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(timeslots[timeslot % tsnum])
                                                       + subcal, &rownum));
        }

        SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       rdrule_mem,
                                                       0,
                                                       FSCL_BLOCK(unit), channel * FLEXE_CORE_NOF_TIMESLOTS + timeslot,
                                                       &rdrule));
        }
    }

    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, TSNUMf, 0x7F);
    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, BLKf, 0xF);;
    soc_mem_field32_set(unit, FSCL_CHPROPERTYm, &chproperty, CYCLEf, 0x7F);

    SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, chproperty_mem, 0, FSCL_BLOCK(unit), channel, &chproperty));
    }

    /*
     * Clear timeslot ready
     */
    SHR_BITREMOVE_RANGE(read_ts, tsmap, 0, FLEXE_CORE_NOF_TIMESLOTS, read_ts);

    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_READY_TSr, WRITE_FSCL_READY_TS_Br, unit, read_ts);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init FlexE MUX
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
STATIC  shr_error_e
_soc_flexe_mux_init(
    int unit)
{
    soc_reg_above_64_val_t wr_cfg;
    uint32 pcs_chid_cfg = 0;
    uint32 rd_cfg = 0xF;
    uint32 reg_val = 0;
    uint32 channel = FLEXE_INVALID_CHANNEL_ID;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CREATE_MASK(wr_cfg, 8 * FLEXE_CORE_NOF_SUBCALS, 0);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_WR_CFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS * FLEXE_CORE_NOF_CHANNELS-1, &wr_cfg));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_WR_CFG_Bm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS * FLEXE_CORE_NOF_CHANNELS-1, &wr_cfg));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RD_CFGm, 0, FLEXE_CORE_NOF_SUBCALS-1, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &rd_cfg));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RD_CFG_Bm, 0, FLEXE_CORE_NOF_SUBCALS-1, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &rd_cfg));

    /*
     * PCS/FlexE disabled for all LPHYs
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PCS_ENABLEr(unit, 0));

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MODEr(unit, &reg_val));
    /*
     * Enable local 833M clock
     */
    soc_reg_field_set(unit, FSCL_FLEXE_MODEr, &reg_val, FLEXE_ENV_MODEf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MODEr(unit, reg_val));

    for (index = 0; index < FLEXE_CORE_NOF_INSTANCES_DIV2; ++index)
    {
        FLEXE_MUX_PCS_CHID_CFG_SET(&pcs_chid_cfg, index, (SHR_BITDCL *) & channel);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_CHID_CFGr(unit, 0, pcs_chid_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_CHID_CFGr(unit, 1, pcs_chid_cfg));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add bypass channel in FlexE MUX
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID mapped from BCM port
 * [in] speed - speed of the phy, should be one of:
 *                    50000
 *                    100000
 *                    200000
 *                    400000
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
STATIC shr_error_e
_soc_flexe_mux_phy_bypass_add(
    int unit,
    int flexe_port,
    int speed,
    int channel)
{
    uint32 pcs_enable_reg = 0;
    uint32 pcs_enable = 0;
    uint32 pcs_num = 0;
    uint32 pcs_sch_cfg = 0;
    uint32 pcs_chid_cfg = 0;
    uint32 num_lphys;
    uint32 pcs_sel;
    uint32 base_pcs_num;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PCS_ENABLEr(unit, &pcs_enable_reg));
    pcs_enable = soc_reg_field_get(unit, FSCL_FLEXE_PCS_ENABLEr, pcs_enable_reg, PCS_ENABLEf);
    pcs_num = soc_reg_field_get(unit, FSCL_FLEXE_PCS_ENABLEr, pcs_enable_reg, PCS_NUMf);

    num_lphys = FLEXE_PHY_NOF_LPHYS(speed);

    /*
     * PCS scheduler which schedules PHY directly. PHY is 1-based
     */
    SHR_IF_ERR_EXIT(READ_FSCL_PCS_SCH_CFGr(unit, &pcs_sch_cfg));
    pcs_sel = flexe_port + 1;

    /*
     * A flexe_port takes num_lphys in PCS scheduler
     */
    base_pcs_num = pcs_enable ? (pcs_num + 1) : pcs_num;
    for (index = base_pcs_num; index < base_pcs_num + num_lphys; ++index)
    {
        FLEXE_MUX_PCS_SCH_CFG_SET(&pcs_sch_cfg, index, &pcs_sel);
    }
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_SCH_CFGr(unit, pcs_sch_cfg));

    /*
     * Channel used for flexe_port
     */
    SHR_IF_ERR_EXIT(READ_FSCL_PCS_CHID_CFGr(unit, flexe_port % FLEXE_CORE_PCS_SEL_DIV, &pcs_chid_cfg));
    FLEXE_MUX_PCS_CHID_CFG_SET(&pcs_chid_cfg, flexe_port / FLEXE_CORE_PCS_SEL_DIV, (SHR_BITDCL *) & channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_CHID_CFGr(unit, flexe_port % FLEXE_CORE_PCS_SEL_DIV, pcs_chid_cfg));

    /** The actual pcs_num should minus one, no need to minus one if pcs is already enabled */
    pcs_num += (pcs_enable ? num_lphys : (num_lphys - 1));
    /* coverity[ptr_arith:FALSE] */
    SHR_BITSET(&pcs_enable, flexe_port);
    /*
     * Mark as bypass
     */
    soc_reg_field_set(unit, FSCL_FLEXE_PCS_ENABLEr, &pcs_enable_reg, PCS_ENABLEf, pcs_enable);

    /*
     * Increase PCS number
     */
    soc_reg_field_set(unit, FSCL_FLEXE_PCS_ENABLEr, &pcs_enable_reg, PCS_NUMf, pcs_num);

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PCS_ENABLEr(unit, pcs_enable_reg));

    /*
     * Refresh configuration to hardware
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Delete a bypass channel from FlexE MUX
 *
 *
 * [in] unit - unit ID
 * [in] flexe_port - FlexE port ID mapped from BCM port
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
STATIC shr_error_e
_soc_flexe_mux_phy_bypass_del(
    int unit,
    int flexe_port)
{
    uint32 reg_val = 0;
    uint32 pcs_enable_reg;
    uint32 pcs_enable;
    uint32 pcs_num = 0;
    uint32 pcs_sch_cfg = 0;
    uint32 pcs_chid_cfg = 0;
    uint32 channel = FLEXE_INVALID_CHANNEL_ID;
    uint32 pcs_sel = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PCS_ENABLEr(unit, &pcs_enable_reg));
    pcs_enable = soc_reg_field_get(unit, FSCL_FLEXE_PCS_ENABLEr, pcs_enable_reg, PCS_ENABLEf);

    SHR_IF_ERR_EXIT(READ_FSCL_PCS_SCH_CFGr(unit, &reg_val));
    for (index = 0; index < FLEXE_CORE_NOF_LPHYS; ++index)
    {
        FLEXE_MUX_PCS_SCH_CFG_GET(&reg_val, index, &pcs_sel);

        /*
         * Remove flexe_port from PCS scheduler
         */
        if ((pcs_sel != flexe_port + 1) && (pcs_sel != 0))
        {
            FLEXE_MUX_PCS_SCH_CFG_SET(&pcs_sch_cfg, pcs_num, &pcs_sel);
            ++pcs_num;
        }
    }
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_SCH_CFGr(unit, pcs_sch_cfg));

    /*
     * Mark as invalid channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_PCS_CHID_CFGr(unit, flexe_port % FLEXE_CORE_PCS_SEL_DIV, &pcs_chid_cfg));
    FLEXE_MUX_PCS_CHID_CFG_SET(&pcs_chid_cfg, flexe_port / FLEXE_CORE_PCS_SEL_DIV, (SHR_BITDCL *) & channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCS_CHID_CFGr(unit, flexe_port % FLEXE_CORE_PCS_SEL_DIV, pcs_chid_cfg));

    /*
     * Clear PCS enable, flexe_port in the range of 0~7
     */
    /* coverity[ptr_arith:FALSE] */
    SHR_BITCLR(&pcs_enable, flexe_port);
    soc_reg_field_set(unit, FSCL_FLEXE_PCS_ENABLEr, &pcs_enable_reg, PCS_ENABLEf, pcs_enable);

    /*
     * Decrease PCS number
     */
    pcs_num = pcs_enable ? (pcs_num - 1) : 0;
    soc_reg_field_set(unit, FSCL_FLEXE_PCS_ENABLEr, &pcs_enable_reg, PCS_NUMf, pcs_num);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PCS_ENABLEr(unit, pcs_enable_reg));

    /*
     * Refresh configuration to hardware
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Add channel in FlexE MUX.
 *
 *
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] global_ts_mask - HW global timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] global_ts_array - HW timeslot array. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] calendar_sel - Which calendar to add this channel to
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
STATIC shr_error_e
_soc_flexe_mux_channel_add(
    int unit,
    int channel,
    SHR_BITDCL global_ts_mask[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    uint32 *global_ts_array,
    int calendar_sel)
{
    soc_reg_above_64_val_t reg_val_above_64;
    soc_reg_above_64_val_t wr_cfg;
    soc_mem_t wrcfg_mem, rdcfg_mem;
    uint32 flexe_group_cfg = 0;
    uint32 reg_val = 0;
    uint32 msi_cfg = 0;
    uint32 min_lphy = 0;
    uint32 lphy;
    int timeslot;
    int tsnum = 0;
    int blk_cnt;
    int cycle_cnt;
    int blk;
    int cycle;
    int rownum;
    int col;
    int subcal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GROUP_CFGr(unit, &flexe_group_cfg));

    SHR_BIT_ITER(global_ts_mask, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * MSI cfg: the channel that takes current timeslot
         */
        FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_FLEXE_MSI_CFGr, READ_FSCL_FLEXE_MSI_CFG_Br, unit, timeslot % 20,
                        &msi_cfg);
        FLEXE_MUX_MSI_CFG_SET(&msi_cfg, timeslot, (SHR_BITDCL *) & channel);
        FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_FLEXE_MSI_CFGr, WRITE_FSCL_FLEXE_MSI_CFG_Br, unit, timeslot % 20,
                        msi_cfg);

        /*
         * Read the min lphy from the group. min_lphy is 1-based
         * min_lphy calculated from original logic timeslot
         */
        if (min_lphy == 0)
        {
            lphy = timeslot / 10;
            FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, lphy, &min_lphy);
        }
        ++tsnum;
    }

    /*
     * Set timeslot number
     */
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TSNUM_CFGr, READ_FSCL_TSNUM_CFG_Br, unit, channel % 20, &reg_val);
    FLEXE_MUX_TSNUM_CFG_SET(&reg_val, channel, (SHR_BITDCL *) & tsnum);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_TSNUM_CFGr, WRITE_FSCL_TSNUM_CFG_Br, unit, channel % 20, reg_val);

    blk_cnt = FLEXE_MAX_COMMON_DIVISOR_10(tsnum) - 1;
    cycle_cnt = tsnum / FLEXE_MAX_COMMON_DIVISOR_10(tsnum) - 1;

    /*
     * Write cycle count
     */
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CYCLE_CFGr, READ_FSCL_CYCLE_CFG_Br, unit, channel % 20, &reg_val);
    FLEXE_MUX_CYCLE_CFG_SET(&reg_val, channel, (SHR_BITDCL *) & cycle_cnt);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CYCLE_CFGr, WRITE_FSCL_CYCLE_CFG_Br, unit, channel % 20, reg_val);

    /*
     * Write block count
     */
    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_BLOCK_CFGr, READ_FSCL_BLOCK_CFG_Br, unit, reg_val_above_64);
    FLEXE_MUX_BLK_CFG_SET(reg_val_above_64, channel, (SHR_BITDCL *) & blk_cnt);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_BLOCK_CFGr, WRITE_FSCL_BLOCK_CFG_Br, unit, reg_val_above_64);
    /*
     * Write minimum LPHY of a group to channel
     */
    SHR_IF_ERR_EXIT(READ_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));
    FLEXE_MUX_CHANNEL_BELONG_FLEXE_SET(reg_val_above_64, channel, &min_lphy);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));

    rdcfg_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_RD_CFGm : FSCL_RD_CFG_Bm;
    wrcfg_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_WR_CFGm : FSCL_WR_CFG_Bm;

    timeslot = 0;
    subcal = 0;

    /*
     * Write WR cfg/rownum
     */
    for (blk = 0; blk <= blk_cnt; ++blk)
    {
        for (cycle = 0; cycle <= cycle_cnt; ++cycle)
        {
            SOC_REG_ABOVE_64_CLEAR(wr_cfg);

            for (col = 0; col < FLEXE_CORE_NOF_SUBCALS; ++col)
            {
                rownum = (blk + col) % FLEXE_CORE_NOF_SUBCALS;
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                           rdcfg_mem,
                                                           FLEXE_DEMUX_TS_TO_WR_TBL_ID(global_ts_array[timeslot % tsnum]),
                                                           FSCL_BLOCK(unit),
                                                           FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(global_ts_array
                                                                                               [timeslot % tsnum]) +
                                                           subcal, &rownum));

                FLEXE_DEMUX_RDRULE_SET(wr_cfg, rownum, &global_ts_array[timeslot % tsnum]);

                ++timeslot;

                if ((timeslot % tsnum) == 0)
                {
                    ++subcal;
                }
            }

            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       wrcfg_mem,
                                                       0,
                                                       FSCL_BLOCK(unit),
                                                       channel * FLEXE_CORE_NOF_TIMESLOTS + blk * (cycle_cnt + 1) +
                                                       cycle, &wr_cfg));
        }
    }

    /*
     * Enable timeslot
     */
    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TS_ENABLEr, READ_FSCL_TS_ENABLE_Br, unit, reg_val_above_64);
    SHR_BITOR_RANGE(reg_val_above_64, global_ts_mask, 0, FLEXE_CORE_NOF_TIMESLOTS, reg_val_above_64);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_TS_ENABLEr, WRITE_FSCL_TS_ENABLE_Br, unit, reg_val_above_64);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Remove channel from FlexE MUX.
 *
 *
 * [in] unit - unit ID
 * [in] unit - unit ID
 * [in] channel - FlexE channel ID
 * [in] global_ts_mask - HW global timeslot bitmap. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
 * [in] global_ts_array - HW global timeslot array. Each enabled bit represents a timeslot for 5G.
 *              Maximum 80 bits (400G).
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
STATIC shr_error_e
_soc_flexe_mux_channel_del(
    int unit,
    int channel,
    SHR_BITDCL global_ts_mask[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    uint32 *global_ts_array,
    int calendar_sel)
{
    SHR_BITDCL channel_map[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    soc_reg_above_64_val_t reg_val_above_64;
    soc_reg_above_64_val_t wr_cfg;
    soc_mem_t wrcfg_mem, rdcfg_mem;
    uint32 reg_val = 0;
    uint32 msi_cfg = 0;
    int timeslot;
    int tsnum = 0;
    int blk_cnt = 0;
    int cycle_cnt = 0;
    int ts_cnt = 0;
    int rownum;
    int subcal;

    SHR_FUNC_INIT_VARS(unit);

    if ((channel >= 0) && (channel < FLEXE_CORE_NOF_CHANNELS))
    {
        SHR_BITCLR_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
        SHR_BITSET(channel_map, channel);
    }
    else
    {
        /*
         * Delete all
         */
        SHR_BITSET_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
    }
    reg_val = FLEXE_INVALID_CHANNEL_ID;
    SHR_BIT_ITER(global_ts_mask, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * Restore MSI cfg
         */
        FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_FLEXE_MSI_CFGr, READ_FSCL_FLEXE_MSI_CFG_Br, unit, timeslot % 20,
                        &msi_cfg);
        FLEXE_MUX_MSI_CFG_SET(&msi_cfg, timeslot, &reg_val);
        FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_FLEXE_MSI_CFGr, WRITE_FSCL_FLEXE_MSI_CFG_Br, unit, timeslot % 20,
                        msi_cfg);
        ++tsnum;
    }

    /*
     * Disable timeslots
     */
    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TS_ENABLEr, READ_FSCL_TS_ENABLE_Br, unit, reg_val_above_64);
    SHR_BITREMOVE_RANGE(reg_val_above_64, global_ts_mask, 0, FLEXE_SAR_NOF_TIMESLOTS, reg_val_above_64);
    FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_TS_ENABLEr, WRITE_FSCL_TS_ENABLE_Br, unit, reg_val_above_64);

    SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
    {
        /*
         * Restore timeslot number
         */
        FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_TSNUM_CFGr, READ_FSCL_TSNUM_CFG_Br, unit, channel % 20, &reg_val);
        FLEXE_MUX_TSNUM_CFG_SET(&reg_val, channel, (SHR_BITDCL *) & ts_cnt);
        FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_TSNUM_CFGr, WRITE_FSCL_TSNUM_CFG_Br, unit, channel % 20, reg_val);

        /*
         * Restore cycle count
         */
        FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_CYCLE_CFGr, READ_FSCL_CYCLE_CFG_Br, unit, channel % 20, &reg_val);
        FLEXE_MUX_CYCLE_CFG_SET(&reg_val, channel, (SHR_BITDCL *) & cycle_cnt);
        FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_CYCLE_CFGr, WRITE_FSCL_CYCLE_CFG_Br, unit, channel % 20, reg_val);

        /*
         * Restore block count
         */
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        FLEXE_OP_A_OR_B(calendar_sel, READ_FSCL_BLOCK_CFGr, READ_FSCL_BLOCK_CFG_Br, unit, reg_val_above_64);
        FLEXE_MUX_BLK_CFG_SET(reg_val_above_64, channel, (SHR_BITDCL *) & blk_cnt);
        FLEXE_OP_A_OR_B(calendar_sel, WRITE_FSCL_BLOCK_CFGr, WRITE_FSCL_BLOCK_CFG_Br, unit, reg_val_above_64);
    }

    SOC_REG_ABOVE_64_CREATE_MASK(wr_cfg, FLEXE_CORE_NOF_TIMESLOTS, 0);
    rownum = 0xF;

    rdcfg_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_RD_CFGm : FSCL_RD_CFG_Bm;
    wrcfg_mem = (calendar_sel == FLEXE_DRV_CALENDAR_SEL_A) ? FSCL_WR_CFGm : FSCL_WR_CFG_Bm;

    /*
     * Restore WR cfg/rownum
     */
    for (timeslot = 0; timeslot < tsnum; ++timeslot)
    {
        for (subcal = 0; subcal < FLEXE_CORE_NOF_SUBCALS; ++subcal)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       rdcfg_mem,
                                                       FLEXE_DEMUX_TS_TO_WR_TBL_ID(global_ts_array[timeslot]),
                                                       FSCL_BLOCK(unit),
                                                       FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(global_ts_array[timeslot % tsnum])
                                                       + subcal, &rownum));
        }

        SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
        {
            SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                       wrcfg_mem,
                                                       0,
                                                       FSCL_BLOCK(unit), channel * FLEXE_CORE_NOF_TIMESLOTS + timeslot,
                                                       &wr_cfg));
        }
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init rate adapter in MUX
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
STATIC shr_error_e
_soc_flexe_mux_rateadp_init(
    int unit)
{
    uint64 adj_cfg;
    uint32 blk_num_cfg = 0;
    uint32 map_reg = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &blk_num_cfg, BLK_NUMf, 1);

    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_SEQf, 0xF);
    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_CHIDf, FLEXE_INVALID_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_ENf, 0);

    COMPILER_64_ZERO(adj_cfg);
    SHR_IF_ERR_EXIT(READ_FSCL_ADJ_CFGr(unit, 0, &adj_cfg));
    soc_reg64_field32_set(unit, FSCL_ADJ_CFGr, &adj_cfg, I_PLL_COEFFf, 0);
    soc_reg64_field32_set(unit, FSCL_ADJ_CFGr, &adj_cfg, I_CFG_FINISHf, 1);

    for (index = 0; index < FLEXE_CORE_NOF_CHANNELS; ++index)
    {
        SHR_IF_ERR_EXIT(WRITE_FSCL_BLK_NUM_CFGr(unit, index, blk_num_cfg));

        /*
         * Map timeslots to invalid channel
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAP_REGr(unit, index, map_reg));
        SHR_IF_ERR_EXIT(WRITE_FSCL_ADJ_CFGr(unit, index, adj_cfg));
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_IND_SELr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init FlexE Client subsys (Bus B, toward TinyMAC)
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
STATIC shr_error_e
_soc_flexe_client_init(
    int unit)
{
    uint32 reg_val = 0;
    uint32 init_done = 0;
    uint32 data;
    int nof_retries = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init MAC subsys (bus b)
     */
    soc_reg_field_set(unit, FSCL_REQ_GEN_PLSr, &reg_val, MAC_ENV_INITf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_PLSr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
    init_done = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, MAC_ENV_INIT_DONEf);

    /*
     * Wait for init done
     */
    while (!init_done && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
        init_done = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, MAC_ENV_INIT_DONEf);
    }

    if (!init_done)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW init done polling timeout %d\n", reg_val);
    }

    data = FLEXE_INVALID_TINYMAC_CHANNEL_ID;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_REQ_GEN_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CALENDAR * FLEXE_CORE_NOF_TIMESLOTS - 1, &data));

    data = FLEXE_TINYMAC_OH_CHANNEL_ID;
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_REQ_GEN_RAMm, 0, 0, FSCL_BLOCK(unit), FLEXE_CORE_NOF_CALENDAR * FLEXE_CORE_NOF_TIMESLOTS, FLEXE_CORE_NOF_CALENDAR * FLEXE_TINYMAC_NOF_TIMESLOTS - 1, &data));

    /*
     * Enable caledar
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_REQ_GEN_CFGr, &reg_val, CALENDAR_ENf, 1);
    soc_reg_field_set(unit, FSCL_REQ_GEN_CFGr, &reg_val, OHIF_1588_RATE_LIMIT_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_CFGr(unit, reg_val));

    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_Mr(unit, 0xC3298F74));
    SHR_IF_ERR_EXIT(WRITE_FSCL_BASE_Mr(unit, 0xC329901F));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_OHIF_1588_SRC_DSTr, &reg_val, EXTRA_SRCf, FLEXE_TINYMAC_OH_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_OHIF_1588_SRC_DSTr, &reg_val, EXTRA_DST_0f, FLEXE_TINYMAC_OH_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_OHIF_1588_SRC_DSTr, &reg_val, EXTRA_DST_1f, FLEXE_TINYMAC_1588_CHANNEL_ID);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_1588_SRC_DSTr(unit, reg_val));

    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_1588_RATIOr(unit, 853));
    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_1588_RATE_LIMIT_RATIOr(unit, 853));

    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_1588_RATE_LIMIT_Mr(unit, 0xE74DB38));
    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_1588_RATE_LIMIT_BASE_Mr(unit, 0xE74DBE3));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_EXTRA_M_Nr, &reg_val, EXTRA_Mf, 39321);
    soc_reg_field_set(unit, FSCL_EXTRA_M_Nr, &reg_val, EXTRA_BASE_Mf, 39322);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXTRA_M_Nr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   FlexE 66bswitch subsys init
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
STATIC shr_error_e
_soc_flexe_66bswitch_init(
    int unit)
{
    int destination;
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (destination = 0; destination < FLEXE_66BSWITCH_NOF_SUBSYS; destination++)
    {
        data <<= 8;
        data |= FLEXE_INVALID_66SW_CHANNEL_ID;
    }

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_SWRAMm, 0, FLEXE_66BSWITCH_NOF_SUBSYS-1, FSCL_BLOCK(unit), 0, FLEXE_CORE_MAX_NOF_CHANNELS-1, &data));

    SHR_IF_ERR_EXIT(WRITE_FSCL_PROTECT_ENr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init OH interface
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
STATIC shr_error_e
_soc_flexe_soam_init(
    int unit)
{
    soc_reg_above_64_val_t data;
    uint32 soam_lb_reg;
    uint32 soh_ins_getpkt_1;
    uint32 soam_flexe_type;
    uint32 soam_mac_type;
    uint32 oam_types[] = { 0x10, 0x27, 0x43, 0x63, 0x87, 0x30, 0xef };
    uint32 reg_val = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOH_INS_GETPKT_1r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SOH_INS_GETPKT_1r, &reg_val, SOH_INST_MODEf, 0xFF);
    soc_reg_field_set(unit, FSCL_SOH_INS_GETPKT_1r, &reg_val, SOH_INS_DRAM_INIT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOH_INS_GETPKT_1r(unit, reg_val));

    /*
    * First, clear
    */
    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_LOOPBACKr(unit, &soam_lb_reg));
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, MAC_INTE_RAM_INIT_ENf, 0);
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, FLEXE_INTE_RAM_INIT_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_LOOPBACKr(unit, soam_lb_reg));

    /*
    * Then, enable
    */
    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_LOOPBACKr(unit, &soam_lb_reg));
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, SOAM_MAC_ALM_EXT_ENf, 1);
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, SOAM_FLEXE_ALM_EXT_ENf, 1);
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, MAC_INTE_RAM_INIT_ENf, 1);
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, FLEXE_INTE_RAM_INIT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_LOOPBACKr(unit, soam_lb_reg));

    SHR_IF_ERR_EXIT(READ_FSCL_SOH_INS_GETPKT_1r(unit, &soh_ins_getpkt_1));
    soc_reg_field_set(unit, FSCL_SOH_INS_GETPKT_1r, &soh_ins_getpkt_1, SOAM_OVERTIMEf, 0xa);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOH_INS_GETPKT_1r(unit, soh_ins_getpkt_1));

    /*
     * Init FSCL_SOAM_FLEXE_TYPE_TABLE/FSCL_SOAM_MAC_TYPE_TABLE with default values
     */
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_OHIF_NOF_OAM_TYPES-1, &oam_types[6]));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_OHIF_NOF_OAM_TYPES-1, &oam_types[6]));

    /*
     * Init selected SOAM FlexE/MAC types
     */
    for (index = 0; index < FLEXE_OHIF_NOF_OAM_TYPES; ++index)
    {
        switch (index)
        {
            case 0x1:
            case 0x2:
                soam_flexe_type = soam_mac_type = oam_types[0];
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            case 0x11:
                soam_flexe_type = soam_mac_type = oam_types[1];
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            case 0x12:
                soam_flexe_type = soam_mac_type = oam_types[2];
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            case 0x13:
                soam_flexe_type = soam_mac_type = oam_types[3];
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            case 0x14:
                soam_flexe_type = soam_mac_type = oam_types[4];
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
                SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                                (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            case 0x15:
                soam_flexe_type = soam_mac_type = oam_types[5];

        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_SOAM_FLEXE_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_flexe_type));
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_SOAM_MAC_TYPE_TABLEm, 0, FSCL_BLOCK(unit), index, &soam_mac_type));
                break;
            default:
                break;
        }
    }

    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, SOAM_FLEXE_CFG_DONEf, 1);
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &soam_lb_reg, SOAM_MAC_CFG_DONEf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_LOOPBACKr(unit, soam_lb_reg));

    /*
     * Enable RX statistics
     */
    SOC_REG_ABOVE_64_CREATE_MASK(data, FLEXE_CORE_NOF_CHANNELS, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MACRX_STAT_ENABLEr(unit, data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init OAM interface
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
STATIC shr_error_e
_soc_flexe_oam_init(
    int unit)
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 flexe_rx_sfcfg_ini[] = {0x000b0002, 0xc8};
    uint32 flexe_rx_sfbeicfg_ini[] = {0x000b0002, 0xc8};
    uint32 flexe_rx_sdcfg_ini[] = {0x000b0002, 0x4e20};
    uint32 flexe_rx_sdbeicfg_ini[] = {0x000b0002, 0x4e20};
    uint32 reg_val;
    uint32 data;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_EN_CFGr(unit, reg_above64_val));
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PERIOD_SELr(unit, reg_above64_val));

    SHR_IF_ERR_EXIT(_soc_flexe_soam_init(unit));

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_JOIT_BIP_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_JOIT_BIP_CFGr, &reg_val, TS_1_DM_INSERT_ENf, 1);
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_JOIT_BIP_CFGr, &reg_val, TS_2_DMM_INSERT_ENf, 1);
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_JOIT_BIP_CFGr, &reg_val, TS_2_DMR_INSERT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_JOIT_BIP_CFGr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_ALM_ENr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_RX_ALM_ENr, &reg_val, TX_SDBIP_ALM_ENf, 0);
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_RX_ALM_ENr, &reg_val, TX_SDBEI_ALM_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_RX_ALM_ENr(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_CFG_01r, &reg_val, BAS_PERIOD_CFG_0f, 0xf);
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_CFG_01r, &reg_val, BAS_PERIOD_CFG_1f, 0x1f);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_PERIOD_CFG_01r(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_CFG_23r, &reg_val, BAS_PERIOD_CFG_2f, 0x3f);
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_CFG_23r, &reg_val, BAS_PERIOD_CFG_3f, 0x1ff);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_PERIOD_CFG_23r(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_Ar, &reg_val, BAS_PERIOD_0f, 0x18);
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_Ar, &reg_val, BAS_PERIOD_1f, 0x28);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_PERIOD_Ar(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_Br, &reg_val, BAS_PERIOD_2f, 0x48);
    soc_reg_field_set(unit, FSCL_FLEXE_BAS_PERIOD_Br, &reg_val, BAS_PERIOD_3f, 0x208);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_PERIOD_Br(unit, reg_val));

    data = FLEXE_INVALID_CHANNEL_ID;

    for (index = 0; index < FLEXE_CORE_NOF_TIMESLOTS; index++)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_FLEXE_REIXC_CFGm, 0, FSCL_BLOCK(unit), index, (uint32 *)&index));
    }
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_RX_SDCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &flexe_rx_sdcfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_RX_SDBEICFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &flexe_rx_sdbeicfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_RX_SFCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &flexe_rx_sfcfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_RX_SFBEICFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &flexe_rx_sfbeicfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_CHID_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_TIMESLOTS-1, &data));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_CLR_RAM_INITr, &reg_val, TS_RAM_INITf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_CLR_RAM_INITr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init SAR OAM interface
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
STATIC shr_error_e
_soc_flexe_sar_oam_init(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_data;
    uint32 flexe_rx_sfcfg_ini[] = {0x000b0002, 0xc8};
    uint32 flexe_rx_sfbeicfg_ini[] = {0x000b0002, 0xc8};
    uint32 flexe_rx_sdcfg_ini[] = {0x000b0002, 0x4e20};
    uint32 flexe_rx_sdbeicfg_ini[] = {0x000b0002, 0x4e20};
    uint32 reg_val;
    uint32 data;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OAM_JOIT_BIP_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_OAM_JOIT_BIP_CFGr, &reg_val, TS_1_DM_INSERT_ENf, 1);
    soc_reg_field_set(unit, FSCL_OAM_JOIT_BIP_CFGr, &reg_val, TS_2_DMM_INSERT_ENf, 1);
    soc_reg_field_set(unit, FSCL_OAM_JOIT_BIP_CFGr, &reg_val, TS_2_DMR_INSERT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OAM_JOIT_BIP_CFGr(unit, reg_val));

    /*
    * Disable OAM by default
    */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_data);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BAS_EN_CFGr(unit, reg_above_64_data));
    SHR_IF_ERR_EXIT(WRITE_FSCL_PERIOD_SELr(unit, reg_above_64_data));

    SHR_IF_ERR_EXIT(READ_FSCL_OAM_RX_ALM_ENr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_OAM_RX_ALM_ENr, &reg_val, TX_SDBIP_ALM_ENf, 0);
    soc_reg_field_set(unit, FSCL_OAM_RX_ALM_ENr, &reg_val, TX_SDBEI_ALM_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OAM_RX_ALM_ENr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_BAS_PERIOD_CFG_01r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_CFG_01r, &reg_val, BAS_PERIOD_CFG_0f, 0xF);
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_CFG_01r, &reg_val, BAS_PERIOD_CFG_1f, 0x1F);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BAS_PERIOD_CFG_01r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_BAS_PERIOD_CFG_23r(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_CFG_23r, &reg_val, BAS_PERIOD_CFG_2f, 0x3F);
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_CFG_23r, &reg_val, BAS_PERIOD_CFG_3f, 0x1FF);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BAS_PERIOD_CFG_23r(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_BAS_PERIOD_Ar(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_Ar, &reg_val, BAS_PERIOD_0f, 0x18);
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_Ar, &reg_val, BAS_PERIOD_1f, 0x28);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BAS_PERIOD_Ar(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_BAS_PERIOD_Br(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_Br, &reg_val, BAS_PERIOD_2f, 0x48);
    soc_reg_field_set(unit, FSCL_BAS_PERIOD_Br, &reg_val, BAS_PERIOD_3f, 0x208);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BAS_PERIOD_Br(unit, reg_val));

    data = FLEXE_INVALID_CHANNEL_ID;

    for (index = 0; index < FLEXE_SAR_NOF_TIMESLOTS; index++)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_REIXC_CFGm, 0, FSCL_BLOCK(unit), index, (uint32 *)&index));
    }
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RX_SDCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &flexe_rx_sdcfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RX_SDBEICFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &flexe_rx_sdbeicfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RX_SFCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &flexe_rx_sfcfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_RX_SFBEICFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &flexe_rx_sfbeicfg_ini));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CHID_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_rx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 reg_val = 0;
    int nof_instances = 1;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure interface channel adapter RX
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_rx_add(unit, bcm_port, flexe_port, speed, serdes_rate));

    /*
     * speed to number of instances
     */
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    for (instance = 0; instance < nof_instances; ++instance)
    {
        /*
         * FlexE OH RX mode. 00: 50G; 01: 100G; 10: 200G; 11: 400G
         */
        rate = FLEXE_PORT_SPEED_TO_RATE(speed);
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_RX_MODE_CFGr(unit, flexe_port + instance, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_RX_MODE_CFGr, &reg_val, FLEXE_RX_MODEf, rate);
        soc_reg_field_set(unit, FSCL_FLEXE_RX_MODE_CFGr, &reg_val, MF_16T_32_RX_CFGf,
                          (rate != FLEXE_DRV_PORT_SPEED_50G));
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_RX_MODE_CFGr(unit, flexe_port + instance, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_rx_phy_del(
    int unit,
    int bcm_port,
    int speed)
{
    uint32 reg_val = 0;
    uint32 flexe_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_RX_MAP_TABLEr(unit, &reg_val));

    /*
     * Save FlexE port that the BCM port mapped to
     */
    FLEXE_RX_MAP_TABLE_GET(&reg_val, bcm_port, &flexe_port);

    /*
     * Restore interface channel adapter RX configuration to default
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_rx_del(unit, bcm_port, speed));

#ifdef FLEXE_DRV_RESTORE_OH_CFG
    {
        uint32 flexe_rx_mode_cfg = 0;
        int instance;
        int nof_instances;

    /*
     * rate to number of instances
     */
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    for (instance = 0; instance < nof_instances; ++instance)
    {
        /*
         * Restore FlexE OH RX mode.
         */
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_RX_MODE_CFGr(unit, flexe_port + instance, &flexe_rx_mode_cfg));
        soc_reg_field_set(unit, FSCL_FLEXE_RX_MODE_CFGr, &flexe_rx_mode_cfg, FLEXE_RX_MODEf, 0);
        soc_reg_field_set(unit, FSCL_FLEXE_RX_MODE_CFGr, &flexe_rx_mode_cfg, MF_16T_32_RX_CFGf, 0);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_RX_MODE_CFGr(unit, flexe_port + instance, flexe_rx_mode_cfg));
    }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_tx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed)
{
    flexe_drv_port_speed_mode_e rate;
    uint32 soh_inst_mode;
    uint32 reg_val = 0;
    int nof_instances = 1;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert speed (50000/100000/200000/400000)  to rate (enum)
     */
    rate = FLEXE_PORT_SPEED_TO_RATE(speed);

    /*
     * Configure interface channel adapter TX
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_tx_add(unit, bcm_port, flexe_port, speed));

    /*
     * rate to number of instances
     */
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    for (instance = 0; instance < nof_instances; ++instance)
    {
        /*
         * FlexE OH TX mode. 00: 50G; 01: 100G; 10: 200G; 11: 400G
         */
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_TX_MODE_CFGr(unit, flexe_port + instance, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &reg_val, FLEXE_TX_MODEf, rate);
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &reg_val, MF_16T_32_TX_CFGf,
                          (rate != FLEXE_DRV_PORT_SPEED_50G));
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_TX_MODE_CFGr(unit, flexe_port + instance, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_SOH_INS_GETPKT_1r(unit, &reg_val));
        soh_inst_mode = soc_reg_field_get(unit, FSCL_SOH_INS_GETPKT_1r, reg_val, SOH_INST_MODEf);

        /* coverity[ptr_arith:FALSE] */
        SHR_BITWRITE(&soh_inst_mode, flexe_port + instance, (rate != FLEXE_DRV_PORT_SPEED_50G));
        soc_reg_field_set(unit, FSCL_SOH_INS_GETPKT_1r, &reg_val, SOH_INST_MODEf, soh_inst_mode);
        SHR_IF_ERR_EXIT(WRITE_FSCL_SOH_INS_GETPKT_1r(unit, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_tx_phy_del(
    int unit,
    int bcm_port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Restore interface channel adapter TX configuration to default
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_tx_del(unit, bcm_port, speed));

#ifdef FLEXE_DRV_RESTORE_OH_CFG
    {
        uint32 flexe_tx_mode_cfg = 0;
        int instance;
        int nof_instances;

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    for (instance = 0; instance < nof_instances; ++instance)
    {
        /*
         * Restore FlexE OH TX mode.
         */
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_TX_MODE_CFGr(unit, flexe_port + instance, &flexe_tx_mode_cfg));
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &flexe_tx_mode_cfg, FLEXE_TX_MODEf, 0);
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &flexe_tx_mode_cfg, MF_16T_32_TX_CFGf, 0);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_TX_MODE_CFGr(unit, flexe_port + instance, flexe_tx_mode_cfg));
    }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_rx_phy_bypass_set(
    int unit,
    int flexe_port,
    int enable,
    int bypass_channel)
{
    uint32 bypass_en = 0;
    uint64 pcschid_cfg;
    uint32 val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable/disable bypass
     */
    SHR_IF_ERR_EXIT(READ_FSCL_BYPASS_ENr(unit, &bypass_en));
    /* coverity[ptr_arith:FALSE] */
    enable ? SHR_BITSET(&bypass_en, flexe_port) : SHR_BITCLR(&bypass_en, flexe_port);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BYPASS_ENr(unit, bypass_en));

    /*
     * Map FlexE port to channel
     */
    COMPILER_64_ZERO(pcschid_cfg);
    SHR_IF_ERR_EXIT(READ_FSCL_PCSCHID_CFGr(unit, &pcschid_cfg));
    val = enable ? bypass_channel : FLEXE_INVALID_66SW_CHANNEL_ID;

    /* coverity[incompatible_cast:FALSE] */
    FLEXE_CHANNELIZE_PCS_CHID_CFG_SET(&pcschid_cfg, flexe_port, &val);
    SHR_IF_ERR_EXIT(WRITE_FSCL_PCSCHID_CFGr(unit, pcschid_cfg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_tx_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel)
{
    int is_bypass;
    flexe_drv_port_speed_mode_e rate;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable/disable bypass in interface adapter TX
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_tx_bypass_set(unit, flexe_port, speed, enable, bypass_channel));

    if (enable)
    {
        /*
         * Add bypass in MUX
         */
        SHR_IF_ERR_EXIT(_soc_flexe_mux_phy_bypass_add(unit, flexe_port, speed, bypass_channel));
    }
    else
    {
        /*
         * Remove bypass in MUX
         */
        SHR_IF_ERR_EXIT(_soc_flexe_mux_phy_bypass_del(unit, flexe_port));
    }

    rate = FLEXE_PORT_SPEED_TO_RATE(speed);

    /*
     * Configure FlexE port FIFO
     */
    is_bypass = enable ? 1 : 0;
    SHR_IF_ERR_EXIT(soc_flexe_std_env_set(unit, flexe_port, rate, is_bypass, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_rx_phy_add(unit, bcm_port, flexe_port, speed, serdes_rate));
    SHR_IF_ERR_EXIT(soc_flexe_std_tx_phy_add(unit, bcm_port, flexe_port, speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_phy_del(
    int unit,
    int bcm_port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_rx_phy_del(unit, bcm_port, speed));
    SHR_IF_ERR_EXIT(soc_flexe_std_tx_phy_del(unit, bcm_port, speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_rx_phy_bypass_set(unit, flexe_port, enable, bypass_channel));
    SHR_IF_ERR_EXIT(soc_flexe_std_tx_phy_bypass_set(unit, flexe_port, speed, enable, bypass_channel));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_rx_init(
    int unit)
{
    uint32 data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init SAR RX
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_rx_init(unit));

    /*
     * Init buffer
     */
    SHR_IF_ERR_EXIT(_soc_flexe_cpb_init(unit));

    /*
     * Init MCMAC for statistics
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mcmac_init(unit));

    data = FLEXE_INVALID_CHANNEL_ID;

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CHID_RAMm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_SAR_NOF_TIMESLOTS-1, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_tx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init SAR TX
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_tx_init(unit));

    /*
     * Init MCMAC rate adapter
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mac_rateadp_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_init(
    int unit)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_rx_init(unit));
    SHR_IF_ERR_EXIT(soc_flexe_std_sar_tx_init(unit));
    /*
     * Clear alarms
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_GLB_CLRr, &reg_val, GLB_ALM_CLRf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_GLB_CLRr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add SAR channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_channel_rx_add(unit, channel, tsmap));

    SHR_BIT_ITER(tsmap, FLEXE_SAR_NOF_TIMESLOTS, index)
    {
        /*
         * Add channel mapping for OAM
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_CHID_RAMm, 0, FSCL_BLOCK(unit), index, &channel));
    }

    /*
     * Add MCMAC channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mcmac_channel_tx_add(unit, channel, tsmap));

    /*
     * Add CPB channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_cpb_channel_add(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove SAR channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_channel_rx_del(unit, channel, tsmap));

    /*
     * Remove MCMAC channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mcmac_channel_tx_del(unit, channel, tsmap));

    /*
     * Remove CPB channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_cpb_channel_del(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add MCMAC channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mcmac_channel_rx_set(unit, channel, 1));

    /*
     * Add SAR channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_channel_tx_add(unit, channel, tsmap));

    /*
     * Add rate adapter channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mac_rateadp_channel_add(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove SAR channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_channel_tx_del(unit, channel, tsmap));

    /*
     * Remove MCMAC channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mcmac_channel_rx_set(unit, channel, 0));

    /*
     * Remove rate adapter channel mapping
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mac_rateadp_channel_del(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_rx_add(unit, channel, tsmap));
    SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_tx_add(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_rx_del(unit, channel, tsmap));
    SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_tx_del(unit, channel, tsmap));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_map_rx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, channel, &reg_val));

    if (enable)
    {
        /*
         * Map SAR channel to ILKN channel
         */
        soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_CHAN_MAPf, ilkn_channel);
        soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, MON_ENf, 1);
    }
    else
    {
        /*
         * Map SAR channel to invalid channel
         */
        soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_CHAN_MAPf, FLEXE_INVALID_ILKN_CHANNEL_ID);
        soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, MON_ENf, 0);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_map_tx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_CTRL_CFGr(unit, channel, &reg_val));

    if (enable)
    {
        /*
         * Map SAR channel to ILKN channel
         */
        soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, TX_CHAN_MAPf, ilkn_channel);
        soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, SEG_ENf, 1);
    }
    else
    {
        /*
         * Map SAR channel to invalid channel
         */
        soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, TX_CHAN_MAPf, FLEXE_INVALID_ILKN_CHANNEL_ID);
        soc_reg_field_set(unit, FSCL_SAR_TX_CTRL_CFGr, &reg_val, SEG_ENf, 0);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_CTRL_CFGr(unit, channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_channel_map_set(
    int unit,
    uint32 flags,
    int channel,
    int ilkn_channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_map_rx_set(unit, channel, ilkn_channel, enable));
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_flexe_std_sar_channel_map_tx_set(unit, channel, ilkn_channel, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_loopback_set(
    int unit,
    flexe_drv_sar_loopback_mode_e mode,
    int enable)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    switch (mode)
    {
        case FLEXE_DRV_SAR_LOOPBACK_REMOTE:
            SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_GLB_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, BDCOME_LOOPBACKf, enable ? 1 : 0);
            SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_GLB_CFGr(unit, reg_val));
            break;
        case FLEXE_DRV_SAR_LOOPBACK_RX:
            SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_GLB_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, RX_LOOPBACKf, enable ? 1 : 0);
            SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_GLB_CFGr(unit, reg_val));
            break;
        case FLEXE_DRV_SAR_LOOPBACK_TX:
            SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_GLB_CFGr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, TX_LOOPBACKf, enable ? 1 : 0);
            SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_GLB_CFGr(unit, reg_val));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_env_set(
    int unit,
    int flexe_port,
    int phy_rate,
    int bypass,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        if (!bypass)
        {
            /*
             * FlexE mode
             */
            switch (phy_rate)
            {
                case FLEXE_DRV_PORT_SPEED_50G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_50G, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_100G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_100G, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_200G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_200G, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_400G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_400G, 1));
                    break;
                default:
                    break;
            }
        }
        else
        {
            /*
             * Bypass mode
             */
            switch (phy_rate)
            {
                case FLEXE_DRV_PORT_SPEED_50G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_50G_BYPASS, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_100G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_100G_BYPASS, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_200G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_200G_BYPASS, 1));
                    break;
                case FLEXE_DRV_PORT_SPEED_400G:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_400G_BYPASS, 1));
                    break;
                default:
                    SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, FLEXE_ENV_50G_BYPASS, 1));
                    break;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(_soc_flexe_env_cfg(unit, flexe_port, 0, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_tmac_calendar_enable_set(
    int unit,
    int enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable/disable calendar for bus b
     */
    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_REQ_GEN_CFGr, &reg_val, CALENDAR_ENf, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_CFGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_mux_rateadp_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)])
{
    uint32 reg_val = 0;
    uint32 num_ts = 0;
    uint32 algo_sel = 0;
    int index;
    uint32 fifo_level[2][80] = {
        {
         0x0d0b0702, 0x19140c03, 0x22180e04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482808, 0x6c583109,
         0x7864370a, 0x8c78440f, 0x9c884c0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         },
        {
         0x0d0b0702, 0x19140c03, 0x22180f04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482908, 0x6c583109,
         0x7864370a, 0x8c78410a, 0x9c88490a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         }
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, index)
    {
        ++num_ts;
    }

    SHR_IF_ERR_EXIT(READ_FSCL_IND_SELr(unit, &reg_val));
    algo_sel = soc_reg_field_get(unit, FSCL_IND_SELr, reg_val, IND_SELf);

    /*
     * Calculate FIFO level according to number of timeslots
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_FIFO_LEVEL_CFGr(unit, channel, fifo_level[algo_sel & 0x1][num_ts - 1]));

    SHR_IF_ERR_EXIT(READ_FSCL_BLK_NUM_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 0);
    soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &reg_val, BLK_NUMf, MIN(num_ts, FLEXE_RATEADP_MAX_NOF_TIMESLOTS));
    SHR_IF_ERR_EXIT(WRITE_FSCL_BLK_NUM_CFGr(unit, channel, reg_val));

    num_ts = 0;

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, index)
    {
        reg_val = 0;
        soc_reg_field_set(unit, FSCL_MAP_REGr, &reg_val, MAP_SEQf, num_ts);
        soc_reg_field_set(unit, FSCL_MAP_REGr, &reg_val, MAP_CHIDf, channel);
        soc_reg_field_set(unit, FSCL_MAP_REGr, &reg_val, MAP_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAP_REGr(unit, index, reg_val));

        /*
         * Up to 16 FIFO blocks
         */
        if (++num_ts >= FLEXE_RATEADP_MAX_NOF_TIMESLOTS)
        {
            break;
        }
    }

    SHR_IF_ERR_EXIT(READ_FSCL_BLK_NUM_CFGr(unit, channel, &reg_val));
    soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_BLK_NUM_CFGr(unit, channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_mux_rateadp_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)])
{
    SHR_BITDCL channel_map[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)];
    uint32 reg_val = 0;
    uint32 map_reg = 0;
    uint32 num_ts = 0;
    uint32 algo_sel = 0;
    int index;
    uint32 fifo_level[2][80] = {
        {
         0x0d0b0702, 0x19140c03, 0x22180e04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482808, 0x6c583109,
         0x7864370a, 0x8c78440f, 0x9c884c0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f, 0xa08c4e0f,
         0xa08c4e0f, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5014, 0xa08c5014, 0xa08c5014,
         0xa08c5014, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c5319, 0xa08c5319,
         0xa08c5319, 0xa08c5319, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         0xa08c551e, 0xa08c551e, 0xa08c551e, 0xa08c551e,
         },
        {
         0x0d0b0702, 0x19140c03, 0x22180f04, 0x2c221405,
         0x3c2d1a06, 0x4c382007, 0x5c482908, 0x6c583109,
         0x7864370a, 0x8c78410a, 0x9c88490a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a, 0xa08c4b0a,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f, 0xa08c4b0f,
         0xa08c4b0f, 0xa08c4b0f, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         0xa08c4b14, 0xa08c4b14, 0xa08c4b14, 0xa08c4b14,
         }
    };

    SHR_FUNC_INIT_VARS(unit);

    if ((channel >= 0) && (channel < FLEXE_CORE_NOF_CHANNELS))
    {
        SHR_BITCLR_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
        SHR_BITSET(channel_map, channel);
    }
    else
    {
        /*
         * Delete all
         */
        SHR_BITSET_RANGE(channel_map, 0, FLEXE_CORE_NOF_CHANNELS);
    }

    SHR_IF_ERR_EXIT(READ_FSCL_IND_SELr(unit, &reg_val));
    algo_sel = soc_reg_field_get(unit, FSCL_IND_SELr, reg_val, IND_SELf);

    SHR_BIT_ITER(channel_map, FLEXE_CORE_NOF_CHANNELS, channel)
    {
        /*
         * Restore FIFO level configuration
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_FIFO_LEVEL_CFGr(unit, channel, fifo_level[algo_sel & 0x1][0]));

        SHR_IF_ERR_EXIT(READ_FSCL_BLK_NUM_CFGr(unit, channel, &reg_val));
        soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &reg_val, CHAN_ENf, 0);
        soc_reg_field_set(unit, FSCL_BLK_NUM_CFGr, &reg_val, BLK_NUMf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_BLK_NUM_CFGr(unit, channel, reg_val));
    }

    num_ts = 0;

    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_SEQf, 0xF);
    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_CHIDf, FLEXE_INVALID_CHANNEL_ID);
    soc_reg_field_set(unit, FSCL_MAP_REGr, &map_reg, MAP_ENf, 0);

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_CHANNELS, index)
    {
        /*
         * Restore map_reg
         */
        SHR_IF_ERR_EXIT(WRITE_FSCL_MAP_REGr(unit, index, map_reg));

        if (++num_ts >= FLEXE_RATEADP_MAX_NOF_TIMESLOTS)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_rx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed)
{
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    int nof_instances;
    uint32 reg_val = 0;
    uint32 grp_mode;
    int phy;
    int instance;
    int is_100g;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * PHY bitmap to intance bitmap
     */
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    SHR_BITCLR_RANGE(instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        for (instance = phy; instance < phy + nof_instances; ++instance)
        {
            /*
             * Add instances to group for deskew
             */
            SHR_BITSET(instance_bitmap, instance);
        }
    }

    /*
     * Configure demux
     */
    SHR_IF_ERR_EXIT(_soc_flexe_demux_phy_add(unit, group, instance_bitmap, lphys, speed));

    if (FLEXE_GROUP_IS_VALID(group))
    {
        is_100g = (speed >= FLEXE_PHY_SPEED_100G);

        /*
         * Configure group mode
         */
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GRP_MODEr(unit, &reg_val));
        grp_mode = soc_reg_field_get(unit, FSCL_FLEXE_GRP_MODEr, reg_val, GRP_MODEf);

        /* coverity[ptr_arith:FALSE] */
        is_100g ? SHR_BITSET(&grp_mode, group) : SHR_BITCLR(&grp_mode, group);
        soc_reg_field_set(unit, FSCL_FLEXE_GRP_MODEr, &reg_val, GRP_MODEf, grp_mode);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_GRP_MODEr(unit, reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_rx_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed)
{
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    int nof_instances;
    int phy;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * PHY bitmap to intance bitmap
     */
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    SHR_BITCLR_RANGE(instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        for (instance = phy; instance < phy + nof_instances; ++instance)
        {
            /*
             * Add instances to group for deskew
             */
            SHR_BITSET(instance_bitmap, instance);
        }
    }

    /*
     * Restore demux configuration
     */
    SHR_IF_ERR_EXIT(_soc_flexe_demux_phy_remove(unit, group, instance_bitmap, lphy_bitmap, speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
STATIC uint32
_soc_flexe_mux_find_sort_lphy(
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)])
{
    uint32 min_lphy = 0;
    int ii;

    /*
     * Sort group phy acording to 1->5->2->6->3->7->4->8
     */
    for (ii = 0; ii < FLEXE_CORE_NOF_PORTS / 2; ++ii)
    {
        if (SHR_BITGET(lphy_bitmap, ii))
        {
            /** 1-based */
            min_lphy = ii + 1;
            break;
        }
        else if (SHR_BITGET(lphy_bitmap, FLEXE_CORE_NOF_PORTS / 2 + ii))
        {
            /** 1-based */
            min_lphy = FLEXE_CORE_NOF_PORTS / 2 + ii + 1;
            break;
        }
    }

    return min_lphy;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_mux_lphy_config_set(
    int unit,
    int flexe_core_port,
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed,
    int enable)
{
    int nof_instances, nof_lphys;
    int index, lphy_index, instance;
    uint32 sch_cfg, val, flexe_mode, pcs_enable_reg = 0, flexe_enable;
    uint32 flexe_mode_reg = 0, cphy2_cfg = 0, en_sel_cfg = 0, inst_sel_cfg = 0;
    uint32 flexe_group_cfg = 0, min_lphy, phy_sel_cfg = 0;
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MODEr(unit, &flexe_mode_reg));
    flexe_mode = soc_reg_field_get(unit, FSCL_FLEXE_MODEr, flexe_mode_reg, FLEXE_MODEf);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PCS_ENABLEr(unit, &pcs_enable_reg));
    flexe_enable = soc_reg_field_get(unit, FSCL_FLEXE_PCS_ENABLEr, pcs_enable_reg, FLEXE_ENABLEf);

    SHR_IF_ERR_EXIT(READ_FSCL_CPHY_2_CFGr(unit, &cphy2_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_EN_SEL_CFGr(unit, &en_sel_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_INST_SEL_CFGr(unit, &inst_sel_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_SCH_CFGr(unit, &sch_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GROUP_CFGr(unit, &flexe_group_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_PHY_SEL_CFGr(unit, &phy_sel_cfg));

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);
    nof_lphys = (speed >= FLEXE_PHY_SPEED_100G) ? 2 : 1;
    lphy_index = 0;
    SHR_BITCLR_RANGE(lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS);

    for (instance = flexe_core_port; instance < flexe_core_port + nof_instances; ++instance)
    {
        for (index = 0; (index < nof_lphys) && (lphy_index < FLEXE_CORE_NOF_LPHYS); ++index)
        {
            if (index == 0)
            {
                val = enable ? ((speed >= FLEXE_PHY_SPEED_100G) ? FLEXE_MUX_MODE_100G : FLEXE_MUX_MODE_50G) : 0;
                FLEXE_MUX_MODE_SET(&flexe_mode, lphys[lphy_index], &val);
                /*
                 * All 100G instances of a FlexE PHY selects 'EN' from base PHY
                 */
                val = enable ? flexe_core_port : lphys[lphy_index];
                FLEXE_MUX_EN_SEL_CFG(&en_sel_cfg, lphys[lphy_index], &val);
                /*
                 * FlexE instance selects the 1st LPHY of a instance
                 */
                val = enable ? lphys[lphy_index] : instance;
                FLEXE_MUX_INST_SEL_CFG_SET(&inst_sel_cfg, instance, &val);
            }
            else
            {
                val = enable ? FLEXE_MUX_MODE_100G_2ND_PHY : 0;
                FLEXE_MUX_MODE_SET(&flexe_mode, lphys[lphy_index], &val);
                /*
                 * Set 2nd LPHY for base LPHY
                 */
                val = enable ? lphys[lphy_index] : lphys[lphy_index - 1];
                FLEXE_MUX_CPHY2_CFG_SET(&cphy2_cfg, lphys[lphy_index - 1], &val);
            }
            val = enable ? (1 << (lphys[lphy_index] % 4)) : 0;
            FLEXE_MUX_SCH_CFG_SET(&sch_cfg, lphys[lphy_index], &val);
            /* coverity[ptr_arith:FALSE] */
            SHR_BITWRITE(&flexe_enable, lphys[lphy_index], enable);
            /*
             * Record LPHYs for sub-sequent processing
             */
            SHR_BITSET(lphy_bitmap, lphys[lphy_index]);
            ++lphy_index;
        }
        val = enable ? (flexe_core_port + 1) : 0;
        FLEXE_PHY_SEL_CFG_SET(&phy_sel_cfg, instance, &val);
    }

    soc_reg_field_set(unit, FSCL_FLEXE_MODEr, &flexe_mode_reg, FLEXE_MODEf, flexe_mode);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MODEr(unit, flexe_mode_reg));

    soc_reg_field_set(unit, FSCL_FLEXE_PCS_ENABLEr, &pcs_enable_reg, FLEXE_ENABLEf, flexe_enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PCS_ENABLEr(unit, pcs_enable_reg));

    SHR_IF_ERR_EXIT(WRITE_FSCL_CPHY_2_CFGr(unit, cphy2_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_EN_SEL_CFGr(unit, en_sel_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_INST_SEL_CFGr(unit, inst_sel_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_SCH_CFGr(unit, sch_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_PHY_SEL_CFGr(unit, phy_sel_cfg));

    min_lphy = _soc_flexe_mux_find_sort_lphy(lphy_bitmap);
    for (index = 0; index < lphy_index; ++index)
    {
        FLEXE_MUX_GROUP_CFG_SET(&flexe_group_cfg, lphys[index], &min_lphy);
    }
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_GROUP_CFGr(unit, flexe_group_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_tx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed)
{
    soc_reg_t grp_cfg_regs[] = { FSCL_FLEXE_INSTANCE_GRP_CFG_0r, FSCL_FLEXE_INSTANCE_GRP_CFG_1r };
    SHR_BITDCL grp_instances[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    SHR_BITDCL instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)];
    uint32 min_instance_plus_1 = FLEXE_INVALID_PORT_ID;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 phy_sel_cfg = 0;
    uint32 grp_cfg = 0;
    uint32 flexe_group_cfg = 0;
    uint32 inst_sel_cfg = 0;
    uint32 instance;
    uint32 lphy = 0;
    uint32 lphy_index;
    uint32 min_lphy = 0;
    uint32 grp_min_lphy = 0;
    uint32 nof_instances;
    uint32 nof_lphys;
    uint32 phy;
    uint32 val;
    int index, ii;

    SHR_FUNC_INIT_VARS(unit);

    nof_lphys = (speed >= FLEXE_PHY_SPEED_100G) ? 2 : 1;
    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);

    lphy_index = 0;
    SHR_BITCLR_RANGE(lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS);
    SHR_BITCLR_RANGE(instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);
    /*
     * Get instance and lphy bitmap
     */
    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        for (instance = phy; instance < phy + nof_instances; ++instance)
        {
            for (index = 0; index < nof_lphys; ++index)
            {
                SHR_BITSET(lphy_bitmap, lphys[lphy_index]);
                ++lphy_index;
            }
            SHR_BITSET(instance_bitmap, instance);
        }
    }
    /*
     * Find the minimum instance in group
     */
    SHR_BITCLR_RANGE(grp_instances, 0, FLEXE_CORE_NOF_INSTANCES);
    SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[group / 4], REG_PORT_ANY, 0, &grp_cfg));
    FLEXE_MULTI_DESKEW_GRP_CFG_GET(&grp_cfg, group, grp_instances);
    /*
     * Clear the instances we are adding, since those instances might be just added in RX function
     */
    SHR_BITREMOVE_RANGE(grp_instances, instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES, grp_instances);

    SHR_IF_ERR_EXIT(READ_FSCL_INST_SEL_CFGr(unit, &inst_sel_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GROUP_CFGr(unit, &flexe_group_cfg));
    /*
     * Get first instance
     */
    min_lphy = _soc_flexe_mux_find_sort_lphy(lphy_bitmap);
    instance = shr_bitop_first_bit_get(grp_instances, FLEXE_CORE_NOF_INSTANCES);
    if (instance != -1)
    {
        min_instance_plus_1 = instance + 1;
        /*
         * Find the minimum LPHY in group
         */
        FLEXE_MUX_INST_SEL_CFG_GET(&inst_sel_cfg, instance, &lphy);
        FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, lphy, &grp_min_lphy);

        if ((((min_lphy - 1) % (FLEXE_CORE_NOF_PORTS / 2)) < ((grp_min_lphy - 1) % (FLEXE_CORE_NOF_PORTS / 2)))
            || ((((min_lphy - 1) % (FLEXE_CORE_NOF_PORTS / 2)) == ((grp_min_lphy - 1) % (FLEXE_CORE_NOF_PORTS / 2)))
            && (min_lphy < grp_min_lphy)))
        {
            val = 0;

            for (ii = 0; ii < FLEXE_CORE_NOF_LPHYS; ++ii)
            {
                FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, ii, &val);
                if (val == grp_min_lphy)
                {
                    /*
                     * Update min LPHY for existing LPHYs
                     */
                    FLEXE_MUX_GROUP_CFG_SET(&flexe_group_cfg, ii, &min_lphy);
                }
            }
            /*
             * Update min LPHY for existing channels
             */
            SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
            SHR_IF_ERR_EXIT(READ_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));

            val = 0;
            for (ii = 0; ii < FLEXE_CORE_NOF_CHANNELS; ++ii)
            {
                FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(reg_val_above_64, ii, &val);
                if (val == grp_min_lphy)
                {
                    FLEXE_MUX_CHANNEL_BELONG_FLEXE_SET(reg_val_above_64, ii, &min_lphy);
                }
            }
            SHR_IF_ERR_EXIT(WRITE_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));
        }
        else
        {
            min_lphy = grp_min_lphy;
        }
    }
    SHR_BIT_ITER(lphy_bitmap, FLEXE_CORE_NOF_LPHYS, lphy)
    {
        /*
         * Align all LPHYs in a group to minimum LPHY
         */
        FLEXE_MUX_GROUP_CFG_SET(&flexe_group_cfg, lphy, &min_lphy);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_GROUP_CFGr(unit, flexe_group_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));
    /*
     * Configure group phy selection for adding PADs
     */
    SHR_IF_ERR_EXIT(READ_FSCL_PHY_SEL_CFGr(unit, &phy_sel_cfg));
    SHR_BIT_ITER(instance_bitmap, FLEXE_CORE_NOF_INSTANCES, instance)
    {
        if (instance + 1 < min_instance_plus_1)
        {
            if (min_instance_plus_1 != FLEXE_INVALID_PORT_ID)
            {
                /*
                 * Minimum instance changed for an existing group, update minimum instance for existing instances
                 */
                val = 0;
                for (ii = 0; ii < FLEXE_CORE_NOF_PORTS; ++ii)
                {
                    FLEXE_PHY_SEL_CFG_GET(&phy_sel_cfg, ii, &val);
                    if (val == min_instance_plus_1)
                    {
                        val = instance + 1;
                        FLEXE_PHY_SEL_CFG_SET(&phy_sel_cfg, ii, &val);
                    }
                }
            }
            /*
             * 1-based
             */
            min_instance_plus_1 = instance + 1;
        }
        FLEXE_PHY_SEL_CFG_SET(&phy_sel_cfg, instance, &min_instance_plus_1);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_PHY_SEL_CFGr(unit, phy_sel_cfg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_tx_remove(
    int unit,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed)
{
    SHR_BITDCL grp_instance_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_INSTANCES)];
    SHR_BITDCL grp_lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)];
    SHR_BITDCL lphy_bitmap_tmp[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)];
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 phy_sel_cfg = 0;
    uint32 min_instance = 0;
    uint32 grp_min_lphy = 0;
    uint32 min_lphy = 0;
    uint32 nof_instances;
    uint32 instance;
    uint32 flexe_group_cfg = 0;
    uint32 inst_sel_cfg = 0;
    uint32 phy;
    uint32 lphy, lphy_tmp;
    uint32 val;
    uint32 ii;
    int nof_lphys_per_phy, nof_lphys_per_instance;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_INST_SEL_CFGr(unit, &inst_sel_cfg));
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GROUP_CFGr(unit, &flexe_group_cfg));
    /*
     * Find minimum LPHY for group
     */
    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        lphy = 0;
        FLEXE_MUX_INST_SEL_CFG_GET(&inst_sel_cfg, phy, &lphy);
        FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, lphy, &grp_min_lphy);
        break;
    }

    if (grp_min_lphy > 0)
    {
        /*
         * Check if minimum LPHY is being removed from the group.
         * If minimum LPHY is being removed, update minimum LPHY for remaining LPHYs
         */
        if (SHR_BITGET(lphy_bitmap, (grp_min_lphy-1)))
        {
            SHR_BITCLR_RANGE(grp_lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS);

            /*
             * Get LPHY bitmap for group
             */
            lphy = 0;

            for (ii=0; ii<FLEXE_CORE_NOF_LPHYS; ++ii)
            {
                FLEXE_MUX_GROUP_CFG_GET(&flexe_group_cfg, ii, &lphy);
                if (lphy == grp_min_lphy)
                {
                    SHR_BITSET(grp_lphy_bitmap, ii);
                }
            }

            /*
             * Clear removed LPHYs from group
             */
            SHR_BITREMOVE_RANGE(grp_lphy_bitmap, lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS, grp_lphy_bitmap);

            min_lphy = _soc_flexe_mux_find_sort_lphy(grp_lphy_bitmap);

            /*
             * Update min LPHY for remaining LPHYs
             */
            SHR_BIT_ITER(grp_lphy_bitmap, FLEXE_CORE_NOF_LPHYS, lphy)
            {
                FLEXE_MUX_GROUP_CFG_SET(&flexe_group_cfg, lphy, &min_lphy);
            }

            /*
             * Update min LPHY for existing channels
             */
            SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
            SHR_IF_ERR_EXIT(READ_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));

            for (ii = 0; ii < FLEXE_CORE_NOF_CHANNELS; ++ii)
            {
                val = 0;
                FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(reg_val_above_64, ii, &val);
                if (val == grp_min_lphy)
                {
                    FLEXE_MUX_CHANNEL_BELONG_FLEXE_SET(reg_val_above_64, ii, &min_lphy);
                }
            }

            SHR_IF_ERR_EXIT(WRITE_FSCL_CH_BELONG_FLEXEr(unit, reg_val_above_64));
        }
    }

    nof_instances = FLEXE_PHY_NOF_INSTANCES(speed);
    nof_lphys_per_instance = (speed >= FLEXE_PHY_SPEED_100G) ? 2 : 1;

    nof_lphys_per_phy = nof_instances * nof_lphys_per_instance;
    ii = 0;
    SHR_BITCLR_RANGE(lphy_bitmap_tmp, 0, FLEXE_CORE_NOF_LPHYS);
    SHR_BIT_ITER(lphy_bitmap, FLEXE_CORE_NOF_LPHYS, lphy)
    {
        SHR_BITSET(lphy_bitmap_tmp, lphy);
        ++ii;
        if (ii == nof_lphys_per_phy)
        {
            min_lphy = _soc_flexe_mux_find_sort_lphy(lphy_bitmap_tmp);
            SHR_BIT_ITER(lphy_bitmap_tmp, FLEXE_CORE_NOF_PORTS, lphy_tmp)
            {
                FLEXE_MUX_GROUP_CFG_SET(&flexe_group_cfg, lphy_tmp, &min_lphy);
            }
            ii = 0;
            SHR_BITCLR_RANGE(lphy_bitmap_tmp, 0, FLEXE_CORE_NOF_LPHYS);
        }
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_GROUP_CFGr(unit, flexe_group_cfg));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

    /*
     * Restore group phy selection
     */
    SHR_IF_ERR_EXIT(READ_FSCL_PHY_SEL_CFGr(unit, &phy_sel_cfg));

    /*
     * Find minimum instance for group
     */
    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        FLEXE_PHY_SEL_CFG_GET(&phy_sel_cfg, phy, &min_instance);
        break;
    }

    if (min_instance > 0)
    {
        /*
         * Check if minimum LPHY is being removed from the group.
         * If minimum LPHY is being removed, update minimum LPHY for remaining LPHYs
         */
        if (SHR_BITGET(phy_bitmap, (min_instance-1)))
        {
            SHR_BITCLR_RANGE(grp_instance_bitmap, 0, FLEXE_CORE_NOF_INSTANCES);

            /*
             * Get LPHY bitmap for group
             */
            instance = 0;

            for (ii=0; ii<FLEXE_CORE_NOF_INSTANCES; ++ii)
            {
                FLEXE_PHY_SEL_CFG_GET(&phy_sel_cfg, ii, &instance);

                if (instance == min_instance)
                {
                    SHR_BITSET(grp_instance_bitmap, ii);
                }
            }

            /*
             * Clear removed instances from group
             */
            SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
            {
                for (instance = phy; instance < phy + nof_instances; ++instance)
                {
                    SHR_BITCLR(grp_instance_bitmap, instance);
                }
            }

            /*
             * Calculate the new minimum instance for the group
             */
            SHR_BIT_ITER(grp_instance_bitmap, FLEXE_CORE_NOF_INSTANCES, instance)
            {
                min_instance = instance + 1;
                break;
            }

            /*
             * Update minimum instance for remaining instances in the group
             */
            SHR_BIT_ITER(grp_instance_bitmap, FLEXE_CORE_NOF_INSTANCES, instance)
            {
                FLEXE_PHY_SEL_CFG_SET(&phy_sel_cfg, instance, &min_instance);
            }
        }
    }

    SHR_BIT_ITER(phy_bitmap, FLEXE_CORE_NOF_PORTS, phy)
    {
        for (instance = phy; instance < phy + nof_instances; ++instance)
        {
            min_instance = phy + 1;
            FLEXE_PHY_SEL_CFG_SET(&phy_sel_cfg, instance, &min_instance);
        }
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_PHY_SEL_CFGr(unit, phy_sel_cfg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_group_rx_add(unit, group, phy_bitmap, lphys, speed));
    SHR_IF_ERR_EXIT(soc_flexe_std_group_tx_add(unit, group, phy_bitmap, lphys, speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_group_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_group_rx_remove(unit, group, phy_bitmap, lphy_bitmap, speed));
    SHR_IF_ERR_EXIT(soc_flexe_std_group_tx_remove(unit, phy_bitmap, lphy_bitmap, speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel)
{
    soc_reg_above_64_val_t reg_above64_val;
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add channel mapping in demux
     */
    SHR_IF_ERR_EXIT(_soc_flexe_demux_channel_add(unit, channel, tsmap, calendar_sel));

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * Add channel mapping for FlexE OAM
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_FLEXE_CHID_RAMm, 0, FSCL_BLOCK(unit), timeslot, &channel));
    }

    /*
     * Clear bypass bit for channel
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above64_val));
    SHR_BITCLR(reg_above64_val, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel)
{
    int invalid_channel = FLEXE_INVALID_CHANNEL_ID;
    int timeslot;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove channel mapping in demux
     */
    SHR_IF_ERR_EXIT(_soc_flexe_demux_channel_del(unit, channel, tsmap, calendar_sel));

    SHR_BIT_ITER(tsmap, FLEXE_CORE_NOF_TIMESLOTS, timeslot)
    {
        /*
         * Remove channel mapping for FlexE OAM
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_FLEXE_CHID_RAMm, 0, FSCL_BLOCK(unit), timeslot, &invalid_channel));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL global_ts_mask[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    uint32 *global_ts_array,
    int calendar_sel)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add channel mapping in mux
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mux_channel_add(unit, channel, global_ts_mask, global_ts_array, calendar_sel));

    /*
     * Remove FlexE OAM bypass flag for channel
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above64_val));
    SHR_BITCLR(reg_above64_val, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL global_ts_mask[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    uint32 *global_ts_array,
    int calendar_sel)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove channel mapping in mux
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mux_channel_del(unit, channel, global_ts_mask, global_ts_array, calendar_sel));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_channel_bypass_set(
    int unit,
    int channel,
    int enable)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable/disable bypass for RX
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above64_val));
    enable ? SHR_BITSET(reg_above64_val, channel) : SHR_BITCLR(reg_above64_val, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above64_val));

    /*
     * Enable/disable bypass for TX
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above64_val));
    enable ? SHR_BITSET(reg_above64_val, channel) : SHR_BITCLR(reg_above64_val, channel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_client_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)])
{
    uint32 calendar_sel;
    uint32 reg_val = 0;
    uint32 hw_busy = 0;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, CURRENT_TABLEf);

    /*
     * Configure standby calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_TINYMAC_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                   FSCL_REQ_GEN_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel,
                                                   &channel));
    }

    reg_val = 0;

    /*
     * switch the calendar (standby->active)
     */
    soc_reg_field_set(unit, FSCL_REQ_GEN_PLSr, &reg_val, TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_PLSr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_TINYMAC_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                                   FSCL_REQ_GEN_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel,
                                                   &channel));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_client_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)])
{
    uint32 calendar_sel;
    uint32 reg_val = 0;
    uint32 hw_busy = 0;
    uint32 data;
    int nof_retries = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
    calendar_sel = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, CURRENT_TABLEf);

    /*
     * Configure standby calendar
     */
    calendar_sel = !calendar_sel;

    data = FLEXE_INVALID_TINYMAC_CHANNEL_ID;
    SHR_BIT_ITER(tsmap, FLEXE_TINYMAC_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_REQ_GEN_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &data));
    }

    reg_val = 0;

    /*
     * switch the calendar (standby->active)
     */
    soc_reg_field_set(unit, FSCL_REQ_GEN_PLSr, &reg_val, TABLE_SWITCHf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_PLSr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
    hw_busy = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, BUSYf);

    /*
     * Poll for hardware busy
     */
    while (hw_busy && (nof_retries++ < FLEXE_REG_ACCESS_MAX_RETRIES))
    {
        sal_usleep(1);

        SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_STATEr(unit, &reg_val));
        hw_busy = soc_reg_field_get(unit, FSCL_REQ_GEN_STATEr, reg_val, BUSYf);
    }

    if (hw_busy)
    {
        SHR_ERR_EXIT(SOC_E_TIMEOUT, "HW busy polling timeout %d\n", reg_val);
    }

    /*
     * Now configure the original active calendar
     */
    calendar_sel = !calendar_sel;

    SHR_BIT_ITER(tsmap, FLEXE_TINYMAC_NOF_TIMESLOTS, index)
    {
        /*
         * Map timeslot to invalid channel
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_REQ_GEN_RAMm, 0, FSCL_BLOCK(unit), (index << 1) | calendar_sel, &data));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_66bsw_set(
    int unit,
    int src_channel,
    int dest_channel,
    int index,
    int enable)
{
    int table_id;
    int entry;
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert channel ID to table ID and entry ID:
     *     0~79: FlexE channel, table ID = 0
     *     80~159: SAR channel, table ID = 1
     *     160~241: TinyMAC channel, table ID = 2
     */
    table_id = src_channel / FLEXE_CORE_NOF_CHANNELS;
    entry = src_channel - table_id * FLEXE_CORE_NOF_CHANNELS;

    if ((table_id >= FLEXE_66BSWITCH_NOF_SUBSYS) || (entry >= FLEXE_CORE_MAX_NOF_CHANNELS))
    {
        SHR_ERR_EXIT(SOC_E_PARAM, "Invalid parameter: src_channel(0x%8x), index(%d), dest_channel(0x%8x)\n",
                     src_channel, index, dest_channel);
    }

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, FSCL_SWRAMm, table_id, FSCL_BLOCK(unit), entry, &data));

    if (!enable)
    {
        dest_channel = FLEXE_INVALID_66SW_CHANNEL_ID;
    }

    /* coverity[ptr_arith:FALSE] */
    SHR_BITCOPY_RANGE(&data, 8 * index, (SHR_BITDCL *) & dest_channel, 0, 8);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, FSCL_SWRAMm, table_id, FSCL_BLOCK(unit), entry, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_loopback_set(
    int unit,
    int instance,
    int enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OH_LOOPBACK_CFGr, &reg_val, OHRX_DIN_SRC_CFGf, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_ENV_CFG_2r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_ENV_CFG_2r, &reg_val, IS_LOCALf, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_ENV_CFG_2r(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Returns the deskew logic revision
 */
STATIC int
_soc_flexe_std_deskew_rev(
    int unit)
{
    uint32 reg_val = 0;

    if (_SHR_E_NONE == READ_FSCL_FLEXE_GRP_SSF_CFGr(unit, &reg_val))
    {
        /* FSCL_FLEXE_GRP_SSF_CFG[15:8] == 0xFF */
        if (((reg_val >> 8) & 0xFF) == 0xFF)
        {
            return 1;
        }
    }

    return 0;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_config(
    int unit,
    int instance)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_SRC_CFGr(unit, instance, &reg_val));

    soc_reg_field_set(unit, FSCL_OH_INS_SRC_CFGr, &reg_val, GID_SRC_CFGf, 0);
    soc_reg_field_set(unit, FSCL_OH_INS_SRC_CFGr, &reg_val, SECTION_SRC_CFGf, 1);
    soc_reg_field_set(unit, FSCL_OH_INS_SRC_CFGr, &reg_val, SHIM_2_SHIM_SRC_CFGf, 1);
    soc_reg_field_set(unit, FSCL_OH_INS_SRC_CFGr, &reg_val, SMC_SRC_CFGf, 2);

    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_SRC_CFGr(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, GID_INSf, 1);
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, PID_INSf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_EXGID_OH_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_EXGID_OH_CFGr, &reg_val, EXPIDf, 1);
    soc_reg_field_set(unit, FSCL_EXGID_OH_CFGr, &reg_val, EXGIDf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXGID_OH_CFGr(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(soc_flexe_std_oh_sc_bit_set(unit, instance, 1));

    if (_soc_flexe_std_deskew_rev(unit) > 0)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FAS_CHK_CFGr(unit, instance, &reg_val));
        soc_reg_field_set(unit, FSCL_FAS_CHK_CFGr, &reg_val, LOF_CLR_CFGf, 0xF);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FAS_CHK_CFGr(unit, instance, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_ALM_EN_CFGr(unit, instance, &reg_val));
        /*
        * Enable deskew OOF alarm
        */
        reg_val |= (0x1 << 17);
        SHR_IF_ERR_EXIT(WRITE_FSCL_ALM_EN_CFGr(unit, instance, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_PAD_ALM_ENr(unit, instance, &reg_val));
        soc_reg_field_set(unit, FSCL_PAD_ALM_ENr, &reg_val, PCS_LF_ENf, 1);
        SHR_IF_ERR_EXIT(WRITE_FSCL_PAD_ALM_ENr(unit, instance, reg_val));
    }

    /*
    * RX OH from loopback, will later on be disabled soon
    */
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OH_LOOPBACK_CFGr, &reg_val, OHRX_DIN_SRC_CFGf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_ieee1588_init(
    int unit)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_LETH_TYPEr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CCU_RX_LETH_TYPEr, &reg_val, CCU_RX_LETH_TYPEf, 0x88f7);
    soc_reg_field_set(unit, FSCL_CCU_RX_LETH_TYPEr, &reg_val, CCU_RX_LETH_TYPE_2f, 0x88f7);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_LETH_TYPEr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_TYPEr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CCU_TX_TYPEr, &reg_val, CCU_TX_ETH_TYPEf, 0x88f7);
    soc_reg_field_set(unit, FSCL_CCU_TX_TYPEr, &reg_val, CCU_TX_VLAN_TAGf, 0x8100);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_TYPEr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_PTP_ENr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CCU_RX_PTP_ENr, &reg_val, CCU_RX_PTP_L_2_UNI_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_PTP_ENr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_ENr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_CCU_TX_ENr, &reg_val, CCU_TX_PTP_L_2_UNI_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_ENr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_init(
    int unit)
{
    uint32 data = 0;
    uint32 reg_val;
    uint32 val;
    int instance;
    soc_reg_t grp_cfg_regs[] = { FSCL_FLEXE_INSTANCE_GRP_CFG_0r, FSCL_FLEXE_INSTANCE_GRP_CFG_1r };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_flexe_std_sar_init(unit));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_TX_RESCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_FLEXE_TX_RESCFGm, 0, 0, FSCL_BLOCK(unit), 0, FLEXE_CORE_NOF_CHANNELS, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_PHYMAP_INS_CFGm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 31, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CCA_INS_CFGm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 31, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CCB_INS_CFGm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 31, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_PHYMAP_RXm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 31, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_PHYMAP_RX_EXPm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 31, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CCAB_RXm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 19, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CCA_RX_EXPm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 19, &data));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_fill_range(unit, FSCL_CCB_RX_EXPm, 0, FLEXE_CORE_NOF_INSTANCES-1, FSCL_BLOCK(unit), 0, 19, &data));

    /*
     * Rate adapter init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mux_rateadp_init(unit));

    /*
     * Restart port
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_RX_PORT_RESTARTr(unit, 0xFF));

    /*
     * Interface channel adapter init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_interface_rx_init(unit));
    SHR_IF_ERR_EXIT(_soc_flexe_interface_tx_init(unit));

    reg_val = 0;

    /*
     * SOH to MAC channel ID
     */
    val = 80;
    FLEXE_66SW_CHID_CFG_SET(&reg_val, 0, &val);

    /*
     * 1588 to MAC channel ID
     */
    val = 81;
    FLEXE_66SW_CHID_CFG_SET(&reg_val, 1, &val);

    /*
     * MAC to 1588 channel ID
     */
    val = 80;
    FLEXE_66SW_CHID_CFG_SET(&reg_val, 2, &val);

    /*
     * MAC to SOH channel ID
     */
    val = 81;
    FLEXE_66SW_CHID_CFG_SET(&reg_val, 3, &val);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CHID_CFGr(unit, reg_val));

    for (instance = 0; instance < FLEXE_CORE_NOF_INSTANCES; ++instance)
    {
        reg_val = 0;
        soc_reg_field_set(unit, FSCL_OMF_CHK_CFGr, &reg_val, OOM_SET_CFGf, 4);
        soc_reg_field_set(unit, FSCL_OMF_CHK_CFGr, &reg_val, OOM_CLR_CFGf, 4);
        soc_reg_field_set(unit, FSCL_OMF_CHK_CFGr, &reg_val, LOM_SET_CFGf, 6);
        soc_reg_field_set(unit, FSCL_OMF_CHK_CFGr, &reg_val, LOM_CLR_CFGf, 6);
        SHR_IF_ERR_EXIT(WRITE_FSCL_OMF_CHK_CFGr(unit, instance, reg_val));

        reg_val = 0;
        soc_reg_field_set(unit, FSCL_PAD_CHK_CFGr, &reg_val, OOP_SET_CFGf, 5);
        soc_reg_field_set(unit, FSCL_PAD_CHK_CFGr, &reg_val, OOP_CLR_CFGf, 2);
        soc_reg_field_set(unit, FSCL_PAD_CHK_CFGr, &reg_val, LOP_SET_CFGf, 5);
        soc_reg_field_set(unit, FSCL_PAD_CHK_CFGr, &reg_val, LOP_CLR_CFGf, 5);
        SHR_IF_ERR_EXIT(WRITE_FSCL_PAD_CHK_CFGr(unit, instance, reg_val));

        data = 0;
        SHR_IF_ERR_EXIT(soc_custom_reg32_get(unit, grp_cfg_regs[instance / 4], REG_PORT_ANY, 0, &reg_val));
        FLEXE_MULTI_DESKEW_GRP_CFG_SET(&reg_val, instance, &data);
        SHR_IF_ERR_EXIT(soc_custom_reg32_set(unit, grp_cfg_regs[instance / 4], REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_TX_MODE_CFGr(unit, instance, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &reg_val, B_66_ERR_RPLf, 1);
        soc_reg_field_set(unit, FSCL_FLEXE_TX_MODE_CFGr, &reg_val, PHY_INST_TX_RI_SW_CFGf, instance);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_TX_MODE_CFGr(unit, instance, reg_val));

        SHR_IF_ERR_EXIT(soc_flexe_std_oh_config(unit, instance));
    }

    for (instance = 0; instance < FLEXE_CORE_NOF_INSTANCES; ++instance)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, &reg_val));
        soc_reg_field_set(unit, FSCL_FLEXE_OH_LOOPBACK_CFGr, &reg_val, OHRX_DIN_SRC_CFGf, 0);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OH_LOOPBACK_CFGr(unit, instance, reg_val));
    }

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_GRP_SSF_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_GRP_SSF_CFGr, &reg_val, GRP_SSF_ENf, 0);

    if (_soc_flexe_std_deskew_rev(unit) > 0)
    {
        soc_reg_field_set(unit, FSCL_FLEXE_GRP_SSF_CFGr, &reg_val, PHY_SSF_ENf, 0xFF);

        /* set phy_ssf_hold_sel to 1for all PHYs. bits 23:16 */
        reg_val |= 0xFF0000;
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_GRP_SSF_CFGr(unit, reg_val));

    if (_soc_flexe_std_deskew_rev(unit) > 0)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, 0x1441E, 0x11000));
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_DESKEW_RATE_M_50_Gr(unit, 804591503));
    SHR_IF_ERR_EXIT(WRITE_FSCL_DESKEW_RATE_BASE_M_50_Gr(unit, 825337743));
    SHR_IF_ERR_EXIT(WRITE_FSCL_DESKEW_RATE_M_100_Gr(unit, 1609183006));
    SHR_IF_ERR_EXIT(WRITE_FSCL_DESKEW_RATE_BASE_M_100_Gr(unit, 1629929246));

    /*
     * hardware default
     */
    SHR_IF_ERR_EXIT(WRITE_FSCL_VERIFY_CFGr(unit, 0x13F0));

    /*
     * TinyMAC subsys init
     */
    _soc_flexe_client_init(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_PRI_ENCODEr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_PRI_ENCODEr, &reg_val, BAS_SOURCE_SELf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_PRI_ENCODEr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_OAM_PRI_ENCODEr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_OAM_PRI_ENCODEr, &reg_val, BAS_SOURCE_SELf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OAM_PRI_ENCODEr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_REQ_GEN_CFGr, &reg_val, OHIF_1588_RATE_LIMIT_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_CFGr(unit, reg_val));

    /*
     * Demux subsys init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_demux_init(unit));

    /*
     * Mux subsys init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_mux_init(unit));

    /*
     * 66b subsys init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_66bswitch_init(unit));

    SHR_IF_ERR_EXIT(READ_FSCL_REQ_GEN_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_REQ_GEN_CFGr, &reg_val, CALENDAR_ENf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_REQ_GEN_CFGr(unit, reg_val));


    SHR_IF_ERR_EXIT(WRITE_FSCL_RX_PORT_RESTARTr(unit, 0));

    /*
     * OAM init
     */
    SHR_IF_ERR_EXIT(_soc_flexe_sar_oam_init(unit));
    SHR_IF_ERR_EXIT(_soc_flexe_oam_init(unit));

    SHR_IF_ERR_EXIT(soc_flexe_std_ieee1588_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_logical_phy_id_set(
    int unit,
    int instance,
    int logical_phy_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, PID_INSf, logical_phy_id);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_EXGID_OH_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_EXGID_OH_CFGr, &reg_val, EXPIDf, logical_phy_id);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXGID_OH_CFGr(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_logical_phy_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *logical_phy_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_ID_RX_VALUEr(unit, instance, &reg_val));
        *logical_phy_id = soc_reg_field_get(unit, FSCL_ID_RX_VALUEr, reg_val, ACPIDf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
        *logical_phy_id = soc_reg_field_get(unit, FSCL_OH_INS_CFG_0r, reg_val, PID_INSf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_group_id_set(
    int unit,
    int instance,
    int group_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, GID_INSf, group_id);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_EXGID_OH_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_EXGID_OH_CFGr, &reg_val, EXGIDf, group_id);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXGID_OH_CFGr(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_group_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *group_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_ID_RX_VALUEr(unit, instance, &reg_val));
        *group_id = soc_reg_field_get(unit, FSCL_ID_RX_VALUEr, reg_val, ACGIDf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
        *group_id = soc_reg_field_get(unit, FSCL_OH_INS_CFG_0r, reg_val, GID_INSf);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_client_id_set(
    int unit,
    int instance,
    int cal_id,
    int timeslot,
    int client_id)
{
    soc_mem_t ins_mem[] = { FSCL_CCA_INS_CFGm, FSCL_CCB_INS_CFGm };
    soc_mem_t rx_exp_mem[] = { FSCL_CCA_RX_EXPm, FSCL_CCB_RX_EXPm };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                               ins_mem[cal_id & 0x1],
                                               instance, FSCL_BLOCK(unit), timeslot, &client_id));

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit,
                                               rx_exp_mem[cal_id & 0x1],
                                               instance, FSCL_BLOCK(unit), timeslot, &client_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_client_id_get(
    int unit,
    int flags,
    int instance,
    int cal_id,
    int timeslot,
    int *client_id)
{
    soc_mem_t ins_mem[] = { FSCL_CCA_INS_CFGm, FSCL_CCB_INS_CFGm };
    int val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                  ins_mem[cal_id & 0x1],
                                                  instance,
                                                  FSCL_BLOCK(unit), timeslot, client_id));
    }
    else if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit,
                                                  FSCL_CCAB_RXm,
                                                  instance,
                                                  FSCL_BLOCK(unit), timeslot, &val));

        /** bits[15:0] for calendar A, bits[31:16] for calendar B */
        *client_id = ((cal_id == 0) ? val : (val >> 16)) & 0xFFFF;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_phymap_update(
    int unit,
    int instance,
    int logical_phy_id,
    int count,
    int clr)
{
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read
                    (unit, FSCL_PHYMAP_INS_CFGm, instance, FSCL_BLOCK(unit), logical_phy_id / 8, &data));

    /*
     * Update FlexE Map, always in the same uint8
     */
    if (clr == 0)
    {
        /* coverity[ptr_arith:FALSE] */
        SHR_BITSET_RANGE(&data, logical_phy_id % 8, count);
    }
    else
    {
        /* coverity[callee_ptr_arith:FALSE] */
        SHR_BITCLR_RANGE(&data, logical_phy_id % 8, count);
    }

    SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                    (unit, FSCL_PHYMAP_INS_CFGm, instance, FSCL_BLOCK(unit), logical_phy_id / 8, &data));
    /*
     * RX expected PHY map
     */
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                    (unit, FSCL_PHYMAP_RX_EXPm, instance, FSCL_BLOCK(unit), logical_phy_id / 8, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_phymap_clear(
    int unit,
    int instance)
{
    uint32 data = 0;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii=0; ii<16; ++ii)
    {
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_PHYMAP_INS_CFGm, instance, FSCL_BLOCK(unit), ii, &data));
        /*
         * RX expected PHY map
         */
        SHR_IF_ERR_EXIT(soc_custom_mem_array_write
                        (unit, FSCL_PHYMAP_RX_EXPm, instance, FSCL_BLOCK(unit), ii, &data));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_cr_bit_set(
    int unit,
    int instance,
    int cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, CR_INSf, (cal_id == 0) ? 0 : 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_cr_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val, ACCRf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_OH_INS_CFG_0r, reg_val, CR_INSf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_ca_bit_set(
    int unit,
    int instance,
    int cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, CA_INSf, (cal_id == 0) ? 0 : 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_ca_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val, ACCAf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_OH_INS_CFG_0r, reg_val, CA_INSf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_c_bit_set(
    int unit,
    int instance,
    int cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_OH_INS_CFG_0r, &reg_val, CCC_INSf, (cal_id == 0) ? 0 : 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OH_INS_CFG_0r(unit, instance, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_EXCC_OH_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_EXCC_OH_CFGr, &reg_val, EXCCCf, (cal_id == 0) ? 0 : 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXCC_OH_CFGr(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_c_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val, ACCCCf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_OH_INS_CFG_0r(unit, instance, &reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_OH_INS_CFG_0r, reg_val, CCC_INSf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_sc_bit_set(
    int unit,
    int instance,
    int sync_config)
{
    uint32 reg_val = 0;
    uint64 reg_val64;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);
    SHR_IF_ERR_EXIT(READ_FSCL_RESERVED_INS_CFGr(unit, instance, &reg_val64));
    soc_reg64_field_set(unit, FSCL_RESERVED_INS_CFGr, &reg_val64, SC_INSf, sync_config);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RESERVED_INS_CFGr(unit, instance, reg_val64));

    SHR_IF_ERR_EXIT(READ_FSCL_EXCC_OH_CFGr(unit, instance, &reg_val));
    soc_reg_field_set(unit, FSCL_EXCC_OH_CFGr, &reg_val, EXSCf, sync_config);
    SHR_IF_ERR_EXIT(WRITE_FSCL_EXCC_OH_CFGr(unit, instance, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_sc_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *sync_config)
{
    uint32 reg_val = 0;
    uint64 reg_val64;

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, &reg_val));
        *sync_config = soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val, ACSCf);
    }
    else if (flags == SOC_PORT_FLEXE_TX)
    {
        COMPILER_64_ZERO(reg_val64);
        SHR_IF_ERR_EXIT(READ_FSCL_RESERVED_INS_CFGr(unit, instance, &reg_val64));
        *sync_config = soc_reg64_field32_get(unit, FSCL_RESERVED_INS_CFGr, reg_val64, SC_INSf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_active_calendar_set(
    int unit,
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int cal_id)
{
    uint32 reg_val[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_C_CFGr(unit, reg_val));

    if (cal_id == 0)
    {
        /* coverity[ptr_arith:FALSE] */
        SHR_BITREMOVE_RANGE(reg_val, lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS, reg_val);
    }
    else
    {
        /* coverity[ptr_arith:FALSE] */
        SHR_BITOR_RANGE(reg_val, lphy_bitmap, 0, FLEXE_CORE_NOF_LPHYS, reg_val);
    }

    SHR_IF_ERR_EXIT(WRITE_FSCL_C_CFGr(unit, reg_val[0]));
    SHR_IF_ERR_EXIT(WRITE_FSCL_CFG_PLSr(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_active_calendar_get(
    int unit,
    int instance,
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    uint32 flags,
    int *cal_id)
{
    int lphy;
    uint32 reg_val[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    if (flags == SOC_PORT_FLEXE_TX)
    {
        SHR_BIT_ITER(lphy_bitmap, FLEXE_CORE_NOF_LPHYS, lphy)
        {
            SHR_IF_ERR_EXIT(READ_FSCL_C_CFGr(unit, reg_val));
            *cal_id = SHR_BITGET(reg_val, lphy) ? 1 : 0;
            break;
        }
    }
    else if (flags == SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, reg_val));
        *cal_id = soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val[0], ACCCCf);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_alarm_status_get(
    int unit,
    int instance,
    uint16 *alarm_status)
{
    int index;
    uint32 reg_val = 0;
    uint32 alm_map[][2] = {
        {GIDM_ALMf, FLEXE_CORE_OH_ALARM_GID_MISMATCH},
        {PIDM_ALMf, FLEXE_CORE_OH_ALARM_PHY_NUM_MISMATCH},
        {LOF_ALMf, FLEXE_CORE_OH_ALARM_LOF},
        {LOM_ALMf, FLEXE_CORE_OH_ALARM_LOM},
        {RPF_ALMf, FLEXE_CORE_OH_ALARM_RPF},
        {OH_1_BLOCK_ALMf, FLEXE_CORE_OH_ALARM_OH1_ALARM},
        {C_BIT_ALMf, FLEXE_CORE_OH_ALARM_C_BIT_ALARM},
        {PMM_ALMf, FLEXE_CORE_OH_ALARM_PHY_MAP_MISMATCH},
        {CRC_ALMf, FLEXE_CORE_OH_ALARM_CRC},
        {SCM_ALMf, FLEXE_CORE_OH_ALARM_SC_MISMATCH},
        {GID_UNEQ_ALMf, FLEXE_CORE_OH_ALARM_GID_UNEQUIPPED}
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OH_RX_ALMr(unit, instance, &reg_val));

    for (index = 0; index < sizeof(alm_map) / sizeof(alm_map[0]); ++index)
    {
        if (soc_reg_field_get(unit, FSCL_OH_RX_ALMr, reg_val, alm_map[index][0]))
        {
            *alarm_status |= alm_map[index][1];
        }
    }

    SHR_IF_ERR_EXIT(READ_FSCL_CCM_RX_ALMr(unit, instance, &reg_val));

    if (reg_val != 0)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_CC_RX_VALUEr(unit, instance, &reg_val));

        *alarm_status |= soc_reg_field_get(unit, FSCL_CC_RX_VALUEr, reg_val, ACCCCf)
            ? FLEXE_CORE_OH_ALARM_CAL_B_MISMATCH : FLEXE_CORE_OH_ALARM_CAL_A_MISMATCH;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_deskew_alarm_get(
    int unit,
    int group_index,
    uint8 *alarm)
{
    int all_alarms[1];
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_DSW_ALMr(unit, &reg_val));
    all_alarms[0] = soc_reg_field_get(unit, FSCL_FLEXE_DSW_ALMr, reg_val, GRP_DSW_ALMf);
    *alarm = SHR_BITGET(all_alarms, group_index) ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oh_cal_mismatch_ts_get(
    int unit,
    int instance,
    SHR_BITDCL * time_slots)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR_RANGE(time_slots, 0, FLEXE_NOF_TIMESLOTS_PER_INSTANCE);

    SHR_IF_ERR_EXIT(READ_FSCL_CCM_RX_ALMr(unit, instance, &reg_val));

    /* coverity[ptr_arith:FALSE] */
    SHR_BITCOPY_RANGE((SHR_BITDCL *) time_slots, 0, &reg_val, 0, FLEXE_NOF_TIMESLOTS_PER_INSTANCE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status)
{
    soc_reg_above_64_val_t reg_above_64_val;
    int index;
    uint32 alm_map[][2] = {
        {FSCL_FLEXE_LPI_ALMr, FLEXE_CORE_OAM_ALARM_RX_CSF_LPI},
        {FSCL_FLEXE_BAS_CS_LFr, FLEXE_CORE_OAM_ALARM_RX_CS_LF},
        {FSCL_FLEXE_BAS_CS_RFr, FLEXE_CORE_OAM_ALARM_RX_CS_RF},
        {FSCL_FLEXE_BAS_NO_RECEIVE_ALMr, FLEXE_CORE_OAM_ALARM_BASE_OAM_LOS},
        {FSCL_FLEXE_RX_SDBIPr, FLEXE_CORE_OAM_ALARM_RX_SDBIP},
        {FSCL_FLEXE_BAS_CRC_ERRr, FLEXE_CORE_OAM_ALARM_RX_BASE_CRC},
        {FSCL_FLEXE_BAS_RDIr, FLEXE_CORE_OAM_ALARM_RX_BASE_RDI},
        {FSCL_FLEXE_BAS_PERIOD_ALMr, FLEXE_CORE_OAM_ALARM_RX_BASE_PERIOD_MISMATCH},
        {FSCL_FLEXE_RX_SDBEIr, FLEXE_CORE_OAM_ALARM_SDREI},
        {FSCL_FLEXE_RX_SFBIPr, FLEXE_CORE_OAM_ALARM_SFBIP},
        {FSCL_FLEXE_RX_SFBEIr, FLEXE_CORE_OAM_ALARM_SFREI},
        {FSCL_FLEXE_RDI_LFr, FLEXE_CORE_OAM_ALARM_RX_LF},
        {FSCL_FLEXE_RF_ALMr, FLEXE_CORE_OAM_ALARM_RX_RF},
    };

    SHR_FUNC_INIT_VARS(unit);

    *alarms_status = 0;

    for (index = 0; index < sizeof(alm_map) / sizeof(alm_map[0]); ++index)
    {
        SHR_IF_ERR_EXIT(soc_custom_reg_above_64_get(unit, alm_map[index][0], REG_PORT_ANY, 0, reg_above_64_val));

        if (SHR_BITGET(reg_above_64_val, client_channel))
        {
            *alarms_status |= alm_map[index][1];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable)
{
    uint32 reg_val;
    int index;
    uint32 alm_en_map[][2] = {
        {FSCL_FLEXE_RX_LPI_RPL_ENr, FLEXE_CORE_OAM_ALARM_RX_CSF_LPI},
        {BAS_CS_LF_ALM_ENf, FLEXE_CORE_OAM_ALARM_RX_CS_LF},
        {BAS_CS_RF_ALM_ENf, FLEXE_CORE_OAM_ALARM_RX_CS_RF},
        {BAS_PERIOD_ALM_ENf, FLEXE_CORE_OAM_ALARM_BASE_OAM_LOS},
        {SDBIP_ALM_ENf, FLEXE_CORE_OAM_ALARM_RX_SDBIP},
        /*
         * {FSCL_FLEXE_BAS_CRC_ERRr, SOC_FLEXE_OAM_ALARM_RX_BASE_CRC},
         */
        {BAS_RDI_ALM_ENf, FLEXE_CORE_OAM_ALARM_RX_BASE_RDI},
        {BAS_PERIOD_ALM_ENf, FLEXE_CORE_OAM_ALARM_RX_BASE_PERIOD_MISMATCH},
    };

    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < sizeof(alm_en_map) / sizeof(alm_en_map[0]); ++index)
    {
        if (alarm_type == alm_en_map[index][1])
        {
            SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_ALM_ENr(unit, &reg_val));
            soc_reg_field_set(unit, FSCL_FLEXE_DSW_ALMr, &reg_val, alm_en_map[index][0], enable ? 1 : 0);
            SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_RX_ALM_ENr(unit, reg_val));
            break;
        }
    }

    if (index >= sizeof(alm_en_map) / sizeof(alm_en_map[0]))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "alarm_type not supported: %d.\r\n", alarm_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PERIOD_CFGr(unit, reg_above_64_val));
        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE(reg_above_64_val, client_channel * 2, (SHR_BITDCL *) & base_period, 0, 2);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_PERIOD_CFGr(unit, reg_above_64_val));
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BAS_PERIOD_CFGr(unit, reg_above_64_val));
        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE(reg_above_64_val, client_channel * 2, (SHR_BITDCL *) & base_period, 0, 2);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_PERIOD_CFGr(unit, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_PERIOD_CFGr(unit, reg_above_64_val));
        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE((SHR_BITDCL *) base_period, 0, reg_above_64_val, client_channel * 2, 2);
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BAS_PERIOD_CFGr(unit, reg_above_64_val));
        /* coverity[ptr_arith:FALSE] */
        SHR_BITCOPY_RANGE((SHR_BITDCL *) base_period, 0, reg_above_64_val, client_channel * 2, 2);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BAS_EN_CFGr(unit, reg_above_64_val));
    SHR_BITWRITE(reg_above_64_val, client_channel, enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BAS_EN_CFGr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_flexe_std_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BAS_EN_CFGr(unit, reg_above_64_val));
    *enable = SHR_BITGET(reg_above_64_val, client_channel);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above_64_val));
        SHR_BITWRITE(reg_above_64_val, client_channel, enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above_64_val));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_RX_IDLE_RPL_ENr(unit, reg_above_64_val));
        SHR_BITWRITE(reg_above_64_val, client_channel, !enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_RX_IDLE_RPL_ENr(unit, reg_above_64_val));
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above_64_val));
        SHR_BITWRITE(reg_above_64_val, client_channel, enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above_64_val));

        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_TX_IDLE_RPL_ENr(unit, reg_above_64_val));
        SHR_BITWRITE(reg_above_64_val, client_channel, !enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_TX_IDLE_RPL_ENr(unit, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_BYPASS_CHNLr(unit, reg_above_64_val));
        *enable = SHR_BITGET(reg_above_64_val, client_channel);
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_TX_BYPASS_CLIENTr(unit, reg_above_64_val));
        *enable = SHR_BITGET(reg_above_64_val, client_channel);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_val_above_64;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_BYPASS_CHNLr(unit, reg_val_above_64));
        SHR_BITWRITE(reg_val_above_64, client_channel, enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_BYPASS_CHNLr(unit, reg_val_above_64));

        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_RX_IDLE_RPL_ENr(unit, reg_val_above_64));
        SHR_BITWRITE(reg_val_above_64, client_channel, !enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_RX_IDLE_RPL_ENr(unit, reg_val_above_64));
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_OAM_TX_BYPASS_CLIENTr(unit, reg_val_above_64));
        SHR_BITWRITE(reg_val_above_64, client_channel, enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_OAM_TX_BYPASS_CLIENTr(unit, reg_val_above_64));

        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_TX_IDLE_RPL_ENr(unit, reg_val_above_64));
        SHR_BITWRITE(reg_val_above_64, client_channel, !enable);
        SHR_IF_ERR_EXIT(WRITE_FSCL_TX_IDLE_RPL_ENr(unit, reg_val_above_64));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_sar_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_val_above_64;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & SOC_PORT_FLEXE_RX)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_BYPASS_CHNLr(unit, reg_val_above_64));
        *enable = SHR_BITGET(reg_val_above_64, client_channel);
    }

    if (flags & SOC_PORT_FLEXE_TX)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_FSCL_OAM_TX_BYPASS_CLIENTr(unit, reg_val_above_64));
        *enable = SHR_BITGET(reg_val_above_64, client_channel);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_CSF_LFr(unit, reg_above_64_val));
    SHR_BITWRITE(reg_above_64_val, client_channel, enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MANUAL_INSERT_CSF_LFr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_CSF_LFr(unit, reg_above_64_val));
    *enable = SHR_BITGET(reg_above_64_val, client_channel);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_CSF_RFr(unit, reg_above_64_val));
    SHR_BITWRITE(reg_above_64_val, client_channel, enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MANUAL_INSERT_CSF_RFr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_CSF_RFr(unit, reg_above_64_val));
    *enable = SHR_BITGET(reg_above_64_val, client_channel);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_rdi_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_RDIr(unit, reg_above_64_val));
    SHR_BITWRITE(reg_above_64_val, client_channel, enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MANUAL_INSERT_RDIr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_rdi_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MANUAL_INSERT_RDIr(unit, reg_above_64_val));
    *enable = SHR_BITGET(reg_above_64_val, client_channel) ? 1 : 0;

exit:
    SHR_FUNC_EXIT;
}
/*
 * Common function to configure block number
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));
    /* coverity[ptr_arith:FALSE] */
    SHR_BITCOPY_RANGE(data, 33, &block_num, 0, 27);
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to get block number
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

    *block_num = 0;
    SHR_BITCOPY_RANGE(block_num, 0, data, 33, 27);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to set trigger threshold
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));
    /* coverity[ptr_arith:FALSE] */
    SHR_BITCOPY_RANGE(data, 16, &threshold, 0, 17);
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to get trigger threshold
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

    *threshold = 0;
    SHR_BITCOPY_RANGE(threshold, 0, data, 16, 17);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to set clear threshold
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));
    /* coverity[ptr_arith:FALSE] */
    SHR_BITCOPY_RANGE(data, 0, &threshold, 0, 16);
    SHR_IF_ERR_EXIT(soc_custom_mem_array_write(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to get clear threshold
 */
STATIC shr_error_e
_soc_flexe_std_oam_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold,
    soc_mem_t mem)
{
    uint32 data[2] = { 0, 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, mem, 0, FSCL_BLOCK(unit), client_channel, &data));

    *threshold = 0;
    SHR_BITCOPY_RANGE(threshold, 0, data, 0, 16);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Common function to get clear threshold
 */
shr_error_e
soc_flexe_std_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    return _soc_flexe_std_oam_alm_block_num_set(unit, client_channel, block_num, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    return _soc_flexe_std_oam_alm_block_num_get(unit, client_channel, block_num, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SDCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    return _soc_flexe_std_oam_alm_block_num_set(unit, client_channel, block_num, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    return _soc_flexe_std_oam_alm_block_num_get(unit, client_channel, block_num, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SDBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    return _soc_flexe_std_oam_alm_block_num_set(unit, client_channel, block_num, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    return _soc_flexe_std_oam_alm_block_num_get(unit, client_channel, block_num, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SFCFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    return _soc_flexe_std_oam_alm_block_num_set(unit, client_channel, block_num, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    return _soc_flexe_std_oam_alm_block_num_get(unit, client_channel, block_num, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_trigger_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_set(unit, client_channel, threshold, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    return _soc_flexe_std_oam_alm_clear_thr_get(unit, client_channel, threshold, FSCL_FLEXE_RX_SFBEICFGm);
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    SHR_BITSET(reg_val_above_64, client_channel);

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_RX_BIPERR_CNT_CLRr(unit, reg_val_above_64));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_read
                    (unit, FSCL_FLEXE_RX_BIPERR_RAMm, 0, FSCL_BLOCK(unit), client_channel, &data));
    COMPILER_64_SET(*val, 0, data);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    SHR_BITSET(reg_val_above_64, client_channel);

    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_RX_BEIERR_CNT_CLRr(unit, reg_val_above_64));
    SHR_IF_ERR_EXIT(soc_custom_mem_array_read
                    (unit, FSCL_FLEXE_RX_BEIERR_RAMm, 0, FSCL_BLOCK(unit), client_channel, &data));
    COMPILER_64_SET(*val, 0, data);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read(unit, FSCL_FLEXE_OAM_CNTm, 0, FSCL_BLOCK(unit), client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_custom_mem_array_read
                    (unit, FSCL_FLEXE_BASE_OAM_CNTm, 0, FSCL_BLOCK(unit), client_channel, val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_flexe_std_oam_alarm_collection_timer_step_set(
    int unit,
    int step)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_ALM_COLECTIONr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SOAM_ALM_COLECTIONr, &reg_val, SOAM_TIMER_STEP_CFGf, step);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_ALM_COLECTIONr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get the OAM alarm collection time step
 */
shr_error_e
soc_flexe_std_oam_alarm_collection_timer_step_get(
    int unit,
    int *step)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_ALM_COLECTIONr(unit, &reg_val));
    *step = soc_reg_field_get(unit, FSCL_SOAM_ALM_COLECTIONr, reg_val, SOAM_TIMER_STEP_CFGf);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure number of steps for oam alarm collection period
 */
shr_error_e
soc_flexe_std_oam_alarm_collection_step_count_set(
    int unit,
    int step_count)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_ALM_COLECTIONr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SOAM_ALM_COLECTIONr, &reg_val, SOAM_FLEXE_ALM_PRDf, step_count);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_ALM_COLECTIONr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get number of steps for oam alarm collection period
 */
shr_error_e
soc_flexe_std_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_ALM_COLECTIONr(unit, &reg_val));
    *step_count = soc_reg_field_get(unit, FSCL_SOAM_ALM_COLECTIONr, reg_val, SOAM_FLEXE_ALM_PRDf);

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure SAR cell mode
 */
shr_error_e
soc_flexe_std_sar_cell_mode_set(
    int unit,
    int cell_mode)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_GLB_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_RX_GLB_CFGr, &reg_val, B_66_SIZE_29Bf, cell_mode);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_GLB_CFGr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_GLB_CFGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SAR_TX_GLB_CFGr, &reg_val, B_66_SIZE_29Bf, cell_mode);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_TX_GLB_CFGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR cell mode
 */
shr_error_e
soc_flexe_std_sar_cell_mode_get(
    int unit,
    int *cell_mode)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_GLB_CFGr(unit, &reg_val));
    *cell_mode = soc_reg_field_get(unit, FSCL_SAR_RX_GLB_CFGr, reg_val, B_66_SIZE_29Bf);

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure Rx channel map table
 */
shr_error_e
soc_flexe_std_rx_channel_map_table_set(
    int unit,
    int bcm_port,
    uint32 local_port)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_RX_MAP_TABLEr(unit, &reg_val));
    FLEXE_RX_MAP_TABLE_SET(&reg_val, bcm_port, &local_port);
    SHR_IF_ERR_EXIT(WRITE_FSCL_RX_MAP_TABLEr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure Tx channel map table
 */
shr_error_e
soc_flexe_std_tx_channel_map_table_set(
    int unit,
    int bcm_port,
    uint32 local_port)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_TX_MAP_TABLEr(unit, &reg_val));
    FLEXE_TX_MAP_TABLE_SET(&reg_val, bcm_port, &local_port);
    SHR_IF_ERR_EXIT(WRITE_FSCL_TX_MAP_TABLEr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get local fault for FlexE client
 */
shr_error_e
soc_flexe_std_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    soc_reg_above_64_val_t reg_above64_val, field_above64_val;

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    *local_fault_status = 0;
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MACRX_FAULT_ALMr(unit, reg_above64_val));
    soc_reg_above_64_field_get(unit, FSCL_FLEXE_MACRX_FAULT_ALMr, reg_above64_val, ARM_LFf, field_above64_val);
    *local_fault_status = SHR_BITGET(field_above64_val, client_channel);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get remote fault for Flexe client
 */
shr_error_e
soc_flexe_std_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    soc_reg_above_64_val_t reg_above64_val, field_above64_val;

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    *remote_fault_status = 0;
    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_MACRX_FAULT_ALMr(unit, reg_above64_val));
    soc_reg_above_64_field_get(unit, FSCL_FLEXE_MACRX_FAULT_ALMr, reg_above64_val, ARM_RFf, field_above64_val);
    *remote_fault_status = SHR_BITGET(field_above64_val, client_channel);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get local fault for FlexE SAR client
 */
shr_error_e
soc_flexe_std_sar_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    soc_reg_above_64_val_t reg_above64_val, field_above64_val;

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    *local_fault_status = 0;
    SHR_IF_ERR_EXIT(READ_FSCL_MACRX_FAULT_ALMr(unit, reg_above64_val));
    soc_reg_above_64_field_get(unit, FSCL_MACRX_FAULT_ALMr, reg_above64_val, ARM_LFf, field_above64_val);
    *local_fault_status = SHR_BITGET(field_above64_val, client_channel);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get remote fault for Flexe SAR client
 */
shr_error_e
soc_flexe_std_sar_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    soc_reg_above_64_val_t reg_above64_val, field_above64_val;

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    *remote_fault_status = 0;
    SHR_IF_ERR_EXIT(READ_FSCL_MACRX_FAULT_ALMr(unit, reg_above64_val));
    soc_reg_above_64_field_get(unit, FSCL_MACRX_FAULT_ALMr, reg_above64_val, ARM_RFf, field_above64_val);
    *remote_fault_status = SHR_BITGET(field_above64_val, client_channel);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 Rx dest MAC
 */
shr_error_e
soc_flexe_std_1588_rx_dest_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 Rx dest MAC
 */
shr_error_e
soc_flexe_std_1588_rx_dest_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 Rx source MAC
 */
shr_error_e
soc_flexe_std_1588_rx_src_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_SAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 Rx source MAC
 */
shr_error_e
soc_flexe_std_1588_rx_src_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_SAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 Tx dest MAC
 */
shr_error_e
soc_flexe_std_1588_tx_dest_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 Tx dest MAC
 */
shr_error_e
soc_flexe_std_1588_tx_dest_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 Tx source MAC
 */
shr_error_e
soc_flexe_std_1588_tx_src_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_SAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 Tx source MAC
 */
shr_error_e
soc_flexe_std_1588_tx_src_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_SAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Enable/disable flexe bas oam to external processor
 */
shr_error_e
soc_flexe_std_bas_oam_to_ext_processor_set(
    int unit,
    int enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_LOOPBACKr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_SOAM_LOOPBACKr, &reg_val, SOAM_FLEXE_BAS_EXT_ENf, enable);
    SHR_IF_ERR_EXIT(WRITE_FSCL_SOAM_LOOPBACKr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get flexe bas oam to external processor enable status
 */
shr_error_e
soc_flexe_std_bas_oam_to_ext_processor_get(
    int unit,
    int *enable)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SOAM_LOOPBACKr(unit, &reg_val));
    *enable = soc_reg_field_get(unit, FSCL_SOAM_LOOPBACKr, reg_val, SOAM_FLEXE_BAS_EXT_ENf);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE OH/OAM dest MAC
 */
shr_error_e
soc_flexe_std_oh_oam_dest_mac_set(
    int unit,
    uint64 val)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OHIF_DASAr(unit, reg_above64_val));
    soc_reg_above_64_field64_set(unit, FSCL_OHIF_DASAr, reg_above64_val, OHIF_DEST_ADDRf, val);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_DASAr(unit, reg_above64_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE OH/OAM dest MAC
 */
shr_error_e
soc_flexe_std_oh_oam_dest_mac_get(
    int unit,
    uint64 *val)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OHIF_DASAr(unit, reg_above64_val));
    *val = soc_reg_above_64_field64_get(unit, FSCL_OHIF_DASAr, reg_above64_val, OHIF_DEST_ADDRf);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE OH/OAM src MAC
 */
shr_error_e
soc_flexe_std_oh_oam_src_mac_set(
    int unit,
    uint64 val)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OHIF_DASAr(unit, reg_above64_val));
    soc_reg_above_64_field64_set(unit, FSCL_OHIF_DASAr, reg_above64_val, OHIF_LOCAL_ADDRf, val);
    SHR_IF_ERR_EXIT(WRITE_FSCL_OHIF_DASAr(unit, reg_above64_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE OH/OAM src MAC
 */
shr_error_e
soc_flexe_std_oh_oam_src_mac_get(
    int unit,
    uint64 *val)
{
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_OHIF_DASAr(unit, reg_above64_val));
    *val = soc_reg_above_64_field64_get(unit, FSCL_OHIF_DASAr, reg_above64_val, OHIF_LOCAL_ADDRf);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 over ETH RX unicast DA
 */
shr_error_e
soc_flexe_std_1588_over_eth_rx_uni_dest_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_ETH_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 over ETH RX unicast DA
 */
shr_error_e
soc_flexe_std_1588_over_eth_rx_uni_dest_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_ETH_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 over ETH TX unicast DA
 */
shr_error_e
soc_flexe_std_1588_over_eth_tx_uni_dest_mac_set(
    int unit,
    uint64 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_ETH_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 over ETH TX unicast DA
 */
shr_error_e
soc_flexe_std_1588_over_eth_tx_uni_dest_mac_get(
    int unit,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_ETH_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 over IPv4 Rx unicast DIP
 */
shr_error_e
soc_flexe_std_1588_over_ipv4_rx_uni_dip_set(
    int unit,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_RX_IPV4_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set FlexE 1588 over IPv4 Tx unicast DIP
 */
shr_error_e
soc_flexe_std_1588_over_ipv4_tx_uni_dip_set(
    int unit,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_FSCL_CCU_TX_IPV4_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get FlexE 1588 over IPv4 Rx unicast DIP
 */
shr_error_e
soc_flexe_std_1588_over_ipv4_rx_uni_dip_get(
    int unit,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_RX_IPV4_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get FlexE 1588 over IPv4 tx unicast DIP
 */
shr_error_e
soc_flexe_std_1588_over_ipv4_tx_uni_dip_get(
    int unit,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_CCU_TX_IPV4_UNI_DAr(unit, val));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get 1588 2-step tx timestamp for FlexE phy
 */
shr_error_e
soc_flexe_std_ptp_2step_tx_info_get(
    int unit,
    int instance,
    uint32 *valid,
    uint64 *timestamp)
{
    uint32 reg_val = 0;
    uint64 reg_val64;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_PTP_TXOH_ALMr(unit, instance, &reg_val));
    *valid = soc_reg_field_get(unit, FSCL_PTP_TXOH_ALMr, reg_val, TX_2_STEP_CAP_READYf);
    if (*valid)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_PTPTX_2_STEP_TS_CAPr(unit, instance, &reg_val64));
        *timestamp = soc_reg64_field_get(unit, FSCL_PTPTX_2_STEP_TS_CAPr, 
                                         reg_val64, PORT_2_STEP_TIMESTAMPf);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - configure the SAR jitter tolerance level
 */
shr_error_e
soc_flexe_std_sar_jitter_fifo_level_set(
    int unit,
    int client_channel,
    const flexe_core_sar_jitter_fifo_level_t *fifo_level)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, client_channel, &reg_val));

    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_HIGH_CFGf, fifo_level->high_level);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_MID_CFGf, fifo_level->tolerance_level);
    soc_reg_field_set(unit, FSCL_SAR_RX_CTRL_CFGr, &reg_val, RX_FIFO_LOW_CFGf, fifo_level->low_level);

    SHR_IF_ERR_EXIT(WRITE_FSCL_SAR_RX_CTRL_CFGr(unit, client_channel, reg_val));

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - configure the SAR jitter tolerance level
 */
shr_error_e
soc_flexe_std_sar_jitter_fifo_level_get(
    int unit,
    int client_channel,
    flexe_core_sar_jitter_fifo_level_t *fifo_level)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, client_channel, &reg_val));

    fifo_level->high_level = soc_reg_field_get(unit, FSCL_SAR_RX_CTRL_CFGr, reg_val, RX_FIFO_HIGH_CFGf);
    fifo_level->tolerance_level = soc_reg_field_get(unit, FSCL_SAR_RX_CTRL_CFGr, reg_val, RX_FIFO_MID_CFGf);
    fifo_level->low_level = soc_reg_field_get(unit, FSCL_SAR_RX_CTRL_CFGr, reg_val, RX_FIFO_LOW_CFGf);

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Enable/disable bas OAM recalculation
 */
shr_error_e
soc_flexe_oam_bas_bip_recal_enable_set(
    int unit,
    int val)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_CONFIGr(unit, &reg_val));
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_RX_CONFIGr, &reg_val, BIP_RECAL_ENf, val);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_RX_CONFIGr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get bas OAM recalculation enable status
 */
shr_error_e
soc_flexe_oam_bas_bip_recal_enable_get(
    int unit,
    int *val)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_CONFIGr(unit, &reg_val));
    *val = soc_reg_field_get(unit, FSCL_FLEXE_OAM_RX_CONFIGr, reg_val, BIP_RECAL_ENf);
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure if Bas OAM period mismatch trigger Los ALM.
 */
shr_error_e
soc_flexe_oam_bas_period_mismatch_trigger_los_alarm_set(
    int unit,
    int val)
{
    uint32 reg_val = 0, bas_los_sel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_CFGr(unit, &reg_val));
    bas_los_sel = val ? 0 : 1;
    soc_reg_field_set(unit, FSCL_FLEXE_OAM_RX_CFGr, &reg_val, OAM_RX_BASE_LOS_SELf, bas_los_sel);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_OAM_RX_CFGr(unit, reg_val));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get if Bas OAM period mismatch trigger Los ALM.
 */
shr_error_e
soc_flexe_oam_bas_period_mismatch_trigger_los_alarm_get(
    int unit,
    int *val)
{
    uint32 reg_val = 0, bas_los_sel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FSCL_FLEXE_OAM_RX_CFGr(unit, &reg_val));
    bas_los_sel = soc_reg_field_get(unit, FSCL_FLEXE_OAM_RX_CFGr, reg_val, OAM_RX_BASE_LOS_SELf);
    *val = bas_los_sel ? 0 : 1;
exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef _ERR_MSG_MODULE_NAME
