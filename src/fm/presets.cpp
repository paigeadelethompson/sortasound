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

#include "fm/presets.hpp"
#include <algorithm>
#include <stdexcept>

namespace toybasic {

/**
 * @brief Constructor for PresetManager
 * 
 * Initializes the preset manager and loads all available presets.
 * This includes creating the default preset configurations for various
 * instrument types like piano, bass, lead, pad, bell, pluck, brass, and flute.
 */
PresetManager::PresetManager() {
    initializePresets();
}

/**
 * @brief Get a preset configuration by index
 * 
 * Retrieves a preset configuration from the internal preset list by its index.
 * The index must be valid (0 to preset count - 1).
 * 
 * @param index The index of the preset to retrieve
 * @return Reference to the preset configuration
 * @throws std::out_of_range if index is invalid
 */
const FMPresetConfig& PresetManager::getPreset(int index) const {
    if (index < 0 || index >= static_cast<int>(presets_.size())) {
        throw std::out_of_range("Preset index out of range");
    }
    return presets_[index];
}

/**
 * @brief Get a preset configuration by name
 * 
 * Searches for a preset configuration by its name and returns a reference to it.
 * The search is case-sensitive.
 * 
 * @param name The name of the preset to retrieve
 * @return Reference to the preset configuration
 * @throws std::invalid_argument if preset name is not found
 */
const FMPresetConfig& PresetManager::getPreset(const std::string& name) const {
    auto it = std::find_if(presets_.begin(), presets_.end(),
        [&name](const FMPresetConfig& preset) { return preset.name == name; });
    
    if (it == presets_.end()) {
        throw std::invalid_argument("Preset not found: " + name);
    }
    return *it;
}

/**
 * @brief Get the total number of available presets
 * 
 * Returns the count of all available preset configurations.
 * 
 * @return The number of presets available
 */
int PresetManager::getPresetCount() const {
    return static_cast<int>(presets_.size());
}

/**
 * @brief Get all preset names
 * 
 * Returns a vector containing the names of all available presets.
 * 
 * @return Vector of preset names
 */
std::vector<std::string> PresetManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : presets_) {
        names.push_back(preset.name);
    }
    return names;
}

/**
 * @brief Apply a preset to a synthesizer channel by index
 * 
 * Applies the preset configuration at the specified index to the given
 * synthesizer channel. This configures all operator parameters, algorithm,
 * and effects settings.
 * 
 * @param synth Reference to the synthesizer to configure
 * @param channel The channel number to apply the preset to
 * @param presetIndex The index of the preset to apply
 */
void PresetManager::applyPreset(FMSynthesizer& synth, int channel, int presetIndex) const {
    const FMPresetConfig& preset = getPreset(presetIndex);
    applyPreset(synth, channel, preset);
}

/**
 * @brief Apply a preset to a synthesizer channel by name
 * 
 * Applies the preset configuration with the specified name to the given
 * synthesizer channel. This configures all operator parameters, algorithm,
 * and effects settings.
 * 
 * @param synth Reference to the synthesizer to configure
 * @param channel The channel number to apply the preset to
 * @param presetName The name of the preset to apply
 */
void PresetManager::applyPreset(FMSynthesizer& synth, int channel, const std::string& presetName) const {
    const FMPresetConfig& preset = getPreset(presetName);
    applyPreset(synth, channel, preset);
}

/**
 * @brief Apply a preset configuration to a synthesizer channel
 * 
 * Applies the given preset configuration to the specified synthesizer channel.
 * This configures all operator parameters, algorithm, and effects settings
 * according to the preset.
 * 
 * @param synth Reference to the synthesizer to configure
 * @param channel The channel number to apply the preset to
 * @param preset The preset configuration to apply
 */
void PresetManager::applyPreset(FMSynthesizer& synth, int channel, const FMPresetConfig& preset) const {
    synth.setAlgorithm(channel, preset.algorithm);
    
    std::array<double, 6> frequencies;
    std::array<double, 6> amplitudes;
    std::array<double, 6> modulationIndices;
    std::array<WaveformType, 6> waveforms;
    std::array<double, 6> attacks;
    std::array<double, 6> decays;
    std::array<double, 6> sustains;
    std::array<double, 6> releases;
    
    for (int i = 0; i < 6; i++) {
        frequencies[i] = preset.operators[i].frequency;
        amplitudes[i] = preset.operators[i].amplitude;
        modulationIndices[i] = preset.operators[i].modulationIndex;
        waveforms[i] = preset.operators[i].waveform;
        attacks[i] = preset.operators[i].attack;
        decays[i] = preset.operators[i].decay;
        sustains[i] = preset.operators[i].sustain;
        releases[i] = preset.operators[i].release;
    }
    
    synth.setPresetConfig(frequencies, amplitudes, modulationIndices, waveforms,
                         attacks, decays, sustains, releases);
    
    synth.setMasterVolume(preset.masterVolume);
    synth.setReverb(preset.reverb);
    synth.setChorus(preset.chorus);
    synth.setDistortion(preset.distortion);
}

void PresetManager::initializePresets() {
    presets_.clear();
    
    presets_.push_back(createSinePiano());
    presets_.push_back(createSineBass());
    presets_.push_back(createSineLead());
    presets_.push_back(createSinePad());
    presets_.push_back(createSineBell());
    presets_.push_back(createSinePluck());
    presets_.push_back(createSineBrass());
    presets_.push_back(createSineFlute());
}

/**
 * @brief Create an operator configuration with specified parameters
 * 
 * Creates a new operator configuration with the given frequency, amplitude,
 * modulation index, waveform type, and envelope parameters.
 * 
 * @param freq Frequency ratio for the operator
 * @param amp Amplitude level (0.0 to 1.0)
 * @param mod Modulation index
 * @param wave Waveform type (sine, sawtooth, square, triangle)
 * @param att Attack time in seconds
 * @param dec Decay time in seconds
 * @param sus Sustain level (0.0 to 1.0)
 * @param rel Release time in seconds
 * @return Configured operator configuration
 */
FMPresetConfig::OperatorConfig PresetManager::createOperator(double freq, double amp, double mod, 
                                                      WaveformType wave, double att, double dec, 
                                                      double sus, double rel) {
    return {freq, amp, mod, wave, att, dec, sus, rel};
}

/**
 * @brief Create a sine wave piano preset
 * 
 * Creates a piano preset using sine waves with algorithm 2 (parallel modulators).
 * The preset features a bright, percussive sound with moderate reverb.
 * 
 * @return Configured piano preset
 */
FMPresetConfig PresetManager::createSinePiano() {
    FMPresetConfig preset;
    preset.name = "PIANO";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_2_PARALLEL_5AND6TO4TO3TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.3;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.1, 0.7, 0.5);
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.08, 0.5, 0.4);
    preset.operators[2] = createOperator(3.0, 0.4, 1.0, WaveformType::SINE, 0.001, 0.06, 0.3, 0.3);
    preset.operators[3] = createOperator(4.0, 0.3, 0.8, WaveformType::SINE, 0.001, 0.04, 0.2, 0.2);
    preset.operators[4] = createOperator(0.5, 0.2, 2.0, WaveformType::SINE, 0.001, 0.02, 0.1, 0.1);
    preset.operators[5] = createOperator(0.25, 0.1, 1.5, WaveformType::SINE, 0.001, 0.01, 0.05, 0.05);
    
    return preset;
}

/**
 * @brief Create a sine wave bass preset
 * 
 * Creates a bass preset using sine waves with algorithm 1 (serial chain).
 * The preset features a deep, punchy sound with light reverb.
 * 
 * @return Configured bass preset
 */
FMPresetConfig PresetManager::createSineBass() {
    FMPresetConfig preset;
    preset.name = "BASS";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_1_SERIAL_6TO5TO4TO3TO2TO1);
    preset.masterVolume = 0.9;
    preset.reverb = 0.2;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.05, 0.8, 0.3);
    preset.operators[1] = createOperator(2.0, 0.6, 1.2, WaveformType::SINE, 0.001, 0.04, 0.6, 0.25);
    preset.operators[2] = createOperator(3.0, 0.4, 0.8, WaveformType::SINE, 0.001, 0.03, 0.4, 0.2);
    preset.operators[3] = createOperator(4.0, 0.3, 0.5, WaveformType::SINE, 0.001, 0.02, 0.2, 0.15);
    preset.operators[4] = createOperator(5.0, 0.2, 0.3, WaveformType::SINE, 0.001, 0.01, 0.1, 0.1);
    preset.operators[5] = createOperator(6.0, 0.1, 0.2, WaveformType::SINE, 0.001, 0.005, 0.05, 0.05);
    
    return preset;
}

/**
 * @brief Create a sine wave lead preset
 * 
 * Creates a lead preset using sine waves with algorithm 7 (triple split).
 * The preset features a bright, cutting sound with chorus and light distortion.
 * 
 * @return Configured lead preset
 */
FMPresetConfig PresetManager::createSineLead() {
    FMPresetConfig preset;
    preset.name = "LEAD";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_7_SERIAL_6TO5TO4_AND_6TO3_AND_6TO2TO1);
    preset.masterVolume = 0.9;
    preset.reverb = 0.2;
    preset.chorus = 0.3;
    preset.distortion = 0.1;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.05, 0.8, 0.3);
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.04, 0.6, 0.25);
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.001, 0.03, 0.4, 0.2);
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.001, 0.02, 0.2, 0.15);
    preset.operators[4] = createOperator(5.0, 0.2, 0.8, WaveformType::SINE, 0.001, 0.01, 0.1, 0.1);
    preset.operators[5] = createOperator(6.0, 0.1, 0.5, WaveformType::SINE, 0.001, 0.005, 0.05, 0.05);
    
    return preset;
}

/**
 * @brief Create a sine wave pad preset
 * 
 * Creates a pad preset using sine waves with algorithm 32 (all parallel carriers).
 * The preset features a lush, sustained sound with heavy reverb and chorus.
 * 
 * @return Configured pad preset
 */
FMPresetConfig PresetManager::createSinePad() {
    FMPresetConfig preset;
    preset.name = "PAD";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_32_ALL_6_PARALLEL_CARRIERS);
    preset.masterVolume = 0.7;
    preset.reverb = 0.6;
    preset.chorus = 0.4;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.8, 0.0, WaveformType::SINE, 0.01, 0.2, 0.8, 1.0);
    preset.operators[1] = createOperator(2.0, 0.6, 0.0, WaveformType::SINE, 0.01, 0.15, 0.6, 0.8);
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.01, 0.1, 0.4, 0.6);
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.01, 0.08, 0.3, 0.4);
    preset.operators[4] = createOperator(5.0, 0.2, 0.0, WaveformType::SINE, 0.01, 0.05, 0.2, 0.3);
    preset.operators[5] = createOperator(6.0, 0.1, 0.0, WaveformType::SINE, 0.01, 0.03, 0.1, 0.2);
    
    return preset;
}

/**
 * @brief Create a sine wave bell preset
 * 
 * Creates a bell preset using sine waves with algorithm 8 (quad split).
 * The preset features a bright, percussive sound with moderate reverb.
 * 
 * @return Configured bell preset
 */
FMPresetConfig PresetManager::createSineBell() {
    FMPresetConfig preset;
    preset.name = "BELL";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_8_SERIAL_6TO5_AND_6TO4_AND_6TO3_AND_6TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.5;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.1, 0.0, 0.8);
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.08, 0.0, 0.6);
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.001, 0.06, 0.0, 0.4);
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.001, 0.04, 0.0, 0.3);
    preset.operators[4] = createOperator(5.0, 0.2, 0.0, WaveformType::SINE, 0.001, 0.02, 0.0, 0.2);
    preset.operators[5] = createOperator(6.0, 0.1, 0.8, WaveformType::SINE, 0.001, 0.01, 0.0, 0.1);
    
    return preset;
}

/**
 * @brief Create a sine wave pluck preset
 * 
 * Creates a pluck preset using sine waves with algorithm 3 (split path).
 * The preset features a sharp, percussive sound with light reverb.
 * 
 * @return Configured pluck preset
 */
FMPresetConfig PresetManager::createSinePluck() {
    FMPresetConfig preset;
    preset.name = "PLUCK";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_3_SERIAL_6TO5TO4TO3TO2_AND_6TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.2;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.01, 0.0, 0.2);
    preset.operators[1] = createOperator(2.0, 0.6, 1.0, WaveformType::SINE, 0.001, 0.01, 0.0, 0.15);
    preset.operators[2] = createOperator(3.0, 0.4, 0.8, WaveformType::SINE, 0.001, 0.01, 0.0, 0.1);
    preset.operators[3] = createOperator(4.0, 0.3, 0.6, WaveformType::SINE, 0.001, 0.01, 0.0, 0.08);
    preset.operators[4] = createOperator(5.0, 0.2, 0.4, WaveformType::SINE, 0.001, 0.01, 0.0, 0.05);
    preset.operators[5] = createOperator(6.0, 0.1, 0.3, WaveformType::SINE, 0.001, 0.01, 0.0, 0.03);
    
    return preset;
}

/**
 * @brief Create a sine wave brass preset
 * 
 * Creates a brass preset using sine waves with algorithm 4 (split path).
 * The preset features a bright, punchy sound with moderate reverb.
 * 
 * @return Configured brass preset
 */
FMPresetConfig PresetManager::createSineBrass() {
    FMPresetConfig preset;
    preset.name = "BRASS";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_4_SERIAL_6TO5TO4TO3_AND_6TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.4;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.01, 0.1, 0.7, 0.4);
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.01, 0.08, 0.5, 0.3);
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.01, 0.06, 0.3, 0.2);
    preset.operators[3] = createOperator(4.0, 0.3, 0.8, WaveformType::SINE, 0.01, 0.04, 0.2, 0.15);
    preset.operators[4] = createOperator(5.0, 0.2, 0.5, WaveformType::SINE, 0.01, 0.02, 0.1, 0.1);
    preset.operators[5] = createOperator(6.0, 0.1, 0.3, WaveformType::SINE, 0.01, 0.01, 0.05, 0.05);
    
    return preset;
}

/**
 * @brief Create a sine wave flute preset
 * 
 * Creates a flute preset using sine waves with algorithm 5 (split path).
 * The preset features a smooth, breathy sound with moderate reverb.
 * 
 * @return Configured flute preset
 */
FMPresetConfig PresetManager::createSineFlute() {
    FMPresetConfig preset;
    preset.name = "FLUTE";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_5_SERIAL_6TO5TO4_AND_6TO3TO2TO1);
    preset.masterVolume = 0.7;
    preset.reverb = 0.4;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    preset.operators[0] = createOperator(1.0, 0.8, 0.0, WaveformType::SINE, 0.01, 0.05, 0.8, 0.2);
    preset.operators[1] = createOperator(2.0, 0.6, 0.8, WaveformType::SINE, 0.01, 0.04, 0.6, 0.15);
    preset.operators[2] = createOperator(3.0, 0.4, 0.5, WaveformType::SINE, 0.01, 0.03, 0.4, 0.1);
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.01, 0.02, 0.2, 0.08);
    preset.operators[4] = createOperator(5.0, 0.2, 0.3, WaveformType::SINE, 0.01, 0.01, 0.1, 0.05);
    preset.operators[5] = createOperator(6.0, 0.1, 0.2, WaveformType::SINE, 0.01, 0.005, 0.05, 0.03);
    
    return preset;
}

} // namespace toybasic