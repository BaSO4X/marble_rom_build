// Copyright (C) 2021 The Android Open Source Project
// SPDX-License-Identifier: Apache-2.0

package android.hardware.ir;

import android.hardware.ir.ConsumerIrFreqRange;

@VintfStability
interface IConsumerIr {
    ConsumerIrFreqRange[] getCarrierFreqs();
    void transmit(in int carrierFreqHz, in int[] pattern);
}
