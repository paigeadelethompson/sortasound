#include "mainwindow.hpp"
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QDoubleSpinBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget_(nullptr)
    , mainLayout_(nullptr)
    , controlsLayout_(nullptr)
    , mainTabWidget_(nullptr)
    , currentSynthesizerIndex_(0)
    , presetManager_(std::make_unique<toybasic::PresetManager>())
    , keyboardWidget_(nullptr)
    , trackerWidget_(nullptr)
    , octaveGroup_(nullptr)
    , octaveSpinBox_(nullptr)
    , octaveLabel_(nullptr)
    , volumeGroup_(nullptr)
    , volumeSlider_(nullptr)
    , volumeLabel_(nullptr)
    , presetGroup_(nullptr)
    , presetCombo_(nullptr)
    , effectsGroup_(nullptr)
    , reverbSlider_(nullptr)
    , reverbLabel_(nullptr)
    , chorusSlider_(nullptr)
    , chorusLabel_(nullptr)
    , distortionSlider_(nullptr)
    , distortionLabel_(nullptr)
    , algorithmGroup_(nullptr)
    , algorithmCombo_(nullptr)
    , audioBitsSpinBox_(nullptr)
    , audioMaxSpinBox_(nullptr)
    , audioMinSpinBox_(nullptr)
    , audioScaleSpinBox_(nullptr)
    , midiA4NoteSpinBox_(nullptr)
    , midiA4FreqSpinBox_(nullptr)
    , midiNotesSpinBox_(nullptr)
    , maxVoicesSpinBox_(nullptr)
    , maxOpsSpinBox_(nullptr)
    , maxChannelsSpinBox_(nullptr)
    , maxAlgsSpinBox_(nullptr)
    , minEnvTimeSpinBox_(nullptr)
    , maxEnvTimeSpinBox_(nullptr)
    , minVolumeSpinBox_(nullptr)
    , maxVolumeSpinBox_(nullptr)
    , minAmplitudeSpinBox_(nullptr)
    , maxAmplitudeSpinBox_(nullptr)
    , pitchBendSlider_(nullptr)
    , modWheelSlider_(nullptr)
    , pitchBendLabel_(nullptr)
    , modWheelLabel_(nullptr)
    , channelGroup_(nullptr)
    , channelCombo_(nullptr)
    , synthManagerGroup_(nullptr)
    , synthSelector_(nullptr)
    , addSynthButton_(nullptr)
    , removeSynthButton_(nullptr)
    , currentChannel_(0)
{
    setupUI();
    setupKeyboardMapping();
    
    // Create the first synthesizer instance
    synthesizers_.push_back(std::make_unique<toybasic::FMSynthesizer>());
    synthesizers_[0]->startAudioThread();
}

MainWindow::~MainWindow()
{
    // Stop all synthesizer threads
    for (auto& synth : synthesizers_) {
        synth->stopAudioThread();
    }
}

// setupUI method moved to mainwindow_ui_setup.cpp

void MainWindow::setupKeyboardMapping()
{
    // Connect keyboard widget signals
    connect(keyboardWidget_, &KeyboardWidget::keyPressed, this, &MainWindow::onKeyboardKeyPressed);
    connect(keyboardWidget_, &KeyboardWidget::keyReleased, this, &MainWindow::onKeyboardKeyReleased);
    
    // Map keys to the current octave (will be updated when octave changes)
    updateKeyboardMapping();
}

void MainWindow::onKeyboardKeyPressed(int note)
{
    // Handle keyboard widget key press
    if (currentSynthesizerIndex_ < synthesizers_.size()) {
        synthesizers_[currentSynthesizerIndex_]->noteOn(note, 100); // Default velocity 100
    }
}

void MainWindow::onKeyboardKeyReleased(int note)
{
    // Handle keyboard widget key release
    if (currentSynthesizerIndex_ < synthesizers_.size()) {
        synthesizers_[currentSynthesizerIndex_]->noteOff(note);
    }
}

void MainWindow::updateKeyboardMapping()
{
    // Get current octave from keyboard widget
    int currentOctave = keyboardWidget_->getCurrentOctave();
    int octaveStart = currentOctave * 12; // Calculate starting note for current octave
    
    // Clear existing mapping
    keyToNoteMap_.clear();
    
    // Map keys to the first octave of the 4-octave range
    // First row: q to ]
    keyToNoteMap_[Qt::Key_Q] = octaveStart;      // C
    keyToNoteMap_[Qt::Key_W] = octaveStart + 1;  // C#
    keyToNoteMap_[Qt::Key_E] = octaveStart + 2;  // D
    keyToNoteMap_[Qt::Key_R] = octaveStart + 3;  // D#
    keyToNoteMap_[Qt::Key_T] = octaveStart + 4;  // E
    keyToNoteMap_[Qt::Key_Y] = octaveStart + 5;  // F
    keyToNoteMap_[Qt::Key_U] = octaveStart + 6;  // F#
    keyToNoteMap_[Qt::Key_I] = octaveStart + 7;  // G
    keyToNoteMap_[Qt::Key_O] = octaveStart + 8;  // G#
    keyToNoteMap_[Qt::Key_P] = octaveStart + 9;  // A
    keyToNoteMap_[Qt::Key_BracketLeft] = octaveStart + 10;  // A#
    keyToNoteMap_[Qt::Key_BracketRight] = octaveStart + 11; // B
    
    // Second row: a to ' (second octave)
    keyToNoteMap_[Qt::Key_A] = octaveStart + 12; // C (next octave)
    keyToNoteMap_[Qt::Key_S] = octaveStart + 13; // C#
    keyToNoteMap_[Qt::Key_D] = octaveStart + 14; // D
    keyToNoteMap_[Qt::Key_F] = octaveStart + 15; // D#
    keyToNoteMap_[Qt::Key_G] = octaveStart + 16; // E
    keyToNoteMap_[Qt::Key_H] = octaveStart + 17; // F
    keyToNoteMap_[Qt::Key_J] = octaveStart + 18; // F#
    keyToNoteMap_[Qt::Key_K] = octaveStart + 19; // G
    keyToNoteMap_[Qt::Key_L] = octaveStart + 20; // G#
    keyToNoteMap_[Qt::Key_Semicolon] = octaveStart + 21; // A
    keyToNoteMap_[Qt::Key_QuoteLeft] = octaveStart + 22; // A#
    keyToNoteMap_[Qt::Key_QuoteDbl] = octaveStart + 23;  // B
    
    // Third row: z to / (third octave)
    keyToNoteMap_[Qt::Key_Z] = octaveStart + 24; // C (next octave)
    keyToNoteMap_[Qt::Key_X] = octaveStart + 25; // C#
    keyToNoteMap_[Qt::Key_C] = octaveStart + 26; // D
    keyToNoteMap_[Qt::Key_V] = octaveStart + 27; // D#
    keyToNoteMap_[Qt::Key_B] = octaveStart + 28; // E
    keyToNoteMap_[Qt::Key_N] = octaveStart + 29; // F
    keyToNoteMap_[Qt::Key_M] = octaveStart + 30; // F#
    keyToNoteMap_[Qt::Key_Comma] = octaveStart + 31; // G
    keyToNoteMap_[Qt::Key_Period] = octaveStart + 32; // G#
    keyToNoteMap_[Qt::Key_Slash] = octaveStart + 33; // A
    
    // Set the key mapping in the keyboard widget
    keyboardWidget_->setKeyMapping(keyToNoteMap_);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    
    int note = keyToNote(static_cast<Qt::Key>(event->key()));
    qDebug() << "MainWindow::keyPressEvent - key:" << event->key() << "note:" << note;
    if (note != -1) {
        noteOn(note);
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    
    int note = keyToNote(static_cast<Qt::Key>(event->key()));
    qDebug() << "MainWindow::keyReleaseEvent - key:" << event->key() << "note:" << note;
    if (note != -1) {
        noteOff(note);
    }
    
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::focusInEvent(QFocusEvent *event)
{
    qDebug() << "MainWindow::focusInEvent - MainWindow gained focus";
    QMainWindow::focusInEvent(event);
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "MainWindow::focusOutEvent - MainWindow lost focus";
    QMainWindow::focusOutEvent(event);
}

int MainWindow::keyToNote(Qt::Key key) const
{
    auto it = keyToNoteMap_.find(key);
    int note = (it != keyToNoteMap_.end()) ? it->second : -1;
    qDebug() << "MainWindow::keyToNote - key:" << key << "mapped to note:" << note;
    return note;
}

void MainWindow::noteOn(int note)
{
    if (activeNotes_.find(note) == activeNotes_.end()) {
        activeNotes_.insert(note);
        if (auto* synth = getCurrentSynthesizer()) {
            synth->noteOn(note);
        }
        keyboardWidget_->setActiveNotes(activeNotes_);
        qDebug() << "MainWindow::noteOn - note:" << note << "activeNotes count:" << activeNotes_.size();
    }
}

void MainWindow::noteOff(int note)
{
    auto it = activeNotes_.find(note);
    if (it != activeNotes_.end()) {
        activeNotes_.erase(it);
        if (auto* synth = getCurrentSynthesizer()) {
            synth->noteOff(note);
        }
        keyboardWidget_->setActiveNotes(activeNotes_);
        qDebug() << "MainWindow::noteOff - note:" << note << "activeNotes count:" << activeNotes_.size();
    }
}

void MainWindow::allNotesOff()
{
    activeNotes_.clear();
    if (auto* synth = getCurrentSynthesizer()) {
        synth->allNotesOff();
    }
    keyboardWidget_->setActiveNotes(activeNotes_);
    qDebug() << "MainWindow::allNotesOff - cleared all notes";
}

// Control event handler methods moved to mainwindow_controls.cpp

// Synthesizer management methods moved to mainwindow_synthesizer.cpp

// setupOperatorTabs method removed - now using proper tab structure

// Tracker integration methods
void MainWindow::onTrackerNoteTriggered(int note, int velocity, int channel) {
    if (auto* synth = getCurrentSynthesizer()) {
        synth->noteOn(note);
        qDebug() << "Tracker triggered note:" << note << "velocity:" << velocity << "channel:" << channel;
    }
}

void MainWindow::onTrackerNoteReleased(int note, int channel) {
    if (auto* synth = getCurrentSynthesizer()) {
        synth->noteOff(note);
        qDebug() << "Tracker released note:" << note << "channel:" << channel;
    }
}

void MainWindow::onOctaveChanged(int octave) {
    keyboardWidget_->setCurrentOctave(octave);
    octaveLabel_->setText(QString("C%1").arg(octave));
    updateKeyboardMapping(); // Update keyboard mapping for new octave
    qDebug() << "Octave changed to:" << octave;
}
