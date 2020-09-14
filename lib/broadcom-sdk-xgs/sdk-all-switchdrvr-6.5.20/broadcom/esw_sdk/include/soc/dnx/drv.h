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
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DNX_DRV_H
/*
 * {
 */
#define _SOC_DNX_DRV_H

#if (!defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT))
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
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnx/dnx_hbmc.h>

/* #include <bcm_int/dnx/init/init.h> */
#include <bcm_int/dnx_dispatch.h>
/*
 * }
 */
/* Global variables for TRs */

typedef int (
    *dnx_startup_test_function_f) (
    int unit);
extern dnx_startup_test_function_f dnx_startup_test_functions[SOC_MAX_NUM_DEVICES];

extern uint32 any_bist_performed[(SOC_MAX_NUM_DEVICES + 31) / 32];

extern const soc_reg_t q2a_dynamic_mem_enable_regs[];
extern const soc_reg_t q2a_dynamic_mem_disable_regs[];
extern const soc_reg_t jr2_dynamic_mem_enable_regs[];
extern const soc_reg_t jr2_dynamic_mem_disable_regs[];
extern const soc_reg_t j2c_dynamic_mem_enable_regs[];
extern const soc_reg_t j2c_dynamic_mem_disable_regs[];
/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */
/*
 * MACROs:

 */
/* Reset flags */
#define SOC_DNX_RESET_ACTION_IN_RESET                               SOC_DNXC_RESET_ACTION_IN_RESET
#define SOC_DNX_RESET_ACTION_OUT_RESET                              SOC_DNXC_RESET_ACTION_OUT_RESET
#define SOC_DNX_RESET_ACTION_INOUT_RESET                            SOC_DNXC_RESET_ACTION_INOUT_RESET

#define SOC_DNX_RESET_MODE_HARD_RESET                               SOC_DNXC_RESET_MODE_HARD_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET                        SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DNX_RESET_MODE_REG_ACCESS                               SOC_DNXC_RESET_MODE_REG_ACCESS
#define SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET             SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET

#define SOC_DNX_RESET_MODE_FLAG_WITHOUT_ILE          (0x1)

#define SOC_DNX_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)

#define SOC_DNX_ALLOW_WARMBOOT_WRITE_NO_ERR(operation, _rv) \
        do { \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv); \
            if (_rv != _SHR_E_UNIT) { \
                _rv = _SHR_E_NONE; \
            } \
        } while(0)

#define SOC_DNX_WARMBOOT_RELEASE_HW_MUTEX(_rv)\
        do {\
            _rv = soc_schan_override_disable(unit); \
        } while(0)

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
    int flag,
    int action);
int soc_bist_all_jr2(
    const int unit,
    const int skip_errors);
int soc_bist_all_j2c(
    const int unit,
    const int skip_errors);
int soc_bist_all_q2a(
    const int unit,
    const int skip_errors);
int soc_bist_all_j2p(
    const int unit,
    const int skip_errors);
int soc_q2a_cpu2tap_init_mems(
    const int unit);
int soc_read_jr2_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);
int soc_read_jr2_hbm_device_id(
    const int unit,
    const unsigned hbm_number,  /* read temp from which HBM */
    soc_dnx_hbm_device_id_t * id);      /* output 82 bit device id lsb word first */
int soc_read_j2c_hbm_temp(
    const int unit,
    uint32 *out_temp);          /* output_temperature */
int soc_read_j2c_hbm_device_id(
    const int unit,
    soc_dnx_hbm_device_id_t * id);      /* output 82 bit device id lsb word first */
/* read and return the temperature of the given HBM, return a negative value on error */
int soc_dnx_read_hbm_temp(
    const int unit,
    const unsigned hbm_number,  /* read temp from which HBM */
    uint32 *out_temp);          /* output_temperature */
/* set the WRST_n signal of HBM 0/1 to 0 */
int soc_set_hbm_wrst_n_to_0(
    const int unit,
    const unsigned hbm_number); /* set WRST_n in which HBM */
int soc_dnx_init_reset_cmic_regs(
    int unit);
int soc_dnx_drv_sbus_broadcast_config(
    int unit);
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
    int unit,
    int without_fabric,
    int without_ile);
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
    int perform_soft_reset,
    int without_fabric,
    int without_ile);

/** copied to here since was in dnx/drv.h - should be moved to sand header */
int soc_sand_info_config_blocks(
    int unit,
    uint8 nof_cores_to_broadcast);

int dnx_init_skip_mem_in_mem_init_testing(
    int unit,
    soc_mem_t mem);
shr_error_e soc_dnx_block_enable_set(
    int unit);

/*
 * }
 */
#endif /* _SOC_DNX_DRV_H */
