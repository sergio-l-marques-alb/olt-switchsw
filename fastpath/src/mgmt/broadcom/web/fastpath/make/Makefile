# This is a package-level makefile used to build a partially-linked package library
#
#   - PKGID designates package identifier used for customizing common make var names
#   - PKGNAME designates the output name of this package
#
PKGID   := XWEB
PKGNAME := $(ROOT_$(PKGID))

DST_DIR=$(FP_ROOT)/$(OUT_OBJECTS)/xui
MAKE_DIR=$(FP_ROOT)/src/mgmt/broadcom/web/fastpath/make

# Identify makefile containing specific build rules to be used.
#
# Note: This makefile must be included within make.magic.pkg to define
#       the rules within the right scope of makefile processing.
#
BLDRULESFILE = $(FP_ROOT)/src/xui/make/makerules.pkg

dep_debug-variables : debug-variables
	$(NOOP)

dep_bldlib :
	$(DBG) echo $(WEB_CONTENT) > $(DST_DIR)/web_content.txt
	$(DBG) echo $(JAVA_CONTENT) > $(DST_DIR)/java_content.txt
	@echo Setting up access permissions ...
	$(DBG) $(SHELL) $(MAKE_DIR)/html-access-setup.sh $(PRODUCT_PLATFORM)
	@echo Setting up Web Menu ...
	$(DBG) $(SHELL) $(MAKE_DIR)/web-menu-setup.sh $(DST_DIR)

dep_clean :
	$(DBG) rm -f $(DST_DIR)/web_content.txt
	$(DBG) rm -f $(DST_DIR)/java_content.txt


# Do not change or move anything below this line.

# Include target package output settings
include $(FP_ROOT)/src/l7tools/build/make.hdr.pkg

# Define all source locations used to build this package library
include $(MAKE_DIR)/make.pkg
include $(MAKE_DIR)/make.html.pkg
include $(FP_ROOT)/src/mgmt/web/make/make.pkg

# Special magic to allow building from output location
include $(FP_ROOT)/src/l7tools/build/make.magic.pkg
