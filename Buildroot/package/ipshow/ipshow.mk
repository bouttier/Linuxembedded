########################################
#
# ipshow
#
########################################

IPSHOW_VERSION:=1.0
IPSHOW_DIR:=$(BUILD_DIR)/ipshow-$(IPSHOW_VERSION)
IPSHOW_BINARY:=ipshow
IPSHOW_TARGET_BINAY:=usr/bin/ipshow
IPSHOW_SOURCE:=ipshow-$(IPSHOW_VERSION).tar.gz

$(IPSHOW_DIR)/.source:
	$(ZCAT) $(DL_DIR)/$(IPSHOW_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(IPSHOW_DIR)/$(IPSHOW_BINARY): $(IPSHOW_DIR)/.source
	$(MAKE) CC=$(TARGET_CC) -C $(IPSHOW_DIR)

$(TARGET_DIR)/$(IPSHOW_TARGET_BINARY): $(IPSHOW_DIR)/$(IPSHOW_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(IPSHOW_DIR) install-strip

ipshow: tslib freetype $(TARGET_DIR)/$(IPSHOW_TARGET_BINARY)



#ipshow-clean:
#	$(MAKE) prefix=$(TARGET_DIR)/usr -C $(IPSHOW_DIR) uninstall



########################################
ifeq ($(BR2_PACKAGE_IPSHOW),y)
TARGET+=ipshow
endif
