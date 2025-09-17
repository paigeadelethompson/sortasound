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

#include "window/tracker.hpp"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>


/**
 * @brief Constructor for TrackerStep
 * 
 * Creates a new tracker step with default values. A step represents a single
 * note event in a tracker pattern with note, instrument, volume, and effects.
 */
TrackerStep::TrackerStep()
    : note(0)
    , instrument(0)
    , volume(127)
    , effect1(0)
    , effect2(0)
    , effect3(0)
    , active(false)
{
}

/**
 * @brief Convert tracker step to string representation
 * 
 * Converts the tracker step data to a human-readable string format
 * that can be displayed in the tracker interface.
 * 
 * @return String representation of the step
 */
QString TrackerStep::toString() const {
    if (!active) {
        return "---";
    }
    
    QString result;
    
    if (note > 0) {
        QString noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        int octave = (note - 12) / 12;
        int noteIndex = (note - 12) % 12;
        result += noteNames[noteIndex] + QString::number(octave);
    } else {
        result += "---";
    }
    
    result += QString(" I%1").arg(instrument, 2, 10, QChar('0'));
    
    result += QString(" V%1").arg(volume, 3, 10, QChar('0'));
    
    if (effect1 > 0 || effect2 > 0 || effect3 > 0) {
        result += QString(" E%1%2%3")
            .arg(effect1, 2, 16, QChar('0'))
            .arg(effect2, 2, 16, QChar('0'))
            .arg(effect3, 2, 16, QChar('0'));
    }
    
    return result;
}

/**
 * @brief Parse tracker step from string representation
 * 
 * Parses a string representation of a tracker step and sets the
 * step data accordingly. This is used for loading step data from
 * the tracker interface.
 * 
 * @param str String representation to parse
 */
void TrackerStep::fromString(const QString& str) {
    if (str.trimmed() == "---" || str.trimmed().isEmpty()) {
        active = false;
        note = 0;
        instrument = 0;
        volume = 127;
        effect1 = effect2 = effect3 = 0;
        return;
    }
    
    active = true;
    note = 60; // Middle C
    instrument = 0;
    volume = 127;
    effect1 = effect2 = effect3 = 0;
}

/**
 * @brief Constructor for TrackerChannel
 * 
 * Creates a new tracker channel with the specified number of steps.
 * A channel represents a single track in a tracker pattern.
 * 
 * @param steps Number of steps in the channel
 */
TrackerChannel::TrackerChannel(int steps)
    : stepCount_(steps)
    , name_("Channel")
    , muted_(false)
    , solo_(false)
{
    steps_.resize(steps);
}

/**
 * @brief Get a step at a specific position
 * 
 * Returns a reference to the step at the given position in the channel.
 * If the position is invalid, returns a reference to an empty step.
 * 
 * @param position The step position (0-based)
 * @return Reference to the step
 */
TrackerStep& TrackerChannel::getStep(int position) {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        return steps_[position];
    }
    static TrackerStep emptyStep;
    return emptyStep;
}

/**
 * @brief Get a step at a specific position (const version)
 * 
 * Returns a const reference to the step at the given position in the channel.
 * If the position is invalid, returns a reference to an empty step.
 * 
 * @param position The step position (0-based)
 * @return Const reference to the step
 */
const TrackerStep& TrackerChannel::getStep(int position) const {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        return steps_[position];
    }
    static TrackerStep emptyStep;
    return emptyStep;
}

/**
 * @brief Set a step at a specific position
 * 
 * Sets the step data at the given position in the channel.
 * 
 * @param position The step position (0-based)
 * @param step The step data to set
 */
void TrackerChannel::setStep(int position, const TrackerStep& step) {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        steps_[position] = step;
    }
}

/**
 * @brief Set the number of steps in the channel
 * 
 * Changes the number of steps in the channel and resizes the step array.
 * 
 * @param steps The new number of steps
 */
void TrackerChannel::setSteps(int steps) {
    stepCount_ = steps;
    steps_.resize(steps);
}

/**
 * @brief Constructor for TrackerPattern
 * 
 * Creates a new tracker pattern with the specified number of channels and steps.
 * A pattern represents a complete musical sequence in the tracker.
 * 
 * @param channels Number of channels in the pattern
 * @param steps Number of steps in each channel
 */
TrackerPattern::TrackerPattern(int channels, int steps)
    : channelCount_(channels)
    , stepCount_(steps)
    , name_("Pattern")
{
    for (int i = 0; i < channels; ++i) {
        channels_.push_back(std::make_unique<TrackerChannel>(steps));
        channels_.back()->setName(QString("Channel %1").arg(i + 1));
    }
}

/**
 * @brief Get a channel at a specific index
 * 
 * Returns a reference to the channel at the given index in the pattern.
 * If the index is invalid, returns a reference to an empty channel.
 * 
 * @param channel The channel index (0-based)
 * @return Reference to the channel
 */
TrackerChannel& TrackerPattern::getChannel(int channel) {
    if (channel >= 0 && channel < static_cast<int>(channels_.size())) {
        return *channels_[channel];
    }
    static TrackerChannel emptyChannel(0);
    return emptyChannel;
}

/**
 * @brief Get a channel at a specific index (const version)
 * 
 * Returns a const reference to the channel at the given index in the pattern.
 * If the index is invalid, returns a reference to an empty channel.
 * 
 * @param channel The channel index (0-based)
 * @return Const reference to the channel
 */
const TrackerChannel& TrackerPattern::getChannel(int channel) const {
    if (channel >= 0 && channel < static_cast<int>(channels_.size())) {
        return *channels_[channel];
    }
    static TrackerChannel emptyChannel(0);
    return emptyChannel;
}

/**
 * @brief Clear all steps in the pattern
 * 
 * Resets all steps in all channels to their default (empty) state.
 */
void TrackerPattern::clear() {
    for (auto& channel : channels_) {
        for (int i = 0; i < stepCount_; ++i) {
            channel->setStep(i, TrackerStep());
        }
    }
}

/**
 * @brief Constructor for TrackerWidget
 * 
 * Creates a new tracker widget with all UI components and initializes
 * the first pattern. Sets up the playback timer and connects all signals.
 * 
 * @param parent Parent widget
 */
TrackerWidget::TrackerWidget(QWidget *parent)
    : QWidget(parent)
    , mainLayout_(nullptr)
    , controlsLayout_(nullptr)
    , patternControlsLayout_(nullptr)
    , playButton_(nullptr)
    , stopButton_(nullptr)
    , pauseButton_(nullptr)
    , tempoLabel_(nullptr)
    , tempoSlider_(nullptr)
    , tempoSpinBox_(nullptr)
    , patternLabel_(nullptr)
    , patternCombo_(nullptr)
    , addPatternButton_(nullptr)
    , removePatternButton_(nullptr)
    , duplicatePatternButton_(nullptr)
    , patternTable_(nullptr)
    , channelControlsGroup_(nullptr)
    , currentPattern_(0)
    , currentStep_(0)
    , playing_(false)
    , paused_(false)
    , tempo_(DEFAULT_TEMPO)
    , playbackTimer_(nullptr)
{
    setupUI();
    
    addPattern();
    
    playbackTimer_ = new QTimer(this);
    connect(playbackTimer_, &QTimer::timeout, this, &TrackerWidget::onPlaybackTimer);
}

/**
 * @brief Destructor for TrackerWidget
 * 
 * Cleans up the tracker widget by stopping playback and freeing resources.
 */
TrackerWidget::~TrackerWidget() {
    stop();
}

/**
 * @brief Setup the tracker user interface
 * 
 * Creates and configures all UI components for the tracker widget, including:
 * - Playback controls (play, pause, stop)
 * - Tempo controls
 * - Pattern management controls
 * - Channel controls (mute, solo)
 * - Pattern table for editing steps
 */
void TrackerWidget::setupUI() {
    mainLayout_ = new QVBoxLayout(this);
    
    controlsLayout_ = new QHBoxLayout();
    
    playButton_ = new QPushButton("Play", this);
    stopButton_ = new QPushButton("Stop", this);
    pauseButton_ = new QPushButton("Pause", this);
    
    playButton_->setCheckable(true);
    pauseButton_->setCheckable(true);
    
    connect(playButton_, &QPushButton::clicked, this, &TrackerWidget::onPlayClicked);
    connect(stopButton_, &QPushButton::clicked, this, &TrackerWidget::onStopClicked);
    connect(pauseButton_, &QPushButton::clicked, this, &TrackerWidget::onPauseClicked);
    
    tempoLabel_ = new QLabel("Tempo:", this);
    tempoSlider_ = new QSlider(Qt::Horizontal, this);
    tempoSlider_->setRange(MIN_TEMPO, MAX_TEMPO);
    tempoSlider_->setValue(DEFAULT_TEMPO);
    
    tempoSpinBox_ = new QSpinBox(this);
    tempoSpinBox_->setRange(MIN_TEMPO, MAX_TEMPO);
    tempoSpinBox_->setValue(DEFAULT_TEMPO);
    tempoSpinBox_->setSuffix(" BPM");
    
    connect(tempoSlider_, &QSlider::valueChanged, this, &TrackerWidget::onTempoChanged);
    connect(tempoSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this, &TrackerWidget::onTempoChanged);
    
    controlsLayout_->addWidget(playButton_);
    controlsLayout_->addWidget(pauseButton_);
    controlsLayout_->addWidget(stopButton_);
    controlsLayout_->addWidget(tempoLabel_);
    controlsLayout_->addWidget(tempoSlider_);
    controlsLayout_->addWidget(tempoSpinBox_);
    controlsLayout_->addStretch();
    
    patternControlsLayout_ = new QHBoxLayout();
    
    patternLabel_ = new QLabel("Pattern:", this);
    patternCombo_ = new QComboBox(this);
    addPatternButton_ = new QPushButton("Add", this);
    removePatternButton_ = new QPushButton("Remove", this);
    duplicatePatternButton_ = new QPushButton("Duplicate", this);
    
    connect(patternCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TrackerWidget::onPatternChanged);
    connect(addPatternButton_, &QPushButton::clicked, this, &TrackerWidget::addPattern);
    connect(removePatternButton_, &QPushButton::clicked, this, &TrackerWidget::removePattern);
    connect(duplicatePatternButton_, &QPushButton::clicked, this, &TrackerWidget::duplicatePattern);
    
    patternControlsLayout_->addWidget(patternLabel_);
    patternControlsLayout_->addWidget(patternCombo_);
    patternControlsLayout_->addWidget(addPatternButton_);
    patternControlsLayout_->addWidget(removePatternButton_);
    patternControlsLayout_->addWidget(duplicatePatternButton_);
    patternControlsLayout_->addStretch();
    
    channelControlsGroup_ = new QGroupBox("Channel Controls", this);
    QHBoxLayout *channelControlsLayout = new QHBoxLayout(channelControlsGroup_);
    
    for (int i = 0; i < DEFAULT_CHANNELS; ++i) {
        QVBoxLayout *channelLayout = new QVBoxLayout();
        
        QLabel *channelLabel = new QLabel(QString("Ch %1").arg(i + 1), this);
        channelLabel->setAlignment(Qt::AlignCenter);
        
        QCheckBox *muteCheckbox = new QCheckBox("Mute", this);
        QCheckBox *soloCheckbox = new QCheckBox("Solo", this);
        
        channelMuteCheckboxes_.push_back(muteCheckbox);
        channelSoloCheckboxes_.push_back(soloCheckbox);
        
        connect(muteCheckbox, &QCheckBox::toggled, [this, i](bool checked) {
            onChannelMuteToggled(i, checked);
        });
        connect(soloCheckbox, &QCheckBox::toggled, [this, i](bool checked) {
            onChannelSoloToggled(i, checked);
        });
        
        channelLayout->addWidget(channelLabel);
        channelLayout->addWidget(muteCheckbox);
        channelLayout->addWidget(soloCheckbox);
        
        channelControlsLayout->addLayout(channelLayout);
    }
    
    setupPatternTable();
    
    mainLayout_->addLayout(controlsLayout_);
    mainLayout_->addLayout(patternControlsLayout_);
    mainLayout_->addWidget(channelControlsGroup_);
    mainLayout_->addWidget(patternTable_);
}

/**
 * @brief Setup the pattern table widget
 * 
 * Creates and configures the table widget that displays and allows editing
 * of tracker steps. Sets up columns for each channel and rows for each step.
 */
void TrackerWidget::setupPatternTable() {
    patternTable_ = new QTableWidget(this);
    
    QStringList headers;
    headers << "Step";
    for (int i = 0; i < DEFAULT_CHANNELS; ++i) {
        headers << QString("Ch %1").arg(i + 1);
    }
    patternTable_->setColumnCount(headers.size());
    patternTable_->setHorizontalHeaderLabels(headers);
    
    patternTable_->setRowCount(DEFAULT_STEPS);
    for (int i = 0; i < DEFAULT_STEPS; ++i) {
        QTableWidgetItem *stepItem = new QTableWidgetItem(QString::number(i + 1));
        stepItem->setFlags(stepItem->flags() & ~Qt::ItemIsEditable);
        patternTable_->setItem(i, 0, stepItem);
    }
    
    patternTable_->setAlternatingRowColors(true);
    patternTable_->setSelectionBehavior(QAbstractItemView::SelectItems);
    patternTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    patternTable_->verticalHeader()->setVisible(false);
    patternTable_->horizontalHeader()->setStretchLastSection(true);
    
    connect(patternTable_, &QTableWidget::cellChanged, this, &TrackerWidget::onStepChanged);
    
    patternTable_->setColumnWidth(0, 50); // Step column
    for (int i = 1; i < headers.size(); ++i) {
        patternTable_->setColumnWidth(i, 120); // Channel columns
    }
}

/**
 * @brief Update the pattern table display
 * 
 * Refreshes the pattern table to show the current pattern data.
 * Updates all step cells with their current values and colors.
 */
void TrackerWidget::updatePatternTable() {
    if (currentPattern_ < 0 || currentPattern_ >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    TrackerPattern& pattern = *patterns_[currentPattern_];
    
    for (int step = 0; step < pattern.getSteps(); ++step) {
        for (int channel = 0; channel < pattern.getChannels(); ++channel) {
            const TrackerStep& trackerStep = pattern.getChannel(channel).getStep(step);
            QTableWidgetItem *item = patternTable_->item(step, channel + 1);
            
            if (!item) {
                item = new QTableWidgetItem();
                patternTable_->setItem(step, channel + 1, item);
            }
            
            item->setText(trackerStep.toString());
            
            if (trackerStep.active) {
                item->setBackground(QColor(200, 255, 200)); // Light green for active steps
            } else {
                item->setBackground(QColor(255, 255, 255)); // White for inactive steps
            }
        }
    }
}

/**
 * @brief Update the playback position display
 * 
 * Updates the visual indication of the current playback position in the
 * pattern table. Highlights the current step being played.
 */
void TrackerWidget::updatePlaybackPosition() {
    if (!playing_ && !paused_) {
        clearPlaybackPosition();
        return;
    }
    
    for (int i = 0; i < patternTable_->rowCount(); ++i) {
        for (int j = 0; j < patternTable_->columnCount(); ++j) {
            QTableWidgetItem *item = patternTable_->item(i, j);
            if (item) {
                if (i == currentStep_) {
                    item->setBackground(QColor(255, 255, 0)); // Yellow for current step
                } else {
                    if (currentPattern_ < static_cast<int>(patterns_.size())) {
                        TrackerPattern& pattern = *patterns_[currentPattern_];
                        if (j > 0 && j - 1 < pattern.getChannels()) {
                            const TrackerStep& step = pattern.getChannel(j - 1).getStep(i);
                            if (step.active) {
                                item->setBackground(QColor(200, 255, 200));
                            } else {
                                item->setBackground(QColor(255, 255, 255));
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Trigger a step for playback
 * 
 * Processes a single step in the current pattern and triggers any notes
 * that should be played at that step. Respects mute and solo settings.
 * 
 * @param step The step number to trigger
 */
void TrackerWidget::triggerStep(int step) {
    if (currentPattern_ < 0 || currentPattern_ >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    TrackerPattern& pattern = *patterns_[currentPattern_];
    
    for (int channel = 0; channel < pattern.getChannels(); ++channel) {
        TrackerChannel& trackerChannel = pattern.getChannel(channel);
        
        if (trackerChannel.isMuted()) {
            continue;
        }
        
        bool hasSolo = false;
        for (int c = 0; c < pattern.getChannels(); ++c) {
            if (pattern.getChannel(c).isSolo()) {
                hasSolo = true;
                break;
            }
        }
        
        if (hasSolo && !trackerChannel.isSolo()) {
            continue;
        }
        
        const TrackerStep& trackerStep = trackerChannel.getStep(step);
        if (trackerStep.active && trackerStep.note > 0) {
            emit noteTriggered(trackerStep.note, trackerStep.volume, channel);
        }
    }
}

/**
 * @brief Clear the playback position display
 * 
 * Removes the visual indication of the current playback position
 * and resets the pattern table colors to their default state.
 */
void TrackerWidget::clearPlaybackPosition() {
    updatePatternTable(); // This will reset all colors
}

/**
 * @brief Add a new pattern
 * 
 * Creates a new empty pattern and adds it to the pattern list.
 * Updates the pattern combo box and sets the new pattern as current.
 */
void TrackerWidget::addPattern() {
    auto pattern = std::make_unique<TrackerPattern>(DEFAULT_CHANNELS, DEFAULT_STEPS);
    pattern->setName(QString("Pattern %1").arg(patterns_.size() + 1));
    patterns_.push_back(std::move(pattern));
    
    patternCombo_->addItem(QString("Pattern %1").arg(patterns_.size()));
    patternCombo_->setCurrentIndex(patterns_.size() - 1);
}

/**
 * @brief Remove a pattern
 * 
 * Removes the pattern at the specified index from the pattern list.
 * Prevents removing the last remaining pattern.
 * 
 * @param index The index of the pattern to remove
 */
void TrackerWidget::removePattern(int index) {
    if (index < 0 || index >= static_cast<int>(patterns_.size()) || patterns_.size() <= 1) {
        return; // Don't remove the last pattern
    }
    
    patterns_.erase(patterns_.begin() + index);
    patternCombo_->removeItem(index);
    
    if (currentPattern_ >= static_cast<int>(patterns_.size())) {
        currentPattern_ = patterns_.size() - 1;
    }
    
    patternCombo_->setCurrentIndex(currentPattern_);
    updatePatternTable();
}

/**
 * @brief Duplicate a pattern
 * 
 * Creates a copy of the pattern at the specified index and adds it
 * to the pattern list. The new pattern is added at the end.
 * 
 * @param index The index of the pattern to duplicate
 */
void TrackerWidget::duplicatePattern(int index) {
    if (index < 0 || index >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    auto newPattern = std::make_unique<TrackerPattern>(DEFAULT_CHANNELS, DEFAULT_STEPS);
    newPattern->setName(QString("Pattern %1").arg(patterns_.size() + 1));
    
    TrackerPattern& sourcePattern = *patterns_[index];
    for (int channel = 0; channel < sourcePattern.getChannels(); ++channel) {
        for (int step = 0; step < sourcePattern.getSteps(); ++step) {
            newPattern->getChannel(channel).setStep(step, sourcePattern.getChannel(channel).getStep(step));
        }
    }
    
    patterns_.push_back(std::move(newPattern));
    patternCombo_->addItem(QString("Pattern %1").arg(patterns_.size()));
}

/**
 * @brief Set the current pattern
 * 
 * Changes the currently active pattern and updates the display.
 * 
 * @param index The index of the pattern to set as current
 */
void TrackerWidget::setCurrentPattern(int index) {
    if (index >= 0 && index < static_cast<int>(patterns_.size())) {
        currentPattern_ = index;
        patternCombo_->setCurrentIndex(index);
        updatePatternTable();
    }
}

/**
 * @brief Start playback
 * 
 * Starts playing the current pattern from the beginning. Sets up the
 * playback timer and updates the UI state.
 */
void TrackerWidget::play() {
    if (playing_) {
        return;
    }
    
    playing_ = true;
    paused_ = false;
    currentStep_ = 0;
    
    playButton_->setChecked(true);
    pauseButton_->setChecked(false);
    
    int interval = 60000 / (tempo_ * 4); // 16th notes
    playbackTimer_->start(interval);
    
    updatePlaybackPosition();
}

/**
 * @brief Stop playback
 * 
 * Stops the current playback and resets the position to the beginning.
 * Updates the UI state and clears the playback position display.
 */
void TrackerWidget::stop() {
    playing_ = false;
    paused_ = false;
    currentStep_ = 0;
    
    playbackTimer_->stop();
    
    playButton_->setChecked(false);
    pauseButton_->setChecked(false);
    
    clearPlaybackPosition();
}

/**
 * @brief Pause or resume playback
 * 
 * Toggles the pause state of the current playback. If playing, pauses;
 * if paused, resumes from the current position.
 */
void TrackerWidget::pause() {
    if (!playing_) {
        return;
    }
    
    paused_ = !paused_;
    
    if (paused_) {
        playbackTimer_->stop();
        pauseButton_->setChecked(true);
    } else {
        int interval = 60000 / (tempo_ * 4);
        playbackTimer_->start(interval);
        pauseButton_->setChecked(false);
    }
}

/**
 * @brief Set the playback tempo
 * 
 * Changes the playback tempo in beats per minute and updates the
 * playback timer interval accordingly.
 * 
 * @param bpm The new tempo in beats per minute
 */
void TrackerWidget::setTempo(int bpm) {
    tempo_ = qBound(MIN_TEMPO, bpm, MAX_TEMPO);
    tempoSlider_->setValue(tempo_);
    tempoSpinBox_->setValue(tempo_);
    
    if (playing_ && !paused_) {
        int interval = 60000 / (tempo_ * 4);
        playbackTimer_->start(interval);
    }
    
    emit tempoChanged(tempo_);
}

/**
 * @brief Set the number of steps in patterns
 * 
 * Changes the number of steps in all patterns. This is a placeholder
 * for future functionality.
 * 
 * @param steps The new number of steps
 */
void TrackerWidget::setPatternSteps(int steps) {
    Q_UNUSED(steps);
}

/**
 * @brief Set the number of channels in patterns
 * 
 * Changes the number of channels in all patterns. This is a placeholder
 * for future functionality.
 * 
 * @param channels The new number of channels
 */
void TrackerWidget::setPatternChannels(int channels) {
    Q_UNUSED(channels);
}

/**
 * @brief Handle play button clicks
 * 
 * Called when the play button is clicked. Toggles between play and pause states.
 */
void TrackerWidget::onPlayClicked() {
    if (playing_) {
        pause();
    } else {
        play();
    }
}

void TrackerWidget::onStopClicked() {
    stop();
}

void TrackerWidget::onPauseClicked() {
    pause();
}

void TrackerWidget::onTempoChanged(int value) {
    setTempo(value);
}

void TrackerWidget::onPatternChanged(int index) {
    setCurrentPattern(index);
}

void TrackerWidget::onStepChanged(int row, int column) {
    if (column == 0 || currentPattern_ < 0 || currentPattern_ >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    int channel = column - 1;
    QTableWidgetItem *item = patternTable_->item(row, column);
    if (!item) {
        return;
    }
    
    TrackerStep step;
    step.fromString(item->text());
    
    TrackerPattern& pattern = *patterns_[currentPattern_];
    pattern.getChannel(channel).setStep(row, step);
    
    if (step.active) {
        item->setBackground(QColor(200, 255, 200));
    } else {
        item->setBackground(QColor(255, 255, 255));
    }
}

void TrackerWidget::onChannelMuteToggled(int channel, bool muted) {
    if (currentPattern_ >= 0 && currentPattern_ < static_cast<int>(patterns_.size())) {
        patterns_[currentPattern_]->getChannel(channel).setMuted(muted);
    }
}

void TrackerWidget::onChannelSoloToggled(int channel, bool solo) {
    if (currentPattern_ >= 0 && currentPattern_ < static_cast<int>(patterns_.size())) {
        patterns_[currentPattern_]->getChannel(channel).setSolo(solo);
    }
}

/**
 * @brief Handle playback timer events
 * 
 * Called periodically by the playback timer to advance the playback position
 * and trigger the current step. This is the main playback loop.
 */
void TrackerWidget::onPlaybackTimer() {
    if (!playing_ || paused_) {
        return;
    }
    
    triggerStep(currentStep_);
    
    currentStep_++;
    if (currentPattern_ < static_cast<int>(patterns_.size())) {
        TrackerPattern& pattern = *patterns_[currentPattern_];
        if (currentStep_ >= pattern.getSteps()) {
            currentStep_ = 0; // Loop
        }
    }
    
    updatePlaybackPosition();
}
