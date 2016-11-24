# $Id: Makefile,v 1.1 2011/04/18 17:11:01 mruas Exp $
# $Copyright: Copyright 2009 Broadcom Corporation.
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
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/esw

export BCM_HIDE_DISPATCHABLE=1

include ${SDK}/make/Make.config

ifdef BCM_5600_C0
SUBDIR_STRATA = strata
endif

ifdef BCM_5605_A0
SUBDIR_STRATA = strata
endif

ifdef BCM_5615_A0
SUBDIR_STRATA = strata
endif

ifdef BCM_5680_A0
SUBDIR_STRATA = strata
endif

ifdef BCM_5680_B0
SUBDIR_STRATA = strata
endif

ifdef BCM_5690_A0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
endif

ifdef BCM_5670_A0
SUBDIR_HERC = hercules
endif

ifdef BCM_5673_A0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
SUBDIR_LYNX = lynx
endif

ifdef BCM_5674_A0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
SUBDIR_LYNX = lynx
endif

ifdef BCM_5675_A0
SUBDIR_HERC = hercules
endif

ifdef BCM_5665_A0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
SUBDIR_TUCANA = tucana
endif

ifdef BCM_5665_B0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
SUBDIR_TUCANA = tucana
endif

ifdef BCM_5695_A0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
endif

ifdef BCM_56601_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56602_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56601_B0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56602_B0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56601_C0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56602_C0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_EASYRIDER = easyrider
endif

ifdef BCM_56504_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56504_B0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56102_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56304_B0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56112_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56314_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56580_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_BRADLEY = bradley
SUBDIR_HERC = hercules
SUBDIR_HUMV = humv
endif

ifdef BCM_56700_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_BRADLEY = bradley
SUBDIR_HERC = hercules
SUBDIR_HUMV = humv
endif

ifdef BCM_56800_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_BRADLEY = bradley
SUBDIR_HERC = hercules
SUBDIR_HUMV = humv
endif

ifdef BCM_5650_C0
SUBDIR_STRATA = strata
SUBDIR_DRACO = draco
SUBDIR_TUCANA = tucana
endif

ifdef BCM_56218_A0
SUBDIR_RAPTOR = raptor
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56514_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56624_A0
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56680_A0
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56624_B0
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56680_B0
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56224_A0
SUBDIR_RAPTOR = raptor
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56224_B0
SUBDIR_RAPTOR = raptor
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_53314_A0
SUBDIR_RAPTOR = raptor
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56820_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_BRADLEY = bradley
SUBDIR_HERC = hercules
SUBDIR_HUMV = humv
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_SCORPION = scorpion
endif

ifdef BCM_56725_A0
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
SUBDIR_BRADLEY = bradley
SUBDIR_HERC = hercules
SUBDIR_HUMV = humv
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_SCORPION = scorpion
SUBDIR_CONQUEROR = conqueror
endif

ifdef BCM_56634_A0
SUBDIR_TRIUMPH2 = triumph2
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56524_A0
SUBDIR_TRIUMPH2 = triumph2
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56685_A0
SUBDIR_TRIUMPH2 = triumph2
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

ifdef BCM_56334_A0
SUBDIR_ENDURO = enduro
SUBDIR_TRIUMPH2 = triumph2
SUBDIR_TRX = trx
SUBDIR_TRIUMPH = triumph
SUBDIR_BRADLEY = bradley
SUBDIR_FIREBOLT = firebolt
SUBDIR_DRACO = draco
endif

subdirs := \
	$(SUBDIR_STRATA) \
	$(SUBDIR_DRACO) \
	$(SUBDIR_HERC) \
	$(SUBDIR_LYNX) \
	$(SUBDIR_TUCANA) \
	$(SUBDIR_EASYRIDER) \
	$(SUBDIR_FIREBOLT) \
	$(SUBDIR_BRADLEY) \
	$(SUBDIR_HUMV) \
	$(SUBDIR_RAPTOR) \
	$(SUBDIR_TRX) \
	$(SUBDIR_TRIUMPH) \
	$(SUBDIR_SCORPION) \
	$(SUBDIR_CONQUEROR) \
        $(SUBDIR_TRIUMPH2) \
        $(SUBDIR_ENDURO) \
	$(SUBDIR_NONE)

include ${SDK}/make/Make.subdirs

lib = libbcm_esw
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
