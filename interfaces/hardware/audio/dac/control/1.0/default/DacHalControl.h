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

#include <media/audiohal/DevicesFactoryHalInterface.h>
#include <media/audiohal/DeviceHalInterface.h>

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

     android::sp<android::DevicesFactoryHalInterface> mAudioDevicesFactory;
     android::sp<android::DeviceHalInterface> mAudioDevice;
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
