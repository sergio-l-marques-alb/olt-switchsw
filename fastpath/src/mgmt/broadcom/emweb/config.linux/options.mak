#
# Release: R6_0_1
#
# CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
# THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
# AND OTHER US AND INTERNATIONAL PATENTS PENDING.
# 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
# 
# Notice to Users of this Software Product:
# 
# This software product of Virata Corporation ("Virata"), 5 Clock Tower
# Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
# source and object code format embodies valuable intellectual property 
# including trade secrets, copyrights and patents which are the exclusive
# proprietary property of Virata. Access, use, reproduction, modification
# disclsoure and distribution are expressly prohibited unless authorized
# in writing by Virata.  Under no circumstances may you copy this
# software or distribute it to anyone else by any means whatsoever except in
# strict accordance with a license agreement between Virata and your
# company.  This software is also protected under patent, trademark and 
# copyright laws of the United States and foreign countries, as well as 
# under the license agreement, against any unauthorized copying, modification,
# or distribution.
# Please see your supervisor or manager regarding any questions that you may
# have about your right to use this software product.  Thank you for your
# cooperation.
# 
# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
# All Rights Reserved
# 
# Virata Corporation
# 5 Clock Tower Place
# Suite 400
# Maynard, MA  01754
# 
# Voice: +1 (978) 461-0888
# Fax:   +1 (978) 461-2080
# 
# http://www.emweb.com/
#   support@agranat.com
#     sales@agranat.com
#
# EmWeb Makefile Options
#


##############################################################################
##
## EmWeb/UPnP
##
## Un-comment the following lines if the EmWeb/UPnP product is installed.
## You must also un-comment the lines for EmWeb/Client and EmWeb/XMLP.
##
##START-OPTION-UPNP ##########################################################
## ews.a is added at beginning because it has references int ews_upnp.a
##    that will not be resolved unless ews.a comes first.
#
#LIBS += $(OD)/ews.a $(OD)/ews_upnp.a
#
#IFLAGS += -I$(SRC)/upnp
#
#EWFLAGS_$(CONFIG) += --map $(SRC)/upnp/EmWebUPnP.map
#
#$(OD)/ews_upnp.a: .FORCE
#	$(MAKE) -C $(SRC)/upnp CONFIG=$(CONFIG)
#
## Make sure that the EmWeb/Client and EmWeb/XML sections below are enabled

##############################################################################
##
## EmWeb/Client
##
## Un-comment the following lines if the EmWeb/Client product is installed,
## or if the EmWeb/UPnP product is installed.
##
##START-OPTION-CLIENT ########################################################
#LIBS += $(OD)/ews_client.a
#IFLAGS += -I$(SRC)/client
#
#$(OD)/ews_client.a: .FORCE
#	$(MAKE) -C $(SRC)/client CONFIG=$(CONFIG)
##END-OPTION-CLIENT ##########################################################

##############################################################################
##
## EmWeb/XMLP
##
## Un-comment the following lines if the EmWeb/XMLP product is installed,
## or if the EmWeb/UPnP product is installed.
##
##START-OPTION-XMLP ##########################################################
#LIBS += $(OD)/ews.a
#LIBS += $(OD)/xmlp.a
#
#IFLAGS += -I$(SRC)/xmlp
#
#$(OD)/xmlp.a: .FORCE
#	$(MAKE) -C $(SRC)/xmlp CONFIG=$(CONFIG)
##END-OPTION-XMLP ############################################################
##END-OPTION-UPNP ############################################################

##############################################################################
##
## EmWeb/Mail
##
## Un-comment the following lines if the EmWeb/Mail product is installed.
##
##START-OPTION-MAIL ##########################################################
#LIBS += $(OD)/ews_smtp.a
#
#$(OD)/ews_smtp.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/mail CONFIG=$(CONFIG)
##END-OPTION-MAIL ############################################################

#############################################################################
#
# EmWeb/CLI
#
# Un-comment the following lines if the EmWeb/CLI product is installed.
#
#START-OPTION-CLI ###########################################################
#LIBS += $(OD)/ews_telnet.a
#IFLAGS += -I$(FP_LSRC)/telnet
#
#$(OD)/ews_telnet.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/telnet CONFIG=$(CONFIG)
##### Begin src/mgmt/emweb/websrc/telnet
localsrc := $(FP_LSRC)/telnet
SOURCES += $(wildcard $(localsrc)/*.c)
IFLAGS  += -I$(mgmtbase)/util 
IFLAGS  += -I$(emwebbase)/linux
##### End src/mgmt/emweb/websrc/telnet
#END-OPTION-CLI #############################################################

##############################################################################
##
## EmWeb Envoy namespace
##
## Un-comment the following lines if the Envoy namespace is needed
##
##START-OPTION-ENVOY #########################################################
#LIBS += $(OD)/ewa_envoy.a $(OD)/ews_snmp.a
#
#$(OD)/ewa_envoy.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/envoy CONFIG=$(CONFIG)
#
#$(OD)/ews_snmp.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/snmp/lib CONFIG=$(CONFIG)
##END-OPTION-ENVOY ###########################################################

##############################################################################
##
## EmWeb Emanate (Epic) namespace
##
## Un-comment the following lines if the Emanate namespace is needed
##
##START-OPTION-EPIC ##########################################################
#LIBS += $(OD)/ewa_epic.a $(OD)/ews_snmp.a
#
#$(OD)/ewa_epic.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/emanate CONFIG=$(CONFIG)
#
#$(OD)/ews_snmp.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/snmp/lib CONFIG=$(CONFIG)
##END-OPTION-EPIC ############################################################


##############################################################################
##
## EmWeb Ati (Ati Nucleus) namespace
##
## Un-comment the following lines if the Ati Nucleus namespace is needed
##
##START-OPTION-ATI ###########################################################
#LIBS += $(OD)/ewa_ati.a $(OD)/ews_snmp.a
#
#$(OD)/ewa_ati.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/ati CONFIG=$(CONFIG)
#
#$(OD)/ews_snmp.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/snmp/lib CONFIG=$(CONFIG)
##END-OPTION-ATI #############################################################


##############################################################################
##
## EmWeb/SSL
##
## Un-comment the following lines if the EmWeb/SSL product is installed
##
## Define location of SSL library, SSL include files and defines required
## to build Emweb Server with SSL package.
##
##START-OPTION-SSL ###########################################################
## the following XCFLAGS are needed for RSA SSL
#
#XCFLAGS += -UEMWEB_MALLOC -USSLC_COMPAT_INCLUDE -I./../sslrsa/include
#IFLAGS += -I./../sslrsa/include
#
#XLIBS += ../sslrsa/lib/libsslc.a
#
#LIBS += $(OD)/ews_rsassl.a
#
#$(OD)/ews_rsassl.a: .FORCE
#	$(MAKE) -C $(FP_LSRC)/rsa CONFIG=$(CONFIG)
##END-OPTION-SSL #############################################################

##############################################################################
##
## EmWeb/GuiKit
##
## Remove the comment character from the appropriate line(s) if the
## EmWeb/GuiKit product is installed.
##
##START-OPTION-GUIKIT ########################################################
## Uncomment the following line if using the obfuscated Javascript.
#GUIKIT = $(ROOT)/guikit/javascript/obfuscated
## Uncomment the following line if using the non-obfuscated Javascript.
#GUIKIT = $(ROOT)/guikit/javascript/plaintext

## Uncomment the following line if including the GUIkit demonstration
## and documentation in the archive.
#GUIKIT += $(ROOT)/guikit

## Uncomment the following line when building any GUIkit application.
#CONTENT += $(GUIKIT)

## Uncomment the following line if installing ScreenPak
#EWFLAGS_$(CONFIG) += -as
##END-OPTION-GUIKIT ##########################################################
