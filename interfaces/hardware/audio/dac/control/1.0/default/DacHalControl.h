/*
 * Copyright (C) 2020 The LineageOS Project
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

#ifndef VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACHALCONTROL_H
#define VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACHALCONTROL_H

#include <vendor/lge/hardware/audio/dac/control/1.0/IDacHalControl.h>

#include <android/hardware/audio/2.0/IDevice.h>
#include <android/hardware/audio/2.0/IDevicesFactory.h>
#include <android/hardware/audio/4.0/IDevice.h>
#include <android/hardware/audio/4.0/IDevicesFactory.h>
#include <android/hardware/audio/5.0/IDevice.h>
#include <android/hardware/audio/5.0/IDevicesFactory.h>
#include <android/hardware/audio/6.0/IDevice.h>
#include <android/hardware/audio/6.0/IDevicesFactory.h>

#include <unordered_set>

#include "Constants.h"

namespace vendor {
namespace lge {
namespace hardware {
namespace audio {
namespace dac {
namespace control {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::vendor::lge::hardware::audio::dac::control::V1_0::HalFeature;
using ::vendor::lge::hardware::audio::dac::control::V1_0::FeatureStates;
using ::vendor::lge::hardware::audio::dac::control::V1_0::FeatureState;

enum AudioVersion { V2_0, V4_0, V5_0, V6_0 };

class DacHalControl : public IDacHalControl {
  public:
    DacHalControl();

    Return<void> getSupportedHalFeatures(getSupportedHalFeatures_cb _hidl_cb) override;

    Return<void> getSupportedHalFeatureValues(HalFeature feature, getSupportedHalFeatureValues_cb _hidl_cb) override;

    Return<bool> setFeatureValue(HalFeature feature, int32_t value) override;

    Return<int32_t> getFeatureValue(HalFeature feature) override;

    std::map<HalFeature, FeatureStates> mSupportedStates;
    std::vector<HalFeature> mSupportedHalFeatures;
  private:

     android::sp<::android::hardware::audio::V2_0::IDevice> mAudioDevice_V2_0;
     android::sp<::android::hardware::audio::V2_0::IDevicesFactory> mAudioDevicesFactory_V2_0;
     android::sp<::android::hardware::audio::V4_0::IDevice> mAudioDevice_V4_0;
     android::sp<::android::hardware::audio::V4_0::IDevicesFactory> mAudioDevicesFactory_V4_0;
     android::sp<::android::hardware::audio::V5_0::IDevice> mAudioDevice_V5_0;
     android::sp<::android::hardware::audio::V5_0::IDevicesFactory> mAudioDevicesFactory_V5_0;
     android::sp<::android::hardware::audio::V6_0::IDevice> mAudioDevice_V6_0;
     android::sp<::android::hardware::audio::V6_0::IDevicesFactory> mAudioDevicesFactory_V6_0;

     AudioVersion usedVersion;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace control
}  // namespace dac
}  // namespace audio
}  // namespace hardware
}  // namespace lge
}  // namespace vendor

#endif  // VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACHALCONTROL_H
