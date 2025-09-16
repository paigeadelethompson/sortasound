#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <queue>
#include <QIODevice>
#include <QAudioSink>
#include <QAudioFormat>
#include <QAudioDevice>
#include <QMediaDevices>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace toybasic {

// FMAudioDevice class removed - using QAudioSink directly now

// Audio sample stream interface - provides blocking read/write functionality
class AudioSampleStream {
public:
    virtual ~AudioSampleStream() = default;
    
    // Read a single sample
    virtual bool readSample(int16_t& sample) = 0;
    
    // Read multiple samples
    virtual size_t readSamples(int16_t* buffer, size_t count) = 0;
    
    // Write a single sample
    virtual bool writeSample(int16_t sample) = 0;
    
    // Write multiple samples
    virtual size_t writeSamples(const int16_t* buffer, size_t count) = 0;
    
    // Check if data is available
    virtual bool hasData() const = 0;
    
    // Get number of samples available
    virtual size_t availableSamples() const = 0;
};

// Callback function type for when samples are ready to be played
// Passes a stream-like interface that the user can read from

// Constants namespace for all constexpr values
namespace Constants {
    // Mathematical constants
    constexpr double PI = M_PI;
    constexpr double TWO_PI = 2.0 * M_PI;
    constexpr double HALF_PI = M_PI / 2.0;
    
    // Audio constants
    constexpr int DEFAULT_SAMPLE_RATE = 44100;
    constexpr int MAX_SAMPLE_RATE = 192000;
    constexpr int MIN_SAMPLE_RATE = 8000;
    
    // Bit precision constants
    constexpr int FREQ_PRECISION_BITS = 22;
    constexpr double FREQ_PRECISION_SCALE = 4194304.0; // 2^22
    constexpr double FREQ_PRECISION_INV = 1.0 / FREQ_PRECISION_SCALE;
    
    // Audio output constants
    constexpr int AUDIO_BITS = 14;
    constexpr int AUDIO_MAX_VALUE = 8191;  // 2^13 - 1
    constexpr int AUDIO_MIN_VALUE = -8192; // -2^13
    constexpr double AUDIO_SCALE = 8191.0;
    
    // MIDI constants
    constexpr int MIDI_A4_NOTE = 69;
    constexpr double MIDI_A4_FREQUENCY = 440.0;
    constexpr int MIDI_NOTES_PER_OCTAVE = 12;
    
    // Synthesizer limits
    constexpr int MAX_VOICES = 16;
    constexpr int MAX_OPERATORS = 6;
    constexpr int MAX_CHANNELS = 8;
    constexpr int MAX_ALGORITHMS = 32;
    constexpr int MAX_MIDI_CHANNELS = 16;
    
    // Envelope timing limits
    constexpr double MIN_ENVELOPE_TIME = 0.001;
    constexpr double MAX_ENVELOPE_TIME = 10.0;
    
    // Volume and amplitude limits
    constexpr double MIN_VOLUME = 0.0;
    constexpr double MAX_VOLUME = 1.0;
    constexpr double MIN_AMPLITUDE = 0.0;
    constexpr double MAX_AMPLITUDE = 1.0;
    
    // Effects constants
    constexpr double MAX_EFFECT_AMOUNT = 1.0;
    constexpr double MIN_EFFECT_AMOUNT = 0.0;
    constexpr double DISTORTION_GAIN_MULTIPLIER = 10.0;
    constexpr double CHORUS_FREQUENCY = 0.5;
    constexpr double CHORUS_DEPTH = 0.1;
    constexpr double REVERB_GAIN = 0.3;
    
    // Panning constants
    constexpr double PAN_LEFT = -0.5;
    constexpr double PAN_CENTER = 0.0;
    constexpr double PAN_RIGHT = 0.5;
    constexpr double PAN_SCALE = 0.5;
}

// Enums for various states and types
enum class EnvelopeState : int {
    OFF = 0,
    ATTACK = 1,
    DECAY = 2,
    SUSTAIN = 3,
    RELEASE = 4
};

enum class MIDIController : int {
    MODULATION_WHEEL = 1,
    VOLUME = 7,
    EXPRESSION = 11,
    SUSTAIN_PEDAL = 64,
    PORTAMENTO = 65,
    SOSTENUTO = 66,
    SOFT_PEDAL = 67
};

enum class AlgorithmType : int {
    SERIAL_CHAIN = 0,
    PARALLEL_MODULATORS = 1,
    MIXED_TOPOLOGY = 2,
    PARALLEL_CHAINS = 3
};

// DX7 FM Algorithm configurations (32 algorithms with 6 operators)
// Based on original Yamaha DX7 algorithms from:
// https://gist.githubusercontent.com/bryc/e997954473940ad97a825da4e7a496fa/raw/0e53d78b3d317f1e3d0bd06911ed771ab71d8fad/!Algorithms.md
enum class FMAlgorithm : int {
    // Algorithm 1: Pure serial chain 6→5→4→3→2→1 (single carrier: 1)
    ALG_1_SERIAL_6TO5TO4TO3TO2TO1 = 0,
    
    // Algorithm 2: Parallel modulators (5+6)→4→3→2→1 (single carrier: 1)
    ALG_2_PARALLEL_5AND6TO4TO3TO2TO1 = 1,
    
    // Algorithm 3: Split path 6→5→4→3→2, 6→1 (dual carriers: 1,2)
    ALG_3_SERIAL_6TO5TO4TO3TO2_AND_6TO1 = 2,
    
    // Algorithm 4: Split path 6→5→4→3, 6→2→1 (dual carriers: 1,3)
    ALG_4_SERIAL_6TO5TO4TO3_AND_6TO2TO1 = 3,
    
    // Algorithm 5: Split path 6→5→4, 6→3→2→1 (dual carriers: 1,4)
    ALG_5_SERIAL_6TO5TO4_AND_6TO3TO2TO1 = 4,
    
    // Algorithm 6: Split path 6→5, 6→4→3→2→1 (dual carriers: 1,5)
    ALG_6_SERIAL_6TO5_AND_6TO4TO3TO2TO1 = 5,
    
    // Algorithm 7: Triple split 6→5→4, 6→3, 6→2→1 (triple carriers: 1,3,4)
    ALG_7_SERIAL_6TO5TO4_AND_6TO3_AND_6TO2TO1 = 6,
    
    // Algorithm 8: Quad split 6→5, 6→4, 6→3, 6→2→1 (quad carriers: 1,3,4,5)
    ALG_8_SERIAL_6TO5_AND_6TO4_AND_6TO3_AND_6TO2TO1 = 7,
    
    // Algorithm 9: Mixed routing 6→5→3, 4→1, 2→1 (single carrier: 1)
    ALG_9_MIXED_6TO5TO3_AND_4TO1_AND_2TO1 = 8,
    
    // Algorithm 10: Mixed routing (5+6)→4→1, 3→1 (single carrier: 1)
    ALG_10_PARALLEL_5AND6TO4TO1_AND_3TO1 = 9,
    
    // Algorithm 11: Triple parallel (4+5+6)→3→1, 2→1 (single carrier: 1)
    ALG_11_PARALLEL_4AND5AND6TO3TO1_AND_2TO1 = 10,
    
    // Algorithm 12: Duplicate of Algorithm 11 - (4+5+6)→3→1, 2→1 (single carrier: 1)
    ALG_12_PARALLEL_4AND5AND6TO3TO1_AND_2TO1_DUPLICATE_OF_11 = 11,
    
    // Algorithm 13: Triple parallel (5+6)→2→1, 4→1, 3→1 (single carrier: 1)
    ALG_13_PARALLEL_5AND6TO2TO1_AND_4TO1_AND_3TO1 = 12,
    
    // Algorithm 14: Duplicate of Algorithm 13 - (5+6)→2→1, 4→1, 3→1 (single carrier: 1)
    ALG_14_PARALLEL_5AND6TO2TO1_AND_4TO1_AND_3TO1_DUPLICATE_OF_13 = 13,
    
    // Algorithm 15: Triple parallel (4+6)→3→1, 2→1, 5→1 (single carrier: 1)
    ALG_15_PARALLEL_4AND6TO3TO1_AND_2TO1_AND_5TO1 = 14,
    
    // Algorithm 16: Duplicate of Algorithm 15 - (4+6)→3→1, 2→1, 5→1 (single carrier: 1)
    ALG_16_PARALLEL_4AND6TO3TO1_AND_2TO1_AND_5TO1_DUPLICATE_OF_15 = 15,
    
    // Algorithm 17: Triple parallel (5+6)→4→1, 3→1, 2→1 (single carrier: 1)
    ALG_17_PARALLEL_5AND6TO4TO1_AND_3TO1_AND_2TO1 = 16,
    
    // Algorithm 18: Mixed routing (2+6)→4→5, 1 (single carrier: 5)
    ALG_18_MIXED_2AND6TO4TO5_AND_1_CARRIER = 17,
    
    // Algorithm 19: Triple parallel (3+5+6)→2→4, 1 (single carrier: 4)
    ALG_19_PARALLEL_3AND5AND6TO2TO4_AND_1_CARRIER = 18,
    
    // Algorithm 20: Mixed routing (3+6)→2→4, 5→4, 1 (single carrier: 4)
    ALG_20_MIXED_3AND6TO2TO4_AND_5TO4_AND_1_CARRIER = 19,
    
    // Algorithm 21: Mixed routing (2+6)→3→4, 5→4, 1 (single carrier: 4)
    ALG_21_MIXED_2AND6TO3TO4_AND_5TO4_AND_1_CARRIER = 20,
    
    // Algorithm 22: Duplicate of Algorithm 21 - (2+6)→3→4, 5→4, 1 (single carrier: 4)
    ALG_22_MIXED_2AND6TO3TO4_AND_5TO4_AND_1_CARRIER_DUPLICATE_OF_21 = 21,
    
    // Algorithm 23: All parallel (1+2+3+4+5)→6 (single carrier: 6)
    ALG_23_PARALLEL_1AND2AND3AND4AND5TO6 = 22,
    
    // Algorithm 24: Duplicate of Algorithm 23 - (1+2+3+4+5)→6 (single carrier: 6)
    ALG_24_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23 = 23,
    
    // Algorithm 25: Mixed routing (3+5+6)→4, 1→2 (dual carriers: 2,4)
    ALG_25_PARALLEL_3AND5AND6TO4_AND_1TO2 = 24,
    
    // Algorithm 26: Duplicate of Algorithm 25 - (3+5+6)→4, 1→2 (dual carriers: 2,4)
    ALG_26_PARALLEL_3AND5AND6TO4_AND_1TO2_DUPLICATE_OF_25 = 25,
    
    // Algorithm 27: Mixed routing (1+3+6)→2, 4, 5 (triple carriers: 2,4,5)
    ALG_27_PARALLEL_1AND3AND6TO2_AND_4_CARRIER_AND_5_CARRIER = 26,
    
    // Algorithm 28: Mixed routing (1+2+3+5)→4, 6 (dual carriers: 4,6)
    ALG_28_PARALLEL_1AND2AND3AND5TO4_AND_6_CARRIER = 27,
    
    // Algorithm 29: Mixed routing (1+2+3+6)→4, 5 (dual carriers: 4,5)
    ALG_29_PARALLEL_1AND2AND3AND6TO4_AND_5_CARRIER = 28,
    
    // Algorithm 30: Duplicate of Algorithm 23 - (1+2+3+4+5)→6 (single carrier: 6)
    ALG_30_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23 = 29,
    
    // Algorithm 31: Duplicate of Algorithm 23 - (1+2+3+4+5)→6 (single carrier: 6)
    ALG_31_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23_2 = 30,
    
    // Algorithm 32: All parallel carriers 1, 2, 3, 4, 5, 6 (six carriers: 1,2,3,4,5,6)
    ALG_32_ALL_6_PARALLEL_CARRIERS = 31
};

// Waveform types for operators
enum class WaveformType {
    SINE = 0,
    SAWTOOTH = 1,
    SQUARE = 2,
    TRIANGLE = 3
};


// FMSampleStream implementation for the synthesizer
class FMSampleStream : public AudioSampleStream {
public:
    FMSampleStream() = default;
    
    bool readSample(int16_t& sample) override {
        std::unique_lock<std::mutex> lock(mutex_);
        while (samples_.empty()) {
            condition_.wait(lock);
        }
        sample = samples_.front();
        samples_.pop();
        return true;
    }
    
    size_t readSamples(int16_t* buffer, size_t count) override {
        std::unique_lock<std::mutex> lock(mutex_);
        size_t samplesRead = 0;
        while (samplesRead < count && !samples_.empty()) {
            buffer[samplesRead] = samples_.front();
            samples_.pop();
            samplesRead++;
        }
        return samplesRead;
    }
    
    bool writeSample(int16_t sample) override {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push(sample);
        condition_.notify_all();
        return true;
    }
    
    size_t writeSamples(const int16_t* buffer, size_t count) override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (size_t i = 0; i < count; i++) {
            samples_.push(buffer[i]);
        }
        condition_.notify_all();
        return count;
    }
    
    bool hasData() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return !samples_.empty();
    }
    
    size_t availableSamples() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return samples_.size();
    }
    
private:
    std::queue<int16_t> samples_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};

// 14-bit FM Synthesizer with 16 voices, 8 channels, 6 operators per voice
class FMSynthesizer {
public:
    // Constructor
    FMSynthesizer(int sampleRate = 44100);
    
    // Destructor
    ~FMSynthesizer();
    
    // Voice management (16 voices)
    void noteOn(int note, double velocity = 1.0); // XXX steals voice 0 if no voices are free?
    void noteOff(int note);
    void allNotesOff();
    
    // Channel management (8 channels)
    void setChannelActive(int channel, bool active);
    bool isChannelActive(int channel) const;
    
    // Operator configuration (6 operators per voice)
    void setOperatorFrequency(int voice, int opIndex, double frequency);
    void setOperatorAmplitude(int voice, int opIndex, double amplitude);
    void setOperatorModulationIndex(int voice, int opIndex, double index);
    void setOperatorWaveform(int voice, int opIndex, WaveformType waveform);
    
    // Algorithm configuration (determines how operators modulate each other)
    void setAlgorithm(int channel, int algorithm);
    
    // Envelope configuration
    void setEnvelope(int voice, int opIndex, double attack, double decay, 
                    double sustain, double release);
    
    // Global parameters
    void setMasterVolume(double volume);
    void setSampleRate(int sampleRate);
    
    // Threading and callback system
    void startAudioThread();
    void stopAudioThread();
    void setSampleStream(AudioSampleStream* stream);
    // setAudioDevice method removed - we now use QAudioSink directly
    bool isAudioThreadRunning() const;
    
    // Audio generation
    void generateSamples(AudioSampleStream& stream);
    
    // Get individual operator outputs for debugging (from most recent voice)
    std::array<double, 6> getOperatorOutputs() const;
    
    // Preset configurations
    
    // Real-time control
    void setPitchBend(int channel, double bend);
    void setModulationWheel(int channel, double mod);
    
    // Effects
    void setReverb(double amount);
    void setChorus(double amount);
    void setDistortion(double amount);
    
    // Set preset configuration for new voices
    void setPresetConfig(const std::array<double, 6>& frequencies,
                        const std::array<double, 6>& amplitudes,
                        const std::array<double, 6>& modulationIndices,
                        const std::array<WaveformType, 6>& waveforms,
                        const std::array<double, 6>& attacks,
                        const std::array<double, 6>& decays,
                        const std::array<double, 6>& sustains,
                        const std::array<double, 6>& releases);
    
    // Note to frequency conversion
    double noteToFrequency(int note);
    double noteToFrequency22Bit(int note); // 22-bit precision version
    
    // 22-bit precision frequency calculations
    double calculatePhaseIncrement22Bit(double frequency);
    
    // Property getters and setters
    int getFreqPrecisionBits() const { return freqPrecisionBits_; }
    void setFreqPrecisionBits(int bits);
    double getFreqPrecisionScale() const { return freqPrecisionScale_; }
    void setFreqPrecisionScale(double scale);
    double getFreqPrecisionInv() const { return freqPrecisionInv_; }
    void setFreqPrecisionInv(double inv);
    int getAudioBits() const { return audioBits_; }
    void setAudioBits(int bits);
    int getAudioMaxValue() const { return audioMaxValue_; }
    void setAudioMaxValue(int value);
    int getAudioMinValue() const { return audioMinValue_; }
    void setAudioMinValue(int value);
    double getAudioScale() const { return audioScale_; }
    void setAudioScale(double scale);
    int getMidiA4Note() const { return midiA4Note_; }
    void setMidiA4Note(int note);
    double getMidiA4Frequency() const { return midiA4Frequency_; }
    void setMidiA4Frequency(double frequency);
    int getMidiNotesPerOctave() const { return midiNotesPerOctave_; }
    void setMidiNotesPerOctave(int notes);
    int getMaxVoices() const { return maxVoices_; }
    void setMaxVoices(int voices);
    int getMaxOperators() const { return maxOperators_; }
    void setMaxOperators(int operators);
    int getMaxChannels() const { return maxChannels_; }
    void setMaxChannels(int channels);
    int getMaxAlgorithms() const { return maxAlgorithms_; }
    void setMaxAlgorithms(int algorithms);
    int getMaxMidiChannels() const { return maxMidiChannels_; }
    void setMaxMidiChannels(int channels);
    double getMinEnvelopeTime() const { return minEnvelopeTime_; }
    void setMinEnvelopeTime(double time);
    double getMaxEnvelopeTime() const { return maxEnvelopeTime_; }
    void setMaxEnvelopeTime(double time);
    double getMinVolume() const { return minVolume_; }
    void setMinVolume(double volume);
    double getMaxVolume() const { return maxVolume_; }
    void setMaxVolume(double volume);
    double getMinAmplitude() const { return minAmplitude_; }
    void setMinAmplitude(double amplitude);
    double getMaxAmplitude() const { return maxAmplitude_; }
    void setMaxAmplitude(double amplitude);
    double getMaxEffectAmount() const { return maxEffectAmount_; }
    void setMaxEffectAmount(double amount);
    double getMinEffectAmount() const { return minEffectAmount_; }
    void setMinEffectAmount(double amount);
    double getDistortionGainMultiplier() const { return distortionGainMultiplier_; }
    void setDistortionGainMultiplier(double multiplier);
    double getChorusFrequency() const { return chorusFrequency_; }
    void setChorusFrequency(double frequency);
    double getChorusDepth() const { return chorusDepth_; }
    void setChorusDepth(double depth);
    double getReverbGain() const { return reverbGain_; }
    void setReverbGain(double gain);
    double getPanLeft() const { return panLeft_; }
    void setPanLeft(double pan);
    double getPanCenter() const { return panCenter_; }
    void setPanCenter(double pan);
    double getPanRight() const { return panRight_; }
    void setPanRight(double pan);
    double getPanScale() const { return panScale_; }
    void setPanScale(double scale);

private:
    // Operator structure
    struct Operator {
        double frequency = 440.0;
        double amplitude = 0.5;
        double modulationIndex = 1.0;
        WaveformType waveform = WaveformType::SINE;
        
        // Envelope parameters
        double attack = 0.01;
        double decay = 0.1;
        double sustain = 0.7;
        double release = 0.3;
        
        // Envelope state
        int envelopeState = static_cast<int>(EnvelopeState::OFF);
        
        double envelopeLevel = 0.0;
        double envelopeTime = 0.0;
        
        // Phase accumulator (22-bit precision)
        double phaseAccumulator = 0.0;
        double phaseIncrement = 0.0;
        
        // 22-bit precision frequency calculation
        // Using fixed-point arithmetic with 22 bits fractional precision
        // This provides 4,194,304 steps of precision (2^22)
        // Equivalent to ~0.00024 Hz precision at 440 Hz
        static constexpr double FREQ_PRECISION_SCALE = 4194304.0; // 2^22
        static constexpr double FREQ_PRECISION_INV = 1.0 / FREQ_PRECISION_SCALE;
        
        // Modulation
        double pitchBend = 1.0;
        double modulationWheel = 0.0;
        double velocity = 1.0;
    };
    
    // Voice structure (16 voices)
    struct Voice {
        std::array<Operator, Constants::MAX_OPERATORS> operators;
        bool active = false;
        int note = -1;
        double velocity = 1.0;
        int channel = 0;
    };
    
    // Channel structure (8 channels)
    struct Channel {
        bool active = false;
        int algorithm = 0;  // 0-31, determines modulation routing
        double masterVolume = 1.0;
        double pitchBend = 1.0;
        double modulationWheel = 0.0;
    };
    
    // Member variables
    std::array<Voice, Constants::MAX_VOICES> voices_;     // 16 voices
    std::array<Channel, Constants::MAX_CHANNELS> channels_;  // 8 channels
    int sampleRate_;
    double masterVolume_;
    double timeStep_;
    
    // Configurable synthesizer properties
    int freqPrecisionBits_;
    double freqPrecisionScale_;
    double freqPrecisionInv_;
    int audioBits_;
    int audioMaxValue_;
    int audioMinValue_;
    double audioScale_;
    int midiA4Note_;
    double midiA4Frequency_;
    int midiNotesPerOctave_;
    int maxVoices_;
    int maxOperators_;
    int maxChannels_;
    int maxAlgorithms_;
    int maxMidiChannels_;
    double minEnvelopeTime_;
    double maxEnvelopeTime_;
    double minVolume_;
    double maxVolume_;
    double minAmplitude_;
    double maxAmplitude_;
    double maxEffectAmount_;
    double minEffectAmount_;
    double distortionGainMultiplier_;
    double chorusFrequency_;
    double chorusDepth_;
    double reverbGain_;
    double panLeft_;
    double panCenter_;
    double panRight_;
    double panScale_;
    
    // Preset configuration storage
    struct PresetConfig {
        std::array<double, Constants::MAX_OPERATORS> frequencies;
        std::array<double, Constants::MAX_OPERATORS> amplitudes;
        std::array<double, Constants::MAX_OPERATORS> modulationIndices;
        std::array<WaveformType, Constants::MAX_OPERATORS> waveforms;
        std::array<double, Constants::MAX_OPERATORS> attacks;
        std::array<double, Constants::MAX_OPERATORS> decays;
        std::array<double, Constants::MAX_OPERATORS> sustains;
        std::array<double, Constants::MAX_OPERATORS> releases;
    };
    PresetConfig currentPreset_;
    
    // Effects
    double reverbAmount_;
    double chorusAmount_;
    double distortionAmount_;
    
    // Threading system
    std::thread audioThread_;
    std::atomic<bool> audioThreadRunning_;
    std::atomic<bool> shouldStop_;
    
    // Internal sample buffer
    std::vector<int16_t> sampleBuffer_;
    std::mutex bufferMutex_;
    std::atomic<size_t> bufferWritePos_;
    std::atomic<size_t> bufferReadPos_;
    static constexpr size_t BUFFER_SIZE = 4096; // Buffer size in samples
    
    // Helper functions
    void updateOperatorPhase(Operator& op);
    void updateEnvelope(Operator& op);
    double generateOperatorOutput(Operator& op, double modulation = 0.0);
    double applyEffects(double sample, int channel);
    
    // Algorithm implementations (32 algorithms with 6 operators)
    double processAlgorithm0(Voice& voice);   // Algorithm 1
    double processAlgorithm1(Voice& voice);   // Algorithm 2
    double processAlgorithm2(Voice& voice);   // Algorithm 3
    double processAlgorithm3(Voice& voice);   // Algorithm 4
    double processAlgorithm4(Voice& voice);   // Algorithm 5
    double processAlgorithm5(Voice& voice);   // Algorithm 6
    double processAlgorithm6(Voice& voice);   // Algorithm 7
    double processAlgorithm7(Voice& voice);   // Algorithm 8
    double processAlgorithm8(Voice& voice);   // Algorithm 9
    double processAlgorithm9(Voice& voice);   // Algorithm 10
    double processAlgorithm10(Voice& voice);  // Algorithm 11
    double processAlgorithm11(Voice& voice);  // Algorithm 12
    double processAlgorithm12(Voice& voice);  // Algorithm 13
    double processAlgorithm13(Voice& voice);  // Algorithm 14
    double processAlgorithm14(Voice& voice);  // Algorithm 15
    double processAlgorithm15(Voice& voice);  // Algorithm 16
    double processAlgorithm16(Voice& voice);  // Algorithm 17
    double processAlgorithm17(Voice& voice);  // Algorithm 18
    double processAlgorithm18(Voice& voice);  // Algorithm 19
    double processAlgorithm19(Voice& voice);  // Algorithm 20
    double processAlgorithm20(Voice& voice);  // Algorithm 21
    double processAlgorithm21(Voice& voice);  // Algorithm 22
    double processAlgorithm22(Voice& voice);  // Algorithm 23
    double processAlgorithm23(Voice& voice);  // Algorithm 24
    double processAlgorithm24(Voice& voice);  // Algorithm 25
    double processAlgorithm25(Voice& voice);  // Algorithm 26
    double processAlgorithm26(Voice& voice);  // Algorithm 27
    double processAlgorithm27(Voice& voice);  // Algorithm 28
    double processAlgorithm28(Voice& voice);  // Algorithm 29
    double processAlgorithm29(Voice& voice);  // Algorithm 30
    double processAlgorithm30(Voice& voice);  // Algorithm 31
    double processAlgorithm31(Voice& voice);  // Algorithm 32
    
    // Initialize presets
    void initializePresets();
    
    // Voice allocation
    int findFreeVoice();
    void releaseVoice(int voice);
    
    // 22-bit precision helper functions
    void updateOperatorPhase22Bit(Operator& op);
    
    // Audio thread function
    void audioThreadFunction();
    
    // Generate a single sample (internal use)
    void generateSample(int16_t& left, int16_t& right);
    
    // Sample stream
    std::unique_ptr<FMSampleStream> sampleStream_;
    
    // External stream (when using shared stream with AudioEngine)
    AudioSampleStream* externalStream_ = nullptr;
    
    // Qt audio components
    QIODevice* audioIODevice_;
    QAudioSink* audioSink_;
    
    // Audio setup
    void setupAudio();
};

// FM Synthesizer Manager - handles multiple synthesizers and routing
class FMSynthesizerManager {
public:
    FMSynthesizerManager(int sampleRate = 44100);
    ~FMSynthesizerManager();
    
    // Synthesizer management
    void addSynthesizer(std::shared_ptr<FMSynthesizer> synth);
    void removeSynthesizer(std::shared_ptr<FMSynthesizer> synth);
    
    // Audio generation - removed, use individual synthesizer callbacks instead
    
    // Global controls
    void setMasterVolume(double volume);
    void setSampleRate(int sampleRate);
    
    // MIDI-like interface
    void noteOn(int channel, int note, double velocity = 1.0);
    void noteOff(int channel, int note);
    void controlChange(int channel, int controller, double value);
    
    // Effects
    void setGlobalReverb(double amount);
    void setGlobalChorus(double amount);
    void setGlobalDistortion(double amount);

private:
    std::vector<std::shared_ptr<FMSynthesizer>> synthesizers_;
    int sampleRate_;
    double masterVolume_;
    
    // Global effects
    double globalReverb_;
    double globalChorus_;
    double globalDistortion_;
    
    // MIDI state
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelVolumes_;
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelPitchBends_;
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelModulations_;
};

} // namespace toybasic