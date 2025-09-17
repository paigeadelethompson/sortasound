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
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QDoubleSpinBox>

/**
 * @brief Constructor for MainWindow
 * 
 * Creates the main application window with all UI components, synthesizer
 * instances, and keyboard mapping. Initializes the first synthesizer
 * and starts its audio thread.
 * 
 * @param parent Parent widget
 */
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
    , synthesizerTabWidget_(nullptr)
    , currentChannel_(0)
{
    setupUI();
    setupKeyboardMapping();
    
    synthesizers_.push_back(std::make_unique<toybasic::FMSynthesizer>());
    synthesizers_[0]->startAudioThread();
}

/**
 * @brief Destructor for MainWindow
 * 
 * Cleans up the main window by stopping all synthesizer audio threads
 * before the window is destroyed.
 */
MainWindow::~MainWindow()
{
    for (auto& synth : synthesizers_) {
        synth->stopAudioThread();
    }
}


/**
 * @brief Setup keyboard mapping for computer keyboard input
 * 
 * Connects the keyboard widget signals and sets up the mapping between
 * computer keyboard keys and MIDI notes for the current octave.
 */
void MainWindow::setupKeyboardMapping()
{
    connect(keyboardWidget_, &KeyboardWidget::keyPressed, this, &MainWindow::onKeyboardKeyPressed);
    connect(keyboardWidget_, &KeyboardWidget::keyReleased, this, &MainWindow::onKeyboardKeyReleased);
    
    updateKeyboardMapping();
}

/**
 * @brief Handle keyboard key press events
 * 
 * Called when a key is pressed on the virtual keyboard widget.
 * Triggers a note on event on the current synthesizer.
 * 
 * @param note The MIDI note number that was pressed
 */
void MainWindow::onKeyboardKeyPressed(int note)
{
    if (currentSynthesizerIndex_ < synthesizers_.size()) {
        synthesizers_[currentSynthesizerIndex_]->noteOn(note, 1.0);
    }
}

/**
 * @brief Handle keyboard key release events
 * 
 * Called when a key is released on the virtual keyboard widget.
 * Triggers a note off event on the current synthesizer.
 * 
 * @param note The MIDI note number that was released
 */
void MainWindow::onKeyboardKeyReleased(int note)
{
    if (currentSynthesizerIndex_ < synthesizers_.size()) {
        synthesizers_[currentSynthesizerIndex_]->noteOff(note);
    }
}

/**
 * @brief Handle synthesizer tab close requests
 * 
 * Called when the user requests to close a synthesizer tab.
 * Prevents closing the last remaining tab and properly cleans up
 * the synthesizer instance.
 * 
 * @param index The index of the tab to close
 */
void MainWindow::onSynthesizerTabCloseRequested(int index)
{
    if (synthesizerTabWidget_->count() <= 1) {
        return;
    }
    
    if (index < synthesizers_.size()) {
        synthesizers_.erase(synthesizers_.begin() + index);
    }
    
    synthesizerTabWidget_->removeTab(index);
    
    if (currentSynthesizerIndex_ >= synthesizers_.size()) {
        currentSynthesizerIndex_ = synthesizers_.size() - 1;
    }
    
    updateSynthesizerTabLabels();
}

/**
 * @brief Add a new synthesizer tab
 * 
 * Creates a new synthesizer instance and adds a new tab to the synthesizer
 * tab widget. The new synthesizer is automatically started.
 */
void MainWindow::onAddSynthesizerTab()
{
    auto newSynthesizer = std::make_unique<toybasic::FMSynthesizer>();
    synthesizers_.push_back(std::move(newSynthesizer));
    
    QWidget *newSynthTab = createSynthesizerTab();
    
    int newIndex = synthesizerTabWidget_->count();
    synthesizerTabWidget_->addTab(newSynthTab, QString("Synth %1").arg(newIndex + 1));
    
    synthesizerTabWidget_->setCurrentIndex(newIndex);
    
    currentSynthesizerIndex_ = newIndex;
    
    updateSynthesizerTabLabels();
}

/**
 * @brief Handle synthesizer tab changes
 * 
 * Called when the user switches between synthesizer tabs. Updates the
 * current synthesizer index to match the active tab.
 * 
 * @param index The index of the newly active tab
 */
void MainWindow::onSynthesizerTabChanged(int index)
{
    currentSynthesizerIndex_ = index;
}

/**
 * @brief Create a new synthesizer tab widget
 * 
 * Creates a new tab widget containing a keyboard widget and controls layout.
 * This is used when adding new synthesizer instances.
 * 
 * @return Pointer to the new tab widget
 */
QWidget* MainWindow::createSynthesizerTab()
{
    QWidget *newTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(newTab);
    
    layout->addWidget(keyboardWidget_);
    
    layout->addLayout(controlsLayout_);
    
    QTabWidget *advancedTabWidget = new QTabWidget();
    layout->addWidget(advancedTabWidget);
    
    return newTab;
}

/**
 * @brief Update synthesizer tab labels
 * 
 * Updates the text labels for all synthesizer tabs to reflect their
 * current numbering (Synth 1, Synth 2, etc.).
 */
void MainWindow::updateSynthesizerTabLabels()
{
    for (int i = 0; i < synthesizerTabWidget_->count(); ++i) {
        synthesizerTabWidget_->setTabText(i, QString("Synth %1").arg(i + 1));
    }
}

/**
 * @brief Handle octave selection changes
 * 
 * Called when the user changes the octave selection. Updates the keyboard
 * widget to display the new octave range and updates the octave label.
 * 
 * @param octave The new octave value
 */
void MainWindow::onOctaveChanged(int octave)
{
    keyboardWidget_->setCurrentOctave(octave);
    
    octaveLabel_->setText(QString("C%1-C%2").arg(octave).arg(octave + 3));
    
    updateKeyboardMapping();
}

/**
 * @brief Update the keyboard mapping for the current octave
 * 
 * Updates the mapping between computer keyboard keys and MIDI notes
 * based on the current octave setting. This allows the user to play
 * different octaves using their computer keyboard.
 */
void MainWindow::updateKeyboardMapping()
{
    int currentOctave = keyboardWidget_->getCurrentOctave();
    int octaveStart = currentOctave * 12;
    
    keyToNoteMap_.clear();
    keyToNoteMap_[Qt::Key_Q] = octaveStart;
    keyToNoteMap_[Qt::Key_W] = octaveStart + 1;
    keyToNoteMap_[Qt::Key_E] = octaveStart + 2;
    keyToNoteMap_[Qt::Key_R] = octaveStart + 3;
    keyToNoteMap_[Qt::Key_T] = octaveStart + 4;
    keyToNoteMap_[Qt::Key_Y] = octaveStart + 5;
    keyToNoteMap_[Qt::Key_U] = octaveStart + 6;
    keyToNoteMap_[Qt::Key_I] = octaveStart + 7;
    keyToNoteMap_[Qt::Key_O] = octaveStart + 8;
    keyToNoteMap_[Qt::Key_P] = octaveStart + 9;
    keyToNoteMap_[Qt::Key_BracketLeft] = octaveStart + 10;
    keyToNoteMap_[Qt::Key_BracketRight] = octaveStart + 11;
    
    keyToNoteMap_[Qt::Key_A] = octaveStart + 12;
    keyToNoteMap_[Qt::Key_S] = octaveStart + 13;
    keyToNoteMap_[Qt::Key_D] = octaveStart + 14;
    keyToNoteMap_[Qt::Key_F] = octaveStart + 15;
    keyToNoteMap_[Qt::Key_G] = octaveStart + 16;
    keyToNoteMap_[Qt::Key_H] = octaveStart + 17;
    keyToNoteMap_[Qt::Key_J] = octaveStart + 18;
    keyToNoteMap_[Qt::Key_K] = octaveStart + 19;
    keyToNoteMap_[Qt::Key_L] = octaveStart + 20;
    keyToNoteMap_[Qt::Key_Semicolon] = octaveStart + 21;
    keyToNoteMap_[Qt::Key_QuoteLeft] = octaveStart + 22;
    keyToNoteMap_[Qt::Key_QuoteDbl] = octaveStart + 23;
    
    keyToNoteMap_[Qt::Key_Z] = octaveStart + 24;
    keyToNoteMap_[Qt::Key_X] = octaveStart + 25;
    keyToNoteMap_[Qt::Key_C] = octaveStart + 26;
    keyToNoteMap_[Qt::Key_V] = octaveStart + 27;
    keyToNoteMap_[Qt::Key_B] = octaveStart + 28;
    keyToNoteMap_[Qt::Key_N] = octaveStart + 29;
    keyToNoteMap_[Qt::Key_M] = octaveStart + 30;
    keyToNoteMap_[Qt::Key_Comma] = octaveStart + 31;
    keyToNoteMap_[Qt::Key_Period] = octaveStart + 32;
    keyToNoteMap_[Qt::Key_Slash] = octaveStart + 33;
    
    keyboardWidget_->setKeyMapping(keyToNoteMap_);
}

/**
 * @brief Handle computer keyboard key press events
 * 
 * Called when a key is pressed on the computer keyboard. Converts the
 * key to a MIDI note and triggers a note on event if the key is mapped.
 * 
 * @param event The key press event
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    
    int note = keyToNote(static_cast<Qt::Key>(event->key()));
    if (note != -1) {
        noteOn(note);
    }
    
    QMainWindow::keyPressEvent(event);
}

/**
 * @brief Handle computer keyboard key release events
 * 
 * Called when a key is released on the computer keyboard. Converts the
 * key to a MIDI note and triggers a note off event if the key is mapped.
 * 
 * @param event The key release event
 */
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    
    int note = keyToNote(static_cast<Qt::Key>(event->key()));
    if (note != -1) {
        noteOff(note);
    }
    
    QMainWindow::keyReleaseEvent(event);
}

/**
 * @brief Handle focus in events
 * 
 * Called when the window gains focus. This is used for keyboard input handling.
 * 
 * @param event The focus in event
 */
void MainWindow::focusInEvent(QFocusEvent *event)
{
    QMainWindow::focusInEvent(event);
}

/**
 * @brief Handle focus out events
 * 
 * Called when the window loses focus. This is used for keyboard input handling.
 * 
 * @param event The focus out event
 */
void MainWindow::focusOutEvent(QFocusEvent *event)
{
    QMainWindow::focusOutEvent(event);
}

/**
 * @brief Convert a Qt key to a MIDI note number
 * 
 * Looks up the MIDI note number corresponding to the given Qt key code.
 * Returns -1 if the key is not mapped to a note.
 * 
 * @param key The Qt key code
 * @return The corresponding MIDI note number, or -1 if not mapped
 */
int MainWindow::keyToNote(Qt::Key key) const
{
    auto it = keyToNoteMap_.find(key);
    int note = (it != keyToNoteMap_.end()) ? it->second : -1;
    return note;
}

/**
 * @brief Trigger a note on event
 * 
 * Starts playing a note on the current synthesizer and updates the
 * active notes display on the keyboard widget.
 * 
 * @param note The MIDI note number to play
 */
void MainWindow::noteOn(int note)
{
    if (activeNotes_.find(note) == activeNotes_.end()) {
        activeNotes_.insert(note);
        if (auto* synth = getCurrentSynthesizer()) {
            synth->noteOn(note);
        }
        keyboardWidget_->setActiveNotes(activeNotes_);
    }
}

/**
 * @brief Trigger a note off event
 * 
 * Stops playing a note on the current synthesizer and updates the
 * active notes display on the keyboard widget.
 * 
 * @param note The MIDI note number to stop
 */
void MainWindow::noteOff(int note)
{
    auto it = activeNotes_.find(note);
    if (it != activeNotes_.end()) {
        activeNotes_.erase(it);
        if (auto* synth = getCurrentSynthesizer()) {
            synth->noteOff(note);
        }
        keyboardWidget_->setActiveNotes(activeNotes_);
    }
}

/**
 * @brief Stop all currently playing notes
 * 
 * Stops all notes on the current synthesizer and clears the active
 * notes display on the keyboard widget.
 */
void MainWindow::allNotesOff()
{
    activeNotes_.clear();
    if (auto* synth = getCurrentSynthesizer()) {
        synth->allNotesOff();
    }
    keyboardWidget_->setActiveNotes(activeNotes_);
}

/**
 * @brief Handle tracker note trigger events
 * 
 * Called when the tracker widget triggers a note. Starts playing
 * the note on the current synthesizer.
 * 
 * @param note The MIDI note number to play
 * @param velocity The note velocity (0-127)
 * @param channel The MIDI channel
 */
void MainWindow::onTrackerNoteTriggered(int note, int velocity, int channel) {
    if (auto* synth = getCurrentSynthesizer()) {
        synth->noteOn(note);
    }
}

/**
 * @brief Handle tracker note release events
 * 
 * Called when the tracker widget releases a note. Stops playing
 * the note on the current synthesizer.
 * 
 * @param note The MIDI note number to stop
 * @param channel The MIDI channel
 */
void MainWindow::onTrackerNoteReleased(int note, int channel) {
    if (auto* synth = getCurrentSynthesizer()) {
        synth->noteOff(note);
    }
}

