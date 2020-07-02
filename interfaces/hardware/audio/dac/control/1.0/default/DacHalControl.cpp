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

#include "DacHalControl.h"

#include <android-base/logging.h>
#include <android-base/strings.h>
#include <cutils/properties.h>

#include <fstream>
#include <unistd.h>

namespace vendor {
namespace lge {
namespace hardware {
namespace audio {
namespace dac {
namespace control {
namespace V1_0 {
namespace implementation {

static constexpr int32_t MAX_BALANCE_VALUE = 0;
static constexpr int32_t MIN_BALANCE_VALUE = -12;

static std::vector<KeyValue> quaddac_states = {{"Off", "0"}, {"On", "1"}};

static std::vector<KeyValue> sound_presets = {{"Normal", "0"},
                                              {"Enhanced", "1"},
                                              {"Detailed", "2"},
                                              {"Live", "3"},
                                              {"Bass", "4"}};

static std::vector<KeyValue> digital_filters = {{"Short", "0"},
                                                {"Sharp", "1"},
                                                {"Slow", "2"}};

DacHalControl::DacHalControl() {
    mAudioDevicesFactory = android::DevicesFactoryHalInterface::create();
    
    if(mAudioDevicesFactory == nullptr) {
        LOG(ERROR) << "mAudioDevicesFactory null, aborting";
        return;
    }

    LOG(INFO) << "DAC Control HAL: created DevicesFactoryHalInterface";

    mAudioDevicesFactory->openDevice("primary", &mAudioDevice);

    if(mAudioDevice == nullptr) {
        LOG(ERROR) << "mAudioDevice null, aborting";
        return;
    }
    LOG(INFO) << "DAC Control HAL: opened primary audio device";

    /* Quad DAC */
    mSupportedHalFeatures.push_back(HalFeature::QuadDAC);
    FeatureStates quaddac_fstates;
    quaddac_fstates.states = hidl_vec<KeyValue> {quaddac_states};
    for(auto e : quaddac_fstates.states) {
        LOG(INFO) << "quaddac_fstates: " << e.name << ":" << e.value;
    }
    mSupportedStates.emplace(HalFeature::QuadDAC, quaddac_fstates);

    /* Digital Filter */
    mSupportedHalFeatures.push_back(HalFeature::DigitalFilter);
    FeatureStates digfilter_fstates;
    digfilter_fstates.states = hidl_vec<KeyValue> {digital_filters};
    mSupportedStates.emplace(HalFeature::DigitalFilter, digfilter_fstates);

    /* Sound Presets */
    mSupportedHalFeatures.push_back(HalFeature::SoundPreset);
    FeatureStates soundpresets_fstates;
    soundpresets_fstates.states = hidl_vec<KeyValue> {sound_presets};
    mSupportedStates.emplace(HalFeature::SoundPreset, soundpresets_fstates);

    /* Balance Left */
    mSupportedHalFeatures.push_back(HalFeature::BalanceLeft);
    FeatureStates balanceleft_fstates;
    balanceleft_fstates.range.max = MAX_BALANCE_VALUE;
    balanceleft_fstates.range.min = MIN_BALANCE_VALUE;
    balanceleft_fstates.range.step = 1;
    mSupportedStates.emplace(HalFeature::BalanceLeft, balanceleft_fstates);

    /* Balance Right */
    mSupportedHalFeatures.push_back(HalFeature::BalanceRight);
    FeatureStates balanceright_fstates;
    balanceright_fstates.range.max = MAX_BALANCE_VALUE;
    balanceright_fstates.range.min = MIN_BALANCE_VALUE;
    balanceright_fstates.range.step = 1;
    mSupportedStates.emplace(HalFeature::BalanceRight, balanceright_fstates);

    setFeatureValue(HalFeature::QuadDAC, getFeatureValue(HalFeature::QuadDAC));
    setFeatureValue(HalFeature::DigitalFilter, getFeatureValue(HalFeature::DigitalFilter));
    setFeatureValue(HalFeature::SoundPreset, getFeatureValue(HalFeature::SoundPreset));
    setFeatureValue(HalFeature::BalanceLeft, getFeatureValue(HalFeature::BalanceLeft));
    setFeatureValue(HalFeature::BalanceRight, getFeatureValue(HalFeature::BalanceRight));
}

Return<void> DacHalControl::getSupportedHalFeatures(getSupportedHalFeatures_cb _hidl_cb) {
    
    std::vector<HalFeature> ret;
    for(auto entry : mSupportedHalFeatures) {
        LOG(INFO) << "DacHalControl::getSupportedHalFeatures: feature: " << std::to_string(static_cast<int32_t>(entry));
        ret.push_back(entry);
    }
    LOG(INFO) << "DacHalControl::getSupportedHalFeatures: size: " << std::to_string(static_cast<int32_t>(ret.size()));
    _hidl_cb(ret);
    
    return Void();
}

Return<void> DacHalControl::getSupportedHalFeatureValues(HalFeature feature, getSupportedHalFeatureValues_cb _hidl_cb) {
    std::map<HalFeature, FeatureStates>::iterator it;
    it = mSupportedStates.find(feature);
    if (it != mSupportedStates.end()) {
        _hidl_cb(it->second);
    } else {
        LOG(ERROR) << "DacHalControl::getSupportedHalFeatureValues: tried to get values for unsupported Feature...";
    }
    
    return Void();
}

Return<bool> DacHalControl::setFeatureValue(HalFeature feature, int32_t value) {
    
    if(std::find(mSupportedHalFeatures.begin(), mSupportedHalFeatures.end(), feature) == mSupportedHalFeatures.end()) {
        LOG(ERROR) << "DacHalControl::setFeatureValue: tried to set value for unsupported Feature...";
        return false;
    }

    KeyValue kv;
    std::string property;
    if(feature == HalFeature::QuadDAC) {
        kv.name = DAC_COMMAND;
        property = PROPERTY_HIFI_DAC_ENABLED;
        if(value == 0) {
            kv.value = SET_DAC_OFF_COMMAND;
        } else if(value == 1) {
            kv.value = SET_DAC_ON_COMMAND;
        }
    } else {
        switch(feature) {
            case HalFeature::DigitalFilter: {
                kv.name = SET_DIGITAL_FILTER_COMMAND;
                property = PROPERTY_DIGITAL_FILTER;
                break;
            }
            case HalFeature::SoundPreset: {
                kv.name = SET_SOUND_PRESET_COMMAND;
                property = PROPERTY_SOUND_PRESET;
                break;
            }
            case HalFeature::BalanceLeft: {
                kv.name = SET_LEFT_BALANCE_COMMAND;
                property = PROPERTY_LEFT_BALANCE;
                break;
            }
            case HalFeature::BalanceRight: {
                kv.name = SET_RIGHT_BALANCE_COMMAND;
                property = PROPERTY_RIGHT_BALANCE;
                break;
            }
            default: return false;
        }
        kv.value = std::to_string(value);
    }
    android::String8 command;
    command.append(kv.name.c_str());
    command.append("=");
    command.append(kv.value.c_str());
    LOG(INFO) << "DacHalControl::setFeatureValue: command: " << command.string();
    android::status_t ret = mAudioDevice->setParameters(command);

    if(ret == android::OK) {
        if(feature == HalFeature::QuadDAC) {
            if(value == 0) {
                property_set(property.c_str(), PROPERTY_VALUE_HIFI_DAC_DISABLED);
            } else if(value == 1) {
                property_set(property.c_str(), PROPERTY_VALUE_HIFI_DAC_ENABLED);
            }
        } else {
            property_set(property.c_str(), kv.value.c_str());
        }
        return true;
    } else {
        return false;
    }
}

Return<int32_t> DacHalControl::getFeatureValue(HalFeature feature) {
    if(std::find(mSupportedHalFeatures.begin(), mSupportedHalFeatures.end(), feature) == mSupportedHalFeatures.end()) {
        LOG(ERROR) << "DacHalControl::getFeatureValue: tried to set value for unsupported Feature...";
        return -1;
    }
    int32_t ret;
    std::string property;
    char value[PROPERTY_VALUE_MAX];
    if(feature == HalFeature::QuadDAC) {
        property = PROPERTY_HIFI_DAC_ENABLED;
        property_get(property.c_str(), value, "off");
        if(strcmp(value, "off") == 0) {
            ret = 0;
        } else if(strcmp(value, "on") == 0) {
            ret = 1;
        } else {
            ret = 0;
        }
    } else {
        switch(feature) {
            case HalFeature::DigitalFilter: {
                property = PROPERTY_DIGITAL_FILTER;
                break;
            }
            case HalFeature::SoundPreset: {
                property = PROPERTY_SOUND_PRESET;
                break;
            }
            case HalFeature::BalanceLeft: {
                property = PROPERTY_LEFT_BALANCE;
                break;
            }
            case HalFeature::BalanceRight: {
                property = PROPERTY_RIGHT_BALANCE;
                break;
            }
            default: return false;
        }
        property_get(property.c_str(), value, "0");
        ret = std::stoi(value);
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
