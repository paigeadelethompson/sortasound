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
 * @brief Handle volume slider changes
 * 
 * Called when the user changes the volume slider. Updates the master volume
 * of the current synthesizer and updates the volume label display.
 * 
 * @param value The new volume value (0-100)
 */
void MainWindow::onVolumeChanged(int value)
{
    double volume = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setMasterVolume(volume);
    }
    volumeLabel_->setText(QString("%1%").arg(value));
}

/**
 * @brief Handle preset selection changes
 * 
 * Called when the user selects a different preset from the dropdown.
 * Applies the new preset to the current synthesizer channel and updates
 * all related UI controls to match the preset settings.
 * 
 * @param index The index of the selected preset
 */
void MainWindow::onPresetChanged(int index)
{
    activeNotes_.clear();
    keyboardWidget_->setActiveNotes(activeNotes_);
    
    if (auto* synth = getCurrentSynthesizer()) {
        presetManager_->applyPreset(*synth, currentChannel_, index);
        
        synth->setPitchBend(currentChannel_, pitchBendSlider_->value() / 200.0 + 1.0);
        synth->setModulationWheel(currentChannel_, modWheelSlider_->value() / 127.0);
    }
    
    const auto& preset = presetManager_->getPreset(index);
    volumeSlider_->setValue(static_cast<int>(preset.masterVolume * 100));
    reverbSlider_->setValue(static_cast<int>(preset.reverb * 100));
    chorusSlider_->setValue(static_cast<int>(preset.chorus * 100));
    distortionSlider_->setValue(static_cast<int>(preset.distortion * 100));
    algorithmCombo_->setCurrentIndex(preset.algorithm);
    
    volumeLabel_->setText(QString("%1%").arg(static_cast<int>(preset.masterVolume * 100)));
    reverbLabel_->setText(QString("%1%").arg(static_cast<int>(preset.reverb * 100)));
    chorusLabel_->setText(QString("%1%").arg(static_cast<int>(preset.chorus * 100)));
    distortionLabel_->setText(QString("%1%").arg(static_cast<int>(preset.distortion * 100)));
    
    setFocus();
    activateWindow();
}

/**
 * @brief Handle reverb slider changes
 * 
 * Called when the user changes the reverb slider. Updates the reverb
 * amount on the current synthesizer and updates the reverb label display.
 * 
 * @param value The new reverb value (0-100)
 */
void MainWindow::onReverbChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setReverb(amount);
    }
    reverbLabel_->setText(QString("%1%").arg(value));
}

/**
 * @brief Handle chorus slider changes
 * 
 * Called when the user changes the chorus slider. Updates the chorus
 * amount on the current synthesizer and updates the chorus label display.
 * 
 * @param value The new chorus value (0-100)
 */
void MainWindow::onChorusChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setChorus(amount);
    }
    chorusLabel_->setText(QString("%1%").arg(value));
}

/**
 * @brief Handle distortion slider changes
 * 
 * Called when the user changes the distortion slider. Updates the distortion
 * amount on the current synthesizer and updates the distortion label display.
 * 
 * @param value The new distortion value (0-100)
 */
void MainWindow::onDistortionChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setDistortion(amount);
    }
    distortionLabel_->setText(QString("%1%").arg(value));
}

/**
 * @brief Handle algorithm selection changes
 * 
 * Called when the user selects a different FM algorithm from the dropdown.
 * Updates the algorithm for the current synthesizer channel.
 * 
 * @param index The index of the selected algorithm (0-31)
 */
void MainWindow::onAlgorithmChanged(int index)
{
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setAlgorithm(currentChannel_, index);
    }
}

/**
 * @brief Handle channel selection changes
 * 
 * Called when the user selects a different MIDI channel from the dropdown.
 * Updates the current channel and activates it on the synthesizer.
 * 
 * @param index The index of the selected channel (0-7)
 */
void MainWindow::onChannelChanged(int index)
{
    currentChannel_ = index;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setChannelActive(currentChannel_, true);
    }
}

/**
 * @brief Handle pitch bend slider changes
 * 
 * Called when the user changes the pitch bend slider. Updates the pitch bend
 * amount for the current synthesizer channel and updates the pitch bend label.
 * 
 * @param value The new pitch bend value (-200 to +200)
 */
void MainWindow::onPitchBendChanged(int value)
{
    double bend = 1.0 + (value / 200.0); // Convert to 0.5 to 1.5 range
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setPitchBend(currentChannel_, bend);
    }
    pitchBendLabel_->setText(QString("%1").arg(value));
    
    // Start timer to return to zero when released
    if (pitchBendReturnTimer_) {
        pitchBendReturnTimer_->stop();
    }
    pitchBendReturnTimer_ = new QTimer(this);
    pitchBendReturnTimer_->setSingleShot(true);
    pitchBendReturnTimer_->setInterval(100); // 100ms delay
    connect(pitchBendReturnTimer_, &QTimer::timeout, [this]() {
        pitchBendSlider_->setValue(0);
    });
    pitchBendReturnTimer_->start();
}

/**
 * @brief Handle modulation wheel slider changes
 * 
 * Called when the user changes the modulation wheel slider. Updates the
 * modulation wheel amount for the current synthesizer channel and updates
 * the modulation wheel label.
 * 
 * @param value The new modulation wheel value (0-127)
 */
void MainWindow::onModWheelChanged(int value)
{
    double mod = (value + 64) / 127.0; // Convert from -64 to 63 range to 0.0 to 1.0 range
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setModulationWheel(currentChannel_, mod);
    }
    modWheelLabel_->setText(QString("%1").arg(value));
}

/**
 * @brief Handle operator parameter changes
 * 
 * Called when operator parameters are changed. This is a placeholder for
 * future operator parameter editing functionality.
 */
void MainWindow::onOperatorParameterChanged()
{
}
