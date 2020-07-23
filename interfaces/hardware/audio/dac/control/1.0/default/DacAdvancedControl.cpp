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

#include "DacAdvancedControl.h"

#include <dirent.h>

#include <android-base/logging.h>
#include <android-base/file.h>
#include <cutils/properties.h>

#include <algorithm>
#include <fstream>
#include <string>

using android::base::ReadFileToString;
using android::base::Trim;
using android::base::WriteStringToFile;

namespace vendor {
namespace lge {
namespace hardware {
namespace audio {
namespace dac {
namespace control {
namespace V1_0 {
namespace implementation {

/*
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

DacAdvancedControl::DacAdvancedControl() {
    DIR* dir;
    for(int i = 0; i < 10; i++) {
        std::string tmp = COMMON_ES9218_PATH + std::to_string(i) + "-0048/";
        if( (dir = opendir(tmp.c_str())) != NULL) {
            mDacBasePath.append(tmp);
            closedir(dir);
            break;
        }
        closedir(dir);
    }
    if(mDacBasePath.empty()) {
        LOG(ERROR) << "DacAdvancedControl: No ES9218 path found, exiting...";
        return;
    }

    avcPath = std::string(mDacBasePath);
    avcPath.append(AVC_VOLUME);
    hifiPath = std::string(mDacBasePath);
    hifiPath.append(HIFI_MODE);

    struct stat buffer;
    if(stat(avcPath.c_str(), &buffer) == 0) {
        mSupportedAdvancedFeatures.push_back(AdvancedFeature::AVCVolume);
        writeAvcVolumeState(getAvcVolumeState());
    }

    if(stat(hifiPath.c_str(), &buffer) == 0) {
        mSupportedAdvancedFeatures.push_back(AdvancedFeature::HifiMode);
        writeHifiModeState(getHifiModeState());
    }
}

Return<void> DacAdvancedControl::getSupportedAdvancedFeatures(getSupportedAdvancedFeatures_cb _hidl_cb) {

    _hidl_cb(mSupportedAdvancedFeatures);

    return Void();
}

FeatureStates DacAdvancedControl::getAvcVolumeStates() {
    FeatureStates states;
    
    states.range.min = -24;
    states.range.max = 0;
    states.range.step = 1;
    
    return states;
}

static std::vector<KeyValue> hifi_modes = {{"Normal", "0"},
                                           {"High Impedance", "1"},
                                           {"AUX", "2"}};

FeatureStates DacAdvancedControl::getHifiModeStates() {
    FeatureStates states;
    
    states.states = hidl_vec<KeyValue> {hifi_modes};
    
    return states;
}

Return<void> DacAdvancedControl::getSupportedAdvancedFeatureValues(AdvancedFeature feature, getSupportedAdvancedFeatureValues_cb _hidl_cb) {

    if(std::find(mSupportedAdvancedFeatures.begin(), mSupportedAdvancedFeatures.end(), feature) != mSupportedAdvancedFeatures.end()) {
        switch(feature) {
            case AdvancedFeature::AVCVolume: {
                    _hidl_cb(getAvcVolumeStates());
                    break;
                }
            case AdvancedFeature::HifiMode: {
                    _hidl_cb(getHifiModeStates());
                    break;
                }
            default: break;
        }
    } else {
        LOG(ERROR) << "DacAdvancedControl::getSupportedAdvancedFeatureValues: tried to get values for unsupported Feature...";
    }
    
    return Void();
}

bool DacAdvancedControl::writeAvcVolumeState(int32_t value) {
    set(avcPath, (-1)*value); //we save it as the actual value, while the kernel requires a positive value
    return (bool)property_set(PROPERTY_HIFI_DAC_AVC_VOLUME, std::to_string(value).c_str());
}

bool DacAdvancedControl::writeHifiModeState(int32_t value) {
    set(hifiPath, value);
    return (bool)property_set(PROPERTY_HIFI_DAC_MODE, std::to_string(value).c_str());
}

Return<bool> DacAdvancedControl::setFeatureValue(AdvancedFeature feature, int32_t value) {

    bool feature_available = false;
    for(AdvancedFeature f : mSupportedAdvancedFeatures) {
        if(f == feature) {
            feature_available = true;
            break;
        }
    }
    if(!feature_available) {
        LOG(ERROR) << "DacAdvancedControl::setFeatureValue: tried to set value for unsupported Feature...";
        return false;
    }

    switch(feature) {
        case AdvancedFeature::AVCVolume: {
                return writeAvcVolumeState(value);
            }
        case AdvancedFeature::HifiMode: {
                return writeHifiModeState(value);
            }
        default: break;
    }
    return false;
}

int32_t DacAdvancedControl::getAvcVolumeState() {
    return property_get_int32(PROPERTY_HIFI_DAC_AVC_VOLUME, AVC_VOLUME_DEFAULT);
}

int32_t DacAdvancedControl::getHifiModeState() {
    return property_get_int32(PROPERTY_HIFI_DAC_MODE, HIFI_MODE_DEFAULT);
}

Return<int32_t> DacAdvancedControl::getFeatureValue(AdvancedFeature feature) {
    int32_t ret;

    bool feature_available = false;
    for(AdvancedFeature f : mSupportedAdvancedFeatures) {
        if(f == feature) {
            feature_available = true;
            break;
        }
    }
    if(!feature_available) {
        LOG(ERROR) << "DacAdvancedControl::getFeatureValue: tried to get value for unsupported Feature...";
        return -1;
    }

    switch(feature) {
        case AdvancedFeature::AVCVolume: {
                ret = getAvcVolumeState();
                break;
            }
        case AdvancedFeature::HifiMode: {
                ret = getHifiModeState();
                break;
            }
        default: break;
    }

    return ret;
}


}  // namespace implementation
}  // namespace V1_0
}  // namespace control
}  // namespace dac
}  // namespace audio
}  // namespace hardware
}  // namespace lge
}  // namespace vendor
