#include "presets.hpp"
#include <algorithm>
#include <stdexcept>

namespace toybasic {

PresetManager::PresetManager() {
    initializePresets();
}

const FMPresetConfig& PresetManager::getPreset(int index) const {
    if (index < 0 || index >= static_cast<int>(presets_.size())) {
        throw std::out_of_range("Preset index out of range");
    }
    return presets_[index];
}

const FMPresetConfig& PresetManager::getPreset(const std::string& name) const {
    auto it = std::find_if(presets_.begin(), presets_.end(),
        [&name](const FMPresetConfig& preset) { return preset.name == name; });
    
    if (it == presets_.end()) {
        throw std::invalid_argument("Preset not found: " + name);
    }
    return *it;
}

int PresetManager::getPresetCount() const {
    return static_cast<int>(presets_.size());
}

std::vector<std::string> PresetManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : presets_) {
        names.push_back(preset.name);
    }
    return names;
}

void PresetManager::applyPreset(FMSynthesizer& synth, int channel, int presetIndex) const {
    const FMPresetConfig& preset = getPreset(presetIndex);
    applyPreset(synth, channel, preset);
}

void PresetManager::applyPreset(FMSynthesizer& synth, int channel, const std::string& presetName) const {
    const FMPresetConfig& preset = getPreset(presetName);
    applyPreset(synth, channel, preset);
}

void PresetManager::applyPreset(FMSynthesizer& synth, int channel, const FMPresetConfig& preset) const {
    // Set algorithm for the channel
    synth.setAlgorithm(channel, preset.algorithm);
    
    // Apply preset configuration to the synthesizer
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
    
    // Set global parameters
    synth.setMasterVolume(preset.masterVolume);
    synth.setReverb(preset.reverb);
    synth.setChorus(preset.chorus);
    synth.setDistortion(preset.distortion);
}

void PresetManager::initializePresets() {
    presets_.clear();
    
    // SINE-ONLY PRESETS USING ALGORITHM CONSTANTS
    presets_.push_back(createSinePiano());
    presets_.push_back(createSineBass());
    presets_.push_back(createSineLead());
    presets_.push_back(createSinePad());
    presets_.push_back(createSineBell());
    presets_.push_back(createSinePluck());
    presets_.push_back(createSineBrass());
    presets_.push_back(createSineFlute());
}

// Helper function to create a basic operator config
FMPresetConfig::OperatorConfig PresetManager::createOperator(double freq, double amp, double mod, 
                                                      WaveformType wave, double att, double dec, 
                                                      double sus, double rel) {
    return {freq, amp, mod, wave, att, dec, sus, rel};
}

// SINE-ONLY PRESETS USING ALGORITHM CONSTANTS
FMPresetConfig PresetManager::createSinePiano() {
    FMPresetConfig preset;
    preset.name = "SINE PIANO";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_2_PARALLEL_5AND6TO4TO3TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.3;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    // Op 1: Main tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.1, 0.7, 0.5);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.08, 0.5, 0.4);
    // Op 3: Modulates op 2
    preset.operators[2] = createOperator(3.0, 0.4, 1.0, WaveformType::SINE, 0.001, 0.06, 0.3, 0.3);
    // Op 4: Modulates op 3
    preset.operators[3] = createOperator(4.0, 0.3, 0.8, WaveformType::SINE, 0.001, 0.04, 0.2, 0.2);
    // Op 5: Parallel modulator
    preset.operators[4] = createOperator(0.5, 0.2, 2.0, WaveformType::SINE, 0.001, 0.02, 0.1, 0.1);
    // Op 6: Parallel modulator
    preset.operators[5] = createOperator(0.25, 0.1, 1.5, WaveformType::SINE, 0.001, 0.01, 0.05, 0.05);
    
    return preset;
}

FMPresetConfig PresetManager::createSineBass() {
    FMPresetConfig preset;
    preset.name = "SINE BASS";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_1_SERIAL_6TO5TO4TO3TO2TO1);
    preset.masterVolume = 0.9;
    preset.reverb = 0.2;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    // Op 1: Main bass tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.05, 0.8, 0.3);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.2, WaveformType::SINE, 0.001, 0.04, 0.6, 0.25);
    // Op 3: Modulates op 2
    preset.operators[2] = createOperator(3.0, 0.4, 0.8, WaveformType::SINE, 0.001, 0.03, 0.4, 0.2);
    // Op 4: Modulates op 3
    preset.operators[3] = createOperator(4.0, 0.3, 0.5, WaveformType::SINE, 0.001, 0.02, 0.2, 0.15);
    // Op 5: Modulates op 4
    preset.operators[4] = createOperator(5.0, 0.2, 0.3, WaveformType::SINE, 0.001, 0.01, 0.1, 0.1);
    // Op 6: Modulates op 5
    preset.operators[5] = createOperator(6.0, 0.1, 0.2, WaveformType::SINE, 0.001, 0.005, 0.05, 0.05);
    
    return preset;
}

FMPresetConfig PresetManager::createSineLead() {
    FMPresetConfig preset;
    preset.name = "SINE LEAD";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_7_SERIAL_6TO5TO4_AND_6TO3_AND_6TO2TO1);
    preset.masterVolume = 0.9;
    preset.reverb = 0.2;
    preset.chorus = 0.3;
    preset.distortion = 0.1;
    
    // Op 1: Main lead tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.05, 0.8, 0.3);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.04, 0.6, 0.25);
    // Op 3: Parallel carrier
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.001, 0.03, 0.4, 0.2);
    // Op 4: Parallel carrier
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.001, 0.02, 0.2, 0.15);
    // Op 5: Modulates op 4
    preset.operators[4] = createOperator(5.0, 0.2, 0.8, WaveformType::SINE, 0.001, 0.01, 0.1, 0.1);
    // Op 6: Modulates op 5 and op 3 and op 2
    preset.operators[5] = createOperator(6.0, 0.1, 0.5, WaveformType::SINE, 0.001, 0.005, 0.05, 0.05);
    
    return preset;
}

FMPresetConfig PresetManager::createSinePad() {
    FMPresetConfig preset;
    preset.name = "SINE PAD";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_32_ALL_6_PARALLEL_CARRIERS);
    preset.masterVolume = 0.7;
    preset.reverb = 0.6;
    preset.chorus = 0.4;
    preset.distortion = 0.0;
    
    // Op 1: Fundamental
    preset.operators[0] = createOperator(1.0, 0.8, 0.0, WaveformType::SINE, 0.01, 0.2, 0.8, 1.0);
    // Op 2: Harmonic
    preset.operators[1] = createOperator(2.0, 0.6, 0.0, WaveformType::SINE, 0.01, 0.15, 0.6, 0.8);
    // Op 3: Harmonic
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.01, 0.1, 0.4, 0.6);
    // Op 4: Harmonic
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.01, 0.08, 0.3, 0.4);
    // Op 5: Harmonic
    preset.operators[4] = createOperator(5.0, 0.2, 0.0, WaveformType::SINE, 0.01, 0.05, 0.2, 0.3);
    // Op 6: Harmonic
    preset.operators[5] = createOperator(6.0, 0.1, 0.0, WaveformType::SINE, 0.01, 0.03, 0.1, 0.2);
    
    return preset;
}

FMPresetConfig PresetManager::createSineBell() {
    FMPresetConfig preset;
    preset.name = "SINE BELL";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_8_SERIAL_6TO5_AND_6TO4_AND_6TO3_AND_6TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.5;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    // Op 1: Main bell tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.1, 0.0, 0.8);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.001, 0.08, 0.0, 0.6);
    // Op 3: Parallel carrier
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.001, 0.06, 0.0, 0.4);
    // Op 4: Parallel carrier
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.001, 0.04, 0.0, 0.3);
    // Op 5: Parallel carrier
    preset.operators[4] = createOperator(5.0, 0.2, 0.0, WaveformType::SINE, 0.001, 0.02, 0.0, 0.2);
    // Op 6: Modulates op 5, op 4, op 3, and op 2
    preset.operators[5] = createOperator(6.0, 0.1, 0.8, WaveformType::SINE, 0.001, 0.01, 0.0, 0.1);
    
    return preset;
}

FMPresetConfig PresetManager::createSinePluck() {
    FMPresetConfig preset;
    preset.name = "SINE PLUCK";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_3_SERIAL_6TO5TO4TO3TO2_AND_6TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.2;
    preset.chorus = 0.0;
    preset.distortion = 0.0;
    
    // Op 1: Main pluck tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.001, 0.01, 0.0, 0.2);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.0, WaveformType::SINE, 0.001, 0.01, 0.0, 0.15);
    // Op 3: Modulates op 2
    preset.operators[2] = createOperator(3.0, 0.4, 0.8, WaveformType::SINE, 0.001, 0.01, 0.0, 0.1);
    // Op 4: Modulates op 3
    preset.operators[3] = createOperator(4.0, 0.3, 0.6, WaveformType::SINE, 0.001, 0.01, 0.0, 0.08);
    // Op 5: Modulates op 4
    preset.operators[4] = createOperator(5.0, 0.2, 0.4, WaveformType::SINE, 0.001, 0.01, 0.0, 0.05);
    // Op 6: Modulates op 5 and op 1
    preset.operators[5] = createOperator(6.0, 0.1, 0.3, WaveformType::SINE, 0.001, 0.01, 0.0, 0.03);
    
    return preset;
}

FMPresetConfig PresetManager::createSineBrass() {
    FMPresetConfig preset;
    preset.name = "SINE BRASS";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_4_SERIAL_6TO5TO4TO3_AND_6TO2TO1);
    preset.masterVolume = 0.8;
    preset.reverb = 0.4;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    // Op 1: Main brass tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.9, 0.0, WaveformType::SINE, 0.01, 0.1, 0.7, 0.4);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 1.5, WaveformType::SINE, 0.01, 0.08, 0.5, 0.3);
    // Op 3: Parallel carrier
    preset.operators[2] = createOperator(3.0, 0.4, 0.0, WaveformType::SINE, 0.01, 0.06, 0.3, 0.2);
    // Op 4: Modulates op 3
    preset.operators[3] = createOperator(4.0, 0.3, 0.8, WaveformType::SINE, 0.01, 0.04, 0.2, 0.15);
    // Op 5: Modulates op 4
    preset.operators[4] = createOperator(5.0, 0.2, 0.5, WaveformType::SINE, 0.01, 0.02, 0.1, 0.1);
    // Op 6: Modulates op 5 and op 2
    preset.operators[5] = createOperator(6.0, 0.1, 0.3, WaveformType::SINE, 0.01, 0.01, 0.05, 0.05);
    
    return preset;
}

FMPresetConfig PresetManager::createSineFlute() {
    FMPresetConfig preset;
    preset.name = "SINE FLUTE";
    preset.algorithm = static_cast<int>(FMAlgorithm::ALG_5_SERIAL_6TO5TO4_AND_6TO3TO2TO1);
    preset.masterVolume = 0.7;
    preset.reverb = 0.4;
    preset.chorus = 0.1;
    preset.distortion = 0.0;
    
    // Op 1: Main flute tone (carrier)
    preset.operators[0] = createOperator(1.0, 0.8, 0.0, WaveformType::SINE, 0.01, 0.05, 0.8, 0.2);
    // Op 2: Modulates op 1
    preset.operators[1] = createOperator(2.0, 0.6, 0.8, WaveformType::SINE, 0.01, 0.04, 0.6, 0.15);
    // Op 3: Modulates op 2
    preset.operators[2] = createOperator(3.0, 0.4, 0.5, WaveformType::SINE, 0.01, 0.03, 0.4, 0.1);
    // Op 4: Parallel carrier
    preset.operators[3] = createOperator(4.0, 0.3, 0.0, WaveformType::SINE, 0.01, 0.02, 0.2, 0.08);
    // Op 5: Modulates op 4
    preset.operators[4] = createOperator(5.0, 0.2, 0.3, WaveformType::SINE, 0.01, 0.01, 0.1, 0.05);
    // Op 6: Modulates op 5 and op 3
    preset.operators[5] = createOperator(6.0, 0.1, 0.2, WaveformType::SINE, 0.01, 0.005, 0.05, 0.03);
    
    return preset;
}

} // namespace toybasic