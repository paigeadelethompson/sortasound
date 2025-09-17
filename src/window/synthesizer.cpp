#include "window/main.hpp"

// Note: Old synthesizer management methods removed - now using tab-based system

void MainWindow::refreshInternalsTab()
{
    auto* currentSynth = getCurrentSynthesizer();
    if (!currentSynth) return;
    
    // Update all controls with current synthesizer values
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

toybasic::FMSynthesizer* MainWindow::getCurrentSynthesizer()
{
    if (currentSynthesizerIndex_ >= 0 && currentSynthesizerIndex_ < static_cast<int>(synthesizers_.size())) {
        return synthesizers_[currentSynthesizerIndex_].get();
    }
    return nullptr;
}
