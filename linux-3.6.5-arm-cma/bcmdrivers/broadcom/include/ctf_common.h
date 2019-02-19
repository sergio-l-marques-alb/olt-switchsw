/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 */

/******************************************************************************/
/*                                                                            */
/* File Name  : ctf_common.h						      */
/*									      */
/* Description: This file contains the specification of some common 	      */
/*      definitions and interfaces to other modules. This file may be	      */
/*      included by both Kernel and userapp (C only).			      */
/*									      */
/******************************************************************************/
#ifndef __CTF_COMMON_H
#define __CTF_COMMON_H

#define CTF_VERSION		"0.1"
#define CTF_VER_STR		"v" CTF_VERSION " " __DATE__ " " __TIME__
#define CTF_MODNAME		"Broadcom Packet CTF (Experimental)"

#define CTF_NAME		"ctf"

#define CTF_TRUE		1
#define CTF_FALSE		0

#define CTF_ERROR		(-1)
#define CTF_SUCCESS		0
#define CTF_ABORT		1 /* Problem with an internal function call. Caller uses
                           * it to abort further processing
                           */

#define CTF_LEARN_DISABLE	0
#define CTF_LEARN_ENABLE	1
#define CTF_LEARN_FLUSH		2

/* CTF Character Device */
#define CTFDRV_MAJOR		232
#define CTFDRV_NAME		CTF_NAME
#define CTFDRV_DEVICE_NAME	"/dev/" CTFDRV_NAME

/* CTF Forwarding Network Device */
#define CTF_NETDEV_NAME		"ctf_sw_gmac"

/* CTF Control Utility Executable */
#define CTF_CTL_UTILITY_PATH	"/bin/ctfctl"

/* CTF Proc FS Directory Path */
#define CTF_PROC_FS_DIR_PATH	CTF_NAME

/*
 * CTF Functional Callbacks
 * - Show: Displaying allocated entires in CTF
 * - Reset: Selected CTF are placed in Hardware Powerup state.
 * - Init: Selected CTF initial (default) state setting.
 * - Enable: Runtime binding with Linux networking
 * - Disable: Runtime unbinding with Linux networking
 */
typedef enum {
	CTF_STATUS,
	CTF_RESET,
	CTF_INIT = 3,
	CTF_ENABLE,
	CTF_DISABLE,
	CTF_REGDUMP,
	CTF_DUMPPT,
	CTF_DUMPNHT,
	CTF_CFG_LOG,
	CTF_ADDFE,
    CTF_ADDPT,
    CTF_ADDNHT
} ctf_callbacks;

/*
 * CTM character device driver IOCTL enums
 * A character device and the associated userspace utility for design debug.
 */
typedef enum ctf_ioctl
{
	FAIOCTL_STATUS,
	FAIOCTL_RESET,
	FAIOCTL_INIT = 3,
	FAIOCTL_ENABLE,
	FAIOCTL_DISABLE,
	FAIOCTL_REGDUMP,
	FAIOCTL_DUMPPT,
	FAIOCTL_DUMPNHT,
	FAIOCTL_CFGLOG,
	FAIOCTL_ADDFE,
    FAIOCTL_ADDPT,
    FAIOCTL_ADDNHT,
	FAIOCTL_INVALID,
} ctf_ioctl_t;

enum {
    CTF_DBG_DEFAULT_LVL = 1,
    CTF_DBG_LOW_LVL = 2,
    CTF_DBG_MEDIUM_LVL = 3,
    CTF_DBG_HIGH_LVL = 4,
    CTF_DBG_PKT_DUMP = 5,
    CTF_DBG_MAX_LVL = 5
};

extern uint32_t ctf_dbg_log_level;

/* Generic call back */
extern int ctf_callback(ctf_ioctl_t callback, int arg);

/*
 *                  Enet Switch Driver Hooks
 *  Ethernet switch driver will register the appropriate handlers that CTF
 *  will invoke to do hardware acceleration via GMAC switch.
 */

typedef enum {
	PKT_DONE,
	PKT_NORM,
} ctf_action_t;

extern int  ctf_load_init(void);
extern void ctf_unload_exit(void);

typedef struct {
	unsigned read_or_clear;
} ctf_status_t;

typedef struct {
	unsigned reset_flags;
} ctf_reset_t;

typedef struct {
	unsigned init_flags;
} ctf_init_t;

typedef struct {
	unsigned enable_flags;
} ctf_enable_t;

typedef struct {
	unsigned disable_flags;
} ctf_disable_t;

typedef struct {
	unsigned regdump_flags;
} ctf_regdump_t;

typedef struct {
	unsigned ptdump_flags;
} ctf_ptdump_t;

typedef struct {
	unsigned nhtdump_flags;
} ctf_nhtdump_t;

typedef struct {
	unsigned cfglog_flags;
} ctf_cfglog_t;

typedef struct {
	unsigned addfe_flags;
} ctf_addfe_t;

#define CTFIOCTL_STATUS		_IOR(CTFDRV_MAJOR, 0, ctf_status_t)
#define CTFIOCTL_RESET		_IORW(CTFDRV_MAJOR, 1, ctf_reset_t)
#define CTFIOCTL_INIT		_IOR(CTFDRV_MAJOR, 3, ctf_init_t)
//#define CTFIOCTL_INITNEW	_IOR(CTFDRV_MAJOR, 3, ctf_init_t)
#define CTFIOCTL_ENABLE		_IOR(CTFDRV_MAJOR, 4, ctf_enable_t)
#define CTFIOCTL_DISABLE	_IOR(CTFDRV_MAJOR, 5, ctf_disable_t)
#define CTFIOCTL_REGDUMP	_IOR(CTFDRV_MAJOR, 6, ctf_regdump_t)
#define CTFIOCTL_DUMPPT		_IOR(CTFDRV_MAJOR, 7, ctf_ptdump_t)
#define CTFIOCTL_DUMPNHT	_IOR(CTFDRV_MAJOR, 8, ctf_nhtdump_t)
#define CTFIOCTL_CFGLOG 	_IOR(CTFDRV_MAJOR, 9, ctf_cfglog_t)
#define CTFIOCTL_ADDFE  	_IOR(CTFDRV_MAJOR, 10, ctf_addfe_t)

#endif  /* defined(__CTF_COMMON_H) */
