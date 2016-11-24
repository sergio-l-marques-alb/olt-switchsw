/*
 * Bsafe uHSM command structures.
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
 * $Id: bsafe.h,v 1.1 2011/04/18 17:10:59 mruas Exp $
 */

#include <bsafe_common.h>

/* bsafe_uhsm always accepts in big endian format */

/* Pass struct.command of size struct.structSize to the chip */
/* currently structs and values other than ones the BSAFE_BIGNUM's  are expected in normal format or bn format.*/

/* packing is fixed at 1 byte */

#ifndef BSAFE_UHSM_H_
#define BSAFE_UHSM_H_


typedef enum BSAFE_Command  /* uHSM Design Spec'n Rev. UA-040R5 */
{
	UHSM_INIT_DEVICEKEY     = 0x01,
	UHSM_SELFTEST           = 0x02,
	UHSM_DEV_KDI_PUBLIC     = 0x03,
	UHSM_DEV_KDC_PUBLIC     = 0x04,
	UHSM_KEK_STATUS         = 0x05,
	UHSM_AKEY_STATUS        = 0x06,
	UHSM_CLR_KEY            = 0x07,
	UHSM_LD_USERKEY         = 0x08,
	UHSM_LD_CFG             = 0x09,
	UHSM_DLIES_GEN_KS       = 0x0a,
	UHSM_DLIES_MSG_DEC      = 0x0b,
	UHSM_USERAUTH           = 0x0c,
	UHSM_FIPS_CLR           = 0x0d,
	UHSM_USER_STATUS        = 0x0e,
	UHSM_USER_RANDOM        = 0x0f,
	UHSM_USER_PKE           = 0x10,
	UHSM_USER_SHA1          = 0x11,
	UHSM_USER_3DES_HMACSHA1 = 0x12,
	UHSM_USER_NOP           = 0x13
}
BSAFE_Command;


#if defined( __GNUC__ )
# define PACK_STRUCT __attribute__((packed))
#else
/* pack all struct to 1 byte alignment for the chip */
#pragma pack(1)
# define PACK_STRUCT 
#endif


typedef struct bsafe_cmd {
	UINT16 command;
	UINT16 structSize;
} PACK_STRUCT bsafe_cmd_t;

typedef struct bsafe_ocmd {
	bsafe_cmd_t cmd;
	UINT32 returnCode;
} PACK_STRUCT bsafe_ocmd_t;

typedef struct bsafe_ptr {
	BYTE * ptr;
	UINT32 len;
} PACK_STRUCT bsafe_ptr_t;

typedef struct base_BIGNUM_ptr {
	BSAFE_BIGNUM * ptr;
	UINT32 len;
} PACK_STRUCT bsafe_BIGNUM_ptr_t;


/* BSAFE_BIGNUM */
#define BSAFE_BIGNUM_PTR_T	bsafe_BIGNUM_ptr_t

typedef struct bsafe_init_devicekey_input {
	bsafe_cmd_t cmd;

	UINT32 uhsmConfig;

	UINT32 prodConfig;	
	BYTE authorization[20];		
	UINT16 numRowRetry;
	UINT16 numColRetry;
} PACK_STRUCT bsafe_init_devicekey_input_t;

typedef struct bsafe_init_devicekey_output {
	bsafe_ocmd_t ocmd;
	BYTE statistics[4];
} PACK_STRUCT bsafe_init_devicekey_output_t;


typedef struct bsafe_selftest_input {
	bsafe_cmd_t cmd;

	UINT32 runtest;
} PACK_STRUCT bsafe_selftest_input_t; 

typedef struct bsafe_selftest_output {
	bsafe_ocmd_t ocmd;
	UINT32 result;
} PACK_STRUCT bsafe_selftest_output_t; 

typedef struct bsafe_fips_clr_input {
	bsafe_cmd_t cmd;
	UINT32 clrVector;
	UINT16 numRetry;
	BYTE reserved[2];
} PACK_STRUCT bsafe_fips_clr_input_t; 

typedef struct bsafe_fips_clr_output {
	bsafe_ocmd_t ocmd;
} PACK_STRUCT bsafe_fips_clr_output_t;


typedef struct bsafe_dev_kdi_public_input {
	bsafe_cmd_t cmd;
	UINT32 options;
} PACK_STRUCT bsafe_dev_kdi_public_input_t; 

/* the values  p, g, q. and y are in bn format MSB to LSB */
typedef struct bsafe_dsa_pub_bn {
        BYTE p[128];      /* 128 bytes */
        BYTE g[128];      /* 128 bytes */
        BYTE q[20];      /* 20 bytes */
        BYTE y[128];      /* 128 bytes */
} PACK_STRUCT bsafe_dsa_pub_bn_t;


typedef struct bsafe_dev_kdi_public_output {
	bsafe_ocmd_t ocmd;
	BYTE configuration[8];		
	BYTE authorization[20];		
	UINT16 keyType;		
	bsafe_dsa_pub_bn_t kdi_p;		/* contains p[128], g[128], q[20], y[128]  */
} PACK_STRUCT bsafe_dev_kdi_public_output_t; 


typedef struct bsafe_dev_kdc_public_input {
	bsafe_cmd_t cmd;
	UINT32 options;
} PACK_STRUCT bsafe_dev_kdc_public_input_t; 

typedef struct bsafe_dev_kdc_public_output {
	bsafe_ocmd_t ocmd;
	UINT16 keyType;		
	BYTE kdc_cp[256];		/* 256(kdc-cpub) bytes */
	BSAFE_BIGNUM msgSig_r[20];		/* r */
	BSAFE_BIGNUM msgSig_s[20];		/* s */
} PACK_STRUCT bsafe_dev_kdc_public_output_t; 


/* key and akey stuct's combined together */
typedef struct bsafe_kek_akey_status_input {
	bsafe_cmd_t cmd;
	UINT16 kekLoc;
	UINT16 akeyLoc;
} PACK_STRUCT bsafe_kek_akey_status_input_t;

typedef struct bsafe_kek_akey_status_output {
	bsafe_ocmd_t ocmd;
	BYTE keyPolicy[6];	
	UINT16 keyType;
	UINT32 kSize;		
} PACK_STRUCT bsafe_kek_akey_status_output_t;

typedef struct bsafe_clr_key_input {
	bsafe_cmd_t cmd;
	UINT16 cache;
	UINT16 location;
} PACK_STRUCT bsafe_clr_key_input_t ;


typedef struct bsafe_clr_key_output {
	bsafe_ocmd_t ocmd;
	UINT32 kSize;		
} PACK_STRUCT bsafe_clr_key_output_t;


/* fix the keyData to point to a union of different BIGNUM array for specific keys */
typedef struct bsafe_ld_userkey_input {
	BSAFE_BIGNUM_PTR_T keyData;	/* filled below the input struct; array of bignums */
		
	bsafe_cmd_t cmd;		/* actual command starts here */
	UINT16 kekLoc;			/* has to be NULL */
	UINT16 akeyLoc;

#if 0
	/* Dynamically determined pointer to it above  */
	BYTE keyData[];
#endif
} PACK_STRUCT bsafe_ld_userkey_input_t;

typedef struct bsafe_ld_userkey_output {
	bsafe_ocmd_t ocmd;
	UINT32 kSize;		
} PACK_STRUCT bsafe_ld_userkey_output_t;

typedef struct bsafe_ld_cfg_input {
	bsafe_ptr_t dlies_msg;
	BSAFE_BIGNUM *authSig_r_ptr;
	BSAFE_BIGNUM *authSig_s_ptr;

	bsafe_cmd_t cmd; 		/* actual command starts here */
	BYTE keyPolicy[6];
	UINT16 keyType;
	BYTE km_pub[404];		/* 404 bytes  km_pub manufacturer public*/
	UINT32 msgID;			/* has to be 0x22754804 */
/* determined dynamically */
#if 0
	BYTE dlies_msg[];
	BSAFE_BIGNUM authSig_r[20];
	BSAFE_BIGNUM authSig_s[20];
#endif
} PACK_STRUCT bsafe_ld_cfg_input_t;

typedef struct bsafe_ld_cfg_output {
	bsafe_ocmd_t ocmd;
} PACK_STRUCT bsafe_ld_cfg_output_t;

typedef struct bsafe_dlies_gen_ks_input {
	bsafe_cmd_t cmd;
	UINT16 kekLoc;		/* in systems with only one session key it is  BSAFE_KSESSION_LOC */
	BYTE reserved[2];
	BYTE challenge[20];
} PACK_STRUCT bsafe_dlies_gen_ks_input_t;

typedef struct bsafe_dlies_gen_ks_output {
	bsafe_ocmd_t ocmd;
	UINT16 keyType;
	BYTE ks_cp[256];
	BSAFE_BIGNUM msgSig_r[20];
	BSAFE_BIGNUM msgSig_s[20];
} PACK_STRUCT bsafe_dlies_gen_ks_output_t;


typedef struct bsafe_userauth_input {
	bsafe_cmd_t cmd;
	BYTE challenge[20];
} PACK_STRUCT bsafe_userauth_input_t;

typedef struct bsafe_userauth_output {
	bsafe_ocmd_t ocmd;
	BSAFE_BIGNUM msgSig_r[20];
	BSAFE_BIGNUM msgSig_s[20];
} PACK_STRUCT bsafe_userauth_output_t;

typedef struct bsafe_dlies_msg_dec_input {
	bsafe_ptr_t dlies_msg;

	bsafe_cmd_t cmd; 		/* actual command starts here */
	UINT16 cache;
	UINT16 nestLevel;
	UINT16 decLocArray[2];
	UINT16 kekLoc;
	UINT16 akeyLoc;
	UINT32 msgID;			/* has to be 0x22754804 */
/* variable determined during allocation */
#if 0
	BYTE dlies_msg[];
#endif
	
} PACK_STRUCT bsafe_dlies_msg_dec_input_t;


typedef struct bsafe_dlies_msg_dec_output {
	bsafe_ocmd_t ocmd;
	UINT32 msgId;
	UINT16 kType;
	UINT16 kNumLocations;
} PACK_STRUCT bsafe_dlies_msg_dec_output_t;



typedef struct bsafe_user_status_input {
	bsafe_cmd_t cmd;
} PACK_STRUCT bsafe_user_status_input_t;

typedef struct bsafe_user_status_output {
	bsafe_ocmd_t ocmd;
	UINT32 salEventStatus;
} PACK_STRUCT bsafe_user_status_output_t;

typedef struct bsafe_user_nop_input {
	bsafe_ptr_t dataIn;

	bsafe_cmd_t cmd;

#if 0
	BYTE dataIn[];
#endif
} PACK_STRUCT bsafe_user_nop_input_t;

typedef struct bsafe_user_nop_output {
	bsafe_ptr_t dataOut;

	bsafe_ocmd_t ocmd;
/* determined latter */
#if 0
	BYTE dataOut[];
#endif
} PACK_STRUCT bsafe_user_nop_output_t;

typedef struct bsafe_user_random_input {
	bsafe_cmd_t cmd;
	UINT32 rngType;
	BYTE random[20];
} PACK_STRUCT bsafe_user_random_input_t;

typedef struct bsafe_user_random_output {
	bsafe_ocmd_t ocmd;
	BYTE random[20];
} PACK_STRUCT bsafe_user_random_output_t;

typedef struct bsafe_user_pke_input {
	bsafe_BIGNUM_ptr_t pkeMsgIn;
	
	bsafe_cmd_t cmd;
	UINT16 kekLoc;
	UINT16 akekLoc;
	UINT16 opCode;
	UINT16 contextLen;	/* not used 0x00 is the value */
#if 0 /* determined latter */
	BYTE pkeMsgIn[];
#endif
} PACK_STRUCT bsafe_user_pke_input_t;


typedef struct bsafe_user_pke_output {
	bsafe_BIGNUM_ptr_t pkeMsgOut;

	bsafe_ocmd_t ocmd;
#if 0 /* determined latter */
	BYTE pkeMsgOut[];
#endif
} PACK_STRUCT bsafe_user_pke_output_t;


typedef struct bsafe_user_sha1_input {
	bsafe_ptr_t data;

	bsafe_cmd_t cmd;
	UINT16 prevLen;
	UINT16 mode;
	BYTE inDigest[20];
#if 0 /* determined latter */
	BYTE data[];
#endif
} PACK_STRUCT bsafe_user_sha1_input_t;

typedef struct bsafe_user_sha1_output {
	bsafe_ocmd_t ocmd;
	BYTE outDigest[20];
} PACK_STRUCT bsafe_user_sha1_output_t;


typedef struct bsafe_user_3des_hmacsha1_input {
	bsafe_ptr_t dataIn; /* ptr to data filled below the input structure */

	bsafe_cmd_t cmd; 	/* start of actual command */
	UINT32 mode;
	UINT16 akey3DesLoc;
	UINT16 akeyHashLoc;
	BYTE reserved[16];
	BYTE iv[8];
#if 0 /* determined at allocation */
	BYTE dataIn[];
#endif
} PACK_STRUCT bsafe_user_3des_hmacsha1_input_t ;

typedef struct bsafe_user_3des_hmacsha1_output {
	bsafe_ptr_t dataOut; /* ptr to data filled below this struct */

	bsafe_ocmd_t ocmd;
	BYTE outDigest[20];
#if 0 /* determined latter */
	BYTE dataOut[];			/* may have iv if it is cbc mode */
#endif
} PACK_STRUCT bsafe_user_3des_hmacsha1_output_t ;


/* revert back the packing or else all other structs would crash on unaligned ptr accesses */
#if !defined( __GNUC__ )
#pragma pack()
#endif

#endif /* BSAFE_UHSM_H_ */
