/*
 * Copyright (C) 2022-2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <vector>

#include <android-base/properties.h>
#include <edify/expr.h>
#include <liblp/liblp.h>
#include <otautil/error_code.h>

using android::fs_mgr::BlockDeviceInfo;
using android::fs_mgr::PartitionOpener;

/*
 * Pie launched devices have no super partition. Their system partition is
 * used as the super block device instead (androidboot.super_partition=system).
 * Flash an initial (empty) dynamic partitions table on first install.
 */
Value* RetrofitDynamicPartitionsFn(const char* /* name */, State* state,
                                   const std::vector<std::unique_ptr<Expr>>& /* argv */) {
    std::string super_partition = android::base::GetProperty("ro.boot.super_partition", "");

    if (super_partition.empty()) {
        return ErrorAbort(state, kVendorFailure,
                          "The current recovery does not seem to support dynamic partitions, "
                          "please update.");
    }

    std::unique_ptr<android::fs_mgr::LpMetadata> pt =
            android::fs_mgr::ReadMetadata(super_partition, 0);
    if (!pt) {  // No dynamic partitions metadata yet - flashing initial metadata is required
        state->updater->UiPrint("Flashing initial dynamic partitions partition table...");

        pt = android::fs_mgr::ReadFromImageFile("/tmp/super_empty.img");
        if (!pt) {
            return ErrorAbort(state, kFileOpenFailure, "Failed to read /tmp/super_empty.img");
        }

        // super_empty.img is built for the smallest supported system block
        // device, grow it to the real block device size.
        PartitionOpener opener;
        BlockDeviceInfo info;
        if (opener.GetInfo(super_partition, &info)) {
            if (pt->block_devices[0].size < info.size) {
                pt->block_devices[0].size = info.size;
            }
        }

        if (!android::fs_mgr::FlashPartitionTable(super_partition, *pt.get())) {
            return ErrorAbort(state, kSetMetadataFailure,
                              "Failed to flash partition table of super partition: %s.",
                              super_partition.c_str());
        }
    }

    return StringValue("0");
}

void Register_librecovery_updater_universal9611() {
    RegisterFunction("universal9611.retrofit_dynamic_partitions", RetrofitDynamicPartitionsFn);
}
