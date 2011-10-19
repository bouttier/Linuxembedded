#############################################################
#
# minigui
#
#############################################################

MINIGUI_VERSION = 0.1
MINIGUI_SOURCE = minigui-$(MINIGUI_VERSION).tar.gz
#MINIGUI_SITE = 
MINIGUI_INSTALL_STAGING = NO
MINIGUI_DEPENDENCIES = freetype tslib

define MINIGUI_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

#define MINIGUI_INSTALL_TARGET_CMDS
#	$(INSTALL) -D -m 0755 $(@D)/libfoo.so* $(TARGET_DIR)/usr/lib
#	$(INSTALL) -d -m 0755 $(TARGET_DIR)/etc/foo.d
#endef

$(eval $(call GENTARGETS,package,minigui))
