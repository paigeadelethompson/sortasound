/*
 * SortaSound - Advanced FM Synthesizer
 * Copyright (C) 2024  Paige Thompson <paige@paige.bio>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <string>
#include "fm.hpp"

namespace toybasic {
namespace PresetConstants {
    constexpr int ALGORITHM_SERIAL = 0;
    constexpr int ALGORITHM_PARALLEL_MODULATORS = 1;
    constexpr int ALGORITHM_PARALLEL_OPERATORS = 7;
    constexpr int ALGORITHM_FOUR_PARALLEL = 8;
    constexpr int ALGORITHM_FOUR_PARALLEL_CHAINS = 15;
    
    constexpr double VOLUME_QUIET = 0.1;
    constexpr double VOLUME_MEDIUM = 0.5;
    constexpr double VOLUME_LOUD = 0.8;
    constexpr double VOLUME_MAX = 0.9;
    
    constexpr double FREQ_FUNDAMENTAL = 1.0;
    constexpr double FREQ_OCTAVE = 2.0;
    constexpr double FREQ_FIFTH = 1.5;
    constexpr double FREQ_THIRD = 1.25;
    constexpr double FREQ_HARMONIC_2 = 2.0;
    constexpr double FREQ_HARMONIC_3 = 3.0;
    constexpr double FREQ_HARMONIC_4 = 4.0;
    constexpr double FREQ_HARMONIC_5 = 5.0;
    constexpr double FREQ_SUB = 0.5;
    constexpr double FREQ_QUARTER = 0.25;
    
    constexpr double AMP_SILENT = 0.0;
    constexpr double AMP_QUIET = 0.2;
    constexpr double AMP_MEDIUM = 0.4;
    constexpr double AMP_LOUD = 0.6;
    constexpr double AMP_VERY_LOUD = 0.8;
    constexpr double AMP_MAX = 0.9;
    
    constexpr double MOD_NONE = 0.0;
    constexpr double MOD_LIGHT = 1.0;
    constexpr double MOD_MEDIUM = 2.0;
    constexpr double MOD_HEAVY = 3.0;
    constexpr double MOD_EXTREME = 5.0;
    
    constexpr double ENV_INSTANT = 0.001;
    constexpr double ENV_VERY_FAST = 0.01;
    constexpr double ENV_FAST = 0.05;
    constexpr double ENV_MEDIUM = 0.1;
    constexpr double ENV_SLOW = 0.3;
    constexpr double ENV_VERY_SLOW = 0.5;
    constexpr double ENV_LONG = 1.0;
    constexpr double ENV_VERY_LONG = 2.0;
    
    constexpr double SUSTAIN_NONE = 0.0;
    constexpr double SUSTAIN_LOW = 0.2;
    constexpr double SUSTAIN_MEDIUM = 0.4;
    constexpr double SUSTAIN_HIGH = 0.6;
    constexpr double SUSTAIN_VERY_HIGH = 0.8;
    constexpr double SUSTAIN_MAX = 0.9;
    
    constexpr double EFFECT_NONE = 0.0;
    constexpr double EFFECT_LIGHT = 0.1;
    constexpr double EFFECT_MEDIUM = 0.3;
    constexpr double EFFECT_HEAVY = 0.6;
    constexpr double EFFECT_MAX = 0.8;
}

struct FMPresetConfig {
    std::string name;
    int algorithm;
    
    struct OperatorConfig {
        double frequency;
        double amplitude;
        double modulationIndex;
        WaveformType waveform;
        
        double attack;
        double decay;
        double sustain;
        double release;
    };
    
    std::array<OperatorConfig, 6> operators;
    
    double masterVolume;
    double reverb;
    double chorus;
    double distortion;
};

class PresetManager {
public:
    PresetManager();
    
    const FMPresetConfig& getPreset(int index) const;
    
    const FMPresetConfig& getPreset(const std::string& name) const;
    
    int getPresetCount() const;
    
    std::vector<std::string> getPresetNames() const;
    
    void applyPreset(FMSynthesizer& synth, int channel, int presetIndex) const;
    void applyPreset(FMSynthesizer& synth, int channel, const std::string& presetName) const;
    void applyPreset(FMSynthesizer& synth, int channel, const FMPresetConfig& preset) const;

private:
    std::vector<FMPresetConfig> presets_;
    void initializePresets();
    
    FMPresetConfig::OperatorConfig createOperator(double freq, double amp, double mod, 
                                          WaveformType wave, double att, double dec, 
                                          double sus, double rel);
    
    FMPresetConfig createSinePiano();
    FMPresetConfig createSineBass();
    FMPresetConfig createSineLead();
    FMPresetConfig createSinePad();
    FMPresetConfig createSineBell();
    FMPresetConfig createSinePluck();
    FMPresetConfig createSineBrass();
    FMPresetConfig createSineFlute();
};

enum class PresetCategory {
    PERCUSSION,
    BASS,
    BRASS,
    WOODWIND,
    STRINGS,
    KEYBOARD,
    SYNTH,
    EFFECTS,
    EXPERIMENTAL
};

std::vector<int> getPresetsByCategory(PresetCategory category);

}
