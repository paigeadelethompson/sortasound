#include "mainwindow.hpp"
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGridLayout>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget_(nullptr)
    , mainLayout_(nullptr)
    , controlsLayout_(nullptr)
    , synthesizer_(std::make_unique<toybasic::FMSynthesizer>())
    , presetManager_(std::make_unique<toybasic::PresetManager>())
    , keyboardWidget_(nullptr)
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
    , operatorGroup_(nullptr)
    , operatorTabs_(nullptr)
    , modulationGroup_(nullptr)
    , pitchBendSlider_(nullptr)
    , modWheelSlider_(nullptr)
    , pitchBendLabel_(nullptr)
    , modWheelLabel_(nullptr)
    , channelGroup_(nullptr)
    , channelCombo_(nullptr)
    , currentChannel_(0)
{
    setupUI();
    setupKeyboardMapping();
    
    // Start audio synthesizer
    synthesizer_->startAudioThread();
}

MainWindow::~MainWindow()
{
    synthesizer_->stopAudioThread();
}

void MainWindow::setupUI()
{
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);
    
    mainLayout_ = new QVBoxLayout(centralWidget_);
    
    // Create keyboard widget
    keyboardWidget_ = new KeyboardWidget(this);
    keyboardWidget_->setMinimumHeight(120);
    mainLayout_->addWidget(keyboardWidget_);
    
    // Create controls layout
    controlsLayout_ = new QHBoxLayout();
    
    // Volume control
    volumeGroup_ = new QGroupBox("Volume", this);
    QVBoxLayout *volumeLayout = new QVBoxLayout(volumeGroup_);
    
    volumeSlider_ = new QSlider(Qt::Horizontal, this);
    volumeSlider_->setRange(0, 100);
    volumeSlider_->setValue(50);
    volumeSlider_->setTickPosition(QSlider::TicksBelow);
    volumeSlider_->setTickInterval(10);
    connect(volumeSlider_, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    
    volumeLabel_ = new QLabel("50%", this);
    volumeLabel_->setAlignment(Qt::AlignCenter);
    
    volumeLayout->addWidget(volumeSlider_);
    volumeLayout->addWidget(volumeLabel_);
    controlsLayout_->addWidget(volumeGroup_);
    
    // Preset selection
    presetGroup_ = new QGroupBox("Preset", this);
    QVBoxLayout *presetLayout = new QVBoxLayout(presetGroup_);
    
    presetCombo_ = new QComboBox(this);
    
    // Add all presets from the preset manager
    auto presetNames = presetManager_->getPresetNames();
    for (const auto& presetName : presetNames) {
        presetCombo_->addItem(QString::fromStdString(presetName));
    }
    
    connect(presetCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPresetChanged);
    
    presetLayout->addWidget(presetCombo_);
    controlsLayout_->addWidget(presetGroup_);
    
    // Effects controls
    effectsGroup_ = new QGroupBox("Effects", this);
    QGridLayout *effectsLayout = new QGridLayout(effectsGroup_);
    
    // Reverb
    reverbSlider_ = new QSlider(Qt::Horizontal, this);
    reverbSlider_->setRange(0, 100);
    reverbSlider_->setValue(0);
    reverbSlider_->setTickPosition(QSlider::TicksBelow);
    reverbSlider_->setTickInterval(10);
    connect(reverbSlider_, &QSlider::valueChanged, this, &MainWindow::onReverbChanged);
    
    reverbLabel_ = new QLabel("0%", this);
    reverbLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Reverb:", this), 0, 0);
    effectsLayout->addWidget(reverbSlider_, 0, 1);
    effectsLayout->addWidget(reverbLabel_, 0, 2);
    
    // Chorus
    chorusSlider_ = new QSlider(Qt::Horizontal, this);
    chorusSlider_->setRange(0, 100);
    chorusSlider_->setValue(0);
    chorusSlider_->setTickPosition(QSlider::TicksBelow);
    chorusSlider_->setTickInterval(10);
    connect(chorusSlider_, &QSlider::valueChanged, this, &MainWindow::onChorusChanged);
    
    chorusLabel_ = new QLabel("0%", this);
    chorusLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Chorus:", this), 1, 0);
    effectsLayout->addWidget(chorusSlider_, 1, 1);
    effectsLayout->addWidget(chorusLabel_, 1, 2);
    
    // Distortion
    distortionSlider_ = new QSlider(Qt::Horizontal, this);
    distortionSlider_->setRange(0, 100);
    distortionSlider_->setValue(0);
    distortionSlider_->setTickPosition(QSlider::TicksBelow);
    distortionSlider_->setTickInterval(10);
    connect(distortionSlider_, &QSlider::valueChanged, this, &MainWindow::onDistortionChanged);
    
    distortionLabel_ = new QLabel("0%", this);
    distortionLabel_->setAlignment(Qt::AlignCenter);
    
    effectsLayout->addWidget(new QLabel("Distortion:", this), 2, 0);
    effectsLayout->addWidget(distortionSlider_, 2, 1);
    effectsLayout->addWidget(distortionLabel_, 2, 2);
    
    controlsLayout_->addWidget(effectsGroup_);
    
    // Algorithm selection
    algorithmGroup_ = new QGroupBox("Algorithm", this);
    QVBoxLayout *algorithmLayout = new QVBoxLayout(algorithmGroup_);
    
    algorithmCombo_ = new QComboBox(this);
    // Add all 32 algorithms (0-31)
    algorithmCombo_->addItem("0: 6→5→4→3→2→1 (Serial Chain)");
    algorithmCombo_->addItem("1: 5,6→4→3→2→1 (Parallel Modulators)");
    algorithmCombo_->addItem("2: 6→5→4→3→2, 6→1 (Serial + Parallel)");
    algorithmCombo_->addItem("3: 6→5→4→3, 6→2→1 (Two Serial Chains)");
    algorithmCombo_->addItem("4: 6→5→4, 6→3→2→1 (Mixed Chains)");
    algorithmCombo_->addItem("5: 6→5, 6→4→3→2→1 (Two Serial Chains)");
    algorithmCombo_->addItem("6: 6→5→4, 6→3, 6→2→1 (Three Chains)");
    algorithmCombo_->addItem("7: 6→5, 6→4, 6→3, 6→2→1 (Four Chains)");
    algorithmCombo_->addItem("8: 6→5→4, 6→3, 6→2, 6→1 (Four Parallel)");
    algorithmCombo_->addItem("9: 6→5, 6→4→3, 6→2→1 (Three Parallel)");
    algorithmCombo_->addItem("10: 6→5→4→3, 6→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("11: 6→5→4, 6→3→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("12: 6→5, 6→4, 6→3→2→1 (Two + Serial)");
    algorithmCombo_->addItem("13: 6→5→4→3→2, 6→1 (Two Parallel)");
    algorithmCombo_->addItem("14: 6→5→4→3, 6→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("15: 6→5→4, 6→3, 6→2, 6→1 (Four Parallel)");
    algorithmCombo_->addItem("16: 6→5, 6→4→3→2→1 (Two Parallel)");
    algorithmCombo_->addItem("17: 6→5→4→3→2→1 (Single Serial)");
    algorithmCombo_->addItem("18: 6→5→4→3, 6→2→1 (Two Parallel)");
    algorithmCombo_->addItem("19: 6→5→4, 6→3→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("20: 6→5, 6→4→3→2→1 (Two Parallel)");
    algorithmCombo_->addItem("21: 6→5→4→3→2, 6→1 (Two Parallel)");
    algorithmCombo_->addItem("22: 6→5→4→3, 6→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("23: 6→5→4, 6→3, 6→2, 6→1 (Four Parallel)");
    algorithmCombo_->addItem("24: 6→5, 6→4→3→2→1 (Two Parallel)");
    algorithmCombo_->addItem("25: 6→5→4→3→2→1 (Single Serial)");
    algorithmCombo_->addItem("26: 6→5→4→3, 6→2→1 (Two Parallel)");
    algorithmCombo_->addItem("27: 6→5→4, 6→3→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("28: 6→5, 6→4→3→2→1 (Two Parallel)");
    algorithmCombo_->addItem("29: 6→5→4→3→2, 6→1 (Two Parallel)");
    algorithmCombo_->addItem("30: 6→5→4→3, 6→2, 6→1 (Three Parallel)");
    algorithmCombo_->addItem("31: 6→5→4, 6→3, 6→2, 6→1 (Four Parallel)");
    
    connect(algorithmCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);
    
    algorithmLayout->addWidget(algorithmCombo_);
    controlsLayout_->addWidget(algorithmGroup_);
    
    // Channel selection
    channelGroup_ = new QGroupBox("Channel", this);
    QVBoxLayout *channelLayout = new QVBoxLayout(channelGroup_);
    
    channelCombo_ = new QComboBox(this);
    for (int i = 0; i < 8; i++) {
        channelCombo_->addItem(QString("Channel %1").arg(i + 1));
    }
    
    connect(channelCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onChannelChanged);
    
    channelLayout->addWidget(channelCombo_);
    controlsLayout_->addWidget(channelGroup_);
    
    mainLayout_->addLayout(controlsLayout_);
    
    // Advanced controls layout
    QHBoxLayout *advancedLayout = new QHBoxLayout();
    
    // Operator controls
    operatorGroup_ = new QGroupBox("Operators", this);
    QVBoxLayout *operatorLayout = new QVBoxLayout(operatorGroup_);
    
    operatorTabs_ = new QTabWidget(this);
    setupOperatorTabs();
    operatorLayout->addWidget(operatorTabs_);
    advancedLayout->addWidget(operatorGroup_);
    
    // Modulation controls
    modulationGroup_ = new QGroupBox("Modulation", this);
    QVBoxLayout *modulationLayout = new QVBoxLayout(modulationGroup_);
    
    // Pitch bend
    pitchBendSlider_ = new QSlider(Qt::Horizontal, this);
    pitchBendSlider_->setRange(-200, 200); // -2 to +2 semitones
    pitchBendSlider_->setValue(0);
    pitchBendSlider_->setTickPosition(QSlider::TicksBelow);
    pitchBendSlider_->setTickInterval(50);
    connect(pitchBendSlider_, &QSlider::valueChanged, this, &MainWindow::onPitchBendChanged);
    
    pitchBendLabel_ = new QLabel("0", this);
    pitchBendLabel_->setAlignment(Qt::AlignCenter);
    
    modulationLayout->addWidget(new QLabel("Pitch Bend:", this));
    modulationLayout->addWidget(pitchBendSlider_);
    modulationLayout->addWidget(pitchBendLabel_);
    
    // Modulation wheel
    modWheelSlider_ = new QSlider(Qt::Horizontal, this);
    modWheelSlider_->setRange(0, 127);
    modWheelSlider_->setValue(0);
    modWheelSlider_->setTickPosition(QSlider::TicksBelow);
    modWheelSlider_->setTickInterval(20);
    connect(modWheelSlider_, &QSlider::valueChanged, this, &MainWindow::onModWheelChanged);
    
    modWheelLabel_ = new QLabel("0", this);
    modWheelLabel_->setAlignment(Qt::AlignCenter);
    
    modulationLayout->addWidget(new QLabel("Mod Wheel:", this));
    modulationLayout->addWidget(modWheelSlider_);
    modulationLayout->addWidget(modWheelLabel_);
    
    advancedLayout->addWidget(modulationGroup_);
    
    mainLayout_->addLayout(advancedLayout);
    
    // Set window properties
    setWindowTitle("FM Synthesizer - Advanced 4-Operator FM Synthesizer");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    setFocusPolicy(Qt::StrongFocus); // Ensure we can receive key events
}

void MainWindow::setupKeyboardMapping()
{
    // Part 1 of octave: q to ]
    keyToNoteMap_[Qt::Key_Q] = OCTAVE_START;      // C
    keyToNoteMap_[Qt::Key_W] = OCTAVE_START + 1;  // C#
    keyToNoteMap_[Qt::Key_E] = OCTAVE_START + 2;  // D
    keyToNoteMap_[Qt::Key_R] = OCTAVE_START + 3;  // D#
    keyToNoteMap_[Qt::Key_T] = OCTAVE_START + 4;  // E
    keyToNoteMap_[Qt::Key_Y] = OCTAVE_START + 5;  // F
    keyToNoteMap_[Qt::Key_U] = OCTAVE_START + 6;  // F#
    keyToNoteMap_[Qt::Key_I] = OCTAVE_START + 7;  // G
    keyToNoteMap_[Qt::Key_O] = OCTAVE_START + 8;  // G#
    keyToNoteMap_[Qt::Key_P] = OCTAVE_START + 9;  // A
    keyToNoteMap_[Qt::Key_BracketLeft] = OCTAVE_START + 10;  // A#
    keyToNoteMap_[Qt::Key_BracketRight] = OCTAVE_START + 11; // B
    
    // Part 2 of octave: a to '
    keyToNoteMap_[Qt::Key_A] = OCTAVE_START + 12; // C (next octave)
    keyToNoteMap_[Qt::Key_S] = OCTAVE_START + 13; // C#
    keyToNoteMap_[Qt::Key_D] = OCTAVE_START + 14; // D
    keyToNoteMap_[Qt::Key_F] = OCTAVE_START + 15; // D#
    keyToNoteMap_[Qt::Key_G] = OCTAVE_START + 16; // E
    keyToNoteMap_[Qt::Key_H] = OCTAVE_START + 17; // F
    keyToNoteMap_[Qt::Key_J] = OCTAVE_START + 18; // F#
    keyToNoteMap_[Qt::Key_K] = OCTAVE_START + 19; // G
    keyToNoteMap_[Qt::Key_L] = OCTAVE_START + 20; // G#
    keyToNoteMap_[Qt::Key_Semicolon] = OCTAVE_START + 21; // A
    keyToNoteMap_[Qt::Key_QuoteLeft] = OCTAVE_START + 22; // A#
    keyToNoteMap_[Qt::Key_QuoteDbl] = OCTAVE_START + 23;  // B
    
    // Part 3 of octave: z to /
    keyToNoteMap_[Qt::Key_Z] = OCTAVE_START + 24; // C (next octave)
    keyToNoteMap_[Qt::Key_X] = OCTAVE_START + 25; // C#
    keyToNoteMap_[Qt::Key_C] = OCTAVE_START + 26; // D
    keyToNoteMap_[Qt::Key_V] = OCTAVE_START + 27; // D#
    keyToNoteMap_[Qt::Key_B] = OCTAVE_START + 28; // E
    keyToNoteMap_[Qt::Key_N] = OCTAVE_START + 29; // F
    keyToNoteMap_[Qt::Key_M] = OCTAVE_START + 30; // F#
    keyToNoteMap_[Qt::Key_Comma] = OCTAVE_START + 31; // G
    keyToNoteMap_[Qt::Key_Period] = OCTAVE_START + 32; // G#
    keyToNoteMap_[Qt::Key_Slash] = OCTAVE_START + 33; // A
    
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
        synthesizer_->noteOn(note);
        keyboardWidget_->setActiveNotes(activeNotes_);
        qDebug() << "MainWindow::noteOn - note:" << note << "activeNotes count:" << activeNotes_.size();
    }
}

void MainWindow::noteOff(int note)
{
    auto it = activeNotes_.find(note);
    if (it != activeNotes_.end()) {
        activeNotes_.erase(it);
        synthesizer_->noteOff(note);
        keyboardWidget_->setActiveNotes(activeNotes_);
        qDebug() << "MainWindow::noteOff - note:" << note << "activeNotes count:" << activeNotes_.size();
    }
}

void MainWindow::allNotesOff()
{
    activeNotes_.clear();
    synthesizer_->allNotesOff();
    keyboardWidget_->setActiveNotes(activeNotes_);
    qDebug() << "MainWindow::allNotesOff - cleared all notes";
}

void MainWindow::onVolumeChanged(int value)
{
    double volume = value / 100.0;
    synthesizer_->setMasterVolume(volume);
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
    presetManager_->applyPreset(*synthesizer_, currentChannel_, index);
    
    // Only reapply non-preset settings (pitch bend and modulation wheel)
    // DO NOT overwrite preset values for volume, reverb, chorus, distortion, or algorithm
    synthesizer_->setPitchBend(currentChannel_, pitchBendSlider_->value() / 200.0 + 1.0);
    synthesizer_->setModulationWheel(currentChannel_, modWheelSlider_->value() / 127.0);
    
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
    synthesizer_->setReverb(amount);
    reverbLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onChorusChanged(int value)
{
    double amount = value / 100.0;
    synthesizer_->setChorus(amount);
    chorusLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onDistortionChanged(int value)
{
    double amount = value / 100.0;
    synthesizer_->setDistortion(amount);
    distortionLabel_->setText(QString("%1%").arg(value));
}

void MainWindow::onAlgorithmChanged(int index)
{
    synthesizer_->setAlgorithm(currentChannel_, index);
}

void MainWindow::onChannelChanged(int index)
{
    currentChannel_ = index;
    // Set the current channel as active
    synthesizer_->setChannelActive(currentChannel_, true);
}

void MainWindow::onPitchBendChanged(int value)
{
    double bend = 1.0 + (value / 200.0); // Convert to 0.5 to 1.5 range
    synthesizer_->setPitchBend(currentChannel_, bend);
    pitchBendLabel_->setText(QString("%1").arg(value));
}

void MainWindow::onModWheelChanged(int value)
{
    double mod = value / 127.0; // Convert to 0.0 to 1.0 range
    synthesizer_->setModulationWheel(currentChannel_, mod);
    modWheelLabel_->setText(QString("%1").arg(value));
}

void MainWindow::onOperatorParameterChanged()
{
    // This method is no longer used with the new voice-based system
    // Operator parameters are now handled through presets
}

void MainWindow::setupOperatorTabs()
{
    // Simplified operator controls - the new backend handles operator configuration
    // through presets, so we'll just show basic information
    
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    
    // Add information about the new system
    QLabel *infoLabel = new QLabel(
        "FM Synthesizer with 6 Operators\n\n"
        "The synthesizer now uses a voice-based system where:\n"
        "• Each note gets its own voice with 6 operators\n"
        "• Operator settings are configured through presets\n"
        "• 32 different algorithms control operator routing\n"
        "• Real-time modulation via pitch bend and mod wheel\n\n"
        "Use the Preset selector to choose different sounds.\n"
        "Use the Algorithm selector to change operator routing.",
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignTop);
    layout->addWidget(infoLabel);
    
    // Add some basic controls that still work
    QGroupBox *basicGroup = new QGroupBox("Basic Controls", this);
    QVBoxLayout *basicLayout = new QVBoxLayout(basicGroup);
    
    // Voice count display (read-only)
    QLabel *voiceLabel = new QLabel("Max Voices: 16", this);
    basicLayout->addWidget(voiceLabel);
    
    // Sample rate display (read-only)
    QLabel *sampleRateLabel = new QLabel("Sample Rate: 44.1 kHz", this);
    basicLayout->addWidget(sampleRateLabel);
    
    // Audio format display (read-only)
    QLabel *formatLabel = new QLabel("Audio Format: 14-bit", this);
    basicLayout->addWidget(formatLabel);
    
    layout->addWidget(basicGroup);
    
    operatorPages_.push_back(page);
    operatorTabs_->addTab(page, "Info");
}
