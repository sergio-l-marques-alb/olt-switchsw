/*
 * $Id: tdm_parse.c.$
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
 * All Rights Reserved.$
 *
 * TDM core parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_abs
@param:

Returns absolute value of an integer
**/
int
tdm_abs(int num)
{
	if (num < 0) {
		return (-num);
	}
	else {
		return num;
	}
}


/**
@name: tdm_fac
@param:

Calculates factorial of an integer
**/
int
tdm_fac(int num_signed)
{
	int i, product, num;
	num = tdm_abs(num_signed);
	product=num;
	
	if (num==0) {
		return 1;
	}
	else {
		for (i=(num-1); i>0; i--) {
			product *= (num-i);
		}	
		return product;
	}
}


/**
@name: tdm_pow
@param:

Calculates unsigned power of an integer
**/
int
tdm_pow(int num, int pow)
{
	int i, product=num;
	
	if (pow==0) {
		return 1;
	}
	else {
		for (i=1; i<pow; i++) {
			product *= num;
		}
		return product;
	}
}


/**
@name: tdm_sqrt
@param:

Calculates approximate square root of an integer using Taylor series without float using Bakhshali Approximation
**/
int
tdm_sqrt(int input_signed)
{
	int n, d=0, s=1, approx, input;
	input = tdm_abs(input_signed);
	
	do {
		d=(input-tdm_pow(s,2));
		if (d<0) {
			--s;
			break;
		}
		if ( ((1000*tdm_abs(d))/tdm_pow(s,2)) <= ((1000*tdm_abs(d+1))/tdm_pow((s+1),2)) ) {
			break;
		}
		s++;
	} while(s<input);
	d=(input-tdm_pow(s,2));

	approx=s;
	if (d<(2*s)) {
		for (n=1; n<3; n++) {
			approx+=((tdm_pow(-1,n)*tdm_fac(2*n)*tdm_pow(d,n))/(tdm_pow(tdm_fac(n),2)*tdm_pow(4,n)*tdm_pow(s,((2*n)-1))*(1-2*n)));
		}
	}
	
	return approx;	
}


/**
@name: tdm_PQ
@param:

Customizable partial quotient ceiling function
**/
/* int tdm_PQ(float f) { return (int)((f < 0.0f) ? f - 0.5f : f + 0.5f); }*/
int tdm_PQ(int f)
{
	return ( (int)( ( f+5 )/10) );
}
