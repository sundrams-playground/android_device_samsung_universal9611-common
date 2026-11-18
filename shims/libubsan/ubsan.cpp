/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdlib>

/*
 * Some older vendor blobs are built with CFI in diagnostic mode and import
 * the handler from libclang_rt.ubsan_standalone, which isn't shipped.
 * A failed CFI check is fatal anyway, so just abort.
 */
extern "C" void __ubsan_handle_cfi_check_fail_abort(void* /* data */, void* /* value */,
                                                    void* /* vtable_is_valid */) {
    abort();
}
