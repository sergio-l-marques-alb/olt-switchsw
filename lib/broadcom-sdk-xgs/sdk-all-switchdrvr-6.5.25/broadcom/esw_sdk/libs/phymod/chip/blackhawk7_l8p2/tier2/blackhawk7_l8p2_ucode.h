/*
 * $Id: osprey7_v2l8p1_ucode.h $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 */
#ifndef BLACKHAWK7_L8P2_UCODE_H
#define BLACKHAWK7_L8P2_UCODE_H

#include <phymod/phymod.h>

#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_VERSION "D005_02"  /* matches the version number from microcode */
#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_SIZE    119552
#define BLACKHAWK7_V1L8P2_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V1L8P2_UCODE_IMAGE_CRC     0xBCA4

#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_VERSION "D005_16"  /* matches the version number from microcode */
#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE    122800
#define BLACKHAWK7_V2L8P2_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V2L8P2_UCODE_IMAGE_CRC     0x5C80

#define BLACKHAWK7_V2L8P2_FLR_UCODE_IMAGE_VERSION "D005_18"  /* matches the version number from microcode */
#define BLACKHAWK7_V2L8P2_FLR_UCODE_IMAGE_SIZE    119820
#define BLACKHAWK7_V2L8P2_FLR_UCODE_STACK_SIZE    0x9F2
#define BLACKHAWK7_V2L8P2_FLR_UCODE_IMAGE_CRC     0x55c3

extern unsigned char*  blackhawk7_l8p2_ucode_get(uint8_t afe_rev_a0, uint8_t flr_ucode);

#endif
