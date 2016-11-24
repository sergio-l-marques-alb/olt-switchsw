/*
 * Bsafe uHSM test
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
 * $Id: bsafe_uhsm_lib_test.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
 */

#include <sal/appl/io.h>

#include <bsafe_uhsm_lib.h>

void dumpArray(char * mesg, unsigned char * buf, int size)
{
        int i=0;
        unsigned char * ptr;

        printk("\n****** %s %d bytes buf=%p ******\n",mesg, size, (void *)buf);
        #define MAX_DISPLAY        500
        ptr = buf;
        for (i=0;i<((size<MAX_DISPLAY)?size:MAX_DISPLAY);i++) {
                printk("%02x ",(unsigned char) *ptr); ptr =ptr+1;
                if (!((i+1)%16))
                        printk("\n");
        }
        printk("\n");
}



int test_bsafe_uhsm_start(int fd)
{
	unsigned char challenge[20]={"12345678901234567890"};
	unsigned short sessionid;
	unsigned char ksc_pub[256];
	unsigned char sig_begin_r[20];
	unsigned char sig_begin_s[20];

	int ret;

	printk("CALLING: bsafe_start \n");
	ret = bsafe_uhsm_start(fd, challenge, 
			&sessionid,
			ksc_pub,
			sig_begin_r,
			sig_begin_s);
	printk("bsafe_start %d\n", ret);

	dumpArray("ksc_pub", ksc_pub, 256);
	dumpArray("sig_begin_r", sig_begin_r, 20);
	dumpArray("sig_begin_s", sig_begin_s, 20);

	return ret;
}

int test_bsafe_uhsm_message(int fd, int nestLevel, unsigned short appkeyid, unsigned short sessionid)
{
	unsigned short dec_loc_array[2]={2,1};
	unsigned char message[24]={"123456781234567812345678"};
	int ret;
	
	printk("CALLING: bsafe_uhsm_message %d\n",nestLevel);
	
	if (appkeyid ==0)
		ret = bsafe_uhsm_message(fd, dec_loc_array,nestLevel, message, 24, NULL, &sessionid);
	else if (sessionid ==0)
		ret = bsafe_uhsm_message(fd, dec_loc_array,nestLevel, message, 24, &appkeyid, NULL);
	else
		ret = -1;
	return ret;
}

int test_bsafe_uhsm_getpubkeys(int fd)
{
	bsafe_dsa_pub_t kdi_pub_out;
	unsigned char kdc_pub_out[256];
	unsigned char p[128], g[128], q[20], y[128];
	int ret;

	kdi_pub_out.p = p;
	kdi_pub_out.g = g;
	kdi_pub_out.q = q;
	kdi_pub_out.y = y;

	printk("CALLING: test_uhsm_getpubkeys \n");
	ret = bsafe_uhsm_getpubkeys(fd, kdc_pub_out, &kdi_pub_out);

	dumpArray("kdi_pub.p", kdi_pub_out.p, 128);
	dumpArray("kdi_pub.g", kdi_pub_out.g, 128);
	dumpArray("kdi_pub.q", kdi_pub_out.q, 20);
	dumpArray("kdi_pub.y", kdi_pub_out.y, 128);
	dumpArray("kdc_pub", kdc_pub_out, 256);

	return ret;
}


int test_bsafe_uhsm_userauth_sha1pp(int fd)
{
	unsigned char challenge[20]={"12345678901234567890"};
	unsigned char sig_begin_r[20];
	unsigned char sig_begin_s[20];
	int ret;

	printk("CALLING: test_bsafe_uhsm_userauth_sha1pp \n");
	ret = bsafe_uhsm_userauth_sha1pp(fd, challenge, 
			sig_begin_r,
			sig_begin_s);

	return ret;
}

int test_bsafe_uhsm_decrypt_rsaprivate(int fd)
{
	unsigned short appkeyid=5;

	unsigned char ip[24]={"123456781234567812345678"};
	unsigned char op[24];
	int ret;

	printk("CALLING: test_bsafe_uhsm_decrypt_rsaprivate \n");
	ret = bsafe_uhsm_decrypt_rsaprivate(fd, appkeyid,
				ip , 24, op);

	return ret;
}

int test_bsafe_uhsm_decrypt_des3(int fd)
{
	unsigned short appkeyid=5;

	unsigned char iv[8]={"cafebabe"};
	unsigned char ip[24]={"123456781234567812345678"};
	unsigned char op[24];
	int ret;

	printk("CALLING: bsafe_uhsm_decrypt_des3 \n");


	ret = bsafe_uhsm_decrypt_des3(fd, appkeyid, iv,
				ip , 24, op);

	return ret;
}


int test_bsafe_init_devicekey(int fd)
{
	unsigned char auth_digest[20]={"12345678901234567890"};
	unsigned int uhsm_config = DC_ENABLE_USERKEY |  DC_AUTH_VALID | SC_EN_LOWFREQ |SC_EN_RSTMON|SC_EN_JMPMON|SC_EN_EXCPTMON;
	unsigned int prod_config = 0x9c6eb875;
	unsigned short num_row_retry =	3;
	unsigned short num_col_retry =	3;
	unsigned char statistics[4];
	int ret;

	printk("CALLING: bsafe_uhsm_init_devicekey \n");
#if 1
uhsm_config |= VOLATILE_MODE;
#endif

	ret = bsafe_init_devicekey(fd, uhsm_config, (unsigned char *) &prod_config, auth_digest, num_row_retry, num_col_retry, statistics);

	return ret;
}

int test_bsafe_selftest(int fd)
{
	unsigned int runtest=0x0;
	unsigned int result;
	int ret;

#if 0
	runtest = RAM_BIST_ROM_BIST_TEST | NVM_CHECKSUM_VERIFY_TEST | HMAC_SHA1_ENGINE_TEST | DES3_ENGINE_TEST | FIPS186_2_PSEUDO_RANDOM_TEST | DSA_SIGN_VERIFY_TEST | DH_TEST | RSA_ENC_DEC_TEST | DSA_SIGN_VERFIY_KDI_TEST| FIPS140_2_RANDOM_NUM_TEST | RANDOM_NUM_STAT_TEST ;
#else
	runtest = HMAC_SHA1_ENGINE_TEST | DES3_ENGINE_TEST | FIPS186_2_PSEUDO_RANDOM_TEST | DSA_SIGN_VERIFY_TEST | DH_TEST | RSA_ENC_DEC_TEST | DSA_SIGN_VERFIY_KDI_TEST| FIPS140_2_RANDOM_NUM_TEST | RANDOM_NUM_STAT_TEST ;
#endif

	ret = bsafe_selftest(fd,runtest, &result);

	return ret;
}


int test_bsafe_kek_akey_status(int fd)
{
	unsigned short location =3;
	unsigned char keypolicy[6];
	unsigned int ksize;
	unsigned short keytype;
	int ret;

	ret = bsafe_kek_status(fd, location, keypolicy, &keytype, &ksize);
	if (ret)
		return ret;
	ret = bsafe_akey_status(fd, location, keypolicy, &keytype, &ksize);
	return ret;
}

int test_bsafe_clr_key(int fd)
{
	unsigned short location =3;
	unsigned short cachetype =BSAFE_KEK_CACHE;
	unsigned int ksize;
	int ret;

	ret = bsafe_clr_key(fd, location, cachetype, &ksize);
	if (ret)
		return ret;
	cachetype =BSAFE_AKEY_CACHE;
	ret = bsafe_clr_key(fd, location, cachetype, &ksize);
	return ret;
}

int test_bsafe_ld_userkey(int fd)
{
	unsigned short akeylocation =3;
	unsigned int ksize;
	unsigned char keydata[32];
	int ret;

	ret = bsafe_ld_userkey(fd, akeylocation, keydata, 32, &ksize);
	
	return ret;
}

int test_bsafe_ld_cfg(int fd)
{
	unsigned char keypolicy[6]={0x1,0x2,0x3,0x4,0x5,0x6};
	unsigned short keyType = 0x16;
	bsafe_dsa_pub_t km_pub;
	unsigned char dlies_msg[24]={"123456789012345678901234"};
	unsigned char authsig_r[20]={"12345123451234512345"};
	unsigned char authsig_s[20]={"67890678906789067890"};
	int ret;

	ret = bsafe_ld_cfg(fd,
				keypolicy,
				keyType,
				&km_pub,	/* 404 bytes p g q y */
				dlies_msg,
				24,
				authsig_r,
				authsig_s
				);
				
	return ret;
}


int test_bsafe_fips_clr(int fd)
{
	int ret;
	ret = bsafe_fips_clr(fd, BSAFE_FIPS_CLR_VECTOR, 24);
	return ret;
}

int test_bsafe_user_status(int fd)
{
	unsigned int SALeventstatus;
	int ret;

	ret = bsafe_user_status(fd, &SALeventstatus);

	return ret;
}

int test_bsafe_user_nop(int fd)
{

	unsigned char datain[24] = {"123456789012345678901234"};
	unsigned char dataout[24];
	int ret;

	ret = bsafe_user_nop(fd , datain, 24, dataout );

	return ret;
}

int test_bsafe_user_random(int fd)
{
	unsigned int rngtype = BSAFE_USER_RANDOM_RNGTYPE_RAW;
	unsigned char random[20];
	int ret;
	int i, j;

	for (j = 0; j < 20; j++) {
	    ret = bsafe_user_random( fd , rngtype, random );
	    if (ret) {
		return ret;
	    }

	    for (i = 0; i < 20; i++) {
		printk("%02x", random[i]);
	    }

	    printk("\n");
	}

	return 0;
}


int test_bsafe_sha1(int fd)
{

	unsigned short prevlen=10;
	unsigned short 	mode = BSAFE_MODE_SHA1_FINISH;
	unsigned char indigest[20] = {"12345678901234567890"};
	unsigned char outdigest[20];
	unsigned char data[24]={"123456789012345678901234"};
	int ret;

	ret = bsafe_user_sha1(fd,
				prevlen,
				mode, 
				indigest /* 20 bytes */,	
				data,
				24,
				outdigest);

	return ret;
}

#if 1
int test_bsafe_le(int fd)
{
#if 1
	unsigned char one[8] ={0x1,0x2,0x3,0x4, 0x5, 0x6, 0x7, 0x8 };	/* for le */
#else
	unsigned char one[8] ={0x8,0x7,0x6,0x5, 0x4, 0x3, 0x2, 0x1 };	/* for bn */
#endif
	int ret;

	ret = bsafe_test_set_bsafe_bignum_endianess(fd,
			 one);

	return ret;
}
#endif

/*
 * The following is a hoaky interface for doing some testing on the
 * bsafe library.
 *
 * Example calling order:
 *	test_bsafe_main(unit, 0) - open the device
 *	test_bsafe_main(unit, 0x200) - run self-test
 *	test_bsafe_main(unit, 0x2) - initialize device key
 *	test_bsafe_main(unit, 0x4) - get public key
 *	test_bsafe_main(unit, 0) - close the device
 *	test_bsafe_main(unit, 0) - open the device again
 *	etc.
 */

int test_bsafe_fd = -1;

int test_bsafe_main(int unit, int flags)
{
	int fd, ret = 0;

	printk("bsafe_open: unit=%d, flags=0x%x\n", unit, flags);

	if (flags == 0) {
	    if (test_bsafe_fd < 0) {
		test_bsafe_fd = bsafe_open(unit, TRUE);

	    } else {
		printk("close: fd=%d\n", test_bsafe_fd);
		bsafe_close(test_bsafe_fd);
		test_bsafe_fd = -1;
	    }

	    return 0;
	}

	fd = test_bsafe_fd;

	if (flags & 0x00000001) {
		printk("bsafe_le: start\n");
		ret = test_bsafe_le(fd);
		printk("bsafe_le: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000002) {
		printk("init key: start\n");
		ret = test_bsafe_init_devicekey(fd);
		printk("init key: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000004) {
	    printk("getpubkeys: start\n");
	    ret = test_bsafe_uhsm_getpubkeys(fd);
	    printk("getpubkeys: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000008) {
	    printk("uhsm_start: start\n");
	    ret = test_bsafe_uhsm_start(fd);	
	    printk("uhsm_start: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000010) {
		/* 10 is just junk so that it is not null */
		printk("uhsm_message 1: start\n");
		ret = test_bsafe_uhsm_message(fd,2,0,10);
		printk("uhsm_message 1: done, ret=0x%x\n", ret);
		printk("uhsm_message 2: start\n");
		ret = test_bsafe_uhsm_message(fd,2,10,0);
		printk("uhsm_message 2: done, ret=0x%x\n", ret);
		printk("uhsm_message 3: start\n");
		ret = test_bsafe_uhsm_message(fd,1,10,0);
		printk("uhsm_message 3: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000020) {
		printk("uhsm_userauth_sha1pp: start\n");
		ret = test_bsafe_uhsm_userauth_sha1pp(fd);
		printk("uhsm_userauth_sha1pp: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000040) {
		printk("uhsm_decrypt_rsaprivate: start\n");
		ret = test_bsafe_uhsm_decrypt_rsaprivate(fd);
		printk("uhsm_decrypt_rsaprivate: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000080) {
		printk("uhsm_decrypt_des3: start\n");
		ret = test_bsafe_uhsm_decrypt_des3(fd);
		printk("uhsm_decrypt_des3: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000100) {
		printk("kek_akey_status: start\n");
		ret = test_bsafe_kek_akey_status(fd);
		printk("kek_akey_status: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000200) {
		printk("selftest: start\n");
		ret = test_bsafe_selftest(fd);
		printk("selftest: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000400) {
		printk("clr_key: start\n");
		ret = test_bsafe_clr_key(fd);
		printk("clr_key: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00000800) {
		printk("ld_userkey: start\n");
		ret = test_bsafe_ld_userkey(fd);
		printk("ld_userkey: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00001000) {
		printk("ld_cfg: start\n");
		ret = test_bsafe_ld_cfg(fd);
		printk("ld_cfg: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00002000) {
		printk("fips_clr: start\n");
		ret = test_bsafe_fips_clr(fd);
		printk("fips_clr: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00004000) {
		printk("user_status: start\n");
		ret = test_bsafe_user_status(fd);
		printk("user_status: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00008000) {
		printk("user_nop: start\n");
		ret = test_bsafe_user_nop(fd);
		printk("user_nop: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00010000) {
		printk("user_random: start\n");
		ret = test_bsafe_user_random(fd);
		printk("user_random: done, ret=0x%x\n", ret);
	}

	if (flags & 0x00020000) {
		printk("sha1: start\n");
		ret = test_bsafe_sha1(fd);
		printk("sha1: done, ret=0x%x\n", ret);
	}

	return ret;
}
