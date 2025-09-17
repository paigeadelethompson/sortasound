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
 * @brief Refresh the internals tab with current synthesizer values
 * 
 * Updates all the spin boxes and controls in the internals tab to display
 * the current values from the active synthesizer. This is called when
 * switching between synthesizer tabs or when the synthesizer parameters change.
 */
void MainWindow::refreshInternalsTab()
{
    auto* currentSynth = getCurrentSynthesizer();
    if (!currentSynth) return;
    
    audioBitsSpinBox_->setValue(currentSynth->getAudioBits());
    audioMaxSpinBox_->setValue(currentSynth->getAudioMaxValue());
    audioMinSpinBox_->setValue(currentSynth->getAudioMinValue());
    audioScaleSpinBox_->setValue(currentSynth->getAudioScale());
    
    midiA4NoteSpinBox_->setValue(currentSynth->getMidiA4Note());
    midiA4FreqSpinBox_->setValue(currentSynth->getMidiA4Frequency());
    midiNotesSpinBox_->setValue(currentSynth->getMidiNotesPerOctave());
    
    maxVoicesSpinBox_->setValue(currentSynth->getMaxVoices());
    maxOpsSpinBox_->setValue(currentSynth->getMaxOperators());
    maxChannelsSpinBox_->setValue(currentSynth->getMaxChannels());
    maxAlgsSpinBox_->setValue(currentSynth->getMaxAlgorithms());
    
    minEnvTimeSpinBox_->setValue(currentSynth->getMinEnvelopeTime());
    maxEnvTimeSpinBox_->setValue(currentSynth->getMaxEnvelopeTime());
    
    minVolumeSpinBox_->setValue(currentSynth->getMinVolume());
    maxVolumeSpinBox_->setValue(currentSynth->getMaxVolume());
    minAmplitudeSpinBox_->setValue(currentSynth->getMinAmplitude());
    maxAmplitudeSpinBox_->setValue(currentSynth->getMaxAmplitude());
}

/**
 * @brief Get the currently active synthesizer
 * 
 * Returns a pointer to the synthesizer instance that corresponds to the
 * currently active tab. This is used throughout the application to
 * interact with the correct synthesizer instance.
 * 
 * @return Pointer to the current synthesizer, or nullptr if no synthesizer is active
 */
toybasic::FMSynthesizer* MainWindow::getCurrentSynthesizer()
{
    if (currentSynthesizerIndex_ >= 0 && currentSynthesizerIndex_ < static_cast<int>(synthesizers_.size())) {
        return synthesizers_[currentSynthesizerIndex_].get();
    }
    return nullptr;
}
