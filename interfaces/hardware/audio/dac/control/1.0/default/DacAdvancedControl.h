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

#ifndef VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACADVANCEDCONTROL_H
#define VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACADVANCEDCONTROL_H

#include <vendor/lge/hardware/audio/dac/control/1.0/IDacAdvancedControl.h>

#include <android-base/strings.h>

#include "Constants.h"

namespace vendor {
namespace lge {
namespace hardware {
namespace audio {
namespace dac {
namespace control {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::vendor::lge::hardware::audio::dac::control::V1_0::AdvancedFeature;
using ::vendor::lge::hardware::audio::dac::control::V1_0::FeatureStates;
using ::vendor::lge::hardware::audio::dac::control::V1_0::FeatureState;
using ::vendor::lge::hardware::audio::dac::control::V1_0::KeyValue;
using ::vendor::lge::hardware::audio::dac::control::V1_0::Range;

class DacAdvancedControl : public IDacAdvancedControl {
  public:
    DacAdvancedControl();
  
    Return<void> getSupportedAdvancedFeatures(getSupportedAdvancedFeatures_cb _hidl_cb) override;

    Return<void> getSupportedAdvancedFeatureValues(AdvancedFeature feature, getSupportedAdvancedFeatureValues_cb _hidl_cb) override;

    Return<bool> setFeatureValue(AdvancedFeature feature, int32_t value) override;

    Return<int32_t> getFeatureValue(AdvancedFeature feature) override;
    
  private:
    FeatureStates getAvcVolumeStates();
    FeatureStates getHifiModeStates();
    bool writeAvcVolumeState(int32_t value);
    bool writeHifiModeState(int32_t value);
    int32_t getAvcVolumeState();
    int32_t getHifiModeState();
  
    std::string mDacBasePath;
    std::string avcPath;
    std::string hifiPath;
    
    std::vector<AdvancedFeature> mSupportedAdvancedFeatures;
    
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace control
}  // namespace dac
}  // namespace audio
}  // namespace hardware
}  // namespace lge
}  // namespace vendor

#endif  // VENDOR_LGE_HARDWARE_AUDIO_DAC_CONTROL_V1_0_DACADVANCEDCONTROL_H
