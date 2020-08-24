###########################################################
# FILE : sal.i
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
##########################################################

%module sal
%{
#define SWIG_FILE_WITH_INIT
#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include "bcm/types.h"
%}

%include "bcm/types.h"

%constant BOOT_F_NO_RC = BOOT_F_NO_RC;
%constant BOOT_F_NO_PROBE = BOOT_F_NO_PROBE;
%constant BOOT_F_NO_ATTACH = BOOT_F_NO_ATTACH;
%constant BOOT_F_SHELL_ON_TRAP = BOOT_F_SHELL_ON_TRAP;
%constant BOOT_F_QUICKTURN = BOOT_F_QUICKTURN;
%constant BOOT_F_PLISIM = BOOT_F_PLISIM;
%constant BOOT_F_RTLSIM = BOOT_F_RTLSIM;
%constant BOOT_F_RELOAD = BOOT_F_RELOAD;
%constant BOOT_F_WARM_BOOT = BOOT_F_WARM_BOOT;
%constant BOOT_F_BCMSIM = BOOT_F_BCMSIM;
%constant BOOT_F_XGSSIM = BOOT_F_XGSSIM;
%constant BOOT_F_NO_INTERRUPTS = BOOT_F_NO_INTERRUPTS;

typedef unsigned short  uint16;
typedef unsigned int  uint32;
typedef unsigned char  uint8;
typedef signed short  int16;
typedef signed int  int32;
typedef signed char  int8;


extern int sal_core_init(void);
extern const char* sal_os_name(void);
extern uint32 sal_boot_flags_get(void);
extern void sal_boot_flags_set(uint32);
extern char *sal_boot_script(void);
extern int sal_appl_init(void);

%{
#include <sal/appl/config.h>
void
sal_config_init_defaults(void)
{
	(void)sal_config_set("os", "unix");
}
%}
