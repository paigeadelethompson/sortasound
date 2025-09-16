#include "mainwindow.hpp"

void MainWindow::onSynthesizerChanged(int index)
{
    if (index >= 0 && index < static_cast<int>(synthesizers_.size())) {
        currentSynthesizerIndex_ = index;
        printf("Switched to synthesizer %d\n", index);
        refreshInternalsTab();
    }
}

void MainWindow::onAddSynthesizer()
{
    // Create new synthesizer instance
    synthesizers_.push_back(std::make_unique<toybasic::FMSynthesizer>());
    synthesizers_.back()->startAudioThread();
    
    // Update the selector
    updateSynthesizerSelector();
    
    // Select the new synthesizer
    currentSynthesizerIndex_ = static_cast<int>(synthesizers_.size()) - 1;
    synthSelector_->setCurrentIndex(currentSynthesizerIndex_);
    
    printf("Added synthesizer %d\n", currentSynthesizerIndex_);
}

void MainWindow::onRemoveSynthesizer()
{
    if (synthesizers_.size() <= 1) {
        printf("Cannot remove the last synthesizer\n");
        return;
    }
    
    // Stop the current synthesizer
    synthesizers_[currentSynthesizerIndex_]->stopAudioThread();
    
    // Remove it from the vector
    synthesizers_.erase(synthesizers_.begin() + currentSynthesizerIndex_);
    
    // Adjust the current index if necessary
    if (currentSynthesizerIndex_ >= static_cast<int>(synthesizers_.size())) {
        currentSynthesizerIndex_ = static_cast<int>(synthesizers_.size()) - 1;
    }
    
    // Update the selector
    updateSynthesizerSelector();
    synthSelector_->setCurrentIndex(currentSynthesizerIndex_);
    
    printf("Removed synthesizer, now using %d\n", currentSynthesizerIndex_);
}

void MainWindow::updateSynthesizerSelector()
{
    synthSelector_->clear();
    for (size_t i = 0; i < synthesizers_.size(); i++) {
        synthSelector_->addItem(QString("Synth %1").arg(i + 1));
    }
    
    // Update remove button state
    removeSynthButton_->setEnabled(synthesizers_.size() > 1);
}

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
