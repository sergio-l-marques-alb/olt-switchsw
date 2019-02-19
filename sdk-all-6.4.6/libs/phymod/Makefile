#
# $Id$
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
#
LOCALDIR = libs/phymod

ifndef PHYMOD
PHYMOD = $(CURDIR)
endif

include ${PHYMOD}/make/config.mk

# Core library
VPATH = acc core diag
VSRCS = $(wildcard acc/*.c) $(wildcard core/*.c) $(wildcard diag/*.c)

# Symbolic debug
VPATH += sym sym/chip
VSRCS += $(wildcard sym/*.c) $(wildcard sym/chip/*.c)

# PHY simulators
VPATH += sim sim/chip
VSRCS += $(wildcard sim/*.c) $(wildcard sim/chip/*.c)

# PHY util
VPATH += util 
VSRCS += $(wildcard util/*.c) 

# Eagle chip support
VPATH += chip/eagle chip/eagle/tier2 chip/eagle/tier1
VSRCS += $(wildcard chip/eagle/*.c)
VSRCS += $(wildcard chip/eagle/tier2/*.c)
VSRCS += $(wildcard chip/eagle/tier1/*.c)

# Falcon chip support
VPATH += chip/falcon chip/falcon/tier2 chip/falcon/tier1
VSRCS += $(wildcard chip/falcon/*.c)
VSRCS += $(wildcard chip/falcon/tier2/*.c)
VSRCS += $(wildcard chip/falcon/tier1/*.c)

# QSGMII/Eagle chip support
VPATH += chip/qsgmiie chip/qsgmiie/tier2 chip/qsgmiie/tier1
VSRCS += $(wildcard chip/qsgmiie/*.c)
VSRCS += $(wildcard chip/qsgmiie/tier2/*.c)
VSRCS += $(wildcard chip/qsgmiie/tier1/*.c)

# TSC/Eagle chip support
VPATH += chip/tsce chip/tsce/tier2 chip/tsce/tier1
VSRCS += $(wildcard chip/tsce/*.c)
VSRCS += $(wildcard chip/tsce/tier2/*.c)
VSRCS += $(wildcard chip/tsce/tier1/*.c)

# TSC/Falcon chip support
VPATH += chip/tscf chip/tscf/tier2 chip/tscf/tier1
VSRCS += $(wildcard chip/tscf/*.c)
VSRCS += $(wildcard chip/tscf/tier2/*.c)
VSRCS += $(wildcard chip/tscf/tier1/*.c)

# Viper chip support
VPATH += chip/viper chip/viper/tier2 chip/viper/tier1
VSRCS += $(wildcard chip/viper/*.c)
VSRCS += $(wildcard chip/viper/tier2/*.c)
VSRCS += $(wildcard chip/viper/tier1/*.c)

# Furia chip support
VPATH += chip/furia chip/furia/tier2 chip/furia/tier1 chip/furia/tier1/falcon_furia_src
VSRCS += $(wildcard chip/furia/*.c)
VSRCS += $(wildcard chip/furia/tier2/*.c)
VSRCS += $(wildcard chip/furia/tier1/*.c)
VSRCS += $(wildcard chip/furia/tier1/falcon_furia_src/falcon_furia_functions.c)
VSRCS += $(wildcard chip/furia/tier1/falcon_furia_src/falcon_furia_dependencies.c)

# QTC chip support
#VPATH += chip/qtce chip/qtce/tier2 chip/qtce/tier1
#VSRCS += $(wildcard chip/qtce/*.c)
#VSRCS += $(wildcard chip/qtce/tier2/*.c)
#VSRCS += $(wildcard chip/qtce/tier1/*.c)

# Sesto chip support
VPATH += chip/sesto chip/sesto/tier2 chip/sesto/tier1 chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src chip/sesto/tier1/sesto_serdes/merlin_sesto_src
VSRCS += $(wildcard chip/sesto/*.c)
VSRCS += $(wildcard chip/sesto/tier2/*.c)
VSRCS += $(wildcard chip/sesto/tier1/*.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_functions.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_dependencies.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/merlin_sesto_src/merlin_sesto_functions.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/merlin_sesto_src/merlin_sesto_dependencies.c)

# quadra28 chip support
VPATH += chip/quadra28 chip/quadra28/tier2 chip/quadra28/tier1 chip/quadra28/tier1/quadra28_serdes
VSRCS += $(wildcard chip/quadra28/*.c)
VSRCS += $(wildcard chip/quadra28/tier2/*.c)
VSRCS += $(wildcard chip/quadra28/tier1/*.c)
VSRCS += $(wildcard chip/quadra28/tier1/quadra28_serdes/merlin_quadra28_functions.c)
VSRCS += $(wildcard chip/quadra28/tier1/quadra28_serdes/merlin_quadra28_dependencies.c)


LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libphymod
include ${PHYMOD}/make/lib.mk

include ${PHYMOD}/make/depend.mk
