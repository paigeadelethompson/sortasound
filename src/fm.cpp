#include "fm.hpp"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <chrono>
#include <thread>
#include <condition_variable>

namespace toybasic {

// FMAudioDevice implementation removed - using QAudioSink directly now

// Constructor
FMSynthesizer::FMSynthesizer(int sampleRate) 
    : sampleRate_(sampleRate), masterVolume_(Constants::MAX_VOLUME), timeStep_(1.0 / sampleRate),
      reverbAmount_(Constants::MIN_EFFECT_AMOUNT), chorusAmount_(Constants::MIN_EFFECT_AMOUNT), 
      distortionAmount_(Constants::MIN_EFFECT_AMOUNT),
      audioThreadRunning_(false), shouldStop_(false),
      sampleBuffer_(BUFFER_SIZE * 2), bufferWritePos_(0), bufferReadPos_(0),
      sampleStream_(std::make_unique<FMSampleStream>()),
      audioIODevice_(nullptr), audioSink_(nullptr),
      // Initialize configurable properties with constant defaults
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
    
    // Initialize default preset configuration
    for (int i = 0; i < 6; i++) {
        currentPreset_.frequencies[i] = 1.0; // Default frequency ratio
        currentPreset_.amplitudes[i] = 0.5;  // Default amplitude
        currentPreset_.modulationIndices[i] = 0.0; // No modulation by default
        currentPreset_.waveforms[i] = WaveformType::SINE; // Default to sine wave
        currentPreset_.attacks[i] = 0.01;    // Quick attack
        currentPreset_.decays[i] = 0.1;      // Quick decay
        currentPreset_.sustains[i] = 0.7;    // Good sustain level
        currentPreset_.releases[i] = 0.3;    // Quick release
    }
    
    // Setup audio
    setupAudio();
}

// Destructor
FMSynthesizer::~FMSynthesizer() {
    stopAudioThread();
}

// Audio setup
void FMSynthesizer::setupAudio() {
    // Set up audio format
    QAudioFormat format;
    format.setSampleRate(sampleRate_);
    format.setChannelCount(2); // Stereo
    format.setSampleFormat(QAudioFormat::Int16);

    // Get default audio output device
    QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (device.isNull()) {
        printf("No audio output device found\n");
        return;
    }

    // Check if format is supported
    if (!device.isFormatSupported(format)) {
        printf("Audio format not supported, using nearest supported format\n");
        format = device.preferredFormat();
    }

    // Create audio sink and start it in push mode
    audioSink_ = new QAudioSink(device, format);
    
    // Start audio sink and get the device for direct writing (push mode)
    audioIODevice_ = audioSink_->start();

    if (audioSink_->state() != QAudio::ActiveState) {
        printf("Failed to start audio output\n");
        return;
    }

    printf("Audio setup completed successfully\n");
}

// Voice management
void FMSynthesizer::noteOn(int note, double velocity) {
    int voice = findFreeVoice();
    if (voice == -1) {
        // No free voices, steal voice 0
        voice = 0;
        releaseVoice(0);
    }
    
    voices_[voice].active = true;
    voices_[voice].note = note;
    voices_[voice].velocity = velocity;
    voices_[voice].channel = 0; // Default to channel 0
    
    // Debug output
    printf("FM noteOn: note=%d, velocity=%.2f, voice=%d\n", note, velocity, voice);
    
    // Set operator frequencies based on note (22-bit precision)
    double baseFreq = noteToFrequency22Bit(note);
    for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
        // Apply preset configuration to the operator
        voices_[voice].operators[op].frequency = baseFreq * currentPreset_.frequencies[op];
        voices_[voice].operators[op].amplitude = currentPreset_.amplitudes[op];
        voices_[voice].operators[op].modulationIndex = currentPreset_.modulationIndices[op];
        voices_[voice].operators[op].waveform = currentPreset_.waveforms[op];
        voices_[voice].operators[op].attack = currentPreset_.attacks[op];
        voices_[voice].operators[op].decay = currentPreset_.decays[op];
        voices_[voice].operators[op].sustain = currentPreset_.sustains[op];
        voices_[voice].operators[op].release = currentPreset_.releases[op];
        
        // Use 22-bit precision for phase increment calculation
        voices_[voice].operators[op].phaseIncrement = calculatePhaseIncrement22Bit(voices_[voice].operators[op].frequency);
        voices_[voice].operators[op].envelopeState = static_cast<int>(EnvelopeState::ATTACK);
        voices_[voice].operators[op].envelopeTime = 0.0;
        voices_[voice].operators[op].envelopeLevel = 0.0;
    }
}

void FMSynthesizer::noteOff(int note) {
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (voices_[voice].active && voices_[voice].note == note) {
            // Trigger release phase
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                voices_[voice].operators[op].envelopeState = static_cast<int>(EnvelopeState::RELEASE);
                voices_[voice].operators[op].envelopeTime = 0.0;
            }
        }
    }
}

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

// Channel management
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

// Operator configuration
void FMSynthesizer::setOperatorFrequency(int voice, int opIndex, double frequency) {
    if (voice >= 0 && voice < Constants::MAX_VOICES && opIndex >= 0 && opIndex < Constants::MAX_OPERATORS) {
        voices_[voice].operators[opIndex].frequency = frequency;
        // Use 22-bit precision for phase increment calculation
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

// Algorithm configuration
void FMSynthesizer::setAlgorithm(int channel, int algorithm) {
    if (channel >= 0 && channel < Constants::MAX_CHANNELS && algorithm >= 0 && algorithm < Constants::MAX_ALGORITHMS) {
        channels_[channel].algorithm = algorithm;
    }
}

// Envelope configuration
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

// Global parameters
void FMSynthesizer::setMasterVolume(double volume) {
    masterVolume_ = std::clamp(volume, Constants::MIN_VOLUME, Constants::MAX_VOLUME);
}

void FMSynthesizer::setSampleRate(int sampleRate) {
    sampleRate_ = sampleRate;
    timeStep_ = 1.0 / sampleRate;
    
    // Update all phase increments with 22-bit precision
    for (auto& voice : voices_) {
        for (auto& op : voice.operators) {
            op.phaseIncrement = calculatePhaseIncrement22Bit(op.frequency);
        }
    }
}

// Threading and callback system
void FMSynthesizer::startAudioThread() {
    if (audioThreadRunning_) {
        printf("FM startAudioThread: Thread already running\n");
        return;
    }
    
    printf("FM startAudioThread: Starting audio thread\n");
    shouldStop_ = false;
    audioThreadRunning_ = true;
    audioThread_ = std::thread(&FMSynthesizer::audioThreadFunction, this);
    printf("FM startAudioThread: Audio thread started\n");
}

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
    printf("FM setSampleStream: Stream set\n");
}

// setAudioDevice method removed - we now use QAudioSink directly

bool FMSynthesizer::isAudioThreadRunning() const {
    return audioThreadRunning_;
}

void FMSynthesizer::generateSamples(AudioSampleStream& stream) {
    // Generate one sample of FM synthesis
    double leftSample = 0.0, rightSample = 0.0;
    
    int activeVoices = 0;
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (!voices_[voice].active) continue;
            activeVoices++;
            
            // Update envelopes for all operators in this voice
            for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
                updateEnvelope(voices_[voice].operators[op]);
            }
            
            // Check if voice should be released
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
            
            // Process based on algorithm (32 algorithms)
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
            
            // Apply effects
            voiceOutput = applyEffects(voiceOutput, channel);
            
            // Update operators
            for (auto& op : voices_[voice].operators) {
                updateOperatorPhase(op);
            }
            
            // Pan to stereo (simple left/right distribution)
            double pan = (voice % 2 == 0) ? Constants::PAN_LEFT : Constants::PAN_RIGHT;
            leftSample += voiceOutput * (Constants::PAN_SCALE - pan);
            rightSample += voiceOutput * (Constants::PAN_SCALE + pan);
        }
        
    // Convert to 14-bit and write to stream
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
    // Generate one sample of FM synthesis
    double leftSample = 0.0, rightSample = 0.0;
    
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (!voices_[voice].active) continue;
        
        // Update envelopes for all operators in this voice
        for (int op = 0; op < Constants::MAX_OPERATORS; op++) {
            updateEnvelope(voices_[voice].operators[op]);
        }
        
        // Check if voice should be released
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
        
        // Process based on algorithm (32 algorithms)
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
        
        // Apply effects
        voiceOutput = applyEffects(voiceOutput, channel);
        
        // Update operators
        for (auto& op : voices_[voice].operators) {
            updateOperatorPhase(op);
        }
        
        // Pan to stereo (simple left/right distribution)
        double pan = (voice % 2 == 0) ? Constants::PAN_LEFT : Constants::PAN_RIGHT;
        leftSample += voiceOutput * (Constants::PAN_SCALE - pan);
        rightSample += voiceOutput * (Constants::PAN_SCALE + pan);
    }
    
    // Convert to 14-bit and return
    left = static_cast<int16_t>(std::clamp(leftSample * Constants::AUDIO_SCALE, 
                                          static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                          static_cast<double>(Constants::AUDIO_MAX_VALUE)));
    right = static_cast<int16_t>(std::clamp(rightSample * Constants::AUDIO_SCALE, 
                                           static_cast<double>(Constants::AUDIO_MIN_VALUE), 
                                           static_cast<double>(Constants::AUDIO_MAX_VALUE)));
}

/*
 * Audio thread function for FM synthesizer.
 * 
 * This function:
 * 1. Calls the callback once to hand off the stream to AudioEngine (callback must return to unblock)
 * 2. Enters a continuous loop to generate audio samples when needed
 * 3. Checks for active voices and generates individual stereo samples or sleeps accordingly
 * 
 * The audio thread generates samples continuously when there are active voices, otherwise sleeps.
 * AudioEngine reads from the stream in its own thread and handles framing for the audio output system.
 * The audio thread writes individual samples to the stream via generateSamples().
 */
void FMSynthesizer::audioThreadFunction() {
    // Enter continuous loop to generate audio samples
    while (!shouldStop_) {
        // Check if we have any active voices
        bool hasActiveVoices = false;
        for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
            if (voices_[voice].active) {
                hasActiveVoices = true;
                break;
            }
        }
        
        if (hasActiveVoices) {
            // Generate samples in batches for better performance
            if (audioIODevice_) {
                const int BATCH_SIZE = 256; // Generate 256 samples at a time
                std::vector<int16_t> sampleBuffer;
                sampleBuffer.reserve(BATCH_SIZE * 2); // Stereo samples
                
                // Generate a batch of samples
                for (int i = 0; i < BATCH_SIZE; i++) {
                    int16_t left, right;
                    generateSample(left, right);
                    sampleBuffer.push_back(left);
                    sampleBuffer.push_back(right);
                }
                
                // Write directly to the QAudioSink's device
                qint64 bytesToWrite = sampleBuffer.size() * sizeof(int16_t);
                qint64 bytesWritten = audioIODevice_->write(
                    reinterpret_cast<const char*>(sampleBuffer.data()), 
                    bytesToWrite
                );
                
                if (bytesWritten != bytesToWrite) {
                    printf("Warning: Only wrote %lld of %lld bytes\n", bytesWritten, bytesToWrite);
                }
            } else {
                // Use stream method
                if (externalStream_) {
                    generateSamples(*externalStream_);
                } else {
                    generateSamples(*sampleStream_);
                }
            }
            
            // Sleep for the time it takes to generate this batch
            // At 44.1kHz, 256 samples = 256/44100 = ~5.8ms
            std::this_thread::sleep_for(std::chrono::microseconds(5800));
        } else {
            // No active voices, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


// Real-time control
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

// Effects
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
    // Store the preset configuration for new voices
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
    
    // Debug output to verify preset is being applied
    printf("Preset applied - Op0: freq=%.2f, amp=%.2f, mod=%.2f\n", 
           frequencies[0], amplitudes[0], modulationIndices[0]);
}

// Helper functions
void FMSynthesizer::updateOperatorPhase(Operator& op) {
    // Use 22-bit precision phase update
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
    // Calculate phase with 22-bit precision
    // Apply modulation with high precision
    
    // Scale modulation to 22-bit precision
    double modScaled = modulation * Constants::FREQ_PRECISION_SCALE;
    double modRounded = round(modScaled) * Constants::FREQ_PRECISION_INV;
    
    // Calculate final phase with high precision
    double phase = op.phaseAccumulator + modRounded * op.modulationIndex;
    double output = 0.0;
    
    // Generate waveform based on type
    switch (static_cast<WaveformType>(op.waveform)) {
        case WaveformType::SINE:
            output = sin(phase);
            break;
        case WaveformType::SAWTOOTH:
            // Sawtooth: phase from 0 to 2π maps to -1 to 1
            output = 2.0 * (phase / Constants::TWO_PI) - 1.0;
            break;
        case WaveformType::SQUARE:
            // Square wave: 1 if phase < π, -1 if phase >= π
            output = (phase < Constants::PI) ? 1.0 : -1.0;
            break;
        case WaveformType::TRIANGLE:
            // Triangle: linear ramp up to π, then down to 2π
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
    // Simple distortion
    if (distortionAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        sample = tanh(sample * (Constants::MAX_VOLUME + distortionAmount_ * Constants::DISTORTION_GAIN_MULTIPLIER));
    }
    
    // Simple chorus (slight pitch modulation)
    if (chorusAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        double chorusMod = sin(Constants::TWO_PI * Constants::CHORUS_FREQUENCY * timeStep_) * chorusAmount_ * Constants::CHORUS_DEPTH;
        sample *= (Constants::MAX_VOLUME + chorusMod);
    }
    
    // Simple reverb (basic delay)
    if (reverbAmount_ > Constants::MIN_EFFECT_AMOUNT) {
        sample *= (Constants::MAX_VOLUME + reverbAmount_ * Constants::REVERB_GAIN);
    }
    
    return sample;
}


double FMSynthesizer::noteToFrequency(int note) {
    // A4 = 440 Hz, MIDI note 69
    return Constants::MIDI_A4_FREQUENCY * pow(2.0, (note - Constants::MIDI_A4_NOTE) / Constants::MIDI_NOTES_PER_OCTAVE);
}

double FMSynthesizer::noteToFrequency22Bit(int note) {
    // 22-bit precision note to frequency conversion
    
    // Calculate frequency with high precision
    double frequency = Constants::MIDI_A4_FREQUENCY * pow(2.0, (note - Constants::MIDI_A4_NOTE) / Constants::MIDI_NOTES_PER_OCTAVE);
    
    // Round to 22-bit precision
    double freqScaled = frequency * Constants::FREQ_PRECISION_SCALE;
    double freqRounded = round(freqScaled) * Constants::FREQ_PRECISION_INV;
    
    return freqRounded;
}

// 22-bit precision frequency calculations
double FMSynthesizer::calculatePhaseIncrement22Bit(double frequency) {
    // Calculate phase increment with 22-bit precision
    // Using fixed-point arithmetic to maintain precision
    
    // Convert frequency to 22-bit fixed point, then back to double
    // This ensures we maintain 22-bit precision in the calculation
    double freqScaled = frequency * Constants::FREQ_PRECISION_SCALE;
    double freqRounded = round(freqScaled) * Constants::FREQ_PRECISION_INV;
    
    // Calculate phase increment with high precision
    double phaseIncrement = Constants::TWO_PI * freqRounded / sampleRate_;
    
    return phaseIncrement;
}

void FMSynthesizer::updateOperatorPhase22Bit(Operator& op) {
    // Update phase accumulator with 22-bit precision
    
    // Calculate phase increment with 22-bit precision
    double phaseInc = calculatePhaseIncrement22Bit(op.frequency);
    
    // Apply pitch bend with high precision
    double pitchBendPhaseInc = phaseInc * op.pitchBend;
    
    // Update phase accumulator
    op.phaseAccumulator += pitchBendPhaseInc;
    
    // Wrap phase accumulator to [0, 2π) with high precision
    if (op.phaseAccumulator >= Constants::TWO_PI) {
        op.phaseAccumulator -= Constants::TWO_PI;
    }
}


// Voice allocation
int FMSynthesizer::findFreeVoice() {
    for (int voice = 0; voice < Constants::MAX_VOICES; voice++) {
        if (!voices_[voice].active) {
            return voice;
        }
    }
    return -1; // No free voices
}

void FMSynthesizer::releaseVoice(int voice) {
    if (voice >= 0 && voice < Constants::MAX_VOICES) {
        voices_[voice].active = false;
        voices_[voice].note = -1;
    }
}

// FMSynthesizerManager implementation
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
            // Could be used for additional volume control
            break;
            
        default:
            // Handle other controllers if needed
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

// Property setter implementations
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