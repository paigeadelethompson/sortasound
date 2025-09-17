#include "window/tracker.hpp"
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

// TrackerStep implementation
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

QString TrackerStep::toString() const {
    if (!active) {
        return "---";
    }
    
    QString result;
    
    // Note
    if (note > 0) {
        QString noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        int octave = (note - 12) / 12;
        int noteIndex = (note - 12) % 12;
        result += noteNames[noteIndex] + QString::number(octave);
    } else {
        result += "---";
    }
    
    // Instrument
    result += QString(" I%1").arg(instrument, 2, 10, QChar('0'));
    
    // Volume
    result += QString(" V%1").arg(volume, 3, 10, QChar('0'));
    
    // Effects
    if (effect1 > 0 || effect2 > 0 || effect3 > 0) {
        result += QString(" E%1%2%3")
            .arg(effect1, 2, 16, QChar('0'))
            .arg(effect2, 2, 16, QChar('0'))
            .arg(effect3, 2, 16, QChar('0'));
    }
    
    return result;
}

void TrackerStep::fromString(const QString& str) {
    // Simple parser - in a real implementation this would be more robust
    if (str.trimmed() == "---" || str.trimmed().isEmpty()) {
        active = false;
        note = 0;
        instrument = 0;
        volume = 127;
        effect1 = effect2 = effect3 = 0;
        return;
    }
    
    active = true;
    // For now, just set basic values - full parsing would be more complex
    note = 60; // Middle C
    instrument = 0;
    volume = 127;
    effect1 = effect2 = effect3 = 0;
}

// TrackerChannel implementation
TrackerChannel::TrackerChannel(int steps)
    : stepCount_(steps)
    , name_("Channel")
    , muted_(false)
    , solo_(false)
{
    steps_.resize(steps);
}

TrackerStep& TrackerChannel::getStep(int position) {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        return steps_[position];
    }
    static TrackerStep emptyStep;
    return emptyStep;
}

const TrackerStep& TrackerChannel::getStep(int position) const {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        return steps_[position];
    }
    static TrackerStep emptyStep;
    return emptyStep;
}

void TrackerChannel::setStep(int position, const TrackerStep& step) {
    if (position >= 0 && position < static_cast<int>(steps_.size())) {
        steps_[position] = step;
    }
}

void TrackerChannel::setSteps(int steps) {
    stepCount_ = steps;
    steps_.resize(steps);
}

// TrackerPattern implementation
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

TrackerChannel& TrackerPattern::getChannel(int channel) {
    if (channel >= 0 && channel < static_cast<int>(channels_.size())) {
        return *channels_[channel];
    }
    static TrackerChannel emptyChannel(0);
    return emptyChannel;
}

const TrackerChannel& TrackerPattern::getChannel(int channel) const {
    if (channel >= 0 && channel < static_cast<int>(channels_.size())) {
        return *channels_[channel];
    }
    static TrackerChannel emptyChannel(0);
    return emptyChannel;
}

void TrackerPattern::clear() {
    for (auto& channel : channels_) {
        for (int i = 0; i < stepCount_; ++i) {
            channel->setStep(i, TrackerStep());
        }
    }
}

// TrackerWidget implementation
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
    
    // Create initial pattern
    addPattern();
    
    // Setup playback timer
    playbackTimer_ = new QTimer(this);
    connect(playbackTimer_, &QTimer::timeout, this, &TrackerWidget::onPlaybackTimer);
}

TrackerWidget::~TrackerWidget() {
    stop();
}

void TrackerWidget::setupUI() {
    mainLayout_ = new QVBoxLayout(this);
    
    // Controls layout
    controlsLayout_ = new QHBoxLayout();
    
    // Playback controls
    playButton_ = new QPushButton("Play", this);
    stopButton_ = new QPushButton("Stop", this);
    pauseButton_ = new QPushButton("Pause", this);
    
    playButton_->setCheckable(true);
    pauseButton_->setCheckable(true);
    
    connect(playButton_, &QPushButton::clicked, this, &TrackerWidget::onPlayClicked);
    connect(stopButton_, &QPushButton::clicked, this, &TrackerWidget::onStopClicked);
    connect(pauseButton_, &QPushButton::clicked, this, &TrackerWidget::onPauseClicked);
    
    // Tempo controls
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
    
    // Pattern controls
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
    
    // Channel controls
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
    
    // Pattern table
    setupPatternTable();
    
    // Add to main layout
    mainLayout_->addLayout(controlsLayout_);
    mainLayout_->addLayout(patternControlsLayout_);
    mainLayout_->addWidget(channelControlsGroup_);
    mainLayout_->addWidget(patternTable_);
}

void TrackerWidget::setupPatternTable() {
    patternTable_ = new QTableWidget(this);
    
    // Set up table headers
    QStringList headers;
    headers << "Step";
    for (int i = 0; i < DEFAULT_CHANNELS; ++i) {
        headers << QString("Ch %1").arg(i + 1);
    }
    patternTable_->setColumnCount(headers.size());
    patternTable_->setHorizontalHeaderLabels(headers);
    
    // Set up rows
    patternTable_->setRowCount(DEFAULT_STEPS);
    for (int i = 0; i < DEFAULT_STEPS; ++i) {
        QTableWidgetItem *stepItem = new QTableWidgetItem(QString::number(i + 1));
        stepItem->setFlags(stepItem->flags() & ~Qt::ItemIsEditable);
        patternTable_->setItem(i, 0, stepItem);
    }
    
    // Configure table
    patternTable_->setAlternatingRowColors(true);
    patternTable_->setSelectionBehavior(QAbstractItemView::SelectItems);
    patternTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    patternTable_->verticalHeader()->setVisible(false);
    patternTable_->horizontalHeader()->setStretchLastSection(true);
    
    // Connect signals
    connect(patternTable_, &QTableWidget::cellChanged, this, &TrackerWidget::onStepChanged);
    
    // Set column widths
    patternTable_->setColumnWidth(0, 50); // Step column
    for (int i = 1; i < headers.size(); ++i) {
        patternTable_->setColumnWidth(i, 120); // Channel columns
    }
}

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
            
            // Color coding
            if (trackerStep.active) {
                item->setBackground(QColor(200, 255, 200)); // Light green for active steps
            } else {
                item->setBackground(QColor(255, 255, 255)); // White for inactive steps
            }
        }
    }
}

void TrackerWidget::updatePlaybackPosition() {
    if (!playing_ && !paused_) {
        clearPlaybackPosition();
        return;
    }
    
    // Highlight current step
    for (int i = 0; i < patternTable_->rowCount(); ++i) {
        for (int j = 0; j < patternTable_->columnCount(); ++j) {
            QTableWidgetItem *item = patternTable_->item(i, j);
            if (item) {
                if (i == currentStep_) {
                    item->setBackground(QColor(255, 255, 0)); // Yellow for current step
                } else {
                    // Reset to original color
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

void TrackerWidget::triggerStep(int step) {
    if (currentPattern_ < 0 || currentPattern_ >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    TrackerPattern& pattern = *patterns_[currentPattern_];
    
    for (int channel = 0; channel < pattern.getChannels(); ++channel) {
        TrackerChannel& trackerChannel = pattern.getChannel(channel);
        
        // Skip muted channels
        if (trackerChannel.isMuted()) {
            continue;
        }
        
        // Check for solo - if any channel is solo, only play solo channels
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

void TrackerWidget::clearPlaybackPosition() {
    updatePatternTable(); // This will reset all colors
}

// Public methods
void TrackerWidget::addPattern() {
    auto pattern = std::make_unique<TrackerPattern>(DEFAULT_CHANNELS, DEFAULT_STEPS);
    pattern->setName(QString("Pattern %1").arg(patterns_.size() + 1));
    patterns_.push_back(std::move(pattern));
    
    patternCombo_->addItem(QString("Pattern %1").arg(patterns_.size()));
    patternCombo_->setCurrentIndex(patterns_.size() - 1);
}

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

void TrackerWidget::duplicatePattern(int index) {
    if (index < 0 || index >= static_cast<int>(patterns_.size())) {
        return;
    }
    
    auto newPattern = std::make_unique<TrackerPattern>(DEFAULT_CHANNELS, DEFAULT_STEPS);
    newPattern->setName(QString("Pattern %1").arg(patterns_.size() + 1));
    
    // Copy data from source pattern
    TrackerPattern& sourcePattern = *patterns_[index];
    for (int channel = 0; channel < sourcePattern.getChannels(); ++channel) {
        for (int step = 0; step < sourcePattern.getSteps(); ++step) {
            newPattern->getChannel(channel).setStep(step, sourcePattern.getChannel(channel).getStep(step));
        }
    }
    
    patterns_.push_back(std::move(newPattern));
    patternCombo_->addItem(QString("Pattern %1").arg(patterns_.size()));
}

void TrackerWidget::setCurrentPattern(int index) {
    if (index >= 0 && index < static_cast<int>(patterns_.size())) {
        currentPattern_ = index;
        patternCombo_->setCurrentIndex(index);
        updatePatternTable();
    }
}

void TrackerWidget::play() {
    if (playing_) {
        return;
    }
    
    playing_ = true;
    paused_ = false;
    currentStep_ = 0;
    
    playButton_->setChecked(true);
    pauseButton_->setChecked(false);
    
    // Calculate timer interval based on tempo
    int interval = 60000 / (tempo_ * 4); // 16th notes
    playbackTimer_->start(interval);
    
    updatePlaybackPosition();
}

void TrackerWidget::stop() {
    playing_ = false;
    paused_ = false;
    currentStep_ = 0;
    
    playbackTimer_->stop();
    
    playButton_->setChecked(false);
    pauseButton_->setChecked(false);
    
    clearPlaybackPosition();
}

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

void TrackerWidget::setTempo(int bpm) {
    tempo_ = qBound(MIN_TEMPO, bpm, MAX_TEMPO);
    tempoSlider_->setValue(tempo_);
    tempoSpinBox_->setValue(tempo_);
    
    // Update timer if playing
    if (playing_ && !paused_) {
        int interval = 60000 / (tempo_ * 4);
        playbackTimer_->start(interval);
    }
    
    emit tempoChanged(tempo_);
}

void TrackerWidget::setPatternSteps(int steps) {
    // This would require resizing all patterns - implementation left for later
    Q_UNUSED(steps);
}

void TrackerWidget::setPatternChannels(int channels) {
    // This would require resizing all patterns - implementation left for later
    Q_UNUSED(channels);
}

// Private slots
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
    
    // Update display
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

void TrackerWidget::onPlaybackTimer() {
    if (!playing_ || paused_) {
        return;
    }
    
    // Trigger current step
    triggerStep(currentStep_);
    
    // Move to next step
    currentStep_++;
    if (currentPattern_ < static_cast<int>(patterns_.size())) {
        TrackerPattern& pattern = *patterns_[currentPattern_];
        if (currentStep_ >= pattern.getSteps()) {
            currentStep_ = 0; // Loop
        }
    }
    
    updatePlaybackPosition();
}
