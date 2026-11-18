#
# Copyright (C) 2024-2026 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#
# Include from the AndroidBoard.mk of Pie launched devices.
#

# Ship dtb.img as radio image so releasetools can flash it to the dtb partition
INSTALLED_RADIOIMAGE_TARGET += $(PRODUCT_OUT)/dtb.img
