/*
 * $Id: cmicx_qspi.h,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:     Header for CMICX QSPI driver
 *
 */
#ifndef _CMICX_QSPI_H_
#define _CMICX_QSPI_H_

/*!
 * Initialize QSPI. software and hardware resources
 *
.* unit: Device ID.
 * max_hz:  Maximum SCK rate in Hz.
 * mode:    Clock polarity, clock phase and other parameters.
 *
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_init(int unit,
                    uint32 max_hz,
                    uint32 mode);

/*!
 * Cleanup QSPI resources
 *
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_cleanup(int unit);

/*!
 * Claim the bus and prepare it for communication.
 * This must be called before doing any transfers
 *
.* unit: Device ID.
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_claim_bus(int unit);

/*!
 * Release the SPI bus
 *
 * This must be called once all transfers have finished
 *
.* unit: Device ID.
 *   slave:	The SPI slave
 */
extern int
cmicx_qspi_release_bus(int unit);

/*!
 * QSPI Read Write
 *
 * This writes  the SPI MOSI port and simultaneously clocks
 * read from the MISO port
.*
.* unit: Device ID.
 * en: Bytes to write and read.
 * dout: Data Out buffer.
 * din: Data in buffer.
 * flags: A bitwise combination of SPI_XFER_* flags.
 *
 *   Returns: 0 on success, not 0 on failure
 */
extern int
cmicx_qspi_rw(int unit,
                  uint32 len,
                  const void *dout,
                  void *din,
                  uint32 flags);

#endif /* _CMICX_QSPI_H_ */
