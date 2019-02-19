/*  $Id: lcint_ecmp_hash_resolve.c,v 1.1 Broadcom SDK $
 *
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 *
 * File:    cint_ecmp_hash_resolve.c
 * Purpose: This CINT simulate the ECMP meter resolution to FEC member performaed by Arad+ and Jericho and runs independently(!) from the SDK.
 *  		This CINT is based on cint_trunk_ecmp_lb_key_and_member_retrieve.c which is depented on the SDK.
 * 
 * Example run:
		cd ../../../../src/examples/dpp/
		cint utility/cint_utils_l3.c
		cint utility/cint_utils_vlan.c
		cint cint_ip_route.c
		cint cint_ecmp_hashing.c 
		cint cint_ecmp_hash_resolve.c
		cint
		uint32 ecmp_group_size = 3;
		print disable_ecmp_hashing(0);
		print ecmp_src_port_enable(0, 0);
		print ecmp_hash_sip(0);
		print ecmp_hash_func_config(0, BCM_HASH_CONFIG_CRC16_0x8423);
		print ecmp_nof_headers_set(0, 13, 1);
		print ecmp_hashing_main(0, 13, 14, ecmp_group_size);
		uint32 ecmp_start_fec = 0x1000;
		uint32 src_ip = 0x0ab8086f;
    	ip4_src_example(src_ip, ecmp_start_fec, ecmp_group_size, 0);
 
 * Transmit the the following packet: 
 * Ethernet DA 00:0C:00:02:01:23, VID 17
 * IPv4 Source 10.184.8.111 (0x0a:b8:08:6f, this is the input for the ip4_src_example function), Destination 10.0.255.0
 *
 * Expected functuin output: ecmp_lb_key=0xb69b, offset=0x2, fec_pointer=0x1002
 * You can use signals in order to verify this output.
 * Jericho signals:
 *    ecmp_lb_key: acc_pb_pp reg_access_arad signal_get addr_high 3 addr_low 0 msb 15 lsb 0 block_id 1
 *    fec_ecmp_pointer: acc_pb_pp reg_access_arad signal_get addr_high 2 addr_low 3 msb 53 lsb 37 block_id 1
 */


/*
 * Function:
 *    dnx_hash
 * Purpose:
 *    Hash function used for ECMP LB key calaculation.
 * Parameters: 
 *    polynom    -- 16bit polynomial used for hashing.
 *    seed       -- hash seed.
 *    lb_width   -- the desired length for the output.
 *    data       -- data to hash.
 *    data_width -- length of data to hash.
 */
uint32 dnx_hash(uint32 polynom, uint32 seed, uint32 lb_width, uint32 *data, uint32 data_width) {
	uint32 msb;
	uint32 lfsr;
	uint32 i;
	uint32 temp;
	int rv;

   printf("Hashing: polynom=0x%x, seed=0x%x, lb_width=%d, data=0x%x-%x-%x-%x, data_width=%d", polynom, seed, lb_width, data[0], data[1], data[2], data[3], data_width);
   lfsr = seed;
   lb_width -=1;
   for (i = 0; i < data_width; i++) {
		msb  = (lfsr >> (lb_width - 1)) & 1;
		lfsr = (lfsr << 1);
		lfsr = lfsr & ((1 << lb_width) - 1);
		temp = (i / 32);
		lfsr = lfsr + (1 & (data[3 - (temp & ((1 << 5) - 1))] >> (i % 32)));
		lfsr = lfsr ^ ((msb ? ((1 << lb_width) - 1) : 0) & polynom);
   }
   
   /*calculating the msb*/
   lfsr = (lfsr & ((1 << lb_width) - 1)) | (seed & (1 << lb_width));
   for (i = 0; i < data_width; i++) {
		temp = (i / 32);
		msb  = (lfsr >> lb_width) & 1;
		lfsr = lfsr & ((1 << lb_width) - 1);
		lfsr = lfsr | (((1 & (data[3 - (temp & ((1 << 5) - 1))] >> (i % 32))) ^ msb) << lb_width);
   }

   printf(", result=0x%x\n", lfsr);
   return (lfsr);
}

/*
 * Function:
 *    ip4_src_example
 * Purpose:
 *    An example which resolves a source IP to a FEC pointer based on hashing of the source IP alone.
 *    Assuming the following ECMP hashing configuration:
 *    1. Hashing header count is 1.
 *    2. Hashing seed and shift are using the default values.
 *    3. Hashing function is BCM_HASH_CONFIG_CRC16_0x8423.
 *    4. Hashing is made based on Source IP only.
 */

void ip4_src_example(uint32 src_ip, uint32 ecmp_start_fec, uint32 ecmp_size, uint32 ecmp_is_protected) {
	uint32 crc_vector[4];
	uint32 ecmp_lb_key, offset, fec_ecmp_pointer;
	ecmp_lb_key = offset = fec_ecmp_pointer = 0;
	
	/*Building the LB-Vector based on Source IP only*/
	crc_vector[0] = 0;
	crc_vector[1] = (src_ip & 0x000000ff);
	crc_vector[2] = 0;
	crc_vector[3] = (src_ip >> 8);
	ecmp_lb_key = dnx_hash(0x423, 1, 16, crc_vector, 120); 
	
	/*Hashing the pmf-data and pp-port, here disabled therefore crc_vector is zero*/
	crc_vector[0] = crc_vector[1] = crc_vector[2] = crc_vector[3] = 0;
	ecmp_lb_key = dnx_hash(0x423, ecmp_lb_key, 16, crc_vector, 120);
	
	offset = (ecmp_size * ecmp_lb_key) >> 16;
	offset = offset * (1 << ecmp_is_protected);
	fec_ecmp_pointer = ecmp_start_fec + offset;
	
	printf("ecmp_lb_key=0x%x, fec_ecmp_pointer=0x%x\n", ecmp_lb_key, fec_ecmp_pointer);
}