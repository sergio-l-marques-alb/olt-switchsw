FEATURE_LIST := CINT ATPTRANS_SOCKET L3 CPU_I2C I2C MEM_SCAN EDITLINE CHASSIS TEST BCM_SAL_PROFILE RCPU INTR APIMODE PTP CUSTOMER MSTP DUNE_UI KBP AUTOCOMPLETE CTEST

BCM_PTL_SPT=1

ALL_DNX_CHIPS = 1
ALL_DNXF_CHIPS = 1
#ALL_DPP_CHIPS = 1
#ALL_DFE_CHIPS = 1
override SBX_CHIPS=

VENDOR_LIST = BROADCOM

#VALGRIND_DIR = /projects/NTSW_SW_USRS/common/tools/valgrind-3.11.0

CFGFLAGS += -D__DUNE_LINUX_BCM_CPU_PCIE__
CFGFLAGS += -D_SIMPLE_MEMORY_ALLOCATION_=9
CFGFLAGS += -DUSE_LINUX_BDE_MMAP=1
#CFGFLAGS += -DUSE_EXTERNAL_MEM_CHECKING=1
CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT
CFGFLAGS += -DBCM_WARM_BOOT_API_TEST
CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT_SW_DUMP
CFGFLAGS += -DSERDES_API_FLOATING_POINT
CFGFLAGS += -DBCM_EASY_RELOAD_WB_COMPAT_SUPPORT
CFGFLAGS += -DBCM_CONTROL_API_TRACKING
#CFGFLAGS += -DUSE_VALGRIND_CLIENT_REQUESTS -I$(VALGRIND_DIR) -I$(VALGRIND_DIR)/include
CFGFLAGS += -DDNX_INTERNAL -Werror=pointer-sign
CFGFLAGS += -DSOC_PCI_DEBUG
#
# Indicate we want to use our string utilities (such as strcmp) and
# not the standard C-runtime-library. 
# This is because it seems that, for this compiler, the runtime-library accesses
# memory in resolution of 8 bytes, which collides with Valgrind.
#
CFGFLAGS += -DUSE_CUSTOM_STR_UTILS=1

#Flags to prevent gcc 8.1.0 new warnings to appear as errors.
#(All these flags have been removed so as to activate the maximal types of
#warning)
#
#DEBUG_CFLAGS= -Wno-error=stringop-overflow
#DEBUG_CFLAGS= -Wno-misleading-indentation

#LINK_STATIC := 0
KBP_DEVICE := KBP_ALG

VENDOR_LIST=CUSTOMER78 BROADCOM DNX
