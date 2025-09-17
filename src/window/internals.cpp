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

#include "window/main.hpp"

/**
 * @brief Setup connections for the internals tab controls
 * 
 * Connects all the spin boxes and controls in the internals tab to their
 * corresponding synthesizer setter methods. This allows real-time editing
 * of synthesizer internal parameters like audio bits, MIDI settings, and
 * various limits and constraints.
 */
void MainWindow::setupInternalsTabConnections()
{
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
