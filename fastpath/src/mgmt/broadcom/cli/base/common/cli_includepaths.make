

ifeq ($(L7_OS_VERSION),-D_L7_OS_LINUX_)
CONFIG=linux
else
CONFIG=vxworks
endif


CLI_INC_PATHS= -I$(EMWEB_DIR)/$(CONFIG) -I$(EMWEB_DIR)/config.$(CONFIG) \
				-I$(EMWEB_DIR)/websrc/include -I$(EMWEB_DIR)/websrc/server \
				-I$(EMWEB_DIR)/lib -I$(EMWEB_DIR)/websrc/telnet \
				-I$(CLI_DIR)/../cli_web_mgr/include 

CLI_INC_PATHS+= -I$(CLI_DIR)/base/infrastructure \
	-I$(CLI_DIR)/base/include \
	-I$(CLI_DIR)/switching/include \
	-I$(CLI_DIR)/security/include \
	-I$(CLI_DIR)/routing/include 
       

