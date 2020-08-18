/** \file soc/dnx/drv.h
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much. 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DNX_DRV_H
/*
 * { 
 */
#define _SOC_DNX_DRV_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/legacy/drv.h>

#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx_dispatch.h>
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */
#define SOC_JR2_HBM_DEVICE_ID_WORDS 3
typedef struct
{
    uint32 device_id_raw[SOC_JR2_HBM_DEVICE_ID_WORDS];  /* Device ID raw data returned from the HBM. See JEDEC for the
                                                         * interpretation of the data. */
} soc_dnx_hbm_device_id_t;

/*
 * }
 */
/*
 * MACROs:
 
 */

/*
 * }
 */
int soc_dnx_info_config(
    int unit);
int soc_dnx_info_config_deinit(
    int unit);
int soc_dnx_init_reset(
    int unit,
    int reset_action);
int soc_dnx_init_reg_reset(
    int unit,
    int reset_action);
int soc_dnx_device_reset(
    int unit,
    int mode,
    int action);
int soc_bist_all_jr2(
    const int unit,
    const int skip_errors);
int soc_read_jr2_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);
int soc_read_jr2_hbm_device_id(
    const int unit,
    const unsigned hbm_number,  /* read temp from which HBM */
    soc_dnx_hbm_device_id_t * id);      /* output 82 bit device id lsb word first */
int soc_dnx_init_reset_cmic_regs(
    int unit);
int soc_dnx_drv_sbus_broadcast_config(
    int unit);
int soc_dnx_device_blocks_reset(
    int unit,
    int reset_action);

/**
 * \brief - Setting chip type - used to implemented SOC_IS... MACROs
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] dev_id - the device ID enumeration value
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_chip_type_set(
    int unit,
    uint16 dev_id);

/**
 * \brief - matching the correct driver to the chip currently running.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] pci_dev_id - the device ID as identified by the PCI.
 *   \param [in] pci_rev_id - the revision ID as identified by the PCI.
 *   \param [out] found_driver - pointer to the found driver code
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_chip_driver_find(
    int unit,
    uint16 pci_dev_id,
    uint8 pci_rev_id,
    soc_driver_t ** found_driver);

/**
 * \brief - attach unit in the SOC level - should be minimal, 
 * most of the init should be done via the bcm_init or bcm_init_advanced
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern shr_error_e soc_dnx_attach(
    int unit);

/**
 * \brief - detach unit - basically undo the steps done in attach
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
extern shr_error_e soc_dnx_detach(
    int unit);

/**
 * \brief - Restores SOC control to its primal state. Should be 
 *        used either during init of soc control or during
 *        deinit to bring it back to initial state.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   SOC_CONTROL is initialized with the driver and attached
 *   flag
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_restore(
    int unit);

/*
 * mark device as not inited
 */
shr_error_e soc_dnx_mark_not_inited(
    int unit);

/**
 * \brief - allocates needed Mutexes for device
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_mutexes_init(
    int unit);

/**
 * \brief - free Devices Mutexes
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_mutexes_deinit(
    int unit);

/* Iproc/PAXB configuration not configured earlier */
shr_error_e soc_dnx_iproc_config(
    int unit);
/* Rings s-bus and broadcast blocks configuration */
shr_error_e soc_dnx_ring_config(
    int unit);
/* Configuration of the Soft reset */
shr_error_e soc_dnx_soft_reset(
    int unit);
/* Configuration of the PLLs */
shr_error_e soc_dnx_pll_configuration(
    int unit);
/* Use polling for s-bus access */
shr_error_e soc_dnx_sbus_polled_interrupts(
    int unit);
/* Making the Soft Init */
shr_error_e soc_dnx_soft_init(
    int unit);
/* DMA init */
shr_error_e soc_dnx_dma_init(
    int unit);
/* DMA deinit */
shr_error_e soc_dnx_dma_deinit(
    int unit);
/* Init of the BIST tests. Needs to be run after WB, because of a global variable init inside sand_init_cpu2tap().*/
shr_error_e soc_dnx_perform_bist_tests(
    int unit);
/* Function for making the Hard Reset */
shr_error_e soc_dnx_hard_reset(
    int unit);
/* Configuration of the endianness */
shr_error_e soc_dnx_endian_config(
    int unit);
/* Polling initialization */
shr_error_e soc_dnx_polling_init(
    int unit);
/* Access of memory write masks to no masks */
shr_error_e soc_dnx_unmask_mem_writes(
    int unit);
/* RCPU initialization */
shr_error_e soc_dnx_rcpu_init(
    int unit);
/* Function that is intilizing the SCHAN */
shr_error_e soc_dnx_schan_config_and_init(
    int unit);

/*
 * Returns TRUE if the memory is dynamic
 */
int dnx_tbl_is_dynamic(
    int unit,
    soc_mem_t mem);

/*
 * The function performs the soft init and also soft reset if perform_soft_reset set to 1.
 * Used for der 0x4 4 command.
 */
int dnx_redo_soft_reset_soft_init(
    int unit,
    int perform_soft_reset);
/*
 * } 
 */
#endif /* _SOC_DNX_DRV_H */
