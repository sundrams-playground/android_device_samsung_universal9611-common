#
# Copyright (C) 2024-2026 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Board configuration for Pie launched exynos9611 devices (e.g. m30s).
#
# These devices have no super partition. Dynamic partitions are retrofitted
# by using the stock "system" block device as the (single) super block device,
# while boot uses header v1 and the DTB lives in its own "dtb" partition.
#

COMMON_PATH := device/samsung/universal9611-common

## Boot Image
# Must be set before including the common configuration since
# BOARD_MKBOOTIMG_ARGS is immediately expanded there.
BOARD_BOOTIMG_HEADER_VERSION := 1

## Inherit common configuration
include $(COMMON_PATH)/BoardConfigCommon.mk

## Dynamic Partitions (retrofit on the system block device)
BOARD_SUPER_PARTITION_METADATA_DEVICE := system

## DTB
# Header v1 boot images can't carry a DTB, build a standalone dtb.img
# (with AVB hash footer) and ship it as a radio image.
BOARD_CUSTOM_DTBIMG_MK := $(COMMON_PATH)/configs/kernel/dtbimage.mk
# Pie bootloaders match the dtb entry on the stock hw rev range (0x0-0xff)
BOARD_DTB_CFG := $(COMMON_PATH)/configs/kernel/exynos9611-pie.cfg
BOARD_DTBIMG_PARTITION_SIZE := 8388608
BOARD_PACK_RADIOIMAGES += dtb.img

## Fstab
# No dedicated metadata partition, use omr instead.
$(call soong_config_set_bool,fstab_universal9611,use_omr_as_metadata,true)

## Partitions
# The system block device is much smaller than super on Q launched devices,
# so the common 800MB per partition doesn't fit. Use the LineageOS reserved
# sizes instead, which leave room for GApps in product.
ifeq (,$(filter true, $(WITHOUT_RESERVED_SIZE) $(WITH_GMS)))
BOARD_PRODUCTIMAGE_PARTITION_RESERVED_SIZE := 1188036608
BOARD_SYSTEMIMAGE_PARTITION_RESERVED_SIZE := 94371840
BOARD_SYSTEM_EXTIMAGE_PARTITION_RESERVED_SIZE := 94371840
endif

## Recovery
TARGET_RECOVERY_UPDATER_LIBS := librecovery_updater_universal9611

## Verified Boot
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --include_descriptors_from_image $(PRODUCT_OUT)/dtb.img
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --flags 3
