/* $Id: utils_dune_fpga_download.h,v 1.3 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
*/
#ifndef UTILS_DUNE_FPGA_DOWNLOAD_H
#define UTILS_DUNE_FPGA_DOWNLOAD_H


typedef enum
{
  UTILS_DUNE_FPGA_DOWNLOAD_V_1=1,
  UTILS_DUNE_FPGA_DOWNLOAD_V_2=2
} UTILS_DUNE_FPGA_DOWNLOAD_TYPE_ENUM;
/*****************************************************************************/
/*  FUNCTION NAME:  utils_dune_AlteraDownload
**  FUNCTION DESCRIPTION:
**  FORMAL PARAMENETS:
**   unsigned char *base_addr  - Beginning address of memory stream to download.
**                               if NULL, then the default ROM file is taken.
**   unsigned long  size       - Size in bytes of that stream. can be '0', in
**                               base_addr is NULL
**  RETURN VALUE:
**    an unsigned integer typed returned:
**     -0  - download OK.
**     -10 - Device was not ready to download (Jumper is probably plugged)
**     -20 - Device had a problem while downloading.
**     -30 - Device had a problem after downloading.
**
**  DESIGN ISSUES:
**    The FPGA by ALTERA needs to be downloaded with a configuration stream
**    after each power-up (Power-Up, NOT Reset!).
**    (The configuration stream is held here above).
**    1). Download emulation facility:
**        The CPU is equipped with 2 I/O port we use to
**        emulate the download sequence to the ALTERA.
**        There are 3 output bits supplied by the Manager towards the
**        FPGA on port B. The FPGA provides 2 status bits that can be read
**        by the Manager in order to monitor the sequence flow on port C.
**    2). Download emulation flow:
**        We use the "passive serial mode" to download the FPGAs. The flow
**        looks as follows:
**        NConfig  _     _____________________________________\\________________
**                |___|                                      \\
**        NStatus  __      _____________________________________||______________
**                   |____|                                     ||
**        ConfDone __                                           ||            ___
**                   |__________________________________________||___________|
**                                __    __    __    __    __    ||_    __    __
**        Dclk     _|||__________|  |__|  |__|  |__|  |__|  |__|||.|__|  |__|  |__
**                              ____  ____  ____  ____  ____  __||__  ____
**        Dout     ------------<_D0_><_D1_><_D2_><_D3_><_D4_><___\\_><_Dn_>-------
**                                                                \\
**        The flow is devided into three stages:
**        -. INIT:
**                Write 'NConfig' = 0;
**                Wait 100mSec
**                Write 'NConfig' = 1;
**
**        -. PROGRAM:
**             -> Write 'Dout'.
**            |   Wait >30nSec
**            |   Write 'Dclk' = 1
**            |   Wait >50nSec
**            |   Write 'Dclk' = 0
**            |   Wait >50nSec
**            |   Read 'NStatus' : if '0' - Device had error.
**            |_________|
**
**        -. MONITOR SUCCESS:
**                Write 'Dclk' = 1
**                Wait >100nSec
**                Read ConfDone : If '0' - Device didn't configured well.
**                run 10 'Dclk' ticks.
**
**        For more details look at application note #59 from altera:
**        "Configuring Flex 10K devices" - Passive serial mode.
**        Notice I added a bunch of clocks around the NConfig falling edge. This
**        was done to fully emulate the behavior of the 'Byte-Blaster' cable.
**
*/
unsigned int
 utils_dune_AlteraDownload (
   const unsigned char                *base_addr,
   const char                         *image_name,
   unsigned long                      size,
   UTILS_DUNE_FPGA_DOWNLOAD_TYPE_ENUM board_version,
   unsigned int                       silent
 );
/*
 */
/*****************************************************
*NAME: utils_dune_download_and_load_fpga_rbf_file
*TYPE: PROC
*DATE: 03/AUG/2003
*FUNCTION: Loads the rbf file from tftp to the
*          altera FPGA (using the above util)
*CALLING SEQUENCE:
*  utils_dune_download_and_load_fpga_rbf_file()
*INPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT: rbf file name is 'fpga_linecard'.
*OUTPUT:
*  SOC_SAND_DIRECT: non-zero in case of an error
*  SOC_SAND_INDIRECT: FPGA is downloaded
*REMARKS:
*SEE ALSO: utils_dune_AlteraDownload
*****************************************************/
unsigned int
  utils_dune_download_and_load_fpga_rbf_file(
    UTILS_DUNE_FPGA_DOWNLOAD_TYPE_ENUM  board_ver_id
  );
#endif
