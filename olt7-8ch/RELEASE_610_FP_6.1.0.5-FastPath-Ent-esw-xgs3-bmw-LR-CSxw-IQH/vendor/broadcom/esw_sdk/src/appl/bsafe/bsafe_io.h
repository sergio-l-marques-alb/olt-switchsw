/*
 * Bsafe uHSM io definitions
 *
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * $Id: bsafe_io.h,v 1.1 2011/04/18 17:10:59 mruas Exp $
 */
/*
 * 	bsafe_io.h
 */

/* default compilation is for little endian host */
/* Endianesss.... all values would take care of endianess at driver, and  all pointers should be taken care by the application */

/* packing has been tried to fix at 4 bytes */

#include <bsafe_common.h>

#ifndef BSAFE_UHSM_IO_H_
#define BSAFE_UHSM_IO_H_


typedef enum bsafe_command_IOCTL {
	BSAFE_UHSM_INIT_DEVICEKEY_IOCTL=1,
	BSAFE_UHSM_SELFTEST_IOCTL,
	BSAFE_UHSM_DEV_KDI_PUBLIC_IOCTL,
	BSAFE_UHSM_DEV_KDC_PUBLIC_IOCTL,
	BSAFE_UHSM_KEK_STATUS_IOCTL,
	BSAFE_UHSM_AKEY_STATUS_IOCTL,
	BSAFE_UHSM_CLR_KEY_IOCTL,
	BSAFE_UHSM_LD_USERKEY_IOCTL,
	BSAFE_UHSM_LD_CFG_IOCTL,
	BSAFE_UHSM_DLIES_GEN_KS_IOCTL,
	BSAFE_UHSM_DLIES_MSG_DEC_IOCTL,
	BSAFE_UHSM_USERAUTH_IOCTL,
	BSAFE_UHSM_FIPS_CLR_IOCTL,
	BSAFE_UHSM_USER_STATUS_IOCTL,
	BSAFE_UHSM_USER_RANDOM_IOCTL,
	BSAFE_UHSM_USER_PKE_IOCTL,
	BSAFE_UHSM_USER_SHA1_IOCTL,
	BSAFE_UHSM_USER_3DES_HMACSHA1_IOCTL,
	BSAFE_UHSM_USER_NOP_IOCTL,
	/* special handling ioctl's  bignums in any format has to be in 4 byte aligned */
	BSAFE_LE_BIGNUM_IOCTL=0x0200,	/* Convert LE Bignum(openssl le machines) to BSAFE_BIGNUM */ 
	BSAFE_BN_2_BIGNUM_IOCTL=0x02001	/* Convert BN(network byte order) to BSAFE_BIGNUM */ 
}bsafe_command_IOCTL_t;



typedef struct bsafe_io_common_param {
	UINT32 returnCode;	/* has to be the first argument */
	INT returnStatus; /* return value from driver could be negative */
	/*unsigned long time_us;*/
} bsafe_io_common_param_t;



typedef struct bsafe_init_devicekey_io {
	bsafe_io_common_param_t param;
	UINT32 uhsmConfig;
	BYTE *prodConfig;		/* 4 bytes */
	BYTE *authorization;		/* 20 bytes */
	UINT16 numRowRetry;
	UINT16 numColRetry;
	BYTE *statistics;		/* 16 bytes */
} bsafe_init_devicekey_io_t;


typedef struct bsafe_selftest_io {
	bsafe_io_common_param_t param;
	UINT32 runtest;
	UINT32 result;
} bsafe_selftest_io_t; 



typedef struct bsafe_fips_clr_io {
	bsafe_io_common_param_t param;
	UINT32 clrVector;
	UINT16 numRetry;
} bsafe_fips_clr_io_t;

typedef struct bsafe_dev_kdi_public_io {
	bsafe_io_common_param_t param;
	UINT32 options;
	BYTE *configuration;	/* 8 bytes */	
	BYTE *authorization;	/* 20 bytes */
	bsafe_dsa_pub_t kdi_p;		/* p, q, g, y  total 404 bytes */
	UINT16 keyType;		/* moved down for aligned packing */
} bsafe_dev_kdi_public_io_t; 


typedef struct bsafe_dev_kdc_public_io {
	bsafe_io_common_param_t param;
	UINT32 options;
	BYTE *kdc_cp;			/* 256(kdc-cpub) bytes */
	BSAFE_BIGNUM *msgSig_r;		/* 20 bytes r */
	BSAFE_BIGNUM *msgSig_s;		/* 20 bytes s */
	UINT16 keyType;			/* moved down for aligned packing */
} bsafe_dev_kdc_public_io_t; 


typedef struct bsafe_kek_akey_status_io {
	bsafe_io_common_param_t param;
	BYTE *keyPolicy;	/* 6 bytes */		
	UINT16 location;
	UINT16 keyType;		
	UINT32 kSize;		
} bsafe_kek_akey_status_io_t;

typedef struct bsafe_clr_key_io {
	bsafe_io_common_param_t param;
	UINT16 cache;
	UINT16 location;
	UINT32 kSize;		
} bsafe_clr_key_io_t;


typedef struct bsafe_ld_userkey_io {
	bsafe_io_common_param_t param;
	UINT32 kSize;
	BYTE * keyData;
	UINT32 keyData_len;
	UINT16 akeyLoc;		
} bsafe_ld_userkey_io_t;

typedef struct bsafe_ld_cfg_io {
	bsafe_io_common_param_t param;
	BYTE *keyPolicy;	/* 6 btyes */
	bsafe_dsa_pub_t *km_pub;
	BYTE *dlies_msg;
	UINT32 dlies_msg_len;
	BSAFE_BIGNUM *authSig_r;	/* 20 bytes */
	BSAFE_BIGNUM *authSig_s;	/* 20 bytes */
	UINT16 keyType;
} bsafe_ld_cfg_io_t;

typedef struct bsafe_dlies_gen_ks_io {
	bsafe_io_common_param_t param;
	UINT16 kekLoc;		/* in systems with only one session key it is  BSAFE_KSESSION_LOC */
	BYTE *challenge;	/* 20 bytes */
	BYTE *ks_cp;		/* 256 bytes */
	BSAFE_BIGNUM *msgSig_r;		/* 20 bytes */
	BSAFE_BIGNUM *msgSig_s;		/* 20 bytes */
	UINT16 keyType;
} bsafe_dlies_gen_ks_io_t;


typedef struct bsafe_userauth_io {
	bsafe_io_common_param_t param;
	BYTE *challenge;	/* 20 bytes */
	BSAFE_BIGNUM *msgSig_r;		/* 20 bytes */
	BSAFE_BIGNUM *msgSig_s;		/* 20 bytes */
} bsafe_userauth_io_t;

typedef enum bsafe_dlies_msg_dec_io_flags {
	BSAFE_UHSM_DRIVER_GET_KEY_LOC
}bsafe_dlies_msg_dec_io_flags_t;

typedef struct bsafe_dlies_msg_dec_io {
	bsafe_io_common_param_t param;
	UINT16 *decLocArray;	
	BYTE *dlies_msg;
	UINT32 dlies_msg_len;
	UINT16 kekLoc;		/* is a return value if not set to NULL*/
	UINT16 akeyLoc;		/* is a return value if not set to NULL*/
	UINT32 msgId;
	UINT16 kType;
	UINT16 kSize;
	UINT32 flags;		/* for driver */
	UINT16 nestLevel;	/* max should be 2 */
} bsafe_dlies_msg_dec_io_t;



typedef struct bsafe_user_status_io {
	bsafe_io_common_param_t param;
	UINT32 salEventStatus;
} bsafe_user_status_io_t;

typedef struct bsafe_user_nop_io {
	bsafe_io_common_param_t param;
	BYTE *dataIn;
	UINT32 data_len;
	BYTE *dataOut;	/* dataOut length == data_len */
} bsafe_user_nop_io_t;

typedef struct bsafe_user_random_io {
	bsafe_io_common_param_t param;
	UINT32 rngType;
	BYTE *random;		/* 20 bytes */
} bsafe_user_random_io_t;


typedef struct bsafe_user_pke_io {
	bsafe_io_common_param_t param;
	BYTE *pkeMsgIn;		/* a bin array[normal format] array */
	UINT32 pkeMsg_len;
	BYTE *pkeMsgOut;	/* pkeMsgOut length == pkeMsgIn_len */ /* bin array */
	UINT16 akekLoc;
	UINT16 opCode;
} bsafe_user_pke_io_t;


/* requires revisit split pkeMsgIn with appropriate values of struct */
typedef struct bsafe_user_sha1_io {
	bsafe_io_common_param_t param;
	UINT16 prevLen;
	UINT16 mode;
	BYTE *inDigest;		/* 20 bytes */
	BYTE *data;
	UINT32 data_len;
	BYTE *outDigest;	/* 20 bytes */
} bsafe_user_sha1_io_t;



typedef struct bsafe_user_3des_hmacsha1_io {
	bsafe_io_common_param_t param;
	UINT32 mode;
	UINT16 akey3DesLoc;
	UINT16 akeyHashLoc;
	BYTE *iv;		/* 8 bytes */
	BYTE *dataIn;
	UINT32 data_len;
	BYTE *outDigest;	/* 20 bytes */
	BYTE *dataOut;		/* dataOut length == data_len */
} bsafe_user_3des_hmacsha1_io_t ;

/* misc ioctls */

typedef struct bsafe_bignum_prop_io {
	bsafe_io_common_param_t param;
} bsafe_bignum_prop_io_t;



#endif /* BSAFE_UHSM_IO_H_ */
