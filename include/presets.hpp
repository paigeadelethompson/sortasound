#pragma once

#include <vector>
#include <string>
#include "fm.hpp"

namespace toybasic {

// Preset parameter constants
namespace PresetConstants {
    // Algorithm constants
    constexpr int ALGORITHM_SERIAL = 0;
    constexpr int ALGORITHM_PARALLEL_MODULATORS = 1;
    constexpr int ALGORITHM_PARALLEL_OPERATORS = 7;
    constexpr int ALGORITHM_FOUR_PARALLEL = 8;
    constexpr int ALGORITHM_FOUR_PARALLEL_CHAINS = 15;
    
    // Volume constants
    constexpr double VOLUME_QUIET = 0.1;
    constexpr double VOLUME_MEDIUM = 0.5;
    constexpr double VOLUME_LOUD = 0.8;
    constexpr double VOLUME_MAX = 0.9;
    
    // Frequency ratio constants
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
    
    // Amplitude constants
    constexpr double AMP_SILENT = 0.0;
    constexpr double AMP_QUIET = 0.2;
    constexpr double AMP_MEDIUM = 0.4;
    constexpr double AMP_LOUD = 0.6;
    constexpr double AMP_VERY_LOUD = 0.8;
    constexpr double AMP_MAX = 0.9;
    
    // Modulation constants
    constexpr double MOD_NONE = 0.0;
    constexpr double MOD_LIGHT = 1.0;
    constexpr double MOD_MEDIUM = 2.0;
    constexpr double MOD_HEAVY = 3.0;
    constexpr double MOD_EXTREME = 5.0;
    
    // Envelope time constants
    constexpr double ENV_INSTANT = 0.001;
    constexpr double ENV_VERY_FAST = 0.01;
    constexpr double ENV_FAST = 0.05;
    constexpr double ENV_MEDIUM = 0.1;
    constexpr double ENV_SLOW = 0.3;
    constexpr double ENV_VERY_SLOW = 0.5;
    constexpr double ENV_LONG = 1.0;
    constexpr double ENV_VERY_LONG = 2.0;
    
    // Sustain level constants
    constexpr double SUSTAIN_NONE = 0.0;
    constexpr double SUSTAIN_LOW = 0.2;
    constexpr double SUSTAIN_MEDIUM = 0.4;
    constexpr double SUSTAIN_HIGH = 0.6;
    constexpr double SUSTAIN_VERY_HIGH = 0.8;
    constexpr double SUSTAIN_MAX = 0.9;
    
    // Effect constants
    constexpr double EFFECT_NONE = 0.0;
    constexpr double EFFECT_LIGHT = 0.1;
    constexpr double EFFECT_MEDIUM = 0.3;
    constexpr double EFFECT_HEAVY = 0.6;
    constexpr double EFFECT_MAX = 0.8;
}

// Preset structure for 6-operator FM synthesis
struct FMPresetConfig {
    std::string name;
    int algorithm;  // 0-31
    
    // Operator configurations (6 operators)
    struct OperatorConfig {
        double frequency;        // Frequency ratio
        double amplitude;        // 0.0 to 1.0
        double modulationIndex;  // Modulation depth
        WaveformType waveform;   // Waveform type
        
        // ADSR envelope
        double attack;   // Attack time in seconds
        double decay;    // Decay time in seconds
        double sustain;  // Sustain level (0.0 to 1.0)
        double release;  // Release time in seconds
    };
    
    std::array<OperatorConfig, 6> operators;
    
    // Global parameters
    double masterVolume;
    double reverb;
    double chorus;
    double distortion;
};

// Preset manager class
class PresetManager {
public:
    PresetManager();
    
    // Get preset by index
    const FMPresetConfig& getPreset(int index) const;
    
    // Get preset by name
    const FMPresetConfig& getPreset(const std::string& name) const;
    
    // Get total number of presets
    int getPresetCount() const;
    
    // Get all preset names
    std::vector<std::string> getPresetNames() const;
    
    // Apply preset to synthesizer
    void applyPreset(FMSynthesizer& synth, int channel, int presetIndex) const;
    void applyPreset(FMSynthesizer& synth, int channel, const std::string& presetName) const;
    void applyPreset(FMSynthesizer& synth, int channel, const FMPresetConfig& preset) const;

private:
    std::vector<FMPresetConfig> presets_;
    void initializePresets();
    
    // Helper function to create operator configs
    FMPresetConfig::OperatorConfig createOperator(double freq, double amp, double mod, 
                                          WaveformType wave, double att, double dec, 
                                          double sus, double rel);
    
    // Sine-only preset creation methods using algorithm constants
    FMPresetConfig createSinePiano();
    FMPresetConfig createSineBass();
    FMPresetConfig createSineLead();
    FMPresetConfig createSinePad();
    FMPresetConfig createSineBell();
    FMPresetConfig createSinePluck();
    FMPresetConfig createSineBrass();
    FMPresetConfig createSineFlute();
};

// Preset categories for organization
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

// Get presets by category
std::vector<int> getPresetsByCategory(PresetCategory category);

} // namespace toybasic
