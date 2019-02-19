# Makefile,v 1.2 Broadcom SDK $
# $Copyright: Copyright 2015 Broadcom Corporation.
# This program is the proprietary software of Broadcom Corporation
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
# $Id: Makefile,v 1.6 2012/09/24 15:23:22 rgodwin Exp $
# Makefile for Caladan3 tests Library
#
LOCALDIR = src/appl/test/caladan3/ucode

#
# Java Settings
#
JAVAPATH ?= /tools/bin
JAVA = $(JAVAPATH)/java -Xmx1024M

ifeq ($(SDK),)
$(error SDK env not set, Please run this within SDK tree)
endif

#
# ASM3 Tools Path Settings
#
CALADAN3_TOOLS_PATH ?= $(SDK)/../caladan_ucode/tools/java_tools
CLASSPATH = $(CALADAN3_TOOLS_PATH)/lib

ifeq "$(wildcard $(CLASSPATH)/*.jar)" ""
$(error Cannot find ASM tools at $(CLASSPATH), check your CALADAN3_TOOLS_PATH)
endif
#
# ASM3 Settings -- DO NOT MODIFY
#
ASM3_LIBS += antlr-3.3-complete.jar c3tools.jar
ASM3_LIBS_EXPANDED = $(addprefix $(CLASSPATH)/, $(ASM3_LIBS))

#
# ASM3 Commands
#
BLANK = $(eval) $(eval)
CALADAN3_CLASSPATH = -classpath $(subst $(BLANK),:,$(ASM3_LIBS_EXPANDED))
BRCM_MDE_PKG_PREFIX = com.broadcom.mde
CALADAN3_ASM = $(JAVA) $(CALADAN3_CLASSPATH) $(BRCM_MDE_PKG_PREFIX).c3plc.Asm3 

TMU_TAPS_SRC = tmu_taps_ut
TMU_TAPS1_SRC = tmu_taps1_ut
TMU_TAPSP_SRC = tmu_tapsp_ut
TMU_TAPS_IPV6_SRC = tmu_taps_ipv6_ut
TMU_HASH_SRC = tmu_hash_ut
TMU_HASH144_SRC = tmu_hash144_ut
TMU_DM_SRC = tmu_dm_ut

tmutaps: $(TMU_TAPS_SRC).asm
	@echo "Assembling $(TMU_TAPS_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_TAPS_SRC).asm -o1 $(TMU_TAPS_SRC).txt

tmutaps1: $(TMU_TAPS1_SRC).asm
	@echo "Assembling $(TMU_TAPS1_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_TAPS1_SRC).asm -o1 $(TMU_TAPS1_SRC).txt

tmutapsp: $(TMU_TAPSP_SRC).asm
	@echo "Assembling $(TMU_TAPS1P_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_TAPSP_SRC).asm -o1 $(TMU_TAPSP_SRC).txt

tmutapsipv6: $(TMU_TAPS_IPV6_SRC).asm
	@echo "Assembling $(TMU_TAPS_IPV6_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_TAPS_IPV6_SRC).asm -o1 $(TMU_TAPS_IPV6_SRC).txt

tmuhash: $(TMU_HASH_SRC).asm
	@echo "Assembling $(TMU_HASH_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_HASH_SRC).asm -o1 $(TMU_HASH_SRC).txt

tmuhash144: $(TMU_HASH144_SRC).asm
	@echo "Assembling $(TMU_HASH144_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_HASH144_SRC).asm -o1 $(TMU_HASH144_SRC).txt

tmudm: $(TMU_DM_SRC).asm
	@echo "Assembling $(TMU_DM_SRC).asm"
	@$(CALADAN3_ASM) -ia $(TMU_DM_SRC).asm -o1 $(TMU_DM_SRC).txt

all: tmutaps tmuhash tmuhash144 tmudm tmutaps1 tmutapsipv6


clean:
	@echo "Cleaning Test ucode area"
	@rm -f *.txt
