/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libinit_dalvik_heap.h>
#include <libinit_utils.h>
#include "vendor_init.h"

#include <android-base/properties.h>
#include <android-base/strings.h>

#include <vector>

using android::base::GetProperty;
using android::base::Split;
using android::base::StartsWith;

/*
 * Devices with NFC and non-NFC variants (e.g. SM-M307F/SM-M307FN) list the
 * bootloader/model prefixes of their NFC variants in ro.vendor.nfc.bootloader_prefixes.
 * Select the NFC hardware SKU from the bootloader version, so the NFC features
 * and HAL are only enabled where the chip exists.
 */
static void set_nfc_sku() {
    std::string prefixes = GetProperty("ro.vendor.nfc.bootloader_prefixes", "");
    if (prefixes.empty() || !GetProperty("ro.boot.product.hardware.sku", "").empty()) {
        return;
    }

    // Match against the bootloader version and the model (without "SM-")
    std::vector<std::string> ids;
    for (const auto& prop : {"ro.boot.bootloader", "ro.boot.em.model", "ro.boot.product.model"}) {
        std::string id = GetProperty(prop, "");
        if (StartsWith(id, "SM-")) {
            id = id.substr(3);
        }
        if (!id.empty()) {
            ids.push_back(id);
        }
    }

    std::string sku = "disabled";
    for (const auto& prefix : Split(prefixes, ",")) {
        for (const auto& id : ids) {
            if (!prefix.empty() && StartsWith(id, prefix)) {
                sku = GetProperty("ro.vendor.nfc.sku", "hcesim");
            }
        }
    }

    property_override("ro.boot.product.hardware.sku", sku);
}

void vendor_load_properties() {
    std::string model;

    model = GetProperty("ro.boot.product.model", "");
    if (model.empty()) {
       model = GetProperty("ro.boot.em.model", "");
    }
    set_ro_build_prop("model", model, true);
    set_ro_build_prop("product", model, false);

    set_dalvik_heap();
    set_nfc_sku();
}
