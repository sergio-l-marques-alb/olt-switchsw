/*
 * Bsafe uHSM context structures and a few local function definitions
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
 * $Id: bsafe_context.h,v 1.1 2011/04/18 17:10:59 mruas Exp $
 */

#ifndef __BSAFE_CONTEXT_H_
#define __BSAFE_CONTEXT_H_

/* abstract DEV_CONTEXT ..just used as pass through for the device ...visible only in the wrapper */
#include "bsafe_core_if.h"

typedef enum bsafe_context_state {
	BSAFE_PUSHED=-1,
	BSAFE_COMPLETION_CALLBACK_DONE
} bsafe_context_state_t;

/* allocate/get struct with command/structSize and ptrs assigned */
/* context accross interrupts */
typedef struct bsafe_context  {
	void * dev_context;	/* the opaque context to pass through the core driver to/from the wrarpper driver */
	UINT32	state;		/* maintains a state of the context */
	void * cmdio_arg;
	void * cmdio;
	UINT32 cmdio_size;
	void * input_cmd;
	UINT32 input_cmd_size;
	void * push_ptr; 	/* ptr in input_cmd to push to device; size is  input_cmd.structSize */
	bsafe_ocmd_t *ocmd;	/* for read of the return common params */
	void * output_cmd;	/* filled after callback */
	UINT32 output_cmd_size;
	void * read_ptr;	/* ptr for reading output in output_cmd which includes the ocmd */
	void (* callback)(struct bsafe_context *);
} bsafe_context_t;


/* only one command is inside chip */
/* or it is  FIFO based  */
int bsafe_push_context(bsafe_context_t * context) ;
bsafe_context_t * bsafe_pop_context(void) ;
#define PUSH_CONTEXT(context)	bsafe_push_context(context)
#define POP_CONTEXT()	bsafe_pop_context()


/* master copy routines */

extern void *(*g_bsafe_copy_from_user)(void *, const void *, size_t);
extern void *(*g_bsafe_copy_to_user)(void *, const void *, size_t);
extern void (*g_bsafe_swap_bignum)(BSAFE_BIGNUM *, int);
#define COPY_FROM_USER	(*g_bsafe_copy_from_user)
#define COPY_TO_USER	(*g_bsafe_copy_to_user)
/* swap used only if enabled else assigned to dummy values */
#define SWAPBIGNUM	(*g_bsafe_swap_bignum)

extern void bsafe_swap_bn(BSAFE_BIGNUM * src, int size);
/* since word swap of bn format is BIGNUM */
#define BN2BIGNUM	bsafe_swap_bn

/* misc routines */
void initialize_kek_akey_localstatus(void * dev_context);
void print_status_error(INT errorcode);
void print_returncode_error(UINT32 returncode);
#endif /* __BSAFE_CONTEXT_H_ */
