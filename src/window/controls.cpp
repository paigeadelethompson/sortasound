#include "window/main.hpp"

void MainWindow::onVolumeChanged(int value)
{
    double volume = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setMasterVolume(volume);
    }
    volumeLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onPresetChanged(int index)
{
    // Clear all active notes when changing presets (clear UI state first)
    activeNotes_.clear();
    keyboardWidget_->setActiveNotes(activeNotes_);
    qDebug() << "MainWindow::onPresetChanged - cleared UI notes";
    
    // Create a new synthesizer instance and apply the preset
    // Apply preset configuration to the synthesizer
    if (auto* synth = getCurrentSynthesizer()) {
        presetManager_->applyPreset(*synth, currentChannel_, index);
        
        // Only reapply non-preset settings (pitch bend and modulation wheel)
        // DO NOT overwrite preset values for volume, reverb, chorus, distortion, or algorithm
        synth->setPitchBend(currentChannel_, pitchBendSlider_->value() / 200.0 + 1.0);
        synth->setModulationWheel(currentChannel_, modWheelSlider_->value() / 127.0);
    }
    
    // Update UI sliders to match the preset values
    const auto& preset = presetManager_->getPreset(index);
    volumeSlider_->setValue(static_cast<int>(preset.masterVolume * 100));
    reverbSlider_->setValue(static_cast<int>(preset.reverb * 100));
    chorusSlider_->setValue(static_cast<int>(preset.chorus * 100));
    distortionSlider_->setValue(static_cast<int>(preset.distortion * 100));
    algorithmCombo_->setCurrentIndex(preset.algorithm);
    
    // Update labels
    volumeLabel_->setText(QString("%1%").arg(static_cast<int>(preset.masterVolume * 100)));
    reverbLabel_->setText(QString("%1%").arg(static_cast<int>(preset.reverb * 100)));
    chorusLabel_->setText(QString("%1%").arg(static_cast<int>(preset.chorus * 100)));
    distortionLabel_->setText(QString("%1%").arg(static_cast<int>(preset.distortion * 100)));
    
    // Ensure MainWindow has focus for keyboard input
    setFocus();
    activateWindow(); // Bring window to front and give it focus
    qDebug() << "Applied preset:" << index << "to channel:" << currentChannel_ << "algorithm:" << preset.algorithm << "MainWindow focus set";
}

void MainWindow::onReverbChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setReverb(amount);
    }
    reverbLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onChorusChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setChorus(amount);
    }
    chorusLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onDistortionChanged(int value)
{
    double amount = value / 100.0;
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setDistortion(amount);
    }
    distortionLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onAlgorithmChanged(int index)
{
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setAlgorithm(currentChannel_, index);
    }
}

void MainWindow::onChannelChanged(int index)
{
    currentChannel_ = index;
    // Set the current channel as active
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setChannelActive(currentChannel_, true);
    }
}

void MainWindow::onPitchBendChanged(int value)
{
    double bend = 1.0 + (value / 200.0); // Convert to 0.5 to 1.5 range
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setPitchBend(currentChannel_, bend);
    }
    pitchBendLabel_->setText(QString("%1").arg(value));
}

void MainWindow::onModWheelChanged(int value)
{
    double mod = value / 127.0; // Convert to 0.0 to 1.0 range
    if (auto* synth = getCurrentSynthesizer()) {
        synth->setModulationWheel(currentChannel_, mod);
    }
    modWheelLabel_->setText(QString("%1").arg(value));
}

void MainWindow::onOperatorParameterChanged()
{
    // This method is no longer used with the new voice-based system
    // Operator parameters are now handled through presets
}
