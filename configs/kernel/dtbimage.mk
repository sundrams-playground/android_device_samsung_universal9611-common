#
# Copyright (C) 2024-2026 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#
# Builds dtb.img for Pie launched devices which flash it to a dedicated
# "dtb" partition. Same as the default vendor/lineage kernel.mk rule, plus
# an AVB hash footer so the descriptor can be chained into vbmeta.
#

MKDTBOIMG := $(HOST_OUT_EXECUTABLES)/mkdtboimg$(HOST_EXECUTABLE_SUFFIX)

$(DTB_OUT):
	mkdir -p $(DTB_OUT)

$(INSTALLED_DTBIMAGE_TARGET): $(MKDTBOIMG) $(AVBTOOL) $(DTC) $(DTB_OUT)
	@echo "Building dtb.img"
	$(hide) find $(DTB_OUT)/arch/$(KERNEL_ARCH)/boot/dts -type f -name "*.dtb" | xargs rm -f
	$(call make-kernel-config,$(DTB_OUT),$(ALL_KERNEL_DEFCONFIG_SRCS))
	$(call make-dtb-target,$(TARGET_KERNEL_DTB))
	$(MKDTBOIMG) cfg_create $@ $(BOARD_DTB_CFG) -d $(DTB_OUT)/arch/$(KERNEL_ARCH)/boot/dts
	$(AVBTOOL) add_hash_footer \
	    --image $@ \
	    --partition_size $(BOARD_DTBIMG_PARTITION_SIZE) \
	    --partition_name dtb
	$(hide) touch -c $(DTB_OUT)

# vbmeta includes the dtb descriptors
$(PRODUCT_OUT)/vbmeta.img: $(INSTALLED_DTBIMAGE_TARGET)
