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

#include "fm/fm.hpp"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <chrono>
#include <thread>
#include <condition_variable>

namespace toybasic {

/**
 * @brief Constructor for FMSynthesizer
 * 
 * Creates a new FM synthesizer with the specified sample rate. Initializes
 * all operators, voices, channels, and audio parameters to their default values.
 * Sets up the audio thread and sample buffer.
 * 
 * @param sampleRate The audio sample rate in Hz (default: 44100)
 */
FMSynthesizer::FMSynthesizer(int sampleRate) 
    : sampleRate_(sampleRate), masterVolume_(Constants::MAX_VOLUME), timeStep_(1.0 / sampleRate),
      reverbAmount_(Constants::MIN_EFFECT_AMOUNT), chorusAmount_(Constants::MIN_EFFECT_AMOUNT), 
      distortionAmount_(Constants::MIN_EFFECT_AMOUNT),
      audioThreadRunning_(false), shouldStop_(false),
      sampleBuffer_(BUFFER_SIZE * 2), bufferWritePos_(0), bufferReadPos_(0),
      sampleStream_(std::make_unique<FMSampleStream>()),
      audioIODevice_(nullptr), audioSink_(nullptr),
      freqPrecisionBits_(Constants::FREQ_PRECISION_BITS),
      freqPrecisionScale_(Constants::FREQ_PRECISION_SCALE),
      freqPrecisionInv_(Constants::FREQ_PRECISION_INV),
      audioBits_(Constants::AUDIO_BITS),
      audioMaxValue_(Constants::AUDIO_MAX_VALUE),
      audioMinValue_(Constants::AUDIO_MIN_VALUE),
      audioScale_(Constants::AUDIO_SCALE),
      midiA4Note_(Constants::MIDI_A4_NOTE),
      midiA4Frequency_(Constants::MIDI_A4_FREQUENCY),
      midiNotesPerOctave_(Constants::MIDI_NOTES_PER_OCTAVE),
      maxVoices_(Constants::MAX_VOICES),
      maxOperators_(Constants::MAX_OPERATORS),
      maxChannels_(Constants::MAX_CHANNELS),
      maxAlgorithms_(Constants::MAX_ALGORITHMS),
      maxMidiChannels_(Constants::MAX_MIDI_CHANNELS),
      minEnvelopeTime_(Constants::MIN_ENVELOPE_TIME),
      maxEnvelopeTime_(Constants::MAX_ENVELOPE_TIME),
      minVolume_(Constants::MIN_VOLUME),
      maxVolume_(Constants::MAX_VOLUME),
      minAmplitude_(Constants::MIN_AMPLITUDE),
      maxAmplitude_(Constants::MAX_AMPLITUDE),
      maxEffectAmount_(Constants::MAX_EFFECT_AMOUNT),
      minEffectAmount_(Constants::MIN_EFFECT_AMOUNT),
      distortionGainMultiplier_(Constants::DISTORTION_GAIN_MULTIPLIER),
      chorusFrequency_(Constants::CHORUS_FREQUENCY),
      chorusDepth_(Constants::CHORUS_DEPTH),
      reverbGain_(Constants::REVERB_GAIN),
      panLeft_(Constants::PAN_LEFT),
      panCenter_(Constants::PAN_CENTER),
      panRight_(Constants::PAN_RIGHT),
      panScale_(Constants::PAN_SCALE) {
    
    for (int i = 0; i < 6; i++) {
        currentPreset_.frequencies[i] = 1.0;
        currentPreset_.amplitudes[i] = 0.5;
        currentPreset_.modulationIndices[i] = 0.0;
        currentPreset_.waveforms[i] = WaveformType::SINE;
        currentPreset_.attacks[i] = 0.01;
        currentPreset_.decays[i] = 0.1;
        currentPreset_.sustains[i] = 0.7;
        currentPreset_.releases[i] = 0.3;
    }
    
    setupAudio();
}

/**
 * @brief Destructor for FMSynthesizer
 * 
 * Cleans up the synthesizer by stopping the audio thread and freeing resources.
 */
FMSynthesizer::~FMSynthesizer() {
    stopAudioThread();
}

void FMSynthesizer::setupAudio() {
    QAudioFormat format;
    format.setSampleRate(sampleRate_);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (device.isNull()) {
        printf("No audio output device found\n");
        return;
    }

    if (!device.isFormatSupported(format)) {
        printf("Audio format not supported, using nearest supported format\n");
        format = device.preferredFormat();
    }

    audioSink_ = new QAudioSink(device, format);
    
    audioIODevice_ = audioSink_->start();

    if (audioSink_->state() != QAudio::ActiveState) {
        printf("Failed to start audio output\n");
        return;
    }

    printf("Audio setup completed successfully\n");
}

/**
 * @brief Trigger a note on event
 * 
 * Starts playing a note on the synthesizer. Finds an available voice and
 * configures it with the note parameters. If no voices are available,
 * steals voice 0.
 * 
 * @param note The MIDI note number to play
 * @param velocity The note velocity (0.0 to 1.0)
 */
void FMSynthesizer::noteOn(int note, double velocity) {
    int voice = findFreeVoice();
    if (voice == -1) {
        voice = 0;
        releaseVoice(0);
    }
    
    voices_[voice].active = true;
    voices_[voice].note = note;
    voices_[voice].velocity = velocity;
    voices_[voice].channel = 0;
    
    double baseFreq = noteToFrequency22Bit(note);
    for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
        voices_[voice].operators[op].frequency = baseFreq * currentPreset_.frequencies[op];
        voices_[voice].operators[op].amplitude = currentPreset_.amplitudes[op];
        voices_[voice].operators[op].modulationIndex = currentPreset_.modulationIndices[op];
        voices_[voice].operators[op].waveform = currentPreset_.waveforms[op];
        voices_[voice].operators[op].attack = currentPreset_.attacks[op];
        voices_[voice].operators[op].decay = currentPreset_.decays[op];
        voices_[voice].operators[op].sustain = currentPreset_.sustains[op];
        voices_[voice].operators[op].release = currentPreset_.releases[op];
        
        voices_[voice].operators[op].phaseIncrement = calculatePhaseIncrement22Bit(voices_[voice].operators[op].frequency);
        voices_[voice].operators[op].envelopeState = static_cast<int>(EnvelopeState::ATTACK);
        voices_[voice].operators[op].envelopeTime = 0.0;
        voices_[voice].operators[op].envelopeLevel = 0.0;
    }
}

/**
 * @brief Trigger a note off event
 * 
 * Stops playing a note on the synthesizer. Finds the voice playing the
 * specified note and triggers its release phase.
 * 
 * @param note The MIDI note number to stop
 */
void FMSynthesizer::noteOff(int note) {
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (voices_[voice].active && voices_[voice].note == note) {
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                voices_[voice].operators[op].envelopeState = static_cast<int>(EnvelopeState::RELEASE);
                voices_[voice].operators[op].envelopeTime = 0.0;
            }
        }
    }
}

/**
 * @brief Stop all currently playing notes
 * 
 * Stops all notes on all voices by triggering their release phases.
 */
void FMSynthesizer::allNotesOff() {
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (voices_[voice].active) {
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                voices_[voice].operators[op].envelopeState = static_cast<int>(EnvelopeState::RELEASE);
                voices_[voice].operators[op].envelopeTime = 0.0;
            }
        }
    }
}

void FMSynthesizer::setChannelActive(int channel, bool active) {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS) {
        channels_[channel].active = active;
    }
}

bool FMSynthesizer::isChannelActive(int channel) const {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS) {
        return channels_[channel].active;
    }
    return false;
}

void FMSynthesizer::setOperatorFrequency(int voice, int opIndex, double frequency) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        voices_[voice].operators[opIndex].frequency = frequency;
        voices_[voice].operators[opIndex].phaseIncrement = 
            calculatePhaseIncrement22Bit(frequency);
    }
}

void FMSynthesizer::setOperatorAmplitude(int voice, int opIndex, double amplitude) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        voices_[voice].operators[opIndex].amplitude = 
            std::clamp(amplitude, Constants::MIN_AMPLITUDE, Constants::MAX_AMPLITUDE);
    }
}

void FMSynthesizer::setOperatorModulationIndex(int voice, int opIndex, double index) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        voices_[voice].operators[opIndex].modulationIndex = index;
    }
}

void FMSynthesizer::setOperatorWaveform(int voice, int opIndex, WaveformType waveform) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        voices_[voice].operators[opIndex].waveform = waveform;
    }
}

void FMSynthesizer::setAlgorithm(int channel, int algorithm) {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS && algorithm >= 0 && algorithm < Constants::MAX_ALGORITHMS) {
        channels_[channel].algorithm = algorithm;
    }
}

void FMSynthesizer::setEnvelope(int voice, int opIndex, double attack, double decay, 
                                double sustain, double release) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        auto& op = voices_[voice].operators[opIndex];
        op.attack = std::max(Constants::MIN_ENVELOPE_TIME, attack);
        op.decay = std::max(Constants::MIN_ENVELOPE_TIME, decay);
        op.sustain = std::clamp(sustain, Constants::MIN_VOLUME, Constants::MAX_VOLUME);
        op.release = std::max(Constants::MIN_ENVELOPE_TIME, release);
    }
}

/**
 * @brief Set the master volume
 * 
 * Sets the overall volume level for the synthesizer output.
 * 
 * @param volume The volume level (0.0 to 1.0)
 */
void FMSynthesizer::setMasterVolume(double volume) {
    masterVolume_ = std::clamp(volume, Constants::MIN_VOLUME, Constants::MAX_VOLUME);
}

/**
 * @brief Set the audio sample rate
 * 
 * Changes the sample rate of the synthesizer and recalculates the time step.
 * 
 * @param sampleRate The new sample rate in Hz
 */
void FMSynthesizer::setSampleRate(int sampleRate) {
    sampleRate_ = sampleRate;
    timeStep_ = 1.0 / sampleRate;
    
    for (auto& voice : voices_) {
        for (auto& op : voice.operators) {
            op.phaseIncrement = calculatePhaseIncrement22Bit(op.frequency);
        }
    }
}

/**
 * @brief Start the audio generation thread
 * 
 * Starts the background thread that generates audio samples and feeds them
 * to the audio output system.
 */
void FMSynthesizer::startAudioThread() {
    if (audioThreadRunning_) {
        return;
    }
    
    shouldStop_ = false;
    audioThreadRunning_ = true;
    audioThread_ = std::thread(&FMSynthesizer::audioThreadFunction, this);
}

/**
 * @brief Stop the audio generation thread
 * 
 * Stops the background audio thread and waits for it to finish.
 */
void FMSynthesizer::stopAudioThread() {
    if (!audioThreadRunning_) {
        return;
    }
    
    shouldStop_ = true;
    
    if (audioThread_.joinable()) {
        audioThread_.join();
    }
    
    audioThreadRunning_ = false;
}


void FMSynthesizer::setSampleStream(AudioSampleStream* stream) {
    externalStream_ = stream;
}


/**
 * @brief Check if the audio thread is running
 * 
 * @return True if the audio thread is running, false otherwise
 */
bool FMSynthesizer::isAudioThreadRunning() const {
    return audioThreadRunning_;
}

/**
 * @brief Generate audio samples and write them to a stream
 * 
 * Generates a buffer of audio samples and writes them to the specified
 * audio sample stream.
 * 
 * @param stream The audio sample stream to write to
 */
void FMSynthesizer::generateSamples(AudioSampleStream& stream) {
    double leftSample = 0.0, rightSample = 0.0;
    
    int activeVoices = 0;
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (!voices_[voice].active) continue;
            activeVoices++;
            
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                updateEnvelope(voices_[voice].operators[op]);
            }
            
            bool allReleased = true;
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                if (voices_[voice].operators[op].envelopeState != static_cast<int>(EnvelopeState::OFF)) {
                    allReleased = false;
                    break;
                }
            }
            if (allReleased) {
                voices_[voice].active = false;
                continue;
            }
            
            double voiceOutput = 0.0;
            int channel = voices_[voice].channel;
            switch (channels_[channel].algorithm) {
                case 0: voiceOutput = processAlgorithm0(voices_[voice]); break;
                case 1: voiceOutput = processAlgorithm1(voices_[voice]); break;
                case 2: voiceOutput = processAlgorithm2(voices_[voice]); break;
                case 3: voiceOutput = processAlgorithm3(voices_[voice]); break;
                case 4: voiceOutput = processAlgorithm4(voices_[voice]); break;
                case 5: voiceOutput = processAlgorithm5(voices_[voice]); break;
                case 6: voiceOutput = processAlgorithm6(voices_[voice]); break;
                case 7: voiceOutput = processAlgorithm7(voices_[voice]); break;
                case 8: voiceOutput = processAlgorithm8(voices_[voice]); break;
                case 9: voiceOutput = processAlgorithm9(voices_[voice]); break;
                case 10: voiceOutput = processAlgorithm10(voices_[voice]); break;
                case 11: voiceOutput = processAlgorithm11(voices_[voice]); break;
                case 12: voiceOutput = processAlgorithm12(voices_[voice]); break;
                case 13: voiceOutput = processAlgorithm13(voices_[voice]); break;
                case 14: voiceOutput = processAlgorithm14(voices_[voice]); break;
                case 15: voiceOutput = processAlgorithm15(voices_[voice]); break;
                case 16: voiceOutput = processAlgorithm16(voices_[voice]); break;
                case 17: voiceOutput = processAlgorithm17(voices_[voice]); break;
                case 18: voiceOutput = processAlgorithm18(voices_[voice]); break;
                case 19: voiceOutput = processAlgorithm19(voices_[voice]); break;
                case 20: voiceOutput = processAlgorithm20(voices_[voice]); break;
                case 21: voiceOutput = processAlgorithm21(voices_[voice]); break;
                case 22: voiceOutput = processAlgorithm22(voices_[voice]); break;
                case 23: voiceOutput = processAlgorithm23(voices_[voice]); break;
                case 24: voiceOutput = processAlgorithm24(voices_[voice]); break;
                case 25: voiceOutput = processAlgorithm25(voices_[voice]); break;
                case 26: voiceOutput = processAlgorithm26(voices_[voice]); break;
                case 27: voiceOutput = processAlgorithm27(voices_[voice]); break;
                case 28: voiceOutput = processAlgorithm28(voices_[voice]); break;
                case 29: voiceOutput = processAlgorithm29(voices_[voice]); break;
                case 30: voiceOutput = processAlgorithm30(voices_[voice]); break;
                case 31: voiceOutput = processAlgorithm31(voices_[voice]); break;
            }
            
            voiceOutput = applyEffects(voiceOutput, channel);
            
            for (auto& op : voices_[voice].operators) {
                updateOperatorPhase(op);
            }
            
            double pan = (voice % 2 == 0) ? Constants::PAN_LEFT : Constants::PAN_RIGHT;
            leftSample += voiceOutput * (Constants::PAN_SCALE - pan);
            rightSample += voiceOutput * (Constants::PAN_SCALE + pan);
        }
    int16_t left = static_cast<int16_t>(std::clamp(leftSample * Constants::AUDIO_SCALE, 
                                                  static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                                  static_cast<double>(Constants::AUDIO_MAX_VALUE)));
    int16_t right = static_cast<int16_t>(std::clamp(rightSample * Constants::AUDIO_SCALE, 
                                                   static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                                   static_cast<double>(Constants::AUDIO_MAX_VALUE)));
    
    
    stream.writeSample(left);
    stream.writeSample(right);
    
}

void FMSynthesizer::generateSample(int16_t& left, int16_t& right) {
    double leftSample = 0.0, rightSample = 0.0;
    
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (!voices_[voice].active) continue;
        
        for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
            updateEnvelope(voices_[voice].operators[op]);
        }
        
        bool allReleased = true;
        for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
            if (voices_[voice].operators[op].envelopeState != static_cast<int>(EnvelopeState::OFF)) {
                allReleased = false;
                break;
            }
        }
        if (allReleased) {
            voices_[voice].active = false;
            continue;
        }
        
        double voiceOutput = 0.0;
        int channel = voices_[voice].channel;
        switch (channels_[channel].algorithm) {
            case 0: voiceOutput = processAlgorithm0(voices_[voice]); break;
            case 1: voiceOutput = processAlgorithm1(voices_[voice]); break;
            case 2: voiceOutput = processAlgorithm2(voices_[voice]); break;
            case 3: voiceOutput = processAlgorithm3(voices_[voice]); break;
            case 4: voiceOutput = processAlgorithm4(voices_[voice]); break;
            case 5: voiceOutput = processAlgorithm5(voices_[voice]); break;
            case 6: voiceOutput = processAlgorithm6(voices_[voice]); break;
            case 7: voiceOutput = processAlgorithm7(voices_[voice]); break;
            case 8: voiceOutput = processAlgorithm8(voices_[voice]); break;
            case 9: voiceOutput = processAlgorithm9(voices_[voice]); break;
            case 10: voiceOutput = processAlgorithm10(voices_[voice]); break;
            case 11: voiceOutput = processAlgorithm11(voices_[voice]); break;
            case 12: voiceOutput = processAlgorithm12(voices_[voice]); break;
            case 13: voiceOutput = processAlgorithm13(voices_[voice]); break;
            case 14: voiceOutput = processAlgorithm14(voices_[voice]); break;
            case 15: voiceOutput = processAlgorithm15(voices_[voice]); break;
            case 16: voiceOutput = processAlgorithm16(voices_[voice]); break;
            case 17: voiceOutput = processAlgorithm17(voices_[voice]); break;
            case 18: voiceOutput = processAlgorithm18(voices_[voice]); break;
            case 19: voiceOutput = processAlgorithm19(voices_[voice]); break;
            case 20: voiceOutput = processAlgorithm20(voices_[voice]); break;
            case 21: voiceOutput = processAlgorithm21(voices_[voice]); break;
            case 22: voiceOutput = processAlgorithm22(voices_[voice]); break;
            case 23: voiceOutput = processAlgorithm23(voices_[voice]); break;
            case 24: voiceOutput = processAlgorithm24(voices_[voice]); break;
            case 25: voiceOutput = processAlgorithm25(voices_[voice]); break;
            case 26: voiceOutput = processAlgorithm26(voices_[voice]); break;
            case 27: voiceOutput = processAlgorithm27(voices_[voice]); break;
            case 28: voiceOutput = processAlgorithm28(voices_[voice]); break;
            case 29: voiceOutput = processAlgorithm29(voices_[voice]); break;
            case 30: voiceOutput = processAlgorithm30(voices_[voice]); break;
            case 31: voiceOutput = processAlgorithm31(voices_[voice]); break;
        }
        
        voiceOutput = applyEffects(voiceOutput, channel);
        
        for (auto& op : voices_[voice].operators) {
            updateOperatorPhase(op);
        }
        
        double pan = (voice % 2 == 0) ? Constants::PAN_LEFT : Constants::PAN_RIGHT;
        leftSample += voiceOutput * (Constants::PAN_SCALE - pan);
        rightSample += voiceOutput * (Constants::PAN_SCALE + pan);
    }
    left = static_cast<int16_t>(std::clamp(leftSample * Constants::AUDIO_SCALE, 
                                          static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                          static_cast<double>(Constants::AUDIO_MAX_VALUE)));
    right = static_cast<int16_t>(std::clamp(rightSample * Constants::AUDIO_SCALE, 
                                           static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                           static_cast<double>(Constants::AUDIO_MAX_VALUE)));
}

void FMSynthesizer::audioThreadFunction() {
    while (!shouldStop_) {
        bool hasActiveVoices = false;
        for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (voices_[voice].active) {
                hasActiveVoices = true;
                break;
            }
        }
        
        if (hasActiveVoices) {
            if (audioIODevice_) {
                const int BATCH_SIZE = 256;
                std::vector<int16_t> sampleBuffer;
                sampleBuffer.reserve(BATCH_SIZE * 2);
                
                for (int i = 0; i < BATCH_SIZE; i++) {
                    int16_t left, right;
                    generateSample(left, right);
                    sampleBuffer.push_back(left);
                    sampleBuffer.push_back(right);
                }
                
                qint64 bytesToWrite = sampleBuffer.size() * sizeof(int16_t);
                qint64 bytesWritten = audioIODevice_->write(
                    reinterpret_cast<const char*>(sampleBuffer.data()), 
                    bytesToWrite
                );
            } else {
                if (externalStream_) {
                    generateSamples(*externalStream_);
                } else {
                    generateSamples(*sampleStream_);
                }
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(5800));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


void FMSynthesizer::setPitchBend(int channel, double bend) {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS) {
        channels_[channel].pitchBend = bend;
        for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (voices_[voice].channel == channel) {
                for (auto& op : voices_[voice].operators) {
                    op.pitchBend = bend;
                }
            }
        }
    }
}

void FMSynthesizer::setModulationWheel(int channel, double mod) {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS) {
        channels_[channel].modulationWheel = mod;
        for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (voices_[voice].channel == channel) {
                for (auto& op : voices_[voice].operators) {
                    op.modulationWheel = mod;
                }
            }
        }
    }
}

void FMSynthesizer::setReverb(double amount) {
    reverbAmount_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
}

void FMSynthesizer::setChorus(double amount) {
    chorusAmount_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
}

void FMSynthesizer::setDistortion(double amount) {
    distortionAmount_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
}

void FMSynthesizer::setPresetConfig(const std::array<double, 6>& frequencies,
                                   const std::array<double, 6>& amplitudes,
                                   const std::array<double, 6>& modulationIndices,
                                   const std::array<WaveformType, 6>& waveforms,
                                   const std::array<double, 6>& attacks,
                                   const std::array<double, 6>& decays,
                                   const std::array<double, 6>& sustains,
                                   const std::array<double, 6>& releases) {
    for (int i = 0; i < 6; i++) {
        currentPreset_.frequencies[i] = frequencies[i];
        currentPreset_.amplitudes[i] = amplitudes[i];
        currentPreset_.modulationIndices[i] = modulationIndices[i];
        currentPreset_.waveforms[i] = waveforms[i];
        currentPreset_.attacks[i] = attacks[i];
        currentPreset_.decays[i] = decays[i];
        currentPreset_.sustains[i] = sustains[i];
        currentPreset_.releases[i] = releases[i];
    }
    
}

void FMSynthesizer::updateOperatorPhase(Operator& op) {
    updateOperatorPhase22Bit(op);
}

void FMSynthesizer::updateEnvelope(Operator& op) {
    op.envelopeTime += timeStep_;
    
    switch (static_cast<EnvelopeState>(op.envelopeState)) {
        case EnvelopeState::ATTACK:
            op.envelopeLevel = op.envelopeTime / op.attack;
            if (op.envelopeLevel >= Constants::MAX_VOLUME) {
                op.envelopeLevel = Constants::MAX_VOLUME;
                op.envelopeState = static_cast<int>(EnvelopeState::DECAY);
                op.envelopeTime = 0.0;
            }
            break;
            
        case EnvelopeState::DECAY:
            op.envelopeLevel = Constants::MAX_VOLUME - (op.envelopeTime / op.decay) * (Constants::MAX_VOLUME - op.sustain);
            if (op.envelopeLevel <= op.sustain) {
                op.envelopeLevel = op.sustain;
                op.envelopeState = static_cast<int>(EnvelopeState::SUSTAIN);
            }
            break;
            
        case EnvelopeState::SUSTAIN:
            op.envelopeLevel = op.sustain;
            break;
            
        case EnvelopeState::RELEASE:
            op.envelopeLevel = op.sustain * (Constants::MAX_VOLUME - op.envelopeTime / op.release);
            if (op.envelopeLevel <= Constants::MIN_VOLUME || 
                op.envelopeTime >= op.release) {
                op.envelopeLevel = Constants::MIN_VOLUME;
                op.envelopeState = static_cast<int>(EnvelopeState::OFF);
            }
            break;
            
        case EnvelopeState::OFF:
            op.envelopeLevel = Constants::MIN_VOLUME;
            break;
    }
}

double FMSynthesizer::generateOperatorOutput(Operator& op, double modulation) {
    double modScaled = modulation * Constants::FREQ_PRECISION_SCALE;
    double modRounded = round(modScaled) * Constants::FREQ_PRECISION_INV;
    
    double phase = op.phaseAccumulator + modRounded * op.modulationIndex;
    double output = 0.0;
    switch (static_cast<WaveformType>(op.waveform)) {
        case WaveformType::SINE:
            output = sin(phase);
            break;
        case WaveformType::SAWTOOTH:
            output = 2.0 * (phase / Constants::TWO_PI) - 1.0;
            break;
        case WaveformType::SQUARE:
            output = (phase < Constants::PI) ? 1.0 : -1.0;
            break;
        case WaveformType::TRIANGLE:
            if (phase < Constants::PI) {
                output = 2.0 * (phase / Constants::PI) - 1.0;
            } else {
                output = 3.0 - 2.0 * (phase / Constants::PI);
            }
            break;
    }
    
    return output * op.amplitude * op.envelopeLevel * op.velocity;
}

double FMSynthesizer::applyEffects(double sample, int channel) {
    if (distortionAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        sample = tanh(sample * (Constants::MAX_VOLUME + distortionAmount_ * Constants::DISTORTION_GAIN_MULTIPLIER));
    }
    
    if (chorusAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        double chorusMod = sin(Constants::TWO_PI * Constants::CHORUS_FREQUENCY * timeStep_) * chorusAmount_ * Constants::CHORUS_DEPTH;
        sample *= (Constants::MAX_VOLUME + chorusMod);
    }
    
    if (reverbAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        sample *= (Constants::MAX_VOLUME + reverbAmount_ * Constants::REVERB_GAIN);
    }
    
    return sample;
}


/**
 * @brief Convert MIDI note number to frequency
 * 
 * Converts a MIDI note number to its corresponding frequency in Hz
 * using the standard MIDI tuning (A4 = 440 Hz).
 * 
 * @param note The MIDI note number (0-127)
 * @return The frequency in Hz
 */
double FMSynthesizer::noteToFrequency(int note) {
    return Constants::MIDI_A4_FREQUENCY * pow(2.0, (note - Constants::MIDI_A4_NOTE) / Constants::MIDI_NOTES_PER_OCTAVE);
}

/**
 * @brief Convert MIDI note number to frequency with 22-bit precision
 * 
 * Converts a MIDI note number to its corresponding frequency in Hz
 * using 22-bit precision for high accuracy.
 * 
 * @param note The MIDI note number (0-127)
 * @return The frequency in Hz with 22-bit precision
 */
double FMSynthesizer::noteToFrequency22Bit(int note) {
    double frequency = Constants::MIDI_A4_FREQUENCY * pow(2.0, (note - Constants::MIDI_A4_NOTE) / Constants::MIDI_NOTES_PER_OCTAVE);
    
    double freqScaled = frequency * Constants::FREQ_PRECISION_SCALE;
    double freqRounded = round(freqScaled) * Constants::FREQ_PRECISION_INV;
    
    return freqRounded;
}

/**
 * @brief Calculate phase increment with 22-bit precision
 * 
 * Calculates the phase increment for a given frequency using 22-bit precision.
 * This provides high accuracy for frequency calculations.
 * 
 * @param frequency The frequency in Hz
 * @return The phase increment with 22-bit precision
 */
double FMSynthesizer::calculatePhaseIncrement22Bit(double frequency) {
    double freqScaled = frequency * Constants::FREQ_PRECISION_SCALE;
    double freqRounded = round(freqScaled) * Constants::FREQ_PRECISION_INV;
    
    double phaseIncrement = Constants::TWO_PI * freqRounded / sampleRate_;
    
    return phaseIncrement;
}

void FMSynthesizer::updateOperatorPhase22Bit(Operator& op) {
    double phaseInc = calculatePhaseIncrement22Bit(op.frequency);
    
    double pitchBendPhaseInc = phaseInc * op.pitchBend;
    
    op.phaseAccumulator += pitchBendPhaseInc;
    
    if (op.phaseAccumulator >= Constants::TWO_PI) {
        op.phaseAccumulator -= Constants::TWO_PI;
    }
}


/**
 * @brief Find an available voice for playing a note
 * 
 * Searches for a voice that is not currently playing a note.
 * If no voices are available, returns -1.
 * 
 * @return The index of an available voice, or -1 if none available
 */
int FMSynthesizer::findFreeVoice() {
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (!voices_[voice].active) {
            return voice;
        }
    }
    return -1;
}

/**
 * @brief Release a voice and stop it from playing
 * 
 * Stops the specified voice and marks it as available for new notes.
 * 
 * @param voice The index of the voice to release
 */
void FMSynthesizer::releaseVoice(int voice) {
    if (voice >= 0 && voice < Constants::MAX_VOICES) {
        voices_[voice].active = false;
        voices_[voice].note = -1;
    }
}

FMSynthesizerManager::FMSynthesizerManager(int sampleRate) 
    : sampleRate_(sampleRate), masterVolume_(Constants::MAX_VOLUME),
      globalReverb_(Constants::MIN_EFFECT_AMOUNT), globalChorus_(Constants::MIN_EFFECT_AMOUNT), 
      globalDistortion_(Constants::MIN_EFFECT_AMOUNT) {
    channelVolumes_.fill(Constants::MAX_VOLUME);
    channelPitchBends_.fill(Constants::MAX_VOLUME);
    channelModulations_.fill(Constants::MIN_EFFECT_AMOUNT);
}

FMSynthesizerManager::~FMSynthesizerManager() {
}

void FMSynthesizerManager::addSynthesizer(std::shared_ptr<FMSynthesizer> synth) {
    synthesizers_.push_back(synth);
}

void FMSynthesizerManager::removeSynthesizer(std::shared_ptr<FMSynthesizer> synth) {
    synthesizers_.erase(
        std::remove(synthesizers_.begin(), synthesizers_.end(), synth),
        synthesizers_.end()
    );
}

void FMSynthesizerManager::setMasterVolume(double volume) {
    masterVolume_ = std::clamp(volume, Constants::MIN_VOLUME, Constants::MAX_VOLUME);
}

void FMSynthesizerManager::setSampleRate(int sampleRate) {
    sampleRate_ = sampleRate;
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->setSampleRate(sampleRate);
        }
    }
}

void FMSynthesizerManager::noteOn(int channel, int note, double velocity) {
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->noteOn(note, velocity * channelVolumes_[channel]);
        }
    }
}

void FMSynthesizerManager::noteOff(int channel, int note) {
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->noteOff(note);
        }
    }
}

void FMSynthesizerManager::controlChange(int channel, int controller, double value) {
    if (channel < 0 || channel >= Constants::MAX_MIDI_CHANNELS) return;
    
    switch (static_cast<MIDIController>(controller)) {
        case MIDIController::MODULATION_WHEEL:
            channelModulations_[channel] = value;
            for (auto& synth : synthesizers_) {
                if (synth) {
                    synth->setModulationWheel(channel, value);
                }
            }
            break;
            
        case MIDIController::VOLUME:
            channelVolumes_[channel] = value;
            break;
            
        case MIDIController::EXPRESSION:
            break;
            
        default:
            break;
    }
}


void FMSynthesizerManager::setGlobalReverb(double amount) {
    globalReverb_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->setReverb(amount);
        }
    }
}

void FMSynthesizerManager::setGlobalChorus(double amount) {
    globalChorus_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->setChorus(amount);
        }
    }
}

void FMSynthesizerManager::setGlobalDistortion(double amount) {
    globalDistortion_ = std::clamp(amount, Constants::MIN_EFFECT_AMOUNT, Constants::MAX_EFFECT_AMOUNT);
    for (auto& synth : synthesizers_) {
        if (synth) {
            synth->setDistortion(amount);
        }
    }
}

void FMSynthesizer::setFreqPrecisionBits(int bits) { 
    freqPrecisionBits_ = bits; 
    freqPrecisionScale_ = std::pow(2.0, bits);
    freqPrecisionInv_ = 1.0 / freqPrecisionScale_;
}
void FMSynthesizer::setFreqPrecisionScale(double scale) { 
    freqPrecisionScale_ = scale; 
    freqPrecisionInv_ = 1.0 / scale;
}
void FMSynthesizer::setFreqPrecisionInv(double inv) { 
    freqPrecisionInv_ = inv; 
    freqPrecisionScale_ = 1.0 / inv;
}
void FMSynthesizer::setAudioBits(int bits) { audioBits_ = bits; }
void FMSynthesizer::setAudioMaxValue(int value) { audioMaxValue_ = value; }
void FMSynthesizer::setAudioMinValue(int value) { audioMinValue_ = value; }
void FMSynthesizer::setAudioScale(double scale) { audioScale_ = scale; }
void FMSynthesizer::setMidiA4Note(int note) { midiA4Note_ = note; }
void FMSynthesizer::setMidiA4Frequency(double frequency) { midiA4Frequency_ = frequency; }
void FMSynthesizer::setMidiNotesPerOctave(int notes) { midiNotesPerOctave_ = notes; }
void FMSynthesizer::setMaxVoices(int voices) { maxVoices_ = voices; }
void FMSynthesizer::setMaxOperators(int operators) { maxOperators_ = operators; }
void FMSynthesizer::setMaxChannels(int channels) { maxChannels_ = channels; }
void FMSynthesizer::setMaxAlgorithms(int algorithms) { maxAlgorithms_ = algorithms; }
void FMSynthesizer::setMaxMidiChannels(int channels) { maxMidiChannels_ = channels; }
void FMSynthesizer::setMinEnvelopeTime(double time) { minEnvelopeTime_ = time; }
void FMSynthesizer::setMaxEnvelopeTime(double time) { maxEnvelopeTime_ = time; }
void FMSynthesizer::setMinVolume(double volume) { minVolume_ = volume; }
void FMSynthesizer::setMaxVolume(double volume) { maxVolume_ = volume; }
void FMSynthesizer::setMinAmplitude(double amplitude) { minAmplitude_ = amplitude; }
void FMSynthesizer::setMaxAmplitude(double amplitude) { maxAmplitude_ = amplitude; }
void FMSynthesizer::setMaxEffectAmount(double amount) { maxEffectAmount_ = amount; }
void FMSynthesizer::setMinEffectAmount(double amount) { minEffectAmount_ = amount; }
void FMSynthesizer::setDistortionGainMultiplier(double multiplier) { distortionGainMultiplier_ = multiplier; }
void FMSynthesizer::setChorusFrequency(double frequency) { chorusFrequency_ = frequency; }
void FMSynthesizer::setChorusDepth(double depth) { chorusDepth_ = depth; }
void FMSynthesizer::setReverbGain(double gain) { reverbGain_ = gain; }
void FMSynthesizer::setPanLeft(double pan) { panLeft_ = pan; }
void FMSynthesizer::setPanCenter(double pan) { panCenter_ = pan; }
void FMSynthesizer::setPanRight(double pan) { panRight_ = pan; }
void FMSynthesizer::setPanScale(double scale) { panScale_ = scale; }

} // namespace toybasic