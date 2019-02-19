# $Id: Makefile,v 1.9 Broadcom SDK $
#
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

SDK :=$(shell if [ -n "$$SDK" ] ; then\
	echo $$SDK;\
	else\
	cd $(dir $(lastword $(MAKEFILE_LIST))); while /usr/bin/test ! -e RELEASE ; do \
	dir=`cd ../;pwd`;			\
	if [ "$$dir" = "/" ] ; then		\
	   echo Cannot find SDK in $(lastword $(MAKEFILE_LIST)) 1>&2;		\
	   exit 1;				\
	fi ;					\
	cd $$dir;				\
	done ;					\
	pwd;					\
	fi)

ifeq ($(SDK),)					
$(error Please run this in a tree)		
endif 				

export SDK

LOCALDIR = src/soc/sbx/fe2k_common

include ${SDK}/make/Make.config

FE2K_INC  = ${SDK}/include/soc/sbx/fe2k_common

#GENERATE_C_FILES=1
#GEN_INCS = $(FE2K_INC)

#export GENERATE_C_FILES
#export GEN_INCS

FE2K_ZFS      = $(wildcard *.zf)
FE2K_ZFCS     = $(addsuffix .c, $(basename ${FE2K_ZFS})) \
		$(addsuffix Console.c, $(basename ${FE2K_ZFS}))
FE2K_ZFHXS    = $(addprefix ${FE2K_INC}/, $(addsuffix .hx, $(basename ${FE2K_ZFS})) \
			 $(addsuffix Console.hx, $(basename ${FE2K_ZFS})))
# Note diff from gu2: FE2K_ZFOS     = $(addsuffix .o, $(basename ${FE2K_ZFS}))
FE2K_ZFOS     = $(addsuffix .o, $(basename ${FE2K_ZFCS}))

MMAPHX   = ${FE2K_INC}/sbG2FeMemMap.hx

BOBJS    += $(addprefix ${BLDDIR}/, ${FE2K_ZFOS})

lib = libsoc_sbx_fe2k_common

include ${SDK}/make/Make.lib

FE2K_ZF_GEN   = ${PERL} ${SDK}/tools/zFrameCodeGen.pl
MMAP_GEN = ${PERL} ${SDK}/tools/GenMemMap.pl

# Note diff from gu2:  %.c %Console.c ${FE2K_INC}/%Console.hx : %.zf
#%.c %Console.c ${FE2K_INC}/%.hx ${FE2K_INC}/%Console.hx : %.zf
#ifdef QUIET
#	@${ECHO} Converting zframe $(notdir $^)
#endif
#	$Q${FE2K_ZF_GEN} -s -z . -p . -g $^ -t c
#	$Q${CP} $*.cx $*.c
#	$Q${CP} $*Console.cx $*Console.c
#	$Q${CP} $*.hx ${FE2K_INC}
#	$Q${CP} $*Console.hx ${FE2K_INC}
#	$Q${RM} $*.cx $*Console.cx $*.hx $*Console.hx


${FE2K_INC}/%.hx: %.mmap
ifdef QUIET
	@${ECHO} Converting mmap $(notdir $^)
endif
	$Q$(MMAP_GEN) $^ $@


#clean::
#	$Q$(RM) $(FE2K_ZFHXS) $(FE2K_ZFCS)

${BLDDIR}/%.d : %.c ${BLDDIR}/.tree ${FE2K_ZFHXS}
	@${ECHO} Dependencies for ${LOCALDIR}/$<
	@$(MAKEDEP) "$@" "$@" "$(DEPEND)"

.SECONDARY:: ${FE2K_ZFCS} ${FE2K_ZFHXS}

include ${SDK}/make/Make.depend

