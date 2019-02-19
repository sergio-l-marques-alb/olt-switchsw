/** \file dnxc_data_property.c
 *
 * Common DNX Data custom soc properties.
 * 
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnxc/legacy/dnxc_data_property.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <sal/appl/sal.h>
/*
 * }
 */

#define DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_OFFSET        (8)
#define DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_MASK          (0x100)
#define DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_BITS_MASK          (0x03)
#define DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MIN_VAL            (0x0)
#define DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MAX_VAL            (0x2)

#define DNXC_DATA_PROPERTY_MAX_TX_TAPS                              (6)

/*
 * See .h file
 */
shr_error_e
dnxc_data_property_fabric_fw_load_method_read(
    int unit,
    uint32 *fabric_fw_load_method)
{
    uint32 mask = DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_MASK | DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_BITS_MASK;
    uint32 load_firmware_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    load_firmware_fabric = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", *fabric_fw_load_method);

    /** Validate only relevant bits are 'set' */
    SHR_MASK_VERIFY(load_firmware_fabric, mask, _SHR_E_CONFIG, "LOAD_FIRMWARE_FABRIC soc property value is not valid");

    *fabric_fw_load_method = load_firmware_fabric & DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_BITS_MASK;

    /** Validate load method */
    /* coverity explanation: coverity has found that we try to test if unsigned variable is
       smaller than zero, we want to keep this check although it's redundant in order to be generic
       Conclusion: It's only redundant check and not real vulunerability */ 
    /* coverity[unsigned_compare:FALSE] */  
    SHR_RANGE_VERIFY(*fabric_fw_load_method,
                     DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MIN_VAL, DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MAX_VAL,
                     _SHR_E_CONFIG, "FW Load Method value should be between %d-%d",
                     DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MIN_VAL, DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_METHOD_MAX_VAL);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_property_fabric_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify)
{
    uint32 load_firmware_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    load_firmware_fabric =
        soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric",
                                    *fabric_fw_load_verify << DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_OFFSET);

    *fabric_fw_load_verify =
        (load_firmware_fabric & DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_MASK) >>
        DNXC_DATA_PROPERTY_FABRIC_FW_LOAD_VERIFY_BITS_OFFSET;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxc_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    bcm_port_phy_tx_t* tx)
{
    char *propval, *prefix, *nrz_prefix, *pam4_prefix, *propval_tmp;
    int len, nrz_len, pam4_len;
    int tap_index;
    int taps[DNXC_DATA_PROPERTY_MAX_TX_TAPS];
    SHR_FUNC_INIT_VARS(unit);

    bcm_port_phy_tx_t_init(tx);

    /*
     * read SoC property, which should be in this format:
     * signaling_mode:pre:main:post:pre2:post2:post3
     * the [:pre2:post2:post3] part is optional.
     */
    propval = soc_property_port_get_str(unit, port, spn_SERDES_TX_TAPS);

    /* if set - parse the SoC property and set it to the struct returned */
    if (propval)
    {
        /* 1. nrz or pam4 signalling mode */
        nrz_prefix = "nrz";
        nrz_len = strlen(nrz_prefix);
        pam4_prefix = "pam4";
        pam4_len = strlen(pam4_prefix);
        if (!sal_strncasecmp(propval, nrz_prefix, nrz_len))
        {
            tx->signalling_mode = bcmPortPhySignallingModeNRZ;
            propval += nrz_len;
        }
        else if (!sal_strncasecmp(propval, pam4_prefix, pam4_len))
        {
            tx->signalling_mode = bcmPortPhySignallingModePAM4;
            propval += pam4_len;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: invalid serdes_tx_taps configuration - signalling mode - \"nrz\" or \"pam4\" is missing", port);
        }

        /* 2. get the taps */
        tx->tx_tap_mode = bcmPortPhyTxTapMode6Tap;

        prefix = ":";
        len = strlen(prefix);
        for (tap_index = 0; tap_index < DNXC_DATA_PROPERTY_MAX_TX_TAPS; ++tap_index)
        {
            if (sal_strncasecmp(propval, prefix, len))
            {
                /* if we didn't find ':' maybe we are in 3-tap mode */
                if (tap_index == 3)
                {
                    tx->tx_tap_mode = bcmPortPhyTxTapMode3Tap;
                    break;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: invalid serdes_tx_taps configuration - \":\" is missing between the taps", port);
                }
            }
            propval += len;

            taps[tap_index] = sal_ctoi(propval, &propval_tmp);
            if (propval == propval_tmp)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: invalid serdes_tx_taps configuration - tap is missing after \":\"", port);
            }

            propval = propval_tmp;
        }

        /* set the taps from SoC property to the struct that will return */
        tx->pre = taps[0];
        tx->main = taps[1];
        tx->post = taps[2];
        /* 6-tap mode - set [pre2, post2, post3] */
        if (tx->tx_tap_mode == bcmPortPhyTxTapMode6Tap)
        {
            tx->pre2 = taps[3];
            tx->post2 = taps[4];
            tx->post3 = taps[5];
        }
    }
    /* if SoC property wasn't set */
    else
    {
        tx->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
