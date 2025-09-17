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
class AudioSampleStream {
public:
    virtual ~AudioSampleStream() = default;
    
    virtual bool readSample(int16_t& sample) = 0;
    
    virtual size_t readSamples(int16_t* buffer, size_t count) = 0;
    
    virtual bool writeSample(int16_t sample) = 0;
    
    virtual size_t writeSamples(const int16_t* buffer, size_t count) = 0;
    
    virtual bool hasData() const = 0;
    
    virtual size_t availableSamples() const = 0;
};

namespace Constants {
    constexpr double PI = M_PI;
    constexpr double TWO_PI = 2.0 * M_PI;
    constexpr double HALF_PI = M_PI / 2.0;
    
    constexpr int DEFAULT_SAMPLE_RATE = 44100;
    constexpr int MAX_SAMPLE_RATE = 192000;
    constexpr int MIN_SAMPLE_RATE = 8000;
    
    constexpr int FREQ_PRECISION_BITS = 22;
    constexpr double FREQ_PRECISION_SCALE = 4194304.0;
    constexpr double FREQ_PRECISION_INV = 1.0 / FREQ_PRECISION_SCALE;
    
    constexpr int AUDIO_BITS = 14;
    constexpr int AUDIO_MAX_VALUE = 8191;
    constexpr int AUDIO_MIN_VALUE = -8192;
    constexpr double AUDIO_SCALE = 8191.0;
    
    constexpr int MIDI_A4_NOTE = 69;
    constexpr double MIDI_A4_FREQUENCY = 440.0;
    constexpr int MIDI_NOTES_PER_OCTAVE = 12;
    
    constexpr int MAX_VOICES = 16;
    constexpr int MAX_OPERATORS = 6;
    constexpr int MAX_CHANNELS = 8;
    constexpr int MAX_ALGORITHMS = 32;
    constexpr int MAX_MIDI_CHANNELS = 16;
    
    constexpr double MIN_ENVELOPE_TIME = 0.001;
    constexpr double MAX_ENVELOPE_TIME = 10.0;
    
    constexpr double MIN_VOLUME = 0.0;
    constexpr double MAX_VOLUME = 1.0;
    constexpr double MIN_AMPLITUDE = 0.0;
    constexpr double MAX_AMPLITUDE = 1.0;
    
    constexpr double MAX_EFFECT_AMOUNT = 1.0;
    constexpr double MIN_EFFECT_AMOUNT = 0.0;
    constexpr double DISTORTION_GAIN_MULTIPLIER = 10.0;
    constexpr double CHORUS_FREQUENCY = 0.5;
    constexpr double CHORUS_DEPTH = 0.1;
    constexpr double REVERB_GAIN = 0.3;
    
    constexpr double PAN_LEFT = -0.5;
    constexpr double PAN_CENTER = 0.0;
    constexpr double PAN_RIGHT = 0.5;
    constexpr double PAN_SCALE = 0.5;
}

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

enum class FMAlgorithm : int {
    ALG_1_SERIAL_6TO5TO4TO3TO2TO1 = 0,
    
    ALG_2_PARALLEL_5AND6TO4TO3TO2TO1 = 1,
    
    ALG_3_SERIAL_6TO5TO4TO3TO2_AND_6TO1 = 2,
    
    ALG_4_SERIAL_6TO5TO4TO3_AND_6TO2TO1 = 3,
    
    ALG_5_SERIAL_6TO5TO4_AND_6TO3TO2TO1 = 4,
    
    ALG_6_SERIAL_6TO5_AND_6TO4TO3TO2TO1 = 5,
    
    ALG_7_SERIAL_6TO5TO4_AND_6TO3_AND_6TO2TO1 = 6,
    
    ALG_8_SERIAL_6TO5_AND_6TO4_AND_6TO3_AND_6TO2TO1 = 7,
    
    ALG_9_MIXED_6TO5TO3_AND_4TO1_AND_2TO1 = 8,
    
    ALG_10_PARALLEL_5AND6TO4TO1_AND_3TO1 = 9,
    
    ALG_11_PARALLEL_4AND5AND6TO3TO1_AND_2TO1 = 10,
    
    ALG_12_PARALLEL_4AND5AND6TO3TO1_AND_2TO1_DUPLICATE_OF_11 = 11,
    
    ALG_13_PARALLEL_5AND6TO2TO1_AND_4TO1_AND_3TO1 = 12,
    
    ALG_14_PARALLEL_5AND6TO2TO1_AND_4TO1_AND_3TO1_DUPLICATE_OF_13 = 13,
    
    ALG_15_PARALLEL_4AND6TO3TO1_AND_2TO1_AND_5TO1 = 14,
    
    ALG_16_PARALLEL_4AND6TO3TO1_AND_2TO1_AND_5TO1_DUPLICATE_OF_15 = 15,
    
    ALG_17_PARALLEL_5AND6TO4TO1_AND_3TO1_AND_2TO1 = 16,
    
    ALG_18_MIXED_2AND6TO4TO5_AND_1_CARRIER = 17,
    
    ALG_19_PARALLEL_3AND5AND6TO2TO4_AND_1_CARRIER = 18,
    
    ALG_20_MIXED_3AND6TO2TO4_AND_5TO4_AND_1_CARRIER = 19,
    
    ALG_21_MIXED_2AND6TO3TO4_AND_5TO4_AND_1_CARRIER = 20,
    
    ALG_22_MIXED_2AND6TO3TO4_AND_5TO4_AND_1_CARRIER_DUPLICATE_OF_21 = 21,
    
    ALG_23_PARALLEL_1AND2AND3AND4AND5TO6 = 22,
    
    ALG_24_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23 = 23,
    
    ALG_25_PARALLEL_3AND5AND6TO4_AND_1TO2 = 24,
    
    ALG_26_PARALLEL_3AND5AND6TO4_AND_1TO2_DUPLICATE_OF_25 = 25,
    
    ALG_27_PARALLEL_1AND3AND6TO2_AND_4_CARRIER_AND_5_CARRIER = 26,
    
    ALG_28_PARALLEL_1AND2AND3AND5TO4_AND_6_CARRIER = 27,
    
    ALG_29_PARALLEL_1AND2AND3AND6TO4_AND_5_CARRIER = 28,
    
    ALG_30_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23 = 29,
    
    ALG_31_PARALLEL_1AND2AND3AND4AND5TO6_DUPLICATE_OF_23_2 = 30,
    
    ALG_32_ALL_6_PARALLEL_CARRIERS = 31
};

enum class WaveformType {
    SINE = 0,
    SAWTOOTH = 1,
    SQUARE = 2,
    TRIANGLE = 3
};


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

class FMSynthesizer {
public:
    FMSynthesizer(int sampleRate = 44100);
    
    ~FMSynthesizer();
    
    void noteOn(int note, double velocity = 1.0); // XXX steals voice 0 if no voices are free?
    void noteOff(int note);
    void allNotesOff();
    
    void setChannelActive(int channel, bool active);
    bool isChannelActive(int channel) const;
    
    void setOperatorFrequency(int voice, int opIndex, double frequency);
    void setOperatorAmplitude(int voice, int opIndex, double amplitude);
    void setOperatorModulationIndex(int voice, int opIndex, double index);
    void setOperatorWaveform(int voice, int opIndex, WaveformType waveform);
    
    void setAlgorithm(int channel, int algorithm);
    
    void setEnvelope(int voice, int opIndex, double attack, double decay, 
                    double sustain, double release);
    
    void setMasterVolume(double volume);
    void setSampleRate(int sampleRate);
    
    void startAudioThread();
    void stopAudioThread();
    void setSampleStream(AudioSampleStream* stream);
    bool isAudioThreadRunning() const;
    
    void generateSamples(AudioSampleStream& stream);
    
    std::array<double, 6> getOperatorOutputs() const;
    
    
    void setPitchBend(int channel, double bend);
    void setModulationWheel(int channel, double mod);
    
    void setReverb(double amount);
    void setChorus(double amount);
    void setDistortion(double amount);
    
    void setPresetConfig(const std::array<double, 6>& frequencies,
                        const std::array<double, 6>& amplitudes,
                        const std::array<double, 6>& modulationIndices,
                        const std::array<WaveformType, 6>& waveforms,
                        const std::array<double, 6>& attacks,
                        const std::array<double, 6>& decays,
                        const std::array<double, 6>& sustains,
                        const std::array<double, 6>& releases);
    
    double noteToFrequency(int note);
    double noteToFrequency22Bit(int note);
    
    double calculatePhaseIncrement22Bit(double frequency);
    
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
    struct Operator {
        double frequency = 440.0;
        double amplitude = 0.5;
        double modulationIndex = 1.0;
        WaveformType waveform = WaveformType::SINE;
        
        double attack = 0.01;
        double decay = 0.1;
        double sustain = 0.7;
        double release = 0.3;
        
        int envelopeState = static_cast<int>(EnvelopeState::OFF);
        
        double envelopeLevel = 0.0;
        double envelopeTime = 0.0;
        
        double phaseAccumulator = 0.0;
        double phaseIncrement = 0.0;
        
        static constexpr double FREQ_PRECISION_SCALE = 4194304.0;
        static constexpr double FREQ_PRECISION_INV = 1.0 / FREQ_PRECISION_SCALE;
        
        double pitchBend = 1.0;
        double modulationWheel = 0.0;
        double velocity = 1.0;
    };
    
    struct Voice {
        std::array<Operator, Constants::MAX_OPERATORS> operators;
        bool active = false;
        int note = -1;
        double velocity = 1.0;
        int channel = 0;
    };
    
    struct Channel {
        bool active = false;
        int algorithm = 0;
        double masterVolume = 1.0;
        double pitchBend = 1.0;
        double modulationWheel = 0.0;
    };
    
    std::array<Voice, Constants::MAX_VOICES> voices_;
    std::array<Channel, Constants::MAX_CHANNELS> channels_;
    int sampleRate_;
    double masterVolume_;
    double timeStep_;
    
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
    
    double reverbAmount_;
    double chorusAmount_;
    double distortionAmount_;
    
    std::thread audioThread_;
    std::atomic<bool> audioThreadRunning_;
    std::atomic<bool> shouldStop_;
    
    std::vector<int16_t> sampleBuffer_;
    std::mutex bufferMutex_;
    std::atomic<size_t> bufferWritePos_;
    std::atomic<size_t> bufferReadPos_;
    static constexpr size_t BUFFER_SIZE = 4096;
    
    void updateOperatorPhase(Operator& op);
    void updateEnvelope(Operator& op);
    double generateOperatorOutput(Operator& op, double modulation = 0.0);
    double applyEffects(double sample, int channel);
    
    double processAlgorithm0(Voice& voice);
    double processAlgorithm1(Voice& voice);
    double processAlgorithm2(Voice& voice);
    double processAlgorithm3(Voice& voice);
    double processAlgorithm4(Voice& voice);
    double processAlgorithm5(Voice& voice);
    double processAlgorithm6(Voice& voice);
    double processAlgorithm7(Voice& voice);
    double processAlgorithm8(Voice& voice);
    double processAlgorithm9(Voice& voice);
    double processAlgorithm10(Voice& voice);
    double processAlgorithm11(Voice& voice);
    double processAlgorithm12(Voice& voice);
    double processAlgorithm13(Voice& voice);
    double processAlgorithm14(Voice& voice);
    double processAlgorithm15(Voice& voice);
    double processAlgorithm16(Voice& voice);
    double processAlgorithm17(Voice& voice);
    double processAlgorithm18(Voice& voice);
    double processAlgorithm19(Voice& voice);
    double processAlgorithm20(Voice& voice);
    double processAlgorithm21(Voice& voice);
    double processAlgorithm22(Voice& voice);
    double processAlgorithm23(Voice& voice);
    double processAlgorithm24(Voice& voice);
    double processAlgorithm25(Voice& voice);
    double processAlgorithm26(Voice& voice);
    double processAlgorithm27(Voice& voice);
    double processAlgorithm28(Voice& voice);
    double processAlgorithm29(Voice& voice);
    double processAlgorithm30(Voice& voice);
    double processAlgorithm31(Voice& voice);
    
    void initializePresets();
    
    int findFreeVoice();
    void releaseVoice(int voice);
    
    void updateOperatorPhase22Bit(Operator& op);
    
    void audioThreadFunction();
    
    void generateSample(int16_t& left, int16_t& right);
    
    std::unique_ptr<FMSampleStream> sampleStream_;
    
    AudioSampleStream* externalStream_ = nullptr;
    
    QIODevice* audioIODevice_;
    QAudioSink* audioSink_;
    
    void setupAudio();
};

class FMSynthesizerManager {
public:
    FMSynthesizerManager(int sampleRate = 44100);
    ~FMSynthesizerManager();
    
    void addSynthesizer(std::shared_ptr<FMSynthesizer> synth);
    void removeSynthesizer(std::shared_ptr<FMSynthesizer> synth);
    
    
    void setMasterVolume(double volume);
    void setSampleRate(int sampleRate);
    
    void noteOn(int channel, int note, double velocity = 1.0);
    void noteOff(int channel, int note);
    void controlChange(int channel, int controller, double value);
    
    void setGlobalReverb(double amount);
    void setGlobalChorus(double amount);
    void setGlobalDistortion(double amount);

private:
    std::vector<std::shared_ptr<FMSynthesizer>> synthesizers_;
    int sampleRate_;
    double masterVolume_;
    
    double globalReverb_;
    double globalChorus_;
    double globalDistortion_;
    
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelVolumes_;
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelPitchBends_;
    std::array<double, Constants::MAX_MIDI_CHANNELS> channelModulations_;
};

}