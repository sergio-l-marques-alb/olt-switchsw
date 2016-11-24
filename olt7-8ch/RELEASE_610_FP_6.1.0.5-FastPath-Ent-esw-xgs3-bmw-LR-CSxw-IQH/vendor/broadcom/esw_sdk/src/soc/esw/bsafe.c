/*
 * $Id: bsafe.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 *
 * BroadSAFE Driver
 */

#include <assert.h>

#include <sal/types.h>

#include <soc/util.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/bsafe.h>

#ifdef BCM_BSAFE_SUPPORT

#define BSAFE_FEATURE_CHECK(unit) \
        if (!soc_feature(unit, soc_feature_bsafe)) { \
            return SOC_E_UNAVAIL;       \
        }


/*
 * Function:
 *	soc_bsafe_init
 * Purpose:
 *	Initialize the BroadSAFE core
 * Parameters:
 *	unit		- Device number
 * Returns:
 *	SOC_E_NONE	- Success
 *	SOC_E_TIMEOUT	- Timeout error
 *	SOC_E_XXX	- Other error
 */

int
soc_bsafe_init(int unit)
{
    uint32		ctrl, stat;
    soc_timeout_t	to;
    sal_usecs_t		tmout;

    BSAFE_FEATURE_CHECK(unit)
    tmout = soc_property_get(unit, spn_BSAFE_TIMEOUT_USEC, 1000000);

    /* Reset uHSM block to ensure known state */

    SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_CMD_CTRLr(unit, &ctrl));
    soc_reg_field_set(unit, BSAFE_GLB_CMD_CTRLr, &ctrl, SOFT_RSTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_BSAFE_GLB_CMD_CTRLr(unit, ctrl));

    /* Wait for init done */

    soc_timeout_init(&to, tmout, 0);

    for (;;) {
	SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_DEV_STATUSr(unit, &stat));

	if (soc_reg_field_get(unit, BSAFE_GLB_DEV_STATUSr,
			      stat, INIT_DONEf)) {
	    break;
	}

	if (soc_timeout_check(&to)) {
	    soc_cm_debug(DK_ERR,
			 "soc_bsafe_command: timeout waiting for INIT_DONE\n");
	    return SOC_E_TIMEOUT;
	}
    }

    /* Verify uHSM block version */

    SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_CMD_CTRLr(unit, &ctrl));

    soc_cm_debug(DK_VERBOSE,
		 "soc_bsafe_init: revid=0x%x\n",
		 soc_reg_field_get(unit, BSAFE_GLB_CMD_CTRLr, ctrl, REVIDf));

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_bsafe_command
 * Purpose:
 *	Execute one BroadSAFE command
 * Parameters:
 *	unit		- Device number
 *	in_data		- Array of input words (least significant word first)
 *	in_count	- Number of input words
 *	out_data	- (OUT) Array of output words
 *	out_count	- Maximum number of output words expected
 *			  (actual number of words read is the amount
 *			  specified in the response)
 * Returns:
 *	SOC_E_NONE	- Success
 *	SOC_E_TIMEOUT	- Timeout error
 *	SOC_E_XXX	- Other error
 */

int
soc_bsafe_command(int unit,
		  uint32 *in_data, int in_count,
		  uint32 *out_data, int out_count)
{
    uint32		ctrl, stat;
    soc_timeout_t	to;
    sal_usecs_t		tmout;
    int			i, words, flush_error;

    BSAFE_FEATURE_CHECK(unit)

    assert(in_count > 0);
    assert(out_count > 0);

    tmout = soc_property_get(unit, spn_BSAFE_TIMEOUT_USEC, 1000000);

    /* Wait for IRDY */

    soc_timeout_init(&to, tmout, 0);

    for (;;) {
	SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_CMD_CTRLr(unit, &ctrl));

	if (soc_reg_field_get(unit, BSAFE_GLB_CMD_CTRLr, ctrl, CMD_IRDYf)) {
	    break;
	}

	if (soc_timeout_check(&to)) {
	    soc_cm_debug(DK_ERR,
			 "soc_bsafe_command: timeout waiting for IRDY\n");
	    return SOC_E_TIMEOUT;
	}
    }

    /* Check device status */

    SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_DEV_STATUSr(unit, &stat));

    i = soc_reg_field_get(unit, BSAFE_GLB_DEV_STATUSr, stat, BUSYf);

    if (i != 0) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: wrong global status BUSY=%d\n",
		     i);
	return SOC_E_INTERNAL;
    }


    i = soc_reg_field_get(unit, BSAFE_GLB_DEV_STATUSr, stat, INIT_DONEf);

    if (!i) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: incomplete initialization\n");
	return SOC_E_INTERNAL;
    }

    /* Set ISYNC */

    soc_reg_field_set(unit, BSAFE_GLB_CMD_CTRLr, &ctrl, CMD_ISYNCf, 1);
    SOC_IF_ERROR_RETURN(WRITE_BSAFE_GLB_CMD_CTRLr(unit, ctrl));

    /* Send all but last word */
    

    for (i = 0; i < in_count - 1; i++) {
	SOC_IF_ERROR_RETURN
	    (WRITE_BSAFE_GLB_CMD_DATA_INr(unit, in_data[i]));
    }

    /* Clear ISYNC */

    soc_reg_field_set(unit, BSAFE_GLB_CMD_CTRLr, &ctrl, CMD_ISYNCf, 0);
    SOC_IF_ERROR_RETURN(WRITE_BSAFE_GLB_CMD_CTRLr(unit, ctrl));

    /* Send last word */

    SOC_IF_ERROR_RETURN
	(WRITE_BSAFE_GLB_CMD_DATA_INr(unit, in_data[in_count - 1]));

    /* Wait for ORDY */

    soc_timeout_init(&to, tmout, 0);

    for (;;) {
	SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_CMD_CTRLr(unit, &ctrl));

	if (soc_reg_field_get(unit, BSAFE_GLB_CMD_CTRLr, ctrl, CMD_ORDYf)) {
	    break;
	}

	if (soc_timeout_check(&to)) {
	    soc_cm_debug(DK_ERR,
			 "soc_bsafe_command: timeout waiting for ORDY\n");
	    return SOC_E_TIMEOUT;
	}
    }

    /* Check that OSYNC is high */

    if (!soc_reg_field_get(unit, BSAFE_GLB_CMD_CTRLr, ctrl, CMD_OSYNCf)) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: OSYNC not high\n");
	return SOC_E_INTERNAL;
    }

    /* Read first word, which contains the original opcode and length */

    SOC_IF_ERROR_RETURN
	(READ_BSAFE_GLB_CMD_DATA_OUTr(unit, &out_data[0]));

    /*
     * Verify that the first word of the response reflects the same
     * opcode that was executed.
     */

    if ((out_data[0] & 0xffff0000) != (in_data[0] & 0xffff0000)) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: "
		     "output command 0x%04x and "
		     "input command 0x%04x disagree\n",
		     out_data[0] >> 16, in_data[0] >> 16);
	return SOC_E_INTERNAL;
    }

    /* Determine the length of the output in the response */

    words = ((out_data[0] & 0x0000ffff) + 3) / 4;

    flush_error = 0;

    if (words > out_count) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: "
		     "output len %d words exceeds buf len %d words\n",
		     words, out_count);

	flush_error = 1;
    }

    /* Read all but last word */
    

    for (i = 1; i < words - 1; i++) {
	SOC_IF_ERROR_RETURN
	    (READ_BSAFE_GLB_CMD_DATA_OUTr(unit,
					  &out_data[flush_error ? 0 : i]));
    }

    /* Check that OSYNC is low */

    SOC_IF_ERROR_RETURN(READ_BSAFE_GLB_CMD_CTRLr(unit, &ctrl));

    if (soc_reg_field_get(unit, BSAFE_GLB_CMD_CTRLr, ctrl, CMD_OSYNCf)) {
	soc_cm_debug(DK_ERR,
		     "soc_bsafe_command: OSYNC not low for last word\n");
	return SOC_E_INTERNAL;
    }

    /* Read last word */

    SOC_IF_ERROR_RETURN
	(READ_BSAFE_GLB_CMD_DATA_OUTr(unit,
				      &out_data[flush_error ? 0 : words - 1]));

    return (flush_error ? SOC_E_MEMORY : SOC_E_NONE);
}

/*
 * Function:
 *	soc_bsafe_selftest
 * Purpose:
 *	Execute the BroadSAFE self-test command and check result
 * Parameters:
 *	unit		- Device number
 *	test_vector	- Vector of input tests to run
 *	fail_vector	- (OUT) Vector of failed tests
 * Returns:
 *	SOC_E_NONE	- Success
 *	SOC_E_TIMEOUT	- Timeout error
 *	SOC_E_XXX	- Other error
 */

int
soc_bsafe_selftest(int unit, uint32 test_vector, uint32 *fail_vector)
{
    uint32		cmd_in[2];
    uint32		cmd_out[3];

    BSAFE_FEATURE_CHECK(unit)

    

    cmd_in[0] = 0x00020008;	/* uHSM_SELF_TEST */
    cmd_in[1] = test_vector;	/* SELFTEST VECTOR */

    SOC_IF_ERROR_RETURN(soc_bsafe_command(unit,
					  cmd_in, 2,
					  cmd_out, 3));

    if (fail_vector != NULL) {
	*fail_vector = cmd_out[2] & 0xffff;
    }

    if (SOC_BSAFE_RESULT_CODE(cmd_out) != 0) {
	soc_cm_debug(DK_VERBOSE,
		     "soc_bsafe_selftest: self test failed "
		     "(retcode=0x%04x vector=0x%04x)\n",
		     SOC_BSAFE_RESULT_CODE(cmd_out),
		     cmd_out[2] & 0xffff);
	return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

#else	/* !BCM_BSAFE_SUPPORT */

int _soc_bsafe_not_Empty;

#endif	/* !BCM_BSAFE_SUPPORT */
