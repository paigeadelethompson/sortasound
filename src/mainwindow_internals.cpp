#include "mainwindow.hpp"

void MainWindow::setupInternalsTabConnections()
{
    // Connect controls to synthesizer setters
    connect(audioBitsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setAudioBits(value);
        }
    });
    
    connect(audioMaxSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setAudioMaxValue(value);
        }
    });
    
    connect(audioMinSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setAudioMinValue(value);
        }
    });
    
    connect(audioScaleSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setAudioScale(value);
        }
    });
    
    connect(midiA4NoteSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMidiA4Note(value);
        }
    });
    
    connect(midiA4FreqSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMidiA4Frequency(value);
        }
    });
    
    connect(midiNotesSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMidiNotesPerOctave(value);
        }
    });
    
    connect(maxVoicesSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxVoices(value);
        }
    });
    
    connect(maxOpsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxOperators(value);
        }
    });
    
    connect(maxChannelsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxChannels(value);
        }
    });
    
    connect(maxAlgsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxAlgorithms(value);
        }
    });
    
    connect(minEnvTimeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMinEnvelopeTime(value);
        }
    });
    
    connect(maxEnvTimeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxEnvelopeTime(value);
        }
    });
    
    connect(minVolumeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMinVolume(value);
        }
    });
    
    connect(maxVolumeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxVolume(value);
        }
    });
    
    connect(minAmplitudeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMinAmplitude(value);
        }
    });
    
    connect(maxAmplitudeSpinBox_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        if (auto* synth = getCurrentSynthesizer()) {
            synth->setMaxAmplitude(value);
        }
    });
}
