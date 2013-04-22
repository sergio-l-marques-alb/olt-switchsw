/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   hpc_sfp_diagnostics.h
*
* @purpose    HPC component's internal data types and functions
*
* @component  hpc
*
* @comments   Unfortunate choice of names as this set of diagnostics is
*             NOT specific to SFPs. It covers XFPs and SFP+ modules as
*             well.
*
* @create     05/30/2008
*
* @author aloks
* @end
*
**********************************************************************/
#ifndef INCLUDE_HPC_SFP_DIAGNOSTICS_H
#define INCLUDE_HPC_SFP_DIAGNOSTICS_H

#include "flex.h"

/* *FP Diagnostics Support*/
#if L7_FEAT_SFP_DIAGNOSTIC
/* The following functions must be defined in the platform if this feature is supported. */
extern L7_RC_t hpcDiagTempRead(L7_uint32 unit, L7_uint32 port, L7_int32 *temperature);
extern L7_RC_t hpcDiagVoltageRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *voltage);
extern L7_RC_t hpcDiagCurrentRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *current);
extern L7_RC_t hpcDiagTxPowerRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txPower);
extern L7_RC_t hpcDiagRxPowerRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *rxPower);
extern L7_RC_t hpcDiagTxFaultRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *txFault);
extern L7_RC_t hpcDiagLosRead(L7_uint32 unit, L7_uint32 port, L7_uint32 *los);
extern L7_RC_t hpcDiagCopperSfpRead(L7_uint32 unit, L7_uint32 port, void *cd);
extern L7_RC_t hpcDiagPowerControl(L7_uint32 unit, L7_uint32 port);
#define HPC_DIAG_TEMP_READ(unit, port,    tempPtr)         hpcDiagTempRead(unit, port, tempPtr)
#define HPC_DIAG_VOLTAGE_READ(unit, port, voltPtr)         hpcDiagVoltageRead(unit, port, voltPtr)
#define HPC_DIAG_CURRENT_READ(unit, port, currentPtr)      hpcDiagCurrentRead(unit, port, currentPtr)
#define HPC_DIAG_TX_POWER_READ(unit, port, txPwrPtr)       hpcDiagTxPowerRead(unit, port, txPwrPtr)
#define HPC_DIAG_RX_POWER_READ(unit, port, rxPwrPtr)       hpcDiagRxPowerRead(unit, port, rxPwrPtr)
#define HPC_DIAG_TX_FAULT_READ(unit, port, txFaultPtr)     hpcDiagTxFaultRead(unit, port, txFaultPtr)
#define HPC_DIAG_LOS_READ(unit, port, losPtr)              hpcDiagLosRead(unit, port, losPtr)
#define HPC_DIAG_COPPER_SFP_READ(unit, port, cd)           hpcDiagCopperSfpRead(unit, port, cd)
#define HPC_DIAG_POWER_CTRL(unit, port)                    hpcDiagPowerControl(unit, port)
#else
#define HPC_DIAG_TEMP_READ(unit, port, tempPtr)            (L7_NOT_SUPPORTED)
#define HPC_DIAG_VOLTAGE_READ(unit, port, voltPtr)         (L7_NOT_SUPPORTED)
#define HPC_DIAG_CURRENT_READ(unit, port, currentPtr)      (L7_NOT_SUPPORTED)
#define HPC_DIAG_TX_POWER_READ(unit, port, txPwrPtr)       (L7_NOT_SUPPORTED)
#define HPC_DIAG_RX_POWER_READ(unit, port, rxPwrPtr)       (L7_NOT_SUPPORTED)
#define HPC_DIAG_TX_FAULT_READ(unit, port, txFault)        (L7_NOT_SUPPORTED)
#define HPC_DIAG_LOS_READ(unit, port, losPtr)              (L7_NOT_SUPPORTED)
#define HPC_DIAG_COPPER_SFP_READ(unit, port, cd)           (L7_NOT_SUPPORTED)
#define HPC_DIAG_POWER_CTRL(unit, port)                    (L7_NOT_SUPPORTED)
#endif /* L7_FEAT_SFP_DIAGNOSTIC */

#endif /* INCLUDE_HPC_SFP_DIAGNOSTICS_H */
