/*
 * $Id: jupiter_ucode.h $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 */
#ifndef JUPITER_UCODE_H_
#define JUPITER_UCODE_H_

#include <phymod/phymod.h>


#define JUPITER_UCODE_IMAGE_VERSION "0101_03"  /* matches the version number from microcode */
#define JUPITER_UCODE_IMAGE_SIZE    16844
#define JUPITER_UCODE_CSR_SIZE      1024
#define JUPITER_UCODE_SRAM_SIZE     (JUPITER_UCODE_IMAGE_SIZE/4)
#define JUPITER_UCODE_IMAGE_CRC     0xA0D3EC03

extern uint32_t *  jupiter_csr_ucode_get(void);
extern uint32_t *  jupiter_sram_ucode_get(void);
extern unsigned char*  jupiter_ucode_get(void);


#endif
